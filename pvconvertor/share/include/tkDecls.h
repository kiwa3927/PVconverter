

#ifndef _TKDECLS
#define _TKDECLS

#ifdef BUILD_tk
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#endif





#ifdef __cplusplus
extern "C" {
#endif



#ifndef Tk_MainLoop_TCL_DECLARED
#define Tk_MainLoop_TCL_DECLARED

EXTERN void		Tk_MainLoop(void);
#endif
#ifndef Tk_3DBorderColor_TCL_DECLARED
#define Tk_3DBorderColor_TCL_DECLARED

EXTERN XColor *		Tk_3DBorderColor(Tk_3DBorder border);
#endif
#ifndef Tk_3DBorderGC_TCL_DECLARED
#define Tk_3DBorderGC_TCL_DECLARED

EXTERN GC		Tk_3DBorderGC(Tk_Window tkwin, Tk_3DBorder border,
				int which);
#endif
#ifndef Tk_3DHorizontalBevel_TCL_DECLARED
#define Tk_3DHorizontalBevel_TCL_DECLARED

EXTERN void		Tk_3DHorizontalBevel(Tk_Window tkwin,
				Drawable drawable, Tk_3DBorder border, int x,
				int y, int width, int height, int leftIn,
				int rightIn, int topBevel, int relief);
#endif
#ifndef Tk_3DVerticalBevel_TCL_DECLARED
#define Tk_3DVerticalBevel_TCL_DECLARED

EXTERN void		Tk_3DVerticalBevel(Tk_Window tkwin,
				Drawable drawable, Tk_3DBorder border, int x,
				int y, int width, int height, int leftBevel,
				int relief);
#endif
#ifndef Tk_AddOption_TCL_DECLARED
#define Tk_AddOption_TCL_DECLARED

EXTERN void		Tk_AddOption(Tk_Window tkwin, CONST char *name,
				CONST char *value, int priority);
#endif
#ifndef Tk_BindEvent_TCL_DECLARED
#define Tk_BindEvent_TCL_DECLARED

EXTERN void		Tk_BindEvent(Tk_BindingTable bindingTable,
				XEvent *eventPtr, Tk_Window tkwin,
				int numObjects, ClientData *objectPtr);
#endif
#ifndef Tk_CanvasDrawableCoords_TCL_DECLARED
#define Tk_CanvasDrawableCoords_TCL_DECLARED

EXTERN void		Tk_CanvasDrawableCoords(Tk_Canvas canvas, double x,
				double y, short *drawableXPtr,
				short *drawableYPtr);
#endif
#ifndef Tk_CanvasEventuallyRedraw_TCL_DECLARED
#define Tk_CanvasEventuallyRedraw_TCL_DECLARED

EXTERN void		Tk_CanvasEventuallyRedraw(Tk_Canvas canvas, int x1,
				int y1, int x2, int y2);
#endif
#ifndef Tk_CanvasGetCoord_TCL_DECLARED
#define Tk_CanvasGetCoord_TCL_DECLARED

EXTERN int		Tk_CanvasGetCoord(Tcl_Interp *interp,
				Tk_Canvas canvas, CONST char *str,
				double *doublePtr);
#endif
#ifndef Tk_CanvasGetTextInfo_TCL_DECLARED
#define Tk_CanvasGetTextInfo_TCL_DECLARED

EXTERN Tk_CanvasTextInfo * Tk_CanvasGetTextInfo(Tk_Canvas canvas);
#endif
#ifndef Tk_CanvasPsBitmap_TCL_DECLARED
#define Tk_CanvasPsBitmap_TCL_DECLARED

EXTERN int		Tk_CanvasPsBitmap(Tcl_Interp *interp,
				Tk_Canvas canvas, Pixmap bitmap, int x,
				int y, int width, int height);
#endif
#ifndef Tk_CanvasPsColor_TCL_DECLARED
#define Tk_CanvasPsColor_TCL_DECLARED

EXTERN int		Tk_CanvasPsColor(Tcl_Interp *interp,
				Tk_Canvas canvas, XColor *colorPtr);
#endif
#ifndef Tk_CanvasPsFont_TCL_DECLARED
#define Tk_CanvasPsFont_TCL_DECLARED

EXTERN int		Tk_CanvasPsFont(Tcl_Interp *interp, Tk_Canvas canvas,
				Tk_Font font);
#endif
#ifndef Tk_CanvasPsPath_TCL_DECLARED
#define Tk_CanvasPsPath_TCL_DECLARED

EXTERN void		Tk_CanvasPsPath(Tcl_Interp *interp, Tk_Canvas canvas,
				double *coordPtr, int numPoints);
#endif
#ifndef Tk_CanvasPsStipple_TCL_DECLARED
#define Tk_CanvasPsStipple_TCL_DECLARED

EXTERN int		Tk_CanvasPsStipple(Tcl_Interp *interp,
				Tk_Canvas canvas, Pixmap bitmap);
#endif
#ifndef Tk_CanvasPsY_TCL_DECLARED
#define Tk_CanvasPsY_TCL_DECLARED

EXTERN double		Tk_CanvasPsY(Tk_Canvas canvas, double y);
#endif
#ifndef Tk_CanvasSetStippleOrigin_TCL_DECLARED
#define Tk_CanvasSetStippleOrigin_TCL_DECLARED

EXTERN void		Tk_CanvasSetStippleOrigin(Tk_Canvas canvas, GC gc);
#endif
#ifndef Tk_CanvasTagsParseProc_TCL_DECLARED
#define Tk_CanvasTagsParseProc_TCL_DECLARED

EXTERN int		Tk_CanvasTagsParseProc(ClientData clientData,
				Tcl_Interp *interp, Tk_Window tkwin,
				CONST char *value, char *widgRec, int offset);
#endif
#ifndef Tk_CanvasTagsPrintProc_TCL_DECLARED
#define Tk_CanvasTagsPrintProc_TCL_DECLARED

EXTERN char *		Tk_CanvasTagsPrintProc(ClientData clientData,
				Tk_Window tkwin, char *widgRec, int offset,
				Tcl_FreeProc **freeProcPtr);
#endif
#ifndef Tk_CanvasTkwin_TCL_DECLARED
#define Tk_CanvasTkwin_TCL_DECLARED

EXTERN Tk_Window	Tk_CanvasTkwin(Tk_Canvas canvas);
#endif
#ifndef Tk_CanvasWindowCoords_TCL_DECLARED
#define Tk_CanvasWindowCoords_TCL_DECLARED

EXTERN void		Tk_CanvasWindowCoords(Tk_Canvas canvas, double x,
				double y, short *screenXPtr,
				short *screenYPtr);
#endif
#ifndef Tk_ChangeWindowAttributes_TCL_DECLARED
#define Tk_ChangeWindowAttributes_TCL_DECLARED

EXTERN void		Tk_ChangeWindowAttributes(Tk_Window tkwin,
				unsigned long valueMask,
				XSetWindowAttributes *attsPtr);
#endif
#ifndef Tk_CharBbox_TCL_DECLARED
#define Tk_CharBbox_TCL_DECLARED

EXTERN int		Tk_CharBbox(Tk_TextLayout layout, int index,
				int *xPtr, int *yPtr, int *widthPtr,
				int *heightPtr);
#endif
#ifndef Tk_ClearSelection_TCL_DECLARED
#define Tk_ClearSelection_TCL_DECLARED

EXTERN void		Tk_ClearSelection(Tk_Window tkwin, Atom selection);
#endif
#ifndef Tk_ClipboardAppend_TCL_DECLARED
#define Tk_ClipboardAppend_TCL_DECLARED

EXTERN int		Tk_ClipboardAppend(Tcl_Interp *interp,
				Tk_Window tkwin, Atom target, Atom format,
				char *buffer);
#endif
#ifndef Tk_ClipboardClear_TCL_DECLARED
#define Tk_ClipboardClear_TCL_DECLARED

EXTERN int		Tk_ClipboardClear(Tcl_Interp *interp,
				Tk_Window tkwin);
#endif
#ifndef Tk_ConfigureInfo_TCL_DECLARED
#define Tk_ConfigureInfo_TCL_DECLARED

EXTERN int		Tk_ConfigureInfo(Tcl_Interp *interp, Tk_Window tkwin,
				Tk_ConfigSpec *specs, char *widgRec,
				CONST char *argvName, int flags);
#endif
#ifndef Tk_ConfigureValue_TCL_DECLARED
#define Tk_ConfigureValue_TCL_DECLARED

EXTERN int		Tk_ConfigureValue(Tcl_Interp *interp,
				Tk_Window tkwin, Tk_ConfigSpec *specs,
				char *widgRec, CONST char *argvName,
				int flags);
#endif
#ifndef Tk_ConfigureWidget_TCL_DECLARED
#define Tk_ConfigureWidget_TCL_DECLARED

EXTERN int		Tk_ConfigureWidget(Tcl_Interp *interp,
				Tk_Window tkwin, Tk_ConfigSpec *specs,
				int argc, CONST84 char **argv, char *widgRec,
				int flags);
#endif
#ifndef Tk_ConfigureWindow_TCL_DECLARED
#define Tk_ConfigureWindow_TCL_DECLARED

EXTERN void		Tk_ConfigureWindow(Tk_Window tkwin,
				unsigned int valueMask,
				XWindowChanges *valuePtr);
#endif
#ifndef Tk_ComputeTextLayout_TCL_DECLARED
#define Tk_ComputeTextLayout_TCL_DECLARED

EXTERN Tk_TextLayout	Tk_ComputeTextLayout(Tk_Font font, CONST char *str,
				int numChars, int wrapLength,
				Tk_Justify justify, int flags, int *widthPtr,
				int *heightPtr);
#endif
#ifndef Tk_CoordsToWindow_TCL_DECLARED
#define Tk_CoordsToWindow_TCL_DECLARED

EXTERN Tk_Window	Tk_CoordsToWindow(int rootX, int rootY,
				Tk_Window tkwin);
#endif
#ifndef Tk_CreateBinding_TCL_DECLARED
#define Tk_CreateBinding_TCL_DECLARED

EXTERN unsigned long	Tk_CreateBinding(Tcl_Interp *interp,
				Tk_BindingTable bindingTable,
				ClientData object, CONST char *eventStr,
				CONST char *command, int append);
#endif
#ifndef Tk_CreateBindingTable_TCL_DECLARED
#define Tk_CreateBindingTable_TCL_DECLARED

EXTERN Tk_BindingTable	Tk_CreateBindingTable(Tcl_Interp *interp);
#endif
#ifndef Tk_CreateErrorHandler_TCL_DECLARED
#define Tk_CreateErrorHandler_TCL_DECLARED

EXTERN Tk_ErrorHandler	Tk_CreateErrorHandler(Display *display, int errNum,
				int request, int minorCode,
				Tk_ErrorProc *errorProc,
				ClientData clientData);
#endif
#ifndef Tk_CreateEventHandler_TCL_DECLARED
#define Tk_CreateEventHandler_TCL_DECLARED

EXTERN void		Tk_CreateEventHandler(Tk_Window token,
				unsigned long mask, Tk_EventProc *proc,
				ClientData clientData);
#endif
#ifndef Tk_CreateGenericHandler_TCL_DECLARED
#define Tk_CreateGenericHandler_TCL_DECLARED

EXTERN void		Tk_CreateGenericHandler(Tk_GenericProc *proc,
				ClientData clientData);
#endif
#ifndef Tk_CreateImageType_TCL_DECLARED
#define Tk_CreateImageType_TCL_DECLARED

EXTERN void		Tk_CreateImageType(Tk_ImageType *typePtr);
#endif
#ifndef Tk_CreateItemType_TCL_DECLARED
#define Tk_CreateItemType_TCL_DECLARED

EXTERN void		Tk_CreateItemType(Tk_ItemType *typePtr);
#endif
#ifndef Tk_CreatePhotoImageFormat_TCL_DECLARED
#define Tk_CreatePhotoImageFormat_TCL_DECLARED

EXTERN void		Tk_CreatePhotoImageFormat(
				Tk_PhotoImageFormat *formatPtr);
#endif
#ifndef Tk_CreateSelHandler_TCL_DECLARED
#define Tk_CreateSelHandler_TCL_DECLARED

EXTERN void		Tk_CreateSelHandler(Tk_Window tkwin, Atom selection,
				Atom target, Tk_SelectionProc *proc,
				ClientData clientData, Atom format);
#endif
#ifndef Tk_CreateWindow_TCL_DECLARED
#define Tk_CreateWindow_TCL_DECLARED

EXTERN Tk_Window	Tk_CreateWindow(Tcl_Interp *interp, Tk_Window parent,
				CONST char *name, CONST char *screenName);
#endif
#ifndef Tk_CreateWindowFromPath_TCL_DECLARED
#define Tk_CreateWindowFromPath_TCL_DECLARED

EXTERN Tk_Window	Tk_CreateWindowFromPath(Tcl_Interp *interp,
				Tk_Window tkwin, CONST char *pathName,
				CONST char *screenName);
#endif
#ifndef Tk_DefineBitmap_TCL_DECLARED
#define Tk_DefineBitmap_TCL_DECLARED

EXTERN int		Tk_DefineBitmap(Tcl_Interp *interp, CONST char *name,
				CONST char *source, int width, int height);
#endif
#ifndef Tk_DefineCursor_TCL_DECLARED
#define Tk_DefineCursor_TCL_DECLARED

EXTERN void		Tk_DefineCursor(Tk_Window window, Tk_Cursor cursor);
#endif
#ifndef Tk_DeleteAllBindings_TCL_DECLARED
#define Tk_DeleteAllBindings_TCL_DECLARED

EXTERN void		Tk_DeleteAllBindings(Tk_BindingTable bindingTable,
				ClientData object);
#endif
#ifndef Tk_DeleteBinding_TCL_DECLARED
#define Tk_DeleteBinding_TCL_DECLARED

EXTERN int		Tk_DeleteBinding(Tcl_Interp *interp,
				Tk_BindingTable bindingTable,
				ClientData object, CONST char *eventStr);
#endif
#ifndef Tk_DeleteBindingTable_TCL_DECLARED
#define Tk_DeleteBindingTable_TCL_DECLARED

EXTERN void		Tk_DeleteBindingTable(Tk_BindingTable bindingTable);
#endif
#ifndef Tk_DeleteErrorHandler_TCL_DECLARED
#define Tk_DeleteErrorHandler_TCL_DECLARED

EXTERN void		Tk_DeleteErrorHandler(Tk_ErrorHandler handler);
#endif
#ifndef Tk_DeleteEventHandler_TCL_DECLARED
#define Tk_DeleteEventHandler_TCL_DECLARED

EXTERN void		Tk_DeleteEventHandler(Tk_Window token,
				unsigned long mask, Tk_EventProc *proc,
				ClientData clientData);
#endif
#ifndef Tk_DeleteGenericHandler_TCL_DECLARED
#define Tk_DeleteGenericHandler_TCL_DECLARED

EXTERN void		Tk_DeleteGenericHandler(Tk_GenericProc *proc,
				ClientData clientData);
#endif
#ifndef Tk_DeleteImage_TCL_DECLARED
#define Tk_DeleteImage_TCL_DECLARED

EXTERN void		Tk_DeleteImage(Tcl_Interp *interp, CONST char *name);
#endif
#ifndef Tk_DeleteSelHandler_TCL_DECLARED
#define Tk_DeleteSelHandler_TCL_DECLARED

EXTERN void		Tk_DeleteSelHandler(Tk_Window tkwin, Atom selection,
				Atom target);
#endif
#ifndef Tk_DestroyWindow_TCL_DECLARED
#define Tk_DestroyWindow_TCL_DECLARED

EXTERN void		Tk_DestroyWindow(Tk_Window tkwin);
#endif
#ifndef Tk_DisplayName_TCL_DECLARED
#define Tk_DisplayName_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_DisplayName(Tk_Window tkwin);
#endif
#ifndef Tk_DistanceToTextLayout_TCL_DECLARED
#define Tk_DistanceToTextLayout_TCL_DECLARED

EXTERN int		Tk_DistanceToTextLayout(Tk_TextLayout layout, int x,
				int y);
#endif
#ifndef Tk_Draw3DPolygon_TCL_DECLARED
#define Tk_Draw3DPolygon_TCL_DECLARED

EXTERN void		Tk_Draw3DPolygon(Tk_Window tkwin, Drawable drawable,
				Tk_3DBorder border, XPoint *pointPtr,
				int numPoints, int borderWidth,
				int leftRelief);
#endif
#ifndef Tk_Draw3DRectangle_TCL_DECLARED
#define Tk_Draw3DRectangle_TCL_DECLARED

EXTERN void		Tk_Draw3DRectangle(Tk_Window tkwin,
				Drawable drawable, Tk_3DBorder border, int x,
				int y, int width, int height,
				int borderWidth, int relief);
#endif
#ifndef Tk_DrawChars_TCL_DECLARED
#define Tk_DrawChars_TCL_DECLARED

EXTERN void		Tk_DrawChars(Display *display, Drawable drawable,
				GC gc, Tk_Font tkfont, CONST char *source,
				int numBytes, int x, int y);
#endif
#ifndef Tk_DrawFocusHighlight_TCL_DECLARED
#define Tk_DrawFocusHighlight_TCL_DECLARED

EXTERN void		Tk_DrawFocusHighlight(Tk_Window tkwin, GC gc,
				int width, Drawable drawable);
#endif
#ifndef Tk_DrawTextLayout_TCL_DECLARED
#define Tk_DrawTextLayout_TCL_DECLARED

EXTERN void		Tk_DrawTextLayout(Display *display,
				Drawable drawable, GC gc,
				Tk_TextLayout layout, int x, int y,
				int firstChar, int lastChar);
#endif
#ifndef Tk_Fill3DPolygon_TCL_DECLARED
#define Tk_Fill3DPolygon_TCL_DECLARED

EXTERN void		Tk_Fill3DPolygon(Tk_Window tkwin, Drawable drawable,
				Tk_3DBorder border, XPoint *pointPtr,
				int numPoints, int borderWidth,
				int leftRelief);
#endif
#ifndef Tk_Fill3DRectangle_TCL_DECLARED
#define Tk_Fill3DRectangle_TCL_DECLARED

EXTERN void		Tk_Fill3DRectangle(Tk_Window tkwin,
				Drawable drawable, Tk_3DBorder border, int x,
				int y, int width, int height,
				int borderWidth, int relief);
#endif
#ifndef Tk_FindPhoto_TCL_DECLARED
#define Tk_FindPhoto_TCL_DECLARED

EXTERN Tk_PhotoHandle	Tk_FindPhoto(Tcl_Interp *interp,
				CONST char *imageName);
#endif
#ifndef Tk_FontId_TCL_DECLARED
#define Tk_FontId_TCL_DECLARED

EXTERN Font		Tk_FontId(Tk_Font font);
#endif
#ifndef Tk_Free3DBorder_TCL_DECLARED
#define Tk_Free3DBorder_TCL_DECLARED

EXTERN void		Tk_Free3DBorder(Tk_3DBorder border);
#endif
#ifndef Tk_FreeBitmap_TCL_DECLARED
#define Tk_FreeBitmap_TCL_DECLARED

EXTERN void		Tk_FreeBitmap(Display *display, Pixmap bitmap);
#endif
#ifndef Tk_FreeColor_TCL_DECLARED
#define Tk_FreeColor_TCL_DECLARED

EXTERN void		Tk_FreeColor(XColor *colorPtr);
#endif
#ifndef Tk_FreeColormap_TCL_DECLARED
#define Tk_FreeColormap_TCL_DECLARED

EXTERN void		Tk_FreeColormap(Display *display, Colormap colormap);
#endif
#ifndef Tk_FreeCursor_TCL_DECLARED
#define Tk_FreeCursor_TCL_DECLARED

EXTERN void		Tk_FreeCursor(Display *display, Tk_Cursor cursor);
#endif
#ifndef Tk_FreeFont_TCL_DECLARED
#define Tk_FreeFont_TCL_DECLARED

EXTERN void		Tk_FreeFont(Tk_Font f);
#endif
#ifndef Tk_FreeGC_TCL_DECLARED
#define Tk_FreeGC_TCL_DECLARED

EXTERN void		Tk_FreeGC(Display *display, GC gc);
#endif
#ifndef Tk_FreeImage_TCL_DECLARED
#define Tk_FreeImage_TCL_DECLARED

EXTERN void		Tk_FreeImage(Tk_Image image);
#endif
#ifndef Tk_FreeOptions_TCL_DECLARED
#define Tk_FreeOptions_TCL_DECLARED

EXTERN void		Tk_FreeOptions(Tk_ConfigSpec *specs, char *widgRec,
				Display *display, int needFlags);
#endif
#ifndef Tk_FreePixmap_TCL_DECLARED
#define Tk_FreePixmap_TCL_DECLARED

EXTERN void		Tk_FreePixmap(Display *display, Pixmap pixmap);
#endif
#ifndef Tk_FreeTextLayout_TCL_DECLARED
#define Tk_FreeTextLayout_TCL_DECLARED

EXTERN void		Tk_FreeTextLayout(Tk_TextLayout textLayout);
#endif
#ifndef Tk_FreeXId_TCL_DECLARED
#define Tk_FreeXId_TCL_DECLARED

EXTERN void		Tk_FreeXId(Display *display, XID xid);
#endif
#ifndef Tk_GCForColor_TCL_DECLARED
#define Tk_GCForColor_TCL_DECLARED

EXTERN GC		Tk_GCForColor(XColor *colorPtr, Drawable drawable);
#endif
#ifndef Tk_GeometryRequest_TCL_DECLARED
#define Tk_GeometryRequest_TCL_DECLARED

EXTERN void		Tk_GeometryRequest(Tk_Window tkwin, int reqWidth,
				int reqHeight);
#endif
#ifndef Tk_Get3DBorder_TCL_DECLARED
#define Tk_Get3DBorder_TCL_DECLARED

EXTERN Tk_3DBorder	Tk_Get3DBorder(Tcl_Interp *interp, Tk_Window tkwin,
				Tk_Uid colorName);
#endif
#ifndef Tk_GetAllBindings_TCL_DECLARED
#define Tk_GetAllBindings_TCL_DECLARED

EXTERN void		Tk_GetAllBindings(Tcl_Interp *interp,
				Tk_BindingTable bindingTable,
				ClientData object);
#endif
#ifndef Tk_GetAnchor_TCL_DECLARED
#define Tk_GetAnchor_TCL_DECLARED

EXTERN int		Tk_GetAnchor(Tcl_Interp *interp, CONST char *str,
				Tk_Anchor *anchorPtr);
#endif
#ifndef Tk_GetAtomName_TCL_DECLARED
#define Tk_GetAtomName_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_GetAtomName(Tk_Window tkwin, Atom atom);
#endif
#ifndef Tk_GetBinding_TCL_DECLARED
#define Tk_GetBinding_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_GetBinding(Tcl_Interp *interp,
				Tk_BindingTable bindingTable,
				ClientData object, CONST char *eventStr);
#endif
#ifndef Tk_GetBitmap_TCL_DECLARED
#define Tk_GetBitmap_TCL_DECLARED

EXTERN Pixmap		Tk_GetBitmap(Tcl_Interp *interp, Tk_Window tkwin,
				CONST char *str);
#endif
#ifndef Tk_GetBitmapFromData_TCL_DECLARED
#define Tk_GetBitmapFromData_TCL_DECLARED

EXTERN Pixmap		Tk_GetBitmapFromData(Tcl_Interp *interp,
				Tk_Window tkwin, CONST char *source,
				int width, int height);
#endif
#ifndef Tk_GetCapStyle_TCL_DECLARED
#define Tk_GetCapStyle_TCL_DECLARED

EXTERN int		Tk_GetCapStyle(Tcl_Interp *interp, CONST char *str,
				int *capPtr);
#endif
#ifndef Tk_GetColor_TCL_DECLARED
#define Tk_GetColor_TCL_DECLARED

EXTERN XColor *		Tk_GetColor(Tcl_Interp *interp, Tk_Window tkwin,
				Tk_Uid name);
#endif
#ifndef Tk_GetColorByValue_TCL_DECLARED
#define Tk_GetColorByValue_TCL_DECLARED

EXTERN XColor *		Tk_GetColorByValue(Tk_Window tkwin, XColor *colorPtr);
#endif
#ifndef Tk_GetColormap_TCL_DECLARED
#define Tk_GetColormap_TCL_DECLARED

EXTERN Colormap		Tk_GetColormap(Tcl_Interp *interp, Tk_Window tkwin,
				CONST char *str);
#endif
#ifndef Tk_GetCursor_TCL_DECLARED
#define Tk_GetCursor_TCL_DECLARED

EXTERN Tk_Cursor	Tk_GetCursor(Tcl_Interp *interp, Tk_Window tkwin,
				Tk_Uid str);
#endif
#ifndef Tk_GetCursorFromData_TCL_DECLARED
#define Tk_GetCursorFromData_TCL_DECLARED

EXTERN Tk_Cursor	Tk_GetCursorFromData(Tcl_Interp *interp,
				Tk_Window tkwin, CONST char *source,
				CONST char *mask, int width, int height,
				int xHot, int yHot, Tk_Uid fg, Tk_Uid bg);
#endif
#ifndef Tk_GetFont_TCL_DECLARED
#define Tk_GetFont_TCL_DECLARED

EXTERN Tk_Font		Tk_GetFont(Tcl_Interp *interp, Tk_Window tkwin,
				CONST char *str);
#endif
#ifndef Tk_GetFontFromObj_TCL_DECLARED
#define Tk_GetFontFromObj_TCL_DECLARED

EXTERN Tk_Font		Tk_GetFontFromObj(Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_GetFontMetrics_TCL_DECLARED
#define Tk_GetFontMetrics_TCL_DECLARED

EXTERN void		Tk_GetFontMetrics(Tk_Font font,
				Tk_FontMetrics *fmPtr);
#endif
#ifndef Tk_GetGC_TCL_DECLARED
#define Tk_GetGC_TCL_DECLARED

EXTERN GC		Tk_GetGC(Tk_Window tkwin, unsigned long valueMask,
				XGCValues *valuePtr);
#endif
#ifndef Tk_GetImage_TCL_DECLARED
#define Tk_GetImage_TCL_DECLARED

EXTERN Tk_Image		Tk_GetImage(Tcl_Interp *interp, Tk_Window tkwin,
				CONST char *name,
				Tk_ImageChangedProc *changeProc,
				ClientData clientData);
#endif
#ifndef Tk_GetImageMasterData_TCL_DECLARED
#define Tk_GetImageMasterData_TCL_DECLARED

EXTERN ClientData	Tk_GetImageMasterData(Tcl_Interp *interp,
				CONST char *name, Tk_ImageType **typePtrPtr);
#endif
#ifndef Tk_GetItemTypes_TCL_DECLARED
#define Tk_GetItemTypes_TCL_DECLARED

EXTERN Tk_ItemType *	Tk_GetItemTypes(void);
#endif
#ifndef Tk_GetJoinStyle_TCL_DECLARED
#define Tk_GetJoinStyle_TCL_DECLARED

EXTERN int		Tk_GetJoinStyle(Tcl_Interp *interp, CONST char *str,
				int *joinPtr);
#endif
#ifndef Tk_GetJustify_TCL_DECLARED
#define Tk_GetJustify_TCL_DECLARED

EXTERN int		Tk_GetJustify(Tcl_Interp *interp, CONST char *str,
				Tk_Justify *justifyPtr);
#endif
#ifndef Tk_GetNumMainWindows_TCL_DECLARED
#define Tk_GetNumMainWindows_TCL_DECLARED

EXTERN int		Tk_GetNumMainWindows(void);
#endif
#ifndef Tk_GetOption_TCL_DECLARED
#define Tk_GetOption_TCL_DECLARED

EXTERN Tk_Uid		Tk_GetOption(Tk_Window tkwin, CONST char *name,
				CONST char *className);
#endif
#ifndef Tk_GetPixels_TCL_DECLARED
#define Tk_GetPixels_TCL_DECLARED

EXTERN int		Tk_GetPixels(Tcl_Interp *interp, Tk_Window tkwin,
				CONST char *str, int *intPtr);
#endif
#ifndef Tk_GetPixmap_TCL_DECLARED
#define Tk_GetPixmap_TCL_DECLARED

EXTERN Pixmap		Tk_GetPixmap(Display *display, Drawable d, int width,
				int height, int depth);
#endif
#ifndef Tk_GetRelief_TCL_DECLARED
#define Tk_GetRelief_TCL_DECLARED

EXTERN int		Tk_GetRelief(Tcl_Interp *interp, CONST char *name,
				int *reliefPtr);
#endif
#ifndef Tk_GetRootCoords_TCL_DECLARED
#define Tk_GetRootCoords_TCL_DECLARED

EXTERN void		Tk_GetRootCoords(Tk_Window tkwin, int *xPtr,
				int *yPtr);
#endif
#ifndef Tk_GetScrollInfo_TCL_DECLARED
#define Tk_GetScrollInfo_TCL_DECLARED

EXTERN int		Tk_GetScrollInfo(Tcl_Interp *interp, int argc,
				CONST84 char **argv, double *dblPtr,
				int *intPtr);
#endif
#ifndef Tk_GetScreenMM_TCL_DECLARED
#define Tk_GetScreenMM_TCL_DECLARED

EXTERN int		Tk_GetScreenMM(Tcl_Interp *interp, Tk_Window tkwin,
				CONST char *str, double *doublePtr);
#endif
#ifndef Tk_GetSelection_TCL_DECLARED
#define Tk_GetSelection_TCL_DECLARED

EXTERN int		Tk_GetSelection(Tcl_Interp *interp, Tk_Window tkwin,
				Atom selection, Atom target,
				Tk_GetSelProc *proc, ClientData clientData);
#endif
#ifndef Tk_GetUid_TCL_DECLARED
#define Tk_GetUid_TCL_DECLARED

EXTERN Tk_Uid		Tk_GetUid(CONST char *str);
#endif
#ifndef Tk_GetVisual_TCL_DECLARED
#define Tk_GetVisual_TCL_DECLARED

EXTERN Visual *		Tk_GetVisual(Tcl_Interp *interp, Tk_Window tkwin,
				CONST char *str, int *depthPtr,
				Colormap *colormapPtr);
#endif
#ifndef Tk_GetVRootGeometry_TCL_DECLARED
#define Tk_GetVRootGeometry_TCL_DECLARED

EXTERN void		Tk_GetVRootGeometry(Tk_Window tkwin, int *xPtr,
				int *yPtr, int *widthPtr, int *heightPtr);
#endif
#ifndef Tk_Grab_TCL_DECLARED
#define Tk_Grab_TCL_DECLARED

EXTERN int		Tk_Grab(Tcl_Interp *interp, Tk_Window tkwin,
				int grabGlobal);
#endif
#ifndef Tk_HandleEvent_TCL_DECLARED
#define Tk_HandleEvent_TCL_DECLARED

EXTERN void		Tk_HandleEvent(XEvent *eventPtr);
#endif
#ifndef Tk_IdToWindow_TCL_DECLARED
#define Tk_IdToWindow_TCL_DECLARED

EXTERN Tk_Window	Tk_IdToWindow(Display *display, Window window);
#endif
#ifndef Tk_ImageChanged_TCL_DECLARED
#define Tk_ImageChanged_TCL_DECLARED

EXTERN void		Tk_ImageChanged(Tk_ImageMaster master, int x, int y,
				int width, int height, int imageWidth,
				int imageHeight);
#endif
#ifndef Tk_Init_TCL_DECLARED
#define Tk_Init_TCL_DECLARED

EXTERN int		Tk_Init(Tcl_Interp *interp);
#endif
#ifndef Tk_InternAtom_TCL_DECLARED
#define Tk_InternAtom_TCL_DECLARED

EXTERN Atom		Tk_InternAtom(Tk_Window tkwin, CONST char *name);
#endif
#ifndef Tk_IntersectTextLayout_TCL_DECLARED
#define Tk_IntersectTextLayout_TCL_DECLARED

EXTERN int		Tk_IntersectTextLayout(Tk_TextLayout layout, int x,
				int y, int width, int height);
#endif
#ifndef Tk_MaintainGeometry_TCL_DECLARED
#define Tk_MaintainGeometry_TCL_DECLARED

EXTERN void		Tk_MaintainGeometry(Tk_Window slave,
				Tk_Window master, int x, int y, int width,
				int height);
#endif
#ifndef Tk_MainWindow_TCL_DECLARED
#define Tk_MainWindow_TCL_DECLARED

EXTERN Tk_Window	Tk_MainWindow(Tcl_Interp *interp);
#endif
#ifndef Tk_MakeWindowExist_TCL_DECLARED
#define Tk_MakeWindowExist_TCL_DECLARED

EXTERN void		Tk_MakeWindowExist(Tk_Window tkwin);
#endif
#ifndef Tk_ManageGeometry_TCL_DECLARED
#define Tk_ManageGeometry_TCL_DECLARED

EXTERN void		Tk_ManageGeometry(Tk_Window tkwin,
				CONST Tk_GeomMgr *mgrPtr,
				ClientData clientData);
#endif
#ifndef Tk_MapWindow_TCL_DECLARED
#define Tk_MapWindow_TCL_DECLARED

EXTERN void		Tk_MapWindow(Tk_Window tkwin);
#endif
#ifndef Tk_MeasureChars_TCL_DECLARED
#define Tk_MeasureChars_TCL_DECLARED

EXTERN int		Tk_MeasureChars(Tk_Font tkfont, CONST char *source,
				int numBytes, int maxPixels, int flags,
				int *lengthPtr);
#endif
#ifndef Tk_MoveResizeWindow_TCL_DECLARED
#define Tk_MoveResizeWindow_TCL_DECLARED

EXTERN void		Tk_MoveResizeWindow(Tk_Window tkwin, int x, int y,
				int width, int height);
#endif
#ifndef Tk_MoveWindow_TCL_DECLARED
#define Tk_MoveWindow_TCL_DECLARED

EXTERN void		Tk_MoveWindow(Tk_Window tkwin, int x, int y);
#endif
#ifndef Tk_MoveToplevelWindow_TCL_DECLARED
#define Tk_MoveToplevelWindow_TCL_DECLARED

EXTERN void		Tk_MoveToplevelWindow(Tk_Window tkwin, int x, int y);
#endif
#ifndef Tk_NameOf3DBorder_TCL_DECLARED
#define Tk_NameOf3DBorder_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOf3DBorder(Tk_3DBorder border);
#endif
#ifndef Tk_NameOfAnchor_TCL_DECLARED
#define Tk_NameOfAnchor_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfAnchor(Tk_Anchor anchor);
#endif
#ifndef Tk_NameOfBitmap_TCL_DECLARED
#define Tk_NameOfBitmap_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfBitmap(Display *display, Pixmap bitmap);
#endif
#ifndef Tk_NameOfCapStyle_TCL_DECLARED
#define Tk_NameOfCapStyle_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfCapStyle(int cap);
#endif
#ifndef Tk_NameOfColor_TCL_DECLARED
#define Tk_NameOfColor_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfColor(XColor *colorPtr);
#endif
#ifndef Tk_NameOfCursor_TCL_DECLARED
#define Tk_NameOfCursor_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfCursor(Display *display,
				Tk_Cursor cursor);
#endif
#ifndef Tk_NameOfFont_TCL_DECLARED
#define Tk_NameOfFont_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfFont(Tk_Font font);
#endif
#ifndef Tk_NameOfImage_TCL_DECLARED
#define Tk_NameOfImage_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfImage(Tk_ImageMaster imageMaster);
#endif
#ifndef Tk_NameOfJoinStyle_TCL_DECLARED
#define Tk_NameOfJoinStyle_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfJoinStyle(int join);
#endif
#ifndef Tk_NameOfJustify_TCL_DECLARED
#define Tk_NameOfJustify_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfJustify(Tk_Justify justify);
#endif
#ifndef Tk_NameOfRelief_TCL_DECLARED
#define Tk_NameOfRelief_TCL_DECLARED

EXTERN CONST84_RETURN char * Tk_NameOfRelief(int relief);
#endif
#ifndef Tk_NameToWindow_TCL_DECLARED
#define Tk_NameToWindow_TCL_DECLARED

EXTERN Tk_Window	Tk_NameToWindow(Tcl_Interp *interp,
				CONST char *pathName, Tk_Window tkwin);
#endif
#ifndef Tk_OwnSelection_TCL_DECLARED
#define Tk_OwnSelection_TCL_DECLARED

EXTERN void		Tk_OwnSelection(Tk_Window tkwin, Atom selection,
				Tk_LostSelProc *proc, ClientData clientData);
#endif
#ifndef Tk_ParseArgv_TCL_DECLARED
#define Tk_ParseArgv_TCL_DECLARED

EXTERN int		Tk_ParseArgv(Tcl_Interp *interp, Tk_Window tkwin,
				int *argcPtr, CONST84 char **argv,
				Tk_ArgvInfo *argTable, int flags);
#endif
#ifndef Tk_PhotoPutBlock_NoComposite_TCL_DECLARED
#define Tk_PhotoPutBlock_NoComposite_TCL_DECLARED

EXTERN void		Tk_PhotoPutBlock_NoComposite(Tk_PhotoHandle handle,
				Tk_PhotoImageBlock *blockPtr, int x, int y,
				int width, int height);
#endif
#ifndef Tk_PhotoPutZoomedBlock_NoComposite_TCL_DECLARED
#define Tk_PhotoPutZoomedBlock_NoComposite_TCL_DECLARED

EXTERN void		Tk_PhotoPutZoomedBlock_NoComposite(
				Tk_PhotoHandle handle,
				Tk_PhotoImageBlock *blockPtr, int x, int y,
				int width, int height, int zoomX, int zoomY,
				int subsampleX, int subsampleY);
#endif
#ifndef Tk_PhotoGetImage_TCL_DECLARED
#define Tk_PhotoGetImage_TCL_DECLARED

EXTERN int		Tk_PhotoGetImage(Tk_PhotoHandle handle,
				Tk_PhotoImageBlock *blockPtr);
#endif
#ifndef Tk_PhotoBlank_TCL_DECLARED
#define Tk_PhotoBlank_TCL_DECLARED

EXTERN void		Tk_PhotoBlank(Tk_PhotoHandle handle);
#endif
#ifndef Tk_PhotoExpand_Panic_TCL_DECLARED
#define Tk_PhotoExpand_Panic_TCL_DECLARED

EXTERN void		Tk_PhotoExpand_Panic(Tk_PhotoHandle handle,
				int width, int height);
#endif
#ifndef Tk_PhotoGetSize_TCL_DECLARED
#define Tk_PhotoGetSize_TCL_DECLARED

EXTERN void		Tk_PhotoGetSize(Tk_PhotoHandle handle, int *widthPtr,
				int *heightPtr);
#endif
#ifndef Tk_PhotoSetSize_Panic_TCL_DECLARED
#define Tk_PhotoSetSize_Panic_TCL_DECLARED

EXTERN void		Tk_PhotoSetSize_Panic(Tk_PhotoHandle handle,
				int width, int height);
#endif
#ifndef Tk_PointToChar_TCL_DECLARED
#define Tk_PointToChar_TCL_DECLARED

EXTERN int		Tk_PointToChar(Tk_TextLayout layout, int x, int y);
#endif
#ifndef Tk_PostscriptFontName_TCL_DECLARED
#define Tk_PostscriptFontName_TCL_DECLARED

EXTERN int		Tk_PostscriptFontName(Tk_Font tkfont,
				Tcl_DString *dsPtr);
#endif
#ifndef Tk_PreserveColormap_TCL_DECLARED
#define Tk_PreserveColormap_TCL_DECLARED

EXTERN void		Tk_PreserveColormap(Display *display,
				Colormap colormap);
#endif
#ifndef Tk_QueueWindowEvent_TCL_DECLARED
#define Tk_QueueWindowEvent_TCL_DECLARED

EXTERN void		Tk_QueueWindowEvent(XEvent *eventPtr,
				Tcl_QueuePosition position);
#endif
#ifndef Tk_RedrawImage_TCL_DECLARED
#define Tk_RedrawImage_TCL_DECLARED

EXTERN void		Tk_RedrawImage(Tk_Image image, int imageX,
				int imageY, int width, int height,
				Drawable drawable, int drawableX,
				int drawableY);
#endif
#ifndef Tk_ResizeWindow_TCL_DECLARED
#define Tk_ResizeWindow_TCL_DECLARED

EXTERN void		Tk_ResizeWindow(Tk_Window tkwin, int width,
				int height);
#endif
#ifndef Tk_RestackWindow_TCL_DECLARED
#define Tk_RestackWindow_TCL_DECLARED

EXTERN int		Tk_RestackWindow(Tk_Window tkwin, int aboveBelow,
				Tk_Window other);
#endif
#ifndef Tk_RestrictEvents_TCL_DECLARED
#define Tk_RestrictEvents_TCL_DECLARED

EXTERN Tk_RestrictProc * Tk_RestrictEvents(Tk_RestrictProc *proc,
				ClientData arg, ClientData *prevArgPtr);
#endif
#ifndef Tk_SafeInit_TCL_DECLARED
#define Tk_SafeInit_TCL_DECLARED

EXTERN int		Tk_SafeInit(Tcl_Interp *interp);
#endif
#ifndef Tk_SetAppName_TCL_DECLARED
#define Tk_SetAppName_TCL_DECLARED

EXTERN CONST char *	Tk_SetAppName(Tk_Window tkwin, CONST char *name);
#endif
#ifndef Tk_SetBackgroundFromBorder_TCL_DECLARED
#define Tk_SetBackgroundFromBorder_TCL_DECLARED

EXTERN void		Tk_SetBackgroundFromBorder(Tk_Window tkwin,
				Tk_3DBorder border);
#endif
#ifndef Tk_SetClass_TCL_DECLARED
#define Tk_SetClass_TCL_DECLARED

EXTERN void		Tk_SetClass(Tk_Window tkwin, CONST char *className);
#endif
#ifndef Tk_SetGrid_TCL_DECLARED
#define Tk_SetGrid_TCL_DECLARED

EXTERN void		Tk_SetGrid(Tk_Window tkwin, int reqWidth,
				int reqHeight, int gridWidth, int gridHeight);
#endif
#ifndef Tk_SetInternalBorder_TCL_DECLARED
#define Tk_SetInternalBorder_TCL_DECLARED

EXTERN void		Tk_SetInternalBorder(Tk_Window tkwin, int width);
#endif
#ifndef Tk_SetWindowBackground_TCL_DECLARED
#define Tk_SetWindowBackground_TCL_DECLARED

EXTERN void		Tk_SetWindowBackground(Tk_Window tkwin,
				unsigned long pixel);
#endif
#ifndef Tk_SetWindowBackgroundPixmap_TCL_DECLARED
#define Tk_SetWindowBackgroundPixmap_TCL_DECLARED

EXTERN void		Tk_SetWindowBackgroundPixmap(Tk_Window tkwin,
				Pixmap pixmap);
#endif
#ifndef Tk_SetWindowBorder_TCL_DECLARED
#define Tk_SetWindowBorder_TCL_DECLARED

EXTERN void		Tk_SetWindowBorder(Tk_Window tkwin,
				unsigned long pixel);
#endif
#ifndef Tk_SetWindowBorderWidth_TCL_DECLARED
#define Tk_SetWindowBorderWidth_TCL_DECLARED

EXTERN void		Tk_SetWindowBorderWidth(Tk_Window tkwin, int width);
#endif
#ifndef Tk_SetWindowBorderPixmap_TCL_DECLARED
#define Tk_SetWindowBorderPixmap_TCL_DECLARED

EXTERN void		Tk_SetWindowBorderPixmap(Tk_Window tkwin,
				Pixmap pixmap);
#endif
#ifndef Tk_SetWindowColormap_TCL_DECLARED
#define Tk_SetWindowColormap_TCL_DECLARED

EXTERN void		Tk_SetWindowColormap(Tk_Window tkwin,
				Colormap colormap);
#endif
#ifndef Tk_SetWindowVisual_TCL_DECLARED
#define Tk_SetWindowVisual_TCL_DECLARED

EXTERN int		Tk_SetWindowVisual(Tk_Window tkwin, Visual *visual,
				int depth, Colormap colormap);
#endif
#ifndef Tk_SizeOfBitmap_TCL_DECLARED
#define Tk_SizeOfBitmap_TCL_DECLARED

EXTERN void		Tk_SizeOfBitmap(Display *display, Pixmap bitmap,
				int *widthPtr, int *heightPtr);
#endif
#ifndef Tk_SizeOfImage_TCL_DECLARED
#define Tk_SizeOfImage_TCL_DECLARED

EXTERN void		Tk_SizeOfImage(Tk_Image image, int *widthPtr,
				int *heightPtr);
#endif
#ifndef Tk_StrictMotif_TCL_DECLARED
#define Tk_StrictMotif_TCL_DECLARED

EXTERN int		Tk_StrictMotif(Tk_Window tkwin);
#endif
#ifndef Tk_TextLayoutToPostscript_TCL_DECLARED
#define Tk_TextLayoutToPostscript_TCL_DECLARED

EXTERN void		Tk_TextLayoutToPostscript(Tcl_Interp *interp,
				Tk_TextLayout layout);
#endif
#ifndef Tk_TextWidth_TCL_DECLARED
#define Tk_TextWidth_TCL_DECLARED

EXTERN int		Tk_TextWidth(Tk_Font font, CONST char *str,
				int numBytes);
#endif
#ifndef Tk_UndefineCursor_TCL_DECLARED
#define Tk_UndefineCursor_TCL_DECLARED

EXTERN void		Tk_UndefineCursor(Tk_Window window);
#endif
#ifndef Tk_UnderlineChars_TCL_DECLARED
#define Tk_UnderlineChars_TCL_DECLARED

EXTERN void		Tk_UnderlineChars(Display *display,
				Drawable drawable, GC gc, Tk_Font tkfont,
				CONST char *source, int x, int y,
				int firstByte, int lastByte);
#endif
#ifndef Tk_UnderlineTextLayout_TCL_DECLARED
#define Tk_UnderlineTextLayout_TCL_DECLARED

EXTERN void		Tk_UnderlineTextLayout(Display *display,
				Drawable drawable, GC gc,
				Tk_TextLayout layout, int x, int y,
				int underline);
#endif
#ifndef Tk_Ungrab_TCL_DECLARED
#define Tk_Ungrab_TCL_DECLARED

EXTERN void		Tk_Ungrab(Tk_Window tkwin);
#endif
#ifndef Tk_UnmaintainGeometry_TCL_DECLARED
#define Tk_UnmaintainGeometry_TCL_DECLARED

EXTERN void		Tk_UnmaintainGeometry(Tk_Window slave,
				Tk_Window master);
#endif
#ifndef Tk_UnmapWindow_TCL_DECLARED
#define Tk_UnmapWindow_TCL_DECLARED

EXTERN void		Tk_UnmapWindow(Tk_Window tkwin);
#endif
#ifndef Tk_UnsetGrid_TCL_DECLARED
#define Tk_UnsetGrid_TCL_DECLARED

EXTERN void		Tk_UnsetGrid(Tk_Window tkwin);
#endif
#ifndef Tk_UpdatePointer_TCL_DECLARED
#define Tk_UpdatePointer_TCL_DECLARED

EXTERN void		Tk_UpdatePointer(Tk_Window tkwin, int x, int y,
				int state);
#endif
#ifndef Tk_AllocBitmapFromObj_TCL_DECLARED
#define Tk_AllocBitmapFromObj_TCL_DECLARED

EXTERN Pixmap		Tk_AllocBitmapFromObj(Tcl_Interp *interp,
				Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_Alloc3DBorderFromObj_TCL_DECLARED
#define Tk_Alloc3DBorderFromObj_TCL_DECLARED

EXTERN Tk_3DBorder	Tk_Alloc3DBorderFromObj(Tcl_Interp *interp,
				Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_AllocColorFromObj_TCL_DECLARED
#define Tk_AllocColorFromObj_TCL_DECLARED

EXTERN XColor *		Tk_AllocColorFromObj(Tcl_Interp *interp,
				Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_AllocCursorFromObj_TCL_DECLARED
#define Tk_AllocCursorFromObj_TCL_DECLARED

EXTERN Tk_Cursor	Tk_AllocCursorFromObj(Tcl_Interp *interp,
				Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_AllocFontFromObj_TCL_DECLARED
#define Tk_AllocFontFromObj_TCL_DECLARED

EXTERN Tk_Font		Tk_AllocFontFromObj(Tcl_Interp *interp,
				Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_CreateOptionTable_TCL_DECLARED
#define Tk_CreateOptionTable_TCL_DECLARED

EXTERN Tk_OptionTable	Tk_CreateOptionTable(Tcl_Interp *interp,
				CONST Tk_OptionSpec *templatePtr);
#endif
#ifndef Tk_DeleteOptionTable_TCL_DECLARED
#define Tk_DeleteOptionTable_TCL_DECLARED

EXTERN void		Tk_DeleteOptionTable(Tk_OptionTable optionTable);
#endif
#ifndef Tk_Free3DBorderFromObj_TCL_DECLARED
#define Tk_Free3DBorderFromObj_TCL_DECLARED

EXTERN void		Tk_Free3DBorderFromObj(Tk_Window tkwin,
				Tcl_Obj *objPtr);
#endif
#ifndef Tk_FreeBitmapFromObj_TCL_DECLARED
#define Tk_FreeBitmapFromObj_TCL_DECLARED

EXTERN void		Tk_FreeBitmapFromObj(Tk_Window tkwin,
				Tcl_Obj *objPtr);
#endif
#ifndef Tk_FreeColorFromObj_TCL_DECLARED
#define Tk_FreeColorFromObj_TCL_DECLARED

EXTERN void		Tk_FreeColorFromObj(Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_FreeConfigOptions_TCL_DECLARED
#define Tk_FreeConfigOptions_TCL_DECLARED

EXTERN void		Tk_FreeConfigOptions(char *recordPtr,
				Tk_OptionTable optionToken, Tk_Window tkwin);
#endif
#ifndef Tk_FreeSavedOptions_TCL_DECLARED
#define Tk_FreeSavedOptions_TCL_DECLARED

EXTERN void		Tk_FreeSavedOptions(Tk_SavedOptions *savePtr);
#endif
#ifndef Tk_FreeCursorFromObj_TCL_DECLARED
#define Tk_FreeCursorFromObj_TCL_DECLARED

EXTERN void		Tk_FreeCursorFromObj(Tk_Window tkwin,
				Tcl_Obj *objPtr);
#endif
#ifndef Tk_FreeFontFromObj_TCL_DECLARED
#define Tk_FreeFontFromObj_TCL_DECLARED

EXTERN void		Tk_FreeFontFromObj(Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_Get3DBorderFromObj_TCL_DECLARED
#define Tk_Get3DBorderFromObj_TCL_DECLARED

EXTERN Tk_3DBorder	Tk_Get3DBorderFromObj(Tk_Window tkwin,
				Tcl_Obj *objPtr);
#endif
#ifndef Tk_GetAnchorFromObj_TCL_DECLARED
#define Tk_GetAnchorFromObj_TCL_DECLARED

EXTERN int		Tk_GetAnchorFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, Tk_Anchor *anchorPtr);
#endif
#ifndef Tk_GetBitmapFromObj_TCL_DECLARED
#define Tk_GetBitmapFromObj_TCL_DECLARED

EXTERN Pixmap		Tk_GetBitmapFromObj(Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_GetColorFromObj_TCL_DECLARED
#define Tk_GetColorFromObj_TCL_DECLARED

EXTERN XColor *		Tk_GetColorFromObj(Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_GetCursorFromObj_TCL_DECLARED
#define Tk_GetCursorFromObj_TCL_DECLARED

EXTERN Tk_Cursor	Tk_GetCursorFromObj(Tk_Window tkwin, Tcl_Obj *objPtr);
#endif
#ifndef Tk_GetOptionInfo_TCL_DECLARED
#define Tk_GetOptionInfo_TCL_DECLARED

EXTERN Tcl_Obj *	Tk_GetOptionInfo(Tcl_Interp *interp, char *recordPtr,
				Tk_OptionTable optionTable, Tcl_Obj *namePtr,
				Tk_Window tkwin);
#endif
#ifndef Tk_GetOptionValue_TCL_DECLARED
#define Tk_GetOptionValue_TCL_DECLARED

EXTERN Tcl_Obj *	Tk_GetOptionValue(Tcl_Interp *interp,
				char *recordPtr, Tk_OptionTable optionTable,
				Tcl_Obj *namePtr, Tk_Window tkwin);
#endif
#ifndef Tk_GetJustifyFromObj_TCL_DECLARED
#define Tk_GetJustifyFromObj_TCL_DECLARED

EXTERN int		Tk_GetJustifyFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, Tk_Justify *justifyPtr);
#endif
#ifndef Tk_GetMMFromObj_TCL_DECLARED
#define Tk_GetMMFromObj_TCL_DECLARED

EXTERN int		Tk_GetMMFromObj(Tcl_Interp *interp, Tk_Window tkwin,
				Tcl_Obj *objPtr, double *doublePtr);
#endif
#ifndef Tk_GetPixelsFromObj_TCL_DECLARED
#define Tk_GetPixelsFromObj_TCL_DECLARED

EXTERN int		Tk_GetPixelsFromObj(Tcl_Interp *interp,
				Tk_Window tkwin, Tcl_Obj *objPtr,
				int *intPtr);
#endif
#ifndef Tk_GetReliefFromObj_TCL_DECLARED
#define Tk_GetReliefFromObj_TCL_DECLARED

EXTERN int		Tk_GetReliefFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *resultPtr);
#endif
#ifndef Tk_GetScrollInfoObj_TCL_DECLARED
#define Tk_GetScrollInfoObj_TCL_DECLARED

EXTERN int		Tk_GetScrollInfoObj(Tcl_Interp *interp, int objc,
				Tcl_Obj *CONST objv[], double *dblPtr,
				int *intPtr);
#endif
#ifndef Tk_InitOptions_TCL_DECLARED
#define Tk_InitOptions_TCL_DECLARED

EXTERN int		Tk_InitOptions(Tcl_Interp *interp, char *recordPtr,
				Tk_OptionTable optionToken, Tk_Window tkwin);
#endif
#ifndef Tk_MainEx_TCL_DECLARED
#define Tk_MainEx_TCL_DECLARED

EXTERN void		Tk_MainEx(int argc, char **argv,
				Tcl_AppInitProc *appInitProc,
				Tcl_Interp *interp);
#endif
#ifndef Tk_RestoreSavedOptions_TCL_DECLARED
#define Tk_RestoreSavedOptions_TCL_DECLARED

EXTERN void		Tk_RestoreSavedOptions(Tk_SavedOptions *savePtr);
#endif
#ifndef Tk_SetOptions_TCL_DECLARED
#define Tk_SetOptions_TCL_DECLARED

EXTERN int		Tk_SetOptions(Tcl_Interp *interp, char *recordPtr,
				Tk_OptionTable optionTable, int objc,
				Tcl_Obj *CONST objv[], Tk_Window tkwin,
				Tk_SavedOptions *savePtr, int *maskPtr);
#endif
#ifndef Tk_InitConsoleChannels_TCL_DECLARED
#define Tk_InitConsoleChannels_TCL_DECLARED

EXTERN void		Tk_InitConsoleChannels(Tcl_Interp *interp);
#endif
#ifndef Tk_CreateConsoleWindow_TCL_DECLARED
#define Tk_CreateConsoleWindow_TCL_DECLARED

EXTERN int		Tk_CreateConsoleWindow(Tcl_Interp *interp);
#endif
#ifndef Tk_CreateSmoothMethod_TCL_DECLARED
#define Tk_CreateSmoothMethod_TCL_DECLARED

EXTERN void		Tk_CreateSmoothMethod(Tcl_Interp *interp,
				Tk_SmoothMethod *method);
#endif


#ifndef Tk_GetDash_TCL_DECLARED
#define Tk_GetDash_TCL_DECLARED

EXTERN int		Tk_GetDash(Tcl_Interp *interp, CONST char *value,
				Tk_Dash *dash);
#endif
#ifndef Tk_CreateOutline_TCL_DECLARED
#define Tk_CreateOutline_TCL_DECLARED

EXTERN void		Tk_CreateOutline(Tk_Outline *outline);
#endif
#ifndef Tk_DeleteOutline_TCL_DECLARED
#define Tk_DeleteOutline_TCL_DECLARED

EXTERN void		Tk_DeleteOutline(Display *display,
				Tk_Outline *outline);
#endif
#ifndef Tk_ConfigOutlineGC_TCL_DECLARED
#define Tk_ConfigOutlineGC_TCL_DECLARED

EXTERN int		Tk_ConfigOutlineGC(XGCValues *gcValues,
				Tk_Canvas canvas, Tk_Item *item,
				Tk_Outline *outline);
#endif
#ifndef Tk_ChangeOutlineGC_TCL_DECLARED
#define Tk_ChangeOutlineGC_TCL_DECLARED

EXTERN int		Tk_ChangeOutlineGC(Tk_Canvas canvas, Tk_Item *item,
				Tk_Outline *outline);
#endif
#ifndef Tk_ResetOutlineGC_TCL_DECLARED
#define Tk_ResetOutlineGC_TCL_DECLARED

EXTERN int		Tk_ResetOutlineGC(Tk_Canvas canvas, Tk_Item *item,
				Tk_Outline *outline);
#endif
#ifndef Tk_CanvasPsOutline_TCL_DECLARED
#define Tk_CanvasPsOutline_TCL_DECLARED

EXTERN int		Tk_CanvasPsOutline(Tk_Canvas canvas, Tk_Item *item,
				Tk_Outline *outline);
#endif
#ifndef Tk_SetTSOrigin_TCL_DECLARED
#define Tk_SetTSOrigin_TCL_DECLARED

EXTERN void		Tk_SetTSOrigin(Tk_Window tkwin, GC gc, int x, int y);
#endif
#ifndef Tk_CanvasGetCoordFromObj_TCL_DECLARED
#define Tk_CanvasGetCoordFromObj_TCL_DECLARED

EXTERN int		Tk_CanvasGetCoordFromObj(Tcl_Interp *interp,
				Tk_Canvas canvas, Tcl_Obj *obj,
				double *doublePtr);
#endif
#ifndef Tk_CanvasSetOffset_TCL_DECLARED
#define Tk_CanvasSetOffset_TCL_DECLARED

EXTERN void		Tk_CanvasSetOffset(Tk_Canvas canvas, GC gc,
				Tk_TSOffset *offset);
#endif
#ifndef Tk_DitherPhoto_TCL_DECLARED
#define Tk_DitherPhoto_TCL_DECLARED

EXTERN void		Tk_DitherPhoto(Tk_PhotoHandle handle, int x, int y,
				int width, int height);
#endif
#ifndef Tk_PostscriptBitmap_TCL_DECLARED
#define Tk_PostscriptBitmap_TCL_DECLARED

EXTERN int		Tk_PostscriptBitmap(Tcl_Interp *interp,
				Tk_Window tkwin, Tk_PostscriptInfo psInfo,
				Pixmap bitmap, int startX, int startY,
				int width, int height);
#endif
#ifndef Tk_PostscriptColor_TCL_DECLARED
#define Tk_PostscriptColor_TCL_DECLARED

EXTERN int		Tk_PostscriptColor(Tcl_Interp *interp,
				Tk_PostscriptInfo psInfo, XColor *colorPtr);
#endif
#ifndef Tk_PostscriptFont_TCL_DECLARED
#define Tk_PostscriptFont_TCL_DECLARED

EXTERN int		Tk_PostscriptFont(Tcl_Interp *interp,
				Tk_PostscriptInfo psInfo, Tk_Font font);
#endif
#ifndef Tk_PostscriptImage_TCL_DECLARED
#define Tk_PostscriptImage_TCL_DECLARED

EXTERN int		Tk_PostscriptImage(Tk_Image image,
				Tcl_Interp *interp, Tk_Window tkwin,
				Tk_PostscriptInfo psinfo, int x, int y,
				int width, int height, int prepass);
#endif
#ifndef Tk_PostscriptPath_TCL_DECLARED
#define Tk_PostscriptPath_TCL_DECLARED

EXTERN void		Tk_PostscriptPath(Tcl_Interp *interp,
				Tk_PostscriptInfo psInfo, double *coordPtr,
				int numPoints);
#endif
#ifndef Tk_PostscriptStipple_TCL_DECLARED
#define Tk_PostscriptStipple_TCL_DECLARED

EXTERN int		Tk_PostscriptStipple(Tcl_Interp *interp,
				Tk_Window tkwin, Tk_PostscriptInfo psInfo,
				Pixmap bitmap);
#endif
#ifndef Tk_PostscriptY_TCL_DECLARED
#define Tk_PostscriptY_TCL_DECLARED

EXTERN double		Tk_PostscriptY(double y, Tk_PostscriptInfo psInfo);
#endif
#ifndef Tk_PostscriptPhoto_TCL_DECLARED
#define Tk_PostscriptPhoto_TCL_DECLARED

EXTERN int		Tk_PostscriptPhoto(Tcl_Interp *interp,
				Tk_PhotoImageBlock *blockPtr,
				Tk_PostscriptInfo psInfo, int width,
				int height);
#endif
#ifndef Tk_CreateClientMessageHandler_TCL_DECLARED
#define Tk_CreateClientMessageHandler_TCL_DECLARED

EXTERN void		Tk_CreateClientMessageHandler(
				Tk_ClientMessageProc *proc);
#endif
#ifndef Tk_DeleteClientMessageHandler_TCL_DECLARED
#define Tk_DeleteClientMessageHandler_TCL_DECLARED

EXTERN void		Tk_DeleteClientMessageHandler(
				Tk_ClientMessageProc *proc);
#endif
#ifndef Tk_CreateAnonymousWindow_TCL_DECLARED
#define Tk_CreateAnonymousWindow_TCL_DECLARED

EXTERN Tk_Window	Tk_CreateAnonymousWindow(Tcl_Interp *interp,
				Tk_Window parent, CONST char *screenName);
#endif
#ifndef Tk_SetClassProcs_TCL_DECLARED
#define Tk_SetClassProcs_TCL_DECLARED

EXTERN void		Tk_SetClassProcs(Tk_Window tkwin,
				Tk_ClassProcs *procs,
				ClientData instanceData);
#endif
#ifndef Tk_SetInternalBorderEx_TCL_DECLARED
#define Tk_SetInternalBorderEx_TCL_DECLARED

EXTERN void		Tk_SetInternalBorderEx(Tk_Window tkwin, int left,
				int right, int top, int bottom);
#endif
#ifndef Tk_SetMinimumRequestSize_TCL_DECLARED
#define Tk_SetMinimumRequestSize_TCL_DECLARED

EXTERN void		Tk_SetMinimumRequestSize(Tk_Window tkwin,
				int minWidth, int minHeight);
#endif
#ifndef Tk_SetCaretPos_TCL_DECLARED
#define Tk_SetCaretPos_TCL_DECLARED

EXTERN void		Tk_SetCaretPos(Tk_Window tkwin, int x, int y,
				int height);
#endif
#ifndef Tk_PhotoPutBlock_Panic_TCL_DECLARED
#define Tk_PhotoPutBlock_Panic_TCL_DECLARED

EXTERN void		Tk_PhotoPutBlock_Panic(Tk_PhotoHandle handle,
				Tk_PhotoImageBlock *blockPtr, int x, int y,
				int width, int height, int compRule);
#endif
#ifndef Tk_PhotoPutZoomedBlock_Panic_TCL_DECLARED
#define Tk_PhotoPutZoomedBlock_Panic_TCL_DECLARED

EXTERN void		Tk_PhotoPutZoomedBlock_Panic(Tk_PhotoHandle handle,
				Tk_PhotoImageBlock *blockPtr, int x, int y,
				int width, int height, int zoomX, int zoomY,
				int subsampleX, int subsampleY, int compRule);
#endif
#ifndef Tk_CollapseMotionEvents_TCL_DECLARED
#define Tk_CollapseMotionEvents_TCL_DECLARED

EXTERN int		Tk_CollapseMotionEvents(Display *display,
				int collapse);
#endif
#ifndef Tk_RegisterStyleEngine_TCL_DECLARED
#define Tk_RegisterStyleEngine_TCL_DECLARED

EXTERN Tk_StyleEngine	Tk_RegisterStyleEngine(CONST char *name,
				Tk_StyleEngine parent);
#endif
#ifndef Tk_GetStyleEngine_TCL_DECLARED
#define Tk_GetStyleEngine_TCL_DECLARED

EXTERN Tk_StyleEngine	Tk_GetStyleEngine(CONST char *name);
#endif
#ifndef Tk_RegisterStyledElement_TCL_DECLARED
#define Tk_RegisterStyledElement_TCL_DECLARED

EXTERN int		Tk_RegisterStyledElement(Tk_StyleEngine engine,
				Tk_ElementSpec *templatePtr);
#endif
#ifndef Tk_GetElementId_TCL_DECLARED
#define Tk_GetElementId_TCL_DECLARED

EXTERN int		Tk_GetElementId(CONST char *name);
#endif
#ifndef Tk_CreateStyle_TCL_DECLARED
#define Tk_CreateStyle_TCL_DECLARED

EXTERN Tk_Style		Tk_CreateStyle(CONST char *name,
				Tk_StyleEngine engine, ClientData clientData);
#endif
#ifndef Tk_GetStyle_TCL_DECLARED
#define Tk_GetStyle_TCL_DECLARED

EXTERN Tk_Style		Tk_GetStyle(Tcl_Interp *interp, CONST char *name);
#endif
#ifndef Tk_FreeStyle_TCL_DECLARED
#define Tk_FreeStyle_TCL_DECLARED

EXTERN void		Tk_FreeStyle(Tk_Style style);
#endif
#ifndef Tk_NameOfStyle_TCL_DECLARED
#define Tk_NameOfStyle_TCL_DECLARED

EXTERN CONST char *	Tk_NameOfStyle(Tk_Style style);
#endif
#ifndef Tk_AllocStyleFromObj_TCL_DECLARED
#define Tk_AllocStyleFromObj_TCL_DECLARED

EXTERN Tk_Style		Tk_AllocStyleFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr);
#endif
#ifndef Tk_GetStyleFromObj_TCL_DECLARED
#define Tk_GetStyleFromObj_TCL_DECLARED

EXTERN Tk_Style		Tk_GetStyleFromObj(Tcl_Obj *objPtr);
#endif
#ifndef Tk_FreeStyleFromObj_TCL_DECLARED
#define Tk_FreeStyleFromObj_TCL_DECLARED

EXTERN void		Tk_FreeStyleFromObj(Tcl_Obj *objPtr);
#endif
#ifndef Tk_GetStyledElement_TCL_DECLARED
#define Tk_GetStyledElement_TCL_DECLARED

EXTERN Tk_StyledElement	 Tk_GetStyledElement(Tk_Style style, int elementId,
				Tk_OptionTable optionTable);
#endif
#ifndef Tk_GetElementSize_TCL_DECLARED
#define Tk_GetElementSize_TCL_DECLARED

EXTERN void		Tk_GetElementSize(Tk_Style style,
				Tk_StyledElement element, char *recordPtr,
				Tk_Window tkwin, int width, int height,
				int inner, int *widthPtr, int *heightPtr);
#endif
#ifndef Tk_GetElementBox_TCL_DECLARED
#define Tk_GetElementBox_TCL_DECLARED

EXTERN void		Tk_GetElementBox(Tk_Style style,
				Tk_StyledElement element, char *recordPtr,
				Tk_Window tkwin, int x, int y, int width,
				int height, int inner, int *xPtr, int *yPtr,
				int *widthPtr, int *heightPtr);
#endif
#ifndef Tk_GetElementBorderWidth_TCL_DECLARED
#define Tk_GetElementBorderWidth_TCL_DECLARED

EXTERN int		Tk_GetElementBorderWidth(Tk_Style style,
				Tk_StyledElement element, char *recordPtr,
				Tk_Window tkwin);
#endif
#ifndef Tk_DrawElement_TCL_DECLARED
#define Tk_DrawElement_TCL_DECLARED

EXTERN void		Tk_DrawElement(Tk_Style style,
				Tk_StyledElement element, char *recordPtr,
				Tk_Window tkwin, Drawable d, int x, int y,
				int width, int height, int state);
#endif
#ifndef Tk_PhotoExpand_TCL_DECLARED
#define Tk_PhotoExpand_TCL_DECLARED

EXTERN int		Tk_PhotoExpand(Tcl_Interp *interp,
				Tk_PhotoHandle handle, int width, int height);
#endif
#ifndef Tk_PhotoPutBlock_TCL_DECLARED
#define Tk_PhotoPutBlock_TCL_DECLARED

EXTERN int		Tk_PhotoPutBlock(Tcl_Interp *interp,
				Tk_PhotoHandle handle,
				Tk_PhotoImageBlock *blockPtr, int x, int y,
				int width, int height, int compRule);
#endif
#ifndef Tk_PhotoPutZoomedBlock_TCL_DECLARED
#define Tk_PhotoPutZoomedBlock_TCL_DECLARED

EXTERN int		Tk_PhotoPutZoomedBlock(Tcl_Interp *interp,
				Tk_PhotoHandle handle,
				Tk_PhotoImageBlock *blockPtr, int x, int y,
				int width, int height, int zoomX, int zoomY,
				int subsampleX, int subsampleY, int compRule);
#endif
#ifndef Tk_PhotoSetSize_TCL_DECLARED
#define Tk_PhotoSetSize_TCL_DECLARED

EXTERN int		Tk_PhotoSetSize(Tcl_Interp *interp,
				Tk_PhotoHandle handle, int width, int height);
#endif
#ifndef Tk_GetUserInactiveTime_TCL_DECLARED
#define Tk_GetUserInactiveTime_TCL_DECLARED

EXTERN long		Tk_GetUserInactiveTime(Display *dpy);
#endif
#ifndef Tk_ResetUserInactiveTime_TCL_DECLARED
#define Tk_ResetUserInactiveTime_TCL_DECLARED

EXTERN void		Tk_ResetUserInactiveTime(Display *dpy);
#endif
#ifndef Tk_Interp_TCL_DECLARED
#define Tk_Interp_TCL_DECLARED

EXTERN Tcl_Interp *	Tk_Interp(Tk_Window tkwin);
#endif
#ifndef Tk_CreateOldImageType_TCL_DECLARED
#define Tk_CreateOldImageType_TCL_DECLARED

EXTERN void		Tk_CreateOldImageType(Tk_ImageType *typePtr);
#endif
#ifndef Tk_CreateOldPhotoImageFormat_TCL_DECLARED
#define Tk_CreateOldPhotoImageFormat_TCL_DECLARED

EXTERN void		Tk_CreateOldPhotoImageFormat(
				Tk_PhotoImageFormat *formatPtr);
#endif

#ifndef TkUnusedStubEntry_TCL_DECLARED
#define TkUnusedStubEntry_TCL_DECLARED

EXTERN void		TkUnusedStubEntry(void);
#endif

typedef struct TkStubHooks {
    struct TkPlatStubs *tkPlatStubs;
    struct TkIntStubs *tkIntStubs;
    struct TkIntPlatStubs *tkIntPlatStubs;
    struct TkIntXlibStubs *tkIntXlibStubs;
} TkStubHooks;

typedef struct TkStubs {
    int magic;
    struct TkStubHooks *hooks;

    void (*tk_MainLoop) (void); 
    XColor * (*tk_3DBorderColor) (Tk_3DBorder border); 
    GC (*tk_3DBorderGC) (Tk_Window tkwin, Tk_3DBorder border, int which); 
    void (*tk_3DHorizontalBevel) (Tk_Window tkwin, Drawable drawable, Tk_3DBorder border, int x, int y, int width, int height, int leftIn, int rightIn, int topBevel, int relief); 
    void (*tk_3DVerticalBevel) (Tk_Window tkwin, Drawable drawable, Tk_3DBorder border, int x, int y, int width, int height, int leftBevel, int relief); 
    void (*tk_AddOption) (Tk_Window tkwin, CONST char *name, CONST char *value, int priority); 
    void (*tk_BindEvent) (Tk_BindingTable bindingTable, XEvent *eventPtr, Tk_Window tkwin, int numObjects, ClientData *objectPtr); 
    void (*tk_CanvasDrawableCoords) (Tk_Canvas canvas, double x, double y, short *drawableXPtr, short *drawableYPtr); 
    void (*tk_CanvasEventuallyRedraw) (Tk_Canvas canvas, int x1, int y1, int x2, int y2); 
    int (*tk_CanvasGetCoord) (Tcl_Interp *interp, Tk_Canvas canvas, CONST char *str, double *doublePtr); 
    Tk_CanvasTextInfo * (*tk_CanvasGetTextInfo) (Tk_Canvas canvas); 
    int (*tk_CanvasPsBitmap) (Tcl_Interp *interp, Tk_Canvas canvas, Pixmap bitmap, int x, int y, int width, int height); 
    int (*tk_CanvasPsColor) (Tcl_Interp *interp, Tk_Canvas canvas, XColor *colorPtr); 
    int (*tk_CanvasPsFont) (Tcl_Interp *interp, Tk_Canvas canvas, Tk_Font font); 
    void (*tk_CanvasPsPath) (Tcl_Interp *interp, Tk_Canvas canvas, double *coordPtr, int numPoints); 
    int (*tk_CanvasPsStipple) (Tcl_Interp *interp, Tk_Canvas canvas, Pixmap bitmap); 
    double (*tk_CanvasPsY) (Tk_Canvas canvas, double y); 
    void (*tk_CanvasSetStippleOrigin) (Tk_Canvas canvas, GC gc); 
    int (*tk_CanvasTagsParseProc) (ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin, CONST char *value, char *widgRec, int offset); 
    char * (*tk_CanvasTagsPrintProc) (ClientData clientData, Tk_Window tkwin, char *widgRec, int offset, Tcl_FreeProc **freeProcPtr); 
    Tk_Window (*tk_CanvasTkwin) (Tk_Canvas canvas); 
    void (*tk_CanvasWindowCoords) (Tk_Canvas canvas, double x, double y, short *screenXPtr, short *screenYPtr); 
    void (*tk_ChangeWindowAttributes) (Tk_Window tkwin, unsigned long valueMask, XSetWindowAttributes *attsPtr); 
    int (*tk_CharBbox) (Tk_TextLayout layout, int index, int *xPtr, int *yPtr, int *widthPtr, int *heightPtr); 
    void (*tk_ClearSelection) (Tk_Window tkwin, Atom selection); 
    int (*tk_ClipboardAppend) (Tcl_Interp *interp, Tk_Window tkwin, Atom target, Atom format, char *buffer); 
    int (*tk_ClipboardClear) (Tcl_Interp *interp, Tk_Window tkwin); 
    int (*tk_ConfigureInfo) (Tcl_Interp *interp, Tk_Window tkwin, Tk_ConfigSpec *specs, char *widgRec, CONST char *argvName, int flags); 
    int (*tk_ConfigureValue) (Tcl_Interp *interp, Tk_Window tkwin, Tk_ConfigSpec *specs, char *widgRec, CONST char *argvName, int flags); 
    int (*tk_ConfigureWidget) (Tcl_Interp *interp, Tk_Window tkwin, Tk_ConfigSpec *specs, int argc, CONST84 char **argv, char *widgRec, int flags); 
    void (*tk_ConfigureWindow) (Tk_Window tkwin, unsigned int valueMask, XWindowChanges *valuePtr); 
    Tk_TextLayout (*tk_ComputeTextLayout) (Tk_Font font, CONST char *str, int numChars, int wrapLength, Tk_Justify justify, int flags, int *widthPtr, int *heightPtr); 
    Tk_Window (*tk_CoordsToWindow) (int rootX, int rootY, Tk_Window tkwin); 
    unsigned long (*tk_CreateBinding) (Tcl_Interp *interp, Tk_BindingTable bindingTable, ClientData object, CONST char *eventStr, CONST char *command, int append); 
    Tk_BindingTable (*tk_CreateBindingTable) (Tcl_Interp *interp); 
    Tk_ErrorHandler (*tk_CreateErrorHandler) (Display *display, int errNum, int request, int minorCode, Tk_ErrorProc *errorProc, ClientData clientData); 
    void (*tk_CreateEventHandler) (Tk_Window token, unsigned long mask, Tk_EventProc *proc, ClientData clientData); 
    void (*tk_CreateGenericHandler) (Tk_GenericProc *proc, ClientData clientData); 
    void (*tk_CreateImageType) (Tk_ImageType *typePtr); 
    void (*tk_CreateItemType) (Tk_ItemType *typePtr); 
    void (*tk_CreatePhotoImageFormat) (Tk_PhotoImageFormat *formatPtr); 
    void (*tk_CreateSelHandler) (Tk_Window tkwin, Atom selection, Atom target, Tk_SelectionProc *proc, ClientData clientData, Atom format); 
    Tk_Window (*tk_CreateWindow) (Tcl_Interp *interp, Tk_Window parent, CONST char *name, CONST char *screenName); 
    Tk_Window (*tk_CreateWindowFromPath) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *pathName, CONST char *screenName); 
    int (*tk_DefineBitmap) (Tcl_Interp *interp, CONST char *name, CONST char *source, int width, int height); 
    void (*tk_DefineCursor) (Tk_Window window, Tk_Cursor cursor); 
    void (*tk_DeleteAllBindings) (Tk_BindingTable bindingTable, ClientData object); 
    int (*tk_DeleteBinding) (Tcl_Interp *interp, Tk_BindingTable bindingTable, ClientData object, CONST char *eventStr); 
    void (*tk_DeleteBindingTable) (Tk_BindingTable bindingTable); 
    void (*tk_DeleteErrorHandler) (Tk_ErrorHandler handler); 
    void (*tk_DeleteEventHandler) (Tk_Window token, unsigned long mask, Tk_EventProc *proc, ClientData clientData); 
    void (*tk_DeleteGenericHandler) (Tk_GenericProc *proc, ClientData clientData); 
    void (*tk_DeleteImage) (Tcl_Interp *interp, CONST char *name); 
    void (*tk_DeleteSelHandler) (Tk_Window tkwin, Atom selection, Atom target); 
    void (*tk_DestroyWindow) (Tk_Window tkwin); 
    CONST84_RETURN char * (*tk_DisplayName) (Tk_Window tkwin); 
    int (*tk_DistanceToTextLayout) (Tk_TextLayout layout, int x, int y); 
    void (*tk_Draw3DPolygon) (Tk_Window tkwin, Drawable drawable, Tk_3DBorder border, XPoint *pointPtr, int numPoints, int borderWidth, int leftRelief); 
    void (*tk_Draw3DRectangle) (Tk_Window tkwin, Drawable drawable, Tk_3DBorder border, int x, int y, int width, int height, int borderWidth, int relief); 
    void (*tk_DrawChars) (Display *display, Drawable drawable, GC gc, Tk_Font tkfont, CONST char *source, int numBytes, int x, int y); 
    void (*tk_DrawFocusHighlight) (Tk_Window tkwin, GC gc, int width, Drawable drawable); 
    void (*tk_DrawTextLayout) (Display *display, Drawable drawable, GC gc, Tk_TextLayout layout, int x, int y, int firstChar, int lastChar); 
    void (*tk_Fill3DPolygon) (Tk_Window tkwin, Drawable drawable, Tk_3DBorder border, XPoint *pointPtr, int numPoints, int borderWidth, int leftRelief); 
    void (*tk_Fill3DRectangle) (Tk_Window tkwin, Drawable drawable, Tk_3DBorder border, int x, int y, int width, int height, int borderWidth, int relief); 
    Tk_PhotoHandle (*tk_FindPhoto) (Tcl_Interp *interp, CONST char *imageName); 
    Font (*tk_FontId) (Tk_Font font); 
    void (*tk_Free3DBorder) (Tk_3DBorder border); 
    void (*tk_FreeBitmap) (Display *display, Pixmap bitmap); 
    void (*tk_FreeColor) (XColor *colorPtr); 
    void (*tk_FreeColormap) (Display *display, Colormap colormap); 
    void (*tk_FreeCursor) (Display *display, Tk_Cursor cursor); 
    void (*tk_FreeFont) (Tk_Font f); 
    void (*tk_FreeGC) (Display *display, GC gc); 
    void (*tk_FreeImage) (Tk_Image image); 
    void (*tk_FreeOptions) (Tk_ConfigSpec *specs, char *widgRec, Display *display, int needFlags); 
    void (*tk_FreePixmap) (Display *display, Pixmap pixmap); 
    void (*tk_FreeTextLayout) (Tk_TextLayout textLayout); 
    void (*tk_FreeXId) (Display *display, XID xid); 
    GC (*tk_GCForColor) (XColor *colorPtr, Drawable drawable); 
    void (*tk_GeometryRequest) (Tk_Window tkwin, int reqWidth, int reqHeight); 
    Tk_3DBorder (*tk_Get3DBorder) (Tcl_Interp *interp, Tk_Window tkwin, Tk_Uid colorName); 
    void (*tk_GetAllBindings) (Tcl_Interp *interp, Tk_BindingTable bindingTable, ClientData object); 
    int (*tk_GetAnchor) (Tcl_Interp *interp, CONST char *str, Tk_Anchor *anchorPtr); 
    CONST84_RETURN char * (*tk_GetAtomName) (Tk_Window tkwin, Atom atom); 
    CONST84_RETURN char * (*tk_GetBinding) (Tcl_Interp *interp, Tk_BindingTable bindingTable, ClientData object, CONST char *eventStr); 
    Pixmap (*tk_GetBitmap) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *str); 
    Pixmap (*tk_GetBitmapFromData) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *source, int width, int height); 
    int (*tk_GetCapStyle) (Tcl_Interp *interp, CONST char *str, int *capPtr); 
    XColor * (*tk_GetColor) (Tcl_Interp *interp, Tk_Window tkwin, Tk_Uid name); 
    XColor * (*tk_GetColorByValue) (Tk_Window tkwin, XColor *colorPtr); 
    Colormap (*tk_GetColormap) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *str); 
    Tk_Cursor (*tk_GetCursor) (Tcl_Interp *interp, Tk_Window tkwin, Tk_Uid str); 
    Tk_Cursor (*tk_GetCursorFromData) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *source, CONST char *mask, int width, int height, int xHot, int yHot, Tk_Uid fg, Tk_Uid bg); 
    Tk_Font (*tk_GetFont) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *str); 
    Tk_Font (*tk_GetFontFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    void (*tk_GetFontMetrics) (Tk_Font font, Tk_FontMetrics *fmPtr); 
    GC (*tk_GetGC) (Tk_Window tkwin, unsigned long valueMask, XGCValues *valuePtr); 
    Tk_Image (*tk_GetImage) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *name, Tk_ImageChangedProc *changeProc, ClientData clientData); 
    ClientData (*tk_GetImageMasterData) (Tcl_Interp *interp, CONST char *name, Tk_ImageType **typePtrPtr); 
    Tk_ItemType * (*tk_GetItemTypes) (void); 
    int (*tk_GetJoinStyle) (Tcl_Interp *interp, CONST char *str, int *joinPtr); 
    int (*tk_GetJustify) (Tcl_Interp *interp, CONST char *str, Tk_Justify *justifyPtr); 
    int (*tk_GetNumMainWindows) (void); 
    Tk_Uid (*tk_GetOption) (Tk_Window tkwin, CONST char *name, CONST char *className); 
    int (*tk_GetPixels) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *str, int *intPtr); 
    Pixmap (*tk_GetPixmap) (Display *display, Drawable d, int width, int height, int depth); 
    int (*tk_GetRelief) (Tcl_Interp *interp, CONST char *name, int *reliefPtr); 
    void (*tk_GetRootCoords) (Tk_Window tkwin, int *xPtr, int *yPtr); 
    int (*tk_GetScrollInfo) (Tcl_Interp *interp, int argc, CONST84 char **argv, double *dblPtr, int *intPtr); 
    int (*tk_GetScreenMM) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *str, double *doublePtr); 
    int (*tk_GetSelection) (Tcl_Interp *interp, Tk_Window tkwin, Atom selection, Atom target, Tk_GetSelProc *proc, ClientData clientData); 
    Tk_Uid (*tk_GetUid) (CONST char *str); 
    Visual * (*tk_GetVisual) (Tcl_Interp *interp, Tk_Window tkwin, CONST char *str, int *depthPtr, Colormap *colormapPtr); 
    void (*tk_GetVRootGeometry) (Tk_Window tkwin, int *xPtr, int *yPtr, int *widthPtr, int *heightPtr); 
    int (*tk_Grab) (Tcl_Interp *interp, Tk_Window tkwin, int grabGlobal); 
    void (*tk_HandleEvent) (XEvent *eventPtr); 
    Tk_Window (*tk_IdToWindow) (Display *display, Window window); 
    void (*tk_ImageChanged) (Tk_ImageMaster master, int x, int y, int width, int height, int imageWidth, int imageHeight); 
    int (*tk_Init) (Tcl_Interp *interp); 
    Atom (*tk_InternAtom) (Tk_Window tkwin, CONST char *name); 
    int (*tk_IntersectTextLayout) (Tk_TextLayout layout, int x, int y, int width, int height); 
    void (*tk_MaintainGeometry) (Tk_Window slave, Tk_Window master, int x, int y, int width, int height); 
    Tk_Window (*tk_MainWindow) (Tcl_Interp *interp); 
    void (*tk_MakeWindowExist) (Tk_Window tkwin); 
    void (*tk_ManageGeometry) (Tk_Window tkwin, CONST Tk_GeomMgr *mgrPtr, ClientData clientData); 
    void (*tk_MapWindow) (Tk_Window tkwin); 
    int (*tk_MeasureChars) (Tk_Font tkfont, CONST char *source, int numBytes, int maxPixels, int flags, int *lengthPtr); 
    void (*tk_MoveResizeWindow) (Tk_Window tkwin, int x, int y, int width, int height); 
    void (*tk_MoveWindow) (Tk_Window tkwin, int x, int y); 
    void (*tk_MoveToplevelWindow) (Tk_Window tkwin, int x, int y); 
    CONST84_RETURN char * (*tk_NameOf3DBorder) (Tk_3DBorder border); 
    CONST84_RETURN char * (*tk_NameOfAnchor) (Tk_Anchor anchor); 
    CONST84_RETURN char * (*tk_NameOfBitmap) (Display *display, Pixmap bitmap); 
    CONST84_RETURN char * (*tk_NameOfCapStyle) (int cap); 
    CONST84_RETURN char * (*tk_NameOfColor) (XColor *colorPtr); 
    CONST84_RETURN char * (*tk_NameOfCursor) (Display *display, Tk_Cursor cursor); 
    CONST84_RETURN char * (*tk_NameOfFont) (Tk_Font font); 
    CONST84_RETURN char * (*tk_NameOfImage) (Tk_ImageMaster imageMaster); 
    CONST84_RETURN char * (*tk_NameOfJoinStyle) (int join); 
    CONST84_RETURN char * (*tk_NameOfJustify) (Tk_Justify justify); 
    CONST84_RETURN char * (*tk_NameOfRelief) (int relief); 
    Tk_Window (*tk_NameToWindow) (Tcl_Interp *interp, CONST char *pathName, Tk_Window tkwin); 
    void (*tk_OwnSelection) (Tk_Window tkwin, Atom selection, Tk_LostSelProc *proc, ClientData clientData); 
    int (*tk_ParseArgv) (Tcl_Interp *interp, Tk_Window tkwin, int *argcPtr, CONST84 char **argv, Tk_ArgvInfo *argTable, int flags); 
    void (*tk_PhotoPutBlock_NoComposite) (Tk_PhotoHandle handle, Tk_PhotoImageBlock *blockPtr, int x, int y, int width, int height); 
    void (*tk_PhotoPutZoomedBlock_NoComposite) (Tk_PhotoHandle handle, Tk_PhotoImageBlock *blockPtr, int x, int y, int width, int height, int zoomX, int zoomY, int subsampleX, int subsampleY); 
    int (*tk_PhotoGetImage) (Tk_PhotoHandle handle, Tk_PhotoImageBlock *blockPtr); 
    void (*tk_PhotoBlank) (Tk_PhotoHandle handle); 
    void (*tk_PhotoExpand_Panic) (Tk_PhotoHandle handle, int width, int height); 
    void (*tk_PhotoGetSize) (Tk_PhotoHandle handle, int *widthPtr, int *heightPtr); 
    void (*tk_PhotoSetSize_Panic) (Tk_PhotoHandle handle, int width, int height); 
    int (*tk_PointToChar) (Tk_TextLayout layout, int x, int y); 
    int (*tk_PostscriptFontName) (Tk_Font tkfont, Tcl_DString *dsPtr); 
    void (*tk_PreserveColormap) (Display *display, Colormap colormap); 
    void (*tk_QueueWindowEvent) (XEvent *eventPtr, Tcl_QueuePosition position); 
    void (*tk_RedrawImage) (Tk_Image image, int imageX, int imageY, int width, int height, Drawable drawable, int drawableX, int drawableY); 
    void (*tk_ResizeWindow) (Tk_Window tkwin, int width, int height); 
    int (*tk_RestackWindow) (Tk_Window tkwin, int aboveBelow, Tk_Window other); 
    Tk_RestrictProc * (*tk_RestrictEvents) (Tk_RestrictProc *proc, ClientData arg, ClientData *prevArgPtr); 
    int (*tk_SafeInit) (Tcl_Interp *interp); 
    CONST char * (*tk_SetAppName) (Tk_Window tkwin, CONST char *name); 
    void (*tk_SetBackgroundFromBorder) (Tk_Window tkwin, Tk_3DBorder border); 
    void (*tk_SetClass) (Tk_Window tkwin, CONST char *className); 
    void (*tk_SetGrid) (Tk_Window tkwin, int reqWidth, int reqHeight, int gridWidth, int gridHeight); 
    void (*tk_SetInternalBorder) (Tk_Window tkwin, int width); 
    void (*tk_SetWindowBackground) (Tk_Window tkwin, unsigned long pixel); 
    void (*tk_SetWindowBackgroundPixmap) (Tk_Window tkwin, Pixmap pixmap); 
    void (*tk_SetWindowBorder) (Tk_Window tkwin, unsigned long pixel); 
    void (*tk_SetWindowBorderWidth) (Tk_Window tkwin, int width); 
    void (*tk_SetWindowBorderPixmap) (Tk_Window tkwin, Pixmap pixmap); 
    void (*tk_SetWindowColormap) (Tk_Window tkwin, Colormap colormap); 
    int (*tk_SetWindowVisual) (Tk_Window tkwin, Visual *visual, int depth, Colormap colormap); 
    void (*tk_SizeOfBitmap) (Display *display, Pixmap bitmap, int *widthPtr, int *heightPtr); 
    void (*tk_SizeOfImage) (Tk_Image image, int *widthPtr, int *heightPtr); 
    int (*tk_StrictMotif) (Tk_Window tkwin); 
    void (*tk_TextLayoutToPostscript) (Tcl_Interp *interp, Tk_TextLayout layout); 
    int (*tk_TextWidth) (Tk_Font font, CONST char *str, int numBytes); 
    void (*tk_UndefineCursor) (Tk_Window window); 
    void (*tk_UnderlineChars) (Display *display, Drawable drawable, GC gc, Tk_Font tkfont, CONST char *source, int x, int y, int firstByte, int lastByte); 
    void (*tk_UnderlineTextLayout) (Display *display, Drawable drawable, GC gc, Tk_TextLayout layout, int x, int y, int underline); 
    void (*tk_Ungrab) (Tk_Window tkwin); 
    void (*tk_UnmaintainGeometry) (Tk_Window slave, Tk_Window master); 
    void (*tk_UnmapWindow) (Tk_Window tkwin); 
    void (*tk_UnsetGrid) (Tk_Window tkwin); 
    void (*tk_UpdatePointer) (Tk_Window tkwin, int x, int y, int state); 
    Pixmap (*tk_AllocBitmapFromObj) (Tcl_Interp *interp, Tk_Window tkwin, Tcl_Obj *objPtr); 
    Tk_3DBorder (*tk_Alloc3DBorderFromObj) (Tcl_Interp *interp, Tk_Window tkwin, Tcl_Obj *objPtr); 
    XColor * (*tk_AllocColorFromObj) (Tcl_Interp *interp, Tk_Window tkwin, Tcl_Obj *objPtr); 
    Tk_Cursor (*tk_AllocCursorFromObj) (Tcl_Interp *interp, Tk_Window tkwin, Tcl_Obj *objPtr); 
    Tk_Font (*tk_AllocFontFromObj) (Tcl_Interp *interp, Tk_Window tkwin, Tcl_Obj *objPtr); 
    Tk_OptionTable (*tk_CreateOptionTable) (Tcl_Interp *interp, CONST Tk_OptionSpec *templatePtr); 
    void (*tk_DeleteOptionTable) (Tk_OptionTable optionTable); 
    void (*tk_Free3DBorderFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    void (*tk_FreeBitmapFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    void (*tk_FreeColorFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    void (*tk_FreeConfigOptions) (char *recordPtr, Tk_OptionTable optionToken, Tk_Window tkwin); 
    void (*tk_FreeSavedOptions) (Tk_SavedOptions *savePtr); 
    void (*tk_FreeCursorFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    void (*tk_FreeFontFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    Tk_3DBorder (*tk_Get3DBorderFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    int (*tk_GetAnchorFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, Tk_Anchor *anchorPtr); 
    Pixmap (*tk_GetBitmapFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    XColor * (*tk_GetColorFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    Tk_Cursor (*tk_GetCursorFromObj) (Tk_Window tkwin, Tcl_Obj *objPtr); 
    Tcl_Obj * (*tk_GetOptionInfo) (Tcl_Interp *interp, char *recordPtr, Tk_OptionTable optionTable, Tcl_Obj *namePtr, Tk_Window tkwin); 
    Tcl_Obj * (*tk_GetOptionValue) (Tcl_Interp *interp, char *recordPtr, Tk_OptionTable optionTable, Tcl_Obj *namePtr, Tk_Window tkwin); 
    int (*tk_GetJustifyFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, Tk_Justify *justifyPtr); 
    int (*tk_GetMMFromObj) (Tcl_Interp *interp, Tk_Window tkwin, Tcl_Obj *objPtr, double *doublePtr); 
    int (*tk_GetPixelsFromObj) (Tcl_Interp *interp, Tk_Window tkwin, Tcl_Obj *objPtr, int *intPtr); 
    int (*tk_GetReliefFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *resultPtr); 
    int (*tk_GetScrollInfoObj) (Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], double *dblPtr, int *intPtr); 
    int (*tk_InitOptions) (Tcl_Interp *interp, char *recordPtr, Tk_OptionTable optionToken, Tk_Window tkwin); 
    void (*tk_MainEx) (int argc, char **argv, Tcl_AppInitProc *appInitProc, Tcl_Interp *interp); 
    void (*tk_RestoreSavedOptions) (Tk_SavedOptions *savePtr); 
    int (*tk_SetOptions) (Tcl_Interp *interp, char *recordPtr, Tk_OptionTable optionTable, int objc, Tcl_Obj *CONST objv[], Tk_Window tkwin, Tk_SavedOptions *savePtr, int *maskPtr); 
    void (*tk_InitConsoleChannels) (Tcl_Interp *interp); 
    int (*tk_CreateConsoleWindow) (Tcl_Interp *interp); 
    void (*tk_CreateSmoothMethod) (Tcl_Interp *interp, Tk_SmoothMethod *method); 
    VOID *reserved218;
    VOID *reserved219;
    int (*tk_GetDash) (Tcl_Interp *interp, CONST char *value, Tk_Dash *dash); 
    void (*tk_CreateOutline) (Tk_Outline *outline); 
    void (*tk_DeleteOutline) (Display *display, Tk_Outline *outline); 
    int (*tk_ConfigOutlineGC) (XGCValues *gcValues, Tk_Canvas canvas, Tk_Item *item, Tk_Outline *outline); 
    int (*tk_ChangeOutlineGC) (Tk_Canvas canvas, Tk_Item *item, Tk_Outline *outline); 
    int (*tk_ResetOutlineGC) (Tk_Canvas canvas, Tk_Item *item, Tk_Outline *outline); 
    int (*tk_CanvasPsOutline) (Tk_Canvas canvas, Tk_Item *item, Tk_Outline *outline); 
    void (*tk_SetTSOrigin) (Tk_Window tkwin, GC gc, int x, int y); 
    int (*tk_CanvasGetCoordFromObj) (Tcl_Interp *interp, Tk_Canvas canvas, Tcl_Obj *obj, double *doublePtr); 
    void (*tk_CanvasSetOffset) (Tk_Canvas canvas, GC gc, Tk_TSOffset *offset); 
    void (*tk_DitherPhoto) (Tk_PhotoHandle handle, int x, int y, int width, int height); 
    int (*tk_PostscriptBitmap) (Tcl_Interp *interp, Tk_Window tkwin, Tk_PostscriptInfo psInfo, Pixmap bitmap, int startX, int startY, int width, int height); 
    int (*tk_PostscriptColor) (Tcl_Interp *interp, Tk_PostscriptInfo psInfo, XColor *colorPtr); 
    int (*tk_PostscriptFont) (Tcl_Interp *interp, Tk_PostscriptInfo psInfo, Tk_Font font); 
    int (*tk_PostscriptImage) (Tk_Image image, Tcl_Interp *interp, Tk_Window tkwin, Tk_PostscriptInfo psinfo, int x, int y, int width, int height, int prepass); 
    void (*tk_PostscriptPath) (Tcl_Interp *interp, Tk_PostscriptInfo psInfo, double *coordPtr, int numPoints); 
    int (*tk_PostscriptStipple) (Tcl_Interp *interp, Tk_Window tkwin, Tk_PostscriptInfo psInfo, Pixmap bitmap); 
    double (*tk_PostscriptY) (double y, Tk_PostscriptInfo psInfo); 
    int (*tk_PostscriptPhoto) (Tcl_Interp *interp, Tk_PhotoImageBlock *blockPtr, Tk_PostscriptInfo psInfo, int width, int height); 
    void (*tk_CreateClientMessageHandler) (Tk_ClientMessageProc *proc); 
    void (*tk_DeleteClientMessageHandler) (Tk_ClientMessageProc *proc); 
    Tk_Window (*tk_CreateAnonymousWindow) (Tcl_Interp *interp, Tk_Window parent, CONST char *screenName); 
    void (*tk_SetClassProcs) (Tk_Window tkwin, Tk_ClassProcs *procs, ClientData instanceData); 
    void (*tk_SetInternalBorderEx) (Tk_Window tkwin, int left, int right, int top, int bottom); 
    void (*tk_SetMinimumRequestSize) (Tk_Window tkwin, int minWidth, int minHeight); 
    void (*tk_SetCaretPos) (Tk_Window tkwin, int x, int y, int height); 
    void (*tk_PhotoPutBlock_Panic) (Tk_PhotoHandle handle, Tk_PhotoImageBlock *blockPtr, int x, int y, int width, int height, int compRule); 
    void (*tk_PhotoPutZoomedBlock_Panic) (Tk_PhotoHandle handle, Tk_PhotoImageBlock *blockPtr, int x, int y, int width, int height, int zoomX, int zoomY, int subsampleX, int subsampleY, int compRule); 
    int (*tk_CollapseMotionEvents) (Display *display, int collapse); 
    Tk_StyleEngine (*tk_RegisterStyleEngine) (CONST char *name, Tk_StyleEngine parent); 
    Tk_StyleEngine (*tk_GetStyleEngine) (CONST char *name); 
    int (*tk_RegisterStyledElement) (Tk_StyleEngine engine, Tk_ElementSpec *templatePtr); 
    int (*tk_GetElementId) (CONST char *name); 
    Tk_Style (*tk_CreateStyle) (CONST char *name, Tk_StyleEngine engine, ClientData clientData); 
    Tk_Style (*tk_GetStyle) (Tcl_Interp *interp, CONST char *name); 
    void (*tk_FreeStyle) (Tk_Style style); 
    CONST char * (*tk_NameOfStyle) (Tk_Style style); 
    Tk_Style (*tk_AllocStyleFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr); 
    Tk_Style (*tk_GetStyleFromObj) (Tcl_Obj *objPtr); 
    void (*tk_FreeStyleFromObj) (Tcl_Obj *objPtr); 
    Tk_StyledElement (*tk_GetStyledElement) (Tk_Style style, int elementId, Tk_OptionTable optionTable); 
    void (*tk_GetElementSize) (Tk_Style style, Tk_StyledElement element, char *recordPtr, Tk_Window tkwin, int width, int height, int inner, int *widthPtr, int *heightPtr); 
    void (*tk_GetElementBox) (Tk_Style style, Tk_StyledElement element, char *recordPtr, Tk_Window tkwin, int x, int y, int width, int height, int inner, int *xPtr, int *yPtr, int *widthPtr, int *heightPtr); 
    int (*tk_GetElementBorderWidth) (Tk_Style style, Tk_StyledElement element, char *recordPtr, Tk_Window tkwin); 
    void (*tk_DrawElement) (Tk_Style style, Tk_StyledElement element, char *recordPtr, Tk_Window tkwin, Drawable d, int x, int y, int width, int height, int state); 
    int (*tk_PhotoExpand) (Tcl_Interp *interp, Tk_PhotoHandle handle, int width, int height); 
    int (*tk_PhotoPutBlock) (Tcl_Interp *interp, Tk_PhotoHandle handle, Tk_PhotoImageBlock *blockPtr, int x, int y, int width, int height, int compRule); 
    int (*tk_PhotoPutZoomedBlock) (Tcl_Interp *interp, Tk_PhotoHandle handle, Tk_PhotoImageBlock *blockPtr, int x, int y, int width, int height, int zoomX, int zoomY, int subsampleX, int subsampleY, int compRule); 
    int (*tk_PhotoSetSize) (Tcl_Interp *interp, Tk_PhotoHandle handle, int width, int height); 
    long (*tk_GetUserInactiveTime) (Display *dpy); 
    void (*tk_ResetUserInactiveTime) (Display *dpy); 
    Tcl_Interp * (*tk_Interp) (Tk_Window tkwin); 
    void (*tk_CreateOldImageType) (Tk_ImageType *typePtr); 
    void (*tk_CreateOldPhotoImageFormat) (Tk_PhotoImageFormat *formatPtr); 
    VOID *reserved274;
    void (*tkUnusedStubEntry) (void); 
} TkStubs;

extern TkStubs *tkStubsPtr;

#ifdef __cplusplus
}
#endif

#if defined(USE_TK_STUBS) && !defined(USE_TK_STUB_PROCS)



#ifndef Tk_MainLoop
#define Tk_MainLoop \
	(tkStubsPtr->tk_MainLoop) 
#endif
#ifndef Tk_3DBorderColor
#define Tk_3DBorderColor \
	(tkStubsPtr->tk_3DBorderColor) 
#endif
#ifndef Tk_3DBorderGC
#define Tk_3DBorderGC \
	(tkStubsPtr->tk_3DBorderGC) 
#endif
#ifndef Tk_3DHorizontalBevel
#define Tk_3DHorizontalBevel \
	(tkStubsPtr->tk_3DHorizontalBevel) 
#endif
#ifndef Tk_3DVerticalBevel
#define Tk_3DVerticalBevel \
	(tkStubsPtr->tk_3DVerticalBevel) 
#endif
#ifndef Tk_AddOption
#define Tk_AddOption \
	(tkStubsPtr->tk_AddOption) 
#endif
#ifndef Tk_BindEvent
#define Tk_BindEvent \
	(tkStubsPtr->tk_BindEvent) 
#endif
#ifndef Tk_CanvasDrawableCoords
#define Tk_CanvasDrawableCoords \
	(tkStubsPtr->tk_CanvasDrawableCoords) 
#endif
#ifndef Tk_CanvasEventuallyRedraw
#define Tk_CanvasEventuallyRedraw \
	(tkStubsPtr->tk_CanvasEventuallyRedraw) 
#endif
#ifndef Tk_CanvasGetCoord
#define Tk_CanvasGetCoord \
	(tkStubsPtr->tk_CanvasGetCoord) 
#endif
#ifndef Tk_CanvasGetTextInfo
#define Tk_CanvasGetTextInfo \
	(tkStubsPtr->tk_CanvasGetTextInfo) 
#endif
#ifndef Tk_CanvasPsBitmap
#define Tk_CanvasPsBitmap \
	(tkStubsPtr->tk_CanvasPsBitmap) 
#endif
#ifndef Tk_CanvasPsColor
#define Tk_CanvasPsColor \
	(tkStubsPtr->tk_CanvasPsColor) 
#endif
#ifndef Tk_CanvasPsFont
#define Tk_CanvasPsFont \
	(tkStubsPtr->tk_CanvasPsFont) 
#endif
#ifndef Tk_CanvasPsPath
#define Tk_CanvasPsPath \
	(tkStubsPtr->tk_CanvasPsPath) 
#endif
#ifndef Tk_CanvasPsStipple
#define Tk_CanvasPsStipple \
	(tkStubsPtr->tk_CanvasPsStipple) 
#endif
#ifndef Tk_CanvasPsY
#define Tk_CanvasPsY \
	(tkStubsPtr->tk_CanvasPsY) 
#endif
#ifndef Tk_CanvasSetStippleOrigin
#define Tk_CanvasSetStippleOrigin \
	(tkStubsPtr->tk_CanvasSetStippleOrigin) 
#endif
#ifndef Tk_CanvasTagsParseProc
#define Tk_CanvasTagsParseProc \
	(tkStubsPtr->tk_CanvasTagsParseProc) 
#endif
#ifndef Tk_CanvasTagsPrintProc
#define Tk_CanvasTagsPrintProc \
	(tkStubsPtr->tk_CanvasTagsPrintProc) 
#endif
#ifndef Tk_CanvasTkwin
#define Tk_CanvasTkwin \
	(tkStubsPtr->tk_CanvasTkwin) 
#endif
#ifndef Tk_CanvasWindowCoords
#define Tk_CanvasWindowCoords \
	(tkStubsPtr->tk_CanvasWindowCoords) 
#endif
#ifndef Tk_ChangeWindowAttributes
#define Tk_ChangeWindowAttributes \
	(tkStubsPtr->tk_ChangeWindowAttributes) 
#endif
#ifndef Tk_CharBbox
#define Tk_CharBbox \
	(tkStubsPtr->tk_CharBbox) 
#endif
#ifndef Tk_ClearSelection
#define Tk_ClearSelection \
	(tkStubsPtr->tk_ClearSelection) 
#endif
#ifndef Tk_ClipboardAppend
#define Tk_ClipboardAppend \
	(tkStubsPtr->tk_ClipboardAppend) 
#endif
#ifndef Tk_ClipboardClear
#define Tk_ClipboardClear \
	(tkStubsPtr->tk_ClipboardClear) 
#endif
#ifndef Tk_ConfigureInfo
#define Tk_ConfigureInfo \
	(tkStubsPtr->tk_ConfigureInfo) 
#endif
#ifndef Tk_ConfigureValue
#define Tk_ConfigureValue \
	(tkStubsPtr->tk_ConfigureValue) 
#endif
#ifndef Tk_ConfigureWidget
#define Tk_ConfigureWidget \
	(tkStubsPtr->tk_ConfigureWidget) 
#endif
#ifndef Tk_ConfigureWindow
#define Tk_ConfigureWindow \
	(tkStubsPtr->tk_ConfigureWindow) 
#endif
#ifndef Tk_ComputeTextLayout
#define Tk_ComputeTextLayout \
	(tkStubsPtr->tk_ComputeTextLayout) 
#endif
#ifndef Tk_CoordsToWindow
#define Tk_CoordsToWindow \
	(tkStubsPtr->tk_CoordsToWindow) 
#endif
#ifndef Tk_CreateBinding
#define Tk_CreateBinding \
	(tkStubsPtr->tk_CreateBinding) 
#endif
#ifndef Tk_CreateBindingTable
#define Tk_CreateBindingTable \
	(tkStubsPtr->tk_CreateBindingTable) 
#endif
#ifndef Tk_CreateErrorHandler
#define Tk_CreateErrorHandler \
	(tkStubsPtr->tk_CreateErrorHandler) 
#endif
#ifndef Tk_CreateEventHandler
#define Tk_CreateEventHandler \
	(tkStubsPtr->tk_CreateEventHandler) 
#endif
#ifndef Tk_CreateGenericHandler
#define Tk_CreateGenericHandler \
	(tkStubsPtr->tk_CreateGenericHandler) 
#endif
#ifndef Tk_CreateImageType
#define Tk_CreateImageType \
	(tkStubsPtr->tk_CreateImageType) 
#endif
#ifndef Tk_CreateItemType
#define Tk_CreateItemType \
	(tkStubsPtr->tk_CreateItemType) 
#endif
#ifndef Tk_CreatePhotoImageFormat
#define Tk_CreatePhotoImageFormat \
	(tkStubsPtr->tk_CreatePhotoImageFormat) 
#endif
#ifndef Tk_CreateSelHandler
#define Tk_CreateSelHandler \
	(tkStubsPtr->tk_CreateSelHandler) 
#endif
#ifndef Tk_CreateWindow
#define Tk_CreateWindow \
	(tkStubsPtr->tk_CreateWindow) 
#endif
#ifndef Tk_CreateWindowFromPath
#define Tk_CreateWindowFromPath \
	(tkStubsPtr->tk_CreateWindowFromPath) 
#endif
#ifndef Tk_DefineBitmap
#define Tk_DefineBitmap \
	(tkStubsPtr->tk_DefineBitmap) 
#endif
#ifndef Tk_DefineCursor
#define Tk_DefineCursor \
	(tkStubsPtr->tk_DefineCursor) 
#endif
#ifndef Tk_DeleteAllBindings
#define Tk_DeleteAllBindings \
	(tkStubsPtr->tk_DeleteAllBindings) 
#endif
#ifndef Tk_DeleteBinding
#define Tk_DeleteBinding \
	(tkStubsPtr->tk_DeleteBinding) 
#endif
#ifndef Tk_DeleteBindingTable
#define Tk_DeleteBindingTable \
	(tkStubsPtr->tk_DeleteBindingTable) 
#endif
#ifndef Tk_DeleteErrorHandler
#define Tk_DeleteErrorHandler \
	(tkStubsPtr->tk_DeleteErrorHandler) 
#endif
#ifndef Tk_DeleteEventHandler
#define Tk_DeleteEventHandler \
	(tkStubsPtr->tk_DeleteEventHandler) 
#endif
#ifndef Tk_DeleteGenericHandler
#define Tk_DeleteGenericHandler \
	(tkStubsPtr->tk_DeleteGenericHandler) 
#endif
#ifndef Tk_DeleteImage
#define Tk_DeleteImage \
	(tkStubsPtr->tk_DeleteImage) 
#endif
#ifndef Tk_DeleteSelHandler
#define Tk_DeleteSelHandler \
	(tkStubsPtr->tk_DeleteSelHandler) 
#endif
#ifndef Tk_DestroyWindow
#define Tk_DestroyWindow \
	(tkStubsPtr->tk_DestroyWindow) 
#endif
#ifndef Tk_DisplayName
#define Tk_DisplayName \
	(tkStubsPtr->tk_DisplayName) 
#endif
#ifndef Tk_DistanceToTextLayout
#define Tk_DistanceToTextLayout \
	(tkStubsPtr->tk_DistanceToTextLayout) 
#endif
#ifndef Tk_Draw3DPolygon
#define Tk_Draw3DPolygon \
	(tkStubsPtr->tk_Draw3DPolygon) 
#endif
#ifndef Tk_Draw3DRectangle
#define Tk_Draw3DRectangle \
	(tkStubsPtr->tk_Draw3DRectangle) 
#endif
#ifndef Tk_DrawChars
#define Tk_DrawChars \
	(tkStubsPtr->tk_DrawChars) 
#endif
#ifndef Tk_DrawFocusHighlight
#define Tk_DrawFocusHighlight \
	(tkStubsPtr->tk_DrawFocusHighlight) 
#endif
#ifndef Tk_DrawTextLayout
#define Tk_DrawTextLayout \
	(tkStubsPtr->tk_DrawTextLayout) 
#endif
#ifndef Tk_Fill3DPolygon
#define Tk_Fill3DPolygon \
	(tkStubsPtr->tk_Fill3DPolygon) 
#endif
#ifndef Tk_Fill3DRectangle
#define Tk_Fill3DRectangle \
	(tkStubsPtr->tk_Fill3DRectangle) 
#endif
#ifndef Tk_FindPhoto
#define Tk_FindPhoto \
	(tkStubsPtr->tk_FindPhoto) 
#endif
#ifndef Tk_FontId
#define Tk_FontId \
	(tkStubsPtr->tk_FontId) 
#endif
#ifndef Tk_Free3DBorder
#define Tk_Free3DBorder \
	(tkStubsPtr->tk_Free3DBorder) 
#endif
#ifndef Tk_FreeBitmap
#define Tk_FreeBitmap \
	(tkStubsPtr->tk_FreeBitmap) 
#endif
#ifndef Tk_FreeColor
#define Tk_FreeColor \
	(tkStubsPtr->tk_FreeColor) 
#endif
#ifndef Tk_FreeColormap
#define Tk_FreeColormap \
	(tkStubsPtr->tk_FreeColormap) 
#endif
#ifndef Tk_FreeCursor
#define Tk_FreeCursor \
	(tkStubsPtr->tk_FreeCursor) 
#endif
#ifndef Tk_FreeFont
#define Tk_FreeFont \
	(tkStubsPtr->tk_FreeFont) 
#endif
#ifndef Tk_FreeGC
#define Tk_FreeGC \
	(tkStubsPtr->tk_FreeGC) 
#endif
#ifndef Tk_FreeImage
#define Tk_FreeImage \
	(tkStubsPtr->tk_FreeImage) 
#endif
#ifndef Tk_FreeOptions
#define Tk_FreeOptions \
	(tkStubsPtr->tk_FreeOptions) 
#endif
#ifndef Tk_FreePixmap
#define Tk_FreePixmap \
	(tkStubsPtr->tk_FreePixmap) 
#endif
#ifndef Tk_FreeTextLayout
#define Tk_FreeTextLayout \
	(tkStubsPtr->tk_FreeTextLayout) 
#endif
#ifndef Tk_FreeXId
#define Tk_FreeXId \
	(tkStubsPtr->tk_FreeXId) 
#endif
#ifndef Tk_GCForColor
#define Tk_GCForColor \
	(tkStubsPtr->tk_GCForColor) 
#endif
#ifndef Tk_GeometryRequest
#define Tk_GeometryRequest \
	(tkStubsPtr->tk_GeometryRequest) 
#endif
#ifndef Tk_Get3DBorder
#define Tk_Get3DBorder \
	(tkStubsPtr->tk_Get3DBorder) 
#endif
#ifndef Tk_GetAllBindings
#define Tk_GetAllBindings \
	(tkStubsPtr->tk_GetAllBindings) 
#endif
#ifndef Tk_GetAnchor
#define Tk_GetAnchor \
	(tkStubsPtr->tk_GetAnchor) 
#endif
#ifndef Tk_GetAtomName
#define Tk_GetAtomName \
	(tkStubsPtr->tk_GetAtomName) 
#endif
#ifndef Tk_GetBinding
#define Tk_GetBinding \
	(tkStubsPtr->tk_GetBinding) 
#endif
#ifndef Tk_GetBitmap
#define Tk_GetBitmap \
	(tkStubsPtr->tk_GetBitmap) 
#endif
#ifndef Tk_GetBitmapFromData
#define Tk_GetBitmapFromData \
	(tkStubsPtr->tk_GetBitmapFromData) 
#endif
#ifndef Tk_GetCapStyle
#define Tk_GetCapStyle \
	(tkStubsPtr->tk_GetCapStyle) 
#endif
#ifndef Tk_GetColor
#define Tk_GetColor \
	(tkStubsPtr->tk_GetColor) 
#endif
#ifndef Tk_GetColorByValue
#define Tk_GetColorByValue \
	(tkStubsPtr->tk_GetColorByValue) 
#endif
#ifndef Tk_GetColormap
#define Tk_GetColormap \
	(tkStubsPtr->tk_GetColormap) 
#endif
#ifndef Tk_GetCursor
#define Tk_GetCursor \
	(tkStubsPtr->tk_GetCursor) 
#endif
#ifndef Tk_GetCursorFromData
#define Tk_GetCursorFromData \
	(tkStubsPtr->tk_GetCursorFromData) 
#endif
#ifndef Tk_GetFont
#define Tk_GetFont \
	(tkStubsPtr->tk_GetFont) 
#endif
#ifndef Tk_GetFontFromObj
#define Tk_GetFontFromObj \
	(tkStubsPtr->tk_GetFontFromObj) 
#endif
#ifndef Tk_GetFontMetrics
#define Tk_GetFontMetrics \
	(tkStubsPtr->tk_GetFontMetrics) 
#endif
#ifndef Tk_GetGC
#define Tk_GetGC \
	(tkStubsPtr->tk_GetGC) 
#endif
#ifndef Tk_GetImage
#define Tk_GetImage \
	(tkStubsPtr->tk_GetImage) 
#endif
#ifndef Tk_GetImageMasterData
#define Tk_GetImageMasterData \
	(tkStubsPtr->tk_GetImageMasterData) 
#endif
#ifndef Tk_GetItemTypes
#define Tk_GetItemTypes \
	(tkStubsPtr->tk_GetItemTypes) 
#endif
#ifndef Tk_GetJoinStyle
#define Tk_GetJoinStyle \
	(tkStubsPtr->tk_GetJoinStyle) 
#endif
#ifndef Tk_GetJustify
#define Tk_GetJustify \
	(tkStubsPtr->tk_GetJustify) 
#endif
#ifndef Tk_GetNumMainWindows
#define Tk_GetNumMainWindows \
	(tkStubsPtr->tk_GetNumMainWindows) 
#endif
#ifndef Tk_GetOption
#define Tk_GetOption \
	(tkStubsPtr->tk_GetOption) 
#endif
#ifndef Tk_GetPixels
#define Tk_GetPixels \
	(tkStubsPtr->tk_GetPixels) 
#endif
#ifndef Tk_GetPixmap
#define Tk_GetPixmap \
	(tkStubsPtr->tk_GetPixmap) 
#endif
#ifndef Tk_GetRelief
#define Tk_GetRelief \
	(tkStubsPtr->tk_GetRelief) 
#endif
#ifndef Tk_GetRootCoords
#define Tk_GetRootCoords \
	(tkStubsPtr->tk_GetRootCoords) 
#endif
#ifndef Tk_GetScrollInfo
#define Tk_GetScrollInfo \
	(tkStubsPtr->tk_GetScrollInfo) 
#endif
#ifndef Tk_GetScreenMM
#define Tk_GetScreenMM \
	(tkStubsPtr->tk_GetScreenMM) 
#endif
#ifndef Tk_GetSelection
#define Tk_GetSelection \
	(tkStubsPtr->tk_GetSelection) 
#endif
#ifndef Tk_GetUid
#define Tk_GetUid \
	(tkStubsPtr->tk_GetUid) 
#endif
#ifndef Tk_GetVisual
#define Tk_GetVisual \
	(tkStubsPtr->tk_GetVisual) 
#endif
#ifndef Tk_GetVRootGeometry
#define Tk_GetVRootGeometry \
	(tkStubsPtr->tk_GetVRootGeometry) 
#endif
#ifndef Tk_Grab
#define Tk_Grab \
	(tkStubsPtr->tk_Grab) 
#endif
#ifndef Tk_HandleEvent
#define Tk_HandleEvent \
	(tkStubsPtr->tk_HandleEvent) 
#endif
#ifndef Tk_IdToWindow
#define Tk_IdToWindow \
	(tkStubsPtr->tk_IdToWindow) 
#endif
#ifndef Tk_ImageChanged
#define Tk_ImageChanged \
	(tkStubsPtr->tk_ImageChanged) 
#endif
#ifndef Tk_Init
#define Tk_Init \
	(tkStubsPtr->tk_Init) 
#endif
#ifndef Tk_InternAtom
#define Tk_InternAtom \
	(tkStubsPtr->tk_InternAtom) 
#endif
#ifndef Tk_IntersectTextLayout
#define Tk_IntersectTextLayout \
	(tkStubsPtr->tk_IntersectTextLayout) 
#endif
#ifndef Tk_MaintainGeometry
#define Tk_MaintainGeometry \
	(tkStubsPtr->tk_MaintainGeometry) 
#endif
#ifndef Tk_MainWindow
#define Tk_MainWindow \
	(tkStubsPtr->tk_MainWindow) 
#endif
#ifndef Tk_MakeWindowExist
#define Tk_MakeWindowExist \
	(tkStubsPtr->tk_MakeWindowExist) 
#endif
#ifndef Tk_ManageGeometry
#define Tk_ManageGeometry \
	(tkStubsPtr->tk_ManageGeometry) 
#endif
#ifndef Tk_MapWindow
#define Tk_MapWindow \
	(tkStubsPtr->tk_MapWindow) 
#endif
#ifndef Tk_MeasureChars
#define Tk_MeasureChars \
	(tkStubsPtr->tk_MeasureChars) 
#endif
#ifndef Tk_MoveResizeWindow
#define Tk_MoveResizeWindow \
	(tkStubsPtr->tk_MoveResizeWindow) 
#endif
#ifndef Tk_MoveWindow
#define Tk_MoveWindow \
	(tkStubsPtr->tk_MoveWindow) 
#endif
#ifndef Tk_MoveToplevelWindow
#define Tk_MoveToplevelWindow \
	(tkStubsPtr->tk_MoveToplevelWindow) 
#endif
#ifndef Tk_NameOf3DBorder
#define Tk_NameOf3DBorder \
	(tkStubsPtr->tk_NameOf3DBorder) 
#endif
#ifndef Tk_NameOfAnchor
#define Tk_NameOfAnchor \
	(tkStubsPtr->tk_NameOfAnchor) 
#endif
#ifndef Tk_NameOfBitmap
#define Tk_NameOfBitmap \
	(tkStubsPtr->tk_NameOfBitmap) 
#endif
#ifndef Tk_NameOfCapStyle
#define Tk_NameOfCapStyle \
	(tkStubsPtr->tk_NameOfCapStyle) 
#endif
#ifndef Tk_NameOfColor
#define Tk_NameOfColor \
	(tkStubsPtr->tk_NameOfColor) 
#endif
#ifndef Tk_NameOfCursor
#define Tk_NameOfCursor \
	(tkStubsPtr->tk_NameOfCursor) 
#endif
#ifndef Tk_NameOfFont
#define Tk_NameOfFont \
	(tkStubsPtr->tk_NameOfFont) 
#endif
#ifndef Tk_NameOfImage
#define Tk_NameOfImage \
	(tkStubsPtr->tk_NameOfImage) 
#endif
#ifndef Tk_NameOfJoinStyle
#define Tk_NameOfJoinStyle \
	(tkStubsPtr->tk_NameOfJoinStyle) 
#endif
#ifndef Tk_NameOfJustify
#define Tk_NameOfJustify \
	(tkStubsPtr->tk_NameOfJustify) 
#endif
#ifndef Tk_NameOfRelief
#define Tk_NameOfRelief \
	(tkStubsPtr->tk_NameOfRelief) 
#endif
#ifndef Tk_NameToWindow
#define Tk_NameToWindow \
	(tkStubsPtr->tk_NameToWindow) 
#endif
#ifndef Tk_OwnSelection
#define Tk_OwnSelection \
	(tkStubsPtr->tk_OwnSelection) 
#endif
#ifndef Tk_ParseArgv
#define Tk_ParseArgv \
	(tkStubsPtr->tk_ParseArgv) 
#endif
#ifndef Tk_PhotoPutBlock_NoComposite
#define Tk_PhotoPutBlock_NoComposite \
	(tkStubsPtr->tk_PhotoPutBlock_NoComposite) 
#endif
#ifndef Tk_PhotoPutZoomedBlock_NoComposite
#define Tk_PhotoPutZoomedBlock_NoComposite \
	(tkStubsPtr->tk_PhotoPutZoomedBlock_NoComposite) 
#endif
#ifndef Tk_PhotoGetImage
#define Tk_PhotoGetImage \
	(tkStubsPtr->tk_PhotoGetImage) 
#endif
#ifndef Tk_PhotoBlank
#define Tk_PhotoBlank \
	(tkStubsPtr->tk_PhotoBlank) 
#endif
#ifndef Tk_PhotoExpand_Panic
#define Tk_PhotoExpand_Panic \
	(tkStubsPtr->tk_PhotoExpand_Panic) 
#endif
#ifndef Tk_PhotoGetSize
#define Tk_PhotoGetSize \
	(tkStubsPtr->tk_PhotoGetSize) 
#endif
#ifndef Tk_PhotoSetSize_Panic
#define Tk_PhotoSetSize_Panic \
	(tkStubsPtr->tk_PhotoSetSize_Panic) 
#endif
#ifndef Tk_PointToChar
#define Tk_PointToChar \
	(tkStubsPtr->tk_PointToChar) 
#endif
#ifndef Tk_PostscriptFontName
#define Tk_PostscriptFontName \
	(tkStubsPtr->tk_PostscriptFontName) 
#endif
#ifndef Tk_PreserveColormap
#define Tk_PreserveColormap \
	(tkStubsPtr->tk_PreserveColormap) 
#endif
#ifndef Tk_QueueWindowEvent
#define Tk_QueueWindowEvent \
	(tkStubsPtr->tk_QueueWindowEvent) 
#endif
#ifndef Tk_RedrawImage
#define Tk_RedrawImage \
	(tkStubsPtr->tk_RedrawImage) 
#endif
#ifndef Tk_ResizeWindow
#define Tk_ResizeWindow \
	(tkStubsPtr->tk_ResizeWindow) 
#endif
#ifndef Tk_RestackWindow
#define Tk_RestackWindow \
	(tkStubsPtr->tk_RestackWindow) 
#endif
#ifndef Tk_RestrictEvents
#define Tk_RestrictEvents \
	(tkStubsPtr->tk_RestrictEvents) 
#endif
#ifndef Tk_SafeInit
#define Tk_SafeInit \
	(tkStubsPtr->tk_SafeInit) 
#endif
#ifndef Tk_SetAppName
#define Tk_SetAppName \
	(tkStubsPtr->tk_SetAppName) 
#endif
#ifndef Tk_SetBackgroundFromBorder
#define Tk_SetBackgroundFromBorder \
	(tkStubsPtr->tk_SetBackgroundFromBorder) 
#endif
#ifndef Tk_SetClass
#define Tk_SetClass \
	(tkStubsPtr->tk_SetClass) 
#endif
#ifndef Tk_SetGrid
#define Tk_SetGrid \
	(tkStubsPtr->tk_SetGrid) 
#endif
#ifndef Tk_SetInternalBorder
#define Tk_SetInternalBorder \
	(tkStubsPtr->tk_SetInternalBorder) 
#endif
#ifndef Tk_SetWindowBackground
#define Tk_SetWindowBackground \
	(tkStubsPtr->tk_SetWindowBackground) 
#endif
#ifndef Tk_SetWindowBackgroundPixmap
#define Tk_SetWindowBackgroundPixmap \
	(tkStubsPtr->tk_SetWindowBackgroundPixmap) 
#endif
#ifndef Tk_SetWindowBorder
#define Tk_SetWindowBorder \
	(tkStubsPtr->tk_SetWindowBorder) 
#endif
#ifndef Tk_SetWindowBorderWidth
#define Tk_SetWindowBorderWidth \
	(tkStubsPtr->tk_SetWindowBorderWidth) 
#endif
#ifndef Tk_SetWindowBorderPixmap
#define Tk_SetWindowBorderPixmap \
	(tkStubsPtr->tk_SetWindowBorderPixmap) 
#endif
#ifndef Tk_SetWindowColormap
#define Tk_SetWindowColormap \
	(tkStubsPtr->tk_SetWindowColormap) 
#endif
#ifndef Tk_SetWindowVisual
#define Tk_SetWindowVisual \
	(tkStubsPtr->tk_SetWindowVisual) 
#endif
#ifndef Tk_SizeOfBitmap
#define Tk_SizeOfBitmap \
	(tkStubsPtr->tk_SizeOfBitmap) 
#endif
#ifndef Tk_SizeOfImage
#define Tk_SizeOfImage \
	(tkStubsPtr->tk_SizeOfImage) 
#endif
#ifndef Tk_StrictMotif
#define Tk_StrictMotif \
	(tkStubsPtr->tk_StrictMotif) 
#endif
#ifndef Tk_TextLayoutToPostscript
#define Tk_TextLayoutToPostscript \
	(tkStubsPtr->tk_TextLayoutToPostscript) 
#endif
#ifndef Tk_TextWidth
#define Tk_TextWidth \
	(tkStubsPtr->tk_TextWidth) 
#endif
#ifndef Tk_UndefineCursor
#define Tk_UndefineCursor \
	(tkStubsPtr->tk_UndefineCursor) 
#endif
#ifndef Tk_UnderlineChars
#define Tk_UnderlineChars \
	(tkStubsPtr->tk_UnderlineChars) 
#endif
#ifndef Tk_UnderlineTextLayout
#define Tk_UnderlineTextLayout \
	(tkStubsPtr->tk_UnderlineTextLayout) 
#endif
#ifndef Tk_Ungrab
#define Tk_Ungrab \
	(tkStubsPtr->tk_Ungrab) 
#endif
#ifndef Tk_UnmaintainGeometry
#define Tk_UnmaintainGeometry \
	(tkStubsPtr->tk_UnmaintainGeometry) 
#endif
#ifndef Tk_UnmapWindow
#define Tk_UnmapWindow \
	(tkStubsPtr->tk_UnmapWindow) 
#endif
#ifndef Tk_UnsetGrid
#define Tk_UnsetGrid \
	(tkStubsPtr->tk_UnsetGrid) 
#endif
#ifndef Tk_UpdatePointer
#define Tk_UpdatePointer \
	(tkStubsPtr->tk_UpdatePointer) 
#endif
#ifndef Tk_AllocBitmapFromObj
#define Tk_AllocBitmapFromObj \
	(tkStubsPtr->tk_AllocBitmapFromObj) 
#endif
#ifndef Tk_Alloc3DBorderFromObj
#define Tk_Alloc3DBorderFromObj \
	(tkStubsPtr->tk_Alloc3DBorderFromObj) 
#endif
#ifndef Tk_AllocColorFromObj
#define Tk_AllocColorFromObj \
	(tkStubsPtr->tk_AllocColorFromObj) 
#endif
#ifndef Tk_AllocCursorFromObj
#define Tk_AllocCursorFromObj \
	(tkStubsPtr->tk_AllocCursorFromObj) 
#endif
#ifndef Tk_AllocFontFromObj
#define Tk_AllocFontFromObj \
	(tkStubsPtr->tk_AllocFontFromObj) 
#endif
#ifndef Tk_CreateOptionTable
#define Tk_CreateOptionTable \
	(tkStubsPtr->tk_CreateOptionTable) 
#endif
#ifndef Tk_DeleteOptionTable
#define Tk_DeleteOptionTable \
	(tkStubsPtr->tk_DeleteOptionTable) 
#endif
#ifndef Tk_Free3DBorderFromObj
#define Tk_Free3DBorderFromObj \
	(tkStubsPtr->tk_Free3DBorderFromObj) 
#endif
#ifndef Tk_FreeBitmapFromObj
#define Tk_FreeBitmapFromObj \
	(tkStubsPtr->tk_FreeBitmapFromObj) 
#endif
#ifndef Tk_FreeColorFromObj
#define Tk_FreeColorFromObj \
	(tkStubsPtr->tk_FreeColorFromObj) 
#endif
#ifndef Tk_FreeConfigOptions
#define Tk_FreeConfigOptions \
	(tkStubsPtr->tk_FreeConfigOptions) 
#endif
#ifndef Tk_FreeSavedOptions
#define Tk_FreeSavedOptions \
	(tkStubsPtr->tk_FreeSavedOptions) 
#endif
#ifndef Tk_FreeCursorFromObj
#define Tk_FreeCursorFromObj \
	(tkStubsPtr->tk_FreeCursorFromObj) 
#endif
#ifndef Tk_FreeFontFromObj
#define Tk_FreeFontFromObj \
	(tkStubsPtr->tk_FreeFontFromObj) 
#endif
#ifndef Tk_Get3DBorderFromObj
#define Tk_Get3DBorderFromObj \
	(tkStubsPtr->tk_Get3DBorderFromObj) 
#endif
#ifndef Tk_GetAnchorFromObj
#define Tk_GetAnchorFromObj \
	(tkStubsPtr->tk_GetAnchorFromObj) 
#endif
#ifndef Tk_GetBitmapFromObj
#define Tk_GetBitmapFromObj \
	(tkStubsPtr->tk_GetBitmapFromObj) 
#endif
#ifndef Tk_GetColorFromObj
#define Tk_GetColorFromObj \
	(tkStubsPtr->tk_GetColorFromObj) 
#endif
#ifndef Tk_GetCursorFromObj
#define Tk_GetCursorFromObj \
	(tkStubsPtr->tk_GetCursorFromObj) 
#endif
#ifndef Tk_GetOptionInfo
#define Tk_GetOptionInfo \
	(tkStubsPtr->tk_GetOptionInfo) 
#endif
#ifndef Tk_GetOptionValue
#define Tk_GetOptionValue \
	(tkStubsPtr->tk_GetOptionValue) 
#endif
#ifndef Tk_GetJustifyFromObj
#define Tk_GetJustifyFromObj \
	(tkStubsPtr->tk_GetJustifyFromObj) 
#endif
#ifndef Tk_GetMMFromObj
#define Tk_GetMMFromObj \
	(tkStubsPtr->tk_GetMMFromObj) 
#endif
#ifndef Tk_GetPixelsFromObj
#define Tk_GetPixelsFromObj \
	(tkStubsPtr->tk_GetPixelsFromObj) 
#endif
#ifndef Tk_GetReliefFromObj
#define Tk_GetReliefFromObj \
	(tkStubsPtr->tk_GetReliefFromObj) 
#endif
#ifndef Tk_GetScrollInfoObj
#define Tk_GetScrollInfoObj \
	(tkStubsPtr->tk_GetScrollInfoObj) 
#endif
#ifndef Tk_InitOptions
#define Tk_InitOptions \
	(tkStubsPtr->tk_InitOptions) 
#endif
#ifndef Tk_MainEx
#define Tk_MainEx \
	(tkStubsPtr->tk_MainEx) 
#endif
#ifndef Tk_RestoreSavedOptions
#define Tk_RestoreSavedOptions \
	(tkStubsPtr->tk_RestoreSavedOptions) 
#endif
#ifndef Tk_SetOptions
#define Tk_SetOptions \
	(tkStubsPtr->tk_SetOptions) 
#endif
#ifndef Tk_InitConsoleChannels
#define Tk_InitConsoleChannels \
	(tkStubsPtr->tk_InitConsoleChannels) 
#endif
#ifndef Tk_CreateConsoleWindow
#define Tk_CreateConsoleWindow \
	(tkStubsPtr->tk_CreateConsoleWindow) 
#endif
#ifndef Tk_CreateSmoothMethod
#define Tk_CreateSmoothMethod \
	(tkStubsPtr->tk_CreateSmoothMethod) 
#endif


#ifndef Tk_GetDash
#define Tk_GetDash \
	(tkStubsPtr->tk_GetDash) 
#endif
#ifndef Tk_CreateOutline
#define Tk_CreateOutline \
	(tkStubsPtr->tk_CreateOutline) 
#endif
#ifndef Tk_DeleteOutline
#define Tk_DeleteOutline \
	(tkStubsPtr->tk_DeleteOutline) 
#endif
#ifndef Tk_ConfigOutlineGC
#define Tk_ConfigOutlineGC \
	(tkStubsPtr->tk_ConfigOutlineGC) 
#endif
#ifndef Tk_ChangeOutlineGC
#define Tk_ChangeOutlineGC \
	(tkStubsPtr->tk_ChangeOutlineGC) 
#endif
#ifndef Tk_ResetOutlineGC
#define Tk_ResetOutlineGC \
	(tkStubsPtr->tk_ResetOutlineGC) 
#endif
#ifndef Tk_CanvasPsOutline
#define Tk_CanvasPsOutline \
	(tkStubsPtr->tk_CanvasPsOutline) 
#endif
#ifndef Tk_SetTSOrigin
#define Tk_SetTSOrigin \
	(tkStubsPtr->tk_SetTSOrigin) 
#endif
#ifndef Tk_CanvasGetCoordFromObj
#define Tk_CanvasGetCoordFromObj \
	(tkStubsPtr->tk_CanvasGetCoordFromObj) 
#endif
#ifndef Tk_CanvasSetOffset
#define Tk_CanvasSetOffset \
	(tkStubsPtr->tk_CanvasSetOffset) 
#endif
#ifndef Tk_DitherPhoto
#define Tk_DitherPhoto \
	(tkStubsPtr->tk_DitherPhoto) 
#endif
#ifndef Tk_PostscriptBitmap
#define Tk_PostscriptBitmap \
	(tkStubsPtr->tk_PostscriptBitmap) 
#endif
#ifndef Tk_PostscriptColor
#define Tk_PostscriptColor \
	(tkStubsPtr->tk_PostscriptColor) 
#endif
#ifndef Tk_PostscriptFont
#define Tk_PostscriptFont \
	(tkStubsPtr->tk_PostscriptFont) 
#endif
#ifndef Tk_PostscriptImage
#define Tk_PostscriptImage \
	(tkStubsPtr->tk_PostscriptImage) 
#endif
#ifndef Tk_PostscriptPath
#define Tk_PostscriptPath \
	(tkStubsPtr->tk_PostscriptPath) 
#endif
#ifndef Tk_PostscriptStipple
#define Tk_PostscriptStipple \
	(tkStubsPtr->tk_PostscriptStipple) 
#endif
#ifndef Tk_PostscriptY
#define Tk_PostscriptY \
	(tkStubsPtr->tk_PostscriptY) 
#endif
#ifndef Tk_PostscriptPhoto
#define Tk_PostscriptPhoto \
	(tkStubsPtr->tk_PostscriptPhoto) 
#endif
#ifndef Tk_CreateClientMessageHandler
#define Tk_CreateClientMessageHandler \
	(tkStubsPtr->tk_CreateClientMessageHandler) 
#endif
#ifndef Tk_DeleteClientMessageHandler
#define Tk_DeleteClientMessageHandler \
	(tkStubsPtr->tk_DeleteClientMessageHandler) 
#endif
#ifndef Tk_CreateAnonymousWindow
#define Tk_CreateAnonymousWindow \
	(tkStubsPtr->tk_CreateAnonymousWindow) 
#endif
#ifndef Tk_SetClassProcs
#define Tk_SetClassProcs \
	(tkStubsPtr->tk_SetClassProcs) 
#endif
#ifndef Tk_SetInternalBorderEx
#define Tk_SetInternalBorderEx \
	(tkStubsPtr->tk_SetInternalBorderEx) 
#endif
#ifndef Tk_SetMinimumRequestSize
#define Tk_SetMinimumRequestSize \
	(tkStubsPtr->tk_SetMinimumRequestSize) 
#endif
#ifndef Tk_SetCaretPos
#define Tk_SetCaretPos \
	(tkStubsPtr->tk_SetCaretPos) 
#endif
#ifndef Tk_PhotoPutBlock_Panic
#define Tk_PhotoPutBlock_Panic \
	(tkStubsPtr->tk_PhotoPutBlock_Panic) 
#endif
#ifndef Tk_PhotoPutZoomedBlock_Panic
#define Tk_PhotoPutZoomedBlock_Panic \
	(tkStubsPtr->tk_PhotoPutZoomedBlock_Panic) 
#endif
#ifndef Tk_CollapseMotionEvents
#define Tk_CollapseMotionEvents \
	(tkStubsPtr->tk_CollapseMotionEvents) 
#endif
#ifndef Tk_RegisterStyleEngine
#define Tk_RegisterStyleEngine \
	(tkStubsPtr->tk_RegisterStyleEngine) 
#endif
#ifndef Tk_GetStyleEngine
#define Tk_GetStyleEngine \
	(tkStubsPtr->tk_GetStyleEngine) 
#endif
#ifndef Tk_RegisterStyledElement
#define Tk_RegisterStyledElement \
	(tkStubsPtr->tk_RegisterStyledElement) 
#endif
#ifndef Tk_GetElementId
#define Tk_GetElementId \
	(tkStubsPtr->tk_GetElementId) 
#endif
#ifndef Tk_CreateStyle
#define Tk_CreateStyle \
	(tkStubsPtr->tk_CreateStyle) 
#endif
#ifndef Tk_GetStyle
#define Tk_GetStyle \
	(tkStubsPtr->tk_GetStyle) 
#endif
#ifndef Tk_FreeStyle
#define Tk_FreeStyle \
	(tkStubsPtr->tk_FreeStyle) 
#endif
#ifndef Tk_NameOfStyle
#define Tk_NameOfStyle \
	(tkStubsPtr->tk_NameOfStyle) 
#endif
#ifndef Tk_AllocStyleFromObj
#define Tk_AllocStyleFromObj \
	(tkStubsPtr->tk_AllocStyleFromObj) 
#endif
#ifndef Tk_GetStyleFromObj
#define Tk_GetStyleFromObj \
	(tkStubsPtr->tk_GetStyleFromObj) 
#endif
#ifndef Tk_FreeStyleFromObj
#define Tk_FreeStyleFromObj \
	(tkStubsPtr->tk_FreeStyleFromObj) 
#endif
#ifndef Tk_GetStyledElement
#define Tk_GetStyledElement \
	(tkStubsPtr->tk_GetStyledElement) 
#endif
#ifndef Tk_GetElementSize
#define Tk_GetElementSize \
	(tkStubsPtr->tk_GetElementSize) 
#endif
#ifndef Tk_GetElementBox
#define Tk_GetElementBox \
	(tkStubsPtr->tk_GetElementBox) 
#endif
#ifndef Tk_GetElementBorderWidth
#define Tk_GetElementBorderWidth \
	(tkStubsPtr->tk_GetElementBorderWidth) 
#endif
#ifndef Tk_DrawElement
#define Tk_DrawElement \
	(tkStubsPtr->tk_DrawElement) 
#endif
#ifndef Tk_PhotoExpand
#define Tk_PhotoExpand \
	(tkStubsPtr->tk_PhotoExpand) 
#endif
#ifndef Tk_PhotoPutBlock
#define Tk_PhotoPutBlock \
	(tkStubsPtr->tk_PhotoPutBlock) 
#endif
#ifndef Tk_PhotoPutZoomedBlock
#define Tk_PhotoPutZoomedBlock \
	(tkStubsPtr->tk_PhotoPutZoomedBlock) 
#endif
#ifndef Tk_PhotoSetSize
#define Tk_PhotoSetSize \
	(tkStubsPtr->tk_PhotoSetSize) 
#endif
#ifndef Tk_GetUserInactiveTime
#define Tk_GetUserInactiveTime \
	(tkStubsPtr->tk_GetUserInactiveTime) 
#endif
#ifndef Tk_ResetUserInactiveTime
#define Tk_ResetUserInactiveTime \
	(tkStubsPtr->tk_ResetUserInactiveTime) 
#endif
#ifndef Tk_Interp
#define Tk_Interp \
	(tkStubsPtr->tk_Interp) 
#endif
#ifndef Tk_CreateOldImageType
#define Tk_CreateOldImageType \
	(tkStubsPtr->tk_CreateOldImageType) 
#endif
#ifndef Tk_CreateOldPhotoImageFormat
#define Tk_CreateOldPhotoImageFormat \
	(tkStubsPtr->tk_CreateOldPhotoImageFormat) 
#endif

#ifndef TkUnusedStubEntry
#define TkUnusedStubEntry \
	(tkStubsPtr->tkUnusedStubEntry) 
#endif

#endif 



#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT

#undef TkUnusedStubEntry

#endif 

