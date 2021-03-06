/* $Log: rmeasure.h,v $
 * Revision 1.1  1992/11/04 18:12:30  base
 * Initial revision
 * */



/*********************************************************************/
/* In dieser Tabelle werden die Indizes der einzelnen Zaehler in     */
/* der Countertabelle des Messystems definiert.                      */
/*                                                                   */
/* >>>  Achtung: bei Updates der Zaehler- oder Routinen-Nummern  <<< */
/* >>>           unbedingt auch die Auswertungsprozeduren        <<< */
/* >>>           in breakpoint:c modifizieren.                   <<< */
/*                                                                   */
/* Fuer jedes Modul wird folgende Zaehlertabelle                     */
/* gefuehrt:                                                         */
/*                                                                   */
/*       C_Routinen Counter Tabelle                                  */
/*                                                                   */
/*                    0         1        ..   NO_T_CNT-1             */
/*      __________________________________________________           */
/*      I         I NEWDESC I REFCOUNT I  ..             I           */
/*      I---------I---------I----------I-----------------I           */
/*   0  I PREPROC I         I          I                 I           */
/*      I---------I---------I----------I-----------------I           */
/*   1  I POSTPRO I         I          I                 I           */
/*      I---------I---------I----------I-----------------I           */
/*   :  I RED_PLUSI         I          I                 I           */
/*      I---------I---------I----------I-----------------I           */
/*      I    :    I                                      I           */
/*      I    :    I                                      I           */
/*      I         I                                      I           */
/*      I_________I______________________________________I           */
/*                                                                   */
/*  Preprocessing und Postprocessing werden jeweils als ein Modul    */
/*  gefuehrt. Einzelne Routinen wie z.B. trav werden nicht gefuehrt, */
/*  sondern es wird unter der rufenden Routine gezaehlt (siehe       */
/*  breakpoint:c )                                                   */
/*********************************************************************/

/* last update  by     why

   copied       by     to

*/
#if MEASURE

#define MAXMESS 16


#define           MS_CALL               0
#define           MS_FAIL               1
#define           MS_POP                2
#define           MS_PUSH               3
#define           MS_READ               4
#define           MS_WRITE              5
#define           MS_IPOP               6
#define           MS_IPUSH              7
#define           MS_IREAD              8
#define           MS_IWRITE             9
#define           MS_NEWDESC           10
#define           MS_REFCNT            11
#define           MS_DESCREF           12
#define           MS_NEWHEAP           13
#define           MS_HEAPSEARCH        14
#define           LOAD_EXPR            15
#define           STORE_EXPR           16


#endif  /* MEASURE */

 /* end of rmeasure.h */
