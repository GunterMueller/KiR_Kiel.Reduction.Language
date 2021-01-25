/* $Log: rheap.c,v $
 * Revision 1.25  1996/02/21  18:17:22  cr
 * prototype implementation of interactions for arbitrary expressions
 * uses modified send/receive-routines from rmessage.c
 * compile with -DSTORE=1 to get the prototype (interactions get/put)
 * or with -DSTORE=0 to get the original files.
 * involved files so far: rheap.c riafunc.c rstelem.h rmessage.c
 * rmessage.c has to be compiled once with -DSTORE=1 to get the
 * modified send/receive-routines, and perhaps a second time with
 * -DSTORE=0 to get its original functionality for the distributed
 * versions.
 *
 * Revision 1.24  1995/12/11  18:25:10  cr
 * support for empty frames: NILSTRUCT
 *
 * Revision 1.23  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.22  1995/07/03  11:39:57  car
 * DBUG included for reference counting
 *
 * Revision 1.21  1995/05/22  09:22:22  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.20  1995/03/28  14:27:54  cr
 * merged dealloc_slot into dealloc_frame, dealloc_slot removed
 *
 * Revision 1.19  1995/03/23  14:50:15  cr
 * warnings in dealloc_slot removed
 *
 * Revision 1.18  1995/03/09  14:38:18  cr
 * Added descriptors for interactions, frames and slots
 *
 * Revision 1.17  1995/01/16  14:20:38  rs
 * more DBUG Output
 *
 * Revision 1.16  1994/09/06  09:31:12  car
 * *** empty log message ***
 *
 * Revision 1.15  1994/07/19  12:58:47  car
 * bug in dealloc_ty/dealloc_ln corrected
 *
 * Revision 1.14  1994/06/22  07:49:51  car
 * ST_COMPTYPE modified
 *
 * Revision 1.13  1994/06/22  07:14:13  car
 * *** empty log message ***
 *
 * Revision 1.12  1994/06/20  18:25:56  mah
 * bugfix for tilde version
 *
 * Revision 1.11  1994/06/20  06:03:01  car
 * first extension for optimizer
 *
 * Revision 1.10  1994/03/09  08:23:10  car
 * *** empty log message ***
 *
 * Revision 1.9  1994/03/09  07:47:57  car
 * support for remote descriptors
 *
 * Revision 1.8  1994/01/18  11:18:57  mah
 * 1.6 fix removed
 * new warning fix (gcc 2.5.6)
 *
 * Revision 1.7  1994/01/18  10:54:44  mah
 * warning fix (gcc 2.5.6)
 *
 * Revision 1.6  1994/01/07  12:24:02  mah
 * bug fix at heap mem allocation
 *
 * Revision 1.5  1993/10/29  08:49:37  ach
 * some error messages rephrased
 *
 * Revision 1.4  1993/10/28  14:07:15  rs
 * removed warnings update
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:49:55  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */







/****************************************************************************/
/*                                                                          */
/*               D e r   H e a p m a n a g e r                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*
 *      Modulname       int/ext     aufgerufen durch: (vollstaendige Liste !?)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  InitHeapManagement    ext      reduce.
 *  inithp                int      InitHeapManagement.
 *  ReInitHeap            ext      reduce.
 *  crea_global_desc      int      InitHeapManagement  , ReInitHeap
 *  init_var              int      inithp , ReInintHeap
 *  newdesc               ext      ueberall
 *  freedesc              ext      ret_expr,ret_func. (in retrieve:c)
 *  moredes               int      newdesc.
 *  get_HBLOCK            int      newheap.
 *  newheap               ext      ueberall
 *  newbuff               ext      closefunc.
 *  freeheap              ext      closefunc,ret_expr,ret_func.
 *  cutheap               ext      digit-div.
 *  compheap              ext      monitor_call , newheap , moredes.
 *  dealloc_sc            ext      Debug: dealloc.  NoDebug: DEC_REFCNT.
 *  dealloc_di            ext      Debug: dealloc.  NoDebug: DEC_REFCNT.
 *  dealloc_li            ext      Debug: dealloc.  NoDebug: DEC_REFCNT.
 *  dealloc_mvt           ext      Debug: dealloc.  NoDebug: DEC_REFCNT.
 *  dealloc_ex            ext      Debug: dealloc.  NoDebug: DEC_REFCNT.
 *  dealloc_co            ext      Debug: dealloc.  NoDebug: DEC_REFCNT.
 *  freeexpr              int      dealloc_ex .
 *
 *       Module NUR fuer die Debug-Version:
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      Modulname       int/ext     aufgerufen durch:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  hpchain               ext      monitor_call , alle moeglichen Heapmodule
 *  isheap                ext      monitor_call , alle moeglichen Heapmodule
 *  isdesc                ext      DescDump,monitor_call, alle moeglichen Heapmodule
 *  !!! isdesc ist aus dem Debug-Block herausgenommen, weil es fuer das Verschicken
 *      gebraucht wird !!!       
 *  deschain              ext      monitor_call, alle moeglichen Heapmodule
 *  inc_refcnt            int      test_inc_refcnt
 *  dec_refcnt            int      test_dec_refcnt
 *  test_inc_refcnt       ext      Makro: INC_REFCNT
 *  test_dec_refcnt       ext      Makro: DEC_REFCNT
 *  dealloc               int      dec_refcnt
 *  res_heap              ext      Makro: RES_HEAP
 *  rel_heap              ext      Makro: REL_HEAP
 *  inchain               int      OutDescArea.
 *
 *       Module  fuer einen Heapdump :
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      Modulname       int/ext     aufgerufen durch:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * OutDescArea            int      HeapDump.
 * DumpElem               int      OutHeapArea.
 * OutHeapArea            int      HeapDump.
 * HeapDump               ext      monitor_call
 *
 ****************************************************************************/

/* last update  by     why
   10.06.88     CS     letrec
   copied       by     to
   11.06.88     sj     (struck)iheap:c
*/

/*-----------------------------------------------------------------------
 *   I n c l u d e s
 *-----------------------------------------------------------------------
 */

#define  HEAPMANAGER   hallo hier ist der Heapmanger

#include "rstdinc.h"
#include "rheapty.h"
#include "rextern.h"
#include "rmeasure.h"
#include <string.h>

#include <stdio.h>               /* ach 30/10/92 */
#include <stdlib.h>              /* RS 30/10/92 */ 

#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

/*-------------------------------------------------------------------------
 * externals for heapdump functions :
 *  used only in the debug-version.
 *------------------------------------------------------------------------
 */
#if        DEBUG
extern  FILE        *DumpFile;
extern  prstel();
#else
/*static        FILE        *DumpFile = stdout ;
static  int DescDump () { return (0);}; */     /* int und ret eingefuegt von RS 30/10/92 */ /* auskommentiert ach 30/10/92 */
#endif

/*-------------------------------------------------------------------------*/
/* external function declarations --                                       */
/*-------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR sts_dec(); /* rsts-dec.c                             */

#if DEBUG
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
void deschain(); /* TB, 4.11.1992 */        /* rheap.c */
void hpchain(); /* TB, 4.11.1992 */         /* rheap.c */
void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
int isdesc(); /* TB, 4.11.1992 */           /* rheap.c */
int isheap(); /* TB, 4.11. 1992 */          /* rheap.c */
void HeapDump(); /* TB, 4.11.1992 */        /* rheap.c */
void dealloc(); /* TB, 4.11.1992 */         /* rheap.c */
int inchain(); /* TB, 4.11.1992 */          /* rheap.c */

/* RS 6.11.1992 */ 
extern int init_scavenge();         /* rscavenge.c */
extern int reinit_scavenge();       /* rscavenge.c */
/* END of RS 6.11.1992 */ 

#ifdef D_MESS_ONLY
extern int moredes();
extern T_HBLOCK *get_HBLOCK();
#endif

/*-------------------------------------------------------------------------*/
/* external variable declarations --                                       */
/*-------------------------------------------------------------------------*/
extern BOOLEAN  inittrigdesc; /* if inittrigdesc() is called once then TRUE*/

extern BOOLEAN  initexpdesc;  /* if initexpdesc()  is called once then TRUE*/

/* RS 30/10/92 */ 
int newheap();
int inithp();

#if (!D_SLAVE)
extern void initcode();                /* rear.c */
#endif

/*End of RS 30/10/92 */ 
#if SCAVENGE
int moredes();
void freeexpr();
#else
#if D_MESS
int moredes();
#else
#ifndef D_MESS_ONLY
static int moredes();    /* static RS 3.12.1992 */
#else 
extern int moredes();
#endif
#endif
static void freeexpr();   /* static RS 3.12.1992 */
#endif
int compheap();

/* end of ach */

/*-------------------------------------------------------------------------
 * declarations --
 *-------------------------------------------------------------------------
 */
#if DEBUG                              /* from rumtime */
extern int stopheap;                   /* simulate heapoverflow after   */
                                       /* stopheap new-calls            */
extern int debcnt;                     /* debug output ?                */
extern int debdesc;                    /* debug output for one desc ?   */
#endif


#ifndef D_MESS_ONLY
#ifdef HEAPMANAGER
   PTR_DESCRIPTOR  _dig0;               /* for digit-arithmetic              */
   PTR_DESCRIPTOR  _dig1;               /* for digit-arithmetic              */
   PTR_DESCRIPTOR  _work;               /* for digit-arithmetic              */
   PTR_DESCRIPTOR  _nilmat;             /* for structuring functions         */
   PTR_DESCRIPTOR  _nilvect;            /* for structuring functions         */
   PTR_DESCRIPTOR  _niltvect;           /* for structuring functions         */
   PTR_DESCRIPTOR  _nil;                /* "for structuring functions"       */
   PTR_DESCRIPTOR  _nilstring;          /* "for structuring functions"       */
   PTR_DESCRIPTOR  _nilstruct;          /* "for structuring functions"       */
   PTR_DESCRIPTOR  _digPI;              /* for trigonometric functions       */
   PTR_DESCRIPTOR  _digLN2;             /* to eval. exp functions in         */
                                        /*           rbibfunc.c              */
   PTR_DESCRIPTOR  _digRG;              /* used in rbibfunc.c                */
                                        /* relativ error                     */
#endif  /* HEAPMANAGER */
#endif

#ifndef D_MESS_ONLY

/*--------------------------------------------------------------------------*/
/* declarations --                                                          */
/*--------------------------------------------------------------------------*/
char pic[] = "3.141592653589793238462643383279502884197169399375105820\
97494459230781640628620899862803482534211706798214808651328230";
char piv[sizeof(pic)];                /*            variable PI             */

char lnc[] = "0.693147180559945309417232121458176568075500134360255254\
12068000949339362196969471560586332699641868754200148102057068";
char lnv[sizeof(lnc)];                /*            variable ln(2)          */

#else

extern char pic[], piv[], lnc[], lnv[];

#endif


#ifndef D_MESS_ONLY
/* heapvariables, which are used in runtime.c */
       int          descnt;        /* number of allocated desc's    */
       T_DESCRIPTOR *StHeapD;      /* initial ptr *global           */
       T_DESCRIPTOR *EnHeapD;      /* arena top *global             */
       T_DESCRIPTOR *deslist;      /* free list ptr                 */
#if (D_SLAVE && D_MESS)
       int          desmax;        /* number of all desc's          */
#else
static int          desmax;        /* number of all desc's          */
#endif
#if D_SLAVE
       T_DESCRIPTOR *highdesc = 0L;
extern int          proc_id;
#endif
static int          odesmax;       /* number of all desc's  *reinit */
static T_DESCRIPTOR *oStHeapD;     /* initial ptr *reinit           */
static T_DESCRIPTOR *oEnHeapD;     /* arena top *reinit             */
       T_HBLOCK     *hpbase;       /* initial arena                 */
static T_HBLOCK     *ohpbase;      /* initial arena *reinit         */
       T_HBLOCK     *hpend;        /* arena top                     */
static T_HBLOCK     *ohpend;       /* arena top *reinit             */
       T_HBLOCK     *hplast;       /* points to last handled block  */
static int          heap_res;      /* heap reserved, if nonzero     */
static int          heapsize ;     /* set by InitHeapManagement     */
static int          heapdes ;      /* set by InitHeapManagement     */
#else
extern    T_DESCRIPTOR *deslist;      /* free list ptr                 */
extern    int          descnt;        /* number of allocated desc's    */
extern    T_DESCRIPTOR *deslist;      /* free list ptr                 */
extern    T_HBLOCK     *hpbase;       /* initial arena                 */
extern    T_HBLOCK     *hpend;        /* arena top                     */
extern    T_HBLOCK     *hplast;       /* points to last handled block  */
extern    T_DESCRIPTOR *StHeapD;      /* initial ptr *global           */
extern    T_DESCRIPTOR *EnHeapD;      /* arena top *global             */
extern    int          desmax;        /* number of all desc's          */
extern  T_DESCRIPTOR *highdesc;
#endif

#ifndef D_MESS_ONLY
#if D_SLAVE
       BOOLEAN      _heap_reserved;
       T_DESCRIPTOR _desc;
       INSTR        *heap_code_start;
       short        stdesccnt;
       short        idcnt;
#endif
#endif

#if (D_DIST || STORE)
#ifndef D_MESS_ONLY
void * static_heap_upper_border;
void * static_heap_lower_border;
void * stdesc_border;
#endif
#endif

/* forward declarations */
#ifndef D_MESS_ONLY
T_DESCRIPTOR * newdesc();
int  dealloc_sc();
int  dealloc_di();
int  dealloc_li();
int  dealloc_mvt();  /* Matrix / Vector / transposed Vector */
int  dealloc_ex();
static int  dealloc_co();      /* static RS 3.12.1992 */
static int  dealloc_fu();        /* dg */   /* static RS 3.12.1992 */
static int  dealloc_cns();       /* dg */   /* static RS 3.12.1992 */
static int  dealloc_mat();       /* uh */   /* static RS 3.12.1992 */
static int  dealloc_pat();       /* uh */   /* static RS 3.12.1992 */
static int  dealloc_ty();	 /* car, 10.06.94 */
static int  dealloc_ln();	 /* car, 10.06.94 */
/* cr 09/03/95, kir(ff), START */
int  dealloc_frame();
int  dealloc_inter();
/* cr 09/03/95, kir(ff), END */
int  dealloc_intact();           /* stt  11.09.95 */
#endif

#ifndef D_MESS_ONLY

FCTPTR dealloc_tab[] = {
  0 
 ,dealloc_sc   /* C_SCALAR     '\1' */
 ,dealloc_di   /* C_DIGIT      '\2' */
 ,dealloc_li   /* C_LIST       '\3' */
 ,dealloc_mvt  /* C_MATRIX     '\4' */
 ,dealloc_fu   /* C_FUNC       '\5' */
 ,dealloc_ex   /* C_EXPRESSION '\6' */
 ,dealloc_co   /* C_CONSTANT   '\7' */
 ,dealloc_mvt  /* C_VECTOR     '\10' */
 ,dealloc_mvt  /* C_TVECTOR    '\11' */
 ,0            /* C_SWAPHEAP   '\12' */
 ,dealloc_mat  /* C_MATCHING   '\13' */
 ,dealloc_pat  /* C_PATTERN    '\14' */
 ,dealloc_cns  /* C_CONS       '\15' */
 ,dealloc_sc   /* use dealloc_sc to deallocate atomic type C_ATOMTYPE '\16' */
 ,dealloc_ty   /* deallocate composite type C_COMPTYPE '\17' */
 ,dealloc_ln   /* deallocate linknode C_LINKNODE '\20' */
/* cr 09/03/95, kir(ff), START */
 ,dealloc_frame   /* C_FRAME       '\21' */
 ,dealloc_inter   /* C_INTER       '\22' */
/* cr 09/03/95, kir(ff), END */
 ,dealloc_intact  /* C_INTACT      '\22' */
};

#define DEALLOC_TAB_SIZE  (sizeof(dealloc_tab) / sizeof(FCTPTR))

#else

/* extern FCTPTR dealloc_tab[]; */

#endif


/*------------------------------------------------------------------------
 * size definitions
 *------------------------------------------------------------------------
 */
#define        HEAPSIZE        (heapsize) /* size of heap in bytes        */
#define        DESCNUM         (heapdes)  /* initial # of desc's          */
#define        TET              0         /* Testzwecke                   */

/*$P*/
/*------------------------------------------------------------------------
 * definitions --
 *------------------------------------------------------------------------
 */

/* Groesse eines Deskriptors */
#define DESLEN                sizeof(T_DESCRIPTOR)

/* offset to the last word of a descriptor                  */
#define OFFTOLASTDESCWORD    ((DESLEN / sizeof(int *)) - 1)


/* In das letzte Wort des freiwerdenden Deskriptors wird    */
/* ein Pointer auf den naechsten freien Deskriptor gesetzt. */
#define HEAP_CHAIN(ptrdesc)  (*(((T_DESCRIPTOR **) (ptrdesc)) + \
                                                   OFFTOLASTDESCWORD))

/* relative error                                                             */
/* get global REDUMA redparm precision, add three and make the result negativ */
/* used as second parameter in function sts_dec("string",exp)                 */
/* to create the digit-descriptor  _digRG                                     */
#define RG    (_prec10 * (-1))


/* Ist das Heapelement in Verwendung ? */
#define testbusy(p)        (p->ref != 0)

      /* freedesc als Makro ist schneller. Verwendet durch: */
      /* alle dealloc_ Routinen, sowie freedesc selbst.     */
#if (D_SLAVE && D_MESS && D_MHEAP)
#define FREEDESC(desc)        { if (desc != NULL)  {       \
if (d_m_freedesc == m_freedesc) { \
  MPRINT_DESC_FREE(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) desc); \
  DBUG_PRINT ("MESS", ("MPRINT_DESC_FREE executed, arg: %d", (int) desc)); } \
                                HEAP_CHAIN(desc) = deslist; \
                                deslist = desc; descnt--; }}
#else
#define FREEDESC(desc)        { if (desc != NULL)  {       \
                                HEAP_CHAIN(desc) = deslist; \
                                deslist = desc; descnt--; }}
#endif

      /* freeheap als Makro ist schneller. Verwendet durch: */
      /* einige dealloc_ Routinen, sowie freeheap selbst.   */
#if (D_SLAVE && D_MESS && D_MHEAP)
#define FREEHEAP(p) { T_HBLOCK *_h_ = (T_HBLOCK *) p; \
                      if (_h_ != NULL) { \
if (d_m_freeheap == m_freeheap) { \
    MPRINT_HEAP_FREE(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) p, (int)(_h_-1)->ref);\
    DBUG_PRINT ("MESS", ("MPRINT_HEAP_FREE executed, arg: %d", (int) p)); } \
                      HPCHAIN; --_h_; \
                      ISHEAP(_h_); _h_->ref = NULL; }}
#else
#if MEASURE
#define FREEHEAP(p) { T_HBLOCK *_h_ = (T_HBLOCK *) p; \
    if (_h_ != NULL) { HPCHAIN; --_h_; \
      mfree((int)(_h_->next - _h_)); ISHEAP(_h_); _h_->ref = NULL; }}
#else
#define FREEHEAP(p) { T_HBLOCK *_h_ = (T_HBLOCK *) p; \
                      if (_h_ != NULL) { \
                        HPCHAIN; --_h_; ISHEAP(_h_); _h_->ref = NULL; }}
#endif
#endif

#define ALIGN         sizeof(T_HBLOCK)


/*------------------------------------------------------------------------
 * debugging definitions
 *------------------------------------------------------------------------
 */
#if        DEBUG
#define PRHEAP     \
/* \
{\
printf("\n\n-----------------------------------\n");\
printf(" descnt   %d \n",descnt);\
printf(" desmax   %d \n",desmax);\
printf("odesmax   %d \n",odesmax);\
printf(" StHeapD  %x \n",StHeapD);\
printf("oStHeapD  %x \n",oStHeapD);\
printf(" EnHeapD  %x \n",EnHeapD);\
printf("oEnHeapD  %x \n",oEnHeapD);\
printf(" deslist  %x \n",deslist);\
printf(" hpbase   %x \n",hpbase);\
printf("ohpbase   %x \n",ohpbase);\
printf(" hpend    %x \n",hpend);\
printf("ohpend    %x \n",ohpend);\
printf(" hplast   %x \n",hplast);\
printf(" heap_res %x \n",heap_res);\
printf("\n\n-----------------------------------\n");\
} \
*/

#else
#define PRHEAP
#endif

#if        DEBUG
#define PRDEBUG(p)        do {if (debcnt) printf(p);} while(0) 
#else
#define PRDEBUG(p)
#endif

#if        (DEBUG || D_SLAVE)
#define ASSERT(expr,text)                                                \
        do {if (!(expr)) {                                                        \
                printf(                                                \
                "Assertion 'expr' failed: file '%s', line %d\n",        \
                __FILE__,__LINE__ );                                        \
                post_mortem(text);                                \
        }} while(0)
#else
#define ASSERT(p,q)
#endif

#if        DEBUG
#define ALIGNTST(ptr,type)        ASSERT(((unsigned)ptr)%sizeof(type) == 0, \
                                "alignment error")
#else
#define ALIGNTST(ptr,type)
#endif

#if        DEBUG
#define HPCHAIN                hpchain()
#else
#define HPCHAIN
#endif

#if        DEBUG
#define DESCHAIN                deschain()
#else
#define DESCHAIN
#endif

#if        DEBUG
#define ISDESC(p)                ASSERT(isdesc(p),                \
                                        "invalid pointer to descriptor")
#else
#define ISDESC(p)
#endif

#if        DEBUG
#define ISHEAP(p)                ASSERT(isheap(p),                \
                                        "invalid pointer to heap element")
#else
#define ISHEAP(p)
#endif

/* for nCUBE-twice-compiling */

#ifndef D_MESS_ONLY

/****************************************************************************/
/*                                                                          */
/*      Module fuer die Debug- und die Nodebug-Version:                     */
/*                                                                          */
/****************************************************************************/


/*****************************************************************************/
/*                            crea_global_desc                               */
/*---------------------------------------------------------------------------*/
/*  Funktion  : Erzeugung  von globalen Standarddeskriptoren die permanent   */
/*              im Heap verbleiben.                                          */
/*              Der '_work'-Descriptor kann nicht durch sts_dec angelegt     */
/*              werden, da er selbst (bei BASE <> 10000) in sts_dec benutzt  */
/*              wird. Der work Deskriptor muss als erster angelegt werden,   */
/*              da die Funktion sts_dec den work Deskriptor benoetigt, um    */
/*              weitere Standard Deskriptoren anzulegen. Bei "manueller"     */
/*              Erzeugung muss die Variable stdescnt inkrementiert werden,   */
/*              ansonsten erfolgt die Inkrementierung in rsts-dec.c          */
/*  called by : InitHeapManagement , ReInitHeap                              */
/*****************************************************************************/
static void crea_global_desc()
{
 DBUG_ENTER ("crea_global_desc");

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((_work = (*d_m_newdesc)()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#else
  if ((_work = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#endif
  DESC_MASK(_work,2,C_DIGIT,TY_DIGIT);
  L_DIGIT((*_work),Exp) = 0;
  L_DIGIT((*_work),ndigit) = 1;
  L_DIGIT((*_work),sign) = 0;
#if (D_SLAVE && D_MESS && D_MHEAP)
  if (((*d_m_newheap)(1,A_DIGIT((*_work),ptdv))) == (int)NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#else
  if ((newheap(1,A_DIGIT((*_work),ptdv))) == (int)NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#endif
  L_DIGIT((*_work),ptdv)[0] = 10;
  stdesccnt ++;

  _dig0 = sts_dec("0",0);

  _dig1 = sts_dec("1",0);

  if (_prec10 <=100) {
    strncpy(piv,pic,_prec10+9);        /*           copy PI to piv           */
    piv[_prec10+10] = '\0';            /*       cut PI to needed length      */
    _digPI = sts_dec(piv,0);           /*    create new descriptor _digPI    */

    strncpy(lnv,lnc,_prec10+9);        /*           copy LN2 to lnv          */
    lnv[_prec10+10] = '\0';            /*      cut ln2 to needed length      */
    _digLN2 = sts_dec(lnv,0);          /*    create new descriptor _digLN    */
  }
  else {
    _digPI  = sts_dec("0",0);          /* create new descriptor _digPI with  */
                                       /* value 0  (dummy descriptor)        */
    _digLN2 = sts_dec("0",0);          /* create new descriptor _digLN2 with */
  }                                    /* value 0  (dummy descriptor)        */

  _digRG = sts_dec("1",RG);            /* relative error decriptor           */
                                       /* used in rbibfunc.c                 */

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((_nilmat = (*d_m_newdesc)()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#else
  if ((_nilmat = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#endif
  MVT_DESC_MASK(_nilmat,1,C_MATRIX,TY_UNDEF);
  L_MVT((*_nilmat),nrows,C_MATRIX) = (int)NULL;    /* das dritte Wort wird von    */
  L_MVT((*_nilmat),ncols,C_MATRIX) = (int)NULL;    /* MVT_DESC_MASK mit 0         */
  L_MVT((*_nilmat),ptdv,C_MATRIX)  = NULL;    /* initialisiert               */
  stdesccnt ++;

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((_nilvect = (*d_m_newdesc)()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#else
  if ((_nilvect = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#endif
  MVT_DESC_MASK(_nilvect,1,C_VECTOR,TY_UNDEF);
  L_MVT((*_nilvect),nrows,C_VECTOR) = (int)NULL;   /* das dritte Wort wird von    */
  L_MVT((*_nilvect),ncols,C_VECTOR) = (int)NULL;   /* MVT_DESC_MASK mit 0         */
  L_MVT((*_nilvect),ptdv,C_VECTOR)  = NULL;   /* initialisiert               */
  stdesccnt ++;

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((_niltvect = (*d_m_newdesc)()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#else
  if ((_niltvect = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#endif
  MVT_DESC_MASK(_niltvect,1,C_TVECTOR,TY_UNDEF);
  L_MVT((*_niltvect),nrows,C_TVECTOR) = (int)NULL; /* das dritte Wort wird von    */
  L_MVT((*_niltvect),ncols,C_TVECTOR) = (int)NULL; /* MVT_DESC_MASK mit 0         */
  L_MVT((*_niltvect),ptdv,C_TVECTOR)  = NULL; /* initialisiert               */
  stdesccnt ++;

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((_nil = (*d_m_newdesc)()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#else
  if ((_nil = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#endif
  LI_DESC_MASK(_nil,1,C_LIST,TY_UNDEF);       /* das zweite und dritte Wort  */
  L_LIST((*_nil),ptdv)    = NULL;             /* wird von LI_DESC_MASK mit 0 */
  stdesccnt ++;                               /* initialisiert               */

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((_nilstruct = (*d_m_newdesc)()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#else
  if ((_nilstruct = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#endif
  DESC_MASK(_nilstruct,1,C_FRAME,TY_FRAME);
  L_FRAME((*_nilstruct),dim)    = 0;
  L_FRAME((*_nilstruct),slots)  = NULL;
  stdesccnt ++;   

#if (D_SLAVE && D_MESS && D_MHEAP)
  if ((_nilstring = (*d_m_newdesc)()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#else
  if ((_nilstring = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
#endif
  LI_DESC_MASK(_nilstring,1,C_LIST,TY_STRING);/* das zweite und dritte Wort  */
  L_LIST((*_nilstring),ptdv)    = NULL;       /* wird von LI_DESC_MASK mit 0 */
  stdesccnt ++;                               /* initialisiert               */

  DBUG_VOID_RETURN;
} /**** end of procedure crea_global_desc ****/

/*****************************************************************************/
/*                            init_var                                       */
/*---------------------------------------------------------------------------*/
/*  Funktion  : Initialisierung  von globalen Variablen in inithp/ReInitHeap */
/*  called by : inithp , ReInitHeap                                          */
/*****************************************************************************/
static void init_var()
{
  DBUG_ENTER ("init_var");

  descnt = 0;                           /* initialize descriptor counter     */
  idcnt  = 0;                           /* indirect list-descriptor counter  */

  stdesccnt = (int)NULL;                     /* initialize global couter-variable */
                                        /* for the number of standard        */
                                        /* descriptors                       */

  inittrigdesc = FALSE;                 /* initialize global variable used   */
                                        /* in function inittrigdesc in       */
                                        /* rbibfunc.c                        */
  initexpdesc  = FALSE;                 /* initialize global variable used   */
                                        /* in function initexpdesc in        */
                                        /* rbibfunc.c                        */
  DBUG_VOID_RETURN;
} /**** end of procedure init_var ****/

/*------------------------------------------------------------------------
 * InitHeapManagement  --  Initialize heap with constant values
 * parameter:    hs - heapsize, hd - number of descriptors.
 * calls:        inithp, newdesc, newheap, post_mortem.
 *------------------------------------------------------------------------
 */
int        InitHeapManagement(hs,hd)
int     hs,hd;
{
#if !SCAVENGE
  static char        *base = NULL;
#endif                                 /* #if #endif eingefuegt von RS 6.11.1992 */ 

#if D_SLAVE
PTR_DESCRIPTOR _old_expr_p;
#else
  extern PTR_DESCRIPTOR _old_expr_p;       /* reduct.c */
#endif

#ifdef NotUsed
#if PI_RED_PLUS
  extern int            pi_red_plus;       /* editor: ctransl1.c */

  DBUG_ENTER ("InitHeapManagement");

  pi_red_plus = 1;
#else
  DBUG_ENTER ("InitHeapManagement");
#endif
#else
  DBUG_ENTER ("InitHeapManagement");
#endif

  /* init global variables */
  heapsize = hs;
  heapdes  = hd;

#if    SCAVENGE
  init_var ();
  if (!init_scavenge (heapsize))
    post_mortem ("Init heap for scavenging failed");
#else /* SCAVENGE */
  if (base) free(base);                    /* free old space,if there was any.   */
  base = (char *) malloc((int) HEAPSIZE);  /* get base address  */

#if D_MESS
  m_base = (int) base;
#endif

  if (base == NULL) /* mah 070194 */
    post_mortem("Init Heap: no space for malloc");

  if (inithp(base,(int)HEAPSIZE,DESCNUM)) {
#if DEBUG
    printf(" *** Init Heap   ok!  *** \n");
#endif
  }
  else
    post_mortem("Init Heap: Heap is NOT ok !");
#endif /* SCAVENGE */

#if (!D_SLAVE)
  initcode(heapsize);
#endif

  crea_global_desc();

#if (D_DIST || STORE)
  static_heap_upper_border = _nilstring;
  static_heap_lower_border = _dig0;
  stdesc_border      = _nilstring;
#if D_SLAVE
  DBUG_PRINT("STATHEAP",("begin of static heap: %08x",static_heap_lower_border));
  DBUG_PRINT("STATHEAP",("end of static heap: %08x",static_heap_upper_border));
  highdesc = _nilstring;
#endif
#endif

  if (R_DESC((*_work),type)!= TY_DIGIT)       /* ODDSEX-flag is tested */
#if ODDSEX
    post_mortem("Please compile everything again with ODDSEX not set!");
#else
    post_mortem("Please compile everything again with ODDSEX set!");
#endif

  /* verpointerte Version ist nicht mehr da */
  _old_expr_p = NULL;
  DBUG_RETURN (0);                                /* ach 30/10/92 */
}

/*------------------------------------------------------------------------
 * inithp  --  initialize heap with 'desmax' descriptors and total size
 *             'size'
 * calls:      post_mortem.
 *------------------------------------------------------------------------
 */
int        inithp(base,size,ndes)
        char                       *base;  /* heap base                */
        int                        size;   /* size in bytes            */
        int                        ndes;   /* # of descriptors         */
{
        register T_DESCRIPTOR        *p, *q;
        register int                i, j;

        DBUG_ENTER ("inithp");

        heap_res = 0;                       /* clear reserved flag     */
        ALIGNTST(sizeof(T_DESCRIPTOR),T_HBLOCK); /* struct's ok ?         */
                                                 /* round to mult of 16   */
        base = (char *)(((unsigned)base + 15) & ~15);
        j = size & ~15;                          /* trunc to mult of 16   */

        /* 12 = number of standard descriptors remain permanent in heap   */
        /*     _work, _dig0, _dig1, _digRG, _digPI, _digLN2               */
        /*     _nilmat, _nilvect, _niltvect, _nil, _nilstring, _nilstruct */
        /* without standard decriptors created in rbibfunc.c              */
        if (ndes < 12)
          post_mortem("Init Heap: number of descriptors too small [ < 12 ]");

        i = j - DESLEN * ( ndes  + 1 );          /* i holds heap lenght   */
        if ((i & 15) != 0)
          post_mortem("---Init Heap: alignment error");
                                                 /* test alignment        */
        if (!(i>0))
          post_mortem("Init Heap: too many descriptors ");
        desmax = ndes;                           /* static info           */
        hpend = (T_HBLOCK *)((unsigned)(base)+i) - 1;
        ALIGNTST(hpend,T_HBLOCK);                /* test alignment        */
        StHeapD = (T_DESCRIPTOR *)(hpend+1);
        ASSERT(((unsigned)StHeapD & 15) == 0,"alignment");
                                                 /* test alignment        */
        hpbase = (T_HBLOCK *)base;               /* start of heap         */
        hpbase->next = hpend;                    /* point to dummy block  */
        hpbase->ref = NULL;                      /* mark free             */
        hpend->next = hpbase;                    /* chain wrap around     */
        hpend->ref = (T_HEAPELEM **) -1;         /* mark dummy used       */
        hplast = (T_HBLOCK *) hpbase;            /* start looking here    */

        init_var();                              /* init global variables */

       /* init descriptor chain */
        p = StHeapD;
        q = p+1;
        for (i=ndes ; --i ; ) {
                *(int *)p = 0;        /* ref_count|class|type = 0 ! */
                HEAP_CHAIN(p) = q++;
                p++;
        }
        HEAP_CHAIN(p) = NULL;
        EnHeapD = p;
        deslist = StHeapD;
        DESCHAIN;

       /* now save values for ReInitHeap        */
        odesmax = desmax;
        oStHeapD = StHeapD;
        oEnHeapD = EnHeapD;
        ohpbase = hpbase;
        ohpend = hpend;

        PRHEAP;
        DBUG_RETURN(1);
}

/*------------------------------------------------------------------------
 * ReInitHeap -- reinitialize heap with old values
 *               In the debug version this routine explicitately frees the
 *               old preprocessed expression kept by reduct. It checks,
 *               whether there are any unexprected descriptors left.
 * calls:        newheap, newdesc, post_mortem,
 *    if DEBUG:  HeapDump, DEC_REFCNT.
 * globals:      _old_expr_p <w>,  reduct:c
 *------------------------------------------------------------------------
 */
int ReInitHeap()                           /* ach 30/10/92: int */
{
#if !SCAVENGE
  register T_DESCRIPTOR *p, *q;
  register int          i;
#endif /* SCAVENGE    */                /* #if #endif eingefuegt von RS 6.11.1992 */ 
#if DEBUG
#if !SCAVENGE /* TB, 6.11.1992 */
  extern PTR_DESCRIPTOR _old_expr_p; 
  char c;
#endif

/*   FILE * fopen();       auskommentiert RS 7.12.1992 */      
extern int _filbuf();  /* RS 7.12.1992 */
extern int getkey();   /* RS 7.12.1992 */
extern unsigned sleep();    /* RS 7.12.1992 */
#endif

DBUG_ENTER("ReInitHeap");

#if    SCAVENGE
  init_var ();
  reinit_scavenge (heapsize);
#else /* SCAVENGE */
  if ( ohpbase == NULL )
    post_mortem("ReInitHeap: not yet initialized");
#if DEBUG
  if (_old_expr_p != NULL) {
    if (T_POINTER((int)_old_expr_p)) /* dg 15.05.90 */
      DEC_REFCNT(_old_expr_p);      /* free old kept expression */
    _old_expr_p = NULL;

    if ((descnt != stdesccnt) ||
        (R_DESC((*_dig0),ref_count) != 2) ||
        (R_DESC((*_dig1),ref_count) != 2) ||
        (R_DESC((*_digPI),ref_count) != 2) ||
        (R_DESC((*_digLN2),ref_count) != 2) ||
        (R_DESC((*_digRG),ref_count) != 2) ||
        (R_DESC((*_work),ref_count) != 2) ||
        (R_DESC((*_nilmat),ref_count)   != 1) ||
        (R_DESC((*_nilmat),type)   != TY_UNDEF) ||
        (R_DESC((*_nilvect),ref_count)  != 1) ||
        (R_DESC((*_nilvect),type)  != TY_UNDEF) ||
        (R_DESC((*_niltvect),ref_count) != 1) ||
        (R_DESC((*_niltvect),type) != TY_UNDEF) ||
        (R_DESC((*_nil),ref_count) != 1)   ||
        (R_DESC((*_nil),type) != TY_UNDEF) ||
        (R_DESC((*_nilstring),ref_count) != 1)    ||
        (R_DESC((*_nilstring),type) != TY_STRING) ||
        (R_DESC((*_nilstruct),ref_count) != 1)    ||
        (R_DESC((*_nilstruct),type) != TY_FRAME) ||
        (R_DESC((*_dig0),class)  != C_DIGIT) ||
        (R_DESC((*_dig1),class)  != C_DIGIT) ||
        (R_DESC((*_digPI),class) != C_DIGIT) ||
        (R_DESC((*_digLN2),class) != C_DIGIT) ||
        (R_DESC((*_digRG),class) != C_DIGIT) ||
        (R_DESC((*_work),class)  != C_DIGIT))
     {
       if (descnt > stdesccnt) {
         if ((descnt - stdesccnt) == 1)
           printf(" Warnung: Heap enthaelt einen ueberfluessigen Deskriptor! \n");
         else
           printf(" Warnung: Heap enthaelt %d ueberfluessige Deskriptoren! \n",descnt - stdesccnt);
       }
       else
         printf(" Warnung: Standard Deskriptoren im Heap sind defekt! \n");

       printf(" Ein heapdump erwuenscht auf Bildschirm (B) \n");
       printf("              erwuenscht auf heap:dump  (D) \n");
       printf("              unerwuenscht              (U) \n");
       printf(" Was soll geschehen : ");
       fflush(stdout);
       if ((c = getchar()) != '\n')
         (void)getchar();
       printf("                                            \n");

       switch(c) {
         case 'b':
         case 'B': DumpFile = stderr;
                   HeapDump("heaplisting");
                   printf("\n Bitte Return eingeben : ");
                   fflush(stdout);
                   (void)getkey();
                   sleep(2);
                   break;
         case 'd':
         case 'D': if ((DumpFile = fopen("heap.dump","w")) == NULL) {
                     printf(" heap.dump konnte nicht geoffnet werden! \n");
                     fflush(stdout);
                   }
                   else {
                     HeapDump("heaplisting");
                     fclose(DumpFile);
                   }
                   break;
         default:;
       }
     }  /* end of errorcase */
   }  /* end of _old_expr_p != Null */
#endif /* DEBUG */

   PRHEAP ;
   hpbase = ohpbase;
   hpend = ohpend;
   hpbase->next = hpend;                  /* point to dummy block   */
   hpbase->ref = NULL;                    /* mark free              */
   hpend->next = hpbase;                  /* chain wrap around      */
   hpend->ref = (T_HEAPELEM **) -1;       /* mark dummy used        */
   hplast = hpbase;                       /* start looking here     */
   heap_res = 0;
   StHeapD = oStHeapD;
   EnHeapD = oEnHeapD;
   desmax = odesmax;

   init_var();                              /* init global variables */

   /* init descriptor chain */
   p = StHeapD;
   q = p+1;
   for (i=desmax ; --i ; ) {
     *(int *)p = 0;        /* ref_count|class|type = 0 ! */
     HEAP_CHAIN(p) = q++;
     p++;
   }
   HEAP_CHAIN(p) = NULL;
   deslist = StHeapD;
   DESCHAIN;

   PRHEAP ;

#endif /* SCAVENGE */

   _work  = NULL;                    /* initialize standard descriptors */
   _dig0  = NULL;                    /* initialize standard descriptors */
   _dig1  = NULL;                    /* initialize standard descriptors */
   _digPI = NULL;                    /* initialize standard descriptors */
   _digRG = NULL;                    /* initialize standard descriptors */
   _digLN2 = NULL;                   /* initialize standard descriptors */
   _nilmat    = NULL;                /* initialize standard descriptors */
   _nilvect   = NULL;                /* initialize standard descriptors */
   _niltvect  = NULL;                /* initialize standard descriptors */
   _nil       = NULL;                /* initialize standard descriptors */
   _nilstring = NULL;                /* initialize standard descriptors */
   _nilstruct = NULL;                /* initialize standard descriptors */

#if DEBUG
   printf(" *** Reinit Heap  ok! *** \n");
#endif

#if (!D_SLAVE)
   initcode(heapsize);
#endif

   crea_global_desc();

   DBUG_RETURN(1);
}

#endif


#if SCAVENGE

#ifndef D_MESS_ONLY

void freedesc(p)
T_DESCRIPTOR *p;
{
}

/* auskommentiert von ach 06/11/92 
static T_HBLOCK *get_HBLOCK(n_int)
int n_int;
{
  return(0);*/           /* RS 6.11.1992 */ 
/* } */

void freeheap(p)
T_HBLOCK *p;
{
}

void cutheap(p,i)        /* void eingefuegt von RS 6.11.1992 */ 
T_HBLOCK *p;
int      i;
{
 return;           /* return eingefuegt von RS 6.11.1992 */ 
}

int compheap()        /* int eingefuegt von ach 06/11/92 */ 
{
return 0;              /* return 0 eingefuegt von ach 06/11/92 */ 
}

/* auskommentiert von ach 06/11/92 
static int  moredes()*/      /* int eingefuegt von ach 06/11/92 */ 
/*{
return 0;*/          /* return 0 eingefuegt von ach 06/11/92 */ 
/* } */

void inc_idcnt()
{
   idcnt++;
}

void dec_idcnt()
{
   if (idcnt > 0) idcnt--;
}

int dealloc_sc(desc)
PTR_DESCRIPTOR desc;
{
return (0);         /* return eingefuegt von RS 6.11.1992 */ 
}

int dealloc_di(desc)
PTR_DESCRIPTOR desc;
{
return(0);   /* return eingefuegt von RS 6.11.1992 */ 
}

/* cr 09/03/95, kir(ff), START */
int dealloc_frame(desc)
PTR_DESCRIPTOR desc;
{
return(0);      /* return eingefuegt von RS 6.11.1992 */ 
}

int dealloc_inter(desc)
PTR_DESCRIPTOR desc;
{
return(0);      /* return eingefuegt von RS 6.11.1992 */ 
}
/* cr 09/03/95, kir(ff), END */

int dealloc_intact(desc)    /* stt  11.09.95 */
PTR_DESCRIPTOR desc;
{
return(0);
}

int dealloc_li(desc)
PTR_DESCRIPTOR desc;
{
return(0);      /* return eingefuegt von RS 6.11.1992 */ 
}

int dealloc_mvt(desc)
PTR_DESCRIPTOR desc;
{
return(0);         /* return eingefuegt von RS 6.11.1992 */ 
}

int dealloc_ex(desc)
PTR_DESCRIPTOR desc;
{
return(0);         /* return eingefuegt von RS 6.11.1992 */ 
}

static int dealloc_co(desc)
PTR_DESCRIPTOR desc;
{
return(0);          /* return eingefuegt von RS 6.11.1992 */ 
}

static int dealloc_fu(desc)
PTR_DESCRIPTOR desc;
{
return(0);        /* return eingefuegt von RS 6.11.1992 */ 
}

static int dealloc_cns(desc)
PTR_DESCRIPTOR desc;
{
return(0);       /* return eingefuegt von RS 6.11.1992 */ 
}

static int dealloc_mat(desc)
PTR_DESCRIPTOR desc;
{
return(0);       /* return eingefuegt von RS 6.11.1992 */ 
}

static int dealloc_pat(desc)
PTR_DESCRIPTOR desc;
{
return(0);       /* return eingefuegt von RS 6.11.1992 */ 
}

static int dealloc_ty(desc)
PTR_DESCRIPTOR desc;
{
return(0);       /* return eingefuegt von RS 6.11.1992 */ 
}

static int dealloc_ln(desc)
PTR_DESCRIPTOR desc;
{
return(0);       /* return eingefuegt von RS 6.11.1992 */ 
}

/* auskommentiert von ach 06/11/92
static  void freeexpr(p)
register PTR_HEAPELEM        p;
{
return;*/             /* void und return eingefuegt von RS 6.11.1992 */ 
/*}*/

void hpchain()
{
return;         /* return und void eingefuegt von RS 6.11.1992 */ 
}

int        isheap(p)
register T_HBLOCK        *p;
{
  return(1);
}

int isdesc(p)
register PTR_DESCRIPTOR        p;
{
  return(T_POINTER(p));
}

/*------------------------------------------------------------------------
 * res_heap  --  reserve heap to prevent heap compaction
 *------------------------------------------------------------------------
 */
void res_heap()           /* void eingefuegt von RS 6.11.1992 */ 
{
#if        DEBUG
        if (heap_res != 0)
                printf("res_heap: Warning: heap is already reserved!\n");
#endif
        heap_res++;
return;      /* RS 6.11.1992 */ 
}
 
/*------------------------------------------------------------------------
 * rel_heap  --  release heap to allow heap compaction
 *------------------------------------------------------------------------
 */
void rel_heap()            /* void eingefuegt von RS 6.11.1992 */  
{
#if        DEBUG
        if (heap_res == 0)
                printf("rel_heap: Warning: heap is not reserved!\n");
#endif
 
        heap_res--;
return;        /* RS 6.11.1992 */ 
}

void deschain()        /* void eingefuegt von RS 6.11.1992 */  
{
return;        
}

void inc_refcnt(p)         /* void eingefuegt von RS 6.11.1992 */  
register PTR_DESCRIPTOR        p ;
{
return;          /* RS 6.11.1992 */ 
}

void dec_refcnt(p)            /* void eingefuegt von RS 6.11.1992 */  
register PTR_DESCRIPTOR p;
{
return;       /* RS 6.11.1992 */ 
}

void test_inc_refcnt(ident,fileid,lineid)      /* void eingefuegt von RS 6.11.1992 */  
PTR_DESCRIPTOR        ident ;
char * fileid;
int lineid ;
{
return;          /* RS 6.11.1992 */ 
}

void test_dec_refcnt(ident,fileid,lineid)      /* void eingefuegt von RS 6.11.1992 */  
PTR_DESCRIPTOR ident ;
char * fileid;
int lineid ;
{
return;         /* RS 6.11.1992 */ 
}

void dealloc(p)       /* void eingefuegt von RS 6.11.1992 */  
PTR_DESCRIPTOR p;
{
 return;      /* RS 6.11.1992 */ 
}

int inchain(p)
T_DESCRIPTOR *p;
{
return(0);         /* RS 6.11.1992 */ 
}

void OutDescArea(XFile)
FILE *XFile   ;
{
return;           /* RS 6.11.1992 */ 
}

int *DumpElem(XFile,p)
FILE     *XFile   ;
T_HBLOCK *p;
{
return(0);       /* RS 6.11.1992 */ 
}

void OutHeapArea(XFile)
FILE *XFile   ;
{
return;      /* RS 6.11.1992 */ 
}

void HeapDump(Ident)          /* void eingefuegt von RS 6.11.1992 */  
char *Ident ;
{
return;           /* RS 6.11.1992 */ 
}

#endif

#else /* SCAVENGE */

#ifndef D_MESS_ONLY

/*------------------------------------------------------------------------
 * newdesc  --  return an identifier to a descriptor, allocate more
 *              descriptors if necessary, return a null, if heap is full.
 * calls:       moredes.
 *------------------------------------------------------------------------
 */
T_DESCRIPTOR *newdesc()

#else

/*------------------------------------------------------------------------
 * m_newdesc  --  return an identifier to a descriptor, allocate more
 *              descriptors if necessary, return a null, if heap is full.
 * calls:       moredes. (measure version here)
 *------------------------------------------------------------------------
 */
T_DESCRIPTOR *m_newdesc()

#endif

{
        register T_DESCRIPTOR *p;

#ifdef D_MESS_ONLY
DBUG_ENTER ("m_newdesc");
#else
DBUG_ENTER ("newdesc"); 
#endif

#if MEASURE
  measure(MS_NEWDESC);
#endif

#if        DEBUG
        if (heap_res)
        {
                printf("newdesc: Warning: heap was reserved");
                REL_HEAP;
        }
        if (stopheap == 0) DBUG_RETURN((T_DESCRIPTOR *) NULL);
        if (stopheap >  0) stopheap--;
#endif

        DESCHAIN;

        if (deslist == NULL)
        {
          moredes();               /* get more descriptors from heap */
          if (deslist == NULL)
          {
              fprintf(stderr,"newdesc: heap out of space\n");
              DBUG_RETURN((void *)NULL);
          }
        }
        p = deslist;
        deslist = HEAP_CHAIN(deslist);
        descnt++;
#if MEASURE
       {
        extern int mdesc;
        if (descnt > mdesc) mdesc = descnt;
       }
#endif

/*        ASSERT(StHeapD<=p && p<=EnHeapD,"address not desc area"); */

	if (StHeapD>p || p>EnHeapD)
	  printf("AU SCHEISSE");

#if        DEBUG
        HEAP_CHAIN(p) = NULL ;

        if (debcnt)
                printf("newdesc at %8.8x: %d used, %d max\n",
                               (unsigned int)p,descnt,desmax);
        if ((debdesc == (int) p) && !debcnt)
                printf("desc allocated %8.8x: %d used, %d max\n",
                               (unsigned int)p,descnt,desmax);
#endif
        DESCHAIN;

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MHEAP)
        MPRINT_DESC_ALLOC(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) p);
        DBUG_PRINT ("MESS", ("MPRINT_DESC_ALLOC executed, arg: %d", (int) p));
#endif
#endif

#if D_SLAVE
	if (highdesc < p)
	  highdesc = p;
#endif

        DBUG_PRINT("ALLOC", ("alloc %08x", (int)p));
        DBUG_RETURN(p);
}

#ifndef D_MESS_ONLY

/*------------------------------------------------------------------------
 * freedesc  --  free a descriptor, just put it into chain
 *------------------------------------------------------------------------
 */
void
freedesc(p)
        register T_DESCRIPTOR *p;

#else

/*------------------------------------------------------------------------
 * m_freedesc  --  free a descriptor, just put it into chain  - measure version
 *------------------------------------------------------------------------
 */
void
m_freedesc(p)
        register T_DESCRIPTOR *p;

#endif
{
#ifdef D_MESS_ONLY
DBUG_ENTER ("m_freedesc");
#else
DBUG_ENTER ("freedesc");
#endif

        if (p == NULL)  DBUG_VOID_RETURN; 

        DESCHAIN;

        ISDESC(p);

#ifdef D_MESS_ONLY
  DBUG_PRINT ("MESS", ("i am in m_freedesc !"));
#endif

        FREEDESC(p);

        DESCHAIN;

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MHEAP)
        MPRINT_DESC_FREE(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int) p);
        DBUG_PRINT ("MESS", ("MPRINT_DESC_FREE executed, arg: %d", (int) p));
#endif
#endif

#if        DEBUG
        if (debcnt)
                printf("freedesc: %d used, %d max\n",descnt,desmax);
        if ((debdesc == (int) p) && !debcnt)
                printf("freedesc: %d used, %d max\n",descnt,desmax);
#endif

DBUG_VOID_RETURN;
}

#ifndef D_MESS_ONLY

/*------------------------------------------------------------------------
 * get_HBLOCK  --  get a heap element with size 'n_int*sizeof(int)'
 *                   first fit, compact heap if necessary
 * calls          compheap.
 *------------------------------------------------------------------------
 */
#if D_MESS
        T_HBLOCK        *get_HBLOCK(n_int)
        int                n_int;                /* # of int's to alloc   */
#else
static        T_HBLOCK        *get_HBLOCK(n_int)
        int                n_int;                /* # of int's to alloc   */
#endif
{
        register T_HBLOCK        *p, *q, *r;
        register int                nb;          /* # of blocks to alloc  */
        int                        retryflg = 0; /* try 1 garbage collect */

DBUG_ENTER ("get_HBLOCK");

#if MEASURE
#endif

        if (n_int == 0) {
#if    DEBUG
        if (debcnt)
                printf(
                "get_HBLOCK:  No request ! \n");
#endif
                DBUG_RETURN((void *)NULL);
        }
        nb = (n_int*sizeof(T_HEAPELEM) + ALIGN - 1) / ALIGN;
        nb++;                                   /* we need 1 ourselves   */
#if MEASURE
        m_heap(MS_NEWHEAP,nb);
#endif
        HPCHAIN;
#if        DEBUG
        if (debcnt)
                printf(
                "get_HBLOCK:  %d req HBLOCK  ",nb);
#endif

        for( ; ; ) {                     /* retry loop after heap compact */
                p = hplast;
                for( ; ; ) {/* loop until once around (hplast==p again) */
                        if(!testbusy(p)) {
                                for (q=p->next ; !testbusy(q) ;) {
                                        ASSERT(q>p&&q<hpend,
                                                "heap chain corrupted");
                                        q = p->next = q->next;
                                }

#if MEASURE
#endif

                                if(q>=p+nb)
                                  goto found;
                                else if ((p < hplast)&&(p->next > hplast))
                                         break;  /* new created block would skip hplast */
                        }
                        q = p;
                        p = p->next;
#if MEASURE
                        m_heap(MS_HEAPSEARCH,n_int);
#endif
                        if(p>q)
                                ASSERT(p<=hpend,"heap chain corrupted");
                        else {
                                ASSERT(q==hpend&&p==hpbase,
                                        "heap chain corrupted");
                        }
                        if (hplast == p)
                                break;
                }

                if (++retryflg > 1)
                    DBUG_RETURN((void *)NULL);

#if        DEBUG
        if (debcnt)
                printf(
                "get_HBLOCK: COMPACT for req %d integers, %d HBLOCKs\n",
                n_int,nb);
#endif

#if (D_SLAVE && D_MESS && D_MHEAP)
                (*d_m_compheap)();
#else
                compheap();               /* compact heap */
#endif
        }

found:
        r = p + nb;
        ASSERT(r<=hpend,"internal heap error");
        ALIGNTST(r,T_HBLOCK);               /* test alignment */
        if(q>r) {
                r->next = q ;    /* p-> next */
                r->ref = NULL;
                p->next = r;
        }
        hplast = r;
#if        DEBUG
        if (debcnt)
                printf(
                "get_HBLOCK:  %d HBLOCKs, ret: %-8.8x\n"
                ,nb,(unsigned int)p);
#endif
        DBUG_RETURN(p);
}

#endif

#ifdef D_MESS_ONLY

/*------------measure newheap !!!-----------------------------------------
 * m_newheap  --  get a heap element with size 'n_int*sizeof(int)'
 *                first fit, compact heap if necassary
 *              returns a 1 if successfull, 0 otherwise.
 * calls        get_HBLOCK.
 *------------------------------------------------------------------------
 */
int
m_newheap(n_int,p_ref)
        int                n_int;                /* # of int's to alloc   */
        T_HEAPELEM        **p_ref;               /* back reference        */

#else

/*------------------------------------------------------------------------
 * newheap  --  get a heap element with size 'n_int*sizeof(int)'
 *                first fit, compact heap if necassary
 *              returns a 1 if successfull, 0 otherwise.
 * calls        get_HBLOCK.
 *------------------------------------------------------------------------
 */
int
newheap(n_int,p_ref)
        int                n_int;                /* # of int's to alloc   */
        T_HEAPELEM        **p_ref;               /* back reference        */

#endif
{
        register T_HBLOCK        *p;

#ifdef D_MESS_ONLY
DBUG_ENTER("m_newheap");
#else
DBUG_ENTER("newheap");
#endif

#if        DEBUG
        if (heap_res)
        {
                printf("newheap: Warning heap was reserved");
                REL_HEAP;
        }
        if (stopheap == 0)
        {
          *p_ref = (T_HEAPELEM *) NULL;
          DBUG_RETURN(0);
        }
        if (stopheap > 0) stopheap--;

        if (n_int < 0) post_mortem("newheap: got a negative heapsize");
#endif

        ASSERT((unsigned)StHeapD<=(unsigned)(p_ref),
                "backreference not to descriptor area");
        ASSERT((unsigned)(p_ref) < (unsigned)(EnHeapD+1),
                "backreference not to descriptor area");     

        p = get_HBLOCK(n_int);

        if (p == NULL)
          if (n_int == 0)
          {
                *p_ref = NULL;
                DBUG_RETURN(1);
          }
          else
          {
            *p_ref = (T_HEAPELEM *) NULL;
/* message is always printed out on stdout !!!!!!!!!!!!!!!!!  */
            fprintf(stderr,"newheap: Heap out of Space\n");
            DBUG_RETURN(0);
          }
        p->ref = p_ref;
        *p_ref = (T_HEAPELEM *)(p+1);

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MHEAP)
         MPRINT_HEAP_ALLOC (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (n_int*sizeof(int)), (int) (p+1), (int) p_ref);
         DBUG_PRINT ("MESS", ("MPRINT_HEAP_ALLOC executed, arg: %d", (int) (p+1)));
#endif
#endif

        DBUG_RETURN(1)   ;
}

#ifndef D_MESS_ONLY

/*------------------------------------------------------------------------
 * newbuff  --  get a heap element with size 'n_int*sizeof(int)'
 *              returns a null when heap is full.
 *------------------------------------------------------------------------
 */
T_HEAPELEM        *newbuff(n_int)
        int                n_int;            /* # of int's to alloc        */
{
        register T_HBLOCK        *p;

 DBUG_ENTER ("newbuff");

#if        DEBUG
        if (heap_res)
                printf("newheap: Warning:heap is reserved");
#endif

        p = get_HBLOCK(n_int);

        if (p == NULL) {
                DBUG_RETURN((void *)NULL);
        }

        p->ref = (T_HEAPELEM **) -1;       /* mark dummy used        */
        DBUG_RETURN((T_HEAPELEM *)(p+1));
}

#endif

#ifdef D_MESS_ONLY

/*------------------------------------------------------------------------
 * m_freeheap  --  free a heap element (measure version)
 *------------------------------------------------------------------------
 */
void    m_freeheap(p)
        T_HBLOCK         *p;

#else

/*------------------------------------------------------------------------
 * freeheap  --  free a heap element
 *------------------------------------------------------------------------
 */
void    freeheap(p)
        T_HBLOCK         *p;

#endif
{
#ifdef D_MESS_ONLY
DBUG_ENTER ("m_freeheap");
#else
DBUG_ENTER ("freeheap");
#endif

        if (p == NULL) DBUG_VOID_RETURN;

        p--;              /* goto the block before */
        HPCHAIN;
        ISHEAP(p);
        ASSERT(testbusy(p),"heap element was already freed");

        p++;  
#ifdef D_MESS_ONLY
 { T_HBLOCK *_h_ = (T_HBLOCK *) p; 
                      if (_h_ != NULL) { 
#if (D_SLAVE && D_MESS && D_MHEAP)
    MPRINT_HEAP_FREE(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int )p, (int)(_h_-1)->ref);
    DBUG_PRINT ("MESS", ("MPRINT_HEAP_FREE executed, arg: %d", p));
#endif
                      HPCHAIN; --_h_; 
                      ISHEAP(_h_); _h_->ref = NULL; }}
#else
        FREEHEAP(p)
#endif
        p--;

        ASSERT(p->next > p && p->next <= hpend,"heap chain corrupted");
DBUG_VOID_RETURN;
}

#ifndef D_MESS_ONLY

/*------------------------------------------------------------------------
 * cutheap  -- shorten a heap element to i integers, but multiples of
 *             ALIGN
 *------------------------------------------------------------------------
 */
void cutheap(p,i)                           /* ach 30/10/92: void */
        register T_HBLOCK         *p;
        register int                i;
{
        register T_HBLOCK         *q;
        int                j,k;

DBUG_ENTER("cutheap");

        --p;                                  /* go to block before        */
        ISHEAP(p);
        ASSERT(testbusy(p),"heap element not in use");
        k = (i * sizeof(T_HEAPELEM) + ALIGN - 1) / ALIGN;
        if ( (j=(p->next)-p) <= (k+1) )
                                               /* not enough to make        */
                DBUG_VOID_RETURN;                        /* new block                */

        j -= 1 + k;
        q = p->next - j;                       /* q points to new block  */
        ALIGNTST(q,T_HBLOCK);                  /* test alignment         */
        q->next = p->next;                     /* chain it in            */
        q->ref = NULL;
        p->next = q;
        HPCHAIN;

DBUG_VOID_RETURN;
}

#endif

#ifdef D_MESS_ONLY

/*------------------------------------------------------------------------
 * m_compheap  --  compact heap, update ptrs in desc's and return 1, if
 *               there was free space to compact, return 0 otherwise
 *------------------------------------------------------------------------
 */

int m_compheap()                    /* ach 30/10/92: int */

#else

/*------------------------------------------------------------------------
 * compheap  --  compact heap, update ptrs in desc's and return 1, if
 *               there was free space to compact, return 0 otherwise
 *------------------------------------------------------------------------
 */

int compheap()                    /* ach 30/10/92: int */

#endif

{
        register T_HBLOCK        *hpp, *hpq, *hpr, *hpx, *hpy;
        register T_DESCRIPTOR    *p    ;    
        T_HBLOCK                 *hpf;
        int                      free_flg = 0;
        int                      buffer_flg = 0;
        int                      /*i,*/free_cnt = 0 ;   /* ach 30/10/92 */
        int                      idc = 0; /* help variable (indirect list-descriptor count) */

#ifdef D_MESS_ONLY
DBUG_ENTER ("m_compheap");
#else
DBUG_ENTER ("compheap");
#endif

#if D_SLAVE
        PRDEBUG("heap compaction initiated.\007\007\n");
        fprintf(stderr,"node %d: heap compaction initiated.\007\007\n",proc_id);
#else
        PRDEBUG("heap compaction initiated.\007\007\n");
        fprintf(stderr,"heap compaction initiated.\007\007\n");
#endif

#ifdef D_MESS_ONLY
#if (D_SLAVE && D_MESS && D_MHEAP)
        MPRINT_HEAP_COMPACT (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID);
#endif
#endif

#if        DEBUG
        if (heap_res)
        {
           printf("res_heap: heap is reserved\n");
           DBUG_RETURN(0);
        }
#endif
        HPCHAIN;

        hpp = hpbase; 
        hpf = hpend;
        for (;;) {
               /* look for first free block */
                for (;(hpp!=hpend)&&testbusy(hpp);hpp=hpp->next)
                        ASSERT(hpbase<=hpp && hpp<=hpend,"heap chain corrupted");
                if (hpp==hpend) break;       /* return, if none free        */

                free_cnt += (hpp->next - hpp) * sizeof(*hpp);

                for (hpq = hpp->next;;) {
                        hpf = hpp;   
                        ASSERT(hpp<=hpq,"heap chain corrupted");
                       /* look for first used block after current free block */
                        for (;(hpq!=hpend)&&!testbusy(hpq);hpq=hpq->next) { /* free block */
                                free_cnt += (hpq->next - hpq) * sizeof(*hpq);
                                ASSERT(hpbase<=hpq && hpq<=hpend,"heap chain corrupted");
                        }
                        if ((int)(hpq->ref)==-1) {/* break, if hpend or buffer */
                                hpp = hpq;        
                                if (hpp != hpend) 
                                 {
                                  buffer_flg++;
                                  hpf = hpend;    /* kein freier Block zuletzt */
                                 }
                                break;
                        }

                        ASSERT(hpp<hpq,"heap chain corrupted");
                        free_flg++;     /* heap changed, big free blk         */

                                         /* valid back reference ?            */
                        ASSERT((unsigned)StHeapD <= (unsigned)(hpq->ref),
                                "back reference not to desc");
                        ASSERT((unsigned)(hpq->ref) < (unsigned)(EnHeapD+1),
                                "back reference not to desc");
                        ASSERT(*(hpq->ref) == (int *)(hpq+1),
                                "back reference corrupted");
                        *hpq->ref = (T_HEAPELEM *)(hpp+1); /* update descr   */
                        hpp->ref = hpq->ref;       /* copy back reference    */
                        hpr = hpq->next;  /* block after the one to be moved */
                        for(hpx=hpp+1,hpy=hpq+1;hpy<hpr;hpx++,hpy++){
                                *hpx = *hpy;                   /* copy block */
                        }
                        ASSERT(hpy==hpr,
                                "alignment");        /* was alignment ok ? */
                        hpp->next = hpx; /* new free block starts after copied*/
                             /* new free block points to follower of old used */
                        hpx->next = hpr;
                        hpx->ref = NULL;   /* block is free                   */
                        hpp = hpx;         /* update ptr of lowest free block */
                        hpq = hpr;         /* next block to be inspected      */

                }                              

                if (hpp==hpend)
                        break;
        }
        hplast = hpbase;
        if (hpf != hpend)
         { 
           hpf->next = hpend;           /* Verschmelzen der letzten Bloecke */
           hplast = hpf;
         }
        HPCHAIN;
        if (free_flg)
         {
          if (!buffer_flg)                        
           {
#if D_SLAVE                    
             fprintf(stderr,"node %d: Heap compaction successful: %d bytes free ; %d descriptors out of %d used\n", proc_id,
#else
             fprintf(stderr,"Heap compaction successful: %d bytes free ; %d descriptors out of %d used\n", 
#endif
                    free_cnt, descnt,desmax); 
           }
          else                           /* buffer found */
           {
#if D_SLAVE
             fprintf(stderr,"node %d: Heap compaction successful: %d bytes free (last block is %ld bytes); %d descriptors out of %d used\n", proc_id,
#else
             fprintf(stderr,"Heap compaction successful: %d bytes free (last block is %ld bytes); %d descriptors out of %d used\n", 
#endif
                    free_cnt, (hpend - hpf) * sizeof(*hpp),descnt,desmax); 
           } 
         }
        else
#if D_SLAVE
	  fprintf(stderr,"node %d: No compaction possible - %d bytes free\n",proc_id,free_cnt);
#else
          fprintf(stderr,"No compaction possible - %d bytes free\n",free_cnt);
#endif
        fflush(stdout);
 
   /*------------------------------------------------------*/
   /* UPDATE INDIRECT DESCRIPTORS   (if exist one)         */
   /* move through the descriptor chain and update the     */
   /* pointer_to_data_vector of the indirect descriptors   */
   /*------------------------------------------------------*/
# if DEBUG
   if (idcnt > descnt)
    post_mortem("compheap: invalid number of indirect descriptors");
# endif
   if (idcnt > 0) {
     for(p = StHeapD; p <= EnHeapD ; p++ ) { /* loop: move to next descriptor */
       if ((int)R_DESC((*p),ref_count) > (int)NULL) {
         switch(R_DESC((*p),class)) {
	   /* do not update remote descriptors ! */
           case C_LIST    : if ((R_LIST((*p),ptdd) != NULL) && (R_DESC((*p), type) != TY_REMOTE)) {
                              /* add new ptdv of direct list-descriptor  */
                              /* to  offset of indirect-list-descriptor  */
                              L_LIST((*p),ptdv) =
                                     R_LIST((*(R_LIST((*p),ptdd))),ptdv) +
                                     R_LIST((*p),special);
                               idc++;
                             }
                             break;
           case C_VECTOR  : if (R_MVT((*p),ptdd,C_VECTOR) != NULL) {
                               /* no use of macro for direct descriptor  */
                               /* because class of dir. desc. unknown    */
                               L_MVT((*p),ptdv,C_VECTOR) =
                                    (*(R_MVT((*p),ptdd,C_VECTOR))).u.mvt.ptdv ;
                               idc++;
                            }
                            break;
           case C_TVECTOR : if (R_MVT((*p),ptdd,C_TVECTOR) != NULL) {
                              /* no use of macro for direct descriptor  */
                              /* because class of dir. desc. unknown    */
                              L_MVT((*p),ptdv,C_TVECTOR) =
                                   (*(R_MVT((*p),ptdd,C_TVECTOR))).u.mvt.ptdv ;
                              idc++;
                           }
                           break;
           case C_MATRIX  : if (R_MVT((*p),ptdd,C_MATRIX) != NULL) {
                               /* no use of macro for direct descriptor  */
                               /* because class of dir. desc. unknown    */
                               L_MVT((*p),ptdv,C_MATRIX) =
                                    (*(R_MVT((*p),ptdd,C_MATRIX))).u.mvt.ptdv ;
                               idc++;
                            }
                            break;
         } /** switch class **/
         if (idc == idcnt) break; /* if max count of indirect descriptors reached */
       }
     }
   }
        DBUG_RETURN (free_flg);
}

#ifndef D_MESS_ONLY

/*------------------------------------------------------------------------
 * moredes  --  get more descriptors from end of heap, shorten heap
 * calls:       compheap.
 *------------------------------------------------------------------------
 */
#if D_MESS
int   moredes()
#else
static int   moredes()              /* ach 30/10/92: int */
#endif
{
        register T_HBLOCK        *hpp;
        register T_DESCRIPTOR        *p, *q;
        register int                i;
        int        retryflg = 0;

DBUG_ENTER ("moredes");

#if        DEBUG
        printf("Trying to allocate more descriptors from heap.\007\007\n");
#endif

        DESCHAIN;
        HPCHAIN;

       /* get last and only block before end */
retry:
        for (hpp=hpbase ; hpp->next!=hpend ; hpp=hpp->next)
                {}

        if (testbusy(hpp)) {              /* compact, if busy               */

#if (D_SLAVE && D_MESS && D_MHEAP)
                (*d_m_compheap)();
#else
                compheap();               /* compact heap                   */
#endif
                retryflg++;               /* don't retry, already compacted */
                goto retry ;
        }

                                              /* no. of descr in heap block */
        i = (hpend - hpp)*sizeof(T_HBLOCK)/DESLEN;
        if ( (i < 10) || (hpp->next != hpend) ) {
                if (++retryflg > 1)
                  DBUG_RETURN(0); /* no space for descriptors available */

#if (D_SLAVE && D_MESS && D_MHEAP)
                (*d_m_compheap)();
#else
                compheap();               /* compact heap */
#endif
                goto retry;
        }
        if (i>desmax/3)
                i = desmax/3;                   /* limit no of new descr's */
       /* decrease upper end of heap */
        hpend = hpend - ((i*DESLEN+ALIGN-1)/ALIGN);
        ALIGNTST(hpend,T_HBLOCK);               /* test alignment */
        hpend->next = hpbase;
        hpend->ref = (int **)-1;
        if (hpp != hpend)
                hpp->next = hpend;
        HPCHAIN;
        StHeapD -= i;
        desmax += i;                              /* update total descr count */
        p = StHeapD;
        q = p + 1;
        for ( ; --i ; ) {
                *(int *)p = 0;        /* ref_count|class|type = 0 ! */
                HEAP_CHAIN(p) = q++;
                p++;
        }
        HEAP_CHAIN(p) = deslist;
        deslist = StHeapD;
        DESCHAIN;

#if        DEBUG
           printf(
                   "moredesc: %d used, new maximum is %d \n",descnt,desmax);
#endif

        DBUG_RETURN(1);
}

/*---------------------------------------------------------------------------*/
/* inc_idcnt  increment indirect descriptor counter                          */
/*            Modul nur fuer Debug-Version  !!!!!                            */
/*---------------------------------------------------------------------------*/
#if DEBUG
void inc_idcnt()
{
DBUG_ENTER("inc_idcnt");

   idcnt++;

DBUG_VOID_RETURN;
}

/*---------------------------------------------------------------------------*/
/* dec_idcnt          decrement indirect descriptor counter                  */
/*            Modul nur fuer Debug-Version  !!!!!                            */
/*---------------------------------------------------------------------------*/
void dec_idcnt()
{
DBUG_ENTER ("dec_idcnt");

   if (idcnt > 0) idcnt--;

DBUG_VOID_RETURN;
}
#endif

/*------------------------------------------------------------------------
 * dealloc_sc -- Freigabe eines Skalardeskriptors
 * ruft auf:     FREEDESC.
 *------------------------------------------------------------------------
 */
int dealloc_sc(desc)
PTR_DESCRIPTOR desc;
{
DBUG_ENTER ("dealloc_sc");
 
  ISDESC(desc);
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_sc desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int) desc) && !debcnt)
                printf(
                "dealloc_sc desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /* DEBUG */
  FREEDESC(desc);
  DBUG_RETURN (0);                     /* ach 30/10/92 */
} /**** end of function dealloc_sc ****/

/*****************************************************************************/
/*                                dealloc_di                                 */
/*---------------------------------------------------------------------------*/
/* Funktion :    Freigabe eines Digit-Deskriptors                            */
/* calls    :    FREEDESC, FREEHEAP.                                         */
/*****************************************************************************/
int dealloc_di(desc)
PTR_DESCRIPTOR desc;
{
DBUG_ENTER ("dealloc_di");

  ISDESC(desc);
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_di desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int) desc) && !debcnt)
                printf(
                "dealloc_di desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /*  DEBUG */
  FREEHEAP(R_DIGIT((*desc),ptdv));
  FREEDESC(desc);
  DBUG_RETURN(0);                    /* ach 30/10/92 */
}

/*------------------------------------------------------------------------
 * dealloc_li -- Freigabe eines Listen-Deskriptors.
 *               Listen werden von hinten nach vorne aufgefuellt.
 *               Eine Null als Listenelement zeigt eine nur partiell
 *               aufgebaute Liste an.
 * ruft auf:     FREEDESC, FREEHEAP, DEC_REFCNT.
 *------------------------------------------------------------------------
 */
int dealloc_li(desc)
PTR_DESCRIPTOR desc;
{
  register int dim;
  int x;

DBUG_ENTER("dealloc_li");

DBUG_ASSERT(desc != _nil);

ISDESC(desc);

#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_li desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int)desc) && !debcnt)
                printf(
                "dealloc_li desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /*  DEBUG  */

 /* handles remote descriptors
    because remote adress is stores]d in ptdv */
 if (R_LIST((*desc),ptdv) == NULL)
 {
   FREEDESC(desc);
   DBUG_RETURN(0);                     /* ach 30/10/92 */   
 }

 if (R_LIST((*desc),ptdd) != (T_PTD)0)  /* if indirect list-descriptor */
 {
  DEC_REFCNT((PTR_DESCRIPTOR)(R_LIST((*desc),ptdd)));
              /* decrement refcount of descriptor, pointes at  */
  FREEDESC(desc);
  DEC_IDCNT;  /* decrement counter of indirect list-descriptor */
# if DEBUG
  if (idcnt < 0)
    post_mortem("indirect discriptor counter is < 0");
# endif
 }
 else
 {
  dim = R_LIST((*desc),dim);
  for ( ; --dim >= 0; )
    if T_POINTER((x = ((int *) R_LIST((*desc),ptdv))[dim]))
      if (x != (int)NULL)
        DEC_REFCNT((PTR_DESCRIPTOR)x);
      else break;   /* partially filled list */
  FREEHEAP(R_LIST((*desc),ptdv));
  FREEDESC(desc);
 }
 DBUG_RETURN(0);                    /* ach 30/10/92 */
} /**** end of function dealloc_li ****/


/****************************************************************************/
/*                             dealloc_mvt                                  */
/*------------------------------------------------------------------------- */
/* Funktion :  Freigabe eines Matrix- oder eines Vector- oder               */
/*             eises transponierten Vector-Decriptors.                      */
/*             "Sie werden von vorne nach hinten aufgefuellt."              */
/*             Eine Null als Matrix-Element kennzeichnet eine nur           */
/*             partiell aufgebaute Matrix. analog mit VECTOR / TVECTOR      */
/* Parameter:  desc  -  zu deallozierender Deskriptor                       */
/* Aufrufe  :  FREEDESC, FREEHEAP, DEC_REFCNT, ISDESC                       */
/****************************************************************************/
int dealloc_mvt(desc)
PTR_DESCRIPTOR desc;
{
  register int  i,size;
  register int  x;
  register char class1;

DBUG_ENTER ("dealloc_mvt");

  ISDESC(desc);
  class1 = R_DESC((*desc),class);
#if DEBUG
        if (debcnt || ((debdesc == (int) desc) && !debcnt)) {
           switch (class1) {
             case C_MATRIX  :
                  printf("dealloc_mvt : C_MATRIX\n");
                  break;
             case C_VECTOR  :
                  printf("dealloc_mvt : C_VECTOR\n");
                  break;
             case C_TVECTOR :
                  printf("dealloc_mvt : C_TVECTOR\n");
                  break;
           } /** switch class1 **/
        }
        if (debcnt)
          printf("dealloc_mvt desc %-8.8x ref: %d \n",(unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int) desc) && !debcnt)
          printf("dealloc_mvt desc %-8.8x ref: %d \n",(unsigned int)desc,R_DESC((*desc),ref_count));
#endif

  if (R_MVT((*desc),ptdd,class1) != (T_PTD)0) { /* if indirect descriptor */
    DEC_REFCNT((PTR_DESCRIPTOR)(R_MVT((*desc),ptdd,class1)));
                          /* decrement refcount of descriptor, pointes at */
    FREEDESC(desc);
    DEC_IDCNT;            /* decrement counter of indirect descriptor     */
# if DEBUG
    if (idcnt < 0)
      post_mortem("indirect discriptor counter is < 0");
# endif
  }
  else {
    if (R_DESC((*desc),type) >= TY_STRING) {
      if (R_MVT((*desc),ptdv,class1) == NULL) {
        FREEDESC(desc);
        DBUG_RETURN(0);                          /* ach 30/10/92 */
      }
      size = R_MVT((*desc),nrows,class1) * R_MVT((*desc),ncols,class1);
      for (i = 0; i < size; i++)
        if ((x = ((int *)R_MVT((*desc),ptdv,class1))[i]) != (int)NULL)
          DEC_REFCNT((PTR_DESCRIPTOR)x);
        else
          break;               /* partially filled matrix / vector / tvector */
    }
    FREEHEAP(R_MVT((*desc),ptdv,class1));
    FREEDESC(desc);
  }
  DBUG_RETURN(0);                     /* ach 30/10/92 */
} /**** end of function dealloc_mvt ****/

/*------------------------------------------------------------------------
 * dealloc_ty -- deallocate a complex type descriptor
 * calls:     freeexpr, FREEDESC, FREEHEAP, DEC_REFCNT.
 *------------------------------------------------------------------------
 */
int dealloc_ty(desc)
PTR_DESCRIPTOR desc;
  {
  register int cnt;
  int x;
  
  DBUG_ENTER("dealloc_ty");
  for (cnt = R_COMPTYPE((*desc),cnt); --cnt >= 0; )
    if T_POINTER((x = ((int *) R_COMPTYPE((*desc),ptypes))[cnt]))
      if (x != (int)NULL)
        DEC_REFCNT((PTR_DESCRIPTOR)x);
      else break;   /* partially filled list */
  FREEHEAP(R_COMPTYPE((*desc),ptypes));
  FREEDESC(desc);
  DBUG_RETURN(0);
  }

/*------------------------------------------------------------------------
 * dealloc_ln -- deallocate a linknode descriptor
 * calls:     freeexpr, FREEDESC, FREEHEAP, DEC_REFCNT.
 *------------------------------------------------------------------------
 */
int dealloc_ln(desc)
PTR_DESCRIPTOR desc;
  {
  DBUG_ENTER("dealloc_ln");
  DEC_REFCNT(R_LINKNODE((*desc), ptype));
  FREEDESC(desc);
  DBUG_RETURN(0);
  }

/*------------------------------------------------------------------------
 * dealloc_ex -- Freigabe eines Expression-Deskriptors
 * ruft auf:     freeexpr, FREEDESC, FREEHEAP, DEC_REFCNT.
 *------------------------------------------------------------------------
 */
int dealloc_ex(desc)
PTR_DESCRIPTOR desc;
{
  register int dim;                                                    /* CS */
  register int i;
  int x;

DBUG_ENTER ("dealloc_ex");

  ISDESC(desc);
  switch ( R_DESC((*desc),type) )
  {
    case TY_MATCH:

#if        DEBUG
        if (debcnt)  {
                printf(
                "dealloc_ex match-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));

        }
#endif   /*     DEBUG  */

         freeexpr(R_MATCH((*desc),guard));

         DBUG_PRINT("heap", ("guard released !"));

         freeexpr(R_MATCH((*desc),body));

         DBUG_PRINT("heap", ("body released !"));

#if (D_SLAVE && D_MESS && D_MHEAP)
         (*d_m_freeheap)((T_HBLOCK *)R_MATCH((*desc),code));   /* cast eingefuegt RS 3/12/1992 */
#else
         freeheap((T_HBLOCK *)R_MATCH((*desc),code));   /* cast eingefuegt RS 3/12/1992 */
#endif
         DBUG_PRINT("heap", ("code released !"));

#if        DEBUG
        if (debcnt) {
        printf("refcnt fuer %8.8x decr. \n",(unsigned int)(PTR_DESCRIPTOR) * (*(A_MATCH((*desc), code))+1));
        printf("T_POINTE %8.8u . \n",T_POINTER(* (*(A_MATCH((*desc), code))+1)));
        }
#endif   /*     DEBUG  */

   if (   ( R_MATCH((*desc), code) != 0  )  
       && (T_POINTER ( * (*(A_MATCH((*desc), code))+1)))
      )
      DEC_REFCNT( (PTR_DESCRIPTOR) * (*(A_MATCH((*desc), code))+1));

         DBUG_PRINT("heap", ("MATCH released !"));

         break;
    case TY_SWITCH:
#if        DEBUG
        if (debcnt) {
                printf(
                "dealloc_ex switch-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
                printf("------------Freigabe des Switch vektor--------------------------- \n");
                }
#endif   /*     DEBUG  */
         DBUG_PRINT ("heap", ("releasing SWITCH..."));
         freeexpr(R_SWITCH((*desc),ptse));

#if !PI_RED_PLUS
        if ( (R_SWITCH((*desc), case_type) != 0)
            && (R_SWITCH((*desc), case_type) != 1)  )
        {/* es liegt ein caserec vor */

         PTR_DESCRIPTOR nam_desc ;
             /*Achtung: jetzt desc mit erstem Matchdescriptor              */
             /*belegt.Daraus den rekursiven Parameter des caserec sammelen */
             /*und dekrementieren                                          */
             nam_desc = (PTR_DESCRIPTOR) ((R_SWITCH ((*(PTR_DESCRIPTOR)desc), ptse))[1]);
             DEC_REFCNT((PTR_DESCRIPTOR) ((R_MATCH ((*nam_desc), code))[4])) ;
        }
#endif /* PI_RED_PLUS      auskommentiert RS 3.12.1992 */

#if        DEBUG
        if (debcnt)
          printf("------------Freigabe des switchvektor beendet-------------------- \n");
#endif   /*     DEBUG  */
         DBUG_PRINT("heap", ("SWITCH released !"));

         break;

    case TY_NOMAT:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex nomat- desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /*      DEBUG     */

  if (0 != ((PTR_HEAPELEM) R_NOMAT((*desc),guard_body)))
{
  T_DEC_REFCNT((PTR_DESCRIPTOR)(R_NOMAT((*desc),guard_body)[1]));
  T_DEC_REFCNT((PTR_DESCRIPTOR)(R_NOMAT((*desc),guard_body)[2]));
}
  if( R_NOMAT((*desc), act_nomat) != 0)
  { /*waehrend des Processing angelegter Nomatchdescr.*/

     /* switchdescr auf jeden Fall descr.*/
     DEC_REFCNT((PTR_DESCRIPTOR)R_NOMAT((*desc),ptsdes));
  }
    /* fuer waehrend des Preprocessing angelegte Nomatchdeskriptoren */
    /* wird der Redcnt auf den switchdeskriptor nicht runtergezaehlt */
         break;


    case TY_EXPR:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex expr-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int) desc) && !debcnt)
                printf(
                "dealloc_ex expr-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif   /*     DEBUG   */
         freeexpr(R_EXPR((*desc),pte));
         break;
    case TY_REC:
    case TY_ZF:
    case TY_SUB:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex func-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int) desc) && !debcnt)
                printf(
                "dealloc_ex func-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif    /*    DEBUG  */
         freeexpr(R_FUNC((*desc),pte));
         if (R_FUNC((*desc),namelist)) {
#if WITHTILDE
	   for (i = ((int *) R_FUNC((*desc),namelist))[0]; i>1; i--)
#else
           for (i = ((int *) R_FUNC((*desc),namelist))[0]; i>0; i--)
#endif /* WITHTILDE */
           {
             x = ((int *) R_FUNC((*desc),namelist))[i];
             DEC_REFCNT((PTR_DESCRIPTOR)x);
           }
           FREEHEAP(R_FUNC((*desc),namelist));
         }
         break;
    case TY_LREC:                                                      /* CS */
#if        DEBUG                                                       /* CS */
        if (debcnt)                                                    /* CS */
                printf(                                                /* CS */
                "dealloc_ex lrec-desc %-8.8x ref: %d \n",              /* CS */
                (unsigned int)desc,R_DESC((*desc),ref_count));                               /* CS */
#endif    /*    DEBUG  */                                              /* CS */
                                                                       /* CS */
         if ( (R_LREC((*desc),ptdv)) != NULL )                         /* CS */
         {                                                             /* CS */
           dim = R_LREC((*desc),nfuncs);                               /* CS */
           for ( ; --dim >= 0; )                                       /* CS */
             if T_POINTER((x = ((int *) R_LREC((*desc),ptdv))[dim]))   /* CS */
               if (x != (int)NULL)                                          /* CS */
                 DEC_REFCNT((PTR_DESCRIPTOR)x);                        /* CS */
               /* else break; */  /* partially filled func defs */     /* CS */
                                  /* nicht alle Funktionsdefinitionen   * CS *
                                     sollen rekonstruiert werden        * CS */
           FREEHEAP(R_LREC((*desc),ptdv));                             /* CS */
         }                                                             /* CS */
                                                                       /* CS */
         for (i = ((int *) R_LREC((*desc),namelist))[0]; i>0; i--)     /* CS */
         {                                                             /* CS */
           x = ((int *) R_LREC((*desc),namelist))[i];                  /* CS */
           DEC_REFCNT((PTR_DESCRIPTOR)x);                              /* CS */
         }                                                             /* CS */
         FREEHEAP(R_LREC((*desc),namelist));                           /* CS */
         break;                                                        /* CS */
    case TY_LREC_IND:                                                  /* CS */
#if        DEBUG                                                       /* CS */
        if (debcnt)                                                    /* CS */
                printf(                                                /* CS */
                "dealloc_ex lrec-ind-desc %-8.8x ref: %d \n",          /* CS */
                (unsigned int)desc,R_DESC((*desc),ref_count));                               /* CS */
#endif    /*    DEBUG  */                                              /* CS */
                                                                       /* CS */
         DEC_REFCNT((PTR_DESCRIPTOR)R_LREC_IND((*desc),ptdd));         /* CS */
         if (T_POINTER((int)R_LREC_IND((*desc),ptf)))                  /* cs */
            DEC_REFCNT((PTR_DESCRIPTOR)R_LREC_IND((*desc),ptf));       /* cs */
         break;                                                        /* CS */
    case TY_LREC_ARGS:                                                 /* CS */
#if        DEBUG                                                       /* CS */
        if (debcnt)                                                    /* CS */
                printf(                                                /* CS */
                "dealloc_ex lrec-args-desc %-8.8x ref: %d \n",         /* CS */
                (unsigned int)desc,R_DESC((*desc),ref_count));                               /* CS */
#endif    /*    DEBUG  */                                              /* CS */
                                                                       /* CS */
         if (R_LREC_ARGS((*desc),ptdv) != NULL)                        /* CS */
         {                                                             /* CS */
           dim = R_LREC_ARGS((*desc),nargs);                           /* CS */
           for ( ; --dim >= 0; )                                       /* CS */
             if T_POINTER((x =                                         /* CS */
                           ((int *) R_LREC_ARGS((*desc),ptdv))[dim]))  /* CS */
               if (x != (int)NULL)                                          /* CS */
                 DEC_REFCNT((PTR_DESCRIPTOR)x);                        /* CS */
               else break;   /* partially filled arg list */           /* CS */
                                                                       /* CS */
           FREEHEAP(R_LREC_ARGS((*desc),ptdv));                        /* CS */
         }                                                             /* CS */
                                                                       /* CS */
         DEC_REFCNT((PTR_DESCRIPTOR)R_LREC_ARGS((*desc),ptdd));        /* CS */
         break;                                                        /* CS */
    case TY_COND:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex cond-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int) desc) && !debcnt)
                printf(
                "dealloc_ex cond-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif   /*     DEBUG  */
         freeexpr(R_COND((*desc),ptte));
         freeexpr(R_COND((*desc),ptee));
         break;
    case TY_NAME:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex name-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int) desc) && !debcnt)
                printf(
                "dealloc_ex name-desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif   /*     DEBUG   */
         FREEHEAP(R_NAME((*desc),ptn));
         break;
    default:
         post_mortem("dealloc: illegal type in descriptor");
  }
  FREEDESC(desc);

  DBUG_RETURN(0);                             /* ach 30/10/92 */
}
/*------------------------------------------------------------------------
 * dealloc_co -- Freigabe eines Variablen-Deskriptors
 *------------------------------------------------------------------------
 */
static int dealloc_co(desc)
PTR_DESCRIPTOR desc;
{
DBUG_ENTER ("dealloc_co");

  ISDESC(desc);
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_co desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int) desc) && !debcnt)
                printf(
                "dealloc_co desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /*      DEBUG     */
  DEC_REFCNT((PTR_DESCRIPTOR)R_VAR((*desc),ptnd));
  FREEDESC(desc);
  DBUG_RETURN(0);                           /* ach 30/10/92 */
}

/* dg */ /*------------------------------------------------------------*/
/* dg */ /* dealloc_fu -- Freigabe eines COMB/CONDI/CLOS-Deskriptors   */
/* dg */ /*------------------------------------------------------------*/
/* dg */
/* dg */ static int dealloc_fu(desc)
/* dg */ PTR_DESCRIPTOR desc;
/* dg */ {
/* dg */   PTR_HEAPELEM   p;
/* dg */   int          dim;

DBUG_ENTER ("dealloc_fu");

/* dg */
/* dg */   ISDESC(desc);
#if        DEBUG
/* dg */         if (debcnt)
/* dg */                 printf(
/* dg */                 "dealloc_fu desc %-8.8x ref: %d \n",
/* dg */                 (unsigned int)desc,R_DESC((*desc),ref_count));
/* dg */         if (debdesc == (int)desc && !debcnt)
/* dg */                 printf(
/* dg */                 "dealloc_fu desc %-8.8x ref: %d \n",
/* dg */                 (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /* DEBUG */
/* dg */
/* dg */   switch(R_DESC((*desc),type)) {
/* dg */     case TY_COMB:  /* DEC_REFCNT(R_COMB((*desc),ptd)); */
/* dg */                    /* codevector steht noch aus !!! */
/* dg */                    break;
/* dg */     case TY_CONDI: /* codevector fuer den then-teil */
/* dg */                    /* codevector fuer den else-teil */
/* dg */                    break;
/* dg */     case TY_CLOS:

                            DBUG_PRINT ("dealloc_fu", ("yo ich geb was frei !"));

/* dg */                    dim = R_CLOS((*desc),args);
/* dg */                    p   = (PTR_HEAPELEM)R_CLOS((*desc),pta);
/* dg */                    for ( ; dim-- >= 0 ; p++) {
DBUG_PRINT ("dealloc_fu", ("yo ich will was freigeben ! %d, %d", p, dim));
                              if (T_POINTER((int)*p)) 
/* dg */                    /*  if (T_POINTER((int)*p && *p)) */ {
DBUG_PRINT ("dealloc_fu", ("yo das geb ich frei !"));
/* dg */                        DEC_REFCNT((T_PTD)*p);
/* dg */                      }
/* dg */                    }
/* dg */                    FREEHEAP(R_CLOS((*desc),pta));
/* dg */                    break;
/* dg */   }
/* dg */   FREEDESC(desc);
           DBUG_RETURN(0);                   /* ach 30/10/92 */
/* dg */ }
/* dg */
/* dg */ /*------------------------------------------------------------*/
/* dg */ /* dealloc_cns -- Freigabe eines CONS-Deskriptors             */
/* dg */ /*------------------------------------------------------------*/
/* dg */
/* dg */ static int dealloc_cns(desc)
/* dg */ PTR_DESCRIPTOR desc;
/* dg */ {
/* dg */   register T_PTD   p;

DBUG_ENTER ("dealloc_cns");

/* dg */
/* dg */   ISDESC(desc);
#if        DEBUG
/* dg */         if (debcnt)
/* dg */                 printf(
/* dg */                 "dealloc_cns desc %-8.8x ref: %d \n",
/* dg */                 (unsigned int)desc,R_DESC((*desc),ref_count));
/* dg */         if (debdesc == (int)desc && !debcnt)
/* dg */                 printf(
/* dg */                 "dealloc_cns desc %-8.8x ref: %d \n",
/* dg */                 (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /* DEBUG */
/* dg */   p = R_CONS((*desc),hd);
/* dg */   if (T_POINTER(p)) DEC_REFCNT(p);
/* dg */   p = R_CONS((*desc),tl);
/* dg */   if (T_POINTER(p)) DEC_REFCNT(p);
/* dg */   FREEDESC(desc);
           DBUG_RETURN(0);                          /* ach 30/10/92 */
/* dg */ }

/* uh */ /*------------------------------------------------------------*/
/* uh */ /* dealloc_mat -- Freigabe eines MATCHING-Deskriptors         */
/* uh */ /*------------------------------------------------------------*/
/* uh */
/* uh */ static int dealloc_mat(desc)
/* uh */ PTR_DESCRIPTOR desc;
/* uh */ {
/* uh */   T_PTD p;
DBUG_ENTER ("dealloc_mat");

/* uh */
/* uh */   ISDESC(desc);
/* uh */   switch(R_DESC((*desc),type)) {
/* uh */     case TY_CLAUSE: p = R_CLAUSE(*desc,next);
/* uh */                     if (p && (T_POINTER(p))) DEC_REFCNT(p);
/* uh */                     FREEHEAP(R_CLAUSE((*desc),sons));
/* uh */                     break;
/* uh */     case TY_SELECTION:  p = R_SELECTION(*desc,clauses);
/* uh */                     if (p && (T_POINTER(p))) DEC_REFCNT(p);
/* uh */                     break;
/* uh */     default: post_mortem("dealloc_mat: unknown TYpe!");
/* uh */   }
/* uh */   FREEDESC(desc);
           DBUG_RETURN(0);                         /* ach 30/10/92 */
/* uh */  }

/* uh */ /*------------------------------------------------------------*/
/* uh */ /* dealloc_pat -- Freigabe eines PATTERN-Deskriptors          */
/* uh */ /*------------------------------------------------------------*/
/* uh */
/* uh */ static int dealloc_pat(desc)
/* uh */ PTR_DESCRIPTOR desc;
/* uh */ {
/* uh */   T_PTD p;
DBUG_ENTER ("dealloc_pat");

/* uh */
/* uh */   ISDESC(desc);
/* uh */   p = R_PATTERN((*desc),pattern);
/* uh */   if (p && T_POINTER(p)) DEC_REFCNT(p);
/* uh */   p = R_PATTERN((*desc),variable);
/* uh */   if (p && T_POINTER(p)) DEC_REFCNT(p);
/* uh */   FREEDESC(desc);
           DBUG_RETURN(0);                       /* ach 30/10/92 */
/* uh */  }

/* cr 09/03/95, kir(ff), START */
/*------------------------------------------------------------------------
 * dealloc_frame -- Freigabe eines Frame-Deskriptors.
 *               Slot-Listen werden von hinten nach vorne aufgefuellt.
 *               Eine Null als Listenelement zeigt eine nur partiell
 *               aufgebaute Liste an.
 * ruft auf:     FREEDESC, FREEHEAP, DEC_REFCNT.
 *------------------------------------------------------------------------
 */
int dealloc_frame(desc)
PTR_DESCRIPTOR desc;
{
  register int dim;
  int x;

DBUG_ENTER("dealloc_frame");

DBUG_ASSERT(desc != _nilstruct);

ISDESC(desc);

#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_frame desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int)desc) && !debcnt)
                printf(
                "dealloc_frame desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /*  DEBUG  */

  if (R_DESC(*desc,type)==TY_SLOT)
  {
    if T_POINTER((x = ((int) R_SLOT((*desc),name))))
        if (x != (int)NULL)
          DEC_REFCNT((PTR_DESCRIPTOR)x);
    if T_POINTER((x = ((int) R_SLOT((*desc),value))))
        if (x != (int)NULL)
          DEC_REFCNT((PTR_DESCRIPTOR)x);
  } 
  else
  {
   /* handles remote descriptors
      because remote adress is stored in slots ?
      copied from dealloc_li, although there are 
      no remote features for frames :-( */
   if (R_FRAME((*desc),slots) == NULL)
   {
     FREEDESC(desc);
     DBUG_RETURN(0);                     /* ach 30/10/92 */   
   }

    dim = R_FRAME((*desc),dim);
    for ( ; --dim >= 0; )
      if T_POINTER((x = ((int *) R_FRAME((*desc),slots))[dim]))
        if (x != (int)NULL)
          DEC_REFCNT((PTR_DESCRIPTOR)x);
        else break;   /* partially filled list */
    FREEHEAP(R_FRAME((*desc),slots));
 }

  FREEDESC(desc);
 
 DBUG_RETURN(0);                    /* ach 30/10/92 */
} /**** end of function dealloc_frame ****/

/*------------------------------------------------------------------------
 * dealloc_inter -- Freigabe eines Interaction-Deskriptors.
 *               Argument-Listen werden von hinten nach vorne aufgefuellt.
 *               Eine Null als Listenelement zeigt eine nur partiell
 *               aufgebaute Liste an.
 * ruft auf:     FREEDESC, FREEHEAP, DEC_REFCNT.
 *------------------------------------------------------------------------
 */
int dealloc_inter(desc)
PTR_DESCRIPTOR desc;
{
  register int dim;
  int x;

DBUG_ENTER("dealloc_inter");

DBUG_ASSERT(desc != _nil);

ISDESC(desc);

#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_inter desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
        if ((debdesc == (int)desc) && !debcnt)
                printf(
                "dealloc_inter desc %-8.8x ref: %d \n",
                (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /*  DEBUG  */

 /* handles remote descriptors
    because remote adress is stores]d in args 
    copied from dealloc_li, although there are 
    no remote features for interactions :-( */
 if (R_INTER((*desc),args) == NULL)
 {
   FREEDESC(desc);
   DBUG_RETURN(0);                     /* ach 30/10/92 */   
 }

  dim = R_INTER((*desc),dim);
  for ( ; --dim >= 0; )
    if T_POINTER((x = ((int *) R_INTER((*desc),args))[dim]))
      if (x != (int)NULL)
        DEC_REFCNT((PTR_DESCRIPTOR)x);
      else break;   /* partially filled list */
  FREEHEAP(R_INTER((*desc),args));
  FREEDESC(desc);
 
 DBUG_RETURN(0);                    /* ach 30/10/92 */
} /**** end of function dealloc_inter ****/
/* cr 09/03/95, kir(ff), END */

/*------------------------------------------------------------------------
 * dealloc_intact -- Freigabe eines Interaktions-Deskriptors.
 *                Argument-Listen werden von hinten nach vorne aufgefuellt.
 *                Eine Null als Listenelement zeigt eine nur partiell
 *                aufgebaute Liste an.
 *                stt  11.09.95
 * ruft auf:      FREEDESC, FREEHEAP, DEC_REFCNT.
 *------------------------------------------------------------------------
 */
int dealloc_intact(desc)
PTR_DESCRIPTOR desc;
{
  register int dim;
  int x;

DBUG_ENTER("dealloc_intact");

DBUG_ASSERT(desc != _nil);

ISDESC(desc);

#if DEBUG
  if (debcnt)
    printf("dealloc_intact desc %-8.8x ref: %d \n",
           (unsigned int)desc,R_DESC((*desc),ref_count));
  if ((debdesc == (int)desc) && !debcnt)
    printf("dealloc_intact desc %-8.8x ref: %d \n",
           (unsigned int)desc,R_DESC((*desc),ref_count));
#endif  /*  DEBUG  */

 /* handles remote descriptors
    because remote adress is stored in args
    copied from dealloc_li, although there are
    no remote features for interactions :-(
  */
  if (R_INTACT((*desc),args) == NULL)
  {
    FREEDESC(desc);
    DBUG_RETURN(0);                     /* ach 30/10/92 */
  }

   dim = R_INTACT((*desc),dim);
   while (--dim >= 0)
     if T_POINTER((x = ((int *) R_INTACT((*desc),args))[dim]))
       if (x != (int)NULL)
         DEC_REFCNT((PTR_DESCRIPTOR)x);
       else break;   /* partially filled list */
   FREEHEAP(R_INTACT((*desc),args));
   FREEDESC(desc);

  DBUG_RETURN(0);                    /* ach 30/10/92 */
} /**** end of function dealloc_intact ****/


/*------------------------------------------------------------------------
 * freeexpr  --  free an expression and contained identifiers
 *------------------------------------------------------------------------
 */
static void freeexpr(p)               /* ach 30/10/92: void */
        register PTR_HEAPELEM        p;
{
        register PTR_HEAPELEM        q;
        register int        i;

DBUG_ENTER ("freeexpr");

#if        DEBUG
        if (debcnt)
                printf(
                "freeexpr heapelement: %-8.8x\n",(unsigned int)p);
#endif  /*      DEBUG     */
        if (p == NULL) DBUG_VOID_RETURN;

        ISHEAP( ((T_HBLOCK *)p) - 1);
        q = p;
        i = *q - TET;
        DBUG_PRINT("heap",("releasing an expression and contained identifiers, i: %i", i));
        for ( q++ ; --i>=0 ; q++ )
                if T_POINTER((*q))
                        DEC_REFCNT((PTR_DESCRIPTOR)(*q));
        DBUG_PRINT("heap",("released..."));
        FREEHEAP(p);
DBUG_VOID_RETURN;
}

/* isdesc aus Debug-Block genommen --- ach 14/05/93 */

/*------------------------------------------------------------------------
 * isdesc  --  is argument a pointer to a descriptor ?
 *------------------------------------------------------------------------
 */
int isdesc(p)
        register PTR_DESCRIPTOR        p;
{
DBUG_ENTER ("isdesc");

        if ( !((StHeapD <= p) && (EnHeapD >= p)) )
                DBUG_RETURN(0);
        if ( ((int)p-(int)StHeapD)%DESLEN != 0 )
                DBUG_RETURN(0);
        DBUG_RETURN(1);
}

/*------------------------------------------------------------------------
 * hpchain  --  test heap chain
 *------------------------------------------------------------------------
 */
void /* TB, 4.11.1992 */
hpchain()
{
        register T_HBLOCK        *p;

DBUG_ENTER ("hpchain");

        for(p=hpbase ; p->next > p ; p=p->next) {
               ASSERT(((p>=hpbase) && (p<=hpend)),"heap ptr out of heap chain!");
        }
        ASSERT(testbusy(hpend),"heap chain corrupted");
        ASSERT(hpend->next == hpbase,"heap chain corrupted");
        ASSERT(((p->next >= hpbase)&&(p->next <= hpend)),
                       "heap chain corrupted,ptr points out of chain");

	DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

void /* TB, 4.11.1992 */
hpchain2()
{
        register T_HBLOCK        *p;

DBUG_ENTER ("hpchain2");

        for(p=hpbase ; p->next > p ; p=p->next) {
               ASSERT(((p>=hpbase) && (p<=hpend)),"before message:heap ptr out of heap chain!");
        }
        ASSERT(testbusy(hpend),"vor message:heap chain corrupted");
        ASSERT(hpend->next == hpbase,"vor message:heap chain corrupted");
        ASSERT(((p->next >= hpbase)&&(p->next <= hpend)),
                       "vor message:heap chain corrupted,ptr points out of chain");

	DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

void /* TB, 4.11.1992 */
hpchain_inter()
{
        register T_HBLOCK        *p;

DBUG_ENTER ("hpchain_inter");

        for(p=hpbase ; p->next > p ; p=p->next) {
	  if (!((p>=hpbase) && (p<=hpend)))
	    fprintf(stderr,"Heap pointer out of heap chain!\n");
        }
        if (!(testbusy(hpend)))
	  fprintf(stderr,"Heap chain corrupted!\n");
        if (!(hpend->next == hpbase))
	  fprintf(stderr,"Heap chain corrupted!\n");
        if (!((p->next >= hpbase)&&(p->next <= hpend)))
	  fprintf(stderr,"Heap chain corrupted!\n");

	DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

/*------------------------------------------------------------------------
 * deschain  --  test descriptor chain
 *------------------------------------------------------------------------
 */

void /* TB, 4.11.1992 */
deschain()
{
        register T_DESCRIPTOR        *stp;
        register int                i=0;

DBUG_ENTER ("deschain");

        if (deslist==NULL) DBUG_VOID_RETURN;

        for (stp=deslist;stp;stp=HEAP_CHAIN(stp)) {
                ASSERT (StHeapD<=stp && stp<=EnHeapD,
                        "descriptor chain corrupted");
                i++;
	      }

	DBUG_VOID_RETURN;
}

/*------------------------------------------------------------------------
 * isheap  --  test if p points to a heap element
 *------------------------------------------------------------------------
 */
int        isheap(p)
        register T_HBLOCK        *p;
{
        register T_HBLOCK        *q;
DBUG_ENTER ("isheap");

        for(q=hpbase ; q->next > q ; q=q->next)
                if (p == q)
                        DBUG_RETURN(1);
        DBUG_RETURN(0);
}
/*------------------------------------------------------------------------
 * res_heap  --  reserve heap to prevent heap compaction
 *------------------------------------------------------------------------
 */

void /* TB, 4.11.1992 */
res_heap()
{
DBUG_ENTER ("res_heap");
#if        DEBUG
        if (heap_res != 0)
                printf("res_heap: Warning: heap is already reserved!\n");
#endif
        heap_res++;

	DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

/*------------------------------------------------------------------------
 * rel_heap  --  release heap to allow heap compaction
 *------------------------------------------------------------------------
 */

void /* TB, 4.11.1992 */
rel_heap()
{
DBUG_ENTER ("rel_heap");

#if        DEBUG
        if (heap_res == 0)
                printf("rel_heap: Warning: heap is not reserved!\n");
#endif

        heap_res--;

	DBUG_VOID_RETURN;
}


/***************************************************************************/
/*                                                                         */
/*     Module NUR fuer die Debug Version!!!!!!!!!!!!!!!!!                  */
/*                                                                         */
/***************************************************************************/

#if DEBUG      /* Das endif steht am Ende der Datei !!!! */

/*----------------------------------------------------------------------
 * inc_refcnt  --  increment refcount of descriptor
 *----------------------------------------------------------------------
 */

void /* TB, 4.11.1992 */ 
inc_refcnt(p)
        register PTR_DESCRIPTOR        p ;
{
DBUG_ENTER ("inc_refcnt");

#if MEASURE
  measure(MS_REFCNT);
#endif

        ISDESC(p);
        if (R_DESC((*p),ref_count) < 65000)
         ((*p).u.sc.ref_count)++;
        else
         post_mortem("inc_refcnt: reference - count overflow");

  DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

/*------------------------------------------------------------------------*/
/* dec_refcnt  --  decrement refcount of descriptor                       */
/* calls:          dealloc.                                               */
/*------------------------------------------------------------------------*/

void /* TB, 4.11.1992 */
dec_refcnt(p)
register PTR_DESCRIPTOR p;
{
DBUG_ENTER ("dec_refcnt");

#if MEASURE
  measure(MS_REFCNT);
#endif

        ISDESC(p);
        if (R_DESC((*p),ref_count) == 0 )
         post_mortem("dec_refcnt: descriptor is freed (reference-count is Null)\n");
        else {
          --((*p).u.sc.ref_count);
          if (R_DESC((*p),ref_count)== 0) {
             PRDEBUG("dec_refcnt : descriptor is to be freed\n");
             dealloc(p);
          }
       }

  DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

/*------------------------------------------------------------------------
 * test_inc_refcnt  --  increment refcount of descriptor
 *------------------------------------------------------------------------
 */

void /* TB, 4.11.1992 */
test_inc_refcnt(ident,fileid,lineid)
        PTR_DESCRIPTOR        ident ;
        char * fileid;
        int lineid ;
{
DBUG_ENTER ("test_inc_refcnt");
DBUG_PRINT("INCREF", ("%08x(%d->%d)", (int)ident, (ident)->u.sc.ref_count, (ident)->u.sc.ref_count+1));

        inc_refcnt(ident) ;
#if        DEBUG
        if (debcnt)
                printf(
                        "test_inc_refcnt: %-8.8x ref: %d  in  %s:%d\n",
                (unsigned int)ident,R_DESC((*ident),ref_count),fileid,lineid);
        if ((debdesc == (int) ident) && !debcnt)
                printf(
                        "test_inc_refcnt: %-8.8x ref: %d  in  %s:%d\n",
                (unsigned int)ident,R_DESC((*ident),ref_count),fileid,lineid);
#endif

	DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}
/*------------------------------------------------------------------------
 * test_dec_refcnt  --  decrement refcount of descriptor
 *------------------------------------------------------------------------
 */
void /* TB, 4.11.1992 */
test_dec_refcnt(ident,fileid,lineid)
        PTR_DESCRIPTOR        ident ;

        char * fileid;
        int lineid ;
{
DBUG_ENTER ("test_dec_refcnt");
DBUG_PRINT("DECREF", ("%08x(%d->%d)", (int)ident, (ident)->u.sc.ref_count, (ident)->u.sc.ref_count-1));
if ((ident)->u.sc.ref_count == 1)
  DBUG_PRINT("DEALLOC", ("dealloc %08x", (int)ident));
#if        DEBUG
        if (debcnt)
                printf(
                "test_dec_refcnt: vor dec %-8.8x ref: %d in  %s:%d\n",
                (unsigned int)ident,R_DESC((*ident),ref_count),fileid,lineid);

        if ((debdesc == (int) ident) && !debcnt)
                printf(
                "test_dec_refcnt: vor dec %-8.8x ref: %d in  %s:%d\n",
                (unsigned int)ident,R_DESC((*ident),ref_count),fileid,lineid);

        DESCHAIN;
        if (inchain(ident))
        {
          printf(
          "test_dec_refcnt: desc %-8.8x ist schon freigegeben worden!\n",
          (unsigned int)ident);
          post_mortem("test_dec_refcnt: desc was already freed");
        }

        if (R_DESC((*ident),ref_count) <= 2)
          if (ident == _dig0)
            post_mortem("test_dec_refcnt: refcnt for 0 gets too low");
          else if (ident == _dig1)
            post_mortem("test_dec_refcnt: refcnt for 1 gets too low");

        if (R_DESC((*ident),ref_count) <= 1) {
          if (ident == _nilmat)
            post_mortem("test_dec_refcnt: nilmat may not be freed");
          if (ident == _nilvect)
            post_mortem("test_dec_refcnt: nilvect may not be freed");
          if (ident == _niltvect)
            post_mortem("test_dec_refcnt: niltvect may not be freed");
          if (ident == _nil)
            post_mortem("test_dec_refcnt: nil may not be freed");
          if (ident == _nilstring)
            post_mortem("test_dec_refcnt: nilstring may not be freed");
          if (ident == _nilstruct)
            post_mortem("test_dec_refcnt: nilstruct may not be freed");
          if (ident == _work)
            post_mortem("test_dec_refcnt: work descriptor may not be freed");
        } /** end if ident ref_count <=1 **/

#endif /*  DEBUG  */
        dec_refcnt(ident) ;
#if        DEBUG
        if (debcnt)
                printf(
                        "test_dec_refcnt: %-8.8x ref: %d  in  %s:%d\n",
                        (unsigned int)ident,R_DESC((*ident),ref_count),fileid,lineid);
        if ((debdesc == (int) ident) && !debcnt)
                printf(
                        "test_dec_refcnt: %-8.8x ref: %d  in  %s:%d\n",
                        (unsigned int)ident,R_DESC((*ident),ref_count),fileid,lineid);
#endif  /*  DEBUG */

	DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

/*------------------------------------------------------------------------
 * dealloc  --  deallocate descriptor/heap structures on zero refcount
 *------------------------------------------------------------------------
 */

void /* TB, 4.11.1992 */
dealloc(p)
PTR_DESCRIPTOR p;
{
DBUG_ENTER ("dealloc");

  ISDESC(p);

#if DEBUG
  ASSERT((R_DESC((*p),class)>0)&&(R_DESC((*p),class)<DEALLOC_TAB_SIZE),"dealloc: Illegal descriptor class");
  if (debcnt) {
    free ((char *) malloc(1)) ;
    printf("dealloc: desc at %-8.8x \n",(unsigned int)p);
  }
  if (debdesc == (int) p && !debcnt) {
   free ((char *) malloc(1)) ;
   printf("dealloc: desc at %-8.8x \n",(unsigned int)p);
   }
#endif
  DESCHAIN;
  HPCHAIN;
  (* dealloc_tab[R_DESC((*p),class)])(p);
  DESCHAIN;
  HPCHAIN;

#if DEBUG
  if (debcnt) {
    free ((char *) malloc(1)) ;
   }
  if (debdesc == (int) p && !debcnt) {
    free ((char *) malloc(1)) ;
    printf("dealloc: desc at %-8.8x freed  mallocs ok \n",(unsigned int)p);
  }
#endif

  DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

/*        $P        */
/*------------------------------------------------------------------------
 * inchain  --   is p in free desc chain ?
 *------------------------------------------------------------------------
 */
int    inchain(p)
register T_DESCRIPTOR        *p;
{
        register T_DESCRIPTOR        *stp;
DBUG_ENTER ("inchain");

        for (stp=deslist ; stp ; stp=HEAP_CHAIN(stp)) {
                if (p == stp)
                        DBUG_RETURN(1);
        }

        DBUG_RETURN(0);
}


/*------------------------------------------------------------------------
 * OutDescArea  --  dump all descriptors
 *------------------------------------------------------------------------
 */
void        OutDescArea(XFile)
        FILE                *XFile   ;
{
        register T_DESCRIPTOR        *p;
        int                free;               /* flag that desc is in freelist*/
DBUG_ENTER ("OutDescArea");

        fprintf(DumpFile,"\n D E S C R I P T O R S\n\n");
        fprintf(DumpFile,"\n Number of used descriptors: %d\n"
                ,descnt);
        fprintf(DumpFile,"\n Number of free descriptors: %d\n"
                ,desmax-descnt);

        for (p=StHeapD ; p <= EnHeapD ; p++) {
                free = inchain(p);

                if ( !free )
                        DescDump (XFile,p,1);
            }
DBUG_VOID_RETURN;
}

/*------------------------------------------------------------------------
 * DumpElem  --  dump one heapblock
 *------------------------------------------------------------------------
 */
int        *DumpElem(XFile,p)
        FILE                *XFile   ;
        register T_HBLOCK        *p;
{
        T_HBLOCK        *q;
        register int        *pi,*pi1;
        register int        j, i, desc_addr;

DBUG_ENTER ("DumpElem");

        fprintf (XFile,"\n");
        q = p->next;                       /* NextHeapElement                */
        fprintf(XFile,"Heap block at: %8.8x, next block at: %8.8x\n",(unsigned int)p,(unsigned int)q);

        if ( (long)(p->ref) == -1 )
                fprintf(XFile,"non-relocatable dynamic buffer\n");
        else {
                desc_addr = ((unsigned)(p->ref) - (unsigned)StHeapD) /
                                DESLEN * DESLEN;
                desc_addr += (unsigned)StHeapD;               /* add base addr*/
                fprintf(XFile,"Heap back reference: %8.8x in desc at %8.8x\n",
                        (unsigned int)p->ref,(unsigned int)desc_addr);
        }

        i = 0;
        for (pi=(int *)(p+1) ; pi < (int *)q ; ) {
                pi1 = pi;
                j=0;
                fprintf(XFile,"%8.8x %4.4d:  ",(unsigned int)pi,i);
                while (pi < (int *)q) {
                        prstel(XFile,*pi,1);
                        pi++;
                        j++;
                        i++;
                        if (j == 8)
                        break;
                }
                fprintf (XFile,"\n");
                fprintf(XFile,"%8.8s %4.4s   ","","");
                j=0;
                while (pi1 < (int *)q) {
                        fprintf (XFile, " %8.8x  ", *pi1);
                        pi1++;
                        j++;
                        if (j == 8)
                        break;
                }
                fprintf (XFile, "%s", "\n");
        }

	DBUG_RETURN((int *)0); /* TB, 4.11.1992 */
}

/*------------------------------------------------------------------------
 * OutHeapArea  --  dump used heap blocks
 *------------------------------------------------------------------------
 */
void        OutHeapArea(XFile)
        FILE                *XFile   ;
{
        T_HBLOCK        *p;

DBUG_ENTER ("OutHeapArea");


        fprintf(DumpFile,"\n H E A P   A R E A \n\n");

        for ( p=hpbase ; p<hpend ; p=p->next)
                if (p->ref != NULL)
                        DumpElem (XFile,p);
                else
                  fprintf(XFile,"Adr.p: %x  Adr.p->next: %x\n",(unsigned int)p,(unsigned int)p->next);

DBUG_VOID_RETURN;
}

/*------------------------------------------------------------------------
 * HeapDump  --  dump all used descriptors and the used heap
 *------------------------------------------------------------------------
 */
void /* TB, 4.11.1992 */
HeapDump(Ident)
char        *Ident ;

{
DBUG_ENTER ("HeapDump");

        fprintf(DumpFile,
                "\n\n*************** Dump Heap : %s ********************\n\n",
                Ident);


        OutDescArea(DumpFile);

        HPCHAIN;
        OutHeapArea(DumpFile);

        fprintf(DumpFile,
                "\n\n*************** Dump Heap : %s ********************\n\n",
                Ident);

        fflush(DumpFile);               /* flush outstanding buffer        */

	DBUG_VOID_RETURN; /* TB, 4.11.1992 */
}

#endif

#endif

#endif /* SCAVENGE */

/* end of file */
