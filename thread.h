#include "console.h"

#ifndef THREAD_H
#define THREAD_H

extern int TASK_READY;
extern int TASK_RUNNING;
extern int TASK_WAITING;

typedef struct thread_s {
    unsigned long esp;
    volatile unsigned long num_ticks;
    int priority;
    int state;
    void *stack_page;
    int ref_count;
} thread_t;


// Push a dword value on the stack of given thread.
// We use this to set up some context for the thread before
// we make it runnable.
static inline void Push( thread_t* t, unsigned long value )
{
    t->esp -= 4;
    *((unsigned long *) t->esp) = value;
//    kprintf("%x = %x\n", t->esp, value);
}

thread_t* create_thread(int priority);
void init_thread(thread_t* t, void* stack_page, int priority);
thread_t* setup_kernel_thread(unsigned long entry_point, int priority );

void schedule(void);

unsigned long get_new_context(void * p);

#endif


