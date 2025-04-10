# Type Checking

The following should cause errors:
- [x] Assignment of wrong type to a declared variable
- [x] Using an undeclared variable (rvalue)
- [x] Assigning to an undeclared (lvalue)
- [x] Use of an uninitialized variable
- [x] Attempt to call an undefined function
- [x] Passing wrong types of actual arguments to function call
- [x] Passing wrong number of arguments to function call
- [x] Returning wrong type from function call (also returning something from a "void" function)
- [x] Missing return from function declared to return i32 or bool
- [x] Mixed arithmetic (i32/bool)
- [x] Arithmetic with bools
- [x] Logical comparison with i32s
- [x] Missing main function
- [x] Missing let, mut, etc
- [ ] Non-i32 array indices
- [x] Duplicate function names
- [x] Duplicate local variables
- [ ] Non-Boolean expression in an if or while statement
- [x] Variable declaration after executable code within a function definition
- [x] Using a "void" function as an rvalue 
- [ ] 

---
Bugs:

---
Next:
- empty return statement;
- error on line of return statement
- if statemnt
- while statement
- print statement

---

Questions:
- What do we do when we have multiple types of errors for the same line? just report them all?

## Function type checking
```
fn f(a: i32, b: bool, c: bool) -> bool {
    return true;
}

fn main() {
    f(1, 3, 5);
}
--------
Error [line 5]: Bad argument types. f expects (i32, bool, bool), but got (i32, i32, i32). 
```


## Next Steps
- Remove type nodes once they have been put in the Symbol Table
- implement TypeError as a class that inherits from std::runtime_error
