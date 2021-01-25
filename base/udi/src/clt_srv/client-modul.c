/**********************************************************************************
***********************************************************************************
**
**   File        : client-modul.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 2.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for client function
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

/*********************************************************************************/
/********** INTERNAL PROGRAM INFORMATIONS FOR THIS MODULE ************************/
/*********************************************************************************/

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdio.h>

#include "clt-srv.h"
#include "client-modul.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define RETRY_DELAY	1
#define RETRY_NUMBER	100

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

#define BEGIN_SEND	signal(SIGUSR2,SIG_IGN)
#define END_SEND	signal(SIGUSR2,MessageDisplay)

#define BEGIN_IGN_SEND	signal(SIGUSR2,IgnoreHandler)
#define END_IGN_SEND	signal(SIGUSR2,MessageDisplay)

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

extern void XR_MessageDisplay(char *buffer);

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static int ServerPID;                                  /* Process id from server */

static int ConnectRetries=RETRY_NUMBER;

int MSGid2=0;                                           /* Name of message queue */

static int MSGid=0;                                     /* Name of message queue */
static MSG msg;                                                /* Message buffer */
static ConnectStruct Client;                               /* Infos about client */

static CommandStruct *Command=NULL;/* Statical command structure for redcommands */
static ResultStruct  *Results=NULL;                   /* Result buffer structure */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void MessageDisplay()
{ /* Message signal handler for syntax editor messages **/
 int result;
 signal(SIGUSR2,SIG_IGN);

 do { /* Prozess message queue. */
  result=RECEIVE_MSG(MSGid2,msg,MSGT_MESSAGE,IPC_NOWAIT);
  if (result>0) {
   XR_MessageDisplay((char *)&(msg.mtext));
  }
 } while (result>0);

 signal(SIGUSR2,MessageDisplay);
} /******/

static void IgnoreHandler()
{ /* Ignore incoming signal for message queue **/
 signal(SIGUSR2,SIG_IGN);

 signal(SIGUSR2,IgnoreHandler);
} /************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

int TestMessageQueue(void)
{ /* Tests the ipc message queues **/
 int result=0;
 MSGid=msgget(MSGKEY,0700);
 MSGid2=msgget(MSGKEY2,0700);
 if (MSGid<0) {
  switch (errno) {
   case ENOENT :
    msgctl(MSGid,IPC_RMID,0); /* Remove message queue */
    msgctl(MSGid2,IPC_RMID,0); /* Remove message queue */
    result=1; /* IPCS exists, but no queue available */
    break;
   case EACCES :
    fprintf(stderr,"Error on system V ipcs message queues : EACCES\n");
    break;
   case EEXIST :
    msgctl(MSGid,IPC_RMID,0); /* Remove message queue */
    msgctl(MSGid2,IPC_RMID,0); /* Remove message queue */
    result=1;
    break;
   case ENOSPC :
    fprintf(stderr,"Error on system V ipcs message queues : ENOSPC\n");
    break;
   default     :
    fprintf(stderr,"Error on system V ipcs message queues : UNKNOWN\n");
    break;
  }
 } else result=1;
 return (result);
} /*********/

int GetAndConnectServer(int PID)
{ /* Try to get server and connect ***********************************************/
 int result;
 int ok=1;

 do {  /* Try to get message queue */

  MSGid=msgget(MSGKEY,0700);                            /* Get message queue */
  MSGid2=msgget(MSGKEY2,0700);                            /* Get message queue */

  if (MSGid<0) {                                        /* Message queue error ? */
   switch (errno) {
    case ENOENT :
     if (ConnectRetries>0) {
      ConnectRetries--;
      sleep(RETRY_DELAY);
     } else {
      fprintf(stderr,"No message queue available.\n");
      ok=0;
     }
     break;
    case EACCES : 
     ok=0;
     fprintf(stderr,"Not allowed to access server, sorry (EACCES).\n");
     break;
    case EEXIST : 
     ok=0;
     fprintf(stderr,"Not allowed to access server, sorry (EEXIST).\n");
     break;
    case ENOSPC :
     ok=0;
     fprintf(stderr,"Not allowed to access server, sorry (ENOSPC).\n");
     break;
    default :
     ok=0;
     fprintf(stderr,"Not allowed to access server, sorry (UNKNOWN).\n");
     break;
   } /* End of switch */
  } else ok=2; /* Exit loop */

  if (ok==0) return (0); /* Error exit connect */

 } while (ok==1); /* Repeat msgqueue get */

 ConnectRetries=RETRY_NUMBER; /* Reset retries */

 ServerPID=PID;                               /* Store server process ID */

 Client.mode=CONNECT_MODE_ON;
 Client.pid=getpid();                                     /* Clients process id */
 Client.len=0;
 SERVER_REQUEST(MSGid,result,msg,MSGT_CONNECT,(&Client));
 
 if (result>=0) {                                       /* Connection succeed ? */
  Command=(CommandStruct *)malloc(MTEXT_SIZE);
  return (1);                                            /* Connect established */
 } else {
  fprintf(stderr,"Can't connect server !\n");
  return (0); /* Error */
 }

} /*******************************************************************************/

void DisconnectServer(void)
{ /* Disconnect server ***********************************************************/
 int result;
 if (MSGid>=0) {
  msgctl(MSGid,IPC_RMID,0);
  msgctl(MSGid2,IPC_RMID,0);
  MSGid=-1;
  MSGid2=-1;
  if (Command) free(Command);
  Command=NULL;
  Results=NULL;
 }
} /*******************************************************************************/

/*********************************************************************************/

int IsEditlineOn(void)
{
 int result;

 Command->ctype=CMD_TESTEDITLINEMODE;
 Command->len=0;

 SERVER_REQUEST(MSGid,result,msg,MSGT_COMMAND,Command);
 RECEIVE_MSG(MSGid,msg,MSGT_RESULTS,0); /* Wait for results */

 Results=(ResultStruct *)&(msg.mtext);

 if (Results->len!=0) return (0);
 else return (Results->result);
} /**********/

void SetSyntaxEditorMode(int mode)
{
 int result;
 BEGIN_SEND;


 Command->ctype=CMD_SETEDITMODE;
 Command->len=0;
 Command->cmd=mode;

 SERVER_REQUEST(MSGid,result,msg,MSGT_COMMAND,Command);
 RECEIVE_MSG(MSGid,msg,MSGT_RESULTS,0); /* Wait for results */

 END_SEND;
} /**********/

void SendRedParms(char *buffer,int len)
{ /* Copy Redparameter to message buffer and send it to the server **/
 int result;
 BEGIN_SEND;

 Command->ctype=CMD_SENDREDPARMS;
 Command->len=len;
 memcpy(CMD_ADR(Command),buffer,len);

 SERVER_REQUEST(MSGid,result,msg,MSGT_COMMAND,Command);
 RECEIVE_MSG(MSGid,msg,MSGT_RESULTS,0); /* Wait for results */

 END_SEND;
} /**********/

void GetRedParms(char *buffer,int len)
{ /* Get actual parameter from the server **/
 int result;
 BEGIN_SEND;

 Command->ctype=CMD_GETREDPARMS;
 Command->len=0;

 SERVER_REQUEST(MSGid,result,msg,MSGT_COMMAND,Command);
 RECEIVE_MSG(MSGid,msg,MSGT_RESULTS,0); /* Wait for results */

 Results=(ResultStruct *)&(msg.mtext);
 if ((Results->len>0)&&(Results->len<=len)) {
  memcpy(buffer,CMD_ADR(Results),Results->len);
 }

 END_SEND;
} /**********/

int SearchAndReplaceQuery(char *search,char *replace,int mode,int number,int dir)
{ /* Setup search and replace query and send job to sfed server **/
 int result;
 SRStruct *SR=(SRStruct *)CMD_ADR(Command);
  
 BEGIN_SEND;

 Command->ctype=CMD_SEARCHREPLACE;
 Command->len=sizeof(SRStruct);
 strcpy(SR->search,search);
 strcpy(SR->replace,replace);
 SR->mode=mode;
 SR->number=number;
 SR->dir=dir;
 SR->error=0;

 SERVER_REQUEST(MSGid,result,msg,MSGT_COMMAND,Command);
 RECEIVE_MSG(MSGid,msg,MSGT_RESULTS,0); /* Wait for results */ 

 Results=(ResultStruct *)&(msg.mtext);
 result=0;
 if (Results->error) {
  SR=(SRStruct *)RES_ADR(Results); 
  if (SR->error==1) {result=1;strcpy(search,SR->search);}
  if (SR->error==2) {result+=2;strcpy(replace,SR->replace);}
 }

 END_SEND;
 return (result);
} /******/

int ExecuteRedCommand(char *command)
{ /* Execute a redumacommand on remote reduma server **/
 int result;
 BEGIN_SEND;

 Command->ctype=CMD_REDCMD;
 Command->len=strlen(command)+1;
 memcpy(CMD_ADR(Command),command,Command->len);

 SERVER_REQUEST(MSGid,result,msg,MSGT_COMMAND,Command);
 RECEIVE_MSG(MSGid,msg,MSGT_RESULTS,0); /* Wait for results */

 Results=(ResultStruct *)&(msg.mtext);

 END_SEND;

 return (Results->error==0);
} /******/

int ReduceCommand(char *command)
{ /* Execute a reduce redumacommand on remote reduma server **/
 int result;
 BEGIN_IGN_SEND;

 Command->ctype=CMD_REDCMD;
 Command->len=strlen(command)+1;
 memcpy(CMD_ADR(Command),command,Command->len);

 SERVER_REQUEST(MSGid,result,msg,MSGT_COMMAND,Command);
 RECEIVE_MSG(MSGid,msg,MSGT_RESULTS,0); /* Wait for results */

 Results=(ResultStruct *)&(msg.mtext);

 END_IGN_SEND;

 return (Results->error==0);
} /******/

void CleanMessageQueues(void)
{
 msgctl(MSGid,IPC_RMID,0); /* Remove message queue */
 msgctl(MSGid2,IPC_RMID,0); /* Remove message queue */
}
