#include "common.h"

#ifndef _LOCK_H_
#define _LOCK_H_

typedef struct mutex
{
	int* spinlock;
	//Access to this struct mutex is being held
	boolean_t is_held;
	//queue of threads waiting on this mutex
	//Queue queue;
} mutex_t;

void acquire_lock(int * lock);
void release_lock(int * lock);

void mutex_lock (mutex_t *m);
void mutex_unlock(mutex_t *m);

#endif
