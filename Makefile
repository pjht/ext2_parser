CC = gcc
CFLAGS = -Wall -g
SRCS = $(wildcard *.c */*.c */*/*.c)
OBJS = $(SRCS:.c=.o)
MAIN = ext2prsr
.PHONY: clean
all: $(MAIN)
run: $(MAIN)
	./$(MAIN)
debug: $(MAIN)
	lldb $(MAIN)
$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
clean:
	$(RM) *.o *~ $(MAIN)
