#include <stdlib.h>
#include <errno.h>

#include "mymutex.h"

/* TODO - 100 tries on a wait-loop, sleep on futex.
 */

static int count;

int mythread_mutex_init(mythread_mutex_t *mutex, mythread_mutex_attr_t *attr)
{

	mutex->head = NULL;
	mutex->tail = NULL;

	return 0;
}

int mythread_mutex_lock(mythread_mutex_t *mutex)
{
	mythread_mutex_queue_node_t mynode, pred;

	int wait_count=0;

	mythread_enter_kernel();
	mynode = (mythread_mutex_queue_node_t)malloc(sizeof(struct mythread_mutex_queue_node));
	mythread_leave_kernel();
	
	if (mynode == NULL) {
		printf("Enter while allocating memory\n");
		return -ENOMEM;
	}
	
	mythread_enter_kernel();
	count++;
	//printf("%d\n",count);
	mythread_leave_kernel();

	mynode->locked = 0;
	futex_init(&mynode->wait_block, 0);
	mynode->next = NULL;

	pred = compare_and_swap_ptr(&mutex->tail, mynode, mutex->tail);

	if ( pred != NULL ) {
		mynode->locked = 1;
		pred->next = (mythread_queue_t)mynode;

		while (wait_count<100 && mynode->locked) wait_count++;
		
		mythread_enter_kernel();
		printf("%d %d\n",count, wait_count);
		mythread_leave_kernel();

		if ( wait_count == 100 )
			futex_down(&mynode->wait_block);

		while ( mynode->locked ) ;
	}

	mutex->head = mynode;
	
	return 0;
}

int mythread_mutex_unlock(mythread_mutex_t *mutex)
{
	mythread_mutex_queue_node_t mynode;
	mynode = mutex->head;

	if ( mynode->next == NULL ) {
		if ( compare_and_swap_ptr(&mutex->tail, NULL, mynode) == mynode ) {
			return 0;
		}
		while ( mynode->next == NULL ) ;
	}

	count--;
	futex_up(&((mythread_mutex_queue_node_t)mynode->next)->wait_block);

	((mythread_mutex_queue_node_t)mynode->next)->locked = 0;

	mynode->next = NULL;

	return 0;
}

int mythread_mutex_destroy(mythread_mutex_t *mutex)
{
	mutex->head = NULL;
	mutex->tail = NULL;

	return 0;
}
