/**********************************************************************************
***********************************************************************************
**
**   File        : FileIO-create.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 8.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This module implements all fileio requester functions.
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
#include "FileIO-create.h"
#include "FileIO-fcts.h"
#include "Menu-create.h"

#include <xview/xview.h>
#include <xview/file_chsr.h>

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct FileRequesterTag { /* Filerequester structure **/
 Frame             frame; /* Window frame */
 File_chooser      filechooser; /*  File chooser handle */
 Panel_choice_item extender;
} FileRequest; /****/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char BUFFER[512];

static int CREATE_LOAD_FILEREQUESTER_NOTDONE=1;                 /* init not done */
static int CREATE_SAVE_FILEREQUESTER_NOTDONE=1;                 /* init not done */
static int CREATE_LOADSETUP_FILEREQUESTER_NOTDONE=1;            /* init not done */
static int CREATE_SAVESETUP_FILEREQUESTER_NOTDONE=1;            /* init not done */

static FileRequest LoadRequester;
static FileRequest SaveRequester;
static FileRequest LoadSetupRequester;
static FileRequest SaveSetupRequester;

static Attr_attribute PARENT_FC_KEY;
static Attr_attribute EXTEN_ITEM_KEY;

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

int LOAD_MODE=1; /* Default loadmode is new program */

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void Mk_ExtenderPattern(char *buf,char *ext)
{ /* Generate a pattern for given extension **/
 static char BUF[64];
 char *tmp=(char *)&BUF;
 DBUG_ENTER("Mk_ExtenderPattern");

 strcpy(BUF,"");
 while (*ext) {
  if (isalnum(*ext)) *tmp=*ext;
  else  {*tmp='\\';tmp++;*tmp=*ext;}
  tmp++;
  ext++;
 }
 *tmp='\0';
 sprintf(buf,".*%s$",BUF);

 DBUG_VOID_RETURN;
} /*****/

static void My_ExtenderFunc(
 Panel_choice_item item,
 int value,
 Event *event)
{ /* Selected extender **/
 char *ext;
 DBUG_ENTER("My_ExtenderFunc");

 switch (value) {
  case 1  : ext=RED_EXTENDER;break;
  case 2  : ext=PP_EXTENDER;break;
  case 3  : ext="";break;
  default : ext=ED_EXTENDER;break;
 }
 Mk_ExtenderPattern(BUFFER,ext);
 xv_set(
  LoadRequester.filechooser,
  FILE_CHOOSER_FILTER_STRING,BUFFER,
  NULL
 );

 DBUG_VOID_RETURN;
} /*********/

static int my_exten_func(
 File_chooser fc,
 Rect *frame_rect,
 Rect *exten_rect,
 int left_edge,
 int right_edge,
 int max_height)
{ /* FILE_CHOOSER_EXTEN_FUNC, layout extension items **/
 Panel_item item=(Panel_item)xv_get(fc,XV_KEY_DATA,EXTEN_ITEM_KEY);
 int item_width=(int)xv_get(item,XV_WIDTH);
 DBUG_ENTER("my_exten_func");

 xv_set(
  item, /* show item centered in frame */
  XV_X,(frame_rect->r_width-item_width)/2,
  XV_Y,exten_rect->r_top,
  PANEL_PAINT,PANEL_NONE,
  NULL
 );

 DBUG_RETURN(-1);
} /*****/

static void AddExtenderItem(File_chooser fc,Panel_item *item)
{ /* Add file extender item to file chooser object **/
 Panel panel;
 int  item_width;
 int  item_height;
 int  frame_width;
 int  frame_height;
 DBUG_ENTER("AddExtenderItem");


 panel=xv_get(fc,FRAME_CMD_PANEL);
 *item=xv_create(
  panel,
  PANEL_CHOICE,
  PANEL_LABEL_STRING,"Fileextension:",
  PANEL_CHOICE_STRINGS,ED_EXTENDER,RED_EXTENDER,PP_EXTENDER,"All",NULL,
  PANEL_NOTIFY_PROC,My_ExtenderFunc,
  XV_KEY_DATA,PARENT_FC_KEY,fc,
  NULL
 );
   
 item_width=(int)xv_get(*item,XV_WIDTH);
 item_height=(int)xv_get(*item,XV_HEIGHT);
   
 /* Adjust Frame default size to make room for the extension item. */
 frame_width=(int)xv_get(fc,XV_WIDTH);
 frame_height=(int)xv_get(fc,XV_HEIGHT);
 xv_set(
  fc,
  XV_WIDTH,MAX(frame_width,(item_width+xv_cols(panel,4))),
  XV_HEIGHT,frame_height+item_height,
  NULL
 );

 /* Adjust Frame Min Size */
 xv_get(fc,FRAME_MIN_SIZE,&frame_width,&frame_height);
 xv_set(
  fc,
  FRAME_MIN_SIZE,
  MAX(frame_width,(item_width+xv_cols(panel,4))),
  frame_height+item_height,
  NULL
 );

 /* Tell File Chooser to reserve layout space for it */
 xv_set(
  fc,
  FILE_CHOOSER_EXTEN_HEIGHT,item_height,
  FILE_CHOOSER_EXTEN_FUNC,my_exten_func,
  XV_KEY_DATA,EXTEN_ITEM_KEY,*item,
  NULL
 );


 DBUG_VOID_RETURN;
} /*******/


static void CreateLoadSetupRequester(void)
{ /* Create a load setup file requester **/
 DBUG_ENTER("CreateLoadSetupFileRequester");

 LoadSetupRequester.filechooser=(File_chooser)xv_create( /* Setup requester */
  XV_NULL, /* no parent needed */
  FILE_CHOOSER_OPEN_DIALOG, /* Type */
  XV_SHOW,FALSE, /* Do not show this requester now */
  FILE_CHOOSER_AUTO_UPDATE,TRUE,
  FILE_CHOOSER_NOTIFY_FUNC,ff_LoadSetup_notify_proc,
  NULL /* End of list */
 );
 CREATE_LOADSETUP_FILEREQUESTER_NOTDONE=0;

 DBUG_VOID_RETURN;
} /******/

static void CreateSaveSetupRequester(void)
{ /* Create a save setup file requester **/
 DBUG_ENTER("CreateSaveSetupFileRequester");

 SaveSetupRequester.filechooser=(File_chooser)xv_create( /* Setup requester */
  XV_NULL, /* no parent needed */
  FILE_CHOOSER_SAVEAS_DIALOG, /* Type */
  XV_SHOW,FALSE, /* Do not show this requester now */
  FILE_CHOOSER_AUTO_UPDATE,TRUE,
  FILE_CHOOSER_NOTIFY_FUNC,ff_SaveSetup_notify_proc,
  NULL /* End of list */
 );
 CREATE_SAVESETUP_FILEREQUESTER_NOTDONE=0;

 DBUG_VOID_RETURN;
} /******/

static void CreateLoadFileRequester(void)
{ /* Create a load file requester **/
 DBUG_ENTER("CreateLoadFileRequester");

 LoadRequester.filechooser=(File_chooser)xv_create( /* Setup requester */
  XV_NULL, /* no parent needed */
  FILE_CHOOSER_OPEN_DIALOG, /* Type */
  XV_SHOW,FALSE, /* Do not show this requester now */
  FILE_CHOOSER_FILTER_MASK,FC_MATCHED_FILES_MASK,
  FILE_CHOOSER_AUTO_UPDATE,TRUE,
  FILE_CHOOSER_NOTIFY_FUNC,ff_LoadFile_notify_proc,
  NULL /* End of list */
 );
 AddExtenderItem(LoadRequester.filechooser,&LoadRequester.extender); 
 CREATE_LOAD_FILEREQUESTER_NOTDONE=0;

 DBUG_VOID_RETURN;
} /******/

static void CreateSaveFileRequester(void)
{ /* Create a save file requester **/
 DBUG_ENTER("CreateSaveFileRequester");

 SaveRequester.filechooser=(File_chooser)xv_create( /* Setup requester */
  XV_NULL, /* no parent needed */
  FILE_CHOOSER_SAVEAS_DIALOG, /* Type */
  XV_SHOW,FALSE, /* Do not show this requester now */
  FILE_CHOOSER_NOTIFY_FUNC,ff_SaveFile_notify_proc,
  NULL /* End of list */
 );
 CREATE_SAVE_FILEREQUESTER_NOTDONE=0;

 DBUG_VOID_RETURN;
} /******/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void fc_Update_Filerequester(void)
{ /* Update all filerequester extensions **/
 DBUG_ENTER("fc_Update_Filerequester");

 if (!CREATE_LOAD_FILEREQUESTER_NOTDONE) {
  if (xv_get(LoadRequester.filechooser,XV_SHOW)==FALSE) {
   xv_destroy_safe(LoadRequester.extender);
   xv_destroy_safe(LoadRequester.filechooser);
   xv_destroy_safe(LoadRequester.frame);
   CREATE_LOAD_FILEREQUESTER_NOTDONE=1;
  }
 }

 DBUG_VOID_RETURN;
} /***/

int fc_ShowLoadFileRequester(int mode)
{ /* Call Load file requester and do IO **/
 DBUG_ENTER("fc_ShowLoadFileRequester");

 if (CREATE_LOAD_FILEREQUESTER_NOTDONE) CreateLoadFileRequester();
 LOAD_MODE=mode;

 DBUG_RETURN(xv_set(LoadRequester.filechooser,XV_SHOW,TRUE,NULL));
} /******/

int fc_ShowLoadSetupRequester(int mode)
{ /* Call Load Setup requester and do IO **/
 DBUG_ENTER("fc_ShowLoadFileRequester");

 if (CREATE_LOADSETUP_FILEREQUESTER_NOTDONE) CreateLoadSetupRequester();
 xv_set(LoadSetupRequester.filechooser,FILE_CHOOSER_DOC_NAME,"red.init",NULL);
 xv_set(LoadSetupRequester.filechooser,FILE_CHOOSER_DIRECTORY,".",NULL);
 LOAD_MODE=mode;

 DBUG_RETURN(xv_set(LoadSetupRequester.filechooser,XV_SHOW,TRUE,NULL));
} /******/

int fc_ShowSaveSetupRequester(int mode)
{ /* Call Save Setup requester and do IO **/
 DBUG_ENTER("fc_ShowSaveFileRequester");

 if (CREATE_SAVESETUP_FILEREQUESTER_NOTDONE) CreateSaveSetupRequester();
 xv_set(SaveSetupRequester.filechooser,FILE_CHOOSER_DOC_NAME,"red.init",NULL);

 DBUG_RETURN(xv_set(SaveSetupRequester.filechooser,XV_SHOW,TRUE,NULL));
} /******/

int fc_ShowSaveFileRequester(int mode)
{ /* Call Save file requester and do IO **/
 DBUG_ENTER("fc_ShowSaveFileRequester");

 if (CREATE_SAVE_FILEREQUESTER_NOTDONE) CreateSaveFileRequester();
 LOAD_MODE=mode;

 xv_set(SaveRequester.filechooser,FILE_CHOOSER_DOC_NAME,"",NULL);
 /* xv_set(SaveRequester.filechooser,FILE_CHOOSER_DIRECTORY,"",NULL); */

 DBUG_RETURN(xv_set(SaveRequester.filechooser,XV_SHOW,TRUE,NULL));
} /******/

void fc_Delete_xv_Objects(void)
{ /* Destroys all Xview object (Should only be called at programmend) **/
 DBUG_ENTER("fc_Delete_xv_Objects");

 if (!CREATE_LOAD_FILEREQUESTER_NOTDONE)
  xv_destroy_safe(LoadRequester.filechooser);

 if (!CREATE_SAVE_FILEREQUESTER_NOTDONE)
  xv_destroy_safe(SaveRequester.filechooser);

 if (!CREATE_LOADSETUP_FILEREQUESTER_NOTDONE)
  xv_destroy_safe(LoadSetupRequester.filechooser);

 if (!CREATE_SAVESETUP_FILEREQUESTER_NOTDONE)
  xv_destroy_safe(SaveSetupRequester.filechooser);

 DBUG_VOID_RETURN;
} /*******/
