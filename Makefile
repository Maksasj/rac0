CC = gcc
SOURCES = rac0.c rac0_assembler.c
EXES = rac0 rac0_assembler

OBJS = $(SOURCES:.c=.o)

all: clean_exec $(EXES) clean

rac0: rac0.o
	$(CC) $^ -o $@

rac0_assembler: rac0_assembler.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -rf $(OBJS)

clean_exec:
	rm -rf $(EXES)

execute: all
	@for exe in $(EXES); do ./$$exe; done
