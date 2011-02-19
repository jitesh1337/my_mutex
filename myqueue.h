/*
 * myqueue.h -- interface for queue ops
 */

#include <malloc.h>
#include <stdio.h>

#include "futex.h"

typedef struct mythread_queue {

  void *item;
  struct mythread_queue *prev, *next;

  int locked;
  struct futex wait_block;

} *mythread_queue_t;
