/**********************************************************************************
***********************************************************************************
**
**   File        : Misc-fcts.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 20.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This module implements some often used functions.
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

#include <xview/notice.h>

#include "dbug.h"

#include "client-modul.h"
#include "Misc-fcts.h"
#include "Reduma-cmd.h"
#include "XReduma.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct SR_StructTag { /* Search and replace structure **/
 Frame frame;
 Panel panel;
 Panel_item pi_searchtxt; /* Search expr */
 Panel_item pi_replacetxt; /* Replace expr */
 Panel_item pi_mode; /* (ask before, number, all) */
 Panel_item pi_range; /* Search from cursor expression or whole programm */
 Panel_item pi_number; /* How many expressions should be replaced */
 Panel_item pi_search; /* Search only */
 Panel_item pi_replace; /* Search only */
 Panel_item pi_cancel;
 SRValues values;
} SR_Struct; /*******/ 

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define SR_DISPLAY_LEN 64
#define SR_STORE_LEN 256

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static int SR_INIT_NOT_DONE=1; /* Signs if search and replace requester */
static SR_Struct SR; /* Search and replace structure */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static int GetSRValues(int mode,SR_Struct *S,SRValues *V)
{ /* Get actual values and check them **/
 char *str;
 DBUG_ENTER("GetSRValues");

 if ((!V)||(!S)) DBUG_RETURN (0); /* No pointer to structure */

 str=(char *)xv_get(S->pi_searchtxt,PANEL_VALUE);

 if (strlen(str)==0) { 
  xv_set(S->frame,FRAME_LEFT_FOOTER,"No search string !",NULL);
  xv_set(S->frame,FRAME_CMD_PIN_STATE,FRAME_CMD_PIN_IN,NULL); 
  DBUG_RETURN (0);
 } 

 strcpy(V->search,str);

 if (mode) { /* Replace modus ? */
  str=(char *)xv_get(S->pi_replacetxt,PANEL_VALUE);
  if (strlen(str)==0) {
   xv_set(S->frame,FRAME_LEFT_FOOTER,"No replace string !",NULL);
   xv_set(S->frame,FRAME_CMD_PIN_STATE,FRAME_CMD_PIN_IN,NULL); 
   DBUG_RETURN (0);
  }

  strcpy(V->replace,str);

  if (strcmp(V->search,V->replace)==0) {
   xv_set(S->frame,FRAME_LEFT_FOOTER,"Hmm, what a strange action !",NULL);
   xv_set(S->frame,FRAME_CMD_PIN_STATE,FRAME_CMD_PIN_OUT,NULL); 
   DBUG_RETURN (0);
  }
 } else strcpy(V->replace,"");

 V->range=(int)xv_get(S->pi_range,PANEL_VALUE);
 V->number=(int)xv_get(S->pi_number,PANEL_VALUE);
 V->mode=(int)xv_get(S->pi_mode,PANEL_VALUE);

 DBUG_RETURN (1);
} /*******/

static void sr_search_proc(Panel_item item,int value,Event *event)
{ /* **/
 int result;
 DBUG_ENTER("sr_search_proc");

 puts("sr_search_proc");

 if (GetSRValues(0,&SR,&(SR.values))==1) {
  result=rcmd_SearchReplace(&(SR.values));

 printf("Result = %d Search <%s> replace <%s> range = %d number = %d mode =%d\n",
  result,
  SR.values.search,
  SR.values.replace,
  SR.values.range,
  SR.values.number,
  SR.values.mode
 ); fflush(stdout);

  if (result==0) {
   xv_set(SR.frame,FRAME_CMD_PIN_STATE,FRAME_CMD_PIN_OUT,NULL);
  } else {
   if ((result&1)==1) xv_set(SR.frame,FRAME_LEFT_FOOTER,SR.values.search,NULL);
   if ((result&2)==2) xv_set(SR.frame,FRAME_RIGHT_FOOTER,SR.values.replace,NULL);
  }
 }

 DBUG_VOID_RETURN;
} /*******/

static void sr_replace_proc(Panel_item item,int value,Event *event)
{ /* **/
 int result;
 DBUG_ENTER("sr_replace_proc");

 puts("sr_replace_proc");

 if (GetSRValues(1,&SR,&(SR.values))==1) {
  result=rcmd_SearchReplace(&(SR.values));

 printf("Result = %d Search <%s> replace <%s> range = %d number = %d mode =%d\n",
  result,
  SR.values.search,
  SR.values.replace,
  SR.values.range,
  SR.values.number,
  SR.values.mode
 ); fflush(stdout);

  if (result==0) {
   xv_set(SR.frame,FRAME_CMD_PIN_STATE,FRAME_CMD_PIN_OUT,NULL);
  } else {
   if ((result&1)==1) xv_set(SR.frame,FRAME_LEFT_FOOTER,SR.values.search,NULL);
   if ((result&2)==2) xv_set(SR.frame,FRAME_RIGHT_FOOTER,SR.values.replace,NULL);
  }
 }

 DBUG_VOID_RETURN;
} /*******/

static void sr_cancel_proc(Panel_item item,int value,Event *event)
{ /* **/
 DBUG_ENTER("sr_cancel_proc");

 xv_set(SR.frame,FRAME_CMD_PIN_STATE,FRAME_CMD_PIN_OUT,NULL);

 DBUG_VOID_RETURN;
} /*******/

static void sr_mode_proc(Panel_item item,int value,Event *event)
{ /* Shows number enter object depend on search mode **/
 DBUG_ENTER("sr_mode_proc");

 xv_set(SR.pi_number,XV_SHOW,FALSE,NULL);

 switch (value) {
  case 0 :
   xv_set(SR.frame,FRAME_LEFT_FOOTER,"Replace all expressions in range",NULL);
   break;
  case 1 :
   xv_set(SR.frame,FRAME_LEFT_FOOTER,"Ask before replacement",NULL);
   break;
  case 2 :
   xv_set(SR.frame,FRAME_LEFT_FOOTER,"Replace only the next expression",NULL);
   break;
  case 3 :
   xv_set(SR.frame,FRAME_LEFT_FOOTER,"Replace next 'n' expressions",NULL);
   xv_set(SR.pi_number,XV_SHOW,TRUE,NULL);
   break;
  default : break;
 }

 DBUG_VOID_RETURN;
} /*******/

static void Create_SearchAndReplaceRequester(void)
{ /* Create a search and replace requester **/
 int x;
 DBUG_ENTER("Create_SearchAndReplaceRequester");

 SR.frame=(Frame)xv_create( /* Search and replace frame window */
  XRED.frame,
  FRAME_CMD,
  FRAME_LABEL,"Search & Replace expressions",
  FRAME_CMD_PIN_STATE,FRAME_CMD_PIN_IN,
  FRAME_SHOW_FOOTER,TRUE,
  NULL
 );

 SR.panel=(Panel)xv_get(SR.frame,FRAME_CMD_PANEL); /* Get panel */
 xv_set(SR.panel,PANEL_LAYOUT,PANEL_VERTICAL,NULL);

 SR.pi_searchtxt=(Panel_item)xv_create(
  SR.panel,
  PANEL_TEXT,
  PANEL_LABEL_STRING,"Search :",
  PANEL_VALUE,"",
  PANEL_VALUE_DISPLAY_LENGTH,SR_DISPLAY_LEN,
  PANEL_VALUE_STORED_LENGTH,SR_STORE_LEN,
  NULL
 );

 SR.pi_replacetxt=(Panel_item)xv_create(
  SR.panel,
  PANEL_TEXT,
  PANEL_LABEL_STRING,"Replace :",
  PANEL_VALUE,"",
  PANEL_VALUE_DISPLAY_LENGTH,SR_DISPLAY_LEN,
  PANEL_VALUE_STORED_LENGTH,SR_STORE_LEN,
  NULL
 );

 SR.pi_mode=(Panel_item)xv_create(
  SR.panel,
  PANEL_CHOICE,
  PANEL_LABEL_STRING,"Replace mode :",
  PANEL_CHOOSE_ONE,TRUE,
  PANEL_CHOICE_STRINGS,
   "Replace all",
   "Ask before replace",
   "Replace next",
   "Replace by counter",
   NULL,
  PANEL_NOTIFY_PROC,sr_mode_proc,
  PANEL_VALUE,0,
  NULL
 );

 SR.pi_range=(Panel_item)xv_create(
  SR.panel,
  PANEL_CHECK_BOX,
  PANEL_LABEL_STRING,"Search & Replace range :",
  PANEL_CHOICE_STRINGS,
   "Program",
   "Cursor expression",
   NULL,
  PANEL_CHOOSE_ONE,TRUE,
  NULL
 );

 xv_set(SR.panel,PANEL_LAYOUT,PANEL_HORIZONTAL,NULL);
 
 SR.pi_number=(Panel_item)xv_create(
  SR.panel,
  PANEL_NUMERIC_TEXT,
  PANEL_VALUE,1,
  PANEL_MIN_VALUE,1,
  PANEL_MAX_VALUE,9999,
  PANEL_VALUE_DISPLAY_LENGTH,8,
  XV_SHOW,(xv_get(SR.pi_mode,PANEL_VALUE)==3),
  NULL
 );

 xv_set(SR.panel,PANEL_LAYOUT,PANEL_VERTICAL,NULL);

 SR.pi_search=(Panel_item)xv_create(
  SR.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Search",
  PANEL_NOTIFY_PROC,sr_search_proc,
  NULL
 );

 xv_set(SR.panel,PANEL_LAYOUT,PANEL_HORIZONTAL,NULL);

 SR.pi_replace=(Panel_item)xv_create(
  SR.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Replace",
  PANEL_NOTIFY_PROC,sr_replace_proc,
  NULL
 );

 SR.pi_cancel=(Panel_item)xv_create(
  SR.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Cancel",
  PANEL_NOTIFY_PROC,sr_cancel_proc,
  NULL
 );

 window_fit(SR.panel);

 x = xv_get(SR.panel,XV_WIDTH)
   - xv_get(SR.pi_search,XV_WIDTH)
   - xv_get(SR.pi_replace,XV_WIDTH)
   - xv_get(SR.pi_cancel,XV_WIDTH)
   - (3 * xv_get(SR.panel,PANEL_ITEM_X_GAP));

 x = x/2; 

 xv_set(SR.pi_search,XV_X,x,NULL);
 x+=(xv_get(SR.pi_search,XV_WIDTH)+xv_get(SR.panel,PANEL_ITEM_X_GAP));
 xv_set(SR.pi_replace,XV_X,x,NULL);
 x+=(xv_get(SR.pi_replace,XV_WIDTH)+xv_get(SR.panel,PANEL_ITEM_X_GAP));
 xv_set(SR.pi_cancel,XV_X,x,NULL);

 window_fit(SR.frame);

 SR.values.search=(char *)malloc(256);
 SR.values.replace=(char *)malloc(256);

 DBUG_VOID_RETURN;
} /*******/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

int mf_ConfirmAction(Panel panel,char *text,Event *event)
{ /* Popup ask requester **/
 DBUG_ENTER("mf_ConfirmAction");

 DBUG_RETURN (NOTICE_YES==notice_prompt(
  panel,NULL,
  NOTICE_FOCUS_XY,event_x(event),event_y(event),
  NOTICE_MESSAGE_STRINGS,text,NULL,
  NOTICE_BUTTON_YES,"Yes",
  NOTICE_BUTTON_NO,"No",
  NULL
 ));

} /*******/

int mf_ConfirmActionWait(char *text,char *yes,char *no)
{ /* **/
 Xv_notice notice;
 int answer=0;
 DBUG_ENTER("mf_ConfirmActionWait");

 notice=(Xv_notice)xv_create(
  XRED.frame,
  NOTICE,
  NOTICE_MESSAGE_STRING,text,
  NOTICE_BUTTON_YES,yes,
  NOTICE_BUTTON_NO,no,
  NOTICE_STATUS,&answer,
  NOTICE_NO_BEEPING,TRUE,
  XV_SHOW,TRUE,
  NULL
 );

 answer=(answer==NOTICE_YES);
 xv_destroy_safe(notice);

 DBUG_RETURN (answer);
} /******/

int mf_Search_and_Replace(void)
{ /* Popup search and replace Requester (Only one possible) **/
 DBUG_ENTER("mf_Search_and_Replace");

 if (SR_INIT_NOT_DONE) {
  Create_SearchAndReplaceRequester();
  SR_INIT_NOT_DONE=0;
 }
 xv_set(SR.frame,XV_SHOW,TRUE,NULL);

 DBUG_RETURN (1); 
} /***/

void mf_SetSyntaxEditorMode(int mode)
{ /* Setup syntaxeditor mode **/
 DBUG_ENTER("mf_SetSyntaxEditorMode");

 XRED.syntax_tty_state=mode;
 SetSyntaxEditorMode(mode);

 DBUG_VOID_RETURN;
} /*******/
