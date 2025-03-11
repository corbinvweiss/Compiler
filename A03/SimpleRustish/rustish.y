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
%token MAIN LCURLY RCURLY IDENTIFIER SEMICOLON NUMBER ASSIGN PRINT COMMA
    PRINTLN ARROW COLON FN I32 BOOL LET MUT FALSE TRUE LPAREN RPAREN 
    PLUS MINUS TIMES DIVIDE MODULUS AND OR NOT IF ELSE WHILE RETURN
    LSQBRACK RSQBRACK NE EQ GT LT LE GE
/* Grammar rules */
%%

/* This program will accept a program made up of empty functions */

program         : func_def_list main_def
                ;

func_def_list   : func_def_list func_def
                | epsilon
                ;

func_def        : FN IDENTIFIER LPAREN RPAREN ARROW func_body
                ;

main_def        : FN MAIN LPAREN RPAREN func_body
                ;

func_body       : LCURLY RCURLY
                ;

epsilon         :
                ;

%%


int main() {
    // todo: pass in a file here to parse with the grammar. 
    while (true) {
        yyparse();
    }
}

