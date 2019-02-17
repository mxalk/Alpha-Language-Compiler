CC=gcc

all: scan

scan: flex
	$(CC) -o scanner scanner.c

flex:
	flex -o scanner.c scanner.l

clean:
	rm scanner.c scanner
