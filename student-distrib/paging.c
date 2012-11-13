/* paging.c - the paging initialization part of the kernel
 */

#include "lib.h"
#include "paging.h"
#include "files.h"
#include "syscalls.h"

int newest_task;

int32_t init_paging(void)
{
	int i;
	int page_table_holder;
	
	newest_task = 0;

	/* Initialize page table for initial space pages. */
	/* Set all to present except for the page at address 0. */
	for( i = 0; i < MAX_PAGE_TABLE_SIZE; i++ ) {
		page_table[i].present = (i == 0) ? 0 : 1;
		page_table[i].read_write = 0;
		page_table[i].user_supervisor = 0;
		page_table[i].write_through = 0;
		page_table[i].cache_disabled = 0;
		page_table[i].accessed = 0;
		page_table[i].dirty = 0;
		page_table[i].pat = 0;
		page_table[i].global = 0;
		page_table[i].avail = 0;
		page_table[i].page_addr = i;
	}

	/* Initialize first page directory entry. */
	page_table_holder = (int)page_table;
	page_directories[0].dentries[0].KB.present = 1;
	page_directories[0].dentries[0].KB.read_write = 0;
	page_directories[0].dentries[0].KB.user_supervisor = 0;
	page_directories[0].dentries[0].KB.write_through = 0;
	page_directories[0].dentries[0].KB.cache_disabled = 0;
	page_directories[0].dentries[0].KB.accessed = 0;
	page_directories[0].dentries[0].KB.page_size = 0;
	page_directories[0].dentries[0].KB.global = 0;
	page_directories[0].dentries[0].KB.avail = 0;
	page_directories[0].dentries[0].KB.table_addr = page_table_holder >> 12;

	/* Initialize the kernel page directory entry. */
	page_directories[0].dentries[1].MB.present = 1;
	page_directories[0].dentries[1].MB.read_write = 1;
	page_directories[0].dentries[1].MB.user_supervisor = 0;
	page_directories[0].dentries[1].MB.write_through = 0;
	page_directories[0].dentries[1].MB.cache_disabled = 0;
	page_directories[0].dentries[1].MB.accessed = 0;
	page_directories[0].dentries[1].MB.dirty = 0;
	page_directories[0].dentries[1].MB.page_size = 1;
	page_directories[0].dentries[1].MB.global = 1;
	page_directories[0].dentries[1].MB.avail = 0;
	page_directories[0].dentries[1].MB.pat = 0;
	page_directories[0].dentries[1].MB.page_addr = 1;

	/* Initialize the remaining page directory entries to absent. */
	for( i = 2; i < MAX_PAGE_DIRECTORY_SIZE; i++ ) {
	page_directories[0].dentries[i].MB.present = 0;
	page_directories[0].dentries[i].MB.read_write = 1;
	page_directories[0].dentries[i].MB.user_supervisor = 0;
	page_directories[0].dentries[i].MB.write_through = 0;
	page_directories[0].dentries[i].MB.cache_disabled = 0;
	page_directories[0].dentries[i].MB.accessed = 0;
	page_directories[0].dentries[i].MB.dirty = 0;
	page_directories[0].dentries[i].MB.page_size = 0;
	page_directories[0].dentries[i].MB.global = 0;
	page_directories[0].dentries[i].MB.avail = 0;
	page_directories[0].dentries[i].MB.pat = 0;
	page_directories[0].dentries[i].MB.page_addr = i;
	}

	/* Set control registers to enable paging correctly. */
	asm (
	"movl $page_directories, %%eax   ;"
	"andl $0xFFFFFFE7, %%eax          ;"
	"movl %%eax, %%cr3                ;"
	"movl %%cr4, %%eax                ;"
	"orl $0x00000090, %%eax           ;"
	"movl %%eax, %%cr4                ;"
	"movl %%cr0, %%eax                ;"
	"orl $0x80000000, %%eax 	      ;"
	"movl %%eax, %%cr0                 "
	: : : "eax", "cc" );
	
	return 0;
}

int32_t setup_new_task(void)
{
	/* Reject the request if we already have 6 tasks running. */
	if( newest_task >= 5 )
	{
		return -1;
	}
	
	newest_task++;
	
	/* Initialize the kernel page directory entry. */
	page_directories[newest_task].dentries[0].MB.present = 1;
	page_directories[newest_task].dentries[0].MB.read_write = 1;
	page_directories[newest_task].dentries[0].MB.user_supervisor = 0;
	page_directories[newest_task].dentries[0].MB.write_through = 0;
	page_directories[newest_task].dentries[0].MB.cache_disabled = 0;
	page_directories[newest_task].dentries[0].MB.accessed = 0;
	page_directories[newest_task].dentries[0].MB.dirty = 0;
	page_directories[newest_task].dentries[0].MB.page_size = 1;
	page_directories[newest_task].dentries[0].MB.global = 1;
	page_directories[newest_task].dentries[0].MB.avail = 0;
	page_directories[newest_task].dentries[0].MB.pat = 0;
	page_directories[newest_task].dentries[0].MB.page_addr = 1;
	
	/* Set up a directory entry for the program image. */
	page_directories[newest_task].dentries[0x20].MB.present = 1;
	page_directories[newest_task].dentries[0x20].MB.read_write = 1;
	page_directories[newest_task].dentries[0x20].MB.user_supervisor = 0;
	page_directories[newest_task].dentries[0x20].MB.write_through = 0;
	page_directories[newest_task].dentries[0x20].MB.cache_disabled = 0;
	page_directories[newest_task].dentries[0x20].MB.accessed = 0;
	page_directories[newest_task].dentries[0x20].MB.dirty = 0;
	page_directories[newest_task].dentries[0x20].MB.page_size = 1;
	page_directories[newest_task].dentries[0x20].MB.global = 1;
	page_directories[newest_task].dentries[0x20].MB.avail = 0;
	page_directories[newest_task].dentries[0x20].MB.pat = 0;
	page_directories[newest_task].dentries[0x20].MB.page_addr = newest_task+1;
}

