/**********************************************************************************
***********************************************************************************
**
**   File        : client-modul.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 2.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for client functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _CLIENTMODUL_HEADER_
#define _CLIENTMODUL_HEADER_ 1

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

int IsEditlineOn(void);
void SetSyntaxEditorMode(int mode);

void GetRedParms(char *buffer,int len);
void SendRedParms(char *buffer,int len);

/* Execute a redumacommand on remote reduma server **/
int ExecuteRedCommand(char *command);
int ReduceCommand(char *command);

/* Try to get server and connect **/
int GetAndConnectServer(int PID);

/* Disconnect server **/
void DisconnectServer(void);

/* Tests the ipc message queues **/
int TestMessageQueue(void);

void CleanMessageQueues(void);

int SearchAndReplaceQuery(char *search,char *replace,int mode,int number,int dir);

#endif
