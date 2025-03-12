# Compiler
Code for Compiler Construction W25

## Parsing
The code for the parsing assignment is in A03/SimpleRustish. The intent is to start small and gradually expand the grammar.

To run the parser run the following commands in A03/SimpleRustish:
```
make all
./rustish test.ri
```
When you are done run
```
make clean
```

Right now the parser can only handle a main function, and only outputs "OK" or "syntax error". 