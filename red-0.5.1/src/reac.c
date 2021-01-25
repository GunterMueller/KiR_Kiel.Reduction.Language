/* $Log: reac.c,v $
 * Revision 1.2  1992/12/16  12:49:50  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */




/*-----------------------------------------------------------------------------
 *  eac.c  - external:  eac;
 *         Aufgerufen durch:  ea_create
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
#include <setjmp.h>

/*---------------------------------------------------------------------------
 * Externe Variablen:
 *---------------------------------------------------------------------------
 */
extern BOOLEAN _interrupt;    /* reduct.c */
extern jmp_buf _interruptbuf; /* reduct.c */

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */
extern STACKELEM st_name();                 /* storepre.c */
extern STACKELEM st_cond();                 /* storepre.c */
extern STACKELEM st_var();                  /* storepre.c */
extern STACKELEM st_list();                 /* storepro.c */
extern STACKELEM st_dec();                  /* storepre.c */
extern STACKELEM st_mvt();                  /* rstpre.c */
extern void      pmprepa ();

/* ach 29/10/92 */
extern void stack_error();                  /* rstack.c */
extern void trav ();                        /* aeh... */
extern STACKELEM st_expr();                 /* aeh... */
extern void ptrav_a_e();                    /* rtrav.c */
/* end of ach */
/* ach 03/11/92 */
#if DEBUG
extern STACKELEM *ppopstack();               /* rstack.c */
extern void test_inc_refcnt();              /* aeh... */
#endif
/* end of ach */

/*-----------------------------------------------------------------------------
 *  eac        --  Lagert aus einem auf dem E-Stack liegenden Ausdruck alle
 *                 Constanten aus, liefert Ergebnisausdruck auf dem E-Stack ab.
 *  globals    --  S_e,S_a     <w>
 *-----------------------------------------------------------------------------
 */
void eac()
{
  register STACKELEM  x,y;
  register unsigned short  arity;
  extern jmp_buf _interruptbuf;             /* reduct.c */
/*  PTR_DESCRIPTOR desc;        ach 29/10/92 */

  START_MODUL("eac");

  setjmp(_interruptbuf);                    /* Interruptbehandlung Preprocessing */
  if (_interrupt)                           /* Interrupt erfolgt */
  {
    _interrupt = FALSE;
    post_mortem("eac: Interrupt received");
  }

  PUSHSTACK(S_m,DOLLAR);                             /* Ende-Symbol */

main_e:
  x = READSTACK(S_e);

  if T_NILMAT(x) {                         /* leere Matrix ?                 */
    PPOPSTACK(S_e);
    INC_REFCNT(_nilmat);                   /* durch nilmat pointer ersetzen  */
    PUSHSTACK(S_a,(STACKELEM)_nilmat);
    goto main_m;
  }

  if T_NILVECT(x) {                        /* leerer Vektor ?                */
    PPOPSTACK(S_e);                        /* durch nilvect pointer ersetzen */
    INC_REFCNT(_nilvect);
    PUSHSTACK(S_a,(STACKELEM)_nilvect);
    goto main_m;
  }

  if T_NILTVECT(x) {                       /* leerer TVektor ?               */
    PPOPSTACK(S_e);                        /* durch niltvect pointer ersetzen*/
    INC_REFCNT(_niltvect);
    PUSHSTACK(S_a,(STACKELEM)_niltvect);
    goto main_m;
  }
  if T_NIL(x) {                            /* leere Liste ?                  */
    PPOPSTACK(S_e);                        /* durch nil pointer ersetzen     */
    INC_REFCNT(_nil);
    PUSHSTACK(S_a,(STACKELEM)_nil);
    goto main_m;
  }
  if T_NILSTRING(x) {                      /* leerer String ?                */
    PPOPSTACK(S_e);                        /* durch nilstring pointer ersetzen*/
    INC_REFCNT(_nilstring);
    PUSHSTACK(S_a,(STACKELEM)_nilstring);
    goto main_m;
  }

  if (T_CON(x))
  {
    if (T_SCALAR(x))
    {
      post_mortem("eac: INT and REAL no more allowed");
    }

    if T_MATRIX(x) {
      PUSHSTACK(S_a,st_mvt(C_MATRIX));                /* Matrix  auslagern   */
      goto main_m;
    }

    if T_VECTOR(x) {
      PUSHSTACK(S_a,st_mvt(C_VECTOR));                /* Vektor  auslagern   */
      goto main_m;
    }

    if T_TVECTOR(x) {
      PUSHSTACK(S_a,st_mvt(C_TVECTOR));               /* TVektor auslagern   */
      goto main_m;
    }
    if (T_PLUS(x) || T_MINUS(x) || T_EXP(x))
    {                                       /* digit-string auslagern */
       PUSHSTACK(S_a,st_dec());
       goto main_m;
    }

#if PI_RED_PLUS
    if (T_PM_CASE(x)) {
      PPOPSTACK(S_e);
      if (T_PM_WHEN(READSTACK(S_e))) {
        if (ARITY(READSTACK(S_e)) > 3) {
          /* multiple pattern, number of patterns onto a */
          PUSHSTACK(S_a,TAG_INT(ARITY(READSTACK(S_e))-2));
          /* now replace the CAS/SWITCH-constructor by NCASE */
          PUSHSTACK(S_m,SET_ARITY(PM_NCASE,ARITY(x)));
          /* and then increment arity of NCASE */
          PUSHSTACK(S_m1,SET_ARITY(PM_NCASE,ARITY(x)+1));
        }
        else {
          PUSHSTACK(S_m,x);
          PUSHSTACK(S_m1,x);
        }
        goto main_e;
      }
      else post_mortem("eac: WHEN expected on stack e");
    }
#endif /* PI_RED_PLUS     auskommentiert RS 3.12.1992 */
#if PI_RED_PLUS
        if (T_PM_WHEN (x))
        {
          if (!T_PM_CASE(READSTACK(S_m))) {
            trav(&S_e,&S_a);
            PUSHSTACK(S_e,PM_END);
            trav(&S_a,&S_e);
            PUSHSTACK(S_e,SET_ARITY(PM_SWITCH,2));
            goto main_e;
          }
#endif /* PI_RED_PLUS    auskommentiert RS 3.12.1992 */

          PPOPSTACK (S_e);

#if PI_RED_PLUS
          if (ARITY(x) > 3) {
            /* multiple patterns into a list */
            PUSHSTACK(S_e,SET_ARITY(LIST,ARITY(x)-2));
          }
#endif /* PI_RED_PLUS   auskommentiert RS 3.12.1992 */
            pmprepa ();
            goto main_e;
        }
        if (T_K_COMM (x)) {
           PPOPSTACK(S_e);
           PUSHSTACK(S_m,x);
           PUSHSTACK(S_m1,x);
/*           if (T_STRING(READSTACK(S_e))) {                changed by RS 17.6.1993 to: */
             if (T_STRING(READSTACK(S_e)) || T_NILSTRING(READSTACK(S_e))) {
 	     goto main_e;
 	   }
           else {
             if ((x = st_expr()) != 0)
               PUSHSTACK(S_a,x);
             else
               post_mortem("eac: heap out of space");
             goto main_m;
	   }
        }

    if (ARITY(x) > 0)
    {
       PPOPSTACK(S_e);       /* sonstiger Konstruktor mit Stelligkeit > 0 */
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_e;
    }

    PPOPSTACK(S_e);          /* sonst. Konstruktor mit Stelligkeit 0 */
    PUSHSTACK(S_a,x);
    goto main_m;
  }

  if ((T_STR(x)) && !(T_STR_CHAR(x)))       /* digitstring oder letterstring  */
  {                                         /* auslagern?                     */
    PUSHSTACK(S_a,st_name());               /*     Namenspointer erzeugen     */
    goto main_m;                            /* recfunc bzw. subfunc behandelt,*/
  }                                         /* koennen also hier nicht auf-   */
                                            /* tauchen;                       */
  if (T_ATOM(x))
  {                                         /*         sonstige atoms         */
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,x);
    goto main_m;
  }

  post_mortem("eac: No match successful on main_e");

main_m:
  x = READSTACK(S_m);
  if ((T_PM_REC(x)) && (T_PM_MATCH(READSTACK(S_a))) )
  {
    /* when Klausel zurueck*/
    trav(&S_a, &S_e);
    y = POPSTACK(S_a);/* der rekursive Parameter*/
    /* when Klausel wieder nach a*/
    trav(&S_e, &S_a);
    PUSHSTACK(S_a, y);/* der rekursive Parameter des caserec*/

  } 
  arity = ARITY(x);
  if (arity > 1)  /* dh die Arity des Konstruktors war >= 2 */
  {
    WRITE_ARITY(S_m,arity-1);
    goto main_e;
  }
  if (arity > 0)          /* DOLLAR hat arity = 0 */
  {
    PPOPSTACK(S_m);
    x = POPSTACK(S_m1);
    PUSHSTACK(S_a,x);
    goto main_m;
  }
  /* Ein DOLLAR. Dann ist eac zu beenden */
  PPOPSTACK(S_m);         /* DOLLAR */

  PTRAV_A_E;             /* ausdruck wieder zurueck */

  END_MODUL("eac");
}
/* end of eac.c */
