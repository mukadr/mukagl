CC = gcc
PROG = mukagl

CFLAGS  = -std=gnu99 -Wall -O2
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wshadow
CFLAGS += $(shell pkg-config --cflags sdl2)

LDFLAGS = -lm $(shell pkg-config --libs sdl2)

OBJS  = gl.o
OBJS += gl_sdl2.o
OBJS += math.o
OBJS += matrix.o
OBJS += vector.o

all: $(PROG) examples test

examples:
	make -C ./examples

test:
	make -C ./test

mukagl: $(OBJS)

-include $(patsubst %.o,.%.d,$(OBJS))

%.o: %.c
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -MMD -MF .$*.d -c $<

clean:
	rm -f $(PROG) *.o *.so .*.d
	make clean -C ./examples
	make clean -C ./test

.PHONY: all clean examples test
