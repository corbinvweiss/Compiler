CC = g++     
OP = # -O2      # no optimizations for now
FLAGS = -g -Wall -Wextra -Wpedantic -Wno-unused-function -Wno-unused-parameter -std=c++20

all: rustish

rustish: rustish.tab.o lex.yy.o parsetree.o
	${CC} ${OP} ${FLAGS} -o rustish rustish.tab.o lex.yy.o parsetree.o

parsetree.o: parsetree.cpp
	${CC} ${OP} ${FLAGS} -c parsetree.cpp

rustish.tab.o: rustish.tab.cpp
	${CC} ${OP} ${FLAGS} -c rustish.tab.cpp

#rustish.tab.cpp: rustish.tab.c
#	mv -f rustish.tab.c rustish.tab.cpp

rustish.tab.cpp: rustish.y 
	bison -v -d -o rustish.tab.cpp rustish.y

lex.yy.o: lex.yy.cpp
	${CC} $(OP) $(FLAGS) -Wno-sign-compare -c lex.yy.cpp 

lex.yy.cpp: rustish.l rustish.tab.hpp
	flex -o lex.yy.cpp rustish.l

clean:
	/bin/rm -f *.o *.yy.c *.yy.cpp *.tab.cpp *.tab.c *.tab.hpp \
	           core *.output *.temp *~ rustish 

