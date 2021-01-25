/**********************************************************************************
***********************************************************************************
**
**   File        : Menu-create.c
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

#include "dbug.h"

#include <xview/notice.h>

#include "XReduma.h"
#include "Menu-create.h"
#include "Menu-fcts.h"
#include "Properties-create.h"
#include "Properties-fcts.h"
#include "Reduma-cmd.h"
#include "Control-create.h"
#include "Misc-fcts.h"
#include "TEditor-create.h"
#include "FileIO-fcts.h"

#include "client-modul.h"

#include "macros/Menu-create.mac"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct MenuStructTag { /* XV-object struct for main menu **/
 Menu menu; /* Menu handle */
 char *title; /* Menu title for pinup menu text */
 char hasdismiss; /* Sign if actual menu has dismiss item */
} MenuStruct; /*******/

typedef struct MenuElemsTag { /* Additional menu items **/
 Panel_item pi_filename; /* Programme filename */
 Panel_item pi_nr_te; /* Number of textitems in this programm */
} MenuElems; /****/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

#define MENU_EMPTY_ITEM         MENU_ITEM,MENU_STRING,"",MENU_INACTIVE,TRUE,NULL

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char BUFFER[512];

static MenuElems  MenuItems; /* Special menu elements */
static MenuStruct MainMenu[NR_MENUES]; /* Static Xred menu */
static void (*pin_proc)()=NULL; /* Pointer to defaults mk pin function */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static Panel_setting new_filename_proc(Panel_item item,int value,Event *event)
{ /* **/
 char *str=(char *)xv_get(item,PANEL_VALUE);
 DBUG_ENTER("new_filename_proc");

 sprintf(BUFFER,"RTry to load file %s",str);
 XR_MessageDisplay(BUFFER);
 rcmd_CurHome();
 rcmd_CurHome();
 ff_LoadFile(str);
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_RETURN (PANEL_NEXT);
} /*****/

static void mb_syntax_editor_layout(Menu menu,Menu_item item)
{ /* **/
 int i;
 int result=0;
 DBUG_ENTER("mb_syntax_editor_layout");

 sprintf(BUFFER,"RSet syntax editor layout to:");
 for (i=(int)xv_get(menu,MENU_NITEMS);i>0;i--) { /* Test all menu items */
  if (xv_get(xv_get(menu,MENU_NTH_ITEM,i),MENU_SELECTED)) {
   switch (i) {
    case 2  : result|=DDECL;strcat(BUFFER," DEF");break;
    case 3  : result|=DTDECL;strcat(BUFFER," IDEF");break;
    case 4  : result|=DGOAL;strcat(BUFFER," SGOAL");break;
    case 5  : result|=DFCT;strcat(BUFFER," FCT");break;
    case 6  : result|=DSMALL;strcat(BUFFER," SMALL");break;
    case 7  : result|=DVERT;strcat(BUFFER," VERT");break;
    default : break;
   }
  }
 }
 XR_MessageDisplay(BUFFER);
 rcmd_SetSyntaxEditorLayout(result);

 DBUG_VOID_RETURN;
} /*********/

static int GetMenuNumber(Menu menu)
{ /* Search the number from the menu handle **/
 int i=0;
 DBUG_ENTER("GetMenuNumber");

 while (i<NR_MENUES) { /* test all menu avalable */
  if (MainMenu[i].menu==menu) DBUG_RETURN (i); /* Menu found */
  i++;
 }

 DBUG_RETURN (0);
} /*******/

static void DismissmenuFrame(Menu menu)
{ /* Disable actual frame **/
 int  menu_nr=GetMenuNumber(menu);
 char *HasDismiss=&(MainMenu[menu_nr].hasdismiss);
 DBUG_ENTER("DismissmenuFrame");

 if (*HasDismiss) {
  xv_set(menu,MENU_REMOVE,(int)xv_get(menu,MENU_NITEMS),NULL); /* Remove item */
  if (menu_nr==MM_BUFFER) {
   xv_set(menu,MENU_NCOLS,2,NULL);
  }
  *HasDismiss=0; /* Dismiss item no longer in use */
 }
 xv_set( /* Release popup frame */
  xv_get(xv_get(menu,MENU_PIN_WINDOW),WIN_FRAME),
  FRAME_CMD_PIN_STATE,FRAME_CMD_PIN_OUT, /* Pull pin out */
  XV_SHOW,FALSE,
  NULL
 );
 sprintf(BUFFER,"RPinup menu %s dismissed.",MainMenu[menu_nr].title);
 XR_MessageDisplay(BUFFER);

 DBUG_VOID_RETURN;
} /****/

static void MakePinMenu(Menu menu,int x,int y)
{ /* make pin menu and check window manager **/
 int  menu_nr=GetMenuNumber(menu);
 char *HasDismiss=&(MainMenu[menu_nr].hasdismiss);
 DBUG_ENTER("MakePinMenu");

 if ((*HasDismiss)&&(XRED.openlook==1)) {
  xv_set(menu,MENU_REMOVE,(int)xv_get(menu,MENU_NITEMS),NULL); /* Remove item */
  *HasDismiss=0;
 } else if ((!(*HasDismiss))&&(XRED.openlook==0)) { /* Add dismiss item ? */
  if (menu_nr==MM_BUFFER) { /* Change layout for buffer pinup frame ? */
   xv_set(menu,MENU_NCOLS,3,NULL);
  }
  xv_set(
   menu,
   MENU_APPEND_ITEM, /* append dismiss item to this menu */
    xv_create(
     XV_NULL,
     MENUITEM,
     MENU_STRING,"Dismiss menu",
     MENU_VALUE,MM_DISMISS,
     NULL
    ),
   NULL
  );
  xv_set(menu,MENU_DEFAULT,(int)xv_get(menu,MENU_NITEMS),NULL);
  sprintf(BUFFER,"RPinup menu %s.",MainMenu[menu_nr].title);
  XR_MessageDisplay(BUFFER);
  *HasDismiss=1;
 }
 (pin_proc)(menu,x,y); /* Call standard gen pin frame function */

 DBUG_VOID_RETURN;
} /*******/

/* Menu item switch functions. They select the call function. **/

static void mb_Reduce_menu(Menu menu,Menu_item item)
{ /* Switch Functions for reduce menu **/
 int value=(int)xv_get(item,MENU_VALUE);
 DBUG_ENTER("mb_Reduce_menu");

 switch (value) {
  case MM_REDUCE_UNDO :
   mf_Reduce(MM_REDUCE_UNDO);
   break;
  case MM_GO_HOME :
   XR_MessageDisplay("RSet cursor to top expression.");
   rcmd_CurHome();
   rcmd_CurHome();
   break;
  case MM_REDUCE_1 :
   mf_Reduce(MM_REDUCE_1);
   break;
  case MM_REDUCE_CNT :
   mf_Reduce(MM_REDUCE_CNT);
   break;
  case MM_REDUCE_REDUCEALL :
   mf_Reduce(MM_REDUCE_REDUCEALL);
   break;
  case MM_REDUCE_SETCOUNTER :
   ShowProperties();
   Set_Property_category(OPTIONS_MISC);
   break;
  default : 
   DismissmenuFrame(menu);
   break;
 }
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /*******/

static void mb_File_menu(Menu menu,Menu_item item)
{ /* Switch Functions for File menu **/
 int value=(int)xv_get(item,MENU_VALUE);
 DBUG_ENTER("mb_File_menu");

 switch (value) {
  case MM_FILE_CLEAR :
   XR_MessageDisplay("RClear program.");
   mf_File_Clear(0);
   break;
  case MM_FILE_LOAD :
   XR_MessageDisplay("RLoad a program.");
   mf_File_Load();
   break;
  case MM_FILE_INCLUDE :
   XR_MessageDisplay("RInclude a subexpression to cursor position.");
   mf_File_Include();
   break;
  case MM_FILE_SAVE :
   XR_MessageDisplay("RSave this program to a file.");
   mf_File_Save();
   break;
  case MM_FILE_SAVEAS :
   XR_MessageDisplay("RSave this program as a new file."); 
   mf_File_Saveas();
   break;
  case MM_FILE_SAVEEXPR :
   XR_MessageDisplay("RSave the actual expression as a file.");
   mf_File_Saveexpr();
   break;
  case MM_FILE_LOADSETUPS :
   XR_MessageDisplay("RLoad program settings from a file.");
   mf_File_LoadSetups();
   break;
  case MM_FILE_SAVESETUPS :
   XR_MessageDisplay("RSave program settings to a file.");
   mf_File_SaveSetups();
   break;
  case MM_FILE_EXIT :
   XR_MessageDisplay("RQuit program.");
   mf_File_Exit();
   break;
  default : 
   DismissmenuFrame(menu);
   break;
 }
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /******/

static void mb_Option_menu(Menu menu,Menu_item item)
{ /* Switch Functions for Option menu **/
 int value=(int)xv_get(item,MENU_VALUE);
 DBUG_ENTER("mb_Option_menu");

 switch (value) {
  case MM_OPTION_REDUMA :
   XR_MessageDisplay("RShow pi-RED property setup requester.");
   ShowProperties();
   break;
  case MM_OPTION_FRONTEND :
   XR_MessageDisplay("RShow frontend property setup requester.");
   ShowXProperties(); 
   break;
  default : 
   DismissmenuFrame(menu);
   break;
 }
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /******/

static void mb_Extra_menu(Menu menu,Menu_item item)
{ /* Switch Functions for Extra menu **/
 int value=(int)xv_get(item,MENU_VALUE);
 DBUG_ENTER("mb_Extra_menu");

 switch (value) {
  case MM_EXTRA_TOOLBOX :
   XR_MessageDisplay("RShow controlbox.");
   Show_ControlPad();
   break;
  default : 
   DismissmenuFrame(menu);
   break;
 }
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /******/

static void mb_Edit_menu(Menu menu,Menu_item item)
{ /* Switch Functions for Edit menu **/
 int value=(int)xv_get(item,MENU_VALUE);
 DBUG_ENTER("mb_Edit_menu");

 switch (value) {
  case MM_EDIT_UNDO :
   mf_Reduce(MM_REDUCE_UNDO);
   break;
  case MM_EDIT_COPY :
   XR_MessageDisplay("RCopy cursor expression to back buffer.");
   rcmd_CopyToStack();
   break;
  case MM_EDIT_PASTE :
   XR_MessageDisplay("RPaste contents of the backup buffer.");
   rcmd_CopyFromStack();
   break;
  case MM_EDIT_CUT :
   XR_MessageDisplay("RCut cursor expression to back buffer.");
   rcmd_CutToStack();
   break;
  case MM_EDIT_TEXTEDITOR :
   XR_MessageDisplay("REdit cursor expression with a lineeditor.");
   Show_TextEditorObject();
   break;
  default : 
   DismissmenuFrame(menu);
   break;
 }
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /******/

static void mb_Find_menu(Menu menu,Menu_item item)
{ /* Switch Functions for Find menu **/
 int value=(int)xv_get(item,MENU_VALUE);
 DBUG_ENTER("mb_Find_menu");

 switch (value) {
  case MM_FIND_SR :
   mf_Search_and_Replace();
   break;
  case MM_FIND_NEXT :
   break;
  default : 
   DismissmenuFrame(menu);
   break;
 }

 DBUG_VOID_RETURN;
} /******/

static void mb_Help_menu(Menu menu,Menu_item item)
{ /* Switch Functions for Help menu **/
 int value=(int)xv_get(item,MENU_VALUE);
 DBUG_ENTER("mb_Help_menu");

 switch (value) {
  case MM_HELP_1 :
   mf_Help_SyntaxEditor(value);
   break;
  case MM_HELP_2 :
   mf_Help_SyntaxEditor(value);
   break;
  case MM_HELP_3 :
   mf_Help_SyntaxEditor(value);
   break;
  case MM_HELP_4 :
   mf_Help_SyntaxEditor(value);
   break;
  case MM_HELP_5 :
   mf_Help_SyntaxEditor(value);
   break;
  case MM_HELP_6 :
   mf_Help_SyntaxEditor(value);
   break;
  case MM_HELP_7 :
   mf_Help_SyntaxEditor(value);
   break;
  case MM_HELP_8 :
   mf_Help_SyntaxEditor(value);
  case MM_HELP_9 :
   mf_Help_SyntaxEditor(value);
   break;
  default : 
   DismissmenuFrame(menu);
   break;
 }

 DBUG_VOID_RETURN;
} /******/

static void mb_Buffer_menu(Menu menu,Menu_item item)
{ /* Switch Functions for buffer menu **/
 int value=(int)xv_get(item,MENU_VALUE);
 DBUG_ENTER("mb_Buffer_menu");

 switch (value) {
  case MM_BUFFER_WRBB :
   mf_Buffer_fcts(BUFFER_WRITE_BACKUP);
   break;
  case MM_BUFFER_WRB1 :
   mf_Buffer_fcts(BUFFER_WRITE_1);
   break;
  case MM_BUFFER_WRB2 :
   mf_Buffer_fcts(BUFFER_WRITE_2);
   break;
  case MM_BUFFER_REBB :
   mf_Buffer_fcts(BUFFER_READ_BACKUP);
   break;
  case MM_BUFFER_REB1 :
   mf_Buffer_fcts(BUFFER_READ_1);
   break;
  case MM_BUFFER_REB2 :
   mf_Buffer_fcts(BUFFER_READ_2);
   break;
  default : 
   DismissmenuFrame(menu);
   break;
 }
 SetupInputFocus(FOCUS_DEFAULT);

 DBUG_VOID_RETURN;
} /******/

/* Menu creation functions **/

static void Create_File_Menu(Panel panel)
{ /* Create menu : File-IO **/
 DBUG_ENTER("Create_File_Menu");

 MainMenu[MM_FILE].menu=(Menu)xv_create(
  XV_NULL, /* Actually no parent */
  MENU, /* Type */
  MENU_GEN_PIN_WINDOW,XRED.frame,"File operations",
  MENU_TITLE_ITEM,"File",
  MENU_NOTIFY_PROC,mb_File_menu,
  MENU_ITEM,
   MENU_STRING,"Clear program",
   MENU_VALUE,MM_FILE_CLEAR,
   NULL,
  MENU_EMPTY_ITEM,
  MENU_ITEM,
   MENU_STRING,"Load...",
   MENU_VALUE,MM_FILE_LOAD,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Include...",
   MENU_VALUE,MM_FILE_INCLUDE,
   NULL,
  MENU_EMPTY_ITEM,
  MENU_ITEM,
   MENU_STRING,"Save",
   MENU_VALUE,MM_FILE_SAVE,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Save as...",
   MENU_VALUE,MM_FILE_SAVEAS,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Save expression...",
   MENU_VALUE,MM_FILE_SAVEEXPR,
   NULL,
  MENU_EMPTY_ITEM,
  MENU_ITEM,
   MENU_STRING,"Load setups...",
   MENU_VALUE,MM_FILE_LOADSETUPS,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Save setups...",
   MENU_VALUE,MM_FILE_SAVESETUPS,
   NULL,
  MENU_EMPTY_ITEM,
  MENU_ITEM,
   MENU_STRING,"Exit",
   MENU_VALUE,MM_FILE_EXIT,
   NULL,
  NULL /* End of file io menu */
 );
 MK_MENU_BUTTON(panel,"File",MM_FILE); /* Create menu button */

 DBUG_VOID_RETURN;
} /*****/

static void Create_Reduce_Menu(Panel panel)
{ /* Create menu : Reduce **/
 DBUG_ENTER("Create_Reduce_Menu");

 MainMenu[MM_REDUCE].menu=(Menu)xv_create(
  XV_NULL, /* Actually no parent */
  MENU, /* Type */
  MENU_GEN_PIN_WINDOW,XRED.frame,"Reduction", /* Make pinup window */
  MENU_TITLE_ITEM,"Reduce expr",
  MENU_NOTIFY_PROC,mb_Reduce_menu, 
  MENU_ITEM,
   MENU_STRING,"Undo last reduction",
   MENU_VALUE,MM_REDUCE_UNDO,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Base expression",
   MENU_VALUE,MM_GO_HOME,
   NULL,
  MENU_EMPTY_ITEM,
  MENU_ITEM,
   MENU_STRING,"One step",
   MENU_VALUE,MM_REDUCE_1,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Set reduction counter...",
   MENU_VALUE,MM_REDUCE_SETCOUNTER,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Use reduction counter",
   MENU_VALUE,MM_REDUCE_CNT,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Reduce expression",
   MENU_VALUE,MM_REDUCE_REDUCEALL,
   NULL,
  NULL /* End of file io menu */
 );
 MK_MENU_BUTTON(panel,"Reduce",MM_REDUCE);

 DBUG_VOID_RETURN;
} /*****/

static void Create_Option_Menu(Panel panel)
{ /* Create menu : System setups **/
 DBUG_ENTER("Create_Option_Menu");

 MainMenu[MM_OPTION].menu=(Menu)xv_create(
  XV_NULL, /* Actually no parent */
  MENU, /* Type */
  MENU_GEN_PIN_WINDOW,XRED.frame,"Program setups",
  MENU_TITLE_ITEM,"Options",
  MENU_NOTIFY_PROC,mb_Option_menu,
  MENU_ITEM,
   MENU_STRING,"pi-RED properties...",
   MENU_VALUE,MM_OPTION_REDUMA,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Frontend properties...",
   MENU_VALUE,MM_OPTION_FRONTEND,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Syntax editor Layout",
   MENU_PULLRIGHT,xv_create(
    XV_NULL,
    MENU_TOGGLE_MENU,
    MENU_GEN_PIN_WINDOW,XRED.frame,"Syntax editor layout",
    MENU_NOTIFY_PROC,mb_syntax_editor_layout,
    MENU_TITLE_ITEM,"Set editor layout",
     MENU_ITEM,
      MENU_STRING,"Abbreviate defines",
      MENU_SELECTED,IS_DDECL(sedit_small),
      NULL,
     MENU_ITEM,
      MENU_STRING,"Abbreviate inner defines",
      MENU_SELECTED,IS_DTDECL(sedit_small),
      NULL,
     MENU_ITEM,
      MENU_STRING,"Display goalexpression",
      MENU_SELECTED,IS_DGOAL(sedit_small),
      NULL,
     MENU_ITEM,
      MENU_STRING,"Display functions",
      MENU_SELECTED,IS_DFCT(sedit_small),
      NULL,
     MENU_ITEM,
      MENU_STRING,"Small layout",
      MENU_SELECTED,IS_SMALL(sedit_small),
      NULL,
     MENU_ITEM,
      MENU_STRING,"Vertical mode",
      MENU_SELECTED,IS_DVERT(sedit_small),
      NULL,
    NULL
   ),
   NULL,
  NULL
 );
 MK_MENU_BUTTON(panel,"Options",MM_OPTION);

 DBUG_VOID_RETURN;
} /*****/

static void Create_Help_Menu(Panel panel)
{ /* menu : Help **/
 DBUG_ENTER("Create_Help_Menu");

 MainMenu[MM_HELP].menu=(Menu)xv_create(
  XV_NULL, /* Actually no parent */
  MENU,
  MENU_GEN_PIN_WINDOW,XRED.frame,"Syntax Editor help",
  MENU_TITLE_ITEM,"Help sections",
  MENU_NOTIFY_PROC,mb_Help_menu,
   MENU_ITEM, 
    MENU_STRING,MM_HELP_1_TEXT,
    MENU_VALUE,MM_HELP_1,
    NULL,
   MENU_ITEM,
    MENU_STRING,MM_HELP_2_TEXT,
    MENU_VALUE,MM_HELP_2,
    NULL,
   MENU_ITEM,
    MENU_STRING,MM_HELP_3_TEXT,
    MENU_VALUE,MM_HELP_3,
    NULL,
   MENU_ITEM,
    MENU_STRING,MM_HELP_4_TEXT,
    MENU_VALUE,MM_HELP_4,
    NULL,
   MENU_ITEM,
    MENU_STRING,MM_HELP_5_TEXT,
    MENU_VALUE,MM_HELP_5,
    NULL,
   MENU_ITEM,
    MENU_STRING,MM_HELP_6_TEXT,
    MENU_VALUE,MM_HELP_6,
    NULL,
   MENU_ITEM,
    MENU_STRING,MM_HELP_7_TEXT,
    MENU_VALUE,MM_HELP_7,
    NULL,
   MENU_ITEM,
    MENU_STRING,MM_HELP_8_TEXT,
    MENU_VALUE,MM_HELP_8,
    NULL,
   MENU_ITEM,
    MENU_STRING,MM_HELP_9_TEXT,
    MENU_VALUE,MM_HELP_9,
    NULL,
  NULL /* End of menu */
 );
 MK_MENU_BUTTON(panel,"Help",MM_HELP);

 DBUG_VOID_RETURN;
} /*****/

static void Create_Buffer_Menu(Panel panel)
{ /* Create buffer handling menu **/
 DBUG_ENTER("Create_Buffer_Menu");

 MainMenu[MM_BUFFER].menu=(Menu)xv_create(
  XV_NULL,
  MENU,
  MENU_GEN_PIN_WINDOW,XRED.frame,"Buffers",
  MENU_TITLE_ITEM,"Buffer functions",
  MENU_NOTIFY_PROC,mb_Buffer_menu,
  MENU_NROWS,3,
  MENU_NCOLS,2,
  MENU_ITEM,
   MENU_STRING,"Write to backup buffer",
   MENU_VALUE,MM_BUFFER_WRBB,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Write to buffer 1",
   MENU_VALUE,MM_BUFFER_WRB1,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Write to buffer 2",
   MENU_VALUE,MM_BUFFER_WRB2,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Read from backup buffer",
   MENU_VALUE,MM_BUFFER_REBB,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Read from buffer 1",
   MENU_VALUE,MM_BUFFER_REB1,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Read from buffer 2",
   MENU_VALUE,MM_BUFFER_REB2,
   NULL,
  NULL
 );
 MK_MENU_BUTTON(panel,"Buffers",MM_BUFFER);

 DBUG_VOID_RETURN;
} /*****/

static void Create_Extras_Menu(Panel panel)
{ /* Create Extra function menu **/
 DBUG_ENTER("Create_Extras_Menu");

 MainMenu[MM_EXTRA].menu=(Menu)xv_create(
  XV_NULL,
  MENU,
  MENU_GEN_PIN_WINDOW,XRED.frame,"Extra",
  MENU_TITLE_ITEM,"Special functions",
  MENU_NOTIFY_PROC,mb_Extra_menu,
  MENU_ITEM,
   MENU_STRING,"Control toolbox...",
   MENU_VALUE,MM_EXTRA_TOOLBOX,
   NULL,
  NULL
 );
 MK_MENU_BUTTON(panel,"Extra",MM_EXTRA);

 DBUG_VOID_RETURN;
} /*****/

static void Create_Edit_Menu(Panel panel)
{ /* Create Edit function menu **/
 DBUG_ENTER("Create_Edit_Menu");

 MainMenu[MM_EDIT].menu=(Menu)xv_create(
  XV_NULL,
  MENU,
  MENU_GEN_PIN_WINDOW,XRED.frame,"Edit",
  MENU_NOTIFY_PROC,mb_Edit_menu,
  MENU_TITLE_ITEM,"Edit",
  MENU_ITEM,
   MENU_STRING,"Undo",
   MENU_VALUE,MM_EDIT_UNDO,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Copy",
   MENU_VALUE,MM_EDIT_COPY,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Paste",
   MENU_VALUE,MM_EDIT_PASTE,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Cut",
   MENU_VALUE,MM_EDIT_CUT,
   NULL,
  MENU_EMPTY_ITEM,
  MENU_ITEM,
   MENU_STRING,"Texteditor...",
   MENU_VALUE,MM_EDIT_TEXTEDITOR,
   NULL,
  NULL
 );
 MK_MENU_BUTTON(panel,"Edit",MM_EDIT);

 DBUG_VOID_RETURN;
} /*****/

static void Create_Find_Menu(Panel panel)
{ /* Create Find function menu **/
 DBUG_ENTER("Create_Find_Menu");
 DBUG_VOID_RETURN;

 MainMenu[MM_FIND].menu=(Menu)xv_create(
  XV_NULL,
  MENU,
  MENU_GEN_PIN_WINDOW,XRED.frame,"Find",
  MENU_NOTIFY_PROC,mb_Find_menu,
  MENU_TITLE_ITEM,"Find",
  MENU_ITEM,
   MENU_STRING,"Search and replace...",
   MENU_VALUE,MM_FIND_SR,
   NULL,
  MENU_ITEM,
   MENU_STRING,"Next",
   MENU_VALUE,MM_FIND_NEXT,
   NULL,
  NULL
 );
 MK_MENU_BUTTON(panel,"Find",MM_FIND);

} /*****/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void mb_CreateMenuPanel(Panel panel)
{ /* Create all menu buttons **/
 DBUG_ENTER("mb_CreateMenuPanel");

 Create_File_Menu(panel);
 Create_Edit_Menu(panel);
 Create_Find_Menu(panel);
 Create_Buffer_Menu(panel);
 Create_Reduce_Menu(panel);
 Create_Option_Menu(panel);
 Create_Extras_Menu(panel);
 Create_Help_Menu(panel);

 MenuItems.pi_filename=(Panel_item)xv_create(
  panel,
  PANEL_TEXT,
  PANEL_NEXT_ROW,-1,
  PANEL_LABEL_STRING,"Filename:",
  PANEL_VALUE,"",
  PANEL_NOTIFY_PROC,new_filename_proc,
  PANEL_VALUE_DISPLAY_LENGTH,30,
  NULL
 );

 MenuItems.pi_nr_te=(Panel_item)xv_create(
  panel,
  PANEL_TEXT,
  PANEL_LABEL_STRING,"Textobjects:",
  PANEL_VALUE,"0",
  PANEL_VALUE_DISPLAY_LENGTH,3,
  PANEL_READ_ONLY,TRUE,
  NULL
 );


 DBUG_VOID_RETURN;
} /*****/

static char FBUFFER[512];

void mc_Set_Filename(char *name)
{ /* Set filename to left footer side **/
 char *str;
 DBUG_ENTER("mc_Set_Filename");

 if (XRED.filename) free(XRED.filename);
 XRED.filename=(char *)strdup(name);
 strcpy(FBUFFER,name);
 str=strrchr(FBUFFER,'.'); 
 if (str) *str='\0';
 xv_set(MenuItems.pi_filename,PANEL_VALUE,FBUFFER,NULL);

 DBUG_VOID_RETURN;
} /*****/

char *mc_GetFilename(void)
{ /* Returns the name of the actual file **/
 char *name;
 DBUG_ENTER("mc_GetFilename");

 name=strrchr(FBUFFER,'/');
 
 if (name==NULL) DBUG_RETURN(FBUFFER);
 else {
  if (*name=='/') {
   name++;
  }
  DBUG_RETURN(name);
 }

} /*******/

char *mc_GetFilepath(void)
{ /* Returns the name of the actual file **/
 char *name;
 static char BUF[512];
 DBUG_ENTER("mc_GetFilepath");

 strcpy(BUF,FBUFFER);
 name=strrchr(BUF,'/');
 
 if (name==NULL) DBUG_RETURN("");
 else {
  if (*name=='/') {
   *name='\0';
  }
  DBUG_RETURN(name);
 }

} /*******/

void mc_SetTextObjectNumber(int number)
{ /* Set number of textobject in use **/
 DBUG_ENTER("mc_SetTextObjectNumber");

 sprintf(BUFFER,"%d",number);
 xv_set(MenuItems.pi_nr_te,PANEL_VALUE,BUFFER,NULL);

 DBUG_VOID_RETURN;
} /********/
