# interrupthandler.S - Set up interrupt handler assembly wrappers

#define ASM	1
#include "x86_desc.h"


.global keyboard_handler
.global clock_handler
.global syscall_handler
.global test_syscall
.global to_the_user_space

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

syscall_jumptable:
	.long halt
	.long execute
	.long read
	.long write
	.long open
	.long close
	.long getargs
	.long vidmap
	.long set_handler
	.long sigreturn


syscall_handler:
	pushl %ebp
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi

	pushl %edx 			#Argument 3
	pushl %ecx			#Argument 2
	pushl %ebx			#Argument 1

	addl $-1, %eax

	call *syscall_jumptable(,%eax,4)

	popl %edx 			#Argument 3
	popl %ecx			#Argument 2
	popl %ebx	

	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popl %ebp

	iret



test_syscall:
	pushl %ebp
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi

	movl	4(%EBP),%EAX

	movl	8(%EBP),%EBX  	
	movl	12(%EBP),%ECX 
	movl	16(%EBP),%EDX 
	
	int	$0x80    

	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popl %ebp

	ret


##############################################################################

#to_the_user_space:

#	movw USER_DS, %ax
#	movw %ax, %ds
#	movw %ax, %es
#	movw %ax, %fs
#	movw %ax, %gs
	
#	movl 4(%ebp), %eax
#	pushl EIP
#	pushl 
