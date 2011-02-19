#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mythread.h>
#include "mymutex.h"

/* Number of threads to start */
#define NTHREADS	100

/* This function will first increment count by 50, yield. When it gets the 
 * control back, it will increment count again and then exit
 */
int glb=0;

mythread_mutex_t mut;

void initialize()
{
        mythread_mutex_init(&mut, NULL);
}

void *thread_func(void *arg)
{
	int *count = (int *)arg;

	mythread_t me = mythread_self();
	/*
	*count = *count + 50;

	mythread_enter_kernel();
	printf("Thread %ld: adds 50 : %d\n", (long int)(me->tid), *count);
	mythread_leave_kernel();

	*count = *count + 50;

	mythread_enter_kernel();
	printf("Thread %ld: again adds 50 : %d\n", (long int)(me->tid), *count);
	mythread_leave_kernel();
	*/
	mythread_mutex_lock(&mut);
	//printf("ekde!\n");	
	//mythread_yield();
	glb += 1;
	mythread_mutex_unlock(&mut);

	mythread_mutex_lock(&mut);
	glb += 1;
	mythread_mutex_unlock(&mut);

	mythread_mutex_lock(&mut);
	glb += 1;
	mythread_mutex_unlock(&mut);

	mythread_mutex_lock(&mut);
	glb += 1;
	mythread_mutex_unlock(&mut);

	/*mythread_enter_kernel();
	printf("Thread %d incremented glb to : %d\n",(long int)(me->tid), glb);
	mythread_leave_kernel();
	*/
	mythread_exit(NULL);
	return NULL;
}

/* This is a simple demonstration of how to use the mythread library.
 * Start NTRHEADS number of threads, collect count value and exit
 */
int main()
{
	mythread_t threads[NTHREADS];
	int count[NTHREADS];
	int i;
	char *status;
	
	mythread_setconcurrency(NTHREADS);

	initialize();

	for (i = 0; i < NTHREADS; i++) {
		count[i] = i;
		mythread_create(&threads[i], NULL, thread_func, &count[i]);
	}

	for (i = 0; i < NTHREADS; i++) {
		mythread_join(threads[i], (void **)&status);
	}

	mythread_mutex_destroy(&mut);

	printf("Final glb val: %d\n",glb);

	mythread_exit(NULL);

	return 0;
}
