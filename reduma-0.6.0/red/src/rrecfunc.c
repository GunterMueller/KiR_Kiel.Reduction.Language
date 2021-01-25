/* $Log: rrecfunc.c,v $
 * Revision 1.2  1992/12/16  12:50:41  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*-----------------------------------------------------------------------------
 *  recfunc.c  - external:  recfunc;
 *         Aufgerufen durch:  ea-create;
 *  recfunc.c  - internal:  keine;
 *-----------------------------------------------------------------------------
 */

/* last update  by     why
   25.05.88     ca     neues 'r1eapre': rr1pr, ersetzt alle gebundenen
                       variablen auf einmal.
   01.06.88     sj     falschen Zugriff auf das Edit-Feld in void rec_func()
                       geaendert. Zugriff darf nicht faelschlicherweise ueber
                       STORE-EDIT erfolgen , sondern muss ueber das Makro L_FUNC
                       geschehen. Weiter ist die Anpassung an die rheapty:h
                       erfolgt. d.h. Ersetzung von "edit" durch special
                       bei einem Zugriff auf das Edit-Feld des
                       Funktions-Deskriptors.                   SJ-ID
   02.06.88     sj     Makro EXTR_EDIT  eingefuegt   (defined in rstelm:h)
                       SJ-ID
   03.06.88     ca     PRE_PROC standardmaessig

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
extern int closefunc();
extern void rr1pr();                        /* ca */
extern PTR_DESCRIPTOR newdesc();

/* ach 29/10/92 */
extern void stack_error ();                 /* rstack.c */
extern int newheap();                       /* rheap.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern void DescDump();
extern STACKELEM *ppopstack();
#endif
/* end of ach */

/*------ recfunc --------------------------------------------------------------
 * Steuert das Schliessen einer Funktion und die Umbenennung in Nummern-
 * variablen. Erst wird die Funktion geschlossen.
 * Alle Variablen werden auf einmal mittels rr1pr umbenannt und falls notwendig
 * werden auch kuenstliche Argumente an den rekursiven Aufruf angefuegt.
 *  globals    --  bindings    <w,r>
 *-----------------------------------------------------------------------------
 */

 void recfunc()
 {
    PTR_DESCRIPTOR pf;          /* Funktionsdeskriptor */
    STACKELEM *name;
    STACKELEM x,y,rec;
    int nbound, nfree/*, i*/;               /* ach 29/10/92 */
    /* short l;                                ach 29/10/92 */
    extern int _bindings;                   /* reduct.c */

    START_MODUL("recfunc");
    if ((pf = newdesc()) == NULL)
      post_mortem("recfunc: Heap out of space");
    PUSHSTACK(S_m,PRELIST);        /* der PRELIST bindet den Funktionsrumpf */
    /* an den Deskriptor, dies ist hier notwendig fuer closefunc (s.unten)  */
    if (_bindings)
      nfree = closefunc(); /* Funktion schliessen u. freie Variablen zaehlen */
    else
      nfree = 0;                       /* keine freien Variablen */
    rec = POPSTACK(S_e);               /* REC */
       y = POPSTACK(S_e);             /* recname */
    x = READSTACK(S_e);               /* n-stelliges SUB */
    if (T_SUB(x)) {                   /* Funktion hat Argumente */
       nbound = ARITY(x) - 1;
       PPOPSTACK(S_e);                /* SUB loeschen         */       /* ca */
     }
    else
       nbound = 0;
    WRITESTACK(S_m,SET_ARITY(PRELIST,nbound + 1));
        /* Anzahl der bindenden Vorkommen im PRELIST merken */
    _bindings += nbound + 1;       /* jetzt nbound+1 offene Vorkommen mehr */

    DESC_MASK(pf,1,C_EXPRESSION,TY_REC);
    L_FUNC((*pf),special) = EXTR_EDIT(rec);  /* Edit-Feld  SJ-ID  SJ-ID */
    L_FUNC((*pf),nargs) = nbound + nfree;   /* Anzahl Argumente der Funktion */
#if DEBUG                          /* 0 statt 1 wg. odd address error  */
    L_FUNC((*pf),pte) = (PTR_HEAPELEM)0;/* Damit DescDump() nicht abstuerzt */
#endif /* DEBUG */

    if (newheap(2 + nbound, (int *)A_FUNC((*pf),namelist)) == 0)
      post_mortem("recfunc: Heap out of space");
    name = (STACKELEM *) R_FUNC((*pf),namelist);     /* Namensliste */
    name[0] = (STACKELEM) nbound + 1;                /* Anzahl der Namen */
    name[nbound + 1] = y;                   /* Rekursionsname eintragen  */

    rr1pr(name,nbound,nfree);               /* ca */
    PUSHSTACK(S_a,(STACKELEM)pf);  /* Deskriptor auf Stack legen */
    END_MODUL("recfunc");
 }
/* end of recfunc.c */
