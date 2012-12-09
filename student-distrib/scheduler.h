/******************************************************************/
/* scheduler.h - The scheduler and PIT controller for the kernel. */
/******************************************************************/
#ifndef SCHEDULER_H
#define SCHEDULER_H


/* PIT Chip's Command Register Port */
#define PIT_CMDREG        0x43

/* PIT Channel 0's Data Register Port */
#define PIT_CHANNEL0      0x40

/* Divisors for PIT Frequency setting 
 * DIVISOR_???HZ = 1193180 / HZ;  
 */
#define DIVISOR_100HZ	11932
#define DIVISOR_33HZ	36157
#define DIVISOR_20HZ	59659

/* IRQ Constant. */
#define PIT_IRQ			0



/* Initializes the PIT for usage. */
void pit_init(void);

/* The handler for an PIT interrupt. */
void pit_interruption(void); 

/* Switch process to next in the list (round-robin style). */
void change_process(void);



#endif /* SCHEDULER_H */
