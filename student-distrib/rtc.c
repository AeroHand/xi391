
#include "lib.h"
#include "rtc.h"
#include "i8259.h"


/* Initialize the RTC */
void
rtc_init(void) {

	/*
	 * set B[7] (SET) to 1 - make update transfter inhibited?
	 * set A[6:4] (DV) to 010 - turn on oscillator/allow RTC to keep time
	 * set A[3:0] (RS) to 0110 - set interrupt rate to 1.024 kHz
	 * set B[6] (PIE) to 1 - to turn on period interrupts
	 * set B[5] (AIE) to 0 - not allow alarm flag to assert irq'
	 * set B[3] (SQWE) to 1 - enable square wave
	 * set B[2] (DM) to 1 - calendar information stored in binary
	 * set B[1] to 1 - enable 24 hour mode
	 */

	/* Mask all interrupts? */

	cli();
	/* select rtc register B and disable NMI by setting the 7th bit */
	outb(INDEX_REGISTER_B, RTC_PORT);
	/* Set PIE=1 and stuff (0xDE = 11011110) */
	outb(ENABLE_PIE, CMOS_PORT);
	/* select rtc register A and disable NMI by setting the 7th bit */
	outb(INDEX_REGISTER_A, RTC_PORT);
	/* Set RS[3:0]=0110 and stuff (0x26 = 00100110) */
	outb(SET_FREQ_1028_HZ, CMOS_PORT);

	/* Here Initialze the time and calendar and stuff? */
	outb(INDEX_REGISTER_B, RTC_PORT);

	sti();
	
	/* Unmask RTC */
	enable_irq(RTC_IRQ);
}
