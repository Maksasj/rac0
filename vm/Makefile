CC = gcc
SOURCES = rac0.c rac0_utils.c plum.c haul.c
EXES = rac0

OBJS = $(SOURCES:.c=.o)

all: clean_exec $(EXES) clean

rac0: rac0.o plum.o rac0_utils.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c -I.. $< -o $@ 

clean:
	rm -rf $(OBJS)

clean_exec:
	rm -rf $(EXES)

execute: all
	@for exe in $(EXES); do ./$$exe; done
