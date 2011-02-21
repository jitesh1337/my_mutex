#include "mythread.h"

#ifndef __MYTHREAD_MUTEX__
#define __MYTHREAD_MUTEX__

typedef struct mythread_mutex_attr {
	int align;  
}mythread_mutex_attr_t;

typedef struct mythread_mutex_queue_node {
	mythread_queue_t prev, next;
	int locked;
	struct futex wait_block;
} *mythread_mutex_queue_node_t;

typedef struct mythread_mutex {
	mythread_mutex_queue_node_t head;
	mythread_mutex_queue_node_t tail;
} mythread_mutex_t;

extern int mythread_mutex_init(mythread_mutex_t *, mythread_mutex_attr_t *);
extern int mythread_mutex_lock(mythread_mutex_t *);
extern int mythread_mutex_unlock(mythread_mutex_t *);
extern int mythread_mutex_destroy(mythread_mutex_t *);

#endif
