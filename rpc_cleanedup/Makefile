# Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l5.html
# Modifcations from MAS:
CC     = gcc
CFLAGS = -g -Wall

# Back to our regularly scheduled Makefile...

BIN    = rdesktop desktop_svc
GEN    = desktop_clnt.c desktop_svc.c desktop_xdr.c desktop.h
RPCCOM = rpcgen

all: $(BIN)

# rdesktop (remote desktop) is the client
rdesktop: rdesktop.o desktop_clnt.o desktop_xdr.o
	$(CC) -o $@ rdesktop.o desktop_clnt.o desktop_xdr.o -lXtst

rdesktop.o: rdesktop.c desktop.h
	$(CC) -g rdesktop.c -c

desktop_svc: desktop_proc.o desktop_svc.o desktop_xdr.o
	$(CC) -o $@ desktop_proc.o desktop_svc.o desktop_xdr.o -lXtst

desktop_proc.o: desktop_proc.c desktop.h

$(GEN): desktop.x
	$(RPCCOM) desktop.x

clean cleanup:
	rm -f $(GEN) *.o $(BIN)

