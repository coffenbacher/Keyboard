# Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l5.html
# Modifcations from MAS:
CC     = gcc
CFLAGS = -g -Wall

# Back to our regularly scheduled Makefile...

BIN    = ravg avg_svc
GEN    = avg_clnt.c avg_svc.c avg_xdr.c avg.h
RPCCOM = rpcgen

INCLUDE = `pkg-config --cflags --libs libpulse` `pkg-config --cflags --libs libpulse-simple`

all: $(BIN)

# ravg (remote average) is the client
ravg: ravg.o avg_clnt.o avg_xdr.o
	$(CC) -o $@ ravg.o avg_clnt.o avg_xdr.o $(INCLUDE)

ravg.o: ravg.c avg.h
	$(CC) -g ravg.c -c $(INCLUDE)

avg_svc: avg_proc.o avg_svc.o avg_xdr.o
	$(CC) -o $@ avg_proc.o avg_svc.o avg_xdr.o $(INCLUDE)

avg_proc.o: avg_proc.c avg.h

$(GEN): avg.x
	$(RPCCOM) avg.x

clean cleanup:
	rm -f $(GEN) *.o $(BIN)

