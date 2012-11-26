#ifndef FILES_H
#define FILES_H

#include "types.h"
#include "lib.h"



/* Constants. */
#define MAX_NUM_FS_DENTRIES  63
#define MAX_FILENAME_LENGTH  32
#define FS_PAGE_SIZE         0x1000   // 4kB



/* 
 * File system statistics format provided at the beginning of
 * the boot block.
 */
typedef struct {
	uint32_t num_dentries;
	uint32_t num_inodes;
	uint32_t num_datablocks;
	uint8_t  reserved[52];
} fs_stats_t;

/*
 * File system directory entry format.
 */
typedef struct {
	int8_t   filename[32];
	uint32_t filetype;
	uint32_t inode;
	uint8_t  reserved[24];
} dentry_t;

/*
 * Inode block format.
 */
typedef struct{
	uint32_t size;
	uint32_t data_blocks[1023];
} inode_t;



/* Functions. */
int32_t fs_open(uint32_t fs_start, uint32_t fs_end);
int32_t fs_close(void);
int32_t fs_read(const int8_t * fname, uint32_t offset, uint8_t * buf, 
                uint32_t length);
int32_t fs_write(void);
int32_t fs_load(const int8_t * fname, uint32_t address);

void fs_init(uint32_t fs_start, uint32_t fs_end);

int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, 
                  uint32_t length);
				  
void files_test(void);

int32_t file_open(void);
int32_t file_close(void);
int32_t file_read(uint8_t * buf, uint32_t length, const int8_t * fname);
int32_t file_write(void);

int32_t dir_open(void);
int32_t dir_close(void);
int32_t dir_read(uint8_t * buf);
int32_t dir_write(void);

void reset_dir_reads(void);


#endif
