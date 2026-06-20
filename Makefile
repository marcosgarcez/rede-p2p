# Makefile — Rede P2P com Grafos em C

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
TARGET  = rede_p2p
SRCS    = main.c grafo.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c grafo.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
