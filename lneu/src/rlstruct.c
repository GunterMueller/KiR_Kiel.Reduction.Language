/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*****************************************************************************/
/*                         MODUL FOR PROCESSING PHASE                        */
/* ------------------------------------------------------------------------- */
/* rlstruct.c                                                                */
/*                                                                           */
/* special functions :                                                       */
/* -------------------                                                       */
/*    Functions which changing the structure of lists and strings            */
/*                                                                           */
/*    LIST and STRING functions :                                            */
/*                -- external:  red_lselect ,     red_substr , red_lrotate , */
/*                -- internal:  gen_id,  [called by functions which creating */
/*                              indirect list-descripotrs (pattern-match)]   */
/*                -- external:  red_lcut,        red_lunite  , red_lreverse  */
/*                              red_ltransform,  red_repstr  , red_lreplace  */
/*                                                                           */
/* called by : rear.c  and by universal functions in rmstruct.c              */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>

/* Include - File for  List- / Matrix- / Vector- / TVector-                  */
/* structuring functions : he contain extern variable , extern function      */
/* declarations and defines for the structuring functions.                   */
#include "rlmvt.h"

PTR_DESCRIPTOR  gen_id();                       /* forward declaration      */

/*****************************************************************************/
/*                              red_lselect                                  */
/*---------------------------------------------------------------------------*/
/* function  : select one element of a list or a string.                     */
/*                                                                           */
/*             lselect : N  x  list       ==>  list                          */
/*                       N  x  substring  ==>  string                        */
/*                                                                           */
/* parameter : arg1  -  position of element to select                        */
/*             arg2  -  list   with TY_UNDEF                                 */
/*                      list   with TY_STRING                                */
/*****************************************************************************/
int red_lselect(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int  index;

  START_MODUL("red_lselect");

  if (!T_POINTER(arg1) || !T_POINTER(arg2)) goto fail;

  if ( R_DESC(DESC(arg2),class) != C_LIST) goto fail;

  STRUCT_ARG(arg1,index);
  if (index <= 0) goto fail;

  if (index > R_LIST(DESC(arg2),dim))
    goto fail;

  index--;

  _desc = (PTR_DESCRIPTOR) R_LIST(DESC(arg2),ptdv)[index];

  if ((R_DESC(DESC(arg2),type) == TY_STRING) && (!T_POINTER((STACKELEM)_desc)))
  {
    _desc = gen_id(++index,index,(PTR_DESCRIPTOR)arg2);/* select a character */
    TEST_DESC;
  }
  else {
    if T_POINTER((STACKELEM)_desc)
      INC_REFCNT(_desc);
  }

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_lselect");
  return(2);

fail:
  END_MODUL("red_lselect");
  return(0);
} /**** end of function red_lselect ****/


/*****************************************************************************/
/*                              red_substr                                   */
/*---------------------------------------------------------------------------*/
/* function  : get a substructure of a list or a string.                     */
/*                                                                           */
/*             substr : N  x  N  x  list    ==>  list                        */
/*                      N  x  N  x  string  ==>  string                      */
/*                                                                           */
/* parameter : arg1  -  first position of substructure                       */
/*             arg2  -  length of substructure                               */
/*             arg3  -  list   with TY_UNDEF                                 */
/*                      list   with TY_STRING                                */
/*****************************************************************************/
int red_substr(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int first_index,                        /* first position of substructure  */
      last_index,                         /* length of substructure          */
      dim      ;                          /* dimension of the list           */

  START_MODUL("red_substr");

  if  ((!T_POINTER(arg1)) || (!T_POINTER(arg2)) || (!T_POINTER(arg3)))
    goto fail;

  if (R_DESC(DESC(arg3),class) != C_LIST)
    goto fail;

  STRUCT_ARG(arg1,first_index);
  STRUCT_ARG(arg2,last_index);

  if ((first_index <= 0) || (last_index < 0))  /* non positive arguments     */
    goto fail;

  if (first_index > (dim = R_LIST(DESC(arg3),dim)))
    goto fail;

  if (last_index == 0) {                       /* if 2. argument is null     */
    NILSTRING_NIL(arg3);                       /* return nil or nilstring    */
  }

  last_index += (first_index - 1);             /* last position of substruct */

  if (last_index > dim)                /* dim(structure) - position > length */
    goto fail;

                                          /* generate an indirect descriptor */
  _desc = gen_id(first_index,last_index,(PTR_DESCRIPTOR)arg3);

  TEST_DESC;
success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_substr");
  return(1);
fail:
  END_MODUL("red_substr");
  return(0);
} /**** end of function red_substr ****/

/*****************************************************************************/
/*                              red_lrotate                                  */
/*---------------------------------------------------------------------------*/
/* function  :                                                               */
/*  Die Funktion lrotate erzeugt eine zyklisch rotierte  Kopie  von 'list',  */
/*  bzw. 'string' . Der Parameter 'index' bestimmt, das  wievielte  Element  */
/*  nach dem ersten das neue erste Element werden soll. Bei einem negativen  */
/*  Parameter wird die Kopie entgegengesetzt rotiert.                        */
/*                                                                           */
/*             lrotate : index  x  list    ==>  list                         */
/*                       index  x  string  ==>  string                       */
/*                                                                           */
/* parameter : arg1  -  index                                                */
/*             arg2  -  list   with TY_UNDEF                                 */
/*                      list   with TY_STRING                                */
/*****************************************************************************/
int red_lrotate(arg1,arg2)
STACKELEM arg1,arg2;
{
  register int dim,count;
  int pos, rotation;
  int *ipnew,*ip,element;

  START_MODUL("red_lrotate");

  if (!T_POINTER(arg1)) goto fail;

  STRUCT_ARG(arg1,rotation);

  if (!T_POINTER(arg2) || R_DESC(DESC(arg2),class) != C_LIST) goto fail;

  rotation = - rotation;  /* wurde nachtraeglich noetig */

  dim = R_LIST(DESC(arg2),dim);

  if (dim == 0) {                                     /* nil  oder nilstring */
    NILSTRING_NIL(arg2);
  }

  if ((rotation = rotation % dim) == 0) goto nothingtodo;

  NEWDESC(_desc); TEST_DESC;
  LI_DESC_MASK(_desc,1,C_LIST,R_DESC(DESC(arg2),type));
  L_LIST((*_desc),dim) = dim;
  if (R_LIST(DESC(arg2),ptdd) != NULL)                                     /* Edit-Feld */
    L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg2),ptdd)),special); /* Edit-Feld */
  else L_LIST((*_desc),special) = R_LIST(DESC(arg2),special);              /* Edit-Feld */
  NEWHEAP(dim,A_LIST((*_desc),ptdv));
  RES_HEAP;
  ipnew = (int *) R_LIST((*_desc),ptdv);
  ip    = (int *) R_LIST(DESC(arg2),ptdv);

  if (rotation > 0)
     pos = rotation;
  else
     pos = dim + rotation;

  for (count = 0; count < dim; count++)
  {
    ipnew[pos] = element = ip[count];
    if T_POINTER(element)
      INC_REFCNT((PTR_DESCRIPTOR)element);
     ++pos;
     if (pos == dim) pos = 0;
  }
  REL_HEAP;

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_lrotate");
  return(1);
nothingtodo:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  _desc = (PTR_DESCRIPTOR) arg2;
  END_MODUL("red_lrotate");
  return(1);
fail:
  END_MODUL("red_lrotate");
  return(0);
} /**** end of function red_lrotate ****/

/******************************************************************************/
/*                                gen_id                                      */
/*----------------------------------------------------------------------------*/
/* function  : generates new indirect list descriptor                         */
/*             returns a PTR_DESCRIPTOR of new indirect descriptor            */
/*             returns with NULL in case of trouble                           */
/*                                                                            */
/*             Note: SPECIAL-component contains OFFSET.                       */
/*                                                                            */
/* parameter : first_index  - 1.element of new list                           */
/*             last-index   - last element of new list                        */
/*             dir_desc     - pointer of direct descriptor                    */
/******************************************************************************/
PTR_DESCRIPTOR  gen_id(first_index,last_index,dir_desc)
 COUNT    first_index,                 /* 1. elem. of new list   */
          last_index;                  /* last elem. of new list */
 PTR_DESCRIPTOR dir_desc;              /* direct descriptor      */
{
   PTR_DESCRIPTOR ind_desc;            /* indirect descriptor    */

   START_MODUL("gen_id");

   NEWDESC(ind_desc);

   if (ind_desc == NULL) {             /* case of trouble */
/*     post_mortem("gen_id: heap out of space ");*/
     END_MODUL("gen_id");
     return(NULL);
   }

   INC_IDCNT;                          /* increment indirect-descriptor-counter */

   LI_DESC_MASK(ind_desc,1,C_LIST,R_DESC(DESC(dir_desc),type));
   L_LIST((*ind_desc),dim) = last_index - first_index + 1;

   if ((R_LIST((*dir_desc),ptdd)) == (T_PTD)0)
   {
       L_LIST((*ind_desc),special) = first_index - 1;
       L_LIST((*ind_desc),ptdd) = (T_PTD)dir_desc;
       L_LIST((*ind_desc),ptdv) = R_LIST((*dir_desc),ptdv) +
                                  R_LIST((*ind_desc),special);
   }
   else
       /* dir_desc in fact indirect */
   {
       L_LIST((*ind_desc),special) = R_LIST((*dir_desc),special) +
                                     first_index - 1;
       L_LIST((*ind_desc),ptdd) = (T_PTD)R_LIST((*dir_desc),ptdd);
       L_LIST((*ind_desc),ptdv) = R_LIST((*dir_desc),ptdv) +
                                  first_index - 1;
   }
   INC_REFCNT((PTR_DESCRIPTOR)(R_LIST((*ind_desc),ptdd)));
                            /* incr. refcount of descriptor, pointed at */
   END_MODUL("gen_id");
   return(ind_desc);
}  /**** end of function gen_id  ****/


/******************************************************************************/
/*                                red_lcut                                    */
/*----------------------------------------------------------------------------*/
/*  Funktion :                                                                */
/*    Die Funktion lcut entfernt Elemente aus einer Liste bzw. Strings        */
/*    (siehe Beispiele). Das Vorzeichen des Parameters (arg1)                 */
/*    entscheidet darueber, ob ein 'cutfirst' (positives Vorzeichen)          */
/*    oder ein 'cutlast' durchgefuehrt wird.                                  */
/*    Der Absolutwert dieses Parameters gibt an, wieviele Elemente der        */
/*    Liste abgeschnitten werden sollen.                                      */
/*    Es duerfen nicht mehr Elemente abgeschnitten werden als vorhanden       */
/*    sind. Werden alle Elemente abgeschitten, so entsteht eine leere         */
/*    Liste.                                                                  */
/*    Die Funktion gen_id generiert einen neuen indirekten Listen-            */
/*    Deskriptor. Der PTDV des indirekten Deskriptors zeigt in den Daten-     */
/*    vektor des Heapspeicherplatzes des zugehoerigen direkten Deskriptor.    */
/*    Der PTDD des direkten Listen-Deskriptors wird auf 0 gesetzt.            */
/*    Der PTDD des indirekten Deskriptors zeigt auf den direkten              */
/*    Deskriptor. Der indirekte Deskriptor hat keine Rueckwaerts-             */
/*    refferenz von dem Heapspeicherplatz zum indirekten Deskriptor.          */
/*    Ein 'cutfirst' entfernt die ersten, ein 'cutlast' entfernt              */
/*    die letzten Elemente der Liste.                                         */
/*                                                                            */
/*               analoges gilt fuer strings                                   */
/*                                                                            */
/*               cutfirst  <==>  lcut( 2, <1,2,3> )  ==> <3>                  */
/*                               lcut(  4 , 'ich bin ich` )  =>  'bin ich`    */
/*               cutlast   <==>  lcut(~1, <1,2,3> )  ==> <1,2>                */
/*                               lcut(~3, <1,2,3> )  ==> nil                  */
/*                               lcut( 0, <1,2,3> )  ==> <1,2,3>              */
/*                               lcut( ~4 , 'ich bin ich` )  =>  'ich bin`    */
/*                                                                            */
/*  Parameter :  arg1 - (Strukturieungsparameter)                             */
/*               arg2 - (Liste)                                               */
/******************************************************************************/
int red_lcut(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int   cut; /* Strukturieungsparameter wird in diese Variable eingelesen */
  COUNT dim; /* die Zahl der Elemente der indirekten Liste wird in diese Variable eingelesen */
  COUNT first_index,   /* first element of new list */
        last_index;    /* last element of new list */

  START_MODUL("red_lcut");

  if ((!T_POINTER(arg1)) || (!T_POINTER(arg2)))
    goto fail;

  STRUCT_ARG(arg1,cut);


  if (R_DESC(DESC(arg2),class) != C_LIST) goto fail;

  dim = R_LIST(DESC(arg2),dim);

  if (cut >= 0) {         /* cutfirst */
    first_index = cut + 1;
    last_index  = dim;
  }
  else {                  /* cutlast  */
    if ((cut * (-1)) > dim)
      goto fail;
    first_index = 1;
    last_index  = dim + cut;
  }

  if (first_index > (last_index + 1)) goto fail;  /* arg1 out of list */

  if (dim == 0) {
    if (cut != 0) goto fail;
    NILSTRING_NIL(arg2);
  }

  if (first_index == (last_index + 1)) {            /*  loesche ganze Liste  */
    NILSTRING_NIL(arg2);
  }
  _desc = gen_id(first_index,last_index,(PTR_DESCRIPTOR)arg2);

  TEST_DESC;

success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_lcut");
    return(1);
fail:
  END_MODUL("red_lcut");
  return(0);
}  /**** end of function red_lcut ****/


/******************************************************************************/
/*                               red_lunite                                   */
/*----------------------------------------------------------------------------*/
/* Funktion:                                                                  */
/*    lunite kann nur Listen oder nur Strings vereinigen.                     */
/*                                                                            */
/* Beispiele:                                                                 */
/*    lunite( <> , <>  )  =>  <>       lunite( '`    , '`    )  => '`         */
/*    lunite( <> , <5> )  =>  <5>      lunite( '`    , 'B`   )  => 'B`        */
/*    lunite( <5>, <>  )  =>  <5>      lunite( 'str` , 'ing` ) => 'string`    */
/*    lunite( <7>, <9> )  =>  <7,9>                                           */
/*                                                                            */
/******************************************************************************/
int red_lunite(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int    i, element,
       *ip, *ipnew,
       dim,               /* Dimension von arg1 */
      dim2;               /* Dimension von arg2 */

  START_MODUL("red_lunite");

  if ((T_POINTER(arg1) && (R_DESC(DESC(arg1),class) == C_LIST)) &&
      (T_POINTER(arg2) && (R_DESC(DESC(arg2),class) == C_LIST)))
  {
    if (R_DESC(DESC(arg1),type) != R_DESC(DESC(arg2),type))
      goto fail;                   /* Typ von arg1 ist ungleich Typ von arg2 */

    dim  = R_LIST(DESC(arg1),dim);
    dim2 = R_LIST(DESC(arg2),dim);

    if ((dim == 0) && (dim2 == 0)) {  /* arg1 und arg2               */
      _desc = (PTR_DESCRIPTOR)arg2;   /* sind nil's bzw. nilstring's */
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      goto success;
    }

    dim += dim2;                           /* Dimension of new descriptor    */

    NEWDESC(_desc); TEST_DESC;
    LI_DESC_MASK(_desc,1,C_LIST,R_DESC(DESC(arg2),type));
    L_LIST((*_desc),dim) = dim;
    if (R_LIST(DESC(arg1),ptdd) != NULL)                                     /* Edit-Feld */
      L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg1),ptdd)),special); /* Edit-Feld */
    else L_LIST((*_desc),special) = R_LIST(DESC(arg1),special);              /* Edit-Feld */
    NEWHEAP(dim,A_LIST((*_desc),ptdv));
    RES_HEAP;
    ipnew = (int *) R_LIST((*_desc),ptdv);
    ip =  (int *) R_LIST(DESC(arg1),ptdv);
    dim = R_LIST(DESC(arg1),dim);
    for (i = 0; i < dim; ++i)
    {
      ipnew[i] = element = ip[i];
      if T_POINTER(element)
        INC_REFCNT((PTR_DESCRIPTOR)element);
    }

    ipnew += dim;
    ip = (int *) R_LIST(DESC(arg2),ptdv);
    dim = R_LIST(DESC(arg2),dim);
    for (i = 0; i < dim; ++i)
    {
      ipnew[i] = element = ip[i];
      if T_POINTER(element)
        INC_REFCNT((PTR_DESCRIPTOR)element);
    }
    REL_HEAP;
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    goto success;
  }  /* end of  arg1 = list  and  arg2 = list */
  else goto fail;

success:
  END_MODUL("red_lunite");
  return(1);
fail:
  END_MODUL("red_lunite");
  return(0);
}  /*** end of function red_lunite ***/

/*****************************************************************************/
/*                              red_reverse                                  */
/*---------------------------------------------------------------------------*/
/* function  :                                                               */
/*  Die Funktion lrotate erzeugt eine zyklisch rotierte  Kopie  von 'list',  */
/*  bzw. 'string' . Der Parameter 'index' bestimmt, das  wievielte  Element  */
/*  nach dem ersten das neue erste Element werden soll. Bei einem negativen  */
/*  Parameter wird die Kopie entgegengesetzt rotiert.                        */
/*                                                                           */
/*             reverse :  list   ==>  list                                   */
/*                        string ==>  string                                 */
/*                                                                           */
/* parameter : arg1  -  list        /   string                               */
/*                      TY_UNDEF        TY_STRING                            */
/*****************************************************************************/
int red_reverse(arg1)
register STACKELEM arg1;
{
  register int i,j,dim;
  int *ip,*ipnew;

  START_MODUL("red_reverse");

  if (T_POINTER(arg1) && (R_DESC(DESC(arg1),class) == C_LIST))
  {
    if (R_LIST(DESC(arg1),dim) == 0) {
      NILSTRING_NIL(arg1);
    }

    NEWDESC(_desc); TEST_DESC;
    LI_DESC_MASK(_desc,1,C_LIST,R_DESC(DESC(arg1),type));
    L_LIST((*_desc),dim) = dim = R_LIST(DESC(arg1),dim);
    if (R_LIST(DESC(arg1),ptdd) != NULL)                                     /* Edit-Feld */
      L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg1),ptdd)),special); /* Edit-Feld */
    else L_LIST((*_desc),special) = R_LIST(DESC(arg1),special);              /* Edit-Feld */
    NEWHEAP(dim,A_LIST((*_desc),ptdv));
    RES_HEAP;
    ipnew = (int *) R_LIST((*_desc),ptdv);
    ip = (int *) R_LIST(DESC(arg1),ptdv);

    dim--;
    for (i = 0; i <= dim; i++)
    {
      if T_POINTER((j = ip[dim - i]))
        INC_REFCNT((PTR_DESCRIPTOR) j);
      ipnew[i] = j;
    }
    REL_HEAP;
    goto success;
  } /* Ende von T_POINTER */

fail:
  END_MODUL("red_reverse");
  return(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  END_MODUL("red_reverse");
  return(1);
} /**** end of red_reverse ****/

#if LARGE
/*****************************************************************************/
/*                             red_ltransform                                */
/* ------------------------------------------------------------------------- */
/* Funktion:                                                                 */
/*  Mit Hilfe von ltransform kann aus einer Liste eine Matrix erzeugt werden.*/
/*  'rowpar' gibt die Anzahl der Zeilen und 'columnpar' gibt die  Anzahl  der*/
/*  Spalten der zu erzeugenden Matrix an. Die Anzahl der  Elemente der  Liste*/
/*  muss  mit der Angabe von  ('rowpar' * 'columpar')  uebereinstimmen.  Alle*/
/*  Elemente der Liste muessen  vom  gleichen Typ sein. Wenn  ein Vektor oder*/
/*  ein  TVektor  als  Ergebnis  der  Funktion  gewuenscht  wird,  muss  eine*/
/*  (1xm)-mat bzw. eine (nx1)-mat erzeugt werden und diese mit Hilfe der Kon-*/
/*  versions-Funktionen to_vect bzw. to_tvect in einen  Vektor  bzw.  TVektor*/
/*  konvertiert werden.                                                      */
/*                                                                           */
/* Beispiele:  ltransform(0,0,     <>     )  =>  nilmat                      */
/*             ltransform(2,2, <1,2,3,4>  )  =>  mat<1,2>,                   */
/*                                                  <3,4>.                   */
/* Parameter:  arg1 - rowpar,                                                */
/*             arg2 - columnpar,                                             */
/*             arg3 - list                                                   */
/*****************************************************************************/
int red_ltransform(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int nrows,ncols,i,dim,type,class,element;
  int *ip,*ipnew;
  double *rpnew;

  START_MODUL("red_ltransform");

  if (T_POINTER(arg1) && T_POINTER(arg2) && T_POINTER(arg3))
  {
    if ((R_DESC(DESC(arg3),class) != C_LIST) ||
        (R_DESC(DESC(arg3),type)  != TY_UNDEF))
      goto fail;

    STRUCT_ARG(arg1,nrows);
    STRUCT_ARG(arg2,ncols);

    /* Dimensionsueberpruefung : */
    if ((dim = R_LIST(DESC(arg3),dim)) != (nrows * ncols)) goto fail;

    if ((nrows == 0) && (ncols == 0)) {
      if (dim == 0) {
        _desc = _nilmat;
        INC_REFCNT(_nilmat);
        goto success;
      }
      else  goto fail;
    }

    if ((nrows <= 0) || (ncols <= 0))
      goto fail;

    /* Typ- und Klassenueberpruefung: */
    ip = (int *) R_LIST(DESC(arg3),ptdv);

    if T_POINTER((element = ip[0]))
    {
      switch(class = R_DESC(DESC(element),class)) {
         case C_MATRIX : type = TY_MATRIX;
                         break;
         case C_LIST   : type = TY_STRING;
                         if (R_DESC(DESC(element),type) == TY_UNDEF)
                           goto fail;
                         break;
         case C_DIGIT  :
         case C_SCALAR : type =  R_DESC(DESC(element),type);
                         break;
         default       : goto fail;
      }

      for (i = 1; i < dim; i++)
        if T_POINTER((element = ip[i]))
           if (class == R_DESC(DESC(element),class))
           {
             if (type  != R_DESC(DESC(element),type))
               if (class == C_SCALAR)
                 type = TY_REAL;  /* es wird eine Matrix von Reals erstellt */
               else
                 if (class != C_MATRIX) goto fail;
           }
           else
             goto fail;
        else
          goto fail;
    }
    else
    {
      if (!T_BOOLEAN(element)) goto fail;
        type = TY_BOOL;
      for (i = 1; i < dim; i++)
        if (T_POINTER(ip[i]) || !T_BOOLEAN(ip[i]))
          goto fail;
    }

    /* Aufbau einer Matrix */
    switch(type)
    {
      case TY_INTEGER:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_INTEGER);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);

           for (i = 0; i < dim; ++i)
           ipnew[i] = R_SCALAR(DESC(ip[i]),vali);
           REL_HEAP;
           goto success;
      case TY_REAL:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);

           for (i = 0; i < dim; ++i)
             if (R_DESC(DESC((element = ip[i])),type) == TY_REAL)
               rpnew[i] = R_SCALAR(DESC(element),valr);
             else
               rpnew[i] = R_SCALAR(DESC(element),vali);
           REL_HEAP;
           goto success;
      case TY_DIGIT:
      case TY_STRING:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,type);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);

           for (i = 0; i < dim; ++i)
           {
             ipnew[i] = element = ip[i];
             INC_REFCNT((PTR_DESCRIPTOR)element);
           }
           REL_HEAP;
           goto success;
      case TY_BOOL:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_BOOL);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);

           for (i = 0; i < dim; ++i)
             ipnew[i] = (T_SA_TRUE(ip[i]) ? 1 : 0);
           REL_HEAP;
           goto success;
      case TY_MATRIX:
           NEWDESC(_desc); TEST_DESC;
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_MATRIX);
           L_MVT((*_desc),nrows,C_MATRIX) = nrows;
           L_MVT((*_desc),ncols,C_MATRIX) = ncols;
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);

           for (i = 0; i < dim; ++i)
           {
             INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
             ipnew[i] = ip[i];
           }
           REL_HEAP;
           goto success;
      default:
           goto fail;
    }
  } /** if T_POINTER arg1 arg2 arg3 **/


fail:
  END_MODUL("red_ltransform");
  return(0);

success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    DEC_REFCNT((PTR_DESCRIPTOR)arg3);
    END_MODUL("red_ltransform");
    return(1);
}  /**** end of function ltransform ****/
#endif 

/*****************************************************************************/
/*                     red_repstr  (replace-sub-structure)                   */
/*---------------------------------------------------------------------------*/
/* function :  replace a substructure of a string or a list at position      */
/*             'position' and length 'length'                                */
/*     repstr( position  x  length  x  List1    x  List1   )  ==> List       */
/*     repstr( position  x  length  x  String1  x  String2 )  ==> String     */
/*                                                                           */
/* parameter : arg1  -  index / position                                     */
/*             arg2  -  length                                               */
/*             arg3  -  List1       /   String1                              */
/*             arg4  -  List2       /   String2                              */
/*                      TY_UNDEF        TY_STRING                            */
/*****************************************************************************/
int red_repstr(arg1,arg2,arg3,arg4)
STACKELEM arg1,arg2,arg3,arg4;
{
  register int i,j,
               dim3,        /* dimension of arg3                             */
               dim4;        /* dimension of arg4                             */
  int position,             /* first position for replacing in structure     */
      length,               /* length of replaceing in structure             */
      type4,                /* Type of arg4                                  */
      *ip,
      *ipnew,
      *iparg3;

  START_MODUL("red_repstr");

  if T_CON(arg3)                    /* zwar kann repstr beliebige Aus-       */
  {                                 /* druecke in eine Liste einbauen, aber  */
    END_MODUL("red_repstr");        /* falls ear fuer Konstruktorausdruecke  */
    return(0);                      /* keinen expression-Pointer anlegt, so  */
  }                                 /* kann das nur bedeuten, das _redcnt    */
  /* gleich Null ist und keine Beta-Reduktionen gezaehlt werden sollen.      */
  /* lreplace darf diesen Ausdruck aber auch nicht selbst auslagern, da      */
  /* sonst evtl. ein Redex in den Heap gelangt. Deshalb muss hier die Beta-  */
  /* Reduktion als gescheitert erklaert werden.                              */

  if ((!T_POINTER(arg1)) || (!T_POINTER(arg2)) || (!T_POINTER(arg4)))
    goto fail;

  if (R_DESC(DESC(arg4),class) != C_LIST) goto fail;

  STRUCT_ARG(arg1,position);
  if (position <= 0)       /* negative or zero structuring parameter of arg1 */
    goto fail;

  STRUCT_ARG(arg2,length);
  if (length < 0)                  /* negative structuring parameter of arg2 */
    goto fail;

  dim4 = R_LIST(DESC(arg4),dim);   /* dimension if replace one element       */

  if ((length + position) > (dim4 + 1)) /* check of arguments                */
    goto fail;

  type4 = R_DESC(DESC(arg4),type);

  if ((T_POINTER(arg3)) && (R_DESC(DESC(arg3),class) == C_LIST)) {
    if (type4 != R_DESC(DESC(arg3),type))
      goto fail;                 /* not equal types in arguments          */

    dim3 = R_LIST(DESC(arg3),dim); /* dimension of arg3                    */

    dim4 = dim4 - length + dim3;  /* dimension new descriptor              */

    if ((dim3 == 0) && (R_DESC(DESC(arg3),type) == TY_UNDEF)) {
      if (dim4 == 0) {
        INC_REFCNT(_nil);         /* the result of the reduction is a <>   */
        _desc = _nil;
        goto success;
      }
    }

    if ((dim4 == 0) && (type4 == TY_STRING)) {
      INC_REFCNT(_nilstring);    /* the result of the reduction is a '`   */
      _desc = _nilstring;
      goto success;
    }
  }
  else
    goto fail;                     /* arg3 is not a list or only a stackelem*/

  position--;                      /* first position in data-vector         */
                                   /* position in data-vector starts with 0 */
  NEWDESC(_desc); TEST_DESC;
  LI_DESC_MASK(_desc,1,C_LIST,type4);
  L_LIST((*_desc),dim) = dim4;
  if (R_LIST(DESC(arg4),ptdd) != NULL)                          /* Edit-Feld */
    L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg4),ptdd)),special);
  else L_LIST((*_desc),special) = R_LIST(DESC(arg4),special);
  NEWHEAP(dim4,A_LIST((*_desc),ptdv));
  RES_HEAP;
  ip = (int *) R_LIST(DESC(arg4),ptdv);
  ipnew = (int *) R_LIST((*_desc),ptdv);

  for (i = 0; i < position; i++)   /* copy if it is possible until position  */
    if T_POINTER((ipnew[i] = ip[i])) /*                          is reached  */
      INC_REFCNT((PTR_DESCRIPTOR)ip[i]);

  iparg3 = (int *) R_LIST(DESC(arg3),ptdv);
  for (j = 0; j < dim3; j++)     /* copy the argument 3                    */
    if (T_POINTER(ipnew[i++] = iparg3[j]))
      INC_REFCNT((PTR_DESCRIPTOR)iparg3[j]);
  switch(length) {            /* first position of the rest in data-vector */
   case 1  : j = i - j;                /* REPLACE one element              */
             break;
   case 0  : j = i - dim3 - 1 ;        /* INSERT / APPEND                  */
             break;
   default : j = i - dim3 - 1 + length;/* DROP   / REPLACE a structure     */
             break;
  } /** end switch length **/
  for ( ;i < dim4; i++) {        /* copy the rest, if there is one         */
    ++j;
    if T_POINTER((ipnew[i] = ip[j]))
      INC_REFCNT((PTR_DESCRIPTOR)ip[j]);
  }
  REL_HEAP;

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  DEC_REFCNT((PTR_DESCRIPTOR)arg4);
  END_MODUL("red_repstr");
  return(1);

fail:
  END_MODUL("red_repstr");
  return(0);
} /**** end of function red_repstr ****/

/*****************************************************************************/
/*                              red_lreplace                                 */
/*---------------------------------------------------------------------------*/
/* Funktion :                                                                */
/*     lreplace ersetzt Unterstrukturen eines Strings 'string' (einer Liste  */
/*     'list'). Der Parameter 'n' bestimmt, welches Stringelement  (Listen-  */
/*     element) durch das mit 'substitute' spezifizierte  Argument  ersetzt  */
/*     werden soll. Bei Anwendung der Funktion auf Listen kann 'substitute'  */
/*     ein beliebiger Ausdruck der Reduktionssprache  sein.  Bei  Anwendung  */
/*     der Funktion auf Strings muss 'substitute'  ein  String  mit der Di-  */
/*     mension 1 oder ein leerer String sein.                                */
/*                                                                           */
/*          lreplace( position , substitute , list   )   ==>  list           */
/*          lreplace( position , string1    , string )   ==>  string         */
/*                                                                           */
/* parameter : arg1  -  position                                             */
/*             arg2  -  substitute                                           */
/*             arg3     list        /   string                               */
/*                      TY_UNDEF        TY_STRING                            */
/*****************************************************************************/
int red_lreplace(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  register int i,dim;
  int index, *ip, *ipnew, *iparg2,
      type3;                       /* type of arg3 */

  START_MODUL("red_lreplace");

  if T_CON(arg2)                   /* zwar kann lreplace beliebige Aus-     */
  {                                /* druecke in eine Liste einbauen, aber  */
    END_MODUL("red_lreplace");     /* falls ear fuer Konstruktorausdruecke  */
    return(0);                     /* keinen expression-Pointer anlegt, so  */
  }                                /* kann das nur bedeuten, das _redcnt    */
  /* gleich Null ist und keine Beta-Reduktionen gezaehlt werden sollen.     */
  /* lreplace darf diesen Ausdruck aber auch nicht selbst auslagern, da     */
  /* sonst evtl. ein Redex in den Heap gelangt. Deshalb muss hier die Beta- */
  /* Reduktion als gescheitert erklaert werden.                             */

  if (!T_POINTER(arg1) || !T_POINTER(arg3)) goto fail;

  if (R_DESC(DESC(arg3),class) != C_LIST) goto fail;

  type3 = R_DESC(DESC(arg3),type);

  if (type3 == TY_STRING) {
    if T_POINTER(arg2) {
      if (R_DESC(DESC(arg2),type) != TY_STRING)
        goto fail;                 /* wenn arg3 ein string ist, so muss      */
                                   /* arg2 ein string der Dimension 1        */
                                   /* oder ein leerer string sein            */
      if (R_LIST(DESC(arg2),dim) > 1)
        goto fail;
    }
    else
      goto fail;
  }

  STRUCT_ARG(arg1,index);
  if ((index <= 0) || (index > (dim = R_LIST(DESC(arg3),dim)))) goto fail;
  index--;
  NEWDESC(_desc); TEST_DESC;
  LI_DESC_MASK(_desc,1,C_LIST,type3);
  L_LIST((*_desc),dim) = dim;
  if (R_LIST(DESC(arg3),ptdd) != NULL)                                     /* Edit-Feld */
    L_LIST((*_desc),special) = R_LIST((*R_LIST(DESC(arg3),ptdd)),special); /* Edit-Feld */
  else L_LIST((*_desc),special) = R_LIST(DESC(arg3),special);              /* Edit-Feld */
  NEWHEAP(dim,A_LIST((*_desc),ptdv));
  RES_HEAP;
  ip = (int *) R_LIST(DESC(arg3),ptdv);
  ipnew = (int *) R_LIST((*_desc),ptdv);

  for (i = 0; i < index; i++)
    if T_POINTER((ipnew[i] = ip[i]))
      INC_REFCNT((PTR_DESCRIPTOR)ip[i]);

  if (type3 == TY_STRING) {
    if (R_LIST(DESC(arg2),dim) == 0)
      ipnew[i++] = arg2;
    else {
      iparg2 = (int *) R_LIST(DESC(arg2),ptdv);
      if T_POINTER((ipnew[i++] = iparg2[0]))
        INC_REFCNT((PTR_DESCRIPTOR)iparg2[0]);
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    }
  }
  else
   ipnew[i++] = arg2;

  for ( ;i < dim; i++)
    if T_POINTER((ipnew[i] = ip[i]))
      INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
  REL_HEAP;
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_lreplace");
  return(1);

fail:
  END_MODUL("red_lreplace");
  return(0);
} /**** end of function red_lreplace ****/

/***************************** end of file lstruct.c *************************/
