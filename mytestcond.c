#include <mythread.h>
#include <mymutex.h>
#include <mycond.h>
#include <limits.h>

mythread_cond_t cond;
mythread_mutex_t mut;

void *fun(void *arg)
{
	int i;

	mythread_mutex_lock(&mut);
	printf("Will now wait\n"); fflush(stdout);
	mythread_cond_wait(&cond, &mut);
	for(i = 0; i < INT_MAX/10; i++);
	printf("Exiting \n"); fflush(stdout);
	mythread_exit(NULL);
	return NULL;
}

int main()
{
	int i;
	mythread_t thread;
	mythread_setconcurrency(10);
	mythread_mutex_init(&mut, NULL);
	mythread_cond_init(&cond, NULL);

	mythread_create(&thread, NULL, fun, NULL);
	for(i = 0; i < INT_MAX/50; i++);

	printf("Broadcast\n"); fflush(stdout);
	mythread_cond_broadcast(&cond);
	printf("Now join\n"); fflush(stdout);
	mythread_join(thread, NULL);
	printf("main exit\n"); fflush(stdout);

	mythread_exit(NULL);
	mythread_cond_destroy(&cond);
	mythread_mutex_destroy(&mut);
	return 1;
}
