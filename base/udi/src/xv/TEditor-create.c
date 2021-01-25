/**********************************************************************************
***********************************************************************************
**
**   File        : TEditor-create.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 30.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This modules is the implementation module for
**                 texteditor object 
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

#define _OTHER_TEXTSW_FUNCTIONS

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>

#include "dbug.h"

#include "XReduma.h"
#include "TEditor-create.h"
#include "Reduma-cmd.h"
#include "Compiler-fcts.h"
#include "Menu-fcts.h"
#include "Menu-create.h"
#include "Misc-fcts.h"

#include "client-modul.h"

#include "strings/WhyNot-TEditor-create.str"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct NameRequestTag { /* Contains information for a name requester **/
 Frame      frame;
 Panel      panel;
 Panel_item pi_name;
 Panel_item pi_accept;
 Panel_item pi_cancel;
 void (*returnfct)();
} NameRequest; /*************/

typedef struct TextEditorObjTag { /* XView object for a small textedit compiler **/
 Textsw      textsw;
 char        *name; /* Name of text object */
 Frame       frame;
 Panel       panel;
 Panel_item  pi_done;
 Panel_item  pi_format;
 Panel_item  pi_insert;
 Panel_item  pi_cancel;
 Panel_item  pi_first;
 Panel_item  pi_prev;
 Panel_item  pi_next;
 Panel_item  pi_last;
} TextEditorObj; /*****/

typedef struct TEObjectTag { /* Text object */
 int                obj_number; /* Number of this object */
 int                textmode;
 TextEditorObj      *data; /* xview and text data */
 CompileInfo        *ci; /* Compile information */
 char               *tmp_filename; /* Temp file with source program (data) */
 NameRequest        NR; /* XView object for name request */
 struct TEObjectTag *next; /* Pointer to next object */
} TEObject; /*****/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define TEOBJECT_KEY	100

#define DEF_NAME	"_TMPExpr_"
#define DOLLAR_TMP_NAME "$_TMPExpr"
#define DOLLAR_TMP_NAME_LEN strlen(DOLLAR_TMP_NAME)

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char     BUFFER[128];

static TEObject *First=NULL;
static TEObject *Last=NULL;
static int      Actual_Obj_Number=0;
static int      Total_Obj_Number=0;

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void nr_accept_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=(TEObject *)xv_get(item,XV_KEY_DATA,TEOBJECT_KEY);
 char *value;
 DBUG_ENTER("nr_accept_proc");

 if (!obj) DBUG_VOID_RETURN;

 value=(char *)xv_get(obj->NR.pi_name,PANEL_VALUE);
 if (strlen(value)==0) value=NULL; /* Cancel operation */

 xv_set(obj->NR.frame,XV_SHOW,FALSE,NULL);
 xv_destroy_safe(obj->NR.frame);

 obj->NR.returnfct(
  value,
  obj
 );

 DBUG_VOID_RETURN;
} /******/

static void EnterDollarName_proc(Panel_item item,Event *event)
{ /* **/
 DBUG_ENTER("EnterDollarName_proc");

 nr_accept_proc(item,event);

 DBUG_VOID_RETURN;
} /*****/

static void nr_cancel_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=(TEObject *)xv_get(item,XV_KEY_DATA,TEOBJECT_KEY);
 DBUG_ENTER("nr_cancel_proc");


 xv_set(obj->NR.frame,XV_SHOW,FALSE,NULL);
 xv_destroy_safe(obj->NR.frame);

 obj->NR.returnfct(NULL,obj);

 DBUG_VOID_RETURN;
} /******/

static void ShowNameRequester(
 char *title,
 char *value,
 void (*return_fct)(),
 TEObject *obj
)
{ /* Popups a name requester to enter a simple name, value is pointer to buffer **/
 int x;
 DBUG_ENTER("ShowNameRequester");

 
 obj->NR.returnfct=return_fct;

 obj->NR.frame=(Frame)xv_create(
  XV_NULL,
  FRAME,
  FRAME_LABEL,"Name request",
  NULL
 );
 obj->NR.panel=(Panel)xv_create(
  obj->NR.frame,
  PANEL,
  NULL
 );
 obj->NR.pi_name=(Panel_item)xv_create(
  obj->NR.panel,
  PANEL_TEXT,
  PANEL_LABEL_STRING,(title==NULL) ? "Please enter a name :" : title,
  PANEL_VALUE,value,
  PANEL_VALUE_DISPLAY_LENGTH,20,
  PANEL_VALUE_STORED_LENGTH,50,
  XV_KEY_DATA,TEOBJECT_KEY,obj,
  PANEL_NOTIFY_PROC,EnterDollarName_proc,
  NULL
 );

 window_fit_width(obj->NR.panel);

 obj->NR.pi_accept=(Panel_item)xv_create( 
  obj->NR.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Accept",
  PANEL_NOTIFY_PROC,nr_accept_proc,
  XV_KEY_DATA,TEOBJECT_KEY,obj,
  NULL
 );
 
 xv_set(obj->NR.panel,PANEL_LAYOUT,PANEL_HORIZONTAL,NULL); 

 obj->NR.pi_cancel=(Panel_item)xv_create( 
  obj->NR.panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Cancel",
  PANEL_NOTIFY_PROC,nr_cancel_proc,
  XV_KEY_DATA,TEOBJECT_KEY,obj,
  NULL
 );

 x = xv_get(obj->NR.panel,XV_WIDTH)
     -xv_get(obj->NR.pi_accept,XV_WIDTH)
     -xv_get(obj->NR.pi_cancel,XV_WIDTH)
     -xv_get(obj->NR.panel,PANEL_ITEM_X_GAP);
 x=x/2;

 xv_set(obj->NR.pi_accept,XV_X,x,NULL); /* Center accept and cancel button */
 xv_set(
  obj->NR.pi_cancel,
  XV_X,x+xv_get(obj->NR.pi_accept,XV_WIDTH)+xv_get(obj->NR.panel,PANEL_ITEM_X_GAP),
  NULL
 ); 

 window_fit(obj->NR.panel);
 window_fit(obj->NR.frame);
 
 xv_set(obj->NR.panel,PANEL_DEFAULT_ITEM,obj->NR.pi_accept,NULL);
 xv_set(obj->NR.frame,XV_SHOW,TRUE,NULL);


 DBUG_VOID_RETURN;
} /******/

static int GetObjNumber(void)
{ /* Calculates smallest free object number **/
 TEObject *tmp=First;
 DBUG_ENTER("GetObjNumber");

 Actual_Obj_Number=1; /* Set number to smallest possible value */
 while (tmp) { /* Test all objects */
  if (tmp->obj_number==Actual_Obj_Number) Actual_Obj_Number++; /* Found ! */
  tmp=tmp->next; /* Test next number */
 }
 DBUG_RETURN (Actual_Obj_Number);
} /****/

static TEObject *Mk_TEObject(void)
{ /* Creates a new TEObject **/
 TEObject *result=(TEObject*)malloc(sizeof(TEObject));
 DBUG_ENTER("Mk_TEObject");

 if (result) {
  result->data=(TextEditorObj *)malloc(sizeof(TextEditorObj)); 
  if (result->data==NULL) {
   free(result);
   result=NULL;
  } else {
   Total_Obj_Number++; /* Increase number of total textediorobjects */
   result->tmp_filename=NULL;
   result->next=NULL; /* No next object */
  }
  result->ci=NULL; /* No Compile information available */
 }
 DBUG_RETURN (result);
} /*******/

static void Delete_TEObject(TEObject *obj)
{ /* Delete Object **/
 DBUG_ENTER("Delete_TEObject");


 if (obj) { /* Exists pointer object ? */

  if (obj==First) { /* Is obj the first one ? */
   if (First==Last) Last=First=NULL; /* Only one object ? */
   else First=First->next;
  } else { /* Search object */
   TEObject *tmp=First->next;
   TEObject *prev=First;
   while (tmp) {
    if (tmp==obj) {
     prev->next=obj->next;
     if (obj==Last) Last=prev;
     break;
    }
    prev=tmp;
    tmp=tmp->next;
   }
  }

  if (obj->tmp_filename) {
   if (obj->textmode==0) cf_RemoveTempfile(obj->tmp_filename); /* Remove pp file */
   free(obj->tmp_filename);
  }

  if (obj->data) {
   xv_set(obj->data->frame,FRAME_NO_CONFIRM,TRUE,NULL);
   xv_destroy_safe(obj->data->frame);
   if (obj->data->name) free(obj->data->name);
   free(obj->data);
  }

  if (obj->ci) Delete_CompileInfo(obj->ci);

  free(obj);

  sprintf(BUFFER,"%s%s",obj->data->name,ED_EXTENDER);
  cf_RemoveTempfile(BUFFER);  /* Delete ed file */

  if (First==NULL) { /* No more objects ? */
   mf_SetSyntaxEditorMode(S_TTY_NORMAL);
   XRED.syntax_tty_state=S_TTY_NORMAL;
  }

  Total_Obj_Number--; /* descrease number of textobjects */
  mc_SetTextObjectNumber(Total_Obj_Number);
 }


 DBUG_VOID_RETURN;
} /*******/

static TEObject *Get_TEObject(void)
{ /* Tries to get an unused object or creates a new one **/
 DBUG_ENTER("Get_TEObject");

 if (First==NULL) {
  First=Last=Mk_TEObject(); /* Create a new one */
  DBUG_RETURN (First);
 } else {
  Last->next=Mk_TEObject();
  Last=Last->next;
  DBUG_RETURN (Last);
 }
} /******/

static TEObject *Search_TEObject(Panel_item item)
{ /* Find panel item object **/
 TEObject *tmp=First;
 DBUG_ENTER("Search_TEObject");

 while (tmp) { /* Search panel item object */
  if (tmp->data) {
   if ((tmp->data->pi_done==item)   ||
       (tmp->data->pi_insert==item) ||
       (tmp->data->pi_cancel==item) ||
       (tmp->data->pi_format==item) ||
       (tmp->data->pi_first==item) ||
       (tmp->data->pi_prev==item) ||
       (tmp->data->pi_next==item) ||
       (tmp->data->pi_last==item)
      ) { /* Found ? */
    DBUG_RETURN (tmp);
   }
  }
  tmp=tmp->next; /* Try next object */
 }

 DBUG_RETURN (NULL);
} /******/

static TEObject *Search_Frame_TEObject(Frame frame)
{ /* Find panel item object **/
 TEObject *tmp=First;
 DBUG_ENTER("Search_Frame_TEObject");

 while (tmp) { /* Search panel item object */
  if (tmp->data) {
   if (tmp->data->frame==frame) { /* Found ? */
    DBUG_RETURN (tmp);
   }
  }
  tmp=tmp->next; /* Try next object */
 }

 DBUG_RETURN (NULL);
} /******/

static void ShowError(TEObject *obj,ErrorInfo *ei)
{ /* Show Error text, position cursor set buttons **/
 DBUG_ENTER("ShowError");

 if (ei) { /* Error information available ? */
  int number=cf_NumberOfErrors(obj->ci); /* Get max number of errors */

  sprintf(BUFFER,"Error : %d/%d",ei->number,number); /* Setup error text */
  xv_set(obj->data->frame,FRAME_RIGHT_FOOTER,BUFFER,NULL);
  sprintf(BUFFER,"Error in line %d : %s",ei->line,ei->text);
  xv_set(obj->data->frame,FRAME_LEFT_FOOTER,BUFFER,NULL);
  xv_set(obj->data->pi_first,PANEL_INACTIVE,FALSE,NULL);
  xv_set(obj->data->pi_prev,PANEL_INACTIVE,FALSE,NULL);
  xv_set(obj->data->pi_next,PANEL_INACTIVE,FALSE,NULL);
  xv_set(obj->data->pi_last,PANEL_INACTIVE,FALSE,NULL);
  if (ei->number==1) {
   xv_set(obj->data->pi_first,PANEL_INACTIVE,TRUE,NULL);
   xv_set(obj->data->pi_prev,PANEL_INACTIVE,TRUE,NULL);
  }
  if (ei->number==number) {
   xv_set(obj->data->pi_next,PANEL_INACTIVE,TRUE,NULL);
   xv_set(obj->data->pi_last,PANEL_INACTIVE,TRUE,NULL);
  }

  xv_set( /* Set cursor to error position */
   obj->data->textsw,
   TEXTSW_FIRST,textsw_index_for_file_line(obj->data->textsw,ei->line-1),
   TEXTSW_INSERTION_POINT,textsw_index_for_file_line(obj->data->textsw,ei->line-1),
   NULL
  );

 }

 DBUG_VOID_RETURN;
} /******/

static void EnableErrorButtons(TEObject *obj)
{ /* Enables error button and shows first error **/
 DBUG_ENTER("EnableErrorButtons");

 window_bell(obj->data->frame);
 xv_set(obj->data->textsw,TEXTSW_BLINK_CARET,TRUE,NULL);
 xv_set(obj->data->pi_first,XV_SHOW,TRUE,NULL);
 xv_set(obj->data->pi_prev,XV_SHOW,TRUE,NULL);
 xv_set(obj->data->pi_next,XV_SHOW,TRUE,NULL);
 xv_set(obj->data->pi_last,XV_SHOW,TRUE,NULL);
 ShowError(obj,cf_FirstError(obj->ci));

 DBUG_VOID_RETURN;
} /**********/

static void DisableErrorButtons(TEObject *obj)
{ /* Disables error buttons */
 DBUG_ENTER("DisableErrorButtons");

 xv_set(obj->data->frame,FRAME_RIGHT_FOOTER,"",FRAME_LEFT_FOOTER,"",NULL);
 xv_set(obj->data->textsw,TEXTSW_BLINK_CARET,FALSE,NULL);
 xv_set(obj->data->pi_first,XV_SHOW,FALSE,NULL);
 xv_set(obj->data->pi_prev,XV_SHOW,FALSE,NULL);
 xv_set(obj->data->pi_next,XV_SHOW,FALSE,NULL);
 xv_set(obj->data->pi_last,XV_SHOW,FALSE,NULL);

 DBUG_VOID_RETURN;
} /**********/

static void close_texteditobj_proc(Frame frame)
{ /* ***/
 TEObject *obj=Search_Frame_TEObject(frame);
 DBUG_ENTER("close_texteditobj_proc");

 if (!obj) DBUG_VOID_RETURN;
 Delete_TEObject(obj); /* Free memory and substructures */

 DBUG_VOID_RETURN;
} /*****/

static void te_done_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj;
 DBUG_ENTER("te_done_proc");

 obj=Search_TEObject(item);

 if (!obj) {
  DBUG_VOID_RETURN;
 } else {
  char *filename=Mk_Tmpfilename(PP_EXTENDER);             /* Create tempfilename */
  if (textsw_store_file(obj->data->textsw,filename,1,1)==0) { /* Store text */
   if (obj->ci) Delete_CompileInfo(obj->ci); /* Delete last object */
   obj->ci=cf_Transform_2_Redfile(filename);
   if (obj->ci) {  /* Pointer okay ? */
    if (obj->ci->okay) { /* Operation succesfully done ? */
     char *filename_2=cf_GetRedFilename(obj->ci); /* Get outputfile name */
     rcmd_SetCursorTo(obj->data->name); /* Move cursor */
     rcmd_LoadRedFile(filename_2); /* Load compile result */
     cf_RemoveTempfile(filename_2);
     sprintf(BUFFER,"cp %s %s",filename,obj->tmp_filename);
     system(BUFFER);
     Delete_TEObject(obj);
    } else { /* Syntax error */
     EnableErrorButtons(obj);
     cf_RemoveTempfile(filename); /* Remove output file */
    }
   }
  }
 }


 DBUG_VOID_RETURN;
} /***********/

static void InsertDolVar(char *value,TEObject *obj)
{ /* **/
 int remove=0;
 DBUG_ENTER("InsertDolVar");

 if (value!=NULL) {
  if ((obj->ci)&&(obj->ci->okay)) {
   char *filename_2=cf_GetRedFilename(obj->ci); /* Get outputfile name */
   rcmd_SetCursorTo(obj->data->name); /* Move cursor */
   rcmd_LoadRedFile(filename_2); /* Load compile result */
   cf_RemoveTempfile(filename_2);
   value[0]=toupper(value[0]);
   rcmd_Change2DollarVar(value,0);
   remove=1;
  }
 }

 xv_set(obj->data->frame,FRAME_BUSY,FALSE,NULL);
 xv_set(obj->data->pi_insert,PANEL_BUSY,FALSE,NULL);

 if (remove) Delete_TEObject(obj); /* Delete text object */


 DBUG_VOID_RETURN;
} /*****/

static void te_insert_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=Search_TEObject(item);
 DBUG_ENTER("te_insert_proc");


 if (!obj) {
  DBUG_VOID_RETURN;
 } else {
  char *filename=Mk_Tmpfilename(PP_EXTENDER);             /* Create tempfilename */
  if (textsw_store_file(obj->data->textsw,filename,1,1)==0) { /* Store text */
   if (obj->ci) Delete_CompileInfo(obj->ci); /* Delete last object */
   obj->ci=cf_Transform_2_Redfile(filename);
   if (obj->ci) {  /* Pointer okay ? */
    if (obj->ci->okay) { /* Operation succesfully done ? */
     xv_set(obj->data->frame,FRAME_BUSY,TRUE,NULL);
     xv_set(obj->data->pi_insert,PANEL_BUSY,TRUE,NULL);
     DisableErrorButtons(obj);
     ShowNameRequester("Enter a dollar variablename","",InsertDolVar,obj);
    } else { /* Syntax error */
     EnableErrorButtons(obj);
     cf_RemoveTempfile(filename); /* Remove output file */
    }
   }
  }
 }
 

 DBUG_VOID_RETURN;
} /***********/

static void te_format_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=Search_TEObject(item);
 DBUG_ENTER("te_format_proc");

 if (!obj) {
  DBUG_VOID_RETURN;
 } else {
  Textsw_status status;
  char *filename=Mk_Tmpfilename(PP_EXTENDER);             /* Create tempfilename */
  xv_set(obj->data->frame,FRAME_BUSY,TRUE,NULL);
  if (textsw_store_file(obj->data->textsw,filename,1,1)==0) { /* Store text */
   if (obj->ci) Delete_CompileInfo(obj->ci); /* Delete last object */
   obj->ci=cf_FormatFile(filename);
   if (obj->ci) {  /* Pointer okay ? */
    if (obj->ci->okay) { /* Operation succesfully done ? */
     char *filename_2=cf_GetRedFilename(obj->ci); /* Get outputfile name */
     xv_set(
      obj->data->textsw,
      TEXTSW_FILE_CONTENTS,filename_2,
      TEXTSW_FIRST,0,
      TEXTSW_STATUS,&status,
      TEXTSW_INSERTION_POINT,0,
      NULL
     );
     switch (status) {
      case TEXTSW_STATUS_CANNOT_INSERT_FROM_FILE : break;
      case TEXTSW_STATUS_OUT_OF_MEMORY : break;
      default : break;
     }
     textsw_normalize_view(obj->data->textsw,0);
     cf_RemoveTempfile(filename_2);
     DisableErrorButtons(obj);
    } else { /* Syntax error */
     EnableErrorButtons(obj);
     cf_RemoveTempfile(filename); /* Remove output file */
    }
   }
  }
  xv_set(obj->data->frame,FRAME_BUSY,FALSE,NULL);
 }

 DBUG_VOID_RETURN;
} /***********/

static void te_cancel_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=Search_TEObject(item);
 DBUG_ENTER("te_cancel_proc");


 if (obj) { /* Exists this object ? */
  if (xv_get(obj->data->textsw,TEXTSW_MODIFIED)) {
   if (!mf_ConfirmAction(  
       obj->data->panel,   
       "Program is changed !\nDo you really want to cancel\nand forget changes ?",
       event))
    DBUG_VOID_RETURN; 
  }
  rcmd_SetCursorTo(obj->data->name); /* Set cursor to expression */
  rcmd_LoadEdFile(obj->data->name); /* Reload old expression */
  textsw_reset(obj->data->textsw,1,1);
  Delete_TEObject(obj); /* Delete text object */
 }


 DBUG_VOID_RETURN;
} /***********/

static void te_firsterror_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=Search_TEObject(item);
 DBUG_ENTER("te_firsterror_proc");

 ShowError(obj,cf_FirstError(obj->ci));

 DBUG_VOID_RETURN;
} /*******/

static void te_preverror_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=Search_TEObject(item);
 DBUG_ENTER("te_preverror_proc");

 ShowError(obj,cf_PrevError(obj->ci));

 DBUG_VOID_RETURN;
} /*******/

static void te_nexterror_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=Search_TEObject(item);
 DBUG_ENTER("te_nexterror_proc");

 ShowError(obj,cf_NextError(obj->ci));

 DBUG_VOID_RETURN;
} /*******/

static void te_lasterror_proc(Panel_item item,Event *event)
{ /* **/
 TEObject *obj=Search_TEObject(item);
 DBUG_ENTER("te_lasterror_proc");

 ShowError(obj,cf_LastError(obj->ci));

 DBUG_VOID_RETURN;
} /*******/

static TEObject *Create_TextEditor(char *name)
{ /* Creates an texteditor object **/
 TEObject *result=Get_TEObject();
 TextEditorObj *obj;
 DBUG_ENTER("Create_TextEditor");


 if (result==NULL) DBUG_RETURN (NULL); /* Can't allocate mem */

 obj=result->data; /* Shortcut */

 obj->name=(char *)strdup(name); /* Copy window name to structure */

 sprintf(BUFFER,"Dollar expression : %s",name);

 obj->frame=(Frame)xv_create(
  XV_NULL,
  FRAME,
  FRAME_DONE_PROC,close_texteditobj_proc,
  FRAME_LABEL,BUFFER, /* Set frame name */
  FRAME_SHOW_FOOTER,TRUE, /* for messages */
  FRAME_NO_CONFIRM,FALSE,/* Show quit confirm requester */
  XV_WIDTH,DEF_TEOBJECT_WIDTH,
  XV_HEIGHT,DEF_TEOBJECT_HEIGHT,
  XV_SHOW,FALSE, /* Do not show yet */
  NULL
 );

 obj->panel=(Panel)xv_create(
  obj->frame,
  PANEL,                                                         /* Type */
  PANEL_BORDER,TRUE,
  XV_X,0,                                                     /* Left corner */
  XV_Y,0,                                                    /* Upper side */
  XV_WIDTH,WIN_EXTEND_TO_EDGE,
  NULL                                                           /* End of list */
 );

 obj->pi_done=(Panel_item)xv_create(
  obj->panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Done",
  PANEL_NOTIFY_PROC,te_done_proc,
  NULL
 );
 obj->pi_insert=(Panel_item)xv_create(
  obj->panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Insert $Var...",
  PANEL_NOTIFY_PROC,te_insert_proc,
  NULL
 );
 obj->pi_format=(Panel_item)xv_create(
  obj->panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Compile",
  PANEL_NOTIFY_PROC,te_format_proc,
  NULL
 );
 obj->pi_cancel=(Panel_item)xv_create(
  obj->panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Cancel",
  PANEL_NOTIFY_PROC,te_cancel_proc,
  NULL
 );
 obj->pi_first=(Panel_item)xv_create(
  obj->panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"First",
  PANEL_NOTIFY_PROC,te_firsterror_proc,
  XV_SHOW,FALSE,
  NULL
 );
 obj->pi_prev=(Panel_item)xv_create(
  obj->panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Prev",
  PANEL_NOTIFY_PROC,te_preverror_proc,
  XV_SHOW,FALSE,
  NULL
 );
 obj->pi_next=(Panel_item)xv_create(
  obj->panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Next",
  PANEL_NOTIFY_PROC,te_nexterror_proc,
  XV_SHOW,FALSE,
  NULL
 );
 obj->pi_last=(Panel_item)xv_create(
  obj->panel,
  PANEL_BUTTON,
  PANEL_LABEL_STRING,"Last",
  PANEL_NOTIFY_PROC,te_lasterror_proc,
  XV_SHOW,FALSE,
  NULL
 );

 window_fit_height(obj->panel); /* Adjust panel to frame */

 obj->textsw=(Textsw)xv_create( 
  obj->frame,
  TEXTSW,
  NULL
 );
  
 window_fit(obj->frame); /* Adjust frame sizes */

 DBUG_RETURN (result); /* All done */
} /*************/

static char *Mk_Defname(void)
{ /* Creates a defaults for the actual texteditor object **/
 int number=GetObjNumber();
 DBUG_ENTER("Mk_Defname");

 sprintf(BUFFER,"%s%d",DEF_NAME,number);
 DBUG_RETURN (strdup(BUFFER));
} /***/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void Show_TextEditorObject(void)
{ /* Pop texteditor object, pp expression save ed file insert $var **/
 char *name=Mk_Defname();
 TEObject *obj;
 DBUG_ENTER("Show_TextEditorObject");


 if ((obj=Create_TextEditor(name))!=NULL) { 
  Textsw_index first,last;
  Textsw_status status;                                          /* File status */

  obj->obj_number=Actual_Obj_Number; /* Set actual object number */
  obj->textmode=0;

  obj->tmp_filename=Mk_Tmpfilename(PP_EXTENDER);
  sprintf(BUFFER,"%s%s",name,ED_EXTENDER);
  cf_StoreTmpFileName(BUFFER);

  rcmd_SavePrettyPrint(obj->tmp_filename);            /* Execute remotecommand */

  sleep(1);

  xv_set(                                              /* Setup texfile field */
   obj->data->textsw,
   TEXTSW_STATUS, &status,
   TEXTSW_FILE,obj->tmp_filename,                                /* Load file */
   TEXTSW_FIRST,0, /* Set cursor to first line */
   NULL
  );

  if (status!=TEXTSW_STATUS_OKAY) { /* Test file loading state */
   window_bell(XRED.frame);
   Delete_TEObject(obj);
   DBUG_VOID_RETURN;
  }

  if (-1!=textsw_find_bytes(
           obj->data->textsw,
           &first,
           &last,
           DOLLAR_TMP_NAME,
           DOLLAR_TMP_NAME_LEN,
           0
          )
     ) { /* Contains text a subexpression ? */
   ThisFunctionIsNotAvailable(
    "texteditorobject",
    "syntax editing",
    WN_ALREADY_EDITING_SUBEXPRESSION
   );
   Delete_TEObject(obj);
   DBUG_VOID_RETURN;
  }

  if (!rcmd_Change2DollarVar(name,1)) { /* Write filedisk, insert $ Variable */
   ThisFunctionIsNotAvailable(
    "texteditorobject",
    "line editing",
    WN_CURSOR_NOT_AT_EXPR
   );
   cf_RemoveTempfile(BUFFER); /* remove ed file */
   Delete_TEObject(obj);
   DBUG_VOID_RETURN;
  }

  cf_RemoveTempfile(obj->tmp_filename);

  xv_set(obj->data->frame,XV_SHOW,TRUE,NULL);
  mf_SetSyntaxEditorMode(S_TTY_READONLY);
  XRED.syntax_tty_state=S_TTY_READONLY;

  mc_SetTextObjectNumber(Total_Obj_Number);
 }
 
 free(name);

 DBUG_VOID_RETURN;
} /******/

void Edit_Textfile(char *textfilename)
{ /* Pop texteditor object **/
 char *name=Mk_Defname();
 TEObject *obj;
 DBUG_ENTER("Edit_Textfile");


 if ((obj=Create_TextEditor(name))!=NULL) { 
  Textsw_index first,last;
  Textsw_status status;                                          /* File status */

  obj->obj_number=Actual_Obj_Number; /* Set actual object number */
  obj->textmode=1;

  obj->tmp_filename=(char *)strdup(textfilename);

  sprintf(BUFFER,"%s%s",name,ED_EXTENDER);
  cf_StoreTmpFileName(BUFFER);

  xv_set(                                              /* Setup texfile field */
   obj->data->textsw,
   TEXTSW_STATUS, &status,
   TEXTSW_FILE,obj->tmp_filename,                                /* Load file */
   TEXTSW_FIRST,0, /* Set cursor to first line */
   NULL
  );

  if (status!=TEXTSW_STATUS_OKAY) { /* Test file loading state */
   window_bell(XRED.frame);
   Delete_TEObject(obj);
   DBUG_VOID_RETURN;
  }

  if (-1!=textsw_find_bytes(
           obj->data->textsw,
           &first,
           &last,
           DOLLAR_TMP_NAME,
           DOLLAR_TMP_NAME_LEN,
           0
          )
     ) { /* Contains text a subexpression ? */
   ThisFunctionIsNotAvailable(
    "texteditorobject",
    "syntax editing",
    WN_ALREADY_EDITING_SUBEXPRESSION
   );
   Delete_TEObject(obj);
   DBUG_VOID_RETURN;
  }

  if (!rcmd_Change2DollarVar(name,1)) { /* Write filedisk, insert $ Variable */
   ThisFunctionIsNotAvailable(
    "texteditorobject",
    "line editing",
    WN_CURSOR_NOT_AT_EXPR
   );
   cf_RemoveTempfile(BUFFER); /* remove ed file */
   Delete_TEObject(obj);
   DBUG_VOID_RETURN;
  }

  xv_set(obj->data->frame,XV_SHOW,TRUE,NULL);
  mf_SetSyntaxEditorMode(S_TTY_READONLY);
  XRED.syntax_tty_state=S_TTY_READONLY;
  mc_SetTextObjectNumber(Total_Obj_Number);
 }
 
 free(name);


 DBUG_VOID_RETURN;
} /******/
