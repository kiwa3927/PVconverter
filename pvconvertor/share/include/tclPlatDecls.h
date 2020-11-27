

#ifndef _TCLPLATDECLS
#define _TCLPLATDECLS

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


#if (defined(_WIN32) || defined(__CYGWIN__)) && !defined(_TCHAR_DEFINED)
#   if defined(_UNICODE)
	typedef wchar_t TCHAR;
#   else
	typedef char TCHAR;
#   endif
#   define _TCHAR_DEFINED
#endif



#ifdef __cplusplus
extern "C" {
#endif



#if defined(__WIN32__) || defined(__CYGWIN__) 
#ifndef Tcl_WinUtfToTChar_TCL_DECLARED
#define Tcl_WinUtfToTChar_TCL_DECLARED

EXTERN TCHAR *		Tcl_WinUtfToTChar(CONST char *str, int len,
				Tcl_DString *dsPtr);
#endif
#ifndef Tcl_WinTCharToUtf_TCL_DECLARED
#define Tcl_WinTCharToUtf_TCL_DECLARED

EXTERN char *		Tcl_WinTCharToUtf(CONST TCHAR *str, int len,
				Tcl_DString *dsPtr);
#endif
#endif 
#ifdef MAC_OSX_TCL 
#ifndef Tcl_MacOSXOpenBundleResources_TCL_DECLARED
#define Tcl_MacOSXOpenBundleResources_TCL_DECLARED

EXTERN int		Tcl_MacOSXOpenBundleResources(Tcl_Interp *interp,
				CONST char *bundleName, int hasResourceFile,
				int maxPathLen, char *libraryPath);
#endif
#ifndef Tcl_MacOSXOpenVersionedBundleResources_TCL_DECLARED
#define Tcl_MacOSXOpenVersionedBundleResources_TCL_DECLARED

EXTERN int		Tcl_MacOSXOpenVersionedBundleResources(
				Tcl_Interp *interp, CONST char *bundleName,
				CONST char *bundleVersion,
				int hasResourceFile, int maxPathLen,
				char *libraryPath);
#endif
#endif 

typedef struct TclPlatStubs {
    int magic;
    struct TclPlatStubHooks *hooks;

#if defined(__WIN32__) || defined(__CYGWIN__) 
    TCHAR * (*tcl_WinUtfToTChar) (CONST char *str, int len, Tcl_DString *dsPtr); 
    char * (*tcl_WinTCharToUtf) (CONST TCHAR *str, int len, Tcl_DString *dsPtr); 
#endif 
#ifdef MAC_OSX_TCL 
    int (*tcl_MacOSXOpenBundleResources) (Tcl_Interp *interp, CONST char *bundleName, int hasResourceFile, int maxPathLen, char *libraryPath); 
    int (*tcl_MacOSXOpenVersionedBundleResources) (Tcl_Interp *interp, CONST char *bundleName, CONST char *bundleVersion, int hasResourceFile, int maxPathLen, char *libraryPath); 
#endif 
} TclPlatStubs;

extern TclPlatStubs *tclPlatStubsPtr;

#ifdef __cplusplus
}
#endif

#if defined(USE_TCL_STUBS) && !defined(USE_TCL_STUB_PROCS)



#if defined(__WIN32__) || defined(__CYGWIN__) 
#ifndef Tcl_WinUtfToTChar
#define Tcl_WinUtfToTChar \
	(tclPlatStubsPtr->tcl_WinUtfToTChar) 
#endif
#ifndef Tcl_WinTCharToUtf
#define Tcl_WinTCharToUtf \
	(tclPlatStubsPtr->tcl_WinTCharToUtf) 
#endif
#endif 
#ifdef MAC_OSX_TCL 
#ifndef Tcl_MacOSXOpenBundleResources
#define Tcl_MacOSXOpenBundleResources \
	(tclPlatStubsPtr->tcl_MacOSXOpenBundleResources) 
#endif
#ifndef Tcl_MacOSXOpenVersionedBundleResources
#define Tcl_MacOSXOpenVersionedBundleResources \
	(tclPlatStubsPtr->tcl_MacOSXOpenVersionedBundleResources) 
#endif
#endif 

#endif 



#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT

#endif 


