CC=gcc
CCFLAGS= -o $@ -g
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
		./out tests_new/backpatch0.asc
	./out tests_new/backpatch0.asc
	./out tests_new/backpatch1.asc
	./out tests_new/backpatch2.asc
	./out tests_new/backpatch3.asc
	./out tests_new/p3t_assignments_complex.asc
	./out tests_new/p3t_assignments_objects.asc
	./out tests_new/p3t_assignments_simple.asc
	./out tests_new/p3t_basic_expr.asc
	./out tests_new/p3t_calls.asc
	./out tests_new/p3t_const_maths.asc
	./out tests_new/p3t_flow_control.asc
	# ./out tests_new/p3t_flow_control_error.asc # works
	./out tests_new/p3t_funcdecl.asc
	./out tests_new/p3t_if_else.asc
	#  ./out tests_new/p3t_object_creation_expr.asc #seg
	./out tests_new/p3t_relational.asc
	./out tests_new/p3t_var_maths.asc
	./out tests_new/vavouris.asc