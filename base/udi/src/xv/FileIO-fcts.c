/**********************************************************************************
***********************************************************************************
**
**   File        : FileIO-fcts.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 8.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This module implements all file io functions.
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

/*********************************************************************************/
/********** INTERNAL PROGRAM INFORMATIONS FOR THIS MODULE ************************/
/*********************************************************************************/

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dbug.h"

#include "XReduma.h"
#include "FileIO-fcts.h"
#include "Misc-fcts.h"
#include "Compiler-fcts.h"
#include "Reduma-cmd.h"
#include "Menu-fcts.h"
#include "TEditor-create.h"
#include "Menu-create.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define FILEFORMAT_UNKNOWN	0
#define FILEFORMAT_ED	1
#define FILEFORMAT_RED	2
#define FILEFORMAT_PP	3

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

#define HAS_EXTENSION(hlp,file,ext)	\
        ((hlp=strstr(file,ext)) && (strcmp(hlp,ext)==0))

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static int TestFileFormat(char *filename)
{ /* Test file : figure out load format **/
 char *pos;
 DBUG_ENTER("TestFileFormat");

 if (HAS_EXTENSION(pos,filename,ED_EXTENDER)) DBUG_RETURN(FILEFORMAT_ED);
 else if (HAS_EXTENSION(pos,filename,RED_EXTENDER)) DBUG_RETURN(FILEFORMAT_RED);
 else if (HAS_EXTENSION(pos,filename,PP_EXTENDER)) DBUG_RETURN(FILEFORMAT_PP);

 DBUG_RETURN(FILEFORMAT_UNKNOWN);
} /******/

static int ConvertFileToEd(char *filename)
{ /* **/
 static char BUFFER[512];
 DBUG_ENTER("ConvertFileToEd");

 if (mf_ConfirmActionWait(
     "Convert this file to editor file ?","Yes","No")) {
  char *str;
  strcpy(BUFFER,filename);
  str=strrchr(BUFFER,'.');
  if (str) *str='\0';
  rcmd_SaveEdFile(BUFFER);     
  strcat(BUFFER,ED_EXTENDER);
  /* mc_Set_Filename(BUFFER); */
  DBUG_RETURN (1);
 } else {
  mc_Set_Filename(filename);
  DBUG_RETURN (0);
 }
} /********/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

int ff_LoadFile(char *filename)
{ /* Load file depend on fileformat **/
 int result=1;
 DBUG_ENTER("ff_LoadFile");


 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL); /* Disable windowframe */

 switch (TestFileFormat(filename)) {

  case FILEFORMAT_RED :
   rcmd_LoadRedFile(filename);
   if (LOAD_MODE) ConvertFileToEd(filename);
   mc_Set_Filename(filename);
   break;

  case FILEFORMAT_PP : {
    CompileInfo *ci=cf_Transform_2_Redfile(filename);
    if (cf_NumberOfErrors(ci)>0) {
     if (mf_ConfirmActionWait(
      "Unable to load ascii file (Contains syntax errors) !\nEdit program with a texteditorobject ?","Yes","No")) {
      Edit_Textfile(filename);
     }
    } else { /* No errors detected */
     char *filename=cf_GetRedFilename(ci);
     rcmd_LoadRedFile(filename);
     if (LOAD_MODE) ConvertFileToEd(filename);
     cf_RemoveTempfile(filename);
     mc_Set_Filename(filename);
    }
    Delete_CompileInfo(ci);
   }
   break;

  case FILEFORMAT_UNKNOWN :
   break;

  default :
   rcmd_LoadEdFile(filename);
   mc_Set_Filename(filename);
   break;

 }

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL); /* Disable windowframe */
 SetupInputFocus(FOCUS_DEFAULT);
 DBUG_RETURN (result);
} /******/

int ff_LoadFile_notify_proc(File_chooser fc,char *path,char *file,Xv_opaque cdata)
{ /* **/
 DBUG_ENTER("ff_LoadFile_notify_proc");

 if (ff_LoadFile(path)) {
  DBUG_RETURN (XV_OK); 
 } else DBUG_RETURN (XV_ERROR);

} /*****/

int ff_LoadSetup_notify_proc(File_chooser fc,char *path,char *file,Xv_opaque cdata)
{ /* **/
 DBUG_ENTER("ff_LoadSetup_notify_proc");

 rcmd_LoadSetups(path);
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_RETURN (XV_OK);
} /*****/

int ff_SaveSetup_notify_proc(File_chooser fc,char *path,char *file,Xv_opaque cdata)
{ /* **/
 DBUG_ENTER("ff_SaveSetup_notify_proc");

 rcmd_SaveSetups(path);
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_RETURN (XV_OK);
} /*****/

int ff_SaveFile_notify_proc(File_chooser fc,char *path,char *file,Xv_opaque cdata)
{ /* **/
 DBUG_ENTER("ff_SaveFile_notify_proc");

 if (LOAD_MODE) {
  rcmd_SaveEdFile(path);
 } else { /* Normal save program request */
  mc_Set_Filename(path); /* Set workfilename to framefooter */
  mf_File_Save(); /* Save working file */
 }

 DBUG_RETURN (XV_OK);
} /*****/
