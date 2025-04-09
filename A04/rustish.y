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
extern int yylineno;
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
                    delete node;
                }
                ;

program         : func_def_list main_def {
                    $$ = new ProgramNode($1, $2);
                }
                ;

func_def_list   : func_def_list func_def {
                    // append func_def to func_def_list
                    static_cast<FuncDefListNode*>($1)->append($2);
                    $$ = $1;
                }
                | /* epsilon */ {
                    // Create a new empty function definition list
                    $$ = new FuncDefListNode(yylineno);
                }
                ;

func_def        : FN identifier LPAREN params_list RPAREN ARROW type LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, $4, $7, $9, $10, yylineno);
                }
                ;

params_list     : params_list COMMA var_decl {
                    // append new var_decl to params_list
                    static_cast<ParamsListNode*>($1)->append($3);
                    $$ = $1;
                } 
                | var_decl {
                    $$ = new ParamsListNode($1, yylineno);
                }
                ;

main_def        : FN MAIN LPAREN RPAREN LCURLY local_decl_list statement_list RCURLY {
                    $$ = new MainDefNode($6, $7, yylineno);
                }
                ;


local_decl_list : local_decl_list LET MUT var_decl SEMICOLON {
                    static_cast<LocalDeclListNode*>($1)->append($4);
                    $$ = $1;
                }
                | /* epsilon */ {
                    // Create a new empty local declaration list
                    $$ = new LocalDeclListNode(yylineno);
                }
                ;

var_decl        : identifier COLON type {
                    $$ = new VarDeclNode($1, $3, yylineno);
                }
                ;

statement_list  : statement_list statement {
                    static_cast<StatementListNode*>($1)->append($2);
                    $$ = $1;
                }
                | /* epsilon */ {
                    $$ = new StatementListNode(yylineno);
                }
                ;

statement       : identifier ASSIGN expression SEMICOLON {
                    $$ = new AssignmentStatementNode($1, $3, yylineno);
                }
                | expression SEMICOLON {
                    $$ = $1;
                }
                ;

func_call_expression : identifier LPAREN actual_args RPAREN {
                    $$ = new CallNode($1, $3, yylineno);
                }

actual_args     : expression {
                    $$ = new ActualArgsNode($1, yylineno);
                }
                | actual_args COMMA expression {
                    static_cast<ActualArgsNode*>($1)->append($3);
                    $$ = $1;
                }
                | /* epsilon */ {
                    $$ = new ActualArgsNode(yylineno);
                }
                ;

expression      : func_call_expression {
                    $$ = $1;
                }
                | literal {
                    $$ = $1;
                }
                | identifier {
                    $$ = $1;
                }
                ;

identifier      : IDENTIFIER {
                    $$ = new IdentifierNode(yytext, yylineno);
                }
                ;

type            : I32 {
                    $$ = new TypeNode(Type::i32, yylineno);
                }
                | BOOL {
                    $$ = new TypeNode(Type::Bool, yylineno);
                }
                ;

literal         : NUMBER {
                    $$ = new LiteralNode(atoi(yytext), yylineno);
                }
                | TRUE {
                    $$ = new LiteralNode(true, yylineno);
                }
                | FALSE {
                    $$ = new LiteralNode(false, yylineno);
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