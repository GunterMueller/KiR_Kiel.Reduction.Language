/* $Log: rlogfunc.c,v $
 * Revision 1.2  1992/12/16  12:50:11  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*****************************************************************************/
/*                        MODULE FOR PROCESSING PHASE                        */
/* ------------------------------------------------------------------------- */
/* rlogfunc.c -- external  : red_eq,    red_lt,    red_le,    red_min,       */
/*                           red_f_eq,  red_not,   red_or,    red_and,       */
/*                           red_xor,                                        */
/*                           str_eq,    str_le,    str_lt,  pm_element       */
/*                                                                           */
/* ========================================================================= */
/*              Kurze Beschreibung der Aufgaben der Funktionen               */
/*                                                                           */
/*  Alle externen Funktionen bis auf red_f_eq und red_eq sind                */
/*  'scalar erweitert', dh. es ist moeglich Scalare und Matrizen             */
/*  miteinander zu verknuepfen. Als Ergebnis entstehen dann Matrizen,        */
/*  die mit Ausnahme der Funktionen red_min vom Typ boolean sind.            */
/*  analog mit Vektoren und TVektoren.                                       */
/*                                                                           */
/*  red_...       Modul fuer                                                 */
/*  -------      -----------                                                 */
/*  eq           eq und ne                                                   */
/*  lt           <  und >=                                                   */
/*  le           <=  und >                                                   */
/*  min          min und max                                                 */
/*  not          not                                                         */
/*  and          and                                                         */
/*  xor          exclusive or                                                */
/*  f_eq         f_eq und f_ne                                               */
/*                                                                           */
/*  Aufrufhierachie der Untermodule :                                        */
/*  ======================================================================== */
/*  m = matrix     v = vector    t = tvector   <===>  mvt                    */
/*                                                                           */
/*  Untermodul:                aufgerufen durch ...                          */
/*  ----------------------     ------------------------------------          */
/*  boolmvt_bop                bool_mvt_bop,  red_or,       red_and,         */
/*                             red_xor,       red_not                        */
/*  bool_mvt_bop               red_lt,        red_le,       red_min          */
/*  bmvt_bmvt_bop              red_or,        red_and,      red_xor          */
/*  scalar_mvt_bop             red_lt,        red_le,       red_min          */
/*  digit_mvt_bop              red_lt,        red_le,       red_min          */
/*  mvt_mvt_bop                red_lt,        red_le,       red_min          */
/*  str_mvt_bop                red_lt,        red_le,       red_min          */
/*                                                                           */
/*  str_eq                     red_eq,        str_mvt_bop,  mvt_mvt_bop      */
/*                             red_f_eq,      pm_red_f_eq,  in pam.c         */
/*                                            pm_element    in pam.c         */
/*  str_le                     red_le,        str_mvt_bop,  mvt_mvt_bop      */
/*  str_lt                     red_lt,        str_mvt_bop,  mvt_mvt_bop      */
/*                                                                           */
/*****************************************************************************/

#include "dbug.h"

#include "rstdinc.h"
#if nCUBE
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


#if D_MESS
#include "d_mess_io.h"
#endif

#if nCUBE
#define READ_H()    D_READSTACK(D_S_H)
#else
#define READ_H()    READSTACK(S_e)
#endif

/*--------------------------------------------------------------------------*/
/*                             extern variables                             */
/*--------------------------------------------------------------------------*/
extern jmp_buf _interruptbuf;       /* <r>, Adresse fuer Ausnahmebehandlung */
extern PTR_DESCRIPTOR _desc;        /* <w>, Variable fuer Ergebnisdeskriptor*/

/*--------------------------------------------------------------------------*/
/*                             extern functions                             */
/*--------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR newdesc();          /* rheap.c    */
extern int       newheap();               /* rheap.c    */
extern BOOLEAN   digit_eq();              /* rdig-v1.c  */
extern BOOLEAN   digit_le();              /* rdig-v1.c  */
extern BOOLEAN   digit_lt();              /* rdig-v1.c  */
extern STACKELEM boolmvt_bop();           /* rlog-mvt.c */
extern STACKELEM bool_mvt_bop();          /* rlog-mvt.c */
extern STACKELEM scalar_mvt_bop();        /* rlog-mvt.c */
extern STACKELEM digit_mvt_bop();         /* rlog-mvt.c */
extern STACKELEM str_mvt_bop();           /* rlog-mvt.c */
extern STACKELEM mvt_mvt_bop();           /* rlog-mvt.c */
extern STACKELEM bmvt_bmvt_bop();         /* rlog-mvt.c */

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

/* RS 30/10/92 */ 
BOOLEAN str_eq();
extern void freedesc();                   /* rheap.c */
/* END of RS 30/10/92 */ 
                
/*--------------------------------------------------------------------------*/
/*                             defines                                      */
/*--------------------------------------------------------------------------*/
/* ------- macros for heapoverflow - handling ---------- */
/* Ein Interrupt bewirkt die Zuruecknahme der Reduktion. */
/* siehe setjmp(_interruptbuf) in der Datei rear.c       */
#if (D_MESS && D_HEAP)
#define NEWHEAP(size,adr) if ((*d_m_newheap)(size,adr) == 0) \
                            longjmp(_interruptbuf,0)
#else
#define NEWHEAP(size,adr) if (newheap(size,adr) == 0) \
                            longjmp(_interruptbuf,0)
#endif
#define INIT_DESC         if ((desc = _desc) == 0)    \
                            longjmp(_interruptbuf,0)

#define TEST(value)       if ((value) == 0) goto fail;

#define DESC(x)  (* (PTR_DESCRIPTOR) x)


/*****************************************************************************/
/*             pm_element   (is char element of string (i.e.set)?)           */
/* ------------------------------------------------------------------------- */
/* function  :  examine whether "char" is element of "set" or not;           */
/* parameter :  pm_char  - character i.e. element of a string                */
/*              set      - a set of characters represented by a string       */
/* returns   :  boolean                                                      */
/* calls     :  str_eq         (case string of string(s))                    */
/* called by :  pm_am          (in ram.c)                                    */
/*****************************************************************************/
BOOLEAN pm_element(pm_char,set)
  T_HEAPELEM pm_char;
  PTR_DESCRIPTOR set;
{
  PTR_HEAPELEM ptr,
               end_of_set;
  int feature = 0;

  DBUG_ENTER ("pm_element");

  START_MODUL("pm_element");

  ptr = R_LIST(DESC(set),ptdv);
  end_of_set = ptr + R_LIST(DESC(set),dim);

  if (   (!T_POINTER(*ptr)) && ((VALUE(*ptr)) == '\\')   )
  {
      ptr++;                  /* first char at this position */
      feature = 1;            /* matching for complement     */
  }

  switch (feature)
  {
  case 0:                                       /* pm_char has do be in set */
      if (T_POINTER(pm_char))
          for ( ; ptr < end_of_set; ptr++)
          {
              if (T_POINTER(*ptr))
                  if (str_eq(pm_char,*ptr))        /* new defined character */
                  {
                      goto success;
                  }
          }
      else
          for ( ; ptr < end_of_set; ptr++)
          {
              if (!(T_POINTER(*ptr)))
                  if ((*ptr & ~F_EDIT) == (pm_char & ~F_EDIT))
                  {
                      goto success;
                  }
          }
       goto fail;
  case 1:                                     /* pm_char must not be in set */
      if (T_POINTER(pm_char))
          for ( ; ptr < end_of_set; ptr++)
          {
              if (T_POINTER(*ptr))
                  if (str_eq(pm_char,*ptr))        /* new defined character */
                  {
                      goto fail;
                  }
          }
      else
          for ( ; ptr < end_of_set; ptr++)
          {
              if (!(T_POINTER(*ptr)))
                  if ((*ptr & ~F_EDIT) == (pm_char & ~F_EDIT))
                  {
                      goto fail;
                  }
          }
      goto success;
  }
  fail:
      END_MODUL(pm_element);
      DBUG_RETURN(FALSE);
  success:
      END_MODUL("pm_element");
      DBUG_RETURN(TRUE);    
} /**** end of function pm_element ****/



/****************************************************************************/
/*                   str_eq  (compare function for strings)                 */
/* ------------------------------------------------------------------------ */
/* function    :  Zwei Descriptoren der Klasse C_LIST und Typ TY_STRING     */
/*                werden auf Gleichheit getestet.                           */
/* parameter   :  str1  --  pointer to descriptor of first argument         */
/*                str2  --  pointer to descriptor of second argument        */
/* returns     :  true or false                                             */
/* called by   :  red_eq, red_f_eq, str_mvt_bop,  mvt_mvt_bop               */
/*                pm_red_f_eq  and in ram.c                                 */
/* calls       :  str_eq                                                    */
/****************************************************************************/
BOOLEAN str_eq(str1,str2)
PTR_DESCRIPTOR str1,str2;
{
  int i,dim,
   *p1,*p2;    

  DBUG_ENTER ("str_eq");

  if ( (dim = R_LIST((*str1),dim)) != R_LIST((*str2),dim))
    DBUG_RETURN(FALSE);                                    /* no equal dimensions */

  p1 = R_LIST((*str1),ptdv);
  p2 = R_LIST((*str2),ptdv);

  for (i = 0; i < dim; i++) {         /* comparison of data-vectors          */
    if ((T_POINTER(p1[i]) && (!T_POINTER(p2[i]))) ||
        ((!T_POINTER(p1[i])) && T_POINTER(p2[i]))) {
      DBUG_RETURN(FALSE);                  /* combination of a pointer and a char */
    }
    else {
      if ((!T_POINTER(p1[i])) && (!T_POINTER(p2[i]))) {
        if ((p1[i] & ~F_EDIT) != (p2[i] & ~F_EDIT))   /* char's not equal    */
          DBUG_RETURN(FALSE);
      }
      else {
        if (R_LIST(DESC(p1[i]),dim) != R_LIST(DESC(p2[i]),dim)) {
          DBUG_RETURN(FALSE);                              /* no equal dimensions */
        }
        else {
           if (!str_eq((PTR_DESCRIPTOR)p1[i],(PTR_DESCRIPTOR)p2[i]))/*recursive call if there are tow pointer*/      /*(PTR_DESCRIPTOR) eingefuegt RS 1.12.1992 */
             DBUG_RETURN(FALSE);
        }
      } /** end else pointer pointer **/
    }
  } /** end for **/
  DBUG_RETURN(TRUE);                                       /* Strings are equal   */

} /**** end of function str_eq ****/

/****************************************************************************/
/*                   str_le  (compare function for strings)                 */
/* ------------------------------------------------------------------------ */
/* function    :  Zwei Descriptoren der Klasse C_LIST und Typ TY_STRING     */
/*                werden auf  '<='  getestet.                               */
/* parameter   :  str1  --  pointer to descriptor of first argument         */
/*                str2  --  pointer to descriptor of second argument        */
/* returns     :  true or false                                             */
/* called by   :  red_le, str_mvt_bop,  mvt_mvt_bop                         */
/* calls       :  str_le                                                    */
/****************************************************************************/
BOOLEAN str_le(str1,str2)
PTR_DESCRIPTOR str1,str2;
{
  int dim,dim1,i;
  int *p1,*p2;

  DBUG_ENTER ("str_le");

  dim1 = R_LIST((*str1),dim);
  dim  = R_LIST((*str2),dim);

  if (dim1 < dim)
    dim = dim1;                    /* "minimum"  of the strings to compare   */

  p1 = R_LIST((*str1),ptdv);
  p2 = R_LIST((*str2),ptdv);

  for (i = 0; i < dim; i++) {      /* comparison of data-vector until dim    */
                                   /* is reached                             */
    if ((!T_POINTER(p1[i])) && (!T_POINTER(p2[i]))) {
      if ((p1[i] & ~F_EDIT) < (p2[i] & ~F_EDIT))
        DBUG_RETURN(TRUE);              /* char1 is le char2                      */
      else
        if ((p1[i] & ~F_EDIT) > (p2[i] & ~F_EDIT))
          DBUG_RETURN(FALSE);           /* char1 is gt char2                      */
    }
    else {
      if (T_POINTER(p1[i]) && T_POINTER(p2[i])) {
        if (!str_le((PTR_DESCRIPTOR)p1[i],(PTR_DESCRIPTOR)p2[i]))  /* recursive call if there are tow pointer*/      /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
          DBUG_RETURN(FALSE);
      }
      else {
        if (T_POINTER(p1[i]) && (!T_POINTER(p2[i])))
          DBUG_RETURN(FALSE);           /* combination of a pointer and a char    */
        else
          DBUG_RETURN(TRUE);            /* combination of a char and a pointer    */
      }
    }
  } /** end for **/

  if (dim == dim1)
    DBUG_RETURN(TRUE);                  /* string1 is less equal than string2     */
  else
    DBUG_RETURN(FALSE);

} /**** end of function str_le ****/

/****************************************************************************/
/*                   str_lt  (compare function for strings)                 */
/* ------------------------------------------------------------------------ */
/* function    :  Zwei Descriptoren der Klasse C_LIST und Typ TY_STRING     */
/*                werden auf  '<'  getestet.                                */
/* parameter   :  str1  --  pointer to descriptor of first argument         */
/*                str2  --  pointer to descriptor of second argument        */
/* returns     :  true or false                                             */
/* called by   :  red_lt, str_mvt_bop,  mvt_mvt_bop                         */
/* calls       :  str_lt                                                    */
/****************************************************************************/
BOOLEAN str_lt(str1,str2)
PTR_DESCRIPTOR str1,str2;
{
  int dim,dim2,i;
  int *p1,*p2;

  DBUG_ENTER ("str_lt");

  dim  = R_LIST((*str1),dim);
  dim2 = R_LIST((*str2),dim);

  if (dim2 < dim)
    dim = dim2;                    /* "minimum" of strings to compare        */

  p1 = R_LIST((*str1),ptdv);
  p2 = R_LIST((*str2),ptdv);

  for (i = 0; i < dim; i++) {      /* comparison of data-vector until dim    */
                                   /* is reached                             */
    if ((!T_POINTER(p1[i])) && (!T_POINTER(p2[i]))) {
      if ((p1[i] & ~F_EDIT) < (p2[i] & ~F_EDIT))
        DBUG_RETURN(TRUE);              /* char1 is le char2                      */
      else
        if ((p1[i] & ~F_EDIT) > (p2[i] & ~F_EDIT))
          DBUG_RETURN(FALSE);           /* char1 is gt char2                      */
    }
    else {
      if (T_POINTER(p1[i]) && T_POINTER(p2[i])) {
        if (str_lt((PTR_DESCRIPTOR)p1[i],(PTR_DESCRIPTOR)p2[i]))   /* recursive call if there are two pointer*/     /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
          DBUG_RETURN(TRUE);
        else
          if (str_lt((PTR_DESCRIPTOR)p2[i],(PTR_DESCRIPTOR)p1[i])) /* recursive call if there are two pointer*/      /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
            DBUG_RETURN(FALSE);
      }
      else {
        if (T_POINTER(p1[i]) && (!T_POINTER(p2[i])))
          DBUG_RETURN(FALSE);           /* combination of a pointer and a char    */
        else
          DBUG_RETURN(TRUE);            /* combination of a char and a pointer    */
      }
    }
  } /** end for **/

  if (dim < dim2)
    DBUG_RETURN(TRUE);                  /* string1 is less than string2           */
  else
    DBUG_RETURN(FALSE);

} /**** end of function str_lt ****/


/*****************************************************************************/
/*              red_not   (logical function with boolean argument)           */
/* ------------------------------------------------------------------------- */
/* function :   not ist anwendbar auf Argumente vom Typ Bool.                */
/*              Auf Matrizen, Vektoren, TVektoren vom Typ Bool               */
/*              wird NOT elementweise angewendet.                            */
/*                                                                           */
/*                     Bool              -->   Bool                          */
/*                     bmat              -->   bmat                          */
/*                     bvect             -->   bvect                         */
/*                     btvect            -->   btvect                        */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/* called by:  rear.c                                                        */
/* calls    :  boolmvt_bop.                                                  */
/*****************************************************************************/
int red_not(arg1)         /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1;
{
  char class1;

  DBUG_ENTER ("red_not");

  START_MODUL("red_not");

  if T_BOOLEAN(arg1) {
    _desc = (PTR_DESCRIPTOR) (T_SA_FALSE(arg1) ? SA_TRUE : SA_FALSE);
    END_MODUL("red_not");
    DBUG_RETURN(1);
  }

  if (T_POINTER(arg1) && ( ((class1 = R_DESC(DESC(arg1),class)) == C_MATRIX) ||
                            (class1 == C_VECTOR) ||
                            (class1 == C_TVECTOR))
                      && (R_DESC(DESC(arg1),type) == TY_BOOL) ) {

    _desc = (PTR_DESCRIPTOR) boolmvt_bop(arg1,OP_NOT);
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_not");
    DBUG_RETURN(1);
  }
  END_MODUL("red_not");
  DBUG_RETURN(0);
} /**** end of function red_not ****/


/*****************************************************************************/
/*             red_or    (logical function with boolean arguments)           */
/* ------------------------------------------------------------------------- */
/* function :  Diese Funktionen sind nur auf Argumente vom Typ Bool          */
/*             anwendbar, also auch auf Matrizen, Vektoren und TVektoren     */
/*             vom Typ Bool. Diese duerfen auch mit true oder verknuepft     */
/*             werden. Werden Matrizen mit Matrizen verknuepft, so muessen   */
/*             beide Matrizen die gleiche Zeilen und Spaltenzahl haben.      */
/*                                                                           */
/*                     bmat   x  bmat    -->   bmat                          */
/*                     Bool   x  bmat    -->   bmat                          */
/*                     bmat   x  Bool    -->   bmat                          */
/*                     bvect  x  bvect   -->   bvect                         */
/*                     Bool   x  bvect   -->   bvect                         */
/*                     bvect  x  Bool    -->   bvect                         */
/*                     btvect x  btvect  -->   btvect                        */
/*                     Bool   x  btvect  -->   btvect                        */
/*                     btvect x  Bool    -->   btvect                        */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/* called by:  rear.c                                                        */
/* calls    :  boolmvt_bop, bmvt_bmvt_bop.                                   */
/*****************************************************************************/
int red_or(arg1,arg2)                  /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

  DBUG_ENTER ("red_or");

  START_MODUL("red_or");

  if T_BOOLEAN(arg1) {
    if T_BOOLEAN(arg2) {
      /* zwei Booleans */
      _desc = (PTR_DESCRIPTOR) (T_SA_TRUE(arg1) || T_SA_TRUE(arg2)
                                                      ? SA_TRUE : SA_FALSE);
      goto success;
    }

    if (T_POINTER(arg2) && ( ((class2 = R_DESC(DESC(arg2),class)) == C_MATRIX) ||
                              (class2 == C_VECTOR) ||
                              (class2 == C_TVECTOR))
                        && (R_DESC(DESC(arg2),type) == TY_BOOL) ) {
                                         /* arg1 ist ein Boolean, arg2 ein   */
                                         /* Deskriptor fuer ein Boolmvt      */
      if T_SA_FALSE(arg1) {
        _desc = (PTR_DESCRIPTOR) arg2;   /* die Identiaetsoperation          */
        goto success;
      }                                  /* es entsteht ein Boolmvt nur mit  */
                                         /* true's besetzt.                  */
      _desc = (PTR_DESCRIPTOR) boolmvt_bop(arg2,OP_TRUE);
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      goto success;
    }
    goto fail;
  } /** T_BOOLEAN(arg1) **/

  if (T_POINTER(arg1) && ( ((class1 = R_DESC(DESC(arg1),class)) == C_MATRIX) ||
                            (class1 == C_VECTOR) ||
                            (class1 == C_TVECTOR))
                      && (R_DESC(DESC(arg1),type) == TY_BOOL) ) {

    if T_BOOLEAN(arg2) {
      if T_SA_FALSE(arg2) {              /* arg1 ist ein Deskriptor fuer    */
                                         /* ein Boolmvt, arg2 ein Boolean.  */
        _desc = (PTR_DESCRIPTOR) arg1;
        goto success;
      }                                  /* es entsteht ein Boolmvt nur     */
                                         /* besetzt mit true                */
      _desc = (PTR_DESCRIPTOR) boolmvt_bop(arg1,OP_TRUE);
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      goto success;
    } /** T_BOOLEAN(arg2) **/

    if (T_POINTER(arg2)
        && ( ((class2 = R_DESC(DESC(arg2),class)) == C_MATRIX) ||
              (class2 == C_VECTOR) ||
              (class2 == C_TVECTOR))
        && (R_DESC(DESC(arg2),type) == TY_BOOL)
        && (result = bmvt_bmvt_bop(arg1,arg2,OP_OR))) {

      _desc = (PTR_DESCRIPTOR) result;   /* zwei Boolmvt wurden erfolgreich  */
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);  /* durch oder verknuepft.           */
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      goto success;
    }
  }
fail:
  END_MODUL("red_or");DBUG_RETURN(0);

success:
  END_MODUL("red_or");
  DBUG_RETURN(1);
} /**** end of function red_or ****/


/*****************************************************************************/
/*             red_and   (logical function with boolean arguments)           */
/* ------------------------------------------------------------------------- */
/* function :  zwei Argumente sollen mittels des logischen AND               */
/*             verknuepft werden.  sea further:  red_or                      */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/* called by:  rear.c                                                        */
/* calls    :  boolmvt_bop, bmvt_bmvt_bop                                    */
/*****************************************************************************/
int red_and(arg1,arg2)                    /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

  DBUG_ENTER ("red_and");

  START_MODUL("red_and");

  if T_BOOLEAN(arg1) {
    if T_BOOLEAN(arg2) {
      _desc = (PTR_DESCRIPTOR) (T_SA_TRUE(arg1) && T_SA_TRUE(arg2)
                                                      ? SA_TRUE : SA_FALSE);
      goto success;
    }

    if (T_POINTER(arg2) && ( ((class2 = R_DESC(DESC(arg2),class)) == C_MATRIX) ||
                              (class2 == C_VECTOR) ||
                              (class2 == C_TVECTOR))
                        && (R_DESC(DESC(arg2),type) == TY_BOOL) ) {

      if T_SA_TRUE(arg1) {
        _desc = (PTR_DESCRIPTOR) arg2;
        goto success;
      }
      _desc = (PTR_DESCRIPTOR) boolmvt_bop(arg2,OP_FALSE);
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      goto success;
    }
    goto fail;
  }

  if (T_POINTER(arg1) && ( ((class1 = R_DESC(DESC(arg1),class)) == C_MATRIX) ||
                            (class1 == C_VECTOR) ||
                            (class1 == C_TVECTOR))
                      && (R_DESC(DESC(arg1),type) == TY_BOOL) ) {

    if T_BOOLEAN(arg2) {
      if T_SA_TRUE(arg2) {
        _desc = (PTR_DESCRIPTOR) arg1;
        goto success;
      }
      _desc = (PTR_DESCRIPTOR) boolmvt_bop(arg1,OP_FALSE);
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      goto success;
    }

    if (T_POINTER(arg2)
        && ( ((class2 = R_DESC(DESC(arg2),class)) == C_MATRIX) ||
              (class2 == C_VECTOR) ||
              (class2 == C_TVECTOR))
        && (R_DESC(DESC(arg2),type) == TY_BOOL)
        && (result = bmvt_bmvt_bop(arg1,arg2,OP_AND))) {

      _desc = (PTR_DESCRIPTOR) result;
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      goto success;
    }
  }
fail:
  END_MODUL("red_and");
  DBUG_RETURN(0);
success:
  END_MODUL("red_and");
  DBUG_RETURN(1);
} /**** end of function red_and ****/


/*****************************************************************************/
/*             red_xor   (logical function with boolean arguments)           */
/* ------------------------------------------------------------------------- */
/* function :  zwei Argumente sollen durch ein logisches exklusives ODER     */
/*             verknuepft werden.  sea further:  red_or                      */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/* called by:  rear.c                                                        */
/* calls    :  boolmvt_bop, bmvt_bmvt_bop                                    */
/*****************************************************************************/
int red_xor(arg1,arg2)                    /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

  DBUG_ENTER ("red_xor"); 

  START_MODUL("red_xor");

  if T_BOOLEAN(arg1) {
    if T_BOOLEAN(arg2) {
      _desc = (PTR_DESCRIPTOR) (T_SA_TRUE(arg1) != T_SA_TRUE(arg2)
                                                      ? SA_TRUE : SA_FALSE);
      goto success;
    }
    if (T_POINTER(arg2) && ( ((class2 = R_DESC(DESC(arg2),class)) == C_MATRIX) ||
                              (class2 == C_VECTOR) ||
                              (class2 == C_TVECTOR))
                        && (R_DESC(DESC(arg2),type) == TY_BOOL) ) {

      if T_SA_FALSE(arg1) {
        _desc = (PTR_DESCRIPTOR) arg2;
        goto success;
      }
      _desc = (PTR_DESCRIPTOR) boolmvt_bop(arg2,OP_NOT);
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      goto success;
    }
    goto fail;
  } /** T_BOOLEAN(arg1) **/

  if (T_POINTER(arg1) && ( ((class1 = R_DESC(DESC(arg1),class)) == C_MATRIX) ||
                            (class1 == C_VECTOR) ||
                            (class1 == C_TVECTOR))
                      && (R_DESC(DESC(arg1),type) == TY_BOOL) ) {

    if T_BOOLEAN(arg2) {
      if T_SA_FALSE(arg2) {
        _desc = (PTR_DESCRIPTOR) arg1;
        goto success;
      }

      _desc = (PTR_DESCRIPTOR) boolmvt_bop(arg1,OP_NOT);
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      goto success;
    }

    if (T_POINTER(arg2)
        && ( ((class2 = R_DESC(DESC(arg2),class)) == C_MATRIX) ||
              (class2 == C_VECTOR) ||
              (class2 == C_TVECTOR))
        && (R_DESC(DESC(arg2),type) == TY_BOOL)
        && (result = bmvt_bmvt_bop(arg1,arg2,OP_XOR))) {

      _desc = (PTR_DESCRIPTOR) result;
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      goto success;
    } /** T_BOOLEAN(arg2) **/
  }
fail:
  END_MODUL("red_xor");DBUG_RETURN(0);

success:
  END_MODUL("red_xor");
  DBUG_RETURN(1);
} /**** end of function red_xor ****/


/*****************************************************************************/
/*                        red_lt    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function : Vergleicht zwei Stackelemente bzw. Descriptoren                */
/*            auf '<' und '>='.                                              */
/*                                                                           */
/*            further:                                                       */
/*            Diese Funktionen vergleichen Datenobjekte vom Typ Bool,        */
/*            Decimal und String. Die Anwendung dieser Funktion auf einen    */
/*            Skalar und eine Matrix gleichen Typs ist erlaubt.              */
/*            Es entsteht dann eine Matrix vom Typ Bool, dessen              */
/*            Elemente aus der Verknuepfung des Skalars mit den einzelnen    */
/*            Matrizenelementen hervorgehen. Bei dem Vergleich zweier        */
/*            Matrizen muessen beide die gleiche Zeilen- und Spaltenzahl     */
/*            haben. analog mit Vektor und TVektor.                          */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement (pointer to result descriptor)  (true / false)   */
/* called by:  rear.c                                                        */
/* calls    :  scalar_mvt_bop,  digit_mvt_bop,                               */
/*             bool_mvt_bop,    mvt_mvt_bop,                                 */
/*             str_lt,          str_mvt_bop                                  */
/*****************************************************************************/
int red_lt(arg1,arg2)                        /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
/*char class1,class2;                         RS 30/10/92 */ 

  DBUG_ENTER ("red_lt");

  START_MODUL("red_lt");

  if (T_INT(arg1))
     { 								/* INT < ? */
      if (T_INT(arg2))
	 { 							/* INT < INT */
	  result = LT_INT(arg1,arg2) ;
          goto success ;
         }
	 else
	 { 							/* INT < ~INT */
          if (T_POINTER(arg2))
	     { 							/* INT < DESCRIPTOR */
	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT < SCALAR-DESC */
		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT < REAL */
                      result = (VAL_INT(arg1) < R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                      goto scalar_success_int1 ;
                     }
                     else
                     { 						/* INT < UNDEF */
                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT < ~SCALAR */
                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {					/* INT < MVT */
		             int help ;
		             help = VAL_INT(arg1) ;
#if (nCUBE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1) ;
#else
		             arg1 = (STACKELEM)newdesc(); TEST(arg1) ; 
#endif
		             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;

                            }
			    break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {
              goto fail ;
             }
	 } 
     }
     else
     { 								/* ~INT < ? */ 
      if ( T_BOOLEAN(arg1) )
	 {							/* BOOLEAN < ? */
	  if ( T_BOOLEAN(arg2) )
	     {							/* BOOLEAN < BOOLEAN */
              result = (T_SA_FALSE(arg1) ? arg2 : SA_FALSE);
	      goto success ;
	     }
	     else
	     {							/* BOOLEAN < ~BOOLEAN */
              if (T_POINTER(arg2))
	         { 						/* BOOLEAN < DESCRIPTOR */
                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                            {					/* BOOLEAN < MVT */
			     if (R_DESC(DESC(arg2),type) == TY_BOOL)
				{				/* BOOLEAN < BMVT */
    				 TEST(result = bool_mvt_bop(arg1,arg2,OP_LT));
    				 DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    				 goto success;
				}
				else
				{				/* BOOLEAN < ~BMVT */
              			 goto fail ;
				}
                            }
			/*    break ;     RS 30.11.1992 */     
                      default: goto fail;
                     }
		 }
		 else
		 {						/* BOOLEAN < ~DESCRIPTOR */
		  goto fail ;
		 }
	     }
	 }
	 else
	 {							/* ~BOOLEAN < ? */
	  if ( T_BOOLEAN(arg2) )
	     {							/* ~BOOLEAN < BOOLEAN */
              if (T_POINTER(arg1))
	         { 						/* DESCRIPTOR < BOOLEAN */
                  switch(R_DESC(DESC(arg1),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                            {					/* MVT < BOOLEAN */
			     if (R_DESC(DESC(arg1),type) == TY_BOOL)
				{				/* BMVT < BOOLEAN */
    				 TEST(result = bool_mvt_bop(arg2,arg1,OP_GT));
    				 DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    				 goto success;
				}
				else
				{				/* ~BMVT < BOOLEAN */
              			 goto fail ;
				}
                            }
			    /* break ;      RS 30.11.1992 */
                      default: goto fail;
                     }
		 }
		 else
		 {						/* ~DESCRIPTOR < BOOLEAN */
		  goto fail ;
		 }
	     }
	     else
	     {							/* ~INT,~BOOLEAN < ? */
      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR < ? */
          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC < ? */
              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL < ? */
                  if (T_INT(arg2))
                     {						/* REAL < INT */
                      result = (R_SCALAR(DESC(arg1),valr) < VAL_INT(arg2)
                                ? SA_TRUE : SA_FALSE);
                      goto scalar_success_int2 ;
                     }
                     else
		     { 						/* REAL < ~INT */
                      if (R_DESC(DESC(arg2),type) == TY_REAL)
                         { 					/* REAL < REAL */
                          result = (R_SCALAR(DESC(arg1),valr) < R_SCALAR(DESC(arg2),valr)
                                    ? SA_TRUE : SA_FALSE);
                          goto decrement ;
                         }
		     }
	         }
		 else
                 { 						/* UNDEF < ? */
                  goto fail ;
		 }
             }
             else
	     { 							/* ~SCALAR < ? */
	      if (T_INT(arg2))
		 { 						/* ~SCALAR < INT */
		  int help ;
		  help = VAL_INT(arg2) ;
#if (nCUBE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2) ;
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2) ; 
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF < ? */
	  goto fail ;
         }
	     }
	 }
     }


  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    switch(R_DESC(DESC(arg1),class)) {

      case C_SCALAR:
           switch(R_DESC(DESC(arg2),class)) {
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,OP_LT));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_SCALAR **/

      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class)) {
             case C_DIGIT:
                  result = (digit_lt(arg1,arg2) ? SA_TRUE : SA_FALSE);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = digit_mvt_bop(arg1,arg2,OP_LT));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_DIGIT **/

      case C_VECTOR :
      case C_TVECTOR:
      case C_MATRIX :
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                                         /* die Argumente werden vertauscht */
                                         /* dh. auch die Klassen            */
                  TEST(result = scalar_mvt_bop(arg2,arg1,OP_GT));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,OP_GT));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,OP_LT));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,OP_GT));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_VECTOR, C_TVECTOR, C_MATRIX **/

      case C_LIST  :
           if (R_DESC(DESC(arg1),type) != TY_STRING)
             goto fail;
           switch(R_DESC(DESC(arg2),class)) {
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  RES_HEAP;
                  result = (str_lt((PTR_DESCRIPTOR)arg1,(PTR_DESCRIPTOR)arg2) ? SA_TRUE : SA_FALSE);   /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
                  REL_HEAP;
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,OP_LT));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_LIST vom Typ  TY_STRING **/
      default:
           goto fail;
    } /** end switch arg1 == class **/
  } /** end if T_POINTER **/


fail:
    END_MODUL("red_lt");
    DBUG_RETURN(0);
decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_lt");
  DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
  _desc = (PTR_DESCRIPTOR) result;
    END_MODUL("red_lt");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
  _desc = (PTR_DESCRIPTOR) result;
    END_MODUL("red_lt");
    DBUG_RETURN(1);
} /**** end of function red_lt ****/

int red_geq(arg1,arg2)                    /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
/*char class1,class2;                       RS 30/10/92 */ 

  DBUG_ENTER ("red_geq");

  START_MODUL("red_geq");
  if (T_INT(arg1))
     { 								/* INT >= ? */
      if (T_INT(arg2))
	 { 							/* INT >= INT */
	  result = GE_INT(arg1,arg2) ;
          goto success ;
         }
	 else
	 { 							/* INT >= ~INT */
          if (T_POINTER(arg2))
	     { 							/* INT >= DESCRIPTOR */
	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT >= SCALAR-DESC */
		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT >= REAL */
                      result = (VAL_INT(arg1) >= R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                      goto scalar_success_int1 ;
                     }
                     else
                     { 						/* INT >= UNDEF */
                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT >= ~SCALAR */
                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {					/* INT >= MVT */
		             int help ;
		             help = VAL_INT(arg1) ;
#if (nCUBE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1) ;
#else
		             arg1 = (STACKELEM)newdesc(); TEST(arg1) ; 
#endif
		             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;

                            }
			    break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {
              goto fail ;
             }
	 } 
     }
     else
     {								/* ~INT >= ? */ 
      if ( T_BOOLEAN(arg1) )
	 {							/* BOOLEAN >= ? */
	  if ( T_BOOLEAN(arg2) )
	     {							/* BOOLEAN >= BOOLEAN */
      	      result = (T_SA_FALSE(arg1) ? arg2 : SA_FALSE);
	      goto success ;
	     }
	     else
	     {							/* BOOLEAN >= ~BOOLEAN */
              if (T_POINTER(arg2))
	         { 						/* BOOLEAN >= DESCRIPTOR */
                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                            {					/* BOOLEAN >= MVT */
			     if (R_DESC(DESC(arg2),type) == TY_BOOL)
				{				/* BOOLEAN >= BMVT */
    				 TEST(result = bool_mvt_bop(arg1,arg2,OP_GE));
    				 DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    				 goto success;
				}
				else
				{				/* BOOLEAN >= ~BMVT */
              			 goto fail ;
				}
                            }
			    /* break ;    RS 30.11.1992 */
                      default: goto fail;
                     }
		 }
		 else
		 {						/* BOOLEAN >= ~DESCRIPTOR */
		  goto fail ;
		 }
	     }
	 }
	 else
	 {							/* ~BOOLEAN >= ? */
	  if ( T_BOOLEAN(arg2) )
	     {							/* ~BOOLEAN >= BOOLEAN */
              if (T_POINTER(arg1))
	         { 						/* DESCRIPTOR >= BOOLEAN */
                  switch(R_DESC(DESC(arg1),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                            {					/* MVT >= BOOLEAN */
			     if (R_DESC(DESC(arg1),type) == TY_BOOL)
				{				/* BMVT >= BOOLEAN */
    				 TEST(result = bool_mvt_bop(arg2,arg1,OP_LE));
    				 DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    				 goto success;
				}
				else
				{				/* ~BMVT >= BOOLEAN */
              			 goto fail ;
				}
                            }
			    /* break ;       RS 30.11.1992 */
                      default: goto fail;
                     }
		 }
		 else
		 {						/* ~DESCRIPTOR >= BOOLEAN */
		  goto fail ;
		 }
	     }
	     else
	     {							/* ~INT,~BOOLEAN >= ? */
      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR >= ? */
          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC >= ? */
              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL >= ? */
                  if (T_INT(arg2))
                     {						/* REAL >= INT */
                      result = (R_SCALAR(DESC(arg1),valr) >= VAL_INT(arg2)
                                ? SA_TRUE : SA_FALSE);
                      goto scalar_success_int2 ;
                     }
                     else
		     { 						/* REAL >= ~INT */
                      if (R_DESC(DESC(arg2),type) == TY_REAL)
                         { 					/* REAL >= REAL */
                          result = (R_SCALAR(DESC(arg1),valr) >= R_SCALAR(DESC(arg2),valr)
                                    ? SA_TRUE : SA_FALSE);
                          goto decrement ;
                         }
		     }
	         }
		 else
                 { 						/* UNDEF >= ? */
                  goto fail ;
		 }
             }
             else
	     { 							/* ~SCALAR >= ? */
	      if (T_INT(arg2))
		 { 						/* ~SCALAR >= INT */
		  int help ;
		  help = VAL_INT(arg2) ;
#if (nCUBE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2) ;
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2) ; 
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF >= ? */
	  goto fail ;
         }
	     }
	 }
     }


  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    switch(R_DESC(DESC(arg1),class)) {

      case C_SCALAR:
           switch(R_DESC(DESC(arg2),class)) {
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,OP_GE));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_SCALAR **/

      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class)) {
             case C_DIGIT:
                  result = (digit_lt(arg1,arg2) ? SA_FALSE : SA_TRUE);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = digit_mvt_bop(arg1,arg2,OP_GE));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_DIGIT **/

      case C_VECTOR :
      case C_TVECTOR:
      case C_MATRIX :
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                                         /* die Argumente werden vertauscht */
                                         /* dh. auch die Klassen            */
                  TEST(result = scalar_mvt_bop(arg2,arg1,OP_LE));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,OP_LE));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,OP_GE));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,OP_LE));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_VECTOR, C_TVECTOR, C_MATRIX **/

      case C_LIST  :
           if (R_DESC(DESC(arg1),type) != TY_STRING)
             goto fail;
           switch(R_DESC(DESC(arg2),class)) {
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  RES_HEAP;
                  result = (str_lt((PTR_DESCRIPTOR)arg1,(PTR_DESCRIPTOR)arg2) ? SA_TRUE : SA_FALSE);  /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */     
                  REL_HEAP;
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,OP_GE));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_LIST vom Typ  TY_STRING **/
      default:
           goto fail;
    } /** end switch arg1 == class **/
  } /** end if T_POINTER **/


fail:
    END_MODUL("red_geq");
    DBUG_RETURN(0);
decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_geq");
  DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
  _desc = (PTR_DESCRIPTOR) result;
    END_MODUL("red_geq");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
  _desc = (PTR_DESCRIPTOR) result;
    END_MODUL("red_geq");
    DBUG_RETURN(1);
} /**** end of function red_geq ****/



/*****************************************************************************/
/*                        red_eq    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function :  Vergleicht zwei Stackelemente auf Gleichheit und Ungleichheit.*/
/*             sea further: red_lt                                           */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement  (pointer to result descriptor)  (true / false)  */
/* called by:  rear.c                                                        */
/* calls    :  digit_eq, str_eq                                              */
/*****************************************************************************/
#ifndef C_FUNC
#define C_FUNC C_EXPRESSION
#endif

int red_eq(arg1,arg2)                        /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  register BOOLEAN result;
              char class1,class2;

  DBUG_ENTER ("red_eq");

  START_MODUL("red_eq");

/*dg*/ if (T_2INT(arg1,arg2)) {
/*dg*/   _desc = (PTR_DESCRIPTOR) (EQ_INT(arg1,arg2));
/*dg*/   END_MODUL("red_eq");
/*dg*/   DBUG_RETURN(1);
/*dg*/ }
  if T_POINTER(arg1) {
    class1 = R_DESC(DESC(arg1),class);
    if (class1 == C_EXPRESSION || class1 == C_CONSTANT || class1 == C_FUNC)
      goto fail;
    /*  z.B Applikation oder ganz freie Variable */
    if T_POINTER(arg2) {
      class2 = R_DESC(DESC(arg2),class);
      if (class2 == C_EXPRESSION || class2 == C_CONSTANT || class2 == C_FUNC)
        goto fail;
      /*  z.B Applikation oder ganz freie Variable */
      if (class1 != class2) {
        result = FALSE;
        goto decrement;
      }
      switch(class1) {
        case C_SCALAR:
/*dg*/       result = R_DESC(DESC(arg2),class) == C_SCALAR
                      && R_SCALAR(DESC(arg1),valr) == R_SCALAR(DESC(arg2),valr);
             goto decrement;
        case C_DIGIT:
             result = digit_eq(arg1,arg2);
             goto decrement;
        case C_LIST  :
             if (R_DESC(DESC(arg1),type) != R_DESC(DESC(arg2),type)) {
               result = FALSE;
               goto decrement;
             }
             if (R_DESC(DESC(arg1),type) == TY_STRING) {
               RES_HEAP;
               result = str_eq((PTR_DESCRIPTOR)arg1,(PTR_DESCRIPTOR)arg2);  /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
               REL_HEAP;
             }
             else {
               if (((PTR_DESCRIPTOR)arg1 == _nil) &&
                   ((PTR_DESCRIPTOR)arg2 == _nil))
                 result = TRUE;
               else {
		 /* (int) von ach voellig falsch eingefuegt, 09/11/92 */
                 /* (int) richtig eingefuegt von RS 10.11.1992 */
                 if (((int)R_LIST(DESC(arg1),dim)  > 0) &&
                     ((int)R_LIST(DESC(arg2),dim)  > 0))
                   goto fail;
                 else
                   result = FALSE;       /* wenn ein Argument eine Liste mit */
               }                         /* Dimension > 1  ist               */
             }
             goto decrement;
        case C_VECTOR:
        case C_TVECTOR:
        case C_MATRIX:
             {
               int i, *ip1, *ip2;         /* "integer pointer" of arg1 /arg2 */
               double *rp1, *rp2;         /*    "real pointer" of arg1 /arg2 */
               int   nrows, ncols=0; /* Initialisierung von TB, 6.11.1992 */
               int   type1, type2;        /* for type of  arg1 and arg2      */
               result = (   ((nrows = R_MVT(DESC(arg1),nrows,class1))
                                      == R_MVT(DESC(arg2),nrows,class1))
                         && ((ncols = R_MVT(DESC(arg1),ncols,class1))
                                      == R_MVT(DESC(arg2),ncols,class1)) );
               if (! result) /* Zeilen- oder Spaltenzahl stimmen nicht */
                 goto decrement;   /* ueberein, also sind die Args. ungleich */

               /* result = TRUE wird postuliert */
               i = nrows * ncols;
               type1 = R_DESC(DESC(arg1),type);
               type2 = R_DESC(DESC(arg2),type);
                                                     /*---------------------*/
               if (type1 == type2) {                 /*   type1  == type2   */
                 switch(type1) {                     /*---------------------*/
                   case TY_DIGIT:
                        while (result && (--i >= 0))
                          result = digit_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                                            R_MVT(DESC(arg2),ptdv,class2)[i]);
                        goto decrement;
                   case TY_STRING:
                        while (result && (--i >= 0))
                          result = str_eq((PTR_DESCRIPTOR)R_MVT(DESC(arg1),ptdv,class1)[i],
                                          (PTR_DESCRIPTOR)R_MVT(DESC(arg2),ptdv,class2)[i]);  /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
                        goto decrement;
                   case TY_UNDEF:      /* zwei nilmat / nilvect / niltvect */
                        goto decrement;
                   case TY_MATRIX:     /* Matrizen von Matrizen sind nicht */
                        goto fail;     /* vergleichbar                     */
                   case TY_BOOL:
                        RES_HEAP;
                        ip1 = ((int *) R_MVT(DESC(arg1),ptdv,class1));
                        ip2 = ((int *) R_MVT(DESC(arg2),ptdv,class2));
                        while (result && (--i >= 0))
                          result = (ip1[i] ? ip2[i] : !ip2[i]);
                        REL_HEAP;
                        goto decrement;
                   case TY_INTEGER:
                        RES_HEAP;
                        ip1 = ((int *) R_MVT(DESC(arg1),ptdv,class1));
                        ip2 = ((int *) R_MVT(DESC(arg2),ptdv,class2));
                        while (result && (--i >= 0))
                          result = ip1[i] == ip2[i];
                        REL_HEAP;
                        goto decrement;
                   case TY_REAL:
                        RES_HEAP;
                        rp1 = ((double *) R_MVT(DESC(arg1),ptdv,class1));
                        rp2 = ((double *) R_MVT(DESC(arg2),ptdv,class2));
                        while (result && (--i >= 0))
                          result = rp1[i] == rp2[i];
                        REL_HEAP;
                        goto decrement;
                   default:
                        post_mortem("red_eq: unknown mvt - type");
                 } /* Ende von switch(type1) */
               }                                     /*---------------------*/
               else {                                /*    type1 != type2   */
                                                     /*---------------------*/
                 if ((type1 == TY_INTEGER) && (type2 == TY_REAL)) {
                   RES_HEAP;
                   ip1 = ((int    *) R_MVT(DESC(arg1),ptdv,class1));
                   rp2 = ((double *) R_MVT(DESC(arg2),ptdv,class2));
                   while (result && (--i >= 0))
                     result = ip1[i] == rp2[i];
                   REL_HEAP;
                   goto decrement;
                 }
                 if ((type1 == TY_REAL) && (type2 == TY_INTEGER)) {
                   RES_HEAP;
                   rp1 = ((double *) R_MVT(DESC(arg1),ptdv,class1));
                   ip2 = ((int    *) R_MVT(DESC(arg2),ptdv,class2));
                   while (result && (--i >= 0))
                     result = rp1[i] == ip2[i];
                   REL_HEAP;
                   goto decrement;
                 }
                 /* ansonsten: */
                 result = FALSE;
                 goto decrement;
               } /** end type1 == type2   or   type1 != type2  **/
             } /** end case C_MATRIX / C_VECTOR / C_TVECTOR **/
        default:
             post_mortem("red_eq: unexpected descriptor class");
      } /** end switch(class1) **/
    } /** end T_POINTER(arg2) **/
    else {
/*dg*/if (T_INT(arg2) && class1 == C_SCALAR) {
/*dg*/  result = (VAL_INT(arg2) == R_SCALAR(DESC(arg1),valr));
/*dg*/  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
/*dg*/  goto success;
/*dg*/}
/*dg*/else
      if (T_CON(arg2))    /* AP- oder SNAP-Applikation */
         goto fail;
      else {
         DEC_REFCNT((PTR_DESCRIPTOR)arg1);
         result = FALSE;
         goto success;
      }
    }
  } /** Ende von T_POINTER(arg1) **/
  else
  if (T_CON(arg1))
     goto fail;
  else {
    if T_POINTER(arg2) {
      class2 = R_DESC(DESC(arg2),class);
      if (class2 == C_EXPRESSION || class2 == C_CONSTANT || class2 == C_FUNC)
        goto fail;
      /*  z.B Applikation         oder  ganz freie Variable */
      /* ansonsten: */
/*dg*/if (T_INT(arg1) && class2 == C_SCALAR) {
/*dg*/  result = (VAL_INT(arg1) == R_SCALAR(DESC(arg2),valr));
/*dg*/}
/*dg*/else
/*dg*/  result = FALSE;
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      goto success;
    }
    else   /* beides keine Pointer */
    if (T_CON(arg2))
       goto fail;
    else {
      result = ((arg1 & ~F_EDIT) == (arg2 & ~F_EDIT));
      /* Editfelder ausblenden nicht vergessen */
      goto success;
    }
  } /* Ende des Else-Teils von T_POINTER(arg1) */

fail:
  END_MODUL("red_eq"); DBUG_RETURN(0);
decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) (result ? SA_TRUE : SA_FALSE);
  END_MODUL("red_eq");
  DBUG_RETURN(1);
} /**** end of function red_eq ****/

int /* TB, 30.10.92 */
red_neq(arg1,arg2)
register STACKELEM arg1,arg2;
{
  DBUG_ENTER ("red_neq");

  START_MODUL("red_eq");
  if (red_eq(arg1,arg2)) {
    _desc = (PTR_DESCRIPTOR)NOT_BOOL((int)_desc);
    END_MODUL("red_eq");
    DBUG_RETURN(1);
  }
  END_MODUL("red_eq");
  DBUG_RETURN(0);
} /**** end of function red_neq ****/



/*****************************************************************************/
/*                        red_le    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function :  Vergleicht zwei Stackelemente bzw Descriptoren                */
/*             auf '<=' und '>'.                                             */
/*             sea further: red_lt                                           */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement  (pointer to result descriptor)  (true / false)  */
/* called by:  rear.c                                                        */
/* calls    :  scalar_mvt_bop,   digit_mvt_bop,                              */
/*             bool_mvt_bop,     mvt_mvt_bop,                                */
/*             str_mvt_bop,      str_le                                      */
/*****************************************************************************/
int /* TB, 30.10.92 */
red_le(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
 
  DBUG_ENTER ("red_le");

  START_MODUL("red_le");
  if (T_INT(arg1))
     { 								/* INT <= ? */
      if (T_INT(arg2))
	 { 							/* INT <= INT */
	  result = LE_INT(arg1,arg2) ;
          goto success ;
         }
	 else
	 { 							/* INT <= ~INT */
          if (T_POINTER(arg2))
	     { 							/* INT <= DESCRIPTOR */
	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT <= SCALAR-DESC */
		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT <= REAL */
                      result = (VAL_INT(arg1) <= R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                      goto scalar_success_int1 ;
                     }
                     else
                     { 						/* INT <= UNDEF */
                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT <= ~SCALAR */
                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {					/* INT <= MVT */
		             int help ;
		             help = VAL_INT(arg1) ;
#if (nCUBE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1) ;
#else
		             arg1 = (STACKELEM)newdesc(); TEST(arg1) ; 
#endif
		             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;

                            }
			    break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {
              goto fail ;
             }
	 } 
     }
     else
     { 								/* ~INT <= ? */ 
      if ( T_BOOLEAN(arg1) )
	 {							/* BOOLEAN <= ? */
	  if ( T_BOOLEAN(arg2) )
	     {							/* BOOLEAN <= BOOLEAN */
              result = (T_SA_FALSE(arg1) ? arg2 : SA_FALSE);
	      goto success ;
	     }
	     else
	     {							/* BOOLEAN <= ~BOOLEAN */
              if (T_POINTER(arg2))
	         { 						/* BOOLEAN <= DESCRIPTOR */
                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                            {					/* BOOLEAN <= MVT */
			     if (R_DESC(DESC(arg2),type) == TY_BOOL)
				{				/* BOOLEAN <= BMVT */
    				 TEST(result = bool_mvt_bop(arg1,arg2,OP_LE));
    				 DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    				 goto success;
				}
				else
				{				/* BOOLEAN <= ~BMVT */
              			 goto fail ;
				}
                            }
			    /* break ;      RS 30.11.1992 */
                      default: goto fail;
                     }
		 }
		 else
		 {						/* BOOLEAN <= ~DESCRIPTOR */
		  goto fail ;
		 }
	     }
	 }
	 else
	 {							/* ~BOOLEAN <= ? */
	  if ( T_BOOLEAN(arg2) )
	     {							/* ~BOOLEAN <= BOOLEAN */
              if (T_POINTER(arg1))
	         { 						/* DESCRIPTOR <= BOOLEAN */
                  switch(R_DESC(DESC(arg1),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                            {					/* MVT <= BOOLEAN */
			     if (R_DESC(DESC(arg1),type) == TY_BOOL)
				{				/* BMVT <= BOOLEAN */
    				 TEST(result = bool_mvt_bop(arg2,arg1,OP_GE));
    				 DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    				 goto success;
				}
				else
				{				/* ~BMVT <= BOOLEAN */
              			 goto fail ;
				}
                            }
			    /* break ;   RS 30.11.1992 */
                      default: goto fail;
                     }
		 }
		 else
		 {						/* ~DESCRIPTOR <= BOOLEAN */
		  goto fail ;
		 }
	     }
	     else
	     {							/* ~INT,~BOOLEAN <= ? */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR <= ? */
          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC <= ? */
              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL <= ? */
                  if (T_INT(arg2))
                     {						/* REAL <= INT */
                      result = (R_SCALAR(DESC(arg1),valr) <= VAL_INT(arg2)
                                ? SA_TRUE : SA_FALSE);
                      goto scalar_success_int2 ;
                     }
                     else
		     { 						/* REAL <= ~INT */
                      if (R_DESC(DESC(arg2),type) == TY_REAL)
                         { 					/* REAL <= REAL */
                          result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),valr)
                                    ? SA_TRUE : SA_FALSE);
                          goto decrement ;
                         }
		     }
	         }
		 else
                 { 						/* UNDEF <= ? */
                  goto fail ;
		 }
             }
             else
	     { 							/* ~SCALAR <= ? */
	      if (T_INT(arg2))
		 { 						/* ~SCALAR <= INT */
		  int help ;
		  help = VAL_INT(arg2) ;
#if (nCUBE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2) ;
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2) ; 
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF <= ? */
	  goto fail ;
         }
	     }
	 }
     }


  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    switch (R_DESC(DESC(arg1),class)) {
      case C_SCALAR:
           switch(R_DESC(DESC(arg2),class)) {
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,OP_LE));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/

      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class)) {
             case C_DIGIT:
                  result = (digit_le(arg1,arg2) ? SA_TRUE : SA_FALSE);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = digit_mvt_bop(arg1,arg2,OP_LE));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/

      case C_VECTOR :
      case C_TVECTOR:
      case C_MATRIX :
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  TEST(result = scalar_mvt_bop(arg2,arg1,OP_GE));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,OP_GE));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,OP_LE));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,OP_GE));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/

      case C_LIST  :
           if (R_DESC(DESC(arg1),type) != TY_STRING)
             goto fail;
           switch(R_DESC(DESC(arg2),class)) {
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  RES_HEAP;
                  result = (str_le((PTR_DESCRIPTOR)arg1,(PTR_DESCRIPTOR)arg2) ? SA_TRUE : SA_FALSE);   /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
                  REL_HEAP;
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,OP_LE));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/
      default:
           goto fail;
    }  /** end switch class von arg1 **/
  }  /** end if T_POINTER von  arg1/arg2 **/

  if T_BOOLEAN(arg1)
    if T_BOOLEAN(arg2) {
      result = (T_SA_TRUE(arg1) ? arg2 : SA_TRUE);
      goto success;
    }
  else {
    TEST(result = bool_mvt_bop(arg1,arg2,OP_LE));
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }

  if T_BOOLEAN(arg2) {
    TEST(result = bool_mvt_bop(arg2,arg1,OP_GE));
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

fail:
    END_MODUL("red_le");
    DBUG_RETURN(0);
decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_le");
  DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    _desc = (PTR_DESCRIPTOR) result;
    END_MODUL("red_le");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    _desc = (PTR_DESCRIPTOR) result;
    END_MODUL("red_le");
    DBUG_RETURN(1);
} /**** end of function red_le ****/


int /* TB, 30.10.92 */
red_gt(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;

  DBUG_ENTER ("red_gt");

  START_MODUL("red_gt");
  if (T_INT(arg1))
     { 								/* INT > ? */
      if (T_INT(arg2))
	 { 							/* INT > INT */
	  result = GT_INT(arg1,arg2) ;
          goto success ;
         }
	 else
	 { 							/* INT > ~INT */
          if (T_POINTER(arg2))
	     { 							/* INT > DESCRIPTOR */
	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT > SCALAR-DESC */
		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT > REAL */
                      result = (VAL_INT(arg1) > R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                      goto scalar_success_int1 ;
                     }
                     else
                     { 						/* INT > UNDEF */
                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT > ~SCALAR */
                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {					/* INT > MVT */
		             int help ;
		             help = VAL_INT(arg1) ;
#if (nCUBE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1) ;
#else
		             arg1 = (STACKELEM)newdesc(); TEST(arg1) ; 
#endif
		             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;

                            }
			    break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {
              goto fail ;
             }
	 } 
     }
     else
     { 								/* ~INT > ? */ 
      if ( T_BOOLEAN(arg1) )
	 {							/* BOOLEAN > ? */
	  if ( T_BOOLEAN(arg2) )
	     {							/* BOOLEAN > BOOLEAN */
              result = GT_BOOL(arg1,arg2) ;
	      goto success ;
	     }
	     else
	     {							/* BOOLEAN > ~BOOLEAN */
              if (T_POINTER(arg2))
	         { 						/* BOOLEAN > DESCRIPTOR */
                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                            {					/* BOOLEAN > MVT */
			     if (R_DESC(DESC(arg2),type) == TY_BOOL)
				{				/* BOOLEAN > BMVT */
    				 TEST(result = bool_mvt_bop(arg1,arg2,OP_GT));
    				 DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    				 goto success;
				}
				else
				{				/* BOOLEAN > ~BMVT */
              			 goto fail ;
				}
                            }
			    /* break ;      RS 30.11.1992 */
                      default: goto fail;
                     }
		 }
		 else
		 {						/* BOOLEAN > ~DESCRIPTOR */
		  goto fail ;
		 }
	     }
	 }
	 else
	 {							/* ~BOOLEAN > ? */
	  if ( T_BOOLEAN(arg2) )
	     {							/* ~BOOLEAN > BOOLEAN */
              if (T_POINTER(arg1))
	         { 						/* DESCRIPTOR > BOOLEAN */
                  switch(R_DESC(DESC(arg1),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                            {					/* MVT > BOOLEAN */
			     if (R_DESC(DESC(arg1),type) == TY_BOOL)
				{				/* BMVT > BOOLEAN */
    				 TEST(result = bool_mvt_bop(arg2,arg1,OP_LT));
    				 DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    				 goto success;
				}
				else
				{				/* ~BMVT > BOOLEAN */
              			 goto fail ;
				}
                            }
			    /* break ;    RS 30.11.1992 */ 
                      default: goto fail;
                     }
		 }
		 else
		 {						/* ~DESCRIPTOR > BOOLEAN */
		  goto fail ;
		 }
	     }
	     else
	     {							/* ~INT,~BOOLEAN > ? */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR > ? */
          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC > ? */
              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL > ? */
                  if (T_INT(arg2))
                     {						/* REAL > INT */
                      result = (R_SCALAR(DESC(arg1),valr) > VAL_INT(arg2)
                                ? SA_TRUE : SA_FALSE);
                      goto scalar_success_int2 ;
                     }
                     else
		     { 						/* REAL > ~INT */
                      if (R_DESC(DESC(arg2),type) == TY_REAL)
                         { 					/* REAL > REAL */
                          result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),valr)
                                    ? SA_TRUE : SA_FALSE);
                          goto decrement ;
                         }
		     }
	         }
		 else
                 { 						/* UNDEF > ? */
                  goto fail ;
		 }
             }
             else
	     { 							/* ~SCALAR > ? */
	      if (T_INT(arg2))
		 { 						/* ~SCALAR > INT */
		  int help ;
		  help = VAL_INT(arg2) ;
#if (nCUBE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2) ;
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2) ; 
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF > ? */
	  goto fail ;
         }
	     }
	 }
     }


  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    switch (R_DESC(DESC(arg1),class)) {
      case C_SCALAR:
           switch(R_DESC(DESC(arg2),class)) {
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,OP_GT));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/

      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class)) {
             case C_DIGIT:
                  result = (digit_le(arg1,arg2) ? SA_FALSE : SA_TRUE);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = digit_mvt_bop(arg1,arg2,OP_GT));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/

      case C_VECTOR :
      case C_TVECTOR:
      case C_MATRIX :
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  TEST(result = scalar_mvt_bop(arg2,arg1,OP_LT));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,OP_LT));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,OP_GT));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,OP_LT));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/

      case C_LIST  :
           if (R_DESC(DESC(arg1),type) != TY_STRING)
             goto fail;
           switch(R_DESC(DESC(arg2),class)) {
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  RES_HEAP;
                  result = (str_le((PTR_DESCRIPTOR)arg1,(PTR_DESCRIPTOR)arg2) ? SA_FALSE : SA_TRUE);  /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
                  REL_HEAP;
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,OP_GT));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/
      default:
           goto fail;
    }  /** end switch class von arg1 **/
  }  /** end if T_POINTER von  arg1/arg2 **/

fail:
    END_MODUL("red_gt");
    DBUG_RETURN(0);
decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_gt");
  DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    _desc = (PTR_DESCRIPTOR) result;
    END_MODUL("red_gt");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    _desc = (PTR_DESCRIPTOR) result;
    END_MODUL("red_gt");
    DBUG_RETURN(1);
} /**** end of function red_gt ****/



/*****************************************************************************/
/*                        red_min   (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function : Ermittelt das Minimum oder das Maximum zweier Argumente,       */
/*            unter Verwendung der Ordnung auf den Typen Boolean,            */
/*            Decimal und String.                                            */
/*                                                                           */
/*    max und min sind auf Datenobjekte vom Typ Bool, Decimal, und String    */
/*    anwendbar. Matrizen und Skalare des gleichen Typs koennen verknuepft   */
/*    werden. Es entsteht eine neue Matrix, desren Elemente aus der Ver-     */
/*    knuepfung des Skalars mit dem jeweiligen Matrizenelement hervorgeht.   */
/*    Soll das Maximum bzw. das Minimum zweier Matrizen ermittelt werden, so */
/*    muessen beide die gleiche Zeilen- und Spaltenzahl haben. max bzw. min  */
/*    wird dann elementweise angewendet. analog mit Vektoren bzw. TVektoren. */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/* called by:  rear.c                                                        */
/* calls    :  digit_le,         digit_mvt_bop,                              */
/*             str_le,           str_mvt_bop,                                */
/*             scalar_mvt_bop,   bool_mvt_bop                                */
/*****************************************************************************/
int /* TB, 30.10.92 */
red_min(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;

  DBUG_ENTER ("red_min");

  START_MODUL("red_min");

  switch (TEST_TYPE(arg1))
   {
    case STE_INT :
         switch (TEST_TYPE(arg2)) 
          {
           case STE_INT :
                    result = MIN_INT(arg1,arg2) ;
		    goto success ;

           case STE_BOOL :  goto fail ;

           case STE_POINTER :
                switch (R_DESC(DESC(arg2),class))
                 {
                  case C_SCALAR:
			   if (VAL_INT(arg1) <= R_SCALAR(DESC(arg2),valr))
			      {
			       result = arg1 ;
			       goto decref_arg2 ;
			      }
			      else
			      {
			       result = arg2 ;
			       goto success ;
			      }
                  case C_VECTOR :
		  case C_TVECTOR:
		  case C_MATRIX :
                       {
#ifdef NotCorrect
			int help ;

		        help = VAL_INT(arg1) ;
		        NEWDESC( (PTR_DESCRIPTOR) arg1) ; TEST( (PTR_DESCRIPTOR) arg1) ; 
		        DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
			L_SCALAR(DESC(arg1),vali) = help ;
#else
                        T_PTD ptd;
                        NEWDESC(ptd) ; TEST(ptd);
                        DESC_MASK(ptd,1,C_SCALAR,TY_INTEGER) ;
                        L_SCALAR(*ptd,vali) = VAL_INT(arg1) ;
                        arg1 = (int)ptd;
#endif /* NotCorrect  auskommentiert RS 30.11.1992 */     
                        TEST(result = scalar_mvt_bop(arg1,arg2,OP_MIN)) ;
			goto decrement ;
                       }
                  default: goto fail ;
                 } /* end switch class von arg2 */

           default: goto fail ;
          } /* end switch TEST_TYPE arg2 */
                                
    case STE_BOOL :
         switch (TEST_TYPE(arg2)) 
          {
           case STE_INT : goto fail ;

           case STE_BOOL :
                  result = (T_SA_FALSE(arg1) ? SA_FALSE : arg2);
                  goto success;

           case STE_POINTER :
                TEST(result = bool_mvt_bop(arg1,arg2,OP_MIN)) ;
		goto success ;
          } /* end switch TEST_TYPE von arg2 */

    case STE_POINTER :
         switch (R_DESC(DESC(arg1),class))
	  {
	   case C_SCALAR :
		switch (TEST_TYPE(arg2))
		 {
		  case STE_INT : 
			   if (R_SCALAR(DESC(arg1),valr) < VAL_INT(arg2))
			      {
			       result = arg1 ;
			       goto success ;
			      }
			      else
			      {
			       result = arg2 ;
			       goto decref_arg1 ;
			      }

		  case STE_BOOL : 
                       TEST(result = bool_mvt_bop(arg2,arg1,OP_MIN)) ;
		       goto success ;
		       
		  case STE_POINTER :
		       switch (R_DESC(DESC(arg2),class))
			{
			 case C_SCALAR :
			      result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),valr)) ?
					arg1 : arg2 ;
			      INC_REFCNT((PTR_DESCRIPTOR)result) ;
			      goto decrement ;
			 case C_VECTOR :
			 case C_TVECTOR:
			 case C_MATRIX :
			      TEST(result = scalar_mvt_bop(arg1,arg2,OP_MIN)) ;
			      goto decrement ;
			 default : goto fail ;
			} /* end switch von class von arg2 */
		 }
	   case C_VECTOR :
	   case C_TVECTOR:
	   case C_MATRIX :
		switch (TEST_TYPE(arg2))
		 {
		  case STE_INT :
                       {
#ifdef NotCorrect
			int help ;

		        help = VAL_INT(arg2) ;
		        NEWDESC( (PTR_DESCRIPTOR) arg2) ; TEST( (PTR_DESCRIPTOR) arg2) ; 
		        DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
			L_SCALAR(DESC(arg2),vali) = help ;
#else
                        T_PTD ptd;
                        NEWDESC(ptd) ; TEST(ptd);
                        DESC_MASK(ptd,1,C_SCALAR,TY_INTEGER) ;
                        L_SCALAR(*ptd,vali) = VAL_INT(arg2) ;
                        arg2 = (int)ptd;
#endif /* NotCorrect       auskommentiert RS 30.11.1992 */
                        TEST(result = scalar_mvt_bop(arg2,arg1,OP_MIN)) ;
			goto decrement ;
		       }
		  case STE_BOOL :
		       TEST(result = bool_mvt_bop(arg2,arg1,OP_MIN));
		       DEC_REFCNT((PTR_DESCRIPTOR)arg1);
		       goto success ;
		  case STE_POINTER :
		       switch (R_DESC(DESC(arg2),class))
			{
			 case C_SCALAR :
			      TEST(result = scalar_mvt_bop(arg2,arg1,OP_MIN)) ;
			      goto decrement ;
			 case C_DIGIT :
			      TEST(result = digit_mvt_bop(arg2,arg1,OP_MIN)) ;
			      goto decrement ;
			 case C_VECTOR :
			 case C_TVECTOR:
			 case C_MATRIX :
			      TEST(result = mvt_mvt_bop(arg1,arg2,OP_MIN)) ;
			      goto decrement ;
			 case C_LIST :
                  	      if (R_DESC(DESC(arg2),type) != TY_STRING)
                    	         {
				  goto fail;
                    	         }
                  	      TEST(result = str_mvt_bop(arg2,arg1,OP_MIN));
                  	      goto decrement;
			 default : goto fail ;     
			} /* end switch von class von arg2 */
		  default : goto fail ;
		 } /* end switch TEST_TYPE von arg2 */
           case C_LIST :
	   	switch (TEST_TYPE(arg2))
		 {
		  case STE_INT :
		  case STE_BOOL : goto fail ;
		  case STE_POINTER :
                       if (R_DESC(DESC(arg1),type) != TY_STRING)
		          {
                           goto fail;
		          }
                       switch(R_DESC(DESC(arg2),class)) 
		        {
                         case C_LIST  :
                              if (R_DESC(DESC(arg2),type) != TY_STRING)
			         {
                                  goto fail;
			         }
                              RES_HEAP;
                              result = (str_lt((PTR_DESCRIPTOR)arg1,(PTR_DESCRIPTOR)arg2)   /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */    
                                      ? arg1
                                      : arg2 ) ;
                              REL_HEAP;
                              INC_REFCNT((PTR_DESCRIPTOR)result);
                              goto decrement;
                         case C_VECTOR :
                         case C_TVECTOR:
                         case C_MATRIX :
                              TEST(result = str_mvt_bop(arg1,arg2,OP_MIN));
                              goto decrement;
                         default: goto fail ;
		        } /* end von switch von class von arg2 */
		 } /* end von TEST_TYPE von arg2 */
	  } /* end switch class von arg1 */
   } /* end von TEST_TYPE von arg1 */

fail:
  END_MODUL("red_min"); DBUG_RETURN(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_min");
  DBUG_RETURN(1);
decref_arg1:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_min");
  DBUG_RETURN(1);
decref_arg2:
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_min");
  DBUG_RETURN(1);
} /**** end of function red_min ****/

int /* TB, 30.10.92 */
red_max(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result=(STACKELEM)0; /* Initialisierung von TB, 6.11.1992 */

  DBUG_ENTER ("red_max");

  START_MODUL("red_max");

  switch (TEST_TYPE(arg1))
   {
    case STE_INT :
         switch (TEST_TYPE(arg2)) 
          {
           case STE_INT :
               	    result = MAX_INT(arg1,arg2) ;
		    goto success ;

           case STE_BOOL :  goto fail ;

           case STE_POINTER :
                switch (R_DESC(DESC(arg2),class))
                 {
                  case C_SCALAR:
			   if (VAL_INT(arg1) < R_SCALAR(DESC(arg2),valr))
			      {
			       result = arg2 ;
			       goto success ;
			      }
			      else
			      {
			       result = arg1 ;
			       goto decref_arg2 ;
			      }
                  case C_VECTOR :
		  case C_TVECTOR:
		  case C_MATRIX :
                       {
#ifdef NotCorrect
			int help ;

		        help = VAL_INT(arg1) ;
		        NEWDESC( (PTR_DESCRIPTOR) arg1) ; TEST( (PTR_DESCRIPTOR) arg1) ; 
		        DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
			L_SCALAR(DESC(arg1),vali) = help ;
#else
                        T_PTD ptd;
                        NEWDESC(ptd) ; TEST(ptd);
                        DESC_MASK(ptd,1,C_SCALAR,TY_INTEGER) ;
                        L_SCALAR(*ptd,vali) = VAL_INT(arg1) ;
                        arg1 = (int)ptd;
#endif /* NotCorrect   auskommentiert RS 30.11.1992 */
                        TEST(result = scalar_mvt_bop(arg1,arg2,OP_MAX)) ;
			goto decrement ;
                       }
                  default: goto fail ;
                 } /* end switch class von arg2 */

           default: goto fail ;
          } /* end switch TEST_TYPE arg2 */
                                
    case STE_BOOL :
         switch (TEST_TYPE(arg2)) 
          {
           case STE_INT : goto fail ;

           case STE_BOOL :
                  result = (T_SA_TRUE(arg1) ? SA_TRUE : arg2);

           case STE_POINTER :
                TEST(result = bool_mvt_bop(arg1,arg2,OP_MAX)) ;
		goto success ;
          } /* end switch TEST_TYPE von arg2 */

    case STE_POINTER :
         switch (R_DESC(DESC(arg1),class))
	  {
	   case C_SCALAR :
		switch (TEST_TYPE(arg2))
		 {
		  case STE_INT : 
			   if (R_SCALAR(DESC(arg1),valr) <= VAL_INT(arg2))
			      {
			       result = arg2 ;
			       goto decref_arg1 ;
			      }
			      else
			      {
			       result = arg1 ;
			       goto success ;
			      }
		  case STE_BOOL : 
                       TEST(result = bool_mvt_bop(arg2,arg1,OP_MAX)) ;
		       goto success ;
		       
		  case STE_POINTER :
		       switch (R_DESC(DESC(arg2),class))
			{
			 case C_SCALAR :
			      result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),valr)) ?
					arg2 : arg1 ;
			      INC_REFCNT((PTR_DESCRIPTOR)result) ;
			      goto decrement ;
			 case C_VECTOR :
			 case C_TVECTOR:
			 case C_MATRIX :
			      TEST(result = scalar_mvt_bop(arg1,arg2,OP_MAX)) ;
			      goto decrement ;
			 default : goto fail ;
			} /* end switch von class von arg2 */
		 }
	   case C_VECTOR :
	   case C_TVECTOR:
	   case C_MATRIX :
		switch (TEST_TYPE(arg2))
		 {
		  case STE_INT :
                       {
#ifdef NotCorrect
			int help ;

		        help = VAL_INT(arg2) ;
		        NEWDESC( (PTR_DESCRIPTOR) arg2) ; TEST( (PTR_DESCRIPTOR) arg2) ; 
		        DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
			L_SCALAR(DESC(arg2),vali) = help ;
#else
                        T_PTD ptd;
                        NEWDESC(ptd) ; TEST(ptd);
                        DESC_MASK(ptd,1,C_SCALAR,TY_INTEGER) ;
                        L_SCALAR(*ptd,vali) = VAL_INT(arg2) ;
                        arg2 = (int)ptd;
#endif /* NotCorrect  auskommentiert RS 30.11.1992 */
                        TEST(result = scalar_mvt_bop(arg2,arg1,OP_MAX)) ;
			goto decrement ;
		       }
		  case STE_BOOL :
		       TEST(result = bool_mvt_bop(arg2,arg1,OP_MAX));
		       DEC_REFCNT((PTR_DESCRIPTOR)arg1);
		       goto success ;
		  case STE_POINTER :
		       switch (R_DESC(DESC(arg2),class))
			{
			 case C_SCALAR :
			      TEST(result = scalar_mvt_bop(arg2,arg1,OP_MAX)) ;
			      goto decrement ;
			 case C_DIGIT :
			      TEST(result = digit_mvt_bop(arg2,arg1,OP_MAX)) ;
			      goto decrement ;
			 case C_VECTOR :
			 case C_TVECTOR:
			 case C_MATRIX :
			      TEST(result = mvt_mvt_bop(arg1,arg2,OP_MAX)) ;
			      goto decrement ;
			 case C_LIST :
                  	      if (R_DESC(DESC(arg2),type) != TY_STRING)
                    	         {
				  goto fail;
                    	         }
                  	      TEST(result = str_mvt_bop(arg2,arg1,OP_MAX));
                  	      goto decrement;
			 default : goto fail ;     
			} /* end switch von class von arg2 */
		  default : goto fail ;
		 } /* end switch TEST_TYPE von arg2 */
           case C_LIST :
	   	switch (TEST_TYPE(arg2))
		 {
		  case STE_INT :
		  case STE_BOOL : goto fail ;
		  case STE_POINTER :
                       if (R_DESC(DESC(arg1),type) != TY_STRING)
		          {
                           goto fail;
		          }
                       switch(R_DESC(DESC(arg2),class)) 
		        {
                         case C_LIST  :
                              if (R_DESC(DESC(arg2),type) != TY_STRING)
			         {
                                  goto fail;
			         }
                              RES_HEAP;
                              result = (str_lt((PTR_DESCRIPTOR)arg1,(PTR_DESCRIPTOR)arg2)  /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */  
                                      ? arg2 
                                      : arg1 ) ;
                              REL_HEAP;
                              INC_REFCNT((PTR_DESCRIPTOR)result);
                              goto decrement;
                         case C_VECTOR :
                         case C_TVECTOR:
                         case C_MATRIX :
                              
                              goto decrement;
                         default: goto fail ;
		        } /* end von switch von class von arg2 */
		 } /* end von TEST_TYPE von arg2 */
	  } /* end switch class von arg1 */
   } /* end von TEST_TYPE von arg1 */

fail:
  END_MODUL("red_max"); DBUG_RETURN(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_max");
  DBUG_RETURN(1);
decref_arg1:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_max");
  DBUG_RETURN(1);
decref_arg2:
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_max");
  DBUG_RETURN(1);
} /**** end of function red_max ****/


/*****************************************************************************/
/*      red_f_eq   (compare function for matrices, vectors and tvectors)     */
/* ------------------------------------------------------------------------- */
/* function :  Diese Funktion enthaelt auch die Funktion       "f_ne"        */
/*             Diese beiden Funktionen sind auf Matrizen, Vektoren und       */
/*             TVektoren des Typs Bool, Decimal und String definiert.        */
/*             Die zu vergleichenden Matrizen muessen die gleiche Zeilen-    */
/*             und Spaltenzahl. Bei Vektoren bzw. TVektoren muss die         */
/*             Dimension der beiden Argumente gleich sein.                   */
/*             Das Ergebnis des Vergleichs ist im Gegensatz zu den Funk-     */
/*             tionen eq und ne ...  eine Matrix oder ein Vektor oder ein    */
/*             TVektor mit Wahrheitswerten, deren Elemente aus Einzelver-    */
/*             vergleichen der Argumentelemente bestimmt wird.               */
/*             Der Vergleich von Matrizen mit mehr als zwei Dimensionen ist  */
/*             nicht erlaubt.                                                */
/*                                                                           */
/*             f_eq, f_ne :   mat    x   mat        -->  bmat                */
/*                            bmat   x   bmat       -->  bmat                */
/*                            smat   x   smat       -->  bmat                */
/*                            vect   x   vect       -->  bvect               */
/*                            bvect  x   bvect      -->  bvect               */
/*                            svect  x   svect      -->  bvect               */
/*                            tvect  x   tvect      -->  btvect              */
/*                            btvect x   btvect     -->  btvect              */
/*                            stvect x   stvect     -->  btvect              */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/*             bmat, bvect, btvect                                           */
/* called by:  rear.c                                                        */
/* calls    :  digit_eq, str_eq.                                             */
/*****************************************************************************/
int /* TB, 30.10.92 */
red_f_eq(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register int i,dim;
  int nrows, ncols;
  int type1,type2;                   /* fuer den Typ von arg1  bzw.  arg2    */
  int *ip1, *ip2, *ipnew;            /* "integer pointer" von arg1 / arg2    */
  double *rp1, *rp2;                 /*    "real pointer" von arg1 / arg2    */
  PTR_DESCRIPTOR desc;
  BOOLEAN     equal;
     char     class1,class2;         /* fuer die Klasse von   arg1 / arg2    */

  DBUG_ENTER ("red_f_eq");

  START_MODUL("red_f_eq");

  if (!T_POINTER(arg1) || !T_POINTER(arg2))
    goto fail;

  if ((class1 = R_DESC(DESC(arg1),class)) !=
      (class2 = R_DESC(DESC(arg2),class)))
    goto fail;              /* fail, wenn arg1 und arg2 nicht der selben     */
                            /*       Klasse entsprechen                      */
  switch(class1) {
    case  C_MATRIX :
    case  C_VECTOR :
    case  C_TVECTOR: break;
    default : goto fail;        /* fail, wenn arg2 nicht in den Klassen      */
  } /** switch class1 **/       /*     C_MATRIX oder C_VECTOR oder C_TVECTOR */

  switch(class2) {
    case  C_MATRIX : if ((nrows = R_MVT(DESC(arg1),nrows,class1)) !=
                                  R_MVT(DESC(arg2),nrows,class2))
                       goto fail;         /* nicht die selbe Zeilenanzahl    */
                     if ((ncols = R_MVT(DESC(arg1),ncols,class1)) !=
                                  R_MVT(DESC(arg2),ncols,class2))
                       goto fail;         /* nicht die selbe  Spaltenanzahl  */
                     break;
    case  C_VECTOR : if ((ncols = R_MVT(DESC(arg1),ncols,class1)) !=
                                  R_MVT(DESC(arg2),ncols,class2))
                       goto fail;                /* nicht dieselbe Dimension */
                     nrows = 1;
                     break;
    case  C_TVECTOR: if ((nrows = R_MVT(DESC(arg1),nrows,class1)) !=
                                  R_MVT(DESC(arg2),nrows,class2))
                       goto fail;                /* nicht dieselbe Dimension */
                     ncols = 1;
                     break;
    default : goto fail;        /* fail, wenn arg2 nicht in den Klassen      */
  } /** switch class2 **/       /*     C_MATRIX oder C_VECTOR oder C_TVECTOR */


  equal = T_F_EQ(READ_H());    /*  fuer die Funktion f_eq              */

  dim = nrows * ncols;
  type1 = R_DESC(DESC(arg1),type);
  type2 = R_DESC(DESC(arg2),type);
                                                     /*----------------------*/
  if (type1 == type2) {                              /*  type1  ==  type2    */
    switch(type1) {                                  /*----------------------*/
      case TY_UNDEF :                   /* arg1 == , arg2 ==  */
           desc = (PTR_DESCRIPTOR)arg1; /* nilmat  , nilmat   */
           INC_REFCNT(desc);            /* nilvect , nilvect  */
           goto success;                /* niltvect, niltvect */

      case TY_MATRIX : goto fail; /* Matrizen mit mehr als zwei Dimensionen  */

     /* RES_HEAP ist im folgenden DIGIT-case nicht erlaubt, da digit_eq      */
     /*                                                       dies schon tun */

      case TY_DIGIT  :
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class1,TY_BOOL);
           L_MVT((*desc),nrows,class1) = nrows;
           L_MVT((*desc),ncols,class1) = ncols;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

           if (equal)
             for (i = 0; i < dim; i++)
               ((int *)R_MVT((*desc),ptdv,class1))[i] =
                     digit_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                              R_MVT(DESC(arg2),ptdv,class2)[i]);
           else
             for (i = 0; i < dim; i++)
               ((int *)R_MVT((*desc),ptdv,class1))[i] = !
                     digit_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                              R_MVT(DESC(arg2),ptdv,class1)[i]);
           goto success;

      case TY_STRING :
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class1,TY_BOOL);
           L_MVT((*desc),nrows,class1) = nrows;
           L_MVT((*desc),ncols,class1) = ncols;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

           RES_HEAP;

           if (equal)
             for (i = 0; i < dim; i++)
               ((int *)R_MVT((*desc),ptdv,class1))[i] =
                       str_eq((PTR_DESCRIPTOR)R_MVT(DESC(arg1),ptdv,class1)[i],
                              (PTR_DESCRIPTOR)R_MVT(DESC(arg2),ptdv,class2)[i]);  /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
           else
             for (i = 0; i < dim; i++)
               ((int *)R_MVT((*desc),ptdv,class1))[i] = !
                       str_eq((PTR_DESCRIPTOR)R_MVT(DESC(arg1),ptdv,class1)[i],
                              (PTR_DESCRIPTOR)R_MVT(DESC(arg2),ptdv,class2)[i]);  /* (PTR_DESCRIPTOR) eingefuegt RS 30.11.1992 */
           REL_HEAP;
           goto success;

      case TY_BOOL   :
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class1,TY_BOOL);
           L_MVT((*desc),nrows,class1) = nrows;
           L_MVT((*desc),ncols,class1) = ncols;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

           RES_HEAP;
           ipnew = ((int *) R_MVT((*desc),ptdv,class1));
           ip1 = ((int *) R_MVT(DESC(arg1),ptdv,class1));
           ip2 = ((int *) R_MVT(DESC(arg2),ptdv,class2));

           if (equal)
             for (i = 0; i < dim; i++)
               ipnew[i] = (ip1[i] ? ip2[i] : !ip2[i]);
           else
             for (i = 0; i < dim; i++)
               ipnew[i] = (ip1[i] ? !ip2[i] : ip2[i]);

           REL_HEAP;
           goto success;

      case TY_INTEGER:
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class1,TY_BOOL);
           L_MVT((*desc),nrows,class1) = nrows;
           L_MVT((*desc),ncols,class1) = ncols;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

           RES_HEAP;
           ipnew = ((int *) R_MVT((*desc),ptdv,class1));
           ip1 = ((int *) R_MVT(DESC(arg1),ptdv,class1));
           ip2 = ((int *) R_MVT(DESC(arg2),ptdv,class2));

           if (equal)
             for (i = 0; i < dim; i++)
               ipnew[i] = (ip1[i] == ip2[i]);
           else
             for (i = 0; i < dim; i++)
               ipnew[i] = (ip1[i] != ip2[i]);

           REL_HEAP;
           goto success;

      case TY_REAL :
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class1,TY_BOOL);
           L_MVT((*desc),nrows,class1) = nrows;
           L_MVT((*desc),ncols,class1) = ncols;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

           RES_HEAP;
           ipnew = ((int *) R_MVT((*desc),ptdv,class1));
           rp1 = ((double *) R_MVT(DESC(arg1),ptdv,class1));
           rp2 = ((double *) R_MVT(DESC(arg2),ptdv,class2));

           if (equal)
             for (i = 0; i < dim; i++)
               ipnew[i] = (rp1[i] == rp2[i]);
           else
             for (i = 0; i < dim; i++)
               ipnew[i] = (rp1[i] != rp2[i]);

           REL_HEAP;
           goto success;

      default  : post_mortem("red_f_eq: unknown mvt - type");
    } /** end switch type1 **/
  } /** end type1 == type2 **/                       /*---------------------*/
  else {                                             /*   type1 != type2    */
                                                     /*---------------------*/
    if ((type1 == TY_INTEGER) && (type2 == TY_REAL)) {
      NEWDESC(_desc); INIT_DESC;
      MVT_DESC_MASK(desc,1,class1,TY_BOOL);
      L_MVT((*desc),nrows,class1) = nrows;
      L_MVT((*desc),ncols,class1) = ncols;
      NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

      RES_HEAP;
      ipnew =   ((int *) R_MVT((*desc),ptdv,class1));
      ip1 =     ((int *) R_MVT(DESC(arg1),ptdv,class1));
      rp2 =  ((double *) R_MVT(DESC(arg2),ptdv,class2));

      if (equal)
        for (i = 0; i < dim; i++)
          ipnew[i] = (ip1[i] == rp2[i]);
      else
        for (i = 0; i < dim; i++)
          ipnew[i] = (ip1[i] != rp2[i]);

      REL_HEAP;
      goto success;
    }

    if ((type1 == TY_REAL) && (type2 == TY_INTEGER)) {
      NEWDESC(_desc); INIT_DESC;
      MVT_DESC_MASK(desc,1,class1,TY_BOOL);
      L_MVT((*desc),nrows,class1) = nrows;
      L_MVT((*desc),ncols,class1) = ncols;
      NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

      RES_HEAP;
      ipnew =    ((int *) R_MVT((*desc),ptdv,class1));
      rp1 =   ((double *) R_MVT(DESC(arg1),ptdv,class1));
      ip2 =   ((int    *) R_MVT(DESC(arg2),ptdv,class2));

      if (equal)
        for (i = 0; i < dim; i++)
          ipnew[i] = (rp1[i] == ip2[i]);
      else
        for (i = 0; i < dim; i++)
          ipnew[i] = (rp1[i] != ip2[i]);

      REL_HEAP;
      goto success;
    }

  } /** end  type1  != type2  **/

fail:
  END_MODUL("red_f_eq");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  _desc = desc;
  END_MODUL("red_f_eq");
  DBUG_RETURN(1);

} /**** end of function red_f_eq ****/

/**************************  end of file  rlogfunc.c  ************************/
