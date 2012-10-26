/* keyboard.c - the keyboard driver for the kernel
 */

#include "lib.h"
#include "keyboard.h"
#include "i8259.h"


unsigned char kbd_chars[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

unsigned char terminal_buffer[TERMINAL_BUFFER_MAX_SIZE];
unsigned int TERMINAL_BUFFER_CURRENT_SIZE;
unsigned int typing_started;


/* Initialize the keyboard */
void
keyboard_init(void) {

	/* Initially zero the buffer. [is this necessary?] */
	int i;
	for( i = 0; i < TERMINAL_BUFFER_MAX_SIZE; i++ ) {
		terminal_buffer[i] = 0;
	}
	
	TERMINAL_BUFFER_CURRENT_SIZE = 0;
	typing_started = 0;
	
	/* Unmask IRQ1 */
	enable_irq(KEYBOARD_IRQ);

}

/* Keyboard Interrupt */
void
keyboard_interruption() {
	
	/* Mask interrupts */
	cli();
	
	unsigned char nowcode;
	unsigned char newcode;
	unsigned char datum;
	
	if( typing_started == 0 ) {
		typing_started = 1;
		clear();
		jump_to_start();
		puts("[yo this the terminal]$ ");
	}
	

	do {
		/* Dequeue the typed character from the keyboard buffer. */
		nowcode = inb(KEYBOARD_PORT);
		
		/* Store the datum received from the keyboard port. */
		datum = kbd_chars[nowcode];

		if( nowcode & 0x80 ) {
			/* Key release. Do nothing? */
		}
		else if( datum == '\n' ) {
			/* Issue the typed command or something... */
			/* For now, print the newline. */
			putc('\n');
			placec('_');
		}
		else if( datum == '\b' ) {
			/* Backspace. */
			terminal_buffer[TERMINAL_BUFFER_CURRENT_SIZE--] = 0;
			delc();
			placec('_');
		}
		else {
			/* Add char to terminal buffer. */
			terminal_buffer[TERMINAL_BUFFER_CURRENT_SIZE++] = datum;
			//delc();
			putc(datum);
			placec('_');
		}
		
		
		
		newcode = inb(KEYBOARD_STATUS_PORT);    //Check to see if the keyboard buffer is full
		
		
		//printf("Buff Status = %c                                                                \n",newcode);
		/* As specified in the MP3 spec, we call test_interrupts() */
		//test_interrupts();
	} while ((newcode & 0x01) != 0x00);			//If the buffer is still full repeat the process until the buffer is empty
		//printf("                                                                                \n");

	/* Send End-of-Interrupt */
	send_eoi(KEYBOARD_IRQ);

	/* Unmask interrupts */
	sti();
	
}

