CC=gcc
CCFLAGS=-O0

all: out

out: parser flex
	$(CC) $(CCFLAGS) -o out parser.c scanner.c

parser: 
	bison --yacc --defines=parser.h --output=parser.c parser.y

scan: flex
	$(CC) -o al $(CCFLAGS) scanner.c ./Structs/Stack.c ./Structs/Queue.c

flex:
	flex -o scanner.c scanner.l

clean:
	rm scanner.c al
