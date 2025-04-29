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
#define ERRDATA ErrorData(lineptr, yylloc.first_line, yylloc.first_column)

int yylex();
int yyparse();
void yyerror (char const *str);

FILE *fdout; // global file descriptor for output MIPS code file
extern FILE *yyin;
extern char* yytext;
extern char *lineptr;
%}

/* BISON Declarations */
%locations
%define parse.error verbose

%token MAIN LCURLY RCURLY IDENTIFIER SEMICOLON NUMBER ASSIGN PRINT COMMA
    PRINTLN LENGTH ARROW COLON FN I32 BOOL LET MUT FALSE TRUE LPAREN RPAREN 
    PLUS MINUS TIMES DIVIDE MODULUS AND OR NOT IF ELSE WHILE RETURN
    LSQBRACK RSQBRACK NE EQ GT LT LE GE ERROR MINUSASSIGN PLUSASSIGN READ
    SINGLEQUOTE CHAR CHARTYPE

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
                    $$ = new ProgramNode($1, $2, fdout);
                }
                ;

func_def_list   : func_def_list func_def {
                    // append func_def to func_def_list
                    static_cast<FuncDefListNode*>($1)->append($2);
                    $$ = $1;
                }
                | /* epsilon */ {
                    // Create a new empty function definition list
                    $$ = new FuncDefListNode(ERRDATA);
                }
                ;

func_def        : FN identifier LPAREN params_list RPAREN ARROW type LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, $4, $7, $9, $10, ERRDATA);
                }
                | FN identifier LPAREN params_list RPAREN LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, $4, nullptr, $7, $8, ERRDATA);
                }
                | FN identifier LPAREN RPAREN ARROW type LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, nullptr, $6, $8, $9, ERRDATA);
                }
                | FN identifier LPAREN RPAREN LCURLY local_decl_list statement_list RCURLY {
                    $$ = new FuncDefNode($2, nullptr, nullptr, $6, $7, ERRDATA);
                }
                ;

params_list     : params_list COMMA var_decl {
                    // append new var_decl to params_list
                    static_cast<ParamsListNode*>($1)->append($3);
                    $$ = $1;
                } 
                | var_decl {
                    $$ = new ParamsListNode($1, ERRDATA);
                }
                ;

main_def        : FN MAIN LPAREN RPAREN LCURLY local_decl_list statement_list RCURLY {
                    $$ = new MainDefNode($6, $7, ERRDATA);
                }
                ;


local_decl_list : local_decl_list LET MUT var_decl SEMICOLON {
                    static_cast<LocalDeclListNode*>($1)->append($4);
                    $$ = $1;
                }
                | /* epsilon */ {
                    // Create a new empty local declaration list
                    $$ = new LocalDeclListNode(ERRDATA);
                }
                ;

var_decl        : identifier COLON type {
                    $$ = new VarDeclNode($1, $3, ERRDATA);
                }
                | identifier COLON LSQBRACK type SEMICOLON number RSQBRACK {
                    $$ = new ArrayDeclNode($1, $4, $6, ERRDATA);
                }
                ;

statement_list  : statement_list statement {
                    static_cast<StatementListNode*>($1)->append($2);
                    $$ = $1;
                }
                | /* epsilon */ {
                    $$ = new StatementListNode(ERRDATA);
                }
                ;

statement       : identifier ASSIGN expression SEMICOLON {
                    $$ = new AssignmentStatementNode($1, $3, ERRDATA);
                }
                | array_access ASSIGN expression SEMICOLON {
                    $$ = new AssignmentStatementNode($1, $3, ERRDATA);
                }
                | identifier MINUSASSIGN expression SEMICOLON {
                    IdentifierNode* id = new IdentifierNode(static_cast<IdentifierNode&>(*$1));
                    $$ = new AssignmentStatementNode($1, new BinaryNode("-", id, $3, ERRDATA), ERRDATA);
                }
                | identifier PLUSASSIGN expression SEMICOLON {
                    IdentifierNode* id = new IdentifierNode(static_cast<IdentifierNode&>(*$1));
                    $$ = new AssignmentStatementNode($1, new BinaryNode("+", id, $3, ERRDATA), ERRDATA);
                }
                | expression SEMICOLON {
                    $$ = $1;
                }
                | RETURN expression SEMICOLON {
                    $$ = new ReturnNode($2, ERRDATA);
                }
                | RETURN SEMICOLON {
                    $$ = new ReturnNode(ERRDATA);
                }
                | IF expression LCURLY statement_list RCURLY {
                    $$ = new IfStatementNode($2, $4, nullptr, ERRDATA);
                }
                | IF expression LCURLY statement_list RCURLY 
                    ELSE LCURLY statement_list RCURLY {
                    $$ = new IfStatementNode($2, $4, $8, ERRDATA);
                }
                | WHILE expression LCURLY statement_list RCURLY {
                    $$ = new WhileStatementNode($2, $4, ERRDATA);
                }
                | PRINT LPAREN actual_args RPAREN SEMICOLON {
                    $$ = new PrintStatementNode($3, false, ERRDATA);
                }
                | PRINTLN LPAREN actual_args RPAREN SEMICOLON {
                    $$ = new PrintStatementNode($3, true, ERRDATA);
                }
                | SEMICOLON {
                    $$ = nullptr;
                }
                ;

func_call_expression : identifier LPAREN actual_args RPAREN {
                    $$ = new CallNode($1, $3, ERRDATA);
                }

actual_args     : expression {
                    $$ = new ActualArgsNode($1, ERRDATA);
                }
                | actual_args COMMA expression {
                    static_cast<ActualArgsNode*>($1)->append($3);
                    $$ = $1;
                }
                | /* epsilon */ {
                    $$ = new ActualArgsNode(ERRDATA);
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
                | char {
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
                | length {
                    $$ = $1;
                }
                | READ LPAREN RPAREN {
                    $$ = new ReadNode(ERRDATA);
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
                    $$ = new ArrayLiteralNode($1, ERRDATA);
                }
                ;

array_access    : identifier LSQBRACK expression RSQBRACK {
                    $$ = new ArrayAccessNode($1, $3, ERRDATA);
                }

group           : LPAREN expression RPAREN {
                    $$ = $2;
                }

length          : identifier LENGTH {
                    $$ = new LengthNode($1, ERRDATA);
                }

binary          : expression PLUS expression {
                    $$ = new BinaryNode("+", $1, $3, ERRDATA);
                }
                | expression MINUS expression {
                    $$ = new BinaryNode("-", $1, $3, ERRDATA);
                }
                | expression TIMES expression {
                    $$ = new BinaryNode("*", $1, $3, ERRDATA);
                }
                | expression DIVIDE expression {
                    $$ = new BinaryNode("/", $1, $3, ERRDATA);
                }
                | expression MODULUS expression {
                    $$ = new BinaryNode("%", $1, $3, ERRDATA);
                }
                | expression AND expression {
                    $$ = new BinaryNode("&&", $1, $3, ERRDATA);
                }
                | expression OR expression {
                    $$ = new BinaryNode("||", $1, $3, ERRDATA);
                }
                | expression EQ expression {
                    $$ = new BinaryNode("==", $1, $3, ERRDATA);
                }
                | expression NE expression {
                    $$ = new BinaryNode("!=", $1, $3, ERRDATA);
                }
                | expression LE expression {
                    $$ = new BinaryNode("<=", $1, $3, ERRDATA);
                }
                | expression GE expression {
                    $$ = new BinaryNode(">=", $1, $3, ERRDATA);
                }
                | expression GT expression {
                    $$ = new BinaryNode(">", $1, $3, ERRDATA);
                }
                | expression LT expression {
                    $$ = new BinaryNode("<", $1, $3, ERRDATA);
                }

                ;
unary           : MINUS expression {
                    $$ = new UnaryNode("-", $2, ERRDATA);
                }
                | PLUS expression {
                    $$ = new UnaryNode("+", $2, ERRDATA);
                }
                | NOT expression {
                    $$ = new UnaryNode("!", $2, ERRDATA);
                }
                ;

identifier      : IDENTIFIER {
                    $$ = new IdentifierNode(yytext, ERRDATA);
                }
                ;

type            : I32 {
                    $$ = new TypeNode(Type::i32, ERRDATA);
                }
                | BOOL {
                    $$ = new TypeNode(Type::Bool, ERRDATA);
                }
                | CHARTYPE {
                    $$ = new TypeNode(Type::Char, ERRDATA);
                }
                | LSQBRACK I32 RSQBRACK {
                    $$ = new TypeNode(Type::array_i32, ERRDATA);
                }
                | LSQBRACK BOOL RSQBRACK {
                    $$ = new TypeNode(Type::array_bool, ERRDATA);
                }
                ;

number          : NUMBER {
                    $$ = new NumberNode(atoi(yytext), ERRDATA);
                }
                ;

bool            : TRUE {
                    $$ = new BoolNode(true, ERRDATA);
                }
                | FALSE {
                    $$ = new BoolNode(false, ERRDATA);
                }
                ;

char            : CHAR {
                    $$ = new CharNode(yytext, ERRDATA);
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

    fdout = fopen("a.s", "w");

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