#ifndef ASWRAPPEDCALL_H
#define ASWRAPPEDCALL_H

// Generate the wrappers by calling the macros in global scope. 
// Then register the wrapper function with the script engine using the asCALL_GENERIC 
// calling convention. The wrapper can handle both global functions and class methods.
//
// Example:
//
// asDECLARE_FUNCTION_WRAPPER(MyGenericWrapper, MyRealFunction);
// asDECLARE_FUNCTION_WRAPPERPR(MyGenericOverloadedWrapper, MyOverloadedFunction, (int), void);
// asDECLARE_METHOD_WRAPPER(MyGenericMethodWrapper, MyClass, Method);
// asDECLARE_METHOD_WRAPPERPR(MyGenericOverloadedMethodWrapper, MyClass, Method, (int) const, void);
//
// This file was generated to accept functions with a maximum of 10 parameters.
//
// **************
// HPL1R: This file was extended with global void return only variants of
//        12 and 17 param funcs for use in Penumbra.
// **************

#include <new> // placement new
#include <angelscript.h>

#define asDECLARE_FUNCTION_WRAPPER(wrapper_name,func) \
    static void wrapper_name(asIScriptGeneric *gen)\
    { \
        asCallWrappedFunc(&func,gen);\
    }

#define asDECLARE_FUNCTION_WRAPPERPR(wrapper_name,func,params,rettype) \
    static void wrapper_name(asIScriptGeneric *gen)\
    { \
        asCallWrappedFunc((rettype (*)params)(&func),gen);\
    }

#define asDECLARE_METHOD_WRAPPER(wrapper_name,cl,func) \
    static void wrapper_name(asIScriptGeneric *gen)\
    { \
        asCallWrappedFunc(&cl::func,gen);\
    }

#define asDECLARE_METHOD_WRAPPERPR(wrapper_name,cl,func,params,rettype) \
    static void wrapper_name(asIScriptGeneric *gen)\
    { \
        asCallWrappedFunc((rettype (cl::*)params)(&cl::func),gen);\
    }

// A helper class to accept reference parameters
template<typename X>
class as_wrapNative_helper
{
public:
    X d;
    as_wrapNative_helper(X d_) : d(d_) {}
};

// 0 parameter(s)

static void asWrapNative_p0_void(void (*func)(),asIScriptGeneric *)
{
    func( );
}

inline void asCallWrappedFunc(void (*func)(),asIScriptGeneric *gen)
{
    asWrapNative_p0_void(func,gen);
}

template<typename R>
static void asWrapNative_p0(R (*func)(),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ));
}

template<typename R>
inline void asCallWrappedFunc(R (*func)(),asIScriptGeneric *gen)
{
    asWrapNative_p0<R>(func,gen);
}

template<typename C>
static void asWrapNative_p0_void_this(void (C::*func)(),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( );
}

template<typename C>
inline void asCallWrappedFunc(void (C::*func)(),asIScriptGeneric *gen)
{
    asWrapNative_p0_void_this<C>(func,gen);
}

template<typename C,typename R>
static void asWrapNative_p0_this(R (C::*func)(),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ));
}

template<typename C,typename R>
inline void asCallWrappedFunc(R (C::*func)(),asIScriptGeneric *gen)
{
    asWrapNative_p0_this<C,R>(func,gen);
}

template<typename C>
static void asWrapNative_p0_void_this_const(void (C::*func)() const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( );
}

template<typename C>
inline void asCallWrappedFunc(void (C::*func)() const,asIScriptGeneric *gen)
{
    asWrapNative_p0_void_this_const<C>(func,gen);
}

template<typename C,typename R>
static void asWrapNative_p0_this_const(R (C::*func)() const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ));
}

template<typename C,typename R>
inline void asCallWrappedFunc(R (C::*func)() const,asIScriptGeneric *gen)
{
    asWrapNative_p0_this_const<C,R>(func,gen);
}

// 1 parameter(s)

template<typename T1>
static void asWrapNative_p1_void(void (*func)(T1),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d );
}

template<typename T1>
inline void asCallWrappedFunc(void (*func)(T1),asIScriptGeneric *gen)
{
    asWrapNative_p1_void<T1>(func,gen);
}

template<typename R,typename T1>
static void asWrapNative_p1(R (*func)(T1),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d ));
}

template<typename R,typename T1>
inline void asCallWrappedFunc(R (*func)(T1),asIScriptGeneric *gen)
{
    asWrapNative_p1<R,T1>(func,gen);
}

template<typename C,typename T1>
static void asWrapNative_p1_void_this(void (C::*func)(T1),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d );
}

template<typename C,typename T1>
inline void asCallWrappedFunc(void (C::*func)(T1),asIScriptGeneric *gen)
{
    asWrapNative_p1_void_this<C,T1>(func,gen);
}

template<typename C,typename R,typename T1>
static void asWrapNative_p1_this(R (C::*func)(T1),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d ));
}

template<typename C,typename R,typename T1>
inline void asCallWrappedFunc(R (C::*func)(T1),asIScriptGeneric *gen)
{
    asWrapNative_p1_this<C,R,T1>(func,gen);
}

template<typename C,typename T1>
static void asWrapNative_p1_void_this_const(void (C::*func)(T1) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d );
}

template<typename C,typename T1>
inline void asCallWrappedFunc(void (C::*func)(T1) const,asIScriptGeneric *gen)
{
    asWrapNative_p1_void_this_const<C,T1>(func,gen);
}

template<typename C,typename R,typename T1>
static void asWrapNative_p1_this_const(R (C::*func)(T1) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d ));
}

template<typename C,typename R,typename T1>
inline void asCallWrappedFunc(R (C::*func)(T1) const,asIScriptGeneric *gen)
{
    asWrapNative_p1_this_const<C,R,T1>(func,gen);
}

// 2 parameter(s)

template<typename T1,typename T2>
static void asWrapNative_p2_void(void (*func)(T1,T2),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d );
}

template<typename T1,typename T2>
inline void asCallWrappedFunc(void (*func)(T1,T2),asIScriptGeneric *gen)
{
    asWrapNative_p2_void<T1,T2>(func,gen);
}

template<typename R,typename T1,typename T2>
static void asWrapNative_p2(R (*func)(T1,T2),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d ));
}

template<typename R,typename T1,typename T2>
inline void asCallWrappedFunc(R (*func)(T1,T2),asIScriptGeneric *gen)
{
    asWrapNative_p2<R,T1,T2>(func,gen);
}

template<typename C,typename T1,typename T2>
static void asWrapNative_p2_void_this(void (C::*func)(T1,T2),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d );
}

template<typename C,typename T1,typename T2>
inline void asCallWrappedFunc(void (C::*func)(T1,T2),asIScriptGeneric *gen)
{
    asWrapNative_p2_void_this<C,T1,T2>(func,gen);
}

template<typename C,typename R,typename T1,typename T2>
static void asWrapNative_p2_this(R (C::*func)(T1,T2),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d ));
}

template<typename C,typename R,typename T1,typename T2>
inline void asCallWrappedFunc(R (C::*func)(T1,T2),asIScriptGeneric *gen)
{
    asWrapNative_p2_this<C,R,T1,T2>(func,gen);
}

template<typename C,typename T1,typename T2>
static void asWrapNative_p2_void_this_const(void (C::*func)(T1,T2) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d );
}

template<typename C,typename T1,typename T2>
inline void asCallWrappedFunc(void (C::*func)(T1,T2) const,asIScriptGeneric *gen)
{
    asWrapNative_p2_void_this_const<C,T1,T2>(func,gen);
}

template<typename C,typename R,typename T1,typename T2>
static void asWrapNative_p2_this_const(R (C::*func)(T1,T2) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d ));
}

template<typename C,typename R,typename T1,typename T2>
inline void asCallWrappedFunc(R (C::*func)(T1,T2) const,asIScriptGeneric *gen)
{
    asWrapNative_p2_this_const<C,R,T1,T2>(func,gen);
}

// 3 parameter(s)

template<typename T1,typename T2,typename T3>
static void asWrapNative_p3_void(void (*func)(T1,T2,T3),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d );
}

template<typename T1,typename T2,typename T3>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3),asIScriptGeneric *gen)
{
    asWrapNative_p3_void<T1,T2,T3>(func,gen);
}

template<typename R,typename T1,typename T2,typename T3>
static void asWrapNative_p3(R (*func)(T1,T2,T3),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d ));
}

template<typename R,typename T1,typename T2,typename T3>
inline void asCallWrappedFunc(R (*func)(T1,T2,T3),asIScriptGeneric *gen)
{
    asWrapNative_p3<R,T1,T2,T3>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3>
static void asWrapNative_p3_void_this(void (C::*func)(T1,T2,T3),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d );
}

template<typename C,typename T1,typename T2,typename T3>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3),asIScriptGeneric *gen)
{
    asWrapNative_p3_void_this<C,T1,T2,T3>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3>
static void asWrapNative_p3_this(R (C::*func)(T1,T2,T3),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3),asIScriptGeneric *gen)
{
    asWrapNative_p3_this<C,R,T1,T2,T3>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3>
static void asWrapNative_p3_void_this_const(void (C::*func)(T1,T2,T3) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d );
}

template<typename C,typename T1,typename T2,typename T3>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3) const,asIScriptGeneric *gen)
{
    asWrapNative_p3_void_this_const<C,T1,T2,T3>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3>
static void asWrapNative_p3_this_const(R (C::*func)(T1,T2,T3) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3) const,asIScriptGeneric *gen)
{
    asWrapNative_p3_this_const<C,R,T1,T2,T3>(func,gen);
}

// 4 parameter(s)

template<typename T1,typename T2,typename T3,typename T4>
static void asWrapNative_p4_void(void (*func)(T1,T2,T3,T4),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d );
}

template<typename T1,typename T2,typename T3,typename T4>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4),asIScriptGeneric *gen)
{
    asWrapNative_p4_void<T1,T2,T3,T4>(func,gen);
}

template<typename R,typename T1,typename T2,typename T3,typename T4>
static void asWrapNative_p4(R (*func)(T1,T2,T3,T4),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d ));
}

template<typename R,typename T1,typename T2,typename T3,typename T4>
inline void asCallWrappedFunc(R (*func)(T1,T2,T3,T4),asIScriptGeneric *gen)
{
    asWrapNative_p4<R,T1,T2,T3,T4>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4>
static void asWrapNative_p4_void_this(void (C::*func)(T1,T2,T3,T4),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4),asIScriptGeneric *gen)
{
    asWrapNative_p4_void_this<C,T1,T2,T3,T4>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4>
static void asWrapNative_p4_this(R (C::*func)(T1,T2,T3,T4),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4),asIScriptGeneric *gen)
{
    asWrapNative_p4_this<C,R,T1,T2,T3,T4>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4>
static void asWrapNative_p4_void_this_const(void (C::*func)(T1,T2,T3,T4) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4) const,asIScriptGeneric *gen)
{
    asWrapNative_p4_void_this_const<C,T1,T2,T3,T4>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4>
static void asWrapNative_p4_this_const(R (C::*func)(T1,T2,T3,T4) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4) const,asIScriptGeneric *gen)
{
    asWrapNative_p4_this_const<C,R,T1,T2,T3,T4>(func,gen);
}

// 5 parameter(s)

template<typename T1,typename T2,typename T3,typename T4,typename T5>
static void asWrapNative_p5_void(void (*func)(T1,T2,T3,T4,T5),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d );
}

template<typename T1,typename T2,typename T3,typename T4,typename T5>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4,T5),asIScriptGeneric *gen)
{
    asWrapNative_p5_void<T1,T2,T3,T4,T5>(func,gen);
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5>
static void asWrapNative_p5(R (*func)(T1,T2,T3,T4,T5),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d ));
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5>
inline void asCallWrappedFunc(R (*func)(T1,T2,T3,T4,T5),asIScriptGeneric *gen)
{
    asWrapNative_p5<R,T1,T2,T3,T4,T5>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5>
static void asWrapNative_p5_void_this(void (C::*func)(T1,T2,T3,T4,T5),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5),asIScriptGeneric *gen)
{
    asWrapNative_p5_void_this<C,T1,T2,T3,T4,T5>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5>
static void asWrapNative_p5_this(R (C::*func)(T1,T2,T3,T4,T5),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5),asIScriptGeneric *gen)
{
    asWrapNative_p5_this<C,R,T1,T2,T3,T4,T5>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5>
static void asWrapNative_p5_void_this_const(void (C::*func)(T1,T2,T3,T4,T5) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5) const,asIScriptGeneric *gen)
{
    asWrapNative_p5_void_this_const<C,T1,T2,T3,T4,T5>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5>
static void asWrapNative_p5_this_const(R (C::*func)(T1,T2,T3,T4,T5) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5) const,asIScriptGeneric *gen)
{
    asWrapNative_p5_this_const<C,R,T1,T2,T3,T4,T5>(func,gen);
}

// 6 parameter(s)

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
static void asWrapNative_p6_void(void (*func)(T1,T2,T3,T4,T5,T6),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d );
}

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4,T5,T6),asIScriptGeneric *gen)
{
    asWrapNative_p6_void<T1,T2,T3,T4,T5,T6>(func,gen);
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
static void asWrapNative_p6(R (*func)(T1,T2,T3,T4,T5,T6),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d ));
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
inline void asCallWrappedFunc(R (*func)(T1,T2,T3,T4,T5,T6),asIScriptGeneric *gen)
{
    asWrapNative_p6<R,T1,T2,T3,T4,T5,T6>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
static void asWrapNative_p6_void_this(void (C::*func)(T1,T2,T3,T4,T5,T6),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6),asIScriptGeneric *gen)
{
    asWrapNative_p6_void_this<C,T1,T2,T3,T4,T5,T6>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
static void asWrapNative_p6_this(R (C::*func)(T1,T2,T3,T4,T5,T6),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6),asIScriptGeneric *gen)
{
    asWrapNative_p6_this<C,R,T1,T2,T3,T4,T5,T6>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
static void asWrapNative_p6_void_this_const(void (C::*func)(T1,T2,T3,T4,T5,T6) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6) const,asIScriptGeneric *gen)
{
    asWrapNative_p6_void_this_const<C,T1,T2,T3,T4,T5,T6>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
static void asWrapNative_p6_this_const(R (C::*func)(T1,T2,T3,T4,T5,T6) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6) const,asIScriptGeneric *gen)
{
    asWrapNative_p6_this_const<C,R,T1,T2,T3,T4,T5,T6>(func,gen);
}

// 7 parameter(s)

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
static void asWrapNative_p7_void(void (*func)(T1,T2,T3,T4,T5,T6,T7),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d );
}

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4,T5,T6,T7),asIScriptGeneric *gen)
{
    asWrapNative_p7_void<T1,T2,T3,T4,T5,T6,T7>(func,gen);
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
static void asWrapNative_p7(R (*func)(T1,T2,T3,T4,T5,T6,T7),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d ));
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
inline void asCallWrappedFunc(R (*func)(T1,T2,T3,T4,T5,T6,T7),asIScriptGeneric *gen)
{
    asWrapNative_p7<R,T1,T2,T3,T4,T5,T6,T7>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
static void asWrapNative_p7_void_this(void (C::*func)(T1,T2,T3,T4,T5,T6,T7),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6,T7),asIScriptGeneric *gen)
{
    asWrapNative_p7_void_this<C,T1,T2,T3,T4,T5,T6,T7>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
static void asWrapNative_p7_this(R (C::*func)(T1,T2,T3,T4,T5,T6,T7),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6,T7),asIScriptGeneric *gen)
{
    asWrapNative_p7_this<C,R,T1,T2,T3,T4,T5,T6,T7>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
static void asWrapNative_p7_void_this_const(void (C::*func)(T1,T2,T3,T4,T5,T6,T7) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6,T7) const,asIScriptGeneric *gen)
{
    asWrapNative_p7_void_this_const<C,T1,T2,T3,T4,T5,T6,T7>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
static void asWrapNative_p7_this_const(R (C::*func)(T1,T2,T3,T4,T5,T6,T7) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6,T7) const,asIScriptGeneric *gen)
{
    asWrapNative_p7_this_const<C,R,T1,T2,T3,T4,T5,T6,T7>(func,gen);
}

// 8 parameter(s)

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
static void asWrapNative_p8_void(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d );
}

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8),asIScriptGeneric *gen)
{
    asWrapNative_p8_void<T1,T2,T3,T4,T5,T6,T7,T8>(func,gen);
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
static void asWrapNative_p8(R (*func)(T1,T2,T3,T4,T5,T6,T7,T8),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d ));
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
inline void asCallWrappedFunc(R (*func)(T1,T2,T3,T4,T5,T6,T7,T8),asIScriptGeneric *gen)
{
    asWrapNative_p8<R,T1,T2,T3,T4,T5,T6,T7,T8>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
static void asWrapNative_p8_void_this(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8),asIScriptGeneric *gen)
{
    asWrapNative_p8_void_this<C,T1,T2,T3,T4,T5,T6,T7,T8>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
static void asWrapNative_p8_this(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8),asIScriptGeneric *gen)
{
    asWrapNative_p8_this<C,R,T1,T2,T3,T4,T5,T6,T7,T8>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
static void asWrapNative_p8_void_this_const(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const,asIScriptGeneric *gen)
{
    asWrapNative_p8_void_this_const<C,T1,T2,T3,T4,T5,T6,T7,T8>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
static void asWrapNative_p8_this_const(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const,asIScriptGeneric *gen)
{
    asWrapNative_p8_this_const<C,R,T1,T2,T3,T4,T5,T6,T7,T8>(func,gen);
}

// 9 parameter(s)

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
static void asWrapNative_p9_void(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d );
}

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9),asIScriptGeneric *gen)
{
    asWrapNative_p9_void<T1,T2,T3,T4,T5,T6,T7,T8,T9>(func,gen);
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
static void asWrapNative_p9(R (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d ));
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
inline void asCallWrappedFunc(R (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9),asIScriptGeneric *gen)
{
    asWrapNative_p9<R,T1,T2,T3,T4,T5,T6,T7,T8,T9>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
static void asWrapNative_p9_void_this(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9),asIScriptGeneric *gen)
{
    asWrapNative_p9_void_this<C,T1,T2,T3,T4,T5,T6,T7,T8,T9>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
static void asWrapNative_p9_this(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9),asIScriptGeneric *gen)
{
    asWrapNative_p9_this<C,R,T1,T2,T3,T4,T5,T6,T7,T8,T9>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
static void asWrapNative_p9_void_this_const(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const,asIScriptGeneric *gen)
{
    asWrapNative_p9_void_this_const<C,T1,T2,T3,T4,T5,T6,T7,T8,T9>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
static void asWrapNative_p9_this_const(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const,asIScriptGeneric *gen)
{
    asWrapNative_p9_this_const<C,R,T1,T2,T3,T4,T5,T6,T7,T8,T9>(func,gen);
}

// 10 parameter(s)

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
static void asWrapNative_p10_void(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10),asIScriptGeneric *gen)
{
    func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d, ((as_wrapNative_helper<T10> *)gen->GetAddressOfArg(9))->d );
}

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10),asIScriptGeneric *gen)
{
    asWrapNative_p10_void<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>(func,gen);
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
static void asWrapNative_p10(R (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d, ((as_wrapNative_helper<T10> *)gen->GetAddressOfArg(9))->d ));
}

template<typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
inline void asCallWrappedFunc(R (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10),asIScriptGeneric *gen)
{
    asWrapNative_p10<R,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
static void asWrapNative_p10_void_this(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10),asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d, ((as_wrapNative_helper<T10> *)gen->GetAddressOfArg(9))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10),asIScriptGeneric *gen)
{
    asWrapNative_p10_void_this<C,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
static void asWrapNative_p10_this(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10),asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d, ((as_wrapNative_helper<T10> *)gen->GetAddressOfArg(9))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10),asIScriptGeneric *gen)
{
    asWrapNative_p10_this<C,R,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>(func,gen);
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
static void asWrapNative_p10_void_this_const(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const,asIScriptGeneric *gen)
{
    ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d, ((as_wrapNative_helper<T10> *)gen->GetAddressOfArg(9))->d );
}

template<typename C,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
inline void asCallWrappedFunc(void (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const,asIScriptGeneric *gen)
{
    asWrapNative_p10_void_this_const<C,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>(func,gen);
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
static void asWrapNative_p10_this_const(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const,asIScriptGeneric *gen)
{
    new(gen->GetAddressOfReturnLocation()) as_wrapNative_helper<R>( ((*((C*)gen->GetObject())).*func)( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d, ((as_wrapNative_helper<T10> *)gen->GetAddressOfArg(9))->d ));
}

template<typename C,typename R,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10>
inline void asCallWrappedFunc(R (C::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const,asIScriptGeneric *gen)
{
    asWrapNative_p10_this_const<C,R,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>(func,gen);
}

// 12 params global only

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10,typename T11,typename T12>
static void asWrapNative_p12_void(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12),asIScriptGeneric *gen)
{
	func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d, ((as_wrapNative_helper<T10> *)gen->GetAddressOfArg(9))->d, ((as_wrapNative_helper<T11> *)gen->GetAddressOfArg(10))->d, ((as_wrapNative_helper<T12> *)gen->GetAddressOfArg(11))->d );
}

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10,typename T11,typename T12>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12),asIScriptGeneric *gen)
{
	asWrapNative_p12_void<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>(func,gen);
}

// 17 params global only

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10,typename T11,typename T12,typename T13,typename T14,typename T15,typename T16,typename T17>
static void asWrapNative_p17_void(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17),asIScriptGeneric *gen)
{
	func( ((as_wrapNative_helper<T1> *)gen->GetAddressOfArg(0))->d, ((as_wrapNative_helper<T2> *)gen->GetAddressOfArg(1))->d, ((as_wrapNative_helper<T3> *)gen->GetAddressOfArg(2))->d, ((as_wrapNative_helper<T4> *)gen->GetAddressOfArg(3))->d, ((as_wrapNative_helper<T5> *)gen->GetAddressOfArg(4))->d, ((as_wrapNative_helper<T6> *)gen->GetAddressOfArg(5))->d, ((as_wrapNative_helper<T7> *)gen->GetAddressOfArg(6))->d, ((as_wrapNative_helper<T8> *)gen->GetAddressOfArg(7))->d, ((as_wrapNative_helper<T9> *)gen->GetAddressOfArg(8))->d, ((as_wrapNative_helper<T10> *)gen->GetAddressOfArg(9))->d, ((as_wrapNative_helper<T11> *)gen->GetAddressOfArg(10))->d, ((as_wrapNative_helper<T12> *)gen->GetAddressOfArg(11))->d, ((as_wrapNative_helper<T13> *)gen->GetAddressOfArg(12))->d, ((as_wrapNative_helper<T14> *)gen->GetAddressOfArg(13))->d, ((as_wrapNative_helper<T15> *)gen->GetAddressOfArg(14))->d, ((as_wrapNative_helper<T16> *)gen->GetAddressOfArg(15))->d, ((as_wrapNative_helper<T17> *)gen->GetAddressOfArg(15))->d );
}

template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T10,typename T11,typename T12,typename T13,typename T14,typename T15,typename T16,typename T17>
inline void asCallWrappedFunc(void (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17),asIScriptGeneric *gen)
{
	asWrapNative_p17_void<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17>(func,gen);
}

#endif // ASWRAPPEDCALL_H

