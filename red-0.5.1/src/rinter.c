/* $Log: rinter.c,v $
 * Revision 1.2  1992/12/16  12:50:04  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/****************************************************************************/
/* file rinter.c                                                            */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/* dg 31.05.90  um diese zeit muss es angefangen haben, oder frueher?       */
/* dg xx.xx.xx  viel schweiss und blut ...                                  */
/* dg 10.08.92  multiple pattern (n-stellige cases) implementiert           */
/* ach 04/01/93 aufrufe fuer dynamische Stackverwaltung eingefuegt          */
/*                                                                          */
/****************************************************************************/

#define INTER "Interpreter fuer abstrakten Code"

#include "rstdinc.h"
#if nCUBE
#include "rncstack.h"
#include "rncmessage.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "rprimf.h"
#include "rinter.h"
#include <string.h>          /* RS 6.11.1992 */

#if nCUBE
#include "rncsched.h"
#endif

#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

#if nCUBE
extern INSTR *distribute();
#endif

#if  SINTRAN3
extern double tused();
#define TIMER() tused()
#else
#if (!nCUBE)
extern long clock();
#endif
#define TIMER() clock() 
#endif

/* --- externe variablen / routinen --- */

extern BOOLEAN _count_reductions;  /* <r>, reduktionen zaehlen? */
extern BOOLEAN _beta_count_only;   /* <r>, nur beta-reduktionen zaehlen */
extern BOOLEAN _digit_recycling;   /* <w>, fuer die ausnahmebehandlung  */
extern BOOLEAN _heap_reserved;     /* <w>, fuer die ausnahmebehandlung  */
extern T_PTD   _desc;              /* <w>, der ergebnisdeskriptor       */

extern int     LazyLists;

#if nCUBE
extern int host;
extern PROCESS_CONTEXT* curr_pid;
extern INSTR_DESCR instr_tab[];
extern int proc_id;
extern INSTR *code;
extern int sig_msg_flag;
int     _redcnt;
BOOLEAN _interrupt;
jmp_buf _interruptbuf;
#else
extern jmp_buf _interruptbuf;      /* <w>, fuer die ausnahmebehandlung  */
extern int     _redcnt;            /* <w>, rstate.c - reduktionszaehler */
extern BOOLEAN _interrupt;         /* <w>, fuer die ausnahmebehandlung  */
#endif

#if    DEBUG
extern FILE    *TraceFp;           /* setup.c */
extern int     OutInstr;           /* setup.c ausgabe von instruktionen */
extern int     OutLen;             /* setup.c laenge des zyklischen ausgabebereiches */
extern int     debdisplay;         /* rruntime.c */
extern char    *stelname();        /* rear.c */
extern void    stackname_dr();     /* debug system */
#endif /* DEBUG   auskommentiert RS 4.12.1992 */

extern int     pm_am();            /* ram.c pattern matching abstract machine */
extern T_PTD   newdesc();          /* rheap.c allocate a new descriptor */
extern T_PTD   gen_id();           /* rlstruct.c generate indirect list descriptor */
extern void    cursorxy();         /* editor: cursor motion */

/* ach 05/11/92 */
extern void stack_error();         /* rstack.c */
extern int newheap();              /* rheap.c */
extern void load_expr();           /* rhinout.c */
extern void trav_e_a();            /* rtrav.c */
extern STACKELEM st_expr();        /* rstpro.c */
extern int conv_digit_int();       /* rdig-v1.c */
/* end of ach */

/* RS 5.11.92 */
extern void test_inc_refcnt();             /* rheap.c */
extern void rel_heap();                    /* rheap.c */
extern void res_heap();                    /* rheap.c */
extern void switchstacks();                /* rruntime.c */
extern void test_dec_refcnt();             /* rheap.c */

/* ach 06/11/92 */
#if SCAVENGE
extern void enable_scav();                 /* rscavenge.c */
extern char *get_heap();                   /* rdummies.c */
extern void disable_scav();                /* rscavenge.c */
#endif
/* end of ach */

#if nCUBE
extern void d_pushstack();
extern STACKELEM d_popstack();
extern STACKELEM *d_ppopstack();
#else
extern STACKELEM *ppopstack();             /* rheap.c */
extern STACKELEM midstack();               /* rstack.c */
extern void updatestack();                 /* rstack.c */
#endif

/* --- interne variablen/routinen --- */

#if nCUBE                  /* ach 04/01/93: Bei dynamischer Stackverwaltung werden andere Typen benutzt */

DStackDesc     *ps_a;              
DStackDesc     *ps_i;

#else

StackDesc      *ps_a;              /* zeiger auf aktuellen a-stack */
StackDesc      *ps_i;              /* zeiger auf aktuellen i-stack */

#endif

static char    buf[80];            /* puffer fuer fehlermeldungen */
static void    load_pmlist();
static void    load_pmilist();

/* --- stackoperationen --- */

#ifdef  PUSH_I
#undef  PUSH_I
#endif

#if nCUBE               /* ach 04/01/92 */

#define POP_A()           D_POPSTACK(*ps_a)
#define PPOP_A()          D_PPOPSTACK(*ps_a)
#define TOP_A()           D_READSTACK(*ps_a)
#define PUSH_A(x)         D_PUSHSTACK(*ps_a,(x))
#define PPUSH_A(x)        D_PPUSHSTACK(*ps_a,(x))
#define WRITE_A(x)        D_WRITESTACK(*ps_a,(x))
#define MID_A(n)          D_MIDSTACK(*ps_a,n)
#define UPDATE_A(n,x)     D_UPDATESTACK(*ps_a,n,(x))

#define POP_I()           D_POPSTACK(*ps_i)
#define PPOP_I()          D_PPOPSTACK(*ps_i)
#define TOP_I()           D_READSTACK(*ps_i)
#define PUSH_I(x)         D_PUSHSTACK(*ps_i,(x))
#define PPUSH_I(x)        D_PPUSHSTACK(*ps_i,(x))
#define WRITE_I(x)        D_WRITESTACK(*ps_i,(x))
#define MID_I(x)          D_MIDSTACK(*ps_i,x)
#define UPDATE_I(n,x)     D_UPDATESTACK(*ps_i,n,(x))

#define POP_R()           D_POPSTACK(D_S_R)
#define PPOP_R()          D_PPOPSTACK(D_S_R)
#define TOP_R()           D_READSTACK(D_S_R)
#define PUSH_R(x)         D_PUSHSTACK(D_S_R,(x))
#define PPUSH_R(x)        D_PPUSHSTACK(D_S_R,(x))
#define WRITE_R(x)        D_WRITESTACK(D_S_R,(x))
#define MID_R(x)          D_MIDSTACK(D_S_R,(x))
#define UPDATE_R(n,x)     D_UPDATESTACK(D_S_R,n,(x))

#define POP_E()           D_POPSTACK(D_S_H)
#define PPOP_E()          D_PPOPSTACK(D_S_H)
#define TOP_E()           D_READSTACK(D_S_H)
#define PUSH_E(x)         D_PUSHSTACK(D_S_H,(x))
#define PPUSH_E(x)        D_PPUSHSTACK(D_S_H,(x))
#define WRITE_E(x)        D_WRITESTACK(D_S_H,(x))
#define MID_E(x)          D_MIDSTACK(D_S_H,(x))
#define UPDATE_E(n,x)     D_UPDATESTACK(D_S_H,n,(x))

#else

#define POP_A()           POPSTACK(*ps_a)
#define PPOP_A()          PPOPSTACK(*ps_a)
#define TOP_A()           READSTACK(*ps_a)
#define PUSH_A(x)         PUSHSTACK(*ps_a,(x))
#define PPUSH_A(x)        PPUSHSTACK(*ps_a,(x))
#define WRITE_A(x)        WRITESTACK(*ps_a,(x))
#define MID_A(n)          MIDSTACK(*ps_a,n)
#define UPDATE_A(n,x)     UPDATESTACK(*ps_a,n,(x))

#define POP_I()           POPSTACK(*ps_i)
#define PPOP_I()          PPOPSTACK(*ps_i)
#define TOP_I()           READSTACK(*ps_i)
#define PUSH_I(x)         PUSHSTACK(*ps_i,(x))
#define PPUSH_I(x)        PPUSHSTACK(*ps_i,(x))
#define WRITE_I(x)        WRITESTACK(S_i,(x))
#define MID_I(x)          MIDSTACK(*ps_i,x)
#define UPDATE_I(n,x)     UPDATESTACK(*ps_i,n,(x))

#define POP_R()           POPSTACK(S_m)
#define PPOP_R()          PPOPSTACK(S_m)
#define TOP_R()           READSTACK(S_m)
#define PUSH_R(x)         PUSHSTACK(S_m,(x))
#define PPUSH_R(x)        PPUSHSTACK(S_m,(x))
#define WRITE_R(x)        WRITESTACK(S_m,(x))
#define MID_R(x)          MIDSTACK(S_m,(x))
#define UPDATE_R(n,x)     UPDATESTACK(S_m,n,(x))

#define POP_E()           POPSTACK(S_e)
#define PPOP_E()          PPOPSTACK(S_e)
#define TOP_E()           READSTACK(S_e)
#define PUSH_E(x)         PUSHSTACK(S_e,(x))
#define PPUSH_E(x)        PPUSHSTACK(S_e,(x))
#define WRITE_E(x)        WRITESTACK(S_e,(x))
#define MID_E(x)          MIDSTACK(S_e,(x))
#define UPDATE_E(n,x)     UPDATESTACK(S_e,n,(x))

#endif /* nCUBE */
/* --- debug: ausgabe der auszufuehrenden instruktion --- */

#if     DEBUG

static int  instrcnt = 0; /* zaehler fuer ausgefuerte instruktionen */
static int  relpos   = 0; /* zyklischer positionszahler fuer bildschirmausgabe */
static int  baserow  = 2; /* erste ausgabezeile */
static int  basecol  = 0; /* ausgabespalte */

#define NEXTINSTR()       cursorxy(baserow+(relpos=(relpos+1)%OutLen),basecol); \
                          printf("%d ",++instrcnt)
#define POST_MORTEM(mes)  {if (TraceFp) { fclose(TraceFp); TraceFp=0; } post_mortem(mes); }

#if nCUBE

#define SWITCHSTACKS()    {register DStackDesc *x = ps_a; ps_a = ps_i; ps_i = x;} \
                           switchstacks(ps_a,ps_i); \
                           fprintf(TraceFp,"_switchstacks();\n")

#else

#define SWITCHSTACKS()    {register StackDesc *x = ps_a; ps_a = ps_i; ps_i = x;} \
                           switchstacks(ps_a,ps_i); \
                           fprintf(TraceFp,"_switchstacks();\n")

#endif

#define COMM0(x)          if (OutInstr && debdisplay) { \
                            NEXTINSTR(); \
                            printf("comment(\"%s\")          \n",x); \
                          } \
                          fprintf(TraceFp,"comment(\"%s\");\n",x)
#define FUNC0(x)          if (OutInstr && debdisplay) { \
                            NEXTINSTR(); \
                            printf("%s()                     \n",x); \
                          } \
                          fprintf(TraceFp,"%s();\n",x)
#define FUNC1(x)          if (OutInstr && debdisplay) { \
                            NEXTINSTR(); \
                            printf("%s(%1d)                  \n",x,*code); \
                          } \
                          fprintf(TraceFp,"%s(%1d);\n",x,*code)
#define FUNC2(x)          if (OutInstr && debdisplay) { \
                            NEXTINSTR(); \
                            printf("%s(%1d, %1d)             \n",x,*code,*(code+1)); \
                          } \
                          fprintf(TraceFp,"%s(%1d, %1d);\n",x,*code,*(code+1))
#define FUNCX(x)          if (OutInstr && debdisplay) { \
                            NEXTINSTR(); \
                            printf("%s(%s)                   \n",x,stelname(*code)); \
                          } \
                          fprintf(TraceFp,"%s(%s);\n",x,stelname(*code))
#define FUNCN(x)          if (OutInstr && debdisplay) { \
                            NEXTINSTR(); \
                            printf("%s(%s)                   \n",x,stelname(*code)); \
                          } \
                          fprintf(TraceFp,"%s(%s);\n",x,stelname(*code))
#define FUNCL(x)          if (OutInstr && debdisplay) { \
                            NEXTINSTR(); \
                            printf("%s(label(0x%08x))        \n",x,*code); \
                          } \
                          fprintf(TraceFp,"%s(label(0x%08x));\n",x,*code)

#else   /* !DEBUG */

#define POST_MORTEM(mes)  post_mortem(mes)

#if nCUBE
#define SWITCHSTACKS()    {register DStackDesc *x = ps_a; ps_a = ps_i; ps_i = x;}
#else
#define SWITCHSTACKS()    {register StackDesc *x = ps_a; ps_a = ps_i; ps_i = x;}
#endif

#define COMM0(x)
#define FUNC0(x)
#define FUNC1(x)
#define FUNC2(x)
#define FUNCX(x)
#define FUNCN(x)
#define FUNCL(x)

#endif /* DEBUG */

/* --- makros --- */

#define TAIL_FLAG         0
#define POP_RET()         (INSTR *)(POP_R() & ~TAIL_FLAG)
#define TEST_TAIL_FLAG()  (TOP_R() == TAIL_FLAG)
#define SET_TAIL_FLAG()   PUSH_R(TAIL_FLAG)
#define DEL_TAIL_FLAG()   PPOP_R()
#define INVERT_TAIL_FLAG() if (TOP_R() == TAIL_FLAG) \
                             PPOP_R(); \
                           else PUSH_R(TAIL_FLAG)
#define STACKSPACE(n)     IS_SPACE((*ps_a),(n))
/* beta,tail, cond und jtrue/jfalse: parameter sind absolute sprungziele */
#define GRAB_DESC(p)      p = ((T_PTD *)p)[-1]
#define COMB_CODE(p)      p = (T_PTD)R_COMB(*p,ptc)

/* GET_DESC(p), GET_HEAP(n,p), MAKEDESC(p,ref,cl,ty) defined in rheapty.h */

/* --- spezielle routinen --- */

static INSTR rout_rtf[]    = { I_RTF };                     /* return */
static INSTR rout_sret[]   = { I_SRET };                    /* free and return */
static INSTR rout_sapply[] = { I_SAPPLY, I_RTS };           /* special apply */
static INSTR rout_eval0[]  = { I_EVAL0, I_RTS };            /* evaluate */
static INSTR rout_updat[]  = { I_UPDAT, I_RTS };            /* eval, update */
       INSTR Rout_Eval[]   = { I_EVAL, I_EXT };             /* eval and exit */
       INSTR Rout_Mkcnd[]  = { I_MKTHEN, I_MKELSE, I_MKCOND, I_EXT }; /* make cond */

/* --------------------------------------------------------------------*/
/* ---                         Interpreter                         --- */
/* --------------------------------------------------------------------*/

#if nCUBE
INTER_RESULT inter()
#else
void inter(code)
register INSTR * VOLATILE code;
#endif
{
  register T_PTD        ptdesc;          /* zeiger auf descriptor     */
  /* 4 Initialisierungen von TB, 6.11.1992, 4 volatile: ach 10/11/92 */
  register VOLATILE int          arg=0;           /* anzahl argumente          */
  register VOLATILE int          arity=0;         /* stelligkeit               */
  register VOLATILE int          func=0;          /* funktion (pf)             */
  register VOLATILE int          help=0;          /* hilfsvariable             */
  register int          delta_count;     /* delta reduktionen zaehlen */
  register PTR_HEAPELEM pth;             /* pointer to heapelement    */
  register int          beta_count;      /* beta  reduktionen zaehlen */
#if    UH_ZF_PM
  /* hier fuegt ulli noch einige definitionen dazu ! */
/*dg, 27.07.90*/    /* wenn irgendwie moeglich keine weiteren variablen einfuehren */
/*dg, 27.07.90*/    /* Aber Namen werden zugestanden:                              */
/*dg, 27.07.90*/  
/*dg, 27.07.90*/  int zb; /* debugging */
/*dg, 27.07.90*/  
#define R0 (int)ptdesc
#define R1 arg
#define R2 arity
#define R3 func
#define R4 help
#define R5 (int)pth
#endif /* UH_ZF_PM      auskommentiert RS 4.12.1992 */

DBUG_ENTER ("inter");

  /* initialisierungen */

#if (!nCUBE)

  ps_a = &S_a;          /* initial working  stack is mapped on S_a  */
  ps_i = &S_m1;         /* initial argument stack is mapped on S_m1 */

#endif

#if (DEBUG && !nCUBE)

  /* rename stacks for debugging purposes (undone at return) */
  strcpy(S_e.Name, "H"); stackname_dr(&S_e);
  strcpy(S_a.Name, "A"); stackname_dr(&S_a);
  strcpy(S_m1.Name,"I"); stackname_dr(&S_m1);
  strcpy(S_m.Name, "R"); stackname_dr(&S_m);

  /* initialize some debug variables */
  instrcnt = 0;         /* zaehler fuer ausgefuerte instruktionen */
  relpos   = 0;         /* zyklischer positionszahler fuer bildschirmausgabe */
#endif /* DEBUG  auskommentiert RS 4.12.1992 */

  _interrupt  = FALSE;
  beta_count  = _count_reductions;
  delta_count = !_beta_count_only;

  START_MODUL("inter");

#if SCAVENGE
  enable_scav();                            /* enable generation scavenging */
#endif /* SCAVENGE         auskommentiert RS 4.12.1992 */

  setjmp(_interruptbuf);                    /* vorbereitung fuer longjmp */
  if (_interrupt)                           /* falls interrupt, dann     */
    goto L_interrupt;                       /* zur interruptbehandlung   */

  /* --- interpreter loop --- */

/*
#if nCUBE
fprintf(stderr,"\aNode %d: interpreter, code: %x\n",proc_id,code);
#endif
*/

loop:

#if NCDEBUG
#  if nCUBE
    DBNCclient();
#  endif
#endif

#if nCUBE
/*
DBUG_PRINT ("DUMP", (""));
DBUG_PRINT ("DUMP", ("A-Stack:"));
DBUG_PRINT ("DUMP", ("Topaddresse: %x; Schotter: %x (class %d, type %d)",
                     ps_a->TopofStack,
                     TOP_A(),
                     ((IS_POINTER(TOP_A()))?R_DESC(*(T_PTD)TOP_A(),class):-1), 
                     ((IS_POINTER(TOP_A()))?R_DESC(*(T_PTD)TOP_A(),type):-1)));
if (d_stacksize(ps_a) > 1)
DBUG_PRINT ("DUMP", ("Unner dem liegt ey: %x (class %d, type %d)", 
                     MID_A(1),
                     ((IS_POINTER(MID_A(1)))?R_DESC(*(T_PTD)MID_A(1),class):-1),
                     ((IS_POINTER(MID_A(1)))?R_DESC(*(T_PTD)MID_A(1),type):-1)));
DBUG_PRINT ("DUMP", ("I_STACK:"));
DBUG_PRINT ("DUMP", ("Topaddresse: %x; Schotter: %x (class %d, type %d)", 
                     ps_i->TopofStack,
                     TOP_I(),
                     ((IS_POINTER(TOP_I()))?R_DESC(*(T_PTD)TOP_I(),class):-1),
                     ((IS_POINTER(TOP_I()))?R_DESC(*(T_PTD)TOP_I(),type):-1)));
if (d_stacksize(ps_i) > 1)
DBUG_PRINT ("DUMP", ("Unner dem liegt ey: %x (class %d, type %d)", 
                     MID_I(1),
                     ((IS_POINTER(MID_I(1)))?R_DESC(*(T_PTD)MID_I(1),class):-1),
                     ((IS_POINTER(MID_I(1)))?R_DESC(*(T_PTD)MID_I(1),type):-1)));
DBUG_PRINT ("DUMP", (""));
*/
#endif

#if nCUBE
DBUG_PRINT("INTER",("proc %x: next instruction: %s (%x),redcount: %d",curr_pid,instr_tab[*code].name,code,_redcnt));
#endif

#if nCUBE
  if (sig_msg_flag)
    msg_check();
#endif

/*  hpchain_inter(); */

  switch (*code++) {
    case I_EXT:      /* === exit === */
                     FUNC0("ext");
                     break;
    case I_PUSH:     /* === PUSH p : push a pointer onto the working stack === */
                     FUNCX("push");
                     INC_REFCNT((T_PTD)(*code));
                     PUSH_A(*code++);
                     goto loop;
    case I_PUSHC:    /* === PUSHC c : push a constant onto the working stack === */
                     FUNCN("pushc");
                     PUSH_A(*code++);
                     goto loop;
    case I_PUSHARG:  /* === PUSHARG n : push I-stackentry n onto the working stack === */
                     FUNC1("pusharg");
                     ptdesc = (T_PTD)MID_I(*code);
		     code++;                                         /* ach 08/01/93 */
                     T_INC_REFCNT(ptdesc);
                     PUSH_A(ptdesc);
                     goto loop;
    case I_PUSHARG0: /* === PUSHARG0 : push stackentry 0 onto the stack === */
                     FUNC0("pusharg0");
                     ptdesc = (T_PTD)TOP_I();
                     T_INC_REFCNT(ptdesc);
                     PUSH_A(ptdesc);
                     goto loop;
    case I_MOVE:     /* === MOVE : move one item from i to a === */
                     FUNC0("move");
                     PUSH_A(POP_I());
                     goto loop;
    case I_DELTA1:   /* === DELTA1 pf : perform unary delta-reduction === */
                     FUNCN("delta1");
                     func = *code++;
                   L_delta1:
                     /* func: primitive funktion (stackelement) */
                     if (_redcnt >= delta_count) {
                       if ((*(red_func_tab[FUNC_INDEX(func)]))(TOP_A())) {
                         /* _desc: resultat der funktionsanwendung */
                         WRITE_A(_desc);
                         _desc = NULL;
                         _redcnt -= delta_count;
                         goto loop;
                       }
                     }
                     PUSH_A(func);
                     arity = arg = 1;
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_DELTA2:   /* === DELTA2 pf : perform binary delta-reduction === */
                     FUNCN("delta2");
		     DBUG_PRINT("DELTA2",("func: %d; arg 1: %x (class %d, type %d); arg2: %x (class %d, type %d)",
					  *code,
					  TOP_A(),
					  ((IS_POINTER(TOP_A()))?R_DESC(*(T_PTD)TOP_A(),class):-1),
					  ((IS_POINTER(TOP_A()))?R_DESC(*(T_PTD)TOP_A(),type):-1),
					  MID_A(1),
					  ((IS_POINTER(MID_A(1)))?R_DESC(*(T_PTD)MID_A(1),class):-1),
					  ((IS_POINTER(MID_A(1)))?R_DESC(*(T_PTD)MID_A(1),type):-1)));
                     func = *code++;
                   L_delta2:
                     /* func: primitive funktion (stackelement) */
                     if (_redcnt >= delta_count) {
                       /* dg 30.01.92 argumente bleiben auf'm stack wg. scavenge */
                       if ((*(red_func_tab[FUNC_INDEX(func)]))(TOP_A(),MID_A(1))) {
                         /* _desc: resultat der funktionsanwendung */
                         PPOP_A();
                         WRITE_A(_desc);
                         _desc = NULL;
                         _redcnt -= delta_count;
                         goto loop;
                       }
                     }
                     PUSH_A(func);
                     arity = arg = 2;
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_DELTA3:   /* === DELTA3 pf : perform ternary delta-reduction === */
                     FUNCN("delta3");
                     func = *code++;
                   L_delta3:
                     /* func: primitive funktion (stackelement) */
                     if (_redcnt >= delta_count) {
                       /* dg 30.01.92 argumente bleiben auf'm stack wg. scavenge */
                       if ((*(red_func_tab[FUNC_INDEX(func)]))(TOP_A(),MID_A(1),MID_A(2))) {
                         /* _desc: resultat der funktionsanwendung */
                         PPOP_A();
                         PPOP_A();
                         WRITE_A(_desc);
                         _desc = NULL;
                         _redcnt -= delta_count;
                         goto loop;
                       }
                     }
                     PUSH_A(func);
                     arity = arg = 3;
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_DELTA4:   /* === DELTA4 pf : perform quaternary delta-reduction === */
                     FUNCN("delta4");
                     func = *code++;
                   L_delta4:
                     /* func: primitive funktion (stackelement) */
                     if (_redcnt >= delta_count) {
                       /* dg 30.01.92 argumente bleiben auf'm stack wg. scavenge */
                       if ((*(red_func_tab[FUNC_INDEX(func)]))(TOP_A(),MID_A(1),MID_A(2),MID_A(3))) {
                         /* _desc: resultat der funktionsanwendung */
                         PPOP_A();
                         PPOP_A();
                         PPOP_A();
                         WRITE_A(_desc);
                         _desc = NULL;
                         _redcnt -= delta_count;
                         goto loop;
                       }
                     }
                     PUSH_A(func);
                     arity = arg = 4;
                     COMM0("mkdclos");
                     goto L_mkdclos;

/* --- shortcut : directly supported primitive functions --- */

#define EXEC_DFUNC(pf_test,pf_makro,pf_func,pf_stel) \
                     arg = TOP_A(); \
                     if (pf_test(arg,MID_A(1))) { \
                       PPOP_A(); \
                       WRITE_A(pf_makro(arg,TOP_A())); \
                       goto loop; \
                     } \
                     if (pf_func(arg,MID_A(1))) { \
                       PPOP_A(); \
                       WRITE_A(_desc); \
                       _desc = NULL; \
                       goto loop; \
                     } \
                     /* failure */ \
                     PUSH_A(pf_stel); \
                     arity = arg = 2; \
                     COMM0("mkdclos"); \
                     goto L_mkdclos

#define EXEC_MFUNC(pf_test,pf_makro,pf_func,pf_stel) \
                     if (pf_test(TOP_A())) { \
                       WRITE_A(pf_makro(TOP_A())); \
                       goto loop; \
                     } \
                     if (pf_func(TOP_A())) { \
                       WRITE_A(_desc); \
                       _desc = NULL; \
                       goto loop; \
                     } \
                     /* failure */ \
                     PUSH_A(pf_stel); \
                     arity = arg = 1; \
                     COMM0("mkdclos"); \
                     goto L_mkdclos

                     /* ------------------ */

    case I_ADD:      /* === ADD : addition === */
                     FUNC0("add");
                     EXEC_DFUNC(T_2INT,ADD_INT,red_plus,D_PLUS);
    case I_SUB:      /* === SUB : subtraction === */
                     FUNC0("sub");
                     EXEC_DFUNC(T_2INT,SUB_INT,red_minus,D_MINUS);
    case I_MUL:      /* === MUL : multiplication === */
                     FUNC0("mul");
                     EXEC_DFUNC(T_2INT,MUL_INT,red_mult,D_MULT);
#define I_SAY_NO(x,y) 0
    case I_DIV:      /* === DIV : division (returns real), dirty === */
                     FUNC0("div");
                     EXEC_DFUNC(I_SAY_NO,DIV_INT,red_div,D_DIV);
#undef I_SAY_NO
    case I_MOD:      /* === MOD : modulo === */
                     FUNC0("mod");
                     EXEC_DFUNC(T_2INT,MOD_INT,red_mod,D_MOD);
    case I_EQ:       /* === EQ : equal === */
                     FUNC0("eq");
                     EXEC_DFUNC(T_2INT,EQ_INT,red_eq,D_EQ);
    case I_NE:       /* === NE : not equal === */
                     FUNC0("ne");
                     EXEC_DFUNC(T_2INT,NE_INT,red_neq,D_NEQ);
    case I_LT:       /* === LT : less than === */
                     FUNC0("lt");
                     EXEC_DFUNC(T_2INT,LT_INT,red_lt,D_LESS);
    case I_LE:       /* === LE : less than or equal === */
                     FUNC0("le");
                     EXEC_DFUNC(T_2INT,LE_INT,red_le,D_LEQ);
    case I_GT:       /* === GT : greater than === */
                     FUNC0("gt");
                     EXEC_DFUNC(T_2INT,GT_INT,red_gt,D_GREAT);
    case I_GE:       /* === GE : greater than or equal === */
                     FUNC0("ge");
                     EXEC_DFUNC(T_2INT,GE_INT,red_geq,D_GEQ);
    case I_NEG:      /* === NEG : change sign === */
                     FUNC0("neg");
                     EXEC_MFUNC(T_INT,NEG_INT,red_neg,M_NEG);
    case I_NOT:      /* === NOT : boolean not === */
                     FUNC0("not");
                     EXEC_MFUNC(T_BOOLEAN,NOT_BOOL,red_not,M_NOT);
    case I_OR:       /* === OR : not exclusive or === */
                     FUNC0("or");
                     EXEC_DFUNC(T_2BOOLEAN,OR_BOOL,red_or,D_OR);
    case I_AND:      /* === AND : and === */
                     FUNC0("and");
                     EXEC_DFUNC(T_2BOOLEAN,AND_BOOL,red_and,D_AND);
    case I_XOR:      /* === XOR : exclusive or === */
                     FUNC0("xor");
                     EXEC_DFUNC(T_2BOOLEAN,XOR_BOOL,red_xor,D_XOR);

/* -------------------------------------------------------- */

    case I_PUSHRET:  /* === PUSHRET r : push return address === */
                     FUNCL("pushret");
                     PUSH_R(*code++);
                     SET_TAIL_FLAG();
                     goto loop;
    case I_JFALSE:   /* === JFALSE l : perform conditional-reduction (per jump) === */
                     FUNCL("jfalse");
                     /* offenes conditional, das praed. liegt oben auf a     */
                     /* falls pred = true/false wird zum then/else-code      */
                     /* gesprungen, ansonsten wird geschlossen.              */
                     arg = POP_A();                   /* arg:    praedikat   */
                     if (IS_TRUE(arg)) {
                       /* then-code folgt */
                       code++;
                       goto loop;
                     }
                     if (IS_FALSE(arg))  {
                       code = (INSTR *)*code;
                       goto loop;
                     }
                     /* argument nicht true/false */
                     goto L_condfail;
    case I_JTRUE:    /* === JTRUE l : perform conditional-reduction (per jump) === */
                     FUNCL("jtrue");
                     /* offenes conditional, das praed. liegt oben auf a     */
                     /* falls pred = true/false wird zum then/else-code      */
                     /* gesprungen, ansonsten wird geschlossen.              */
                     arg = POP_A();                   /* arg:    praedikat   */
                     if (IS_FALSE(arg)) {
                       /* else-code folgt */
                       code++;
                       goto loop;
                     }
                     if (IS_TRUE(arg))  {
                       code = (INSTR *)*code;
                       goto loop;
                     }
                     /* argument nicht true/false */
                     goto L_condfail;
    case I_JCOND:    /* === JCOND l : perform conditional-reduction (per jump) === */
                     FUNCL("jcond");
                     /* offenes conditional, das praed. liegt oben auf a     */
                     /* falls pred = true/false wird zum then/else-code      */
                     /* gesprungen, ansonsten wird geschlossen.              */
                     arg = POP_A();                   /* arg:    praedikat   */
                     if (_redcnt >= delta_count) {
                       if (T_SA_TRUE(arg)) {
                         _redcnt -= delta_count;
                         /* then-code folgt */
                         code++;
                         goto loop;
                       }
                       if (T_SA_FALSE(arg))  {
                         _redcnt -= delta_count;
                         code = (INSTR *)*code;
                         goto loop;
                       }
                     }
                     /* conditional nicht reduzierbar, schliessen */
                   L_condfail:
                     PPUSH_A(arg);                        /* praedikat */
                     ptdesc = (T_PTD)*code++;             /* ptdesc: zeiger auf cond-code */
                     GRAB_DESC(ptdesc);                   /* ptdesc: conditional-descriptor */
                     INC_REFCNT(ptdesc);                  /* refcount */
                     PPUSH_A(ptdesc);                     /* conditional */
                     arity = R_CONDI(*ptdesc,nargs);      /* anzahl argumente (incl. praedikat) */
                     MAKEDESC(_desc,1,C_FUNC,TY_CLOS);    /* descriptor holen und initialisieren */
                     GET_HEAP(arity+1,A_CLOS(*_desc,pta)); /* heap holen und einhaengen */
                     L_CLOS(*_desc,args) = arity;         /* descriptor fuellen */
                     L_CLOS(*_desc,nargs) = 0;
                     L_CLOS(*_desc,ftype) = TY_CONDI;
                     RES_HEAP;
                     pth = (PTR_HEAPELEM)R_CLOS(*_desc,pta);
                     *pth++ = (T_HEAPELEM)TOP_A();        /* conditional */
                     if (R_CONDI(*(T_PTD)TOP_A(),args)) {
                      /* argumente duerfen nicht aufgebraucht werden */
                       register T_PTD ptd;
                       for (help = 0; --arity > 0 ; help++) {
                         ptd = (T_PTD)MID_I(help);
                         T_INC_REFCNT(ptd);
                         *pth++ = (T_HEAPELEM)ptd;
                       }
                     }
                     else {
                       /* argumente duerfen aufgebraucht werden */
                       while (--arity > 0) {
                         *pth++ = (T_HEAPELEM)POP_I();
                       }
                     }
                     PPOP_A();                            /* conditional */
                     *pth = (T_HEAPELEM)TOP_A();          /* praedikat   */
                     REL_HEAP;
                     WRITE_A(_desc);
                     _desc = NULL;
                     goto L_rtf;
    case I_BETA:     /* === BETA f : perform n-ary function call === */
                     FUNCL("beta");
                     ptdesc = (T_PTD)*code++;
                   L_beta:
                     /* ptdesc: funktion (zeiger auf descriptor/code) */
                     if (_redcnt >= beta_count) {
                       _redcnt -= beta_count;        /* count reduction   */
                       SWITCHSTACKS();
                       PUSH_R(code);                 /* return address    */
                       code = (INSTR *)ptdesc;       /* subroutine call   */
                       goto loop;
                     }
                     /* descriptor holen */
                   L_betafail:
                     GRAB_DESC(ptdesc);              /* descriptor holen */
                     INC_REFCNT(ptdesc);             /* prepare for push */
                     PUSH_A(ptdesc);                 /* push function    */
                     arg = R_COMB(*ptdesc,nargs);    /* args needed      */
                     if (arg == 0) goto loop;        /* keine closure    */
                     arity = arg;                    /* number of args   */
                     COMM0("mkbclos");
                     goto L_mkbclos;
    case I_BETA0:    /* === BETA0 f : perform 0-ary function call === */
                     FUNCL("beta0");
                     /* ptdesc: funktion (zeiger auf descriptor/code) */
                     if (_redcnt >= beta_count) {
                       _redcnt -= beta_count;        /* count reduction   */
                       PUSH_R(code+1);               /* return address    */
                       SET_TAIL_FLAG();
                       code = (INSTR *)*code;        /* subroutine call   */
                       goto loop;
                     }
                     ptdesc = (T_PTD)*code++;
                     goto L_betafail;
    case I_BETA1:    /* === BETA1 f : perform 1-ary function call === */
                     FUNCL("beta1");
                     /* ptdesc: funktion (zeiger auf descriptor/code) */
                     if (_redcnt >= beta_count) {
                       _redcnt -= beta_count;        /* count reduction   */
                       PUSH_I(POP_A());              /* move argument */
                       PUSH_R(code+1);               /* return address    */
                       SET_TAIL_FLAG();
                       code = (INSTR *)*code;        /* subroutine call   */
                       goto loop;
                     }
                     ptdesc = (T_PTD)*code++;
                     goto L_betafail;
    case I_JTAIL:    /* === JTAIL f : n-ary tail end recursion === */
                     FUNCL("jtail");
                     /* vorsicht: tail-aufrufe sind sensibel wegen dem */
                     /* stackswitch. anhand eines TAIL_FLAGS auf dem   */
                     /* r-stack wird bei rtf entschieden ob ein stack- */
                     /* switch gemacht wird oder nicht.                */
                     if (_redcnt >= beta_count) {
                       _redcnt -= beta_count;
                       SWITCHSTACKS();
                       INVERT_TAIL_FLAG();
                       code = (INSTR *)*code;
                       goto loop;
                     }
                     ptdesc = (T_PTD)*code++;
                     goto L_betafail;
    case I_JTAIL0:   /* === JTAIL0 f : 0-ary tail end recursion === */
                     FUNCL("jtail0");
                     /* ptdesc: funktion (zeiger auf descriptor/code) */
                     if (_redcnt >= beta_count) {
                       _redcnt -= beta_count;
                       code = (INSTR *)*code;
                       goto loop;
                     }
                     ptdesc = (T_PTD)*code++;
                     goto L_betafail;
    case I_JTAIL1:   /* === JTAIL1 f : 1-ary tail end recursion === */
                     FUNCL("jtail1");
                     /* ptdesc: funktion (zeiger auf descriptor/code) */
                     if (_redcnt >= beta_count) {
                       _redcnt -= beta_count;
                       PUSH_I(POP_A());
                       code = (INSTR *)*code;
                       goto loop;
                     }
                     ptdesc = (T_PTD)*code++;
                     goto L_betafail;
    case I_BETAQ:    /* === BETAQ f : perform quick n-ary function call === */
                     FUNCL("betaq");
                     SWITCHSTACKS();
                     PUSH_R(code+1);               /* return address    */
                     code = (INSTR *)*code;        /* subroutine call   */
                     goto loop;
    case I_BETAQ0:   /* === BETAQ0 f : perform quick 0-ary function call === */
                     FUNCL("betaq0");
                     PUSH_R(code+1);               /* return address    */
                     SET_TAIL_FLAG();
                     code = (INSTR *)*code;        /* subroutine call   */
                     goto loop;
    case I_BETAQ1:   /* === BETAQ1 f : perform quick 1-ary function call === */
                     FUNCL("betaq1");
                     PUSH_I(POP_A());
                     PUSH_R(code+1);               /* return address    */
                     SET_TAIL_FLAG();
                     code = (INSTR *)*code;        /* subroutine call   */
                     goto loop;
    case I_JTAILQ:   /* === JTAILQ f : perform quick n-ary tail call === */
                     FUNCL("jtailq");
                     SWITCHSTACKS();
                     INVERT_TAIL_FLAG();
                     code = (INSTR *)*code;
                     goto loop;
    case I_JTAILQ0:  /* === JTAILQ0 f : perform quick 0-ary tail call === */
                     FUNCL("jtailq0");
                     code = (INSTR *)*code;
                     goto loop;
    case I_JTAILQ1:  /* === JTAILQ1 f : perform quick 1-ary tail call === */
                     FUNCL("jtailq1");
                     PUSH_I(POP_A());
                     code = (INSTR *)*code;
                     goto loop;
    case I_MKLIST:   /* === MKLIST n : make n-ary list === */
                     FUNC1("mklist");
                     arity = *code++;
                     /* arity: anzahl der listenelemente (>0!) */
                     /* store list, vgl. store_l in rstpro.c */
                     /* dg 30.01.92 verwendung des globalen _desc wg. scavenge */
                     GET_DESC(_desc);
                     LI_DESC_MASK(_desc,1,C_LIST,TY_UNDEF);
                     GET_HEAP(arity,A_LIST(*_desc,ptdv));
                     L_LIST(*_desc,dim) = arity;
                     RES_HEAP;
                     pth = R_LIST(*_desc,ptdv);
                     while (--arity > 0)
                       pth[arity] = (T_HEAPELEM)POP_A();
                     pth[arity] = (T_HEAPELEM)TOP_A();
                     REL_HEAP;
                     WRITE_A(_desc);
                     _desc = NULL;
                     goto loop;
    case I_FREE:     /* === FREE n : free an n-size argument frame === */
                     FUNC1("free");
                     arg = *code++;
                     /* arg: anzahl der von i freizugebenden argumente (>0!) */
                     do {
                       ptdesc = (T_PTD)POP_I();
                       T_DEC_REFCNT(ptdesc);
                     } while (--arg > 0);
                     goto loop;
    case I_FREE1:    /* === FREE1 : free one argument === */
                     FUNC0("free1");
                     ptdesc = (T_PTD)POP_I();
                     T_DEC_REFCNT(ptdesc);
                     goto loop;
    case I_RTF:      /* === RTF : return function value === */
                     FUNC0("rtf");
                   L_rtf:
                     if (TEST_TAIL_FLAG()) {
                       DEL_TAIL_FLAG();
                       code = POP_RET();             /* returnadresse */
                       goto loop;
                     }
                     SWITCHSTACKS();
                     PUSH_A(POP_I());                /* move result */
                     code = POP_RET();             /* returnadresse */
                     goto loop;
    case I_RTM:      /* === RTM : move conditionally from a to i and return === */
                     FUNC0("rtm");
                     if (TEST_TAIL_FLAG()) {
                       DEL_TAIL_FLAG();
                       PUSH_A(POP_I());                /* move */
                     }
                     else {
                       SWITCHSTACKS();
                     }
                     code = POP_RET();             /* returnadresse */
                     goto loop;
    case I_RTP:      /* === RTP p : return pointer === */
                     FUNCX("rtp");
                     ptdesc = (T_PTD)*code;
                     if (TEST_TAIL_FLAG()) {
                       DEL_TAIL_FLAG();
                     }
                     else {
                       SWITCHSTACKS();
                     }
                     INC_REFCNT(ptdesc);
                     PUSH_A(ptdesc);
                     code = POP_RET();             /* returnadresse */
                     goto loop;
    case I_RTC:      /* === RTC c : return constant === */
                     FUNCN("rtc");
                     ptdesc = (T_PTD)*code;
                     if (TEST_TAIL_FLAG()) {
                       DEL_TAIL_FLAG();
                     }
                     else {
                       SWITCHSTACKS();
                     }
                     PUSH_A(ptdesc);
                     code = POP_RET();             /* returnadresse */
                     goto loop;
    case I_RTS:      /* === RTS : return from subroutine === */
                     FUNC0("rts");
                     code = POP_RET();             /* returnadresse */
                     goto loop;
    case I_MKGCLOS:  /* === MKGCLOS n : make general-closure === */
                     FUNC1("mkgclos");
                     arity = *code++;       /* vorhandene argumente */
                   L_mkgclos:
                     arg = arity;
                     func = TY_CLOS;        /* CLOS-flag      */
                     goto L_mkclos;
    case I_MKBCLOS:  /* === MKBCLOS n m : make beta-closure === */
                     FUNC2("mkbclos");
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
                   L_mkbclos:
                     func = TY_COMB;        /* COMB-flag  */
                     goto L_mkclos;
    case I_MKCCLOS:  /* === MKCCLOS n m : make cond-closure === */
                     FUNC2("mkcclos");
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
                   L_mkcclos:
                     func = TY_CONDI;       /* COND-flag */
                     goto L_mkclos;
    case I_MKSCLOS:  /* === MKSCLOS n m : make switch/case-closure === */
                     FUNC2("mksclos");
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
                   L_mksclos:
                     func = TY_CASE;        /* CASE-flag */
                     goto L_mkclos;
    case I_MKDCLOS:  /* === MKDCLOS n m : make delta-closure === */
                     FUNC2("mkdclos");
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
                   L_mkdclos:
                     func = TY_PRIM;        /* PRIM(FUNC)-flag */
                   L_mkclos:
                     /* stack a: oben die funktion, darunter die argumente */
                     /* arity: anzahl der argumente auf'm stack (>0!)      */
                     /* arg: anzahl der von der fkt. benoetigten argumente */
                     /* func: flag das den typ der funktion angibt         */
                     if (arity == 0) goto loop;
                     /* dg 30.01.92 verwendung des globalen _desc wg. scavenge */
                     MAKEDESC(_desc,1,C_FUNC,TY_CLOS);
                     GET_HEAP(arg+1,A_CLOS(*_desc,pta));
                     L_CLOS(*_desc,args) = arity;
                     L_CLOS(*_desc,nargs) = arg - arity;
                     L_CLOS(*_desc,ftype) = func;      /* flag */
                     RES_HEAP;
                     pth = (PTR_HEAPELEM)R_CLOS(*_desc,pta);
                     do {
                       *pth++ = (T_HEAPELEM)POP_A();
                     } while (--arity >= 0);
                     REL_HEAP;

                     DBUG_PRINT ("INTER", ("refcnt: %d", (((T_PTD)_desc)->u.sc.ref_count)));

                     PPUSH_A(_desc);
                     _desc = NULL;
                     goto loop;
    case I_MKAP:     /* === MKAP n : make application (not reducible at all) === */
                     FUNC1("mkap");
                     arity = *code++;
                     /* arity: anzahl der argumente (>0!)                   */
                     /* stack a: oben die "funktion" darunter die argumente */
                     goto L_mkap;
    case I_APPLY:    /* === APPLY n : perform application === */
                     FUNC1("apply");
                     arity = *code++;
                     goto L_apply;

                     /* --- specials --- */

    case I_SAPPLY:   /* === SAPPLY : special apply === */
                     FUNC0("sapply");
                     arity = POP_R();       /* anz. der arg. vom stack r */
                     COMM0("apply");
                     goto L_apply;
    case I_SRET:     /* === SRET : special free and return === */
                     FUNC0("sret");
                     arity = POP_E(); /* POP_M() */ /* anz. der arg. vom stack e */
                     for ( ; --arity >= 0 ; ) {
                       ptdesc = (T_PTD)POP_I();
                       T_DEC_REFCNT(ptdesc);
                     }
                     code = POP_RET();             /* returnadresse */
                     goto loop;

                     /* --- specials for postprocessing --- */

    case I_MKTHEN:   /* === MKTHEN : close then-expression === */
                     /* stack a |.. arg_n .. arg_1 ptcond */
                     FUNC0("mkthen");
                     PUSH_R(_redcnt);                         /* save redcnt */
                     _redcnt = delta_count;                   /* reset       */
                     ptdesc = (T_PTD)POP_A();                 /* conditional */
                     PUSH_R(ptdesc);                          /* conditional */
                     if (R_CONDI(*ptdesc,args) == 0) {        /* jtrue       */
                       /* conditional gibt die argumente frei */
                       register T_PTD ptd;
                       arity = R_CONDI(*ptdesc,nargs)-2;
                       /* copy arguments */
                       for (arg = arity ; arg >= 0 ; arg-- ) {
                         ptd = (T_PTD)MID_A(arity);
                         T_INC_REFCNT(ptd);
                         PUSH_A(ptd);
                       }
                     }
                     SWITCHSTACKS();
                     PUSH_A(SA_TRUE);
                     PUSH_R(code);                            /* return addr */
                     code = R_CONDI(*ptdesc,ptc);
                     goto loop;
    case I_MKELSE:   /* === MKELSE : close else-expression === */
                     /* stack r |.. _redcnt ptcond, stack a |.. then  */
                     FUNC0("mkelse");
                     _redcnt = delta_count;                   /* reset       */
                     ptdesc = (T_PTD)TOP_R();                 /* conditional */
                     PUSH_R(POP_A());                         /* then-expr   */
                     if (R_CONDI(*ptdesc,args) == 0) {        /* jtrue       */
                       /* conditional gibt die argumente frei */
                       register T_PTD ptd;
                       arity = R_CONDI(*ptdesc,nargs)-2;
                       for (arg = arity ; arg >= 0 ; arg-- ) {
                         ptd = (T_PTD)MID_A(arity);
                         T_INC_REFCNT(ptd);
                         PUSH_A(ptd);
                       }
                     }
                     SWITCHSTACKS();
                     PUSH_A(SA_FALSE);
                     PUSH_R(code);                            /* return addr */
                     code = R_CONDI(*ptdesc,ptc);
                     goto loop;
    case I_MKCOND:   /* === MKCOND : make conditional === */
                     /* stack r |.. _redcnt ptcond then, stack a: |.. else */
                     FUNC0("mkcond");
                     arg = POP_A();                  /* else-expr      */
                     arity = POP_R();                /* then-expr      */
                     ptdesc = (T_PTD)POP_R();        /* conditional    */
                     DEC_REFCNT(ptdesc);             /* wird nicht frei */
                     PUSH_A(arity);                  /* then-expr      */
                     PUSH_A(arg);                    /* else-expr      */
                     PUSH_A(SET_ARITY(COND,2));      /* cond-constr    */
                     _redcnt = POP_R();              /* alter _redcnt  */
                     goto loop;
    case I_CASE:     /* === CASE f : execute case === */
                     FUNCL("case");
                     ptdesc = (T_PTD)*code++;
                   L_case:
                     if (_redcnt >= delta_count) {
                       _redcnt -= delta_count;
                       PUSH_R(code);
                       SET_TAIL_FLAG();   
                       code = (INSTR *)ptdesc;       /* subroutine call   */
                       goto loop;
                     }
                     /* case schliessen */
                     GRAB_DESC(ptdesc);         /* descriptor holen */
                     func = TY_CASE;
                     COMM0("nomatch");
                     goto L_nomatch;
    case I_WHEN:     /* === WHEN f : execute pattern match === */
                     FUNCX("when");
                     ptdesc = (T_PTD)*code++;
                     RES_HEAP;       /* heap wird in pm_am freigegeben !!! */
                     pm_am(R_MATCH(*ptdesc,code),TOP_A());
                     /* pm_am hinterlaesst auf stack a SA_TRUE (DOLLAR auf m) */
                     /* falls das pattern passt, HASH falls das pattern nicht */
                     /* passt und KLAA falls das matching nicht entscheidbar */
                     goto loop;
    case I_GUARD:    /* === GUARD f : reduce guard === */
                     FUNCL("guard");
                     ptdesc = (T_PTD)*code++;
                     if (T_HASH(TOP_A())) {
                       /* pattern passt nicht, body instr. folgt */
                       goto loop;
                     }
                     if (T_SA_TRUE(TOP_A())) {
                       /* pattern passt */
                       PPOP_A();                   /* SA_TRUE */
                       PUSH_R(code);               /* return adresse */
                       SET_TAIL_FLAG();            /* TAIL-call */
                       code = (INSTR *)ptdesc;     /* subroutine jump */
                       goto loop;
                     }
                     /* matching unentscheidbar */
                     GRAB_DESC(ptdesc);            /* GUARD descriptor holen */
                     goto L_undecided;
    case I_BODY:     /* === BODY f : reduce body === */
                     FUNCL("body");
                     ptdesc = (T_PTD)*code++;
                     if (T_HASH(TOP_A())) {
                       /* pattern passt nicht */
                       PPOP_A();                   /* hash symbol */
                       goto loop;
                     }
                     if (T_SA_TRUE(TOP_A())) {
                       /* pattern passt und guard true */
                       register T_PTD ptd;
                       PPOP_A();                   /* SA_TRUE */
                       if (T_DOLLAR(TOP_R()))
                         PPOP_R();                 /* anzahl der patternvariablen */
                       ptd = (T_PTD)POP_A();       /* das zu matchende argument */
                       if (T_POINTER((int)ptd))
                         DEC_REFCNT(ptd);
                       code = (INSTR *)ptdesc;     /* jump   */
                       goto loop;
                     }
                     if (T_SA_FALSE(TOP_A())) {
                       /* pattern passt aber guard false */
                       register T_PTD ptd;
                       PPOP_A();                   /* SA_FALSE */
                       arg = VALUE(POP_R());       /* anzahl der patternvariablen */
                       /* variableninstanzen freigeben */
                       for (; arg > 0 ; arg--) {
                         ptd = (T_PTD)POP_I();
                         if (T_POINTER((int)ptd))
                           DEC_REFCNT(ptd);
                       }
                       goto loop;
                     }
                     /* guard weder true noch false */
                     GRAB_DESC(ptdesc);              /* BODY descriptor holen */
                   L_undecided:
                     {
                       register T_PTD ptd;
                       register int i;
                       /* NOMAT descriptor anlegen */
                       MAKEDESC(ptd,1,C_EXPRESSION,TY_NOMAT);
                       /* nr. des fehlgeschlagenen when eintragen */
                       L_NOMAT(*ptd,act_nomat) = R_CASE(*ptdesc,args);
                       ptdesc = R_CASE(*ptdesc,ptd);        /* CASE descriptor */
                       arity = R_CASE(*ptdesc,nargs);       /* gesamtanzahl argumente des CASE*/
                       ptdesc = R_CASE(*ptdesc,ptd);        /* SWITCH descriptor */
                       arg  = R_SWITCH(*ptdesc,case_type);  /* NCASE: stelligkeit des SWITCH */
                       INC_REFCNT(ptdesc);
                       L_NOMAT(*ptd,ptsdes) = ptdesc;       /* SWITCH descriptor */

                       if (T_KLAA(TOP_A())) {
                         /* match nicht entscheidbar */
                         COMM0("match undecided");
                         L_NOMAT(*ptd,guard_body) = 0;        /* no GUARD/BODY */
                         PPOP_A();                            /* KLAA */
                       }
                       else {
                         /* pattern passte, aber guard weder true noch false */
                         COMM0("guard undecided"); 
                         /* MATCH descriptor holen */
                         ptdesc = (T_PTD)R_SWITCH(*ptdesc,ptse)[R_NOMAT(*ptd,act_nomat)];
                         /* das ist ein gewuerge hier! */
                         load_expr(A_MATCH(*ptdesc,body));  /* body (auf e) einlagern */
                         if (T_SNAP(TOP_E())) {
                           /* nummernvariablen ersetzen */
                           /* VALUE(TOP_R(): anzahl der pm-bindungen auf a */
                           /* arity-arg: anzahl der freien variablen des case */
                           help = arity-arg-1+VALUE(TOP_R());
                           /* help ist der index der untersten variablenbindung auf i */
                           for (i = ARITY(TOP_E()); i > 0 ; i-- ) {
                             if (T_NUM(MID_E(i))) {
                               ptdesc = (T_PTD)MID_I(help-VALUE(MID_E(i))); /* adressierung "von unten" */
                               T_INC_REFCNT(ptdesc);
                               UPDATE_E(i,ptdesc);
                             }
                           }
                         }
                         if ((i = VALUE(POP_R())) > 0 ) {
                           /* i: anz pm-bindungen >0 also body schliessen */
                           help = T_SNAP(TOP_E()) ? ARITY(POP_E())+i : i+1;
                           for (; i > 0 ; i--)
                             PUSH_E(POP_I());
                           PUSH_E(SET_ARITY(SNAP,help));
                         }
                         trav_e_a();
                         if ((ptdesc = (T_PTD)st_expr()) == NULL)
                           POST_MORTEM("inter: heap overflow (body)");
                         /* NOMAT-descriptor anlegen */
                         GET_HEAP(3,A_NOMAT(*ptd,guard_body));
                         L_NOMAT(*ptd,guard_body)[0] = 2;            /* laenge */
                         L_NOMAT(*ptd,guard_body)[1] = POP_A();      /* guard */
                         L_NOMAT(*ptd,guard_body)[2] = (int)ptdesc;  /* body */
                       }
                       /* ptd: NOMAT descriptor */
                       /* arity : anzahl argumente des CASE */
                       /* arg : stelligkeit des SWITCH (NCASE) */
                       if (arg > 1) {
                         /* NCASE: multiple pattern, load argument list onto the stack */
                         load_pmlist(ps_a,POP_A());
                       }
                       /* close NOMAT-descriptor */
                       MAKEDESC(ptdesc,1,C_EXPRESSION,TY_EXPR);
                       arity += (arity > arg) ? 4 : 3;
                       GET_HEAP(arity,A_EXPR(*ptdesc,pte));
                       RES_HEAP;
                       pth    = (PTR_HEAPELEM)R_EXPR(*ptdesc,pte); /* heap vector */
                       *pth++ = (T_HEAPELEM)(--arity);             /* groesse     */
                       *pth++ = (T_HEAPELEM)SET_ARITY(AP,arg+1);   /* AP */
                       for (i = arg; --i >=0 ; --arity)
                         *pth++ = (T_HEAPELEM)POP_A();             /* argument */
                       arity -= 2;
                       if (arity > 0) {
                         *pth++ = SET_ARITY(SNAP,arity);
                         for (--arity ; --arity >= 0 ;) {
                           if (T_POINTER(MID_I(arity)))
                             INC_REFCNT((T_PTD)MID_I(arity));
                           *pth++ = (T_HEAPELEM)MID_I(arity);
                         }
                       }
                       *pth++ = (T_HEAPELEM)ptd;          /* NOMAT descr */
                       REL_HEAP;
                       PPUSH_A(ptdesc);                   /* EXPR descriptor */
                       code = rout_rtf;                   /* nachher return */
                       goto loop;
                     }
    case I_NOMATCH:  /* === NOMATCH f : handle nomatch === */
                     FUNCX("nomatch");
                     ptdesc = (T_PTD)*code++;
                     _redcnt += delta_count;
                     func = TY_CASE;

                     /* NCASE */
                     if ((int)R_SWITCH(*R_CASE(*ptdesc,ptd),case_type) > 1) {
                       /* multiple pattern, load argument list onto the stack */
                       load_pmilist(ps_a,POP_A());
                     }

                   L_nomatch:
                     /* ptdesc: case (nicht auf'm stack) */
                     /* func:   function type (TY_COMB, TY_CASE ...) */
                     INC_REFCNT(ptdesc);
                     arity = R_CASE(*ptdesc,nargs);   /* anz. arg. */
                     /* NCASE: stelligkeit des original SWITCH ermitteln */
                     arg = R_SWITCH(*R_CASE(*ptdesc,ptd),case_type);
                     help = (int)ptdesc;              /* case */
                     MAKEDESC(ptdesc,1,C_FUNC,TY_CLOS);
                     GET_HEAP(arity+1,A_CLOS(*ptdesc,pta));
                     L_CLOS(*ptdesc,args) = arity;
                     L_CLOS(*ptdesc,nargs) = 0;
                     L_CLOS(*ptdesc,ftype) = func;
                     RES_HEAP;
                     pth = (PTR_HEAPELEM)R_CLOS(*ptdesc,pta);
                     *pth++ = (T_HEAPELEM)help;    /* case */
                     /* NCASE: anzahl der freien variablen ermitteln und auf a pushen */
                     { int i = arity-arg;
                       for (arg = 0 ; --i >= 0 ; arg++, arity--) {
                         help = MID_I(arg);
                         if (T_POINTER(help))
                           INC_REFCNT((T_PTD)help);
                         *pth++ = (T_HEAPELEM)help;
                       }
                     }
                     /* NCASE: argumente in die closure packen */
                     for ( ; --arity >= 0 ; ) {
                       *pth++ = POP_A();          /* argument */
                     }
                     REL_HEAP;
                     PPUSH_A(ptdesc);
                     goto loop;
    case I_DUPARG:   /* === DUPARG n : duplicate argument n (on stack i) === */
                     FUNC1("duparg");
                     arg = *code++;
                     ptdesc = (T_PTD)MID_I(arg);
                     T_INC_REFCNT(ptdesc);
                     PUSH_I(ptdesc);
                     goto loop;
    case I_LSEL:     /* === LSEL : select list element (inline) === */
                   L_lsel:
                     FUNC0("lsel");
                     if (_redcnt >= delta_count) {
                       /* red_lselect inline (vgl. rlstruct.c) */
                       arg = POP_A();         /* index */
                       arity = TOP_A();       /* list/string */
                       if (T_POINTER(arity)) {
                         register T_PTD ptd1 = (T_PTD)arg;
                         register T_PTD ptd2 = (T_PTD)arity;
                         int index;
                         if (R_DESC(*ptd2,class) == C_LIST) {
                           /* index extrahieren */
                           if (T_INT(arg))
                             index = VAL_INT(arg);
                           else
                           if (T_POINTER(arg))
                             if (R_DESC(*ptd1,class) == C_SCALAR) {
                               index = R_SCALAR(*ptd1,valr);
                               if (index != R_SCALAR(*ptd1,valr))
                                 index = 0; /* weil nicht ganzzahlig! */
                             }
                             else
                             if (R_DESC(*ptd1,class) == C_DIGIT
                              && conv_digit_int(arg,&index))
                               ; /* konvertierung hat geklappt */
                             else index = 0;
                           else index = 0;
                           /* damit ist der index extrahiert */
                           if (0 < index && index <= (int) R_LIST(*ptd2,dim)) {
              /* int gecastet von RS 5.11.1992 */
                             /* element aus der liste rausholen */
                             ptdesc = (T_PTD)R_LIST(*ptd2,ptdv)[--index];
                             T_DEC_REFCNT(ptd1);       /* parameter freigeben */
                             _redcnt -= delta_count;
                             if (T_POINTER((int)ptdesc)) {
                               INC_REFCNT(ptdesc);
                               WRITE_A(ptdesc);
                               /* LAZY LISTS */
                               if (LazyLists
                                && R_DESC(*ptdesc,class) == C_FUNC
                                && R_UNIV(*ptdesc,nargs) == 0) {
                                 if (R_DESC(*ptd2,ref_count) > 1) {
                                   /* eval und update */
                                   PUSH_I(index);
                                   PUSH_I(ptd2);              /* liste */
                                   PUSH_R(code);
                                   code = rout_updat;
                                 }
                                 else {
                                   DEC_REFCNT(ptd2);   /* liste freigeben */
                                 }
                                 COMM0("eval");
                                 goto L_eval;
                               } /* end LAZY LISTS */
                             } /* end T_POINTER */
                             else {
                               /* kein pointer, character? */
                               if (T_STR_CHAR((int)ptdesc)) {
                                 /* indirekten descriptor fuer char anlegen */
                                 ++index;
                                 ptdesc = gen_id(index,index,ptd2);
                               }
                               WRITE_A(ptdesc);
                             }
                             DEC_REFCNT(ptd2);       /* liste freigeben */
                             goto loop;
                           }
                           /* index nicht im bereich */
                         }
                         /* keine liste */
                       }
                       /* argumente sind nicht pointer, lselect klappt nie! */
                       PPUSH_A(arg);
                     }
                     PUSH_A(LSELECT);
                     arity = arg = 2;
                     COMM0("mkdclos");
                     goto L_mkdclos;

/* #ifdef LAZY */
                     /* --- lazy specials --- */

    case I_EVAL:     /* === EVAL : evaluate === */
                     FUNC0("eval");
                     /* hier wird vorausgesetzt, dass auf a eine */
                     /* closure oder eine 0-stellige funktion liegt */
                   L_eval:
                     ptdesc = (T_PTD)POP_A();
                     if (R_DESC(*ptdesc,type) == TY_CLOS) {
                       /* (gesaettigte?) closure auf a */
                       register PTR_HEAPELEM pth;
                       arity = R_CLOS(*ptdesc,args); /* anzahl argumente */
                       PUSH_I(arity);                /* anzahl argumente */
                       PUSH_I(0);                    /* bottomsymbol */
                       RES_HEAP;
                       pth = R_CLOS(*ptdesc,pta);
                       /* argumente und funktion auf stack i */
                       do {
                         T_INC_REFCNT((T_PTD)*pth);
                         PUSH_I(*pth++);
                       } while (--arity >= 0);
                       REL_HEAP;
                       DEC_REFCNT(ptdesc);           /* closure freigeben */
                       COMM0("eval0");
                       goto L_eval0;
                     }
                  /* if (R_DESC(*ptdesc,type) == TY_COMB) */
                     /* 0-stellige funktion auf a */
                     DEC_REFCNT(ptdesc);             /* wird nicht frei! */
                     COMB_CODE(ptdesc);              /* zeiger auf code holen */
                     COMM0("beta");
                     goto L_beta;
    case I_EVAL0:    /* === EVAL0 : auxiliary eval === */
                     FUNC0("eval0");
                   L_eval0:
                     /* argumente und funktion von i nach a schaufeln */
                     /* und gegebenenfalls auswerten                  */
                     while ((ptdesc = (T_PTD)POP_I()) != 0) {
                       PUSH_A(ptdesc);
                       if (T_POINTER((int)ptdesc)
                        && R_DESC(*ptdesc,class) == C_FUNC
                        && R_UNIV(*ptdesc,nargs) == 0) {
                         /* closure oder 0-stellige funktion */
                         PUSH_R(code);
                         code = rout_eval0;
                         goto L_eval;
                       }
                     }
                     /* das bottomsymbol (0) ist erreicht */
                     /* argumente und funktion sind auf a */
                     arity = POP_I();                /* anzahl argumente */
                     COMM0("apply");
                     goto L_apply;
/* dg 02.05.91 cons-listen */
    case I_CONS:     /* === CONS : cons list (constructor) === */
                     FUNC0("cons");
                     MAKEDESC(ptdesc,1,C_CONS,TY_CONS);
                     L_CONS(*ptdesc,tl) = (T_PTD)POP_A();
                     L_CONS(*ptdesc,hd) = (T_PTD)TOP_A();
                     WRITE_A(ptdesc);
                     goto loop;
    case I_FCONS:    /* === FCONS : cons lists (primfunc)  === */
                     FUNC0("fcons");
                     MAKEDESC(ptdesc,1,C_CONS,TY_CONS);
                     L_CONS(*ptdesc,hd) = (T_PTD)POP_A();
                     L_CONS(*ptdesc,tl) = (T_PTD)TOP_A();
                     WRITE_A(ptdesc);
                     goto loop;
    case I_FIRST:    /* === FIRST : list head === */
                     FUNC0("first");
                     ptdesc = (T_PTD)TOP_A();
                     if (T_POINTER(ptdesc) && R_DESC(*ptdesc,class) == C_CONS) {
                       arg = (int)R_CONS(*ptdesc,hd);
                       T_INC_REFCNT((T_PTD)arg);
                       WRITE_A(arg);
                       DEC_REFCNT(ptdesc);
                       goto loop;
                     }
                     if (T_POINTER(ptdesc) && R_DESC(*ptdesc,class) == C_LIST) {
                       PUSH_A(TAG_INT(1));
                       goto L_lsel;
                     }
                     /* failure */
                     PUSH_A(M_FIRST);
                     arity = arg = 1;
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_REST:     /* === REST : list tail === */
                     FUNC0("rest");
                     ptdesc = (T_PTD)TOP_A();
                     if (T_POINTER(ptdesc) && R_DESC(*ptdesc,class) == C_CONS) {
                       arg = (int)R_CONS(*ptdesc,tl);
                       T_INC_REFCNT((T_PTD)arg);
                       WRITE_A(arg);
                       DEC_REFCNT(ptdesc);
                       goto loop;
                     }
                     if (T_POINTER(ptdesc) && R_DESC(*ptdesc,class) == C_LIST) {
                       PUSH_A(TAG_INT(1));
                       func = LCUT;
                       goto L_delta2;
                     }
                     PUSH_A(M_FIRST);
                     arity = arg = 1;
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_UPDAT:    /* === UPDAT : update list element === */
                     FUNC0("updat");
                     ptdesc = (T_PTD)POP_I();        /* liste */
                     arg = POP_I();                  /* index */
                     arity = TOP_A();                /* element */
                     if (T_POINTER(arity))
                       INC_REFCNT((T_PTD)arity);
                     RES_HEAP;
                     help = R_LIST(*ptdesc,ptdv)[arg];
                     R_LIST(*ptdesc,ptdv)[arg] = arity;
                     REL_HEAP;
                     if (T_POINTER(help))
                       DEC_REFCNT((T_PTD)help);
                     DEC_REFCNT(ptdesc);             /* liste freigeben */
                     goto loop;
    case I_MKILIST:  /* === MKILIST n : make inverse n-ary list === */
                     FUNC1("mkilist");
                     arity = *code++;
                     /* arity: anzahl der listenelemente (>0!) */
                     /* store list, vgl. store_l in rstpro.c */
                     /* dg 30.01.92 verwendung des globalen _desc wg. scavenge */
                     GET_DESC(_desc);
                     LI_DESC_MASK(_desc,1,C_LIST,TY_UNDEF);
                     GET_HEAP(arity,A_LIST(*_desc,ptdv));
                     L_LIST(*_desc,dim) = arity;
                     RES_HEAP;
                     pth = R_LIST(*_desc,ptdv);
                     while (--arity > 0)
                       *pth++ = (T_HEAPELEM)POP_A();
                     *pth = (T_HEAPELEM)TOP_A();
                     REL_HEAP;
                     WRITE_A(_desc);
                     _desc = NULL;
                     goto loop;
/* #endif LAZY */
#ifdef TESTSPACE
    case I_SPACE:    /* === SPACE n : test stackspace === */   /* ifdef TESTSPACE */
                     FUNC1("space");
                     arg = *code++;
                     STACKSPACE(arg);
                     goto loop;
#endif /* TESTSPACE      auskommentiert RS 4.12.1992 */

/* Hier kommt der Verteilungs- und Messkram       RS ab 1.1.1993 */

#if D_DIST

    case I_PUSHH:    /* === PUSHH x : push x in H-Stack === */
                     FUNC1("pushh");
                     arity = *code++;
#if nCUBE
		     PUSH_E(arity);
#endif
                     goto loop;

    case I_DIST:     /* === DIST l1 l2 a b: starts distribution === */
#if DEBUG
                     if (OutInstr && debdisplay) { 
                       NEXTINSTR(); 
                       printf("%s(%1x, %1x, %1d, %1d)             \n","dist",*code,*(code+1),*(code+2),*(code+3)); 
                       } 
                     fprintf(TraceFp,"%s(%1x, %1x, %1d, %1d);\n","dist",*code,*(code+1),*(code+2),*(code+3));
#endif
#if nCUBE
#if (nCUBE && D_MESS && D_MSCHED)
    code = (*d_m_distribute)((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),DIST_N);
#else
		     code = distribute((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),DIST_N);
#endif
#endif
                     goto loop;

    case I_DISTEND:  /* === DIST_END : Ende der Berechnung eines vert. Ausdrucks === */
                     FUNC0("dist_end");
#if nCUBE
#if NCDEBUG
		     DBNCsetdistlevel(-1);
#endif /* NCDEBUG */
		     arg = POP_R();
		     if (arg>= 0)
#if (nCUBE && D_MESS && D_MCOMMU)
                       {(*d_m_msg_send)(arg,MT_RESULT);
#else
		       {msg_send(arg,MT_RESULT);
#endif

/* #if D_MESS      was sollte das denn hier ?????
      fflush(d_mess_dat);
#endif */

			DBUG_RETURN (IR_DIST_END);}
		     else
		       WRITE_E(TOP_E()-1);
#endif
                     goto loop;

    case I_WAIT:     /* === WAIT : wartet auf vert. Ergebnisse === */
                     FUNC1("wait");
#if nCUBE
		     WRITE_E(TOP_E()-*code++);
		     if (TOP_E()>0)

/* #if D_MESS      was sollte das denn hier ?
        { fflush(d_mess_dat);
#endif */
		       DBUG_RETURN (IR_WAIT);
/* #if D_MESS
	}
#endif */

#endif
                     goto loop;

    case I_POPH:     /* === POPH : entfernt das oberste H-Stack-Element === */
                     FUNC0("poph");
#if nCUBE
		     PPOP_E();
#endif
                     goto loop;

    case I_DISTB:     /* === DISTB l1 l2 a b: starts distribution  a bitmap === */
#if DEBUG
                     if (OutInstr && debdisplay) {
                       NEXTINSTR();
                       printf("%s(%1x, %1x, %1d, %1d)             \n","distb",*code,*(code+1),*(code+2),*(code+3));
                       }
                     fprintf(TraceFp,"%s(%1x, %1x, %1d, %1d);\n","distb",*code,*(code+1),*(code+2),*(code+3));
#endif
#if nCUBE
#if (nCUBE && D_MESS && D_MSCHED)
  code = (*d_m_distribute)((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),DIST_B);
#else
		     code = distribute((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),DIST_B);
#endif
#endif
                     goto loop;

#endif /* D_DIST */

  /* hier faengt der Mess-Kram an ! */

#if D_MESS

   case I_COUNT  :  /* === COUNT n : count command n === */

                    FUNC1("count");
                    arity = *code++;
                    /* arity: Nr. des auszufuehrenden und somit
                       zu zaehlenden ASM-Befehls */

 DBUG_PRINT ("inter", ("*** ich counte...: arity %d", arity)); 
#if nCUBE
                    MPRINT_COUNT (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, arity);
#else
post_mortem ("rinter: measure attempt not on nCUBE !!!!");
#endif
/* DBUG_PRINT("inter", ("*** ich habe gecountet...")); */

                    goto loop;

#if D_MHEAP

   case I_MHPALLON :  /* === MHPALLON : turn on heap alloc measure === */
                    
                      FUNC0("mhpallon");
       
#if nCUBE
                      DBUG_PRINT ("inter", ("*** ich schalte HEAP ALLOC an !!"));
                      d_m_newheap = m_newheap;
#endif
 
                      goto loop;

   case I_MHFREEON :  /* === MHFREEON : turn on heap free measure === */

                      FUNC0("mhfreeon");
      
#if nCUBE
                      DBUG_PRINT ("inter", ("*** ich schalte HEAP FREE an !!"));
                      d_m_freeheap = m_freeheap;
#endif

                      goto loop;

   case I_MHPCMPON :  /* === MHPCMPON : turn on heap compaction measure === */

                      FUNC0("mhpcmpon");

#if nCUBE
                      DBUG_PRINT ("inter", ("*** ich schalte HEAP COMPACT an !!"));
                      d_m_compheap = m_compheap;
#endif

                      goto loop;

   case I_MDESCALLON :  /* === MDESCALLON : turn on descriptor alloc measure === */

                        FUNC0("mdescallon");

#if nCUBE
                        DBUG_PRINT ("inter", ("*** ich schalte DESC ALLOC an !!"));
                        d_m_newdesc = m_newdesc;
#endif

                        goto loop;

   case I_MDESCFREON :  /* === MDESCFREON : turn on descriptor free measure === */

                        FUNC0("mdescfreon");
 
#if nCUBE
                        DBUG_PRINT ("inter", ("*** ich schalte DESC FREE an !!"));
                        d_m_freedesc = m_freedesc;
#endif

                        goto loop;

#endif /* D_MHEAP */

#if D_MSCHED

   case I_MSDISTON  :  /* === MSDISTON : turn on proc-distribute measure === */

                       FUNC0("msdiston");

#if nCUBE
                       DBUG_PRINT ("INTER", ("PROCESS DISTRIBUTION measure enabled !"));
                       d_m_distribute = m_distribute;
#endif

                       goto loop;

   case I_MSNODIST  : /* === MSNODIST : seperate exprs not to distribute === */

                      FUNC0("msnodist");

#if nCUBE
                      MPRINT_PROC_NODIST (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID);
#endif

                      goto loop;

   case I_MPROCREON :  /* === MPROCREON : turn on process creation measure === */

                       FUNC0("mprocreon");

#if nCUBE
                       DBUG_PRINT ("INTER", ("PROCESS CREATE measure enabled !"));
                       d_m_process_create = m_process_create;
#endif

                       goto loop;

#endif /* D_MSCHED */

   case I_MSDISTEND :  /* === MSDISTEND : process distend measure === */

                       FUNC0("msdistend");

#if nCUBE
                       if (TOP_R() >= 0) {
                         DBUG_PRINT ("INTER", ("DISTEND zurueck an Prozessor %d !", TOP_R()));
                         MPRINT_PROC_DISTEND(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, curr_pid->ppid, TOP_R());
                         }
#endif

                       goto loop;

#if D_MSCHED

   case I_MPRSLEEON :  /* === MPRSLEEON : turn on process suspend measure === */

                       FUNC0("mprsleeon");

#if nCUBE
                       DBUG_PRINT ("RINTER", ("PROCESS SLEEP measure enabled !"));
                       d_m_process_sleep = m_process_sleep;
#endif
   
                       goto loop;

   case I_MPRORUNON :  /* === MPRORUNON : turn on process run measure === */

                       FUNC0("mprorunon");

#if nCUBE
                       DBUG_PRINT ("RINTER", ("PROCESS RUN measure enabled !"));
                       d_m_process_run = m_process_run;
#endif

                       goto loop;

   case I_MPROWAKON :  /* === MPROWAKON : turn on process wake up measure === */

                       FUNC0("mprowakon");

#if nCUBE
                       DBUG_PRINT ("RINTER", ("PROCESS WAKE UP measure enabled !"));
                       d_m_process_wakeup = m_process_wakeup;
#endif

                       goto loop;

     case I_MPROTERON :  /* === MPROTERON : turn on process run measure === */

                       FUNC0("mproteron");

#if nCUBE
                       DBUG_PRINT ("RINTER", ("PROCESS TERMINATE measure enabled !"));
                       d_m_process_terminate = m_process_terminate;
#endif

                       goto loop;

#endif /* D_MSCHED */

#if D_MCOMMU

   case I_MCOMSEBON :  /* === MCOMSEBON : turn on communication send begin measure === */

                       FUNC0("mcomsebon");

#if nCUBE
                       DBUG_PRINT ("INTER", ("SEND BEGIN measure enabled !"));
                       d_m_msg_send = m_msg_send;
                       m_msg_send_begin = 1;
#endif

                       goto loop;

   case I_MCOMSEEON :  /* === MCOMSEEON : turn on communication send end measure === */

                       FUNC0("mcomseeon");

#if nCUBE
                       DBUG_PRINT ("INTER", ("SEND END measure enabled !"));
                       d_m_msg_send = m_msg_send;
                       m_msg_send_end = 1;
#endif

                       goto loop;

   case I_MCOMREBON :  /* === MCOMREBON : turn on communication receive begin measure === */

                       FUNC0("mcomrebon");

#if nCUBE
                       DBUG_PRINT ("INTER", ("RECEIVE BEGIN measure enabled !"));
                       d_m_msg_receive = m_msg_receive;
                       m_msg_receive_begin = 1;
#endif

                       goto loop;

   case I_MCOMREEON :  /* === MCOMREEON : turn on communication receive measure === */

                       FUNC0("mcomreeon");

#if nCUBE
                       DBUG_PRINT ("INTER", ("RECEIVE measure enabled !"));
                       d_m_msg_receive = m_msg_receive;
                       m_msg_receive_end = 1;
#endif

                       goto loop;

#endif /* D_MCOMMU */

#if D_MSTACK

   case I_MSTCKPUON :  /* === MSTCKPUON : turn on stack-push measure === */

                       FUNC0("mstckpuon");

#if nCUBE
                       DBUG_PRINT ("RINTER", ("STACK PUSH measure enabled !"));
                       d_m_stack_push = m_stack_push;
#endif

                       goto loop;

   case I_MSTCKPPON :  /* === MSTCKPPON : turn on stack-pop measure === */

                       FUNC0("mstckppon");

#if nCUBE
                       DBUG_PRINT ("RINTER", ("STACK POP measure enabled !"));
                       d_m_stack_pop = m_stack_pop;
#endif 

                       goto loop;

   case I_MSTKSEGAL :  /* === MSTKSEGAL : turn on stack-seg alloc measure === */

                       FUNC0("mstksegal");

#if nCUBE
                       DBUG_PRINT ("inter", ("*** ich schalte STACK SEG ALLOC an !!"));
                       d_m_allocate_d_stack_segment = m_allocate_d_stack_segment;
#endif

                       goto loop;

   case I_MSTKSEGFR :  /* === MSTKSEGFR : turn on stack-seg free measure === */

                       FUNC0("mstksegfr");

#if nCUBE
                       DBUG_PRINT ("inter", ("*** ich schalte STACK SEG FREE an !!"));
                       d_m_free_d_stack_segment = m_free_d_stack_segment;
#endif

                       goto loop;

#endif /* D_MSTACK */

#endif /* D_MESS */

   case I_CHKFRAME  :  /* === CHKFRAME : check for enough stackspace === */

                       FUNC1("chkframe");
                       arity = *code++;

#if D_CHECKFRAME

#endif
                       goto loop;

/* hier endet der Verteilungs- und Messkram       RS ab 1.1.1993 */

#if    UH_ZF_PM
    /* hier klinkt sich ulli ein ! */
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_APPEND:     /* ( W l -- ) ( E e1 ... en n -- e1 ... en en+1 n+1 ) */
/*dguh, 27.07.90*/                FUNC0("append");
/*dguh, 27.07.90*/                arity = TOP_E();
/*dguh, 27.07.90*/                WRITE_E(POP_A());
/*dguh, 27.07.90*/                PUSH_E(INC(arity));
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_BEGINZF:   /* ( E -- nil 0)  */
/*dguh, 27.07.90*/                FUNC0("zf{");
/*dguh, 27.07.90*/                PUSH_E(_nil);
/*dguh, 27.07.90*/                INC_REFCNT(_nil);
/*dguh, 27.07.90*/                PUSH_E(SET_ARITY(ZF_UH,0));
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_ENDZF:    /* ( E list e1 ... en n -- ) ( W -- list++[e1..en] ) */
/*dguh, 27.07.90*/                   /* Verwendung der Register:   */
/*dguh, 27.07.90*/                   /* arity:               Anzahl der (noch) erzeugten Elemente */
/*dguh, 27.07.90*/                   /* ptdesc:              Descriptor der Liste ausgelagerten Elemente */
/*dguh, 27.07.90*/                   /* pth:                 Zum Durchlaufen von Heapbereichen */
/*dguh, 27.07.90*/                   /* arg:                 Zeiger auf Resultat               */
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                FUNC0("}zf");
/*dguh, 27.07.90*/                arity = ARITY(POP_E());
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Liste erzeugen und Zeiger in ptdesc ablegen. */
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                if (arity) {  /* Ueberhaupt noch Elemente erzeugt. */
/*dguh, 27.07.90*/                  /* Elemente in den Heap auslagern */
/*dguh, 27.07.90*/                  /* Listendescriptor anfordern */
/*dguh, 27.07.90*/                  NEWDESC(ptdesc);
/*dguh, 27.07.90*/                  LI_DESC_MASK(ptdesc,1,C_LIST,TY_UNDEF);
/*dguh, 27.07.90*/                  L_LIST(*ptdesc,dim) = arity;
/*dguh, 27.07.90*/                  /* entsprechend grossen Heapblock anfordern */
/*dguh, 27.07.90*/                  GET_HEAP(arity,A_LIST(*ptdesc,ptdv));
/*dguh, 27.07.90*/                  RES_HEAP;
/*dguh, 27.07.90*/                  /* Heapblock mit Elementen fuellen */
/*dguh, 27.07.90*/                  pth = (PTR_HEAPELEM)R_LIST(*ptdesc,ptdv);
/*dguh, 27.07.90*/                  for ( ; --arity >= 0; ) {
/*dguh, 27.07.90*/                     pth[arity] = POP_E();
/*dguh, 27.07.90*/                  }
/*dguh, 27.07.90*/                  REL_HEAP;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                else { /* keine (weiteren) Elemente erzeugt */
/*dguh, 27.07.90*/                  INC_REFCNT(_nil);
/*dguh, 27.07.90*/                  ptdesc = _nil;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Anfangsstueck testen. */
/*dguh, 27.07.90*/                if (TOP_E() != (STACKELEM)_nil) {  /* Anfangsstueck nicht leer */
/*dguh, 27.07.90*/                  /* Ergebnisliste erzeugen und Zeiger in arg ablegen */
/*dguh, 27.07.90*/                  if (ptdesc==_nil) {  /* und keine weiteren Elemente erzeugt */
/*dguh, 27.07.90*/                    arg=POP_E();
/*dguh, 27.07.90*/                    /* ptdesc enthaelt _nil, das nicht mehr benoetigt wird. */
/*dguh, 27.07.90*/                    DEC_REFCNT(_nil);
/*dguh, 27.07.90*/                    PUSH_A((T_PTD)arg);
/*dguh, 27.07.90*/                    goto loop;
/*dguh, 27.07.90*/                  } else {  /* nicht leeres Anfangsstueck und  noch Elemente erzeugt */
/*dguh, 27.07.90*/                            /* AP Anfang Liste UNITE erzeugen */
/*dguh, 27.07.90*/                    /* Expression-Descriptor erzeugen */
/*dguh, 27.07.90*/                    MAKE_DESC(arg,1,C_EXPRESSION,TY_EXPR); /* for PCS */
/*dguh, 27.07.90*/                    /* Heapblock fuer Ausdruck anfordern */
/*dguh, 27.07.90*/                    GET_HEAP(5,A_EXPR(*(T_PTD)arg,pte)); /* Platz fuer @ x y z */
/*dguh, 27.07.90*/                    /* Heapblock fuellen */
/*dguh, 27.07.90*/                    RES_HEAP;
/*dguh, 27.07.90*/                    pth = (PTR_HEAPELEM)R_EXPR(*(T_PTD)arg,pte);
/*dguh, 27.07.90*/                    *pth++ = 4;
/*dguh, 27.07.90*/                    *pth++ = SET_ARITY(AP,3);
/*dguh, 27.07.90*/                    *pth++ = (int)ptdesc;        /* Liste */
/*dguh, 27.07.90*/                    *pth++ = POP_E();            /* Anfang */
/*dguh, 27.07.90*/                    *pth   = LUNI;
/*dguh, 27.07.90*/                    REL_HEAP;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                    /* Resultat ablegen */
/*dguh, 27.07.90*/                    PUSH_A((T_PTD)arg);
/*dguh, 27.07.90*/                    goto loop;
/*dguh, 27.07.90*/                  }
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                /* Anfangsstueck leer, noch erzeugte Elemente in ptdesc */
/*dguh, 27.07.90*/                POP_E();                  /* nil */
/*dguh, 27.07.90*/                DEC_REFCNT(_nil);
/*dguh, 27.07.90*/                PUSH_A(ptdesc);
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_DROPP:       /* ( W ptr -- ) */
/*dguh, 27.07.90*/                FUNC0("dropp");
/*dguh, 27.07.90*/                ptdesc = (T_PTD)POP_A();
/*dguh, 27.07.90*/                DEC_REFCNT(ptdesc);
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_JUMP:      /* Jump unconditionally */
/*dguh, 27.07.90*/                FUNC1("jump");
/*dguh, 27.07.90*/                code = (INSTR *)*code;
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_EXTRACT:    /* ( W listdesc n -- listdesc n-1 )  ( A -- el )  */
/*dguh, 27.07.90*/                FUNC0("extract");
/*dguh, 27.07.90*/                arity = TOP_A();
/*dguh, 27.07.90*/                ptdesc = (T_PTD)MID_A(1);
/*dguh, 27.07.90*/                pth = R_LIST(*ptdesc,ptdv);
/*dguh, 27.07.90*/                arg = pth[ R_LIST(*ptdesc,dim) - VAL_INT(arity) ];
/*dguh, 27.07.90*/                if (T_POINTER(arg))
/*dguh, 27.07.90*/                  INC_REFCNT((T_PTD)arg);
/*dguh, 27.07.90*/                PUSH_I(arg);
/*dguh, 27.07.90*/                WRITE_A(TAG_INT(VAL_INT(arity)-1));
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_DIM:        /* ( W listdesc -- dim ) */
/*dguh, 27.07.90*/                FUNC0("Dimension");
/*dguh, 27.07.90*/                ptdesc = (T_PTD)TOP_A();
/*dguh, 27.07.90*/                PUSH_A( TAG_INT(R_LIST(*ptdesc,dim)) );
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_TZERO:      /* ( W n -- n ) ( 0 -- )  */
/*dguh, 27.07.90*/                FUNC1("testzero");
/*dguh, 27.07.90*/                if (VAL_INT(TOP_A())) {
/*dguh, 27.07.90*/                  code++;
/*dguh, 27.07.90*/                  goto loop;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                POP_A();
/*dguh, 27.07.90*/                code = (INSTR *)*code;
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_TFALSE:    /* ( W f -- f )  ( ff -- ) */
/*dguh, 27.07.90*/                FUNC1("testfalse");
/*dguh, 27.07.90*/                arg = TOP_A();
/*dguh, 27.07.90*/                if (IS_FALSE(arg)) {
/*dguh, 27.07.90*/                   POP_A();
/*dguh, 27.07.90*/                   code = (INSTR *)*code;
/*dguh, 27.07.90*/                   goto loop;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                code++;
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_TTRUE:    /* ( W f -- f )  ( tf -- ) */
/*dguh, 27.07.90*/                FUNC1("testtrue");
/*dguh, 27.07.90*/                arg = TOP_A();
/*dguh, 27.07.90*/                if (IS_TRUE(arg)) {
/*dguh, 27.07.90*/                   POP_A();
/*dguh, 27.07.90*/                   code = (INSTR *)*code;
/*dguh, 27.07.90*/                   goto loop;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                code++;
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_TLIST:      /* ( W desc -- flag ) */
/*dguh, 27.07.90*/                FUNC1("testlist");
/*dguh, 27.07.90*/                ptdesc = (T_PTD)TOP_A();
/*dguh, 27.07.90*/                if (T_POINTER((int)ptdesc) && R_DESC(*ptdesc,class)==C_LIST) {
/*dguh, 27.07.90*/                   code++;
/*dguh, 27.07.90*/                   goto loop;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                code = (INSTR *)*code;
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_MAKEZF:     /* ( W ??? -- closure )  */
/*dguh, 27.07.90*/                FUNC1("makezflist");
/*dguh, 27.07.90*/                /* Auf Stack E liegen die bisher generierten Elemente der */
/*dguh, 27.07.90*/                /* Comprehension zusammen mit einer evtl. schon erzeugten */
/*dguh, 27.07.90*/                /* Closure (Anfangsstueck).  */
/*dguh, 27.07.90*/                /* Sie muessen nun zusammengefasst werden, und mit */
/*dguh, 27.07.90*/                /* Hilfe des Code-Descriptors zu einer weiteren Closure verarbeitet */
/*dguh, 27.07.90*/                /* werden. */
/*dguh, 27.07.90*/                arity=ARITY(TOP_E());     /* Anzahl der erzeugten Elemente */
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* ptdesc auf Liste der erzeugten Elemente setzen */
/*dguh, 27.07.90*/                if (arity) { /* Elemente erzeugt */
/*dguh, 27.07.90*/                  /* Platz fuer genuegend Elemente anfordern und Elemente wegschreiben */
/*dguh, 27.07.90*/                  /* vgl. END ZF */
/*dguh, 27.07.90*/                  POP_E();
/*dguh, 27.07.90*/                  NEWDESC(ptdesc);
/*dguh, 27.07.90*/                  LI_DESC_MASK(ptdesc,1,C_LIST,TY_UNDEF);
/*dguh, 27.07.90*/                  L_LIST(*ptdesc,dim) = arity;
/*dguh, 27.07.90*/                  GET_HEAP(arity,A_LIST(*ptdesc,ptdv));
/*dguh, 27.07.90*/                  RES_HEAP;
/*dguh, 27.07.90*/                  pth = (PTR_HEAPELEM)R_LIST(*ptdesc,ptdv);
/*dguh, 27.07.90*/                  for ( ; --arity >= 0; ) {
/*dguh, 27.07.90*/                     pth[arity] = POP_E();
/*dguh, 27.07.90*/                  }
/*dguh, 27.07.90*/                  REL_HEAP;
/*dguh, 27.07.90*/                } else { /* keine Elemente erzeugt. */
/*dguh, 27.07.90*/                  POP_E();
/*dguh, 27.07.90*/                  ptdesc= _nil;
/*dguh, 27.07.90*/                  INC_REFCNT(_nil);
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Diese Liste mit dem bisherigen Anfang vereinigen Ergebnis nach ptdesc*/
/*dguh, 27.07.90*/                if (TOP_E() != (STACKELEM)_nil) { /* bisheriger Anfang nicht leer */
/*dguh, 27.07.90*/                   /* erzeuge evtl. "AP Anfang Liste UNITE" */
/*dguh, 27.07.90*/                   if (ptdesc==_nil) { /* keine weiteren erzeugten Elemente */
/*dguh, 27.07.90*/                      /*  AP Anfang Liste UNITE  ist dann gleich   Anfang  */
/*dguh, 27.07.90*/                      ptdesc=(T_PTD)TOP_E();
/*dguh, 27.07.90*/                      /* in ptdesc stand _nil, das nicht mehr gebraucht wird. */
/*dguh, 27.07.90*/                      DEC_REFCNT(_nil);
/*dguh, 27.07.90*/                   } else {  /* weitere Elemente erzeugt */
/*dguh, 27.07.90*/                      /* AP Anfang Liste UNITE erzeugen */
/*dguh, 27.07.90*/                      MAKE_DESC(arg,1,C_EXPRESSION,TY_EXPR); /* For PCS */
/*dguh, 27.07.90*/                      GET_HEAP(5,A_EXPR(*(T_PTD)arg,pte)); /* Platz fuer @ x y z */
/*dguh, 27.07.90*/                      RES_HEAP;
/*dguh, 27.07.90*/                      /* Heapbereich fuellen */
/*dguh, 27.07.90*/                      pth = (PTR_HEAPELEM)R_EXPR(*(T_PTD)arg,pte);
/*dguh, 27.07.90*/                      *pth++ = 4;
/*dguh, 27.07.90*/                      *pth++ = SET_ARITY(AP,3);
/*dguh, 27.07.90*/                      *pth++ = (int)ptdesc;
/*dguh, 27.07.90*/                      *pth++ = TOP_E();
/*dguh, 27.07.90*/                      *pth   = LUNI;
/*dguh, 27.07.90*/                      REL_HEAP;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                      ptdesc = (T_PTD)arg;
/*dguh, 27.07.90*/                   }
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Closure erzeugen --> func */
/*dguh, 27.07.90*/                   arg = *code++;  /* Codedescriptor holen */
/*dguh, 27.07.90*/                   arity = R_ZFCODE(*(T_PTD)arg,nargs); /* Anzahl der bisher erz. ZF-Var */
/*dguh, 27.07.90*/                zb=R_ZFCODE(*(T_PTD)arg,zfbound);
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   /* ZF-Descriptor fuer Closure erzeugen. */
/*dguh, 27.07.90*/                   MAKE_DESC(help,1,C_EXPRESSION,TY_ZF); /* For PCS */
/*dguh, 27.07.90*/                   L_ZF(*(T_PTD)help,special)=ZF_START; /* markiere als ZF-Start */
/*dguh, 27.07.90*/                   L_ZF(*(T_PTD)help,nargs)=arity;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   /* Hier darf NICHT auf den gleichen HEAP-Block verwiesen werden! */
/*dguh, 27.07.90*/                   L_ZF(*(T_PTD)help,namelist)=R_ZFCODE(*(T_PTD)arg,varnames);
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   /* Heap-block fuer ZF-Applikation erzeugen */
/*dguh, 27.07.90*/                   GET_HEAP(4,A_ZF(*(T_PTD)help,pte));
/*dguh, 27.07.90*/                   pth=(PTR_HEAPELEM)R_ZF(*(T_PTD)help,pte);
/*dguh, 27.07.90*/                   *pth++ = 3;
/*dguh, 27.07.90*/                   *pth++ = SET_ARITY(AP,2);
/*dguh, 27.07.90*/                   *pth++ = POP_A();           /* Listen-Closure */
/*dguh, 27.07.90*/                   *pth++ = (int)R_ZFCODE(*(T_PTD)arg,ptd);
/*dguh, 27.07.90*/                            /* Zeiger auf folgenden Descr. */
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   /* SNAPplikation der aktuellen Belegung erzeugen */
/*dguh, 27.07.90*/                   MAKE_DESC(func,1,C_EXPRESSION,TY_EXPR); /* For PCS */
/*dguh, 27.07.90*/                   GET_HEAP(arity+3,A_EXPR(*(T_PTD)func,pte)); /* SNAP erzeugen */
/*dguh, 27.07.90*/                   pth=(PTR_HEAPELEM)R_EXPR(*(T_PTD)func,pte);
/*dguh, 27.07.90*/                   RES_HEAP;
/*dguh, 27.07.90*/                   printf("Arity hat in MAKEZF den Wert %d\nEs sind %d ZF Variablen gebunden.\n",arity,zb);
/*dguh, 27.07.90*/                   *pth++ = arity+2;
/*dguh, 27.07.90*/                   *pth++ = SET_ARITY(SNAP,arity+1);
/*dguh, 27.07.90*/                   *(pth+arity)=help;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   /* Werte der freie Variablen aufsammeln. */
/*dguh, 27.07.90*/                   help=R_ZFCODE(*(T_PTD)arg,zfbound); /* Anzahl der gebundenen ZF-Variablen */
/*dguh, 27.07.90*/                   while (help) {
/*dguh, 27.07.90*/                      help--;
/*dguh, 27.07.90*/                      if (T_POINTER(MID_I(help))) INC_REFCNT((T_PTD)MID_I(help)); /* uh 04Okt 91 */
/*dguh, 27.07.90*/                      *pth++=MID_I(help);   /* ZF-Variablen */
/*dguh, 27.07.90*/                   }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   help=R_ZFCODE(*(T_PTD)arg,zfbound);
/*dguh, 27.07.90*/                   while (arity>help) {
/*dguh, 27.07.90*/                      arity--;
/*dguh, 27.07.90*/                      if (T_POINTER(MID_I(arity))) INC_REFCNT((T_PTD)MID_I(arity)); /* uh 04Okt91 */
/*dguh, 27.07.90*/                      *pth++=MID_I(arity);  /* lambda Variablen */
/*dguh, 27.07.90*/                   }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   REL_HEAP;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Neues Anfangsstueck mit boolescher Closure vereinigen:  -> Stack E */
/*dguh, 27.07.90*/                if (ptdesc == _nil) {  /* Kein altes Anfangsstueck */
/*dguh, 27.07.90*/                   WRITE_E(func);       /* Closure ist neues Anfangsstueck */
/*dguh, 27.07.90*/                   /* ptdesc enthalet _nil, das nicht mehr gebraucht wird */
/*dguh, 27.07.90*/                   DEC_REFCNT(_nil);
/*dguh, 27.07.90*/                } else { /* Anfangsstueck vereinigen */
/*dguh, 27.07.90*/                   /* jetzt "@ unite Anfang Closure" erzeugen */
/*dguh, 27.07.90*/                   MAKE_DESC(arg,1,C_EXPRESSION,TY_EXPR); /* For PCS */
/*dguh, 27.07.90*/                   GET_HEAP(5,A_EXPR(*(T_PTD)arg,pte)); /* Platz fuer @ x y z */
/*dguh, 27.07.90*/                   RES_HEAP;
/*dguh, 27.07.90*/                   pth = (PTR_HEAPELEM)R_EXPR(*(T_PTD)arg,pte);
/*dguh, 27.07.90*/                   *pth++ = 4;
/*dguh, 27.07.90*/                   *pth++ = SET_ARITY(AP,3);
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   *pth++ = func;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   *pth++ = (int)ptdesc;        /* Anfang */
/*dguh, 27.07.90*/                   *pth   = LUNI;
/*dguh, 27.07.90*/                   REL_HEAP;
/*dguh, 27.07.90*/                   WRITE_E((T_PTD)arg);
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Zaehler fuer weitere Elemente drauf */
/*dguh, 27.07.90*/                PUSH_E(SET_ARITY(ZF_UH,0));
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_MAKEBOOL:   /*  ( w ??? -- closure ) */
/*dguh, 27.07.90*/                FUNC1("makebool");
/*dguh, 27.07.90*/                /* Auf Stack E liegen die bisher generierten Elemente der */
/*dguh, 27.07.90*/                /* Comprehension zusammen mit einer evtl. schon erzeugten */
/*dguh, 27.07.90*/                /* Closure (Anfangsstueck).  */
/*dguh, 27.07.90*/                /* Sie muessen nun zusammengefasst werden, und mit */
/*dguh, 27.07.90*/                /* Hilfe des Code-Descriptors zu einer weiteren Closure verarbeitet */
/*dguh, 27.07.90*/                /* werden. */
/*dguh, 27.07.90*/                arity=ARITY(TOP_E());     /* Anzahl der erzeugten Elemente */
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* ptdesc auf Liste der erzeugten Elemente setzen */
/*dguh, 27.07.90*/                if (arity) { /* Elemente erzeugt */
/*dguh, 27.07.90*/                  /* Platz fuer genuegend Elemente anfordern und Elemente wegschreiben */
/*dguh, 27.07.90*/                  /* vgl. END ZF */
/*dguh, 27.07.90*/                  POP_E();
/*dguh, 27.07.90*/                  NEWDESC(ptdesc);
/*dguh, 27.07.90*/                  LI_DESC_MASK(ptdesc,1,C_LIST,TY_UNDEF);
/*dguh, 27.07.90*/                  L_LIST(*ptdesc,dim) = arity;
/*dguh, 27.07.90*/                  GET_HEAP(arity,A_LIST(*ptdesc,ptdv));
/*dguh, 27.07.90*/                  RES_HEAP;
/*dguh, 27.07.90*/                  pth = (PTR_HEAPELEM)R_LIST(*ptdesc,ptdv);
/*dguh, 27.07.90*/                  for ( ; --arity >= 0; ) {
/*dguh, 27.07.90*/                     pth[arity] = POP_E();
/*dguh, 27.07.90*/                  }
/*dguh, 27.07.90*/                  REL_HEAP;
/*dguh, 27.07.90*/                } else { /* keine Elemente erzeugt. */
/*dguh, 27.07.90*/                  POP_E();
/*dguh, 27.07.90*/                  ptdesc= _nil;
/*dguh, 27.07.90*/                  INC_REFCNT(_nil);
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Diese Liste mit dem bisherigen Anfang vereinigen Ergebnis nach ptdesc*/
/*dguh, 27.07.90*/                if (TOP_E() != (STACKELEM)_nil) { /* bisheriger Anfang nicht leer */
/*dguh, 27.07.90*/                   /* erzeuge "@ unite Anfang Liste" */
/*dguh, 27.07.90*/                   if (ptdesc==_nil) { /* keine erzeugten Elemente */
/*dguh, 27.07.90*/                      ptdesc=(T_PTD)TOP_E();
/*dguh, 27.07.90*/                      DEC_REFCNT(_nil);
/*dguh, 27.07.90*/                   } else {
/*dguh, 27.07.90*/                      MAKE_DESC(arg,1,C_EXPRESSION,TY_EXPR); /* For PCS */
/*dguh, 27.07.90*/                      GET_HEAP(5,A_EXPR(*(T_PTD)arg,pte)); /* Platz fuer @ x y z */
/*dguh, 27.07.90*/                      RES_HEAP;
/*dguh, 27.07.90*/                      pth = (PTR_HEAPELEM)R_EXPR(*(T_PTD)arg,pte);
/*dguh, 27.07.90*/                      *pth++ = 4;
/*dguh, 27.07.90*/                      *pth++ = SET_ARITY(AP,3);
/*dguh, 27.07.90*/                      *pth++ = (int)ptdesc;
/*dguh, 27.07.90*/                      *pth++ = TOP_E();
/*dguh, 27.07.90*/                      *pth   = LUNI;
/*dguh, 27.07.90*/                      REL_HEAP;
/*dguh, 27.07.90*/                      ptdesc = (T_PTD)arg;
/*dguh, 27.07.90*/                   }
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Closure erzeugen --> func */
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   arg = *code++;  /* Codedescriptor holen */
/*dguh, 27.07.90*/                   arity = R_ZFCODE(*(T_PTD)arg,nargs); /* Anzahl der bisher erz. ZF-Var */
/*dguh, 27.07.90*/                 zb= R_ZFCODE(*(T_PTD)arg,zfbound);
/*dguh, 27.07.90*/                printf("In MAKEBOOL hat arity den Wert %d\n Es sind %d ZF-VAriablen gebunden\n",arity, zb);
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   MAKE_DESC(help,1,C_EXPRESSION,TY_ZF); /* For PCS */
/*dguh, 27.07.90*/                   L_ZF(*(T_PTD)help,special)=ZF_START; /* markiere als ZF-Start */
/*dguh, 27.07.90*/                   L_ZF(*(T_PTD)help,nargs)=arity;
/*dguh, 27.07.90*/                   L_ZF(*(T_PTD)help,namelist)=R_ZFCODE(*(T_PTD)arg,varnames);
/*dguh, 27.07.90*/                   GET_HEAP(4,A_ZF(*(T_PTD)help,pte));
/*dguh, 27.07.90*/                   /* L_ZF(*(T_PTD)help,pte)=R_ZFCODE(*(T_PTD)arg,pte); */
/*dguh, 27.07.90*/                   pth=(PTR_HEAPELEM)R_ZF(*(T_PTD)help,pte);
/*dguh, 27.07.90*/                   *pth++ = 3;
/*dguh, 27.07.90*/                   *pth++ = SET_ARITY(AP,2);
/*dguh, 27.07.90*/                   *pth++ = POP_A();
/*dguh, 27.07.90*/                   *pth++ = (int)R_ZFCODE(*(T_PTD)arg,ptd);
/*dguh, 27.07.90*/                              /* Zeiger auf folgenden Descr. */
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                     MAKE_DESC(func,1,C_EXPRESSION,TY_EXPR); /* For PCS */
/*dguh, 27.07.90*/                     GET_HEAP(arity+3,A_EXPR(*(T_PTD)func,pte)); /* SNAP erzeugen */
/*dguh, 27.07.90*/                     pth=(PTR_HEAPELEM)R_EXPR(*(T_PTD)func,pte);
/*dguh, 27.07.90*/                     RES_HEAP;
/*dguh, 27.07.90*/                     *pth++ = arity+2;
/*dguh, 27.07.90*/                     *pth++ = SET_ARITY(SNAP,arity+1);
/*dguh, 27.07.90*/                     *(pth+arity)=help;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                     help=R_ZFCODE(*(T_PTD)arg,zfbound); /* Anzahl der gebundenen ZF-Variablen */
/*dguh, 27.07.90*/                     while (help) {
/*dguh, 27.07.90*/                        help--;
/*dguh, 27.07.90*/                        if (T_POINTER(MID_I(help))) INC_REFCNT((T_PTD)MID_I(help)); /* uh 04Okt91 */
/*dguh, 27.07.90*/                        *pth++=MID_I(help);   /* ZF-Variablen */
/*dguh, 27.07.90*/                     }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                     help=R_ZFCODE(*(T_PTD)arg,zfbound);
/*dguh, 27.07.90*/                     while (arity>help) {
/*dguh, 27.07.90*/                        arity--;
/*dguh, 27.07.90*/                        if (T_POINTER(MID_I(arity))) INC_REFCNT((T_PTD)MID_I(arity)); /* uh 04Okt91 */
/*dguh, 27.07.90*/                        *pth++=MID_I(arity);  /* lambda Variablen */
/*dguh, 27.07.90*/                     }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                     REL_HEAP;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Neues Anfangsstueck mit boolescher Closure vereinigen:  -> Stack E */
/*dguh, 27.07.90*/                if (ptdesc == _nil) {  /* Kein altes Anfangsstueck */
/*dguh, 27.07.90*/                   WRITE_E(func);       /* Closure ist neues Anfangsstueck */
/*dguh, 27.07.90*/                } else { /* Anfangsstueck vereinigen */
/*dguh, 27.07.90*/                   /* jetzt "@ unite Anfang Closure" erzeugen */
/*dguh, 27.07.90*/                   MAKE_DESC(arg,1,C_EXPRESSION,TY_EXPR); /* FOR PCS */
/*dguh, 27.07.90*/                   GET_HEAP(5,A_EXPR(*(T_PTD)arg,pte)); /* Platz fuer @ x y z */
/*dguh, 27.07.90*/                   RES_HEAP;
/*dguh, 27.07.90*/                   pth = (PTR_HEAPELEM)R_EXPR(*(T_PTD)arg,pte);
/*dguh, 27.07.90*/                   *pth++ = 4;
/*dguh, 27.07.90*/                   *pth++ = SET_ARITY(AP,3);
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   *pth++ = func;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   *pth++ = (int)ptdesc;        /* Anfang */
/*dguh, 27.07.90*/                   *pth   = LUNI;
/*dguh, 27.07.90*/                   REL_HEAP;
/*dguh, 27.07.90*/                   WRITE_E((T_PTD)arg);
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                /* Zaehler fuer weitere Elemente drauf */
/*dguh, 27.07.90*/                PUSH_E(SET_ARITY(ZF_UH,0));
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
#endif /* UH_ZF_PM       auskommentiert RS 4.12.1992 */
    default:         sprintf(buf,"inter: invalid instruction (%d)",code[-1]);
                     POST_MORTEM(buf);
  } /* end switch */

#if    DEBUG
/* if (TraceFp) fclose(TraceFp); */
/* TraceFp = Null; */

#if !nCUBE

  strcpy(S_e.Name, "e");  stackname_dr(&S_e);
  strcpy(S_a.Name, "a");  stackname_dr(&S_a);
  strcpy(S_m1.Name,"m1"); stackname_dr(&S_m1);
  strcpy(S_m.Name, "m");  stackname_dr(&S_m);

#endif /* !nCUBE */
#endif /* DEBUG    auskommentiert RS 4.12.1992 */
  /* terminieren */
  END_MODUL("inter");
#if nCUBE
#if (nCUBE && D_MESS && D_MCOMMU)
  (*d_m_msg_send)(host,MT_END_RESULT);
#else
  msg_send(host,MT_END_RESULT);
#endif

#if D_MESS
  fflush(d_mess_dat);
#endif

  DBUG_RETURN (IR_EXT);
#else
  DBUG_VOID_RETURN;
#endif
/* code fuer I_APPLY aus dem case rausgenommen, weil zu lang 27.02.91 */

L_apply:

  /* stack a: | .. arg_arity .. arg_1 func */
  /* arity: anzahl der argumente auf'm stack (>0!!!) */
  ptdesc = (T_PTD)TOP_A();

  DBUG_PRINT ("INTER", ("descriptor: %d", (int) ptdesc));

  /* ptdesc: (pointer auf) die funktion oben auf a */
  if (T_POINTER((int)ptdesc) && R_DESC(*ptdesc,class) == C_FUNC) {
    /* funktionsdescriptor (COMB,CASE,CLOS,CONDI) auf a */

    DBUG_PRINT ("INTER", (" funktionsdescriptor (COMB,CASE,CLOS,CONDI) auf a"));

    arg = R_UNIV(*ptdesc,nargs);
    /* arg: anzahl der benoetigten argumente */
    if (arity == arg) {
      
      DBUG_PRINT ("INTER", (" passend viele argumente (>0!) "));

      /* passend viele argumente (>0!) */
      if (R_DESC(*ptdesc,type) == TY_COMB) {

        DBUG_PRINT ("INTER", ("(R_DESC(*ptdesc,type) == TY_COMB)"));

        PPOP_A();                 /* funktion weg von a */
        DEC_REFCNT(ptdesc);       /* wird nicht frei! */
        ptdesc = (T_PTD)R_COMB(*ptdesc,ptc); /* pc holen */
        COMM0("beta");
        goto L_beta;
      } /* end TY_COMB */
      else
      if (R_DESC(*ptdesc,type) == TY_CASE) {

        DBUG_PRINT ("INTER", ("(R_DESC(*ptdesc,type) == TY_CASE)"));

        PPOP_A();                 /* case weg von a */
        DEC_REFCNT(ptdesc);       /* wird nicht frei! */
        ptdesc = (T_PTD)R_CASE(*ptdesc,ptc); /* pc holen */
        COMM0("case");
        goto L_case;
      }
      else
      if (R_DESC(*ptdesc,type) == TY_CLOS) {

        DBUG_PRINT ("INTER", ("(R_DESC(*ptdesc,type) == TY_CLOS)"));

        PPOP_A();                 /* closure weg von a */
        /* ptdesc: zeiger auf die closure (weg von a) */
        /* arity: anzahl der argumente auf'm stack (>0!) */
        arg = R_CLOS(*ptdesc,args);
        /* arg: anzahl der argumente der closure (>0!) */
        if (R_CLOS(*ptdesc,ftype) == TY_COMB) {
          /* geschlossener combinator, einlagern */
          register PTR_HEAPELEM pth;

          DBUG_PRINT ("INTER", ("(R_CLOS(*ptdesc,ftype) == TY_COMB)"));

          COMM0("beta clos");
          RES_HEAP;
          pth = R_CLOS(*ptdesc,pta) + arg;
          help = (int)ptdesc;              /* closure */
          do {
            ptdesc = (T_PTD)*pth--;
            T_INC_REFCNT(ptdesc);
            PUSH_A(ptdesc);
          } while (--arg > 0);
          ptdesc = (T_PTD)*pth;              /* funktion */ 
          REL_HEAP;

          DBUG_PRINT ("INTER", ("refcnt: %d", (((T_PTD)help)->u.sc.ref_count)));

          DEC_REFCNT((T_PTD)help);         /* closure */ 

          DBUG_PRINT ("INTER", ("refcnt: %d", (((T_PTD)help)->u.sc.ref_count)));

          COMB_CODE(ptdesc);      /* zeiger auf code holen */
          COMM0("beta");
          goto L_beta;
        }
        else
        if (R_CLOS(*ptdesc,ftype) == TY_PRIM) {
          /* geschlossene primitive funktion, einlagern */
          register PTR_HEAPELEM pth;

          DBUG_PRINT ("INTER", ("geschlossene primitive funktion, einlagern "));

          COMM0("primf clos");
          RES_HEAP;
          /* arg: anzahl der argumente der closure (>0!) */
          pth = R_CLOS(*ptdesc,pta) + arg;
          help = (int)ptdesc;              /* closure */
          do {
            ptdesc = (T_PTD)*pth--;
            T_INC_REFCNT(ptdesc);
            PUSH_A(ptdesc);
          } while (--arg > 0);
          func = (int)*pth;                  /* primfunc */
          REL_HEAP;
          DEC_REFCNT((T_PTD)help);         /* closure */ 
          arg = FUNC_ARITY(func);
          COMM0("delta");
          if (func == LSELECT) goto L_lsel;
          if (arg == 2) goto L_delta2;
          if (arg == 1) goto L_delta1;
          if (arg == 3) goto L_delta3;
          if (arg == 4) goto L_delta4;
          POST_MORTEM("inter: pf_arity out of range (apply)");
        }
        else
        if (R_CLOS(*ptdesc,ftype) == TY_CONDI) {

          DBUG_PRINT ("INTER", ("geschlossenes conditional"));

          /* geschlossenes conditional */
          /* ptdesc: zeiger auf die closure */
          /* arity: anzahl der argumente auf'm stack */
          /* arg: anzahl der argumente der closure */
          /* vorsicht, hier wird arity=1 unterstellt !! */
          COMM0("cond clos");
          help = POP_A();                 /* praedikat */
          if (T_BOOLEAN(help)) {
            /* conditional ausfuehrbar, argumente einlagern */
            register PTR_HEAPELEM pth;

            DBUG_PRINT ("INTER", ("conditional ausfuehrbar, argumente einlagern"));

            arity = (int)ptdesc;           /* closure   */
            RES_HEAP;
            pth = R_CLOS(*ptdesc,pta) + arg;
            do {
              ptdesc = (T_PTD)*pth--;
              T_INC_REFCNT(ptdesc);
              PUSH_I(ptdesc);
            } while (--arg > 0);
            ptdesc = (T_PTD)*pth;       /* conditional       */
            REL_HEAP;
            PUSH_A(help);             /* praedikat */
            DEC_REFCNT((T_PTD)arity); /* closure freigeben */
            PUSH_R(code);             /* returnadresse     */
            SET_TAIL_FLAG();          /* SWITCH vermeiden  */
            code = R_CONDI(*ptdesc,ptc);
            goto loop;
          }
          else {

            DBUG_PRINT ("INTER", ("conditional nicht ausfuehrbar, schliessen"));

            /* conditional nicht ausfuehrbar, schliessen */
            /* ptdesc: zeiger auf die closure (weg von a) */
            /* arg: anzahl der argumente der closure */
            /* help: praedikat (liegt nicht mehr auf a) */
            if (R_DESC(*ptdesc,ref_count) == 1) {

              DBUG_PRINT ("INTER", ("closure mit refcnt 1 wird erweitert"));

              /* closure mit refcnt 1 wird erweitert */
              COMM0("expand clos");
              L_CLOS(*ptdesc,nargs) = 0;
              L_CLOS(*ptdesc,args)  = ++arg;
              RES_HEAP;
              L_CLOS(*ptdesc,pta)[arg] = help; /* pred */
              REL_HEAP;
              PPUSH_A(ptdesc);             /* closure */
              goto loop;
            }
            else {

              DBUG_PRINT ("INTER", ("neue, erweiterte closure bilden"));

              /* neue, erweiterte closure bilden */
              /* ptdesc: zeiger auf die closure (weg von a) */
              /* arg: anzahl der argumente der closure */
              /* help: praedikat (weg von a) */
              COMM0("new expanded clos");
              func = (int)ptdesc;
              MAKEDESC(ptdesc,1,C_FUNC,TY_CLOS);
              L_CLOS(*ptdesc,args) += 1;  /* zusaetzliches argument  */
              L_CLOS(*ptdesc,nargs) = 0;  /* braucht keine args mehr */
              L_CLOS(*ptdesc,ftype) = TY_CONDI;
#if (nCUBE && D_MESS && D_MHEAP)
              if ((*d_m_newheap)(arg+2,A_CLOS(*ptdesc,pta))) {
#else
              if (newheap(arg+2,A_CLOS(*ptdesc,pta))) {
#endif
                register PTR_HEAPELEM p1,p2;
                RES_HEAP;
                p1 = (PTR_HEAPELEM)R_CLOS(*(T_PTD)func,pta);
                p2 = (PTR_HEAPELEM)R_CLOS(*ptdesc,pta);
                for ( ; arg-- >= 0 ; ) {
                  T_INC_REFCNT((T_PTD)*p1);
                  *p2++ = *p1++;
                }
                *p2 = (T_HEAPELEM)help;  /* praedikat */
                REL_HEAP;
                DEC_REFCNT((T_PTD)func); /* alte clos */
                PPUSH_A(ptdesc);         /* closure   */
                goto loop;
              }
              /* descriptor freigeben? */
              POST_MORTEM("inter: heap overflow (mkclos)");
            }
          } /* end else */
        } /* end TY_CONDI */
        else
        if (R_CLOS(*ptdesc,ftype) == TY_CASE) {
          /* geschlossenes case */
          /* ptdesc: zeiger auf die closure */
          /* arity: anzahl der argumente auf'm stack */
          /* arg: anzahl der argumente der closure */
          /* vorsicht, hier wird arity=1 unterstellt !! */
          /* DAS IST NICHT GUT wg. NCASE !!! */
          register PTR_HEAPELEM pth;

          DBUG_PRINT ("INTER", ("geschlossenes case"));

          func = (int)ptdesc;       /* closure   */
          PUSH_R(code);             /* next instruction */
/* dg 09.09.92 ---- rumgemacht bis */
          /* argumente einlagern */
          RES_HEAP;
          pth = R_CLOS(*ptdesc,pta);  /* pth: --> f a_1 ... a_arg */
          /* anzahl der freien variablen ermitteln */
          ptdesc = *(T_PTD *)pth;     /* CASE */
          arity = R_CASE(*ptdesc,nargs) - R_SWITCH(*R_CASE(*ptdesc,ptd),case_type);
          /* anzahl der durch SRET freizugebenden argumente nach case auf e pushen */
          PUSH_E(arity);
          /* substitute fuer freie variablen auf i */
          for (pth += arg ; --arity >= 0 ; arg--) {
            ptdesc = (T_PTD)*pth--;
            T_INC_REFCNT(ptdesc);
            PUSH_I(ptdesc);
          }
          /* und nun argument(e) auf a */
          for ( ; --arg >= 0 ; ) {
            ptdesc = (T_PTD)*pth--;
            T_INC_REFCNT(ptdesc);
            PUSH_A(ptdesc);
          }
/* dg 09.09.92 --- her */
          ptdesc = (T_PTD)*pth;       /* case */
          REL_HEAP;
          DEC_REFCNT((T_PTD)func);    /* closure freigeben */
          PUSH_R(rout_sret);          /* special return */
          SET_TAIL_FLAG();            /* SWITCH vermeiden  */
          code = R_CASE(*ptdesc,ptc);
          goto loop;
        }
        else {
          sprintf(buf,"inter (apply): invalid CLOS ftype (%d)",R_CLOS(*ptdesc,ftype));
          POST_MORTEM(buf);
        }
      } /* end TY_CLOS */
      else
      if (R_DESC(*ptdesc,type) == TY_CONDI) {

        DBUG_PRINT ("INTER", ("(R_DESC(*ptdesc,type) == TY_CONDI)"));

        /* stack a | .. pred arg_arity-1 .. arg_1 ptcond */
        /* ptdesc: zeiger auf's conditional (auf a) */
        /* arity: anzahl der argumente auf'm stack (>0!) */
        /* arg: anzahl der benoetigten argumente (=arity) */
        COMM0("cond");
        help = MID_A(arg);           /* praedikat  */
        if (T_BOOLEAN(help)) {
          /* argumente auf i schaufeln */
          for (; --arg > 0 ;)
            PUSH_I(MID_A(arg));
          PPOP_A();                  /* conditional */
          DEC_REFCNT(ptdesc);        /* wird nicht frei! */
          /* argumente von a poppen */
          for (; --arity > 0 ;)
            PPOP_A();
          /* vorsicht hier: falls der then/else-code die */
          /* argumente nicht freigibt muss das anschliessend */
          /* geschehen !!! */ 
          if (R_CONDI(*ptdesc,args)) {
            /* conditional gibt argumente nicht frei! */
            PUSH_E(R_CONDI(*ptdesc,nargs-1)); /* nr. args */
            PUSH_R(code);            /* returnadresse    */
            code = rout_sret;        /* special return */
          }
          PUSH_R(code);              /* returnadresse    */
          SET_TAIL_FLAG();           /* SWITCH vermeiden */
          code = R_CONDI(*ptdesc,ptc);
          goto loop;
        }
        COMM0("mkcclos");
        goto L_mkcclos;
      } /* end TY_CONDI */
      else {
        sprintf(buf,"inter (apply): invalid FUNC type (%d)",R_DESC(*ptdesc,type)); 
        POST_MORTEM(buf);
      }
    } /* end if (arity == arg) */
    else
    if (arity < arg) {

      DBUG_PRINT ("INTER", ("zuwenige argumente"));

      /* zuwenige argumente */
      if (R_DESC(*ptdesc,type) == TY_CLOS) {

        DBUG_PRINT ("INTER", ("(R_DESC(*ptdesc,type) == TY_CLOS)"));

        if (R_DESC(*ptdesc,ref_count) == 1) {
          /* closure mit refcount 1 wird erweitert! */
          /* stack a: | .. arg_arity .. arg_1 clos */
          /* ptdesc: zeiger auf die closure auf'm stack */
          /* arity: anzahl der argumente auf'm stack (>0!) */
          /* arg: anzahl der benoetigten argumente (>1!) */
          register PTR_HEAPELEM pth;

          DBUG_PRINT ("INTER", ("closure mit refcount 1 wird erweitert"));

          COMM0("expand clos");
          RES_HEAP;
          pth = (PTR_HEAPELEM)R_CLOS(*ptdesc,pta);
          pth += R_CLOS(*ptdesc,args);
          for (arg = arity ; --arg >= 0 ; ) {
            PPOP_A();
            *++pth = TOP_A();
          }
          REL_HEAP;
          L_CLOS(*ptdesc,args) += arity;  /* mehr args */
          L_CLOS(*ptdesc,nargs) -= arity; /* braucht weniger */
          WRITE_A(ptdesc);                /* die closure */
          goto loop;
        }
        else {

          DBUG_PRINT ("INTER", ("neue, erweiterte closure bilden"));

          /* neue, erweiterte closure bilden */
          COMM0("new clos");
          help = R_CLOS(*ptdesc,args);
          /* stack a: | .. arg_arity .. arg_1 clos */
          /* ptdesc: zeiger auf die closure auf'm stack */
          /* arity: anzahl der argumente auf'm stack */
          /* arg: anzahl der benoetigten argumente */
          /* help: anzahl der closure argumente */
          func = (int)ptdesc;              /* closure */
          MAKEDESC(ptdesc,1,C_FUNC,TY_CLOS);
          L_CLOS(*ptdesc,args) = help+arity; /* mehr args */
          L_CLOS(*ptdesc,nargs) = arg-arity;/* weniger nargs */
          L_CLOS(*ptdesc,ftype) = R_CLOS(*(T_PTD)func,ftype);
          arg += help + 1;      /* laenge der arg.liste */
#if (nCUBE && D_MESS && D_MHEAP)
          if ((*d_m_newheap)(arg,A_CLOS(*ptdesc,pta))) {
#else
          if (newheap(arg,A_CLOS(*ptdesc,pta))) {
#endif
            register PTR_HEAPELEM p1,p2;
            RES_HEAP;
            p1 = (PTR_HEAPELEM)R_CLOS(*(T_PTD)func,pta);
            p2 = (PTR_HEAPELEM)R_CLOS(*ptdesc,pta);
            do {
              T_INC_REFCNT((T_PTD)*p1);
              *p2++ = *p1++;
            } while (--help >= 0);
            PPOP_A();                    /* alte closure */
            do {
              *p2++ = (T_HEAPELEM)POP_A();
            } while (--arity > 0);
            REL_HEAP;
            DEC_REFCNT((T_PTD)func);     /* alte closure */
            PPUSH_A(ptdesc);             /* neue closure */
            goto loop;
          }
          /* descriptor freigeben? */
          POST_MORTEM("inter: heap overflow (mkclos)");
        } /* end else */
      } /* end TY_CLOS */
      else
      if (R_DESC(*ptdesc,type) == TY_COMB) {

        DBUG_PRINT ("INTER", ("COMM0('mkbclos')"));

        COMM0("mkbclos");
        goto L_mkbclos;             /* make closure    */
      }
      else
      if (R_DESC(*ptdesc,type) == TY_CASE) {

        DBUG_PRINT ("INTER", ("goto L_mksclos"));

        COMM0("mksclos");
        goto L_mksclos;             /* make closure    */
      }
      else {
        sprintf(buf,"inter: invalid FUNC type (%d)",R_DESC(*ptdesc,type));
        POST_MORTEM(buf);
      }
    } /* end arity < arg */
    /* sonst zuviele argumente, behandlung unten! */
  }
  else
  if (T_FUNC((int)ptdesc)) {

    DBUG_PRINT ("INTER", ("T_FUNC((int)ptdesc)"));

    /* primitive funktion oben auf a */
    /* arity: anzahl der argumente auf'm stack */
    func = POP_A();               /* primfunc */ 
    arg = FUNC_ARITY(func);       /* args needed */
    if (arity == arg) {
      /* passend viele argumente */
      COMM0("delta");
      if (func == LSELECT) goto L_lsel;
      if (arg == 2) goto L_delta2;       /* 2-stellig */
      if (arg == 1) goto L_delta1;       /* 1-stellig */
      if (arg == 3) goto L_delta3;       /* 3-stellig */
      if (arg == 4) goto L_delta4;       /* 4-stellig */
      POST_MORTEM("inter: pf_arity out of range (apply)");
    }
    else
    if (arity < arg) {
      /* zuwenig argumente */
      PPUSH_A(func);
      COMM0("mkdclos");
      goto L_mkdclos;             /* delta-closure bilden  */
    } /* end arity < arg */
    /* sonst zuviele argumente, behandlung unten */
  } /* end T_FUNC */
  else {
    /* sonstige pointer oder stackelemente oben auf a */

    DBUG_PRINT ("INTER", ("goto L_mkap;  applikation auslagern"));

    COMM0("mkap");
    goto L_mkap;                  /* applikation auslagern */
  }
  /* sonst arity > arg d.h. zuviele argumente              */
  if (arg == 0) goto L_mkgclos;   /* universelle closure   */
  /* das apply muss in zwei apply's aufgespalten werden,   */
  /* dieses geschieht mithilfe der routine rout_sapply.    */
  /* arity: anzahl der argumente auf'm stack */
  /* arg: anzahl der benoetigten argumente */

  DBUG_PRINT ("INTER", ("universelle closure"));

  PUSH_R(code);                   /* returnadresse         */
  PUSH_R(arity-arg);              /* anz. ueberfl. arg.    */
  arity = arg;                    /* passend viele arg.    */
  code = rout_sapply;             /* special apply routine */
  goto L_apply;

L_interrupt:

  /* eine primitive funktion war nicht durchfuehrbar */
/*#if nCUBE
  post_mortem("interrupt in primitive function");
#endif*/
  END_MODUL("?");                           /* welches ist ja nicht bekannt */
  if (_heap_reserved) REL_HEAP;
  /* evtl. halb erstellte descriptoren freigeben */
  if (_desc != NULL) DEC_REFCNT(_desc);
  _interrupt = FALSE;
  _digit_recycling = TRUE;
  /* wiederherstellen der applikation (func enthaelt die funktion) */
/*  switch (FUNC_ARITY(func)) {
    case 4: PPUSH_A(help);
    case 3: PPUSH_A(arity);
    case 2: PPUSH_A(arg);
    case 1: PUSH_A(func);
            arity = FUNC_ARITY(func);
            break;
  } *//* end switch */
      PUSH_A(func);
      arity = FUNC_ARITY(func);
  /* CONTINUE! */

L_mkap:

  /* auslagern einer applikation vgl. store_e in rstpro.c */
  MAKEDESC(ptdesc,1,C_EXPRESSION,TY_EXPR);
  GET_HEAP(arity+3,A_EXPR(*ptdesc,pte));
  RES_HEAP;
  pth    = (PTR_HEAPELEM)R_EXPR(*ptdesc,pte);
  *pth++ = (T_HEAPELEM)(arity+2);             /* groesse     */
  *pth++ = (T_HEAPELEM)SET_ARITY(AP,(arity+1)); /* konstruktor */
  pth += arity;
  do {
    *pth-- = (T_HEAPELEM)POP_A();
  } while (--arity >= 0);
  REL_HEAP;
  PPUSH_A(ptdesc);
  goto loop;

} /* end inter */


/* --- load_pmlist : load pm argument list --- */

static void load_pmlist(st,p)
#if nCUBE
DStackDesc *st;
#else
StackDesc *st;
#endif

T_PTD     p;
{
  int i;
  for (i = 0 ; i < (int) R_LIST(*p,dim); i++ ) {
            /* int gecastet von RS 5.11.1992 */ 
#if nCUBE
    D_PUSHSTACK(*st,(R_LIST(*p,ptdv))[i]);
    T_INC_REFCNT((T_PTD)D_READSTACK(*st));
#else
    PUSHSTACK(*st,(R_LIST(*p,ptdv))[i]);
    T_INC_REFCNT((T_PTD)READSTACK(*st));
#endif

  }
  DEC_REFCNT(p);
}

/* --- load_pmilist : load inverse pm argument list --- */

static void load_pmilist(st,p)
#if nCUBE
DStackDesc *st;
#else
StackDesc *st;
#endif

T_PTD     p;
{
  int i;
  for (i = R_LIST(*p,dim); --i >= 0; ) {
#if nCUBE
    D_PUSHSTACK(*st,(R_LIST(*p,ptdv))[i]);
    T_INC_REFCNT((T_PTD)D_READSTACK(*st));
#else
    PUSHSTACK(*st,(R_LIST(*p,ptdv))[i]);
    T_INC_REFCNT((T_PTD)READSTACK(*st));
#endif
  }
  DEC_REFCNT(p);
}

/* end of file */
