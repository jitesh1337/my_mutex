/* Single Author info:
 *      jhshah	Jitesh H  Shah
 * Group info:
 *      jhshah     Jitesh H Shah
 *      sskanitk  Salil S Kanitkar
 *      ajalgao    Aditya A Jalgaonkar
 */

#include <mythread.h>
#include <mymutex.h>
#include <mycond.h>
#include <limits.h>

#define	NTHREADS	100

mythread_cond_t cond;
mythread_mutex_t mut;

void *fun(void *arg)
{

	mythread_mutex_lock(&mut);
	printf("[%lx] Waiting for a signal\n", (unsigned long)mythread_self()); fflush(stdout);
	mythread_cond_wait(&cond, &mut);
	mythread_cond_signal(&cond);
	printf("[%lx] Exiting \n", (unsigned long)mythread_self()); fflush(stdout);
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

	mythread_cond_signal(&cond);
	for(i = 0; i < NTHREADS; i++)
		mythread_join(thread[i], NULL);	
	printf("Main exit\n"); fflush(stdout);

	printf("What this test was about:\n");
	printf("%d threads wait on a condition variable. Main does a signal and each woken up threads continue to signal other sleeping threads till all of them exit\n", NTHREADS);
	mythread_exit(NULL);
	mythread_cond_destroy(&cond);
	mythread_mutex_destroy(&mut);
	return 1;
}
