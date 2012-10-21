#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

void exception_DE(){
	printf("Divide Error!\n");
}

void exception_DB(){
	printf("Debug Exception!\n");
}

void exception_NMI(){
	printf("Non Maskable Interrupt Exception!\n");
}

void exception_BP(){
	printf("Breakpoint Exception!\n");
}

void exception_OF(){
	printf("Overflow Exception!\n");
}

void exception_BR(){
	printf("BOUND Range Exceeded Exception!\n");
}

void exception_UD(){
	printf("Invalid Opcode Exception!\n");
}

void exception_NM(){
	printf("Device Not Available Exception!\n");
}

void exception_DF(){
	printf("Double Fault Exception!\n");
}

void exception_CS(){
	printf("Coprocessor Segment Exception!\n");
}

void exception_TS(){
	printf("Invalid TSS Exception!\n");
}

void exception_NP(){
	printf("Segment Not Present!\n");
}

void exception_SS(){
	printf("Stack Fault Exception!\n");
}

void exception_GP(){
	printf("General Protection Exception!\n");
}

void exception_PF(){
	printf("Page Fault Exception!\n");
}

void exception_MF(){
	printf("Floating Point Error\n");
}

void exception_AC(){
	printf("Alignment Check Exception!\n");
}

void exception_MC(){
	printf("Machine Check Exception!\n");
}

void exception_XF(){
	printf("SIMD Floating-Point Exception!\n");
}

void general_interruption(){
	cli();
	printf("Stop bothering me with your motherfucking unnamed interruptions");
	sti();
	send_eoi(1);
}

void keyboard_interruption(){
	cli();
	int scancode = inb(0x60);
	printf("We got something=%x\n",scancode);
	sti();
	send_eoi(1);
}

void clock_interruption(){
	cli();
	printf("Tick Tock Fuck Clocks");
	sti();
	send_eoi(8);
}

void 
init_idt (){
	int index;

    lidt(idt_desc_ptr);

	for(index = 0; index < NUM_VEC ; index++){
		idt[index].present = 0x1;
		idt[index].dpl = 0x0;
		idt[index].reserved0 = 0x0;
		idt[index].size = 0x1;
		idt[index].reserved1 = 0x1;
		idt[index].reserved2 = 0x1;
		idt[index].reserved3 = 0x1;
		idt[index].reserved4 = 0x0;
		idt[index].seg_selector = 0x10;
		if(index >= 32){
			idt[index].reserved3 = 0x0;
			SET_IDT_ENTRY(idt[index], general_interruption);
		}
	}

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
	SET_IDT_ENTRY(idt[33], keyboard_interruption);
	SET_IDT_ENTRY(idt[40], clock_interruption);
}
