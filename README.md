# Compiler
Code for Compiler Construction W25

## Parsing
The code for the parsing assignment is in A03/SimpleRustish. The intent is to start small and gradually expand the grammar.

To run the parser run the following commands in A03/SimpleRustish:
```
make all
./rustish sample/tst.ri
```
When you are done run
```
make clean
```

The parser can handle all of the code included in `sample/`

## Arrays
```
let mut b: i32;
let mut a: [i32; 5];
a[2] = 0;
b = a[2];
```
The grammar rule for an array declaration needs to be 
```
array_decl  : LET MUT identifier COLON LBRACKET type SEMICOLON number RBRACKET SEMICOLON
```
The rule for assignment to an array
```
assignment_statement: array_access ASSIGN expression SEMICOLON
```
The rule for assignment from array
```
assignment_statement: identifier ASSIGN array_access SEMICOLON
```
where 
```
array_access : identifier LSQBRACK expression RSQBRACK
```

In this case the assignment from an array is just like any other assignment.