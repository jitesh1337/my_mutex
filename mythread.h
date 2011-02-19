/*
 * mythread.h -- interface of user threads library
 */

#ifndef MYTHREAD_H
#define MYTHREAD_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#include <pthread.h>
#include "myqueue.h"
#include "futex.h"

typedef struct mythread_attr { /* thread attributes */
  int attr;                    /* not yet used */
} mythread_attr_t;

typedef struct mythread {      /* thread control block */
  pid_t tid;
  struct futex block;
  void *stack;
  int state;                   /* state of execution */
  void * (*start_func)(void*); /* thread_func to be called */
  void *arg;                   /* thread_arg arguments of thread_func */
  mythread_queue_t joinq;      /* Q of threads waiting for my return */
  void *returnValue;           /* Pointer to detached function's return value */
} *mythread_t;

/*
 * mythread_setconcurrency - set the number of LWPs
 * (max. number of parallel threads)
 */
void mythread_setconcurrency(int new_level);

/*
 * mythread_enter_kernel - enter the monolithic threading kernel
 */
void mythread_enter_kernel(void);

/*
 * mythread_leave_kernel - leave the monolithic threading kernel
 */
void mythread_leave_kernel(void);

/*
 * mythread_self - thread id of running thread
 */
mythread_t mythread_self(void);

/*
 * mythread_block - remove currently running thread off run q,
 * if q is non-NULL, enq this thread on q, suspend the thread,
 * add the state flags to the thread's state (via bit-wise OR | state)
 * and activate ready threads if an LWP becomes available
 * pre: must be in monolithic kernel
 * post: return outside the monolithic kernel
 */
void mythread_block(mythread_queue_t *q, int state);

/*
 * mythread_unblock - resumes the thread at the head of q,
 * remove the state flags to the thread's state (via bit-wise AND & ~state)
 * and activate the thread if an LWP becomes available (o/w mark as ready)
 * pre: must be in monolithic kernel
 * post: return outside the monolithic kernel
 */
void mythread_unblock(mythread_queue_t *q, int state);

/*
 * mythread_block_phase1 - prepare to block (see block op below):
 * remove currently running thread off run q,
 * if q is non-NULL, enq this thread on q but DO NOT suspend the thread,
 * add the state flags to the thread's state (via bit-wise OR | state)
 * pre: must be in monolithic kernel
 * post: return outside the monolithic kernel
 * NOTICE: use in conjunction with block as follows:
 *   mythread_enter_kernel();
 *   mythread_block_phase1(q, state);
 *   some_other_operation();
 *   mythread_enter_kernel();
 *   mythread_block_phase2();
 */
void mythread_block_phase1(mythread_queue_t *q, int state);

/*
 * mythread_block_phase2 - commit to block (see block op below):
 * suspend the thread,
 * and activate ready threads if an LWP becomes available
 * pre: must be in monolithic kernel
 * post: return outside the monolithic kernel
 */
void mythread_block_phase2(void);

/*
 * mythread_create - prepares context of new_thread_ID as start_func(arg),
 * attr is ignored right now.
 * Threads are activated (run) according to the number of available LWPs
 * or are marked as ready.
 */
int mythread_create(mythread_t *new_thread_ID,
		    const mythread_attr_t *attr,
		    void * (*start_func)(void *),
		    void *arg);

/*
 * mythread_yield - switch from running thread to the next ready one
 */
int mythread_yield(void);

/*
 * mythread_join - suspend calling thread if target_thread has not finished,
 * enqueue on the join Q of the target thread, then dispatch ready thread;
 * once target_thread finishes, it activates the calling thread / marks it
 * as ready.
 */
int mythread_join(mythread_t target_thread, void **status);

/*
 * mythread_exit - exit thread, awakes joiners on return
 * from thread_func and dequeue itself from run Q before dispatching run->next
 */
void mythread_exit(void *retval);

#endif /* MYTHREAD_H */
