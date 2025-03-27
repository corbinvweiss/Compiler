# Abstract Syntax Tree

See the assignment [here](https://www.cs.southern.edu/halterman/Courses/Winter2025/415/Assignments/typecheck.html).

## Strategy
- ASTNode
    - type
        - Checks type validity (e.g. AssignmentNode checks left and right side have same type)
        - Returns type of value produced by this node (e.g. BoolNode returns a bool type, AssignmentNode with invalid type assignment will return null)
- ProgramNode
    - type: none
    - FuncDefListNode
    - MainDefNode

- Type
    - bool
    - i32
    - [bool]
    - [i32]
    - none

## Plan
Support the following:
```
fn main() {
    let mut a: i32;
    a = 5;
    a = true;   
}
------------------
error line 4: invalid assignment "bool" to "i32".
```