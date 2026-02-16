include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR) -I/usr/include/openblas

array.so: array.c
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) $< -lopenblas -llapacke -llapack -lm

install: array.so
	cp array.so $(INSTALLDIR)

clean:
	rm -rf array.so
