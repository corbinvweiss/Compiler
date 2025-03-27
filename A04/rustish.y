/* Rustish Parser */
/* Adapted from Dr. Halterman's examples */

%{
/* #define YYSTYPE double */

#include <iostream>
#include <limits>
#include "AST.h"

#define YYSTYPE ASTNode *

int yylex();
int yyparse();
void yyerror(const char *s);

extern FILE *yyin;
extern char* yytext;
%}

/* BISON Declarations */
%token MAIN LCURLY RCURLY IDENTIFIER SEMICOLON NUMBER ASSIGN PRINT COMMA
    PRINTLN ARROW COLON FN I32 BOOL LET MUT FALSE TRUE LPAREN RPAREN 
    PLUS MINUS TIMES DIVIDE MODULUS AND OR NOT IF ELSE WHILE RETURN
    LSQBRACK RSQBRACK NE EQ GT LT LE GE

%left OR                   /* Lowest precedence */
%left AND
%left EQ NE                 
%left GT LT LE GE
%left PLUS MINUS MODULUS    
%left TIMES DIVIDE          
%left NOT                  /* Higher precedence */
%%

/* Grammar rules */

input           : program {
                    auto node = $1;
                    std::cout << typeToString(node->get_type()) << '\n';
                    delete node;
                }
                ;

program         : main_def {
                    $$ = new ProgramNode($1);
                }
                ;

main_def        : FN MAIN LPAREN RPAREN LCURLY func_body RCURLY {
                    $$ = new MainDefNode($6);
                }
                ;

func_body       : local_decl_list {
                    $$ = new FuncBodyNode($1);
                }

local_decl_list : local_decl_list LET MUT var_decl SEMICOLON {
                    static_cast<LocalDeclListNode*>($1)->append($4);
                    $$ = $1;
                }
                | /* epsilon */ {
                    // Create a new empty local declaration list
                    $$ = new LocalDeclListNode();
                }
                ;

var_decl        : identifier COLON type {
                    $$ = new VarDeclNode($1, $3);
                }
                ;

identifier      : IDENTIFIER {
                    $$ = new IdentifierNode(yytext);
                }
                ;

type            : I32 {
                    $$ = new TypeNode(Type::i32);
                }
                | BOOL {
                    $$ = new TypeNode(Type::Bool);
                }

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