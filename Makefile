CC=gcc
CCFLAGS=-O0 -o out
LIBS= ./Structs/Stack.c ./Structs/Queue.c ./Structs/SymTable.c

all: clean out

out: parser flex
	$(CC) $(CCFLAGS)  parser.c scanner.c $(LIBS)

parser: 
	bison --yacc --defines=parser.h --output=parser.c -v parser.y

a.out:	flex
	gcc scanner.c $(LIBS) -o al
	./al test6.txt

flex:
	flex -o scanner.c scanner.l

clean:
	$(RM) al scanner.c parser.c parser.h parser.output
