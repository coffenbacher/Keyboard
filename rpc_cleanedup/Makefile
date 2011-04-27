# Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l5.html
# Modifcations from MAS:
CC     = gcc
CFLAGS = -g -Wall

# Back to our regularly scheduled Makefile...

BIN    = remtop remtop_svc
GEN    = remtop_clnt.c remtop_svc.c remtop_xdr.c remtop.h
RPCCOM = rpcgen

all: $(BIN)

# remtop (remote desktop) is the client
remtop: remtop.o remtop_clnt.o remtop_xdr.o
	$(CC) -o $@ remtop.o remtop_clnt.o remtop_xdr.o -lXtst

remtop.o: remtop.c remtop.h
	$(CC) -g remtop.c -c

remtop_svc: remtop_proc.o remtop_svc.o remtop_xdr.o
	$(CC) -o $@ remtop_proc.o remtop_svc.o remtop_xdr.o -lXtst

remtop_proc.o: remtop_proc.c remtop.h

$(GEN): remtop.x
	$(RPCCOM) remtop.x

clean cleanup:
	rm -f $(GEN) *.o $(BIN)

