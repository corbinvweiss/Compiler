# Strings

I would like the following to work:

```rust
let mut a: str;
a = "hello";

print(a, "world!");

---
hello world!
```

Also you can also change strings by character

```rust
let mut a: str;
a = "hello";
a[0] = 'y';
print(a)

---
yello
```
## How:
1. Add Characters
2. Add strings as arrays of characters that are simply defined as array literals.


To represent strings in mips, I will use an array of characters. 

Upon further review I will need to add characters

# Characters
```rust
let mut c: char;
c = 'a';
```
