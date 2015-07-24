CC = gcc
PROG = texture1 texture1-gl

CFLAGS  = -std=gnu99 -Wall -O2
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wshadow
CFLAGS += $(shell pkg-config --cflags SDL2_image)

LDFLAGS     = -lm $(shell pkg-config --libs sdl2 SDL2_image)
LDFLAGS_GL  = -lGL -lGLU -lglut

LIB  = ../../gl.o
LIB += ../../gl_sdl2.o
LIB += ../../vector.o
LIB += ../../math.o
LIB += ../../matrix.o

all: $(PROG)

texture1: main.c
	$(CC) $(CFLAGS) -D__MUKAGL__ main.c $(LIB) $(LDFLAGS) -o $@

texture1-gl: main.c
	$(CC) $(CFLAGS) main.c $(LDFLAGS) $(LDFLAGS_GL) -o $@

clean:
	rm -f $(PROG) *.o

.PHONY: all clean
