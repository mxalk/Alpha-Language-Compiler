CC=gcc
CCFLAGS= -o $@
SHELL:=/bin/sh
STRUCTS := Structs
OBJ := obj
DIR = obj/
SOURCES := $(STRUCTS)/Stack.c $(STRUCTS)/Queue.c $(STRUCTS)/SymTable.c $(STRUCTS)/Quad.c
OBJECTS := $(patsubst $(STRUCTS)/%.c, $(OBJ)/%.o, $(SOURCES))

GREY="\033[0;37m"
CYAN="\033[0;36m"
NC="\033[0m" # No Color

all: clean_start $(DIR) out
	@echo -n ${NC}

out: $(OBJECTS) parser.o scanner.o
	@echo -n ${CYAN}
	$(CC) $^ $(CCFLAGS) 
	@echo -n ${NC}
	@echo "========================== Compilation_Succesfull =========================="
	

$(OBJ)/%.o: $(STRUCTS)/%.c 
	@echo -n ${GREY}
	$(CC) -I$(STRUCTS) -c $< -o $@
	@echo -n ${NC}

$(DIR):
	mkdir $@

# out: parser flex
# 	$(CC) $(CCFLAGS) $(LIBS) parser.c scanner.c

parser.o: parser.c
	@echo -n ${GREY}
	$(CC) -I$(STRUCTS) -c $< -o $@
	@echo -n ${NC}
scanner.o: scanner.c
	@echo -n ${GREY}
	$(CC) -I$(STRUCTS) -c $< -o $@
	@echo -n ${NC}

parser.c: parser.y
	@echo -n ${CYAN}
	bison --yacc --defines=parser.h --output=parser.c -v parser.y 
	@echo -n ${NC}

scanner.c: scanner.l
	@echo -n ${CYAN}
	flex -o scanner.c scanner.l
	@echo -n ${NC}

clean_start:
	@echo -n ${NC}
	clear
	@echo "=========================== Compilation_Started ==========================="
	$(RM) obj/*.o parser.o scanner.o scanner.c parser.c parser.h parser.output

clean:
	@echo -n ${NC}
	$(RM) obj/*.o parser.o scanner.o scanner.c parser.c parser.h parser.output
	rmdir obj/

test: all
	./out antest.txt


t_success:	all
	./out testfiles/working/Anonymous.asc
	./out testfiles/working/Block.asc
	./out testfiles/working/Circle.asc
	./out testfiles/working/GlobalAndLocal.asc
	./out testfiles/working/Grammar.asc
	./out testfiles/working/Random.asc
	./out testfiles/working/ShadowedNameOffunctions.asc
	./out testfiles/working/Simple.asc
	# ./out testfiles/working/Tree.asc