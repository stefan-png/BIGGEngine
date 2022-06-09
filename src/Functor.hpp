//      This class is pretty much a simplified version of std::function. It was made for
//      learning / experimenting purposes, and is not meant to replace std::function.


// struct Functor
// (object which can be called like a function)
//
// It has 1 void* of storage on the stack, which is either the lambda / function pointer
// If the size of the lambda is greater than the size of void*, or if the implementation's
// size of a function pointer is greater than the size of void*, the storage will instead
// point to memory in the heap with any size.

// It can safely hold:
//      a free function pointer
//      a lambda with no capture
//      a lambda which captures scalars / plain old data

// It can probably safely hold (not extensively tested)
//      a lambda which captures non-scalars / objects with a lifetime
//      a lambda which captures raw or smart pointers

// It is possible to capture a member function like this:
//      Functor foo([this](auto args){ return this->memberFunction(args); });
// but it's unsafe because the 'this' pointer may fall out of scope before the Functor is called
// which is undefined behaviour.
#pragma once

#include <type_traits>  // for all the SFINAE garbage
#include "Log.hpp"      // for debug

template<typename Ret, typename ... Args>
struct Functor {

private:
    // --------------------------- Helper templates ----------------------------------

    // checks if bool T::operator(Arg) exists.
    // by default derive from std::false_type
    template <typename T, typename = void>
    struct is_call_operator_implemented
            : std::false_type {};

    // If std::invoke_result_t<T, int> is a valid type
    template <typename T>
    struct is_call_operator_implemented<T
            , std::void_t<std::invoke_result_t<T, Args...>>>
            : std::true_type {};

    template< class T >
    static inline constexpr bool is_call_operator_implemented_v = is_call_operator_implemented<T>::value;

    typedef Ret(*funcPtrType)(Args...);
    union Storage {
        char small[sizeof(void*)];
        funcPtrType funcPtr;
    };

    template<typename Func>
    struct use_small_storage
            : std::integral_constant<bool, sizeof(Func) <= sizeof(Storage)
                                           && alignof(Func) <= alignof(Storage)
                    //&& std::is_trivially_copy_constructible<Func>::value
                    //&& std::is_trivially_destructible<Func>::value> {};
            > {};

    template< class T >
    static inline constexpr bool use_small_storage_v = use_small_storage<T>::value;

    // --------------------------- Functor ----------------------------------

private:
    char lambda[sizeof(void*)];
    funcPtrType funcPtr = nullptr;

public:
    // Null Function type
    Functor() {}

    // Free function type
    template<class Func, std::enable_if_t<std::is_function_v<std::remove_pointer_t<std::decay_t<Func>>>, bool> = true>
    Functor(Func&& func) {
        static_assert(std::is_same_v<Ret, std::invoke_result_t<Func, Args...>>, "Function pointer must return type Ret.");

        BIGG_LOG_TRACE("constructing function.\n");
        funcPtr = func; // a function will be implicitly converted to a function pointer
//        invokeFunction = [](Storage& storage, Args... args) -> Ret { return (*storage.funcPtr)(std::forward<Args>(args)...); };
        return;
    }

    // Lambda functor type
    template<class Lambda, std::enable_if_t<!std::is_function_v<std::remove_pointer_t<std::decay_t<Lambda>>>, bool> = true>
    Functor(Lambda&& lambda) {
        static_assert(is_call_operator_implemented_v<Lambda>, "Call operator is not implemented on this object! (It is not a lambda or functor).");
        static_assert(std::is_same_v<Ret, std::invoke_result_t<Lambda, Args...>>, "Lambda must return type Ret.");
        static_assert(use_small_storage_v<Lambda>, "Lambda must be at most 'sizeof(void*)' bytes in size!");

        // placement new into the stack memory
        BIGG_LOG_TRACE("constructing small lambda.\n");
        funcPtr = new (&lambda) std::decay_t<Lambda>(std::move(lambda));
//        invokeFunction = [](Storage& storage, Args... args) -> Ret { return (*reinterpret_cast<std::decay_t<Lambda>*>(&storage.small))(std::forward<Args>(args)...); };
//        destroyFunction = [](Storage& storage){ reinterpret_cast<std::decay_t<Lambda>*>(&storage.small)->~Lambda(); };
    }

    // copy constructor
    Functor(const Functor& other) : lambda(other.lambda), funcPtr(other.funcPtr) {
        if(other.funcPtr == &other.lambda) {
            // its a lambda for sure. Point this->funcPtr to this->lambda
            funcPtr = &lambda;
        }
    }

    // move constructor
    Functor(Functor&& other) : lambda(other.lambda), funcPtr(other.funcPtr) {
        if(other.funcPtr == &other.lambda) {
            // its a lambda for sure. Point this->funcPtr to this->lambda
            funcPtr = &lambda;
            other.funcPtr = funcPtr;
        } else {
            other.funcPtr = nullptr;
        }
    }

    // destructor (not allowed to be templated so I save the destruction code during construction and call it here)
//    ~Functor() {
//        destroyFunction(storage);
//    }

    Ret operator()(Args... args) {
//        return invokeFunction(storage, std::forward<Args>(args)...);
        if(funcPtr) {
            return (*funcPtr)(std::forward<Args>(args)...);
        }
        BIGG_LOG_WARN("Trying to call a non-initialized Functor.");
        return Ret();   // idk
    }
};

template<typename Event>
using EventFunctor = Functor<bool, Event>;