
#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

//Keyboard interrupt asm wrapper
extern void keyboard_handler();

//Clock interrupt asm wrapper
extern void clock_handler();

//System Call interrupt asm wrapper
extern void syscall_handler();

//System Call interrupt asm wrapper
extern void test_syscall(int syscallnum, int status);

#endif /* INTERRUPT_HANDLER_H*/

