#######################################################################
## interrupthandler.h -  Set up interrupt handler assembly wrappers. ##
#######################################################################

#define ASM     1
#include "x86_desc.h"

.global keyboard_handler
.global clock_handler
.global pit_handler
.global syscall_handler
.global test_syscall
.global to_the_user_space



# keyboard_handler()
# The interrupt wrapper for a keyboard generated interrupt.
# Interrupt handler must have use an assembly wrapper because it's an interrupt!
# The most important instruction that this code contains is the iret command
# This iret command returns the instruction pointer back to the interrupted program
# This couldn't be done in C code as inline assembly because the iret line would have to 
# come before the C functions leave and ret command, thereby rendering it useless
# Inputs   : none
# Outputs  : none
# Registers: saves and restores ebp, eax, ebx, ecx, edx, edi, esi, fl
keyboard_handler:
	pushl %ebp
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
	pushfl

	call keyboard_interruption

	popfl
	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popl %ebp

	iret

# clock_handler()
# The interrupt wrapper for a rtc generated interrupt.
# Interrupt handler must have use an assembly wrapper because it's an interrupt!
# The most important instruction that this code contains is the iret command
# This iret command returns the instruction pointer back to the interrupted program
# This couldn't be done in C code as inline assembly because the iret line would have to 
# come before the C functions leave and ret command, thereby rendering it useless
# Inputs   : none
# Outputs  : none
# Registers: saves and restores ebp, eax, ebx, ecx, edx, edi, esi, fl
clock_handler:
	pushl %ebp
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
	pushfl

	call clock_interruption

	popfl
	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popl %ebp

	iret

# clock_handler()
# The interrupt wrapper for a PIT generated interrupt.
# Interrupt handler must have use an assembly wrapper because it's an interrupt!
# The most important instruction that this code contains is the iret command
# This iret command returns the instruction pointer back to the interrupted program
# This couldn't be done in C code as inline assembly because the iret line would have to 
# come before the C functions leave and ret command, thereby rendering it useless
# Inputs   : none
# Outputs  : none
# Registers: saves and restores ebp, eax, ebx, ecx, edx, edi, esi, fl
pit_handler:
	pushl %ebp
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
	pushfl

	call pit_interruption

	popfl
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

# syscall_handler()
# Saves registers and jumps to respective C-implemented system call function.
# Inputs   :
# Outputs  :
# Registers:
syscall_handler:
#	pushl %xes
#	pushl %xds
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx

	pushl %edx 			# Argument 3
	pushl %ecx			# Argument 2
	pushl %ebx			# Argument 1

	cmpl $1, %eax		# Check that eax is greater than 1
	jl bad_eax
	cmpl $10, %eax		# Check that eax is less than 10
	jg bad_eax
	
	movl syscall_jumptable(,%eax,4),%eax
	call *%eax
	jmp end_syscall
	
bad_eax:
	movl $-1, %eax

end_syscall:
	addl $12,%esp		# Pop the args
	
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
#	popl %xds
#	popl %xes

	iret

# test_syscall()
# Test wrapper - preps for an soft interrupt and does an "int $0x80".
test_syscall:
	pushl %ebp
	movl	%esp, %ebp

	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi

	MOVL	8(%EBP),%EAX	# Put syscallnum into eax
	MOVL	12(%EBP),%EBX  	# Put param1 into ebx
	MOVL	16(%EBP),%ECX 	# Put param2 into ecx
	MOVL	20(%EBP),%EDX 	# Put param3 into edx
	
	INT	$0x80         

	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax

	leave
	ret



# to_the_user_space()
# 
# Inputs   :
# Outputs  :
# Registers:
to_the_user_space:

	cli
	
	movl 4(%esp),%ebx 		# Extended Instruction Pointer -> ebx

	movl $USER_DS, %eax 	# Load segement selectors
	movw %ax, %gs 
	movw %ax, %fs  
	movw %ax, %ds 
	movw %ax, %es

	pushl %eax

	pushl $0x83FFFF0

	pushf
	popl %eax
	orl $0x4200, %eax 
	pushl %eax
	pushl $USER_CS
	pushl %ebx

	iret
