#include "mythread.h"

/* TODO : Make locked and wait_block part of mythread_queue_t struct.
 */
typedef struct mythread_mutex_attr {
	int align;  
}mythread_mutex_attr_t;

typedef struct mythread_mutex_q {

	struct mythread_mutex_q *prev, *next;
	int locked;
	struct futex wait_block;
	
}mythread_mutex_q_t;

typedef struct mythread_mutex {
	
	mythread_mutex_q_t *head;
	mythread_mutex_q_t *tail;

}mythread_mutex_t;

extern int mythread_mutex_init(mythread_mutex_t *, mythread_mutex_attr_t *);
extern int mythread_mutex_lock(mythread_mutex_t *);
extern int mythread_mutex_unlock(mythread_mutex_t *);
extern int mythread_mutex_destroy(mythread_mutex_t *);

