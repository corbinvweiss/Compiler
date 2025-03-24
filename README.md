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

The parser can handle up to what is included in test.ri.

## Questions:
1. At this point the grammar cannot handle a function with no parameters that has a return type such as 
```
fn test() -> bool {}
```