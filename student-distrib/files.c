/****************************************************/
/* files.c - The file system driver for the kernel. */
/****************************************************/

#include "files.h"



/* 
 * The format of the file system statistics provided at the beginning of
 * the boot block.
 */
typedef struct 
{
	uint32_t num_dentries;
	uint32_t num_inodes;
	uint32_t num_datablocks;
	uint8_t  reserved[52];
} fs_stats_t;

/*
 * The format of a file system directory entry.
 */
typedef struct 
{
	uint8_t  filename[32];
	uint32_t filetype;
	uint32_t inode;
	uint8_t  reserved[24];
} dentry_t;



/* The array of directory entries for the file system. */
dentry_t fs_dentries[MAX_NUM_FS_DENTRIES];



/*
 * read_dentry_by_name()
 * 
 * Returns directory entry information (file name, file type, inode number)
 * for the file with the given name via the dentry_t block passed in.
 *
 * Retvals
 * -1: failure (non-existent file)
 * 0: success 
 */
int32_t read_dentry_by_name
	( 
	const uint8_t *  fname, 
	      dentry_t * dentry 
	)
{
int i;

/* Find the entry in the array. */
for( i = 0; i < MAX_NUM_FS_DENTRIES; i++ ) 
	{
	if( strlen( fs_dentries[i].filename ) == strlen( fname ) ) 
		{
		if( 0 == strncmp(fs_dentries[i].filename, fname, strlen( fname ) ) ) 
			{
			/* Found it! Copy the data into 'dentry'. */
			strcpy( dentry->filename, fs_dentries[i].filename );
			dentry->filetype = fs_dentries[i].filetype;
			dentry->inode = fs_dentries[i].inode;
			return 0;
			}
		}
	}

/* If we did not find the file, return failure. */
return -1;

}

/*
 * read_dentry_by_index()
 * 
 * Returns directory entry information (file name, file type, inode number) 
 * for the file with the given index via the dentry_t block passed in.
 *
 * Retvals
 * -1: failure (invalid index)
 * 0: success
 */
int32_t read_dentry_by_index
	( 
	uint32_t   index, 
	dentry_t * dentry 
	)
{
/* Check for an invalid index. */
if( index < 0 || index >= MAX_NUM_FS_DENTRIES )
	{
	return -1;
	}
	
/* Copy the data into 'dentry'. */
strcpy( dentry->filename, fs_dentries[index].filename );
dentry->filetype = fs_dentries[index].filetype;
dentry->inode = fs_dentries[index].inode;

/* Return success. */
return 0;

}

/*
 * read_data()
 * 
 * Reads (up to) 'length' bytes starting from position 'offset' in the file 
 * with inode number 'inode'. Returns the number of bytes read and placed 
 * in the buffer 'buf'. 
 *
 * Retvals
 * -1: failure (bad data block number within inode)
 * 0: end of file has been reached
 * n: number of bytes read and placed in the buffer
 */
int32_t read_data
	( 
	uint32_t  inode, 
	uint32_t  offset, 
	uint8_t * buf, 
	uint32_t  length 
	)
{
}

