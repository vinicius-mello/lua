include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR) 
INTERVAL_LIB = -I fi_lib -l:fi_lib.a -L fi_lib

adia.so: adia.c
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) $< $(INTERVAL_LIB) 

install: adia.so
	cp adia.so $(INSTALLDIR)

clean:
	rm -rf adia.so
