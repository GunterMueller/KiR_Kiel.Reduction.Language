/**********************************************************************************
***********************************************************************************
**
**   File        : Properties-ard.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 05.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : System properties
**                 Apply, defaults , reset functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_PROPERTIESARD_HEADER_
#define _XREDUMA_PROPERTIESARD_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include "Properties-fcts.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/* Setup symbol to initialize syntax editor **/
#define SEM_KEYS	1	                            /* Setup cursor keys */
#define SEM_PARMS	2	                         /* Setup parmater again */
#define SEM_ALL		255                                   /* Make all setups */

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Set parameter, modes and names to the syntaxeditor **/
extern void Setup_SyntaxEditor(int mode);

/* Arrange ARD buttons for property **/
extern void CenterARDButtons(PropStandard *ps);

/* Get editor values and apply them to Xparameter property values **/
extern void Update_Properties(void);

/* Apply Xparameter property values to editor **/
extern void Update_Parameter(void);

#define SEM_ALL		255                        /* Make all setups */

/* Set all values of property object to their default values **/
extern void SetDefaults_All(void);
extern void SetDefaults_NumberFormat(int Update);
extern void SetDefaults_CalcPrecison(int Update);
extern void SetDefaults_StackSize(int Update);
extern void SetDefaults_HeapSize(int Update);
extern void SetDefaults_Misc(int Update);
extern void SetDefaults_FileExtender(int Update);
extern void SetDefaults_SyntaxEditor(int Update);
extern void SetDefaults_PropertyButtons(int Update);

/* Set all values of property object values **/
extern void Apply_NumberFormat(int Update);
extern void Apply_CalcPrecison(int Update);
extern void Apply_StackSize(int Update);
extern void Apply_HeapSize(int Update);
extern void Apply_Misc(int Update);
extern void Apply_FileExtender(int Update);
extern void Apply_SyntaxEditor(int Update);
extern void Apply_PropertyButtons(int Update);

/* ReSet all values of property object **/
extern void Reset_NumberFormat(int Update);
extern void Reset_CalcPrecison(int Update);
extern void Reset_StackSize(int Update);
extern void Reset_HeapSize(int Update);
extern void Reset_Misc(int Update);
extern void Reset_FileExtender(int Update);
extern void Reset_SyntaxEditor(int Update);
extern void Reset_PropertyButtons(int Update);
	
#endif
