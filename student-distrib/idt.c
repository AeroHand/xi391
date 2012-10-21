#include "idt.h"
#include "x86_desc.h"
#include "lib.h"

void 
init_idt (){
	int index;

    lidt(idt_desc_ptr);

	for(index = 0; index < 256 ; index++){
		idt[index].present = 0x1;
		idt[index].dpl = 0x0;
		idt[index].reserved0 = 0x0;
		idt[index].size = 0x1;
		idt[index].reserved1 = 0x1;
		idt[index].reserved2 = 0x1;
		idt[index].reserved3 = 0x1;
		idt[index].reserved4 = 0x0;
		idt[index].seg_selector = 0x10;
		SET_IDT_ENTRY(idt[index], test_interrupts);
	}
}
