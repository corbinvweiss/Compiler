
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
static int next_column = 1;
int column = 0;
const int LEXER_ERROR = 1;

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

#define YY_USER_ACTION                        \
    yylloc.first_line = yylloc.last_line;     \
    yylloc.first_column = yylloc.last_column; \
    for (int i = 0; yytext[i] != '\0'; ++i) { \
        if (yytext[i] == '\n') {              \
            ++yylloc.last_line;               \
            yylloc.last_column = 1;           \
        } else {                              \
            ++yylloc.last_column;             \
        }                                     \
    }

%}

%option noyywrap
%option yylineno

ws     [ ]
tab    \t
eol    \n


/* Rules Section */
%%

{ws}        { }
{tab}       { }
{eol}       { next_column = 1; } 

"//".*{eol} { next_column -= strlen(yytext); }

"let"       { return LET;}

"mut"       { return MUT;}

"fn"        { return FN;}

"if"        { return IF; }

"else"      { return ELSE; }

"while"     { return WHILE; }

"return"    { return RETURN; }

"read"      { return READ; }

"i32"       { return I32; }

"bool"      { return BOOL;}

"false"     { return FALSE; }

"true"      { return TRUE; }

"main"      { return MAIN;}

"print"     { return PRINT;}

"println"   { return PRINTLN; }

".len"      { return LENGTH; }

\{          { return LCURLY;}

\}          { return RCURLY;}

\[          { return LSQBRACK; }

\]          { return RSQBRACK; }

\(          { return LPAREN; } 

\)          { return RPAREN; } 

\,          { return COMMA;}

":"         { return COLON;}

";"         { return SEMICOLON;}

"="         { return ASSIGN;}

"-="        { return MINUSASSIGN; }

"+="        { return PLUSASSIGN; }

\+          { return PLUS; } 

\-          { return MINUS; } 

\*          { return TIMES; } 

\/          { return DIVIDE; } 

\%          { return MODULUS; }

"&&"        { return AND; }

"||"        { return OR; }

\!          { return NOT; }

"=="        { return EQ; }

"!="        { return NE; }

\<          { return LT; }

\>          { return GT; }

"<="        { return LE; }

">="        { return GE; }

"->"        { return ARROW; }

[A-Za-z_][0-9A-Za-z_]* { /* identifier */
                
                return IDENTIFIER;
            }
0|[1-9][0-9]* {
                
                yylval = std::stoi(yytext);
                return NUMBER;
            }

. { 
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
    fprintf(stderr,"unexpected symbol '%s' in line %d, column %d:\n", str, yylloc.first_line, yylloc.first_column);
    fprintf(stderr,"%s", lineptr);
    for(int i = 0; i < yylloc.first_column - 1; i++)
        fprintf(stderr,"_");
    fprintf(stderr,"^\n");
    exit(LEXER_ERROR);
}

size_t min(size_t a, size_t b) {
    return b < a ? b : a;
}
