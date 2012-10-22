
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

	outb(INDEX_REGISTER_A, RTC_PORT);
	unsigned char a_old = inb(CMOS_PORT);
	outb(INDEX_REGISTER_B, RTC_PORT);
	unsigned char b_old = inb(CMOS_PORT);

	outb(INDEX_REGISTER_A, RTC_PORT);
	outb((0x80 & a_old) | 0x2F, CMOS_PORT);

	outb(INDEX_REGISTER_B, RTC_PORT);
	outb((0x8F & b_old) | 0x40, CMOS_PORT);

	/* Unmask RTC */
	enable_irq(RTC_IRQ);

	/* Here Initialze the time and calendar and stuff? */

	/* select rtc register B and disable NMI by setting the 7th bit */
	// outb(INDEX_REGISTER_B, RTC_PORT);
	/* Set PIE=1 and stuff (0xDE = 11011110) */
	// outb(ENABLE_PIE, CMOS_PORT);
	/* select rtc register A and disable NMI by setting the 7th bit */
	// outb(INDEX_REGISTER_A, RTC_PORT);
	/* Set RS[3:0]=0110 and stuff (0x26 = 00100110) */
	// outb(SET_FREQ_1028_HZ, CMOS_PORT);

}

/* RTC Interrupt */
void
clock_interruption() {
	cli();

	outb(INDEX_REGISTER_C, RTC_PORT);
	inb(CMOS_PORT);
	printf("Tik tok");

	send_eoi(RTC_IRQ);
	
	sti();
}

