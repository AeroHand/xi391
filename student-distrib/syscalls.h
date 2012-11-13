#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "files.h"
#include "paging.h"

typedef struct file_op_table{
	int32_t (*read)(void* buf, int32_t nbytes);
	int32_t (*write)(const void* buf, int32_t nbytes);
	int32_t (*close)();
} file_op_table;

/*** System calls. ***/

/* System Call: halt */
int32_t halt(uint8_t status);
/* System Call: execute */
int32_t execute(const uint8_t* command);
/* System Call: read */
int32_t read(int32_t fd, void* buf, int32_t nbytes);
/* System Call: write */
int32_t write(int32_t fd, const void* buf, int32_t nbytes);
/* System Call: open */
int32_t open(const uint8_t* filename);
/* System Call: close */
int32_t close(int32_t fd);
/* System Call: getargs */
int32_t getargs(uint8_t* buf, int32_t nbytes);
/* System Call: vidmap */
int32_t vidmap(uint8_t** screen_start);
/* System Call: set_handler */
int32_t set_handler(int32_t signum, void* handler_address);
/* System Call: sigreturn */
int32_t sigreturn(void);

/* execute_test */
void execute_test(void);

#endif /* SYSCALLS_H*/

