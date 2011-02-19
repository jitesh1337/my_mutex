/*
 * myqueue.h -- interface for queue ops
 */

#include <malloc.h>
#include <stdio.h>

typedef struct mythread_queue {
  void *item;
  struct mythread_queue *prev, *next;
} *mythread_queue_t;
