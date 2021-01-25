/**********************************************************************************
***********************************************************************************
**
**   File        : Properties.c
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

#include <xview/font.h>

#include "XReduma.h"
#include "Properties-create.h"
#include "Properties-ard.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define DEF_START_REQUESTER	0

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

#include "macros/Properties.mac"
#define AllocNameSpace(var,name) var=(char *)malloc(128);strcpy(var,name);

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char BUFFER[256];
static int  x,y;

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

int  REDUMA_PROPERTY_CREATE_DONE=0;
int  XREDUMA_PROPERTY_CREATE_DONE=0;

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void SetupCalculationProps(void)
{ /* REQUESTER : Calculation precison **/
 DBUG_ENTER("SetupCalculationProps");
 
 SetupProps.CP.ps.panel=(Panel)xv_create( /* Calculation precison panel */
  SetupProps.frame, /* Parent */
  PANEL,
  MY_PANEL_BORDER,TRUE,
  PANEL_LAYOUT,PANEL_VERTICAL,
  XV_X,x,
  XV_Y,y,
  NULL
 );

 SetupProps.CP.PI_trunc=(Panel_item)xv_create(
  SetupProps.CP.ps.panel, /* Parent */
  PANEL_SLIDER, /* Type */
  PANEL_LABEL_STRING,"Digit truncation:",
  PANEL_MIN_VALUE,DEF_TRUNC_MIN,
  PANEL_MAX_VALUE,DEF_TRUNC_MAX,
  PANEL_TICKS,DEF_CP_TICKS,
  PANEL_SLIDER_WIDTH,DEF_CP_SLIDERWIDTH,
  PANEL_SHOW_RANGE,TRUE,
  PANEL_INACTIVE,SetupProps.CP.trunc_value<0,
  PANEL_VALUE,SetupProps.CP.trunc_value,
  NULL
 );

 SetupProps.CP.PI_trunc_l=(Panel_item)xv_create(
  SetupProps.CP.ps.panel, /* Parent */
  PANEL_CHECK_BOX, /* Type */
  PANEL_CHOICE_STRINGS,"No digit truncation",NULL,
  PANEL_VALUE,SetupProps.CP.trunc_value<0,
  PANEL_NOTIFY_PROC,CP_truncUL_proc,
  NULL
 );

 SetupProps.CP.PI_mult=(Panel_item)xv_create(
  SetupProps.CP.ps.panel, /* Parent */
  PANEL_SLIDER, /* Type */
  PANEL_LABEL_STRING,"Multiplication precison:",
  PANEL_MIN_VALUE,DEF_MULT_MIN,
  PANEL_MAX_VALUE,DEF_MULT_MAX,
  PANEL_TICKS,DEF_CP_TICKS,
  PANEL_SLIDER_WIDTH,DEF_CP_SLIDERWIDTH,
  PANEL_SHOW_RANGE,TRUE,
  PANEL_INACTIVE,SetupProps.CP.mult_value<0,
  PANEL_VALUE,SetupProps.CP.mult_value,
  NULL
 );

 SetupProps.CP.PI_mult_l=(Panel_item)xv_create(
  SetupProps.CP.ps.panel, /* Parent */
  PANEL_CHECK_BOX, /* Type */
  PANEL_CHOICE_STRINGS,"Highest multiplication precison",NULL,
  PANEL_VALUE,SetupProps.CP.mult_value<0,
  PANEL_NOTIFY_PROC,CP_multUL_proc,
  NULL
 );

 SetupProps.CP.PI_div=(Panel_item)xv_create(
  SetupProps.CP.ps.panel, /* Parent */
  PANEL_SLIDER, /* Type */
  PANEL_LABEL_STRING,"Divison precison:",
  PANEL_MIN_VALUE,DEF_DIV_MIN,
  PANEL_MAX_VALUE,DEF_DIV_MAX,
  PANEL_TICKS,DEF_CP_TICKS,
  PANEL_SLIDER_WIDTH,DEF_CP_SLIDERWIDTH,
  PANEL_SHOW_RANGE,TRUE,
  PANEL_INACTIVE,SetupProps.CP.div_value<0,
  PANEL_VALUE,SetupProps.CP.div_value,
  NULL
 );

 SetupProps.CP.PI_div_l=(Panel_item)xv_create(
  SetupProps.CP.ps.panel, /* Parent */
  PANEL_CHECK_BOX, /* Type */
  PANEL_CHOICE_STRINGS,"Highest divison precison",NULL,
  PANEL_VALUE,SetupProps.CP.div_value<0,
  PANEL_NOTIFY_PROC,CP_divUL_proc,
  NULL
 );

 justify_items(SetupProps.CP.ps.panel,FALSE);

 MAC_CREATE_AP_DEF_RES(CP);

 DBUG_VOID_RETURN;
} /***********/

static void SetupStacksizeProps(void)
{ /* REQUESTER : Stack size **/
 DBUG_ENTER("SetupStacksizeProps");
 
 SetupProps.SS.ps.panel=(Panel)xv_create( /* Stacksize panel */
  SetupProps.frame, /* Parent */
  PANEL,
  MY_PANEL_BORDER,TRUE,
  PANEL_LAYOUT,PANEL_VERTICAL,
  XV_X,x,
  XV_Y,y,
  NULL
 );

 SetupProps.SS.SS_qstack=(Panel_item)xv_create(
  SetupProps.SS.ps.panel, /* Parent */
  PANEL_SLIDER, /* Type */
  PANEL_LABEL_STRING,"Q stack size:",
  PANEL_MIN_VALUE,DEF_QSTACK_MIN,
  PANEL_MAX_VALUE,DEF_QSTACK_MAX,
  PANEL_VALUE,SetupProps.SS.qstack_value,
  PANEL_VALUE_DISPLAY_LENGTH,10,
  PANEL_TICKS,DEF_SS_TICKS,
  PANEL_SLIDER_WIDTH,DEF_SS_SLIDERWIDTH,
  PANEL_NOTIFY_PROC,SS_stack_proc,
  PANEL_SHOW_RANGE,TRUE,
  NULL
 );

 SetupProps.SS.SS_istack=(Panel_item)xv_create(
  SetupProps.SS.ps.panel, /* Parent */
  PANEL_SLIDER, /* Type */
  PANEL_LABEL_STRING,"I stack size:",
  PANEL_MIN_VALUE,DEF_ISTACK_MIN,
  PANEL_MAX_VALUE,DEF_ISTACK_MAX,
  PANEL_VALUE,SetupProps.SS.istack_value,
  PANEL_VALUE_DISPLAY_LENGTH,10,
  PANEL_TICKS,DEF_SS_TICKS,
  PANEL_SLIDER_WIDTH,DEF_SS_SLIDERWIDTH,
  PANEL_NOTIFY_PROC,SS_stack_proc,
  PANEL_SHOW_RANGE,TRUE,
  NULL
 );

 SetupProps.SS.SS_mstack=(Panel_item)xv_create(
  SetupProps.SS.ps.panel, /* Parent */
  PANEL_SLIDER, /* Type */
  PANEL_LABEL_STRING,"M stack size:",
  PANEL_MIN_VALUE,DEF_MSTACK_MIN,
  PANEL_MAX_VALUE,DEF_MSTACK_MAX,
  PANEL_VALUE,SetupProps.SS.mstack_value,
  PANEL_VALUE_DISPLAY_LENGTH,10,
  PANEL_TICKS,DEF_SS_TICKS,
  PANEL_SLIDER_WIDTH,DEF_SS_SLIDERWIDTH,
  PANEL_NOTIFY_PROC,SS_stack_proc,
  PANEL_SHOW_RANGE,TRUE,
  NULL
 );

 SetupProps.SS.SS_mem=(Panel_item)xv_create(
  SetupProps.SS.ps.panel, /* Parent */
  PANEL_TEXT,
  PANEL_LABEL_STRING,"Total memory size :",
  PANEL_VALUE,"0",
  PANEL_VALUE_DISPLAY_LENGTH,20,
  PANEL_READ_ONLY,TRUE,
  NULL
 );

 CalcTotalStackSize(); /* Set total stack size */

 justify_items(SetupProps.SS.ps.panel,FALSE);

 MAC_CREATE_AP_DEF_RES(SS);

 DBUG_VOID_RETURN;
} /***********/

static void SetupHeapsizeProps(void)
{ /* REQUESTER : Heap size **/
 DBUG_ENTER("SetupHeapsizeProps");
 
 SetupProps.HS.ps.panel=(Panel)xv_create( /* Heapsize panel */
  SetupProps.frame, /* Parent */
  PANEL,
  MY_PANEL_BORDER,TRUE,
  PANEL_LAYOUT,PANEL_VERTICAL,
  XV_X,x,
  XV_Y,y,
  NULL
 );

 sprintf(BUFFER,"Number of heap descriptor * %d:",DEF_NR_DESCR_FACTOR);

 SetupProps.HS.HS_nr_descr=(Panel_item)xv_create(
  SetupProps.HS.ps.panel, /* Parent */
  PANEL_SLIDER, /* Type */
  PANEL_LABEL_STRING,BUFFER,
  PANEL_VALUE,SetupProps.HS.nr_descr_value,
  PANEL_VALUE_DISPLAY_LENGTH,20,
  PANEL_TICKS,DEF_HS_TICKS,
  PANEL_SLIDER_WIDTH,DEF_HS_SLIDERWIDTH,
  PANEL_NOTIFY_PROC,HS_nr_descr_proc,
  PANEL_SHOW_RANGE,TRUE,
  PANEL_MIN_VALUE,DEF_NR_DESCR_MIN,
  PANEL_MAX_VALUE,DEF_NR_DESCR_MAX,
  NULL
 );

 SetupProps.HS.HS_size=(Panel_item)xv_create(
  SetupProps.HS.ps.panel, /* Parent */
  PANEL_SLIDER, /* Type */
  PANEL_LABEL_STRING,"Heapsize in Kbytes:",
  PANEL_VALUE,SetupProps.HS.size_value,
  PANEL_VALUE_DISPLAY_LENGTH,20,
  PANEL_TICKS,DEF_HS_TICKS,
  PANEL_SLIDER_WIDTH,DEF_HS_SLIDERWIDTH,
  PANEL_SHOW_RANGE,TRUE,
  PANEL_MIN_VALUE,DEF_NR_SIZE_MIN,
  PANEL_MAX_VALUE,DEF_NR_SIZE_MAX,
  PANEL_NOTIFY_PROC,HS_size_proc,
  NULL
 );

 SetupProps.HS.HS_mem=(Panel_item)xv_create(
  SetupProps.HS.ps.panel, /* Parent */
  PANEL_TEXT,
  PANEL_LABEL_STRING,"Total free heap memory :",
  PANEL_VALUE,"0",
  PANEL_VALUE_DISPLAY_LENGTH,20,
  PANEL_READ_ONLY,TRUE,
  NULL
 );

 CalcTotalHeapSize(1); /* Set total heap size */

 justify_items(SetupProps.HS.ps.panel,FALSE);
 
 MAC_CREATE_AP_DEF_RES(HS);

 DBUG_VOID_RETURN;
} /***********/

static void SetupNumberFormatProps(void)
{ /* REQUESTER : Number format **/
 DBUG_ENTER("SetupNumberFormatProps");

 SetupProps.PNF.ps.panel=(Panel)xv_create( /* Number format panel */
  SetupProps.frame, /* Parent */
  PANEL,
  MY_PANEL_BORDER,TRUE,
  PANEL_LAYOUT,PANEL_VERTICAL,
  NULL
 );

 x=xv_get(SetupProps.PNF.ps.panel,XV_X);
 y=xv_get(SetupProps.PNF.ps.panel,XV_Y);

 SetupProps.PNF.format=(Panel_item)xv_create( /* Create number format choice */
  SetupProps.PNF.ps.panel, /* Parent */
  PANEL_CHECK_BOX, /* Type */
  PANEL_CHOOSE_ONE,TRUE,
  PANEL_LABEL_STRING,"Number format :",
  PANEL_VALUE,SetupProps.PNF.numberformat,
  PANEL_CHOICE_STRINGS,"Fix format","Variable format",NULL,
  PANEL_NOTIFY_PROC,PNF_number_format_proc,
  NULL
 );

 SetupProps.PNF.basenumber=(Panel_item)xv_create( /* Create number edit field */
  SetupProps.PNF.ps.panel, /* Parent */
  PANEL_NUMERIC_TEXT, /* Type */
  PANEL_LABEL_STRING,"Base number :", 
  PANEL_INACTIVE,SetupProps.PNF.numberformat==PNF_FIX_FORMAT,
  PANEL_VALUE,SetupProps.PNF.numbervalue,
  PANEL_MIN_VALUE,DEF_BASENUMBER_MIN,
  PANEL_MAX_VALUE,DEF_BASENUMBER_MAX,
  NULL
 );

 xv_set(SetupProps.PNF.ps.panel,PANEL_LAYOUT,PANEL_VERTICAL,NULL);

 justify_items(SetupProps.PNF.ps.panel,FALSE);

 MAC_CREATE_AP_DEF_RES(PNF);

 DBUG_VOID_RETURN;
} /**********/

static void SetupMiscProps(void)
{ /* REQUESTER : Misc setups **/
 DBUG_ENTER("SetupMiscProps");

 SetupProps.MI.ps.panel=(Panel)xv_create( /* Number format panel */
  SetupProps.frame, /* Parent */
  PANEL,
  MY_PANEL_BORDER,TRUE,
  PANEL_LAYOUT,PANEL_VERTICAL,
  XV_X,x,
  XV_Y,y,
  NULL
 );

 SetupProps.MI.MI_redcnt=(Panel_item)xv_create( /* Create reductioncnt field */
  SetupProps.MI.ps.panel, /* Parent */
  PANEL_NUMERIC_TEXT, /* Type */
  PANEL_LABEL_STRING,"Reduction counter :", 
  PANEL_VALUE,SetupProps.MI.redcnt_value,
  PANEL_MIN_VALUE,DEF_REDCNT_MIN,
  PANEL_MAX_VALUE,DEF_REDCNT_MAX,
  NULL
 );

 SetupProps.MI.MI_beta=(Panel_item)xv_create( /* Create misc betacount choice */
  SetupProps.MI.ps.panel, /* Parent */
  PANEL_CHECK_BOX, /* Type */
  PANEL_LAYOUT,PANEL_VERTICAL,
  PANEL_VALUE,SetupProps.MI.beta_value,
  PANEL_CHOICE_STRINGS,"count beta-reductions",NULL,
  NULL
 );

 justify_items(SetupProps.MI.ps.panel,FALSE);

 MAC_CREATE_AP_DEF_RES(MI);

 DBUG_VOID_RETURN;
} /**********/

static void SetupFileextenderProps(void)
{ /* REQUESTER : Fileextender setups **/
 DBUG_ENTER("SetupFileextenderProps");

 SetupProps.FE.ps.panel=(Panel)xv_create( /* File extender panel */
  SetupProps.frame, /* Parent */
  PANEL,
  MY_PANEL_BORDER,TRUE,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  XV_X,x,
  XV_Y,y,
  NULL
 );

 MAC_FILE_EXTENDER_ENTRY("Editor files",
  DEF_FE_ED_PATH1,DEF_FE_ED_PATH2 ,ed);
 MAC_FILE_EXTENDER_ENTRY("Reduma stackelement files",
  DEF_FE_RED_PATH1,DEF_FE_RED_PATH2,red);
 MAC_FILE_EXTENDER_ENTRY("Pretty print files",
  DEF_FE_PP_PATH1,DEF_FE_PP_PATH2,pp);
 MAC_FILE_EXTENDER_ENTRY("Document files",
  DEF_FE_DOC_PATH1,DEF_FE_DOC_PATH2,doc);
 MAC_FILE_EXTENDER_ENTRY("Protocol files",
  DEF_FE_PRT_PATH1,DEF_FE_PRT_PATH2,prt);
 MAC_FILE_EXTENDER_ENTRY("ASCII files",
  DEF_FE_ASC_PATH1,DEF_FE_ASC_PATH2,asc);
 MAC_FILE_EXTENDER_ENTRY("Preprozessed files",
  DEF_FE_PRE_PATH1,DEF_FE_PRE_PATH2,pre);

 (void)xv_create(
   SetupProps.FE.ps.panel,
   PANEL_MESSAGE,
   PANEL_LABEL_STRING,"Default load format :",
   PANEL_LABEL_BOLD,TRUE,
   PANEL_LABEL_WIDTH,DEF_FILEPATH_WIDTH,
   NULL
  );

 SetupProps.FE.load=(Panel_item)xv_create(
  SetupProps.FE.ps.panel,
  PANEL_CHECK_BOX,
  PANEL_CHOOSE_ONE,TRUE,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"",
  PANEL_VALUE,SetupProps.FE.load_format,
  PANEL_CHOICE_STRINGS,"Editor files","Reduma files","Pretty print files",NULL,
  NULL
 );

 (void)xv_create(
   SetupProps.FE.ps.panel,
   PANEL_MESSAGE,
   PANEL_LABEL_STRING,"Default save format :",
   PANEL_LABEL_BOLD,TRUE,
   PANEL_LABEL_WIDTH,DEF_FILEPATH_WIDTH,
   NULL
  );

 SetupProps.FE.save=(Panel_item)xv_create(
  SetupProps.FE.ps.panel,
  PANEL_CHECK_BOX,
  PANEL_CHOOSE_ONE,TRUE,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"",
  PANEL_VALUE,SetupProps.FE.save_format,
  PANEL_CHOICE_STRINGS,"Editor files","Reduma files","Pretty print files",NULL,
  NULL
 );

 xv_set( SetupProps.FE.ps.panel,PANEL_LAYOUT,PANEL_VERTICAL,NULL);

 MAC_CREATE_AP_DEF_RES(FE);

 DBUG_VOID_RETURN;
} /**********/

static void CreateProperties(void)
{ /* Create a system property dialog **/
 DBUG_ENTER("CreateProperties");
 
 sprintf(BUFFER,"%s : pi-RED Properties",PROGRAMM_NAME); /* Popup name */

 SetupProps.frame=(Frame)xv_create( /* Create a layout for system setup */
  XRED.frame, /* Main window is parent */
  FRAME_PROPS, /* Type */
  XV_LABEL,BUFFER, /* Window name */
  NULL /* End of list */
 );

 SetupProps.panel=(Panel)xv_get( /* get property panel */
  SetupProps.frame, /* Parent */
  FRAME_PROPS_PANEL
 );

 SetupProps.category=(Panel_item)xv_create( /* Create property menu */
  SetupProps.panel, /* Parent */
  PANEL_CHOICE_STACK,
  PANEL_LAYOUT, PANEL_HORIZONTAL,
  PANEL_LABEL_STRING, "Category:",
  PANEL_CHOICE_STRINGS,
   "Number format",
   "Calculation precison",
   "Stack size",
   "Heap size",
   "Reduction counter",
   "Filepathnames",
   NULL,
  PANEL_VALUE,DEF_START_REQUESTER,
  PANEL_NOTIFY_PROC,Property_category_proc,
  NULL /* End of list */
 );
  
 xv_set( /* Setup panel layout */
  SetupProps.panel, /* Parent */
  PANEL_LAYOUT,PANEL_VERTICAL, /* Justifycation */
  NULL /* End of list */
 );

 window_fit_height(SetupProps.panel); /* Adjust height */

 SetupNumberFormatProps(); /* Number format properties */
 SetupCalculationProps(); /* Calculation properties */
 SetupStacksizeProps(); /* Stack size properties */
 SetupHeapsizeProps(); /* Heap size properties */
 SetupMiscProps(); /* Heap size properties */
 SetupFileextenderProps(); /* Heap size properties */

 xv_set(SetupProps.PNF.ps.panel,XV_SHOW,TRUE,NULL); /* Disable requester */
 xv_set(SetupProps.CP.ps.panel,XV_SHOW,FALSE,NULL); /* Disable requester */
 xv_set(SetupProps.SS.ps.panel,XV_SHOW,FALSE,NULL); /* Disable requester */
 xv_set(SetupProps.HS.ps.panel,XV_SHOW,FALSE,NULL); /* Disable requester */
 xv_set(SetupProps.MI.ps.panel,XV_SHOW,FALSE,NULL); /* Disable requester */
 xv_set(SetupProps.FE.ps.panel,XV_SHOW,FALSE,NULL); /* Disable requester */

 switch_category(0,TRUE); /* Set category */

 REDUMA_PROPERTY_CREATE_DONE=1; /* Property frame created */

 DBUG_VOID_RETURN;
} /********/

static void SetupXSyntaxEditor(void)
{ /* REQUESTER : Syntaxeditor setup, cursor movement ... */
 DBUG_ENTER("SetupXSyntaxEditor");

 SetupProps.SE.ps.panel=(Panel)xv_create( /* Texteditor setup requester */
  SetupProps.xframe, /* Parent */
  PANEL,
  MY_PANEL_BORDER,TRUE,
  PANEL_LAYOUT,PANEL_VERTICAL,
  NULL
 );

 x=xv_get(SetupProps.SE.ps.panel,XV_X);
 y=xv_get(SetupProps.SE.ps.panel,XV_Y);

 SetupProps.SE.c_up=(Panel_item)xv_create(
  SetupProps.SE.ps.panel,
  PANEL_CHOICE_STACK,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Cursor up action",
  PANEL_CHOICE_STRINGS,
   "Previous expression",
   "Line up, 1st expression",
   "Line up",
   NULL,
  PANEL_VALUE,SetupProps.SE.c_up_value,
  NULL
 );

 SetupProps.SE.c_down=(Panel_item)xv_create(
  SetupProps.SE.ps.panel,
  PANEL_CHOICE_STACK,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Cursor down action",
  PANEL_CHOICE_STRINGS,
   "Next expression",
   "Line down, 1st expression",
   "Line down",
   NULL,
  PANEL_VALUE,SetupProps.SE.c_down_value,
  NULL
 );

 SetupProps.SE.c_left=(Panel_item)xv_create(
  SetupProps.SE.ps.panel,
  PANEL_CHOICE_STACK,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Cursor left action",
  PANEL_CHOICE_STRINGS,
   "Previous expression",
   NULL,
  PANEL_VALUE,SetupProps.SE.c_left_value,
  NULL
 );

 SetupProps.SE.c_right=(Panel_item)xv_create(
  SetupProps.SE.ps.panel,
  PANEL_CHOICE_STACK,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Cursor right action",
  PANEL_CHOICE_STRINGS,
   "Next expression",
   NULL,
  PANEL_VALUE,SetupProps.SE.c_right_value,
  NULL
 );

 SetupProps.SE.c_home=(Panel_item)xv_create(
  SetupProps.SE.ps.panel,
  PANEL_CHOICE_STACK,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Home key action",
  PANEL_CHOICE_STRINGS,
   "Expression home",
   "Program home",
   NULL,
  PANEL_VALUE,SetupProps.SE.c_up_value,
  NULL
 );

 SetupProps.SE.c_return=(Panel_item)xv_create(
  SetupProps.SE.ps.panel,
  PANEL_CHOICE_STACK,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Return key action",
  PANEL_CHOICE_STRINGS,
   "Next expression",
   "Expand expression",
   "Edit expression",
   "Carriage return",
   "Line feed",
   "CR + LF",
   NULL,
  PANEL_VALUE,SetupProps.SE.c_up_value,
  NULL
 );
 justify_items(SetupProps.SE.ps.panel,FALSE);

 MAC_CREATE_AP_DEF_RES(SE);

 DBUG_VOID_RETURN;
} /***/

static void SetupButtonsProperties(void)
{ /* REQUESTER : Property setups **/
 DBUG_ENTER("SetupButtonsProperties");

 SetupProps.PP.ps.panel=(Panel)xv_create( /* Texteditor setup requester */
  SetupProps.xframe, /* Parent */
  PANEL,
  XV_X,x,
  XV_Y,y,
  MY_PANEL_BORDER,TRUE,
  PANEL_LAYOUT,PANEL_VERTICAL,
  NULL
  );

 (void)xv_create(
  SetupProps.PP.ps.panel,
  PANEL_MESSAGE,
  PANEL_LABEL_STRING,"Behavior of property frame for button:",
  NULL
 );

 SetupProps.PP.apply_set=(Panel_item)xv_create(
  SetupProps.PP.ps.panel,
  PANEL_CHECK_BOX,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Apply",
  PANEL_VALUE,SetupProps.PP.apply_set_value,
  PANEL_CHOICE_STRINGS,"dismiss frame","ask before",NULL,
  NULL
 );

 SetupProps.PP.defaults_set=(Panel_item)xv_create(
  SetupProps.PP.ps.panel,
  PANEL_CHECK_BOX,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Defaults",
  PANEL_VALUE,SetupProps.PP.defaults_set_value,
  PANEL_CHOICE_STRINGS,"dismiss frame","ask before",NULL,
  NULL
 );

 SetupProps.PP.reset_set=(Panel_item)xv_create(
  SetupProps.PP.ps.panel,
  PANEL_CHECK_BOX,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_LABEL_STRING,"Reset",
  PANEL_VALUE,SetupProps.PP.reset_set_value,
  PANEL_CHOICE_STRINGS,"dismiss frame","ask before",NULL,
  NULL
 );

 justify_items(SetupProps.PP.ps.panel,FALSE);

 MAC_CREATE_AP_DEF_RES(PP);

 DBUG_VOID_RETURN;
} /********/

static void CreateXProperties(void)
{ /* Create a system property dialog for XWindow settings **/
 DBUG_ENTER("CreateXProperties");

 sprintf(BUFFER,"%s : XWindows Properties",PROGRAMM_NAME); /* Popup name */
 
 SetupProps.xframe=(Frame)xv_create( /* Create a layout for system setup */
  XRED.frame, /* Main window is parent */
  FRAME_PROPS, /* Type */
  XV_LABEL,BUFFER, /* Window name */
  NULL /* End of list */
 );

 SetupProps.xpanel=(Panel)xv_get( /* get property panel */
  SetupProps.xframe, /* Parent */
  FRAME_PROPS_PANEL
 );

 SetupProps.xcategory=(Panel_item)xv_create( /* Create property menu */
  SetupProps.xpanel, /* Parent */
  PANEL_CHOICE_STACK,
  PANEL_LAYOUT, PANEL_HORIZONTAL,
  PANEL_LABEL_STRING, "Category:",
  PANEL_CHOICE_STRINGS,
   "Syntaxeditor",
   "Standard button behavior",
   NULL,
  PANEL_VALUE,DEF_START_XREQUESTER,
  PANEL_NOTIFY_PROC,XProperty_category_proc,
  NULL /* End of list */
 );
  
 xv_set( /* Setup panel layout */
  SetupProps.xpanel, /* Parent */
  PANEL_LAYOUT,PANEL_VERTICAL, /* Justifycation */
  NULL /* End of list */
 );

 window_fit_height(SetupProps.xpanel); /* Adjust height */

 SetupXSyntaxEditor();
 SetupButtonsProperties();

 xv_set(SetupProps.SE.ps.panel,XV_SHOW,FALSE,NULL); /* Disable requester */

 switch_category(PROP_STANDARD,FALSE);
 switch_category(DEF_START_XREQUESTER,TRUE); /* Set category */

 XREDUMA_PROPERTY_CREATE_DONE=1; /* X Property frame created */

 DBUG_VOID_RETURN;
} /********/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void InitPropertyValues(void)
{ /* Allocates and initialize system property variables **/
 DBUG_ENTER("InitPropertyValues");

 AllocNameSpace(SetupProps.FE.ed_p_value,"");
 AllocNameSpace(SetupProps.FE.ed_e_value,"");
 AllocNameSpace(SetupProps.FE.pp_p_value,"");
 AllocNameSpace(SetupProps.FE.pp_e_value,"");
 AllocNameSpace(SetupProps.FE.doc_p_value,"");
 AllocNameSpace(SetupProps.FE.doc_e_value,"");
 AllocNameSpace(SetupProps.FE.prt_p_value,"");
 AllocNameSpace(SetupProps.FE.prt_e_value,"");
 AllocNameSpace(SetupProps.FE.asc_p_value,"");
 AllocNameSpace(SetupProps.FE.asc_e_value,"");
 AllocNameSpace(SetupProps.FE.red_p_value,"");
 AllocNameSpace(SetupProps.FE.red_e_value,"");
 AllocNameSpace(SetupProps.FE.pre_p_value,"");
 AllocNameSpace(SetupProps.FE.pre_e_value,"");

 SetDefaults_All(); /* Set all properties to default */

 DBUG_VOID_RETURN;
} /****/

void ShowProperties(void)
{ /* Get Reduma parameter and show Reduma property window **/
 DBUG_ENTER("ShowProperties");
 if (!REDUMA_PROPERTY_CREATE_DONE) CreateProperties();
 Update_Properties();
 xv_set(SetupProps.frame,XV_SHOW,TRUE,NULL);
 DBUG_VOID_RETURN;
} /********/

void ShowXProperties(void)
{ /* Show XReduma property window **/
 DBUG_ENTER("ShowXProperties");
 if (!XREDUMA_PROPERTY_CREATE_DONE) CreateXProperties();
 xv_set(SetupProps.xframe,XV_SHOW,TRUE,NULL);
 DBUG_VOID_RETURN;
} /********/

void pc_Delete_xv_Objects(void)
{ /* Destroys all Xview object (Should only be called at programmend) **/
 DBUG_ENTER("pc_Delete_xv_Objects");
 if (REDUMA_PROPERTY_CREATE_DONE) xv_destroy_safe(SetupProps.frame);
 if (XREDUMA_PROPERTY_CREATE_DONE) xv_destroy_safe(SetupProps.xframe);
 DBUG_VOID_RETURN;
} /*******/
