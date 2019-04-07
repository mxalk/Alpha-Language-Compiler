CC=gcc
CCFLAGS=-O0 -o out 
LIBS= ./Structs/Stack.c ./Structs/Queue.c ./Structs/SymTable.c ./Structs/Quad.c

all: clean out

out: parser flex
	$(CC) $(CCFLAGS) $(LIBS) parser.c scanner.c

parser: 
	bison --yacc --defines=parser.h --output=parser.c -v parser.y 

a.out:	flex
	gcc scanner.c $(LIBS) -o al
	./al test6.txt

flex:
	flex -o scanner.c scanner.l

clean:
	$(RM) al scanner.c parser.c parser.h parser.output

t_success:	all
	./out testfiles/working/Anonymous.asc
	./out testfiles/working/Block.asc
	./out testfiles/working/Circle.asc
	./out testfiles/working/GlobalAndLocal.asc
	./out testfiles/working/Grammar.asc
	./out testfiles/working/Random.asc
	./out testfiles/working/ShadowedNameOffunctions.asc
	./out testfiles/working/Simple.asc
	#./out testfiles/working/Tree.asc # x

t_error:	all
	#./out testfiles/errors/Error0.asc 
	# ./out testfiles/errors/Error1.asc
	#./out testfiles/errors/Error2.asc
	#./out testfiles/errors/Error3.asc
	#./out testfiles/errors/Error4.asc
	# ./out testfiles/errors/Error5.asc
	# ./out testfiles/errors/Error6.asc 
	# ./out testfiles/errors/Error7.asc
	# ./out testfiles/errors/Error8.asc
	# ./out testfiles/errors/Error9.asc 
	 #./out testfiles/errors/Error10.asc
	# ./out testfiles/errors/Error11.asc
	# ./out testfiles/errors/Error12.asc