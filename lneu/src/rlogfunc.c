/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
/*****************************************************************************/
/*                        MODULE FOR PROCESSING PHASE                        */
/* ------------------------------------------------------------------------- */
/* rlogfunc.c -- external  : red_eq,    red_lt,    red_le,    red_min,       */
/*                           red_f_eq,  red_not,   red_or,    red_and,       */
/*                           red_xor,                                        */
/*                           str_eq,    str_le,    str_lt                    */
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
/*  str_le                     red_le,        str_mvt_bop,  mvt_mvt_bop      */
/*  str_lt                     red_lt,        str_mvt_bop,  mvt_mvt_bop      */
/*                                                                           */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rops.h"
#include <setjmp.h>

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

/*--------------------------------------------------------------------------*/
/*                             defines                                      */
/*--------------------------------------------------------------------------*/
/* ------- macros for heapoverflow - handling ---------- */
/* Ein Interrupt bewirkt die Zuruecknahme der Reduktion. */
/* siehe setjmp(_interruptbuf) in der Datei rear.c       */
#define NEWHEAP(size,adr) if (newheap(size,adr) == 0) \
                            longjmp(_interruptbuf,0)
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
      END_MODUL("pm_element");
      return(FALSE);
  success:
      END_MODUL("pm_element");
      return(TRUE);    
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

  if ( (dim = R_LIST((*str1),dim)) != R_LIST((*str2),dim))
    return(FALSE);                                    /* no equal dimensions */

  p1 = R_LIST((*str1),ptdv);
  p2 = R_LIST((*str2),ptdv);

  for (i = 0; i < dim; i++) {         /* comparison of data-vectors          */
    if ((T_POINTER(p1[i]) && (!T_POINTER(p2[i]))) ||
        ((!T_POINTER(p1[i])) && T_POINTER(p2[i]))) {
      return(FALSE);                  /* combination of a pointer and a char */
    }
    else {
      if ((!T_POINTER(p1[i])) && (!T_POINTER(p2[i]))) {
        if ((p1[i] & ~F_EDIT) != (p2[i] & ~F_EDIT))   /* char's not equal    */
          return(FALSE);
      }
      else {
        if (R_LIST(DESC(p1[i]),dim) != R_LIST(DESC(p2[i]),dim)) {
          return(FALSE);                              /* no equal dimensions */
        }
        else {
           if (!str_eq(p1[i],p2[i]))/*recursive call if there are tow pointer*/
             return(FALSE);
        }
      } /** end else pointer pointer **/
    }
  } /** end for **/
  return(TRUE);                                       /* Strings are equal   */

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
        return(TRUE);              /* char1 is le char2                      */
      else
        if ((p1[i] & ~F_EDIT) > (p2[i] & ~F_EDIT))
          return(FALSE);           /* char1 is gt char2                      */
    }
    else {
      if (T_POINTER(p1[i]) && T_POINTER(p2[i])) {
        if (!str_le(p1[i],p2[i]))  /* recursive call if there are tow pointer*/
          return(FALSE);
      }
      else {
        if (T_POINTER(p1[i]) && (!T_POINTER(p2[i])))
          return(FALSE);           /* combination of a pointer and a char    */
        else
          return(TRUE);            /* combination of a char and a pointer    */
      }
    }
  } /** end for **/

  if (dim == dim1)
    return(TRUE);                  /* string1 is less equal than string2     */
  else
    return(FALSE);

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
        return(TRUE);              /* char1 is le char2                      */
      else
        if ((p1[i] & ~F_EDIT) > (p2[i] & ~F_EDIT))
          return(FALSE);           /* char1 is gt char2                      */
    }
    else {
      if (T_POINTER(p1[i]) && T_POINTER(p2[i])) {
        if (str_lt(p1[i],p2[i]))   /* recursive call if there are two pointer*/
          return(TRUE);
        else
          if (str_lt(p2[i],p1[i])) /* recursive call if there are two pointer*/
            return(FALSE);
      }
      else {
        if (T_POINTER(p1[i]) && (!T_POINTER(p2[i])))
          return(FALSE);           /* combination of a pointer and a char    */
        else
          return(TRUE);            /* combination of a char and a pointer    */
      }
    }
  } /** end for **/

  if (dim < dim2)
    return(TRUE);                  /* string1 is less than string2           */
  else
    return(FALSE);

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
red_not(arg1)
register STACKELEM arg1;
{
  char class1;

  START_MODUL("red_not");

  if T_BOOLEAN(arg1) {
    _desc = (PTR_DESCRIPTOR) (T_SA_FALSE(arg1) ? SA_TRUE : SA_FALSE);
    END_MODUL("red_not");
    return(1);
  }

  if (T_POINTER(arg1) && ( ((class1 = R_DESC(DESC(arg1),class)) == C_MATRIX) ||
                            (class1 == C_VECTOR) ||
                            (class1 == C_TVECTOR))
                      && (R_DESC(DESC(arg1),type) == TY_BOOL) ) {

    _desc = (PTR_DESCRIPTOR) boolmvt_bop(arg1,OP_NOT);
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_not");
    return(1);
  }
  END_MODUL("red_not");
  return(0);
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
red_or(arg1,arg2)
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

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
  END_MODUL("red_or");return(0);

success:
  END_MODUL("red_or");
  return(1);
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
red_and(arg1,arg2)
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

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
  return(0);
success:
  END_MODUL("red_and");
  return(1);
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
red_xor(arg1,arg2)
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

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
  END_MODUL("red_xor");return(0);

success:
  END_MODUL("red_xor");
  return(1);
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
red_lt(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
  int op,invop;                         /* inverse Operation bei vertausch- */
                                        /* ten Argumenten                   */
  char class1,class2;

  START_MODUL("red_lt");

                                                                             
    op = OP_LT;                                                                
    invop = OP_GT;                                                             

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    switch(R_DESC(DESC(arg1),class)) {

      case C_SCALAR:
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),vali) < R_SCALAR(DESC(arg2),vali)
                                ? SA_TRUE : SA_FALSE);
                    else
                      result = (R_SCALAR(DESC(arg1),vali) < R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),valr) < R_SCALAR(DESC(arg2),vali)
                                ? SA_TRUE : SA_FALSE);
                    else
                      result = (R_SCALAR(DESC(arg1),valr) < R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,op));
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
                  TEST(result = digit_mvt_bop(arg1,arg2,op));
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
                  TEST(result = scalar_mvt_bop(arg2,arg1,invop));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,invop));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,op));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,invop));
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
                  result = (str_lt(arg1,arg2) ? SA_TRUE : SA_FALSE);
                  REL_HEAP;
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_STRING **/
      default:
           goto fail;
    } /** end switch arg1 == class **/
  } /** end if T_POINTER **/

  if T_BOOLEAN(arg1)
    if T_BOOLEAN(arg2) {
      result = (T_SA_FALSE(arg1) ? arg2 : SA_FALSE);
      goto success;
    }
  else {
    TEST(result = bool_mvt_bop(arg1,arg2,op));
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }

  if T_BOOLEAN(arg2) {
    TEST(result = bool_mvt_bop(arg2,arg1,invop));
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

fail:
  END_MODUL("red_lt"); return(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_lt");
  return(1);
} /**** end of function red_lt ****/

/*****************************************************************************/
/*                        red_ge    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function : Vergleicht zwei Stackelemente bzw. Descriptoren                */
/*            auf '>='.                                                      */
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
red_ge(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
  int op,invop;                         /* inverse Operation bei vertausch- */
                                        /* ten Argumenten                   */
  char class1,class2;

  START_MODUL("red_ge");

                                                                             
    op = OP_GE;                                                                
    invop = OP_LE;                                                             

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    switch(R_DESC(DESC(arg1),class)) {

      case C_SCALAR:
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                   result = (R_SCALAR(DESC(arg1),vali) >= R_SCALAR(DESC(arg2),vali)
                                ? SA_TRUE : SA_FALSE);
                    else
                      result = (R_SCALAR(DESC(arg1),vali) >= R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),valr) >= R_SCALAR(DESC(arg2),vali)
                                ? SA_TRUE : SA_FALSE);
                    else
                      result = (R_SCALAR(DESC(arg1),valr) >= R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                     goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,op));
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
                  TEST(result = digit_mvt_bop(arg1,arg2,op));
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
                  TEST(result = scalar_mvt_bop(arg2,arg1,invop));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,invop));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,op));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,invop));
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
                  result = (str_lt(arg1,arg2) ? SA_FALSE : SA_TRUE);
                  REL_HEAP;
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end case arg1 == C_LIST vom Typ  TY_STRING **/
      default:
           goto fail;
    } /** end switch arg1 == class **/
  } /** end if T_POINTER **/

  if T_BOOLEAN(arg1)
    if T_BOOLEAN(arg2) {
      result = (T_SA_FALSE(arg2) ? SA_TRUE : arg1);
      goto success;
    }
  else {
    TEST(result = bool_mvt_bop(arg1,arg2,op));
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }

  if T_BOOLEAN(arg2) {
    TEST(result = bool_mvt_bop(arg2,arg1,invop));
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

fail:
  END_MODUL("red_ge"); return(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_ge");
  return(1);
} /**** end of function red_ge ****/


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
red_eq(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register BOOLEAN result;
              char class1,class2;

  START_MODUL("red_eq");

  if T_POINTER(arg1)
  {
    class1 = R_DESC(DESC(arg1),class);

    if ((class1 == C_EXPRESSION) || (class1 == C_CONSTANT)) goto fail;
    /*  z.B Applikation         oder  ganz freie Variable */

    if T_POINTER(arg2)
    {
      class2 = R_DESC(DESC(arg2),class);

      if ((class2 == C_EXPRESSION) || (class2 == C_CONSTANT)) goto fail;
      /*  z.B Applikation         oder  ganz freie Variable */

      if (class1 != class2)
      {
        result = FALSE;
        goto decrement;
      }

      switch(class1) {
        case C_SCALAR:
             result = ( R_DESC(DESC(arg1),type) == TY_INTEGER
                        ? ( R_DESC(DESC(arg2),type) == TY_INTEGER
                            ? R_SCALAR(DESC(arg1),vali) == R_SCALAR(DESC(arg2),vali)
                            : R_SCALAR(DESC(arg1),vali) == R_SCALAR(DESC(arg2),valr)
                          )
                        : ( R_DESC(DESC(arg2),type) == TY_INTEGER
                            ? R_SCALAR(DESC(arg1),valr) == R_SCALAR(DESC(arg2),vali)
                            : R_SCALAR(DESC(arg1),valr) == R_SCALAR(DESC(arg2),valr)
                          )
                      );
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
               result = str_eq(arg1,arg2);
               REL_HEAP;
             }
             else {
               if (((PTR_DESCRIPTOR)arg1 == _nil) &&
                   ((PTR_DESCRIPTOR)arg2 == _nil))
                 result = TRUE;
               else {
                 if ((R_LIST(DESC(arg1),dim)  > 0) &&
                     (R_LIST(DESC(arg2),dim)  > 0))
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
               int   nrows, ncols;
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
                          result = str_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                                          R_MVT(DESC(arg2),ptdv,class2)[i]);
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

      if ((class2 == C_EXPRESSION) || (class2 == C_CONSTANT)) goto fail;
      /*  z.B Applikation         oder  ganz freie Variable */

      /* ansonsten: */
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      result = FALSE;
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
  END_MODUL("red_eq"); return(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) (result ? SA_TRUE : SA_FALSE);
  END_MODUL("red_eq");
  return(1);
} /**** end of function red_eq ****/

/*****************************************************************************/
/*                        red_neq    (compare function)                      */
/* ------------------------------------------------------------------------- */
/* function :  Vergleicht zwei Stackelemente auf Ungleichheit.               */
/*             sea further: red_ge                                           */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement  (pointer to result descriptor)  (true / false)  */
/* called by:  rear.c                                                        */
/* calls    :  digit_eq, str_eq                                              */
/*****************************************************************************/
red_neq(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register BOOLEAN result;
              char class1,class2;

  START_MODUL("red_neq");

  if T_POINTER(arg1)
  {
    class1 = R_DESC(DESC(arg1),class);

    if ((class1 == C_EXPRESSION) || (class1 == C_CONSTANT)) goto fail;
    /*  z.B Applikation         oder  ganz freie Variable */

    if T_POINTER(arg2)
    {
      class2 = R_DESC(DESC(arg2),class);

      if ((class2 == C_EXPRESSION) || (class2 == C_CONSTANT)) goto fail;
      /*  z.B Applikation         oder  ganz freie Variable */

      if (class1 != class2)
      {
        result = FALSE;
        goto decrement;
      }

      switch(class1) {
        case C_SCALAR:
             result = ( R_DESC(DESC(arg1),type) == TY_INTEGER
                        ? ( R_DESC(DESC(arg2),type) == TY_INTEGER
                            ? R_SCALAR(DESC(arg1),vali) == R_SCALAR(DESC(arg2),vali)
                            : R_SCALAR(DESC(arg1),vali) == R_SCALAR(DESC(arg2),valr)
                          )
                        : ( R_DESC(DESC(arg2),type) == TY_INTEGER
                            ? R_SCALAR(DESC(arg1),valr) == R_SCALAR(DESC(arg2),vali)
                            : R_SCALAR(DESC(arg1),valr) == R_SCALAR(DESC(arg2),valr)
                          )
                      );
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
               result = str_eq(arg1,arg2);
               REL_HEAP;
             }
             else {
               if (((PTR_DESCRIPTOR)arg1 == _nil) &&
                   ((PTR_DESCRIPTOR)arg2 == _nil))
                 result = TRUE;
               else {
                 if ((R_LIST(DESC(arg1),dim)  > 0) &&
                     (R_LIST(DESC(arg2),dim)  > 0))
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
               int   nrows, ncols;
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
                          result = str_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                                          R_MVT(DESC(arg2),ptdv,class2)[i]);
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
                        post_mortem("red_neq: unknown mvt - type");
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
             post_mortem("red_neq: unexpected descriptor class");
      } /** end switch(class1) **/
    } /** end T_POINTER(arg2) **/
    else {
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

      if ((class2 == C_EXPRESSION) || (class2 == C_CONSTANT)) goto fail;
      /*  z.B Applikation         oder  ganz freie Variable */

      /* ansonsten: */
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
      result = FALSE;
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
  END_MODUL("red_neq"); return(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) (result ? SA_FALSE : SA_TRUE);
  END_MODUL("red_neq");
  return(1);
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
red_le(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
  int op,invop;

  START_MODUL("red_le");

    op = OP_LE; invop = OP_GE;   

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    switch (R_DESC(DESC(arg1),class)) {
      case C_SCALAR:

           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),vali) <= R_SCALAR(DESC(arg2),vali)
                                ? SA_TRUE : SA_FALSE);
                    else
                      result = (R_SCALAR(DESC(arg1),vali) <= R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),vali)
                                ? SA_TRUE : SA_FALSE);
                    else
                      result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,op));
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
                  TEST(result = digit_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/

      case C_VECTOR :
      case C_TVECTOR:
      case C_MATRIX :
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  TEST(result = scalar_mvt_bop(arg2,arg1,invop));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,invop));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,op));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,invop));
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
                  result = (str_le(arg1,arg2) ? SA_TRUE : SA_FALSE);
                  REL_HEAP;
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,op));
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
    TEST(result = bool_mvt_bop(arg1,arg2,op));
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }

  if T_BOOLEAN(arg2) {
    TEST(result = bool_mvt_bop(arg2,arg1,invop));
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

fail:
  END_MODUL("red_le"); return(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_le");
  return(1);
} /**** end of function red_le ****/

/*****************************************************************************/
/*                        red_gt    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function :  Vergleicht zwei Stackelemente bzw Descriptoren                */
/*             auf '>'.                                                      */
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
red_gt(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
  int op,invop;

  START_MODUL("red_gt");

    op = OP_GT; invop = OP_LT;   

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    switch (R_DESC(DESC(arg1),class)) {
      case C_SCALAR:

           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),vali) > R_SCALAR(DESC(arg2),vali)
                                ? SA_TRUE : SA_FALSE);
                    else
                      result = (R_SCALAR(DESC(arg1),vali) > R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),vali)
                                ? SA_TRUE : SA_FALSE);
                    else
                      result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),valr)
                                ? SA_TRUE : SA_FALSE);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,op));
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
                  TEST(result = digit_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch  class von arg2 **/

      case C_VECTOR :
      case C_TVECTOR:
      case C_MATRIX :
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  TEST(result = scalar_mvt_bop(arg2,arg1,invop));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,invop));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,op));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,invop));
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
                  result = (str_le(arg1,arg2) ? SA_FALSE : SA_TRUE);
                  REL_HEAP;
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,op));
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
      result = (T_SA_TRUE(arg2) ? SA_FALSE : arg1);
      goto success;
    }
  else {
    TEST(result = bool_mvt_bop(arg1,arg2,op));
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }

  if T_BOOLEAN(arg2) {
    TEST(result = bool_mvt_bop(arg2,arg1,invop));
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

fail:
  END_MODUL("red_gt"); return(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_gt");
  return(1);
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
red_min(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
  int op;

  START_MODUL("red_min");

  op = OP_MIN ;

  if (T_POINTER(arg1) && T_POINTER(arg2)) {
    switch (R_DESC(DESC(arg1),class)) {

      case C_SCALAR:
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),vali) <= R_SCALAR(DESC(arg2),vali)
                                ? arg1 : arg2);
                    else
                      result = (R_SCALAR(DESC(arg1),vali) <= R_SCALAR(DESC(arg2),valr)
                                ? arg1 : arg2);
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),vali)
                                ? arg1 : arg2);
                    else
                      result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),valr)
                                ? arg1 : arg2);
                  if (op == OP_MAX)
                    result = (result == arg1 ? arg2 : arg1);
                  INC_REFCNT((PTR_DESCRIPTOR)result);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch class von arg2 **/

      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class)) {
             case C_DIGIT:
                  result = (digit_le(arg1,arg2) ?  arg1 : arg2);
                  INC_REFCNT((PTR_DESCRIPTOR)result);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = digit_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch class von arg2 **/

      case C_VECTOR :
      case C_TVECTOR:
      case C_MATRIX :
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  TEST(result = scalar_mvt_bop(arg2,arg1,op));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,op));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,op));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch class von arg2 **/

      case C_LIST  :
           if (R_DESC(DESC(arg1),type) != TY_STRING)
             goto fail;
           switch(R_DESC(DESC(arg2),class)) {
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  RES_HEAP;
                  result = (str_lt(arg1,arg2)
                                 ? arg1 : arg2);
                  REL_HEAP;
                  INC_REFCNT((PTR_DESCRIPTOR)result);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch class von arg2 **/
      default:
           goto fail;
    } /** end switch class von arg1 **/
  }  /** end if T_POINTER von arg1/arg1 **/

  if T_BOOLEAN(arg1)
    if T_BOOLEAN(arg2) {
        result = (T_SA_FALSE(arg1) ? SA_FALSE : arg2);
      goto success;
    }
  else {
    TEST(result = bool_mvt_bop(arg1,arg2,op));
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }

  if T_BOOLEAN(arg2) {
    TEST(result = bool_mvt_bop(arg2,arg1,op));
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

fail:
  END_MODUL("red_min"); return(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_min");
  return(1);
} /**** end of function red_min ****/

/*****************************************************************************/
/*                        red_max   (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function : Ermittelt das Maximum zweier Argumente,                        */
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
red_max(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
  int op;

  START_MODUL("red_max");

  op = OP_MAX ;

  if (T_POINTER(arg1) && T_POINTER(arg2)) {
    switch (R_DESC(DESC(arg1),class)) {

      case C_SCALAR:
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  if (R_DESC(DESC(arg1),type) == TY_INTEGER)
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),vali) > R_SCALAR(DESC(arg2),vali)
                                ? arg1 : arg2);
                    else
                      result = (R_SCALAR(DESC(arg1),vali) > R_SCALAR(DESC(arg2),valr)
                                ? arg1 : arg2);
                  else
                    if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                      result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),vali)
                                ? arg1 : arg2);
                    else
                      result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),valr)
                                ? arg1 : arg2);
                  INC_REFCNT((PTR_DESCRIPTOR)result);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = scalar_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch class von arg2 **/

      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class)) {
             case C_DIGIT:
                  result = (digit_le(arg1,arg2) ? arg2 : arg1);
                  INC_REFCNT((PTR_DESCRIPTOR)result);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = digit_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch class von arg2 **/

      case C_VECTOR :
      case C_TVECTOR:
      case C_MATRIX :
           switch(R_DESC(DESC(arg2),class)) {
             case C_SCALAR:
                  TEST(result = scalar_mvt_bop(arg2,arg1,op));
                  goto decrement;
             case C_DIGIT:
                  TEST(result = digit_mvt_bop(arg2,arg1,op));
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = mvt_mvt_bop(arg1,arg2,op));
                  goto decrement;
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  TEST(result = str_mvt_bop(arg2,arg1,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch class von arg2 **/

      case C_LIST  :
           if (R_DESC(DESC(arg1),type) != TY_STRING)
             goto fail;
           switch(R_DESC(DESC(arg2),class)) {
             case C_LIST  :
                  if (R_DESC(DESC(arg2),type) != TY_STRING)
                    goto fail;
                  RES_HEAP;
                  result = (str_lt(arg1,arg2)
                                 ? arg2 : arg1);
                  REL_HEAP;
                  INC_REFCNT((PTR_DESCRIPTOR)result);
                  goto decrement;
             case C_VECTOR :
             case C_TVECTOR:
             case C_MATRIX :
                  TEST(result = str_mvt_bop(arg1,arg2,op));
                  goto decrement;
             default:
                  goto fail;
           } /** end switch class von arg2 **/
      default:
           goto fail;
    } /** end switch class von arg1 **/
  }  /** end if T_POINTER von arg1/arg1 **/

  if T_BOOLEAN(arg1)
    if T_BOOLEAN(arg2) {
        result = (T_SA_TRUE(arg1) ? SA_TRUE : arg2);
      goto success;
    }
  else {
    TEST(result = bool_mvt_bop(arg1,arg2,op));
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }

  if T_BOOLEAN(arg2) {
    TEST(result = bool_mvt_bop(arg2,arg1,op));
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

fail:
  END_MODUL("red_max"); return(0);

decrement:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_max");
  return(1);
} /**** end of function red_max ****/


                  
/*****************************************************************************/
/*      red_f_eq   (compare function for matrices, vectors and tvectors)     */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/*             Diese Funktion ist auf Matrizen, Vektoren und                 */
/*             TVektoren des Typs Bool, Decimal und String definiert.        */
/*             Die zu vergleichenden Matrizen muessen die gleiche Zeilen-    */
/*             und Spaltenzahl. Bei Vektoren bzw. TVektoren muss die         */
/*             Dimension der beiden Argumente gleich sein.                   */
/*             Das Ergebnis des Vergleichs ist im Gegensatz zu den Funk-     */
/*             tionen eq und ne ...  eine Matrix oder ein Vektor oder ein    */
/*             TVektor mit Wahrheitswerten, deren Elemente aus Einzelver-    */
/*             vergleichen der Argumentelementebe stimmt wird.               */
/*             Der Vergleich von Matrizen mit mehr als zwei Dimensionen ist  */
/*             nicht erlaubt.                                                */
/*                                                                           */
/*             f_eq       :   mat    x   mat        -->  bmat                */
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
red_f_eq(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register int i,dim;
  int nrows, ncols;
  int type1,type2;                   /* fuer den Typ von arg1  bzw.  arg2    */
  int *ip1, *ip2, *ipnew;            /* "integer pointer" von arg1 / arg2    */
  double *rp1, *rp2;                 /*    "real pointer" von arg1 / arg2    */
  PTR_DESCRIPTOR desc;
     char     class1,class2;         /* fuer die Klasse von   arg1 / arg2    */

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

     /* RES_HEAP ist im folgenden DIGIT-case  nicht erlaubt, da digit_eq,    */
     /* bzw. str_eq dies schon tun                                           */

      case TY_DIGIT  :
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class1,TY_BOOL);
           L_MVT((*desc),nrows,class1) = nrows;
           L_MVT((*desc),ncols,class1) = ncols;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

             for (i = 0; i < dim; i++)
               ((int *)R_MVT((*desc),ptdv,class1))[i] =
                     digit_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                              R_MVT(DESC(arg2),ptdv,class2)[i]);
           goto success;

      case TY_STRING :
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class1,TY_BOOL);
           L_MVT((*desc),nrows,class1) = nrows;
           L_MVT((*desc),ncols,class1) = ncols;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

           RES_HEAP;

           for (i = 0; i < dim; i++)
               ((int *)R_MVT((*desc),ptdv,class1))[i] =
                       str_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                              R_MVT(DESC(arg2),ptdv,class2)[i]);
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

             for (i = 0; i < dim; i++)
               ipnew[i] = (ip1[i] ? ip2[i] : !ip2[i]);

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

             for (i = 0; i < dim; i++)
               ipnew[i] = (ip1[i] == ip2[i]);

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

             for (i = 0; i < dim; i++)
               ipnew[i] = (rp1[i] == rp2[i]);

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

        for (i = 0; i < dim; i++)
          ipnew[i] = (ip1[i] == rp2[i]);

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

        for (i = 0; i < dim; i++)
          ipnew[i] = (rp1[i] == ip2[i]);

      REL_HEAP;
      goto success;
    }

  } /** end  type1  != type2  **/

fail:
  END_MODUL("red_f_eq");
  return(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  _desc = desc;
  END_MODUL("red_f_eq");
  return(1);

} /**** end of function red_f_eq ****/

                  
/*****************************************************************************/
/*      red_f_ne       (compare function for matrices, vectors and tvectors) */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/*             Diese Funktion ist auf Matrizen, Vektoren und                 */
/*             TVektoren des Typs Bool, Decimal und String definiert.        */
/*             Die zu vergleichenden Matrizen muessen die gleiche Zeilen-    */
/*             und Spaltenzahl. Bei Vektoren bzw. TVektoren muss die         */
/*             Dimension der beiden Argumente gleich sein.                   */
/*             Das Ergebnis des Vergleichs ist im Gegensatz zu den Funk-     */
/*             tionen eq und ne ...  eine Matrix oder ein Vektor oder ein    */
/*             TVektor mit Wahrheitswerten, deren Elemente aus Einzelver-    */
/*             vergleichen der Argumentelementebe stimmt wird.               */
/*             Der Vergleich von Matrizen mit mehr als zwei Dimensionen ist  */
/*             nicht erlaubt.                                                */
/*                                                                           */
/*                   f_ne :   mat    x   mat        -->  bmat                */
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
red_f_ne(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register int i,dim;
  int nrows, ncols;
  int type1,type2;                   /* fuer den Typ von arg1  bzw.  arg2    */
  int *ip1, *ip2, *ipnew;            /* "integer pointer" von arg1 / arg2    */
  double *rp1, *rp2;                 /*    "real pointer" von arg1 / arg2    */
  PTR_DESCRIPTOR desc;
     char     class1,class2;         /* fuer die Klasse von   arg1 / arg2    */

  START_MODUL("red_f_ne");

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

     /* RES_HEAP ist im folgenden DIGIT-case  nicht erlaubt, da digit_eq,    */
     /* bzw. str_eq dies schon tun                                           */

      case TY_DIGIT  :
           NEWDESC(_desc); INIT_DESC;
           MVT_DESC_MASK(desc,1,class1,TY_BOOL);
           L_MVT((*desc),nrows,class1) = nrows;
           L_MVT((*desc),ncols,class1) = ncols;
           NEWHEAP(dim,A_MVT((*desc),ptdv,class1));

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

             for (i = 0; i < dim; i++)
               ((int *)R_MVT((*desc),ptdv,class1))[i] = !
                       str_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                              R_MVT(DESC(arg2),ptdv,class2)[i]);

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

             for (i = 0; i < dim; i++)
               ipnew[i] = (rp1[i] != rp2[i]);

           REL_HEAP;
           goto success;

      default  : post_mortem("red_f_ne: unknown mvt - type");
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

        for (i = 0; i < dim; i++)
          ipnew[i] = (rp1[i] != ip2[i]);

      REL_HEAP;
      goto success;
    }

  } /** end  type1  != type2  **/

fail:
  END_MODUL("red_f_ne");
  return(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  _desc = desc;
  END_MODUL("red_f_ne");
  return(1);

} /**** end of function red_f_ne ****/

    

/*--------------------------------------------------------------------*/
/* red_dummy -- wird aufgerufen, falls eine Funktion nicht implemen-  */
/*              tiert ist.Diese primitive Funktion scheitert immer    */
/*              und liefert deshalb eine Null zurueck.                */
/*--------------------------------------------------------------------*/

int red_dummy()
{
  START_MODUL("red_dummy");

  END_MODUL("red_dummy");
  return (0);
}

/**************************  end of file  rlogfunc.c  ************************/
