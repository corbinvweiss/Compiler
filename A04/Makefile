CC = g++     
OP = # -O2      # no optimizations for now
FLAGS = -g -Wall -Wextra -Wpedantic -Wno-unused-function -Wno-unused-parameter -std=c++20

all: rustish

rustish: rustish.tab.o lex.yy.o AST.o SymbolTable.o
	${CC} ${OP} ${FLAGS} -o rustish rustish.tab.o lex.yy.o AST.o SymbolTable.o

AST.o: AST.cpp
	${CC} ${OP} ${FLAGS} -c AST.cpp

SymbolTable.o: SymbolTable.cpp
	${CC} ${OP} ${FLAGS} -c SymbolTable.cpp

rustish.tab.o: rustish.tab.cpp
	${CC} ${OP} ${FLAGS} -c rustish.tab.cpp

#rustish.tab.cpp: rustish.tab.c
#	mv -f rustish.tab.c rustish.tab.cpp

rustish.tab.cpp: rustish.y 
	bison -Wcounterexamples -v -d -o rustish.tab.cpp rustish.y

lex.yy.o: lex.yy.cpp
	${CC} $(OP) $(FLAGS) -Wno-sign-compare -c lex.yy.cpp 

lex.yy.cpp: rustish.l rustish.tab.hpp
	flex -o lex.yy.cpp rustish.l

clean:
	/bin/rm -f *.o *.yy.c *.yy.cpp *.tab.cpp *.tab.c *.tab.hpp \
	           core *.output *.temp *~ rustish 

