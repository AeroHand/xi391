/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
	/* Send the 4 ICWs to both the slave and master: */

	/* ICW1 */
	outb( ICW1, MASTER_8259_PORT ); /* Master port */
	outb( ICW1, SLAVE_8259_PORT ); /* Slave port */
	
	/* ICW2 */
	outb( ICW2_MASTER, MASTER_8259_PORT + 1 ); /* offset in the IDT */
	outb( ICW2_SLAVE , SLAVE_8259_PORT  + 1 ); /* offset in the IDT */
	
	/* ICW3 */
	outb( ICW3_MASTER, MASTER_8259_PORT + 1 ); /* Slaves attached to IR line 2 */
	outb( ICW3_SLAVE , SLAVE_8259_PORT  + 1 ); /* This in IR line 2 of master */
	
	/* ICW4 */
	outb( 0x05, MASTER_8259_PORT + 1 ); /* Set as master */
	outb( ICW4, SLAVE_8259_PORT  + 1 ); /* Set as slave */
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	/* Ensure irq_num is in proper bounds */
	if ((irq_num > 7) || (irq_num < 0)) {
		return;
	}

	/* If irq_num is in proper bounds: */

	/* initially mask = 11111110 */
	uint8_t mask = 0xF7; 
	/* left circular shift by irq_num */
	int b;
	for (b = 0; b < irq_num; b++) {
		mask = (mask << 1) + 1;
	}

	/* Write an 8-bit value corresponding to the mask */
	outb( mask, MASTER_8259_PORT + 1 );
	outb( mask, SLAVE_8259_PORT  + 1 );
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	
}

