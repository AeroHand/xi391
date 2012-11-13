#include "syscalls.h"
#include "lib.h"
#include "types.h"
#include "files.h"
#include "x86_desc.h"


typedef struct file_descriptor {
	file_op_table* jumptable;
	dentry_t* inodepointer;
	int32_t fileposition;
	int32_t flags;
} file_descriptor;


int32_t halt(uint8_t status){
	printf("It Works %d\n", status );
	return NULL;
}

int32_t execute(const uint8_t* command){

	if(concurrent_programs > 1)
			return -1;

		/* Local variables. */
	dentry_t dentry;
	uint8_t buf[4];
	uint8_t magicnumber[4] = {0x7f, 0x45, 0x4c, 0x46};

	/* Check for invalid file name. */
	if( command == NULL )
	{
		return -1;
	}

	//check to see if file name exists	
	if( -1 == read_dentry_by_name((uint8_t *)command, &dentry) )
	{
		return -1;
	}


	//check to see if file name has information	
	if( -1 == read_data(dentry.inode, 0, buf, 4) )
	{
		return -1;
	}

	//Shell memory should start at 128MB and because our PDE is "hardwired" to out 4mbs the page that starts at 128MB should be the 32nd page
	//This will correspond to the 32 pde, then we subtract two due to our implementation of the initial space and kernel page which gives us
	// and index of 30 into the remaining_pdes[i]

	remaining_pdes[30].present = 1;
	remaining_pdes[30].page_size = 1;
	remaining_pdes[30].global = 1;
	remaining_pdes[30].page_addr = (0x800000>>12)+4096*concurrent_programs;
	remaining_pdes[30].read_write = 1;
	remaining_pdes[30].user_supervisor = 1;
	remaining_pdes[30].global = 1;

	//check for magic numbers
	if( !strncmp((int8_t*)buf, (int8_t*)magicnumber,4)){
		return -1;
	}

	if(0 > fs_load(command, (0x800000>>12)+4096*concurrent_programs))
		return -1;


	concurrent_programs++;
	return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes){
	return NULL;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){
	return NULL;
}

int32_t open(const uint8_t* filename){
	return NULL;
}

int32_t close(int32_t fd){
	return NULL;
}

int32_t getargs(uint8_t* buf, int32_t nbytes){
	return NULL;
}

int32_t vidmap(uint8_t** screen_start){
	return NULL;
}

int32_t set_handler(int32_t signum, void* handler_address){
	return NULL;
}

int32_t sigreturn(void){
	return NULL;
}
