/**********************************************************************************
***********************************************************************************
**
**   File        : server-modul.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 2.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for server functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _SERVERMODUL_HEADER_
#define _SERVERMODUL_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include "clt-srv.h"

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Install server and send invitation message ************************************/
extern int InstallServer(void);

/* Signals xfrontend to edit actual expression with a texteditor object **/
extern void CallTextEditObj(void);

#endif