/**********************************************************************************
***********************************************************************************
**
**   File        : Help-fcts.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 21.12.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This module implements some help functions.
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

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/notice.h>
#include <xview/textsw.h>

#include "Menu-fcts.h"
#include "Reduma-cmd.h"
#include "XReduma.h"
#include "Help-fct.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct NameRequestTag {
 Frame frame;
 Panel panel;
 Panel_item pi_name;
} NameRequest;

typedef struct HelpViewerStructTag {
 Frame frame; 
 Panel panel;
 Textsw textsw;
 Panel_item p_1;
 Panel_item p_2;
 Panel_item p_3;
 Panel_item p_4;
 Panel_item p_5;
 Panel_item p_6;
 Panel_item p_7;
 Panel_item p_8;
 Panel_item p_9;
 Panel_item p_close;
 Panel_item p_search;
 Panel_item p_next;
} HelpViewerStruct;

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define HELP_KEY	100

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

extern void textsw_normalize_view();
extern void textsw_set_selection();

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char HStr[512];
static HelpViewerStruct HV;
static NameRequest NR;

static int Active = 0;
static int Request=0;

static char LastSearch[255];
static Textsw_index SearchStart;

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

void SetHelpPos(int value)
{ /* **/
 Textsw_index first,last;
 static char BUF[128];

 first = (Textsw_index)xv_get(HV.textsw,TEXTSW_FIRST);

 switch (value) {
  case 1  : strcpy(BUF,"^editor");break;
  case 2  : strcpy(BUF,"^led");break;
  case 3  : strcpy(BUF,"^keys");break;
  case 4  : strcpy(BUF,"^cmd");break;
  case 5  : strcpy(BUF,"^sys");break;
  case 6  : strcpy(BUF,"^files");break;
  case 7  : strcpy(BUF,"^syntax");break;
  case 8  : strcpy(BUF,"^reduma");break;
  case 9  : strcpy(BUF,"^index");break;
  default : strcpy(BUF,"^help");break;
 }

 if (-1!=textsw_find_bytes(
           HV.textsw,
           &first,
           &last,
           BUF,
           strlen(BUF),
           0
          )) {
  textsw_normalize_view(HV.textsw,first);
  textsw_scroll_lines(HV.textsw,3);
 } else {
  window_bell(HV.frame); 
 }

} /*******/

static void Next_proc(Panel_item item,Event *event)
{ /* **/
 Textsw_index first,last;

 if (strlen(LastSearch)==0) {
  window_bell(HV.frame); 
  return;
 }

 first = SearchStart;
 if (-1!=textsw_find_bytes(
          HV.textsw,
          &first,
          &last,
          LastSearch,
          strlen(LastSearch),
          0
         )) {
   textsw_normalize_view(HV.textsw,first);
   textsw_set_selection(HV.textsw,first,last,1);
   SearchStart = last;
 } else {
  SearchStart = (Textsw_index)xv_get(HV.textsw,TEXTSW_FIRST);
  window_bell(HV.frame); 
 }

} /******/

static void EnterName_proc(Panel_item item,Event *event)
{ /* **/
 char *value;
 Textsw_index first,last;
 
 value=(char *)xv_get(NR.pi_name,PANEL_VALUE);

 if (value) {

  if (strcmp(value,LastSearch)) {
   first = (Textsw_index)xv_get(HV.textsw,TEXTSW_FIRST);
  } else {
   first = SearchStart;
  }
  strcpy(LastSearch,value);

  if (-1!=textsw_find_bytes(
           HV.textsw,
           &first,
           &last,
           value,
           strlen(value),
           0
          )) {
   textsw_normalize_view(HV.textsw,first);
   textsw_set_selection(HV.textsw,first,last,1);
   SearchStart = last;
  }
 } else {
  window_bell(HV.frame); 
 }
 Request = 0;
 xv_destroy_safe(NR.frame);

} /******/

static void search_hv_proc(Panel_item item,Event *event)
{ /* **/

 if (Request) return;

 Request = 1;
 NR.frame=(Frame)xv_create(
   XV_NULL,
   FRAME,
   FRAME_LABEL,"Search request",
   NULL
 );

 NR.panel=(Panel)xv_create(
  NR.frame,
  PANEL,
  NULL
 );

 NR.pi_name=(Panel_item)xv_create(
  NR.panel,
  PANEL_TEXT,
  PANEL_LABEL_STRING,"Please searchtext :",
  PANEL_VALUE_DISPLAY_LENGTH,30,
  PANEL_VALUE_STORED_LENGTH,50,
  PANEL_NOTIFY_PROC,EnterName_proc,
  NULL
 );

 window_fit(NR.panel);
 window_fit(NR.frame);

 xv_set(NR.frame,XV_SHOW,TRUE,NULL);

} /*******/

static void close_hv_proc(Panel_item item,Event *event)
{ /* **/
 Active = 0;
 xv_destroy_safe(HV.frame);
} /******/

static void help_proc(Panel_item item,Event *event)
{ /* **/
 SetHelpPos((int)xv_get(item,XV_KEY_DATA,HELP_KEY));
} /******/

static void CreateHelpViewer(void)
{ /* Creates a read-only textedit frame **/

 HV.frame = (Frame)xv_create(
  XV_NULL,
  FRAME,
  FRAME_LABEL,"Help --- Information about the syntax direct editor",
  FRAME_SHOW_FOOTER,TRUE, /* for messages */
  FRAME_NO_CONFIRM,TRUE,/* Show quit confirm requester */
  XV_WIDTH,600,
  XV_HEIGHT,400,
  XV_SHOW,TRUE, /* Do not show yet */
  NULL
 );

 HV.panel = (Panel)xv_create(
  HV.frame,
  PANEL,                                                         /* Type */
  PANEL_BORDER,TRUE,
  XV_X,0,                                                     /* Left corner */
  XV_Y,0,                                                    /* Upper side */
  XV_WIDTH,WIN_EXTEND_TO_EDGE,
  NULL                                                           /* End of list */
 );

 HV.p_1 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_1_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,1,
  NULL
 );
 HV.p_2 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_2_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,2,
  NULL
 );
 HV.p_3 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_3_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,3,
  NULL
 );
 HV.p_4 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_4_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,4,
  NULL
 );
 HV.p_8 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_8_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,8,
  NULL
);
 HV.p_5 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_5_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,5,
  NULL
 );
 HV.p_6 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_6_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,6,
  NULL
 );
 HV.p_7 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_7_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,7,
  NULL
 );
 HV.p_9 = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,MM_HELP_9_TEXT,
  PANEL_NOTIFY_PROC,help_proc,
  XV_KEY_DATA,HELP_KEY,9,
  NULL
 );
 HV.p_search = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Search...",
  PANEL_NOTIFY_PROC,search_hv_proc,
  NULL
 );
 HV.p_next = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Next",
  PANEL_NOTIFY_PROC,Next_proc,
  NULL
 );
 HV.p_close = (Panel_item)xv_create(
  HV.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Close",
  PANEL_NOTIFY_PROC,close_hv_proc,
  NULL
 );

 window_fit_height(HV.panel);

 HV.textsw = (Textsw)xv_create(
  HV.frame,
  TEXTSW,
  NULL
 );

 window_fit(HV.frame); /* Adjust frame sizes */

} /********/

static int InitHelpFile(void)
{ /* Init Help module and loads the file **/
 char *ptr;

 if ((ptr=getenv("FHELPFILE"))!=NULL) {
  strcpy(HStr,ptr);
  return (1);
 }

 return (0);
} /*******/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void hf_ShowHelpViewer(int value)
{ /* PopHelpviewer frame **/

 if (Active) {
  xv_set(HV.frame,XV_SHOW,TRUE,NULL);
  SetHelpPos(value);
  return;
 }

 if (InitHelpFile()) {
  Textsw_status status;                                          /* File status */
  char *tmp=(char *)strdup(HStr);

  CreateHelpViewer();
  xv_set(                                              /* Setup texfile field */
   HV.textsw,
   TEXTSW_STATUS, &status,
   TEXTSW_FILE,tmp,
   TEXTSW_FIRST,0, /* Set cursor to first line */
   TEXTSW_READ_ONLY,TRUE,
   TEXTSW_DISABLE_LOAD,TRUE,
   NULL
  );

  if (status!=TEXTSW_STATUS_OKAY) { /* Test file loading state */
   window_bell(XRED.frame);
   XR_MessageDisplay("RCan not load helpfile !");
   close_hv_proc(XV_NULL,NULL);
  } else {
   Active = 1;
   SetHelpPos(value);
   strcpy(LastSearch,"");
   SearchStart = (Textsw_index)xv_get(HV.textsw,TEXTSW_FIRST);
  }

  free(tmp);

 }
 
} /****/
