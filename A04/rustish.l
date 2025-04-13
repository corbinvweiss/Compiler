
/* Parse Rustish */

/* C++ declarations section */
%{

#include <iostream>
#include <string>
#include "rustish.tab.hpp"
extern int yylval;
void reset_cin();
void lex_error(int, std::string);
%}

%option noyywrap
%option yylineno

ws     [ ]
tab    \t
eol    \n


/* Rules Section */
%%

{ws}        {}
{tab}       {}
{eol}       {} 

"//".*{eol} {}

"let"       { return LET;}

"mut"       { return MUT;}

"fn"        { return FN;}

"if"        { return IF; }

"else"      { return ELSE; }

"while"     { return WHILE; }

"return"    { return RETURN; }

"i32"       { return I32; }

"bool"      { return BOOL;}

"false"     { return FALSE; }

"true"      { return TRUE; }

"main"      {return MAIN;}

"print"     {return PRINT;}

"println"   {return PRINTLN; }

\{          {return LCURLY;}

\}          {return RCURLY;}

\[          { return LSQBRACK; }

\]          { return RSQBRACK; }

\(          { return LPAREN; } 

\)          { return RPAREN; } 

\,          {return COMMA;}

":"         { return COLON;}

";"         { return SEMICOLON;}

"="         { return ASSIGN;}

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
            
.           {
               std::string msg = "unexpected symbol '";
               msg += yytext;
               msg += "'.";
               lex_error(yylineno, msg);
            }

%%

//int yywrap() {
//    return 0;
//}

void lex_error(int line, std::string msg) {
    std::cout << "error [line " << line << "]: "  << msg << '\n';
}


void yyerror(const char *s) {
    std::cout << s << '\n';
}

