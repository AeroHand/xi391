
#include "lib.h"
#include "rtc.h"
#include "i8259.h"


/* Initialize the RTC */
void
rtc_init(void) {

	/* This is how we will -learn- do this.
	 * ::
	 * Do this:
	 * so the first thing you need to do is set bit 7 of reg b to 1 to prevent
	 * updates from occuring while init-ing
	 * then init time, cal, etc
	 * [ figure out how to change clock freq ]
	 *    change freq:
	 * 	   set rs 3 through 0 
	 * then set reg b bit 7 to 0
	 * READ PAGE 18, 19, 15?
	 */

	/* FIXME */
	/* select Status Register A, and disable NMI (by setting the 0x80 bit) */
	outb(0x8A, 0x70); 
	/* write to CMOS/RTC RAM */
	outb(0x20, 0x71); 

	/* Unmask RTC */
	enable_irq(8);

}
