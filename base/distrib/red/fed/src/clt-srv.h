/**********************************************************************************
***********************************************************************************
**
**   File        : clt-srv.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 1.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for client server functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _CLT_SRV_HEADER_
#define _CLT_SRV_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define CMD_REDCMD		1
#define CMD_SENDREDPARMS	2
#define CMD_GETREDPARMS		3
#define CMD_TESTEDITLINEMODE	4
#define CMD_SETEDITMODE		5
#define CMD_SEARCHREPLACE	6

#define MTEXT_SIZE      10240
#define MSGKEY		11
#define MSGKEY2		13

#define MSGT_CONNECT    1
#define MSGT_COMMAND   	2 

#define MSGT_SERVERMSGS	16
#define MSGT_RESULTS	17
#define MSGT_MESSAGE	18

#define CONNECT_MODE_ON	1

#define SEND_MSG(mid,result,msg,msgtype,data,flags) \
	msg.mtype=msgtype;\
        data->len+=sizeof(*data); \
	memcpy(&(msg.mtext),(char *)(data),data->len); \
        result=msgsnd(mid,(struct msgbuf *)(&msg),data->len,flags)

#define SERVER_REQUEST(mid,result,msg,msgtype,data) \
	SEND_MSG(mid,result,msg,msgtype,data,IPC_NOWAIT);\
	kill(ServerPID,SIGUSR1)
	
#define RECEIVE_MSG(mid,msg,type,flags) \
	msgrcv(mid,(struct msgbuf *)&msg,sizeof(MSG),type,flags)

#define RES_ADR(cmd) (&(((char*)cmd)[sizeof(ResultStruct)]))
#define CMD_ADR(cmd) (&(((char*)cmd)[sizeof(CommandStruct)]))
#define SR_ADR(cmd) (&(((char*)cmd)[sizeof(SRStruct)]))

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct msgform { /* Generell message buffer type **/
 long mtype; /* Type of message */
 char mtext[MTEXT_SIZE]; /* Message */
} MSG; /*******/

typedef struct ConnectStructTag { /* Connect struct for client/server **/
 int pid; /* Process id */
 int mode; /* Connection mode (connect or disconnect */
 int len; /* Connect msg len */
} ConnectStruct; /*******/

typedef struct CommandStructTag { /* Commando bufferstructure **/
 int ctype; /* Commandtype */
 int len; /* Commandlen */
 int cmd;
} CommandStruct; /********/

typedef struct ResultStructTag { /* Result bufferstructure **/
 int error; /* Error flag */
 int len; /* Resultlen */
 int result; /* Result value */
} ResultStruct; /********/

typedef struct SRStructTag { /* Search and replace query **/
 char search[256];
 char replace[256];
 int mode;
 int number;
 int dir;
 int error;
} SRStruct; /*****/

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

#endif
