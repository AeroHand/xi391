
#include "lib.h"
#include "keyboard.h"
#include "i8259.h"


/* Initialize the keyboard */
void
keyboard_init(void) {

	/* Unmask IRQ1 */
	enable_irq(KEYBOARD_IRQ);

}

/* Keyboard Interrupt */
void
keyboard_interruption() {
	cli();
	
	int nowcode;
	int newcode;
	
	do {
		newcode= inb(0x64);
		printf("Buff Status=       %x                      \n",newcode);
		nowcode= inb(0x60);
		printf("We got something=  %x                      \n",nowcode);
		newcode= inb(0x64);
		printf("Buff Status=       %x                      \n",newcode);
	} while ((newcode & 0x01) != 0x0);
		printf("finish                                     \n");

	send_eoi(KEYBOARD_IRQ);
	sti();
	
}

