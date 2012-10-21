
#include "lib.h"
#include "rtc.h"


/* Initialize the RTC */
void
rtc_init(void) {
	/* select Status Register A, and disable NMI (by setting the 0x80 bit) */
	outb(0x70, 0x8A); 
	/* write to CMOS/RTC RAM */
	outb(0x71, 0x20); 
}
