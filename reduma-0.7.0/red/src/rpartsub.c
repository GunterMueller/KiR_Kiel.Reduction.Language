/* $Log: rpartsub.c,v $
 * Revision 1.2  1992/12/16  12:50:28  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*-----------------------------------------------------------------------------
 *  part-sub:c  - external:  part_sub;
 *         Aufgerufen durch:  ea-retrieve;
 *  part-sub:c  - internal:  keine;
 *-----------------------------------------------------------------------------
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


/* ach 30/10/92 */
extern void stack_error();
extern STACKELEM inc_expr();                    /* rhinout.c */
/* end of ach */

#if DEBUG
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */       /* rstack.c */
#endif

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 */

/*-----------------------------------------------------------------------------
 *  part_sub --  fuellt in der Postprocessing-Phase den neuangelegten
 *               Inkarnationsblock , wobei nur Substitutionen wirklich
 *               durchgefuehrt werden, die mit SNAP gekennzeichnet sind.
 *               Die Variable rest ist die Anzahl der bleibenden Variablen,
 *               ueber die hinweg substituiert werden muss  ->  Korrektur
 *               der Nummernvariablen in den Argumenten. Dieses Modul wird
 *               auch dazu benutzt, geschlossene conditionals wieder zu
 *               oeffnen.
 *  globals  --
 *-----------------------------------------------------------------------------
 */
 int part_sub(narg)
    int narg;
 {
    STACKELEM x;
    int rest, i;
    int size = SIZEOFSTACK(S_hilf);
    int j;

    START_MODUL("part_sub");
    x = READSTACK(S_m);
    if (x == SET_ARITY(SNAP,1))    /* also liegt auf E eine 'Funktion' */
    {
       PPOPSTACK(S_m);   /* SNAP */
       rest = narg - ARITY(POPSTACK(S_m1)) + 1;   /* Anzahl uebrigbleibender */
    }                                             /* Argumente */
    else
    {
       rest = narg;       /* alle Argumente bleiben uebrig */
    }
    for (j = 0; j < size; j++)     /* rette elemente von S_hilf */
       PUSHSTACK(S_e,POPSTACK(S_hilf));

    for (i=narg - 1; i >=0; i--)   /* I.block fuellen */
       if (i < rest)
    {
       PUSH_I(SET_VALUE(NUM,i));   /* kein Argument vorhanden, also wird */
    }                              /* die Nummernvariable selbst eingetragen */
    else
    {
       PUSH_I(inc_expr(rest));     /* sonst Argument eintragen, wobei die in */
    }                              /* dem Argument enthaltenen Nummernvariablen */
    for (j = 0; j < size; j++)     /* restauriere elemente auf S_hilf */
       PUSHSTACK(S_hilf,POPSTACK(S_e));
    END_MODUL("part_sub");         /* ihre Nummer um die Anzahl der verbleibenden */
    return(rest);                  /* Variablen erhoeht bekommen muss */
 }                                 /* Rueckgabe der Anzahl der verbleibenden */
                                   /* Argumente */
