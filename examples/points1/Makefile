CC = gcc
PROG = points1 points1-gl

CFLAGS  = -std=gnu99 -Wall -O2
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wshadow

LDFLAGS     = -lm -lX11
LDFLAGS_GL  = -lGL -lGLU -lglut

LIB  = ../../gl.o
LIB += ../../gl_x11.o
LIB += ../../vector.o
LIB += ../../matrix.o

all: $(PROG)

points1: main.c
	$(CC) $(CFLAGS) -D__MUKAGL__ main.c $(LIB) $(LDFLAGS) -o points1

points1-gl: main.c
	$(CC) $(CFLAGS) main.c $(LDFLAGS) $(LDFLAGS_GL) -o points1-gl

clean:
	rm -f $(PROG) *.o

.PHONY: all clean
