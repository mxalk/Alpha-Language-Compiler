CC=gcc

all: scan

scan: flex
	$(CC) -o al -O0 scanner.c ./Structs/Stack.c
	./al test.txt

flex:
	flex -o scanner.c scanner.l

clean:
	rm scanner.c al
