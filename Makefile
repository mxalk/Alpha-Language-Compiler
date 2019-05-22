CC=gcc
CCFLAGS= -o $@ -g
SHELL:=/bin/sh
STRUCTS := Structs
OBJ := obj
DIR = obj/
SOURCES := $(STRUCTS)/Stack.c $(STRUCTS)/Queue.c $(STRUCTS)/SymTable.c $(STRUCTS)/Quad.c $(STRUCTS)/t_libAVM.c
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
	bison --yacc --defines=parser.h --output=parser.c -v parser.y -Wno-conflicts-sr
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
	./out temp.txt


t_success:	all
	./out tests_4h_5h/basic_complex.asc
	./out tests_4h_5h/basic_simple.asc
	./out tests_4h_5h/calc.asc
	./out tests_4h_5h/Circle.asc
	./out tests_4h_5h/delegation.asc
	./out tests_4h_5h/err1.asc
	./out tests_4h_5h/err2.asc
	./out tests_4h_5h/err3.asc
	./out tests_4h_5h/err4.asc
	./out tests_4h_5h/err5.asc
	./out tests_4h_5h/err6.asc
	./out tests_4h_5h/funcs.asc
	./out tests_4h_5h/hercules.asc
	./out tests_4h_5h/libfuncs.asc
	./out tests_4h_5h/line_point.asc
	./out tests_4h_5h/queens.asc
	./out tests_4h_5h/Random.asc
	./out tests_4h_5h/ShadowedFunctions.asc
	./out tests_4h_5h/tables1.asc
	./out tests_4h_5h/tables2.asc
	./out tests_4h_5h/tables3.asc
	./out tests_4h_5h/tables_bonus.asc
	./out tests_4h_5h/Tree1.asc
	./out tests_4h_5h/Tree2.asc
	./out tests_4h_5h/visitor.asc

