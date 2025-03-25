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
                    node->show(0);
                    delete node;
                }
                ;

program         : func_def_list main_def {
                    $$ = new ProgramNode($1, $2);
                }
                ;

func_def_list   : func_def_list func_def {
                    // append the new func_def to the func_def_list
                    // Cast $1 from ParseTreeNode* to FuncDefListNode* before calling append.
                    static_cast<FuncDefListNode*>($1)->append($2);
                    $$ = $1;    // return the func_def_list
                }
                | /* epsilon */ {
                    // Create a new, empty function definition list.
                    $$ = new FuncDefListNode();
                }
                ;

func_def        : FN identifier LPAREN params_list RPAREN ARROW type func_body {
                    $$ = new FuncDefNode($2, $4, $7, $8);
                }
                | FN identifier LPAREN params_list RPAREN func_body {
                    $$ = new FuncDefNode($2, $4, $6);
                }
                | FN identifier LPAREN RPAREN func_body {
                    $$ = new FuncDefNode($2, $5);
                }
                ;

main_def        : FN MAIN LPAREN RPAREN func_body {
                    $$ = new MainDefNode($5);
                }
                ;

params_list     : params_list COMMA var_decl {
                    // append new var_decl to params_list
                    // cast $1 to instance of params_list before appending
                    static_cast<ParamsListNode*>($1)->append($3);
                    $$ = $1;     // return the params_list
                }
                | var_decl {
                    $$ = new ParamsListNode($1);
                }
                ;

var_decl        : identifier COLON type {
                    $$ = new VarDeclNode($1, $3);
                }
                ;

type            : I32 {
                    $$ = new TypeNode(Type::_I32);
                }
                | BOOL {
                    $$ = new TypeNode(Type::_BOOL);
                }
                | LSQBRACK I32 RSQBRACK {
                    $$ = new TypeNode(Type::ARRAY_I32);
                }
                | LSQBRACK BOOL RSQBRACK {
                    $$ = new TypeNode(Type::ARRAY_BOOL);
                }
                ;

func_body       : LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncBodyNode($2, $3);
                }
                ;

local_decl_list : local_decl_list LET MUT var_decl SEMICOLON {

                    static_cast<LocalDeclListNode*>($1)->append($4);

                    // Return the updated local_decl_list
                    $$ = $1;
                }
                | /* epsilon */ {
                    // Create a new empty local declaration list
                    $$ = new LocalDeclListNode();
                }
                ;

statement_list  : statement_list statement {
                    static_cast<StatementListNode*>($1)->append($2);
                    $$ = $1;
                }
                | /* epsilon */ {
                    $$ = new StatementListNode();
                }
                ;

statement       : expression SEMICOLON {
                    $$ = new StatementNode($1);
                }
                | identifier ASSIGN expression SEMICOLON {
                    $$ = new AssignmentStatementNode($1, $3);
                }
                | PRINT LPAREN actual_args RPAREN SEMICOLON {
                    $$ = new PrintStatementNode($3);
                }
                | PRINTLN LPAREN actual_args RPAREN SEMICOLON {
                    $$ = new PrintlnStatementNode($3);
                }
                | IF expression LCURLY statement_list RCURLY {
                    $$ = new IfStatementNode($2, $4);
                }
                | IF expression LCURLY statement_list RCURLY 
                    ELSE LCURLY statement_list RCURLY {
                    $$ = new IfElseStatementNode($2, $4, $8);
                }
                | WHILE expression LCURLY statement_list RCURLY {
                    $$ = new WhileStatementNode($2, $4);
                }
                | func_call_expression SEMICOLON {
                    $$ = $1;
                }
                ;

func_call_expression : identifier LPAREN actual_args RPAREN {
                    $$ = new CallNode($1, $3);
                }

actual_args     : expression {
                    $$ = new ActualArgsNode($1);
                }
                | actual_args COMMA expression {
                    static_cast<ActualArgsNode *>($1)->append($3);
                }
                | /* epsilon */ {
                    $$ = new ActualArgsNode();
                }

expression      : NUMBER {
                    $$ = new NumberNode(atoi(yytext));
                }
                | bool {
                    $$ = $1;
                }
                | identifier {
                    $$ = $1;
                }
                | unary {
                    $$ = $1;
                }
                | binary {
                    $$ = $1;
                }
                ;

binary          : expression PLUS expression {
                    $$ = new BinaryNode(Operator::_PLUS, $1, $3);
                }
                | expression MINUS expression {
                    $$ = new BinaryNode(Operator::_MINUS, $1, $3);
                }
                | expression TIMES expression {
                    $$ = new BinaryNode(Operator::_TIMES, $1, $3);
                }
                | expression DIVIDE expression {
                    $$ = new BinaryNode(Operator::_DIVIDE, $1, $3);
                }
                | expression MODULUS expression {
                    $$ = new BinaryNode(Operator::_MODULUS, $1, $3);
                }
                | expression AND expression {
                    $$ = new BinaryNode(Operator::_AND, $1, $3);
                }
                | expression OR expression {
                    $$ = new BinaryNode(Operator::_OR, $1, $3);
                }
                | expression EQ expression {
                    $$ = new BinaryNode(Operator::_EQ, $1, $3);
                }
                | expression NE expression {
                    $$ = new BinaryNode(Operator::_NE, $1, $3);
                }
                | expression LE expression {
                    $$ = new BinaryNode(Operator::_LE, $1, $3);
                }
                | expression GE expression {
                    $$ = new BinaryNode(Operator::_GE, $1, $3);
                }
                | expression GT expression {
                    $$ = new BinaryNode(Operator::_GT, $1, $3);
                }
                | expression LT expression {
                    $$ = new BinaryNode(Operator::_LT, $1, $3);
                }
                ;

unary           : MINUS expression {
                    $$ = new UnaryNode(Operator::_MINUS, $2);
                }
                | PLUS expression {
                    $$ = new UnaryNode(Operator::_PLUS, $2);
                }
                | NOT expression {
                    $$ = new UnaryNode(Operator::_NOT, $2);
                }
                ;

bool            : TRUE {
                    $$ = new BoolNode(true);
                }
                | FALSE {
                    $$ = new BoolNode(false);
                }

identifier      : IDENTIFIER {
                    $$ = new IdentifierNode(yytext);
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