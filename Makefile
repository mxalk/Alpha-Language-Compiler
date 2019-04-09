CC=gcc
CCFLAGS=-O0 -o $@
SHELL:=/bin/sh
STRUCTS := Structs
OBJ := obj

SOURCES := $(STRUCTS)/Stack.c $(STRUCTS)/Queue.c $(STRUCTS)/SymTable.c $(STRUCTS)/Quad.c
OBJECTS := $(patsubst $(STRUCTS)/%.c, $(OBJ)/%.o, $(SOURCES))


all: clean out

out: $(OBJECTS) parser.o scanner.o
	$(CC) $^ $(CCFLAGS) 

$(OBJ)/%.o: $(STRUCTS)/%.c 
	$(CC) -I$(STRUCTS) -c $< -o $@

# out: parser flex
# 	$(CC) $(CCFLAGS) $(LIBS) parser.c scanner.c

parser.o: parser.c
	$(CC) -I$(STRUCTS) -c $< -o $@

scanner.o: scanner.c
	$(CC) -I$(STRUCTS) -c $< -o $@

parser.c: parser.y
	bison --yacc --defines=parser.h --output=parser.c -v parser.y 

scanner.c: scanner.l
	flex -o scanner.c scanner.l

clean:
	clear
	$(RM) obj/*.o parser.o scanner.o scanner.c parser.c parser.h parser.output