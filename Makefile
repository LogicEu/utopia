# utopia makefile

NAME = libutopia

CC = gcc
STD = -std=c89
WFLAGS = -Wall -Wextra -pedantic
OPT = -O2

TMPDIR = tmp
BINDIR = bin
INCDIR = utopia

SCRIPT = build.sh
SRC = $(wildcard $(INCDIR)/*.h)
OBJS = $(patsubst $(INCDIR)/%.h,$(TMPDIR)/%.o,$(SRC))

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

CFLAGS = $(STD) $(WFLAGS) $(OPT)

$(TARGET).a: $(BINDIR) $(OBJS)
	ar -cr $@ $(OBJS)

.PHONY: shared all clean install uninstall

shared: $(LIB)

all: $(LIB) $(TARGET).a

$(LIB): $(BINDIR) $(OBJS)
	$(CC) $(CFLAGS) $(DLIB) -o $@ $(OBJS)

$(TMPDIR)/%.o: $(INCDIR)/%.h
	$(CC) $(CFLAGS) -x c -DUTOPIA_IMPLEMENTATION -c $< -o $@

$(OBJS): | $(TMPDIR)

$(TMPDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

clean:
	$(RM) -r $(TMPDIR) $(BINDIR)

install: $(SCRIPT)
	./$^ $@

uninstall: $(SCRIPT)
	./$^ $@
