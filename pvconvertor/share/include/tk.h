

#ifndef _TK
#define _TK

#include <tcl.h>
#if (TCL_MAJOR_VERSION != 8) || (TCL_MINOR_VERSION < 5)
#	error Tk 8.5 must be compiled with tcl.h from Tcl 8.5 or better
#endif

#ifndef _ANSI_ARGS_
#   ifndef NO_PROTOTYPES
#	define _ANSI_ARGS_(x)	x
#   else
#	define _ANSI_ARGS_(x)	()
#   endif
#endif



#ifdef __cplusplus
extern "C" {
#endif



#define TK_MAJOR_VERSION	8
#define TK_MINOR_VERSION	5
#define TK_RELEASE_LEVEL	TCL_FINAL_RELEASE
#define TK_RELEASE_SERIAL	19

#define TK_VERSION		"8.5"
#define TK_PATCH_LEVEL		"8.5.19"



#ifndef RC_INVOKED

#ifndef _XLIB_H
#   include <X11/Xlib.h>
#   ifdef MAC_OSX_TK
#	include <X11/X.h>
#   endif
#endif
#ifdef __STDC__
#   include <stddef.h>
#endif

#ifdef BUILD_tk
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS	DLLEXPORT
#endif



#ifdef XNQueryInputStyle
#define TK_USE_INPUT_METHODS
#endif



typedef struct Tk_BindingTable_ *Tk_BindingTable;
typedef struct Tk_Canvas_ *Tk_Canvas;
typedef struct Tk_Cursor_ *Tk_Cursor;
typedef struct Tk_ErrorHandler_ *Tk_ErrorHandler;
typedef struct Tk_Font_ *Tk_Font;
typedef struct Tk_Image__ *Tk_Image;
typedef struct Tk_ImageMaster_ *Tk_ImageMaster;
typedef struct Tk_OptionTable_ *Tk_OptionTable;
typedef struct Tk_PostscriptInfo_ *Tk_PostscriptInfo;
typedef struct Tk_TextLayout_ *Tk_TextLayout;
typedef struct Tk_Window_ *Tk_Window;
typedef struct Tk_3DBorder_ *Tk_3DBorder;
typedef struct Tk_Style_ *Tk_Style;
typedef struct Tk_StyleEngine_ *Tk_StyleEngine;
typedef struct Tk_StyledElement_ *Tk_StyledElement;



typedef const char *Tk_Uid;



typedef enum {
    TK_OPTION_BOOLEAN,
    TK_OPTION_INT,
    TK_OPTION_DOUBLE,
    TK_OPTION_STRING,
    TK_OPTION_STRING_TABLE,
    TK_OPTION_COLOR,
    TK_OPTION_FONT,
    TK_OPTION_BITMAP,
    TK_OPTION_BORDER,
    TK_OPTION_RELIEF,
    TK_OPTION_CURSOR,
    TK_OPTION_JUSTIFY,
    TK_OPTION_ANCHOR,
    TK_OPTION_SYNONYM,
    TK_OPTION_PIXELS,
    TK_OPTION_WINDOW,
    TK_OPTION_END,
    TK_OPTION_CUSTOM,
    TK_OPTION_STYLE
} Tk_OptionType;



typedef struct Tk_OptionSpec {
    Tk_OptionType type;		
    const char *optionName;	
    const char *dbName;		
    const char *dbClass;	
    const char *defValue;	
    int objOffset;		
    int internalOffset;		
    int flags;			
    ClientData clientData;	
    int typeMask;		
} Tk_OptionSpec;



#define TK_OPTION_NULL_OK		(1 << 0)
#define TK_OPTION_DONT_SET_DEFAULT	(1 << 3)



typedef int (Tk_CustomOptionSetProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, Tk_Window tkwin, Tcl_Obj **value, char *widgRec,
	int offset, char *saveInternalPtr, int flags));
typedef Tcl_Obj *(Tk_CustomOptionGetProc) _ANSI_ARGS_((ClientData clientData,
	Tk_Window tkwin, char *widgRec, int offset));
typedef void (Tk_CustomOptionRestoreProc) _ANSI_ARGS_((ClientData clientData,
	Tk_Window tkwin, char *internalPtr, char *saveInternalPtr));
typedef void (Tk_CustomOptionFreeProc) _ANSI_ARGS_((ClientData clientData,
	Tk_Window tkwin, char *internalPtr));

typedef struct Tk_ObjCustomOption {
    const char *name;		
    Tk_CustomOptionSetProc *setProc;
				
    Tk_CustomOptionGetProc *getProc;
				
    Tk_CustomOptionRestoreProc *restoreProc;
				
    Tk_CustomOptionFreeProc *freeProc;
				
    ClientData clientData;	
} Tk_ObjCustomOption;



#ifdef offsetof
#define Tk_Offset(type, field) ((int) offsetof(type, field))
#else
#define Tk_Offset(type, field) ((int) ((char *) &((type *) 0)->field))
#endif



typedef struct Tk_SavedOption {
    struct TkOption *optionPtr;	
    Tcl_Obj *valuePtr;		
    double internalForm;	
} Tk_SavedOption;

#ifdef TCL_MEM_DEBUG
#   define TK_NUM_SAVED_OPTIONS 2
#else
#   define TK_NUM_SAVED_OPTIONS 20
#endif

typedef struct Tk_SavedOptions {
    char *recordPtr;		
    Tk_Window tkwin;		
    int numItems;		
    Tk_SavedOption items[TK_NUM_SAVED_OPTIONS];
				
    struct Tk_SavedOptions *nextPtr;
				
} Tk_SavedOptions;





#ifndef __NO_OLD_CONFIG

typedef int (Tk_OptionParseProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, Tk_Window tkwin, CONST84 char *value, char *widgRec,
	int offset));
typedef char *(Tk_OptionPrintProc) _ANSI_ARGS_((ClientData clientData,
	Tk_Window tkwin, char *widgRec, int offset,
	Tcl_FreeProc **freeProcPtr));

typedef struct Tk_CustomOption {
    Tk_OptionParseProc *parseProc;
				
    Tk_OptionPrintProc *printProc;
				
    ClientData clientData;	
} Tk_CustomOption;



typedef struct Tk_ConfigSpec {
    int type;			
    char *argvName;		
    Tk_Uid dbName;		
    Tk_Uid dbClass;		
    Tk_Uid defValue;		
    int offset;			
    int specFlags;		
    Tk_CustomOption *customPtr;	
} Tk_ConfigSpec;



typedef enum {
    TK_CONFIG_BOOLEAN, TK_CONFIG_INT, TK_CONFIG_DOUBLE, TK_CONFIG_STRING,
    TK_CONFIG_UID, TK_CONFIG_COLOR, TK_CONFIG_FONT, TK_CONFIG_BITMAP,
    TK_CONFIG_BORDER, TK_CONFIG_RELIEF, TK_CONFIG_CURSOR,
    TK_CONFIG_ACTIVE_CURSOR, TK_CONFIG_JUSTIFY, TK_CONFIG_ANCHOR,
    TK_CONFIG_SYNONYM, TK_CONFIG_CAP_STYLE, TK_CONFIG_JOIN_STYLE,
    TK_CONFIG_PIXELS, TK_CONFIG_MM, TK_CONFIG_WINDOW, TK_CONFIG_CUSTOM,
    TK_CONFIG_END
} Tk_ConfigTypes;



#define TK_CONFIG_ARGV_ONLY	1
#define TK_CONFIG_OBJS		0x80



#define TK_CONFIG_NULL_OK		(1 << 0)
#define TK_CONFIG_COLOR_ONLY		(1 << 1)
#define TK_CONFIG_MONO_ONLY		(1 << 2)
#define TK_CONFIG_DONT_SET_DEFAULT	(1 << 3)
#define TK_CONFIG_OPTION_SPECIFIED      (1 << 4)
#define TK_CONFIG_USER_BIT		0x100
#endif 



typedef struct {
    char *key;			
    int type;			
    char *src;			
    char *dst;			
    char *help;			
} Tk_ArgvInfo;



#define TK_ARGV_CONSTANT		15
#define TK_ARGV_INT			16
#define TK_ARGV_STRING			17
#define TK_ARGV_UID			18
#define TK_ARGV_REST			19
#define TK_ARGV_FLOAT			20
#define TK_ARGV_FUNC			21
#define TK_ARGV_GENFUNC			22
#define TK_ARGV_HELP			23
#define TK_ARGV_CONST_OPTION		24
#define TK_ARGV_OPTION_VALUE		25
#define TK_ARGV_OPTION_NAME_VALUE	26
#define TK_ARGV_END			27



#define TK_ARGV_NO_DEFAULTS		0x1
#define TK_ARGV_NO_LEFTOVERS		0x2
#define TK_ARGV_NO_ABBREV		0x4
#define TK_ARGV_DONT_SKIP_FIRST_ARG	0x8



typedef enum {
    TK_DEFER_EVENT, TK_PROCESS_EVENT, TK_DISCARD_EVENT
} Tk_RestrictAction;



#define TK_WIDGET_DEFAULT_PRIO	20
#define TK_STARTUP_FILE_PRIO	40
#define TK_USER_DEFAULT_PRIO	60
#define TK_INTERACTIVE_PRIO	80
#define TK_MAX_PRIO		100



#define TK_RELIEF_NULL		-1
#define TK_RELIEF_FLAT		0
#define TK_RELIEF_GROOVE	1
#define TK_RELIEF_RAISED	2
#define TK_RELIEF_RIDGE		3
#define TK_RELIEF_SOLID		4
#define TK_RELIEF_SUNKEN	5



#define TK_3D_FLAT_GC		1
#define TK_3D_LIGHT_GC		2
#define TK_3D_DARK_GC		3



#define TK_NOTIFY_SHARE		20



typedef enum {
    TK_ANCHOR_N, TK_ANCHOR_NE, TK_ANCHOR_E, TK_ANCHOR_SE,
    TK_ANCHOR_S, TK_ANCHOR_SW, TK_ANCHOR_W, TK_ANCHOR_NW,
    TK_ANCHOR_CENTER
} Tk_Anchor;



typedef enum {
    TK_JUSTIFY_LEFT, TK_JUSTIFY_RIGHT, TK_JUSTIFY_CENTER
} Tk_Justify;



typedef struct Tk_FontMetrics {
    int ascent;			
    int descent;		
    int linespace;		
} Tk_FontMetrics;



#define TK_WHOLE_WORDS		1
#define TK_AT_LEAST_ONE		2
#define TK_PARTIAL_OK		4



#define TK_IGNORE_TABS		8
#define TK_IGNORE_NEWLINES	16



typedef Window (Tk_ClassCreateProc) _ANSI_ARGS_((Tk_Window tkwin,
	Window parent, ClientData instanceData));
typedef void (Tk_ClassWorldChangedProc) _ANSI_ARGS_((ClientData instanceData));
typedef void (Tk_ClassModalProc) _ANSI_ARGS_((Tk_Window tkwin,
	XEvent *eventPtr));

typedef struct Tk_ClassProcs {
    unsigned int size;
    Tk_ClassWorldChangedProc *worldChangedProc;
				
    Tk_ClassCreateProc *createProc;
				
    Tk_ClassModalProc *modalProc;
				
} Tk_ClassProcs;



#define Tk_GetClassProc(procs, which) \
    (((procs) == NULL) ? NULL : \
    (((procs)->size <= Tk_Offset(Tk_ClassProcs, which)) ? NULL:(procs)->which))



typedef void (Tk_GeomRequestProc) _ANSI_ARGS_((ClientData clientData,
	Tk_Window tkwin));
typedef void (Tk_GeomLostSlaveProc) _ANSI_ARGS_((ClientData clientData,
	Tk_Window tkwin));

typedef struct Tk_GeomMgr {
    const char *name;		
    Tk_GeomRequestProc *requestProc;
				
    Tk_GeomLostSlaveProc *lostSlaveProc;
				
} Tk_GeomMgr;



#define TK_SCROLL_MOVETO	1
#define TK_SCROLL_PAGES		2
#define TK_SCROLL_UNITS		3
#define TK_SCROLL_ERROR		4



#define VirtualEvent	    (MappingNotify + 1)
#define ActivateNotify	    (MappingNotify + 2)
#define DeactivateNotify    (MappingNotify + 3)
#define MouseWheelEvent     (MappingNotify + 4)
#define TK_LASTEVENT	    (MappingNotify + 5)

#define MouseWheelMask	    (1L << 28)
#define ActivateMask	    (1L << 29)
#define VirtualEventMask    (1L << 30)



typedef struct {
    int type;
    unsigned long serial;	
    Bool send_event;		
    Display *display;		
    Window event;		
    Window root;		
    Window subwindow;		
    Time time;			
    int x, y;			
    int x_root, y_root;		
    unsigned int state;		
    Tk_Uid name;		
    Bool same_screen;		
    Tcl_Obj *user_data;		
} XVirtualEvent;

typedef struct {
    int type;
    unsigned long serial;	
    Bool send_event;		
    Display *display;		
    Window window;		
} XActivateDeactivateEvent;
typedef XActivateDeactivateEvent XActivateEvent;
typedef XActivateDeactivateEvent XDeactivateEvent;



#define Tk_Display(tkwin)	(((Tk_FakeWin *) (tkwin))->display)
#define Tk_ScreenNumber(tkwin)	(((Tk_FakeWin *) (tkwin))->screenNum)
#define Tk_Screen(tkwin) \
    (ScreenOfDisplay(Tk_Display(tkwin), Tk_ScreenNumber(tkwin)))
#define Tk_Depth(tkwin)		(((Tk_FakeWin *) (tkwin))->depth)
#define Tk_Visual(tkwin)	(((Tk_FakeWin *) (tkwin))->visual)
#define Tk_WindowId(tkwin)	(((Tk_FakeWin *) (tkwin))->window)
#define Tk_PathName(tkwin) 	(((Tk_FakeWin *) (tkwin))->pathName)
#define Tk_Name(tkwin)		(((Tk_FakeWin *) (tkwin))->nameUid)
#define Tk_Class(tkwin) 	(((Tk_FakeWin *) (tkwin))->classUid)
#define Tk_X(tkwin)		(((Tk_FakeWin *) (tkwin))->changes.x)
#define Tk_Y(tkwin)		(((Tk_FakeWin *) (tkwin))->changes.y)
#define Tk_Width(tkwin)		(((Tk_FakeWin *) (tkwin))->changes.width)
#define Tk_Height(tkwin) \
    (((Tk_FakeWin *) (tkwin))->changes.height)
#define Tk_Changes(tkwin)	(&((Tk_FakeWin *) (tkwin))->changes)
#define Tk_Attributes(tkwin)	(&((Tk_FakeWin *) (tkwin))->atts)
#define Tk_IsEmbedded(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_EMBEDDED)
#define Tk_IsContainer(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_CONTAINER)
#define Tk_IsMapped(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_MAPPED)
#define Tk_IsTopLevel(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_TOP_LEVEL)
#define Tk_HasWrapper(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_HAS_WRAPPER)
#define Tk_WinManaged(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_WIN_MANAGED)
#define Tk_TopWinHierarchy(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_TOP_HIERARCHY)
#define Tk_IsManageable(tkwin) \
    (((Tk_FakeWin *) (tkwin))->flags & TK_WM_MANAGEABLE)
#define Tk_ReqWidth(tkwin)	(((Tk_FakeWin *) (tkwin))->reqWidth)
#define Tk_ReqHeight(tkwin)	(((Tk_FakeWin *) (tkwin))->reqHeight)

#define Tk_InternalBorderWidth(tkwin) \
    (((Tk_FakeWin *) (tkwin))->internalBorderLeft)
#define Tk_InternalBorderLeft(tkwin) \
    (((Tk_FakeWin *) (tkwin))->internalBorderLeft)
#define Tk_InternalBorderRight(tkwin) \
    (((Tk_FakeWin *) (tkwin))->internalBorderRight)
#define Tk_InternalBorderTop(tkwin) \
    (((Tk_FakeWin *) (tkwin))->internalBorderTop)
#define Tk_InternalBorderBottom(tkwin) \
    (((Tk_FakeWin *) (tkwin))->internalBorderBottom)
#define Tk_MinReqWidth(tkwin)	(((Tk_FakeWin *) (tkwin))->minReqWidth)
#define Tk_MinReqHeight(tkwin)	(((Tk_FakeWin *) (tkwin))->minReqHeight)
#define Tk_Parent(tkwin)	(((Tk_FakeWin *) (tkwin))->parentPtr)
#define Tk_Colormap(tkwin)	(((Tk_FakeWin *) (tkwin))->atts.colormap)



typedef struct Tk_FakeWin {
    Display *display;
    char *dummy1;		
    int screenNum;
    Visual *visual;
    int depth;
    Window window;
    char *dummy2;		
    char *dummy3;		
    Tk_Window parentPtr;	
    char *dummy4;		
    char *dummy5;		
    char *pathName;
    Tk_Uid nameUid;
    Tk_Uid classUid;
    XWindowChanges changes;
    unsigned int dummy6;	
    XSetWindowAttributes atts;
    unsigned long dummy7;	
    unsigned int flags;
    char *dummy8;		
#ifdef TK_USE_INPUT_METHODS
    XIC dummy9;			
#endif 
    ClientData *dummy10;	
    int dummy11;		
    int dummy12;		
    char *dummy13;		
    char *dummy14;		
    ClientData dummy15;		
    int reqWidth, reqHeight;
    int internalBorderLeft;
    char *dummy16;		
    char *dummy17;		
    ClientData dummy18;		
    char *dummy19;		
    int internalBorderRight;
    int internalBorderTop;
    int internalBorderBottom;
    int minReqWidth;
    int minReqHeight;
} Tk_FakeWin;



#define TK_MAPPED		1
#define TK_TOP_LEVEL		2
#define TK_ALREADY_DEAD		4
#define TK_NEED_CONFIG_NOTIFY	8
#define TK_GRAB_FLAG		0x10
#define TK_CHECKED_IC		0x20
#define TK_DONT_DESTROY_WINDOW	0x40
#define TK_WM_COLORMAP_WINDOW	0x80
#define TK_EMBEDDED		0x100
#define TK_CONTAINER		0x200
#define TK_BOTH_HALVES		0x400
#define TK_DEFER_MODAL		0x800
#define TK_WRAPPER		0x1000
#define TK_REPARENTED		0x2000
#define TK_ANONYMOUS_WINDOW	0x4000
#define TK_HAS_WRAPPER		0x8000
#define TK_WIN_MANAGED		0x10000
#define TK_TOP_HIERARCHY	0x20000
#define TK_PROP_PROPCHANGE	0x40000
#define TK_WM_MANAGEABLE	0x80000



typedef enum {
    TK_STATE_NULL = -1, TK_STATE_ACTIVE, TK_STATE_DISABLED,
    TK_STATE_NORMAL, TK_STATE_HIDDEN
} Tk_State;

typedef struct Tk_SmoothMethod {
    char *name;
    int (*coordProc) _ANSI_ARGS_((Tk_Canvas canvas,
		double *pointPtr, int numPoints, int numSteps,
		XPoint xPoints[], double dblPoints[]));
    void (*postscriptProc) _ANSI_ARGS_((Tcl_Interp *interp,
		Tk_Canvas canvas, double *coordPtr,
		int numPoints, int numSteps));
} Tk_SmoothMethod;



#define TK_TAG_SPACE 3

typedef struct Tk_Item {
    int id;			
    struct Tk_Item *nextPtr;	
    Tk_Uid staticTagSpace[TK_TAG_SPACE];
				
    Tk_Uid *tagPtr;		
    int tagSpace;		
    int numTags;		
    struct Tk_ItemType *typePtr;
    int x1, y1, x2, y2;		
    struct Tk_Item *prevPtr;	
    Tk_State state;		
    char *reserved1;		
    int redraw_flags;		

    
} Tk_Item;



#define TK_ITEM_STATE_DEPENDANT		1
#define TK_ITEM_DONT_REDRAW		2



#ifdef USE_OLD_CANVAS
typedef int	Tk_ItemCreateProc _ANSI_ARGS_((Tcl_Interp *interp,
		    Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
		    char **argv));
typedef int	Tk_ItemConfigureProc _ANSI_ARGS_((Tcl_Interp *interp,
		    Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
		    char **argv, int flags));
typedef int	Tk_ItemCoordProc _ANSI_ARGS_((Tcl_Interp *interp,
		    Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
		    char **argv));
#else
typedef int	Tk_ItemCreateProc _ANSI_ARGS_((Tcl_Interp *interp,
		    Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
		    Tcl_Obj *const objv[]));
typedef int	Tk_ItemConfigureProc _ANSI_ARGS_((Tcl_Interp *interp,
		    Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
		    Tcl_Obj *const objv[], int flags));
typedef int	Tk_ItemCoordProc _ANSI_ARGS_((Tcl_Interp *interp,
		    Tk_Canvas canvas, Tk_Item *itemPtr, int argc,
		    Tcl_Obj *const argv[]));
#endif
typedef void	Tk_ItemDeleteProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, Display *display));
typedef void	Tk_ItemDisplayProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, Display *display, Drawable dst,
		    int x, int y, int width, int height));
typedef double	Tk_ItemPointProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, double *pointPtr));
typedef int	Tk_ItemAreaProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, double *rectPtr));
typedef int	Tk_ItemPostscriptProc _ANSI_ARGS_((Tcl_Interp *interp,
		    Tk_Canvas canvas, Tk_Item *itemPtr, int prepass));
typedef void	Tk_ItemScaleProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, double originX, double originY,
		    double scaleX, double scaleY));
typedef void	Tk_ItemTranslateProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, double deltaX, double deltaY));
typedef int	Tk_ItemIndexProc _ANSI_ARGS_((Tcl_Interp *interp,
		    Tk_Canvas canvas, Tk_Item *itemPtr, char *indexString,
		    int *indexPtr));
typedef void	Tk_ItemCursorProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, int index));
typedef int	Tk_ItemSelectionProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, int offset, char *buffer,
		    int maxBytes));
typedef void	Tk_ItemInsertProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, int beforeThis, char *string));
typedef void	Tk_ItemDCharsProc _ANSI_ARGS_((Tk_Canvas canvas,
		    Tk_Item *itemPtr, int first, int last));

#ifndef __NO_OLD_CONFIG

typedef struct Tk_ItemType {
    char *name;			
    int itemSize;		
    Tk_ItemCreateProc *createProc;
				
    Tk_ConfigSpec *configSpecs;	
    Tk_ItemConfigureProc *configProc;
				
    Tk_ItemCoordProc *coordProc;
    Tk_ItemDeleteProc *deleteProc;
				
    Tk_ItemDisplayProc *displayProc;
				
    int alwaysRedraw;		
    Tk_ItemPointProc *pointProc;
    Tk_ItemAreaProc *areaProc;	
    Tk_ItemPostscriptProc *postscriptProc;
				
    Tk_ItemScaleProc *scaleProc;
    Tk_ItemTranslateProc *translateProc;
				
    Tk_ItemIndexProc *indexProc;
    Tk_ItemCursorProc *icursorProc;
				
    Tk_ItemSelectionProc *selectionProc;
				
    Tk_ItemInsertProc *insertProc;
				
    Tk_ItemDCharsProc *dCharsProc;
				
    struct Tk_ItemType *nextPtr;
    char *reserved1;		
    int reserved2;		
    char *reserved3;		
    char *reserved4;
} Tk_ItemType;

#endif



typedef struct Tk_CanvasTextInfo {
    Tk_3DBorder selBorder;	
    int selBorderWidth;		
    XColor *selFgColorPtr;	
    Tk_Item *selItemPtr;	
    int selectFirst;		
    int selectLast;		
    Tk_Item *anchorItemPtr;	
    int selectAnchor;		
    Tk_3DBorder insertBorder;	
    int insertWidth;		
    int insertBorderWidth;	
    Tk_Item *focusItemPtr;	
    int gotFocus;		
    int cursorOn;		
} Tk_CanvasTextInfo;



typedef struct Tk_Dash {
    int number;
    union {
	char *pt;
	char array[sizeof(char *)];
    } pattern;
} Tk_Dash;

typedef struct Tk_TSOffset {
    int flags;			
    int xoffset;		
    int yoffset;		
} Tk_TSOffset;



#define TK_OFFSET_INDEX		1
#define TK_OFFSET_RELATIVE	2
#define TK_OFFSET_LEFT		4
#define TK_OFFSET_CENTER	8
#define TK_OFFSET_RIGHT		16
#define TK_OFFSET_TOP		32
#define TK_OFFSET_MIDDLE	64
#define TK_OFFSET_BOTTOM	128

typedef struct Tk_Outline {
    GC gc;			
    double width;		
    double activeWidth;		
    double disabledWidth;	
    int offset;			
    Tk_Dash dash;		
    Tk_Dash activeDash;		
    Tk_Dash disabledDash;	
    VOID *reserved1;		
    VOID *reserved2;
    VOID *reserved3;
    Tk_TSOffset tsoffset;	
    XColor *color;		
    XColor *activeColor;	
    XColor *disabledColor;	
    Pixmap stipple;		
    Pixmap activeStipple;	
    Pixmap disabledStipple;	
} Tk_Outline;



typedef struct Tk_ImageType Tk_ImageType;
#ifdef USE_OLD_IMAGE
typedef int (Tk_ImageCreateProc) _ANSI_ARGS_((Tcl_Interp *interp,
	char *name, int argc, char **argv, Tk_ImageType *typePtr,
	Tk_ImageMaster master, ClientData *masterDataPtr));
#else
typedef int (Tk_ImageCreateProc) _ANSI_ARGS_((Tcl_Interp *interp,
	char *name, int objc, Tcl_Obj *const objv[], Tk_ImageType *typePtr,
	Tk_ImageMaster master, ClientData *masterDataPtr));
#endif
typedef ClientData (Tk_ImageGetProc) _ANSI_ARGS_((Tk_Window tkwin,
	ClientData masterData));
typedef void (Tk_ImageDisplayProc) _ANSI_ARGS_((ClientData instanceData,
	Display *display, Drawable drawable, int imageX, int imageY,
	int width, int height, int drawableX, int drawableY));
typedef void (Tk_ImageFreeProc) _ANSI_ARGS_((ClientData instanceData,
	Display *display));
typedef void (Tk_ImageDeleteProc) _ANSI_ARGS_((ClientData masterData));
typedef void (Tk_ImageChangedProc) _ANSI_ARGS_((ClientData clientData,
	int x, int y, int width, int height, int imageWidth,
	int imageHeight));
typedef int (Tk_ImagePostscriptProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, Tk_Window tkwin, Tk_PostscriptInfo psinfo,
	int x, int y, int width, int height, int prepass));



struct Tk_ImageType {
    char *name;			
    Tk_ImageCreateProc *createProc;
				
    Tk_ImageGetProc *getProc;	
    Tk_ImageDisplayProc *displayProc;
				
    Tk_ImageFreeProc *freeProc;	
    Tk_ImageDeleteProc *deleteProc;
				
    Tk_ImagePostscriptProc *postscriptProc;
				
    struct Tk_ImageType *nextPtr;
				
    char *reserved;		
};





typedef void *Tk_PhotoHandle;



typedef struct Tk_PhotoImageBlock {
    unsigned char *pixelPtr;	
    int width;			
    int height;			
    int pitch;			
    int pixelSize;		
    int offset[4];		
} Tk_PhotoImageBlock;



#define TK_PHOTO_COMPOSITE_OVERLAY	0
#define TK_PHOTO_COMPOSITE_SET		1



typedef struct Tk_PhotoImageFormat Tk_PhotoImageFormat;
#ifdef USE_OLD_IMAGE
typedef int (Tk_ImageFileMatchProc) _ANSI_ARGS_((Tcl_Channel chan,
	char *fileName, char *formatString, int *widthPtr, int *heightPtr));
typedef int (Tk_ImageStringMatchProc) _ANSI_ARGS_((char *string,
	char *formatString, int *widthPtr, int *heightPtr));
typedef int (Tk_ImageFileReadProc) _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Channel chan, char *fileName, char *formatString,
	Tk_PhotoHandle imageHandle, int destX, int destY,
	int width, int height, int srcX, int srcY));
typedef int (Tk_ImageStringReadProc) _ANSI_ARGS_((Tcl_Interp *interp,
	char *string, char *formatString, Tk_PhotoHandle imageHandle,
	int destX, int destY, int width, int height, int srcX, int srcY));
typedef int (Tk_ImageFileWriteProc) _ANSI_ARGS_((Tcl_Interp *interp,
	char *fileName, char *formatString, Tk_PhotoImageBlock *blockPtr));
typedef int (Tk_ImageStringWriteProc) _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_DString *dataPtr, char *formatString,
	Tk_PhotoImageBlock *blockPtr));
#else
typedef int (Tk_ImageFileMatchProc) _ANSI_ARGS_((Tcl_Channel chan,
	const char *fileName, Tcl_Obj *format, int *widthPtr,
	int *heightPtr, Tcl_Interp *interp));
typedef int (Tk_ImageStringMatchProc) _ANSI_ARGS_((Tcl_Obj *dataObj,
	Tcl_Obj *format, int *widthPtr, int *heightPtr,
	Tcl_Interp *interp));
typedef int (Tk_ImageFileReadProc) _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Channel chan, const char *fileName, Tcl_Obj *format,
	Tk_PhotoHandle imageHandle, int destX, int destY,
	int width, int height, int srcX, int srcY));
typedef int (Tk_ImageStringReadProc) _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *dataObj, Tcl_Obj *format, Tk_PhotoHandle imageHandle,
	int destX, int destY, int width, int height, int srcX, int srcY));
typedef int (Tk_ImageFileWriteProc) _ANSI_ARGS_((Tcl_Interp *interp,
	const char *fileName, Tcl_Obj *format, Tk_PhotoImageBlock *blockPtr));
typedef int (Tk_ImageStringWriteProc) _ANSI_ARGS_((Tcl_Interp *interp,
	Tcl_Obj *format, Tk_PhotoImageBlock *blockPtr));
#endif



struct Tk_PhotoImageFormat {
    char *name;			
    Tk_ImageFileMatchProc *fileMatchProc;
				
    Tk_ImageStringMatchProc *stringMatchProc;
				
    Tk_ImageFileReadProc *fileReadProc;
				
    Tk_ImageStringReadProc *stringReadProc;
				
    Tk_ImageFileWriteProc *fileWriteProc;
				
    Tk_ImageStringWriteProc *stringWriteProc;
				
    struct Tk_PhotoImageFormat *nextPtr;
				
};





#define TK_STYLE_VERSION_1      0x1
#define TK_STYLE_VERSION        TK_STYLE_VERSION_1



typedef void (Tk_GetElementSizeProc) _ANSI_ARGS_((ClientData clientData,
        char *recordPtr, const Tk_OptionSpec **optionsPtr, Tk_Window tkwin,
        int width, int height, int inner, int *widthPtr, int *heightPtr));
typedef void (Tk_GetElementBoxProc) _ANSI_ARGS_((ClientData clientData,
        char *recordPtr, const Tk_OptionSpec **optionsPtr, Tk_Window tkwin,
        int x, int y, int width, int height, int inner, int *xPtr, int *yPtr,
        int *widthPtr, int *heightPtr));
typedef int (Tk_GetElementBorderWidthProc) _ANSI_ARGS_((ClientData clientData,
        char *recordPtr, const Tk_OptionSpec **optionsPtr, Tk_Window tkwin));
typedef void (Tk_DrawElementProc) _ANSI_ARGS_((ClientData clientData,
        char *recordPtr, const Tk_OptionSpec **optionsPtr, Tk_Window tkwin,
        Drawable d, int x, int y, int width, int height, int state));

typedef struct Tk_ElementOptionSpec {
    char *name;                 
    Tk_OptionType type;         
} Tk_ElementOptionSpec;

typedef struct Tk_ElementSpec {
    int version;                
    char *name;                 
    Tk_ElementOptionSpec *options;
                                
    Tk_GetElementSizeProc *getSize;
                                
    Tk_GetElementBoxProc *getBox;
                                
    Tk_GetElementBorderWidthProc *getBorderWidth;
                                
    Tk_DrawElementProc *draw;	
} Tk_ElementSpec;



#define TK_ELEMENT_STATE_ACTIVE         1<<0
#define TK_ELEMENT_STATE_DISABLED       1<<1
#define TK_ELEMENT_STATE_FOCUS          1<<2
#define TK_ELEMENT_STATE_PRESSED        1<<3



#define TK_READABLE		TCL_READABLE
#define TK_WRITABLE		TCL_WRITABLE
#define TK_EXCEPTION		TCL_EXCEPTION

#define TK_DONT_WAIT		TCL_DONT_WAIT
#define TK_X_EVENTS		TCL_WINDOW_EVENTS
#define TK_WINDOW_EVENTS	TCL_WINDOW_EVENTS
#define TK_FILE_EVENTS		TCL_FILE_EVENTS
#define TK_TIMER_EVENTS		TCL_TIMER_EVENTS
#define TK_IDLE_EVENTS		TCL_IDLE_EVENTS
#define TK_ALL_EVENTS		TCL_ALL_EVENTS

#define Tk_IdleProc		Tcl_IdleProc
#define Tk_FileProc		Tcl_FileProc
#define Tk_TimerProc		Tcl_TimerProc
#define Tk_TimerToken		Tcl_TimerToken

#define Tk_BackgroundError	Tcl_BackgroundError
#define Tk_CancelIdleCall	Tcl_CancelIdleCall
#define Tk_CreateFileHandler	Tcl_CreateFileHandler
#define Tk_CreateTimerHandler	Tcl_CreateTimerHandler
#define Tk_DeleteFileHandler	Tcl_DeleteFileHandler
#define Tk_DeleteTimerHandler	Tcl_DeleteTimerHandler
#define Tk_DoOneEvent		Tcl_DoOneEvent
#define Tk_DoWhenIdle		Tcl_DoWhenIdle
#define Tk_Sleep		Tcl_Sleep



#define Tk_EventuallyFree	Tcl_EventuallyFree
#define Tk_FreeProc		Tcl_FreeProc
#define Tk_Preserve		Tcl_Preserve
#define Tk_Release		Tcl_Release


#define Tk_Main(argc, argv, proc) \
    Tk_MainEx(argc, argv, proc, Tcl_CreateInterp())

const char *		Tk_InitStubs _ANSI_ARGS_((Tcl_Interp *interp,
			    const char *version, int exact));
EXTERN const char *	Tk_PkgInitStubsCheck _ANSI_ARGS_((Tcl_Interp *interp,
			    const char *version, int exact));

#ifndef USE_TK_STUBS
#define Tk_InitStubs(interp, version, exact) \
    Tk_PkgInitStubsCheck(interp, version, exact)
#endif 

#define Tk_InitImageArgs(interp, argc, argv) 



typedef int (Tk_ErrorProc) _ANSI_ARGS_((ClientData clientData,
	XErrorEvent *errEventPtr));
typedef void (Tk_EventProc) _ANSI_ARGS_((ClientData clientData,
	XEvent *eventPtr));
typedef int (Tk_GenericProc) _ANSI_ARGS_((ClientData clientData,
	XEvent *eventPtr));
typedef int (Tk_ClientMessageProc) _ANSI_ARGS_((Tk_Window tkwin,
	XEvent *eventPtr));
typedef int (Tk_GetSelProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, char *portion));
typedef void (Tk_LostSelProc) _ANSI_ARGS_((ClientData clientData));
typedef Tk_RestrictAction (Tk_RestrictProc) _ANSI_ARGS_((
	ClientData clientData, XEvent *eventPtr));
typedef int (Tk_SelectionProc) _ANSI_ARGS_((ClientData clientData,
	int offset, char *buffer, int maxBytes));



#include "tkDecls.h"

#ifdef USE_OLD_IMAGE
#undef Tk_CreateImageType
#define Tk_CreateImageType		Tk_CreateOldImageType
#undef Tk_CreatePhotoImageFormat
#define Tk_CreatePhotoImageFormat	Tk_CreateOldPhotoImageFormat
#endif 



#ifdef USE_COMPOSITELESS_PHOTO_PUT_BLOCK
#   ifdef Tk_PhotoPutBlock
#	undef Tk_PhotoPutBlock
#   endif
#   define Tk_PhotoPutBlock		Tk_PhotoPutBlock_NoComposite
#   ifdef Tk_PhotoPutZoomedBlock
#	undef Tk_PhotoPutZoomedBlock
#   endif
#   define Tk_PhotoPutZoomedBlock	Tk_PhotoPutZoomedBlock_NoComposite
#   define USE_PANIC_ON_PHOTO_ALLOC_FAILURE
#else 
#   ifdef USE_PANIC_ON_PHOTO_ALLOC_FAILURE
#	ifdef Tk_PhotoPutBlock
#	    undef Tk_PhotoPutBlock
#	endif
#	define Tk_PhotoPutBlock		Tk_PhotoPutBlock_Panic
#	ifdef Tk_PhotoPutZoomedBlock
#	    undef Tk_PhotoPutZoomedBlock
#	endif
#	define Tk_PhotoPutZoomedBlock	Tk_PhotoPutZoomedBlock_Panic
#   endif 
#endif 
#ifdef USE_PANIC_ON_PHOTO_ALLOC_FAILURE
#   ifdef Tk_PhotoExpand
#	undef Tk_PhotoExpand
#   endif
#   define Tk_PhotoExpand		Tk_PhotoExpand_Panic
#   ifdef Tk_PhotoSetSize
#	undef Tk_PhotoSetSize
#   endif
#   define Tk_PhotoSetSize		Tk_PhotoSetSize_Panic
#endif 

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT

#endif 



#ifdef __cplusplus
}
#endif

#endif 


