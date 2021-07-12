# Intro
This document outlines some common practices, including their rationales, that help in writing efficient, readable and maintainable C++ code, which are to be enforced throughout PGE.

# Semantic

## Pass parameters as reference-to-const for anything but built-in primitive types
This avoid unnecessary copies.

Example:
```cpp
void myFunc1(const MyClass& mc);
void myFunc2(int i);
```


## Only ever return types by value as const
This prevents assigning to temporaries.

**Important: Any type supporting [copy elision](https://en.cppreference.com/w/cpp/language/copy_elision), which includes STL containers, must *never* be returned as const, as doing so would render copy elision impossible.**

Built-in primitives already behave this way and thus adding the specifier would only introduce visual clutter.

Example:
```cpp
const MyClass myFunc();
...
myFunc() = MyClass(); // Error: This is no longer possible (which it shouldn't be!)
```


# Style

## Nothing but 1TBS in combination with 4 spaces for intendation

Example:
```cpp
// 1TBS.
if (cond) {
    func1();
}

// This is also allowed.
if (cond) { func1(); }


// None of the following are allowed.

if (cond)
{
   func1();
}

if ( cond )
    {func1();}
    
// (And nothing inbetween or beyond.)
```

## Prefer postfix operations over prefix operations where the choice presents itself

We have enough trust in the compiler to optimize away the unused return value and postfix is visually superior.

Example:
```cpp
it++; // Yes
++it; // No
```


## Prefer `using` over `typedef`

They are semantically the same and he syntax of `using` is superior.

Example:
```cpp
using MyInt = int32_t; // Yes
typedef int32_t MyInt; // No
```
