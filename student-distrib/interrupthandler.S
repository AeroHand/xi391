# interrupthandler.S - Set up interrupt handler assembly wrappers

#define ASM	1
#include "x86_desc.h"


.global keyboard_handler
.global clock_handler
.global timer_chip_handler

# Interrupt handler must have use an assembly wrapper because it's an interrupt!
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

