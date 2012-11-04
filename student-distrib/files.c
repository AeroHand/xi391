/****************************************************/
/* files.c - The file system driver for the kernel. */
/****************************************************/

#include "files.h"



/* The array of directory entries for the file system. */
//dentry_t fs_dentries[MAX_NUM_FS_DENTRIES];
dentry_t * fs_dentries;

/* The statistics for the file system provided by the boot block. */
fs_stats_t fs_stats;

/* The address of the boot block. */
uint32_t bb_start;

/* The array of inodes. */
inode_t * inodes;

/* The address of the first data block. */
uint32_t data_start;



/*
 * filesystem_init()
 * Initializes global variables associated with the file system.
 */
void filesystem_init(uint32_t fs_start, uint32_t fs_end)
{
	/* Set the location of the boot block. */
	bb_start = fs_start;

	/* Populate the fs_stats variable with the filesystem statistics. */
	memcpy( &fs_stats, (void *)bb_start, 64 );

	/* Set the location of the directory entries array. */
	fs_dentries = (dentry_t *)(bb_start + 64);

	/* Set the location of the array of inodes. */
	inodes = (inode_t *)(bb_start + FS_PAGE_SIZE);

	/* Set the location of the first data block. */
	data_start = bb_start + (fs_stats.num_inodes+1)*FS_PAGE_SIZE;
}

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
int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry)
{
	/* Local variables. */
	int i;
	int8_t * new_fname;

	/* Convert the file name to a type compatible with lib functions. */
	new_fname = (int8_t *)fname;

	/* Find the entry in the array. */
	for( i = 0; i < MAX_NUM_FS_DENTRIES; i++ ) 
	{
		if( strlen( fs_dentries[i].filename ) == strlen( new_fname ) ) 
		{
			if( 0 == strncmp( fs_dentries[i].filename, new_fname, strlen( new_fname ) ) ) 
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
int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry)
{
	/* Check for an invalid index. */
	if( index >= MAX_NUM_FS_DENTRIES )
	{
		return -1;
	}
	
	/* QUESTION: do we need to check for a valid dentry? */
	
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
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length)
{
	/* Local variables. */
	uint32_t  total_successful_reads;
	uint32_t  successful_reads_within_block;
	uint32_t  cur_data_block;
	uint32_t  valid_data_blocks;
	uint8_t * read_addr;

	/* Initializations. */
	total_successful_reads = 0;
	successful_reads_within_block = 0;

	/* Check for an invalid inode number. */
	if( inode >= fs_stats.num_inodes )
	{
		return -1;
	}
	
	/* Check for invalid offset. */
	if( offset >= inodes[inode].size )
	{
		return -1;
	}

	/* [Check for a "bad data block number" somehow?] */
	/* This may need to happen elsewhere. */

	/* 
	 * read_addr and valid_data_blocks need to be initialized here
	 * once we know that 'inode' is a valid inode number.
	 */
 
	cur_data_block = offset/FS_PAGE_SIZE;

	/* Check for an invalid data block. */
	if( inodes[inode].data_blocks[cur_data_block] >= fs_stats.num_datablocks )
	{
		return -1;
	}
 
	/* Calculate the address to start reading from. */
	read_addr = (uint8_t *)(data_start + 
				(inodes[inode].data_blocks[cur_data_block])*FS_PAGE_SIZE + 
				offset % FS_PAGE_SIZE);

	/* Calculate the number of valid data blocks. */
	if( inodes[inode].size % FS_PAGE_SIZE == 0 )
	{
		valid_data_blocks = inodes[inode].size/FS_PAGE_SIZE;
	}
	else
	{
		valid_data_blocks = inodes[inode].size/FS_PAGE_SIZE + 1;
	}

	/* Read all the data. */
	while( total_successful_reads < length )
	{
		if( successful_reads_within_block >= FS_PAGE_SIZE )
		{
			successful_reads_within_block = 0;
		
			/* Move to the next data block. */
			cur_data_block++;
		
			/* Check for an invalid data block. */
			if( inodes[inode].data_blocks[cur_data_block] >= fs_stats.num_datablocks )
			{
				return -1;
			}

			/* Find the start of the next data block. */
			read_addr = (uint8_t *)(data_start + (inodes[inode].data_blocks[cur_data_block])*FS_PAGE_SIZE);
		}
	
		/* See if we've reached the end of the file. */
		if( total_successful_reads >= inodes[inode].size )
		{
			return total_successful_reads;
		}
		
		/* Read a byte. */
		buf[total_successful_reads] = *read_addr;
		successful_reads_within_block++;
		total_successful_reads++;
		read_addr++;
	}

	return total_successful_reads;
}

void files_test(void)
{
	//dentry_t dentry;
	int i;
	//int8_t * asdf = "frame1.txt";
	uint8_t buf[6000];
	
	clear();
	jump_to_point(0,0);
	/*
	for( i = 0; i < 10; i++ )
	{
		read_dentry_by_index( i, &dentry );
		puts(dentry.filename);
		putc(' ');
		printf("%d", dentry.inode);
		putc('\n');
	}
	
	read_dentry_by_name((uint8_t *)asdf, &dentry); 
	printf("%d\n", dentry.inode);
	*/
	read_data(1, 0, buf, 6000);
	
	for( i = 0; i < 200; i++ )
	{
		printf("%x ", buf[i]);
		if( i % 16 == 0 )
		{
			putc('\n');
		}
	}
	//puts((int8_t *)buf);
	/*
	. 0
	frame1.txt 15
	ls 18
	grep 9
	sched 19
	hello 1
	rtc 3
	testprint 10
	sigtest 16
	shell 17
	*/
}

