/* $Log: lpa-sub.c,v $
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:49:03  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*-----------------------------------------------------------------------------
 *  lpa-sub:c  - external:  lpa_sub;
 *         Aufgerufen durch:  ea-retrieve;
 *  lpa-sub:c  - internal:  keine;
 *-----------------------------------------------------------------------------
 */
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rstelem.h"
#include "rmeasure.h"

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 */

#if DEBUG
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

/*-----------------------------------------------------------------------------
 *  lpa_sub --  fuellt in der Postprocessing-Phase den neuangelegten
 *              Inkarnationsblock , wobei nur Substitutionen wirklich
 *              durchgefuehrt werden, die mit SNAP gekennzeichnet sind.
 *              Die Variable rest ist die Anzahl der bleibenden Variablen,
 *              ueber die hinweg substituiert werden muss  ->  Korrektur
 *              der Nummernvariablen in den Argumenten. Dieses Modul wird
 *              auch dazu benutzt, geschlossene conditionals wieder zu
 *              oeffnen.
 *  globals  --
 *-----------------------------------------------------------------------------
 */
 int lpa_sub(narg,snarity)
    int narg;
    int snarity;
 {
    STACKELEM x;
    PTR_DESCRIPTOR y;
    int args;
    int rest, i;

    START_MODUL("lpa_sub");

    rest = narg - snarity;
    for (i=narg - 1; i >=0; i--)   /* I.block fuellen */
       if (i < rest) {             /* rest == 0 falls expr */
                                   /* urspruenglich keine  */
                                   /* Funktion war         */
       PUSH_I(SET_VALUE(NUM,i)); /* kein Argument vorhanden,      */
                                 /* also wird die Nummernvariable */
    }                            /* selbst eingetragen            */
    else {
       y = (PTR_DESCRIPTOR) READSTACK(S_hilf);
       args = R_LREC_ARGS((*y),nargs) - 1;
       x = (STACKELEM) R_LREC_ARGS((*y),ptdv)[args - (i-rest)];

       if ((T_POINTER(x))
          &&  (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_EXPR))
       {                        /* aus inc-expr.c (jetzt rhinout.c) */
          int j;
          STACKELEM * p;
          p = (STACKELEM *) R_EXPR((* (PTR_DESCRIPTOR)x),pte);
          for (j = (int)p[0]; j>0; j--)
             if T_NUM(p[j])
                post_mortem("lpa_sub: freie Variable hinter Expressionpointer");
       }
       else
       if (T_NUM(x)) {
          x = SET_VALUE(x,VALUE(x) + rest);
       }
       /* else ;    auskommentiert RS 14.6.1993 */
       if (T_POINTER(x))
          INC_REFCNT((PTR_DESCRIPTOR)x);
       /* else ;    auskommentiert RS 14.6.1993 */
       PUSH_I(x);
    }
    END_MODUL("lpa_sub");         /* ihre Nummer um die Anzahl der verbleibenden */
    return(rest);                  /* Variablen erhoeht bekommen muss */
 }                                 /* Rueckgabe der Anzahl der verbleibenden */
                                   /* Argumente */
/* end of lpa-sub.c */
