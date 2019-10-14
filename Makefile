CC = gcc
PROG = mukagl

CFLAGS  = -std=gnu99 -Wall -O2
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wshadow
CFLAGS += $(shell pkg-config --cflags sdl2)
CFLAGS += -D__MUKAGL_DEBUGFPS__

LDFLAGS = -lm $(shell pkg-config --libs sdl2)

OBJS  = gl.o
OBJS += gl_sdl2.o
OBJS += matrix.o
OBJS += vector.o

all: $(PROG) examples

examples: mukagl
	make -C ./examples

mukagl: $(OBJS)

-include $(patsubst %.o,.%.d,$(OBJS))

%.o: %.c
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -MMD -MF .$*.d -c $<

clean:
	rm -f $(PROG) *.o *.so .*.d
	make clean -C ./examples

.PHONY: all clean examples
