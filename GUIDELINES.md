# Intro
This document outlines some common practices, including their rationales, that help in writing efficient, readable and maintainable C++ code, which are to be enforced throughout PGE.


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
myFunc() = MyClass(); // Error: This is no longer possible (which it shouldn't be!)
```
