# Abstract Syntax Tree

See the assignment [here](https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/typecheck.html).

## Testing
To test the Abstract Syntax Tree code, run
```
make all
./rustish sample/<filename>

...
make clean
```

## Structure
```
rustish.y
    - rustish.l
    - AST.h
        - SymbolTable.h
            - SymbolInfo.h

```

## Type Checking
The following types are used:
- bool
- i32
- [bool]
- [i32]
- none
- any (used for == and != operators which can operate on bools or ints)

Every ASTNode has a return type, a pointer to the global and local symbol table, and a TypeCheck() method. Type checking is done in the following way:
1. Symbol Table initialization
    1. ProgramNode creates a Global symbol table and passes it to each function and the main function.
    2. Each function creates its own local symbol table and passes the global and local symbol table to its declarations and statements.
    3. Each declaration and statement holds a pointer to the global and local symbol tables.
2. CheckType() recursively called.
    1. ProgramNode calls CheckType() on all functions.
    2. Each function puts itself in the global symbol table, then calls CheckType on its declarations and statements.
    3. Each declaration puts itself in the local symbol table, checking for redeclarations as it goes.
    4. Each statement checks its type.

### Notes:
- Function type checking includes checking return type. 
    - Find all return statements in the function and checking that their return types match the return type of the function.
    - This does not check all code paths, but it does make sure all returns are valid.

- Error reporting is done with the global `error` function which simply prints the line number and the error message.
- All semantic errors are reported without short-circuit evaluation. For example
```
7 > false;
---
error [line 13]: incompatible types 'i32' and 'bool'.
error [line 13]: rtype 'bool' not compatible with operator '>'.
```

## Arrays
This AST constructor supports arrays:
```
let mut arr: [i32; 5];
```
creates an array of 5 integers.

It supports both indexed and literal initialization:
```
arr[0] = 5;                // indexed
arr = [0, 1, 2, 3, f(4)]   // literal initialization. Valid provided f returns an i32.
```
- Indexed intialization is valid if the rvalue's type matches the array's type.
- Literal initialization is valid if the number of values matches the length of the array, and the type of the values all match the array's type.
- Indexed array access bound checking will be a runtime error because the index may not be known at compile time. For example,
```
arr[f(4)] = arr[0];
```
