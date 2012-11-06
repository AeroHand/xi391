/* rtc.c - the rtc driver for the kernel
 */

#include "lib.h"
#include "rtc.h"
#include "i8259.h"

int interrupt_occurred = 0;
int rtc_is_open = 0;

/* Initialize the RTC */
void
rtc_init(void) {

	/* Get previous values of the RTC's Registers A and B */
	outb(INDEX_REGISTER_A, RTC_PORT);
	unsigned char a_old = inb(CMOS_PORT);
	outb(INDEX_REGISTER_B, RTC_PORT);
	unsigned char b_old = inb(CMOS_PORT);

	/* set A[6:4] (DV) to 010 - turn on oscillator/allow RTC to keep time
	 * set A[3:0] (RS) to 1111 - set interrupt rate to 2 Hz */
	outb(INDEX_REGISTER_A, RTC_PORT);
	outb((KILL_DV_RS & a_old) | DV_RS, CMOS_PORT);

	/* set B[7] (SET) to 0 - to allow normal updating
	 * set B[6] (PIE) to 1 - to turn on period interrupts
	 * set B[5] (AIE) to 0 - not allow alarm flag to assert irq'
	 * set B[4] (UIE) to 0 - not allow update-end flag to assert irq' */
	outb(INDEX_REGISTER_B, RTC_PORT);
	outb((KILL_SET_PIE_AIE_UIE & b_old) | SET_PIE_AIE_UIE, CMOS_PORT);

}

/* RTC Interrupt */
void
clock_interruption(void) {
	/* Mask interrupts */
	cli();

	/* We want to read in what Register C has got for us */
	outb(INDEX_REGISTER_C, RTC_PORT);
	/* We don't even care what it is at this point */
	inb(CMOS_PORT);

	/* As specified in the MP3 spec, we call test_interrupts() */
	//test_interrupts();

	/* Send End-of-Interrupt */
	send_eoi(RTC_IRQ);

	/* Set the interrupt_occured flag to one. */
	interrupt_occurred = 1;
	
	/* Unmask interrupts */
	sti();
}

/* System Call: read */
/* For the real-time clock, this call should always return 0, but only after
 * an interrupt has occurred (set a flag and wait until the interrupt handler
 * clears it, then return 0).
 */
int32_t
rtc_read (void) {
	
	/* Spin until the interrupt has occurred */
	while (!interrupt_occurred);
	
	/* Clear the flag back to zero. */
	interrupt_occurred = 0;

	/* Always return 0 */
	return 0;

}

/* System Call: write */
/* In the case of the real-time clock, the system call should always accept
 * only a 4-byte integer specifying the interrupt rate in Hz, and should set
 * the rate of periodic interrupts accordingly
 */
/* The call returns the number of bytes written, or -1 on failure. */
int32_t
rtc_write (int32_t nbytes) {
	
	/* TODO mask NMIs? */

	outb(INDEX_REGISTER_A, RTC_PORT);
	unsigned char a_old = inb(CMOS_PORT);

	int8_t rs;
	switch(nbytes) {
		case 8192:
		case 4096:
		case 2048:
			return -1;
		case 1024: rs = HZ1024; break;
		case 512: rs = HZ512; break;
		case 256: rs = HZ256; break;
		case 128: rs = HZ128; break;
		case 64: rs = HZ64; break;
		case 32: rs = HZ32; break;
		case 16: rs = HZ16; break;
		case 8: rs = HZ8; break;
		case 4: rs = HZ4; break;
		case 2: rs = HZ2; break;
		case 0: rs = HZ0; break;
		default:
			return -1;
	}

	/* set A[3:0] (RS) to rs */
	outb(INDEX_REGISTER_A, RTC_PORT);
	outb((KILL_RS & a_old) | rs, CMOS_PORT);

	/* return number of bytes on success (always 0) */
	// TODO ask if i should return 0, 1, 2, 3 on success
	return 0;

	/* TODO unmask NMIs? */
}

/* System Call: open */
/* The call should find the directory entry corresponding to the named file,
 * allocate an unused file descriptor, and set up any data necessary to handle
 * the given type of file (directory, real-time clock device, or regular file).
 * If the named file does not exist or no descriptors are free, the call returns
 * -1.
 */
int32_t rtc_open (void) {
	if(rtc_is_open) {
		return -1;
	} else {
		/* Initialize RTC */
		rtc_init();
		/* Unmask RTC */
		enable_irq(RTC_IRQ);
		/* Set rtc open flag */
		rtc_is_open = 1;

		return 0;
	}
}

/* System Call: close */
/* The close system call closes the specified file descriptor and makes it
 * available for return from later calls to open.  You should not allow the user
 * to close the default descriptors (0 for input and 1 for output). Trying to
 * close an invalid descriptor should result in a return value of -1; successful
 * closes should return 0.  
 */
int32_t rtc_close (void) {
	if(rtc_is_open) {
		/* Mask RTC */
		disable_irq(RTC_IRQ);
		/* Clear rtc open flag */
		rtc_is_open = 0;

		return 0;
	} else {
		return -1;
	}
}

