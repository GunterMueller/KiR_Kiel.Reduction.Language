/* $Log: rpartrec.c,v $
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:26  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*-----------------------------------------------------------------------------
 *  part-rec:c  - external:  part_rec;
 *         Aufgerufen durch:  ea-retrieve;
 *  part-rec:c  - internal:  keine;
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

/* RS 30/10/92 */
extern void stack_error();                  /* rstack.c */
/* END of RS 30/10/92 */

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 */
extern STACKELEM inc_expr();                /* inc-expr:c */

#if DEBUG
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
#endif

/*-----------------------------------------------------------------------------
 *  part_rec --  fuellt in der Postprocessing-Phase den neuangelegten
 *               Inkarnationsblock , wobei nur Substitutionen wirklich
 *               durchgefuehrt werden, die mit SNAP gekennzeichnet sind.
 *               Ferner wird entschieden, ob eine rekursive Funktion
 *               expandiert werden soll oder nicht. Dementsprechend wird
 *               der Rekursionspointer oder eine Nummernvariable in
 *               den Inkarnationsstack eingetragen.
 *               die Variable rest gibt die Zahl der uebrigbleibenden
 *               Nummernvariablen an, ueber die hinwegsubstituiert werden
 *               muss.
 *               echt markiert, ob fuer die Rekursion ein Pointer oder
 *               eine Nummernvariable eingesetzt wird.
 *  globals  --  S_e,S_a     <w>
 *-----------------------------------------------------------------------------
 */
 int part_rec(narg,pfunc)
    int narg;                      /* Zahl uebrigbleibender Argumente */
    PTR_DESCRIPTOR pfunc;          /* zur Funktion gehoeriger Deskriptor */
 {
    STACKELEM x, y;
    int rest, i, echt;
    int mod;
    int size = SIZEOFSTACK(S_hilf);
    int j;

    START_MODUL("part_rec");
    echt = (R_FUNC((*pfunc),namelist)[0]) - 1;       /* Anzahl der echten  */
    x = READSTACK(S_m);                              /* Argumente der ur-  */
    if (x == SET_ARITY(SNAP,1))                      /* spruenglichen      */
    {                                                /* Funktion */
       PPOPSTACK(S_m);   /* SNAP */
       rest = narg - ARITY(POPSTACK(S_m1)) + 1;
       if (rest == echt)
          y = SET_VALUE(NUM,rest);          /* alle urspruenglichen Argumente */
       else                                 /* bleiben erhalten */
       {
          y = (STACKELEM) pfunc;            /* sonst muss der (bzw. die) Aufruf */
          INC_REFCNT(pfunc);                /* der Funktion expandiert werden */
       }                                    /* Also muss der Aufruf durch  */
    }                                       /* einen Pointer ersetzt werden */
    else
    {
       rest = narg;                         /* da kein SNAP da war, muessen */
       y = SET_VALUE(NUM,narg);             /* alle urspruenglichen Argumente */
    }                                       /* erhalten bleiben */

    mod = (T_NUM(y) ? (rest + 1) : rest);   /* fuer inc_expr() */
    for (j = 0; j < size; j++)     /* rette elemente von S_hilf */
       PUSHSTACK(S_e,POPSTACK(S_hilf));
    for (i=narg; i >=0; i--)                /* Fuellen des I.blockes */
       if (i == echt)                       /* rek. Funktion selbst, also  */
          PUSH_I(y);                        /*  Pointer oder Nummernvar. */

       else if ((i < rest) && (i != echt))
       {
          PUSH_I(SET_VALUE(NUM,i));         /* Variable ohne Argument. Wird */
       }                                    /* durch sich selbst ersetzt */
       else if ((i >= rest) && (i != echt))
       {
          PUSH_I(inc_expr(mod));            /* bei echten Argumenten muessen */
       }                                    /* die Nummern von Nummernvar    */
       /*  else ; auskommentiert RS 14.6.1993 */                             /* angepasst werden */
    for (j = 0; j < size; j++)     /* rette elemente von S_hilf */
       PUSHSTACK(S_hilf,POPSTACK(S_e));
    END_MODUL("part_rec");
    if (rest != 0)
       return(rest);                        /* Zahl uebrigbleibender Var */
    else                                    /* einschliesslich Recname!!! */
       if T_NUM(y)
          return(-1);                       /* Funktion bleibt erhalten,hatte */
       else                                 /* aber nie Argumente */
          return(rest);
 }
