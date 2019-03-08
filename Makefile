CC=gcc
CCFLAGS=-O0
LIBS= ./Structs/Stack.c ./Structs/Queue.c

all: out

out: parser flex
	$(CC) $(CCFLAGS) -o out parser.c scanner.c $(LIBS)

parser: 
	bison --yacc --defines=parser.h --output=parser.c -v parser.y

flex:
	flex -o scanner.c scanner.l

clean:
	$(RM) al scanner.c parser.c parser.h parser.output
