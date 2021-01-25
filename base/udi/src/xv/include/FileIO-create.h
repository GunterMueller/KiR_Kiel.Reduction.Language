/**********************************************************************************
***********************************************************************************
**
**   File        : FileIO-create.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 8.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : File requester
**                 generell input output xview create functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_FILEIOCREATE_HEADER_
#define _XREDUMA_FILEIOCREATE_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

extern int LOAD_MODE;

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Update all filerequester extensions **/
extern void fc_Update_Filerequester(void);

/* Call Load file requester and do IO **/
extern int fc_ShowLoadFileRequester(int mode);

extern int fc_ShowLoadSetupRequester(int mode);
extern int fc_ShowSaveSetupRequester(int mode);

/* Call Save file requester and do IO **/
extern int fc_ShowSaveFileRequester(int mode);

/* Popups a requester to select a user definied directory path **/
extern char *fc_GetDirectoryPath(void);

/* Destroys all Xview object (Should only be called at programmend) **/
extern void fc_Delete_xv_Objects(void);

#endif
