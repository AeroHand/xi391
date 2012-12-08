/*************************************************/
/* syscalls.h - The system call implementations. */
/*************************************************/
#ifndef SYSCALLS_H
#define SYSCALLS_H



#include "files.h"
#include "paging.h"



/* Constants. */
#define		IN_USE			1
#define		NOT_IN_USE		0
#define		FILE_TYPE_RTC			0
#define		FILE_TYPE_DIRECTORY		1
#define		FILE_TYPE_REGULAR_FILE	2



typedef struct file_op_table{
	int32_t (*read)(void* buf, int32_t nbytes);
	int32_t (*write)(const void* buf, int32_t nbytes);
	int32_t (*close)();
} file_op_table;



/*** System calls. ***/

/* 
 * Terminates a process, returning the specified value to its 
 * parent process. 
 */
int32_t halt(uint8_t status);

/* 
 * Attempts to load and execute a new program, handing off the 
 * processor to the new program until it terminates. 
 */
int32_t execute(const uint8_t* command);

/* 
 * Reads 'nbytes' bytes into 'buf' from the file corresponding to 
 * the given 'fd'. 
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes);

/* 
 * Writes 'nbytes' bytes from 'buf' into the file associated with 'fd'. 
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes);

/* 
 * Attempts to open the file with the given filename and give it a spot
 * in the file array in the pcb associated with the current process. 
 */
int32_t open(const uint8_t* filename);

/* 
 * Closes the specifed file descriptor and makes it available for return from
 * later calls to open. 
 */
int32_t close(int32_t fd);

/* 
 * Reads the program’s command line arguments into a user-level buffer. 
 */
int32_t getargs(uint8_t* buf, int32_t nbytes);

/* 
 * Maps the text-mode video memory into user space at a pre-set virtual address. 
 */
int32_t vidmap(uint8_t** screen_start);

/* 
 * Related to signal handling. 
 */
int32_t set_handler(int32_t signum, void* handler_address);

/* 
 * Related to signal handling. 
 */
int32_t sigreturn(void);



/*** Other functions. ***/

/* 
 * Called when we need to open stdin to initialize a new process. 
 */
void open_stdin( int32_t fd );

/* 
 * Called when we need to open stdout to initialize a new process. 
 */
void open_stdout( int32_t fd );

/* 
 * Our test function for the execute syscall. 
 */
void execute_test(void);

/* 
 * A function that literally does absolutely nothing. 
 */
int32_t no_function(void);

#endif /* SYSCALLS_H */

