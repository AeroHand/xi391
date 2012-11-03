/* keyboard.c - the keyboard driver for the kernel
 */

#include "lib.h"
#include "keyboard.h"
#include "i8259.h"


unsigned char kbd_chars[128] =
{
    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
 	'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0, 'a', 's',	
 	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,'\\', 'z', 'x', 'c', 'v', 
 	'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',	 0,
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


unsigned char shift_kbd_chars[128] =
{
    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
 	'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,0, 'A', 'S',	
 	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,'|', 'Z', 'X', 'C', 'V', 
 	'B', 'N', 'M', '<', '>', '?', 0, '*', 0, 0,	 0,
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

unsigned int terminalsize;
unsigned char command_buffer[TERMINAL_BUFFER_MAX_SIZE];
unsigned char print_buffer[TERMINAL_BUFFER_MAX_SIZE+20] = {'[', 't', 'e', 'r', 'm', 'i', 'n', 'a', 'l', ']', '$', ' ', };
unsigned int command_length;
unsigned int cursor_x;
unsigned int cursor_y;
unsigned char keyboardflag; // 0x0000 'alt''caps''shift'
unsigned int typing_started;


void update_cursor(int col, int row) {
    unsigned short position=(row*80) + col;
 
    // cursor LOW port to vga INDEX register
    outb(0x0F, 0x3D4);
    outb((unsigned char)(position&0xFF), 0x3D5);
    // cursor HIGH port to vga INDEX register
    outb(0x0E, 0x3D4);
    outb((unsigned char )((position>>8)&0xFF), 0x3D5);
 }

/* Initialize the keyboard */
void
keyboard_init(void) {

	/* Initially zero the buffer. [is this necessary?] */
	int i;
	for( i = 0; i < TERMINAL_BUFFER_MAX_SIZE; i++ ) {
		command_buffer[i] = 0;
	}
	
	keyboardflag = 0x00;
	typing_started = 0;
	command_length = 0;
	terminalsize = 12;
	cursor_x = terminalsize;
	cursor_y = 0;

	update_cursor(cursor_x, cursor_y);
	
	/* Unmask IRQ1 */
	enable_irq(KEYBOARD_IRQ);

}

void
printthebuffer(){
	int i;
	jump_to_start(cursor_y);
	for(i=0; i<=terminalsize+command_length; i++){
		if((i)%NUM_COLS == 0 && i!=0){
			putc('\n');
		}
		putc(print_buffer[i]);
	}
}



void
dosomethingwiththis(unsigned char scancode)
{
	unsigned char datum;
	unsigned char nextcode;
	int i;
	int cursor_index;
	int shiftptr;
	/* Store the datum received from the keyboard port. */

	if((scancode >= 0x10 && scancode<=0x19) || (scancode >= 0x1E && scancode<=0x26) || (scancode >= 0x2C && scancode<=0x32) )
	{
		if( (keyboardflag & 0x03) != 0x0) {
			/* Store the datum received from the keyboard port as shifted data. */
			if(command_length < 1024){
				datum = shift_kbd_chars[scancode];
				cursor_index = cursor_x - terminalsize;
				shiftptr = command_length;
				while( cursor_index < shiftptr){
					command_buffer[shiftptr+1] = command_buffer[shiftptr];
					shiftptr--;
				}
				command_buffer[cursor_index] = datum;
				command_length++;
				cursor_x++;
			}
		}else{
			if(command_length < 1024){
				datum = kbd_chars[scancode];
				cursor_index = cursor_x - terminalsize;
				shiftptr = command_length;
				while( cursor_index < shiftptr){
					command_buffer[shiftptr+1] = command_buffer[shiftptr];
					shiftptr--;
				}
				command_buffer[cursor_index] = datum;
				command_length++;
				cursor_x++;
			}
		}
	}
	else if((scancode >= 0x02 && scancode<=0x0D) || (scancode >= 0x33 && scancode<=0x35) || (scancode>=0x1A && scancode<=0x1B) || scancode == 0x2B || scancode == 0x27 || scancode == 0x29 ||scancode == 0x28 || scancode == 0x39)
	{
		if( (keyboardflag & 0x01)) {
			/* Store the datum received from the keyboard port as shifted data. */
			if(command_length < 1024){
				datum = shift_kbd_chars[scancode];
				cursor_index = cursor_x - terminalsize;
				shiftptr = command_length;
				while( cursor_index < shiftptr){
					command_buffer[shiftptr+1] = command_buffer[shiftptr];
					shiftptr--;
				}
				command_buffer[cursor_index] = datum;
				command_length++;
				cursor_x++;
			}
		}else{
			if(command_length < 1024){
				datum = kbd_chars[scancode];
				cursor_index = cursor_x - terminalsize;
				shiftptr = command_length;
				while( cursor_index < shiftptr){
					command_buffer[shiftptr+1] = command_buffer[shiftptr];
					shiftptr--;
				}
				command_buffer[cursor_index] = datum;
				command_length++;
				cursor_x++;
			}
		}
	}
	else if(scancode == 0x1C) //Enter
	{
		for( i = 0; i < command_length; i++ ) {
			command_buffer[i] = NULL;
		}
		cursor_y += (cursor_x-1-((cursor_x-1)%80))/NUM_COLS + 1;
		cursor_x = terminalsize;
	}
	else if(scancode == 0x0E) //Backspace
	{
		cursor_index = cursor_x - terminalsize;
		if(cursor_index >0 ){
			cursor_index--;
			while( cursor_index < command_length){
				command_buffer[cursor_index] = command_buffer[cursor_index+1];
				cursor_index++;
			}
			command_length--;
			cursor_x--;
		}
	}
	else if(scancode == 0x53) //Delete
	{
		cursor_index = cursor_x - terminalsize;
		if(cursor_index >= 0 && cursor_index < command_length){
			while( cursor_index < command_length){
				command_buffer[cursor_index] = command_buffer[cursor_index+1];
				cursor_index++;
			}
			command_length--;
		}
	}
	else if(scancode == 0x0F) //TAB????
	{

	}
	else if(scancode == 0x3A) //Caps Lock
	{
		keyboardflag^=0x02;
	}
	else if(scancode == 0x2A || scancode == 0x36) //ShiftDown
	{
		keyboardflag |= 0x01;
	}
	else if(scancode == 0xAA || scancode == 0xB6) //ShiftUp
	{
		keyboardflag &= ~0x01;
	}
	else if(scancode == 0x1D) //Ctrl
	{

	}
	else if(scancode == 0x38) //Alt?
	{

	}
	else if(scancode == 0xE0) //Directional?
	{
		nextcode = inb(KEYBOARD_PORT);
		if(nextcode == 0x4B && cursor_x > terminalsize && cursor_x){
			cursor_x--;
		}else if(nextcode == 0x4D && cursor_x < terminalsize+command_length ){
			cursor_x++;
		}	
	}
	
	command_buffer[command_length] = '\0';
	for(i=0; i<=command_length; i++){
		print_buffer[i+terminalsize] =  command_buffer[i];
	}
	update_cursor(cursor_x, cursor_y);
}

/* Keyboard Interrupt */
void
keyboard_interruption() {
	
	/* Mask interrupts */
	cli();
	
	unsigned char keyboardscancode;
	unsigned char keyboardstatus;
	
	if( typing_started == 0 ) {
		typing_started = 1;
		clear();
		jump_to_start(cursor_x);
	}
	

	do {
		/* Dequeue the typed character from the keyboard buffer. */
		keyboardscancode = inb(KEYBOARD_PORT);
		
		dosomethingwiththis(keyboardscancode);

		printthebuffer();

		/*
		if( nowcode & 0x80 ) {
			//Key release. Do nothing? 
		}
		else if( datum == '\n' ) {
			// Issue the typed command or something... 
			// For now, print the newline. 
			putc('\n');
			placec('_');
		}
		else if( datum == '\b' ) {
			// Backspace. 
			command_buffer[TERMINAL_BUFFER_CURRENT_SIZE--] = 0;
			delc();
			placec('_');
		}
		else {
			// Add char to terminal buffer.
			command_buffer[TERMINAL_BUFFER_CURRENT_SIZE++] = datum;
			//delc();
			putc(datum);
			placec('_');
		}
		*/
		
		
		keyboardstatus = inb(KEYBOARD_STATUS_PORT);    //Check to see if the keyboard buffer is full
		
		
		//printf("Buff Status = %c                                                                \n",newcode);
		/* As specified in the MP3 spec, we call test_interrupts() */
		//test_interrupts();
	} while ((keyboardstatus & 0x02) != 0x00);			//If the buffer is still full repeat the process until the buffer is empty
		//printf("                                                                                \n");

	/* Send End-of-Interrupt */
	send_eoi(KEYBOARD_IRQ);

	/* Unmask interrupts */
	sti();
	
}

