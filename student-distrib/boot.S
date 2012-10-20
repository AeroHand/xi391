# boot.S - start point for the kernel after GRUB gives us control
# vim:ts=4 noexpandtab

#define ASM     1

#include "multiboot.h"
#include "x86_desc.h"

.text

	# Multiboot header (required for GRUB to boot us)
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC+MULTIBOOT_HEADER_FLAGS)

# Entrypoint to the kernel
.globl  start, _start

.align 4
start:
_start:
	# Make sure interrupts are off
	cli
	jmp     continue

continue:
	# Load the GDT
	# A label (gdt) preceded by a dollar sign results in an immediate operand with
	# the value of the label.
	movl	$gdt, gdt_reg+2		# gdt_reg+2 <- gdt
	lgdt	gdt_reg				# Load the GDTR

	# Load the IDT
	movl	$idt, idt_reg+2		# idt_reg+2 <- idt
	lidt	idt_reg				# Load the IDTR

	# Load CS with the new descriptor value
	ljmp    $KERNEL_CS, $keep_going
	

keep_going:
	# Set up ESP so we can have an initial stack
	movl    $0x800000, %esp

	# Set up the rest of the segment selector registers
	movw    $KERNEL_DS, %cx
	movw    %cx, %ss
	movw    %cx, %ds
	movw    %cx, %es
	movw    %cx, %fs
	movw    %cx, %gs

	# Push the parameters that entry() expects (see kernel.c):
	# eax = multiboot magic
	# ebx = address of multiboot info struct
	pushl   %ebx
	pushl   %eax

	# Jump to the C entrypoint to the kernel.
	call    entry

	# We'll never get back here, but we put in a hlt anyway.
halt:
	hlt
	jmp     halt

	# We put the GDTR here. It holds the 32-bit base address and 16-bit table limit
	# for the GDT. Thus we make space for a word and a long. We init the long above
	# with "movl $gdt, gdt_reg+2".
gdt_reg:
	.word 0x003F
	.long 0x0

gdt_reg_bottom:

	# We put the IDTR here. It holds the 32-bit base address and 16-bit table limit
	# for the IDT. Thus we make space for a word and a long. We init the long above
	# with "movl $idt, idt_reg+2".
idt_reg:
	.word 0x07FF
	.long 0x0

idt_reg_bottom:
