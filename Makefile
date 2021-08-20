
STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
IDIR=-I.
CC=gcc
NAME=libutopia
SRC=src/*.c

CFLAGS=$(STD) $(WFLAGS) $(OPT) $(IDIR)
OS=$(shell uname -s)

ifeq ($(OS),Darwin)
	OSFLAGS=-dynamiclib
	LIB=$(NAME).dylib
else
	OSFLAGS=-fPIC
	LIB=$(NAME).so
endif

libutopia.a: $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) && ar -crv $(NAME).a *.o && rm *.o

shared: src/*.c
	$(CC) -o $(LIB) $(SRC) $(CFLAGS) $(OSFLAGS)
	
