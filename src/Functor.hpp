// Functor (struct with a operator() overload) which can safely be constructed from a
// free function or non-capturing lambda. Can also be constructed by a member function
// or capturing lambda, although its undefined behaviour. The idea is that a member
// function pointer:
//
//              void (Object::*)(int)
//
// can also be called using:
//
//              void (*) (Object*, int)
//
// as long as its type-punned to the correct signature.

// Clang doesn't like casting between these two types but for some reason
// has no problem with casting to void(*)(Object*, int)& <--- note: reference '&'

// Using Clang 12.0.0, there is less assembly generated for -O0, compared to std::function
// in one test case. Clang inlines both Functor and std::function at -O2. GCC is much
// worse at inlining std::function, even at -O3, whereas Functor gets completely inlined.
#pragma once

#include <type_traits> // for remove_reference

// assumes signature is bool foo(const Arg& arg) { ... }
// safe with free functions and static member functions
// safe with non-capturing lambdas since compiler reduces them to free functions
// unsafe with member functions (technically UB but whatever)
// unsafe with capturing lambdas since I implemented them as member functions
template<typename Arg>
struct Functor {

    typedef bool(*freefuncptrtype)(const Arg&);
    typedef bool(*memfuncptrtype)(void*, const Arg&);

    union {
        freefuncptrtype freefunc;
        memfuncptrtype memfunc;
    };
    // Raw pointer to object yikes
    constexpr void* thisptr = nullptr;

    // free constructor
    Functor(freefuncptrtype func) : freefunc(func) {}

    // member constructor non-const
    template<typename T>
    Functor(T* thisptr, bool (T::*func)(const Arg&))
            : memfunc(reinterpret_cast<memfuncptrtype&>(func))  // stupid type pun
            , thisptr(thisptr) {}

    // member constructor const
    template<typename T>
    Functor(T* thisptr, bool (T::*func)(const Arg&) const)
            : memfunc(reinterpret_cast<memfuncptrtype&>(func))  // stupid type pun
            , thisptr(thisptr) {}

    //lambda. function is
    template<typename L>
    Functor (L&& lambda)
            : Functor(&lambda, &std::remove_reference<L>::type::operator()) {}

    bool operator()(const Arg& arg) const {
        if (thisptr == nullptr) {
            return freefunc(arg);
        } else {
            return memfunc(thisptr, arg);
        }
    }
};
