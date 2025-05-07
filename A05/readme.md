# Code Generation
This is the final phase of the rustish compiler. Here we will generate MIPS code from the Rustish source code.

## Use
To use the compiler:
```
make all
./rustish path/to/src.ri
```
This creates an executable called `a.s`, which can then be executed using MARS
```
mars a.s
```

## Compiler Features
This is a level 5 compiler which additionally supports strings. Any valid Rustish program can be compiled using this compiler, and any invalid Rustish program will produce an error message either at compile-time or runtime

### Arrays:
```
let mut arr: [i32; 5];  // creates an array of 5 integers
```
- Arrays are allocated on the heap using the MIPS malloc routine, and are freed when they go out of scope using the corresponding MIPS free routine. 
- The start address of the array holds the number of elements, and is used for out-of-bounds runtime error checking.
#### Initializing Arrays
Arrays can be initialized in the following ways:
```
arr[0] = 5; // element access
arr = [1, 2, 3, 4, 5];  // array literal
```
Array literals are allocated on the heap. When an identifier is assigned to an array literal, its pointer is simply redirected to point to the start of the literal.

### Strings:
Strings are simply arrays of characters. Characters are used as follows:
```
let mut c: char;
c = 'q';    // note the single quotes
```
Strings are implemented as character array literals:
```
let mut s: str;
s = "hello";                    // these are
s = ['h', 'e', 'l', 'l', 'o'];  // equivalent
```
