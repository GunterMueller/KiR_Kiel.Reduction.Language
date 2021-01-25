/* $Log: rinter.c,v $
 * Revision 1.50  1995/05/22  09:30:32  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.49  1995/04/06  14:25:52  rs
 * UH-PM "in etc. bugfix
 *
 * Revision 1.48  1995/04/05  14:19:39  rs
 * thete's something wrong with BINDSUBLIST depending on the direction of the matching :-((
 * (errr, make that THERE's ! :-)
 *
 * Revision 1.47  1995/03/01  16:51:02  rs
 * various bugfixes and new features concerning UH's patternmatching
 *
 * Revision 1.46  1995/02/28  16:59:21  rs
 * DIGIT and SCALAR added (UH-PM)
 *
 * Revision 1.45  1995/02/28  08:51:29  rs
 * removed some warnings...
 *
 * Revision 1.44  1995/02/23  14:06:04  rs
 * minor bugfix in uh's pm
 *
 * Revision 1.43  1995/02/22  15:51:20  rs
 * SELECTION-descriptors used only in preprocessing
 *
 * Revision 1.42  1995/02/16  16:35:51  rs
 * UH PM GAMMA and GAMMACASE 0 red step
 *
 * Revision 1.41  1995/02/15  15:51:00  rs
 * now MATCHLIST REALLY matches lists ! :-)
 * no more UNDECIDED matching if the argument is a
 * (unnamed) function (as in old PM)
 *
 * Revision 1.40  1995/02/14  16:03:30  rs
 * minor changes
 *
 * Revision 1.39  1995/02/10  14:44:22  rs
 * stackelem == stackelem (UH's sourcecode)
 * UARGH !
 * ...corrected...
 *
 * Revision 1.38  1995/02/09  16:14:27  rs
 * UH PM bugfix -> BINDSUBL now generates an empty string
 *
 * Revision 1.37  1995/02/08  15:34:52  rs
 * bugfix in single when
 *
 * Revision 1.36  1995/02/07  15:49:23  rs
 * UH String PM implemented
 *
 * Revision 1.35  1995/01/31  14:49:12  rs
 * MKIFRAME -> MKAFRAME
 *
 * Revision 1.34  1995/01/19  10:17:20  rs
 * UH PM using 4 stacks
 *
 * Revision 1.33  1995/01/13  16:42:43  rs
 * uh pm bugfixes
 *
 * Revision 1.32  1995/01/11  14:55:35  rs
 * uh pm changes
 *
 * Revision 1.31  1995/01/10  16:28:13  rs
 * UH PM bugfixes
 *
 * Revision 1.30  1995/01/09  15:50:09  rs
 * more UH bugfixlike changes
 *
 * Revision 1.29  1995/01/06  16:32:27  rs
 * UH patternmatch bugfixes
 *
 * Revision 1.28  1995/01/03  15:17:24  rs
 * UH PM bugfixes
 *
 * Revision 1.27  1995/01/03  11:13:47  rs
 * more UH PM stuff
 *
 * Revision 1.26  1994/12/21  15:01:47  rs
 * more UH pm stuff
 *
 * Revision 1.25  1994/12/02  08:45:17  car
 * DYADIC instruction mkdclos
 *
 * Revision 1.24  1994/11/02  10:01:34  rs
 * UH_ZF_PM + Tilde
 *
 * Revision 1.23  1994/10/21  15:13:05  rs
 * *** empty log message ***
 *
 * Revision 1.22  1994/10/21  13:29:56  rs
 * core UH_ZF_PM source integrated
 *
 * Revision 1.21  1994/09/13  11:46:26  rs
 * first time-slicing changes
 *
 * Revision 1.20  1994/06/16  14:24:18  mah
 * minor bugfix
 *
 * Revision 1.19  1994/06/14  13:59:43  mah
 * free1sw_t added
 *
 * Revision 1.18  1994/05/24  13:21:30  ach
 * PUSHH/POPH changed to R-Stack for tilde version
 *
 * Revision 1.17  1994/05/16  11:41:50  ach
 * more bug fixes
 *
 * Revision 1.16  1994/05/06  12:23:24  ach
 * support for DIST, DISTB in tilde version added
 *
 * Revision 1.15  1994/05/05  10:01:46  rs
 * compile bugfix (D_MESS ## WITHTILDE)
 *
 * Revision 1.14  1994/05/04  07:02:52  ach
 * first beta release of distributed tilde-version
 *
 * Revision 1.13  1994/05/03  12:52:54  rs
 * first beta of merged picard with tilde
 *
 * Revision 1.12  1994/05/03  07:30:24  mah
 * *** empty log message ***
 *
 * Revision 1.11  1994/03/29  17:29:16  mah
 * popfree_t generation added to apply instruction
 *
 * Revision 1.10  1994/03/08  18:12:23  mah
 * bug fixes for tilde version
 *
 * Revision 1.9  1994/02/23  15:56:15  mah
 * pattern match added to tilde version
 *
 * Revision 1.8  1994/02/09  13:43:06  mah
 * bux fix in primfunc handling of l_apply
 *
 * Revision 1.7  1994/01/18  08:43:55  mah
 * new style push and move instructions
 *
 * Revision 1.6  1993/12/09  08:31:54  mah
 * *** empty log message ***
 *
 * Revision 1.5  1993/11/25  14:10:40  mah
 * ps_i, ps_a changed to ps_a, ps_w
 *
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
/* mah 26.10.93 Codierung des Tail_flags als Bit 0 in der Rueckkehradresse  */
/*                                                                          */
/****************************************************************************/

#define INTER "Interpreter fuer abstrakten Code"

#include "rstdinc.h"
#if D_SLAVE
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

#if D_SLAVE
#if TIME_SLICE
#include "spec_sched.h"
#endif
#include "rncsched.h"
#endif

#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

#if D_SLAVE
extern INSTR *distribute();
#endif

#if  SINTRAN3
extern double tused();
#define TIMER() tused()
#else
#if (!D_SLAVE)
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

#if D_SLAVE
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

#if UH_ZF_PM
extern PTR_DESCRIPTOR  gen_ind();
extern digit_eq();
#endif /* UH_ZF_PM */

#if    DEBUG
extern FILE    *TraceFp;           /* setup.c */
extern int     OutInstr;           /* setup.c ausgabe von instruktionen */
extern int     OutLen;             /* setup.c laenge des zyklischen ausgabebereiches */
extern int     debdisplay;         /* rruntime.c */
extern char    *stelname();        /* rdesc.c */
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

#if D_SLAVE
extern void d_pushstack();
extern STACKELEM d_popstack();
extern STACKELEM *d_ppopstack();
#else
extern STACKELEM *ppopstack();             /* rheap.c */
extern STACKELEM midstack();               /* rstack.c */
extern void updatestack();                 /* rstack.c */
#endif

#if WITHTILDE
extern void trav_a_hilf();
extern int mvheap();
#endif /* WITHTILDE */

/* --- interne variablen/routinen --- */

#if D_SLAVE                /* ach 04/01/93: Bei dynamischer Stackverwaltung werden andere Typen benutzt */

DStackDesc     *ps_w;              
DStackDesc     *ps_a;

#if WITHTILDE
DStackDesc     *ps_t;
DStackDesc     *ps_r;
#endif /* WITHTILDE */

#else

StackDesc      *ps_w;              /* zeiger auf aktuellen a-stack */
StackDesc      *ps_a;              /* zeiger auf aktuellen i-stack */
#if WITHTILDE
StackDesc      *ps_t;              /* zeiger auf aktuellen tilde-stack  mah 261093 */
StackDesc      *ps_r;              /* zeiger auf aktuellen return-stack mah 171193 */
#endif /* WITHTILDE */

#endif

static char    buf[80];            /* puffer fuer fehlermeldungen */

#if !WITHTILDE
static void    load_pmlist();
static void    load_pmilist();
#endif /* !WITHTILDE */

/* --- stackoperationen --- */

#ifdef  PUSH_A
#undef  PUSH_A
#endif

#if D_SLAVE             /* ach 04/01/92 */

#define POP_W()           D_POPSTACK(*ps_w)
#define PPOP_W()          D_PPOPSTACK(*ps_w)
#define TOP_W()           D_READSTACK(*ps_w)
#define PUSH_W(x)         D_PUSHSTACK(*ps_w,(x))
#define PPUSH_W(x)        D_PPUSHSTACK(*ps_w,(x))
#define WRITE_W(x)        D_WRITESTACK(*ps_w,(x))
#define MID_W(n)          D_MIDSTACK(*ps_w,n)
#define UPDATE_W(n,x)     D_UPDATESTACK(*ps_w,n,(x))

#define POP_A()           D_POPSTACK(*ps_a)
#define PPOP_A()          D_PPOPSTACK(*ps_a)
#define TOP_A()           D_READSTACK(*ps_a)
#define PUSH_A(x)         D_PUSHSTACK(*ps_a,(x))
#define PPUSH_A(x)        D_PPUSHSTACK(*ps_a,(x))
#define WRITE_A(x)        D_WRITESTACK(*ps_a,(x))
#define MID_A(x)          D_MIDSTACK(*ps_a,x)
#define UPDATE_A(n,x)     D_UPDATESTACK(*ps_a,n,(x))

#if WITHTILDE
#define POP_R()           D_POPSTACK(*ps_r)
#define PPOP_R()          D_PPOPSTACK(*ps_r)
#define TOP_R()           D_READSTACK(*ps_r)
#define PUSH_R(x)         D_PUSHSTACK(*ps_r,(x))
#define PPUSH_R(x)        D_PPUSHSTACK(*ps_r,(x))
#define WRITE_R(x)        D_WRITESTACK(*ps_r,(x))
#define MID_R(x)          D_MIDSTACK(*ps_r,(x))
#define UPDATE_R(n,x)     D_UPDATESTACK(*ps_r,n,(x))
#else
#define POP_R()           D_POPSTACK(D_S_R)
#define PPOP_R()          D_PPOPSTACK(D_S_R)
#define TOP_R()           D_READSTACK(D_S_R)
#define PUSH_R(x)         D_PUSHSTACK(D_S_R,(x))
#define PPUSH_R(x)        D_PPUSHSTACK(D_S_R,(x))
#define WRITE_R(x)        D_WRITESTACK(D_S_R,(x))
#define MID_R(x)          D_MIDSTACK(D_S_R,(x))
#define UPDATE_R(n,x)     D_UPDATESTACK(D_S_R,n,(x))
#endif /* WITHTILDE */

#define POP_E()           D_POPSTACK(D_S_H)
#define PPOP_E()          D_PPOPSTACK(D_S_H)
#define TOP_E()           D_READSTACK(D_S_H)
#define PUSH_E(x)         D_PUSHSTACK(D_S_H,(x))
#define PPUSH_E(x)        D_PPUSHSTACK(D_S_H,(x))
#define WRITE_E(x)        D_WRITESTACK(D_S_H,(x))
#define MID_E(x)          D_MIDSTACK(D_S_H,(x))
#define UPDATE_E(n,x)     D_UPDATESTACK(D_S_H,n,(x))

#if WITHTILDE
#define POP_T()           D_POPSTACK(*ps_t)
#define PPOP_T()          D_PPOPSTACK(*ps_t)
#define TOP_T()           D_READSTACK(*ps_t)
#define PUSH_T(x)         D_PUSHSTACK(*ps_t,(x))
#define PPUSH_T(x)        D_PPUSHSTACK(*ps_t,(x))
#define WRITE_T(x)        D_WRITESTACK(*ps_t,(x))
#define MID_T(x)          D_MIDSTACK(*ps_t,(x))
#define UPDATE_T(n,x)     D_UPDATESTACK(*ps_t,n,(x))
#endif /* WITHTILDE */

#else

#define POP_W()           POPSTACK(*ps_w)
#define PPOP_W()          PPOPSTACK(*ps_w)
#define TOP_W()           READSTACK(*ps_w)
#define PUSH_W(x)         PUSHSTACK(*ps_w,(x))
#define PPUSH_W(x)        PPUSHSTACK(*ps_w,(x))
#define WRITE_W(x)        WRITESTACK(*ps_w,(x))
#define MID_W(n)          MIDSTACK(*ps_w,n)
#define UPDATE_W(n,x)     UPDATESTACK(*ps_w,n,(x))

#define POP_A()           POPSTACK(*ps_a)
#define PPOP_A()          PPOPSTACK(*ps_a)
#define TOP_A()           READSTACK(*ps_a)
#define PUSH_A(x)         PUSHSTACK(*ps_a,(x))
#define PPUSH_A(x)        PPUSHSTACK(*ps_a,(x))
#define WRITE_A(x)        WRITESTACK(S_i,(x))
#define MID_A(x)          MIDSTACK(*ps_a,x)
#define UPDATE_A(n,x)     UPDATESTACK(*ps_a,n,(x))

#if WITHTILDE
#define POP_R()           POPSTACK(*ps_r)
#define PPOP_R()          PPOPSTACK(*ps_r)
#define TOP_R()           READSTACK(*ps_r)
#define PUSH_R(x)         PUSHSTACK(*ps_r,(x))
#define PPUSH_R(x)        PPUSHSTACK(*ps_r,(x))
#define WRITE_R(x)        WRITESTACK(*ps_r,(x))
#define MID_R(x)          MIDSTACK(*ps_r,(x))
#define UPDATE_R(n,x)     UPDATESTACK(*ps_r,n,(x))
#else
#define POP_R()           POPSTACK(S_m)
#define PPOP_R()          PPOPSTACK(S_m)
#define TOP_R()           READSTACK(S_m)
#define PUSH_R(x)         PUSHSTACK(S_m,(x))
#define PPUSH_R(x)        PPUSHSTACK(S_m,(x))
#define WRITE_R(x)        WRITESTACK(S_m,(x))
#define MID_R(x)          MIDSTACK(S_m,(x))
#define UPDATE_R(n,x)     UPDATESTACK(S_m,n,(x))
#endif /* WITHTILDE */

#define POP_E()           POPSTACK(S_e)
#define PPOP_E()          PPOPSTACK(S_e)
#define TOP_E()           READSTACK(S_e)
#define PUSH_E(x)         PUSHSTACK(S_e,(x))
#define PPUSH_E(x)        PPUSHSTACK(S_e,(x))
#define WRITE_E(x)        WRITESTACK(S_e,(x))
#define MID_E(x)          MIDSTACK(S_e,(x))
#define UPDATE_E(n,x)     UPDATESTACK(S_e,n,(x))

/* mah 261093 */
#if WITHTILDE
#define POP_T()           POPSTACK(*ps_t)
#define PPOP_T()          PPOPSTACK(*ps_t)
#define TOP_T()           READSTACK(*ps_t)
#define PUSH_T(x)         PUSHSTACK(*ps_t,(x))
#define PPUSH_T(x)        PPUSHSTACK(*ps_t,(x))
#define WRITE_T(x)        WRITESTACK(*ps_t,(x))
#define MID_T(x)          MIDSTACK(*ps_t,(x))
#define UPDATE_T(n,x)     UPDATESTACK(*ps_t,n,(x))
#endif /* WITHTILDE */

#endif /* D_SLAVE */
/* --- debug: ausgabe der auszufuehrenden instruktion --- */

#if     DEBUG

static int  instrcnt = 0; /* zaehler fuer ausgefuerte instruktionen */
static int  relpos   = 0; /* zyklischer positionszahler fuer bildschirmausgabe */
static int  baserow  = 2; /* erste ausgabezeile */
static int  basecol  = 0; /* ausgabespalte */

#define NEXTINSTR()       cursorxy(baserow+(relpos=(relpos+1)%OutLen),basecol); \
                          printf("%d ",++instrcnt)
#define POST_MORTEM(mes)  {if (TraceFp) { fclose(TraceFp); TraceFp=0; } post_mortem(mes); }

#if D_SLAVE

#define SWITCHSTACKS()    {register DStackDesc *x = ps_w; ps_w = ps_a; ps_a = x;} \
                           switchstacks(ps_w,ps_a); \
                           fprintf(TraceFp,"_switchstacks();\n")
#if WITHTILDE
#define SWITCHSTACKS_T()  {register DStackDesc *x = ps_r; ps_r = ps_t; ps_t = x;} \
                           switchstacks(ps_r,ps_t); \
                           fprintf(TraceFp,"_switchstacks_t();\n")
#endif /* WITHTILDE */

#else

#define SWITCHSTACKS()    {register StackDesc *x = ps_w; ps_w = ps_a; ps_a = x;} \
                           switchstacks(ps_w,ps_a); \
                           fprintf(TraceFp,"_switchstacks();\n")

#if WITHTILDE
#define SWITCHSTACKS_T()  {register StackDesc *x = ps_r; ps_r = ps_t; ps_t = x;} \
                           switchstacks(ps_r,ps_t); \
                           fprintf(TraceFp,"_switchstacks_t();\n")
#endif /* WITHTILDE */

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

#if UH_ZF_PM
#define WAIT PUSHSTACK(S_i,DOLLAR); POPSTACK(S_i)
#endif /* UH_ZF_PM */


#if WITHTILDE
                          /* Die Closures sind im Tilde-Konzept dreistellig */
#define FUNC3(x)          if (OutInstr && debdisplay) { \
                            NEXTINSTR(); \
                            printf("%s(%1d, %1d, %1d)        \n",x,*code,*(code+1),*(code+2)); \
                          } \
                          fprintf(TraceFp,"%s(%1d, %1d, %1d);\n",x,*code,*(code+1),*(code+2))
#endif /* WITHTILDE */

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

#if D_SLAVE
#define SWITCHSTACKS()    {register DStackDesc *x = ps_w; ps_w = ps_a; ps_a = x;}
#if WITHTILDE
#define SWITCHSTACKS_T()  {register DStackDesc *x = ps_r; ps_r = ps_t; ps_t = x;}
#endif /* WITHTILDE */
#else
#define SWITCHSTACKS()    {register StackDesc *x = ps_w; ps_w = ps_a; ps_a = x;}

#if WITHTILDE
#define SWITCHSTACKS_T()  {register StackDesc *x = ps_r; ps_r = ps_t; ps_t = x;}
#endif /* WITHTILDE */

#endif

#define COMM0(x)
#define FUNC0(x)
#define FUNC1(x)
#define FUNC2(x)

#if UH_ZF_PM
#define FUNC3(x)
#define WAIT
#endif /* UH_ZF_PM */ /* I know it's a collision with withtide :-) */

#if WITHTILDE
#define FUNC3(x)
#endif /* WITHTILDE */
#define FUNCX(x)
#define FUNCN(x)
#define FUNCL(x)

#endif /* DEBUG */

/* --- makros --- */

#define TAIL_FLAG            1
#define POP_RET()            (INSTR *)(POP_R() & ~TAIL_FLAG)
#define TEST_TAIL_FLAG()     ((TOP_R() & TAIL_FLAG) == TAIL_FLAG)
#define SET_TAIL_FLAG()      WRITE_R(TOP_R() | TAIL_FLAG)
#define DEL_TAIL_FLAG()      WRITE_R(TOP_R() & ~TAIL_FLAG)
#define INVERT_TAIL_FLAG()   WRITE_R(TOP_R() ^ TAIL_FLAG)

/* In der 4-stack-version, in der der returnstack mitbenutzt wird,    */
/* um den neuen Tilde-frame aufzubauen, koennen tail-end-rekursionen  */
/* nicht optimiert werden. Deswegen wird kein tail-flag benoetigt     */
/* In einer 5-stack-version, die den returnstack nicht mitbenutzt,    */
/* waeren diese optmierungen moeglich und das tail_t-flag noetig,     */
/* aber die Goetter sind dagegen.                                     */

#if WITHTILDE
#define TAIL_FLAG_T          2
#define TEST_TAIL_FLAG_T()   ((TOP_R() & TAIL_FLAG_T) == TAIL_FLAG_T)
#define SET_TAIL_FLAG_T()    WRITE_R(TOP_R() | TAIL_FLAG_T)
#define DEL_TAIL_FLAG_T()    WRITE_R(TOP_R() & ~TAIL_FLAG_T)
#define INVERT_TAIL_FLAG_T() WRITE_R(TOP_R() ^ TAIL_FLAG_T)

#if 0
/* Ein gammabeta-Funktionsaufruf setzt das GAMMABETA-Flag und rtf  */
/* kann dann feststellen, ob zusaetzlich t und r vertauscht werden */
/* muessen. mah 301193                                             */
/* nicht mehr noetig mah 120194                                    */

#define GAMMABETA_FLAG       TAIL_FLAG_T
#define TEST_GAMMABETA()     TEST_TAIL_FLAG_T()
#define SET_GAMMABETA()      SET_TAIL_FLAG_T()
#define DEL_GAMMABETA()      DEL_TAIL_FLAG_T()
#endif /* 0 */
#endif /* WITHTILDE */


#define STACKSPACE(n)     IS_SPACE((*ps_w),(n))
/* beta,tail, cond und jtrue/jfalse: parameter sind absolute sprungziele */
#define GRAB_DESC(p)      p = ((T_PTD *)p)[-1]
#define COMB_CODE(p)      p = (T_PTD)R_COMB(*p,ptc)

/* GET_DESC(p), GET_HEAP(n,p), MAKEDESC(p,ref,cl,ty) defined in rheapty.h */

/* --- spezielle routinen --- */

static INSTR rout_rtf[]    = { I_RTF };                     /* return */
static INSTR rout_sret[]   = { I_SRET };                    /* free and return */
#if WITHTILDE
static INSTR rout_popft[]  = { I_POPFREE_T, I_RTS };
static INSTR rout_sclos[]  = { I_MKSCLOS, 0, 0, 42, I_RTF };
#endif /* WITHTILDE */
static INSTR rout_sapply[] = { I_SAPPLY, I_RTS };           /* special apply */
static INSTR rout_eval0[]  = { I_EVAL0, I_RTS };            /* evaluate */
static INSTR rout_updat[]  = { I_UPDAT, I_RTS };            /* eval, update */
       INSTR Rout_Eval[]   = { I_EVAL, I_EXT };             /* eval and exit */
       INSTR Rout_Mkcnd[]  = { I_MKTHEN, I_MKELSE, I_MKCOND, I_EXT }; /* make cond */

/* -------------------------------------------------------------------- */
/* ---                         Interpreter                          --- */
/* -------------------------------------------------------------------- */

#if D_SLAVE
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
#if WITHTILDE
  register VOLATILE int          nfv=0; /* Anzahl der relativ freien Variablen */
  register VOLATILE int          gamma_closure = 0;
#endif /* WITHTILDE */
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

#if (!D_SLAVE)

  ps_w = &S_a;          /* initial working  stack is mapped on S_a  */
  ps_a = &S_m1;         /* initial argument stack is mapped on S_m1 */

#if WITHTILDE
  ps_t = &S_tilde;      /* initial tilde stack is mapped on S_tilde     */
  ps_r = &S_m;          /* initial return/tilde2stack is mapped on S_m */
#endif

#endif /* !D_SLAVE */

#if (DEBUG && !D_SLAVE)

  /* rename stacks for debugging purposes (undone at return) */
  strcpy(S_e.Name,     "H"); stackname_dr(&S_e);
  strcpy(S_a.Name,     "W"); stackname_dr(&S_a);
  strcpy(S_m1.Name,    "A"); stackname_dr(&S_m1);
  strcpy(S_m.Name,     "R"); stackname_dr(&S_m);
#if WITHTILDE
  strcpy(S_tilde.Name, "T"); stackname_dr(&S_tilde); /* mah 251093 */
#endif

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
#if D_SLAVE
fprintf(stderr,"\aNode %d: interpreter, code: %x\n",proc_id,code);
#endif
*/

loop:

#if NCDEBUG
#  if D_SLAVE
    DBNCclient();
#  endif
#endif

#if D_SLAVE
/*
DBUG_PRINT ("DUMP", (""));
DBUG_PRINT ("DUMP", ("A-Stack:"));
DBUG_PRINT ("DUMP", ("Topaddresse: %x; Schotter: %x (class %d, type %d)",
                     ps_w->TopofStack,
                     TOP_W(),
                     ((IS_POINTER(TOP_W()))?R_DESC(*(T_PTD)TOP_W(),class):-1), 
                     ((IS_POINTER(TOP_W()))?R_DESC(*(T_PTD)TOP_W(),type):-1)));
if (d_stacksize(ps_w) > 1)
DBUG_PRINT ("DUMP", ("Unner dem liegt ey: %x (class %d, type %d)", 
                     MID_W(1),
                     ((IS_POINTER(MID_W(1)))?R_DESC(*(T_PTD)MID_W(1),class):-1),
                     ((IS_POINTER(MID_W(1)))?R_DESC(*(T_PTD)MID_W(1),type):-1)));
DBUG_PRINT ("DUMP", ("I_STACK:"));
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
DBUG_PRINT ("DUMP", (""));
*/
#endif

#if D_SLAVE
DBUG_PRINT("INTER",("proc %x: next instruction: %s (%x),redcount: %d",curr_pid,instr_tab[*code].name,code,_redcnt));
#endif

#if (D_SLAVE && TIME_SLICE)    /* check for timeout */
  if (SCHED_TIME_OUT) 
    DBUG_RETURN (IR_TIME_OUT);
#endif /* D_SLAVE && TIME_SLICE */

#if D_SLAVE
  if (sig_msg_flag)
    msg_check();
#endif

/*  hpchain_inter(); */

  switch (*code++) {
    case I_EXT:      /* === exit === */
                     FUNC0("ext");
                     break;

    case I_PUSH_W:   /* === PUSH_W p : push a pointer onto the working stack === */
		     /* mah 261193:war in 3-Stack-Version: PUSH */
                     FUNCX("push_w");
                     INC_REFCNT((T_PTD)(*code));
                     PUSH_W(*code++);
                     goto loop;
    case I_PUSHC_W:  /* === PUSHC_W c : push a constant onto the working stack === */
		     /* mah 261193:war in 3-Stack-Version: PUSHC */
                     FUNCN("pushc_w");
                     PUSH_W(*code++);
                     goto loop;
    case I_PUSH_AW:  /* === PUSH_AW n : push I-stackentry n onto the working stack === */
		     /* mah 261193:war in 3-Stack-Version: PUSHARG */
                     FUNC1("push_aw");
                     ptdesc = (T_PTD)MID_A(*code);
		     code++;                                         /* ach 08/01/93 */
                     T_INC_REFCNT(ptdesc);
                     PUSH_W(ptdesc);
                     goto loop;
    case I_PUSH_AW0: /* === PUSH_AW0 : push stackentry 0 onto the stack === */
		     /* mah 261193:war in 3-Stack-Version: PUSHARG */
                     FUNC0("push_aw0");
                     ptdesc = (T_PTD)TOP_A();
                     T_INC_REFCNT(ptdesc);
                     PUSH_W(ptdesc);
                     goto loop;
    case I_MOVE_AW:  /* === MOVE_AW : move one item from a to w === */
		     /* mah 261193:war in 3-Stack-Version: MOVE */
                     FUNC0("move_aw");
                     PUSH_W(POP_A());
                     goto loop;
    case I_DELTA1:   /* === DELTA1 pf : perform unary delta-reduction === */
                     FUNCN("delta1");
                     func = *code++;
                   L_delta1:
                     /* func: primitive funktion (stackelement) */
                     if (_redcnt >= delta_count) {
                       if ((*(red_func_tab[FUNC_INDEX(func)]))(TOP_W())) {
                         /* _desc: resultat der funktionsanwendung */
                         WRITE_W(_desc);
                         _desc = NULL;
                         _redcnt -= delta_count;
                         goto loop;
                       }
                     }
                     PUSH_W(func);
                     arity = arg = 1;
#if WITHTILDE
		     nfv = 0;
#endif
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_DELTA2:   /* === DELTA2 pf : perform binary delta-reduction === */
                     FUNCN("delta2");
		     DBUG_PRINT("DELTA2",("func: %d; arg 1: %x (class %d, type %d): value %d; arg2: %x (class %d, type %d):value %d",
					  *code,
					  TOP_W(),
					  ((IS_POINTER(TOP_W()))?R_DESC(*(T_PTD)TOP_W(),class):-1),
					  ((IS_POINTER(TOP_W()))?R_DESC(*(T_PTD)TOP_W(),type):-1),
					  ((IS_POINTER(TOP_W()) && (R_DESC(*(T_PTD)TOP_W(),class) == C_DIGIT))?*R_DIGIT(*(T_PTD)TOP_W(),ptdv):TOP_W()),
					  MID_W(1),
					  ((IS_POINTER(MID_W(1)))?R_DESC(*(T_PTD)MID_W(1),class):-1),
					  ((IS_POINTER(MID_W(1)))?R_DESC(*(T_PTD)MID_W(1),type):-1),
					  ((IS_POINTER(MID_W(1)) && (R_DESC(*(T_PTD)MID_W(1),class) == C_DIGIT))?*R_DIGIT(*(T_PTD)MID_W(1),ptdv):MID_W(1))));
                     func = *code++;
                   L_delta2:
                     /* func: primitive funktion (stackelement) */
                     if (_redcnt >= delta_count) {
                       /* dg 30.01.92 argumente bleiben auf'm stack wg. scavenge */
                       if ((*(red_func_tab[FUNC_INDEX(func)]))(TOP_W(),MID_W(1))) {
                         /* _desc: resultat der funktionsanwendung */
                         PPOP_W();
                         WRITE_W(_desc);
                         _desc = NULL;
                         _redcnt -= delta_count;
                         goto loop;
                       }
                     }
                     PUSH_W(func);
                     arity = arg = 2;
#if WITHTILDE
		     nfv = 0;
#endif
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_DELTA3:   /* === DELTA3 pf : perform ternary delta-reduction === */
                     FUNCN("delta3");
                     func = *code++;
                   L_delta3:
                     /* func: primitive funktion (stackelement) */
                     if (_redcnt >= delta_count) {
                       /* dg 30.01.92 argumente bleiben auf'm stack wg. scavenge */
                       if ((*(red_func_tab[FUNC_INDEX(func)]))(TOP_W(),MID_W(1),MID_W(2))) {
                         /* _desc: resultat der funktionsanwendung */
                         PPOP_W();
                         PPOP_W();
                         WRITE_W(_desc);
                         _desc = NULL;
                         _redcnt -= delta_count;
                         goto loop;
                       }
                     }
                     PUSH_W(func);
                     arity = arg = 3;
#if WITHTILDE
		     nfv = 0;
#endif
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_DELTA4:   /* === DELTA4 pf : perform quaternary delta-reduction === */
                     FUNCN("delta4");
                     func = *code++;
                   L_delta4:
                     /* func: primitive funktion (stackelement) */
                     if (_redcnt >= delta_count) {
                       /* dg 30.01.92 argumente bleiben auf'm stack wg. scavenge */
                       if ((*(red_func_tab[FUNC_INDEX(func)]))(TOP_W(),MID_W(1),MID_W(2),MID_W(3))) {
                         /* _desc: resultat der funktionsanwendung */
                         PPOP_W();
                         PPOP_W();
                         PPOP_W();
                         WRITE_W(_desc);
                         _desc = NULL;
                         _redcnt -= delta_count;
                         goto loop;
                       }
                     }
                     PUSH_W(func);
                     arity = arg = 4;
#if WITHTILDE
		     nfv = 0;
#endif
                     COMM0("mkdclos");
                     goto L_mkdclos;

/* --- shortcut : directly supported primitive functions --- */
#if WITHTILDE
#define EXEC_DFUNC(pf_test,pf_makro,pf_func,pf_stel) \
                     arg = TOP_W(); \
                     if (pf_test(arg,MID_W(1))) { \
                       PPOP_W(); \
                       WRITE_W(pf_makro(arg,TOP_W())); \
                       goto loop; \
                     } \
                     if (pf_func(arg,MID_W(1))) { \
                       PPOP_W(); \
                       WRITE_W(_desc); \
                       _desc = NULL; \
                       goto loop; \
                     } \
                     /* failure */ \
                     PUSH_W(pf_stel); \
                     arity = arg = 2; \
                     nfv = 0; \
                     COMM0("mkdclos"); \
                     goto L_mkdclos

#define EXEC_MFUNC(pf_test,pf_makro,pf_func,pf_stel) \
                     if (pf_test(TOP_W())) { \
                       WRITE_W(pf_makro(TOP_W())); \
                       goto loop; \
                     } \
                     if (pf_func(TOP_W())) { \
                       WRITE_W(_desc); \
                       _desc = NULL; \
                       goto loop; \
                     } \
                     /* failure */ \
                     PUSH_W(pf_stel); \
                     arity = arg = 1; \
                     nfv = 0; \
                     COMM0("mkdclos"); \
                     goto L_mkdclos
#else
#define EXEC_DFUNC(pf_test,pf_makro,pf_func,pf_stel) \
                     arg = TOP_W(); \
                     if (pf_test(arg,MID_W(1))) { \
                       PPOP_W(); \
                       WRITE_W(pf_makro(arg,TOP_W())); \
                       goto loop; \
                     } \
                     if (pf_func(arg,MID_W(1))) { \
                       PPOP_W(); \
                       WRITE_W(_desc); \
                       _desc = NULL; \
                       goto loop; \
                     } \
                     /* failure */ \
                     PUSH_W(pf_stel); \
                     arity = arg = 2; \
                     COMM0("mkdclos"); \
                     goto L_mkdclos

#define EXEC_MFUNC(pf_test,pf_makro,pf_func,pf_stel) \
                     if (pf_test(TOP_W())) { \
                       WRITE_W(pf_makro(TOP_W())); \
                       goto loop; \
                     } \
                     if (pf_func(TOP_W())) { \
                       WRITE_W(_desc); \
                       _desc = NULL; \
                       goto loop; \
                     } \
                     /* failure */ \
                     PUSH_W(pf_stel); \
                     arity = arg = 1; \
                     COMM0("mkdclos"); \
                     goto L_mkdclos
#endif /* WITHTILDE */
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
                     arg = POP_W();                   /* arg:    praedikat   */
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
                     arg = POP_W();                   /* arg:    praedikat   */
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
                     arg = POP_W();                   /* arg:    praedikat   */
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
                     PPUSH_W(arg);                        /* praedikat */
                     ptdesc = (T_PTD)*code++;             /* ptdesc: zeiger auf cond-code */
                     GRAB_DESC(ptdesc);                   /* ptdesc: conditional-descriptor */
                     INC_REFCNT(ptdesc);                  /* refcount */
                     PPUSH_W(ptdesc);                     /* conditional */
                     arity = R_CONDI(*ptdesc,nargs);      /* anzahl argumente (incl. praedikat) */
#if WITHTILDE
		     nfv = (R_CONDI(*ptdesc,args) >> 2);
#endif /* WITHTILDE */
                     MAKEDESC(_desc,1,C_FUNC,TY_CLOS);    /* descriptor holen und initialisieren */
#if WITHTILDE
		     GET_HEAP(arity+nfv+1,A_CLOS(*_desc,pta));
#else
                     GET_HEAP(arity+1,A_CLOS(*_desc,pta)); /* heap holen und einhaengen */
#endif /* WITHTILDE */
                     L_CLOS(*_desc,args) = arity;         /* descriptor fuellen */
                     L_CLOS(*_desc,nargs) = 0;
#if WITHTILDE
		     L_CLOS(*_desc,nfv) = nfv;
#endif /* WITHTILDE */
                     L_CLOS(*_desc,ftype) = TY_CONDI;
                     RES_HEAP;
                     pth = (PTR_HEAPELEM)R_CLOS(*_desc,pta);
                     *pth++ = (T_HEAPELEM)TOP_W();        /* conditional */
#if WITHTILDE
		     if (R_CONDI(*(T_PTD)TOP_W(),args) & 0x3) { /* nur die unteren 2 bit */
#else
                     if (R_CONDI(*(T_PTD)TOP_W(),args)) {
#endif /* WITHTILDE */
                      /* argumente duerfen nicht aufgebraucht werden */
                       register T_PTD ptd;
                       for (help = 0; --arity > 0 ; help++) {
                         ptd = (T_PTD)MID_A(help);
                         T_INC_REFCNT(ptd);
                         *pth++ = (T_HEAPELEM)ptd;
                       }
                     }
                     else {
                       /* argumente duerfen aufgebraucht werden */
                       while (--arity > 0) {
                         *pth++ = (T_HEAPELEM)POP_A();
                       }
                     }
#if WITHTILDE
		     for ( help = 0; nfv > 0; nfv--,help++) { /* Tildeframe einsammeln */
		       register T_PTD ptd;

		       ptd = (T_PTD)MID_T(help);
		       T_INC_REFCNT(ptd);
		       *pth++ = (T_HEAPELEM)ptd;
		     }
#endif /* WITHTILDE */
                     PPOP_W();                            /* conditional */
                     *pth = (T_HEAPELEM)TOP_W();          /* praedikat   */
                     REL_HEAP;
                     WRITE_W(_desc);
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
                     PUSH_W(ptdesc);                 /* push function    */
                     arg = R_COMB(*ptdesc,nargs);    /* args needed      */
#if WITHTILDE
		     nfv = R_COMB(*ptdesc,args);     /* free variables   */
		     if ((arg == 0) && (nfv == 0))
		       goto loop;
#else
                     if (arg == 0) goto loop;        /* keine closure    */
#endif /* WITHTILDE */
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
                       PUSH_A(POP_W());              /* move argument */
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
                       PUSH_A(POP_W());
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
                     PUSH_A(POP_W());
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
                     PUSH_A(POP_W());
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
                       pth[arity] = (T_HEAPELEM)POP_W();
                     pth[arity] = (T_HEAPELEM)TOP_W();
                     REL_HEAP;
                     WRITE_W(_desc);
                     _desc = NULL;
                     goto loop;
    case I_FREE_A:   /* === FREE_A n : free an n-size argument frame === */
                     FUNC1("free_a");
                     arg = *code++;
                     /* arg: anzahl der von i freizugebenden argumente (>0!) */
                     do {
                       ptdesc = (T_PTD)POP_A();
                       T_DEC_REFCNT(ptdesc);
                     } while (--arg > 0);
                     goto loop;
    case I_FREE1_A:  /* === FREE1_A : free one argument === */
                     FUNC0("free1_a");
                     ptdesc = (T_PTD)POP_A();
                     T_DEC_REFCNT(ptdesc);
                     goto loop;
    case I_RTF:      /* === RTF : return function value === */
                     FUNC0("rtf");
                   L_rtf:
                     if (TEST_TAIL_FLAG()) {
                       DEL_TAIL_FLAG();
                     }
		     else {
		       SWITCHSTACKS();
		       PUSH_W(POP_A());                /* move result */
		     }
                     code = POP_RET();             /* returnadresse */
                     goto loop;
    case I_RTM:      /* === RTM : move conditionally from a to w and return === */
                     FUNC0("rtm");
                     if (TEST_TAIL_FLAG()) {
                       DEL_TAIL_FLAG();
                       PUSH_W(POP_A());                /* move */
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
                     PUSH_W(ptdesc);
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
                     PUSH_W(ptdesc);
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
#if WITHTILDE
		     FUNC3("mkbclos");
#else
                     FUNC2("mkbclos");
#endif /* WITHTILDE */
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
#if WITHTILDE
		     nfv   = *code++;       /* relativ freie Variablen */
#endif /* WITHTILDE */
                   L_mkbclos:
                     func = TY_COMB;        /* COMB-flag  */
                     goto L_mkclos;
    case I_MKCCLOS:  /* === MKCCLOS n m : make cond-closure === */
#if WITHTILDE
		     FUNC3("mkcclos");
#else
                     FUNC2("mkcclos");
#endif /* WITHTILDE */
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
#if WITHTILDE
		     nfv   = *code++;
#endif /* WITHTILDE */
                   L_mkcclos:
                     func = TY_CONDI;       /* COND-flag */
                     goto L_mkclos;
    case I_MKSCLOS:  /* === MKSCLOS n m : make switch/case-closure === */
#if WITHTILDE
		     FUNC3("mksclos");
#else
                     FUNC2("mksclos");

                     DBUG_PRINT ("INTER", ("MKSCLOS reached..."));
#endif /* WITHTILDE */
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
#if WITHTILDE
		     nfv   = *code++;
#endif /* WITHTILDE */
                   L_mksclos:
                     func = TY_CASE;        /* CASE-flag */
                     DBUG_PRINT ("INTER", ("L_mksclos reached..."));
                     goto L_mkclos;
    case I_MKDCLOS:  /* === MKDCLOS n m : make delta-closure === */
                     FUNC2("mkdclos");
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
#if WITHTILDE
		     nfv   = 0;
#endif /* WITHTILDE */                   
		   L_mkdclos:
                     func = TY_PRIM;        /* PRIM(FUNC)-flag */
                   L_mkclos:
                     /* stack w: oben die funktion, darunter die argumente */
                     /* arity: anzahl der argumente auf'm stack (>0!)      */
                     /* arg: anzahl der von der fkt. benoetigten argumente */
		     /* nfv: anzahl der relativ freien Variablen der fkt   */
		     /*      Sollten an dieser Stelle alle vorhanden sein! */
                     /* func: flag das den typ der funktion angibt         */

                   DBUG_PRINT ("INTER", ("L_mkclos reached..."));
#if WITHTILDE
		     if ((arity == 0) && (nfv == 0)) goto loop;
#else
                     if (arity == 0) goto loop;
#endif /* WITHTILDE */
                     /* dg 30.01.92 verwendung des globalen _desc wg. scavenge */
                     MAKEDESC(_desc,1,C_FUNC,TY_CLOS);
#if WITHTILDE
		     GET_HEAP(nfv+arg+1,A_CLOS(*_desc,pta));
#else
                     GET_HEAP(arg+1,A_CLOS(*_desc,pta));
#endif
                     L_CLOS(*_desc,args) = arity;
                     L_CLOS(*_desc,nargs) = arg - arity;
#if WITHTILDE
		     L_CLOS(*_desc,nfv) = nfv;
#endif /* WITHTILDE */
                     L_CLOS(*_desc,ftype) = func;      /* flag */
                     RES_HEAP;
                     pth = (PTR_HEAPELEM)R_CLOS(*_desc,pta);
#if WITHTILDE
                     DBUG_PRINT ("INTER", ("building closure..."));

		     /* Die closure sieht aus:                               */
		     /*                                                      */
		     /* Funktionspointer | DeBruinvariablen | Tildevariablen */
		     /*                                                      */
		     /* Die Anzahl der DeBruinvariablen oder die Zahl der    */
		     /* Tildevariablen kann Null sein, nicht jedoch beide    */

		     if (gamma_closure > 0) {
                       DBUG_PRINT ("INTER", ("gamma_closure"));
		       *pth++ = (T_HEAPELEM)POP_R(); /* Funktionszeiger liegt auf R */

		       while (arity-- > 0) {
			 *pth++ = (T_HEAPELEM)POP_W(); /* DeBruinvariablen */
		       }
		       {
			 if (gamma_closure == 1) {
			   while (nfv > 0) {
			     /* gammaclosures poppen ihre argumente von r */
			     /* da free_r direkt folgt                    */
			     *pth++ = (T_HEAPELEM)POP_R();
			     nfv--;
			   }
			 }
			 else { /* gamma_closure = 2 */
			   int old_nfv = nfv;

			   while (nfv > 0) {
			     /* tildevariablen werden vom freesw_t */
			     /* freigegeben                        */
			     *pth++ = (T_HEAPELEM)MID_R(old_nfv-nfv);
			     T_INC_REFCNT((PTR_DESCRIPTOR)MID_R(old_nfv-nfv));
			     nfv--;
			   }
			   SWITCHSTACKS_T(); /* for freesw_t */
			 }
		       }
		       gamma_closure = 0;
		     }
		     else { /* gamma_closure = 0 */
                       DBUG_PRINT ("INTER", ("no gamma_closure")); 
		       *pth++ = (T_HEAPELEM)POP_W(); /* Funktionszeiger liegt auf W */
                       DBUG_PRINT ("INTER", ("arity: %i, nfv: %i", arity, nfv));

		       while (arity-- > 0) {
			 *pth++ = (T_HEAPELEM)POP_W(); /* DeBruinvariablen */
		       }
		       {
			 int old_nfv = nfv;

			 while (nfv > 0) {
			   /* tildevariablen werden in der aufrufenden */
			   /* funktion freigegeben                     */
			   *pth++ = (T_HEAPELEM)MID_T(old_nfv-nfv);
			   T_INC_REFCNT((PTR_DESCRIPTOR)MID_T(old_nfv-nfv));
			   nfv--;
			 }
		       }
                       DBUG_PRINT("INTER", ("where's the problem ?"));
		     }
#else
                     do {
                       *pth++ = (T_HEAPELEM)POP_W();
                     } while (--arity >= 0);
#endif
                     REL_HEAP;

                     DBUG_PRINT ("INTER", ("refcnt: %d", (((T_PTD)_desc)->u.sc.ref_count)));

                     DBUG_PRINT("INTER", ("closure ready..."));

                     PPUSH_W(_desc);
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
                       ptdesc = (T_PTD)POP_A();
                       T_DEC_REFCNT(ptdesc);
                     }
                     code = POP_RET();             /* returnadresse */
                     goto loop;

#if NotUsed
                     /* --- specials for postprocessing --- */

    case I_MKTHEN:   /* === MKTHEN : close then-expression === */
                     /* stack a |.. arg_n .. arg_1 ptcond */
                     FUNC0("mkthen");
                     PUSH_R(_redcnt);                         /* save redcnt */
                     _redcnt = delta_count;                   /* reset       */
                     ptdesc = (T_PTD)POP_W();                 /* conditional */
                     PUSH_R(ptdesc);                          /* conditional */
                     if (R_CONDI(*ptdesc,args) == 0) {        /* jtrue       */
                       /* conditional gibt die argumente frei */
                       register T_PTD ptd;
                       arity = R_CONDI(*ptdesc,nargs)-2;
                       /* copy arguments */
                       for (arg = arity ; arg >= 0 ; arg-- ) {
                         ptd = (T_PTD)MID_W(arity);
                         T_INC_REFCNT(ptd);
                         PUSH_W(ptd);
                       }
                     }
                     SWITCHSTACKS();
                     PUSH_W(SA_TRUE);
                     PUSH_R(code);                            /* return addr */
                     code = R_CONDI(*ptdesc,ptc);
                     goto loop;
    case I_MKELSE:   /* === MKELSE : close else-expression === */
                     /* stack r |.. _redcnt ptcond, stack a |.. then  */
                     FUNC0("mkelse");
                     _redcnt = delta_count;                   /* reset       */
                     ptdesc = (T_PTD)TOP_R();                 /* conditional */
                     PUSH_R(POP_W());                         /* then-expr   */
                     if (R_CONDI(*ptdesc,args) == 0) {        /* jtrue       */
                       /* conditional gibt die argumente frei */
                       register T_PTD ptd;
                       arity = R_CONDI(*ptdesc,nargs)-2;
                       for (arg = arity ; arg >= 0 ; arg-- ) {
                         ptd = (T_PTD)MID_W(arity);
                         T_INC_REFCNT(ptd);
                         PUSH_W(ptd);
                       }
                     }
                     SWITCHSTACKS();
                     PUSH_W(SA_FALSE);
                     PUSH_R(code);                            /* return addr */
                     code = R_CONDI(*ptdesc,ptc);
                     goto loop;
    case I_MKCOND:   /* === MKCOND : make conditional === */
                     /* stack r |.. _redcnt ptcond then, stack a: |.. else */
                     FUNC0("mkcond");
                     arg = POP_W();                  /* else-expr      */
                     arity = POP_R();                /* then-expr      */
                     ptdesc = (T_PTD)POP_R();        /* conditional    */
                     DEC_REFCNT(ptdesc);             /* wird nicht frei */
                     PUSH_W(arity);                  /* then-expr      */
                     PUSH_W(arg);                    /* else-expr      */
                     PUSH_W(SET_ARITY(COND,2));      /* cond-constr    */
                     _redcnt = POP_R();              /* alter _redcnt  */
                     goto loop;

#endif /* NotUsed : mah 010294 */

/* #if !UH_ZF_PM */

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
#if UH_ZF_PM
         /*            post_mortem ("UH_PM: no _redcounting yet !"); NOW IT's THERE ! :-) */
#endif /* UH_ZF_PM */

#if WITHTILDE
		   L_casefail:
                     GRAB_DESC(ptdesc);         /* descriptor holen */
		     INC_REFCNT(ptdesc);
		     if (gamma_closure)
		       PUSH_R(ptdesc);
		     else
		       PUSH_W(ptdesc);
		     arg = R_CASE(*ptdesc,nargs); /* args needed      */
		     nfv = R_CASE(*ptdesc,args);  /* tilde frame size */
		     if ((arg == 0) && (nfv == 0))
		       goto loop;
		     arity = arg;
		     COMM0("mksclos");
		     goto L_mksclos;
#else
                     GRAB_DESC(ptdesc);         /* descriptor holen */
                     func = TY_CASE;
                     COMM0("nomatch");
                     goto L_nomatch;
#endif /* WITHTILDE */
    case I_WHEN:     /* === WHEN f : execute pattern match === */
                     FUNCX("when");
                     ptdesc = (T_PTD)*code++;
                     RES_HEAP;       /* heap wird in pm_am freigegeben !!! */
                     pm_am(R_MATCH(*ptdesc,code),TOP_W());
                     /* pm_am hinterlaesst auf stack w SA_TRUE (DOLLAR auf m) */
                     /* falls das pattern passt, HASH falls das pattern nicht */
                     /* passt und KLAA falls das matching nicht entscheidbar */
                     goto loop;
    case I_GUARD:    /* === GUARD f : reduce guard === */
                     FUNCL("guard");
                     ptdesc = (T_PTD)*code++;
                     if (T_HASH(TOP_W())) {
                       /* pattern passt nicht, body instr. folgt */
                       goto loop;
                     }
                     if (T_SA_TRUE(TOP_W())) {
                       /* pattern passt */
                       PPOP_W();                   /* SA_TRUE */
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
                     if (T_HASH(TOP_W())) {
                       /* pattern passt nicht */
                       PPOP_W();                   /* hash symbol */
                       goto loop;
                     }
                     if (T_SA_TRUE(TOP_W())) {
                       /* pattern passt und guard true */
                       register T_PTD ptd;
                       PPOP_W();                   /* SA_TRUE */
                       if (T_DOLLAR(TOP_R()))
                         PPOP_R();                 /* anzahl der patternvariablen */
                       ptd = (T_PTD)POP_W();       /* das zu matchende argument */
                       if (T_POINTER((int)ptd))
                         DEC_REFCNT(ptd);
                       code = (INSTR *)ptdesc;     /* jump   */
                       goto loop;
                     }
                     if (T_SA_FALSE(TOP_W())) {
                       /* pattern passt aber guard false */
                       register T_PTD ptd;
                       PPOP_W();                   /* SA_FALSE */
                       arg = VALUE(POP_R());       /* anzahl der patternvariablen */
                       /* variableninstanzen freigeben */
                       for (; arg > 0 ; arg--) {
                         ptd = (T_PTD)POP_A();
                         if (T_POINTER((int)ptd))
                           DEC_REFCNT(ptd);
                       }
                       goto loop;
                     }
                     /* guard weder true noch false */
                     GRAB_DESC(ptdesc);              /* BODY descriptor holen */
                   L_undecided:
                   DBUG_PRINT("inter",("bei L_undecided angekommen !"));
                     {
                       register T_PTD ptd;
#if !WITHTILDE
                       register int i;
#endif /* !WITHTILDE */
                       /* NOMAT descriptor anlegen */
                       MAKEDESC(ptd,1,C_EXPRESSION,TY_NOMAT);
                       /* nr. des fehlgeschlagenen when eintragen */
#if UH_ZF_PM
                       L_NOMAT(*ptd,act_nomat) = (int)R_CASE(*ptdesc,ptc);
#else
                       L_NOMAT(*ptd,act_nomat) = R_CASE(*ptdesc,args);
#endif

#if !UH_ZF_PM
                       ptdesc = R_CASE(*ptdesc,ptd);        /* CASE descriptor */
#endif
                       DBUG_PRINT("inter",("hier hinweg gekommen !"));
#if WITHTILDE
		       nfv = R_CASE(*ptdesc,args);           /* relativly free vars */
#endif
                       arity = R_CASE(*ptdesc,nargs);       /* gesamtanzahl argumente des CASE*/
                       ptdesc = R_CASE(*ptdesc,ptd);        /* SWITCH descriptor */
#if WITHTILDE
                       arg  = R_SWITCH(*ptdesc,anz_args);    /* Stelligkeit des SWITCH */
#else
                       arg  = R_SWITCH(*ptdesc,case_type);  /* NCASE: stelligkeit des SWITCH */
#endif /* WITHTILDE */
                       INC_REFCNT(ptdesc);
                       L_NOMAT(*ptd,ptsdes) = ptdesc;       /* SWITCH descriptor */

                       if (T_KLAA(TOP_W())) {
                         /* match nicht entscheidbar */
                         COMM0("match undecided");
                         DBUG_PRINT ("rinter", ("match undecided"));
#if WITHTILDE
			 GET_HEAP(4,A_NOMAT(*ptd,guard_body));
			 L_NOMAT(*ptd,guard_body)[0] = 3;          /* length   */
                         L_NOMAT(*ptd,guard_body)[1] = 0;          /* guard    */
                         L_NOMAT(*ptd,guard_body)[2] = 0;          /* body     */
                         PPOP_W();                                 /* KLAA     */
                         L_NOMAT(*ptd,guard_body)[3] = POP_W();    /* argument */

			 L_NOMAT(*ptd,reason) = 2;          /* match undecided */

#else
			 L_NOMAT(*ptd,guard_body) = 0;        /* no GUARD/BODY */
                         PPOP_W();                            /* KLAA */
#endif /* WITHTILDE */
                       }
                       else {

#if WITHTILDE
			 int top_r;
#endif /* WITHTILDE */

                         /* pattern passte, aber guard weder true noch false */
                         COMM0("guard undecided"); 
                         /* MATCH descriptor holen */
                         ptdesc = (T_PTD)R_SWITCH(*ptdesc,ptse)[R_NOMAT(*ptd,act_nomat)];
#if WITHTILDE
			 /* build closure around body expression */

			 /* [ closure -> comb -> sub [patvars] -> body ] */

			 top_r = VALUE(TOP_R());

			 MAKEDESC(_desc,1,C_FUNC,TY_CLOS);

			 GET_HEAP(nfv+top_r+1,A_CLOS(*_desc,pta));

			 L_CLOS(*_desc,args) = top_r;
			 L_CLOS(*_desc,nargs) = 0;
			 L_CLOS(*_desc,nfv) = nfv;
			 L_CLOS(*_desc,ftype) = TY_NOMATBODY;

			 pth = (PTR_HEAPELEM)R_CLOS(*_desc,pta);			 

			 *pth++ = (T_HEAPELEM)0; /* dummy, pointer to body is entered later */

			 while (top_r-- > 0) {
			   *pth++ = (T_HEAPELEM)POP_A(); /* DeBruinvariablen */
			 }
			 {
			   int dec_nfv = nfv;

			   while (dec_nfv > 0) {
			     /* tildevariablen werden in der aufrufenden */
			     /* funktion freigegeben                     */
			     *pth++ = (T_HEAPELEM)MID_T(nfv-dec_nfv);
			     T_INC_REFCNT((PTR_DESCRIPTOR)MID_T(nfv-dec_nfv));
			     dec_nfv--;
			   }
			 }

			 load_expr(A_MATCH(*ptdesc,body));

#else
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
                               ptdesc = (T_PTD)MID_A(help-VALUE(MID_E(i))); /* adressierung "von unten" */
                               T_INC_REFCNT(ptdesc);
                               UPDATE_E(i,ptdesc);
                             }
                           }
                         }

                         if ((i = VALUE(POP_R())) > 0 ) {
                           /* i: anz pm-bindungen >0 also body schliessen */
                           help = T_SNAP(TOP_E()) ? ARITY(POP_E())+i : i+1;
                           for (; i > 0 ; i--)
                             PUSH_E(POP_A());
                           PUSH_E(SET_ARITY(SNAP,help));
                         }
#endif /* WITHTILDE */
                         trav_e_a();
#if WITHTILDE
			 /* hier bekommt der ausdruck keinen expr-descriptor, */
			 /* sondern einen sub-descriptor, der die variablen   */
			 /* bindet, die auch das match bindet.==> nomat-post  */

			 trav_a_hilf();

			 {
			   register T_PTD ptd2;

			   MAKEDESC(ptd2,1,C_EXPRESSION,TY_SUB);

			   L_FUNC(*ptd2,nargs)    = VALUE(TOP_R()); /* bound variables */
			   L_FUNC(*ptd2,special)  = 0;
			   L_FUNC(*ptd2,namelist) = 0;

			   if (mvheap(A_FUNC((*ptd2),pte)) == NULL)       /* Auslagern */
			     POST_MORTEM("inter: Heap out of space");

			   L_CLOS(*_desc,pta)[0] = (T_HEAPELEM)ptd2;      /* sub */

			 }
			 /* and now number one:  the comb descriptor */

			 {
			   register T_PTD ptd2;

			   MAKEDESC(ptd2,1,C_FUNC,TY_COMB);

			   L_COMB((*ptd2),args)  = 0;       /* tildeargs */
			   L_COMB((*ptd2),nargs) = VALUE(POP_R());
			   L_COMB((*ptd2),ptd)   = (T_PTD)R_CLOS(*_desc,pta)[0]; /* sub */
			   L_COMB((*ptd2),ptc)   = (INSTR *)0;

			   L_CLOS(*_desc,pta)[0] = (T_HEAPELEM)ptd2;      /* comb */
			 }
#else
                         if ((ptdesc = (T_PTD)st_expr()) == NULL)
                           POST_MORTEM("inter: heap overflow (body)");
#endif /* WITHTILDE */

                         /* NOMAT-descriptor anlegen */
#if WITHTILDE
			 GET_HEAP(4,A_NOMAT(*ptd,guard_body));
                         L_NOMAT(*ptd,guard_body)[0] = 3;            /* length          */
                         L_NOMAT(*ptd,guard_body)[1] = POP_W();      /* guard           */
                         L_NOMAT(*ptd,guard_body)[2] = (int)_desc;   /* body in closure */
			 L_NOMAT(*ptd,guard_body)[3] = POP_W();      /* argument        */

			 L_NOMAT(*ptd,reason) = 3; /* guard undecided (not true nor false) */

			 _desc = NULL;
#else
                         GET_HEAP(3,A_NOMAT(*ptd,guard_body));
                         L_NOMAT(*ptd,guard_body)[0] = 2;            /* laenge */
                         L_NOMAT(*ptd,guard_body)[1] = POP_W();      /* guard */
                         L_NOMAT(*ptd,guard_body)[2] = (int)ptdesc;  /* body */
#endif /* WITHTILDE */
                       }
                       /* ptd: NOMAT descriptor */
                       /* arity : anzahl argumente des CASE */
                       /* arg : stelligkeit des SWITCH (NCASE) */

#if !WITHTILDE
                       if (arg > 1) {
                         /* NCASE: multiple pattern, load argument list onto the stack */
                         load_pmlist(ps_w,POP_W());
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
                         *pth++ = (T_HEAPELEM)POP_W();             /* argument */
                       arity -= 2;
                       if (arity > 0) {
                         *pth++ = SET_ARITY(SNAP,arity);
                         for (--arity ; --arity >= 0 ;) {
                           if (T_POINTER(MID_A(arity)))
                             INC_REFCNT((T_PTD)MID_A(arity));
                           *pth++ = (T_HEAPELEM)MID_A(arity);
                         }
                       }
                       *pth++ = (T_HEAPELEM)ptd;          /* NOMAT descr */
                       REL_HEAP;
                       PPUSH_W(ptdesc);                   /* EXPR descriptor */
#else
		       PUSH_W(ptd);                       /* NOMAT descriptor */

		       if (nfv > 0) {
			 /* build closure */
			 COMM0("mksclos");
			 code = rout_sclos;
                         DBUG_PRINT ("INTER", ("nfv for rout_sclos prepared !"));
			 *(code + 3) = nfv;
			 goto loop;
		       }

#endif /* !WITHTILDE */
                       code = rout_rtf;                   /* nachher return */
                       goto loop;
                     }
    case I_NOMATCH:  /* === NOMATCH f : handle nomatch === */
                     FUNCX("nomatch");
                     ptdesc = (T_PTD)*code++;
L_real_nomatch:
                     _redcnt += delta_count;
#if WITHTILDE
		     {
		       register T_PTD ptd;

		       nfv = R_CASE(*ptdesc,args);

		       /* NOMAT descriptor anlegen */
                       MAKEDESC(ptd,1,C_EXPRESSION,TY_NOMAT);
                       /* nr. des fehlgeschlagenen when eintragen */
                       L_NOMAT(*ptd,act_nomat) = 0;

		       INC_REFCNT(ptdesc);
                       L_NOMAT(*ptd,ptsdes) = R_CASE(*ptdesc,ptd);   /* SWITCH descriptor */

		       GET_HEAP(4,A_NOMAT(*ptd,guard_body));
		       L_NOMAT(*ptd,guard_body)[0] = 3;          /* length   */
		       L_NOMAT(*ptd,guard_body)[1] = 0;          /* guard    */
		       L_NOMAT(*ptd,guard_body)[2] = 0;          /* body     */
		       L_NOMAT(*ptd,guard_body)[3] = POP_W();    /* argument */
		       
		       L_NOMAT(*ptd,reason) = 1;                  /* nomatch */
		       PUSH_W(ptd);                      /* NOMAT descriptor */

		       if (nfv > 0) {
			 /* build closure */
			 COMM0("mksclos");
                         DBUG_PRINT ("INTER", ("nfv for rout_sclos prepared !"));
			 code = rout_sclos;
			 *(code + 3) = nfv;                 /* replace dummy */
			 goto loop;
		       }
		       goto loop;
		     }
#else
                     func = TY_CASE;

                     /* NCASE */
                     if ((int)R_SWITCH(*R_CASE(*ptdesc,ptd),case_type) > 1) {
                       /* multiple pattern, load argument list onto the stack */
                       load_pmilist(ps_w,POP_W());
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
                         help = MID_A(arg);
                         if (T_POINTER(help))
                           INC_REFCNT((T_PTD)help);
                         *pth++ = (T_HEAPELEM)help;
                       }
                     }
                     /* NCASE: argumente in die closure packen */
                     for ( ; --arity >= 0 ; ) {
                       *pth++ = POP_W();          /* argument */
                     }
                     REL_HEAP;
                     PPUSH_W(ptdesc);
                     goto loop;
#endif /* WITHTILDE */

/* #endif UH_ZF_PM */

    case I_DUPARG:   /* === DUPARG n : duplicate argument n (on stack i) === */
                     FUNC1("duparg");
                     arg = *code++;
                     ptdesc = (T_PTD)MID_A(arg);
                     T_INC_REFCNT(ptdesc);
                     PUSH_A(ptdesc);
                     goto loop;
    case I_LSEL:     /* === LSEL : select list element (inline) === */
                   L_lsel:
                     FUNC0("lsel");
                     if (_redcnt >= delta_count) {
                       /* red_lselect inline (vgl. rlstruct.c) */
                       arg = POP_W();         /* index */
                       arity = TOP_W();       /* list/string */
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
                               WRITE_W(ptdesc);
                               /* LAZY LISTS */
                               if (LazyLists
                                && R_DESC(*ptdesc,class) == C_FUNC
                                && R_UNIV(*ptdesc,nargs) == 0) {
                                 if (R_DESC(*ptd2,ref_count) > 1) {
                                   /* eval und update */
                                   PUSH_A(index);
                                   PUSH_A(ptd2);              /* liste */
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
                               WRITE_W(ptdesc);
                             }
                             DEC_REFCNT(ptd2);       /* liste freigeben */
                             goto loop;
                           }
                           /* index nicht im bereich */
                         }
                         /* keine liste */
                       }
                       /* argumente sind nicht pointer, lselect klappt nie! */
                       PPUSH_W(arg);
                     }
                     PUSH_W(LSELECT);
                     arity = arg = 2;
#if WITHTILDE
		     nfv = 0;
#endif /* WITHTILDE */
                     COMM0("mkdclos");
                     goto L_mkdclos;

/* #ifdef LAZY */
                     /* --- lazy specials --- */

    case I_EVAL:     /* === EVAL : evaluate === */
                     FUNC0("eval");
                     /* hier wird vorausgesetzt, dass auf a eine */
                     /* closure oder eine 0-stellige funktion liegt */
                   L_eval:
                     ptdesc = (T_PTD)POP_W();
                     if (R_DESC(*ptdesc,type) == TY_CLOS) {
                       /* (gesaettigte?) closure auf a */
                       register PTR_HEAPELEM pth;
                       arity = R_CLOS(*ptdesc,args); /* anzahl argumente */
                       PUSH_A(arity);                /* anzahl argumente */
                       PUSH_A(0);                    /* bottomsymbol */
                       RES_HEAP;
                       pth = R_CLOS(*ptdesc,pta);
                       /* argumente und funktion auf stack i */
                       do {
                         T_INC_REFCNT((T_PTD)*pth);
                         PUSH_A(*pth++);
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
                     while ((ptdesc = (T_PTD)POP_A()) != 0) {
                       PUSH_W(ptdesc);
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
                     arity = POP_A();                /* anzahl argumente */
                     COMM0("apply");
                     goto L_apply;
/* dg 02.05.91 cons-listen */
    case I_CONS:     /* === CONS : cons list (constructor) === */
                     FUNC0("cons");
                     MAKEDESC(ptdesc,1,C_CONS,TY_CONS);
                     L_CONS(*ptdesc,tl) = (T_PTD)POP_W();
                     L_CONS(*ptdesc,hd) = (T_PTD)TOP_W();
                     WRITE_W(ptdesc);
                     goto loop;
    case I_FCONS:    /* === FCONS : cons lists (primfunc)  === */
                     FUNC0("fcons");
                     MAKEDESC(ptdesc,1,C_CONS,TY_CONS);
                     L_CONS(*ptdesc,hd) = (T_PTD)POP_W();
                     L_CONS(*ptdesc,tl) = (T_PTD)TOP_W();
                     WRITE_W(ptdesc);
                     goto loop;
    case I_FIRST:    /* === FIRST : list head === */
                     FUNC0("first");
                     ptdesc = (T_PTD)TOP_W();
                     if (T_POINTER(ptdesc) && R_DESC(*ptdesc,class) == C_CONS) {
                       arg = (int)R_CONS(*ptdesc,hd);
                       T_INC_REFCNT((T_PTD)arg);
                       WRITE_W(arg);
                       DEC_REFCNT(ptdesc);
                       goto loop;
                     }
                     if (T_POINTER(ptdesc) && R_DESC(*ptdesc,class) == C_LIST) {
                       PUSH_W(TAG_INT(1));
                       goto L_lsel;
                     }
                     /* failure */
                     PUSH_W(M_FIRST);
                     arity = arg = 1;
#if WITHTILDE
		     nfv = 0;
#endif /* WITHTILDE */
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_REST:     /* === REST : list tail === */
                     FUNC0("rest");
                     ptdesc = (T_PTD)TOP_W();
                     if (T_POINTER(ptdesc) && R_DESC(*ptdesc,class) == C_CONS) {
                       arg = (int)R_CONS(*ptdesc,tl);
                       T_INC_REFCNT((T_PTD)arg);
                       WRITE_W(arg);
                       DEC_REFCNT(ptdesc);
                       goto loop;
                     }
                     if (T_POINTER(ptdesc) && R_DESC(*ptdesc,class) == C_LIST) {
                       PUSH_W(TAG_INT(1));
                       func = LCUT;
                       goto L_delta2;
                     }
                     PUSH_W(M_FIRST);
                     arity = arg = 1;
#if WITHTILDE
		     nfv = 0;
#endif /* WITHTILDE */
                     COMM0("mkdclos");
                     goto L_mkdclos;
    case I_UPDAT:    /* === UPDAT : update list element === */
                     FUNC0("updat");
                     ptdesc = (T_PTD)POP_A();        /* liste */
                     arg = POP_A();                  /* index */
                     arity = TOP_W();                /* element */
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
                       *pth++ = (T_HEAPELEM)POP_W();
                     *pth = (T_HEAPELEM)TOP_W();
                     REL_HEAP;
                     WRITE_W(_desc);
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
#if D_SLAVE
#if WITHTILDE
		     PUSH_R(arity);
#else
		     PUSH_E(arity);
#endif
#endif
                     goto loop;

    case I_DIST:     /* === DIST l1 l2 a b: starts distribution === */
#if DEBUG            /* HIER NOCH KEIN WITHTILDE */
                     if (OutInstr && debdisplay) { 
                       NEXTINSTR(); 
                       printf("%s(%1x, %1x, %1d, %1d)             \n","dist",*code,*(code+1),*(code+2),*(code+3)); 
                       } 
                     fprintf(TraceFp,"%s(%1x, %1x, %1d, %1d);\n","dist",*code,*(code+1),*(code+2),*(code+3));
#endif
#if D_SLAVE
#if (D_SLAVE && D_MESS && D_MSCHED)  /*** HIER MUSS RS VIELLEICHT NOCH WAS AENDERN!!! ***/
#if WITHTILDE
		     code = (*d_m_distribute)((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),*(code+4),*(code+5),DIST_N);
#else
		     code = (*d_m_distribute)((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),DIST_N);
#endif /* WITHTILDE */
#else
#if WITHTILDE
		     code = distribute((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),*(code+4),*(code+5),DIST_N);
#else
		     code = distribute((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),DIST_N);
#endif /* WITHTILDE */
#endif /* ...MESS... */
#else
                     code+=4;
#endif /* D_SLAVE */
                     goto loop;

    case I_DISTEND:  /* === DIST_END : Ende der Berechnung eines vert. Ausdrucks === */
                     FUNC0("dist_end");
#if D_SLAVE
#if NCDEBUG
		     DBNCsetdistlevel(-1);
#endif /* NCDEBUG */
#if WITHTILDE
		     arg = (POP_R() >> 1);
#else
		     arg = POP_R();
#endif
		     if (arg>= 0)

#if (D_SLAVE && D_MESS && D_MCOMMU)
                       {(*d_m_msg_send)(arg,MT_RESULT);
#else
		       {msg_send(arg,MT_RESULT);
#endif

/* #if D_MESS      was sollte das denn hier ?????
      fflush(d_mess_dat);
#endif */

			DBUG_RETURN (IR_DIST_END);}
		     else
#if WITHTILDE
		       WRITE_R(TOP_R()-1);
#else
		       WRITE_E(TOP_E()-1);
#endif
#endif
                     goto loop;

    case I_WAIT:     /* === WAIT : wartet auf vert. Ergebnisse === */
                     FUNC1("wait");
                     arity = *code++;
#if D_SLAVE
#if WITHTILDE
		     WRITE_R(TOP_R()-arity);
		     if (TOP_R()>0)
#else
		     WRITE_E(TOP_E()-arity);
		     if (TOP_E()>0)
#endif
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
#if D_SLAVE
#if WITHTILDE
		     PPOP_R();
#else
		     PPOP_E();
#endif
#endif
                     goto loop;

    case I_DISTB:     /* === DISTB l1 l2 a b: starts distribution  a bitmap === */
#if DEBUG             /* HIER NOCH KEIN WITHTILDE */
                     if (OutInstr && debdisplay) {
                       NEXTINSTR();
                       printf("%s(%1x, %1x, %1d, %1d)             \n","distb",*code,*(code+1),*(code+2),*(code+3));
                       }
                     fprintf(TraceFp,"%s(%1x, %1x, %1d, %1d);\n","distb",*code,*(code+1),*(code+2),*(code+3));
#endif
#if D_SLAVE
#if (D_SLAVE && D_MESS && D_MSCHED) /*** HIER MUSS RS VIELLEICHT NOCH WAS AENDERN!!! ***/
#if WITHTILDE
                     code = (*d_m_distribute)((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),*(code+4),*(code+5),DIST_B);
#else
                     code = (*d_m_distribute)((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),DIST_B);
#endif /* WITHTILDE */
#else
#if WITHTILDE
		     code = distribute((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),*(code+4),*(code+5),DIST_B);
#else
		     code = distribute((INSTR *)*code,(INSTR *)*(code+1),*(code+2),*(code+3),DIST_B);
#endif /* WITHTILDE */
#endif
#else
                     code+=4;                 
#endif  /* D_SLAVE */
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
#if D_SLAVE
                    MPRINT_COUNT (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, arity);
#endif
/* DBUG_PRINT("inter", ("*** ich habe gecountet...")); */

                    goto loop;

#if D_MHEAP

   case I_MHPALLON :  /* === MHPALLON : turn on heap alloc measure === */
                    
                      FUNC0("mhpallon");
       
#if D_SLAVE
                      DBUG_PRINT ("inter", ("*** ich schalte HEAP ALLOC an !!"));
                      d_m_newheap = m_newheap;
#endif
 
                      goto loop;

   case I_MHFREEON :  /* === MHFREEON : turn on heap free measure === */

                      FUNC0("mhfreeon");
      
#if D_SLAVE
                      DBUG_PRINT ("inter", ("*** ich schalte HEAP FREE an !!"));
                      d_m_freeheap = m_freeheap;
#endif

                      goto loop;

   case I_MHPCMPON :  /* === MHPCMPON : turn on heap compaction measure === */

                      FUNC0("mhpcmpon");

#if D_SLAVE
                      DBUG_PRINT ("inter", ("*** ich schalte HEAP COMPACT an !!"));
                      d_m_compheap = m_compheap;
#endif

                      goto loop;

   case I_MDESCALLON :  /* === MDESCALLON : turn on descriptor alloc measure === */

                        FUNC0("mdescallon");

#if D_SLAVE
                        DBUG_PRINT ("inter", ("*** ich schalte DESC ALLOC an !!"));
                        d_m_newdesc = m_newdesc;
#endif

                        goto loop;

   case I_MDESCFREON :  /* === MDESCFREON : turn on descriptor free measure === */

                        FUNC0("mdescfreon");
 
#if D_SLAVE
                        DBUG_PRINT ("inter", ("*** ich schalte DESC FREE an !!"));
                        d_m_freedesc = m_freedesc;
#endif

                        goto loop;

#endif /* D_MHEAP */

#if D_MSCHED

   case I_MSDISTON  :  /* === MSDISTON : turn on proc-distribute measure === */

                       FUNC0("msdiston");

#if D_SLAVE
                       DBUG_PRINT ("INTER", ("PROCESS DISTRIBUTION measure enabled !"));
                       d_m_distribute = m_distribute;
#endif

                       goto loop;

   case I_MSNODIST  : /* === MSNODIST : seperate exprs not to distribute === */

                      FUNC0("msnodist");

#if D_SLAVE
                      MPRINT_PROC_NODIST (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID);
#endif

                      goto loop;

   case I_MSDISTCK  : /* === MSDISTCK : print lenght of stackframe === */

                      FUNC1("msdistck");
                      arity = *code++;


#if D_SLAVE
                      MPRINT_PROC_DISTSTACKS(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, arity);
#endif

                      goto loop;

   case I_MPROCREON :  /* === MPROCREON : turn on process creation measure === */

                       FUNC0("mprocreon");

#if D_SLAVE
                       DBUG_PRINT ("INTER", ("PROCESS CREATE measure enabled !"));
                       d_m_process_create = m_process_create;
#endif

                       goto loop;

#endif /* D_MSCHED */

   case I_MSDISTEND :  /* === MSDISTEND : process distend measure === */

                       FUNC0("msdistend");

#if D_SLAVE
                       if (TOP_R() >= 0) {
                         DBUG_PRINT ("INTER", ("DISTEND zurueck an Prozessor %d !", TOP_R()));
                         MPRINT_PROC_DISTEND(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int)curr_pid->ppid, (int)TOP_R());
                         }
#endif

                       goto loop;

#if D_MSCHED

   case I_MPRSLEEON :  /* === MPRSLEEON : turn on process suspend measure === */

                       FUNC0("mprsleeon");

#if D_SLAVE
                       DBUG_PRINT ("RINTER", ("PROCESS SLEEP measure enabled !"));
                       d_m_process_sleep = m_process_sleep;
#endif
   
                       goto loop;

   case I_MPRORUNON :  /* === MPRORUNON : turn on process run measure === */

                       FUNC0("mprorunon");

#if D_SLAVE
                       DBUG_PRINT ("RINTER", ("PROCESS RUN measure enabled !"));
                       d_m_process_run = m_process_run;
#endif

                       goto loop;

   case I_MPROWAKON :  /* === MPROWAKON : turn on process wake up measure === */

                       FUNC0("mprowakon");

#if D_SLAVE
                       DBUG_PRINT ("RINTER", ("PROCESS WAKE UP measure enabled !"));
                       d_m_process_wakeup = m_process_wakeup;
#endif

                       goto loop;

     case I_MPROTERON :  /* === MPROTERON : turn on process run measure === */

                       FUNC0("mproteron");

#if D_SLAVE
                       DBUG_PRINT ("RINTER", ("PROCESS TERMINATE measure enabled !"));
                       d_m_process_terminate = m_process_terminate;
#endif

                       goto loop;

#endif /* D_MSCHED */

#if D_MCOMMU

   case I_MCOMSEBON :  /* === MCOMSEBON : turn on communication send begin measure === */

                       FUNC0("mcomsebon");

#if D_SLAVE
                       DBUG_PRINT ("INTER", ("SEND BEGIN measure enabled !"));
                       d_m_msg_send = m_msg_send;
                       m_msg_send_begin = 1;
#endif

                       goto loop;

   case I_MCOMSEEON :  /* === MCOMSEEON : turn on communication send end measure === */

                       FUNC0("mcomseeon");

#if D_SLAVE
                       DBUG_PRINT ("INTER", ("SEND END measure enabled !"));
                       d_m_msg_send = m_msg_send;
                       m_msg_send_end = 1;
#endif

                       goto loop;

   case I_MCOMREBON :  /* === MCOMREBON : turn on communication receive begin measure === */

                       FUNC0("mcomrebon");

#if D_SLAVE
                       DBUG_PRINT ("INTER", ("RECEIVE BEGIN measure enabled !"));
                       d_m_msg_receive = m_msg_receive;
                       m_msg_receive_begin = 1;
#endif

                       goto loop;

   case I_MCOMREEON :  /* === MCOMREEON : turn on communication receive measure === */

                       FUNC0("mcomreeon");

#if D_SLAVE
                       DBUG_PRINT ("INTER", ("RECEIVE measure enabled !"));
                       d_m_msg_receive = m_msg_receive;
                       m_msg_receive_end = 1;
#endif

                       goto loop;

#endif /* D_MCOMMU */

#if D_MSTACK

   case I_MSTCKPUON :  /* === MSTCKPUON : turn on stack-push measure === */

                       FUNC0("mstckpuon");

#if D_SLAVE
                       DBUG_PRINT ("RINTER", ("STACK PUSH measure enabled !"));
                       d_m_stack_push = m_stack_push;
#endif

                       goto loop;

   case I_MSTCKPPON :  /* === MSTCKPPON : turn on stack-pop measure === */

                       FUNC0("mstckppon");

#if D_SLAVE
                       DBUG_PRINT ("RINTER", ("STACK POP measure enabled !"));
                       d_m_stack_pop = m_stack_pop;
#endif 

                       goto loop;

   case I_MSTKSEGAL :  /* === MSTKSEGAL : turn on stack-seg alloc measure === */

                       FUNC0("mstksegal");

#if D_SLAVE
                       DBUG_PRINT ("inter", ("*** ich schalte STACK SEG ALLOC an !!"));
                       d_m_allocate_d_stack_segment = m_allocate_d_stack_segment;
#endif

                       goto loop;

   case I_MSTKSEGFR :  /* === MSTKSEGFR : turn on stack-seg free measure === */

                       FUNC0("mstksegfr");

#if D_SLAVE
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
/*dguh, 27.07.90*/                WRITE_E(POP_W());
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
/*dguh, 27.07.90*/                    PUSH_W((T_PTD)arg);
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
/*dguh, 27.07.90*/                    PUSH_W((T_PTD)arg);
/*dguh, 27.07.90*/                    goto loop;
/*dguh, 27.07.90*/                  }
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                /* Anfangsstueck leer, noch erzeugte Elemente in ptdesc */
/*dguh, 27.07.90*/                PPOP_E();                  /* nil */
/*dguh, 27.07.90*/                DEC_REFCNT(_nil);
/*dguh, 27.07.90*/                PUSH_W(ptdesc);
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_DROPP:       /* ( W ptr -- ) */
/*dguh, 27.07.90*/                FUNC0("dropp");
/*dguh, 27.07.90*/                ptdesc = (T_PTD)POP_W();
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
/*dguh, 27.07.90*/                arity = TOP_W();
/*dguh, 27.07.90*/                ptdesc = (T_PTD)MID_W(1);
/*dguh, 27.07.90*/                pth = R_LIST(*ptdesc,ptdv);
/*dguh, 27.07.90*/                arg = pth[ R_LIST(*ptdesc,dim) - VAL_INT(arity) ];
/*dguh, 27.07.90*/                if (T_POINTER(arg))
/*dguh, 27.07.90*/                  INC_REFCNT((T_PTD)arg);
/*dguh, 27.07.90*/                PUSH_A(arg);
/*dguh, 27.07.90*/                WRITE_W(TAG_INT(VAL_INT(arity)-1));
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_DIM:        /* ( W listdesc -- dim ) */
/*dguh, 27.07.90*/                FUNC0("Dimension");
/*dguh, 27.07.90*/                ptdesc = (T_PTD)TOP_W();
/*dguh, 27.07.90*/                PUSH_W( TAG_INT(R_LIST(*ptdesc,dim)) );
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_TZERO:      /* ( W n -- n ) ( 0 -- )  */
/*dguh, 27.07.90*/                FUNC1("testzero");
/*dguh, 27.07.90*/                if (VAL_INT(TOP_W())) {
/*dguh, 27.07.90*/                  code++;
/*dguh, 27.07.90*/                  goto loop;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                PPOP_W();
/*dguh, 27.07.90*/                code = (INSTR *)*code;
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_TFALSE:    /* ( W f -- f )  ( ff -- ) */
/*dguh, 27.07.90*/                FUNC1("testfalse");
/*dguh, 27.07.90*/                arg = TOP_W();
/*dguh, 27.07.90*/                if (IS_FALSE(arg)) {
/*dguh, 27.07.90*/                   PPOP_W();
/*dguh, 27.07.90*/                   code = (INSTR *)*code;
/*dguh, 27.07.90*/                   goto loop;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                code++;
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_TTRUE:    /* ( W f -- f )  ( tf -- ) */
/*dguh, 27.07.90*/                FUNC1("testtrue");
/*dguh, 27.07.90*/                arg = TOP_W();
/*dguh, 27.07.90*/                if (IS_TRUE(arg)) {
/*dguh, 27.07.90*/                   PPOP_W();
/*dguh, 27.07.90*/                   code = (INSTR *)*code;
/*dguh, 27.07.90*/                   goto loop;
/*dguh, 27.07.90*/                }
/*dguh, 27.07.90*/                code++;
/*dguh, 27.07.90*/                goto loop;
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/  case I_TLIST:      /* ( W desc -- flag ) */
/*dguh, 27.07.90*/                FUNC1("testlist");
/*dguh, 27.07.90*/                ptdesc = (T_PTD)TOP_W();
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
/*dguh, 27.07.90*/                  PPOP_E();
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
/*dguh, 27.07.90*/                  PPOP_E();
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
/*dguh, 27.07.90*/                   *pth++ = POP_W();           /* Listen-Closure */
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
/*dguh, 27.07.90*/                      if (T_POINTER(MID_A(help))) INC_REFCNT((T_PTD)MID_A(help)); /* uh 04Okt 91 */
/*dguh, 27.07.90*/                      *pth++=MID_A(help);   /* ZF-Variablen */
/*dguh, 27.07.90*/                   }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                   help=R_ZFCODE(*(T_PTD)arg,zfbound);
/*dguh, 27.07.90*/                   while (arity>help) {
/*dguh, 27.07.90*/                      arity--;
/*dguh, 27.07.90*/                      if (T_POINTER(MID_A(arity))) INC_REFCNT((T_PTD)MID_A(arity)); /* uh 04Okt91 */
/*dguh, 27.07.90*/                      *pth++=MID_A(arity);  /* lambda Variablen */
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
/*dguh, 27.07.90*/                  PPOP_E();
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
/*dguh, 27.07.90*/                  PPOP_E();
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
/*dguh, 27.07.90*/                   *pth++ = POP_W();
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
/*dguh, 27.07.90*/                        if (T_POINTER(MID_A(help))) INC_REFCNT((T_PTD)MID_A(help)); /* uh 04Okt91 */
/*dguh, 27.07.90*/                        *pth++=MID_A(help);   /* ZF-Variablen */
/*dguh, 27.07.90*/                     }
/*dguh, 27.07.90*/  
/*dguh, 27.07.90*/                     help=R_ZFCODE(*(T_PTD)arg,zfbound);
/*dguh, 27.07.90*/                     while (arity>help) {
/*dguh, 27.07.90*/                        arity--;
/*dguh, 27.07.90*/                        if (T_POINTER(MID_A(arity))) INC_REFCNT((T_PTD)MID_A(arity)); /* uh 04Okt91 */
/*dguh, 27.07.90*/                        *pth++=MID_A(arity);  /* lambda Variablen */
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
/*uh, 16.10.90*/
/*uh, 16.10.90*/  /* Pattern Matching Instruktionen */
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MKWFRAME:  /* size ( W x -- x 0(1) 0(2) ... 0(size) x ) */
/*uh, 16.10.90*/     /* MKWFRAME "MaKeWorkFRAME"  */
/*uh, 16.10.90*/     /* legt auf dem W_Stack ein Work-Frame der Groesse SIZE+1 an. In ihm werden bei */
/*uh, 16.10.90*/     /* geschachtelten Listen-Pattern die Zeiger und Zaehler der umfassenden Listen  */
/*uh, 16.10.90*/     /* abgelegt. X ist ein Zeiger auf das Argument. */
/*uh, 16.10.90*/     /* Es bleibt unter dem Stack-Frame liegen, */
/*uh, 16.10.90*/     /* damit es moeglich ist, das gesamte Argument zu erreichen.  */
/*uh, 16.10.90*/     /* Zusaetzlich wird das Argument auf das oberste Element des Frames kopiert.  */
/*uh, 16.10.90*/     /* Hier liegen waehrend des gesamten Matchens der Matchzeiger (second W) und das  */
/*uh, 16.10.90*/     /* aktuelle Argument (top W), das gematcht werden soll. */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  FUNC1("makewframe");
/*uh, 16.10.90*/                  arg = TOP_W();
/*uh, 16.10.90*/                  for (help=(int)(*code++);help>0; help--) {
/*uh, 16.10.90*/                     PUSH_W(0);
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  PUSH_W(arg);   /* Refcount Behandlung???? */
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MKBTFRAME:  /* size ( E x -- x 1(1) 1(2) ... 1(size) ) */
/*uh, 16.10.90*/     /* BTFRAME "MaKeBackTrackFRAME"   */
/*uh, 16.10.90*/     /* legt auf dem E-Stack ein Frame der Groesse SIZE an, indem im Backtrackfall */
/*uh, 16.10.90*/     /* der Matchzeiger gesichert wird.  */
/*uh, 16.10.90*/                  FUNC1("makebtframe");
/*uh, 16.10.90*/                  for (help=(int)(*code++);help>0; help--) {
/*uh, 16.10.90*/                     PUSH_R(1); /* 1 wegen RefCount Behandlung (eg. kein Pointer) */
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MKAFRAME:  /* size ( I x -- x 1(1) 1(2) ... 1(size) ) */
/*uh, 16.10.90*/    /* MKAFRAME "MaKeInkanationFrame" */
/*uh, 16.10.90*/    /* legt auf dem Inkarnations-Stack A ein Frame der Groesse SIZE an, in das die */
/*uh, 16.10.90*/    /* Bindungen der Variablen des Patterns aufgenommen werden koennen. */
/*uh, 16.10.90*/                  FUNC1("makeiframe");
/*uh, 16.10.90*/                  for (help=(int)(*code++);help>0; help--) {
/*uh, 16.10.90*/                     PUSH_A(TAG_INT(42));  /* 1 wegen Refcount-Behandlung (kein Pointer) */
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_RMWFRAME: /* size ( W e1 e2 ... esize -- ) */
/*uh, 16.10.90*/      /* RMWFRAME "ReMoveWorkFRAME"  */
/*uh, 16.10.90*/      /* Entfernt das Frame der Groesse SIZE vom Work-Stack. */
/*uh, 16.10.90*/                  FUNC1("remwframe");
/*uh, 16.10.90*/                  for (help=(int)(*code++); help>0; help--) {
/*uh, 16.10.90*/                     PPOP_W() ;   /* Refcount-Behandlung???? */
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_RMBTFRAME: /* size ( E e1 e2 ... esize -- ) */
/*uh, 16.10.90*/      /* RMBTFRAME "ReMoveBackTrackFRAME" */
/*uh, 16.10.90*/      /* Entfernt das Frame der Groesse SIZE vom Backtrack-Stack E */
/*uh, 16.10.90*/                  FUNC1("rembtframe");
/*uh, 16.10.90*/                  for (help=(int)(*code++); help>0; help--) {
/*uh, 16.10.90*/                     PPOP_R() ;   /* Refcount-Behandlung???? */
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_BIND: /* index ( W: x -- x ) ( I: bindings -- bindings' )  */
/*uh, 16.10.90*/    /* BIND */
/*uh, 16.10.90*/    /* Bindet die durch INDEX referenzierte Variable des Patterns an den Wert X,  */
/*uh, 16.10.90*/    /* der oben auf dem Work-Stack steht, indem X an die Stelle INDEX in das Frame */
/*uh, 16.10.90*/    /* auf dem Inkarnations-Stack geschrieben wird. */
/*uh, 16.10.90*/                  FUNC1("bind");
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  ptdesc = (T_PTD)TOP_W();
                                  if (T_POINTER(MID_A(arg)))
                                     DEC_REFCNT((T_PTD)MID_A(arg));
/*uh, 16.10.90*/                  if (T_POINTER((int)ptdesc))
/*uh, 16.10.90*/                     INC_REFCNT(ptdesc);
/*uh, 16.10.90*/                  UPDATE_A(arg,(STACKELEM)ptdesc);
/*uh, 16.10.90*/                  goto loop;

/*uh, 16.10.90*/  case I_BINDS: /* match't ein Element eines strings, neuen Stringdeskripter drumherum bauen... */
/*uh, 16.10.90*/                  FUNC1("binds"); 
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  ptdesc = (T_PTD)TOP_W();
                                  if (T_POINTER(MID_A(arg)))
                                     DEC_REFCNT((T_PTD)MID_A(arg));
/*uh, 16.10.90*/                  if (T_POINTER((int)ptdesc))
/*uh, 16.10.90*/                     INC_REFCNT(ptdesc);
                                  if ((_desc = newdesc()) == NULL)
                                     post_mortem("rinter: BINDS heap out of space");
                                  LI_DESC_MASK(_desc,1,C_LIST,TY_STRING);
                                  GET_HEAP(1,A_LIST(*(T_PTD)_desc,ptdv));
                                  L_LIST(*(T_PTD)_desc,dim) = 1;  
                                  pth = R_LIST(*(T_PTD)_desc,ptdv); 
                                  RES_HEAP;
                                  *pth = (int) ptdesc; 
                                  REL_HEAP;
/*uh, 16.10.90*/                  UPDATE_A(arg,(STACKELEM)_desc);
/*uh, 16.10.90*/                  goto loop;

/*uh, 16.10.90*/
#define LEN arg
#define ARG1 arg
#define ARG2 ((int)func)
#define FAIL ((int)func)
#define UNDECIDED ((int)ptdesc)
#define ARGUMENT help
#define IS_VAR_OR_CLOS_DESC(x)  ( (x)==C_EXPRESSION || (x)==C_CONSTANT) /* || \ */
/*uh, 16.10.90*/                                 /*   (x) == C_FUNC) */
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MATCHC:  /* Constant, FAIL-Addr, UNDECIDED-Addr ( W x -- x ) */
/*uh, 16.10.90*/      /* MATCHC "MATCH-Constant" */
/*uh, 16.10.90*/      /* Vergleicht das oberste Element X des Work-Stacks mit der Konstanten CONSTANT */
/*uh, 16.10.90*/      /* Sind sie gleich, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/      /* unmittelbar auf MATCHC folgt. */
/*uh, 16.10.90*/      /* Sind sie ungleich, so wird die Ausfuehrung an der Adresse FAIL-Addr fortgesetzt.*/
/*uh, 16.10.90*/      /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/      /* UNDECIDED-Addr fortgesetzt. */
/*uh, 16.10.90*/                  FUNC3("matchc");
/*uh, 16.10.90*/  /* printf("\n refcnt(_nil)=%d",R_DESC(*_nil,ref_count)); */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG1=*code++;
/*uh, 16.10.90*/                  ARG2=TOP_W();
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (ARG1==ARG2) goto matchc_succeed;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if IS_VAR_OR_CLOS_DESC(help)
/*uh, 16.10.90*/                        goto matchc_undecided;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                     /* Behandlung von TOP_W == C_SCALAR-Descriptor */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                     goto matchc_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  /* if (T_CON(ARG2)) goto matchc_undecided; AP oder SNAP */
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchc_fail:
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchc_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchc_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MATCHINT:  /* intConstant, FAIL-Addr, UNDECIDED-Addr ( W x -- x ) */
/*uh, 16.10.90*/      /* MATCHINT "MATCH-Integer" */
/*uh, 16.10.90*/      /* Vergleicht das oberste Element X des Work-Stacks mit der Integer */
/*uh, 16.10.90*/      /* Konstanten INTCONSTANT */
/*uh, 16.10.90*/      /* Sind sie gleich, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/      /* unmittelbar auf MATCHINT folgt. */
/*uh, 16.10.90*/      /* Sind sie ungleich, so wird die Ausfuehrung an der Adresse FAIL-Addr fortgesetzt.*/
/*uh, 16.10.90*/      /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/      /* UNDECIDED-Addr fortgesetzt. */
/*uh, 16.10.90*/                  FUNC3("matchint");
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG1=*code++;
/*uh, 16.10.90*/                  ARG2=TOP_W();
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_INT(ARG2)) { /* Argument auch mit Tag versehen */
/*uh, 16.10.90*/                     if (EQ_INT(ARG1,ARG2)==SA_TRUE)  /* gleich */
/*uh, 16.10.90*/                    /*   if (VAL_INT(ARG2)==ARG1) */  /* gleich */
/*uh, 16.10.90*/                        goto matchint_succeed;
/*uh, 16.10.90*/                     goto matchint_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if  (IS_VAR_OR_CLOS_DESC(help))
/*uh, 16.10.90*/                          goto matchint_undecided;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                     goto matchint_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchint_fail:
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchint_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchint_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/

/*uh, 16.10.90*/  case I_MATCHREAL:  
/*uh, 16.10.90*/      /* Vergleicht das oberste Element X des Work-Stacks mit der Integer */
/*uh, 16.10.90*/      /* Konstanten INTCONSTANT */
/*uh, 16.10.90*/                  FUNC3("matchreal");
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG1=*code++;
/*uh, 16.10.90*/                  ARG2=TOP_W();
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2) && (R_DESC(*(T_PTD)ARG2,class)==C_SCALAR) && (R_DESC(*(T_PTD)ARG2,type)==TY_REAL)) {
/*uh, 16.10.90*/                     if (R_SCALAR(*(T_PTD)ARG1,valr) == R_SCALAR(*(T_PTD)ARG2,valr))
/*uh, 16.10.90*/                        goto matchreal_succeed;
/*uh, 16.10.90*/                     goto matchreal_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if  (IS_VAR_OR_CLOS_DESC(help))
/*uh, 16.10.90*/                          goto matchreal_undecided;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                     goto matchreal_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchreal_fail:
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchreal_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchreal_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/

/*uh, 16.10.90*/  case I_MATCHDIGIT:
/*uh, 16.10.90*/                  FUNC3("matchdigit");
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG1=*code++;
/*uh, 16.10.90*/                  ARG2=TOP_W();
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2) && (R_DESC(*(T_PTD)ARG2,class)==C_DIGIT)) {
/*uh, 16.10.90*/                     if (digit_eq(ARG1,ARG2))
/*uh, 16.10.90*/                        goto matchdigit_succeed;
/*uh, 16.10.90*/                     goto matchdigit_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if  (IS_VAR_OR_CLOS_DESC(help))
/*uh, 16.10.90*/                          goto matchdigit_undecided;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                     goto matchdigit_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchdigit_fail:
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchdigit_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchdigit_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;


/*uh, 16.10.90*/  case I_MATCHTRUE:  /* FAIL-Addr, UNDECIDED-Addr ( W x -- x ) */
/*uh, 16.10.90*/      /* MATCHTRUE "MATCH-true */
/*uh, 16.10.90*/      /* Vergleicht das oberste Element X des Work-Stacks mit der booleschen*/
/*uh, 16.10.90*/      /* Konstanten TRUE */
/*uh, 16.10.90*/      /* Sind sie gleich, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/      /* unmittelbar auf MATCHTRUE folgt. */
/*uh, 16.10.90*/      /* Sind sie ungleich, so wird die Ausfuehrung an der Adresse FAIL-Addr fortgesetzt.*/
/*uh, 16.10.90*/      /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/      /* UNDECIDED-Addr fortgesetzt. */
/*uh, 16.10.90*/                  FUNC2("matchtrue");
/*uh, 16.10.90*/  /* printf("\n refcnt(_nil)=%d",R_DESC(*_nil,ref_count)); */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG2=TOP_W();
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_BOOLEAN(ARG2)) {
/*uh, 16.10.90*/                    if (T_SA_TRUE(ARG2)) goto matchtrue_succeed;
/*uh, 16.10.90*/                    goto matchtrue_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(help))
/*uh, 16.10.90*/                       goto matchtrue_undecided;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                    goto matchtrue_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchtrue_fail:
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchtrue_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchtrue_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MATCHFALSE:  /* FAIL-Addr, UNDECIDED-Addr ( W x -- x ) */
/*uh, 16.10.90*/      /* MATCHFALSE "MATCH-false" */
/*uh, 16.10.90*/      /* Vergleicht das oberste Element X des Work-Stacks mit der booleschen*/
/*uh, 16.10.90*/      /* Konstanten FALSE */
/*uh, 16.10.90*/      /* Sind sie gleich, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/      /* unmittelbar auf MATCHFALSE folgt. */
/*uh, 16.10.90*/      /* Sind sie ungleich, so wird die Ausfuehrung an der Adresse FAIL-Addr fortgesetzt.*/
/*uh, 16.10.90*/      /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/      /* UNDECIDED-Addr fortgesetzt. */
/*uh, 16.10.90*/                  FUNC2("matchfalse");
/*uh, 16.10.90*/  /* printf("\n refcnt(_nil)=%d",R_DESC(*_nil,ref_count)); */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG2=TOP_W();
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_BOOLEAN(ARG2)) {
/*uh, 16.10.90*/                    if (T_SA_FALSE(ARG2)) goto matchfalse_succeed;
/*uh, 16.10.90*/                    goto matchfalse_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(help))
/*uh, 16.10.90*/                       goto matchfalse_undecided;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                     goto matchfalse_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchfalse_fail:
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchfalse_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchfalse_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MATCHBOOL:  /* boolConstant, FAIL-Addr, UNDECIDED-Addr ( W x -- x ) */
/*uh, 16.10.90*/      /* MATCHBOOL "MATCH-bool" */
/*uh, 16.10.90*/      /* Vergleicht das oberste Element X des Work-Stacks mit der booleschen*/
/*uh, 16.10.90*/      /* Konstanten BOOLCONSTANT */
/*uh, 16.10.90*/      /* Sind sie gleich, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/      /* unmittelbar auf MATCHBOOL folgt. */
/*uh, 16.10.90*/      /* Sind sie ungleich, so wird die Ausfuehrung an der Adresse FAIL-Addr fortgesetzt.*/
/*uh, 16.10.90*/      /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/      /* UNDECIDED-Addr fortgesetzt. */
/*uh, 16.10.90*/                  FUNC3("matchbool");
/*uh, 16.10.90*/  /* printf("\n refcnt(_nil)=%d",R_DESC(*_nil,ref_count)); */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG1=*code++;
/*uh, 16.10.90*/                  ARG2=TOP_W();
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_BOOLEAN(ARG2)) {
/*uh, 16.10.90*/                    if (EQ_BOOL(ARG1,ARG2)==SA_TRUE) goto matchbool_succeed;
/*uh, 16.10.90*/                    goto matchbool_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(help))
/*uh, 16.10.90*/                       goto matchbool_undecided;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                     goto matchbool_fail;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchbool_fail:
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchbool_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchbool_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MATCHNIL:  /* FAIL-Addr, UNDECIDED-Addr ( W x -- x ) */
/*uh, 16.10.90*/      /* MATCHNIL "MATCH-NIL" */
/*uh, 16.10.90*/      /* Vergleicht das oberste Element X des Work-Stacks mit der Konstanten NIL */
/*uh, 16.10.90*/      /* Sind sie gleich, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/      /* unmittelbar auf MATCHNIL folgt. */
/*uh, 16.10.90*/      /* Sind sie ungleich, so wird die Ausfuehrung an der Adresse FAIL-Addr fortgesetzt.*/
/*uh, 16.10.90*/      /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/      /* UNDECIDED-Addr fortgesetzt. */
/*uh, 16.10.90*/                  FUNC2("matchnil");
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG2=TOP_W();
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(help)) goto matchnil_undecided;
/*uh, 16.10.90*/                    if ((T_PTD)ARG2==_nil) goto matchnil_succeed;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/      /*       matchnil_fail: */
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchnil_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchnil_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/

/*uh, 16.10.90*/  case I_MATCHPRIM:  /* Constant, FAIL-Addr, UNDECIDED-Addr ( W x -- x ) */
/*uh, 16.10.90*/      /* MATCHPRIM "MATCH-Constant" */
/*uh, 16.10.90*/      /* Vergleicht das oberste Element X des Work-Stacks mit der Konstanten CONSTANT */
/*uh, 16.10.90*/      /* Sind sie gleich, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/      /* unmittelbar auf MATCHC folgt. */
/*uh, 16.10.90*/      /* Sind sie ungleich, so wird die Ausfuehrung an der Adresse FAIL-Addr fortgesetzt.*/
/*uh, 16.10.90*/      /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/      /* UNDECIDED-Addr fortgesetzt. */
/*uh, 16.10.90*/                  FUNC3("matchprim");
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ARG1=*code++;
/*uh, 16.10.90*/                  ARG2=TOP_W() & ~F_EDIT;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (T_POINTER(ARG2)) {
/*uh, 16.10.90*/                    help = R_DESC(*(T_PTD)ARG2,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(help)) goto matchprim_undecided;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  if (ARG1==ARG2) goto matchprim_succeed;
/*uh, 16.10.90*/            /*  if (T_FUNC(ARG2) && (ARG1==FUNC_INDEX(ARG2))) goto matchprim_succeed; */
/*uh, 16.10.90*/
/*uh, 16.10.90*/ /*            matchprim_fail: */
/*uh, 16.10.90*/                  code = (int *)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchprim_undecided:
                                  PUSH_T (KLAA);
/*uh, 16.10.90*/                  code+=1;
/*uh, 16.10.90*/                  code = (int*)*code;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/               matchprim_succeed:
/*uh, 16.10.90*/                  code+=2;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_BACKUP: /* range ( W p *p -- p-range *p )  */
/*uh, 16.10.90*/      /* Vermindert den Zeiger, der als zweites Element auf dem Work-Stack steht, */
/*uh, 16.10.90*/      /* um RANGE.  Ist RANGE 1, so zeigt er dann auf das vorangehende Element. */
/*uh, 16.10.90*/                  FUNC1("backup");
/*uh, 16.10.90*/                  help = *code++;
/*uh, 16.10.90*/                  arg = (int) (((PTR_HEAPELEM)MID_W(1)) - help);
/*uh, 16.10.90*/                  UPDATE_W(1,arg); /* uh 04 Okt 91 */
/*uh, 16.10.90*/                  /* MID_W(1)= (STACKELEM)arg; */
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_ADVANCE: /* range ( W p *p -- p+range *p )  */
/*uh, 16.10.90*/      /* Erhoeht den Zeiger, der als zweites Element auf dem Work-Stack steht, um RANGE. */
/*uh, 16.10.90*/      /* Ist RANGE 1, so zeigt er dann auf das naechste Element. */
/*uh, 16.10.90*/                  FUNC1("advance");
/*uh, 16.10.90*/                  help = *code++;
/*uh, 16.10.90*/                  arg = (int) (((PTR_HEAPELEM)MID_W(1)) + help);
/*uh, 16.10.90*/                  UPDATE_W(1,arg);  /* uh 04 Okt 91 */
/*uh, 16.10.90*/                  /* MID_W(1)= (STACKELEM)arg; */
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_DEREF:   /* ( W p x -- p p* ) */
/*uh, 16.10.90*/    /* "DEREFerence" */
/*uh, 16.10.90*/    /* Dereferenziert den aktuellen Matchzeiger (second W) und schreibt das so gewonnene */
/*uh, 16.10.90*/    /* aktuelle Match-Element auf das oberste Element des Work-Stacks. */
/*uh, 16.10.90*/                  FUNC0("deref");
/*uh, 16.10.90*/                  ptdesc = (T_PTD)(*((PTR_HEAPELEM)MID_W(1)));
/*uh, 16.10.90*/                  WRITE_W((STACKELEM)ptdesc);
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_DROP:    /* ( W p -- ) */
/*uh, 16.10.90*/     /* DROP */
/*uh, 16.10.90*/     /* entfernt das oberste Element P des Work-Stacks. Ist dieses Element ein Zeiger */
/*uh, 16.10.90*/     /* auf einen Descriptor, so wird der Refcount behandelt. */
/*uh, 16.10.90*/                  FUNC0("drop");
/*uh, 16.10.90*/                  ptdesc= (T_PTD)POP_W();
/*uh, 16.10.90*/                  if (ptdesc && T_POINTER(ptdesc)) DEC_REFCNT(ptdesc);
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_PICK:    /* depth  ( W e(depth) ... e(1) e(0) -- e(depth) ... e(1) e(depth) ) */
/*uh, 16.10.90*/      /* PICK */
/*uh, 16.10.90*/      /* Ueberschreibt das oberste Elements des Work-Stacks durch das DEPTHte Element */
/*uh, 16.10.90*/      /* des Work-Stacks (gezaehlt von 0). */
/*uh, 16.10.90*/                  FUNC1("pick");
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  arg = (int) (MID_W(arg));
/*uh, 16.10.90*/                  WRITE_W(arg);   /* Refcount */
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_RESTOREBT: /* depth  ( W ptr x -- ptr' x ) */
/*uh, 16.10.90*/     /* "RESTOREBackTrack"  */
/*uh, 16.10.90*/     /* Kopiert den im Backtrackframe an Position DEPTH gesicherten Wert des Matchzeigers */
/*uh, 16.10.90*/     /* ueber den aktuellen Zeiger (second W). */
/*uh, 16.10.90*/                  FUNC1("restorebt");
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  ptdesc = (T_PTD)MID_R(arg+BTPTR);
/*uh, 16.10.90*/                  UPDATE_W(1,(STACKELEM)ptdesc); /* Refcount!!! */
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_SAVEBT: /* depth ( E: frame -- frame ) ( W ptr x -- ptr x )  */
/*uh, 16.10.90*/     /* SAVEBT "SAVEBackTrack" */
/*uh, 16.10.90*/     /* Nimmt den aktuellen Matchzeiger (zweites Element vom Work-Stack) und sichert */
/*uh, 16.10.90*/     /* diesen Wert auf dem Backtrack-Stack an die Stelle DEPTH. */
/*uh, 16.10.90*/                  FUNC1("savebt");
/*uh, 16.10.90*/                  arg = *code++;  /* depth */
/*uh, 16.10.90*/                  UPDATE_R(arg+BTPTR,MID_W(1));
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_SAVEPTR: /* depth ( W e(depth-1) ... e(1) e(0) -- e(1) ... e(1) e(0) ) */
/*uh, 16.10.90*/     /*    Sichert den aktuellen Matchzeiger im Stackframe an der Stelle DEPTH-1. */
/*uh, 16.10.90*/                  FUNC1("saveptr");
/*uh, 16.10.90*/                  /* WAIT; */
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  ptdesc = (T_PTD)MID_W(1);
/*uh, 16.10.90*/                  UPDATE_W(arg,(STACKELEM)ptdesc); /* Refcount */
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_RESTPTR: /* depth ( W e(depth-1) ... e(1) e(0) -- e(depth-1) ... e(depth-1) e(0) ) */
/*uh, 16.10.90*/     /* Setzt den Matchpointer E(1) auf den frueher im Stackframe gesicherten Wert */
/*uh, 16.10.90*/     /* E(DEPTHI-1).  */
/*uh, 16.10.90*/                  FUNC1("restptr");
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  ptdesc = (T_PTD)MID_W(arg);
/*uh, 16.10.90*/                  UPDATE_W(1,(STACKELEM)ptdesc); /* Refcount!!! */
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_ENDLIST: /* ( W p x -- p~ x ) */
/*uh, 16.10.90*/     /* Setzt den Matchzeiger P auf des Ende der Liste (P~), die durch den */
/*uh, 16.10.90*/     /* Listen-Descriptor X beschrieben wird. */
/*uh, 16.10.90*/                  FUNC0("endlist");
/*uh, 16.10.90*/                  ptdesc = (T_PTD) (TOP_W());
#ifdef DEBUG
/*uh, 16.10.90*/                  if (!ptdesc || !T_POINTER(ptdesc) || !(R_DESC(*ptdesc,class)==C_LIST)) {
/*uh, 16.10.90*/                     post_mortem("rinter: ENDLIST expects LIST on top of W-Stack");
/*uh, 16.10.90*/                  }
#endif
/*uh, 16.10.90*/                  arg = (int) (R_LIST(*ptdesc,ptdv)+R_LIST(*ptdesc,dim));
/*uh, 16.10.90*/                  UPDATE_W(1,(STACKELEM)arg);
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_NESTLIST: /* ( W p x -- p~ x ) */
/*uh, 16.10.90*/     /* Setzt den Matchzeiger P auf das Anfang P~ der Liste, die durch den */
/*uh, 16.10.90*/     /* Listen-Descriptor X beschrieben wird. */
/*uh, 16.10.90*/                  FUNC0("nestlist");
/*uh, 16.10.90*/                  /* WAIT; */
/*uh, 16.10.90*/                  ptdesc = (T_PTD) (TOP_W());
#ifdef DEBUG
/*uh, 16.10.90*/                  if (!ptdesc || !T_POINTER(ptdesc) || !(R_DESC(*ptdesc,class)==C_LIST)) {
/*uh, 16.10.90*/                     post_mortem("rinter: NESTLIST expects LIST on top of W-Stack");
/*uh, 16.10.90*/                  }
#endif
/*uh, 16.10.90*/                  if (R_LIST(*ptdesc,dim))
/*uh, 16.10.90*/                    arg = (int) R_LIST(*ptdesc,ptdv);
/*uh, 16.10.90*/                  else
/*uh, 16.10.90*/                    arg = (int) _nil;  /* sth to deref */
/*uh, 16.10.90*/                  UPDATE_W(1,(STACKELEM)arg);
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MATCHARB: /* len, FAIL, UNDECIDED ( W x -- x ) */
/*uh, 16.10.90*/  /*  Prueft, ob das oberste Element des Work-Stacks X eine Liste ist, die */
/*uh, 16.10.90*/  /*  mindestens die Stelligkeit LEN besitzt. */
/*uh, 16.10.90*/  /*  Ist dies der Fall, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/  /*  unmittelbar auf MATCHARB folgt.  */
/*uh, 16.10.90*/  /*  Sonst wird die Ausfuehrung an der Adresse FAIL fortgesetzt. */
/*uh, 16.10.90*/  /*  Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/  /*  UNDECIDED fortgesetzt.  */
/*uh, 16.10.90*/                  FUNC3("matcharb");
/*uh, 16.10.90*/                  LEN = *code++;
/*uh, 16.10.90*/                  FAIL = *code++;
/*uh, 16.10.90*/                  UNDECIDED = *code++;
/*uh, 16.10.90*/                  ARGUMENT=TOP_W();
/*uh, 16.10.90*/                  if (T_POINTER(ARGUMENT)) {
/*uh, 16.10.90*/                    /* Argument ist Pointer ... */
/*uh, 16.10.90*/                    if ((R_DESC(*(T_PTD)ARGUMENT,class)== C_LIST) &&
                                        (R_DESC(*(T_PTD)ARGUMENT,type)== TY_UNDEF) &&
/*uh, 16.10.90*/                        (R_LIST(*(T_PTD)ARGUMENT,dim) >= LEN)) goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                    /* ... aber keine Liste */
/*uh, 16.10.90*/                    ARGUMENT=R_DESC(*(T_PTD)ARGUMENT,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(ARGUMENT)) {
                                       PUSH_T (KLAA);
/*uh, 16.10.90*/                       code = (int*)UNDECIDED;
/*uh, 16.10.90*/                       goto loop;
/*uh, 16.10.90*/                    }
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  code = (int*)FAIL;
/*uh, 16.10.90*/                  goto loop;

/*uh, 16.10.90*/  case I_MATCHARBS: /* matcharb fuer strings, neu von rs 1995 */
/*uh, 16.10.90*/                  FUNC3("matcharbs");
/*uh, 16.10.90*/                  LEN = *code++;
/*uh, 16.10.90*/                  FAIL = *code++;
/*uh, 16.10.90*/                  UNDECIDED = *code++;
/*uh, 16.10.90*/                  ARGUMENT=TOP_W();
/*uh, 16.10.90*/                  if (T_POINTER(ARGUMENT)) {
/*uh, 16.10.90*/                    /* Argument ist Pointer ... */
/*uh, 16.10.90*/                    if ((R_DESC(*(T_PTD)ARGUMENT,class)== C_LIST) &&
                                        (R_DESC(*(T_PTD)ARGUMENT,type)== TY_STRING) &&
/*uh, 16.10.90*/                        (R_LIST(*(T_PTD)ARGUMENT,dim) >= LEN)) goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                    /* ... aber keine Liste */
/*uh, 16.10.90*/                    ARGUMENT=R_DESC(*(T_PTD)ARGUMENT,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(ARGUMENT)) {
                                       PUSH_T (KLAA);
/*uh, 16.10.90*/                       code = (int*)UNDECIDED;
/*uh, 16.10.90*/                       goto loop;
/*uh, 16.10.90*/                    }
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  code = (int*)FAIL;
/*uh, 16.10.90*/                  goto loop;

/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MATCHLIST: /* len, FAIL, UNDECIDED ( W x -- x ) */
/*uh, 16.10.90*/  /* Prueft, ob das oberste Element des Work-Stacks X eine Liste mit der Stelligkeit */
/*uh, 16.10.90*/  /* LEN ist. */
/*uh, 16.10.90*/  /* Ist dies der Fall, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/  /* unmittelbar auf MATCHLIST folgt. */
/*uh, 16.10.90*/  /* Sonst wird die Ausfuehrung an der Adresse FAIL fortgesetzt. */
/*uh, 16.10.90*/  /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/  /* UNDECIDED fortgesetzt.  */
/*uh, 16.10.90*/                  FUNC3("matchlist");
/*uh, 16.10.90*/                  LEN = *code++;
/*uh, 16.10.90*/                  FAIL = *code++;
/*uh, 16.10.90*/                  UNDECIDED = *code++;
/*uh, 16.10.90*/                  ARGUMENT=TOP_W();
/*uh, 16.10.90*/                  if (T_POINTER(ARGUMENT)) {
/*uh, 16.10.90*/                    /* Argument ist Pointer ... */
/*uh, 16.10.90*/                    if (R_DESC(*(T_PTD)ARGUMENT,class)== C_LIST &&
                                        (R_DESC(*(T_PTD)ARGUMENT,type)== TY_UNDEF) &&
/*uh, 16.10.90*/                        R_LIST(*(T_PTD)ARGUMENT,dim) == LEN) goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                    /* ... aber keine Liste */
/*uh, 16.10.90*/                    ARGUMENT=R_DESC(*(T_PTD)ARGUMENT,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(ARGUMENT)) {
                                       PUSH_T (KLAA);
/*uh, 16.10.90*/                       code = (int*)UNDECIDED;
/*uh, 16.10.90*/                       goto loop;
/*uh, 16.10.90*/                    }
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  code = (int*)FAIL;
/*uh, 16.10.90*/                  goto loop;

/*uh, 16.10.90*/  case I_MATCHSTR: /* matching auf strings, neu von rs */
/*uh, 16.10.90*/                  FUNC3("matchstr");
/*uh, 16.10.90*/                  LEN = *code++;
/*uh, 16.10.90*/                  FAIL = *code++;
/*uh, 16.10.90*/                  UNDECIDED = *code++;
/*uh, 16.10.90*/                  ARGUMENT=TOP_W();
/*uh, 16.10.90*/                  if (T_POINTER(ARGUMENT)) {
/*uh, 16.10.90*/                    /* Argument ist Pointer ... */
/*uh, 16.10.90*/                    if ((R_DESC(*(T_PTD)ARGUMENT,class)== C_LIST) &&
                                        (R_DESC(*(T_PTD)ARGUMENT,type)== TY_STRING) &&
/*uh, 16.10.90*/                        R_LIST(*(T_PTD)ARGUMENT,dim) == LEN) goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/                    /* ... aber keine Liste */
/*uh, 16.10.90*/                    ARGUMENT=R_DESC(*(T_PTD)ARGUMENT,class);
/*uh, 16.10.90*/                    if (IS_VAR_OR_CLOS_DESC(ARGUMENT)) {
                                       PUSH_T (KLAA);
/*uh, 16.10.90*/                       code = (int*)UNDECIDED;
/*uh, 16.10.90*/                       goto loop;
/*uh, 16.10.90*/                    }
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  code = (int*)FAIL;
/*uh, 16.10.90*/                  goto loop;

/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MKCASE:  {
/*uh, 16.10.90*/         FUNCX("mkcase");
/*uh, 16.10.90*/         /* Ein CASE-Konstrukt soll geschlossen werden. */
/*uh, 16.10.90*/         /* Dh. es soll ein Paket geschnuert werden, dass neben der */
/*uh, 16.10.90*/         /* Struktur des Ergebnisses auch die aktuellen Werte der */
/*uh, 16.10.90*/         /* relativ freien Variablen enthaelt. */
/*uh, 16.10.90*/
/*uh, 16.10.90*/         /* Zunaechst die Struktur des Ergebnisausdrucks aufbauen */
/*uh, 16.10.90*/
                         DBUG_PRINT ("INTER", ("MKCASE occured !"));

                                      ptdesc = (T_PTD)*code++;
                                      if ((SIZEOFSTACK(S_tilde)>0)&& (T_KLAA(TOP_T()))) {
                                        PPOP_T();
                                        PUSH_W(KLAA);
                                        goto L_undecided; }
                                      else if (T_HASH(TOP_W())) {
                                        PPOP_W();
                                        goto L_undecided; }
                                      goto L_real_nomatch;
                                         
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_TGUARD:   /* FAIL UNDECIDED ( W: value -- ) */

/* das war hier einmal JBOOL, jetzt ist es TGUARD ! */   /* RS 1.3.1995 */
/* Argumente: fail-Label, Original-Switch-Deskriptor, Anzahl PM-bound-variables, Groesse W-Frame,  */
/* Groesse BT-Frame (auf R) zwecks Abraeumung, weil bei undecided direkt nach L_undecided (alter */
/* code) gesprungen wird und nicht etwa zum undecided Label (da wuerden die pm-gebundenen Variablen */
/* freigegeben werden also verloren...nix ruecktrans.! Moment, sollte der Kommentar nicht in */
/* englisch sein ? ;-) */
/* ok, Anzahl PM-bound-variables ist jetzt das letzte Argument wg. der Benutzung von (Interpreter-) lokalen */
/* Variablen...*/

/*uh, 16.10.90*/    /* "JumpBOOL" */
/*uh, 16.10.90*/    /* Testet den obersten Wert VALUE des Work-Stacks. Ist er TRUE, so wird  */
/*uh, 16.10.90*/    /* mit der Ausfuehrung an der Instruktion, die JBOOL unmittelbar folgt, forgefahren.*/
/*uh, 16.10.90*/    /* Ist VALUE FALSE, so wird zur Adresse FAIL gesprungen. */
/*uh, 16.10.90*/    /* Ist VALUE weder FALSE noch TRUE, so wird zur Adresse UNDECIDED gesprungen. */
/*uh, 16.10.90*/                  FUNC0("tguard(no args available)");
/*uh, 16.10.90*/                  FAIL = *code++;
/*uh, 16.10.90*/                  ptdesc = (T_PTD) *code++;
/*uh, 16.10.90*/                  arg = POP_W();
/*uh, 16.10.90*/                  if (IS_TRUE(arg)) { code+=3; goto loop;}
/*uh, 16.10.90*/                  if (IS_FALSE(arg)) {
                                     DBUG_PRINT("inter",("GUARD false..."));
/*uh, 16.10.90*/                     code = (int*)FAIL; }
/*uh, 16.10.90*/                  else {
                                    for (help=(int)(*code++); help>0; help--) {
/*uh, 16.10.90*/                      PPOP_W(); 
/*uh, 16.10.90*/                      }
                                    for (help=(int)(*code++); help>0; help--) {
/*uh, 16.10.90*/                      PPOP_R();
/*uh, 16.10.90*/                      }
                                     PUSH_R(SET_VALUE(DOLLAR,(int)(*code++)));
                                     PUSH_W(arg);
                                     goto L_undecided;
/*uh, 16.10.90*/                     /* code = (int*)UNDECIDED; */ }
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_STARTSUBL: /* level ( W: Startindex ListDesc -- Startindex ListDesc )  */
/*uh, 16.10.90*/    /* "STARTSUBList"  */
/*uh, 16.10.90*/    /* Schreibt den Listendescriptor LISTDESC und den aktuellen Matchpointer STARTINDEX */
/*uh, 16.10.90*/    /* in das Backtrack-Frame an der Stelle LEVEL. */
/*uh, 16.10.90*/                  FUNC1("startsublist");
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  help = TOP_W();
/*uh, 16.10.90*/                  UPDATE_R(arg,help);
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  help = MID_W(1);
/*uh, 16.10.90*/                  UPDATE_R(arg,help);
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_ENDSUBL: /* level ( E: ptr x -- ptr x ) */
/*uh, 16.10.90*/  /* "ENDSUBList" */
/*uh, 16.10.90*/  /*  Traegt das Ende der gematchten Teilliste in das Backtrack-Frame an der Stelle   */
/*uh, 16.10.90*/  /*  LEVEL ein.  Dort ist dann die gesamte Information vorhanden, damit spaeter eine */
/*uh, 16.10.90*/  /*  Bindung an dieses Teilliste gemacht werden kann.  */
/*uh, 16.10.90*/                  FUNC1("endsublist");
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  help = MID_W(1);      /* Zeiger auf noch gematchtes Element */
/*uh, 16.10.90*/                  UPDATE_R(arg,help);
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_BINDSUBL: /*level bindlevel ( E: frames -- frames ) ( I: bindings -- bindings' ) */
/*uh, 16.10.90*/     /* Erzeugt fuer die Teilliste, die im Backtrackframe an der Stelle LEVEL eingetragen */
/*uh, 16.10.90*/     /* ist, einen Listendescriptor und schreibt ihn in das Frame auf dem Inkarnations- */
/*uh, 16.10.90*/     /* Stack an die Stelle BINDLEVEL. Dadurch wird die Variable */
/*uh, 16.10.90*/     /* mit dem Index BINDLEVEL an die gematchte Teilliste gebunden.  */
/*uh, 16.10.90*/                  FUNC2("bindsublist");
/*uh, 16.10.90*/                  ptdesc     = (T_PTD)MID_R(*code++);
/*uh, 16.10.90*/                  (int)func  = (PTR_HEAPELEM)MID_R(*code++) - R_LIST(*ptdesc,ptdv); /* Start */
                                  DBUG_PRINT ("INTER", ("R_LIST(*ptdesc,ptdv) = %x", R_LIST(*ptdesc,ptdv)));
                                  help       = (int) MID_R(*code++);
                                  DBUG_PRINT ("INTER", ("(PTR_HEAPELEM)MID_R(*code++) = %x", help));
/*uh, 16.10.90*/                  help       = (PTR_HEAPELEM) help - R_LIST(*ptdesc,ptdv) - 1; /* ende */
                                  DBUG_PRINT ("INTER", ("limits are: func=%i (start), help=%i (end)", (int)func, (int) help));
/*uh, 16.10.90*/   /* printf("\n\nbindsubl: from=%d; to=%d\n",(int)func,help); */
/*uh, 16.10.90*/                  if (help < (int)func) {
/*uh, 16.10.90*/  /* printf("bindsubl: take nil"); */
                                     if (R_DESC(*ptdesc, type) == TY_STRING) {
                                       ptdesc = _nilstring;
/*uh, 16.10.90*/                       INC_REFCNT(_nilstring); }
                                     else {
/*uh, 16.10.90*/                       ptdesc = _nil;
/*uh, 16.10.90*/                       INC_REFCNT(_nil); }
/*uh, 16.10.90*/                  } else {
/*uh, 16.10.90*/                     ptdesc = (T_PTD)gen_ind((int)func,help,ptdesc);
/*uh, 16.10.90*/  /* printf("bindsubl: take sublist"); */
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  arg = *code++;
/*uh, 16.10.90*/                  UPDATE_A(arg,(STACKELEM)ptdesc);
/*uh, 16.10.90*/                  goto loop;

/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_INITBT: /* level ( E: frames -- ) ( W:  ptr listdesc -- ptr listdesc ) */
/*uh, 16.10.90*/     /* "INITBackTrackframe" */
/*uh, 16.10.90*/     /* Initialisiert das Backtrack-Stackframe der Tiefe LEVEL. */
/*uh, 16.10.90*/     /* PTR zeigt auf den Anfang der noch zu matchenden Teilliste und gibt so */
/*uh, 16.10.90*/     /* die Stelle an, an der der Backtrack das erste Mal wieder aufsetzten muss. */
/*uh, 16.10.90*/     /* LISTDESC ist der Listendescriptor der Liste, in der gerade gematcht wird.   */
/*uh, 16.10.90*/     /* RESTLEN gibt an, wieviele Listenelemente das nun folgende Pattern maximal  */
/*uh, 16.10.90*/     /* ueberdecken kann. Daraus wird das Ende der vom folgenden Pattern zu matchenden  */
/*uh, 16.10.90*/     /* Teilliste berechnet und in das Frame eingetragen. Die Grenzen fuer die   */
/*uh, 16.10.90*/     /* Aufsetzpunkte des Backtracks sind damit bestimmt. */
/*uh, 16.10.90*/                  FUNC3("initbt");
/*uh, 16.10.90*/                  help = *code++;  /* rest */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ptdesc = (T_PTD)MID_W(1);      /* Zeiger auf noch gematchtes Element */
/*uh, 16.10.90*/                  arg = *code++;  /* level1 */
/*uh, 16.10.90*/                  UPDATE_R(arg,ptdesc);   /* Start */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  arg = *code++;  /* level2 */
/*uh, 16.10.90*/                  UPDATE_R(arg,ptdesc);     /* Ptr */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  ptdesc = (T_PTD)TOP_W();
/*uh, 16.10.90*/                  arg = *code++;  /* level3 */
/*uh, 16.10.90*/                  UPDATE_R(arg,(STACKELEM)ptdesc);   /* Descriptor */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  help = (int) ((PTR_HEAPELEM)R_LIST(*ptdesc,ptdv)
/*uh, 16.10.90*/                                +R_LIST(*ptdesc,dim)-help);
/*uh, 16.10.90*/                  arg = *code++;  /* level4 */
/*uh, 16.10.90*/                  UPDATE_R(arg,help);     /* End */
/*uh, 16.10.90*/
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_MATCHIN: /* indesc FAIL-addr UNDECIDED-addr ( W: x -- x ) */
/*uh, 16.10.90*/    /* Prueft, ob das oberste Element des Work-Stacks X unter den Elementen ist, */
/*uh, 16.10.90*/    /* die in der durch den Listendescriptor INDESC beschriebenen Liste sind.  */
/*uh, 16.10.90*/    /* Ist dies der Fall, so wird die Ausfuehrung mit der Instruktion forgesetzt, die */
/*uh, 16.10.90*/    /* unmittelbar auf MATCHIN folgt. */
/*uh, 16.10.90*/    /* Sonst wird die Ausfuehrung an der Adresse FAIL-Addr fortgesetzt. */
/*uh, 16.10.90*/    /* Ist der Vergleich nicht entscheidbar, so wird die Ausfuehrung an der Adresse */
/*uh, 16.10.90*/    /* UNDECIDED-Addr fortgesetzt.  */
/*uh, 16.10.90*/                  FUNC3("matchin");
/*uh, 16.10.90*/                  arg=*code++;  /* In Descriptor is Listendescriptor */
/*uh, 16.10.90*/                  FAIL = *code++;
/*uh, 16.10.90*/                  UNDECIDED= *code++;
/*uh, 16.10.90*/                  ARGUMENT=TOP_W();
/*uh, 16.10.90*/                  /* Test auf undecided durchfuehren */
/*uh, 16.10.90*/                  if (T_POINTER(ARGUMENT) &&
/*uh, 16.10.90*/                      ((R_DESC(*(T_PTD)ARGUMENT,type)==TY_EXPR) ||
/*uh, 16.10.90*/                       (R_DESC(*(T_PTD)ARGUMENT,type)==TY_NAME))) {
                                       PUSH_T (KLAA);
/*uh, 16.10.90*/                       code = (int*)UNDECIDED;
/*uh, 16.10.90*/                       goto loop;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  /* Ergebnis wird True oder False */
                                  if (T_LETTER(ARGUMENT)) {
/*uh, 16.10.90*/                  (PTR_HEAPELEM)ptdesc = R_LIST(*(T_PTD)arg,ptdv);
/*uh, 16.10.90*/                  for (LEN=R_LIST(*(T_PTD)arg,dim); LEN>0; LEN--) {
/*uh, 16.10.90*/                     if ((ARGUMENT&F_VALUE) == (*((PTR_HEAPELEM)ptdesc)++&F_VALUE)) goto loop;
/*uh, 16.10.90*/                  }  } 
/*uh, 16.10.90*/                  code=(int*)FAIL;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_ATEND: /* btlevel hit-label  ( E: frames -- frames ) (W: addr x -- addr x )*/
/*uh, 16.10.90*/    /* Prueft, ob der aktuelle Matchzeiger ADDR groesser ist als das  */
/*uh, 16.10.90*/    /* im FRAME der Tiefe LEVEL vermerkte Ende der Teilliste.   Ist dies der Fall, */
/*uh, 16.10.90*/    /*  so wird zur Adresse HIT-LABEL gesprungen. Ist dies nicht der Fall, so wird */
/*uh, 16.10.90*/    /* die Ausfuehrung mit dem Befehl fortgesetzt, der unmittelbar auf ATEND folgt. */
/*uh, 16.10.90*/                  FUNC2("atstart");
/*uh, 16.10.90*/                  arg=*code++;
/*uh, 16.10.90*/                  if (MID_R(arg)<MID_W(1)) { /* running pointer at start */
/*uh, 16.10.90*/                      code = (int*)*code;
/*uh, 16.10.90*/                      goto loop;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  code++;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_ATSTART: /* btlevel hit-label  ( E: frames -- frames ) (W: addr x -- addr x )*/
/*uh, 16.10.90*/   /*  Prueft, ob der aktuelle Matchzeiger ADDR kleiner ist als der */
/*uh, 16.10.90*/   /*  im FRAME der Tiefe LEVEL vermerkte Start der Teilliste.  Ist dies der Fall, */
/*uh, 16.10.90*/   /*  so wird zur Adresse HIT-LABEL gesprungen. Ist dies nicht der Fall, so wird */
/*uh, 16.10.90*/   /*  die Ausfuehrung mit dem Befehl fortgesetzt, der unmittelbar auf ATSTART folgt. */
/*uh, 16.10.90*/                  FUNC2("atstart");
/*uh, 16.10.90*/                  arg=*code++;
/*uh, 16.10.90*/                  if (MID_R(arg)>MID_W(1)) { /* running pointer at start */
                             /*     help = (int) (((PTR_HEAPELEM)MID_R(arg)) + 1); */
/*uh, 16.10.90*/             /*     UPDATE_R(arg,help);       */
/*uh, 16.10.90*/                      code = (int*)*code;
/*uh, 16.10.90*/                      goto loop;
/*uh, 16.10.90*/                  }
/*uh, 16.10.90*/                  code++;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_FETCH:  /* ( W: heapptr -- val ) */
/*uh, 16.10.90*/   /* Ersetzt den auf dem Work-Stack stehenden Zeiger in den Heap (HEAPPTR) durch  */
/*uh, 16.10.90*/   /* den Wert, der an dieser Stelle im Heap steht.  */
/*uh, 16.10.90*/                  FUNC1("fetch");
/*uh, 16.10.90*/                  arg = TOP_W();
/*uh, 16.10.90*/                  help = (int)*(PTR_HEAPELEM)arg;  /* Zugriff */
/*uh, 16.10.90*/                  if ( (T_PTD)help && T_POINTER(help)) INC_REFCNT((T_PTD)help);
/*uh, 16.10.90*/                  WRITE_W(help);
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/
/*uh, 16.10.90*/  case I_DESC:  /* desc  */
/*uh, 16.10.90*/  /* ueberliest den folgenden Descriptor */
/*uh, 16.10.90*/                  FUNC1("desc");
/*uh, 16.10.90*/                  code++;
/*uh, 16.10.90*/                  goto loop;
/*uh, 16.10.90*/  /* VVVVVVVVVVVVVVVVVVVVVVVVVVVVV alter Code VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV */
/*uh, 16.10.90*/
#endif /* UH_ZF_PM       auskommentiert RS 4.12.1992 */


/******************************************/
/* Die neuen Befehle fuer den Tilde-Stack */
/******************************************/

/* In der 4-stackversion (W A R T) wird vor dem Aufruf einer Funktion der */
/* neue Argumentframe auf dem aktuellen Workspacestack ( W ) aufgebaut    */
/* Analog dazu wird der neue Tildeframe auf dem aktuellen R-stack         */
/* aufgebaut. Mit dem Sprung in die neue Funktion werden dann die Stacks  */
/* W und A sowie T und R ge'switched'                                     */
/* Das Ergebnis einer Funktion liegt vor dem Ruecksprung oben auf dem     */
/* A-Stack.                                                               */

#if WITHTILDE

     case I_PUSH_R:  /* === PUSH_R p : push pointer onto stack r */
		     FUNCX("push_r");
                     INC_REFCNT((T_PTD)(*code));
                     PUSH_R(*code++);
                     goto loop;
    case I_PUSHC_R:  /* === PUSHC_R c : push constant onto stack r */
		     FUNCN("pushc_r");
                     PUSH_R(*code++);
                     goto loop;
    case I_PUSH_TW:  /* === PUSH_TW n : 
			push T-stackentry n onto the w stack === */
                     FUNC1("push_tw");
                     ptdesc = (T_PTD)MID_T(*code);
		     code++;
                     T_INC_REFCNT(ptdesc);
                     PUSH_W(ptdesc);
                     goto loop;
    case I_PUSH_TW0: /* === PUSH_TW0 :
			push T-stackentry 0 onto the a (Workspace) stack === */
                     FUNC0("push_tw0");
                     ptdesc = (T_PTD)TOP_T();
                     T_INC_REFCNT(ptdesc);
                     PUSH_W(ptdesc);
                     goto loop;
    case I_PUSH_TR:  /* === PUSH_TR n : 
			push T-stackentry n onto the r (Return) stack === */
                     FUNC1("push_tr");
                     ptdesc = (T_PTD)MID_T(*code);
		     code++;
                     T_INC_REFCNT(ptdesc);
                     PUSH_R(ptdesc);
                     goto loop;
    case I_PUSH_TR0: /* === PUSH_TR0 :
			push T-stackentry 0 onto the r (Return) stack === */
                     FUNC0("push_tr0");
                     ptdesc = (T_PTD)TOP_T();
                     T_INC_REFCNT(ptdesc);
                     PUSH_R(ptdesc);
                     goto loop;
    case I_PUSH_AR:  /* === PUSH_AR n : 
			push A-stackentry n onto the r (Return) stack === */
                     FUNC1("push_ar");
                     ptdesc = (T_PTD)MID_A(*code);
		     code++;
                     T_INC_REFCNT(ptdesc);
                     PUSH_R(ptdesc);
                     goto loop;
    case I_PUSH_AR0: /* === PUSH_AR0 :
			push A-stackentry 0 onto the r (Return) stack === */
                     FUNC0("push_ar0");
                     ptdesc = (T_PTD)TOP_A();
                     T_INC_REFCNT(ptdesc);
                     PUSH_R(ptdesc);
                     goto loop;

    /* push_aw wird als pusharg weiter oben behandelt */

    case I_MOVE_TW:  /* === MOVE_TW i : move one item from T to W === */
                     FUNC0("move_tw");
                     PUSH_W(POP_T());
                     goto loop;
    case I_MOVE_TR:  /* === MOVE_TR i : move one item from T to R === */
                     FUNC0("move_tr");
                     PUSH_R(POP_T());
                     goto loop;
    case I_MOVE_AR:  /* === MOVE_AR i : move one item from A to R === */
                     FUNC0("move_ar");
                     PUSH_R(POP_A());
                     goto loop;
    case I_MOVE_WR:  /* === MOVE_WR i : move one item from W to R === */
		     FUNC0("move_wr");
		     PUSH_R(POP_W());
		     goto loop;

    /* move_aw wird als move weiter oben behandelt */

    case I_FREE_R:     /* === FREE_R n : free an n-size tilde frame === */
                     FUNC1("free_r");
                     arg = *code++;
                     /* arg: anzahl der von r freizugebenden argumente (>0!) */
                     do {
                       ptdesc = (T_PTD)POP_R();
                       T_DEC_REFCNT(ptdesc);
                     } while (--arg > 0);
                     goto loop;
    case I_FREE1_R:  /* === FREE1_R : free one argument === */
                     FUNC0("free1_r");
                     ptdesc = (T_PTD)POP_R();
                     T_DEC_REFCNT(ptdesc);
                     goto loop;
    case I_RTM_T:    /* === RTM_T : move conditionally from a to i and return ===  */
                     /* mah 171193 nicht mehr conditionally, da kein tail_t-flag   */
                     FUNC0("rtm_t");
		     PUSH_W(POP_T());
                     code = POP_RET();             /* returnadresse */
		     SWITCHSTACKS_T();
                     goto loop;
    case I_RTT:    /* === RTT : return function value === */
                     FUNC0("rtt");
		     /* Tail-end-rekursionen beim Tildestack werden nicht optimiert */
		     /* Das Ergebnis muss nicht gemoved werden                      */
                     code = POP_RET();             /* returnadresse */
		     SWITCHSTACKS_T();
                     goto loop;
    case I_RTC_T:    /* === RTC_T c : return constant === */
                     FUNCN("rtc_t");
                     ptdesc = (T_PTD)*code;
		     SWITCHSTACKS_T();
                     PUSH_W(ptdesc);
                     code = POP_RET();             /* returnadresse */
                     goto loop;
    case I_FREESW_T: /* === FREESW_T n : pop n element off stack t and switch t,r === */
		     /* used only following a GAMMABETA or GAMMACASE instruction      */
		     FUNC1("freesw_t");
                     arg = *code++;
                     /* arg: anzahl der von t freizugebenden argumente (>0!) */
                     do {
                       ptdesc = (T_PTD)POP_T();
                       T_DEC_REFCNT(ptdesc);
                     } while (--arg > 0);
		     SWITCHSTACKS_T();
                     goto loop;
    case I_FREE1SW_T:/* === FREE1SW_T : pop one element off stack t and switch t,r === */
		     /* used only following a GAMMABETA or GAMMACASE instruction      */
		     FUNC0("free1sw_t");
		     ptdesc = (T_PTD)POP_T();
                     T_DEC_REFCNT(ptdesc);
		     SWITCHSTACKS_T();
                     goto loop;
    case I_GAMMA:    /* === GAMMA f : perform n-ary function call using tilde-stack === */
                     FUNCL("gamma");
                     ptdesc = (T_PTD)*code++;
                     /* ptdesc: funktion (zeiger auf descriptor/code) */
                       SWITCHSTACKS_T();
                       PUSH_R(code);                 /* return address    */
                       code = (INSTR *)ptdesc;       /* subroutine call   */
                       goto loop;
                     /* descriptor holen */
                   L_gammafail:
                     GRAB_DESC(ptdesc);              /* descriptor holen */
                     INC_REFCNT(ptdesc);             /* prepare for push */
                     PUSH_R(ptdesc);                 /* push function    */
                     arg = R_COMB(*ptdesc,nargs);    /* args needed      */
		     nfv = R_COMB(*ptdesc,args);     /* free variables   */
                     if ((arg == 0) && (nfv == 0))
		       goto loop;                    /* keine closure    */
                     arity = arg;                    /* number of args   */
		     gamma_closure = 2;              /* function on R    */
		     COMM0("mkgaclos"); 
                     goto L_mkbclos;
    case I_GAMMABETA:/* GAMMABETA f :
			perform n-ary function call using tilde and workspace stack */
		     FUNCL("gammabeta");
		     ptdesc = (T_PTD)*code++;
		     if (_redcnt >= beta_count) {
                       _redcnt -= beta_count;        /* count reduction   */
                       SWITCHSTACKS_T();
		       SWITCHSTACKS();
                       PUSH_R(code);                 /* return address    */
                       code = (INSTR *)ptdesc;       /* subroutine call   */
                       goto loop;
                     }
		     goto L_gammafail;


    case I_GAMMACASE:/* GAMMACASE c :
			execute closed case */
		     FUNCL("gammacase");
		     ptdesc = (T_PTD)*code++;
		     if (_redcnt >= delta_count) {
		       _redcnt -= delta_count;
		       SWITCHSTACKS_T();
		       PUSH_R(code);
		       SET_TAIL_FLAG();
		       code = (INSTR *)ptdesc;       /* subroutine call   */
		       goto loop;
		     }
		     /* case schliessen */
#if UH_ZF_PM
         /*           post_mortem ("UH_PM: no _redcounting yet !"); it should be possible by now ! */
#endif /* UH_ZF_PM */
#if 1
		     gamma_closure = 2;
		     goto L_casefail;
#else
                     GRAB_DESC(ptdesc);         /* descriptor holen */
                     func = TY_CASE;
                     COMM0("nomatch");
                     goto L_nomatch;
#endif /* 1 */
    case I_MKGACLOS: /* === MKGACLOS n m : make gamma-closure === */
                     FUNC3("mkgaclos");
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
		     nfv   = *code++;       /* relativ freie Variablen */
		     gamma_closure = 1;     /* hole Funktionszeiger von R statt von W */
                     func = TY_COMB;        /* COMB-flag  */
                     goto L_mkclos;
    case I_MKGSCLOS: /* === MKGSCLOS n m : make gammaswitch-closure === */
                     FUNC3("mkgsclos");
                     arity = *code++;       /* vorhandene argumente */
                     arg   = *code++;       /* benoetigte argumente */
		     nfv   = *code++;       /* relativ freie Variablen */
		     gamma_closure = 1;     /* hole Funktionszeiger von R statt von W */
                     func = TY_CASE;        /* CASE-flag  */
                     goto L_mkclos;
    case I_POPFREE_T: /* === POPFREE_T :
			 pop argument n off stack r to free n elements from stack t */
		     FUNC0("popfree_t");
                     {
		       int tofree;
		       
		       tofree = POP_R(); /* number of args to pop from t */
		       while (tofree>0) {
			 ptdesc = (T_PTD)POP_T();
			 T_DEC_REFCNT(ptdesc);
			 tofree--;
		       }
		     }
		     goto loop;
    case I_PUSHRET_T:/* === PUSHRET_T r : push return address and switch t,r === */
                     FUNCL("pushret_t");
		     SWITCHSTACKS_T();
                     PUSH_R(*code++);
                     SET_TAIL_FLAG();
                     goto loop;
/* mah 171193: in der 4stackversion koennen keine gamma-tail-end-rekursionen */
/*             optimiert werden, da moeglicherweise die Return-adresse, die  */
/*             die Tail-bits enthielte, zugedeckt waere von einem neuen      */
/*             Tilde-frame. Also entfallen alle JTAIL-instruktionen          */

#endif /* WITHTILDE */

    default:         sprintf(buf,"inter: invalid instruction (%d)",code[-1]);
                     POST_MORTEM(buf);
  } /* end switch */

#if    DEBUG
/* if (TraceFp) fclose(TraceFp); */
/* TraceFp = Null; */

#if !D_SLAVE

  strcpy(S_e.Name,     "e");  stackname_dr(&S_e);
  strcpy(S_a.Name,     "a");  stackname_dr(&S_a);
  strcpy(S_m1.Name,    "m1"); stackname_dr(&S_m1);
  strcpy(S_m.Name,     "m");  stackname_dr(&S_m);
#if WITHTILDE
  strcpy(S_tilde.Name, "tilde"); stackname_dr(&S_tilde); /* mah 251093 */
#endif

#endif /* !D_SLAVE */
#endif /* DEBUG    auskommentiert RS 4.12.1992 */
  /* terminieren */
  END_MODUL("inter");
#if D_SLAVE
#if (D_SLAVE && D_MESS && D_MCOMMU)
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

  /* stack w: | .. arg_arity .. arg_1 func */
  /* arity: anzahl der argumente auf'm stack (>0!!!) */
  ptdesc = (T_PTD)TOP_W();

  DBUG_PRINT ("INTER", ("descriptor: %d", (int) ptdesc));

  /* ptdesc: (pointer auf) die funktion oben auf w */
  if (T_POINTER((int)ptdesc) && R_DESC(*ptdesc,class) == C_FUNC) {
    /* funktionsdescriptor (COMB,CASE,CLOS,CONDI) auf w */

    DBUG_PRINT ("INTER", (" funktionsdescriptor (COMB,CASE,CLOS,CONDI) auf w"));

    arg = R_UNIV(*ptdesc,nargs);
#if WITHTILDE
    nfv = (R_DESC(*ptdesc,type) == TY_CLOS) ? R_CLOS(*ptdesc,nfv) : R_UNIV(*ptdesc,args);
#endif /* WITHTILDE */
    /* arg: anzahl der benoetigten argumente    */
    /* nfv: anzahl der relativ freien Variablen */
    if (arity == arg) {
      
      DBUG_PRINT ("INTER", (" passend viele argumente (>0!) "));

      /* passend viele argumente (>0!) */
      if (R_DESC(*ptdesc,type) == TY_COMB) {

        DBUG_PRINT ("INTER", ("(R_DESC(*ptdesc,type) == TY_COMB)"));

        PPOP_W();                 /* funktion weg von w */
        DEC_REFCNT(ptdesc);       /* wird nicht frei! */
        ptdesc = (T_PTD)R_COMB(*ptdesc,ptc); /* pc holen */
        COMM0("beta");
        goto L_beta;
      } /* end TY_COMB */
      else
      if (R_DESC(*ptdesc,type) == TY_CASE) {

        DBUG_PRINT ("INTER", ("(R_DESC(*ptdesc,type) == TY_CASE)"));
        PPOP_W();                 /* case weg von w */
        DEC_REFCNT(ptdesc);       /* wird nicht frei! */
        ptdesc = (T_PTD)R_CASE(*ptdesc,ptc); /* pc holen */
        COMM0("case");
        goto L_case;
      }
      else
      if (R_DESC(*ptdesc,type) == TY_CLOS) {

        DBUG_PRINT ("INTER", ("(R_DESC(*ptdesc,type) == TY_CLOS)"));

        PPOP_W();                 /* closure weg von w */
        /* ptdesc: zeiger auf die closure (weg von w) */
        /* arity: anzahl der argumente auf'm stack (>0!) */
        arg = R_CLOS(*ptdesc,args);
#if WITHTILDE
	nfv = R_CLOS(*ptdesc,nfv);
#if 0
	PUSH_R(nfv); /* fuer popfree_t */
#endif
#endif /* WITHTILDE */

        /* arg: anzahl der argumente der closure (>0!) */
	/*      bei WITHTILDE >=0                      */
	/* nfv: anzahl der relativ freien variablen der closure (>=0) */

        if (R_CLOS(*ptdesc,ftype) == TY_COMB) {
          /* geschlossener combinator, einlagern */
          register PTR_HEAPELEM pth;

          DBUG_PRINT ("INTER", ("(R_CLOS(*ptdesc,ftype) == TY_COMB)"));

          COMM0("beta clos");
          RES_HEAP;
#if WITHTILDE
	  pth = R_CLOS(*ptdesc,pta) + arg + nfv;
#else
          pth = R_CLOS(*ptdesc,pta) + arg;
#endif /* WITHTILDE */
          help = (int)ptdesc;              /* closure */

#if WITHTILDE
	  {
	    int dummy = nfv;

	    while (dummy-- > 0) { /* tilde frame */
	      ptdesc = (T_PTD)*pth--;
	      T_INC_REFCNT(ptdesc);
	      PUSH_T(ptdesc);
	    }
	  }
	  while (arg-- > 0) { /* argument frame */
	    ptdesc = (T_PTD)*pth--;
	    T_INC_REFCNT(ptdesc);
	    PUSH_W(ptdesc);
	  }	    
#else
          do {
            ptdesc = (T_PTD)*pth--;
            T_INC_REFCNT(ptdesc);
            PUSH_W(ptdesc);
          } while (--arg > 0);
#endif /* WITHTILDE */

          ptdesc = (T_PTD)*pth;              /* funktion */ 
          REL_HEAP;

          DBUG_PRINT ("INTER", ("refcnt: %d", (((T_PTD)help)->u.sc.ref_count)));

          DEC_REFCNT((T_PTD)help);         /* closure */ 

          DBUG_PRINT ("INTER", ("refcnt: %d", (((T_PTD)help)->u.sc.ref_count)));

          COMB_CODE(ptdesc);      /* zeiger auf code holen */
#if WITHTILDE
	  if (nfv > 0) {
	    PUSH_R(code);
	    PUSH_R(nfv);        /* fuer popfree_t */
	    code = rout_popft;
	  }
#endif /* WITHTILDE */

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
	  /* mah 291293:                                 */
	  /* delta closures tragen keine tildevariablen! */
	  /* also kein popfree_t noetig                  */
          pth = R_CLOS(*ptdesc,pta) + arg;
          help = (int)ptdesc;              /* closure */
          do {
            ptdesc = (T_PTD)*pth--;
            T_INC_REFCNT(ptdesc);
            PUSH_W(ptdesc);
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
          help = POP_W();                 /* praedikat */
          if (T_BOOLEAN(help)) {
            /* conditional ausfuehrbar, argumente einlagern */
            register PTR_HEAPELEM pth;

            DBUG_PRINT ("INTER", ("conditional ausfuehrbar, argumente einlagern"));

            arity = (int)ptdesc;           /* closure   */
            RES_HEAP;
#if WITHTILDE
	    pth = R_CLOS(*ptdesc,pta) + arg + nfv;
	    {
	      int dummy=nfv;

	      while (dummy > 0) { /* tilde frame */
		ptdesc = (T_PTD)*pth--;
		T_INC_REFCNT(ptdesc);
		PUSH_T(ptdesc);
		dummy--;
	      }
	    }
	    while (arg > 0) { /* argument frame */
	      ptdesc = (T_PTD)*pth--;
              T_INC_REFCNT(ptdesc);
              PUSH_A(ptdesc);
	      arg--;
	    }
#else
            pth = R_CLOS(*ptdesc,pta) + arg;
            do {
              ptdesc = (T_PTD)*pth--;
              T_INC_REFCNT(ptdesc);
              PUSH_A(ptdesc);
            } while (--arg > 0);
#endif /* WITHTILDE */
            ptdesc = (T_PTD)*pth;       /* conditional       */
            REL_HEAP;
            PUSH_W(help);             /* praedikat */
            DEC_REFCNT((T_PTD)arity); /* closure freigeben */
            PUSH_R(code);             /* returnadresse     */
#if !WITHTILDE
            SET_TAIL_FLAG();          /* SWITCH vermeiden  */
#endif /* !WITHTILDE */
#if WITHTILDE
	    if (nfv > 0) {
	      PUSH_R(nfv);        
	      PUSH_R(rout_popft);       /* POPFREE_T */
	    }
	    SET_TAIL_FLAG();          /* SWITCH vermeiden  */
#endif /* WITHTILDE */
            code = R_CONDI(*ptdesc,ptc);
            goto loop;
          }
          else {

            DBUG_PRINT ("INTER", ("conditional nicht ausfuehrbar, schliessen"));

            /* conditional nicht ausfuehrbar, schliessen */
            /* ptdesc: zeiger auf die closure (weg von a) */
            /* arg: anzahl der argumente der closure */
            /* help: praedikat (liegt nicht mehr auf w) */
            if (R_DESC(*ptdesc,ref_count) == 1) {

              DBUG_PRINT ("INTER", ("closure mit refcnt 1 wird erweitert"));

              /* closure mit refcnt 1 wird erweitert */
              COMM0("expand clos");
              L_CLOS(*ptdesc,nargs) = 0;
              L_CLOS(*ptdesc,args)  = ++arg;
              RES_HEAP;
#if WITHTILDE
	      L_CLOS(*ptdesc,pta)[arg + nfv] = help; /* besser ganz hinten an */
#else
              L_CLOS(*ptdesc,pta)[arg] = help; /* pred */
#endif /* WITHTILDE */
              REL_HEAP;
              PPUSH_W(ptdesc);             /* closure */
              goto loop;
            }
            else {

              DBUG_PRINT ("INTER", ("neue, erweiterte closure bilden"));

              /* neue, erweiterte closure bilden */
              /* ptdesc: zeiger auf die closure (weg von w) */
              /* arg: anzahl der argumente der closure */
              /* help: praedikat (weg von a) */
              COMM0("new expanded clos");
              func = (int)ptdesc;
              MAKEDESC(ptdesc,1,C_FUNC,TY_CLOS);
#if 1
	      /* bug fix: mah 050193                                     */
              /* dies ist ein zusaetzliches argument zu den vorhandenen  */
	      /* nicht zu irgendwas (args ist nicht vorinitialisiert)    */
	      L_CLOS(*ptdesc,args) = R_CLOS(*((T_PTD)func),args) + 1;
#else
              L_CLOS(*ptdesc,args) += 1;   /* zusaetzliches argument  */
#endif /* 1 */

#if WITHTILDE
	      L_CLOS(*ptdesc,nfv)   = nfv; /* relativ freie variablen */
#endif /* WITHTILDE */
              L_CLOS(*ptdesc,nargs) = 0;   /* braucht keine args mehr */
              L_CLOS(*ptdesc,ftype) = TY_CONDI;
#if (D_SLAVE && D_MESS && D_MHEAP)
#if WITHTILDE
              /* auch relativ freie variablen brauchen platz zum leben */
              if ((*d_m_newheap)(arg+2+nfv,A_CLOS(*ptdesc,pta))) {
                int lauf; /* zaehler fuer die kopierschleife */
#else
              if ((*d_m_newheap)(arg+2,A_CLOS(*ptdesc,pta))) {
#endif /* WITHTILDE */
#else
#if WITHTILDE
	      /* auch relativ freie variablen brauchen platz zum leben */
	      if (newheap(arg+2+nfv,A_CLOS(*ptdesc,pta))) {
		int lauf; /* zaehler fuer die kopierschleife */
#else
              if (newheap(arg+2,A_CLOS(*ptdesc,pta))) {
#endif /* WITHTILDE */
#endif
                register PTR_HEAPELEM p1,p2;
                RES_HEAP;
                p1 = (PTR_HEAPELEM)R_CLOS(*(T_PTD)func,pta);
                p2 = (PTR_HEAPELEM)R_CLOS(*ptdesc,pta);
#if WITHTILDE
		for (lauf = arg+nfv; lauf >= 0; lauf--) {
#else
                for ( ; arg-- >= 0 ; ) {
#endif /* WITHTILDE */
                  T_INC_REFCNT((T_PTD)*p1);
                  *p2++ = *p1++;
                }
                *p2 = (T_HEAPELEM)help;  /* praedikat */
                REL_HEAP;
                DEC_REFCNT((T_PTD)func); /* alte clos */
                PPUSH_W(ptdesc);         /* closure   */
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
          /* arity: anzahl der argumente auf'm stack  */
          /* arg: anzahl der argumente der closure    */
	  /* nfv: anzahl der relativ freien variablen */
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
#if WITHTILDE
/* #if UH_ZF_PM */
      /*    arity = R_CASE(*ptdesc,nargs) - R_SWITCH(*(T_PTD)R_CLAUSE(*(T_PTD)R_SELECTION(*R_CASE(*ptdesc,ptd),clauses),sons)[3],anz_args); */
/* #else */
          arity = R_CASE(*ptdesc,nargs) - R_SWITCH(*R_CASE(*ptdesc,ptd),anz_args);
/* #endif */ /* UH_ZF_PM */
#else
          arity = R_CASE(*ptdesc,nargs) - R_SWITCH(*R_CASE(*ptdesc,ptd),case_type);
#endif /* WITHTILDE */
          /* anzahl der durch SRET freizugebenden argumente nach case auf e pushen */
#if WITHTILDE
	  pth += arg + nfv;

	  {
	    int dummy=nfv;

	    while (dummy > 0) { /* tilde frame */
	      ptdesc = (T_PTD)*pth--;
	      T_INC_REFCNT(ptdesc);
	      PUSH_T(ptdesc);
	      dummy--;
	    }
	  }
	  while (arg > 0) { /* argument frame */
	    ptdesc = (T_PTD)*pth--;
	    T_INC_REFCNT(ptdesc);
	    PUSH_W(ptdesc);
	    arg--;
	  }
	  arity = 0;

	  if (nfv > 0) {
	    PUSH_R(nfv);
	    PUSH_R(rout_popft);
	  }
#else
          PUSH_E(arity);
          /* substitute fuer freie variablen auf a */
          for (pth += arg ; --arity >= 0 ; arg--) {
            ptdesc = (T_PTD)*pth--;
            T_INC_REFCNT(ptdesc);
            PUSH_A(ptdesc);
          }
          /* und nun argument(e) auf w */
          for ( ; --arg >= 0 ; ) {
            ptdesc = (T_PTD)*pth--;
            T_INC_REFCNT(ptdesc);
            PUSH_W(ptdesc);
          }
#endif /* WITHTILDE */
/* dg 09.09.92 --- her */
          ptdesc = (T_PTD)*pth;       /* case */
          REL_HEAP;
          DEC_REFCNT((T_PTD)func);    /* closure freigeben */
#if !WITHTILDE
          PUSH_R(rout_sret);          /* special return */
#endif /* !WITHTILDE */
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
        /* ptdesc: zeiger auf's conditional (auf w) */
        /* arity: anzahl der argumente auf'm stack (>0!) */
        /* arg: anzahl der benoetigten argumente (=arity) */
        COMM0("cond");

        help = MID_W(arg);           /* praedikat  */
        if (T_BOOLEAN(help)) {
          /* argumente auf a schaufeln */
          for (; --arg > 0 ;)
            PUSH_A(MID_W(arg));
          PPOP_W();                  /* conditional */
          DEC_REFCNT(ptdesc);        /* wird nicht frei! */
          /* argumente von w poppen */
          for (; --arity > 0 ;)
            PPOP_W();
          /* vorsicht hier: falls der then/else-code die */
          /* argumente nicht freigibt muss das anschliessend */
          /* geschehen !!! */ 
#if WITHTILDE
	  if (R_CONDI(*ptdesc,args) & 0x3) { /* nur die unteren 2 bit */
#else
          if (R_CONDI(*ptdesc,args)) {
#endif /* WITHTILDE */
            /* conditional gibt argumente nicht frei! */
#if D_SLAVE
            post_mortem ("rout_sret ? NOT ALLOWED ! (E Stack used...)");
#endif
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
          /* stack w: | .. arg_arity .. arg_1 clos */
          /* ptdesc: zeiger auf die closure auf'm stack */
          /* arity: anzahl der argumente auf'm stack (>0!) */
          /* arg: anzahl der benoetigten argumente (>1!) */
          register PTR_HEAPELEM pth;

          DBUG_PRINT ("INTER", ("closure mit refcount 1 wird erweitert"));

          COMM0("expand clos");
          RES_HEAP;
#if WITHTILDE
	  {
	    /* erst den tilde frame in sicherheit bringen */

	    pth = (PTR_HEAPELEM)R_CLOS(*ptdesc,pta);
	    pth += R_CLOS(*ptdesc,args);
	    pth += R_CLOS(*ptdesc,nfv);

	    for (arg = R_CLOS(*ptdesc,nfv) ; arg >= 0 ; arg--, pth--) {
	      *(pth + arity) = *pth;
	    }
	  }
#endif /* WITHTILDE */
          pth = (PTR_HEAPELEM)R_CLOS(*ptdesc,pta);
          pth += R_CLOS(*ptdesc,args);
          for (arg = arity ; --arg >= 0 ; ) {
            PPOP_W();
            *++pth = TOP_W();
          }
          REL_HEAP;
          L_CLOS(*ptdesc,args) += arity;  /* mehr args */
          L_CLOS(*ptdesc,nargs) -= arity; /* braucht weniger */
          WRITE_W(ptdesc);                /* die closure */
          goto loop;
        }
        else {

          DBUG_PRINT ("INTER", ("neue, erweiterte closure bilden"));

          /* neue, erweiterte closure bilden */
          COMM0("new clos");
          help = R_CLOS(*ptdesc,args);
          /* stack w: | .. arg_arity .. arg_1 clos */
          /* ptdesc: zeiger auf die closure auf'm stack */
          /* arity: anzahl der argumente auf'm stack */
          /* arg: anzahl der benoetigten argumente */
          /* help: anzahl der closure argumente */
          func = (int)ptdesc;              /* closure */
          MAKEDESC(ptdesc,1,C_FUNC,TY_CLOS);
          L_CLOS(*ptdesc,args) = help+arity; /* mehr args */
#if WITHTILDE
	  nfv = R_CLOS(*(T_PTD)func,nfv);
	  L_CLOS(*ptdesc,nfv) = nfv;
#endif /* WITHTILDE */
          L_CLOS(*ptdesc,nargs) = arg-arity;/* weniger nargs */
          L_CLOS(*ptdesc,ftype) = R_CLOS(*(T_PTD)func,ftype);
          arg += help + 1;      /* laenge der arg.liste */
#if (D_SLAVE && D_MESS && D_MHEAP)
          if ((*d_m_newheap)(arg,A_CLOS(*ptdesc,pta))) {
#else
#if WITHTILDE
	  if (newheap(arg+nfv,A_CLOS(*ptdesc,pta))) {
#else
          if (newheap(arg,A_CLOS(*ptdesc,pta))) {
#endif /* WITHTILDE */
#endif
            register PTR_HEAPELEM p1,p2;
            RES_HEAP;
            p1 = (PTR_HEAPELEM)R_CLOS(*(T_PTD)func,pta);
            p2 = (PTR_HEAPELEM)R_CLOS(*ptdesc,pta);
            do {                   /* altes argument frame */
              T_INC_REFCNT((T_PTD)*p1);
              *p2++ = *p1++;
            } while (--help >= 0);
            PPOP_W();                    /* alte closure */
            do {                   /* zusaetzliche argumente */
              *p2++ = (T_HEAPELEM)POP_W();
            } while (--arity > 0);
#if WITHTILDE
	    while (nfv-- > 0) {   /* tilde frame */
	      T_INC_REFCNT((T_PTD)*p1);
	      *p2++ = *p1++;
	    }
#endif /* WITHTILDE */
            REL_HEAP;
            DEC_REFCNT((T_PTD)func);     /* alte closure */
            PPUSH_W(ptdesc);             /* neue closure */
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

    /* primitive funktion oben auf w */
    /* arity: anzahl der argumente auf'm stack */
    func = TOP_W();               /* primfunc */ 
    arg = FUNC_ARITY(func);       /* args needed */
    if (arity == arg) {
      /* passend viele argumente */
      COMM0("delta");
      PPOP_W();                   /* primfunc */
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
      COMM0("mkdclos");
      goto L_mkdclos;             /* delta-closure bilden  */
    } /* end arity < arg */
    /* sonst zuviele argumente, behandlung unten */
  } /* end T_FUNC */
  else {
    /* sonstige pointer oder stackelemente oben auf w */

    DBUG_PRINT ("INTER", ("goto L_mkap;  applikation auslagern"));

    COMM0("mkap");
    goto L_mkap;                  /* applikation auslagern */
  }
  /* sonst arity > arg d.h. zuviele argumente              */

#if WITHTILDE
#if 0
  if ((arg == 0) && (nfv == 0)) {
    PUSH_R(0);
    goto L_mkgclos;
  }
#else
  if (arg == 0) {
    nfv = 0;
    goto L_mkgclos;
  }
#endif /* 0 */
#else
  if (arg == 0) goto L_mkgclos;   /* universelle closure   */
#endif

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
/*#if D_SLAVE
  post_mortem("interrupt in primitive function");
#endif*/
  END_MODUL("?");                           /* welches ist ja nicht bekannt */
#if DEBUG
  if (_heap_reserved) REL_HEAP;
#endif
  /* evtl. halb erstellte descriptoren freigeben */
  if (_desc != NULL) DEC_REFCNT(_desc);
  _interrupt = FALSE;
  _digit_recycling = TRUE;
  /* wiederherstellen der applikation (func enthaelt die funktion) */
/*  switch (FUNC_ARITY(func)) {
    case 4: PPUSH_W(help);
    case 3: PPUSH_W(arity);
    case 2: PPUSH_W(arg);
    case 1: PUSH_W(func);
            arity = FUNC_ARITY(func);
            break;
  } *//* end switch */
      PUSH_W(func);
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
    *pth-- = (T_HEAPELEM)POP_W();
  } while (--arity >= 0);
  REL_HEAP;
  PPUSH_W(ptdesc);
  goto loop;

} /* end inter */


#if !WITHTILDE
/* --- load_pmlist : load pm argument list --- */

static void load_pmlist(st,p)
#if D_SLAVE
DStackDesc *st;
#else
StackDesc *st;
#endif

T_PTD     p;
{
  int i;
  for (i = 0 ; i < (int) R_LIST(*p,dim); i++ ) {
            /* int gecastet von RS 5.11.1992 */ 
#if D_SLAVE
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
#if D_SLAVE
DStackDesc *st;
#else
StackDesc *st;
#endif

T_PTD     p;
{
  int i;
  for (i = R_LIST(*p,dim); --i >= 0; ) {
#if D_SLAVE
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

#endif /* !WITHTILDE */
