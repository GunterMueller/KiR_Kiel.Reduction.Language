/**********************************************************************************
***********************************************************************************
**
**   File        : Misc-fcts.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 20.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : Different functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_MISCFCTS_HEADER_
#define _XREDUMA_MISCFCTS_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <xview/panel.h>

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct SRValuesTag { /* Requester values **/
 char *search;
 char *replace;
 int  range;
 int  number;
 int  mode;
 int error;
} SRValues; /*****/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Popup ask requester (yes/no) **/
extern int mf_ConfirmAction(Panel panel,char *text,Event *event);

/* Popup ask requester and wait for answer **/
extern int mf_ConfirmActionWait(char *text,char *yes,char *no);

/* Popup search and replace Requester (Only one possible) **/
extern int mf_Search_and_Replace(void);

extern void mf_SetSyntaxEditorMode(int mode);

#endif
