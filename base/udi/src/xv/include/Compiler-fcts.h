/**********************************************************************************
***********************************************************************************
**
**   File        : Compiler-fcts.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 28.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : Compiler functionscalls
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_COMPFCTS_HEADER_
#define _XREDUMA_COMPFCTS_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct ErrorInfoTag { /* Contains information to syntax errors **/
 int number; /* Error number */
 int line; /* Linenumber of error occurenz */
 int col; /* Column of error (if available) */
 char *text; /* Errormessage */
} ErrorInfo; /******/

typedef struct ErrorStructTag { /* Contains information about errors **/
 ErrorInfo EI; /* Error Informationstructure */
 struct ErrorStructTag *prev; /* Previous error message */
 struct ErrorStructTag *next; /* Next error message */
} ErrorStruct; /********/

typedef struct CompileInfoTag { /* Stores information refered to compile handle **/
 int okay; /* Signs if last operation was done or not */
 char *infilename; /* Pointer to last infilename (maybe tempfilename) */
 char *outfilename; /* Pointer to last outfilename (maybe tempfilename) */
 int  free_outfilename; /* Indicate, if the outfilename mem should be freed */
 int  last_operation; /* Identifier which operation was last used */
 int  error_number; /* Number of errors in last operation */
 ErrorStruct *errors; /* Array of error information structures */
 ErrorStruct *first_err; /* Pointer to actual error */
 ErrorStruct *act_err; /* Pointer to actual error */
 ErrorStruct *last_err; /* Pointer to actual error */
} CompileInfo; /***/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Removes all tempfiles **/
extern void cf_RemoveAllTempFiles(void);

/* Delete (frees) memory allocated by compile infos **/
extern void Delete_CompileInfo(CompileInfo *ci);

/* Makes a new tempfile with a given fileextension **/
extern char *Mk_Tmpfilename(char *extension);

/* Stores filename as a named temporaere file **/
extern void cf_StoreTmpFileName(char *filename);

/* Remove file **/
extern void cf_RemoveTempfile(char *filename);

extern CompileInfo *cf_Transform_2_Redfile(char *filename);

extern CompileInfo *cf_FormatFile(char *filename);

extern char *cf_GetRedFilename(CompileInfo *ci);

/* Returns the number of errors in last compile action **/
extern int cf_NumberOfErrors(CompileInfo *ci);

/* Returns pointer to first error structure **/
extern ErrorInfo *cf_FirstError(CompileInfo *ci);

/* Returns pointer to the previous error structure **/
extern ErrorInfo *cf_PrevError(CompileInfo *ci);

/* Returns pointer to the next error structure **/
ErrorInfo *cf_NextError(CompileInfo *ci);

/* Returns pointer to the last error structure **/
extern ErrorInfo *cf_LastError(CompileInfo *ci);

#endif
