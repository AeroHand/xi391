/* paging.c - the paging initialization part of the kernel
 */

#include "lib.h"
#include "paging.h"

void init_paging
	(
	void
	)
{
	int i;
	int page_table_holder;

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
	initial_space_pde.present = 1;
	initial_space_pde.read_write = 0;
	initial_space_pde.user_supervisor = 0;
	initial_space_pde.write_through = 0;
	initial_space_pde.cache_disabled = 0;
	initial_space_pde.accessed = 0;
	initial_space_pde.page_size = 0;
	initial_space_pde.global = 0;
	initial_space_pde.avail = 0;
	initial_space_pde.table_addr = page_table_holder >> 12;

	/* Initialize the kernel page directory entry. */
	kernel_page_pde.present = 1;
	kernel_page_pde.read_write = 1;
	kernel_page_pde.user_supervisor = 0;
	kernel_page_pde.write_through = 0;
	kernel_page_pde.cache_disabled = 0;
	kernel_page_pde.accessed = 0;
	kernel_page_pde.dirty = 0;
	kernel_page_pde.page_size = 1;
	kernel_page_pde.global = 0;
	kernel_page_pde.avail = 0;
	kernel_page_pde.pat = 0;
	kernel_page_pde.page_addr = 1;

	/* Initialize the remaining page directory entries to absent. */
	for( i = 0; i < MAX_PAGE_DIRECTORY_SIZE-2; i++ ) {
	remaining_pdes[i].present = 0;
	remaining_pdes[i].read_write = 1;
	remaining_pdes[i].user_supervisor = 0;
	remaining_pdes[i].write_through = 0;
	remaining_pdes[i].cache_disabled = 0;
	remaining_pdes[i].accessed = 0;
	remaining_pdes[i].dirty = 0;
	remaining_pdes[i].page_size = 0;
	remaining_pdes[i].global = 0;
	remaining_pdes[i].avail = 0;
	remaining_pdes[i].pat = 0;
	remaining_pdes[i].page_addr = 2+i;
	}

	/* Set control registers to enable paging correctly. */
	asm (
	"movl $initial_space_pde, %%eax   ;"
	"andl $0xFFFFFFE7, %%eax          ;"
	"movl %%eax, %%cr3                ;"
	"movl %%cr4, %%eax                ;"
	"orl $0x00000010, %%eax           ;"
	"movl %%eax, %%cr4                ;"
	"movl %%cr0, %%eax                ;"
	"orl $0x80000000, %%eax 	      ;"
	"movl %%eax, %%cr0                 "
	: : : "eax", "cc" );
}

