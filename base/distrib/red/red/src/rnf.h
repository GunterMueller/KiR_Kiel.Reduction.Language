/* $Log: rnf.h,v $
 * Revision 1.1  1992/11/04 18:12:30  base
 * Initial revision
 * */
/*-----------------------------------------------------------------------*/
/*                                                                       */
/* ak                                                   25.02.1992       */
/*                                                                       */
/* Exportierte Funktionen aus der Datei rnf.c                            */
/* zur Reduktion zur Normalform eines Ausdrucks                          */
/*                                                                       */
/* reduce_to_nf ()                                                       */
/*                                                                       */
/* Aufgerufen in                                                         */
/*                  rreduct.c                                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/

#define NF_MODUL /* Flag um mehrfaches Einbinden zu verhinden */

extern void reduce_to_nf();

