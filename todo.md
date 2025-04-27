# Code Generation
Look [here](https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/codegen.html) for the assignment.

[x] print literals.
[x] local variables and expressions.
[x] loops and conditionals
[x] functions
[x] arrays

---
allow functions to call other functions that haven't been defined yet
but don't allow main to call anything that hasn't been defined.

[x] array pointer initialized to
[x] Add a length(array) to access array length
[x] Add array literals
[ ] Add array assignment from other arrays
[ ] Add array equality comparison
[x] Free arrays at the end of functions

## Someday

Return arrays from functions

## Questions:

Should array elements all be manually initialized to zero? 

in mutual_recurs.ri there is no way to typecheck the calls of the function that is defined later without a second pass over the AST.
This means that something like this will not throw an error:

```
f() {
    println(g());
}

main() {
    f();
}

```
This is kind of a big issue. I need to fix this.

