

LUA_TOPDIR= /usr/local
LUA_INCDIR= $(LUA_TOPDIR)/include
LUA_BINDIR= $(LUA_TOPDIR)/bin
    LIBDIR= $(LUA_TOPDIR)/lib/lua/5.3
       LUA= $(LUA_BINDIR)/lua

CFLAGS= -Wall -Wextra -Wfatal-errors -O2
MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)
GLFW_LIB = 

all: gl.so glfw.so
	

gl.so: gl.c 
	cc $(MYCFLAGS) -o $@ -shared -fPIC $^ -lGLEW -lGL 

glfw.so: glfw.c
	cc $(MYCFLAGS) -o $@ -shared -fPIC $< $(GLFW_LIB) -lglfw

install:
	cp gl.so glfw.so $(LIBDIR)

clean:
	rm -rf gl.so glfw.so
