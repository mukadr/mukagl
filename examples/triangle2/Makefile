CC = gcc
PROG = triangle2 triangle2-gl

CFLAGS  = -std=gnu99 -Wall -O2
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wshadow

LDFLAGS     = -lm $(shell pkg-config --libs sdl2)
LDFLAGS_GL  = -lGL -lGLU -lglut

LIB  = ../../gl.o
LIB += ../../gl_sdl2.o
LIB += ../../vector.o
LIB += ../../math.o
LIB += ../../matrix.o

all: $(PROG)

triangle2: main.c
	$(CC) $(CFLAGS) -D__MUKAGL__ main.c $(LIB) $(LDFLAGS) -o $@

triangle2-gl: main.c
	$(CC) $(CFLAGS) main.c $(LDFLAGS) $(LDFLAGS_GL) -o $@

clean:
	rm -f $(PROG) *.o

.PHONY: all clean
