/* $Log: rear.c,v $
 * Revision 1.2  1992/12/16  12:49:52  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/*****************************************************************************/
/* file rear.c                                                               */
/*                                                                           */
/*      - compileflags: see rstdinc.h                                        */
/*                                                                           */
/*****************************************************************************/

#define EACOMP   "Compiler in abstrakten Code"

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rinter.h"
#include "rprimf.h"

#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

#if D_DIST                     /* using Stack S_pm for dist-ASM-Code */
extern StackDesc S_pm;
extern void updatestack();                 /* rstack.c */
extern STACKELEM midstack();               /* rstack.c */
extern int d_nodistnr;
#endif

/* --- interne globale/lokale routinen --- */

       void      initcode();
       void      ear();

static void      newcode();
static void      load_body();
static void      del_expr();
static int       test_switch();
static int       test_num();
static STACKELEM new_func();
static STACKELEM new_cond();
static STACKELEM new_case();
static STACKELEM new_sub();
static int       collect_args();

/* --- externe variablen/routinen --- */

extern int      _redcnt;
extern int      _count_reductions;                  /* red zaehlen?  */
extern int      _beta_count_only;                   /* nur beta-red. */
extern T_PTD    newdesc();                          /* rheap.c */
extern void     load_expr();                        /* rhinout.c */
extern void     res_heap(); /* TB, 4.11.1992 */     /* rheap.c */
extern void     rel_heap(); /* TB, 4.11.1992 */     /* rheap.c */
extern void     test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void     test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */   /* rstack,c */
/* setups */

extern char     CodeFile[];                         /* rreduce.c */
extern char     CodeDump[];                         /* rreduce.c */
extern int      JumpPrefered;                       /* rreduce.c */
extern int      LazyLists;                          /* rreduce.c */

/* RS 30/10/92 */

extern T_HEAPELEM *newbuff();                       /* rheap.c */
extern void stack_error();                          /* rstack.c */
/* END of RS 30/10/92 */

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

/* --- panik exit --- */

#if    DEBUG
static FILE *codefp = 0;
       FILE *fopen();
#define POST_MORTEM(mes)      if (codefp) { fclose(codefp); post_mortem(mes); } else post_mortem(mes)
#else
#define POST_MORTEM(mes)      post_mortem(mes)
#endif /* DEBUG     immer auskommentiert von RS 3.12.1992 */

/* --- codedescriptor --- */

T_CODEDESCR codedesc;         /* siehe rinter.h */

#define NEWCODE(c)            newcode((INSTR)(c))

#define INFO_BUF_LEN          16

static char buf[80];

/* --- function initcode: --- */

void initcode(hplen)
int hplen;
{
  extern int CodeAreaPercent; /* setup.c */
  int len;

#if ! SCAVENGE
  /* puffer fuer dumpheap-info alloziieren */
  if (newbuff(INFO_BUF_LEN) == 0)
    post_mortem("initcode: heap overflow");
#endif /* SCAVENGE */

  /* codevektor alloziieren */
  len = ((hplen / sizeof(T_HEAPELEM)) * CodeAreaPercent) / 100;
  if ((CODEVEC = (INSTR *)newbuff(len)) != NULL) {
    CODEPTR = CODEVEC;
    CODECNT = len;
    CODESIZ = len;
  }
  else post_mortem("initcode: heap overflow");
}

/* --- function newcode: --- */

static void newcode(c)
INSTR c;
{
  if (--CODECNT) {
    *CODEPTR++ = (INSTR)(c);
  }
  else {
    POST_MORTEM("newcode: code vector overflow");
  }
}

/* --- macros --- */

#if    DEBUG

extern char *stelname();      /* rdesc.c (rinter.h) */

#define GEN_INSTR0(i,nm)      (NEWCODE(i), \
                               fprintf(codefp,"\t %s();\n",nm))
#define GEN_INSTR1(i,nm,a)    (NEWCODE(i), NEWCODE(a), \
                               fprintf(codefp,"\t %s(%d);\n",nm,a))
#define GEN_INSTR2(i,nm,a,b)  (NEWCODE(i), NEWCODE(a), NEWCODE(b), \
                               fprintf(codefp,"\t %s(%d,%d);\n",nm,a,b))

#if D_DIST              /* dist and distb-ASM 4 Arguments only */
#define GEN_INSTR4(i,nm,a,b,c,c2)  (NEWCODE(i), NEWCODE(a), NEWCODE(b), NEWCODE(c), NEWCODE(c2), \
                                  fprintf(codefp,"\t %s(%d,%d,%d,%d);\n",nm,a,b,c,c2))
#endif

#define GEN_INSTRN(i,nm,a)    (NEWCODE(i), NEWCODE(a), \
                               fprintf(codefp,"\t %s(%s);\n",nm,stelname((STACKELEM)a)))     /* STACKELEM von ach 09/11/92 */
#define GEN_INSTRX(i,nm,a)    (NEWCODE(i), NEWCODE(a), \
                               fprintf(codefp,"\t %s(%s);\n",nm,stelname((STACKELEM)a)))     /* STACKELEM von ach 09/11/92 */
#define GEN_INSTRL(i,nm,p,a)  (NEWCODE(i), NEWCODE(a), \
                               fprintf(codefp,"\t %s(%s_%x);\n",nm,p,(unsigned int)a))
#define GEN_DESC(desc,class,type,args,nargs,ptd,prefix,ptc) \
                              (NEWCODE(I_DESC), NEWCODE(desc), \
                               fprintf(codefp,"\t _desc(0x%08x,%s,%s,%d,%d,0x%08x,%s_%x);\n", \
                               (unsigned int)desc,class,type,args,nargs,(unsigned int)ptd,prefix,(unsigned int)ptc))
#define GEN_LABEL(p,label)    fprintf(codefp,"%s_%x:\n",p,(unsigned int)label)

#else

#define GEN_INSTR0(i,nm)      (NEWCODE(i))
#define GEN_INSTR1(i,nm,a)    (NEWCODE(i), NEWCODE(a))
#define GEN_INSTR2(i,nm,a,b)  (NEWCODE(i), NEWCODE(a), NEWCODE(b))

#if D_DIST              /* dist and distb-ASM 4 Arguments only */
#define GEN_INSTR4(i,nm,a,b,c,c2)  (NEWCODE(i), NEWCODE(a), NEWCODE(b), NEWCODE(c), NEWCODE(c2))
#endif

#define GEN_INSTRN(i,nm,a)    (NEWCODE(i), NEWCODE(a))
#define GEN_INSTRX(i,nm,a)    (NEWCODE(i), NEWCODE(a))
#define GEN_INSTRL(i,nm,p,a)  (NEWCODE(i), NEWCODE(a))
#define GEN_DESC(desc,class,type,args,nargs,ptd,prefix,ptc) \
                              (NEWCODE(I_DESC), NEWCODE(desc))
#define GEN_LABEL(p,label)

#endif /* DEBUG */

#define END_OF_BODY()         T_DOLLAR(READSTACK(S_m))
#define NO_MORE_VARIABLES()   T_DOLLAR(READSTACK(S_e)) 

/* descriptor anlegen und initialisieren */
/* dg 07.05.92 MAKEDESC in rheapty.h definiert */
/* #define MAKEDESC(p,ref,cl,ty) if ((p = newdesc()) == NULL) { \
                                POST_MORTEM("ear: no more descriptors"); \
                              } else DESC_MASK(p,ref,cl,ty) */

/* umrechnung von nummernvariablen in indizes */
#define ARG_INDEX(i)      (i)

/* die folgenden vier flags geben bei CONDI descriptoren (feld args) an */
/* ob die argumente im then/else code freigegeben werden duerfen (FREE/KEEP) */
/* darueberhinaus gibt FREE/KEEP_ARGS2 an ob es sich um ein conditional mit */
/* vorangestellter PUSHRET instruktion handelt */

#define FREE_ARGS         0
#define KEEP_ARGS         1
#define FREE_ARGS2        2
#define KEEP_ARGS2        3

/* test auf KEEP/FREE bzw mit/ohne PUSHRET */
#define KEEP_FLAG(x)      (x % 2)
#define COND_FLAG(x)      (x / 2)

/* generiert instruktionen zur freigabe von argumenten */
#if D_MESS

#define FREE_INSTR(i,nf)  do {if ((i) > 0) { \
                            if ((i) == 1) { \
  if (D_M_C_FREE1) \
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE1); \
                              GEN_INSTR0(I_FREE1,"free1"); \
                            } \
                            else { \
  if (D_M_C_FREE) \
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE); \
                              GEN_INSTR1(I_FREE,"free",(i)); \
                            } \
                            nf += (i); \
                          }} while(0)
#else

#define FREE_INSTR(i,nf)  do if ((i) > 0) { \
                            if ((i) == 1) { \
                              GEN_INSTR0(I_FREE1,"free1"); \
                            } \
                            else { \
                              GEN_INSTR1(I_FREE,"free",(i)); \
                            } \
                            nf += (i); \
                          } while(0)
#endif

#if    UH_ZF_PM
/* hier haengt ulli noch einge makros ein ! */
/*uh, 16.7.90*/   /*dg 3.8.90*/
#define FORWARDMARK(desc)         MAKEDESC(desc,1,C_FUNC,TY_CONDI); \
/*uh, 16.7.90*/                                    L_CONDI(*desc,ptc) = CODEPTR + 1; \
/*uh, 16.7.90*/                                    L_CONDI(*desc,ptd) = _nil; \
/*uh, 16.7.90*/                                    INC_REFCNT(_nil)
#define FORWARDRESOLVE(text,desc) *R_CONDI(*desc,ptc) = (int)CODEPTR; \
/*uh, 16.7.90*/                                    GEN_LABEL(text,desc)
#define BACKWARDMARK(text,desc)   MAKEDESC(desc,1,C_FUNC,TY_CONDI); \
/*uh, 16.7.90*/                                    L_CONDI(*desc,ptc) = CODEPTR; \
/*uh, 16.7.90*/                                    L_CONDI(*desc,ptd) = _nil; \
/*uh, 16.7.90*/                                    INC_REFCNT(_nil); \
/*uh, 16.7.90*/                                    GEN_LABEL(text,desc)
#define BACKWARDRESOLVE(desc)     *(CODEPTR-1) = (int)R_CONDI(*desc,ptc)
/*uh, 16.7.90*/  
#endif /* UH_ZF_PM */

/*---------------------------------------------------------------------*/
/* load_body: (vgl. load_expr in rhinout.c)                            */
/*            kopiert einen heapausdruck in den e-stack. der parameter */
/*            pptr ist ein pointer auf die heapdarstellung, deren      */
/*            laenge im ersten feld steht. die position ab der keine   */
/*            nummernvariablen und conditionals mehr vorkommen wird mit*/
/*            dollar markiert.                                         */
/*---------------------------------------------------------------------*/

static void load_body(pptr,margs,nargs)
STACKELEM **pptr;                       /* pointer auf heapdarstellung */
int       margs;                        /* gesamtzahl der argumente */
int       nargs;                        /* freizugebende argumente */
{
  register int          size;
  register STACKELEM       x;
  register STACKELEM    *ptr;
           int          index;

  START_MODUL("load_body");
  RES_HEAP;
  ptr = *pptr;
  size = ptr[0];
  while (size > 0) {
    x = ptr[size];
    if (T_POINTER(x)) {
      INC_REFCNT((T_PTD)x);
      if (nargs > 0) {
        if (R_DESC(*(T_PTD)x,type) == TY_COND) {
          /* die argumente duerfen nicht vor dem cond freigegeben werden */
          PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
          nargs = 0;
        }
        else
        if (R_DESC(*(T_PTD)x,type) == TY_SWITCH) {
          /* die argumente duerfen nicht vor dem switch freigegeben */
          /* werden, wenn dieses relativ freie variablen enthaelt */
          if (test_switch(x)) {
            /* switch hat relativ freie variablen */
            PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
            nargs = 0;
          }
        }
#if    UH_ZF_PM
        /* ullis zf-expressions muessen hier beruechsichtigt werden ! */
                         else
/*uh, 24.7.90*/          if  (R_DESC(*(T_PTD)x,type) == TY_ZF)  {
/*uh, 24.7.90*/            PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
/*uh, 24.7.90*/            nargs = 0;
/*uh, 24.7.90*/          }
#endif /* UH_ZF_PM */
      }
      /* nargs == 0 */
    }
    else
    if (nargs && T_NUM(x)) {
      /* da wo argumente freigegeben werden koennen wird ein DOLLAR */
      /* dazwischengeschoben dessen VALUE-Feld deren anzahl angibt  */
      index = margs-1-VALUE(x);
      if (index < nargs) {
        nargs -= index;
        PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
        nargs = index;
      }
    }
    PUSHSTACK(S_e,x);
    size--;
  }
  REL_HEAP;
  if (nargs > 0)
    PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
  END_MODUL("load_body");
}

/* ------------------------------------------------------------------- */
/* del_expr: loescht einen ausdruck, der keine multi-atoms enthaelt    */
/* ------------------------------------------------------------------- */

static void del_expr(st)
StackDesc *st;
{
  STACKELEM x;
  int       i;

  START_MODUL("del_expr");
  for (i = 1 ; i > 0 ; i--) {
    x = POPSTACK(*st);
    if (T_CON(x))
      i += ARITY(x);
    else
    if (T_POINTER(x))
      DEC_REFCNT((T_PTD)x);
    else
    /* dg 13.07.92 deletion of expression loaded by load_body */
    if (T_NUM(x) && T_DOLLAR(READSTACK(*st)))
      PPOPSTACK(*st);
  }
  END_MODUL("del_expr");
}

/* ------------------------------------------------------------------- */
/* test_switch: testet ob ein switch relativ freie variablen enthaelt  */
/* ------------------------------------------------------------------- */

static int test_switch(pts)
T_PTD pts;
{
  int i;
  T_PTD ptd;
/*  STACKELEM x;           RS 30/10/92 */

  i = R_SWITCH(*pts,nwhen);
  ptd = (T_PTD)R_SWITCH(*pts,ptse)[i];
  if (R_DESC(*ptd,type) == TY_NOMAT) {
    /* nomatch descriptor, skippen */
    --i;
  }
  for ( ; i > 0 ; i--) {
    /* match descriptor holen */
    ptd = (T_PTD)R_SWITCH(*pts,ptse)[i];
    if (R_MATCH(*ptd,guard)) {
      /* guard ungleich NULL */
      if (test_num(R_MATCH(*ptd,guard)))
        return(i);
      /* sonst: i-ter guard hat keine relativ freie variablen */
    }
    if (R_MATCH(*ptd,body)) {
      /* body ungleich NULL */
      if (test_num(R_MATCH(*ptd,body)))
        return(i);
      /* sonst: i-ter body hat keine relativ freie variablen */
    }
  }
  /* switch hat keine relativ freie variablen */
  return(0);
}

/* --------------------------------------------------------------------- */
/* test_num: durchsucht einen heapbereich rekursiv nach nummernvariablen */
/* --------------------------------------------------------------------- */

static int test_num(p)
PTR_HEAPELEM p;
{
  int len = *p++; /* erster eintrag: laenge des bereichs */

  for (; len > 0 ; p++,len--) {
    if (T_POINTER(*p)) {
      T_PTD ptd = (T_PTD)*p;
      if (R_DESC(*ptd,class) == C_EXPRESSION) {
        switch (R_DESC(*ptd,type)) {
          case TY_COND:
                        if (test_num(R_COND(*ptd,ptte)))
                          return(1);
                        if (test_num(R_COND(*ptd,ptee)))
                          return(1);
                        break;
          case TY_EXPR:
                        if (test_num(R_EXPR(*ptd,pte)))
                          return(1);
                        break;
          case TY_SWITCH:
                        if (test_switch(ptd))  /* *ptd durch ptd ersetzt, warum auch immer RS 4.12.1992 */
                          return(1);
                        break;
        }
      }
      /* ausdruck enthaelt keine nummernvariablen */
    }
    else
    if (T_NUM(*p)) {
      /* nummernvariable gefunden */
      return(1);
    }
  } /* end for */
  /* keine nummernvariable in dem heapbereich */
  return(0);
} /* end test_num */

/* ------------------------------------------------------------------- */
/*  new_func : durchsucht den i-stack nach einem eintrag fuer x und    */
/*             fuegt x gegebenenfalls unten ein                        */
/*        Bem. eine ziemlich bloedsinnige such- und einfuegroutine,    */
/*             sollte geschickter implementiert werden !  (dg)         */
/* ------------------------------------------------------------------- */

static STACKELEM new_func(x,n)
STACKELEM x;
int       n;
{
  STACKELEM h=(STACKELEM)0; /* Initialisierung von TB, 6.11.1992 */
  T_PTD     ptd /*,px*/;   /* RS 30/10/92 */
  int       i;
  int       found = 0;

  /* suchen ob schon vorhanden */
  for (i = SIZEOFSTACK(S_v) ; i > 0 ; i--) {
    h = POPSTACK(S_v);
    PUSHSTACK(S_hilf,h);
    found = (R_UNIV((*(T_PTD)h),ptd) == (T_PTD)x);
    if (found) break;
  }
  for (i = SIZEOFSTACK(S_hilf) ; i > 0 ; i--)
    PUSHSTACK(S_v,POPSTACK(S_hilf));
  if (found) {
   /* INC_REFCNT((T_PTD)h); */
    return(h);
  }
  /* einfuegen auf i */
  for (i = SIZEOFSTACK(S_i) ; i > 0 ; i--) {
    PUSHSTACK(S_hilf,POPSTACK(S_i));
  }
  MAKEDESC(ptd,1,C_FUNC,TY_COMB);
  L_COMB((*ptd),args) = 0;
  L_COMB((*ptd),nargs) = n;
  L_COMB((*ptd),ptd) = (T_PTD)x;
  L_COMB((*ptd),ptc) = (INSTR *)0;
  PUSHSTACK(S_v,(STACKELEM)ptd);
  PUSHSTACK(S_i,(STACKELEM)ptd);
  for (i = SIZEOFSTACK(S_hilf) ; i > 0 ; i--)
    PUSHSTACK(S_i,POPSTACK(S_hilf));
  return((STACKELEM)ptd);
}

/* ------------------------------------------------------------------- */
/*  new_cond()  legt einen neuen cond-pointer unter alle bereits       */
/*              vorhandenen cond-pointer auf den i-stack               */
/* ------------------------------------------------------------------- */

static STACKELEM new_cond(x,n)
STACKELEM x;
int       n;
{
  T_PTD ptd;
  int   i;

  for (i = SIZEOFSTACK(S_i) ; i > 0 ; i--) {
    ptd = (T_PTD)READSTACK(S_i);
    if (R_DESC((*ptd),type) != TY_CONDI)
      break;
    PUSHSTACK(S_hilf,POPSTACK(S_i));
  }
  MAKEDESC(ptd,1,C_FUNC,TY_CONDI);
  L_CONDI((*ptd),args) = 0;
  L_CONDI((*ptd),nargs) = n;
  L_CONDI((*ptd),ptd) = (T_PTD)x;
  L_CONDI((*ptd),ptc) = (INSTR *)0;
  PUSHSTACK(S_i,ptd);
  for (i = SIZEOFSTACK(S_hilf) ; i > 0 ; i--)
    PUSHSTACK(S_i,POPSTACK(S_hilf));
  return((STACKELEM)ptd);
}

/* ------------------------------------------------------------------- */
/*  new_case()  legt einen neuen case-pointer unter alle bereits       */
/*              vorhandenen cond/case-pointer auf den i-stack          */
/* ------------------------------------------------------------------- */

static STACKELEM new_case(x,n)
STACKELEM x;                      /* switch-descriptor */
int       n;                      /* laenge arg.frame + 1 */
{
  T_PTD ptd;
  int   i;

  for (i = SIZEOFSTACK(S_i) ; i > 0 ; i--) {
    ptd = (T_PTD)READSTACK(S_i);
    if (R_DESC((*ptd),type) != TY_CONDI)
      break;
    PUSHSTACK(S_hilf,POPSTACK(S_i));
  }
  MAKEDESC(ptd,1,C_FUNC,TY_CASE);
  i = test_switch((T_PTD)x); /* rel. fr. var.? 0: nein, >0: ja */ /* T_PTD eingefuegt RS 3.12.1992 */
  L_CASE((*ptd),args) = i;
  L_CASE((*ptd),nargs) = i ? n : 1;
  /* NCASE: add number of multiple patterns */
  /* case_type is the pattern arity! */
  L_CASE((*ptd),nargs) +=  R_SWITCH(*(T_PTD)x,case_type) - 1;
  L_CASE((*ptd),ptd) = (T_PTD)x;
  L_CASE((*ptd),ptc) = (INSTR *)0;
  PUSHSTACK(S_i,ptd);
  for (i = SIZEOFSTACK(S_hilf) ; i > 0 ; i--)
    PUSHSTACK(S_i,POPSTACK(S_hilf));
  return((STACKELEM)ptd);
}

/* ------------------------------------------------------------------- */
/*  new_sub()   legt einen neuen sub-pointer unter alle bereits        */
/*              vorhandenen cond-pointer auf den i-stack               */
/* ------------------------------------------------------------------- */

static STACKELEM new_sub(x,n)
STACKELEM x;
int       n;     /* argsneeded */
{
  T_PTD ptd;
  int   i;

  for (i = SIZEOFSTACK(S_i) ; i > 0 ; i--) {
    ptd = (T_PTD)READSTACK(S_i);
    if (R_DESC((*ptd),type) != TY_CONDI)
      break;
    PUSHSTACK(S_hilf,POPSTACK(S_i));
  }
  MAKEDESC(ptd,1,C_FUNC,TY_COMB);
  L_COMB((*ptd),args) = 0;
  L_COMB((*ptd),nargs) = n;
  L_COMB((*ptd),ptd) = (T_PTD)x;
  L_COMB((*ptd),ptc) = (INSTR *)0;
  PUSHSTACK(S_i,ptd);
  for (i = SIZEOFSTACK(S_hilf) ; i > 0 ; i--)
    PUSHSTACK(S_i,POPSTACK(S_hilf));
  return((STACKELEM)ptd);
}

/* ------------------------------------------------------------------- */
/* collect_args : zaehlt die argumente auf a und entfernt die          */
/*                dazugehoerigen applikatoren von den m-stacks.        */
/* ------------------------------------------------------------------- */

static int collect_args()
{
  register int args = 0;

#if D_DIST       /* check for T_AP_1 and T_DAP_1, Application and Dapplication with counter 1 */
  while (T_AP_1(READSTACK(S_m)) || T_DAP_1(READSTACK(S_m)))
#else     
  while (T_AP_1(READSTACK(S_m)))
#endif
    {
    args += (READ_ARITY(S_m1) - 1);
    PPOPSTACK(S_m);
    PPOPSTACK(S_m1);
  }
  return(args);
}

/* ------------------------------------------------------------------- */
/*  ear: codegenerierung                                               */
/*                                                                     */
/*  traversieren von e nach a, dabei codegenerierung.                  */
/*  werden funktions-pointer angetroffen, so landen diese auf v und    */
/*  auf i (falls nicht schon auf v vorhanden) um spaeter wieder von    */
/*  i runtergeholt und bearbeitet zu werden. fertig, wenn auf m ein    */
/*  trennsymbol KLAA liegt und i leer ist.                             */
/* ------------------------------------------------------------------- */

void ear()
{
  register STACKELEM x;
  register T_PTD     desc, px;
           int       args;
           int       argsneeded;
           int       arity;
           int       margs = 0;       /* laenge des argumentframes */
           int       nfree = 0;       /* bereits freigegebene arg. */
           int       gen_return = 1;  /* return generieren j/n  */
           int       gen_exit   = 1;  /* exit   generieren j/n  */
           int       listcnt = 0;     /* listen-verschachtelung    */
           int       i;               /* laufindex */
           INSTR    *cp;              /* hilfspointer   */
#if D_DIST
           STACKELEM hooray;          /* dummy for POPSTACK */
           int       yeah;            /* new variable for dist-ASM-code */
#endif

#if    UH_ZF_PM
           /* hier fuegt ulli noch einige deklarationen dazu ! */
/*uh, 19.7.90*/             T_PTD     for_desc, rof_desc, if_desc, listerr_desc, fi_desc1, fi_desc2, end_desc;
/*uh, 19.7.90*/             T_PTD        cdesc;
/*uh, 19.7.90*/             PTR_HEAPELEM zfc=0;    /* Pointer to ZF-Subexpression */
/*uh, 19.7.90*/             PTR_HEAPELEM vars=0;   /* Pointer to bound Variablenames */
/*uh, 19.7.90*/             T_PTD        zfd=0;
/*uh, 19.7.90*/             /* int          zfbound = 0;  umbenannt in nr_additionals uh 11.4.92 */
/*uh, 19.7.90*/             int          nr_additionals = 0;
#endif /* UH_ZF_PM */

  START_MODUL("ear");

  CODEPTR = CODEVEC;        /* codepointer initialisieren */
  CODECNT = CODESIZ;        /* rueckwaertszaehler initialisieren */
  SYMBTAB = NULL;           /* noch keine symboltabelle */
  SYMBLEN = 0;              /* laenge 0 */

  NEWCODE(I_TABSTART);      /* verweis auf symboltabelle initialisieren */
  NEWCODE(0);               /* position noch nicht bekannt */

  PUSHSTACK(S_m,KLAA);      /* bottomsymbol auf m */
  x = READSTACK(S_e);       /* auf e wird ein expressionpointer erwartet! */
  WRITESTACK(S_e,KLAA);     /* bottomsymbol auf e */
#if    DEBUG
  /* codefile aufmachen */
  if (codefp) fclose(codefp);
  if ((codefp = fopen(CodeFile,"w")) == NULL)
     post_mortem("ear: can't open codefile");
  if (x == 0 || !T_POINTER(x) || R_DESC(*(T_PTD)x,class) != C_EXPRESSION)
     POST_MORTEM("ear: must start with an expression pointer");
#endif /* DEBUG */
  /* ersten codedescriptor anlegen und initialisieren */
  MAKEDESC(desc,1,C_FUNC,TY_COMB);
  L_COMB(*desc,args) = 0;
  L_COMB(*desc,nargs) = 0;
  L_COMB(*desc,ptd) = (T_PTD)x;
  GEN_DESC(desc,"FUNC","COMB",0,0,R_COMB(*desc,ptd),"func",desc);
  L_COMB(*desc,ptc) = CODEPTR;

/* dbug-system problem with uninitialized codevector   */
/* so first instruction will be initialized with I_END */   /* RS 22.1.1993 */

  *CODEPTR = I_END; 

  GEN_LABEL("start",desc);
  PUSHSTACK(S_a,desc);
  load_expr((STACKELEM **)A_EXPR((*(T_PTD)x),pte));  /* ausdruck einlagern */

/*        ***** for dymanic on and off switching !!!!!!! ****
#if D_MESS

#if D_MHEAP
  if (D_M_C_HEAP_ALLOC)
    GEN_INSTR0(I_MHPALLON, "mhpallon");
  if (D_M_C_HEAP_FREE)
    GEN_INSTR0(I_MHFREEON, "mhfreeon");
  if (D_M_C_HEAP_COMPACT)
    GEN_INSTR0(I_MHPCMPON, "mhpcmpon");
  if (D_M_C_DESC_ALLOC)
    GEN_INSTR0(I_MDESCALLON, "mdescallon");
  if (D_M_C_DESC_FREE)
    GEN_INSTR0(I_MDESCFREON, "mdescfreon");
#endif

#if D_MSCHED
  if (D_M_C_PROC_DIST)
    GEN_INSTR0(I_MSDISTON, "msdiston");
  if (D_M_C_PROC_CREATE)
    GEN_INSTR0(I_MPROCREON, "mprocreon");
  if (D_M_C_PROC_TERMINATE)
    GEN_INSTR0(I_MPROTERON, "mproteron");
  if (D_M_C_PROC_SLEEP)
    GEN_INSTR0(I_MPRSLEEON, "mprsleeon");
  if (D_M_C_PROC_RUN)
    GEN_INSTR0(I_MPRORUNON, "mprorunon");
  if (D_M_C_PROC_WAKEUP)
    GEN_INSTR0(I_MPROWAKON, "mprowakon");
#endif

#if D_MCOMMU
  if (D_M_C_SEND_BEGIN)
    GEN_INSTR0(I_MCOMSEBON, "mcomsebon");
  if (D_M_C_SEND_END)
    GEN_INSTR0(I_MCOMSEEON, "mcomseeon");
  if (D_M_C_RECEIVE_BEGIN)
    GEN_INSTR0(I_MCOMREBON, "mcomrebon");
  if (D_M_C_RECEIVE_END)
    GEN_INSTR0(I_MCOMREEON, "mcomreeon");
#endif

#if D_MSTACK
  if (D_M_C_STACK_PUSH)
    GEN_INSTR0(I_MSTCKPUON, "mstckpuon");
  if (D_M_C_STACK_POP)
    GEN_INSTR0(I_MSTCKPPON, "mstckppon");
  if (D_M_C_STACK_SEG_ALLOC)
    GEN_INSTR0(I_MSTKSEGAL, "mstksegal");
  if (D_M_C_STACK_SEG_FREE)
    GEN_INSTR0(I_MSTKSEGFR, "mstksegfr");
#endif
#endif
for later use */

  main_e:        /* --- e-line --- */

  x = POPSTACK(S_e);                                 /* top            */

#if     DEBUG
  if (x == 0) POST_MORTEM("ear: null on stack e");   /* zur sicherheit */
#endif /* DEBUG */

  if (T_DOLLAR(x)) {                               /* argumente freigeben */
    i = VALUE(x);
    FREE_INSTR(i,nfree);
    goto main_e;
  }

  if T_NUM(x) {                                      /* nummernvariable */
#if    UH_ZF_PM
    /* ullis Berechnung der Indizes f. ZF-Variablen */
/*uh, 26.7.90*/  /*dg 3.8.90 treibe nicht schindluder mit den moeglichkeiten die C bietet. (altes chinesisches sprichwort) */
/*uh, 26.7.90*/  /*uh  if ((i=VALUE(x) >= zfbound) && (i<(margs+zfbound))) i = ARG_INDEX(margs-1-VALUE(x)+2*zfbound); */
/*uh, 26.7.90*/  
/*uh, 26.7.90*/  
/*uh, 26.7.90*/  /*uh+dg*/ /* if (((i=VALUE(x)) >= zfbound) && (i<(margs+zfbound))) i = ARG_INDEX(margs-1-VALUE(x)+2*zfbound); */
/*uh, 26.7.90*/  
/*uh, 26.7.90*/  /* if ( (VALUE(x) >= nr_additionals)  && (VALUE(x) <margs+nr_additionals) ) {  */
/*uh, 26.7.90*/  /*     i = ARG_INDEX(margs-1-VALUE(x)+2*(nr_additionals));                     */
/*uh, 26.7.90*/  /* } else {								       */
/*uh, 26.7.90*/  /*     i = ARG_INDEX(VALUE(x));						       */
/*uh, 26.7.90*/  /* }									       */	
/*uh, 26.7.90*/  
/*uh, 26.7.90*/  if ( (VALUE(x) < nr_additionals) || (VALUE(x) >= margs) ) { /* Variable in erweiterten Frames */
/*uh, 26.7.90*/         i = ARG_INDEX(VALUE(x));
/*uh, 26.7.90*/  } else {
/*uh, 26.7.90*/         i = ARG_INDEX(margs - 1 - VALUE(x) + nr_additionals);
/*uh, 26.7.90*/  }
/*uh, 26.7.90*/  
/*uh, 26.7.90*/  
/*uh, 26.7.90*/  /* Berechnung der PUSHARG-Indizes im erweiterten Stack-Frame */
/*uh, 26.7.90*/  /*  Nummernvariable                            Typ                              Index      */
/*uh, 26.7.90*/  /*-----------------------------------------------------------------------------------------*/
/*uh, 26.7.90*/  /*  0..nr_additionals-1                   erweiterte-Variable                    wie #     */
/*uh, 26.7.90*/  /*  nr_additionals..margs-1                 lambda-Variable                  invertierte # */
/*uh, 26.7.90*/  /*  margs ..                    erweiterte-Variable umschliessender PM/ZF-Expr   wie #     */
/*uh, 26.7.90*/  
/*uh, 26.7.90*/  /*       bisher: */
/*uh, 26.7.90*/  /*  nr_additionals..nr_additionals+margs-1  lambda-Variable                  invertierte # */
/*uh, 26.7.90*/  /*  nr_additionals+margs ..     erweiterte-Variable umschliessender PM/ZF-Expr   wie #     */
/*uh, 26.7.90*/  
#if  DEBUG
/*uh, 26.7.90*/   if (i<0) post_mortem("EAR: negative Index generated");
#endif
/*uh, 26.7.90*/  
#else
    i = ARG_INDEX(margs-1-VALUE(x));
#endif
    /* argumentframe freizugeben? */
    if (NO_MORE_VARIABLES()) {                  /* argumente koennen  */
      i = VALUE(POPSTACK(S_e)) - 1;                  /* freigegeben werden */
      /* folgt return? */
      if (i == 0 && END_OF_BODY()) {
        /* return folgt, optimierung */
#if D_MESS
  if (D_M_C_RTM)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTM);
#endif
        GEN_INSTR0(I_RTM,"rtm");
        gen_return = 0;
      }
      else {
#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("1: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (nfree-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */

#if D_MESS
  if (D_M_C_MOVE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE);
#endif
        GEN_INSTR0(I_MOVE,"move");                /* optimierung    */
      }
      nfree++;
      FREE_INSTR(i,nfree);
    }
    else {
      if (i == 0 || i == nfree) {
#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("2: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))   /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (nfree-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */

#if D_MESS
  if (D_M_C_PUSHARG0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHARG0);
#endif
        GEN_INSTR0(I_PUSHARG0,"pusharg0");
      }
      else {
#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("3: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, i)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (i-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */

#if D_MESS
  if (D_M_C_PUSHARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHARG);
#endif
        GEN_INSTR1(I_PUSHARG,"pusharg",i-nfree);
      }
    }
    if ((args = collect_args()) > 0) {
      /* in funktionsposition, apply instruktion */
      if (LazyLists && listcnt > 0) {
#if D_MESS
  if (D_M_C_MKGCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKGCLOS);
#endif
        GEN_INSTR1(I_MKGCLOS,"mkgclos",args);
      }
      else {
#if D_MESS
  if (D_M_C_APPLY)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_APPLY);
#endif
        GEN_INSTR1(I_APPLY,"apply",args);
      }
    }
    goto main_m;                                     /* weiter auf m       */
  }

  if T_POINTER(x) {                                  /* pointer auf e   */
    desc = (T_PTD)x;                                 /* cast            */
    DEC_REFCNT(desc);                                /* refcount runter */
    if ((R_DESC(*desc,class)) != C_EXPRESSION) {
      /* argumentframe freizugeben? */
      if (NO_MORE_VARIABLES()) {                /* argumente koennen  */
        i = VALUE(POPSTACK(S_e));                    /* freigegeben werden */
        FREE_INSTR(i,nfree);
      }
      /* ein daten oder variablendeskriptor, push-instruktion */
      /* folgt return? */
      if (END_OF_BODY()) {
        /* return folgt, optimierung */
#if D_MESS
  if (D_M_C_RTP)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTP);
#endif
        GEN_INSTRX(I_RTP,"rtp",x);
        gen_return = 0;
      }
      else {
#if D_MESS
  if (D_M_C_PUSH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH);
#endif
        GEN_INSTRX(I_PUSH,"push",x);
      }
      goto main_m;
    }
    /* also ein expression pointer */
    switch(R_DESC(*desc,type)) {
/*uh, 7.3.91*/        case TY_COND:                                  /* conditional          */
/*uh, 7.3.91*/                      args = collect_args();           /* argumente aufsammeln */
/*uh, 7.3.91*/                      if (args > 0) {
/*uh, 7.3.91*/                        /* conditional in funktionsposition */
/*uh, 7.3.91*/                        MAKEDESC(desc,1,C_FUNC,TY_CONDI); /* descriptor anlegen */
/*uh, 7.3.91*/                        L_CONDI(*desc,nargs) = margs+1;   /* anz. argumente */
/*uh, 7.3.91*/                        L_CONDI(*desc,ptd) = (T_PTD)x; /* original cond */
                      if (args == 1 && END_OF_BODY()) {
                        /* das if-then-else ist der vollstaendige rumpf */
                        /* darf das argumentframe im then/else teil */
                        /* freigegeben werden? */
			/* (int) von TB, 9.11.1992 */
                        i = (R_DESC(*(T_PTD)READSTACK(S_a),type) == TY_CONDI
                            && KEEP_FLAG((int)R_CONDI(*(T_PTD)READSTACK(S_a),
						      args)))
                            ? KEEP_ARGS : FREE_ARGS;
                        /* free instruktion wird unterdrueckt, falls jcond */
                        /* das argumentframe selber freigibt/aufbraucht */
                        if (NO_MORE_VARIABLES() && i == FREE_ARGS)
                          PPOPSTACK(S_e);
                      }
                      else {
                        /* if-then-else ist nicht der vollstaendige rumpf */
                        /* cond/pushret kenntlich machen */
                        if (NO_MORE_VARIABLES()
                         && VALUE(READSTACK(S_e)) == margs) {
                          /* argumente koennen im then/else code freigegeben */
                          /* werden, free instruktion hier unterdruecken */
                          PPOPSTACK(S_e);             /* DOLLAR */
                          i = FREE_ARGS2;
                        }
                        else i = KEEP_ARGS2;
#if D_MESS
  if (D_M_C_PUSHRET)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHRET);
#endif
                        GEN_INSTRL(I_PUSHRET,"pushret","cont",desc);
                        if (args > 1) {
                          /* zu viele argumente, applikatoren zurueck auf m */
                          PUSHSTACK(S_m,SET_ARITY(AP,1));
                          PUSHSTACK(S_m1,SET_ARITY(AP,args));
                        }
                      }
                      /* conditional inline zu bearbeiten */
                      L_CONDI(*desc,args) = i;
                      L_CONDI(*desc,ptc) = CODEPTR;        /* code */
                      PUSHSTACK(S_a,desc);
                      GEN_LABEL("jcond",desc);
                      /* then u. else-expr einlagern, die positionen an */
                      /* denen argumente freigegeben werden koennen, werden */
                      /* mit DOLLAR markiert. */
                      px = (T_PTD)x;
                      i = KEEP_FLAG(i) ? 0 : margs;
                      PUSHSTACK(S_e,HASH);                 /* trennsymbol */
                      if (_beta_count_only) {
                        if (!JumpPrefered ^ *R_COND(*px,ptte) >= *R_COND(*px,ptee)) {
#if D_MESS
  if (D_M_C_JFALSE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JFALSE);
#endif
                          GEN_INSTRL(I_JFALSE,"jfalse","cond",desc);
                          load_body((STACKELEM **)A_COND(*px,ptee),i,i);
                          PUSHSTACK(S_e,HASH);             /* trennsymbol */
                          load_body((STACKELEM **)A_COND(*px,ptte),i,i);
                        }
                        else {
#if D_MESS
  if (D_M_C_JTRUE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTRUE);
#endif
                          GEN_INSTRL(I_JTRUE,"jtrue","cond",desc);
                          load_body((STACKELEM **)A_COND(*px,ptte),i,i);
                          PUSHSTACK(S_e,HASH);             /* trennsymbol */
                          load_body((STACKELEM **)A_COND(*px,ptee),i,i);
                        }
                      }
                      else {
#if D_MESS
  if (D_M_C_JCOND)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JCOND);
#endif
                        GEN_INSTRL(I_JCOND,"jcond","cond",desc);
                        load_body((STACKELEM **)A_COND(*px,ptee),i,i);
                        PUSHSTACK(S_e,HASH);             /* trennsymbol */
                        load_body((STACKELEM **)A_COND(*px,ptte),i,i);
                      }
                      PUSHSTACK(S_m,SET_ARITY(COND,2)); /* cond konstruktor */
                      PUSHSTACK(S_m,DOLLAR);
                      /* nfree = 0; */
                      goto main_e;
                    }
                    else {
                      /* conditional nicht in funktionsposition */
                      x = new_cond(x,margs+1);       /* spaetere bearbeitung */
                      L_CONDI(*(T_PTD)x,args) = FREE_ARGS;
                      if (margs > 0) {
                        /* conditional schliessen */
                        for (arity = margs ; --arity > 0 ;) {
#if D_DIST
       if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("4: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, arity)); */
         if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
           UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (arity-(nfree - VAL_INT(MIDSTACK(S_pm, 2))))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */
#if D_MESS
  if (D_M_C_PUSHARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHARG);
#endif
                          GEN_INSTR1(I_PUSHARG,"pusharg",ARG_INDEX(arity));
                        }
                        if (NO_MORE_VARIABLES()) {
                          /* argumente koennen freigegeben werden */
                          i = VALUE(POPSTACK(S_e));
#if D_DIST
      if ((margs-nfree) < 32) { 
/*        DBUG_PRINT ("rear", ("5: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
        if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
          UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (nfree-VAL_INT(MIDSTACK(S_pm, 2))))));  }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */
#if D_MESS
  if (D_M_C_MOVE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE);
#endif
                          GEN_INSTR0(I_MOVE,"move");
                          FREE_INSTR(i-1,nfree);
                        }
                        else {
#if D_DIST
      if ((margs-nfree) < 32) {
/*        DBUG_PRINT ("rear", ("6: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
        if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
          UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (nfree-VAL_INT(MIDSTACK(S_pm, 2))))));  }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */
#if D_MESS
  if (D_M_C_PUSHARG0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHARG0);
#endif
                          GEN_INSTR0(I_PUSHARG0,"pusharg0");
                        }
                      }
#if D_MESS
  if (D_M_C_PUSH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH);
#endif
                      GEN_INSTRX(I_PUSH,"push",x);
                      if (margs > 0) {
#if D_MESS
  if (D_M_C_MKCCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKCCLOS);
#endif
                        GEN_INSTR2(I_MKCCLOS,"mkcclos",margs,margs+1);
                      }
                    }
                    goto main_m;
                    /* end case TY_COND */
      case TY_SUB:                                   /* lambda-funktion      */
                    args = collect_args();           /* argumente aufsammeln */
                    argsneeded = R_FUNC(*desc,nargs);
                    x = new_sub(x,argsneeded);       /* spaetere bearbeitung */
                    goto L_comb;
      case TY_LREC_IND:                              /* definierte funktion  */
                    args = collect_args();           /* argumente aufsammeln */
                    /* analyse des funktionsrumpfes */
                    px = (T_PTD)R_LREC_IND(*desc,ptf);
                    if ((T_POINTER((int)px)) && ((R_DESC(*px,type)) == TY_SUB))
                      argsneeded = R_FUNC(*px,nargs);
                    else argsneeded = 0;
                    x = new_func(x,argsneeded);      /* spaetere bearbeitung */
                  L_comb:
                    /* argumentframe freizugeben? */
                    if (NO_MORE_VARIABLES()) {   /* argumente koennen  */
                      i = VALUE(POPSTACK(S_e));       /* freigegeben werden */
                      FREE_INSTR(i,nfree);
                    }
                    if (args >= argsneeded) {
                      /* ausreichend viele argumente */
                      if (LazyLists && listcnt > 0) {
#if D_MESS
  if (D_M_C_PUSH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH);
#endif
                        GEN_INSTRX(I_PUSH,"push",x);
#if D_MESS
  if (D_M_C_MKBCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKBCLOS);
#endif
                        GEN_INSTR2(I_MKBCLOS,"mkbclos",argsneeded,argsneeded);
                      }
                      else
                      if (args == argsneeded && END_OF_BODY()) {
                        /* tail-end function call */
                        if (_count_reductions)
#ifdef NotUsed
                          if (argsneeded == 0)
#if D_MESS
{ if (D_M_C_JTAIL0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTAIL0);
#endif 
                            GEN_INSTRL(I_JTAIL0,"tail0","func",x);
#if D_MESS
}
#endif
                          else
                          if (argsneeded == 1)
#if D_MESS
{ if (D_M_C_JTAIL1)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTAIL1);
#endif
                            GEN_INSTRL(I_JTAIL1,"tail1","func",x);
#if D_MESS
}
#endif
                          else
#endif /* NotUsed */
#if D_MESS
{ if (D_M_C_JTAIL)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTAIL);
#endif
                            GEN_INSTRL(I_JTAIL,"tail","func",x);
#if D_MESS
}
#endif
                        else
                          if (argsneeded == 0)
#if D_MESS
{ if (D_M_C_JTAILQ0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTAILQ0);
#endif
                            GEN_INSTRL(I_JTAILQ0,"tailq0","func",x);
#if D_MESS
}
#endif
                          else
#ifdef NotUsed
                          if (argsneeded == 1)
#if D_MESS
{ if (D_M_C_JTAILQ1)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTAILQ1);
#endif
                            GEN_INSTRL(I_JTAILQ1,"tailq1","func",x);
#if D_MESS
}
#endif
                          else
#endif /* NotUsed */
#if D_MESS
{ if (D_M_C_JTAILQ)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTAILQ);
#endif
                            GEN_INSTRL(I_JTAILQ,"tailq","func",x);
#if D_MESS
}
#endif
                      }
                      else {
                        /* no tail-end function call */
                        if (_count_reductions)
#ifdef NotUsed
                          if (argsneeded == 0)
#if D_MESS
{ if (D_M_C_BETA0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_BETA0);
#endif
                            GEN_INSTRL(I_BETA0,"beta0","func",x);
#if D_MESS
}
#endif
                          else
                          if (argsneeded == 1)
#if D_MESS
{ if (D_M_C_BETA1)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_BETA1);
#endif
                            GEN_INSTRL(I_BETA1,"beta1","func",x);
#if D_MESS
}
#endif
                          else
#endif /* NotUsed */
#if D_MESS
{ if (D_M_C_BETA)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_BETA);
#endif
                            GEN_INSTRL(I_BETA,"beta","func",x);
#if D_MESS
}
#endif
                        else
                          if (argsneeded == 0)
#if D_MESS
{ if (D_M_C_BETAQ0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_BETAQ0);
#endif
                            GEN_INSTRL(I_BETAQ0,"betaq0","func",x);
#if D_MESS
}
#endif
#ifdef NotUsed
                          else
                          if (argsneeded == 1)
#if D_MESS
{ if (D_M_C_BETAQ1)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_BETAQ1);
#endif
                            GEN_INSTRL(I_BETAQ1,"betaq1","func",x);
#if D_MESS
}
#endif
#endif /* NotUsed */
                          else
#if D_MESS
{ if (D_M_C_BETAQ)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_BETAQ);
#endif
                            GEN_INSTRL(I_BETAQ,"betaq","func",x);
#if D_MESS
}
#endif
                      }
                      if (args > argsneeded) {
                        /* zuviele argumente, apply instruktion */
                        if (LazyLists && listcnt > 0) {
#if D_MESS
  if (D_M_C_MKGCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKGCLOS);
#endif
                          GEN_INSTR1(I_MKGCLOS,"mkgclos",args-argsneeded);
                        }
                        else {
#if D_MESS
  if (D_M_C_APPLY)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_APPLY);
#endif
                          GEN_INSTR1(I_APPLY,"apply",args-argsneeded);
                        }
                      }
                      /* sonstiges nicht moeglich */
                    }
                    else {
                      /* zuwenig argumente, push instruktion */
#if D_MESS
  if (D_M_C_PUSH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH);
#endif
                      GEN_INSTRX(I_PUSH,"push",x);
                      if (args > 0) {
                        /* in funktionsposition, closure bilden */
#if D_MESS
  if (D_M_C_MKBCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKBCLOS);
#endif
                        GEN_INSTR2(I_MKBCLOS,"mkbclos",args,argsneeded);
                      }
                    }
                    goto main_m;
                    /* end case TY_COMB */
      case TY_EXPR:                                  /* expression pointer */
                    /* argumentframe freizugeben? */
                    if (NO_MORE_VARIABLES()) {
                      /* argumente koennen freigegeben werden */
                      i = VALUE(POPSTACK(S_e));
                      FREE_INSTR(i,nfree);
                    }
                    load_expr((STACKELEM **)A_EXPR(*desc,pte));
                    goto main_e;
      case TY_SWITCH:
                    args = collect_args();
                    x = new_case(x,margs+1);          /* spaetere bearbeitung */
                    if (args >= (int) R_SWITCH(*R_CASE(*(T_PTD)x,ptd),case_type)) {
                               /* auf int umgecastet von RS 04/11/92 */ 
                      /* case in funktionsposition und ausreichend viele argumente */
#if D_MESS
  if (D_M_C_CASE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_CASE);
#endif
                      GEN_INSTRX(I_CASE,"case",x);
                      if (NO_MORE_VARIABLES()) {
                        /* argumente koennen freigegeben werden */
                        i = VALUE(POPSTACK(S_e));
                        FREE_INSTR(i,nfree);
                      }
                      /* NCASE: bei multiplen pattern zusaeztliche argumente */
                      i = R_CASE(*(T_PTD)x,nargs) - (R_CASE(*(T_PTD)x,args) ? margs : 0);
                      if (args > i) {
                        /* zuviele argumente, apply instruktion */
#if D_MESS
  if (D_M_C_APPLY)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_APPLY);
#endif
                        GEN_INSTR1(I_APPLY,"apply",args-i);
                      }
                    }
                    else {
                      /* case erhaelt nicht ausreichend viele argumente, */
                      /* bzw. ist nicht in funktionsposition */

DBUG_PRINT ("EAR", ("args: %d, margs: %d, R_CASE(*(T_PTD)x,args): %d,  R_CASE(*(T_PTD)x,nargs): %d", args, margs, R_CASE(*(T_PTD)x,args),  R_CASE(*(T_PTD)x,nargs)));

                      if (margs > 0 && R_CASE(*(T_PTD)x,args)) { 

                        /* falls case rel. freie var enthaelt schliessen */
                        for (arity = margs ; --arity > 0 ;) {
#if D_DIST       
       if ((margs-nfree) < 32) {
/*         DBUG_PRINT ("rear", ("7: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, arity)); */
         if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
           UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (arity-(nfree-VAL_INT(MIDSTACK(S_pm, 2))))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */
#if D_MESS
  if (D_M_C_PUSHARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHARG);
#endif
                          GEN_INSTR1(I_PUSHARG,"pusharg",ARG_INDEX(arity));
                        }
                        if (NO_MORE_VARIABLES()) {
                          /* argumente koennen freigegeben werden */

DBUG_PRINT ("rear", ("NO_MORE_VARIABLES !"));

                          i = VALUE(POPSTACK(S_e));
#if D_DIST      
       if ((margs-nfree) < 32) {
/*         DBUG_PRINT ("rear", ("8: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
         if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
           UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (nfree-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */
#if D_MESS
  if (D_M_C_MOVE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE);
#endif
                          GEN_INSTR0(I_MOVE,"move");
                          FREE_INSTR(i-1,nfree);
                        }
                        else {

DBUG_PRINT ("rear", ("NO_MORE_VARIABLES war falsch! "));

#if D_DIST       
       if ((margs-nfree) < 32) {
/*         DBUG_PRINT ("rear", ("9: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
         if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
           UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (nfree-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */
#if D_MESS
  if (D_M_C_PUSHARG0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHARG0);
#endif
                          GEN_INSTR0(I_PUSHARG0,"pusharg0");
                        }
#if D_MESS
  if (D_M_C_PUSH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH);
#endif
                        GEN_INSTRX(I_PUSH,"push",x);
                        /* GEN_INSTR2(I_MKSCLOS,"mksclos",margs,margs+1); */

#if D_MESS
  if (D_M_C_MKSCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKSCLOS);
#endif
                        GEN_INSTR2(I_MKSCLOS,"mksclos",margs,R_CASE(*(T_PTD)x,nargs));  /* NCASE 09.09.92 */
                      }
                      else {

DBUG_PRINT ("EAR", ("Ich bin leider hier...."));

#if D_MESS
  if (D_M_C_PUSH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH);
#endif
                        GEN_INSTRX(I_PUSH,"push",x);

DBUG_PRINT ("rear", ("i have (T_AP(READSTACK(S_m)) = %d, ARITY(READSTACK(S_m))) = %d", T_AP(READSTACK(S_m)), ARITY(READSTACK(S_m))));

/* RS 11.5.1993 */
/*                      if (T_AP(READSTACK(S_m)) && (ARITY(READSTACK(S_m)))) */
                        GEN_INSTR2(I_MKSCLOS,"mksclos",args,R_CASE(*(T_PTD)x,nargs));
/* end of RS 11.5.1993 */

DBUG_PRINT ("rear", ("Jetzt bin ich vorbei !"));

                      }
                    }
                    goto main_m;
                    /* end case TY_SWITCH */

      case TY_REC:  POST_MORTEM("ear: rec not implemented");

#if    UH_ZF_PM
                    /* ullis zf-expression und pm-behandlung ! */
/*uh, 16.7.90*/  
/*uh, 16.7.90*/   case TY_ZF: /* Ein ZF-Descriptor. Ist der Pointer to Variable Names NIL, so liegt ein Filter vor */
/*uh, 16.7.90*/               /* sonst ein Generator. */
/*uh, 16.7.90*/               if (R_ZF(*desc,special)== ZF_START) { /* ZF-Start-Descriptor */
/*uh, 16.7.90*/                  GEN_INSTR0(I_BEGINZF,"beginzf");
/*uh, 16.7.90*/                  PUSHSTACK(S_m,zfc);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,vars);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,zfd);
/*uh, 16.7.90*/                  zfc=R_ZF(*desc,pte);
/*uh, 16.7.90*/                  vars=R_ZF(*desc,namelist);
/*uh, 16.7.90*/                  zfd=desc;
/*uh, 16.7.90*/                  PUSHSTACK(S_m,SET_ARITY(ZF_UH,nr_additionals));
/*uh, 16.7.90*/                  nr_additionals = 0;
/*uh, 16.7.90*/                  PUSHSTACK(S_m,SET_ARITY(ZFINT_UH,0));
/*uh, 16.7.90*/                  load_body((STACKELEM **)A_FUNC(*desc,pte),margs,0);
/*uh, 16.7.90*/                  goto main_e;
/*uh, 16.7.90*/               }
/*uh, 16.7.90*/               POPSTACK(S_m); POPSTACK(S_m1); /* Applikator entfernen. */
/*uh, 16.7.90*/               POPSTACK(S_m); /* ZFINT fuer Rumpf entfernen. */
/*uh, 16.7.90*/               if (R_ZF(*desc,namelist)==(PTR_HEAPELEM)0) {
/*uh, 16.7.90*/                  /* Filter */
/*uh, 16.7.90*/                  FORWARDMARK(fi_desc1);
/*uh, 16.7.90*/                  GEN_INSTRL(I_TFALSE,"testfalse","fi",fi_desc1);
/*uh, 16.7.90*/                  FORWARDMARK(if_desc);
/*uh, 16.7.90*/                  GEN_INSTRL(I_TTRUE,"testtrue","if",if_desc);
/*uh, 16.7.90*/                  /* Code-Descriptor erzeugen und in den Code einflechten */
/*uh, 16.7.90*/                  MAKEDESC(cdesc,1,C_EXPRESSION,TY_ZFCODE);
/*uh, 16.7.90*/                  /* Graphfragment in Codedescriptor eintragen */
/*uh, 16.7.90*/                  L_ZFCODE(*cdesc,zfbound)=nr_additionals;
/*uh, 16.7.90*/                  L_ZFCODE(*cdesc,nargs)=margs;
/*uh, 16.7.90*/                  L_ZFCODE(*cdesc,ptd)=desc;
/*uh, 16.7.90*/                  INC_REFCNT(desc);
/*uh, 16.7.90*/                  L_ZFCODE(*cdesc,varnames)=vars;
/*uh, 16.7.90*/                  PUSHSTACK(S_m,zfc);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,vars);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,zfd);
/*uh, 16.7.90*/                  zfc=R_ZF(*desc,pte);
/*uh, 16.7.90*/                  vars=R_ZF(*desc,namelist);
/*uh, 16.7.90*/                  zfd=desc;
/*uh, 16.7.90*/                  GEN_INSTR1(I_MAKEBOOL,"makebool",cdesc);
/*uh, 16.7.90*/                  FORWARDMARK(fi_desc2);
/*uh, 16.7.90*/                  GEN_INSTRL(I_JUMP,"jump","fi",fi_desc2);
/*uh, 16.7.90*/                  /* LABEL IF erzeugen */
/*uh, 16.7.90*/                  FORWARDRESOLVE("if",if_desc);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,fi_desc1);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,fi_desc2);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,SET_ARITY(ZFINT_UH,1));
/*uh, 16.7.90*/                  PUSHSTACK(S_m,SET_ARITY(ZFINT_UH,0)); /* Fuer Rumpf */
/*uh, 16.7.90*/                  PUSHSTACK(S_e,HASH);                 /* trennsymbol */
/*uh, 16.7.90*/                  load_body((STACKELEM **)A_FUNC(*desc,pte),margs,0);
/*uh, 16.7.90*/                  goto main_e;
/*uh, 16.7.90*/               }
/*uh, 16.7.90*/               /* Ein generator  - precode generieren*/
/*uh, 16.7.90*/                  PUSHSTACK(S_m,zfc);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,vars);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,zfd);
/*uh, 16.7.90*/                  zfc=R_ZF(*desc,pte);
/*uh, 16.7.90*/                  vars=R_ZF(*desc,namelist);
/*uh, 16.7.90*/                  zfd=desc;
/*uh, 16.7.90*/                  nr_additionals++;
/*uh, 16.7.90*/                  margs++;                  /* uh 11.4.92 */
/*uh, 16.7.90*/                  FORWARDMARK(listerr_desc);
/*uh, 16.7.90*/                  GEN_INSTRL(I_TLIST,"testlist","listerr",listerr_desc);
/*uh, 16.7.90*/                  GEN_INSTR0(I_DIM,"dimension");
/*uh, 16.7.90*/                  /* generiere Label FOR: */
/*uh, 16.7.90*/                  BACKWARDMARK("for",for_desc);
/*uh, 16.7.90*/                  FORWARDMARK(rof_desc);
/*uh, 16.7.90*/                  GEN_INSTRL(I_TZERO,"testzero","rof",rof_desc);
/*uh, 16.7.90*/                        GEN_INSTR0(I_EXTRACT,"extract");
/*uh, 16.7.90*/                  PUSHSTACK(S_m,listerr_desc);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,rof_desc);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,for_desc);
/*uh, 16.7.90*/                  PUSHSTACK(S_m,SET_ARITY(ZFINT_UH,2));
/*uh, 16.7.90*/                  PUSHSTACK(S_m,SET_ARITY(ZFINT_UH,0)); /* Fuer Rumpf */
/*uh, 16.7.90*/                  PUSHSTACK(S_e,HASH);                 /* trennsymbol */
/*uh, 16.7.90*/                  load_body((STACKELEM **)A_FUNC(*desc,pte),margs,0);
/*uh, 16.7.90*/                  goto main_e;
/*uh, 16.7.90*/  
#endif /* UH_ZF_PM */

      default:
                    sprintf(buf,"ear: (2) unexpected pointer %08x on stack e (type %x)",(unsigned int)desc,R_DESC(*desc,type));
                    POST_MORTEM(buf);
    } /* end switch */
  } /* end T_POINTER */

  /* argumentframe freizugeben? */
  if (NO_MORE_VARIABLES()) {
    /* argumente koennen freigegeben werden */
    i = VALUE(POPSTACK(S_e));
    FREE_INSTR(i,nfree);
  }

  if T_CON(x) {                                      /* konstruktor auf e  */


    if (T_LREC(x)) {                                 /* letrec konstruktor */
      del_expr(&S_e);                                /* loesche p_lrec     */
      goto main_e;                                   /* startausdruck      */
    }

    if T_SNAP(x) {                                   /* snap wird zu ap !  */
      PUSHSTACK(S_m ,SET_ARITY(AP,ARITY(x)));
      PUSHSTACK(S_m1,READSTACK(S_m));
      goto main_e;
    }

    if (T_K_COMM(x)) {
      PPOPSTACK(S_e);  /* kommentar */
      goto main_e;
    }

#if    UH_ZF_PM
    /* ullis 2. zf-expression und pm-behandlung ! */
/*uh, 16.7.90*/      if (T_ZF_UH(x)) {
/*uh, 16.7.90*/                post_mortem("ZF-Konstruktor auf E: THIS SHOULD NEVER HAPPEN!");
/*uh, 16.7.90*/                { GEN_INSTR0(I_BEGINZF,"beginzf");
/*uh, 16.7.90*/                PUSHSTACK(S_m,SET_ARITY(x,nr_additionals));
/*uh, 16.7.90*/                nr_additionals = 0;
/*uh, 16.7.90*/                PUSHSTACK(S_m,SET_ARITY(ZFINT_UH,0)); }
/*uh, 16.7.90*/         goto main_e;
/*uh, 16.7.90*/      }
/*uh, 16.7.90*/  
#
#endif /* UH_ZF_PM */

    /* alle sonstigen Konstruktoren: ap, list */

    if (ARITY(x) > 0) {

#if D_CHECKFRAME
  if (T_AP(x)) 
    GEN_INSTR1(I_CHKFRAME, "chkframe", ARITY(x));
#endif /* D_CHECKFRAME */

#if D_DIST
  if (T_DAP(x)) {         /*  DAP-Constructor on Top  ? */

    PUSHSTACK(S_m, x);    /* normal Constructor-Pushes */
    PUSHSTACK(S_m1, x);
   
    DBUG_PRINT ("EAR", ("Number of Expressions not to distribute: %d !", d_nodistnr));

    if ((ARITY(x)-d_nodistnr) >= 2) {         /* arity-test for distribution */
     DBUG_PRINT ("ear", ("Insert PUSHH for distribute...."));
#if D_MESS
  if (D_M_C_PUSHH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHH);
#endif
     GEN_INSTR1(I_PUSHH, "pushh", ARITY(x)-1);                   /* insert pushh */

/* for ((margs-nfree) < 32) insert distb, otherwise dist !!!! */

#if DEBUG
     if ((margs-nfree) > 31)
        fprintf(codefp, "/*** following dist at %1x: ***/\n", (int)CODEPTR);             /* for debug only */
     else
        fprintf(codefp, "/*** following distb at %1x: ***/\n", (int)CODEPTR);
#endif

     if ((margs-nfree) > 31)       /* more than 31 arguments in frame ? So dist ! */
#if D_MESS
{ if (D_M_C_DIST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DIST);
#endif 
       GEN_INSTR4(I_DIST, "dist", 0, 0, (margs-nfree), nfree);     /* insert dist , all arguments dummy entries except of (margs-nfree) */
#if D_MESS
}
#endif
     else
#if D_MESS
{ if (D_M_C_DISTB)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DISTB);
#endif
       GEN_INSTR4(I_DISTB, "distb", 0, 0, (margs-nfree), nfree);     /* insert distb , all arguments dummy entries except of (margs-nfree) */
#if D_MESS
}
#endif

     *(CODEPTR-4) = (INSTR) CODEPTR;                             /* updating first argument: label to code for distribute */
     GEN_LABEL("distarg", CODEPTR);

     if ((margs-nfree) < 32) {
       PUSHSTACK (S_pm, TAG_INT(nfree));
       PUSHSTACK (S_pm, TAG_INT(0)); }            /* mark here used arguments */

     PUSHSTACK (S_pm, (STACKELEM) ( (int)(CODEPTR-5) << 4));                  /* pushing update-address in S_pm */
     }

    } else {
#endif /* D_DIST */

      PUSHSTACK(S_m,x);                              /* konstruktor auf m */
      PUSHSTACK(S_m1,x);                             /* und auf m1        */
      if (T_LIST(x)) {
        ++listcnt;
      }
#if D_DIST       /* for C-syntax */
    }
#endif /* D_DIST */

      goto main_e;                                   /* weiter auf e      */
    }
    else {
      /* nullstelliger konstruktor, koennte akzeptiert werden, aber */
      POST_MORTEM("ear: nullary constructor on stack e");
    }
  } /* ende von T_CON */

  if (T_FUNC(x)) {                                   /* primitive funktion   */
    args = collect_args();                           /* argumente aufsammeln */
    argsneeded = FUNC_ARITY(x);                      /* benoetigte argumente */
    if (args == 0) {
      /* nicht in funktionsposition, push instruktion */
      /* folgt return? */
      if (END_OF_BODY()) {
        /* return folgt, optimierung */
#if D_MESS
  if (D_M_C_RTC)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTC);
#endif
        GEN_INSTRN(I_RTC,"rtc",x);
        gen_return = 0;
      }
      else {
#if D_MESS
  if (D_M_C_PUSHC)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC);
#endif
        GEN_INSTRN(I_PUSHC,"pushc",x);
      }
      goto main_m;                                   /* weiter auf m   */
    }
    /* in funktionsposition (args > 0) */
    if (args < argsneeded) {
      /* zuwenig argumente, push instruktion */
#if D_MESS
  if (D_M_C_PUSHC)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC);
#endif
      GEN_INSTRN(I_PUSHC,"pushc",x);
      /* closure bilden */
#if D_MESS
  if (D_M_C_MKDCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKDCLOS);
#endif
      GEN_INSTR2(I_MKDCLOS,"mkdclos",args,argsneeded);
      goto main_m;                                     /* weiter auf m       */
    }
    else {
      /* ausreichend viele argumente vorhanden */
      switch(argsneeded) {
        case 1:
                 if (_beta_count_only) {
                   if (T_M_NEG(x))   { 
#if D_MESS
  if (D_M_C_NEG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_NEG);
#endif
                                       GEN_INSTR0(I_NEG,"neg"); } else
                   if (T_M_NOT(x))   { 
#if D_MESS
  if (D_M_C_NOT)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_NOT);
#endif
                                       GEN_INSTR0(I_NOT,"not"); } else
                   /* dg 06.05.91 cons-listen */
                   if (T_M_FIRST(x)) { 
#if D_MESS
  if (D_M_C_FIRST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FIRST);
#endif
                                       GEN_INSTR0(I_FIRST,"first"); } else
                   if (T_M_REST(x))  { 
#if D_MESS
  if (D_M_C_REST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_REST);
#endif
                                       GEN_INSTR0(I_REST,"rest"); } else
                   { 
#if D_MESS
  if (D_M_C_DELTA1)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DELTA1);
#endif
                     GEN_INSTRN(I_DELTA1,"delta1",x); }
                 }
                 else {
#if D_MESS
  if (D_M_C_DELTA1)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DELTA1);
#endif
                   GEN_INSTRN(I_DELTA1,"delta1",x);
                 }
                 break;
        case 2:
        /* inline: */
                 if (T_LSELECT(x)) {
                   if (LazyLists && listcnt > 0) {
#if D_MESS
  if (D_M_C_PUSHC)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC);
#endif
                     GEN_INSTRN(I_PUSHC,"pushc",LSELECT);
#if D_MESS
  if (D_M_C_MKDCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKDCLOS);
#endif
                     GEN_INSTR2(I_MKDCLOS,"mkdclos",2,2);
                   }
                   else
#if D_MESS
{ if (D_M_C_LSEL)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_LSEL);
#endif
                     GEN_INSTR0(I_LSEL,"lsel");
#if D_MESS
}
#endif
                 }
                 else
                 if (_beta_count_only) {
                   if (T_D_PLUS(x))  { 
#if D_MESS
  if (D_M_C_ADD)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_ADD);
#endif
                                       GEN_INSTR0(I_ADD,"add"); } else
                   if (T_D_MINUS(x)) { 
#if D_MESS
  if (D_M_C_SUB)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_SUB);
#endif
                                       GEN_INSTR0(I_SUB,"sub"); } else
                   if (T_D_MULT(x))  { 
#if D_MESS
  if (D_M_C_MUL)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MUL);
#endif
                                       GEN_INSTR0(I_MUL,"mul"); } else
                   if (T_D_DIV(x))   { 
#if D_MESS
  if (D_M_C_DIV)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DIV);
#endif
                                       GEN_INSTR0(I_DIV,"div"); } else
                   if (T_D_MOD(x))   { 
#if D_MESS
  if (D_M_C_MOD)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOD);
#endif
                                       GEN_INSTR0(I_MOD,"mod"); } else
                   if (T_D_OR(x))    { 
#if D_MESS
  if (D_M_C_OR)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_OR);
#endif
                                       GEN_INSTR0(I_OR,"or");   } else
                   if (T_D_AND(x))   { 
#if D_MESS
  if (D_M_C_AND)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_AND);
#endif
                                       GEN_INSTR0(I_AND,"and"); } else
                   if (T_D_XOR(x))   { 
#if D_MESS
  if (D_M_C_XOR)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_XOR);
#endif
                                       GEN_INSTR0(I_XOR,"xor"); } else
                   if (T_D_EQ(x))    { 
#if D_MESS
  if (D_M_C_EQ)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_EQ);
#endif
                                       GEN_INSTR0(I_EQ,"eq");   } else
                   if (T_D_NEQ(x))   { 
#if D_MESS
  if (D_M_C_NE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_NE);
#endif
                                       GEN_INSTR0(I_NE,"ne");   } else
                   if (T_D_LESS(x))  { 
#if D_MESS
  if (D_M_C_LT)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_LT);
#endif
                                       GEN_INSTR0(I_LT,"lt");   } else
                   if (T_D_LEQ(x))   { 
#if D_MESS
  if (D_M_C_LE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_LE);
#endif
                                       GEN_INSTR0(I_LE,"le");   } else
                   if (T_D_GREAT(x)) { 
#if D_MESS
  if (D_M_C_GT)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_GT);
#endif
                                       GEN_INSTR0(I_GT,"gt");   } else
                   if (T_D_GEQ(x))   { 
#if D_MESS
  if (D_M_C_GE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_GE);
#endif
                                       GEN_INSTR0(I_GE,"ge");   } else
                   /* dg 06.05.91 cons-listen */
                   if (T_D_CONS(x))  { 
#if D_MESS
  if (D_M_C_FCONS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FCONS);
#endif
                                       GEN_INSTR0(I_FCONS,"fcons"); } else
                   { 
#if D_MESS
  if (D_M_C_DELTA2)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DELTA2);
#endif
                     GEN_INSTRN(I_DELTA2,"delta2",x); }
                 }
                 else {
#if D_MESS
  if (D_M_C_DELTA2)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DELTA2);
#endif
                   GEN_INSTRN(I_DELTA2,"delta2",x);
                 }
                 break;
        case 3:  
#if D_MESS
  if (D_M_C_DELTA3)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DELTA3);
#endif
                 GEN_INSTRN(I_DELTA3,"delta3",x); break;
        case 4:  
#if D_MESS
  if (D_M_C_DELTA4)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DELTA4);
#endif
                 GEN_INSTRN(I_DELTA4,"delta4",x); break;
        default: POST_MORTEM("ear: number of pf-args out of range");
      } /* end switch */
    }
    if (args > argsneeded) {
      /* zuviele argumente, apply instruktion */
#if D_MESS
  if (D_M_C_APPLY)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_APPLY);
#endif
      GEN_INSTR1(I_APPLY,"apply",args-argsneeded);
    }
    goto main_m;                                     /* weiter auf m       */
  } /* end T_FUNC */

  /* sonstige stackelemente auf dem e-stack, push instruktion */
  /* folgt return? */
  if (END_OF_BODY()) {
    /* return folgt, optimierung */
#if D_MESS
  if (D_M_C_RTC)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTC);
#endif
    GEN_INSTRN(I_RTC,"rtc",x);
    gen_return = 0;
  }
  else {
#if D_MESS
  if (D_M_C_PUSHC)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC);
#endif
    GEN_INSTRN(I_PUSHC,"pushc",x);
  }
  goto main_m;
  /* end e-line */

  main_m:                 /* --- m-linie --- */

  x = READSTACK(S_m);                                /* top lesen */

  /* argumentframe freizugeben? */
  if (NO_MORE_VARIABLES()) {                /* argumente koennen  */
    i = VALUE(POPSTACK(S_e));                    /* freigegeben werden */
    FREE_INSTR(i,nfree);
  }

  if (T_CON(x)) {
    /* kostruktor auf dem m-stack */
#if    UH_ZF_PM
    /* ullis 3. zf-expression und pm-behandlung ! */
/*uh, 16.7.90*/   if (T_ZF_UH(x)) {
/*uh, 16.7.90*/     nr_additionals = ARITY(POPSTACK(S_m));
/*uh, 16.7.90*/     GEN_INSTR0(I_ENDZF,"endzf");
/*uh, 16.7.90*/     zfd=(T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/     vars=(PTR_HEAPELEM)POPSTACK(S_m);
/*uh, 16.7.90*/     zfc=(PTR_HEAPELEM)POPSTACK(S_m);
/*uh, 16.7.90*/     goto main_m;
/*uh, 16.7.90*/   }
/*uh, 16.7.90*/  
/*uh, 16.7.90*/   if ( T_ZFINT_UH(x) ) {
/*uh, 16.7.90*/     POPSTACK(S_m);
/*uh, 16.7.90*/     if (ARITY(x)==1) {
/*uh, 16.7.90*/       /* Ein Filter */
/*uh, 16.7.90*/       /* Filter postcode */
/*uh, 16.7.90*/       /* Generieren und aufloesen von Label FI */
/*uh, 16.7.90*/       fi_desc2 = (T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       fi_desc1 = (T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       FORWARDRESOLVE("fi",fi_desc1);
/*uh, 16.7.90*/       FORWARDRESOLVE("fi",fi_desc2);
/*uh, 16.7.90*/       POPSTACK(S_e);  /* Trennsymbol */
/*uh, 16.7.90*/       zfd=(T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       vars=(PTR_HEAPELEM)POPSTACK(S_m);
/*uh, 16.7.90*/       zfc=(PTR_HEAPELEM)POPSTACK(S_m);
/*uh, 16.7.90*/       goto main_m;
/*uh, 16.7.90*/     }
/*uh, 16.7.90*/     if (ARITY(x)==2) {
/*uh, 16.7.90*/       /* Also Stelligkeit 2, ein Generator */
/*uh, 16.7.90*/       /* Generator postcode */
#if D_MESS
  if (D_M_C_FREE1)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE1);
#endif
/*uh, 16.7.90*/       GEN_INSTR0(I_FREE1,"free1");
/*uh, 16.7.90*/       nr_additionals--;
/*uh, 16.7.90*/       margs--;
/*uh, 16.7.90*/       for_desc = (T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/             rof_desc = (T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       listerr_desc = (T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       GEN_INSTRL(I_JUMP,"jump","for",for_desc);
/*uh, 16.7.90*/       BACKWARDRESOLVE(for_desc);
/*uh, 16.7.90*/       /* Generieren und aufloesen von Label LISTERR */
/*uh, 16.7.90*/       FORWARDRESOLVE("listerr",listerr_desc);
/*uh, 16.7.90*/       /* Code-Descriptor erzeugen und in den Code einflechten */
/*uh, 16.7.90*/  
/*uh, 16.7.90*/       MAKEDESC(cdesc,1,C_EXPRESSION,TY_ZFCODE);
/*uh, 16.7.90*/       /* Graphfragment in Codedescriptor eintragen */
/*uh, 16.7.90*/       L_ZFCODE(*cdesc,zfbound)=nr_additionals;
/*uh, 16.7.90*/       L_ZFCODE(*cdesc,nargs)=margs;
/*uh, 16.7.90*/       L_ZFCODE(*cdesc,ptd)=zfd;
/*uh, 16.7.90*/       INC_REFCNT(desc);
/*uh, 16.7.90*/       L_ZFCODE(*cdesc,varnames)=vars;
/*uh, 16.7.90*/  
/*uh, 16.7.90*/       GEN_INSTR1(I_MAKEZF,"makezflist",cdesc);
/*uh, 16.7.90*/       FORWARDMARK(end_desc);
/*uh, 16.7.90*/       GEN_INSTRL(I_JUMP,"jump","end",end_desc);
/*uh, 16.7.90*/       /* Generieren und aufloesen von Label ROF */
/*uh, 16.7.90*/       FORWARDRESOLVE("rof",rof_desc);
/*uh, 16.7.90*/       GEN_INSTR0(I_DROPP,"dropp");
/*uh, 16.7.90*/       /* Generieren und aufloesen von Label END */
/*uh, 16.7.90*/       FORWARDRESOLVE("end",end_desc);
/*uh, 16.7.90*/       POPSTACK(S_e);  /* Trennsymbol */
/*uh, 16.7.90*/       zfd=(T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       vars=(PTR_HEAPELEM)POPSTACK(S_m);
/*uh, 16.7.90*/       zfc=(PTR_HEAPELEM)POPSTACK(S_m);
/*uh, 16.7.90*/       goto main_m;
/*uh, 16.7.90*/     }
/*uh, 16.7.90*/     /* Stelligkeit 0, Rumpfausdruck zu Ende */
/*uh, 16.7.90*/     GEN_INSTR0(I_APPEND,"append");
/*uh, 16.7.90*/     /* zfc=(PTR_HEAPELEM)POPSTACK(S_m); */
/*uh, 16.7.90*/     goto main_m;
/*uh, 16.7.90*/   }
/*uh, 16.7.90*/  
/*uh, 16.7.90*/  
/*uh, 16.7.90*/  
#endif /* UH_ZF_PM */
    arity = ARITY(x);                                /* stelligkeit          */
    gen_return = 1;                                  /* return zulassen */

/*************************************************DIST*************************************************/

#if D_DIST         /* end of code generation for distribution */
    if (T_DAP(READSTACK(S_m))) {         /* great, distributed AP (LETPAR) constructor */
      if ((arity-d_nodistnr) > 2) {            /* there are still more arguments to distribute !? */
#if D_MESS
  if (D_M_C_PROC_DISTEND)
    GEN_INSTR0(I_MSDISTEND, "msdistend");
  if (D_M_C_DISTEND)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DISTEND);
#endif
        GEN_INSTR0(I_DISTEND, "dist_end");   /* oh, there are ! */
        yeah = (int) (READSTACK(S_pm)) >> 4;        /* get old dist address in codevector */
        *((INSTR *)yeah+2) = (INSTR) CODEPTR;           /* updating label (argument 2 in dist) */
        if (nfree > ((int) *((INSTR *)yeah+4)))      /* arguments have been released ? */
           *((INSTR *)yeah+4) = nfree -  ((int) *((INSTR *)yeah+4));         /* yo, updating argument 4 for released arguments */
        else
           *((INSTR *)yeah+4) = 0;                                           /* no, reset argument 4 */

        if (((margs-nfree) < 32) && (*((INSTR *)yeah) == I_DISTB)) {
        *((INSTR *)yeah+3) = VAL_INT(MIDSTACK(S_pm, 1));
        if ((SIZEOFSTACK(S_pm) > 3) && (*((INSTR *)((int) (MIDSTACK(S_pm,3)) >> 4)) == I_DISTB))      /* more than one dap-entry on S_pm ? */
          UPDATESTACK (S_pm, 4, TAG_INT(VAL_INT(MIDSTACK(S_pm, 4)) | (VAL_INT(MIDSTACK(S_pm, 1)) << (VAL_INT(MIDSTACK(S_pm, 2)) - VAL_INT(MIDSTACK(S_pm, 5)) )) )); }  /* updating entry below */

#if DEBUG           /* showing updated dist-arguments */
        if (((margs-nfree) > 31) || (*((INSTR *)yeah) == I_DIST))
          fprintf(codefp,"/*** updating dist at %1x to : dist(%1x, %1x, %1d, %1d); ***/\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4));
        else
          fprintf(codefp,"/*** updating distb at %1x to : distb(%1x, %1x, %1x, %1d); ***/\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4));
#endif /* DEBUG */

        GEN_LABEL ("distend", CODEPTR);

#if DEBUG
        if ((margs-nfree) > 31)
          fprintf(codefp, "/*** following dist at %1x: ***/\n", (unsigned int)CODEPTR);             /* for debug only */
        else
          fprintf(codefp, "/*** following distb at %1x: ***/\n", (unsigned int)CODEPTR);
#endif /* DEBUG */

        if ((margs-nfree) > 31)
#if D_MESS
{ if (D_M_C_DIST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DIST);
#endif
          GEN_INSTR4(I_DIST, "dist", 0, 0, (margs-nfree), nfree);              /* whow, the next dist ! */ 
#if D_MESS
}
#endif
        else  
#if D_MESS
{ if (D_M_C_DISTB)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DISTB);
#endif
          GEN_INSTR4(I_DISTB, "distb", 0, 0, (margs-nfree), nfree);              /* whow, the next distb ! */
#if D_MESS
}
#endif

        *(CODEPTR-4) = (INSTR) CODEPTR;                                      /* updating argument 1 */
        GEN_LABEL("distarg", CODEPTR);                                       /* label for debug */
        hooray=POPSTACK (S_pm);                                                     /* pop old S_pm entry */       

        if ((margs-nfree) < 32) {
          hooray=POPSTACK (S_pm);           /* update underlying entry here... */
          UPDATESTACK (S_pm, 0, TAG_INT(nfree));  /* backup actual nfree here */

          PUSHSTACK (S_pm, TAG_INT(0));  }     /* mark here used arguments */

        PUSHSTACK (S_pm, (STACKELEM) ((int)(CODEPTR-5) << 4));                           /* push argument 2 address on stack S_pm */

        }
      else if ((arity-d_nodistnr) == 2) {      /* ****************** last expression to be distributed */ 
#if D_MESS
  if (D_M_C_PROC_DISTEND)
    GEN_INSTR0(I_MSDISTEND, "msdistend");
  if (D_M_C_DISTEND)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DISTEND);
#endif
        GEN_INSTR0(I_DISTEND, "dist_end");
        yeah = (int)(READSTACK(S_pm)) >> 4;            /* get address of dist in codevector */
        *((INSTR *)yeah+2) = (INSTR) CODEPTR;           /* updating label (argument 2 in dist) */
        if (nfree > ((int) *((INSTR *)yeah+4)))      /* arguments have been released ? */
           *((INSTR *)yeah+4) = nfree -  ((int) *((INSTR *)yeah+4));        /* yo, updating argument 4 for released arguments */
        else
           *((INSTR *)yeah+4) = 0;              /* no, reset argument 4 */

        if (((margs-nfree) < 32) && (*((INSTR *)yeah) == I_DISTB)) {
          *((INSTR *)yeah+3) = VAL_INT(MIDSTACK(S_pm, 1));
          if ((SIZEOFSTACK(S_pm) > 3) && (*((INSTR *)((int) (MIDSTACK(S_pm,3)) >> 4)) == I_DISTB))      /* more than one dap-entry on S_pm ? */
            UPDATESTACK (S_pm, 4, TAG_INT(VAL_INT(MIDSTACK(S_pm, 4)) | (VAL_INT(MIDSTACK(S_pm, 1)) << (VAL_INT(MIDSTACK(S_pm, 2)) - VAL_INT(MIDSTACK(S_pm, 5)) )) )); } /* updating underlying entry */

#if DEBUG           /* showing updated dist-arguments */
        if (((margs-nfree) > 31) || (*((INSTR *)yeah) == I_DIST))
          fprintf(codefp,"/*** updating dist at %1x to : dist(%1x, %1x, %1d, %1d); ***/\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1),  *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4));
        else
          fprintf(codefp,"/*** updating distb at %1x to : distb(%1x, %1x, %1x, %1d); ***/\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4));
#endif /* DEBUG */

        GEN_LABEL ("distend", CODEPTR);          /* debug */
        hooray=POPSTACK (S_pm);                         /* remove S_pm entry */

        if ((margs-nfree) < 32) {
          hooray=POPSTACK (S_pm);         
          hooray=POPSTACK (S_pm); }

#if D_MESS
  if (D_M_C_PROC_NODIST)
        if (arity != 2)
          GEN_INSTR0(I_MSNODIST, "nodist");
#endif

        }  
#if D_MESS
  else {
    if (D_M_C_PROC_NODIST)
      if (arity != 2)
        GEN_INSTR0(I_MSNODIST, "nodist"); 
    }
#endif

      DBUG_PRINT ("EAR", ("Yeah, arity = %d, (ARITY(READSTACK(S_m1))-d_nodistnr = %d", arity, (ARITY(READSTACK(S_m1))-d_nodistnr)));

      if ((arity == 2) && ((ARITY(READSTACK(S_m1))-d_nodistnr) > 1)) {   /* Dapplication with argument to be distributed (arity was > 2) */
#if D_MESS
  if (D_M_C_WAIT)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_WAIT);
#endif
        GEN_INSTR1(I_WAIT, "wait", d_nodistnr);          /* inserting wait and poph */
#if D_MESS
  if (D_M_C_POPH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_POPH);
#endif
        GEN_INSTR0(I_POPH, "poph");
        }
      }
#endif  /* D_DIST */

/************************************************DIST*************************************************/

    if (arity > 1) {                                 /* stelligkeit > 1    */
      WRITE_ARITY(S_m,arity - 1);                    /* stelligkeit runter */
      goto main_e;                                   /* weiter auf e       */
    }
    /* arity == 1 */

    if (T_AP(x))
                 {                                   /* applikator         */
      arity = collect_args();                        /* argumente sammeln  */

      /* der ausdruck in fkt.pos. ist keine fkt., also mkap instruktion    */
#if D_MESS
  if (D_M_C_MKAP)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKAP);
#endif
      GEN_INSTR1(I_MKAP,"mkap",arity);
    }
    else
    if (T_LIST(x)) {                                 /* listenkonstruktor  */
      PPOPSTACK(S_m);                                /* weg damit (von m)  */
      x = POPSTACK(S_m1);                            /* und von m1         */
      arity = ARITY(x);                              /* stelligkeit        */
      --listcnt;
#if D_MESS
  if (D_M_C_MKLIST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKLIST);
#endif
      GEN_INSTR1(I_MKLIST,"mklist",arity);
    }
    /* dg 06.05.91 cons-listen */
    else
    if (T_CONS(x)) {
      PPOPSTACK(S_m);                                /* weg damit (von m)  */
      PPOPSTACK(S_m1);                               /* weg damit (von m1) */
#if D_MESS
  if (D_M_C_CONS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_CONS);
#endif
      GEN_INSTR0(I_CONS,"cons");
    }
    else {
      /* unerwarteter konstruktor */
      POST_MORTEM("ear: unexpected constructor on stack m");
    }
    goto main_m;                                     /* weiter auf m       */
  } /* end T_CON */

  /* also ein DOLLAR oder KLAA oder ? */

  if T_DOLLAR(x) {                                   /* funktion bearbeitet */
    if (gen_return) {
#if D_MESS
  if (D_M_C_RTF)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTF);
#endif
      GEN_INSTR0(I_RTF,"rtf");
    }
    PPOPSTACK(S_m);                                  /* das dollarsymbol    */
    x = READSTACK(S_m);
    if (T_COND(x)) {                                 /* conditional          */
      /* sonderbehandlung der conditionals */
      desc = (T_PTD)READSTACK(S_a);         /* descriptor           */
      if ((arity = ARITY(x)) > 1) {
        WRITE_ARITY(S_m,arity - 1);                  /* stelligkeit runter   */
        /* code fuer den (then-)else-teil folgt */
        px = (T_PTD)R_CONDI(*desc,ptd);  /* original cond */
	/* (int) von TB, 9.11.1992 */
        GEN_DESC(desc,"FUNC","CONDI",KEEP_FLAG((int) R_CONDI(*desc,args)),
		 R_CONDI(*desc,nargs),px,"jcond",desc);
        /* zieladresse fuer jcond eintragen */
#if D_MESS
        if (D_M_C_JCOND)
          R_CONDI(*desc,ptc)[3] = (INSTR)CODEPTR;
        else
          R_CONDI(*desc,ptc)[1] = (INSTR)CODEPTR;
#else
        R_CONDI(*desc,ptc)[1] = (INSTR)CODEPTR;
#endif
        GEN_LABEL("cond",desc);
        PPOPSTACK(S_e);                              /* trennsymbol HASH   */
        PUSHSTACK(S_m,DOLLAR);
        gen_return = 1;
        nfree = 0;
        goto main_e;
      }
      else {
        /* cond-konstruktor mit stelligkeit 1 */
	/* (int) von TB, 9.11.1992 */
        if (COND_FLAG((int) R_CONDI(*desc,args))) {
          /* returnadresse fuer pushret instruktion eintragen */
          R_CONDI(*desc,ptc)[-1] = (INSTR)CODEPTR;
	  /* (int) von TB, 9.11.1992 */
          L_CONDI(*desc,args) = KEEP_FLAG((int) R_CONDI(*desc,args));
          GEN_LABEL("cont",desc);
        }
        PPOPSTACK(S_e);                              /* trennsymbol HASH   */
        PPOPSTACK(S_a);                              /* CONDI-descriptor  */
        PPOPSTACK(S_m);                              /* COND-konstruktor  */
        /* auf m1 liegt kein COND-konstruktor! */
        gen_return = 0;          /* darauffolgendes return unterdruecken */
        goto main_m;                                 /* weiter auf m      */
      }
    } /* end T_COND */
    else PPOPSTACK(S_a);                             /* der zugeh. pointer! */
  }
  /* jetzt muss ein KLAA gefunden werden! */
  x = READSTACK(S_m);
  if (T_KLAA(x)) {
    if (gen_exit) {
#if D_MESS
  if (D_M_C_EXT)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_EXT);
#endif
      GEN_INSTR0(I_EXT,"ext");                       /* exit instruktion */
      gen_exit = 0;
    }
    GEN_INSTR0(I_END,"end");                         /* ende der codesequenz */
    gen_return = 1;
    if (SIZEOFSTACK(S_i) == 0) {
      /* keine weiteren pointer */
      WRITESTACK(S_e,POPSTACK(S_a));                 /* der zugeh. pointer */
      PPOPSTACK(S_m);                                /* das klaasymbol   */
    }
    else { /* SIZEOFSTACK(S_i) > 0 */
      /* weitere pointer (!) zu bearbeiten */
      desc = (T_PTD)POPSTACK(S_i);                   /* weg damit */
      PUSHSTACK(S_a,(STACKELEM)desc);                /* auf a */
      switch (R_DESC(*desc,type)) {
        case TY_COMB:                                /* definierte funktion */
                  margs = R_COMB(*desc,nargs);       /* benoetigte argumente */
                  nfree = 0;                         /* noch keine arg. frei */
                  px = (T_PTD)R_COMB(*desc,ptd);     /* original funktion */
                  PUSHSTACK(S_m,DOLLAR);             /* trennsymbol auf m   */
                  if (R_DESC(*px,type) == TY_LREC_IND)
                    px = R_LREC_IND(*px,ptf);        /* dereferenzieren */
                  if (T_POINTER((int)px) && R_DESC(*px,type) == TY_SUB) {
                    load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs);
                  }
                  else {
                    if (T_POINTER((int)px))
                      INC_REFCNT(px);
                    PUSHSTACK(S_e,(STACKELEM)px);
                  }
                  GEN_DESC(desc,"FUNC","COMB",0,margs,R_COMB(*desc,ptd),"func",desc);
                  L_COMB(*desc,ptc) = CODEPTR;       /* zeiger auf code      */
                  GEN_LABEL("func",desc);
                  goto main_e;
        case TY_CONDI:
                  margs = R_CONDI(*desc,nargs)-1;    /* benoetigte argumente */
                  nfree = 0;                         /* noch keine arg. frei */
                  px = (T_PTD)R_CONDI(*desc,ptd);    /* original conditional */
                  L_CONDI(*desc,ptc) = CODEPTR;
                  /* then- und else-expr einlagern */
                  i = R_CONDI(*desc,args) ? 0 : margs;
                  PUSHSTACK(S_m ,SET_ARITY(COND,2));  /* cond konstr. */
                  PUSHSTACK(S_e,HASH);               /* trennsymbol */
                  GEN_LABEL("jcond",desc);
                  if (_beta_count_only) {
                    if (!JumpPrefered ^ *R_COND(*px,ptte) >= *R_COND(*px,ptee)) {
#if D_MESS
  if (D_M_C_JFALSE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JFALSE);
#endif
                      GEN_INSTRL(I_JFALSE,"jfalse","cond",desc);
                      load_body((STACKELEM **)A_COND(*px,ptee),i,i);
                      PUSHSTACK(S_e,HASH);             /* trennsymbol */
                      load_body((STACKELEM **)A_COND(*px,ptte),i,i);
                    }
                    else {
#if D_MESS
  if (D_M_C_JTRUE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTRUE);
#endif
                      GEN_INSTRL(I_JTRUE,"jtrue","cond",desc);
                      load_body((STACKELEM **)A_COND(*px,ptte),i,i);
                      PUSHSTACK(S_e,HASH);             /* trennsymbol */
                      load_body((STACKELEM **)A_COND(*px,ptee),i,i);
                    }
                  }
                  else {
#if D_MESS
  if (D_M_C_JCOND)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JCOND);
#endif
                    GEN_INSTRL(I_JCOND,"jcond","cond",desc);
                    load_body((STACKELEM **)A_COND(*px,ptee),i,i);
                    PUSHSTACK(S_e,HASH);             /* trennsymbol */
                    load_body((STACKELEM **)A_COND(*px,ptte),i,i);
                  }
                  PUSHSTACK(S_m,DOLLAR);
                  goto main_e;
        case TY_CASE:
                  margs = R_CASE(*desc,nargs);     /* benoetigte argumente */
                  nfree = 0;                       /* noch keine arg. frei */
                  px = R_CASE(*desc,ptd);          /* original SWITCH */
                  args  = R_CASE(*desc,args);      /* rel. freie var.? */
                  x = (STACKELEM)desc;             /* CASE descriptor */
                  PPOPSTACK(S_a);                  /* CASE descriptor */
                  PUSHSTACK(S_v,0);                /* bottom-symbol */
                  GEN_DESC(desc,"FUNC","CASE",args,margs,px,"case",desc);
                  GEN_LABEL("case",desc);
                  L_CASE(*desc,ptc) = CODEPTR;

                  /* NCASE : generate list */
                  i = R_SWITCH(*px,case_type);
                  if (i > 1) {
#if D_MESS
  if (D_M_C_MKILIST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKILIST);
#endif
                    GEN_INSTR1(I_MKILIST,"mkilist",i);
                  }

                  for ( i = 1 ; i <= R_SWITCH(*px,nwhen) ; i++ ) {
                    register T_PTD ptd = (T_PTD)(R_SWITCH(*px,ptse)[i]);
                    if (R_DESC(*ptd,type) == TY_MATCH) {
                      if (R_MATCH(*ptd,code) != 0) {
#if D_MESS
  if (D_M_C_WHEN)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_WHEN);
#endif
                        GEN_INSTRX(I_WHEN,"when",ptd);
                      }
                      else
                      if (R_MATCH(*ptd,guard) == 0) {
                        /* code und guard 0: otherwise */
#if D_MESS
  if (D_M_C_PUSHC)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC);
#endif
                        GEN_INSTRN(I_PUSHC,"pushc",SA_TRUE);
                      }
                      if (R_MATCH(*ptd,guard) != 0) {
                        /* GUARD descriptor anlegen */
                        MAKEDESC(desc,1,C_FUNC,TY_GUARD);
                        L_CASE(*desc,args) = i;
                        L_CASE(*desc,nargs) = test_num(R_MATCH(*ptd,guard));
                        L_CASE(*desc,ptd) = (T_PTD)x; /* ptd; */
                        L_CASE(*desc,ptc) = (INSTR *)0;
                        PUSHSTACK(S_v,desc);
#if D_MESS
  if (D_M_C_GUARD)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_GUARD);
#endif
                        GEN_INSTRX(I_GUARD,"guard",desc);
                      }
                      if (R_MATCH(*ptd,body) != 0) {
                        /* BODY descriptor anlegen */
                        MAKEDESC(desc,1,C_FUNC,TY_BODY);
                        L_CASE(*desc,args) = i;
                        L_CASE(*desc,nargs) = test_num(R_MATCH(*ptd,body));
                        L_CASE(*desc,ptd) = (T_PTD)x; /* ptd; */
                        L_CASE(*desc,ptc) = (INSTR *)0;
                        PUSHSTACK(S_v,desc);
#if D_MESS
  if (D_M_C_BODY)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_BODY);
#endif
                        GEN_INSTRX(I_BODY,"body",desc);
                      }
                    }
                    else
                    if (R_DESC(*ptd,type) == TY_NOMAT) {
#if D_MESS
  if (D_M_C_NOMATCH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_NOMATCH);
#endif
                      GEN_INSTRX(I_NOMATCH,"nomatch",x);
                    }
                    else POST_MORTEM("ear: invalid SWITCH vector entry");
                  } /* end for */
                  while ((i = POPSTACK(S_v)) != 0)
                    PUSHSTACK(S_i,i);
#if D_MESS
  if (D_M_C_RTF)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTF);
#endif
                  GEN_INSTR0(I_RTF,"rtf");
                  goto main_m;
        case TY_GUARD:
                  PUSHSTACK(S_m,DOLLAR);             /* trennsymbol auf m */
                  /* MATCH descriptor holen : GUARD -> CASE -> SWITCH -> MATCH */
                  px = R_CASE(*desc,ptd);            /* CASE */

                  /* NCASE: multiple pattern beruecksichtigen! */
                  margs = R_CASE(*px,nargs);         /* alle var des CASE */
                  px = R_CASE(*px,ptd);              /* SWITCH */
                  margs -= R_SWITCH(*px,case_type);  /* freie var des CASE */

                  px = (T_PTD)(R_SWITCH(*px,ptse)[R_CASE(*desc,args)]);
                  if (R_CASE(*desc,nargs) == 0) {
                    /* guard hat keine raltiv freie variablen */
                    load_body((STACKELEM **)A_MATCH(*px,guard),margs,0);
                    px = (T_PTD)READSTACK(S_e);
                    if (T_POINTER((int)px) && R_DESC(*px,type) == TY_SUB) {
                      PPOPSTACK(S_e);                       /* sub funktion */
                      margs = R_FUNC(*px,nargs);
                      load_body((STACKELEM **)A_FUNC(*px,pte),margs,0);
                      DEC_REFCNT(px);
                    }
                    else margs = 0;
                    L_CASE(*desc,nargs) = margs;
                    GEN_DESC(desc,"FUNC","GUARD",R_CASE(*desc,args),margs,R_CASE(*desc,ptd),"guard",desc);
                    GEN_LABEL("guard",desc);
                    L_CASE(*desc,ptc) = CODEPTR;
                  }
                  else {
                    /* guard hat relativ freie variablen */
                    int freeargs,offset;
                    if (T_SNAP(R_MATCH(*px,guard)[1])) {
                      /* guard geschlossen */
                      load_body((STACKELEM **)A_MATCH(*px,guard),margs,0);
                      i = R_MATCH(*px,guard)[0];               /* laenge */
                      px = (T_PTD)R_MATCH(*px,guard)[i];       /* SUB descr */
                      args = R_FUNC(*px,nargs);                /* anz. arg. */
                      freeargs = ARITY(POPSTACK(S_e)) - 1;     /* anz snapargs */
                      L_CASE(*desc,nargs) = args;
                      GEN_DESC(desc,"FUNC","GUARD",R_CASE(*desc,args),R_CASE(*desc,nargs),R_CASE(*desc,ptd),"guard",desc);
                      L_CASE(*desc,ptc) = CODEPTR;
                      GEN_LABEL("guard",desc);
                      offset = margs-1+args-freeargs;
                      for (args = freeargs ; args > 0 ; args--) {
                        i = ARG_INDEX(offset-VALUE(POPSTACK(S_e)));
                        offset++;
#if D_MESS
  if (D_M_C_DUPARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DUPARG);
#endif
                        GEN_INSTR1(I_DUPARG,"duparg",i);
                      }
                      margs = R_FUNC(*px,nargs);
                      PPOPSTACK(S_e);                       /* sub funktion */
                      PUSHSTACK(S_e,SET_VALUE(DOLLAR,freeargs));
                      load_body((STACKELEM **)A_FUNC(*px,pte),margs,0);
                      DEC_REFCNT(px);
                    }
                    else {
                      /* mit allen freien variablen schliessen */
                      L_CASE(*desc,nargs) = margs;
                      load_body((STACKELEM **)A_MATCH(*px,guard),margs,margs);
                      GEN_DESC(desc,"FUNC","GUARD",R_CASE(*desc,args),R_CASE(*desc,nargs),R_CASE(*desc,ptd),"guard",desc);
                      L_CASE(*desc,ptc) = CODEPTR;
                      GEN_LABEL("guard",desc);
                      for (args = margs ; --args >= 0 ;) {
#if D_MESS
  if (D_M_C_DUPARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DUPARG);
#endif
                        GEN_INSTR1(I_DUPARG,"duparg",margs-1);
                      }
                    }
                  }
                  nfree = 0;
                  goto main_e;
        case TY_BODY:
                  PUSHSTACK(S_m,DOLLAR);             /* trennsymbol auf m */
                  /* MATCH descriptor holen : BODY -> CASE -> SWITCH -> MATCH */
                  px = R_CASE(*desc,ptd);            /* CASE */

                  /* NCASE: multiple pattern beruecksichtigen! */
                  margs = R_CASE(*px,nargs);         /* alle var des CASE */
                  px = R_CASE(*px,ptd);              /* SWITCH */
                  margs -= R_SWITCH(*px,case_type);  /* freie var des CASE */

                  px = (T_PTD)(R_SWITCH(*px,ptse)[R_CASE(*desc,args)]);
                  if (R_CASE(*desc,nargs) == 0) {
                    /* body hat keine raltiv freie variablen */
                    load_body((STACKELEM **)A_MATCH(*px,body),margs,0);
                    px = (T_PTD)READSTACK(S_e);
                    if (T_POINTER((int)px) && R_DESC(*px,type) == TY_SUB) {
                      PPOPSTACK(S_e);
                      margs = R_FUNC(*px,nargs);
                      load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs);
                      DEC_REFCNT(px);
                    }
                    else margs = 0;
                    L_CASE(*desc,nargs) = margs;
                    GEN_DESC(desc,"FUNC","BODY",R_CASE(*desc,args),margs,R_CASE(*desc,ptd),"body",desc);
                    GEN_LABEL("body",desc);
                    L_CASE(*desc,ptc) = CODEPTR;
                  }
                  else {
                    /* body hat relativ freie variablen */
                    int freeargs,offset;
                    if (T_SNAP(R_MATCH(*px,body)[1])) {
                      /* body geschlossen */
                      load_body((STACKELEM **)A_MATCH(*px,body),margs,0);
                      i = R_MATCH(*px,body)[0];               /* laenge */
                      px = (T_PTD)R_MATCH(*px,body)[i];       /* SUB descr */
                      args = R_FUNC(*px,nargs);                /* anz. arg. */
                      freeargs = ARITY(POPSTACK(S_e)) - 1;     /* anz snapargs */
                      L_CASE(*desc,nargs) = args;
                      GEN_DESC(desc,"FUNC","BODY",R_CASE(*desc,args),R_CASE(*desc,nargs),R_CASE(*desc,ptd),"body",desc);
                      L_CASE(*desc,ptc) = CODEPTR;
                      GEN_LABEL("body",desc);
                      offset = margs-1+args-freeargs;
                      for (args = freeargs ; args > 0 ; args--) {
                        i = ARG_INDEX(offset-VALUE(POPSTACK(S_e)));
                        offset++;
#if D_MESS
  if (D_M_C_DUPARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DUPARG);
#endif
                        GEN_INSTR1(I_DUPARG,"duparg",i);
                      }
                      margs = R_FUNC(*px,nargs);
                      PPOPSTACK(S_e);                       /* sub funktion */
                      load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs);
                      DEC_REFCNT(px);
                    }
                    else {
                      /* mit allen freien variablen schliessen */
                      L_CASE(*desc,nargs) = margs;
                      load_body((STACKELEM **)A_MATCH(*px,body),margs,margs);
                      GEN_DESC(desc,"FUNC","BODY",R_CASE(*desc,args),R_CASE(*desc,nargs),R_CASE(*desc,ptd),"body",desc);
                      L_CASE(*desc,ptc) = CODEPTR;
                      GEN_LABEL("body",desc);
                      for (args = margs ; --args >= 0 ;) {
#if D_MESS
  if (D_M_C_DUPARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DUPARG);
#endif
                        GEN_INSTR1(I_DUPARG,"duparg",margs-1);
                      }
                    }
                  }
                  nfree = 0;
                  goto main_e;

#ifdef NotUsed
                  load_body((STACKELEM **)A_MATCH(*px,body),margs,0);
                  if (T_SNAP(READSTACK(S_e))) {
                    /* body hat relativ freie variablen */
                    int freeargs,offset;
                    i = R_MATCH(*px,body)[0];                /* laenge */
                    px = (T_PTD)R_MATCH(*px,body)[i];        /* SUB descr */
                    L_CASE(*desc,nargs) = R_FUNC(*px,nargs); /* anz. arg. */
                    GEN_DESC(desc,"FUNC","BODY",R_CASE(*desc,args),R_CASE(*desc,nargs),R_CASE(*desc,ptd),"body",desc);
                    L_CASE(*desc,ptc) = CODEPTR;
                    GEN_LABEL("body",desc);
                    freeargs = ARITY(READSTACK(S_e))-1;
                    offset = margs-1+R_CASE(*desc,nargs)-freeargs;
                    for (args = ARITY(POPSTACK(S_e)) - 1 ; args > 0 ; args--) {
                      /* while (NO_MORE_VARIABLES()) */
                        /* POPSTACK(S_e); */
                      i = ARG_INDEX(offset-VALUE(POPSTACK(S_e)));
                      offset++;
#if D_MESS
  if (D_M_C_DUPARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DUPARG);
#endif
                      GEN_INSTR1(I_DUPARG,"duparg",i);
                    }
                    PPOPSTACK(S_e);                       /* sub funktion */
                    margs = R_FUNC(*px,nargs);
                    load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs);
                    DEC_REFCNT(px);
                  }
                  else {
                    /* body hat keine raltiv freie variablen */
                    px = (T_PTD)READSTACK(S_e);
                    if (T_POINTER((int)px) && R_DESC(*px,type) == TY_SUB) {
                      PPOPSTACK(S_e);                       /* sub funktion */
                      margs = R_FUNC(*px,nargs);
                      load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs);
                      DEC_REFCNT(px);
                    }
                    /* else margs = margs -1;    */      /* 02.02.90 !!! */
                    L_CASE(*desc,nargs) = margs;
                    GEN_DESC(desc,"FUNC","BODY",R_CASE(*desc,args),margs,R_CASE(*desc,ptd),"body",desc);
                    L_CASE(*desc,ptc) = CODEPTR;
                    GEN_LABEL("body",desc);
                  }
                  nfree = 0;
                  goto main_e;
#endif /* NotUsed */

        default :
                  sprintf(buf,"ear: (1) unexpected pointer %08x on stack i (type %x)",(unsigned int)desc,R_DESC(*desc,type));
                  POST_MORTEM(buf);
      } /* end switch */
    } /* end (SIZEOFSTACK(S_i) > 0) */
  } /* end T_KLAA(x) */
  else POST_MORTEM("ear: unexpected symbol on stack m");

  /* aufraeumen und terminieren */

  /* auf v stehen die funktions-pointer des gesamten ausdrucks */
  for (args = SIZEOFSTACK(S_v) ; args > 0 ; args--) {
    /* v-stack abraeumen */
    PPOPSTACK(S_v);
  }

#if     DEBUG
  if (codefp) fclose(codefp);                           /* codefile schliessen */
  /* codedump fuer wb schreiben */
  if ((codefp = fopen(CodeDump,"w")) == NULL)
    post_mortem("can't open codedump");

#define NEWLINE()     fprintf(codefp,"\n");
#define OUTLABEL(l)   fprintf(codefp,"%08x: \t",(unsigned int)l)
#define OUTINSTR(i)   fprintf(codefp,"%-12s\n",i)
#define OUTARG(p)     OUTLABEL(p);fprintf(codefp,"%08x\n",*p)
#else
#define NEWLINE()
#define OUTLABEL(l)
#define OUTINSTR(i)
#define OUTARG(p)
#endif  /* DEBUG */

/* codeadressse in symboltabelle eintragen */
#define NEWSYMB(p)     do {if (R_DESC(*(T_PTD)p,class) == C_FUNC \
                       &&  R_DESC(*(T_PTD)p,type) != TY_CLOS) { \
                         NEWCODE(R_UNIV(*(T_PTD)p,ptc)); ++SYMBLEN; \
                       }} while(0)
#define NEWTARGET(p)   NEWCODE(p); ++SYMBLEN

  SYMBTAB = CODEPTR;   /* start der symboltabelle */
  NEWCODE(I_SYMTAB);   /* directive */
  NEWCODE(0);          /* laenge 0 */

  /* durchlauf durch die codesequenz um die parameter von BETA u. TAIL   */
  /* instruktionen, die zeiger auf descriptoren darstellen, durch zeiger */
  /* auf die entsprechenden codesequenzen zu ersetzen                    */

  for (cp = CODEVEC ; cp < CODEPTR ; ) {
    OUTLABEL(cp);
    switch(*cp++) {
      case I_EXT:      OUTINSTR("I_EXT");                       break;
      case I_PUSH:     OUTINSTR("I_PUSH");
                       NEWSYMB(*cp);          OUTARG(cp); cp++; break;
      case I_PUSHC:    OUTINSTR("I_PUSHC");   OUTARG(cp); cp++; break;
      case I_PUSHARG:  OUTINSTR("I_PUSHARG"); OUTARG(cp); cp++; break;
      case I_PUSHARG0: OUTINSTR("I_PUSHARG0");                  break;
      case I_MOVE:     OUTINSTR("I_MOVE");                      break;
      case I_DELTA1:   OUTINSTR("I_DELTA1");  OUTARG(cp); cp++; break;
      case I_DELTA2:   OUTINSTR("I_DELTA2");  OUTARG(cp); cp++; break;
      case I_DELTA3:   OUTINSTR("I_DELTA3");  OUTARG(cp); cp++; break;
      case I_DELTA4:   OUTINSTR("I_DELTA4");  OUTARG(cp); cp++; break;
      case I_ADD:      OUTINSTR("I_ADD");                       break;
      case I_SUB:      OUTINSTR("I_SUB");                       break;
      case I_MUL:      OUTINSTR("I_MUL");                       break;
      case I_DIV:      OUTINSTR("I_DIV");                       break;
      case I_MOD:      OUTINSTR("I_MOD");                       break;
      case I_EQ:       OUTINSTR("I_EQ");                        break;
      case I_NE:       OUTINSTR("I_NE");                        break;
      case I_LT:       OUTINSTR("I_LT");                        break;
      case I_LE:       OUTINSTR("I_LE");                        break;
      case I_GT:       OUTINSTR("I_GT");                        break;
      case I_GE:       OUTINSTR("I_GE");                        break;
      case I_NEG:      OUTINSTR("I_NEG");                       break;
      case I_NOT:      OUTINSTR("I_NOT");                       break;
      case I_OR:       OUTINSTR("I_OR");                        break;
      case I_AND:      OUTINSTR("I_AND");                       break;
      case I_XOR:      OUTINSTR("I_XOR");                       break;
      case I_JFALSE:   OUTINSTR("I_JFALSE");
                       NEWTARGET(*cp);        OUTARG(cp); cp++; break;
      case I_JTRUE:    OUTINSTR("I_JTRUE");
                       NEWTARGET(*cp);        OUTARG(cp); cp++; break;
      case I_JCOND:    OUTINSTR("I_JCOND");
                       NEWTARGET(*cp);        OUTARG(cp); cp++; break;
      case I_PUSHRET:  OUTINSTR("I_PUSHRET");
                       NEWTARGET(*cp);        OUTARG(cp); cp++; break;
      case I_BETA:     *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_BETA");    OUTARG(cp); cp++; break;
      case I_BETA0:    *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_BETA0");   OUTARG(cp); cp++; break;
      case I_BETA1:    *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_BETA1");   OUTARG(cp); cp++; break;
      case I_JTAIL:    *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAIL");    OUTARG(cp); cp++; break;
      case I_JTAIL0:    *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAIL0");   OUTARG(cp); cp++; break;
      case I_JTAIL1:   *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAIL1");   OUTARG(cp); cp++; break;
      case I_BETAQ:    *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_BETAQ");   OUTARG(cp); cp++; break;
      case I_BETAQ0:   *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_BETAQ0");  OUTARG(cp); cp++; break;
      case I_BETAQ1:   *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_BETAQ1");  OUTARG(cp); cp++; break;
      case I_JTAILQ:   *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAILQ");   OUTARG(cp); cp++; break;
      case I_JTAILQ0:  *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAILQ0");  OUTARG(cp); cp++; break;
      case I_JTAILQ1:  *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAILQ1");  OUTARG(cp); cp++; break;
      case I_MKLIST:   OUTINSTR("I_MKLIST");  OUTARG(cp); cp++; break;
      case I_FREE:     OUTINSTR("I_FREE");    OUTARG(cp); cp++; break;
      case I_FREE1:    OUTINSTR("I_FREE1");                     break;
      case I_RTF:      OUTINSTR("I_RTF");                       break;
      case I_RTM:      OUTINSTR("I_RTM");                       break;
      case I_RTP:      OUTINSTR("I_RTP");     OUTARG(cp); cp++; break;
      case I_RTC:      OUTINSTR("I_RTC");     OUTARG(cp); cp++; break;
      case I_RTS:      OUTINSTR("I_RTS");                       break;
      case I_MKGCLOS:  OUTINSTR("I_MKGCLOS"); OUTARG(cp); cp++; break;
      case I_MKBCLOS:  OUTINSTR("I_MKBCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKCCLOS:  OUTINSTR("I_MKCCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKSCLOS:  OUTINSTR("I_MKSCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKDCLOS:  OUTINSTR("I_MKDCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKAP:     OUTINSTR("I_MKAP");    OUTARG(cp); cp++; break;
      case I_APPLY:    OUTINSTR("I_APPLY");   OUTARG(cp); cp++; break;
      case I_CASE:     *cp = (INSTR)R_CASE(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_CASE");    OUTARG(cp); cp++; break;
      case I_WHEN:  /* *cp = (INSTR)R_CASE(*(T_PTD)*cp,ptc); */
                       OUTINSTR("I_WHEN");    OUTARG(cp); cp++; break;
      case I_GUARD:    *cp = (INSTR)R_CASE(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_GUARD");   OUTARG(cp); cp++; break;
      case I_BODY:     *cp = (INSTR)R_CASE(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_BODY");    OUTARG(cp); cp++; break;
      case I_NOMATCH:  OUTINSTR("I_NOMATCH"); OUTARG(cp); cp++; break;
      case I_DUPARG:   OUTINSTR("I_DUPARG");  OUTARG(cp); cp++; break;
      case I_SAPPLY:   OUTINSTR("I_SAPPLY");                    break;
      case I_MKTHEN:   OUTINSTR("I_MKTHEN");                    break;
      case I_MKELSE:   OUTINSTR("I_MKELSE");                    break;
      case I_MKCOND:   OUTINSTR("I_MKCOND");                    break;
      case I_LSEL:     OUTINSTR("I_LSEL");                      break;
      case I_EVAL:     OUTINSTR("I_EVAL");                      break;
      case I_EVAL0:    OUTINSTR("I_EVAL0");                     break;
      case I_CONS:     OUTINSTR("I_CONS");                      break;
      case I_FCONS:    OUTINSTR("I_FCONS");                     break;
      case I_FIRST:    OUTINSTR("I_FIRST");                     break;
      case I_REST:     OUTINSTR("I_REST");                      break;
      case I_UPDAT:    OUTINSTR("I_UPDAT");   OUTARG(cp); cp++; break;
      case I_MKILIST:  OUTINSTR("I_MKILIST"); OUTARG(cp); cp++; break;
      case I_DESC:     NEWSYMB(*cp);
                       OUTINSTR("I_DESC");    OUTARG(cp); cp++; break;
      case I_TABSTART: OUTINSTR("I_TABSTART");
                       *cp = (INSTR)SYMBTAB;  OUTARG(cp); cp++; break;
      case I_SYMTAB:   OUTINSTR("I_SYMTAB");
                       *cp = SYMBLEN;
                       OUTARG(cp);
                       cp++;
                       for (i = SYMBLEN ; i > 0 ; i--) {
                         /* NEWLINE(); */
                         /* OUTLABEL(cp); */
                         OUTARG(cp);
                         cp++;
                       }
                       NEWCODE(I_END);
                       break;
      case I_END:      OUTINSTR("I_END");                       break;

/* hier beginnt der Verteilungs- und Messkram    RS ab 1.1.1993 */

      case I_PUSHH:    OUTINSTR("I_PUSHH"); OUTARG(cp); cp++;   break;
      case I_DIST:     OUTINSTR("I_DIST"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_DISTEND:  OUTINSTR("I_DIST_END");                  break;
      case I_WAIT:     OUTINSTR("I_WAIT"); OUTARG(cp); cp++;    break;
      case I_POPH:     OUTINSTR("I_POPH");                      break;
      case I_DISTB:     OUTINSTR("I_DISTB"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;

   /* hier beginnt der Messkram */

      case I_COUNT:    OUTINSTR("I_COUNT"); OUTARG(cp); cp++;   break;
      case I_MHPALLON: OUTINSTR("I_MHPALLON");                  break;
      case I_MHFREEON: OUTINSTR("I_MHFREEON");                  break;
      case I_MHPCMPON: OUTINSTR("I_MHPCMPON");                  break;
      case I_MDESCALLON: OUTINSTR("I_MDESCALLON");              break;
      case I_MDESCFREON: OUTINSTR("I_MDESCFREON");              break;

      case I_MPROCREON: OUTINSTR("I_MPROCREON");                break;
      case I_MSDISTEND: OUTINSTR("I_MSDISTEND");                break;
      case I_MPRSLEEON: OUTINSTR("I_MPRSLEEON");                break;
      case I_MPRORUNON: OUTINSTR("I_MPRORUNON");                break;
      case I_MPROWAKON: OUTINSTR("I_MPROWAKON");                break;
      case I_MPROTERON: OUTINSTR("I_MPROTERON");                break;

      case I_MCOMSEBON: OUTINSTR("I_MCOMSEBON");                break;
      case I_MCOMSEEON: OUTINSTR("I_MCOMSEEON");                break;
      case I_MCOMREBON: OUTINSTR("I_MCOMREBON");                break;
      case I_MCOMREEON: OUTINSTR("I_MCOMREEON");                break;

      case I_MSTCKPUON: OUTINSTR("I_MSTCKPUON");                break;
      case I_MSTCKPPON: OUTINSTR("I_MSTCKPPON");                break;
      case I_MSTKSEGAL: OUTINSTR("I_MSTKSEGAL");                break;
      case I_MSTKSEGFR: OUTINSTR("I_MSTKSEGFR");                break;

      case I_MSDISTON : OUTINSTR("I_MSDISTON");                 break;
      case I_MSNODIST : OUTINSTR("I_MSNODIST");                 break;
    
      case I_CHKFRAME : OUTINSTR("I_CHKFRAME"); OUTARG(cp); cp++;   break;

/* hier endet der Verteilungs- und Messkram */

#if    UH_ZF_PM
      /* ullis instruktionen */
/*uh, 16.7.90*/  
/*uh, 16.7.90*/        case I_APPEND:   OUTINSTR("I_APPEND"); break;
/*uh, 16.7.90*/        case I_BEGINZF:  OUTINSTR("I_BEGINZF"); break;
/*uh, 16.7.90*/        case I_ENDZF:    OUTINSTR("I_ENDZF"); break;
/*uh, 16.7.90*/        case I_DROPP:    OUTINSTR("I_DROPP");  break;
/*uh, 16.7.90*/        case I_TLIST:    OUTINSTR("I_TLIST");OUTARG(cp); cp++; break;
/*uh, 16.7.90*/        case I_TZERO:    OUTINSTR("I_TZERO");OUTARG(cp); cp++; break;
/*uh, 16.7.90*/        case I_TFALSE:   OUTINSTR("I_TFALSE");OUTARG(cp); cp++; break;
/*uh, 16.7.90*/        case I_TTRUE:    OUTINSTR("I_TTRUE");OUTARG(cp); cp++; break;
/*uh, 16.7.90*/        case I_JUMP:     OUTINSTR("I_JUMP");NEWTARGET(*cp);
/*uh, 16.7.90*/                                            OUTARG(cp); cp++; break;
/*uh, 16.7.90*/        case I_EXTRACT:  OUTINSTR("I_EXTRACT");break;
/*uh, 16.7.90*/        case I_DIM:      OUTINSTR("I_DIM");break;
/*uh, 16.7.90*/        case I_MAKEBOOL: OUTINSTR("I_MAKEBOOL"); OUTARG(cp); cp++; break;
/*uh, 16.7.90*/        case I_MAKEZF:   OUTINSTR("I_MAKEZF"); OUTARG(cp); cp++; break;
#endif /* UH_ZF_PM */
      default:         POST_MORTEM("ear: invalid instruction");
    } /* end switch */
    /* NEWLINE(); */
  } /* end for */
  NEWCODE(I_STOP);

#if    DEBUG
  if (codefp) fclose(codefp);                           /* codefile schliessen */
#endif /* DEBUG */

  END_MODUL("ear");
}

/* end of file */
