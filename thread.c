#include "thread.h"
#include "memory.h"
#include "console.h"
#include "common.h"

extern unsigned int g_reschedule;
extern thread_t* t1;

int TASK_READY 		= 0x10;
int TASK_RUNNING	= 0x20;
int TASK_WAITING	= 0x30;

// Initialize a new Kernel_Thread.
void init_thread( thread_t* t, void* stack_page, int priority)
{
    memset( t, '\0', sizeof(*t) );
    t->stack_page = stack_page;
    t->esp = ((uint32_t) t->stack_page) + 4096;
    t->num_ticks = 0;
    t->priority = priority;
	t->ref_count = 1;
}

// Create a new raw thread object.
// Returns a null pointer if there isn't enough memory.
thread_t* create_thread(int priority)
{
    thread_t* t = 0;
    void* stack_page = 0;

	//  interrupts ?

    // For now, just allocate one page each for the thread context
    // object and the thread's stack.
    t = (thread_t *) page_malloc();
	if ( t == 0 )
	{
		kprintf( "create_thread : thread allocation failed!\n" );
		return 0;
	}

   	stack_page = (void *) page_malloc();
    // Make sure that the memory allocations succeeded.
    if ( stack_page == 0 )
    {
		kprintf( "create_thread : stack allocation failed!\n" );
		return 0;
    }

    // Initialize the stack pointer of the new thread
    // and accounting info
    init_thread(t, stack_page, priority);

    kprintf( "create_thread : %x, base (%x) , top (%x) \n ",
    	t,
    	t->stack_page,
    	t->esp );

    return t;
}

// Set up the initial context for a kernel-mode-only thread.
thread_t* setup_kernel_thread(unsigned long entry_point, int priority )
{
	thread_t* t = create_thread(priority);
    if ( t != 0 )
    {
		unsigned long eflags;

		// To make the thread schedulable, we need to make it look
		// like it was suspended by an interrupt.  This means pushing
		// an "eflags, cs, eip" sequence onto the stack,
		// as well as int num, error code, saved registers, etc.

		// Use current value of eflags, except clear the interrupt flag
		// so the thread starts execution with interrupts disabled.
		// (The Schedule() function assumes that Switch_To_Thread()
		// will return with interrupts disabled.)
		eflags = get_flags();
		eflags &= ~(0x200);
		Push( t, eflags );
		// Code Segment
		Push( t, 0x08 );
		// Entry Point (EIP)
		Push( t, entry_point );
		// Push fake error code and interrupt number.
		Push( t, 0xaa );
		Push( t, 0xee );
		// Push values for saved segment registers.
		Push( t, 0x10 ); // ds
		Push( t, 0x10 ); // es
		Push( t, 0 ); // fs
		Push( t, 0 ); // gs
		// Push initial values for general-purpose registers.
		Push( t, 0 ); // eax
		Push( t, 0 ); // ebx
		Push( t, 0 ); // edx
		Push( t, 0 ); // edx
		Push( t, 0 ); // esi
		Push( t, 0 ); // edi
		Push( t, 0 ); // ebp
    }

    return t;
}

void schedule( void )
{
	int n = 0;
//    struct Kernel_Thread* runnable;

    // Make sure interrupts really are disabled
//    KASSERT( !Interrupts_Enabled() );

    // Get next thread to run from the run queue
//    runnable = Get_Next_Runnable();

    // Activate the new thread, saving the context of the current thread.
    // Eventually, this thread will get re-activated and Switch_To_Thread()
    // will "return", and then Schedule() will return to wherever
    // it was called from.
//    Switch_To_Thread( runnable );

	if (g_reschedule)
	{
		g_reschedule = 0;

		__asm__ __volatile__ (
		"int $0x30"
		:
		: "a" (n)	// system call number specified in eax
		);
	}
}

unsigned long get_new_context(void * p)
{
	unsigned long ctx = 0;
	ctx = t1->esp;
	return ctx;
}