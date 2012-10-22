/* rtc.c - the rtc driver for the kernel
 */

#include "lib.h"
#include "rtc.h"
#include "i8259.h"


/* Initialize the RTC */
void
rtc_init(void) {

	/* Get previous values of the RTC's Registers A and B */
	outb(INDEX_REGISTER_A, RTC_PORT);
	unsigned char a_old = inb(CMOS_PORT);
	outb(INDEX_REGISTER_B, RTC_PORT);
	unsigned char b_old = inb(CMOS_PORT);

	/* set A[6:4] (DV) to 010 - turn on oscillator/allow RTC to keep time
	 * set A[3:0] (RS) to 0110 - set interrupt rate to 1.024 kHz */
	outb(INDEX_REGISTER_A, RTC_PORT);
	outb((KILL_DV_RS & a_old) | DV_RS, CMOS_PORT);

	/* set B[6] (PIE) to 1 - to turn on period interrupts
	 * set B[5] (AIE) to 0 - not allow alarm flag to assert irq'
	 * set B[4] (UIE) to 0 - not allow update-end flag to assert irq' */
	outb(INDEX_REGISTER_B, RTC_PORT);
	outb((KILL_PIE_AIE_UIE & b_old) | PIE_AIE_UIE, CMOS_PORT);

	/* Unmask RTC */
	enable_irq(RTC_IRQ);

	/* Here Initialze the time and calendar and stuff? */

}

/* RTC Interrupt */
void
clock_interruption() {
	/* Mask interrupts */
	cli();

	/* We want to read in what Register C has got for us */
	outb(INDEX_REGISTER_C, RTC_PORT);
	/* We don't even care what it is at this point */
	inb(CMOS_PORT);

	/* Print a ticker to the screen */
	//printf("Tick tock                                                                       \n");
	/* As specified in the MP3 spec, we call test_interrupts() */
	test_interrupts();

	/* Send End-of-Interrupt */
	send_eoi(RTC_IRQ);
	
	/* Unmask interrupts */
	sti();
}

