/* Single Author info:
 *      sskanitk   Salil S Kanitkar
 * Group info:
 *      jhshah     Jitesh H Shah
 *      sskanitk  Salil S Kanitkar
 *      ajalgao    Aditya A Jalgaonkar
 */

#include <stdlib.h>
#include <errno.h>

#include "mymutex.h"

/* Initialise mutex. Allocate memory here */
int mythread_mutex_init(mythread_mutex_t *mutex, mythread_mutex_attr_t *attr)
{
	*mutex = (mythread_mutex_t)malloc(sizeof(struct mythread_mutex));
	if ((*mutex) == NULL)
		return -ENOMEM;

	(*mutex)->head = NULL;
	(*mutex)->tail = NULL;
	return 0;
}

/* Mutex lock */
int mythread_mutex_lock(mythread_mutex_t *mutex)
{
	mythread_mutex_queue_node_t mynode, pred;

	int wait_count=0;

	/* Possible race here. Protect non-thread-safe library function malloc */
	mythread_enter_kernel();
	mynode = (mythread_mutex_queue_node_t)malloc(sizeof(struct mythread_mutex_queue_node));
	mythread_leave_kernel();
	
	if (mynode == NULL) {
		printf("Enter while allocating memory\n");
		return -ENOMEM;
	}
	
	/* Initialise the allocated node */
	mynode->locked = 0;
	futex_init(&mynode->wait_block, 0);
	mynode->next = NULL;

	/* Put mynode to the tail */
	do {
		pred = compare_and_swap_ptr(&(*mutex)->tail, mynode, (*mutex)->tail);
	} while (pred == (*mutex)->tail);

	if (pred != NULL) {
		/* Someone already has the lock (and we put ourselves on the tail */
		mynode->locked = 1;
		pred->next = (mythread_queue_t)mynode;

		/* Try to acquire the lock for some time. If we fail, we'll sleep */
		while (wait_count<100 && mynode->locked) wait_count++;
		
		/* Failed to acquire lock. Will sleep. */
		if (wait_count == 100)
			futex_down(&mynode->wait_block);

		/* We are woken up, but there might be a window where we still
		 * haven't got the lock. So, loop on the lock for a while
		 */
		while (mynode->locked);
	}

	/* We got the lock means we are at the head of the queue */
	(*mutex)->head = mynode;
	
	return 0;
}

/* Unlock */
int mythread_mutex_unlock(mythread_mutex_t *mutex)
{
	mythread_mutex_queue_node_t mynode;

	/* Since this thread is calling unlock, it must be at the head of the queue */
	mynode = (*mutex)->head;

	/* There is no one else waiting */
	if (mynode->next == NULL) {
		if (compare_and_swap_ptr(&(*mutex)->tail, NULL, mynode) == mynode) {
			return 0;
		}
		/* Race, someone is waiting for the lock, but still hasn't
		 * updated our next pointer. Wait for sometime till our pointer
		 * gets updated.
		 */
		while (mynode->next == NULL);
	}

	/* Wake-up sleeping process */
	futex_up(&((mythread_mutex_queue_node_t)mynode->next)->wait_block);

	/* Unlock other process */
	((mythread_mutex_queue_node_t)mynode->next)->locked = 0;

	mynode->next = NULL;
	return 0;
}

/* Destroy */
int mythread_mutex_destroy(mythread_mutex_t *mutex)
{
	(*mutex)->head = NULL;
	(*mutex)->tail = NULL;

	free(*mutex);
	return 0;
}
