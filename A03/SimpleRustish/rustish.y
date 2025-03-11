/* Rustish Parser */
/* Adapted from Dr. Halterman's examples */

%{
/* #define YYSTYPE double */

#include <iostream>
#include <limits>

int yylex();
int yyparse();
void yyerror(const char *s);
%}

/* BISON Declarations */
%token LET MUT I32 BOOL 
    SEMICOLON ASSIGN COLON PLUS MINUS TIMES DIVIDE LPAREN RPAREN
    IDENTIFIER NUMBER
    OR AND NEQ EQUAL GT LT GEQ LEQ BANG 
    ERROR
%left OR
%left AND
%left EQUAL NEQ
%left GT LT GEQ LEQ
%left PLUS MINUS
%left TIMES DIVIDE

/* Grammar rules */
%%

declaration : vardecl
            | statement
            {
                std::cout << "declaration\n";
            }
            ;

vardecl     : LET MUT IDENTIFIER COLON type SEMICOLON {
                std::cout << "OK\n";
            }
            ;

type        : I32
            | BOOL
            ;

statement   : exprstmt
            {
                std::cout << "statement\n";
            }
            ;
exprstmt    : expression SEMICOLON
            {
                std::cout << "exprstmt\n";
            }
            ;
            
expression  : assignment
            {
                std::cout << "expression\n";
            }
            ;
assignment  : logic_or
            {
                std::cout << "1\n";
            }
            ;

logic_or    : logic_or OR logic_and
            | logic_and
            {
                std::cout << "2\n";
            }
            ;

logic_and   : logic_and AND equality
            | equality
            {
                std::cout << "3\n";
            }
            ;
            
equality    : equality eq comparison
            | comparison 
            {
                std::cout << "4\n";
            }
            ;

eq          : EQUAL
            | NEQ
            ;

comparison  : comparison comp term 
            | term
            {
                std::cout << "5\n";
            }
            ;
comp        : GT
            | LT
            | GEQ
            | LEQ
            ;

term        : term pm factor
            | factor
            {
                std::cout << "6\n";
            }
            ;
pm          : PLUS
            | MINUS
            ;

factor      : factor fact unary
            | unary
            {
                std::cout << "7\n";
            }
            ;
fact        : TIMES
            | DIVIDE
            ;

unary       : un unary
            | call
            {
                std::cout << "8\n";
            }
            ;
un          : BANG
            | MINUS
            ;

call        : primary
            {
                std::cout << "9\n";
            }
            ;

primary     : BOOL
            | NUMBER
            | IDENTIFIER
            ;
%%


int main() {
    while (true) {
        yyparse();
    }
}

