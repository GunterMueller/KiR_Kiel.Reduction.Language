/*
 * $Log: rvstruct.c,v $
 * Revision 1.4  2001/07/02 14:53:33  base
 * compiler warnings eliminated
 *
 * Revision 1.3  1995/05/22 11:35:31  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.2  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 */

/*****************************************************************************/
/*                         MODUL FOR PROCESSING PHASE                        */
/* ------------------------------------------------------------------------- */
/* rvstruct.c                                                                */
/*                                                                           */
/* special functions :                                                       */
/* -------------------                                                       */
/*    Functions which changing the structure of  vectors or transposed       */
/*    vectors                                                                */
/*                                                                           */
/*    VECTOR / TVECTOR functions:                                            */
/*                -- external:  red_vcut,       red_vselect,    red_vreplace,*/
/*                -- external:  red_vrotate,    red_vunite                   */
/*                                                                           */
/* called by : rear.c  and by universal functions in rmstruct.c              */
/*****************************************************************************/

/* copied and modified from lneu-sources by RS in June 1993 */

/*****************************************************************************/
/* rvstruct.c                                                                */
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

extern int _formated;
extern int DescDump();
extern int test_inc_refcnt();
extern int res_heap();
extern int rel_heap();
extern int test_dec_refcnt();

/*****************************************************************************/
/*                               red_vcut                                    */
/* ------------------------------------------------------------------------- */
/* function :  Diese Funktion schneidet je nach arg1 einen Teil der          */
/*             Elemente des Datenvektors ab. Ein 'cutfirst' (arg1 mit        */
/*             positiven Vorzeichen) schneidet die ersten mit arg1           */
/*             spezifizierten Elemente ab. Ein 'cutlast' (arg1 mit           */
/*             negativen Vorzeichen) schneidet die letzten mit arg1          */
/*             spezifizierten Elemente ab.                                   */
/*                                                                           */
/*             vcut :    Z x vect    -->  vect                               */
/*             vcut :    Z x tvect   -->  tvect                              */
/*                                                                           */
/* parameter:  arg1 - rowpar,                                                */
/*             arg2 - vector / tvector                                       */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_vcut(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int cut;
  register int dim,                        /* Dimension des Ergebnis-Vektors */
               pos;
  int  i,element;
  char class1;                             /* class von arg2                 */

  int *ipnew,*ip,type;
  double *rpnew,*rp;

  DBUG_ENTER ("red_vcut");
  START_MODUL("red_vcut");

  /* changes for lred by RS in June 1993 */
  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2)) {

    switch(class1 = R_DESC(DESC(arg2),class)) {
      case C_VECTOR : dim = R_MVT(DESC(arg2),ncols,class1);
                      break;
      case C_TVECTOR: dim = R_MVT(DESC(arg2),nrows,class1);
                      break;
      default       : goto fail;                    /* nicht vect bzw. tvect */
    }

    /* changes for lred by RS in June 1993 */
    if (T_INT(arg1))
      cut = VAL_INT(arg1);
    else
      STRUCT_ARG(arg1,cut);

    if (cut >= 0) {                                        /* cutfirst       */
      dim -=cut;
      pos  = cut;
    }
    else {                                                 /* cutlast        */
      dim += cut;
      pos  = 0;
    }

    if (dim < 0)
      goto fail;

    if (dim == 0) {                                 /* nilvect oder niltvect */
     switch(class1) {
       case C_VECTOR  : _desc = _nilvect;
                        INC_REFCNT(_nilvect);
                        break;
       case C_TVECTOR : _desc = _niltvect;
                        INC_REFCNT(_niltvect);
                        break;
     }
     goto success;
    }

    NEWDESC(_desc); TEST_DESC;

    if ((type = R_DESC(DESC(arg2),type)) != TY_REAL) {
      MVT_DESC_MASK(_desc,1,class1,type);         /* create a new descriptos */
      NEWHEAP(dim,A_MVT((*_desc),ptdv,class1));
      RES_HEAP;
      ipnew = (int *) R_MVT((*_desc),ptdv,class1);   /*----------------------*/
      ip    = (int *) R_MVT(DESC(arg2),ptdv,class1); /* copy "cutted vect    */
                                                     /*              tvect"  */
                                                     /*----------------------*/
      if (type >= TY_STRING)
        for (i = 0; i < dim; ++i) {
          element = ipnew[i] = ip[pos];
          INC_REFCNT((PTR_DESCRIPTOR)element);
          ++pos;
        }
      else
        for (i = 0; i < dim; ++i) {
          ipnew[i] = ip[pos];
          ++pos;
        }
    }
    else {                                        /* type == TY_REAL         */
      MVT_DESC_MASK(_desc,1,class1,TY_REAL);      /* create a new descriptos */
      NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,class1));
      RES_HEAP;
      rpnew = (double *)R_MVT((*_desc),ptdv,class1);  /*----------------------*/
      rp    = (double *)R_MVT(DESC(arg2),ptdv,class1);/* copy "cutted vect    */
                                                      /*              tvect"  */
                                                      /*----------------------*/
      for (i = 0; i < dim; ++i) {
        rpnew[i] = rp[pos];
        ++pos;
      }
    } /** end else TY_REAL **/
    REL_HEAP;
    if (class1 == C_VECTOR) {
      L_MVT((*_desc),nrows,class1) = 1;           /* Wertebereich ist immer  */
      L_MVT((*_desc),ncols,class1) = dim;         /* ein Vektor              */
    }
    else {
      L_MVT((*_desc),nrows,class1) = dim;         /* Wertebereich ist immer  */
      L_MVT((*_desc),ncols,class1) = 1;           /* ein TVektor             */
    }
    goto success;
  } /** end of if  T_POINTER.. **/

fail:
  END_MODUL("red_vcut");
  DBUG_RETURN(0);

success:
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_vcut");
  DBUG_RETURN(1);
} /**** end of function red_vcut ****/

/*****************************************************************************/
/*                               red_vselect                                 */
/* ------------------------------------------------------------------------- */
/* function :                                                                */
/*          Mit Hilfe dieser Funktion koennen Elemente aus einem Vektor bzw. */
/*          TVektor selektiert werden.  Das  erste  Argument  bestimmt  das  */
/*          Selektionselement in dem Vektor bzw. TVektor.                    */
/*                                                                           */
/*             vselect :   N  x  vect  -->  Scalar / String                  */
/*                         N  x  tvect -->  Scalar / String                  */
/*                                                                           */
/* parameter:  arg1 - rowpar / columnpar                                     */
/*             arg2 - vector / tvector                                       */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_vselect(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int rowcol;                          /* Strukturierungsargument            */
  char   class1;                       /* class des Deskriptors arg2         */

  DBUG_ENTER ("red_vselect");
  START_MODUL("red_vselect");

  /* changes for lred by RS in June 1993 */
  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2)) {

    /* changes for lred by RS in June 1993 */
    if (T_INT(arg1))
      rowcol = VAL_INT(arg1);
    else 
      STRUCT_ARG(arg1,rowcol);
    if (rowcol < 1)  goto fail;

    switch(class1 = R_DESC(DESC(arg2),class)) {
     case C_VECTOR : if (rowcol > (int)R_MVT(DESC(arg2),ncols,C_VECTOR))
                       goto fail;
                     break;
     case C_TVECTOR: if (rowcol > (int)R_MVT(DESC(arg2),nrows,C_TVECTOR))
                       goto fail;
                     break;
     default       : goto fail;
    } /** end switch class1 **/
                                                   /*------------------------*/
    rowcol --;                                     /* "select a SCALAR"      */
    switch (R_DESC(DESC(arg2),type)) {             /*------------------------*/
      case TY_INTEGER:
           /* changes for lred by RS in June 1993 */
           if (_formated) {
             DBUG_PRINT ("red_vselect", ("the value is: %d", (((int *) R_MVT(DESC(arg2),ptdv,class1))[rowcol])));
             _desc = (PTR_DESCRIPTOR) TAG_INT(((int *) R_MVT(DESC(arg2),ptdv,class1))[rowcol]);
             goto success;
             }
           else {
           NEWDESC(_desc); TEST_DESC;              /* interne Typ-Konversion */
           DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER); /* vect  --> Number       */
                                                   /* tvect --> Number       */
           L_SCALAR((*_desc),vali) = ((int *) R_MVT(DESC(arg2),ptdv,class1))
                                    [rowcol];
           goto success; 
           }
      case TY_REAL:
           NEWDESC(_desc); TEST_DESC;              /* interne Typ-Konversion */
           DESC_MASK(_desc,1,C_SCALAR,TY_REAL);    /* vect  --> Number       */
                                                   /* tvect --> Number       */
           L_SCALAR((*_desc),valr) = ((double *) R_MVT(DESC(arg2),ptdv,class1))
                                    [rowcol];
           goto success;
      case TY_BOOL:
           if (R_MVT(DESC(arg2),ptdv,class1)       /* interne Typ-Konversion */
                                    [rowcol])      /* bvect  --> Bool        */
                                                   /* btvect --> Bool        */
              _desc = (PTR_DESCRIPTOR)SA_TRUE;
           else
              _desc = (PTR_DESCRIPTOR)SA_FALSE;
           goto success;
      default:               /* svect  --> String  /  vect  --> "TY_DIGIT"   */
                             /* stvect --> String  /  tvect --> "TY_DIGIT"   */
           _desc = ((PTR_DESCRIPTOR *) R_MVT(DESC(arg2),ptdv,class1)) [rowcol];
           INC_REFCNT(_desc);
           goto success;
    }
  } /** end T_POINTER **/
fail:
  END_MODUL("red_vselect");
  DBUG_RETURN(0);
success:
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_vselect");
  DBUG_RETURN(1);
} /**** end of function red_vselect ****/

/*****************************************************************************/
/*                               red_vreplace                                */
/* ------------------------------------------------------------------------- */
/* function :  replacing substructure specified in arg2                      */
/*             in position arg1                                              */
/*                                                                           */
/*             vreplace  :  N  x  Scalar / String x  vect  -->  vect         */
/*                          N  x  Scalar / String x  tvect -->  tvect        */
/*                                                                           */
/* paramter:   arg1 - rowpar   /  columnpar                                  */
/*             arg2 - Number   /  Bool  /  String                            */
/*             arg3 - vector   /  tvector                                    */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_vreplace(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  register int i,
               dim;                         /* dimension of data vector      */
  int rowcol,                               /* structuring parameter ==      */
                                            /* position in dopevector        */
      type;                                 /* same type of elements         */
                                            /* in arg2  and arg3             */
  int    *ip, *ipnew;
  double *rp, *rpnew;
  char   class3;                            /* class of arg3                 */

  DBUG_ENTER ("red_vreplace");
  START_MODUL("red_vreplace");

  /* changes for lred by RS in June 1993 */
  if ((!T_POINTER(arg1) && !T_INT(arg1)) || !T_POINTER(arg3))
    goto fail;

  /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    rowcol = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,rowcol);

  switch(class3 = R_DESC(DESC(arg3),class)) {
    case C_VECTOR : dim = R_MVT(DESC(arg3),ncols,class3);
                    break;
    case C_TVECTOR: dim = R_MVT(DESC(arg3),nrows,class3);
                    break;
  default       : goto fail;
  }

  if ((rowcol < 1) || (rowcol > dim))
     goto fail;

  type = R_DESC(DESC(arg3),type);

  if (T_POINTER(arg2)) {
    switch(R_DESC(DESC(arg2),class)) {       /* Type-check of  arg2 and arg3 */
      case C_LIST   : if (type != TY_STRING)
                        goto fail;
                      break;
      case C_SCALAR : if ((type != TY_INTEGER) && (type != TY_REAL))
                        goto fail;
                      break;
      case C_DIGIT  : if (type != TY_DIGIT)
                        goto fail;
                        break;
      default       : goto fail;
    }
  } /** end T_POINTER **/

  if (T_BOOLEAN(arg2)) {
    if (type != TY_BOOL)                     /* Type-check of  arg2 and arg3 */
      goto fail;
  }

  rowcol--;                                  /* zu ersetzende Position im    */
                                             /* im Dopevektor                */

  switch(type) {                             /* switch ueber eine Matrix     */
   /*~~~~~~~~~~~~~~~~*/                      /* mit Elementen eines bestim-  */
    case TY_INTEGER:                         /* mten Typs                    */
   /*~~~~~~~~~~~~~~~~*/
         /* changes for lred by RS in June 1993 */
         if (!T_INT(arg2))
           if (!T_POINTER(arg2) || (R_DESC(DESC(arg2),class) != C_SCALAR))
             goto fail;

         NEWDESC(_desc); TEST_DESC;

         if (T_INT(arg2) || (R_DESC(DESC(arg2),type) == TY_INTEGER)) {
           MVT_DESC_MASK(_desc,1,class3,TY_INTEGER);
           NEWHEAP(dim,A_MVT((*_desc),ptdv,class3));
           RES_HEAP;
           ip    = (int *) R_MVT(DESC(arg3),ptdv,class3);
           ipnew = (int *) R_MVT((*_desc),ptdv,class3);
           for (i = 0; i < dim; i++)
             ipnew[i] = ip[i];
           /* changes for lred by RS in June 1993 */
           if (T_INT(arg2))
             ipnew[rowcol] = VAL_INT(arg2);
           else
             ipnew[rowcol] = R_SCALAR(DESC(arg2),vali);
           REL_HEAP;
           goto success;
         }
         MVT_DESC_MASK(_desc,1,class3,TY_REAL);    /* arg2 ist vom Typ  REAL */
         NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,class3));
         RES_HEAP;
         ip    =    (int *) R_MVT(DESC(arg3),ptdv,class3);
         rpnew = (double *) R_MVT((*_desc),ptdv,class3);
         for (i = 0; i < dim; i++)
            rpnew[i] = (double) ip[i];
         rpnew[rowcol] =  R_SCALAR(DESC(arg2),valr);
         REL_HEAP;
         goto success;

   /*~~~~~~~~~~~~~~~~*/
    case TY_REAL:
   /*~~~~~~~~~~~~~~~~*/
         if (!T_POINTER(arg2) || (R_DESC(DESC(arg2),class) != C_SCALAR))
           goto fail;

         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,class3,TY_REAL);
         NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,class3));
         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg3),ptdv,class3);
         rpnew = (double *) R_MVT((*_desc),ptdv,class3);
         for (i = 0; i < dim; i++)
            rpnew[i] = rp[i];
         if (R_DESC(DESC(arg2),type) == TY_INTEGER)/* arg2 ist vom Typ INTEGER*/
           rpnew[rowcol] = (double) R_SCALAR(DESC(arg2),vali);
         else                                      /* arg2 ist vom Typ  REAL  */
           rpnew[rowcol] =  R_SCALAR(DESC(arg2),valr);
         REL_HEAP;
         goto success;

   /*~~~~~~~~~~~~~~~~*/
    case TY_BOOL:
   /*~~~~~~~~~~~~~~~~*/
         if (!T_BOOLEAN(arg2))
           goto fail;
         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,class3,TY_BOOL);
         NEWHEAP(dim,A_MVT((*_desc),ptdv,class3));
         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg3),ptdv,class3);
         ipnew = (int *) R_MVT((*_desc),ptdv,class3);
         for (i = 0; i < dim; i++)
           ipnew[i] = ip[i];
         ipnew[rowcol] = (T_SA_TRUE(arg2) ? TRUE : FALSE);
         REL_HEAP;
         goto success_3;

   /*~~~~~~~~~~~~~~~~*/
    case TY_DIGIT :
    case TY_STRING:
   /*~~~~~~~~~~~~~~~~*/
         if ((!T_POINTER(arg2)) || (type != R_DESC(DESC(arg3),type)))
           goto fail;

         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,class3,type);
         NEWHEAP(dim,A_MVT((*_desc),ptdv,class3));
         RES_HEAP;
         ip =    (int *) R_MVT(DESC(arg3),ptdv,class3);
         ipnew = (int *) R_MVT((*_desc),ptdv,class3);
         for (i = 0; i < dim; i++) {
          if (i == rowcol)
            ipnew[i] = arg2;
          else
            INC_REFCNT((PTR_DESCRIPTOR)(ipnew[i] = ip[i]));
         }
         REL_HEAP;
         goto success_3;
  } /** switch type **/

fail:
  END_MODUL("red_vreplace");
  DBUG_RETURN(0);
success:
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg2))
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
success_3:
  if (class3 == C_VECTOR) {
    L_MVT((*_desc),nrows,class3) = 1;             /* Wertebereich ist immer  */
    L_MVT((*_desc),ncols,class3) = dim;           /* ein Vektor              */
  }
  else {
    L_MVT((*_desc),nrows,class3) = dim;           /* Wertebereich ist immer  */
    L_MVT((*_desc),ncols,class3) = 1;             /* ein TVektor             */
  }
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_vreplace");
  DBUG_RETURN(1);
} /**** end of function red_vreplace ****/

/*****************************************************************************/
/*                               red_vrotate                                 */
/* ------------------------------------------------------------------------- */
/* function :  Die Funktion vrotate erzeugt eine zyklisch rotierte Kopie     */
/*             eines Vektors bzw. eines TVektors.                            */
/*             arg1 bestimmt, das erste Element des neu entstehenden         */
/*             Dopevektors.                                                  */
/*                                                                           */
/*             vrotate   :  Z  x  vect   -->  vect                           */
/*                          Z  x  tvect  -->  tvect                          */
/*                                                                           */
/* paramter:   arg1 - rowpar  /  columnpar                                   */
/*             arg2 - vector  /  tvector                                     */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_vrotate(arg1,arg2)
STACKELEM arg1,arg2;
{
  register int count,
               nrowcol;                /* Anzahl der Zeilen bzw. Spalten     */
                                       /* (Dimension des Dopevektors)        */
  int    rowcol;                       /* Strukturierungsargument            */
  int    *ipnew,*ip,type,element;
  double *rpnew,*rp;
  char   class1;                       /* class des Deskriptors              */

  DBUG_ENTER ("red_vrotate");
  START_MODUL("red_vrotate");

  /* changes for lred by RS in June 1993 */
  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2)) {

    /* changes for lred by RS in June 1993 */
    if (T_INT(arg1))
      rowcol = VAL_INT(arg1);
    else
      STRUCT_ARG(arg1,rowcol);
    rowcol = - rowcol;

    switch(class1 = R_DESC(DESC(arg2),class)) {

      case C_VECTOR : if (arg2 == (int)_nilvect) {
                        _desc = _nilvect;
                        INC_REFCNT(_desc);
                        goto success;
                      }
                      nrowcol = R_MVT(DESC(arg2),ncols,class1);
                      goto vrotate;
      case C_TVECTOR: if (arg2 == (int)_niltvect) {
                        _desc = _niltvect;
                        INC_REFCNT(_desc);
                        goto success;
                      }
                      nrowcol = R_MVT(DESC(arg2),nrows,class1);
                      goto vrotate;
      default       : goto fail;
    } /** end switch class1 **/

  } /* Ende von T_POINTER */
  goto fail;

vrotate:
      if ((rowcol = rowcol % nrowcol) == 0)
        goto nothingtodo;
      else
        if (rowcol  < 1)
          rowcol += nrowcol;

      if ((type = R_DESC(DESC(arg2),type)) != TY_REAL) {
        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,class1,type);
        L_MVT((*_desc),nrows,class1) = R_MVT(DESC(arg2),nrows,class1);
        L_MVT((*_desc),ncols,class1) = R_MVT(DESC(arg2),ncols,class1);
        NEWHEAP(nrowcol,A_MVT((*_desc),ptdv,class1));
        RES_HEAP;
        ipnew = (int *) R_MVT((*_desc),ptdv,class1);
        ip    = (int *) R_MVT(DESC(arg2),ptdv,class1);

        if (type >= TY_STRING)
          for (count = 0; count < nrowcol; count++) {
            ipnew[rowcol] = element = ip[count];
            INC_REFCNT((PTR_DESCRIPTOR)element);
            ++rowcol;
            if (rowcol == nrowcol) rowcol = 0;
          }
        else
          for (count = 0; count < nrowcol; count++) {
            ipnew[rowcol] = ip[count];
            ++rowcol;
            if (rowcol == nrowcol) rowcol = 0;
          }
        REL_HEAP;
        goto success;
      }
      else {
        NEWDESC(_desc); TEST_DESC;
        MVT_DESC_MASK(_desc,1,class1,TY_REAL);
        L_MVT((*_desc),nrows,class1) = R_MVT(DESC(arg2),nrows,class1);
        L_MVT((*_desc),ncols,class1) = R_MVT(DESC(arg2),ncols,class1);
        NEWHEAP(nrowcol<<1,A_MVT((*_desc),ptdv,class1));
        RES_HEAP;
        rpnew = (double *) R_MVT((*_desc),ptdv,class1);
        rp    = (double *) R_MVT(DESC(arg2),ptdv,class1);

        for (count = 0; count < nrowcol; count++) {
          rpnew[rowcol] = rp[count];
          ++rowcol;
          if (rowcol == nrowcol) rowcol = 0;
        }
        REL_HEAP;
        goto success;
      }

nothingtodo:
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  _desc = (PTR_DESCRIPTOR) arg2;
  END_MODUL("red_vrotate");
  DBUG_RETURN(1);

fail:
  END_MODUL("red_vrotate");
  DBUG_RETURN(0);

success:
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_vrotate");
  DBUG_RETURN(1);
} /**** end of function red_vrotate ****/

/*****************************************************************************/
/*                               red_vunite                                  */
/* ------------------------------------------------------------------------- */
/* function :  Diese Funktion vereinigt jeweils nach unten stehendem         */
/*             Definitionsbereich zwei Vektoren bzw. TVektoren.              */
/*             Die Anzahl der Elemente in den Argumenten der Vektoren bzw.   */
/*             TVektoren koennen beliebig sein.                              */
/*                                                                           */
/*             vunite  :  vect  x  vect    -->  vect                         */
/*                        tvect x  tvect   -->  tvect                        */
/*                                                                           */
/* parameter:  arg1 - vector   / tvector                                     */
/*             arg2 - vector   / tvector                                     */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_vunite(arg1,arg2)
register STACKELEM arg1,arg2;
{
  int    i,
         dim,                         /* Dimension des Ergebnis Datenvektors*/
         dim1,dim2,                   /* Dimension des Dopevektors der Arg's*/
         type1,type2,                 /* fuer den Typ von arg1 bzw. arg2    */
         *ip,*ipnew;
  double *rp, *rpnew;
  char   class1,class2;               /* fuer die Klasse von arg1 bzw. arg2 */

  DBUG_ENTER ("red_vunite");
  START_MODUL("red_vunite");

  if (!T_POINTER(arg1) || !T_POINTER(arg2))
    goto fail;

  switch(class1 = R_DESC(DESC(arg1),class)) {
    case C_VECTOR : break;
    case C_TVECTOR: break;
    default       : goto fail;
  }

  switch(class2 = R_DESC(DESC(arg2),class)) {
    case C_VECTOR : break;
    case C_TVECTOR: break;
    default       : goto fail;
  }

  if (class1 != class2)
    goto fail;

  type1 = R_DESC(DESC(arg1),type);
  type2 = R_DESC(DESC(arg2),type);

  if (type1 == TY_UNDEF) {             /* arg1 ist ein nilvect oder niltvect */
    _desc = (PTR_DESCRIPTOR)arg2;
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_vunite");
    DBUG_RETURN(1);
  }
  if (type2 == TY_UNDEF)  {            /* arg2 ist ein nilvect oder niltvect */
    _desc = (PTR_DESCRIPTOR)arg1;
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_vunite");
    DBUG_RETURN(1);
  }

  if (class1 == C_VECTOR)
    dim = ((dim1 = R_MVT(DESC(arg1),ncols,class1)) +
           (dim2 = R_MVT(DESC(arg2),ncols,class1)));
  else
    dim = ((dim1 = R_MVT(DESC(arg1),nrows,class1)) +
           (dim2 = R_MVT(DESC(arg2),nrows,class1)));

  switch(type1) {

    case TY_INTEGER:
         switch(type2) {

           case TY_INTEGER:                       /* Wertebereich ist immer  */
                NEWDESC(_desc); TEST_DESC;        /* ein Vektor bzw. TVektor */
                MVT_DESC_MASK(_desc,1,class1,TY_INTEGER);
                NEWHEAP(dim,A_MVT((*_desc),ptdv,class1));
                RES_HEAP;
                ipnew = (int *) R_MVT((*_desc),ptdv,class1);

                ip    = (int *) R_MVT(DESC(arg1),ptdv,class1);
                for (i = 0; i < dim1; ++i)
                  ipnew[i] = ip[i];

                ipnew += dim1;
                ip  = (int *) R_MVT(DESC(arg2),ptdv,class1);
                for (i = 0; i < dim2; ++i)
                 ipnew[i] = ip[i];
                REL_HEAP;
                break;

           case TY_REAL:                          /* Wertebereich ist immer  */
                NEWDESC(_desc); TEST_DESC;        /* ein Vektor bzw. TVektor */
                MVT_DESC_MASK(_desc,1,class1,TY_REAL);
                NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,class1));
                RES_HEAP;
                rpnew = (double *) R_MVT((*_desc),ptdv,class1);

                ip    = (int *) R_MVT(DESC(arg1),ptdv,class1);
                for (i = 0; i < dim1; ++i)
                  rpnew[i] = (double) ip[i];

                rpnew += dim1;
                rp = (double *) R_MVT(DESC(arg2),ptdv,class1);
                for (i = 0; i < dim2; ++i)
                  rpnew[i] = rp[i];
                REL_HEAP;
                break;
           default: goto fail;
         }
         break;

    case TY_REAL:
         switch (type2) {

           case TY_INTEGER:                       /* Wertebereich ist immer  */
                NEWDESC(_desc); TEST_DESC;        /* ein Vektor bzw. TVektor */
                MVT_DESC_MASK(_desc,1,class1,TY_REAL);
                NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,class1));
                RES_HEAP;
                rpnew = (double *) R_MVT((*_desc),ptdv,class1);

                rp    = (double *) R_MVT(DESC(arg1),ptdv,class1);
                for (i = 0; i < dim1; ++i)
                  rpnew[i] = rp[i];

                rpnew += dim1;
                ip = (int *) R_MVT(DESC(arg2),ptdv,class1);
                for (i = 0; i < dim2; ++i)
                  rpnew[i] = (double) ip[i];
                REL_HEAP;
                break;

           case TY_REAL:                          /* Wertebereich ist immer  */
                NEWDESC(_desc); TEST_DESC;        /* ein Vektor bzw. TVektor */
                MVT_DESC_MASK(_desc,1,class1,TY_REAL);
                NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,class1));
                RES_HEAP;
                rpnew = (double *) R_MVT((*_desc),ptdv,class1);

                rp    = (double *) R_MVT(DESC(arg1),ptdv,class1);
                for (i = 0; i < dim1; ++i)
                  rpnew[i] = rp[i];

                rpnew += dim1;
                rp = (double *) R_MVT(DESC(arg2),ptdv,class1);
                for (i = 0; i < dim2; ++i)
                  rpnew[i] = rp[i];
                REL_HEAP;
                break;
           default: goto fail;
         }
         break;

    default:
         if (type1 != type2)
           goto fail;
         NEWDESC(_desc); TEST_DESC;               /* Wertebereich ist immer  */
         MVT_DESC_MASK(_desc,1,class1,type2);     /* ein Vektor bzw. TVektor */
         NEWHEAP(dim,A_MVT((*_desc),ptdv,class1));
         RES_HEAP;
         ipnew = (int *) R_MVT((*_desc),ptdv,class1);

         ip    = (int *) R_MVT(DESC(arg1),ptdv,class1);
         if (type2 == TY_BOOL)
           for (i = 0; i < dim1; ++i)
             ipnew[i] = ip[i];
         else
           for (i = 0; i < dim1; ++i) {
             ipnew[i] = ip[i];
             INC_REFCNT((PTR_DESCRIPTOR) ip[i]);
           }

         ipnew += dim1;
         ip = (int *) R_MVT(DESC(arg2),ptdv,class1);
         if (type2 == TY_BOOL)
           for (i = 0; i < dim2; ++i)
             ipnew[i] = ip[i];
         else
           for (i = 0; i < dim2; ++i) {
             ipnew[i] = ip[i];
             INC_REFCNT((PTR_DESCRIPTOR) ip[i]);
           }
         REL_HEAP;
  } /** end switch type of arg1 **/

  if (class1 == C_VECTOR) {
    L_MVT((*_desc),nrows,class1) = 1;             /* Wertebereich ist immer  */
    L_MVT((*_desc),ncols,class1) = dim;           /* ein Vektor              */
  }
  else {
    L_MVT((*_desc),nrows,class1) = dim;           /* Wertebereich ist immer  */
    L_MVT((*_desc),ncols,class1) = 1;             /* ein TVektor             */
  }

/* success:   RS June 1993 */
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_vunite");
  DBUG_RETURN(1);

fail:
  END_MODUL("red_vunite");
  DBUG_RETURN(0);

} /**** end of function red_vunite ****/

/***************************** end of file rvstruct.c ************************/
