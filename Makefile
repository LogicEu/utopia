# utopia makefile

STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
IDIR=-I.
CC=gcc
NAME=libutopia
SRC=src/*.c

OS=$(shell uname -s)

ifeq ($(OS),Darwin)
	OSFLAGS=-dynamiclib
	LIB=$(NAME).dylib
	OSARG=-mmacos-version-min=10.10
else
	OSFLAGS=-shared -fPIC
	LIB=$(NAME).so
endif

CFLAGS=$(STD) $(WFLAGS) $(OPT) $(IDIR) $(OSARG)

$(NAME).a: $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) && ar -cr $(NAME).a *.o && rm *.o

shared: $(SRC)
	$(CC) -o $(LIB) $(SRC) $(CFLAGS) $(OSFLAGS)

clean: build.sh
	./$^ $@

install: build.sh
	./$^ $@

uninstall: build.sh
	./$^ $@
