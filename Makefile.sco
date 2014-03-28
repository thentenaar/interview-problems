# Makefile for SCO
#
# To build:
#
#  1. Ensure /usr/ccs/bin is in your path
#  2. make -f Makefile.sco
#  3. Profit
CC=cc
RM=rm
MV=mv

# For information about the various CFLAGS settings
# available in SCO's cc, see 'man cc' or
#
# http://osr507doc.sco.com/en/man/html.CP/cc.CP.html
#
CFLAGS=-O2 -a ansi -b elf -w 3 -X c

TARGETS=\
	src/atoi.o     \
	src/calc.o     \
	src/llmedian.o \
	src/phone.o    \
	src/rand.o     \
	src/strrev.o   \
	src/subarray.o

all: clean $(TARGETS)

clean:
	@$(RM) -f bin/*

.c.o:
	@echo "Building $<"
	@$(CC) $(CFLAGS) $< -o $*
	@$(MV) $* bin/

