#include <mythread.h>
#include <mymutex.h>
#include <mycond.h>
#include <limits.h>

#define	NTHREADS	100

mythread_cond_t cond;
mythread_mutex_t mut;

void *fun(void *arg)
{
	//int i;

	mythread_mutex_lock(&mut);
	printf("Will now wait\n"); fflush(stdout);
	mythread_cond_wait(&cond, &mut);
	//for(i = 0; i < INT_MAX/10; i++);
	mythread_cond_signal(&cond);
	printf("Exiting \n"); fflush(stdout);
	mythread_mutex_unlock(&mut);
	mythread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	mythread_t thread[NTHREADS];
	mythread_setconcurrency(NTHREADS+1);
	mythread_mutex_init(&mut, NULL);
	mythread_cond_init(&cond, NULL);

	for(i = 0; i < NTHREADS; i++)
		mythread_create(&thread[i], NULL, fun, NULL);
	for(i = 0; i < INT_MAX/10; i++);

	printf("Broadcast\n"); fflush(stdout);
	mythread_cond_signal(&cond);
	printf("Now join\n"); fflush(stdout);
	for(i = 0; i < NTHREADS; i++)
		mythread_join(thread[i], NULL);	
	printf("main exit\n"); fflush(stdout);

	mythread_exit(NULL);
	mythread_cond_destroy(&cond);
	mythread_mutex_destroy(&mut);
	return 1;
}
