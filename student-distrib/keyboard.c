
#include "lib.h"
#include "keyboard.h"
#include "i8259.h"


/* Initialize the keyboard */
void
keyboard_init(void) {

	/* Unmask IRQ1 */
	enable_irq(1);

}

