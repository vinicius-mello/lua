
LUA_TOPDIR= /usr/local
LUA_INCDIR= $(LUA_TOPDIR)/include
LUA_BINDIR= $(LUA_TOPDIR)/bin
    LIBDIR= $(LUA_TOPDIR)/lib/lua/5.4
       LUA= $(LUA_BINDIR)/lua

CFLAGS= -Wall -Wextra -Wfatal-errors -O2 -g
MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR) -DDEBUG

all: lpt2.so lpt3.so

lpt2.so: llpt2.c llpt_inc.c lpt2.o morton.o queue.o
	cc $(MYCFLAGS) -o $@ -shared -fPIC llpt2.c lpt2.o morton.o queue.o

lpt3.so: llpt3.c  llpt_inc.c lpt3.o morton.o queue.o
	cc $(MYCFLAGS) -o $@ -shared -fPIC llpt3.c lpt3.o morton.o queue.o

morton.o: morton.c morton.h
	cc $(MYCFLAGS) -c morton.c

queue.o: queue.c queue.h
	cc $(MYCFLAGS) -c queue.c

lpt2.o: lpt2.c lpt2.h lpt_inc.c
	cc $(MYCFLAGS) -c -fPIC lpt2.c

lpt3.o: lpt3.c lpt3.h lpt_inc.c
	cc $(MYCFLAGS) -c -fPIC lpt3.c

test: test.c lpt3.o lpt2.o morton.o
	cc $(MYCFLAGS) -o $@ test.c lpt2.o lpt3.o morton.o queue.o

install:
	cp lpt2.so lpt3.so $(LIBDIR)

clean:
	rm -rf lpt2.so lpt3.so lpt2.o lpt3.o morton.o test
