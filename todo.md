# Type Checking
- Throw an error when a variable that has not been initialized is referenced. Perhaps add a flag in the symbol table to keep track of whether the variable has been assigned. 
- Template the symbol table to support two value types:
    - IdentifierInfo
    - FunctionInfo
- Implement LiteralNode
    - Constructor can take either bool or int
    - implement LiteralToString(Type t, Literal l)


## Check
1. Assignment of wrong type to a declared variable
2. Using an undeclared variable
3. Assigning to an undeclared 
4. Use of an uninitialized variable
5. Passign wrong types of actual args to function call
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