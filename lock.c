#include "lock.h"

void acquire_lock(int *lock)
{
     while (test_and_set(1, lock)) { }
}

void release_lock(int *lock)
{
	*lock = 0;
}

void mutex_lock (mutex_t *m)
{
	acquire_lock(m->spinlock);       //mutex acquires lock

	if (m->is_held == TRUE)
	{
		//some other thread holds lock
		//enqueue current thread in m->queue
		release_lock(m->spinlock);
		// block current thread   (current thread goes in wait state)
	}
	else
	{
		m->is_held = TRUE;  // true
		// run current thread
	}
	release_lock(m->spinlock);
}

void mutex_unlock(mutex_t *m)
{
	acquire_lock(m->spinlock);
	m->is_held = FALSE; // false
/*
	if ( m->queue not empty)
	{
	   //other thread is waiting for lock
	   //dequeue next lock in m->queue
	   //unblock that thread
	}
*/
	release_lock(m->spinlock);
}



