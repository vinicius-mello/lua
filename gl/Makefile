include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)
GLFW_LIB = 

all: gl.so glfw.so
	
gl.so: gl.c 
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) $< -lGLEW -lGL 

glfw.so: glfw.c
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) $< $(GLFW_LIB) -lglfw

install: gl.so glfw.so
	cp gl.so glfw.so $(LIBDIR)

clean:
	rm -rf gl.so glfw.so
