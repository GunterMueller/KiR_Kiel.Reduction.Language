/**********************************************************************************
***********************************************************************************
**
**   File        : Reduma-cmd.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 8.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This module implements all redumaeditor callfunctions.
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
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

#include "dbug.h"

#include "XReduma.h"
#include "Reduma-cmd.h"
#include "Menu-fcts.h"

#include "client-modul.h"

#include "cscred.h"
#include "ctermio.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define DEF_CMD_BUFFERSIZE 4096                       /* Size of reduma commands */

/* Functionkey shortcuts. Stringconstant to emulate rededitor functionkey ********/
#define RED_FKEY_01	":1\n"
#define RED_FKEY_02	":2\n"
#define RED_FKEY_03	":3\n"
#define RED_FKEY_04	":4\n"
#define RED_FKEY_05	":5\n"
#define RED_FKEY_06	":6\n"
#define RED_FKEY_07	":7\n"
#define RED_FKEY_08	":8\n"
#define RED_FKEY_09	":9\n"
#define RED_FKEY_10	":10\n"
#define RED_FKEY_11	":11\n"
#define RED_FKEY_12	":12\n"
#define RED_FKEY_13	":13\n"
#define RED_FKEY_14	":14\n"
#define RED_FKEY_15	":15\n"
#define RED_FKEY_16	":16\n"

/* Symbolic names for rededitor functions ****************************************/
#define ENTER_COMMAND  		":8"
#define READ_FROM_BACKUPBUFFER	RED_FKEY_05
#define WRITE_TO_BACKUPBUFFER	RED_FKEY_13
#define READ_FROM_BUFFER_1	RED_FKEY_06
#define WRITE_TO_BUFFER_1	RED_FKEY_14
#define READ_FROM_BUFFER_2	RED_FKEY_07
#define WRITE_TO_BUFFER_2	RED_FKEY_15
#define DISPLAY_ACTUAL_EXPR	RED_FKEY_03
#define DISPLAY_FATHER_EXPR	RED_FKEY_11
#define REDUCE_EXPR		RED_FKEY_09
#define REDUCE_WITH_REDCNT	RED_FKEY_01
#define LOAD_ED_EXPR		RED_FKEY_02
#define SAVE_ED_EXPR		RED_FKEY_10

/* Available rededitor commands **************************************************/
#define RED_CMD_RESIZE			"resize"
#define RED_CMD_PRETTYPRINTFILE		"pp"
#define RED_CMD_REDUCE			"r"

/* Special rededitor commands ****************************************************/
#define WRITE_TO_SPECIAL_BUFFER		"my_stack u"
#define READ_FROM_SPECIAL_BUFFER	"my_stack i"

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char CMD_BUFFER[DEF_CMD_BUFFERSIZE];                    /* Command buffer */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

T_UEBERGABE	EditParms; /* Editor-parameter */
char		EditLibs[FN_ANZ][LENLIB]; /* Library FilePathes */
char		EditExt[FN_ANZ][LENEXT]; /* File extensions */
int             sedit_small=0;                      /* Syntax editor layout mode */

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void SendKey(char c)
{ /* Send a single key to red editor *********************************************/
 static char KEY[]=" ";
 KEY[0]=c;
 SEND_RED_CMD(KEY);
} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void rcmd_GetEditorParameter(void)
{ /* Get editor and readuma parameter from the editor ****************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 GetRedParms(CMD_BUFFER,DEF_CMD_BUFFERSIZE);
 memcpy(&(EditLibs),CMD_BUFFER,EDITLIB_SIZE);
 memcpy(&(EditExt),&(CMD_BUFFER[EDITLIB_SIZE]),EDITEXT_SIZE);
 memcpy(&(EditParms),&(CMD_BUFFER[EDITLIB_SIZE+EDITEXT_SIZE]),sizeof(T_UEBERGABE));

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_SetEditorParameter(void)
{ /* Send actual parameter to reduma *********************************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 memcpy(CMD_BUFFER,&(EditLibs),EDITLIB_SIZE);
 memcpy(&(CMD_BUFFER[EDITLIB_SIZE]),&(EditExt),EDITEXT_SIZE);
 memcpy(&(CMD_BUFFER[EDITLIB_SIZE+EDITEXT_SIZE]),&EditParms,sizeof(T_UEBERGABE));
 SendRedParms(CMD_BUFFER,PARAMETER_SIZE);

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_ResizeWindow(int x,int y)
{ /* Resize rededitor tty ********************************************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"%s %d %d",RED_CMD_RESIZE,x,y);
 ExecuteRedCommand(CMD_BUFFER);                            /* Call remote reduma */

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_SavePrettyPrint(char *name)
{ /* Call prettyprint cmd to pretty print file to name ***************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"my_special 4 %s\n",name);
 ExecuteRedCommand(CMD_BUFFER); /* Call remote reduma */ 

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_CurHome(void)
{ /* Go to first expression (home) ***********************************************/
 SendKey(_HOME);
} /*******************************************************************************/

void rcmd_ClearInputline(void)
{ /* *****************************************************************************/
 SendKey(_HOME);
 SendKey(_DELEOL);
 SendKey('\n');
} /*******************************************************************************/

void rcmd_CurGoal(void)
{ /* *****************************************************************************/
 SendKey(_GOAL);
} /*******************************************************************************/

void rcmd_CurUp(void)
{ /* *****************************************************************************/
 SendKey(UP);
} /*******************************************************************************/

void rcmd_CurDown(void)
{ /* *****************************************************************************/
 SendKey(DOWN);
} /*******************************************************************************/

void rcmd_CurLeft(void)
{ /* *****************************************************************************/
 SendKey(LEFT);
} /*******************************************************************************/

void rcmd_CurRight(void)
{ /* *****************************************************************************/
 SendKey(RIGHT);
} /*******************************************************************************/

void rcmd_DeleteExpr(void)
{ /* *****************************************************************************/
 SEND_RED_CMD(WRITE_TO_BACKUPBUFFER);
 SEND_RED_CMD("#\n");
} /*******************************************************************************/

void rcmd_F3Func(void)
{ /* *****************************************************************************/
 SEND_RED_CMD(":3\n");
} /*******************************************************************************/

void rcmd_F11Func(void)
{ /* *****************************************************************************/
 SEND_RED_CMD(":11\n");
} /*******************************************************************************/

int rcmd_Change2DollarVar(char *name,int archive)
{ /* *****************************************************************************/
 int result=1;
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"ed_save %s",name);
 ExecuteRedCommand(CMD_BUFFER);
 sprintf(CMD_BUFFER,"my_input $%s",name);
 if (ExecuteRedCommand(CMD_BUFFER)) {
  if (archive) {
   sprintf(CMD_BUFFER,"archive_expr a %s",name);
   ExecuteRedCommand(CMD_BUFFER);
  }
  rcmd_CurHome();
 } else {
  result=0;
 }

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
 return (result);
} /*******************************************************************************/

void rcmd_SetCursorTo(char *name)
{ /* *****************************************************************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"archive_expr r %s",name);
 ExecuteRedCommand(CMD_BUFFER);
 
 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_LoadEdFile(char *name)
{ /* *****************************************************************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"ed_load %s",name);
 ExecuteRedCommand(CMD_BUFFER);

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_SaveEdFile(char *name)
{ /* *****************************************************************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"ed_save %s",name);
 ExecuteRedCommand(CMD_BUFFER);

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_LoadRedFile(char *name)
{ /* *****************************************************************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"load %s\n",name);            /* Newline is VERY important ! */
 ExecuteRedCommand(CMD_BUFFER);

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_LoadSetups(char *name)
{ /* Loads red parameter from file name **/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"setupload %s",name);
 ExecuteRedCommand(CMD_BUFFER);

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******/

void rcmd_SaveSetups(char *name)
{ /* Save red parameter from file name **/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 sprintf(CMD_BUFFER,"setupsave %s",name);
 ExecuteRedCommand(CMD_BUFFER);

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******/

void rcmd_Reduce(int number)
{ /* Reduce reduma expression ****************************************************/

 DBUG_ENTER("rcmd_Reduce");

 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 if (number>REDUCE_ALL) {                                  /* Reduce with number */
  sprintf(CMD_BUFFER,"%s %d\n",RED_CMD_REDUCE,number);
  ReduceCommand(CMD_BUFFER);
  DBUG_PRINT("OK",(""));
 } else {
  if (number==REDUCE_ALL) {
   sprintf(CMD_BUFFER,"%s %d\n",RED_CMD_REDUCE,2000000000);
   ReduceCommand(CMD_BUFFER);
  } else if (number==REDUCE_UNDO) {
   ReduceCommand("my_special 1\n");
  } else {
   ReduceCommand("NOP");
  }
 }

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);

 DBUG_VOID_RETURN;
} /*******************************************************************************/

void rcmd_ClearProgram(void)
{ /* *****************************************************************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);
 ExecuteRedCommand("clearprogram");
 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

void rcmd_Buffer_fcts(int number)
{ /* *****************************************************************************/
 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 switch (number) {
  case BUFFER_READ_BACKUP  : SEND_RED_CMD(READ_FROM_BACKUPBUFFER);break;
  case BUFFER_READ_1       : SEND_RED_CMD(READ_FROM_BUFFER_1);break;
  case BUFFER_READ_2       : SEND_RED_CMD(READ_FROM_BUFFER_2);break;
  case BUFFER_WRITE_BACKUP : SEND_RED_CMD(WRITE_TO_BACKUPBUFFER);break;
  case BUFFER_WRITE_1      : SEND_RED_CMD(WRITE_TO_BUFFER_1);break;
  case BUFFER_WRITE_2      : SEND_RED_CMD(WRITE_TO_BUFFER_2);break;
  default : break;
 }

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);
} /*******************************************************************************/

static int MY_STACK_USED=0;

void rcmd_CopyToStack(void)
{
 MY_STACK_USED=1;
 ExecuteRedCommand("my_stack i");
}

int rcmd_CopyFromStack(void)
{
 if (MY_STACK_USED) {
  ExecuteRedCommand("my_stack u");
  return (1);
 } else return (0);
}

void rcmd_CutToStack(void)
{
 MY_STACK_USED=1;
 ExecuteRedCommand("my_stack i");
 ExecuteRedCommand("my_input #"); 
}

void rcmd_CursorKeySetting(char u,char d,char l,char r,char h,char ret)
{ /* Defines Cursor key behaviour ************************************************/
 sprintf(CMD_BUFFER,"my_keymodes %d %d %d %d %d %d",u,d,l,r,h,ret);
 ExecuteRedCommand(CMD_BUFFER);
} /*******************************************************************************/

int rcmd_SearchReplace(SRValues *SV)
{ /* *****************************************************************************/
 int result;

 if (SV->range==0) {                                            /* Cursor home ? */
  rcmd_CurHome();
  rcmd_CurHome();
 }
 result=SearchAndReplaceQuery(
  SV->search,
  SV->replace,
  SV->mode,
  SV->number,
  0
 );
 
 return (result);
} /*******************************************************************************/

void rcmd_SetSyntaxEditorLayout(int result)
{ /* *****************************************************************************/
 sprintf(CMD_BUFFER,"my_special 2 %d\n",result);
 ExecuteRedCommand(CMD_BUFFER);
} /*******************************************************************************/

void rcmd_GoHome(void)
{ /* Set cursor to top expression ************************************************/
 ExecuteRedCommand("my_special 3\n");
} /*******************************************************************************/
