/******************************************************************/
/* scheduler.c - The scheduler and PIT controller for the kernel. */
/******************************************************************/
#include "scheduler.h"
#include "lib.h"
#include "i8259.h"



/*
 * pit_init()
 *
 * Description:
 * Initializes the PIT.
 *
 * Inputs: none
 * Retvals: none
 */
void pit_init(void) 
{
	/* Set frequency to 33HZ (interrupt every 30 milliseconds). */
    outb(0x36, PIT_CMDREG);
    outb(DIVISOR_33HZ & 0xFF, PIT_CHANNEL0);
    outb(DIVISOR_33HZ >> 8, PIT_CHANNEL0);

	/* Output from PIT channel 0 is connected to the PIC chip, so that it 
	 * generates an "IRQ 0" */
	enable_irq(PIT_IRQ);
}

/*
 * pit_interruption()
 *
 * Description:
 * The handler for an PIT interrupt. Invokes process scheduling action.
 *
 * Inputs: none
 * Retvals: none
 */
void pit_interruption(void)
{
	/* Mask interrupts */
	cli();

	/* ????? */
	change_process();
	/* ????? */

	/* Send EOI, otherwise we freeze up. */
	send_eoi(PIT_IRQ);

	/* Unmask interrupts */
	sti();
}

/* ?????????????????????????????????????????????????????????????????????????????
 * change_process()
 *
 * Description:
 * Here is what I imagine we do to get this scheduler to work.
 *  - (What's already happened: the PIT is a timer which will fire occasionally
 *     when it's time to move on to the next process in the round-robin
 *     scheduling technique. The PIT fires and is set up in the idt to link to a
 * 	   handy interrupt wrapper which calls "pit_interruption" which calls this.)
 *  - We need to switch to the next available process now.
 *  - To do that we do what we do in a halt syscall, but instead of dropping
 *    back into the parent process, we want to simply move onto the next process
 *    in our "running_processes" bitmask.
 *  --- All this should take is swapping the current ksp and ebp with the
 *      next_ksp and next_ebp (instead of the parent_ksp and parent_ebp).
 *  - PITFALL: We can't simply move onto the next bit in the bitmask (from bit 7
 *    to bit 6) because if bit 6 represents a shell which is currently running
 *    fish, it will fuck up the terminating of fish or do something weird-ish.
 * Sound good? 
 *  -rob
 * 
 * Inputs: ???
 * Retvals: ???
 * ?????????????????????????????????????????????????????????????????????????????
 */
void change_process()
{

}