# Compiler
Code for Compiler Construction W25.

Compiling Rustish code into MIPS machine code.

The compiler is built up in phases. 
- A0 is a symbol table example
- A02 is a lexer that outputs tokens
- A03 is a parser that outputs a parsetree representation of code
- A04 constructs an Abstract Syntax tree.
- A05 is a complete level 5 compiler that adds support for strings.

## Run the Compiler
To run the compiler, navigate to the appropriate directory and execute the following commands:
```
make all
./rustish sample/test.ri
```
When you are done run
```
make clean
```
The rustish compiler should be able to handle all tests in the `sample/` directory. 

## Notes
For more details on any of the pieces of the compiler, see the readme in their directory.