# Rustish Grammar

Adapted from [Crafting Interpreters](https://craftinginterpreters.com/appendix-i.html)

TODO: add arrays. Where would they fit?

```
<program>       ::= <declaration>* EOF

<declaration>   ::= <funDeclaration>
                |   <varDeclaration>
                |   <statement>

<funDeclaration>::= "fn" <identifier>"("(<identifer>":" <type>)*")" "->" <type> <block>
<varDeclaration>::= "let" "mut" <identifier> ":" <type> ";"


<statement>     ::= <exprStmt>
                |   <ifStmt>
                |   <printStmt>
                |   <returnStmt>
                |   <whileStmt>
                |   <block>

<exprStmt>      ::= <expression> ";"
<ifStmt>        ::= "if" <expression> <block>
<printStmt>     ::= "print" <expression> ";"
<returnStmt>    ::= "return" <expression>? ";"
<whileStmt>     ::= "while" <expression> <block>
<block>         ::= "{" <statement>* "}"


<expression>    ::= <assignment>
<assignment>    ::= IDENTIFIER "=" <logic_or>
<logic_or>      ::= <logic_and> ("||" <logic_and>)*
<logic_and>     ::= <equality> ("&&" <equality>)*
<equality>      ::= <comparison> (("!=" | "==") <comparison>)*
<comparison>    ::= <term> ((">" | ">=" | "<" | "<=") <term>)*
<term>          ::= <factor> (("+" | "-") <factor>)*
<factor>        ::= <unary> (("*" | "/") <unary>)*
<unary>         ::= ("!" | "-") <unary> | <call>
<call>          ::= <primary> ("("(<expression> ("," <expression>)*)*")")*
<primary>       ::= BOOLEAN | NUMBER | IDENTIFIER

BOOLEAN         ::= "true" | "false"
IDENTIFER       ::= [alpha][alnum]*
NUMBER          ::= [1-9][0-9]* | "0"


```