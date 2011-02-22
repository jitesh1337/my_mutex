#include <mymutex.h>

#ifndef __MTHREAD_COND_
#define __MYTHREAD_COND__

typedef struct mythread_condattr {
} *mythread_condattr_t;

typedef struct mythread_cond {
	mythread_queue_t head, tail;
	struct futex cond_atomicity;
} *mythread_cond_t;

extern int mythread_cond_wait(mythread_cond_t *cond, mythread_mutex_t *mymutex);
extern int mythread_cond_broadcast(mythread_cond_t *cond);
extern int mythread_cond_signal(mythread_cond_t *cond);
extern int mythread_cond_destroy(mythread_cond_t *cond);
extern int mythread_cond_init(mythread_cond_t *cond, const mythread_condattr_t *attr);

#endif
