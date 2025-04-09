# Type Checking

The following should cause errors:
- [x] Assignment of wrong type to a declared variable
- [x] Using an undeclared variable (rvalue)
- [x] Assigning to an undeclared (lvalue)
- [x] Use of an uninitialized variable
- [x] Attempt to call an undefined function
- [x] Passing wrong types of actual arguments to function call
- [x] Passing wrong number of arguments to function call
- [ ] Returning wrong type from function call (also returning something from a "void" function)
- [ ] Missing return from function declared to return i32 or bool
- [ ] Mixed arithmetic (i32/bool)
- [ ] Arithmetic with bools
- [ ] Logical comparison with i32s
- [x] Missing main function
- [x] Missing let, mut, etc
- [ ] Non-i32 array indices
- [x] Duplicate function names
- [x] Duplicate local variables
- [ ] Non-Boolean expression in an if or while statement
- [x] Variable declaration after executable code within a function definition

---
Bugs:
- not initialized error and type assignment error for same statement
- you can have two functions with the same name
- You can call an undefined function

---

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
- implement assignment from identifiers
- move UpdateSymbolTable to program level so that it has everything initialized first


## Next Steps
- Remove type nodes once they have been put in the Symbol Table
- implement TypeError as a class that inherits from std::runtime_error


## Check
1. Assignment of wrong type to a declared variable
2. Using an undeclared variable
3. Assigning to an undeclared 
4. Use of an uninitialized variable
5. Passing wrong types of actual args to function call
6. Passign wrong number of args to function call
7. Returning wrong type from function call (also returning something from a "void" function)
8. Missing return from function declared to return i32 or bool
9. Calling an undeclared function
10. Mixed arithmetic (i32/bool)
11. Airthmetic with bools
12. Logical comparison with i32s
13. Missing main function
14. Missing let, mut, etc
15. Non-i32 array indices