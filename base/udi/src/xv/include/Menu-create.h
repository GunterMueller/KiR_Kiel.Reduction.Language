/**********************************************************************************
***********************************************************************************
**
**   File        : Menu-create.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 28.6.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : menu buttons
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_MENUCREATE_HEADER_
#define _XREDUMA_MENUCREATE_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <xview/xview.h>
#include <xview/panel.h>

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define Set_DefFilename()       mc_Set_Filename("default.ed")

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Set filename to name, display name in left frame footer **/
extern void mc_Set_Filename(char *name);
extern char *mc_GetFilename(void);
extern char *mc_GetFilepath(void);

/* Create all menu buttons *******************************************************/
extern void mb_CreateMenuPanel(Panel panel);

/* Set number of textobject in use **/
extern void mc_SetTextObjectNumber(int number);

#endif
