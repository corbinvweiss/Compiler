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
What do I do with functions that call each other? I need to save the frame pointer for each one.
At the beginning of each function:
1. Save the frame pointer from the outside function
2. Save the return address to the outside function
3. frame pointer = stack pointer
4. ... function stuff ...
5. restore frame pointer
6. restore return address

```
fn f(x: i32) -> i32 {
    return x * x;
}

fn main() {
    let mut x: i32;
    x = 5;
    print(f(x));
}
```
stack:
```
main:
- $fp
- $ra
- x
- 5
f:
- $fp       (main's frame pointer)
- $ra
- x         <- f's frame pointer

```

Start of each function call:
1. push $fp, $ra
2. set $fp = $sp

End of each function call:
1. move $sp, $fp    # clear the stack of local variables
1. lw $ra, 4($fp)
2. lw $fp, 8($fp)
3. addi $sp, $sp, 8 # clear the stack