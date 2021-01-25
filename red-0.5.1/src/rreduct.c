/* $Log: rreduct.c,v $
 * Revision 1.2  1992/12/16  12:50:46  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*----------------------------------------------------------------------------
 * reduct -- steuert die drei Phasen der Reduktionsmaschine, und sorgt
 *           fuer ein evtl. verpointertes Wiederaufsetzen.
 *           Der zu reduzierende Ausdruck wird auf dem E-Stack erwartet.
 *           Der Ergebnisausdruck wird auf den E-Stack zurueckgeliefert.
 *           Es erfolgt fuer die einzelnen Phasen eine Zeitmessung, die an den
 *           Editor uebergeben wird.
 * globals --
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 * reduct:c external -- reduct
 * reduct:c internal -- none
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
#include "rinter.h"
#include "dbug.h"
#if D_DIST
#include "DBHexport.h"
#endif

/* #include "rnf.h" */
/* ak *//* rnf.h/rnf.c  :  Modul zur Berechnung einer Normalform */
        /* Importiert die Funktion reduce_to_nf ().              */
        /* Diese Funktion treibt die Reduktion rekursiv in den   */
        /* Rumpf von Abstraktionen.                              */

/* --- externe variablen und funktionen --- */

#if D_DIST
#include <sys/time.h>
#endif

#if D_MESS
extern getkey();
#endif

#if DEBUG
extern char TraceFile[];	/* rreduce.c */
extern FILE *TraceFp;		/* rreduce.c */
extern int showdisplay;		/* rruntime.c */
#endif
extern PTR_DESCRIPTOR _old_expr_p; /* kopie des zuletzt bearbeiteten ausdrucks */
extern void           ptrav_a_e();           /* trav:c */
extern STACKELEM      st_expr();             /* storepro:c */
extern void           ret_expr();            /* retrieve:c */
extern void           ea_create();           /* ea-create:c*/
extern void           ear();                 /* ear:c      */
extern void           ea_retrieve();         /* ea_ret:c   */
extern void           load_expr();
extern void           inter();               /* rinter.c */ /* dg */
extern T_CODEDESCR    codedesc;              /* rinter.h */ /* dg */
extern void           reduce_to_nf();        /* rnf.c */ /* ak */

#if D_DIST
extern void send_params();
extern void send_graph();
extern void receive_result();

extern int cube_dim;
#endif

/* ach 05/11/92 */
extern int init_print_heap();                /* dumpheap.c */
extern void loleila();                       /* rdummies.c */
extern void print_heap();                    /* dumpheap.c */
extern void load_heap();                     /* dumpheap.c */
extern void stack_error();                   /* rstack.c */
/* end of ach */

/* RS 5.11.1992 */
extern void setdisplay();  /* rruntime.c */
extern void monitor_call();  /* rruntime.c */ 
extern void DescDump();      /* rdesc.c */
extern STACKELEM *ppopstack();    /* rstack,c */
extern void test_inc_refcnt();    /* rheap.c */
extern void test_dec_refcnt();    /* rheap.c */
extern void res_heap();           /* rheap.c */
extern void rel_heap();           /* rheap.c */
/* END of RS 5.11.1992 */

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
extern void dynamic_gc();                  /* rscavenge.c */
/* END of RS 6.11.1992 */ 

#if SCAV_DEBUG
extern void scav_write_log ();
#endif

/* --- interne variablen und funktionen --- */

       void           evaluate();   /* forward */   /* dg */
       PTR_DESCRIPTOR code_desc;                    /* dg */

double tvorher;                     /* zeitmessung */
double tpre;                        /* zeitmessung */
double tpro;                        /* zeitmessung */
double tpos;                        /* zeitmessung */
double tges;                        /* zeitmessung */

#if  SINTRAN3
extern double tused();
#define TIMER() tused()
#else
extern long clock();
#define TIMER() (clock() / 1000000.0)
#endif

/* dg */ /*------------------------------------------------------------------*/
/* dg */ /* reduct                                                           */
/* dg */ /*------------------------------------------------------------------*/
/* dg */
/* dg */ void reduct()
/* dg */ {
/* dg */   /*STACKELEM  x;                       ach 05/11/92 */

#if D_DIST
  double zeit1 = 0.0, zeit2 = 0.0;
  struct timeval t0;
#endif

#if D_MESS
  double m_zeit1 = 0.0, m_zeit2 = 0.0;
#endif

/* dg */
/* dg */   START_MODUL("reduct");
/* dg */
#if DEBUG
/* dg */   showdisplay = TRUE;
/* dg */   setdisplay();         /* Stackbildaufbau */
/* dg */   monitor_call();       /* Benutzer kann interagieren */
           if ((TraceFp = fopen(TraceFile,"w")) == NULL)
             post_mortem("cannot open tracefile");
#endif

/* dg */   /* initialisierungen fuer die zeitmessung */
/* dg */   tvorher = tpre = tpro = tpos = tges = 0.0;
/* dg */   tvorher = TIMER();           /* tvorher eingefuegt: ach 05/11/92 */
/* dg */   tvorher = TIMER();
/* dg */
/* dg */   if (T_KLAA(READSTACK(S_e))) {
/* dg */     /*-------------------------------*/
/* dg */     /* verpointertes Wiederaufsetzen */
/* dg */     /*-------------------------------*/
/* dg */     if (_old_expr_p == NULL)
/* dg */       post_mortem("reduct: there's no preprocessed expression.");
/* dg */     WRITESTACK(S_e,(STACKELEM)_old_expr_p); /* KLAA ueberschreiben */
/* dg */     tpre = TIMER();
/* dg */     evaluate();        /* auswertung anstossen */
/* dg */     tpro = TIMER();
/* dg */   }
/* dg */   else {
/* dg */     /*---------------*/
/* dg */     /* Preprocessing */
/* dg */     /*---------------*/
/* dg */     _old_expr_p = NULL; /* kein alter Ausdruck gespeichert    */
/* dg */     ea_create();        /* Steuermodul der Preprocessingphase */
/* dg */     /* der preprocesste ausdruck liegt als EXPR-descriptor auf e */
/* dg */     ear();                                   /* compilieren (abstr.) */
/* dg */     /* der compilierte ausdruck liegt als COMB-descriptor auf e */
/* dg */     code_desc = (T_PTD)READSTACK(S_e);       /* codedescriptor */
#if SCAVENGE
/* tg */     dynamic_gc ();
#endif
/* dg */     init_print_heap();                       /* dg 04.03.91 HeapDumpFile,HeapBase,HeapAlign */
/* dg */     loleila(R_COMB(*code_desc,ptc),SYMBTAB); /* loleila compiler */
/* dg */     /*------------*/
/* dg */     /* Processing */
/* dg */     /*------------*/
/* dg */     print_heap(NULL);                        /* dg 25.01.91 NULL: name aus red.setup */
/* dg */     tpre = TIMER();

#if D_DIST
 gettimeofday (&t0, NULL);
 zeit1 =  t0.tv_sec + t0.tv_usec/1000000.0;
#endif

#if D_DIST

DBUG_PRINT ("rreduct", ("Zeit vorher: %f", zeit1));

#if DEBUG
 DBUG_PRINT ("rreduct", ("Jetzt wuerde ich gern den screen loeschen !"));

#endif
	     flush_queue();
	     send_params();
	     send_graph();

#if NCDEBUG
	     DBHinit(1<<cube_dim);
	     DBHrun();
#endif
	     receive_result();

#else
/* dg */     inter(R_COMB(*code_desc,ptc));           /* codeausfuehrung */
#endif

DBUG_PRINT ("rreduct", ("Hi, I'm back !"));

#if D_DIST
 gettimeofday (&t0, NULL);
 zeit2 =  t0.tv_sec + t0.tv_usec/1000000.0;

DBUG_PRINT ("rreduct", ("Zeit nachher: %f", zeit2));
DBUG_PRINT ("rreduct", ("Differenz: %4.2f", zeit2-zeit1));

#endif

#if D_MESS
             fflush (stdout);
             gettimeofday (&t0, NULL); 
             m_zeit1 = t0.tv_sec + t0.tv_usec/1000000.0; 
             clearscreen();
             printf (" merging measurement files ...\n");
             fflush (stdout);
#ifdef M_OLD_MERGING
             get_em_merged();   /*  old merging */
#else
#ifdef M_SUPER_MERGING
             m_super_merging();
#else
             m_merging();
#endif
#endif
#ifndef M_SUPER_MERGING
             m_remote_copy();
#endif

             gettimeofday (&t0, NULL); 
             m_zeit2 = t0.tv_sec + t0.tv_usec/1000000.0; 
             if (m_zeit1 == m_zeit2)
               DBUG_PRINT ("REDUCT", ("great time-measurement !"));
             else
               printf ("\n merging time: %4.2f s\n", m_zeit2-m_zeit1);
             printf (" press return ...\n");
             fflush (stdout);
             getkey();
#endif

/* dg */     tpro = TIMER();

#if SCAV_DEBUG
/* tg */     scav_write_log ();
#endif
/* dg */     PPOPSTACK(S_e);                          /* codedescriptor */
/* dg */     load_heap(NULL);                         /* dg 04.02.92 NULL: name aus red.setup */
/* dg */   }
/* dg */   _old_expr_p = (PTR_DESCRIPTOR)POPSTACK(S_a);
/* dg */   if (T_POINTER((int)_old_expr_p))
/* dg */     INC_REFCNT(_old_expr_p);
/* dg */   PUSHSTACK(S_e,_old_expr_p);
#if SCAVENGE
/* tg */   disable_scav();
#endif
/* dg */   /*----------------*/
/* dg */   /* Postprocessing */
/* dg */   /*----------------*/
#if RED_TO_NF
/* ak */    reduce_to_nf ();        /* Vorantreiben der Reduktion, Berechnung der Normalform  */
#endif
/* dg */   ea_retrieve();          /* Steuermodul der Postprocessing Phase */
/* dg */   tpos = TIMER();
/* dg */   tpos -= tpro;
/* dg */   tpro -= tpre;
/* dg */   tpre -= tvorher;

#if D_DIST
    tpro = zeit2-zeit1;
#endif

/* dg */   tges = tpre + tpro + tpos;
/* dg */ 
#if DEBUG
/* dg */   showdisplay = FALSE;    /* Stackbildmanipulation unterdruecken */
           if (TraceFp)
             fclose(TraceFp);
#endif
/* dg */   END_MODUL("reduct");
/* dg */ }

/* file eval.c */

#include <setjmp.h>

/* --- externe variablen/funktionen --- */

extern BOOLEAN _interrupt;    /* rreduct.c */
extern jmp_buf _interruptbuf; /* rreduct.c */
extern void    freeheap();    /* rheap.c */
extern int     Rout_Eval[];   /* rinter.c */

#define CHECK_EXPR(ptd) \
           (R_DESC(*ptd,class) == C_LIST && R_DESC(*ptd,type) == TY_UNDEF) || \
           (R_DESC(*ptd,type) == TY_CLOS) || \
           (R_DESC(*ptd,type) == TY_COMB && R_COMB(*ptd,nargs) == 0) || \
           (R_DESC(*ptd,type) == TY_EXPR) || \
           (R_DESC(*ptd,type) == TY_NOMAT)

/* -------------------------------------------------- */
/* evaluate: evaluiert den ausdruck auf stack e und   */
/*           bewegt ihn auf stack a                   */
/* -------------------------------------------------- */

void evaluate()
{
  register STACKELEM  x,y;
  register int        dim;
  register VOLATILE T_PTD      ptdesc=(T_PTD)0; /* Initialisierung von TB, 6.11.1992, volatile: ach 10/11/92 */

  START_MODUL("evaluate");

  /* interruptbehandlung neu initialisieren */
  setjmp(_interruptbuf);
  if (_interrupt) {                              /* interrupt aufgetreten? */
    _interrupt = FALSE;                          /* zuruecksetzen */
    post_mortem("evaluate: interrupt received"); /* alles abbrechen */
  }

  PUSHSTACK(S_m,KLAA);             /* bottomsymbol */

main_e:

  x = READSTACK(S_e);
  if (T_POINTER(x)) {
    ptdesc = (T_PTD)x;
    if (R_DESC(*ptdesc,type) == TY_COMB && R_COMB(*ptdesc,nargs) == 0) {
      /* nullstellige funktion (constant applicative form) */
      PUSHSTACK(S_a,x);          /* COMB */
      PPOPSTACK(S_e);            /* COMB */
      INC_REFCNT(ptdesc);        /* um freigabe durch inter zu verhindern */

DBUG_PRINT ("RREDUCT", ("Yups, ich bin hier !"));

      inter(Rout_Eval);          /* evaluieren */

      DEC_REFCNT(ptdesc);        /* freigabe */
      goto main_m;
    } /* end TY_COMB */
    else
    if (R_DESC(*ptdesc,type) == TY_CLOS) {
      /* (un)gesaettigte closure  */
      if (R_CLOS(*ptdesc,args) == 0) {
        /* 0-stellige closure (indirektionsknoten) */
        y = R_CLOS(*ptdesc,pta)[0]; /* dereferenzieren */
        if (T_POINTER(y))
          INC_REFCNT((T_PTD)y);
        PUSHSTACK(S_a,y);          /* und auf a! ablegen */
        PPOPSTACK(S_e);            /* closure */
        DEC_REFCNT(ptdesc);        /* freigeben */
        goto main_m;
      }
      else {
        /* keine 0-stellige closure */
        int count = 0;
        PUSHSTACK(S_a,x);          /* closure */
        WRITESTACK(S_e,DOLLAR);    /* trennsymbol ueber closure */
        /* testen ob die closure nicht ausgewertete ausdruecke enthaelt */
        /* ACHTUNG: hiermit wird die reihenfolge der auswertung geaendert !!! */
        /* for (dim = R_CLOS(*ptdesc,args) ; dim >= 0 ; dim--) { */
        for (dim = 0; dim <= (int) R_CLOS(*ptdesc,args) ; dim++) {
                        /* int gecastet von RS 04/11/92 */ 
          y = R_CLOS(*ptdesc,pta)[dim];
          if (T_POINTER(y)) {
            register T_PTD ptd = (T_PTD)y;
            if (CHECK_EXPR(ptd)) {
              INC_REFCNT(ptd);
              PUSHSTACK(S_e,ptd);
              PUSHSTACK(S_m,SET_ARITY(AP,dim));
              count++;
            }
          }
        } /* end for */
        goto main_e;
      }
    } /* end TY_CLOS */
    else
    if (R_DESC(*ptdesc,class) == C_LIST && R_DESC(*ptdesc,type) == TY_UNDEF) {
      /* liste zu behandeln */
      int count = 0;
      PUSHSTACK(S_a,x);                   /* list */
      WRITESTACK(S_e,HASH);               /* trennsymbol */
      /* testen ob die liste nicht ausgewertete ausdruecke enthaelt */
      for (dim = R_LIST(*ptdesc,dim); --dim >= 0 ;) {
        y = R_LIST(*ptdesc,ptdv)[dim];
        if (T_POINTER(y)) {
          register T_PTD ptd = (T_PTD)y;
          if (CHECK_EXPR(ptd)) {
            INC_REFCNT(ptd);
            PUSHSTACK(S_e,ptd);
            PUSHSTACK(S_m,SET_ARITY(LIST,dim));
            count++;
          }
        }
      } /* end for */
      if (count == 0) {
        PPOPSTACK(S_e);      /* HASH weg */
        goto main_m;
      }
      goto main_e;
    } /* end C_LIST */
    else
    if (R_DESC(*ptdesc,type) == TY_EXPR) {
      /* (nicht ausfuehrbare) applikation */
      int count = 0;
      PUSHSTACK(S_a,x);                   /* expr */
      WRITESTACK(S_e,HASH);               /* trennsymbol */
      /* testen ob die applikation nicht ausgewertete ausdruecke enthaelt */
      for (dim = R_EXPR(*ptdesc,pte)[0]; dim > 0 ; dim--) {
        y = R_EXPR(*ptdesc,pte)[dim];
        if (T_POINTER(y)) {
          register T_PTD ptd = (T_PTD)y;
          if (CHECK_EXPR(ptd)) {
            INC_REFCNT(ptd);
            PUSHSTACK(S_e,ptd);
            PUSHSTACK(S_m,SET_ARITY(SNAP,dim));
            count++;
          }
        }
      } /* end for */
      if (count == 0) {
        PPOPSTACK(S_e);      /* HASH weg */
        goto main_m;
      }
      goto main_e;
    } /* end TY_EXPR */
    else {
      /* sonstige faelle */
      PUSHSTACK(S_a,x);
      PPOPSTACK(S_e);
      goto main_m;
    }
  } /* end T_POINTER(x) */
  else
  if (T_DOLLAR(x)) {
    /* ende der behandlung einer closure */
    register PTR_HEAPELEM pth = R_CLOS(*ptdesc,pta);
    ptdesc = (T_PTD)READSTACK(S_a); /* closure */
    INC_REFCNT(ptdesc);             /* um freigabe durch inter zu verhindern */
    PUSHSTACK(S_a,ptdesc);          /* closure verdoppeln */

DBUG_PRINT ("RREDUCT", ("Ich bin hier unten !"));

    inter(Rout_Eval);               /* evaluieren */

    /* argumente der closure freigeben */
    RES_HEAP;
    for (dim = R_CLOS(*ptdesc,args) ; dim >= 0 ; dim--) {
      if (T_POINTER(pth[dim])) {
        DEC_REFCNT((T_PTD)pth[dim]);
      }
    }
    freeheap(R_CLOS(*ptdesc,pta));
    REL_HEAP;
    /* indirektionsknoten anlegen (CLOS mit args == 0) */
    L_CLOS(*ptdesc,args) = 0;
    y = POPSTACK(S_a);             /* ergebnis von eval */
    if (T_POINTER(y))
      INC_REFCNT((T_PTD)y);
    R_CLOS(*ptdesc,pta)[0] = y;     /* resultat eintragen */
    WRITESTACK(S_a,y);              /* ergebnis von eval ueber closure */
    DEC_REFCNT(ptdesc);             /* closure freigeben */
    PPOPSTACK(S_e);                 /* DOLLAR */
    goto main_m;
  }
  else
  if (T_HASH(x)) {
    /* ende der behandlung einer liste oder applikation */
    PPOPSTACK(S_e);                 /* HASH */
    goto main_m;
  }
  else {
    /* alle sonstigen faelle */
    PUSHSTACK(S_a,x);
    PPOPSTACK(S_e);
    goto main_m;
  }

main_m:

  x = READSTACK(S_m);
  if (T_AP(x)) {
    /* argument einer closure wurde evaluiert */
    y = POPSTACK(S_a);                            /* result of evaluation */
    ptdesc = (T_PTD)READSTACK(S_a);               /* CLOS-descriptor */
    dim = ARITY(x);                               /* arity of AP(n) */
    DEC_REFCNT((T_PTD)R_CLOS(*ptdesc,pta)[dim]);  /* always a pointer !!! */
    R_CLOS(*ptdesc,pta)[dim] = y;                 /* replace by value */
    PPOPSTACK(S_m);
    goto main_e;
  } /* end T_AP */
  else
  if (T_LIST(x)) {
    /* element einer liste wurde evaluiert */
    y = POPSTACK(S_a);                            /* result of evaluation */
    ptdesc = (T_PTD)READSTACK(S_a);               /* LIST-descriptor */
    dim = ARITY(x);                               /*  arity of LIST(n) */
    DEC_REFCNT((T_PTD)R_LIST(*ptdesc,ptdv)[dim]); /* always a pointer !!! */
    R_LIST(*ptdesc,ptdv)[dim] = y;                /* replace by value */
    PPOPSTACK(S_m);
    goto main_e;
  } /* end T_LIST */
  else
  if (T_SNAP(x)) {
    /* element einer applikation wurde evaluiert */
    y = POPSTACK(S_a);                           /* result of evaluation */
    ptdesc = (T_PTD)READSTACK(S_a);              /* EXPR-descriptor */
    dim = ARITY(x);                              /* arity of SNAP(n) */
    DEC_REFCNT((T_PTD)R_EXPR(*ptdesc,pte)[dim]); /* always a pointer !!! */
    R_EXPR(*ptdesc,pte)[dim] = y;                /* replace by value */
    PPOPSTACK(S_m);
    goto main_e;
  } /* end T_SNAP */
  else
  if (T_KLAA(x)) {
    /* terminierung */
    PPOPSTACK(S_m);
    END_MODUL("evaluate");
    return;
  } /* end T_KLAA */
  else {
    post_mortem("evaluate: unexpected symbol on stack m");
  }
} /* end evaluate() */

/* end of       rreduct.c */
