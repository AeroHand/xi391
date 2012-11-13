# interrupthandler.S - Set up interrupt handler assembly wrappers

#define ASM     1
#include "x86_desc.h"

.global keyboard_handler
.global clock_handler
.global syscall_handler
.global test_syscall

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


# A jump table to C functions that implement the system calls themselves.
syscall_jumptable:
	.long 0x0
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

# Saves registers and jumps to respective C-implemented system call function.
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

	movl syscall_jumptable(,%eax,4),%eax
	call *%eax

	addl $12,%esp		# pop the args
	
	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popl %ebp

	iret


# Test wrapper: preps for an soft interrupt and does an "int $0x80".
test_syscall:
	pushl %ebp
	movl	%esp, %ebp

	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi

	MOVL	8(%EBP),%EAX	# put syscallnum into eax
	MOVL	12(%EBP),%EBX  	# put param1 into ebx
	MOVL	16(%EBP),%ECX 	# put param2 into ecx
	MOVL	20(%EBP),%EDX 	# put param3 into edx
	
	INT	$0x80         

	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax

	leave
	ret


####################################

to_the_user_space:

	cli

	pushl %ebp		  	#save old frame pointer
	movl %esp,%ebp 		#point to new frame
	
	movl 8(%ebp),%ebx 	#This is getting the first argument aka EIP

	movw USER_DS, %ax 	#load segement selectors
	movw %ax, %gs 
	movw %ax, %fs  
	movw %ax, %ds 
	movw %ax, %es

	push %ax 			

	pushl 0x083FFFF0

	pushf
	popl %eax
	or $0x4000, %eax 
	pushl %eax
	pushl USER_CS
	pushl %ebx

	iret
