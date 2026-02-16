include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)

complex.so: lcomplex.c
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) $< -lm

install: complex.so
	cp complex.so weierstrass.lua $(INSTALLDIR)

clean:
	rm -f *.o *.so
