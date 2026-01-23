

LUA_TOPDIR= /usr/local
LUA_INCDIR= $(LUA_TOPDIR)/include
LUA_BINDIR= $(LUA_TOPDIR)/bin
    LIBDIR= $(LUA_TOPDIR)/lib/lua/5.3
       LUA= $(LUA_BINDIR)/lua

CFLAGS= -Wall -Wextra -Wfatal-errors -O2
MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)

array.so: array.c
	cc $(MYCFLAGS) -o $@ -shared -fPIC $< -lblas -llapacke -llapack -lm

install: array.so
	cp array.so $(LIBDIR)

clean:
	rm -rf array.so
