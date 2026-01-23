

LUA_TOPDIR= /usr/local
LUA_INCDIR= $(LUA_TOPDIR)/include
LUA_BINDIR= $(LUA_TOPDIR)/bin
    LIBDIR= $(LUA_TOPDIR)/lib/lua/5.3
       LUA= $(LUA_BINDIR)/lua

CFLAGS= -Wall -Wextra -Wfatal-errors -O2
MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)
INTERVAL_LIB = 

interval.so: linterval.cpp
	c++ $(MYCFLAGS) -o $@ -shared -fPIC $< $(INTERVAL_LIB) 

install:
	cp interval.so adia.lua ad.lua $(LIBDIR)

clean:
	rm -rf interval.so
