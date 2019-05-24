CC=gcc
CCFLAGS= -o $@ -g
SHELL:=/bin/zsh
STRUCTS := Structs
AVM := AVM
OBJ := obj
EXEC := AVM/executions
EXECOBJ := AVM/executions/obj
DIR = obj/
EXECSOURCES := $(EXEC)/exec_assign.c $(EXEC)/exec_func.c $(EXEC)/exec_jumps.c $(EXEC)/exec_nop.c $(EXEC)/exec_operations.c $(EXEC)/exec_table.c 
SOURCES := $(STRUCTS)/Stack.c $(STRUCTS)/Queue.c $(STRUCTS)/SymTable.c $(STRUCTS)/Quad.c $(STRUCTS)/t_libAVM.c 
OBJECTS := $(patsubst $(STRUCTS)/%.c, $(OBJ)/%.o, $(SOURCES))
EXECOBJECTS := $(patsubst $(EXEC)/%.c, $(EXECOBJ)/%.o, $(EXECSOURCES))

GREY="\033[0;37m"
CYAN="\033[0;36m"
NC="\033[0m" # No Color

all: clean_start $(DIR) out $(EXECOBJ) avm
	@echo -n ${NC}
	@echo "========================== Compilation_Succesfull =========================="

out: $(OBJECTS) writer.o parser.o scanner.o 
	@echo -n ${CYAN}
	$(CC) $(OBJECTS) writer.o parser.o scanner.o  $(CCFLAGS)
	@echo -n ${NC}
	

$(OBJ)/%.o: $(STRUCTS)/%.c 
	@echo -n ${GREY}
	$(CC) -I$(STRUCTS) -c $< -o $@
	@echo -n ${NC}

$(DIR):
	mkdir $@

$(EXECOBJ):
	mkdir $@

# out: parser flex
# 	$(CC) $(CCFLAGS) $(LIBS) parser.c scanner.c

$(EXECOBJ)/%.o: $(EXEC)/%.c 
	@echo -n ${GREY}
	$(CC) -I$(AVM) -c $< -o $@
	@echo -n ${NC} 

avm:  reader.o $(EXECOBJECTS) avm.o 
	$(CC) $(EXECOBJECTS) reader.o avm.o   $(CCFLAGS)

reader.o: $(AVM)/reader.c
	@echo -n ${GREY}
	$(CC) -I$(STRUCTS) -I$(AVM) -c $< -o $@
	@echo -n ${NC}

avm.o: $(AVM)/avm.c
	@echo -n ${GREY}
	$(CC) -I$(STRUCTS) -I$(AVM) -c $< -o $@
	@echo -n ${NC}

writer.o: $(AVM)/writer.c
	@echo -n ${GREY}
	$(CC) -I$(STRUCTS) -I$(AVM) -c $< -o $@
	@echo -n ${NC}

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
	bison --yacc --defines=parser.h --output=parser.c -v parser.y -Wnone
	@echo -n ${NC}

scanner.c: scanner.l
	@echo -n ${CYAN}
	flex -o scanner.c scanner.l
	@echo -n ${NC}

clean_start:
	@echo -n ${NC}
	clear
	@echo "=========================== Compilation_Started ==========================="
	$(RM) obj/*.o parser.o scanner.o scanner.c parser.c parser.h parser.output writer.o reader.o $(EXECOBJ)/*.o

clean:
	@echo -n ${NC}
	$(RM) obj/*.o parser.o scanner.o scanner.c parser.c parser.h parser.output writer.o reader.o *.abc
	$(RM) tests_4h_5h/*.abc
	rmdir obj/

test: all
	./out temp.txt

clean_reader:
	$(RM) reader.o reader

reader: clean_reader reader.o
	$(CC) reader.o -o reader -g
	./reader

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

