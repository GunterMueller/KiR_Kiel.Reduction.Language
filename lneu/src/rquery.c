/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
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
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rusedeb.h"
#include <setjmp.h>

/*---------------------------------------------------------------------------*/
/*                              defines                                      */
/*---------------------------------------------------------------------------*/
#define DESC(x) (*((PTR_DESCRIPTOR)(x)))

#define TEST_DESC  if (_desc == 0)  longjmp(_interruptbuf,0); else

/* Einlesen eines Strukturierungsparameters in eine Variable */
#if LARGE 
#define STRUCT_ARG(ARG,VAR)         \
switch( R_DESC(DESC(ARG),class) ) { \
  case C_SCALAR:                    \
       if (R_DESC(DESC(ARG),type) == TY_INTEGER) \
         VAR = R_SCALAR(DESC(ARG),vali);         \
       else      \
         if ((VAR = R_SCALAR(DESC(ARG),valr)) != R_SCALAR(DESC(ARG),valr)) \
            goto fail; \
       break;  \
 case C_DIGIT: \
      if (conv_digit_int(ARG,&VAR)==0) goto fail; \
        break; \
 default:       goto fail; }

#else
#define STRUCT_ARG(ARG,VAR)         \
switch( R_DESC(DESC(ARG),class) ) { \
  case C_SCALAR:                    \
       if (R_DESC(DESC(ARG),type) == TY_INTEGER) \
         VAR = R_SCALAR(DESC(ARG),vali);         \
       else      \
         if ((VAR = R_SCALAR(DESC(ARG),valr)) != R_SCALAR(DESC(ARG),valr)) \
            goto fail; \
       break;  \
 default:       goto fail; }
#endif


/*---------------------------------------------------------------------------*/
/*                              extern variables                             */
/*---------------------------------------------------------------------------*/
extern jmp_buf _interruptbuf;          /* <r>, jump-adress for heapoverflow  */
extern PTR_DESCRIPTOR _desc;           /* <w>, resultdescriptor              */
extern BOOLEAN _formated;              /* <r>, for function ldim, dim, vdim  */

/*---------------------------------------------------------------------------*/
/*                              extern functions                             */
/*---------------------------------------------------------------------------*/
#if LARGE
extern BOOLEAN conv_digit_int();                   /* rdig-v1.c              */
extern PTR_DESCRIPTOR conv_int_digit();            /* rdig-v1.c              */
#endif 
extern PTR_DESCRIPTOR newdesc();                   /* rheap.c                */

#if LARGE
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
int red_dim(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int par1;                                 /* structuring parameter of arg1 */

  START_MODUL("red_dim");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {

    STRUCT_ARG(arg1,par1);

    switch(R_DESC(DESC(arg2),class)) {
       case C_LIST   : if (par1 != 0) goto fail;
                       if (red_ldim(arg2) == 0)
                        goto fail;
                       else
                         goto decarg1;
       case C_VECTOR :
       case C_TVECTOR: if (par1 != 0) goto fail;
                       if (red_vdim(arg2) == 0)
                         goto fail;
                       else
                         goto decarg1;
       case C_MATRIX : if (red_mdim(arg1,arg2) == 0)
                         goto fail;
                       else
                         goto success;
       default       : goto fail;
    } /** end switch class arg2 **/
    } /** T_POINTER arg2 **/

fail    :
  END_MODUL("red_dim");
  return(0);
decarg1 :
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
success :
  END_MODUL("red_dim");
  return(1);
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
int red_mdim(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int coordinate;                        /* coordinate (arg1)  of the matrix */

  START_MODUL("red_mdim");

  if (T_POINTER(arg1) && T_POINTER(arg2)) {
    if (R_DESC(DESC(arg2),class) != C_MATRIX) goto fail;

    STRUCT_ARG(arg1,coordinate);

    if (_formated) {                                  /* formated == TRUE  */
      if (coordinate == 1) {
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
        L_SCALAR((*_desc),vali) = R_MVT(DESC(arg2),nrows,C_MATRIX);
      }
      else if (coordinate == 2) {
        NEWDESC(_desc); TEST_DESC;
        DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
        L_SCALAR((*_desc),vali) = R_MVT(DESC(arg2),ncols,C_MATRIX);
      }
      else goto fail;
    }
    else {                                            /* formated == FALSE */
      if (coordinate == 1)
        if ((_desc = conv_int_digit(R_MVT(DESC(arg2),nrows,C_MATRIX))) == NULL)
          longjmp(_interruptbuf,0); else;
      else
       if (coordinate == 2)
         if ((_desc = conv_int_digit(R_MVT(DESC(arg2),ncols,C_MATRIX))) == NULL)
           longjmp(_interruptbuf,0); else;
       else goto fail;
    }
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("red_mdim");
  return(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  END_MODUL("red_mdim");
  return(1);
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
int red_vdim(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_vdim");

  if (T_POINTER(arg1)) {
    switch(R_DESC(DESC(arg1),class)) {
      case C_VECTOR :
             if (_formated) {                            /* formated == TRUE  */
               NEWDESC(_desc); TEST_DESC;
               DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
               L_SCALAR((*_desc),vali) = R_MVT(DESC(arg1),ncols,C_VECTOR);
             }
             else {                                      /* formated == FALSE */
               if ((_desc = conv_int_digit(R_MVT(DESC(arg1),ncols,C_VECTOR)))
                                                                       == NULL)
                 longjmp(_interruptbuf,0);
             }
             goto success;
      case C_TVECTOR:
             if (_formated) {                            /* formated == TRUE  */
               NEWDESC(_desc); TEST_DESC;
               DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
               L_SCALAR((*_desc),vali) = R_MVT(DESC(arg1),nrows,C_TVECTOR);
             }
             else {                                      /* formated == FALSE */
               if ((_desc = conv_int_digit(R_MVT(DESC(arg1),nrows,C_TVECTOR)))
                                                                       == NULL)
                 longjmp(_interruptbuf,0);
             }
             goto success;
      default: goto fail;      /* if not class is not C_VECTOR or C_TVECTOR */
    } /** switch **/

  }  /**  end of if T_POINTER **/
fail:
  END_MODUL("red_vdim");
  return(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  END_MODUL("red_vdim");
  return(1);
} /**** end of function red_vdim ****/
#endif 


/*****************************************************************************/
/*                                red_ldim                                   */
/*---------------------------------------------------------------------------*/
/* Anwendung:  ldim( struct )                                                */
/* Funktion:   Die Funktion ldim ist auf Listen und Strings  definiert.      */
/*             Sie liefert als Ergebnis die Laenge der Liste bzw. Strings    */
/*****************************************************************************/
red_ldim(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_ldim");


  if T_POINTER(arg1)
  {
    if (R_DESC(DESC(arg1),class) != C_LIST) goto fail;

#if LARGE
    if (_formated)
    {
#endif 
      NEWDESC(_desc); TEST_DESC;
      DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER);
      L_SCALAR((*_desc),vali) = R_LIST(DESC(arg1),dim);
#if LARGE
    }
    else
      if ((_desc = conv_int_digit(R_LIST(DESC(arg1),dim))) == NULL) goto fail;
#endif 
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }
fail:
  END_MODUL("red_ldim");
  return(0);

success:
  END_MODUL("red_ldim");
  return(1);
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
red_empty(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_empty");

  if T_POINTER(arg1) {

    switch(R_DESC(DESC(arg1),class)) {
      case C_LIST     :                   /* if (nil  v  nilstring)          */
                        _desc = (PTR_DESCRIPTOR) ((R_LIST(DESC(arg1),dim) == 0)
                                                         ? SA_TRUE : SA_FALSE);
                        goto success;
#if LARGE
      case C_MATRIX   :                   /* if (nilmat v nilvect v niltvect) */
      case C_VECTOR   :                   /* then return true                 */
      case C_TVECTOR  :                   /* else return false                */
                        _desc = (PTR_DESCRIPTOR) (R_DESC(DESC(arg1),type)
                                             == TY_UNDEF ? SA_TRUE : SA_FALSE);
                        goto success;
#endif 
      default         : goto fail;
    } /** end switch class **/
  } /** end of T_POINTER **/

fail    :
  END_MODUL("red_empty");
  return(0);
success :
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  END_MODUL("red_empty");
  return(1);
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
/*                 alle Strings,   einschliesslich des leeren Strings.       */
/*                                                                           */
/*    Laesst sich der Ausdruck nicht einer dieser Klassen zuordnen, so ist   */
/*    die Funktion nicht anwendbar.                                          */
/*                                                                           */
/* Anwendung :  class( expr )                                                */
/* parameter :  arg1 - expr                                                  */
/* returns   :  stackelement   (pointer to result descriptor)                */
/* called by :  rear.c                                                       */
/*****************************************************************************/
red_class(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_class");

  if T_POINTER(arg1) {
    switch(R_DESC(DESC(arg1),class)) {
#if LARGE
      case C_DIGIT   :
#endif 
      case C_SCALAR  : _desc = (PTR_DESCRIPTOR) CL_SCALAR ;
                       break;
      case C_LIST    : if (R_DESC(DESC(arg1),type) == TY_STRING) {
                         _desc = (PTR_DESCRIPTOR) CHAR;
                         break;
                       }
                       else {
                         _desc = (PTR_DESCRIPTOR) CL_TREE;
                         break;
                       }

#if LARGE
      case C_MATRIX  : _desc = (PTR_DESCRIPTOR) CL_MATRIX;
                       break;
      case C_VECTOR  : _desc = (PTR_DESCRIPTOR) CL_VECTOR;
                       break;
      case C_TVECTOR : _desc = (PTR_DESCRIPTOR) CL_TVECTOR;
                       break;
#endif 
      case C_EXPRESSION:
                     switch(R_DESC(DESC(arg1),type)) {
                       case TY_REC:
                       case TY_SUB:
                            _desc = (PTR_DESCRIPTOR) CL_FUNCTION;
                            break;
                       default:
                            goto fail;
                     }
                     break;
      default:       goto fail;
    }
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

  if T_SA(arg1) {
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
  }
fail:
  END_MODUL("red_class");
  return(0);

success:
  END_MODUL("red_class");
  return(1);
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
red_type(arg1)
register STACKELEM arg1;
{
  START_MODUL("red_type");

  if T_POINTER(arg1) {
    switch(R_DESC(DESC(arg1),type)) {
#if LARGE
      case TY_DIGIT:
#endif 
      case TY_INTEGER:
      case TY_REAL:    _desc = (PTR_DESCRIPTOR) DECIMAL;
                       break;
      case TY_BOOL:    _desc = (PTR_DESCRIPTOR) BOOL;
                       break;
      case TY_STRING:  _desc = (PTR_DESCRIPTOR) CHAR;
                       break;
      case TY_REC:
      case TY_SUB:     _desc = (PTR_DESCRIPTOR) USERFUNC;
                       break;
      default:         goto fail;
    }
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    goto success;
  }

  if T_SA(arg1) {
    if T_BOOLEAN(arg1) {
      _desc = (PTR_DESCRIPTOR) BOOL;
      goto success;
    }
    if (T_FUNC(arg1)) {
      _desc = (PTR_DESCRIPTOR) PRIMFUNC;
      goto success;
    }
  }
fail:
  END_MODUL("red_type");
  return(0);

success:
  END_MODUL("red_type");
  return(1);
}  /**** end of function red_type ****/



/*---------------------------------------------------------------------------
 * failure -- kuemmert sich um den hoffentlich selten Fall, das eine Primitive
 *            Funktion nicht anwendbar ist. Die Argumente werden wieder auf den
 *            A-Stack gelegt, die Aplikatoren werden auf den M- und M1-Stacks
 *            rekonstruiert, und die Primitive Funktion wird vom E-Stack auf
 *            den A-Stack gelegt.
 *            Diese Funktion ist deshalb noetig, weil die in dem Aplikatoren
 *            enthaltene Editinformation nicht verloren gehen darf.
 *            Falls alle Argumente Null sind werden nur die Applikatoren
 *            restauriert.
 * globals -- _argsneeded       <r>
 *            S_e,S_a,S_m,S_m1  <w>
 *----------------------------------------------------------------------------
 */
failure(count,arg1,arg2,arg3,arg4)
int count;
STACKELEM arg1,arg2,arg3,arg4;
{
  STACKELEM  ap;
  extern int _argsneeded;  
  int where_to_put =0;

  START_MODUL("failure");

  if (arg1 != NULL)
    if (count == 1)
    {
#if DEBUG
      if (READSTACK(S_a) != arg1)
        post_mortem("failure: last argument is not on the A-Stack.");
#endif
    }
    else if (count == 2)
    {
#if DEBUG
      if (READSTACK(S_a) != arg2)
        post_mortem("failure: last argument is not on the A-Stack.");
#endif
      PUSHSTACK(S_a,arg1);
    }
    else if (count == 3)
    {
#if DEBUG
      if (READSTACK(S_a) != arg3)
        post_mortem("failure: last argument is not on the A-Stack.");
#endif
      PUSHSTACK(S_a,arg2);
      PUSHSTACK(S_a,arg1);
    }
    else if (count == 4)
    {
#if DEBUG
      if (READSTACK(S_a) != arg4)
        post_mortem("failure: last argument is not on the A-Stack.");
#endif
      PUSHSTACK(S_a,arg3);
      PUSHSTACK(S_a,arg2);
      PUSHSTACK(S_a,arg1);
    }
    else
      post_mortem("failure: count out of range");

  while (_argsneeded < 0)  /* ein Teil der Argumente koennen von dem obersten */
  {                        /* Apliktor auf dem M1-Stack abgeknapst worden sein */
    WRITESTACK(S_m1,INC(READSTACK(S_m1)));
    count--;
    _argsneeded++;
  }

  while (count > 0)
  {
    ap = * (S_m1.TopofStack+1);
    if ( !T_AP(ap) || (ARITY(ap) > (count+1)) )
      post_mortem("failure: unexpected situation.");
    count -= ARITY(ap) - 1;  
    PUSHSTACK(S_m1,ap); where_to_put++;
    PUSHSTACK(S_m,AP_1);
  }
  if (stop_on_error)
   do_fail(where_to_put);/* leider wegen dirty trick TOS+1  !! ugah !!*/ 

  PUSHSTACK(S_a,POPSTACK(S_e));
  END_MODUL("failure");
} /**** end of function failure ****/
 

/*---------------------------------------------------------------------------
 * do_fail : special : H.
 *----------------------------------------------------------------------------
 */
do_fail(count)
int count ;
{
STACKELEM m1,m;

if (count)
  { /* store ap's */
   m1 =  POPSTACK(S_m1);
   m  =  POPSTACK(S_m);
   do_fail(--count);
   /*   restore ap's */
   PUSHSTACK(S_m1,m1); 
   PUSHSTACK(S_m,m);
  } 
  else
  {
   ERR_FAIL;
  }
}

/* ---> car inserted do_fail_cond() mit m Test auf @ */

do_fail_cond(count)
int count ;
{
STACKELEM m1,m;

if (count)
  { /* store ap's */
   if (T_AP_1(READSTACK(S_m))) {
     m  =  POPSTACK(S_m);
     m1 =  POPSTACK(S_m1);
     }
   do_fail_cond(--count);
   /*   restore ap's */
   PUSHSTACK(S_m1,m1); 
   PUSHSTACK(S_m,m);
  } 
  else
  {
   ERR_FAIL;
  }
}

/*****************************  end of file rquery.c *************************/
