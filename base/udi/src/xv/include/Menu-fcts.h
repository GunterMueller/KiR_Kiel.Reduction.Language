/**********************************************************************************
***********************************************************************************
**
**   File        : Menu-fcts.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 28.6.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : menu buttons
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_MENUFCTS_HEADER_
#define _XREDUMA_MENUFCTS_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <xview/xview.h>
#include <xview/panel.h>

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define MM_DISMISS		99 /* Generell ID for pinup frame dismiss */

/* Identifier to each user system menu **/
#define NR_MENUES		8 /* Nr of menus used */
#define MM_FILE			0
#define MM_REDUCE		1
#define MM_OPTION		2
#define MM_BUFFER		3
#define MM_HELP			4
#define MM_EDIT			5
#define MM_EXTRA		6
#define MM_FIND			7

/* Identifier for menu : REDUCE **/
#define MM_REDUCE_UNDO          1
#define MM_REDUCE_1             2
#define MM_REDUCE_CNT           3
#define MM_REDUCE_REDUCEALL     4
#define MM_GO_HOME		5
#define MM_REDUCE_SETCOUNTER	6

/* Identifier for menu : EDIT **/
#define MM_EDIT_UNDO		1
#define MM_EDIT_COPY		2
#define MM_EDIT_PASTE		3
#define MM_EDIT_CUT		4
#define MM_EDIT_TEXTEDITOR	5

/* Identifier for menu : FILE **/
#define MM_FILE_CLEAR           1
#define MM_FILE_LOAD            3
#define MM_FILE_INCLUDE         4
#define MM_FILE_SAVE            5
#define MM_FILE_SAVEAS          6
#define MM_FILE_SAVESETUPS      7
#define MM_FILE_EXIT            8
#define MM_FILE_ABOUT		9
#define MM_FILE_SAVEEXPR	10
#define MM_FILE_LOADSETUPS      11

/* Identifier for menu : OPTIONS **/
#define MM_OPTION_REDUMA        1       
#define MM_OPTION_FRONTEND      2
#define MM_OPTION_PROPS         3

/* Identifier for menu : EXTRAS **/
#define MM_EXTRA_TOOLBOX		1
#define MM_EXTRA_SYNTAXMODE		2
#define MM_EXTRA_LINEMODE		3
#define MM_EXTRA_EXPANDDOLLARVAR	4
#define MM_EXTRA_SAVEASDOLLARVAR	5

/* Identifier for menu : FIND **/
#define MM_FIND_SEARCH		1
#define MM_FIND_SR		2
#define MM_FIND_NEXT		3

/* Identifier for menu : Help **/
#define MM_HELP_1               1
#define MM_HELP_2               2
#define MM_HELP_3               3
#define MM_HELP_4               4
#define MM_HELP_5               5
#define MM_HELP_6               6
#define MM_HELP_7               7
#define MM_HELP_8               8
#define MM_HELP_9               9

#define MM_HELP_1_TEXT  "Syntax editor"
#define MM_HELP_2_TEXT  "Lineediting"
#define MM_HELP_3_TEXT  "Syntaxeditor keys"
#define MM_HELP_4_TEXT  "Syntax editor commands"
#define MM_HELP_5_TEXT  "System parameter"
#define MM_HELP_6_TEXT  "Editor files"
#define MM_HELP_7_TEXT  "KiR Syntax"
#define MM_HELP_8_TEXT  "Reduma"
#define MM_HELP_9_TEXT  "Index"

/* Identifier for menu : BUFFERS **/
#define MM_BUFFER_WRBB          1
#define MM_BUFFER_WRB1          2
#define MM_BUFFER_WRB2          3
#define MM_BUFFER_REBB          4
#define MM_BUFFER_REB1          5
#define MM_BUFFER_REB2          6

#define BUFFER_READ_BACKUP	1
#define BUFFER_READ_1		2
#define BUFFER_READ_2		3
#define BUFFER_WRITE_BACKUP	4
#define BUFFER_WRITE_1		5
#define BUFFER_WRITE_2		6

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Calculates size of one panel item **/
extern int LabelLength(Panel_item item);

/* Calculates the longest panelitem label **/
extern int LongestLabel(Panel panel);

extern void ThisFunctionIsNotAvailable(char *function,char *mode,char *whynot);

/* Function definitions for menu : FILE **/
extern int mf_File_Clear(int dontask);
extern void mf_File_Load(void);
extern void mf_File_Include(void);
extern void mf_File_Save(void);
extern void mf_File_Saveas(void);
extern void mf_File_Saveexpr(void);
extern void mf_File_SaveSetups(void);
extern void mf_File_LoadSetups(void);
extern void mf_File_Exit(void);

/* Function definitions for menu : REDUCE **/
extern void mf_Reduce(int function);
extern void mf_Reduce_Navigator(void);

/* Function definitions for menu : HELP **/
extern void mf_Help_SyntaxEditor(int help_number);

/* Function definitions for menu : BUFFER **/
extern void mf_Buffer_fcts(int number);

#endif
