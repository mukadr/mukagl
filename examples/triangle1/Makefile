CC = gcc
PROG = triangle1

CFLAGS  = -std=gnu99 -Wall -O2
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wshadow
CFLAGS += -D__MUKAGL__

LDFLAGS  = -lm -lX11
#LDFLAGS += -lGL -lGLU -lglut

OBJS  = main.o
OBJS += ../../gl.o
OBJS += ../../gl_x11.o
OBJS += ../../vector.o
OBJS += ../../matrix.o

all: $(PROG)

triangle1: $(OBJS)
	@echo "  LINK    $@"
	@$(CC) $^ $(LDFLAGS) -o $@

-include $(patsubst %.o,.%.d,$(OBJS) fuse.o ghost.o)

%.o: %.c
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -MMD -MF .$*.d -c $<

clean:
	rm -f $(PROG) *.o *.so .*.d

.PHONY: all clean
