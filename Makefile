CC = gcc
PROG = mukagl

CFLAGS  = -std=gnu99 -Wall -O2
CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wshadow

LDFLAGS = -lm -lX11

OBJS  = main.o
OBJS += matrix.o
OBJS += vector.o

all: $(PROG)

mukagl: $(OBJS)
	@echo "  LINK    $@"
	@$(CC) $^ $(LDFLAGS) -o $@

-include $(patsubst %.o,.%.d,$(OBJS) fuse.o ghost.o)

%.o: %.c
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -MMD -MF .$*.d -c $<

clean:
	rm -f $(PROG) *.o *.so .*.d

.PHONY: all clean
