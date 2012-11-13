/*************************************************/
/* syscalls.c - The system call implementations. */
/*************************************************/

#include "syscalls.h"



typedef struct file_descriptor {
	file_op_table* jumptable;
	dentry_t* inodepointer;
	int32_t fileposition;
	int32_t flags;
} file_descriptor;


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
	
	/* Initializations. */
	entry_point = 0;
	
	/* Check for an invalid command. */

	if( command == NULL )
	{
		return -1;
	}
	
	/* Get the file name of the program to be executed. */
	for( i = 0; command[i] != ' '; i++ )
	{
		fname[i] = command[i];
	}

	fname[i] = '\0';
	
	/* Read the identifying 3 bytes from the file into buf. */
	if( -1 == fs_read((const int8_t *)fname, 1, buf, 3) )
	{
		return -1;
	}
	
	/* Ensure an executable program image. */
	if( 0 != strncmp((const int8_t*)buf, "ELF", 3) )
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
	
	/* Load the program to the appropriate starting address. */
	fs_load((const int8_t *)fname, 0x08048000);
	
	/* Jump to the entry point and begin execution. */
	
	
	return 0;
}

void execute_test(void)
{
	const uint8_t * test_string = "asdf hi ";
	execute(test_string);
}


int32_t read(int32_t fd, void* buf, int32_t nbytes)
{
	return 0;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
	return 0;
}

int32_t open(const uint8_t* filename)
{
	return 0;
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
>>>>>>> aa60d2298a433602b7d166662aee355ba9e55278
}
