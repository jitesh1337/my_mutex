#include "mythread.h"

typedef struct mythread_mutex_attr {
	int align;  
}mythread_mutex_attr_t;

typedef struct mythread_mutex {
	
	mythread_queue_t head;
	mythread_queue_t tail;

}mythread_mutex_t;

extern int mythread_mutex_init(mythread_mutex_t *, mythread_mutex_attr_t *);
extern int mythread_mutex_lock(mythread_mutex_t *);
extern int mythread_mutex_unlock(mythread_mutex_t *);
extern int mythread_mutex_destroy(mythread_mutex_t *);

