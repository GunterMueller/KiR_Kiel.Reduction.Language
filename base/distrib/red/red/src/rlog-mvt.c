/*
 * $Log: rlog-mvt.c,v $
 * Revision 1.5  2001/07/02 14:53:33  base
 * compiler warnings eliminated
 *
 * Revision 1.4  1995/05/22 09:46:39  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:09  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */

/*****************************************************************************/
/*                         MODULE FOR PROCESSING PHASE                       */
/* ------------------------------------------------------------------------- */
/* rlog-mvt.c -- external:   boolmvt_bop,     bmvt_bmvt_bop,                 */
/*                           scalar_mvt_bop,  digit_mvt_bop,                 */
/*                           bool_mvt_bop,    mvt_mvt_bop,                   */
/*                           str_mvt_bop                                     */
/*            -- called by:  functions in rlogfunc.c                         */
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
#include "rops.h"
#include "rmeasure.h"
#include <setjmp.h>

#include "dbug.h"

#if D_MESS
#include "d_mess_io.h"
#endif

/*--------------------------------------------------------------------------*/
/*                            extern variables                              */
/*--------------------------------------------------------------------------*/
extern jmp_buf _interruptbuf;       /* <r>, Adresse fuer Ausnahmebehandlung */
extern PTR_DESCRIPTOR _desc;        /* <w>, Variable fuer Ergebnisdeskriptor*/

/*--------------------------------------------------------------------------*/
/*                            extern functions                              */
/*--------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR newdesc();   /* rheap.c    */
extern int     newheap();          /* rheap.c    */
extern BOOLEAN digit_eq();         /* rdig-v1.c  */
extern BOOLEAN digit_le();         /* rdig-v1.c  */
extern BOOLEAN digit_lt();         /* rdig-v1.c  */
extern BOOLEAN str_eq();           /* rlogfunc.c */
extern BOOLEAN str_le();           /* rlogfunc.c */
extern BOOLEAN str_lt();           /* rlogfunc.c */

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

#if DEBUG
extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

/*--------------------------------------------------------------------------*/
/*                            defines                                       */
/*--------------------------------------------------------------------------*/
/* ------- macros for heapoverflow - handling ---------- */
/* Ein Interrupt bewirkt die Zuruecknahme der Reduktion. */
/* siehe setjmp(_interruptbuf) in der Datei rear.c       */
#if (D_SLAVE && D_MESS && D_MHEAP)
#define NEWHEAP(size,adr)  if ((*d_m_newheap)(size,adr) == 0) \
                             longjmp(_interruptbuf,0)
#else
#define NEWHEAP(size,adr)  if (newheap(size,adr) == 0) \
                             longjmp(_interruptbuf,0)
#endif
#define INIT_DESC          if ((desc = _desc) == 0)    \
                             longjmp(_interruptbuf,0)
#define TEST(value)        if ((value) == 0) goto fail;

#define DESC(x)  (* (PTR_DESCRIPTOR) x)

/*------ used in: str_mvt_bop, mvt_mvt_bop --------------------*/
#define STR_NEW    ((BOOLEAN *) R_MVT((*desc),ptdv,class2))

/*------ used in: digit_mvt_bop, mvt_mvt_bop ------------------*/
#define IPNEW          ((int *) R_MVT((*desc),ptdv,class2))

/*------ used in: mvt_mvt_bop ---------------------------------*/
#define IP1            ((int *) R_MVT(DESC(arg1),ptdv,class2))

/*------ used in: digit_mvt_bop, str_mvt_bop , mvt_mvt_bop-----*/
#define IP2            ((int *) R_MVT(DESC(arg2),ptdv,class2))


/*****************************************************************************/
/*                             boolmvt_bop                                   */
/* ------------------------------------------------------------------------- */
/* function  : Auf eine Matrix bzw. ein Vektor bzw. ein  TVektor von         */
/*             Booleans wird die Operation not angewand,                     */
/*             oder das True- bzw. False-XXX  soll erstellt werden.          */
/*             Hat der Dekriptor den Refcount 1, so wird der Deskriptor      */
/*             wiederverwendet.                                              */
/* paramteter: olddesc --  pointer to descriptor                             */
/*             op      --  boolean operator                                  */
/* returns   : stackelement                                                  */
/* called by : bool_mvt_bop, red_or, red_and, red_not                        */
/* calls     : none                                                          */
/*****************************************************************************/
STACKELEM boolmvt_bop(olddesc,op)
PTR_DESCRIPTOR olddesc;
int  op;       /* eine boolmvt operation */
{
  register int i,dim;
  register int *p,*pnew;
  PTR_DESCRIPTOR desc;
  char           class1;

  DBUG_ENTER ("boolmvt_bop");

  class1 = R_DESC((*olddesc),class);

  if (R_DESC((*olddesc),ref_count) == 1) {
                                          /* die Matrix wird wiederverwendet */
    INC_REFCNT(olddesc);
    RES_HEAP;
    pnew = R_MVT((*olddesc),ptdv,class1);
    dim  = R_MVT((*olddesc),nrows,class1) * R_MVT((*olddesc),ncols,class1);

    switch(op) {
      case OP_NOT:
           for (i = 0; i < dim; i++ )
             pnew[i] = !pnew[i];
           REL_HEAP;
           DBUG_RETURN((STACKELEM)olddesc);
      case OP_TRUE:
           for (i = 0; i < dim; i++ )
             pnew[i] = TRUE;
           REL_HEAP;
           DBUG_RETURN((STACKELEM)olddesc);
      case OP_FALSE:
           for (i = 0; i < dim; i++ )
             pnew[i] = FALSE;
           REL_HEAP;
           DBUG_RETURN((STACKELEM)olddesc);
      default: post_mortem("boolmvt_bop: Unknown operation");
    } /** end switch op **/
  } /** end if ref_count == 1 **/

  else {
    NEWDESC(_desc); INIT_DESC;
    MVT_DESC_MASK(desc,1,class1,TY_BOOL);
    L_MVT((*desc),nrows,class1) = dim = R_MVT((*olddesc),nrows,class1);
    L_MVT((*desc),ncols,class1) =  i  = R_MVT((*olddesc),ncols,class1);
    dim *= i;
    NEWHEAP(dim,A_MVT((*desc),ptdv,class1));
    RES_HEAP;
    pnew = (int *) R_MVT((*desc),ptdv,class1);
    p    = (int *) R_MVT((*olddesc),ptdv,class1);

    switch(op) {
      case OP_NOT:
           for (i = 0; i < dim; i++ )
             pnew[i] = !p[i];
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);
      case OP_TRUE:
           for (i = 0; i < dim; i++ )
             pnew[i] = TRUE;
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);
      case OP_FALSE:
           for (i = 0; i < dim; i++ )
             pnew[i] = FALSE;
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);
      default: post_mortem("boolmvt_bop: Unknown operation");
    } /** end switch op **/
  } /** end else ref_count != 1 **/
  DBUG_RETURN ((STACKELEM) 0);                   /* ach 30/10/92 */
} /**** end of function boolmvt_bop ****/


/*****************************************************************************/
/*                             bool_mvt_bop                                  */
/* ------------------------------------------------------------------------- */
/* function  : verknuepft einen Boolean mit einem unbekannten Argument.      */
/*             liefert eine Null zurueck, falls nicht anwendbar.             */
/* paramteter: arg1 --  Boolean                                              */
/*             arg2 --  unknown argument                                     */
/*             op   --  compare operator                                     */
/* returns   : stackelement                                                  */
/* called by : red_lt, red_le, red_min                                       */
/* calls     : boolmvt_bop                                                   */
/*****************************************************************************/
STACKELEM bool_mvt_bop(arg1,arg2,op)
STACKELEM arg1; /* ein Boolean              */
STACKELEM arg2; /* ein unbekanntes Argument */
int op;         /* eine compare Operation   */
{
  char class2;  /* class of arg2            */

  DBUG_ENTER ("bool_mvt_bop");

  /* das unbekannte Argument muss ein Boolmvt sein: */
  if ( !T_POINTER(arg2) ||
       (((class2 = R_DESC(DESC(arg2),class)) != C_MATRIX)    &&
         (class2                             != C_VECTOR)    &&
         (class2                             != C_TVECTOR))  ||
       (R_DESC(DESC(arg2),type) != TY_BOOL) )
    DBUG_RETURN(0);

  if T_SA_TRUE(arg1)
    switch (op) {
        case OP_NEQ:
        case OP_GT:
             DBUG_RETURN(boolmvt_bop((PTR_DESCRIPTOR)arg2,OP_NOT));  /* RS 30.11.1992 */
        case OP_LT:
             DBUG_RETURN(boolmvt_bop((PTR_DESCRIPTOR)arg2,OP_FALSE)); /* RS 30.11.1992 */
        case OP_GE:
        case OP_MAX:
             DBUG_RETURN(boolmvt_bop((PTR_DESCRIPTOR)arg2,OP_TRUE));  /* RS 30.11.1992 */
        default: /* die Identitaetsoperation */
             INC_REFCNT((PTR_DESCRIPTOR)arg2);
             DBUG_RETURN(arg2);
    }  /** end switch op **/
  else
    switch (op) {
        case OP_EQ:
        case OP_GE:
             DBUG_RETURN(boolmvt_bop((PTR_DESCRIPTOR)arg2,OP_NOT));   /* RS 30.11.1992 */
        case OP_GT:
        case OP_MIN:
             DBUG_RETURN(boolmvt_bop((PTR_DESCRIPTOR)arg2,OP_FALSE));  /* RS 30.11.1992 */
        case OP_LE:
             DBUG_RETURN(boolmvt_bop((PTR_DESCRIPTOR)arg2,OP_TRUE));   /* RS 30.11.1992 */
        default: /* die Identitaetsoperation */
             INC_REFCNT((PTR_DESCRIPTOR)arg2);
             DBUG_RETURN(arg2);
    } /** end switch op **/
} /**** end of function bool_mvt_bop ****/


/*****************************************************************************/
/*                           scalar_mvt_bop                                  */
/* ------------------------------------------------------------------------- */
/* function  : Ein Scalar wird mit einer Matrix, einem Vektor bwz.           */
/*             einem TVektor unbekannten Typs verknuepft.                    */
/*             Die Funktion liefert eine Null zurueck, falls nicht anwendbar.*/
/* paramteter: arg1 -- pointer to scalar descriptor                          */
/*             arg2 -- pointer to matrix- / vector- / tvector-descriptor     */
/*             op   -- compare operator                                      */
/* returns   : stackelement                                                  */
/* called by : red_lt, red_le, red_min                                       */
/* calls     : none                                                          */
/*****************************************************************************/
STACKELEM scalar_mvt_bop(arg1,arg2,op)
PTR_DESCRIPTOR arg1;           /* ein Scalardeskriptor                       */
PTR_DESCRIPTOR arg2;           /* der Matrix / Vektor / Tvektor-Deskriptor   */
int  op;                       /* eine compare Operation                     */
{
  register PTR_DESCRIPTOR desc;
  register int count,dim;
  int    i, *ip/*, *ipnew = (int *) 0*/;       /* "i" == integer                             */
  BOOLEAN *ipnew = (BOOLEAN *) 0;          /* neuer Typ von RS 6.11.1992 */
  double r = 0.0, *rp, *rpnew = (double *) 0;       /* "r" == real                                */
  /* Initialisierungen von RS 6.11.1992 */
  char   class2;               /* class von arg2                             */

  DBUG_ENTER ("scalar_mvt_bop");

  class2 = R_DESC((*arg2),class);

  if (R_DESC((*arg1),type) == TY_INTEGER)
                                          /*--------------------------------*/
    switch(R_DESC((*arg2),type)) {        /* Der Scalar ist vom Typ INTEGER */
                                          /*--------------------------------*/
      case TY_INTEGER:
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class2,TY_BOOL);
           L_MVT((*desc),nrows,class2) = dim = R_MVT((*arg2),nrows,class2);
           L_MVT((*desc),ncols,class2) =  i  = R_MVT((*arg2),ncols,class2);
           dim *= i;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
           RES_HEAP;
           ip    = (int *) R_MVT((*arg2),ptdv,class2);
           ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);
           i     = R_SCALAR((*arg1),vali);

           switch(op) {
             case OP_EQ:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i == ip[count] );
                  break;
             case OP_NEQ:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i != ip[count] );
                  break;
             case OP_GT:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i > ip[count] );
                  break;
             case OP_GE:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i >= ip[count] );
                  break;
             case OP_LE:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i <= ip[count] );
                  break;
             case OP_LT:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i < ip[count] );
                  break;
             case OP_MAX:
                  /* Max produziert keine Matrix / kein Vektor / kein TVektor */
                  /* von Booleans, sondern eine MVT von Integern.             */
                  MVT_DESC_MASK(desc,1,class2,TY_INTEGER);
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i < ip[count] ? ip[count] : i);
                  break;
             case OP_MIN:
                  /*  dito  */
                  MVT_DESC_MASK(desc,1,class2,TY_INTEGER);
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i < ip[count] ? i : ip[count]);
                  break;
             default:
                  post_mortem("scalar_mvt_bop: Unknown operation received");
           } /** end switch op **/
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);

      case TY_REAL:
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class2,TY_BOOL);
           L_MVT((*desc),nrows,class2) = dim = R_MVT((*arg2),nrows,class2);
           L_MVT((*desc),ncols,class2) =  i  = R_MVT((*arg2),ncols,class2);
           dim *= i;

           if (op < OP_MAX) {
             NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
             RES_HEAP;
             ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);
           }
           else {
               /* Es entsteht doch keine Matrix / kein Vektor / kein TVektor */
               /* von Boolean                                                */
             MVT_DESC_MASK(desc,1,class2,TY_REAL);
             NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class2));
             RES_HEAP;
             rpnew = (double *) R_MVT((*desc),ptdv,class2);
           }
           rp    = (double *) R_MVT((*arg2),ptdv,class2);
           r     =   (double) R_SCALAR((*arg1),vali);

           switch(op) {
             case OP_EQ:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r == rp[count] );
                  break;
             case OP_NEQ:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r != rp[count] );
                  break;
             case OP_GT:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r > rp[count] );
                  break;
             case OP_GE:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r >= rp[count] );
                  break;
             case OP_LE:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r <= rp[count] );
                  break;
             case OP_LT:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r < rp[count] );
                  break;
             case OP_MAX:
                  for (count = 0; count < dim; ++count)
                    rpnew[count] = ( r < rp[count] ? rp[count] : r);
                  break;
             case OP_MIN:
                  for (count = 0; count < dim; ++count)
                    rpnew[count] = ( r < rp[count] ? r : rp[count]);
                  break;
             default:
                  post_mortem("scalar_mvt_bop: Unknown operation received");
           } /** end switch op **/
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);
      default:
           DBUG_RETURN(0);
    } /** end if Scalar ist vom Typ INTEGER **/
  else                                    /*---------------------------------*/
                                          /* Der Scalar ist vom Typ REAL     */
    switch(R_DESC((*arg2),type)) {        /*---------------------------------*/

      case TY_INTEGER:
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class2,TY_BOOL);
           L_MVT((*desc),nrows,class2) = dim = R_MVT((*arg2),nrows,class2);
           L_MVT((*desc),ncols,class2) =  i  = R_MVT((*arg2),ncols,class2);
           dim *= i;

           if (op < OP_MAX) {
             NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
             RES_HEAP;
             ip    =     (int *) R_MVT((*arg2),ptdv,class2);
             ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);
             i     =       (int) R_SCALAR((*arg1),valr);
           }
           else {
             MVT_DESC_MASK(desc,1,class2,TY_REAL);
             NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class2));
             RES_HEAP;
             ip    =    (int *) R_MVT((*arg2),ptdv,class2);
             rpnew = (double *) R_MVT((*desc),ptdv,class2);
             r     =            R_SCALAR((*arg1),valr);
             i     =      (int) R_SCALAR((*arg1),valr);
           }

           switch(op) {
             case OP_EQ:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i == ip[count] );
                  break;
             case OP_NEQ:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i != ip[count] );
                  break;
             case OP_GT:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i > ip[count] );
                  break;
             case OP_GE:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i >= ip[count] );
                  break;
             case OP_LE:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i <= ip[count] );
                  break;
             case OP_LT:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( i < ip[count] );
                  break;
             case OP_MAX:
                  for (count = 0; count < dim; ++count)
                    rpnew[count] = ( i < ip[count] ? (double) ip[count] : r);
                  break;
             case OP_MIN:
                  for (count = 0; count < dim; ++count)
                    rpnew[count] = ( i < ip[count] ? r : (double) ip[count]);
                  break;
             default:
                  post_mortem("scalar_mvt_bop: Unknown operation received");
           } /** end switch op **/
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);

      case TY_REAL:
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class2,TY_BOOL);
           L_MVT((*desc),nrows,class2) = dim = R_MVT((*arg2),nrows,class2);
           L_MVT((*desc),ncols,class2) =  i  = R_MVT((*arg2),ncols,class2);
           dim *= i;

           if (op < OP_MAX) {
             NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
             RES_HEAP;
             ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);
           }
           else {
             MVT_DESC_MASK(desc,1,class2,TY_REAL);
             NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class2));
             RES_HEAP;
             rpnew = (double *) R_MVT((*desc),ptdv,class2);
           }

           rp    = (double *) R_MVT((*arg2),ptdv,class2);
           r     = R_SCALAR((*arg1),valr);

           switch(op) {
             case OP_EQ:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r == rp[count] );
                  break;
             case OP_NEQ:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r != rp[count] );
                  break;
             case OP_GT:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r > rp[count] );
                  break;
             case OP_GE:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r >= rp[count] );
                  break;
             case OP_LE:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r <= rp[count] );
                  break;
             case OP_LT:
                  for (count = 0; count < dim; ++count)
                    ipnew[count] = ( r < rp[count] );
                  break;
             case OP_MAX:
                  for (count = 0; count < dim; ++count)
                    rpnew[count] = ( r < rp[count] ? rp[count] : r);
                  break;
             case OP_MIN:
                  for (count = 0; count < dim; ++count)
                    rpnew[count] = ( r < rp[count] ? r : rp[count]);
                  break;
             default:
                  post_mortem("scalar_mvt_bop: Unknown operation received");
           } /** end switch op **/
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);
      default:
           DBUG_RETURN(0);
    } /** end else Scalar ist vom Typ REAL **/

} /**** end of function scalar_mvt_bop ****/


/*****************************************************************************/
/*                            digit_mvt_bop                                  */
/* ------------------------------------------------------------------------- */
/* function  : Eine Digitzahl wird mit einer Matrix, bzw. Vektor  bzw.       */
/*             TVektor von unbekannten Typs verknuepft.                      */
/*             Es wird eine Null zurueckgegeben, falls die                   */
/*             Funktion nicht anwendbar ist.                                 */
/* paramteter: arg1 -- digit-descriptor                                      */
/*             arg2 -- matrix- / vector- / tvector-descriptor                */
/*             op   -- compare operator                                      */
/* returns   : stackelement                                                  */
/* called by : red_lt, red_le, red_min                                       */
/* calls     : digit_eq, digit_lt, digit_le.                                 */
/*****************************************************************************/
STACKELEM digit_mvt_bop(arg1,arg2,op)
PTR_DESCRIPTOR arg1;       /* der Digit-Deskriptor                           */
PTR_DESCRIPTOR arg2;       /* der Matrix / Vektor / TVektor-Deskriptor       */
int op;                    /* eine compare Operation                         */
{
  register PTR_DESCRIPTOR desc;
  register int i,dim;
  /* int *ipnew, *ip;                     ach 30/10/92 */
  char class2;

  DBUG_ENTER ("digit_mvt_bop");


  if (R_DESC((*arg2),type) != TY_DIGIT)  /* die Matrix / der Vektor / der    */
    DBUG_RETURN(0);                           /* TVektor ist nicht vom Typ DIGIT  */

  class2 = R_DESC((*arg2),class);        /* lesen der Klasse von arg2        */
                                         /* C_MATRIX / C_VECTOR / C_TVECTOR  */
  NEWDESC(_desc); INIT_DESC;
  MVT_DESC_MASK(desc,1,class2,TY_BOOL);
  L_MVT((*desc),nrows,class2) = dim = R_MVT((*arg2),nrows,class2);
  L_MVT((*desc),ncols,class2) =  i  = R_MVT((*arg2),ncols,class2);
  dim *= i;

  NEWHEAP(dim,A_MVT((*desc),ptdv,class2));

  switch(op) {
    case OP_EQ:
         for (i = 0; i < dim; ++i)
           (IPNEW[i]) = digit_eq(arg1,IP2[i]);
         break;
    case OP_NEQ:
         for (i = 0; i < dim; ++i)
           (IPNEW[i]) = !digit_eq(arg1,IP2[i]);
         break;
    case OP_GT:
         for (i = 0; i < dim; ++i)
           (IPNEW[i]) = !digit_le(arg1,IP2[i]);
         break;
    case OP_GE:
         for (i = 0; i < dim; ++i)
           (IPNEW[i]) = !digit_lt(arg1,IP2[i]);
         break;
    case OP_LE:
         for (i = 0; i < dim; ++i)
           (IPNEW[i]) = digit_le(arg1,IP2[i]);
         break;
    case OP_LT:
         for (i = 0; i < dim; ++i)
           (IPNEW[i]) = digit_lt(arg1,IP2[i]);
         break;
    case OP_MAX:
         /* es entsteht doch keine bmat / kein bvect / kein btvect */
         MVT_DESC_MASK(desc,1,class2,TY_DIGIT);
         for (i = 0; i < dim; ++i) {
           (IPNEW[i]) = (digit_le(arg1,IP2[i]) ? IP2[i] : (STACKELEM)arg1);
           INC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
         }
         break;
    case OP_MIN:
         /*  dito  */
         MVT_DESC_MASK(desc,1,class2,TY_DIGIT);
         for (i = 0; i < dim; ++i) {
           (IPNEW[i]) = (digit_le(arg1,IP2[i]) ? (STACKELEM)arg1 : IP2[i]);
           INC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
         }
         break;
    default:
         post_mortem("digit_mvt_bop: Unkown operation received");
  } /** end switch op **/
  DBUG_RETURN((STACKELEM)desc);

} /**** end of function digit_mvt_bop ****/


/*****************************************************************************/
/*                              str_mvt_bop                                  */
/* ------------------------------------------------------------------------- */
/* function  : Ein String wird mit einer Matrix, einem Vektor bzw. einem     */
/*             TVektors von unbekannten Typs verknuepft.                     */
/*             Es wird eine Null zurueckgegeben, falls                       */
/*             die Funktion nicht anwendbar ist.                             */
/* paramteter: arg1 -- string descriptor                                     */
/*             arg2 -- matrix- / vector- / tvector-descriptor                */
/*             op   -- compare operator                                      */
/* returns   : stackelement                                                  */
/* called by : red_lt, red_le, red_min                                       */
/* calls     : str_eq, str_lt, str_le                                        */
/*****************************************************************************/
STACKELEM str_mvt_bop(arg1,arg2,op)
PTR_DESCRIPTOR arg1;         /* der String-Deskriptor                        */
PTR_DESCRIPTOR arg2;         /* der Matrix / Vektor / TVektor-Deskriptor     */
int op;                      /* eine compare Operation                       */
{
  register PTR_DESCRIPTOR desc;
  register int i,dim;
  char class2;

  DBUG_ENTER ("str_mvt_bop");

  if (R_DESC((*arg2),type) != TY_STRING) /* wenn keine Matrix / Vektor /     */
    DBUG_RETURN(0);                           /* TVektor vom Typ STRING vorliegt  */

  class2 = R_DESC((*arg2),class);        /* lesen der Klasse von arg2        */
                                         /* C_MATRIX / C_VECTOR / C_TVECTOR  */
  NEWDESC(_desc); INIT_DESC;
  MVT_DESC_MASK(desc,1,class2,TY_BOOL);
  L_MVT((*desc),nrows,class2) = dim = R_MVT((*arg2),nrows,class2);
  L_MVT((*desc),ncols,class2) =  i  = R_MVT((*arg2),ncols,class2);
  dim *= i;

  NEWHEAP(dim,A_MVT((*desc),ptdv,class2));

  RES_HEAP;

  switch(op) {
    case OP_EQ:
         for (i = 0; i < dim; ++i)
           STR_NEW[i] = str_eq(arg1,IP2[i]);
         break;
    case OP_NEQ:
         for (i = 0; i < dim; ++i)
           STR_NEW[i] = !str_eq(arg1,IP2[i]);
         break;
    case OP_GT:
         for (i = 0; i < dim; ++i)
           STR_NEW[i] = !str_le(arg1,IP2[i]);
         break;
    case OP_GE:
         for (i = 0; i < dim; ++i)
           STR_NEW[i] = !str_lt(arg1,IP2[i]);
         break;
    case OP_LE:
         for (i = 0; i < dim; ++i)
           STR_NEW[i] = str_le(arg1,IP2[i]);
         break;
    case OP_LT:
         for (i = 0; i < dim; ++i)
           STR_NEW[i] = str_lt(arg1,IP2[i]);
         break;
    case OP_MAX:
         MVT_DESC_MASK(desc,1,class2,TY_STRING);
         for (i = 0; i < dim; ++i) {
           STR_NEW[i] = (str_le(arg1,IP2[i]) ? IP2[i] : (STACKELEM)arg1);
           INC_REFCNT((PTR_DESCRIPTOR)STR_NEW[i]);
         }
         break;
    case OP_MIN:
         MVT_DESC_MASK(desc,1,class2,TY_STRING);
         for (i = 0; i < dim; ++i) {
           STR_NEW[i] = (str_le(arg1,IP2[i]) ? (STACKELEM)arg1 : IP2[i]);
           INC_REFCNT((PTR_DESCRIPTOR)STR_NEW[i]);
         }
         break;
    default:
         REL_HEAP;
         post_mortem("str_mvt_bop: Unkown operation received");
  } /** end switch op **/
  REL_HEAP;
  DBUG_RETURN((STACKELEM)desc);

} /**** end of function str_mvt_bop ****/


/*****************************************************************************/
/*                              mvt_mvt_bop                                  */
/* ------------------------------------------------------------------------- */
/* function  : Zwei Matrizen / Vektoren / TVektoren  unbekannten Typs        */
/*             werden verknuepft.                                            */
/*             Die Funktion liefert eine Null zurueck, falls sie             */
/*             nicht anwendbar ist.                                          */
/* paramteter: arg1 --  matrix- / vector- / tvector-descriptor               */
/*             arg2 --  matrix- / vector- / tvector-descriptor               */
/*             op   --  compare operator                                     */
/* returns   : stackelement                                                  */
/* called by : red_lt, red_le, red_min                                       */
/* calls     : digit_eq,  digit_lt,  digit_le,                               */
/*             str_eq,    str_lt,    str_le.                                 */
/*****************************************************************************/
STACKELEM mvt_mvt_bop(arg1,arg2,op)
PTR_DESCRIPTOR arg1,arg2;   /* Matrizen- / Vektor- / TVektor- Deskriptoren   */
int op;                     /* eine compare Operation                        */
{
  register PTR_DESCRIPTOR desc;
  register int count,dim;
  int nrows = 0,ncols = 0;
  int    *ip1, *ip2/*, *ipnew**/;                /* "i" == intger                 */
  BOOLEAN *ipnew = (BOOLEAN *) 0;   /* Initialisierungen und ipnew neu von RS 6.11.92 */
  double *rp1, *rp2, *rpnew = (double *) 0;                /* "r" == real                   */
  char   class1,class2;                     /* fuer die Klasse der Argumente */

  DBUG_ENTER ("mvt_mvt_bop");

  class1 = R_DESC(DESC(arg1),class);
  class2 = R_DESC(DESC(arg2),class);

  if (class1 != class2)
    DBUG_RETURN(0);

  switch(class2) {
    case  C_MATRIX : if (((nrows = R_MVT(DESC(arg1),nrows,class1)) !=
                                   R_MVT(DESC(arg2),nrows,class2)) ||
                         ((ncols = R_MVT(DESC(arg1),ncols,class1)) !=
                                   R_MVT(DESC(arg2),ncols,class2)))
                       /* wenn nicht gleiche Zeilenanzahl bzw. Spaltenanzahl */
                       DBUG_RETURN(0);
                     break;
    case  C_VECTOR : if ((ncols = R_MVT(DESC(arg1),ncols,class1)) !=
                                  R_MVT(DESC(arg2),ncols,class2))
                       DBUG_RETURN(0);                /* nicht dieselbe Dimension */
                     nrows = 1;
                     break;
    case  C_TVECTOR: if ((nrows = R_MVT(DESC(arg1),nrows,class1)) !=
                                  R_MVT(DESC(arg2),nrows,class2))
                       DBUG_RETURN(0);                /* nicht dieselbe Dimension */
                     ncols = 1;
                     break;
  } /** switch class2 **/

  dim = nrows * ncols;

  NEWDESC(_desc); INIT_DESC;        /* muss evtl. wieder freigegeben werden! */
  MVT_DESC_MASK(desc,1,class2,TY_BOOL);
  L_MVT((*desc),nrows,class2) = nrows;
  L_MVT((*desc),ncols,class2) = ncols;
  L_MVT((*desc),ptdv,class2)  = NULL;

  switch (R_DESC((*arg1),type)) {

  /*~~~~~~~~~~~~~~~~~*/
    case TY_INTEGER:
  /*~~~~~~~~~~~~~~~~~*/
         switch(R_DESC((*arg2),type)) {
           case TY_INTEGER:
                NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
                RES_HEAP;
                ip1   = (int *) R_MVT((*arg1),ptdv,class1);
                ip2   = (int *) R_MVT((*arg2),ptdv,class2);
                ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);

                switch(op) {
                  case OP_EQ:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] == ip2[count] );
                       break;
                  case OP_NEQ:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] != ip2[count] );
                       break;
                  case OP_GT:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] > ip2[count] );
                       break;
                  case OP_GE:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] >= ip2[count] );
                       break;
                  case OP_LE:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] <= ip2[count] );
                       break;
                  case OP_LT:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] < ip2[count] );
                       break;
                  case OP_MAX:
                       MVT_DESC_MASK(desc,1,class2,TY_INTEGER);
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] < ip2[count] ? ip2[count] : ip1[count]);
                       break;
                  case OP_MIN:
                       MVT_DESC_MASK(desc,1,class2,TY_INTEGER);
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] < ip2[count] ? ip1[count] : ip2[count]);
                       break;
                  default:
                       post_mortem("mvt_mvt_bop: Unknown operation received");
                } /** end switch op **/
                REL_HEAP;
                DBUG_RETURN((STACKELEM)desc);
           case TY_REAL:
                if (op < OP_MAX) {
                  NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
                  RES_HEAP;
                  ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);
                }
                else {
                  MVT_DESC_MASK(desc,1,class2,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class2));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*desc),ptdv,class2);
                }
                ip1   = (int *) R_MVT((*arg1),ptdv,class1);
                rp2   = (double *) R_MVT((*arg2),ptdv,class2);

                switch(op) {
                  case OP_EQ:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] == rp2[count] );
                       break;
                  case OP_NEQ:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] != rp2[count] );
                       break;
                  case OP_GT:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] > rp2[count] );
                       break;
                  case OP_GE:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] >= rp2[count] );
                       break;
                  case OP_LE:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] <= rp2[count] );
                       break;
                  case OP_LT:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( ip1[count] < rp2[count] );
                       break;
                  case OP_MAX:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ( ip1[count] < rp2[count] ? rp2[count] : ip1[count]);
                       break;
                  case OP_MIN:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ( ip1[count] < rp2[count] ? ip1[count] : rp2[count]);
                       break;
                  default:
                       post_mortem("mvt_mvt_bop: Unknown operation received");
                }
                REL_HEAP;
                DBUG_RETURN((STACKELEM)desc);
           default:
                goto fail;
         } /** end switch op **/
  /*~~~~~~~~~~~~~~~~~*/
    case TY_REAL:
  /*~~~~~~~~~~~~~~~~~*/
         switch(R_DESC((*arg2),type)) {
           case TY_INTEGER:
                if (op < OP_MAX) {
                  NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
                  RES_HEAP;
                  ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);
                }
                else {
                  MVT_DESC_MASK(desc,1,class2,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class2));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*desc),ptdv,class2);
                }
                rp1   = (double *) R_MVT((*arg1),ptdv,class1);
                ip2   =    (int *) R_MVT((*arg2),ptdv,class2);

                switch(op) {
                  case OP_EQ:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] == ip2[count] );
                       break;
                  case OP_NEQ:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] != ip2[count] );
                       break;
                  case OP_GT:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] > ip2[count] );
                       break;
                  case OP_GE:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] >= ip2[count] );
                       break;
                  case OP_LE:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] <= ip2[count] );
                       break;
                  case OP_LT:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] < ip2[count] );
                       break;
                  case OP_MAX:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ( rp1[count] < ip2[count] ? ip2[count] : rp1[count]);
                       break;
                  case OP_MIN:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ( rp1[count] < ip2[count] ? rp1[count] : ip2[count]);
                       break;
                  default:
                       post_mortem("mvt_mvt_bop: Unknown operation received");
                } /** end switch op **/
                REL_HEAP;
                DBUG_RETURN((STACKELEM)desc);
           case TY_REAL:
                if (op < OP_MAX) {
                  NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
                  RES_HEAP;
                  ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);
                }
                else {
                  MVT_DESC_MASK(desc,1,class2,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*desc),ptdv,class2));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*desc),ptdv,class2);
                }
                rp1   = (double *) R_MVT((*arg1),ptdv,class1);
                rp2   = (double *) R_MVT((*arg2),ptdv,class2);

                switch(op) {
                  case OP_EQ:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] == rp2[count] );
                       break;
                  case OP_NEQ:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] != rp2[count] );
                       break;
                  case OP_GT:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] > rp2[count] );
                       break;
                  case OP_GE:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] >= rp2[count] );
                       break;
                  case OP_LE:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] <= rp2[count] );
                       break;
                  case OP_LT:
                       for (count = 0; count < dim; ++count)
                         ipnew[count] = ( rp1[count] < rp2[count] );
                       break;
                  case OP_MAX:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ( rp1[count] < rp2[count] ? rp2[count] : rp1[count]);
                       break;
                  case OP_MIN:
                       for (count = 0; count < dim; ++count)
                         rpnew[count] = ( rp1[count] < rp2[count] ? rp1[count] : rp2[count]);
                       break;
                  default:
                       post_mortem("mvt_mvt_bop: Unknown operation received");
                } /** end switch op **/
                REL_HEAP;
                DBUG_RETURN((STACKELEM)desc);
           default:
                goto fail;
         }
  /*~~~~~~~~~~~~~~~~~*/
    case TY_DIGIT:
  /*~~~~~~~~~~~~~~~~~*/
         if (R_DESC((*arg2),type) != TY_DIGIT) goto fail;

         NEWHEAP(dim,A_MVT((*desc),ptdv,class2));

         switch(op) {
           case OP_EQ:
                for (count = 0; count < dim; ++count)
                  (IPNEW[count]) = digit_eq(IP1[count],IP2[count]);
                break;
           case OP_NEQ:
                for (count = 0; count < dim; ++count)
                  (IPNEW[count]) = !digit_eq(IP1[count],IP2[count]);
                break;
           case OP_GT:
                for (count = 0; count < dim; ++count)
                  (IPNEW[count]) = !digit_le(IP1[count],IP2[count]);
                break;
           case OP_GE:
                for (count = 0; count < dim; ++count)
                  (IPNEW[count]) = !digit_lt(IP1[count],IP2[count]);
                break;
           case OP_LE:
                for (count = 0; count < dim; ++count)
                  (IPNEW[count]) = digit_le(IP1[count],IP2[count]);
                break;
           case OP_LT:
                for (count = 0; count < dim; ++count)
                  (IPNEW[count]) = digit_lt(IP1[count],IP2[count]);
                break;
           case OP_MAX:
                MVT_DESC_MASK(desc,1,class2,TY_DIGIT);
                for (count = 0; count < dim; ++count) {
                  (IPNEW[count]) = (digit_le(IP1[count],IP2[count]) ? IP2[count] : (STACKELEM)IP1[count]);
                  INC_REFCNT((PTR_DESCRIPTOR)(IPNEW[count]));
                }
                break;
           case OP_MIN:
                MVT_DESC_MASK(desc,1,class2,TY_DIGIT);
                for (count = 0; count < dim; ++count) {
                  (IPNEW[count]) = (digit_le(IP1[count],IP2[count]) ? (STACKELEM)IP1[count] : IP2[count]);
                  INC_REFCNT((PTR_DESCRIPTOR)(IPNEW[count]));
                }
                break;
           default:
                post_mortem("mvt_mvt_bop: Unkown operation received");
         } /** end switch op **/
         DBUG_RETURN((STACKELEM)desc);

  /*~~~~~~~~~~~~~~~~~*/
    case TY_STRING:
  /*~~~~~~~~~~~~~~~~~*/
         if (R_DESC((*arg2),type) != TY_STRING) goto fail;

         NEWHEAP(dim,A_MVT((*desc),ptdv,class2));

         RES_HEAP;
         switch(op) {
           case OP_EQ:
                for (count = 0; count < dim; ++count)
                  STR_NEW[count] = str_eq(IP1[count],IP2[count]);
                break;
           case OP_NEQ:
                for (count = 0; count < dim; ++count)
                  STR_NEW[count] = !str_eq(IP1[count],IP2[count]);
                break;
           case OP_GT:
                for (count = 0; count < dim; ++count)
                  STR_NEW[count] = !str_le(IP1[count],IP2[count]);
                break;
           case OP_GE:
                for (count = 0; count < dim; ++count)
                  STR_NEW[count] = !str_lt(IP1[count],IP2[count]);
                break;
           case OP_LE:
                for (count = 0; count < dim; ++count)
                  STR_NEW[count] = str_le(IP1[count],IP2[count]);
                break;
           case OP_LT:
                for (count = 0; count < dim; ++count)
                  STR_NEW[count] = str_lt(IP1[count],IP2[count]);
                break;
           case OP_MAX:
                MVT_DESC_MASK(desc,1,class2,TY_STRING);
                for (count = 0; count < dim; ++count)
                {
                  STR_NEW[count] = (str_le(IP1[count],IP2[count]) ? IP2[count] : (STACKELEM)IP1[count]);
                  INC_REFCNT((PTR_DESCRIPTOR)STR_NEW[count]);
                }
                break;
           case OP_MIN:
                MVT_DESC_MASK(desc,1,class2,TY_STRING);
                for (count = 0; count < dim; ++count)
                {
                  STR_NEW[count] = (str_le(IP1[count],IP2[count]) ? (STACKELEM)IP1[count] : IP2[count]);
                  INC_REFCNT((PTR_DESCRIPTOR)STR_NEW[count]);
                }
                break;
           default:
                REL_HEAP;
                post_mortem("mvt_mvt_bop: Unkown operation received");
         } /** end switch op **/
         REL_HEAP;
         DBUG_RETURN((STACKELEM)desc);

  /*~~~~~~~~~~~~~~~~~*/
    case TY_BOOL:
  /*~~~~~~~~~~~~~~~~~*/
         if (R_DESC((*desc),type) != TY_BOOL) goto fail;
         NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
         RES_HEAP;
         ip1   = (BOOLEAN *) R_MVT((*arg1),ptdv,class1);
         ip2   = (BOOLEAN *) R_MVT((*arg2),ptdv,class2);
         ipnew = (BOOLEAN *) R_MVT((*desc),ptdv,class2);

         /* Der Vergleich zweier Boolmatrizen ist nicht so ganz einfach, */
         /* da der true-Wert nicht eindeutig ist!!!!!!                   */

         switch(op) {
           case OP_EQ:
                for (count = 0; count < dim; ++count)
                  ipnew[count] = ( ip1[count] ? ip2[count] : !ip2[count]);
                break;
           case OP_NEQ:
                for (count = 0; count < dim; ++count)
                  ipnew[count] = ( ip1[count] ? !ip2[count] : ip2[count]);
                break;
           case OP_GT:
                for (count = 0; count < dim; ++count)
                  ipnew[count] = ( ip1[count] ? !ip2[count] : FALSE );
                break;
           case OP_GE:
                for (count = 0; count < dim; ++count)
                  ipnew[count] = ( ip1[count] ? TRUE : !ip2[count] );
                break;
           case OP_LE:
                for (count = 0; count < dim; ++count)
                  ipnew[count] = (ip1[count] ?  ip2[count] : TRUE );
                break;
           case OP_LT:
                for (count = 0; count < dim; ++count)
                  ipnew[count] = ( ip1[count] ? FALSE : ip2[count] );
                break;
           case OP_MAX:
                MVT_DESC_MASK(desc,1,class2,TY_BOOL);
                for (count = 0; count < dim; ++count)
                  ipnew[count] = ( ip2[count] ? ip2[count] : ip1[count]);
                break;
           case OP_MIN:
                MVT_DESC_MASK(desc,1,class2,TY_BOOL);
                for (count = 0; count < dim; ++count)
                  ipnew[count] = ( ip2[count] ? ip1[count] : ip2[count]);
                break;
           default:
                post_mortem("mvt_mvt_bop: Unknown operation received");
         } /** switch op **/
         REL_HEAP;
         DBUG_RETURN((STACKELEM)desc);
  } /** end switch type arg1 **/
fail:
  DEC_REFCNT(desc);
  DBUG_RETURN(0);
} /**** end of function mvt_mvt_bop ****/


/*****************************************************************************/
/*                              bmvt_bmvt_bop                                */
/* ------------------------------------------------------------------------- */
/* function  : Zwei Matrizen / Vektoren / TVektoren des Typs Boolean         */
/*             werden durch or,and, oder xor verglichen.                     */
/*             Die Funktion  liefert, falls nicht anwendbar, eine Null       */
/*             zurueck.                                                      */
/* paramteter: desc1 --  matrix- / vector- / tvector-descriptor              */
/*             desc2 --  matrix- / vector- / tvector-descriptor              */
/*             op    --  compare operator                                    */
/* returns   : stackelement                                                  */
/* called by : red_or, red_and, red_xor                                      */
/* calls     : none                                                          */
/*****************************************************************************/
STACKELEM bmvt_bmvt_bop(desc1,desc2,op)
PTR_DESCRIPTOR desc1,desc2; /* Matrizen / Vektoren/TVektoren vom Typ Boolean */
int  op;                    /* OP_OR, OP_AND, OP_XOR                         */
{
  register int i=0,dim=0;   /* Initialisierung von RS 6.11.1992 */
  register int *p1,*p2,*pnew;
  PTR_DESCRIPTOR desc;

  char   class1,class2;                     /* fuer die Klasse der Argumente */

  DBUG_ENTER ("bmvt_bmvt_bop");

  class1 = R_DESC((*desc1),class);
  class2 = R_DESC((*desc2),class);

  if (class1 !=class2)
     DBUG_RETURN(0);

  switch(class2) {
    case  C_MATRIX : if (((dim   = R_MVT((*desc1),nrows,class1)) !=
                                   R_MVT((*desc2),nrows,class2)) ||
                         ((i     = R_MVT((*desc1),ncols,class1)) !=
                                   R_MVT((*desc2),ncols,class2)))
                       /* wenn nicht gleiche Zeilenanzahl bzw. Spaltenanzahl */
                       DBUG_RETURN(0);
                     break;
    case  C_VECTOR : if ((i     = R_MVT((*desc1),ncols,class1)) !=
                                  R_MVT((*desc2),ncols,class2))
                       DBUG_RETURN(0);                /* nicht dieselbe Dimension */
                     dim   = 1;
                     break;
    case  C_TVECTOR: if ((dim   = R_MVT((*desc1),nrows,class1)) !=
                                  R_MVT((*desc2),nrows,class2))
                       DBUG_RETURN(0);                /* nicht dieselbe Dimension */
                     i     = 1;
                     break;
  } /** switch class2 **/

  NEWDESC(_desc); INIT_DESC;
  MVT_DESC_MASK(desc,1,class2,TY_BOOL);
  L_MVT((*desc),nrows,class2) = dim;
  L_MVT((*desc),ncols,class2) =  i ;
  dim *= i;
  NEWHEAP(dim,A_MVT((*desc),ptdv,class2));
  RES_HEAP;
  pnew = (int *) R_MVT((*desc),ptdv,class2);
  p1   = (int *) R_MVT((*desc1),ptdv,class2);
  p2   = (int *) R_MVT((*desc2),ptdv,class2);

  switch(op) {
    case OP_OR:
           for (i = 0; i < dim; i++ )
             pnew[i] = p1[i] || p2[i];
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);
    case OP_AND:
           for (i = 0; i < dim; i++ )
             pnew[i] = p1[i] && p2[i];
           REL_HEAP;
           DBUG_RETURN((STACKELEM)desc);
    case OP_XOR:
           /* Beachte: ein Vergleich mittels '!=' ist nicht moeglich, */
           /* da true keinen eindeutigen Wert hat.                    */
           for (i = 0; i < dim; i++ )
             pnew[i] = (p1[i] ? !p2[i] : p2[i]);
           REL_HEAP;
           
    default: post_mortem("bmvt_bmvt_bop: Unknown operation");
  } /** end switch op **/
 DBUG_RETURN ((STACKELEM) 0 );                /* ach 30/10/92 */
} /**** end of function bmvt_bmvt_bop ****/

/*******************  end of file rlog-mvt.c  ********************************/
