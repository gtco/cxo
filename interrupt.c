#include "common.h"
#include "keyboard.h"
#include "console.h"
#include "interrupt.h"
#include "memory.h"
#include "process.h"

extern unsigned int g_debug;

const unsigned short ring0_interrupt_gate = 0x8e00;
const unsigned short ring0_trap_gate = 0x8f00;
const unsigned short ring3_interrupt_gate = 0xee00;
const unsigned short ring3_trap_gate = 0xef00;

const char* g_progress = "|/-\\";
unsigned long g_quantum = 0;
unsigned long g_displayChar = 0;

gate_descriptor_t g_idt[IDT_SIZE];

unsigned long g_idtr[2] = {
	// Limit
	(IDT_SIZE * sizeof(unsigned long long) - 1) << 16,
	// Base
	(unsigned long)&g_idt[0]
};

static inline void set_interrupt_handler(gate_descriptor_t *idt, int interrupt_number,
	unsigned long handler_address)
{
	idt[interrupt_number].low = (unsigned short) (handler_address & 0xffff);
	idt[interrupt_number].selector = KERNEL_CS;	// kernel code selector
	idt[interrupt_number].type= ring0_interrupt_gate;
	idt[interrupt_number].high = (unsigned short) (handler_address >> 16);
}

void init_interrupt_descriptor_table()
{
/*
	20h	32 (0) Timer		28h	40 (8) RealTime Clock
	21h	33 (1) Keyboard		29h	41 (9)
	22h	34 (2) Reserved		30h	42 (10)
	23h	35 (3) COM 2,4		31h	43 (11)
	24h	36 (4) COM 1,3		32h	44 (12) PS/2 Mouse
	25h	37 (5) LPT			33h	45 (13) i386 Coprocessor
	26h	38 (6) Floppy		34h	46 (14) Primary IDE
	27h	39 (7)				35h	47 (15)
*/
	unsigned long i = 0;

	for (i = 0; i < IDT_SIZE; i++)
	{
		set_interrupt_handler(g_idt, i, (unsigned long) ((unsigned char *) &isr_default));
	}

	set_interrupt_handler(g_idt, 13, (unsigned long) ((unsigned char *) &isr_gp_fault));
	set_interrupt_handler(g_idt, 14, (unsigned long) ((unsigned char *) &isr_page_fault));
//	set_interrupt_handler(g_idt, 32, (unsigned long) ((unsigned char *) &isr_timer));
	set_interrupt_handler(g_idt, 32, (unsigned long) ((unsigned char *) &isr_switch));
	set_interrupt_handler(g_idt, 33, (unsigned long) ((unsigned char *) &isr_keyboard));
//	set_interrupt_handler(g_idt, 48, (unsigned long) ((unsigned char *) &isr_task_switch));

    set_idtr(((unsigned char *)g_idtr) + 2);
}

void default_handler_impl(interrupt_info_t *info)
{
	TEXT_COLOR = RED;
	kprintf ("\n------ EXCEPTION ------\n");
	kprintf ("EDI = %x ESI = %x EBP = %x ESP = %x\n",
		info->edi, info->esi, info->ebp, info->esp);
	kprintf ("EBX = %x EDX = %x ECX = %x EAX = %x\n",
		info->ebx, info->edx, info->ecx, info->eax);
	kprintf ("GS = %x FS = %x ES = %x DS = %x\n",
		info->gs, info->fs, info->es, info->ds);
	kprintf ("Interrupt = %x Error code = %x\n",
		info->interrupt_number, info->error_code);
	kprintf ("EIP = %x CS = %x EFLAGS = %x\n",
		info->eip, info->cs, info->eflags);
	kprintf ("-----------------------\n");
//	TEXT_COLOR = WHITE;
	while(1);
}

void init_8259()
{
	/**
	 * The timer output frequency is obtained by dividing the 8253 master clock
	 * frequency of 1,193,180 Hz by a 16 bit divisor.  (A zero divisor is treated as
	 * 65,536.  This is the value programmed by the BIOS, to give a system timer of
	 * 18.2065 Hz.)  The timer is programmed by sending a control byte to port 43h,
	 * followed by a divisor word (low byte, high byte) to port 4Xh where X is the
	 * timer being programmed.
	 */
	unsigned short latch = 1193180/100;		// 100 times per second

    /**
     * re-program the 8259 chips so
     * IRQ 0-7 -> INT 20h-27h,
	 * IRQ 8-15 -> INT 28h-2Fh
     */
	outb(0x20, 0x11); /* ICW1 */
	outb(0xA0, 0x11);

	outb(0x21, 0x20); /* ICW2: route IRQs 0...7 to INTs 20h...27h */
	outb(0xA1, 0x28); /* ...IRQs 8...15 to INTs 28h...2Fh */

	outb(0x21, 0x04); /* ICW3 */
	outb(0xA1, 0x02);

	outb(0x21, 0x01); /* ICW4 */
	outb(0xA1, 0x01);

	/**
	 * Timer, IRQ0, interrupt 32 (0x20)
	 */
	outb(0x36, 0x43);			/* binary, mode 3, LSB/MSB, ch 0 */
	outb(latch & 0xff, 0x40);	/* LSB */
	outb(latch >> 8, 0x40);		/* MSB */

	/**
	 *  Enable IRQ1 (keyboard) and IRQ0 (timer) at the
	 *	8259 PIC chips, disable others:
	 */
 	outb(0x21, ~0x03);
    outb(0xA1, ~0x00);
}

void send_eoi(int irq)
{
    /* Handlers for IRQs must issue End Of Interrupt (EOI)
	 * to one or both PIC chips (MSTR and SLV).
	 *
	 * For IRQ 8-15, both 8259 chips must get EOI:
	 *
	 */
    if (irq >= 8 && irq <= 15)
	{
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);
}

void keyboard_handler_impl(interrupt_info_t *info)
{
	unsigned char scan_code, key_char;

	// disable irq
	outb(0x21, (inb(0x21) | 0x03));

	scan_code = inb(0x60);
	key_char = get_char(scan_code);

	if (scan_code < 0x80 && key_char < 0x80)
	{
		kprintf ("%c", key_char);
	}

	// eoi
	outb(0x20, 0x20);

	// enable irq
	outb(0x21, (inb(0x21) & 0xFC));
	return;
}

void page_fault_handler_impl(interrupt_info_t *info)
{
	TEXT_COLOR = YELLOW;
	kprintf ("\n------ PAGE FAULT ------\n");
	kprintf ("EDI = %x ESI = %x EBP = %x ESP = %x\n",
		info->edi, info->esi, info->ebp, info->esp);
	kprintf ("EBX = %x EDX = %x ECX = %x EAX = %x\n",
		info->ebx, info->edx, info->ecx, info->eax);
	kprintf ("GS = %x FS = %x ES = %x DS = %x\n",
		info->gs, info->fs, info->es, info->ds);
	kprintf ("Interrupt = %x \n", info->interrupt_number);
	kprintf ("CR2 = 0x0%x\n", get_cr2());
	kprintf ("Error code = %x ", info->error_code);

	info->error_code & 0x01 ? kprintf ("(User, ") : kprintf ("(Supervisor, ");
	info->error_code & 0x02 ? kprintf ("Write, ") : kprintf ("Read, ");
	info->error_code & 0x04 ? kprintf ("Protection)\n") : kprintf ("Not Present)\n");

	kprintf ("EIP = %x CS = %x EFLAGS = %x\n",
		info->eip, info->cs, info->eflags);

	while(1);
}

void gp_handler_impl(interrupt_info_t *info)
{
	TEXT_COLOR = CYAN;
	kprintf ("\n\n------ GENERAL PROTECTION ------\n");
	kprintf ("EDI = %x ESI = %x EBP = %x ESP = %x\n",
		info->edi, info->esi, info->ebp, info->esp);
	kprintf ("EBX = %x EDX = %x ECX = %x EAX = %x\n",
		info->ebx, info->edx, info->ecx, info->eax);
	kprintf ("GS = %x FS = %x ES = %x DS = %x\n",
		info->gs, info->fs, info->es, info->ds);
	kprintf ("Interrupt = %x Error code = %x\n",
		info->interrupt_number, info->error_code);
	kprintf ("EIP = %x CS = %x EFLAGS = %x\n",
		info->eip, info->cs, info->eflags);
	while(1);
}

void timer_handler_impl(interrupt_info_t *info)
{
	// disable irq
//	outb(0x21, (inb(0x21) | 0x03));

	// Acknowledge interrupt - EOI
	outb(0x20, 0x20);

	if (g_quantum % 10 == 0)
	{
/*
		if (g_displayChar > 3)
		{
			g_displayChar = 0;
		}
		write_char(156, g_progress[g_displayChar], BRIGHT_BLUE);
		++g_displayChar;
*/

		schedule();
	}
	++g_quantum;


	// enable irq
//	outb(0x21, (inb(0x21) & 0xFC));

	return;
}

