/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7 */
uint8_t slave_mask = 0xFF; /* IRQs 8-15 */

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
	outb( ICW3_MASTER, MASTER_8259_PORT + 1 ); /* Slave attached to IR line 2 */
	outb( ICW3_SLAVE , SLAVE_8259_PORT  + 1 ); /* This in IR line 2 of master */
	
	/* ICW4 */
	outb( 0x05, MASTER_8259_PORT + 1 ); /* Set as master */
	outb( ICW4, SLAVE_8259_PORT  + 1 ); /* Set as slave */
}

/* Enable (unmask) the specified IRQ 
 * Write an 8-bit value corresponding to the mask 
 * Active low!
 */
void
enable_irq(uint32_t irq_num)
{

	/* Return if irq_num is invalid */
	if ((irq_num > 15) || (irq_num < 0)) {
		return;
	}

	/* initially mask = 11111110 */
	uint8_t mask = 0xFE; 

	/**** If irq_num is in master bounds: ****/
	if ((irq_num >= 0) && (irq_num <= 7)) {
		/* left circular shift by irq_num */
		int b;
		for (b = 0; b < irq_num; b++) {
			mask = (mask << 1) + 1;
		}

		master_mask = master_mask & mask;
		outb( master_mask, MASTER_8259_PORT + 1 );
		return;
	}

	/**** If irq_num is in slave bounds: ****/
	if ((irq_num >= 8) && (irq_num <= 15)) {
		irq_num -= 8; /* Get irq_num into 0-7 range */
		/* left circular shift by irq_num */
		int b;
		for (b = 0; b < irq_num; b++) {
			mask = (mask << 1) + 1;
		}

		slave_mask = slave_mask & mask;
		outb( slave_mask, SLAVE_8259_PORT + 1 );
		return;
	}
}

/* Disable (mask) the specified IRQ
 * Write an 8-bit value corresponding to the mask 
 * InActive high!
 */
void
disable_irq(uint32_t irq_num)
{
	/* Return if irq_num is invalid */
	if ((irq_num > 15) || (irq_num < 0)) {
		return;
	}

	/* initially mask = 00000001 */
	uint8_t mask = 0x01; 

	/**** If irq_num is in master bounds: ****/
	if ((irq_num >= 0) && (irq_num <= 7)) {
		/* left circular shift by irq_num */
		int b;
		for (b = 0; b < irq_num; b++) {
			mask = (mask << 1);
		}

		master_mask = master_mask | mask;
		outb( master_mask, MASTER_8259_PORT + 1 );
		return;
	}

	/**** If irq_num is in slave bounds: ****/
	if ((irq_num >= 8) && (irq_num <= 15)) {
		irq_num -= 8; /* Get irq_num into 0-7 range */
		/* left circular shift by irq_num */
		int b;
		for (b = 0; b < irq_num; b++) {
			mask = (mask << 1);
		}

		slave_mask = slave_mask | mask;
		outb( slave_mask, SLAVE_8259_PORT + 1 );
		return;
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	/**** If irq_num is in master bounds: ****/

	if ((irq_num >= 0) && (irq_num <= 7)) {
		outb( EOI + irq_num, MASTER_8259_PORT);
	}

	/**** If irq_num is in slave bounds: ****/

	if ((irq_num >= 8) && (irq_num <= 15)) {
		outb( EOI | irq_num, SLAVE_8259_PORT );
		outb( EOI | irq_num, MASTER_8259_PORT);
	}

}

