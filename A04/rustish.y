/* Rustish Parser */
/* Adapted from Dr. Halterman's examples */

%{
/* #define YYSTYPE double */

#include <iostream>
#include <limits>
#include "AST.h"
#include "ErrorData.h"

#define YYSTYPE ASTNode *
#define YYERROR_VERBOSE 1

int yylex();
int yyparse();
void yyerror (char const *str);

extern FILE *yyin;
extern char* yytext;
extern char *lineptr;
%}

/* BISON Declarations */
%locations
%define parse.error verbose

%token MAIN LCURLY RCURLY IDENTIFIER SEMICOLON NUMBER ASSIGN PRINT COMMA
    PRINTLN ARROW COLON FN I32 BOOL LET MUT FALSE TRUE LPAREN RPAREN 
    PLUS MINUS TIMES DIVIDE MODULUS AND OR NOT IF ELSE WHILE RETURN
    LSQBRACK RSQBRACK NE EQ GT LT LE GE ERROR

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
                    $$ = new FuncDefListNode(ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

func_def        : FN identifier LPAREN params_list RPAREN ARROW type LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, $4, $7, $9, $10, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | FN identifier LPAREN params_list RPAREN LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, $4, nullptr, $7, $8, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | FN identifier LPAREN RPAREN ARROW type LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, nullptr, $6, $8, $9, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | FN identifier LPAREN RPAREN LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, nullptr, nullptr, $6, $7, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

params_list     : params_list COMMA var_decl {
                    // append new var_decl to params_list
                    static_cast<ParamsListNode*>($1)->append($3);
                    $$ = $1;
                } 
                | var_decl {
                    $$ = new ParamsListNode($1, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

main_def        : FN MAIN LPAREN RPAREN LCURLY local_decl_list statement_list RCURLY {
                    $$ = new MainDefNode($6, $7, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;


local_decl_list : local_decl_list LET MUT var_decl SEMICOLON {
                    static_cast<LocalDeclListNode*>($1)->append($4);
                    $$ = $1;
                }
                | /* epsilon */ {
                    // Create a new empty local declaration list
                    $$ = new LocalDeclListNode(ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

var_decl        : identifier COLON type {
                    $$ = new VarDeclNode($1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | identifier COLON LSQBRACK type SEMICOLON number RSQBRACK {
                    $$ = new ArrayDeclNode($1, $4, $6, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

statement_list  : statement_list statement {
                    static_cast<StatementListNode*>($1)->append($2);
                    $$ = $1;
                }
                | /* epsilon */ {
                    $$ = new StatementListNode(ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

statement       : identifier ASSIGN expression SEMICOLON {
                    $$ = new AssignmentStatementNode($1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | array_access ASSIGN expression SEMICOLON {
                    $$ = new AssignmentStatementNode($1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression SEMICOLON {
                    $$ = $1;
                }
                | RETURN expression SEMICOLON {
                    $$ = new ReturnNode($2, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | RETURN SEMICOLON {
                    $$ = new ReturnNode(ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | IF expression LCURLY statement_list RCURLY {
                    $$ = new IfStatementNode($2, $4, nullptr, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | IF expression LCURLY statement_list RCURLY 
                    ELSE LCURLY statement_list RCURLY {
                    $$ = new IfStatementNode($2, $4, $8, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | WHILE expression LCURLY statement_list RCURLY {
                    $$ = new WhileStatementNode($2, $4, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | PRINT LPAREN actual_args RPAREN SEMICOLON {
                    $$ = new PrintStatementNode($3, false, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | PRINTLN LPAREN actual_args RPAREN SEMICOLON {
                    $$ = new PrintStatementNode($3, true, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

func_call_expression : identifier LPAREN actual_args RPAREN {
                    $$ = new CallNode($1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }

actual_args     : expression {
                    $$ = new ActualArgsNode($1, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | actual_args COMMA expression {
                    static_cast<ActualArgsNode*>($1)->append($3);
                    $$ = $1;
                }
                | /* epsilon */ {
                    $$ = new ActualArgsNode(ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

expression      : func_call_expression {
                    $$ = $1;
                }
                | array_access {
                    $$ = $1;
                }               
                | number {
                    $$ = $1;
                }
                | bool {
                    $$ = $1;
                }
                | array_literal {
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
                | group {
                    $$ = $1;
                }
                ;

array_literal   : LSQBRACK expr_list RSQBRACK {
                    $$ = $2;
                }
                ;

expr_list       : expr_list COMMA expression {
                    static_cast<ArrayLiteralNode*>($1)->append($3);
                    $$ = $1;
                }
                | expression {
                    $$ = new ArrayLiteralNode($1, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

array_access    : identifier LSQBRACK expression RSQBRACK {
                    $$ = new ArrayAccessNode($1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }

group           : LPAREN expression RPAREN {
                    $$ = $2;
                }

binary          : expression PLUS expression {
                    $$ = new BinaryNode("+", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression MINUS expression {
                    $$ = new BinaryNode("-", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression TIMES expression {
                    $$ = new BinaryNode("*", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression DIVIDE expression {
                    $$ = new BinaryNode("/", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression MODULUS expression {
                    $$ = new BinaryNode("%", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression AND expression {
                    $$ = new BinaryNode("&&", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression OR expression {
                    $$ = new BinaryNode("||", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression EQ expression {
                    $$ = new BinaryNode("==", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression NE expression {
                    $$ = new BinaryNode("!=", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression LE expression {
                    $$ = new BinaryNode("<=", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression GE expression {
                    $$ = new BinaryNode(">=", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression GT expression {
                    $$ = new BinaryNode(">", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | expression LT expression {
                    $$ = new BinaryNode("<", $1, $3, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }

                ;
unary           : MINUS expression {
                    $$ = new UnaryNode("-", $2, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | PLUS expression {
                    $$ = new UnaryNode("+", $2, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | NOT expression {
                    $$ = new UnaryNode("!", $2, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

identifier      : IDENTIFIER {
                    $$ = new IdentifierNode(yytext, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

type            : I32 {
                    $$ = new TypeNode(Type::i32, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | BOOL {
                    $$ = new TypeNode(Type::Bool, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | LSQBRACK I32 RSQBRACK {
                    $$ = new TypeNode(Type::array_i32, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | LSQBRACK BOOL RSQBRACK {
                    $$ = new TypeNode(Type::array_bool, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

number          : NUMBER {
                    $$ = new NumberNode(atoi(yytext), ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                ;

bool            : TRUE {
                    $$ = new BoolNode(true, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
                }
                | FALSE {
                    $$ = new BoolNode(false, ErrorData(lineptr, yylloc.first_line, yylloc.first_column));
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

// from https://stackoverflow.com/questions/62115979/how-to-implement-better-error-messages-for-flex-bison
void yyerror(const char *str)
{
    fprintf(stderr,"error: %s in line %d, column %d\n", str, yylloc.first_line, yylloc.first_column);
    fprintf(stderr,"%s", lineptr);
    for(int i = 0; i < yylloc.first_column - 1; i++)
        fprintf(stderr,"_");
    fprintf(stderr,"^\n");
}