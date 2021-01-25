/**********************************************************************************
***********************************************************************************
**
**   File        : list-modu.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 17.03.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions for list handling
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

/*********************************************************************************/
/********** INTERNAL PROGRAM INFORMATIONS FOR THIS MODULE ************************/
/*********************************************************************************/

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdlib.h>
#include "dbug.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct ListEndRememberTag { /* Remember end of all used lists ************/
 void *list;                                             /* Pointer to used list */
 void *last;                                          /* Last entry of this list */
 int  NR;                                     /* Number of elements in this list */
} ListEndRemember; /**************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define DEF_NR_LISTREMEMBER     8192
#define DEF_NR_LISTREMEMBER_ADD 1024

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static ListEndRemember *LER=NULL;     /* Dynamical array of end of list pointers */
static int LER_size=0;                               /* Actual size of LER array */
static int LER_end=0;                                  /* Next free index of LER */
static int LER_last=0;                           /* Index of last used LER entry */

/*********************************************************************************/
/********** GLOBAL VARIABLES IN MODULE *******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

void InitLER(void)
{ /* Allocates memory for List End Remember array ********************************/
 LER=(ListEndRemember *)malloc(sizeof(ListEndRemember)*DEF_NR_LISTREMEMBER);
 LER_end=0;                      /* Actual index of free entry in Remember array */
 LER_size=DEF_NR_LISTREMEMBER;                             /* Default array size */
 LER_last=0;                                           /* No last list available */
} /*******************************************************************************/

void DeleteLER(void)
{ /* Resets LER structures and deallocates memory ********************************/
 DBUG_PRINT("DeleteLER",("LER Size=%d LER End=%d",LER_size,LER_end));
 if (LER) free(LER);                          /* Release list end remember array */
 LER=NULL;                            /* Dynamical array of end of list pointers */
 LER_size=0;                                         /* Actual size of LER array */
 LER_end=0;                                            /* Next free index of LER */
 LER_last=0;                                     /* Index of last used LER entry */
} /*******************************************************************************/

void UpDateLER(void *list,void *last)
{ /* Adds one ListEnd Pointer to array *******************************************/
 DBUG_PRINT("UPDATE",("LER_size=%d list=%p last=%p",LER_size,list,last));
 if (LER[LER_last].list==list) {                              /* Use last list ? */
   LER[LER_last].NR++;                     /* Increase number of entries in list */
   LER[LER_last].last=last;                             /* UpDate last list used */
   return;
 } else {                                         /* It's not the last used list */
  int i=LER_end-1;                    /* Start at end of already defined entries */
  while (i>=0) {                                       /* End of array reached ? */
   if (LER[i].list==list) {                           /* Is it this list entry ? */
    LER[i].last=last;                          /* Yes, update end of listpointer */
    LER_last=i;                                  /* Set new last used list index */
    LER[LER_last].NR++;                    /* Increase number of entries in list */
    return;
   }
   i--;                                       /* Test next elements of array LER */
  }
  LER[LER_end].list=list;                                    /* Create new entry */
  LER[LER_end].last=last;                                  /* with actual values */
  LER_last=LER_end;                              /* Set new last used list index */
  LER[LER_last].NR=1;                                /* First entry in this list */
  LER_end++;                                     /* Increase index to free entry */
  if (LER_end==LER_size) {                            /* max. tablesize execed ? */
   LER_size+=DEF_NR_LISTREMEMBER_ADD;                       /* Expand array size */
   LER=(ListEndRemember*)realloc(LER,LER_size*sizeof(ListEndRemember));
   DBUG_PRINT("REALLOC LER",("New size=%d",LER_size));
  }
 }
} /*******************************************************************************/

void *GetEndOfList(void *list)
{ /* Search in LER array for list and returns pointer to last element of list ****/
 if ((LER_last>=0) && (LER[LER_last].list==list)) {           /* Use last list ? */
   return (LER[LER_last].last);                    /* Yes, give end of list back */
 } else {                                         /* It's not the last used list */
  int i=LER_end-1;                    /* Start at end of already defined entries */
  while (i>=0) {                                       /* End of array reached ? */
   if (LER[i].list==list) {                           /* Is it this list entry ? */
    LER_last=i;                                       /* Set new last used index */
    return (LER[i].last);                        /* Yes, give end of listpointer */
   }
   i--;                                       /* Test next elements of array LER */
  }
 }
 return (NULL);                        /* No entry found, should be impossible ! */
} /*******************************************************************************/

int GetDimOfList(void *list)
{ /* Search in LER array for list and returns number of element in list **********/
 if ((LER_last>=0) && (LER[LER_last].list==list)) {           /* Use last list ? */
   DBUG_PRINT("LAST",("GetDimOfList(%p)=%d",list,LER[LER_last].NR));
   return (LER[LER_last].NR);                      /* Yes, give end of list back */
 } else {                                         /* It's not the last used list */
  int i=LER_end-1;                    /* Start at end of already defined entries */
  while (i>=0) {                                       /* End of array reached ? */
   if (LER[i].list==list) {                           /* Is it this list entry ? */
    LER_last=i;                                       /* Set new last used index */
    DBUG_PRINT("SEARCH LEN",("i=%d GetDimOfList(%p)=%d",i,list,LER[i].NR));
    return (LER[i].NR);                                   /* Yes, give Dim(list) */
   }
   i--;                                       /* Test next elements of array LER */
  }
 }
 return (0);
} /*******************************************************************************/

void CancelLERList(void *list)
{ /* Search in LER array for list and sign list as canceled **********************/
 if ((LER_last>=0) && (LER[LER_last].list==list)) {           /* Use last list ? */
  LER[LER_last].list=NULL;
  LER_last=LER_end;
 } else {                                         /* It's not the last used list */
  int i=LER_end-1;                    /* Start at end of already defined entries */
  while (i>=0) {                                       /* End of array reached ? */
   if (LER[i].list==list) {                           /* Is it this list entry ? */
    LER_last=LER_end;                                 /* Set new last used index */
    LER[i].list=NULL;
   }
   i--;                                       /* Test next elements of array LER */
  }
 }
} /*******************************************************************************/
