SRCS =  mymutex.c mytest_mutex.c mytest_cond.c mycond.c mytest_p2.c board.c
INC = futex.h  futex_inline.h  myatomic.h  mythread.h  myqueue.h board.h mymutex.h mycond.h
OBJS = $(SRCS:.c=.o)
LIB = mythread.a

CFLAGS = -Wall -Werror -I.
LDFLAGS = -L.
EXTRA_CFLAGS = -g

CC = gcc

a5:  all
all: lib mytest_mutex mytest_cond mytest_p2
test: mytest_mutex mytest_cond mytest_p2

lib: $(OBJS) $(INC)

%.o: %.c $(INC)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c $<

mytest_mutex: mymutex.o mycond.o mytest_mutex.o $(LIB) $(INC)
	$(CC) -o mytest_mutex $(CFLAGS) $(EXTRA_CFLAGS) mytest_mutex.o mymutex.o $(LIB)

mytest_cond: mymutex.o mycond.o mytest_cond.o $(LIB) $(INC)
	$(CC) -o mytest_cond $(CFLAGS) $(EXTRA_CFLAGS) mytest_cond.o mycond.o mymutex.o $(LIB)

mytest_p2: mymutex.o mycond.o mytest_p2.o board.o $(LIB) $(INC)
	$(CC) -o mytest_p2 $(CFLAGS) $(EXTRA_CFLAGS) mytest_p2.o mycond.o mymutex.o board.o $(LIB)

clean:
	rm -f $(OBJS) mytest_mutex mytest_cond mytest_p2 *~
tags:
	find . -name "*.[cChH]" | xargs ctags
	find . -name "*.[cChH]" | etags -





