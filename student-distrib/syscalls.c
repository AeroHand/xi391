/*************************************************/
/* syscalls.c - The system call implementations. */
/*************************************************/

#include "syscalls.h"
#include "interrupthandler.h"
#include "keyboard.h"
#include "rtc.h"
#include "files.h"


typedef struct file_descriptor {
	file_op_table* jumptable;
	dentry_t* inodepointer;
	int32_t fileposition;
	int32_t flags;
} file_descriptor;

int rtc_in_use;


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
	printf("\nYou syscalled a \"halt\". The status is: %d\n",status);
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
	
	/* Initializations. */
	entry_point = 0;
	
	/* Check for an invalid command. */
	if( command == NULL )
	{
		return -1;
	}
	
	/* Get the file name of the program to be executed. */
	for( i = 0; command[i] != '\0' ; i++ )
	{
		if( i >= 32 )
		{
			return -1;
		}
		fname[i] = command[i];
	}

	fname[i] = '\0';
	
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
	if( -1 == setup_new_task() )
	{
		return -1;
	}
	
	/* Load the program to the appropriate starting address. */
	fs_load((const int8_t *)fname, 0x08048000);
	//printf("About to jump to user space...\n");
	
	/* Jump to the entry point and begin execution. */
	to_the_user_space((int32_t)entry_point, 0x083FFFF0);
	
	return 0;
}

void execute_test(void)
{
	const uint8_t * test_string = "shell";
	execute(test_string);
}


int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	terminal_read(buf,nbytes);
	return nbytes;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	int byteswritten = terminal_write(buf,nbytes);
	return byteswritten;
}

int32_t open(const uint8_t* filename)
{
	int i;

	dentry_t tempdentry;

	pcb_t * process_control_block = (pcb_t *) (kernel_stack_pointer & 0xFFFFE000);

	if( -1 == read_dentry_by_name(filename, &tempdentry)) {
		return -1;
	}

	for (i=2; i<8; i++) {
		if (process_control_block->fds[i].flags == 0) {	
			
			if (tempdentry.filetype == 0) //RTC
			{ 		
				if (-1 == rtc_open()) {
					return -1;
				} else {
					process_control_block->fds[i].jumptable[0] = (uint32_t)(rtc_open);
					process_control_block->fds[i].jumptable[1] = (uint32_t)(rtc_read);
					process_control_block->fds[i].jumptable[2] = (uint32_t)(rtc_write);
					process_control_block->fds[i].jumptable[3] = (uint32_t)(rtc_close);
				}
			}
			else if(tempdentry.filetype == 1) //Directory
			{ 
					process_control_block->fds[i].jumptable[0] = (uint32_t)(no_function);
					process_control_block->fds[i].jumptable[1] = (uint32_t)(no_function);
					process_control_block->fds[i].jumptable[2] = (uint32_t)(no_function);
					process_control_block->fds[i].jumptable[3] = (uint32_t)(no_function);
			}
			else if(tempdentry.filetype == 2) //Regular File
			{ 
					process_control_block->fds[i].jumptable[0] = (uint32_t)(no_function);
					process_control_block->fds[i].jumptable[1] = (uint32_t)(no_function);
					process_control_block->fds[i].jumptable[2] = (uint32_t)(no_function);
					process_control_block->fds[i].jumptable[3] = (uint32_t)(no_function);
			}

			process_control_block->fds[i].flags = 1;
			return i;
		}		
	}

	printf("The File Descriptor Array is Filled\n");
	return -1;	
}

int32_t close(int32_t fd)
{
	return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes)
{
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
