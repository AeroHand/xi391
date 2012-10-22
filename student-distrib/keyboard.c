/* keyboard.c - the keyboard driver for the kernel
 */

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
	
	/* Mask interrupts */
	cli();
	
	int nowcode;
	int newcode;
	
	/* TODO write explanation */
	do {
		newcode= inb(KEYBOARD_STATUS_PORT);
		printf("Buff Status=       %x                                                           \n",newcode);
		nowcode= inb(KEYBOARD_PORT);
		printf("We got something=  %x                                                           \n",nowcode);
		newcode= inb(KEYBOARD_STATUS_PORT);
		printf("Buff Status=       %x                                                           \n",newcode);
	} while ((newcode & 0x01) != 0x00);
		printf("                                                                                \n");

	/* Send End-of-Interrupt */
	send_eoi(KEYBOARD_IRQ);

	/* Unmask interrupts */
	sti();
	
}

