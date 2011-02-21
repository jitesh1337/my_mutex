#include <stdio.h>
#include <mythread.h>
#include <mycond.h>

int mythread_cond_wait(mythread_cond_t *cond, mythread_mutex_t *mutex)
{
	mythread_enter_kernel();
	mythread_block_phase1(&(*cond)->head, 0);

	mythread_mutex_unlock(mutex);

	mythread_enter_kernel();
	mythread_block_phase2();

	mythread_mutex_lock(mutex);
	return 0;
}

int mythread_cond_broadcast(mythread_cond_t *cond)
{
	mythread_enter_kernel();
	while((*cond)->head != NULL) {
		mythread_unblock(&(*cond)->head, 0);
		mythread_enter_kernel();
	}
	mythread_leave_kernel();
	return 0;
}

int mythread_cond_signal(mythread_cond_t *cond)
{
	mythread_enter_kernel();
	if ((*cond)->head != NULL)
		mythread_unblock(&(*cond)->head, 0);
	else
		mythread_leave_kernel();
		
	return 0;
}

int mythread_cond_destroy(mythread_cond_t *cond)
{
	free(*cond);
	return 1;
}

int mythread_cond_init(mythread_cond_t *cond, const mythread_condattr_t *attr)
{
	*cond = (mythread_cond_t)malloc(sizeof(struct mythread_cond));
	if (cond == NULL)
		return -1;

	(*cond)->head = NULL;
	(*cond)->tail = NULL;

	return 0;
}

