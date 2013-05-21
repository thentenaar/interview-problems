# Makefile for Microsoft QuickC 2.0
#
# To build:
#
#   1. Ensure QCL.EXE / LINK.EXE are in your path, etc.
#      This assumes you have QuickC 2.0 installed at C:\QC2.
#      Then, run:
#          C:\PROBLEMS> set LIB=C:\QC2\LIB
#
#   2. Execute this makefile with:
#          C:\PROBLEMS> nmake -f Makefile.qc
#
#   3. Profit
#
CC=qcl
RM=del
CP=copy

# Include/lib paths
HPATH=C:\QC2\BIN
INCLUDE=C:\QC2\INCLUDE
LIB=C:\QC2\LIB

# QuickC Flags:
# -AS      Small model
# -W3      All warnings (Level 3)
# -Za      Disable M$ Extensions (ANSI mode)
# -Zp      Pack structures
# -Ox      Enable Full Optimization
# -Gs      Disable stack checking
# -DNDEBUG No Debug stuff
CFLAGS=-AS -W3 -Za -Zp -Gs -Ox -DNDEBUG -I$(INCLUDE)

TARGETS=\
	src\atoi.exe     \
	src\llmedian.exe \
	src\phone.exe    \
	src\rand.exe     \
	src\strrev.exe   \
	src\subarray.exe

all: clean $(TARGETS)

clean:
	@$(RM) bin\*.*

.c.exe:
	@tools\crlf $< $<x
	@copy $<x $<
	@del $<x
	@$(CC) $(CFLAGS) -o $@ $<
	@$(CP) $@ BIN
	@$(RM) $@

# vi:set ts=4 sw=4 ft=make:
