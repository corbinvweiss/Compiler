# Code Generation
This is the final phase of the rustish compiler. Here we will generate MIPS code from the Rustish source code.

## Plan
- Add a stack offset to the Symbol Table
- Add an EmitCode method to all of the ASTNodes
- Create macros for the commonly used MIPS code such as
    - store register (to the stack)
    - restore register
