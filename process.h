#ifndef _PROCESS_H_
#define _PROCESS_H_

#define KSTACKTOP	4096
#define USTACKTOP	4096

#define TASK_COUNT	3

/*
typedef uchar_t  unsigned char;  // -->Länge: 8 bit
typedef unsigned short unsigned short; // -->Länge: 16 bit
typedef unsigned int   unsigned int;   // -->Länge: 32 bit
typedef ulong_t  unsigned long;  // -->Länge: 64 bit
typedef void (*entry_t)(void);
*/

typedef struct {
	unsigned int esp;    //actual position of esp
	unsigned int ss;     //actual stack segment.
	unsigned int kstack; //stacktop of kernel stack
	unsigned int ustack; //stacktop of user stack
	unsigned int page_dir;
	unsigned int number;
	unsigned int parent;
	unsigned int owner;
	unsigned int group;
	unsigned int timetorun;
	unsigned int sleep;
	unsigned int priority;
	unsigned int filehandle;
//  console_t *process_console;
//  memtree_t *vmm_alloc;
//	unsigned char name[32];
} process_t;

typedef struct {
	unsigned short	backlink, __blh;
	unsigned int	esp0;
	unsigned short	ss0, __ss0h;
	unsigned int	esp1;
	unsigned short	ss1, __ss1h;
	unsigned int	esp2;
	unsigned short	ss2, __ss2h;
	unsigned int	cr3;
	unsigned int	eip;
	unsigned int	eflags;
	unsigned int	eax, ecx, edx, ebx;
	unsigned int	esp, ebp, esi, edi;
	unsigned short	es, __esh;
	unsigned short	cs, __csh;
	unsigned short	ss, __ssh;
	unsigned short	ds, __dsh;
	unsigned short	fs, __fsh;
	unsigned short	gs, __gsh;
	unsigned short	ldt, __ldth;
	unsigned short	trace, bitmap;
} tss_t;

extern process_t *g_current;
extern process_t g_tasks[];
extern process_t g_p0;

extern tss_t g_tss;

//extern unsigned **stacks;
//extern unsigned **kstacks;

unsigned int stacks[TASK_COUNT][USTACKTOP+1];
unsigned int kstacks[TASK_COUNT][KSTACKTOP+1];

extern unsigned int g_reschedule;
extern unsigned int g_start_scheduler;

extern unsigned int task_num;

void isr_switch(void);
void task_init(process_t* t, unsigned int entry,int priority, int d);
void schedule(void);

#endif

