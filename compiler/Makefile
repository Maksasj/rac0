CC = gcc
SOURCES = main.c rac0a_assembler.c rac0a_lexer.c rac0a_parser.c rac0_utils.c plum.c haul.c rac0a_hl.c
EXES = rac0a

OBJS = $(SOURCES:.c=.o)

all: clean_exec $(EXES) clean

rac0a: main.o plum.o haul.o rac0a_assembler.o rac0a_lexer.o rac0a_parser.o rac0_utils.o rac0a_hl.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c -I.. $< -o $@

clean:
	rm -rf $(OBJS)

clean_exec:
	rm -rf $(EXES)

execute: all
	@for exe in $(EXES); do ./$$exe; done
