# utopia makefile

NAME = libutopia

CC = gcc
STD = -std=c89
WFLAGS = -Wall -Wextra -pedantic
OPT = -O2
INC = -I.

SRCDIR = src
TMPDIR = tmp
BINDIR = bin
INCDIR = utopia

SCRIPT = build.sh
SRC = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(TMPDIR)/%.o,$(SRC))

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
	DLIB = -dynamiclib
	SUFFIX = .dylib
else
	DLIB = -shared -fPIC
	SUFFIX = .so
endif

TARGET = $(BINDIR)/$(NAME)
LIB = $(TARGET)$(SUFFIX)

CFLAGS = $(STD) $(WFLAGS) $(OPT) $(INC)

$(TARGET).a: $(BINDIR) $(OBJS)
	ar -cr $@ $(OBJS)

$(TMPDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS): | $(TMPDIR)

$(TMPDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

shared: $(LIB)

$(LIB): $(BINDIR) $(OBJS)
	$(CC) $(CFLAGS) $(DLIB) -o $@ $(OBJS)

all: $(LIB) $(TARGET).a

clean: $(SCRIPT)
	./$^ $@

install: $(SCRIPT)
	./$^ $@

uninstall: $(SCRIPT)
	./$^ $@
