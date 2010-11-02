CC = gcc
CFLAGS = -Wall -Wextra -pedantic -c -g -o
LDFLAGS = -lrt
ASM = nasm
AFLAGS = -g -f elf -o
OBJ = bin/bf.o bin/main.o

all: bin bin/main

bin/%.o: src/%.asm
	$(ASM) $(AFLAGS) $@ $<

bin/%.o: src/%.c
	$(CC) $(CFLAGS) $@ $<

bin/main: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

bin:
	mkdir -p bin

.PHONY: clean

clean:
	rm -r bin
