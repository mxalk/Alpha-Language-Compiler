CC=gcc
CCFLAGS=-O0
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CC=gcc
	CCFLAGS += -D OSX
endif

all: scan

scan: flex
	$(CC) -o al $(CCFLAGS) scanner.c
	./al test.txt

flex:
	flex -o scanner.c scanner.l

clean:
	rm scanner.c al
