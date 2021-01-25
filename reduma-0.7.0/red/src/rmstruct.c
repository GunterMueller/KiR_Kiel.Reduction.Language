/* $Log: rmstruct.c,v $
 * Revision 1.4  1995/05/22  09:52:19  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:18  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */

/* copied from lneu-sources by RS in June 1993 */

/*****************************************************************************/
/* rmstruct.c                                                                */
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

/* RS June 1993 */

extern int _formated;
extern DescDump();
extern test_inc_refcnt();
extern res_heap();
extern rel_heap();
extern test_dec_refcnt();
extern red_lcut();
extern red_vcut();
extern red_lselect();
extern red_vselect();
extern red_lreplace();
extern red_vreplace();
extern red_lrotate();
extern red_vrotate();
extern red_lunite();
extern red_vunite();

/*****************************************************************************/
/*                               red_mcut                                    */
/* ------------------------------------------------------------------------- */
/* function :                                                                */
/*  Die Funktion mcut entfernt Elemente aus einer Kopie von 'mat'. Es koen-  */
/*  nen sowohl Zeilen als auch Spalten abgeschnitten werden. Das Vorzeichen  */
/*  eines Parameters entscheidet darueber, ob in der betreffenden Koordinate */
/*  ein 'cutfirst' (positives Vorzeichen) oder ein 'cutlast' (negatives      */
/*  Vorzeichen) durchgefuehrt wird. Ein 'cutfirst' entfernt die ersten,      */
/*  ein 'cutlast' die letzten Elemente entlang einer Koordinate. Der         */
/*  Absolutwert eines Parameters gibt an, wieviele Elemente abgeschnitten    */
/*  werden sollen. Allerdings duerfen nicht mehr Elemente als vorhanden      */
/*  entfernt werden. Werden alle Elemente einer Koordinate abgeschnitten,    */
/*  so entsteht ein leere Matrix  (nilmat).                                  */
/*                                                                           */
/* Implementierungsbemerkungen:                                              */
/*   Eine Sonderbehandlung fuer dem Fall, das weder Zeilen- noch             */
/*   Spaltenschnitt gefordert, ist erfolgt nicht.                            */
/*   Da der Spaltenschnitt schwieriger ist als der Zeilenschnitt, gibt es    */
/*   zwei verschiedene Schleifen fuer das Zusammenstellen der neuen Matrix.  */
/*                                                                           */
/*             mcut :    Z x Z x mat  -->  mat                               */
/*                       0 x Z x mat  -->  mat                               */
/*                       Z x 0 x mat  -->  mat                               */
/*                       Z x Z x mmat -->  mmat                              */
/*                                                                           */
/* parameter:  arg1 - rowpar,                                                */
/*             arg2 - columnpar,                                             */
/*             arg3 - matrix                                                 */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/
int red_mcut(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int rowcut,colcut,nrows,ncols;
  int dim;                         /* die Zahl der Elemente der neuen Matrix */
  int pos;                         /* die Position des ersten Elementes der  */
                                   /* alten Matrix, welches an die erste     */
                                   /* Postion der neuen Matrix gelegt werden */
                                   /* muss.                                  */
  int jump = 0;                    /* die Zahl der Elemente die bei einem    */
                                   /* Spaltenschnitt uebersprungen werden    */
                                   /* muessen.                               */
  int i,j,element;

  int *ipnew,*ip,type;
  double *rpnew,*rp;

  DBUG_ENTER ("red_mcut");
  START_MODUL("red_mcut");

/* changes for lred by RS in June 1993 */
  if ((T_POINTER(arg1) && T_POINTER(arg2) && T_POINTER(arg3)) ||
      (T_INT(arg1) && T_INT(arg2) && T_POINTER(arg3))) {
    if (R_DESC(DESC(arg3),class) != C_MATRIX) goto fail;

    /* changes for lred by RS in June 1993 */
    if (T_INT(arg1))
      rowcut = VAL_INT(arg1);
    else
      STRUCT_ARG(arg1,rowcut);

    nrows = R_MVT(DESC(arg3),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg3),ncols,C_MATRIX);

    if (rowcut > 0) {                                /* row - cutfirst      */
      nrows -= rowcut;
      DBUG_PRINT ("red_mcut", ("nrows: %d", nrows));
      if (nrows < 0) goto fail;
      pos = rowcut * ncols;
    }
    else if (rowcut < 0) {                           /* row - cutlast       */
           nrows += rowcut;
            DBUG_PRINT ("red_mcut", ("nrows: %d", nrows));
           if (nrows < 0) goto fail;
           pos = 0;
         }
         else pos = 0;

    /* changes for lred by RS in June 1993 */
    if (T_INT(arg2))
      colcut = VAL_INT(arg2);
    else
      STRUCT_ARG(arg2,colcut);

    if (colcut > 0) {                                /* column - cutfirst   */
      ncols -= colcut;
       DBUG_PRINT ("red_mcut", ("ncols: %d", ncols));
      if (ncols < 0) goto fail;
      jump = colcut;
      pos += colcut;
    }
    else if (colcut < 0) {                           /* column - cutlast    */
           ncols += colcut;
           DBUG_PRINT ("red_mcut", ("ncols: %d", ncols));
           if (ncols < 0) goto fail;
           jump = -colcut;
         }

    if ((dim = nrows * ncols) == 0) {                /* empty matrix        */
      _desc = _nilmat;
      INC_REFCNT(_nilmat);
      goto success;
    }

    NEWDESC(_desc); TEST_DESC;

    if ((type = R_DESC(DESC(arg3),type)) != TY_REAL) {
      MVT_DESC_MASK(_desc,1,C_MATRIX,type);       /* create a new descriptos */
      L_MVT((*_desc),nrows,C_MATRIX) = nrows;
      L_MVT((*_desc),ncols,C_MATRIX) = ncols;
      NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
      RES_HEAP;
      ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);  /*----------------------*/
      ip    = (int *) R_MVT(DESC(arg3),ptdv,C_MATRIX);/* copy "cutted matrix" */
                                                      /*----------------------*/
      if (colcut == 0)
        if (type >= TY_STRING)
          for (i = 0; i < dim; ++i) {
            element = ipnew[i] = ip[pos];
            INC_REFCNT((PTR_DESCRIPTOR)element);
            ++pos;
          }
        else for (i = 0; i < dim; ++i) {
               ipnew[i] = ip[pos];
               ++pos;
             }
      else                              /* auch ein Spaltenschnitt verlangt: */
        if (type >= TY_STRING)
          for ( i = 0; i < dim; i+= ncols) {
            for ( j = 0; j < ncols; ++j) {
              ipnew[i+j] = element = ip[pos];
              INC_REFCNT((PTR_DESCRIPTOR)element);
              ++pos;
            }
            pos += jump;
          }
        else
          for ( i = 0; i < dim; i+= ncols) {
            for ( j = 0; j < ncols; ++j) {
              ipnew[i+j] = ip[pos];
              ++pos;
            }
            pos += jump;
          }
    }
    else {                                        /* type == TY_REAL         */
      MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);    /* create a new descriptos */
      L_MVT((*_desc),nrows,C_MATRIX) = nrows;
      L_MVT((*_desc),ncols,C_MATRIX) = ncols;
      NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
      RES_HEAP;
      rpnew = (double *)R_MVT((*_desc),ptdv,C_MATRIX);  /*----------------------*/
      rp    = (double *)R_MVT(DESC(arg3),ptdv,C_MATRIX);/* copy "cutted matrix" */
                                                        /*----------------------*/
      if (colcut == 0)
        for (i = 0; i < dim; ++i) {
          rpnew[i] = rp[pos];
          ++pos;
        }
      else                              /* auch ein Spaltenschnitt verlangt: */
        for ( i = 0; i < dim; i+= ncols) {
          for ( j = 0; j < ncols; ++j) {
            rpnew[i+j] = rp[pos];
            ++pos;
          }
          pos += jump;
        }
    } /* end else TY_REAL */
    REL_HEAP;
    goto success;
  } /* end of if  T_POINTER.. */

fail:
  END_MODUL("red_mcut");
  DBUG_RETURN(0);

success:
    /* changes for lred by RS in June 1993 */
    if (T_POINTER(arg1))
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    if (T_POINTER(arg2))
      DEC_REFCNT((PTR_DESCRIPTOR)arg2);
    DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_mcut");
  DBUG_RETURN(1);
}   /**** end of function red_mcut ****/

/*****************************************************************************/
/*                               red_mselect                                 */
/* ------------------------------------------------------------------------- */
/* function :                                                                */
/*      Mit Hilfe der Funktion mselect koennen einzelen Elemente, Zeilen     */
/*      oder Spalten der Matrix  'mat' selektiert  werden. Ist der  Wert     */
/*      von 'columnpar' (bzw. 'rowpar') gleich  Null, so liefert mselect     */
/*      die durch den zweiten Parameter bestimmte  Zeile  (bzw. Spalte).     */
/*      Ist 'rowpar` bzw. 'columnpar` ungleich Null, so wird ein Element     */
/*      (spezifiziert durch rowpar,columnpar) der Matrix selektiert. Bei     */
/*      Matrizen  auf  Matrizen  ist  nur  die  Selektion  einer  Matrix     */
/*      moeglich.                                                            */
/*                                                                           */
/*             mselect :   N  x   N   x  mat   -->  Number                   */
/*                         N  x  {0}  x  mat   -->  vect                     */
/*                        {0} x   N   x  mat   -->  tvect                    */
/*                         N  x   N   x  mmat  -->  mat  / bmat /            */
/*                                                  smat / mmat              */
/*                         analog mit arg3 == bmat bzw. smat                 */
/*                                                                           */
/* parameter:  arg1 - rowpar,                                                */
/*             arg2 - columnpar,                                             */
/*             arg3 - matrix                                                 */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_mselect(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int    col,row,nrows,ncols,i,type;
  int    *ip, *ipnew;
  double *rp, *rpnew;

  DBUG_ENTER ("red_mselect");
  START_MODUL("red_mselect");

/* changes for lred by RS in June 1993 */
  if ((T_POINTER(arg1) && T_POINTER(arg2) && T_POINTER(arg3)) ||
      (T_INT(arg1) && T_INT(arg2) && T_POINTER(arg3))) {
    if (R_DESC(DESC(arg3),class) != C_MATRIX) goto fail;

    /* changes for lred by RS in June 1993 */
    if (T_INT(arg1))
      row = VAL_INT(arg1);
    else
      STRUCT_ARG(arg1,row);
    if (row < 0) goto fail;

    /* changes for lred by RS in June 1993 */
    if (T_INT(arg2))
      col = VAL_INT(arg2);
    else
      STRUCT_ARG(arg2,col);
    if (col < 0) goto fail;

    if ((row > (nrows = R_MVT(DESC(arg3),nrows,C_MATRIX)) ) ||
        (col > (ncols = R_MVT(DESC(arg3),ncols,C_MATRIX))) )
      goto fail;

    /* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

                                            /*-------------------------------*/
                                            /*      "select a TVECTOR"       */
    if (row == 0)        /* "arg1" = 0  */  /*-------------------------------*/
                         /* "arg2" = 0  */
      if ((col == 0) ||
          ((type = R_DESC(DESC(arg3),type)) == TY_MATRIX))
        goto fail;
      else

        if (ncols == 1) {                   /*===============================*/
                                            /* {0} x 1 x (nx1)-mat --> tvect */
                                            /*-------------------------------*/
                                            /* generate indirect descriptor  */
         _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR)arg3);
         TEST_DESC;
         goto success;
        }
        else {                              /*===============================*/
                                            /* {0} x 1 x (nxm)-mat --> tvect */
          NEWDESC(_desc); TEST_DESC;        /*-------------------------------*/
          if (type != TY_REAL) {
            MVT_DESC_MASK(_desc,1,C_TVECTOR,type); /*  interne Typ-Konversion*/
            L_MVT((*_desc),nrows,C_TVECTOR) = nrows;/* mat --> tvect         */
            L_MVT((*_desc),ncols,C_TVECTOR) = 1;
            NEWHEAP(nrows,A_MVT((*_desc),ptdv,C_TVECTOR));
            RES_HEAP;
            ipnew = (int *)R_MVT((*_desc),ptdv,C_TVECTOR);
            ip    = (int *)R_MVT(DESC(arg3),ptdv,C_MATRIX);
            col--;
            if (type >= TY_STRING)      /* so sind die Elemente Descriptoren */
              for (i = 0; i < nrows; ++i) {
                ipnew[i] = ip[col];
                INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
                col += ncols;
              }
            else
              for (i = 0; i < nrows; ++i) {
                ipnew[i] = ip[col];
                col += ncols;
              }
            REL_HEAP;
            goto success;
          } /** arg3  ist nicht vom Typ REAL  **/
          else {
            MVT_DESC_MASK(_desc,1,C_TVECTOR,TY_REAL);/* interne Typ-Konversion*/
            L_MVT((*_desc),nrows,C_TVECTOR) = nrows; /* mat --> tvect         */
            L_MVT((*_desc),ncols,C_TVECTOR) = 1;
            NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,C_TVECTOR));
            RES_HEAP;
            rpnew = (double *) R_MVT((*_desc),ptdv,C_TVECTOR);
            rp    = (double *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
            col--;
            for (i = 0; i < nrows; ++i) {
              rpnew[i] = rp[col];
              col += ncols;
            }
            REL_HEAP;
            goto success;
          } /** arg3  ist vom Typ REAL  **/
        } /** end else ncols > 1) **/

    /* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

                                            /*-------------------------------*/
                                            /*      "select a VECTOR"        */
                                            /*-------------------------------*/
    if (col == 0) {
      if ((type = R_DESC(DESC(arg3),type)) == TY_MATRIX)
        goto fail;
                                            /*===============================*/
                                            /* 1 x {0} x (1xm)-mat --> vect  */
      if (nrows == 1) {                     /*-------------------------------*/
                                            /* generate indirect descriptor  */
        _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR)arg3);
        TEST_DESC;
        goto success;
      }
      else {                                /*===============================*/
                                            /* 1 x {0} x (nxm)-mat --> vect  */
        NEWDESC(_desc); TEST_DESC;          /*-------------------------------*/
        if (type != TY_REAL) {
          MVT_DESC_MASK(_desc,1,C_VECTOR,type);    /* interne Typ-Konversion */
          L_MVT((*_desc),nrows,C_VECTOR) = 1;      /* mat --> vect           */
          L_MVT((*_desc),ncols,C_VECTOR) = ncols;
          NEWHEAP(ncols,A_MVT((*_desc),ptdv,C_VECTOR));
          RES_HEAP;
          ipnew = (int *) R_MVT((*_desc),ptdv,C_VECTOR);
          ip    = ((int *) R_MVT(DESC(arg3),ptdv,C_MATRIX)) + (row-1)*ncols;

          if (type >= TY_STRING)        /* so sind die Elemente Descriptoren */
            for (i = 0; i < ncols; ++i) {
              ipnew[i] = ip[i];
              INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
            }
          else
            for (i = 0; i < ncols; ++i)
              ipnew[i] = ip[i];
          REL_HEAP;
          goto success;
        } /** arg3  ist nicht vom Typ REAL  **/
        else {
          MVT_DESC_MASK(_desc,1,C_VECTOR,TY_REAL); /* interne Typ-Konversion */
          L_MVT((*_desc),ncols,C_VECTOR) = ncols;  /* mat --> vect           */
          L_MVT((*_desc),nrows,C_VECTOR) = 1;
          NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,C_VECTOR));
          RES_HEAP;
          rpnew =  (double *) R_MVT((*_desc),ptdv,C_VECTOR);
          rp    = ((double *) R_MVT(DESC(arg3),ptdv,C_MATRIX)) + (row-1)*ncols;

          for (i = 0; i < ncols; ++i)
            rpnew[i] = rp[i];
          REL_HEAP;
          goto success;
        } /** arg3  ist vom Typ REAL  **/
      } /** end else nrows > 0) **/
    } /** col == 1 **/
    /* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

                                                   /*------------------------*/
    row--; col--;                                  /* "select a SCALAR"      */
    switch (R_DESC(DESC(arg3),type)) {             /*------------------------*/
      case TY_INTEGER:
           /* changes for lred by RS in June 1993 */
           if (_formated) {
             _desc = (PTR_DESCRIPTOR) TAG_INT(((int *) R_MVT(DESC(arg3),ptdv,C_MATRIX))[row * ncols + col]);
             }
           else {
           NEWDESC(_desc); TEST_DESC;              /* interne Typ-Konversion */
           DESC_MASK(_desc,1,C_SCALAR,TY_INTEGER); /* mat --> Number         */
           L_SCALAR((*_desc),vali) = ((int *) R_MVT(DESC(arg3),ptdv,C_MATRIX))
                                    [row * ncols + col];
             }
           goto success;
      case TY_REAL:
           NEWDESC(_desc); TEST_DESC;              /* interne Typ-Konversion */
           DESC_MASK(_desc,1,C_SCALAR,TY_REAL);    /* mat --> Number         */
           L_SCALAR((*_desc),valr) = ((double *) R_MVT(DESC(arg3),ptdv,C_MATRIX))
                                    [row * ncols + col];
           goto success;
      case TY_BOOL:
           if (R_MVT(DESC(arg3),ptdv,C_MATRIX)     /* interne Typ-Konversion */
                             [row * ncols + col])  /* bmat --> Bool          */
              _desc = (PTR_DESCRIPTOR)SA_TRUE;
           else
              _desc = (PTR_DESCRIPTOR)SA_FALSE;
           goto success;
      default:                                     /* smat --> String        */
                                                   /* mat  --> "TY_DIGIT"    */
                                                   /* mmat --> mat           */
           _desc = ((PTR_DESCRIPTOR *) R_MVT(DESC(arg3),ptdv,C_MATRIX))
                                    [row * ncols + col];
           INC_REFCNT(_desc);
           goto success;
    }
  } /* Ende von T_POINTER */
fail:
  END_MODUL("red_mselect");
  DBUG_RETURN(0);
success:
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  if (T_POINTER(arg2))
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_mselect");
  DBUG_RETURN(1);
} /**** end of function red_mselect ****/

/*****************************************************************************/
/*                               red_mreplace                                */
/* ------------------------------------------------------------------------- */
/* function :                                                                */
/*     mreplace ersetzt ein Element einer in Matrix ('mat'), ('mmat'). Wenn  */
/*     ein Scalar in einer Matrix  ersetzt  werden soll, muss  der Typ  des  */
/*     Elementes des dritten Parameters mit dem Typ der Matrix (des vierten  */
/*     Parameters) uebereinstimmen. Bei Matrizen mit mehr  als  zwei Dimen-  */
/*     sionen kann der Typ  des  Substitutes  (mat)  und  der des Matrizen-  */
/*     lementes  der Matrix auf Matrizen verschieden sein.                   */
/*                                                                           */
/*            mreplace :    N  x  N  x  Number  x mat   -->  mat             */
/*                          N  x  N  x  Bool    x bmat  -->  bmat            */
/*                          N  x  N  x  String  x bmat  -->  smat            */
/*                          N  x  N  x  mat     x mmat  -->  mmat            */
/*                          N  x  N  x  bmat    x mmat  -->  mmat            */
/*                          N  x  N  x  smat    x mmat  -->  mmat            */
/*                                                                           */
/* paramter :  arg1 - rowpar,                                                */
/*             arg2 - columnpar,                                             */
/*             arg3 - substitute (Number/Bool/String oder mat/bmat/smat),    */
/*             arg4 - matrix / Matrix mit mehr als zwei Dimensionen          */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_mreplace(arg1,arg2,arg3,arg4)
STACKELEM arg1,arg2,arg3,arg4;
{
  register int i,
               pos,                         /* position in dopevector        */
               dim,                         /* dimension of data vector      */
               nrows;                       /* number of rows                */
  int ncols,                                /* number of columns             */
      row,                                  /* structuring parameter rowpar  */
      col,                                  /* structuring parra. columnpar  */
      type;                                 /* type of arg4                  */

  int    *ip, *ipnew;
  double *rp, *rpnew;

  DBUG_ENTER ("red_mreplace");
  START_MODUL("red_mreplace");

  if ((!T_POINTER(arg1) || !T_POINTER(arg2) || !T_POINTER(arg4)) &&
      (!T_INT(arg1) || !T_INT(arg2) || !T_POINTER(arg4)))
    goto fail;

  if (R_DESC(DESC(arg4),class) != C_MATRIX) /* wenn arg4 keine Matrix ist   */
    goto fail;

/* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    row = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,row);
  if (T_INT(arg2))
    col = VAL_INT(arg2);
  else
    STRUCT_ARG(arg2,col);

   DBUG_PRINT ("red_mreplace",("yo here am i !**************"));

  nrows = R_MVT(DESC(arg4),nrows,C_MATRIX);
  ncols = R_MVT(DESC(arg4),ncols,C_MATRIX);

  DBUG_PRINT ("red_mreplace", ("col: %d, ncols: %d, row: %d, nrows: %d", col, ncols, row, nrows));

  if ((col < 1) || (col > ncols)) goto fail; /*------------------------------*/
  if ((row < 1) || (row > nrows)) goto fail; /*    substitute a NUMBER       */
  row--;                                     /*    or         a MATRIX       */
  col--;                                     /*------------------------------*/

  type = R_DESC(DESC(arg4),type);

  DBUG_PRINT ("red_mreplace", ("Is it a pointer: %d ???", T_POINTER(arg3)));

  if (T_POINTER(arg3)) {
    switch(R_DESC(DESC(arg3),class)) {       /* Type-check of  arg3 and arg4 */
      case C_LIST   : if (type != TY_STRING)
                        goto fail;
                      break;
      case C_SCALAR : if ((type != TY_INTEGER) && (type != TY_REAL))
                        goto fail;
                      break;
      case C_DIGIT  : if (type != TY_DIGIT)
                        goto fail;
                      break;
      case C_MATRIX : if (type != TY_MATRIX)
                        goto fail;
                      break;
      default       : goto fail;
    }
  } /** end T_POINTER **/

 DBUG_PRINT ("red_mreplace",("still here ! **************"));

  if (T_BOOLEAN(arg3)) {                    /* Type-check of  arg3 and arg4 */
    if (type != TY_BOOL)
      goto fail;
  }

 /* changes for lred by RS in June 1993 */
  if (T_INT(arg3)) {
    if ((type != TY_INTEGER) && (type != TY_REAL))
      goto fail;
    }

  dim = nrows * ncols;                       /* laenge des Dopevektors       */
  pos = row * ncols + col;                   /* zu ersetzende Position im    */
                                             /* im Dopevektor                */

  switch(type) {                             /* switch ueber eine Matrix     */
   /*~~~~~~~~~~~~~~~~*/                      /* mit Elementen eines bestim-  */
    case TY_INTEGER:                         /* mten Typs                    */
   /*~~~~~~~~~~~~~~~~*/
 /* changes for lred by RS in June 1993 */
         DBUG_PRINT ("red_mreplace",("here am i !**************"));
         if ((!T_POINTER(arg3) || (R_DESC(DESC(arg3),class) != C_SCALAR)) &&
             (!T_INT(arg3)))
           goto fail;

         NEWDESC(_desc); TEST_DESC;

    /* changes for lred by RS in June 1993 */
         if (T_INT(arg3) || (R_DESC(DESC(arg3),type) == TY_INTEGER)) {
           DBUG_PRINT ("red_mreplace", ("Yoyo here am i ! *********"));
           MVT_DESC_MASK(_desc,1,C_MATRIX,TY_INTEGER);
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ip    = (int *) R_MVT(DESC(arg4),ptdv,C_MATRIX);
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
           for (i = 0; i < dim; i++)
             ipnew[i] = ip[i];
            /* changes for lred by RS in June 1993 */
           if (T_INT(arg3))
             ipnew[pos] = VAL_INT(arg3);
           else
             ipnew[pos] = R_SCALAR(DESC(arg3),vali);
           REL_HEAP;
           goto success;
         }
         MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);   /* arg3 ist vom Typ REAL */
         NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
         RES_HEAP;
         ip    =    (int *) R_MVT(DESC(arg4),ptdv,C_MATRIX);
         rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
         for (i = 0; i < dim; i++)
            rpnew[i] = (double) ip[i];
         rpnew[pos] =  R_SCALAR(DESC(arg3),valr);
         REL_HEAP;
         goto success;

   /*~~~~~~~~~~~~~~~~*/
    case TY_REAL:
   /*~~~~~~~~~~~~~~~~*/
         if (!T_POINTER(arg3) || (R_DESC(DESC(arg3),class) != C_SCALAR))
           goto fail;

         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
         NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg4),ptdv,C_MATRIX);
         rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
         for (i = 0; i < dim; i++)
            rpnew[i] = rp[i];

         if (R_DESC(DESC(arg3),type) == TY_INTEGER)/* arg3 ist vom Typ INTEGER*/
           rpnew[pos] = (double) R_SCALAR(DESC(arg3),vali);
         else                                      /* arg3 ist vom Typ REAL   */
           rpnew[pos] =  R_SCALAR(DESC(arg3),valr);
         REL_HEAP;
         goto success;

   /*~~~~~~~~~~~~~~~~*/
    case TY_BOOL:
   /*~~~~~~~~~~~~~~~~*/
         if (!T_BOOLEAN(arg3))
           goto fail;
         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,C_MATRIX,TY_BOOL);
         NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg4),ptdv,C_MATRIX);
         ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
         for (i = 0; i < dim; i++)
           ipnew[i] = ip[i];
         ipnew[pos] = (T_SA_TRUE(arg3) ? TRUE : FALSE);
         REL_HEAP;
         goto success_3;

   /*~~~~~~~~~~~~~~~~*/
    case TY_DIGIT :
    case TY_STRING:
    case TY_MATRIX:
   /*~~~~~~~~~~~~~~~~*/
         if (!T_POINTER(arg3))
           goto fail;

         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,C_MATRIX,type);
         NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
         RES_HEAP;
         ip =    (int *) R_MVT(DESC(arg4),ptdv,C_MATRIX);
         ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
         for (i = 0; i < dim; i++) {
          if (i == pos)
            ipnew[i] = arg3;
          else
            INC_REFCNT((PTR_DESCRIPTOR)(ipnew[i] = ip[i]));
         }
         REL_HEAP;
         goto success_3;
  } /** end of substitue a NUMBER */

 DBUG_PRINT ("red_mreplace",("here am i , that's not good !!**************"));

fail:
  END_MODUL("red_mreplace");
  DBUG_RETURN(0);
success:
  /* changes for lred by RS in June 1993 */
  if (!T_INT(arg3))
    DEC_REFCNT((PTR_DESCRIPTOR)arg3);
success_3:
  L_MVT((*_desc),nrows,C_MATRIX) = nrows;
  L_MVT((*_desc),ncols,C_MATRIX) = ncols;
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  if (T_POINTER(arg2))
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  DEC_REFCNT((PTR_DESCRIPTOR)arg4);
  END_MODUL("red_mreplace");
  DBUG_RETURN(1);
} /**** end of function red_mreplace ****/

/*****************************************************************************/
/*                        red_mre_c  (replace a column)                      */
/* ------------------------------------------------------------------------- */
/* function :  mreplace_c ersetzt eine Spalte in  einer  Matrix. Das  zweite */
/*             Argument muss ein transponierter Vektor sein. Mit 'columnpar' */
/*             kann man die zu ersetzende Spalte der Matrix angeben. Der Typ */
/*             des TVektors und der Matrix muss gleich sein.                 */
/*                                                                           */
/*             mreplace_c :  N  x  tvect   x  mat  -->  mat                  */
/*                           N  x  tbvect  x  bmat -->  bmat                 */
/*                           N  x  tsvect  x  smat -->  smat                 */
/*                                                                           */
/* paramter :  arg1 - columnpar                                              */
/*             arg2 - tvector                                                */
/*             arg3 - matrix                                                 */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/
int red_mre_c(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  register int i,
               pos,                         /* position in dopevector        */
               dim,                         /* dimension of data vector      */
               nrows;                       /* number of rows                */
  int ncols,                                /* number of columns             */
      col,                                  /* structuring parameter         */
      type;                                 /* same type of elements         */
                                            /* in arg2  and arg3             */
  int    *ip, *ipnew;
  double *rp, *rpnew;

  DBUG_ENTER ("red_mre_c");
  START_MODUL("red_mre_c");

 /* changes for lred by RS in June 1993 */
  if ((!T_POINTER(arg1) && !T_INT(arg1)) || !T_POINTER(arg2) || !T_POINTER(arg3) ||
      ((type = R_DESC(DESC(arg3),type)) == TY_MATRIX))
     goto fail;

  if ( (R_DESC(DESC(arg2),class) != C_TVECTOR) ||
       (R_DESC(DESC(arg3),class) != C_MATRIX) )
    goto fail;

 /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    col = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,col);
  nrows = R_MVT(DESC(arg3),nrows,C_MATRIX);
  ncols = R_MVT(DESC(arg3),ncols,C_MATRIX);
  dim = nrows * ncols;                       /* length of data vector        */

  if ((col < 1) || (col > ncols))
    goto fail;

  if (R_MVT(DESC(arg2),nrows,C_TVECTOR) != nrows)
    goto fail;
                                             /*------------------------------*/
  col--;                                     /*    substitute a TVECTOR      */
                                             /*------------------------------*/

  if (type == R_DESC(DESC(arg2),type)) {     /* arg2 und arg3 sind vom       */
                                             /* selben Typ                   */
    if (ncols == 1) {
              /* arg3 ist eine (1x1)-Matrix  oder eine (nx1)-Matrix  */
              /* erzeuge einen indirekten Deskriptor aus dem TVektor */
      _desc = gen_many_id(C_MATRIX,(PTR_DESCRIPTOR)arg2);
      TEST_DESC;
      goto success_arg2;
    }
    NEWDESC(_desc); TEST_DESC;

    if (type == TY_REAL) {                            /* arg2      arg3      */
      MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);        /* TY_REAL   TY_REAL   */
      NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
      RES_HEAP;
      rp    = (double *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
      rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
      pos = col;
      for (i = 0; i < dim; i++)
        if (i != pos)
          rpnew[i] = rp[i];
        else
          pos += ncols;
      rp = (double *) R_MVT(DESC(arg2),ptdv,C_TVECTOR);
      rpnew += col;
      for (i = 0; i < nrows; i++) {
        *rpnew = rp[i];
        rpnew += ncols;
      }
      REL_HEAP;
      goto success;
    }
    MVT_DESC_MASK(_desc,1,C_MATRIX,type);
    NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
    RES_HEAP;
    ip =    (int *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
    ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
    pos = col;

    if (type >= TY_STRING) {                        /* arg2        arg3      */
      for (i = 0; i < dim; i++)                     /* TY_DIGIT    TY_DIGIT  */
        if (i != pos)                               /* TY_STRING   TY_STRING */
          INC_REFCNT((PTR_DESCRIPTOR)(ipnew[i] = ip[i]));
        else
          pos += ncols;
      ip = (int *) R_MVT(DESC(arg2),ptdv,C_TVECTOR);
      ipnew += col;
      for (i = 0; i < nrows; i++) {
        INC_REFCNT((PTR_DESCRIPTOR)(*ipnew = ip[i]));
        ipnew += ncols;
      }
    }
    else {                                      /* arg2          arg3       */
      for (i = 0; i < dim; i++)                 /* TY_INTEGER    TY_INTEGER */
        if (i != pos)
          ipnew[i] = ip[i];
        else
          pos += ncols;
      ip = (int *) R_MVT(DESC(arg2),ptdv,C_TVECTOR);
      ipnew += col;
      for (i = 0; i < nrows; i++) {
        *ipnew = ip[i];
        ipnew += ncols;
      }
    }
    REL_HEAP;
    goto success;
  } /** end of same type of arg2 and arg3 **/

  if ((ncols == 1) &&
      ((R_DESC(DESC(arg2),type) == TY_REAL) ||
       (R_DESC(DESC(arg2),type) == TY_INTEGER))) {
         /* arg3 ist eine (1x1)-Matrix  oder eine (nx1)-Matrix  */
         /* erzeuge einen indirekten Deskriptor aus dem TVektor */
    _desc = gen_many_id(C_MATRIX,(PTR_DESCRIPTOR)arg2);
    TEST_DESC;
    goto success_arg2;
  }

  switch(R_DESC(DESC(arg2),type)) {          /* arg2              arg3       */
   /*~~~~~~~~~~~~~~~~*/                      /* TY_REAL     ==>   TY_INTEGER */
    case TY_REAL:                            /* TY_INTEGER  ==>   TY_REAL    */
   /*~~~~~~~~~~~~~~~~*/                      /* sonst fail                   */
         if (type != TY_INTEGER) goto fail;
         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
         NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
         rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
         pos = col;
         for (i = 0; i < dim; i++)
           if (i != pos)
             rpnew[i] = (double) ip[i];
           else
             pos += ncols;
         rp = (double *) R_MVT(DESC(arg2),ptdv,C_TVECTOR);
         rpnew += col;
         for (i = 0; i < nrows; i++) {
           *rpnew = rp[i];
           rpnew += ncols;
         }
         REL_HEAP;
         goto success;
   /*~~~~~~~~~~~~~~~~*/
    case TY_INTEGER:
   /*~~~~~~~~~~~~~~~~*/
         if (type != TY_REAL) goto fail;
         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
         NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
         rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
         pos = col;
         for (i = 0; i < dim; i++)
           if (i != pos)
             rpnew[i] = rp[i];
           else
             pos += ncols;
         ip = (int *) R_MVT(DESC(arg2),ptdv,C_TVECTOR);
         rpnew += col;
         for (i = 0; i < nrows; i++)
         {
           *rpnew = (double) ip[i];
           rpnew += ncols;
         }
         REL_HEAP;
         goto success;
  } /** end switch type of arg2 **/

fail:
  END_MODUL("red_mre_c");
  DBUG_RETURN(0);
success:
  L_MVT((*_desc),nrows,C_MATRIX) = nrows;
  L_MVT((*_desc),ncols,C_MATRIX) = ncols;
success_arg2:
  /* changes for lred by RS in June 1993 */
  if (!T_INT(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_mre_c");
  DBUG_RETURN(1);
} /**** end of function red_mre_c ****/

/*****************************************************************************/
/*                         red_mre_r  (replace a row)                        */
/* ------------------------------------------------------------------------- */
/* function :  mreplace_r ersetzt eine Zeile in einer Matrix. Das zweite     */
/*             Argument  muss  ein  Vektor  sein. Mit 'rowpar' kann man      */
/*             die zu ersetzende Zeile der Matrix angeben. Der Typ  des      */
/*             Vektors und der Matrix muss gleich sein.                      */
/*                                                                           */
/*             mreplace_r :  N  x  vect  x  mat  -->  mat                    */
/*                           N  x  bvect x  bmat -->  bmat                   */
/*                           N  x  svect x  smat -->  smat                   */
/*                                                                           */
/* paramter :  arg1 - rowpar,                                                */
/*             arg2 - vector                                                 */
/*             arg3 - matrix                                                 */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/
int red_mre_r(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  register int i,
               pos,                         /* position in dopevector        */
               dim,                         /* dimension of data vector      */
               nrows;                       /* number of rows                */
  int ncols,                                /* number of columns             */
      row,                                  /* structuring parameter         */
      type;                                 /* same type of elements         */
                                            /* in arg2  and arg3             */
  int    *ip, *ipnew;
  double *rp, *rpnew;

  DBUG_ENTER ("red_mre_r");
  START_MODUL("red_mre_r");
  
  /* changes for lred by RS in June 1993 */
  if ((!T_POINTER(arg1) && !T_INT(arg1)) || !T_POINTER(arg2) || !T_POINTER(arg3) ||
      ((type = R_DESC(DESC(arg3),type)) == TY_MATRIX))
     goto fail;

  if ( (R_DESC(DESC(arg2),class) != C_VECTOR) ||
       (R_DESC(DESC(arg3),class) != C_MATRIX) )
    goto fail;

  /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    row = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,row);
  nrows = R_MVT(DESC(arg3),nrows,C_MATRIX);
  ncols = R_MVT(DESC(arg3),ncols,C_MATRIX);
  dim = nrows * ncols;                       /* length of data vector        */

  if ((row < 1) || (row > nrows))
    goto fail;

  if (R_MVT(DESC(arg2),ncols,C_VECTOR) != ncols)
    goto fail;
                                              /*------------------------------*/
  row--;                                      /*     substitute a VECTOR      */
                                              /*------------------------------*/
  if (type == R_DESC(DESC(arg2),type)) {     /* arg2 und arg3 sind vom       */
                                             /* selben Typ                   */
    if (nrows == 1) {
              /* arg3 ist eine (1x1)-Matrix  oder eine (1xm)-Matrix  */
              /* erzeuge einen indirekten Deskriptor aus dem Vektor  */
      _desc = gen_many_id(C_MATRIX,(PTR_DESCRIPTOR)arg2);
      TEST_DESC;
      goto success_arg2;
    }

    NEWDESC(_desc); TEST_DESC;

    if (type == TY_REAL) {                            /* arg2      arg3      */
      MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);        /* TY_REAL   TY_REAL   */
      NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
      RES_HEAP;
      rp    = (double *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
      rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
      pos = row * ncols;
      for (i = 0; i < pos; i++)
        rpnew[i] = rp[i];
      for (i = pos + ncols; i < dim; i++)
        rpnew[i] = rp[i];
      rp = (double *) R_MVT(DESC(arg2),ptdv,C_VECTOR);
      rpnew += pos;
      for (i = 0; i < ncols; i++)
      rpnew[i] = rp[i];
      REL_HEAP;
      goto success;
    }

    MVT_DESC_MASK(_desc,1,C_MATRIX,type);
    NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
    RES_HEAP;
    ip = (int *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
    ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
    pos = row * ncols;

    if (type >= TY_STRING) {                        /* arg2        arg3      */
                                                    /* TY_DIGIT    TY_DIGIT  */
      for (i = 0; i < pos; i++)                     /* TY_STRING   TY_STRING */
        INC_REFCNT((PTR_DESCRIPTOR)(ipnew[i] = ip[i]));
      for (i = pos + ncols; i < dim; i++)
        INC_REFCNT((PTR_DESCRIPTOR)(ipnew[i] = ip[i]));
      ip = (int *) R_MVT(DESC(arg2),ptdv,C_VECTOR);
      ipnew += pos;
      for (i = 0; i < ncols; i++)
        INC_REFCNT((PTR_DESCRIPTOR)(ipnew[i] = ip[i]));
    }
    else {
      for (i = 0; i < pos; i++)                  /* arg2          arg3       */
        ipnew[i] = ip[i];                        /* TY_INTEGER    TY_INTEGER */
      for (i = pos + ncols; i < dim; i++)
        ipnew[i] = ip[i];
      ip = (int *) R_MVT(DESC(arg2),ptdv,C_VECTOR);
      ipnew += pos;
      for (i = 0; i < ncols; i++)
        ipnew[i] = ip[i];
    }
    REL_HEAP;
    goto success;
  }

  if ((nrows == 1) &&
      ((R_DESC(DESC(arg2),type) == TY_REAL) ||
       (R_DESC(DESC(arg2),type) == TY_INTEGER))) {
         /* arg3 ist eine (1x1)-Matrix  oder eine (1xm)-Matrix  */
         /* erzeuge einen indirekten Deskriptor aus dem Vektor  */
    _desc = gen_many_id(C_MATRIX,(PTR_DESCRIPTOR)arg2);
    TEST_DESC;
    goto success_arg2;
  }

  switch(R_DESC(DESC(arg2),type)) {          /* arg2              arg3       */
    /*~~~~~~~~~~~~~~*/                       /* TY_REAL     ==>   TY_INTEGER */
    case TY_REAL:                            /* TY_INTEGER  ==>   TY_REAL    */
    /*~~~~~~~~~~~~~~*/                       /* sonst fail                   */
         if (type != TY_INTEGER) goto fail;
         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
         NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
         RES_HEAP;
         ip    =    (int *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
         rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
         pos = row * ncols;
         for (i = 0; i < pos; i++)
           rpnew[i] = (double) ip[i];
         for (i = pos + ncols; i < dim; i++)
           rpnew[i] = (double) ip[i];
         rp = (double *) R_MVT(DESC(arg2),ptdv,C_VECTOR);
         rpnew += pos;
         for (i = 0; i < ncols; i++)
           rpnew[i] = rp[i];
         REL_HEAP;
         goto success;
    /*~~~~~~~~~~~~~~*/
    case TY_INTEGER:
    /*~~~~~~~~~~~~~~*/
         if (type != TY_REAL) goto fail;
         NEWDESC(_desc); TEST_DESC;
         MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
         NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
         RES_HEAP;
         rp    = (double *) R_MVT(DESC(arg3),ptdv,C_MATRIX);
         rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
         pos = row * ncols;
         for (i = 0; i < pos; i++)
           rpnew[i] = rp[i];
         for (i = pos + ncols; i < dim; i++)
           rpnew[i] = rp[i];
         ip = (int *) R_MVT(DESC(arg2),ptdv,C_VECTOR);
         rpnew += pos;
         for (i = 0; i < ncols; i++)
           rpnew[i] = (double) ip[i];
         REL_HEAP;
         goto success;
  } /** end switch type of arg2 **/

fail:
  END_MODUL("red_mre_r");
  DBUG_RETURN(0);
success:
  L_MVT((*_desc),nrows,C_MATRIX) = nrows;
  L_MVT((*_desc),ncols,C_MATRIX) = ncols;
success_arg2:
  /* changes for lred by RS in June 1993 */
  if (!T_INT(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_mre_r");
  DBUG_RETURN(1);
} /**** end of function red_mre_r ****/

/*****************************************************************************/
/*                               red_mrotate                                 */
/* ------------------------------------------------------------------------- */
/* function :                                                                */
/*    Die Funktion rotate erzeugt eine zyklisch rotierte Matrix von 'mat'.   */
/*    Dabei kann gleichzeitig eine Zeilen- und eine Spaltenrotation durch-   */
/*    gefuehrt werden. Der Parameter einer Koordinate bestimmt,  das  wie-   */
/*    vielte Element nach dem ersten, das neue erste Element dieser  Koor-   */
/*    dinate werden soll. Bei  einem  negativen  Parameter  wird die Kopie   */
/*    entgegengesetzt rotiert.                                               */
/*                                                                           */
/*             mrotate :    Z x Z x mat  -->  mat                            */
/*                          Z x Z x mmat -->  mmat                           */
/*                                                                           */
/* parameter:  arg1 - rowpar,                                                */
/*             arg2 - columnpar,                                             */
/*             arg3 - matrix                                                 */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/
int red_mrotate(arg1,arg2,arg3)
STACKELEM arg1,arg2,arg3;
{
  register int dim,count;
  int  rowrotation,
       colrotation,
       nrows,
       ncols,
       pos,                                       /* Position im Dopevektor */
       newrow,
       newcol;

  int    *ipnew,*ip,type,element;
  double *rpnew,*rp;

  DBUG_ENTER ("red_mrotate");
  START_MODUL("red_mrotate");

  /* changes for lred by RS in June 1993 */
  if ((T_POINTER(arg1) && T_POINTER(arg2) && T_POINTER(arg3)) ||
      (T_INT(arg1) && T_INT(arg2) && T_POINTER(arg3))) {
    if (R_DESC(DESC(arg3),class) != C_MATRIX) goto fail;

    /* changes for lred by RS in June 1993 */
    if (T_INT(arg1))
      rowrotation = VAL_INT(arg1);
    else
      STRUCT_ARG(arg1,rowrotation);
    rowrotation = - rowrotation;            /* wurde nachtraeglich notwendig */

    if (arg3 == (int)_nilmat) {
      _desc = _nilmat;
      INC_REFCNT(_desc);
      goto success;
    }
    nrows = R_MVT(DESC(arg3),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg3),ncols,C_MATRIX);
                                                   /*-----------------------*/
    if (ncols == 1)                                /* (nx1) - Matrix        */
      if ((rowrotation = rowrotation % nrows) == 0)/*-----------------------*/
        goto nothingtodo;
      else {
        dim = nrows;
        if (rowrotation > 0)
          pos = rowrotation;
        else
          pos = dim + rowrotation;
        goto alg1;
      }
    /* else; */
    /* changes for lred by RS in June 1993 */
    if (T_INT(arg2))
      colrotation = VAL_INT(arg2);
    else
      STRUCT_ARG(arg2,colrotation);
    colrotation = - colrotation;
                                                   /*-----------------------*/
    if (nrows == 1)                                /* (1xm) - Matrix         */
      if ((colrotation = colrotation % ncols) == 0)/*-----------------------*/
        goto nothingtodo;
      else {
        dim = ncols;
        if (colrotation > 0)
          pos = colrotation;
        else
          pos = dim + colrotation;
        goto alg1;
      }
    /* else; */

    rowrotation = rowrotation % nrows;
    if (rowrotation < 0) rowrotation += nrows;
    colrotation = colrotation % ncols;
    if (colrotation < 0) colrotation += ncols;

    if (colrotation == 0)
      if (rowrotation == 0)
        goto nothingtodo;
      else {
        dim = ncols * nrows;
        pos = rowrotation * ncols;
        goto alg1;
      }
    /* else; */

    /* Dh. wir haben eine echtes MATRIX vor uns bei dem mindestens eine */
    /* Spaltenrotation gemacht werden muss:                             */

    newrow = rowrotation * ncols;                  /* initialize newrow      */
    newcol = colrotation;                          /* initialize newcol      */
    dim = nrows * ncols;                           /* initialize dim         */

    if ((type = R_DESC(DESC(arg3),type)) != TY_REAL)
    {
      NEWDESC(_desc); TEST_DESC;
      MVT_DESC_MASK(_desc,1,C_MATRIX,type);
      L_MVT((*_desc),nrows,C_MATRIX) = nrows;
      L_MVT((*_desc),ncols,C_MATRIX) = ncols;
      NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
      RES_HEAP;
      ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
      ip    = (int *) R_MVT(DESC(arg3),ptdv,C_MATRIX);

      if (type >= TY_STRING)
        for (count = 0; count < dim; count++) {
          ipnew[newrow + newcol] = element = ip[count];
          INC_REFCNT((PTR_DESCRIPTOR)element);
          newcol++;
          if (newcol == ncols) newcol = 0;
          else if (newcol == colrotation) {
                 newrow += ncols;
                 if (newrow == dim)
                   newrow = 0;
               }
        }
      else
        for (count = 0; count < dim; count++) {
          ipnew[newrow + newcol] = ip[count];
          newcol++;
          if (newcol == ncols) newcol = 0;
          else if (newcol == colrotation) {
                 newrow += ncols;
                 if (newrow == dim)
                   newrow = 0;
               }
        }
      REL_HEAP;
      goto success;
    }
    else {
      NEWDESC(_desc); TEST_DESC;
      MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
      L_MVT((*_desc),nrows,C_MATRIX) = nrows;
      L_MVT((*_desc),ncols,C_MATRIX) = ncols;
      NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
      RES_HEAP;
      rpnew = (double *)R_MVT((*_desc),ptdv,C_MATRIX);
      rp    = (double *)R_MVT(DESC(arg3),ptdv,C_MATRIX);

      for (count = 0; count < dim; count++) {
        rpnew[newrow + newcol] = rp[count];
        newcol++;
        if (newcol == ncols) newcol = 0;
        else if (newcol == colrotation) {
               newrow += ncols;
               if (newrow == dim)
                 newrow = 0;
             }
      }
      REL_HEAP;
      goto success;
    }
  } /* Ende von T_POINTER */
  goto fail;

alg1:
  NEWDESC(_desc); TEST_DESC;

  if ((type = R_DESC(DESC(arg3),type)) != TY_REAL) {
    MVT_DESC_MASK(_desc,1,C_MATRIX,type);
    L_MVT((*_desc),nrows,C_MATRIX) = nrows;
    L_MVT((*_desc),ncols,C_MATRIX) = ncols;
    NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
    RES_HEAP;
    ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
    ip    = (int *) R_MVT(DESC(arg3),ptdv,C_MATRIX);

    if (type >= TY_STRING)
      for (count = 0; count < dim; count++) {
        ipnew[pos] = element = ip[count];
        INC_REFCNT((PTR_DESCRIPTOR)element);
        ++pos;
        if (pos == dim) pos = 0;
      }
    else
      for (count = 0; count < dim; count++) {
        ipnew[pos] = ip[count];
        ++pos;
        if (pos == dim) pos = 0;
      }
    REL_HEAP;
    goto success;
  }
  else {
    MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
    L_MVT((*_desc),nrows,C_MATRIX) = nrows;
    L_MVT((*_desc),ncols,C_MATRIX) = ncols;
    NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
    RES_HEAP;
    rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
    rp    = (double *) R_MVT(DESC(arg3),ptdv,C_MATRIX);

    for (count = 0; count < dim; count++) {
      rpnew[pos] = rp[count];
      ++pos;
      if (pos == dim) pos = 0;
    }
    REL_HEAP;
    goto success;
  }

nothingtodo:
  /* changes for lred by RS in June 1993 */
  if (!T_INT(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  /* changes for lred by RS in June 1993 */
  if (!T_INT(arg2))
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  _desc = (PTR_DESCRIPTOR) arg3;
  END_MODUL("red_mrotate");
  DBUG_RETURN(1);

fail:
  END_MODUL("red_mrotate");
  DBUG_RETURN(0);

success:
  /* changes for lred by RS in June 1993 */
  if (!T_INT(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  /* changes for lred by RS in June 1993 */
  if (!T_INT(arg2))
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_mrotate");
  DBUG_RETURN(1);
} /**** end of function red_mrotate ****/

/*****************************************************************************/
/*                               red_munite                                  */
/* ------------------------------------------------------------------------- */
/* function :                                                                */
/*  Diese Funktion vereinigt Matrizen, Vektoren und TVektoren. Die Argumente */
/*  koennen  mit  Aussnahme  von  Matrizen auf Matrizen beliebig  kombiniert */
/*  werden, d.h. es koennen z.B. Vektoren und Matrizen vereinigt werden. Die */
/*  Vereinigung ist nur moeglich, wenn der Typ  der Elemente  von den beiden */
/*  Argumenten der gleiche ist. Es wird  bei 'coor'  gleich Eins zeilenweise */
/*  und bei 'coor' gleich Zwei spaltenweise vereinigt. munite  arbeitet  nur */
/*  auf Kopien der Argumente 'arg1' und 'arg2'. Falls eine Matrix  des  Typs */
/*  Integer mit einer Matrix des Typs Real vereinigt werden  soll,  so  ent- */
/*  steht eine Matrix mit  Elementen  des Typs Real. Wenn  eine  Matrix  mit */
/*  einem Vektor bzw. TVektor oder ein  Vektor  mit  einem TVektor vereinigt */
/*  werden soll, dann  muss  die  Dimension der "entsprechenden" Spalte bzw. */
/*  Zeile der beiden Argumente gleich sein. Das Ergebnis einer erfolgreichen */
/*  Reduktion ist immer eine Matrix.                                         */
/*                                                                           */
/*             munite  :   {1,2} x mmat  x mmat  -->  mmat                   */
/*                         {1,2} x mat   x mat   -->  mat                    */
/*                         {1,2} x mat   x vect  -->  mat                    */
/*                         {1,2} x mat   x tvect -->  mat                    */
/*                         {1,2} x vect  x mat   -->  mat                    */
/*                         {1,2} x vect  x vect  -->  mat                    */
/*                         {1,2} x vect  x tvect -->  mat                    */
/*                         {1,2} x tvect x mat   -->  mat                    */
/*                         {1,2} x tvect x tvect -->  mat                    */
/*                         {1,2} x tvect x vect  -->  mat                    */
/*                                                                           */
/* parameter:  arg1 - coor,                                                  */
/*             arg2 - matrix / vektor / tvector                              */
/*             arg3 - matrix / vektor / tvector                              */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_munite(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int    i,j,dim,element,coordinate,
         nrows,ncols,ncol1,ncol2,
         type2,                       /* fuer den Typ von arg2              */
         type3,                       /* fuer den Typ von arg3              */
         *ip,*ipnew,*ip1,*ip2;
  double *rp, *rpnew,*rp1,*rp2;
  char   class2,class3;               /* fuer die Klasse von arg2 bzw. arg3 */

  DBUG_ENTER ("red_munite");
  START_MODUL("red_munite");

  /* changes for lred by RS in June 1993 */
  if ((!T_POINTER(arg1) && !T_INT(arg1)) || !T_POINTER(arg2) || !T_POINTER(arg3))
    goto fail;

  switch(class2 = R_DESC(DESC(arg2),class)) {
    case C_VECTOR : break;
    case C_TVECTOR: break;
    case C_MATRIX : break;
    default       : goto fail;
  }

  switch(class3 = R_DESC(DESC(arg3),class)) {
    case C_VECTOR : break;
    case C_TVECTOR: break;
    case C_MATRIX : break;
    default       : goto fail;
  }

  /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    coordinate = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,coordinate);

  if ((coordinate != 1) && (coordinate != 2))
    goto fail;                            /* coordinate ist !=  (1 oder 2)  */

  if ((type2 = R_DESC(DESC(arg2),type)) == TY_UNDEF)
    goto nilmat;                             /* nilmat / nilvect / niltvect */

  if ((type3 = R_DESC(DESC(arg3),type)) == TY_UNDEF) {
                                             /* nilmat / nilvect / niltvect */
    _desc = (PTR_DESCRIPTOR)arg3;            /* Vertauschen der Argumente   */
    arg3 = arg2;
    arg2 = (STACKELEM)_desc;
    goto nilmat;
  }

  /*-----------------*/
  /* coodinate == 1  */
  /*-----------------*/
  if (coordinate == 1) {
    if ((ncols = R_MVT(DESC(arg2),ncols,class2)) !=
                 R_MVT(DESC(arg3),ncols,class3))
       goto fail;

    nrows = R_MVT(DESC(arg2),nrows,class2) +
            R_MVT(DESC(arg3),nrows,class3);
    dim = ncols * nrows;

    switch(type2) {

      case TY_INTEGER:
           switch(type3) {

             case TY_INTEGER:                     /* Wertebereich ist immer  */
                  NEWDESC(_desc); TEST_DESC;      /* eine Matrix             */
                  MVT_DESC_MASK(_desc,1,C_MATRIX,TY_INTEGER);
                  NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
                  RES_HEAP;
                  ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
                  ip    = (int *) R_MVT(DESC(arg2),ptdv,class2);
                  dim   = ncols * R_MVT(DESC(arg2),nrows,class2);

                  for (i = 0; i < dim; ++i)
                    ipnew[i] = ip[i];

                  ipnew += dim;
                  ip  = (int *) R_MVT(DESC(arg3),ptdv,class3);
                  dim = ncols * R_MVT(DESC(arg3),nrows,class3);

                  for (i = 0; i < dim; ++i)
                  ipnew[i] = ip[i];
                  REL_HEAP;
                  break;

             case TY_REAL:                        /* Wertebereich ist immer  */
                  NEWDESC(_desc); TEST_DESC;      /* eine Matrix             */
                  MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
                  ip    = (int *) R_MVT(DESC(arg2),ptdv,class2);
                  dim   = ncols * R_MVT(DESC(arg2),nrows,class2);

                  for (i = 0; i < dim; ++i)
                    rpnew[i] = (double) ip[i];

                  rpnew += dim;
                  rp = (double *) R_MVT(DESC(arg3),ptdv,class3);
                  dim = ncols * R_MVT(DESC(arg3),nrows,class3);

                  for (i = 0; i < dim; ++i)
                    rpnew[i] = rp[i];
                  REL_HEAP;
                  break;
             default: goto fail;
           }
           break;

      case TY_REAL:
           switch (type3) {

             case TY_INTEGER:                     /* Wertebereich ist immer  */
                  NEWDESC(_desc); TEST_DESC;      /* eine Matrix             */
                  MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
                  rp    = (double *) R_MVT(DESC(arg2),ptdv,class2);
                  dim = ncols * R_MVT(DESC(arg2),nrows,class2);

                  for (i = 0; i < dim; ++i)
                    rpnew[i] = rp[i];

                  rpnew += dim;
                  ip = (int *) R_MVT(DESC(arg3),ptdv,class3);
                  dim = ncols * R_MVT(DESC(arg3),nrows,class3);

                  for (i = 0; i < dim; ++i)
                    rpnew[i] = (double) ip[i];
                  REL_HEAP;
                  break;

             case TY_REAL:                        /* Wertebereich ist immer  */
                  NEWDESC(_desc); TEST_DESC;      /* eine Matrix             */
                  MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
                  rp    = (double *) R_MVT(DESC(arg2),ptdv,class2);
                  dim = ncols * R_MVT(DESC(arg2),nrows,class2);

                  for (i = 0; i < dim; ++i)
                    rpnew[i] = rp[i];

                  rpnew += dim;
                  rp = (double *) R_MVT(DESC(arg3),ptdv,class3);
                  dim = ncols * R_MVT(DESC(arg3),nrows,class3);

                  for (i = 0; i < dim; ++i)
                    rpnew[i] = rp[i];
                  REL_HEAP;
                  break;
             default: goto fail;
           }
           break;

      default:
           if (type3 != type2)
             goto fail;
           NEWDESC(_desc); TEST_DESC;             /* Wertebereich ist immer  */
           MVT_DESC_MASK(_desc,1,C_MATRIX,type3); /* eine Matrix             */
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
           ip    = (int *) R_MVT(DESC(arg2),ptdv,class2);
           dim = ncols * R_MVT(DESC(arg2),nrows,class2);

           if (type3 == TY_BOOL)
             for (i = 0; i < dim; ++i)
               ipnew[i] = ip[i];
           else
             for (i = 0; i < dim; ++i) {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR) ip[i]);
             }

           ipnew += dim;
           ip = (int *) R_MVT(DESC(arg3),ptdv,class3);
           dim = ncols * R_MVT(DESC(arg3),nrows,class3);

           if (type3 == TY_BOOL)
             for ( i = 0; i < dim; ++i)
               ipnew[i] = ip[i];
           else
             for (i = 0; i < dim; ++i) {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR) ip[i]);
             }
           REL_HEAP;
    } /** end switch type of arg2 **/

    L_MVT((*_desc),nrows,C_MATRIX) = nrows;       /* Wertebereich ist immer  */
    L_MVT((*_desc),ncols,C_MATRIX) = ncols;       /* eine Matrix             */
    goto success;
  } /** Ende des Falles Koordinate == 1 **/

  /*-----------------*/
  /* coodinate == 2  */
  /*-----------------*/
  if (coordinate == 2) {
    if ((nrows = R_MVT(DESC(arg2),nrows,class2)) !=
                 R_MVT(DESC(arg3),nrows,class3))
      goto fail;

    ncols  = ncol1 = R_MVT(DESC(arg2),ncols,class2);
    ncols += ncol2 = R_MVT(DESC(arg3),ncols,class3);
    dim = ncols * nrows;

    switch(type2) {

      case TY_INTEGER:
           switch(type3) {

             case TY_INTEGER:                     /* Wertebereich ist immer  */
                  NEWDESC(_desc); TEST_DESC;      /* eine Matrix             */
                  MVT_DESC_MASK(_desc,1,C_MATRIX,TY_INTEGER);
                  NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
                  RES_HEAP;
                  ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
                  ip1   = (int *) R_MVT(DESC(arg2),ptdv,class2);
                  ip2   = (int *) R_MVT(DESC(arg3),ptdv,class3);

                  for (i = 0; i < nrows; ++i) {
                    for (j = 0; j < ncol1; ++j)
                      *ipnew++ = *ip1++;
                    for (j = 0; j < ncol2; ++j)
                      *ipnew++ = *ip2++;
                  }
                  REL_HEAP;
                  break;

             case TY_REAL:                        /* Wertebereich ist immer  */
                  NEWDESC(_desc); TEST_DESC;      /* eine Matrix             */
                  MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
                  ip1   =    (int *) R_MVT(DESC(arg2),ptdv,class2);
                  rp2   = (double *) R_MVT(DESC(arg3),ptdv,class3);

                  for (i = 0; i < nrows; ++i) {
                    for (j = 0; j < ncol1; ++j)
                      *rpnew++ = (double) *ip1++;
                    for (j = 0; j < ncol2; ++j)
                      *rpnew++ = *rp2++;
                  }
                  REL_HEAP;
                  break;
             default:
                  goto fail;
           }
           break;

      case TY_REAL:
           switch(type3) {

             case TY_INTEGER:                     /* Wertebereich ist immer  */
                  NEWDESC(_desc); TEST_DESC;      /* eine Matrix             */
                  MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
                  rp1   = (double *) R_MVT(DESC(arg2),ptdv,class2);
                  ip2   =    (int *) R_MVT(DESC(arg3),ptdv,class3);

                  for (i = 0; i < nrows; ++i) {
                    for (j = 0; j < ncol1; ++j)
                      *rpnew++ = *rp1++;
                    for (j = 0; j < ncol2; ++j)
                      *rpnew++ = (double) *ip2++;
                  }
                  REL_HEAP;
                  break;

             case TY_REAL:                        /* Wertebereich ist immer  */
                  NEWDESC(_desc); TEST_DESC;      /* eine Matrix             */
                  MVT_DESC_MASK(_desc,1,C_MATRIX,TY_REAL);
                  NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,C_MATRIX));
                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,C_MATRIX);
                  rp1   = (double *) R_MVT(DESC(arg2),ptdv,class2);
                  rp2   = (double *) R_MVT(DESC(arg3),ptdv,class3);

                  for (i = 0; i < nrows; ++i) {
                    for (j = 0; j < ncol1; ++j)
                      *rpnew++ = *rp1++;
                    for (j = 0; j < ncol2; ++j)
                      *rpnew++ = *rp2++;
                  }
                  REL_HEAP;
                  break;

             default:
                  goto fail;
           }
           break;

      default:
           if (type3 != type2)
             goto fail;
           NEWDESC(_desc); TEST_DESC;             /* Wertebereich ist immer  */
           MVT_DESC_MASK(_desc,1,C_MATRIX,type3); /* eine Matrix             */
           NEWHEAP(dim,A_MVT((*_desc),ptdv,C_MATRIX));
           RES_HEAP;
           ipnew = (int *) R_MVT((*_desc),ptdv,C_MATRIX);
           ip1   = (int *) R_MVT(DESC(arg2),ptdv,class2);
           ip2   = (int *) R_MVT(DESC(arg3),ptdv,class3);

           if (type3 == TY_BOOL)
             for (i = 0; i < nrows; ++i) {
               for (j = 0; j < ncol1; ++j)
                 *ipnew++ = *ip1++;
               for (j = 0; j < ncol2; ++j)
                 *ipnew++ = *ip2++;
             }
           else
             for (i = 0; i < nrows; ++i) {
               for (j = 0; j < ncol1; ++j) {
                 *ipnew++ = element = *ip1++;
                 INC_REFCNT((PTR_DESCRIPTOR)element);
               }
               for (j = 0; j < ncol2; ++j) {
                 *ipnew++ = element = *ip2++;
                 INC_REFCNT((PTR_DESCRIPTOR)element);
               }
             }
           REL_HEAP;
    } /** end switch type von arg2 **/

    L_MVT((*_desc),nrows,C_MATRIX) = nrows;       /* Wertebereich ist immer  */
    L_MVT((*_desc),ncols,C_MATRIX) = ncols;       /* eine Matrix             */
    goto success;
  } /** Ende des Falles Koordinate == 2 **/


nilmat:  /* "arg2" ist ein nilmat / nilvect / niltvect */

  switch(class3) {
    case C_MATRIX : if (R_DESC(DESC(arg3),type) == TY_UNDEF) {
                      _desc = (PTR_DESCRIPTOR)arg3;
                      goto success_nilmat;
                    }
                    else {
                      _desc = gen_many_id(C_MATRIX,(PTR_DESCRIPTOR)arg3);
                      TEST_DESC;
                      goto success;
                    }
    case C_VECTOR :
    case C_TVECTOR: if (R_DESC(DESC(arg3),type) == TY_UNDEF) {
                      INC_REFCNT(_nilmat);
                      _desc = _nilmat;
                    }
                    else {
                      _desc = gen_many_id(C_MATRIX,(PTR_DESCRIPTOR)arg3);
                      TEST_DESC;
                    }
                    goto success;
  } /** end switch class3 **/

fail:
  END_MODUL("red_munite");
  DBUG_RETURN(0);
success_nilmat :
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_munite");
  DBUG_RETURN(1);
success:
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  DEC_REFCNT((PTR_DESCRIPTOR)arg3);
  END_MODUL("red_munite");
  DBUG_RETURN(1);
} /**** end of function red_munite ****/

/*****************************************************************************/
/*                            red_transform                                  */
/* ------------------------------------------------------------------------- */
/* function :  transform a matrix or a vector or a tvector  into a list      */
/*                                                                           */
/*             transform :  mat   -->  list                                  */
/*                          mmat  -->  list                                  */
/*                          vect  -->  list                                  */
/*                          tvect -->  list                                  */
/*                                                                           */
/* parameter:  arg1 - a matrix  or a  vector  or a  tvector                  */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_transform(arg1)
register STACKELEM arg1;
{
  PTR_DESCRIPTOR hilf_desc;
  int i,
      dim,                                     /* Dimension des Datenvektors */
      element,
      *ip,*ipnew;
  /* double *rpnew;   RS in June 1993 */
  char  class1;

  DBUG_ENTER ("red_transform");
  START_MODUL("red_transform");

  if (  !T_POINTER(arg1) ||
      (((class1 = R_DESC(DESC(arg1),class)) != C_MATRIX) &&
       (class1                             != C_VECTOR) &&
       (class1                             != C_TVECTOR)))
   goto fail;

  dim = R_MVT(DESC(arg1),nrows,class1) * R_MVT(DESC(arg1),ncols,class1);

  if (dim == 0) {                         /* arg1 ist eine nilmat oder     */
    _desc = (PTR_DESCRIPTOR)NIL;          /* ein nilvect oder ein niltvect */
    goto success;                         /* ==> result: leere Liste       */
  }

  NEWDESC(_desc); TEST_DESC;
  LI_DESC_MASK(_desc,1,C_LIST,TY_UNDEF);
  L_LIST((*_desc),dim) = dim;
  L_LIST((*_desc),special) = LIST;        /* Edit-Feld */
  NEWHEAP(dim,A_LIST((*_desc),ptdv));

  switch(R_DESC(DESC(arg1),type)) {
    case TY_INTEGER:
         for (i = dim; --i >= 0; ) {
           /* changes for lred by RS in June 1993 */
           if (_formated) {
             ((int *) L_LIST((*_desc),ptdv))[i] = TAG_INT(((int *)R_MVT(DESC(arg1),ptdv,class1))[i]);
             }
           else {

           if ((hilf_desc = newdesc()) == 0) longjmp(_interruptbuf,0);
           DESC_MASK(hilf_desc,1,C_SCALAR,TY_INTEGER);
           L_SCALAR((*hilf_desc),vali) =
                                ((int *)R_MVT(DESC(arg1),ptdv,class1))[i];
           ((int *) L_LIST((*_desc),ptdv))[i] = (int) hilf_desc; 

           }
         }
         goto success;
    case TY_REAL:
         for (i = dim; --i >= 0; ) {
           if ((hilf_desc = newdesc()) == 0) longjmp(_interruptbuf,0);
           DESC_MASK(hilf_desc,1,C_SCALAR,TY_REAL);
           L_SCALAR((*hilf_desc),valr) =
                              ((double *)R_MVT(DESC(arg1),ptdv,class1))[i];
           ((int *) L_LIST((*_desc),ptdv))[i] = (int) hilf_desc;
         }
         goto success;
    case TY_BOOL:
         RES_HEAP;
         ip    = (int *) R_MVT(DESC(arg1),ptdv,class1);
         ipnew = (int *) R_LIST((*_desc),ptdv);
         for (i = dim; --i >= 0; )
           ipnew[i] = (ip[i] ? SA_TRUE : SA_FALSE);
         REL_HEAP;
         goto success;
    default:
         RES_HEAP;
         ip = (int *)R_MVT(DESC(arg1),ptdv,class1);
         ipnew = (int *) R_LIST((*_desc),ptdv);
         for (i = dim; --i >= 0; ) {
           ipnew[i] = element = ip[i];
           INC_REFCNT((PTR_DESCRIPTOR)element);
         }
         REL_HEAP;
         goto success;
  } /** Ende der Fallunterscheidung des Typs des Argumentes **/

fail:
  END_MODUL("red_transform");
  DBUG_RETURN(0);

success:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_transform");
    DBUG_RETURN(1);
} /**** end of function red_transform ****/

/*****************************************************************************/
/*                               red_transpose                               */
/* ------------------------------------------------------------------------- */
/* function :  transpose a matrix  or  a vector   or  a tvector              */
/*                                                                           */
/*             transpose  :   mat   -->  mat                                 */
/*                            mmat  -->  mmat                                */
/*                            vect  -->  tvect                               */
/*                            tvect -->  vect                                */
/*                                                                           */
/* parameter:  arg1 - a matrix  or a  vector  or a  tvector                  */
/* returns  :  stackelement  (pointer to result descriptor)                  */
/*****************************************************************************/

int red_transpose(arg1)
register STACKELEM arg1;
{
  register int i,j,dim;
  int nrows,
      ncols,
      type,
      *ip,*ipnew;
  double *rp,*rpnew;
  char class1;

  DBUG_ENTER ("red_transpose");
  START_MODUL("red_transpose");

  if (  !T_POINTER(arg1) ||
      (((class1 = R_DESC(DESC(arg1),class)) != C_MATRIX) &&
       (class1                             != C_VECTOR) &&
       (class1                             != C_TVECTOR)))
    goto fail;

  nrows = R_MVT(DESC(arg1),nrows,class1);

  if (nrows == 0) {                         /* nilmat or nilvect or niltvect */
    switch(class1) {
      case C_MATRIX  : _desc = (PTR_DESCRIPTOR) arg1;
                       END_MODUL("red_transpose");
                       DBUG_RETURN(1);
      case C_VECTOR  : _desc = (PTR_DESCRIPTOR) _niltvect;
                       INC_REFCNT(_niltvect);
                       goto success;
      case C_TVECTOR : _desc = (PTR_DESCRIPTOR) _nilvect;
                       INC_REFCNT(_nilvect);
                       goto success;
    } /** switch class1 **/
  } /** end of nrows **/

  ncols = R_MVT(DESC(arg1),ncols,class1);

  if ((nrows == 1) || (ncols == 1)) {          /*----------------------------*/
                                               /* (1xm)-mat   /  (nx1)-mat   */
                                               /* vector      /  tvector     */
                                               /*----------------------------*/
    switch(class1) {
                     /*--------- create a new indirect descriptor --------*/
                     /* (1xm)-mat --> (nx1)-mat / (nx1)-mat --> (1xm)-mat */
      case C_MATRIX : _desc = gen_many_id(C_MATRIX,(PTR_DESCRIPTOR)arg1);
                      TEST_DESC;
                     /*--------- now transpose the matrix ----------------*/
                      L_MVT((*_desc),nrows,C_MATRIX) = ncols;
                      L_MVT((*_desc),ncols,C_MATRIX) = nrows;
                      break;

                     /*--------- create a new indirect descriptor --------*/
                     /*              vect  --> tvect                      */
      case C_VECTOR : _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR)arg1);
                      TEST_DESC;
                      break;

                     /*--------- create a new indirect descriptor --------*/
                     /*              tvect --> vect                       */
      case C_TVECTOR: _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR)arg1);
                      TEST_DESC;
                      break;
    } /** switch class1 **/
    goto success;
  } /** (nrows == 1  ||  ncols == 1) **/
                                               /*----------------------------*/
  else {                                       /* nrows > 1 and ncols > 1    */
                                               /*============================*/
                                               /* transpose a (nxm)-mat      */
#if DEBUG                                      /* (nxm)-mmat  with n,m > 1   */
    if (R_DESC(DESC(arg1),class) != C_MATRIX)  /*----------------------------*/
      post_mortem("red_transpose: inconsistent descriptor class");
#endif

    NEWDESC(_desc); TEST_DESC;
    dim = nrows * ncols;

    if ((type = R_DESC(DESC(arg1),type)) == TY_REAL) {
      MVT_DESC_MASK(_desc,1,class1,TY_REAL);
      NEWHEAP(dim<<1,A_MVT((*_desc),ptdv,class1));
      RES_HEAP;
      rpnew = (double *)R_MVT((*_desc),ptdv,class1);
      rp    = (double *)R_MVT(DESC(arg1),ptdv,class1);

      for (j = 0; j < ncols; ++j) {
        for (i = 0; i < dim; i+=ncols)
          *rpnew++ = rp[i];
        rp++;
      }
    }
    else {   /* type != TY_REAL */
      MVT_DESC_MASK(_desc,1,class1,type);
      NEWHEAP(dim,A_MVT((*_desc),ptdv,class1));
      RES_HEAP;
      ipnew = (int *) R_MVT((*_desc),ptdv,class1);
      ip = (int *) R_MVT(DESC(arg1),ptdv,class1);

      if (type < TY_STRING)
        for (j = 0; j < ncols; ++j) {
          for (i = 0; i < dim; i+=ncols)
            *ipnew++ = ip[i];
         ip++;
        }
      else /* type >= TY_STRING */
        for (j = 0; j < ncols; ++j) {
          for(i = 0; i < dim; i+=ncols) {
            *ipnew++ = ip[i];
            INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
          }
          ip++;
        }
    } /** Ende von Type != TY_REAL **/
    REL_HEAP;
    L_MVT((*_desc),nrows,class1) = ncols;
    L_MVT((*_desc),ncols,class1) = nrows;
    goto success;
  } /** Ende von nrows > 1 und ncols > 1 **/

fail:
  END_MODUL("red_transpose");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
  END_MODUL("red_transpose");
  DBUG_RETURN(1);
}

/*****************************************************************************/
/*                  red_cut       (universelle cut Funktion)                 */
/* ------------------------------------------------------------------------- */
/* function :  Programmverteiler auf die einzelnen speziellen cut-Funktionen */
/*             parameter :                                      calls :      */
/*      cut :  0       x  Z       x  list      ==>  list        red_lcut     */
/*             0       x  Z       x  string    ==>  string      red_lcut     */
/*             0       x  Z       x  vect      ==>  vect        red_vcut     */
/*             0       x  Z       x  tvect     ==>  tvect       red_vcut     */
/*             Z       x  Z       x  mmat      ==>  mmat        red_mcut     */
/*             Z       x  Z       x  mat       ==>  mat         red_mcut     */
/*             Z       x  0       x  mat       ==>  mat         red_mcut     */
/*             0       x  Z       x  mat       ==>  mat         red_mcut     */
/*                                                                           */
/* returns  :  '1' if reduction was successfull     else  '0'                */
/* called by:  rear.c                                                        */
/*****************************************************************************/

int red_cut(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int par1;                                 /* structuring parameter of arg1 */

  DBUG_ENTER ("red_cut");
  START_MODUL("red_cut");
  /* changes for lred by RS in June 1993 */
  if (((!T_POINTER(arg1)) && !T_INT(arg1)) || ((!T_POINTER(arg2)) && !T_INT(arg2)) || (!T_POINTER(arg3)))
    goto fail;

  /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    par1 = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,par1);

  switch(R_DESC(DESC(arg3),class)) {
     case C_LIST   : if (par1 != 0) goto fail;
                     if (red_lcut(arg2,arg3) == 0)
                      goto fail;
                     goto decarg1;
     case C_VECTOR :
     case C_TVECTOR: if (par1 != 0) goto fail;
                     if (red_vcut(arg2,arg3) == 0)
                       goto fail;
                     goto decarg1;
     case C_MATRIX : if (red_mcut(arg1,arg2,arg3) == 0)
                       goto fail;
                     else
                       goto success;
     default       : goto fail;
  } /** end switch class arg3 **/

decarg1 :
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
success :
  END_MODUL("red_cut");
  DBUG_RETURN(1);
fail    :
  END_MODUL("red_cut");
  DBUG_RETURN(0);
} /**** end of function red_cut ****/

/*****************************************************************************/
/*                  red_select    (universelle select Funktion)              */
/* ------------------------------------------------------------------------- */
/* function :  Programmverteiler auf die einzelnen speziellen                */
/*             select-Funktionen                                             */
/*             parameter :                                      calls :      */
/*   select :  0   x   N    x   list     ==>  Object            red_lselect  */
/*             0   x   N    x   string   ==>  string            red_lselect  */
/*             0   x   N    x   vect     ==>  Scalar / string   red_vselect  */
/*             0   x   N    x   tvect    ==>  Scalar / string   red_vselect  */
/*             N   x   N    x   mmat     ==>  mat / mmat        red_mselect  */
/*             N   x   N    x   mat      ==>  Scalar / string   red_mselect  */
/*             N   x   0    x   mat      ==>  vect              red_mselect  */
/*             0   x   N    x   mat      ==>  tvect             red_mselect  */
/*                                                                           */
/* returns  :  '1' if reduction was successfull     else  '0'                */
/* called by:  rear.c                                                        */
/*****************************************************************************/

int red_select(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int par1;                                /* structutring parameter of arg1 */

  DBUG_ENTER("red_select");
  START_MODUL("red_select");

  /* changes for lred by RS in June 1993 */
  if (((!T_POINTER(arg1)) && !T_INT(arg1)) || ((!T_POINTER(arg2)) && !T_INT(arg2)) || (!T_POINTER(arg3)))
    goto fail;

  /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    par1 = VAL_INT (arg1);
  else
    STRUCT_ARG(arg1,par1);

  switch(R_DESC(DESC(arg3),class)) {
     case C_LIST   : if (par1 != 0) goto fail;
                     if (red_lselect(arg2,arg3) == 0)
                       goto fail;
                     goto decarg1;
     case C_VECTOR :
     case C_TVECTOR: if (par1 != 0) goto fail;
                     if (red_vselect(arg2,arg3) == 0)
                       goto fail;
                     goto decarg1;
     case C_MATRIX : if (red_mselect(arg1,arg2,arg3) == 0)
                       goto fail;
                     else
                       goto success;
     default       : goto fail;
  } /** end switch class arg3 **/

fail    :
  END_MODUL("red_select");
  DBUG_RETURN(0);
decarg1 :
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
success :
  END_MODUL("red_select");
  DBUG_RETURN(2);
} /**** end of function red_select ****/

/*****************************************************************************/
/*                  red_replace   (universelle replace Funktion)             */
/* ------------------------------------------------------------------------- */
/* function :  Programmverteiler auf die einzelnen speziellen                */
/*             replace-Funktionen                                            */
/*             parameter :                                      calls :      */
/*   replace:  0  x  N   x  Object  x  list     ==>  list       red_repstr   */
/*             0  x  N   x  string  x  string   ==>  string     red_repstr   */
/*             0  x  N   x  Scalar  x  vect     ==>  vect       red_vreplace */
/*             0  x  N   x  Scalar  x  tvect    ==>  tvect      red_vreplace */
/*             N  x  N   x  mat     x  mmat     ==>  mmat       red_mreplace */
/*             N  x  N   x  Scalar  x  mat      ==>  mat        red_mreplace */
/*             0  x  N   x  tvect   x  mat      ==>  mat        red_mre_c    */
/*             0  x  N   x  vect    x  mat      ==>  mat        red_mre_r    */
/*                                                                           */
/* returns  :  '1' if reduction was successfull     else  '0'                */
/* called by:  rear.c                                                        */
/*****************************************************************************/

int red_replace(arg1,arg2,arg3,arg4)
register STACKELEM arg1,arg2,arg3,arg4;
{
  int par1;                                /* structutring parameter of arg1 */

  DBUG_ENTER ("red_replace");
  START_MODUL("red_replace");

  if T_CON(arg3)  goto fail;               /* Kommentar wie bei red_repstr   */
                                           /* in file rlstruct.c             */
  /* changes for lred by RS in June 1993 */
  if (((!T_POINTER(arg1)) && !T_INT(arg1)) || ((!T_POINTER(arg2)) && !T_INT(arg2)) || (!T_POINTER(arg4)))
    goto fail;

  /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    par1 = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,par1);

  switch(R_DESC(DESC(arg4),class)) {
     case C_LIST   : if (par1 != 0) goto fail;
                     if (red_lreplace(arg2,arg3,arg4) == 0)
                       goto fail;
                     goto decarg1;
     case C_VECTOR :
     case C_TVECTOR: if (par1 != 0) goto fail;
                     if (red_vreplace(arg2,arg3,arg4) == 0)
                       goto fail;
                     goto decarg1;
     case C_MATRIX : if (T_BOOLEAN(arg3)) {
                       if (red_mreplace(arg1,arg2,arg3,arg4) == 0)
                         goto fail;
                       goto success;
                     }
                     else {
                       /* changes for lred by RS in June 1993 */
                       if (T_INT(arg3)) {
                         if (red_mreplace(arg1,arg2,arg3,arg4) == 0)
                           goto fail;
                         goto success; 
                         } 
                       if (!T_POINTER(arg3))
                         goto fail;
                       switch(R_DESC(DESC(arg3),class)) {
                         case C_VECTOR : if (par1 != 0) goto fail;
                                         if (red_mre_r(arg2,arg3,arg4) == 0)
                                           goto fail;
                                         goto decarg1;
                         case C_TVECTOR: if (par1 != 0) goto fail;
                                         if (red_mre_c(arg2,arg3,arg4) == 0)
                                           goto fail;
                                         goto decarg1;
                         case C_LIST   :
                         case C_SCALAR :
                         case C_DIGIT  :
                         case C_MATRIX : if (red_mreplace(arg1,arg2,arg3,arg4) == 0)
                                           goto fail;
                                         goto success;
                         default       : goto fail;
                       } /** end switch class arg3 **/
                     }
     default       : goto fail;
  } /** end switch class arg4 **/

fail    :
  END_MODUL("red_replace");
  DBUG_RETURN(0);
decarg1 :
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
success :
  END_MODUL("red_replace");
  DBUG_RETURN(1);
} /**** end of function red_replace ****/

/*****************************************************************************/
/*                  red_rotate    (universelle rotate Funktion)              */
/* ------------------------------------------------------------------------- */
/* function :  Programmverteiler auf die einzelnen speziellen                */
/*             rotate-Funktionen                                             */
/*             parameter :                                      calls :      */
/*   rotate :  0  x  Z  x  list    ==>  list                    red_lrotate  */
/*             0  x  Z  x  string  ==>  string                  red_lrotate  */
/*             0  x  Z  x  vect    ==>  vect                    red_vrotate  */
/*             0  x  Z  x  tvect   ==>  tvect                   red_vrotate  */
/*             Z  x  Z  x  mmat    ==>  mmat                    red_mrotate  */
/*             Z  x  Z  x  mat     ==>  mat                     red_mrotate  */
/*                                                                           */
/* returns  :  '1' if reduction was successfull     else  '0'                */
/* called by:  rear.c                                                        */
/*****************************************************************************/

int red_rotate(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int par1;                                /* structutring parameter of arg1 */

  DBUG_ENTER ("red_rotate");
  START_MODUL("red_rotate");

  /* changes for lred by RS in June 1993 */
  if (((!T_POINTER(arg1)) && !T_INT(arg1)) || ((!T_POINTER(arg2)) && !T_INT(arg2)) || (!T_POINTER(arg3)))
    goto fail;

  /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    par1 = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,par1);

  switch(R_DESC(DESC(arg3),class)) {
     case C_LIST   : if (par1 != 0) goto fail;
                     if (red_lrotate(arg2,arg3) == 0)
                       goto fail;
                     goto decarg1;
     case C_VECTOR :
     case C_TVECTOR: if (par1 != 0) goto fail;
                     if (red_vrotate(arg2,arg3) == 0)
                       goto fail;
                     goto decarg1;
     case C_MATRIX : if (red_mrotate(arg1,arg2,arg3) == 0)
                       goto fail;
                     else
                       goto success;
     default       : goto fail;
  } /** end switch class arg3 **/

fail    :
  END_MODUL("red_rotate");
  DBUG_RETURN(0);
decarg1 :
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
success :
  END_MODUL("red_rotate");
  DBUG_RETURN(1);
} /**** end of function red_rotate ****/

/*****************************************************************************/
/*                  red_unite     (universelle unite Funktion)               */
/* ------------------------------------------------------------------------- */
/* function :  Programmverteiler auf die einzelnen speziellen                */
/*             unite-Funktionen                                              */
/*             parameter :                                      calls :      */
/*    unite :  0       x  list    x  list      ==>  list        red_lunite   */
/*             0       x  string  x  string    ==>  string      red_lunite   */
/*             0       x  vect    x  vect      ==>  vect        red_vunite   */
/*             0       x  tvect   x  tvect     ==>  tvect       red_vunite   */
/*             {1,2}   x  mmat    x  mmat      ==>  mmat        red_munite   */
/*             {1,2}   x  mat     x  mat       ==>  mat         red_munite   */
/*             {1,2}   x  mat     x  vect      ==>  mat         red_munite   */
/*             {1,2}   x  mat     x  tvect     ==>  mat         red_munite   */
/*             {1,2}   x  vect    x  mat       ==>  mat         red_munite   */
/*             {1,2}   x  vect    x  vect      ==>  mat         red_munite   */
/*             {1,2}   x  vect    x  tvect     ==>  mat         red_munite   */
/*             {1,2}   x  tvect   x  mat       ==>  mat         red_munite   */
/*             {1,2}   x  tvect   x  tvect     ==>  mat         red_munite   */
/*             {1,2}   x  tvect   x  vect      ==>  mat         red_munite   */
/*                                                                           */
/* returns  :  '1' if reduction was successfull     else  '0'                */
/* called by:  rear.c                                                        */
/*****************************************************************************/

int red_unite(arg1,arg2,arg3)
register STACKELEM arg1,arg2,arg3;
{
  int par1;                                /* structutring parameter of arg1 */
  int class2, class3;                      /* class of arg2 and arg3         */

  DBUG_ENTER ("red_unite");
  START_MODUL("red_unite");

  /* changes for lred by RS in June 1993 */
  if (!T_POINTER(arg1) && !T_INT(arg1)) goto fail;

  /* changes for lred by RS in June 1993 */
  if (T_INT(arg1))
    par1 = VAL_INT(arg1);
  else
    STRUCT_ARG(arg1,par1);

  if (T_POINTER(arg2) && T_POINTER(arg3)) {
    class2 = R_DESC(DESC(arg3),class);
    class3 = R_DESC(DESC(arg3),class);
    if (class2 == class3) {
      switch(class3) {
         case C_LIST   : if (par1 != 0) goto fail;
                         if (red_lunite(arg2,arg3) == 0)
                           goto fail;
                         goto decarg1;
         case C_VECTOR :
         case C_TVECTOR: if (par1 == 0) {
                          if (red_vunite(arg2,arg3) == 0)
                            goto fail;
                          goto decarg1;
                        }
         case C_MATRIX : if (red_munite(arg1,arg2,arg3) == 0)
                           goto fail;
                         goto success;
         default       : goto fail;
      } /** end switch class3 **/
    } /** end if class2 == class3 **/
    else {
      switch(class3) {
        case C_VECTOR  :
        case C_TVECTOR :
        case C_MATRIX  : switch(class2) {
                           case C_VECTOR  :
                           case C_TVECTOR :
                           case C_MATRIX  : if (red_munite(arg1,arg2,arg3) == 0)
                                             goto fail;
                                            goto success;
                           default        : goto fail;
                         } /** end switch class2 **/
        default        : goto fail;
      } /** end switch class3 **/
    } /** end else **/
  } /** end T_POINTER of arg2 and arg3 **/

fail    :
  END_MODUL("red_unite");
  DBUG_RETURN(0);
decarg1 :
  /* changes for lred by RS in June 1993 */
  if (T_POINTER(arg1))
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
success :
  END_MODUL("red_unite");
  DBUG_RETURN(1);
} /**** end of function red_unite ****/

/*************************  end of file rmstruct.c  **************************/
