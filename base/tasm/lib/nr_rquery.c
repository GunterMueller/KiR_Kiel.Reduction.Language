/*****************************************************************************/
/* all decrements of the refcounts for the arguments removed                 */
/* original was rquery.c, difference is that here won't be any               */
/* reference counting [sf 080595]                                            */
/*****************************************************************************/
/*                       MODUL FOR PROCESSING PHASE                          */
/* ------------------------------------------------------------------------- */
/* rquery.c -- external : red_empty, red_type, red_class,                    */
/*                        red_ldim,  red_vdim,  red_mdim, red_dim            */
/*                        failure                                            */
/*          -- called by: rear.c                                             */
/*          -- internal : none;                                              */
/*                                                                           */
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
#include "rmeasure.h"
#include <setjmp.h>
#include "rlmvt.h"

#include "dbug.h"

extern int failure();

#if D_MESS
#include "d_mess_io.h"
#endif

extern BOOLEAN _formated;              /* <r>, for function ldim, dim, vdim  */
extern PTR_DESCRIPTOR conv_int_digit();            /* rdig-v1.c              */

/* ach 30/10/92 */
int nr_red_ldim();
int nr_red_vdim();
int nr_red_mdim();
extern int stack_error();                 /* rstack.c */
/* end of ach */

#if D_SLAVE
int _argsneeded;
#endif

#if DEBUG
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

#if D_SLAVE
#define READ_A()       D_READSTACK(D_S_A)
#define PUSH_A(arg)    D_PUSHSTACK(D_S_A,arg)
#define STACK_I        D_S_I
#define WRITE_I(arg)   D_WRITESTACK(D_S_I,arg)
#define READ_I()       D_READSTACK(D_S_I)
#define PUSH_I(arg)    D_PUSHSTACK(D_S_I,arg)
#define POP_H()        D_POPSTACK(D_S_H)
#else
#define READ_A()       READSTACK(S_a)
#define PUSH_A(arg)    PUSHSTACK(S_a,arg)
#define STACK_I        S_m1
#define WRITE_I(arg)   WRITESTACK(S_m1,arg)

/* keene Ahnung warum, aber bitte : */
#ifdef READ_I
#undef READ_I
#endif
#ifdef PUSH_I
#undef PUSH_I
#endif

#define READ_I()       READSTACK(S_m1)
#define PUSH_I(arg)    PUSHSTACK(S_m1,arg)
#define POP_H()        POPSTACK(S_e)
#endif


/*****************************************************************************/
/*                  red_dim       (universelle dim Funktion)                 */
/* ------------------------------------------------------------------------- */
/* function  : Programmverteiler auf die einzelnen speziellen dim-Funktionen */
/*             parameter :               calls :                             */
/*      dim  : 0     x list   -->  N     red_ldim                            */
/*             0     x string -->  N     red_ldim                            */
/*             0     x vect   -->  N     red_vdim                            */
/*             0     x tvect  -->  N     red_vdim                            */
/*             {1,2} x smat   -->  N     red_mdim                            */
/*             {1,2} x mmat   -->  N     red_mdim                            */
/*                                                                           */
/* returns   :  stackelemnt  (pointer to result descriptor)                  */
/* called by :  rear.c                                                       */
/*****************************************************************************/
int nr_red_dim(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int par1;                                 /* structuring parameter of arg1 */

  DBUG_ENTER ("nr_red_dim");

  START_MODUL("nr_red_dim");

  if (T_POINTER(arg2)) {
    STRUCT_ARG(arg1,par1);
    switch(R_DESC(DESC(arg2),class)) {
       case C_LIST   : if (par1 != 0) goto fail;
                       if (nr_red_ldim(arg2) == 0)
                        goto fail;
                       break;
       case C_VECTOR :
       case C_TVECTOR: if (par1 != 0) goto fail;
                       if (nr_red_vdim(arg2) == 0)
                         goto fail;
                       break;
       case C_MATRIX : if (nr_red_mdim(arg1,arg2) == 0)
                         goto fail;
                       break;
       default       : goto fail;
    } /** end switch class arg2 **/
    /* success : */
    END_MODUL("nr_red_dim");
    DBUG_RETURN(1);
  } /** T_POINTER arg2 **/
fail:
  END_MODUL("nr_red_dim");
  DBUG_RETURN(0);
} /**** end of function red_dim ****/

/*****************************************************************************/
/*                                red_mdim                                   */
/* ------------------------------------------------------------------------- */
/* function  :  mdim :   {1,2} x mat  -->  N                                 */
/*                       {1,2} x bmat -->  N                                 */
/*                       {1,2} x smat -->  N                                 */
/*                       {1,2} x mmat -->  N                                 */
/*                                                                           */
/*              Die Funktion mdim ist nur auf  Matrizen definiert. Sie       */
/*              liefert die Zahl der Zeilen (bzw. Spalten), falls 'coor'     */
/*              gleich Eins (bzw. Zwei) ist.                                 */
/*                                                                           */
/* Anwendung :  mdim(coor,mat)                                               */
/* parameter :  arg1 - coor                                                  */
/*              arg2 - matrix                                                */
/* returns   :  stackelemnt  (pointer to result descriptor)                  */
/* called by :  rear.c                                                       */
/*****************************************************************************/
int nr_red_mdim(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int coordinate;                        /* coordinate (arg1)  of the matrix */

  DBUG_ENTER ("nr_red_mdim");

  START_MODUL("nr_red_mdim");

  if (T_POINTER(arg2) && R_DESC(DESC(arg2),class) == C_MATRIX) {
    STRUCT_ARG(arg1,coordinate);
    if (_formated) {                                  /* formated == TRUE  */
      if (coordinate == 1) {
        _desc = (PTR_DESCRIPTOR)TAG_INT(R_MVT(DESC(arg2),nrows,C_MATRIX));
      }
      else if (coordinate == 2) {
        _desc = (PTR_DESCRIPTOR)TAG_INT(R_MVT(DESC(arg2),ncols,C_MATRIX));
      }
      else goto fail;
    }
    else {                                            /* formated == FALSE */
      if (coordinate == 1)
        { if ((_desc = conv_int_digit(R_MVT(DESC(arg2),nrows,C_MATRIX))) == NULL)
          longjmp(_interruptbuf,0);} 
      else
       if (coordinate == 2)
        { if ((_desc = conv_int_digit(R_MVT(DESC(arg2),ncols,C_MATRIX))) == NULL)
           longjmp(_interruptbuf,0);} 
       else goto fail;
    }
    /* success: */
    END_MODUL("nr_red_mdim");
    DBUG_RETURN(1);
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("nr_red_mdim");
  DBUG_RETURN(0);
} /**** end of function red_mdim ****/


/*****************************************************************************/
/*                                red_vdim                                   */
/* ------------------------------------------------------------------------- */
/* function  :  vdim  :   vect  -->  N                                       */
/*                        tvect -->  N                                       */
/*                                                                           */
/*              Die Funktion vdim ist nur auf Vektoren bzw. TVektoren        */
/*              definiert. Sie liefert die Zahl der Dimension des Vektors    */
/*              bzw. TVektors                                                */
/*                                                                           */
/* Anwendung :  vdim(vect)  v  vdim(tvect)                                   */
/* parameter :  arg1 - vector  /  tvektor                                    */
/* returns   :  stackelement  (pointer to result descriptor)                 */
/* called by :  rear.c                                                       */
/*****************************************************************************/
int nr_red_vdim(arg1)
register STACKELEM arg1;
{

  DBUG_ENTER ("nr_red_vdim");

  START_MODUL("nr_red_vdim");

  if (T_POINTER(arg1)) {
    switch(R_DESC(DESC(arg1),class)) {
      case C_VECTOR :
            if (_formated) {                            /* formated == TRUE  */
              _desc = (PTR_DESCRIPTOR)TAG_INT(R_MVT(DESC(arg1),ncols,C_VECTOR));
            }
            else {                                      /* formated == FALSE */
              if ((_desc = conv_int_digit(R_MVT(DESC(arg1),ncols,C_VECTOR)))
                                                                      == NULL)
                longjmp(_interruptbuf,0);
            }
            break;
      case C_TVECTOR:
            if (_formated) {                            /* formated == TRUE  */
             _desc = (PTR_DESCRIPTOR)TAG_INT(R_MVT(DESC(arg1),nrows,C_TVECTOR));
            }
            else {                                      /* formated == FALSE */
              if ((_desc = conv_int_digit(R_MVT(DESC(arg1),nrows,C_TVECTOR)))
                                                                      == NULL)
                longjmp(_interruptbuf,0);
            }
            break;
      default:
            goto fail;      /* if class is not C_VECTOR or C_TVECTOR */
    } /** switch **/
    /* success: */
    END_MODUL("nr_red_vdim");
    DBUG_RETURN(1);
  }  /**  end of if T_POINTER **/
fail:
  END_MODUL("nr_red_vdim");
  DBUG_RETURN(0);
} /**** end of function red_vdim ****/


/*****************************************************************************/
/*                                red_ldim                                   */
/*---------------------------------------------------------------------------*/
/* Anwendung:  ldim( struct )                                                */
/* Funktion:   Die Funktion ldim ist auf Listen und Strings  definiert.      */
/*             Sie liefert als Ergebnis die Laenge der Liste bzw. Strings    */
/*****************************************************************************/
int nr_red_ldim(arg1)                     /* ach 30/10/92: int */
register STACKELEM arg1;
{

  DBUG_ENTER ("nr_red_ldim");

  START_MODUL("nr_red_ldim");

  if (T_POINTER(arg1) && R_DESC(DESC(arg1),class) == C_LIST) {
    if (_formated) {
      _desc = (PTR_DESCRIPTOR)TAG_INT(R_LIST(DESC(arg1),dim));
    }
    else
      if ((_desc = conv_int_digit(R_LIST(DESC(arg1),dim))) == NULL) goto fail;
    /* success: */
    END_MODUL("nr_red_ldim");
    DBUG_RETURN(1);
  }
fail:
  END_MODUL("nr_red_ldim");
  DBUG_RETURN(0);
} /**** end of function red_ldim ****/

/*****************************************************************************/
/*                                red_empty                                  */
/* ------------------------------------------------------------------------- */
/* function  :  empty :  struct   -->  Bool                                  */
/*                                                                           */
/*    Die Funktion empty ist auf Listen , Strings , Matrizen ,               */
/*    Vektoren bzw. transponierten Vektoren anwendbar. Sie liefert true,     */
/*    falls die Struktur 'struct' die leere Liste , der leere String , die   */
/*    leere Matrix, der leere Vector bzw. der leere transponierte Vector ist.*/
/*    Ansonsten reduziert sich die Applikation zu false.                     */
/*                                                                           */
/* Anwendung :  empty(struct)                                                */
/* parameter :  arg1 - struct                                                */
/* returns   :  stackelement  (pointer to result descriptor)                 */
/* called by :  rear.c                                                       */
/*****************************************************************************/
int nr_red_empty(arg1)                  /* ach 30/10/92: int */
register STACKELEM arg1;
{
  DBUG_ENTER ("nr_red_empty");

  START_MODUL("nr_red_empty");

  if T_POINTER(arg1) {

    switch(R_DESC(DESC(arg1),class)) {
      case C_LIST     :                   /* if (nil  v  nilstring)          */
                        _desc = (PTR_DESCRIPTOR) ((R_LIST(DESC(arg1),dim) == 0)
                                                         ? SA_TRUE : SA_FALSE);
                        goto success;
      case C_CONS     : _desc = (PTR_DESCRIPTOR)SA_FALSE;
                        goto success;
      case C_MATRIX   :                   /* if (nilmat v nilvect v niltvect) */
      case C_VECTOR   :                   /* then return true                 */
      case C_TVECTOR  :                   /* else return false                */
                        _desc = (PTR_DESCRIPTOR) (R_DESC(DESC(arg1),type)
                                             == TY_UNDEF ? SA_TRUE : SA_FALSE);
                        goto success;
      default         : goto fail;
    } /** end switch class **/
  } /** end of T_POINTER **/

fail    :
  END_MODUL("nr_red_empty");
  DBUG_RETURN(0);
success :
  END_MODUL("nr_red_empty");
  DBUG_RETURN(1);
} /****  end of function red_empty ****/


/*****************************************************************************/
/*                                red_class                                  */
/* ------------------------------------------------------------------------- */
/* function  :  class :  object   -->  Class                                 */
/*                                                                           */
/*    Vielen Ausdruecken der Sprache kann eine Klasse zugeordnet werden:     */
/*                                                                           */
/*    Die Klasse             umfasst                                         */
/*    ---------------------------------------------------------------        */
/*    Function     alle primitiven und benutzerdefinierten Funktionen.       */
/*    Scalar       alle Zahlen und Wahrheitswerte                            */
/*    Matrix       alle Matrizen,  einschliesslich der leeren Matrix.        */
/*    Vector       alle Vektoren,  einschliesslich des leeren Vektors.       */
/*    TVector      alle TVektoren, einschliesslich des leeren TVektors.      */
/*    List         alle Listen,    einschliesslich der leeren Liste.         */
/*    String       alle Strings,   einschliesslich des leeren Strings.       */
/*                                                                           */
/*    Laesst sich der Ausdruck nicht einer dieser Klassen zuordnen, so ist   */
/*    die Funktion nicht anwendbar.                                          */
/*                                                                           */
/* Anwendung :  class( expr )                                                */
/* parameter :  arg1 - expr                                                  */
/* returns   :  stackelement   (pointer to result descriptor)                */
/* called by :  rear.c                                                       */
/*****************************************************************************/
int nr_red_class(arg1)                      /* ach 30/10/92: int */
register STACKELEM arg1;
{
  DBUG_ENTER ("nr_red_class");

  START_MODUL("nr_red_class");

  if T_POINTER(arg1) {
    switch(R_DESC(DESC(arg1),class)) {
      case C_SCALAR  :
      case C_DIGIT   : _desc = (PTR_DESCRIPTOR) CL_SCALAR ;
                       break;
      case C_LIST    : if (R_DESC(DESC(arg1),type) == TY_STRING) {
                         _desc = (PTR_DESCRIPTOR) CHAR;
                         break;
                       }
                       else {
                         _desc = (PTR_DESCRIPTOR) CL_TREE;
                         break;
                       }
      case C_MATRIX  : _desc = (PTR_DESCRIPTOR) CL_MATRIX;
                       break;
      case C_VECTOR  : _desc = (PTR_DESCRIPTOR) CL_VECTOR;
                       break;
      case C_TVECTOR : _desc = (PTR_DESCRIPTOR) CL_TVECTOR;
                       break;
      case C_EXPRESSION:
                     switch(R_DESC(DESC(arg1),type)) {
                       case TY_REC:
                       case TY_SUB:
#if WITHTILDE
		       case TY_SNSUB:
#endif /* WITHTILDE */
                            _desc = (PTR_DESCRIPTOR) CL_FUNCTION;
                            break;
                       default:
                            goto fail;
                     }
                     break;
      default:       goto fail;
    }
    goto success;
  }

  if (T_INT(arg1)) {                           /* dg */
    _desc = (PTR_DESCRIPTOR) CL_SCALAR;        /* dg */
    goto success;                              /* dg */
  }                                            /* dg */
  if (T_FUNC(arg1)) {
    _desc = (PTR_DESCRIPTOR) CL_FUNCTION;
    goto success;
  }
  if T_BOOLEAN(arg1) {
    _desc = (PTR_DESCRIPTOR) CL_SCALAR;
    goto success;
  }
  /* Ansonsten */
  goto fail;
fail:
  END_MODUL("nr_red_class");
  DBUG_RETURN(0);

success:
  END_MODUL("nr_red_class");
  DBUG_RETURN(1);
} /**** end of function red_class ****/


/*****************************************************************************/
/*                                red_type                                   */
/* ------------------------------------------------------------------------- */
/* function  :  type  :  object   -->  Type                                  */
/*                                                                           */
/*    Vielen Ausdruecken der Reduktionssprache kann ein Typ zugeordnet       */
/*    werden:                                                                */
/*                                                                           */
/*    Der Typ               umfasst                                          */
/*    ---------------------------------------------------------------        */
/*    Primfunc     alle primitiven Funktionen.                               */
/*    Userfunc     alle benutzerdefinierten Funktionen.                      */
/*    Decimal      alle Zahlen und Felder von Zahlen.                        */
/*    Bool         alle Wahrheitswerte und Felder von Wahrheitswerten.       */
/*    String       alle Texte und Felder von Texten.                         */
/*                                                                           */
/*    Kann das Argument nicht einem dieser Typen zugeordnet werden, so ist   */
/*    die Funktionsanwendung nicht reduzierbar.                              */
/*                                                                           */
/* Anwendung :  type(object)                                                 */
/* parameter :  arg1 - object                                                */
/* returns   :  stackelement   (pointer to result descriptor)                */
/* called by :  rear.c                                                       */
/*****************************************************************************/
int nr_red_type(arg1)                            /* ach 30/10/92: int */
register STACKELEM arg1;
{
  DBUG_ENTER ("nr_red_type");

  START_MODUL("nr_red_type");

  if T_POINTER(arg1) {
    switch(R_DESC(DESC(arg1),type)) {
      case TY_INTEGER:
      case TY_REAL:
      case TY_DIGIT:   _desc = (PTR_DESCRIPTOR) DECIMAL;
                       break;
      case TY_BOOL:    _desc = (PTR_DESCRIPTOR) BOOL;
                       break;
      case TY_STRING:  _desc = (PTR_DESCRIPTOR) CHAR;
                       break;
      case TY_REC:
#if WITHTILDE
      case TY_SNSUB:
#endif /* WITHTILDE */
      case TY_SUB:     _desc = (PTR_DESCRIPTOR) USERFUNC;
                       break;
      default:         goto fail;
    }
    goto success;
  }

  if T_INT(arg1) {                         /* dg */
    _desc = (PTR_DESCRIPTOR) DECIMAL;      /* dg */
    goto success;                          /* dg */
  }                                        /* dg */
  if T_BOOLEAN(arg1) {
    _desc = (PTR_DESCRIPTOR) BOOL;
    goto success;
  }
  if (T_FUNC(arg1)) {
    _desc = (PTR_DESCRIPTOR) PRIMFUNC;
    goto success;
  }
fail:
  END_MODUL("nr_red_type");
  DBUG_RETURN(0);

success:
  END_MODUL("nr_red_type");
  DBUG_RETURN(1);
}  /**** end of function red_type ****/



/*****************************  end of file rquery.c *************************/
