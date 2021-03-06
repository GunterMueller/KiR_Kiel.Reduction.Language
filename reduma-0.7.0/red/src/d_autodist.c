#ifdef AUTODIST

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include "dbug.h"
#include <setjmp.h>

/* defines: */

#define T_AUTODIST_LIST(Stack) (T_LIST(READSTACK(Stack)) && \
                               (!T_LREC(MIDSTACK(Stack,1))))
#define T_AUTODIST_AP(x)       (T_AP(x) && !T_DAP(x))

/*---------------------------------------------------------------------------
 * Externe Variablen:
 *---------------------------------------------------------------------------
 */
extern BOOLEAN _interrupt;    /* reduct.c */
extern jmp_buf _interruptbuf; /* reduct.c */
extern StackDesc S_pm;
extern stack_error();
#if DEBUG
extern test_dec_refcnt();
extern DescDump();
extern midstack();
extern updatestack();
#endif

extern int d_autodist_unary;
extern char d_autodist_name[];
extern int d_autodist_ack; 

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */

extern void trav ();
extern void ptrav_a_e();                    /* rtrav.c */
extern void trav_a_e();
extern void trav_e_a();
extern void trav_a_hilf();
extern void trav_hilf_a();
extern newdesc(); 
extern newheap();

extern getkey();
extern clearscreen();

#if DEBUG
extern STACKELEM *ppopstack();              /* rstack.c */
extern void test_inc_refcnt();
#endif

/********************************************/

int d_lpcounter=0;

PTR_DESCRIPTOR d_its_me;

int d_autodist_app[5];

/********************************************/

void d_unary_trav_e_a()
{
    register STACKELEM  x = POPSTACK(S_e);
    register int        i;
  
    DBUG_ENTER ("d_unary_trav_e_a");

    START_MODUL ("unary_trav_e_a");

    if (T_CON(x)) {
      for (i = ARITY(x); --i>=0; )
        d_unary_trav_e_a();
      PUSHSTACK(S_a,x);
      }
    else if (T_POINTER(x) && ((int) x == (int) d_its_me)) {
      DEC_REFCNT ((PTR_DESCRIPTOR) x);
      trav_hilf_a();
      }
    else
      PUSHSTACK(S_a,x);

    END_MODUL ("unary_trav_e_a");

    DBUG_VOID_RETURN;
}

/********************************************************************/
/**/
/**/
/********************************************************************/

void d_mknewname()
{
 register PTR_DESCRIPTOR desc;
 STACKELEM *descname;
 static char d_lpname[80];
 int namelength, i;

 DBUG_ENTER ("d_mknewname");

 if ((int)(desc = (PTR_DESCRIPTOR) newdesc()) == NULL)
   post_mortem("d_mknewname: heap out of space");
 DESC_MASK(desc,1,C_EXPRESSION,TY_NAME);

 sprintf (d_lpname, "%s_%i", d_autodist_name, d_lpcounter);
 namelength = strlen(d_lpname);

 if (newheap( 1 + strlen(d_lpname),(int *) A_NAME((*desc),ptn)) == 0)
   post_mortem("d_mknewname: heap out of space");
 descname = (STACKELEM *) R_NAME((*desc),ptn);
 
 /* String eintragen : */
 for ( i = 1; i <= namelength; i++)
   if (i == 1)
     descname[i] = (((int)d_lpname[i-1]) << 16) | 0x802c; 
   else if (i == namelength)
     descname[i] = (((int)d_lpname[i-1]) << 16) | 0x800c; 
   else
     descname[i] = (((int)d_lpname[i-1]) << 16) | 0x002c ;
 descname[0] = namelength;

 PUSHSTACK (S_a, desc); 
 
 PUSHSTACK (S_v, desc);
 INC_REFCNT(desc);

 DBUG_VOID_RETURN;
}

/********************************************************************/
/**/
/**/
/********************************************************************/

/* body on S_a, counter as argument, expressions on S_hilf, on top is the
name descriptor, expressions on S_hilf not transposed */

void d_mkautoletpar(d_num)
int d_num;
{
  int i;

  DBUG_ENTER ("d_mkautoletpar");

  START_MODUL("autoletpar");

  if (d_num > 4)
    d_autodist_app[4]++;
  else
    d_autodist_app[d_num]++;
 
  if ((d_num == 1) && d_autodist_unary) { /* don't insert letpar for unaries */

    /* first: traverse body back to e-Stack */
    trav_a_e();

    /* now traverse expression back to a-Stack while comparing descriptors */
    d_its_me = (PTR_DESCRIPTOR) POPSTACK(S_hilf);
    d_unary_trav_e_a();
    DEC_REFCNT(d_its_me);

    }
  else {

  /* first: traverse body back to e-Stack */
  trav_a_e();

  /* now put the expressions for the arguments on a-Stack (transposed) */
  for (i = 0; i<d_num; i++) {
    PUSHSTACK(S_pm, POPSTACK(S_hilf));   /* put name-descriptor on S_pm */
    trav_hilf_a();                       /* traverse expressions back to a */
    }

  /* now traverse name-descriptors back to a */
  for (i = 0; i<d_num; i++)
    PUSHSTACK(S_a, POPSTACK(S_pm));

  /* now put body back to a-Stack */
  trav_e_a();

  /* now put sub-constructor on a */
  PUSHSTACK(S_a, SET_ARITY(SUB, d_num+1));

  /* now put let/letpar-constructor on a */
  PUSHSTACK(S_a, SET_ARITY(P_DAP, d_num+1));

    }

  END_MODUL ("autoletpar");

  DBUG_VOID_RETURN;
}

/********************************************************************/
/**/
/**/
/********************************************************************/

void d_trav_e_a()
{
  register STACKELEM  x;
  register STACKELEM  y;
  register int arity;

  DBUG_ENTER ("d_trav_e_a");
  START_MODUL ("d_trav_e_a");

 PUSHSTACK(S_m,DOLLAR);                             /* Ende-Symbol */

main_e:

  x = READSTACK(S_e);

  if (T_CON(x)) {
    if (ARITY(x) > 0) {
      PPOPSTACK(S_e);       /* Konstruktor mit Stelligkeit > 0 */
      PUSHSTACK(S_m,x);
      PUSHSTACK(S_m1,x);
      if (T_AUTODIST_AP(x) || T_AUTODIST_LIST(S_m))      /* put counter on stack S_i */
        PUSHSTACK(S_i, TAG_INT(0));
      goto main_e;
      }
    PPOPSTACK(S_e);          /* sonst. Konstruktor mit Stelligkeit 0 */
    PUSHSTACK(S_a,x);
    goto main_m;
    }

  if (T_ATOM(x))
  {                                         /*         sonstige atoms         */
    PPOPSTACK(S_e);
    PUSHSTACK(S_a,x);
    goto main_m;
  }
  post_mortem("autodist: No match successful on main_e");

main_m:
  x = READSTACK(S_m);
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
    y = READSTACK(S_a);
    PUSHSTACK(S_a,x);

    if (T_AUTODIST_AP(x) && T_POINTER(y) && \
        (R_DESC((*(PTR_DESCRIPTOR)y),type) == TY_NAME)) {
      if (VAL_INT(READSTACK(S_i)) != 0) { /* recursiv letpar !!! */
        d_mkautoletpar(VAL_INT(POPSTACK(S_i)));
        PUSHSTACK(S_i, TAG_INT(0));
        }
      trav_a_hilf();
      d_mknewname();
      d_lpcounter++;
      PUSHSTACK(S_hilf, READSTACK(S_a));
      INC_REFCNT((PTR_DESCRIPTOR) READSTACK(S_a));
      UPDATESTACK(S_i, 0, TAG_INT(VAL_INT(READSTACK(S_i))+1));
    }
   
    if (T_AUTODIST_AP(x) || (T_LIST(x) && !T_LREC(READSTACK(S_m)))) {

    if (!T_DOLLAR(READSTACK(S_m)) && (T_AUTODIST_AP(READSTACK(S_m)) || \
                                      T_AUTODIST_LIST(S_m))) {
      /* update underlying entry */
      x = VAL_INT(POPSTACK(S_i));
      UPDATESTACK(S_i, 0, TAG_INT(VAL_INT(READSTACK(S_i)) + x));
      }
    else if (SIZEOFSTACK(S_i) > 0) {
           if ((x = VAL_INT(POPSTACK(S_i))) != 0)  /* now build up letpar or let */ 
             d_mkautoletpar(x);
           else
             d_autodist_app[0]++;
      }
    
      }

    goto main_m;
  }
  PPOPSTACK(S_m);         /* DOLLAR */

  END_MODUL ("d_trav_e_a");
   
  DBUG_VOID_RETURN;
}

/***********************/
/* function d_autodist */
/***********************/

void d_autodist()
{
  extern jmp_buf _interruptbuf;             /* reduct.c */
  int i;

  DBUG_ENTER ("d_autodist");

  START_MODUL("autodist");

  setjmp(_interruptbuf);                    /* Interruptbehandlung Preprocessing */
  if (_interrupt)                           /* Interrupt erfolgt */
  {
    _interrupt = FALSE;
    post_mortem("autodist: Interrupt received");
  }
  
  for (i = 0; i<5; i++)
    d_autodist_app[i] = 0;

  d_lpcounter = 0;

  d_trav_e_a();

  trav_a_e();

  if (d_autodist_ack) {
   clearscreen();
   printf (" autodist statistics: \n\n");
   if (d_autodist_app[0] > 0)
     printf (" %d tr%s failed to insert a letpar,\n", d_autodist_app[0], (d_autodist_app[0] == 1) ? "y" : "ies");
   if (d_autodist_unary == 0) {
     if (d_autodist_app[1] > 0)
       printf (" %d possible letpar%s containing one expression,\n", d_autodist_app[1], (d_autodist_app[1] == 1) ? "" : "s");
     }
   else
      if (d_autodist_app[1] > 0) 
       printf (" %d unary letpar%s transformed back,\n", d_autodist_app[1], (d_autodist_app[1] == 1) ? "" : "s");
   if (d_autodist_app[2] > 0)
     printf (" %d letpar%s inserted distributing two expressions,\n", d_autodist_app[2], (d_autodist_app[2] == 1) ? "" : "s" );
   if (d_autodist_app[3] > 0)
     printf (" %d letpar%s inserted distributing three expressions and\n", d_autodist_app[3], (d_autodist_app[3] == 1) ? "" : "s" ); 
   if (d_autodist_app[4] > 0)
     printf (" %d letpar%s inserted distributing four or more expressions.\n", d_autodist_app[4], (d_autodist_app[4] == 1) ? "" : "s" );
   printf ("\n press return...\n");
   fflush(stdout); 
   getkey();
   }

  END_MODUL("autodist");

  DBUG_VOID_RETURN;
}

#endif
