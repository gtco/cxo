#include "multiboot.h"
#include "common.h"
#include "console.h"
#include "library.h"
#include "interrupt.h"
#include "build_info.h"
#include "memory.h"
#include "process.h"
#include "glib.h"

void kernel_main(struct multiboot_info *mbi);

const unsigned short KERNEL_CS   =     0x08;
const unsigned short KERNEL_SS   =     0x10;
const unsigned short KERNEL_DS   =     0x10;
const unsigned short USER_CS     =     0x18;
const unsigned short USER_DS     =     0x20;

unsigned long long g_gdt[GDT_SIZE] = {

    0x0000000000000000LL,  // NULL SEGMENT
    0x00cf98000000ffffLL,  // KERNEL_CS
    0x00cf92000000ffffLL,  // KERNEL_SS
    0x00cff8000000ffffLL,  // USER_CS
    0x00cff2000000ffffLL   // USER_DS
};

unsigned long g_gdtr[2] = {
    // Limit
    (GDT_SIZE * sizeof(unsigned long long) - 1) << 16,
    // Base
    (unsigned long)&g_gdt[0]
};

char p1[] = "The quick brown fox.";

char p2[128];

char cat[] = "cat";
char dog[] = "dog";
char mouse[] = "mouse";
char turtle[] = "turtle";
char zebra[] = "zebra";


void busy0();
void busy1();
void busy2();

void kernel_init(struct multiboot_info *mbi)
{
	GList *gl;
	int x = 0;
    set_gdtr(((unsigned char *)g_gdtr) + 2);

    clear_screen();
    TEXT_COLOR = BRIGHT_WHITE;

    kprintf("%s (Build %s), %s\n", BUILD_NAME, BUILD_VER, BUILD_DATE);
    kprintf("%s\n", BUILD_HOST);
//  print_mbi(mbi);

    TEXT_COLOR = WHITE;

    init_8259();

    init_interrupt_descriptor_table();

    if (check_flag (mbi->flags, 0))
    {
        allocator_init(mbi->mem_upper*1024);
    }
    else
    {
        kprintf("Failed to initialize memory allocator.\n");
    }

	gl = g_list_alloc();
	gl = g_list_append (gl, &cat);
	gl = g_list_append (gl, &dog);
	gl = g_list_append (gl, &mouse);
	gl = g_list_append (gl, &turtle);

	x =  g_list_index(gl, &dog);
	kprintf("dog = %d\n", x);
	x =  g_list_index(gl, &cat);
	kprintf("cat = %d\n", x);
	x =  g_list_index(gl, &zebra);
	kprintf("zebra = %d\n", x);
	x =  g_list_index(gl, &turtle);
	kprintf("turtle = %d\n", x);

	g_mem_profile();

	task_init(&g_tasks[0], (unsigned int) &busy0, 99, 0);
	task_init(&g_tasks[1], (unsigned int) &busy1, 99, 1);
	task_init(&g_tasks[2], (unsigned int) &busy2, 99, 2);

	g_current = &g_p0;
	g_start_scheduler = 1;

	while (1)
	{
		write_char(140, 'a' + (++x%26), PINK);
	}

}

void busy0()
{
	unsigned int x = 0;
	while (1)
	{
		write_char(144, 'a' + (++x%26), YELLOW);
	}
}

void busy1()
{
	unsigned int x = 0;
	while (1)
	{
		write_char(148, 'a' + (++x%26), GREEN);
	}
}

void busy2()
{
	unsigned int x = 0;
	while (1)
	{
		write_char(152, 'a' + (++x%26), CYAN);
	}
}

/* @todo

    * threads
    * message passing
    * signals
    * clocks
    * timers
    * interrupt handlers
    * semaphores
    * mutual exclusion locks (mutexes)
    * condition variables (condvars)
    * barriers.
*/

