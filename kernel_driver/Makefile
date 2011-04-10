CC = gcc
CFLAGS   = -O0 -Wall -g -ansi -pedantic
KVERSION = $(shell uname -r)
obj-m = hello2.o

all: hello

hello:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean: 
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean