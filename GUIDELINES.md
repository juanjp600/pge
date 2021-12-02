# Intro
This document outlines some common practices, including their rationales, that help in writing efficient, readable and maintainable C++ code, which are to be enforced throughout PGE.

# Semantic

## Utilize references-to-const in parameters and for-each loops
This avoid unnecessary copies.

Built-in primitives are trivial to copy and limited in size, therefore they should be passed by value.

Even if your custom type is trivial to copy and limited in size the compiler might not optimize it in the same way as built-in primitives.

**Example:**
```cpp
void myFunc1(const MyClass& mc);
void myFunc2(int i);
// ...
for (const MyClass& m : myClasses) { /* ... */ }
```


## Only ever return types by value as const
This prevents assigning to/modifying temporaries.

Built-in primitives already behave this way and thus adding the specifier would only introduce visual clutter.

**Example:**
```cpp
const MyClass myFunc();
// ...
myFunc() = MyClass(); // Error: This is no longer possible (which it shouldn't be!)
```


## Prefer references over pointers
References are less flexible, but more robust than pointers, what they reference is immutable and they lack nullability. This makes them ideal candidates for member variables.

**Example:**
```cpp
class MyBadClass {
    private:
        MyOtherClass* other; // Mutable and nullable, ew!
        // (Making the pointer const would only solve one of those issues.)
        
    public:
        MyBadClass(MyOtherClass* o) {
            other = o;
        }
};

class MyGoodClass {
    private:
        MyOtherClass& other; // Immutable and not nullable, yum!
        // (What is being referenced is not necessarily immutable, which is correct.)
        
    public:
        MyBadClas(MyOtherClass& other)
        : other(other) { }
};
```

**Stub!**


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


## Prefer `std::vector::emplace_back` over `std::vector::push_back`
`emplace_back` is able to construct elements in place via perfect forwarding, where `push_back` has to copy an existing object.

`emplace_back` also copies when it needs to, thus it is advisable to always use it.


**Example:**
```cpp
std::vector<String> myVec;
myVec.push_back(String('O')); // No.
myVec.emplace_back('O'); // Yes.

String myComplicatedString;
// ... Do something complicated.
myVec.push_back(myComplicatedString);
myVec.emplace_back(myComplicatedString); // Works just as well!
```


## Prefer `std::vector::reserve` over `std::vector::resize` etc.
`reserve` does not have the costs associated with initializing objects as well as guarding against possible mismatches between the amount of reserved objects and the amount of actually added objects.

**Warning:** In the case of primitives and trivially-constructible types the added overhead of using `emplace_back` over simple assignment outweights any performance gain of using reserve by a significant margin, which is why for those types `resize` *should* be used.

**Example:**
```cpp
std::vector<int> myBadSeries(50); // This is equivalent to resize.
for (int i = 0; i < 50; i++) {
    myBadSeries[i] = i;
}

std::vector<int> myGoodSeries; myGoodSeries.reserve(50);
for (int i = 0; i < 50; i++) {
    myGoodSeries.emplace_back(i);
}
```


## Rely on C functions and functionality when appropriate
In general the C++ functions and functionality should clearly be preferred, though some exceptions present themselves.

### Prefer `memcpy` over `std::copy` for copying data primitively
Although `std::copy` has the benefit of type-safety and can be used in more cases it may not compile to the most performant option (`memcpy`) for primitive data.

### Prefer C-style casts
A C-style cast subsumes all but `dynamic_cast` (which is the obvious exception to this guideline) while having a more concise syntax and the added verbosity of C++-style casts is unnecessary.


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

## Never use the result of assignment, incrementing or decrementing
It makes your code harder to understand. Writing separate statements is always clearer.

**Example:**
```cpp
int myInt = otherInt++; // This is bad.
int myInt = otherInt; otherInt++; // This is good.

a = b = c; // This is bad.
a = c; b = c; // This is good.
```

## Define assignment, increment and decrement operators with return type void
This makes it impossible to make the mistake mentioned above with custom types.

**Example:**
```cpp
MyType& operator=(const MyType& other); // This is bad.
void operator=(const MyType& other); // This is good.
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


## Place `static_assert`-ions at the very top of their related scope
It only makes sense for them to be as close to their related template types as possible.

**Example:**
```cpp
template <typename T>
class MyClass {
    static_assert(std::is_same<T, MyClass>::value); // First line of class, before access specifiers, unindented.
    
    public:
        T t;
};

template <typename S>
const S myMethod(const S& s) const {
    static_assert(true); // First line of function.
    return s;
}
```


## Never rely on default access specifiers
**Stub.**


## Always utilize `override` while omitting `virtual` when overriding methods
`override` clearly implies `virtual`.

**Example:**
```cpp
class MyBase {
    protected:
        virtual void myFuncA() = 0;
        virtual void myFuncB() const;
        virtual void myFuncC();
};

class MyDerived {
    public:
        void myFuncA(); // No.
        void myFuncB() const override; // Yes.
        virtual void myFuncC() override; // No.
};
```


## Prefer direct over copy initialization on declaration
While they should be semantically the same in practice, direct initialization avoids repeating the type unnecessarily and avoids possibly confusing syntax.

**Example:**
```cpp
MyClass c = 42; // Bad.
MyClass b = MyClass(42); // Better, still bad.
MyClass a(42); // Good.
```


## Utilize structured bindings
Way better than using cancerous pair types directly!

**Example:**
```cpp
// Bad!
for (const auto& it : myMap) {
    KeyType key = it.first;
    ValueType val = it.second;
    // ...
}

// Good!
for (const auto& [key, value] : myMap) {
    // ...
}
```

## Consider putting enums outside of classes
Despite nested enums establishing a more obvious connection with their respective class, if an enum on its own gives enough context as to what it seeks to express it should likely not be put inside of that class. This is because nesting an enum in a class requires specifying the class when the enum is used outside of it, which can add unnecessary noise.

**Example:**
```cpp
// Bad!
class MyClass {
    public:
        enum class MyMode {
            A,
            B,
            C,
        };
        
        void setMode(MyMode m);
};

// ...

MyClass mc;
mc.setMode(MyClass::MyMode::A); // Noise!


// Good!
enum class MyMode {
    A,
    B,
    C,
};

class MyClass {
    public:
        void setMode(MyMode m);
};

// ...

MyClass mc;
mc.setMode(MyMode::A); // No noise!
```
