include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)

all: lpt2.so lpt3.so

lpt2.so: llpt2.c llpt_inc.c lpt2.o morton.o queue.o
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) llpt2.c lpt2.o morton.o queue.o -lm

lpt3.so: llpt3.c  llpt_inc.c lpt3.o morton.o queue.o
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) llpt3.c lpt3.o morton.o queue.o -lm

morton.o: morton.c morton.h
	$(CC) $(MYCFLAGS) -c morton.c

queue.o: queue.c queue.h
	$(CC) $(MYCFLAGS) -c queue.c

lpt2.o: lpt2.c lpt2.h lpt_inc.c
	$(CC) $(MYCFLAGS) -fPIC -c lpt2.c

lpt3.o: lpt3.c lpt3.h lpt_inc.c
	$(CC) $(MYCFLAGS) -fPIC -c lpt3.c

test: test.c lpt3.o lpt2.o morton.o
	$(CC) $(MYCFLAGS) -o $@ test.c lpt2.o lpt3.o morton.o queue.o -lm

install: lpt2.so lpt3.so
	cp lpt2.so lpt3.so $(INSTALLDIR)

clean:
	rm -rf lpt2.so lpt3.so lpt2.o lpt3.o morton.o test
