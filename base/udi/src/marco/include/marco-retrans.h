/**********************************************************************************
***********************************************************************************
**
**   File        : marco-retrans.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 21.5:1
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for Re-Transformation from Orel0 to Source
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

#ifndef _UDIRETRANS_HEADER_
#define _UDIRETRANS_HEADER_ 1

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

/* Transforms Orel0 code to source code **/
extern int udi_Orel0_2_Source();

#endif
