CC=gcc
SOURCE=main.c
OBJ=$(SOURCE:.c=.o)
EXE=rac0

all: $(EXE) clean execute

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@

.o: .c
	$(CC) -c $< -o $@

clean:
	rm -rf $(OBJ)

execute:
	./$(EXE)