/**********************************************/
/* idt.c - The idt config part of the kernel. */
/**********************************************/
#ifndef IDT_H
#define IDT_H



#define SYSCALL_INT	0x80



/* Initialize the IDT */
void init_idt ();



#endif /* IDT_H */
