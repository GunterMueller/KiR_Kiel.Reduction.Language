/* $Log: rsubfunc.c,v $
 * Revision 1.2  1992/12/16  12:51:16  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*-----------------------------------------------------------------------------
 *  subfunc.c  - external:  subfunc;
 *         Aufgerufen durch:  ea-create;
 *  subfunc.c  - internal:  keine;
 *-----------------------------------------------------------------------------
 */
/* last update  by     why
   25.05.88     ca     neues 'r1eapre': r1pr, ersetzt alle gebundenen
                       variablen auf einmal.
   01.05.88     sj     falschen Zugriff auf das Edit-Feld in void subfunc()
                       geaendert. Zugriff darf nicht faelschlicherweise ueber
                       STORE_EDIT erfolgen, sondern muss ueber das Marko L_FUNC
                       geschehen. Weiter ist die Anpassung an die rheapty:h
                       erfolgt, d.h. Ersetzung von "edit" durch special bei
                       einem Zugriff auf das Edit-Feld des Funktions-Deskriptors
                       SJ-ID
   02.06.88     sj     Makro EXTR_EDIT  eingefuegt  (defined in rstelem:h)
                       SJ-ID
   03.06.88     ca     umschaltung PRE_PROC entfernt, standardmaessig !

   copied       by     to

*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 */
extern int closefunc();                     /* closefunc.c */
extern void r1pr();                         /* r1pr.c      */ /* ca */
extern PTR_DESCRIPTOR newdesc();            /* heap.c      */

/* ach 29/10/92 */
extern int newheap();                       /* rheap.c */
extern void stack_error();                  /* rstack.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern void DescDump();
#endif
/* end of ach */

/*------ subfunc --------------------------------------------------------------
 * Steuert das Schliessen einer Funktion und die Umbenennung in Nummern-
 * variablen. Erst wird, wenn noetig, die Funktion geschlossen.
 * Dann wird ein Deskriptor angefordert.
 * Alle Variablen werden auf einmal umbenannt.
 *
 *  globals    --  bindings    <w,r>              reduct.c
 *-----------------------------------------------------------------------------
 */

 void subfunc(lrec)
 int lrec;                /* falls (lrec == 1) closefunc() nicht ausfuehren */
 {
    PTR_DESCRIPTOR pf;    /* Funktionsdeskriptor */
    STACKELEM *name;
    STACKELEM x/*,y*/;                      /* ach 29/10/92 */
    int nbound, nfree/*, i*/;               /* ach 29/10/92 */
    /* short l;                                ach 29/10/92 */
    extern int _bindings;          /* reduct.c */
    extern int _bound;             /* state.c */

    START_MODUL("subfunc");

    if ((pf = newdesc()) == NULL)           /* Funktionsdeskriptor anlegen */
      post_mortem("subfunc: Heap out of space");

    x = READSTACK(S_e);                     /* n-stelliges SUB             */
    nbound = ARITY(x) - 1;
    PUSHSTACK(S_m,SET_ARITY(PRELIST,nbound));        /* ca */

    if ((_bindings) && (!lrec))
       nfree = closefunc();
    else
       nfree = _bound;

    _bindings += nbound;                    /* ab sofort sind nbound weitere */
                                            /* Variablen "aktiv", d.h. hier  */
    DESC_MASK(pf,1,C_EXPRESSION,TY_SUB);    /* ist ihr Bindungsbereich       */
    L_FUNC((*pf),special) = EXTR_EDIT(x);   /* Edit-Feld SJ-ID   SJ-ID       */
    L_FUNC((*pf),nargs) = nbound + nfree;   /* Anzahl Argumente              */
#if DEBUG                          /* 1 in 0 geaendert wg. odd address       */
    L_FUNC((*pf),pte) = (PTR_HEAPELEM)0;    /* Damit DescDump() nicht bei    */
#endif /*  DEBUG */                         /* diesem halbfertigen Deskri-   */
                                            /* ptor abstuerzt */
    if (newheap(1 + nbound, A_FUNC((*pf),namelist)) == 0)
      post_mortem("subfunc: Heap out of space");

    name = (STACKELEM *) R_FUNC((*pf),namelist);     /* Fuer die Variablen- */
    name[0] = (STACKELEM) nbound;                    /* namen */
    r1pr(name);                             /* ca */
    PUSHSTACK(S_a,(STACKELEM)pf);           /* Der Deskriptor ist durch     */
    END_MODUL("subfunc");                   /* ein PRELIST an den Rumpf ge- */
 }                                          /* bunden.                      */
/* end of subfunc.c */
