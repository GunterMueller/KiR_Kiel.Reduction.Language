/* $Log: rwhen-pre.c,v $
 * Revision 1.2  1992/12/16  12:51:28  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/****************************************************************************/
/*   in diesem File : pmprepa                                           */
/*                    pm_put                                                */
/*                    pm_trav                                               */
/****************************************************************************/

/*----------------------------------------------------------*/
/*  include files                                           */
/*                                                          */
/*----------------------------------------------------------*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>


extern int  pm_patvar ();	       /* patpre:c */

extern StackDesc S_pm;

/*  RS 30/10/92 */
extern void stack_error();              /* rstack.c */
extern void trav();                     /* rtrav.c */
/* END of RS 30/10/92 */ 

#if DEBUG
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
#endif

/*-------------------------------------------------------------------------------
 * pm_put  -- kopiert n Variablen vom Hilfstack auf den E-stack; fuer das
 *            Kopieren wird der A-stack benutzt !
 *--------------------------------------------------------------------------
 */

void pm_put (n)
int     n;

{
    register    STACKELEM x;
    int     i;


    START_MODUL ("pm_put");

    for (i = 1; n >= i; i++)
    {
	x = POPSTACK (S_hilf);

	if (T_POINTER (x))
	{
	    PUSHSTACK (S_e, x);
	    PUSHSTACK (S_a, x);
	    INC_REFCNT ((PTR_DESCRIPTOR) x);

	}

	else
	    if (T_LET0 (x))
	    {
		PUSHSTACK (S_e, x);
		PUSHSTACK (S_a, x);
	    }

	    else
		if (T_LET1 (x))
		{
		    PUSHSTACK (S_e, TOGGLE (x));
		    PUSHSTACK (S_a, TOGGLE (x));

		    for (; x = POPSTACK (S_hilf), T_LET1 (x);)
		    {
			PUSHSTACK (S_e, x);
			PUSHSTACK (S_a, x);
		    }

		    PUSHSTACK (S_e, TOGGLE (x));
		    PUSHSTACK (S_a, TOGGLE (x));
		}
    }				/* ende for */

    for (i = 1; n >= i; i++)	/* jetzt die Variablen wieder zurueck auf 
				*/
    {				/* S_hilf                                 
				*/
	x = POPSTACK (S_a);

	if (T_POINTER (x))
	    PUSHSTACK (S_hilf, x);

	else
	    if (T_LET0 (x))
		PUSHSTACK (S_hilf, x);

	    else
		if (T_LET1 (x))
		{
		    PUSHSTACK (S_hilf, TOGGLE (x));

		    for (; x = POPSTACK (S_a), T_STR1 (x);)
			PUSHSTACK (S_hilf, x);

		    PUSHSTACK (S_hilf, TOGGLE (x));
		}

    }				/* ende for */

    END_MODUL ("pm_put");
    return;
}
/*-------------------------*------------------------------------------------------
 * pm_trav_spec  -- legt n Variablen vom PM-Stack auf den Hilfstack
 *--------------------------------------------------------------------------
 */

void pm_trav_spec (n)
int     n;

{

    register    STACKELEM x;
    int     i;

    START_MODUL ("pm_trav_spec");

    for (i = 1; n >= i; i++)
    {


	x = POPSTACK (S_pm);

	if (T_POINTER (x))
	{
	    PUSHSTACK (S_hilf, x);
	}
	else

	    if (T_LET0 (x))
	    {
		PUSHSTACK (S_hilf, x);
	    }
	    else
		if (T_LET1 (x))
		{
		    PUSHSTACK (S_a, TOGGLE (x));
		    x = POPSTACK (S_pm);
		    for (; T_LET1 (x);)
		    {
			PUSHSTACK (S_a ,x);
			x = POPSTACK (S_pm);
		    }
		    PUSHSTACK (S_a, TOGGLE (x));


		    x = POPSTACK (S_a);
		    PUSHSTACK (S_hilf, TOGGLE (x));
		    x = POPSTACK (S_a);
		    for (; T_LET1 (x);)
		    {
			PUSHSTACK (S_hilf, x);
			x = POPSTACK (S_a);
		    }
		    PUSHSTACK (S_hilf, TOGGLE (x));
		}
    }				/* ende for */


    END_MODUL ("pm_trav_spec");
    return;
}

/*-------------------------------------------------------------------------------
 * pm_trav  -- kopiert n Variablen vom Hilfstack auf den E-stack; 
 *--------------------------------------------------------------------------
 */

void pm_trav (n)
int     n;

{
    register    STACKELEM x;
    int     i;


    START_MODUL ("pm_trav");

    for (i = 1; n >= i; i++)
    {
	x = POPSTACK (S_hilf);

	if (T_POINTER (x))
	{
	    PUSHSTACK (S_e, x);

	}

	else
	    if (T_LET0 (x))
	    {
		PUSHSTACK (S_e, x);
	    }

	    else
		if (T_LET1 (x))
		{
		    PUSHSTACK (S_e, TOGGLE (x));

		    for (; x = POPSTACK (S_hilf), T_LET1 (x);)
		    {
			PUSHSTACK (S_e, x);
		    }

		    PUSHSTACK (S_e, TOGGLE (x));
		}
    }				/* ende for */


    END_MODUL ("pm_trav");
    return;
}
/***************************************************************************/
/*   pmprepa schreibt vor das Pattern, den Guard und den body  die            */
/*   Variabalen, die mit Hilfe von pm_patvar als durch das Pattern gebundene  */
/*   Variablen erkannt werden. Diese Variablen werden durch ein  Sub mit      */
/*   dem Pattern bzw. dem Guard und dem Body zusammengehalten                 */
/******************************************************************************/

void pmprepa ()
{
    int   /*  i,*/         /*  RS 30/10/92 */
            j;


    START_MODUL ("pmprepa");

    PUSHSTACK (S_m, SET_ARITY (PM_MATCH, 3));
    PUSHSTACK (S_m1, SET_ARITY (PM_MATCH, 3));



    j = pm_patvar ();		/* pm_patvar traversiert pattern auf A-stack */
                                /* legt die Variablen auf den Pm-Stack       */
                                /* pm_patvar liefert als Ergebnis die Zahl   */
                                /* der gefundenen Patternvariablen           */

   /* legt j Variablen vom Pm-Stack auf den Hilfsstack. Dadurch wird die  */
   /* Reihenfolge der Variablen umgedreht                                 */
    pm_trav_spec(j);

    trav (&S_e, &S_a);		/* guard auf A-stack             */

   /* j Variablen vom Hilfstack auf den E-Stack*/
    pm_put (j);

   /* das entspechend stellige  sub vor den body legen */
    if (j >= 1)
	PUSHSTACK (S_e, SET_ARITY (SUB, j + 1));

    trav (&S_a, &S_e);		/* den guard auf den E_stack */

    if (!(T_SA_TRUE (READSTACK (S_e))))
    {
       /* j Variablen vom Hilfstack auf den E-Stack*/
	pm_put (j);

       /* das entspechend stellige  sub vor den body legen */
	if (j >= 1)
	    PUSHSTACK (S_e, SET_ARITY (SUB, j + 1));
    }

    trav (&S_a, &S_e);		/* pattern zuruecktraversieren */

    pm_trav (j);

   /* das entspechend stellige  sub vor den body legen */
    if (j >= 1)
	PUSHSTACK (S_e, SET_ARITY (SUB, j + 1));

    END_MODUL ("pmprepa");
}
