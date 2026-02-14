include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR)
INTERVAL_LIB = 

interval.so: linterval.cpp
	$(CXX) $(MYCFLAGS) -o $@ $(SHARED) $< $(INTERVAL_LIB) 

install: interval.so
	cp interval.so adia.lua ad.lua $(LIBDIR)

clean:
	rm -rf interval.so
