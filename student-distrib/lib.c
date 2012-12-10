/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab
 */

#include "lib.h"
#include "types.h"
#include "keyboard.h"

/* Represents which tty is currently being processed: 0, 1 or 2. */
static int process_term_number = 0;

/* Represents which tty is active: 0, 1 or 2. */
static int active_term = 0;

/* This is an array of the current x position on the three terminals */
static int screen_x[3];

/* This is an array of the current y position on the three terminals */
static int screen_y[3];

/* This is an array of the current x position of the most recent command on the three terminals */
static int command_x[3];

/* This is an array of the current y position of the most recent command on the three terminals */
static int command_y[3];

/* This is a pointer to video memory */
static char* video_mem = (char *)VIDEO;

/* 
 * This is an array of pointers to 3 separate video buffers.
 * These buffers are used to store the actions of the three separate terminals.
 * The video data for terminal 0 is in videobuff[0] and so on.
 */
static char* video_buff[3] = { (char *)VIDEO_BUF1,
                               (char *)VIDEO_BUF2,
                               (char *)VIDEO_BUF3
                             };


/* 
 * set_process_term_number()
 *
 * Description:
 * Sets the value of the current tty to process_term_number
 * so that the putc command called by terminal write knows 
 * which video buffer to write to.
 *
 * Inputs:
 * value: new tty being processed
 *
 * Outputs: 
 * none
 */
void
set_process_term_number(uint32_t value){
    if( value >= 0 && value < 3){
            process_term_number  = value;
    }        
}

/* 
 * set_active_term()
 *
 * Description:
 * Sets the value of the current active terminal to active_term
 * so that the putc command called by terminal write knows 
 * whether to write commands to video memory as well as video buffers
 *
 * Inputs:
 * value: new active terminal being viewed
 *
 * Outputs: 
 * none
 */
void
set_active_term(uint32_t value){
    if( value >= 0 && value < 3){
            active_term  = value;
    }        
}

/* 
 * get_active_term()
 *
 * Description:
 * Returns the active_term to an external source as saved by the lib.c
 *
 * Inputs: none
 *
 * Outputs:
 * active_term: the value of active_terminal as saved by the lib.c
 */
uint32_t
get_active_term( void ){
    return active_term;      
}


/* 
 * clear()
 *
 * Description:
 * Will clear all of current memory and the video buffer of the active terminal
 *
 * Inputs: none
 *
 * Outputs: none
 *
 */
void
clear( void ) 
{
    int32_t i;
    for(i=0; i<NUM_ROWS*NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }

    for(i=0; i<NUM_ROWS*NUM_COLS; i++) {
        *(uint8_t *)(video_buff[active_term] + (i << 1)) = ' ';
        *(uint8_t *)(video_buff[active_term] + (i << 1) + 1) = ATTRIB;
    }

}

/* 
 * clear_the_screen()
 *
 * Description:
 * Will clear all of current memory and the video buffer of the active terminal
 * and will also reset the command and screen pointers back to the top left most
 * char space.
 *
 * Inputs: none
 *
 * Outputs: none
 *
 */
void clear_the_screen( void ) {
    clear();
    screen_x[active_term] = 0;
    screen_y[active_term] = 0;
    command_x[active_term] = 0;
    command_y[active_term] = 0;
    update_cursor(0); 
}

/* 
 * load_video_memory()
 *
 * Description:
 * This function is called on terminal switch, it loads in the video stored
 * in the individual video buffers into the active video memory. The cursor is 
 * then refreshed to be in the correct location on load.
 *
 * Inputs: 
 * new_terminal: the terminal buffer that we want to load into video memory
 *
 * Outputs: none
 *
 */
void load_video_memory(uint32_t new_terminal) {
    memcpy(video_mem, video_buff[new_terminal], _4KB);
    update_cursor(0); 
}

/* 
 * carriage_return()
 *
 * Description:
 * This function is called right before the print_the_buffer function
 * In order to display the command buffer, we reprint it each time it is changed
 * This implementation made moving the cursor and deleting easier
 *
 * Inputs: none
 *
 * Outputs: none
 *
 */
void carriage_return( void ) {
    screen_x[active_term] = command_x[active_term];
    screen_y[active_term] = command_y[active_term];
}

/* 
 * set_command_location()
 *
 * Description:
 * This function is called write before the terminal read function spins waiting
 * for the user to enter their command. What this does is sets the command x and
 * y location so that the command may be reprinted on keyboard interrupt
 *
 * Inputs: 
 * tty: the current terminal read being processed
 *
 * Outputs: none
 *
 */
void set_command_location(uint32_t tty){
    command_x[tty] = screen_x[tty];
    command_y[tty] = screen_y[tty]; 
}


/* 
 * update_cursor()
 *
 * Description:
 * This is the kernel's interface with the cursor. This function will use
 * the current command location plus the offset argument passed in to place
 * the cursor on the screen logically
 *
 * Inputs: 
 * x: the x offset of the cursor location 
 *
 * Outputs: none
 *
 */
void update_cursor(int x) {

    uint16_t position = (command_y[active_term] * NUM_COLS) + command_x[active_term] + x;
 
    /* cursor LOW port to vga INDEX register */
    outb(0x0F, 0x3D4);
    outb((unsigned char)(position&0xFF), 0x3D5);
    /* cursor HIGH port to vga INDEX register */
    outb(0x0E, 0x3D4);
    outb((unsigned char )((position>>8)&0xFF), 0x3D5);

 }

/* 
 * update_cursor()
 *
 * Description:
 * This is the kernel's interface with the cursor. This function will use
 * the current command location plus the offset argument passed in to place
 * the cursor on the screen logically
 *
 * Inputs: 
 * x: the x offset of the cursor location 
 *
 * Outputs: none
 *
 */
void scrolling_mem(void){
        
    int x, y;

    for(y=0; y<NUM_ROWS-1; y++){
        for(x=0; x<NUM_COLS; x++){
            *(uint8_t *)(video_mem + ((NUM_COLS*y + x) << 1)) = *(uint8_t *)(video_mem + ((NUM_COLS*(y+1) + x) << 1));
            *(uint8_t *)(video_mem + ((NUM_COLS*y + x) << 1) + 1) = *(uint8_t *)(video_mem + ((NUM_COLS*(y+1) + x) << 1) + 1);
        }
    }

    for(x=0; x<NUM_COLS; x++){
        *(uint8_t *)(video_mem + ((NUM_COLS*(NUM_ROWS-1) + x) << 1)) = ' ';
        *(uint8_t *)(video_mem + ((NUM_COLS*(NUM_ROWS-1) + x) << 1) + 1) = ATTRIB;
    }
    
}

void scrolling_buff(uint32_t tty){
        
    int x, y;

    if(screen_x[tty] ==0 && screen_y[tty] ==24){
        command_y[tty]--;
    }       

    for(y=0; y<NUM_ROWS-1; y++){
        for(x=0; x<NUM_COLS; x++){
            *(uint8_t *)(video_buff[tty] + ((NUM_COLS*y + x ) << 1)) = *(uint8_t *)(video_buff[tty] + ((NUM_COLS*(y+1) + x) << 1));
            *(uint8_t *)(video_buff[tty] + ((NUM_COLS*y + x) << 1) + 1) = *(uint8_t *)(video_buff[tty] + ((NUM_COLS*(y+1) + x) << 1) + 1);
        }
    }


    for(x=0; x<NUM_COLS; x++){
        *(uint8_t *)(video_buff[tty] + ((NUM_COLS*(NUM_ROWS-1) + x) << 1)) = ' ';
        *(uint8_t *)(video_buff[tty] + ((NUM_COLS*(NUM_ROWS-1) + x) << 1) + 1) = ATTRIB;
    }
    
}

/* 
 * new_line()
 *
 * Description:
 * A public function that can be called that does a simple new line logic
 *
 * Inputs: none
 *
 * Outputs: none
 *
 */
void new_line(void){
    screen_x[active_term] =0;
    
    if(screen_y[active_term] < NUM_ROWS-1){
        screen_y[active_term]++;
    }else{
        scrolling_buff(active_term);
        scrolling_mem();
    }
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output.
 * */
int32_t
printf(int8_t *format, ...)
{
    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while(*buf != '\0') {
            switch(*buf) {
                    case '%':
                            {
                                    int32_t alternate = 0;
                                    buf++;

format_char_switch:
                                    /* Conversion specifiers */
                                    switch(*buf) {
                                            /* Print a literal '%' character */
                                            case '%':
                                                    putc('%');
                                                    break;

                                            /* Use alternate formatting */
                                            case '#':
                                                    alternate = 1;
                                                    buf++;
                                                    /* Yes, I know gotos are bad.  This is the
                                                     * most elegant and general way to do this,
                                                     * IMHO. */
                                                    goto format_char_switch;

                                            /* Print a number in hexadecimal form */
                                            case 'x':
                                                    {
                                                            int8_t conv_buf[64];
                                                            if(alternate == 0) {
                                                                    itoa(*((uint32_t *)esp), conv_buf, 16);
                                                                    puts(conv_buf);
                                                            } else {
                                                                    int32_t starting_index;
                                                                    int32_t i;
                                                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                                                                    i = starting_index = strlen(&conv_buf[8]);
                                                                    while(i < 8) {
                                                                            conv_buf[i] = '0';
                                                                            i++;
                                                                    }
                                                                    puts(&conv_buf[starting_index]);
                                                            }
                                                            esp++;
                                                    }
                                                    break;

                                            /* Print a number in unsigned int form */
                                            case 'u':
                                                    {
                                                            int8_t conv_buf[36];
                                                            itoa(*((uint32_t *)esp), conv_buf, 10);
                                                            puts(conv_buf);
                                                            esp++;
                                                    }
                                                    break;

                                            /* Print a number in signed int form */
                                            case 'd':
                                                    {
                                                            int8_t conv_buf[36];
                                                            int32_t value = *((int32_t *)esp);
                                                            if(value < 0) {
                                                                    conv_buf[0] = '-';
                                                                    itoa(-value, &conv_buf[1], 10);
                                                            } else {
                                                                    itoa(value, conv_buf, 10);
                                                            }
                                                            puts(conv_buf);
                                                            esp++;
                                                    }
                                                    break;

                                            /* Print a single character */
                                            case 'c':
                                                    putc( (uint8_t) *((int32_t *)esp));
                                                    esp++;
                                                    break;

                                            /* Print a NULL-terminated string */
                                            case 's':
                                                    puts( *((int8_t **)esp) );
                                                    esp++;
                                                    break;

                                            default:
                                                    break;
                                    }

                            }
                            break;

                    default:
                            putc(*buf);
                            break;
            }
            buf++;
    }

    return (buf - format);
}


void
putit(uint8_t c, uint32_t tty)
{
	if(active_term == tty){
	    putc(c);
	}else {     
        if(c == '\n' || c == '\r') {
            if(screen_y[tty] < NUM_ROWS-1){
			    screen_y[tty]++;
			}else{
				scrolling_buff(tty);
			}

            screen_x[tty]=0;
        } else if(c =='\0'){
            *(uint8_t *)(video_buff[tty] + ((NUM_COLS*screen_y[tty] + screen_x[tty]) << 1)) = c;
            *(uint8_t *)(video_buff[tty] + ((NUM_COLS*screen_y[tty] + screen_x[tty]) << 1) + 1) = ATTRIB;
        }else {
            *(uint8_t *)(video_buff[tty] + ((NUM_COLS*screen_y[tty] + screen_x[tty]) << 1)) = c;
            *(uint8_t *)(video_buff[tty] + ((NUM_COLS*screen_y[tty] + screen_x[tty]) << 1) + 1) = ATTRIB;
            screen_x[tty]++;
            if(screen_x[tty] > 79){
                screen_x[tty] = 0;
                
                if(screen_y[tty] < NUM_ROWS-1){
				    screen_y[tty]++;
				}else{
					scrolling_buff(tty);
				}
            }
        }
    }  
}

void
putc(uint8_t c)
{
    if(c == '\n' || c == '\r') {
        if(screen_y[active_term] < NUM_ROWS-1){
            screen_y[active_term]++;
        }else{
            scrolling_buff(active_term);
            scrolling_mem();
        }
        screen_x[active_term]=0;
    } else if(c =='\0'){
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1) + 1) = ATTRIB;
        *(uint8_t *)(video_buff[active_term] + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1)) = c;
        *(uint8_t *)(video_buff[active_term] + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1) + 1) = ATTRIB;
    }else {
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1) + 1) = ATTRIB;
        *(uint8_t *)(video_buff[active_term] + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1)) = c;
        *(uint8_t *)(video_buff[active_term] + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1) + 1) = ATTRIB;
        screen_x[active_term]++;
        if(screen_x[active_term] > 79){
                screen_x[active_term] = 0;
                if(screen_y[active_term] < NUM_ROWS-1){
                    screen_y[active_term]++;
                }else{
                    scrolling_buff(active_term);
                    scrolling_mem();
                }
        }
    }
}

/* Output a string to the console */
int32_t
puts(int8_t* s)
{
    register int32_t index = 0;
    while(s[index] != '\0') {
        putc(s[index]);
        index++;
    }

    return index;
}



/*
 * delc()
 * Deletes the character on the screen at location (screen_x,screen_y) and
 * adjusts (screen_x,screen_y) to point to the previous location.
 */
void
delc(void)
{
    *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1)) = ' ';
    *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1) + 1) = ATTRIB;
        *(uint8_t *)(video_buff[active_term] + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1)) = ' ';
    *(uint8_t *)(video_buff[active_term] + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1) + 1) = ATTRIB;
        
    if( screen_x[active_term] == 0 ) {
        if( screen_y[active_term] == 0 ) {
                return;
        }
        screen_x[active_term] = NUM_COLS - 1;
        screen_y[active_term]--;
    }
    else {
        screen_x[active_term]--;
    }
}

void
placec(uint8_t c)
{
    if(c == '\n' || c == '\r') {
        screen_y[active_term]++;
        screen_x[active_term]=0;
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1) + 1) = ATTRIB;
        *(uint8_t *)(video_buff[active_term] + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1)) = c;
        *(uint8_t *)(video_buff[active_term] + ((NUM_COLS*screen_y[active_term] + screen_x[active_term]) << 1) + 1) = ATTRIB;
        /* No screen_x or screen_y adjustment. */
    }
}

/* Convert a number to its ASCII representation, with base "radix" */
int8_t*
itoa(uint32_t value, int8_t* buf, int32_t radix)
{
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int8_t *newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if(value == 0) {
            buf[0]='0';
            buf[1]='\0';
            return buf;
    }

    /* Go through the number one place value at a time, and add the
     * correct digit to "newbuf".  We actually add characters to the
     * ASCII string from lowest place value to highest, which is the
     * opposite of how the number should be printed.  We'll reverse the
     * characters later. */
    while(newval > 0) {
            i = newval % radix;
            *newbuf = lookup[i];
            newbuf++;
            newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0';

    /* Reverse the string and return */
    return strrev(buf);
}

/* In-place string reversal */
int8_t*
strrev(int8_t* s)
{
    register int8_t tmp;
    register int32_t beg=0;
    register int32_t end=strlen(s) - 1;

    while(beg < end) {
            tmp = s[end];
            s[end] = s[beg];
            s[beg] = tmp;
            beg++;
            end--;
    }

    return s;
}

/* String length */
uint32_t
strlen(const int8_t* s)
{
    register uint32_t len = 0;
    while(s[len] != '\0')
            len++;

    return len;
}

/* Optimized memset */
void*
memset(void* s, int32_t c, uint32_t n)
{
    c &= 0xFF;
    asm volatile("                  \n\
                    .memset_top:            \n\
                    testl   %%ecx, %%ecx    \n\
                    jz      .memset_done    \n\
                    testl   $0x3, %%edi     \n\
                    jz      .memset_aligned \n\
                    movb    %%al, (%%edi)   \n\
                    addl    $1, %%edi       \n\
                    subl    $1, %%ecx       \n\
                    jmp     .memset_top     \n\
                    .memset_aligned:        \n\
                    movw    %%ds, %%dx      \n\
                    movw    %%dx, %%es      \n\
                    movl    %%ecx, %%edx    \n\
                    shrl    $2, %%ecx       \n\
                    andl    $0x3, %%edx     \n\
                    cld                     \n\
                    rep     stosl           \n\
                    .memset_bottom:         \n\
                    testl   %%edx, %%edx    \n\
                    jz      .memset_done    \n\
                    movb    %%al, (%%edi)   \n\
                    addl    $1, %%edi       \n\
                    subl    $1, %%edx       \n\
                    jmp     .memset_bottom  \n\
                    .memset_done:           \n\
                    "
                    :
                    : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
                    : "edx", "memory", "cc"
                    );

    return s;
}

/* Optimized memset_word */
void*
memset_word(void* s, int32_t c, uint32_t n)
{
    asm volatile("                  \n\
                    movw    %%ds, %%dx      \n\
                    movw    %%dx, %%es      \n\
                    cld                     \n\
                    rep     stosw           \n\
                    "
                    :
                    : "a"(c), "D"(s), "c"(n)
                    : "edx", "memory", "cc"
                    );

    return s;
}

/* Optimized memset_dword */
void*
memset_dword(void* s, int32_t c, uint32_t n)
{
    asm volatile("                  \n\
                    movw    %%ds, %%dx      \n\
                    movw    %%dx, %%es      \n\
                    cld                     \n\
                    rep     stosl           \n\
                    "
                    :
                    : "a"(c), "D"(s), "c"(n)
                    : "edx", "memory", "cc"
                    );

    return s;
}

/* Optimized memcpy */
void*
memcpy(void* dest, const void* src, uint32_t n)
{
    asm volatile("                  \n\
                    .memcpy_top:            \n\
                    testl   %%ecx, %%ecx    \n\
                    jz      .memcpy_done    \n\
                    testl   $0x3, %%edi     \n\
                    jz      .memcpy_aligned \n\
                    movb    (%%esi), %%al   \n\
                    movb    %%al, (%%edi)   \n\
                    addl    $1, %%edi       \n\
                    addl    $1, %%esi       \n\
                    subl    $1, %%ecx       \n\
                    jmp     .memcpy_top     \n\
                    .memcpy_aligned:        \n\
                    movw    %%ds, %%dx      \n\
                    movw    %%dx, %%es      \n\
                    movl    %%ecx, %%edx    \n\
                    shrl    $2, %%ecx       \n\
                    andl    $0x3, %%edx     \n\
                    cld                     \n\
                    rep     movsl           \n\
                    .memcpy_bottom:         \n\
                    testl   %%edx, %%edx    \n\
                    jz      .memcpy_done    \n\
                    movb    (%%esi), %%al   \n\
                    movb    %%al, (%%edi)   \n\
                    addl    $1, %%edi       \n\
                    addl    $1, %%esi       \n\
                    subl    $1, %%edx       \n\
                    jmp     .memcpy_bottom  \n\
                    .memcpy_done:           \n\
                    "
                    :
                    : "S"(src), "D"(dest), "c"(n)
                    : "eax", "edx", "memory", "cc"
                    );

    return dest;
}

/* Optimized memmove (used for overlapping memory areas) */
void*
memmove(void* dest, const void* src, uint32_t n)
{
    asm volatile("                  \n\
                    movw    %%ds, %%dx      \n\
                    movw    %%dx, %%es      \n\
                    cld                     \n\
                    cmp     %%edi, %%esi    \n\
                    jae     .memmove_go     \n\
                    leal    -1(%%esi, %%ecx), %%esi    \n\
                    leal    -1(%%edi, %%ecx), %%edi    \n\
                    std                     \n\
                    .memmove_go:            \n\
                    rep     movsb           \n\
                    "
                    :
                    : "D"(dest), "S"(src), "c"(n)
                    : "edx", "memory", "cc"
                    );

    return dest;
}

/* Standard strncmp */
int32_t
strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
{
    int32_t i;
    for(i=0; i<n; i++) {
            if( (s1[i] != s2[i]) ||
                            (s1[i] == '\0') /* || s2[i] == '\0' */ ) {

                    /* The s2[i] == '\0' is unnecessary because of the short-circuit
                     * semantics of 'if' expressions in C.  If the first expression
                     * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
                     * s2[i], then we only need to test either s1[i] or s2[i] for
                     * '\0', since we know they are equal. */

                    return s1[i] - s2[i];
            }
    }
    return 0;
}

/* Standard strcpy */
int8_t*
strcpy(int8_t* dest, const int8_t* src)
{
    int32_t i=0;
    while(src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
    return dest;
}

/* Standard strncpy */
int8_t*
strncpy(int8_t* dest, const int8_t* src, uint32_t n)
{
    int32_t i=0;
    while(src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }

    while(i < n) {
        dest[i] = '\0';
        i++;
    }

    return dest;
}

void
test_interrupts(void)
{
    int32_t i;
    for (i=0; i < NUM_ROWS*NUM_COLS; i++) {
        video_mem[i<<1]++;
    }
}