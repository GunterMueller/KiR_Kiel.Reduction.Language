/**********************************************************************************
***********************************************************************************
**
**   File        : Properties-fcts.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 5.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This module implements reduma property setups.
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

#include "dbug.h"

#include "XReduma.h"
#include "Properties-create.h"
#include "Properties-fcts.h"
#include "Properties-ard.h"

#include <xview/font.h>
#include <xview/notice.h>

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define _KBYTE	1024
#define _MBYTE	(1024*_KBYTE)

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

#define QSTACKSIZE (int)xv_get(SetupProps.SS.SS_qstack,PANEL_VALUE)
#define ISTACKSIZE (int)xv_get(SetupProps.SS.SS_istack,PANEL_VALUE)
#define MSTACKSIZE (int)xv_get(SetupProps.SS.SS_mstack,PANEL_VALUE)

#define _HEAPSIZE_ (int)xv_get(SetupProps.HS.HS_size,PANEL_VALUE)
#define _HEAPDESCR_ (int)xv_get(SetupProps.HS.HS_nr_descr,PANEL_VALUE)

#define IS_HEAP_TO_SMALL (TotalHeapSize<MIN_FREE_HEAP)

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char BUFFER[256];
static long TotalStackSize;
static long TotalHeapSize;
static long ActHeapSize=DEF_HS_SIZE;
static long ActHeapDescr=DEF_HS_NRDESCR;

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

SystemProperties SetupProps; /* Setup handle */

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static int AskDoYouReally(Panel panel,char *text,Event *event)
{ /* Popup ask requester **/
 DBUG_ENTER("AskDoYouReally");
 sprintf(BUFFER,"Please confirm your action !\n%s\nDo you want to continue ?",text);
 DBUG_RETURN (notice_prompt(
  panel,NULL,
  NOTICE_FOCUS_XY,event_x(event),event_y(event),
  NOTICE_MESSAGE_STRINGS,BUFFER,NULL,
  NOTICE_BUTTON_YES,"Yes",
  NOTICE_BUTTON_NO,"No",
  NULL
 ));
} /*******/

static void resize_text_item(Panel panel, Panel_item text_item)
{
 Xv_Font font;
 int width;
 int n;
 DBUG_ENTER("resize_text_item");

 if (panel == XV_NULL || text_item == XV_NULL)
  DBUG_VOID_RETURN;

 /* Set the display width of the fillin field to extend to the
  * right edge of the panel. 
  */
 width = (int)xv_get(panel, XV_WIDTH) -
  (int)xv_get(text_item, PANEL_VALUE_X) -
  (int)xv_get(panel, PANEL_ITEM_X_GAP);

 font = (Xv_Font)xv_get(panel, XV_FONT);
 n = width / (int)xv_get(font, FONT_DEFAULT_CHAR_WIDTH);

 /*
  * Make sure it gets no smaller than 5 characters and no larger
  * than the stored length.
  */
 if (n < 5)
  n = 5;
 else if (n > (int)xv_get(text_item, PANEL_VALUE_STORED_LENGTH))
  n = (int)xv_get(text_item, PANEL_VALUE_STORED_LENGTH);

 xv_set(text_item, 
  PANEL_VALUE_DISPLAY_LENGTH, n,
  NULL);

 DBUG_VOID_RETURN;
} /******/

static void SetBytesBuffer(long bytes)
{ /* Calculates bytes to string **/
 DBUG_ENTER("SetBytesBuffer");
 if (bytes/_MBYTE>0) { /* Size in Megabytes */
 sprintf(BUFFER,"%d.%3d Mbytes",(int)(bytes/_MBYTE),(int)((bytes%_MBYTE)*1000/_MBYTE));
 } else if (bytes/_KBYTE>0) { /* Size in kbytes */
 sprintf(BUFFER,"%d.%3d Kbytes",(int)(bytes/_KBYTE),(int)((bytes%_KBYTE)*1000/_KBYTE));
 } else { /* Size in bytes */
  sprintf(BUFFER,"%ld bytes",bytes);
 }

 DBUG_VOID_RETURN;
} /******/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void CalcTotalStackSize(void)
{ /* Calc total stacksize for actual stack sizes **/
 DBUG_ENTER("CalcTotalStackSize");
 if (XRED.RED_machine==REDTYP_INTER) { /* Lneu (interpreterversion) ? */
  TotalStackSize = LNEU_NR_OF_ISTACKS*ISTACKSIZE*STACKELEM_SIZE
                 + LNEU_NR_OF_MSTACKS*MSTACKSIZE*STACKELEM_SIZE
                 + LNEU_NR_OF_QSTACKS*QSTACKSIZE*STACKELEM_SIZE;
 } else if (XRED.RED_machine==REDTYP_COMP) { /* Lred (compilerversion) ? */
  TotalStackSize = LRED_NR_OF_ISTACKS*ISTACKSIZE*STACKELEM_SIZE
                 + LRED_NR_OF_MSTACKS*MSTACKSIZE*STACKELEM_SIZE
                 + LRED_NR_OF_QSTACKS*MSTACKSIZE*STACKELEM_SIZE;
 } else { /* dummy settings */
  TotalStackSize = 1*QSTACKSIZE*STACKELEM_SIZE
                 + 1*ISTACKSIZE*STACKELEM_SIZE
                 + 1*MSTACKSIZE*STACKELEM_SIZE;
 }
 SetBytesBuffer(TotalStackSize);
 xv_set(SetupProps.SS.SS_mem,PANEL_VALUE,BUFFER,NULL);

 DBUG_VOID_RETURN;
} /******/

void CalcTotalHeapSize(int mode)
{ /* Calc total heapsize for actual heap sizes **/
 DBUG_ENTER("CalcTotalHeapSize");
  if (mode) { /* Get actuall values ? */
   ActHeapSize=SetupProps.HS.size_value;
   ActHeapDescr=SetupProps.HS.nr_descr_value;
  }
  TotalHeapSize = ActHeapSize
                - ActHeapDescr*HEAPDESCRITOR_SIZE;
 SetBytesBuffer(TotalHeapSize);
 xv_set(SetupProps.HS.HS_mem,PANEL_VALUE,BUFFER,NULL);

 DBUG_VOID_RETURN;
} /******/

void justify_items(Panel panel, int resize)
{ /* Arrange panel items dependent on their text size **/
 register Panel_item item;
 register int  value_x;
 register Panel_item_type class;
 Xv_Font   font = XV_NULL;
 Font_string_dims font_size;
 int   longest = 0;
 char   *string;
 DBUG_ENTER("justify_items");

 if (panel == XV_NULL)
  DBUG_VOID_RETURN;

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

 value_x = longest + 2 * (int)xv_get(panel, PANEL_ITEM_X_GAP);

 /* Layout each item (except buttons) on the panel */
 PANEL_EACH_ITEM(panel, item) {
  if ((int)xv_get(item, PANEL_SHOW_ITEM) &&
      ((class = (Panel_item_type)xv_get(item, PANEL_ITEM_CLASS))
        != PANEL_BUTTON_ITEM)) {
   xv_set(item, 
    PANEL_VALUE_X, value_x, 
    NULL);
   if (resize && class == PANEL_TEXT_ITEM) {
    resize_text_item(panel, item);
   }
  }
 }
 PANEL_END_EACH;


 DBUG_VOID_RETURN;
} /********/

void switch_category(int value,int show)
{ /* Switch property category to ... **/
 PropStandard *ps=NULL;
 Panel cpanel; /* Used category panel */
 Frame cframe; /* Property frame to use */
 Panel_button_item *capply; /* Pointer to global apply button */
 void (*call_func)();
 DBUG_ENTER("switch_category");

 if (value>=XPROPERTY_ADD) {  /* use Xproperties ? */
  cpanel=SetupProps.xpanel; 
  cframe=SetupProps.xframe;
  capply=&(SetupProps.xapply);
 } else {
  cpanel=SetupProps.panel; 
  cframe=SetupProps.frame;
  capply=&(SetupProps.apply);
 }
 
 switch (value) {
  case 0 : /* Number format */
   ps=&SetupProps.PNF.ps;
   break;
  case 1 : /* Calculation precison */
   ps=&SetupProps.CP.ps;
   break;
  case 2 : /* Stack size */
   ps=&SetupProps.SS.ps;
   break;
  case 3 : /* Heap size */
   ps=&SetupProps.HS.ps;
   break;
  case 4 : /* Misc setups */
   ps=&SetupProps.MI.ps;
   break;
  case 5 : /* Fileextender setups */
   ps=&SetupProps.FE.ps;
   break;
  case PROP_SYNTAXEDITOR : /* First XWindows requester : Syntaxeditor setup */
   ps=&SetupProps.SE.ps;
   break;
  case PROP_STANDARD : /* Second Property requester : Button behavior */
   ps=&SetupProps.PP.ps;
   break;
  default : fprintf(stderr, "Invalid category in switch_category.\n");
 }

 if (ps==NULL) DBUG_VOID_RETURN; /* No category selected */

 if (show==TRUE) { /* Show this panel ? */
  int height,width;
  width=xv_get(ps->panel,XV_WIDTH); /* Get panel size */
  xv_set(cpanel,XV_WIDTH,width,NULL); /* Set window panel size */
  xv_set(ps->panel,XV_SHOW,TRUE,NULL); /* Show this panel */
  height=xv_get(ps->panel,XV_HEIGHT)+xv_get(cpanel,XV_HEIGHT);
  xv_set( /* Adjust property window size */
   cframe,
   XV_WIDTH,width,
   XV_HEIGHT,height,
   NULL
  );
  *capply=ps->apply; /* Set default apply function for property */
 } else { /* Disable panel */
  call_func=(void (*)())xv_get(ps->apply,PANEL_NOTIFY_PROC);
  call_func();
  xv_set(ps->panel,XV_SHOW,FALSE,NULL); /* Hide panel */ 
 }


 DBUG_VOID_RETURN;
} /********/

static int oldvalue=0; /* Last used pi-red properties category */

void Property_category_proc(void)
{ /* Enable property category **/
 int value;
 DBUG_ENTER("Property_category_proc");
 value=xv_get(SetupProps.category,PANEL_VALUE);
 if (value!=oldvalue) { /* New property menu ? */
  switch_category(oldvalue, FALSE);
  switch_category(value, TRUE);
  oldvalue=value;
 }
 DBUG_VOID_RETURN;
} /**********/

void Set_Property_category(int value)
{ /* Enable property category **/
 DBUG_ENTER("Set_Property_category");
 if (value!=oldvalue) { /* New property menu ? */
  switch_category(oldvalue, FALSE);
  switch_category(value, TRUE);
  oldvalue=value;
 }
 DBUG_VOID_RETURN;
} /**********/

void XProperty_category_proc(void)
{ /* Enable XWindows property category **/
 int value;
 static int oldvalue=XPROPERTY_ADD;
 DBUG_ENTER("XProperty_category_proc");
 value=XPROPERTY_ADD+xv_get(SetupProps.xcategory,PANEL_VALUE);
 if (value!=oldvalue) { /* New property menu ? */
  switch_category(oldvalue, FALSE);
  switch_category(value, TRUE);
  oldvalue=value;
 }
 DBUG_VOID_RETURN;
} /**********/

void CP_truncUL_proc(Panel_item item,int value,Event *event)
{
 DBUG_ENTER("CP_truncUL_proc");
 xv_set(SetupProps.CP.PI_trunc,PANEL_INACTIVE,(value) ? TRUE : FALSE,NULL);

 DBUG_VOID_RETURN;
} /******/

void CP_multUL_proc(Panel_item item,int value,Event *event)
{
 DBUG_ENTER("CP_multUL_proc");
 xv_set(SetupProps.CP.PI_mult,PANEL_INACTIVE,(value) ? TRUE : FALSE,NULL);

 DBUG_VOID_RETURN;
} /******/

void CP_divUL_proc(Panel_item item,int value,Event *event)
{
 DBUG_ENTER("CP_divUL_proc");
 xv_set(SetupProps.CP.PI_div,PANEL_INACTIVE,(value) ? TRUE : FALSE,NULL);

 DBUG_VOID_RETURN;
} /******/

void PNF_number_format_proc(Panel_item item,int value,Event *event)
{
 DBUG_ENTER("PNF_number_format_proc");
 xv_set(
  SetupProps.PNF.basenumber,
  PANEL_INACTIVE,(value==PNF_VAR_FORMAT) ? TRUE : FALSE,
  NULL
 );

 DBUG_VOID_RETURN;
} /*********/

char *ExpandDir(char *dir)
{ /* **/
 DBUG_ENTER("ExpandDir");
 if (dir!=NULL) {
  static char BUFFER[256];
  char *tmp=dir;
  char *bp=BUFFER;
  while (*tmp!='\0') {
   if (*tmp=='$') {
    char dollar[32]; 
    char *s;
    int i=0;
    tmp++;
    while ((*tmp!='\0')&&(*tmp!='/')) {
     dollar[i]=*tmp;
     i++;
     tmp++;
    }
    dollar[i]='\0';
    s=getenv(dollar);
    if (s!=NULL) {
     strcpy(bp,s);
     bp+=(strlen(s));
    }
   } else {
    *bp=*tmp;
    bp++;tmp++;
   }
  }
  *bp='\0';
 DBUG_RETURN (BUFFER);
 }
 DBUG_RETURN (NULL);
} /*****/

void FE_menu_proc(Menu menu,Menu_item item)
{ /* **/
 Panel_item pi;
 char *dir=NULL;
 DBUG_ENTER("FE_menu_proc");

 xv_set(menu,XV_INCREMENT_REF_COUNT,TRUE,NULL);

 switch (xv_get(item,MENU_VALUE)) {
  case ed_SELECT  : pi=SetupProps.FE.FE_ed;break;
  case ed_DIR1    : pi=SetupProps.FE.FE_ed;dir=DEF_FE_ED_PATH1;break;
  case ed_DIR2    : pi=SetupProps.FE.FE_ed;dir=DEF_FE_ED_PATH2;break;

  case red_SELECT : pi=SetupProps.FE.FE_red;break;
  case red_DIR1   : pi=SetupProps.FE.FE_red;dir=DEF_FE_RED_PATH1;break;
  case red_DIR2   : pi=SetupProps.FE.FE_red;dir=DEF_FE_RED_PATH2;break;

  case pp_SELECT  : pi=SetupProps.FE.FE_pp;break;
  case pp_DIR1    : pi=SetupProps.FE.FE_pp;dir=DEF_FE_PP_PATH1;break;
  case pp_DIR2    : pi=SetupProps.FE.FE_pp;dir=DEF_FE_PP_PATH2;break;

  case asc_SELECT : pi=SetupProps.FE.FE_asc;break;
  case asc_DIR1   : pi=SetupProps.FE.FE_asc;dir=DEF_FE_ASC_PATH1;break;
  case asc_DIR2   : pi=SetupProps.FE.FE_asc;dir=DEF_FE_ASC_PATH2;break;

  case doc_SELECT : pi=SetupProps.FE.FE_doc;break;
  case doc_DIR1   : pi=SetupProps.FE.FE_doc;dir=DEF_FE_DOC_PATH1;break;
  case doc_DIR2   : pi=SetupProps.FE.FE_doc;dir=DEF_FE_DOC_PATH2;break;

  case prt_SELECT : pi=SetupProps.FE.FE_prt;break;
  case prt_DIR1   : pi=SetupProps.FE.FE_prt;dir=DEF_FE_PRT_PATH1;break;
  case prt_DIR2   : pi=SetupProps.FE.FE_prt;dir=DEF_FE_PRT_PATH2;break;

  case pre_SELECT : pi=SetupProps.FE.FE_pre;break;
  case pre_DIR1   : pi=SetupProps.FE.FE_pre;dir=DEF_FE_PRE_PATH1;break;
  case pre_DIR2   : pi=SetupProps.FE.FE_pre;dir=DEF_FE_PRE_PATH2;break;

  default         : DBUG_VOID_RETURN;
 }

 xv_set(pi,PANEL_VALUE,ExpandDir(dir),NULL);


 DBUG_VOID_RETURN;
} /*********/

void props_apply_proc(Panel_item item,Event *event)
{ /* Set apply values **/
 Panel panel=XV_NULL;
 void (*apply_func)();
 DBUG_ENTER("props_apply_proc");

 if (SetupProps.PNF.ps.apply==item) {
  apply_func=Apply_NumberFormat;
  panel=SetupProps.PNF.ps.panel;
 } else if (SetupProps.CP.ps.apply==item) {
  apply_func=Apply_CalcPrecison;
  panel=SetupProps.CP.ps.panel;
 } else if (SetupProps.SS.ps.apply==item) {
  apply_func=Apply_StackSize;
  panel=SetupProps.SS.ps.panel;
 } else if (SetupProps.HS.ps.apply==item) {
  apply_func=Apply_HeapSize;
  panel=SetupProps.HS.ps.panel;
 } else if (SetupProps.MI.ps.apply==item) {
  apply_func=Apply_Misc;
  panel=SetupProps.MI.ps.panel;
 } else if (SetupProps.FE.ps.apply==item) {
  apply_func=Apply_FileExtender;
  panel=SetupProps.FE.ps.panel;
 } else if (SetupProps.PP.ps.apply==item) {
  apply_func=Apply_PropertyButtons;
  panel=SetupProps.PP.ps.panel;
 } else if (SetupProps.SE.ps.apply==item) {
  apply_func=Apply_SyntaxEditor;
  panel=SetupProps.SE.ps.panel;
 };

 if (panel!=XV_NULL) {
  if (ASKBEFORE_APPLY_FRAME) {
   if (AskDoYouReally(panel,"Apply changes to actual settings",event)==NOTICE_YES) {
    apply_func(TRUE);
   }
  } else apply_func(TRUE);
  if (DISMISS_APPLY_FRAME) {
   xv_set(item,PANEL_NOTIFY_STATUS,XV_ERROR,NULL); /* Not dismiss frame */
  }
 }

 DBUG_VOID_RETURN;
} /***/

void props_close_proc(Panel_item item,Event *event) {}

void props_reset_proc(Panel_item item,Event *event)
{ /* Reset values **/
 Panel panel=XV_NULL;
 void (*reset_func)();
 DBUG_ENTER("props_reset_proc");

 if (SetupProps.PNF.ps.reset==item) {
  reset_func=Reset_NumberFormat;
  panel=SetupProps.PNF.ps.panel;
 } else if (SetupProps.CP.ps.reset==item) {
  reset_func=Reset_CalcPrecison;
  panel=SetupProps.CP.ps.panel;
 } else if (SetupProps.SS.ps.reset==item) {
  reset_func=Reset_StackSize;
  panel=SetupProps.SS.ps.panel;
 } else if (SetupProps.HS.ps.reset==item) {
  reset_func=Reset_HeapSize;
  panel=SetupProps.HS.ps.panel;
 } else if (SetupProps.MI.ps.reset==item) {
  reset_func=Reset_Misc;
  panel=SetupProps.MI.ps.panel;
 } else if (SetupProps.FE.ps.reset==item) {
  reset_func=Reset_FileExtender;
  panel=SetupProps.FE.ps.panel;
 } else if (SetupProps.SE.ps.reset==item) {
  reset_func=Reset_SyntaxEditor;
  panel=SetupProps.SE.ps.panel;
 } else if (SetupProps.PP.ps.reset==item) {
  reset_func=Reset_PropertyButtons;
  panel=SetupProps.PP.ps.panel;
 };

 if (panel!=XV_NULL) {
  if (ASKBEFORE_RESET_FRAME) 
   if (AskDoYouReally(panel,"Reset changes to last applied",event)!=NOTICE_YES)
    DBUG_VOID_RETURN;
   reset_func(TRUE);
   if (DISMISS_RESET_FRAME)
    xv_set(item,PANEL_NOTIFY_STATUS,XV_ERROR,NULL); /* Not dismiss frame */
 }

 DBUG_VOID_RETURN;
} /***/

void props_defaults_proc(Panel_item item,Event *event)
{ /* Set default values **/
 Panel panel=XV_NULL;
 void (*defaults_func)();
 DBUG_ENTER("props_defaults_proc");

 if (SetupProps.PNF.ps.defaults==item) {
  defaults_func=SetDefaults_NumberFormat;
  panel=SetupProps.PNF.ps.panel;
 } else if (SetupProps.CP.ps.defaults==item) {
  defaults_func=SetDefaults_CalcPrecison;
  panel=SetupProps.CP.ps.panel;
 } else if (SetupProps.SS.ps.defaults==item) {
  defaults_func=SetDefaults_StackSize;
  panel=SetupProps.SS.ps.panel;
 } else if (SetupProps.HS.ps.defaults==item) {
  defaults_func=SetDefaults_HeapSize;
  panel=SetupProps.HS.ps.panel;
 } else if (SetupProps.MI.ps.defaults==item) {
  defaults_func=SetDefaults_Misc;
  panel=SetupProps.MI.ps.panel;
 } else if (SetupProps.FE.ps.defaults==item) {
  defaults_func=SetDefaults_FileExtender;
  panel=SetupProps.FE.ps.panel;
 } else if (SetupProps.SE.ps.defaults==item) {
  defaults_func=SetDefaults_SyntaxEditor;
  panel=SetupProps.SE.ps.panel;
 } else if (SetupProps.PP.ps.defaults==item) {
  defaults_func=SetDefaults_PropertyButtons;
  panel=SetupProps.PP.ps.panel;
 };

 if (panel!=XV_NULL) {
  if (ASKBEFORE_DEFAULTS_FRAME) 
   if (AskDoYouReally(panel,"Set settings to defaults",event)!=NOTICE_YES)
    DBUG_VOID_RETURN;
   defaults_func(TRUE);
   if (DISMISS_DEFAULTS_FRAME)
    xv_set(item,PANEL_NOTIFY_STATUS,XV_ERROR,NULL); /* Not dismiss frame */
 }

 DBUG_VOID_RETURN;
} /***/

/* Notifier functions for stack size setups **/
void SS_stack_proc(Panel_item item,int value,Event *event)
{ /* Actuallize stack sizes **/
 DBUG_ENTER("SS_stack_proc");
 CalcTotalStackSize();

 DBUG_VOID_RETURN;
} /****/

/* Notifier functions for heap size setups **/

void HS_nr_descr_proc(Panel_item item,int value,Event *event)
{ /* Actuallize heapdescriptor number **/
 DBUG_ENTER("HS_nr_descr_proc");
 ActHeapDescr=DEF_NR_DESCR_FACTOR*value;
 CalcTotalHeapSize(0);
 if (IS_HEAP_TO_SMALL) { /* Heap to small ? */
  ActHeapSize = (ActHeapDescr*HEAPDESCRITOR_SIZE) + MIN_FREE_HEAP;
  xv_set(
   SetupProps.HS.HS_size,
   PANEL_VALUE,ActHeapSize/DEF_NR_SIZE_FACTOR,
   NULL
  );
  CalcTotalHeapSize(0);
 }

 DBUG_VOID_RETURN;
} /****/

void HS_size_proc(Panel_item item,int value,Event *event)
{ /* Actuallize heap size **/
 DBUG_ENTER("HS_size_proc");
 ActHeapSize=DEF_NR_SIZE_FACTOR*value;
 CalcTotalHeapSize(0);
 if (IS_HEAP_TO_SMALL) { /* Heap to small ? */
  ActHeapDescr = (ActHeapSize - MIN_FREE_HEAP) / HEAPDESCRITOR_SIZE;
  xv_set( 
   SetupProps.HS.HS_nr_descr,
   PANEL_VALUE,ActHeapDescr/DEF_NR_DESCR_FACTOR,
   NULL
  );
  CalcTotalHeapSize(0);
 }

 DBUG_VOID_RETURN;
} /****/
