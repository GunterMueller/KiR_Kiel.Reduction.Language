/*****************************************************************************/
/*                        MODULE FOR PROCESSING PHASE                        */
/* ------------------------------------------------------------------------- */
/* rlogfunc.c -- external  : red_eq,    red_lt,    red_le,    red_min,       */
/*                           red_not,   red_or,    red_and,   red_xor        */
/*                           str_eq,    str_le,    str_lt                    */
/*                                                                           */
/* ========================================================================= */
/*              Kurze Beschreibung der Aufgaben der Funktionen               */
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
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/* called by:  rear.c                                                        */
/*****************************************************************************/
red_not(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_not");

  if T_BOOLEAN(arg1) {
    _desc = (PTR_DESCRIPTOR) (T_SA_FALSE(arg1) ? SA_TRUE : SA_FALSE);
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
/*             anwendbar. Realisiert logisches OR.                           */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/* called by:  rear.c                                                        */
/*****************************************************************************/
red_or(arg1,arg2)
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

  START_MODUL("red_or");

  if (T_BOOLEAN(arg1) && T_BOOLEAN(arg2)) {
      /* zwei Booleans */
      _desc = (PTR_DESCRIPTOR) (T_SA_TRUE(arg1) || T_SA_TRUE(arg2)
                                                      ? SA_TRUE : SA_FALSE);
      END_MODUL("red_or");
      return(1);
    }
  END_MODUL("red_or");return(0);
} /**** end of function red_or ****/


/*****************************************************************************/
/*             red_and   (logical function with boolean arguments)           */
/* ------------------------------------------------------------------------- */
/* function :  Diese Funktionen sind nur auf Argumente vom Typ Bool          */
/*             anwendbar. Realisiert logisches AND.                          */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/* called by:  rear.c                                                        */
/*****************************************************************************/
red_and(arg1,arg2)
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

  START_MODUL("red_or");

  if (T_BOOLEAN(arg1) && T_BOOLEAN(arg2)) {
      /* zwei Booleans */
      _desc = (PTR_DESCRIPTOR) (T_SA_TRUE(arg1) && T_SA_TRUE(arg2)
                                                      ? SA_TRUE : SA_FALSE);
      END_MODUL("red_and");
      return(1);
    }
  END_MODUL("red_and");return(0);
} /**** end of function red_and ****/


/*****************************************************************************/
/*             red_xor   (logical function with boolean arguments)           */
/* ------------------------------------------------------------------------- */
/* function :  Diese Funktionen sind nur auf Argumente vom Typ Bool          */
/*             anwendbar. Realisiert logisches XOR.                          */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/* called by:  rear.c                                                        */
/*****************************************************************************/
red_xor(arg1,arg2)
register STACKELEM arg1,arg2;
{
  STACKELEM result;
       char class1,class2;

  START_MODUL("red_xor");

  if (T_BOOLEAN(arg1) && T_BOOLEAN(arg2)) {
      /* zwei Booleans */
      _desc = (PTR_DESCRIPTOR) (T_SA_TRUE(arg1) != T_SA_TRUE(arg2)
                                                      ? SA_TRUE : SA_FALSE);
      END_MODUL("red_xor");
      return(1);
    }
  END_MODUL("red_xor");return(0);
} /**** end of function red_xor ****/


/*****************************************************************************/
/*                        red_lt    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function : Vergleicht zwei Stackelemente bzw. Descriptoren                */
/*            auf '<'.                                                      */
/*                                                                           */
/*            further:                                                       */
/*            Diese Funktionen vergleichen Datenobjekte vom Typ Bool,        */
/*            Integer, Real und String.                                      */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement (pointer to result descriptor)  (true / false)   */
/* called by:  rear.c                                                        */
/* calls    :  str_lt                                                        */
/*****************************************************************************/
red_lt(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;

  START_MODUL("red_lt");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    if ((R_DESC(DESC(arg1),class)==C_SCALAR) && (R_DESC(DESC(arg2),class)==C_SCALAR)) {
        if (R_DESC(DESC(arg1),type) == TY_INTEGER)
          if (R_DESC(DESC(arg2),type) == TY_INTEGER)
            result = (R_SCALAR(DESC(arg1),vali) < R_SCALAR(DESC(arg2),vali) ? SA_TRUE : SA_FALSE);
          else
            result = (R_SCALAR(DESC(arg1),vali) < R_SCALAR(DESC(arg2),valr) ? SA_TRUE : SA_FALSE);
        else
          if (R_DESC(DESC(arg2),type) == TY_INTEGER)
            result = (R_SCALAR(DESC(arg1),valr) < R_SCALAR(DESC(arg2),vali) ? SA_TRUE : SA_FALSE);
          else
            result = (R_SCALAR(DESC(arg1),valr) < R_SCALAR(DESC(arg2),valr) ? SA_TRUE : SA_FALSE);
    	}
    else
      if ((R_DESC(DESC(arg1),class)==C_LIST) && (R_DESC(DESC(arg2),class)==C_LIST)
           && (R_DESC(DESC(arg1),type) != TY_STRING) && (R_DESC(DESC(arg2),type) != TY_STRING)) {
         RES_HEAP;
         result = (str_lt(arg1,arg2) ? SA_TRUE : SA_FALSE);
         REL_HEAP;
		}
    else
      { END_MODUL("red_lt"); return(0); }
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    } /** end if T_POINTER **/
  else
    if (T_BOOLEAN(arg1) && T_BOOLEAN(arg2))
      result = (T_SA_FALSE(arg1) ? arg2 : SA_FALSE);
  else
    { END_MODUL("red_lt"); return(0); }

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
/*            Integer, Real und String.                                      */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement (pointer to result descriptor)  (true / false)   */
/* called by:  rear.c                                                        */
/* calls    :  str_lt                                                        */
/*****************************************************************************/
red_ge(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;

  START_MODUL("red_ge");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    if ((R_DESC(DESC(arg1),class)==C_SCALAR) && (R_DESC(DESC(arg2),class)==C_SCALAR)) {
        if (R_DESC(DESC(arg1),type) == TY_INTEGER)
          if (R_DESC(DESC(arg2),type) == TY_INTEGER)
            result = (R_SCALAR(DESC(arg1),vali) >= R_SCALAR(DESC(arg2),vali) ? SA_TRUE : SA_FALSE);
          else
            result = (R_SCALAR(DESC(arg1),vali) >= R_SCALAR(DESC(arg2),valr) ? SA_TRUE : SA_FALSE);
        else
          if (R_DESC(DESC(arg2),type) == TY_INTEGER)
            result = (R_SCALAR(DESC(arg1),valr) >= R_SCALAR(DESC(arg2),vali) ? SA_TRUE : SA_FALSE);
          else
            result = (R_SCALAR(DESC(arg1),valr) >= R_SCALAR(DESC(arg2),valr) ? SA_TRUE : SA_FALSE);
    	}
    else
      if ((R_DESC(DESC(arg1),class)==C_LIST) && (R_DESC(DESC(arg2),class)==C_LIST)
           && (R_DESC(DESC(arg1),type) != TY_STRING) && (R_DESC(DESC(arg2),type) != TY_STRING)) {
         RES_HEAP;
         result = (str_lt(arg1,arg2) ? SA_FALSE : SA_TRUE);
         REL_HEAP;
		}
    else
      { END_MODUL("red_ge"); return(0); }
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    } /** end if T_POINTER **/
  else
    if (T_BOOLEAN(arg1) && T_BOOLEAN(arg2))
      result = (T_SA_FALSE(arg2) ? SA_TRUE : arg1);
  else
    { END_MODUL("red_ge"); return(0); }

  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_ge");
  return(1);
} /**** end of function red_ge ****/


/*****************************************************************************/
/*                        red_le    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function : Vergleicht zwei Stackelemente bzw. Descriptoren                */
/*            auf '<='.                                                      */
/*                                                                           */
/*            further:                                                       */
/*            Diese Funktionen vergleichen Datenobjekte vom Typ Bool,        */
/*            Integer, Real und String.                                      */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement (pointer to result descriptor)  (true / false)   */
/* called by:  rear.c                                                        */
/* calls    :  str_lt                                                        */
/*****************************************************************************/
red_le(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;

  START_MODUL("red_le");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    if ((R_DESC(DESC(arg1),class)==C_SCALAR) && (R_DESC(DESC(arg2),class)==C_SCALAR)) {
        if (R_DESC(DESC(arg1),type) == TY_INTEGER)
          if (R_DESC(DESC(arg2),type) == TY_INTEGER)
            result = (R_SCALAR(DESC(arg1),vali) <= R_SCALAR(DESC(arg2),vali) ? SA_TRUE : SA_FALSE);
          else
            result = (R_SCALAR(DESC(arg1),vali) <= R_SCALAR(DESC(arg2),valr) ? SA_TRUE : SA_FALSE);
        else
          if (R_DESC(DESC(arg2),type) == TY_INTEGER)
            result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),vali) ? SA_TRUE : SA_FALSE);
          else
            result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),valr) ? SA_TRUE : SA_FALSE);
    	}
    else
      if ((R_DESC(DESC(arg1),class)==C_LIST) && (R_DESC(DESC(arg2),class)==C_LIST)
           && (R_DESC(DESC(arg1),type) != TY_STRING) && (R_DESC(DESC(arg2),type) != TY_STRING)) {
         RES_HEAP;
         result = (str_le(arg1,arg2) ? SA_FALSE : SA_TRUE);
         REL_HEAP;
		}
    else
      { END_MODUL("red_ge"); return(0); }
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    } /** end if T_POINTER **/
  else
    if (T_BOOLEAN(arg1) && T_BOOLEAN(arg2))
      result = (T_SA_FALSE(arg1) ? SA_TRUE : arg2);
  else
    { END_MODUL("red_le"); return(0); }

  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_le");
  return(1);
} /**** end of function red_le ****/



/*****************************************************************************/
/*                        red_gt    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function : Vergleicht zwei Stackelemente bzw. Descriptoren                */
/*            auf '>'.                                                      */
/*                                                                           */
/*            further:                                                       */
/*            Diese Funktionen vergleichen Datenobjekte vom Typ Bool,        */
/*            Integer, Real und String.                                      */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement (pointer to result descriptor)  (true / false)   */
/* called by:  rear.c                                                        */
/* calls    :  str_lt                                                        */
/*****************************************************************************/
red_gt(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;

  START_MODUL("red_gt");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    if ((R_DESC(DESC(arg1),class)==C_SCALAR) && (R_DESC(DESC(arg2),class)==C_SCALAR)) {
        if (R_DESC(DESC(arg1),type) == TY_INTEGER)
          if (R_DESC(DESC(arg2),type) == TY_INTEGER)
            result = (R_SCALAR(DESC(arg1),vali) > R_SCALAR(DESC(arg2),vali) ? SA_TRUE : SA_FALSE);
          else
            result = (R_SCALAR(DESC(arg1),vali) > R_SCALAR(DESC(arg2),valr) ? SA_TRUE : SA_FALSE);
        else
          if (R_DESC(DESC(arg2),type) == TY_INTEGER)
            result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),vali) ? SA_TRUE : SA_FALSE);
          else
            result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),valr) ? SA_TRUE : SA_FALSE);
    	}
    else
      if ((R_DESC(DESC(arg1),class)==C_LIST) && (R_DESC(DESC(arg2),class)==C_LIST)
           && (R_DESC(DESC(arg1),type) != TY_STRING) && (R_DESC(DESC(arg2),type) != TY_STRING)) {
         RES_HEAP;
         result = (str_le(arg1,arg2) ? SA_FALSE : SA_TRUE);
         REL_HEAP;
		}
    else
      { END_MODUL("red_gt"); return(0); }
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    } /** end if T_POINTER **/
  else
    if (T_BOOLEAN(arg1) && T_BOOLEAN(arg2))
      result = (T_SA_TRUE(arg2) ? SA_FALSE : arg1);
  else
    { END_MODUL("red_gt"); return(0); }

  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_gt");
  return(1);
} /**** end of function red_gt ****/


/*****************************************************************************/
/*                        red_eq    (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function :  Vergleicht zwei Stackelemente auf Gleichheit.                 */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement  (pointer to result descriptor)  (true / false)  */
/* called by:  rear.c                                                        */
/* calls    :  str_eq                                                        */
/*****************************************************************************/
red_eq(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register BOOLEAN result;
              char class1,class2;

  START_MODUL("red_eq");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((class1==C_SCALAR) && (class1==C_SCALAR))
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
     else if ((class1==C_LIST) && (class2==C_LIST) &&
               (R_DESC(DESC(arg1),type) == TY_STRING) && (R_DESC(DESC(arg2),type) == TY_STRING)) {
             RES_HEAP;
             result = str_eq(arg1,arg2);
             REL_HEAP;
           }
     else
        { END_MODUL("red_eq"); return(0); }
     }
  else if (T_POINTER(arg1) || T_POINTER(arg2) || T_CON(arg1) || T_CON(arg2))
      { END_MODUL("red_eq"); return(0); }
  else
      { result = ((arg1 & ~F_EDIT) == (arg2 & ~F_EDIT)); goto success; }

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
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement  (pointer to result descriptor)  (true / false)  */
/* called by:  rear.c                                                        */
/* calls    :  str_eq                                                        */
/*****************************************************************************/
red_neq(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register BOOLEAN result;
              char class1,class2;

  START_MODUL("red_neq");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((class1==C_SCALAR) && (class1==C_SCALAR))
        result = ( R_DESC(DESC(arg1),type) == TY_INTEGER
                 ? ( R_DESC(DESC(arg2),type) == TY_INTEGER
                     ? R_SCALAR(DESC(arg1),vali) != R_SCALAR(DESC(arg2),vali)
                     : R_SCALAR(DESC(arg1),vali) != R_SCALAR(DESC(arg2),valr)
                   )
                 : ( R_DESC(DESC(arg2),type) == TY_INTEGER
                     ? R_SCALAR(DESC(arg1),valr) != R_SCALAR(DESC(arg2),vali)
                     : R_SCALAR(DESC(arg1),valr) != R_SCALAR(DESC(arg2),valr)
                   )
                 );
     else if ((class1==C_LIST) && (class2==C_LIST) &&
               (R_DESC(DESC(arg1),type) == TY_STRING) && (R_DESC(DESC(arg2),type) == TY_STRING)) {
             RES_HEAP;
             result = str_eq(arg1,arg2);
             REL_HEAP;
           }
     else
        { END_MODUL("red_neq"); return(0); }
     }
  else if (T_POINTER(arg1) || T_POINTER(arg2) || T_CON(arg1) || T_CON(arg2))
      { END_MODUL("red_neq"); return(0); }
  else
      { result = ((arg1 & ~F_EDIT) == (arg2 & ~F_EDIT)); goto success; }

  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) (result ? SA_FALSE : SA_TRUE);
  END_MODUL("red_neq");
  return(1);
} /**** end of function red_neq ****/


/*****************************************************************************/
/*                        red_min   (compare function)                       */
/* ------------------------------------------------------------------------- */
/* function : Ermittelt das Minimum zweier Argumente,                        */
/*            unter Verwendung der Ordnung auf den Typen Boolean,            */
/*            Decimal und String.                                            */
/*                                                                           */
/*    max und min sind auf Datenobjekte vom Typ Bool, Decimal, und String    */
/*    anwendbar.                                                             */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/* called by:  rear.c                                                        */
/* calls    :  str_lt                                                        */
/*****************************************************************************/
red_min(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
           char class1,class2;

  START_MODUL("red_min");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((R_DESC(DESC(arg1),class)==C_SCALAR) && (R_DESC(DESC(arg2),class)==C_SCALAR)) {
        if (R_DESC(DESC(arg1),type) == TY_INTEGER)
            if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                result = (R_SCALAR(DESC(arg1),vali) <= R_SCALAR(DESC(arg2),vali) ? arg1 : arg2);
            else
                result = (R_SCALAR(DESC(arg1),vali) <= R_SCALAR(DESC(arg2),valr) ? arg1 : arg2);
        else
            if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),vali) ? arg1 : arg2);
            else
                result = (R_SCALAR(DESC(arg1),valr) <= R_SCALAR(DESC(arg2),valr) ? arg1 : arg2);
        INC_REFCNT((PTR_DESCRIPTOR)result);
        }
    else if ((class1==C_LIST) && (class2==C_LIST) &&
           (R_DESC(DESC(arg1),type) == TY_STRING) && (R_DESC(DESC(arg2),type) == TY_STRING)) {
        RES_HEAP;
        result = (str_lt(arg1,arg2) ? arg1 : arg2);
        REL_HEAP;
        INC_REFCNT((PTR_DESCRIPTOR)result);
        }
    else
        { END_MODUL("red_min"); return(0); }
    }
  else if (T_BOOLEAN(arg1) &&  T_BOOLEAN(arg2)) {
      result = (T_SA_FALSE(arg1) ? SA_FALSE : arg2);
      goto success;
    }
  else
    { END_MODUL("red_min"); return(0); }

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
/*    anwendbar.                                                             */
/*                                                                           */
/* parameter:  arg1 --  argument from stack                                  */
/*             arg2 --  argument from stack                                  */
/* returns  :  stackelement   (pointer to result descriptor)                 */
/* called by:  rear.c                                                        */
/* calls    :  str_lt                                                        */
/*****************************************************************************/
red_max(arg1,arg2)
register STACKELEM arg1,arg2;
{
  register STACKELEM result;
           char class1,class2;

  START_MODUL("red_max");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {
    class1 = R_DESC(DESC(arg1),class); class2 = R_DESC(DESC(arg2),class);
    if ((R_DESC(DESC(arg1),class)==C_SCALAR) && (R_DESC(DESC(arg2),class)==C_SCALAR)) {
        if (R_DESC(DESC(arg1),type) == TY_INTEGER)
            if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                result = (R_SCALAR(DESC(arg1),vali) > R_SCALAR(DESC(arg2),vali) ? arg1 : arg2);
            else
                result = (R_SCALAR(DESC(arg1),vali) > R_SCALAR(DESC(arg2),valr) ? arg1 : arg2);
        else
            if (R_DESC(DESC(arg2),type) == TY_INTEGER)
                result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),vali) ? arg1 : arg2);
            else
                result = (R_SCALAR(DESC(arg1),valr) > R_SCALAR(DESC(arg2),valr) ? arg1 : arg2);
        INC_REFCNT((PTR_DESCRIPTOR)result);
        }
    else if ((class1==C_LIST) && (class2==C_LIST) &&
           (R_DESC(DESC(arg1),type) == TY_STRING) && (R_DESC(DESC(arg2),type) == TY_STRING)) {
        RES_HEAP;
        result = (str_lt(arg1,arg2) ? arg2 : arg1);
        REL_HEAP;
        INC_REFCNT((PTR_DESCRIPTOR)result);
        }
    else
        { END_MODUL("red_max"); return(0); }
    }
  else if (T_BOOLEAN(arg1) &&  T_BOOLEAN(arg2)) {
      result = (T_SA_TRUE(arg1) ? SA_TRUE : arg2);
      goto success;
    }
  else
    { END_MODUL("red_max"); return(0); }

  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success:
  _desc = (PTR_DESCRIPTOR) result;
  END_MODUL("red_max");
  return(1);
} /**** end of function red_max ****/


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
