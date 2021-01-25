


/******************************************************************************/
/*  MODUL OF ELEMENTARY FUNCTIONS IMPLEMENTED FOR PROCESSING PHASE            */
/*                                                                            */
/*  functions: sin(x) / cos(x) / tan(x) / ln(x) / exp(x)                      */
/* -------------------------------------------------------------------------- */
/*  rbibfunc.c - externals: red_sin, red_cos, red_tan, red_ln, red_exp        */
/*             - called by: rear.c                                            */
/*             - internals: sub_dig                                           */
/*                          pi, pirec,  (evaluation of PI)                    */
/*                          inittrig,trigdesc, rgdesc,                        */
/*                          sintrans, sinfkt, sinrec                          */
/*                          costrans,                                         */
/*                          initexp, expdesc,                                 */
/*                          lnfkt, lnrec, lntrans1, lntrans2,                 */
/*                          expfkt, exprec, exptrans, pow2                    */
/* -------------------------------------------------------------------------- */
/*       Note: We have to take care  that proper freeing  of descriptors  is  */
/*       ===== done  when we use  digit-routines.  This requires intermedium  */
/*             storing of any result of digit-routines  which return pointer  */
/*             to descriptors.                                                */
/******************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rstelem.h"
#include "rmeasure.h"
#include <string.h>                 /* to store length of string PI           */
#include <math.h>                   /* to eval. PIPREC and BIBPREC            */
                                    /* sin(x),cos(x),tan(x),log(x),exp(x)     */
#include <setjmp.h>

/*----------------------------------------------------------------------------*/
/*                            extern functions                                */
/*----------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR sts_dec();           /* rsts-dec.c */
extern PTR_DESCRIPTOR newdesc();           /* rheap.c    */
extern int newheap();                      /* rheap.c    */
extern PTR_DESCRIPTOR digit_add();         /* rdig-add.c */
extern PTR_DESCRIPTOR digit_sub();         /* rdig-sub.c */
extern PTR_DESCRIPTOR digit_div();         /* rdig-div.c */
extern PTR_DESCRIPTOR digit_mul();         /* rdig-mul.c */
extern PTR_DESCRIPTOR digit_mod();         /* rdig-mod.c */
extern PTR_DESCRIPTOR digit_trunc();       /* rdig-v2.c  */
extern PTR_DESCRIPTOR digit_abs();         /* rdig-v2.c  */
extern BOOLEAN        digit_eq();          /* rdig-v1.c  */
extern BOOLEAN        digit_le();          /* rdig-v1.c  */
extern BOOLEAN        digit_lt();          /* rdig-v1.c  */

/*----------------------------------------------------------------------------*/
/*                           extern variables                                 */
/*----------------------------------------------------------------------------*/
extern int            _prec;            /* precision to base BASE             */
extern int            _prec_mult;       /* global reduma mult-precision       */
extern int            _base;            /* base of digitstrings               */

extern jmp_buf        _interruptbuf;    /* <r>, jump-adress for heapoverflow  */
extern BOOLEAN        _digit_recycling; /* <w>, flag for digit routines       */
extern PTR_DESCRIPTOR _desc;            /* <w>, resultdescriptor              */

extern char pic[];                      /* vector containing PI-constant      */
extern char piv[];                      /*            variable PI             */
extern char lnc[];                      /* vector containing LN2-constant     */
extern char lnv[];                      /*            variable LN2            */

#if DEBUG
extern int debconf;                     /* for DEBUG informations during eval.*/
#endif

/*----------------------------------------------------------------------------*/
/*                           global variables                                 */
/*----------------------------------------------------------------------------*/
BOOLEAN inittrigdesc;                  /* flag for trigdesc()                 */
                                       /* (if called once then TRUE)          */
BOOLEAN initexpdesc;                   /* flag for expdesc()                  */
                                       /* (if called once then TRUE)          */

/*----------------------------------------------------------------------------*/
/*                           static variables                                 */
/*----------------------------------------------------------------------------*/
static int     _redumaprec;            /* REDUMA-precision has to saved       */
static int     save_prec_mult;         /* REDUMA-mult-prec has to saved       */
static int     oldprec;                /* if user defines new precision       */
                                       /* this can be determined by           */
                                       /* comparing with oldprec              */

static PTR_DESCRIPTOR argx;            /* transformed arg in sinfkt/sinrec    */
                                       /* eval. of red_ln    in lnfkt/lnrec   */
                                       /* eval. of exp(x)    in expfkt/exprec */
static PTR_DESCRIPTOR xpo4;            /* 4th power of arg   in sinfkt/sinrec */
static PTR_DESCRIPTOR sqrx;            /* square of arg      in sinfkt/sinrec */

static PTR_DESCRIPTOR asqr4;           /* eval. of 'PI'      in pi/pirec      */
static PTR_DESCRIPTOR bsqr4;           /* eval. of 'PI'      in pi/pirec      */
static PTR_DESCRIPTOR csqr4;           /* eval. of 'PI'      in pi/pirec      */

static PTR_DESCRIPTOR zx;              /* eval. of red_ln    in lnfkt/lnrec   */
static PTR_DESCRIPTOR lrsqr;           /* eval. of red_ln    in lnfkt/lnrec   */

/* -------------------------------------  PRE-DEFINED DESCRIPTORS ----------- */
static PTR_DESCRIPTOR _digm1       = NULL,               /* <==>  _dig-1      */
                      _dig0p707    = NULL,               /* 0p707  <==> 0.707 */
                      _dig1p414    = NULL,               /* 1p414  <==> 1.414 */
                      _dig2        = NULL,
                      _dig3        = NULL,
                      _dig3pi      = NULL,
                      _dig4        = NULL,
                      _dig5        = NULL,
                      _dig6        = NULL,
                      _dig7        = NULL,
                      _dig8        = NULL,
                      _dig9        = NULL,
                      _dig11       = NULL,
                      _dig12       = NULL,
                      _dig13       = NULL,
                      _dig15       = NULL,
                      _dig18       = NULL,
                      _dig35       = NULL,
                      _dig57       = NULL,
                      _dig36       = NULL,
                      _dig42       = NULL,
                      _dig99       = NULL,
                      _dig110      = NULL,
                      _dig239      = NULL,
                      _dig5040     = NULL,
                      _dig39916800 = NULL,
                      _dig18p3     = NULL,                /*      18 power  3 */
                      _dig57p3     = NULL,                /*      57 power  3 */
                      _dig239p3    = NULL,                /*     239 power  3 */
                      _dig18p4     = NULL,                /*      18 power  4 */
                      _dig57p4     = NULL,                /*      57 power  4 */
                      _dig239p4    = NULL,                /*     239 power  4 */
                      _dig18p5     = NULL,                /*      18 power  5 */
                      _dig57p5     = NULL,                /*      57 power  5 */
                      _dig239p5    = NULL,                /*     239 power  5 */
                      _dig18p7     = NULL,                /*      18 power  7 */
                      _dig57p7     = NULL,                /*      57 power  7 */
                      _dig239p7    = NULL,                /*     239 power  7 */
                      _dig18p9     = NULL,                /*      18 power  9 */
                      _dig57p9     = NULL,                /*      57 power  9 */
                      _dig239p9    = NULL,                /*     239 power  9 */
                      _dig18p11    = NULL,                /*      18 power 11 */
                      _dig57p11    = NULL,                /*      57 power 11 */
                      _dig239p11   = NULL,                /*     239 power 11 */
                      _dig18p13    = NULL,                /*      18 power 13 */
                      _dig57p13    = NULL,                /*      57 power 13 */
                      _dig239p13   = NULL,                /*     239 power 13 */
                      _dig18p15    = NULL,                /*      18 power 15 */
                      _dig57p15    = NULL,                /*      57 power 15 */
                      _dig239p15   = NULL,                /*     239 power 15 */
                      _digRGPI     = NULL;                /* relative error   */
                                                          /* to evaluate 'PI' */

/*----------------------------------------------------------------------------*/
/*                                  defines                                   */
/*----------------------------------------------------------------------------*/
#define DI_ADD(x,y) (R_DIGIT(DESC(x),sign)==0  \
                    ? (R_DIGIT(DESC(y),sign)==0  \
                      ? digit_add(x,y) : digit_sub(x,y) ) \
                    : (R_DIGIT(DESC(y),sign)==0 \
                      ? digit_sub(y,x) : digit_add(x,y) ) )
#define DI_SUB(x,y)  (R_DIGIT(DESC(x),sign) == 0   \
                     ? (R_DIGIT(DESC(y),sign) == 0  \
                       ? digit_sub(x,y) : digit_add(x,y) ) \
                     : (R_DIGIT(DESC(y),sign) == 0  \
                       ? digit_add(x,y) : digit_sub(y,x) ) )

#define ADD(x,y)    digit_add((x),(y))
#define SUBB(x,y)   digit_sub((x),(y))    /* SUB is already defined elsewhere */
#define MUL(x,y)    digit_mul((x),(y))
#define DIV(x,y)    digit_div((x),(y))
#define MOD(x,y)    digit_mod((x),(y))
#define TRUNC(x)    digit_trunc((x))
#define ABS(x)      digit_abs((x))
#define EQ(x,y)     digit_eq((x),(y))
#define LE(x,y)     digit_le((x),(y))
#define LT(x,y)     digit_lt((x),(y))

/* free descriptor created with routine sts_dec   in rsts-dec.c  and          */
/* decrement standard descriptor counter                                      */
#define FREE_STDESC(id)     DEC_REFCNT(id); \
                            DEC_REFCNT(id); \
                            stdesccnt --;

#define MINUS_SIGN        1            /* value, stored in sign-comp. of desc.*/
#define PLUS_SIGN         0            /* value, stored in sign-comp. of desc.*/

#define RG    (_prec10 * (-1))

/* definition of exponent of relative error for PI                            */
#define RGPI  ((_prec10 + 3) * (-1))

/* special precision during the eval. of sin/cos/tan/exp/ln                   */
#define  BIBPREC  (int) ((double)(_prec10 + 2) *   \
                        log((double)10) / log((double)_base) + 0.9999)


/* special precision during the eval. of  number 'PI'                         */
#define  PIPREC  (int) ((double)(_prec10 + 9) *   \
                       log((double)10) / log((double)_base) + 0.9999)


/* ---- macros for heapoverflow-handling ---- */
#define INIT_DESC          {if ((desc = newdesc()) == NULL) \
                              longjmp(_interruptbuf,0);}
#define NEWHEAP(size,adr)  {if (newheap(size,adr) == NULL) \
                              longjmp(_interruptbuf,0);}
#define TEST_DESC          {if (_desc == NULL)  \
                              longjmp(_interruptbuf,0);}
#define TEST(desc)         {if ((desc) == NULL) \
                              post_mortem("rbibfunc : Heap out of space");}
#define NEWHEAPSUB(size,adr)  {if (newheap(size,adr) == NULL) \
                              post_mortem("sub_dig : Heap out of space");}

#define DESC(x)  (* (PTR_DESCRIPTOR) x)


/*****************************************************************************/
/*                       sub_dig     (sub-digitstring)                       */
/* ------------------------------------------------------------------------- */
/*   function:  extracts certain number of digits from a digit-string;       */
/* parameters:  _dig   - pointer to descriptor of source-string              */
/*              ndigit - number of digits to be extracted at most            */
/*                       (starting with most significant digit); if          */
/*                       source-string is shorter than ndigit, ndigit will   */
/*                       be set to length of it; leaving 0s are cut off !!   */
/*    returns:  pointer to descriptor of extracted string                    */
/*  called by:  pirec, sinfkt, lnfkt, expfkt                                 */
/* ------------------------------------------------------------------------- */
/*       Note: digit-strings are placed reversed in heap, i.e. less sig-     */
/*       ===== nificant digits have lower addresses.                         */
/*****************************************************************************/
PTR_DESCRIPTOR sub_dig(_dig,ndigit)
  PTR_DESCRIPTOR _dig;
  COUNT        ndigit;
{
  register PTR_HEAPELEM   ptr1,ptr2;           /*     pointer for copying    */
  PTR_DESCRIPTOR desc;                         /*      return-descriptor     */
  COUNT i,j;                                   /*      counter-variable      */

  INIT_DESC;                                   /*         create 'desc'      */
  DESC_MASK(desc,1,C_DIGIT,TY_DIGIT);          /*         initialize         */

  L_DIGIT((*desc),Exp)    = R_DIGIT((*_dig),Exp);
  L_DIGIT((*desc),sign)   = R_DIGIT((*_dig),sign);

  if (R_DIGIT((*_dig),ndigit) < ndigit)        /* source-string shorter than */
      ndigit = R_DIGIT((*_dig),ndigit);        /* ndigit;                    */

  /* ---- set pointer in source-string to copy-startposition ---- */
  ptr1 = R_DIGIT((*_dig),ptdv) + R_DIGIT((*_dig),ndigit) - ndigit;

  for (j=0; (*ptr1 == 0)&&(R_DIGIT((*_dig),ndigit) > 1); j++)
      ptr1++;                                  /*    don't copy leaving 0s   */
  ndigit -= j;                                 /*   no space for leaving 0s  */

  L_DIGIT((*desc),ndigit) = ndigit;
  NEWHEAPSUB(ndigit,A_DIGIT((*desc),ptdv));    /*     allocate heapspace     */
  ptr2 = R_DIGIT((*desc),ptdv);

  RES_HEAP;
  for (i=ndigit; i ; i--)                      /*       copy substring       */
      *ptr2++ = *ptr1++;
  REL_HEAP;

  return(desc);
}


/*****************************************************************************/
/*                          pirec      (pi-recursion)                        */
/*-------------------------------------------------------------------------- */
/* function  : evaluation of 'PI' with  relative error  "_digRGPI"           */
/*                                                                           */
/*             PI = 48*arctan(1/18) + 32*arctan(1/57) - 20*arctan(1/239)     */
/*                                                                           */
/*             The evaluation of the arctan-function with help of            */
/*             'Talylor-series'. The three arctan-functions are integrated   */
/*             in one recursion. Every two terms of the series are trans-    */
/*             formed to one term.                                           */
/*                                                                           */
/* parameter : s             - increasing number of the exponent in          */
/*                             numerator of actual term in 'Taylor-series'   */
/*                             and increasing number of numerator and        */
/*                             denominator in the same term.                 */
/*             ax1,bx1,cx1                                                   */
/*             ax2,bx2,cx2   - special integrated arguments of actual term   */
/*                             in 'Taylor-series'                            */
/*             prz1          - product in numerator of the first exponent    */
/*                             in actual term in 'Taylor-series'             */
/*             prz2          - product in numerator of the second exponent   */
/*                             in actual term in 'Taylor-series'             */
/*             prodz1        - first product in numerator in actual term     */
/*             prodz2        - second product in numerator in actual term    */
/*             prodn         - product in denominator in actual term         */
/*             result        - intermedium result                            */
/*             quot          - evaluated expression per step                 */
/*                                                                           */
/* static var: asqr4,                                                        */
/*             bsqr4,                                                        */
/*             csqr4         - assignment in function pi()                   */
/*                                                                           */
/* result    : The evaluated result (digit 'PI')  is stored in heap at       */
/*             descriptor _digPI                                             */
/*                                                                           */
/* called by : pi()                                                          */
/* calls     : digit_ ***                                                    */
/*****************************************************************************/
pirec(s,ax1,bx1,cx1,ax2,bx2,cx2,prz1,prz2,prodz1,prodz2,prodn,result,quot)

register PTR_DESCRIPTOR prz1,prz2,result,quot;
PTR_DESCRIPTOR s,ax1,bx1,cx1,ax2,bx2,cx2,prodz1,prodz2,prodn;
{
 PTR_DESCRIPTOR help,hlp1,hlp2,hlp3,   /* help-pointers to free descrip-     */
                hlp4,hlp5,hlp6,hlp7,   /* tors, created by digit-rout.'s     */
                hlp8,hlp9,hlp10,hlp11,
                hlp12,hlp13,hlp14;
#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** QUOT / RESULT ***              \n");
#endif
 TEST(hlp1 = DIV(quot,result));

 if (LT(hlp1,_digRGPI)) {
   DEC_REFCNT(hlp1);                        /* --- dealloc hlp1 variable --- */

   TEST(hlp1 = ADD(result,quot));           /* (result + quot)               */

   FREE_STDESC(_digPI);                     /* dealloc standard-descriptor   */
                                            /* _digPI;                       */
   TEST(hlp2 = MUL(_dig4,hlp1));            /* 4 * (result + quot)           */
   TEST(_digPI = sub_dig(hlp2,_prec + 1));  /* cut PI to needed length       */
   DEC_REFCNT(hlp2);                        /* --- dealloc hlp2 variable --- */
   INC_REFCNT(_digPI);                      /* _digPI points to digit-desc.  */
                                            /* of evaluated 'PI';            */
   stdesccnt ++;                            /* standard-desc.count ++        */
                                            /* _digPI is standard-descriptor */

   DEC_REFCNT(hlp1);                        /* --- dealloc hlp1 variable --- */

   DEC_REFCNT(asqr4);                       /* -- dealloc static variable -- */
   DEC_REFCNT(bsqr4);                       /* -- dealloc static variable -- */
   DEC_REFCNT(csqr4);                       /* -- dealloc static variable -- */

   DEC_REFCNT(s);                           /*   dealloc descriptors         */
   DEC_REFCNT(ax1);                         /*   of "function parameters"    */
   DEC_REFCNT(bx1);
   DEC_REFCNT(cx1);
   DEC_REFCNT(ax2);
   DEC_REFCNT(bx2);
   DEC_REFCNT(cx2);
   DEC_REFCNT(prz1);
   DEC_REFCNT(prz2);
   DEC_REFCNT(prodz1);
   DEC_REFCNT(prodz2);
   DEC_REFCNT(prodn);
   DEC_REFCNT(result);
   DEC_REFCNT(quot);

#if DEBUG
   if (debconf) printf("\n\n\n pirec: **************** 'PI' is evaluated ****************\n");
#endif

   return;
 }
 else {   /*---------------------------------------------------------------*/
          /* ((quot/result) ge _digRGPI)        start of next recursion    */
          /*---------------------------------------------------------------*/

   DEC_REFCNT(hlp1);

#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 1st parameter ***  \n");
#endif
   TEST(hlp1 = ADD(s,_dig1));


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 2nd parameter ***  \n");
#endif
   TEST(hlp2 = MUL(ax1,asqr4));


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 3rd parameter ***  \n");
#endif
   TEST(hlp3 = MUL(bx1,bsqr4));


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 4th parameter ***  \n");
#endif
   TEST(hlp4 = MUL(cx1,csqr4));


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 5th parameter ***  \n");
#endif
   TEST(hlp5 = MUL(ax2,asqr4));


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 6th parameter ***  \n");
#endif
   TEST(hlp6 = MUL(bx2,bsqr4));


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 7th parameter ***  \n");
#endif
   TEST(hlp7 = MUL(cx2,csqr4));


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 8th parameter ***  \n");
#endif
   TEST(help = MUL(_dig4,s));
   TEST(hlp8 = ADD(help,_dig1));      /* (4*s) + 1 */


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 9th parameter ***  \n");
#endif
   TEST(hlp9 = ADD(help,_dig3pi));    /* (4*s) + 3 */
   DEC_REFCNT(help);
   DEC_REFCNT(s);


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 10th parameter *** \n");
#endif
   TEST(hlp10 = MUL(_dig12,ax1));    /* (pa*ax1)                           */
   DEC_REFCNT(ax1);

   TEST(hlp11 = MUL(_dig8,bx1));     /* (pb*bx1)                           */
   DEC_REFCNT(bx1);

   TEST(hlp12 = ADD(hlp10,hlp11));   /* ((pa*ax1) + (pb*bx1))              */
   DEC_REFCNT(hlp10);
   DEC_REFCNT(hlp11);

   TEST(hlp10 = MUL(_dig5,cx1));     /* (pc*cx1)                           */
   DEC_REFCNT(cx1);

   TEST(hlp11 = SUBB(hlp12,hlp10));  /* (((pa*ax1) + (pb*bx1)) - (pc*cx1)) */
   DEC_REFCNT(hlp12);
   DEC_REFCNT(hlp10);

   TEST(hlp10 = MUL(prz2,hlp11));    /* prz2 *                             */
   DEC_REFCNT(hlp11);                /* (((pa*ax1) + (pb*bx1)) - (pc*cx1)) */


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 11th parameter *** \n");
#endif
   TEST(hlp11 = MUL(_dig12,ax2));    /* (pa*ax2)                           */
   DEC_REFCNT(ax2);

   TEST(hlp12 = MUL(_dig8,bx2));     /* (ba*bx2)                           */
   DEC_REFCNT(bx2);

   TEST(hlp13 = ADD(hlp11,hlp12));   /* ((pa*ax2) + (ba*bx2))              */
   DEC_REFCNT(hlp11);
   DEC_REFCNT(hlp12);

   TEST(hlp11 = MUL(_dig5,cx2));     /* (pc*cx2)                           */
   DEC_REFCNT(cx2);

   TEST(hlp12 = SUBB(hlp13,hlp11));  /* (((pa*ax2) + (ba*bx2)) - (pc*cx2)) */
   DEC_REFCNT(hlp13);
   DEC_REFCNT(hlp11);

   TEST(hlp11 = MUL(prz1,hlp12));    /* prz1 *                             */
   DEC_REFCNT(hlp12);                /* (((pa*ax2) + (ba*bx2)) - (pc*cx2)) */


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 12th parameter *** \n");
#endif
   TEST(hlp12 = MUL(prz1,prz2));
   DEC_REFCNT(prz1);
   DEC_REFCNT(prz2);


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 13th parameter *** \n");
#endif
   TEST(hlp13 = ADD(result,quot));
   DEC_REFCNT(result);
   DEC_REFCNT(quot);


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** start eval. 14th parameter *** \n");
#endif
   TEST(help = SUBB(prodz1,prodz2));/* (prodz1 - prodz2)                  */
   DEC_REFCNT(prodz1);
   DEC_REFCNT(prodz2);

   TEST(hlp14 = DIV(help,prodn));   /* (prodz1 - prodz2) / prodn          */
   DEC_REFCNT(help);
   DEC_REFCNT(prodn);


#if DEBUG
   if (debconf) printf("\n\n\n pirec: *** call of pirec ***                \n");
#endif

   pirec(hlp1,hlp2,hlp3,hlp4,hlp5,hlp6,hlp7,hlp8,hlp9,hlp10,hlp11,
                                                 hlp12,hlp13,hlp14);
 }
} /*** end of function pirec ***/


/*****************************************************************************/
/*                          pi    (initialization - routine)                 */
/*---------------------------------------------------------------------------*/
/* function  : creates descriptors for constants to eval. 'PI' and           */
/*             initializes recursive call of pirec to eval. 'PI' ;           */
/*             free all decriptors not used in pirec                         */
/*                                                                           */
/* static var: asqr4  = (1 /  18power4)                                      */
/*             bsqr4  = (1 /  57power4)                                      */
/*             csqr4  = (1 / 239power4)                                      */
/*                                                                           */
/* NOTE      : pa = (48 / 4) = _dig12                                        */
/*             pb = (32 / 4) = _dig8                                         */
/*             pc = (20 / 4) = _dig5                                         */
/*                                                                           */
/* called by : red_sin / red_cos / red_tan                                   */
/* calls     : pirec   / sts_dec / digit_ ***                                */
/*****************************************************************************/
void pi() {
 PTR_DESCRIPTOR hlp1,hlp2,hlp3,hlp4;   /* help-pointers to free descrip-     */
                                       /* tors, created by digit-rout.'s     */
 PTR_DESCRIPTOR ax1,bx1,cx1,
                ax2,bx2,cx2,
                prodz1,prodz2,
                result,
                quot;


  TEST(_digRGPI = sts_dec("1",RGPI));  /* special relative error descriptor  */
                                       /* to evaluate PI                     */
#if DEBUG
  if (debconf) printf("\n\n\n pi: *** end sts_desc initalization of _digRGPI  ***      \n");
#endif

  _redumaprec = _prec;                 /* save the global REDUMA reduction-  */
                                       /* parameter precision                */
  _prec = PIPREC;                      /* set the precision for the          */
                                       /* evaluation of 'PI'                 */
  save_prec_mult = _prec_mult;         /* save the global REDUMA reduction-  */
                                       /* parameter _prec_mult               */
  _prec_mult = PIPREC;                 /* set internal used _prec_mult       */


  /* ------- initialize descriptors (stored in static variables) ----------- */
  /* ----------------- using in pirec to evaluate 'PI' --------------------- */
  TEST(_dig12 = sts_dec("12",0));       /* (48 / 4) = _dig12                   */

  TEST(_dig18p4  = sts_dec("104976",0));/* 18 power  4                       */
  TEST(asqr4 = DIV(_dig1,_dig18p4));
  FREE_STDESC(_dig18p4);                /* ------ dealloc _dig18p4 --------- */

  TEST(_dig57p4  = sts_dec("10556001",0));    /* 57 power  4                 */
  TEST(bsqr4 = DIV(_dig1,_dig57p4));
  FREE_STDESC(_dig57p4);                /* ------ dealloc _dig57p4 --------- */

  TEST(_dig239p4 = sts_dec("3262808641",0));  /* 239 power  4                */
  TEST(csqr4 = DIV(_dig1,_dig239p4));
  FREE_STDESC(_dig239p4);               /* ------ dealloc _dig239p4 -------- */


  /* ------------------- start initialize evaluation of 2nd parameter ------ */
  TEST(_dig18p13  = sts_dec("20822964865671168",0)); /*  18 power 13         */
  TEST(ax1 = DIV(_dig1,_dig18p13));
  FREE_STDESC(_dig18p13);               /* ------ dealloc _dig18p13 -------- */


  /* ------------------- start initialize evaluation of 3rd parameter ------ */
  TEST(_dig57p13  = sts_dec("67046038752496061076057",0)); /*  57 power 13   */
  TEST(bx1 = DIV(_dig1,_dig57p13));
  FREE_STDESC(_dig57p13);              /* ------ dealloc _dig57p13 --------- */



  /* ------------------- start initialize evaluation of 4th parameter ------ */
  TEST(_dig239p13 = sts_dec("8301808522036075428899283574319",0));
                                       /* 239 power 13                       */
  TEST(cx1 = DIV(_dig1,_dig239p13));
  FREE_STDESC(_dig239p13);             /* ------ dealloc _dig239p13 -------- */


  /* ------------------- start initialize evaluation of 5th parameter ------ */
  TEST(_dig18p15  = sts_dec("6746640616477458432",0)); /* 18 power 15        */
  TEST(ax2 = DIV(_dig1,_dig18p15));
  FREE_STDESC(_dig18p15);              /* ------ dealloc _dig18p15 --------- */


  /* ------------------- start initialize evaluation of 6th parameter ------ */
  TEST(_dig57p15  = sts_dec("217832579906859702436109193",0));
                                       /*  57 power 15                       */
  TEST(bx2 = DIV(_dig1,_dig57p15));
  FREE_STDESC(_dig57p15);              /* ------ dealloc _dig57p15 --------- */


  /* ------------------- start initialize evaluation of 7th parameter ------ */
  TEST(_dig239p15 = sts_dec("474207604587222664574155977048675599",0));
                                       /* 239 power 15                       */
  TEST(cx2 = DIV(_dig1,_dig239p15));
  FREE_STDESC(_dig239p15);             /* ------ dealloc _dig239p15 -------- */


  /* ------------------- start initialize evaluation of 10th parameter ----- */
  TEST(_dig18p9  = sts_dec("198359290368",0)); /* 18 power  9                */
  TEST(hlp1 = DIV(_dig1,_dig18p9));   /* (1/18p9)                            */
  TEST(hlp2 = MUL(_dig12,hlp1));      /* (pa* (1/18p9))                      */
  DEC_REFCNT(hlp1);
  FREE_STDESC(_dig18p9);              /* ------- dealloc _dig18p9----------- */

  TEST(_dig57p9  = sts_dec("6351461955384057",0)); /* 57 power  9            */
  TEST(hlp1 = DIV(_dig1,_dig57p9));   /* (1/57p9)                            */
  TEST(hlp3 = MUL(_dig8,hlp1));       /* (pb* (1/57p9))                      */
  DEC_REFCNT(hlp1);
  FREE_STDESC(_dig57p9);              /* ------- dealloc _dig57p9 ---------- */

  TEST(hlp1 = ADD(hlp2,hlp3));        /* ((pa*(1/18p9)) + (pb*(1/57p9)))     */
  DEC_REFCNT(hlp2);
  DEC_REFCNT(hlp3);

  TEST(_dig239p9 = sts_dec("2544374934440439784559",0)); /* 239 power  9     */
  TEST(hlp2 = DIV(_dig1,_dig239p9)); /* (1/239p9)                            */
  TEST(hlp3 = MUL(_dig5,hlp2));      /* (pc* (1/239p9))                      */
  DEC_REFCNT(hlp2);
  FREE_STDESC(_dig239p9);            /* ------- dealloc _dig239p9 ---------- */

  TEST(hlp2 = SUBB(hlp1,hlp3));     /* (((pa*(1/18p9)) + (pb*(1/57p9))) -   */
  DEC_REFCNT(hlp1);                 /*  (pc* (1/239p9)))                    */
  DEC_REFCNT(hlp3);

  TEST(_dig11  = sts_dec("11",0));
  TEST(prodz1 = MUL(_dig11,hlp2));   /* 11 * (((pa*(1/18p9))  +              */
                                     /*        (pb*(1/57p9))) -              */
  DEC_REFCNT(hlp2);                  /*       (pc* (1/239p9)))               */
  FREE_STDESC(_dig11);               /* ------- dealloc _dig11 ------------- */


  /* ------------------- start initialize evaluation of 11th parameter ----- */
  TEST(_dig18p11  = sts_dec("64268410079232",0)); /*  18 power 11            */
  TEST(hlp1 = DIV(_dig1,_dig18p11)); /* (1/18p11)                            */
  TEST(hlp2 = MUL(_dig12,hlp1));     /* (pa* (1/18p11))                      */
  DEC_REFCNT(hlp1);
  FREE_STDESC(_dig18p11);            /* ------- dealloc _dig18p11 ---------- */

  TEST(_dig57p11  = sts_dec("20635899893042801193",0)); /*  57 power 11      */
  TEST(hlp1 = DIV(_dig1,_dig57p11)); /* (1/57p11)                            */
  TEST(hlp3 = MUL(_dig8,hlp1));      /* (pa* (1/57p11))                      */
  DEC_REFCNT(hlp1);
  FREE_STDESC(_dig57p11);            /* ------- dealloc _dig57p11 ---------- */

  TEST(hlp1 = ADD(hlp2,hlp3));       /* ((pa*(1/18p19)) + (pb*(1/57p19)))    */
  DEC_REFCNT(hlp2);
  DEC_REFCNT(hlp3);

  TEST(_dig239p11 = sts_dec("145337240630172360933794639",0));/* 239 power 11*/
  TEST(hlp2 = DIV(_dig1,_dig239p11));/* (1/239p11)                           */
  TEST(hlp3 = MUL(_dig5,hlp2));      /* (pc* (1/239p11))                     */
  DEC_REFCNT(hlp2);
  FREE_STDESC(_dig239p11);           /* ------- dealloc _dig239p11 --------- */

  TEST(hlp2 = SUBB(hlp1,hlp3));      /* (((pa*(1/18p11)) + (pb*(1/57p11))) - */
  DEC_REFCNT(hlp1);                  /*  (pc* (1/239p11)))                   */
  DEC_REFCNT(hlp3);

  TEST(_dig9  = sts_dec("9",0));
  TEST(prodz2 = MUL(_dig9,hlp2));    /*  9 * (((pa*(1/18p11))  +             */
                                     /*        (pb*(1/57p11))) -             */
  DEC_REFCNT(hlp2);                  /*       (pc* (1/239p11)))              */
  FREE_STDESC(_dig9);                /* -------- dealloc _dig9 ------------- */


  /* ------------------- start initialize evaluation of 13th parameter ----- */
  TEST(_dig18 = sts_dec("18",0));
  TEST(hlp1 = DIV(_dig1,_dig18));    /* (1/18)                               */
  TEST(hlp2 = MUL(_dig12,hlp1));     /* (pa*(1/18))                          */
  DEC_REFCNT(hlp1);
  FREE_STDESC(_dig18);               /* -------- dealloc _dig18 ------------ */

  TEST(_dig57 = sts_dec("57",0));
  TEST(hlp1 = DIV(_dig1,_dig57));    /* (1/57)                               */
  TEST(hlp3 = MUL(_dig8,hlp1));      /* (pb*(1/57))                          */
  DEC_REFCNT(hlp1);
  FREE_STDESC(_dig57);               /* -------- dealloc _dig57 ------------ */

  TEST(hlp1 = ADD(hlp2,hlp3));       /* ((pa*(1/18)) + (pb*(1/57)))          */
  DEC_REFCNT(hlp2);
  DEC_REFCNT(hlp3);

  TEST(_dig239 = sts_dec("239",0));
  TEST(hlp2 = DIV(_dig1,_dig239));   /* (1/239)                              */
  TEST(hlp3 = MUL(_dig5,hlp2));      /* (pc*(1/239))                         */
  DEC_REFCNT(hlp2);
  FREE_STDESC(_dig239);              /* -------- dealloc _dig239 ----------- */

  TEST(hlp2 = SUBB(hlp1,hlp3));      /* (((pa*(1/18)) + (pb*(1/57))) -       */
  DEC_REFCNT(hlp1);                  /*  (pc*(1/239)))                       */
  DEC_REFCNT(hlp3);

  TEST(_dig3pi = sts_dec("3",0));
  TEST(hlp1    = MUL(_dig3pi,hlp2)); /* 3 * (((pa*(1/18)) + (pb*(1/57))) -   */
  DEC_REFCNT(hlp2);                  /*      (pc*(1/239)))                   */
                                  /* " prodz1 first term " is stored in hlp1 */

  TEST(_dig18p3 = sts_dec("5832",0));      /*  18 power  3                   */
  TEST(hlp2 = DIV(_dig1,_dig18p3));  /* (1/18p3)                             */
  TEST(hlp3 = MUL(_dig12,hlp2));     /* (pa*(1/18p3))                        */
  DEC_REFCNT(hlp2);
  FREE_STDESC(_dig18p3);             /* ------- dealloc _dig18p3 ----------- */

  TEST(_dig57p3 = sts_dec("185193",0));    /*  57 power  3                   */
  TEST(hlp2 = DIV(_dig1,_dig57p3));  /* (1/57p3)                             */
  TEST(hlp4 = MUL(_dig8,hlp2));      /* (pb*(1/57p3))                        */
  DEC_REFCNT(hlp2);
  FREE_STDESC(_dig57p3);             /* ------- dealloc _dig57p3 ----------- */

  TEST(hlp2 = ADD(hlp3,hlp4));       /* ((pa*(1/18p3)) + (pb*(1/57p3)))      */
  DEC_REFCNT(hlp3);
  DEC_REFCNT(hlp4);

  TEST(_dig239p3 = sts_dec("13651919",0)); /* 239 power  3                   */
  TEST(hlp3 = DIV(_dig1,_dig239p3)); /* (1/239p3)                            */
  TEST(hlp4 = MUL(_dig5,hlp3));      /* (pc*(1/239p3))                       */
  DEC_REFCNT(hlp3);
  FREE_STDESC(_dig239p3);            /* ------- dealloc _dig239p3 ---------- */

  TEST(hlp3 = SUBB(hlp2,hlp4));      /* (((pa*(1/18p3)) + (pb*(1/57p3))) -   */
  DEC_REFCNT(hlp2);                  /*  (pc*(1/239p3)))                     */
  DEC_REFCNT(hlp4);
                                  /* " prodz2 first term " is stored in hlp3 */

  TEST(hlp2 = SUBB(hlp1,hlp3));      /* ("(prodz1 first term)"  -            */
  DEC_REFCNT(hlp1);                  /* "(prodz2 first term)")               */
  DEC_REFCNT(hlp3);

  TEST(result = DIV(hlp2,_dig3pi));  /* ("(prodz1 first term)"  -            */
                                     /* "(prodz2 first term)")    / 3        */
                                     /* result of first term is stored       */
  DEC_REFCNT(hlp2);                  /* in variable result                   */


  /* ------------------- start initialize evaluation of 14th parameter ----- */
  TEST(_dig18p5  = sts_dec("1889568",0));  /*  18 power  5                   */
  TEST(hlp1 = DIV(_dig1,_dig18p5));  /* (1/18p5)                             */
  TEST(hlp2 = MUL(_dig12,hlp1));     /* (pa*(1/18p5))                        */
  DEC_REFCNT(hlp1);
  FREE_STDESC(_dig18p5);             /* ------- dealloc _dig18p5 ----------- */

  TEST(_dig57p5  = sts_dec("601692057",0));/*  57 power  5                   */
  TEST(hlp1 = DIV(_dig1,_dig57p5));  /* (1/57p5)                             */
  TEST(hlp3 = MUL(_dig8,hlp1));      /* (pb*(1/57p5))                        */
  DEC_REFCNT(hlp1);
  FREE_STDESC(_dig57p5);             /* ------- dealloc _dig57p5 ----------- */

  TEST(hlp1 = ADD(hlp2,hlp3));       /* ((pa*(1/18p5)) + (pb*(1/57p5)))      */
  DEC_REFCNT(hlp2);
  DEC_REFCNT(hlp3);

  TEST(_dig239p5 = sts_dec("779811265199",0)); /* 239 power  5               */
  TEST(hlp2 = DIV(_dig1,_dig239p5)); /* (1/239p5)                            */
  TEST(hlp3 = MUL(_dig5,hlp2));      /* (pc*(1/239p5))                       */
  DEC_REFCNT(hlp2);
  FREE_STDESC(_dig239p5);            /* ------- dealloc _dig239p5 ---------- */

  TEST(hlp2 = SUBB(hlp1,hlp3));      /* (((pa*(1/18p5)) + (pb*(1/57p5))) -   */
  DEC_REFCNT(hlp1);                  /*  (pc*(1/239p5)))                     */
  DEC_REFCNT(hlp3);

  TEST(hlp1 = MUL(_dig7,hlp2));      /* 7 * (((pa*(1/18p5))  +               */
                                     /*      (pb*(1/57p5)))  -               */
  DEC_REFCNT(hlp2);                  /*      (pc*(1/239p5)))                 */
                                 /* " prodz1 second term " is stored in hlp1 */

  TEST(_dig18p7  = sts_dec("612220032",0)); /*  18 power  7                  */
  TEST(hlp2 = DIV(_dig1,_dig18p7));  /* (1/18p7)                             */
  TEST(hlp3 = MUL(_dig12,hlp2));     /* (pa*(1/18p7))                        */
  DEC_REFCNT(hlp2);
  FREE_STDESC(_dig18p7);             /* ------- dealloc _dig18p7 ----------- */

  TEST(_dig57p7  = sts_dec("1954897493193",0)); /*  57 power  7              */
  TEST(hlp2 = DIV(_dig1,_dig57p7));  /* (1/57p7)                             */
  TEST(hlp4 = MUL(_dig8,hlp2));      /* (pb*(1/57p7))                        */
  DEC_REFCNT(hlp2);
  FREE_STDESC(_dig57p7);             /* ------- dealloc _dig57p7 ----------- */

  TEST(hlp2 = ADD(hlp3,hlp4));       /* ((pa*(1/18p7)) + (pb*(1/57p7)))      */
  DEC_REFCNT(hlp3);
  DEC_REFCNT(hlp4);

  TEST(_dig239p7 = sts_dec("44543599279432079",0)); /* 239 power  7          */
  TEST(hlp3 = DIV(_dig1,_dig239p7)); /* (1/239p7)                            */
  TEST(hlp4 = MUL(_dig5,hlp3));      /* (pc*(1/239p7))                       */
  DEC_REFCNT(hlp3);
  FREE_STDESC(_dig239p7);            /* ------- dealloc _dig239p7 ---------- */

  TEST(hlp3 = SUBB(hlp2,hlp4));      /* (((pa*(1/18p7)) + (pb*(1/57p7))) -   */
  DEC_REFCNT(hlp2);                  /*  (pc*(1/239p7)))                     */
  DEC_REFCNT(hlp4);

                                     /* _dig5 is a standard descriptor       */
                                     /* allocated in trigdesc or in expdesc  */
  TEST(hlp2 = MUL(_dig5,hlp3));      /* 5 * (((pa*(1/18p7))   +              */
                                     /*       (pb*(1/57p7)))  -              */
  DEC_REFCNT(hlp3);                  /*      (pc*(1/239p7)))                 */
                                 /* " prodz2 second term " is stored in hlp2 */

  TEST(hlp3 = SUBB(hlp1,hlp2));      /* ("(prodz1 second term)"  -           */
  DEC_REFCNT(hlp1);                  /* "(prodz2 second term)")              */
  DEC_REFCNT(hlp2);

  TEST(_dig35 = sts_dec("35",0));
  TEST(quot   = DIV(hlp3,_dig35));   /* ("(prodz1 second term)"  -           */
                                     /* "(prodz2 second term)")    / 35      */
  DEC_REFCNT(hlp3);                  /* result of second term is stored      */
                                     /* in variable quot                     */
  FREE_STDESC(_dig35);               /* ------- dealloc _dig35 ------------- */

#if DEBUG
   if (debconf) printf("\n\n\n pi: *** end initalization recursive call of pirec  ***              \n");
#endif

 TEST(_dig13 = sts_dec("13",0));
 TEST(_dig15 = sts_dec("15",0));
 TEST(_dig99 = sts_dec("99",0));
                                     /* _dig4 is a standard descriptor      */
                                     /* allocated in trigdesc               */

 /* --------------------  FUNCTION CALL OF PIREC  ------------------------- */
 /* ---> car INC_REFCNT extracted from functioncall */
 INC_REFCNT(_dig4);
 INC_REFCNT(_dig13);
 INC_REFCNT(_dig15);
 INC_REFCNT(_dig99);
 pirec(_dig4, ax1,bx1,cx1,ax2,bx2,cx2,_dig13,_dig15,prodz1,prodz2,
       _dig99,result,quot);

                                     /* no FREE_STDESC(_dig8)                */
                                     /* no FREE_STDESC(_dig5)                */
                                     /* allocated as standard-descriptors in */
                                     /* trigdesc or expdesc                  */
 FREE_STDESC(_digRGPI);              /* --------- dealloc _digRGPI --------- */
 FREE_STDESC(_dig3pi);               /* --------- dealloc _dig3pi ---------- */
 FREE_STDESC(_dig12);                /* --------- dealloc _dig12 ----------- */
 FREE_STDESC(_dig13);                /* --------- dealloc _dig13 ----------- */
 FREE_STDESC(_dig15);                /* --------- dealloc _dig15 ----------- */
 FREE_STDESC(_dig99);                /* --------- dealloc _dig99 ----------- */

 _prec = _redumaprec;                /* reset the global REDUMA              */
                                     /* reduction parameter precision        */
 _prec_mult = save_prec_mult;        /* reset the global REDUMA              */
                                     /* reduction parameter _prec_mult       */

} /*** end of function pi ***/



/******************************************************************************/
/*                  trigdesc (sin/cos/tan desc.initialization)                */
/*----------------------------------------------------------------------------*/
/*   function:  create descriptor-constants for evaluation of trigonometric   */
/*              functions;                                                    */
/* parameters:  none                                                          */
/*    returns:  nothing                                                       */
/*  called by:  triginit                                                      */
/*      calls:  sts_dec                                                       */
/******************************************************************************/
void trigdesc()
{
  TEST(_digm1       = sts_dec("-1",0));
  TEST(_dig4        = sts_dec("4",0));
  TEST(_dig6        = sts_dec("6",0));
  TEST(_dig7        = sts_dec("7",0));
  TEST(_dig8        = sts_dec("8",0));
  TEST(_dig36       = sts_dec("36",0));
  TEST(_dig42       = sts_dec("42",0));
  TEST(_dig110      = sts_dec("110",0));
  TEST(_dig5040     = sts_dec("5040",0));
  TEST(_dig39916800 = sts_dec("39916800",0));

  if (!initexpdesc) {                       /* descriptors for exp(x) & ln(x) */
                                            /* not yet created;               */
     TEST(_dig2     = sts_dec("2",0));
     TEST(_dig5     = sts_dec("5",0));
  }

#if DEBUG
if (debconf) printf("\n\n\n trigdesc: *** end sts_desc initalization of permanent descriptors  ***\n");
#endif

  inittrigdesc = TRUE;                      /* initialization ok; descriptors */
                                            /* created remain permanent in    */
                                            /* heap;                          */
} /*** end of function trigdesc ***/


/******************************************************************************/
/*                   rgdesc (rel. error initialization)                       */
/*----------------------------------------------------------------------------*/
/*   function:  create descriptor-constant 'relative error'                   */
/* parameters:  none                                                          */
/*    returns:  nothing                                                       */
/*  called by:  initexp, inittrig                                             */
/*      calls:  sts_dec                                                       */
/******************************************************************************/
void rgdesc() {
   FREE_STDESC(_digRG);                /* dealloc descriptor _digRG          */
   TEST(_digRG    = sts_dec("1",RG));  /* relative error descriptor, used in */
                                       /* all functions of this file         */
                                       /* (except _digPI)                    */
#if DEBUG
  if (debconf) printf("\n\n\n rgdesc: *** end sts_desc initalization of _digRG  ***\n");
#endif

}  /*** end of function rgdesc ***/

/******************************************************************************/
/*                       expdesc (exp/ln desc.initialization)                 */
/*----------------------------------------------------------------------------*/
/*   function:  create descriptor-constants for evaluation of exponential-    */
/*              and logarithm-function;                                       */
/* parameters:  none                                                          */
/*    returns:  nothing                                                       */
/*  called by:  initexp                                                       */
/*      calls:  sts_dec                                                       */
/******************************************************************************/
void expdesc()
{
  TEST(_dig0p707    = sts_dec("0.707",0));  /*        0p707  <==> 0.707       */
  TEST(_dig1p414    = sts_dec("1.414",0));  /*        1p414  <==> 1.414       */
  TEST(_dig3        = sts_dec("3",0));

  if (!inittrigdesc) {                      /* descriptors for exp(x) & ln(x) */
    TEST(_dig2        = sts_dec("2",0));    /* not yet created;               */
    TEST(_dig5        = sts_dec("5",0));
  }

#if DEBUG
  if (debconf) printf("\n\n\n expdesc: *** end sts_dec initalization of permanent descriptors  ***\n");
#endif

  initexpdesc = TRUE;                       /* initialization ok; descriptors */
                                            /* created remain permanent in    */
                                            /* heap;                          */

} /*** end of function expdesc ***/

/******************************************************************************/
/*                inittrig (initialize red_sin/red_cos/red_tan)               */
/*----------------------------------------------------------------------------*/
/*   function: initialize trigonom. functions and create descriptor-constants */
/*             if necessarry;                                                 */
/* parameters: none                                                           */
/*    returns: nothing                                                        */
/*  called by: red_sin, red_cos, red_tan                                      */
/*      calls: trigdesc, rgdesc                                               */
/******************************************************************************/
void inittrig() {
  if (!inittrigdesc) {                 /*    if trigdesc() not called yet    */
    trigdesc();                        /*     define descriptor constants    */
    if (_prec10 > 100)                 /* PI-constant in PIstring too short  */
      pi();                            /*       create decriptor _digPI      */
    oldprec = _prec10;                 /* new constants successfully stored  */
  }
  else
    if (oldprec != _prec10) {          /*     user defined new precision     */
      rgdesc();                        /*    create new descriptor _digRG    */
      if (_prec10 > 100) {             /* PI-constant in PIstring too short  */
        pi();                          /*    create new descriptor _digPI    */
      }
      else {
        FREE_STDESC(_digPI);           /*      dealloc descriptor _digPI     */

        strncpy(piv,pic,_prec10+6);    /*           copy PI to piv           */
        piv[_prec10+7] = '\0';         /*       cut PI to needed length      */
        TEST(_digPI = sts_dec(piv,0)); /*    create new descriptor _digPI    */
      }
      oldprec = _prec10;               /* new constants successfully stored  */
    }
} /*** end of function inittrig ***/


/******************************************************************************/
/*                     initexp (initialize red_exp/red_ln)                    */
/*----------------------------------------------------------------------------*/
/*   function: initialize red_exp / red_ln and create descriptor-constants    */
/*             if necessarry;                                                 */
/* parameters: none                                                           */
/*    returns: nothing                                                        */
/*  called by: red_exp, red_ln,                                               */
/*      calls: expdesc, rgdesc                                                */
/******************************************************************************/

PTR_DESCRIPTOR lnfkt();

/******************************************************************************/
/*                      ln2 (create descriptor _digLN2)                       */
/*----------------------------------------------------------------------------*/
/*   function: _digLN2 has rank of standard descriptor                        */
/* parameters: none                                                           */
/*    returns: nothing                                                        */
/*  called by: initexp                                                        */
/*      calls: nothing                                                        */
/******************************************************************************/
void ln2() {
  PTR_DESCRIPTOR save_digRG;

  save_prec_mult = _prec_mult;
  _prec_mult = PIPREC;
  _redumaprec = _prec;
  _prec = PIPREC;

  save_digRG = _digRG;
  _digRG = sts_dec("1",RG - 7);        /*    for needed precision of _digLN2 */

  TEST(_digLN2 = lnfkt(_dig2));        /*    create new descriptor _digLN2   */
  INC_REFCNT(_digLN2);
  stdesccnt++;                         /*    standard descriptor count++     */

  FREE_STDESC(_digRG);                 /*     dealloc descriptor _digRG      */
  _digRG = save_digRG;                 /*      resore old relative error     */

  _prec_mult = save_prec_mult;
  _prec = _redumaprec;
}

void initexp() {
  if (!initexpdesc) {                  /*   if expdesc() not called yet      */
    expdesc();                         /*     define descriptor constants    */
  if (_prec10 > 100)
    ln2();                             /*    create new descriptor _digLN2   */
  oldprec = _prec10;                   /* new constants successfully stored  */
  }
  else {
    if (oldprec != _prec10) {          /*     user defined new precision     */
      rgdesc();                        /*    create new descriptor _digRG    */
      FREE_STDESC(_digLN2);            /*      dealloc descriptor _digLN2    */
      if (_prec10 > 100)
      {
        ln2();                         /*    create new descriptor _digLN2   */
      }
      else {
        strncpy(lnv,lnc,_prec10+9);    /*           copy ln2 to piv          */
        piv[_prec10+10] = '\0';        /*       cut ln2 to needed length     */
        TEST(_digLN2 = sts_dec(lnv,0));/*  create new descriptor _digLN2     */
      }
      if (inittrigdesc) {              /*         update PI necessary        */
        if (_prec10 > 100)
        {
           pi();                       /* PI-constant in PIstring too short  */
        }                              /*       create decriptor _digPI      */
        else {
          FREE_STDESC(_digPI);         /*      dealloc descriptor _digPI     */

          strncpy(piv,pic,_prec10+6);  /*           copy PI to piv           */
          piv[_prec10+7] = '\0';       /*       cut PI to needed length      */
          TEST(_digPI = sts_dec(piv,0)); /*  create new descriptor _digPI    */
        }
      }
      oldprec = _prec10;               /* new constants successfully stored  */
    }
  }

} /*** end of function initexp ***/


/*****************************************************************************/
/*                         sinrec     (sinus-recursion)                      */
/* ------------------------------------------------------------------------- */
/*   function:  is the essential recursive function for evaluation of        */
/*              (sin(x) / x) up to relative error _digRG;                    */
/* parameters:  s      - recursiondepth                                      */
/*              prod   - product in modified expression                      */
/*              powx   - increasing power of argument                        */
/*              fac    - factorial                                           */
/*              result - intermedium result                                  */
/*              quot;    evaluated expression per step                       */
/*    returns:  pointer to descriptor of sin(x)   (not sin(x)/x !)           */
/*  called by:  sinfkt, sinrec                                               */
/*****************************************************************************/
PTR_DESCRIPTOR sinrec(s,prod,powx,fac,result,quot)
register PTR_DESCRIPTOR s,                  /* recursiondepth                 */
                        prod;               /* product in modified expression */
PTR_DESCRIPTOR          powx,               /* increasing power of argument   */
                        fac;                /* factorial                      */
register PTR_DESCRIPTOR result,             /* intermedium result             */
                        quot;               /* evaluated expression per step  */
{
  PTR_DESCRIPTOR hlp1, hlp2, hlp3, hlp4,    /* help-pointers to free descrip- */
                 hlp5, hlp6, hlp7;          /* tors, created by digit-rout.'s */


  hlp1 = DIV(quot,result);                  /*         quot / result          */
  if (LT(hlp1,_digRG)) {                    /*   relative error  <  _digRG    */
     DEC_REFCNT(hlp1);
     TEST(hlp1 = ADD(result,quot));
     TEST(hlp2 = MUL(argx,hlp1));           /*     argx * (result + quot)     */
     DEC_REFCNT(hlp1);
     DEC_REFCNT(sqrx);                      /*     ---- dealloc  sqrx   ----  */
     DEC_REFCNT(xpo4);                      /*     ---- dealloc  xpo4   ----  */
     DEC_REFCNT(s);                         /*     ---- dealloc  s      ----  */
     DEC_REFCNT(prod);                      /*     ---- dealloc  prod   ----  */
     DEC_REFCNT(powx);                      /*     ---- dealloc  powx   ----  */
     DEC_REFCNT(fac);                       /*     ---- dealloc  fac    ----  */
     DEC_REFCNT(result);                    /*     ---- dealloc  result ----  */
     DEC_REFCNT(quot);                      /*     ---- dealloc  quot   ----  */
     TEST(hlp1 = sub_dig(hlp2,_prec));      /*     cut result to precision    */
     DEC_REFCNT(hlp2);

     return(hlp1);
  }
  else {
     DEC_REFCNT(hlp1);

     /* ---- start evaluation of 1st parameter ---- */
     TEST(hlp1 = ADD(s,_dig4));             /*            (s + 4)             */

     /* ---- start evaluation of 2nd parameter ---- */
     TEST(hlp2 = MUL(_dig8,s));             /*            (8 * s)             */
     TEST(hlp3 = ADD(hlp2,_dig36));         /*           (8*s) + 36           */
     DEC_REFCNT(hlp2);
     TEST(hlp2 = ADD(prod,hlp3));           /*         prod + (8*s+36)        */
     DEC_REFCNT(hlp3);

     /* ---- start evaluation of 3rd parameter ---- */
     TEST(hlp3 = MUL(powx,xpo4));           /*           powx * xpo4          */

     /* ---- start evaluation of 4th parameter ---- */
     TEST(hlp4 = ADD(s,_dig7));             /*            (s + 7)             */
     TEST(hlp5 = ADD(s,_dig6));             /*            (s + 6)             */
     TEST(hlp6 = MUL(hlp5,hlp4));           /*         (s+6) * (s+7)          */
     DEC_REFCNT(hlp4);
     DEC_REFCNT(hlp5);
     TEST(hlp4 = ADD(s,_dig5));             /*            (s + 5)             */
     TEST(hlp5 = MUL(hlp4,hlp6));           /*      (s+5) * ((s+6)*(s+7))     */
     DEC_REFCNT(hlp4);
     DEC_REFCNT(hlp6);
     TEST(hlp4 = ADD(s,_dig4));             /*            (s + 4)             */
     DEC_REFCNT(s);                         /*      ---- dealloc  s ----      */
     TEST(hlp6 = MUL(hlp4,hlp5));           /*   (s+4) * ((s+5)*(s+6)*(s+7))  */
     DEC_REFCNT(hlp4);
     DEC_REFCNT(hlp5);
     TEST(hlp4 = MUL(fac,hlp6));            /* fac * ((s+4)*(s+5)*(s+6)*(s+7))*/
     DEC_REFCNT(hlp6);

     /* ---- start evaluation of 5th parameter ---- */
     TEST(hlp5 = ADD(result,quot));         /*         result + quot          */
     DEC_REFCNT(result);                    /*   ---- dealloc  result ----    */
     DEC_REFCNT(quot);                      /*    ---- dealloc  quot ----     */

     /* ---- start evaluation of 6th parameter ---- */
     TEST(hlp6 = SUBB(prod,sqrx));          /*          prod - sqrx           */
     DEC_REFCNT(prod);                      /*    ---- dealloc  prod ----     */
     TEST(hlp7 = MUL(powx,hlp6));           /*       powx * (prod-sqrx)       */
     DEC_REFCNT(powx);                      /*    ---- dealloc  powx ----     */
     DEC_REFCNT(hlp6);
     TEST(hlp6 = DIV(hlp7,fac));            /*    (powx*(prod-sqrx)) / fac    */
     DEC_REFCNT(hlp7);
     DEC_REFCNT(fac);                       /*     ---- dealloc  fac ----     */


     return(sinrec(hlp1, hlp2, hlp3, hlp4, hlp5, hlp6));
  }
}  /*** end of function sinrec ***/


/*****************************************************************************/
/*                         sinfkt     (sinus-function)                       */
/* ------------------------------------------------------------------------- */
/*   function:  initializes recursive call of sinrec; sin(x) evaluated with  */
/*              help of 'Taylor-series'. Every two terms of series are       */
/*              transformed to one term.                                     */
/* parameters:  arg    - transformed argument in [0,PI/2]                    */
/*    returns:  pointer to descriptor of sin(x)                              */
/*  called by:  red_sin, red_cos, red_tan                                    */
/*****************************************************************************/
PTR_DESCRIPTOR sinfkt(arg)
register PTR_DESCRIPTOR arg;
{
  PTR_DESCRIPTOR hlp3, hlp5, hlp6, hlp7;   /* help-pointers to free descrip- */
                                           /* tors, created by digit-rout.'s */

  if (EQ(arg,_dig0)) {                           /* return '0' when arg = 0  */
      INC_REFCNT(arg);                           /*  save arg from freeing   */
      return(arg);
  }
  else {
     TEST(argx = arg);                          /* argx available in sinrec */
     TEST(sqrx = MUL(arg,arg));                 /* sqrx available in sinrec */
     TEST(xpo4 = MUL(sqrx,sqrx));               /* xpo4 available in sinrec */

     /* ---- start evaluation of 3rd parameter ---- */
     TEST(hlp3 = MUL(xpo4,xpo4));               /*        xpo4 * xpo4       */

     /* ---- start evaluation of 5th parameter ---- */
     TEST(hlp6 = SUBB(_dig6,sqrx));             /*          6 * sqrx        */
     TEST(hlp5 = DIV(hlp6,_dig6));              /*        (6*sqrx) / 6      */
     DEC_REFCNT(hlp6);

     /* ---- start evaluation of 6th parameter ---- */
     TEST(hlp6 = SUBB(_dig42,sqrx));            /*         42 - sqrx        */
     TEST(hlp7 = MUL(xpo4,hlp6));               /*     xpo4 * (42-sqrx)     */
     DEC_REFCNT(hlp6);
     TEST(hlp6 = DIV(hlp7,_dig5040));           /* (xpo4*(42-sqrx)) / 5040  */
     DEC_REFCNT(hlp7);

 /* ---> car INC_REFCNT extracted from functioncall */
     INC_REFCNT(_dig8);
     INC_REFCNT(_dig110);
     INC_REFCNT(_dig39916800);
     return(sinrec(_dig8,_dig110,hlp3,_dig39916800,hlp5,hlp6));
  }
} /*** end of function sinfkt ***/


/*****************************************************************************/
/*                       sintrans   (sinus-transformation)                   */
/* ------------------------------------------------------------------------- */
/*   function:  transforms argument of sinus into Intervall [0,PI/2]         */
/* parameters:  arg    - argument of sinus                                   */
/*              _sign  - pointer to sign of evaluated sin (sinfkt returns    */
/*                       only positive results because of arg.transform.)    */
/*    returns:  pointer to descriptor of argument                            */
/*              (*_sign implicitly returned)                                 */
/*  called by:  red_sin                                                      */
/*****************************************************************************/
STACKELEM sintrans(arg,_sign)
PTR_DESCRIPTOR  arg,
                *_sign;                     /* return sign of function-value */
{
 PTR_DESCRIPTOR hlp1,hlp2;                  /* help-pointers to free descrip-*/
                                            /* tors, created by digit-rout.'s*/

 if (LT(arg,_dig0))                         /*     argument less than 0      */
 {
    TEST(*_sign = _digm1);                  /*    multiply result with -1    */
    L_DIGIT((*arg),sign) = PLUS_SIGN;
 }
 else {
    TEST(*_sign =  _dig1);
 }

 TEST(hlp1 = MUL(_dig2,_digPI));                   /*        2 * PI         */
 TEST(hlp2 = arg);
 TEST(arg = MOD(hlp2,hlp1));                       /* arg = arg modulo 2PI  */
 DEC_REFCNT(hlp1);
 DEC_REFCNT(hlp2);

 if (!(LT(arg,_digPI)))                            /*     arg ge PI         */
 {
    TEST(hlp1 = arg);
    TEST(arg = SUBB(hlp1,_digPI));                 /*     arg = arg - PI    */
    DEC_REFCNT(hlp1);
    *_sign = (EQ(*_sign,_dig1) ?                   /*     _sign lt 0        */
                            _digm1 :
                            _dig1  );
 }

 TEST(hlp1 = DIV(_digPI,_dig2));                   /*         PI / 2        */
 if (!(LT(arg,hlp1)))                              /*     arg ge (PI/2)     */
 {
    TEST(hlp2 = arg);
    TEST(arg = SUBB(_digPI,hlp2));                 /*     arg = PI - arg    */
    DEC_REFCNT(hlp2);
 }
 DEC_REFCNT(hlp1);

 return((STACKELEM) arg);
} /*** end of function sintrans ***/



/******************************************************************************/
/*                    red_sin     (REDUMA sinus-function)                     */
/* -------------------------------------------------------------------------- */
/*   function:                                                                */
/* case C_DIGIT : evaluation of sin(x) with relative error not worse than     */
/*                REDUMA precision (always possible if x in [0,PI/2]; most    */
/*                times possible if x not in [0,PI/2] ).                      */
/*                Evaluation is done with help of argument-transformation     */
/*                (in sintrans) and modified 'Taylor-series'                  */
/*                (in sinfkt/sinrec).                                         */
/* case C_SCALAR: evaluation of 'System' library-function sin(x)              */
/* parameters:  arg    - argument from stack                                  */
/*    returns:  stackelement (pointer to descriptor of sin(x) )               */
/*  called by:  ear                                                           */
/******************************************************************************/
int red_sin(arg)
register STACKELEM arg;
{
 double argst;                          /* to eval. trigonometric function    */
                                        /* included in <math.h>;              */
 PTR_DESCRIPTOR _sign,                  /*       sign of function-value       */
                hlp;                    /* help-pointer to free descriptor,   */
                                        /* created by digit-rout.'s           */
 BOOLEAN save_digit_rec;

 START_MODUL("red_sin");

 if (T_POINTER(arg)) {

  switch(R_DESC(DESC(arg),class)) {
    case C_SCALAR :
      if (R_DESC(DESC(arg),type) == TY_INTEGER) {
        argst = R_SCALAR(DESC(arg),vali);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = sin(argst);
      }
      else {
        argst = R_SCALAR(DESC(arg),valr);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = sin(argst);
      }
      goto success;

    case C_DIGIT :
      save_digit_rec = _digit_recycling;
      _digit_recycling = FALSE;
      inittrig();                       /* create descriptor-constants which  */
                                        /* are needed in trigonom. funtions;  */

      save_prec_mult = _prec_mult;      /* save global REDUMA mult-precision  */
      _prec_mult = BIBPREC;             /* set intern _prec_mult              */

      _redumaprec = _prec;              /*    save global REDUMA precision    */
      _prec = BIBPREC;                  /* set precision for eval. of sin(x)  */

      TEST(arg = sintrans(arg,&_sign)); /*    transform arg into [0,PI/2]     */
      TEST(hlp = sinfkt(arg));
                                               /*-----------------------------*/
      TEST(_desc = MUL(_sign,hlp));            /*      evaluate  "sin(x)"     */
                                               /*-----------------------------*/
      DEC_REFCNT(hlp);


      _prec = _redumaprec;              /*   reset global REDUMA precision    */

      _digit_recycling = save_digit_rec;     /*         restore flag          */
      _prec_mult = save_prec_mult;      /* reset global REDUMA mult-precision */
      goto success;

    default : goto fail;
  } /** end switch **/
 }
 fail :
   END_MODUL("red_sin");
   return(0);
 success :
   DEC_REFCNT((PTR_DESCRIPTOR)arg);
   END_MODUL("red_sin");
   return(1);

} /*** end of function red_sin ***/


/*****************************************************************************/
/*                       costrans   (cosinus-transformation)                 */
/* ------------------------------------------------------------------------- */
/*   function:  transforms argument of cosinus into Intervall [0,PI/2]       */
/* parameters:  arg    - argument of cosinus                                 */
/*              _sign  - pointer to sign of evaluated cos (sinfkt returns    */
/*                       only positive results because of arg.transform.)    */
/*    returns:  pointer to descriptor of argument                            */
/*              (*_sign implicitly returned)                                 */
/*  called by:  red_cos                                                      */
/*****************************************************************************/
STACKELEM costrans(arg,_sign)
PTR_DESCRIPTOR  arg,
                *_sign;                     /* return sign of function-value */
{
 PTR_DESCRIPTOR hlp1,hlp2;                  /* help-pointers to free descrip-*/
                                            /* tors, created by digit-rout.'s*/
 TEST(hlp1 = DIV(_digPI,_dig2));            /*           (PI / 2)            */
 TEST(hlp2 = arg);
 TEST(arg  = ADD(hlp2,hlp1));               /*         arg - (PI / 2)        */
 DEC_REFCNT(hlp1);
 DEC_REFCNT(hlp2);

 if (LT(arg,_dig0))                         /*     argument less than 0      */
 {
    TEST(*_sign = _digm1);                  /*    multiply result with -1    */
    L_DIGIT((*arg),sign) = PLUS_SIGN;
 }
 else {
    TEST(*_sign =  _dig1);
 }

 TEST(hlp1 = MUL(_dig2,_digPI));                    /*        2 * PI         */
 TEST(hlp2 = arg);
 TEST(arg = MOD(hlp2,hlp1));                        /* arg = arg modulo 2PI  */
 DEC_REFCNT(hlp1);
 DEC_REFCNT(hlp2);

 if (!(LT(arg,_digPI)))                            /*     arg ge PI         */
 {
    TEST(hlp1 = arg);
    TEST(arg = SUBB(hlp1,_digPI));                 /*     arg = arg - PI    */
    DEC_REFCNT(hlp1);
    *_sign = (EQ(*_sign,_dig1) ?                   /*     _sign lt 0        */
                            _digm1 :
                            _dig1  );
 }

 TEST(hlp1 = DIV(_digPI,_dig2));                   /*         PI / 2        */
 if (!(LT(arg,hlp1)))                              /*     arg ge (PI/2)     */
 {
    TEST(hlp2 = arg);
    TEST(arg = SUBB(_digPI,hlp2));                 /*     arg = PI - arg    */
    DEC_REFCNT(hlp2);
 }
 DEC_REFCNT(hlp1);

 return((STACKELEM) arg);
} /*** end of function costrans ***/



/******************************************************************************/
/*                    red_cos     (REDUMA cosinus-function)                   */
/* -------------------------------------------------------------------------- */
/*   function:                                                                */
/* case C_DIGIT : evaluation of cos(x) with relative error not worse than     */
/*                REDUMA precision (most times possible).                     */
/*                Evaluation is done with help of argument-transformation     */
/*                (in costrans) and modified 'Taylor-series'                  */
/*                (in sinfkt/sinrec).                                         */
/* case C_SCALAR: evaluation of 'System' library-function cos(x)              */
/* parameters:  arg    - argument from stack                                  */
/*    returns:  stackelement (pointer to descriptor of cos(x) )               */
/*  called by:  ear                                                           */
/******************************************************************************/
int red_cos(arg)
register STACKELEM arg;
{
 double argst;                          /* to eval. trigonometric function    */
                                        /* included in <math.h>;              */
 PTR_DESCRIPTOR _sign,                  /*       sign of function-value       */
                hlp;                    /* help-pointer to free descriptor,   */
                                        /* created by digit-rout.'s           */
 BOOLEAN save_digit_rec;

 START_MODUL("red_cos");

 if (T_POINTER(arg)) {

  switch(R_DESC(DESC(arg),class)) {
    case C_SCALAR :
      if (R_DESC(DESC(arg),type) == TY_INTEGER) {
        argst = R_SCALAR(DESC(arg),vali);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = cos(argst);
      }
      else {
        argst = R_SCALAR(DESC(arg),valr);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = cos(argst);
      }
      goto success;

    case C_DIGIT :
      save_digit_rec = _digit_recycling;
      _digit_recycling = FALSE;
      inittrig();                       /* create descriptor-constants which  */
                                        /* are needed in trigonom. funtions;  */

      save_prec_mult = _prec_mult;      /* save global REDUMA mult-precision  */
      _prec_mult = BIBPREC;             /* set internal used _prec_mult       */

      _redumaprec = _prec;              /*    save global REDUMA precision    */
      _prec = BIBPREC;                  /* set precision for eval. of sin(x)  */

      if (EQ(arg,_dig0)) {              /* cos(0) = 1                         */
       INC_REFCNT(_dig1);
       TEST(_desc = _dig1);
      }
      else {
       TEST(arg = costrans(arg,&_sign)); /*    transform arg into [0,PI/2]    */
       TEST(hlp = sinfkt(arg));
                                               /*-----------------------------*/
       TEST(_desc = MUL(_sign,hlp));           /*      evaluate  "cos(x)"     */
                                               /*-----------------------------*/
       DEC_REFCNT(hlp);
      }

      _prec = _redumaprec;              /*   reset global REDUMA precision    */

      _digit_recycling = save_digit_rec;     /*         restore flag          */
      _prec_mult = save_prec_mult;      /* reset global REDUMA mult-precision */
      goto success;

    default : goto fail;
  } /** end switch **/
 }
 fail :
   END_MODUL("red_cos");
   return(0);
 success :
   END_MODUL("red_cos");
   DEC_REFCNT((PTR_DESCRIPTOR)arg);
   return(1);

} /*** end of function red_cos ***/


/******************************************************************************/
/*                    red_tan     (REDUMA tangens-function)                   */
/* -------------------------------------------------------------------------- */
/*   function:                                                                */
/* case C_DIGIT : evaluation of tan(x) with relative error not worse than     */
/*                REDUMA precision (most times possible).                     */
/*                Evaluation is done with help of argument-transformation     */
/*                (in sintrans and costrans) and modified 'Taylor-series'     */
/*                (in sinfkt/sinrec).                                         */
/*                                               sin(x)                       */
/*                              eval.   tan(x) = ------                       */
/*                                               cos(x)                       */
/*                                                                            */
/* case C_SCALAR: evaluation of 'System' library-function tan(x)              */
/* parameters:  arg    - argument from stack                                  */
/*    returns:  stackelement (pointer to descriptor of tan(x) )               */
/*  called by:  ear                                                           */
/******************************************************************************/
int red_tan(arg)
register STACKELEM arg;
{
 double argst;                          /* to eval. trigonometric function    */
                                        /* included in <math.h>;              */
 STACKELEM cosarg;                      /* transformed argument of cos(x)     */
 PTR_DESCRIPTOR _sign,                  /*       sign of function-value       */
                hlp1,hlp2,hlp3;         /* help-pointer to free descriptors,  */
                                        /* created by digit-rout.'s           */
 BOOLEAN save_digit_rec;

 START_MODUL("red_tan");

 if (T_POINTER(arg)) {

  switch(R_DESC(DESC(arg),class)) {
    case C_SCALAR :
      if (R_DESC(DESC(arg),type) == TY_INTEGER) {
        argst = R_SCALAR(DESC(arg),vali);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = tan(argst);
      }
      else {
        argst = R_SCALAR(DESC(arg),valr);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = tan(argst);
      }
      goto success;

    case C_DIGIT :
      save_digit_rec = _digit_recycling;
      _digit_recycling = FALSE;
      _digit_recycling = FALSE;
      inittrig();                       /* create descriptor-constants which  */
                                        /* are needed in trigonom. funtions;  */

       save_prec_mult = _prec_mult;     /* save global REDUMA mult-precision  */
       _prec_mult = BIBPREC;            /* set internal used _prec_mult       */

      _redumaprec = _prec;              /*    save global REDUMA precision    */

      _prec = BIBPREC;                  /* set precision for eval. of sin(x)  */

      INC_REFCNT((PTR_DESCRIPTOR)arg);        /* arg not freed in costrans    */

      TEST(cosarg = costrans(arg,&_sign));    /* transform arg into [0,PI/2]  */
      TEST(hlp1 = sinfkt(cosarg));
      DEC_REFCNT((PTR_DESCRIPTOR) cosarg); 
      if (hlp1 == 0)
        post_mortem("the result of tan(x)  is undefined");

      TEST(hlp2 = MUL(_sign,hlp1));            /*-----------------------------*/
      DEC_REFCNT(hlp1);                        /*  hlp2  =  "cos(x)"          */
                                               /*-----------------------------*/

      TEST(arg  = sintrans(arg,&_sign));       /* transform arg into [0,PI/2] */
      TEST(hlp1 = sinfkt(arg));
                                               /*-----------------------------*/
      TEST(hlp3 = MUL(_sign,hlp1));            /*  hlp3  =  "sin(x)"          */
      DEC_REFCNT(hlp1);                        /*-----------------------------*/

      TEST(_desc = DIV(hlp3,hlp2));            /*-----------------------------*/
      DEC_REFCNT(hlp3);                        /*  tan(x)  =  sin(x) / cos(x) */
      DEC_REFCNT(hlp2);                        /*-----------------------------*/

      _prec = _redumaprec;                  /* reset global REDUMA precision  */
      _prec_mult = save_prec_mult;      /* reset global REDUMA mult-precision */

      _digit_recycling = save_digit_rec;    /*        restore flag            */
      goto success;

    default : goto fail;

  } /** end switch **/
 }
 fail :
   END_MODUL("red_tan");
   return(0);
 success :
   DEC_REFCNT((PTR_DESCRIPTOR)arg);
   END_MODUL("red_tan");
   return(1);

}  /*** end of function red_tan ***/


/*****************************************************************************/
/*                  lnrec      (logarithms-function-recursion)               */
/* ------------------------------------------------------------------------- */
/* function  :  is the essential recursive function for evaluation of        */
/*              ln(x) up to relative error _digRG;                           */
/* parameter :  power  - increasing power of tranformed argument             */
/*              nen    - denominator                                         */
/*              n      - increasing variable                                 */
/*              result - intermedium result                                  */
/*              quot   - evaluated expression per step                       */
/* returns   :  pointer to descriptor of ln(x)                               */
/* calls     :  lnrec, sub_dig                                               */
/* called by :  lnfkt, lnrec                                                 */
/*****************************************************************************/
PTR_DESCRIPTOR lnrec(power,nen,n,result,quot)
register PTR_DESCRIPTOR power,nen,result,quot;
PTR_DESCRIPTOR n;
{
  PTR_DESCRIPTOR hlp1,hlp2,hlp3,       /* help-pointer to free descriptor,   */
                 hlp4,hlp5;            /* created by digit-rout.'s           */

  TEST(hlp1 = DIV(quot,result));

  if (LT(hlp1,_digRG)) {
    DEC_REFCNT(hlp1);

    DEC_REFCNT(lrsqr);                           /* --- dealloc lrsqr ------ */
    DEC_REFCNT(power);                           /* --- dealloc power ------ */
    DEC_REFCNT(nen);                             /* --- dealloc nen -------- */
    DEC_REFCNT(n);                               /* --- dealloc n ---------- */

    TEST(hlp1 = SUBB(zx,_dig2));                 /* (zx - 2)                 */
    DEC_REFCNT(zx);                              /* ---- dealloc zx -------- */

    TEST(hlp2 = ADD(argx,_dig1));                /* (x + 1)                  */

    TEST(hlp3 = DIV(hlp1,hlp2));                 /* ((zx - 2) / (x + 1))     */
    DEC_REFCNT(hlp1);                            /* ---- dealloc hlp1 ------ */
    DEC_REFCNT(hlp2);                            /* ---- dealloc hlp2 ------ */

    TEST(hlp2 = ADD(result,quot));               /* (result + quot) -------- */
    DEC_REFCNT(result);                          /* ---- dealloc result ---- */
    DEC_REFCNT(quot);                            /* ---- dealloc quot ------ */

    TEST(hlp1 = MUL(hlp3,hlp2));                 /* (((zx - 2) / (x + 1)) *  */
                                                 /*  (result + quot)         */
    DEC_REFCNT(hlp3);                            /* ---- dealloc hlp3 ------ */
    DEC_REFCNT(hlp2);                            /* ---- dealloc hlp2 ------ */

    TEST(hlp2 = sub_dig(hlp1,_prec));            /* cut result to precision            */
    DEC_REFCNT(hlp1);                            /* ---- dealloc hlp1 ------ */

    return(hlp2);
  }
  else {
    DEC_REFCNT(hlp1);

    TEST(hlp1 = MUL(power,lrsqr));               /* (power * lrsqr)          */

    TEST(hlp3 = MUL(_dig2,n));                   /* (2 * n)                  */
    TEST(hlp2 = ADD(hlp3,_dig1));                /* ((2 * n) + 1)            */
    DEC_REFCNT(hlp3);

    TEST(hlp3 = ADD(n,_dig1));                   /* (n + 1)                  */
    DEC_REFCNT(n);                               /* ---- dealloc n --------- */

    TEST(hlp4 = ADD(result,quot));               /* (result + quot)          */
    DEC_REFCNT(result);                          /* ---- dealloc result ---- */
    DEC_REFCNT(quot);                            /* ---- dealloc quot ------ */

    TEST(hlp5 = DIV(power,nen));                 /* (power / nen)            */
    DEC_REFCNT(power);                           /* ---- dealloc power ----- */
    DEC_REFCNT(nen);                             /* ---- dealloc nen - ----- */

    return(lnrec(hlp1, hlp2, hlp3, hlp4, hlp5));
  }
} /*** end of function lnrec ***/


/*****************************************************************************/
/*                   lnfkt      (logarithms-function)                        */
/* ------------------------------------------------------------------------- */
/* function  :  initializes recursive call of lnprec; ln(x) evaluated with   */
/*              help of 'Taylor-series'. Every two terms of series are       */
/*              transformed to one term.                                     */
/* parameter :  arg    - transformed argument                                */
/* returns   :  pointer to descriptor of ln(x)                               */
/* calls     :  lnrec                                                        */
/* called by :  lntrans1, lntrans2, exptrans                                 */
/*****************************************************************************/
PTR_DESCRIPTOR lnfkt(arg)
register PTR_DESCRIPTOR arg;
{
  PTR_DESCRIPTOR hlp1,hlp2,hlp3;       /* help-pointer to free descriptor,   */
                                       /* created by digit-rout.'s           */
  TEST(argx = arg);

  TEST(hlp1 = MUL(arg,arg));                /* (x * x)                       */
  TEST(zx   = MUL(_dig2,arg));              /* (2 * x)                       */

  TEST(hlp2 = ADD(hlp1,_dig1));             /* ((x * x) + 1)                 */

  TEST(hlp3 = SUBB(hlp2,zx));               /* ((x * x) * 1) - (2 * x)       */
  DEC_REFCNT(hlp2);                         /* ------ dealloc hlp2 --------- */

  TEST(hlp2 = ADD(hlp1,zx));                /* ((x * x) + (2 * x)            */
  DEC_REFCNT(hlp1);                         /* ------ dealloc hlp1 --------- */

  TEST(hlp1 = ADD(hlp2,_dig1));             /* (((x * x) + (2 *x)) + 1)      */
  DEC_REFCNT(hlp2);                         /* ------ dealloc hlp2 --------- */

  TEST(lrsqr = DIV(hlp3,hlp1));             /* (((x * x) * 1) - (2 * x) /    */
                                            /*  (((x * x) + (2 *x)) + 1))    */
  DEC_REFCNT(hlp3);                         /* ------ dealloc hlp3 --------- */
  DEC_REFCNT(hlp1);                         /* ------ dealloc hlp1 --------- */

  TEST(hlp1 = MUL(lrsqr,lrsqr));            /* (lrsqr * lrsqr)               */
  TEST(hlp2 = DIV(lrsqr,_dig3));            /* (lrsqr / 3)                   */

  INC_REFCNT(_dig5);
  INC_REFCNT(_dig3);
  INC_REFCNT(_dig1);
  return(lnrec(hlp1,_dig5,_dig3,_dig1,hlp2));

} /*** end of function lnfkt ***/


/*****************************************************************************/
/*                     lntrans1  (logarithm-function-transformation)         */
/* ------------------------------------------------------------------------- */
/* function  :                                                               */
/*               argument < 0.707                                            */
/*               argumenttransformation with use of relation                 */
/*                                                                           */
/*               ln(x) = n * ln(2) + ln(g)                                   */
/*                       with g = x * 2 power +-n                            */
/*                                                                           */
/*               transform argument into  [ 0.707 , 1.414 ]                  */
/*                                                                           */
/* parameter :  arg    - argument of logarithm-function                      */
/*              n      - decreasing index                                    */
/* returns   :  evaluated ln(x)                                              */
/* calls     :  lnfkt                                                        */
/* called by :  red_ln                                                       */
/*****************************************************************************/
PTR_DESCRIPTOR lntrans1(arg,n)
register PTR_DESCRIPTOR arg,n;
{
  PTR_DESCRIPTOR hlp1,hlp2,hlp3;       /* help-pointer to free descriptors,  */
                                       /* created by digit-rout.'s           */
  if (!LT(arg,_dig0p707)) {
    _prec_mult = BIBPREC; 
    _prec = BIBPREC; 
    TEST(hlp2 = MUL(n,_digLN2));                   /* (n * ln(2))            */
    DEC_REFCNT(n);                                 /* --- dealloc n -------- */

    if (EQ(arg,_dig1)) {
      TEST(hlp1 = _dig0);
      INC_REFCNT(hlp1);
    }
    else {
      _prec_mult = BIBPREC; 
      _prec = BIBPREC; 
      TEST(hlp1 = lnfkt(arg));                     /* ln(x)                  */
    }

    DEC_REFCNT(arg);                               /* decrement refcnt of    */
                                                   /* but not freed          */
    TEST(hlp3 = DI_ADD(hlp2,hlp1));                /* (n * ln(2)) + ln(x)    */
    DEC_REFCNT(hlp1);                              /* --- dealloc hlp1 ----- */
    DEC_REFCNT(hlp2);                              /* --- dealloc hlp2 ----- */

    return(hlp3);                                  /* return eval. ln(x)     */
  }
  else {
    TEST(hlp1 = MUL(arg,_dig2));                   /* (x * 2)                */
    DEC_REFCNT(arg);                               /* --- dealloc arg ------ */

    TEST(hlp2 = DI_SUB(n,_dig1));                  /* (n - 1)                */
    DEC_REFCNT(n);                                 /* --- dealloc n -------- */

    return(lntrans1(hlp1, hlp2));                  /* recursive function call*/
  }
} /*** end of function lntrans1 ***/


/*****************************************************************************/
/*                     lntrans2  (logarithm-function-transformation)         */
/* ------------------------------------------------------------------------- */
/* function  :                                                               */
/*               argument > 1.414                                            */
/*               argumenttransformation with use of relation                 */
/*                                                                           */
/*               ln(x) = n * ln(2) + ln(g)                                   */
/*                       with g = x * 2 power +-n                            */
/*                                                                           */
/*               transform argument into  [ 0.707 , 1.414 ]                  */
/*                                                                           */
/* parameter :  arg    - argument of logarithm-function                      */
/*              n      - increasing index                                    */
/* returns   :  evaluated ln(x)                                              */
/* calls     :  lnfkt                                                        */
/* called by :  red_ln                                                       */
/*****************************************************************************/
PTR_DESCRIPTOR lntrans2(arg,n)
register PTR_DESCRIPTOR arg,n;
{
  PTR_DESCRIPTOR hlp1,hlp2,hlp3;       /* help-pointer to free descriptor,   */
                                       /* created by digit-rout.'s           */
  if (LE(arg,_dig1p414)) {
      _prec_mult = BIBPREC; 
      _prec = BIBPREC; 
    TEST(hlp2 = MUL(n,_digLN2));                   /* (n * ln(2))            */
    DEC_REFCNT(n);                                 /* --- dealloc n -------- */

    if (EQ(arg,_dig1)) {
      TEST(hlp1 = _dig0);
      INC_REFCNT(hlp1);
    }
    else {
      _prec_mult = BIBPREC; 
      _prec = BIBPREC; 
      TEST(hlp1 = lnfkt(arg));                     /* ln(x)                  */
    }

    DEC_REFCNT(arg);                               /* decrement refcnt of    */
                                                   /* but not freed          */
    TEST(hlp3 = DI_ADD(hlp2,hlp1));                /* (n * ln(2)) + ln(x)    */
    DEC_REFCNT(hlp1);                              /* --- dealloc hlp1 ----- */
    DEC_REFCNT(hlp2);                              /* --- dealloc hlp2 ----- */

    return(hlp3);                                  /* return eval. ln(x)     */
  }
  else {
    TEST(hlp1 = DIV(arg,_dig2));                   /* (x / 2)                */
    DEC_REFCNT(arg);                               /* --- dealloc arg ------ */

    TEST(hlp2 = ADD(n,_dig1));                     /* (n + 1)                */
    DEC_REFCNT(n);                                 /* --- dealloc n -------- */

    return(lntrans2(hlp1,hlp2));                   /* recursive function call*/
  }

} /*** end of function lntrans2 ***/


/*****************************************************************************/
/*                    red_ln  (REDUMA natural logarithm-function)            */
/* --------------------------------------------------------------------------*/
/* function  :                                                               */
/*             case C_DIGIT :                                                */
/*                   evaluation of ln(x) with relative error not worse than  */
/*                   REDUMA precision (most times possible).                 */
/*                   Evaluation is done with help of argument-transformation */
/*                   (in lntrans1 and lntrans2) and modified 'Taylor-series' */
/*                   (in lnfkt/lnrec).                                       */
/*                                                                           */
/*             case C_SCALAR :                                               */
/*                   evaluation of 'System' library-function ln(x)           */
/*                                                                           */
/* parameter : arg   - argument from stack                                   */
/* returns   : stackelement (pointer to descriptor of ln(x)  )               */
/* calls     : lntrans1, lntrans2, lnfkt                                     */
/* called by : ear                                                           */
/*****************************************************************************/
int red_ln(arg)
register STACKELEM arg;
{
 double argst;                         /* to eval. logarithm                 */
                                       /* function included in <math.h>      */
 BOOLEAN save_digit_rec;

 START_MODUL("red_ln");

 if (T_POINTER(arg)) {

  switch(R_DESC(DESC(arg),class)) {
    case C_SCALAR :
      if (R_DESC(DESC(arg),type) == TY_INTEGER) {
        argst = R_SCALAR(DESC(arg),vali);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = log(argst);
      }
      else {
        argst = R_SCALAR(DESC(arg),valr);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = log(argst);
      }
      goto success;

    case C_DIGIT :
      save_digit_rec = _digit_recycling;
      _digit_recycling = FALSE;

      initexp();                     /* create descriptor-constants which    */
                                     /* are needed to eval. exp(x) and ln(x) */
      if (LE(arg,_dig0))
        post_mortem("the argument of ln(x) must be   gt  0");

      save_prec_mult = _prec_mult;     /* save global REDUMA mult-precision  */
      _prec_mult = PIPREC;             /* set internal used _prec_mult       */

      _redumaprec = _prec;             /* save the global REDUMA reduction-  */
                                       /* parameter precision                */
      _prec = PIPREC;                  /* set precision for eval. of ln(x)   */

      if (EQ(arg,_dig1)) {             /* argument = 1                       */
        INC_REFCNT(_dig0);
        TEST(_desc = _dig0);
      }
      else {
                                       /* arg is in  [ 0.707 , 1.414 ]       */
       if ((LT(arg,_dig0p707)) || (!LE(arg,_dig1p414))) {
         INC_REFCNT((PTR_DESCRIPTOR)arg);/* do not freed in lntrans1 +  --2  */
         INC_REFCNT(_dig0);            /* do not freed in lntrans1 + --2     */
         if (LT(arg,_dig0p707)) {      /* arg < 0.707                        */
                                       /* recursive argument transformation  */
                                       /* and evaluation of ln(x)            */
           TEST(_desc = lntrans1(arg,_dig0));
         }
         else {                        /* arg > 1.414                        */
                                       /* recursive argument transformation  */
                                       /* and evaluation of ln(x)            */
          TEST(_desc = lntrans2(arg,_dig0));
         }
       }
       else {                          /* argument is in [ 0.707 , 1.414 ]   */
         TEST(_desc = lnfkt(arg));     /* evaluate  ln(x)                    */
       }
      }

      _prec = _redumaprec;             /* reset the global REDUMA reduction- */
                                       /* parameter precision                */
      _prec_mult = save_prec_mult;     /* reset global REDUMA mult-precision */
      _digit_recycling = save_digit_rec;  /* restore flag                    */
      goto success;

    default : goto fail;

  } /** end switch **/
 }
 fail :
   END_MODUL("red_ln");
   return(0);
 success :
   DEC_REFCNT((PTR_DESCRIPTOR)arg);
   END_MODUL("red_ln");
   return(1);

} /*** end of function red_ln ***/



/*****************************************************************************/
/*                  exprec     (exponential-function-recursion)              */
/* ------------------------------------------------------------------------- */
/* function  :  is the essential recursive function for evaluation of        */
/*              exp(x) up to relative error _digRG;                          */
/* parameter :  powx   - increasing power of tranformed argument             */
/*              fac    - factorial                                           */
/*              n      - increasing                                          */
/*              result - intermedium result                                  */
/*              quot   - evaluated expression per step                       */
/* returns   :  pointer to descriptor of exp(x)                              */
/* calls     :  exprec, sub_dig                                              */
/* called by :  expfkt, exprec                                               */
/*****************************************************************************/
PTR_DESCRIPTOR exprec(powx,fac,n,result,quot)
register PTR_DESCRIPTOR powx,fac,result,quot;
PTR_DESCRIPTOR n;
{
  PTR_DESCRIPTOR hlp1,hlp2,hlp3,       /* help-pointer to free descriptors,  */
                 hlp4,hlp5;            /* created by digit-rout.'s           */

  hlp1 = DIV(quot,result);                       /* (quot / result)          */

  if (LT(hlp1,_digRG)) {
    DEC_REFCNT(hlp1);                            /* ----- dealloc hlp1 ----- */

    TEST(hlp1 = ADD(result,quot));               /* (result + quot)          */
    DEC_REFCNT(powx);                            /* ----- dealloc powx ----- */
    DEC_REFCNT(fac);                             /* ----- dealloc fac ------ */
    DEC_REFCNT(n);                               /* ----- dealloc n -------- */
    DEC_REFCNT(result);                          /* ----- dealloc result --- */
    DEC_REFCNT(quot);                            /* ----- dealloc qout ----- */

    TEST(hlp2 = sub_dig(hlp1,_prec + 1));        /* cut result to precision  */
    DEC_REFCNT(hlp1);                            /* ----- dealloc hlp1 ----- */

    return(hlp2);                                /* return result of exp(x)  */
  }
  else {
    DEC_REFCNT(hlp1);                            /* ----- dealloc hlp1 ----- */

    TEST(hlp1 = MUL(powx,argx));                 /* (powx * x)               */

    TEST(hlp2 = MUL(fac,n));                     /* (fac * n)                */

    TEST(hlp3 = ADD(n,_dig1));                   /* (n + 1)                  */
    DEC_REFCNT(n);                               /* ----- dealloc n -------- */

    TEST(hlp4 = ADD(result,quot));               /* (result + quot)          */
    DEC_REFCNT(result);                          /* ----- dealloc result --- */
    DEC_REFCNT(quot);                            /* ----- dealloc quot ----- */

    TEST(hlp5 = DIV(powx,fac));                  /* (powx / fac)             */
    DEC_REFCNT(powx);                            /* ----- dealloc powx ----- */
    DEC_REFCNT(fac);                             /* ----- dealloc fac ------ */

    return(exprec(hlp1, hlp2, hlp3, hlp4, hlp5));/* recursive function call  */
  }

} /*** end of function exprec ***/


/*****************************************************************************/
/*                   expfkt     (exponential-function)                       */
/* ------------------------------------------------------------------------- */
/* function  :  initializes recursive call of exprec; exp(x) evaluated with  */
/*              help of 'Taylor-series'. Every two terms of series are       */
/*              transformed to one term.                                     */
/* parameter :  arg    - transformed argument                                */
/* returns   :  pointer to descriptor of exp(x)                              */
/* calls     :  exprec                                                       */
/* called by :  exptrans                                                     */
/*****************************************************************************/
PTR_DESCRIPTOR expfkt(arg)
register PTR_DESCRIPTOR arg;
{
  register PTR_DESCRIPTOR powx;

  TEST(argx = arg);

  TEST(powx = MUL(arg,arg));                  /* (x * x)                     */

 /* ---> car INC_REFCNT extracted from functioncall */
  INC_REFCNT(_dig2);
  INC_REFCNT(_dig3);
  INC_REFCNT(_dig1);
  INC_REFCNT(arg);
  return(exprec(powx,_dig2,_dig3,_dig1,arg));

} /*** end of function expfkt ***/


/*****************************************************************************/
/*                  pow2     (2 power x  function)                           */
/* ------------------------------------------------------------------------- */
/* function  :  is the essential recursive function for evaluation of        */
/*              2 power n                                                    */
/* parameter :  result -  (must be called with _dig1)                        */
/*              n      -  decreasing index (must be called with n > _dig0)   */
/* returns   :  pointer to descriptor of 2 power n                           */
/* called by :  exptrans                                                     */
/*****************************************************************************/
PTR_DESCRIPTOR pow2(result,n)
register PTR_DESCRIPTOR  result,n;
{
  PTR_DESCRIPTOR  hlp1,hlp2;           /* help-pointer to free descriptors,  */
                                       /* created by digit-rout.'s           */
  while (!EQ(n,_dig0)) {
    TEST(hlp1 = MUL(result,_dig2));            /* (result * 2)               */
    DEC_REFCNT(result);                        /* ---- dealloc result ------ */
    TEST(result = hlp1);

    TEST(hlp2 = SUBB(n,_dig1));                /* (n - 1)                    */
    DEC_REFCNT(n);                             /* ---- dealloc n ----------- */
    TEST(n = hlp2);
  }
  DEC_REFCNT(n);                               /* ---- dealloc n ----------- */

  return(result);                              /* return result of 2 power n */

} /*** end of function pow2 ***/


/*****************************************************************************/
/*                       exptrans   (exponential-function-transformation)    */
/* ------------------------------------------------------------------------- */
/* function  :  transforms argument with use of relation                     */
/*                       (e power ln(2)) power n=2 power n                   */
/*                       x = n * ln(2) + g                                   */
/*                       exp(x) = exp(g) * (2 power n)                       */
/*                                                                           */
/*                       transform argument into  [ 0 , ln(2) ]              */
/*                                                                           */
/*                       [ exp(~x) = (1/exp(x))  transformed in red_exp ]    */
/* parameter :  arg    - argument of exponential-function                    */
/* returns   :  pointer to descriptor of argument                            */
/* calls     :  lnfkt, pow2, expfkt                                          */
/* called by :  red_exp                                                      */
/*****************************************************************************/
PTR_DESCRIPTOR exptrans(arg)
register PTR_DESCRIPTOR arg;
{
  PTR_DESCRIPTOR y,m;
  register PTR_DESCRIPTOR hlp1,hlp2,   /* help-pointer to free descriptors,  */
                          hlp3;        /* created by digit-rout.'s           */

  TEST(hlp1 = DIV(arg,_digLN2));                   /* (x / y)                */
  TEST(m    = TRUNC(hlp1));                        /* m = trunc(arg / y)     */
  DEC_REFCNT(hlp1);                                /* --- dealloc hlp1 ----- */

  TEST(hlp1 = MUL(m,_digLN2));                     /* (m * y)                */

  TEST(hlp2 = SUBB(arg,hlp1));                     /* g =  (x - (m * y))     */
  DEC_REFCNT(hlp1);                                /* --- dealloc hlp1 ----- */

  _prec_mult = BIBPREC;
  _prec = BIBPREC;
  TEST(hlp1 = expfkt(hlp2));                       /* exp(g)                 */
  DEC_REFCNT(hlp2);                                /* ---- dealloc hlp2 ---- */

  if (EQ(m,_dig0)) {
    TEST(hlp2 = _dig1);                            /* <==> 2 power 0  = 1    */
    INC_REFCNT(hlp2);
    DEC_REFCNT(m);                                 /* --- dealloc m -------- */
  }
  else {
 /* ---> car INC_REFCNT extracted from functioncall */
    INC_REFCNT(_dig1);
    TEST(hlp2 = pow2(_dig1,m)); /* pow2(_dig1,m)         */
  }

  TEST(hlp3 = MUL(hlp2,hlp1));                     /* pow2(m)  * exp(g)      */
  DEC_REFCNT(hlp1);                                /* --- dealloc hlp1 ----- */
  DEC_REFCNT(hlp2);                                /* --- dealloc hlp2 ----- */

  return(hlp3);

} /*** end of function exptrans ***/


/*****************************************************************************/
/*                    red_exp     (REDUMA exponential-function)              */
/* --------------------------------------------------------------------------*/
/* function  :                                                               */
/*             case C_DIGIT :                                                */
/*                   evaluation of exp(x) with relative error not worse than */
/*                   REDUMA precision (most times possible).                 */
/*                   Evaluation is done with help of argument-transformation */
/*                   (in exptrans and costrans) and modified 'Taylor-series' */
/*                   (in expfkt/exprec).                                     */
/*                                                                           */
/*             case C_SCALAR :                                               */
/*                   evaluation of 'System' library-function exp(x)          */
/*                                                                           */
/* parameter : arg    - argument from stack                                  */
/* returns   : stackelement (pointer to descriptor of exp(x) )               */
/* calls     : exptrans                                                                            */
/* called by : ear                                                           */
/*****************************************************************************/
int red_exp(arg)
register STACKELEM arg;
{
 double argst;                           /* to eval. exponential function    */
                                         /* included in <math.h>             */
 PTR_DESCRIPTOR hlp1,hlp2;               /* help-pointer to free descriptors,*/
                                         /* created by digit-routines        */
 BOOLEAN save_digit_rec;

 START_MODUL("red_exp");

 if (T_POINTER(arg)) {

  switch(R_DESC(DESC(arg),class)) {
    case C_SCALAR :
      if (R_DESC(DESC(arg),type) == TY_INTEGER) {
        argst = R_SCALAR(DESC(arg),vali);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = exp(argst);
      }
      else {
        argst = R_SCALAR(DESC(arg),valr);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = exp(argst);
      }
      goto success;

    case C_DIGIT :
      save_digit_rec = _digit_recycling;
      _digit_recycling = FALSE;

      initexp();                     /* create descriptor-constants which    */
                                     /* are needed to eval. exp(x) and ln(x) */
      save_prec_mult = _prec_mult;    /* save global REDUMA mult-precision   */
      _prec_mult = PIPREC;            /* set internal use of _prec_mult      */

      _redumaprec = _prec;            /* save the global REDUMA reduction-   */
                                      /* parameter precision                 */
      _prec = PIPREC;                 /* set the precision for eval.of exp(x)*/

      if (EQ(arg,_dig0)) {            /* exp(0) = 1                          */
       INC_REFCNT(_dig1);
       TEST(_desc = _dig1);
      }
      else {
        if (LT(arg,_dig0)) {          /* transform argument                  */
                                      /* exp(~x) = 1 / exp(x)                */
          TEST(hlp1 = ABS(arg));      /* abs(x)                              */

          TEST(hlp2 = exptrans(hlp1));/* transform arg  and  eval. exp(x)    */
          DEC_REFCNT(hlp1);

          TEST(_desc = DIV(_dig1,hlp2));/* 1 / exp(x)                        */
          DEC_REFCNT(hlp2);
        }
        else {
          TEST(_desc = exptrans(arg)); /* transform arg  and  eval. exp(x)   */
        }
      }
      _prec = _redumaprec;            /* reset the global REDUMA reduction-  */
                                      /* parameter precision                 */
      _prec_mult = save_prec_mult;    /* reset global REDUMA mult-precision  */

      _digit_recycling = save_digit_rec;  /* restore flag                    */
      goto success;

    default : goto fail;

  } /** end switch **/
 }
 fail :
   END_MODUL("red_exp");
   return(0);
 success :
   DEC_REFCNT((PTR_DESCRIPTOR)arg);
   END_MODUL("red_exp");
   return(1);

} /*** end of function red_exp ***/

/*************************   end file of rbibfunc.c   *************************/


