

#ifndef _TCL
#define _TCL



#ifdef __cplusplus
extern "C" {
#endif



#define TCL_ALPHA_RELEASE	0
#define TCL_BETA_RELEASE	1
#define TCL_FINAL_RELEASE	2



#define TCL_MAJOR_VERSION   8
#define TCL_MINOR_VERSION   5
#define TCL_RELEASE_LEVEL   TCL_FINAL_RELEASE
#define TCL_RELEASE_SERIAL  19

#define TCL_VERSION	    "8.5"
#define TCL_PATCH_LEVEL	    "8.5.19"



#ifndef __WIN32__
#   if defined(_WIN32) || defined(WIN32) || defined(__MSVCRT__) || defined(__BORLANDC__) || (defined(__WATCOMC__) && defined(__WINDOWS_386__))
#	define __WIN32__
#	ifndef WIN32
#	    define WIN32
#	endif
#	ifndef _WIN32
#	    define _WIN32
#	endif
#   endif
#endif



#ifdef __WIN32__
#   ifndef STRICT
#	define STRICT
#   endif
#endif 



#ifndef STRINGIFY
#  define STRINGIFY(x) STRINGIFY1(x)
#  define STRINGIFY1(x) #x
#endif
#ifndef JOIN
#  define JOIN(a,b) JOIN1(a,b)
#  define JOIN1(a,b) a##b
#endif



#ifndef RC_INVOKED



#ifdef TCL_THREADS
#define TCL_DECLARE_MUTEX(name) static Tcl_Mutex name;
#else
#define TCL_DECLARE_MUTEX(name)
#endif



#include <stdio.h>



#include <stdarg.h>
#ifndef TCL_NO_DEPRECATED
#    define TCL_VARARGS(type, name) (type name, ...)
#    define TCL_VARARGS_DEF(type, name) (type name, ...)
#    define TCL_VARARGS_START(type, name, list) (va_start(list, name), name)
#endif



#if (defined(__WIN32__) && (defined(_MSC_VER) || (defined(__BORLANDC__) && (__BORLANDC__ >= 0x0550)) || defined(__LCC__) || defined(__WATCOMC__) || (defined(__GNUC__) && defined(__declspec))))
#   define HAVE_DECLSPEC 1
#   ifdef STATIC_BUILD
#       define DLLIMPORT
#       define DLLEXPORT
#       ifdef _DLL
#           define CRTIMPORT __declspec(dllimport)
#       else
#           define CRTIMPORT
#       endif
#   else
#       define DLLIMPORT __declspec(dllimport)
#       define DLLEXPORT __declspec(dllexport)
#       define CRTIMPORT __declspec(dllimport)
#   endif
#else
#   define DLLIMPORT
#   if defined(__GNUC__) && __GNUC__ > 3
#       define DLLEXPORT __attribute__ ((visibility("default")))
#   else
#       define DLLEXPORT
#   endif
#   define CRTIMPORT
#endif



#undef TCL_STORAGE_CLASS
#ifdef BUILD_tcl
#   define TCL_STORAGE_CLASS DLLEXPORT
#else
#   ifdef USE_TCL_STUBS
#      define TCL_STORAGE_CLASS
#   else
#      define TCL_STORAGE_CLASS DLLIMPORT
#   endif
#endif



#undef _ANSI_ARGS_
#undef CONST
#ifndef INLINE
#   define INLINE
#endif

#ifndef NO_CONST
#   define CONST const
#else
#   define CONST
#endif

#ifndef NO_PROTOTYPES
#   define _ANSI_ARGS_(x)	x
#else
#   define _ANSI_ARGS_(x)	()
#endif

#ifdef USE_NON_CONST
#   ifdef USE_COMPAT_CONST
#      error define at most one of USE_NON_CONST and USE_COMPAT_CONST
#   endif
#   define CONST84
#   define CONST84_RETURN
#else
#   ifdef USE_COMPAT_CONST
#      define CONST84
#      define CONST84_RETURN CONST
#   else
#      define CONST84 CONST
#      define CONST84_RETURN CONST
#   endif
#endif



#ifdef EXTERN
#   undef EXTERN
#endif 

#ifdef __cplusplus
#   define EXTERN extern "C" TCL_STORAGE_CLASS
#else
#   define EXTERN extern TCL_STORAGE_CLASS
#endif



#if defined(__WIN32__) && !defined(HAVE_WINNT_IGNORE_VOID)
#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif
#endif 



#ifndef __VXWORKS__
#   ifndef NO_VOID
#	define VOID void
#   else
#	define VOID char
#   endif
#endif



#ifndef _CLIENTDATA
#   ifndef NO_VOID
	typedef void *ClientData;
#   else
	typedef int *ClientData;
#   endif
#   define _CLIENTDATA
#endif



#ifdef __APPLE__
#   ifdef __LP64__
#	undef TCL_WIDE_INT_TYPE
#	define TCL_WIDE_INT_IS_LONG 1
#	define TCL_CFG_DO64BIT 1
#    else 
#	define TCL_WIDE_INT_TYPE long long
#	undef TCL_WIDE_INT_IS_LONG
#	undef TCL_CFG_DO64BIT
#    endif 
#    undef HAVE_STRUCT_STAT64
#endif 



#if !defined(TCL_WIDE_INT_TYPE)&&!defined(TCL_WIDE_INT_IS_LONG)
#   if defined(__WIN32__)
#      define TCL_WIDE_INT_TYPE __int64
#      ifdef __BORLANDC__
#         define TCL_LL_MODIFIER	"L"
#      else 
#         define TCL_LL_MODIFIER	"I64"
#      endif 
#   elif defined(__GNUC__)
#      define TCL_WIDE_INT_TYPE long long
#      define TCL_LL_MODIFIER	"ll"
#   else 

#      ifdef NO_LIMITS_H
#	  error please define either TCL_WIDE_INT_TYPE or TCL_WIDE_INT_IS_LONG
#      else 
#	  include <limits.h>
#	  if (INT_MAX < LONG_MAX)
#	     define TCL_WIDE_INT_IS_LONG	1
#	  else
#	     define TCL_WIDE_INT_TYPE long long
#         endif
#      endif 
#   endif 
#endif 
#ifdef TCL_WIDE_INT_IS_LONG
#   undef TCL_WIDE_INT_TYPE
#   define TCL_WIDE_INT_TYPE	long
#endif 

typedef TCL_WIDE_INT_TYPE		Tcl_WideInt;
typedef unsigned TCL_WIDE_INT_TYPE	Tcl_WideUInt;

#ifdef TCL_WIDE_INT_IS_LONG
#   define Tcl_WideAsLong(val)		((long)(val))
#   define Tcl_LongAsWide(val)		((long)(val))
#   define Tcl_WideAsDouble(val)	((double)((long)(val)))
#   define Tcl_DoubleAsWide(val)	((long)((double)(val)))
#   ifndef TCL_LL_MODIFIER
#      define TCL_LL_MODIFIER		"l"
#   endif 
#else 

#   ifndef TCL_LL_MODIFIER
#      define TCL_LL_MODIFIER		"ll"
#   endif 
#   define Tcl_WideAsLong(val)		((long)((Tcl_WideInt)(val)))
#   define Tcl_LongAsWide(val)		((Tcl_WideInt)((long)(val)))
#   define Tcl_WideAsDouble(val)	((double)((Tcl_WideInt)(val)))
#   define Tcl_DoubleAsWide(val)	((Tcl_WideInt)((double)(val)))
#endif 

#if defined(__WIN32__)
#   ifdef __BORLANDC__
	typedef struct stati64 Tcl_StatBuf;
#   elif defined(_WIN64)
	typedef struct __stat64 Tcl_StatBuf;
#   elif (defined(_MSC_VER) && (_MSC_VER < 1400)) || defined(_USE_32BIT_TIME_T)
	typedef struct _stati64	Tcl_StatBuf;
#   else
	typedef struct _stat32i64 Tcl_StatBuf;
#   endif 
#elif defined(__CYGWIN__)
    typedef struct {
	dev_t st_dev;
	unsigned short st_ino;
	unsigned short st_mode;
	short st_nlink;
	short st_uid;
	short st_gid;
	
	dev_t st_rdev;
	
	long long st_size;
	struct {long tv_sec;} st_atim;
	struct {long tv_sec;} st_mtim;
	struct {long tv_sec;} st_ctim;
	
    } Tcl_StatBuf;
#elif defined(HAVE_STRUCT_STAT64) && !defined(__APPLE__)
    typedef struct stat64 Tcl_StatBuf;
#else
    typedef struct stat Tcl_StatBuf;
#endif



typedef struct Tcl_Interp {
    char *result;		
    void (*freeProc) _ANSI_ARGS_((char *blockPtr));
				
    int errorLine;		
} Tcl_Interp;

typedef struct Tcl_AsyncHandler_ *Tcl_AsyncHandler;
typedef struct Tcl_Channel_ *Tcl_Channel;
typedef struct Tcl_ChannelTypeVersion_ *Tcl_ChannelTypeVersion;
typedef struct Tcl_Command_ *Tcl_Command;
typedef struct Tcl_Condition_ *Tcl_Condition;
typedef struct Tcl_Dict_ *Tcl_Dict;
typedef struct Tcl_EncodingState_ *Tcl_EncodingState;
typedef struct Tcl_Encoding_ *Tcl_Encoding;
typedef struct Tcl_Event Tcl_Event;
typedef struct Tcl_InterpState_ *Tcl_InterpState;
typedef struct Tcl_LoadHandle_ *Tcl_LoadHandle;
typedef struct Tcl_Mutex_ *Tcl_Mutex;
typedef struct Tcl_Pid_ *Tcl_Pid;
typedef struct Tcl_RegExp_ *Tcl_RegExp;
typedef struct Tcl_ThreadDataKey_ *Tcl_ThreadDataKey;
typedef struct Tcl_ThreadId_ *Tcl_ThreadId;
typedef struct Tcl_TimerToken_ *Tcl_TimerToken;
typedef struct Tcl_Trace_ *Tcl_Trace;
typedef struct Tcl_Var_ *Tcl_Var;



#if defined __WIN32__
typedef unsigned (__stdcall Tcl_ThreadCreateProc) _ANSI_ARGS_((ClientData clientData));
#else
typedef void (Tcl_ThreadCreateProc) _ANSI_ARGS_((ClientData clientData));
#endif



#if defined __WIN32__
#   define Tcl_ThreadCreateType		unsigned __stdcall
#   define TCL_THREAD_CREATE_RETURN	return 0
#else
#   define Tcl_ThreadCreateType		void
#   define TCL_THREAD_CREATE_RETURN
#endif



#define TCL_THREAD_STACK_DEFAULT (0)    
#define TCL_THREAD_NOFLAGS	 (0000) 
#define TCL_THREAD_JOINABLE	 (0001) 



#define TCL_MATCH_NOCASE	(1<<0)



#define	TCL_REG_BASIC		000000	
#define	TCL_REG_EXTENDED	000001	
#define	TCL_REG_ADVF		000002	
#define	TCL_REG_ADVANCED	000003	
#define	TCL_REG_QUOTE		000004	
#define	TCL_REG_NOCASE		000010	
#define	TCL_REG_NOSUB		000020	
#define	TCL_REG_EXPANDED	000040	
#define	TCL_REG_NLSTOP		000100  
#define	TCL_REG_NLANCH		000200  
#define	TCL_REG_NEWLINE		000300  
#define	TCL_REG_CANMATCH	001000  



#define	TCL_REG_NOTBOL	0001	
#define	TCL_REG_NOTEOL	0002	



typedef struct Tcl_RegExpIndices {
    long start;			
    long end;			
} Tcl_RegExpIndices;

typedef struct Tcl_RegExpInfo {
    int nsubs;			
    Tcl_RegExpIndices *matches;	
    long extendStart;		
    long reserved;		
} Tcl_RegExpInfo;



typedef Tcl_StatBuf *Tcl_Stat_;
typedef struct stat *Tcl_OldStat_;



#define TCL_OK			0
#define TCL_ERROR		1
#define TCL_RETURN		2
#define TCL_BREAK		3
#define TCL_CONTINUE		4

#define TCL_RESULT_SIZE		200



#define TCL_SUBST_COMMANDS	001
#define TCL_SUBST_VARIABLES	002
#define TCL_SUBST_BACKSLASHES	004
#define TCL_SUBST_ALL		007



typedef enum {
    TCL_INT, TCL_DOUBLE, TCL_EITHER, TCL_WIDE_INT
} Tcl_ValueType;

typedef struct Tcl_Value {
    Tcl_ValueType type;		
    long intValue;		
    double doubleValue;		
    Tcl_WideInt wideValue;	
} Tcl_Value;



struct Tcl_Obj;



typedef int (Tcl_AppInitProc) _ANSI_ARGS_((Tcl_Interp *interp));
typedef int (Tcl_AsyncProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int code));
typedef void (Tcl_ChannelProc) _ANSI_ARGS_((ClientData clientData, int mask));
typedef void (Tcl_CloseProc) _ANSI_ARGS_((ClientData data));
typedef void (Tcl_CmdDeleteProc) _ANSI_ARGS_((ClientData clientData));
typedef int (Tcl_CmdProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, CONST84 char *argv[]));
typedef void (Tcl_CmdTraceProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int level, char *command, Tcl_CmdProc *proc,
	ClientData cmdClientData, int argc, CONST84 char *argv[]));
typedef int (Tcl_CmdObjTraceProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int level, CONST char *command,
	Tcl_Command commandInfo, int objc, struct Tcl_Obj * CONST * objv));
typedef void (Tcl_CmdObjTraceDeleteProc) _ANSI_ARGS_((ClientData clientData));
typedef void (Tcl_DupInternalRepProc) _ANSI_ARGS_((struct Tcl_Obj *srcPtr,
	struct Tcl_Obj *dupPtr));
typedef int (Tcl_EncodingConvertProc)_ANSI_ARGS_((ClientData clientData,
	CONST char *src, int srcLen, int flags, Tcl_EncodingState *statePtr,
	char *dst, int dstLen, int *srcReadPtr, int *dstWrotePtr,
	int *dstCharsPtr));
typedef void (Tcl_EncodingFreeProc)_ANSI_ARGS_((ClientData clientData));
typedef int (Tcl_EventProc) _ANSI_ARGS_((Tcl_Event *evPtr, int flags));
typedef void (Tcl_EventCheckProc) _ANSI_ARGS_((ClientData clientData,
	int flags));
typedef int (Tcl_EventDeleteProc) _ANSI_ARGS_((Tcl_Event *evPtr,
	ClientData clientData));
typedef void (Tcl_EventSetupProc) _ANSI_ARGS_((ClientData clientData,
	int flags));
typedef void (Tcl_ExitProc) _ANSI_ARGS_((ClientData clientData));
typedef void (Tcl_FileProc) _ANSI_ARGS_((ClientData clientData, int mask));
typedef void (Tcl_FileFreeProc) _ANSI_ARGS_((ClientData clientData));
typedef void (Tcl_FreeInternalRepProc) _ANSI_ARGS_((struct Tcl_Obj *objPtr));
typedef void (Tcl_FreeProc) _ANSI_ARGS_((char *blockPtr));
typedef void (Tcl_IdleProc) _ANSI_ARGS_((ClientData clientData));
typedef void (Tcl_InterpDeleteProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp));
typedef int (Tcl_MathProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, Tcl_Value *args, Tcl_Value *resultPtr));
typedef void (Tcl_NamespaceDeleteProc) _ANSI_ARGS_((ClientData clientData));
typedef int (Tcl_ObjCmdProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int objc, struct Tcl_Obj * CONST * objv));
typedef int (Tcl_PackageInitProc) _ANSI_ARGS_((Tcl_Interp *interp));
typedef int (Tcl_PackageUnloadProc) _ANSI_ARGS_((Tcl_Interp *interp,
	int flags));
typedef void (Tcl_PanicProc) _ANSI_ARGS_((CONST char *format, ...));
typedef void (Tcl_TcpAcceptProc) _ANSI_ARGS_((ClientData callbackData,
	Tcl_Channel chan, char *address, int port));
typedef void (Tcl_TimerProc) _ANSI_ARGS_((ClientData clientData));
typedef int (Tcl_SetFromAnyProc) _ANSI_ARGS_((Tcl_Interp *interp,
	struct Tcl_Obj *objPtr));
typedef void (Tcl_UpdateStringProc) _ANSI_ARGS_((struct Tcl_Obj *objPtr));
typedef char *(Tcl_VarTraceProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, CONST84 char *part1, CONST84 char *part2,
	int flags));
typedef void (Tcl_CommandTraceProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, CONST char *oldName, CONST char *newName,
	int flags));
typedef void (Tcl_CreateFileHandlerProc) _ANSI_ARGS_((int fd, int mask,
	Tcl_FileProc *proc, ClientData clientData));
typedef void (Tcl_DeleteFileHandlerProc) _ANSI_ARGS_((int fd));
typedef void (Tcl_AlertNotifierProc) _ANSI_ARGS_((ClientData clientData));
typedef void (Tcl_ServiceModeHookProc) _ANSI_ARGS_((int mode));
typedef ClientData (Tcl_InitNotifierProc) _ANSI_ARGS_((VOID));
typedef void (Tcl_FinalizeNotifierProc) _ANSI_ARGS_((ClientData clientData));
typedef void (Tcl_MainLoopProc) _ANSI_ARGS_((void));



typedef struct Tcl_ObjType {
    char *name;			
    Tcl_FreeInternalRepProc *freeIntRepProc;
				
    Tcl_DupInternalRepProc *dupIntRepProc;
				
    Tcl_UpdateStringProc *updateStringProc;
				
    Tcl_SetFromAnyProc *setFromAnyProc;
				
} Tcl_ObjType;



typedef struct Tcl_Obj {
    int refCount;		
    char *bytes;		
    int length;			
    Tcl_ObjType *typePtr;	
    union {			
	long longValue;		
	double doubleValue;	
	VOID *otherValuePtr;	
	Tcl_WideInt wideValue;	
	struct {		
	    VOID *ptr1;
	    VOID *ptr2;
	} twoPtrValue;
	struct {		
	    VOID *ptr;		
	    unsigned long value;
	} ptrAndLongRep;
    } internalRep;
} Tcl_Obj;



void		Tcl_IncrRefCount _ANSI_ARGS_((Tcl_Obj *objPtr));
void		Tcl_DecrRefCount _ANSI_ARGS_((Tcl_Obj *objPtr));
int		Tcl_IsShared _ANSI_ARGS_((Tcl_Obj *objPtr));



typedef struct Tcl_SavedResult {
    char *result;
    Tcl_FreeProc *freeProc;
    Tcl_Obj *objResultPtr;
    char *appendResult;
    int appendAvl;
    int appendUsed;
    char resultSpace[TCL_RESULT_SIZE+1];
} Tcl_SavedResult;



typedef struct Tcl_Namespace {
    char *name;			
    char *fullName;		
    ClientData clientData;	
    Tcl_NamespaceDeleteProc *deleteProc;
				
    struct Tcl_Namespace *parentPtr;
				
} Tcl_Namespace;



typedef struct Tcl_CallFrame {
    Tcl_Namespace *nsPtr;
    int dummy1;
    int dummy2;
    VOID *dummy3;
    VOID *dummy4;
    VOID *dummy5;
    int dummy6;
    VOID *dummy7;
    VOID *dummy8;
    int dummy9;
    VOID *dummy10;
    VOID *dummy11;
    VOID *dummy12;
    VOID *dummy13;
} Tcl_CallFrame;



typedef struct Tcl_CmdInfo {
    int isNativeObjectProc;	
    Tcl_ObjCmdProc *objProc;	
    ClientData objClientData;	
    Tcl_CmdProc *proc;		
    ClientData clientData;	
    Tcl_CmdDeleteProc *deleteProc;
				
    ClientData deleteData;	
    Tcl_Namespace *namespacePtr;
} Tcl_CmdInfo;



#define TCL_DSTRING_STATIC_SIZE 200
typedef struct Tcl_DString {
    char *string;		
    int length;			
    int spaceAvl;		
    char staticSpace[TCL_DSTRING_STATIC_SIZE];
				
} Tcl_DString;

#define Tcl_DStringLength(dsPtr) ((dsPtr)->length)
#define Tcl_DStringValue(dsPtr) ((dsPtr)->string)
#define Tcl_DStringTrunc Tcl_DStringSetLength



#define TCL_MAX_PREC		17
#define TCL_DOUBLE_SPACE	(TCL_MAX_PREC+10)



#define TCL_INTEGER_SPACE	24



#define TCL_DONT_USE_BRACES	1
#define TCL_DONT_QUOTE_HASH	8



#define TCL_EXACT	1


#define TCL_NO_EVAL		0x10000
#define TCL_EVAL_GLOBAL		0x20000
#define TCL_EVAL_DIRECT		0x40000
#define TCL_EVAL_INVOKE		0x80000



#define TCL_VOLATILE		((Tcl_FreeProc *) 1)
#define TCL_STATIC		((Tcl_FreeProc *) 0)
#define TCL_DYNAMIC		((Tcl_FreeProc *) 3)



#define TCL_GLOBAL_ONLY		 1
#define TCL_NAMESPACE_ONLY	 2
#define TCL_APPEND_VALUE	 4
#define TCL_LIST_ELEMENT	 8
#define TCL_TRACE_READS		 0x10
#define TCL_TRACE_WRITES	 0x20
#define TCL_TRACE_UNSETS	 0x40
#define TCL_TRACE_DESTROYED	 0x80
#define TCL_INTERP_DESTROYED	 0x100
#define TCL_LEAVE_ERR_MSG	 0x200
#define TCL_TRACE_ARRAY		 0x800
#ifndef TCL_REMOVE_OBSOLETE_TRACES

#define TCL_TRACE_OLD_STYLE	 0x1000
#endif

#define TCL_TRACE_RESULT_DYNAMIC 0x8000
#define TCL_TRACE_RESULT_OBJECT  0x10000



#define TCL_ENSEMBLE_PREFIX 0x02



#define TCL_TRACE_RENAME 0x2000
#define TCL_TRACE_DELETE 0x4000

#define TCL_ALLOW_INLINE_COMPILATION 0x20000



#ifndef TCL_NO_DEPRECATED
#   define TCL_PARSE_PART1	0x400
#endif



#define TCL_LINK_INT		1
#define TCL_LINK_DOUBLE		2
#define TCL_LINK_BOOLEAN	3
#define TCL_LINK_STRING		4
#define TCL_LINK_WIDE_INT	5
#define TCL_LINK_CHAR		6
#define TCL_LINK_UCHAR		7
#define TCL_LINK_SHORT		8
#define TCL_LINK_USHORT		9
#define TCL_LINK_UINT		10
#define TCL_LINK_LONG		11
#define TCL_LINK_ULONG		12
#define TCL_LINK_FLOAT		13
#define TCL_LINK_WIDE_UINT	14
#define TCL_LINK_READ_ONLY	0x80



typedef struct Tcl_HashKeyType Tcl_HashKeyType;
typedef struct Tcl_HashTable Tcl_HashTable;
typedef struct Tcl_HashEntry Tcl_HashEntry;

typedef unsigned int (Tcl_HashKeyProc) _ANSI_ARGS_((Tcl_HashTable *tablePtr,
	VOID *keyPtr));
typedef int (Tcl_CompareHashKeysProc) _ANSI_ARGS_((VOID *keyPtr,
	Tcl_HashEntry *hPtr));
typedef Tcl_HashEntry *(Tcl_AllocHashEntryProc) _ANSI_ARGS_((
	Tcl_HashTable *tablePtr, VOID *keyPtr));
typedef void (Tcl_FreeHashEntryProc) _ANSI_ARGS_((Tcl_HashEntry *hPtr));



#ifndef TCL_HASH_KEY_STORE_HASH
#   define TCL_HASH_KEY_STORE_HASH 1
#endif



struct Tcl_HashEntry {
    Tcl_HashEntry *nextPtr;	
    Tcl_HashTable *tablePtr;	
#if TCL_HASH_KEY_STORE_HASH
    VOID *hash;			
#else
    Tcl_HashEntry **bucketPtr;	
#endif
    ClientData clientData;	
    union {			
	char *oneWordValue;	
	Tcl_Obj *objPtr;	
	int words[1];		
	char string[4];		
    } key;			
};



#define TCL_HASH_KEY_RANDOMIZE_HASH 0x1
#define TCL_HASH_KEY_SYSTEM_HASH    0x2



#define TCL_HASH_KEY_TYPE_VERSION 1
struct Tcl_HashKeyType {
    int version;		
    int flags;			
    Tcl_HashKeyProc *hashKeyProc;
				
    Tcl_CompareHashKeysProc *compareKeysProc;
				
    Tcl_AllocHashEntryProc *allocEntryProc;
				
    Tcl_FreeHashEntryProc *freeEntryProc;
				
};



#define TCL_SMALL_HASH_TABLE 4
struct Tcl_HashTable {
    Tcl_HashEntry **buckets;	
    Tcl_HashEntry *staticBuckets[TCL_SMALL_HASH_TABLE];
				
    int numBuckets;		
    int numEntries;		
    int rebuildSize;		
    int downShift;		
    int mask;			
    int keyType;		
    Tcl_HashEntry *(*findProc) _ANSI_ARGS_((Tcl_HashTable *tablePtr,
	    CONST char *key));
    Tcl_HashEntry *(*createProc) _ANSI_ARGS_((Tcl_HashTable *tablePtr,
	    CONST char *key, int *newPtr));
    Tcl_HashKeyType *typePtr;	
};



typedef struct Tcl_HashSearch {
    Tcl_HashTable *tablePtr;	
    int nextIndex;		
    Tcl_HashEntry *nextEntryPtr;
} Tcl_HashSearch;



#define TCL_STRING_KEYS		0
#define TCL_ONE_WORD_KEYS	1
#define TCL_CUSTOM_TYPE_KEYS	-2
#define TCL_CUSTOM_PTR_KEYS	-1



typedef struct {
    void *next;			
    int epoch;			
    Tcl_Dict dictionaryPtr;	
} Tcl_DictSearch;



#define TCL_DONT_WAIT		(1<<1)
#define TCL_WINDOW_EVENTS	(1<<2)
#define TCL_FILE_EVENTS		(1<<3)
#define TCL_TIMER_EVENTS	(1<<4)
#define TCL_IDLE_EVENTS		(1<<5)	
#define TCL_ALL_EVENTS		(~TCL_DONT_WAIT)



struct Tcl_Event {
    Tcl_EventProc *proc;	
    struct Tcl_Event *nextPtr;	
};



typedef enum {
    TCL_QUEUE_TAIL, TCL_QUEUE_HEAD, TCL_QUEUE_MARK
} Tcl_QueuePosition;



#define TCL_SERVICE_NONE 0
#define TCL_SERVICE_ALL 1



typedef struct Tcl_Time {
    long sec;			
    long usec;			
} Tcl_Time;

typedef void (Tcl_SetTimerProc) _ANSI_ARGS_((Tcl_Time *timePtr));
typedef int (Tcl_WaitForEventProc) _ANSI_ARGS_((Tcl_Time *timePtr));



typedef void (Tcl_GetTimeProc)   _ANSI_ARGS_((Tcl_Time *timebuf,
	ClientData clientData));
typedef void (Tcl_ScaleTimeProc) _ANSI_ARGS_((Tcl_Time *timebuf,
	ClientData clientData));



#define TCL_READABLE		(1<<1)
#define TCL_WRITABLE		(1<<2)
#define TCL_EXCEPTION		(1<<3)



#define TCL_STDIN		(1<<1)
#define TCL_STDOUT		(1<<2)
#define TCL_STDERR		(1<<3)
#define TCL_ENFORCE_MODE	(1<<4)



#define TCL_CLOSE_READ		(1<<1)
#define TCL_CLOSE_WRITE		(1<<2)



#define TCL_CLOSE2PROC		((Tcl_DriverCloseProc *) 1)



#define TCL_CHANNEL_VERSION_1	((Tcl_ChannelTypeVersion) 0x1)
#define TCL_CHANNEL_VERSION_2	((Tcl_ChannelTypeVersion) 0x2)
#define TCL_CHANNEL_VERSION_3	((Tcl_ChannelTypeVersion) 0x3)
#define TCL_CHANNEL_VERSION_4	((Tcl_ChannelTypeVersion) 0x4)
#define TCL_CHANNEL_VERSION_5	((Tcl_ChannelTypeVersion) 0x5)



#define TCL_CHANNEL_THREAD_INSERT (0)
#define TCL_CHANNEL_THREAD_REMOVE (1)



typedef int	(Tcl_DriverBlockModeProc) _ANSI_ARGS_((
		    ClientData instanceData, int mode));
typedef int	(Tcl_DriverCloseProc) _ANSI_ARGS_((ClientData instanceData,
		    Tcl_Interp *interp));
typedef int	(Tcl_DriverClose2Proc) _ANSI_ARGS_((ClientData instanceData,
		    Tcl_Interp *interp, int flags));
typedef int	(Tcl_DriverInputProc) _ANSI_ARGS_((ClientData instanceData,
		    char *buf, int toRead, int *errorCodePtr));
typedef int	(Tcl_DriverOutputProc) _ANSI_ARGS_((ClientData instanceData,
		    CONST84 char *buf, int toWrite, int *errorCodePtr));
typedef int	(Tcl_DriverSeekProc) _ANSI_ARGS_((ClientData instanceData,
		    long offset, int mode, int *errorCodePtr));
typedef int	(Tcl_DriverSetOptionProc) _ANSI_ARGS_((
		    ClientData instanceData, Tcl_Interp *interp,
		    CONST char *optionName, CONST char *value));
typedef int	(Tcl_DriverGetOptionProc) _ANSI_ARGS_((
		    ClientData instanceData, Tcl_Interp *interp,
		    CONST84 char *optionName, Tcl_DString *dsPtr));
typedef void	(Tcl_DriverWatchProc) _ANSI_ARGS_((
		    ClientData instanceData, int mask));
typedef int	(Tcl_DriverGetHandleProc) _ANSI_ARGS_((
		    ClientData instanceData, int direction,
		    ClientData *handlePtr));
typedef int	(Tcl_DriverFlushProc) _ANSI_ARGS_((ClientData instanceData));
typedef int	(Tcl_DriverHandlerProc) _ANSI_ARGS_((
		    ClientData instanceData, int interestMask));
typedef Tcl_WideInt (Tcl_DriverWideSeekProc) _ANSI_ARGS_((
		    ClientData instanceData, Tcl_WideInt offset,
		    int mode, int *errorCodePtr));

typedef void	(Tcl_DriverThreadActionProc) _ANSI_ARGS_ ((
		    ClientData instanceData, int action));

typedef int	(Tcl_DriverTruncateProc) _ANSI_ARGS_((
		    ClientData instanceData, Tcl_WideInt length));



typedef struct Tcl_ChannelType {
    char *typeName;		
    Tcl_ChannelTypeVersion version;
				
    Tcl_DriverCloseProc *closeProc;
				
    Tcl_DriverInputProc *inputProc;
				
    Tcl_DriverOutputProc *outputProc;
				
    Tcl_DriverSeekProc *seekProc;
				
    Tcl_DriverSetOptionProc *setOptionProc;
				
    Tcl_DriverGetOptionProc *getOptionProc;
				
    Tcl_DriverWatchProc *watchProc;
				
    Tcl_DriverGetHandleProc *getHandleProc;
				
    Tcl_DriverClose2Proc *close2Proc;
				
    Tcl_DriverBlockModeProc *blockModeProc;
				
    
    Tcl_DriverFlushProc *flushProc;
				
    Tcl_DriverHandlerProc *handlerProc;
				
    
    Tcl_DriverWideSeekProc *wideSeekProc;
				
    
    Tcl_DriverThreadActionProc *threadActionProc;
				

    
    Tcl_DriverTruncateProc *truncateProc;
				
} Tcl_ChannelType;



#define TCL_MODE_BLOCKING	0	
#define TCL_MODE_NONBLOCKING	1	



typedef enum Tcl_PathType {
    TCL_PATH_ABSOLUTE,
    TCL_PATH_RELATIVE,
    TCL_PATH_VOLUME_RELATIVE
} Tcl_PathType;



typedef struct Tcl_GlobTypeData {
    int type;			
    int perm;			
    Tcl_Obj *macType;		
    Tcl_Obj *macCreator;	
} Tcl_GlobTypeData;



#define TCL_GLOB_TYPE_BLOCK		(1<<0)
#define TCL_GLOB_TYPE_CHAR		(1<<1)
#define TCL_GLOB_TYPE_DIR		(1<<2)
#define TCL_GLOB_TYPE_PIPE		(1<<3)
#define TCL_GLOB_TYPE_FILE		(1<<4)
#define TCL_GLOB_TYPE_LINK		(1<<5)
#define TCL_GLOB_TYPE_SOCK		(1<<6)
#define TCL_GLOB_TYPE_MOUNT		(1<<7)

#define TCL_GLOB_PERM_RONLY		(1<<0)
#define TCL_GLOB_PERM_HIDDEN		(1<<1)
#define TCL_GLOB_PERM_R			(1<<2)
#define TCL_GLOB_PERM_W			(1<<3)
#define TCL_GLOB_PERM_X			(1<<4)



#define TCL_UNLOAD_DETACH_FROM_INTERPRETER	(1<<0)
#define TCL_UNLOAD_DETACH_FROM_PROCESS		(1<<1)



typedef int (Tcl_FSStatProc) _ANSI_ARGS_((Tcl_Obj *pathPtr, Tcl_StatBuf *buf));
typedef int (Tcl_FSAccessProc) _ANSI_ARGS_((Tcl_Obj *pathPtr, int mode));
typedef Tcl_Channel (Tcl_FSOpenFileChannelProc) _ANSI_ARGS_((
	Tcl_Interp *interp, Tcl_Obj *pathPtr, int mode, int permissions));
typedef int (Tcl_FSMatchInDirectoryProc) _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *result, Tcl_Obj *pathPtr, CONST char *pattern,
	Tcl_GlobTypeData * types));
typedef Tcl_Obj * (Tcl_FSGetCwdProc) _ANSI_ARGS_((Tcl_Interp *interp));
typedef int (Tcl_FSChdirProc) _ANSI_ARGS_((Tcl_Obj *pathPtr));
typedef int (Tcl_FSLstatProc) _ANSI_ARGS_((Tcl_Obj *pathPtr,
	Tcl_StatBuf *buf));
typedef int (Tcl_FSCreateDirectoryProc) _ANSI_ARGS_((Tcl_Obj *pathPtr));
typedef int (Tcl_FSDeleteFileProc) _ANSI_ARGS_((Tcl_Obj *pathPtr));
typedef int (Tcl_FSCopyDirectoryProc) _ANSI_ARGS_((Tcl_Obj *srcPathPtr,
	Tcl_Obj *destPathPtr, Tcl_Obj **errorPtr));
typedef int (Tcl_FSCopyFileProc) _ANSI_ARGS_((Tcl_Obj *srcPathPtr,
	Tcl_Obj *destPathPtr));
typedef int (Tcl_FSRemoveDirectoryProc) _ANSI_ARGS_((Tcl_Obj *pathPtr,
	int recursive, Tcl_Obj **errorPtr));
typedef int (Tcl_FSRenameFileProc) _ANSI_ARGS_((Tcl_Obj *srcPathPtr,
	Tcl_Obj *destPathPtr));
typedef void (Tcl_FSUnloadFileProc) _ANSI_ARGS_((Tcl_LoadHandle loadHandle));
typedef Tcl_Obj * (Tcl_FSListVolumesProc) _ANSI_ARGS_((void));

struct utimbuf;
typedef int (Tcl_FSUtimeProc) _ANSI_ARGS_((Tcl_Obj *pathPtr,
	struct utimbuf *tval));
typedef int (Tcl_FSNormalizePathProc) _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *pathPtr, int nextCheckpoint));
typedef int (Tcl_FSFileAttrsGetProc) _ANSI_ARGS_((Tcl_Interp *interp,
	int index, Tcl_Obj *pathPtr, Tcl_Obj **objPtrRef));
typedef CONST char ** (Tcl_FSFileAttrStringsProc) _ANSI_ARGS_((
	Tcl_Obj *pathPtr, Tcl_Obj **objPtrRef));
typedef int (Tcl_FSFileAttrsSetProc) _ANSI_ARGS_((Tcl_Interp *interp,
	int index, Tcl_Obj *pathPtr, Tcl_Obj *objPtr));
typedef Tcl_Obj * (Tcl_FSLinkProc) _ANSI_ARGS_((Tcl_Obj *pathPtr,
	Tcl_Obj *toPtr, int linkType));
typedef int (Tcl_FSLoadFileProc) _ANSI_ARGS_((Tcl_Interp * interp,
	Tcl_Obj *pathPtr, Tcl_LoadHandle *handlePtr,
	Tcl_FSUnloadFileProc **unloadProcPtr));
typedef int (Tcl_FSPathInFilesystemProc) _ANSI_ARGS_((Tcl_Obj *pathPtr,
	ClientData *clientDataPtr));
typedef Tcl_Obj * (Tcl_FSFilesystemPathTypeProc) _ANSI_ARGS_((
	Tcl_Obj *pathPtr));
typedef Tcl_Obj * (Tcl_FSFilesystemSeparatorProc) _ANSI_ARGS_((
	Tcl_Obj *pathPtr));
typedef void (Tcl_FSFreeInternalRepProc) _ANSI_ARGS_((ClientData clientData));
typedef ClientData (Tcl_FSDupInternalRepProc) _ANSI_ARGS_((
	ClientData clientData));
typedef Tcl_Obj * (Tcl_FSInternalToNormalizedProc) _ANSI_ARGS_((
	ClientData clientData));
typedef ClientData (Tcl_FSCreateInternalRepProc) _ANSI_ARGS_((
	Tcl_Obj *pathPtr));

typedef struct Tcl_FSVersion_ *Tcl_FSVersion;




#define TCL_FILESYSTEM_VERSION_1	((Tcl_FSVersion) 0x1)



typedef struct Tcl_Filesystem {
    CONST char *typeName;	
    int structureLength;	
    Tcl_FSVersion version;	
    Tcl_FSPathInFilesystemProc *pathInFilesystemProc;
				
    Tcl_FSDupInternalRepProc *dupInternalRepProc;
				
    Tcl_FSFreeInternalRepProc *freeInternalRepProc;
				
    Tcl_FSInternalToNormalizedProc *internalToNormalizedProc;
				
    Tcl_FSCreateInternalRepProc *createInternalRepProc;
				
    Tcl_FSNormalizePathProc *normalizePathProc;
				
    Tcl_FSFilesystemPathTypeProc *filesystemPathTypeProc;
				
    Tcl_FSFilesystemSeparatorProc *filesystemSeparatorProc;
				
    Tcl_FSStatProc *statProc;	
    Tcl_FSAccessProc *accessProc;
				
    Tcl_FSOpenFileChannelProc *openFileChannelProc;
				
    Tcl_FSMatchInDirectoryProc *matchInDirectoryProc;
				
    Tcl_FSUtimeProc *utimeProc;	
    Tcl_FSLinkProc *linkProc;	
    Tcl_FSListVolumesProc *listVolumesProc;
				
    Tcl_FSFileAttrStringsProc *fileAttrStringsProc;
				
    Tcl_FSFileAttrsGetProc *fileAttrsGetProc;
				
    Tcl_FSFileAttrsSetProc *fileAttrsSetProc;
				
    Tcl_FSCreateDirectoryProc *createDirectoryProc;
				
    Tcl_FSRemoveDirectoryProc *removeDirectoryProc;
				
    Tcl_FSDeleteFileProc *deleteFileProc;
				
    Tcl_FSCopyFileProc *copyFileProc;
				
    Tcl_FSRenameFileProc *renameFileProc;
				
    Tcl_FSCopyDirectoryProc *copyDirectoryProc;
				
    Tcl_FSLstatProc *lstatProc;	
    Tcl_FSLoadFileProc *loadFileProc;
				
    Tcl_FSGetCwdProc *getCwdProc;
				
    Tcl_FSChdirProc *chdirProc;	
} Tcl_Filesystem;



#define TCL_CREATE_SYMBOLIC_LINK	0x01
#define TCL_CREATE_HARD_LINK		0x02



typedef struct Tcl_NotifierProcs {
    Tcl_SetTimerProc *setTimerProc;
    Tcl_WaitForEventProc *waitForEventProc;
    Tcl_CreateFileHandlerProc *createFileHandlerProc;
    Tcl_DeleteFileHandlerProc *deleteFileHandlerProc;
    Tcl_InitNotifierProc *initNotifierProc;
    Tcl_FinalizeNotifierProc *finalizeNotifierProc;
    Tcl_AlertNotifierProc *alertNotifierProc;
    Tcl_ServiceModeHookProc *serviceModeHookProc;
} Tcl_NotifierProcs;



typedef struct Tcl_EncodingType {
    CONST char *encodingName;	
    Tcl_EncodingConvertProc *toUtfProc;
				
    Tcl_EncodingConvertProc *fromUtfProc;
				
    Tcl_EncodingFreeProc *freeProc;
				
    ClientData clientData;	
    int nullSize;		
} Tcl_EncodingType;



#define TCL_ENCODING_START		0x01
#define TCL_ENCODING_END		0x02
#define TCL_ENCODING_STOPONERROR	0x04





typedef struct Tcl_Token {
    int type;			
    CONST char *start;		
    int size;			
    int numComponents;		
} Tcl_Token;



#define TCL_TOKEN_WORD		1
#define TCL_TOKEN_SIMPLE_WORD	2
#define TCL_TOKEN_TEXT		4
#define TCL_TOKEN_BS		8
#define TCL_TOKEN_COMMAND	16
#define TCL_TOKEN_VARIABLE	32
#define TCL_TOKEN_SUB_EXPR	64
#define TCL_TOKEN_OPERATOR	128
#define TCL_TOKEN_EXPAND_WORD	256



#define TCL_PARSE_SUCCESS		0
#define TCL_PARSE_QUOTE_EXTRA		1
#define TCL_PARSE_BRACE_EXTRA		2
#define TCL_PARSE_MISSING_BRACE		3
#define TCL_PARSE_MISSING_BRACKET	4
#define TCL_PARSE_MISSING_PAREN		5
#define TCL_PARSE_MISSING_QUOTE		6
#define TCL_PARSE_MISSING_VAR_BRACE	7
#define TCL_PARSE_SYNTAX		8
#define TCL_PARSE_BAD_NUMBER		9



#define NUM_STATIC_TOKENS 20

typedef struct Tcl_Parse {
    CONST char *commentStart;	
    int commentSize;		
    CONST char *commandStart;	
    int commandSize;		
    int numWords;		
    Tcl_Token *tokenPtr;	
    int numTokens;		
    int tokensAvailable;	
    int errorType;		

    

    CONST char *string;		
    CONST char *end;		
    Tcl_Interp *interp;		
    CONST char *term;		
    int incomplete;		
    Tcl_Token staticTokens[NUM_STATIC_TOKENS];
				
} Tcl_Parse;



typedef struct Tcl_Breakpoint {
    Tcl_Obj *fileName;		
    int lineNum;		
    int state;			
    int counter;		
    struct Tcl_Breakpoint *next;
} Tcl_Breakpoint;


typedef struct {
  Tcl_Obj *curFileName;		
  int curLineNum;		
  int lastTracedLine;		
  int curCharPos;		
  int commandLen;		
  int stackLevel;		
  Tcl_Obj* lastResultPtr;	
  int flags;                    
  Tcl_Breakpoint *breakpoints;  
} Tcl_SourceInfo;





#define TCL_CONVERT_MULTIBYTE	-1
#define TCL_CONVERT_SYNTAX	-2
#define TCL_CONVERT_UNKNOWN	-3
#define TCL_CONVERT_NOSPACE	-4



#ifndef TCL_UTF_MAX
#define TCL_UTF_MAX		3
#endif



#if TCL_UTF_MAX > 4
    
typedef unsigned int Tcl_UniChar;
#else
typedef unsigned short Tcl_UniChar;
#endif



typedef struct Tcl_Config {
    CONST char *key;		
    CONST char *value;		
} Tcl_Config;



#define TCL_LIMIT_COMMANDS	0x01
#define TCL_LIMIT_TIME		0x02



typedef void (Tcl_LimitHandlerProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp));
typedef void (Tcl_LimitHandlerDeleteProc) _ANSI_ARGS_((ClientData clientData));

typedef struct mp_int mp_int;
#define MP_INT_DECLARED
typedef unsigned int mp_digit;
#define MP_DIGIT_DECLARED



#define TCL_STUB_MAGIC		((int) 0xFCA3BACF)



EXTERN CONST char *	Tcl_InitStubs _ANSI_ARGS_((Tcl_Interp *interp,
			    CONST char *version, int exact));
EXTERN CONST char *	TclTomMathInitializeStubs _ANSI_ARGS_((
			    Tcl_Interp *interp, CONST char *version,
			    int epoch, int revision));

#ifndef USE_TCL_STUBS



#define Tcl_InitStubs(interp, version, exact) \
    Tcl_PkgInitStubsCheck(interp, version, exact)

#endif

    




EXTERN void		Tcl_Main _ANSI_ARGS_((int argc, char **argv,
			    Tcl_AppInitProc *appInitProc));
EXTERN CONST char *	Tcl_PkgInitStubsCheck _ANSI_ARGS_((Tcl_Interp *interp,
			    CONST char *version, int exact));
#if defined(TCL_THREADS) && defined(USE_THREAD_ALLOC)
EXTERN void		Tcl_GetMemoryInfo _ANSI_ARGS_((Tcl_DString *dsPtr));
#endif



#include "tclDecls.h"



#include "tclPlatDecls.h"



#ifdef TCL_MEM_DEBUG

#   define ckalloc(x) Tcl_DbCkalloc(x, __FILE__, __LINE__)
#   define ckfree(x)  Tcl_DbCkfree(x, __FILE__, __LINE__)
#   define ckrealloc(x,y) Tcl_DbCkrealloc((x), (y),__FILE__, __LINE__)
#   define attemptckalloc(x) Tcl_AttemptDbCkalloc(x, __FILE__, __LINE__)
#   define attemptckrealloc(x,y) Tcl_AttemptDbCkrealloc((x), (y), __FILE__, __LINE__)

#else 



#   define ckalloc(x) Tcl_Alloc(x)
#   define ckfree(x) Tcl_Free(x)
#   define ckrealloc(x,y) Tcl_Realloc(x,y)
#   define attemptckalloc(x) Tcl_AttemptAlloc(x)
#   define attemptckrealloc(x,y) Tcl_AttemptRealloc(x,y)
#   undef  Tcl_InitMemory
#   define Tcl_InitMemory(x)
#   undef  Tcl_DumpActiveMemory
#   define Tcl_DumpActiveMemory(x)
#   undef  Tcl_ValidateAllMemory
#   define Tcl_ValidateAllMemory(x,y)

#endif 

#ifdef TCL_MEM_DEBUG
#   define Tcl_IncrRefCount(objPtr) \
	Tcl_DbIncrRefCount(objPtr, __FILE__, __LINE__)
#   define Tcl_DecrRefCount(objPtr) \
	Tcl_DbDecrRefCount(objPtr, __FILE__, __LINE__)
#   define Tcl_IsShared(objPtr) \
	Tcl_DbIsShared(objPtr, __FILE__, __LINE__)
#else
#   define Tcl_IncrRefCount(objPtr) \
	++(objPtr)->refCount
    
#   define Tcl_DecrRefCount(objPtr) \
	do { \
	    Tcl_Obj *_objPtr = (objPtr); \
	    if (--(_objPtr)->refCount <= 0) { \
		TclFreeObj(_objPtr); \
	    } \
	} while(0)
#   define Tcl_IsShared(objPtr) \
	((objPtr)->refCount > 1)
#endif



#ifdef TCL_MEM_DEBUG
#  undef  Tcl_NewBignumObj
#  define Tcl_NewBignumObj(val) \
     Tcl_DbNewBignumObj(val, __FILE__, __LINE__)
#  undef  Tcl_NewBooleanObj
#  define Tcl_NewBooleanObj(val) \
     Tcl_DbNewBooleanObj(val, __FILE__, __LINE__)
#  undef  Tcl_NewByteArrayObj
#  define Tcl_NewByteArrayObj(bytes, len) \
     Tcl_DbNewByteArrayObj(bytes, len, __FILE__, __LINE__)
#  undef  Tcl_NewDoubleObj
#  define Tcl_NewDoubleObj(val) \
     Tcl_DbNewDoubleObj(val, __FILE__, __LINE__)
#  undef  Tcl_NewIntObj
#  define Tcl_NewIntObj(val) \
     Tcl_DbNewLongObj(val, __FILE__, __LINE__)
#  undef  Tcl_NewListObj
#  define Tcl_NewListObj(objc, objv) \
     Tcl_DbNewListObj(objc, objv, __FILE__, __LINE__)
#  undef  Tcl_NewLongObj
#  define Tcl_NewLongObj(val) \
     Tcl_DbNewLongObj(val, __FILE__, __LINE__)
#  undef  Tcl_NewObj
#  define Tcl_NewObj() \
     Tcl_DbNewObj(__FILE__, __LINE__)
#  undef  Tcl_NewStringObj
#  define Tcl_NewStringObj(bytes, len) \
     Tcl_DbNewStringObj(bytes, len, __FILE__, __LINE__)
#  undef  Tcl_NewWideIntObj
#  define Tcl_NewWideIntObj(val) \
     Tcl_DbNewWideIntObj(val, __FILE__, __LINE__)
#endif 



#define Tcl_GetHashValue(h) ((h)->clientData)
#define Tcl_SetHashValue(h, value) ((h)->clientData = (ClientData) (value))
#define Tcl_GetHashKey(tablePtr, h) \
	((char *) (((tablePtr)->keyType == TCL_ONE_WORD_KEYS || \
		    (tablePtr)->keyType == TCL_CUSTOM_PTR_KEYS) \
		   ? (h)->key.oneWordValue \
		   : (h)->key.string))



#undef  Tcl_FindHashEntry
#define Tcl_FindHashEntry(tablePtr, key) \
	(*((tablePtr)->findProc))(tablePtr, key)
#undef  Tcl_CreateHashEntry
#define Tcl_CreateHashEntry(tablePtr, key, newPtr) \
	(*((tablePtr)->createProc))(tablePtr, key, newPtr)



#ifndef TCL_THREADS
#undef  Tcl_MutexLock
#define Tcl_MutexLock(mutexPtr)
#undef  Tcl_MutexUnlock
#define Tcl_MutexUnlock(mutexPtr)
#undef  Tcl_MutexFinalize
#define Tcl_MutexFinalize(mutexPtr)
#undef  Tcl_ConditionNotify
#define Tcl_ConditionNotify(condPtr)
#undef  Tcl_ConditionWait
#define Tcl_ConditionWait(condPtr, mutexPtr, timePtr)
#undef  Tcl_ConditionFinalize
#define Tcl_ConditionFinalize(condPtr)
#endif 

#ifndef TCL_NO_DEPRECATED
    

#   define Tcl_Ckalloc		Tcl_Alloc
#   define Tcl_Ckfree		Tcl_Free
#   define Tcl_Ckrealloc	Tcl_Realloc
#   define Tcl_Return		Tcl_SetResult
#   define Tcl_TildeSubst	Tcl_TranslateFileName
#   define panic		Tcl_Panic
#   define panicVA		Tcl_PanicVA
#endif



#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS

EXTERN int		Tcl_AppInit _ANSI_ARGS_((Tcl_Interp *interp));

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT

#endif 



#ifdef __cplusplus
}
#endif

#endif 


