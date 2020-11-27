

#ifndef _TKPLATDECLS
#define _TKPLATDECLS

#ifdef BUILD_tk
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#endif





#ifdef __cplusplus
extern "C" {
#endif



#if defined(__WIN32__) || defined(__CYGWIN__) 
#ifndef Tk_AttachHWND_TCL_DECLARED
#define Tk_AttachHWND_TCL_DECLARED

EXTERN Window		Tk_AttachHWND(Tk_Window tkwin, HWND hwnd);
#endif
#ifndef Tk_GetHINSTANCE_TCL_DECLARED
#define Tk_GetHINSTANCE_TCL_DECLARED

EXTERN HINSTANCE	Tk_GetHINSTANCE(void);
#endif
#ifndef Tk_GetHWND_TCL_DECLARED
#define Tk_GetHWND_TCL_DECLARED

EXTERN HWND		Tk_GetHWND(Window window);
#endif
#ifndef Tk_HWNDToWindow_TCL_DECLARED
#define Tk_HWNDToWindow_TCL_DECLARED

EXTERN Tk_Window	Tk_HWNDToWindow(HWND hwnd);
#endif
#ifndef Tk_PointerEvent_TCL_DECLARED
#define Tk_PointerEvent_TCL_DECLARED

EXTERN void		Tk_PointerEvent(HWND hwnd, int x, int y);
#endif
#ifndef Tk_TranslateWinEvent_TCL_DECLARED
#define Tk_TranslateWinEvent_TCL_DECLARED

EXTERN int		Tk_TranslateWinEvent(HWND hwnd, UINT message,
				WPARAM wParam, LPARAM lParam,
				LRESULT *result);
#endif
#endif 
#ifdef MAC_OSX_TK 
#ifndef Tk_MacOSXSetEmbedHandler_TCL_DECLARED
#define Tk_MacOSXSetEmbedHandler_TCL_DECLARED

EXTERN void		Tk_MacOSXSetEmbedHandler(
				Tk_MacOSXEmbedRegisterWinProc *registerWinProcPtr,
				Tk_MacOSXEmbedGetGrafPortProc *getPortProcPtr,
				Tk_MacOSXEmbedMakeContainerExistProc *containerExistProcPtr,
				Tk_MacOSXEmbedGetClipProc *getClipProc,
				Tk_MacOSXEmbedGetOffsetInParentProc *getOffsetProc);
#endif
#ifndef Tk_MacOSXTurnOffMenus_TCL_DECLARED
#define Tk_MacOSXTurnOffMenus_TCL_DECLARED

EXTERN void		Tk_MacOSXTurnOffMenus(void);
#endif
#ifndef Tk_MacOSXTkOwnsCursor_TCL_DECLARED
#define Tk_MacOSXTkOwnsCursor_TCL_DECLARED

EXTERN void		Tk_MacOSXTkOwnsCursor(int tkOwnsIt);
#endif
#ifndef TkMacOSXInitMenus_TCL_DECLARED
#define TkMacOSXInitMenus_TCL_DECLARED

EXTERN void		TkMacOSXInitMenus(Tcl_Interp *interp);
#endif
#ifndef TkMacOSXInitAppleEvents_TCL_DECLARED
#define TkMacOSXInitAppleEvents_TCL_DECLARED

EXTERN void		TkMacOSXInitAppleEvents(Tcl_Interp *interp);
#endif
#ifndef TkGenWMConfigureEvent_TCL_DECLARED
#define TkGenWMConfigureEvent_TCL_DECLARED

EXTERN void		TkGenWMConfigureEvent(Tk_Window tkwin, int x, int y,
				int width, int height, int flags);
#endif
#ifndef TkMacOSXInvalClipRgns_TCL_DECLARED
#define TkMacOSXInvalClipRgns_TCL_DECLARED

EXTERN void		TkMacOSXInvalClipRgns(Tk_Window tkwin);
#endif
#ifndef TkMacOSXGetDrawablePort_TCL_DECLARED
#define TkMacOSXGetDrawablePort_TCL_DECLARED

EXTERN VOID *		TkMacOSXGetDrawablePort(Drawable drawable);
#endif
#ifndef TkMacOSXGetRootControl_TCL_DECLARED
#define TkMacOSXGetRootControl_TCL_DECLARED

EXTERN VOID *		TkMacOSXGetRootControl(Drawable drawable);
#endif
#ifndef Tk_MacOSXSetupTkNotifier_TCL_DECLARED
#define Tk_MacOSXSetupTkNotifier_TCL_DECLARED

EXTERN void		Tk_MacOSXSetupTkNotifier(void);
#endif
#ifndef Tk_MacOSXIsAppInFront_TCL_DECLARED
#define Tk_MacOSXIsAppInFront_TCL_DECLARED

EXTERN int		Tk_MacOSXIsAppInFront(void);
#endif
#endif 

typedef struct TkPlatStubs {
    int magic;
    struct TkPlatStubHooks *hooks;

#if defined(__WIN32__) || defined(__CYGWIN__) 
    Window (*tk_AttachHWND) (Tk_Window tkwin, HWND hwnd); 
    HINSTANCE (*tk_GetHINSTANCE) (void); 
    HWND (*tk_GetHWND) (Window window); 
    Tk_Window (*tk_HWNDToWindow) (HWND hwnd); 
    void (*tk_PointerEvent) (HWND hwnd, int x, int y); 
    int (*tk_TranslateWinEvent) (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result); 
#endif 
#ifdef MAC_OSX_TK 
    void (*tk_MacOSXSetEmbedHandler) (Tk_MacOSXEmbedRegisterWinProc *registerWinProcPtr, Tk_MacOSXEmbedGetGrafPortProc *getPortProcPtr, Tk_MacOSXEmbedMakeContainerExistProc *containerExistProcPtr, Tk_MacOSXEmbedGetClipProc *getClipProc, Tk_MacOSXEmbedGetOffsetInParentProc *getOffsetProc); 
    void (*tk_MacOSXTurnOffMenus) (void); 
    void (*tk_MacOSXTkOwnsCursor) (int tkOwnsIt); 
    void (*tkMacOSXInitMenus) (Tcl_Interp *interp); 
    void (*tkMacOSXInitAppleEvents) (Tcl_Interp *interp); 
    void (*tkGenWMConfigureEvent) (Tk_Window tkwin, int x, int y, int width, int height, int flags); 
    void (*tkMacOSXInvalClipRgns) (Tk_Window tkwin); 
    VOID * (*tkMacOSXGetDrawablePort) (Drawable drawable); 
    VOID * (*tkMacOSXGetRootControl) (Drawable drawable); 
    void (*tk_MacOSXSetupTkNotifier) (void); 
    int (*tk_MacOSXIsAppInFront) (void); 
#endif 
} TkPlatStubs;

extern TkPlatStubs *tkPlatStubsPtr;

#ifdef __cplusplus
}
#endif

#if defined(USE_TK_STUBS) && !defined(USE_TK_STUB_PROCS)



#if defined(__WIN32__) || defined(__CYGWIN__) 
#ifndef Tk_AttachHWND
#define Tk_AttachHWND \
	(tkPlatStubsPtr->tk_AttachHWND) 
#endif
#ifndef Tk_GetHINSTANCE
#define Tk_GetHINSTANCE \
	(tkPlatStubsPtr->tk_GetHINSTANCE) 
#endif
#ifndef Tk_GetHWND
#define Tk_GetHWND \
	(tkPlatStubsPtr->tk_GetHWND) 
#endif
#ifndef Tk_HWNDToWindow
#define Tk_HWNDToWindow \
	(tkPlatStubsPtr->tk_HWNDToWindow) 
#endif
#ifndef Tk_PointerEvent
#define Tk_PointerEvent \
	(tkPlatStubsPtr->tk_PointerEvent) 
#endif
#ifndef Tk_TranslateWinEvent
#define Tk_TranslateWinEvent \
	(tkPlatStubsPtr->tk_TranslateWinEvent) 
#endif
#endif 
#ifdef MAC_OSX_TK 
#ifndef Tk_MacOSXSetEmbedHandler
#define Tk_MacOSXSetEmbedHandler \
	(tkPlatStubsPtr->tk_MacOSXSetEmbedHandler) 
#endif
#ifndef Tk_MacOSXTurnOffMenus
#define Tk_MacOSXTurnOffMenus \
	(tkPlatStubsPtr->tk_MacOSXTurnOffMenus) 
#endif
#ifndef Tk_MacOSXTkOwnsCursor
#define Tk_MacOSXTkOwnsCursor \
	(tkPlatStubsPtr->tk_MacOSXTkOwnsCursor) 
#endif
#ifndef TkMacOSXInitMenus
#define TkMacOSXInitMenus \
	(tkPlatStubsPtr->tkMacOSXInitMenus) 
#endif
#ifndef TkMacOSXInitAppleEvents
#define TkMacOSXInitAppleEvents \
	(tkPlatStubsPtr->tkMacOSXInitAppleEvents) 
#endif
#ifndef TkGenWMConfigureEvent
#define TkGenWMConfigureEvent \
	(tkPlatStubsPtr->tkGenWMConfigureEvent) 
#endif
#ifndef TkMacOSXInvalClipRgns
#define TkMacOSXInvalClipRgns \
	(tkPlatStubsPtr->tkMacOSXInvalClipRgns) 
#endif
#ifndef TkMacOSXGetDrawablePort
#define TkMacOSXGetDrawablePort \
	(tkPlatStubsPtr->tkMacOSXGetDrawablePort) 
#endif
#ifndef TkMacOSXGetRootControl
#define TkMacOSXGetRootControl \
	(tkPlatStubsPtr->tkMacOSXGetRootControl) 
#endif
#ifndef Tk_MacOSXSetupTkNotifier
#define Tk_MacOSXSetupTkNotifier \
	(tkPlatStubsPtr->tk_MacOSXSetupTkNotifier) 
#endif
#ifndef Tk_MacOSXIsAppInFront
#define Tk_MacOSXIsAppInFront \
	(tkPlatStubsPtr->tk_MacOSXIsAppInFront) 
#endif
#endif 

#endif 



#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT

#endif 
