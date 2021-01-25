#define WITHTILDE 1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dbug.h"
#include "rstdinc.h"
#include "rstelem.h"
#include "rheapty.h"
#include "rstackty.h"
extern PTR_DESCRIPTOR _desc; 
extern PTR_DESCRIPTOR _nil; 
extern PTR_DESCRIPTOR _nilstring;
extern PTR_DESCRIPTOR _nilmat;
extern PTR_DESCRIPTOR _nilvect;
extern PTR_DESCRIPTOR _niltvect;

int newdesc(), post_mortem(), newheap(), controlled_exit();

PTR_DESCRIPTOR mvt_desc;
  
extern int AsciiOutput, KiROutput, withtilde, number_of_descs;
#define DESC(x)  (* (PTR_DESCRIPTOR) x)
/************************************************************/
/* elementwise multiplication of two mvt if dimensions fit  */
/************************************************************/
T_PTD t_mvt_mvt_mult(T_PTD a, T_PTD b, int typ)
{
  int *amvt, *bmvt;
  double *armvt, *brmvt, *new_rmvt;
  int arows, acols, brows, bcols, i, cl;

  DBUG_ENTER("mvt_mvt_mult");

  cl = R_DESC(DESC(a), class);
  arows = R_MVT(DESC(a), nrows, cl);
  acols = R_MVT(DESC(a), ncols, cl);
  brows = R_MVT(DESC(b), nrows, cl);
  bcols = R_MVT(DESC(b), ncols, cl);

  if ((arows != brows) || (acols != bcols))
    controlled_exit("dimension of mat/vect/tvect don't fit in mult");
  else
  {
    MAKEDESC(mvt_desc, 1, cl, TY_REAL);
    L_MVT((*mvt_desc), ptdd, cl) = NULL;
    L_MVT(*mvt_desc, nrows, cl) = arows;
    L_MVT(*mvt_desc, ncols, cl) = acols;
    switch(typ)
    {
      case 0:
        /* int x int */
        GET_HEAP(arows * acols, A_MVT(*mvt_desc, ptdv, cl));
        amvt = R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        for (i = 0; i < arows * acols; i++)
        {
          L_MVT(*mvt_desc, ptdv, cl)[i] = amvt[i] * bmvt[i];
        }
        L_MVT(*mvt_desc, type, cl) = TY_INTEGER;
        break;
      case 1:
        /* int x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        amvt = (int *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = (double)amvt[i] * brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 2:
        /* real x int */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = armvt[i] * (double)bmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 3:
        /* real x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = armvt[i] * brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      default:
        controlled_exit("undefined type constellation in mvt_mvt_mult");
        break;
    }
  }
  DBUG_RETURN(mvt_desc);
}


/******************************************************/
/* elementwise addition of two mvt if dimensions fit  */
/******************************************************/
T_PTD t_mvt_mvt_plus(T_PTD a, T_PTD b, int typ)
{
  int *amvt, *bmvt;
  double *armvt, *brmvt, *new_rmvt;
  int arows, acols, brows, bcols, i, cl;

  DBUG_ENTER("mvt_mvt_plus");

  cl = R_DESC(DESC(a), class);
  arows = R_MVT(DESC(a), nrows, cl);
  acols = R_MVT(DESC(a), ncols, cl);
  brows = R_MVT(DESC(b), nrows, cl);
  bcols = R_MVT(DESC(b), ncols, cl);

  if ((arows != brows) || (acols != bcols))
    controlled_exit("dimension of mat/vect/tvect don't fit in mult");
  else
  {
    MAKEDESC(mvt_desc, 1, cl, TY_REAL);
    L_MVT((*mvt_desc), ptdd, cl) = NULL;
    L_MVT(*mvt_desc, nrows, cl) = arows;
    L_MVT(*mvt_desc, ncols, cl) = acols;
    switch(typ)
    {
      case 0:
        /* int x int */
        GET_HEAP(arows * acols, A_MVT(*mvt_desc, ptdv, cl));
        amvt = R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        for (i = 0; i < arows * acols; i++)
        {
          L_MVT(*mvt_desc, ptdv, cl)[i] = amvt[i] + bmvt[i];
        }
        L_MVT(*mvt_desc, type, cl) = TY_INTEGER;
        break;
      case 1:
        /* int x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        amvt = (int *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = (double)amvt[i] + brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 2:
        /* real x int */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = armvt[i] + (double)bmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 3:
        /* real x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = armvt[i] + brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      default:
        controlled_exit("undefined type constellation in mvt_mvt_mult");
        break;
    }
  }
  DBUG_RETURN(mvt_desc);
}


/*********************************************************/
/* elementwise subtraction of two mvt if dimensions fit  */
/*********************************************************/
T_PTD t_mvt_mvt_minus(T_PTD a, T_PTD b, int typ)
{
  int *amvt, *bmvt;
  double *armvt, *brmvt, *new_rmvt;
  int arows, acols, brows, bcols, i, cl;

  DBUG_ENTER("mvt_mvt_plus");

  cl = R_DESC(DESC(a), class);
  arows = R_MVT(DESC(a), nrows, cl);
  acols = R_MVT(DESC(a), ncols, cl);
  brows = R_MVT(DESC(b), nrows, cl);
  bcols = R_MVT(DESC(b), ncols, cl);

  if ((arows != brows) || (acols != bcols))
    controlled_exit("dimension of mat/vect/tvect don't fit in mult");
  else
  {
    MAKEDESC(mvt_desc, 1, cl, TY_REAL);
    L_MVT((*mvt_desc), ptdd, cl) = NULL;
    L_MVT(*mvt_desc, nrows, cl) = arows;
    L_MVT(*mvt_desc, ncols, cl) = acols;
    switch(typ)
    {
      case 0:
        /* int x int */
        GET_HEAP(arows * acols, A_MVT(*mvt_desc, ptdv, cl));
        amvt = R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        for (i = 0; i < arows * acols; i++)
        {
          L_MVT(*mvt_desc, ptdv, cl)[i] = amvt[i] - bmvt[i];
        }
        L_MVT(*mvt_desc, type, cl) = TY_INTEGER;
        break;
      case 1:
        /* int x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        amvt = (int *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = (double)amvt[i] - brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 2:
        /* real x int */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = armvt[i] - (double)bmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 3:
        /* real x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = armvt[i] - brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      default:
        controlled_exit("undefined type constellation in mvt_mvt_mult");
        break;
    }
  }
  DBUG_RETURN(mvt_desc);
}


/******************************************************/
/* elementwise division of two mvt if dimensions fit  */
/******************************************************/
T_PTD t_mvt_mvt_div(T_PTD a, T_PTD b, int typ)
{
  int *amvt, *bmvt;
  double *armvt, *brmvt, *new_rmvt;
  int arows, acols, brows, bcols, i, cl;

  DBUG_ENTER("mvt_mvt_plus");

  cl = R_DESC(DESC(a), class);
  arows = R_MVT(DESC(a), nrows, cl);
  acols = R_MVT(DESC(a), ncols, cl);
  brows = R_MVT(DESC(b), nrows, cl);
  bcols = R_MVT(DESC(b), ncols, cl);

  if ((arows != brows) || (acols != bcols))
    controlled_exit("dimension of mat/vect/tvect don't fit in mult");
  else
  {
    MAKEDESC(mvt_desc, 1, cl, TY_REAL);
    L_MVT((*mvt_desc), ptdd, cl) = NULL;
    L_MVT(*mvt_desc, nrows, cl) = arows;
    L_MVT(*mvt_desc, ncols, cl) = acols;
    switch(typ)
    {
      case 0:
        /* int x int */
        GET_HEAP(arows * acols, A_MVT(*mvt_desc, ptdv, cl));
        amvt = R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        for (i = 0; i < arows * acols; i++)
        {
          L_MVT(*mvt_desc, ptdv, cl)[i] = amvt[i] / bmvt[i];
        }
        L_MVT(*mvt_desc, type, cl) = TY_INTEGER;
        break;
      case 1:
        /* int x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        amvt = (int *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = (double)amvt[i] / brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 2:
        /* real x int */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = armvt[i] / (double)bmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 3:
        /* real x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          new_rmvt[i] = armvt[i] / brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      default:
        controlled_exit("undefined type constellation in mvt_mvt_mult");
        break;
    }
  }
  DBUG_RETURN(mvt_desc);
}



/****************************************************/
/* elementwise modulo of two mvt if dimensions fit  */
/****************************************************/
T_PTD t_mvt_mvt_mod(T_PTD a, T_PTD b, int typ)
{
  int *amvt, *bmvt;
  double *armvt, *brmvt, *new_rmvt;
  int arows, acols, brows, bcols, i, cl, help;

  DBUG_ENTER("mvt_mvt_plus");

  cl = R_DESC(DESC(a), class);
  arows = R_MVT(DESC(a), nrows, cl);
  acols = R_MVT(DESC(a), ncols, cl);
  brows = R_MVT(DESC(b), nrows, cl);
  bcols = R_MVT(DESC(b), ncols, cl);

  if ((arows != brows) || (acols != bcols))
    controlled_exit("dimension of mat/vect/tvect don't fit in mult");
  else
  {
    MAKEDESC(mvt_desc, 1, cl, TY_REAL);
    L_MVT((*mvt_desc), ptdd, cl) = NULL;
    L_MVT(*mvt_desc, nrows, cl) = arows;
    L_MVT(*mvt_desc, ncols, cl) = acols;
    switch(typ)
    {
      case 0:
        /* int x int */
        GET_HEAP(arows * acols, A_MVT(*mvt_desc, ptdv, cl));
        amvt = R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        for (i = 0; i < arows * acols; i++)
        {
          L_MVT(*mvt_desc, ptdv, cl)[i] = amvt[i] % bmvt[i];
        }
        L_MVT(*mvt_desc, type, cl) = TY_INTEGER;
        break;
      case 1:
        /* int x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        amvt = (int *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          help = (double)amvt[i] / brmvt[i];
          new_rmvt[i] = (double)amvt[i] - help * brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 2:
        /* real x int */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        bmvt = R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          help = amvt[i] / (double)brmvt[i];
          new_rmvt[i] = amvt[i] - help * (double)brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      case 3:
        /* real x real */
        GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, cl));
        new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, cl);
        armvt = (double *)R_MVT(DESC(a), ptdv, cl);
        brmvt = (double *)R_MVT(DESC(b), ptdv, cl);
        RES_HEAP;
        for (i = 0; i < arows * acols; i++)
        {
          help = amvt[i] / brmvt[i];
          new_rmvt[i] = amvt[i] - help * brmvt[i];
        }
        L_MVT(*mvt_desc, ptdv, cl) = (PTR_HEAPELEM)new_rmvt; 
        REL_HEAP;
        break;
      default:
        controlled_exit("undefined type constellation in mvt_mvt_mult");
        break;
    }
  }
  DBUG_RETURN(mvt_desc);
}

/********************************/
/*    VECTOR ip TVECTOR => INT  */
/********************************/
int t_v_t_ip_int(T_PTD a, T_PTD b, int typ)
{
  int ires = 0, n;
  DBUG_ENTER("t_v_t_ip_int");
  if (R_MVT(DESC(a), ncols, C_VECTOR) != R_MVT(DESC(b), nrows, C_TVECTOR))
    controlled_exit("incompatible (T)VECTORs in ip");
  n = R_MVT(DESC(b), nrows, C_TVECTOR);
  while (n > 0)
  {
    n--;
    ires += R_MVT(DESC(a), ptdv, C_TVECTOR)[n] 
          * R_MVT(DESC(b), ptdv, C_TVECTOR)[n];
  }
  DBUG_RETURN(TAG_INT(ires));
}

/********************************/
/*    VECTOR ip TVECTOR => REAL */
/********************************/
double t_v_t_ip_real(T_PTD a, T_PTD b, int typ)
{
  double *armvt, *brmvt, dres;
  int n;
  
  DBUG_ENTER("t_v_t_ip_real");
  if (R_MVT(DESC(a), ncols, C_VECTOR) != R_MVT(DESC(b), nrows, C_TVECTOR))
    controlled_exit("incompatible (T)VECTORs in ip");
  else
  {
    n = R_MVT(DESC(b), nrows, C_TVECTOR);
    switch(typ)
    {
      case 1:
        brmvt = (double *)R_MVT(DESC(b), ptdv, C_TVECTOR);
        dres = 0;
        while (n > 0)
        {
          n--;
          dres += (double)R_MVT(DESC(a), ptdv, C_TVECTOR)[n] * brmvt[n];
        }
        break;
      case 2:
        dres = 0;
        armvt = (double *)R_MVT(DESC(a), ptdv, C_TVECTOR);
        while (n > 0)
        {
          n--;
          dres += (double)R_MVT(DESC(b), ptdv, C_TVECTOR)[n] * armvt[n];
        }
        break;
      case 3:
        armvt = (double *)R_MVT(DESC(a), ptdv, C_TVECTOR);
        brmvt = (double *)R_MVT(DESC(b), ptdv, C_TVECTOR);
        dres = 0;
        while (n > 0)
        { 
          n--;
          dres += brmvt[n] * armvt[n];
        }
        break;
      default:
        controlled_exit("undefined type constellation in TVECTOR ip C__TVECTOR");
    }
  }
  DBUG_RETURN(dres);
}

/********************************************/
/* (T)VECTOR ip (T)VECTOR                   */
/* works only if dimensions of vectors == 1 */
/* not yet supported                        */
/********************************************/
T_PTD t_tv_tv_ip(T_PTD a, T_PTD b, int typ)
{
  double *armvt, *brmvt;
  
  DBUG_ENTER("t_tv_tv_ip");
  if ((1 != R_MVT(DESC(a), nrows, C_TVECTOR)) || (1 != R_MVT(DESC(b), nrows, C_TVECTOR)))
    controlled_exit("incompatible (T)VECTORs in ip");
  if ((1 != R_MVT(DESC(a), ncols, C_VECTOR)) || (1 != R_MVT(DESC(b), ncols, C_VECTOR)))
    controlled_exit("incompatible (T)VECTORs in ip");
      
  else
  {
    switch(typ)
    {
      case 0:
        mvt_desc = (T_PTD)(R_MVT(DESC(a), ptdv, C_VECTOR)[0] 
                         * R_MVT(DESC(b), ptdv, C_VECTOR)[0]);
        mvt_desc = (T_PTD)TAG_INT((int)mvt_desc);
        break;
      case 1:
        MAKEDESC(mvt_desc, 1, C_SCALAR, TY_REAL);
        brmvt = (double *)R_MVT(DESC(b), ptdv, C_VECTOR);
        L_SCALAR(*mvt_desc, valr) = (double)R_MVT(DESC(a), ptdv, C_VECTOR)[0] * brmvt[0];
        break;
      case 2:
        MAKEDESC(mvt_desc, 1, C_SCALAR, TY_REAL);
        armvt = (double *)R_MVT(DESC(a), ptdv, C_VECTOR);
        L_SCALAR(*mvt_desc, valr) = (double)R_MVT(DESC(b), ptdv, C_VECTOR)[0] * armvt[0];
        break;
      case 3:
        MAKEDESC(mvt_desc, 1, C_SCALAR, TY_REAL);
        armvt = (double *)R_MVT(DESC(a), ptdv, C_VECTOR);
        brmvt = (double *)R_MVT(DESC(b), ptdv, C_VECTOR);
        L_SCALAR(*mvt_desc, valr) = brmvt[0] * armvt[0];
        break;
      default:
        controlled_exit("undefined type constellation in (TVECTOR ip TVECTOR)");
    }
  }
  DBUG_RETURN(mvt_desc);
}


T_PTD t_mvt_mvt_ip(T_PTD a, T_PTD b, int typ, int res_class)
{
  int *amvt, *bmvt;
  double *armvt, *brmvt, *new_rmvt;
  int arows, acols, brows, bcols, i, class_a, class_b;

  DBUG_ENTER("mvt_mvt_ip");

  class_a = R_DESC(DESC(a), class);
  class_b = R_DESC(DESC(b), class);
  arows = R_MVT(DESC(a), nrows, class_a);
  acols = R_MVT(DESC(a), ncols, class_a);
  brows = R_MVT(DESC(b), nrows, class_b);
  bcols = R_MVT(DESC(b), ncols, class_b);
  if (acols != brows)
    controlled_exit("dimension of mat/vect/tvect don't fit in mult");
  switch(typ)
  {
    case 0:
      /* int x int */
      MAKEDESC(mvt_desc, 1, res_class, TY_INTEGER);
      L_MVT(*mvt_desc, ptdd, res_class) = NULL;
      GET_HEAP(arows * acols, A_MVT(*mvt_desc, ptdv, res_class));
      amvt = R_MVT(DESC(a), ptdv, class_a);
      bmvt = R_MVT(DESC(b), ptdv, class_b);
      for (i = 0; i < arows * acols; i++)
      {
        L_MVT(*mvt_desc, ptdv, res_class)[i] = amvt[i] * bmvt[i];
      }
      break;
    case 1:
      /* int x real */
      MAKEDESC(mvt_desc, 1, res_class, TY_REAL);
      L_MVT(*mvt_desc, ptdd, res_class) = NULL;
      GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, res_class));
      new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, res_class);
      amvt = (int *)R_MVT(DESC(a), ptdv, class_a);
      brmvt = (double *)R_MVT(DESC(b), ptdv, class_b);
      RES_HEAP;
      for (i = 0; i < arows * acols; i++)
      {
        new_rmvt[i] = (double)amvt[i] * brmvt[i];
      }
      L_MVT(*mvt_desc, ptdv, res_class) = (PTR_HEAPELEM)new_rmvt; 
      REL_HEAP;
      break;
    case 2:
      /* real x int */
      MAKEDESC(mvt_desc, 1, res_class, TY_REAL);
      L_MVT(*mvt_desc, ptdd, res_class) = NULL;
      GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, res_class));
      new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, res_class);
      armvt = (double *)R_MVT(DESC(a), ptdv, class_a);
      bmvt = R_MVT(DESC(b), ptdv, class_b);
      RES_HEAP;
      for (i = 0; i < arows * acols; i++)
      {
        new_rmvt[i] = armvt[i] * (double)bmvt[i];
      }
      L_MVT(*mvt_desc, ptdv, res_class) = (PTR_HEAPELEM)new_rmvt; 
      REL_HEAP;
      break;
    case 3:
      /* real x real */
      MAKEDESC(mvt_desc, 1, res_class, TY_REAL);
      L_MVT(*mvt_desc, ptdd, res_class) = NULL;
      GET_HEAP(arows * acols * sizeof(double), A_MVT(*mvt_desc, ptdv, res_class));
      new_rmvt = (double *)R_MVT(*mvt_desc, ptdv, res_class);
      armvt = (double *)R_MVT(DESC(a), ptdv, class_a);
      brmvt = (double *)R_MVT(DESC(b), ptdv, class_b);
      RES_HEAP;
      for (i = 0; i < arows * acols; i++)
      {
        new_rmvt[i] = armvt[i] * brmvt[i];
      }
      L_MVT(*mvt_desc, ptdv, res_class) = (PTR_HEAPELEM)new_rmvt; 
      REL_HEAP;
      break;
    default:
      controlled_exit("undefined type constellation in t_mvt_mvt_ip");
      break;
  }
  DBUG_RETURN(mvt_desc);
}

/****************************************/
/* MVT ABS                              */
/****************************************/
T_PTD t_mvt_abs(T_PTD a, int class, int type)
{
  double  *rmvt, *armvt;
  int rows, cols, n, help, *imvt, *aimvt;
  
  DBUG_ENTER("mvt_abs");
  rows = R_MVT(*a, nrows, class);
  cols = R_MVT(*a, ncols, class);
  n = rows * cols;
  switch(type)
  {
    case TY_INTEGER:
      MAKEDESC(mvt_desc, 1, class, TY_INTEGER);
      L_MVT(*mvt_desc, ptdd, class) = NULL;
      L_MVT(*mvt_desc, nrows, class) = rows;
      L_MVT(*mvt_desc, ncols, class) = cols;
      GET_HEAP(n, A_MVT(*mvt_desc, ptdv, class));
      imvt = R_MVT(*mvt_desc, ptdv, class); 
      aimvt = R_MVT(DESC(a), ptdv, class);
      RES_HEAP;
      while (n > 0)
      {
        n--;
        if (0 > aimvt[n])
          imvt[n] = -aimvt[n];
        else
          imvt[n] = aimvt[n];
      }
      REL_HEAP;
      break;
    case TY_REAL:
      {
        MAKEDESC(mvt_desc, 1, class, TY_REAL);
        L_MVT(*mvt_desc, ptdd, class) = NULL;
        L_MVT(*mvt_desc, nrows, class) = rows;
        L_MVT(*mvt_desc, ncols, class) = cols;
        GET_HEAP(n * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
        rmvt = (double *)R_MVT(*mvt_desc, ptdv, class); 
        armvt = (double *)R_MVT(DESC(a), ptdv, class);
        RES_HEAP;
        while (n > 0)
        {
          n--;
          if (0 > armvt[n])
            rmvt[n] = -armvt[n];
          else
            rmvt[n] = armvt[n];
        }
        REL_HEAP;
      }
      break;
    default:
      controlled_exit("undefined type in mvt_abs");
  }
  DBUG_RETURN(mvt_desc);
}
/****************************************/
/* MVT TRUNCATE                         */
/****************************************/
T_PTD mvt_trunc(T_PTD a, int class, int type)
{
  DBUG_ENTER("mvt_trunc");
  switch(type)
  {
    case TY_INTEGER:
      INC_REFCNT(a);
      DBUG_RETURN(a);
      break;
    case TY_REAL:
      {
        double  *armvt;
        int rows, cols, n, help, *imvt;
        rows = R_MVT(*a, nrows, class);
        cols = R_MVT(*a, ncols, class);
        n = rows * cols;
        MAKEDESC(mvt_desc, 1, class, TY_INTEGER);
        L_MVT(*mvt_desc, ptdd, class) = NULL;
        L_MVT(*mvt_desc, nrows, class) = rows;
        L_MVT(*mvt_desc, ncols, class) = cols;
        GET_HEAP(n, A_MVT(*mvt_desc, ptdv, class));
        imvt = R_MVT(*mvt_desc, ptdv, class); 
        armvt = (double *)R_MVT(DESC(a), ptdv, class);
        RES_HEAP;
        while (n > 0)
        {
          n--;
          imvt[n] = (int)armvt[n];
        }
        REL_HEAP;
      }
      DBUG_RETURN(mvt_desc);
      break;
    default:
      controlled_exit("undefined type in mvt_trunc");
  }
  DBUG_RETURN(mvt_desc);
}




/****************************************/
/* MVT FRACTURE                         */
/****************************************/
T_PTD mvt_frac(T_PTD a, int class, int type)
{
  double *rmvt, *armvt;
  int rows, cols, n;
  
  DBUG_ENTER("mvt_frac");
  rows = R_MVT(*a, nrows, class);
  cols = R_MVT(*a, ncols, class);
  n = rows * cols;
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = rows;
  L_MVT(*mvt_desc, ncols, class) = cols;
  switch(type)
  {
    case TY_INTEGER:
      GET_HEAP(n, A_MVT(*mvt_desc, ptdv, class));
      while (n > 0)
      {
        n--;
        R_MVT(*mvt_desc, ptdv, class)[n] = 0;
      }
      break;
    case TY_REAL:
      {
        GET_HEAP(n * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
        rmvt = (double *)R_MVT(*mvt_desc, ptdv, class); 
        armvt = (double *)R_MVT(DESC(a), ptdv, class);
        RES_HEAP;
        while (n > 0)
        {
          n--;
          rmvt[n] = armvt[n] - (int)armvt[n];
        }
        REL_HEAP;
      }
      break;
    default:
      controlled_exit("undefined type in mvt_frac");
  }
  DBUG_RETURN(mvt_desc);
}

/****************************************/
/* MVT make NEGATIVE (int)              */
/****************************************/
T_PTD mvt_int_neg(T_PTD a)
{
  int rows, cols, n, *imvt, class;
  DBUG_ENTER("mvt_int_neg");
  
  class = R_DESC(*a, class);
  rows = R_MVT(*a, nrows, class);
  cols = R_MVT(*a, ncols, class);
  n = rows * cols;
  imvt = (int *)R_MVT(*a, ptdv, class);
  MAKEDESC(mvt_desc, 1, class, TY_INTEGER);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = rows;
  L_MVT(*mvt_desc, ncols, class) = cols;
  GET_HEAP(n, A_MVT(*mvt_desc, ptdv, class));
  while (n > 0)
  {
    n--;
    L_MVT(*mvt_desc, ptdv, class)[n] = -imvt[n];
  }
  DBUG_RETURN(mvt_desc);
}
  
/****************************************/
/* MVT make NEGATIVE (real)             */
/****************************************/
T_PTD mvt_real_neg(T_PTD a)
{
  int rows, cols, n, class;
  double *rmvt, *armvt;
  DBUG_ENTER("mvt_real_neg");
  
  class = R_DESC(*a, class);
  rows = R_MVT(*a, nrows, class);
  cols = R_MVT(*a, ncols, class);
  n = rows * cols;
  armvt = (double *)R_MVT(*a, ptdv, class);
  MAKEDESC(mvt_desc, 1, class, TY_REAL);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = rows;
  L_MVT(*mvt_desc, ncols, class) = cols;
  GET_HEAP(n * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  rmvt = (double *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  while (n > 0)
  {
    n--;
    rmvt[n] = -armvt[n];
  }
  L_MVT(*mvt_desc, ptdv, class) = (PTR_HEAPELEM)rmvt;
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}

/****************************************/
/* MVT make CEILING  (int)              */
/****************************************/
T_PTD mvt_int_ceil(T_PTD a)
{
  DBUG_ENTER("mvt_int_ceil");
  INC_REFCNT(a);
  DBUG_RETURN(mvt_desc);
}
  
/****************************************/
/* MVT make CEILING  (real)             */
/****************************************/
T_PTD mvt_real_ceil(T_PTD a)
{
  int rows, cols, n, class, *imvt;
  double *armvt;
  DBUG_ENTER("mvt_real_ceil");
  
  class = R_DESC(*a, class);
  rows = R_MVT(*a, nrows, class);
  cols = R_MVT(*a, ncols, class);
  n = rows * cols;
  armvt = (double *)R_MVT(*a, ptdv, class);
  MAKEDESC(mvt_desc, 1, class, TY_INTEGER);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = rows;
  L_MVT(*mvt_desc, ncols, class) = cols;
  GET_HEAP(n, A_MVT(*mvt_desc, ptdv, class));
  imvt = R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  while (n > 0)
  {
    n--;
    imvt[n] = (int)(armvt[n] <= 0.0 ? armvt[n] : armvt[n] + 1);
  }
  L_MVT(*mvt_desc, ptdv, class) = (PTR_HEAPELEM)imvt;
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}

/****************************************/
/* MVT make FLOOR    (int)              */
/****************************************/
T_PTD mvt_int_floor(T_PTD a)
{
  DBUG_ENTER("mvt_int_floor");
  INC_REFCNT(a);
  DBUG_RETURN(mvt_desc);
}
  
/****************************************/
/* MVT make FLOOR    (real)             */
/****************************************/
T_PTD mvt_real_floor(T_PTD a)
{
  int rows, cols, n, class, *imvt;
  double *armvt;
  DBUG_ENTER("mvt_real_floor");
  
  class = R_DESC(*a, class);
  rows = R_MVT(*a, nrows, class);
  cols = R_MVT(*a, ncols, class);
  n = rows * cols;
  armvt = (double *)R_MVT(*a, ptdv, class);
  MAKEDESC(mvt_desc, 1, class, TY_INTEGER);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = rows;
  L_MVT(*mvt_desc, ncols, class) = cols;
  GET_HEAP(n, A_MVT(*mvt_desc, ptdv, class));
  imvt = R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  while (n > 0)
  {
    n--;
    imvt[n] = (int)(armvt[n] >= 0.0 ? armvt[n] : armvt[n] + 1);
  }
  L_MVT(*mvt_desc, ptdv, class) = (PTR_HEAPELEM)imvt;
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


/*****************************************/
/* VC_MAX (INT, BOOL VECTOR)             */
/*****************************************/

T_PTD t_tv_vc_max(T_PTD a, int type, int class)
{
  int i, vali, *ip, dim;
  DBUG_ENTER("t_tv_vc_max");
  
  switch(type)
  {
    case TY_INTEGER:
      RES_HEAP;
      ip = (int *) R_MVT(DESC(a),ptdv,class);
      vali = ip[0];
      for (i = 1; i < dim; i++)
        if (vali < ip[i]) vali = ip[i];
      REL_HEAP;
      mvt_desc = (PTR_DESCRIPTOR)TAG_INT(vali);
      break;
    case TY_BOOL:
      RES_HEAP;
      ip = (int *) R_MVT(DESC(a),ptdv,class);
      vali = ip[0];
      for (i = 1; i < dim; i++)
        if (vali < ip[i]) vali = ip[i];
      REL_HEAP;
      if (vali) 
        mvt_desc = (PTR_DESCRIPTOR)SA_TRUE;
      else
        mvt_desc = (PTR_DESCRIPTOR)SA_FALSE;
      break;
    default:
     controlled_exit("undefined type in t_tv_vc_max"); 
  }
  DBUG_RETURN(mvt_desc);
}

/*****************************************/
/* VC_MAX (REAL VECTOR)                  */
/*****************************************/

double t_tvr_vc_max(T_PTD a, int type, int class)
{
  int i, dim;
  double valr, *rp; 
  DBUG_ENTER("t_tvr_vc_max");
  
  RES_HEAP;
  rp = (double *) R_MVT(DESC(a),ptdv,class);
  valr = rp[0];
  for (i = 1; i < dim; i++)
    if (valr < rp[i]) valr = rp[i];
  REL_HEAP;
  
  DBUG_RETURN(valr);
}


/*****************************************/
/* VC_MIN (INT, BOOL VECTOR)             */
/*****************************************/

T_PTD t_tv_vc_min(T_PTD a, int type, int class)
{
  int i, vali, *ip, dim;
  DBUG_ENTER("t_tv_vc_min");
  
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  switch(type)
  {
    case TY_INTEGER:
      RES_HEAP;
      ip = (int *) R_MVT(DESC(a),ptdv,class);
      vali = ip[0];
      for (i = 1; i < dim; i++)
        if (vali > ip[i]) vali = ip[i];
      REL_HEAP;
      mvt_desc = (PTR_DESCRIPTOR)TAG_INT(vali);
      break;
    case TY_BOOL:
      RES_HEAP;
      ip = (int *) R_MVT(DESC(a),ptdv,class);
      vali = ip[0];
      for (i = 1; i < dim; i++)
        if (vali > ip[i]) vali = ip[i];
      REL_HEAP;
      if (vali) 
        mvt_desc = (PTR_DESCRIPTOR)SA_TRUE;
      else
        mvt_desc = (PTR_DESCRIPTOR)SA_FALSE;
      break;
    default:
     controlled_exit("undefined type in t_tv_vc_min"); 
  }
  DBUG_RETURN(mvt_desc);
}

/*****************************************/
/* VC_MIN (REAL VECTOR)                  */
/*****************************************/

double t_tvr_vc_min(T_PTD a, int type, int class)
{
  int i, dim;
  double valr, *rp; 
  DBUG_ENTER("t_tvr_vc_min");
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  rp = (double *) R_MVT(DESC(a),ptdv,class);
  valr = rp[0];
  for (i = 1; i < dim; i++)
    if (valr > rp[i]) valr = rp[i];
  REL_HEAP;
  
  DBUG_RETURN(valr);
}


/************************************/
/* VC_PLUS (INT VECTOR)             */
/************************************/

T_PTD t_tv_vc_plus(T_PTD a, int type, int class)
{
  int i, vali, *ip, dim;
  DBUG_ENTER("t_tv_vc_plus");
  
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  ip = (int *) R_MVT(DESC(a),ptdv,class);
  vali = ip[0];
  for (i = 1; i < dim; i++)
    vali += ip[i];
  REL_HEAP;
  mvt_desc = (PTR_DESCRIPTOR)TAG_INT(vali);
  DBUG_RETURN(mvt_desc);
}

/*****************************************/
/* VC_PLUS (REAL VECTOR)                 */
/*****************************************/

double t_tvr_vc_plus(T_PTD a, int type, int class)
{
  int i, dim;
  double valr, *rp; 
  DBUG_ENTER("t_tvr_vc_plus");
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  rp = (double *) R_MVT(DESC(a),ptdv,class);
  valr = rp[0];
  for (i = 1; i < dim; i++)
    valr += rp[i];
  REL_HEAP;
  
  DBUG_RETURN(valr);
}

/*************************************/
/* VC_MULT  (INT VECTOR)             */
/*************************************/

T_PTD t_tv_vc_mult(T_PTD a, int type, int class)
{
  int i, vali, *ip, dim;
  DBUG_ENTER("t_tv_vc_mult");
  
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  ip = (int *) R_MVT(DESC(a),ptdv,class);
  vali = ip[0];
  for (i = 1; i < dim; i++)
    vali *= ip[i];
  REL_HEAP;
  mvt_desc = (PTR_DESCRIPTOR)TAG_INT(vali);
  DBUG_RETURN(mvt_desc);
}

/*****************************************/
/* VC_MULT (REAL VECTOR)                 */
/*****************************************/

double t_tvr_vc_mult(T_PTD a, int type, int class)
{
  int i, dim;
  double valr, *rp; 
  DBUG_ENTER("t_tvr_vc_mult");
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  rp = (double *) R_MVT(DESC(a),ptdv,class);
  valr = rp[0];
  for (i = 1; i < dim; i++)
    valr *= rp[i];
  REL_HEAP;
  
  DBUG_RETURN(valr);
}

/*************************************/
/* VC_DIV   (INT VECTOR)             */
/*************************************/

double t_tv_vc_div(T_PTD a, int type, int class)
{
  int i, *ip, dim;
  double valr;
  DBUG_ENTER("t_tv_vc_div");
  
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  ip = (int *) R_MVT(DESC(a),ptdv,class);
  valr = (double)ip[0];
  for (i = 1; i < dim; ++i)
  {
    valr /= (double) ip[i];
    if (++i >= dim) break;
    valr *= (double) ip[i];
  }
  REL_HEAP;
  DBUG_RETURN(valr);
}

/*****************************************/
/* VC_DIV  (REAL VECTOR)                 */
/*****************************************/

double t_tvr_vc_div(T_PTD a, int type, int class)
{
  int i, dim;
  double valr, *rp; 
  DBUG_ENTER("t_tvr_vc_div");
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  rp = (double *) R_MVT(DESC(a),ptdv,class);
  valr = rp[0];
  for (i = 1; i < dim; ++i)
  {
    valr /= rp[i];
    if (++i >= dim) break;
    valr *= rp[i];
  }
  REL_HEAP;
  
  DBUG_RETURN(valr);
}


/*************************************/
/* VC_MINUS (INT VECTOR)             */
/*************************************/

T_PTD t_tv_vc_minus(T_PTD a, int type, int class)
{
  int i, *ip, dim;
  int vali;
  DBUG_ENTER("t_tv_vc_minus");
  
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  ip = (int *) R_MVT(DESC(a),ptdv,class);
  vali = ip[0];
  for (i = 1; i < dim; ++i)
  {
    vali -=  ip[i];
    if (++i >= dim) break;
    vali +=  ip[i];
  }
  REL_HEAP;
  DBUG_RETURN((PTR_DESCRIPTOR)TAG_INT(vali));
}

/*****************************************/
/* VC_MINUS(REAL VECTOR)                 */
/*****************************************/

double t_tvr_vc_minus(T_PTD a, int type, int class)
{
  int i, dim;
  double valr, *rp; 
  DBUG_ENTER("t_tvr_vc_minus");
  dim = R_MVT(DESC(a), ncols, class) * R_MVT(DESC(a), nrows, class); 
  RES_HEAP;
  rp = (double *) R_MVT(DESC(a),ptdv,class);
  valr = rp[0];
  for (i = 1; i < dim; ++i)
  {
    valr -= rp[i];
    if (++i >= dim) break;
    valr += rp[i];
  }
  REL_HEAP;
  
  DBUG_RETURN(valr);
}
