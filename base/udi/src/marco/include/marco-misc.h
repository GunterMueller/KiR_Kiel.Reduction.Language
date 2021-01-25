/**********************************************************************************
***********************************************************************************
**
**   File        : marco-misc.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 7.5.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for standart functions (misc fcts)
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _UDIMISC_HEADER_
#define _UDIMISC_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define I_READ  0
#define I_WRITE 1
#define I_FAIL  (-1)
#define NULLFILE (FILE *)0

#define INITFILE        "red.init"

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/


/* REDUMA reduce function : defined in ~base/red/src/rreduce.c **/
extern int reduce(STACKELEM *stack,int *top,PTR_UEBERGABE p_parm);

/* REDUMA prstel function : defined in ~base/red/src/rprint.c **/
extern int prstel(FILE * XFile,int Conv,int Flag);

/* Read or write of reduction-parameters **/
extern int misc_InitRedParms(char *initfile,int dir,PTR_UEBERGABE p_Parms);

/* Set default values to red parameter **/
extern void misc_SetDefRedParms(PTR_UEBERGABE p_Parms);

/* Call reduma **/
extern int misc_CallReduma(void);

/* Generate output **/
extern void misc_OutputResults(void);

/* Converts new STACKELEMS to old format **/
extern STACKELEM misc_conv2old(STACKELEM elem);

/* Converts old STACKELEMS to new format **/
extern STACKELEM misc_conv2new(STACKELEM elem);

/* Tests, if an infix operations exits for the code pf **/
extern int ExistsInfixPF(int pf);

#endif
