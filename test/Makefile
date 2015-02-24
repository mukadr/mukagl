CC = gcc
PROG = raster1

CFLAGS  = -std=gnu99 -Wall -O2
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wshadow

all: $(PROG)

raster1: raster1.c
	$(CC) $(CFLAGS) ../math.o $< -o $@

clean:
	rm -f $(PROG) *.o *.so .*.d

.PHONY: all clean
