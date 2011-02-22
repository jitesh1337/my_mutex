SRCS =  mymutex.c mytest.c mytestcond.c mycond.c
INC = futex.h  futex_inline.h  myatomic.h  mythread.h  myqueue.h mymutex.h
OBJS = $(SRCS:.c=.o)
LIB = mythread.a

CFLAGS = -Wall -Werror -I.
LDFLAGS = -L.
EXTRA_CFLAGS = -g

CC = gcc

a5:  all
all: lib mytest mytestcond

lib: $(OBJS) $(INC)

%.o: %.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<

mytest: mymutex.o mytest.o $(LIB) $(INC)
	$(CC) -o mytest $(CFLAGS) $(EXTRA_CFLAGS) mytest.o mymutex.o $(LIB)

mytestcond: mymutex.o mytestcond.o $(LIB) $(INC)
	$(CC) -o mytestcond $(CFLAGS) $(EXTRA_CFLAGS) mytestcond.o mycond.o mymutex.o $(LIB)

clean:
	rm -f $(OBJS) mytest mytestcond *~
tags:
	find . -name "*.[cChH]" | xargs ctags
	find . -name "*.[cChH]" | etags -





