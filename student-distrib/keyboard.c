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
	

	do {
		nowcode= inb(KEYBOARD_PORT);			//Deque the typed character from the keyboard buffer
		printf("We got tyeped character=  %x                                                    \n",nowcode);
		newcode= inb(KEYBOARD_STATUS_PORT);		//Check to see if the keyboard buffer is full
		printf("Buff Status=       %x                                                           \n",newcode);
	} while ((newcode & 0x01) != 0x00);			//If the buffer is still full repeat the process unitl the buffer is empty
		printf("                                                                                \n");

	/* Send End-of-Interrupt */
	send_eoi(KEYBOARD_IRQ);

	/* Unmask interrupts */
	sti();
	
}

