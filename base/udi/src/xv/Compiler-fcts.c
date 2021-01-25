/**********************************************************************************
***********************************************************************************
**
**   File        : Compiler-fcts.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 28.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. Handles all compiler unit functions calls.
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

/*********************************************************************************/
/********** INTERNAL PROGRAM INFORMATIONS FOR THIS MODULE ************************/
/*********************************************************************************/

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dbug.h"

#include "marco-options.h"
#include "Compiler-fcts.h"
#include "Properties-fcts.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct FileNamesTag { /* Infos about temp files **************************/
 char *name;
 struct FileNamesTag *next;
} FileNames; /********************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define CI_OP_NONE	0
#define CH_OP_PP2RED	1
#define CH_OP_PP2PP	2

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

extern int remove(const char *);             /* Defined in stdlib (normally) ;-) */

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static FileNames *FN_First=NULL;
static FileNames *FN_Last=NULL;

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static CompileInfo *Mk_CompileHandle(char *infilename,char *outfilename,int type)
{ /* *****************************************************************************/
 CompileInfo *CI=(CompileInfo *)malloc(sizeof(CompileInfo));
 DBUG_ENTER("Mk_CompileHandle");

 if (CI==NULL) DBUG_RETURN(CI);

 CI->infilename=NULL;
 CI->outfilename=NULL;
 CI->last_operation=CI_OP_NONE;
 CI->error_number=0;
 CI->errors=NULL;

 CI->infilename=(char *)strdup(infilename);                      /* Copyfilename */

 if (outfilename==NULL) {
  CI->free_outfilename = 0; 
  switch (type) {
   case CH_OP_PP2RED : 
    CI->outfilename=Mk_Tmpfilename(RED_EXTENDER);
    break;
   case CH_OP_PP2PP :
    CI->outfilename=Mk_Tmpfilename(PP_EXTENDER);
    break;
   default :
    CI->outfilename=Mk_Tmpfilename(PP_EXTENDER);
    break;
  }
 } else {
  CI->free_outfilename = 1; 
  CI->outfilename=(char *)strdup(outfilename);
 }
 
 CI->last_operation=type;
 CI->okay=0;

 DBUG_RETURN(CI);                                                    /* All done */
} /*******************************************************************************/

static void LoadErrorFile(CompileInfo *ci)
{ /* Load errorfile to memory structure ******************************************/
 FILE *err_fh=fopen(OPTIONS.errfilename,"r");
 DBUG_ENTER("LoadErrorFile");

 if (err_fh) {                                      /* Okay to open error file ? */
  static char Filename[256];
  static int  LineNumber;
  static char ErrorText[256];
  ErrorStruct *hlp=NULL;
  ErrorStruct *prev=NULL;
  int result;
  ci->error_number=0;
  ci->errors=NULL;
  while (feof(err_fh)==0) {
   result=fscanf(err_fh,"%[^:]:%d: %[^\n]\n",Filename,&LineNumber,ErrorText);
   if ((result!=EOF)&&(result!=0)) {
    ci->error_number++;
    hlp=(ErrorStruct *)malloc(sizeof(ErrorStruct));
    if (ci->error_number==1) ci->errors=hlp;                      /* First error */
    hlp->EI.number=ci->error_number;
    hlp->EI.line=LineNumber;
    hlp->EI.col=0;
    hlp->EI.text=(char *)strdup(ErrorText);
    hlp->next=NULL;
    hlp->prev=prev;
    if (prev) prev->next=hlp;
    prev=hlp;
   }
  }
  ci->last_err=prev;
  ci->first_err=ci->act_err=ci->errors;
  fclose(err_fh);
 }

 cf_RemoveTempfile(OPTIONS.errfilename);

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static CompileInfo *Do_Action(CompileInfo *ci)
{ /* *****************************************************************************/
 DBUG_ENTER("Do_Action");

 SetCompilerOptions2Default();             /* Set compilerunit tp default values */

 OPTIONS.errfilename=Mk_Tmpfilename(".err");

 switch (ci->last_operation) {
  case CH_OP_PP2RED :
   OPTIONS.filename=(char *)strdup(ci->infilename);            /* Store filename */
   OPTIONS.fileformat=FF_SOURCE;                         /* Fileformat is source */
   OPTIONS.AllocStack=1;                                    /* Output is needed */
   OPTIONS.outputformat=OF_STACKELEMS;
   OPTIONS.outfilename=(char *)strdup(ci->outfilename);        /* Store filename */
   break;
  case CH_OP_PP2PP :
   OPTIONS.filename=(char *)strdup(ci->infilename);            /* Store filename */
   OPTIONS.fileformat=FF_SOURCE;                         /* Fileformat is source */
   OPTIONS.AllocStack=1;                                    /* Output is needed */
   OPTIONS.outputformat=OF_SRC;
   OPTIONS.outfilename=(char *)strdup(ci->outfilename);        /* Store filename */
   break;
  default :
   CleanUpCompilerUnit(0);
   Delete_CompileInfo(ci);
   DBUG_RETURN(NULL);
 }

 if (InitCompilerUnit()) { /* Init translation unit ? ****************************/
  if (CompileLanguageFile()) {                         /* Loading/Parsing OKAY ? */
   ReduceProgramm();                     /* Start reduction and generate results */
   cf_RemoveTempfile(OPTIONS.errfilename);
   ci->okay=1;
  } else {                                      /* End of if CompileLanguageFile */
   LoadErrorFile(ci);
  }
 }                                                 /* End of if InitCompilerUnit */
 CleanUpCompilerUnit(0); /* Free resources and tidy up modules *******************/

 DBUG_RETURN(ci);
} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void cf_RemoveTempfile(char *filename)
{ /* Remove file *****************************************************************/
 FileNames *tmp=FN_First;
 FileNames *prev=FN_First;
 
 DBUG_ENTER("cf_RemoveTempfile");

 while (tmp) {

  if (strcmp(filename,tmp->name)==0) {                           /* Name found ? */
   DBUG_PRINT("Tempfiles",("Name <%s>",tmp->name));
   free(tmp->name);
   if (tmp==FN_First) {
    FN_First=tmp->next;
    if (tmp==FN_Last) {
     FN_Last=NULL;
    }
   } else {
    prev->next=tmp->next;
    if (tmp==FN_Last) {
     FN_Last=prev;
    }
   }
   free(tmp);

   remove(filename);

   break;
  }
  prev=tmp;
  tmp=tmp->next;

 }


 DBUG_VOID_RETURN;
} /*******************************************************************************/

void cf_RemoveAllTempFiles(void)
{ /* Removes all tempfiles *******************************************************/
 FileNames *tmp=FN_First;
 FileNames *hlp;
 DBUG_ENTER("cf_RemoveAllTempFiles");

 while (tmp) {
  DBUG_PRINT("Tempfiles",("Name <%s>",tmp->name));
  remove(tmp->name);                              /* Remove file from filesystem */
  free(tmp->name);                                           /* Free name memory */
  hlp=tmp;                                       /* Store pointer to listelement */
  tmp=tmp->next;                              /* Set pointer to next listelement */
  free(hlp);                                          /* Free listelement struct */
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

void cf_StoreTmpFileName(char *filename)
{ /* Stores filename as a named temporaere file **********************************/
 FileNames *hlp=(FileNames*)malloc(sizeof(FileNames));
 DBUG_ENTER("cf_StoreTmpFileName");

 hlp->name=(char *)strdup(filename);
 hlp->next=NULL;

 DBUG_PRINT("Tempfiles",("Name <%s> FN_LAST=%p",filename,FN_Last));

 if (FN_Last!=NULL) {
  FN_Last->next=hlp;
  FN_Last=hlp;
 } else {
  FN_Last=FN_First=hlp;
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

char *Mk_Tmpfilename(char *extension)
{ /* Makes a new tempfile with a given fileextension *****************************/
 static char BUFFER[256];
 DBUG_ENTER("Mk_Tmpfilename");

 sprintf(BUFFER,"%s%s",tempnam("/tmp","red_"),extension);
 cf_StoreTmpFileName(BUFFER);

 DBUG_RETURN((char *)strdup(BUFFER));                           /* Return a copy */
} /*******************************************************************************/

void Delete_CompileInfo(CompileInfo *ci)
{ /* Delete (frees) memory allocated by compile infos ****************************/
 DBUG_ENTER("Delete_CompileInfo");

 if (ci->error_number>0) {
  ErrorStruct *hlp=ci->errors;
  ErrorStruct *tmp;
  while (hlp) {
   if (hlp->EI.text) free(hlp->EI.text);
   tmp=hlp;
   hlp=hlp->next;
   free(tmp);
  }
 }

 if (ci->infilename) free(ci->infilename);

 if (ci->free_outfilename) {
  if (ci->outfilename) free(ci->outfilename);
 }

 free(ci);

 DBUG_VOID_RETURN;
} /*******************************************************************************/

int reduce(STACKELEM *stack,int *top,PTR_UEBERGABE p_parms)
{ /* Actually just a dummy function **********************************************/
 DBUG_ENTER("reduce");

 DBUG_RETURN (1);
} /*******************************************************************************/

CompileInfo *cf_Transform_2_Redfile(char *filename)
{ /* *****************************************************************************/
 CompileInfo *ci=NULL;
 DBUG_ENTER("cf_Transform_2_Redfile");

 if ((ci=Mk_CompileHandle(filename,NULL,CH_OP_PP2RED))!=NULL) {
  DBUG_RETURN (Do_Action(ci));
 }
 DBUG_RETURN (ci);
} /*******************************************************************************/

CompileInfo *cf_FormatFile(char *filename)
{ /* *****************************************************************************/
 CompileInfo *ci=NULL;
 DBUG_ENTER("cf_FormatFile");

 if ((ci=Mk_CompileHandle(filename,NULL,CH_OP_PP2PP))!=NULL) {
  DBUG_RETURN (Do_Action(ci));
 }

 DBUG_RETURN (ci);
} /*******************************************************************************/

char *cf_GetRedFilename(CompileInfo *ci)
{ /* *****************************************************************************/
 DBUG_ENTER("cf_GetRedFilename");
 DBUG_RETURN (ci->outfilename);
} /*******************************************************************************/

/* Error functions ***************************************************************/

int cf_NumberOfErrors(CompileInfo *ci)
{ /* Returns the number of errors in last compile action *************************/
 DBUG_ENTER("cf_NumberOfErrors");
 DBUG_RETURN (ci->error_number);
} /*******************************************************************************/

ErrorInfo *cf_FirstError(CompileInfo *ci)
{ /* Returns pointer to first error structure ************************************/
 DBUG_ENTER("cf_FirstError");
 ci->act_err=ci->first_err;
 DBUG_RETURN (ci->act_err ? (&(ci->act_err->EI)) : NULL);
} /*******************************************************************************/

ErrorInfo *cf_PrevError(CompileInfo *ci)
{ /* Returns pointer to the previous error structure *****************************/
 DBUG_ENTER("cf_PrevError");
 if (ci->act_err) {
  ci->act_err=ci->act_err->prev;
 }
 DBUG_RETURN (ci->act_err ? (&(ci->act_err->EI)) : NULL);
} /*******************************************************************************/

ErrorInfo *cf_NextError(CompileInfo *ci)
{ /* Returns pointer to the next error structure *********************************/
 DBUG_ENTER("cf_NextError");
 if (ci->act_err) {
  ci->act_err=ci->act_err->next;
 }
 DBUG_RETURN (ci->act_err ? (&(ci->act_err->EI)) : NULL);
} /*******************************************************************************/

ErrorInfo *cf_LastError(CompileInfo *ci)
{ /* Returns pointer to the last error structure *********************************/
 DBUG_ENTER("cf_LastError");
 ci->act_err=ci->last_err;
 DBUG_RETURN (ci->act_err ? (&(ci->act_err->EI)) : NULL);
} /*******************************************************************************/
