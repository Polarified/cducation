CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))
TARGET = ./bin/main

all: $(TARGET) main_asm

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

main_asm: $(SRCS)
	@mkdir -p obj
	$(CC) -S -O -fno-asynchronous-unwind-tables -fcf-protection=none $^ -o obj/main.s

clean:
	rm -f obj/*.o obj/*.s $(TARGET)

.PHONY: all clean main_asm
