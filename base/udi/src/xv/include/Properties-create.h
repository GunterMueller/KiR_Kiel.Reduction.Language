/**********************************************************************************
***********************************************************************************
**
**   File        : Properties-create.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 05.7.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for XReduma front end : System properties
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _XREDUMA_PROPERTIESCREATE_HEADER_
#define _XREDUMA_PROPERTIESCREATE_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define DEF_START_XREQUESTER    XPROPERTY_ADD

/* Programmconstants for : Fileextender requester **/
#define FILEEXTENDER_LEN		5
#define FILEEXTENDER_DISPLAYLEN		5
#define FE_PATHDISPLAY_LEN		24
#define FE_PATHSTORE_LEN		512
#define DEF_FILEPATH_WIDTH		200

/* Programmconstants for : Calculation precison requester **/
#define DEF_CP_TICKS		5
#define DEF_CP_SLIDERWIDTH	200
#define DEF_TRUNC_MIN		0
#define DEF_TRUNC_MAX		1000
#define DEF_MULT_MIN		0
#define DEF_MULT_MAX		1000
#define DEF_DIV_MIN		0
#define DEF_DIV_MAX		1000

/* Programmconstants for : Stacksize requester **/
#define DEF_SS_TICKS		11
#define DEF_SS_SLIDERWIDTH	300
#define DEF_QSTACK_MIN		200
#define DEF_QSTACK_MAX		2000000
#define DEF_ISTACK_MIN		200
#define DEF_ISTACK_MAX		2000000
#define DEF_MSTACK_MIN		200
#define DEF_MSTACK_MAX		2000000

/* Programmconstants for : Heapsize requester **/
#define DEF_HS_TICKS		11
#define DEF_HS_SLIDERWIDTH      200
#define DEF_NR_DESCR_MIN	1
#define DEF_NR_DESCR_MAX	10000
#define DEF_NR_DESCR_FACTOR	100
#define DEF_NR_SIZE_MIN		8
#define DEF_NR_SIZE_MAX		32000
#define DEF_NR_SIZE_FACTOR	1024

/* Programmconstants for : Number format requester **/
#define DEF_BASENUMBER_MIN	10
#define DEF_BASENUMBER_MAX	1000000000

/* Programmconstants for : Misc requester **/
#define OPTIONS_MISC		4
#define DEF_REDCNT_MIN		1
#define DEF_REDCNT_MAX		1000000000

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

extern int  REDUMA_PROPERTY_CREATE_DONE;
extern int  XREDUMA_PROPERTY_CREATE_DONE;
extern int  PREDUMA_PROPERTY_CREATE_DONE;

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Allocates and initialize system property variables **/
void InitPropertyValues(void);

/* Show properties dialog window for reduma setups **/
extern void ShowProperties(void);

/* Show properties dialog window for xreduma setups **/
extern void ShowXProperties(void);

/* Destroys all Xview object (Should only be called at programmend) **/
extern void pc_Delete_xv_Objects(void);

#endif
