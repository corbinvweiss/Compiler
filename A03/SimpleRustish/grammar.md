# Simple Rustish
In the interests of creating a grammar for Rustish, we will begin with a massively simplified version and build up gradually.

```
<declaration> ::= <VarDecl>
                | <statement>
```

1. Variable Declarations
```
<VarDecl> ::= "let" "mut" <identifier> ":" <type> ";"
<identifier> : [A-Za-z_][0-9A-Za-z_]*
<type>: "i32" | "bool"
```
2. Statement
```
<statement> ::= <exprStmt>

<exprStmt>  ::= <expression> ";"
<expression> ::= <assignment>
<assignment> ::= IDENTIFIER "=" <assignment>
             |   <logic_or>
<logic_or>   ::= <logic_and> ()
```
3. 

the parser should output valid or invalid
