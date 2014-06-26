# Makefile for Borland Turbo C 2.0
#
# To build:
#
#   1. Ensure TCC.EXE is in your path, etc.
#   2. Execute this makefile with:
#       C:\PROBLEMS> make -fmakefile.tcc
#   3. Profit
#
CC=tcc
RM=del

# Turbo C flags:
# -A      ANSI Keywords only
# -G      Optimize for Speed
# -O      Enable Jump Optimization
# -Z      Enable Register Optimization
# -f-     Don't link floating point lib
# -r      Use Register Variables
# -mt     Tiny memory model
# -w      Display all warnings
# -w-pia  Supress 'Possibly incorrect assignment' warnings
# -n<dir> Output to <dir>
#
CFLAGS=-A -G -O -Z -f- -r -mt -w -w-pia -nbin

TARGETS=\
	src\8queens.exe  \
	src\atoi.exe     \
	src\calc.exe     \
	src\llmedian.exe \
	src\phone.exe    \
	src\rand.exe     \
	src\strrev.exe   \
	src\subarray.exe

all: clean $(TARGETS)

clean:
	@$(RM) bin\*.*

.c.exe:
	@echo "Building $<"
	@tools\crlf $< $<x
	@copy $<x $<
	@del $<x
	@$(CC) $(CFLAGS) $<

# vi:set ts=4 sw=4 ft=make:
