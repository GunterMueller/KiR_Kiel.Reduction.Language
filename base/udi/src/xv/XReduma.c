/**********************************************************************************
***********************************************************************************
**
**   File        : XReduma.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 28.6.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. The XWindows code depends on XVIEW 3.0
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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/wait.h>
#include <xview/notice.h>

#include "dbug.h"

#include "XReduma.h"
#include "Menu-create.h"
#include "FileIO-create.h"
#include "FileIO-fcts.h"
#include "Properties-create.h"
#include "Properties-ard.h"
#include "Reduma-cmd.h"
#include "TEditor-create.h"
#include "Compiler-fcts.h"
#include "Control-create.h"

#include "../clt_srv/client-modul.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define DEF_WINDOWMANAGER	0

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

static void StartRedumaEditor(void);                         /* Start the editor */

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static int  ServerConnectDone=0;           /* Flag, signs if server is connected */

short IconImage[]= {                                      /* Programm icon image */
#include "images/XReduma.icon"
};

/* Some variable needed to manage object resizing feature ************************/
static int oldwidth=-1,oldheight=-1,width=0,height=0;
static int deltawidth=0,deltaheight=0;
static int INTEDIT_RESIZED=0,RED_TTY_RESIZED=0;

static char **RedArgv=NULL;                         /* Arguments for redumastart */

static int NrOfStarts=0;                    /* Count number of start from reduma */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

XREDStruct XRED;                              /* Main XWindow frontend structure */

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static int search_exe(char *name)
{ /*******************************************************************************/
 static char BUF[1024];
 static char BUF2[1024];
 char *path=getenv("PATH");
 char *tst;
 char *bak;
 int found = 0;
 static struct stat S;

 if (path) {
  strcpy(BUF,path);
  tst = bak = BUF;
  while (tst && !found ) {
   tst=strtok(bak,":");
   if (tst) {
     if (tst[strlen(tst)-1]=='/') tst[strlen(tst)-1] = '\0';
     sprintf(BUF2,"%s/%s",tst,name);
     if (stat(BUF2,&S) == 0) {
      found = 1;
     } 
   } else break;
  if (bak) bak = NULL;
  }
 }

 return (found);
} /*******************************************************************************/

static void ConnectServer(void)
{ /* Server started, connect server **********************************************/
 DBUG_ENTER("ConnectServer");

 if (!ServerConnectDone) {
  if (GetAndConnectServer(XRED.red_pid)) {           /* Connection established ? */
   ServerConnectDone=1;                     /* Client connected to reduma server */
   mf_SetSyntaxEditorMode(XRED.syntax_tty_state); /* Set default editstatusflags */
  }
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static int InitXREDStructure()
{ /* Setup program variables and do init FrontEnd ********************************/
 DBUG_ENTER("InitXREDStructure");

 XRED.RedumaCmd=DEF_REDUMA_CMD;                                /* Reduma command */
 XRED.RedumaParm=DEF_REDUMA_PARMS;                   /* Reduma command parameter */
 XRED.Inputfocus=FOCUS_REDUMA_TTY;                     /* Set input focus object */
 XRED.RED_machine=REDTYP_DEFAULT;                  /* Set default reduma machine */
 XRED.tty=XV_NULL;                                     /* Object not initialized */
 XRED.filename=NULL;                                           /* No file loaded */
 XRED.openlook=DEF_WINDOWMANAGER;                  /* Set default window manager */
 XRED.syntax_tty_state=S_TTY_NORMAL;
 DBUG_RETURN (1);                                                   /* Init done */
} /*******************************************************************************/

static void WindowEventProc(Xv_Window window,Event *event)
{ /* Process all window events ***************************************************/
 DBUG_ENTER("WindowEventProc");

 if (event) {                                              /* Test event pointer */
  switch (event_id(event)) {                                       /* Test event */
   case WIN_RESIZE :
    if (ServerConnectDone) {
     width=(int)xv_get(XRED.frame,XV_WIDTH);
     height=(int)xv_get(XRED.frame,XV_HEIGHT);
     if ((oldwidth!=width)||(oldheight!=height)) {
      RED_TTY_RESIZED=INTEDIT_RESIZED=0;
      ResizeRedumaTTY();
      oldwidth=width;
      oldheight=height;
     }
    }
    break;
   case ACTION_NULL_EVENT : SetupInputFocus(FOCUS_DEFAULT); break;
   default :                                    /* Default action : ignore event */
    break;
  }
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static Notify_value My_wait_func(
 Tty tty,int pid,union wait *status,struct rusage *usage)
{ /* Tty subwindow controll function *********************************************/
 DBUG_ENTER("My_wait_func");

 if (!(WIFSTOPPED(*status))) {
  Xv_notice notice;
  int answer=0;
  notice=(Xv_notice)xv_create(
   XRED.frame,
   NOTICE,
   NOTICE_MESSAGE_STRINGS,
    "pi RED editor programm is stopped !",
    "Do you want to start a new one ?",
    "Otherwise the programm will exit now.",
    NULL,
   NOTICE_BUTTON_YES,"Yes",
   NOTICE_BUTTON_NO,"No",
   NOTICE_STATUS,&answer,
   XV_SHOW,TRUE,
   NULL
  );
  if (answer==NOTICE_YES) {                                /* Restart programm ? */
   xv_destroy_safe(notice);
   ServerConnectDone=0;                              /* Server no longer valid ! */
   DisconnectServer();
   StartRedumaEditor();                                  /* Start programm again */
  } else {                                                    /* Quit programm ? */
   xv_destroy_safe(notice);
   QuitXReduma();
  }
 }

 DBUG_RETURN (NOTIFY_DONE);                                /* Ok, let's continue */
} /*******************************************************************************/

static void StartRedumaEditor(void)
{ /* Start reduma programm in tty subwindow to setup controll functions **********/
 DBUG_ENTER("StartRedumaEditor");

 if (search_exe(DEF_REDUMA_CMD)==0) {
  fprintf(stderr,"CAN'T FIND SYNTAX DIRECTED EDITOR IN YOUR SEARCH PATH !\n");
  fprintf(stderr,"Please add the binarypath for <%s> to your environment variable PATH.\n",DEF_REDUMA_CMD);
  exit (1);
 }

 NrOfStarts++;                                  /* Increase number of (re)starts */

 XRED.tty=(Tty)xv_create(               /* Create a terminal for reduma programm */
  XRED.frame,                                           /* Parent is main Window */
  TTY,                                                                   /* Type */
  WIN_X,0,                                        /* Arrange window to left side */
  WIN_BELOW,XRED.menu_panel,                  /* Set tty window below menu panel */
  XV_SHOW,TRUE,                                        /* Show syntax editor tty */
  TTY_CONSOLE,FALSE,
  TTY_ARGV,RedArgv,                                           /* Startcmd-vector */
  NULL                                                           /* End of list */
 ); 

 XRED.red_pid=(int)xv_get(XRED.tty,TTY_PID);         /* Get process id of editor */
 notify_set_wait3_func(XRED.tty,My_wait_func,XRED.red_pid); /* set controll fct */

 SetupInputFocus(FOCUS_REDUMA_TTY);             /* Set input focus to tty reduma */

 ConnectServer();

 if (NrOfStarts==1) {                                  /* First programm start ? */
  rcmd_GetEditorParameter();                        /* Get parameter from editor */
  SetDefaults_SyntaxEditor(0);
  Setup_SyntaxEditor(SEM_KEYS);
 } else {
  if (strcmp(XRED.filename,"default.ed")) {                  /* Other filename ? */
   static char BUFFER[256];
   sprintf(BUFFER,"Do you want to reload last edited file\n%s ?",XRED.filename);
   if (mf_ConfirmActionWait(BUFFER,"Load","Cancel")) {
    ff_LoadFile(XRED.filename);
   } else {
    Set_DefFilename();
   }
  }
  Setup_SyntaxEditor(SEM_ALL);     /* Tries to setup syntax editor to last state */
 }


 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void SetupRedArgv(int argc,char *argv[])
{ /* Pass arguments to reduma editor server **************************************/
 int nr,i;
 DBUG_ENTER("SetupRedArgv");

 if (RedArgv!=NULL) free(RedArgv);                                /* Free Memory */
 nr=2+argc;                                           /* Size of argument vector */
 RedArgv=(char **)malloc(nr*sizeof(char *));                        /* Get space */
 RedArgv[0]=XRED.RedumaCmd;                                  /* Programm command */
 RedArgv[1]=XRED.RedumaParm;                               /* Programm parameter */
 i=1;
 while (i<argc) { 
  if ((argv[i][0]=='-')&&(argv[i][1]=='#')) {
                                                            /* DBUG_PUSH("t:d"); */
  } else {
   RedArgv[i+1]=argv[i];                                  /* Set first parameter */
  }
  i++; 
 }
 RedArgv[i+1]=NULL; /* End of argument vector */ 

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void XFrontEndSignalHandler()
{ /* Handles signals from the system *********************************************/
 signal(SIGUSR1,SIG_IGN);
 XR_MessageDisplay("RPop up texteditor object.");
 Show_TextEditorObject();
 signal(SIGUSR1,XFrontEndSignalHandler);
} /*******************************************************************************/

static void ExternalProgrammStop(int sig)
{ /* Programm is stopped by external event ***************************************/
 signal(sig,SIG_DFL);                           /* Install default signalhandler */
 
 cf_RemoveAllTempFiles();                                /* Delete all tempfiles */
 CleanMessageQueues();                                  /* Delete all msg-queues */

 kill(getpid(),sig);             /* Send signal again to enable default function */
} /*******************************************************************************/

static void InstallSignalHandler(void)
{ /* Installs signal handler for the frontend programm ***************************/

 signal(SIGUSR1,XFrontEndSignalHandler);        /* Install port for syntaxeditor */
 signal(SIGINT,ExternalProgrammStop);
 signal(SIGBUS,ExternalProgrammStop);
 signal(SIGSEGV,ExternalProgrammStop);

} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void XR_MessageDisplay(char *buffer)
{ /* Message signal handler for syntax editor messages ***************************/
 if (buffer[0]=='R') {
  xv_set(XRED.frame,FRAME_LEFT_FOOTER,&(buffer[1]),NULL);
 } else if (buffer[0]=='L') {
  mc_Set_Filename(&(buffer[1]));
 }
} /*******************************************************************************/

void QuitXReduma(void)
{ /* Quit programm in save way, save all infos and ... ***************************/
                                                 /* Save RedParameter : red.init */
                                            /* Save XParameter   : $HOME/.xredrc */
                                                         /* Programm not saved ? */
 DBUG_ENTER("QuitXReduma");

 fc_Delete_xv_Objects();
 pc_Delete_xv_Objects();
 xv_destroy_safe(XRED.frame);             /* Cleanup windows and destroy objects */
 Hide_ControlPad();
 Kill_ControlPad();
 DisconnectServer();          /* Remove client from server and destroy msg queue */
 cf_RemoveAllTempFiles();                                /* Delete all tempfiles */
 xv_window_return(0);                         /* Program end, exit from mainloop */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

void ResizeRedumaTTY(void)
{ /* Neecary to resize reduma tty ? **********************************************/
 DBUG_ENTER("ResizeRedumaTTY");

 if ((RED_TTY_RESIZED==0)&&(XRED.tty!=XV_NULL)) {
  rcmd_AdjustWindowSize();
  RED_TTY_RESIZED=1;                                       /* Reduma tty resized */
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

void QuitXRedumaAsk(Panel_item item,Event *event)
{ /* *****************************************************************************/
 int answer=NOTICE_YES;                                  /* Defaultvalue is quit */
 DBUG_ENTER("QuitXRedumaAsk");


   answer = notice_prompt(XRED.menu_panel, NULL,
    NOTICE_FOCUS_XY,        event_x(event), event_y(event),
    NOTICE_MESSAGE_STRINGS, "Do you really want to quit?", NULL,
    NOTICE_BUTTON_YES,      "Yes",
    NOTICE_BUTTON_NO,       "No",
    NULL);


 if (answer==NOTICE_YES) QuitXReduma();

 DBUG_VOID_RETURN;
} /*******************************************************************************/

void SetupInputFocus(int focus)
{ /* Set focus to actual active input subwindow (redtty, editor or internal ed **/
 DBUG_ENTER("SetupInputFocus");

 if (focus==FOCUS_DEFAULT) focus=XRED.Inputfocus;              /* Use last focus */
 else XRED.Inputfocus=focus;                                /* Store focus value */
 switch (focus) {
  case FOCUS_REDUMA_TTY :             /* Set focus to reduma tty (default value) */
  default : xv_set(XRED.tty,WIN_SET_FOCUS,NULL);break;
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

/*********************************************************************************/
/********** PROGRAM MAIN FUNCTION ************************************************/
/*********************************************************************************/

int main(int argc,char *argv[])
{ /* Programm main function ******************************************************/
 Icon closedIcon;
 Server_image Closed_image;
 DBUG_ENTER("Xmain");
 DBUG_PUSH ("t:d");

 if (!TestMessageQueue()) {                 /* Test System V ipcs message queues */
  fprintf(stderr,"Error on communication. Can not start program !\n");
  exit(1);
 }

 if (InitXREDStructure()==0) {   /* Setup program variables and do init FrontEnd */
  fprintf(stderr,"%s initialisation failed !\n",PROGRAMM_NAME);
  exit(1);                                                    /* Programm failed */
 }

 InitPropertyValues();                                       /* Do property init */

 xv_init(XV_INIT_ARGC_PTR_ARGV,&argc,argv,NULL);            /* Init Xview server */
 SetupRedArgv(argc,argv);              /* Pass arguments to reduma editor server */

 Closed_image=(Server_image)xv_create(                /* Create close icon image */
  XV_NULL,                                                   /* No parent needed */
  SERVER_IMAGE,                                                          /* Type */
  XV_WIDTH,64,                                                /* Iconimage width */
  XV_HEIGHT,64,                                              /* Iconimage height */
  SERVER_IMAGE_BITS,IconImage,                                           /* Bits */
  NULL                                                            /* End of list */
 );

 XRED.frame=(Frame)xv_create(                                 /* Make main frame */
  XV_NULL,                                                  /* No parent allowed */
  FRAME,                                                                 /* Type */
  FRAME_LABEL,argv[0],                        /* Programm header is command name */
  FRAME_SHOW_FOOTER,TRUE,                       /* Show programm infos at footer */
  FRAME_LEFT_FOOTER,"",                                           /* Left footer */
  FRAME_RIGHT_FOOTER,"",                                         /* Right footer */
  FRAME_NO_CONFIRM,FALSE,                         /* Show quit confirm requester */
  FRAME_DONE_PROC,QuitXReduma,                             /* Quit programm save */
  WIN_CONSUME_EVENTS,WIN_MOUSE_BUTTONS,NULL,
  WIN_EVENT_PROC,WindowEventProc,                  /* Callback for window events */
  NULL                                                   /* End of argument list */
 );

 closedIcon=(Icon)xv_create(                               /* Create icon handle */
  XRED.frame,                                           /* Main window is parent */
  ICON,                                                                  /* Type */
  ICON_IMAGE,Closed_image,                                       /* Imaga handle */
  NULL                                                            /* End of list */
 );

 xv_set(XRED.frame,FRAME_ICON,closedIcon,NULL);     /* Assign icon to mainwindow */

 XRED.menu_panel=(Panel)xv_create(
  XRED.frame, /* Main window is parent */ 
  PANEL,                                                                 /* Type */
  MY_PANEL_BORDER,TRUE,
  XV_X,0,                                                         /* Left corner */
  XV_Y,0,                                                          /* Upper side */
  XV_WIDTH,WIN_EXTEND_TO_EDGE,
  NULL                                                            /* End of list */
 );

 mb_CreateMenuPanel(XRED.menu_panel);                       /* Make menu buttons */
 window_fit_height(XRED.menu_panel);                        /* Adjust panel size */

 InstallSignalHandler();

 StartRedumaEditor();                                          /* Let's play ;-) */

 window_fit(XRED.frame);                              /* Adjust main window size */

/* Sets window to minimal size
 xv_set(
  XRED.frame,
  FRAME_MIN_SIZE,xv_get(XRED.frame,XV_WIDTH),xv_get(XRED.frame,XV_HEIGHT),
  NULL
 );
*/

 deltawidth=(int)xv_get(XRED.frame,XV_WIDTH)-(int)xv_get(XRED.tty,XV_WIDTH);
 deltaheight=(int)xv_get(XRED.frame,XV_HEIGHT)-(int)xv_get(XRED.tty,XV_HEIGHT);

 Set_DefFilename();
 XR_MessageDisplay("RRED 2.3 Function- Version");

 xv_main_loop(XRED.frame);                                 /* Loop window events */

 DBUG_RETURN (0);
} /*******************************************************************************/
