/* $Log: rea-ret.c,v $
 * Revision 1.9  1996/02/21  18:19:30  stt
 * move ret_intact-call to ea-retrieve
 *
 * Revision 1.8  1995/11/15  15:04:10  cr
 * postprocessing for frames (ret_frame) and slots (ret_slot)
 *
 * Revision 1.7  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.6  1994/10/21  15:12:03  rs
 * *** empty log message ***
 *
 * Revision 1.5  1994/10/20  14:24:25  rs
 * some UH_ZF_PM stuff added
 *
 * Revision 1.4  1993/10/07  15:53:48  mah
 * *** empty log message ***
 *
 * Revision 1.2  1992/12/16  12:49:48  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*****************************************************************************/
/*                       MODUL FOR POSTPROCESSING PHASE                      */
/* ------------------------------------------------------------------------- */
/* rea-ret.c : - external  : ea_retrieve                                     */
/*             - internal  : ae_retrieve                                     */
/*             - called by : rreduct.c                                       */
/*****************************************************************************/

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
extern BOOLEAN _interrupt;    /* reduct:c */
extern jmp_buf _interruptbuf; /* reduct:c */

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */

/* TB, 30.10.92 */
extern void stack_error();         /* rstack.c */
extern void trav();                /* rtrav.c */
extern void trav_a_e();            /* rtrav.c */
extern void ret_func();            /* rhinout.c */
extern void ret_expr();            /* rhinout.c */
extern void ret_dec();             /* rhinout.c */
extern void freeheap();            /* rheap.c */
extern void convi();               /* rconvir.c */
/* end of TB */

#if DEBUG
extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */  /* rstack.c */
extern void display_stack(); /* TB, 4.11.1992 */   /* rruntime.c */
#endif

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

extern void ldel_expr();           /* ldel-expr.c */
extern void lrec_args();           /* lr-args.c */
extern void aeret_lrec();          /* lr-ret.c */
extern int earet_lrec();           /* lr-ret.c */
extern void set_protect();         /* lr-prot.c */
#if DORSY
extern T_HEAPELEM * lfun_def();    /* lfun-def.c */
#else
extern int check_goal_expr();      /* lr-goal.c */
#endif
extern int lpa_sub();              /* lpa-sub.c */
extern void ret_name();            /* retrieve:c */
extern void ret_scal();            /* retrieve:c */
extern void ret_cond();            /* retrieve:c */
extern void ret_var();             /* retrieve:c */
extern void ret_list();            /* retrieve:c */
extern void ret_frame();            /* rhinout.c */
extern void ret_slot();            /* rhinout.c */
extern void ret_mvt();             /* rhinout.c  */
extern void ret_intact();          /* rhinout.c  */
extern  void pm_ret_switch ();            /* retrieve:c */
extern  void pm_ret_match ();             /* retrieve:c */
extern  void pm_ret_nomatch ();           /* retrieve:c */
extern void trav_e_a();            /* trav:c     */
extern int part_rec();             /* part-rec:c */
extern int part_sub();             /* part-sub:c */
extern void pos_func();            /* pos-func:c */

static void ae_retrieve();      /* forward declaration */
/*-----------------------------------------------------------------------------
 *  ea_retrieve  --  Postprocessed einen auf dem E-Stack liegenden Ausdruck,
 *                   und liefert den Ergebnisausdruck auf dem E-Stack ab.
 *                   Dabei wird eine partielle Substitution durchgefuehrt,
 *                   d.h. es werden alle SNAP's aufgeloest. part_sub() bzw.
 *                   part_rec() legen den Inkarnationsblock an. Die eigentliche
 *                   Substitution der Nummernvariablen fuehrt dann ea_retrieve
 *                   durch. Anschliessend erfolgt die Umbenennung der ver-
 *                   bliebenen Nummernvariablen in ihre urspruenglichen Namen.
 *                   Diese Umbenennung wird erst gestartet, wenn ea_retrieve
 *                   die zugehoehrige Funktion auf den A-Stack traversiert hat.
 *                   Das Einlagern der Datenstrukturen erfolgt durch ae_retrieve.
 *  ak:              Relativ freie Variablen koennen mit Hilfe von UNPROTECTS
 *                   wie global freie Variablen behandelt werden.
 *  globals    --    S_e,S_a,S_m,S_m1   <w>
 *
 *-----------------------------------------------------------------------------
 */
void ea_retrieve()
{
#if UH_ZF_PM 
  int zfbound=0;         /* Anzahl der gebundenen zf-Variablen */
#endif /* UH_ZF_PM */
  PTR_HEAPELEM p;
  register STACKELEM  x, hilf; /* Initialisierung von TB, 6.11.1992 */
  register VOLATILE STACKELEM y=(STACKELEM)0; /* volatile: ach 10/11/92 */
  register unsigned short  arity;
  int narg, rest;
  int i,j;
  char class1;                /* for 2nd parameter of function call ret_mvt */
  PTR_DESCRIPTOR desc;
  STACKELEM xy,xd;
  int first,check,snarity,func,f_lrec,lrec,/*ari,*/lpa; /* TB, 30.10.92 */
#if DORSY
  T_HEAPELEM * VOLATILE tab=(T_HEAPELEM *)0; /* Initialisierung von TB, 6.11.1992, volatile: ach 10/11/92 */
  VOLATILE int t_exists;        /* volatile: ach 10/11/92 */

  t_exists = 0;  /* fuer freeheap(tab) vor return */
#else            /* !DORSY */
  STACKELEM lis;
#endif           /* !DORSY */
  lpa = 0;
  first = 0;
  check = 0;
  snarity = 0;
  func = 0;
  f_lrec = 0;
  lrec = 0;     /* zur Unterscheidung von Funktionsdefinitionen und  */
                /* goal_expr : muss rekursiv nachgehalten werden, da */
                /* LREC-Strukturen in Startausdruecke von Fuktions-  */
                /* definitionen substituiert werden koennen          */
                /* => f_lrec und func                                */

  START_MODUL("ea_retrieve");

  setjmp(_interruptbuf);           /* Interruptbehandlung neu initialisieren */
  if (_interrupt)                  /* Interrupt aufgetreten? */
  {
    _interrupt = FALSE;            /* zuruecksetzen */
    post_mortem("ea_retrieve: Interrupt received");   /* alles abbrechen */
  }                                                  /* und zurueck zum Editor */

  PUSHSTACK(S_m,KLAA);             /* Endemarkierung @ */

  PUSHSTACK(S_hilf,KLAA); /* Hilsstack fuer plrec(_args)      */
                          /* werden benoetigt, um lambda-bars */
                          /* einsetzen zu koennen             */
#if DORSY
  first = 0;
#else
  PUSHSTACK(S_v,KLAA);    /* Hilfsstack fuer func_names */

  x = READSTACK(S_e);
  if ( (T_LREC(x))
     || ( (T_POINTER(x))
        && (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_EXPR) ))
  {
     check = check_goal_expr();
                 /* loest C_LIST und TY_EXPR sowie temporaer        */
                 /* TY_SUB, TY_REC, TY_COND und func_defs hinter    */
                 /* TY_LREC(_ARGS) im Startausdruck auf, um         */
                 /* plrec_ind's zu finden:                          */
                 /* -  entfernt die LREC's, auf die keine Verweise  */
                 /*    via "plrec_ind" existieren                   */
                 /* -  sammelt die LREC's, die rekonstruiert werden */
                 /*    muessen                                      */
     first = 1;
     lrec = 1;            /* Funktionsdefinitionen */
     if (check)           /* es gibt LREC-Strukturen */
        first = earet_lrec(first,POPSTACK(S_a));
                          /* erster Aufruf nach check_goal_expr */
     else
        first = 0;        /* kein p_lrec */
  }
#endif
main_e:
  x = POPSTACK(S_e);

  if (T_CON(x))
  {
#if DORSY
#else
     if ( (T_LREC(x)) && ((!lrec) || (!f_lrec)) )
     {           /* Startausdruck (auch in func_def) : SNAP-args    */
                 /* werden gebraucht, z.B. in part_sub, aber nicht  */
                 /* die LREC's                                      */
                 /* => Stelligkeit -2 fuer LREC                     */
                 /* plrec(_args) auf S_hilf und S_a,                */
                 /* Stelligkeit (S_m1,LREC) fuer "snarity"          */
        STACKELEM y,z;
        int args;

        y = READSTACK(S_e);
        if (T_SNAP(y))
        {
           args = ARITY(y) - 1;
           lrec_args(1,1);                  /* => plrec_args */
           z = POPSTACK(S_e);               /* plrec_args */
           PUSHSTACK(S_hilf,z);
           PUSHSTACK(S_a,z);
           INC_REFCNT((PTR_DESCRIPTOR)z);
        }
        else
        if (R_DESC((*(PTR_DESCRIPTOR)y),type) == TY_LREC_ARGS)
        {
           POPSTACK(S_e);
           PUSHSTACK(S_hilf,y);
           PUSHSTACK(S_a,y);
           INC_REFCNT((PTR_DESCRIPTOR)y);
           args = R_LREC_ARGS((*(PTR_DESCRIPTOR)y),nargs);
        }
        else
        if (R_DESC((*(PTR_DESCRIPTOR)y),type) == TY_LREC)
        {
           POPSTACK(S_e);
           PUSHSTACK(S_hilf,y);
           PUSHSTACK(S_a,y);
           INC_REFCNT((PTR_DESCRIPTOR)y);
           args = 0;
        }
        else
           post_mortem("ea_retrieve:unexpected lrec type");
        PUSHSTACK(S_m,SET_ARITY(x,-2));     /* LREC(-2) */
        PUSHSTACK(S_m1,SET_ARITY(x,args));   /* LREC(args) */
        goto main_e;               /* Startausdruck eines LREC's im */
                                   /* Startausdruck                 */
     }
     else
     if ((T_LREC(x)) && (f_lrec))
     {
        PUSHSTACK(S_m,SET_ARITY(PRELIST,-1));
        PUSHSTACK(S_m,x);
        PUSHSTACK(S_m1,x);
        goto main_e;
     }
     else
#endif
     if (T_SNAP(x))
     {
        PUSHSTACK(S_m1,x);
        PUSHSTACK(S_m,SET_ARITY(SNAP,1));
        for (i=ARITY(x),i--; i > 0; i--)    /* Argumente fuer SNAP auf S_a  */
        {                                   /* legen, um das vorzeitige     */
          x = READSTACK(S_e);               /* Expandieren eines Rekursions-*/
          if T_CON(x)  trav_e_a();          /* pointers zu verhindern,      */
          else                              /* wenn naemlich ein Rekursions-*/
          {                                 /* pointer kuenstliches         */
             PPOPSTACK(S_e);                /* Argument ist                 */
             if T_NUM(x)  READ_I(VALUE(x),x);
             PUSHSTACK(S_a,x);
          }
        }
        goto main_e;
     }
     else
     /*fuer die Darstellung eines Case mit einem guard ungleich true oder*/
     /*false ist das Argument des Case noch einmal auf den E-Stack kopiert*/
     /*worden. Dieses Argument enthaelt ein schon ausgewertetes letrec    */
     if ((T_LREC(x)) && (ARITY(x) == 3))
     {
        /*letrec zurueck*/
        PUSHSTACK(S_e, x);

        /*letrec Ausdruck wieder auf den A-Stack*/
        trav(&S_e, &S_a);
        
         goto main_m;
     }
     else
     {
       if (ARITY(x) > 0)
       {
         PUSHSTACK(S_m,x);                  /* sonstiger Konstruktor */
         PUSHSTACK(S_m1,x);
         goto main_e;
       }
       else
       {
         PUSHSTACK(S_a,x);                  /* sonstiger Konstruktor */
         goto main_m;
       }
     }
  }     /* end if (T_CON())        */
  else


  if (T_POINTER(x))
  {
   if ( (!(T_KLAA(READSTACK(S_m))))
      && ( (R_DESC((*(PTR_DESCRIPTOR)x),class) == C_EXPRESSION)
         || (R_DESC((*(PTR_DESCRIPTOR)x),class) == C_LIST) ) )
                 /* nur fuer SUB-, REC-, COND-pointer ? */
                 /* sowie fuer SWITCH-, MATCH-, NOMATCH-pointer ?! */
      if ((SIZEOFSTACK(S_m1)) && (T_LIST(READSTACK(S_m1)))) {
         STACKELEM z,wz;

         xy = POPSTACK(S_m);
         z = POPSTACK(S_m1);
         wz = READSTACK(S_m);
         if ((T_LREC(wz))
            && ( (ARITY(wz) > 1)
               || (ARITY(wz) == -1)
               || (ARITY(wz) == -2) ))
            snarity = ARITY(READSTACK(S_m1));
         else
            snarity = 0;
                 /* snarity == Anzahl der relativ freien Variablen */
                 /* in den Funktionsdefinitionen eines LRECs       */
#if DORSY
#else
         if ((T_LREC(wz))
            && ( (ARITY(wz) > 1)
               || (ARITY(wz) == -1) ))
            func = 1;
         else
            func = 0;
#endif
         PUSHSTACK(S_m,xy);
         PUSHSTACK(S_m1,z);
      }
      else
         snarity = 0;
   else
      snarity = 0;
    switch(R_DESC((* ((PTR_DESCRIPTOR)x)),type))
    {
/* dg */      case TY_CONS:
/* dg */                    desc = R_CONS(*(T_PTD)x,tl);
/* dg */                    if (T_POINTER(desc)) INC_REFCNT(desc);
/* dg */                    PUSHSTACK(S_e,desc);
/* dg */                    desc = R_CONS(*(T_PTD)x,hd);
/* dg */                    if (T_POINTER(desc)) INC_REFCNT(desc);
/* dg */                    PUSHSTACK(S_e,desc);
/* dg */                    DEC_REFCNT((T_PTD)x);
/* dg */                    PUSHSTACK(S_m,SET_ARITY(CONS,2));
/* dg */                    PUSHSTACK(S_m1,SET_ARITY(CONS,2));
/* dg */                    goto main_e;
/* dg */      case TY_COMB:
/* dg */                    desc = R_COMB((*(T_PTD)x),ptd);
/* dg */                    /* desc zeigt jetzt entweder auf einen */
/* dg */                    /* LREC_IND oder einen SUB descriptor  */
/* dg */                    /* ohne relativ freien variablen !     */
/* dg */                    INC_REFCNT(desc);
/* dg */                    PUSHSTACK(S_e,desc);
/* dg */                    DEC_REFCNT((T_PTD)x);
/* dg */                    goto main_e;
/* dg */      case TY_CASE:
/* dg */                    desc = R_CASE((*(T_PTD)x),ptd);
/* dg */                    /* desc zeigt jetzt auf einen SWITCH descriptor */
/* dg */                    /* ohne relativ freien variablen !     */
/* dg */                    DEC_REFCNT((T_PTD)x);
/* dg */                    INC_REFCNT(desc);
/* dg */                    PUSHSTACK(S_e,desc);
/* dg */                    goto main_e;
/* dg */      case TY_CLOS:
/* dg */                    desc = (T_PTD)x;
/* dg */                    arity = R_CLOS((*desc),args);     /* clos. args */
/* dg */                    narg  = R_CLOS((*desc),nargs);    /* argsneeded */
/* dg */                  /*L_clos:*/ /* TB, 30.10.92 */
/* dg */                    /* einlagern vgl. rinter.c (I_APPLY) */
/* dg */                    RES_HEAP;
/* dg */                    p = (PTR_HEAPELEM)R_CLOS((*desc),pta);
/* dg */                    /* die argumente der closure auf a */
/* dg */                    for (i = R_CLOS(*desc,args) ; i >= 0 ; i--) {
/* dg */                      if (T_POINTER((int)p[i]))
/* dg */                        INC_REFCNT((T_PTD)p[i]);
/* dg */                      PUSHSTACK(S_a,p[i]);
/* dg */                    }
/* dg */                    REL_HEAP;
/* dg */                    desc = (T_PTD)POPSTACK(S_a);
#if UH_ZF_PM
/*uh, 17.2.92*/  if (T_POINTER(desc) && R_DESC((*desc),type) == TY_SELECTION) {
/*uh, 17.2.92*/       /* Hier ist eine CASE-Closure gefunden worden. Die */
/*uh, 17.2.92*/       /* Variablennamen, der Variablen, die waehrende der Auswertung des CASEs  */
/*uh, 17.2.92*/       /* gebunden sind, befinden sich in der Namensliste des SUB-Descriptors,   */
/*uh, 17.2.92*/       /* die in der Pattern-Komponente des CLAUSE-Triples steht.  */
/*uh, 17.2.92*/       /* Im Argumentvektor des Closure-Descriptors findet sich die Struktur:    */
/*uh, 17.2.92*/       /* {SELdesc|casearg|args}                                                 */
/*uh, 17.2.92*/       /* Sie wird jetzt in Applikationen und SnApplikationen aufgeloest.        */
/*uh, 17.2.92*/       /* Auf A liegen jetzt schon die Argumente oben auf der SEL-Desc.          */
/*uh, 17.2.92*/
/*uh, 17.2.92*/       /* zunaechst mal Argumentzahl ermitteln */
/*uh, 17.2.92*/          int nargs;
/*uh, 17.2.92*/          T_PTD subdesc,clausedesc;
/*uh, 17.2.92*/
/*uh, 17.2.92*/          clausedesc=R_SELECTION(*desc,clauses);
/*uh, 17.2.92*/          if (clausedesc)
/*uh, 17.2.92*/          {
/*uh, 17.2.92*/             subdesc=(T_PTD)R_CLAUSE(*clausedesc,sons)[0];
/*uh, 17.2.92*/             nargs=R_FUNC(*subdesc,nargs)-R_FUNC(*subdesc,namelist)[0];
/*uh, 17.2.92*/          }
/*uh, 17.2.92*/          else
/*uh, 17.2.92*/          {
/*uh, 17.2.92*/             nargs=0;
/*uh, 17.2.92*/          }
/*uh, 17.2.92*/
/*uh, 17.2.92*/          PUSHSTACK(S_a,desc);
/*uh, 17.2.92*/          PUSHSTACK(S_a,SET_ARITY(SNAP,nargs+1));
/*uh, 17.2.92*/          PUSHSTACK(S_a,SET_ARITY(AP,2));
/*uh, 17.2.92*/          trav_a_e();
/*uh, 17.2.92*/          goto main_e;
/*uh, 17.2.92*/  }
/*uh, 26.12.90*/
/*uh, 26.12.90*/  /* ------------------------- Ende List comprehensions -----------------------*/
/*uh, 26.12.90*/
/*uh, 26.12.90*/        case TY_SELECTION:
/*uh, 26.12.90*/           /* An dieser Stelle werden die Klauseln und der  */
/*uh, 26.12.90*/           /* Otherwise-Ausdruck aus dem Selection-Descriptor ausgegraben */
/*uh, 26.12.90*/           /* und auf den Stack E geworfen, damit sie weiterbehandelt */
/*uh, 26.12.90*/           /* werden koennen. */
/*uh, 26.12.90*/           { int nclauses=0;
/*uh, 26.12.90*/             T_PTD clause=R_SELECTION(*(T_PTD)x,clauses);
/*uh, 26.12.90*/             T_PTD otherwise=R_SELECTION(*(T_PTD)x,otherwise);
/*uh, 26.12.90*/
/*uh, 26.12.90*/             if (otherwise==NULL) { /* keine Otherwise Klausel */
/*uh, 26.12.90*/                PUSHSTACK(S_e,PM_END);
/*uh, 26.12.90*/             } else { /* Mit Otherwise-Klausel */
/*uh, 26.12.90*/                PUSHSTACK(S_e,otherwise);
/*uh, 26.12.90*/                PUSHSTACK(S_e,SET_ARITY(PM_OTHERWISE,1));
/*uh, 26.12.90*/             }
/*uh, 26.12.90*/
/*uh, 26.12.90*/             PUSHSTACK(S_a,NULL);
/*uh, 26.12.90*/
/*uh, 26.12.90*/             while (clause!=NULL) { /* Die Klauseln auf den Stack E */
/*uh, 26.12.90*/                PUSHSTACK(S_a,clause);
/*uh, 26.12.90*/                clause = R_CLAUSE(*clause,next);
/*uh, 26.12.90*/                nclauses++;
/*uh, 26.12.90*/             }
/*uh, 26.12.90*/
/*uh, 26.12.90*/             while (READSTACK(S_a)!=NULL) {
/*uh, 26.12.90*/                clause=(T_PTD)POPSTACK(S_a);
/*uh, 26.12.90*/                PUSHSTACK(S_e,clause);
/*uh, 26.12.90*/             }
/*uh, 26.12.90*/             POPSTACK(S_a);
/*uh, 26.12.90*/
/*uh, 26.12.90*/
/*uh, 26.12.90*/             /* Umgebende Variablen auf den Bindungsstack */
/*uh, 26.12.90*/
/*uh, 26.12.90*/             clause =  R_SELECTION(*(T_PTD)x,clauses);
/*uh, 26.12.90*/
/*uh, 26.12.90*/             if (clause && T_SNAP(READSTACK(S_m))) {
/*uh, 26.12.90*/                T_PTD subdesc = (T_PTD)R_CLAUSE(*clause,sons)[0];
/*uh, 26.12.90*/                int i;
/*uh, 26.12.90*/                T_PTD desc;
/*uh, 26.12.90*/                int nargs=R_FUNC(*subdesc,nargs)-R_FUNC(*subdesc,namelist)[0];
/*uh, 26.12.90*/                printf("%d Variablen in der Closure!\n",nargs);
/*uh, 26.12.90*/
/*uh, 26.12.90*/                POPSTACK(S_m); /* SNAP */
/*uh, 26.12.90*/                POPSTACK(S_m1);  /* Auf m1 steht auch ein SNAP, der sagt wieviele werte bekannt sind */
/*uh, 26.12.90*/                for (i=0; i<nargs; i++) {
/*uh, 26.12.90*/                 /*  PUSHSTACK(S_i,R_FUNC(*subdesc,namelist)[i]); */
/*uh, 26.12.90*/                 desc = (T_PTD)POPSTACK(S_a);
/*uh, 26.12.90*/                 PUSHSTACK(S_i,desc);
/*uh, 26.12.90*/                }
/*uh, 26.12.90*/                PUSHSTACK(S_m,nargs);
/*uh, 26.12.90*/             } else { /* keine umfassendes Frame anyulegen */
/*uh, 26.12.90*/                PUSHSTACK(S_m,0);  /* Anzahl der Variablen */
/*uh, 26.12.90*/             }
/*uh, 26.12.90*/
/*uh, 26.12.90*/             PUSHSTACK(S_m,SET_ARITY(PM_SWITCH,nclauses+1));
/*uh, 26.12.90*/             PUSHSTACK(S_m1,SET_ARITY(PM_SWITCH,nclauses+1));
/*uh, 26.12.90*/
/*uh, 26.12.90*/             goto main_m;
/*uh, 26.12.90*/          }
/*uh, 26.12.90*/
/*uh, 26.12.90*/
/*uh, 26.12.90*/       case TY_CLAUSE:  {
/*uh, 26.12.90*/         /* ein Clausel-Descriptor auf E. */
/*uh, 26.12.90*/         /* Hier muessen jetzt Pattern Guard und Body aufgedroeselt werden. */
/*uh, 26.12.90*/           T_PTD pattern = (T_PTD)R_CLAUSE(*(T_PTD)x,sons)[0];
/*uh, 26.12.90*/           T_PTD guard   = (T_PTD)R_CLAUSE(*(T_PTD)x,sons)[1];
/*uh, 26.12.90*/           T_PTD body    = (T_PTD)R_CLAUSE(*(T_PTD)x,sons)[2];
/*uh, 26.12.90*/
/*uh, 26.12.90*/           PUSHSTACK(S_e,body);       INC_REFCNT(body);
/*uh, 26.12.90*/           PUSHSTACK(S_e,guard);      INC_REFCNT(guard);
/*uh, 26.12.90*/
/*uh, 26.12.90*/           /* Das Pattern wird durch einen SUB-Descriptor dargestellt, dessen */
/*uh, 26.12.90*/           /* Expression jetzt geladen werden muss. */
/*uh, 26.12.90*/
/*uh, 26.12.90*/           /* PUSHSTACK(S_e,pattern);    INC_REFCNT(pattern); */
/*uh, 26.12.90*/           load_expr((STACKELEM **)A_FUNC(*(T_PTD)pattern,pte));
/*uh, 26.12.90*/
/*uh, 26.12.90*/
/*uh, 26.12.90*/           /* Patternvariablen auf Bindungsstack */
/*uh, 26.12.90*/           {  int npatvar=R_FUNC(*pattern,namelist)[0];
/*uh, 26.12.90*/              int i;
/*uh, 26.12.90*/           /*   printf(" Jetzt schreibe ich die Bindungen auf Stack I\n"); */
/*uh, 26.12.90*/             for (i=npatvar;i>=1;i--) {
/*uh, 26.12.90*/                PUSHSTACK(S_i,R_FUNC(*pattern,namelist)[i]);
/*uh, 26.12.90*/             }
/*uh, 26.12.90*/           PUSHSTACK(S_m,npatvar);
/*uh, 26.12.90*/           }
/*uh, 26.12.90*/
/*uh, 26.12.90*/           PUSHSTACK(S_m,SET_ARITY(PM_WHEN,3));
/*uh, 26.12.90*/           PUSHSTACK(S_m1,SET_ARITY(PM_WHEN,3));
/*uh, 26.12.90*/
/*uh, 26.12.90*/           goto main_e;
/*uh, 26.12.90*/        }
/*uh, 26.12.90*/
/*uh, 26.12.90*/
/*uh, 26.12.90*/
#endif /* UH_ZF_PM */
#ifdef NotUsed
/* dg */                    /* vorsicht falls desc auch eine closure ist */
/* dg */                    if (T_POINTER((int)desc) && (R_DESC((*desc),type) == TY_CLOS)) {
/* dg */                      arity += R_CLOS((*desc),args);
/* dg */                      goto L_clos;
/* dg */                    }
#endif /* NotUsed auskommentiert RS 3.12.1992 */
/* dg */                    /* desc zeigt jetzt auf die funktion der closure */
/* dg */                    if (T_POINTER((int)desc)) {
/* dg */                      if (R_DESC((*desc),type) == TY_COMB) {
/* dg */                        T_PTD ptdesc = R_COMB((*desc),ptd);
/* dg */                        desc = ptdesc;
/* dg */                        /* desc zeigt jetzt entweder auf einen */
/* dg */                        /* LREC_IND oder einen SUB descriptor  */
/* dg */                        INC_REFCNT(desc);
/* dg */                        PUSHSTACK(S_a,desc);
/* dg */                        /* falls LREC_IND: dereferenzieren */
/* dg */                        if (R_DESC((*desc),type) == TY_LREC_IND)
/* dg */                          ptdesc = R_LREC_IND((*desc),ptf);
/* dg */                        if (R_DESC((*ptdesc),type) == TY_SUB) {
/* dg */                          int ownargs,subargs;
/* dg */                          subargs = R_FUNC((*ptdesc),nargs);
/* dg */                          ownargs = R_FUNC((*ptdesc),namelist)[0];
/* dg */                          if ((desc != ptdesc) && (subargs-ownargs < (int) arity) && ((int) arity < subargs)) {
      /* zweimal auf int gecastet von RS 04/11/92 */ 
/* dg */                            /* partielle anwendung eines LREC_IND ==> */
/* dg */                            /* LREC_IND durch SUB-descriptor ersetzen */
/* dg */                            DEC_REFCNT(desc);
/* dg */                            INC_REFCNT(ptdesc);
/* dg */                            WRITESTACK(S_a,ptdesc);
/* dg */                          }
/* dg */                          /* hat die funktion rel. freie variablen? */
/* dg */                          if (ownargs < subargs) {
/* dg */                            /* die funktion hat rel. freie variablen */
/* dg */                            if (narg == 0) {
/* dg */                              /* totale anwendung */
/* dg */                              PUSHSTACK(S_a,SET_ARITY(SNAP,subargs-ownargs+1));
/* dg */                              arity -= (subargs-ownargs);
/* dg */                            }
/* dg */                            /* sonst partielle anwendung, also nur snap's */
/* dg */                          }
/* dg */                          /* die funktion hat keine relativ freien variablen */
/* dg */                        }
/* dg */                        else narg = 0; /* um snaps zu verhindern */
/* dg */                      } /* end TY_COMB */
/* dg */                      else {
/* dg */                        /* alle anderen pointer */
/* dg */                        INC_REFCNT(desc);
/* dg */                        PUSHSTACK(S_a,desc);
/* dg */                        if (R_DESC((*desc),type) == TY_CONDI) {
/* dg */                          if ((arity > 1) || (narg > 0)) {
/* dg */                            /* snapargs dabei, wie viele? */
/* dg */                            arity += (narg > 0) ? 1 : 0;
/* dg */                            PUSHSTACK(S_a,SET_ARITY(SNAP,arity));
/* dg */                            arity = (narg == 0) ? 1 : 0;
/* dg */                          }
/* dg */                        }
/* dg */                        else
/* dg */                        if (R_DESC((*desc),type) == TY_CASE) {
/* dg */                          /* NCASE: SWITCH stelligkeit beachten! */

/* dg */                          narg = R_CASE((*desc),nargs) - R_SWITCH(*R_CASE((*desc),ptd),case_type);
/* dg */                          if (narg > 0) {
/* dg */                            /* snapargs dabei */
/* dg */                            PUSHSTACK(S_a,SET_ARITY(SNAP,narg+1));
/* dg */                            arity -= narg;
#ifdef NotUsed
/* dg */                          if (arity > R_SWITCH(*R_CASE((*desc),ptd),case_type) || narg > 0) { */
/* dg */                            /* snapargs dabei, wie viele? */
/* dg */                            arity -= (narg > 0) ? 0 :  R_SWITCH(*R_CASE((*desc),ptd),case_type);
/* dg */                            PUSHSTACK(S_a,SET_ARITY(SNAP,arity+1));
/* dg */                            arity = (narg > 0) ? 0 : R_SWITCH(*R_CASE((*desc),ptd),case_type);
#endif /* NotUsed  auskommentiert RS 3.12.1992 */
/* dg */                          }
/* dg */                        }
/* dg */                        narg = 0;
/* dg */                      }
/* dg */                    }
/* dg */                    else {
/* dg */                      /* stackelement */
/* dg */                      PUSHSTACK(S_a,desc);
/* dg */                      narg = 0;
/* dg */                    }
/* dg */                    /* applikatoren erzeugen */
/* dg */                    if (arity > 0) {
/* dg */                      if (narg > 0) {
/* dg */                        /* partielle fkt.anwendung, also snaps */
/* dg */                        PUSHSTACK(S_a,SET_ARITY(SNAP,arity+1));
/* dg */                      }
/* dg */                      else {
/* dg */                        /* sonst aps */
/* dg */                        PUSHSTACK(S_a,SET_ARITY(AP,arity+1));
/* dg */                      }
/* dg */                    }
/* dg */                    DEC_REFCNT((T_PTD)x);    /* freigabe der closure */
/* dg */                    trav_a_e();              /* auf e traversieren   */
/* dg */                    goto main_e;
/* dg */      case TY_CONDI:
#ifdef NotUsed
/* dg */                    PUSHSTACK(S_a,x);
/* dg */                    { extern int Rout_Mkcnd[];
/* dg */                      inter(Rout_Mkcnd);
/* dg */                    }
/* dg */                    trav_a_e();              /* auf e traversieren   */
/* dg */                    /* evtl. vorh. snappl. entfernen */
/* dg */                    if (T_SNAP(READSTACK(S_m))) {
/* dg */                      for (arity = ARITY(READSTACK(S_m1)) ; --arity > 0 ; ) {
/* dg */                        desc = (T_PTD)POPSTACK(S_a);
/* dg */                        if (T_POINTER((int)desc))
/* dg */                          DEC_REFCNT(desc);
/* dg */                      }
/* dg */                      PPOPSTACK(S_m);
/* dg */                      PPOPSTACK(S_m1);
/* dg */                    }
/* dg */                    goto main_e;
#endif /* NotUsed      auskommentiert RS 3.12.1992 */
/* dg */                    desc = R_CONDI((*(T_PTD)x),ptd);
/* dg */                    /* desc zeigt jetzt auf einen COND descriptor  */
/* dg */                    INC_REFCNT(desc);
/* dg */                    PUSHSTACK(S_e,desc);
/* dg */                    DEC_REFCNT((T_PTD)x);
/* dg */                    goto main_e;
#if UH_ZF_PM
/*uh, 26.12.90*/  /* --------------------------- List comprehensions --------------------------*/
/*uh, 26.12.90*/        case TY_ZFCODE:    /* nix */
/*uh, 26.12.90*/                           post_mortem("Cannot retranslate ZFcode-Descriptor!");
/*uh, 26.12.90*/                           break;
/*uh, 26.12.90*/
/*uh, 26.12.90*/        case TY_ZF:        if ( R_ZF(*(T_PTD)x,special)==ZF_START) { /* ZF-Expr Start */
/*uh, 26.12.90*/                             PUSHSTACK(S_m,zfbound);
/*uh, 26.12.90*/                             PUSHSTACK(S_m,SET_ARITY(ZF_UH,1));
/*uh, 26.12.90*/                             PUSHSTACK(S_m1,SET_ARITY(ZF_UH,0));
/*uh, 26.12.90*/                           PUSHSTACK(S_i,KLAA); /* uh 04Okt91 */
/*uh, 26.12.90*/                             for (i=0;i<R_ZF(*(T_PTD)x,nargs);i++) {
/*uh, 26.12.90*/                               /* PUSHSTACK(S_i,R_ZF(*(T_PTD)x,namelist)[i+1]); */
/*uh, 26.12.90*/                               y=MIDSTACK(S_a,i);
/*uh, 26.12.90*/                               PUSHSTACK(S_i,y);
/*uh, 26.12.90*/                             }
/*uh, 26.12.90*/                             load_expr((STACKELEM **)A_ZF(*(T_PTD)x,pte));
/*uh, 26.12.90*/                             goto main_e;
/*uh, 26.12.90*/                           }
/*uh, 26.12.90*/                           if ( R_ZF(*(T_PTD)x,namelist)==0) { /* zf-filter */
/*uh, 26.12.90*/                             PUSHSTACK(S_m,SET_ARITY(ZFINT_UH,1));
/*uh, 26.12.90*/                             PUSHSTACK(S_m1,SET_ARITY(ZFINT_UH,1));
/*uh, 26.12.90*/                             load_expr((STACKELEM **)A_ZF(*(T_PTD)x,pte));
/*uh, 26.12.90*/                             goto main_e;
/*uh, 26.12.90*/                           }
/*uh, 26.12.90*/                           /* Generator */
/*uh, 26.12.90*/                           { T_PTD zfvar=(T_PTD)R_ZF(*(T_PTD)x,namelist)[1];
/*uh, 26.12.90*/                           PUSHSTACK(S_m,zfvar); /* ZF-Variable */
/*uh, 26.12.90*/                           INC_REFCNT(zfvar); /* ZF-Variable */
/*uh, 26.12.90*/                           zfbound++;
/*uh, 26.12.90*/
/*uh, 26.12.90*/                           PUSHSTACK(S_m,SET_ARITY(ZFIN_UH,1));
/*uh, 26.12.90*/                           PUSHSTACK(S_m1,SET_ARITY(ZFIN_UH,1));
/*uh, 26.12.90*/
/*uh, 26.12.90*/                           PUSHSTACK(S_a,zfvar); /* ZF-Variable */
/*uh, 26.12.90*/                           INC_REFCNT(zfvar); /* ZF-Variable */
/*uh, 26.12.90*/                           PUSHSTACK(S_a,SET_ARITY(ZFIN_UH,2));
/*uh, 26.12.90*/
/*uh, 26.12.90*/                           i=0;
/*uh, 26.12.90*/                           while (!T_KLAA(MIDSTACK(S_i,i))) {
/*uh, 26.12.90*/                              if (T_NUM(y=MIDSTACK(S_i,i))) {
/*uh, 26.12.90*/                                 UPDATESTACK(S_i,i,INC(y));
/*uh, 26.12.90*/                              }
/*uh, 26.12.90*/                             i++;
/*uh, 26.12.90*/                           }
/*uh, 26.12.90*/                           PUSHSTACK(S_i,SET_ARITY(NUM,0));
/*uh, 26.12.90*/
/*uh, 26.12.90*/                           /* restausdruck reinziehen */
/*uh, 26.12.90*/                           load_expr((STACKELEM **)A_ZF(*(T_PTD)x,pte));
/*uh, 26.12.90*/                           goto main_e;
/*uh, 26.12.90*/                           }
/*uh, 26.12.90*/
/*uh, 26.12.90*/  /* ------------------------- Ende List comprehensions -----------------------*/
/*uh, 26.12.90*/
#endif /* UH_PM */
      case TY_VAR:       {
/* ak */                 int unprotects; /* Anzahl der zu entfernenden Schutzstriche */
/* ak */                                 /* fuer relativ freie Variable.             */

/* ak */                 unprotects = R_VAR(*((PTR_DESCRIPTOR)x), nlabar); 

                         ret_var(x);                 /* ganz freie Variable */
                       
/* ak */                 for (; unprotects < 0; unprotects++) /* UNPROTECTS muessen vor den */
/* ak */                                                      /* PROTECTS gepushed werden! */
/* ak */                 {
/* ak */                    PUSHSTACK (S_e, SET_ARITY(UNPROTECT,1));
/* ak */                 }

                         set_protect(NULL,
                                     (PTR_DESCRIPTOR)READSTACK(S_e));
                         goto main_e;                /* durch Namenspointer */
                                                     /* und PROTECT's ersetzen */
                         }
      case TY_LREC_IND:
#if DORSY
                        {
                         STACKELEM fx,xm,xm1,z;
                         PTR_DESCRIPTOR px,v,w;
                         int k,args,fd;

                         fd = 0;
                         px = (PTR_DESCRIPTOR)x;
                         fx = x;
                         w = (PTR_DESCRIPTOR)R_LREC_IND((*px),ptdd);
                         k = R_LREC_IND((*px),index);
                         v =
                            (PTR_DESCRIPTOR)R_LREC((*w),namelist)[k+1];
                         INC_REFCNT(v);
                         PUSHSTACK(S_e,(STACKELEM)v);
                         DEC_REFCNT(px);          /* plrec_ind */
PUSHSTACK(S_v,KLAA);
while ( (!fd) && (!(T_KLAA(x = POPSTACK(S_hilf)))) ) {
   if (R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_LREC)
      fd = (w == (PTR_DESCRIPTOR)x);        /* fd == 1 : Abbruch */
   else {
      px = (PTR_DESCRIPTOR)x;
      if (R_DESC((*px),type) == TY_LREC_ARGS) {
         if (w == (PTR_DESCRIPTOR)R_LREC_ARGS((*px),ptdd))
         {                 /* vergleiche args von px und w (auf S_a) */
            if (T_SNAP(READSTACK(S_m))) {
               args = R_LREC_ARGS((*px),nargs);
               j = 0;
               while (VALUE(R_LREC_ARGS((*px),ptdv)[j])
                      == (args - (j+1)))
                  j++;
               if (j == args)
                  fd = 1;             /* Uebereinstimmung */
               else {
                  PUSHSTACK(S_e,KLAA);
                  i = 0;
                  while ( (i < args)
                        && ( ((y = POPSTACK(S_a))
                             == (z=
                                (STACKELEM)R_LREC_ARGS((*px),ptdv)[i]))
                           || ((T_NUM(y)) && (T_NUM(z)))
                          /* || (VALUE(y) == VALUE(z)) */ ) ) {
                     i++;
                     PUSHSTACK(S_e,y);
                  }
                  if (i != args)
                  {
                     fd = 0;    /* kein Uebereinstimmung in den args */
                     PUSHSTACK(S_a,y);  /* ptdv[i] != y */
                  }
                  else    /* i == args */
                     fd = 1;       /* Uebereinstimmung in den args */
                  while (!(T_KLAA(y = POPSTACK(S_e))))
                     PUSHSTACK(S_a,y);
               }
            }
            else {
               post_mortem("ea_retrieve:plrec_ind without SNAP-args");
            }
         }
         else       /* w != px */
            fd = 0;
      }    /* end type == LREC_ARGS */
      else {
         post_mortem("ea_retrieve: unexpected lrec-type");
      }
   }    /* end else type != TY_LREC */
   PUSHSTACK(S_v,x);
}
                         if (T_KLAA(x))
                            PUSHSTACK(S_hilf,x);     /* @ */
                         /* else ;  auskommentiert RS 14.6.1993 */
                         while (!(T_KLAA(x = POPSTACK(S_v))))
                            PUSHSTACK(S_hilf,x);
                         if (!fd) {         /* restauriere func_def */
                            /* set_protect(NULL,v); */
                            /* hier werden keine lambda-bars eingesetzt, */
                            /* da es sich bei "v" um den neuen           */
                            /* Startausdruck handelt !!                  */
                                   /* setze lambda-bars fuer func_name ein */
                                   /* plrec = NULL, da @ptdd noch nicht    */
                                   /* auf S_hilf existiert                 */
                            PUSHSTACK(S_e,(STACKELEM)w);
                            INC_REFCNT(w);
                            if (T_SNAP(READSTACK(S_m))) {
                               xm = POPSTACK(S_m);
                               xm1 = POPSTACK(S_m1);
                               PUSHSTACK(S_m,SET_ARITY(LREC,2));
                               PUSHSTACK(S_m1,SET_ARITY(LREC,2));
                                            /* fuer "snarity" */
                                   /* wird bei TY_LREC wiederholt ! */
                               PUSHSTACK(S_m,xm);
                               PUSHSTACK(S_m1,xm1);
                            }
                            else {
                               PUSHSTACK(S_m,SET_ARITY(LREC,2));
                               PUSHSTACK(S_m1,SET_ARITY(LREC,0));
                                            /* fuer "snarity" */
                            }
                            PUSHSTACK(S_a,fx);
                            PUSHSTACK(S_m,SET_ARITY(PRELIST,0));
                         }
                         else {    /* (fd) */
                            set_protect(w,v);
                                   /* setze lambda-bars fuer func_name ein */
                                   /* @ptdd existiert auf S_hilf           */
                            if (T_SNAP(READSTACK(S_m))) {
                                  /* entferne SNAP und args */
                               PPOPSTACK(S_m);  /* TB, 30.10.92 */   /* SNAP */
                               for (i =
                                   ARITY(POPSTACK(S_m1))-1; i > 0; i--)
                                  PPOPSTACK(S_a); /* TB, 30.10.92 */ /* args */
                            }
                            /* else ;  auskommentiert RS 14.6.1993 */
                         }
                         goto main_e;
                                  /* weiter mit plrec oder func_name */
                       }
#else
                        {
                         PTR_DESCRIPTOR px,v,w;
                         int k;

                         px = (PTR_DESCRIPTOR)x;
                         w = (PTR_DESCRIPTOR)R_LREC_IND((*px),ptdd);
                         k = R_LREC_IND((*px),index);
                        v = (PTR_DESCRIPTOR)R_LREC((*w),namelist)[k+1];
                         INC_REFCNT(v);
                         DEC_REFCNT(px);
                         PUSHSTACK(S_e,(STACKELEM)v);
                         if (lrec)    /* Funktionsdefinitionen */
                            set_protect(w,v);
                                 /* im "neuen" Startausdruck gibt es */
                                 /* hier keine lambda-bars (es gibt  */
                                 /* keine "aeussereren" Funktionen); */
                                 /* sie treten hoechstens in den     */
                                 /* Funktionsdefinitionen auf        */
                                 /* !!! ist nur korrekt, falls Funk- */
                                 /* tionsgleichungen rekonstruiert   */
                                 /* werden, sonst koennen auch in    */
                                 /* Startausdruecken func's stehen ! */
                         goto main_e;
                       }
#endif
      case TY_LREC:    {
                         int k;
#if DORSY
                         STACKELEM xp,fx=(STACKELEM)0; /* Initialisierung von TB, 6.11.1992 */
                         if (T_PRELIST(READSTACK(S_m))) {
                            xp = POPSTACK(S_m);
                            fx = POPSTACK(S_a);
                         }
                         else
                            xp = NULL;
#endif
                         if (T_SNAP(READSTACK(S_m)))
                         {         /* alle SNAP-args liegen auf A   */
                            k = (ARITY(READSTACK(S_m1)) - 1);
                            PUSHSTACK(S_e,x);        /* plrec */
                            INC_REFCNT((PTR_DESCRIPTOR)x);
                            lrec_args(0,0);          /* => plrec_args auf S_e */
                                 /* ohne Substitution (ist schon ausgefuehrt) */
                            xd = POPSTACK(S_e);
                            PUSHSTACK(S_hilf,xd);
                                                /* plrec_args fuer PART_SUB() */
                            INC_REFCNT((PTR_DESCRIPTOR)xd);
                            WRITE_ARITY(S_m1,k);
                               /* LREC(ARITY(SNAP)-1) fuer "snarity" */
                         }
                         else
                         {
                            PUSHSTACK(S_hilf,x);         /* plrec */
                            INC_REFCNT((PTR_DESCRIPTOR)x);
                            WRITE_ARITY(S_m1,0);
                                  /* LREC(0) fuer "snarity" */
                         }
#if DORSY
                         if (T_PRELIST(xp)) {
                            PUSHSTACK(S_m,xp);
                            PUSHSTACK(S_a,x);
                            PUSHSTACK(S_e,fx);
                            tab = lfun_def();
                                            /* loesche ueberfluessige*/
                                            /* func_def's            */
                            t_exists = 1;
                            PPOPSTACK(S_a); /* TB, 30.10.92 */
                         }         /* xp == NULL */
                         else {
                            PUSHSTACK(S_a,x);
                            tab = lfun_def();
                                            /* loesche ueberfluessige*/
                                            /* func_def's            */
                            t_exists = 1;
                            PPOPSTACK(S_a); /* TB, 30.10.92 */
                         }
                         first = earet_lrec(first,x,tab);
#else
                         first = earet_lrec(first,x);
#endif
                      /* lrec = 1; */
                         goto main_e;
                       }
      case TY_LREC_ARGS: {
                            PTR_DESCRIPTOR w;
                            int args;

                            w = (PTR_DESCRIPTOR)x;
                            args = R_LREC_ARGS((*w),nargs);
                            xy = (STACKELEM)R_LREC_ARGS((*w),ptdd);
                            PUSHSTACK(S_e,xy);
                            INC_REFCNT((PTR_DESCRIPTOR)xy);
                            for (i = 0; i < args; i++)
                            {
                               xy =
                                 (STACKELEM)R_LREC_ARGS((*w),ptdv)[i];
                               PUSHSTACK(S_e,xy);
                               if (T_POINTER(xy))
                                  INC_REFCNT((PTR_DESCRIPTOR)xy);
                            }
                            PUSHSTACK(S_e,SET_ARITY(SNAP,args + 1));
                                   /* plrec_args wird aufgeloest, um */
                                   /* Ersetzungen in den args aus-   */
                                   /* fuehren zu koennen.            */
                            goto main_e;
                         }
      case TY_SUB:       desc = (PTR_DESCRIPTOR)x;
                         narg = R_FUNC((*desc),nargs);
                         PUSH_I_BLOCK(narg);         /* passen narg El. auf I? */
                      if ((snarity != 0) && (T_LIST(READSTACK(S_m)))) {
                         STACKELEM z,lz;
                         z = POPSTACK(S_m);
                         if ((T_LREC(lz = READSTACK(S_m)))
                            && ((ARITY(lz) > 1)
                               || (ARITY(lz) == -1)
                                  || (ARITY(lz) == -2)))
                            lpa = 1;
                         else
                            lpa = 0;
                         PUSHSTACK(S_m,z);
                      }
                      else
                         lpa = 0;
                      if (lpa)
                         rest = lpa_sub(narg,snarity);
                      else
                         rest = part_sub(narg);      /* I.block aufbauen    */
                         if (rest > 0)
                         {                           /* Funktion wird nicht */
                            INC_REFCNT(desc);        /* aufgeloest, weil    */
                            PUSHSTACK(S_a,x);        /* Variablen ueber bleiben */
                            PUSHSTACK(S_m,SET_ARITY(PRELIST,rest));
                         }
                         PUSHSTACK(S_m,DOLLAR);      /* Markierung fuer I.block */
                         ret_func(desc);             /* Funktionsrumpf herein- */
                                                     /* holen */
#if DORSY
#else
                         if (func)
                            if (T_LREC(READSTACK(S_e)))
                               f_lrec = 1;
                                   /* geschachtelte func_defs          */
                                   /* !!! ist nur korrekt, falls Funk- */
                                   /* tionsgleichungen rekonstruiert   */
                                   /* werden, sonst koennen auch in    */
                                   /* Startausdruecken func's stehen ! */
                            else
                            {
                               func = 0;
                               f_lrec = 0;
                            }
                         else
                            f_lrec = 0;
#endif
                         goto main_e;

      case TY_REC:       desc = (PTR_DESCRIPTOR)x;
                         narg = R_FUNC((*desc),nargs);
                         PUSH_I_BLOCK(narg + 1);     /* Platz fuer narg+1 El. auf I ? */
                         rest = part_rec(narg , desc);/* I.block aufbauen   */
                         if (rest > 0)               /* Funktion wird nicht */
                         {                           /* aufgeloest, weil    */
                            INC_REFCNT(desc);        /* Variablen ueber sind */
                            PUSHSTACK(S_a,x);
                            PUSHSTACK(S_m,SET_ARITY(PRELIST,rest));
                         }                       /* verbleib. Variablen in PRELIST notieren */
                         else                    /* Spezialfall:               */
                            if (rest == -1)      /* Rekursion hatte von Anfang */
                            {                    /* an kein Argument, bleibt   */
                               INC_REFCNT(desc); /* aber erhalten              */
                               PUSHSTACK(S_a,x); /* wie z.B  rec f f           */
                               PUSHSTACK(S_m,SET_ARITY(PRELIST,0));
                            }
                         PUSHSTACK(S_m,DOLLAR);  /* Markierung fuer I.block */
                         ret_func(desc);         /* Funktionsrumpf hereinholen */
                         goto main_e;
      case TY_COND:      if T_SNAP(READSTACK(S_m))
                         {   /* ein geschlossenes conditional -> Substitution */
                           narg = ARITY(READSTACK(S_m1)) - 1; /* Anzahl Argumente */
                           PUSH_I_BLOCK(narg);   /* passen narg El. auf I? */
                        if ((snarity !=0) && (T_LIST(READSTACK(S_m)))){
                           STACKELEM z,lz;
                           z = POPSTACK(S_m);
                           if ((T_LREC(lz = READSTACK(S_m)))
                              && ((ARITY(lz) > 1)
                                 || (ARITY(lz) == -1)
                                    || (ARITY(lz) == -2)))
                              lpa = 1;
                           else
                              lpa = 0;
                           PUSHSTACK(S_m,z);
                        }
                        else
                           lpa = 0;
                        if (lpa)
                           lpa_sub(narg,snarity);
                        else
                           part_sub(narg);       /* I.block fuellen */
                           PUSHSTACK(S_m,DOLLAR); /* Markierung fuer I.block */
                         }
                         ret_cond(x);       /* Conditional einlagern */
                         goto main_e;
      case TY_EXPR:     
            /*geschlossene Expressionpointer sind wegen Schliessen   */
            /*von Guard und Rumpfausdruecken bei den Eager Evaluation*/
            /*moeglich                                               */
            if (T_SNAP(READSTACK(S_m)))
            {
                narg = ARITY(READSTACK(S_m1)) -1;
                PUSH_I_BLOCK(narg);
                part_sub(narg);
                PUSHSTACK(S_m, DOLLAR);
             }
             
             ret_expr(x);
             
#if DORSY
#else
                         if (func)
                            if (T_LREC(READSTACK(S_e)))
                               f_lrec = 1;
                                          /* geschachtelte func_defs */
                            else
                            {
                               func = 0;
                               f_lrec = 0;
                            }
                         else
                            f_lrec = 0;
#endif
                         goto main_e;
      case TY_STRING :
      case TY_UNDEF  :   switch(class1 = R_DESC((*((PTR_DESCRIPTOR)x)),class)) {
                           case C_LIST    :   /* Liste einlagern  incl. <>  */
                                            ret_list(x);
                                            goto main_e;
                           case C_MATRIX  :        /* nilmat   bzw.         */
                           case C_TVECTOR :        /* niltvect bzw.         */
                           case C_VECTOR  :        /* nilvect  einlagern    */
                                            ret_mvt(x,class1);
                                            goto main_e;
                          }

      case TY_SLOT:    ret_slot(x);  /* cr 14.11.95 */
                       goto main_e;
      case TY_FRAME:   ret_frame(x);  /* cr 14.11.95 */
                       goto main_e;
      case TY_INTACT:  ret_intact(x);     /* Interaktion einlagern */
                       goto main_e;       /* stt 11.09.95 */


            case TY_SWITCH:
                 if T_SNAP(READSTACK (S_m))/* switch, das         */
                                              /*im Processing        */
                                          /* geschlossen wurde   */
                 {
                     narg = ARITY (READSTACK (S_m1)) - 1;
                     PUSH_I_BLOCK (narg);
                     part_sub (narg);/* g e h t  d a s  s o  ??? */
                     PUSHSTACK (S_m, DOLLAR);/* Markierung fuer I.block */
                 }
                 pm_ret_switch (x);/* Switch einlagern */
                 goto main_e;



            case TY_MATCH:
                 pm_ret_match (x);
                 goto main_e;

            case TY_NOMAT:
                 if T_SNAP (READSTACK (S_m))
                 {
                     narg = ARITY (READSTACK (S_m1)) - 1;
                     PUSH_I_BLOCK (narg);
                     part_sub (narg);/* g e h t  d a s  s o  ??? */
                     PUSHSTACK (S_m, DOLLAR);/* Markierung fuer I.block */
                 }
                 pm_ret_nomatch (x);
                 goto main_e;
      default:           PUSHSTACK(S_a,x);      /* Datenpointer, wird erst in */
                         goto main_m;           /* ae-retrieve retrieved      */
    }
  }
  if (T_NUM(x))                        /* Substitution (der nachgehaltenen */
  {                                    /* Bindung) */
     READ_I(ARITY(x),y);
     if (T_NUM(y))
     {
        PUSHSTACK(S_a,y);          /* Nummernvar auf A-Stack, um eine     */
        goto main_m;               /* weitere Substitution zu verhindern. */
     }
     else
     {
        PUSHSTACK(S_e,y);          /* Sonst alles auf E, weil es evtl. noch */
        goto main_e;               /* verarbeitet werden muss */
     }
  }

  if (T_ATOM(x))                   /* sonstiges Atom traversieren */
  {
    PUSHSTACK(S_a,x);
    goto main_m;
  }
  post_mortem("ea_retrieve: No match successful on main_e");

main_m:
  x = READSTACK(S_m);

  if (T_LREC(x) && ARITY(x) == 2)
  {
     WRITESTACK(S_m,DEC(x));
     f_lrec = 0;    /* goal (u.U. in func_def) */
     goto main_e;
  }
  else
  if (T_LREC(x) && ARITY(x) == 1)
  {
     PPOPSTACK(S_m1);
     PUSHSTACK(S_a,SET_ARITY(POPSTACK(S_m),3));
#if DORSY
#else
     if ((T_PRELIST(READSTACK(S_m))) && (ARITY(READSTACK(S_m)) == -1))
     {
        POPSTACK(S_m);
        f_lrec = 1;       /* weiter func_def */
     }
     else ;
#endif
     if (T_KLAA(xd = POPSTACK(S_hilf))) {
        post_mortem("ea_retrieve: Read on empty stack hilf");
     }
     else
        DEC_REFCNT((PTR_DESCRIPTOR)xd);
     goto main_m;
  }
  else
#if DORSY
#else
  if ((T_LREC(x)) && (ARITY(x) == -2))
                    /* LREC(-2) muss hier entfernt werden, da die      */
                    /* allgemeinen Traversieralgorithmen nicht fuer    */
                    /* Konstruktoren mit Stelligkeit -2 definiert sind */
  {
     PPOPSTACK(S_m);      /* LREC */
     PPOPSTACK(S_m1);     /* LREC */
     trav_a_e();          /* "Startausdruck" */
     PPOPSTACK(S_a);      /* p_lrec_args */
     trav_e_a();          /* "Startausdruck" */
     if (T_KLAA(xd = POPSTACK(S_hilf))) {
        post_mortem("ea_retrieve: Read on empty stack hilf");
     }
     else
        DEC_REFCNT((PTR_DESCRIPTOR)xd);
     goto main_m;
  }
  else
#endif

  if (T_PRELIST(x))             /* eine Funktion liegt jetzt vollstaendig    */
  {                             /* auf dem A-Stack und kann nun fertig       */
     pos_func(ARITY(x));        /* postprocessed werden, d.h. z.B Varia-     */
     PPOPSTACK(S_m);            /* blensubstitution. Die arity des PRELIST   */
     goto main_m;               /* enthaelt die Anzahl der uebrig bleibenden */
  }                             /* Variablen. pos_func() steuert die Umbe-   */
                                /* nennung der Variablen */

#if UH_ZF_PM
/*uh, 7.1.91*/
/*uh, 7.1.91*/    if (T_ZF_UH(x)) {
/*uh, 7.1.91*/      POPSTACK(S_m);
/*uh, 7.1.91*/      zfbound=POPSTACK(S_m);  /* Anzahl der gebundenen ZF-Variablen */
/*uh, 7.1.91*/      x =POPSTACK(S_m1);
/*uh, 7.1.91*/      PUSHSTACK(S_a,SET_ARITY(ZF_UH,ARITY(x)));
/*uh, 7.1.91*/      goto main_m;
/*uh, 7.1.91*/    }
/*uh, 7.1.91*/
/*uh, 7.1.91*/    if (T_ZFIN_UH(x)) {
/*uh, 7.1.91*/      int arity;
/*uh, 7.1.91*/      y = POPSTACK(S_m);   /* ZF-IN Konstruktor */
/*uh, 7.1.91*/      x = POPSTACK(S_m1);  /* akkumulierter ZF-Konstruktor */
/*uh, 7.1.91*/      arity = ARITY(x);  /* Anzahl der ZF-Komponenten hinter dem Generator */
/*uh, 7.1.91*/      y = POPSTACK(S_m);   /* zf-variable */
/*uh, 7.1.91*/      POPSTACK(S_m);       /* Applikator */
/*uh, 7.1.91*/      POPSTACK(S_m1);      /* Applikator */
/*uh, 7.1.91*/      WRITESTACK(S_m1,SET_ARITY(x,ARITY(x)+1));
/*uh, 7.1.91*/      for (i=0; i<arity; i++) {
/*uh, 7.1.91*/         TRAV_A_E;
/*uh, 7.1.91*/      }
/*uh, 7.1.91*/      PUSHSTACK(S_m,PRELIST);
/*uh, 7.1.91*/      for (i=0; i<arity; i++){
/*uh, 7.1.91*/        r1eapos(NUM,y);
/*uh, 7.1.91*/      }
/*uh, 7.1.91*/      POPSTACK(S_m);
/*uh, 7.1.91*/
/*uh, 7.1.91*/      /* ZF-Variablen entfernen */
/*uh, 7.1.91*/      POPSTACK(S_i);
/*uh, 7.1.91*/      i=0;
/*uh, 7.1.91*/      while (!T_KLAA(MIDSTACK(S_i,i))) {
/*uh, 7.1.91*/        if (T_NUM(y=MIDSTACK(S_i,i))) {
/*uh, 7.1.91*/             UPDATESTACK(S_i,i,DEC(y));
/*uh, 7.1.91*/                              }
/*uh, 7.1.91*/        i++;
/*uh, 7.1.91*/      }
/*uh, 7.1.91*/      goto main_m;
/*uh, 7.1.91*/    }
/*uh, 7.1.91*/
/*uh, 7.1.91*/    if(T_ZFINT_UH(x)) {
/*uh, 7.1.91*/      POPSTACK(S_m);   x = POPSTACK(S_m1);
/*uh, 7.1.91*/      POPSTACK(S_m);  /* Applikator */
/*uh, 7.1.91*/      POPSTACK(S_m1);
/*uh, 7.1.91*/      WRITESTACK(S_m1,SET_ARITY(x,ARITY(x)+1));
/*uh, 7.1.91*/      goto main_m;
/*uh, 7.1.91*/    }
/*uh, 7.1.91*/
/*uh, 7.1.91*/    if(T_PM_SWITCH(x)) { /* Bindungen des CASE aufloesen. */
/*uh, 7.1.91*/      if (ARITY(x)==0) {
/*uh, 7.1.91*/        int nvar;
/*uh, 7.1.91*/        int i;
/*uh, 7.1.91*/        POPSTACK(S_m); /* CASE */
/*uh, 7.1.91*/        nvar = POPSTACK(S_m);
/*uh, 7.1.91*/        for (i=0; i<nvar; i++) POPSTACK(S_i);
/*uh, 7.1.91*/        x=POPSTACK(S_m1);
/*uh, 7.1.91*/        PUSHSTACK(S_a,x);
/*uh, 7.1.91*/        goto main_m;
/*uh, 7.1.91*/      } else {
/*uh, 7.1.91*/        WRITESTACK(S_m,SET_ARITY(x,ARITY(x)-1));
/*uh, 7.1.91*/        goto main_e;
/*uh, 7.1.91*/      }
/*uh, 7.1.91*/    }
/*uh, 7.1.91*/
/*uh, 7.1.91*/    if (T_PM_WHEN(x)) { /* Bindungen der Klausel aufloesen, falls abgelaufen */
/*uh, 7.1.91*/      if (ARITY(x)==1) {
/*uh, 7.1.91*/        int npatvar;
/*uh, 7.1.91*/        int i;
/*uh, 7.1.91*/        POPSTACK(S_m); /* WHEN */
/*uh, 7.1.91*/        npatvar = POPSTACK(S_m);
/*uh, 7.1.91*/        for (i=0; i<npatvar; i++) POPSTACK(S_i);
/*uh, 7.1.91*/        x=POPSTACK(S_m1);
/*uh, 7.1.91*/        PUSHSTACK(S_a,x);
/*uh, 7.1.91*/        goto main_m;
/*uh, 7.1.91*/      } else {
/*uh, 7.1.91*/        WRITESTACK(S_m,SET_ARITY(x,ARITY(x)-1));
/*uh, 7.1.91*/        goto main_e;
/*uh, 7.1.91*/      }
/*uh, 7.1.91*/    }
/*uh, 7.1.91*/
/*uh, 7.1.91*/
#endif /* UH_ZF_PM */

  if (T_DOLLAR(x))              /* obersten Block des Inkarnationsstacks  */
  {                             /* entfernen */
     PPOPSTACK(S_m);            /* DOLLAR */
     POP_I_BLOCK(hilf);         /* Einen Block von I entfernen  */
     goto main_m;
  }
/* mah 071093 */
#if 0
  if (T_SNAP(x) && ARITY(x) == 1) 
#endif
  if (T_SNAP(x) && (ARITY(x) == 1))      /* Dieses SNAP  stammt von einer    */
     {                                   /* geschlossenen rekursiven Funktion*/
        PPOPSTACK(S_m);                  /* d.h. es muss entfernt werden inc.*/
        j = ARITY(POPSTACK(S_m1)) - 1;   /* der zugehoerigen Argumente       */
        x = READSTACK(S_a);                                            /* CS */
        if (T_CON(x))                                                  /* CS */
           trav_a_e();                                                 /* CS */
        else                                                           /* CS */
           PPOPSTACK(S_a); /* TB, 30.10.92 */                          /* CS */
        for (i = 1; i <= j; i++)
            if (T_CON(READSTACK(S_a)))                                 /* CS */
               ldel_expr(&S_a);                                        /* CS */
                 /* loescht einen Ausdruck von stack A;              *//* CS */
                 /* hier kamen vermutlich nur #-Variablen und        *//* CS */
                 /* pointer vor, die von S_i eingelagert werden;     *//* CS */
                 /* es werden auch die plrec_ind-SNAP-args entfernt, *//* CS */
                 /* da ein plrec_ind nur durch den Funktionsnamen    *//* CS */
                 /* ersetzt wird                                     *//* CS */
            else                                                       /* CS */
            {
               y = POPSTACK(S_a);
               if T_POINTER(y) DEC_REFCNT((PTR_DESCRIPTOR)y);
            }
        if (T_CON(x))                                                  /* CS */
           trav_e_a();                                                 /* CS */
        else                                                           /* CS */
           PUSHSTACK(S_a,x);
        goto main_m;
     }

  if (T_CON(x))
     if (ARITY(x) > 1)  /* dh die Arity des Konstruktors auf M war >= 2 */
     {
       WRITESTACK(S_m,DEC(x));            /* das uebliche */
#if DORSY
#else
       if (T_LIST(x))
       {
          STACKELEM u;

          u = POPSTACK(S_m);       /* LIST */

          if ((T_LREC(READSTACK(S_m)))
             && ( (ARITY(READSTACK(S_m)) > 1)
                || (ARITY(READSTACK(S_m)) == -1) ))
/*                || (ARITY(READSTACK(S_m)) == -2) ))  */
                /* Stelligkeit -2 ist uninteressant fuer Vars auf S_v, */
                /* da Variablen nur in den Funktionsdefinitionen       */
                /* rekonstruiert werden (push(S_v) in earet_lrec)      */
          {
             if (T_KLAA(xd = POPSTACK(S_v))) {
                post_mortem("ea_retrieve: Read on empty stack V");
             }
             else                           /* func_name */
                DEC_REFCNT((PTR_DESCRIPTOR)xd);
          }
          else ;
          PUSHSTACK(S_m,u);        /* LIST */
       }
       else ;
#endif
       goto main_e;
     }
     else
     {
       PPOPSTACK(S_m);
       PUSHSTACK(S_a,POPSTACK(S_m1));
#if DORSY
#else            /* !DORSY */
       if (T_LIST(x))
       {
          if ((T_LREC(READSTACK(S_m)))
             && (ARITY(READSTACK(S_m)) > 1))
          {
             if (T_KLAA(xd = POPSTACK(S_v))) {
                post_mortem("ea_retrieve: Read on empty stack V");
             }
             else                           /* func_name */
                DEC_REFCNT((PTR_DESCRIPTOR)xd);
             if (lrec)
             {
                xy = POPSTACK(S_m);
                if (T_KLAA(READSTACK(S_m)))
                {
                   f_lrec = 0;       /* Startausdruck */
                   lrec = 0;       /* Startausdruck */
                }
                PUSHSTACK(S_m,xy);
             }
          }
          else
          if ((T_LREC(READSTACK(S_m)))
             && (ARITY(READSTACK(S_m)) == -1)) {
             if (T_KLAA(xd = POPSTACK(S_v))) {
                post_mortem("ea_retrieve: Read on empty stack V");
             }
             else                           /* func_name */
                DEC_REFCNT((PTR_DESCRIPTOR)xd);
             PPOPSTACK(S_m);                 /* zusaetzliches LREC */
             PPOPSTACK(S_m1);                /* zusaetzliches LREC */
             if (T_KLAA(xd = POPSTACK(S_hilf))) {
                post_mortem("ea_retrieve: Read on empty stack hilf");
             }
             else                           /* p_lrec(_args) */
                DEC_REFCNT((PTR_DESCRIPTOR)xd);
             ari = (ARITY(POPSTACK(S_a)));                  /* LIST(ari) */
                                   /* func_defs vollstaendig traversiert */
             PUSHSTACK(S_m,(lis = POPSTACK(S_e)));           /* LIST */
                                   /* naechste Funktionsdefinitionen */
             PUSHSTACK(S_m1,SET_ARITY(lis,ARITY(lis) + ari));
                                  /* listenkonstruktor fuer alle(!) func_defs */
             goto main_e;
          }
          else ;
       }
       else ;
#endif           /* !DORSY */
       goto main_m;
     }

  if (T_KLAA(x))     /* @ bedeutet, ea_retrieve ist zuende! */
  {
     PPOPSTACK(S_hilf); /* TB, 30.10.92 */
#if DORSY
#else
     POPSTACK(S_v);
#endif
     ae_retrieve();  /* Vom A-Stack auf den E-Stack traversieren und restliche */
                     /* Pointer einlagern.                                     */
     PPOPSTACK(S_m);  /* KLAA */
#if DORSY
     if (t_exists)
        freeheap(tab);
     /* else ;  auskommentiert RS 14.6.1993 */
#endif
     END_MODUL("ea_retrieve");
     return;
  }
}

/*-----------------------------------------------------------------------------
 *  ae_retrieve  --  traversiert eine Ausdruck vom A-Stack auf den E-Stack.
 *                   Dabei werden die letzten verblieben Pointer fuer Daten-
 *                   Strukturen und Namen wieder eingelagert.
 *  ak:              UNPROTECTS werden durch Streichen von PROTECTS geloescht.
 *  globals    --    S_e,S_a     <w>
 *-----------------------------------------------------------------------------
 */
static void ae_retrieve()
{
  register STACKELEM  x;
  /*register unsigned short  arity;*/ /* TB, 30.10.92 */
    register    STACKELEM hilf;
    /*register int    bool;*/ /* TB, 30.10.92 */
    register int    i;
    /*int     level = 0;*/ /* TB, 30.10.92 */
  int first;
  char class1;                /* for 2nd parameter of function call ret_mvt */

  START_MODUL("ae_retrieve");

  first = 0;            /* Rekonstruktion der inneren p_lrec(_args) */
main_a:
  x = POPSTACK(S_a);

  if (T_CON(x))                             /* Konstruktor nur traversieren */
  {

#if UH_ZF_PM
/*uh, 7.1.91*/       if (T_ZFIN_UH(x)) {
/*uh, 7.1.91*/          x=POPSTACK(S_a);
/*uh, 7.1.91*/          PUSHSTACK(S_m,x);
/*uh, 7.1.91*/          PUSHSTACK(S_m,SET_ARITY(ZFIN_UH,2));
/*uh, 7.1.91*/          goto main_a;
/*uh, 7.1.91*/       }
/*uh, 7.1.91*/
/*uh, 7.1.91*/       if (T_ZF_UH(x)) {
/*uh, 7.1.91*/          trav(&S_a,&S_i);
/*uh, 7.1.91*/          PUSHSTACK(S_m,x);
/*uh, 7.1.91*/          PUSHSTACK(S_m1,x);
/*uh, 7.1.91*/          goto main_a;
/*uh, 7.1.91*/       }
/*uh, 7.1.91*/
#endif UH_ZF_PM

        if (T_PM_WHEN (x))         /* Beginn der Behandlung des */
                                /* Pattern Matching          */
        {

            /*body und guard nach e*/
            trav(&S_a, &S_e);/*body*/
            trav(&S_a, &S_e);/*guard*/
            if (T_SUB (READSTACK (S_a)))
        /* pattern beginnt mit einem Sub, enthaelt */
        /* also Variablen, wegpoppen des sub und der Variablen */

            {
                 i = ARITY (POPSTACK (S_a)) - 1; /*sub*/
                 trav (&S_a, &S_hilf);/*rumpf*/
                 for (; i > 0; i--)
                     {
                     hilf = POPSTACK(S_a);/*die Variablen*/
                     DEC_REFCNT ((PTR_DESCRIPTOR) hilf );
                      }
                 trav (&S_hilf, &S_a);/*rumpf zurueck*/

                 /*Behandlung des guard*/
                if(T_SA_TRUE(READSTACK(S_e)))
                                {
                                trav(&S_e, &S_a);
                                }
                else
                 {
                 i = ARITY (POPSTACK (S_e)) - 1;/*sub*/
                 for (; i > 0; i--)/* pop fuer die variablen */
                     {
                     hilf = POPSTACK(S_e);
                     DEC_REFCNT ((PTR_DESCRIPTOR) hilf );
                    }
                 trav(&S_e, &S_a);/*rumpf zurueck*/
                }

                 /*Behandlung des body*/
                 i = ARITY (POPSTACK (S_e)) - 1;/*sub*/
                 for (; i > 0; i--)/* pop fuer das sub und */
                     {
                     hilf = POPSTACK(S_e);
                     DEC_REFCNT ((PTR_DESCRIPTOR) hilf );
                    }
                 trav (&S_e, &S_a);/*rumpf zurueck*/

            }                      /* ende von Behandlung des pattern matching */
            else
            {/*alles Traversieren umsonst, Pattern enthaelt*/
             /*keine variablen                             */

            /*body und guard zurueck nach hilf*/
            trav(&S_e, &S_a);/*guard*/
            trav(&S_e, &S_a);/*body*/
            }
        }                          /* ende von T_PM_WHEN */

     if (ARITY(x) > 0)
     {
/* ak *//* Streichen von PROTECTS, die durch UNPROTECTS aufgehoben werden */
        /* Vor.: UNPROTECTS stehen auf dem A-Stack (vorher auf E-Stack)   */
        /*       "unter" den PROTECTS.                                    */
       if (T_UNPROTECT(x))
       {
         if (T_PROTECT(READSTACK (S_m)))
         {
           PPOPSTACK(S_m); /* TB, 30.10.92 */
           PPOPSTACK(S_m1); /* TB, 30.10.92 */
         }
         else /* sollte nie ausgefuehrt werden, */
              /* da es nie mehr UNPROTECT als PROTECT */
              /* Konstruktoren geben darf. */
         {
           PUSHSTACK(S_m,x);
           PUSHSTACK(S_m1,x);
         }
       }
       else
       if (T_PROTECT(x))
       { 
         if (T_UNPROTECT(READSTACK (S_m)))
         {
           PPOPSTACK(S_m); /* TB, 30.10.92 */
           PPOPSTACK(S_m1); /* TB, 30.10.92 */
         }
         else 
         {
           PUSHSTACK(S_m,x);
           PUSHSTACK(S_m1,x);
         }
       }
       else
/* ak *//* Ende des neueingefuegten Teils */
       {
         PUSHSTACK(S_m,x);
         PUSHSTACK(S_m1,x);
       }
       goto main_a;
     }
     else
     {
       PUSHSTACK(S_e,x);
       goto main_m;
     }
  }

  if (T_INT(x))					/* BM */
     {						/* BM */
      convi(VAL_INT(x)) ;			/* BM */
      goto main_m ;				/* BM */
     }						/* BM */

  if (T_POINTER(x))                         /* Pointer aufloesen! Es darf keiner */
    switch(class1 = R_DESC( (* ((PTR_DESCRIPTOR)x) ),class))/* uebrig bleiben */
    {
      case C_EXPRESSION:           /* z.Z. nur Namenspointer */
                         switch (R_DESC( (* ((PTR_DESCRIPTOR)x) ),type))
                         {
                           case TY_NAME: ret_name(x);    /* Namen einlagern auf E */
                                         goto main_m;
                           default:      post_mortem("ae_retrieve:Unexpected descriptortype");
                         }
                         break;
      case C_SCALAR:     ret_scal(x);       /* formatierte Zahl einlagern */
                         goto main_m;
      case C_DIGIT :     ret_dec(x);        /* Digitstring einlagern */
                         goto main_m;
      case C_MATRIX :                       /* Matrix  bzw.      */
      case C_VECTOR :                       /* Vektor  bzw.      */
      case C_TVECTOR:    ret_mvt(x,class1); /* TVektor einlagern */
                         goto main_m;
      default:           post_mortem("ae_retrieve: Unexpected descriptorclass");
    } /* Ende von T_POINTER */

  if (T_ATOM(x))            /* sonstiges Atom */
  {
    PUSHSTACK(S_e,x);
    goto main_m;
  }
  post_mortem("ae_create: No match successful on main_a");

main_m:
  x = READSTACK(S_m);

#if UH_ZF_PM
/*uh, 7.1.91*/    if (T_ZFIN_UH(x)) {
/*uh, 7.1.91*/       if (ARITY(x)>1) {
/*uh, 7.1.91*/         POPSTACK(S_m);
/*uh, 7.1.91*/         x = POPSTACK(S_m);
/*uh, 7.1.91*/         PUSHSTACK(S_a,x);   /* ZF-Variable */
/*uh, 7.1.91*/         PUSHSTACK(S_m,SET_ARITY(ZFIN_UH,1));
/*uh, 7.1.91*/         goto main_a;
/*uh, 7.1.91*/       }
/*uh, 7.1.91*/       POPSTACK(S_m);
/*uh, 7.1.91*/       PUSHSTACK(S_e,SET_ARITY(x,2));
/*uh, 7.1.91*/       goto main_m;
/*uh, 7.1.91*/    }
/*uh, 7.1.91*/ 
/*uh, 7.1.91*/    if (T_ZF_UH(x)) {
/*uh, 7.1.91*/      if (ARITY(x) > 2) {
/*uh, 7.1.91*/         WRITESTACK(S_m,DEC(x));
/*uh, 7.1.91*/         goto main_a;
/*uh, 7.1.91*/      }
/*uh, 7.1.91*/      if (ARITY(x) == 2) {
/*uh, 7.1.91*/         trav(&S_i,&S_a);
/*uh, 7.1.91*/         WRITESTACK(S_m,DEC(x));
/*uh, 7.1.91*/         goto main_a;
/*uh, 7.1.91*/      }
/*uh, 7.1.91*/      POPSTACK(S_m);
/*uh, 7.1.91*/      x = POPSTACK(S_m1);
/*uh, 7.1.91*/      PUSHSTACK(S_e,x);
/*uh, 7.1.91*/ 
/*uh, 7.1.91*/      /* uh 04Okt91 aufrauemen auf Stack I */
/*uh, 7.1.91*/      while (!T_KLAA(READSTACK(S_i))) {
/*uh, 7.1.91*/        POPSTACK(S_i);
/*uh, 7.1.91*/      }
/*uh, 7.1.91*/      POPSTACK(S_i); /* Klaa */
/*uh, 7.1.91*/     
/*uh, 7.1.91*/      goto main_m;
/*uh, 7.1.91*/    }
#endif /* UH_ZF_PM */

  if (T_CON(x))
#if DORSY
#else
     if ((T_LREC(x)) && (ARITY(x) == 2))
        {
           if (!first)
           {
              x = POPSTACK(S_m);
              if (T_KLAA(READSTACK(S_m)))
                 first = 1;               /* aeussere p_lrec(_args) */
              else
                 first = 0;               /* innere p_lrec(_args) */
              PUSHSTACK(S_m,x);
           }
           aeret_lrec(first);
           goto main_a;
        }
     else
     if ((T_LREC(x)) && (ARITY(x) == -2))
                                  /* LREC im Startausdruck */
     {                            /* entferne LREC(-2) und         */
                                  /* p_lrec(_args) pointer von S_a */
        PPOPSTACK(S_m);
        PPOPSTACK(S_m1);
        POPSTACK(S_a);
        goto main_m;
     }
     else
#endif
     if (ARITY(x) > 1)  /* dh die Arity des Konstruktors war >= 2 */
     {
       WRITESTACK(S_m,DEC(x));              /* das uebliche */
       goto main_a;
     }
     else
     {
       PPOPSTACK(S_m);
       /* --- dg 14.08.92 */
       if (T_PM_NCASE(x)) {
         /* change PM_NCASE to PM_SWITCH */
         PUSHSTACK(S_e,SET_ARITY(PM_SWITCH,ARITY(POPSTACK(S_m1))));
       }
       else
       if (T_PM_WHEN(x) && T_PM_NCASE(READSTACK(S_m))) {
         /* NCASE: multiple patterns */
         if (T_LIST(READSTACK(S_e))) {
           /* generate WHEN with increased arity */
           WRITESTACK(S_e,SET_ARITY(x,ARITY(POPSTACK(S_m1))+ARITY(READSTACK(S_e))-1));
         }
         else post_mortem("ae_retrieve: pattern list expected on stack e");
       }
       else
       /* --- dg 14.08.92 */
       PUSHSTACK(S_e,POPSTACK(S_m1));
       goto main_m;
     }

  if (T_KLAA(x))                            /* ae_retrieve ist zu beenden */
  {
     END_MODUL("ae_retrieve");
     return;
  }
  post_mortem("ae_retrieve main_m: no match");
}

/* End of file ea-retrieve */

