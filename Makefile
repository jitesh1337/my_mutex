SRCS =  mymutex.c mytest.c mytestcond.c mycond.c mytestp2.c board.c
INC = futex.h  futex_inline.h  myatomic.h  mythread.h  myqueue.h board.h mymutex.h mycond.h
OBJS = $(SRCS:.c=.o)
LIB = mythread.a

CFLAGS = -Wall -Werror -I.
LDFLAGS = -L.
EXTRA_CFLAGS = -g

CC = gcc

a5:  all
all: lib mytest mytestcond mytestp2

lib: $(OBJS) $(INC)

%.o: %.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<

mytest: mymutex.o mytest.o $(LIB) $(INC)
	$(CC) -o mytest $(CFLAGS) $(EXTRA_CFLAGS) mytest.o mymutex.o $(LIB)

mytestcond: mymutex.o mytestcond.o $(LIB) $(INC)
	$(CC) -o mytestcond $(CFLAGS) $(EXTRA_CFLAGS) mytestcond.o mycond.o mymutex.o $(LIB)

mytestp2: mymutex.o mytestp2.o board.o $(LIB) $(INC)
	$(CC) -o mytestp2 $(CFLAGS) $(EXTRA_CFLAGS) mytestp2.o mycond.o mymutex.o board.o $(LIB)

clean:
	rm -f $(OBJS) mytest mytestcond mytestp2 *~
tags:
	find . -name "*.[cChH]" | xargs ctags
	find . -name "*.[cChH]" | etags -





