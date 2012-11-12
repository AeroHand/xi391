/* idt.c - the idt config part of the kernel
 */

#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "rtc.h"
#include "keyboard.h"
#include "interrupthandler.h"

/* Divide Error Exception */
void
exception_DE(){
	printf("Divide Error!\n");
	while(1){}
}

/* Debug Exception */
void
exception_DB(){
	printf("Debug Exception!\n");
	while(1){}
}

/* Non Maskable Interrupt Exception */
void
exception_NMI(){
	printf("Non Maskable Interrupt Exception!\n");
	while(1){}
}

/* Breakpoint Exception */
void
exception_BP(){
	printf("Breakpoint Exception!\n");
	while(1){}
}

/* Overflow Exception */
void
exception_OF(){
	printf("Overflow Exception!\n");
	while(1){}
}

/* BOUND Range Exceeded Exception */
void
exception_BR(){
	printf("BOUND Range Exceeded Exception!\n");
	while(1){}
}

/* Invalid Opcode Exception */
void
exception_UD(){
	printf("Invalid Opcode Exception!\n");
	while(1){}
}

/* Device Not Available Exception */
void
exception_NM(){
	printf("Device Not Available Exception!\n");
	while(1){}
}

/* Double Fault Exception */
void
exception_DF(){
	printf("Double Fault Exception!\n");
	while(1){}
}

/* Coprocessor Segment Exception */
void
exception_CS(){
	printf("Coprocessor Segment Exception!\n");
	while(1){}
}

/* Invalid TSS Exception */
void
exception_TS(){
	printf("Invalid TSS Exception!\n");
	while(1){}
}

/* Segment Not Present */
void
exception_NP(){
	printf("Segment Not Present!\n");
	while(1){}
}

/* Stack Fault Exception */
void
exception_SS(){
	printf("Stack Fault Exception!\n");
	while(1){}
}

/* General Protection Exception */
void
exception_GP(){
	printf("General Protection Exception!\n");
	while(1){}
}

/* Page Fault Exception */
void
exception_PF(){
	printf("Page Fault Exception!\n");
	while(1){}
}

/* Floating Point Exception */
void
exception_MF(){
	printf("Floating Point Exception!\n");
	while(1){}
}

/* Alignment Check Exception */
void
exception_AC(){
	printf("Alignment Check Exception!\n");
	while(1){}
}

/* Machine Check Exception */
void
exception_MC(){
	printf("Machine Check Exception!\n");
	while(1){}
}

/* SIMD Floating-Point Exception */
void
exception_XF(){
	printf("SIMD Floating-Point Exception!\n");
	while(1){}
}

/* Undefined Interrupt */
void
general_interruption() {
	cli();
	printf("Undefined interruption!");
	sti();
}

/* Initialize the IDT 
 * In order to initialize the IDT we fill in the values of the array of struct
 * with the values required of the interruption descriptor as defined in the 
 * ISA manual. Now because of the definition of the struct we don't have to 
 * worry about the size of individual bit manipulations, just put the desired 
 * value in the struct's parameters.
 */
void 
init_idt () {
	int index;									//Initialize counter

    lidt(idt_desc_ptr);							//Load IDT size and base address
    											//to the IDTR

	for(index = 0; index < NUM_VEC; index++) {
		idt[index].present = 0x1;				//Interruption vector present
		idt[index].dpl = 0x0;					//Priority level O
		idt[index].reserved0 = 0x0;				//All vecotrs less than 32
		idt[index].size = 0x1;					//are initialized as exceptions
		idt[index].reserved1 = 0x1;				//with types 01111
		idt[index].reserved2 = 0x1;
		idt[index].reserved3 = 0x1;
		idt[index].reserved4 = 0x0;				//Unused bits
		idt[index].seg_selector = KERNEL_CS;	//Set seg selector to Kernel CS
		
		if(index >= 32) {
			/* All vectors greater than 32 are initialized as interrupts with
			 * types 01111 and with a general handler for now
			 */
			idt[index].reserved3 = 0x0;
			SET_IDT_ENTRY(idt[index], general_interruption);
		}
	}

	SET_IDT_ENTRY(idt[0], exception_DE);	//Exception 0 handler is defined
	SET_IDT_ENTRY(idt[1], exception_DF);	//Exception 1 handler is defined
	SET_IDT_ENTRY(idt[2], exception_NMI);	//Exception 2 handler is defined
	SET_IDT_ENTRY(idt[3], exception_BP);	//Exception 3 handler is defined
	SET_IDT_ENTRY(idt[4], exception_OF);	//Exception 4 handler is defined
	SET_IDT_ENTRY(idt[5], exception_BR);	//Exception 5 handler is defined
	SET_IDT_ENTRY(idt[6], exception_UD);	//Exception 6 handler is defined
	SET_IDT_ENTRY(idt[7], exception_NM);	//Exception 7 handler is defined
	SET_IDT_ENTRY(idt[8], exception_DF);	//Exception 8 handler is defined
	SET_IDT_ENTRY(idt[9], exception_CS);	//Exception 9 handler is defined
	SET_IDT_ENTRY(idt[10], exception_TS);  //Exception 10 handler is defined
	SET_IDT_ENTRY(idt[11], exception_NP);  //Exception 11 handler is defined
	SET_IDT_ENTRY(idt[12], exception_SS);  //Exception 12 handler is defined
	SET_IDT_ENTRY(idt[13], exception_GP);  //Exception 13 handler is defined
	SET_IDT_ENTRY(idt[14], exception_PF);  //Exception 14 handler is defined		
	SET_IDT_ENTRY(idt[16], exception_MF);  //Exception 16 handler is defined
	SET_IDT_ENTRY(idt[17], exception_AC);  //Exception 17 handler is defined
	SET_IDT_ENTRY(idt[18], exception_MC);  //Exception 18 handler is defined
	SET_IDT_ENTRY(idt[19], exception_XF);  //Exception 19 handler is defined
	
	// Keyboard interrupt routed to asm wrapper
	SET_IDT_ENTRY(idt[33], keyboard_handler);	
	// Clock interrupt routed to asm wrapper
	SET_IDT_ENTRY(idt[40], clock_handler);
	// System Call interrupt routed to asm wrapper
	SET_IDT_ENTRY(idt[0x80], syscall_handler);


}

