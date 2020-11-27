

#ifndef _TCLDECLS
#define _TCLDECLS

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





#ifdef __cplusplus
extern "C" {
#endif



#ifndef Tcl_PkgProvideEx_TCL_DECLARED
#define Tcl_PkgProvideEx_TCL_DECLARED

EXTERN int		Tcl_PkgProvideEx(Tcl_Interp *interp,
				CONST char *name, CONST char *version,
				ClientData clientData);
#endif
#ifndef Tcl_PkgRequireEx_TCL_DECLARED
#define Tcl_PkgRequireEx_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_PkgRequireEx(Tcl_Interp *interp,
				CONST char *name, CONST char *version,
				int exact, ClientData *clientDataPtr);
#endif
#ifndef Tcl_Panic_TCL_DECLARED
#define Tcl_Panic_TCL_DECLARED

EXTERN void		Tcl_Panic(CONST char *format, ...);
#endif
#ifndef Tcl_Alloc_TCL_DECLARED
#define Tcl_Alloc_TCL_DECLARED

EXTERN char *		Tcl_Alloc(unsigned int size);
#endif
#ifndef Tcl_Free_TCL_DECLARED
#define Tcl_Free_TCL_DECLARED

EXTERN void		Tcl_Free(char *ptr);
#endif
#ifndef Tcl_Realloc_TCL_DECLARED
#define Tcl_Realloc_TCL_DECLARED

EXTERN char *		Tcl_Realloc(char *ptr, unsigned int size);
#endif
#ifndef Tcl_DbCkalloc_TCL_DECLARED
#define Tcl_DbCkalloc_TCL_DECLARED

EXTERN char *		Tcl_DbCkalloc(unsigned int size, CONST char *file,
				int line);
#endif
#ifndef Tcl_DbCkfree_TCL_DECLARED
#define Tcl_DbCkfree_TCL_DECLARED

EXTERN void		Tcl_DbCkfree(char *ptr, CONST char *file, int line);
#endif
#ifndef Tcl_DbCkrealloc_TCL_DECLARED
#define Tcl_DbCkrealloc_TCL_DECLARED

EXTERN char *		Tcl_DbCkrealloc(char *ptr, unsigned int size,
				CONST char *file, int line);
#endif
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
#ifndef Tcl_CreateFileHandler_TCL_DECLARED
#define Tcl_CreateFileHandler_TCL_DECLARED

EXTERN void		Tcl_CreateFileHandler(int fd, int mask,
				Tcl_FileProc *proc, ClientData clientData);
#endif
#endif 
#ifdef MAC_OSX_TCL 
#ifndef Tcl_CreateFileHandler_TCL_DECLARED
#define Tcl_CreateFileHandler_TCL_DECLARED

EXTERN void		Tcl_CreateFileHandler(int fd, int mask,
				Tcl_FileProc *proc, ClientData clientData);
#endif
#endif 
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
#ifndef Tcl_DeleteFileHandler_TCL_DECLARED
#define Tcl_DeleteFileHandler_TCL_DECLARED

EXTERN void		Tcl_DeleteFileHandler(int fd);
#endif
#endif 
#ifdef MAC_OSX_TCL 
#ifndef Tcl_DeleteFileHandler_TCL_DECLARED
#define Tcl_DeleteFileHandler_TCL_DECLARED

EXTERN void		Tcl_DeleteFileHandler(int fd);
#endif
#endif 
#ifndef Tcl_SetTimer_TCL_DECLARED
#define Tcl_SetTimer_TCL_DECLARED

EXTERN void		Tcl_SetTimer(Tcl_Time *timePtr);
#endif
#ifndef Tcl_Sleep_TCL_DECLARED
#define Tcl_Sleep_TCL_DECLARED

EXTERN void		Tcl_Sleep(int ms);
#endif
#ifndef Tcl_WaitForEvent_TCL_DECLARED
#define Tcl_WaitForEvent_TCL_DECLARED

EXTERN int		Tcl_WaitForEvent(Tcl_Time *timePtr);
#endif
#ifndef Tcl_AppendAllObjTypes_TCL_DECLARED
#define Tcl_AppendAllObjTypes_TCL_DECLARED

EXTERN int		Tcl_AppendAllObjTypes(Tcl_Interp *interp,
				Tcl_Obj *objPtr);
#endif
#ifndef Tcl_AppendStringsToObj_TCL_DECLARED
#define Tcl_AppendStringsToObj_TCL_DECLARED

EXTERN void		Tcl_AppendStringsToObj(Tcl_Obj *objPtr, ...);
#endif
#ifndef Tcl_AppendToObj_TCL_DECLARED
#define Tcl_AppendToObj_TCL_DECLARED

EXTERN void		Tcl_AppendToObj(Tcl_Obj *objPtr, CONST char *bytes,
				int length);
#endif
#ifndef Tcl_ConcatObj_TCL_DECLARED
#define Tcl_ConcatObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_ConcatObj(int objc, Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_ConvertToType_TCL_DECLARED
#define Tcl_ConvertToType_TCL_DECLARED

EXTERN int		Tcl_ConvertToType(Tcl_Interp *interp,
				Tcl_Obj *objPtr, Tcl_ObjType *typePtr);
#endif
#ifndef Tcl_DbDecrRefCount_TCL_DECLARED
#define Tcl_DbDecrRefCount_TCL_DECLARED

EXTERN void		Tcl_DbDecrRefCount(Tcl_Obj *objPtr, CONST char *file,
				int line);
#endif
#ifndef Tcl_DbIncrRefCount_TCL_DECLARED
#define Tcl_DbIncrRefCount_TCL_DECLARED

EXTERN void		Tcl_DbIncrRefCount(Tcl_Obj *objPtr, CONST char *file,
				int line);
#endif
#ifndef Tcl_DbIsShared_TCL_DECLARED
#define Tcl_DbIsShared_TCL_DECLARED

EXTERN int		Tcl_DbIsShared(Tcl_Obj *objPtr, CONST char *file,
				int line);
#endif
#ifndef Tcl_DbNewBooleanObj_TCL_DECLARED
#define Tcl_DbNewBooleanObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewBooleanObj(int boolValue, CONST char *file,
				int line);
#endif
#ifndef Tcl_DbNewByteArrayObj_TCL_DECLARED
#define Tcl_DbNewByteArrayObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewByteArrayObj(CONST unsigned char *bytes,
				int length, CONST char *file, int line);
#endif
#ifndef Tcl_DbNewDoubleObj_TCL_DECLARED
#define Tcl_DbNewDoubleObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewDoubleObj(double doubleValue,
				CONST char *file, int line);
#endif
#ifndef Tcl_DbNewListObj_TCL_DECLARED
#define Tcl_DbNewListObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewListObj(int objc, Tcl_Obj *CONST *objv,
				CONST char *file, int line);
#endif
#ifndef Tcl_DbNewLongObj_TCL_DECLARED
#define Tcl_DbNewLongObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewLongObj(long longValue, CONST char *file,
				int line);
#endif
#ifndef Tcl_DbNewObj_TCL_DECLARED
#define Tcl_DbNewObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewObj(CONST char *file, int line);
#endif
#ifndef Tcl_DbNewStringObj_TCL_DECLARED
#define Tcl_DbNewStringObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewStringObj(CONST char *bytes, int length,
				CONST char *file, int line);
#endif
#ifndef Tcl_DuplicateObj_TCL_DECLARED
#define Tcl_DuplicateObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DuplicateObj(Tcl_Obj *objPtr);
#endif
#ifndef TclFreeObj_TCL_DECLARED
#define TclFreeObj_TCL_DECLARED

EXTERN void		TclFreeObj(Tcl_Obj *objPtr);
#endif
#ifndef Tcl_GetBoolean_TCL_DECLARED
#define Tcl_GetBoolean_TCL_DECLARED

EXTERN int		Tcl_GetBoolean(Tcl_Interp *interp, CONST char *src,
				int *boolPtr);
#endif
#ifndef Tcl_GetBooleanFromObj_TCL_DECLARED
#define Tcl_GetBooleanFromObj_TCL_DECLARED

EXTERN int		Tcl_GetBooleanFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *boolPtr);
#endif
#ifndef Tcl_GetByteArrayFromObj_TCL_DECLARED
#define Tcl_GetByteArrayFromObj_TCL_DECLARED

EXTERN unsigned char *	Tcl_GetByteArrayFromObj(Tcl_Obj *objPtr,
				int *lengthPtr);
#endif
#ifndef Tcl_GetDouble_TCL_DECLARED
#define Tcl_GetDouble_TCL_DECLARED

EXTERN int		Tcl_GetDouble(Tcl_Interp *interp, CONST char *src,
				double *doublePtr);
#endif
#ifndef Tcl_GetDoubleFromObj_TCL_DECLARED
#define Tcl_GetDoubleFromObj_TCL_DECLARED

EXTERN int		Tcl_GetDoubleFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, double *doublePtr);
#endif
#ifndef Tcl_GetIndexFromObj_TCL_DECLARED
#define Tcl_GetIndexFromObj_TCL_DECLARED

EXTERN int		Tcl_GetIndexFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, CONST84 char **tablePtr,
				CONST char *msg, int flags, int *indexPtr);
#endif
#ifndef Tcl_GetInt_TCL_DECLARED
#define Tcl_GetInt_TCL_DECLARED

EXTERN int		Tcl_GetInt(Tcl_Interp *interp, CONST char *src,
				int *intPtr);
#endif
#ifndef Tcl_GetIntFromObj_TCL_DECLARED
#define Tcl_GetIntFromObj_TCL_DECLARED

EXTERN int		Tcl_GetIntFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *intPtr);
#endif
#ifndef Tcl_GetLongFromObj_TCL_DECLARED
#define Tcl_GetLongFromObj_TCL_DECLARED

EXTERN int		Tcl_GetLongFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, long *longPtr);
#endif
#ifndef Tcl_GetObjType_TCL_DECLARED
#define Tcl_GetObjType_TCL_DECLARED

EXTERN Tcl_ObjType *	Tcl_GetObjType(CONST char *typeName);
#endif
#ifndef Tcl_GetStringFromObj_TCL_DECLARED
#define Tcl_GetStringFromObj_TCL_DECLARED

EXTERN char *		Tcl_GetStringFromObj(Tcl_Obj *objPtr, int *lengthPtr);
#endif
#ifndef Tcl_InvalidateStringRep_TCL_DECLARED
#define Tcl_InvalidateStringRep_TCL_DECLARED

EXTERN void		Tcl_InvalidateStringRep(Tcl_Obj *objPtr);
#endif
#ifndef Tcl_ListObjAppendList_TCL_DECLARED
#define Tcl_ListObjAppendList_TCL_DECLARED

EXTERN int		Tcl_ListObjAppendList(Tcl_Interp *interp,
				Tcl_Obj *listPtr, Tcl_Obj *elemListPtr);
#endif
#ifndef Tcl_ListObjAppendElement_TCL_DECLARED
#define Tcl_ListObjAppendElement_TCL_DECLARED

EXTERN int		Tcl_ListObjAppendElement(Tcl_Interp *interp,
				Tcl_Obj *listPtr, Tcl_Obj *objPtr);
#endif
#ifndef Tcl_ListObjGetElements_TCL_DECLARED
#define Tcl_ListObjGetElements_TCL_DECLARED

EXTERN int		Tcl_ListObjGetElements(Tcl_Interp *interp,
				Tcl_Obj *listPtr, int *objcPtr,
				Tcl_Obj ***objvPtr);
#endif
#ifndef Tcl_ListObjIndex_TCL_DECLARED
#define Tcl_ListObjIndex_TCL_DECLARED

EXTERN int		Tcl_ListObjIndex(Tcl_Interp *interp,
				Tcl_Obj *listPtr, int index,
				Tcl_Obj **objPtrPtr);
#endif
#ifndef Tcl_ListObjLength_TCL_DECLARED
#define Tcl_ListObjLength_TCL_DECLARED

EXTERN int		Tcl_ListObjLength(Tcl_Interp *interp,
				Tcl_Obj *listPtr, int *lengthPtr);
#endif
#ifndef Tcl_ListObjReplace_TCL_DECLARED
#define Tcl_ListObjReplace_TCL_DECLARED

EXTERN int		Tcl_ListObjReplace(Tcl_Interp *interp,
				Tcl_Obj *listPtr, int first, int count,
				int objc, Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_NewBooleanObj_TCL_DECLARED
#define Tcl_NewBooleanObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewBooleanObj(int boolValue);
#endif
#ifndef Tcl_NewByteArrayObj_TCL_DECLARED
#define Tcl_NewByteArrayObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewByteArrayObj(CONST unsigned char *bytes,
				int length);
#endif
#ifndef Tcl_NewDoubleObj_TCL_DECLARED
#define Tcl_NewDoubleObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewDoubleObj(double doubleValue);
#endif
#ifndef Tcl_NewIntObj_TCL_DECLARED
#define Tcl_NewIntObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewIntObj(int intValue);
#endif
#ifndef Tcl_NewListObj_TCL_DECLARED
#define Tcl_NewListObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewListObj(int objc, Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_NewLongObj_TCL_DECLARED
#define Tcl_NewLongObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewLongObj(long longValue);
#endif
#ifndef Tcl_NewObj_TCL_DECLARED
#define Tcl_NewObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewObj(void);
#endif
#ifndef Tcl_NewStringObj_TCL_DECLARED
#define Tcl_NewStringObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewStringObj(CONST char *bytes, int length);
#endif
#ifndef Tcl_SetBooleanObj_TCL_DECLARED
#define Tcl_SetBooleanObj_TCL_DECLARED

EXTERN void		Tcl_SetBooleanObj(Tcl_Obj *objPtr, int boolValue);
#endif
#ifndef Tcl_SetByteArrayLength_TCL_DECLARED
#define Tcl_SetByteArrayLength_TCL_DECLARED

EXTERN unsigned char *	Tcl_SetByteArrayLength(Tcl_Obj *objPtr, int length);
#endif
#ifndef Tcl_SetByteArrayObj_TCL_DECLARED
#define Tcl_SetByteArrayObj_TCL_DECLARED

EXTERN void		Tcl_SetByteArrayObj(Tcl_Obj *objPtr,
				CONST unsigned char *bytes, int length);
#endif
#ifndef Tcl_SetDoubleObj_TCL_DECLARED
#define Tcl_SetDoubleObj_TCL_DECLARED

EXTERN void		Tcl_SetDoubleObj(Tcl_Obj *objPtr, double doubleValue);
#endif
#ifndef Tcl_SetIntObj_TCL_DECLARED
#define Tcl_SetIntObj_TCL_DECLARED

EXTERN void		Tcl_SetIntObj(Tcl_Obj *objPtr, int intValue);
#endif
#ifndef Tcl_SetListObj_TCL_DECLARED
#define Tcl_SetListObj_TCL_DECLARED

EXTERN void		Tcl_SetListObj(Tcl_Obj *objPtr, int objc,
				Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_SetLongObj_TCL_DECLARED
#define Tcl_SetLongObj_TCL_DECLARED

EXTERN void		Tcl_SetLongObj(Tcl_Obj *objPtr, long longValue);
#endif
#ifndef Tcl_SetObjLength_TCL_DECLARED
#define Tcl_SetObjLength_TCL_DECLARED

EXTERN void		Tcl_SetObjLength(Tcl_Obj *objPtr, int length);
#endif
#ifndef Tcl_SetStringObj_TCL_DECLARED
#define Tcl_SetStringObj_TCL_DECLARED

EXTERN void		Tcl_SetStringObj(Tcl_Obj *objPtr, CONST char *bytes,
				int length);
#endif
#ifndef Tcl_AddErrorInfo_TCL_DECLARED
#define Tcl_AddErrorInfo_TCL_DECLARED

EXTERN void		Tcl_AddErrorInfo(Tcl_Interp *interp,
				CONST char *message);
#endif
#ifndef Tcl_AddObjErrorInfo_TCL_DECLARED
#define Tcl_AddObjErrorInfo_TCL_DECLARED

EXTERN void		Tcl_AddObjErrorInfo(Tcl_Interp *interp,
				CONST char *message, int length);
#endif
#ifndef Tcl_AllowExceptions_TCL_DECLARED
#define Tcl_AllowExceptions_TCL_DECLARED

EXTERN void		Tcl_AllowExceptions(Tcl_Interp *interp);
#endif
#ifndef Tcl_AppendElement_TCL_DECLARED
#define Tcl_AppendElement_TCL_DECLARED

EXTERN void		Tcl_AppendElement(Tcl_Interp *interp,
				CONST char *element);
#endif
#ifndef Tcl_AppendResult_TCL_DECLARED
#define Tcl_AppendResult_TCL_DECLARED

EXTERN void		Tcl_AppendResult(Tcl_Interp *interp, ...);
#endif
#ifndef Tcl_AsyncCreate_TCL_DECLARED
#define Tcl_AsyncCreate_TCL_DECLARED

EXTERN Tcl_AsyncHandler	 Tcl_AsyncCreate(Tcl_AsyncProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_AsyncDelete_TCL_DECLARED
#define Tcl_AsyncDelete_TCL_DECLARED

EXTERN void		Tcl_AsyncDelete(Tcl_AsyncHandler async);
#endif
#ifndef Tcl_AsyncInvoke_TCL_DECLARED
#define Tcl_AsyncInvoke_TCL_DECLARED

EXTERN int		Tcl_AsyncInvoke(Tcl_Interp *interp, int code);
#endif
#ifndef Tcl_AsyncMark_TCL_DECLARED
#define Tcl_AsyncMark_TCL_DECLARED

EXTERN void		Tcl_AsyncMark(Tcl_AsyncHandler async);
#endif
#ifndef Tcl_AsyncReady_TCL_DECLARED
#define Tcl_AsyncReady_TCL_DECLARED

EXTERN int		Tcl_AsyncReady(void);
#endif
#ifndef Tcl_BackgroundError_TCL_DECLARED
#define Tcl_BackgroundError_TCL_DECLARED

EXTERN void		Tcl_BackgroundError(Tcl_Interp *interp);
#endif
#ifndef Tcl_Backslash_TCL_DECLARED
#define Tcl_Backslash_TCL_DECLARED

EXTERN char		Tcl_Backslash(CONST char *src, int *readPtr);
#endif
#ifndef Tcl_BadChannelOption_TCL_DECLARED
#define Tcl_BadChannelOption_TCL_DECLARED

EXTERN int		Tcl_BadChannelOption(Tcl_Interp *interp,
				CONST char *optionName,
				CONST char *optionList);
#endif
#ifndef Tcl_CallWhenDeleted_TCL_DECLARED
#define Tcl_CallWhenDeleted_TCL_DECLARED

EXTERN void		Tcl_CallWhenDeleted(Tcl_Interp *interp,
				Tcl_InterpDeleteProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_CancelIdleCall_TCL_DECLARED
#define Tcl_CancelIdleCall_TCL_DECLARED

EXTERN void		Tcl_CancelIdleCall(Tcl_IdleProc *idleProc,
				ClientData clientData);
#endif
#ifndef Tcl_Close_TCL_DECLARED
#define Tcl_Close_TCL_DECLARED

EXTERN int		Tcl_Close(Tcl_Interp *interp, Tcl_Channel chan);
#endif
#ifndef Tcl_CommandComplete_TCL_DECLARED
#define Tcl_CommandComplete_TCL_DECLARED

EXTERN int		Tcl_CommandComplete(CONST char *cmd);
#endif
#ifndef Tcl_Concat_TCL_DECLARED
#define Tcl_Concat_TCL_DECLARED

EXTERN char *		Tcl_Concat(int argc, CONST84 char *CONST *argv);
#endif
#ifndef Tcl_ConvertElement_TCL_DECLARED
#define Tcl_ConvertElement_TCL_DECLARED

EXTERN int		Tcl_ConvertElement(CONST char *src, char *dst,
				int flags);
#endif
#ifndef Tcl_ConvertCountedElement_TCL_DECLARED
#define Tcl_ConvertCountedElement_TCL_DECLARED

EXTERN int		Tcl_ConvertCountedElement(CONST char *src,
				int length, char *dst, int flags);
#endif
#ifndef Tcl_CreateAlias_TCL_DECLARED
#define Tcl_CreateAlias_TCL_DECLARED

EXTERN int		Tcl_CreateAlias(Tcl_Interp *slave,
				CONST char *slaveCmd, Tcl_Interp *target,
				CONST char *targetCmd, int argc,
				CONST84 char *CONST *argv);
#endif
#ifndef Tcl_CreateAliasObj_TCL_DECLARED
#define Tcl_CreateAliasObj_TCL_DECLARED

EXTERN int		Tcl_CreateAliasObj(Tcl_Interp *slave,
				CONST char *slaveCmd, Tcl_Interp *target,
				CONST char *targetCmd, int objc,
				Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_CreateChannel_TCL_DECLARED
#define Tcl_CreateChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_CreateChannel(Tcl_ChannelType *typePtr,
				CONST char *chanName,
				ClientData instanceData, int mask);
#endif
#ifndef Tcl_CreateChannelHandler_TCL_DECLARED
#define Tcl_CreateChannelHandler_TCL_DECLARED

EXTERN void		Tcl_CreateChannelHandler(Tcl_Channel chan, int mask,
				Tcl_ChannelProc *proc, ClientData clientData);
#endif
#ifndef Tcl_CreateCloseHandler_TCL_DECLARED
#define Tcl_CreateCloseHandler_TCL_DECLARED

EXTERN void		Tcl_CreateCloseHandler(Tcl_Channel chan,
				Tcl_CloseProc *proc, ClientData clientData);
#endif
#ifndef Tcl_CreateCommand_TCL_DECLARED
#define Tcl_CreateCommand_TCL_DECLARED

EXTERN Tcl_Command	Tcl_CreateCommand(Tcl_Interp *interp,
				CONST char *cmdName, Tcl_CmdProc *proc,
				ClientData clientData,
				Tcl_CmdDeleteProc *deleteProc);
#endif
#ifndef Tcl_CreateEventSource_TCL_DECLARED
#define Tcl_CreateEventSource_TCL_DECLARED

EXTERN void		Tcl_CreateEventSource(Tcl_EventSetupProc *setupProc,
				Tcl_EventCheckProc *checkProc,
				ClientData clientData);
#endif
#ifndef Tcl_CreateExitHandler_TCL_DECLARED
#define Tcl_CreateExitHandler_TCL_DECLARED

EXTERN void		Tcl_CreateExitHandler(Tcl_ExitProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_CreateInterp_TCL_DECLARED
#define Tcl_CreateInterp_TCL_DECLARED

EXTERN Tcl_Interp *	Tcl_CreateInterp(void);
#endif
#ifndef Tcl_CreateMathFunc_TCL_DECLARED
#define Tcl_CreateMathFunc_TCL_DECLARED

EXTERN void		Tcl_CreateMathFunc(Tcl_Interp *interp,
				CONST char *name, int numArgs,
				Tcl_ValueType *argTypes, Tcl_MathProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_CreateObjCommand_TCL_DECLARED
#define Tcl_CreateObjCommand_TCL_DECLARED

EXTERN Tcl_Command	Tcl_CreateObjCommand(Tcl_Interp *interp,
				CONST char *cmdName, Tcl_ObjCmdProc *proc,
				ClientData clientData,
				Tcl_CmdDeleteProc *deleteProc);
#endif
#ifndef Tcl_CreateSlave_TCL_DECLARED
#define Tcl_CreateSlave_TCL_DECLARED

EXTERN Tcl_Interp *	Tcl_CreateSlave(Tcl_Interp *interp,
				CONST char *slaveName, int isSafe);
#endif
#ifndef Tcl_CreateTimerHandler_TCL_DECLARED
#define Tcl_CreateTimerHandler_TCL_DECLARED

EXTERN Tcl_TimerToken	Tcl_CreateTimerHandler(int milliseconds,
				Tcl_TimerProc *proc, ClientData clientData);
#endif
#ifndef Tcl_CreateTrace_TCL_DECLARED
#define Tcl_CreateTrace_TCL_DECLARED

EXTERN Tcl_Trace	Tcl_CreateTrace(Tcl_Interp *interp, int level,
				Tcl_CmdTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_DeleteAssocData_TCL_DECLARED
#define Tcl_DeleteAssocData_TCL_DECLARED

EXTERN void		Tcl_DeleteAssocData(Tcl_Interp *interp,
				CONST char *name);
#endif
#ifndef Tcl_DeleteChannelHandler_TCL_DECLARED
#define Tcl_DeleteChannelHandler_TCL_DECLARED

EXTERN void		Tcl_DeleteChannelHandler(Tcl_Channel chan,
				Tcl_ChannelProc *proc, ClientData clientData);
#endif
#ifndef Tcl_DeleteCloseHandler_TCL_DECLARED
#define Tcl_DeleteCloseHandler_TCL_DECLARED

EXTERN void		Tcl_DeleteCloseHandler(Tcl_Channel chan,
				Tcl_CloseProc *proc, ClientData clientData);
#endif
#ifndef Tcl_DeleteCommand_TCL_DECLARED
#define Tcl_DeleteCommand_TCL_DECLARED

EXTERN int		Tcl_DeleteCommand(Tcl_Interp *interp,
				CONST char *cmdName);
#endif
#ifndef Tcl_DeleteCommandFromToken_TCL_DECLARED
#define Tcl_DeleteCommandFromToken_TCL_DECLARED

EXTERN int		Tcl_DeleteCommandFromToken(Tcl_Interp *interp,
				Tcl_Command command);
#endif
#ifndef Tcl_DeleteEvents_TCL_DECLARED
#define Tcl_DeleteEvents_TCL_DECLARED

EXTERN void		Tcl_DeleteEvents(Tcl_EventDeleteProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_DeleteEventSource_TCL_DECLARED
#define Tcl_DeleteEventSource_TCL_DECLARED

EXTERN void		Tcl_DeleteEventSource(Tcl_EventSetupProc *setupProc,
				Tcl_EventCheckProc *checkProc,
				ClientData clientData);
#endif
#ifndef Tcl_DeleteExitHandler_TCL_DECLARED
#define Tcl_DeleteExitHandler_TCL_DECLARED

EXTERN void		Tcl_DeleteExitHandler(Tcl_ExitProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_DeleteHashEntry_TCL_DECLARED
#define Tcl_DeleteHashEntry_TCL_DECLARED

EXTERN void		Tcl_DeleteHashEntry(Tcl_HashEntry *entryPtr);
#endif
#ifndef Tcl_DeleteHashTable_TCL_DECLARED
#define Tcl_DeleteHashTable_TCL_DECLARED

EXTERN void		Tcl_DeleteHashTable(Tcl_HashTable *tablePtr);
#endif
#ifndef Tcl_DeleteInterp_TCL_DECLARED
#define Tcl_DeleteInterp_TCL_DECLARED

EXTERN void		Tcl_DeleteInterp(Tcl_Interp *interp);
#endif
#ifndef Tcl_DetachPids_TCL_DECLARED
#define Tcl_DetachPids_TCL_DECLARED

EXTERN void		Tcl_DetachPids(int numPids, Tcl_Pid *pidPtr);
#endif
#ifndef Tcl_DeleteTimerHandler_TCL_DECLARED
#define Tcl_DeleteTimerHandler_TCL_DECLARED

EXTERN void		Tcl_DeleteTimerHandler(Tcl_TimerToken token);
#endif
#ifndef Tcl_DeleteTrace_TCL_DECLARED
#define Tcl_DeleteTrace_TCL_DECLARED

EXTERN void		Tcl_DeleteTrace(Tcl_Interp *interp, Tcl_Trace trace);
#endif
#ifndef Tcl_DontCallWhenDeleted_TCL_DECLARED
#define Tcl_DontCallWhenDeleted_TCL_DECLARED

EXTERN void		Tcl_DontCallWhenDeleted(Tcl_Interp *interp,
				Tcl_InterpDeleteProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_DoOneEvent_TCL_DECLARED
#define Tcl_DoOneEvent_TCL_DECLARED

EXTERN int		Tcl_DoOneEvent(int flags);
#endif
#ifndef Tcl_DoWhenIdle_TCL_DECLARED
#define Tcl_DoWhenIdle_TCL_DECLARED

EXTERN void		Tcl_DoWhenIdle(Tcl_IdleProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_DStringAppend_TCL_DECLARED
#define Tcl_DStringAppend_TCL_DECLARED

EXTERN char *		Tcl_DStringAppend(Tcl_DString *dsPtr,
				CONST char *bytes, int length);
#endif
#ifndef Tcl_DStringAppendElement_TCL_DECLARED
#define Tcl_DStringAppendElement_TCL_DECLARED

EXTERN char *		Tcl_DStringAppendElement(Tcl_DString *dsPtr,
				CONST char *element);
#endif
#ifndef Tcl_DStringEndSublist_TCL_DECLARED
#define Tcl_DStringEndSublist_TCL_DECLARED

EXTERN void		Tcl_DStringEndSublist(Tcl_DString *dsPtr);
#endif
#ifndef Tcl_DStringFree_TCL_DECLARED
#define Tcl_DStringFree_TCL_DECLARED

EXTERN void		Tcl_DStringFree(Tcl_DString *dsPtr);
#endif
#ifndef Tcl_DStringGetResult_TCL_DECLARED
#define Tcl_DStringGetResult_TCL_DECLARED

EXTERN void		Tcl_DStringGetResult(Tcl_Interp *interp,
				Tcl_DString *dsPtr);
#endif
#ifndef Tcl_DStringInit_TCL_DECLARED
#define Tcl_DStringInit_TCL_DECLARED

EXTERN void		Tcl_DStringInit(Tcl_DString *dsPtr);
#endif
#ifndef Tcl_DStringResult_TCL_DECLARED
#define Tcl_DStringResult_TCL_DECLARED

EXTERN void		Tcl_DStringResult(Tcl_Interp *interp,
				Tcl_DString *dsPtr);
#endif
#ifndef Tcl_DStringSetLength_TCL_DECLARED
#define Tcl_DStringSetLength_TCL_DECLARED

EXTERN void		Tcl_DStringSetLength(Tcl_DString *dsPtr, int length);
#endif
#ifndef Tcl_DStringStartSublist_TCL_DECLARED
#define Tcl_DStringStartSublist_TCL_DECLARED

EXTERN void		Tcl_DStringStartSublist(Tcl_DString *dsPtr);
#endif
#ifndef Tcl_Eof_TCL_DECLARED
#define Tcl_Eof_TCL_DECLARED

EXTERN int		Tcl_Eof(Tcl_Channel chan);
#endif
#ifndef Tcl_ErrnoId_TCL_DECLARED
#define Tcl_ErrnoId_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_ErrnoId(void);
#endif
#ifndef Tcl_ErrnoMsg_TCL_DECLARED
#define Tcl_ErrnoMsg_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_ErrnoMsg(int err);
#endif
#ifndef Tcl_Eval_TCL_DECLARED
#define Tcl_Eval_TCL_DECLARED

EXTERN int		Tcl_Eval(Tcl_Interp *interp, CONST char *script);
#endif
#ifndef Tcl_EvalFile_TCL_DECLARED
#define Tcl_EvalFile_TCL_DECLARED

EXTERN int		Tcl_EvalFile(Tcl_Interp *interp,
				CONST char *fileName);
#endif
#ifndef Tcl_EvalObj_TCL_DECLARED
#define Tcl_EvalObj_TCL_DECLARED

EXTERN int		Tcl_EvalObj(Tcl_Interp *interp, Tcl_Obj *objPtr);
#endif
#ifndef Tcl_EventuallyFree_TCL_DECLARED
#define Tcl_EventuallyFree_TCL_DECLARED

EXTERN void		Tcl_EventuallyFree(ClientData clientData,
				Tcl_FreeProc *freeProc);
#endif
#ifndef Tcl_Exit_TCL_DECLARED
#define Tcl_Exit_TCL_DECLARED

EXTERN void		Tcl_Exit(int status);
#endif
#ifndef Tcl_ExposeCommand_TCL_DECLARED
#define Tcl_ExposeCommand_TCL_DECLARED

EXTERN int		Tcl_ExposeCommand(Tcl_Interp *interp,
				CONST char *hiddenCmdToken,
				CONST char *cmdName);
#endif
#ifndef Tcl_ExprBoolean_TCL_DECLARED
#define Tcl_ExprBoolean_TCL_DECLARED

EXTERN int		Tcl_ExprBoolean(Tcl_Interp *interp, CONST char *expr,
				int *ptr);
#endif
#ifndef Tcl_ExprBooleanObj_TCL_DECLARED
#define Tcl_ExprBooleanObj_TCL_DECLARED

EXTERN int		Tcl_ExprBooleanObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, int *ptr);
#endif
#ifndef Tcl_ExprDouble_TCL_DECLARED
#define Tcl_ExprDouble_TCL_DECLARED

EXTERN int		Tcl_ExprDouble(Tcl_Interp *interp, CONST char *expr,
				double *ptr);
#endif
#ifndef Tcl_ExprDoubleObj_TCL_DECLARED
#define Tcl_ExprDoubleObj_TCL_DECLARED

EXTERN int		Tcl_ExprDoubleObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, double *ptr);
#endif
#ifndef Tcl_ExprLong_TCL_DECLARED
#define Tcl_ExprLong_TCL_DECLARED

EXTERN int		Tcl_ExprLong(Tcl_Interp *interp, CONST char *expr,
				long *ptr);
#endif
#ifndef Tcl_ExprLongObj_TCL_DECLARED
#define Tcl_ExprLongObj_TCL_DECLARED

EXTERN int		Tcl_ExprLongObj(Tcl_Interp *interp, Tcl_Obj *objPtr,
				long *ptr);
#endif
#ifndef Tcl_ExprObj_TCL_DECLARED
#define Tcl_ExprObj_TCL_DECLARED

EXTERN int		Tcl_ExprObj(Tcl_Interp *interp, Tcl_Obj *objPtr,
				Tcl_Obj **resultPtrPtr);
#endif
#ifndef Tcl_ExprString_TCL_DECLARED
#define Tcl_ExprString_TCL_DECLARED

EXTERN int		Tcl_ExprString(Tcl_Interp *interp, CONST char *expr);
#endif
#ifndef Tcl_Finalize_TCL_DECLARED
#define Tcl_Finalize_TCL_DECLARED

EXTERN void		Tcl_Finalize(void);
#endif
#ifndef Tcl_FindExecutable_TCL_DECLARED
#define Tcl_FindExecutable_TCL_DECLARED

EXTERN void		Tcl_FindExecutable(CONST char *argv0);
#endif
#ifndef Tcl_FirstHashEntry_TCL_DECLARED
#define Tcl_FirstHashEntry_TCL_DECLARED

EXTERN Tcl_HashEntry *	Tcl_FirstHashEntry(Tcl_HashTable *tablePtr,
				Tcl_HashSearch *searchPtr);
#endif
#ifndef Tcl_Flush_TCL_DECLARED
#define Tcl_Flush_TCL_DECLARED

EXTERN int		Tcl_Flush(Tcl_Channel chan);
#endif
#ifndef Tcl_FreeResult_TCL_DECLARED
#define Tcl_FreeResult_TCL_DECLARED

EXTERN void		Tcl_FreeResult(Tcl_Interp *interp);
#endif
#ifndef Tcl_GetAlias_TCL_DECLARED
#define Tcl_GetAlias_TCL_DECLARED

EXTERN int		Tcl_GetAlias(Tcl_Interp *interp,
				CONST char *slaveCmd,
				Tcl_Interp **targetInterpPtr,
				CONST84 char **targetCmdPtr, int *argcPtr,
				CONST84 char ***argvPtr);
#endif
#ifndef Tcl_GetAliasObj_TCL_DECLARED
#define Tcl_GetAliasObj_TCL_DECLARED

EXTERN int		Tcl_GetAliasObj(Tcl_Interp *interp,
				CONST char *slaveCmd,
				Tcl_Interp **targetInterpPtr,
				CONST84 char **targetCmdPtr, int *objcPtr,
				Tcl_Obj ***objv);
#endif
#ifndef Tcl_GetAssocData_TCL_DECLARED
#define Tcl_GetAssocData_TCL_DECLARED

EXTERN ClientData	Tcl_GetAssocData(Tcl_Interp *interp,
				CONST char *name,
				Tcl_InterpDeleteProc **procPtr);
#endif
#ifndef Tcl_GetChannel_TCL_DECLARED
#define Tcl_GetChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_GetChannel(Tcl_Interp *interp,
				CONST char *chanName, int *modePtr);
#endif
#ifndef Tcl_GetChannelBufferSize_TCL_DECLARED
#define Tcl_GetChannelBufferSize_TCL_DECLARED

EXTERN int		Tcl_GetChannelBufferSize(Tcl_Channel chan);
#endif
#ifndef Tcl_GetChannelHandle_TCL_DECLARED
#define Tcl_GetChannelHandle_TCL_DECLARED

EXTERN int		Tcl_GetChannelHandle(Tcl_Channel chan, int direction,
				ClientData *handlePtr);
#endif
#ifndef Tcl_GetChannelInstanceData_TCL_DECLARED
#define Tcl_GetChannelInstanceData_TCL_DECLARED

EXTERN ClientData	Tcl_GetChannelInstanceData(Tcl_Channel chan);
#endif
#ifndef Tcl_GetChannelMode_TCL_DECLARED
#define Tcl_GetChannelMode_TCL_DECLARED

EXTERN int		Tcl_GetChannelMode(Tcl_Channel chan);
#endif
#ifndef Tcl_GetChannelName_TCL_DECLARED
#define Tcl_GetChannelName_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_GetChannelName(Tcl_Channel chan);
#endif
#ifndef Tcl_GetChannelOption_TCL_DECLARED
#define Tcl_GetChannelOption_TCL_DECLARED

EXTERN int		Tcl_GetChannelOption(Tcl_Interp *interp,
				Tcl_Channel chan, CONST char *optionName,
				Tcl_DString *dsPtr);
#endif
#ifndef Tcl_GetChannelType_TCL_DECLARED
#define Tcl_GetChannelType_TCL_DECLARED

EXTERN Tcl_ChannelType * Tcl_GetChannelType(Tcl_Channel chan);
#endif
#ifndef Tcl_GetCommandInfo_TCL_DECLARED
#define Tcl_GetCommandInfo_TCL_DECLARED

EXTERN int		Tcl_GetCommandInfo(Tcl_Interp *interp,
				CONST char *cmdName, Tcl_CmdInfo *infoPtr);
#endif
#ifndef Tcl_GetCommandName_TCL_DECLARED
#define Tcl_GetCommandName_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_GetCommandName(Tcl_Interp *interp,
				Tcl_Command command);
#endif
#ifndef Tcl_GetErrno_TCL_DECLARED
#define Tcl_GetErrno_TCL_DECLARED

EXTERN int		Tcl_GetErrno(void);
#endif
#ifndef Tcl_GetHostName_TCL_DECLARED
#define Tcl_GetHostName_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_GetHostName(void);
#endif
#ifndef Tcl_GetInterpPath_TCL_DECLARED
#define Tcl_GetInterpPath_TCL_DECLARED

EXTERN int		Tcl_GetInterpPath(Tcl_Interp *askInterp,
				Tcl_Interp *slaveInterp);
#endif
#ifndef Tcl_GetMaster_TCL_DECLARED
#define Tcl_GetMaster_TCL_DECLARED

EXTERN Tcl_Interp *	Tcl_GetMaster(Tcl_Interp *interp);
#endif
#ifndef Tcl_GetNameOfExecutable_TCL_DECLARED
#define Tcl_GetNameOfExecutable_TCL_DECLARED

EXTERN CONST char *	Tcl_GetNameOfExecutable(void);
#endif
#ifndef Tcl_GetObjResult_TCL_DECLARED
#define Tcl_GetObjResult_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_GetObjResult(Tcl_Interp *interp);
#endif
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
#ifndef Tcl_GetOpenFile_TCL_DECLARED
#define Tcl_GetOpenFile_TCL_DECLARED

EXTERN int		Tcl_GetOpenFile(Tcl_Interp *interp,
				CONST char *chanID, int forWriting,
				int checkUsage, ClientData *filePtr);
#endif
#endif 
#ifdef MAC_OSX_TCL 
#ifndef Tcl_GetOpenFile_TCL_DECLARED
#define Tcl_GetOpenFile_TCL_DECLARED

EXTERN int		Tcl_GetOpenFile(Tcl_Interp *interp,
				CONST char *chanID, int forWriting,
				int checkUsage, ClientData *filePtr);
#endif
#endif 
#ifndef Tcl_GetPathType_TCL_DECLARED
#define Tcl_GetPathType_TCL_DECLARED

EXTERN Tcl_PathType	Tcl_GetPathType(CONST char *path);
#endif
#ifndef Tcl_Gets_TCL_DECLARED
#define Tcl_Gets_TCL_DECLARED

EXTERN int		Tcl_Gets(Tcl_Channel chan, Tcl_DString *dsPtr);
#endif
#ifndef Tcl_GetsObj_TCL_DECLARED
#define Tcl_GetsObj_TCL_DECLARED

EXTERN int		Tcl_GetsObj(Tcl_Channel chan, Tcl_Obj *objPtr);
#endif
#ifndef Tcl_GetServiceMode_TCL_DECLARED
#define Tcl_GetServiceMode_TCL_DECLARED

EXTERN int		Tcl_GetServiceMode(void);
#endif
#ifndef Tcl_GetSlave_TCL_DECLARED
#define Tcl_GetSlave_TCL_DECLARED

EXTERN Tcl_Interp *	Tcl_GetSlave(Tcl_Interp *interp,
				CONST char *slaveName);
#endif
#ifndef Tcl_GetStdChannel_TCL_DECLARED
#define Tcl_GetStdChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_GetStdChannel(int type);
#endif
#ifndef Tcl_GetStringResult_TCL_DECLARED
#define Tcl_GetStringResult_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_GetStringResult(Tcl_Interp *interp);
#endif
#ifndef Tcl_GetVar_TCL_DECLARED
#define Tcl_GetVar_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_GetVar(Tcl_Interp *interp,
				CONST char *varName, int flags);
#endif
#ifndef Tcl_GetVar2_TCL_DECLARED
#define Tcl_GetVar2_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_GetVar2(Tcl_Interp *interp,
				CONST char *part1, CONST char *part2,
				int flags);
#endif
#ifndef Tcl_GlobalEval_TCL_DECLARED
#define Tcl_GlobalEval_TCL_DECLARED

EXTERN int		Tcl_GlobalEval(Tcl_Interp *interp,
				CONST char *command);
#endif
#ifndef Tcl_GlobalEvalObj_TCL_DECLARED
#define Tcl_GlobalEvalObj_TCL_DECLARED

EXTERN int		Tcl_GlobalEvalObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr);
#endif
#ifndef Tcl_HideCommand_TCL_DECLARED
#define Tcl_HideCommand_TCL_DECLARED

EXTERN int		Tcl_HideCommand(Tcl_Interp *interp,
				CONST char *cmdName,
				CONST char *hiddenCmdToken);
#endif
#ifndef Tcl_Init_TCL_DECLARED
#define Tcl_Init_TCL_DECLARED

EXTERN int		Tcl_Init(Tcl_Interp *interp);
#endif
#ifndef Tcl_InitHashTable_TCL_DECLARED
#define Tcl_InitHashTable_TCL_DECLARED

EXTERN void		Tcl_InitHashTable(Tcl_HashTable *tablePtr,
				int keyType);
#endif
#ifndef Tcl_InputBlocked_TCL_DECLARED
#define Tcl_InputBlocked_TCL_DECLARED

EXTERN int		Tcl_InputBlocked(Tcl_Channel chan);
#endif
#ifndef Tcl_InputBuffered_TCL_DECLARED
#define Tcl_InputBuffered_TCL_DECLARED

EXTERN int		Tcl_InputBuffered(Tcl_Channel chan);
#endif
#ifndef Tcl_InterpDeleted_TCL_DECLARED
#define Tcl_InterpDeleted_TCL_DECLARED

EXTERN int		Tcl_InterpDeleted(Tcl_Interp *interp);
#endif
#ifndef Tcl_IsSafe_TCL_DECLARED
#define Tcl_IsSafe_TCL_DECLARED

EXTERN int		Tcl_IsSafe(Tcl_Interp *interp);
#endif
#ifndef Tcl_JoinPath_TCL_DECLARED
#define Tcl_JoinPath_TCL_DECLARED

EXTERN char *		Tcl_JoinPath(int argc, CONST84 char *CONST *argv,
				Tcl_DString *resultPtr);
#endif
#ifndef Tcl_LinkVar_TCL_DECLARED
#define Tcl_LinkVar_TCL_DECLARED

EXTERN int		Tcl_LinkVar(Tcl_Interp *interp, CONST char *varName,
				char *addr, int type);
#endif

#ifndef Tcl_MakeFileChannel_TCL_DECLARED
#define Tcl_MakeFileChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_MakeFileChannel(ClientData handle, int mode);
#endif
#ifndef Tcl_MakeSafe_TCL_DECLARED
#define Tcl_MakeSafe_TCL_DECLARED

EXTERN int		Tcl_MakeSafe(Tcl_Interp *interp);
#endif
#ifndef Tcl_MakeTcpClientChannel_TCL_DECLARED
#define Tcl_MakeTcpClientChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_MakeTcpClientChannel(ClientData tcpSocket);
#endif
#ifndef Tcl_Merge_TCL_DECLARED
#define Tcl_Merge_TCL_DECLARED

EXTERN char *		Tcl_Merge(int argc, CONST84 char *CONST *argv);
#endif
#ifndef Tcl_NextHashEntry_TCL_DECLARED
#define Tcl_NextHashEntry_TCL_DECLARED

EXTERN Tcl_HashEntry *	Tcl_NextHashEntry(Tcl_HashSearch *searchPtr);
#endif
#ifndef Tcl_NotifyChannel_TCL_DECLARED
#define Tcl_NotifyChannel_TCL_DECLARED

EXTERN void		Tcl_NotifyChannel(Tcl_Channel channel, int mask);
#endif
#ifndef Tcl_ObjGetVar2_TCL_DECLARED
#define Tcl_ObjGetVar2_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_ObjGetVar2(Tcl_Interp *interp, Tcl_Obj *part1Ptr,
				Tcl_Obj *part2Ptr, int flags);
#endif
#ifndef Tcl_ObjSetVar2_TCL_DECLARED
#define Tcl_ObjSetVar2_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_ObjSetVar2(Tcl_Interp *interp, Tcl_Obj *part1Ptr,
				Tcl_Obj *part2Ptr, Tcl_Obj *newValuePtr,
				int flags);
#endif
#ifndef Tcl_OpenCommandChannel_TCL_DECLARED
#define Tcl_OpenCommandChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_OpenCommandChannel(Tcl_Interp *interp, int argc,
				CONST84 char **argv, int flags);
#endif
#ifndef Tcl_OpenFileChannel_TCL_DECLARED
#define Tcl_OpenFileChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_OpenFileChannel(Tcl_Interp *interp,
				CONST char *fileName, CONST char *modeString,
				int permissions);
#endif
#ifndef Tcl_OpenTcpClient_TCL_DECLARED
#define Tcl_OpenTcpClient_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_OpenTcpClient(Tcl_Interp *interp, int port,
				CONST char *address, CONST char *myaddr,
				int myport, int async);
#endif
#ifndef Tcl_OpenTcpServer_TCL_DECLARED
#define Tcl_OpenTcpServer_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_OpenTcpServer(Tcl_Interp *interp, int port,
				CONST char *host,
				Tcl_TcpAcceptProc *acceptProc,
				ClientData callbackData);
#endif
#ifndef Tcl_Preserve_TCL_DECLARED
#define Tcl_Preserve_TCL_DECLARED

EXTERN void		Tcl_Preserve(ClientData data);
#endif
#ifndef Tcl_PrintDouble_TCL_DECLARED
#define Tcl_PrintDouble_TCL_DECLARED

EXTERN void		Tcl_PrintDouble(Tcl_Interp *interp, double value,
				char *dst);
#endif
#ifndef Tcl_PutEnv_TCL_DECLARED
#define Tcl_PutEnv_TCL_DECLARED

EXTERN int		Tcl_PutEnv(CONST char *assignment);
#endif
#ifndef Tcl_PosixError_TCL_DECLARED
#define Tcl_PosixError_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_PosixError(Tcl_Interp *interp);
#endif
#ifndef Tcl_QueueEvent_TCL_DECLARED
#define Tcl_QueueEvent_TCL_DECLARED

EXTERN void		Tcl_QueueEvent(Tcl_Event *evPtr,
				Tcl_QueuePosition position);
#endif
#ifndef Tcl_Read_TCL_DECLARED
#define Tcl_Read_TCL_DECLARED

EXTERN int		Tcl_Read(Tcl_Channel chan, char *bufPtr, int toRead);
#endif
#ifndef Tcl_ReapDetachedProcs_TCL_DECLARED
#define Tcl_ReapDetachedProcs_TCL_DECLARED

EXTERN void		Tcl_ReapDetachedProcs(void);
#endif
#ifndef Tcl_RecordAndEval_TCL_DECLARED
#define Tcl_RecordAndEval_TCL_DECLARED

EXTERN int		Tcl_RecordAndEval(Tcl_Interp *interp,
				CONST char *cmd, int flags);
#endif
#ifndef Tcl_RecordAndEvalObj_TCL_DECLARED
#define Tcl_RecordAndEvalObj_TCL_DECLARED

EXTERN int		Tcl_RecordAndEvalObj(Tcl_Interp *interp,
				Tcl_Obj *cmdPtr, int flags);
#endif
#ifndef Tcl_RegisterChannel_TCL_DECLARED
#define Tcl_RegisterChannel_TCL_DECLARED

EXTERN void		Tcl_RegisterChannel(Tcl_Interp *interp,
				Tcl_Channel chan);
#endif
#ifndef Tcl_RegisterObjType_TCL_DECLARED
#define Tcl_RegisterObjType_TCL_DECLARED

EXTERN void		Tcl_RegisterObjType(Tcl_ObjType *typePtr);
#endif
#ifndef Tcl_RegExpCompile_TCL_DECLARED
#define Tcl_RegExpCompile_TCL_DECLARED

EXTERN Tcl_RegExp	Tcl_RegExpCompile(Tcl_Interp *interp,
				CONST char *pattern);
#endif
#ifndef Tcl_RegExpExec_TCL_DECLARED
#define Tcl_RegExpExec_TCL_DECLARED

EXTERN int		Tcl_RegExpExec(Tcl_Interp *interp, Tcl_RegExp regexp,
				CONST char *text, CONST char *start);
#endif
#ifndef Tcl_RegExpMatch_TCL_DECLARED
#define Tcl_RegExpMatch_TCL_DECLARED

EXTERN int		Tcl_RegExpMatch(Tcl_Interp *interp, CONST char *text,
				CONST char *pattern);
#endif
#ifndef Tcl_RegExpRange_TCL_DECLARED
#define Tcl_RegExpRange_TCL_DECLARED

EXTERN void		Tcl_RegExpRange(Tcl_RegExp regexp, int index,
				CONST84 char **startPtr,
				CONST84 char **endPtr);
#endif
#ifndef Tcl_Release_TCL_DECLARED
#define Tcl_Release_TCL_DECLARED

EXTERN void		Tcl_Release(ClientData clientData);
#endif
#ifndef Tcl_ResetResult_TCL_DECLARED
#define Tcl_ResetResult_TCL_DECLARED

EXTERN void		Tcl_ResetResult(Tcl_Interp *interp);
#endif
#ifndef Tcl_ScanElement_TCL_DECLARED
#define Tcl_ScanElement_TCL_DECLARED

EXTERN int		Tcl_ScanElement(CONST char *src, int *flagPtr);
#endif
#ifndef Tcl_ScanCountedElement_TCL_DECLARED
#define Tcl_ScanCountedElement_TCL_DECLARED

EXTERN int		Tcl_ScanCountedElement(CONST char *src, int length,
				int *flagPtr);
#endif
#ifndef Tcl_SeekOld_TCL_DECLARED
#define Tcl_SeekOld_TCL_DECLARED

EXTERN int		Tcl_SeekOld(Tcl_Channel chan, int offset, int mode);
#endif
#ifndef Tcl_ServiceAll_TCL_DECLARED
#define Tcl_ServiceAll_TCL_DECLARED

EXTERN int		Tcl_ServiceAll(void);
#endif
#ifndef Tcl_ServiceEvent_TCL_DECLARED
#define Tcl_ServiceEvent_TCL_DECLARED

EXTERN int		Tcl_ServiceEvent(int flags);
#endif
#ifndef Tcl_SetAssocData_TCL_DECLARED
#define Tcl_SetAssocData_TCL_DECLARED

EXTERN void		Tcl_SetAssocData(Tcl_Interp *interp,
				CONST char *name, Tcl_InterpDeleteProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_SetChannelBufferSize_TCL_DECLARED
#define Tcl_SetChannelBufferSize_TCL_DECLARED

EXTERN void		Tcl_SetChannelBufferSize(Tcl_Channel chan, int sz);
#endif
#ifndef Tcl_SetChannelOption_TCL_DECLARED
#define Tcl_SetChannelOption_TCL_DECLARED

EXTERN int		Tcl_SetChannelOption(Tcl_Interp *interp,
				Tcl_Channel chan, CONST char *optionName,
				CONST char *newValue);
#endif
#ifndef Tcl_SetCommandInfo_TCL_DECLARED
#define Tcl_SetCommandInfo_TCL_DECLARED

EXTERN int		Tcl_SetCommandInfo(Tcl_Interp *interp,
				CONST char *cmdName,
				CONST Tcl_CmdInfo *infoPtr);
#endif
#ifndef Tcl_SetErrno_TCL_DECLARED
#define Tcl_SetErrno_TCL_DECLARED

EXTERN void		Tcl_SetErrno(int err);
#endif
#ifndef Tcl_SetErrorCode_TCL_DECLARED
#define Tcl_SetErrorCode_TCL_DECLARED

EXTERN void		Tcl_SetErrorCode(Tcl_Interp *interp, ...);
#endif
#ifndef Tcl_SetMaxBlockTime_TCL_DECLARED
#define Tcl_SetMaxBlockTime_TCL_DECLARED

EXTERN void		Tcl_SetMaxBlockTime(Tcl_Time *timePtr);
#endif
#ifndef Tcl_SetPanicProc_TCL_DECLARED
#define Tcl_SetPanicProc_TCL_DECLARED

EXTERN void		Tcl_SetPanicProc(Tcl_PanicProc *panicProc);
#endif
#ifndef Tcl_SetRecursionLimit_TCL_DECLARED
#define Tcl_SetRecursionLimit_TCL_DECLARED

EXTERN int		Tcl_SetRecursionLimit(Tcl_Interp *interp, int depth);
#endif
#ifndef Tcl_SetResult_TCL_DECLARED
#define Tcl_SetResult_TCL_DECLARED

EXTERN void		Tcl_SetResult(Tcl_Interp *interp, char *result,
				Tcl_FreeProc *freeProc);
#endif
#ifndef Tcl_SetServiceMode_TCL_DECLARED
#define Tcl_SetServiceMode_TCL_DECLARED

EXTERN int		Tcl_SetServiceMode(int mode);
#endif
#ifndef Tcl_SetObjErrorCode_TCL_DECLARED
#define Tcl_SetObjErrorCode_TCL_DECLARED

EXTERN void		Tcl_SetObjErrorCode(Tcl_Interp *interp,
				Tcl_Obj *errorObjPtr);
#endif
#ifndef Tcl_SetObjResult_TCL_DECLARED
#define Tcl_SetObjResult_TCL_DECLARED

EXTERN void		Tcl_SetObjResult(Tcl_Interp *interp,
				Tcl_Obj *resultObjPtr);
#endif
#ifndef Tcl_SetStdChannel_TCL_DECLARED
#define Tcl_SetStdChannel_TCL_DECLARED

EXTERN void		Tcl_SetStdChannel(Tcl_Channel channel, int type);
#endif
#ifndef Tcl_SetVar_TCL_DECLARED
#define Tcl_SetVar_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_SetVar(Tcl_Interp *interp,
				CONST char *varName, CONST char *newValue,
				int flags);
#endif
#ifndef Tcl_SetVar2_TCL_DECLARED
#define Tcl_SetVar2_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_SetVar2(Tcl_Interp *interp,
				CONST char *part1, CONST char *part2,
				CONST char *newValue, int flags);
#endif
#ifndef Tcl_SignalId_TCL_DECLARED
#define Tcl_SignalId_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_SignalId(int sig);
#endif
#ifndef Tcl_SignalMsg_TCL_DECLARED
#define Tcl_SignalMsg_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_SignalMsg(int sig);
#endif
#ifndef Tcl_SourceRCFile_TCL_DECLARED
#define Tcl_SourceRCFile_TCL_DECLARED

EXTERN void		Tcl_SourceRCFile(Tcl_Interp *interp);
#endif
#ifndef Tcl_SplitList_TCL_DECLARED
#define Tcl_SplitList_TCL_DECLARED

EXTERN int		Tcl_SplitList(Tcl_Interp *interp,
				CONST char *listStr, int *argcPtr,
				CONST84 char ***argvPtr);
#endif
#ifndef Tcl_SplitPath_TCL_DECLARED
#define Tcl_SplitPath_TCL_DECLARED

EXTERN void		Tcl_SplitPath(CONST char *path, int *argcPtr,
				CONST84 char ***argvPtr);
#endif
#ifndef Tcl_StaticPackage_TCL_DECLARED
#define Tcl_StaticPackage_TCL_DECLARED

EXTERN void		Tcl_StaticPackage(Tcl_Interp *interp,
				CONST char *pkgName,
				Tcl_PackageInitProc *initProc,
				Tcl_PackageInitProc *safeInitProc);
#endif
#ifndef Tcl_StringMatch_TCL_DECLARED
#define Tcl_StringMatch_TCL_DECLARED

EXTERN int		Tcl_StringMatch(CONST char *str, CONST char *pattern);
#endif
#ifndef Tcl_TellOld_TCL_DECLARED
#define Tcl_TellOld_TCL_DECLARED

EXTERN int		Tcl_TellOld(Tcl_Channel chan);
#endif
#ifndef Tcl_TraceVar_TCL_DECLARED
#define Tcl_TraceVar_TCL_DECLARED

EXTERN int		Tcl_TraceVar(Tcl_Interp *interp, CONST char *varName,
				int flags, Tcl_VarTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_TraceVar2_TCL_DECLARED
#define Tcl_TraceVar2_TCL_DECLARED

EXTERN int		Tcl_TraceVar2(Tcl_Interp *interp, CONST char *part1,
				CONST char *part2, int flags,
				Tcl_VarTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_TranslateFileName_TCL_DECLARED
#define Tcl_TranslateFileName_TCL_DECLARED

EXTERN char *		Tcl_TranslateFileName(Tcl_Interp *interp,
				CONST char *name, Tcl_DString *bufferPtr);
#endif
#ifndef Tcl_Ungets_TCL_DECLARED
#define Tcl_Ungets_TCL_DECLARED

EXTERN int		Tcl_Ungets(Tcl_Channel chan, CONST char *str,
				int len, int atHead);
#endif
#ifndef Tcl_UnlinkVar_TCL_DECLARED
#define Tcl_UnlinkVar_TCL_DECLARED

EXTERN void		Tcl_UnlinkVar(Tcl_Interp *interp,
				CONST char *varName);
#endif
#ifndef Tcl_UnregisterChannel_TCL_DECLARED
#define Tcl_UnregisterChannel_TCL_DECLARED

EXTERN int		Tcl_UnregisterChannel(Tcl_Interp *interp,
				Tcl_Channel chan);
#endif
#ifndef Tcl_UnsetVar_TCL_DECLARED
#define Tcl_UnsetVar_TCL_DECLARED

EXTERN int		Tcl_UnsetVar(Tcl_Interp *interp, CONST char *varName,
				int flags);
#endif
#ifndef Tcl_UnsetVar2_TCL_DECLARED
#define Tcl_UnsetVar2_TCL_DECLARED

EXTERN int		Tcl_UnsetVar2(Tcl_Interp *interp, CONST char *part1,
				CONST char *part2, int flags);
#endif
#ifndef Tcl_UntraceVar_TCL_DECLARED
#define Tcl_UntraceVar_TCL_DECLARED

EXTERN void		Tcl_UntraceVar(Tcl_Interp *interp,
				CONST char *varName, int flags,
				Tcl_VarTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_UntraceVar2_TCL_DECLARED
#define Tcl_UntraceVar2_TCL_DECLARED

EXTERN void		Tcl_UntraceVar2(Tcl_Interp *interp,
				CONST char *part1, CONST char *part2,
				int flags, Tcl_VarTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_UpdateLinkedVar_TCL_DECLARED
#define Tcl_UpdateLinkedVar_TCL_DECLARED

EXTERN void		Tcl_UpdateLinkedVar(Tcl_Interp *interp,
				CONST char *varName);
#endif
#ifndef Tcl_UpVar_TCL_DECLARED
#define Tcl_UpVar_TCL_DECLARED

EXTERN int		Tcl_UpVar(Tcl_Interp *interp, CONST char *frameName,
				CONST char *varName, CONST char *localName,
				int flags);
#endif
#ifndef Tcl_UpVar2_TCL_DECLARED
#define Tcl_UpVar2_TCL_DECLARED

EXTERN int		Tcl_UpVar2(Tcl_Interp *interp, CONST char *frameName,
				CONST char *part1, CONST char *part2,
				CONST char *localName, int flags);
#endif
#ifndef Tcl_VarEval_TCL_DECLARED
#define Tcl_VarEval_TCL_DECLARED

EXTERN int		Tcl_VarEval(Tcl_Interp *interp, ...);
#endif
#ifndef Tcl_VarTraceInfo_TCL_DECLARED
#define Tcl_VarTraceInfo_TCL_DECLARED

EXTERN ClientData	Tcl_VarTraceInfo(Tcl_Interp *interp,
				CONST char *varName, int flags,
				Tcl_VarTraceProc *procPtr,
				ClientData prevClientData);
#endif
#ifndef Tcl_VarTraceInfo2_TCL_DECLARED
#define Tcl_VarTraceInfo2_TCL_DECLARED

EXTERN ClientData	Tcl_VarTraceInfo2(Tcl_Interp *interp,
				CONST char *part1, CONST char *part2,
				int flags, Tcl_VarTraceProc *procPtr,
				ClientData prevClientData);
#endif
#ifndef Tcl_Write_TCL_DECLARED
#define Tcl_Write_TCL_DECLARED

EXTERN int		Tcl_Write(Tcl_Channel chan, CONST char *s, int slen);
#endif
#ifndef Tcl_WrongNumArgs_TCL_DECLARED
#define Tcl_WrongNumArgs_TCL_DECLARED

EXTERN void		Tcl_WrongNumArgs(Tcl_Interp *interp, int objc,
				Tcl_Obj *CONST objv[], CONST char *message);
#endif
#ifndef Tcl_DumpActiveMemory_TCL_DECLARED
#define Tcl_DumpActiveMemory_TCL_DECLARED

EXTERN int		Tcl_DumpActiveMemory(CONST char *fileName);
#endif
#ifndef Tcl_ValidateAllMemory_TCL_DECLARED
#define Tcl_ValidateAllMemory_TCL_DECLARED

EXTERN void		Tcl_ValidateAllMemory(CONST char *file, int line);
#endif
#ifndef Tcl_AppendResultVA_TCL_DECLARED
#define Tcl_AppendResultVA_TCL_DECLARED

EXTERN void		Tcl_AppendResultVA(Tcl_Interp *interp,
				va_list argList);
#endif
#ifndef Tcl_AppendStringsToObjVA_TCL_DECLARED
#define Tcl_AppendStringsToObjVA_TCL_DECLARED

EXTERN void		Tcl_AppendStringsToObjVA(Tcl_Obj *objPtr,
				va_list argList);
#endif
#ifndef Tcl_HashStats_TCL_DECLARED
#define Tcl_HashStats_TCL_DECLARED

EXTERN char *		Tcl_HashStats(Tcl_HashTable *tablePtr);
#endif
#ifndef Tcl_ParseVar_TCL_DECLARED
#define Tcl_ParseVar_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_ParseVar(Tcl_Interp *interp,
				CONST char *start, CONST84 char **termPtr);
#endif
#ifndef Tcl_PkgPresent_TCL_DECLARED
#define Tcl_PkgPresent_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_PkgPresent(Tcl_Interp *interp,
				CONST char *name, CONST char *version,
				int exact);
#endif
#ifndef Tcl_PkgPresentEx_TCL_DECLARED
#define Tcl_PkgPresentEx_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_PkgPresentEx(Tcl_Interp *interp,
				CONST char *name, CONST char *version,
				int exact, ClientData *clientDataPtr);
#endif
#ifndef Tcl_PkgProvide_TCL_DECLARED
#define Tcl_PkgProvide_TCL_DECLARED

EXTERN int		Tcl_PkgProvide(Tcl_Interp *interp, CONST char *name,
				CONST char *version);
#endif
#ifndef Tcl_PkgRequire_TCL_DECLARED
#define Tcl_PkgRequire_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_PkgRequire(Tcl_Interp *interp,
				CONST char *name, CONST char *version,
				int exact);
#endif
#ifndef Tcl_SetErrorCodeVA_TCL_DECLARED
#define Tcl_SetErrorCodeVA_TCL_DECLARED

EXTERN void		Tcl_SetErrorCodeVA(Tcl_Interp *interp,
				va_list argList);
#endif
#ifndef Tcl_VarEvalVA_TCL_DECLARED
#define Tcl_VarEvalVA_TCL_DECLARED

EXTERN int		Tcl_VarEvalVA(Tcl_Interp *interp, va_list argList);
#endif
#ifndef Tcl_WaitPid_TCL_DECLARED
#define Tcl_WaitPid_TCL_DECLARED

EXTERN Tcl_Pid		Tcl_WaitPid(Tcl_Pid pid, int *statPtr, int options);
#endif
#ifndef Tcl_PanicVA_TCL_DECLARED
#define Tcl_PanicVA_TCL_DECLARED

EXTERN void		Tcl_PanicVA(CONST char *format, va_list argList);
#endif
#ifndef Tcl_GetVersion_TCL_DECLARED
#define Tcl_GetVersion_TCL_DECLARED

EXTERN void		Tcl_GetVersion(int *major, int *minor,
				int *patchLevel, int *type);
#endif
#ifndef Tcl_InitMemory_TCL_DECLARED
#define Tcl_InitMemory_TCL_DECLARED

EXTERN void		Tcl_InitMemory(Tcl_Interp *interp);
#endif
#ifndef Tcl_StackChannel_TCL_DECLARED
#define Tcl_StackChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_StackChannel(Tcl_Interp *interp,
				Tcl_ChannelType *typePtr,
				ClientData instanceData, int mask,
				Tcl_Channel prevChan);
#endif
#ifndef Tcl_UnstackChannel_TCL_DECLARED
#define Tcl_UnstackChannel_TCL_DECLARED

EXTERN int		Tcl_UnstackChannel(Tcl_Interp *interp,
				Tcl_Channel chan);
#endif
#ifndef Tcl_GetStackedChannel_TCL_DECLARED
#define Tcl_GetStackedChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_GetStackedChannel(Tcl_Channel chan);
#endif
#ifndef Tcl_SetMainLoop_TCL_DECLARED
#define Tcl_SetMainLoop_TCL_DECLARED

EXTERN void		Tcl_SetMainLoop(Tcl_MainLoopProc *proc);
#endif

#ifndef Tcl_AppendObjToObj_TCL_DECLARED
#define Tcl_AppendObjToObj_TCL_DECLARED

EXTERN void		Tcl_AppendObjToObj(Tcl_Obj *objPtr,
				Tcl_Obj *appendObjPtr);
#endif
#ifndef Tcl_CreateEncoding_TCL_DECLARED
#define Tcl_CreateEncoding_TCL_DECLARED

EXTERN Tcl_Encoding	Tcl_CreateEncoding(CONST Tcl_EncodingType *typePtr);
#endif
#ifndef Tcl_CreateThreadExitHandler_TCL_DECLARED
#define Tcl_CreateThreadExitHandler_TCL_DECLARED

EXTERN void		Tcl_CreateThreadExitHandler(Tcl_ExitProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_DeleteThreadExitHandler_TCL_DECLARED
#define Tcl_DeleteThreadExitHandler_TCL_DECLARED

EXTERN void		Tcl_DeleteThreadExitHandler(Tcl_ExitProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_DiscardResult_TCL_DECLARED
#define Tcl_DiscardResult_TCL_DECLARED

EXTERN void		Tcl_DiscardResult(Tcl_SavedResult *statePtr);
#endif
#ifndef Tcl_EvalEx_TCL_DECLARED
#define Tcl_EvalEx_TCL_DECLARED

EXTERN int		Tcl_EvalEx(Tcl_Interp *interp, CONST char *script,
				int numBytes, int flags);
#endif
#ifndef Tcl_EvalObjv_TCL_DECLARED
#define Tcl_EvalObjv_TCL_DECLARED

EXTERN int		Tcl_EvalObjv(Tcl_Interp *interp, int objc,
				Tcl_Obj *CONST objv[], int flags);
#endif
#ifndef Tcl_EvalObjEx_TCL_DECLARED
#define Tcl_EvalObjEx_TCL_DECLARED

EXTERN int		Tcl_EvalObjEx(Tcl_Interp *interp, Tcl_Obj *objPtr,
				int flags);
#endif
#ifndef Tcl_ExitThread_TCL_DECLARED
#define Tcl_ExitThread_TCL_DECLARED

EXTERN void		Tcl_ExitThread(int status);
#endif
#ifndef Tcl_ExternalToUtf_TCL_DECLARED
#define Tcl_ExternalToUtf_TCL_DECLARED

EXTERN int		Tcl_ExternalToUtf(Tcl_Interp *interp,
				Tcl_Encoding encoding, CONST char *src,
				int srcLen, int flags,
				Tcl_EncodingState *statePtr, char *dst,
				int dstLen, int *srcReadPtr,
				int *dstWrotePtr, int *dstCharsPtr);
#endif
#ifndef Tcl_ExternalToUtfDString_TCL_DECLARED
#define Tcl_ExternalToUtfDString_TCL_DECLARED

EXTERN char *		Tcl_ExternalToUtfDString(Tcl_Encoding encoding,
				CONST char *src, int srcLen,
				Tcl_DString *dsPtr);
#endif
#ifndef Tcl_FinalizeThread_TCL_DECLARED
#define Tcl_FinalizeThread_TCL_DECLARED

EXTERN void		Tcl_FinalizeThread(void);
#endif
#ifndef Tcl_FinalizeNotifier_TCL_DECLARED
#define Tcl_FinalizeNotifier_TCL_DECLARED

EXTERN void		Tcl_FinalizeNotifier(ClientData clientData);
#endif
#ifndef Tcl_FreeEncoding_TCL_DECLARED
#define Tcl_FreeEncoding_TCL_DECLARED

EXTERN void		Tcl_FreeEncoding(Tcl_Encoding encoding);
#endif
#ifndef Tcl_GetCurrentThread_TCL_DECLARED
#define Tcl_GetCurrentThread_TCL_DECLARED

EXTERN Tcl_ThreadId	Tcl_GetCurrentThread(void);
#endif
#ifndef Tcl_GetEncoding_TCL_DECLARED
#define Tcl_GetEncoding_TCL_DECLARED

EXTERN Tcl_Encoding	Tcl_GetEncoding(Tcl_Interp *interp, CONST char *name);
#endif
#ifndef Tcl_GetEncodingName_TCL_DECLARED
#define Tcl_GetEncodingName_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_GetEncodingName(Tcl_Encoding encoding);
#endif
#ifndef Tcl_GetEncodingNames_TCL_DECLARED
#define Tcl_GetEncodingNames_TCL_DECLARED

EXTERN void		Tcl_GetEncodingNames(Tcl_Interp *interp);
#endif
#ifndef Tcl_GetIndexFromObjStruct_TCL_DECLARED
#define Tcl_GetIndexFromObjStruct_TCL_DECLARED

EXTERN int		Tcl_GetIndexFromObjStruct(Tcl_Interp *interp,
				Tcl_Obj *objPtr, CONST VOID *tablePtr,
				int offset, CONST char *msg, int flags,
				int *indexPtr);
#endif
#ifndef Tcl_GetThreadData_TCL_DECLARED
#define Tcl_GetThreadData_TCL_DECLARED

EXTERN VOID *		Tcl_GetThreadData(Tcl_ThreadDataKey *keyPtr,
				int size);
#endif
#ifndef Tcl_GetVar2Ex_TCL_DECLARED
#define Tcl_GetVar2Ex_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_GetVar2Ex(Tcl_Interp *interp, CONST char *part1,
				CONST char *part2, int flags);
#endif
#ifndef Tcl_InitNotifier_TCL_DECLARED
#define Tcl_InitNotifier_TCL_DECLARED

EXTERN ClientData	Tcl_InitNotifier(void);
#endif
#ifndef Tcl_MutexLock_TCL_DECLARED
#define Tcl_MutexLock_TCL_DECLARED

EXTERN void		Tcl_MutexLock(Tcl_Mutex *mutexPtr);
#endif
#ifndef Tcl_MutexUnlock_TCL_DECLARED
#define Tcl_MutexUnlock_TCL_DECLARED

EXTERN void		Tcl_MutexUnlock(Tcl_Mutex *mutexPtr);
#endif
#ifndef Tcl_ConditionNotify_TCL_DECLARED
#define Tcl_ConditionNotify_TCL_DECLARED

EXTERN void		Tcl_ConditionNotify(Tcl_Condition *condPtr);
#endif
#ifndef Tcl_ConditionWait_TCL_DECLARED
#define Tcl_ConditionWait_TCL_DECLARED

EXTERN void		Tcl_ConditionWait(Tcl_Condition *condPtr,
				Tcl_Mutex *mutexPtr, Tcl_Time *timePtr);
#endif
#ifndef Tcl_NumUtfChars_TCL_DECLARED
#define Tcl_NumUtfChars_TCL_DECLARED

EXTERN int		Tcl_NumUtfChars(CONST char *src, int length);
#endif
#ifndef Tcl_ReadChars_TCL_DECLARED
#define Tcl_ReadChars_TCL_DECLARED

EXTERN int		Tcl_ReadChars(Tcl_Channel channel, Tcl_Obj *objPtr,
				int charsToRead, int appendFlag);
#endif
#ifndef Tcl_RestoreResult_TCL_DECLARED
#define Tcl_RestoreResult_TCL_DECLARED

EXTERN void		Tcl_RestoreResult(Tcl_Interp *interp,
				Tcl_SavedResult *statePtr);
#endif
#ifndef Tcl_SaveResult_TCL_DECLARED
#define Tcl_SaveResult_TCL_DECLARED

EXTERN void		Tcl_SaveResult(Tcl_Interp *interp,
				Tcl_SavedResult *statePtr);
#endif
#ifndef Tcl_SetSystemEncoding_TCL_DECLARED
#define Tcl_SetSystemEncoding_TCL_DECLARED

EXTERN int		Tcl_SetSystemEncoding(Tcl_Interp *interp,
				CONST char *name);
#endif
#ifndef Tcl_SetVar2Ex_TCL_DECLARED
#define Tcl_SetVar2Ex_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_SetVar2Ex(Tcl_Interp *interp, CONST char *part1,
				CONST char *part2, Tcl_Obj *newValuePtr,
				int flags);
#endif
#ifndef Tcl_ThreadAlert_TCL_DECLARED
#define Tcl_ThreadAlert_TCL_DECLARED

EXTERN void		Tcl_ThreadAlert(Tcl_ThreadId threadId);
#endif
#ifndef Tcl_ThreadQueueEvent_TCL_DECLARED
#define Tcl_ThreadQueueEvent_TCL_DECLARED

EXTERN void		Tcl_ThreadQueueEvent(Tcl_ThreadId threadId,
				Tcl_Event *evPtr, Tcl_QueuePosition position);
#endif
#ifndef Tcl_UniCharAtIndex_TCL_DECLARED
#define Tcl_UniCharAtIndex_TCL_DECLARED

EXTERN Tcl_UniChar	Tcl_UniCharAtIndex(CONST char *src, int index);
#endif
#ifndef Tcl_UniCharToLower_TCL_DECLARED
#define Tcl_UniCharToLower_TCL_DECLARED

EXTERN Tcl_UniChar	Tcl_UniCharToLower(int ch);
#endif
#ifndef Tcl_UniCharToTitle_TCL_DECLARED
#define Tcl_UniCharToTitle_TCL_DECLARED

EXTERN Tcl_UniChar	Tcl_UniCharToTitle(int ch);
#endif
#ifndef Tcl_UniCharToUpper_TCL_DECLARED
#define Tcl_UniCharToUpper_TCL_DECLARED

EXTERN Tcl_UniChar	Tcl_UniCharToUpper(int ch);
#endif
#ifndef Tcl_UniCharToUtf_TCL_DECLARED
#define Tcl_UniCharToUtf_TCL_DECLARED

EXTERN int		Tcl_UniCharToUtf(int ch, char *buf);
#endif
#ifndef Tcl_UtfAtIndex_TCL_DECLARED
#define Tcl_UtfAtIndex_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_UtfAtIndex(CONST char *src, int index);
#endif
#ifndef Tcl_UtfCharComplete_TCL_DECLARED
#define Tcl_UtfCharComplete_TCL_DECLARED

EXTERN int		Tcl_UtfCharComplete(CONST char *src, int length);
#endif
#ifndef Tcl_UtfBackslash_TCL_DECLARED
#define Tcl_UtfBackslash_TCL_DECLARED

EXTERN int		Tcl_UtfBackslash(CONST char *src, int *readPtr,
				char *dst);
#endif
#ifndef Tcl_UtfFindFirst_TCL_DECLARED
#define Tcl_UtfFindFirst_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_UtfFindFirst(CONST char *src, int ch);
#endif
#ifndef Tcl_UtfFindLast_TCL_DECLARED
#define Tcl_UtfFindLast_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_UtfFindLast(CONST char *src, int ch);
#endif
#ifndef Tcl_UtfNext_TCL_DECLARED
#define Tcl_UtfNext_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_UtfNext(CONST char *src);
#endif
#ifndef Tcl_UtfPrev_TCL_DECLARED
#define Tcl_UtfPrev_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_UtfPrev(CONST char *src, CONST char *start);
#endif
#ifndef Tcl_UtfToExternal_TCL_DECLARED
#define Tcl_UtfToExternal_TCL_DECLARED

EXTERN int		Tcl_UtfToExternal(Tcl_Interp *interp,
				Tcl_Encoding encoding, CONST char *src,
				int srcLen, int flags,
				Tcl_EncodingState *statePtr, char *dst,
				int dstLen, int *srcReadPtr,
				int *dstWrotePtr, int *dstCharsPtr);
#endif
#ifndef Tcl_UtfToExternalDString_TCL_DECLARED
#define Tcl_UtfToExternalDString_TCL_DECLARED

EXTERN char *		Tcl_UtfToExternalDString(Tcl_Encoding encoding,
				CONST char *src, int srcLen,
				Tcl_DString *dsPtr);
#endif
#ifndef Tcl_UtfToLower_TCL_DECLARED
#define Tcl_UtfToLower_TCL_DECLARED

EXTERN int		Tcl_UtfToLower(char *src);
#endif
#ifndef Tcl_UtfToTitle_TCL_DECLARED
#define Tcl_UtfToTitle_TCL_DECLARED

EXTERN int		Tcl_UtfToTitle(char *src);
#endif
#ifndef Tcl_UtfToUniChar_TCL_DECLARED
#define Tcl_UtfToUniChar_TCL_DECLARED

EXTERN int		Tcl_UtfToUniChar(CONST char *src, Tcl_UniChar *chPtr);
#endif
#ifndef Tcl_UtfToUpper_TCL_DECLARED
#define Tcl_UtfToUpper_TCL_DECLARED

EXTERN int		Tcl_UtfToUpper(char *src);
#endif
#ifndef Tcl_WriteChars_TCL_DECLARED
#define Tcl_WriteChars_TCL_DECLARED

EXTERN int		Tcl_WriteChars(Tcl_Channel chan, CONST char *src,
				int srcLen);
#endif
#ifndef Tcl_WriteObj_TCL_DECLARED
#define Tcl_WriteObj_TCL_DECLARED

EXTERN int		Tcl_WriteObj(Tcl_Channel chan, Tcl_Obj *objPtr);
#endif
#ifndef Tcl_GetString_TCL_DECLARED
#define Tcl_GetString_TCL_DECLARED

EXTERN char *		Tcl_GetString(Tcl_Obj *objPtr);
#endif
#ifndef Tcl_GetDefaultEncodingDir_TCL_DECLARED
#define Tcl_GetDefaultEncodingDir_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_GetDefaultEncodingDir(void);
#endif
#ifndef Tcl_SetDefaultEncodingDir_TCL_DECLARED
#define Tcl_SetDefaultEncodingDir_TCL_DECLARED

EXTERN void		Tcl_SetDefaultEncodingDir(CONST char *path);
#endif
#ifndef Tcl_AlertNotifier_TCL_DECLARED
#define Tcl_AlertNotifier_TCL_DECLARED

EXTERN void		Tcl_AlertNotifier(ClientData clientData);
#endif
#ifndef Tcl_ServiceModeHook_TCL_DECLARED
#define Tcl_ServiceModeHook_TCL_DECLARED

EXTERN void		Tcl_ServiceModeHook(int mode);
#endif
#ifndef Tcl_UniCharIsAlnum_TCL_DECLARED
#define Tcl_UniCharIsAlnum_TCL_DECLARED

EXTERN int		Tcl_UniCharIsAlnum(int ch);
#endif
#ifndef Tcl_UniCharIsAlpha_TCL_DECLARED
#define Tcl_UniCharIsAlpha_TCL_DECLARED

EXTERN int		Tcl_UniCharIsAlpha(int ch);
#endif
#ifndef Tcl_UniCharIsDigit_TCL_DECLARED
#define Tcl_UniCharIsDigit_TCL_DECLARED

EXTERN int		Tcl_UniCharIsDigit(int ch);
#endif
#ifndef Tcl_UniCharIsLower_TCL_DECLARED
#define Tcl_UniCharIsLower_TCL_DECLARED

EXTERN int		Tcl_UniCharIsLower(int ch);
#endif
#ifndef Tcl_UniCharIsSpace_TCL_DECLARED
#define Tcl_UniCharIsSpace_TCL_DECLARED

EXTERN int		Tcl_UniCharIsSpace(int ch);
#endif
#ifndef Tcl_UniCharIsUpper_TCL_DECLARED
#define Tcl_UniCharIsUpper_TCL_DECLARED

EXTERN int		Tcl_UniCharIsUpper(int ch);
#endif
#ifndef Tcl_UniCharIsWordChar_TCL_DECLARED
#define Tcl_UniCharIsWordChar_TCL_DECLARED

EXTERN int		Tcl_UniCharIsWordChar(int ch);
#endif
#ifndef Tcl_UniCharLen_TCL_DECLARED
#define Tcl_UniCharLen_TCL_DECLARED

EXTERN int		Tcl_UniCharLen(CONST Tcl_UniChar *uniStr);
#endif
#ifndef Tcl_UniCharNcmp_TCL_DECLARED
#define Tcl_UniCharNcmp_TCL_DECLARED

EXTERN int		Tcl_UniCharNcmp(CONST Tcl_UniChar *ucs,
				CONST Tcl_UniChar *uct,
				unsigned long numChars);
#endif
#ifndef Tcl_UniCharToUtfDString_TCL_DECLARED
#define Tcl_UniCharToUtfDString_TCL_DECLARED

EXTERN char *		Tcl_UniCharToUtfDString(CONST Tcl_UniChar *uniStr,
				int uniLength, Tcl_DString *dsPtr);
#endif
#ifndef Tcl_UtfToUniCharDString_TCL_DECLARED
#define Tcl_UtfToUniCharDString_TCL_DECLARED

EXTERN Tcl_UniChar *	Tcl_UtfToUniCharDString(CONST char *src, int length,
				Tcl_DString *dsPtr);
#endif
#ifndef Tcl_GetRegExpFromObj_TCL_DECLARED
#define Tcl_GetRegExpFromObj_TCL_DECLARED

EXTERN Tcl_RegExp	Tcl_GetRegExpFromObj(Tcl_Interp *interp,
				Tcl_Obj *patObj, int flags);
#endif
#ifndef Tcl_EvalTokens_TCL_DECLARED
#define Tcl_EvalTokens_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_EvalTokens(Tcl_Interp *interp,
				Tcl_Token *tokenPtr, int count);
#endif
#ifndef Tcl_FreeParse_TCL_DECLARED
#define Tcl_FreeParse_TCL_DECLARED

EXTERN void		Tcl_FreeParse(Tcl_Parse *parsePtr);
#endif
#ifndef Tcl_LogCommandInfo_TCL_DECLARED
#define Tcl_LogCommandInfo_TCL_DECLARED

EXTERN void		Tcl_LogCommandInfo(Tcl_Interp *interp,
				CONST char *script, CONST char *command,
				int length);
#endif
#ifndef Tcl_ParseBraces_TCL_DECLARED
#define Tcl_ParseBraces_TCL_DECLARED

EXTERN int		Tcl_ParseBraces(Tcl_Interp *interp,
				CONST char *start, int numBytes,
				Tcl_Parse *parsePtr, int append,
				CONST84 char **termPtr);
#endif
#ifndef Tcl_ParseCommand_TCL_DECLARED
#define Tcl_ParseCommand_TCL_DECLARED

EXTERN int		Tcl_ParseCommand(Tcl_Interp *interp,
				CONST char *start, int numBytes, int nested,
				Tcl_Parse *parsePtr);
#endif
#ifndef Tcl_ParseExpr_TCL_DECLARED
#define Tcl_ParseExpr_TCL_DECLARED

EXTERN int		Tcl_ParseExpr(Tcl_Interp *interp, CONST char *start,
				int numBytes, Tcl_Parse *parsePtr);
#endif
#ifndef Tcl_ParseQuotedString_TCL_DECLARED
#define Tcl_ParseQuotedString_TCL_DECLARED

EXTERN int		Tcl_ParseQuotedString(Tcl_Interp *interp,
				CONST char *start, int numBytes,
				Tcl_Parse *parsePtr, int append,
				CONST84 char **termPtr);
#endif
#ifndef Tcl_ParseVarName_TCL_DECLARED
#define Tcl_ParseVarName_TCL_DECLARED

EXTERN int		Tcl_ParseVarName(Tcl_Interp *interp,
				CONST char *start, int numBytes,
				Tcl_Parse *parsePtr, int append);
#endif
#ifndef Tcl_GetCwd_TCL_DECLARED
#define Tcl_GetCwd_TCL_DECLARED

EXTERN char *		Tcl_GetCwd(Tcl_Interp *interp, Tcl_DString *cwdPtr);
#endif
#ifndef Tcl_Chdir_TCL_DECLARED
#define Tcl_Chdir_TCL_DECLARED

EXTERN int		Tcl_Chdir(CONST char *dirName);
#endif
#ifndef Tcl_Access_TCL_DECLARED
#define Tcl_Access_TCL_DECLARED

EXTERN int		Tcl_Access(CONST char *path, int mode);
#endif
#ifndef Tcl_Stat_TCL_DECLARED
#define Tcl_Stat_TCL_DECLARED

EXTERN int		Tcl_Stat(CONST char *path, struct stat *bufPtr);
#endif
#ifndef Tcl_UtfNcmp_TCL_DECLARED
#define Tcl_UtfNcmp_TCL_DECLARED

EXTERN int		Tcl_UtfNcmp(CONST char *s1, CONST char *s2,
				unsigned long n);
#endif
#ifndef Tcl_UtfNcasecmp_TCL_DECLARED
#define Tcl_UtfNcasecmp_TCL_DECLARED

EXTERN int		Tcl_UtfNcasecmp(CONST char *s1, CONST char *s2,
				unsigned long n);
#endif
#ifndef Tcl_StringCaseMatch_TCL_DECLARED
#define Tcl_StringCaseMatch_TCL_DECLARED

EXTERN int		Tcl_StringCaseMatch(CONST char *str,
				CONST char *pattern, int nocase);
#endif
#ifndef Tcl_UniCharIsControl_TCL_DECLARED
#define Tcl_UniCharIsControl_TCL_DECLARED

EXTERN int		Tcl_UniCharIsControl(int ch);
#endif
#ifndef Tcl_UniCharIsGraph_TCL_DECLARED
#define Tcl_UniCharIsGraph_TCL_DECLARED

EXTERN int		Tcl_UniCharIsGraph(int ch);
#endif
#ifndef Tcl_UniCharIsPrint_TCL_DECLARED
#define Tcl_UniCharIsPrint_TCL_DECLARED

EXTERN int		Tcl_UniCharIsPrint(int ch);
#endif
#ifndef Tcl_UniCharIsPunct_TCL_DECLARED
#define Tcl_UniCharIsPunct_TCL_DECLARED

EXTERN int		Tcl_UniCharIsPunct(int ch);
#endif
#ifndef Tcl_RegExpExecObj_TCL_DECLARED
#define Tcl_RegExpExecObj_TCL_DECLARED

EXTERN int		Tcl_RegExpExecObj(Tcl_Interp *interp,
				Tcl_RegExp regexp, Tcl_Obj *textObj,
				int offset, int nmatches, int flags);
#endif
#ifndef Tcl_RegExpGetInfo_TCL_DECLARED
#define Tcl_RegExpGetInfo_TCL_DECLARED

EXTERN void		Tcl_RegExpGetInfo(Tcl_RegExp regexp,
				Tcl_RegExpInfo *infoPtr);
#endif
#ifndef Tcl_NewUnicodeObj_TCL_DECLARED
#define Tcl_NewUnicodeObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewUnicodeObj(CONST Tcl_UniChar *unicode,
				int numChars);
#endif
#ifndef Tcl_SetUnicodeObj_TCL_DECLARED
#define Tcl_SetUnicodeObj_TCL_DECLARED

EXTERN void		Tcl_SetUnicodeObj(Tcl_Obj *objPtr,
				CONST Tcl_UniChar *unicode, int numChars);
#endif
#ifndef Tcl_GetCharLength_TCL_DECLARED
#define Tcl_GetCharLength_TCL_DECLARED

EXTERN int		Tcl_GetCharLength(Tcl_Obj *objPtr);
#endif
#ifndef Tcl_GetUniChar_TCL_DECLARED
#define Tcl_GetUniChar_TCL_DECLARED

EXTERN Tcl_UniChar	Tcl_GetUniChar(Tcl_Obj *objPtr, int index);
#endif
#ifndef Tcl_GetUnicode_TCL_DECLARED
#define Tcl_GetUnicode_TCL_DECLARED

EXTERN Tcl_UniChar *	Tcl_GetUnicode(Tcl_Obj *objPtr);
#endif
#ifndef Tcl_GetRange_TCL_DECLARED
#define Tcl_GetRange_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_GetRange(Tcl_Obj *objPtr, int first, int last);
#endif
#ifndef Tcl_AppendUnicodeToObj_TCL_DECLARED
#define Tcl_AppendUnicodeToObj_TCL_DECLARED

EXTERN void		Tcl_AppendUnicodeToObj(Tcl_Obj *objPtr,
				CONST Tcl_UniChar *unicode, int length);
#endif
#ifndef Tcl_RegExpMatchObj_TCL_DECLARED
#define Tcl_RegExpMatchObj_TCL_DECLARED

EXTERN int		Tcl_RegExpMatchObj(Tcl_Interp *interp,
				Tcl_Obj *textObj, Tcl_Obj *patternObj);
#endif
#ifndef Tcl_SetNotifier_TCL_DECLARED
#define Tcl_SetNotifier_TCL_DECLARED

EXTERN void		Tcl_SetNotifier(Tcl_NotifierProcs *notifierProcPtr);
#endif
#ifndef Tcl_GetAllocMutex_TCL_DECLARED
#define Tcl_GetAllocMutex_TCL_DECLARED

EXTERN Tcl_Mutex *	Tcl_GetAllocMutex(void);
#endif
#ifndef Tcl_GetChannelNames_TCL_DECLARED
#define Tcl_GetChannelNames_TCL_DECLARED

EXTERN int		Tcl_GetChannelNames(Tcl_Interp *interp);
#endif
#ifndef Tcl_GetChannelNamesEx_TCL_DECLARED
#define Tcl_GetChannelNamesEx_TCL_DECLARED

EXTERN int		Tcl_GetChannelNamesEx(Tcl_Interp *interp,
				CONST char *pattern);
#endif
#ifndef Tcl_ProcObjCmd_TCL_DECLARED
#define Tcl_ProcObjCmd_TCL_DECLARED

EXTERN int		Tcl_ProcObjCmd(ClientData clientData,
				Tcl_Interp *interp, int objc,
				Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_ConditionFinalize_TCL_DECLARED
#define Tcl_ConditionFinalize_TCL_DECLARED

EXTERN void		Tcl_ConditionFinalize(Tcl_Condition *condPtr);
#endif
#ifndef Tcl_MutexFinalize_TCL_DECLARED
#define Tcl_MutexFinalize_TCL_DECLARED

EXTERN void		Tcl_MutexFinalize(Tcl_Mutex *mutex);
#endif
#ifndef Tcl_CreateThread_TCL_DECLARED
#define Tcl_CreateThread_TCL_DECLARED

EXTERN int		Tcl_CreateThread(Tcl_ThreadId *idPtr,
				Tcl_ThreadCreateProc proc,
				ClientData clientData, int stackSize,
				int flags);
#endif
#ifndef Tcl_ReadRaw_TCL_DECLARED
#define Tcl_ReadRaw_TCL_DECLARED

EXTERN int		Tcl_ReadRaw(Tcl_Channel chan, char *dst,
				int bytesToRead);
#endif
#ifndef Tcl_WriteRaw_TCL_DECLARED
#define Tcl_WriteRaw_TCL_DECLARED

EXTERN int		Tcl_WriteRaw(Tcl_Channel chan, CONST char *src,
				int srcLen);
#endif
#ifndef Tcl_GetTopChannel_TCL_DECLARED
#define Tcl_GetTopChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_GetTopChannel(Tcl_Channel chan);
#endif
#ifndef Tcl_ChannelBuffered_TCL_DECLARED
#define Tcl_ChannelBuffered_TCL_DECLARED

EXTERN int		Tcl_ChannelBuffered(Tcl_Channel chan);
#endif
#ifndef Tcl_ChannelName_TCL_DECLARED
#define Tcl_ChannelName_TCL_DECLARED

EXTERN CONST84_RETURN char * Tcl_ChannelName(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelVersion_TCL_DECLARED
#define Tcl_ChannelVersion_TCL_DECLARED

EXTERN Tcl_ChannelTypeVersion Tcl_ChannelVersion(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelBlockModeProc_TCL_DECLARED
#define Tcl_ChannelBlockModeProc_TCL_DECLARED

EXTERN Tcl_DriverBlockModeProc * Tcl_ChannelBlockModeProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelCloseProc_TCL_DECLARED
#define Tcl_ChannelCloseProc_TCL_DECLARED

EXTERN Tcl_DriverCloseProc * Tcl_ChannelCloseProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelClose2Proc_TCL_DECLARED
#define Tcl_ChannelClose2Proc_TCL_DECLARED

EXTERN Tcl_DriverClose2Proc * Tcl_ChannelClose2Proc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelInputProc_TCL_DECLARED
#define Tcl_ChannelInputProc_TCL_DECLARED

EXTERN Tcl_DriverInputProc * Tcl_ChannelInputProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelOutputProc_TCL_DECLARED
#define Tcl_ChannelOutputProc_TCL_DECLARED

EXTERN Tcl_DriverOutputProc * Tcl_ChannelOutputProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelSeekProc_TCL_DECLARED
#define Tcl_ChannelSeekProc_TCL_DECLARED

EXTERN Tcl_DriverSeekProc * Tcl_ChannelSeekProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelSetOptionProc_TCL_DECLARED
#define Tcl_ChannelSetOptionProc_TCL_DECLARED

EXTERN Tcl_DriverSetOptionProc * Tcl_ChannelSetOptionProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelGetOptionProc_TCL_DECLARED
#define Tcl_ChannelGetOptionProc_TCL_DECLARED

EXTERN Tcl_DriverGetOptionProc * Tcl_ChannelGetOptionProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelWatchProc_TCL_DECLARED
#define Tcl_ChannelWatchProc_TCL_DECLARED

EXTERN Tcl_DriverWatchProc * Tcl_ChannelWatchProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelGetHandleProc_TCL_DECLARED
#define Tcl_ChannelGetHandleProc_TCL_DECLARED

EXTERN Tcl_DriverGetHandleProc * Tcl_ChannelGetHandleProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelFlushProc_TCL_DECLARED
#define Tcl_ChannelFlushProc_TCL_DECLARED

EXTERN Tcl_DriverFlushProc * Tcl_ChannelFlushProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_ChannelHandlerProc_TCL_DECLARED
#define Tcl_ChannelHandlerProc_TCL_DECLARED

EXTERN Tcl_DriverHandlerProc * Tcl_ChannelHandlerProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_JoinThread_TCL_DECLARED
#define Tcl_JoinThread_TCL_DECLARED

EXTERN int		Tcl_JoinThread(Tcl_ThreadId threadId, int *result);
#endif
#ifndef Tcl_IsChannelShared_TCL_DECLARED
#define Tcl_IsChannelShared_TCL_DECLARED

EXTERN int		Tcl_IsChannelShared(Tcl_Channel channel);
#endif
#ifndef Tcl_IsChannelRegistered_TCL_DECLARED
#define Tcl_IsChannelRegistered_TCL_DECLARED

EXTERN int		Tcl_IsChannelRegistered(Tcl_Interp *interp,
				Tcl_Channel channel);
#endif
#ifndef Tcl_CutChannel_TCL_DECLARED
#define Tcl_CutChannel_TCL_DECLARED

EXTERN void		Tcl_CutChannel(Tcl_Channel channel);
#endif
#ifndef Tcl_SpliceChannel_TCL_DECLARED
#define Tcl_SpliceChannel_TCL_DECLARED

EXTERN void		Tcl_SpliceChannel(Tcl_Channel channel);
#endif
#ifndef Tcl_ClearChannelHandlers_TCL_DECLARED
#define Tcl_ClearChannelHandlers_TCL_DECLARED

EXTERN void		Tcl_ClearChannelHandlers(Tcl_Channel channel);
#endif
#ifndef Tcl_IsChannelExisting_TCL_DECLARED
#define Tcl_IsChannelExisting_TCL_DECLARED

EXTERN int		Tcl_IsChannelExisting(CONST char *channelName);
#endif
#ifndef Tcl_UniCharNcasecmp_TCL_DECLARED
#define Tcl_UniCharNcasecmp_TCL_DECLARED

EXTERN int		Tcl_UniCharNcasecmp(CONST Tcl_UniChar *ucs,
				CONST Tcl_UniChar *uct,
				unsigned long numChars);
#endif
#ifndef Tcl_UniCharCaseMatch_TCL_DECLARED
#define Tcl_UniCharCaseMatch_TCL_DECLARED

EXTERN int		Tcl_UniCharCaseMatch(CONST Tcl_UniChar *uniStr,
				CONST Tcl_UniChar *uniPattern, int nocase);
#endif
#ifndef Tcl_FindHashEntry_TCL_DECLARED
#define Tcl_FindHashEntry_TCL_DECLARED

EXTERN Tcl_HashEntry *	Tcl_FindHashEntry(Tcl_HashTable *tablePtr,
				CONST char *key);
#endif
#ifndef Tcl_CreateHashEntry_TCL_DECLARED
#define Tcl_CreateHashEntry_TCL_DECLARED

EXTERN Tcl_HashEntry *	Tcl_CreateHashEntry(Tcl_HashTable *tablePtr,
				CONST char *key, int *newPtr);
#endif
#ifndef Tcl_InitCustomHashTable_TCL_DECLARED
#define Tcl_InitCustomHashTable_TCL_DECLARED

EXTERN void		Tcl_InitCustomHashTable(Tcl_HashTable *tablePtr,
				int keyType, Tcl_HashKeyType *typePtr);
#endif
#ifndef Tcl_InitObjHashTable_TCL_DECLARED
#define Tcl_InitObjHashTable_TCL_DECLARED

EXTERN void		Tcl_InitObjHashTable(Tcl_HashTable *tablePtr);
#endif
#ifndef Tcl_CommandTraceInfo_TCL_DECLARED
#define Tcl_CommandTraceInfo_TCL_DECLARED

EXTERN ClientData	Tcl_CommandTraceInfo(Tcl_Interp *interp,
				CONST char *varName, int flags,
				Tcl_CommandTraceProc *procPtr,
				ClientData prevClientData);
#endif
#ifndef Tcl_TraceCommand_TCL_DECLARED
#define Tcl_TraceCommand_TCL_DECLARED

EXTERN int		Tcl_TraceCommand(Tcl_Interp *interp,
				CONST char *varName, int flags,
				Tcl_CommandTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_UntraceCommand_TCL_DECLARED
#define Tcl_UntraceCommand_TCL_DECLARED

EXTERN void		Tcl_UntraceCommand(Tcl_Interp *interp,
				CONST char *varName, int flags,
				Tcl_CommandTraceProc *proc,
				ClientData clientData);
#endif
#ifndef Tcl_AttemptAlloc_TCL_DECLARED
#define Tcl_AttemptAlloc_TCL_DECLARED

EXTERN char *		Tcl_AttemptAlloc(unsigned int size);
#endif
#ifndef Tcl_AttemptDbCkalloc_TCL_DECLARED
#define Tcl_AttemptDbCkalloc_TCL_DECLARED

EXTERN char *		Tcl_AttemptDbCkalloc(unsigned int size,
				CONST char *file, int line);
#endif
#ifndef Tcl_AttemptRealloc_TCL_DECLARED
#define Tcl_AttemptRealloc_TCL_DECLARED

EXTERN char *		Tcl_AttemptRealloc(char *ptr, unsigned int size);
#endif
#ifndef Tcl_AttemptDbCkrealloc_TCL_DECLARED
#define Tcl_AttemptDbCkrealloc_TCL_DECLARED

EXTERN char *		Tcl_AttemptDbCkrealloc(char *ptr, unsigned int size,
				CONST char *file, int line);
#endif
#ifndef Tcl_AttemptSetObjLength_TCL_DECLARED
#define Tcl_AttemptSetObjLength_TCL_DECLARED

EXTERN int		Tcl_AttemptSetObjLength(Tcl_Obj *objPtr, int length);
#endif
#ifndef Tcl_GetChannelThread_TCL_DECLARED
#define Tcl_GetChannelThread_TCL_DECLARED

EXTERN Tcl_ThreadId	Tcl_GetChannelThread(Tcl_Channel channel);
#endif
#ifndef Tcl_GetUnicodeFromObj_TCL_DECLARED
#define Tcl_GetUnicodeFromObj_TCL_DECLARED

EXTERN Tcl_UniChar *	Tcl_GetUnicodeFromObj(Tcl_Obj *objPtr,
				int *lengthPtr);
#endif
#ifndef Tcl_GetMathFuncInfo_TCL_DECLARED
#define Tcl_GetMathFuncInfo_TCL_DECLARED

EXTERN int		Tcl_GetMathFuncInfo(Tcl_Interp *interp,
				CONST char *name, int *numArgsPtr,
				Tcl_ValueType **argTypesPtr,
				Tcl_MathProc **procPtr,
				ClientData *clientDataPtr);
#endif
#ifndef Tcl_ListMathFuncs_TCL_DECLARED
#define Tcl_ListMathFuncs_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_ListMathFuncs(Tcl_Interp *interp,
				CONST char *pattern);
#endif
#ifndef Tcl_SubstObj_TCL_DECLARED
#define Tcl_SubstObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_SubstObj(Tcl_Interp *interp, Tcl_Obj *objPtr,
				int flags);
#endif
#ifndef Tcl_DetachChannel_TCL_DECLARED
#define Tcl_DetachChannel_TCL_DECLARED

EXTERN int		Tcl_DetachChannel(Tcl_Interp *interp,
				Tcl_Channel channel);
#endif
#ifndef Tcl_IsStandardChannel_TCL_DECLARED
#define Tcl_IsStandardChannel_TCL_DECLARED

EXTERN int		Tcl_IsStandardChannel(Tcl_Channel channel);
#endif
#ifndef Tcl_FSCopyFile_TCL_DECLARED
#define Tcl_FSCopyFile_TCL_DECLARED

EXTERN int		Tcl_FSCopyFile(Tcl_Obj *srcPathPtr,
				Tcl_Obj *destPathPtr);
#endif
#ifndef Tcl_FSCopyDirectory_TCL_DECLARED
#define Tcl_FSCopyDirectory_TCL_DECLARED

EXTERN int		Tcl_FSCopyDirectory(Tcl_Obj *srcPathPtr,
				Tcl_Obj *destPathPtr, Tcl_Obj **errorPtr);
#endif
#ifndef Tcl_FSCreateDirectory_TCL_DECLARED
#define Tcl_FSCreateDirectory_TCL_DECLARED

EXTERN int		Tcl_FSCreateDirectory(Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSDeleteFile_TCL_DECLARED
#define Tcl_FSDeleteFile_TCL_DECLARED

EXTERN int		Tcl_FSDeleteFile(Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSLoadFile_TCL_DECLARED
#define Tcl_FSLoadFile_TCL_DECLARED

EXTERN int		Tcl_FSLoadFile(Tcl_Interp *interp, Tcl_Obj *pathPtr,
				CONST char *sym1, CONST char *sym2,
				Tcl_PackageInitProc **proc1Ptr,
				Tcl_PackageInitProc **proc2Ptr,
				Tcl_LoadHandle *handlePtr,
				Tcl_FSUnloadFileProc **unloadProcPtr);
#endif
#ifndef Tcl_FSMatchInDirectory_TCL_DECLARED
#define Tcl_FSMatchInDirectory_TCL_DECLARED

EXTERN int		Tcl_FSMatchInDirectory(Tcl_Interp *interp,
				Tcl_Obj *result, Tcl_Obj *pathPtr,
				CONST char *pattern, Tcl_GlobTypeData *types);
#endif
#ifndef Tcl_FSLink_TCL_DECLARED
#define Tcl_FSLink_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSLink(Tcl_Obj *pathPtr, Tcl_Obj *toPtr,
				int linkAction);
#endif
#ifndef Tcl_FSRemoveDirectory_TCL_DECLARED
#define Tcl_FSRemoveDirectory_TCL_DECLARED

EXTERN int		Tcl_FSRemoveDirectory(Tcl_Obj *pathPtr,
				int recursive, Tcl_Obj **errorPtr);
#endif
#ifndef Tcl_FSRenameFile_TCL_DECLARED
#define Tcl_FSRenameFile_TCL_DECLARED

EXTERN int		Tcl_FSRenameFile(Tcl_Obj *srcPathPtr,
				Tcl_Obj *destPathPtr);
#endif
#ifndef Tcl_FSLstat_TCL_DECLARED
#define Tcl_FSLstat_TCL_DECLARED

EXTERN int		Tcl_FSLstat(Tcl_Obj *pathPtr, Tcl_StatBuf *buf);
#endif
#ifndef Tcl_FSUtime_TCL_DECLARED
#define Tcl_FSUtime_TCL_DECLARED

EXTERN int		Tcl_FSUtime(Tcl_Obj *pathPtr, struct utimbuf *tval);
#endif
#ifndef Tcl_FSFileAttrsGet_TCL_DECLARED
#define Tcl_FSFileAttrsGet_TCL_DECLARED

EXTERN int		Tcl_FSFileAttrsGet(Tcl_Interp *interp, int index,
				Tcl_Obj *pathPtr, Tcl_Obj **objPtrRef);
#endif
#ifndef Tcl_FSFileAttrsSet_TCL_DECLARED
#define Tcl_FSFileAttrsSet_TCL_DECLARED

EXTERN int		Tcl_FSFileAttrsSet(Tcl_Interp *interp, int index,
				Tcl_Obj *pathPtr, Tcl_Obj *objPtr);
#endif
#ifndef Tcl_FSFileAttrStrings_TCL_DECLARED
#define Tcl_FSFileAttrStrings_TCL_DECLARED

EXTERN CONST char **	Tcl_FSFileAttrStrings(Tcl_Obj *pathPtr,
				Tcl_Obj **objPtrRef);
#endif
#ifndef Tcl_FSStat_TCL_DECLARED
#define Tcl_FSStat_TCL_DECLARED

EXTERN int		Tcl_FSStat(Tcl_Obj *pathPtr, Tcl_StatBuf *buf);
#endif
#ifndef Tcl_FSAccess_TCL_DECLARED
#define Tcl_FSAccess_TCL_DECLARED

EXTERN int		Tcl_FSAccess(Tcl_Obj *pathPtr, int mode);
#endif
#ifndef Tcl_FSOpenFileChannel_TCL_DECLARED
#define Tcl_FSOpenFileChannel_TCL_DECLARED

EXTERN Tcl_Channel	Tcl_FSOpenFileChannel(Tcl_Interp *interp,
				Tcl_Obj *pathPtr, CONST char *modeString,
				int permissions);
#endif
#ifndef Tcl_FSGetCwd_TCL_DECLARED
#define Tcl_FSGetCwd_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSGetCwd(Tcl_Interp *interp);
#endif
#ifndef Tcl_FSChdir_TCL_DECLARED
#define Tcl_FSChdir_TCL_DECLARED

EXTERN int		Tcl_FSChdir(Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSConvertToPathType_TCL_DECLARED
#define Tcl_FSConvertToPathType_TCL_DECLARED

EXTERN int		Tcl_FSConvertToPathType(Tcl_Interp *interp,
				Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSJoinPath_TCL_DECLARED
#define Tcl_FSJoinPath_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSJoinPath(Tcl_Obj *listObj, int elements);
#endif
#ifndef Tcl_FSSplitPath_TCL_DECLARED
#define Tcl_FSSplitPath_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSSplitPath(Tcl_Obj *pathPtr, int *lenPtr);
#endif
#ifndef Tcl_FSEqualPaths_TCL_DECLARED
#define Tcl_FSEqualPaths_TCL_DECLARED

EXTERN int		Tcl_FSEqualPaths(Tcl_Obj *firstPtr,
				Tcl_Obj *secondPtr);
#endif
#ifndef Tcl_FSGetNormalizedPath_TCL_DECLARED
#define Tcl_FSGetNormalizedPath_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSGetNormalizedPath(Tcl_Interp *interp,
				Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSJoinToPath_TCL_DECLARED
#define Tcl_FSJoinToPath_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSJoinToPath(Tcl_Obj *pathPtr, int objc,
				Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_FSGetInternalRep_TCL_DECLARED
#define Tcl_FSGetInternalRep_TCL_DECLARED

EXTERN ClientData	Tcl_FSGetInternalRep(Tcl_Obj *pathPtr,
				Tcl_Filesystem *fsPtr);
#endif
#ifndef Tcl_FSGetTranslatedPath_TCL_DECLARED
#define Tcl_FSGetTranslatedPath_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSGetTranslatedPath(Tcl_Interp *interp,
				Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSEvalFile_TCL_DECLARED
#define Tcl_FSEvalFile_TCL_DECLARED

EXTERN int		Tcl_FSEvalFile(Tcl_Interp *interp, Tcl_Obj *fileName);
#endif
#ifndef Tcl_FSNewNativePath_TCL_DECLARED
#define Tcl_FSNewNativePath_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSNewNativePath(Tcl_Filesystem *fromFilesystem,
				ClientData clientData);
#endif
#ifndef Tcl_FSGetNativePath_TCL_DECLARED
#define Tcl_FSGetNativePath_TCL_DECLARED

EXTERN CONST char *	Tcl_FSGetNativePath(Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSFileSystemInfo_TCL_DECLARED
#define Tcl_FSFileSystemInfo_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSFileSystemInfo(Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSPathSeparator_TCL_DECLARED
#define Tcl_FSPathSeparator_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSPathSeparator(Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSListVolumes_TCL_DECLARED
#define Tcl_FSListVolumes_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_FSListVolumes(void);
#endif
#ifndef Tcl_FSRegister_TCL_DECLARED
#define Tcl_FSRegister_TCL_DECLARED

EXTERN int		Tcl_FSRegister(ClientData clientData,
				Tcl_Filesystem *fsPtr);
#endif
#ifndef Tcl_FSUnregister_TCL_DECLARED
#define Tcl_FSUnregister_TCL_DECLARED

EXTERN int		Tcl_FSUnregister(Tcl_Filesystem *fsPtr);
#endif
#ifndef Tcl_FSData_TCL_DECLARED
#define Tcl_FSData_TCL_DECLARED

EXTERN ClientData	Tcl_FSData(Tcl_Filesystem *fsPtr);
#endif
#ifndef Tcl_FSGetTranslatedStringPath_TCL_DECLARED
#define Tcl_FSGetTranslatedStringPath_TCL_DECLARED

EXTERN CONST char *	Tcl_FSGetTranslatedStringPath(Tcl_Interp *interp,
				Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSGetFileSystemForPath_TCL_DECLARED
#define Tcl_FSGetFileSystemForPath_TCL_DECLARED

EXTERN Tcl_Filesystem *	 Tcl_FSGetFileSystemForPath(Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_FSGetPathType_TCL_DECLARED
#define Tcl_FSGetPathType_TCL_DECLARED

EXTERN Tcl_PathType	Tcl_FSGetPathType(Tcl_Obj *pathPtr);
#endif
#ifndef Tcl_OutputBuffered_TCL_DECLARED
#define Tcl_OutputBuffered_TCL_DECLARED

EXTERN int		Tcl_OutputBuffered(Tcl_Channel chan);
#endif
#ifndef Tcl_FSMountsChanged_TCL_DECLARED
#define Tcl_FSMountsChanged_TCL_DECLARED

EXTERN void		Tcl_FSMountsChanged(Tcl_Filesystem *fsPtr);
#endif
#ifndef Tcl_EvalTokensStandard_TCL_DECLARED
#define Tcl_EvalTokensStandard_TCL_DECLARED

EXTERN int		Tcl_EvalTokensStandard(Tcl_Interp *interp,
				Tcl_Token *tokenPtr, int count);
#endif
#ifndef Tcl_GetTime_TCL_DECLARED
#define Tcl_GetTime_TCL_DECLARED

EXTERN void		Tcl_GetTime(Tcl_Time *timeBuf);
#endif
#ifndef Tcl_CreateObjTrace_TCL_DECLARED
#define Tcl_CreateObjTrace_TCL_DECLARED

EXTERN Tcl_Trace	Tcl_CreateObjTrace(Tcl_Interp *interp, int level,
				int flags, Tcl_CmdObjTraceProc *objProc,
				ClientData clientData,
				Tcl_CmdObjTraceDeleteProc *delProc);
#endif
#ifndef Tcl_GetCommandInfoFromToken_TCL_DECLARED
#define Tcl_GetCommandInfoFromToken_TCL_DECLARED

EXTERN int		Tcl_GetCommandInfoFromToken(Tcl_Command token,
				Tcl_CmdInfo *infoPtr);
#endif
#ifndef Tcl_SetCommandInfoFromToken_TCL_DECLARED
#define Tcl_SetCommandInfoFromToken_TCL_DECLARED

EXTERN int		Tcl_SetCommandInfoFromToken(Tcl_Command token,
				CONST Tcl_CmdInfo *infoPtr);
#endif
#ifndef Tcl_DbNewWideIntObj_TCL_DECLARED
#define Tcl_DbNewWideIntObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewWideIntObj(Tcl_WideInt wideValue,
				CONST char *file, int line);
#endif
#ifndef Tcl_GetWideIntFromObj_TCL_DECLARED
#define Tcl_GetWideIntFromObj_TCL_DECLARED

EXTERN int		Tcl_GetWideIntFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, Tcl_WideInt *widePtr);
#endif
#ifndef Tcl_NewWideIntObj_TCL_DECLARED
#define Tcl_NewWideIntObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewWideIntObj(Tcl_WideInt wideValue);
#endif
#ifndef Tcl_SetWideIntObj_TCL_DECLARED
#define Tcl_SetWideIntObj_TCL_DECLARED

EXTERN void		Tcl_SetWideIntObj(Tcl_Obj *objPtr,
				Tcl_WideInt wideValue);
#endif
#ifndef Tcl_AllocStatBuf_TCL_DECLARED
#define Tcl_AllocStatBuf_TCL_DECLARED

EXTERN Tcl_StatBuf *	Tcl_AllocStatBuf(void);
#endif
#ifndef Tcl_Seek_TCL_DECLARED
#define Tcl_Seek_TCL_DECLARED

EXTERN Tcl_WideInt	Tcl_Seek(Tcl_Channel chan, Tcl_WideInt offset,
				int mode);
#endif
#ifndef Tcl_Tell_TCL_DECLARED
#define Tcl_Tell_TCL_DECLARED

EXTERN Tcl_WideInt	Tcl_Tell(Tcl_Channel chan);
#endif
#ifndef Tcl_ChannelWideSeekProc_TCL_DECLARED
#define Tcl_ChannelWideSeekProc_TCL_DECLARED

EXTERN Tcl_DriverWideSeekProc * Tcl_ChannelWideSeekProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_DictObjPut_TCL_DECLARED
#define Tcl_DictObjPut_TCL_DECLARED

EXTERN int		Tcl_DictObjPut(Tcl_Interp *interp, Tcl_Obj *dictPtr,
				Tcl_Obj *keyPtr, Tcl_Obj *valuePtr);
#endif
#ifndef Tcl_DictObjGet_TCL_DECLARED
#define Tcl_DictObjGet_TCL_DECLARED

EXTERN int		Tcl_DictObjGet(Tcl_Interp *interp, Tcl_Obj *dictPtr,
				Tcl_Obj *keyPtr, Tcl_Obj **valuePtrPtr);
#endif
#ifndef Tcl_DictObjRemove_TCL_DECLARED
#define Tcl_DictObjRemove_TCL_DECLARED

EXTERN int		Tcl_DictObjRemove(Tcl_Interp *interp,
				Tcl_Obj *dictPtr, Tcl_Obj *keyPtr);
#endif
#ifndef Tcl_DictObjSize_TCL_DECLARED
#define Tcl_DictObjSize_TCL_DECLARED

EXTERN int		Tcl_DictObjSize(Tcl_Interp *interp, Tcl_Obj *dictPtr,
				int *sizePtr);
#endif
#ifndef Tcl_DictObjFirst_TCL_DECLARED
#define Tcl_DictObjFirst_TCL_DECLARED

EXTERN int		Tcl_DictObjFirst(Tcl_Interp *interp,
				Tcl_Obj *dictPtr, Tcl_DictSearch *searchPtr,
				Tcl_Obj **keyPtrPtr, Tcl_Obj **valuePtrPtr,
				int *donePtr);
#endif
#ifndef Tcl_DictObjNext_TCL_DECLARED
#define Tcl_DictObjNext_TCL_DECLARED

EXTERN void		Tcl_DictObjNext(Tcl_DictSearch *searchPtr,
				Tcl_Obj **keyPtrPtr, Tcl_Obj **valuePtrPtr,
				int *donePtr);
#endif
#ifndef Tcl_DictObjDone_TCL_DECLARED
#define Tcl_DictObjDone_TCL_DECLARED

EXTERN void		Tcl_DictObjDone(Tcl_DictSearch *searchPtr);
#endif
#ifndef Tcl_DictObjPutKeyList_TCL_DECLARED
#define Tcl_DictObjPutKeyList_TCL_DECLARED

EXTERN int		Tcl_DictObjPutKeyList(Tcl_Interp *interp,
				Tcl_Obj *dictPtr, int keyc,
				Tcl_Obj *CONST *keyv, Tcl_Obj *valuePtr);
#endif
#ifndef Tcl_DictObjRemoveKeyList_TCL_DECLARED
#define Tcl_DictObjRemoveKeyList_TCL_DECLARED

EXTERN int		Tcl_DictObjRemoveKeyList(Tcl_Interp *interp,
				Tcl_Obj *dictPtr, int keyc,
				Tcl_Obj *CONST *keyv);
#endif
#ifndef Tcl_NewDictObj_TCL_DECLARED
#define Tcl_NewDictObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewDictObj(void);
#endif
#ifndef Tcl_DbNewDictObj_TCL_DECLARED
#define Tcl_DbNewDictObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewDictObj(CONST char *file, int line);
#endif
#ifndef Tcl_RegisterConfig_TCL_DECLARED
#define Tcl_RegisterConfig_TCL_DECLARED

EXTERN void		Tcl_RegisterConfig(Tcl_Interp *interp,
				CONST char *pkgName,
				Tcl_Config *configuration,
				CONST char *valEncoding);
#endif
#ifndef Tcl_CreateNamespace_TCL_DECLARED
#define Tcl_CreateNamespace_TCL_DECLARED

EXTERN Tcl_Namespace *	Tcl_CreateNamespace(Tcl_Interp *interp,
				CONST char *name, ClientData clientData,
				Tcl_NamespaceDeleteProc *deleteProc);
#endif
#ifndef Tcl_DeleteNamespace_TCL_DECLARED
#define Tcl_DeleteNamespace_TCL_DECLARED

EXTERN void		Tcl_DeleteNamespace(Tcl_Namespace *nsPtr);
#endif
#ifndef Tcl_AppendExportList_TCL_DECLARED
#define Tcl_AppendExportList_TCL_DECLARED

EXTERN int		Tcl_AppendExportList(Tcl_Interp *interp,
				Tcl_Namespace *nsPtr, Tcl_Obj *objPtr);
#endif
#ifndef Tcl_Export_TCL_DECLARED
#define Tcl_Export_TCL_DECLARED

EXTERN int		Tcl_Export(Tcl_Interp *interp, Tcl_Namespace *nsPtr,
				CONST char *pattern, int resetListFirst);
#endif
#ifndef Tcl_Import_TCL_DECLARED
#define Tcl_Import_TCL_DECLARED

EXTERN int		Tcl_Import(Tcl_Interp *interp, Tcl_Namespace *nsPtr,
				CONST char *pattern, int allowOverwrite);
#endif
#ifndef Tcl_ForgetImport_TCL_DECLARED
#define Tcl_ForgetImport_TCL_DECLARED

EXTERN int		Tcl_ForgetImport(Tcl_Interp *interp,
				Tcl_Namespace *nsPtr, CONST char *pattern);
#endif
#ifndef Tcl_GetCurrentNamespace_TCL_DECLARED
#define Tcl_GetCurrentNamespace_TCL_DECLARED

EXTERN Tcl_Namespace *	Tcl_GetCurrentNamespace(Tcl_Interp *interp);
#endif
#ifndef Tcl_GetGlobalNamespace_TCL_DECLARED
#define Tcl_GetGlobalNamespace_TCL_DECLARED

EXTERN Tcl_Namespace *	Tcl_GetGlobalNamespace(Tcl_Interp *interp);
#endif
#ifndef Tcl_FindNamespace_TCL_DECLARED
#define Tcl_FindNamespace_TCL_DECLARED

EXTERN Tcl_Namespace *	Tcl_FindNamespace(Tcl_Interp *interp,
				CONST char *name,
				Tcl_Namespace *contextNsPtr, int flags);
#endif
#ifndef Tcl_FindCommand_TCL_DECLARED
#define Tcl_FindCommand_TCL_DECLARED

EXTERN Tcl_Command	Tcl_FindCommand(Tcl_Interp *interp, CONST char *name,
				Tcl_Namespace *contextNsPtr, int flags);
#endif
#ifndef Tcl_GetCommandFromObj_TCL_DECLARED
#define Tcl_GetCommandFromObj_TCL_DECLARED

EXTERN Tcl_Command	Tcl_GetCommandFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr);
#endif
#ifndef Tcl_GetCommandFullName_TCL_DECLARED
#define Tcl_GetCommandFullName_TCL_DECLARED

EXTERN void		Tcl_GetCommandFullName(Tcl_Interp *interp,
				Tcl_Command command, Tcl_Obj *objPtr);
#endif
#ifndef Tcl_FSEvalFileEx_TCL_DECLARED
#define Tcl_FSEvalFileEx_TCL_DECLARED

EXTERN int		Tcl_FSEvalFileEx(Tcl_Interp *interp,
				Tcl_Obj *fileName, CONST char *encodingName);
#endif
#ifndef Tcl_SetExitProc_TCL_DECLARED
#define Tcl_SetExitProc_TCL_DECLARED

EXTERN Tcl_ExitProc *	Tcl_SetExitProc(Tcl_ExitProc *proc);
#endif
#ifndef Tcl_LimitAddHandler_TCL_DECLARED
#define Tcl_LimitAddHandler_TCL_DECLARED

EXTERN void		Tcl_LimitAddHandler(Tcl_Interp *interp, int type,
				Tcl_LimitHandlerProc *handlerProc,
				ClientData clientData,
				Tcl_LimitHandlerDeleteProc *deleteProc);
#endif
#ifndef Tcl_LimitRemoveHandler_TCL_DECLARED
#define Tcl_LimitRemoveHandler_TCL_DECLARED

EXTERN void		Tcl_LimitRemoveHandler(Tcl_Interp *interp, int type,
				Tcl_LimitHandlerProc *handlerProc,
				ClientData clientData);
#endif
#ifndef Tcl_LimitReady_TCL_DECLARED
#define Tcl_LimitReady_TCL_DECLARED

EXTERN int		Tcl_LimitReady(Tcl_Interp *interp);
#endif
#ifndef Tcl_LimitCheck_TCL_DECLARED
#define Tcl_LimitCheck_TCL_DECLARED

EXTERN int		Tcl_LimitCheck(Tcl_Interp *interp);
#endif
#ifndef Tcl_LimitExceeded_TCL_DECLARED
#define Tcl_LimitExceeded_TCL_DECLARED

EXTERN int		Tcl_LimitExceeded(Tcl_Interp *interp);
#endif
#ifndef Tcl_LimitSetCommands_TCL_DECLARED
#define Tcl_LimitSetCommands_TCL_DECLARED

EXTERN void		Tcl_LimitSetCommands(Tcl_Interp *interp,
				int commandLimit);
#endif
#ifndef Tcl_LimitSetTime_TCL_DECLARED
#define Tcl_LimitSetTime_TCL_DECLARED

EXTERN void		Tcl_LimitSetTime(Tcl_Interp *interp,
				Tcl_Time *timeLimitPtr);
#endif
#ifndef Tcl_LimitSetGranularity_TCL_DECLARED
#define Tcl_LimitSetGranularity_TCL_DECLARED

EXTERN void		Tcl_LimitSetGranularity(Tcl_Interp *interp, int type,
				int granularity);
#endif
#ifndef Tcl_LimitTypeEnabled_TCL_DECLARED
#define Tcl_LimitTypeEnabled_TCL_DECLARED

EXTERN int		Tcl_LimitTypeEnabled(Tcl_Interp *interp, int type);
#endif
#ifndef Tcl_LimitTypeExceeded_TCL_DECLARED
#define Tcl_LimitTypeExceeded_TCL_DECLARED

EXTERN int		Tcl_LimitTypeExceeded(Tcl_Interp *interp, int type);
#endif
#ifndef Tcl_LimitTypeSet_TCL_DECLARED
#define Tcl_LimitTypeSet_TCL_DECLARED

EXTERN void		Tcl_LimitTypeSet(Tcl_Interp *interp, int type);
#endif
#ifndef Tcl_LimitTypeReset_TCL_DECLARED
#define Tcl_LimitTypeReset_TCL_DECLARED

EXTERN void		Tcl_LimitTypeReset(Tcl_Interp *interp, int type);
#endif
#ifndef Tcl_LimitGetCommands_TCL_DECLARED
#define Tcl_LimitGetCommands_TCL_DECLARED

EXTERN int		Tcl_LimitGetCommands(Tcl_Interp *interp);
#endif
#ifndef Tcl_LimitGetTime_TCL_DECLARED
#define Tcl_LimitGetTime_TCL_DECLARED

EXTERN void		Tcl_LimitGetTime(Tcl_Interp *interp,
				Tcl_Time *timeLimitPtr);
#endif
#ifndef Tcl_LimitGetGranularity_TCL_DECLARED
#define Tcl_LimitGetGranularity_TCL_DECLARED

EXTERN int		Tcl_LimitGetGranularity(Tcl_Interp *interp, int type);
#endif
#ifndef Tcl_SaveInterpState_TCL_DECLARED
#define Tcl_SaveInterpState_TCL_DECLARED

EXTERN Tcl_InterpState	Tcl_SaveInterpState(Tcl_Interp *interp, int status);
#endif
#ifndef Tcl_RestoreInterpState_TCL_DECLARED
#define Tcl_RestoreInterpState_TCL_DECLARED

EXTERN int		Tcl_RestoreInterpState(Tcl_Interp *interp,
				Tcl_InterpState state);
#endif
#ifndef Tcl_DiscardInterpState_TCL_DECLARED
#define Tcl_DiscardInterpState_TCL_DECLARED

EXTERN void		Tcl_DiscardInterpState(Tcl_InterpState state);
#endif
#ifndef Tcl_SetReturnOptions_TCL_DECLARED
#define Tcl_SetReturnOptions_TCL_DECLARED

EXTERN int		Tcl_SetReturnOptions(Tcl_Interp *interp,
				Tcl_Obj *options);
#endif
#ifndef Tcl_GetReturnOptions_TCL_DECLARED
#define Tcl_GetReturnOptions_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_GetReturnOptions(Tcl_Interp *interp, int result);
#endif
#ifndef Tcl_IsEnsemble_TCL_DECLARED
#define Tcl_IsEnsemble_TCL_DECLARED

EXTERN int		Tcl_IsEnsemble(Tcl_Command token);
#endif
#ifndef Tcl_CreateEnsemble_TCL_DECLARED
#define Tcl_CreateEnsemble_TCL_DECLARED

EXTERN Tcl_Command	Tcl_CreateEnsemble(Tcl_Interp *interp,
				CONST char *name,
				Tcl_Namespace *namespacePtr, int flags);
#endif
#ifndef Tcl_FindEnsemble_TCL_DECLARED
#define Tcl_FindEnsemble_TCL_DECLARED

EXTERN Tcl_Command	Tcl_FindEnsemble(Tcl_Interp *interp,
				Tcl_Obj *cmdNameObj, int flags);
#endif
#ifndef Tcl_SetEnsembleSubcommandList_TCL_DECLARED
#define Tcl_SetEnsembleSubcommandList_TCL_DECLARED

EXTERN int		Tcl_SetEnsembleSubcommandList(Tcl_Interp *interp,
				Tcl_Command token, Tcl_Obj *subcmdList);
#endif
#ifndef Tcl_SetEnsembleMappingDict_TCL_DECLARED
#define Tcl_SetEnsembleMappingDict_TCL_DECLARED

EXTERN int		Tcl_SetEnsembleMappingDict(Tcl_Interp *interp,
				Tcl_Command token, Tcl_Obj *mapDict);
#endif
#ifndef Tcl_SetEnsembleUnknownHandler_TCL_DECLARED
#define Tcl_SetEnsembleUnknownHandler_TCL_DECLARED

EXTERN int		Tcl_SetEnsembleUnknownHandler(Tcl_Interp *interp,
				Tcl_Command token, Tcl_Obj *unknownList);
#endif
#ifndef Tcl_SetEnsembleFlags_TCL_DECLARED
#define Tcl_SetEnsembleFlags_TCL_DECLARED

EXTERN int		Tcl_SetEnsembleFlags(Tcl_Interp *interp,
				Tcl_Command token, int flags);
#endif
#ifndef Tcl_GetEnsembleSubcommandList_TCL_DECLARED
#define Tcl_GetEnsembleSubcommandList_TCL_DECLARED

EXTERN int		Tcl_GetEnsembleSubcommandList(Tcl_Interp *interp,
				Tcl_Command token, Tcl_Obj **subcmdListPtr);
#endif
#ifndef Tcl_GetEnsembleMappingDict_TCL_DECLARED
#define Tcl_GetEnsembleMappingDict_TCL_DECLARED

EXTERN int		Tcl_GetEnsembleMappingDict(Tcl_Interp *interp,
				Tcl_Command token, Tcl_Obj **mapDictPtr);
#endif
#ifndef Tcl_GetEnsembleUnknownHandler_TCL_DECLARED
#define Tcl_GetEnsembleUnknownHandler_TCL_DECLARED

EXTERN int		Tcl_GetEnsembleUnknownHandler(Tcl_Interp *interp,
				Tcl_Command token, Tcl_Obj **unknownListPtr);
#endif
#ifndef Tcl_GetEnsembleFlags_TCL_DECLARED
#define Tcl_GetEnsembleFlags_TCL_DECLARED

EXTERN int		Tcl_GetEnsembleFlags(Tcl_Interp *interp,
				Tcl_Command token, int *flagsPtr);
#endif
#ifndef Tcl_GetEnsembleNamespace_TCL_DECLARED
#define Tcl_GetEnsembleNamespace_TCL_DECLARED

EXTERN int		Tcl_GetEnsembleNamespace(Tcl_Interp *interp,
				Tcl_Command token,
				Tcl_Namespace **namespacePtrPtr);
#endif
#ifndef Tcl_SetTimeProc_TCL_DECLARED
#define Tcl_SetTimeProc_TCL_DECLARED

EXTERN void		Tcl_SetTimeProc(Tcl_GetTimeProc *getProc,
				Tcl_ScaleTimeProc *scaleProc,
				ClientData clientData);
#endif
#ifndef Tcl_QueryTimeProc_TCL_DECLARED
#define Tcl_QueryTimeProc_TCL_DECLARED

EXTERN void		Tcl_QueryTimeProc(Tcl_GetTimeProc **getProc,
				Tcl_ScaleTimeProc **scaleProc,
				ClientData *clientData);
#endif
#ifndef Tcl_ChannelThreadActionProc_TCL_DECLARED
#define Tcl_ChannelThreadActionProc_TCL_DECLARED

EXTERN Tcl_DriverThreadActionProc * Tcl_ChannelThreadActionProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_NewBignumObj_TCL_DECLARED
#define Tcl_NewBignumObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_NewBignumObj(mp_int *value);
#endif
#ifndef Tcl_DbNewBignumObj_TCL_DECLARED
#define Tcl_DbNewBignumObj_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_DbNewBignumObj(mp_int *value, CONST char *file,
				int line);
#endif
#ifndef Tcl_SetBignumObj_TCL_DECLARED
#define Tcl_SetBignumObj_TCL_DECLARED

EXTERN void		Tcl_SetBignumObj(Tcl_Obj *obj, mp_int *value);
#endif
#ifndef Tcl_GetBignumFromObj_TCL_DECLARED
#define Tcl_GetBignumFromObj_TCL_DECLARED

EXTERN int		Tcl_GetBignumFromObj(Tcl_Interp *interp,
				Tcl_Obj *obj, mp_int *value);
#endif
#ifndef Tcl_TakeBignumFromObj_TCL_DECLARED
#define Tcl_TakeBignumFromObj_TCL_DECLARED

EXTERN int		Tcl_TakeBignumFromObj(Tcl_Interp *interp,
				Tcl_Obj *obj, mp_int *value);
#endif
#ifndef Tcl_TruncateChannel_TCL_DECLARED
#define Tcl_TruncateChannel_TCL_DECLARED

EXTERN int		Tcl_TruncateChannel(Tcl_Channel chan,
				Tcl_WideInt length);
#endif
#ifndef Tcl_ChannelTruncateProc_TCL_DECLARED
#define Tcl_ChannelTruncateProc_TCL_DECLARED

EXTERN Tcl_DriverTruncateProc * Tcl_ChannelTruncateProc(
				CONST Tcl_ChannelType *chanTypePtr);
#endif
#ifndef Tcl_SetChannelErrorInterp_TCL_DECLARED
#define Tcl_SetChannelErrorInterp_TCL_DECLARED

EXTERN void		Tcl_SetChannelErrorInterp(Tcl_Interp *interp,
				Tcl_Obj *msg);
#endif
#ifndef Tcl_GetChannelErrorInterp_TCL_DECLARED
#define Tcl_GetChannelErrorInterp_TCL_DECLARED

EXTERN void		Tcl_GetChannelErrorInterp(Tcl_Interp *interp,
				Tcl_Obj **msg);
#endif
#ifndef Tcl_SetChannelError_TCL_DECLARED
#define Tcl_SetChannelError_TCL_DECLARED

EXTERN void		Tcl_SetChannelError(Tcl_Channel chan, Tcl_Obj *msg);
#endif
#ifndef Tcl_GetChannelError_TCL_DECLARED
#define Tcl_GetChannelError_TCL_DECLARED

EXTERN void		Tcl_GetChannelError(Tcl_Channel chan, Tcl_Obj **msg);
#endif
#ifndef Tcl_InitBignumFromDouble_TCL_DECLARED
#define Tcl_InitBignumFromDouble_TCL_DECLARED

EXTERN int		Tcl_InitBignumFromDouble(Tcl_Interp *interp,
				double initval, mp_int *toInit);
#endif
#ifndef Tcl_GetNamespaceUnknownHandler_TCL_DECLARED
#define Tcl_GetNamespaceUnknownHandler_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_GetNamespaceUnknownHandler(Tcl_Interp *interp,
				Tcl_Namespace *nsPtr);
#endif
#ifndef Tcl_SetNamespaceUnknownHandler_TCL_DECLARED
#define Tcl_SetNamespaceUnknownHandler_TCL_DECLARED

EXTERN int		Tcl_SetNamespaceUnknownHandler(Tcl_Interp *interp,
				Tcl_Namespace *nsPtr, Tcl_Obj *handlerPtr);
#endif
#ifndef Tcl_GetEncodingFromObj_TCL_DECLARED
#define Tcl_GetEncodingFromObj_TCL_DECLARED

EXTERN int		Tcl_GetEncodingFromObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, Tcl_Encoding *encodingPtr);
#endif
#ifndef Tcl_GetEncodingSearchPath_TCL_DECLARED
#define Tcl_GetEncodingSearchPath_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_GetEncodingSearchPath(void);
#endif
#ifndef Tcl_SetEncodingSearchPath_TCL_DECLARED
#define Tcl_SetEncodingSearchPath_TCL_DECLARED

EXTERN int		Tcl_SetEncodingSearchPath(Tcl_Obj *searchPath);
#endif
#ifndef Tcl_GetEncodingNameFromEnvironment_TCL_DECLARED
#define Tcl_GetEncodingNameFromEnvironment_TCL_DECLARED

EXTERN CONST char *	Tcl_GetEncodingNameFromEnvironment(
				Tcl_DString *bufPtr);
#endif
#ifndef Tcl_PkgRequireProc_TCL_DECLARED
#define Tcl_PkgRequireProc_TCL_DECLARED

EXTERN int		Tcl_PkgRequireProc(Tcl_Interp *interp,
				CONST char *name, int objc,
				Tcl_Obj *CONST objv[],
				ClientData *clientDataPtr);
#endif
#ifndef Tcl_AppendObjToErrorInfo_TCL_DECLARED
#define Tcl_AppendObjToErrorInfo_TCL_DECLARED

EXTERN void		Tcl_AppendObjToErrorInfo(Tcl_Interp *interp,
				Tcl_Obj *objPtr);
#endif
#ifndef Tcl_AppendLimitedToObj_TCL_DECLARED
#define Tcl_AppendLimitedToObj_TCL_DECLARED

EXTERN void		Tcl_AppendLimitedToObj(Tcl_Obj *objPtr,
				CONST char *bytes, int length, int limit,
				CONST char *ellipsis);
#endif
#ifndef Tcl_Format_TCL_DECLARED
#define Tcl_Format_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_Format(Tcl_Interp *interp, CONST char *format,
				int objc, Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_AppendFormatToObj_TCL_DECLARED
#define Tcl_AppendFormatToObj_TCL_DECLARED

EXTERN int		Tcl_AppendFormatToObj(Tcl_Interp *interp,
				Tcl_Obj *objPtr, CONST char *format,
				int objc, Tcl_Obj *CONST objv[]);
#endif
#ifndef Tcl_ObjPrintf_TCL_DECLARED
#define Tcl_ObjPrintf_TCL_DECLARED

EXTERN Tcl_Obj *	Tcl_ObjPrintf(CONST char *format, ...);
#endif
#ifndef Tcl_AppendPrintfToObj_TCL_DECLARED
#define Tcl_AppendPrintfToObj_TCL_DECLARED

EXTERN void		Tcl_AppendPrintfToObj(Tcl_Obj *objPtr,
				CONST char *format, ...);
#endif


















































#ifndef TclUnusedStubEntry_TCL_DECLARED
#define TclUnusedStubEntry_TCL_DECLARED

EXTERN void		TclUnusedStubEntry(void);
#endif





























#ifndef Tcl_outputCmdFrame_TCL_DECLARED
#define Tcl_outputCmdFrame_TCL_DECLARED

EXTERN int		Tcl_outputCmdFrame(Tcl_Interp *interp);
#endif



















#ifndef Tcl_GetSourceInfo_TCL_DECLARED
#define Tcl_GetSourceInfo_TCL_DECLARED

EXTERN int		Tcl_GetSourceInfo(Tcl_Interp *interp);
#endif

typedef struct TclStubHooks {
    struct TclPlatStubs *tclPlatStubs;
    struct TclIntStubs *tclIntStubs;
    struct TclIntPlatStubs *tclIntPlatStubs;
} TclStubHooks;

typedef struct TclStubs {
    int magic;
    struct TclStubHooks *hooks;

    int (*tcl_PkgProvideEx) (Tcl_Interp *interp, CONST char *name, CONST char *version, ClientData clientData); 
    CONST84_RETURN char * (*tcl_PkgRequireEx) (Tcl_Interp *interp, CONST char *name, CONST char *version, int exact, ClientData *clientDataPtr); 
    void (*tcl_Panic) (CONST char *format, ...); 
    char * (*tcl_Alloc) (unsigned int size); 
    void (*tcl_Free) (char *ptr); 
    char * (*tcl_Realloc) (char *ptr, unsigned int size); 
    char * (*tcl_DbCkalloc) (unsigned int size, CONST char *file, int line); 
    void (*tcl_DbCkfree) (char *ptr, CONST char *file, int line); 
    char * (*tcl_DbCkrealloc) (char *ptr, unsigned int size, CONST char *file, int line); 
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
    void (*tcl_CreateFileHandler) (int fd, int mask, Tcl_FileProc *proc, ClientData clientData); 
#endif 
#if defined(__WIN32__) 
    VOID *reserved9;
#endif 
#ifdef MAC_OSX_TCL 
    void (*tcl_CreateFileHandler) (int fd, int mask, Tcl_FileProc *proc, ClientData clientData); 
#endif 
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
    void (*tcl_DeleteFileHandler) (int fd); 
#endif 
#if defined(__WIN32__) 
    VOID *reserved10;
#endif 
#ifdef MAC_OSX_TCL 
    void (*tcl_DeleteFileHandler) (int fd); 
#endif 
    void (*tcl_SetTimer) (Tcl_Time *timePtr); 
    void (*tcl_Sleep) (int ms); 
    int (*tcl_WaitForEvent) (Tcl_Time *timePtr); 
    int (*tcl_AppendAllObjTypes) (Tcl_Interp *interp, Tcl_Obj *objPtr); 
    void (*tcl_AppendStringsToObj) (Tcl_Obj *objPtr, ...); 
    void (*tcl_AppendToObj) (Tcl_Obj *objPtr, CONST char *bytes, int length); 
    Tcl_Obj * (*tcl_ConcatObj) (int objc, Tcl_Obj *CONST objv[]); 
    int (*tcl_ConvertToType) (Tcl_Interp *interp, Tcl_Obj *objPtr, Tcl_ObjType *typePtr); 
    void (*tcl_DbDecrRefCount) (Tcl_Obj *objPtr, CONST char *file, int line); 
    void (*tcl_DbIncrRefCount) (Tcl_Obj *objPtr, CONST char *file, int line); 
    int (*tcl_DbIsShared) (Tcl_Obj *objPtr, CONST char *file, int line); 
    Tcl_Obj * (*tcl_DbNewBooleanObj) (int boolValue, CONST char *file, int line); 
    Tcl_Obj * (*tcl_DbNewByteArrayObj) (CONST unsigned char *bytes, int length, CONST char *file, int line); 
    Tcl_Obj * (*tcl_DbNewDoubleObj) (double doubleValue, CONST char *file, int line); 
    Tcl_Obj * (*tcl_DbNewListObj) (int objc, Tcl_Obj *CONST *objv, CONST char *file, int line); 
    Tcl_Obj * (*tcl_DbNewLongObj) (long longValue, CONST char *file, int line); 
    Tcl_Obj * (*tcl_DbNewObj) (CONST char *file, int line); 
    Tcl_Obj * (*tcl_DbNewStringObj) (CONST char *bytes, int length, CONST char *file, int line); 
    Tcl_Obj * (*tcl_DuplicateObj) (Tcl_Obj *objPtr); 
    void (*tclFreeObj) (Tcl_Obj *objPtr); 
    int (*tcl_GetBoolean) (Tcl_Interp *interp, CONST char *src, int *boolPtr); 
    int (*tcl_GetBooleanFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *boolPtr); 
    unsigned char * (*tcl_GetByteArrayFromObj) (Tcl_Obj *objPtr, int *lengthPtr); 
    int (*tcl_GetDouble) (Tcl_Interp *interp, CONST char *src, double *doublePtr); 
    int (*tcl_GetDoubleFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, double *doublePtr); 
    int (*tcl_GetIndexFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, CONST84 char **tablePtr, CONST char *msg, int flags, int *indexPtr); 
    int (*tcl_GetInt) (Tcl_Interp *interp, CONST char *src, int *intPtr); 
    int (*tcl_GetIntFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *intPtr); 
    int (*tcl_GetLongFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, long *longPtr); 
    Tcl_ObjType * (*tcl_GetObjType) (CONST char *typeName); 
    char * (*tcl_GetStringFromObj) (Tcl_Obj *objPtr, int *lengthPtr); 
    void (*tcl_InvalidateStringRep) (Tcl_Obj *objPtr); 
    int (*tcl_ListObjAppendList) (Tcl_Interp *interp, Tcl_Obj *listPtr, Tcl_Obj *elemListPtr); 
    int (*tcl_ListObjAppendElement) (Tcl_Interp *interp, Tcl_Obj *listPtr, Tcl_Obj *objPtr); 
    int (*tcl_ListObjGetElements) (Tcl_Interp *interp, Tcl_Obj *listPtr, int *objcPtr, Tcl_Obj ***objvPtr); 
    int (*tcl_ListObjIndex) (Tcl_Interp *interp, Tcl_Obj *listPtr, int index, Tcl_Obj **objPtrPtr); 
    int (*tcl_ListObjLength) (Tcl_Interp *interp, Tcl_Obj *listPtr, int *lengthPtr); 
    int (*tcl_ListObjReplace) (Tcl_Interp *interp, Tcl_Obj *listPtr, int first, int count, int objc, Tcl_Obj *CONST objv[]); 
    Tcl_Obj * (*tcl_NewBooleanObj) (int boolValue); 
    Tcl_Obj * (*tcl_NewByteArrayObj) (CONST unsigned char *bytes, int length); 
    Tcl_Obj * (*tcl_NewDoubleObj) (double doubleValue); 
    Tcl_Obj * (*tcl_NewIntObj) (int intValue); 
    Tcl_Obj * (*tcl_NewListObj) (int objc, Tcl_Obj *CONST objv[]); 
    Tcl_Obj * (*tcl_NewLongObj) (long longValue); 
    Tcl_Obj * (*tcl_NewObj) (void); 
    Tcl_Obj * (*tcl_NewStringObj) (CONST char *bytes, int length); 
    void (*tcl_SetBooleanObj) (Tcl_Obj *objPtr, int boolValue); 
    unsigned char * (*tcl_SetByteArrayLength) (Tcl_Obj *objPtr, int length); 
    void (*tcl_SetByteArrayObj) (Tcl_Obj *objPtr, CONST unsigned char *bytes, int length); 
    void (*tcl_SetDoubleObj) (Tcl_Obj *objPtr, double doubleValue); 
    void (*tcl_SetIntObj) (Tcl_Obj *objPtr, int intValue); 
    void (*tcl_SetListObj) (Tcl_Obj *objPtr, int objc, Tcl_Obj *CONST objv[]); 
    void (*tcl_SetLongObj) (Tcl_Obj *objPtr, long longValue); 
    void (*tcl_SetObjLength) (Tcl_Obj *objPtr, int length); 
    void (*tcl_SetStringObj) (Tcl_Obj *objPtr, CONST char *bytes, int length); 
    void (*tcl_AddErrorInfo) (Tcl_Interp *interp, CONST char *message); 
    void (*tcl_AddObjErrorInfo) (Tcl_Interp *interp, CONST char *message, int length); 
    void (*tcl_AllowExceptions) (Tcl_Interp *interp); 
    void (*tcl_AppendElement) (Tcl_Interp *interp, CONST char *element); 
    void (*tcl_AppendResult) (Tcl_Interp *interp, ...); 
    Tcl_AsyncHandler (*tcl_AsyncCreate) (Tcl_AsyncProc *proc, ClientData clientData); 
    void (*tcl_AsyncDelete) (Tcl_AsyncHandler async); 
    int (*tcl_AsyncInvoke) (Tcl_Interp *interp, int code); 
    void (*tcl_AsyncMark) (Tcl_AsyncHandler async); 
    int (*tcl_AsyncReady) (void); 
    void (*tcl_BackgroundError) (Tcl_Interp *interp); 
    char (*tcl_Backslash) (CONST char *src, int *readPtr); 
    int (*tcl_BadChannelOption) (Tcl_Interp *interp, CONST char *optionName, CONST char *optionList); 
    void (*tcl_CallWhenDeleted) (Tcl_Interp *interp, Tcl_InterpDeleteProc *proc, ClientData clientData); 
    void (*tcl_CancelIdleCall) (Tcl_IdleProc *idleProc, ClientData clientData); 
    int (*tcl_Close) (Tcl_Interp *interp, Tcl_Channel chan); 
    int (*tcl_CommandComplete) (CONST char *cmd); 
    char * (*tcl_Concat) (int argc, CONST84 char *CONST *argv); 
    int (*tcl_ConvertElement) (CONST char *src, char *dst, int flags); 
    int (*tcl_ConvertCountedElement) (CONST char *src, int length, char *dst, int flags); 
    int (*tcl_CreateAlias) (Tcl_Interp *slave, CONST char *slaveCmd, Tcl_Interp *target, CONST char *targetCmd, int argc, CONST84 char *CONST *argv); 
    int (*tcl_CreateAliasObj) (Tcl_Interp *slave, CONST char *slaveCmd, Tcl_Interp *target, CONST char *targetCmd, int objc, Tcl_Obj *CONST objv[]); 
    Tcl_Channel (*tcl_CreateChannel) (Tcl_ChannelType *typePtr, CONST char *chanName, ClientData instanceData, int mask); 
    void (*tcl_CreateChannelHandler) (Tcl_Channel chan, int mask, Tcl_ChannelProc *proc, ClientData clientData); 
    void (*tcl_CreateCloseHandler) (Tcl_Channel chan, Tcl_CloseProc *proc, ClientData clientData); 
    Tcl_Command (*tcl_CreateCommand) (Tcl_Interp *interp, CONST char *cmdName, Tcl_CmdProc *proc, ClientData clientData, Tcl_CmdDeleteProc *deleteProc); 
    void (*tcl_CreateEventSource) (Tcl_EventSetupProc *setupProc, Tcl_EventCheckProc *checkProc, ClientData clientData); 
    void (*tcl_CreateExitHandler) (Tcl_ExitProc *proc, ClientData clientData); 
    Tcl_Interp * (*tcl_CreateInterp) (void); 
    void (*tcl_CreateMathFunc) (Tcl_Interp *interp, CONST char *name, int numArgs, Tcl_ValueType *argTypes, Tcl_MathProc *proc, ClientData clientData); 
    Tcl_Command (*tcl_CreateObjCommand) (Tcl_Interp *interp, CONST char *cmdName, Tcl_ObjCmdProc *proc, ClientData clientData, Tcl_CmdDeleteProc *deleteProc); 
    Tcl_Interp * (*tcl_CreateSlave) (Tcl_Interp *interp, CONST char *slaveName, int isSafe); 
    Tcl_TimerToken (*tcl_CreateTimerHandler) (int milliseconds, Tcl_TimerProc *proc, ClientData clientData); 
    Tcl_Trace (*tcl_CreateTrace) (Tcl_Interp *interp, int level, Tcl_CmdTraceProc *proc, ClientData clientData); 
    void (*tcl_DeleteAssocData) (Tcl_Interp *interp, CONST char *name); 
    void (*tcl_DeleteChannelHandler) (Tcl_Channel chan, Tcl_ChannelProc *proc, ClientData clientData); 
    void (*tcl_DeleteCloseHandler) (Tcl_Channel chan, Tcl_CloseProc *proc, ClientData clientData); 
    int (*tcl_DeleteCommand) (Tcl_Interp *interp, CONST char *cmdName); 
    int (*tcl_DeleteCommandFromToken) (Tcl_Interp *interp, Tcl_Command command); 
    void (*tcl_DeleteEvents) (Tcl_EventDeleteProc *proc, ClientData clientData); 
    void (*tcl_DeleteEventSource) (Tcl_EventSetupProc *setupProc, Tcl_EventCheckProc *checkProc, ClientData clientData); 
    void (*tcl_DeleteExitHandler) (Tcl_ExitProc *proc, ClientData clientData); 
    void (*tcl_DeleteHashEntry) (Tcl_HashEntry *entryPtr); 
    void (*tcl_DeleteHashTable) (Tcl_HashTable *tablePtr); 
    void (*tcl_DeleteInterp) (Tcl_Interp *interp); 
    void (*tcl_DetachPids) (int numPids, Tcl_Pid *pidPtr); 
    void (*tcl_DeleteTimerHandler) (Tcl_TimerToken token); 
    void (*tcl_DeleteTrace) (Tcl_Interp *interp, Tcl_Trace trace); 
    void (*tcl_DontCallWhenDeleted) (Tcl_Interp *interp, Tcl_InterpDeleteProc *proc, ClientData clientData); 
    int (*tcl_DoOneEvent) (int flags); 
    void (*tcl_DoWhenIdle) (Tcl_IdleProc *proc, ClientData clientData); 
    char * (*tcl_DStringAppend) (Tcl_DString *dsPtr, CONST char *bytes, int length); 
    char * (*tcl_DStringAppendElement) (Tcl_DString *dsPtr, CONST char *element); 
    void (*tcl_DStringEndSublist) (Tcl_DString *dsPtr); 
    void (*tcl_DStringFree) (Tcl_DString *dsPtr); 
    void (*tcl_DStringGetResult) (Tcl_Interp *interp, Tcl_DString *dsPtr); 
    void (*tcl_DStringInit) (Tcl_DString *dsPtr); 
    void (*tcl_DStringResult) (Tcl_Interp *interp, Tcl_DString *dsPtr); 
    void (*tcl_DStringSetLength) (Tcl_DString *dsPtr, int length); 
    void (*tcl_DStringStartSublist) (Tcl_DString *dsPtr); 
    int (*tcl_Eof) (Tcl_Channel chan); 
    CONST84_RETURN char * (*tcl_ErrnoId) (void); 
    CONST84_RETURN char * (*tcl_ErrnoMsg) (int err); 
    int (*tcl_Eval) (Tcl_Interp *interp, CONST char *script); 
    int (*tcl_EvalFile) (Tcl_Interp *interp, CONST char *fileName); 
    int (*tcl_EvalObj) (Tcl_Interp *interp, Tcl_Obj *objPtr); 
    void (*tcl_EventuallyFree) (ClientData clientData, Tcl_FreeProc *freeProc); 
    void (*tcl_Exit) (int status); 
    int (*tcl_ExposeCommand) (Tcl_Interp *interp, CONST char *hiddenCmdToken, CONST char *cmdName); 
    int (*tcl_ExprBoolean) (Tcl_Interp *interp, CONST char *expr, int *ptr); 
    int (*tcl_ExprBooleanObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int *ptr); 
    int (*tcl_ExprDouble) (Tcl_Interp *interp, CONST char *expr, double *ptr); 
    int (*tcl_ExprDoubleObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, double *ptr); 
    int (*tcl_ExprLong) (Tcl_Interp *interp, CONST char *expr, long *ptr); 
    int (*tcl_ExprLongObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, long *ptr); 
    int (*tcl_ExprObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, Tcl_Obj **resultPtrPtr); 
    int (*tcl_ExprString) (Tcl_Interp *interp, CONST char *expr); 
    void (*tcl_Finalize) (void); 
    void (*tcl_FindExecutable) (CONST char *argv0); 
    Tcl_HashEntry * (*tcl_FirstHashEntry) (Tcl_HashTable *tablePtr, Tcl_HashSearch *searchPtr); 
    int (*tcl_Flush) (Tcl_Channel chan); 
    void (*tcl_FreeResult) (Tcl_Interp *interp); 
    int (*tcl_GetAlias) (Tcl_Interp *interp, CONST char *slaveCmd, Tcl_Interp **targetInterpPtr, CONST84 char **targetCmdPtr, int *argcPtr, CONST84 char ***argvPtr); 
    int (*tcl_GetAliasObj) (Tcl_Interp *interp, CONST char *slaveCmd, Tcl_Interp **targetInterpPtr, CONST84 char **targetCmdPtr, int *objcPtr, Tcl_Obj ***objv); 
    ClientData (*tcl_GetAssocData) (Tcl_Interp *interp, CONST char *name, Tcl_InterpDeleteProc **procPtr); 
    Tcl_Channel (*tcl_GetChannel) (Tcl_Interp *interp, CONST char *chanName, int *modePtr); 
    int (*tcl_GetChannelBufferSize) (Tcl_Channel chan); 
    int (*tcl_GetChannelHandle) (Tcl_Channel chan, int direction, ClientData *handlePtr); 
    ClientData (*tcl_GetChannelInstanceData) (Tcl_Channel chan); 
    int (*tcl_GetChannelMode) (Tcl_Channel chan); 
    CONST84_RETURN char * (*tcl_GetChannelName) (Tcl_Channel chan); 
    int (*tcl_GetChannelOption) (Tcl_Interp *interp, Tcl_Channel chan, CONST char *optionName, Tcl_DString *dsPtr); 
    Tcl_ChannelType * (*tcl_GetChannelType) (Tcl_Channel chan); 
    int (*tcl_GetCommandInfo) (Tcl_Interp *interp, CONST char *cmdName, Tcl_CmdInfo *infoPtr); 
    CONST84_RETURN char * (*tcl_GetCommandName) (Tcl_Interp *interp, Tcl_Command command); 
    int (*tcl_GetErrno) (void); 
    CONST84_RETURN char * (*tcl_GetHostName) (void); 
    int (*tcl_GetInterpPath) (Tcl_Interp *askInterp, Tcl_Interp *slaveInterp); 
    Tcl_Interp * (*tcl_GetMaster) (Tcl_Interp *interp); 
    CONST char * (*tcl_GetNameOfExecutable) (void); 
    Tcl_Obj * (*tcl_GetObjResult) (Tcl_Interp *interp); 
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
    int (*tcl_GetOpenFile) (Tcl_Interp *interp, CONST char *chanID, int forWriting, int checkUsage, ClientData *filePtr); 
#endif 
#if defined(__WIN32__) 
    VOID *reserved167;
#endif 
#ifdef MAC_OSX_TCL 
    int (*tcl_GetOpenFile) (Tcl_Interp *interp, CONST char *chanID, int forWriting, int checkUsage, ClientData *filePtr); 
#endif 
    Tcl_PathType (*tcl_GetPathType) (CONST char *path); 
    int (*tcl_Gets) (Tcl_Channel chan, Tcl_DString *dsPtr); 
    int (*tcl_GetsObj) (Tcl_Channel chan, Tcl_Obj *objPtr); 
    int (*tcl_GetServiceMode) (void); 
    Tcl_Interp * (*tcl_GetSlave) (Tcl_Interp *interp, CONST char *slaveName); 
    Tcl_Channel (*tcl_GetStdChannel) (int type); 
    CONST84_RETURN char * (*tcl_GetStringResult) (Tcl_Interp *interp); 
    CONST84_RETURN char * (*tcl_GetVar) (Tcl_Interp *interp, CONST char *varName, int flags); 
    CONST84_RETURN char * (*tcl_GetVar2) (Tcl_Interp *interp, CONST char *part1, CONST char *part2, int flags); 
    int (*tcl_GlobalEval) (Tcl_Interp *interp, CONST char *command); 
    int (*tcl_GlobalEvalObj) (Tcl_Interp *interp, Tcl_Obj *objPtr); 
    int (*tcl_HideCommand) (Tcl_Interp *interp, CONST char *cmdName, CONST char *hiddenCmdToken); 
    int (*tcl_Init) (Tcl_Interp *interp); 
    void (*tcl_InitHashTable) (Tcl_HashTable *tablePtr, int keyType); 
    int (*tcl_InputBlocked) (Tcl_Channel chan); 
    int (*tcl_InputBuffered) (Tcl_Channel chan); 
    int (*tcl_InterpDeleted) (Tcl_Interp *interp); 
    int (*tcl_IsSafe) (Tcl_Interp *interp); 
    char * (*tcl_JoinPath) (int argc, CONST84 char *CONST *argv, Tcl_DString *resultPtr); 
    int (*tcl_LinkVar) (Tcl_Interp *interp, CONST char *varName, char *addr, int type); 
    VOID *reserved188;
    Tcl_Channel (*tcl_MakeFileChannel) (ClientData handle, int mode); 
    int (*tcl_MakeSafe) (Tcl_Interp *interp); 
    Tcl_Channel (*tcl_MakeTcpClientChannel) (ClientData tcpSocket); 
    char * (*tcl_Merge) (int argc, CONST84 char *CONST *argv); 
    Tcl_HashEntry * (*tcl_NextHashEntry) (Tcl_HashSearch *searchPtr); 
    void (*tcl_NotifyChannel) (Tcl_Channel channel, int mask); 
    Tcl_Obj * (*tcl_ObjGetVar2) (Tcl_Interp *interp, Tcl_Obj *part1Ptr, Tcl_Obj *part2Ptr, int flags); 
    Tcl_Obj * (*tcl_ObjSetVar2) (Tcl_Interp *interp, Tcl_Obj *part1Ptr, Tcl_Obj *part2Ptr, Tcl_Obj *newValuePtr, int flags); 
    Tcl_Channel (*tcl_OpenCommandChannel) (Tcl_Interp *interp, int argc, CONST84 char **argv, int flags); 
    Tcl_Channel (*tcl_OpenFileChannel) (Tcl_Interp *interp, CONST char *fileName, CONST char *modeString, int permissions); 
    Tcl_Channel (*tcl_OpenTcpClient) (Tcl_Interp *interp, int port, CONST char *address, CONST char *myaddr, int myport, int async); 
    Tcl_Channel (*tcl_OpenTcpServer) (Tcl_Interp *interp, int port, CONST char *host, Tcl_TcpAcceptProc *acceptProc, ClientData callbackData); 
    void (*tcl_Preserve) (ClientData data); 
    void (*tcl_PrintDouble) (Tcl_Interp *interp, double value, char *dst); 
    int (*tcl_PutEnv) (CONST char *assignment); 
    CONST84_RETURN char * (*tcl_PosixError) (Tcl_Interp *interp); 
    void (*tcl_QueueEvent) (Tcl_Event *evPtr, Tcl_QueuePosition position); 
    int (*tcl_Read) (Tcl_Channel chan, char *bufPtr, int toRead); 
    void (*tcl_ReapDetachedProcs) (void); 
    int (*tcl_RecordAndEval) (Tcl_Interp *interp, CONST char *cmd, int flags); 
    int (*tcl_RecordAndEvalObj) (Tcl_Interp *interp, Tcl_Obj *cmdPtr, int flags); 
    void (*tcl_RegisterChannel) (Tcl_Interp *interp, Tcl_Channel chan); 
    void (*tcl_RegisterObjType) (Tcl_ObjType *typePtr); 
    Tcl_RegExp (*tcl_RegExpCompile) (Tcl_Interp *interp, CONST char *pattern); 
    int (*tcl_RegExpExec) (Tcl_Interp *interp, Tcl_RegExp regexp, CONST char *text, CONST char *start); 
    int (*tcl_RegExpMatch) (Tcl_Interp *interp, CONST char *text, CONST char *pattern); 
    void (*tcl_RegExpRange) (Tcl_RegExp regexp, int index, CONST84 char **startPtr, CONST84 char **endPtr); 
    void (*tcl_Release) (ClientData clientData); 
    void (*tcl_ResetResult) (Tcl_Interp *interp); 
    int (*tcl_ScanElement) (CONST char *src, int *flagPtr); 
    int (*tcl_ScanCountedElement) (CONST char *src, int length, int *flagPtr); 
    int (*tcl_SeekOld) (Tcl_Channel chan, int offset, int mode); 
    int (*tcl_ServiceAll) (void); 
    int (*tcl_ServiceEvent) (int flags); 
    void (*tcl_SetAssocData) (Tcl_Interp *interp, CONST char *name, Tcl_InterpDeleteProc *proc, ClientData clientData); 
    void (*tcl_SetChannelBufferSize) (Tcl_Channel chan, int sz); 
    int (*tcl_SetChannelOption) (Tcl_Interp *interp, Tcl_Channel chan, CONST char *optionName, CONST char *newValue); 
    int (*tcl_SetCommandInfo) (Tcl_Interp *interp, CONST char *cmdName, CONST Tcl_CmdInfo *infoPtr); 
    void (*tcl_SetErrno) (int err); 
    void (*tcl_SetErrorCode) (Tcl_Interp *interp, ...); 
    void (*tcl_SetMaxBlockTime) (Tcl_Time *timePtr); 
    void (*tcl_SetPanicProc) (Tcl_PanicProc *panicProc); 
    int (*tcl_SetRecursionLimit) (Tcl_Interp *interp, int depth); 
    void (*tcl_SetResult) (Tcl_Interp *interp, char *result, Tcl_FreeProc *freeProc); 
    int (*tcl_SetServiceMode) (int mode); 
    void (*tcl_SetObjErrorCode) (Tcl_Interp *interp, Tcl_Obj *errorObjPtr); 
    void (*tcl_SetObjResult) (Tcl_Interp *interp, Tcl_Obj *resultObjPtr); 
    void (*tcl_SetStdChannel) (Tcl_Channel channel, int type); 
    CONST84_RETURN char * (*tcl_SetVar) (Tcl_Interp *interp, CONST char *varName, CONST char *newValue, int flags); 
    CONST84_RETURN char * (*tcl_SetVar2) (Tcl_Interp *interp, CONST char *part1, CONST char *part2, CONST char *newValue, int flags); 
    CONST84_RETURN char * (*tcl_SignalId) (int sig); 
    CONST84_RETURN char * (*tcl_SignalMsg) (int sig); 
    void (*tcl_SourceRCFile) (Tcl_Interp *interp); 
    int (*tcl_SplitList) (Tcl_Interp *interp, CONST char *listStr, int *argcPtr, CONST84 char ***argvPtr); 
    void (*tcl_SplitPath) (CONST char *path, int *argcPtr, CONST84 char ***argvPtr); 
    void (*tcl_StaticPackage) (Tcl_Interp *interp, CONST char *pkgName, Tcl_PackageInitProc *initProc, Tcl_PackageInitProc *safeInitProc); 
    int (*tcl_StringMatch) (CONST char *str, CONST char *pattern); 
    int (*tcl_TellOld) (Tcl_Channel chan); 
    int (*tcl_TraceVar) (Tcl_Interp *interp, CONST char *varName, int flags, Tcl_VarTraceProc *proc, ClientData clientData); 
    int (*tcl_TraceVar2) (Tcl_Interp *interp, CONST char *part1, CONST char *part2, int flags, Tcl_VarTraceProc *proc, ClientData clientData); 
    char * (*tcl_TranslateFileName) (Tcl_Interp *interp, CONST char *name, Tcl_DString *bufferPtr); 
    int (*tcl_Ungets) (Tcl_Channel chan, CONST char *str, int len, int atHead); 
    void (*tcl_UnlinkVar) (Tcl_Interp *interp, CONST char *varName); 
    int (*tcl_UnregisterChannel) (Tcl_Interp *interp, Tcl_Channel chan); 
    int (*tcl_UnsetVar) (Tcl_Interp *interp, CONST char *varName, int flags); 
    int (*tcl_UnsetVar2) (Tcl_Interp *interp, CONST char *part1, CONST char *part2, int flags); 
    void (*tcl_UntraceVar) (Tcl_Interp *interp, CONST char *varName, int flags, Tcl_VarTraceProc *proc, ClientData clientData); 
    void (*tcl_UntraceVar2) (Tcl_Interp *interp, CONST char *part1, CONST char *part2, int flags, Tcl_VarTraceProc *proc, ClientData clientData); 
    void (*tcl_UpdateLinkedVar) (Tcl_Interp *interp, CONST char *varName); 
    int (*tcl_UpVar) (Tcl_Interp *interp, CONST char *frameName, CONST char *varName, CONST char *localName, int flags); 
    int (*tcl_UpVar2) (Tcl_Interp *interp, CONST char *frameName, CONST char *part1, CONST char *part2, CONST char *localName, int flags); 
    int (*tcl_VarEval) (Tcl_Interp *interp, ...); 
    ClientData (*tcl_VarTraceInfo) (Tcl_Interp *interp, CONST char *varName, int flags, Tcl_VarTraceProc *procPtr, ClientData prevClientData); 
    ClientData (*tcl_VarTraceInfo2) (Tcl_Interp *interp, CONST char *part1, CONST char *part2, int flags, Tcl_VarTraceProc *procPtr, ClientData prevClientData); 
    int (*tcl_Write) (Tcl_Channel chan, CONST char *s, int slen); 
    void (*tcl_WrongNumArgs) (Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], CONST char *message); 
    int (*tcl_DumpActiveMemory) (CONST char *fileName); 
    void (*tcl_ValidateAllMemory) (CONST char *file, int line); 
    void (*tcl_AppendResultVA) (Tcl_Interp *interp, va_list argList); 
    void (*tcl_AppendStringsToObjVA) (Tcl_Obj *objPtr, va_list argList); 
    char * (*tcl_HashStats) (Tcl_HashTable *tablePtr); 
    CONST84_RETURN char * (*tcl_ParseVar) (Tcl_Interp *interp, CONST char *start, CONST84 char **termPtr); 
    CONST84_RETURN char * (*tcl_PkgPresent) (Tcl_Interp *interp, CONST char *name, CONST char *version, int exact); 
    CONST84_RETURN char * (*tcl_PkgPresentEx) (Tcl_Interp *interp, CONST char *name, CONST char *version, int exact, ClientData *clientDataPtr); 
    int (*tcl_PkgProvide) (Tcl_Interp *interp, CONST char *name, CONST char *version); 
    CONST84_RETURN char * (*tcl_PkgRequire) (Tcl_Interp *interp, CONST char *name, CONST char *version, int exact); 
    void (*tcl_SetErrorCodeVA) (Tcl_Interp *interp, va_list argList); 
    int (*tcl_VarEvalVA) (Tcl_Interp *interp, va_list argList); 
    Tcl_Pid (*tcl_WaitPid) (Tcl_Pid pid, int *statPtr, int options); 
    void (*tcl_PanicVA) (CONST char *format, va_list argList); 
    void (*tcl_GetVersion) (int *major, int *minor, int *patchLevel, int *type); 
    void (*tcl_InitMemory) (Tcl_Interp *interp); 
    Tcl_Channel (*tcl_StackChannel) (Tcl_Interp *interp, Tcl_ChannelType *typePtr, ClientData instanceData, int mask, Tcl_Channel prevChan); 
    int (*tcl_UnstackChannel) (Tcl_Interp *interp, Tcl_Channel chan); 
    Tcl_Channel (*tcl_GetStackedChannel) (Tcl_Channel chan); 
    void (*tcl_SetMainLoop) (Tcl_MainLoopProc *proc); 
    VOID *reserved285;
    void (*tcl_AppendObjToObj) (Tcl_Obj *objPtr, Tcl_Obj *appendObjPtr); 
    Tcl_Encoding (*tcl_CreateEncoding) (CONST Tcl_EncodingType *typePtr); 
    void (*tcl_CreateThreadExitHandler) (Tcl_ExitProc *proc, ClientData clientData); 
    void (*tcl_DeleteThreadExitHandler) (Tcl_ExitProc *proc, ClientData clientData); 
    void (*tcl_DiscardResult) (Tcl_SavedResult *statePtr); 
    int (*tcl_EvalEx) (Tcl_Interp *interp, CONST char *script, int numBytes, int flags); 
    int (*tcl_EvalObjv) (Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[], int flags); 
    int (*tcl_EvalObjEx) (Tcl_Interp *interp, Tcl_Obj *objPtr, int flags); 
    void (*tcl_ExitThread) (int status); 
    int (*tcl_ExternalToUtf) (Tcl_Interp *interp, Tcl_Encoding encoding, CONST char *src, int srcLen, int flags, Tcl_EncodingState *statePtr, char *dst, int dstLen, int *srcReadPtr, int *dstWrotePtr, int *dstCharsPtr); 
    char * (*tcl_ExternalToUtfDString) (Tcl_Encoding encoding, CONST char *src, int srcLen, Tcl_DString *dsPtr); 
    void (*tcl_FinalizeThread) (void); 
    void (*tcl_FinalizeNotifier) (ClientData clientData); 
    void (*tcl_FreeEncoding) (Tcl_Encoding encoding); 
    Tcl_ThreadId (*tcl_GetCurrentThread) (void); 
    Tcl_Encoding (*tcl_GetEncoding) (Tcl_Interp *interp, CONST char *name); 
    CONST84_RETURN char * (*tcl_GetEncodingName) (Tcl_Encoding encoding); 
    void (*tcl_GetEncodingNames) (Tcl_Interp *interp); 
    int (*tcl_GetIndexFromObjStruct) (Tcl_Interp *interp, Tcl_Obj *objPtr, CONST VOID *tablePtr, int offset, CONST char *msg, int flags, int *indexPtr); 
    VOID * (*tcl_GetThreadData) (Tcl_ThreadDataKey *keyPtr, int size); 
    Tcl_Obj * (*tcl_GetVar2Ex) (Tcl_Interp *interp, CONST char *part1, CONST char *part2, int flags); 
    ClientData (*tcl_InitNotifier) (void); 
    void (*tcl_MutexLock) (Tcl_Mutex *mutexPtr); 
    void (*tcl_MutexUnlock) (Tcl_Mutex *mutexPtr); 
    void (*tcl_ConditionNotify) (Tcl_Condition *condPtr); 
    void (*tcl_ConditionWait) (Tcl_Condition *condPtr, Tcl_Mutex *mutexPtr, Tcl_Time *timePtr); 
    int (*tcl_NumUtfChars) (CONST char *src, int length); 
    int (*tcl_ReadChars) (Tcl_Channel channel, Tcl_Obj *objPtr, int charsToRead, int appendFlag); 
    void (*tcl_RestoreResult) (Tcl_Interp *interp, Tcl_SavedResult *statePtr); 
    void (*tcl_SaveResult) (Tcl_Interp *interp, Tcl_SavedResult *statePtr); 
    int (*tcl_SetSystemEncoding) (Tcl_Interp *interp, CONST char *name); 
    Tcl_Obj * (*tcl_SetVar2Ex) (Tcl_Interp *interp, CONST char *part1, CONST char *part2, Tcl_Obj *newValuePtr, int flags); 
    void (*tcl_ThreadAlert) (Tcl_ThreadId threadId); 
    void (*tcl_ThreadQueueEvent) (Tcl_ThreadId threadId, Tcl_Event *evPtr, Tcl_QueuePosition position); 
    Tcl_UniChar (*tcl_UniCharAtIndex) (CONST char *src, int index); 
    Tcl_UniChar (*tcl_UniCharToLower) (int ch); 
    Tcl_UniChar (*tcl_UniCharToTitle) (int ch); 
    Tcl_UniChar (*tcl_UniCharToUpper) (int ch); 
    int (*tcl_UniCharToUtf) (int ch, char *buf); 
    CONST84_RETURN char * (*tcl_UtfAtIndex) (CONST char *src, int index); 
    int (*tcl_UtfCharComplete) (CONST char *src, int length); 
    int (*tcl_UtfBackslash) (CONST char *src, int *readPtr, char *dst); 
    CONST84_RETURN char * (*tcl_UtfFindFirst) (CONST char *src, int ch); 
    CONST84_RETURN char * (*tcl_UtfFindLast) (CONST char *src, int ch); 
    CONST84_RETURN char * (*tcl_UtfNext) (CONST char *src); 
    CONST84_RETURN char * (*tcl_UtfPrev) (CONST char *src, CONST char *start); 
    int (*tcl_UtfToExternal) (Tcl_Interp *interp, Tcl_Encoding encoding, CONST char *src, int srcLen, int flags, Tcl_EncodingState *statePtr, char *dst, int dstLen, int *srcReadPtr, int *dstWrotePtr, int *dstCharsPtr); 
    char * (*tcl_UtfToExternalDString) (Tcl_Encoding encoding, CONST char *src, int srcLen, Tcl_DString *dsPtr); 
    int (*tcl_UtfToLower) (char *src); 
    int (*tcl_UtfToTitle) (char *src); 
    int (*tcl_UtfToUniChar) (CONST char *src, Tcl_UniChar *chPtr); 
    int (*tcl_UtfToUpper) (char *src); 
    int (*tcl_WriteChars) (Tcl_Channel chan, CONST char *src, int srcLen); 
    int (*tcl_WriteObj) (Tcl_Channel chan, Tcl_Obj *objPtr); 
    char * (*tcl_GetString) (Tcl_Obj *objPtr); 
    CONST84_RETURN char * (*tcl_GetDefaultEncodingDir) (void); 
    void (*tcl_SetDefaultEncodingDir) (CONST char *path); 
    void (*tcl_AlertNotifier) (ClientData clientData); 
    void (*tcl_ServiceModeHook) (int mode); 
    int (*tcl_UniCharIsAlnum) (int ch); 
    int (*tcl_UniCharIsAlpha) (int ch); 
    int (*tcl_UniCharIsDigit) (int ch); 
    int (*tcl_UniCharIsLower) (int ch); 
    int (*tcl_UniCharIsSpace) (int ch); 
    int (*tcl_UniCharIsUpper) (int ch); 
    int (*tcl_UniCharIsWordChar) (int ch); 
    int (*tcl_UniCharLen) (CONST Tcl_UniChar *uniStr); 
    int (*tcl_UniCharNcmp) (CONST Tcl_UniChar *ucs, CONST Tcl_UniChar *uct, unsigned long numChars); 
    char * (*tcl_UniCharToUtfDString) (CONST Tcl_UniChar *uniStr, int uniLength, Tcl_DString *dsPtr); 
    Tcl_UniChar * (*tcl_UtfToUniCharDString) (CONST char *src, int length, Tcl_DString *dsPtr); 
    Tcl_RegExp (*tcl_GetRegExpFromObj) (Tcl_Interp *interp, Tcl_Obj *patObj, int flags); 
    Tcl_Obj * (*tcl_EvalTokens) (Tcl_Interp *interp, Tcl_Token *tokenPtr, int count); 
    void (*tcl_FreeParse) (Tcl_Parse *parsePtr); 
    void (*tcl_LogCommandInfo) (Tcl_Interp *interp, CONST char *script, CONST char *command, int length); 
    int (*tcl_ParseBraces) (Tcl_Interp *interp, CONST char *start, int numBytes, Tcl_Parse *parsePtr, int append, CONST84 char **termPtr); 
    int (*tcl_ParseCommand) (Tcl_Interp *interp, CONST char *start, int numBytes, int nested, Tcl_Parse *parsePtr); 
    int (*tcl_ParseExpr) (Tcl_Interp *interp, CONST char *start, int numBytes, Tcl_Parse *parsePtr); 
    int (*tcl_ParseQuotedString) (Tcl_Interp *interp, CONST char *start, int numBytes, Tcl_Parse *parsePtr, int append, CONST84 char **termPtr); 
    int (*tcl_ParseVarName) (Tcl_Interp *interp, CONST char *start, int numBytes, Tcl_Parse *parsePtr, int append); 
    char * (*tcl_GetCwd) (Tcl_Interp *interp, Tcl_DString *cwdPtr); 
    int (*tcl_Chdir) (CONST char *dirName); 
    int (*tcl_Access) (CONST char *path, int mode); 
    int (*tcl_Stat) (CONST char *path, struct stat *bufPtr); 
    int (*tcl_UtfNcmp) (CONST char *s1, CONST char *s2, unsigned long n); 
    int (*tcl_UtfNcasecmp) (CONST char *s1, CONST char *s2, unsigned long n); 
    int (*tcl_StringCaseMatch) (CONST char *str, CONST char *pattern, int nocase); 
    int (*tcl_UniCharIsControl) (int ch); 
    int (*tcl_UniCharIsGraph) (int ch); 
    int (*tcl_UniCharIsPrint) (int ch); 
    int (*tcl_UniCharIsPunct) (int ch); 
    int (*tcl_RegExpExecObj) (Tcl_Interp *interp, Tcl_RegExp regexp, Tcl_Obj *textObj, int offset, int nmatches, int flags); 
    void (*tcl_RegExpGetInfo) (Tcl_RegExp regexp, Tcl_RegExpInfo *infoPtr); 
    Tcl_Obj * (*tcl_NewUnicodeObj) (CONST Tcl_UniChar *unicode, int numChars); 
    void (*tcl_SetUnicodeObj) (Tcl_Obj *objPtr, CONST Tcl_UniChar *unicode, int numChars); 
    int (*tcl_GetCharLength) (Tcl_Obj *objPtr); 
    Tcl_UniChar (*tcl_GetUniChar) (Tcl_Obj *objPtr, int index); 
    Tcl_UniChar * (*tcl_GetUnicode) (Tcl_Obj *objPtr); 
    Tcl_Obj * (*tcl_GetRange) (Tcl_Obj *objPtr, int first, int last); 
    void (*tcl_AppendUnicodeToObj) (Tcl_Obj *objPtr, CONST Tcl_UniChar *unicode, int length); 
    int (*tcl_RegExpMatchObj) (Tcl_Interp *interp, Tcl_Obj *textObj, Tcl_Obj *patternObj); 
    void (*tcl_SetNotifier) (Tcl_NotifierProcs *notifierProcPtr); 
    Tcl_Mutex * (*tcl_GetAllocMutex) (void); 
    int (*tcl_GetChannelNames) (Tcl_Interp *interp); 
    int (*tcl_GetChannelNamesEx) (Tcl_Interp *interp, CONST char *pattern); 
    int (*tcl_ProcObjCmd) (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]); 
    void (*tcl_ConditionFinalize) (Tcl_Condition *condPtr); 
    void (*tcl_MutexFinalize) (Tcl_Mutex *mutex); 
    int (*tcl_CreateThread) (Tcl_ThreadId *idPtr, Tcl_ThreadCreateProc proc, ClientData clientData, int stackSize, int flags); 
    int (*tcl_ReadRaw) (Tcl_Channel chan, char *dst, int bytesToRead); 
    int (*tcl_WriteRaw) (Tcl_Channel chan, CONST char *src, int srcLen); 
    Tcl_Channel (*tcl_GetTopChannel) (Tcl_Channel chan); 
    int (*tcl_ChannelBuffered) (Tcl_Channel chan); 
    CONST84_RETURN char * (*tcl_ChannelName) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_ChannelTypeVersion (*tcl_ChannelVersion) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverBlockModeProc * (*tcl_ChannelBlockModeProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverCloseProc * (*tcl_ChannelCloseProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverClose2Proc * (*tcl_ChannelClose2Proc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverInputProc * (*tcl_ChannelInputProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverOutputProc * (*tcl_ChannelOutputProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverSeekProc * (*tcl_ChannelSeekProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverSetOptionProc * (*tcl_ChannelSetOptionProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverGetOptionProc * (*tcl_ChannelGetOptionProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverWatchProc * (*tcl_ChannelWatchProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverGetHandleProc * (*tcl_ChannelGetHandleProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverFlushProc * (*tcl_ChannelFlushProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_DriverHandlerProc * (*tcl_ChannelHandlerProc) (CONST Tcl_ChannelType *chanTypePtr); 
    int (*tcl_JoinThread) (Tcl_ThreadId threadId, int *result); 
    int (*tcl_IsChannelShared) (Tcl_Channel channel); 
    int (*tcl_IsChannelRegistered) (Tcl_Interp *interp, Tcl_Channel channel); 
    void (*tcl_CutChannel) (Tcl_Channel channel); 
    void (*tcl_SpliceChannel) (Tcl_Channel channel); 
    void (*tcl_ClearChannelHandlers) (Tcl_Channel channel); 
    int (*tcl_IsChannelExisting) (CONST char *channelName); 
    int (*tcl_UniCharNcasecmp) (CONST Tcl_UniChar *ucs, CONST Tcl_UniChar *uct, unsigned long numChars); 
    int (*tcl_UniCharCaseMatch) (CONST Tcl_UniChar *uniStr, CONST Tcl_UniChar *uniPattern, int nocase); 
    Tcl_HashEntry * (*tcl_FindHashEntry) (Tcl_HashTable *tablePtr, CONST char *key); 
    Tcl_HashEntry * (*tcl_CreateHashEntry) (Tcl_HashTable *tablePtr, CONST char *key, int *newPtr); 
    void (*tcl_InitCustomHashTable) (Tcl_HashTable *tablePtr, int keyType, Tcl_HashKeyType *typePtr); 
    void (*tcl_InitObjHashTable) (Tcl_HashTable *tablePtr); 
    ClientData (*tcl_CommandTraceInfo) (Tcl_Interp *interp, CONST char *varName, int flags, Tcl_CommandTraceProc *procPtr, ClientData prevClientData); 
    int (*tcl_TraceCommand) (Tcl_Interp *interp, CONST char *varName, int flags, Tcl_CommandTraceProc *proc, ClientData clientData); 
    void (*tcl_UntraceCommand) (Tcl_Interp *interp, CONST char *varName, int flags, Tcl_CommandTraceProc *proc, ClientData clientData); 
    char * (*tcl_AttemptAlloc) (unsigned int size); 
    char * (*tcl_AttemptDbCkalloc) (unsigned int size, CONST char *file, int line); 
    char * (*tcl_AttemptRealloc) (char *ptr, unsigned int size); 
    char * (*tcl_AttemptDbCkrealloc) (char *ptr, unsigned int size, CONST char *file, int line); 
    int (*tcl_AttemptSetObjLength) (Tcl_Obj *objPtr, int length); 
    Tcl_ThreadId (*tcl_GetChannelThread) (Tcl_Channel channel); 
    Tcl_UniChar * (*tcl_GetUnicodeFromObj) (Tcl_Obj *objPtr, int *lengthPtr); 
    int (*tcl_GetMathFuncInfo) (Tcl_Interp *interp, CONST char *name, int *numArgsPtr, Tcl_ValueType **argTypesPtr, Tcl_MathProc **procPtr, ClientData *clientDataPtr); 
    Tcl_Obj * (*tcl_ListMathFuncs) (Tcl_Interp *interp, CONST char *pattern); 
    Tcl_Obj * (*tcl_SubstObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, int flags); 
    int (*tcl_DetachChannel) (Tcl_Interp *interp, Tcl_Channel channel); 
    int (*tcl_IsStandardChannel) (Tcl_Channel channel); 
    int (*tcl_FSCopyFile) (Tcl_Obj *srcPathPtr, Tcl_Obj *destPathPtr); 
    int (*tcl_FSCopyDirectory) (Tcl_Obj *srcPathPtr, Tcl_Obj *destPathPtr, Tcl_Obj **errorPtr); 
    int (*tcl_FSCreateDirectory) (Tcl_Obj *pathPtr); 
    int (*tcl_FSDeleteFile) (Tcl_Obj *pathPtr); 
    int (*tcl_FSLoadFile) (Tcl_Interp *interp, Tcl_Obj *pathPtr, CONST char *sym1, CONST char *sym2, Tcl_PackageInitProc **proc1Ptr, Tcl_PackageInitProc **proc2Ptr, Tcl_LoadHandle *handlePtr, Tcl_FSUnloadFileProc **unloadProcPtr); 
    int (*tcl_FSMatchInDirectory) (Tcl_Interp *interp, Tcl_Obj *result, Tcl_Obj *pathPtr, CONST char *pattern, Tcl_GlobTypeData *types); 
    Tcl_Obj * (*tcl_FSLink) (Tcl_Obj *pathPtr, Tcl_Obj *toPtr, int linkAction); 
    int (*tcl_FSRemoveDirectory) (Tcl_Obj *pathPtr, int recursive, Tcl_Obj **errorPtr); 
    int (*tcl_FSRenameFile) (Tcl_Obj *srcPathPtr, Tcl_Obj *destPathPtr); 
    int (*tcl_FSLstat) (Tcl_Obj *pathPtr, Tcl_StatBuf *buf); 
    int (*tcl_FSUtime) (Tcl_Obj *pathPtr, struct utimbuf *tval); 
    int (*tcl_FSFileAttrsGet) (Tcl_Interp *interp, int index, Tcl_Obj *pathPtr, Tcl_Obj **objPtrRef); 
    int (*tcl_FSFileAttrsSet) (Tcl_Interp *interp, int index, Tcl_Obj *pathPtr, Tcl_Obj *objPtr); 
    CONST char ** (*tcl_FSFileAttrStrings) (Tcl_Obj *pathPtr, Tcl_Obj **objPtrRef); 
    int (*tcl_FSStat) (Tcl_Obj *pathPtr, Tcl_StatBuf *buf); 
    int (*tcl_FSAccess) (Tcl_Obj *pathPtr, int mode); 
    Tcl_Channel (*tcl_FSOpenFileChannel) (Tcl_Interp *interp, Tcl_Obj *pathPtr, CONST char *modeString, int permissions); 
    Tcl_Obj * (*tcl_FSGetCwd) (Tcl_Interp *interp); 
    int (*tcl_FSChdir) (Tcl_Obj *pathPtr); 
    int (*tcl_FSConvertToPathType) (Tcl_Interp *interp, Tcl_Obj *pathPtr); 
    Tcl_Obj * (*tcl_FSJoinPath) (Tcl_Obj *listObj, int elements); 
    Tcl_Obj * (*tcl_FSSplitPath) (Tcl_Obj *pathPtr, int *lenPtr); 
    int (*tcl_FSEqualPaths) (Tcl_Obj *firstPtr, Tcl_Obj *secondPtr); 
    Tcl_Obj * (*tcl_FSGetNormalizedPath) (Tcl_Interp *interp, Tcl_Obj *pathPtr); 
    Tcl_Obj * (*tcl_FSJoinToPath) (Tcl_Obj *pathPtr, int objc, Tcl_Obj *CONST objv[]); 
    ClientData (*tcl_FSGetInternalRep) (Tcl_Obj *pathPtr, Tcl_Filesystem *fsPtr); 
    Tcl_Obj * (*tcl_FSGetTranslatedPath) (Tcl_Interp *interp, Tcl_Obj *pathPtr); 
    int (*tcl_FSEvalFile) (Tcl_Interp *interp, Tcl_Obj *fileName); 
    Tcl_Obj * (*tcl_FSNewNativePath) (Tcl_Filesystem *fromFilesystem, ClientData clientData); 
    CONST char * (*tcl_FSGetNativePath) (Tcl_Obj *pathPtr); 
    Tcl_Obj * (*tcl_FSFileSystemInfo) (Tcl_Obj *pathPtr); 
    Tcl_Obj * (*tcl_FSPathSeparator) (Tcl_Obj *pathPtr); 
    Tcl_Obj * (*tcl_FSListVolumes) (void); 
    int (*tcl_FSRegister) (ClientData clientData, Tcl_Filesystem *fsPtr); 
    int (*tcl_FSUnregister) (Tcl_Filesystem *fsPtr); 
    ClientData (*tcl_FSData) (Tcl_Filesystem *fsPtr); 
    CONST char * (*tcl_FSGetTranslatedStringPath) (Tcl_Interp *interp, Tcl_Obj *pathPtr); 
    Tcl_Filesystem * (*tcl_FSGetFileSystemForPath) (Tcl_Obj *pathPtr); 
    Tcl_PathType (*tcl_FSGetPathType) (Tcl_Obj *pathPtr); 
    int (*tcl_OutputBuffered) (Tcl_Channel chan); 
    void (*tcl_FSMountsChanged) (Tcl_Filesystem *fsPtr); 
    int (*tcl_EvalTokensStandard) (Tcl_Interp *interp, Tcl_Token *tokenPtr, int count); 
    void (*tcl_GetTime) (Tcl_Time *timeBuf); 
    Tcl_Trace (*tcl_CreateObjTrace) (Tcl_Interp *interp, int level, int flags, Tcl_CmdObjTraceProc *objProc, ClientData clientData, Tcl_CmdObjTraceDeleteProc *delProc); 
    int (*tcl_GetCommandInfoFromToken) (Tcl_Command token, Tcl_CmdInfo *infoPtr); 
    int (*tcl_SetCommandInfoFromToken) (Tcl_Command token, CONST Tcl_CmdInfo *infoPtr); 
    Tcl_Obj * (*tcl_DbNewWideIntObj) (Tcl_WideInt wideValue, CONST char *file, int line); 
    int (*tcl_GetWideIntFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, Tcl_WideInt *widePtr); 
    Tcl_Obj * (*tcl_NewWideIntObj) (Tcl_WideInt wideValue); 
    void (*tcl_SetWideIntObj) (Tcl_Obj *objPtr, Tcl_WideInt wideValue); 
    Tcl_StatBuf * (*tcl_AllocStatBuf) (void); 
    Tcl_WideInt (*tcl_Seek) (Tcl_Channel chan, Tcl_WideInt offset, int mode); 
    Tcl_WideInt (*tcl_Tell) (Tcl_Channel chan); 
    Tcl_DriverWideSeekProc * (*tcl_ChannelWideSeekProc) (CONST Tcl_ChannelType *chanTypePtr); 
    int (*tcl_DictObjPut) (Tcl_Interp *interp, Tcl_Obj *dictPtr, Tcl_Obj *keyPtr, Tcl_Obj *valuePtr); 
    int (*tcl_DictObjGet) (Tcl_Interp *interp, Tcl_Obj *dictPtr, Tcl_Obj *keyPtr, Tcl_Obj **valuePtrPtr); 
    int (*tcl_DictObjRemove) (Tcl_Interp *interp, Tcl_Obj *dictPtr, Tcl_Obj *keyPtr); 
    int (*tcl_DictObjSize) (Tcl_Interp *interp, Tcl_Obj *dictPtr, int *sizePtr); 
    int (*tcl_DictObjFirst) (Tcl_Interp *interp, Tcl_Obj *dictPtr, Tcl_DictSearch *searchPtr, Tcl_Obj **keyPtrPtr, Tcl_Obj **valuePtrPtr, int *donePtr); 
    void (*tcl_DictObjNext) (Tcl_DictSearch *searchPtr, Tcl_Obj **keyPtrPtr, Tcl_Obj **valuePtrPtr, int *donePtr); 
    void (*tcl_DictObjDone) (Tcl_DictSearch *searchPtr); 
    int (*tcl_DictObjPutKeyList) (Tcl_Interp *interp, Tcl_Obj *dictPtr, int keyc, Tcl_Obj *CONST *keyv, Tcl_Obj *valuePtr); 
    int (*tcl_DictObjRemoveKeyList) (Tcl_Interp *interp, Tcl_Obj *dictPtr, int keyc, Tcl_Obj *CONST *keyv); 
    Tcl_Obj * (*tcl_NewDictObj) (void); 
    Tcl_Obj * (*tcl_DbNewDictObj) (CONST char *file, int line); 
    void (*tcl_RegisterConfig) (Tcl_Interp *interp, CONST char *pkgName, Tcl_Config *configuration, CONST char *valEncoding); 
    Tcl_Namespace * (*tcl_CreateNamespace) (Tcl_Interp *interp, CONST char *name, ClientData clientData, Tcl_NamespaceDeleteProc *deleteProc); 
    void (*tcl_DeleteNamespace) (Tcl_Namespace *nsPtr); 
    int (*tcl_AppendExportList) (Tcl_Interp *interp, Tcl_Namespace *nsPtr, Tcl_Obj *objPtr); 
    int (*tcl_Export) (Tcl_Interp *interp, Tcl_Namespace *nsPtr, CONST char *pattern, int resetListFirst); 
    int (*tcl_Import) (Tcl_Interp *interp, Tcl_Namespace *nsPtr, CONST char *pattern, int allowOverwrite); 
    int (*tcl_ForgetImport) (Tcl_Interp *interp, Tcl_Namespace *nsPtr, CONST char *pattern); 
    Tcl_Namespace * (*tcl_GetCurrentNamespace) (Tcl_Interp *interp); 
    Tcl_Namespace * (*tcl_GetGlobalNamespace) (Tcl_Interp *interp); 
    Tcl_Namespace * (*tcl_FindNamespace) (Tcl_Interp *interp, CONST char *name, Tcl_Namespace *contextNsPtr, int flags); 
    Tcl_Command (*tcl_FindCommand) (Tcl_Interp *interp, CONST char *name, Tcl_Namespace *contextNsPtr, int flags); 
    Tcl_Command (*tcl_GetCommandFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr); 
    void (*tcl_GetCommandFullName) (Tcl_Interp *interp, Tcl_Command command, Tcl_Obj *objPtr); 
    int (*tcl_FSEvalFileEx) (Tcl_Interp *interp, Tcl_Obj *fileName, CONST char *encodingName); 
    Tcl_ExitProc * (*tcl_SetExitProc) (Tcl_ExitProc *proc); 
    void (*tcl_LimitAddHandler) (Tcl_Interp *interp, int type, Tcl_LimitHandlerProc *handlerProc, ClientData clientData, Tcl_LimitHandlerDeleteProc *deleteProc); 
    void (*tcl_LimitRemoveHandler) (Tcl_Interp *interp, int type, Tcl_LimitHandlerProc *handlerProc, ClientData clientData); 
    int (*tcl_LimitReady) (Tcl_Interp *interp); 
    int (*tcl_LimitCheck) (Tcl_Interp *interp); 
    int (*tcl_LimitExceeded) (Tcl_Interp *interp); 
    void (*tcl_LimitSetCommands) (Tcl_Interp *interp, int commandLimit); 
    void (*tcl_LimitSetTime) (Tcl_Interp *interp, Tcl_Time *timeLimitPtr); 
    void (*tcl_LimitSetGranularity) (Tcl_Interp *interp, int type, int granularity); 
    int (*tcl_LimitTypeEnabled) (Tcl_Interp *interp, int type); 
    int (*tcl_LimitTypeExceeded) (Tcl_Interp *interp, int type); 
    void (*tcl_LimitTypeSet) (Tcl_Interp *interp, int type); 
    void (*tcl_LimitTypeReset) (Tcl_Interp *interp, int type); 
    int (*tcl_LimitGetCommands) (Tcl_Interp *interp); 
    void (*tcl_LimitGetTime) (Tcl_Interp *interp, Tcl_Time *timeLimitPtr); 
    int (*tcl_LimitGetGranularity) (Tcl_Interp *interp, int type); 
    Tcl_InterpState (*tcl_SaveInterpState) (Tcl_Interp *interp, int status); 
    int (*tcl_RestoreInterpState) (Tcl_Interp *interp, Tcl_InterpState state); 
    void (*tcl_DiscardInterpState) (Tcl_InterpState state); 
    int (*tcl_SetReturnOptions) (Tcl_Interp *interp, Tcl_Obj *options); 
    Tcl_Obj * (*tcl_GetReturnOptions) (Tcl_Interp *interp, int result); 
    int (*tcl_IsEnsemble) (Tcl_Command token); 
    Tcl_Command (*tcl_CreateEnsemble) (Tcl_Interp *interp, CONST char *name, Tcl_Namespace *namespacePtr, int flags); 
    Tcl_Command (*tcl_FindEnsemble) (Tcl_Interp *interp, Tcl_Obj *cmdNameObj, int flags); 
    int (*tcl_SetEnsembleSubcommandList) (Tcl_Interp *interp, Tcl_Command token, Tcl_Obj *subcmdList); 
    int (*tcl_SetEnsembleMappingDict) (Tcl_Interp *interp, Tcl_Command token, Tcl_Obj *mapDict); 
    int (*tcl_SetEnsembleUnknownHandler) (Tcl_Interp *interp, Tcl_Command token, Tcl_Obj *unknownList); 
    int (*tcl_SetEnsembleFlags) (Tcl_Interp *interp, Tcl_Command token, int flags); 
    int (*tcl_GetEnsembleSubcommandList) (Tcl_Interp *interp, Tcl_Command token, Tcl_Obj **subcmdListPtr); 
    int (*tcl_GetEnsembleMappingDict) (Tcl_Interp *interp, Tcl_Command token, Tcl_Obj **mapDictPtr); 
    int (*tcl_GetEnsembleUnknownHandler) (Tcl_Interp *interp, Tcl_Command token, Tcl_Obj **unknownListPtr); 
    int (*tcl_GetEnsembleFlags) (Tcl_Interp *interp, Tcl_Command token, int *flagsPtr); 
    int (*tcl_GetEnsembleNamespace) (Tcl_Interp *interp, Tcl_Command token, Tcl_Namespace **namespacePtrPtr); 
    void (*tcl_SetTimeProc) (Tcl_GetTimeProc *getProc, Tcl_ScaleTimeProc *scaleProc, ClientData clientData); 
    void (*tcl_QueryTimeProc) (Tcl_GetTimeProc **getProc, Tcl_ScaleTimeProc **scaleProc, ClientData *clientData); 
    Tcl_DriverThreadActionProc * (*tcl_ChannelThreadActionProc) (CONST Tcl_ChannelType *chanTypePtr); 
    Tcl_Obj * (*tcl_NewBignumObj) (mp_int *value); 
    Tcl_Obj * (*tcl_DbNewBignumObj) (mp_int *value, CONST char *file, int line); 
    void (*tcl_SetBignumObj) (Tcl_Obj *obj, mp_int *value); 
    int (*tcl_GetBignumFromObj) (Tcl_Interp *interp, Tcl_Obj *obj, mp_int *value); 
    int (*tcl_TakeBignumFromObj) (Tcl_Interp *interp, Tcl_Obj *obj, mp_int *value); 
    int (*tcl_TruncateChannel) (Tcl_Channel chan, Tcl_WideInt length); 
    Tcl_DriverTruncateProc * (*tcl_ChannelTruncateProc) (CONST Tcl_ChannelType *chanTypePtr); 
    void (*tcl_SetChannelErrorInterp) (Tcl_Interp *interp, Tcl_Obj *msg); 
    void (*tcl_GetChannelErrorInterp) (Tcl_Interp *interp, Tcl_Obj **msg); 
    void (*tcl_SetChannelError) (Tcl_Channel chan, Tcl_Obj *msg); 
    void (*tcl_GetChannelError) (Tcl_Channel chan, Tcl_Obj **msg); 
    int (*tcl_InitBignumFromDouble) (Tcl_Interp *interp, double initval, mp_int *toInit); 
    Tcl_Obj * (*tcl_GetNamespaceUnknownHandler) (Tcl_Interp *interp, Tcl_Namespace *nsPtr); 
    int (*tcl_SetNamespaceUnknownHandler) (Tcl_Interp *interp, Tcl_Namespace *nsPtr, Tcl_Obj *handlerPtr); 
    int (*tcl_GetEncodingFromObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, Tcl_Encoding *encodingPtr); 
    Tcl_Obj * (*tcl_GetEncodingSearchPath) (void); 
    int (*tcl_SetEncodingSearchPath) (Tcl_Obj *searchPath); 
    CONST char * (*tcl_GetEncodingNameFromEnvironment) (Tcl_DString *bufPtr); 
    int (*tcl_PkgRequireProc) (Tcl_Interp *interp, CONST char *name, int objc, Tcl_Obj *CONST objv[], ClientData *clientDataPtr); 
    void (*tcl_AppendObjToErrorInfo) (Tcl_Interp *interp, Tcl_Obj *objPtr); 
    void (*tcl_AppendLimitedToObj) (Tcl_Obj *objPtr, CONST char *bytes, int length, int limit, CONST char *ellipsis); 
    Tcl_Obj * (*tcl_Format) (Tcl_Interp *interp, CONST char *format, int objc, Tcl_Obj *CONST objv[]); 
    int (*tcl_AppendFormatToObj) (Tcl_Interp *interp, Tcl_Obj *objPtr, CONST char *format, int objc, Tcl_Obj *CONST objv[]); 
    Tcl_Obj * (*tcl_ObjPrintf) (CONST char *format, ...); 
    void (*tcl_AppendPrintfToObj) (Tcl_Obj *objPtr, CONST char *format, ...); 
    VOID *reserved580;
    VOID *reserved581;
    VOID *reserved582;
    VOID *reserved583;
    VOID *reserved584;
    VOID *reserved585;
    VOID *reserved586;
    VOID *reserved587;
    VOID *reserved588;
    VOID *reserved589;
    VOID *reserved590;
    VOID *reserved591;
    VOID *reserved592;
    VOID *reserved593;
    VOID *reserved594;
    VOID *reserved595;
    VOID *reserved596;
    VOID *reserved597;
    VOID *reserved598;
    VOID *reserved599;
    VOID *reserved600;
    VOID *reserved601;
    VOID *reserved602;
    VOID *reserved603;
    VOID *reserved604;
    VOID *reserved605;
    VOID *reserved606;
    VOID *reserved607;
    VOID *reserved608;
    VOID *reserved609;
    VOID *reserved610;
    VOID *reserved611;
    VOID *reserved612;
    VOID *reserved613;
    VOID *reserved614;
    VOID *reserved615;
    VOID *reserved616;
    VOID *reserved617;
    VOID *reserved618;
    VOID *reserved619;
    VOID *reserved620;
    VOID *reserved621;
    VOID *reserved622;
    VOID *reserved623;
    VOID *reserved624;
    VOID *reserved625;
    VOID *reserved626;
    VOID *reserved627;
    VOID *reserved628;
    VOID *reserved629;
    void (*tclUnusedStubEntry) (void); 
    VOID *reserved631;
    VOID *reserved632;
    VOID *reserved633;
    VOID *reserved634;
    VOID *reserved635;
    VOID *reserved636;
    VOID *reserved637;
    VOID *reserved638;
    VOID *reserved639;
    VOID *reserved640;
    VOID *reserved641;
    VOID *reserved642;
    VOID *reserved643;
    VOID *reserved644;
    VOID *reserved645;
    VOID *reserved646;
    VOID *reserved647;
    VOID *reserved648;
    VOID *reserved649;
    VOID *reserved650;
    VOID *reserved651;
    VOID *reserved652;
    VOID *reserved653;
    VOID *reserved654;
    VOID *reserved655;
    VOID *reserved656;
    VOID *reserved657;
    VOID *reserved658;
    VOID *reserved659;
    int (*tcl_outputCmdFrame) (Tcl_Interp *interp); 
    VOID *reserved661;
    VOID *reserved662;
    VOID *reserved663;
    VOID *reserved664;
    VOID *reserved665;
    VOID *reserved666;
    VOID *reserved667;
    VOID *reserved668;
    VOID *reserved669;
    VOID *reserved670;
    VOID *reserved671;
    VOID *reserved672;
    VOID *reserved673;
    VOID *reserved674;
    VOID *reserved675;
    VOID *reserved676;
    VOID *reserved677;
    VOID *reserved678;
    VOID *reserved679;
    int (*tcl_GetSourceInfo) (Tcl_Interp *interp); 
} TclStubs;

extern TclStubs *tclStubsPtr;

#ifdef __cplusplus
}
#endif

#if defined(USE_TCL_STUBS) && !defined(USE_TCL_STUB_PROCS)



#ifndef Tcl_PkgProvideEx
#define Tcl_PkgProvideEx \
	(tclStubsPtr->tcl_PkgProvideEx) 
#endif
#ifndef Tcl_PkgRequireEx
#define Tcl_PkgRequireEx \
	(tclStubsPtr->tcl_PkgRequireEx) 
#endif
#ifndef Tcl_Panic
#define Tcl_Panic \
	(tclStubsPtr->tcl_Panic) 
#endif
#ifndef Tcl_Alloc
#define Tcl_Alloc \
	(tclStubsPtr->tcl_Alloc) 
#endif
#ifndef Tcl_Free
#define Tcl_Free \
	(tclStubsPtr->tcl_Free) 
#endif
#ifndef Tcl_Realloc
#define Tcl_Realloc \
	(tclStubsPtr->tcl_Realloc) 
#endif
#ifndef Tcl_DbCkalloc
#define Tcl_DbCkalloc \
	(tclStubsPtr->tcl_DbCkalloc) 
#endif
#ifndef Tcl_DbCkfree
#define Tcl_DbCkfree \
	(tclStubsPtr->tcl_DbCkfree) 
#endif
#ifndef Tcl_DbCkrealloc
#define Tcl_DbCkrealloc \
	(tclStubsPtr->tcl_DbCkrealloc) 
#endif
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
#ifndef Tcl_CreateFileHandler
#define Tcl_CreateFileHandler \
	(tclStubsPtr->tcl_CreateFileHandler) 
#endif
#endif 
#ifdef MAC_OSX_TCL 
#ifndef Tcl_CreateFileHandler
#define Tcl_CreateFileHandler \
	(tclStubsPtr->tcl_CreateFileHandler) 
#endif
#endif 
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
#ifndef Tcl_DeleteFileHandler
#define Tcl_DeleteFileHandler \
	(tclStubsPtr->tcl_DeleteFileHandler) 
#endif
#endif 
#ifdef MAC_OSX_TCL 
#ifndef Tcl_DeleteFileHandler
#define Tcl_DeleteFileHandler \
	(tclStubsPtr->tcl_DeleteFileHandler) 
#endif
#endif 
#ifndef Tcl_SetTimer
#define Tcl_SetTimer \
	(tclStubsPtr->tcl_SetTimer) 
#endif
#ifndef Tcl_Sleep
#define Tcl_Sleep \
	(tclStubsPtr->tcl_Sleep) 
#endif
#ifndef Tcl_WaitForEvent
#define Tcl_WaitForEvent \
	(tclStubsPtr->tcl_WaitForEvent) 
#endif
#ifndef Tcl_AppendAllObjTypes
#define Tcl_AppendAllObjTypes \
	(tclStubsPtr->tcl_AppendAllObjTypes) 
#endif
#ifndef Tcl_AppendStringsToObj
#define Tcl_AppendStringsToObj \
	(tclStubsPtr->tcl_AppendStringsToObj) 
#endif
#ifndef Tcl_AppendToObj
#define Tcl_AppendToObj \
	(tclStubsPtr->tcl_AppendToObj) 
#endif
#ifndef Tcl_ConcatObj
#define Tcl_ConcatObj \
	(tclStubsPtr->tcl_ConcatObj) 
#endif
#ifndef Tcl_ConvertToType
#define Tcl_ConvertToType \
	(tclStubsPtr->tcl_ConvertToType) 
#endif
#ifndef Tcl_DbDecrRefCount
#define Tcl_DbDecrRefCount \
	(tclStubsPtr->tcl_DbDecrRefCount) 
#endif
#ifndef Tcl_DbIncrRefCount
#define Tcl_DbIncrRefCount \
	(tclStubsPtr->tcl_DbIncrRefCount) 
#endif
#ifndef Tcl_DbIsShared
#define Tcl_DbIsShared \
	(tclStubsPtr->tcl_DbIsShared) 
#endif
#ifndef Tcl_DbNewBooleanObj
#define Tcl_DbNewBooleanObj \
	(tclStubsPtr->tcl_DbNewBooleanObj) 
#endif
#ifndef Tcl_DbNewByteArrayObj
#define Tcl_DbNewByteArrayObj \
	(tclStubsPtr->tcl_DbNewByteArrayObj) 
#endif
#ifndef Tcl_DbNewDoubleObj
#define Tcl_DbNewDoubleObj \
	(tclStubsPtr->tcl_DbNewDoubleObj) 
#endif
#ifndef Tcl_DbNewListObj
#define Tcl_DbNewListObj \
	(tclStubsPtr->tcl_DbNewListObj) 
#endif
#ifndef Tcl_DbNewLongObj
#define Tcl_DbNewLongObj \
	(tclStubsPtr->tcl_DbNewLongObj) 
#endif
#ifndef Tcl_DbNewObj
#define Tcl_DbNewObj \
	(tclStubsPtr->tcl_DbNewObj) 
#endif
#ifndef Tcl_DbNewStringObj
#define Tcl_DbNewStringObj \
	(tclStubsPtr->tcl_DbNewStringObj) 
#endif
#ifndef Tcl_DuplicateObj
#define Tcl_DuplicateObj \
	(tclStubsPtr->tcl_DuplicateObj) 
#endif
#ifndef TclFreeObj
#define TclFreeObj \
	(tclStubsPtr->tclFreeObj) 
#endif
#ifndef Tcl_GetBoolean
#define Tcl_GetBoolean \
	(tclStubsPtr->tcl_GetBoolean) 
#endif
#ifndef Tcl_GetBooleanFromObj
#define Tcl_GetBooleanFromObj \
	(tclStubsPtr->tcl_GetBooleanFromObj) 
#endif
#ifndef Tcl_GetByteArrayFromObj
#define Tcl_GetByteArrayFromObj \
	(tclStubsPtr->tcl_GetByteArrayFromObj) 
#endif
#ifndef Tcl_GetDouble
#define Tcl_GetDouble \
	(tclStubsPtr->tcl_GetDouble) 
#endif
#ifndef Tcl_GetDoubleFromObj
#define Tcl_GetDoubleFromObj \
	(tclStubsPtr->tcl_GetDoubleFromObj) 
#endif
#ifndef Tcl_GetIndexFromObj
#define Tcl_GetIndexFromObj \
	(tclStubsPtr->tcl_GetIndexFromObj) 
#endif
#ifndef Tcl_GetInt
#define Tcl_GetInt \
	(tclStubsPtr->tcl_GetInt) 
#endif
#ifndef Tcl_GetIntFromObj
#define Tcl_GetIntFromObj \
	(tclStubsPtr->tcl_GetIntFromObj) 
#endif
#ifndef Tcl_GetLongFromObj
#define Tcl_GetLongFromObj \
	(tclStubsPtr->tcl_GetLongFromObj) 
#endif
#ifndef Tcl_GetObjType
#define Tcl_GetObjType \
	(tclStubsPtr->tcl_GetObjType) 
#endif
#ifndef Tcl_GetStringFromObj
#define Tcl_GetStringFromObj \
	(tclStubsPtr->tcl_GetStringFromObj) 
#endif
#ifndef Tcl_InvalidateStringRep
#define Tcl_InvalidateStringRep \
	(tclStubsPtr->tcl_InvalidateStringRep) 
#endif
#ifndef Tcl_ListObjAppendList
#define Tcl_ListObjAppendList \
	(tclStubsPtr->tcl_ListObjAppendList) 
#endif
#ifndef Tcl_ListObjAppendElement
#define Tcl_ListObjAppendElement \
	(tclStubsPtr->tcl_ListObjAppendElement) 
#endif
#ifndef Tcl_ListObjGetElements
#define Tcl_ListObjGetElements \
	(tclStubsPtr->tcl_ListObjGetElements) 
#endif
#ifndef Tcl_ListObjIndex
#define Tcl_ListObjIndex \
	(tclStubsPtr->tcl_ListObjIndex) 
#endif
#ifndef Tcl_ListObjLength
#define Tcl_ListObjLength \
	(tclStubsPtr->tcl_ListObjLength) 
#endif
#ifndef Tcl_ListObjReplace
#define Tcl_ListObjReplace \
	(tclStubsPtr->tcl_ListObjReplace) 
#endif
#ifndef Tcl_NewBooleanObj
#define Tcl_NewBooleanObj \
	(tclStubsPtr->tcl_NewBooleanObj) 
#endif
#ifndef Tcl_NewByteArrayObj
#define Tcl_NewByteArrayObj \
	(tclStubsPtr->tcl_NewByteArrayObj) 
#endif
#ifndef Tcl_NewDoubleObj
#define Tcl_NewDoubleObj \
	(tclStubsPtr->tcl_NewDoubleObj) 
#endif
#ifndef Tcl_NewIntObj
#define Tcl_NewIntObj \
	(tclStubsPtr->tcl_NewIntObj) 
#endif
#ifndef Tcl_NewListObj
#define Tcl_NewListObj \
	(tclStubsPtr->tcl_NewListObj) 
#endif
#ifndef Tcl_NewLongObj
#define Tcl_NewLongObj \
	(tclStubsPtr->tcl_NewLongObj) 
#endif
#ifndef Tcl_NewObj
#define Tcl_NewObj \
	(tclStubsPtr->tcl_NewObj) 
#endif
#ifndef Tcl_NewStringObj
#define Tcl_NewStringObj \
	(tclStubsPtr->tcl_NewStringObj) 
#endif
#ifndef Tcl_SetBooleanObj
#define Tcl_SetBooleanObj \
	(tclStubsPtr->tcl_SetBooleanObj) 
#endif
#ifndef Tcl_SetByteArrayLength
#define Tcl_SetByteArrayLength \
	(tclStubsPtr->tcl_SetByteArrayLength) 
#endif
#ifndef Tcl_SetByteArrayObj
#define Tcl_SetByteArrayObj \
	(tclStubsPtr->tcl_SetByteArrayObj) 
#endif
#ifndef Tcl_SetDoubleObj
#define Tcl_SetDoubleObj \
	(tclStubsPtr->tcl_SetDoubleObj) 
#endif
#ifndef Tcl_SetIntObj
#define Tcl_SetIntObj \
	(tclStubsPtr->tcl_SetIntObj) 
#endif
#ifndef Tcl_SetListObj
#define Tcl_SetListObj \
	(tclStubsPtr->tcl_SetListObj) 
#endif
#ifndef Tcl_SetLongObj
#define Tcl_SetLongObj \
	(tclStubsPtr->tcl_SetLongObj) 
#endif
#ifndef Tcl_SetObjLength
#define Tcl_SetObjLength \
	(tclStubsPtr->tcl_SetObjLength) 
#endif
#ifndef Tcl_SetStringObj
#define Tcl_SetStringObj \
	(tclStubsPtr->tcl_SetStringObj) 
#endif
#ifndef Tcl_AddErrorInfo
#define Tcl_AddErrorInfo \
	(tclStubsPtr->tcl_AddErrorInfo) 
#endif
#ifndef Tcl_AddObjErrorInfo
#define Tcl_AddObjErrorInfo \
	(tclStubsPtr->tcl_AddObjErrorInfo) 
#endif
#ifndef Tcl_AllowExceptions
#define Tcl_AllowExceptions \
	(tclStubsPtr->tcl_AllowExceptions) 
#endif
#ifndef Tcl_AppendElement
#define Tcl_AppendElement \
	(tclStubsPtr->tcl_AppendElement) 
#endif
#ifndef Tcl_AppendResult
#define Tcl_AppendResult \
	(tclStubsPtr->tcl_AppendResult) 
#endif
#ifndef Tcl_AsyncCreate
#define Tcl_AsyncCreate \
	(tclStubsPtr->tcl_AsyncCreate) 
#endif
#ifndef Tcl_AsyncDelete
#define Tcl_AsyncDelete \
	(tclStubsPtr->tcl_AsyncDelete) 
#endif
#ifndef Tcl_AsyncInvoke
#define Tcl_AsyncInvoke \
	(tclStubsPtr->tcl_AsyncInvoke) 
#endif
#ifndef Tcl_AsyncMark
#define Tcl_AsyncMark \
	(tclStubsPtr->tcl_AsyncMark) 
#endif
#ifndef Tcl_AsyncReady
#define Tcl_AsyncReady \
	(tclStubsPtr->tcl_AsyncReady) 
#endif
#ifndef Tcl_BackgroundError
#define Tcl_BackgroundError \
	(tclStubsPtr->tcl_BackgroundError) 
#endif
#ifndef Tcl_Backslash
#define Tcl_Backslash \
	(tclStubsPtr->tcl_Backslash) 
#endif
#ifndef Tcl_BadChannelOption
#define Tcl_BadChannelOption \
	(tclStubsPtr->tcl_BadChannelOption) 
#endif
#ifndef Tcl_CallWhenDeleted
#define Tcl_CallWhenDeleted \
	(tclStubsPtr->tcl_CallWhenDeleted) 
#endif
#ifndef Tcl_CancelIdleCall
#define Tcl_CancelIdleCall \
	(tclStubsPtr->tcl_CancelIdleCall) 
#endif
#ifndef Tcl_Close
#define Tcl_Close \
	(tclStubsPtr->tcl_Close) 
#endif
#ifndef Tcl_CommandComplete
#define Tcl_CommandComplete \
	(tclStubsPtr->tcl_CommandComplete) 
#endif
#ifndef Tcl_Concat
#define Tcl_Concat \
	(tclStubsPtr->tcl_Concat) 
#endif
#ifndef Tcl_ConvertElement
#define Tcl_ConvertElement \
	(tclStubsPtr->tcl_ConvertElement) 
#endif
#ifndef Tcl_ConvertCountedElement
#define Tcl_ConvertCountedElement \
	(tclStubsPtr->tcl_ConvertCountedElement) 
#endif
#ifndef Tcl_CreateAlias
#define Tcl_CreateAlias \
	(tclStubsPtr->tcl_CreateAlias) 
#endif
#ifndef Tcl_CreateAliasObj
#define Tcl_CreateAliasObj \
	(tclStubsPtr->tcl_CreateAliasObj) 
#endif
#ifndef Tcl_CreateChannel
#define Tcl_CreateChannel \
	(tclStubsPtr->tcl_CreateChannel) 
#endif
#ifndef Tcl_CreateChannelHandler
#define Tcl_CreateChannelHandler \
	(tclStubsPtr->tcl_CreateChannelHandler) 
#endif
#ifndef Tcl_CreateCloseHandler
#define Tcl_CreateCloseHandler \
	(tclStubsPtr->tcl_CreateCloseHandler) 
#endif
#ifndef Tcl_CreateCommand
#define Tcl_CreateCommand \
	(tclStubsPtr->tcl_CreateCommand) 
#endif
#ifndef Tcl_CreateEventSource
#define Tcl_CreateEventSource \
	(tclStubsPtr->tcl_CreateEventSource) 
#endif
#ifndef Tcl_CreateExitHandler
#define Tcl_CreateExitHandler \
	(tclStubsPtr->tcl_CreateExitHandler) 
#endif
#ifndef Tcl_CreateInterp
#define Tcl_CreateInterp \
	(tclStubsPtr->tcl_CreateInterp) 
#endif
#ifndef Tcl_CreateMathFunc
#define Tcl_CreateMathFunc \
	(tclStubsPtr->tcl_CreateMathFunc) 
#endif
#ifndef Tcl_CreateObjCommand
#define Tcl_CreateObjCommand \
	(tclStubsPtr->tcl_CreateObjCommand) 
#endif
#ifndef Tcl_CreateSlave
#define Tcl_CreateSlave \
	(tclStubsPtr->tcl_CreateSlave) 
#endif
#ifndef Tcl_CreateTimerHandler
#define Tcl_CreateTimerHandler \
	(tclStubsPtr->tcl_CreateTimerHandler) 
#endif
#ifndef Tcl_CreateTrace
#define Tcl_CreateTrace \
	(tclStubsPtr->tcl_CreateTrace) 
#endif
#ifndef Tcl_DeleteAssocData
#define Tcl_DeleteAssocData \
	(tclStubsPtr->tcl_DeleteAssocData) 
#endif
#ifndef Tcl_DeleteChannelHandler
#define Tcl_DeleteChannelHandler \
	(tclStubsPtr->tcl_DeleteChannelHandler) 
#endif
#ifndef Tcl_DeleteCloseHandler
#define Tcl_DeleteCloseHandler \
	(tclStubsPtr->tcl_DeleteCloseHandler) 
#endif
#ifndef Tcl_DeleteCommand
#define Tcl_DeleteCommand \
	(tclStubsPtr->tcl_DeleteCommand) 
#endif
#ifndef Tcl_DeleteCommandFromToken
#define Tcl_DeleteCommandFromToken \
	(tclStubsPtr->tcl_DeleteCommandFromToken) 
#endif
#ifndef Tcl_DeleteEvents
#define Tcl_DeleteEvents \
	(tclStubsPtr->tcl_DeleteEvents) 
#endif
#ifndef Tcl_DeleteEventSource
#define Tcl_DeleteEventSource \
	(tclStubsPtr->tcl_DeleteEventSource) 
#endif
#ifndef Tcl_DeleteExitHandler
#define Tcl_DeleteExitHandler \
	(tclStubsPtr->tcl_DeleteExitHandler) 
#endif
#ifndef Tcl_DeleteHashEntry
#define Tcl_DeleteHashEntry \
	(tclStubsPtr->tcl_DeleteHashEntry) 
#endif
#ifndef Tcl_DeleteHashTable
#define Tcl_DeleteHashTable \
	(tclStubsPtr->tcl_DeleteHashTable) 
#endif
#ifndef Tcl_DeleteInterp
#define Tcl_DeleteInterp \
	(tclStubsPtr->tcl_DeleteInterp) 
#endif
#ifndef Tcl_DetachPids
#define Tcl_DetachPids \
	(tclStubsPtr->tcl_DetachPids) 
#endif
#ifndef Tcl_DeleteTimerHandler
#define Tcl_DeleteTimerHandler \
	(tclStubsPtr->tcl_DeleteTimerHandler) 
#endif
#ifndef Tcl_DeleteTrace
#define Tcl_DeleteTrace \
	(tclStubsPtr->tcl_DeleteTrace) 
#endif
#ifndef Tcl_DontCallWhenDeleted
#define Tcl_DontCallWhenDeleted \
	(tclStubsPtr->tcl_DontCallWhenDeleted) 
#endif
#ifndef Tcl_DoOneEvent
#define Tcl_DoOneEvent \
	(tclStubsPtr->tcl_DoOneEvent) 
#endif
#ifndef Tcl_DoWhenIdle
#define Tcl_DoWhenIdle \
	(tclStubsPtr->tcl_DoWhenIdle) 
#endif
#ifndef Tcl_DStringAppend
#define Tcl_DStringAppend \
	(tclStubsPtr->tcl_DStringAppend) 
#endif
#ifndef Tcl_DStringAppendElement
#define Tcl_DStringAppendElement \
	(tclStubsPtr->tcl_DStringAppendElement) 
#endif
#ifndef Tcl_DStringEndSublist
#define Tcl_DStringEndSublist \
	(tclStubsPtr->tcl_DStringEndSublist) 
#endif
#ifndef Tcl_DStringFree
#define Tcl_DStringFree \
	(tclStubsPtr->tcl_DStringFree) 
#endif
#ifndef Tcl_DStringGetResult
#define Tcl_DStringGetResult \
	(tclStubsPtr->tcl_DStringGetResult) 
#endif
#ifndef Tcl_DStringInit
#define Tcl_DStringInit \
	(tclStubsPtr->tcl_DStringInit) 
#endif
#ifndef Tcl_DStringResult
#define Tcl_DStringResult \
	(tclStubsPtr->tcl_DStringResult) 
#endif
#ifndef Tcl_DStringSetLength
#define Tcl_DStringSetLength \
	(tclStubsPtr->tcl_DStringSetLength) 
#endif
#ifndef Tcl_DStringStartSublist
#define Tcl_DStringStartSublist \
	(tclStubsPtr->tcl_DStringStartSublist) 
#endif
#ifndef Tcl_Eof
#define Tcl_Eof \
	(tclStubsPtr->tcl_Eof) 
#endif
#ifndef Tcl_ErrnoId
#define Tcl_ErrnoId \
	(tclStubsPtr->tcl_ErrnoId) 
#endif
#ifndef Tcl_ErrnoMsg
#define Tcl_ErrnoMsg \
	(tclStubsPtr->tcl_ErrnoMsg) 
#endif
#ifndef Tcl_Eval
#define Tcl_Eval \
	(tclStubsPtr->tcl_Eval) 
#endif
#ifndef Tcl_EvalFile
#define Tcl_EvalFile \
	(tclStubsPtr->tcl_EvalFile) 
#endif
#ifndef Tcl_EvalObj
#define Tcl_EvalObj \
	(tclStubsPtr->tcl_EvalObj) 
#endif
#ifndef Tcl_EventuallyFree
#define Tcl_EventuallyFree \
	(tclStubsPtr->tcl_EventuallyFree) 
#endif
#ifndef Tcl_Exit
#define Tcl_Exit \
	(tclStubsPtr->tcl_Exit) 
#endif
#ifndef Tcl_ExposeCommand
#define Tcl_ExposeCommand \
	(tclStubsPtr->tcl_ExposeCommand) 
#endif
#ifndef Tcl_ExprBoolean
#define Tcl_ExprBoolean \
	(tclStubsPtr->tcl_ExprBoolean) 
#endif
#ifndef Tcl_ExprBooleanObj
#define Tcl_ExprBooleanObj \
	(tclStubsPtr->tcl_ExprBooleanObj) 
#endif
#ifndef Tcl_ExprDouble
#define Tcl_ExprDouble \
	(tclStubsPtr->tcl_ExprDouble) 
#endif
#ifndef Tcl_ExprDoubleObj
#define Tcl_ExprDoubleObj \
	(tclStubsPtr->tcl_ExprDoubleObj) 
#endif
#ifndef Tcl_ExprLong
#define Tcl_ExprLong \
	(tclStubsPtr->tcl_ExprLong) 
#endif
#ifndef Tcl_ExprLongObj
#define Tcl_ExprLongObj \
	(tclStubsPtr->tcl_ExprLongObj) 
#endif
#ifndef Tcl_ExprObj
#define Tcl_ExprObj \
	(tclStubsPtr->tcl_ExprObj) 
#endif
#ifndef Tcl_ExprString
#define Tcl_ExprString \
	(tclStubsPtr->tcl_ExprString) 
#endif
#ifndef Tcl_Finalize
#define Tcl_Finalize \
	(tclStubsPtr->tcl_Finalize) 
#endif
#ifndef Tcl_FindExecutable
#define Tcl_FindExecutable \
	(tclStubsPtr->tcl_FindExecutable) 
#endif
#ifndef Tcl_FirstHashEntry
#define Tcl_FirstHashEntry \
	(tclStubsPtr->tcl_FirstHashEntry) 
#endif
#ifndef Tcl_Flush
#define Tcl_Flush \
	(tclStubsPtr->tcl_Flush) 
#endif
#ifndef Tcl_FreeResult
#define Tcl_FreeResult \
	(tclStubsPtr->tcl_FreeResult) 
#endif
#ifndef Tcl_GetAlias
#define Tcl_GetAlias \
	(tclStubsPtr->tcl_GetAlias) 
#endif
#ifndef Tcl_GetAliasObj
#define Tcl_GetAliasObj \
	(tclStubsPtr->tcl_GetAliasObj) 
#endif
#ifndef Tcl_GetAssocData
#define Tcl_GetAssocData \
	(tclStubsPtr->tcl_GetAssocData) 
#endif
#ifndef Tcl_GetChannel
#define Tcl_GetChannel \
	(tclStubsPtr->tcl_GetChannel) 
#endif
#ifndef Tcl_GetChannelBufferSize
#define Tcl_GetChannelBufferSize \
	(tclStubsPtr->tcl_GetChannelBufferSize) 
#endif
#ifndef Tcl_GetChannelHandle
#define Tcl_GetChannelHandle \
	(tclStubsPtr->tcl_GetChannelHandle) 
#endif
#ifndef Tcl_GetChannelInstanceData
#define Tcl_GetChannelInstanceData \
	(tclStubsPtr->tcl_GetChannelInstanceData) 
#endif
#ifndef Tcl_GetChannelMode
#define Tcl_GetChannelMode \
	(tclStubsPtr->tcl_GetChannelMode) 
#endif
#ifndef Tcl_GetChannelName
#define Tcl_GetChannelName \
	(tclStubsPtr->tcl_GetChannelName) 
#endif
#ifndef Tcl_GetChannelOption
#define Tcl_GetChannelOption \
	(tclStubsPtr->tcl_GetChannelOption) 
#endif
#ifndef Tcl_GetChannelType
#define Tcl_GetChannelType \
	(tclStubsPtr->tcl_GetChannelType) 
#endif
#ifndef Tcl_GetCommandInfo
#define Tcl_GetCommandInfo \
	(tclStubsPtr->tcl_GetCommandInfo) 
#endif
#ifndef Tcl_GetCommandName
#define Tcl_GetCommandName \
	(tclStubsPtr->tcl_GetCommandName) 
#endif
#ifndef Tcl_GetErrno
#define Tcl_GetErrno \
	(tclStubsPtr->tcl_GetErrno) 
#endif
#ifndef Tcl_GetHostName
#define Tcl_GetHostName \
	(tclStubsPtr->tcl_GetHostName) 
#endif
#ifndef Tcl_GetInterpPath
#define Tcl_GetInterpPath \
	(tclStubsPtr->tcl_GetInterpPath) 
#endif
#ifndef Tcl_GetMaster
#define Tcl_GetMaster \
	(tclStubsPtr->tcl_GetMaster) 
#endif
#ifndef Tcl_GetNameOfExecutable
#define Tcl_GetNameOfExecutable \
	(tclStubsPtr->tcl_GetNameOfExecutable) 
#endif
#ifndef Tcl_GetObjResult
#define Tcl_GetObjResult \
	(tclStubsPtr->tcl_GetObjResult) 
#endif
#if !defined(__WIN32__) && !defined(MAC_OSX_TCL) 
#ifndef Tcl_GetOpenFile
#define Tcl_GetOpenFile \
	(tclStubsPtr->tcl_GetOpenFile) 
#endif
#endif 
#ifdef MAC_OSX_TCL 
#ifndef Tcl_GetOpenFile
#define Tcl_GetOpenFile \
	(tclStubsPtr->tcl_GetOpenFile) 
#endif
#endif 
#ifndef Tcl_GetPathType
#define Tcl_GetPathType \
	(tclStubsPtr->tcl_GetPathType) 
#endif
#ifndef Tcl_Gets
#define Tcl_Gets \
	(tclStubsPtr->tcl_Gets) 
#endif
#ifndef Tcl_GetsObj
#define Tcl_GetsObj \
	(tclStubsPtr->tcl_GetsObj) 
#endif
#ifndef Tcl_GetServiceMode
#define Tcl_GetServiceMode \
	(tclStubsPtr->tcl_GetServiceMode) 
#endif
#ifndef Tcl_GetSlave
#define Tcl_GetSlave \
	(tclStubsPtr->tcl_GetSlave) 
#endif
#ifndef Tcl_GetStdChannel
#define Tcl_GetStdChannel \
	(tclStubsPtr->tcl_GetStdChannel) 
#endif
#ifndef Tcl_GetStringResult
#define Tcl_GetStringResult \
	(tclStubsPtr->tcl_GetStringResult) 
#endif
#ifndef Tcl_GetVar
#define Tcl_GetVar \
	(tclStubsPtr->tcl_GetVar) 
#endif
#ifndef Tcl_GetVar2
#define Tcl_GetVar2 \
	(tclStubsPtr->tcl_GetVar2) 
#endif
#ifndef Tcl_GlobalEval
#define Tcl_GlobalEval \
	(tclStubsPtr->tcl_GlobalEval) 
#endif
#ifndef Tcl_GlobalEvalObj
#define Tcl_GlobalEvalObj \
	(tclStubsPtr->tcl_GlobalEvalObj) 
#endif
#ifndef Tcl_HideCommand
#define Tcl_HideCommand \
	(tclStubsPtr->tcl_HideCommand) 
#endif
#ifndef Tcl_Init
#define Tcl_Init \
	(tclStubsPtr->tcl_Init) 
#endif
#ifndef Tcl_InitHashTable
#define Tcl_InitHashTable \
	(tclStubsPtr->tcl_InitHashTable) 
#endif
#ifndef Tcl_InputBlocked
#define Tcl_InputBlocked \
	(tclStubsPtr->tcl_InputBlocked) 
#endif
#ifndef Tcl_InputBuffered
#define Tcl_InputBuffered \
	(tclStubsPtr->tcl_InputBuffered) 
#endif
#ifndef Tcl_InterpDeleted
#define Tcl_InterpDeleted \
	(tclStubsPtr->tcl_InterpDeleted) 
#endif
#ifndef Tcl_IsSafe
#define Tcl_IsSafe \
	(tclStubsPtr->tcl_IsSafe) 
#endif
#ifndef Tcl_JoinPath
#define Tcl_JoinPath \
	(tclStubsPtr->tcl_JoinPath) 
#endif
#ifndef Tcl_LinkVar
#define Tcl_LinkVar \
	(tclStubsPtr->tcl_LinkVar) 
#endif

#ifndef Tcl_MakeFileChannel
#define Tcl_MakeFileChannel \
	(tclStubsPtr->tcl_MakeFileChannel) 
#endif
#ifndef Tcl_MakeSafe
#define Tcl_MakeSafe \
	(tclStubsPtr->tcl_MakeSafe) 
#endif
#ifndef Tcl_MakeTcpClientChannel
#define Tcl_MakeTcpClientChannel \
	(tclStubsPtr->tcl_MakeTcpClientChannel) 
#endif
#ifndef Tcl_Merge
#define Tcl_Merge \
	(tclStubsPtr->tcl_Merge) 
#endif
#ifndef Tcl_NextHashEntry
#define Tcl_NextHashEntry \
	(tclStubsPtr->tcl_NextHashEntry) 
#endif
#ifndef Tcl_NotifyChannel
#define Tcl_NotifyChannel \
	(tclStubsPtr->tcl_NotifyChannel) 
#endif
#ifndef Tcl_ObjGetVar2
#define Tcl_ObjGetVar2 \
	(tclStubsPtr->tcl_ObjGetVar2) 
#endif
#ifndef Tcl_ObjSetVar2
#define Tcl_ObjSetVar2 \
	(tclStubsPtr->tcl_ObjSetVar2) 
#endif
#ifndef Tcl_OpenCommandChannel
#define Tcl_OpenCommandChannel \
	(tclStubsPtr->tcl_OpenCommandChannel) 
#endif
#ifndef Tcl_OpenFileChannel
#define Tcl_OpenFileChannel \
	(tclStubsPtr->tcl_OpenFileChannel) 
#endif
#ifndef Tcl_OpenTcpClient
#define Tcl_OpenTcpClient \
	(tclStubsPtr->tcl_OpenTcpClient) 
#endif
#ifndef Tcl_OpenTcpServer
#define Tcl_OpenTcpServer \
	(tclStubsPtr->tcl_OpenTcpServer) 
#endif
#ifndef Tcl_Preserve
#define Tcl_Preserve \
	(tclStubsPtr->tcl_Preserve) 
#endif
#ifndef Tcl_PrintDouble
#define Tcl_PrintDouble \
	(tclStubsPtr->tcl_PrintDouble) 
#endif
#ifndef Tcl_PutEnv
#define Tcl_PutEnv \
	(tclStubsPtr->tcl_PutEnv) 
#endif
#ifndef Tcl_PosixError
#define Tcl_PosixError \
	(tclStubsPtr->tcl_PosixError) 
#endif
#ifndef Tcl_QueueEvent
#define Tcl_QueueEvent \
	(tclStubsPtr->tcl_QueueEvent) 
#endif
#ifndef Tcl_Read
#define Tcl_Read \
	(tclStubsPtr->tcl_Read) 
#endif
#ifndef Tcl_ReapDetachedProcs
#define Tcl_ReapDetachedProcs \
	(tclStubsPtr->tcl_ReapDetachedProcs) 
#endif
#ifndef Tcl_RecordAndEval
#define Tcl_RecordAndEval \
	(tclStubsPtr->tcl_RecordAndEval) 
#endif
#ifndef Tcl_RecordAndEvalObj
#define Tcl_RecordAndEvalObj \
	(tclStubsPtr->tcl_RecordAndEvalObj) 
#endif
#ifndef Tcl_RegisterChannel
#define Tcl_RegisterChannel \
	(tclStubsPtr->tcl_RegisterChannel) 
#endif
#ifndef Tcl_RegisterObjType
#define Tcl_RegisterObjType \
	(tclStubsPtr->tcl_RegisterObjType) 
#endif
#ifndef Tcl_RegExpCompile
#define Tcl_RegExpCompile \
	(tclStubsPtr->tcl_RegExpCompile) 
#endif
#ifndef Tcl_RegExpExec
#define Tcl_RegExpExec \
	(tclStubsPtr->tcl_RegExpExec) 
#endif
#ifndef Tcl_RegExpMatch
#define Tcl_RegExpMatch \
	(tclStubsPtr->tcl_RegExpMatch) 
#endif
#ifndef Tcl_RegExpRange
#define Tcl_RegExpRange \
	(tclStubsPtr->tcl_RegExpRange) 
#endif
#ifndef Tcl_Release
#define Tcl_Release \
	(tclStubsPtr->tcl_Release) 
#endif
#ifndef Tcl_ResetResult
#define Tcl_ResetResult \
	(tclStubsPtr->tcl_ResetResult) 
#endif
#ifndef Tcl_ScanElement
#define Tcl_ScanElement \
	(tclStubsPtr->tcl_ScanElement) 
#endif
#ifndef Tcl_ScanCountedElement
#define Tcl_ScanCountedElement \
	(tclStubsPtr->tcl_ScanCountedElement) 
#endif
#ifndef Tcl_SeekOld
#define Tcl_SeekOld \
	(tclStubsPtr->tcl_SeekOld) 
#endif
#ifndef Tcl_ServiceAll
#define Tcl_ServiceAll \
	(tclStubsPtr->tcl_ServiceAll) 
#endif
#ifndef Tcl_ServiceEvent
#define Tcl_ServiceEvent \
	(tclStubsPtr->tcl_ServiceEvent) 
#endif
#ifndef Tcl_SetAssocData
#define Tcl_SetAssocData \
	(tclStubsPtr->tcl_SetAssocData) 
#endif
#ifndef Tcl_SetChannelBufferSize
#define Tcl_SetChannelBufferSize \
	(tclStubsPtr->tcl_SetChannelBufferSize) 
#endif
#ifndef Tcl_SetChannelOption
#define Tcl_SetChannelOption \
	(tclStubsPtr->tcl_SetChannelOption) 
#endif
#ifndef Tcl_SetCommandInfo
#define Tcl_SetCommandInfo \
	(tclStubsPtr->tcl_SetCommandInfo) 
#endif
#ifndef Tcl_SetErrno
#define Tcl_SetErrno \
	(tclStubsPtr->tcl_SetErrno) 
#endif
#ifndef Tcl_SetErrorCode
#define Tcl_SetErrorCode \
	(tclStubsPtr->tcl_SetErrorCode) 
#endif
#ifndef Tcl_SetMaxBlockTime
#define Tcl_SetMaxBlockTime \
	(tclStubsPtr->tcl_SetMaxBlockTime) 
#endif
#ifndef Tcl_SetPanicProc
#define Tcl_SetPanicProc \
	(tclStubsPtr->tcl_SetPanicProc) 
#endif
#ifndef Tcl_SetRecursionLimit
#define Tcl_SetRecursionLimit \
	(tclStubsPtr->tcl_SetRecursionLimit) 
#endif
#ifndef Tcl_SetResult
#define Tcl_SetResult \
	(tclStubsPtr->tcl_SetResult) 
#endif
#ifndef Tcl_SetServiceMode
#define Tcl_SetServiceMode \
	(tclStubsPtr->tcl_SetServiceMode) 
#endif
#ifndef Tcl_SetObjErrorCode
#define Tcl_SetObjErrorCode \
	(tclStubsPtr->tcl_SetObjErrorCode) 
#endif
#ifndef Tcl_SetObjResult
#define Tcl_SetObjResult \
	(tclStubsPtr->tcl_SetObjResult) 
#endif
#ifndef Tcl_SetStdChannel
#define Tcl_SetStdChannel \
	(tclStubsPtr->tcl_SetStdChannel) 
#endif
#ifndef Tcl_SetVar
#define Tcl_SetVar \
	(tclStubsPtr->tcl_SetVar) 
#endif
#ifndef Tcl_SetVar2
#define Tcl_SetVar2 \
	(tclStubsPtr->tcl_SetVar2) 
#endif
#ifndef Tcl_SignalId
#define Tcl_SignalId \
	(tclStubsPtr->tcl_SignalId) 
#endif
#ifndef Tcl_SignalMsg
#define Tcl_SignalMsg \
	(tclStubsPtr->tcl_SignalMsg) 
#endif
#ifndef Tcl_SourceRCFile
#define Tcl_SourceRCFile \
	(tclStubsPtr->tcl_SourceRCFile) 
#endif
#ifndef Tcl_SplitList
#define Tcl_SplitList \
	(tclStubsPtr->tcl_SplitList) 
#endif
#ifndef Tcl_SplitPath
#define Tcl_SplitPath \
	(tclStubsPtr->tcl_SplitPath) 
#endif
#ifndef Tcl_StaticPackage
#define Tcl_StaticPackage \
	(tclStubsPtr->tcl_StaticPackage) 
#endif
#ifndef Tcl_StringMatch
#define Tcl_StringMatch \
	(tclStubsPtr->tcl_StringMatch) 
#endif
#ifndef Tcl_TellOld
#define Tcl_TellOld \
	(tclStubsPtr->tcl_TellOld) 
#endif
#ifndef Tcl_TraceVar
#define Tcl_TraceVar \
	(tclStubsPtr->tcl_TraceVar) 
#endif
#ifndef Tcl_TraceVar2
#define Tcl_TraceVar2 \
	(tclStubsPtr->tcl_TraceVar2) 
#endif
#ifndef Tcl_TranslateFileName
#define Tcl_TranslateFileName \
	(tclStubsPtr->tcl_TranslateFileName) 
#endif
#ifndef Tcl_Ungets
#define Tcl_Ungets \
	(tclStubsPtr->tcl_Ungets) 
#endif
#ifndef Tcl_UnlinkVar
#define Tcl_UnlinkVar \
	(tclStubsPtr->tcl_UnlinkVar) 
#endif
#ifndef Tcl_UnregisterChannel
#define Tcl_UnregisterChannel \
	(tclStubsPtr->tcl_UnregisterChannel) 
#endif
#ifndef Tcl_UnsetVar
#define Tcl_UnsetVar \
	(tclStubsPtr->tcl_UnsetVar) 
#endif
#ifndef Tcl_UnsetVar2
#define Tcl_UnsetVar2 \
	(tclStubsPtr->tcl_UnsetVar2) 
#endif
#ifndef Tcl_UntraceVar
#define Tcl_UntraceVar \
	(tclStubsPtr->tcl_UntraceVar) 
#endif
#ifndef Tcl_UntraceVar2
#define Tcl_UntraceVar2 \
	(tclStubsPtr->tcl_UntraceVar2) 
#endif
#ifndef Tcl_UpdateLinkedVar
#define Tcl_UpdateLinkedVar \
	(tclStubsPtr->tcl_UpdateLinkedVar) 
#endif
#ifndef Tcl_UpVar
#define Tcl_UpVar \
	(tclStubsPtr->tcl_UpVar) 
#endif
#ifndef Tcl_UpVar2
#define Tcl_UpVar2 \
	(tclStubsPtr->tcl_UpVar2) 
#endif
#ifndef Tcl_VarEval
#define Tcl_VarEval \
	(tclStubsPtr->tcl_VarEval) 
#endif
#ifndef Tcl_VarTraceInfo
#define Tcl_VarTraceInfo \
	(tclStubsPtr->tcl_VarTraceInfo) 
#endif
#ifndef Tcl_VarTraceInfo2
#define Tcl_VarTraceInfo2 \
	(tclStubsPtr->tcl_VarTraceInfo2) 
#endif
#ifndef Tcl_Write
#define Tcl_Write \
	(tclStubsPtr->tcl_Write) 
#endif
#ifndef Tcl_WrongNumArgs
#define Tcl_WrongNumArgs \
	(tclStubsPtr->tcl_WrongNumArgs) 
#endif
#ifndef Tcl_DumpActiveMemory
#define Tcl_DumpActiveMemory \
	(tclStubsPtr->tcl_DumpActiveMemory) 
#endif
#ifndef Tcl_ValidateAllMemory
#define Tcl_ValidateAllMemory \
	(tclStubsPtr->tcl_ValidateAllMemory) 
#endif
#ifndef Tcl_AppendResultVA
#define Tcl_AppendResultVA \
	(tclStubsPtr->tcl_AppendResultVA) 
#endif
#ifndef Tcl_AppendStringsToObjVA
#define Tcl_AppendStringsToObjVA \
	(tclStubsPtr->tcl_AppendStringsToObjVA) 
#endif
#ifndef Tcl_HashStats
#define Tcl_HashStats \
	(tclStubsPtr->tcl_HashStats) 
#endif
#ifndef Tcl_ParseVar
#define Tcl_ParseVar \
	(tclStubsPtr->tcl_ParseVar) 
#endif
#ifndef Tcl_PkgPresent
#define Tcl_PkgPresent \
	(tclStubsPtr->tcl_PkgPresent) 
#endif
#ifndef Tcl_PkgPresentEx
#define Tcl_PkgPresentEx \
	(tclStubsPtr->tcl_PkgPresentEx) 
#endif
#ifndef Tcl_PkgProvide
#define Tcl_PkgProvide \
	(tclStubsPtr->tcl_PkgProvide) 
#endif
#ifndef Tcl_PkgRequire
#define Tcl_PkgRequire \
	(tclStubsPtr->tcl_PkgRequire) 
#endif
#ifndef Tcl_SetErrorCodeVA
#define Tcl_SetErrorCodeVA \
	(tclStubsPtr->tcl_SetErrorCodeVA) 
#endif
#ifndef Tcl_VarEvalVA
#define Tcl_VarEvalVA \
	(tclStubsPtr->tcl_VarEvalVA) 
#endif
#ifndef Tcl_WaitPid
#define Tcl_WaitPid \
	(tclStubsPtr->tcl_WaitPid) 
#endif
#ifndef Tcl_PanicVA
#define Tcl_PanicVA \
	(tclStubsPtr->tcl_PanicVA) 
#endif
#ifndef Tcl_GetVersion
#define Tcl_GetVersion \
	(tclStubsPtr->tcl_GetVersion) 
#endif
#ifndef Tcl_InitMemory
#define Tcl_InitMemory \
	(tclStubsPtr->tcl_InitMemory) 
#endif
#ifndef Tcl_StackChannel
#define Tcl_StackChannel \
	(tclStubsPtr->tcl_StackChannel) 
#endif
#ifndef Tcl_UnstackChannel
#define Tcl_UnstackChannel \
	(tclStubsPtr->tcl_UnstackChannel) 
#endif
#ifndef Tcl_GetStackedChannel
#define Tcl_GetStackedChannel \
	(tclStubsPtr->tcl_GetStackedChannel) 
#endif
#ifndef Tcl_SetMainLoop
#define Tcl_SetMainLoop \
	(tclStubsPtr->tcl_SetMainLoop) 
#endif

#ifndef Tcl_AppendObjToObj
#define Tcl_AppendObjToObj \
	(tclStubsPtr->tcl_AppendObjToObj) 
#endif
#ifndef Tcl_CreateEncoding
#define Tcl_CreateEncoding \
	(tclStubsPtr->tcl_CreateEncoding) 
#endif
#ifndef Tcl_CreateThreadExitHandler
#define Tcl_CreateThreadExitHandler \
	(tclStubsPtr->tcl_CreateThreadExitHandler) 
#endif
#ifndef Tcl_DeleteThreadExitHandler
#define Tcl_DeleteThreadExitHandler \
	(tclStubsPtr->tcl_DeleteThreadExitHandler) 
#endif
#ifndef Tcl_DiscardResult
#define Tcl_DiscardResult \
	(tclStubsPtr->tcl_DiscardResult) 
#endif
#ifndef Tcl_EvalEx
#define Tcl_EvalEx \
	(tclStubsPtr->tcl_EvalEx) 
#endif
#ifndef Tcl_EvalObjv
#define Tcl_EvalObjv \
	(tclStubsPtr->tcl_EvalObjv) 
#endif
#ifndef Tcl_EvalObjEx
#define Tcl_EvalObjEx \
	(tclStubsPtr->tcl_EvalObjEx) 
#endif
#ifndef Tcl_ExitThread
#define Tcl_ExitThread \
	(tclStubsPtr->tcl_ExitThread) 
#endif
#ifndef Tcl_ExternalToUtf
#define Tcl_ExternalToUtf \
	(tclStubsPtr->tcl_ExternalToUtf) 
#endif
#ifndef Tcl_ExternalToUtfDString
#define Tcl_ExternalToUtfDString \
	(tclStubsPtr->tcl_ExternalToUtfDString) 
#endif
#ifndef Tcl_FinalizeThread
#define Tcl_FinalizeThread \
	(tclStubsPtr->tcl_FinalizeThread) 
#endif
#ifndef Tcl_FinalizeNotifier
#define Tcl_FinalizeNotifier \
	(tclStubsPtr->tcl_FinalizeNotifier) 
#endif
#ifndef Tcl_FreeEncoding
#define Tcl_FreeEncoding \
	(tclStubsPtr->tcl_FreeEncoding) 
#endif
#ifndef Tcl_GetCurrentThread
#define Tcl_GetCurrentThread \
	(tclStubsPtr->tcl_GetCurrentThread) 
#endif
#ifndef Tcl_GetEncoding
#define Tcl_GetEncoding \
	(tclStubsPtr->tcl_GetEncoding) 
#endif
#ifndef Tcl_GetEncodingName
#define Tcl_GetEncodingName \
	(tclStubsPtr->tcl_GetEncodingName) 
#endif
#ifndef Tcl_GetEncodingNames
#define Tcl_GetEncodingNames \
	(tclStubsPtr->tcl_GetEncodingNames) 
#endif
#ifndef Tcl_GetIndexFromObjStruct
#define Tcl_GetIndexFromObjStruct \
	(tclStubsPtr->tcl_GetIndexFromObjStruct) 
#endif
#ifndef Tcl_GetThreadData
#define Tcl_GetThreadData \
	(tclStubsPtr->tcl_GetThreadData) 
#endif
#ifndef Tcl_GetVar2Ex
#define Tcl_GetVar2Ex \
	(tclStubsPtr->tcl_GetVar2Ex) 
#endif
#ifndef Tcl_InitNotifier
#define Tcl_InitNotifier \
	(tclStubsPtr->tcl_InitNotifier) 
#endif
#ifndef Tcl_MutexLock
#define Tcl_MutexLock \
	(tclStubsPtr->tcl_MutexLock) 
#endif
#ifndef Tcl_MutexUnlock
#define Tcl_MutexUnlock \
	(tclStubsPtr->tcl_MutexUnlock) 
#endif
#ifndef Tcl_ConditionNotify
#define Tcl_ConditionNotify \
	(tclStubsPtr->tcl_ConditionNotify) 
#endif
#ifndef Tcl_ConditionWait
#define Tcl_ConditionWait \
	(tclStubsPtr->tcl_ConditionWait) 
#endif
#ifndef Tcl_NumUtfChars
#define Tcl_NumUtfChars \
	(tclStubsPtr->tcl_NumUtfChars) 
#endif
#ifndef Tcl_ReadChars
#define Tcl_ReadChars \
	(tclStubsPtr->tcl_ReadChars) 
#endif
#ifndef Tcl_RestoreResult
#define Tcl_RestoreResult \
	(tclStubsPtr->tcl_RestoreResult) 
#endif
#ifndef Tcl_SaveResult
#define Tcl_SaveResult \
	(tclStubsPtr->tcl_SaveResult) 
#endif
#ifndef Tcl_SetSystemEncoding
#define Tcl_SetSystemEncoding \
	(tclStubsPtr->tcl_SetSystemEncoding) 
#endif
#ifndef Tcl_SetVar2Ex
#define Tcl_SetVar2Ex \
	(tclStubsPtr->tcl_SetVar2Ex) 
#endif
#ifndef Tcl_ThreadAlert
#define Tcl_ThreadAlert \
	(tclStubsPtr->tcl_ThreadAlert) 
#endif
#ifndef Tcl_ThreadQueueEvent
#define Tcl_ThreadQueueEvent \
	(tclStubsPtr->tcl_ThreadQueueEvent) 
#endif
#ifndef Tcl_UniCharAtIndex
#define Tcl_UniCharAtIndex \
	(tclStubsPtr->tcl_UniCharAtIndex) 
#endif
#ifndef Tcl_UniCharToLower
#define Tcl_UniCharToLower \
	(tclStubsPtr->tcl_UniCharToLower) 
#endif
#ifndef Tcl_UniCharToTitle
#define Tcl_UniCharToTitle \
	(tclStubsPtr->tcl_UniCharToTitle) 
#endif
#ifndef Tcl_UniCharToUpper
#define Tcl_UniCharToUpper \
	(tclStubsPtr->tcl_UniCharToUpper) 
#endif
#ifndef Tcl_UniCharToUtf
#define Tcl_UniCharToUtf \
	(tclStubsPtr->tcl_UniCharToUtf) 
#endif
#ifndef Tcl_UtfAtIndex
#define Tcl_UtfAtIndex \
	(tclStubsPtr->tcl_UtfAtIndex) 
#endif
#ifndef Tcl_UtfCharComplete
#define Tcl_UtfCharComplete \
	(tclStubsPtr->tcl_UtfCharComplete) 
#endif
#ifndef Tcl_UtfBackslash
#define Tcl_UtfBackslash \
	(tclStubsPtr->tcl_UtfBackslash) 
#endif
#ifndef Tcl_UtfFindFirst
#define Tcl_UtfFindFirst \
	(tclStubsPtr->tcl_UtfFindFirst) 
#endif
#ifndef Tcl_UtfFindLast
#define Tcl_UtfFindLast \
	(tclStubsPtr->tcl_UtfFindLast) 
#endif
#ifndef Tcl_UtfNext
#define Tcl_UtfNext \
	(tclStubsPtr->tcl_UtfNext) 
#endif
#ifndef Tcl_UtfPrev
#define Tcl_UtfPrev \
	(tclStubsPtr->tcl_UtfPrev) 
#endif
#ifndef Tcl_UtfToExternal
#define Tcl_UtfToExternal \
	(tclStubsPtr->tcl_UtfToExternal) 
#endif
#ifndef Tcl_UtfToExternalDString
#define Tcl_UtfToExternalDString \
	(tclStubsPtr->tcl_UtfToExternalDString) 
#endif
#ifndef Tcl_UtfToLower
#define Tcl_UtfToLower \
	(tclStubsPtr->tcl_UtfToLower) 
#endif
#ifndef Tcl_UtfToTitle
#define Tcl_UtfToTitle \
	(tclStubsPtr->tcl_UtfToTitle) 
#endif
#ifndef Tcl_UtfToUniChar
#define Tcl_UtfToUniChar \
	(tclStubsPtr->tcl_UtfToUniChar) 
#endif
#ifndef Tcl_UtfToUpper
#define Tcl_UtfToUpper \
	(tclStubsPtr->tcl_UtfToUpper) 
#endif
#ifndef Tcl_WriteChars
#define Tcl_WriteChars \
	(tclStubsPtr->tcl_WriteChars) 
#endif
#ifndef Tcl_WriteObj
#define Tcl_WriteObj \
	(tclStubsPtr->tcl_WriteObj) 
#endif
#ifndef Tcl_GetString
#define Tcl_GetString \
	(tclStubsPtr->tcl_GetString) 
#endif
#ifndef Tcl_GetDefaultEncodingDir
#define Tcl_GetDefaultEncodingDir \
	(tclStubsPtr->tcl_GetDefaultEncodingDir) 
#endif
#ifndef Tcl_SetDefaultEncodingDir
#define Tcl_SetDefaultEncodingDir \
	(tclStubsPtr->tcl_SetDefaultEncodingDir) 
#endif
#ifndef Tcl_AlertNotifier
#define Tcl_AlertNotifier \
	(tclStubsPtr->tcl_AlertNotifier) 
#endif
#ifndef Tcl_ServiceModeHook
#define Tcl_ServiceModeHook \
	(tclStubsPtr->tcl_ServiceModeHook) 
#endif
#ifndef Tcl_UniCharIsAlnum
#define Tcl_UniCharIsAlnum \
	(tclStubsPtr->tcl_UniCharIsAlnum) 
#endif
#ifndef Tcl_UniCharIsAlpha
#define Tcl_UniCharIsAlpha \
	(tclStubsPtr->tcl_UniCharIsAlpha) 
#endif
#ifndef Tcl_UniCharIsDigit
#define Tcl_UniCharIsDigit \
	(tclStubsPtr->tcl_UniCharIsDigit) 
#endif
#ifndef Tcl_UniCharIsLower
#define Tcl_UniCharIsLower \
	(tclStubsPtr->tcl_UniCharIsLower) 
#endif
#ifndef Tcl_UniCharIsSpace
#define Tcl_UniCharIsSpace \
	(tclStubsPtr->tcl_UniCharIsSpace) 
#endif
#ifndef Tcl_UniCharIsUpper
#define Tcl_UniCharIsUpper \
	(tclStubsPtr->tcl_UniCharIsUpper) 
#endif
#ifndef Tcl_UniCharIsWordChar
#define Tcl_UniCharIsWordChar \
	(tclStubsPtr->tcl_UniCharIsWordChar) 
#endif
#ifndef Tcl_UniCharLen
#define Tcl_UniCharLen \
	(tclStubsPtr->tcl_UniCharLen) 
#endif
#ifndef Tcl_UniCharNcmp
#define Tcl_UniCharNcmp \
	(tclStubsPtr->tcl_UniCharNcmp) 
#endif
#ifndef Tcl_UniCharToUtfDString
#define Tcl_UniCharToUtfDString \
	(tclStubsPtr->tcl_UniCharToUtfDString) 
#endif
#ifndef Tcl_UtfToUniCharDString
#define Tcl_UtfToUniCharDString \
	(tclStubsPtr->tcl_UtfToUniCharDString) 
#endif
#ifndef Tcl_GetRegExpFromObj
#define Tcl_GetRegExpFromObj \
	(tclStubsPtr->tcl_GetRegExpFromObj) 
#endif
#ifndef Tcl_EvalTokens
#define Tcl_EvalTokens \
	(tclStubsPtr->tcl_EvalTokens) 
#endif
#ifndef Tcl_FreeParse
#define Tcl_FreeParse \
	(tclStubsPtr->tcl_FreeParse) 
#endif
#ifndef Tcl_LogCommandInfo
#define Tcl_LogCommandInfo \
	(tclStubsPtr->tcl_LogCommandInfo) 
#endif
#ifndef Tcl_ParseBraces
#define Tcl_ParseBraces \
	(tclStubsPtr->tcl_ParseBraces) 
#endif
#ifndef Tcl_ParseCommand
#define Tcl_ParseCommand \
	(tclStubsPtr->tcl_ParseCommand) 
#endif
#ifndef Tcl_ParseExpr
#define Tcl_ParseExpr \
	(tclStubsPtr->tcl_ParseExpr) 
#endif
#ifndef Tcl_ParseQuotedString
#define Tcl_ParseQuotedString \
	(tclStubsPtr->tcl_ParseQuotedString) 
#endif
#ifndef Tcl_ParseVarName
#define Tcl_ParseVarName \
	(tclStubsPtr->tcl_ParseVarName) 
#endif
#ifndef Tcl_GetCwd
#define Tcl_GetCwd \
	(tclStubsPtr->tcl_GetCwd) 
#endif
#ifndef Tcl_Chdir
#define Tcl_Chdir \
	(tclStubsPtr->tcl_Chdir) 
#endif
#ifndef Tcl_Access
#define Tcl_Access \
	(tclStubsPtr->tcl_Access) 
#endif
#ifndef Tcl_Stat
#define Tcl_Stat \
	(tclStubsPtr->tcl_Stat) 
#endif
#ifndef Tcl_UtfNcmp
#define Tcl_UtfNcmp \
	(tclStubsPtr->tcl_UtfNcmp) 
#endif
#ifndef Tcl_UtfNcasecmp
#define Tcl_UtfNcasecmp \
	(tclStubsPtr->tcl_UtfNcasecmp) 
#endif
#ifndef Tcl_StringCaseMatch
#define Tcl_StringCaseMatch \
	(tclStubsPtr->tcl_StringCaseMatch) 
#endif
#ifndef Tcl_UniCharIsControl
#define Tcl_UniCharIsControl \
	(tclStubsPtr->tcl_UniCharIsControl) 
#endif
#ifndef Tcl_UniCharIsGraph
#define Tcl_UniCharIsGraph \
	(tclStubsPtr->tcl_UniCharIsGraph) 
#endif
#ifndef Tcl_UniCharIsPrint
#define Tcl_UniCharIsPrint \
	(tclStubsPtr->tcl_UniCharIsPrint) 
#endif
#ifndef Tcl_UniCharIsPunct
#define Tcl_UniCharIsPunct \
	(tclStubsPtr->tcl_UniCharIsPunct) 
#endif
#ifndef Tcl_RegExpExecObj
#define Tcl_RegExpExecObj \
	(tclStubsPtr->tcl_RegExpExecObj) 
#endif
#ifndef Tcl_RegExpGetInfo
#define Tcl_RegExpGetInfo \
	(tclStubsPtr->tcl_RegExpGetInfo) 
#endif
#ifndef Tcl_NewUnicodeObj
#define Tcl_NewUnicodeObj \
	(tclStubsPtr->tcl_NewUnicodeObj) 
#endif
#ifndef Tcl_SetUnicodeObj
#define Tcl_SetUnicodeObj \
	(tclStubsPtr->tcl_SetUnicodeObj) 
#endif
#ifndef Tcl_GetCharLength
#define Tcl_GetCharLength \
	(tclStubsPtr->tcl_GetCharLength) 
#endif
#ifndef Tcl_GetUniChar
#define Tcl_GetUniChar \
	(tclStubsPtr->tcl_GetUniChar) 
#endif
#ifndef Tcl_GetUnicode
#define Tcl_GetUnicode \
	(tclStubsPtr->tcl_GetUnicode) 
#endif
#ifndef Tcl_GetRange
#define Tcl_GetRange \
	(tclStubsPtr->tcl_GetRange) 
#endif
#ifndef Tcl_AppendUnicodeToObj
#define Tcl_AppendUnicodeToObj \
	(tclStubsPtr->tcl_AppendUnicodeToObj) 
#endif
#ifndef Tcl_RegExpMatchObj
#define Tcl_RegExpMatchObj \
	(tclStubsPtr->tcl_RegExpMatchObj) 
#endif
#ifndef Tcl_SetNotifier
#define Tcl_SetNotifier \
	(tclStubsPtr->tcl_SetNotifier) 
#endif
#ifndef Tcl_GetAllocMutex
#define Tcl_GetAllocMutex \
	(tclStubsPtr->tcl_GetAllocMutex) 
#endif
#ifndef Tcl_GetChannelNames
#define Tcl_GetChannelNames \
	(tclStubsPtr->tcl_GetChannelNames) 
#endif
#ifndef Tcl_GetChannelNamesEx
#define Tcl_GetChannelNamesEx \
	(tclStubsPtr->tcl_GetChannelNamesEx) 
#endif
#ifndef Tcl_ProcObjCmd
#define Tcl_ProcObjCmd \
	(tclStubsPtr->tcl_ProcObjCmd) 
#endif
#ifndef Tcl_ConditionFinalize
#define Tcl_ConditionFinalize \
	(tclStubsPtr->tcl_ConditionFinalize) 
#endif
#ifndef Tcl_MutexFinalize
#define Tcl_MutexFinalize \
	(tclStubsPtr->tcl_MutexFinalize) 
#endif
#ifndef Tcl_CreateThread
#define Tcl_CreateThread \
	(tclStubsPtr->tcl_CreateThread) 
#endif
#ifndef Tcl_ReadRaw
#define Tcl_ReadRaw \
	(tclStubsPtr->tcl_ReadRaw) 
#endif
#ifndef Tcl_WriteRaw
#define Tcl_WriteRaw \
	(tclStubsPtr->tcl_WriteRaw) 
#endif
#ifndef Tcl_GetTopChannel
#define Tcl_GetTopChannel \
	(tclStubsPtr->tcl_GetTopChannel) 
#endif
#ifndef Tcl_ChannelBuffered
#define Tcl_ChannelBuffered \
	(tclStubsPtr->tcl_ChannelBuffered) 
#endif
#ifndef Tcl_ChannelName
#define Tcl_ChannelName \
	(tclStubsPtr->tcl_ChannelName) 
#endif
#ifndef Tcl_ChannelVersion
#define Tcl_ChannelVersion \
	(tclStubsPtr->tcl_ChannelVersion) 
#endif
#ifndef Tcl_ChannelBlockModeProc
#define Tcl_ChannelBlockModeProc \
	(tclStubsPtr->tcl_ChannelBlockModeProc) 
#endif
#ifndef Tcl_ChannelCloseProc
#define Tcl_ChannelCloseProc \
	(tclStubsPtr->tcl_ChannelCloseProc) 
#endif
#ifndef Tcl_ChannelClose2Proc
#define Tcl_ChannelClose2Proc \
	(tclStubsPtr->tcl_ChannelClose2Proc) 
#endif
#ifndef Tcl_ChannelInputProc
#define Tcl_ChannelInputProc \
	(tclStubsPtr->tcl_ChannelInputProc) 
#endif
#ifndef Tcl_ChannelOutputProc
#define Tcl_ChannelOutputProc \
	(tclStubsPtr->tcl_ChannelOutputProc) 
#endif
#ifndef Tcl_ChannelSeekProc
#define Tcl_ChannelSeekProc \
	(tclStubsPtr->tcl_ChannelSeekProc) 
#endif
#ifndef Tcl_ChannelSetOptionProc
#define Tcl_ChannelSetOptionProc \
	(tclStubsPtr->tcl_ChannelSetOptionProc) 
#endif
#ifndef Tcl_ChannelGetOptionProc
#define Tcl_ChannelGetOptionProc \
	(tclStubsPtr->tcl_ChannelGetOptionProc) 
#endif
#ifndef Tcl_ChannelWatchProc
#define Tcl_ChannelWatchProc \
	(tclStubsPtr->tcl_ChannelWatchProc) 
#endif
#ifndef Tcl_ChannelGetHandleProc
#define Tcl_ChannelGetHandleProc \
	(tclStubsPtr->tcl_ChannelGetHandleProc) 
#endif
#ifndef Tcl_ChannelFlushProc
#define Tcl_ChannelFlushProc \
	(tclStubsPtr->tcl_ChannelFlushProc) 
#endif
#ifndef Tcl_ChannelHandlerProc
#define Tcl_ChannelHandlerProc \
	(tclStubsPtr->tcl_ChannelHandlerProc) 
#endif
#ifndef Tcl_JoinThread
#define Tcl_JoinThread \
	(tclStubsPtr->tcl_JoinThread) 
#endif
#ifndef Tcl_IsChannelShared
#define Tcl_IsChannelShared \
	(tclStubsPtr->tcl_IsChannelShared) 
#endif
#ifndef Tcl_IsChannelRegistered
#define Tcl_IsChannelRegistered \
	(tclStubsPtr->tcl_IsChannelRegistered) 
#endif
#ifndef Tcl_CutChannel
#define Tcl_CutChannel \
	(tclStubsPtr->tcl_CutChannel) 
#endif
#ifndef Tcl_SpliceChannel
#define Tcl_SpliceChannel \
	(tclStubsPtr->tcl_SpliceChannel) 
#endif
#ifndef Tcl_ClearChannelHandlers
#define Tcl_ClearChannelHandlers \
	(tclStubsPtr->tcl_ClearChannelHandlers) 
#endif
#ifndef Tcl_IsChannelExisting
#define Tcl_IsChannelExisting \
	(tclStubsPtr->tcl_IsChannelExisting) 
#endif
#ifndef Tcl_UniCharNcasecmp
#define Tcl_UniCharNcasecmp \
	(tclStubsPtr->tcl_UniCharNcasecmp) 
#endif
#ifndef Tcl_UniCharCaseMatch
#define Tcl_UniCharCaseMatch \
	(tclStubsPtr->tcl_UniCharCaseMatch) 
#endif
#ifndef Tcl_FindHashEntry
#define Tcl_FindHashEntry \
	(tclStubsPtr->tcl_FindHashEntry) 
#endif
#ifndef Tcl_CreateHashEntry
#define Tcl_CreateHashEntry \
	(tclStubsPtr->tcl_CreateHashEntry) 
#endif
#ifndef Tcl_InitCustomHashTable
#define Tcl_InitCustomHashTable \
	(tclStubsPtr->tcl_InitCustomHashTable) 
#endif
#ifndef Tcl_InitObjHashTable
#define Tcl_InitObjHashTable \
	(tclStubsPtr->tcl_InitObjHashTable) 
#endif
#ifndef Tcl_CommandTraceInfo
#define Tcl_CommandTraceInfo \
	(tclStubsPtr->tcl_CommandTraceInfo) 
#endif
#ifndef Tcl_TraceCommand
#define Tcl_TraceCommand \
	(tclStubsPtr->tcl_TraceCommand) 
#endif
#ifndef Tcl_UntraceCommand
#define Tcl_UntraceCommand \
	(tclStubsPtr->tcl_UntraceCommand) 
#endif
#ifndef Tcl_AttemptAlloc
#define Tcl_AttemptAlloc \
	(tclStubsPtr->tcl_AttemptAlloc) 
#endif
#ifndef Tcl_AttemptDbCkalloc
#define Tcl_AttemptDbCkalloc \
	(tclStubsPtr->tcl_AttemptDbCkalloc) 
#endif
#ifndef Tcl_AttemptRealloc
#define Tcl_AttemptRealloc \
	(tclStubsPtr->tcl_AttemptRealloc) 
#endif
#ifndef Tcl_AttemptDbCkrealloc
#define Tcl_AttemptDbCkrealloc \
	(tclStubsPtr->tcl_AttemptDbCkrealloc) 
#endif
#ifndef Tcl_AttemptSetObjLength
#define Tcl_AttemptSetObjLength \
	(tclStubsPtr->tcl_AttemptSetObjLength) 
#endif
#ifndef Tcl_GetChannelThread
#define Tcl_GetChannelThread \
	(tclStubsPtr->tcl_GetChannelThread) 
#endif
#ifndef Tcl_GetUnicodeFromObj
#define Tcl_GetUnicodeFromObj \
	(tclStubsPtr->tcl_GetUnicodeFromObj) 
#endif
#ifndef Tcl_GetMathFuncInfo
#define Tcl_GetMathFuncInfo \
	(tclStubsPtr->tcl_GetMathFuncInfo) 
#endif
#ifndef Tcl_ListMathFuncs
#define Tcl_ListMathFuncs \
	(tclStubsPtr->tcl_ListMathFuncs) 
#endif
#ifndef Tcl_SubstObj
#define Tcl_SubstObj \
	(tclStubsPtr->tcl_SubstObj) 
#endif
#ifndef Tcl_DetachChannel
#define Tcl_DetachChannel \
	(tclStubsPtr->tcl_DetachChannel) 
#endif
#ifndef Tcl_IsStandardChannel
#define Tcl_IsStandardChannel \
	(tclStubsPtr->tcl_IsStandardChannel) 
#endif
#ifndef Tcl_FSCopyFile
#define Tcl_FSCopyFile \
	(tclStubsPtr->tcl_FSCopyFile) 
#endif
#ifndef Tcl_FSCopyDirectory
#define Tcl_FSCopyDirectory \
	(tclStubsPtr->tcl_FSCopyDirectory) 
#endif
#ifndef Tcl_FSCreateDirectory
#define Tcl_FSCreateDirectory \
	(tclStubsPtr->tcl_FSCreateDirectory) 
#endif
#ifndef Tcl_FSDeleteFile
#define Tcl_FSDeleteFile \
	(tclStubsPtr->tcl_FSDeleteFile) 
#endif
#ifndef Tcl_FSLoadFile
#define Tcl_FSLoadFile \
	(tclStubsPtr->tcl_FSLoadFile) 
#endif
#ifndef Tcl_FSMatchInDirectory
#define Tcl_FSMatchInDirectory \
	(tclStubsPtr->tcl_FSMatchInDirectory) 
#endif
#ifndef Tcl_FSLink
#define Tcl_FSLink \
	(tclStubsPtr->tcl_FSLink) 
#endif
#ifndef Tcl_FSRemoveDirectory
#define Tcl_FSRemoveDirectory \
	(tclStubsPtr->tcl_FSRemoveDirectory) 
#endif
#ifndef Tcl_FSRenameFile
#define Tcl_FSRenameFile \
	(tclStubsPtr->tcl_FSRenameFile) 
#endif
#ifndef Tcl_FSLstat
#define Tcl_FSLstat \
	(tclStubsPtr->tcl_FSLstat) 
#endif
#ifndef Tcl_FSUtime
#define Tcl_FSUtime \
	(tclStubsPtr->tcl_FSUtime) 
#endif
#ifndef Tcl_FSFileAttrsGet
#define Tcl_FSFileAttrsGet \
	(tclStubsPtr->tcl_FSFileAttrsGet) 
#endif
#ifndef Tcl_FSFileAttrsSet
#define Tcl_FSFileAttrsSet \
	(tclStubsPtr->tcl_FSFileAttrsSet) 
#endif
#ifndef Tcl_FSFileAttrStrings
#define Tcl_FSFileAttrStrings \
	(tclStubsPtr->tcl_FSFileAttrStrings) 
#endif
#ifndef Tcl_FSStat
#define Tcl_FSStat \
	(tclStubsPtr->tcl_FSStat) 
#endif
#ifndef Tcl_FSAccess
#define Tcl_FSAccess \
	(tclStubsPtr->tcl_FSAccess) 
#endif
#ifndef Tcl_FSOpenFileChannel
#define Tcl_FSOpenFileChannel \
	(tclStubsPtr->tcl_FSOpenFileChannel) 
#endif
#ifndef Tcl_FSGetCwd
#define Tcl_FSGetCwd \
	(tclStubsPtr->tcl_FSGetCwd) 
#endif
#ifndef Tcl_FSChdir
#define Tcl_FSChdir \
	(tclStubsPtr->tcl_FSChdir) 
#endif
#ifndef Tcl_FSConvertToPathType
#define Tcl_FSConvertToPathType \
	(tclStubsPtr->tcl_FSConvertToPathType) 
#endif
#ifndef Tcl_FSJoinPath
#define Tcl_FSJoinPath \
	(tclStubsPtr->tcl_FSJoinPath) 
#endif
#ifndef Tcl_FSSplitPath
#define Tcl_FSSplitPath \
	(tclStubsPtr->tcl_FSSplitPath) 
#endif
#ifndef Tcl_FSEqualPaths
#define Tcl_FSEqualPaths \
	(tclStubsPtr->tcl_FSEqualPaths) 
#endif
#ifndef Tcl_FSGetNormalizedPath
#define Tcl_FSGetNormalizedPath \
	(tclStubsPtr->tcl_FSGetNormalizedPath) 
#endif
#ifndef Tcl_FSJoinToPath
#define Tcl_FSJoinToPath \
	(tclStubsPtr->tcl_FSJoinToPath) 
#endif
#ifndef Tcl_FSGetInternalRep
#define Tcl_FSGetInternalRep \
	(tclStubsPtr->tcl_FSGetInternalRep) 
#endif
#ifndef Tcl_FSGetTranslatedPath
#define Tcl_FSGetTranslatedPath \
	(tclStubsPtr->tcl_FSGetTranslatedPath) 
#endif
#ifndef Tcl_FSEvalFile
#define Tcl_FSEvalFile \
	(tclStubsPtr->tcl_FSEvalFile) 
#endif
#ifndef Tcl_FSNewNativePath
#define Tcl_FSNewNativePath \
	(tclStubsPtr->tcl_FSNewNativePath) 
#endif
#ifndef Tcl_FSGetNativePath
#define Tcl_FSGetNativePath \
	(tclStubsPtr->tcl_FSGetNativePath) 
#endif
#ifndef Tcl_FSFileSystemInfo
#define Tcl_FSFileSystemInfo \
	(tclStubsPtr->tcl_FSFileSystemInfo) 
#endif
#ifndef Tcl_FSPathSeparator
#define Tcl_FSPathSeparator \
	(tclStubsPtr->tcl_FSPathSeparator) 
#endif
#ifndef Tcl_FSListVolumes
#define Tcl_FSListVolumes \
	(tclStubsPtr->tcl_FSListVolumes) 
#endif
#ifndef Tcl_FSRegister
#define Tcl_FSRegister \
	(tclStubsPtr->tcl_FSRegister) 
#endif
#ifndef Tcl_FSUnregister
#define Tcl_FSUnregister \
	(tclStubsPtr->tcl_FSUnregister) 
#endif
#ifndef Tcl_FSData
#define Tcl_FSData \
	(tclStubsPtr->tcl_FSData) 
#endif
#ifndef Tcl_FSGetTranslatedStringPath
#define Tcl_FSGetTranslatedStringPath \
	(tclStubsPtr->tcl_FSGetTranslatedStringPath) 
#endif
#ifndef Tcl_FSGetFileSystemForPath
#define Tcl_FSGetFileSystemForPath \
	(tclStubsPtr->tcl_FSGetFileSystemForPath) 
#endif
#ifndef Tcl_FSGetPathType
#define Tcl_FSGetPathType \
	(tclStubsPtr->tcl_FSGetPathType) 
#endif
#ifndef Tcl_OutputBuffered
#define Tcl_OutputBuffered \
	(tclStubsPtr->tcl_OutputBuffered) 
#endif
#ifndef Tcl_FSMountsChanged
#define Tcl_FSMountsChanged \
	(tclStubsPtr->tcl_FSMountsChanged) 
#endif
#ifndef Tcl_EvalTokensStandard
#define Tcl_EvalTokensStandard \
	(tclStubsPtr->tcl_EvalTokensStandard) 
#endif
#ifndef Tcl_GetTime
#define Tcl_GetTime \
	(tclStubsPtr->tcl_GetTime) 
#endif
#ifndef Tcl_CreateObjTrace
#define Tcl_CreateObjTrace \
	(tclStubsPtr->tcl_CreateObjTrace) 
#endif
#ifndef Tcl_GetCommandInfoFromToken
#define Tcl_GetCommandInfoFromToken \
	(tclStubsPtr->tcl_GetCommandInfoFromToken) 
#endif
#ifndef Tcl_SetCommandInfoFromToken
#define Tcl_SetCommandInfoFromToken \
	(tclStubsPtr->tcl_SetCommandInfoFromToken) 
#endif
#ifndef Tcl_DbNewWideIntObj
#define Tcl_DbNewWideIntObj \
	(tclStubsPtr->tcl_DbNewWideIntObj) 
#endif
#ifndef Tcl_GetWideIntFromObj
#define Tcl_GetWideIntFromObj \
	(tclStubsPtr->tcl_GetWideIntFromObj) 
#endif
#ifndef Tcl_NewWideIntObj
#define Tcl_NewWideIntObj \
	(tclStubsPtr->tcl_NewWideIntObj) 
#endif
#ifndef Tcl_SetWideIntObj
#define Tcl_SetWideIntObj \
	(tclStubsPtr->tcl_SetWideIntObj) 
#endif
#ifndef Tcl_AllocStatBuf
#define Tcl_AllocStatBuf \
	(tclStubsPtr->tcl_AllocStatBuf) 
#endif
#ifndef Tcl_Seek
#define Tcl_Seek \
	(tclStubsPtr->tcl_Seek) 
#endif
#ifndef Tcl_Tell
#define Tcl_Tell \
	(tclStubsPtr->tcl_Tell) 
#endif
#ifndef Tcl_ChannelWideSeekProc
#define Tcl_ChannelWideSeekProc \
	(tclStubsPtr->tcl_ChannelWideSeekProc) 
#endif
#ifndef Tcl_DictObjPut
#define Tcl_DictObjPut \
	(tclStubsPtr->tcl_DictObjPut) 
#endif
#ifndef Tcl_DictObjGet
#define Tcl_DictObjGet \
	(tclStubsPtr->tcl_DictObjGet) 
#endif
#ifndef Tcl_DictObjRemove
#define Tcl_DictObjRemove \
	(tclStubsPtr->tcl_DictObjRemove) 
#endif
#ifndef Tcl_DictObjSize
#define Tcl_DictObjSize \
	(tclStubsPtr->tcl_DictObjSize) 
#endif
#ifndef Tcl_DictObjFirst
#define Tcl_DictObjFirst \
	(tclStubsPtr->tcl_DictObjFirst) 
#endif
#ifndef Tcl_DictObjNext
#define Tcl_DictObjNext \
	(tclStubsPtr->tcl_DictObjNext) 
#endif
#ifndef Tcl_DictObjDone
#define Tcl_DictObjDone \
	(tclStubsPtr->tcl_DictObjDone) 
#endif
#ifndef Tcl_DictObjPutKeyList
#define Tcl_DictObjPutKeyList \
	(tclStubsPtr->tcl_DictObjPutKeyList) 
#endif
#ifndef Tcl_DictObjRemoveKeyList
#define Tcl_DictObjRemoveKeyList \
	(tclStubsPtr->tcl_DictObjRemoveKeyList) 
#endif
#ifndef Tcl_NewDictObj
#define Tcl_NewDictObj \
	(tclStubsPtr->tcl_NewDictObj) 
#endif
#ifndef Tcl_DbNewDictObj
#define Tcl_DbNewDictObj \
	(tclStubsPtr->tcl_DbNewDictObj) 
#endif
#ifndef Tcl_RegisterConfig
#define Tcl_RegisterConfig \
	(tclStubsPtr->tcl_RegisterConfig) 
#endif
#ifndef Tcl_CreateNamespace
#define Tcl_CreateNamespace \
	(tclStubsPtr->tcl_CreateNamespace) 
#endif
#ifndef Tcl_DeleteNamespace
#define Tcl_DeleteNamespace \
	(tclStubsPtr->tcl_DeleteNamespace) 
#endif
#ifndef Tcl_AppendExportList
#define Tcl_AppendExportList \
	(tclStubsPtr->tcl_AppendExportList) 
#endif
#ifndef Tcl_Export
#define Tcl_Export \
	(tclStubsPtr->tcl_Export) 
#endif
#ifndef Tcl_Import
#define Tcl_Import \
	(tclStubsPtr->tcl_Import) 
#endif
#ifndef Tcl_ForgetImport
#define Tcl_ForgetImport \
	(tclStubsPtr->tcl_ForgetImport) 
#endif
#ifndef Tcl_GetCurrentNamespace
#define Tcl_GetCurrentNamespace \
	(tclStubsPtr->tcl_GetCurrentNamespace) 
#endif
#ifndef Tcl_GetGlobalNamespace
#define Tcl_GetGlobalNamespace \
	(tclStubsPtr->tcl_GetGlobalNamespace) 
#endif
#ifndef Tcl_FindNamespace
#define Tcl_FindNamespace \
	(tclStubsPtr->tcl_FindNamespace) 
#endif
#ifndef Tcl_FindCommand
#define Tcl_FindCommand \
	(tclStubsPtr->tcl_FindCommand) 
#endif
#ifndef Tcl_GetCommandFromObj
#define Tcl_GetCommandFromObj \
	(tclStubsPtr->tcl_GetCommandFromObj) 
#endif
#ifndef Tcl_GetCommandFullName
#define Tcl_GetCommandFullName \
	(tclStubsPtr->tcl_GetCommandFullName) 
#endif
#ifndef Tcl_FSEvalFileEx
#define Tcl_FSEvalFileEx \
	(tclStubsPtr->tcl_FSEvalFileEx) 
#endif
#ifndef Tcl_SetExitProc
#define Tcl_SetExitProc \
	(tclStubsPtr->tcl_SetExitProc) 
#endif
#ifndef Tcl_LimitAddHandler
#define Tcl_LimitAddHandler \
	(tclStubsPtr->tcl_LimitAddHandler) 
#endif
#ifndef Tcl_LimitRemoveHandler
#define Tcl_LimitRemoveHandler \
	(tclStubsPtr->tcl_LimitRemoveHandler) 
#endif
#ifndef Tcl_LimitReady
#define Tcl_LimitReady \
	(tclStubsPtr->tcl_LimitReady) 
#endif
#ifndef Tcl_LimitCheck
#define Tcl_LimitCheck \
	(tclStubsPtr->tcl_LimitCheck) 
#endif
#ifndef Tcl_LimitExceeded
#define Tcl_LimitExceeded \
	(tclStubsPtr->tcl_LimitExceeded) 
#endif
#ifndef Tcl_LimitSetCommands
#define Tcl_LimitSetCommands \
	(tclStubsPtr->tcl_LimitSetCommands) 
#endif
#ifndef Tcl_LimitSetTime
#define Tcl_LimitSetTime \
	(tclStubsPtr->tcl_LimitSetTime) 
#endif
#ifndef Tcl_LimitSetGranularity
#define Tcl_LimitSetGranularity \
	(tclStubsPtr->tcl_LimitSetGranularity) 
#endif
#ifndef Tcl_LimitTypeEnabled
#define Tcl_LimitTypeEnabled \
	(tclStubsPtr->tcl_LimitTypeEnabled) 
#endif
#ifndef Tcl_LimitTypeExceeded
#define Tcl_LimitTypeExceeded \
	(tclStubsPtr->tcl_LimitTypeExceeded) 
#endif
#ifndef Tcl_LimitTypeSet
#define Tcl_LimitTypeSet \
	(tclStubsPtr->tcl_LimitTypeSet) 
#endif
#ifndef Tcl_LimitTypeReset
#define Tcl_LimitTypeReset \
	(tclStubsPtr->tcl_LimitTypeReset) 
#endif
#ifndef Tcl_LimitGetCommands
#define Tcl_LimitGetCommands \
	(tclStubsPtr->tcl_LimitGetCommands) 
#endif
#ifndef Tcl_LimitGetTime
#define Tcl_LimitGetTime \
	(tclStubsPtr->tcl_LimitGetTime) 
#endif
#ifndef Tcl_LimitGetGranularity
#define Tcl_LimitGetGranularity \
	(tclStubsPtr->tcl_LimitGetGranularity) 
#endif
#ifndef Tcl_SaveInterpState
#define Tcl_SaveInterpState \
	(tclStubsPtr->tcl_SaveInterpState) 
#endif
#ifndef Tcl_RestoreInterpState
#define Tcl_RestoreInterpState \
	(tclStubsPtr->tcl_RestoreInterpState) 
#endif
#ifndef Tcl_DiscardInterpState
#define Tcl_DiscardInterpState \
	(tclStubsPtr->tcl_DiscardInterpState) 
#endif
#ifndef Tcl_SetReturnOptions
#define Tcl_SetReturnOptions \
	(tclStubsPtr->tcl_SetReturnOptions) 
#endif
#ifndef Tcl_GetReturnOptions
#define Tcl_GetReturnOptions \
	(tclStubsPtr->tcl_GetReturnOptions) 
#endif
#ifndef Tcl_IsEnsemble
#define Tcl_IsEnsemble \
	(tclStubsPtr->tcl_IsEnsemble) 
#endif
#ifndef Tcl_CreateEnsemble
#define Tcl_CreateEnsemble \
	(tclStubsPtr->tcl_CreateEnsemble) 
#endif
#ifndef Tcl_FindEnsemble
#define Tcl_FindEnsemble \
	(tclStubsPtr->tcl_FindEnsemble) 
#endif
#ifndef Tcl_SetEnsembleSubcommandList
#define Tcl_SetEnsembleSubcommandList \
	(tclStubsPtr->tcl_SetEnsembleSubcommandList) 
#endif
#ifndef Tcl_SetEnsembleMappingDict
#define Tcl_SetEnsembleMappingDict \
	(tclStubsPtr->tcl_SetEnsembleMappingDict) 
#endif
#ifndef Tcl_SetEnsembleUnknownHandler
#define Tcl_SetEnsembleUnknownHandler \
	(tclStubsPtr->tcl_SetEnsembleUnknownHandler) 
#endif
#ifndef Tcl_SetEnsembleFlags
#define Tcl_SetEnsembleFlags \
	(tclStubsPtr->tcl_SetEnsembleFlags) 
#endif
#ifndef Tcl_GetEnsembleSubcommandList
#define Tcl_GetEnsembleSubcommandList \
	(tclStubsPtr->tcl_GetEnsembleSubcommandList) 
#endif
#ifndef Tcl_GetEnsembleMappingDict
#define Tcl_GetEnsembleMappingDict \
	(tclStubsPtr->tcl_GetEnsembleMappingDict) 
#endif
#ifndef Tcl_GetEnsembleUnknownHandler
#define Tcl_GetEnsembleUnknownHandler \
	(tclStubsPtr->tcl_GetEnsembleUnknownHandler) 
#endif
#ifndef Tcl_GetEnsembleFlags
#define Tcl_GetEnsembleFlags \
	(tclStubsPtr->tcl_GetEnsembleFlags) 
#endif
#ifndef Tcl_GetEnsembleNamespace
#define Tcl_GetEnsembleNamespace \
	(tclStubsPtr->tcl_GetEnsembleNamespace) 
#endif
#ifndef Tcl_SetTimeProc
#define Tcl_SetTimeProc \
	(tclStubsPtr->tcl_SetTimeProc) 
#endif
#ifndef Tcl_QueryTimeProc
#define Tcl_QueryTimeProc \
	(tclStubsPtr->tcl_QueryTimeProc) 
#endif
#ifndef Tcl_ChannelThreadActionProc
#define Tcl_ChannelThreadActionProc \
	(tclStubsPtr->tcl_ChannelThreadActionProc) 
#endif
#ifndef Tcl_NewBignumObj
#define Tcl_NewBignumObj \
	(tclStubsPtr->tcl_NewBignumObj) 
#endif
#ifndef Tcl_DbNewBignumObj
#define Tcl_DbNewBignumObj \
	(tclStubsPtr->tcl_DbNewBignumObj) 
#endif
#ifndef Tcl_SetBignumObj
#define Tcl_SetBignumObj \
	(tclStubsPtr->tcl_SetBignumObj) 
#endif
#ifndef Tcl_GetBignumFromObj
#define Tcl_GetBignumFromObj \
	(tclStubsPtr->tcl_GetBignumFromObj) 
#endif
#ifndef Tcl_TakeBignumFromObj
#define Tcl_TakeBignumFromObj \
	(tclStubsPtr->tcl_TakeBignumFromObj) 
#endif
#ifndef Tcl_TruncateChannel
#define Tcl_TruncateChannel \
	(tclStubsPtr->tcl_TruncateChannel) 
#endif
#ifndef Tcl_ChannelTruncateProc
#define Tcl_ChannelTruncateProc \
	(tclStubsPtr->tcl_ChannelTruncateProc) 
#endif
#ifndef Tcl_SetChannelErrorInterp
#define Tcl_SetChannelErrorInterp \
	(tclStubsPtr->tcl_SetChannelErrorInterp) 
#endif
#ifndef Tcl_GetChannelErrorInterp
#define Tcl_GetChannelErrorInterp \
	(tclStubsPtr->tcl_GetChannelErrorInterp) 
#endif
#ifndef Tcl_SetChannelError
#define Tcl_SetChannelError \
	(tclStubsPtr->tcl_SetChannelError) 
#endif
#ifndef Tcl_GetChannelError
#define Tcl_GetChannelError \
	(tclStubsPtr->tcl_GetChannelError) 
#endif
#ifndef Tcl_InitBignumFromDouble
#define Tcl_InitBignumFromDouble \
	(tclStubsPtr->tcl_InitBignumFromDouble) 
#endif
#ifndef Tcl_GetNamespaceUnknownHandler
#define Tcl_GetNamespaceUnknownHandler \
	(tclStubsPtr->tcl_GetNamespaceUnknownHandler) 
#endif
#ifndef Tcl_SetNamespaceUnknownHandler
#define Tcl_SetNamespaceUnknownHandler \
	(tclStubsPtr->tcl_SetNamespaceUnknownHandler) 
#endif
#ifndef Tcl_GetEncodingFromObj
#define Tcl_GetEncodingFromObj \
	(tclStubsPtr->tcl_GetEncodingFromObj) 
#endif
#ifndef Tcl_GetEncodingSearchPath
#define Tcl_GetEncodingSearchPath \
	(tclStubsPtr->tcl_GetEncodingSearchPath) 
#endif
#ifndef Tcl_SetEncodingSearchPath
#define Tcl_SetEncodingSearchPath \
	(tclStubsPtr->tcl_SetEncodingSearchPath) 
#endif
#ifndef Tcl_GetEncodingNameFromEnvironment
#define Tcl_GetEncodingNameFromEnvironment \
	(tclStubsPtr->tcl_GetEncodingNameFromEnvironment) 
#endif
#ifndef Tcl_PkgRequireProc
#define Tcl_PkgRequireProc \
	(tclStubsPtr->tcl_PkgRequireProc) 
#endif
#ifndef Tcl_AppendObjToErrorInfo
#define Tcl_AppendObjToErrorInfo \
	(tclStubsPtr->tcl_AppendObjToErrorInfo) 
#endif
#ifndef Tcl_AppendLimitedToObj
#define Tcl_AppendLimitedToObj \
	(tclStubsPtr->tcl_AppendLimitedToObj) 
#endif
#ifndef Tcl_Format
#define Tcl_Format \
	(tclStubsPtr->tcl_Format) 
#endif
#ifndef Tcl_AppendFormatToObj
#define Tcl_AppendFormatToObj \
	(tclStubsPtr->tcl_AppendFormatToObj) 
#endif
#ifndef Tcl_ObjPrintf
#define Tcl_ObjPrintf \
	(tclStubsPtr->tcl_ObjPrintf) 
#endif
#ifndef Tcl_AppendPrintfToObj
#define Tcl_AppendPrintfToObj \
	(tclStubsPtr->tcl_AppendPrintfToObj) 
#endif


















































#ifndef TclUnusedStubEntry
#define TclUnusedStubEntry \
	(tclStubsPtr->tclUnusedStubEntry) 
#endif





























#ifndef Tcl_outputCmdFrame
#define Tcl_outputCmdFrame \
	(tclStubsPtr->tcl_outputCmdFrame) 
#endif



















#ifndef Tcl_GetSourceInfo
#define Tcl_GetSourceInfo \
	(tclStubsPtr->tcl_GetSourceInfo) 
#endif

#endif 



#undef TclUnusedStubEntry

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT

#undef Tcl_PkgPresent
#define Tcl_PkgPresent(interp, name, version, exact) \
	Tcl_PkgPresentEx(interp, name, version, exact, NULL)
#undef Tcl_PkgProvide
#define Tcl_PkgProvide(interp, name, version) \
	Tcl_PkgProvideEx(interp, name, version, NULL)
#undef Tcl_PkgRequire
#define Tcl_PkgRequire(interp, name, version, exact) \
	Tcl_PkgRequireEx(interp, name, version, exact, NULL)
#undef Tcl_GetIndexFromObj
#define Tcl_GetIndexFromObj(interp, objPtr, tablePtr, msg, flags, indexPtr) \
	Tcl_GetIndexFromObjStruct(interp, objPtr, tablePtr, \
	sizeof(char *), msg, flags, indexPtr)
#undef Tcl_NewBooleanObj
#define Tcl_NewBooleanObj(boolValue) \
	Tcl_NewIntObj((boolValue)!=0)
#undef Tcl_DbNewBooleanObj
#define Tcl_DbNewBooleanObj(boolValue, file, line) \
	Tcl_DbNewLongObj((boolValue)!=0, file, line)
#undef Tcl_SetBooleanObj
#define Tcl_SetBooleanObj(objPtr, boolValue) \
	Tcl_SetIntObj((objPtr), (boolValue)!=0)
#undef Tcl_SetVar
#define Tcl_SetVar(interp, varName, newValue, flags) \
	Tcl_SetVar2(interp, varName, NULL, newValue, flags)
#undef Tcl_UnsetVar
#define Tcl_UnsetVar(interp, varName, flags) \
	Tcl_UnsetVar2(interp, varName, NULL, flags)
#undef Tcl_GetVar
#define Tcl_GetVar(interp, varName, flags) \
	Tcl_GetVar2(interp, varName, NULL, flags)
#undef Tcl_TraceVar
#define Tcl_TraceVar(interp, varName, flags, proc, clientData) \
	Tcl_TraceVar2(interp, varName, NULL, flags, proc, clientData)
#undef Tcl_UntraceVar
#define Tcl_UntraceVar(interp, varName, flags, proc, clientData) \
	Tcl_UntraceVar2(interp, varName, NULL, flags, proc, clientData)
#undef Tcl_VarTraceInfo
#define Tcl_VarTraceInfo(interp, varName, flags, proc, prevClientData) \
	Tcl_VarTraceInfo2(interp, varName, NULL, flags, proc, prevClientData)
#undef Tcl_UpVar
#define Tcl_UpVar(interp, frameName, varName, localName, flags) \
	Tcl_UpVar2(interp, frameName, varName, NULL, localName, flags)

#if defined(USE_TCL_STUBS) && !defined(USE_TCL_STUB_PROCS)
#   if defined(__CYGWIN__) && defined(TCL_WIDE_INT_IS_LONG)

#	undef Tcl_DbNewLongObj
#	undef Tcl_GetLongFromObj
#	undef Tcl_NewLongObj
#	undef Tcl_SetLongObj
#	undef Tcl_ExprLong
#	undef Tcl_ExprLongObj
#	undef Tcl_UniCharNcmp
#	undef Tcl_UtfNcmp
#	undef Tcl_UtfNcasecmp
#	undef Tcl_UniCharNcasecmp
#	define Tcl_DbNewLongObj ((Tcl_Obj*(*)(long,const char*,int))Tcl_DbNewWideIntObj)
#	define Tcl_GetLongFromObj ((int(*)(Tcl_Interp*,Tcl_Obj*,long*))Tcl_GetWideIntFromObj)
#	define Tcl_NewLongObj ((Tcl_Obj*(*)(long))Tcl_NewWideIntObj)
#	define Tcl_SetLongObj ((void(*)(Tcl_Obj*,long))Tcl_SetWideIntObj)
#	define Tcl_ExprLong TclExprLong
	static inline int TclExprLong(Tcl_Interp *interp, const char *string, long *ptr){
	    int intValue;
	    int result = tclStubsPtr->tcl_ExprLong(interp, string, (long *)&intValue);
	    if (result == TCL_OK) *ptr = (long)intValue;
	    return result;
	}
#	define Tcl_ExprLongObj TclExprLongObj
	static inline int TclExprLongObj(Tcl_Interp *interp, Tcl_Obj *obj, long *ptr){
	    int intValue;
	    int result = tclStubsPtr->tcl_ExprLongObj(interp, obj, (long *)&intValue);
	    if (result == TCL_OK) *ptr = (long)intValue;
	    return result;
	}
#	define Tcl_UniCharNcmp(ucs,uct,n) \
		((int(*)(const Tcl_UniChar*,const Tcl_UniChar*,unsigned int))tclStubsPtr->tcl_UniCharNcmp)(ucs,uct,(unsigned int)(n))
#	define Tcl_UtfNcmp(s1,s2,n) \
		((int(*)(const char*,const char*,unsigned int))tclStubsPtr->tcl_UtfNcmp)(s1,s2,(unsigned int)(n))
#	define Tcl_UtfNcasecmp(s1,s2,n) \
		((int(*)(const char*,const char*,unsigned int))tclStubsPtr->tcl_UtfNcasecmp)(s1,s2,(unsigned int)(n))
#	define Tcl_UniCharNcasecmp(ucs,uct,n) \
		((int(*)(const Tcl_UniChar*,const Tcl_UniChar*,unsigned int))tclStubsPtr->tcl_UniCharNcasecmp)(ucs,uct,(unsigned int)(n))
#   endif
#endif



#undef Tcl_EvalObj
#define Tcl_EvalObj(interp,objPtr) \
    Tcl_EvalObjEx((interp),(objPtr),0)
#undef Tcl_GlobalEvalObj
#define Tcl_GlobalEvalObj(interp,objPtr) \
    Tcl_EvalObjEx((interp),(objPtr),TCL_EVAL_GLOBAL)

#endif 
