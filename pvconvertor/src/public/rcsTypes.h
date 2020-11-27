



#ifndef RCSTYPES_H_
#define RCSTYPES_H_

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef  __GNUC_PREREQ

#if defined __GNUC__ && defined __GNUC_MINOR__
# define __GNUC_PREREQ(maj, min) \
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define __GNUC_PREREQ(maj, min) 0
#endif

# if defined  __cplusplus ? __GNUC_PREREQ(3, 0) : __GNUC_PREREQ(3, 0)
#   define GCC3 1
# else
#   define GCC2 1
# endif

# if defined __cplusplus ? __GNUC_PREREQ (2, 6) : __GNUC_PREREQ (2, 4)
#   define __UT_FUNCTION    __PRETTY_FUNCTION__
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __UT_FUNCTION    __func__
#  else
#   define __UT_FUNCTION    ((__const char *) 0)
#  endif
# endif

#if defined __cplusplus && __GNUC_PREREQ (2,95)
# define __UT_VOID_CAST static_cast<void>
#else
# define __UT_VOID_CAST (void)
#endif


#ifdef DEBUG 
#define onAssertFail(pExpr, pFile, nLine, pFun, canContinue) \
{\
    ::fprintf(stderr, "\nAssertion failed at "\
              pFile \
              ":%d (%s)\n"\
              "\n*****  Utassert(%s);\n\n",\
              nLine, pFun, pExpr);\
}

#define Utassert(ex)                                                \
{                                                                   \
    if (!(ex))                                                      \
    {                                                               \
        onAssertFail(#ex, __FILE__, __LINE__, __UT_FUNCTION, false);\
    }                                                               \
}                                                                   \
(__UT_VOID_CAST(0))
#else 
#define Utassert(ex)  (__UT_VOID_CAST(0))
#endif 

template<bool>
struct __CompileTimeError
{
};

template<>
struct __CompileTimeError<true>
{
    __CompileTimeError(...) { }
};

#define UtCompileAssert(expr, msg) \
          { \
            class __ERROR_##msg##_ { }; \
            __ERROR_##msg##_  err;\
            __CompileTimeError<!!(expr)> a(err); \
          }


#if __BYTE_ORDER == __LITTLE_ENDIAN
#define HV_LITTLE_ENDIAN 1
#elif __BYTE_ORDER == __BIG_ENDIAN
#define HV_BIG_ENDIAN 1
#elif __BYTE_ORDER == __PDP_ENDIAN
#define HV_PDP_ENDIAN 1
#endif

typedef __int8_t            hvInt8;
typedef __int16_t           hvInt16;
typedef __int32_t           hvInt32;
typedef __int64_t           hvInt64;

typedef __uint8_t           hvUInt8;
typedef __uint16_t          hvUInt16;
typedef __uint32_t          hvUInt32;
typedef __uint64_t          hvUInt64;

typedef intptr_t            hvIntp;
typedef uintptr_t           hvUIntp;

typedef hvInt32             hvCoord;
typedef double              hvRealArea;
typedef double              hvLength;

#endif 
