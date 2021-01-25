/**********************************************************************************
***********************************************************************************
**
**   File        : marco-print.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 03.6.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for internal node print functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _UDIPRINT_HEADER_
#define _UDIPRINT_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

#include "marco-create.h"

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

/* Set up modul for printouts **/
extern void Init_PrintModul(FILE *FileHandle);

extern void print_Token(char *token,int len);

extern void print_Delimiter(char *delimiter,int len);

extern void print_SetTab(void);

extern void print_ClearTab(void);

extern void print_Indent(void);

extern void print_NORMAL(NODE *node);

extern void print_CarriageReturn(void);

extern void print_BindSy(void);

extern void print_Tab(void);

#endif
