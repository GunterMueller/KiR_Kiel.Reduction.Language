/**********************************************************************************
***********************************************************************************
**
**   File        : Menu-fcts.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 28.6.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This module implements all menu button code.
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
#include <signal.h>

#include <xview/notice.h>

#include "dbug.h"

#include "Menu-create.h"
#include "Menu-fcts.h"
#include "FileIO-create.h"
#include "Reduma-cmd.h"
#include "Properties-fcts.h"
#include "Misc-fcts.h"
#include "XReduma.h"
#include "Compiler-fcts.h"
#include "Help-fct.h"

#include "client-modul.h"

#include "strings/WhyNot-Menu-fcts.str"
#include "strings/WhyNot-TEditor-create.str"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define AS_X_OFFSET	100

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char BUFFER[512]; /* Local work buffer */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

int LabelLength(Panel_item item)
{ /* Calculates size of one panel item **/
 Xv_Font          font=XV_NULL;
 Font_string_dims font_size;
 char             *string;
 DBUG_ENTER("LabelLength");

 font=(Xv_Font)xv_get(item,PANEL_LABEL_FONT);
 string=(char *)xv_get(item,PANEL_LABEL_STRING);
 xv_get(font,FONT_STRING_DIMS,string,&font_size);

 DBUG_RETURN (font_size.width);
} /**********/

int LongestLabel(Panel panel)
{ /* Calculates the longest panelitem label **/
 Panel_item       item;
 Xv_Font          font=XV_NULL;
 Font_string_dims font_size;
 int              longest=0;
 char             *string;
 DBUG_ENTER("LongestLabel");


 if (panel==XV_NULL) DBUG_RETURN (longest);

  /* Compute the longest label excluding all panel buttons **/
 PANEL_EACH_ITEM(panel, item) {
  if ((int)xv_get(item, PANEL_SHOW_ITEM) && ((Panel_item_type)
      xv_get(item, PANEL_ITEM_CLASS) != PANEL_BUTTON_ITEM)) {
   font = (Xv_Font)xv_get(item, PANEL_LABEL_FONT);
   string = (char *)xv_get(item, PANEL_LABEL_STRING);
   xv_get(font,FONT_STRING_DIMS, string, &font_size);
   if (font_size.width > longest)  {
    longest = font_size.width;
   }
  }
 } PANEL_END_EACH;

 DBUG_RETURN (longest);
} /****/

void ThisFunctionIsNotAvailable(char *function,char *mode,char *whynot)
{
 int result=0;
 static char BUF1[64];
 static char BUF2[64];
 static Xv_notice notice;
 DBUG_ENTER("ThisFunctionIsNotAvailable");

 sprintf(BUF1,"to use the %s function",function);
 sprintf(BUF2,"in the %s mode !",mode);
 notice=(Xv_notice)xv_create(
  XRED.frame,NOTICE,
  NOTICE_MESSAGE_STRINGS,
   "I am sorry, but it is not possible",
   BUF1,
   BUF2,
   NULL,
  NOTICE_BUTTON_YES,"Okay",
  NOTICE_BUTTON_NO,"Why not?",
  NOTICE_STATUS,&result,
  XV_SHOW,TRUE,
  NULL
 );
 xv_destroy_safe(notice);
 if (result!=NOTICE_YES) {
  notice=(Xv_notice)xv_create(
   XRED.frame,NOTICE,
   NOTICE_MESSAGE_STRINGS,
    "It quiet easy. The reason is :",
    whynot,
    NULL,
   NOTICE_BUTTON_YES,"Okay",
   XV_SHOW,TRUE,
   NOTICE_NO_BEEPING,TRUE,
   NULL
  );
  xv_destroy_safe(notice);
 }

 DBUG_VOID_RETURN;
} /***********/

/* Function implementations for menu : FILE **/

int mf_File_Clear(int dontask)
{ /* Clear (delete actual file) **/
 int result=0;
 DBUG_ENTER("mf_File_Clear");


 if (dontask || mf_ConfirmActionWait(
  "Do you you really want to clear the old program ?","Yes","No")) {
  rcmd_ClearProgram();
  Set_DefFilename();
  rcmd_Buffer_fcts(BUFFER_WRITE_BACKUP);
  XR_MessageDisplay("RProgram cleared.");
  result=1;
 } else {
  XR_MessageDisplay("RClear program canceled.");
 }

 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_RETURN (result);
} /*****/

void mf_File_Load(void)
{ /* Load a new program **/
 DBUG_ENTER("mf_File_Load");

 if (mf_File_Clear(1)) {
  fc_ShowLoadFileRequester(1);
 }
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*********/

void mf_File_Include(void)
{ /* Include a subexpression **/
 DBUG_ENTER("mf_File_Include");

 if (mf_ConfirmActionWait(
  "Do you really want to include a subexpression ?","Yes","No")) {
  fc_ShowLoadFileRequester(0);
 }
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*****/

void mf_File_Save(void)
{ /* Save actual program under current filename **/
 DBUG_ENTER("mf_File_Save");

 rcmd_GoHome();
 rcmd_SaveEdFile(XRED.filename);
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*********/

void mf_File_Saveas(void)
{ /* Save actual program under a new programname **/
 DBUG_ENTER("mf_File_Saveas");

 fc_ShowSaveFileRequester(0);
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*********/

void mf_File_Saveexpr(void)
{ /* **/
 DBUG_ENTER("mf_File_Saveexpr");

 fc_ShowSaveFileRequester(1);
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*********/

void mf_File_LoadSetups(void)
{ /* **/
 DBUG_ENTER("mf_File_LoadSetups");

 fc_ShowLoadSetupRequester(1);
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*********/

void mf_File_SaveSetups(void)
{ /* **/
 DBUG_ENTER("mf_File_SaveSetups");

 fc_ShowSaveSetupRequester(1);
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*********/

void mf_File_Exit(void)
{ /* **/
 DBUG_ENTER("mf_File_Exit");

 QuitXReduma();

 DBUG_VOID_RETURN;
} /*********/

/* Function implementations for menu : REDUCE **/

void mf_Reduce(int function)
{ /* Reduce expression step **/
 DBUG_ENTER("mf_Reduce");

 if (!UDI_CHECK_MODE(S_TTY_EXECUTE)) {
  ThisFunctionIsNotAvailable(
   "reduction",
   "texteditors editing",
   WN_EXECUTION_IS_LOCKED
  );
  DBUG_VOID_RETURN;
 }

 if (IsEditlineOn()) {
  ThisFunctionIsNotAvailable(
   "reduction",
   "line editing",
   WN_SYNTAX_EDITLINE_IS_ON
  );
  DBUG_VOID_RETURN;
 }

 xv_set(XRED.frame,FRAME_BUSY,TRUE,NULL);

 switch (function) { /* Which reduce function to use ? */
  case MM_REDUCE_UNDO :
   XR_MessageDisplay("RUndo last changes.");
   rcmd_Reduce(REDUCE_UNDO);
   break;
  case MM_REDUCE_1 :
   XR_MessageDisplay("RReduce one step.");
   DBUG_PRINT("REDUCE ONE >",(""));
   rcmd_Reduce(1);
   DBUG_PRINT("REDUCE ONE <",(""));
   break;
  case MM_REDUCE_CNT :
   sprintf(BUFFER,"RMake %d reduction steps.",SetupProps.MI.redcnt_value);
   XR_MessageDisplay(BUFFER);
   rcmd_Reduce(SetupProps.MI.redcnt_value);
   break;
  case MM_REDUCE_REDUCEALL :
   XR_MessageDisplay("RExecute programm.");
   rcmd_Reduce(REDUCE_ALL);
   break;
  default : break; /* do nothing */
 } /* End of switch reduce function */

 xv_set(XRED.frame,FRAME_BUSY,FALSE,NULL);

 DBUG_VOID_RETURN;
} /*********/

/* Function implementations for menu : HELP **/

void mf_Help_SyntaxEditor(int help_number)
{ /* Display help text from syntax editor in tty window **/
 DBUG_ENTER("mf_Help_SyntaxEditor");

 XR_MessageDisplay("RShow helpage for syntax editor.");

 hf_ShowHelpViewer(help_number);

 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*****/

/* Function implementations for menu : Buffer **/

void mf_Buffer_fcts(int number)
{ /* Handles buffer operations **/
 DBUG_ENTER("mf_Buffer_fcts");


 if (IsEditlineOn()) {
  ThisFunctionIsNotAvailable(
   "buffer",
   "line editing",
   WN_SYNTAX_EDITLINE_IS_ON
  );
  DBUG_VOID_RETURN;
 }

 if (number==BUFFER_READ_BACKUP) XR_MessageDisplay("RCopy backup buffer to actual expression.");
 else if (number==BUFFER_READ_1) XR_MessageDisplay("RCopy buffer 1 to actual expression.");
 else if (number==BUFFER_READ_2) XR_MessageDisplay("RCopy buffer 2 to actual expression.");
 else if (number==BUFFER_WRITE_BACKUP) XR_MessageDisplay("RCopy actual expression to backup buffer.");
 else if (number==BUFFER_WRITE_1) XR_MessageDisplay("RCopy actual expression to backup buffer 1.");
 else if (number==BUFFER_WRITE_2) XR_MessageDisplay("RCopy actual expression to backup buffer 2.");

 rcmd_Buffer_fcts(number); /* Call rededitor buffer function */


 DBUG_VOID_RETURN;
} /*******/
