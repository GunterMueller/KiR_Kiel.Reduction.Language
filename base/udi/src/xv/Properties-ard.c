/**********************************************************************************
***********************************************************************************
**
**   File        : Properties-ard.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 5.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for XWindows Front for the
**                 Reduma. This module implements reduma property setups.
**                 For apply,reset and default values
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
#include "Properties-ard.h"
#include "Properties-create.h"
#include "FileIO-create.h"
#include "Reduma-cmd.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

#include "macros/Proper-ard.mac"

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void Update_SyntaxEditor(void)
{ /* Set new values to panelitems **/
 DBUG_ENTER("Update_SyntaxEditor");

 UPDATE_PI(SE,c_up,c_up_value);
 UPDATE_PI(SE,c_down,c_down_value);
 UPDATE_PI(SE,c_right,c_right_value);
 UPDATE_PI(SE,c_left,c_left_value);
 UPDATE_PI(SE,c_home,c_home_value);
 UPDATE_PI(SE,c_return,c_return_value);

 DBUG_VOID_RETURN;
} /*******/

static void Update_NumberFormat(void)
{ /* Set new values to panelitems **/
 DBUG_ENTER("Update_NumberFormat");

 UPDATE_PI(PNF,format,numberformat);
 UPDATE_PI(PNF,basenumber,numbervalue);
 xv_set( /* En/Disable Basenumber panel item */
  SetupProps.PNF.basenumber,
  PANEL_INACTIVE,SetupProps.PNF.numberformat==PNF_VAR_FORMAT,
  NULL
 );

 DBUG_VOID_RETURN;
} /*******/

static void Update_CalcPrecison(void)
{ /* Set new values to panelitems **/
 DBUG_ENTER("Update_CalcPrecison");

 UPDATE_PI_CP(CP,PI_trunc,trunc_value);
 UPDATE_PI_CP(CP,PI_mult,mult_value);
 UPDATE_PI_CP(CP,PI_div,div_value);

 DBUG_VOID_RETURN;
} /*******/

static void Update_StackSize(void)
{ /* Set new values to panelitems **/
 DBUG_ENTER("Update_StackSize");

 UPDATE_PI(SS,SS_qstack,qstack_value);
 UPDATE_PI(SS,SS_istack,istack_value);
 UPDATE_PI(SS,SS_mstack,mstack_value);
 CalcTotalStackSize(); /* Update memory usage */

 DBUG_VOID_RETURN;
} /*******/

static void Update_HeapSize(void)
{ /* Set new values to panelitems **/
 DBUG_ENTER("Update_HeapSize");

 UPDATE_PI(HS,HS_nr_descr,nr_descr_value);
 UPDATE_PI(HS,HS_size,size_value);
 CalcTotalHeapSize(1); /* Update memory usage */

 DBUG_VOID_RETURN;
} /*******/

static void Update_Misc(void)
{ /* Set new values to panelitems **/
 DBUG_ENTER("Update_Misc");

 UPDATE_PI(MI,MI_redcnt,redcnt_value);
 UPDATE_PI(MI,MI_beta,beta_value);

 DBUG_VOID_RETURN;
} /*******/

static void Update_FileExtender(void)
{ /* Set new values to panelitems **/
 DBUG_ENTER("Update_FileExtender");

 UPDATE_PI_FE(ed);
 UPDATE_PI_FE(pp);
 UPDATE_PI_FE(doc);
 UPDATE_PI_FE(prt);
 UPDATE_PI_FE(asc);
 UPDATE_PI_FE(red);
 UPDATE_PI_FE(pre);
 
 UPDATE_PI(FE,load,load_format);
 UPDATE_PI(FE,save,save_format);
 fc_Update_Filerequester(); /* Update Filerequester code */

 DBUG_VOID_RETURN;
} /*******/

static void Update_PropertyButtons(int Update)
{ /* Set all values of Buttons object to their defined values **/
 DBUG_ENTER("Update_PropertyButtons");

 UPDATE_PI(PP,apply_set,apply_set_value);
 UPDATE_PI(PP,reset_set,reset_set_value);
 UPDATE_PI(PP,defaults_set,defaults_set_value);

 DBUG_VOID_RETURN;
} /*******/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERN FUNCTIONS **********************************/
/*********************************************************************************/

void CenterARDButtons(PropStandard *ps)
{ /* Arrange ARD buttons for property **/
 DBUG_ENTER("CenterARDButtons");

 if (ps) { /* Check pointer */
  int pxig=(int)xv_get(ps->panel,PANEL_ITEM_X_GAP);
  int ax = ((int)xv_get(ps->panel,XV_WIDTH)
         - (int)xv_get(ps->apply,XV_WIDTH)
         - (int)xv_get(ps->reset,XV_WIDTH)
         - (int)xv_get(ps->defaults,XV_WIDTH)
         - 3 * pxig)/3;
  if (ax<0) ax=0;
  xv_set(ps->apply,PANEL_ITEM_X,ax,NULL);
  xv_set(ps->defaults,PANEL_ITEM_X,ax+pxig+(int)xv_get(ps->apply,XV_WIDTH),NULL);
  xv_set(
   ps->reset,PANEL_ITEM_X,
   (int)xv_get(ps->defaults,PANEL_ITEM_X)+pxig+(int)xv_get(ps->defaults,XV_WIDTH),
   NULL 
  );
  xv_set(
   ps->close,PANEL_ITEM_X,
   (int)xv_get(ps->reset,PANEL_ITEM_X)+pxig+(int)xv_get(ps->reset,XV_WIDTH),
   NULL 
  );
 }

 DBUG_VOID_RETURN;
} /******/

void SetDefaults_All(void)
{ /* Set all properties to their default values **/
 DBUG_ENTER("SetDefaults_All");

 SetDefaults_SyntaxEditor(FALSE);
 SetDefaults_NumberFormat(FALSE);
 SetDefaults_CalcPrecison(FALSE);
 SetDefaults_StackSize(FALSE);
 SetDefaults_HeapSize(FALSE);
 SetDefaults_Misc(FALSE);
 SetDefaults_FileExtender(FALSE); 
 SetDefaults_PropertyButtons(FALSE);

 DBUG_VOID_RETURN;
} /*******/ 

void SetDefaults_SyntaxEditor(int Update)
{ /* Set all values of Syntaxeditorobject to their default values **/
 DBUG_ENTER("SetDefaults_SyntaxEditor");

 SetupProps.SE.c_up_value=DEF_CURSOR_UP;
 SetupProps.SE.c_down_value=DEF_CURSOR_DOWN;
 SetupProps.SE.c_right_value=DEF_CURSOR_RIGHT;
 SetupProps.SE.c_left_value=DEF_CURSOR_LEFT;
 SetupProps.SE.c_home_value=DEF_CURSOR_HOME;
 SetupProps.SE.c_return_value=DEF_RETURN_KEY;
 if (Update) Update_SyntaxEditor();

 DBUG_VOID_RETURN;
} /****/

void SetDefaults_NumberFormat(int Update)
{ /* Set all values of NumberFormatobject to their default values **/
 DBUG_ENTER("SetDefaults_NumberFormat");

 SetupProps.PNF.numberformat=DEF_PNF_FORMAT;
 SetupProps.PNF.numbervalue=DEF_PNF_VALUE;

 if (Update) Update_NumberFormat();

 DBUG_VOID_RETURN;
} /****/

void SetDefaults_CalcPrecison(int Update)
{ /* Set all values of CalcPrecisonobject to their default values **/
 DBUG_ENTER("SetDefaults_CalcPrecison");

 SetupProps.CP.trunc_value=DEF_CP_TRUNC;
 SetupProps.CP.mult_value=DEF_CP_MULT;
 SetupProps.CP.div_value=DEF_CP_DIV;

 if (Update) Update_CalcPrecison();

 DBUG_VOID_RETURN;
} /****/

void SetDefaults_StackSize(int Update)
{ /* Set all values of StackSizeobject to their default values **/
 DBUG_ENTER("SetDefaults_StackSize");

 SetupProps.SS.qstack_value=DEF_SS_QSTACK;
 SetupProps.SS.istack_value=DEF_SS_ISTACK;
 SetupProps.SS.mstack_value=DEF_SS_MSTACK;

 if (Update) Update_StackSize();

 DBUG_VOID_RETURN;
} /****/

void SetDefaults_HeapSize(int Update)
{ /* Set all values of HeapSize to their default values **/
 DBUG_ENTER("SetDefaults_HeapSize");

 SetupProps.HS.nr_descr_value=DEF_HS_NRDESCR/DEF_NR_DESCR_FACTOR;
 SetupProps.HS.size_value=DEF_HS_SIZE/DEF_NR_SIZE_FACTOR;

 if (Update) Update_HeapSize();

 DBUG_VOID_RETURN;
} /****/

void SetDefaults_Misc(int Update)
{ /* Set all values of Miscobject to their default values **/
 DBUG_ENTER("SetDefaults_Misc");

 SetupProps.MI.redcnt_value=DEF_MI_REDCNT;
 SetupProps.MI.beta_value=DEF_BETAVALUES;

 if (Update) Update_Misc();

 DBUG_VOID_RETURN;
} /****/

void SetDefaults_FileExtender(int Update)
{ /* Set all values of FileExtenderobject to their default values **/
 DBUG_ENTER("SetDefaults_FileExtender");

 SetupProps.FE.load_format=DEF_FE_LOADFORMAT;
 SetupProps.FE.save_format=DEF_FE_SAVEFORMAT;

 strcpy(SetupProps.FE.ed_p_value,ExpandDir(DEF_FE_ED_PATH1));
 strcpy(SetupProps.FE.ed_e_value,DEF_FE_ED_EXT);
 strcpy(SetupProps.FE.pp_p_value,ExpandDir(DEF_FE_PP_PATH1));
 strcpy(SetupProps.FE.pp_e_value,DEF_FE_PP_EXT);
 strcpy(SetupProps.FE.doc_p_value,ExpandDir(DEF_FE_DOC_PATH1));
 strcpy(SetupProps.FE.doc_e_value,DEF_FE_DOC_EXT);
 strcpy(SetupProps.FE.prt_p_value,ExpandDir(DEF_FE_PRT_PATH1));
 strcpy(SetupProps.FE.prt_e_value,DEF_FE_PRT_EXT);
 strcpy(SetupProps.FE.asc_p_value,ExpandDir(DEF_FE_ASC_PATH1));
 strcpy(SetupProps.FE.asc_e_value,DEF_FE_ASC_EXT);
 strcpy(SetupProps.FE.red_p_value,ExpandDir(DEF_FE_RED_PATH1));
 strcpy(SetupProps.FE.red_e_value,DEF_FE_RED_EXT);
 strcpy(SetupProps.FE.pre_p_value,ExpandDir(DEF_FE_PRE_PATH1));
 strcpy(SetupProps.FE.pre_e_value,DEF_FE_PRE_EXT);

 if (Update) Update_FileExtender();

 DBUG_VOID_RETURN;
} /****/

void SetDefaults_PropertyButtons(int Update)
{ /* Set all values of Propertyobject to their default values **/
 DBUG_ENTER("SetDefaults_PropertyButtons");

 SetupProps.PP.apply_set_value=DEF_PP_APPLY_MODE;
 SetupProps.PP.reset_set_value=DEF_PP_RESET_MODE;
 SetupProps.PP.defaults_set_value=DEF_PP_DEFAULTS_MODE;

 if (Update) Update_PropertyButtons(Update);

 DBUG_VOID_RETURN;
} /*******/

/*******/

void Apply_PropertyButtons(int Update)
{ /* Set all values of Buttons object to their defined values **/
 DBUG_ENTER("Apply_PropertyButtons");

 GET_PI(PP,apply_set,apply_set_value,int);
 GET_PI(PP,reset_set,reset_set_value,int);
 GET_PI(PP,defaults_set,defaults_set_value,int);

 DBUG_VOID_RETURN;
} /*******/

void Apply_SyntaxEditor(int Update)
{ /* Apply requester changes to data structure values for the syntaxeditorobj **/
 DBUG_ENTER("Apply_SyntaxEditor");

 GET_PI(SE,c_up,c_up_value,int);
 GET_PI(SE,c_down,c_down_value,int);
 GET_PI(SE,c_left,c_left_value,int);
 GET_PI(SE,c_right,c_right_value,int);
 GET_PI(SE,c_home,c_home_value,int);
 GET_PI(SE,c_return,c_return_value,int);
 Setup_SyntaxEditor(SEM_KEYS);

 DBUG_VOID_RETURN;
} /******/

void Apply_NumberFormat(int Update)
{ /* Set all values of NumberFormatobject to their default values **/
 DBUG_ENTER("Apply_NumberFormat");

 GET_PI(PNF,format,numberformat,int);
 GET_PI(PNF,basenumber,numbervalue,int);
 Update_Parameter();

 DBUG_VOID_RETURN;
} /****/

void Apply_CalcPrecison(int Update)
{ /* Set all values of CalcPrecisonobject to their default values **/
 DBUG_ENTER("Apply_CalcPrecison");

 GET_PI(CP,PI_trunc,trunc_value,int);
 GET_PI(CP,PI_mult,mult_value,int);
 GET_PI(CP,PI_div,div_value,int);
 Update_Parameter();

 DBUG_VOID_RETURN;
} /****/

void Apply_StackSize(int Update)
{ /* Set all values of StackSizeobject to their default values **/
 DBUG_ENTER("Apply_StackSize");

 GET_PI(SS,SS_qstack,qstack_value,int);
 GET_PI(SS,SS_istack,istack_value,int);
 GET_PI(SS,SS_mstack,mstack_value,int);
 Update_Parameter();

 DBUG_VOID_RETURN;
} /****/

void Apply_HeapSize(int Update)
{ /* Set all values of HeapSize to their default values **/
 DBUG_ENTER("Apply_HeapSize");

 GET_PI(HS,HS_nr_descr,nr_descr_value,int);
 GET_PI(HS,HS_size,size_value,int);
 Update_Parameter();

 DBUG_VOID_RETURN;
} /****/

void Apply_Misc(int Update)
{ /* Set all values of Miscobject to their default values **/
 DBUG_ENTER("Apply_Misc");

 GET_PI(MI,MI_redcnt,redcnt_value,int);
 GET_PI(MI,MI_beta,beta_value,int);
 Update_Parameter();

 DBUG_VOID_RETURN;
} /****/

void Apply_FileExtender(int Update)
{ /* Set all values of FileExtenderobject to their default values **/
 DBUG_ENTER("Apply_FileExtender");

 GET_PI_FE(ed);
 GET_PI_FE(pp);
 GET_PI_FE(doc);
 GET_PI_FE(prt);
 GET_PI_FE(asc);
 GET_PI_FE(red);
 GET_PI_FE(pre);
 
 GET_PI(FE,load,load_format,int);
 GET_PI(FE,save,save_format,int);
 Update_Parameter();

 DBUG_VOID_RETURN;
} /****/

/*******/

void Reset_PropertyButtons(int Update)
{ /* ReSet all values of texteditor object to their values **/
 DBUG_ENTER("Reset_PropertyButtons");

 Update_PropertyButtons(Update);

 DBUG_VOID_RETURN;
} /****/

void Reset_NumberFormat(int Update)
{ /* ReSet all values of NumberFormatobject to their values **/
 DBUG_ENTER("Reset_NumberFormat");

 Update_NumberFormat();

 DBUG_VOID_RETURN;
} /****/

void Reset_CalcPrecison(int Update)
{ /* ReSet all values of CalcPrecisonobject to their values **/
 DBUG_ENTER("Reset_CalcPrecison");

 Update_CalcPrecison();

 DBUG_VOID_RETURN;
} /****/

void Reset_StackSize(int Update)
{ /* ReSet all values of StackSizeobject to their values **/
 DBUG_ENTER("Reset_StackSize");

 Update_StackSize();

 DBUG_VOID_RETURN;
} /****/

void Reset_HeapSize(int Update)
{ /* ReSet all values of HeapSize to their values **/
 DBUG_ENTER("Reset_HeapSize");

 Update_HeapSize();

 DBUG_VOID_RETURN;
} /****/

void Reset_Misc(int Update)
{ /* ReSet all values of Miscobject to their values **/
 DBUG_ENTER("Reset_Misc");

 Update_Misc();

 DBUG_VOID_RETURN;
} /****/

void Reset_FileExtender(int Update)
{ /* ReSet all values of FileExtenderobject to their values **/
 DBUG_ENTER("Reset_FileExtender");

 Update_FileExtender();

 DBUG_VOID_RETURN;
} /****/

void Reset_SyntaxEditor(int Update)
{ /* ReSet all values of Syntaxeditorobject to their values **/
 DBUG_ENTER("Reset_SyntaxEditor");

 Update_SyntaxEditor();

 DBUG_VOID_RETURN;
} /*********/

void Update_Properties(void)
{ /* Get editor values and apply them to Xparameter property values **/
 DBUG_ENTER("Update_Properties");

 rcmd_GetEditorParameter(); /* Get parameter from editor */

 /* Setup Fileextendervalues **/
 strcpy(SetupProps.FE.ed_e_value,EditExt[0]);
 strcpy(SetupProps.FE.pp_e_value,EditExt[1]);
 strcpy(SetupProps.FE.doc_e_value,EditExt[2]);
 strcpy(SetupProps.FE.prt_e_value,EditExt[3]);
 strcpy(SetupProps.FE.asc_e_value,EditExt[4]);
 strcpy(SetupProps.FE.red_e_value,EditExt[5]);
 strcpy(SetupProps.FE.pre_e_value,EditExt[6]);
 strcpy(SetupProps.FE.ed_p_value,EditLibs[0]);
 strcpy(SetupProps.FE.pp_p_value,EditLibs[1]);
 strcpy(SetupProps.FE.doc_p_value,EditLibs[2]);
 strcpy(SetupProps.FE.prt_p_value,EditLibs[3]);
 strcpy(SetupProps.FE.asc_p_value,EditLibs[4]);
 strcpy(SetupProps.FE.red_p_value,EditLibs[5]);
 strcpy(SetupProps.FE.pre_p_value,EditLibs[6]);
 Update_FileExtender();  /* Apply changes */

 /* Setup Misc Property **/
 SetupProps.MI.redcnt_value=EditParms.redcnt;
 SetupProps.MI.beta_value=EditParms.beta_count;
 Update_Misc(); /* Apply changes */

 /* Setup Calculation precison **/
 SetupProps.CP.trunc_value=EditParms.truncation;
 SetupProps.CP.mult_value=EditParms.prec_mult;
 SetupProps.CP.div_value=EditParms.precision;
 Update_CalcPrecison();  /* Apply changes */
 
 /* Setup Heapvalues **/
 SetupProps.HS.size_value=EditParms.heapsize/DEF_NR_SIZE_FACTOR;
 SetupProps.HS.nr_descr_value=EditParms.heapdes/DEF_NR_DESCR_FACTOR;
 Update_HeapSize(); /* Apply changes */

 /* Setup Stack values **/
 SetupProps.SS.qstack_value=EditParms.qstacksize;
 SetupProps.SS.istack_value=EditParms.istacksize;
 SetupProps.SS.mstack_value=EditParms.mstacksize;
 Update_StackSize(); /* Apply changes */

 /* Setup Numberformat values **/
 SetupProps.PNF.numberformat=1-EditParms.formated;
 SetupProps.PNF.numbervalue=EditParms.base;
 Update_NumberFormat(); /* Apply changes */


 DBUG_VOID_RETURN;
} /*****/

void Update_Parameter(void)
{ /* Apply Xparameter property values to editor **/
 DBUG_ENTER("Update_Parameter");

 /* Setup Fileextendervalues **/
 strcpy(EditExt[0],SetupProps.FE.ed_e_value);
 strcpy(EditExt[1],SetupProps.FE.pp_e_value);
 strcpy(EditExt[2],SetupProps.FE.doc_e_value);
 strcpy(EditExt[3],SetupProps.FE.prt_e_value);
 strcpy(EditExt[4],SetupProps.FE.asc_e_value);
 strcpy(EditExt[5],SetupProps.FE.red_e_value);
 strcpy(EditExt[6],SetupProps.FE.pre_e_value);
 strcpy(EditLibs[0],SetupProps.FE.ed_p_value);
 strcpy(EditLibs[1],SetupProps.FE.pp_p_value);
 strcpy(EditLibs[2],SetupProps.FE.doc_p_value);
 strcpy(EditLibs[3],SetupProps.FE.prt_p_value);
 strcpy(EditLibs[4],SetupProps.FE.asc_p_value);
 strcpy(EditLibs[5],SetupProps.FE.red_p_value);
 strcpy(EditLibs[6],SetupProps.FE.pre_p_value);

 /* Setup Misc Property **/
 EditParms.redcnt=SetupProps.MI.redcnt_value;
 EditParms.beta_count=SetupProps.MI.beta_value;

 /* Setup Calculation precison **/
 EditParms.truncation=SetupProps.CP.trunc_value;
 EditParms.prec_mult=SetupProps.CP.mult_value;
 EditParms.precision=SetupProps.CP.div_value;

 /* Setup Heapvalues **/
 EditParms.heapsize=SetupProps.HS.size_value*DEF_NR_SIZE_FACTOR;
 EditParms.heapdes=SetupProps.HS.nr_descr_value*DEF_NR_DESCR_FACTOR;

 /* Setup Stack values **/
 EditParms.qstacksize=SetupProps.SS.qstack_value;
 EditParms.istacksize=SetupProps.SS.istack_value;
 EditParms.mstacksize=SetupProps.SS.mstack_value;

 /* Setup Numberformat values **/
 EditParms.formated=1-SetupProps.PNF.numberformat;
 EditParms.base=SetupProps.PNF.numbervalue;

 Setup_SyntaxEditor(SEM_PARMS);


 DBUG_VOID_RETURN;
} /*****/

void Setup_SyntaxEditor(int mode)
{ /* Set parameter, modes and names to the syntaxeditor **/
 DBUG_ENTER("Setup_SyntaxEditor");

 if ((mode&SEM_KEYS)==SEM_KEYS) { /* Setup syntaxeditor keymodes ? */
  rcmd_CursorKeySetting( /* Send setup to syntax editor */
   SetupProps.SE.c_up_value,
   SetupProps.SE.c_down_value,
   SetupProps.SE.c_left_value,
   SetupProps.SE.c_right_value,
   SetupProps.SE.c_home_value,
   SetupProps.SE.c_return_value
  );
 }

 if ((mode&SEM_PARMS)==SEM_PARMS) { /* Setup editor parameter ? */
  rcmd_SetEditorParameter(); /* parms to editor */
 }

 DBUG_VOID_RETURN;
} /********/
