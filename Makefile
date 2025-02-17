CC = gcc
SOURCES = rac0.c rac0a.c rac0a_assembler.c rac0a_lexer.c rac0a_parser.c rac0_utils.c plum.c haul.c rac0a_hl.c
EXES = rac0 rac0a

OBJS = $(SOURCES:.c=.o)

all: clean_exec $(EXES) clean

rac0: rac0.o plum.o rac0_utils.o
	$(CC) $^ -o $@

rac0a: rac0a.o plum.o haul.o rac0a_assembler.o rac0a_lexer.o rac0a_parser.o rac0_utils.o rac0a_hl.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -rf $(OBJS)

clean_exec:
	rm -rf $(EXES)

execute: all
	@for exe in $(EXES); do ./$$exe; done
