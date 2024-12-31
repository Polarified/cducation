CC = gcc
SRCDIR = ./src
CFLAGS = -Wall -Wextra -Werror -O2
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean