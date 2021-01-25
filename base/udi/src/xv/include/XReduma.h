/**********************************************************************************
***********************************************************************************
**
**   File        : XReduma.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 28.6.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_HEADER_
#define _XREDUMA_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

#include <xview/xview.h>
#include <xview/ttysw.h>
#include <xview/textsw.h>
#include <xview/termsw.h>
#include <xview/panel.h>

#include "Properties-fcts.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct XREDTag { /* Global object and variables used in XReduma **********/
 int    syntax_tty_state;                         /* Status of syntax tty object */
 char   *RedumaCmd;                                       /* Redumastart command */
 char   *RedumaParm;                                    /* Redumastart parameter */
 char   *filename;                                           /* Actuall workfile */
 int    red_pid;                                /* Process ID from reduma editor */
 int    openlook;                        /* Openlook window manager (true,false) */
 /* Xview main objects ***********************************************************/
 Frame  frame;                                                    /* Main window */
 Panel  menu_panel;                                       /* Upperside menupanel */
 int    Inputfocus;                            /* Object, which gets input focus */
 int    RED_machine;                          /* Reduma machine type (lred,lneu) */
 Tty    tty;                                /* Terminal object for reduma editor */
} XREDStruct; /*******************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define PROGRAMM_NAME "Xred"              /* Programmname used as basewindowname */
#define PROGRAM_SETUPFILE "Xred.init"           /* Initfile for Xwindow frontend */

#define DEF_REDUMA_CMD  	"sxred"           /* Default reduma command name */
#define DEF_REDUMA_PARMS	"-S"                /* Default startup parameter */

#define SEND_RED_CMD(cmd) ttysw_input(XRED.tty,cmd,strlen(cmd))

#define SetRightFooter(text)	xv_set(XRED.frame,FRAME_RIGHT_FOOTER,text,NULL)
#define SetLeftFooter(text)	xv_set(XRED.frame,FRAME_LEFT_FOOTER,text,NULL)

#define COMMAND_TEXT(text)	SetRightFooter(text)

#define MY_PANEL_BORDER	WIN_BORDER           /* Panel border type (PANEL_BORDER) */

/* Input focus defines ***********************************************************/
#define FOCUS_DEFAULT 		0
#define FOCUS_REDUMA_TTY	1

/* Syntax tty status modes : *****************************************************/
/* WARNING : The same definitions are needed in sfed/udi-EditDefines.h ! *********/
#define S_TTY_READONLY		1               /* set when only allowed to read */
#define S_TTY_EXECUTE		2                 /* set when allowed to execute */
#define S_TTY_COMMANDLINE	4          /* Set when allowed to enter commands */
#define S_TTY_FUNCTIONKEYS	8             /* Set when allowed to press fkeys */
#define S_TTY_NORMAL    (S_TTY_FUNCTIONKEYS|S_TTY_COMMANDLINE|S_TTY_EXECUTE)
#define UDI_CHECK_MODE(mode) ((XRED.syntax_tty_state&mode)==mode)

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

extern XREDStruct XRED;                       /* Main XWindow frontend structure */
extern short IconImage[]; /* Program icon */

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

extern void ResizeRedumaTTY(void);

/* Exit reduma with optinal question mode ****************************************/
extern void QuitXRedumaAsk();
extern void QuitXReduma(void);

/* Set focus to actual active input subwindow (redumatty) **/
extern void SetupInputFocus(int focus);

/* Message signal handler for syntax editor messages **/
extern void XR_MessageDisplay(char *buffer);

#endif
