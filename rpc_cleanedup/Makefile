# Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l5.html
# Modifcations from MAS:
CC     = gcc
CFLAGS = -ansi -pedantic -g -Wall

# Back to our regularly scheduled Makefile...

BIN    = remtop remtop_svc rscreenshot screenshot_svc graphics
GEN    = remtop_clnt.c remtop_svc.c remtop_xdr.c remtop.h screenshot_clnt.c screenshot_svc.c screenshot_xdr.c screenshot.h
RPCCOM = rpcgen

INCLUDES = -I/usr/include/glib-2.0/ -I/usr/include/gtk-2.0/ -I/usr/lib/glib-2.0/include/ -I/usr/include/cairo/ -I/usr/include/pango-1.0/ -I/usr/lib/gtk-2.0/include/ -I/usr/include/gdk-pixbuf-2.0/ `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`

all: $(BIN)

# remtop (remote desktop) is the client
remtop: remtop.o remtop_clnt.o remtop_xdr.o
	$(CC) -o $@ remtop.o remtop_clnt.o remtop_xdr.o -lXtst

remtop.o: remtop.c remtop.h
	$(CC) -g remtop.c -c

remtop_svc: remtop_proc.o remtop_svc.o remtop_xdr.o
	$(CC) -o $@ remtop_proc.o remtop_svc.o remtop_xdr.o -lXtst

remtop_proc.o: remtop_proc.c remtop.h

rscreenshot: rscreenshot.o screenshot_clnt.o screenshot_xdr.o
	$(CC) -o $@ rscreenshot.o screenshot_clnt.o screenshot_xdr.o $(INCLUDES)

rscreenshot.o: rscreenshot.c screenshot.h
	$(CC) -g rscreenshot.c -c $(INCLUDES)

screenshot_svc: screenshot_proc.o screenshot_svc.o screenshot_xdr.o
	$(CC) -o $@ screenshot_proc.o screenshot_svc.o screenshot_xdr.o `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`

screenshot_proc.o: screenshot_proc.c screenshot.h
	$(CC) -c screenshot_proc.c $(INCLUDES)

graphics: graphics.c
	$(CC) -o $@ graphics.c $(INCLUDES)

$(GEN): remtop.x screenshot.x
	$(RPCCOM) remtop.x
	$(RPCCOM) screenshot.x


clean cleanup:
	rm -f $(GEN) *.o $(BIN)

