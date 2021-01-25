/* $Log: rr1eapos.c,v $
 * Revision 1.2  1992/12/16  12:50:38  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*-----------------------------------------------------------------------------
 *  MODUL DER PROCESSING PHASE
 *  r1eapos:c  - external:  r1eapos;
 *         Aufgerufen durch:  posfunc;
 *  r1eapos:c  - internal:  keine;
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
extern void ret_expr();
/*end of ach */

#if DEBUG
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
#endif

/*-----------------------------------------------------------------------------
 *  r1eapos    --  substituiert fuer die Nummernvariable 0 einen Variablennamen.
 *                 Namenskonflikte werden dabei beachtet. Auf dem M-Stack liegt
 *                 ein sub-Konstruktor, dessen Stelligkeit groesser als 1 sein
 *                 kann, dh. auf dem E-Stack liegen noch eine Reihe von Namens-
 *                 deskriptoren.
 *  globals    --  S_e,S_a     <w>
 *                 S_m         <r>
 *-----------------------------------------------------------------------------
 */

 void r1eapos(searched,sub)
    STACKELEM searched;   /* zu ersetzende Variable, zur Zeit nur NUM(0) */
    STACKELEM sub;        /* einzusetzender Variablenname, auf jeden Fall */
                          /* ein Namensdeskriptor */
 {
    STACKELEM x;
    int b = 0;
    int bl = 0;
    int n = 0;
    int i;

    START_MODUL("r1eapos");
    /* Behandlung der bereits restaurieren Paramter des Sub-Konstruktors auf */
    /* dem M-Stack,dh. hier wird beruecksichtigt, ob ein Name in einer       */
    /* Funktion doppelt auftritt */
    for (i = READ_ARITY(S_m); --i > 0; )
    {
      if ((x = POPSTACK(S_e)) == sub) bl++;     /* gleicher Name */
      PUSHSTACK(S_a,x);                         /* Name auf A-Stack */
    }
    WRITE_ARITY(S_m,bl);                    /* Anzahl gleicher Namen notieren */
    b += bl;                                /* (im SUB) */
    bl = 0;

main_e:
    x = READSTACK(S_e);

    if (x == searched)    /* eine zu ersetzende Variable gefunden */
    {
       PPOPSTACK(S_e); /* TB, 30.10.92 */
       PUSHSTACK(S_a,sub);
       INC_REFCNT((PTR_DESCRIPTOR)sub);    /* Variablenname ist Deskriptor */
       for (i = b; i > 0; i--)
          PUSHSTACK(S_a,SET_ARITY(PROTECT,1));      /* Schutzstriche aufbauen */
       goto main_m;
    }

    if (T_NUM(x))
    {                                       /* eine andere Nummernvariable */
       PUSHSTACK(S_a,DEC(POPSTACK(S_e)));   /* eine Nummer kleiner */
       goto main_m;
    }

    if (T_SUB(x))                           /* innere Funktion. Diese ist bereits */
    {                                       /* umbenannt. Also muss die An- */
       PUSHSTACK(S_m1,POPSTACK(S_e));       /* Variabler gleichen Namens fest- */
       for (i = ARITY(x); i  > 1; i--)      /* gestellt werden, um spaeter die */
       {                                    /* richtige Anzahl von PROTECT's */
          if ((x = POPSTACK(S_e)) == sub) bl++;      /* schreiben zu koennen */
          PUSHSTACK(S_a,x);
       }
       PUSHSTACK(S_m,SET_ARITY(SUB,bl));    /* Anzahl gleicher Var. im SUB */
       b += bl;                             /* nachhalten */
       bl = 0;
       goto main_e;
    }
    if (T_REC(x))
    {                                        /* innere Rekursion. Diese ist bereits */
       PUSHSTACK(S_m1,POPSTACK(S_e));        /* umbenannt. Also muss die An- */
       if ((x = POPSTACK(S_e)) == sub) bl++; /* Variabler gleichen Namens fest- */
       PUSHSTACK(S_a,x);                     /* gestellt werden, um spaeter die */
       PUSHSTACK(S_m,SET_ARITY(REC,bl));     /* richtige Anzahl von PROTECT's */
       b += bl;                              /* schreiben zu koennen. Hier  */
       bl = 0;                               /* wird nur der Rekursionsname */
       goto main_e;                          /* behandelt. Die Argumente haben */
    }                                        /* ein SUB, also geht es dort */
                                             /* weiter */
#if DORSY
    if (T_LREC(x))
    {
       int i,funcs;
       STACKELEM y,z;

       PUSHSTACK(S_m,DEC(POPSTACK(S_e)));
                          /* LREC(2) wg. list of func_names (s.u.) */
       y = POPSTACK(S_e);          /* LIST(funcs) */
       funcs = ARITY(y);

       for (i = 0; i < funcs; i++) {
          z = POPSTACK(S_e);
          if (z == sub) bl++;
          PUSHSTACK(S_a,z);
       }
       PUSHSTACK(S_a,y);           /* LIST */

       PUSHSTACK(S_m1,SET_ARITY(LREC,bl));
       b += bl;
       bl = 0;
       goto main_e;
    }
#else
    if (T_LREC(x))
    {
       int i,funcs;
       PTR_DESCRIPTOR plr;
       STACKELEM *name;

       PUSHSTACK(S_m,POPSTACK(S_e));

       plr = (PTR_DESCRIPTOR) READSTACK(S_e);
       name = (STACKELEM *) R_LREC((*plr),namelist);
       funcs = (int) name[0];

       for (i = 1; i <= funcs; i++)
           if (name[i] == sub) bl++;

       PUSHSTACK(S_m1,SET_ARITY(LREC,bl));
       b += bl;
       bl = 0;
       goto main_e;
    }
#endif
    if (T_PROTECT(x))
    {                                        /* Schutzstriche einsammeln */
       PPOPSTACK(S_e); /* TB, 30.10.92 */
       n++;
       goto main_e;
    }

    if (T_POINTER(x) &&
       (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME))
    {                                       /* Variablenname */
       if ((n >= b) && (x == sub)) n++;     /* evtl. ein Schutzstrich dazu */
       PUSHSTACK(S_a,x);                    /* Name auf A */
       PPOPSTACK(S_e);
       for (; n > 0; n--)
          PUSHSTACK(S_a,SET_ARITY(PROTECT,1));       /* Schutzstriche wieder */
                                                     /* dazupacken */
       goto main_m;
    }

    if (T_CON(x))                              /* Konstruktor traversieren */
    {
       if (ARITY(x) > 0)
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

    if (T_POINTER(x) && (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_EXPR))
    {
       PPOPSTACK(S_e);
       ret_expr(x);
       goto main_e;
    }

    if (T_ATOM(x))                 /*    sonstiger atom traversieren    */
    {
       PUSHSTACK(S_a,x);
       PPOPSTACK(S_e);
       goto main_m;
    }

    post_mortem ("r1eapos main_e: no match");


main_m:
    x = READSTACK(S_m);

    if (T_PRELIST(x))              /* Funktion zuende */
    {
       END_MODUL("r1eapos");
       return;
    }

    if (T_LREC(x))
    {
       STACKELEM z;

       if (ARITY(x) > 1 )
       {
          WRITESTACK(S_m,DEC(x));
          goto main_e;
       }
       z = POPSTACK(S_m1);
       b -= ARITY(z);
       PPOPSTACK(S_m);
       PUSHSTACK(S_a,SET_ARITY(z,3));
       goto main_m;
    }

    if (T_SUB(x) || T_REC(x))      /* aus SUB bzw. REC die Anzahl Variablen */
    {                              /* gleichen Namens lesen in b korrigieren, */
       b -= ARITY(x);              /* da die betreffende innere Funktion */
       PPOPSTACK(S_m);             /* verlassen wird */
       PUSHSTACK(S_a,POPSTACK(S_m1));
       goto main_m;
    }

    if (T_CON(x))
    {
       if (ARITY(x) > 1)           /* das uebliche */
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

    post_mortem("r1eapos main_m: no match");
 }
