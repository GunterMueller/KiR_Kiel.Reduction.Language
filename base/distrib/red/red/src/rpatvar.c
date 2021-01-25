/*
 * $Log: rpatvar.c,v $
 * Revision 1.3  2001/07/02 14:53:33  base
 * compiler warnings eliminated
 *
 * Revision 1.2  1992/12/16 12:50:29  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */




#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "rinstr.h"



extern StackDesc S_pm;

/* RS 02/11/92 */ 
extern void stack_error();             /* rstack.c */
extern void trav();                    /* trav.c */ 
/* END of RS 02/11/92 */ 

#if DEBUG
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
#endif

/*-----------------------------------------------------------------------------
 *  pm_patvar -- traversiert ein Pattern vom E-stack auf den A-stack,
 *               und pusht die im pattern vorkommenden Variablen auf
 *               den pm-stack.
 *               Variable aus einem as ... X liegen dabei ueber den
 *               anderen Variablen des Pattern .
 *               Als Ergebnis liefert pm_patvar die Zahl der festgestellten
 *               Variablen
 *----------------------------------------------------------------------------
 */
int     pm_patvar ()
{
    int     nvar = 0;                                            /* zaehlt die
                                                                    Zahl der
                                                                    Variaben  */
    int     nskipvar = 0;                                        /* zaehlt die
                                                                    Zahl der
                                                                    Variaben  */
    int i;
 /* int     level = 0;                         RS 02/11/92 */ 
    register    STACKELEM x;
    register    STACKELEM hilf;


    START_MODUL ("pm_patvar");

    PUSHSTACK (S_m, KLAA);
    /*bei einem caserec muss zusaetzlich zu den Patternvariablen     */
    /*der Parameter f als Variable vor den Rumpf und Body geschrieben*/
    /*werden bei Aufruf von pm_patvar liegt auf dem M-Stack entweder */
    /*ein case oder ein caserec Konstruktor, Der Parameter f liegt im*/
    /*Falle eines caserec auf dem A-Stack                            */
     hilf = POPSTACK(S_m1);/* den Matchkonstruktor wegnehmen um     */
                           /* festzustellen ob darunter ein caserec liegt*/
     if (T_PM_REC(READSTACK(S_m1)))
     {
       PUSHSTACK(S_m1, hilf); /*Matchkonstr. zurueck*/
       x= READSTACK(S_a);/*Deskriptor fuer das f */      
       PUSHSTACK(S_pm, x);
       ++nvar;
       INC_REFCNT((PTR_DESCRIPTOR) x);
     }
     else
     {
       PUSHSTACK(S_m1, hilf); /*Matchkonstr. zurueck*/
     } 


trav_source:

    x = POPSTACK (S_e);

    if (T_CON (x))
    {
        if( (T_PM_AS(x))
            && ((T_PM_SKSKIP(READSTACK(S_e)))
                ||  (T_PM_DOLLAR(READSTACK(S_e))))    )
        {
             PUSHSTACK(S_m, x); /* das as */
             PUSHSTACK(S_a, POPSTACK(S_e)); /* das SKSKIP   */
                                                /* oder Dollar  */

    /* Kopieren der Variablen */
    x = POPSTACK (S_e);
            if (T_POINTER (x))
            {
                 if (R_DESC ((*(PTR_DESCRIPTOR) x), type) == TY_NAME)
                 {
                     PUSHSTACK (S_i, x);
                     PUSHSTACK (S_a, x);
                     ++nskipvar;
                     INC_REFCNT ((PTR_DESCRIPTOR) x);
                     PUSHSTACK(S_a, POPSTACK(S_m));
                     goto trav_m;
                 }
            }
    if (T_LET0 (x))
    {
        PUSHSTACK (S_a, x);
        PUSHSTACK (S_i, x);
        ++nskipvar;
        PUSHSTACK(S_a, POPSTACK(S_m));
        goto trav_m;
    }

    if (T_LET1 (x))
    {
        if (VALUE(x) == '%')            /* String-Variable erkannt */
        { 
            x = POPSTACK(S_e);
            if (T_LET0 (x))
            {
                PUSHSTACK (S_a, x);
                PUSHSTACK (S_i, x);
                ++nskipvar;
                PUSHSTACK(S_a, POPSTACK(S_m));
                goto trav_m;
            }
        }
        PUSHSTACK (S_a, TOGGLE (x));
        PUSHSTACK (S_i, TOGGLE (x));
        for (; x = POPSTACK (S_e), T_STR1 (x);)
        {
            PUSHSTACK (S_a, x);
            PUSHSTACK (S_i, x);
        }
        PUSHSTACK (S_a, TOGGLE (x));
        PUSHSTACK (S_i, TOGGLE (x));
        ++nskipvar;
        PUSHSTACK(S_a, POPSTACK(S_m));
        goto trav_m;
    }
    /* Pattern hat die Form as ... . */
    if (T_PM_SKIP(x))
    {
        PUSHSTACK(S_a, x);
        
        /* as vom m-stack auf den a-stack*/
        hilf = POPSTACK(S_m);
        PUSHSTACK(S_a, hilf);

        goto trav_m;
    }
    }
        PUSHSTACK (S_m, x);
        PUSHSTACK (S_m1, x);
        goto trav_source;
    }

    if (T_LET0 (x))
    {
        PUSHSTACK (S_a, x);
        PUSHSTACK (S_pm, x);
        ++nvar;
        goto trav_m;
    }

    if (T_LET1 (x))
    {
        if (VALUE(x) == '%')            /* String-Variable erkannt */
        {
            x = POPSTACK(S_e);
            if (T_LET0 (x))
            {
                PUSHSTACK (S_a, x);
                PUSHSTACK (S_i, x);
                ++nskipvar;
                goto trav_m;
            }
            PUSHSTACK (S_a, TOGGLE(x));
            PUSHSTACK (S_i, TOGGLE(x));
            for (; x = POPSTACK (S_e), T_STR1 (x);)
            {
                PUSHSTACK (S_a, x);
                PUSHSTACK (S_i, x);
            }
            PUSHSTACK (S_a, TOGGLE (x));
            PUSHSTACK (S_i, TOGGLE (x));
            ++nskipvar;
        }
        else
        {
            PUSHSTACK (S_a, TOGGLE (x));
            PUSHSTACK (S_pm, TOGGLE (x));
            for (; x = POPSTACK (S_e), T_STR1 (x);)
            {
                PUSHSTACK (S_a, x);
                PUSHSTACK (S_pm, x);
            }
            PUSHSTACK (S_a, TOGGLE (x));
            PUSHSTACK (S_pm, TOGGLE (x));
            ++nvar;
        }

        goto trav_m;
    }
    if (T_DIG0 (x))
    {
        PUSHSTACK (S_a, x);
        goto trav_m;
    }

    if (T_DIG1 (x))
    {
        PUSHSTACK (S_a, TOGGLE (x));
        for (; x = POPSTACK (S_e), T_DIG1 (x);)
            PUSHSTACK (S_a, x);
        PUSHSTACK (S_a, TOGGLE (x));
        goto trav_m;
    }
    if (T_ATOM (x)) {                /*  ein Atom (ausser letter und digit)  */
                                     /*  also z.B. character,TRUE,FALSE ...  */
      if (T_POINTER (x)) {           /*  hier muss Variable auf Hilfstack    */
                                     /*  gelegt werden;                      */
        if (R_DESC ((*(PTR_DESCRIPTOR) x), type) == TY_NAME) {
           PUSHSTACK (S_pm, x);
           ++nvar;
           INC_REFCNT ((PTR_DESCRIPTOR) x);
        }
      }
      PUSHSTACK (S_a, x);
      goto trav_m;
    }  /** Ende von T_ATOM **/

    post_mortem ("pm_patvar: No match successfull on trav_source");

trav_m:

    x = READSTACK (S_m);

    if (T_CON (x))
    {
        if (ARITY (x) > 1)
        {
            WRITESTACK (S_m, DEC (x));
            goto trav_source;
        }
        else
        {
            PPOPSTACK (S_m); /* TB, 2.11.1992 */
            PUSHSTACK (S_a, POPSTACK (S_m1));
            goto trav_m;
        }
    }

    if (T_KLAA (x))
    {
        PPOPSTACK (S_m); /* TB, 2.11.1992 */
        /* doppelt traversieren, sonst wird aus der Variablen*/
        /* A B'   die Variable B A'                          */
        for (i = nskipvar - 1 ; i >= 0; i--)
             {
             trav(&S_i, &S_e);
             }
        for (i = nskipvar - 1 ; i >= 0; i--)
             {
             trav(&S_e, &S_pm);
             }
        END_MODUL ("pm_patvar");
        return (nvar + nskipvar);
    }

    post_mortem ("pm_patvar: No match successfull on trav_m");
    return (0);                  /* RS 02/11/92 */ 
}
