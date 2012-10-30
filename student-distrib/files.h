#ifndef FILES_H
#define FILES_H



/* Constants. */
#define MAX_NUM_FS_DENTRIES  63
#define MAX_FILENAME_LENGTH  32



/* Functions. */
int32_t read_dentry_by_name
	( 
	const uint8_t *  fname, 
	      dentry_t * dentry 
	);
	
int32_t read_dentry_by_index
	( 
	uint32_t   index, 
	dentry_t * dentry 
	);
	
int32_t read_data
	( 
	uint32_t  inode, 
	uint32_t  offset, 
	uint8_t * buf, 
	uint32_t  length 
	);



#endif