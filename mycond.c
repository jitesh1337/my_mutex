/* Single Author info:
 *      jhshah	Jitesh H  Shah
 * Group info:
 *      jhshah     Jitesh H Shah
 *      sskanitk  Salil S Kanitkar
 *      ajalgao    Aditya A Jalgaonkar
 */

#include <stdio.h>
#include <mythread.h>
#include <mycond.h>

/* Condition wait */
int mythread_cond_wait(mythread_cond_t *cond, mythread_mutex_t *mutex)
{
	/* At this stage, we are inside a critical section protected by	
	 * "mutex". So, we are safe from race conditions.
	 */
	futex_down(&(*cond)->cond_atomicity);
	mythread_enter_kernel();
	/* Remove ourselves from the run-q and add to the condition variable
	 * q
	 */
	mythread_block_phase1(&(*cond)->head, 0);

	/* In the next step, we are going to release the mutex, which will
	 * expose us to race conditions. Especially one where we are not
	 * sleeping yet and we get a signal. The signal will be lost. 
	 * Hence, enter another critical region to avoid a lost signal.
	 */
	mythread_mutex_unlock(mutex);

	/* Enter kernel. Another critical section */
	mythread_enter_kernel();
	futex_up(&(*cond)->cond_atomicity);
	mythread_block_phase2();

	/* Lock mutex before returning */
	mythread_mutex_lock(mutex);
	return 0;
}

int mythread_cond_broadcast(mythread_cond_t *cond)
{
	/* Ensure that we aren't interrupting a cond_wait. Block on
 	 * atomicity futex if we are.
 	 */
	futex_down(&(*cond)->cond_atomicity);
	mythread_enter_kernel();

	/* Wake-up all threads waiting on the condition */
	while((*cond)->head != NULL) {
		mythread_unblock(&(*cond)->head, 0);
		mythread_enter_kernel();
	}
	futex_up(&(*cond)->cond_atomicity);
	mythread_leave_kernel();
	return 0;
}

int mythread_cond_signal(mythread_cond_t *cond)
{
	/* Ensure that we aren't interruptin a cond_wait */
	futex_down(&(*cond)->cond_atomicity);
	mythread_enter_kernel();

	/* Unblock the thread at the head or if there is none, exit gracefully */
	if ((*cond)->head != NULL)
		mythread_unblock(&(*cond)->head, 0);
	else
		mythread_leave_kernel();
	futex_up(&(*cond)->cond_atomicity);
		
	return 0;
}

/* Destroy */
int mythread_cond_destroy(mythread_cond_t *cond)
{
	free(*cond);
	return 1;
}

/* Init */
int mythread_cond_init(mythread_cond_t *cond, const mythread_condattr_t *attr)
{
	*cond = (mythread_cond_t)malloc(sizeof(struct mythread_cond));
	if (cond == NULL)
		return -1;

	(*cond)->head = NULL;
	(*cond)->tail = NULL;
	futex_init(&(*cond)->cond_atomicity, 1);

	return 0;
}

