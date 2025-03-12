/* Rustish Parser */
/* Adapted from Dr. Halterman's examples */

%{
/* #define YYSTYPE double */

#include <iostream>
#include <limits>
#include "parsetree.h"

#define YYSTYPE ParseTreeNode *

int yylex();
int yyparse();
void yyerror(const char *s);

extern FILE *yyin;
%}

/* BISON Declarations */
%token MAIN LCURLY RCURLY IDENTIFIER SEMICOLON NUMBER ASSIGN PRINT COMMA
    PRINTLN ARROW COLON FN I32 BOOL LET MUT FALSE TRUE LPAREN RPAREN 
    PLUS MINUS TIMES DIVIDE MODULUS AND OR NOT IF ELSE WHILE RETURN
    LSQBRACK RSQBRACK NE EQ GT LT LE GE
/* Grammar rules */
%%

/* This program will accept a program made up of empty functions */

input           : program {
                    auto node = $1;
                    node->show(0);
                    delete node;
                }

program         : main_def
                ;

main_def        : FN MAIN LPAREN RPAREN func_body {
                    $$ = new MainDefNode($5);
                }
                ;

func_body       : LCURLY RCURLY {
                    $$ = new FuncBodyNode();
                }
                ;

%%


int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Error opening file");
        return 1;
    }

    yyparse();  // Call the Bison parser

    fclose(yyin);
    return 0;
}