/*****************************************************************************/
/* all decrements of the refcounts for the arguments removed                 */
/* original was rbibfunc.c, difference is that here won't be any             */
/* reference counting [sf 080595]                                            */
/******************************************************************************/
/*  MODUL OF ELEMENTARY FUNCTIONS IMPLEMENTED FOR PROCESSING PHASE            */

/* copied from lneu by RS in June 1993                                        */

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
/*                          expfkt, exprec, exptrans, power2                    */
/* -------------------------------------------------------------------------- */
/*       Note: We have to take care  that proper freeing  of descriptors  is  */
/*       ===== done  when we use  digit-routines.  This requires intermedium  */
/*             storing of any result of digit-routines  which return pointer  */
/*             to descriptors.                                                */
/******************************************************************************/

#include "rstdinc.h"
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rextern.h"
#include "rstelem.h"
#include "rmeasure.h"
#include <string.h>                 /* to store length of string PI           */
#include <math.h>                   /* to eval. PIPREC and BIBPREC            */
                                    /* sin(x),cos(x),tan(x),log(x),exp(x)     */
#include <setjmp.h>

#if D_MESS
#include "d_mess_io.h"
#endif

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

/* inserted by [sf 080595] */
extern PTR_DESCRIPTOR sub_dig();
extern void pirec();
extern void pi();
extern void trigdesc();
extern void rgdesc();
extern void expdesc();
extern void inittrig();
extern void ln2();
extern PTR_DESCRIPTOR lnfkt();
extern void initexp();
extern PTR_DESCRIPTOR sinrec();
extern PTR_DESCRIPTOR sinfkt();
extern STACKELEM sintrans();
extern STACKELEM costrans();
extern PTR_DESCRIPTOR lnrec();
extern PTR_DESCRIPTOR lntrans1();
extern PTR_DESCRIPTOR lntrans2();
extern PTR_DESCRIPTOR exprec();
extern PTR_DESCRIPTOR expfkt();
extern PTR_DESCRIPTOR power2();
extern PTR_DESCRIPTOR exptrans();
/* END inserted by [sf 080595] */

extern DescDump();
extern res_heap();
extern rel_heap();
extern test_dec_refcnt();
extern test_inc_refcnt();

#include "dbug.h"

/*----------------------------------------------------------------------------*/
/*                           global variables                                 */
/*----------------------------------------------------------------------------*/
extern BOOLEAN inittrigdesc;                  /* flag for trigdesc()                 */
                                       /* (if called once then TRUE)          */
extern BOOLEAN initexpdesc;                   /* flag for expdesc()                  */
                                       /* (if called once then TRUE)          */

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
/*                           static variables                                 */
/*----------------------------------------------------------------------------*/
static int     _redumaprec;            /* REDUMA-precision has to saved       */
static int     save_prec_mult;         /* REDUMA-mult-prec has to saved       */
extern int     oldprec;                /* if user defines new precision       */
                                       /* this can be determined by           */
                                       /* comparing with oldprec              */

extern PTR_DESCRIPTOR argx;            /* transformed arg in sinfkt/sinrec    */
                                       /* eval. of red_ln    in lnfkt/lnrec   */
                                       /* eval. of exp(x)    in expfkt/exprec */
extern PTR_DESCRIPTOR xpo4;            /* 4th power of arg   in sinfkt/sinrec */
extern PTR_DESCRIPTOR sqrx;            /* square of arg      in sinfkt/sinrec */

extern PTR_DESCRIPTOR asqr4;           /* eval. of 'PI'      in pi/pirec      */
extern PTR_DESCRIPTOR bsqr4;           /* eval. of 'PI'      in pi/pirec      */
extern PTR_DESCRIPTOR csqr4;           /* eval. of 'PI'      in pi/pirec      */

extern PTR_DESCRIPTOR zx;              /* eval. of red_ln    in lnfkt/lnrec   */
extern PTR_DESCRIPTOR lrsqr;           /* eval. of red_ln    in lnfkt/lnrec   */

/* -------------------------------------  PRE-DEFINED DESCRIPTORS ----------- */
static PTR_DESCRIPTOR _dig0p707 ,               /* 0p707  <==> 0.707 */
                      _dig1p414 ;               /* 1p414  <==> 1.414 */
extern PTR_DESCRIPTOR _digm1 ,               /* <==>  _dig-1      */
                      _dig2 ,
                      _dig3 ,
                      _dig3pi ,
                      _dig4 ,
                      _dig5 ,
                      _dig6 ,
                      _dig7 ,
                      _dig8 ,
                      _dig9 ,
                      _dig11 ,
                      _dig12 ,
                      _dig13 ,
                      _dig15 ,
                      _dig18 ,
                      _dig35 ,
                      _dig57 ,
                      _dig36 ,
                      _dig42 ,
                      _dig99 ,
                      _dig110 ,
                      _dig239 ,
                      _dig5040 ,
                      _dig39916800 ,
                      _dig18p3 ,                /*      18 power  3 */
                      _dig57p3 ,                /*      57 power  3 */
                      _dig239p3 ,                /*     239 power  3 */
                      _dig18p4 ,                /*      18 power  4 */
                      _dig57p4 ,                /*      57 power  4 */
                      _dig239p4 ,                /*     239 power  4 */
                      _dig18p5 ,                /*      18 power  5 */
                      _dig57p5 ,                /*      57 power  5 */
                      _dig239p5 ,                /*     239 power  5 */
                      _dig18p7 ,                /*      18 power  7 */
                      _dig57p7 ,                /*      57 power  7 */
                      _dig239p7 ,                /*     239 power  7 */
                      _dig18p9 ,                /*      18 power  9 */
                      _dig57p9 ,                /*      57 power  9 */
                      _dig239p9 ,                /*     239 power  9 */
                      _dig18p11 ,                /*      18 power 11 */
                      _dig57p11 ,                /*      57 power 11 */
                      _dig239p11 ,                /*     239 power 11 */
                      _dig18p13 ,                /*      18 power 13 */
                      _dig57p13 ,                /*      57 power 13 */
                      _dig239p13 ,                /*     239 power 13 */
                      _dig18p15 ,                /*      18 power 15 */
                      _dig57p15 ,                /*      57 power 15 */
                      _dig239p15 ,                /*     239 power 15 */
                      _digRGPI ;                /* relative error   */
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

int nr_red_sin(arg)
register STACKELEM arg;
{
 double argst;                          /* to eval. trigonometric function    */
                                        /* included in <math.h>;              */
 PTR_DESCRIPTOR _sign,                  /*       sign of function-value       */
                hlp;                    /* help-pointer to free descriptor,   */
                                        /* created by digit-rout.'s           */
 BOOLEAN save_digit_rec;

  DBUG_ENTER ("nr_red_sin");

 START_MODUL("nr_red_sin");

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
/* new for lred by RS in June 1993 */
 else if (T_INT(arg)) {
        argst = (double) VAL_INT(arg);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = sin(argst);
        goto success2;
  }
 fail :
   END_MODUL("nr_red_sin");
   DBUG_RETURN(0);
 success :
 success2 :   /* new for lred by RS in June 1993 */
   END_MODUL("nr_red_sin");
   DBUG_RETURN(1);
}


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

int nr_red_cos(arg)
register STACKELEM arg;
{
 double argst;                          /* to eval. trigonometric function    */
                                        /* included in <math.h>;              */
 PTR_DESCRIPTOR _sign,                  /*       sign of function-value       */
                hlp;                    /* help-pointer to free descriptor,   */
                                        /* created by digit-rout.'s           */
 BOOLEAN save_digit_rec;

  DBUG_ENTER ("nr_red_cos");

 START_MODUL("nr_red_cos");

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
 /* new for lred by RS in June 1993 */
 else if (T_INT(arg)) {
        argst = (double) VAL_INT(arg);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = cos(argst);
       goto success2;
  }
 fail :
   END_MODUL("nr_red_cos");
   DBUG_RETURN(0);
 success :
 success2 :  /* new for lred by RS in June 1993 */
   END_MODUL("nr_red_cos");
   DBUG_RETURN(1);
}

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

int nr_red_tan(arg)
register STACKELEM arg;
{
 double argst;                          /* to eval. trigonometric function    */
                                        /* included in <math.h>;              */
 STACKELEM cosarg;                      /* transformed argument of cos(x)     */
 PTR_DESCRIPTOR _sign,                  /*       sign of function-value       */
                hlp1,hlp2,hlp3;         /* help-pointer to free descriptors,  */
                                        /* created by digit-rout.'s           */
 BOOLEAN save_digit_rec;

  DBUG_ENTER ("nr_red_tan");

  START_MODUL("nr_red_tan");

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
/* new for lred by RS in June 1993 */
 else if (T_INT(arg)) {
        argst = (double) VAL_INT(arg);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = tan(argst);
        goto success2;
   }
 fail :
   END_MODUL("nr_red_tan");
   DBUG_RETURN(0);
 success :
 success2 :  /* new for lred by RS in June 1993 */
   END_MODUL("nr_red_tan");
   DBUG_RETURN(1);
}

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

int nr_red_ln(arg)
register STACKELEM arg;
{
 double argst;                         /* to eval. logarithm                 */
                                       /* function included in <math.h>      */
 BOOLEAN save_digit_rec;

  DBUG_ENTER ("nr_red_ln");


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
 /* new for lred by RS in June 1993 */
 else if (T_INT(arg)) {
        argst = (double) VAL_INT(arg);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = log(argst);
        goto success2;
   }
 fail :
   END_MODUL("nr_red_ln");
   DBUG_RETURN(0);
 success :
 success2 :    /* new for lred by RS in June 1993 */
   END_MODUL("nr_red_ln");
   DBUG_RETURN(1);
}

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
/* calls     : exptrans                                                      */
/* called by : ear                                                           */
/*****************************************************************************/

int nr_red_exp(arg)
register STACKELEM arg;
{
 double argst;                           /* to eval. exponential function    */
                                         /* included in <math.h>             */
 PTR_DESCRIPTOR hlp1,hlp2;               /* help-pointer to free descriptors,*/
                                         /* created by digit-routines        */
 BOOLEAN save_digit_rec;

  DBUG_ENTER ("nr_red_exp");
 START_MODUL("nr_red_exp");

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
 /* new for lred by RS in June 1993 */
 else if (T_INT(arg)) {
        argst = (double) VAL_INT(arg);
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
        L_SCALAR((*_desc),valr) = exp(argst);
        goto success2;
  }
 fail :
   END_MODUL("nr_red_exp");
   DBUG_RETURN(0);
 success :
 success2 : /* new for lred by RS in June 1993 */
   END_MODUL("nr_red_exp");
   DBUG_RETURN(1);
}

