#include "paging.h"

void init_paging
	(
	void
	)
{
	int i;
	int asdf;

	/* Initialize page table for initial space pages. */
	for( i = 0; i < MAX_PAGE_TABLE_SIZE; i++ ) {
		page_table[i].present = 1;
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


		//initial_space_ptes[i].val = (i << 12) | 0x1; // present bit
	}

	/* Initialize page directory. */
	asdf = (int)page_table;
	initial_space_pde.present = 1;
	initial_space_pde.read_write = 0;
	initial_space_pde.user_supervisor = 0;
	initial_space_pde.write_through = 0;
	initial_space_pde.cache_disabled = 0;
	initial_space_pde.accessed = 0;
	initial_space_pde.page_size = 0;
	initial_space_pde.global = 0;
	initial_space_pde.avail = 0;
	initial_space_pde.table_addr = asdf >> 12;

	kernel_page_pde.present = 1;
	kernel_page_pde.read_write = 0;
	kernel_page_pde.user_supervisor = 0;
	kernel_page_pde.write_through = 0;
	kernel_page_pde.cache_disabled = 0;
	kernel_page_pde.accessed = 0;
	kernel_page_pde.dirty = 0;
	kernel_page_pde.page_size = 0;
	kernel_page_pde.global = 0;
	kernel_page_pde.avail = 0;
	kernel_page_pde.pat = 0;
	kernel_page_pde.page_addr = 1;

	//initial_space_pde.val = (page_table & 0xFFFFF000) | 0x1; // present bit
	//kernel_page_pde.val = 0x00400000 | 0x80 | 0x1; // address 4MB, page size bit, present bit
	/*
	asm volatile (
	"movl $initial_space_pde, %%cr3   ;"
	"orl $0x80000000, %%cr0           "
    : );*/
}
