LUA_TOPDIR= /usr/local
LUA_INCDIR= $(LUA_TOPDIR)/include
LUA_BINDIR= $(LUA_TOPDIR)/bin
    LIBDIR= $(LUA_TOPDIR)/lib/lua/5.4
       LUA= $(LUA_BINDIR)/lua

CC= gcc -std=c99
CXX= g++
CFLAGS= -Wall -Wextra -Wfatal-errors -O2
SHARED= -shared -fPIC