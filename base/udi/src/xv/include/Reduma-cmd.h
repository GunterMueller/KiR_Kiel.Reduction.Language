/**********************************************************************************
***********************************************************************************
**
**   File        : Reduma-cmd.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 8.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : Reduma editor commands
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_REDCMDS_HEADER_
#define _XREDUMA_REDCMDS_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include "cedit.h"
#include "cfiles.h"
#include "cparm.h"
#include "cscred.h"
#include "Misc-fcts.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define REDUCE_UNDO	-1
#define REDUCE_ALL	0

#define EDITLIB_SIZE	(FN_ANZ*LENLIB)
#define EDITEXT_SIZE	(FN_ANZ*LENEXT)
#define REDPARMS_SIZE	sizeof(T_UEBERGABE)
#define PARAMETER_SIZE	(EDITLIB_SIZE+EDITEXT_SIZE+REDPARMS_SIZE)

#define rcmd_AdjustWindowSize() rcmd_ResizeWindow(-1,-1) /* Fit to window */

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

extern T_UEBERGABE     EditParms; /* Editor-parameter */
extern char            EditLibs[FN_ANZ][LENLIB]; /* Library FilePathes */
extern char            EditExt[FN_ANZ][LENEXT]; /* File extensions */
extern int             sedit_small; /* Syntax editor layout mode */

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

extern void rcmd_SetSyntaxEditorLayout(int result);

extern int rcmd_SearchReplace(SRValues *SV);

extern void rcmd_Reduce(int number);

extern void rcmd_Help(int section);

extern void rcmd_CopyToStack(void);
extern int rcmd_CopyFromStack(void);
extern void rcmd_CutToStack(void);

extern void rcmd_ClearProgram(void);
extern void rcmd_ClearInputline(void);

/* Cursor movement functions **/
extern void rcmd_CurUp(void);
extern void rcmd_CurDown(void);
extern void rcmd_CurLeft(void);
extern void rcmd_CurRight(void);
extern void rcmd_CurHome(void);
extern void rcmd_CurGoal(void);
extern void rcmd_GoHome(void);

/* Special function in red editor **/
extern void rcmd_F3Func(void);
extern void rcmd_F11Func(void);
extern void rcmd_DeleteExpr(void);

extern void rcmd_ResizeWindow(int x,int y);

extern void rcmd_GetEditorParameter(void);
extern void rcmd_SetEditorParameter(void);

extern void rcmd_Buffer_fcts(int number);

extern void rcmd_SetCursorTo(char *name);

extern void rcmd_LoadEdFile(char *name);
extern void rcmd_SaveEdFile(char *name);
extern void rcmd_LoadSetups(char *name);
extern void rcmd_SaveSetups(char *name);

extern void rcmd_LoadRedFile(char *name);
extern int rcmd_Change2DollarVar(char *name,int archive);

/* Call prettyprint cmd to pretty print file to name **/
extern void rcmd_SavePrettyPrint(char *name);

/* Defines Cursor key behaviour **/
extern void rcmd_CursorKeySetting(
 char up,char down,char left,char right,char home,char ret);

#endif
