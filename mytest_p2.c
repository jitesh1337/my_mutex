#include <stdio.h>
#include <unistd.h>

#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include "board.h"

#include "mythread.h"
#include "mymutex.h"
#include "mycond.h"

#include <sys/syscall.h>

#define MAX_THREAD_NUM 200

#define NUM_TO_BOARD 4

int iHacker=1;
int iSerf=0;

int boardHackerCount;
int boardSerfCount;

int deplaneCount;

mythread_mutex_t for_Boarding_mut;
mythread_cond_t for_Boarding_cond;

mythread_mutex_t for_RowBoat_mut;

mythread_mutex_t for_Deplaning_mut;
mythread_cond_t for_Deplaning_cond;

mythread_mutex_t for_Restart_mut;
mythread_cond_t for_Restart_cond;

mythread_mutex_t for_Invalid_mut;
mythread_cond_t for_Invalid_cond;

mythread_t rower_t;
pid_t rower_p;

int is_it_safe_to_board_a_Hacker()
{
  if ( boardHackerCount == 2 && boardSerfCount == 1 ) 
    return 1;
  else 
    return 0;
}

int is_it_safe_to_board_a_Serf()
{
  if ( boardHackerCount == 3 && boardSerfCount == 0 ) 
    return 1;
  else 
    return 0;
}

int invalidCond=0;

int convertToSerf=0;

int boatTrip=0;

int myleave=0;

void* hacker(void *arg);
void* serf(void *arg);

void* hacker(void *arg)
{
  //struct timeval now;
  //struct timespec timeout;
  //int retcode;

  while(1) {  
    
    //printf("1\n");
    if ( (boardHackerCount+boardSerfCount) == NUM_TO_BOARD) {
      mythread_mutex_lock(&for_Restart_mut);
      while ( (boardHackerCount+boardSerfCount) == NUM_TO_BOARD) {
	mythread_cond_wait(&for_Restart_cond, &for_Restart_mut);
	if ( myleave == 1 ) {
	mythread_mutex_unlock(&for_Restart_mut);
	  if ( convertToSerf == 1 )
	    leave((void *)&iSerf);
	  else
	    leave((void *)&iHacker);
	  mythread_exit(NULL);
	}
      }
      mythread_mutex_unlock(&for_Restart_mut);
    }
    //printf("2\n");
    mythread_mutex_lock(&for_Invalid_mut);
    if ( is_it_safe_to_board_a_Hacker() ) {
      invalidCond=1;
      while( (invalidCond==1) ) {
	mythread_cond_wait(&for_Invalid_cond, &for_Invalid_mut);
	if ( myleave == 1 ) {
	 mythread_mutex_unlock(&for_Invalid_mut);
	  leave((void *)&iHacker);
	  mythread_exit(NULL);
	}
      }
    }
    mythread_mutex_unlock(&for_Invalid_mut);

    //printf("3\n");
    mythread_mutex_lock(&for_Boarding_mut);
    if ( convertToSerf ==1 )
      boardSerfCount++;
    else
      boardHackerCount++;
    if ( (boardHackerCount+boardSerfCount) == NUM_TO_BOARD)
      mythread_cond_broadcast(&for_Boarding_cond);
    mythread_mutex_unlock(&for_Boarding_mut);
    //printf("4\n");

    mythread_mutex_lock(&for_Boarding_mut);
    while( (boardHackerCount+boardSerfCount) != NUM_TO_BOARD ) {
      mythread_cond_wait(&for_Boarding_cond, &for_Boarding_mut);
      if ( myleave == 1 ) {
	  mythread_mutex_unlock(&for_Boarding_mut);
	  if ( convertToSerf == 1 )
	    leave((void *)&iSerf);
	  else
	    leave((void *)&iHacker);
	mythread_exit(NULL);
      }
    }

    /*    if ( retcode == ETIMEDOUT ) {
      if ( convertToSerf == 1 )
	leave((void *)&iSerf);
      else
	leave((void *)&iHacker);
      boardHackerCount--;
      mythread_mutex_unlock(&for_Boarding_mut);  
      mythread_cond_broadcast(&for_Restart_cond);
      mythread_mutex_unlock(&for_Restart_mut);
      invalidCond=0;
      mythread_cond_broadcast(&for_Invalid_cond);
      mythread_exit(NULL);
      }*/
    mythread_mutex_unlock(&for_Boarding_mut);  
    //printf("5\n");

    mythread_mutex_lock(&for_Boarding_mut);
    if ( convertToSerf == 1 ) 
      board((void *)&iSerf);
    else
      board((void *)&iHacker);
    deplaneCount--;
    if (deplaneCount == 0) {
      mythread_cond_broadcast(&for_Boarding_cond);
    }
    mythread_mutex_unlock(&for_Boarding_mut);
  
    //printf("6\n");
    mythread_mutex_lock(&for_Boarding_mut);
    while( deplaneCount > 0 )
      mythread_cond_wait(&for_Boarding_cond, &for_Boarding_mut);
    mythread_mutex_unlock(&for_Boarding_mut);  

    //printf("7\n");
    mythread_mutex_lock(&for_RowBoat_mut);
    if ( rower_p == 0 ) {
      rower_t = mythread_self();
      //printf("1 %d\n",rower_t->tid);
      rower_p = rower_t->tid;
    }
    //printf("8\n");
    mythread_t temp=mythread_self();
    if ( rower_p == temp->tid ) {
      if ( convertToSerf == 1)
	rowBoat((void *)&iSerf);
      else
	rowBoat((void *)&iHacker);
      boatTrip=1;
      if ( boardHackerCount == 1 && boardSerfCount == 3 ) 
	convertToSerf=1;
    }
    mythread_mutex_unlock(&for_RowBoat_mut);
    //printf("9\n");
    mythread_mutex_lock(&for_Deplaning_mut);
    temp = mythread_self();
    while ( rower_p == temp->tid && peopleOnBoard()!=1 ) 
      mythread_cond_wait(&for_Deplaning_cond, &for_Deplaning_mut);
    mythread_mutex_unlock(&for_Deplaning_mut);    
    //printf("10\n");
    while ( boatTrip == 0 ) ;
      deplane((void *)&iHacker);

    if (peopleOnBoard() == 1) 
      mythread_cond_signal(&for_Deplaning_cond);

    temp = mythread_self();
    if ( rower_p != temp->tid ) {
      mythread_exit(NULL);
    }
    else {
      boardHackerCount=0;
      boardSerfCount=0;
      deplaneCount = NUM_TO_BOARD;
      boatTrip=0;
      //convertToSerf=0;
      mythread_cond_broadcast(&for_Restart_cond);
      invalidCond=0;
      mythread_cond_broadcast(&for_Invalid_cond);
    }

  } //while 1 loop

}

void* serf(void *arg)
{

  //struct timeval now;
  //struct timespec timeout;
  //int retcode;

  while(1) {

    if ((boardHackerCount+boardSerfCount) == NUM_TO_BOARD) {
      mythread_mutex_lock(&for_Restart_mut);
      while ( (boardHackerCount+boardSerfCount) == NUM_TO_BOARD) {
	mythread_cond_wait(&for_Restart_cond, &for_Restart_mut);
	if ( myleave == 1 ) {
	 mythread_mutex_unlock(&for_Restart_mut);
	  leave((void *)&iSerf);
	  mythread_exit(NULL);
	}
    }
      mythread_mutex_unlock(&for_Restart_mut);
    }

    mythread_mutex_lock(&for_Invalid_mut);
    if ( is_it_safe_to_board_a_Serf() ) {
      invalidCond=1;
      while( (invalidCond==1) ) {
	mythread_cond_wait(&for_Invalid_cond, &for_Invalid_mut);
	if ( myleave == 1 ) {
	mythread_mutex_unlock(&for_Invalid_mut);
	  leave((void *)&iSerf);
	  mythread_exit(NULL);
	}
      }
    }
    mythread_mutex_unlock(&for_Invalid_mut);

    mythread_mutex_lock(&for_Boarding_mut);
    boardSerfCount++;
    if ( (boardHackerCount+boardSerfCount) == NUM_TO_BOARD)
      mythread_cond_broadcast(&for_Boarding_cond);
    mythread_mutex_unlock(&for_Boarding_mut);

    mythread_mutex_lock(&for_Boarding_mut);
    while( (boardHackerCount+boardSerfCount) != NUM_TO_BOARD ) {
      mythread_cond_wait(&for_Boarding_cond, &for_Boarding_mut);
      if ( myleave == 1 ) {
	mythread_mutex_unlock(&for_Boarding_mut);  
	leave((void *)&iSerf);
	mythread_exit(NULL);
      }
    }

    /*
    if ( retcode == ETIMEDOUT ) {
      leave((void *)&iSerf);
      boardSerfCount--;
      mythread_mutex_unlock(&for_Boarding_mut);  
      mythread_cond_broadcast(&for_Restart_cond);
      mythread_mutex_unlock(&for_Restart_mut);
      invalidCond=0;
      mythread_cond_broadcast(&for_Invalid_cond);
      mythread_exit(NULL);
      }*/
    mythread_mutex_unlock(&for_Boarding_mut);  

    mythread_mutex_lock(&for_Boarding_mut);
    board((void *)&iSerf);
    deplaneCount--;
    if (deplaneCount == 0) {
      mythread_cond_broadcast(&for_Boarding_cond);
    }
    mythread_mutex_unlock(&for_Boarding_mut);

    mythread_mutex_lock(&for_Boarding_mut);
    while( deplaneCount > 0 )
      mythread_cond_wait(&for_Boarding_cond, &for_Boarding_mut);
    mythread_mutex_unlock(&for_Boarding_mut);  

    mythread_mutex_lock(&for_RowBoat_mut);
    if ( rower_p == 0 ) {
      rower_t = mythread_self();
      rower_p = rower_t->tid;
    }
    mythread_t temp = mythread_self();
    if ( rower_p == temp->tid ) {
      rowBoat((void *)&iSerf);
      boatTrip=1;
    }
    mythread_mutex_unlock(&for_RowBoat_mut);

    mythread_mutex_lock(&for_Deplaning_mut);
    temp = mythread_self();
    while ( rower_p == temp->tid && peopleOnBoard()!=1 ) 
      mythread_cond_wait(&for_Deplaning_cond, &for_Deplaning_mut);
    mythread_mutex_unlock(&for_Deplaning_mut);    

    while ( boatTrip == 0 ) ;

    deplane((void *)&iSerf);

    if (peopleOnBoard() == 1) 
      mythread_cond_signal(&for_Deplaning_cond);

    temp = mythread_self();
    if ( rower_p != temp->tid )
      mythread_exit(NULL);
    else {
      boardHackerCount=0;
      boardSerfCount=0;
      deplaneCount = NUM_TO_BOARD;
      boatTrip=0;
      //convertToSerf=0;
      mythread_cond_broadcast(&for_Restart_cond);
      invalidCond=0;
      mythread_cond_broadcast(&for_Invalid_cond);
    }

  }//while 1 loop

}

void initialize(void)
{

  initialize_me();

  boardHackerCount=0;
  boardSerfCount=0;

  deplaneCount=NUM_TO_BOARD;
  rower_t=0;
  rower_p=0;

  convertToSerf=0;

  invalidCond=0;

  myleave = 0;

  mythread_mutex_init(&for_Boarding_mut, NULL);
  mythread_mutex_init(&for_RowBoat_mut, NULL);
  mythread_mutex_init(&for_Deplaning_mut, NULL);
  mythread_mutex_init(&for_Restart_mut, NULL);
  mythread_mutex_init(&for_Invalid_mut, NULL);

  mythread_cond_init(&for_Boarding_cond, NULL);
  mythread_cond_init(&for_Deplaning_cond, NULL);
  mythread_cond_init(&for_Restart_cond, NULL);
  mythread_cond_init(&for_Invalid_cond, NULL);

  printf("---------------------------------------\n");

}

int main(int argc, char** argv)
{
	mythread_t th[MAX_THREAD_NUM];
	int th_arg[MAX_THREAD_NUM];
	int i;
	int numOfHacker, numOfSerf;

	struct timeval now;
	struct timeval timeout;

	while(1) {

 	        scanf("%d %d", &numOfHacker, &numOfSerf);

		if ( numOfHacker == 0 && numOfSerf == 0 )
			break;

		//mythread_setconcurrency(numOfHacker + numOfSerf + 1);
		mythread_setconcurrency(5);

		for ( i=0 ; i<(numOfHacker+numOfSerf) ; i++ )
		        scanf("%d", &(th_arg[i]));

		initialize();

		for ( i=0 ; i<(numOfHacker+numOfSerf) ; i++ ) {
			if ( th_arg[i] == 1 )
				mythread_create(&(th[i]), NULL, hacker, (void*)(&th_arg[i]));
			else
				mythread_create(&(th[i]), NULL, serf, (void*)(&th_arg[i]));
		}

		gettimeofday(&now, NULL);
		
		do {
		  mythread_yield();
		  gettimeofday(&timeout, NULL);
		}while (timeout.tv_sec <= now.tv_sec+3);

		myleave = 1;
		mythread_cond_broadcast(&for_Restart_cond);
		mythread_cond_broadcast(&for_Invalid_cond);
		mythread_cond_broadcast(&for_Boarding_cond);

		for ( i=0 ; i<(numOfHacker+numOfSerf) ; i++ )
			mythread_join(th[i], NULL);

	}

	return 0;
}
