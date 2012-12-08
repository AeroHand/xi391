/**********************************************/
/* idt.c - The idt config part of the kernel. */
/**********************************************/
#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"
#include "keyboard.h"
#include "interrupthandler.h"


/* Divide Error Exception */
void exception_DE(){
	printf("Divide Error!\n");
	while(1){}
}

/* Debug Exception */
void exception_DB(){
	printf("Debug Exception!\n");
	while(1){}
}

/* Non Maskable Interrupt Exception */
void exception_NMI(){
	printf("Non Maskable Interrupt Exception!\n");
	while(1){}
}

/* Breakpoint Exception */
void exception_BP(){
	printf("Breakpoint Exception!\n");
	while(1){}
}

/* Overflow Exception */
void exception_OF(){
	printf("Overflow Exception!\n");
	while(1){}
}

/* BOUND Range Exceeded Exception */
void exception_BR(){
	printf("BOUND Range Exceeded Exception!\n");
	while(1){}
}

/* Invalid Opcode Exception */
void exception_UD(){
	printf("Invalid Opcode Exception!\n");
	while(1){}
}

/* Device Not Available Exception */
void exception_NM(){
	printf("Device Not Available Exception!\n");
	while(1){}
}

/* Double Fault Exception */
void exception_DF(){
	printf("Double Fault Exception!\n");
	while(1){}
}

/* Coprocessor Segment Exception */
void exception_CS(){
	printf("Coprocessor Segment Exception!\n");
	while(1){}
}

/* Invalid TSS Exception */
void exception_TS(){
	printf("Invalid TSS Exception!\n");
	while(1){}
}

/* Segment Not Present */
void exception_NP(){
	printf("Segment Not Present!\n");
	while(1){}
}

/* Stack Fault Exception */
void exception_SS(){
	printf("Stack Fault Exception!\n");
	while(1){}
}

/* General Protection Exception */
void exception_GP(){
	printf("General Protection Exception!\n");
	while(1){}
}

/* Page Fault Exception */
void exception_PF(){
	asm volatile(" movl %cr3, %eax ");

	printf("Page Fault Exception!\n");
	while(1){}
}

/* Floating Point Exception */
void exception_MF(){
	printf("Floating Point Exception!\n");
	while(1){}
}

/* Alignment Check Exception */
void exception_AC(){
	printf("Alignment Check Exception!\n");
	while(1){}
}

/* Machine Check Exception */
void exception_MC(){
	printf("Machine Check Exception!\n");
	while(1){}
}

/* SIMD Floating-Point Exception */
void exception_XF(){
	printf("SIMD Floating-Point Exception!\n");
	while(1){}
}

/* Undefined Interrupt */
void general_interruption() {
	cli();
	printf("Undefined interruption!");
	sti();
}

/*
 * init_idt()
 *
 * Description:
 * Initializes the Interrupt Descriptor Table.
 * In order to initialize the IDT we fill in the values of the array of struct
 * with the values required of the interruption descriptor as defined in the 
 * ISA manual. Now because of the definition of the struct we don't have to 
 * worry about the size of individual bit manipulations, just put the desired 
 * value in the struct's parameters.
 *
 * Inputs: none
 *
 * Retvals: none
 */
void init_idt () {

	/* Initialize counter */
	int index;

	/* Load IDT size and base address to the IDTR */
	lidt(idt_desc_ptr);
    											
	for(index = 0; index < NUM_VEC; index++) {

		/* Set interruption vector present */
		idt[index].present = 0x1;

		/* Set privilege level to O */
		idt[index].dpl = 0x0;

		/* All vecotrs less than 32 are initialized as interrupts with types 01111 */				
		idt[index].reserved0 = 0x0;
		idt[index].size = 0x1;

		/* Unused bits */
		idt[index].reserved1 = 0x1;
		idt[index].reserved2 = 0x1;
		idt[index].reserved3 = 0x1;
		idt[index].reserved4 = 0x0;

		/* Set seg selector to Kernel CS */
		idt[index].seg_selector = KERNEL_CS;
		
		/* All vectors greater than 32 are initialized as interrupts with
		 * types 01111 and with a general handler for now */
		if(index >= 32) {
			idt[index].reserved3 = 0x0;
			SET_IDT_ENTRY(idt[index], general_interruption);
		}
		
		/* A syscall (int 0x80) comes from privilege level 3 */
		if(index == 0x80) {
			idt[index].dpl = 0x3;
		}
	}

	/** Define INT 0x00 through INT 0x13. Route them to respective handlers. */
	SET_IDT_ENTRY(idt[0], exception_DE);
	SET_IDT_ENTRY(idt[1], exception_DF);
	SET_IDT_ENTRY(idt[2], exception_NMI);
	SET_IDT_ENTRY(idt[3], exception_BP);
	SET_IDT_ENTRY(idt[4], exception_OF);
	SET_IDT_ENTRY(idt[5], exception_BR);
	SET_IDT_ENTRY(idt[6], exception_UD);
	SET_IDT_ENTRY(idt[7], exception_NM);
	SET_IDT_ENTRY(idt[8], exception_DF);
	SET_IDT_ENTRY(idt[9], exception_CS);
	SET_IDT_ENTRY(idt[10], exception_TS);
	SET_IDT_ENTRY(idt[11], exception_NP);
	SET_IDT_ENTRY(idt[12], exception_SS);
	SET_IDT_ENTRY(idt[13], exception_GP);
	SET_IDT_ENTRY(idt[14], exception_PF);	
	SET_IDT_ENTRY(idt[16], exception_MF);
	SET_IDT_ENTRY(idt[17], exception_AC);
	SET_IDT_ENTRY(idt[18], exception_MC);
	SET_IDT_ENTRY(idt[19], exception_XF);

	/* PIT interrupt routed to asm wrapper named: pit_handler */
	SET_IDT_ENTRY(idt[32], pit_handler);
	
	/* Keyboard interrupt routed to asm wrapper named: keyboard_handler */
	SET_IDT_ENTRY(idt[33], keyboard_handler);

	/* RTC interrupt routed to asm wrapper named: clock_handler */
	SET_IDT_ENTRY(idt[40], clock_handler);

	/* System Call interrupt routed to asm wrapper named: syscall_handler */
	SET_IDT_ENTRY(idt[SYSCALL_INT], syscall_handler);

}
