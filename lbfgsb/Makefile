LUA_TOPDIR= /usr/local
LUA_INCDIR= $(LUA_TOPDIR)/include
LUA_BINDIR= $(LUA_TOPDIR)/bin
    LIBDIR= $(LUA_TOPDIR)/lib/lua/5.3
       LUA= $(LUA_BINDIR)/lua

CFLAGS= -Wall -Wextra -Wfatal-errors -O2
MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)


FC = gfortran
FFLAGS = -c -Wall -fbounds-check -g -Wno-uninitialized 
#-fc-prototypes-external

lbfgsb.so: lbfgsb.c lbfgsb.o linpack.o blas.o timer.o
	cc $(MYCFLAGS) -o $@ -shared -fPIC lbfgsb.c lbfgsb.o linpack.o blas.o timer.o -lgfortran

lbfgsb.o: lbfgsb.f
	$(FC) $(FFLAGS) lbfgsb.f -o lbfgsb.o

linpack.o: linpack.f
	$(FC) $(FFLAGS) linpack.f -o linpack.o

blas.o: blas.f
	$(FC) $(FFLAGS) blas.f -o blas.o

timer.o: timer.f
	$(FC) $(FFLAGS) timer.f -o timer.o

install:
	cp lbfgsb.so $(LIBDIR)

clean:
	rm -rf lbfgsb.so lbfgsb.o linpack.o blas.o timer.o