# Intro
This document outlines some common practices, including their rationales, that help in writing efficient, readable and maintainable C++ code, which are to be enforced throughout PGE.

# Semantic

## Pass parameters as reference-to-const for anything but built-in primitive types
This avoid unnecessary copies.

**Example:**
```cpp
void myFunc1(const MyClass& mc);
void myFunc2(int i);
```


## Only ever return types by value as const
This prevents assigning to temporaries.

**Important: Any type supporting [copy elision](https://en.cppreference.com/w/cpp/language/copy_elision), which includes STL containers, must *never* be returned as const, as doing so would render copy elision impossible.**

Built-in primitives already behave this way and thus adding the specifier would only introduce visual clutter.

**Example:**
```cpp
const MyClass myFunc();
// ...
myFunc() = MyClass(); // Error: This is no longer possible (which it shouldn't be!)
```


## Consider preventing heap allocation for non-polymorphic classes
Heap allocations are prone to causing issues in user code, thus disallowing them in most classes should be standard.

Deleting the delete operators isn't strictly necessary, but not doing so might imply that correct usage of it them is possible, which it generally is not.

**Note:**
Heap allocation is still possible via exotic new operator overloads, but at that point we expect the author to either know exactly what they're doing or intentionally try to write erroneous code, which in both cases we don't care about.

**Example:**
```cpp
class MyClass {
    public:
        void* operator new(size_t) = delete;
        void* operator new[](size_t) = delete;
        void operator delete(void*) = delete;
        void operator delete[](void*) = delete;
        // ...
        // Extendable via other variations of the new and delete operators.
};
```

**PGE specific:**
Simply **privately** inherit from NoHeap in `<PGE/ResourceManagement/NoHeap.h>` instead of duplicating the deletion code.

**See:**
- https://en.cppreference.com/w/cpp/memory/new/operator_new
- https://en.cppreference.com/w/cpp/memory/new/operator_delete


## Utilize perfect forwarding for template arguments
Using perfect forwarding (e.g. to a constructor) preserves r-values, which might enable copy elision.

**Example:**
```cpp
template <typename T, typename... Args>
T* addNewResource(Args&&... args) { // Note the double ampersand.
    return new T(std::forward<Args>(args)...); // Note the argument pack expansion after the forwarding.
}
```

**See:**
- https://en.cppreference.com/w/cpp/utility/forward


# Style

## Nothing but 1TBS in combination with 4 spaces for intendation
**Example:**
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

## Indent everything but the contents of an all-encompassing namespaces
**Example:**
```cpp
namespace PGE {

class MyClass { // Don't indent.
    public: // Indent.
        MyClass(); // Indent.
};

namespace MyNamespace { // Don't indent.
    void myFunc(); // Indent.
}

}
```


## Prefer postfix operations over prefix operations where the choice presents itself
We have enough trust in the compiler to optimize away the unused return value and postfix is visually superior.

**Example:**
```cpp
it++; // Yes.
++it; // No.
```


## Prefer `using` over `typedef`
They are semantically the same and the syntax of `using` is superior.

**Example:**
```cpp
using MyInt = int32_t; // Yes.
typedef int32_t MyInt; // No.
```


## Prefer `typename` over `class` for templates
`typename` poses a syntactic superset to `class` and `class` could potentially be interpreted as excluding non-class types (e.g. `struct`s or primitive types).

**Example:**
```cpp
template <typename T>
class MyClass { ... }
// ^ Yes.
// v No.
template <class T>
class MyClass { ... }
```


## Deleted methods must be public
This prevents the compiler from possibly issuing a superfluous access violation.

**Example:**
```cpp
class MyClass {
    public:
        operator=(const MyClass&) = delete; // Yes.
        
    private:
        MyClass(const MyClass&) = delete; // No.
};
```
