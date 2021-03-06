/* $Log: rval-int.c,v $
 * Revision 1.4  1995/05/22  11:30:18  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:51:19  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*****************************************************************************/
/*                      MODULE FOR PROCESSING PHASE                          */
/*-------------------------------------------------------------------------- */
/* rval-int:c - externals: scal_mvt_op, digit_mvt_op, mvt_mvt_op, mvt_op     */
/*                         (called by routines in rvalfunc:c)                */
/*            - internals: none                                              */
/*****************************************************************************/

#include "rstdinc.h"
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rops.h"         /* contains operation-codes for dyad./monad. ops. */
#include "rmeasure.h"
#include <setjmp.h>

#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

/*****************************************************************************/
/*                          extern variables                                 */
/*****************************************************************************/
extern jmp_buf _interruptbuf;    /* <r>, jump-adress for heapoverflow       */
extern PTR_DESCRIPTOR _desc;     /* <w>, resultdescriptor                   */
extern BOOLEAN _digit_recycling; /* <w>, flag for digit-routines            */
extern PTR_DESCRIPTOR _dig0;     /* <r>, permanent descriptor for value 0   */

/*****************************************************************************/
/*                          extern functions                                 */
/*****************************************************************************/
extern PTR_DESCRIPTOR newdesc();         /* rheap:c */
extern int newheap();                    /* rheap:c */
extern void failure();                   /* rquery:c */
extern PTR_DESCRIPTOR  digit_add();
extern PTR_DESCRIPTOR  digit_sub();
extern PTR_DESCRIPTOR  digit_div();
extern PTR_DESCRIPTOR  digit_mul();
extern PTR_DESCRIPTOR  digit_mod();
extern PTR_DESCRIPTOR  digit_abs();      /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_neg();      /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_trunc();    /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_frac();     /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_floor();    /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_ceil();     /* rdig-v2:c */
extern int             conv_digit_int(); /* rdig-v1:c */
extern BOOLEAN         digit_eq();       /* rdig-v1:c */

#if DEBUG
extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

/*****************************************************************************/
/*                                defines                                    */
/*****************************************************************************/
/* ---- macros for heapoverflow-handling ---- */
#if (D_SLAVE && D_MESS && D_MHEAP)
#define NEWHEAP(size,adr) if ((*d_m_newheap)(size,adr) == 0) longjmp(_interruptbuf,0)
#else   
#define NEWHEAP(size,adr) if (newheap(size,adr) == 0) longjmp(_interruptbuf,0)
#endif
#define INIT_DESC         if ((desc = _desc) == NULL) longjmp(_interruptbuf,0)
#define TEST(desc)        if ((desc) == NULL)         longjmp(_interruptbuf,0)

#define DESC(x)  (* (PTR_DESCRIPTOR) x)

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

/* --- NOTE: dyadic and monadic operation-codes now included from rops:h --- */

/*****************************************************************************/
/*           scal_mvt_op   (scalar, matrix/vector/tr.vector operation)       */
/* ------------------------------------------------------------------------- */
/* function  :  do operation with scalar (of type real or integer) and       */
/*              matrix / vector / transposed vector.                         */
/* parameter :  arg1  - descriptor of class scalar                           */
/*              arg2  - descriptor of class matrix, vector or transp. vector */
/*              op    - dyadic operation                                     */
/* returns   :  pointer to descriptor of result of operation                 */
/* calls     :                                                               */
/* called by :                                                               */
/*****************************************************************************/
PTR_DESCRIPTOR scal_mvt_op(arg1,arg2,op)
PTR_DESCRIPTOR arg1,arg2;
int op;
{
  register PTR_DESCRIPTOR desc;
  register char class,                        /*        class of arg2        */
                type;                         /*         type of arg2        */
  register int count,dim=0; /* Initialisierung von TB, 6.11.1992 */
  int i,*ip,*ipnew;                           /*    (new) integer-pointer    */
  double r, *rp,*rpnew;                       /*      (new) real-pointer     */
  int    m;                                   /* only for modulo function    */
  double help;                                /* truncation of variable help */

  DBUG_ENTER ("scal_mvt_op");

  class = R_DESC((*arg2),class);
  type  = R_DESC((*arg2),type);
  NEWDESC(_desc); INIT_DESC;
  MVT_DESC_MASK(desc,1,class,type);
  switch(class)
  {
  /*~~~~~~~~~~~~~~*/
    case C_MATRIX:
  /*~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = dim = R_MVT((*arg2),nrows,class);
        L_MVT((*desc),ncols,class) = i   = R_MVT((*arg2),ncols,class);
        dim *= i;
        break;
  /*~~~~~~~~~~~~~~*/
    case C_VECTOR:
  /*~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = 1;
        L_MVT((*desc),ncols,class) = dim = R_MVT((*arg2),ncols,class);
        break;
  /*~~~~~~~~~~~~~~~*/
    case C_TVECTOR:
  /*~~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = dim = R_MVT((*arg2),nrows,class);
        L_MVT((*desc),ncols,class) = 1;
        break;
  }

  if (R_DESC((*arg1),type) == TY_INTEGER)
  {
    switch(type)
    {
    /*----------------*/
      case TY_INTEGER:
    /*----------------*/
          if ((op == OP_DIV) || (op == OP_DIVI))
          {
            L_DESC((*desc),type) = TY_REAL;
            NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
            RES_HEAP;
            rpnew = (double *) R_MVT((*desc),ptdv,class);
            ip    = (int *) R_MVT((*arg2),ptdv,class);
            r     = (double) R_SCALAR((*arg1),vali);

            if (op == OP_DIV)
            {
              for (count = 0; count < dim; ++count)
              rpnew[count] = r / ((double)ip[count]);
              REL_HEAP; DBUG_RETURN(desc);
            }
            for (count = 0; count < dim; ++count)
            rpnew[count] = ((double)ip[count]) / r;
            REL_HEAP; DBUG_RETURN(desc);
          }

          NEWHEAP(dim,A_MVT((*desc),ptdv,class));
          RES_HEAP;
          ipnew = (int *) R_MVT((*desc),ptdv,class);
          ip    = (int *) R_MVT((*arg2),ptdv,class);
          i     = R_SCALAR((*arg1),vali);

          switch(op)
          {
            case OP_ADD:
                 for (count = 0; count < dim; ++count)
                 ipnew[count] = i + ip[count];
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_MULT:
                 for (count = 0; count < dim; ++count)
                 ipnew[count] = i * ip[count];
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_SUB:
                 for (count = 0; count < dim; ++count)
                 ipnew[count] = i - ip[count];
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_SUBI:
                 for (count = 0; count < dim; ++count)
                 ipnew[count] = ip[count] - i;
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_MOD:
                 for (count = 0; count < dim; ++count)
                 ipnew[count] = i % ip[count];
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_MODI:
                 for (count = 0; count < dim; ++count)
                 ipnew[count] = ip[count] % i;
                 REL_HEAP; DBUG_RETURN(desc);
            default:
                 post_mortem("scal_mvt_op: Unknown operation");
          } /* end of case of operations */
    /*-------------*/
      case TY_REAL:
    /*-------------*/
          NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
          RES_HEAP;
          rpnew = (double *) R_MVT((*desc),ptdv,class);
          rp    = (double *) R_MVT((*arg2),ptdv,class);
          i     = R_SCALAR((*arg1),vali);

          switch(op)
          {
            case OP_ADD:
                 for (count = 0; count < dim; ++count)
                 rpnew[count] = i + rp[count];
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_MULT:
                 for (count = 0; count < dim; ++count)
                 rpnew[count] = i * rp[count];
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_SUB:
                 for (count = 0; count < dim; ++count)
                 rpnew[count] = i - rp[count];
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_SUBI:
                 for (count = 0; count < dim; ++count)
                 rpnew[count] = rp[count] - i;
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_DIV:
                 for (count = 0; count < dim; ++count)
                 rpnew[count] = i / rp[count];
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_DIVI:
                 for (count = 0; count < dim; ++count)
                 rpnew[count] = rp[count] / i;
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_MOD:
                 for (count = 0 ; count < dim; ++count) {
                    help = (double)i / rp[count];
                    m = help;
                    help = (double)i - m * rp[count];
                    rpnew[count] = help;
                 }
                 REL_HEAP; DBUG_RETURN(desc);
            case OP_MODI:
                 for (count = 0 ; count < dim; ++count) {
                    help = rp[count] / (double)i;
                    m = help;
                    help = rp[count] - m * (double)i;
                    rpnew[count] = help;
                 }
                 REL_HEAP; DBUG_RETURN(desc);
            default:
                 post_mortem("scal_mvt_op: Unknown operation");
          } /* end of case of operatins */
      default:
          DEC_REFCNT(desc);
          DBUG_RETURN((PTR_DESCRIPTOR)0);
    } /* end of case of type of arg2 */
  }
  else
  {
    switch(type)
    {
      case TY_INTEGER:
           L_DESC((*desc),type) = TY_REAL;
           NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
           RES_HEAP;
           rpnew = (double *) R_MVT((*desc),ptdv,class);
           ip    = (int *) R_MVT((*arg2),ptdv,class);
            r     = R_SCALAR((*arg1),valr);

           switch(op)
           {
             case OP_ADD:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = r + ip[count];
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_MULT:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = r * ip[count];
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_SUB:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = r - ip[count];
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_SUBI:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = ip[count] - r;
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_DIV:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = r / ip[count];
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_DIVI:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = ip[count] / r;
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_MOD:
                  for (count = 0 ; count < dim; ++count) {
                     help = r / ((double)ip[count]);
                     m = help;
                     help = r - m * ((double)ip[count]);
                     rpnew[count] = help;
                  }
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_MODI:
                  for (count = 0 ; count < dim; ++count) {
                     help = ((double)ip[count]) / r;
                     m = help;
                     help = ((double)ip[count]) - m * r;
                     rpnew[count] = help;
                  }
                  REL_HEAP; DBUG_RETURN(desc);
             default:
                  post_mortem("scal_mvt_op: Unknown operation");
           } /* end of case of operations */
      case TY_REAL:
           NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
           RES_HEAP;
           rpnew = (double *) R_MVT((*desc),ptdv,class);
           rp    = (double *) R_MVT((*arg2),ptdv,class);
           r     = R_SCALAR((*arg1),valr);

           switch(op)
           {
             case OP_ADD:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = r + rp[count];
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_MULT:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = r * rp[count];
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_SUB:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = r - rp[count];
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_SUBI:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = rp[count] - r;
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_DIV:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = r / rp[count];
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_DIVI:
                  for (count = 0; count < dim; ++count)
                  rpnew[count] = rp[count] / r;
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_MOD:
                  for (count = 0 ; count < dim; ++count) {
                     help = r / rp[count];
                     m = help;
                     help = r - m * rp[count];
                     rpnew[count] = help;
                  }
                  REL_HEAP; DBUG_RETURN(desc);
             case OP_MODI:
                  for (count = 0 ; count < dim; ++count) {
                     help = rp[count] / r;
                     m = help;
                     help = rp[count] - m * r;
                     rpnew[count] = help;
                  }
                  REL_HEAP; DBUG_RETURN(desc);
             default:
                  post_mortem("scal_mvt_op: Unknown operation");
           } /* end of case of operations */
      default:
           DEC_REFCNT(desc);
           DBUG_RETURN((PTR_DESCRIPTOR) 0);
    } /* end of case of type of arg2 */
  }
}

/*****************************************************************************/
/*          digit_mvt_op    (digit, matrix/vector/tr.vector operation)       */
/* ------------------------------------------------------------------------- */
/* function  :  do operation with digitstring and matrix / vector /          */
/*              transposed vector.                                           */
/* parameter :  arg1  - descriptor of class digit                            */
/*              arg2  - descriptor of class matrix, vector or transp. vector */
/*              op    - dyadic operation                                     */
/* returns   :  pointer to descriptor of result of operation                 */
/* calls     :                                                               */
/* called by :                                                               */
/* ------------------------------------------------------------------------- */
/*      Note : In contrary to other mvt-operations we must not store the     */
/*             old and new heapaddresses in local variables because digit-   */
/*             operations may cause heapcompaction. The heap-addresses       */
/*             would be invalid afterwards.                                  */
/*****************************************************************************/
 PTR_DESCRIPTOR digit_mvt_op(arg1,arg2,op)
PTR_DESCRIPTOR arg1,arg2;
int op;
{
  register PTR_DESCRIPTOR desc;
  register char class;                        /*        class of arg2        */
  register int count,dim=0; /* Initialisierung von TB, 6.11.1992 */
  int i,element;

  DBUG_ENTER ("digit_mvt_op");

  if (R_DESC((*arg2),type) != TY_DIGIT)
    DBUG_RETURN((PTR_DESCRIPTOR) 0);
  class = R_DESC((*arg2),class);
  NEWDESC(_desc); INIT_DESC;
  MVT_DESC_MASK(desc,1,class,TY_DIGIT);
  switch(class)
  {
  /*~~~~~~~~~~~~~~*/
    case C_MATRIX:
  /*~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = dim = R_MVT((*arg2),nrows,class);
        L_MVT((*desc),ncols,class) = i   = R_MVT((*arg2),ncols,class);
        dim *= i;
        break;
  /*~~~~~~~~~~~~~~*/
    case C_VECTOR:
  /*~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = 1;
        L_MVT((*desc),ncols,class) = dim = R_MVT((*arg2),ncols,class);
        break;
  /*~~~~~~~~~~~~~~~*/
    case C_TVECTOR:
  /*~~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = dim = R_MVT((*arg2),nrows,class);
        L_MVT((*desc),ncols,class) = 1;
        break;
  }
  NEWHEAP(dim,A_MVT((*desc),ptdv,class));
  _digit_recycling = FALSE;

  switch(op)
  {
    case OP_ADD:
         for (count = 0; count < dim; ++count)
         {
           element = R_MVT(DESC(arg2),ptdv,class)[count];
           TEST(L_MVT((*desc),ptdv,class)[count] = (int) DI_ADD(arg1,element));
         }
         _digit_recycling = TRUE;
         DBUG_RETURN(desc);
    case OP_MULT:
         for (count = 0; count < dim; ++count)
           TEST(L_MVT((*desc),ptdv,class)[count] = (int) digit_mul(arg1,R_MVT(DESC(arg2),ptdv,class)[count]));
         _digit_recycling = TRUE;
         DBUG_RETURN(desc);
    case OP_SUB:
         for (count = 0; count < dim; ++count)
         {
           element = R_MVT(DESC(arg2),ptdv,class)[count];
           TEST(L_MVT((*desc),ptdv,class)[count] = (int) DI_SUB(arg1,element));
         }
         _digit_recycling = TRUE;
         DBUG_RETURN(desc);
    case OP_SUBI:
         for (count = 0; count < dim; ++count)
         {
           element = R_MVT(DESC(arg2),ptdv,class)[count];
           TEST(L_MVT((*desc),ptdv,class)[count] = (int) DI_SUB(element,arg1));
         }
         _digit_recycling = TRUE;
         DBUG_RETURN(desc);
    case OP_DIV:
         for (count = 0; count < dim; ++count)
           TEST(L_MVT((*desc),ptdv,class)[count] = (int) digit_div(arg1,R_MVT(DESC(arg2),ptdv,class)[count]));
         _digit_recycling = TRUE;
         DBUG_RETURN(desc);
    case OP_DIVI:
         if (digit_eq(_dig0,arg1)) longjmp(_interruptbuf,0);
         for (count = 0; count < dim; ++count)
           TEST(L_MVT((*desc),ptdv,class)[count] = (int) digit_div(R_MVT(DESC(arg2),ptdv,class)[count],arg1));
         _digit_recycling = TRUE;
         DBUG_RETURN(desc);
    case OP_MOD:
         for (count = 0; count < dim; ++count)
           TEST(L_MVT((*desc),ptdv,class)[count] = (int) digit_mod(arg1,R_MVT(DESC(arg2),ptdv,class)[count]));
         _digit_recycling = TRUE;
         DBUG_RETURN(desc);
    case OP_MODI:
         if (digit_eq(_dig0,arg1))  longjmp(_interruptbuf,0);
         for (count = 0; count < dim; ++count)
           TEST(L_MVT((*desc),ptdv,class)[count] = (int) digit_mod(R_MVT(DESC(arg2),ptdv,class)[count],arg1));
         _digit_recycling = TRUE;
         DBUG_RETURN(desc);
    default: post_mortem("digit_mvt_op: Unknown operation");
  } /* Ende der Fallunterscheidung der Operationen. */

  DBUG_RETURN((PTR_DESCRIPTOR)0); /* TB, 30.10.92 */
}

/*****************************************************************************/
/*          mvt_mvt_op      (dyadic matrix/vector/tr.vector operation)       */
/* ------------------------------------------------------------------------- */
/* function  :  do dyadic operation with matrix / vector / transposed vector */
/* parameter :  arg1  - descriptor of class matrix, vector or transp. vector */
/*              arg2  - descriptor of class matrix, vector or transp. vector */
/*              op    - dyadic operation                                     */
/* returns   :  pointer to descriptor of result of operation                 */
/* calls     :                                                               */
/* called by :                                                               */
/*****************************************************************************/
 PTR_DESCRIPTOR mvt_mvt_op(arg1,arg2,op)
PTR_DESCRIPTOR arg1,arg2;
int op;
{
  register PTR_DESCRIPTOR desc;
  register char class,                        /*        class of arg2        */
                class1,                       /*        class of arg1        */  
                type;                         /*         type of arg2        */
  register int row,col,count,dim=0; /* Initialisierung von TB, 6.11.1992 */
  int element1,element2;
  int  *ip1,*ip2,*ipnew;                      /*    (new) integer-pointer    */
  double *rp1,*rp2,*rpnew;                    /*      (new) real-pointer     */
  int    m;                                   /* only for modulo function    */
  double help;                                /* truncation of variable help */

  DBUG_ENTER ("mvt_mvt_op");

  class  = R_DESC((*arg2),class);
  class1 = R_DESC((*arg1),class); 
  type   = R_DESC((*arg2),type);
  if ( ((row = R_MVT((*arg1),nrows,class1)) != R_MVT((*arg2),nrows,class)) ||
       ((col = R_MVT((*arg1),ncols,class1)) != R_MVT((*arg2),ncols,class)) ||
       ((unsigned char)class != R_DESC((*arg1),class)) )
    DBUG_RETURN((PTR_DESCRIPTOR)0);

  if ((row == 0) && (col == 0)) {     /* nilvect   x  nilvect  ==>  nilvect  */
    INC_REFCNT((PTR_DESCRIPTOR)arg1); /* niltvect  x  niltvect ==>  niltvect */
    DBUG_RETURN(arg1);                     /* nilmat    x  nilmat   ==>  nilmat   */
  }

  NEWDESC(_desc); INIT_DESC;
  MVT_DESC_MASK(desc,1,class,type);

  switch(class)
  {
  /*~~~~~~~~~~~~~~*/
    case C_MATRIX:
  /*~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = row;
        L_MVT((*desc),ncols,class) = col;
        dim = row * col;
        break;
  /*~~~~~~~~~~~~~~*/
    case C_VECTOR:
  /*~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = 1;
        L_MVT((*desc),ncols,class) = dim = col;
        break;
  /*~~~~~~~~~~~~~~~*/
    case C_TVECTOR:
  /*~~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = dim = row;
        L_MVT((*desc),ncols,class) = 1;
        break;
  }

  switch( R_DESC((*arg1),type) )
  {
    case TY_INTEGER:
         switch(type)
         {
           case TY_INTEGER:
                if (op == OP_DIV)
                {
                  L_DESC((*desc),type) = TY_REAL;
                  NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*desc),ptdv,class);
                  ip1 = (int *) R_MVT((*arg1),ptdv,class);
                  ip2 = (int *) R_MVT((*arg2),ptdv,class);

                  for (count = 0; count < dim; ++count)
                    rpnew[count] = ((double) ip1[count]) / ((double)ip2[count]);
                  REL_HEAP; DBUG_RETURN(desc);
                }
                NEWHEAP(dim,A_MVT((*desc),ptdv,class));
                RES_HEAP;
                ipnew = (int *) R_MVT((*desc),ptdv,class);
                ip1 = (int *) R_MVT((*arg1),ptdv,class);
                ip2 = (int *) R_MVT((*arg2),ptdv,class);

                switch(op)
                {
                  case OP_ADD:
                       for (count = 0; count < dim; ++count)
                       ipnew[count] = ip1[count] + ip2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_MULT:
                       for (count = 0; count < dim; ++count)
                       ipnew[count] = ip1[count] * ip2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_SUB:
                       for (count = 0; count < dim; ++count)
                       ipnew[count] = ip1[count] - ip2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_MOD:
                       for (count = 0; count < dim; ++count)
                       ipnew[count] = ip1[count] % ip2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  default: post_mortem("mvt_mvt_op: Unknown operation");
                } /* end of case of operations */
           case TY_REAL:
                NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
                RES_HEAP;
                rpnew = (double *) R_MVT((*desc),ptdv,class);
                ip1 = (int *) R_MVT((*arg1),ptdv,class);
                rp2 = (double *) R_MVT((*arg2),ptdv,class);

                switch(op)
                {
                  case OP_ADD:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ip1[count] + rp2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_MULT:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ip1[count] * rp2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_SUB:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ip1[count] - rp2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_DIV:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ip1[count] / rp2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_MOD:
                       for (count = 0 ; count < dim; ++count) {
                         help = ((double)ip1[count]) / rp2[count];
                         m = help;
                         help = ((double)ip1[count]) - m * rp2[count];
                         rpnew[count] = help;
                       }
                       REL_HEAP; DBUG_RETURN(desc);
                  default: post_mortem("mvt_mvt_op: Unknown operation");
                } /* end of case of operations */
           default:
                DEC_REFCNT(desc);
                DBUG_RETURN((PTR_DESCRIPTOR)0);
         } /* end of case of type of arg2 */
    case TY_REAL:
         switch(type)
         {
           case TY_INTEGER:
                L_DESC((*desc),type) = TY_REAL;
                NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
                RES_HEAP;
                rpnew = (double *) R_MVT((*desc),ptdv,class);
                rp1 = (double *) R_MVT((*arg1),ptdv,class);
                ip2 = (int *) R_MVT((*arg2),ptdv,class);

                switch(op)
                {
                  case OP_ADD:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = rp1[count] + ip2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_MULT:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = rp1[count] * ip2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_SUB:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = rp1[count] - ip2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_DIV:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = rp1[count] / ip2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_MOD:
                       for (count = 0 ; count < dim; ++count) {
                         help = rp1[count] / ((double)ip2[count]);
                         m = help;
                         help = rp1[count] - m * ((double)ip2[count]);
                         rpnew[count] = help;
                       }
                       REL_HEAP; DBUG_RETURN(desc);
                  default: post_mortem("mvt_mvt_op: Unknown operation");
                } /* end of case of operations */
           case TY_REAL:
                NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
                RES_HEAP;
                rpnew = (double *) R_MVT((*desc),ptdv,class);
                rp1 = (double *) R_MVT((*arg1),ptdv,class);
                rp2 = (double *) R_MVT((*arg2),ptdv,class);

                switch(op)
                {
                  case OP_ADD:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = rp1[count] + rp2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_MULT:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = rp1[count] * rp2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_SUB:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = rp1[count] - rp2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_DIV:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = rp1[count] / rp2[count];
                       REL_HEAP; DBUG_RETURN(desc);
                  case OP_MOD:
                       for (count = 0 ; count < dim; ++count) {
                         help = rp1[count] / rp2[count];
                         m = help;
                         help = rp1[count] - m * rp2[count];
                         rpnew[count] = help;
                       }
                       REL_HEAP; DBUG_RETURN(desc);
                  default: post_mortem("mvt_mvt_op: Unknown operation");
                } /* end of case of operations */
           default: DEC_REFCNT(desc);
                    DBUG_RETURN((PTR_DESCRIPTOR)0);
         } /* end of case of type of arg2 */
    case TY_DIGIT:
         if (type != TY_DIGIT)
         {
           DEC_REFCNT(desc);
           DBUG_RETURN((PTR_DESCRIPTOR)0);
         }
         NEWHEAP(dim,A_MVT((*desc),ptdv,class));
         _digit_recycling = FALSE;

         switch(op)
         {
           case OP_ADD:
                for (count = 0; count < dim; ++count)
                {
                  element1 = R_MVT(DESC(arg1),ptdv,class)[count];
                  element2 = R_MVT(DESC(arg2),ptdv,class)[count];
                  TEST(L_MVT((*desc),ptdv,class)[count] = (int) DI_ADD(element1,element2));
                }
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_MULT:
                for (count = 0; count < dim; ++count)
                {
                  element1 = R_MVT(DESC(arg1),ptdv,class)[count];
                  element2 = R_MVT(DESC(arg2),ptdv,class)[count];
                  TEST(L_MVT((*desc),ptdv,class)[count] = (int) digit_mul(element1,element2));
                }
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_SUB:
                for (count = 0; count < dim; ++count)
                {
                  element1 = R_MVT(DESC(arg1),ptdv,class)[count];
                  element2 = R_MVT(DESC(arg2),ptdv,class)[count];
                  TEST(L_MVT((*desc),ptdv,class)[count] = (int) DI_SUB(element1,element2));
                }
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_DIV:
                _digit_recycling = FALSE;
                for (count = 0; count < dim; ++count)
                {
                  element1 = R_MVT(DESC(arg1),ptdv,class)[count];
                  element2 = R_MVT(DESC(arg2),ptdv,class)[count];
                  TEST(L_MVT((*desc),ptdv,class)[count] = (int) digit_div(element1,element2));
                }
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_MOD:
                _digit_recycling = FALSE;
                for (count = 0; count < dim; ++count)
                {
                  element1 = R_MVT(DESC(arg1),ptdv,class)[count];
                  element2 = R_MVT(DESC(arg2),ptdv,class)[count];
                  TEST(L_MVT((*desc),ptdv,class)[count] = (int) digit_mod(element1,element2));
                }
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           default: post_mortem("mvt_mvt_op: Unknown operation");
         } /* end of case of operations */
    default:
      DEC_REFCNT(desc);
      DBUG_RETURN((PTR_DESCRIPTOR)0);
  } /* end of case of type of arg1 */

}

/*****************************************************************************/
/*              mvt_op      (monadic matrix/vector/tr.vector operation)      */
/* ------------------------------------------------------------------------- */
/* function  :  do monadic operation with matrix / vector / transp. vector   */
/* parameter :  olddesc  - descri. of class matrix, vector or transp. vector */
/*              op       - monadic operation                                 */
/* returns   :  pointer to descriptor of result of operation                 */
/* calls     :                                                               */
/* called by :                                                               */
/*****************************************************************************/
 PTR_DESCRIPTOR mvt_op(olddesc,op)
PTR_DESCRIPTOR olddesc;
int  op;
{
  PTR_DESCRIPTOR desc;
  register int i,dim=0; /* Initialisierung von TB, 6.11.1992 */
  register char class,                        /*      class of olddesc       */
                type;                         /*       type of olddesc       */
  register int *ip,*ipnew=(int *)0;           /*    (new) integer-pointer    */
  /* Initialisierung von TB, 6.11.1992 */
  double *rp,*rpnew=(double *)0;              /*      (new) real-pointer     */
  /* Initialisierung von TB, 6.11.1992 */
 
  DBUG_ENTER ("mvt_op");

  class = R_DESC((*olddesc),class);
  type  = R_DESC((*olddesc),type);
  NEWDESC(_desc); INIT_DESC;
  MVT_DESC_MASK(desc,1,class,type);

  switch(class)
  {
  /*~~~~~~~~~~~~~~*/
    case C_MATRIX:
  /*~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = dim = R_MVT((*olddesc),nrows,class);
        L_MVT((*desc),ncols,class) = i   = R_MVT((*olddesc),ncols,class);
        dim *= i;
        break;
  /*~~~~~~~~~~~~~~*/
    case C_VECTOR:
  /*~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = 1;
        L_MVT((*desc),ncols,class) = dim = R_MVT((*olddesc),ncols,class);
        break;
  /*~~~~~~~~~~~~~~~*/
    case C_TVECTOR:
  /*~~~~~~~~~~~~~~~*/
        L_MVT((*desc),nrows,class) = dim = R_MVT((*olddesc),nrows,class);
        L_MVT((*desc),ncols,class) = 1;
        break;
  }

  switch(type)
  {
    case TY_INTEGER:
         NEWHEAP(dim,A_MVT((*desc),ptdv,class));
         RES_HEAP;
         ipnew = (int *)R_MVT((*desc),ptdv,class);
         ip    = (int *)R_MVT((*olddesc),ptdv,class);
         switch(op)
         {
           case OP_ABS:
                for (i = 0; i < dim; ++i)
                  ipnew[i] = (ip[i] > 0 ? ip[i] : -ip[i]);
                REL_HEAP; DBUG_RETURN(desc);
           case OP_NEG:
                for (i = 0; i < dim; ++i)
                  ipnew[i] = - ip[i];
                REL_HEAP; DBUG_RETURN(desc);
           case OP_FRAC:
                for (i = 0; i < dim; ++i)
                  ipnew[i] = 0;
                REL_HEAP; DBUG_RETURN(desc);
           case OP_TRUNC:
           case OP_FLOOR:
           case OP_CEIL:
                for (i = 0; i < dim; ++i)
                  ipnew[i] = ip[i];
                REL_HEAP; DBUG_RETURN(desc);
           default: post_mortem("mvt_op: Unknown operation");
         }
    case TY_REAL:
         rp    = (double *)R_MVT((*olddesc),ptdv,class);
         if ((op == OP_ABS) || (op == OP_FRAC))
         {
           NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class));
           RES_HEAP;
           rpnew = (double *)R_MVT((*desc),ptdv,class);
         }
         else
         {
           L_DESC((*desc),type) = TY_INTEGER;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class));
           RES_HEAP;
           ipnew = (int *) R_MVT((*desc),ptdv,class);
         }
         switch(op)
         {
           case OP_ABS:
                for (i = 0; i < dim; ++i)
                  rpnew[i] = (rp[i] > 0 ? rp[i] : -rp[i]);
                REL_HEAP; DBUG_RETURN(desc);
           case OP_NEG:
                for (i = 0; i < dim; ++i)
                  rpnew[i] = - rp[i];
                REL_HEAP; DBUG_RETURN(desc);
           case OP_TRUNC:
                for (i = 0; i < dim; ++i)
                  ipnew[i] = rp[i];
                REL_HEAP; DBUG_RETURN(desc);
           case OP_FRAC:
                for (i = 0; i < dim; ++i)
                  rpnew[i] = rp[i] - (int)rp[i];
                REL_HEAP; DBUG_RETURN(desc);
           case OP_FLOOR:
                for (i = 0; i < dim; ++i)
                  ipnew[i] = (rp[i] >= 0 ? rp[i] : rp[i] - 1);
                REL_HEAP; DBUG_RETURN(desc);
           case OP_CEIL:
                for (i = 0; i < dim; ++i)
                {
                  ipnew[i] = rp[i];
                  if (rp[i] > 0.0) ipnew[i]++;
                }
                REL_HEAP; DBUG_RETURN(desc);
           default: post_mortem(" mvt_op: Unknown operation");
         }
    case TY_DIGIT:
         NEWHEAP(dim,A_MVT((*desc),ptdv,class));

         switch(op)
         {
           case OP_ABS:
                if (R_DESC((*olddesc),ref_count) > 1) _digit_recycling = FALSE;
                for (i = 0; i < dim; ++i)
                  TEST(L_MVT((*desc),ptdv,class)[i] = (int) digit_abs(R_MVT((*olddesc),ptdv,class)[i]));
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_NEG:
                if (R_DESC((*olddesc),ref_count) > 1) _digit_recycling = FALSE;
                for (i = 0; i < dim; ++i)
                  TEST(L_MVT((*desc),ptdv,class)[i] = (int) digit_neg(R_MVT((*olddesc),ptdv,class)[i]));
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_TRUNC:
                if (R_DESC((*olddesc),ref_count) > 1) _digit_recycling = FALSE;
                for (i = 0; i < dim; ++i)
                  TEST(L_MVT((*desc),ptdv,class)[i] = (int) digit_trunc(R_MVT((*olddesc),ptdv,class)[i]));
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_FRAC:
                if (R_DESC((*olddesc),ref_count) > 1) _digit_recycling = FALSE;
                for (i = 0; i < dim; ++i)
                  TEST(L_MVT((*desc),ptdv,class)[i] = (int) digit_frac(R_MVT((*olddesc),ptdv,class)[i]));
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_FLOOR:
                _digit_recycling = FALSE;
                for (i = 0; i < dim; ++i)
                  TEST(L_MVT((*desc),ptdv,class)[i] = (int) digit_floor(R_MVT((*olddesc),ptdv,class)[i]));
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           case OP_CEIL:
                _digit_recycling = FALSE;
                for (i = 0; i < dim; ++i)
                  TEST(L_MVT((*desc),ptdv,class)[i] = (int) digit_ceil(R_MVT((*olddesc),ptdv,class)[i]));
                _digit_recycling = TRUE;
                DBUG_RETURN(desc);
           default: post_mortem("mvt_op: Unknown operation");
         }
    default:
        DEC_REFCNT(desc);
        DBUG_RETURN((PTR_DESCRIPTOR)0);
  }
}

/************************  end of file rval-int.c ***************************/
