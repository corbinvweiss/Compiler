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
How do I handle labels for nested if statements?

```
if {
    if {

    }
    else {

    }
}
else {

}
```
should produce something like:
```
    beqz $s0, _else0
    ...
    beqz $s0, _else1
    ...
    j _endif1
_else1
    ...
_endif1
    j _endif0
_else0
    ...
_endif0
    ...
```

This means that I need a label tracker instance that is shared between each call to if statement node