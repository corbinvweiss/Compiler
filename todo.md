# Code Generation
Look [here](https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/codegen.html) for the assignment.

[x] print literals.
[x] local variables and expressions.
[ ] loops and conditionals
[ ] functions
[ ] arrays

## Someday
make TypeCheck() return a boolean value so that it stops the process when it encounters a fatal error, and does not try to generate MIPS code for a program that won't work.

## Questions:
How do I do arrays?
### 1. Allocating arrays
```
let mut a: [i32; 5];
```
allocates space for 5 i32's on the heap.
The heap looks like this then:
```
5 0 0 0 0 0 
- - - - - -
^
a points here
```
### 2. Accessing arrays
```
a[0] = 7;
5 7 0 0 0 0
- - - - - - 
