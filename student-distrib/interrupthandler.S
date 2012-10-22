# interrupthandler.S - Set up interrupt handler assembly wrappers

#define ASM	1
#include "x86_desc.h"


.global keyboard_handler
.global clock_handler
.global timer_chip_handler

# Interrupt handler must have use an assembly wrapper because it's an interrupt!
# The most important instruction that this code contains is the iret command
# This iret command returns the instruction pointer back to the interrupted program
# This couldn't be done in C code as inline assembly because the iret line would have to 
# come before the C functions leave and ret command, thereby rendering it useless

keyboard_handler:
	pushl %ebp
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi

	call keyboard_interruption

	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popl %ebp

	iret

# Interrupt handler must have use an assembly wrapper because it's an interrupt!
# The most important instruction that this code contains is the iret command
# This iret command returns the instruction pointer back to the interrupted program
# This couldn't be done in C code as inline assembly because the iret line would have to 
# come before the C functions leave and ret command, thereby rendering it useless

clock_handler:
	pushl %ebp
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi

	call clock_interruption

	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popl %ebp

	iret

