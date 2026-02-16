include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)

FC = gfortran
FFLAGS = -c -Wall -fbounds-check -g -Wno-uninitialized 
#-fc-prototypes-external

lbfgsb.so: lbfgsb.c lbfgsb.o linpack.o blas.o timer.o
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) lbfgsb.c lbfgsb.o linpack.o blas.o timer.o -lgfortran

lbfgsb.o: lbfgsb.f
	$(FC) $(FFLAGS) lbfgsb.f -o lbfgsb.o

linpack.o: linpack.f
	$(FC) $(FFLAGS) linpack.f -o linpack.o

blas.o: blas.f
	$(FC) $(FFLAGS) blas.f -o blas.o

timer.o: timer.f
	$(FC) $(FFLAGS) timer.f -o timer.o

install: lbfgsb.so
	cp lbfgsb.so $(INSTALLDIR)

clean:
	rm -rf lbfgsb.so lbfgsb.o linpack.o blas.o timer.o