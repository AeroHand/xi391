
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

	/* select rtc register B and disable NMI by setting the 7th bit */
	outb(0x8B, 0x70);
	/* Set PIE=1 and stuff */
	outb(0xDE, 0x71);
	/* select rtc register A and disable NMI by setting the 7th bit */
	outb(0x8A, 0x70);
	/* Set RS[3:0]=0110 and stuff */
	outb(0x26, 0x71);

	/* Here Initialze the time and calendar and stuff? */
	/* Here set B[7] (SET) back to 0? */

	/* Unmask RTC */
	enable_irq(8);
}
