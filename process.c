#include "process.h"
#include "console.h"
#include "common.h"

//global pointer to current task:
process_t *g_current;
process_t g_tasks[TASK_COUNT];

process_t g_p0;

tss_t g_tss;

unsigned int stacks[TASK_COUNT][USTACKTOP+1];
unsigned int kstacks[TASK_COUNT][KSTACKTOP+1];

unsigned int g_reschedule = 0;
unsigned int g_start_scheduler = 0;

unsigned int task_num = -1;

void task_init(process_t* t, unsigned int entry,int priority, int d)
{
	unsigned int *s;

	s = &kstacks[d][KSTACKTOP];
	*s--;
	*s-- = 0x0202;					// eflags
	*s-- = KERNEL_CS;				// cs
	*s-- = (unsigned int) entry; 	// eip
	*s-- = 0;    					// ebp
	*s-- = 0;    					// esp
	*s-- = 0;    					// edi
	*s-- = 0;    					// esi
	*s-- = 0;    					// edx
	*s-- = 0;    					// ecx
	*s-- = 0;    					// ebx
	*s-- = 0;    					// eax
	*s-- = KERNEL_DS;				// ds
	*s-- = KERNEL_DS;				// es
	*s-- = KERNEL_DS;				// fs
	*s = KERNEL_DS;					// gs

	t->esp = (unsigned int) s;
	t->ss = KERNEL_SS;
	t->kstack = (unsigned int) &kstacks[d][KSTACKTOP];
	t->ustack = (unsigned int) &stacks[d][USTACKTOP];
	t->page_dir = 0; //get_page_dir()
	t->number = 1;
	t->parent = 1;
	t->owner = 1;
	t->group = 1;
	t->timetorun = 100;
	t->sleep = 0;
	t->priority = priority;
	t->filehandle = 0xff;

//	kprintf("Stack top : %x, Task esp : %x\n", &kstacks[d][KSTACKTOP], t->esp);
}


void schedule(void)
{
	if (g_start_scheduler)
	{
		++task_num;
		if (task_num >= TASK_COUNT)
		{
			task_num = 0;
		}

		g_current = &g_tasks[task_num];

		if (1)
		{
			g_reschedule = 1;
		}
	}
}
