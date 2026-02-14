include ../make.inc

MYCFLAGS= $(CFLAGS) -I$(LUA_INCDIR) -Isrc

pdfsg.so: lpdfsg.c src/pdfsg.c
	$(CC) $(MYCFLAGS) -o $@ $(SHARED) $^

install: pdfsg.so
	cp pdfsg.so $(LIBDIR)

clean:
	rm -f *.o *.so test.pdf
