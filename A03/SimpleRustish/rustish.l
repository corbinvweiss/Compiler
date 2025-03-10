
/* Parse Rustish */

/* C++ declarations section */
%{

#include <iostream>
#include <string>
#include "rustish.tab.hpp"
extern int yylval;
void reset_cin();

%}

%option noyywrap

ws     [ ]
tab    \t
eol    \n


/* Rules Section */
%%

{ws}        {}
{tab}       {}
{eol}       { return 0; } 

":"         { return COLON;}

";"         { return SEMICOLON;}

"="         { return ASSIGN;}

\+          { return PLUS; } 

\-          { return MINUS; } 

\*          { return TIMES; } 

\/          { return DIVIDE; } 

\(          { return LPAREN; } 

\)          { return RPAREN; } 

"=="        { return EQUAL;}
"!="        { return NEQ;}
"||"        { return OR;}
"&&"        { return AND;}
"<"         { return LT;}
">"         { return GT;}
">="        { return GEQ;}
"<="        { return LEQ;}
"!"         { return BANG;}

"let"       { return LET;}

"mut"       { return MUT;}

"i32"       { return I32; }
"bool"      { return BOOL;}


("q"|"Q")   {
                std::exit(0);
            }

[A-Za-z_][0-9A-Za-z_]* { /* identifier */
                return IDENTIFIER;
            }
[1-9][0-9]* {
                yylval = std::stoi(yytext);
                return NUMBER;
            }
            
.           { 
               /*yytext[0];*/ 
            }

%%

//int yywrap() {
//    return 0;
//}


void yyerror(const char *s) {
    std::cout << s << '\n';
}

