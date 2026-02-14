include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)

pack.so: lpack.c
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) $< 

install: pack.so
	cp pack.so $(LIBDIR)

clean:
	rm -f *.o *.so