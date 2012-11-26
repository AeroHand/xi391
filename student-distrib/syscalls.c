/*************************************************/
/* syscalls.c - The system call implementations. */
/*************************************************/

#include "syscalls.h"
#include "interrupthandler.h"
#include "keyboard.h"
#include "rtc.h"
#include "files.h"


uint8_t running_processes = 0x80;
uint32_t kernel_stack_bottom;

/* used in halt for switching the page directory */
uint32_t page_dir_addr;

typedef struct file_descriptor_t {
	uint32_t * jumptable;
	int32_t inode;
	int32_t fileposition;
	int32_t flags;
} file_descriptor_t;


typedef struct pcb_t {
	file_descriptor_t fds[8];
	uint8_t filenames[8][32]; 
	uint32_t parent_ksp;
	uint32_t parent_kbp;
	uint8_t process_number;
	uint8_t parent_process_number;
	uint8_t argbuf[100];
} pcb_t;

int rtc_in_use;


/* Initialize the file operations tables -- we will make the
 * file descriptors' jumptable pointers point to these tables when
 * we open a file.
 */
 /* stdin file operations table */
uint32_t stdin_fops_table[4] = { (uint32_t)(no_function),
								 (uint32_t)(terminal_read),
								 (uint32_t)(no_function),
								 (uint32_t)(no_function) };
/* stdout file operations table */
uint32_t stdout_fops_table[4] = { (uint32_t)(no_function),
								  (uint32_t)(no_function),
								  (uint32_t)(terminal_write),
								  (uint32_t)(no_function) };
/* rtc file operations table */
uint32_t rtc_fops_table[4] = { (uint32_t)(rtc_open),
							   (uint32_t)(rtc_read),
							   (uint32_t)(rtc_write),
							   (uint32_t)(rtc_close) };
/* file file operations table */
uint32_t file_fops_table[4] = { (uint32_t)(file_open),
							    (uint32_t)(file_read),
							    (uint32_t)(file_write),
							    (uint32_t)(file_close) };
/* directory file operations table */
uint32_t dir_fops_table[4] = { (uint32_t)(dir_open),
							   (uint32_t)(dir_read),
							   (uint32_t)(dir_write),
							   (uint32_t)(dir_close) };
			   
/*
 * halt()
 *
 * Terminates a process, returning the specified value to its parent process.
 *
 * Retvals
 * 
 */
int32_t halt(uint8_t status)
{
	/* Local variables */
	int i;
	printf("\nYou syscalled a \"halt\". The status is: %d\n",status);
	
	/* Extract the PCB from the KSP */
	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);
	
	/* Mark the current process as 0, aka this process is done and its slot
	 * is now available for new processes
	 */
	uint8_t bitmask = 0x7F;
	for( i = 0; i < process_control_block->process_number; i++ )
	{
		bitmask = (bitmask >> 1) + 0x80;
	}
	running_processes = running_processes & bitmask;
	
	/* Load the page directory of the parent */
	page_dir_addr = (uint32_t)(&page_directories[process_control_block->parent_process_number]);
	asm (
	"movl page_dir_addr, %%eax                   ;"
	"andl $0xFFFFFFE7, %%eax          ;"
	"movl %%eax, %%cr3                ;"
	"movl %%cr4, %%eax                ;"
	"orl $0x00000090, %%eax           ;"
	"movl %%eax, %%cr4                ;"
	"movl %%cr0, %%eax                ;"
	"orl $0x80000000, %%eax 	      ;"
	"movl %%eax, %%cr0                 "
	: : : "eax", "cc" );
	
	/* Set the kernel_stack_bottom and the TSS to point back at the parent's kernel stack */
	kernel_stack_bottom = tss.esp0 = 0x00800000 - (0x2000)*process_control_block->parent_process_number - 4;
	
	/* Switch the kernel stack back to the parent's kernel stack by
	 * restoring ESP and EBP.
	 * -- NOTE: We need to store "status" since we will be losing it when we
	 *          switch the stack. To do this, move ESP to the new stack,
	 *          push status (which is referenced by EBP), move EBP to the new
	 *          stack, then pop status back into EAX for the return of halt.
	 */
	asm volatile("movl %0, %%esp	;"
				 "pushl %1			;"::"g"(process_control_block->parent_ksp),"g"(status)); // Asm stuff put the "parent_ksp" into the %ESP
	asm volatile("movl %0, %%ebp"::"g"(process_control_block->parent_kbp)); // Asm stuff put the "parent_kbp" into the %EBP
	asm volatile("popl %eax");
	
	/* We have now switched back to the stack of the parent. Remember that the parent
	 * stack was where we originally called 'execute' for this process. Thus, the parent's
	 * stack (now the current stack) contains the appropriate data that was pushed when 
	 * we made the 'execute' syscall. If we now leave and ret, we will return back to the
	 * syscall_handler for the original 'execute' syscall, which can then iret.
	 * -- NOTE: We never iret from the 'halt' command... we just ditch its stack information
	 *          when we switch back to the parent stack.
	 */
	asm volatile("leave");
	asm volatile("ret");
	
	return 0;
}

/*
 * execute()
 *
 * Attempts to load and execute a new program, handing off the processor to the
 * new program until it terminates.
 *
 * Retvals
 * 
 */
int32_t execute(const uint8_t* command)
{
	/* Local variables. */
	uint8_t fname[32];
	uint8_t buf[4];
	uint32_t i;
	uint32_t entry_point;
	uint8_t magic_nums[4] = {0x7f, 0x45, 0x4c, 0x46};
	uint8_t open_process;
	uint32_t first_space_reached = 0;
	uint32_t length_of_fname = 0;
	uint8_t localargbuf[100];
	
	/* Initializations. */
	entry_point = 0;
	
	/* Check for an invalid command. */
	if( command == NULL )
	{
		return -1;
	}
	
	/* Look for an open slot for the process */
	uint8_t bitmask = 0x80;
	for( i = 0; i < 8; i++ ) {
		if( !(running_processes & bitmask) ) 
		{
			open_process = i;
			running_processes = running_processes | bitmask;
			break;
		}
		bitmask = (bitmask >> 1);
		if( bitmask == 0 )
		{
			return -1;
		}
	}
	
	/* Get the file name of the program to be executed and store */
	/* the additional args into the argbuf.                      */
	for( i = 0; command[i] != '\0' ; i++ )
	{
		if( command[i] == ' ' && first_space_reached == 0 )
		{
			first_space_reached = 1;
			length_of_fname = i;
			fname[i] = '\0';
		}
		else if( first_space_reached == 1 )
		{
			localargbuf[i-length_of_fname-1] = command[i];
		}
		else
		{
			fname[i] = command[i];
		}
	}
	localargbuf[i-length_of_fname-1] = '\0';
	
	if( first_space_reached == 0 )
	{
		fname[i] = '\0';
	}
	
	/* Read the identifying 4 bytes from the file into buf. */
	if( -1 == fs_read((const int8_t *)fname, 0, buf, 4) )
	{
		return -1;
	}
	
	/* Ensure an executable program image. */
	if( 0 != strncmp((const int8_t*)buf, (const int8_t*)magic_nums, 4) )
	{
		return -1;
	}
	
	/* Get the entry point to the program. */
	if( -1 == fs_read((const int8_t *)fname, 24, buf, 4) )
	{
		return -1;
	}
	
	/* Save the entry point. */
	for( i = 0; i < 4; i++ )
	{
		entry_point |= (buf[i] << 8*i);
	}
	
	/* Set up the new page directory for the new task. */
	if( -1 == setup_new_task( open_process ) )
	{
		return -1;
	}
	
	/* Load the program to the appropriate starting address. */
	fs_load((const int8_t *)fname, 0x08048000);
	
	pcb_t * process_control_block = (pcb_t *)( 0x00800000 - (0x2000)*(open_process + 1) );
	
	uint32_t esp;
	asm volatile("movl %%esp, %0":"=g"(esp)); // Asm stuff to get the %ESP into the C-variable "esp"
	process_control_block->parent_ksp = esp;
	
	uint32_t ebp;
	asm volatile("movl %%ebp, %0":"=g"(ebp)); // Asm stuff to get the %ESP into the C-variable "ebp"
	process_control_block->parent_kbp = ebp;
	
	if( running_processes == 0xC0 )
	{
		/* if this process was the first process called (aka, it was called from "no processes running" process),
		 * make the parent process number 0 -- NOTE: we must do it like this because the "no processes running"
		 * process does not have a PCB
		 */
		process_control_block->parent_process_number = 0;
	}
	else
	{
		/* set the parent_process_number of the new process to be the process number of the current
		 * process that called it (find this in the PCB)
		 */
		process_control_block->parent_process_number = ( (pcb_t *)(esp & 0xFFFFE000) )->process_number;
	}
	process_control_block->process_number = open_process;
	
	for( i = 0; i < 8; i++ )
	{
		process_control_block->fds[i].inode = 0;
		process_control_block->fds[i].fileposition = 0;
		process_control_block->fds[i].flags = NOT_IN_USE;
	}
	
	strcpy((int8_t*)process_control_block->argbuf, (const int8_t*)localargbuf);
	
	kernel_stack_bottom = tss.esp0 = 0x00800000 - (0x2000)*open_process - 4;
	
	open_stdin( 0 );
	open_stdout( 1 );
	
	asm volatile("movl %0, %%esp	;"
				 "pushl %1			;"::"g"(kernel_stack_bottom), "g"(entry_point)); // Asm stuff put the "kernel_stack_bottom" into the %ESP
	asm volatile("movl %0, %%ebp"::"g"(kernel_stack_bottom)); // Asm stuff put the "kernel_stack_bottom" into the %EBP

	int32_t entry;
	asm volatile("popl %0			;":"=g"(entry));
	
	/* Jump to the entry point and begin execution. */
	to_the_user_space(entry);

	return 0;
}

void execute_test(void)
{
	const uint8_t * test_string = "shell";
	execute(test_string);
}

/*
 * read()
 *
 * Reads 'nbytes' bytes into 'buf' from the file corresponding to the
 * given 'fd'.
 *
 * Retvals
 * 
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes)
{

	int bytesread;
	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);
	
	//Check for invalid fd or buf. 
	if( fd < 0 || fd > 7 || buf == NULL || process_control_block->fds[fd].flags == NOT_IN_USE )
	{
		return -1;
	}

	uint8_t* filename = process_control_block->filenames[fd];
	uint32_t fileposition = process_control_block->fds[fd].fileposition;

	asm volatile("pushl %0		;"
				 "pushl %1		;"
				 "pushl %2		;"
				 "pushl %3		;"
				 "call  %4		;"
				 :
				 : "g" (fileposition), "g" ((int32_t)filename), "g" (nbytes), "g" ((int32_t)buf),
				   "g" (process_control_block->fds[fd].jumptable[1]));
				 
	asm volatile("movl %%eax, %0":"=g"(bytesread));
	asm volatile("addl $16, %esp	;");
	
	process_control_block->fds[fd].fileposition += bytesread;
	
	return bytesread;



/*	dentry_t dentry;
	
	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);
	
	//Check for invalid fd or buf. 
	if( fd < 0 || fd > 7 || buf == NULL )
	{
		return -1;
	}
	
	if( -1 == read_dentry_by_name(process_control_block->filenames[fd], &dentry)) {
		return -1;
	}
	
	// Stdin. 
	if( fd == 0 )
	{
		terminal_read(buf,nbytes);
		return nbytes;
	}
	
	// 'Read' on stdout does nothing.
	if( fd == 1 )
	{
		return -1;
	}
	
	// Call read for the file with the given fd.
	if( dentry.filetype == 0 ) // rtc
	{
		//( (void (*)(void))(process_control_block->fds[fd].jumptable[1]) )(void);
		return 0;
	}
	else if( dentry.filetype == 1 ) // dir
	{
		//( (void (*)(uint8_t*))(process_control_block->fds[fd].jumptable[1]) )((uint8_t *)buf);
		return 0;
	}
	else if( dentry.filetype == 2 ) // reg file
	{
		//( (void (*)(const int8_t*,uint8_t*,uint32_t))(process_control_block->fds[fd].jumptable[1]) )
		//((const int8_t*)process_control_block->filenames[fd], (uint8_t*)buf, (uint32_t)nbytes);
		return 0;
	}

	return -1;
	*/
}

/*
 * write()
 *
 * Writes 'nbytes' bytes from 'buf' into the file associated with 'fd'.
 *
 * Retvals
 * 
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	int byteswritten;
	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);
	
	//Check for invalid fd or buf. 
	if( fd < 0 || fd > 7 || buf == NULL || process_control_block->fds[fd].flags == NOT_IN_USE )
	{
		return -1;
	}

	asm volatile("pushl %0		;"
				 "pushl %1		;"
				 "call  %2		;"
				 :
				 : "g" (nbytes), "g" ((int32_t)buf), "g" (process_control_block->fds[fd].jumptable[2]));
				 
	asm volatile("movl %%eax, %0":"=g"(byteswritten));
	asm volatile("addl $8, %esp	;");
	
	return byteswritten;
}

/*
 * open()
 *
 * Attempts to open the file with the given filename and give it a spot
 * in the file array in the pcb associated with the current process.
 *
 * Retvals
 * 
 */
int32_t open(const uint8_t* filename)
{
	int i;

	dentry_t tempdentry;

	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);

	/* Call appropriate function for opening stdin. */
	if( 0 == strncmp((const int8_t*)filename, (const int8_t*)"stdin", 5) ) 
	{
		open_stdin( 0 );
		return 0;
	}
	
	/* Call appropriate function for opening stdout. */
	if( 0 == strncmp((const int8_t*)filename, (const int8_t*)"stdout", 5) ) 
	{
		open_stdout( 1 );
		return 0;
	}
	
	/* Get dentry information associated with the filename. */
	if( -1 == read_dentry_by_name(filename, &tempdentry)) {
		return -1;
	}

	for (i=2; i<8; i++) {
		if (process_control_block->fds[i].flags == NOT_IN_USE) {	
			
			if (tempdentry.filetype == 0) //RTC
			{ 		
				if (-1 == rtc_open()) {
					return -1;
				} else {
					//process_control_block->fds[i].jumptable[0] = (uint32_t)(rtc_open);
					//process_control_block->fds[i].jumptable[1] = (uint32_t)(rtc_read);
					//process_control_block->fds[i].jumptable[2] = (uint32_t)(rtc_write);
					//process_control_block->fds[i].jumptable[3] = (uint32_t)(rtc_close);
					process_control_block->fds[i].jumptable = rtc_fops_table;
				}
			}
			else if(tempdentry.filetype == 1) //Directory
			{ 
				//process_control_block->fds[i].jumptable[0] = (uint32_t)(dir_open);
				//process_control_block->fds[i].jumptable[1] = (uint32_t)(dir_read);
				//process_control_block->fds[i].jumptable[2] = (uint32_t)(dir_write);
				//process_control_block->fds[i].jumptable[3] = (uint32_t)(dir_close);
				process_control_block->fds[i].jumptable = dir_fops_table;
			}
			else if(tempdentry.filetype == 2) //Regular File
			{ 
				//process_control_block->fds[i].jumptable[0] = (uint32_t)(file_open);
				//process_control_block->fds[i].jumptable[1] = (uint32_t)(file_read);
				//process_control_block->fds[i].jumptable[2] = (uint32_t)(file_write);
				//process_control_block->fds[i].jumptable[3] = (uint32_t)(file_close);
				process_control_block->fds[i].jumptable = file_fops_table;
			}

			process_control_block->fds[i].flags = IN_USE;
			process_control_block->fds[i].inode = tempdentry.inode;
			strcpy((int8_t*)process_control_block->filenames[i], (const int8_t*)filename);
			return i;
		}		
	}

	printf("The File Descriptor Array is Filled\n");
	return -1;	
}

void open_stdin( int32_t fd )
{
	/* get the PCB by using the KSP */
	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);
	
	/* set the jumptable -- NOTE: for stdin, we only have a read function */
	//process_control_block->fds[fd].jumptable[0] = (uint32_t)(no_function);
	//process_control_block->fds[fd].jumptable[1] = (uint32_t)(terminal_read);
	//process_control_block->fds[fd].jumptable[2] = (uint32_t)(no_function);
	//process_control_block->fds[fd].jumptable[3] = (uint32_t)(no_function);
	process_control_block->fds[fd].jumptable = stdin_fops_table;
	
	/* mark this fd as in use */
	process_control_block->fds[fd].flags = IN_USE;
}

void open_stdout( int32_t fd )
{
	/* get the PCB by using the KSP */
	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);
	
	/* set the jumptable -- NOTE: for stdout, we only have a write function */
	//process_control_block->fds[fd].jumptable[0] = (uint32_t)(no_function);
	//process_control_block->fds[fd].jumptable[1] = (uint32_t)(no_function);
	//process_control_block->fds[fd].jumptable[2] = (uint32_t)(terminal_write);
	//process_control_block->fds[fd].jumptable[3] = (uint32_t)(no_function);
	process_control_block->fds[fd].jumptable = stdout_fops_table;
	
	/* mark this fd as in use */
	process_control_block->fds[fd].flags = IN_USE;
}

//( (void (*)(char*,int))(jumptable[1]) )(parameter1, parameter2);

int32_t close(int32_t fd)
{
	uint32_t retval;
	
	/* get the PCB by using the KSP */
	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);
	
	//Check for invalid fd. 
	if( fd < 2 || fd > 7 || process_control_block->fds[fd].flags == NOT_IN_USE )
	{
		return -1;
	}
	
	asm volatile("call  %0		;"
				 :
				 : "g" (process_control_block->fds[fd].jumptable[3]));
	asm volatile("movl %%eax, %0":"=g"(retval));
	
	process_control_block->fds[fd].jumptable = NULL;
	process_control_block->fds[fd].inode = 0;
	process_control_block->fds[fd].fileposition = 0;
	process_control_block->fds[fd].flags = NOT_IN_USE;
	
	return retval;
}

int32_t getargs(uint8_t* buf, int32_t nbytes)
{
	if( buf == NULL || nbytes == 0 )
	{
		return -1;
	}
	
	/* get the PCB by using the KSP */
	pcb_t * process_control_block = (pcb_t *)(kernel_stack_bottom & 0xFFFFE000);
	
	if( strlen((const int8_t*)process_control_block->argbuf) > nbytes )
	{
		return -1;
	}
	
	strcpy((int8_t*)buf, (const int8_t*)process_control_block->argbuf);
	
	return 0;
}

int32_t vidmap(uint8_t** screen_start)
{
	return 0;
}

int32_t set_handler(int32_t signum, void* handler_address)
{
	return 0;
}

int32_t sigreturn(void)
{
	return 0;
}

int32_t no_function(void)
{
	return 0;
}
