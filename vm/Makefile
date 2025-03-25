CC = gcc
SOURCES = main.c rac0.c rac0_utils.c plum.c haul.c sdl_peripheral.c debug_peripheral.c
EXES = rac0

OBJS = $(SOURCES:.c=.o)

all: clean_exec $(EXES) clean

rac0: main.o rac0.o plum.o haul.o rac0_utils.o sdl_peripheral.o debug_peripheral.o
	$(CC) $^ -o $@ -lSDL2 -lm

%.o: %.c
	$(CC) -c -I.. $< -o $@ 

clean:
	rm -rf $(OBJS)

clean_exec:
	rm -rf $(EXES)

execute: all
	@for exe in $(EXES); do ./$$exe; done
