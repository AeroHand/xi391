/* keyboard.c - the keyboard driver for the kernel
 */

#include "lib.h"
#include "keyboard.h"
#include "i8259.h"

// There are three char scan arrays, 0 = nothing, 1 = SHIFT, 2 = Caps, 3=Shift+Caps;
unsigned char kbd_chars[4][128] = {
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
	},{
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
	},{
		0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 	'Q', 'W', 'E', 'R','T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0, 0, 'A', 'S',	
	 	'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0,'\\', 'Z', 'X', 'C', 'V', 
	 	'B', 'N', 'M', ',', '.', '/', 0, '*', 0, 0,	 0,
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
	},{
	    
	    0,  0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,
	 	'q', 'w', 'e', 'r','t', 'y', 'u', 'i', 'o', 'p', '{', '}', 0,0, 'a', 's',	
	 	'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~', 0,'|', 'z', 'x', 'c', 'v', 
	 	'b', 'n', 'm', '<', '>', '?', 0, '*', 0, 0,	 0,
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
	}	
};

unsigned int terminalsize;
unsigned char command_buffer[TERMINAL_BUFFER_MAX_SIZE];
unsigned char print_buffer[TERMINAL_BUFFER_MAX_SIZE+20] = {'[', 't', 'e', 'r', 'm', 'i', 'n', 'a', 'l', ']', '$', ' ', };
unsigned int command_length;
unsigned int cursor_x;
unsigned char keyboardflag; // 0x0000 'ctrl''caps''shift'
unsigned int allow_terminal_read;

int terminal_read(unsigned char * buf, int cnt){
	int i;
	if(allow_terminal_read){
		//int printbufferlength = terminalsize + command_length -1 ;
		for(i=0; i<cnt; i++){
			buf[i] = command_buffer[i];
			command_buffer[i] = NULL;
		}
		command_length = 0;
		cursor_x = 0;
		allow_terminal_read = 0;
		return 1;
	}else{
		return 0;
	}
}



void place_character_at_index(unsigned char scancode, int index) {
	int end_of_line = command_length;
	unsigned char datum;

	datum = kbd_chars[keyboardflag & 0x03][scancode];

	while( index <= end_of_line){
		command_buffer[end_of_line+1] = command_buffer[end_of_line];
		end_of_line--;
	}
	command_buffer[index] = datum;
	command_length++;
	cursor_x++;
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
	allow_terminal_read = 0;
	command_length = 0;
	terminalsize = 12;
	cursor_x = 0;
	clear();
	jump_to_point(0,0);
	
	/* Unmask IRQ1 */
	enable_irq(KEYBOARD_IRQ);

}

void printthebuffer(){
	int i;
	int delta_y = (terminalsize+command_length)/NUM_COLS;
	carriage_return(delta_y);
	for(i=0; i<=command_length; i++){
		print_buffer[i+terminalsize] =  command_buffer[i];
	}
	for(i=0; i<=terminalsize+command_length; i++){
		putc(print_buffer[i]);
	}

}


void
dosomethingwiththis(unsigned char scancode)
{
	unsigned char nextcode;
	int cursor_index = cursor_x;
	int i;
	/* Store the datum received from the keyboard port. */
	if( (keyboardflag & 0x04) == 0 &&
			((scancode >= MAKE_1 && scancode <= MAKE_EQUALS) ||
			 (scancode >= MAKE_Q && scancode <= MAKE_R_SQUARE_BRACKET) ||
			 (scancode >= MAKE_A && scancode <= MAKE_ACCENT) ||
			 (scancode >= MAKE_BACKSLASH && scancode <= MAKE_SLASH) ||
			  scancode == MAKE_SPACE)
			)
	{
		if(command_length < TERMINAL_BUFFER_MAX_SIZE)
			place_character_at_index(scancode, cursor_x);
	}
	else if(scancode == MAKE_ENTER) //Enter
	{
		allow_terminal_read = 1;
		//printf("\n\n\n\nWe are on line %d, it was incremented by %d, printbufferlength = %d", cursor_y, (printbufferlength-(printbufferlength % NUM_COLS))/NUM_COLS + 1, printbufferlength);
	}
	else if(scancode == MAKE_BKSP) //Backspace
	{
		if(cursor_index > 0 ){
			cursor_index--;
			while( cursor_index < command_length){
				command_buffer[cursor_index] = command_buffer[cursor_index+1];
				cursor_index++;
			}
			command_length--;
			cursor_x--;
		}
	}
	else if(scancode == MAKE_DELETE) //Delete
	{
		if(cursor_index >= 0 && cursor_index < command_length){
			while( cursor_index < command_length){
				command_buffer[cursor_index] = command_buffer[cursor_index+1];
				cursor_index++;
			}
			command_length--;
		}
	}
	else if(scancode == MAKE_TAB) //TAB????
	{

	}
	else if(scancode == MAKE_CAPS) //Caps Lock
	{
		keyboardflag ^= 0x02;
	}
	else if(scancode == MAKE_L_SHFT || scancode == MAKE_R_SHFT) //ShiftDown
	{
		keyboardflag |= 0x01;
	}
	else if(scancode == BREAK_L_SHFT || scancode == BREAK_R_SHFT) //ShiftUp
	{
		keyboardflag &= ~0x01;
	}
	else if(scancode == MAKE_L_CTRL) //CtrlDown
	{
		keyboardflag |= 0x04;
	}
	else if(scancode == BREAK_L_CTRL) //CtrlUp
	{
		keyboardflag &= ~0x4;
	}
	else if(scancode == MAKE_L_ALT) //Alt?
	{

	}
	else if(scancode == EXTRAS) //Directional and RCTRL
	{
		nextcode = inb(KEYBOARD_PORT);
		printf("\n%x",nextcode );
		if(nextcode == MAKE_L_ARROW && cursor_x > 0){
			cursor_x--;
		}else if(nextcode == MAKE_R_ARROW && cursor_x < command_length ){
			cursor_x++;
		}else if(nextcode == MAKE_L_CTRL){
			keyboardflag |=  0x04;
		}else if(nextcode == BREAK_L_CTRL){
			keyboardflag &= ~0x04;
		}	
	}
	else if(keyboardflag & 0x04){   //CTRL + L
		if(scancode == MAKE_L){
			for(i=0; i<command_length; i++){
				command_buffer[i] = NULL;
			}
			command_length = 0;
			cursor_x = 0;
			clear_the_screen();
			keyboardflag &= ~0x4;
		}
	}

	update_cursor(terminalsize + cursor_x);
}

/* Keyboard Interrupt */
void
keyboard_interruption() {
	
	/* Mask interrupts */
	cli();
	
	unsigned char keyboardscancode;
	unsigned char keyboardstatus;

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

