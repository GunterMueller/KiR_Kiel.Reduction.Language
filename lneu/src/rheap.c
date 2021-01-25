/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
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
#include "rstackty.h"
#include "rmeasure.h"
#include <string.h>
#include "list.h"
#include "case.h"




/*--------------------------------------------------------------------------
 * Internal type definition --
 *--------------------------------------------------------------------------
 */
typedef struct T_hblock {
          struct T_hblock *next;  /* ptr to next block        */
          T_HEAPELEM      **ref;  /* blk is ref'ed by ...     */
        }                T_HBLOCK;

/*-------------------------------------------------------------------------
 * externals for heapdump functions :
 *  used only in the debug-version.
 *------------------------------------------------------------------------
         for (i = ((int ) R_FUNC((*desc),nargs) + 1 ); i > 1; i--)
         {
           x = ((int *) R_FUNC((*desc),namelist))[i];
           DEC_REFCNT((PTR_DESCRIPTOR)x);
         }
         FREEHEAP(R_FUNC((*desc),namelist));
 */
#if        DEBUG
extern  FILE        *DumpFile;
extern  prstel();
#else
static        FILE        *DumpFile = stdout ;
static  DescDump () {};
#endif
/* Einen leeren Deskriptor erzeugen: Es reicht, den RefCount auf Null   */
/* zu setzen                                                            */

#define EMPTYDESC(desc) \
  L_DESC( (*desc), ref_count ) = 0;

/*-------------------------------------------------------------------------*/
/* external function declarations --                                       */
/*-------------------------------------------------------------------------*/
#if LARGE
extern PTR_DESCRIPTOR sts_dec(); /* rsts-dec.c                             */
#endif
/*-------------------------------------------------------------------------*/
/* external variable declarations --                                       */
/*-------------------------------------------------------------------------*/
#if LARGE
extern BOOLEAN  inittrigdesc; /* if inittrigdesc() is called once then TRUE*/

extern BOOLEAN  initexpdesc;  /* if initexpdesc()  is called once then TRUE*/
#endif

#if DEBUG                               /* from rumtime                      */
extern int stopheap;                    /* simulate heapoverflow after       */
                                        /* stopheap new-calls                */
extern int debcnt;                      /* debug output ?                    */
extern int debdesc;                     /* debug output for one desc ?       */
#endif


#ifdef HEAPMANAGER
#if LARGE
PTR_DESCRIPTOR  _dig0,_dig1,_work;     /* for digit-arithmetic              */
PTR_DESCRIPTOR  _nilmat;             /* for structuring functions         */
PTR_DESCRIPTOR  _nilvect;            /* for structuring functions         */
PTR_DESCRIPTOR  _niltvect;           /* for structuring functions         */
#endif
PTR_DESCRIPTOR  _nil;                /* "for structuring functions"       */
PTR_DESCRIPTOR  _nilstring;          /* "for structuring functions"       */
#if LARGE
PTR_DESCRIPTOR  _digPI;                /* for trigonometric functions       */
PTR_DESCRIPTOR  _digLN2;               /* to eval. exp functions in         */
                                       /*           rbibfunc.c              */
PTR_DESCRIPTOR  _digRG;                /* used in rbibfunc.c                */
                                       /* relativ error                     */
#endif
#endif

/*--------------------------------------------------------------------------*/
/* declarations --                                                          */
/*--------------------------------------------------------------------------*/
char pic[] = "3.141592653589793238462643383279502884197169399375105820\
97494459230781640628620899862803482534211706798214808651328230";
char piv[sizeof(pic)];                /*            variable PI             */

char lnc[] = "0.693147180559945309417232121458176568075500134360255254\
12068000949339362196969471560586332699641868754200148102057068";
char lnv[sizeof(lnc)];                /*            variable ln(2)          */


/* heapvariables, which are used in runtime.c */
              int                descnt;     /* number of allocated desc's    */
         T_DESCRIPTOR        *StHeapD;       /* initial ptr *global           */
         T_DESCRIPTOR        *EnHeapD;       /* arena top *global             */
         T_DESCRIPTOR        *deslist;       /* free list ptr                 */

static        int             desmax;        /* number of all desc's          */
static        int             odesmax;       /* number of all desc's  *reinit */
static        T_DESCRIPTOR    *oStHeapD;     /* initial ptr *reinit           */
static        T_DESCRIPTOR    *oEnHeapD;     /* arena top *reinit             */
static        T_HBLOCK        *hpbase;       /* initial arena                 */
static        T_HBLOCK        *ohpbase;      /* initial arena *reinit         */
static        T_HBLOCK        *hpend;        /* arena top                     */
static        T_HBLOCK        *ohpend;       /* arena top *reinit             */
static        T_HBLOCK        *hplast;       /* points to last handled block  */
static        int             heap_res;      /* heap reserved, if nonzero     */
static        int             heapsize ;     /* set by InitHeapManagement     */
static        int             heapdes ;      /* set by InitHeapManagement     */

/* forward declarations */
T_DESCRIPTOR * newdesc();
int  dealloc_sc();
int  dealloc_li();
#if LARGE
int  dealloc_di();
int  dealloc_mvt();  /* Matrix / Vector / transposed Vector */
#endif 
int  dealloc_ex();
int  dealloc_co();

/* Tabellensprung bei dealloc */
FCTPTR dealloc_tab[] = {
  0,
  dealloc_sc,
#if LARGE
  dealloc_di,
#else
  0,
#endif
  dealloc_li,
#if LARGE
  dealloc_mvt,
#else
  0,
#endif 
  0,
  dealloc_ex,
  dealloc_co,
#if LARGE
  dealloc_mvt,
  dealloc_mvt
#else
 0,
 0
#endif 
};

#define DEALLOC_TAB_SIZE  (sizeof(dealloc_tab) / sizeof(FCTPTR))


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
#define FREEDESC(desc)        { if (desc != NULL)  {       \
                                HEAP_CHAIN(desc) = deslist; \
                                deslist = desc; descnt--; }}

      /* freeheap als Makro ist schneller. Verwendet durch: */
      /* einige dealloc_ Routinen, sowie freeheap selbst.   */
#if MEASURE
#define FREEHEAP(p) { T_HBLOCK *_h_ = (T_HBLOCK *) p; \
    if (_h_ != NULL) { HPCHAIN; --_h_; \
      mfree((int)(_h_->next - _h_)); ISHEAP(_h_); _h_->ref = NULL; }}
#else
#define FREEHEAP(p) { T_HBLOCK *_h_ = (T_HBLOCK *) p; \
                      if (_h_ != NULL) { \
                        HPCHAIN; --_h_; ISHEAP(_h_); _h_->ref = NULL; }}
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
#define PRDEBUG(p)        if (debcnt) printf(p); else
#else
#define PRDEBUG(p)
#endif

#if        DEBUG
#define ASSERT(expr,text)                                                \
        if (!(expr)) {                                                        \
                printf(                                                \
                "Assertion 'expr' failed: file '%s', line %d\n",        \
                __FILE__,__LINE__ );                                        \
                post_mortem(text);                                \
        } else
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

#if LARGE
  if ((_work = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
  DESC_MASK(_work,2,C_DIGIT,TY_DIGIT);
  L_DIGIT((*_work),Exp) = 0;
  L_DIGIT((*_work),ndigit) = 1;
  L_DIGIT((*_work),sign) = 0;
  if ((newheap(1,A_DIGIT((*_work),ptdv))) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
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


  if ((_nilmat = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
  MVT_DESC_MASK(_nilmat,1,C_MATRIX,TY_UNDEF);
  L_MVT((*_nilmat),nrows,C_MATRIX) = NULL;    /* das dritte Wort wird von    */
  L_MVT((*_nilmat),ncols,C_MATRIX) = NULL;    /* MVT_DESC_MASK mit 0         */
  L_MVT((*_nilmat),ptdv,C_MATRIX)  = NULL;    /* initialisiert               */
  stdesccnt ++;

  if ((_nilvect = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
  MVT_DESC_MASK(_nilvect,1,C_VECTOR,TY_UNDEF);
  L_MVT((*_nilvect),nrows,C_VECTOR) = NULL;   /* das dritte Wort wird von    */
  L_MVT((*_nilvect),ncols,C_VECTOR) = NULL;   /* MVT_DESC_MASK mit 0         */
  L_MVT((*_nilvect),ptdv,C_VECTOR)  = NULL;   /* initialisiert               */
  stdesccnt ++;

  if ((_niltvect = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
  MVT_DESC_MASK(_niltvect,1,C_TVECTOR,TY_UNDEF);
  L_MVT((*_niltvect),nrows,C_TVECTOR) = NULL; /* das dritte Wort wird von    */
  L_MVT((*_niltvect),ncols,C_TVECTOR) = NULL; /* MVT_DESC_MASK mit 0         */
  L_MVT((*_niltvect),ptdv,C_TVECTOR)  = NULL; /* initialisiert               */
  stdesccnt ++;
#endif

  if ((_nil = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
  LI_DESC_MASK(_nil,1,C_LIST,TY_UNDEF);       /* das zweite und dritte Wort  */
  L_LIST((*_nil),ptdv)    = NULL;             /* wird von LI_DESC_MASK mit 0 */
  stdesccnt ++;                               /* initialisiert               */

  if ((_nilstring = newdesc()) == NULL) post_mortem("(Re)InitHeap: Heap out of Space");
  LI_DESC_MASK(_nilstring,1,C_LIST,TY_STRING);/* das zweite und dritte Wort  */
  L_LIST((*_nilstring),ptdv)    = NULL;       /* wird von LI_DESC_MASK mit 0 */
  stdesccnt ++;                               /* initialisiert               */

  return;
} /**** end of procedure crea_global_desc ****/

/*****************************************************************************/
/*                            init_var                                       */
/*---------------------------------------------------------------------------*/
/*  Funktion  : Initialisierung  von globalen Variablen in inithp/ReInitHeap */
/*  called by : inithp , ReInitHeap                                          */
/*****************************************************************************/
static void init_var()
{
  descnt = 0;                           /* initialize descriptor counter     */
  idcnt  = 0;                           /* indirect list-descriptor counter  */

  stdesccnt = NULL;                     /* initialize global couter-variable */
                                        /* for the number of standard        */
                                        /* descriptors                       */

#if LARGE
  inittrigdesc = FALSE;                 /* initialize global variable used   */
                                        /* in function inittrigdesc in       */
                                        /* rbibfunc.c                        */
  initexpdesc  = FALSE;                 /* initialize global variable used   */
                                        /* in function initexpdesc in        */
                                        /* rbibfunc.c                        */
#endif
  return;
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
 static char        *base = NULL;
        extern PTR_DESCRIPTOR _old_expr_p;       /* reduct:c */
        extern PTR_DESCRIPTOR    PRE_LIST;
        extern int                anz_PRE; 


        heapsize = hs;    /* init global variables */
        heapdes  = hd;

        if (base != (char *) NULL) free(base);   /* free old space,if */
                                                 /* there was any.    */
        base = (char *) malloc((int) HEAPSIZE);  /* get base address  */
        if ( (long)base <= 0 )
          post_mortem("Init Heap: no space for malloc");

        if (inithp(base,(int)HEAPSIZE,DESCNUM))
        {
#if DEBUG
                printf(" *** Init Heap   ok ! ***\n");
#endif
        }
        else
          post_mortem("Init Heap: Heap is NOT ok !");

       crea_global_desc();

        /* ODDSEX-flag is tested */
        if ((R_DESC((*_nil),type)!= TY_UNDEF)
           && (R_DESC((*_nil),class)!= C_LIST))
#if    ODDSEX
           post_mortem("Please compile everything again with ODDSEX not set!");
#else
           post_mortem("Please compile everything again with ODDSEX set!");
#endif


        /* verpointerte Version ist nicht mehr da */
        _old_expr_p = NULL;     

        /* Initialisieren von PRE_LIST */
         PRE_LIST = NULL;
         anz_PRE  = 0;
                       
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

        heap_res = 0;                       /* clear reserved flag     */
        ALIGNTST(sizeof(T_DESCRIPTOR),T_HBLOCK); /* struct's ok ?         */
                                                 /* round to mult of 16   */
        base = (char *)(((unsigned)base + 15) & ~15);
        j = size & ~15;                          /* trunc to mult of 16   */

        /* 11 = number of standard descriptors remain permanent in heap   */
        /*     _work, _dig0, _dig1, _digRG, _digPI, _digLN2               */
        /*     _nilmat, _nilvect, _niltvect, _nil, _nilstring             */
        /* without standard decriptors created in rbibfunc.c              */
        if (ndes < 11)
          post_mortem("Init Heap: number of descriptors too small [ < 11 ]");

        i = j - DESLEN * ( ndes  + 1 );          /* i holds heap lenght   */
        if ((i & 15) != 0)
          post_mortem("Init Heap: allignment error");
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
                                        /* rbibfunc.c                         */
        init_var();                              /* init global variables */

 
       /* init descriptor chain */
        p = StHeapD;
        q = p+1;
        for (i=ndes ; --i ; ) {
                HEAP_CHAIN(p) = q++;
				EMPTYDESC( p );
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
        return(1);
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
ReInitHeap()
{
        register T_DESCRIPTOR        *p, *q;
        register int                i;
        extern PTR_DESCRIPTOR PRE_LIST;

#if DEBUG
        extern PTR_DESCRIPTOR _old_expr_p;       /* reduct:c */
        char c;
        FILE * fopen();

#endif

        if ( ohpbase == NULL )
                post_mortem("ReInitHeap: not yet initialized");

#if DEBUG
        if (_old_expr_p != NULL || PRE_LIST != NULL )
        {                                           

          if ( _old_expr_p != NULL)
          {
            DEC_REFCNT(_old_expr_p);      /* free old kept expression */
            _old_expr_p = NULL;
           }
          if ( PRE_LIST != NULL )
          {
             DEC_REFCNT(PRE_LIST);
             PRE_LIST = NULL; 
             anz_PRE = 0 ;
          }

          if ((descnt != stdesccnt) ||
#if LARGE
              (R_DESC((*_dig0),ref_count) != 2) ||
              (R_DESC((*_dig1),ref_count) != 2) ||
              (R_DESC((*_digPI),ref_count) != 2) ||
              (R_DESC((*_digLN2),ref_count) != 2) ||
              (R_DESC((*_digRG),ref_count) != 2) ||
              (R_DESC((*_work),ref_count) != 2) ||
              (R_DESC((*_nilmat),ref_count)   != 1) ||
              (R_DESC((*_nilmat),type)   != TY_UNDEF) ||
#endif
              (R_DESC((*_nil),ref_count) != 1)   ||
              (R_DESC((*_nil),type) != TY_UNDEF) ||
              (R_DESC((*_nilstring),ref_count) != 1)    ||
              (R_DESC((*_nilstring),type) != TY_STRING) 
#if LARGE
               ||
              (R_DESC((*_nilvect),ref_count)  != 1) ||
              (R_DESC((*_nilvect),type)  != TY_UNDEF) ||
              (R_DESC((*_niltvect),ref_count) != 1) ||
              (R_DESC((*_niltvect),type) != TY_UNDEF) ||
              (R_DESC((*_dig0),class) != C_DIGIT) ||
              (R_DESC((*_dig1),class) != C_DIGIT) ||
              (R_DESC((*_digPI),class) != C_DIGIT) ||
              (R_DESC((*_digLN2),class) != C_DIGIT) ||
              (R_DESC((*_digRG),class) != C_DIGIT) ||
              (R_DESC((*_work),class) != C_DIGIT)   
#endif
              )
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
            scanf ("%c",&c);
            printf("\n                                          \n");

            switch(c)
            {
              case 'b':
              case 'B': DumpFile = stderr;
                        HeapDump("heaplisting");
                        printf("\n Bitte ein Zeichen eingeben : ");
                        scanf("%c",&c);
                        break;
              case 'd':
              case 'D': if ((DumpFile = fopen("heap.dump","w")) == NULL)
                          printf(" heap.dump konnte nicht geoffnet werden!\n");
                        else
                        {
                          HeapDump("heaplisting");
                          fclose(DumpFile);
                        }
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
                HEAP_CHAIN(p) = q++;
                p++;
        }
        HEAP_CHAIN(p) = NULL;
        deslist = StHeapD;
        DESCHAIN;

        PRHEAP ;

        /* initialisation of the global descriptors */
        /*-------------------------------------------------------------------*/
        /* Der '_work'-Descriptor kann nicht durch sts_dec angelegt werden,  */
        /* da er selbst (bei BASE <> 10000) in sts_dec benutzt wird.         */
        /* Der work Deskriptor muss als erster angelegt werden, da           */
        /* die Funktion sts_dec den work Deskriptor benoetigt, um weitere    */
        /* Standard Deskriptoren anzulegen.                                  */
        /*-------------------------------------------------------------------*/
#if LARGE
        _work      = NULL;                /* initialize standard descriptors */
        _dig0      = NULL;                /* initialize standard descriptors */
        _dig1      = NULL;                /* initialize standard descriptors */
        _digPI     = NULL;                /* initialize standard descriptors */
        _digRG     = NULL;                /* initialize standard descriptors */
        _digLN2    = NULL;                /* initialize standard descriptors */
        _nilmat    = NULL;                /* initialize standard descriptors */
        _nilvect   = NULL;                /* initialize standard descriptors */
        _niltvect  = NULL;                /* initialize standard descriptors */
#endif
        _nil       = NULL;                /* initialize standard descriptors */
        _nilstring = NULL;                /* initialize standard descriptors */


        crea_global_desc();


#if DEBUG
                printf(" *** Reinit Heap   ok ! ***\n");
#endif
        return(1);
}


/*------------------------------------------------------------------------
 * newdesc  --  return an identifier to a descriptor, allocate more
 *              descriptors if necessary, return a null, if heap is full.
 * calls:       moredes.
 *------------------------------------------------------------------------
 */
T_DESCRIPTOR *newdesc()
{
        register T_DESCRIPTOR *p;

#if MEASURE
  measure(MS_NEWDESC);
#endif

#if        DEBUG
        if (heap_res)
        {
                printf("newdesc: Warning: heap was reserved");
                REL_HEAP;
        }
        if (stopheap == 0) return((T_DESCRIPTOR *) NULL);
        if (stopheap >  0) stopheap--;
#endif

        DESCHAIN;

        if (deslist == NULL)
        {
          moredes();               /* get more descriptors from heap */
          if (deslist == NULL)
          {
              printf("newdesc: heap out of space\n");
              return(NULL);
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

        ASSERT(StHeapD<=p && p<=EnHeapD,"address not desc area");
#if        DEBUG
        HEAP_CHAIN(p) = NULL ;

        if (debcnt)
                printf("newdesc at %8.8x: %d used, %d max\n",
                               p,descnt,desmax);
        if (debdesc == (int)p && !debcnt)
                printf("desc allocated %8.8x: %d used, %d max\n",
                               p,descnt,desmax);
#endif
        DESCHAIN;
        return(p);
}

/*------------------------------------------------------------------------
 * freedesc  --  free a descriptor, just put it into chain
 *------------------------------------------------------------------------
 */
void
freedesc(p)
        register T_DESCRIPTOR *p;
{
        if (p == NULL) return;

        DESCHAIN;

        ISDESC(p);

        FREEDESC(p);

        DESCHAIN;

#if        DEBUG
        if (debcnt)
                printf("freedesc: %d used, %d max\n",descnt,desmax);
        if (debdesc == (int)p && !debcnt)
                printf("freedesc: %d used, %d max\n",descnt,desmax);
#endif
}                                         
/*--------------------------------------------------------------------*/
/*                                                                    */
/* -------------------------------------------------------------------*/

int get_no_of_descs()   

  /* liefert die Zahl der im Moment vorhandenen Deskriptoren zurueck */
  /* fuer ksend.c                                                    */

{
  return( desmax);
}  /* get_no_of_descs */

/*------------------------------------------------------------------------
 * get_HBLOCK  --  get a heap element with size 'n_int*sizeof(int)'
 *                   first fit, compact heap if necessary
 * calls          compheap.
 *------------------------------------------------------------------------
 */
static        T_HBLOCK        *get_HBLOCK(n_int)
        int                n_int;                /* # of int's to alloc   */
{
        register T_HBLOCK        *p, *q, *r;
        register int                nb;          /* # of blocks to alloc  */
        int                        retryflg = 0; /* try 1 garbage collect */

#if MEASURE
#endif

        if (n_int == 0) {
#if    DEBUG
        if (debcnt)
                printf(
                "get_HBLOCK:  No request ! \n");
#endif
                return(NULL);
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
                    return(NULL);

#if        DEBUG
        if (debcnt)
                printf(
                "get_HBLOCK: COMPACT for req %d integers, %d HBLOCKs\n",
                n_int,nb);
#endif
                compheap();               /* compact heap */
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
                ,nb,p);
#endif
        return(p);
}


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
{
        register T_HBLOCK        *p;

#if        DEBUG
        if (heap_res)
        {
                printf("newheap: Warning heap was reserved");
                REL_HEAP;
        }
        if (stopheap == 0)
        {
          *p_ref = (T_HEAPELEM *) NULL;
          return(0);
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
                return(1);
          }
          else
          {
            *p_ref = (T_HEAPELEM *) NULL;
/* message is always printed out on stdout !!!!!!!!!!!!!!!!!  */
            printf("newheap: Heap out of Space\n");
            return(0);
          }
        p->ref = p_ref;
        *p_ref = (T_HEAPELEM *)(p+1);
        return(1)   ;
}

/*------------------------------------------------------------------------
 * newbuff  --  get a heap element with size 'n_int*sizeof(int)'
 *              returns a null when heap is full.
 *------------------------------------------------------------------------
 */
T_HEAPELEM        *newbuff(n_int)
        int                n_int;            /* # of int's to alloc        */
{
        register T_HBLOCK        *p;

#if        DEBUG
        if (heap_res)
                printf("newheap: Warning:heap is reserved");
#endif

        p = get_HBLOCK(n_int);

        if (p == NULL) {
                return(NULL);
        }

        p->ref = (T_HEAPELEM **) -1;       /* mark dummy used        */
        return((T_HEAPELEM *)(p+1));
}

/*------------------------------------------------------------------------
 * freeheap  --  free a heap element
 *------------------------------------------------------------------------
 */
void    freeheap(p)
        T_HBLOCK         *p;
{
        if (p == NULL) return;

        p--;              /* goto the block before */
        HPCHAIN;
        ISHEAP(p);
        ASSERT(testbusy(p),"heap element was already freed");

        p++; FREEHEAP(p); p--;

        ASSERT(p->next > p && p->next <= hpend,"heap chain corrupted");
}

/*------------------------------------------------------------------------
 * cutheap  -- shorten a heap element to i integers, but multiples of
 *             ALIGN
 *------------------------------------------------------------------------
 */
cutheap(p,i)
        register T_HBLOCK         *p;
        register int                i;
{
        register T_HBLOCK         *q;
        int                j,k;

        --p;                                  /* go to block before        */
        ISHEAP(p);
        ASSERT(testbusy(p),"heap element not in use");
        k = (i * sizeof(T_HEAPELEM) + ALIGN - 1) / ALIGN;
        if ( (j=(p->next)-p) <= (k+1) )
                                               /* not enough to make        */
                return;                        /* new block                */

        j -= 1 + k;
        q = p->next - j;                       /* q points to new block  */
        ALIGNTST(q,T_HBLOCK);                  /* test alignment         */
        q->next = p->next;                     /* chain it in            */
        q->ref = NULL;
        p->next = q;
        HPCHAIN;
}
/*------------------------------------------------------------------------
 * compheap  --  compact heap, update ptrs in desc's and return 1, if
 *               there was free space to compact, return 0 otherwise
 *------------------------------------------------------------------------
 */
compheap()
{
        register T_HBLOCK        *hpp, *hpq, *hpr, *hpx, *hpy;
        register T_DESCRIPTOR  *p    ;
        T_HBLOCK                 *hpf;
        int                        free_flg = 0;
        int                        i           ;
        int                    idc = 0; /* help variable (indirect list-descriptor count) */
        PTR_DESCRIPTOR           desc;

        PRDEBUG("heap compaction initiated.\007\007\n");
        printf("heap compaction initiated.\007\007\n");

#if        DEBUG
        if (heap_res)
        {
           printf("res_heap: heap is reserved\n");
           return(0);
        }
#endif
        HPCHAIN;

        hpp = hpbase;
        /* hpf = hpend; */ /* never used */
        for (;;) {
               /* look for first free block */
                for (;(hpp!=hpend)&&testbusy(hpp);hpp=hpp->next)
                        ASSERT(hpbase<=hpp && hpp<=hpend,"heap chain corrupted");
                if (hpp==hpend) break;       /* return, if none free        */

                for (hpq = hpp->next;;) {
                        ASSERT(hpp<=hpq,"heap chain corrupted");
                       /* look for first used block after current free block */
                        for (;(hpq!=hpend)&&!testbusy(hpq);hpq=hpq->next)
                                ASSERT(hpbase<=hpq && hpq<=hpend,"heap chain corrupted");
                        if ((int)(hpq->ref)==-1) {/* break, if none used*/
                                hpp = hpq;
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
        HPCHAIN;
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
       if (R_DESC((*p),ref_count) > NULL) {
         switch(R_DESC((*p),class)) {
           case C_LIST    : if (R_LIST((*p),ptdd) != NULL) {
                              /* add new ptdv of direct list-descriptor  */
                              /* to  offset of indirect-list-descriptor  */
                              L_LIST((*p),ptdv) =
                                     R_LIST((*(R_LIST((*p),ptdd))),ptdv) +
                                     R_LIST((*p),special);
                               idc++;
                             }
                             break;
#if LARGE
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
#endif 
                            break;
         } /** switch class **/
         if (idc == idcnt) break; /* if max count of indirect descriptors reached */
       }
     }
   }
        return(free_flg);
}


/*------------------------------------------------------------------------
 * moredes  --  get more descriptors from end of heap, shorten heap
 * calls:       compheap.
 *------------------------------------------------------------------------
 */
static    moredes()
{
        register T_HBLOCK        *hpp;
        register T_DESCRIPTOR        *p, *q;
        register int                i;
        int        retryflg = 0;

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
                compheap();               /* compact heap                   */
                retryflg++;               /* don't retry, already compacted */
                goto retry ;
        }

                                              /* no. of descr in heap block */
        i = (hpend - hpp)*sizeof(T_HBLOCK)/DESLEN;
        if ( (i < 10) || (hpp->next != hpend) ) {
                if (++retryflg > 1)
                  return(0); /* no space for descriptors available */
                compheap();               /* compact heap */
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
                HEAP_CHAIN(p) = q++;
				EMPTYDESC( p );
                p++;
        }
        HEAP_CHAIN(p) = deslist;
		EMPTYDESC( p );
        deslist = StHeapD;
        DESCHAIN;

#if        DEBUG
           printf(
                   "moredesc: %d used, new maximum is %d \n",descnt,desmax);
#endif

        return(1);
}

/*---------------------------------------------------------------------------*/
/* inc_idcnt  increment indirect descriptor counter                          */
/*            Modul nur fuer Debug-Version  !!!!!                            */
/*---------------------------------------------------------------------------*/
#if DEBUG
void inc_idcnt()
{
   idcnt++;
}

/*---------------------------------------------------------------------------*/
/* dec_idcnt          decrement indirect descriptor counter                  */
/*            Modul nur fuer Debug-Version  !!!!!                            */
/*---------------------------------------------------------------------------*/
void dec_idcnt()
{
   idcnt--;
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
  ISDESC(desc);
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_sc desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_sc desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif  /* DEBUG */
  FREEDESC(desc);
}

#if LARGE
/*****************************************************************************/
/*                                dealloc_di                                 */
/*---------------------------------------------------------------------------*/
/* Funktion :    Freigabe eines Digit-Deskriptors                            */
/* calls    :    FREEDESC, FREEHEAP.                                         */
/*****************************************************************************/
int dealloc_di(desc)
PTR_DESCRIPTOR desc;
{
  ISDESC(desc);
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_di desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == desc && !debcnt)
                printf(
                "dealloc_di desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif  /*  DEBUG */
  FREEHEAP(R_DIGIT((*desc),ptdv));
  FREEDESC(desc);
}
#endif 

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

ISDESC(desc);

#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_li desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == desc && !debcnt)
                printf(
                "dealloc_li desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif  /*  DEBUG  */

 if (R_LIST((*desc),ptdv) == NULL)
 {
   FREEDESC(desc);
   return;
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
      if (x != NULL)
        DEC_REFCNT((PTR_DESCRIPTOR)x);
      else break;   /* partially filled list */
  FREEHEAP(R_LIST((*desc),ptdv));
  FREEDESC(desc);
 }
} /**** end of function dealloc_li ****/

#if LARGE

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

  ISDESC(desc);
  class1 = R_DESC((*desc),class);
#if DEBUG
        if (debcnt || ((debdesc == desc) && !debcnt)) {
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
          printf("dealloc_mvt desc %-8.8x ref: %d \n",desc,(desc->ref_count));
        if (debdesc == desc && !debcnt)
          printf("dealloc_mvt desc %-8.8x ref: %d \n",desc,(desc->ref_count));
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
        return;
      }
      size = R_MVT((*desc),nrows,class1) * R_MVT((*desc),ncols,class1);
      for (i = 0; i < size; i++)
        if ((x = ((int *)R_MVT((*desc),ptdv,class1))[i]) != NULL)
          DEC_REFCNT((PTR_DESCRIPTOR)x);
        else
          break;               /* partially filled matrix / vector / tvector */
    }
    FREEHEAP(R_MVT((*desc),ptdv,class1));
    FREEDESC(desc);
  }
} /**** end of function dealloc_mvt ****/
#endif 

/*------------------------------------------------------------------------
 * dealloc_ex -- Freigabe eines Expression-Deskriptors
 * ruft auf:     freeexpr, FREEDESC, FREEHEAP, DEC_REFCNT.
 *------------------------------------------------------------------------
 */
int dealloc_ex(desc)
PTR_DESCRIPTOR desc;
{
  register int               dim;                                                    /* CS */
  register int                 i;
  int                      anz,x;
  PTR_HEAPELEM        p_arg,ptse;
  PTR_HEAPELEM     *offset,*code;
  PTR_DESCRIPTOR   desc1,sw_desc;
  STACKELEM            *namelist;

  ISDESC(desc);
  switch ( R_DESC((*desc),type) )
  {
    case TY_EXPR:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex expr-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_ex expr-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif   /*     DEBUG   */
         freeexpr(R_EXPR((*desc),pte));
         break;
    case TY_SUB:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex func-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_ex func-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif    /*    DEBUG  */
         if ( R_FUNC((*desc),pte) != NULL )
              freeexpr(R_FUNC((*desc),pte));
         for (i = ((int ) R_FUNC((*desc),nargs) + 1 ); i > 1; i--)
         {
           x = ((int *) R_FUNC((*desc),namelist))[i];
           DEC_REFCNT((PTR_DESCRIPTOR)x);
         }
         FREEHEAP(R_FUNC((*desc),namelist));
         break;
    case TY_CLOS:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex clos-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_ex clos-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif    /*    DEBUG  */
 /*      RES_HEAP;  */
         p_arg = R_CLOS((*desc),pta);
         for (i = R_CLOS((*desc),ndargs)+1 ; i<= p_arg[0] ; i++)
             {
               x = p_arg[i];
               if (T_POINTER(x))
                  DEC_REFCNT((PTR_DESCRIPTOR)x);
             }          
/*       REL_HEAP;    */ 
         FREEHEAP(R_CLOS((*desc),pta));  

/*       freeexpr(R_CLOS((*desc),pta));*/

         x = (int)R_CLOS((*desc),ptfd);
         DEC_REFCNT((PTR_DESCRIPTOR)x);
         break;

    case TY_P_CLOS:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex primclos-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_ex primclos-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif    /*    DEBUG  */

         anz = (R_P_CLOS((*desc),pta))[0];
         for (i = R_P_CLOS((*desc),ndargs)+1 ; i<= anz ; i++)
             {
               x = (R_P_CLOS((*desc),pta))[i];
               if (T_POINTER(x))
                  DEC_REFCNT((PTR_DESCRIPTOR)x);
             }          
         FREEHEAP(R_P_CLOS((*desc),pta));  
         break;


    case TY_MATCH:

#if        DEBUG
        if (debcnt)  {
                printf(
                "dealloc_ex match-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));

        }
#endif   /*     DEBUG  */
         
         if ( R_MATCH((*desc),code) != (PTR_HEAPELEM)0 )                    
            {
               desc1=(PTR_DESCRIPTOR)((PTR_HEAPELEM *)R_MATCH((*desc),code)[OPAT]);   
              /* das Originalpattern */
               DEC_REFCNT(desc1);  
               code = (PTR_HEAPELEM *)R_MATCH((*desc),code);
               offset = (PTR_HEAPELEM *)(code + (int)code[NAMELIST]);
               /* offset verweist auf die Liste der gebundenen Variablen. */
              
               anz = (int)offset[0];  /* = Anzahl der gebundenen Variablen */
     
               for (i = 1 ; i <= anz ; i++)
                   {
                     x = ((int *)offset)[i];
                     DEC_REFCNT((PTR_DESCRIPTOR)x);
                   }

               FREEHEAP(R_MATCH((*desc),code));  
            }
         
         if (R_MATCH((*desc),guard) != (PTR_HEAPELEM)0)
             freeexpr(R_MATCH((*desc),guard));

         freeexpr(R_MATCH((*desc),body));


                                           
#if        DEBUG
        if (debcnt) {
        printf("refcnt fuer %8.8x decr. \n",(PTR_DESCRIPTOR) * (*(A_MATCH((*desc), code))+1));
        printf("T_POINTE %8.8u . \n",T_POINTER(* (*(A_MATCH((*desc), code))+1)));
        }
#endif   /*     DEBUG  */

/* if(T_POINTER ( * (*(A_MATCH((*desc), code))+2)))
      DEC_REFCNT( (PTR_DESCRIPTOR) * (*(A_MATCH((*desc), code))+2));*/


         break;

    case TY_SWITCH:
#if        DEBUG
        if (debcnt) {
                printf(
                "dealloc_ex switch-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
                }
#endif   /*     DEBUG  */


         for ( i = R_SWITCH((*desc),nwhen) - 1 ; i > 0 ; i--)
             {
              /* Freigabe des i-ten Match-Descriptors: */
              desc1 = (PTR_DESCRIPTOR)(R_SWITCH((*desc),ptse))[i];
              DEC_REFCNT(desc1);

             }                                
         if ((R_SWITCH((*desc),ptse))[R_SWITCH((*desc),nwhen)] != NULL)
          /* Deskriptor fuer den otherwise-Ausdruck freigeben */
             {
              /* Freigabe des i-ten Match-Descriptors: */
              desc1 = (PTR_DESCRIPTOR)(R_SWITCH((*desc),ptse))[R_SWITCH((*desc),nwhen)];
              DEC_REFCNT(desc1);

             }                                

         i = (int)R_SWITCH((*desc),ptse)[0] - 1; 
         desc1 = (PTR_DESCRIPTOR)R_SWITCH((*desc),ptse)[i];
         if (desc1 != NULL )
              DEC_REFCNT( desc1);

         FREEHEAP(R_SWITCH((*desc),ptse));

         break;

    case TY_NOMAT:
#if        DEBUG
        if (debcnt)        
                printf(
                "dealloc_ex nomat- desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif  /*      DEBUG     */


        if (0 != ((PTR_HEAPELEM) (R_NOMAT((*desc),guard_body)[1])))
        {
           if (T_POINTER(R_NOMAT((*desc),guard_body)[1]))
               DEC_REFCNT((PTR_DESCRIPTOR)(R_NOMAT((*desc),guard_body)[1]));  
           if (T_POINTER(R_NOMAT((*desc),guard_body)[2]))
               DEC_REFCNT((PTR_DESCRIPTOR)(R_NOMAT((*desc),guard_body)[2]));  
        }
        if ( R_NOMAT((*desc), act_nomat) != -2 )
        {
          if (T_POINTER(R_NOMAT((*desc),guard_body)[3]))
              DEC_REFCNT((PTR_DESCRIPTOR)(R_NOMAT((*desc),guard_body)[3]));  
        }


        DEC_REFCNT((PTR_DESCRIPTOR)R_NOMAT((*desc),ptsdes));  
       break;

    case TY_LREC:                                                      
#if        DEBUG                                                       
        if (debcnt)                                                   
                printf(                                                
                "dealloc_ex lrec-desc %-8.8x ref: %d \n",              
                desc,(desc->ref_count));                               
#endif    /*    DEBUG  */                                              
                                                                       
#if 0 
        if ( (R_LREC((*desc),ptdv)) != NULL )                         
         {                                                             
           dim = R_LREC((*desc),nfuncs);                               
           for ( ; --dim >= 0; )                                       
             if T_POINTER((x = ((int *) R_LREC((*desc),ptdv))[dim]))   
               if (x != NULL)                                          
                 DEC_REFCNT((PTR_DESCRIPTOR)x);                        
               /* else break; */  /* partially filled func defs */     
                                  /* nicht alle Funktionsdefinitionen  
                                     sollen rekonstruiert werden       */
           FREEHEAP(R_LREC((*desc),ptdv));                             
         }                                                             
                                                                       
         for (i = ((int *) R_LREC((*desc),namelist))[0]; i>0; i--)     
         {                                                             
           x = ((int *) R_LREC((*desc),namelist))[i];                  
           DEC_REFCNT((PTR_DESCRIPTOR)x);                              
         }                                                             
         FREEHEAP(R_LREC((*desc),namelist));                           
         break;    
#endif      
         break;                                     
    case TY_LREC_IND:                                                  
#if        DEBUG                                                       
        if (debcnt)                                                    
                printf(                                                
                "dealloc_ex lrec-ind-desc %-8.8x ref: %d \n",          
                desc,(desc->ref_count));                               
#endif    /*    DEBUG  */                                              
                                                                       
     /*    DEC_REFCNT((PTR_DESCRIPTOR)R_LREC_IND((*desc),ptdd));    */     
         break;                                                        
    case TY_COND:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex cond-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_ex cond-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif   /*     DEBUG  */
         freeexpr(R_COND((*desc),ptte));
         freeexpr(R_COND((*desc),ptee));
         break;
    case TY_NAME:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex name-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_ex name-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif   /*     DEBUG   */
         FREEHEAP(R_NAME((*desc),ptn));
         break;
    case TY_FNAME:
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_ex name-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_ex name-desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif   /*     DEBUG   */
         FREEHEAP(R_FNAME((*desc),ptn));
         break;
    default:
         post_mortem("dealloc: illegal type in descriptor");
  }
  FREEDESC(desc);
}
/*------------------------------------------------------------------------
 * dealloc_co -- Freigabe eines Variablen-Deskriptors
 *------------------------------------------------------------------------
 */
static int dealloc_co(desc)
PTR_DESCRIPTOR desc;
{
  ISDESC(desc);
#if        DEBUG
        if (debcnt)
                printf(
                "dealloc_co desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
        if (debdesc == (int)desc && !debcnt)
                printf(
                "dealloc_co desc %-8.8x ref: %d \n",
                desc,(desc->ref_count));
#endif  /*      DEBUG     */
  DEC_REFCNT((PTR_DESCRIPTOR)R_VAR((*desc),ptnd));
  FREEDESC(desc);
}

/*------------------------------------------------------------------------
 * freeexpr  --  free an expression and contained identifiers
 *------------------------------------------------------------------------
 */
static  freeexpr(p)
        register PTR_HEAPELEM        p;
{
        register PTR_HEAPELEM        q;
        register int        i;

#if        DEBUG
        if (debcnt)
                printf(
                "freeexpr heapelement: %-8.8x\n",p);
#endif  /*      DEBUG     */
        if (p == NULL) return;

        ISHEAP( ((T_HBLOCK *)p) - 1);
        q = p;
        i = *q - TET;
        for ( q++ ; --i>=0 ; q++ )
                if T_POINTER((*q))
                        DEC_REFCNT((PTR_DESCRIPTOR)(*q));
        FREEHEAP(p);
}
              

/***************************************************************************/
/* inq_desmax liefert die maximale Anzahl von Deskriptoren im Heap         */ 
/*            ( benoetigt fuer OS_send )                                   */
/***************************************************************************/                                                                             
int inq_desmax()
{
  return(desmax);
}



/***************************************************************************/
/*                                                                         */
/*     Module NUR fuer die Debug Version!!!!!!!!!!!!!!!!!                  */
/*                                                                         */
/***************************************************************************/

#if DEBUG        /* Das endif steht am Ende der Datei !!!! */

/*------------------------------------------------------------------------
 * hpchain  --  test heap chain
 *------------------------------------------------------------------------
 */
hpchain()
{
        register T_HBLOCK        *p;
        for(p=hpbase ; p->next > p ; p=p->next) {
               /* VOID */ ;
        }
        ASSERT(testbusy(hpend),"heap chain corrupted");
        ASSERT(hpend->next == hpbase,"heap chain corrupted");
        ASSERT(((p->next >= hpbase)&&(p->next <= hpend)),
                       "heap chain corrupted,ptr points out of chain");
}

/*------------------------------------------------------------------------
 * isheap  --  test if p points to a heap element
 *------------------------------------------------------------------------
 */
int        isheap(p)
        register T_HBLOCK        *p;
{
        register T_HBLOCK        *q;
        for(q=hpbase ; q->next > q ; q=q->next)
                if (p == q)
                        return(1);
        return(0);
}

/*------------------------------------------------------------------------
 * isdesc  --  is argument a pointer to a descriptor ?
 *------------------------------------------------------------------------
 */
int isdesc(p)
        register PTR_DESCRIPTOR        p;
{
        if ( !((StHeapD <= p) && (EnHeapD >= p)) )
                return(0);
        if ( ((int)p-(int)StHeapD)%DESLEN != 0 )
                return(0);
        return(1);
}

/*------------------------------------------------------------------------
 * res_heap  --  reserve heap to prevent heap compaction
 *------------------------------------------------------------------------
 */
res_heap()
{
#if        DEBUG
        if (heap_res != 0)
                printf("res_heap: Warning: heap is already reserved!\n");
#endif
        heap_res++;
}

/*------------------------------------------------------------------------
 * rel_heap  --  release heap to allow heap compaction
 *------------------------------------------------------------------------
 */
rel_heap()
{
#if        DEBUG
        if (heap_res == 0)
                printf("rel_heap: Warning: heap is not reserved!\n");
#endif

        heap_res--;
}

/*------------------------------------------------------------------------
 * deschain  --  test descriptor chain
 *------------------------------------------------------------------------
 */
deschain()
{
        register T_DESCRIPTOR        *stp;
        register int                i=0;

        if (deslist==NULL) return;

        for (stp=deslist;stp;stp=HEAP_CHAIN(stp)) {
                ASSERT (StHeapD<=stp && stp<=EnHeapD,
                        "descriptor chain corrupted");
                i++;
        }
        ASSERT (i+descnt == desmax,
                "descriptor chain corrupted, desc's missing");
        if ((i + descnt) != desmax)
          printf("descnt = %d,desmax = %d, Elemente in der Freiliste = %d.\n");
}

/*----------------------------------------------------------------------
 * inc_refcnt  --  increment refcount of descriptor
 *----------------------------------------------------------------------
 */
inc_refcnt(p)
        register PTR_DESCRIPTOR        p ;
{

#if MEASURE
  measure(MS_REFCNT);
#endif

        ISDESC(p);
        if ((p->ref_count) < 65000)
         (p->ref_count)++;
        else
         post_mortem("inc_refcnt: reference - count overflow");
}

/*------------------------------------------------------------------------*/
/* dec_refcnt  --  decrement refcount of descriptor                       */
/* calls:          dealloc.                                               */
/*------------------------------------------------------------------------*/

dec_refcnt(p)
register PTR_DESCRIPTOR p;
{

#if MEASURE
  measure(MS_REFCNT);
#endif

        ISDESC(p);
        if ((p->ref_count) == 0 )
         post_mortem("dec_refcnt: descriptor is freed (reference-count is Null)\n");
        else {
          --(p->ref_count);
          if ((p->ref_count)== 0) {
             PRDEBUG("dec_refcnt : descriptor is to be freed\n");
             dealloc(p);
          }
       }
}

/*------------------------------------------------------------------------
 * test_inc_refcnt  --  increment refcount of descriptor
 *------------------------------------------------------------------------
 */
test_inc_refcnt(ident,fileid,lineid)
        PTR_DESCRIPTOR        ident ;
        char * fileid;
        int lineid ;
{
        inc_refcnt(ident) ;
#if        DEBUG
        if (debcnt)
                printf(
                        "test_inc_refcnt: %-8.8x ref: %d  in  %s:%d\n",
                ident,(ident->ref_count),fileid,lineid);
        if (debdesc == (int)ident && !debcnt)
                printf(
                        "test_inc_refcnt: %-8.8x ref: %d  in  %s:%d\n",
                ident,(ident->ref_count),fileid,lineid);
#endif
}
/*------------------------------------------------------------------------
 * test_dec_refcnt  --  decrement refcount of descriptor
 *------------------------------------------------------------------------
 */
test_dec_refcnt(ident,fileid,lineid)
        PTR_DESCRIPTOR        ident ;

        char * fileid;
        int lineid ;
{

#if        DEBUG
        if (debcnt)
                printf(
                "test_dec_refcnt: vor dec %-8.8x ref: %d in  %s:%d\n",
                ident,(ident->ref_count),fileid,lineid);

        if (debdesc == (int)ident && !debcnt)
                printf(
                "test_dec_refcnt: vor dec %-8.8x ref: %d in  %s:%d\n",
                ident,(ident->ref_count),fileid,lineid);

        DESCHAIN;
        if (inchain(ident))
        {
          printf(
          "test_dec_refcnt: desc %-8.8x ist schon freigegeben worden!\n",
          ident);
          post_mortem("test_dec_refcnt: desc was already freed");
        }
#if LARGE

        if (ident->ref_count <= 2)
          if (ident == _dig0)
            post_mortem("test_dec_refcnt: refcnt for 0 gets too low");
          else if (ident == _dig1)
            post_mortem("test_dec_refcnt: refcnt for 1 gets too low");

#endif
        if (ident->ref_count <= 1) {
#if LARGE
          if (ident == _nilmat)
            post_mortem("test_dec_refcnt: nilmat may not be freed");
          if (ident == _nilvect)
            post_mortem("test_dec_refcnt: nilvect may not be freed");
          if (ident == _niltvect)
            post_mortem("test_dec_refcnt: niltvect may not be freed");
          if (ident == _work)
            post_mortem("test_dec_refcnt: work descriptor may not be freed");
#endif
          if (ident == _nil)
            post_mortem("test_dec_refcnt: nil may not be freed");
          if (ident == _nilstring)
            post_mortem("test_dec_refcnt: nilstring may not be freed");
        } /** end if ident ref_count <=1 **/

                                   
#endif /*  DEBUG  */
        dec_refcnt(ident) ;
#if        DEBUG
        if (debcnt)
                printf(
                        "test_dec_refcnt: %-8.8x ref: %d  in  %s:%d\n",
                        ident,(ident->ref_count),fileid,lineid);
        if (debdesc == (int)ident && !debcnt)
                printf(
                        "test_dec_refcnt: %-8.8x ref: %d  in  %s:%d\n",
                        ident,(ident->ref_count),fileid,lineid);
#endif  /*  DEBUG */
}

/*------------------------------------------------------------------------
 * dealloc  --  deallocate descriptor/heap structures on zero refcount
 *------------------------------------------------------------------------
 */
dealloc(p)
PTR_DESCRIPTOR p;
{
        ISDESC(p);

#if        DEBUG
         ASSERT((R_DESC((*p),class)>0)&&(R_DESC((*p),class)<DEALLOC_TAB_SIZE),"dealloc: Illegal descriptor class");

        if (debcnt)
                {
                free (malloc(1)) ;
                printf("dealloc: desc at %-8.8x \n",p);
                }
        if (debdesc == (int)p && !debcnt)
                {
                free (malloc(1)) ;
                printf("dealloc: desc at %-8.8x \n",p);
                }
#endif

        DESCHAIN;
        HPCHAIN;
        (* dealloc_tab[R_DESC((*p),class)])(p);
        DESCHAIN;
        HPCHAIN;

#if        DEBUG
        if (debcnt)
                {
                free (malloc(1)) ;
                printf("dealloc: desc at %-8.8x freed  mallocs ok \n",p);
                }
        if (debdesc == (int)p && !debcnt)
                {
                free (malloc(1)) ;
                printf("dealloc: desc at %-8.8x freed  mallocs ok \n",p);
                }
#endif
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

        for (stp=deslist ; stp ; stp=HEAP_CHAIN(stp)) {
                if (p == stp)
                        return(1);
        }

        return(0);
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

        fprintf (XFile,"\n");
        q = p->next;                       /* NextHeapElement                */
        fprintf(XFile,"Heap block at: %8.8x, next block at: %8.8x\n",p,q);

        if ( (long)(p->ref) == -1 )
                fprintf(XFile,"non-relocatable dynamic buffer\n");
        else {
                desc_addr = ((unsigned)(p->ref) - (unsigned)StHeapD) /
                                DESLEN * DESLEN;
                desc_addr += (unsigned)StHeapD;               /* add base addr*/
                fprintf(XFile,"Heap back reference: %8.8x in desc at %8.8x\n",
                        p->ref,desc_addr);
        }

        i = 0;
        for (pi=(int *)(p+1) ; pi < (int *)q ; ) {
                pi1 = pi;
                j=0;
                fprintf(XFile,"%8.8x %4.4d:  ",pi,i);
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
}

/*------------------------------------------------------------------------
 * OutHeapArea  --  dump used heap blocks
 *------------------------------------------------------------------------
 */
void        OutHeapArea(XFile)
        FILE                *XFile   ;
{
        T_HBLOCK        *p;


        fprintf(DumpFile,"\n H E A P   A R E A \n\n");

        for ( p=hpbase ; p<hpend ; p=p->next)
                if (p->ref != NULL)
                        DumpElem (XFile,p);
                else
                  fprintf(XFile,"Adr.p: %x  Adr.p->next: %x\n",p,p->next);
}

/*------------------------------------------------------------------------
 * HeapDump  --  dump all used descriptors and the used heap
 *------------------------------------------------------------------------
 */
HeapDump(Ident)
char        *Ident ;

{
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
}

#endif

