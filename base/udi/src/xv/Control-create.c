/**********************************************************************************
***********************************************************************************
**
**   File        : Control-create.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 28.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma.
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

#include <xview/xview.h>
#include <xview/panel.h>

#include "dbug.h"

#include "FileIO-create.h"
#include "Control-create.h"
#include "Menu-fcts.h"
#include "Reduma-cmd.h"
#include "TEditor-create.h"
#include "Properties-create.h"
#include "XReduma.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct ImageStructTag {
 unsigned short *image_bits;
 Server_image simage;
 int value;
 void (*proc)();
} ImageStruct;

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define IMAGE_WIDTH		16
#define IMAGE_HEIGHT		16
#define IMAGE_DEPTH		1	

#define ReBBuffer_VALUE		1
#define WrBBuffer_VALUE		2
#define ReBuffer1_VALUE		3
#define WrBuffer1_VALUE		4
#define ReBuffer2_VALUE		5
#define WrBuffer2_VALUE		6
#define CurUp_VALUE		7
#define CurDown_VALUE		8
#define CurLeft_VALUE		9
#define CurRight_VALUE		10
#define CurHome_VALUE		11
#define CurGoal_VALUE		12
#define Reduce_VALUE		13
#define F3Function_VALUE	14
#define F11Function_VALUE	15
#define Edit_VALUE		16
#define Load_VALUE		17
#define Save_VALUE		18
#define Undo_VALUE		19
#define Dismiss_VALUE		20
#define Delete_VALUE		21
#define FLoad_VALUE		22
#define FSave_VALUE		23
#define Print_VALUE             24
#define Cut_VALUE		25
#define Copy_VALUE		26
#define Paste_VALUE		27
#define Clear_VALUE		28
#define ReduceOne_VALUE		29
#define ReduceCnt_VALUE		30

#define NEXT_ROW		-1
#define NEXT_ROW_SKIP		-15
#define END_OF_IMAGES		99

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

static void ReBBuffer_proc(Panel_item item,Event *event);
static void WrBBuffer_proc(Panel_item item,Event *event);
static void ReBuffer1_proc(Panel_item item,Event *event);
static void WrBuffer1_proc(Panel_item item,Event *event);
static void ReBuffer2_proc(Panel_item item,Event *event);
static void WrBuffer2_proc(Panel_item item,Event *event);
static void CurUp_proc(Panel_item item,Event *event);
static void CurDown_proc(Panel_item item,Event *event);
static void CurLeft_proc(Panel_item item,Event *event);
static void CurRight_proc(Panel_item item,Event *event);
static void CurHome_proc(Panel_item item,Event *event);
static void CurGoal_proc(Panel_item item,Event *event);
static void Reduce_proc(Panel_item item,Event *event);
static void F3Function_proc(Panel_item item,Event *event);
static void F11Function_proc(Panel_item item,Event *event);
static void Edit_proc(Panel_item item,Event *event);
static void Load_proc(Panel_item item,Event *event);
static void Save_proc(Panel_item item,Event *event);
static void Undo_proc(Panel_item item,Event *event);
static void Delete_proc(Panel_item item,Event *event);
static void Dismiss_proc(Panel_item item,Event *event);
static void FLoad_proc(Panel_item item,Event *event);
static void FSave_proc(Panel_item item,Event *event);
static void Print_proc(Panel_item item,Event *event);
static void Cut_proc(Panel_item item,Event *event);
static void Copy_proc(Panel_item item,Event *event);
static void Paste_proc(Panel_item item,Event *event);
static void Clear_proc(Panel_item item,Event *event);
static void ReduceOne_proc(Panel_item item,Event *event);
static void ReduceCnt_proc(Panel_item item,Event *event);

/*********************************************************************************/
/********** LOCAL GRAPHIC IMAGES IN MODULE ***************************************/
/*********************************************************************************/

static unsigned short ControlPad_bits[]={
#include "images/Toolbox.icon"
};

static unsigned short ReBBuffer_bits[]={
#include "images/ReBBuffer"
};
static unsigned short WrBBuffer_bits[]={
#include "images/WrBBuffer"
};
static unsigned short ReBuffer1_bits[]={
#include "images/ReBuffer1"
};
static unsigned short WrBuffer1_bits[]={
#include "images/WrBuffer1"
};
static unsigned short ReBuffer2_bits[]={
#include "images/ReBuffer2"
};
static unsigned short WrBuffer2_bits[]={
#include "images/WrBuffer2"
};
static unsigned short CurUp_bits[]={
#include "images/CurUp"
};
static unsigned short CurDown_bits[]={
#include "images/CurDown"
};
static unsigned short CurLeft_bits[]={
#include "images/CurLeft"
};
static unsigned short CurRight_bits[]={
#include "images/CurRight"
};
static unsigned short CurHome_bits[]={
#include "images/CurHome"
};
static unsigned short CurGoal_bits[]={
#include "images/CurGoal"
};
static unsigned short Reduce_bits[]={
#include "images/Reduce"
};
static unsigned short F3Function_bits[]={
#include "images/F3Function"
};
static unsigned short F11Function_bits[]={
#include "images/F11Function"
};
static unsigned short Edit_bits[]={
#include "images/Edit"
};
static unsigned short Load_bits[]={
#include "images/Load"
};
static unsigned short Save_bits[]={
#include "images/Save"
};
static unsigned short Undo_bits[]={
#include "images/Undo"
};
static unsigned short Dismiss_bits[]={
#include "images/Dismiss"
};
static unsigned short Delete_bits[]={
#include "images/Delete"
};
static unsigned short FLoad_bits[]={
#include "images/FLoad"
};
static unsigned short FSave_bits[]={
#include "images/FSave"
};
static unsigned short Print_bits[]={
#include "images/Print"
};
static unsigned short Cut_bits[]={
#include "images/Cut"
};
static unsigned short Copy_bits[]={
#include "images/Copy"
};
static unsigned short Paste_bits[]={
#include "images/Paste"
};
static unsigned short Clear_bits[]={
#include "images/Clear"
};
static unsigned short ReduceOne_bits[]={
#include "images/ReduceOne"
};
static unsigned short ReduceCnt_bits[]={
#include "images/ReduceCnt"
};

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static ImageStruct Images[] = {
/* Pointer to image,handle  , Return Value **/
 { NULL            ,XV_NULL,0                 ,NULL},
 { ReBBuffer_bits  ,XV_NULL,ReBBuffer_VALUE   ,ReBBuffer_proc},
 { WrBBuffer_bits  ,XV_NULL,WrBBuffer_VALUE   ,WrBBuffer_proc},
 { ReBuffer1_bits  ,XV_NULL,ReBuffer1_VALUE   ,ReBuffer1_proc},
 { WrBuffer1_bits  ,XV_NULL,WrBuffer1_VALUE   ,WrBuffer1_proc},
 { ReBuffer2_bits  ,XV_NULL,ReBuffer2_VALUE   ,ReBuffer2_proc},
 { WrBuffer2_bits  ,XV_NULL,WrBuffer2_VALUE   ,WrBuffer2_proc},
 { CurUp_bits      ,XV_NULL,CurUp_VALUE       ,CurUp_proc},
 { CurDown_bits    ,XV_NULL,CurDown_VALUE     ,CurDown_proc},
 { CurLeft_bits    ,XV_NULL,CurLeft_VALUE     ,CurLeft_proc},
 { CurRight_bits   ,XV_NULL,CurRight_VALUE    ,CurRight_proc},
 { CurHome_bits    ,XV_NULL,CurHome_VALUE     ,CurHome_proc},
 { CurGoal_bits    ,XV_NULL,CurGoal_VALUE     ,CurGoal_proc},
 { Reduce_bits     ,XV_NULL,Reduce_VALUE      ,Reduce_proc},
 { F3Function_bits ,XV_NULL,F3Function_VALUE  ,F3Function_proc},
 { F11Function_bits,XV_NULL,F11Function_VALUE ,F11Function_proc},
 { Edit_bits       ,XV_NULL,Edit_VALUE        ,Edit_proc},
 { Load_bits       ,XV_NULL,Load_VALUE        ,Load_proc},
 { Save_bits       ,XV_NULL,Save_VALUE        ,Save_proc},
 { Undo_bits       ,XV_NULL,Undo_VALUE        ,Undo_proc},
 { Dismiss_bits    ,XV_NULL,Dismiss_VALUE     ,Dismiss_proc},
 { Delete_bits     ,XV_NULL,Delete_VALUE      ,Delete_proc},
 { FLoad_bits      ,XV_NULL,FLoad_VALUE       ,FLoad_proc},
 { FSave_bits      ,XV_NULL,FSave_VALUE       ,FSave_proc},
 { Print_bits      ,XV_NULL,Print_VALUE       ,Print_proc},
 { Paste_bits      ,XV_NULL,Paste_VALUE       ,Paste_proc},
 { Copy_bits       ,XV_NULL,Copy_VALUE        ,Copy_proc},
 { Cut_bits        ,XV_NULL,Cut_VALUE         ,Cut_proc},
 { Clear_bits      ,XV_NULL,Clear_VALUE       ,Clear_proc},
 { ReduceOne_bits  ,XV_NULL,ReduceOne_VALUE   ,ReduceOne_proc},
 { ReduceCnt_bits  ,XV_NULL,ReduceCnt_VALUE   ,ReduceCnt_proc}
};

static int ImageOrder[] = {
 Undo_VALUE,
 Delete_VALUE,
 Clear_VALUE,
 NEXT_ROW_SKIP,
 Cut_VALUE,
 Copy_VALUE,
 Paste_VALUE,
 NEXT_ROW_SKIP,
 ReduceOne_VALUE,
 ReduceCnt_VALUE,
 Reduce_VALUE,
 NEXT_ROW_SKIP,
 CurHome_VALUE,
 CurUp_VALUE,
 F3Function_VALUE,
 NEXT_ROW,
 CurLeft_VALUE,
 Edit_VALUE,
 CurRight_VALUE,
 NEXT_ROW,
 F11Function_VALUE,
 CurDown_VALUE,
 CurGoal_VALUE,
 NEXT_ROW_SKIP,
 WrBBuffer_VALUE,
 WrBuffer1_VALUE,
 WrBuffer2_VALUE,
 NEXT_ROW,
 ReBBuffer_VALUE,
 ReBuffer1_VALUE,
 ReBuffer2_VALUE, 
 NEXT_ROW_SKIP,
 FLoad_VALUE,
 FSave_VALUE,
 Print_VALUE,
 NEXT_ROW_SKIP,
 Load_VALUE,
 Save_VALUE,
 Dismiss_VALUE,
 END_OF_IMAGES
};

static int NumberOfImages=sizeof(Images)/sizeof(ImageStruct); 
static int CREATE_CONTROL_PAD_DONE=0;

static struct ControlPadStruct {
 Frame frame;
 Panel panel;
 Icon icon;
 Server_image image;
} ControlPad;

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void ReBBuffer_proc(Panel_item item,Event *event)
{ /* Read backbuffer to actual expression **/
 DBUG_ENTER("ReBBuffer_proc");

 mf_Buffer_fcts(BUFFER_READ_BACKUP);

 DBUG_VOID_RETURN;
} /*****/

static void WrBBuffer_proc(Panel_item item,Event *event)
{ /* Write actual expression to backup buffer **/
 DBUG_ENTER("WrBBuffer_proc");

 mf_Buffer_fcts(BUFFER_WRITE_BACKUP);

 DBUG_VOID_RETURN;
} /*****/

static void ReBuffer1_proc(Panel_item item,Event *event)
{ /* Read buffer 1 to actual expression **/
 DBUG_ENTER("ReBuffer1_proc");

 mf_Buffer_fcts(BUFFER_READ_1);

 DBUG_VOID_RETURN;
} /*****/

static void WrBuffer1_proc(Panel_item item,Event *event)
{ /* Write actual expression to buffer 1 **/
 DBUG_ENTER("WrBuffer1_proc");

 mf_Buffer_fcts(BUFFER_WRITE_1);

 DBUG_VOID_RETURN;
} /*****/

static void ReBuffer2_proc(Panel_item item,Event *event)
{ /* Read buffer 2 to actual expression **/
 DBUG_ENTER("ReBuffer2_proc");

 mf_Buffer_fcts(BUFFER_READ_2);

 DBUG_VOID_RETURN;
} /*****/

static void WrBuffer2_proc(Panel_item item,Event *event)
{ /* Write actual expression to buffer 2 **/
 DBUG_ENTER("WrBuffer2_proc");

 mf_Buffer_fcts(BUFFER_WRITE_2);

 DBUG_VOID_RETURN;
} /*****/

static void CurUp_proc(Panel_item item,Event *event)
{ /* Move Cursor up **/
 DBUG_ENTER("CurUp_proc");

 XR_MessageDisplay("RMove cursor up.");
 rcmd_CurUp();

 DBUG_VOID_RETURN;
} /*****/

static void CurDown_proc(Panel_item item,Event *event)
{ /* Move cursor down **/
 DBUG_ENTER("CurDown_proc");

 XR_MessageDisplay("RMove cursor down.");
 rcmd_CurDown();

 DBUG_VOID_RETURN;
} /*****/

static void CurLeft_proc(Panel_item item,Event *event)
{ /* Move cursor left **/
 DBUG_ENTER("CurLeft_proc");

 XR_MessageDisplay("RMove cursor left.");
 rcmd_CurLeft();

 DBUG_VOID_RETURN;
} /*****/

static void CurRight_proc(Panel_item item,Event *event)
{ /* Move cursor right **/
 DBUG_ENTER("CurRight_proc");

 XR_MessageDisplay("RMove cursor right.");
 rcmd_CurRight();

 DBUG_VOID_RETURN;
} /*****/

static void CurHome_proc(Panel_item item,Event *event)
{ /* Cursor home (First expr) **/
 DBUG_ENTER("CurHome_proc");

 XR_MessageDisplay("RMove cursor to top expression.");
 rcmd_CurHome();
 rcmd_CurHome();

 DBUG_VOID_RETURN;
} /*****/

static void CurGoal_proc(Panel_item item,Event *event)
{ /* Goto goalexpression **/
 DBUG_ENTER("CurGoal_proc");

 XR_MessageDisplay("RGoto goal expression.");
 rcmd_CurGoal();

 DBUG_VOID_RETURN;
} /*****/

static void F3Function_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("F3Function_proc");

 XR_MessageDisplay("RExpand actual cursor expression.");
 rcmd_F3Func();

 DBUG_VOID_RETURN;
} /*****/

static void F11Function_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("F11Function_proc");

 XR_MessageDisplay("RShow expression context.");
 rcmd_F11Func();

 DBUG_VOID_RETURN;
} /*****/

static void Edit_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("Edit_proc");

 XR_MessageDisplay("REdit cursor expression with texteditorobject.");
 Show_TextEditorObject();

 DBUG_VOID_RETURN;
} /*******/

static void Load_proc(Panel_item item,Event *event)
{ /* Reloads last saved expression **/
 DBUG_ENTER("Load_proc");

 XR_MessageDisplay("RReload last edited program.");
 rcmd_GoHome();
 rcmd_LoadEdFile(XRED.filename);

 DBUG_VOID_RETURN;
} /*******/

static void Save_proc(Panel_item item,Event *event)
{ /* Save actual expression **/
 DBUG_ENTER("Save_proc");

 XR_MessageDisplay("RStore actual program to a file.");
 rcmd_GoHome();
 rcmd_SaveEdFile(XRED.filename);

 DBUG_VOID_RETURN;
} /*******/

static void Undo_proc(Panel_item item,Event *event)
{ /* Undo last changes **/
 DBUG_ENTER("Undo_proc");

 mf_Reduce(MM_REDUCE_UNDO);

 DBUG_VOID_RETURN;
} /*******/

static void Delete_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("Delete_proc");

 XR_MessageDisplay("RDelete cursor expression.");
 rcmd_DeleteExpr();

 DBUG_VOID_RETURN;
} /*******/

static void Dismiss_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("Dismiss_proc");

 XR_MessageDisplay("RHide controlbox.");
 Hide_ControlPad();

 DBUG_VOID_RETURN;
} /*******/

static void FLoad_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("FLoad_proc");

 XR_MessageDisplay("RPopup load-filerequester. Select a file to load.");
 fc_ShowLoadFileRequester(1);

 DBUG_VOID_RETURN;
} /***********/

static void FSave_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("FSave_proc");

 XR_MessageDisplay("RPopup save-filerequester. Select a file to load.");
 fc_ShowSaveFileRequester(1);

 DBUG_VOID_RETURN;
} /***********/

static void Print_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("Print_proc");

 XR_MessageDisplay("RPop pi-RED reduction counter setup.");
 ShowProperties();
 Set_Property_category(OPTIONS_MISC);

 DBUG_VOID_RETURN;
} /***********/

static void Cut_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("Cut_proc");

 XR_MessageDisplay("RCut cursor expression and copy it to backup buffer.");
 rcmd_CutToStack();

 DBUG_VOID_RETURN;
} /***********/

static void Copy_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("Copy_proc");

 XR_MessageDisplay("RCopy cursor expression to backup buffer.");
 rcmd_CopyToStack();

 DBUG_VOID_RETURN;
} /***********/

static void Paste_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("Paste_proc");

 XR_MessageDisplay("RPaste backup buffer to cursor expression.");
 rcmd_CopyFromStack();

 DBUG_VOID_RETURN;
} /***********/

static void Clear_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("Clear_proc");

 XR_MessageDisplay("RClear program.");
 mf_File_Clear(0);

 DBUG_VOID_RETURN;
} /***********/

static void ReduceOne_proc(Panel_item item,Event *event)
{ /* Make one reduction step **/
 DBUG_ENTER("ReduceOne_proc");

 mf_Reduce(MM_REDUCE_1);

 DBUG_VOID_RETURN;
} /***********/

static void ReduceCnt_proc(Panel_item item,Event *event)
{ /* Make cnt reduction steps **/
 DBUG_ENTER("ReduceCnt_proc");

 mf_Reduce(MM_REDUCE_CNT);

 DBUG_VOID_RETURN;
} /***********/

static void Reduce_proc(Panel_item item,Event *event)
{ /* Reduce expression **/
 DBUG_ENTER("Reduce_proc");

 mf_Reduce(MM_REDUCE_REDUCEALL);

 DBUG_VOID_RETURN;
} /*****/

/* End of callback functions ******/

static void Mk_ServerImages(void)
{ /* Create server images for all bitmap icon in image array **/
 int i;
 DBUG_ENTER("Mk_ServerImages");

 for (i=1;i<NumberOfImages;i++) { /* Create server images (zero is a dummy obj) */
  Images[i].simage=(Server_image)xv_create(
   XV_NULL,
   SERVER_IMAGE,
   XV_WIDTH,IMAGE_WIDTH,
   XV_HEIGHT,IMAGE_HEIGHT,
   SERVER_IMAGE_DEPTH,IMAGE_DEPTH,
   SERVER_IMAGE_BITS,Images[i].image_bits,
   NULL
  );
 }

 DBUG_VOID_RETURN;
} /******/

static void Create_ControlPad(void)
{ /* Create frame and panels for the controlpad **/
 int i=0;
 DBUG_ENTER("Create_ControlPad");


 Mk_ServerImages(); /* Create images */

 ControlPad.image=(Server_image)xv_create( /* Create icon for controlpad toolbox */
  XV_NULL,
  SERVER_IMAGE,
  XV_WIDTH,64,
  XV_HEIGHT,64,
  SERVER_IMAGE_BITS,ControlPad_bits,
  NULL 
 );

 ControlPad.frame=(Frame)xv_create( /* main frame for control pad */
  XV_NULL,
  FRAME,
  FRAME_LABEL,"Controls",
  XV_SHOW,FALSE, /* dont show the frame yet */
  NULL
 );

 ControlPad.icon=(Icon)xv_create(
  ControlPad.frame,
  ICON,
  ICON_IMAGE,ControlPad.image,
  NULL
 );

 ControlPad.panel=(Panel)xv_create( /* Image panel */
  ControlPad.frame,
  PANEL,
  PANEL_LAYOUT,PANEL_HORIZONTAL,
  PANEL_BORDER,TRUE,
  PANEL_ITEM_X_GAP,5,
  PANEL_ITEM_Y_GAP,5,
  NULL
 );

 while (ImageOrder[i]!=END_OF_IMAGES) { /* Arrange all image buttons */
  if (ImageOrder[i]<0) { /* Make a new row */
   int value;
   if (ImageOrder[i]==NEXT_ROW) value=-1;
   else value=-ImageOrder[i];
   i++;
   (void)xv_create(
    ControlPad.panel,
    PANEL_BUTTON,
    PANEL_NEXT_ROW,value, /* use specified y gap */
    PANEL_LABEL_IMAGE,Images[ImageOrder[i]].simage,
    PANEL_NOTIFY_PROC,Images[ImageOrder[i]].proc,
    NULL
   );
  } else { /* Normal case */
   (void)xv_create(
    ControlPad.panel,
    PANEL_BUTTON,
    PANEL_LABEL_IMAGE,Images[ImageOrder[i]].simage,
    PANEL_NOTIFY_PROC,Images[ImageOrder[i]].proc,
    NULL
   );
  }
  i++; /* Increase image button number */
 }

 window_fit(ControlPad.panel); /* Adjust image panel height */
 window_fit(ControlPad.frame); /* Adjust frame size */

 CREATE_CONTROL_PAD_DONE=1;

 DBUG_VOID_RETURN;
} /*********/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void Hide_ControlPad(void)
{ /* Hides controlpad (dismiss frame) **/
 DBUG_ENTER("Hide_ControlPad");

 if (CREATE_CONTROL_PAD_DONE)
  xv_set(ControlPad.frame,XV_SHOW,FALSE,NULL);

 DBUG_VOID_RETURN;
} /****/

void Kill_ControlPad(void)
{ /* deletes Controlpad **/
 DBUG_ENTER("Kill_ControlPad");

 if (CREATE_CONTROL_PAD_DONE) {
  xv_destroy_safe(ControlPad.frame);
 }

 DBUG_VOID_RETURN;
} /******/

void Show_ControlPad(void)
{ /* Popup frame with reduma control images **/
 DBUG_ENTER("Show_ControlPad");

 if (!CREATE_CONTROL_PAD_DONE) Create_ControlPad();
 xv_set(ControlPad.frame,XV_SHOW,TRUE,NULL);

 DBUG_VOID_RETURN;
} /******/
