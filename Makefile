
RM      = rm -f
CC      = gcc
CFLAGS  = -ansi -pedantic -Wall -O2
OBJS    = $(subst src,bin,$(wildcard src/*.c))

all: $(OBJS)
	@echo "Done"

clean:
	@$(RM) bin/*

bin/%.c:
	@echo "Building $*..."
	@$(CC) $(CFLAGS) -o bin/$* src/$*.c


# vi:set ts=4 sw=4:
