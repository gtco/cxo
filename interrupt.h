#include "common.h"

#ifndef INTERRUPT_H
#define INTERRUPT_H

#define IDT_SIZE	49

extern const unsigned short ring0_interrupt_gate;
extern const unsigned short ring0_trap_gate;
extern const unsigned short ring3_interrupt_gate;
extern const unsigned short ring3_trap_gate;

extern const char* g_progress;
extern unsigned long g_quantum;
extern unsigned long g_displayChar;
/*
	the only difference between an interrupt gate and
	a trap gate is the way the processor handles the
	IF flag in the EFLAGS register
*/

typedef struct gate_descriptor_s {
	unsigned short low;
	unsigned short selector;
	unsigned short type;
	unsigned short high;
} gate_descriptor_t;

typedef struct interrupt_info_s {
	long edi;
	long esi;
	long ebp;
	long esp;
	long ebx;
	long edx;
	long ecx;
	long eax;
	long gs;
	long fs;
	long es;
	long ds;
	long interrupt_number;
	long error_code;
	long eip;
	long cs;
	long eflags;
//	long user_esp;
//	long user_ss;
} interrupt_info_t;

extern unsigned long g_idtr[2];

static inline unsigned long get_cr2()
{
	unsigned long cr2;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2));
	return cr2;
}

// Init the Programmable Interrupt Controller (PIC)
void init_8259();

// Send End Of Interrupt (EOI) to PIC
void send_eoi(int irq);

// Init IDT
void init_interrupt_descriptor_table();

// Interrupt Handlers
void default_handler_impl(interrupt_info_t* info);
void keyboard_handler_impl(interrupt_info_t *info);
void page_fault_handler_impl(interrupt_info_t *info);
void gp_handler_impl(interrupt_info_t *info);

#endif
