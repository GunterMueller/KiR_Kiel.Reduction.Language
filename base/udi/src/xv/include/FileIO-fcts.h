/**********************************************************************************
***********************************************************************************
**
**   File        : FileIO-fcts.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 8.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : Fileio functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_FILEIOFCTS_HEADER_
#define _XREDUMA_FILEIOFCTS_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <xview/file_chsr.h>
#include "FileIO-create.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

extern int ff_LoadFile(char *filename);

extern int ff_LoadFile_notify_proc(
 File_chooser fc,char *path,char *file,Xv_opaque cdata);

extern int ff_LoadSetup_notify_proc(
 File_chooser fc,char *path,char *file,Xv_opaque cdata);

extern int ff_SaveSetup_notify_proc(
 File_chooser fc,char *path,char *file,Xv_opaque cdata);

extern int ff_SaveFile_notify_proc(
 File_chooser fc,char *path,char *file,Xv_opaque cdata);

extern void ff_GetPath_notify_proc(void);

#endif
