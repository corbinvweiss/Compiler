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
    SEMICOLON EQUAL COLON PLUS MINUS TIMES DIVIDE LPAREN RPAREN
    IDENTIFIER NUMBER
    ERROR
%left PLUS MINUS
%left TIMES DIVIDE

/* Grammar rules */
%%

declaration : vardecl
            | statement;

vardecl     : LET MUT IDENTIFIER COLON type SEMICOLON {
                std::cout << "OK\n";
            };

type        : I32
            | BOOL;

statement   : exprstmt;
exprstmt    : expression SEMICOLON;
expression  : assignment;
assignment  : IDENTIFIER EQUAL rval {
                std::cout << "OK" << '\n';
            };
            
rval        : NUMBER
            | BOOL;


%%


int main() {
    while (true) {
        yyparse();
    }
}

