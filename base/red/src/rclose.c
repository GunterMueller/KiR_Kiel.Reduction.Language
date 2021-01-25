/*
 * $Log: rclose.c,v $
 * Revision 1.3  2001/07/02 14:53:11  base
 * compiler warnings eliminated
 *
 * Revision 1.2  1992/12/16 12:49:24  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */



/*----------------------------------------------------------------------------
 *  closefunc:c - external: closefunc;
 *        Aufgerufen durch: subfunc,recfunc;
 *  closefunc:c - internal: keine;
 *----------------------------------------------------------------------------
 */

/* last update  by     why

   copied       by     to

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
extern void stack_error();                 /* rstack.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();
#endif
/* end of ach */
/*--------------------------------------------------------------------------
 *  closefunc -- Schliesst eine Funktion und gibt die Anzahl der ehemals
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

int closefunc()
{
  T_HEAPELEM  *tab,*speicher = (T_HEAPELEM *) 0; /* Initialisierung von RS 6.11.1992 */
  STACKELEM     x;
  STACKELEM     y;
  int           i;
  int   nfree = 0;
/*int      sp = 0;        ueberfluessig, ca          */
  extern  int _bindings;           /* reduct:c */

  START_MODUL("closefunc");

  if ( ((     tab = newbuff(_bindings)) == 0) ||     /* Tabellen anlegen */
       ((speicher = newbuff(_bindings)) == 0))
    post_mortem("closefunc: Heap out of Space");     /* nicht geklappt */
  for (i = 0; i <  _bindings; i++)
     tab[i] = NOVAL;                                 /* initialisieren */

main_e:
  x = READSTACK(S_e);

  if T_CON(x)                      /* Konstruktoren gleich auf M */
  {
     if (ARITY(x) > 0)             /* wenn Stelligkeit > 0 */
     {
       PPOPSTACK(S_e);
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
     }
     else
     {
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;
     }
  }

  if T_NUM(x)                               /* relativ freie Variable   */
  {                                         /* gefunden                 */
    if (tab[VALUE(x)] == NOVAL)             /* erstes Auftreten dieser  */
    {                                       /* Variablen                */
      speicher [nfree] = x;                 /* -> neue interne Variable */
      tab[VALUE(x)] = SET_VALUE(NUM,nfree); /* bereitstellen und ...    */
      nfree++;
    }
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,tab[VALUE(x)]);           /* ersetzen durch neue      */
                                            /* Nummernvariable          */
    goto main_m;
   }

  if T_ATOM(x)                              /* atom auf a-Stack  */
  {
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,x);
    goto main_m;
  }

  post_mortem("closefunc: No match on main_e");

main_m:
  x = READSTACK(S_m);

  if T_PRELIST(x)                           /* Rumpf fertig            */
  {
    PTRAV_A_E;                              /* Zuruecktraversieren         */
    if (nfree)                              /* freie Variable vorhanden?   */
    {                                       /* Erzeugen der zusaetzlichen  */
      for (i = 0; i < nfree; i++)           /* Argumente                   */
        PUSHSTACK(S_a,speicher[i]);
      PPOPSTACK(S_m);  /* PRELIST */        /* SNAP unter PRELIST schieben */
      y = POPSTACK(S_m); /* PRELIST *//* SNAP unter PRELIST schieben *//* CS */
      PUSHSTACK(S_m,(STACKELEM) SET_ARITY(SNAP,1));
      PUSHSTACK(S_m1,(STACKELEM) SET_ARITY(SNAP,nfree + 1));
      PUSHSTACK(S_m,y);                     /* PRELIST */              /* CS */
      PUSHSTACK(S_m,x);                     /* PRELIST */
    }
    freeheap(tab);                          /* Tabellen freigeben          */
    freeheap(speicher);
    END_MODUL("closefunc");
    return (nfree);
  }

  if T_CON(x)                               /* das uebliche */
  {
    if (ARITY(x) > 1)
    {
      WRITESTACK(S_m,DEC(x));
      goto main_e;
    }
    else
    {
      PPOPSTACK(S_m);
      PUSHSTACK(S_a,POPSTACK(S_m1));
      goto main_m;
    }
  }

  post_mortem("closefunc: No match on main_m");
  return(0);                               /* ach 29/10/92 */
}
/* end of        closefunc.c       */
