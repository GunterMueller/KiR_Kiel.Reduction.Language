/**********************************************************************************
***********************************************************************************
**
**   File        : Properties-fcts.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 05.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : System properties
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_PROPERTIESFCTS_HEADER_
#define _XREDUMA_PROPERTIESFCTS_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <xview/xview.h>
#include <xview/panel.h>

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct PropStandardTag { /* Standard Property items **/
 Frame frame; /* Property frame (window) */
 Panel panel; /* Controll panel */
 Panel_button_item apply; /* Accept setups */
 Panel_button_item defaults; /* Default setups */
 Panel_button_item reset; /* reset setups */
 Panel_button_item close; /* close property setups */
} PropStandard; /****/

typedef struct SystemPropertiesTag { /* Systemsetup window */
 /* Properties variables for Reduma setups **/
 Frame             	frame; /* property frame */
 Panel                  panel; /* menu panel */
 Panel_item             category; /* Property selector */
 Panel_button_item      apply; /* Accept setups */

 struct PropsNumberFormat {
  PropStandard ps; /* Property standard items */
  int numberformat;
  int numbervalue;
  Panel_item basenumber; /* Base number value */
  Panel_item format; /* Number format */
 } PNF;
 struct PrecisonTag { /* Calculation precison */
  PropStandard ps; /* Property standard items */
  int trunc_value;
  int mult_value;
  int div_value;
  Panel_item PI_trunc;
  Panel_item PI_trunc_l;
  Panel_item PI_mult;
  Panel_item PI_mult_l;
  Panel_item PI_div;
  Panel_item PI_div_l;
 } CP;
 struct StackSizeTag { /* Defines size of different stacks */
  PropStandard ps; /* Property standard items */
  int qstack_value;
  int istack_value;
  int mstack_value;
  Panel_item SS_qstack;
  Panel_item SS_istack;
  Panel_item SS_mstack;
  Panel_item SS_mem;
 } SS;
 struct HeapSizeTag { /* Defines size of heap */
  PropStandard ps; /* Property standard items */
  int nr_descr_value;
  int size_value;
  Panel_item HS_size;
  Panel_item HS_nr_descr;
  Panel_item HS_mem;
 } HS;
 struct MiscTag { /* Defines various setups */
  PropStandard ps; /* Property standard items */
  int redcnt_value;
  int beta_value;
  Panel_item MI_beta;
  Panel_item MI_redcnt;
 } MI;
 struct FilextenderTag { /* Defines fileextender setups */
  PropStandard ps; /* Property standard items */
  char *ed_p_value,*ed_e_value;
  char *pp_p_value,*pp_e_value;
  char *doc_p_value,*doc_e_value;
  char *prt_p_value,*prt_e_value;
  char *asc_p_value,*asc_e_value;
  char *red_p_value,*red_e_value;
  char *pre_p_value,*pre_e_value;
  int load_format;
  int save_format;
  Panel_item FE_ed,FE_ed_m,FE_ed_e;
  Panel_item FE_doc,FE_doc_m,FE_doc_e;
  Panel_item FE_pp,FE_pp_m,FE_pp_e;
  Panel_item FE_prt,FE_prt_m,FE_prt_e;
  Panel_item FE_asc,FE_asc_m,FE_asc_e;
  Panel_item FE_red,FE_red_m,FE_red_e;
  Panel_item FE_pre,FE_pre_m,FE_pre_e;
  Panel_item load,save;
 } FE;

 /* Properties variables for XReduma setups **/
 Frame             	xframe; /* property frame */
 Panel                  xpanel; /* menu panel */
 Panel_item             xcategory; /* Property selector */
 Panel_button_item      xapply; /* Accept setups */

 struct SyntaxEditorTag { /* Setup for the syntax editor */
  PropStandard ps; /* Property standard items */
  char c_up_value;
  char c_down_value;
  char c_right_value;
  char c_left_value;
  char c_home_value;
  char c_return_value;
  Panel_item c_up;
  Panel_item c_down;
  Panel_item c_left;
  Panel_item c_right;
  Panel_item c_home;
  Panel_item c_return;
 } SE;

 struct PropsTag { /* Generell property behavioir **/
  PropStandard ps; /* Property standard items */
  int apply_set_value;
  int reset_set_value;
  int defaults_set_value;
  int call_func_value;
  Panel_item apply_set;
  Panel_item defaults_set;
  Panel_item reset_set;
  Panel_item call_func;
 } PP; 
} SystemProperties; /*******/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/* Offset for XWindows properties **/
#define XPROPERTY_ADD   16
#define PROP_SYNTAXEDITOR	XPROPERTY_ADD
#define PROP_STANDARD		(XPROPERTY_ADD+1)

/* Defines for Numberformat requester **/
#define PNF_FIX_FORMAT	1
#define PNF_VAR_FORMAT	0
#define DEF_PNF_FORMAT	PNF_VAR_FORMAT
#define DEF_PNF_VALUE	10000

/* Defines for Calcprecison requester **/
#define DEF_CP_TRUNC	5	
#define DEF_CP_MULT	-1
#define DEF_CP_DIV	5

/* Defines for stacksize requester **/
#define DEF_SS_QSTACK	50000
#define DEF_SS_ISTACK	50000
#define DEF_SS_MSTACK	50000

/* Defines for heapsize requester **/
#define DEF_HS_NRDESCR	200
#define DEF_HS_SIZE	250000

/* Defines for misc requester **/
#define DEF_MI_REDCNT	1
#define DEF_BETAVALUES	0

/* Defines for the syntax editor setup requester **/
#define DEF_CURSOR_UP		2
#define DEF_CURSOR_DOWN		2
#define DEF_CURSOR_LEFT		0
#define DEF_CURSOR_RIGHT	0
#define DEF_CURSOR_HOME		0
#define DEF_RETURN_KEY		2

/* Defines for fileextender requester **/

#define FE_FILEIO_FORMAT_ED	0
#define FE_FILEIO_FORMAT_RED	1
#define FE_FILEIO_FORMAT_PP	2
#define DEF_FE_LOADFORMAT	FE_FILEIO_FORMAT_ED
#define DEF_FE_SAVEFORMAT	FE_FILEIO_FORMAT_ED

#define DEF_FE_ED_PATH2		"edlib"
#define DEF_FE_ED_PATH1		"$HOME/red/edlib"
#define DEF_FE_ED_EXT		".ed"
#define DEF_FE_PP_PATH2		"pplib"
#define DEF_FE_PP_PATH1		"$HOME/red/pplib"
#define DEF_FE_PP_EXT		".pp"
#define DEF_FE_DOC_PATH2	"doclib"
#define DEF_FE_DOC_PATH1	"$HOME/red/doclib"
#define DEF_FE_DOC_EXT		".doc"
#define DEF_FE_PRT_PATH2	"prtlib"
#define DEF_FE_PRT_PATH1	"$HOME/red/prtlib"
#define DEF_FE_PRT_EXT		".prt"
#define DEF_FE_ASC_PATH2	"asclib"
#define DEF_FE_ASC_PATH1	"$HOME/red/asclib"
#define DEF_FE_ASC_EXT		".asc"
#define DEF_FE_RED_PATH2	"redlib"
#define DEF_FE_RED_PATH1	"$HOME/red/redlib"
#define DEF_FE_RED_EXT		".red"
#define DEF_FE_PRE_PATH2	"prelib"
#define DEF_FE_PRE_PATH1	"$HOME/red/prelib"
#define DEF_FE_PRE_EXT		".pre"

#define ed_SELECT		1
#define ed_DIR1			2
#define ed_DIR2			3
#define pp_SELECT		4
#define pp_DIR1			5
#define pp_DIR2			6
#define doc_SELECT		7
#define doc_DIR1		8
#define doc_DIR2		9
#define prt_SELECT		10
#define prt_DIR1		11
#define prt_DIR2		12
#define asc_SELECT		13
#define asc_DIR1		14
#define asc_DIR2		15
#define red_SELECT		16
#define red_DIR1		17
#define red_DIR2		18
#define pre_SELECT		19
#define pre_DIR1		20
#define pre_DIR2		21

/* Defines for buttontex requester **/
#define APPLY_FUNC		1
#define DEFAULTS_FUNC		2
#define RESET_FUNC		4
#define DEF_PP_CALL_FUNC	APPLY_FUNC
#define DEF_PP_APPLY_MODE	2
#define DEF_PP_DEFAULTS_MODE	2
#define DEF_PP_RESET_MODE	2

/* Reduma machine type **/
#define REDTYP_INTER    1
#define REDTYP_COMP     2
#define REDTYP_DEFAULT  REDTYP_INTER

/* Reduma dependend values **/
#define MIN_FREE_HEAP           4096
#define STACKELEM_SIZE          4
#define HEAPDESCRITOR_SIZE      16

#define LNEU_NR_OF_ISTACKS      5
#define LNEU_NR_OF_MSTACKS      2
#define LNEU_NR_OF_QSTACKS      3

#define LRED_NR_OF_ISTACKS      3
#define LRED_NR_OF_MSTACKS      2
#define LRED_NR_OF_QSTACKS      3

/*********************************************************************************/
/********** PROGRAM MODULE MACROS ************************************************/
/*********************************************************************************/

#define CONCAT_3(a,b,c)		a##b##c
#define APPEND_EXTENDER(type)	CONCAT_3(SetupProps.FE.,type,_e_value)

#define ED_EXTENDER		APPEND_EXTENDER(ed)
#define PP_EXTENDER		APPEND_EXTENDER(pp)
#define RED_EXTENDER		APPEND_EXTENDER(red)
#define DOC_EXTENDER		APPEND_EXTENDER(doc)
#define ASC_EXTENDER		APPEND_EXTENDER(asc)
#define PRT_EXTENDER		APPEND_EXTENDER(prt)
#define PRE_EXTENDER		APPEND_EXTENDER(pre)

#define DISMISS_APPLY_FRAME		(1!=(SetupProps.PP.apply_set_value&1))
#define DISMISS_RESET_FRAME		(1!=(SetupProps.PP.reset_set_value&1))
#define DISMISS_DEFAULTS_FRAME		(1!=(SetupProps.PP.defaults_set_value&1))
#define ASKBEFORE_APPLY_FRAME		(2==(SetupProps.PP.apply_set_value&2))
#define ASKBEFORE_RESET_FRAME		(2==(SetupProps.PP.reset_set_value&2))
#define ASKBEFORE_DEFAULTS_FRAME	(2==(SetupProps.PP.defaults_set_value&2))


/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

extern SystemProperties SetupProps; /* Setup handle */

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

extern char *ExpandDir(char *dir);

/* Calc Stack and heap size functions **/
extern void CalcTotalStackSize(void);
extern void CalcTotalHeapSize(int mode);

/* Often use sizing functions **/
extern void justify_items(Panel panel, int resize);
extern void switch_category(int value,int show);

/* Select property function callback **/
extern void Property_category_proc(void);
extern void XProperty_category_proc(void);
extern void PProperty_category_proc(void);
extern void Set_Property_category(int value);

/* Apply/Reset functions for property setups **/
extern void props_apply_proc(Panel_item item,Event *event);
extern void props_reset_proc(Panel_item item,Event *event);
extern void props_defaults_proc(Panel_item item,Event *event);
extern void props_close_proc(Panel_item item,Event *event);

/* Notifier procedure for basenumber format property **/
extern void PNF_number_format_proc(Panel_item item, int value,Event *event);

/* Notifier procedure for number precison format property **/
extern void CP_truncUL_proc(Panel_item item, int value,Event *event);
extern void CP_multUL_proc(Panel_item item, int value,Event *event);
extern void CP_divUL_proc(Panel_item item, int value,Event *event);

/* Notifier procedure for stack size property **/
void SS_stack_proc(Panel_item item,int value,Event *event);

/* Notifier procedure for heap size property **/
extern void HS_nr_descr_proc(Panel_item item, int value,Event *event);
extern void HS_size_proc(Panel_item item, int value,Event *event);

/* Notifier procedure for fileextender setup property **/
extern void FE_menu_proc(Menu menu,Menu_item item);

#endif
