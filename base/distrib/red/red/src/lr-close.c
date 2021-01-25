/*
 * $Log: lr-close.c,v $
 * Revision 1.3  2001/07/02 14:53:11  base
 * compiler warnings eliminated
 *
 * Revision 1.2  1992/12/16 12:49:05  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */


/*----------------------------------------------------------------------------
 *  lr_close:c - external: lr_close;
 *        Aufgerufen durch: subfunc,recfunc;
 *  lr_close:c - internal: keine;
 *----------------------------------------------------------------------------
 */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

/*----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *---------------------------------------------------------------------------
 */
extern T_HEAPELEM * newbuff();              /* heap:c */
extern void freeheap();                     /* heap:c */
extern void ptrav_a_e();                    /* trav:c */


/* ach 29/10/92 */
extern void stack_error();                  /* rstack.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();
#endif
/* end of ach */

/*--------------------------------------------------------------------------
 *  lr_close -- Schliesst eine Funktion und gibt die Anzahl der ehemals
 *               freien Variablen zurueck;
 *               Dabei wird eine Tabelle tab angelegt, die fuer genau jede
 *               aktive Variable (d.h. die Funktion befindet sich in ihrem
 *               Bindungsbereich) eine Stelle hat. Trifft man nun auf eine
 *               Nummernvariable, so wird diese durch den entsprechenden
 *               Eintrag in tab ersetzt. Ist kein Eintrag vorhanden, so wird
 *               die naechste freie Nummer vergeben (nfree) und in die Tabelle
 *               eingetragen. Die alte Nummernvariable wird in speicher ein-
 *               getragen, um daraus die (zusaetzlichen) Argumente fuer diese
 *               Funktion bilden zu koennen.
 *  globals      _bindings <r>
 *---------------------------------------------------------------------------
 */

T_HEAPELEM * lr_close(speicher)
  T_HEAPELEM  *speicher;
{
  T_HEAPELEM  *tab;
  STACKELEM     x/*,y*/;           /* ach 29/10/92 */
  int           i;
  int   nfree = 0;
  extern  int _bindings;           /* state.c */

  START_MODUL("lr_close");

  if ( !(tab = newbuff(_bindings)))     /* Tabelle anlegen */
     post_mortem("lr_close: Heap out of Space");     /* nicht geklappt */
  for (i = 0; i <  _bindings; i++)
     tab[i] = NOVAL;                                 /* initialisieren */

  PUSHSTACK(S_m,KLAA);    /* @ */
main_e:
  x = READSTACK(S_e);

  if T_CON(x)                      /* Konstruktoren gleich auf M */
  {
     if (ARITY(x) > 0) {           /* wenn Stelligkeit > 0 */
       PPOPSTACK(S_e);
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
     }
     else {
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;
     }
  }

  if T_NUM(x)                               /* relativ freie Variable   */
  {                                         /* gefunden                 */
    if (tab[VALUE(x)] == NOVAL)             /* erstes Auftreten dieser  */
    {                                       /* Variablen                */
      tab[VALUE(x)] = SET_VALUE(NUM,nfree); /* -> neue interne Variable */
      nfree++;                              /* bereitstellen und ...    */
      speicher [nfree] = x;
    }
    PPOPSTACK(S_e);                         /* ersetzen durch neue      */
    PUSHSTACK(S_a,tab[VALUE(x)]);           /* Nummernvariable          */
    goto main_m;
   }

  if T_ATOM(x) {                            /* atom auf a-Stack  */
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,x);
    goto main_m;
  }

  post_mortem("lr_close: No match on main_e");

main_m:
  x = READSTACK(S_m);

  if T_KLAA(x) {                         /* liste der func_def's fertig */
    PPOPSTACK(S_m);       /* @ */
    PTRAV_A_E;                              /* zuruecktraversieren */
    freeheap(tab);                          /* tab freigeben */
    speicher[0] = nfree;
    END_MODUL("lr_close");
    return (speicher);
  }
  else
    if T_CON(x) {
      if (ARITY(x) > 1) {
        WRITESTACK(S_m,DEC(x));
        goto main_e;
      }
      else {
        PPOPSTACK(S_m);
        PUSHSTACK(S_a,POPSTACK(S_m1));
        goto main_m;
      }
    }

  post_mortem("lr_close: No match on main_m");
  return ((T_HEAPELEM *) NULL);            /* ach 29/10/92 */
}
/* end of        lr_close.c       */



