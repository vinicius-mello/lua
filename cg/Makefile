include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)

FC = gfortran -std=legacy
FFLAGS = -c -Wno-uninitialized -fPIC -Os -s -fdata-sections -ffunction-sections -Wl,--gc-sections 
#-fc-prototypes-external

cg.so: lcg.c  cgfam.o cgsearch.o blas.o
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) $^ -lgfortran

cgfam.o: cgfam.f
	$(FC) $(FFLAGS) cgfam.f -o cgfam.o

cgsearch.o: cgsearch.f
	$(FC) $(FFLAGS) cgsearch.f -o cgsearch.o

blas.o: blas.f
	$(FC) $(FFLAGS) blas.f -o blas.o

install: cg.so
	cp cg.so $(INSTALLDIR)

clean:
	rm -rf cg.so cgfam.o cgsearch.o blas.o 
