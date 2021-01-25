/* $Log: rear.c,v $
 * Revision 1.105  1996/06/24  13:33:37  cr
 * generate apply instruction for precompiled functions, too
 *
 * Revision 1.104  1996/06/14  14:04:50  rs
 * the (famous) F1-problem with some programs should be fixed
 * (there was a problem with unary letp's)
 *
 * Revision 1.103  1996/06/13  13:11:29  rs
 * building closures for mkcase and tguard must not use targs
 *
 * Revision 1.102  1996/06/11  15:46:29  rs
 * one line had to be moved... ;-)
 *
 * Revision 1.101  1996/04/19  12:53:29  rs
 * temp_targs inserted (changing targs in "case TY_LREC_IND"
 * caused some side FX errrrr effects !)
 *
 * Revision 1.100  1996/04/04  16:10:20  rs
 * more DBUG output
 *
 * Revision 1.99  1996/04/03  16:50:34  rs
 * some DBUG-messages added
 *
 * Revision 1.98  1996/03/29  14:35:23  rs
 * some work on the famous MAH-tilde-BUG
 *
 * Revision 1.97  1996/03/28  14:37:27  rs
 * #if TARG_FIX changed to #if (TARG_FIX && WITHTILDE)
 *
 * Revision 1.96  1996/03/26  15:24:20  cr
 * size of tilde frame (targs) wasn't properly maintained
 * causing 'side-effects' for everything between two SNSUBs
 * (look for TARG_FIX)
 *
 * Revision 1.95  1996/03/25  14:39:22  cr
 * #ifdef CAF_UPDATE : if the result of a beta call does not
 * depend on any environment (beta(0) and no tilde-frame), an
 * I_UPDATE-instruction is inserted after the I_BETA to update
 * the original function code with I_RTP result (I_RTC result).
 *
 * Revision 1.94  1996/03/21  13:09:08  rs
 * TAG_INT at some CASE-ptcs inserted (bug-fixed)
 *
 * Revision 1.93  1996/03/07  18:16:17  rs
 * C_FUNC TY_CASE ptc's might be used to indicate the number of the when-clause
 * (bugfix for the distributed version)
 *
 * Revision 1.92  1996/02/15  15:38:24  cr
 * ASM-instruction mkslot : 1 -> 0 parameters
 *
 * Revision 1.91  1996/02/08  17:14:31  stt
 * latest bugfix for 0-ary interactions removed because
 * they are no longer put into descriptors. (intact(ia))
 *
 * Revision 1.90  1996/02/06  19:23:07  car
 * fixed distb output
 *
 * Revision 1.89  1996/01/08  15:36:59  stt
 * error with 0-ary interactions fixed
 *
 * Revision 1.88  1995/11/15  15:07:49  cr
 * introduced ASM-instructions to build frames (I_MKFRAME) and slots (I_MKSLOT)
 *
 * Revision 1.87  1995/11/06  17:07:42  stt
 * filepointers are set to NULL after closing
 *
 * Revision 1.86  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.85  1995/08/30  14:04:08  rs
 * some tilde + letpar bugfixes
 *
 * Revision 1.84  1995/08/29  14:43:46  rs
 * some more DBUG-output messages
 *
 * Revision 1.83  1995/08/10  14:26:58  rs
 * thanks to car for erasing my debug-messages ;-(
 *
 * Revision 1.82  1995/08/10  13:27:18  car
 * *** empty log message ***
 *
 * Revision 1.81  1995/08/10  13:24:22  car
 * compiler directives for dist[b]
 *
 * Revision 1.79  1995/07/10  11:51:12  mah
 * incorrect descriptor entries fixed for tilde version
 *
 * Revision 1.78  1995/07/10  09:15:38  car
 * outstyle of dist[b] changed
 *
 * Revision 1.77  1995/07/07  08:29:50  car
 * output DIST
 *
 * Revision 1.76  1995/06/30  19:07:17  mah
 * and again.
 *
 * Revision 1.75  1995/06/23  09:49:41  car
 * compiles again$%#$$$%$^%&^@
 *
 * Revision 1.74  1995/06/22  13:10:38  mah
 * bug fix in tilde version
 * look for lrec_sub
 *
 * Revision 1.73  1995/06/19  13:09:48  rs
 * a small (UH- :-) PM-Bugfix
 *
 * Revision 1.72  1995/05/15  13:40:58  rs
 * problem with I_MATCHIN in string-backtracking solved (hopefully)
 * (added the undecided-label handling and renamed the
 * undecided label to another fail label, because while
 * backtracking undecided equals fail)
 *
 * Revision 1.71  1995/05/12  11:57:24  rs
 * I_MATCHIN str-output added (Extended Codefile)
 *
 * Revision 1.70  1995/04/06  14:25:08  rs
 * UH-PM "in etc. bugfix
 *
 * Revision 1.69  1995/04/05  14:19:03  rs
 * it's still not the final UH-PM bugfix :-( but we're coming closer !
 *
 * Revision 1.68  1995/04/04  14:35:49  rs
 * (nearly) fixed the (ex-:) UH-PM "in "... Bug, hang on !
 *
 * Revision 1.67  1995/04/03  15:37:31  rs
 * minor changes
 *
 * Revision 1.66  1995/03/27  09:19:50  rs
 * old UH-PM relict (COND_FLAG etc., see line 4537) removed
 *
 * Revision 1.65  1995/03/24  12:13:02  sf
 * added tilde arguments to case descriptors in codefile
 *
 * Revision 1.64  1995/03/24  11:04:59  sf
 * bug fixed in output of atend and atstart (new macro GEN_INSTR2LA)
 *
 * Revision 1.63  1995/03/16  14:48:11  rs
 * variable indexing corrected
 *
 * Revision 1.62  1995/03/16  11:02:46  rs
 * minor changes..(UH-PM)
 *
 * Revision 1.61  1995/03/13  09:08:47  car
 * *SUBLIST -> *SUBL renamed
 *
 * Revision 1.60  1995/03/03  10:50:03  rs
 * cosmetic changes for "car"
 *
 * Revision 1.59  1995/03/02  13:19:26  rs
 * minor changes
 *
 * Revision 1.58  1995/03/02  12:46:57  rs
 * no type,string etc. in UH-PM allowed
 *
 * Revision 1.57  1995/03/02  10:29:09  rs
 * makew/btframe -> mkw/btframe
 *
 * Revision 1.56  1995/03/01  16:50:36  rs
 * various bugfixes and new features concerning UH's patternmatching
 *
 * Revision 1.55  1995/02/28  16:59:07  rs
 * DIGIT and SCALAR added
 * (UH-PM)
 *
 * Revision 1.54  1995/02/27  17:22:09  rs
 * some warnings removed...
 *
 * Revision 1.53  1995/02/23  14:24:16  car
 * forcing codefile output
 *
 * Revision 1.52  1995/02/22  15:50:59  rs
 * SELECTION-descriptors used only in preprocessing
 *
 * Revision 1.51  1995/02/14  16:03:19  rs
 * minor changes
 *
 * Revision 1.50  1995/02/08  15:34:37  rs
 * bugfix in single when
 *
 * Revision 1.49  1995/02/07  15:49:07  rs
 * UH String PM implemented
 *
 * Revision 1.48  1995/01/31  14:48:57  rs
 * MKIFRAME -> MKAFRAME
 *
 * Revision 1.47  1995/01/17  15:35:47  rs
 * rear.c                 rs
 * ./rncsched.c             dmk
 * rs@batman (42)> vi pre-casefunc.c
 * rs@batman (43)> ci pre-casefunc.c
 * RCS/pre-casefunc.c,v  <--  pre-casefunc.c
 * new revision: 1.11; previous revision: 1.10
 * enter log message, terminated with single '.' or end of file:
 * >> another UH PM bugfix
 * err....sorry, wrong messages ! :-)
 * (never do copy paste with the mouse... ;-)
 * anyway: another UH PM bugfix, leaving out
 * the PUSH_AW command inserted by the compiler
 *
 * Revision 1.46  1995/01/13  16:41:14  rs
 * uh pm bugfixes
 *
 * Revision 1.45  1995/01/12  15:59:35  rs
 * mksclos + UH PM bugfix
 *
 * Revision 1.44  1995/01/10  16:28:02  rs
 * UH PM bugfixes
 *
 * Revision 1.43  1995/01/09  15:49:49  rs
 * more UH bugfixes
 * err...pattermatch !
 *
 * Revision 1.42  1995/01/06  16:32:11  rs
 * UH patternmatch bugfixes
 *
 * Revision 1.41  1995/01/05  16:22:41  rs
 * some mote UH pm stuff
 *
 * Revision 1.40  1995/01/04  14:48:09  rs
 * another day's work of UH-patternmatch...
 *
 * Revision 1.39  1995/01/03  15:17:11  rs
 * UH PM bugfixes
 *
 * Revision 1.38  1995/01/03  11:13:36  rs
 * more UH PM stuff
 *
 * Revision 1.37  1995/01/02  08:37:19  mah
 * *** empty log message ***
 *
 * Revision 1.36  1994/12/31  13:06:50  mah
 * bug fix in tilde conditional
 *
 * Revision 1.35  1994/12/21  15:01:33  rs
 * more UH pm stuff
 *
 * Revision 1.34  1994/12/02  08:45:17  car
 * DYADIC instruction mkdclos
 *
 * Revision 1.33  1994/11/30  10:15:52  car
 * lsel instruction removed
 * third argument of mkdclos removed
 *
 * Revision 1.32  1994/11/02  10:01:17  rs
 * UH_ZF_PM + Tilde
 *
 * Revision 1.31  1994/11/01  15:54:30  car
 * generate codefile in non-DEBUG-version
 *
 * Revision 1.30  1994/10/24  07:56:06  car
 * added tilde arguments to descriptors in codefile
 *
 * Revision 1.29  1994/10/21  15:12:56  rs
 * *** empty log message ***
 *
 * Revision 1.28  1994/10/21  10:31:39  rs
 * UH_ZF_PM core source added
 *
 * Revision 1.27  1994/10/04  10:51:39  car
 * write codefile in non-debug versions
 *
 * Revision 1.26  1994/06/15  11:23:19  mah
 * both closing strategies in one binary: CloseGoal
 *
 * Revision 1.25  1994/06/14  14:00:27  mah
 * free1sw_t added
 *
 * Revision 1.24  1994/05/24  11:47:04  rs
 * NFREE_R for free'ed tilde arguments
 *
 * Revision 1.23  1994/05/09  12:24:59  rs
 * minor bugfix
 *
 * Revision 1.22  1994/05/06  15:02:49  rs
 * another beta version of picard and tilde
 *
 * Revision 1.21  1994/05/04  07:40:19  rs
 * compile bugfix :-) for WITHTILDE
 *
 * Revision 1.20  1994/05/04  07:27:52  rs
 * beta release for picard and tilde
 *
 * Revision 1.19  1994/05/03  07:33:24  mah
 * *** empty log message ***
 *
 * Revision 1.18  1994/03/29  17:27:35  mah
 * generation of popfree_t instructions removed
 *
 * Revision 1.17  1994/03/08  11:50:15  mah
 * *** empty log message ***
 *
 * Revision 1.16  1994/02/25  16:18:43  rs
 * bugfix for (trivial example)
 * let a=1,b=2 in def f[x]=a in b
 * fixed resulting 1 to correct 2 !!
 * (see function load_body in rear.c for details)
 *
 * Revision 1.14  1994/02/23  15:53:02  mah
 * bug fix in conditional
 * revision 1.13 included
 *
 * Revision 1.12  1994/02/09  13:19:46  mah
 * bug fix in in primfunc handling of l_apply
 *
 * Revision 1.11  1994/01/21  10:31:54  car
 * *** empty log message ***
 *
 * Revision 1.10  1994/01/20  14:41:13  car
 * new parameter to printelements
 *
 * Revision 1.9  1994/01/19  16:33:57  mah
 * extended code file generation added
 *
 * Revision 1.8  1994/01/18  14:40:20  mah
 * POST_SNAP added
 *
 * Revision 1.7  1994/01/18  08:41:55  mah
 * new style push and move instructions
 *
 * Revision 1.6  1993/11/25  14:10:40  mah
 * ps_i, ps_a changed to ps_a, ps_w
 *
 * Revision 1.5  1993/11/18  07:38:08  rs
 * bugfix for IF-THEN-ELSE in GUARD-expr.,
 * removed ver. 1.4 fix,
 * force PUSHRET-insertion in ASM-compilation
 * (no optimization)
 *
 * Revision 1.4  1993/11/12  16:37:36  rs
 * bugfix at line 932
 * (if-then-else in guard)
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
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

#define TARG_FIX 1
  /* attempt to restore old targ-value when leaving a TY_SNSUB */
  /* possible problem: targ needs to be stored on M-stack and  */
  /* may thus cause lookups for other M-stack elements to fail */
  /* e.g. information is stored *below* all AP_1s to avoid     */
  /* problems with collect_args()       cr 26.03.96            */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rinter.h"
#include "rprimf.h"

#if UH_ZF_PM
#include "pmanalyze.h"
extern StackDesc S_tilde;
extern void freeheap();
extern void load_list();
extern void print_info();
#endif /* UH_ZF_PM */

#if WITHTILDE
#include "case.h"

/* FREEING OF TILDE VARIABLES, change to nfree_r */
#if D_DIST
#define NFREE_R 0
#endif

#endif /* WITHTILDE */

#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

#if D_DIST                     /* using Stack S_pm for dist-ASM-Code */
extern StackDesc S_pm;
extern void updatestack();                 /* rstack.c */
extern int d_nodistnr;
#endif

#if UH_ZF_PM
/*uh, 28.1.91*/  extern  pattern_info pmanalyze();                    /* pmanalyze.c */
#endif UH_ZF_PM


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
extern PTR_DESCRIPTOR    newdesc();                 /* rheap.c */
extern void     load_expr();                        /* rhinout.c */
extern void     res_heap(); /* TB, 4.11.1992 */     /* rheap.c */
extern void     rel_heap(); /* TB, 4.11.1992 */     /* rheap.c */
extern void     test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void     test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */   /* rstack,c */
extern STACKELEM midstack();               /* rstack.c */
extern int      ExtendedCodeFile;
extern int      newheap();

extern int      printelements();
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

static FILE *codefp = 0;
       FILE *fopen();
#define POST_MORTEM(mes)      if (codefp) { fclose(codefp); codefp = NULL; post_mortem(mes); } else post_mortem(mes)
  /* geaendert von stt */

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

extern char *stelname();      /* rdesc.c (rinter.h) */

#define GEN_INSTR0(i,nm)       (NEWCODE(i), \
                                fprintf(codefp,"\t %s();\n",nm))
#define GEN_INSTR1(i,nm,a)     (NEWCODE(i), NEWCODE(a), \
                                fprintf(codefp,"\t %s(%d);\n",nm,a))
#define GEN_INSTR2(i,nm,a,b)   (NEWCODE(i), NEWCODE(a), NEWCODE(b), \
                                fprintf(codefp,"\t %s(%d,%d);\n",nm,a,b))
#if WITHTILDE
#define GEN_INSTR3(i,nm,a,b,c) (NEWCODE(i), NEWCODE(a), NEWCODE(b), NEWCODE(c), \
                                fprintf(codefp,"\t %s(%d,%d,%d);\n",nm,a,b,c))
#endif /* WITHTILDE */

#if D_DIST              /* dist and distb-ASM 4 Arguments only */
#define GEN_INSTRD4(i,nm,a,b,c,c2)  (NEWCODE(i), NEWCODE(a), NEWCODE(b), NEWCODE(c), NEWCODE(c2), \
                                  fprintf(codefp,"\t %s(distarg_%02d,distend_%02d,%d,%d);\n",nm,a,b,c,c2))
#if WITHTILDE           /* dist and distb-ASM~ 5 Arguments only */
#define GEN_INSTRD6(i,nm,a,b,c,c2,c3,c4)  (NEWCODE(i), NEWCODE(a), NEWCODE(b), NEWCODE(c), NEWCODE(c2),NEWCODE(c3),NEWCODE(c4), \
                                  fprintf(codefp,"\t %s(distarg_%02d,distend_%02d,%d,%d,%d,%d);\n",nm,a,b,c,c2,c3,c4))
#endif /* WITHTILDE */
#endif /* D_DIST */

#define GEN_INSTRN(i,nm,a)    (NEWCODE(i), NEWCODE(a), \
                               fprintf(codefp,"\t %s(%s);\n",nm,stelname((STACKELEM)a)))     /* STACKELEM von ach 09/11/92 */
#define GEN_INSTRX(i,nm,a)    (NEWCODE(i), NEWCODE(a), \
                               fprintf(codefp,"\t %s(ptd(0x%08x));\n", \
                                 nm,(int)a), \
                               ExtendedCodeFile? printelements(codefp, a, 0) : 0)
                               
#define GEN_INSTRL(i,nm,p,a)  (NEWCODE(i), NEWCODE(a), \
                               fprintf(codefp,"\t %s(%s_%x);\n",nm,p,(unsigned int)a))
#define GEN_DESC(desc,class,type,args,nargs,ptd,prefix,ptc) \
                              (NEWCODE(I_DESC), NEWCODE(desc), \
                               fprintf(codefp,"\t _desc(0x%08x,%s,%s,%d,%d,0x%08x,%s_%x);\n", \
                               (unsigned int)desc,class,type,args,nargs,(unsigned int)ptd,prefix,(unsigned int)ptc))

#define GEN_DESCI(desc,class,type,args,nargs,ptd,prefix,ptc) \
                              (NEWCODE(I_DESC), NEWCODE(desc), \
                               fprintf(codefp,"\t _desc(0x%08x,%s,%s,%d,%d,0x%08x,%s_%02d);\n", \
                               (unsigned int)desc,class,type,args,nargs,(unsigned int)ptd,prefix,(unsigned int)ptc))

#define GEN_LABEL(p,label)    fprintf(codefp,"%s_%x:\n",p,(unsigned int)label)

#define END_OF_BODY()         T_DOLLAR(READSTACK(S_m))
#define NO_MORE_VARIABLES()   T_DOLLAR(READSTACK(S_e)) 

#if WITHTILDE
#define NO_MORE_TILDEVARS()   T_DOLLAR_TILDE(READSTACK(S_e))
#endif /* WITHTILDE */

#if UH_ZF_PM

/* THESE ARE DEBUG MAKROS....!!!!!!!!!!! */

#define GEN_INSTR4(i,nm,a,b,c,deh) \
/*uh, 17.10.90*/            (NEWCODE(i), NEWCODE(a), NEWCODE(b), NEWCODE(c), NEWCODE(deh),\
/*uh, 17.10.90*/            fprintf(codefp,"\t %s(%d,%d,%d,%d);\n",nm,a,b,c,deh))
/*uh, 17.10.90*/
#define GEN_INSTR5(i,nm,a,b,c,deh,e) \
/*uh, 17.10.90*/            (NEWCODE(i), NEWCODE(a), NEWCODE(b), NEWCODE(c), NEWCODE(deh),\
/*uh, 17.10.90*/             NEWCODE(e), \
/*uh, 17.10.90*/            fprintf(codefp,"\t %s(%d,%d,%d,%d,%d);\n",nm,a,b,c,deh,e))

#define GEN_INSTR5_RS(i,nm,l1,a,b,c,deh,e) \
                            (NEWCODE(i), NEWCODE(a), NEWCODE(b), NEWCODE(c), NEWCODE(deh),\
                             NEWCODE(e), \
                            fprintf(codefp,"\t %s(%s_%x,ptd(0x%08x),%d,%d,%d);\n",nm,l1,a,(int)b,(int)c,(int)deh,(int)e))

#define GEN_INSTR3_UH(i,nm,a,n1,l1,n2,l2) \
/*uh, 17.10.90*/            (NEWCODE(i), NEWCODE(a), NEWCODE(l1), NEWCODE(l2),\
/*uh, 17.10.90*/            fprintf(codefp,"\t %s(%d,%s_%x,%s_%x);\n",nm,a,n1,l1,n2,l2))
/* another makro for strname output... */
#define GEN_INSTR3_UHS(i,nm,a,n1,l1,n2,l2) \
/*uh, 17.10.90*/            (NEWCODE(i), NEWCODE(a), NEWCODE(l1), NEWCODE(l2),\
/*uh, 17.10.90*/            fprintf(codefp,"\t %s(%s,%s_%x,%s_%x);\n",nm,stelname((STACKELEM)a),n1,l1,n2,l2))
/*uh, 17.10.90*/
/* special macro for car ruthkamp rathsack */
#define GEN_INSTR3_UHS2(i,nm,a,n1,l1,n2,l2) \
                            (NEWCODE(i), NEWCODE(a), NEWCODE(l1), NEWCODE(l2),\
                            fprintf(codefp,"\t %s(ptd(0x%08x),%s_%x,%s_%x);\n",nm,a,n1,l1,n2,l2))
#define GEN_INSTR1L(i,nm,a,n1,l1) \
/*uh, 17.10.90*/            (NEWCODE(i), NEWCODE(a), NEWCODE(l1),\
/*uh, 17.10.90*/            fprintf(codefp,"\t %s(%d,%s_%x);\n",nm,a,n1,l1))
/*uh, 17.10.90*/
#define GEN_INSTR2LA(i,nm,l1,n2,l2) \
/*uh, 17.10.90*/            (NEWCODE(i), NEWCODE(l1), NEWCODE(l2),\
/*uh, 17.10.90*/            fprintf(codefp,"\t %s(%x,%s_%x);\n",nm,l1,n2,l2))
/*uh, 17.10.90*/
#define GEN_INSTR2L(i,nm,n1,l1,n2,l2) \
/*uh, 17.10.90*/            (NEWCODE(i), NEWCODE(l1), NEWCODE(l2),\
/*uh, 17.10.90*/            fprintf(codefp,"\t %s(%s_%x,%s_%x);\n",nm,n1,l1,n2,l2))
/*uh, 17.10.90*/

#define MARK(desc,n)           MAKEDESC(desc,1,C_FUNC,TY_CONDI); \
/*uh, 17.10.90*/                                 L_CONDI(*desc,ptc) = CODEPTR +(n); \
/*uh, 17.10.90*/                                 L_CONDI(*desc,ptd) = _nil; \
/*uh, 17.10.90*/                                 INC_REFCNT(_nil)
/*uh, 17.10.90*/
/*uh, 17.10.90*/  /* ENCLOSING_LIST erzeugt Code, der die umfassenden Listendescriptor des */
/*uh, 17.10.90*/  /* momentan gerade gematchten Elements auf das oberste Feld des A-Stacks */
/*uh, 17.10.90*/  /* schreibt. (Fuer ... oder last...) */
#define ENCLOSING_LIST \
/*uh, 17.10.90*/        if (actual_depth>1)  {\
/*uh, 17.10.90*/          GEN_INSTR1(I_PICK,"pick",actual_depth+1);\
/*uh, 17.10.90*/          GEN_INSTR0(I_FETCH,"fetch");\
/*uh, 17.10.90*/        } else\
/*uh, 17.10.90*/          GEN_INSTR1(I_PICK,"pick",pattern_depth+2)
/*uh, 17.10.90*/
#define IS_DOTDOTDOT(ptr)  ( (R_DESC(*(T_PTD)ptr,class)==C_PATTERN) && \
/*uh, 17.10.90*/                               (R_PATTERN(*(T_PTD)ptr,type)==TY_DOTDOTDOT))
/*uh, 17.10.90*/
#define IS_LASTDOTDOTDOT(ptr)  ( (R_DESC(*(T_PTD)ptr,class)==C_PATTERN) && \
/*uh, 17.10.90*/                                 (R_PATTERN(*(T_PTD)ptr,type)==TY_LASTDOTDOTDOT))
/*uh, 17.10.90*/
#define IS_DOTPLUS(ptr)  ( (R_DESC(*(T_PTD)ptr,class)==C_PATTERN) && \
/*uh, 17.10.90*/                               (R_PATTERN(*(T_PTD)ptr,type)==TY_DOTPLUS))
/*uh, 17.10.90*/
#define IS_LASTDOTPLUS(ptr)  ( (R_DESC(*(T_PTD)ptr,class)==C_PATTERN) && \
/*uh, 17.10.90*/                                 (R_PATTERN(*(T_PTD)ptr,type)==TY_LASTDOTPLUS))
/*uh, 17.10.90*/
#define IS_DOTSTAR(ptr)  ( (R_DESC(*(T_PTD)ptr,class)==C_PATTERN) && \
/*uh, 17.10.90*/                               (R_PATTERN(*(T_PTD)ptr,type)==TY_DOTSTAR))
/*uh, 17.10.90*/
#define IS_LASTDOTSTAR(ptr)  ( (R_DESC(*(T_PTD)ptr,class)==C_PATTERN) && \
/*uh, 17.10.90*/                                 (R_PATTERN(*(T_PTD)ptr,type)==TY_LASTDOTSTAR))
/*uh, 17.10.90*/
#define IS_LAST(ptr) (IS_LASTDOTDOTDOT(ptr) || IS_LASTDOTPLUS(ptr) || \
/*uh, 17.10.90*/                        IS_LASTDOTSTAR(ptr))
#define IS_DOTS(ptr) (IS_DOTDOTDOT(ptr) || IS_DOTPLUS(ptr) || IS_DOTSTAR(ptr))
/*uh, 17.10.90*/
#define DEPTH_L()  (S_l-Stack_l)
/*uh, 17.10.90*/
#define PUSH_L(x)  if (S_l > Stack_l+SIZE_L){ \
/*uh, 17.10.90*/                          printf("Detected Stacksize=%d\n",S_l-Stack_l);\
/*uh, 17.10.90*/                          printf("File: %s Line: %d\n",__FILE__,__LINE__);\
/*uh, 17.10.90*/                          post_mortem("EAR: Stack overflow on Label-Stack L");\
/*uh, 17.10.90*/                     } else \
/*uh, 17.10.90*/                         (*(++S_l)) = (int)(x)
#define POP_L()    ((S_l <= Stack_l)  \
/*uh, 17.10.90*/                     ? printf("File: %s Line: %d\n",__FILE__,__LINE__),\
/*uh, 17.10.90*/                       post_mortem("EAR: Stack underflow on Label-Stack L")\
/*uh, 17.10.90*/                     : (int)(*(S_l--)))
/*uh, 17.10.90*/
#define READ_L()  (int)(*(S_l))
/*uh, 17.10.90*/
#endif /* UH_ZF_PM */

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
#if WITHTILDE
/* Die oberen 14 Bits von args enthalten die Groesse des Tilde-frames */
#define KEEP_FLAG(x)      ((int)(x & 0x3) % 2)
#define COND_FLAG(x)      ((int)(x & 0x3) / 2)
#else
#define KEEP_FLAG(x)      (x % 2)
#define COND_FLAG(x)      (x / 2)
#endif /* WITHTILDE */

/* generiert instruktionen zur freigabe von argumenten */
#if D_MESS

#define FREE_INSTR(i,nf)  do {if ((i) > 0) { \
                            if ((i) == 1) { \
  if (D_M_C_FREE1_A) \
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE1_A); \
                              GEN_INSTR0(I_FREE1_A,"free1_a"); \
                            } \
                            else { \
  if (D_M_C_FREE_A) \
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE_A); \
                              GEN_INSTR1(I_FREE_A,"free_a",(i)); \
                            } \
                            nf += (i); \
                          }} while(0)
#if WITHTILDE
#define FREE_INSTR_R(i,nf)  do if ((i) > 0) { \
                              if ((i) == 1) { \
  if (D_M_C_FREE1_R) \
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE1_R); \
                                GEN_INSTR0(I_FREE1_R,"free1_r"); \
                              } \
                              else { \
  if (D_M_C_FREE_R) \
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE_A); \
                                GEN_INSTR1(I_FREE_R,"free_r",(i)); \
                              } \
                              nf += (i); \
                            } while(0)
#endif /* WITHTILDE */

#else

#define FREE_INSTR(i,nf)  do if ((i) > 0) { \
                            if ((i) == 1) { \
                              GEN_INSTR0(I_FREE1_A,"free1_a"); \
                            } \
                            else { \
                              GEN_INSTR1(I_FREE_A,"free_a",(i)); \
                            } \
                            nf += (i); \
                          } while(0)
#if WITHTILDE
#define FREE_INSTR_R(i,nf)  do if ((i) > 0) { \
                              if ((i) == 1) { \
                                GEN_INSTR0(I_FREE1_R,"free1_r"); \
                              } \
                              else { \
                                GEN_INSTR1(I_FREE_R,"free_r",(i)); \
                              } \
                              nf += (i); \
                            } while(0)
#endif /* WITHTILDE */
#endif /* D_MESS */

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

#if !WITHTILDE

/*---------------------------------------------------------------------*/
/* size_lrec : wird aufgerufen von load_body (s.u.)                    */
/*             stellt fest, an welchem index der goal-Ausdruck beginnt */
/*---------------------------------------------------------------------*/

int size_lrec (ptr, absolut)
STACKELEM    *ptr;          /* base of array */
int          absolut;       /* absolute index in array */
{
  int a=absolut,i;

  DBUG_ENTER("size_lrec");

  DBUG_PRINT ("RS",("argument absolut=%i", absolut));
  if (T_CON(ptr[absolut])) {
    a++;
    for (i = ARITY(ptr[absolut]); --i>=0; ) {
      a = size_lrec (ptr, a);
      DBUG_PRINT ("RS", ("number %i", a));
      }
    DBUG_RETURN(a);
    }
  else
    DBUG_RETURN(absolut+1);
}

#endif /* !WITHTILDE */

/*---------------------------------------------------------------------*/
/* load_body: (vgl. load_expr in rhinout.c)                            */
/*            kopiert einen heapausdruck in den e-stack. der parameter */
/*            pptr ist ein pointer auf die heapdarstellung, deren      */
/*            laenge im ersten feld steht. die position ab der keine   */
/*            nummernvariablen und conditionals mehr vorkommen wird mit*/
/*            dollar markiert.                                         */
/*---------------------------------------------------------------------*/

#if WITHTILDE
/* Die Laenge des Tildeframes wird nicht extra       */
/* uebergeben, da die Tildestackindizes genau den    */
/* Tildeindizes der einzelnen Variablen entsprechen. */

static void load_body(pptr,margs,nargs,tnargs)
int       tnargs;                       /* freizugebende tilde-argumente */
#else
static void load_body(pptr,margs,nargs)
#endif /* WITHTILDE */

STACKELEM **pptr;                       /* pointer auf heapdarstellung */
int       margs;                        /* gesamtzahl der argumente */
int       nargs;                        /* freizugebende argumente */
{
  register int          size;
  register STACKELEM       x;
  register STACKELEM    *ptr;
           int          index;
#if !WITHTILDE
/*RS*/     int          i,k=0;
/*RS*/     int          cut_off=0;
#endif /* !WITHTILDE */

  START_MODUL("load_body");
  RES_HEAP;
  ptr = *pptr;
  size = ptr[0];

#if !WITHTILDE
/*RS*/ /* bugfix for releasing unused variables in goal expression of */
/*RS*/ /* lrec expression, RS FEB 1994, without this bugfix the example program */
/*RS*/ /* let a=1,b=2 in def f[x]=a in b will reduce to 1 instead of 2 !! (that's great) */
/*RS*/
/*RS*/ if ((size > 0) && T_LREC(ptr[1])) {
/*RS*/   k = 2;
/*RS*/   for (i = ARITY(ptr[2]); --i>0; )
/*RS*/     k = size_lrec (ptr, k);
/*RS*/   cut_off = 1;
/*RS*/   }
#endif /* !WITHTILDE */

#if WITHTILDE
  tnargs = 0;
  /* tildevariablen werden noch nicht vorzeitig freigegeben */
#endif /* WITHTILDE */
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
#if !WITHTILDE
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
#else
	else
	if ((R_DESC(*(T_PTD)x,type) == TY_SWITCH)
	    || (R_DESC(*(T_PTD)x,type) == TY_MATCH)) {
	  PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
	  nargs = 0;
	}
        else
	  if ((R_DESC(*(T_PTD)x,type) == TY_SNSUB)){
	    PUSHSTACK(S_e,SET_VALUE(DOLLAR_TILDE,R_FUNC(*(T_PTD)x,nargs)));
	  }
#endif /* WITHTILDE */
#if    UH_ZF_PM
        /* ullis zf-expressions muessen hier beruechsichtigt werden ! */
         else
/*uh, 24.7.90*/ if  (R_DESC(*(T_PTD)x,type) == TY_ZF)  {
/*uh, 24.7.90*/   PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
/*uh, 24.7.90*/   nargs = 0;
/*uh, 24.7.90*/ }
#endif /* UH_ZF_PM */
      } /* if (nargs > 0) */
    }   /* T_POINTER(x)   */
    else
      if (nargs && T_NUM(x)) {
	/* da wo argumente freigegeben werden koennen wird ein DOLLAR */
	/* dazwischengeschoben, dessen VALUE-Feld deren anzahl angibt */
	index = margs-1-VALUE(x);
	if (index < nargs) {
	  nargs -= index;
	  PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
	  nargs = index;
	}
      }
#if 0
      else
      if ((tnargs && T_TILDE(x)) && tilde_sub) {
	/* analog zu T_NUM nur mit DOLLAR_TILDE  */
	/* Indizes invers zu normalen Argumenten */
	index = VALUE(x);                
	if (index < tnargs) {
	  tnargs -= index;
	  PUSHSTACK(S_e,SET_VALUE(DOLLAR_TILDE,tnargs));
	  tnargs = index;
	}
      }
#endif /* WITHTILDE */
    PUSHSTACK(S_e,x);
    size--;

#if !WITHTILDE
/*RS*/ if (cut_off && (size == (k-1)) && (nargs > 0))
/*RS*/   PUSHSTACK(S_e,SET_VALUE(DOLLAR,nargs));
#endif /* !WITHTILDE */

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

#if UH_ZF_PM
/*uh, 22.10.90*/
/*uh, 22.10.90*/  /* Resultat: 0: keine freien Variablen;  */
/*uh, 22.10.90*/  /* >0 freie Variablen            */
/*uh, 22.10.90*/  /* 1 im Otherwise-Ausruck */
/*uh, 22.10.90*/  /* i in der i-1. Klausel  */
/*uh, 22.10.90*/
/*uh, 22.10.90*/  static int test_switch(pts)
/*uh, 22.10.90*/  T_PTD pts;  /* Pointer To Switch */
/*uh, 22.10.90*/  {
/*uh, 22.10.90*/    int i=R_SELECTION(*pts,nclauses);      /* Anzahl Klauseln */
/*uh, 22.10.90*/    T_PTD ptd=R_SELECTION(*pts,clauses);   /* Klausel */
/*uh, 22.10.90*/    T_PTD exprdesc;
/*uh, 22.10.90*/
/*uh, 22.10.90*/    while ( ptd ) {
/*uh, 22.10.90*/      exprdesc=(T_PTD)R_CLAUSE(*ptd,sons)[1]; /* Guard */
/*uh, 22.10.90*/      if (test_num(R_EXPR(*exprdesc,pte)))  return(i);
/*uh, 22.10.90*/          /* sonst: i-ter guard hat keine relativ freie variablen */
/*uh, 22.10.90*/      exprdesc=(T_PTD)R_CLAUSE(*ptd,sons)[2]; /* Body */
/*uh, 22.10.90*/      if (test_num(R_EXPR(*exprdesc,pte)))  return(i);
/*uh, 22.10.90*/         /* sonst: i-ter body hat keine relativ freie variablen */
/*uh, 22.10.90*/
/*uh, 22.10.90*/      ptd=R_CLAUSE(*ptd,next);
/*uh, 22.10.90*/      i--;
/*uh, 22.10.90*/    }
/*uh, 22.10.90*/
/*uh, 22.10.90*/    /* otherwise Ausdruck ueberpruefen */
/*uh, 22.10.90*/    if ((exprdesc=R_SELECTION(*pts,otherwise))) {
/*uh, 22.10.90*/      if (test_num(R_EXPR(*exprdesc,pte))) return(1); /* Otherwiseausdruck hat relativ freie Variablen */
/*uh, 22.10.90*/    }
/*uh, 22.10.90*/
/*uh, 22.10.90*/    /* switch hat keine relativ freie variablen */
/*uh, 22.10.90*/    return(0);
/*uh, 22.10.90*/  }
/*uh, 22.10.90*/
/*uh, 22.10.90*/  static void load_clauses(pdesc)
/*uh, 22.10.90*/    T_PTD pdesc;
/*uh, 22.10.90*/    { T_PTD px;
/*uh, 22.10.90*/
                      START_MODUL("load_clauses");
/*uh, 22.10.90*/      /* Die Klauseln auf den Stack */
/*uh, 22.10.90*/      /* Otherwise vorhanden? */
                      DBUG_PRINT("load_clauses", ("starting..."));
/*uh, 22.10.90*/      if ((px=R_SELECTION(*pdesc,otherwise))!=NULL) { /* Aus den Stack holen */
                        DBUG_PRINT("load_clauses", ("otherwise!=NULL"));
/*uh, 22.10.90*/        load_body((STACKELEM **)A_EXPR(*px,pte),0,0,0);
/*uh, 22.10.90*/        PUSHSTACK(S_e,PM_OTHERWISE);
/*uh, 22.10.90*/      }
/*uh, 22.10.90*/
/*uh, 22.10.90*/      /* Klauseln holen */
                      DBUG_PRINT("load_clauses", ("Klauseln holen"));
/*uh, 22.10.90*/      if ((px=R_SELECTION(*pdesc,clauses))!=NULL) { /* Klauseln da */
                        DBUG_PRINT("load_clauses", ("clauses!=NULL"));
/*uh, 22.10.90*/        PUSHSTACK(S_i,DOLLAR);
/*uh, 22.10.90*/        INC_REFCNT(px);
/*uh, 22.10.90*/        PUSHSTACK(S_i,px);
                        DBUG_PRINT("load_clauses", ("px = %x", px));
/*uh, 22.10.90*/        while ((px=R_CLAUSE(*px,next))!=NULL) {
                           DBUG_PRINT("load_clauses", ("px = %x", px));
                           DBUG_PRINT("load_clauses", ("R_CLAUSE(*px,next))!=NULL"));
/*uh, 22.10.90*/           INC_REFCNT(px);
/*uh, 22.10.90*/           PUSHSTACK(S_i,px);
/*uh, 22.10.90*/        }
/*uh, 22.10.90*/
/*uh, 22.10.90*/        while (!T_DOLLAR(READSTACK(S_i))) {
                          DBUG_PRINT("load_clauses", ("(!T_DOLLAR(READSTACK(S_i)))"));
/*uh, 22.10.90*/          PUSHSTACK(S_e,POPSTACK(S_i));
/*uh, 22.10.90*/        }
                        DBUG_PRINT("load_clauses", ("pop'ing..."));
/*uh, 22.10.90*/        px = (T_PTD) POPSTACK(S_i); /* avoid warning: value computed is not used */
/*uh, 22.10.90*/      }
                      DBUG_PRINT("load_clauses", ("done..."));
                      END_MODUL("load_clauses");
/*uh, 22.10.90*/    }
/*uh, 22.10.90*/

#else /* UH_ZF_PM */

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

#endif /* UH_ZF_PM */

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
#if UH_ZF_PM
/*uh, 23.9.91*/            case TY_SELECTION:
                        if (test_switch(*ptd))
                          return(1);
                        break;
#else /* UH_ZF_PM */
          case TY_SWITCH:
                        if (test_switch(ptd))  /* *ptd durch ptd ersetzt, warum auch immer RS 4.12.1992 */
                          return(1);
                        break;
#endif /* UH_ZF_PM */
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

#if WITHTILDE
static STACKELEM new_func(x,n,t)
int       t;                        /* tildeargs needed */
#else
static STACKELEM new_func(x,n)
#endif /* WITHTILDE */
STACKELEM x;
int       n;
{
  STACKELEM h=(STACKELEM)0; /* Initialisierung von TB, 6.11.1992 */
  T_PTD     ptd /*,px*/;   /* RS 30/10/92 */
  int       i;
  int       found = 0;

  /* suchen ob schon vorhanden */
#if WITHTILDE
  {
    T_PTD letrec, comb, desc;

    desc = (T_PTD)x;                        /* x ist lrec_ind-deskriptor */
    letrec = (T_PTD)R_LREC_IND(*desc,ptdd); /* zeiger auf den deskriptor des
					       letrec, in dem diese funktion
					       definiert ist */
    for (i = SIZEOFSTACK(S_v) ; i > 0 ; i--) {
      h = POPSTACK(S_v);
      desc = comb = (T_PTD)h;

      if (R_DESC(*desc,type) == TY_COMB) {         /* comb-descriptor      */
	desc = (T_PTD)R_COMB(*desc,ptd);           /* lrec-ind-descriptor  */
	if (R_DESC(*desc,type) == TY_LREC_IND) {
	  desc = (T_PTD)R_LREC_IND(*desc,ptdd);    /* letrec-descriptor    */
	  if (letrec == desc) {                    /* beide funktionen aus
						      gleichem letrec      */
	    if (t < (R_COMB(*comb,args)))
	      t = R_COMB(*comb,args);              /* groesser: korrekt    */
	  }
	}
      }
      PUSHSTACK(S_hilf,h);
      found = (R_UNIV((*(T_PTD)h),ptd) == (T_PTD)x);
      if (found) break;
    }
  }
#else
  for (i = SIZEOFSTACK(S_v) ; i > 0 ; i--) {
    h = POPSTACK(S_v);
    PUSHSTACK(S_hilf,h);
    found = (R_UNIV((*(T_PTD)h),ptd) == (T_PTD)x);
    if (found) break;
  }
#endif /* WITHTILDE */
  for (i = SIZEOFSTACK(S_hilf) ; i > 0 ; i--)
    PUSHSTACK(S_v,POPSTACK(S_hilf));
  if (found) {
   /* INC_REFCNT((T_PTD)h); */
    return(h);
  }
  /* einfuegen auf i */
#if WITHTILDE
  {
    T_PTD letrec, comb, desc;

    desc = (T_PTD)x;                        /* x ist lrec_ind-deskriptor */
    letrec = (T_PTD)R_LREC_IND(*desc,ptdd); /* zeiger auf den deskriptor des
					       letrec, in dem diese funktion
					       definiert ist */
    for (i = SIZEOFSTACK(S_i) ; i > 0 ; i--) {
      h = POPSTACK(S_i);
      desc = comb = (T_PTD)h;

      if (R_DESC(*desc,type) == TY_COMB) {         /* comb-descriptor      */
	desc = (T_PTD)R_COMB(*desc,ptd);           /* lrec-ind-descriptor  */
	if (R_DESC(*desc,type) == TY_LREC_IND) {
	  desc = (T_PTD)R_LREC_IND(*desc,ptdd);    /* letrec-descriptor    */
	  if (letrec == desc) {                    /* beide funktionen aus
						      gleichem letrec      */
	    if (t < (R_COMB(*comb,args)))
	      t = R_COMB(*comb,args);              /* groesser: korrekt    */
	  }
	}
      }
      PUSHSTACK(S_hilf,h);
    }
  }
#else
  for (i = SIZEOFSTACK(S_i) ; i > 0 ; i--) {
    PUSHSTACK(S_hilf,POPSTACK(S_i));
  }
#endif /* WITHTILDE */
  MAKEDESC(ptd,1,C_FUNC,TY_COMB);
#if WITHTILDE
  L_COMB((*ptd),args) = t;
#else
  L_COMB((*ptd),args) = 0;
#endif /* WITHTILDE */
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

#if WITHTILDE
static STACKELEM new_cond(x,n,t)
int       t;
#else
static STACKELEM new_cond(x,n)
#endif /* WITHTILDE */
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
#if WITHTILDE
  /* t darf nicht groesser als 2^15 - 1 werden!          */
  /* Die unteren zwei Bit sind fuer KEEP/FREE reserviert */
  L_CONDI((*ptd),args) = (t << 2);
#else
  L_CONDI((*ptd),args) = 0;
#endif
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

#if WITHTILDE
static STACKELEM new_case(x, n, t)
int       t;                      /* size of tilde frame */
#else
static STACKELEM new_case(x,n)
#endif /* WITHTILDE */
STACKELEM x;                      /* switch-descriptor */
int       n;                      /* laenge arg.frame + 1 */
{
  T_PTD ptd;
#if WITHTILDE
  T_PTD desc;
#endif
  int   i;
#if UH_ZF_PM
  T_PTD oldmatch;
#endif

   START_MODUL("new_case");

#if UH_ZF_PM
  oldmatch = R_SELECTION(*(T_PTD)x, clauses);
#endif

  for (i = SIZEOFSTACK(S_i) ; i > 0 ; i--) {
    ptd = (T_PTD)READSTACK(S_i);
    if (R_DESC((*ptd),type) != TY_CONDI)
      break;
    PUSHSTACK(S_hilf,POPSTACK(S_i));
  }

#if WITHTILDE
#if UH_ZF_PM
  if (R_DESC(*(T_PTD)R_CLAUSE(*oldmatch,sons)[3],type) == TY_MATCH) { 
#else
  if (R_DESC(*(T_PTD)x,type) == TY_MATCH) {
#endif
    MAKEDESC(desc,2,C_EXPRESSION,TY_SWITCH);
    L_SWITCH(*desc,nwhen) = 2;
#if UH_ZF_PM
    L_SWITCH(*desc,anz_args) = (COUNT)R_MATCH(*(T_PTD)R_CLAUSE(*oldmatch,sons)[3],code)[STELLIGKEIT];
#else
    L_SWITCH(*desc,anz_args) = (COUNT)R_MATCH(*(T_PTD)x,code)[STELLIGKEIT];
#endif
    GET_HEAP(5,A_SWITCH(*desc,ptse));
    RES_HEAP;
    L_SWITCH(*desc,ptse)[0] = 4;
#if UH_ZF_PM
    L_SWITCH(*desc,ptse)[1] = (T_HEAPELEM)R_CLAUSE(*oldmatch,sons)[3]; /* MATCH */
#else
    L_SWITCH(*desc,ptse)[1] = (T_HEAPELEM)x; /* MATCH */
#endif
    L_SWITCH(*desc,ptse)[2] = (T_HEAPELEM)0; /* no otherwise */
    L_SWITCH(*desc,ptse)[3] = (T_HEAPELEM)0;
    L_SWITCH(*desc,ptse)[4] = (T_HEAPELEM)0;
    L_SWITCH(*desc,casetype) =  WHEN_WITHOUT_CASE; /* new casetype */
    REL_HEAP;

#if UH_ZF_PM
    L_CLAUSE(*oldmatch,sons)[3] = (T_HEAPELEM)desc;
#else
    x = (STACKELEM)desc;
#endif
  }
#endif /* WITHTILDE */

  MAKEDESC(ptd,1,C_FUNC,TY_CASE);
#if WITHTILDE
  L_CASE((*ptd),args) = t;                      /* relativ freie Variablen */
#else
  i = test_switch((T_PTD)x); /* rel. fr. var.? 0: nein, >0: ja */ /* T_PTD eingefuegt RS 3.12.1992 */
  L_CASE((*ptd),args) = i;                      /* relativ freie Variablen */
  L_CASE((*ptd),nargs) = i ? n : 1;
#endif /* WITHTILDE : mah 030294 */
  /* NCASE: add number of multiple patterns */
  /* case_type is the pattern arity! */
#if WITHTILDE
#if UH_ZF_PM
  L_CASE((*ptd),nargs) =  R_SWITCH(*(T_PTD)R_CLAUSE(*oldmatch,sons)[3],anz_args); /* args needed */
#else
  L_CASE((*ptd),nargs) =  R_SWITCH(*(T_PTD)x,anz_args); /* args needed */
#endif 
#else
  L_CASE((*ptd),nargs) +=  R_SWITCH(*(T_PTD)x,case_type) - 1;
#endif /* WITHTILDE */
  L_CASE((*ptd),ptd) = (T_PTD)x;
  L_CASE((*ptd),ptc) = (INSTR *)0;
  PUSHSTACK(S_i,ptd);
  for (i = SIZEOFSTACK(S_hilf) ; i > 0 ; i--)
    PUSHSTACK(S_i,POPSTACK(S_hilf));
  END_MODUL("new_case");
  return((STACKELEM)ptd);
}

/* ------------------------------------------------------------------- */
/*  new_sub()   legt einen neuen sub-pointer unter alle bereits        */
/*              vorhandenen cond-pointer auf den i-stack               */
/* ------------------------------------------------------------------- */

#if WITHTILDE
static STACKELEM new_sub(x,n,t)
int       t;     /* tildeargs needed */
#else
static STACKELEM new_sub(x,n)
#endif /* WITHTILDE */
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
#if WITHTILDE
  L_COMB((*ptd),args) = t;       /* tildeargs */
#else
  L_COMB((*ptd),args) = 0;       /* unused */
#endif /* WITHTILDE */
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

  DBUG_ENTER ("collect_args");

#if WITHTILDE
#if D_DIST
  if (T_AP_1(READSTACK(S_m)) || T_DAP_1(READSTACK(S_m)))
#else
  if (T_AP_1(READSTACK(S_m)))
#endif
  {
#endif /* WITHTILDE */
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
#if WITHTILDE
  }
  else
  {
    while (T_SNAP_1(READSTACK(S_m)))
    {
      args += (READ_ARITY(S_m1) -1);
      PPOPSTACK(S_m);
      PPOPSTACK(S_m1);
    }
  }
#endif /* WITHTILDE */

  DBUG_RETURN(args);
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
           int       args         = 0;
           int       argsneeded   = 0;
           int       arity;
           int       margs = 0;       /* laenge des argumentframes */
           int       nfree = 0;       /* bereits freigegebene arg. */
#if WITHTILDE
           int       targs        = 0;/* Laenge des tildeframes    */
           int       nfree_r      = 0;/* analog zu nfree           */
           int       tilde_sub    = 0;/* flag S-Schema             */
           int       argsneeded_t = 0;/* analog zu argsneeded      */
           int       args_t       = 0;/* analog zu args            */
           int       ap_to_def    = 0;

           int       closed_lrec  = 0;/* generate gamma for closed lrec */

           int       no_code      = 0;/* kein code erzeugen fuer   */
                                      /* Argumente eines POST_SNAP */
           int       temp_targs   = 0;
#endif /* WITHTILDE */
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
/*uh, 19.7.90*/             T_PTD     oldswitch,uhtmp,/* px2,*/ for_desc, rof_desc, if_desc, listerr_desc, fi_desc1, fi_desc2, end_desc;
/*uh, 19.7.90*/             T_PTD        cdesc;
/*uh, 19.7.90*/             PTR_HEAPELEM zfc=0;    /* Pointer to ZF-Subexpression */
/*uh, 19.7.90*/             PTR_HEAPELEM vars=0;   /* Pointer to bound Variablenames */
/*uh, 19.7.90*/             T_PTD        zfd=0;
/*uh, 19.7.90*/             /* int          zfbound = 0;  umbenannt in nr_additionals uh 11.4.92 */
/*uh, 19.7.90*/             int          nr_additionals = 0;
#endif /* UH_ZF_PM */

#if UH_ZF_PM
/*uh, 17.10.90*/             int       n,rest;
/*uh, 17.10.90*/             /* STACKELEM el; */
/*uh, 17.10.90*/             T_PTD     fail, undecided, cont, backtrack, loop; /* Label */
/*uh, 17.10.90*/             T_PTD     patsub;
/*uh, 17.10.90*/             int       within_pattern = FALSE;
/*uh, 17.10.90*/             int       actual_depth = 0; /* Aktuelle Verschachtelungstiefe der Pattern */
/*uh, 17.10.90*/             int       pattern_depth = 0; /* maximale Verschachtelungstiefe der Pattern */
/*uh, 17.10.90*/             int       bindings  = 0; /* maximale Anzahl der Bindungen im Pattern */
/*uh, 17.10.90*/             int       binding   = 0; /* lfde Nummer der aktuellen Bindung */
/*uh, 17.10.90*/             int       btsize    = 0; /* maximal benoetigte Tiefe des BT-Stacks */
/*uh, 17.10.90*/             int       btlevel   = 0; /* aktuelle BT-Ebene */
/*uh, 17.10.90*/             int       lastarbs  = 0; /* Anzahl der letzten ... im Pattern */
/*uh, 17.10.90*/             int       lastlevel = 0; /* Index in BT-Stack fuer Teillisteninformation */
/*uh, 17.10.90*/             T_PTD     charset   = 0; /* Descriptor fuer IN Konstrukt */
/*uh, 17.10.90*/             int       guard     = 0; /* Flag, ob konstanter Guard TRUE vorhanden. */
/*uh, 17.10.90*/             int       smallfit  = 0; /* Match-Strtegie fue Skipelemente */
#define SIZE_L 16384
/*uh, 17.10.90*/             int       Stack_l[SIZE_L];  /* Stack fuer Labels */
/*uh, 17.10.90*/             int       *S_l = Stack_l; /* Stackptr fuer Labelstack */
/*uh, 17.10.90*/             T_PTD     thiscase = NULL;  /* Zeiger auf aktuellen SELECTION Descriptor */
/*uh, 17.10.90*/             T_PTD     thisclause = NULL ; /* Zeiger auf aktuellen CLAUSE Descriptor */
                             int clause_nr=0;
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

  /* codefile aufmachen */
  if (codefp) fclose(codefp);
  if ((codefp = fopen(CodeFile,"w")) == NULL)
     post_mortem("ear: can't open codefile");
  if (x == 0 || !T_POINTER(x) || R_DESC(*(T_PTD)x,class) != C_EXPRESSION)
     POST_MORTEM("ear: must start with an expression pointer");

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

#if WITHTILDE
  if (T_DOLLAR_TILDE(x)) {
    i = VALUE(x);
    FREE_INSTR_R(i,nfree_r);
    goto main_e;
  }
  if (T_POST_SNAP(x)) {
    /* das POST_SNAP liftet die freien Variablen */
    /* vor das zugehoerige lrec, damit im Post-  */
    /* prozessor vor Eintritt in die Ruempfe des */
    /* lrec die relativ freien Variablen bekannt */
    /* sind. Waehrend der Compilationsphase muss */
    /* das POST_SNAP mit seinen Argumenten!      */
    /* ignoriert werden: kein code!   mah 180194 */

    no_code = 1;
  }

#endif /* WITHTILDE */


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
/*uh, 26.7.90*/  /*if ( (VALUE(x) < nr_additionals) || (VALUE(x) >= margs) )  { */  /* Variable in erweiterten Frames */
/*uh, 26.7.90*/     /*    i = ARG_INDEX(VALUE(x)); */
/*uh, 26.7.90*/ /* } else { */ 
/*uh, 26.7.90*/      /*   i = ARG_INDEX(margs - 1 - VALUE(x) + nr_additionals); */
/*uh, 26.7.90*/ /* } */
/*uh, 26.7.90*/  

                i = ARG_INDEX(margs-1-VALUE(x));

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

#if WITHTILDE
    if (no_code)
      goto main_m;
#endif /* WITHTILDE */

    /* argumentframe freizugeben? */
    if (NO_MORE_VARIABLES()) {                       /* argumente koennen  */
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
#if WITHTILDE
	if (tilde_sub) {

#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("1: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (nfree-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */

#if D_MESS
  if (D_M_C_MOVE_AR)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE_AR);
#endif
	  GEN_INSTR0(I_MOVE_AR,"move_ar"); }
	else {
#endif /* WITHTILDE */

#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("1: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (nfree-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */

#if D_MESS
  if (D_M_C_MOVE_AW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE_AW);
#endif

	  GEN_INSTR0(I_MOVE_AW,"move_aw");                /* optimierung    */
#if WITHTILDE
          }
#endif /* WITHTILDE */

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

#if WITHTILDE
	if (tilde_sub) {
#if D_MESS
  if (D_M_C_PUSH_AR0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_AR0);
#endif
	  GEN_INSTR0(I_PUSH_AR0,"push_ar0"); }
	else {
#endif /* WITHTILDE */

#if D_MESS
  if (D_M_C_PUSH_AW0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_AW0);
#endif
	  GEN_INSTR0(I_PUSH_AW0,"push_aw0");

#if WITHTILDE
          }
#endif /* WITHTILDE */
      }
      else {
#if WITHTILDE
	if (tilde_sub) {

#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("3: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, i)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (i-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */

#if D_MESS
  if (D_M_C_PUSH_AR)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_AR);
#endif

	  GEN_INSTR1(I_PUSH_AR,"push_ar",i-nfree); }
	else {
#endif /* WITHTILDE */

#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("3: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, i)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))      /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT(VAL_INT(MIDSTACK(S_pm, 1)) | (INT << (i-VAL_INT(MIDSTACK(S_pm, 2)))))); }
              /* updating S_pm used-stackelements-entry */
#endif /* D_DIST */

#if D_MESS
  if (D_M_C_PUSH_AW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_AW);
#endif

	  GEN_INSTR1(I_PUSH_AW,"push_aw",i-nfree);

#if WITHTILDE
          }
#endif /* WITHTILDE */
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

#if UH_ZF_PM
/*uh, 17.10.90*/
/*uh, 17.10.90*/  if (T_PM_OTHERWISE(x)) {
/*uh, 17.10.90*/     /* Klausel ist Otherwise-Klausel */
/*uh, 17.10.90*/     /* Auf E steht nun der Otherwise Ausdruck. */
/*uh, 17.10.90*/     /* Es wird jetzt "when . guard true" erzeugt. */
                     DBUG_PRINT("rear", ("Baue otherwise !"));
/*uh, 17.10.90*/
/*uh, 17.10.90*/     PUSH_L(thisclause);
/*uh, 17.10.90*/     /* printf("PUSH_L otherwise depth=%d\n",DEPTH_L()); */
/*uh, 17.10.90*/
/*uh, 17.10.90*/     PUSHSTACK(S_e,SA_TRUE);  /* Guard */
/*uh, 17.10.90*/     PUSHSTACK(S_e,PM_SKIP);  /* Pattern */
/*uh, 17.10.90*/
/*uh, 17.10.90*/     within_pattern=TRUE;
/*uh, 17.10.90*/     pattern_depth = btsize = btlevel = lastarbs = bindings = 0;
/*uh, 17.10.90*/     binding =  actual_depth = 0;
/*uh, 17.10.90*/     lastlevel = 0;
/*uh, 17.10.90*/
/*uh, 17.10.90*/     PUSHSTACK(S_m,SET_ARITY(PM_WHEN,2));  /* WHEN Konstruktor nach M */
/*uh, 17.10.90*/     PUSHSTACK(S_m1,DOLLAR);  /* FAIL */
/*uh, 17.10.90*/     PUSH_L(DOLLAR);   /* UNDECIDED */
/*uh, 17.10.90*/     goto main_e;
/*uh, 17.10.90*/  }
/*uh, 17.10.90*/
/*uh, 17.10.90*/
/*uh, 17.10.90*/    if (within_pattern) {
/*uh, 17.10.90*/      /* Ein Pattern wird traversiert. */
/*uh, 17.10.90*/      if (T_POINTER(x)) {
/*uh, 17.10.90*/         desc = (T_PTD)x;
/*uh, 17.10.90*/         if (R_DESC(*desc,class) == C_LIST) {
/*uh, 17.10.90*/            /* if (R_DESC(*desc,type) == TY_STRING) { */
/*uh, 17.10.90*/            /*   POST_MORTEM("ear: Stringpattern werden z.Zt. nicht unterstuetzt."); */
/*uh, 17.10.90*/            /* } */
/*uh, 17.10.90*/            /* Das (Teil-) Pattern ist ein ListenPattern */
/*uh, 17.10.90*/            /* Es muss nun Code fuer den Abstieg in die Listenelemente erzeugt */
/*uh, 17.10.90*/            /* werden. Dann wird Code fuer jedes einzelne Listenelement erzeugt */
/*uh, 17.10.90*/
/*uh, 17.10.90*/            i=R_LIST(*desc,dim);
/*uh, 17.10.90*/
/*uh, 17.10.90*/            if (!i) { /* leere Liste */
/*uh, 17.10.90*/               MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/               MARK(undecided,3); PUSH_L(undecided);
/*uh, 17.10.90*/
                               if (R_DESC(*desc,type) == TY_STRING)
                                 GEN_INSTR3_UH(I_MATCHSTR,"matchstr",0,"fail",(int)fail,"undecided",(int)undecided);
                              /* GEN_INSTR2L(I_MATCHNILS,"matchnils","fail",(int)fail,"undecided",(int)undecided); */
                               else
/*uh, 17.10.90*/              /* GEN_INSTR2L(I_MATCHNIL,"matchnil","fail",(int)fail,"undecided",(int)undecided); */
                                 GEN_INSTR3_UH(I_MATCHLIST,"matchlist",0,"fail",(int)fail,"undecided",(int)undecided);
/*uh, 17.10.90*/               goto main_m;
/*uh, 17.10.90*/            }
/*uh, 17.10.90*/
/*uh, 17.10.90*/            PUSHSTACK(S_m,SET_ARITY(LIST,i));
                            if (R_DESC(*desc,type) == TY_STRING)
                              PUSHSTACK(S_tilde, _nilstring);
                            else
                              PUSHSTACK(S_tilde, _nil);
/*uh, 17.10.90*/
/*uh, 17.10.90*/            actual_depth++;
/*uh, 17.10.90*/
/*uh, 17.10.90*/            MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/            MARK(undecided,3); PUSH_L(undecided);
/*uh, 17.10.90*/
                            if (R_DESC(*desc,type) == TY_STRING)
/*uh, 17.10.90*/              if /* (level0arbs(desc)) */ (INFO(desc,skskips))
/*uh, 17.10.90*/                GEN_INSTR3_UH(I_MATCHARBS ,"matcharbs" ,INFO(desc,elements),"fail",(int)fail,"undecided",(int)undecided);
/*uh, 17.10.90*/              else
/*uh, 17.10.90*/                GEN_INSTR3_UH(I_MATCHSTR,"matchstr",i,"fail",(int)fail,"undecided",(int)undecided);
                            else
/*uh, 17.10.90*/              if /* (level0arbs(desc)) */ (INFO(desc,skskips))
/*uh, 17.10.90*/                GEN_INSTR3_UH(I_MATCHARB ,"matcharb" ,INFO(desc,elements),"fail",(int)fail,"undecided",(int)undecided);
/*uh, 17.10.90*/              else
/*uh, 17.10.90*/                GEN_INSTR3_UH(I_MATCHLIST,"matchlist",i,"fail",(int)fail,"undecided",(int)undecided);
                    

/*uh, 17.10.90*/
/*uh, 17.10.90*/            FREE_INFO(desc);
/*uh, 17.10.90*/
/*uh, 17.10.90*/            GEN_INSTR1(I_SAVEPTR,"saveptr",actual_depth+1);
/*uh, 17.10.90*/            GEN_INSTR0(I_NESTLIST,"nestlist");
/*uh, 17.10.90*/            GEN_INSTR0(I_DEREF,"dereference");
/*uh, 17.10.90*/
/*uh, 17.10.90*/            load_list(desc);
/*uh, 17.10.90*/
/*uh, 17.10.90*/             /*           for (i=R_LIST(*desc,dim)-1; i>=0; i--) {  */
/*uh, 17.10.90*/             /*            PUSHSTACK(S_e,R_LIST(*desc,ptdv)[i]);    */
/*uh, 17.10.90*/             /*          } */
/*uh, 17.10.90*/            goto main_e;
/*uh, 17.10.90*/         }  /* class == C_LIST */
/*uh, 17.10.90*/         if (R_DESC(*desc,type)==TY_NAME) {
/*uh, 17.10.90*/            /* Variable innerhalb eines Patterns */
/*uh, 17.10.90*/            /* GEN_INSTR1(I_BIND,"bind",bindings-binding-1); REVERSE ORDER (OK it was OK ! :-) */
                                 /* the problem was UH's strange NUM indexing (now turned off, see above) */

/* binding is stored in the (modified) NAME-Descriptor ! */
                            binding = VAL_INT((STACKELEM)R_NAME((*desc),ptn));

                            if (SIZEOFSTACK(S_tilde) > 0) 
                              if (READSTACK(S_tilde) == (int)_nilstring)
                                GEN_INSTR1(I_BINDS,"binds",bindings-binding-1);
                              else
                                GEN_INSTR1(I_BIND,"bind",bindings-binding-1); 
                            else
                              GEN_INSTR1(I_BIND,"bind",bindings-binding-1);
/*uh, 17.10.90*/            binding++;
/*uh, 17.10.90*/            goto main_m;
/*uh, 17.10.90*/         }
/*uh, 17.10.90*/         if (R_DESC(*desc,type)==TY_EXPR) {
/*uh, 17.10.90*/            /* Ausdruck innerhalb eines Patterns */
/*uh, 17.10.90*/            load_expr((STACKELEM **)A_EXPR(*desc,pte));
/*uh, 17.10.90*/            goto main_e;
/*uh, 17.10.90*/         }
/*uh, 17.10.90*/
/*uh, 17.10.90*/         if (IS_DOTS(desc)) {  /* ... oder .+ im Pattern */
/*uh, 17.10.90*/           /* Hier ist ein echter ... in einem Pattern entdeckt worden.  */
/*uh, 17.10.90*/           /* Nun muss Code zum Retten der Backtrack-Information generiert werden. */
/*uh, 17.10.90*/           /* Ausserdem muss das FailLabel (lokal) auf den Backtrackfall gesetzt werden */
/*uh, 17.10.90*/
/*uh, 17.10.90*/           /* Bestimme Match-Richtung */
/*uh, 17.10.90*/           if (IS_DOTSTAR(desc))  smallfit=FALSE; else smallfit=TRUE;
/*uh, 17.10.90*/
/*uh, 17.10.90*/           /* Berechne die minimale Anzahl der im Argument benoetigten Elemente */
/*uh, 17.10.90*/           n = ARITY(READSTACK(S_m))-1;
/*uh, 17.10.90*/           rest = R_PATTERN(*desc,following);
/*uh, 17.10.90*/
/*uh, 17.10.90*/           if (IS_DOTPLUS(desc)) { /* .+ match mindestens ein Element */
/*uh, 17.10.90*/              rest--;
/*uh, 17.10.90*/           }
/*uh, 17.10.90*/
/*uh, 17.10.90*/           ENCLOSING_LIST;  /* Umfassende Liste wird hierdurch auf A geschrieben */
/*uh, 17.10.90*/           GEN_INSTR5(I_INITBT,"initbt",rest, /* <-- Patrestlist */
/*uh, 17.10.90*/                      btlevel+BTSTART,btlevel+BTPTR,
/*uh, 17.10.90*/                      btlevel+BTDESC, btlevel+BTEND);
/*uh, 17.10.90*/
/*uh, 17.10.90*/           if (!smallfit) {
/*uh, 17.10.90*/             GEN_INSTR0(I_ENDLIST,"endlist");
/*uh, 17.10.90*/             GEN_INSTR1(I_ADVANCE,"advance",-rest);
/*uh, 17.10.90*/           }
/*uh, 17.10.90*/
/*uh, 17.10.90*/           if (IS_DOTPLUS(desc)) { /* .+ matcht mindestens ein Element */
/*uh, 17.10.90*/              if (smallfit)
/*uh, 17.10.90*/                GEN_INSTR1(I_ADVANCE,"advance",1);
/*uh, 17.10.90*/              else
/*uh, 17.10.90*/                GEN_INSTR1(I_ADVANCE,"advance",-1);
/*uh, 17.10.90*/           }
/*uh, 17.10.90*/
/*uh, 17.10.90*/           BACKWARDMARK("bt",backtrack);
/*uh, 17.10.90*/           MARK(fail,2); PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/
/*uh, 17.10.90*/           if (smallfit) {
/*uh, 17.10.90*/             GEN_INSTR2LA(I_ATEND,"atend",btlevel+BTEND,"fail",(int)fail);
/*uh, 17.10.90*/           } else {
/*uh, 17.10.90*/             GEN_INSTR2LA(I_ATSTART,"atstart",btlevel+BTSTART,"fail",(int)fail);
/*uh, 17.10.90*/           }
/*uh, 17.10.90*/
/*uh, 17.10.90*/           GEN_INSTR1(I_SAVEBT,"savebt",btlevel);
/*uh, 17.10.90*/           PUSHSTACK(S_m1,backtrack);
/*uh, 17.10.90*/           PUSHSTACK(S_m1,charset);  /* fuer Backtrackcode */
/*uh, 17.10.90*/           PUSHSTACK(S_m1,smallfit); /* fuer Backtrackcode */
/*uh, 17.10.90*/           PUSHSTACK(S_m1,DOLLAR); /* Markiere Ende des Aufloesens dieser BT Ebene */
/*uh, 17.10.90*/           btlevel+= BT_ITEM_SIZE;
/*uh, 17.10.90*/           WRITESTACK(S_m,DEC(READSTACK(S_m)));
/*uh, 17.10.90*/
/*uh, 17.10.90*/           GEN_INSTR0(I_DEREF,"dereference");
/*uh, 17.10.90*/           goto main_e;
/*uh, 17.10.90*/         }
/*uh, 17.10.90*/
/*uh, 17.10.90*/         if (IS_LAST(desc))   /* letztes .+ im Pattern oder */
/*uh, 17.10.90*/                           {  /* letztes ... im Pattern */
/*uh, 17.10.90*/         /* Waehrend des matchen einer liste, muss nun am aktuellen Element abgebrochen */
/*uh, 17.10.90*/         /* und das matchen vom Ende der Liste her fortgesetzt werden. */
/*uh, 17.10.90*/         /* Der Listenkonstruktor muss entfernt werden.  Fuer ihn wir ein */
/*uh, 17.10.90*/         /* Prelist - Konstruktor gleicher Stelligkeit auf den M-Stack gelegt. */
/*uh, 17.10.90*/         /* durch AS Konstrukte bedingt liegt der ListenKonstruktor NICHT immer */
/*uh, 17.10.90*/         /* OBEN auf M */
/*uh, 17.10.90*/
#ifdef DEBUG
/*uh, 17.10.90*/         if ((!T_LIST (READSTACK(S_m))) &&
/*uh, 17.10.90*/             (!T_PM_AS(READSTACK(S_m)))
/*uh, 17.10.90*/            )
/*uh, 17.10.90*/             POST_MORTEM("ear: Listenkonstrukor oder AS auf m (innerhalb eines Patterns) erwartet!");
#endif
/*uh, 17.10.90*/          PUSHSTACK(S_i,DOLLAR);
/*uh, 17.10.90*/          while (!T_LIST(x=POPSTACK(S_m))) {  /* Rette verbergende Konstruktoren auf I */
/*uh, 17.10.90*/            PUSHSTACK(S_i,x);
/*uh, 17.10.90*/          }
/*uh, 17.10.90*/
/*uh, 17.10.90*/          while (!T_DOLLAR(READSTACK(S_i))) {  /* Wieder zurueck */
/*uh, 17.10.90*/            PUSHSTACK(S_m,POPSTACK(S_i));
/*uh, 17.10.90*/          }
/*uh, 17.10.90*/          PPOPSTACK(S_i);
/*uh, 17.10.90*/
/*uh, 17.10.90*/          PUSHSTACK(S_m,SET_ARITY(PRELIST,ARITY(x)));
/*uh, 17.10.90*/          /* Code generieren der das Matchen des Arguments vom Ende an initiiert */
/*uh, 17.10.90*/          ENCLOSING_LIST;
/*uh, 17.10.90*/          GEN_INSTR0(I_ENDLIST,"endlist");
/*uh, 17.10.90*/
/*uh, 17.10.90*/          goto main_m;
/*uh, 17.10.90*/
/*uh, 17.10.90*/         }
/*uh, 17.10.90*/
/*uh, 17.10.90*/         if (IS_DOTPLUS(desc)) { /* .+ im Pattern */
/*uh, 17.10.90*/            post_mortem("ear: .+ decsriptor");
/*uh, 17.10.90*/         }

/* hier kommt jetzt die SCALAR(real) und DIGIT-Behandlung ! RS 28.2.1995 */

                         if ((R_DESC(*desc,class) == C_SCALAR) && (R_DESC(*desc,type)==TY_REAL)) {
                           MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
                           MARK(undecided,3); PUSH_L(undecided);
                           GEN_INSTR3_UHS(I_MATCHREAL,"matchreal",desc,"fail",(int)fail,"undecided",(int)undecided);
                           goto main_m;
                           }

                         if (R_DESC(*desc,class)==C_DIGIT) {
                           MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
                           MARK(undecided,3); PUSH_L(undecided);
                           GEN_INSTR3_UHS(I_MATCHDIGIT,"matchdigit",desc,"fail",(int)fail,"undecided",(int)undecided);
                           goto main_m;
                           }
                    
/*uh, 17.10.90*/
/*uh, 17.10.90*/         POST_MORTEM("ear: unexpected DESCRIPTOR in pattern!");
/*uh, 17.10.90*/      } /* T_POINTER(x) */
/*uh, 17.10.90*/
/*uh, 17.10.90*/      if (T_CON(x)) {
/*uh, 17.10.90*/        if (T_PM_AS(x)) {
/*uh, 17.10.90*/           /* Unter einem AS-Konstruktor steht ein Pattern und eine Variable */
/*uh, 17.10.90*/           /* das Pattern wird normal uebersetzt, dann folgt ein Zwischencode und */
/*uh, 17.10.90*/           /* dann wird die Variable normal uebersetzt. */
/*uh, 17.10.90*/
/*uh, 17.10.90*/           int arg1=READSTACK(S_e);
/*uh, 17.10.90*/
/*uh, 17.10.90*/           if (T_PM_IN(arg1)) { /* IN Construkt */
/*uh, 17.10.90*/              PPOPSTACK(S_e);
/*uh, 17.10.90*/              arg1=POPSTACK(S_e); /* skips */
/*uh, 17.10.90*/              charset=(T_PTD)POPSTACK(S_e);
/*uh, 17.10.90*/              PUSHSTACK(S_e,arg1);
                              /* GEN_INSTR1(I_ENDSUBL,"endsubl",lastlevel-BT_ITEM_SIZE+BTPTR); */
/*uh, 17.10.90*/           }
/*uh, 17.10.90*/
/*uh, 17.10.90*/           if (T_POINTER(arg1) && IS_DOTS(arg1)) {
/*uh, 17.10.90*/             int rett; /* ... oder .+ aus dem Weg */
/*uh, 17.10.90*/
/*uh, 17.10.90*/             rett=POPSTACK(S_e);
/*uh, 17.10.90*/             x = POPSTACK(S_m); /* Umgebender LIST-Konstruktor */
/*uh, 17.10.90*/            /*  px = (T_PTD)(binding++); HAHAHAH */
                             px = (T_PTD) VAL_INT((STACKELEM)R_NAME(*(T_PTD)READSTACK(S_e),ptn));
                      
/*uh, 17.10.90*/             PPOPSTACK(S_e); /* die Variable */
/*uh, 17.10.90*/
/*uh, 17.10.90*/             PUSHSTACK(S_m,btlevel);
/*uh, 17.10.90*/             PUSHSTACK(S_m,px);       /* binding # */
/*uh, 17.10.90*/             PUSHSTACK(S_m,charset);
/*uh, 17.10.90*/             PUSHSTACK(S_m,rett);
/*uh, 17.10.90*/             PUSHSTACK(S_m,x);
/*uh, 17.10.90*/             PUSHSTACK(S_e,rett);
/*uh, 17.10.90*/             goto main_e;
/*uh, 17.10.90*/           }
/*uh, 17.10.90*/
/*uh, 17.10.90*/           if (T_POINTER(arg1) && IS_LAST(arg1)) {
/*uh, 17.10.90*/             int rett;  /* ... oder .+ aus dem Weg */
/*uh, 17.10.90*/             /* Die Stelle, an der gerade gematcht wird, muss nun gemerkt werden. */
/*uh, 17.10.90*/             /* Sie ist der Anfang der spaeter zu bindenden Teilliste des Arguments. */
/*uh, 17.10.90*/             lastlevel -= BT_ITEM_SIZE;
/*uh, 17.10.90*/             ENCLOSING_LIST;
/*uh, 17.10.90*/             GEN_INSTR2(I_STARTSUBL,"startsubl",
/*uh, 17.10.90*/                        lastlevel+BTDESC,lastlevel+BTSTART);
/*uh, 17.10.90*/
/*uh, 17.10.90*/             /* Das Merken des Endes darf erst nach erfolgreichem Matchen der Restliste */
/*uh, 17.10.90*/             /* Durchgefuehrt werden. Das AS wird daher entfernt und ein LAST... */
/*uh, 17.10.90*/             /* unter den Listen-Dscriptor gelegt, um die entsprechende */
/*uh, 17.10.90*/             /* Abschlussbehandlung zu signalisieren. */
/*uh, 17.10.90*/
/*uh, 17.10.90*/             rett=POPSTACK(S_e);
/*uh, 17.10.90*/             x = POPSTACK(S_m); /* Umgebender LIST-Konstruktor */
/*uh, 17.10.90*/             /* px = (T_PTD)(binding++); */
                             px = (T_PTD) VAL_INT((STACKELEM)R_NAME(*(T_PTD)READSTACK(S_e),ptn));
/*uh, 17.10.90*/             PPOPSTACK(S_e); /* die Variable */
/*uh, 17.10.90*/
/*uh, 17.10.90*/             PUSHSTACK(S_m,lastlevel);
/*uh, 17.10.90*/             PUSHSTACK(S_m,px); /* binding # */
/*uh, 17.10.90*/             PUSHSTACK(S_m,charset);
/*uh, 17.10.90*/             PUSHSTACK(S_m,rett);
/*uh, 17.10.90*/             PUSHSTACK(S_m,x);
/*uh, 17.10.90*/             PUSHSTACK(S_e,rett);
/*uh, 17.10.90*/             goto main_e;
/*uh, 17.10.90*/           }
/*uh, 17.10.90*/
/*uh, 17.10.90*/           /* Normales Pattern im AS-Konstrukt */
/*uh, 17.10.90*/           PUSHSTACK(S_m,x);
/*uh, 17.10.90*/           goto main_e;
/*uh, 17.10.90*/        }
/*uh, 17.10.90*/
/*uh, 17.10.90*/        if (T_PM_IN(x)) { /* IN Construkt */
/*uh, 17.10.90*/           x=POPSTACK(S_e); /* skips */
/*uh, 17.10.90*/           charset=(T_PTD)POPSTACK(S_e);
/*uh, 17.10.90*/           PUSHSTACK(S_e,x);
                           lastlevel-=BT_ITEM_SIZE;
                           GEN_INSTR2(I_STARTSUBL,"startsubl",lastlevel+BTDESC,lastlevel+BTSTART);
                           /* GEN_INSTR1(I_ENDSUBL,"endsubl",lastlevel+BTPTR); */
/*uh, 17.10.90*/           goto main_e;
/*uh, 17.10.90*/        }
/*uh, 17.10.90*/
/*uh, 17.10.90*/        POST_MORTEM("ear: unexpected CONSTRUCTOR in patern!");
/*uh, 17.10.90*/      } /* T_CON(x) */
/*uh, 17.10.90*/
/*uh, 17.10.90*/
/*uh, 17.10.90*/      /* single item */
/*uh, 17.10.90*/      if (T_PM_SKIP(x)) {
/*uh, 17.10.90*/         if (charset) { /* restriktion fuer . vorhanden */
/*uh, 17.10.90*/           MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/           MARK(undecided,3); PUSH_L(undecided);
/*uh, 17.10.90*/           INC_REFCNT(charset);
/*uh, 17.10.90*/           GEN_INSTR3_UHS2(I_MATCHIN,"matchin",(int)charset,"fail",(int)fail,"undecided",(int)undecided);
                           if (ExtendedCodeFile)
                             printelements(codefp, charset, 0);
/*uh, 17.10.90*/           charset=0;
/*uh, 17.10.90*/         }
/*uh, 17.10.90*/         goto main_m;
/*uh, 17.10.90*/      }
/*uh, 17.10.90*/      if (T_PM_PT_PLUS(x)) post_mortem("ear_e: .+ entdeckt!");
/*uh, 17.10.90*/
/*uh, 17.10.90*/
/*uh, 17.10.90*/      /* Je nach Typ von x eine unterschiedliche MATCHC Instruktion erzeugen */

/* neu: check auf fiese Konstanten (analog zu rprint.c) (RS 2.3.1995) */

   switch (x&~F_EDIT)
   {
     case CLASS:
     case TYPE:
     case CL_FUNCTION:
     case CL_SCALAR:
     case CL_VECTOR:
     case CL_TVECTOR:
     case CL_MATRIX:
     case CL_SET:
     case CL_TREE:
       post_mortem("ear: constant not expected in pattern !");
     default:
     }

/*uh, 17.10.90*/
/*uh, 17.10.90*/      if (T_INT(x)) { /* tagged integer */
/*uh, 17.10.90*/         MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/         MARK(undecided,3); PUSH_L(undecided);
/*uh, 17.10.90*/      /* GEN_INSTR3_UH(I_MATCHINT,"matchint",VAL_INT(x),"fail",fail,"undecided",undecided); */
/*uh, 17.10.90*/         GEN_INSTR3_UHS(I_MATCHINT,"matchint",x,"fail",(int)fail,"undecided",(int)undecided);
/*uh, 17.10.90*/      } else
/*uh, 17.10.90*/      if ((x==SA_TRUE) || (x==SA_FALSE)) { /* boolean */
/*uh, 17.10.90*/         MARK(fail,1)     ; PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/         MARK(undecided,2); PUSH_L(undecided);
/*uh, 17.10.90*/         /* GEN_INSTR3_UH(I_MATCHBOOL,"matchbool",x,"fail",fail,"undecided",undecided); */
/*uh, 17.10.90*/         if (x == SA_TRUE) {
/*uh, 17.10.90*/           GEN_INSTR2L(I_MATCHTRUE,"matchtrue","fail",(int)fail,"undecided",(int)undecided);
/*uh, 17.10.90*/         } else {
/*uh, 17.10.90*/           GEN_INSTR2L(I_MATCHFALSE,"matchfalse","fail",(int)fail,"undecided",(int)undecided);
/*uh, 17.10.90*/         }
/*uh, 17.10.90*/      } else {
/*uh, 17.10.90*/         MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/         MARK(undecided,3); PUSH_L(undecided);
/*uh, 17.10.90*/         /* GEN_INSTR3_UH(I_MATCHPRIM,"matchprim", FUNC_INDEX(x),"fail",fail,"undecided",undecided); */
/*uh, 17.10.90*/         GEN_INSTR3_UHS(I_MATCHPRIM,"matchprim",x&~F_EDIT,"fail",(int)fail,"undecided",(int)undecided);
/*uh, 17.10.90*/      }
/*uh, 17.10.90*/      goto main_m;
/*uh, 17.10.90*/
/*uh, 17.10.90*/    } /* within_pattern */
/*uh, 17.10.90*/
/*uh, 17.10.90*/  /* VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV alter Code VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV */
/*uh, 17.10.90*/
#endif /* UH_ZF_PM */


#if WITHTILDE
  if T_TILDE(x) {

    if (no_code)
      goto main_m;

    /* Der Praeprozessor legt die Tildeargumente */
    /* in umgekehrter Reihenfolge ab. Deswegen   */
    /* ist die Indizierung einfacher             */
    i = ARG_INDEX(VALUE(x));

    /* argumentframe freizugeben? */
    if (NO_MORE_TILDEVARS()) {                       /* argumente koennen  */
      i = VALUE(POPSTACK(S_e)) - 1;                  /* freigegeben werden */
      /* folgt return? */
#if 0
      if (i == 0 && END_OF_BODY()) {
        /* return folgt, optimierung */
#if D_MESS
  if (D_M_C_RTM_T)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTM_T);
#endif
	GEN_INSTR0(I_RTM_T,"rtm_t");
        gen_return = 0;
      }
      else {
	if (tilde_sub) {
#if D_MESS
  if (D_M_C_MOVE_TR)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE_TR);
#endif
	  GEN_INSTR0(I_MOVE_TR,"move_tr");        /* optimierung    */ 
          }
	else {
#if D_MESS
  if (D_M_C_MOVE_TW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE_TW);
#endif
	  GEN_INSTR0(I_MOVE_TW,"move_tw");        /* optimierung    */
          }
      }
      nfree_r++;
      FREE_INSTR_R(i,nfree_r);
#else
      if (i == 0) {
	if (tilde_sub) {
#if D_MESS
  if (D_M_C_PUSH_TR0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_TR0);
#endif
	  GEN_INSTR0(I_PUSH_TR0,"push_tr0"); }
	else {
#if D_MESS
  if (D_M_C_PUSH_TW0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_TW0);
#endif
	  GEN_INSTR0(I_PUSH_TW0,"push_tw0"); }
      }
      else {
	if (tilde_sub) {
#if D_MESS
  if (D_M_C_PUSH_TR)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_TR);
#endif
	  GEN_INSTR1(I_PUSH_TR,"push_tr",i); }/* keine Vorzeitige freigabe mah 141293 */
	else {
#if D_MESS
  if (D_M_C_PUSH_TW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_TW);
#endif
	  GEN_INSTR1(I_PUSH_TW,"push_tw",i); }
      }
#endif /* dieser fall hat hier nichts zu suchen und ist nur zu testzwecken hier mah 120193 */
    }
    else {
#if 0
      if (i == 0 || i == nfree_r) {
#else
      if (i == 0) {
#endif
	if (tilde_sub) {
#if D_MESS
  if (D_M_C_PUSH_TR0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_TR0);
#endif
	  GEN_INSTR0(I_PUSH_TR0,"push_tr0"); }
	else {
#if D_MESS
  if (D_M_C_PUSH_TW0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_TW0);
#endif
	  GEN_INSTR0(I_PUSH_TW0,"push_tw0"); }
      }
      else {
	if (tilde_sub) {
#if D_MESS
  if (D_M_C_PUSH_TR)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_TR);
#endif
	  GEN_INSTR1(I_PUSH_TR,"push_tr",i); /* keine Vorzeitige freigabe mah 141293 */
        }
	else {
#if D_MESS
  if (D_M_C_PUSH_TW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_TW);
#endif
	  GEN_INSTR1(I_PUSH_TW,"push_tw",i); }
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

#endif /* WITHTILDE */


  if T_POINTER(x) {                                  /* pointer auf e   */
    desc = (T_PTD)x;                                 /* cast            */
    DEC_REFCNT(desc);                                /* refcount runter */
    if ((R_DESC(*desc,class)) != C_EXPRESSION) {

#if UH_ZF_PM
/*uh, 8.4.91*/  if (R_DESC(*desc,type) == TY_SELECTION) {
/*uh, 8.4.91*/   /* Auf Stack E liegt ein Switch Descriptor. Er enthaelt in seinem PTSE Feld */
/*uh, 8.4.91*/   /* einen Zeiger auf die Liste der Klauseln. Sie muessen jetzt auf die Stacks  */
/*uh, 8.4.91*/   /* gezogen werden, damit die einzelnen Klauseln nacheinander uebersetzt werden  */
/*uh, 8.4.91*/   /* koennen */
/*uh, 8.4.91*/
/*uh, 8.4.91*/   /* args = collect_args(); */
/*uh, 8.4.91*/
 
            /*      px2 = new_case(x,margs+1,targs);  spaetere bearbeitung */

/*uh, 8.4.91*/   l_SELECTION:
/*uh, 8.4.91*/
                 goto l_SELECTION_NEU;
                 }
/*uh, 8.4.91*/  if (R_DESC(*desc,type)==TY_CLAUSE) {
/*uh, 8.4.91*/   /* Die einzelnen Klauseln des CASE-Ausdrucks stehen auf dem Stack E. Es muss */
/*uh, 8.4.91*/   /* nun die oberste Klausel bearbeitet werden. Zuerst muss Code fuer das Pattern */
/*uh, 8.4.91*/   /* erzeugt werden, dann fuer den Guard und dann fuer den Rumpf. */
/*uh, 8.4.91*/   /* Body und Guard werden in den Stack gezogen. */
/*uh, 8.4.91*/   T_PTD bodydesc =(T_PTD)R_CLAUSE(*desc,sons)[2];
/*uh, 8.4.91*/   T_PTD guarddesc=(T_PTD)R_CLAUSE(*desc,sons)[1];
                 clause_nr++;
/*uh, 8.4.91*/
/*uh, 8.4.91*/   PUSH_L(thisclause); /* PM Rueckuebersetzung */
/*uh, 8.4.91*/   thisclause=desc;
/*uh, 8.4.91*/     /* printf("thisclause auf den L-Stack gelegt! depth=%d\n",DEPTH_L()); */
/*uh, 8.4.91*/
/*uh, 8.4.91*/   load_body((STACKELEM **)A_EXPR(*bodydesc,pte),0,0,0);    /* body */
/*uh, 8.4.91*/   load_body((STACKELEM **)A_EXPR(*guarddesc,pte),0,0,0);   /* guard */
/*uh, 8.4.91*/
/*uh, 8.4.91*/   /* Jetzt wird das Pattern geladen, und dies im Flag within_pattern gemerkt. */
/*uh, 8.4.91*/   /* Wenn das Pattern traversiert ist, wird within_pattern wieder zurueckgesetzt. */
/*uh, 8.4.91*/   /* (Auf der M-Linie) */
/*uh, 8.4.91*/   patsub=(T_PTD)R_CLAUSE(*desc,sons)[0];
/*uh, 8.4.91*/
/*uh, 8.4.91*/   load_body((STACKELEM **)A_FUNC(*patsub,pte),0,0,0);
/*uh, 8.4.91*/   within_pattern=TRUE;
/*uh, 8.4.91*/
/*uh, 8.4.91*/   /* ANALYSE des Patterns */
/*uh, 8.4.91*/   START_MODUL("pmanalyze");
/*uh, 8.4.91*/   pattern_depth = btsize = btlevel = lastarbs = bindings = 0;
/*uh, 8.4.91*/   {
/*uh, 8.4.91*/     pattern_info p;
/*uh, 8.4.91*/     PTR_HEAPELEM ptpattern;
/*uh, 8.4.91*/
/*uh, 8.4.91*/     ptpattern=(PTR_HEAPELEM)R_FUNC(*patsub,pte);
/*uh, 8.4.91*/
/*uh, 8.4.91*/  PUSHSTACK(S_e,ptpattern);
/*uh, 8.4.91*/
/*uh, 8.4.91*/     p=pmanalyze(ptpattern+1);  /* Laengenangabe ueberspringen */
/*uh, 8.4.91*/
/*uh, 8.4.91*/     if (p.sublist)  {  /* pattern hat Listenpattern als Teilpattern */
#if DEBUG
/*uh, 8.4.91*/        print_info(p.list_info);
#endif
/*uh, 8.4.91*/        btsize = p.list_info.totalskskips-p.list_info.lastskskips;
/*uh, 8.4.91*/        lastarbs = p.list_info.lastskskips;
/*uh, 8.4.91*/        if (lastarbs+btsize>0) { /* mindestens ein echter ... */
/*uh, 8.4.91*/           GEN_INSTR1(I_MKBTFRAME,"mkbtframe",(lastarbs+btsize)*BT_ITEM_SIZE) ;
/*uh, 8.4.91*/        }
/*uh, 8.4.91*/        pattern_depth = p.list_info.levels;
/*uh, 8.4.91*/     }
/*uh, 8.4.91*/     bindings = p.bindings;
                   DBUG_PRINT("EAR", ("UH-PM: bindings: %i", bindings));
/*uh, 8.4.91*/     nr_additionals += bindings;  /* Frame um Pattern-Variablen erweitert. uh 11 Apr 92 */
                   margs = bindings;
/*uh, 8.4.91*/     /* margs +=bindings; */            /* uh 11 Apr 92 */
/*uh, 8.4.91*/   }
/*uh, 8.4.91*/
/*uh, 8.4.91*/  PPOPSTACK(S_e);
/*uh, 8.4.91*/   END_MODUL("pmanalyze");
/*uh, 8.4.91*/
/*uh, 8.4.91*/
/*uh, 8.4.91*/   binding =  actual_depth = 0;
/*uh, 8.4.91*/
/*uh, 8.4.91*/   x=READSTACK(S_e);
/*uh, 8.4.91*/
/*uh, 8.4.91*/   lastlevel = (lastarbs + btsize) * BT_ITEM_SIZE;
/*uh, 8.4.91*/
/*uh, 8.4.91*/   /* Erzeuge Code am Anfang einer Klausel */
/*uh, 8.4.91*/   if (pattern_depth>0) GEN_INSTR1(I_MKWFRAME,"mkwframe",pattern_depth+2-1);
/*uh, 8.4.91*/
/*uh, 8.4.91*/   /* Wenn Bindungen im Pattern erzeugt werden, dann sollen der entsprechende */
/*uh, 8.4.91*/   /* Stackframe auf dem I_STACK schon jetzt angelegt werden. Er kann dann auch auf */
/*uh, 8.4.91*/   /* einen Rutsch wieder freigegeben werden */
/*uh, 8.4.91*/   if (bindings) GEN_INSTR1(I_MKAFRAME,"mkaframe",bindings);
/*uh, 8.4.91*/
/*uh, 8.4.91*/   /* Markiere das Pattern auf dem M-Stack, damit nach dem Traversieren die  */
/*uh, 8.4.91*/   /* Kontrolle wieder uebernommen werden kann. */
/*uh, 8.4.91*/   PUSHSTACK(S_m,SET_ARITY(PM_WHEN,2));
/*uh, 8.4.91*/   PUSHSTACK(S_m1,DOLLAR);  /* FAIL */
/*uh, 8.4.91*/   PUSH_L(DOLLAR);   /* UNDECIDED */
/*uh, 8.4.91*/   goto main_e;
/*uh, 8.4.91*/  }
#endif /* UH_ZF_PM */

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
  if (D_M_C_PUSH_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_W);
#endif
        GEN_INSTRX(I_PUSH_W,"push_w",x);
      }

#if STORE
  /* we may have imported a precompiled expression that is a function */  
  if (R_DESC(*desc,class) == C_FUNC) {
      if ((args = collect_args()) > 0) {
        /* in funktionsposition, apply instruktion */
#if D_MESS
    if (D_M_C_APPLY)
      GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_APPLY);
#endif
          GEN_INSTR1(I_APPLY,"apply",args);
      }
}
#endif /* STORE */

      goto main_m;
    }
    /* also ein expression pointer */
    switch(R_DESC(*desc,type)) {
          case TY_COND:                                  /* conditional          */
                      args = collect_args();           /* argumente aufsammeln */
                      if (args > 0) {
                        /* conditional in funktionsposition */
                        MAKEDESC(desc,1,C_FUNC,TY_CONDI); /* descriptor anlegen */
                        L_CONDI(*desc,nargs) = margs+1;   /* anz. argumente */
#if WITHTILDE
			/* der args-eintrag wird mitbenutzt, um die */
			/* groesse des tildeframes nachzuhalten     */

			L_CONDI(*desc,args) = (R_CONDI(*desc,args) | (targs << 2));
#endif /* WITHTILDE */
                        L_CONDI(*desc,ptd) = (T_PTD)x; /* original cond */
			if (args == 1 && END_OF_BODY() &&
/* this is new : */      !(R_DESC(*(T_PTD)READSTACK(S_a),type) == TY_GUARD)) {
/* changes for IF-THEN-ELSE in GUARD RS 18.11.1993 */
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
#if WITHTILDE
		      /* die oberen 14 bit werden fuer die groesse des */
		      /* tildeframes benutzt                           */
		      L_CONDI(*desc,args) = (R_CONDI(*desc,args) & 0xfffc); /* untere 2 bit loeschen */
		      L_CONDI(*desc,args) = (R_CONDI(*desc,args) | i);
#else
                      L_CONDI(*desc,args) = i;
#endif
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
#if WITHTILDE
                          load_body((STACKELEM **)A_COND(*px,ptee),i,i,targs);
#else
                          load_body((STACKELEM **)A_COND(*px,ptee),i,i);
#endif /* WITHTILDE */
                          PUSHSTACK(S_e,HASH);             /* trennsymbol */
#if WITHTILDE
                          load_body((STACKELEM **)A_COND(*px,ptte),i,i,targs);
#else
                          load_body((STACKELEM **)A_COND(*px,ptte),i,i);
#endif /* WITHTILDE */
                        }
                        else {
#if D_MESS
  if (D_M_C_JTRUE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTRUE);
#endif
                          GEN_INSTRL(I_JTRUE,"jtrue","cond",desc);
#if WITHTILDE
                          load_body((STACKELEM **)A_COND(*px,ptte),i,i,targs);
#else
                          load_body((STACKELEM **)A_COND(*px,ptte),i,i);
#endif
                          PUSHSTACK(S_e,HASH);             /* trennsymbol */
#if WITHTILDE
                          load_body((STACKELEM **)A_COND(*px,ptee),i,i,targs);
#else
                          load_body((STACKELEM **)A_COND(*px,ptee),i,i);
#endif /* WITHTILDE */
                        }
                      }
                      else {
#if D_MESS
  if (D_M_C_JCOND)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JCOND);
#endif
                        GEN_INSTRL(I_JCOND,"jcond","cond",desc);
#if WITHTILDE
                        load_body((STACKELEM **)A_COND(*px,ptee),i,i,targs);
#else
                        load_body((STACKELEM **)A_COND(*px,ptee),i,i);
#endif /* WITHTILDE */
                        PUSHSTACK(S_e,HASH);             /* trennsymbol */
#if WITHTILDE
                        load_body((STACKELEM **)A_COND(*px,ptte),i,i,targs);
#else
                        load_body((STACKELEM **)A_COND(*px,ptte),i,i);
#endif /* WITHTILDE */
                      }
                      PUSHSTACK(S_m,SET_ARITY(COND,2)); /* cond konstruktor */
                      PUSHSTACK(S_m,DOLLAR);
                      /* nfree = 0; */
                      goto main_e;
                    }
                    else {
                      /* conditional nicht in funktionsposition */
#if WITHTILDE
		      x = new_cond(x,margs+1,targs);
		      L_CONDI(*(T_PTD)x,args) = (R_CONDI(*(T_PTD)x,args) & 0xfffc); /* untere 2 bit loeschen */
		      L_CONDI(*(T_PTD)x,args) = (R_CONDI(*(T_PTD)x,args) | FREE_ARGS);
#else
                      x = new_cond(x,margs+1);       /* spaetere bearbeitung */
                      L_CONDI(*(T_PTD)x,args) = FREE_ARGS;
#endif /* WITHTILDE */
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
  if (D_M_C_PUSH_AW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_AW);
#endif
                          GEN_INSTR1(I_PUSH_AW,"push_aw",ARG_INDEX(arity));
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
  if (D_M_C_MOVE_AW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE_AW);
#endif
                          GEN_INSTR0(I_MOVE_AW,"move_aw");
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
  if (D_M_C_PUSH_AW0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_AW0);
#endif
                          GEN_INSTR0(I_PUSH_AW0,"push_aw0");
                        }
                      }
#if D_MESS
  if (D_M_C_PUSH_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_W);
#endif
                      GEN_INSTRX(I_PUSH_W,"push_w",x);
#if WITHTILDE
                      if ((margs > 0) || (targs > 0)){
#else
		      if (margs > 0) {
#endif /* WITHTILDE */

#if D_MESS
  if (D_M_C_MKCCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKCCLOS);
#endif
#if WITHTILDE
                        GEN_INSTR3(I_MKCCLOS,"mkcclos",margs,margs+1,targs);
#else
                        GEN_INSTR2(I_MKCCLOS,"mkcclos",margs,margs+1);
#endif /* WITHTILDE */

                      }
                    }
                    goto main_m;
                    /* end case TY_COND */
#if WITHTILDE				    
      case TY_SNSUB:
                    DBUG_PRINT ("rear", ("behandle TY_SNSUB"));
		    args_t = collect_args();           /* tilde-argumente */
		      /* es werden nur dann keine Argumente */
		      /* gefunden, wenn durch Optimierung   */
		      /* eines Def im Lifter ein sub~ ohne  */
		      /* snap erzeugt wurde                 */
		    argsneeded_t = R_FUNC(*desc, nargs);
		      /* argsneeded unterscheided sich nur  */
                      /* im oben genannten Fall von der     */
                      /* Anzahl der vorhandenen Argumente   */
                    DBUG_PRINT("rear", ("args_t: %d, argsneeded_t: %d", args_t, argsneeded_t));
		    if (args_t == argsneeded_t) {
		      PTR_HEAPELEM lookout; 

#if TARG_FIX
                      /* check for T_AP_1 and T_DAP_1, Application and Dapplication with counter 1 */
                      /* T_SNAP_1 have been collected for args_t above. no nested T_SNAP_1s ??     */
#if D_DIST            
                      for( i=0; T_AP_1(READSTACK(S_m)) || T_DAP_1(READSTACK(S_m)); i++ ) PUSHSTACK(S_hilf,POPSTACK(S_m));
#else     
                      for( i=0; T_AP_1(READSTACK(S_m)) ; i++ ) PUSHSTACK(S_hilf,POPSTACK(S_m));
#endif
                      PUSHSTACK(S_m,TAG_INT(targs));    /* groesse des aktuellen tilde frames */
                      INC_REFCNT(desc);                 /* auf M-Stack sichern, cr 25.03.96   */
                      PUSHSTACK(S_m,desc);              /* in Ermangelung eines passenden Stackelementes */
                                                        /* pointer auf TY_SNSUB darueberlegen */
                      for( ; i > 0  ; i-- ) PUSHSTACK(S_m,POPSTACK(S_hilf));
#endif /* TARG_FIX */
		      tilde_sub = 0;
		      targs = args_t; /* neuer tilde frame */
		      DBUG_PRINT ("EAR", ("neuer Tilde-Frame: %d", targs));

		      /* wie sieht das erste Element des */
		      /* Rumpfes aus?                    */
		      lookout = R_FUNC(*desc,pte);
		      lookout++;
		      
		      px = (T_PTD)*lookout;
		      if (T_POINTER((int)px)) {
			if (R_DESC(*px,type) == TY_SUB) {
			  /* sub~ sub- Regel */
#if 0
			  load_expr((STACKELEM **)A_FUNC(*desc, pte));
#else
			  load_body((STACKELEM **)A_FUNC(*desc, pte),margs,0,targs);
#endif

			  goto main_e;
			}
			else
			  if (R_DESC(*px,type) == TY_LREC_IND) {
			    /* gelifteter Funktionsaufruf */
			    load_body((STACKELEM **)A_FUNC(*desc, pte),margs,0,targs);
			    goto main_e;
			  }
			  else
			    if ((R_DESC(*px,type) == TY_SWITCH)
				|| (R_DESC(*px,type) == TY_MATCH)) {
			      /* geliftetes Patternmatch */
			      load_body((STACKELEM **)A_FUNC(*desc, pte),margs,0,targs);
			      goto main_e;
			    }
                            else
#if UH_ZF_PM
                              if (R_DESC(*px,type) == TY_SELECTION) {
                                /* geliftetes UH-Patternmatch */
                                load_body((STACKELEM **)A_FUNC(*desc, pte),margs,0,targs);
                                goto main_e;
                                } else
#endif
			      POST_MORTEM("ear: sub~: unexpected descriptor type");
		      } /* T_POINTER */
		      else {
			/* closed lrec (with goal expression)  */
			/* in funktionsposition ?              */
			ap_to_def = collect_args();

			/* no debruijn-indices in goal expression */
			x = new_sub(x,0,argsneeded_t);

			argsneeded = 0;

			/* now a little flag that tells l_comb to generate a gamma */
			/* in case of a closed lrec definition and a gammabeta in  */
			/* case of a closed function call                          */

			if (CloseGoal)
			  closed_lrec = 1;

                        temp_targs = targs;
			goto L_comb;
		      }
		    }
		    else
		      POST_MORTEM("rear: sub~: args != argsneeded");

#endif /* WITHTILDE */
      case TY_SUB:                                   /* lambda-funktion      */
                    DBUG_PRINT ("rear", ("Behandle TY_SUB"));
                    args = collect_args();           /* argumente aufsammeln */
                    argsneeded = R_FUNC(*desc,nargs);
#if WITHTILDE
		    x = new_sub(x, argsneeded, targs);
                    temp_targs = targs;
#else
                    x = new_sub(x,argsneeded);       /* spaetere bearbeitung */
#endif /* WITHTILDE */
                    goto L_comb;
      case TY_LREC_IND:                              /* definierte funktion  */
                    args = collect_args();           /* argumente aufsammeln */
                    /* analyse des funktionsrumpfes */
                    px = (T_PTD)R_LREC_IND(*desc,ptf);
		    if ((T_POINTER((int)px)) && ((R_DESC(*px,type)) == TY_SUB)) {
		      argsneeded = R_FUNC(*px,nargs);
		    }
#if UH_ZF_PM
/*uh, 11.4.92*/ /*    else */
/*uh, 11.4.92*/   /*    if ((T_POINTER((int)px)) && ((R_DESC(*px,type)) == TY_SELECTION)) */
/*uh, 11.4.92*/     /*     argsneeded=1; */
#endif /* UH_ZF_PM */
                    else 
		      argsneeded = 0;

#if WITHTILDE
                    temp_targs = R_LREC(*(T_PTD)R_LREC_IND(*desc,ptdd),ntilde); /* argsneeded_t = ??? */

                    DBUG_PRINT("EAR", ("new argsneeded_t: %d", argsneeded_t));
                    
                    if (argsneeded_t == 0) /* no snap~ sub ~ */
                      x = new_func(x,argsneeded,temp_targs);
                    else
                      x = new_func(x,argsneeded,argsneeded_t); 
#else
                    x = new_func(x,argsneeded);      /* spaetere bearbeitung */
#endif
                  L_comb:
                    /* argumentframe freizugeben? */
		    if (NO_MORE_VARIABLES()) {        /* argumente koennen  */
		      i = VALUE(POPSTACK(S_e));       /* freigegeben werden */
		      FREE_INSTR(i,nfree);
		    }

#if WITHTILDE
		      /* befindet man sich in der Compiliationsregel */
		      /* fuer S[ sub~ sub e ], dann muessen statt    */
		      /* PUSH_W (alt: PUSH) PUSH_R erzeugt werden    */
		      /* und tail-end-spruenge koennen nicht opti-   */
		      /* miert werden.                               */

		    if (argsneeded_t > 0){
		      if (args >= argsneeded) { 
			if (LazyLists && listcnt > 0) {
#if D_MESS
  if (D_M_C_PUSH_R)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_R);
#endif
			  GEN_INSTRX(I_PUSH_R,"push_r",x);
#if D_MESS
  if (D_M_C_MKGACLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKGACLOS);
#endif
			  GEN_INSTR3(I_MKGACLOS,"mkgaclos",argsneeded,argsneeded,args_t);
			}
			else {
			  /* keine gamma-tail-end-optimierungen */
			  /* gamma und beta mit ausreichend vielen argumenten */
			  /* also wird beides ausgefuehrt                     */

			    if ((((args > 0) || (argsneeded == 0)) && (!ap_to_def))
				&& (!closed_lrec)) { 
			      /* closed_lrec is 1 with CloseGoal lifting method */
			      /* when generating code for a def (not a closed   */
			      /* function call )                                */

			      /* ap~ sub~ steht in Funktionposition und */
			      /* es folgt ein sub. beides zusammen:     */

			      /* argsneeded = 0 ist der sonderfall eines*/
			      /* gelifteten aufrufs einer parameterlosen*/
			      /* funktion                               */
#if D_MESS
  if (D_M_C_GAMMABETA)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_GAMMABETA);
#endif
			      GEN_INSTRL(I_GAMMABETA,"gammabeta","func",x);
			      /* die bedingung muss immer wahr sein ! */
			      if (NO_MORE_TILDEVARS()) {        /* argumente koennen  */
				i = VALUE(POPSTACK(S_e));       /* freigegeben werden */
				if (i == 1)
				  GEN_INSTR0(I_FREE1SW_T,"free1sw_t");
				else {
#if D_MESS
  if (D_M_C_FREESW_T)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREESW_T);
#endif
				  GEN_INSTR1(I_FREESW_T,"freesw_t",i);
                                }
				nfree_r += i;
			      }
			      else {
				if (argsneeded_t == 1)
				  GEN_INSTR0(I_FREE1SW_T,"free1sw_t");
				else {
#if D_MESS
  if (D_M_C_FREESW_T)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREESW_T);
#endif
                                  GEN_INSTR1(I_FREESW_T,"freesw_t",argsneeded_t);
                                }
				nfree_r += argsneeded_t;
			      }
			    }
			    else {
			      /* ap~ sub~ steht nicht in Funktionsposition */
#if D_MESS
  if (D_M_C_GAMMA)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_GAMMA);
#endif
			      GEN_INSTRL(I_GAMMA,"gamma","func",x);
			      args = ap_to_def;
			      ap_to_def = 0;

                              if (CloseGoal)
				closed_lrec = 0;

			    }
			}
			if (args > argsneeded)
			  /* zuviele argumente fuer sub, apply instruktion */
			  if (LazyLists && listcnt > 0) {
#if D_MESS
  if (D_M_C_MKGACLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKGACLOS);
#endif
			    GEN_INSTR3(I_MKGACLOS,"mkgaclos",args,argsneeded,args_t);
			    if (NO_MORE_TILDEVARS())                /* gammaclosure kann tildeframe poppen */
			      i = VALUE(POPSTACK(S_e));
			  }
			  else {
#if D_MESS
  if (D_M_C_APPLY)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_APPLY);
#endif
			    GEN_INSTR1(I_APPLY,"apply",args-argsneeded);
			  }
		      }
		      else { /* args < argsneeded */
#if D_MESS
  if (D_M_C_PUSH_R)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_R);
#endif
			GEN_INSTRX(I_PUSH_R,"push_r",x);
			if ((args > 0) || (args_t > 0)) {
			  /* in funktionsposition, gamma closure bilden */
			  /* gamma closure holt sich den funktionszeiger*/
			  /* von r. sonst wie beta closure              */
#if D_MESS
  if (D_M_C_MKGACLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKGACLOS);
#endif
			  GEN_INSTR3(I_MKGACLOS,"mkgaclos",args,argsneeded,args_t); /* args_t = argsneeded_t */
			  if (NO_MORE_TILDEVARS())                    /* gammaclosure kann tildeframe poppen */
			    i = VALUE(POPSTACK(S_e));
			}
		      }
		      /* argsneeded_t hier nur noch flag */
		      argsneeded_t = 0; 
		      if (NO_MORE_TILDEVARS()) {        /* argumente koennen  */
			i = VALUE(POPSTACK(S_e));       /* freigegeben werden */
			FREE_INSTR_R(i,nfree_r);
		      }
		      goto main_m;
		    }
		    else /* argsneeded_t == 0: C[ sub e ] */
#endif /* WITHTILDE */
                    if (args >= argsneeded) {
                      /* ausreichend viele argumente */

                      if (LazyLists && listcnt > 0) {
#if D_MESS
  if (D_M_C_PUSH_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_W);
#endif
                        GEN_INSTRX(I_PUSH_W,"push_w",x);
#if D_MESS
  if (D_M_C_MKBCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKBCLOS);
#endif
#if WITHTILDE
                        GEN_INSTR3(I_MKBCLOS,"mkbclos",argsneeded,argsneeded, temp_targs);
#else
                        GEN_INSTR2(I_MKBCLOS,"mkbclos",argsneeded,argsneeded);
#endif /* WITHTILDE */
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
                            {
                              GEN_INSTRL(I_BETAQ0,"betaq0","func",x);

#ifdef CAF_UPDATE
                              if (R_COMB(*(T_PTD)x,args) == 0)  
                                GEN_INSTRL(I_UPDATE,"update","func",x);
                                /* result of beta-call does not depend on */
                                /* environment, update code for next call */
#endif /* CAF_UPDATE */
                            }
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
  if (D_M_C_PUSH_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_W);
#endif
                      GEN_INSTRX(I_PUSH_W,"push_w",x);
#if WITHTILDE
                      if ((args > 0) || (temp_targs > 0)) {
#else
                      if (args > 0) {
#endif /* WITHTILDE */
                        /* in funktionsposition, closure bilden */
#if D_MESS
  if (D_M_C_MKBCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKBCLOS);
#endif
#if WITHTILDE
                        GEN_INSTR3(I_MKBCLOS,"mkbclos",args,argsneeded, temp_targs);
#else
                        GEN_INSTR2(I_MKBCLOS,"mkbclos",args,argsneeded);
#endif /* WITHTILDE */
                      }
                    }
#if WITHTILDE
                    argsneeded_t = 0;
#endif /* WITHTILDE */
                    goto main_m;
                    /* end case TY_COMB */
      case TY_EXPR:                                  /* expression pointer */
                    /* argumentframe freizugeben? */
                    if (NO_MORE_VARIABLES()) {
                      /* argumente koennen freigegeben werden */
                      i = VALUE(POPSTACK(S_e));
                      FREE_INSTR(i,nfree);
                    }
#if WITHTILDE
                    /* Der Rumpf einer parameterlosen geliftete Funktion  */
		    /* ist vom Typ TY_EXPR, aber es muessen hier die      */
		    /* Tildevariablen freigegeben werden koennen          */

                    load_body((STACKELEM **)A_EXPR(*desc,pte),0,0,targs);
#else
                    load_expr((STACKELEM **)A_EXPR(*desc,pte));
#endif
                    goto main_e;

#if WITHTILDE
       case TY_MATCH:
                    /* when [match list] guard [guard expr] do [body expr] */
#endif /* WITHTILDE */
      case TY_SWITCH:

                    /* case                                       */
		    /* when [match list] guard [expr] do [expr]   */
		    /*   .                                        */
                    /*   .                                        */
		    /*   .                                        */
		    /* end / otherwise [expr]                     */

#if UH_ZF_PM
l_SELECTION_NEU:
                    /* oldswitch=R_CLAUSE(*R_SELECTION(*(T_PTD)x,clauses),sons)[3]; */
#endif

                    args = collect_args();
#if WITHTILDE
                    x = new_case(x,margs+1,targs);       /* spaetere bearbeitung */
#if UH_ZF_PM
                    oldswitch=(T_PTD)R_CLAUSE(*R_SELECTION(*(T_PTD)R_CASE(*(T_PTD)x,ptd),clauses),sons)[3];
                    if (args >= (int) R_SWITCH(*(T_PTD)oldswitch,anz_args)) {
#else
                    if (args >= (int) R_SWITCH(*R_CASE(*(T_PTD)x,ptd),anz_args)) {
#endif
#else
                    x = new_case(x,margs+1);
                    if (args >= (int) R_SWITCH(*R_CASE(*(T_PTD)x,ptd),case_type)) {
#endif /* WITHTILDE */
                               /* auf int umgecastet von RS 04/11/92 */ 
                      /* case in funktionsposition und ausreichend viele argumente */
                     DBUG_PRINT ("rear", ("case in funktionsposition und ausreichend viele argumente"));
#if WITHTILDE
                      if (argsneeded_t > 0) {

			if (NO_MORE_VARIABLES()) {
			  /* argumente koennen freigegeben werden */
			  i = VALUE(POPSTACK(S_e));
			  FREE_INSTR(i,nfree);
			}
#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("2: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))   /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT((1<<(margs-nfree))-1)); }
              /* updating S_pm used-stackelements-entry : RS 22.2.1994 for CASE => abandon optimization */
#endif /* D_DIST */

#if D_MESS
  if (D_M_C_GAMMACASE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_GAMMACASE);
#endif
			GEN_INSTRL(I_GAMMACASE,"gammacase","case",x);

			if (NO_MORE_TILDEVARS()) {        /* argumente koennen  */
			  i = VALUE(POPSTACK(S_e));       /* freigegeben werden */
			  if (i == 1)
			    GEN_INSTR0(I_FREE1SW_T,"free1sw_t");
			  else {
#if D_MESS
  if (D_M_C_FREESW_T)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREESW_T);
#endif
			    GEN_INSTR1(I_FREESW_T,"freesw_t",i);
                          }
			  nfree_r += i;
			}
			else {
			  if (argsneeded_t == 1)
			    GEN_INSTR0(I_FREE1SW_T,"free1sw_t");
			  else {
#if D_MESS
  if (D_M_C_FREESW_T)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREESW_T);
#endif
			    GEN_INSTR1(I_FREESW_T,"freesw_t",argsneeded_t);
                          }
			  nfree_r += argsneeded_t;
			}
		      }
		      else {
			if (NO_MORE_VARIABLES()) {
			  /* argumente koennen freigegeben werden */
			  i = VALUE(POPSTACK(S_e));
			  FREE_INSTR(i,nfree);
			}
#if D_MESS
  if (D_M_C_CASE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_CASE);
#endif

#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("2: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))   /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT((1<<(margs-nfree))-1)); }
              /* updating S_pm used-stackelements-entry : RS 22.2.1994 for CASE => abandon optimization */
#endif /* D_DIST */

                        GEN_INSTRL(I_CASE,"case","case",x);
		      }
#else
#if D_DIST
        if ((margs-nfree) < 32) {
/*          DBUG_PRINT ("rear", ("2: pusharg...margs=%d, nfree=%d, actual=%d\n", margs, nfree, nfree)); */
          if ((SIZEOFSTACK(S_pm) > 0) && (*((INSTR *)((int) (READSTACK(S_pm)) >> 4)) == I_DISTB))   /* while ear something on pm-Stack => dap */
            UPDATESTACK(S_pm, 1, TAG_INT((1<<(margs-nfree))-1)); }
              /* updating S_pm used-stackelements-entry : RS 22.2.1994 for CASE => abandon optimization */
#endif /* D_DIST */

                      GEN_INSTRL(I_CASE,"case","case",x);
                      if (NO_MORE_VARIABLES()) {
                        /* argumente koennen freigegeben werden */
                        i = VALUE(POPSTACK(S_e));
                        FREE_INSTR(i,nfree);
                      }
#endif /* WITHTILDE */
#if WITHTILDE
#if UH_ZF_PM
                      i = (int) R_SWITCH(*oldswitch,anz_args);
#else
                      i = (int) R_SWITCH(*R_CASE(*(T_PTD)x,ptd),anz_args);
#endif
#else
                      /* NCASE: bei multiplen pattern zusaeztliche argumente */
                      i = R_CASE(*(T_PTD)x,nargs) - (R_CASE(*(T_PTD)x,args) ? margs : 0);
                      DBUG_PRINT ("rear", ("args: %i, i: %i, nargs: %i, args: %i, margs: %i", 
                                 args, i, R_CASE(*(T_PTD)x,nargs),R_CASE(*(T_PTD)x,args),margs));
#endif /* WITHTILDE */
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
                      DBUG_PRINT ("rear", ("case erhaelt nicht ausreichend viele argumente, bzw. ist nicht in funktionsposition"));

#if WITHTILDE
		      if (NO_MORE_VARIABLES()) {
			/* argumente koennen freigegeben werden */
			i = VALUE(POPSTACK(S_e));
			FREE_INSTR(i,nfree);
		      }
		      if (argsneeded_t > 0) {
#if D_MESS
  if (D_M_C_PUSH_R)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_R);
#endif
			  GEN_INSTRX(I_PUSH_R,"push_r",x);
#if D_MESS
  if (D_M_C_MKGSCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKGSCLOS);
#endif
                          DBUG_PRINT("rear",("mkgsclos problem mit uh-pm !"));
#if UH_ZF_PM
                          GEN_INSTR3(I_MKGSCLOS,"mkgsclos",args,R_SWITCH(*(T_PTD)R_CLAUSE(*(T_PTD)R_SELECTION(*R_CASE(*(T_PTD)x,ptd),clauses),sons)[3],anz_args),targs); 
#else
			  GEN_INSTR3(I_MKGSCLOS,"mkgsclos",args,R_SWITCH(*R_CASE(*(T_PTD)x,ptd),anz_args),targs); /* NCASE 09.09.92 */
#endif /* UH_ZF_PM */
			  if (NO_MORE_TILDEVARS())      /* gsclosure frees tilde frame */
			    i = VALUE(POPSTACK(S_e)); 
			}
		      else
			if ((args > 0) || (targs > 0)) {
#if D_MESS
  if (D_M_C_PUSH_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_W);
#endif
			  GEN_INSTRX(I_PUSH_W,"push_w",x);
#if D_MESS
  if (D_M_C_MKSCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKSCLOS);
#endif
                          DBUG_PRINT("rear",("mkgsclos problem 2 !"));
#if UH_ZF_PM
                          GEN_INSTR3(I_MKSCLOS,"mksclos",args,R_SWITCH(*(T_PTD)R_CLAUSE(*(T_PTD)R_SELECTION(*R_CASE(*(T_PTD)x,ptd),clauses),sons)[3],anz_args),targs); 
#else
			  GEN_INSTR3(I_MKSCLOS,"mksclos",args,R_SWITCH(*R_CASE(*(T_PTD)x,ptd),anz_args),targs); /* NCASE 09.09.92 */
#endif /* UH_ZF_PM */
			}
			else
#if D_MESS
  if (D_M_C_PUSH_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_W);
#endif
			  GEN_INSTRX(I_PUSH_W,"push_w",x);

		      if (NO_MORE_TILDEVARS()) {        /* argumente koennen  */
			i = VALUE(POPSTACK(S_e));       /* freigegeben werden */
#if D_MESS
  if (D_M_C_FREE_R)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE_R);
#endif
			GEN_INSTR1(I_FREE_R,"free_r",i);
			nfree_r += i;
		      }
#else

DBUG_PRINT ("rear", ("args: %d, margs: %d, R_CASE(*(T_PTD)x,args): %d,  R_CASE(*(T_PTD)x,nargs): %d", args, margs, R_CASE(*(T_PTD)x,args),  R_CASE(*(T_PTD)x,nargs)));

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
  if (D_M_C_PUSH_AW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_AW);
#endif
                          GEN_INSTR1(I_PUSH_AW,"push_aw",ARG_INDEX(arity));
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
  if (D_M_C_MOVE_AW)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MOVE_AW);
#endif
                          GEN_INSTR0(I_MOVE_AW,"move_aw");
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
  if (D_M_C_PUSH_AW0)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_AW0);
#endif
                          GEN_INSTR0(I_PUSH_AW0,"push_aw0");
                        }
#if D_MESS
  if (D_M_C_PUSH_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_W);
#endif
                        GEN_INSTRX(I_PUSH_W,"push_w",x);
                        /* GEN_INSTR2(I_MKSCLOS,"mksclos",margs,margs+1); */

#if D_MESS
  if (D_M_C_MKSCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKSCLOS);
#endif
#if WITHTILDE
                        GEN_INSTR3(I_MKSCLOS,"mksclos",margs,R_CASE(*(T_PTD)x,nargs),targs);  /* NCASE 09.09.92 */
#else
                        GEN_INSTR2(I_MKSCLOS,"mksclos",margs,R_CASE(*(T_PTD)x,nargs));  /* NCASE 09.09.92 */
#endif /* WITHTILDE */
                      }
                      else {

DBUG_PRINT ("rear", ("Ich bin leider hier...."));

#if D_MESS
  if (D_M_C_PUSH_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSH_W);
#endif
                        GEN_INSTRX(I_PUSH_W,"push_w",x);

DBUG_PRINT ("rear", ("i have (T_AP(READSTACK(S_m)) = %d, ARITY(READSTACK(S_m))) = %d", T_AP(READSTACK(S_m)), ARITY(READSTACK(S_m))));

/* RS 11.5.1993 */
/*                      if (T_AP(READSTACK(S_m)) && (ARITY(READSTACK(S_m)))) */

#if D_MESS
  if (D_M_C_MKSCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKSCLOS);
#endif

#if WITHTILDE
                        GEN_INSTR3(I_MKSCLOS,"mksclos",args,R_CASE(*(T_PTD)x,nargs),targs);
#else
                        GEN_INSTR2(I_MKSCLOS,"mksclos",args,R_CASE(*(T_PTD)x,nargs));
#endif
/* end of RS 11.5.1993 */

DBUG_PRINT ("rear", ("Jetzt bin ich vorbei !"));

                      }
#endif /* WITHTILDE */
                    }
#if WITHTILDE
                    /* argsneeded_t is used to determine wether */
                    /* a function call is closed or not.        */
                    argsneeded_t = 0;
#endif /* WITHTILDE */

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
/*uh, 16.7.90*/               PPOPSTACK(S_m); PPOPSTACK(S_m1); /* Applikator entfernen. */
/*uh, 16.7.90*/               PPOPSTACK(S_m); /* ZFINT fuer Rumpf entfernen. */
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
/*uh, 16.7.90*/                  GEN_INSTR1(I_MAKEBOOL,"makebool",(int)cdesc);
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
#if WITHTILDE
      no_code = 0;                                 /* Ende eines POST_SNAP */ 
#endif /* WITHTILDE */
      goto main_e;                                   /* startausdruck      */
    }

    if T_SNAP(x) {
#if WITHTILDE
      tilde_sub = 1;

      PUSHSTACK(S_m, x);    /* normal Constructor-Pushes */
      PUSHSTACK(S_m1, x);
      goto main_e;
#else /* !WITHTILDE */
      PUSHSTACK(S_m ,SET_ARITY(AP,ARITY(x)));
      PUSHSTACK(S_m1,READSTACK(S_m));
      goto main_e;
#endif /* WITHTILDE */
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
   
    DBUG_PRINT ("rear", ("Number of Expressions not to distribute: %d !", d_nodistnr));

    if ((ARITY(x)-d_nodistnr) >= 2) {         /* arity-test for distribution */
     DBUG_PRINT ("ear", ("Insert PUSHH for distribute...."));
#if D_MESS
  if (D_M_C_PUSHH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHH);
#endif

     DBUG_PRINT ("rear", ("letpar started with PUSHH"));

     GEN_INSTR1(I_PUSHH, "pushh", ARITY(x)-1);                   /* insert pushh */

/* for ((margs-nfree) < 32) insert distb, otherwise dist !!!! */

     if ((margs-nfree) > 31)
        fprintf(codefp, "#addr 0x%08x\n", (int)CODEPTR);             /* for debug only */
     else
        fprintf(codefp, "#addr 0x%08x\n", (int)CODEPTR);

     if ((margs-nfree) > 31)       /* more than 31 arguments in frame ? So dist ! */
#if D_MESS
{ if (D_M_C_DIST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DIST);
  if (D_M_C_PROC_DISTSTACKS)
    GEN_INSTR1(I_MSDISTCK, "msdistck", (margs-nfree));
#endif 
#if WITHTILDE
       GEN_INSTRD6(I_DIST, "dist", 0, 0, (margs-nfree), nfree, (targs-NFREE_R), NFREE_R);     /* insert dist , all arguments dummy entries except of (margs-nfree) */
#else
       GEN_INSTRD4(I_DIST, "dist", 0, 0, (margs-nfree), nfree);     /* insert dist , all arguments dummy entries except of (margs-nfree) */
#endif /* WITHTILDE */
#if D_MESS
}
#endif
     else
#if D_MESS
{ if (D_M_C_DISTB)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DISTB);
  if (D_M_C_PROC_DISTSTACKS)
    GEN_INSTR1(I_MSDISTCK, "msdistck", (margs-nfree));
#endif
#if WITHTILDE
       GEN_INSTRD6(I_DISTB, "distb", 0, 0, (margs-nfree), nfree, (targs-NFREE_R), NFREE_R); 
       DBUG_PRINT ("REAR", ("DISTB: margs %d nfree %d targs %d NFREE_R %d", margs, nfree, targs, NFREE_R));
#else
       GEN_INSTRD4(I_DISTB, "distb", 0, 0, (margs-nfree), nfree);     /* insert distb , all arguments dummy entries except of (margs-nfree) */
#endif /* WITHTILDE */
#if D_MESS
}
#endif

#if WITHTILDE
     *(CODEPTR-6) = (INSTR) CODEPTR;                             /* updating first argument: label to code for distribute */
#else
     *(CODEPTR-4) = (INSTR) CODEPTR;                             /* updating first argument: label to code for distribute */
#endif

     GEN_LABEL("distarg", CODEPTR);

     if ((margs-nfree) < 32) {
       PUSHSTACK (S_pm, TAG_INT(nfree));
       PUSHSTACK (S_pm, TAG_INT(0)); }            /* mark here used arguments */

#if WITHTILDE
     PUSHSTACK (S_pm, (STACKELEM) ( (int)(CODEPTR-7) << 4));                  /* pushing update-address in S_pm */
#else
     PUSHSTACK (S_pm, (STACKELEM) ( (int)(CODEPTR-5) << 4));                  /* pushing update-address in S_pm */
#endif
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

  if (T_FUNC(x) || T_IAFUNC(x)) {    /* primitive funktion oder Interaktion  */
                         /* Unterscheidung von FUNC und IAFUNC  stt 11.09.95 */
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
  if (D_M_C_PUSHC_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC_W);
#endif
        GEN_INSTRN(I_PUSHC_W,"pushc_w",x);
      }
      goto main_m;                                   /* weiter auf m   */
    }
    /* in funktionsposition (args > 0) */
    if (args < argsneeded) {
      /* zuwenig argumente, push instruktion */
#if D_MESS
  if (D_M_C_PUSHC_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC_W);
#endif
      GEN_INSTRN(I_PUSHC_W,"pushc_w",x);
      /* closure bilden */
#if D_MESS
  if (D_M_C_MKDCLOS)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKDCLOS);
#endif
      if (T_FUNC(x)) /* primitive Funktion */
        GEN_INSTR2(I_MKDCLOS,"mkdclos",args,argsneeded);
      else            /* Interaktion */
        GEN_INSTR2(I_MKICLOS,"mkiclos",args,argsneeded);
      goto main_m;                                     /* weiter auf m  */
    }
    else
    if (T_IAFUNC(x))      /* interaction */
      GEN_INSTRN(I_INTACT, "intact", x);
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
  else if (T_PM_END(x) && T_STRUCT(READSTACK(S_m)))
  {
    /* END not needed , cr 13.11.95 */
    goto main_m;
  }
  else {
#if D_MESS
  if (D_M_C_PUSHC_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC_W);
#endif
    GEN_INSTRN(I_PUSHC_W,"pushc_w",x);
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
/*uh, 16.7.90*/     PPOPSTACK(S_m);
/*uh, 16.7.90*/     if (ARITY(x)==1) {
/*uh, 16.7.90*/       /* Ein Filter */
/*uh, 16.7.90*/       /* Filter postcode */
/*uh, 16.7.90*/       /* Generieren und aufloesen von Label FI */
/*uh, 16.7.90*/       fi_desc2 = (T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       fi_desc1 = (T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       FORWARDRESOLVE("fi",fi_desc1);
/*uh, 16.7.90*/       FORWARDRESOLVE("fi",fi_desc2);
/*uh, 16.7.90*/       PPOPSTACK(S_e);  /* Trennsymbol */
/*uh, 16.7.90*/       zfd=(T_PTD)POPSTACK(S_m);
/*uh, 16.7.90*/       vars=(PTR_HEAPELEM)POPSTACK(S_m);
/*uh, 16.7.90*/       zfc=(PTR_HEAPELEM)POPSTACK(S_m);
/*uh, 16.7.90*/       goto main_m;
/*uh, 16.7.90*/     }
/*uh, 16.7.90*/     if (ARITY(x)==2) {
/*uh, 16.7.90*/       /* Also Stelligkeit 2, ein Generator */
/*uh, 16.7.90*/       /* Generator postcode */
#if D_MESS
  if (D_M_C_FREE1_A)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_FREE1_A);
#endif
/*uh, 16.7.90*/       GEN_INSTR0(I_FREE1_A,"free1_a");
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
/*uh, 16.7.90*/       GEN_INSTR1(I_MAKEZF,"makezflist",(int)cdesc);
/*uh, 16.7.90*/       FORWARDMARK(end_desc);
/*uh, 16.7.90*/       GEN_INSTRL(I_JUMP,"jump","end",end_desc);
/*uh, 16.7.90*/       /* Generieren und aufloesen von Label ROF */
/*uh, 16.7.90*/       FORWARDRESOLVE("rof",rof_desc);
/*uh, 16.7.90*/       GEN_INSTR0(I_DROPP,"dropp");
/*uh, 16.7.90*/       /* Generieren und aufloesen von Label END */
/*uh, 16.7.90*/       FORWARDRESOLVE("end",end_desc);
/*uh, 16.7.90*/       PPOPSTACK(S_e);  /* Trennsymbol */
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

/*uh, 17.10.90*/   /* Konstruktor auf dem M-Stack */
/*uh, 17.10.90*/   if (T_PM_WHEN(x)) {
/*uh, 17.10.90*/   /* Hier kann eingehakt werden, falls nach beendigung des Patterns des Guards oder des */
/*uh, 17.10.90*/   /* Bodys noch spezielle Aktionen ausgefuehrt werden sollen. */
/*uh, 17.10.90*/      gen_return=0;
/*uh, 17.10.90*/      within_pattern = FALSE;
/*uh, 17.10.90*/      arity=ARITY(x);
/*uh, 17.10.90*/      switch (arity) {
/*uh, 17.10.90*/         case 2: /* Nach dem Pattern */
/*uh, 17.10.90*/                 /* if (pattern_depth>0)  */
/*uh, 17.10.90*/                 /*   GEN_INSTR1(I_RMWFRAME,"rmwframe",pattern_depth+2); */
/*uh, 17.10.90*/                 /* if (btsize>0) */
/*uh, 17.10.90*/                 /*   GEN_INSTR1(I_RMBTFRAME,"rmbtframe",btsize*BT_ITEM_SIZE); */
/*uh, 17.10.90*/                 /* Konstanten guard TRUE weg-optimieren */
/*uh, 17.10.90*/                 if (T_SA_TRUE(READSTACK(S_e))) { /* kein Guard-Code erzeugen */
/*uh, 17.10.90*/                    PPOPSTACK(S_e);
/*uh, 17.10.90*/                    arity--;
/*uh, 17.10.90*/                    guard=FALSE;
/*uh, 17.10.90*/                 } else {
/*uh, 17.10.90*/                    guard=TRUE;
/*uh, 17.10.90*/                    break;
/*uh, 17.10.90*/                 }
/*uh, 17.10.90*/         case 1: /* Nach dem Guard */
/*uh, 17.10.90*/                 if (guard) { T_PTD ncase; 
/*uh, 17.10.90*/                   /* MARK(fail,1)     ; PUSHSTACK(S_m1,fail); */
/*uh, 17.10.90*/                 /*  MARK(undecided,3); PUSH_L(undecided); */

                                   MAKEDESC(ncase,1,C_FUNC,TY_CASE);
                                   L_CASE(*ncase,ptc)=NULL;
                                   L_CASE(*ncase,args)=R_CASE(*(T_PTD)READSTACK(S_a),args);
                                   L_CASE(*ncase,ptd)=R_CASE(*(T_PTD)READSTACK(S_a),ptd);
                                   L_CASE(*ncase,nargs)=margs+1; 
                                   L_CASE(*ncase,ptc)=(PTR_HEAPELEM)TAG_INT(clause_nr);
                 
                                 /*  GEN_INSTR3_UH(I_JBOOL,"jbool",(int) (margs-nfree),"fail",(int)fail,"case",(int)ncase); */
                                 GEN_DESCI(ncase,"FUNC","CASE",R_CASE(*(T_PTD)READSTACK(S_a),args),margs+1,R_CASE(*ncase,ptd),"when",(int)R_CASE(*ncase,ptc)); 

                                 MARK(fail,1)     ; PUSHSTACK(S_m1,fail);

DBUG_PRINT ("MOIN", ("targs: %d, temp_targs: %d", targs, temp_targs));

                                 GEN_INSTR5_RS(I_TGUARD,"tguard","fail",(int)fail,(int)ncase,(int)((pattern_depth>0) ? pattern_depth+2 : 0), (int) ((lastarbs+btsize>0) ? (lastarbs+btsize)*BT_ITEM_SIZE : 0),(int) (margs-nfree));
/*uh, 17.10.90*/                   /* GEN_INSTR2L(I_JBOOL,"jbool","fail",(int)fail,"undecided",(int)undecided); */
/*uh, 17.10.90*/                 }
/*uh, 17.10.90*/                 if (pattern_depth>0)
/*uh, 17.10.90*/                    GEN_INSTR1(I_RMWFRAME,"rmwframe",pattern_depth+2);
/*uh, 17.10.90*/                 if (lastarbs+btsize>0)
/*uh, 17.10.90*/                    GEN_INSTR1(I_RMBTFRAME,"rmbtframe",(lastarbs+btsize)*BT_ITEM_SIZE);
/*uh, 17.10.90*/                 GEN_INSTR0(I_DROP,"drop");
/*uh, 17.10.90*/                 break;
/*uh, 17.10.90*/         case 0: /* Nach dem Body */
/*uh, 17.10.90*/                 /* if (bindings) GEN_INSTR1(I_FREE,"free",bindings); */
                                 DBUG_PRINT ("rear", ("uh-pm 1: margs %i, nfree %i", margs, nfree));
/*uh, 17.10.90*/                 if (margs-nfree) GEN_INSTR1(I_FREE_A,"free_a", (margs-nfree)); /* uh 11.4.92 */
/*uh, 17.10.90*/                 GEN_INSTR0(I_RTF,"rtf");
/*uh, 17.10.90*/                 PPOPSTACK(S_m);
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 /* Aufloesen der UNDECIDED Label */
/*uh, 17.10.90*/                 while (!T_DOLLAR(READ_L())) {
/*uh, 17.10.90*/                    undecided = (T_PTD)POP_L();
/*uh, 17.10.90*/                    FORWARDRESOLVE("undecided",undecided);
/*uh, 17.10.90*/                 }
/*uh, 17.10.90*/                 POP_L(); /* Dollar */
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 /* printf("Dollar vom Stack genommen depth=%d\n",DEPTH_L()); */
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 { T_PTD ncase/* ,nsel,otherwise*/;
/*uh, 17.10.90*/                   /* PTR_HEAPELEM pth; */
/*uh, 17.10.90*/
/*uh, 17.10.90*/                   if (pattern_depth>0)
/*uh, 17.10.90*/                     GEN_INSTR1(I_RMWFRAME,"rmwframe",pattern_depth+2);
/*uh, 17.10.90*/                   if (lastarbs+btsize>0)
/*uh, 17.10.90*/                     GEN_INSTR1(I_RMBTFRAME,"rmbtframe",(lastarbs+btsize)*BT_ITEM_SIZE);
                                   DBUG_PRINT ("rear", ("uh-pm 2: margs %i, nfree %i", margs, nfree));
/*uh, 17.10.90*/                   if (margs-nfree) 
/*uh, 17.10.90*/                     GEN_INSTR1(I_FREE_A,"free_a",(margs-nfree)); 
/*uh, 17.10.90*/
/*uh, 17.10.90*/                   MAKEDESC(ncase,1,C_FUNC,TY_CASE);
/*uh, 17.10.90*/                   L_CASE(*ncase,ptc)=NULL;
                                   L_CASE(*ncase,args)=R_CASE(*(T_PTD)READSTACK(S_a),args);
/*uh, 17.10.90*/                 /*  L_CASE(*ncase,ptd)=R_CLAUSE(*R_SELECTION(*R_CASE(*(T_PTD)READSTACK(S_a),ptd),clauses),sons)[3]; */ /* nsel; */
                                   L_CASE(*ncase,ptd)=R_CASE(*(T_PTD)READSTACK(S_a),ptd);
                                   L_CASE(*ncase,ptc)=(PTR_HEAPELEM)TAG_INT(clause_nr);
/*uh, 17.10.90*/                   L_CASE(*ncase,nargs)=margs+1;
/*uh, 17.10.90*/
                                   GEN_DESCI(ncase,"FUNC","CASE",R_CASE(*(T_PTD)READSTACK(S_a),args),margs+1,R_CASE(*ncase,ptd),"when",R_CASE(*ncase,ptc));
/*uh, 17.10.90*/                   GEN_INSTRX(I_MKCASE,"mkcase",ncase);
/*uh, 17.10.90*/                 }
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 GEN_INSTR0(I_RTF,"rtf");
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 /* Aufloesen der BT-Label */
/*uh, 17.10.90*/                 PUSH_L(DOLLAR); /* markieren */
/*uh, 17.10.90*/                 if (guard) {
/*uh, 17.10.90*/                   undecided = (T_PTD)POPSTACK(S_m1); /* Sprung hinter dem Guard */
/*uh, 17.10.90*/                   PUSH_L((STACKELEM)undecided);
/*uh, 17.10.90*/                 }
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 for (i=btsize; i>0; i--) {  /* fuer jede BT Ebene */
/*uh, 17.10.90*/                   while (!T_DOLLAR(READSTACK(S_m1))) {
/*uh, 17.10.90*/                      fail = (T_PTD)POPSTACK(S_m1);
/*uh, 17.10.90*/                      FORWARDRESOLVE("fail",fail);
/*uh, 17.10.90*/                   }
/*uh, 17.10.90*/                   PPOPSTACK(S_m1); /* DOLLAR */
/*uh, 17.10.90*/                   smallfit=POPSTACK(S_m1);
/*uh, 17.10.90*/                   charset=(T_PTD)POPSTACK(S_m1);
/*uh, 17.10.90*/
/*uh, 17.10.90*/                   fail=(T_PTD)POPSTACK(S_m1); /* Label fuer Rueckwaerts-Sprung */
/*uh, 17.10.90*/                   btlevel -= BT_ITEM_SIZE;
/*uh, 17.10.90*/                   GEN_INSTR1(I_RESTOREBT,"restorebt",btlevel);
/*uh, 17.10.90*/
/*uh, 17.10.90*/                   if (charset) { T_PTD fail,undecided;
/*uh, 17.10.90*/                     GEN_INSTR0(I_DEREF,"dereference");
/*uh, 17.10.90*/                     MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/                     MARK(undecided,3); PUSH_L(undecided);
/*uh, 17.10.90*/                     INC_REFCNT(charset);
/*uh, 17.10.90*/                     GEN_INSTR3_UHS2(I_MATCHIN,"matchin",(int)charset,"fail",(int)fail,"fail",(int)undecided);
                                     if (ExtendedCodeFile)
                                       printelements(codefp, charset, 0);
/*uh, 17.10.90*/                     charset=0;
/*uh, 17.10.90*/                   }
/*uh, 17.10.90*/
/*uh, 17.10.90*/                   if (smallfit)
/*uh, 17.10.90*/                     GEN_INSTR1(I_ADVANCE,"advance",1);
/*uh, 17.10.90*/                   else
/*uh, 17.10.90*/                     GEN_INSTR1(I_ADVANCE,"advance",-1);
/*uh, 17.10.90*/
/*uh, 17.10.90*/                   GEN_INSTRL(I_JUMP,"jump","bt",fail);
/*uh, 17.10.90*/                   BACKWARDRESOLVE(fail);
/*uh, 17.10.90*/                 }
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 /* Alle BT Stuecke sind aufgeloest */
/*uh, 17.10.90*/                 /* Zusaetzliche FAIL Spruenge auf M1 */
/*uh, 17.10.90*/                 while ( !T_DOLLAR(READ_L()) ) {
/*uh, 17.10.90*/                    fail = (T_PTD)POP_L();
/*uh, 17.10.90*/                    PUSHSTACK(S_m1,(STACKELEM)fail);
/*uh, 17.10.90*/                 }
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 if (!T_DOLLAR(POP_L()))  /* Dollar */
/*uh, 17.10.90*/                    post_mortem("ear: DOLLAR expected on Label-Stack L");
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 thisclause=(T_PTD)POP_L();  /* PM Rueckuebesetzung */
/*uh, 17.10.90*/
/*uh, 17.10.90*/                 /* aufloesen der FAIL Label */
/*uh, 17.10.90*/                 while (!T_DOLLAR(READSTACK(S_m1))) {
/*uh, 17.10.90*/                    fail = (T_PTD)POPSTACK(S_m1);
/*uh, 17.10.90*/                    FORWARDRESOLVE("fail",fail);
/*uh, 17.10.90*/                 }
/*uh, 17.10.90*/                 if (pattern_depth>0)
/*uh, 17.10.90*/                   GEN_INSTR1(I_RMWFRAME,"rmwframe",pattern_depth+2);
/*uh, 17.10.90*/                 if (lastarbs+btsize>0)
/*uh, 17.10.90*/                   GEN_INSTR1(I_RMBTFRAME,"rmbtframe",(lastarbs+btsize)*BT_ITEM_SIZE);
                                 DBUG_PRINT ("rear", ("uh-pm 3: bindings %i", bindings));
/*uh, 17.10.90*/                 if (bindings)
/*uh, 17.10.90*/                   GEN_INSTR1(I_FREE_A,"free_a",bindings);
/*uh, 17.10.90*/                 PPOPSTACK(S_m1);  /* Dollar */
/*uh, 17.10.90*/                 margs -=bindings;  /* uh 11Apr92 */
/*uh, 17.10.90*/                 nr_additionals=0;  /* uh 11Apr92 */
/*uh, 17.10.90*/                 bindings=0;  /* uh 6Apr92 */
/*uh, 17.10.90*/                 goto main_m;
/*uh, 17.10.90*/      }
/*uh, 17.10.90*/      WRITE_ARITY(S_m,arity-1);
/*uh, 17.10.90*/      goto main_e;
/*uh, 17.10.90*/   }
/*uh, 17.10.90*/   if (T_LIST(x) && within_pattern) {
/*uh, 17.10.90*/     if (ARITY(x)>1) {
/*uh, 17.10.90*/       /* weitere Listenelemente bearbeiten */
/*uh, 17.10.90*/       GEN_INSTR1(I_ADVANCE,"advance",1);
/*uh, 17.10.90*/       GEN_INSTR0(I_DEREF,"dereference");
/*uh, 17.10.90*/       WRITESTACK(S_m,DEC(x));
/*uh, 17.10.90*/       goto main_e;
/*uh, 17.10.90*/     }
/*uh, 17.10.90*/     GEN_INSTR1(I_RESTPTR,"restoreptr",actual_depth+1);
/*uh, 17.10.90*/     actual_depth--;
/*uh, 17.10.90*/     PPOPSTACK(S_m);
                     PPOPSTACK(S_tilde);
/*uh, 17.10.90*/     goto main_m;
/*uh, 17.10.90*/   }
/*uh, 17.10.90*/   if (T_PRELIST(x) && within_pattern) {   /* Listenteile rueckwaerts abarbeiten */
/*uh, 17.10.90*/     if (ARITY(x)>1) {
/*uh, 17.10.90*/       /* weitere Listenelemente bearbeiten */
/*uh, 17.10.90*/       GEN_INSTR1(I_ADVANCE,"advance",-1);
/*uh, 17.10.90*/       GEN_INSTR0(I_DEREF,"dereference");
/*uh, 17.10.90*/       WRITESTACK(S_m,DEC(x));
/*uh, 17.10.90*/       goto main_e;
/*uh, 17.10.90*/     }
/*uh, 17.10.90*/
/*uh, 17.10.90*/     /* Arity <=1 */
/*uh, 17.10.90*/
/*uh, 17.10.90*/     PPOPSTACK(S_m);
                     PPOPSTACK(S_tilde);
/*uh, 17.10.90*/
/*uh, 17.10.90*/     /* Alle Elemente der Patternliste bearbeitet. Abschlusscode */ /* divided by RS April 1995 */
/*uh, 17.10.90*/     if (T_POINTER(READSTACK(S_m)) && IS_LAST(READSTACK(S_m))) {
/*uh, 17.10.90*/       /* Auf der M-Linie wird ein last... oder ein last.+ gefunden. Es muss also ein */
/*uh, 17.10.90*/       /* AS ... (.+) Var Konstrukt an letzter ... Position aufgetreten sein. */
/*uh, 17.10.90*/       /* Der Anfang der gematchten Teilliste im Argument wird bei Ausfuehrung durch */
/*uh, 17.10.90*/       /* Die bereits generierte Instruktion startsublist markiert. Nun soll eine */
/*uh, 17.10.90*/       /* Instruktion zum markieren des Endes entstehen. */
                       if (!charset) GEN_INSTR1(I_ENDSUBL,"endsubl",lastlevel+BTPTR);
                      /* else
                         GEN_INSTR1(I_ENDSUBL,"endsubl",lastlevel+BTSTART); */
                       charset = (T_PTD)MIDSTACK(S_m,1); }

/*uh, 17.10.90*/       if (charset) {  /* von ... ueberdeckte Elemente pruefen */
                          GEN_INSTR1(I_ENDSUBL,"endsubl",lastlevel+BTPTR); 
/*uh, 17.10.90*/          BACKWARDMARK("inloop",loop);
/*uh, 17.10.90*/          GEN_INSTR1(I_ADVANCE,"advance",-1);
/*uh, 17.10.90*/          MARK(cont,2); GEN_INSTR2LA(I_ATSTART,"atstart",lastlevel+BTSTART,"cont",(int)cont);
/*uh, 17.10.90*/          GEN_INSTR0(I_DEREF,"dereference");
/*uh, 17.10.90*/          MARK(fail,2)     ; PUSHSTACK(S_m1,fail);
/*uh, 17.10.90*/          MARK(undecided,3); PUSH_L(undecided);
/*uh, 17.10.90*/          INC_REFCNT(charset);
/*uh, 17.10.90*/          GEN_INSTR3_UHS2(I_MATCHIN,"matchin",(int)charset,"fail",(int)fail,"undecided",(int)undecided);
                          if (ExtendedCodeFile)
                            printelements(codefp, charset, 0);
/*uh, 17.10.90*/          GEN_INSTRL(I_JUMP,"jump","inloop",loop);
/*uh, 17.10.90*/          BACKWARDRESOLVE(loop);
/*uh, 17.10.90*/          FORWARDRESOLVE("cont",cont);
/*uh, 17.10.90*/          charset=0;
/*uh, 17.10.90*/       }

/*uh, 17.10.90*/     /* Alle Elemente der Patternliste bearbeitet. Abschlusscode */
/*uh, 17.10.90*/   /*  if (T_POINTER(READSTACK(S_m)) && IS_LAST(READSTACK(S_m))) */
                   /*  if (!charset) GEN_INSTR1(I_ENDSUBL,"endsubl",lastlevel+BTPTR); */

/*uh, 17.10.90*/
/*uh, 17.10.90*/     /*}*/
/*uh, 17.10.90*/
/*uh, 17.10.90*/     GEN_INSTR1(I_RESTPTR,"restoreptr",actual_depth+1);
/*uh, 17.10.90*/     actual_depth--;
/*uh, 17.10.90*/     goto main_m;
/*uh, 17.10.90*/   }
/*uh, 17.10.90*/   if (T_PM_SWITCH(x)) {
/*uh, 17.10.90*/     post_mortem("EAR_M: SWITCH occured");
/*uh, 17.10.90*/     /* Klausel beendet */
/*uh, 17.10.90*/     if (ARITY(x)>1) {
/*uh, 17.10.90*/       /* weiter Klauseln */
/*uh, 17.10.90*/       WRITESTACK(S_m,DEC(x));
/*uh, 17.10.90*/       goto main_e;
/*uh, 17.10.90*/     }
/*uh, 17.10.90*/     /* gesamter Case-Ausdruck beendet */
/*uh, 17.10.90*/     PPOPSTACK(S_m);
/*uh, 17.10.90*/
/*uh, 17.10.90*/     if (T_POINTER(READSTACK(S_m)) &&
/*uh, 17.10.90*/         R_DESC(*(T_PTD)READSTACK(S_m),type) == TY_CONDI) {
/*uh, 17.10.90*/         cont=(T_PTD)POPSTACK(S_m);
/*uh, 17.10.90*/         FORWARDRESOLVE("cond",cont);
/*uh, 17.10.90*/     }
/*uh, 17.10.90*/     goto main_m;
/*uh, 17.10.90*/   }
/*uh, 17.10.90*/   if (T_PM_AS(x)) {
/*uh, 17.10.90*/     switch (ARITY(x)) {
/*uh, 17.10.90*/       case 2: /* Das Pattern des AS-Konstrukts ist traversiert worden. */
/*uh, 17.10.90*/               /* Nun muss die Zwischensequenz traversiert werden, dann die */
/*uh, 17.10.90*/               /* Variable uebersetzt werden */
/*uh, 17.10.90*/               WRITESTACK(S_m,DEC(x));
/*uh, 17.10.90*/
/*uh, 17.10.90*/               if (actual_depth > 0) {
/*uh, 17.10.90*/                 GEN_INSTR0(I_DEREF,"dereference");
/*uh, 17.10.90*/                 goto main_e;
/*uh, 17.10.90*/               }
/*uh, 17.10.90*/               if (pattern_depth > 0) GEN_INSTR1(I_PICK,"pick",pattern_depth+2);
/*uh, 17.10.90*/               goto main_e;
/*uh, 17.10.90*/
/*uh, 17.10.90*/       case 1: /* Nun ist auch die Variable traversiert. Das AS-Konstrukt ist beendet */
/*uh, 17.10.90*/               PPOPSTACK(S_m);
/*uh, 17.10.90*/               goto main_m;
/*uh, 17.10.90*/     }
/*uh, 17.10.90*/   }
/*uh, 17.10.90*/
#ifdef DEBUG
/*uh, 17.10.90*/   if (T_PM_IN(x))
/*uh, 17.10.90*/      post_mortem("ear_m: PM_IN Konstruktor auf M nicht erwartet!");
#endif
#endif /* UH_ZF_PM */

    arity = ARITY(x);                                /* stelligkeit          */
    gen_return = 1;                                  /* return zulassen */

/*************************************************DIST*************************************************/

#if D_DIST         /* end of code generation for distribution */
    if (T_DAP(READSTACK(S_m))) {         /* great, distributed AP (LETPAR) constructor */
      DBUG_PRINT ("EAR", ("T_DAP successfull, arity=%i, d_nodistnr=%i", arity, d_nodistnr));
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

#if WITHTILDE
        if (NFREE_R > ((int) *((INSTR *)yeah+6)))      /* tilde-arguments have been released ? */
           *((INSTR *)yeah+6) = nfree_r -  ((int) *((INSTR *)yeah+6));         /* yo, updating argument 6 for released arguments */
        else
           *((INSTR *)yeah+6) = 0;                                           /* no, reset argument 6 */
#endif

        if (((margs-nfree) < 32) && (*((INSTR *)yeah) == I_DISTB)) {
        *((INSTR *)yeah+3) = VAL_INT(MIDSTACK(S_pm, 1));
        if ((SIZEOFSTACK(S_pm) > 3) && (*((INSTR *)((int) (MIDSTACK(S_pm,3)) >> 4)) == I_DISTB))      /* more than one dap-entry on S_pm ? */
          UPDATESTACK (S_pm, 4, TAG_INT(VAL_INT(MIDSTACK(S_pm, 4)) | (VAL_INT(MIDSTACK(S_pm, 1)) << (VAL_INT(MIDSTACK(S_pm, 2)) - VAL_INT(MIDSTACK(S_pm, 5)) )) )); }  /* updating entry below */

        DBUG_PRINT ("rear", ("UPDATING dist-arguments"));

        /* showing updated dist-arguments */
        if (((margs-nfree) > 31) || (*((INSTR *)yeah) == I_DIST))
#if WITHTILDE
          fprintf(codefp,"#update 0x%08x\n\t dist(distarg_%1x, distend_%1x, %1d, %1d, %1d, %1d);\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3), *((INSTR *)yeah+4), *((INSTR *)yeah+5), *((INSTR *)yeah+6));
#else
          fprintf(codefp,"#update 0x%08x\n\t dist(distarg_%1x, distend_%1x, %1d, %1d);\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4));
#endif
        else
#if WITHTILDE
          fprintf(codefp,"#update 0x%08x\n\t distb(distarg_%1x, distend_%1x, %1d, %1d, %1d, %1d);\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3), *((INSTR *)yeah+4), *((INSTR *)yeah+5), *((INSTR *)yeah+6));
#else
          fprintf(codefp,"#update 0x%08x\n\t distb(distarg_%1x, distend_%1x, %1d, %1d);\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4));
#endif

        GEN_LABEL ("distend", CODEPTR);

        if ((margs-nfree) > 31)
          fprintf(codefp, "#addr 0x%08x\n", (unsigned int)CODEPTR);             /* for debug only */
        else
          fprintf(codefp, "#addr 0x%08x\n", (unsigned int)CODEPTR);

        if ((margs-nfree) > 31)
#if D_MESS
{ if (D_M_C_DIST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DIST);
  if (D_M_C_PROC_DISTSTACKS)
    GEN_INSTR1(I_MSDISTCK, "msdistck", (margs-nfree));
#endif
/* old version: GEN_INSTRD6(I_DIST, "distb", 0, 0, (margs-nfree), nfree, (targs-NFREE_R), NFREE_R); */
/* but the static knowledge of the tilde-frame at the beginning of the letpar must be conserved */

#if WITHTILDE
          GEN_INSTRD6(I_DIST, "dist", 0, 0, (margs-nfree), nfree, *((INSTR *)yeah+5)-NFREE_R, NFREE_R);   
#else
          GEN_INSTRD4(I_DIST, "dist", 0, 0, (margs-nfree), nfree);              /* whow, the next dist ! */ 
#endif
#if D_MESS
}
#endif
        else  
#if D_MESS
{ if (D_M_C_DISTB)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DISTB);
  if (D_M_C_PROC_DISTSTACKS)
    GEN_INSTR1(I_MSDISTCK, "msdistck", (margs-nfree));
#endif

/* old version: GEN_INSTRD6(I_DISTB, "distb", 0, 0, (margs-nfree), nfree, (targs-NFREE_R), NFREE_R); */
/* but the static knowledge of the tilde-frame at the beginning of the letpar must be conserved */

#if WITHTILDE
          GEN_INSTRD6(I_DISTB, "distb", 0, 0, (margs-nfree), nfree, *((INSTR *)yeah+5)-NFREE_R, NFREE_R);   
          DBUG_PRINT ("REAR", ("DISTB 2: margs %d nfree %d targs %d NFREE_R %d", margs, nfree, targs, NFREE_R));
#else
          GEN_INSTRD4(I_DISTB, "distb", 0, 0, (margs-nfree), nfree);              /* whow, the next distb ! */
#endif
#if D_MESS
}
#endif

#if WITHTILDE
        *(CODEPTR-6) = (INSTR) CODEPTR;                                      /* updating argument 1 */
#else
        *(CODEPTR-4) = (INSTR) CODEPTR;                                      /* updating argument 1 */
#endif
        GEN_LABEL("distarg", CODEPTR);                                       /* label for debug */
        hooray=POPSTACK (S_pm);                                                     /* pop old S_pm entry */       

        if ((margs-nfree) < 32) {
          hooray=POPSTACK (S_pm);           /* update underlying entry here... */
          UPDATESTACK (S_pm, 0, TAG_INT(nfree));  /* backup actual nfree here */

          PUSHSTACK (S_pm, TAG_INT(0));  }     /* mark here used arguments */

#if WITHTILDE
        PUSHSTACK (S_pm, (STACKELEM) ((int)(CODEPTR-7) << 4));                           /* push argument 2 address on stack S_pm */
#else
        PUSHSTACK (S_pm, (STACKELEM) ((int)(CODEPTR-5) << 4));                           /* push argument 2 address on stack S_pm */
#endif

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

#if WITHTILDE
        if (NFREE_R > ((int) *((INSTR *)yeah+6)))      /* tilde-arguments have been released ? */
           *((INSTR *)yeah+6) = nfree_r -  ((int) *((INSTR *)yeah+6));        /* yo, updating argument 6 for released arguments */
        else
           *((INSTR *)yeah+6) = 0;              /* no, reset argument 6 */
#endif

        if (((margs-nfree) < 32) && (*((INSTR *)yeah) == I_DISTB)) {
          *((INSTR *)yeah+3) = VAL_INT(MIDSTACK(S_pm, 1));
          if ((SIZEOFSTACK(S_pm) > 3) && (*((INSTR *)((int) (MIDSTACK(S_pm,3)) >> 4)) == I_DISTB))      /* more than one dap-entry on S_pm ? */
            UPDATESTACK (S_pm, 4, TAG_INT(VAL_INT(MIDSTACK(S_pm, 4)) | (VAL_INT(MIDSTACK(S_pm, 1)) << (VAL_INT(MIDSTACK(S_pm, 2)) - VAL_INT(MIDSTACK(S_pm, 5)) )) )); } /* updating underlying entry */

        DBUG_PRINT ("rear", ("UPDATING dist-arguments"));

        if (((margs-nfree) > 31) || (*((INSTR *)yeah) == I_DIST))
#if WITHTILDE
          fprintf(codefp,"#update 0x%08x\n\t dist(distarg_%1x, distend_%1x, %1d, %1d, %1d, %1d);\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1),  *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4), *((INSTR *)yeah+5),  *((INSTR *)yeah+6));
#else
          fprintf(codefp,"#update 0x%08x\n\t dist(distarg_%1x, distend_%1x, %1d, %1d);\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1),  *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4));
#endif
        else
#if WITHTILDE
          fprintf(codefp,"#update 0x%08x\n\t distb(distarg_%1x, distend_%1x, %1d, %1d, %1d, %1d);\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4), *((INSTR *)yeah+5),  *((INSTR *)yeah+6));
#else
          fprintf(codefp,"#update 0x%08x\n\t distb(distarg_%1x, distend_%1x, %1d, %1d);\n", (unsigned int)(INSTR *)yeah, *((INSTR *)yeah+1), *((INSTR *)yeah+2),  *((INSTR *)yeah+3),  *((INSTR *)yeah+4));
#endif

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

        DBUG_PRINT("rear", ("letpar ended with POPH"));
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
    else
    if (T_ENTRY(x)) {                                /* slot-konstruktor  */
      PPOPSTACK(S_m);                                /* weg damit (von m)  */
      x = POPSTACK(S_m1);                            /* und von m1         */
      arity = ARITY(x);                              /* stelligkeit        */
/* noch nicht, cr 09.11.95
#if D_MESS
  if (D_M_C_MKSLOT)
    GEN_INSTR0(I_COUNT, "count", MINDEX_COUNT_MKSLOT);
#endif
*/
      GEN_INSTR0(I_MKSLOT,"mkslot");                 /* stelligkeit = 2 ! */
    }
    else
    if (T_STRUCT(x)) {                               /* frame-konstruktor  */
      PPOPSTACK(S_m);                                /* weg damit (von m)  */
      x = POPSTACK(S_m1);                            /* und von m1         */
      arity = ARITY(x) - 1;                          /* stelligkeit        */
/* noch nicht, cr 09.11.95
#if D_MESS
  if (D_M_C_MKFRAME)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKFRAME);
#endif
*/
      GEN_INSTR1(I_MKFRAME,"mkframe",arity);
    }
    else
#if WITHTILDE
      if (T_POST_SNAP(x)) {
        PPOPSTACK(S_m);                                /* weg damit (von m)  */
        PPOPSTACK(S_m1);                               /* weg damit (von m1) */
      }
      else
#endif /* WITHTILDE */
        /* unerwarteter konstruktor */
        POST_MORTEM("ear: very unexpected constructor on stack m");
    goto main_m;                                     /* weiter auf m       */
  } /* end T_CON */

#if UH_ZF_PM
/*uh, 28.1.91*/
/*uh, 28.1.91*/    if (T_POINTER(x) && IS_LAST(x)) {
/*uh, 28.1.91*/      /* Abschlusscode fuer das Binden der Variablen */
/*uh, 28.1.91*/      PPOPSTACK(S_m);
/*uh, 28.1.91*/      PPOPSTACK(S_m);  /* charset */
/*uh, 28.1.91*/      x = POPSTACK(S_m);  /* level */
/*uh, 28.1.91*/      i = POPSTACK(S_m);  /* last-level */
                  /* if (!smallfit)
                       GEN_INSTR4(I_BINDSUBL,"bindsubl",i+BTDESC,i+BTPTR,i+BTSTART,bindings-x-1);
                     else    */
/*uh, 28.1.91*/        GEN_INSTR4(I_BINDSUBL,"bindsubl",i+BTDESC,i+BTSTART,i+BTPTR,bindings-x-1);
/*uh, 28.1.91*/      goto main_m;
/*uh, 28.1.91*/    }
/*uh, 28.1.91*/
/*uh, 28.1.91*/    if (T_POINTER(x) && IS_DOTS(x)) {
/*uh, 28.1.91*/      PPOPSTACK(S_m);
/*uh, 28.1.91*/      charset=(T_PTD)POPSTACK(S_m);
/*uh, 28.1.91*/      x = POPSTACK(S_m);  /* binding # */
/*uh, 28.1.91*/      i = POPSTACK(S_m);  /* bt level */
                /*   if (!smallfit)
                       GEN_INSTR4(I_BINDSUBL,"bindsubl",i+BTDESC,i+BTPTR,i+BTSTART,bindings-x-1);
                     else   */
/*uh, 28.1.91*/        GEN_INSTR4(I_BINDSUBL,"bindsubl",i+BTDESC,i+BTSTART,i+BTPTR,bindings-x-1);
/*uh, 28.1.91*/      goto main_m;
/*uh, 28.1.91*/    }
/*uh, 28.1.91*/
/*uh, 28.1.91*/    if (T_POINTER(x) && IS_DOTPLUS(x)) {
/*uh, 28.1.91*/       post_mortem("ear_m: .+ auf m");
/*uh, 28.1.91*/    }
#endif /* UH_ZF_PM */

#if (TARG_FIX && WITHTILDE)
  if (T_POINTER(x) && (R_DESC(*(T_PTD)x,type) == TY_SNSUB)) {
    DEC_REFCNT((T_PTD)POPSTACK(S_m));   /* unter dem TY_SNSUB-pointer liegt */
    targs = GET_INT(POPSTACK(S_m));     /* die groesse des alten tilde frames */
    goto main_m;                        /* cr 25.03.96 */
  }
#endif /* TARG_FIX */

  /* also ein DOLLAR oder KLAA oder ? */
#if WITHTILDE
  if (T_DOLLAR(x) || T_DOLLAR_TILDE(x)) {            /* funktion bearbeitet */
    if (gen_return) {
      if (NO_MORE_TILDEVARS()) { /* abschliessendes free_r der funktion mah 131293    */
                                 /* alle vars, die nicht vorzeitig freigegeben wurden */
        i = VALUE(POPSTACK(S_e));
        FREE_INSTR_R(i,nfree_r);
      }
      if T_DOLLAR(x) {
#if D_MESS
  if (D_M_C_RTF)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTF);
#endif
        GEN_INSTR0(I_RTF,"rtf");
      }
      else /* T_DOLLAR_TILDE */  {
#if D_MESS
  if (D_M_C_RTT)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTT);
#endif
        GEN_INSTR0(I_RTT,"rtt"); }
    }/* gen_return */ 

#else /* !WITHTILDE */

    if T_DOLLAR(x) {                                 /* funktion bearbeitet */
      if (gen_return) {
#if D_MESS
  if (D_M_C_RTF)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_RTF);
#endif
      GEN_INSTR0(I_RTF,"rtf");                
    } /* gen_return */

#endif /* WITHTILDE */

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
        /* KEEP_FLAP eliminated by car 24.10.94 */
        /*
        GEN_DESC(desc,"FUNC","CONDI",KEEP_FLAG((int) R_CONDI(*desc,args)),
                 R_CONDI(*desc,nargs),px,"jcond",desc);
        */
        GEN_DESC(desc,"FUNC","CONDI",R_CONDI(*desc,args),
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
#if WITHTILDE
	  /* args ist vom typ COUNT, also 16 bit */
	  {
	    int flag;
	    flag = KEEP_FLAG((int) R_CONDI(*desc,args));

	    L_CONDI(*desc,args) = (R_CONDI(*desc,args) & 0xfffc); /* untere 2 bit loeschen */
	    L_CONDI(*desc,args) = (R_CONDI(*desc,args) | flag);
	  }
#else
          L_CONDI(*desc,args) = KEEP_FLAG((int) R_CONDI(*desc,args));
#endif /* WITHTILDE */
          GEN_LABEL("cont",desc);
        }
        PPOPSTACK(S_e);                              /* trennsymbol HASH   */
        PPOPSTACK(S_a);                              /* CONDI-descriptor  */
        PPOPSTACK(S_m);                              /* COND-konstruktor  */
        /* auf m1 liegt kein COND-konstruktor! */
/* changes for IF-THEN-ELSE in GUARD ! RS 18.11.1993 */
        if (R_DESC(*(T_PTD)READSTACK(S_a),type) == TY_GUARD) 
          gen_return = 1;  /* insert RTF for not optimized IF-THEN_ELSE in GUARD */
        else
          gen_return = 0;          /* darauffolgendes return unterdruecken */

	/* too many arguments applied to conditional */
	/* mah 230294                                */

	if (T_AP_1(READSTACK(S_m))) {
	  int i;
	  i = VALUE(POPSTACK(S_m1)) - 1;
	  PPOPSTACK(S_m);
#if D_MESS
  if (D_M_C_APPLY)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_APPLY);
#endif
	  GEN_INSTR1(I_APPLY,"apply",i);
	  gen_return = 1;
	}

        goto main_m;                                 /* weiter auf m      */
      }
    } /* end T_COND */

#if UH_ZF_PM
/*uh, 23.10.90*/      else if (T_PM_CASE(x)) { /* Pattern Matching CASE */
/*uh, 23.10.90*/        if (ARITY(x)>1) { /* Weitere Klauseln folgen */
/*uh, 23.10.90*/          nfree=VALUE(MIDSTACK(S_m,1)); /* uh 11.4.92 */
/*uh, 23.10.90*/          WRITESTACK(S_m,DEC(x));
/*uh, 23.10.90*/          PUSHSTACK(S_m,DOLLAR);
/*uh, 23.10.90*/          gen_return=1;
/*uh, 23.10.90*/          goto main_e;
/*uh, 23.10.90*/        }
/*uh, 23.10.90*/
/*uh, 23.10.90*/        desc = (T_PTD)READSTACK(S_a);
/*uh, 23.10.90*/
/*uh, 23.10.90*/        /* Abschlusscode fuer das gesamte CASE-Konstrukt */
/*uh, 23.10.90*/      /*   GEN_INSTRL(I_MKCASE,"mkcase","case",thiscase); */
                                   MAKEDESC(uhtmp,1,C_FUNC,TY_CASE);
/*uh, 17.10.90*/                   L_CASE(*uhtmp,ptc)=NULL;
/*uh, 17.10.90*/      /*             L_CASE(*uhtmp,ptd)=R_CLAUSE(*R_SELECTION(*R_CASE(*(T_PTD)desc,ptd),clauses),sons)[3]; */
                                   L_CASE(*uhtmp,ptd)=R_CASE(*(T_PTD)desc,ptd);
/*uh, 17.10.90*/                   L_CASE(*uhtmp,nargs)=margs+1;
                                   L_CASE(*uhtmp,args)=R_CASE(*(T_PTD)READSTACK(S_a),args);
                                   L_CASE(*uhtmp,ptc)=(PTR_HEAPELEM)TAG_INT(clause_nr);

                        GEN_DESCI(uhtmp,"FUNC","CASE",R_CASE(*(T_PTD)READSTACK(S_a),args),margs+1,R_CASE(*uhtmp,ptd),"when",R_CASE(*uhtmp,ptc));
                        GEN_INSTRX(I_MKCASE,"mkcase",uhtmp);
/*uh, 23.10.90*/        GEN_INSTR0(I_RTF,"rtf");
/*uh, 23.10.90*/        gen_return=0;
/*uh, 23.10.90*/
/*uh, 23.10.90*/        /* printf("Vor dem Wegnehmen von thiscase. Depth=%d\n",DEPTH_L()); */
/*uh, 23.10.90*/        thiscase = (T_PTD)POP_L(); /* PM Rueckuebesetzung */
/*uh, 23.10.90*/
/*uh, 23.10.90*/        /* if (COND_FLAG(R_CASE(*desc,args))) { */  /* pushret Label aufloesen */
/*uh, 23.10.90*/        /*  R_CASE(*desc,ptc)[-1] = (INSTR)CODEPTR; */
/*uh, 23.10.90*/        /*   L_CASE(*desc,args)= KEEP_FLAG(R_CASE(*desc,args)); */
/*uh, 23.10.90*/        /*   GEN_LABEL("case",desc); */
/*uh, 23.10.90*/        /* } */
/*uh, 23.10.90*/
/*uh, 23.10.90*/        PPOPSTACK(S_m);  /* nfree uh 11.4.92 */
/*uh, 23.10.90*/        PPOPSTACK(S_m);
/*uh, 23.10.90*/        PPOPSTACK(S_a);
/*uh, 23.10.90*/        gen_return=0;
/*uh, 23.10.90*/        goto main_m;
/*uh, 23.10.90*/      } /* PM_CASE */
#endif /* UH_ZF_PM */

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
#if !WITHTILDE
                  margs = R_COMB(*desc,nargs);       /* benoetigte argumente */
                  nfree = 0;                         /* noch keine arg. frei */
#endif /* !WITHTILDE */
		  PUSHSTACK(S_m,DOLLAR);
                  px = (T_PTD)R_COMB(*desc,ptd);     /* original funktion */
#if WITHTILDE
		  if (R_DESC(*px,type) == TY_LREC_IND) {
		    targs = R_COMB(*desc,args);      /* new tilde frame */
		    px = R_LREC_IND(*px,ptf);
		  }
#else
                  if (R_DESC(*px,type) == TY_LREC_IND)
                    px = R_LREC_IND(*px,ptf);        /* dereferenzieren */
#endif /* WITHTILDE */

#if UH_ZF_PM
/*uh, 11.4.92*/               /*    if (margs==1 && T_POINTER((int)px) && R_DESC(*px,type) == TY_SELECTION) { */

                                    if (T_POINTER((int)px) && R_DESC(*px,type) == TY_SELECTION) {
/*uh, 11.4.92*/
/*uh, 11.4.92*/                         GEN_DESC(desc,"FUNC","COMB",targs,0,R_COMB(*desc,ptd),"func",desc);
/*uh, 11.4.92*/                         L_COMB(*desc,ptc) = CODEPTR;       /* zeiger auf code */
/*uh, 11.4.92*/                         GEN_LABEL("func",desc);
/*uh, 11.4.92*/                         /* GEN_INSTR0(I_MOVE_AW,"move_aw"); */
/*uh, 11.4.92*/                         margs=0;
/*uh, 11.4.92*/                         desc = px;
/*uh, 11.4.92*/                         x = (int)px;
/*uh, 11.4.92*/                         args=1;
/*uh, 11.4.92*/                         goto l_SELECTION;
/*uh, 11.4.92*/                     }
#endif /* UH_ZF_PM */

#if WITHTILDE
                  if (T_POINTER((int)px) && (R_DESC(*px,type) == TY_SNSUB)) {
		    /* Im Falle eines snsub muss die alte Argumentframegroesse */
		    /* margs erhalten bleiben                                  */
		    targs = R_COMB(*desc,args);        /* benoetigte tildeargumente */
                    DBUG_PRINT("rear", ("neues TILDE-Frame : %d", targs));
		    nfree_r = 0;

		    margs = R_COMB(*desc,nargs);
		    nfree = 0;

		    /* dollar_tilde um rtt zu erzeugen statt rtf */
		    WRITESTACK(S_m,DOLLAR_TILDE);       /* trennsymbol auf m   */
		    load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs,targs);
		  }
                  else
#endif /* WITHTILDE */
                    if (T_POINTER((int)px) && (R_DESC(*px,type) == TY_SUB))
		    {
#if WITHTILDE
		      /* Die Tildeframegroesse aendert sich nicht */
		      targs = R_COMB(*desc,args); 
		      margs = R_COMB(*desc,nargs);       /* benoetigte argumente */
		      nfree = 0;                         /* noch keine arg. frei */
		      load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs,targs);
#else /* !WITHTILDE */
                      load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs);
#endif /* WITHTILDE */
                    }
		    else {
#if WITHTILDE
		      targs = R_COMB(*desc,args);
		      margs = R_COMB(*desc,nargs);       /* benoetigte argumente */
		      nfree = 0;                         /* noch keine arg. frei */
#endif /* WITHTILDE */
		      if (T_POINTER((int)px))
			INC_REFCNT(px);
		      PUSHSTACK(S_e,(STACKELEM)px);
		    }
#if WITHTILDE
                  /* targs added by car 24.10.94 */
		  GEN_DESC(desc,"FUNC","COMB",targs,margs,R_COMB(*desc,ptd),"func",desc);
#else
		  GEN_DESC(desc,"FUNC","COMB",0,margs,R_COMB(*desc,ptd),"func",desc);
#endif /* WITHTILDE */
		  L_COMB(*desc,ptc) = CODEPTR;       /* zeiger auf code      */
		  GEN_LABEL("func",desc);
		  goto main_e;
        case TY_CONDI:
                  margs = R_CONDI(*desc,nargs)-1;    /* benoetigte argumente */
#if WITHTILDE
                  targs = (R_CONDI(*desc,args) >> 2);/* benoetigte tildeargumente */
                  nfree = nfree_r = 0;               /* noch keine arg. frei */
#else
                  nfree = 0;
#endif /* WITHTILDE */
                  px = (T_PTD)R_CONDI(*desc,ptd);    /* original conditional */
                  L_CONDI(*desc,ptc) = CODEPTR;
                  /* then- und else-expr einlagern */
#if WITHTILDE
                  i = (R_CONDI(*desc,args) & 0x3) ? 0 : margs;
#else
		  i = R_CONDI(*desc,args) ? 0 : margs;
#endif /* WITHTILDE */
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
#if WITHTILDE
                      load_body((STACKELEM **)A_COND(*px,ptee),i,i,targs);
#else
                      load_body((STACKELEM **)A_COND(*px,ptee),i,i);
#endif /* WITHTILDE */
                      PUSHSTACK(S_e,HASH);             /* trennsymbol */
#if WITHTILDE
                      load_body((STACKELEM **)A_COND(*px,ptee),i,i,targs);
#else
                      load_body((STACKELEM **)A_COND(*px,ptte),i,i);
#endif /* WITHTILDE */
                    }
                    else {
#if D_MESS
  if (D_M_C_JTRUE)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JTRUE);
#endif
                      GEN_INSTRL(I_JTRUE,"jtrue","cond",desc);
#if WITHTILDE
                      load_body((STACKELEM **)A_COND(*px,ptee),i,i,targs);
#else
                      load_body((STACKELEM **)A_COND(*px,ptte),i,i);
#endif /* WITHTILDE */
                      PUSHSTACK(S_e,HASH);             /* trennsymbol */
#if WITHTILDE
                      load_body((STACKELEM **)A_COND(*px,ptee),i,i,targs);
#else
                      load_body((STACKELEM **)A_COND(*px,ptee),i,i);
#endif /* WITHTILDE */
                    }
                  }
                  else {
#if D_MESS
  if (D_M_C_JCOND)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_JCOND);
#endif
                    GEN_INSTRL(I_JCOND,"jcond","cond",desc);
#if WITHTILDE
                    load_body((STACKELEM **)A_COND(*px,ptee),i,i,targs);
#else
                    load_body((STACKELEM **)A_COND(*px,ptee),i,i);
#endif
                    PUSHSTACK(S_e,HASH);             /* trennsymbol */
#if WITHTILDE
                    load_body((STACKELEM **)A_COND(*px,ptte),i,i,targs);
#else
                    load_body((STACKELEM **)A_COND(*px,ptte),i,i);
#endif /* WITHTILDE */
                  }
                  PUSHSTACK(S_m,DOLLAR);
                  goto main_e;
#if UH_ZF_PM

/*uh, 17.10.90*/    /* Hier war der Code fuer zurueckgestellte TY_BODY, TY_GUARD und TY_CASE */
/*uh, 17.10.90*/    /* Er ist jetzt ersetzt durch: */
/*uh, 17.10.90*/        case TY_CASE: /* Ein CASE wurde zurueckgestellt. */
/*uh, 17.10.90*/                      /* Es soll jetzt uebersetzt werden. */
/*uh, 17.10.90*/                      /* dazu werden jetzt die einzelnen Klauseln ausgegraben und durch */
/*uh, 17.10.90*/                      /* HASH getrennt auf den E-Stack gelegt. */
/*uh, 17.10.90*/                        margs = R_CASE(*desc,nargs)-1;
/*uh, 17.10.90*/                        nfree = 0;
                                        thiscase = R_CASE(*desc,ptd);
                                        R_CASE(*desc,ptd) = (T_PTD) R_CLAUSE(*(T_PTD)R_SELECTION(*(T_PTD)R_CASE(*desc,ptd),clauses),sons)[3];
                                        GEN_DESC(desc,"FUNC","CASE",R_CASE(*desc,args),R_SWITCH(*(T_PTD)R_CASE(*desc,ptd),anz_args),R_CASE(*desc,ptd),"case",desc);
/*uh, 17.10.90*/                        L_CASE(*desc,ptc)=CODEPTR;
/*uh, 17.10.90*/
/*uh, 17.10.90*/
/*uh, 17.10.90*/                        px = thiscase /* R_CASE(*desc,ptd) */;  /* SWITCH-Descriptor hervorholen */
/*uh, 17.10.90*/                                                  /* um an die Klauseln zu kommen */
/*uh, 17.10.90*/
/*uh, 17.10.90*/                        PUSHSTACK(S_m,SET_ARITY(DOLLAR,nfree)); /* uh 11.4.92 */
/*uh, 17.10.90*/                        PUSHSTACK(S_m,SET_ARITY(PM_SWITCH,R_SELECTION(*px,nclauses)));
                                        clause_nr = 0;
/*uh, 17.10.90*/                        PUSHSTACK(S_m,DOLLAR);
/*uh, 17.10.90*/
/*uh, 17.10.90*/                        PUSH_L(thiscase); /* PM-Rueckuebersetzung */
/*uh, 17.10.90*/                        thiscase=desc;
/*uh, 17.10.90*/     /* printf("thiscase auf den L-Stack gelegt! depth=%d\n",DEPTH_L());  */
/*uh, 17.10.90*/
/*uh, 17.10.90*/                        /* Klauseln laden */
/*uh, 17.10.90*/                        load_clauses(px);
/*uh, 17.10.90*/
/*uh, 17.10.90*/                        GEN_LABEL("case",desc);

                  /* NCASE : generate list */
                  i = R_SWITCH(*(T_PTD)R_CLAUSE(*(T_PTD)R_SELECTION(*px,clauses),sons)[3],anz_args);
                  if (i > 1) {
#if D_MESS
  if (D_M_C_MKILIST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKILIST);
#endif
                    GEN_INSTR1(I_MKILIST,"mkilist",i);
                  }

/*uh, 17.10.90*/          DBUG_PRINT ("rear", ("ready go ! (UH PM :-)"));
/*uh, 17.10.90*/                        goto main_e;

#else /* UH_ZF_PM */

        case TY_CASE:
                  margs = R_CASE(*desc,nargs);     /* benoetigte argumente */
                  nfree = 0;                       /* noch keine arg. frei */
                  px = R_CASE(*desc,ptd);          /* original SWITCH */
#if UH_ZF_PM
                  DBUG_PRINT ("rear", ("hier darf ich nicht sein..."));
#endif /* UH_ZF_PM */

#if WITHTILDE
		  targs = R_CASE(*desc,args);      /* relativ freie vars */
		  nfree_r = 0;
#endif /* WITHTILDE */
                  args  = R_CASE(*desc,args);      /* rel. freie var.? */
                  x = (STACKELEM)desc;             /* CASE descriptor */
                  PPOPSTACK(S_a);                  /* CASE descriptor */
                  PUSHSTACK(S_v,0);                /* bottom-symbol */
                  GEN_DESC(desc,"FUNC","CASE",args,margs,px,"case",desc);
                  GEN_LABEL("case",desc);
                  L_CASE(*desc,ptc) = CODEPTR;

                  /* NCASE : generate list */
#if WITHTILDE
                  i = R_SWITCH(*px,anz_args);
#else
                  i = R_SWITCH(*px,case_type);
#endif /* WITHTILDE */
                  if (i > 1) {
#if D_MESS
  if (D_M_C_MKILIST)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_MKILIST);
#endif
                    GEN_INSTR1(I_MKILIST,"mkilist",i);
                  }

                  for ( i = 1 ; i <= R_SWITCH(*px,nwhen) ; i++ ) {
                    register T_PTD ptd = (T_PTD)(R_SWITCH(*px,ptse)[i]);
		    if ((i == R_SWITCH(*px,nwhen)) && (ptd == NULL)) {
		      /* no otherwise */
#if D_MESS
  if (D_M_C_NOMATCH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_NOMATCH);
#endif
		      GEN_INSTRX(I_NOMATCH,"nomatch",x); }
		    else
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
  if (D_M_C_PUSHC_W)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_PUSHC_W);
#endif
                        GEN_INSTRN(I_PUSHC_W,"pushc_w",SA_TRUE);
                      }
                      if (R_MATCH(*ptd,guard) != 0) {
                        /* GUARD descriptor anlegen */
                        MAKEDESC(desc,1,C_FUNC,TY_GUARD);
                        L_CASE(*desc,args) = i;
#if WITHTILDE
			L_CASE(*desc,nargs) = targs;
#else
                        L_CASE(*desc,nargs) = test_num(R_MATCH(*ptd,guard));
#endif /* WITHTILDE */
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
#if WITHTILDE
			/* number of relativly free variables */
			/* ---> size of the tilde frame       */
			L_CASE(*desc,nargs) = targs;
#else
                        L_CASE(*desc,nargs) = test_num(R_MATCH(*ptd,body));
#endif /* WITHTILDE */
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
#if !WITHTILDE
		    /* tilde-preprocessor does not generate nomat descriptors */
                    else
                    if (R_DESC(*ptd,type) == TY_NOMAT) {
#if D_MESS
  if (D_M_C_NOMATCH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_NOMATCH);
#endif
                      GEN_INSTRX(I_NOMATCH,"nomatch",x);
                    }
#endif /* !WITHTILDE */
                    else POST_MORTEM("ear: invalid SWITCH vector entry");
                  } /* end for */

#if 0
		  /* mah 100294 : last instruction in case: nomatch */
#if D_MESS
  if (D_M_C_NOMATCH)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_NOMATCH);
#endif
                      GEN_INSTRX(I_NOMATCH,"nomatch",x);
#endif /* WITHTILDE */

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
#if WITHTILDE
		  targs = R_CASE(*px, args);

		  /* number of bound variables in guard expression */
		  {
		    int offset;

		    px = R_CASE(*px,ptd);              /* SWITCH */
		    px = (T_PTD)(R_SWITCH(*px,ptse)[R_CASE(*desc,args)]);

		    if (R_MATCH(*px,code) != NULL) {
		      offset = R_MATCH(*px,code)[NAMELIST];
		      margs = R_MATCH(*px,code)[offset];
		    }
		    else
		      margs = 0;
		  }
#else
                  /* NCASE: multiple pattern beruecksichtigen! */
                  margs = R_CASE(*px,nargs);         /* alle var des CASE */
                  px = R_CASE(*px,ptd);              /* SWITCH */
                  margs -= R_SWITCH(*px,case_type);  /* freie var des CASE */
                  px = (T_PTD)(R_SWITCH(*px,ptse)[R_CASE(*desc,args)]);
#endif /* WITHTILDE */
                  if (R_CASE(*desc,nargs) == 0) {
                    /* guard hat keine raltiv freie variablen */
#if WITHTILDE
		    load_body((STACKELEM **)A_MATCH(*px,guard),margs,0,targs);
#else
                    load_body((STACKELEM **)A_MATCH(*px,guard),margs,0);
                    px = (T_PTD)READSTACK(S_e);
                    if (T_POINTER((int)px) && R_DESC(*px,type) == TY_SUB) {
                      PPOPSTACK(S_e);                       /* sub funktion */
                      margs = R_FUNC(*px,nargs);
                      load_body((STACKELEM **)A_FUNC(*px,pte),margs,0);
                      DEC_REFCNT(px);
                    }
                    else margs = 0;
#endif /* WITHTILDE */
                    L_CASE(*desc,nargs) = margs;
                    GEN_DESC(desc,"FUNC","GUARD",R_CASE(*desc,args),margs,R_CASE(*desc,ptd),"guard",desc);
                    GEN_LABEL("guard",desc);
                    L_CASE(*desc,ptc) = CODEPTR;
                  }
                  else {
                    /* guard hat relativ freie variablen */
#if !WITHTILDE
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
#endif /* !WITHTILDE */
                      /* mit allen freien variablen schliessen */
                      L_CASE(*desc,nargs) = margs;
#if WITHTILDE
		      load_body((STACKELEM **)A_MATCH(*px,guard),margs,0,targs);
#else
                      load_body((STACKELEM **)A_MATCH(*px,guard),margs,margs);
#endif /* WITHTILDE */
                      GEN_DESC(desc,"FUNC","GUARD",R_CASE(*desc,args),R_CASE(*desc,nargs),R_CASE(*desc,ptd),"guard",desc);
                      L_CASE(*desc,ptc) = CODEPTR;
                      GEN_LABEL("guard",desc);
#if !WITHTILDE
                      for (args = margs ; --args >= 0 ;) {
#if D_MESS
  if (D_M_C_DUPARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DUPARG);
#endif
                        GEN_INSTR1(I_DUPARG,"duparg",margs-1);
                      }
                    }
#endif /* !WITHTILDE */
                  }
                  nfree = 0;
                  goto main_e;
        case TY_BODY:
                  PUSHSTACK(S_m,DOLLAR);             /* trennsymbol auf m */
                  /* MATCH descriptor holen : BODY -> CASE -> SWITCH -> MATCH */
                  px = R_CASE(*desc,ptd);            /* CASE */

#if WITHTILDE
		  targs = R_CASE(*px,args);

		  /* number of bound variables in body expression */
		  {
		    int offset;

		    px = R_CASE(*px,ptd);              /* SWITCH */
		    px = (T_PTD)(R_SWITCH(*px,ptse)[R_CASE(*desc,args)]);

		    if (R_MATCH(*px,code) != NULL) {
		      offset = R_MATCH(*px,code)[NAMELIST];
		      margs = R_MATCH(*px,code)[offset];
		    }
		    else
		      margs = 0;
		  }
#else
                  /* NCASE: multiple pattern beruecksichtigen! */
                  margs = R_CASE(*px,nargs);         /* alle var des CASE */
                  px = R_CASE(*px,ptd);              /* SWITCH */
                  margs -= R_SWITCH(*px,case_type);  /* freie var des CASE */
                  px = (T_PTD)(R_SWITCH(*px,ptse)[R_CASE(*desc,args)]);

#endif /* WITHTILDE */
                  if (R_CASE(*desc,nargs) == 0) {
                    /* body hat keine raltiv freie variablen */
#if WITHTILDE
		    load_body((STACKELEM **)A_MATCH(*px,body),margs,margs,targs);
#else
                    load_body((STACKELEM **)A_MATCH(*px,body),margs,0);
                    px = (T_PTD)READSTACK(S_e);
                    if (T_POINTER((int)px) && R_DESC(*px,type) == TY_SUB) {
                      PPOPSTACK(S_e);
                      margs = R_FUNC(*px,nargs);
                      load_body((STACKELEM **)A_FUNC(*px,pte),margs,margs);
                      DEC_REFCNT(px);
                    }
                    else margs = 0;
#endif /* WITHTILDE */
                    L_CASE(*desc,nargs) = margs;
                    GEN_DESC(desc,"FUNC","BODY",R_CASE(*desc,args),margs,R_CASE(*desc,ptd),"body",desc);
                    GEN_LABEL("body",desc);
                    L_CASE(*desc,ptc) = CODEPTR;
                  }
                  else {
                    /* body hat relativ freie variablen */
#if !WITHTILDE
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
#endif /* !WITHTILDE */
                      L_CASE(*desc,nargs) = margs;
#if WITHTILDE
                      load_body((STACKELEM **)A_MATCH(*px,body),margs,margs,targs);
#else
                      load_body((STACKELEM **)A_MATCH(*px,body),margs,margs);
#endif /* WITHTILDE */
                      GEN_DESC(desc,"FUNC","BODY",R_CASE(*desc,args),R_CASE(*desc,nargs),R_CASE(*desc,ptd),"body",desc);
                      L_CASE(*desc,ptc) = CODEPTR;
                      GEN_LABEL("body",desc);
#if !WITHTILDE
                      for (args = margs ; --args >= 0 ;) {
#if D_MESS
  if (D_M_C_DUPARG)
    GEN_INSTR1(I_COUNT, "count", MINDEX_COUNT_DUPARG);
#endif
                        GEN_INSTR1(I_DUPARG,"duparg",margs-1);
                      }
                    }
#endif /* !WITHTILDE */
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
                    /* body hat keine raltiv fr<eie variablen */
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

#endif /* UH_ZF_PM */

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

  if (codefp)                           /* codefile schliessen */
  {
    fclose(codefp);
    codefp = NULL;  /* stt */
  }
  
  /* codedump fuer wb schreiben */
  if ((codefp = fopen(CodeDump,"w")) == NULL)
    post_mortem("can't open codedump");

#if     DEBUG
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
      case I_PUSH_W:   OUTINSTR("I_PUSH_W");
                       NEWSYMB(*cp);          OUTARG(cp); cp++; break;
      case I_PUSHC_W:  OUTINSTR("I_PUSHC_W"); OUTARG(cp); cp++; break;
      case I_PUSH_AW:  OUTINSTR("I_PUSH_AW"); OUTARG(cp); cp++; break;
      case I_PUSH_AW0: OUTINSTR("I_PUSH_AW0");                  break;
      case I_MOVE_AW:  OUTINSTR("I_MOVE_AW");                   break;
      case I_DELTA1:   OUTINSTR("I_DELTA1");  OUTARG(cp); cp++; break;
      case I_DELTA2:   OUTINSTR("I_DELTA2");  OUTARG(cp); cp++; break;
      case I_DELTA3:   OUTINSTR("I_DELTA3");  OUTARG(cp); cp++; break;
      case I_DELTA4:   OUTINSTR("I_DELTA4");  OUTARG(cp); cp++; break;
      case I_INTACT:   OUTINSTR("I_INTACT");  OUTARG(cp); cp++; break;
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
#ifdef CAF_UPDATE
      case I_UPDATE:   *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_UPDATE");  OUTARG(cp); cp++; break;
#endif /* CAF_UPDATE */
      case I_BETAQ1:   *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_BETAQ1");  OUTARG(cp); cp++; break;
      case I_JTAILQ:   *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAILQ");   OUTARG(cp); cp++; break;
      case I_JTAILQ0:  *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAILQ0");  OUTARG(cp); cp++; break;
      case I_JTAILQ1:  *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                       OUTINSTR("I_JTAILQ1");  OUTARG(cp); cp++; break;
      case I_MKLIST:   OUTINSTR("I_MKLIST");  OUTARG(cp); cp++; break;
      case I_MKSLOT:   OUTINSTR("I_MKSLOT");                    break;
      case I_MKFRAME:   OUTINSTR("I_MKFRAME");  OUTARG(cp); cp++; break;
      case I_FREE_A:   OUTINSTR("I_FREE_A");  OUTARG(cp); cp++; break;
      case I_FREE1_A:  OUTINSTR("I_FREE1_A");                   break;
      case I_RTF:      OUTINSTR("I_RTF");                       break;
      case I_RTM:      OUTINSTR("I_RTM");                       break;
      case I_RTP:      OUTINSTR("I_RTP");     OUTARG(cp); cp++; break;
      case I_RTC:      OUTINSTR("I_RTC");     OUTARG(cp); cp++; break;
      case I_RTS:      OUTINSTR("I_RTS");                       break;
      case I_MKGCLOS:  OUTINSTR("I_MKGCLOS"); OUTARG(cp); cp++; break;
#if WITHTILDE
      case I_MKBCLOS:  OUTINSTR("I_MKBCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKGACLOS: OUTINSTR("I_MKGACLOS");OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKGSCLOS: OUTINSTR("I_MKGSCLOS");OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKCCLOS:  OUTINSTR("I_MKCCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKSCLOS:  OUTINSTR("I_MKSCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
#else
      case I_MKBCLOS:  OUTINSTR("I_MKBCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKCCLOS:  OUTINSTR("I_MKCCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKSCLOS:  OUTINSTR("I_MKSCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
#endif /* WITHTILDE */
      case I_MKDCLOS:  OUTINSTR("I_MKDCLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
      case I_MKICLOS:  OUTINSTR("I_MKICLOS"); OUTARG(cp); cp++; OUTARG(cp); cp++; break;
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
#if WITHTILDE
      case I_DIST:     OUTINSTR("I_DIST"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
#else
      case I_DIST:     OUTINSTR("I_DIST"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
#endif
      case I_DISTEND:  OUTINSTR("I_DIST_END");                  break;
      case I_WAIT:     OUTINSTR("I_WAIT"); OUTARG(cp); cp++;    break;
      case I_POPH:     OUTINSTR("I_POPH");                      break;
#if WITHTILDE
      case I_DISTB:     OUTINSTR("I_DISTB"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
#else
      case I_DISTB:     OUTINSTR("I_DISTB"); OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; OUTARG(cp); cp++; break;
#endif

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
      case I_MSDISTCK : OUTINSTR("I_MSDISTCK");  OUTARG(cp); cp++;   break;
    
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
/*uh, 17.10.90*/              case I_MKWFRAME :   OUTINSTR("I_MKWFRAME"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MKAFRAME :   OUTINSTR("I_MKAFRAME"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_RMWFRAME :   OUTINSTR("I_REMWFRAME"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_BIND     :   OUTINSTR("I_BIND"); OUTARG(cp); cp++;  break;
/*uh, 17.10.90*/              case I_MATCHC   :   OUTINSTR("I_MATCHC");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHNIL :   OUTINSTR("I_MATCHNIL");
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHTRUE :   OUTINSTR("I_MATCHTRUE");
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHFALSE :   OUTINSTR("I_MATCHFALSE");
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHINT :   OUTINSTR("I_MATCHINT");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHBOOL:   OUTINSTR("I_MATCHBOOL");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHPRIM:   OUTINSTR("I_MATCHPRIM");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHIN  :   OUTINSTR("I_MATCHIN");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp);  OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_ADVANCE  :   OUTINSTR("I_ADVANCE");
/*uh, 17.10.90*/                                  OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_ENDLIST  :   OUTINSTR("I_ENDLIST"); break;
/*uh, 17.10.90*/              case I_DEREF    :   OUTINSTR("I_DEREFERENCE"); break;
/*uh, 17.10.90*/              case I_DROP     :   OUTINSTR("I_DROP"); break;
/*uh, 17.10.90*/              case I_PICK     :   OUTINSTR("I_PICK"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_SAVEPTR  :   OUTINSTR("I_SAVEPTR"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_RESTPTR  :   OUTINSTR("I_RESTPTR"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_NESTLIST :   OUTINSTR("I_NESTLIST"); break;
/*uh, 17.10.90*/              case I_MATCHARB :
/*uh, 17.10.90*/                            OUTINSTR("I_MATCHARB");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHLIST:   OUTINSTR("I_MATCHLIST");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_TGUARD   :   OUTINSTR("I_GUARD");
                                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            OUTARG(cp); cp++; OUTARG(cp); cp++; 
/*uh, 17.10.90*/                            OUTARG(cp); cp++; OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_MKCASE   :   OUTINSTR("I_MKCASE");
/*uh, 17.10.90*/                            OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_BACKUP   :   OUTINSTR("I_BACKUP");
/*uh, 17.10.90*/                            OUTARG(cp); cp++; break;
/*uh, 17.10.90*/
/*uh, 17.10.90*/        case I_STARTSUBL:   OUTINSTR("I_STARTSUBL");
/*uh, 17.10.90*/                            OUTARG(cp); cp++; OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_ENDSUBL:     OUTINSTR("I_ENDSUBL");   OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_BINDSUBL:    OUTINSTR("I_BINDSUBL");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_MKBTFRAME:   OUTINSTR("I_MKBTFRAME"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_RMBTFRAME:   OUTINSTR("I_RMBTFRAME"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_RESTOREBT:   OUTINSTR("I_RESTOREBT"); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_INITBT:      OUTINSTR("I_INITBT");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_SAVEBT:      OUTINSTR("I_SAVEBT");
/*uh, 17.10.90*/                            OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_ATEND:       OUTINSTR("I_ATEND");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_ATSTART:     OUTINSTR("I_ATSTART");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/        case I_FETCH:       OUTINSTR("I_FETCH"); break;
/*uh, 17.10.90*/

/* new by rs 1995 */

/*uh, 17.10.90*/              case I_MATCHREAL :   OUTINSTR("I_MATCHREAL");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHARBS :
/*uh, 17.10.90*/                            OUTINSTR("I_MATCHARBS");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;
/*uh, 17.10.90*/              case I_MATCHSTR:   OUTINSTR("I_MATCHSTR");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;

/*uh, 17.10.90*/              case I_BINDS     :   OUTINSTR("I_BINDS"); OUTARG(cp); cp++;  break;
/*uh, 17.10.90*/              case I_MATCHDIGIT :   OUTINSTR("I_MATCHDIGIT");
/*uh, 17.10.90*/                            OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++;
/*uh, 17.10.90*/                            NEWTARGET(*cp); OUTARG(cp); cp++; break;


#endif /* UH_ZF_PM */

#if WITHTILDE
      case I_PUSH_TW:    OUTINSTR("I_PUSH_TW");   OUTARG(cp); cp++; break;
      case I_PUSH_TW0:   OUTINSTR("I_PUSH_TW0");                    break;
      case I_MOVE_TW:    OUTINSTR("I_MOVE_TW");                     break;
      case I_FREE_R:     OUTINSTR("I_FREE_R");    OUTARG(cp); cp++; break;
      case I_FREE1_R:    OUTINSTR("I_FREE1_R");                     break;
      case I_RTF_T:      OUTINSTR("I_RTF_T");                       break;
      case I_FREESW_T:   OUTINSTR("I_FREESW_T");  OUTARG(cp); cp++; break;
      case I_FREE1SW_T:  OUTINSTR("I_FREE1SW_T");                   break;
      case I_RTM_T:      OUTINSTR("I_RTM_T");                       break;
      case I_RTC_T:      OUTINSTR("I_RTC_T");     OUTARG(cp); cp++; break;
      case I_GAMMA :     *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                         OUTINSTR("I_GAMMA");     OUTARG(cp); cp++; break;
      case I_PUSH_TR:    OUTINSTR("I_PUSH_TR");   OUTARG(cp); cp++; break;
      case I_PUSH_TR0:   OUTINSTR("I_PUSH_TR0");                    break;
      case I_MOVE_TR:    OUTINSTR("I_MOVE_TR");                     break;
      case I_PUSH_AR:    OUTINSTR("I_PUSH_AR");   OUTARG(cp); cp++; break;
      case I_PUSH_AR0:   OUTINSTR("I_PUSH_AR0");                    break;
      case I_MOVE_AR:    OUTINSTR("I_MOVE_AR");                     break;
      case I_MOVE_WR:    OUTINSTR("I_MOVE_WR");                     break;
      case I_GAMMABETA:  *cp = (INSTR)R_COMB(*(T_PTD)*cp,ptc);
                         OUTINSTR("I_GAMMABETA");  OUTARG(cp); cp++;break;
      case I_GAMMACASE:  *cp = (INSTR)R_CASE(*(T_PTD)*cp,ptc);
                         OUTINSTR("I_GAMMACASE");    OUTARG(cp); cp++;   break;
      case I_PUSH_R:     OUTINSTR("I_PUSH_R");
                            NEWSYMB(*cp);          OUTARG(cp); cp++;break;

      /* Es gibt fuer GAMMA keine Tail-end-optimierungen */
      /* in der 4-Stack-Version.                         */
      /* Also gibt es kein Tail-flag und damit keine     */
      /* GAMMAQ-Befehle.                                 */
#endif /* WITHTILDE */

      default:         POST_MORTEM("ear: invalid instruction");
    } /* end switch */
    /* NEWLINE(); */
  } /* end for */
  NEWCODE(I_STOP);

#if    DEBUG
  if (codefp)                            /* codefile schliessen */
  {
    fclose(codefp);
    codefp = NULL;   /* stt */
  }
  
#endif /* DEBUG */

  END_MODUL("ear");
}

/* end of file */
