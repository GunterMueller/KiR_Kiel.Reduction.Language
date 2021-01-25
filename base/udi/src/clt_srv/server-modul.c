/**********************************************************************************
***********************************************************************************
**
**   File        : server-modul.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 2.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for server function
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
#include <fcntl.h>
#include "server-modul.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#ifdef MK_LOGFILE
#define TESTING(output) fprintf(LogFile,output);fflush(LogFile)
#define TESTING_OPEN	LogFile=fopen("LOGFILE","w")
#else
#define TESTING(output)
#define TESTING_OPEN
#endif

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** EXTERN FUNCTION PROTOTYPES *******************************************/
/*********************************************************************************/

extern int EXT_ERROR;
extern char *EXT_ERRORMSG;

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static int x,y;

static char CMD_BUFFER[MTEXT_SIZE];

static FILE *LogFile=NULL;

static int CMD_conter=0;
static int MSGid_cmd;                                     /* Id of message queue */
static int MSGid_cmd2;                                    /* Id of message queue */
static MSG msg;                                         /* Server message buffer */
static MSG msg2;                                        /* Server message buffer */

static ConnectStruct Client;                     /* Client information structure */
static ResultStruct  *Results=NULL;          /* Result struct for redumacommands */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void cleanup(int sig)
{ /* Cleanup message queue *******************************************************/
 sprintf(CMD_BUFFER,"SERVER STOPPED WITH : Signal received %d\n",sig);
 TESTING(CMD_BUFFER);
 msgctl(MSGid_cmd,IPC_RMID,0);                         /* Clean up message queue */
 msgctl(MSGid_cmd2,IPC_RMID,0);                        /* Clean up message queue */
 exit(1);
} /*******************************************************************************/

static void DoConnect(int size)
{ /* Connect or Disconnect a client to this server *******************************/
 TESTING("DoConnect\n");
 memcpy((char*)(&Client),(char*)(&msg.mtext),sizeof(ConnectStruct));
 sprintf(CMD_BUFFER,"Client PID = %d\n",Client.pid);
 TESTING(CMD_BUFFER);
} /*******************************************************************************/

static void DoCommand(int size)
{ /* Process command of a client to this server **********************************/
 CommandStruct *cmd=(CommandStruct*)&(msg.mtext); /* Command buffer */
 int result;

 CMD_conter++;

 TESTING("DoCommand\n");
 switch (cmd->ctype) { /* Test command type */

  case CMD_REDCMD : /* Call reduma command (F8 command) */
   memcpy(CMD_BUFFER,CMD_ADR(cmd),cmd->len); /* Copy command to internal buffer */
   TESTING(CMD_BUFFER);
   result=Xudi_redcmd(CMD_BUFFER,cmd->len,0);
   TESTING("RED CMD RETURNED\n");
   Results->error=EXT_ERROR;
   Results->len=0;
   SEND_MSG(MSGid_cmd,result,msg,MSGT_RESULTS,Results,IPC_NOWAIT);
   break;

  case CMD_SEARCHREPLACE : {
   SRStruct *SR=(SRStruct *)CMD_ADR(cmd);
   TESTING("SEARCH-REPLACE\n");
   sprintf(CMD_BUFFER,"s=<%s> r=<%s> mode=%d number=%d dir=%d\n",
    SR->search,SR->replace,SR->mode,SR->number,SR->dir);
   TESTING(CMD_BUFFER); 
   Results->error=Xudi_searchreplace((SRStruct*)CMD_ADR(cmd));
   Results->len=sizeof(SRStruct);
   memcpy(RES_ADR(cmd),cmd,Results->len);
   TESTING("SEARCH-REPLACE DONE\n");
   SEND_MSG(MSGid_cmd,result,msg,MSGT_RESULTS,Results,IPC_NOWAIT);
   break;
  }

  case CMD_SENDREDPARMS :
   TESTING("CMD_SENDREDPARMS\n");
   Xudi_setparms(CMD_ADR(cmd));
   Results->error=0;
   Results->len=0;
   SEND_MSG(MSGid_cmd,result,msg,MSGT_RESULTS,Results,IPC_NOWAIT);
   TESTING("---- SP DONE ---\n");
   break;
  case CMD_GETREDPARMS :
   TESTING("CMD_GETREDPARMS\n");
   Results->len=Xudi_getparms(CMD_ADR(Results));
   Results->error=0;
   SEND_MSG(MSGid_cmd,result,msg,MSGT_RESULTS,Results,IPC_NOWAIT);
   TESTING("---- GP DONE ---\n");
   break;
  case CMD_TESTEDITLINEMODE :
   Results->len=0;
   Results->result=Xudi_lineeditmode();
   Results->error=0;
   SEND_MSG(MSGid_cmd,result,msg,MSGT_RESULTS,Results,IPC_NOWAIT);
   break;
  case CMD_SETEDITMODE :
   Xudi_seteditmode(cmd->cmd);
   Results->len=0;
   Results->result=0;
   Results->error=0;
   SEND_MSG(MSGid_cmd,result,msg,MSGT_RESULTS,Results,IPC_NOWAIT);
   break;
  default : /* Do nothing */
   fprintf(stderr,"Illegal (unknown command) called !\n");
   break;
 }

 TESTING("DoCommand DONE.\n");
} /*******************************************************************************/

static void ReceiveMessages()
{ /* Receive messages and ... ****************************************************/
 int size;
 signal(SIGUSR1,SIG_IGN);                                      /* Ignore signals */

 sprintf(CMD_BUFFER,"ReceiveMessages <%s>\n",msg.mtext);
 TESTING(CMD_BUFFER);

 do {                                                        /* Receive messages */
  size=RECEIVE_MSG(MSGid_cmd,msg,-MSGT_SERVERMSGS,IPC_NOWAIT);
  if (size>0) {                                            /* Message received ? */
   switch (msg.mtype) {                                             /* Do action */
    case MSGT_CONNECT :                                /* (Dis)Connect message ? */
     DoConnect(size);
     break;
    case MSGT_COMMAND :                                     /* Do command action */
     DoCommand(size);
     break;
    default :                                                  /* Ignore message */
     break;
   }
  } 
 } while (size>0);                                            /* More messages ? */

 signal(SIGUSR1,ReceiveMessages);
 TESTING("MESSAGE RECEIVE DONE\n");
} /*******************************************************************************/

static void SetUpSignalHandler(void)
{ /* Setup signal handler for normal exception signals ***************************/
 int i;
 for (i=0;i<20;i++) 
  signal(i,cleanup);                  /* Setup signal handler for normal signals */
 signal(SIGUSR1,ReceiveMessages);                 /* Install receive msg handler */
 TESTING("SERVER SIGNAL HANDLER installed !\n");
} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

void SendSEMessage(char *msgtext)
{ /* **/
 if (CMD_conter==0) return;
 sprintf(msg2.mtext,"R%s",msgtext);
 msg2.mtype=MSGT_MESSAGE;
 msgsnd(MSGid_cmd2,(struct msgbuf *)(&msg2),strlen(msgtext)+2,IPC_NOWAIT);
 kill(Client.pid,SIGUSR2);
} /***/

void SendFilename(char *name)
{ /* **/
 if (CMD_conter==0) return;
 sprintf(msg2.mtext,"L%s",name);
 TESTING(msg2.mtext);
 msg2.mtype=MSGT_MESSAGE;
 msgsnd(MSGid_cmd2,(struct msgbuf *)(&msg2),strlen(name)+2,IPC_NOWAIT);
 kill(Client.pid,SIGUSR2);
} /***/

void CallTextEditObj(void)
{ /* Signals xfrontend to edit actual expression with a texteditor object **/
 kill(Client.pid,SIGUSR1);
} /*****/

int InstallServer(void)
{ /* Install server and send invitation message **********************************/

 TESTING_OPEN;
 TESTING("START SERVER INSTALLATION !\n");

 MSGid_cmd=msgget(MSGKEY,0700);                       /* Get message queue */
 MSGid_cmd2=msgget(MSGKEY2,0700);                       /* Get message queue */
 if (MSGid_cmd>=0) { /* Exists already a queue ? */
  TESTING("OLD MSG cmd Queue removed !\n");
  msgctl(MSGid_cmd,IPC_RMID,0);                       /* Clean up message queue */
  msgctl(MSGid_cmd2,IPC_RMID,0);                       /* Clean up message queue */
 }

 MSGid_cmd=msgget(MSGKEY,0700|IPC_EXCL|IPC_CREAT);          /* Get message queue */
 MSGid_cmd2=msgget(MSGKEY2,0700|IPC_EXCL|IPC_CREAT);        /* Get message queue */

 if (MSGid_cmd<0) {               /* Message queue error ? */
  switch (errno) {
   case EACCES :
    TESTING("Not allowed to access server, sorry (EACCES).\n");
    break;
   case EEXIST :
    TESTING("Not allowed to access server, sorry.\n");
    TESTING("There is already one server running !\n");
    break;
   case ENOENT :
    TESTING("Not allowed to access server, sorry (ENOENT).\n");
    break;
   case ENOSPC :
    TESTING("Not allowed to access server, sorry (ENOSPC).\n");
    break;
   default :
    TESTING("Not allowed to access server, sorry (UNKNOWN).\n");
    break;
  }
  TESTING("SERVER NOT INSTALLED !\n");
  return (0);
 }

 SetUpSignalHandler();
 if (Results==NULL) Results=(ResultStruct *)malloc(MTEXT_SIZE);

 TESTING("SERVER INSTALLATION DONE!\n");

 return (1);                                                         /* All done */
} /*******************************************************************************/
