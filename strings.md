# Strings

I would like the following to work:

```rust
let mut a: str;
a = "hello";

print(a, "world!");

---
hello world!
```
## How:
1. Add Characters
2. Add strings as arrays of characters that are simply defined as array literals.


Lexer:
1. Add ' to the lexer
2. Add " to the lexer

Parser:

2. Add CHAR to the parser
3. Add STRING to the parser

AST:

3. Add Type::str
4. Add StringNode
5. Add StringNode::EmitCode

To represent strings in mips, I will use an array of characters. 


Upon further review I will need to add characters

# Characters
```rust
let mut c: char;
c = 'a';

```
