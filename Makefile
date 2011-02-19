SRCS =  mymutex.c mytest.c
INC = futex.h  futex_inline.h  myatomic.h  mythread.h  myqueue.h mymutex.h
OBJS = $(SRCS:.c=.o)

CFLAGS = -Wall -Werror -g -I.
LDFLAGS = -L.
EXTRA_CFLAGS = -I.

CC = gcc

a5:  all
all: lib

lib: $(OBJS) $(INC)

%.o: %.c $(INC)
	$(CC) $(CFLAGS) $< mythread.a

clean:
	rm -f $(OBJS) mytest *~
tags:
	find . -name "*.[cChH]" | xargs ctags
	find . -name "*.[cChH]" | etags -





