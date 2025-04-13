
/* Parse Rustish */

/* C++ declarations section */
%{

#include <iostream>
#include <string>
#include "rustish.tab.hpp"
extern int yylval;
void reset_cin();
void lex_error(const char* str);

// from https://stackoverflow.com/questions/62115979/how-to-implement-better-error-messages-for-flex-bison
static int next_column = 2;
int column = 1;
const int LEXER_ERROR = 1;

#define HANDLE_COLUMN column = next_column; next_column += strlen(yytext);

char *lineptr = NULL;
size_t n = 0;
size_t consumed = 0;
size_t available = 0;

size_t min(size_t a, size_t b);
#define YY_INPUT(buf,result,max_size) {\
    if(available <= 0) {\
        consumed = 0;\
        available = getline(&lineptr, &n, yyin);\
        if (available < 0) {\
            if (ferror(yyin)) { perror("read error:"); }\
                available = 0;\
            }\
    }\
    result = min(available, max_size);\
    strncpy(buf, lineptr + consumed, result);\
    consumed += result;\
    available -= result;\
}

%}

%option noyywrap
%option yylineno

ws     [ ]
tab    \t
eol    \n
VALID [a-zA-Z0-9_()\[\]{}=+\-*/%&!;:, ]


/* Rules Section */
%%

{ws}        {HANDLE_COLUMN; }
{tab}       {HANDLE_COLUMN; }
{eol}       {HANDLE_COLUMN; next_column = 2; } 

"//".*{eol} {}

"let"       { HANDLE_COLUMN; return LET;}

"mut"       { HANDLE_COLUMN; return MUT;}

"fn"        { HANDLE_COLUMN; return FN;}

"if"        { HANDLE_COLUMN; return IF; }

"else"      { HANDLE_COLUMN; return ELSE; }

"while"     { HANDLE_COLUMN; return WHILE; }

"return"    { HANDLE_COLUMN; return RETURN; }

"i32"       { HANDLE_COLUMN; return I32; }

"bool"      { HANDLE_COLUMN; return BOOL;}

"false"     { HANDLE_COLUMN; return FALSE; }

"true"      { HANDLE_COLUMN; return TRUE; }

"main"      { HANDLE_COLUMN; return MAIN;}

"print"     { HANDLE_COLUMN; return PRINT;}

"println"   { HANDLE_COLUMN; return PRINTLN; }

\{          { HANDLE_COLUMN; return LCURLY;}

\}          { HANDLE_COLUMN; return RCURLY;}

\[          { HANDLE_COLUMN; return LSQBRACK; }

\]          { HANDLE_COLUMN; return RSQBRACK; }

\(          { HANDLE_COLUMN; return LPAREN; } 

\)          { HANDLE_COLUMN; return RPAREN; } 

\,          { HANDLE_COLUMN; return COMMA;}

":"         { HANDLE_COLUMN; return COLON;}

";"         { HANDLE_COLUMN; return SEMICOLON;}

"="         { HANDLE_COLUMN; return ASSIGN;}

\+          { HANDLE_COLUMN; return PLUS; } 

\-          { HANDLE_COLUMN; return MINUS; } 

\*          { HANDLE_COLUMN; return TIMES; } 

\/          { HANDLE_COLUMN; return DIVIDE; } 

\%          { HANDLE_COLUMN; return MODULUS; }

"&&"        { HANDLE_COLUMN; return AND; }

"||"        { HANDLE_COLUMN; return OR; }

\!          { HANDLE_COLUMN; return NOT; }

"=="        { HANDLE_COLUMN; return EQ; }

"!="        { HANDLE_COLUMN; return NE; }

\<          { HANDLE_COLUMN; return LT; }

\>          { HANDLE_COLUMN; return GT; }

"<="        { HANDLE_COLUMN; return LE; }

">="        { HANDLE_COLUMN; return GE; }

"->"        { HANDLE_COLUMN; return ARROW; }

[A-Za-z_][0-9A-Za-z_]* { /* identifier */
                return IDENTIFIER;
            }
0|[1-9][0-9]* {
                yylval = std::stoi(yytext);
                return NUMBER;
            }

. { HANDLE_COLUMN;
    if (yytext[0] == '\0') {
        // Could be EOF or some internal Flex error
        return 0;
    }
    lex_error(yytext);
}

%%

//int yywrap() {
//    return 0;
//}

void lex_error(const char *str) {
    fprintf(stderr,"unexpected symbol '%s' in line %d, column %d:\n", str, yylineno, column);
    fprintf(stderr,"%s", lineptr);
    for(int i = 0; i < column - 1; i++)
        fprintf(stderr,"_");
    fprintf(stderr,"^\n");
    exit(LEXER_ERROR);
}

size_t min(size_t a, size_t b) {
    return b < a ? b : a;
}
