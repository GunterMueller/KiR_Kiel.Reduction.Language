/**********************************************************************************
***********************************************************************************
**
**   File        : TEditor-create.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 30.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : Internal text editor
**                 object.
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_TEDITORCREATE_HEADER_
#define _XREDUMA_TEDITORCREATE_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define DEF_TEOBJECT_WIDTH	600
#define DEF_TEOBJECT_HEIGHT	300

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

extern void Show_TextEditorObject(void);
extern void Edit_Textfile(char *textfilename);

#endif