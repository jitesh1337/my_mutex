
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "mythread.h"
#include "mymutex.h"
#include "mycond.h"

static int boardedHackers = 0;
static int boardedSerfs = 0;
mythread_mutex_t board_mutex;

void initialize_me()
{
  mythread_mutex_init(&board_mutex, NULL);
}

void myprintftest(char *s, int i) {
  char out[80];

  sprintf(out, s, i);
  write(1, out, strlen(out));
}

void board(void* arg) {
  int isHacker = *(int*)(arg);

  //printf("in board\n");
  mythread_mutex_lock(&board_mutex);
  //printf("board: took lock\n");
  if ( isHacker )
    boardedHackers++;
  else
    boardedSerfs++;

  myprintftest("%d ", boardedHackers);
  myprintftest("%d ", boardedSerfs);

  mythread_t temp = mythread_self();
  if ( isHacker )
    myprintftest("hacker[%08x] boards\n", temp->tid);
  else
    myprintftest("serf  [%08x] boards\n", temp->tid);

  mythread_mutex_unlock(&board_mutex);
  //printf("leaving board\n");
}

void rowBoat(void* arg) {
  int isHacker = *(int*)(arg);

  mythread_mutex_lock(&board_mutex);

  mythread_t temp = mythread_self();

  if ( isHacker ) {
    myprintftest("%d*", boardedHackers);
    myprintftest("%d ", boardedSerfs);
    myprintftest("hacker[%08x] rows the boat\n", temp->tid);
  }
  else {
    myprintftest("%d ", boardedHackers);
    myprintftest("%d*", boardedSerfs);
    myprintftest("serf  [%08x] rows the boat\n", temp->tid);
  }

  mythread_mutex_unlock(&board_mutex);
}

void deplane(void *arg)
{
  int isHacker = *(int*)(arg);

  mythread_mutex_lock(&board_mutex);

  if ( isHacker )
    boardedHackers--;
  else
    boardedSerfs--;

  myprintftest("%d ", boardedHackers);
  myprintftest("%d ", boardedSerfs);

  mythread_t temp = mythread_self();

  if ( isHacker )
    myprintftest("hacker[%08x] deplanes\n", temp->tid);
  else
    myprintftest("serf  [%08x] deplanes\n", temp->tid);

  mythread_mutex_unlock(&board_mutex);
}

void leave(void *arg)
{
  int isHacker = *(int*)(arg);

  mythread_mutex_lock(&board_mutex);

  myprintftest("%d ", boardedHackers);
  myprintftest("%d ", boardedSerfs);

  mythread_t temp = mythread_self();

  if ( isHacker )
    myprintftest("hacker[%08x] leaves\n", temp->tid);
  else
    myprintftest("serf  [%08x] leaves\n", temp->tid);

  mythread_mutex_unlock(&board_mutex);
}

int peopleOnBoard(void)
{
  int ret;

  mythread_mutex_lock(&board_mutex);
  ret = boardedHackers + boardedSerfs;
  mythread_mutex_unlock(&board_mutex);

  return ret;
}
