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
PTR_HEAPELEM pth;
extern int AsciiOutput, KiROutput, withtilde, number_of_descs;
#define DESC(x)  (* (PTR_DESCRIPTOR) x)
/********************************************************************/
/* EQUAL ON                                                         */     
/* int_mat x int                                                    */     
/* real_mat x int                                                   */     
/* int_mat x real                                                   */     
/* real_mat x real                                                  */     
/* int x int_mat                                                    */     
/* int x real_mat                                                   */     
/* real x int_mat                                                   */     
/* real x real_mat                                                  */     
/* RESULT is always a bool_mat                                      */     
/********************************************************************/
T_PTD t_scal_mvt_equal(T_PTD a, T_PTD b)
{
  int rows, cols, i, class;
  register double *rmvt;
  DBUG_ENTER("t_compare");

  if (T_INT((STACKELEM)(STACKELEM)a))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*b, ptdv, class)[i] == VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] == (double)VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*a, class)) && (TY_REAL == R_DESC(*a, type)))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*b, ptdv, class)[i] == R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] == R_SCALAR(*a, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if (T_INT((STACKELEM)b))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*a, ptdv, class)[i] == VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] == (double)VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*b, class)) && (TY_REAL == R_DESC(*b, type)))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*a, ptdv, class)[i] == R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] == R_SCALAR(*b, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  DBUG_RETURN(mvt_desc);
}


/********************************************************************/
/* NOT EQUAL ON                                                     */     
/* int_mat x int                                                    */     
/* real_mat x int                                                   */     
/* int_mat x real                                                   */     
/* real_mat x real                                                  */     
/* int x int_mat                                                    */     
/* int x real_mat                                                   */     
/* real x int_mat                                                   */     
/* real x real_mat                                                  */     
/* RESULT is always a bool_mat                                      */     
/********************************************************************/
T_PTD t_scal_mvt_nequal(T_PTD a, T_PTD b)
{
  int class;
  int rows, cols, i;
  register double *rmvt;
  DBUG_ENTER("t_compare");

  if (T_INT((STACKELEM)a))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*b, ptdv, class)[i] != VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] != (double)VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*a, class)) && (TY_REAL == R_DESC(*a, type)))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*b, ptdv, class)[i] != R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] != R_SCALAR(*a, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if (T_INT((STACKELEM)b))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*a, ptdv, class)[i] != VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] != (double)VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*b, class)) && (TY_REAL == R_DESC(*b, type)))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*a, ptdv, class)[i] != R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] != R_SCALAR(*b, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  DBUG_RETURN(mvt_desc);
}


/********************************************************************/
/* LESS EQUAL ON                                                    */     
/* int_mat x int                                                    */     
/* real_mat x int                                                   */     
/* int_mat x real                                                   */     
/* real_mat x real                                                  */     
/* int x int_mat                                                    */     
/* int x real_mat                                                   */     
/* real x int_mat                                                   */     
/* real x real_mat                                                  */     
/* RESULT is always a bool_mat                                      */     
/********************************************************************/
T_PTD t_scal_mvt_lequal(T_PTD a, T_PTD b)
{
  int class;
  int rows, cols, i;
  register double *rmvt;
  DBUG_ENTER("t_compare");

  if (T_INT((STACKELEM)a))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*b, ptdv, class)[i] >= VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] >= (double)VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*a, class)) && (TY_REAL == R_DESC(*a, type)))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*b, ptdv, class)[i] >= R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] >= R_SCALAR(*a, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if (T_INT((STACKELEM)b))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*a, ptdv, class)[i] <= VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] <= (double)VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*b, class)) && (TY_REAL == R_DESC(*b, type)))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*a, ptdv, class)[i] >= R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] <= R_SCALAR(*b, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  DBUG_RETURN(mvt_desc);
}

/********************************************************************/
/* GREATER EQUAL ON                                                 */     
/* int_mat x int                                                    */     
/* real_mat x int                                                   */     
/* int_mat x real                                                   */     
/* real_mat x real                                                  */     
/* int x int_mat                                                    */     
/* int x real_mat                                                   */     
/* real x int_mat                                                   */     
/* real x real_mat                                                  */     
/* RESULT is always a bool_mat                                      */     
/********************************************************************/
T_PTD t_scal_mvt_gequal(T_PTD a, T_PTD b)
{
  int class;
  int rows, cols, i;
  register double *rmvt;
  DBUG_ENTER("t_compare");

  if (T_INT((STACKELEM)a))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*b, ptdv, class)[i] <= VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] <= (double)VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*a, class)) && (TY_REAL == R_DESC(*a, type)))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*b, ptdv, class)[i] <= R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] <= R_SCALAR(*a, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if (T_INT((STACKELEM)b))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*a, ptdv, class)[i] >= VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] >= (double)VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*b, class)) && (TY_REAL == R_DESC(*b, type)))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*a, ptdv, class)[i] <= R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] >= R_SCALAR(*b, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  DBUG_RETURN(mvt_desc);
}


/********************************************************************/
/* GREATER ON                                                       */     
/* int_mat x int                                                    */     
/* real_mat x int                                                   */     
/* int_mat x real                                                   */     
/* real_mat x real                                                  */     
/* int x int_mat                                                    */     
/* int x real_mat                                                   */     
/* real x int_mat                                                   */     
/* real x real_mat                                                  */     
/* RESULT is always a bool_mat                                      */     
/********************************************************************/
T_PTD t_scal_mvt_greater(T_PTD a, T_PTD b)
{
  int class;
  int rows, cols, i;
  register double *rmvt;
  DBUG_ENTER("t_compare");

  if (T_INT((STACKELEM)a))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*b, ptdv, class)[i] < VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] < (double)VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*a, class)) && (TY_REAL == R_DESC(*a, type)))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*b, ptdv, class)[i] < R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] < R_SCALAR(*a, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if (T_INT((STACKELEM)b))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*a, ptdv, class)[i] > VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] > (double)VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*b, class)) && (TY_REAL == R_DESC(*b, type)))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*a, ptdv, class)[i] < R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] > R_SCALAR(*b, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  DBUG_RETURN(mvt_desc);
}

/********************************************************************/
/* LESS ON                                                          */     
/* int_mat x int                                                    */     
/* real_mat x int                                                   */     
/* int_mat x real                                                   */     
/* real_mat x real                                                  */     
/* int x int_mat                                                    */     
/* int x real_mat                                                   */     
/* real x int_mat                                                   */     
/* real x real_mat                                                  */     
/* RESULT is always a bool_mat                                      */     
/********************************************************************/
T_PTD t_scal_mvt_less(T_PTD a, T_PTD b)
{
  int class;
  int rows, cols, i;
  register double *rmvt;
  DBUG_ENTER("t_compare");

  if (T_INT((STACKELEM)a))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*b, ptdv, class)[i] > VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] > (double)VAL_INT((int)a))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;   
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*a, class)) && (TY_REAL == R_DESC(*a, type)))
  {
    class = R_DESC(DESC(b), class);
    rows = R_MVT(*b, nrows, class);
    cols = R_MVT(*b, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*b, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*b, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*b, ptdv, class)[i] > R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*b, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] > R_SCALAR(*a, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if (T_INT((STACKELEM)b))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if (R_MVT(*a, ptdv, class)[i] < VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] < (double)VAL_INT((int)b))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  else if ((C_SCALAR == R_DESC(*b, class)) && (TY_REAL == R_DESC(*b, type)))
  {
    class = R_DESC(DESC(a), class);
    rows = R_MVT(*a, nrows, class);
    cols = R_MVT(*a, nrows, class);
    MAKEDESC(mvt_desc, 1, R_MVT(*a, class, class), TY_BOOL);
    L_MVT((*mvt_desc), ptdd, class) = NULL;
    L_MVT(*mvt_desc, nrows, class) = rows;
    L_MVT(*mvt_desc, ncols, class) = cols;
    GET_HEAP(rows * cols, A_MVT((*mvt_desc), ptdv, class));
    switch(R_MVT(*a, type, class))
    {
      case TY_INTEGER:
        for (i = 0; i < rows * cols; i++)
        {
          if ((double)(R_MVT(*a, ptdv, class)[i] > R_SCALAR(*a, valr)))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      case TY_REAL:
        rmvt = (double *)R_MVT(*a, ptdv, class);
        for (i = 0; i < rows * cols; i++)
        {
          if (rmvt[i] < R_SCALAR(*b, valr))
            L_MVT(*mvt_desc, ptdv, class)[i] = TRUE;
          else
            L_MVT(*mvt_desc, ptdv, class)[i] = FALSE;
        }
        break;
      default:
        controlled_exit("t_equal: unknown type");
    }
  }
  DBUG_RETURN(mvt_desc);
}

/**************************/
/* MVT to LIST            */
/**************************/
T_PTD t_to_list(T_PTD a, int class, int type)
{
  int dim, rows, cols, i, j;
  PTR_DESCRIPTOR h_desc, r_desc;

  DBUG_ENTER("t_to_list");
  rows = R_MVT(DESC(a), nrows, class);  
  cols = R_MVT(DESC(a), ncols, class);  
  if (0 == cols * rows)
    mvt_desc = _nil;
  else
  {
    switch(class)
    {
      case C_VECTOR:
      case C_TVECTOR:
        dim = rows * cols;
        MAKEDESC(mvt_desc, 1, C_LIST, TY_UNDEF);
        L_LIST(*mvt_desc, ptdd) = NULL;
        L_LIST(*mvt_desc, special) = NULL;
        L_LIST(*mvt_desc, dim) = dim;
        GET_HEAP(dim, A_LIST(*mvt_desc, ptdv));
        RES_HEAP;
        pth = R_LIST(*mvt_desc,ptdv);
        RES_HEAP;
        switch(type)
        {
          case TY_INTEGER:
            for (i = 0; i < dim; i++)
              pth[i] = TAG_INT(R_MVT(DESC(a), ptdv, class)[i]);
            break;
          case TY_REAL:
            {
              double *rmvt, valr;
              rmvt = (double *)R_MVT(DESC(a), ptdv, class);
              for (i = 0; i < dim; i++)
              {
                MAKEDESC(h_desc, 1, C_SCALAR, TY_REAL);
                L_SCALAR(*h_desc, valr) = rmvt[i];
                pth[i] = (T_HEAPELEM)h_desc; 
              }
            }
            break;
          case TY_BOOL:
            for (i = 0; i < dim; i++)
              pth[i] = R_MVT(DESC(a), ptdv, class)[i] ? SA_TRUE : SA_FALSE;
            break;
          default: controlled_exit("unknown type in t_to_list");
        }
        REL_HEAP;
        break;
      case C_MATRIX:
        MAKEDESC(mvt_desc, 1, C_LIST, TY_UNDEF);
        L_LIST(*mvt_desc, ptdd) = NULL;
        L_LIST(*mvt_desc, special) = NULL;
        L_LIST(*mvt_desc, dim) = rows;
        GET_HEAP(rows, A_LIST(*mvt_desc, ptdv));
        switch(type)
        {
          case TY_INTEGER:
            for (i = 0; i < rows; i++)
            {
              MAKEDESC(h_desc, 1, C_LIST, TY_UNDEF);
              GET_HEAP(cols, A_LIST(*h_desc, ptdv));
              RES_HEAP;
              L_LIST(*mvt_desc, ptdv)[i] = (T_HEAPELEM)h_desc;
              L_LIST(*h_desc, ptdd) = NULL;
              L_LIST(*h_desc, special) = NULL;
              L_LIST(*h_desc, dim) = cols;
              pth = R_LIST(*h_desc, ptdv);
              for (j = 0; j < cols; j++)
                pth[j] = TAG_INT(R_MVT(DESC(a), ptdv, class)[i * cols + j]);
              REL_HEAP;
            }
            break;
          case TY_REAL:
            {
              double *rmvt, valr;
              for (i = 0; i < rows; i++)
              {
                MAKEDESC(h_desc, 1, C_LIST, TY_UNDEF);
                GET_HEAP(cols, A_LIST(*h_desc, ptdv));
                RES_HEAP;
                L_LIST(*mvt_desc, ptdv)[i] = (T_HEAPELEM)h_desc;
                L_LIST(*h_desc, ptdd) = NULL;
                L_LIST(*h_desc, special) = NULL;
                L_LIST(*h_desc, dim) = cols;
                pth = R_LIST(*h_desc, ptdv);
                rmvt = (double *)R_MVT(DESC(a), ptdv, class);
                REL_HEAP;
                for (j = 0; j < cols; j++)
                {
                  MAKEDESC(r_desc, 1, C_SCALAR, TY_REAL);
                  RES_HEAP;
                  L_SCALAR(*r_desc, valr) = rmvt[i * cols + j];
                  pth[j] = (T_HEAPELEM)r_desc; 
                  REL_HEAP;
                }
              }
            }
            break;
          case TY_BOOL:
            for (i = 0; i < rows; i++)
            {
              MAKEDESC(h_desc, 1, C_LIST, TY_UNDEF);
              GET_HEAP(cols, A_LIST(*h_desc, ptdv));
              RES_HEAP;
              L_LIST(*mvt_desc, ptdv)[i] = (T_HEAPELEM)h_desc;
              L_LIST(*h_desc, ptdd) = NULL;
              L_LIST(*h_desc, special) = NULL;
              L_LIST(*h_desc, dim) = cols;
              pth = R_LIST(*h_desc, ptdv);
              for (j = 0; j < cols; j++)
                pth[j] = R_MVT(DESC(a), ptdv, class)[i * cols + j] ? SA_TRUE : SA_FALSE;
              REL_HEAP;
            }
            break;
          default: controlled_exit("unknown type in t_to_list");
        }
        break;
    }
  }
  DBUG_RETURN(mvt_desc);
}


T_PTD t_vvcut(int a, T_PTD b, int class, int type)
{
  int dim, i, pos; 
  register int *ip, *ipnew;
  register double *rp, *rpnew;

  DBUG_ENTER("t_vvcut");
  dim = R_MVT(DESC(b), ncols, class);

  if (a >= 0) {                                        /* cutfirst       */
    dim -=a;
    pos  = a;
  }
  else {                                                 /* cutlast        */
    dim += a;
    pos  = 0;
  }
  if (dim < 0)
    controlled_exit("vcut: vect to short");
  if (0 == dim)
  {
    INC_REFCNT(_nilvect);
    DBUG_RETURN(_nilvect);
  }
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT((*mvt_desc), ptdd, class) = NULL;
  L_MVT((*mvt_desc), nrows, class) = 1;
  L_MVT((*mvt_desc), ncols, class) = dim;
  switch(type)
  {
    case TY_INTEGER:
    case TY_BOOL:
      GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
      RES_HEAP;
      ip = (int *) R_MVT(DESC(b), ptdv, class);
      ipnew = (int *) R_MVT(*mvt_desc, ptdv, class);
      for (i = 0; i < dim; i++, pos++)
        ipnew[i] = ip[pos];
      REL_HEAP;
      break;
    case TY_REAL:
      GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
      RES_HEAP;
      rp = (double *) R_MVT(DESC(b), ptdv, class);
      rpnew = (double *) R_MVT(*mvt_desc, ptdv, class);
      for (i = 0; i < dim; i++, pos++)
        rpnew[i] = rp[pos];
      REL_HEAP;
      break;
  }
  DBUG_RETURN(mvt_desc);
}


T_PTD t_tvcut(int a, T_PTD b, int class, int type)
{
  int dim, i, pos; 
  register int *ip, *ipnew;
  register double *rp, *rpnew;

  DBUG_ENTER("t_tvcut");
  dim = R_MVT(DESC(b), nrows, class);

  if (a >= 0) {                                        /* cutfirst       */
    dim -=a;
    pos  = a;
  }
  else {                                                 /* cutlast        */
    dim += a;
    pos  = 0;
  }
  if (dim < 0)
    controlled_exit("vcut: tvect to short");
  if (0 == dim)
  {
    INC_REFCNT(_niltvect);
    DBUG_RETURN(_niltvect);
  }
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT((*mvt_desc), ptdd, class) = NULL;
  L_MVT((*mvt_desc), ncols, class) = 1;
  L_MVT((*mvt_desc), nrows, class) = dim;
  switch(type)
  {
    case TY_INTEGER:
    case TY_BOOL:
      GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
      RES_HEAP;
      ip = (int *) R_MVT(DESC(b), ptdv, class);
      ipnew = (int *) R_MVT(*mvt_desc, ptdv, class);
      for (i = 0; i < dim; i++, pos++)
        ipnew[i] = ip[pos];
      REL_HEAP;
      break;
    case TY_REAL:
      GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
      RES_HEAP;
      rp = (double *) R_MVT(DESC(b), ptdv, class);
      rpnew = (double *) R_MVT(*mvt_desc, ptdv, class);
      for (i = 0; i < dim; i++, pos++)
        rpnew[i] = rp[pos];
      REL_HEAP;
      break;
  }
  DBUG_RETURN(mvt_desc);
}


T_PTD t_vselect(int a, T_PTD b, int class, int type)
{
  int h;
  DBUG_ENTER("t_vselect");
  
  if ((a < 1) || (a >= R_MVT(DESC(b), nrows, class) * R_MVT(DESC(b), ncols, class)))
    controlled_exit("vselect: (t)vect too short or index <= 0");
  switch(type)
  {
    case TY_INTEGER:
      DBUG_RETURN((T_PTD)TAG_INT(R_MVT(DESC(b), ptdv, class)[a - 1]));
      break;
    case TY_BOOL:
      h = R_MVT(DESC(b), ptdv, class)[a - 1];
      h = h ? SA_TRUE : SA_FALSE;
      DBUG_RETURN((T_PTD)h);
      break;
  }    
  DBUG_RETURN((T_PTD)0);
}


T_PTD t_vrselect(int a, T_PTD b, int class, int type)
{
  int h;
  register double *rmvt;
  DBUG_ENTER("t_vrselect");
  
  if ((a < 1) || (a >= R_MVT(DESC(b), nrows, class) * R_MVT(DESC(b), ncols, class)))
    controlled_exit("vselect: (t)vect too short or index <= 0");
  rmvt = (double *) R_MVT(DESC(b), ptdv, class);
  MAKEDESC(mvt_desc, 1, C_SCALAR, TY_REAL);
  L_SCALAR(*mvt_desc, valr) = rmvt[a - 1];
  DBUG_RETURN(mvt_desc);
}


T_PTD t_vvrotate(int a, T_PTD b, int class, int type)
{
  int dim = R_MVT(DESC(b), ncols, class);
  register int *ipnew, *ip, pos, elem, i;
  register double *rpnew, *rp;
  
  DBUG_ENTER("t_vvrotate");
  if (0 == dim)
  {
    INC_REFCNT(b);
    DBUG_RETURN((b));
  } 
  a = -a;
  if (0 == (a = a % dim))
  {
    INC_REFCNT(_nilvect);
    DBUG_RETURN((_nilvect));
  }
  else if (a < 1) a += dim;
  
  pos = R_MVT(DESC(b),ncols,class);
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, ncols, class) = dim;
  L_MVT(*mvt_desc, nrows, class) = 1;
  switch(type)
  {
    case TY_INTEGER:
    case TY_BOOL:
      GET_HEAP(dim, A_MVT((*mvt_desc), ptdv, class));
      RES_HEAP;
      ipnew = (int *) R_MVT((*mvt_desc),ptdv,class);
      ip= (int *) R_MVT(DESC(b),ptdv,class);
      for (i = 0; i < pos; i++)
      {
        ipnew[a] = ip[i];
        a++;
        if (a == pos)
          a = 0;
      }
      REL_HEAP;
      break;
    case TY_REAL:
      GET_HEAP(dim * sizeof(double), A_MVT((*mvt_desc), ptdv, class));
      RES_HEAP;
      rpnew = (double *) R_MVT((*mvt_desc),ptdv,class);
      rp= (double *) R_MVT(DESC(b),ptdv,class);
      for (i = 0; i < pos; i++)
      {
        rpnew[a] = rp[i];
        a++;
        if (a == pos)
          a = 0;
      }
      REL_HEAP;
      break;
  }
  DBUG_RETURN(mvt_desc);
}


T_PTD t_tvrotate(int a, T_PTD b, int class, int type)
{
  int dim = R_MVT(DESC(b), nrows, class);
  register int *ipnew, *ip, pos, elem, i;
  double *rpnew, *rp;
  
  DBUG_ENTER("t_tvrotate");
  if (0 == dim)
  {
    INC_REFCNT(_niltvect);
    DBUG_RETURN((_niltvect));
  } 
  a = -a;
  if (0 == (a = a % dim))
  {
    INC_REFCNT(b);
    DBUG_RETURN((b));
  }
  else if (a < 1) a += dim;
  
  pos = R_MVT(DESC(b),nrows,class);
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = dim;
  L_MVT(*mvt_desc, ncols, class) = 1;
  switch(type)
  {
    case TY_INTEGER:
    case TY_BOOL:
      GET_HEAP(dim, A_MVT((*mvt_desc), ptdv, class));
      RES_HEAP;
      ipnew = (int *) R_MVT((*mvt_desc),ptdv,class);
      ip= (int *) R_MVT(DESC(b),ptdv,class);
      for (i = 0; i < pos; i++)
      {
        ipnew[a] = ip[i];
        a++;
        if (a == pos)
          a = 0;
      }
      REL_HEAP;
      break;
    case TY_REAL:
      GET_HEAP(dim * sizeof(double), A_MVT((*mvt_desc), ptdv, class));
      RES_HEAP;
      rpnew = (double *) R_MVT((*mvt_desc),ptdv,class);
      rp= (double *) R_MVT(DESC(b),ptdv,class);
      for (i = 0; i < pos; i++)
      {
        rpnew[a] = rp[i];
        a++;
        if (a == pos)
          a = 0;
      }
      REL_HEAP;
      break;
  }
  DBUG_RETURN(mvt_desc);
}

T_PTD t_vv_unite_empty(T_PTD a)
{
  DBUG_ENTER("t_vv_unite_empty");
  INC_REFCNT(a);
  DBUG_RETURN(a);
}

T_PTD t_vv_unite(T_PTD a, T_PTD b, int class, int type)
{
  int dim, dima, dimb, i;
  DBUG_ENTER("t_vv_unite");
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  switch(class)
  {
    case C_VECTOR:
      dima = R_MVT(DESC(a), ncols, class);
      dimb = R_MVT(DESC(b), ncols, class);
      dim = dima + dimb;
      L_MVT(*mvt_desc, nrows, class) = 1;
      L_MVT(*mvt_desc, ncols, class) = dim;
      break;
    case C_TVECTOR:
      dima = R_MVT(DESC(a), nrows, class);
      dimb = R_MVT(DESC(b), nrows, class);
      dim = dima + dimb;
      L_MVT(*mvt_desc, ncols, class) = 1;
      L_MVT(*mvt_desc, nrows, class) = dim;
      break;
  }   
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  RES_HEAP;
  for (i = 0; i < dima; i++)
    L_MVT(*mvt_desc, ptdv, class)[i] = R_MVT(DESC(a), ptdv, class)[i];
  for (i = 0; i < dimb; i++)
    L_MVT(*mvt_desc, ptdv, class)[i + dima] = R_MVT(DESC(b), ptdv, class)[i];
  REL_HEAP;
  if (0 == dim)
  {
    if (C_TVECTOR == class)
    {
      INC_REFCNT(_niltvect);
      mvt_desc = _niltvect;
    }
    else
    {
      INC_REFCNT(_nilvect);
      mvt_desc = _nilvect;
    }
  }
  DBUG_RETURN(mvt_desc);
}
 
T_PTD t_vv_unite_rr(T_PTD a, T_PTD b, int class, int type)
{
  int dim, dima, dimb, i;
  register double *armvt, *brmvt, *resrmvt;
  DBUG_ENTER("t_vv_unite_rr");
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  switch(class)
  {
    case C_VECTOR:
      dima = R_MVT(DESC(a), ncols, class);
      dimb = R_MVT(DESC(b), ncols, class);
      dim = dima + dimb;
      L_MVT(*mvt_desc, nrows, class) = 1;
      L_MVT(*mvt_desc, ncols, class) = dim;
      break;
    case C_TVECTOR:
      dima = R_MVT(DESC(a), nrows, class);
      dimb = R_MVT(DESC(b), nrows, class);
      dim = dima + dimb;
      L_MVT(*mvt_desc, ncols, class) = 1;
      L_MVT(*mvt_desc, nrows, class) = dim;
      break;
  }   
  GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  resrmvt = (double *)R_MVT(*mvt_desc, ptdv, class);  
  armvt = (double *)R_MVT(DESC(a), ptdv, class);
  brmvt = (double *)R_MVT(DESC(b), ptdv, class);
  RES_HEAP;
  for (i = 0; i < dima; i++)
    resrmvt[i] = armvt[i];
  for (i = 0; i < dimb; i++)
    resrmvt[i + dima] = brmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_vv_unite_ri(T_PTD a, T_PTD b, int class, int type)
{
  int dim, dima, dimb, i, *bimvt;
  register double *armvt, *resrmvt;
  DBUG_ENTER("t_vv_unite_ri");
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  switch(class)
  {
    case C_VECTOR:
      dima = R_MVT(DESC(a), ncols, class);
      dimb = R_MVT(DESC(b), ncols, class);
      dim = dima + dimb;
      L_MVT(*mvt_desc, nrows, class) = 1;
      L_MVT(*mvt_desc, ncols, class) = dim;
      break;
    case C_TVECTOR:
      dima = R_MVT(DESC(a), nrows, class);
      dimb = R_MVT(DESC(b), nrows, class);
      dim = dima + dimb;
      L_MVT(*mvt_desc, ncols, class) = 1;
      L_MVT(*mvt_desc, nrows, class) = dim;
      break;
  }   
  GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  resrmvt = (double *)R_MVT(*mvt_desc, ptdv, class);  
  armvt = (double *)R_MVT(DESC(a), ptdv, class);
  bimvt = (int *)R_MVT(DESC(b), ptdv, class);
  RES_HEAP;
  for (i = 0; i < dima; i++)
    resrmvt[i] = armvt[i];
  for (i = 0; i < dimb; i++)
    resrmvt[i + dima] = (double)bimvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}



T_PTD t_vv_unite_ir(T_PTD a, T_PTD b, int class, int type)
{
  int dim, dima, dimb, i, *aimvt;
  register double *brmvt, *resrmvt;
  DBUG_ENTER("t_vv_unite_ri");
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  switch(class)
  {
    case C_VECTOR:
      dima = R_MVT(DESC(a), ncols, class);
      dimb = R_MVT(DESC(b), ncols, class);
      dim = dima + dimb;
      L_MVT(*mvt_desc, nrows, class) = 1;
      L_MVT(*mvt_desc, ncols, class) = dim;
      break;
    case C_TVECTOR:
      dima = R_MVT(DESC(a), nrows, class);
      dimb = R_MVT(DESC(b), nrows, class);
      dim = dima + dimb;
      L_MVT(*mvt_desc, ncols, class) = 1;
      L_MVT(*mvt_desc, nrows, class) = dim;
      break;
  }   
  GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  resrmvt = (double *)R_MVT(*mvt_desc, ptdv, class);  
  brmvt = (double *)R_MVT(DESC(b), ptdv, class);
  aimvt = (int *)R_MVT(DESC(a), ptdv, class);
  RES_HEAP;
  for (i = 0; i < dima; i++)
    resrmvt[i] = (double)aimvt[i];
  for (i = 0; i < dimb; i++)
    resrmvt[i + dima] = brmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_v_replace(int a, int b, T_PTD c, int class, int type)
{
  register int dim, i, *imvt;
  DBUG_ENTER("t_v_replace");
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  switch(class)
  {
    case C_VECTOR:
      dim = R_MVT(DESC(c), ncols, class);
      L_MVT(*mvt_desc, nrows, class) = 1;
      L_MVT(*mvt_desc, ncols, class) = dim;
      break;
    case C_TVECTOR:
      dim = R_MVT(DESC(c), nrows, class);
      L_MVT(*mvt_desc, ncols, class) = 1;
      L_MVT(*mvt_desc, nrows, class) = dim;
      break;
  }   
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = R_MVT(DESC(c), ptdv, class)[i];
  if (TY_BOOL == type)
    imvt[a] = (b == SA_FALSE ? FALSE : TRUE);
  else
    imvt[a] = b;
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}

T_PTD t_vr_replace(int a, double b, T_PTD c, int class, int type)
{
  int dim, i;
  register double *rmvt, *crmvt;
  
  DBUG_ENTER("t_vr_replace");
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  switch(class)
  {
    case C_VECTOR:
      dim = R_MVT(DESC(c), ncols, class);
      L_MVT(*mvt_desc, nrows, class) = 1;
      L_MVT(*mvt_desc, ncols, class) = dim;
      break;
    case C_TVECTOR:
      dim = R_MVT(DESC(c), nrows, class);
      L_MVT(*mvt_desc, ncols, class) = 1;
      L_MVT(*mvt_desc, nrows, class) = dim;
      break;
  }   
  GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  rmvt = (double *)R_MVT(*mvt_desc, ptdv, class);
  crmvt = (double *)R_MVT(DESC(c), ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    rmvt[i] = crmvt[i];
  rmvt[a] = b;
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}

T_PTD t_mcut(int r, int c, T_PTD p, int type)
{
  int rows, cols, i, pos, nrows, ncols;
  register int *imvt, *amvt;

  DBUG_ENTER("t_mcut");
  rows = R_MVT(DESC(p), nrows, C_MATRIX);
  cols = R_MVT(DESC(p), ncols, C_MATRIX);
  nrows = rows - abs(r);
  ncols = cols - abs(c);
  if ((nrows == 0) || (ncols == 0))
  {
    INC_REFCNT(_nilmat);
    DBUG_RETURN(_nilmat);
  }
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
  GET_HEAP(nrows * ncols, A_MVT(*mvt_desc, ptdv, C_MATRIX));
  amvt = (int *)R_MVT(DESC(p), ptdv, C_MATRIX);
  if ((r >= 0) && (c >= 0))
  {
    if ((r > rows) || (c > cols))
      controlled_exit("dimensions don't fit in mcut");
    amvt += cols * r;
    imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX); 
    RES_HEAP;
    pos = 0;
    for (i = 0; i < nrows * cols; i++)
    {
      if (i % cols >= c) 
      {
        imvt[pos] = amvt[i];
        pos++;
      }
    }
    REL_HEAP;
  }
  else if ((r >= 0) && (c < 0))
  {
    if ((r > rows) || (-c > cols))
      controlled_exit("dimensions don't fit in mcut");
    amvt += cols * r;
    imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX); 
    RES_HEAP;
    pos = 0;
    c = cols + c;
    for (i = 0; i < nrows * cols; i++)
    {
      if (i % cols < c) 
      {
        imvt[pos] = amvt[i];
        pos++;
      }
    }
    REL_HEAP;
  }
  else if ((r < 0) && (c >= 0))
  {
    if ((-r > rows) || (c > cols))
      controlled_exit("dimensions don't fit in mcut");
    imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    RES_HEAP;
    pos = 0;
    for (i = 0; i < nrows * cols; i++)
    {
      if (i % cols >= c)        
      {
        imvt[pos] = amvt[i];
        pos++;
      }
    }
    REL_HEAP;
  }
  else        /* ((r < 0) && (c < 0)) */
  {
    if ((-r > rows) || (-c > cols))
      controlled_exit("dimensions don't fit in mcut");
    imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    RES_HEAP;
    pos = 0;
    c = cols + c;
    for (i = 0; i < nrows * cols; i++)
    {
      if (i % cols < c)        
      {
        imvt[pos] = amvt[i];
        pos++;
      }
    }
    REL_HEAP;
  }
  DBUG_RETURN(mvt_desc);
}



T_PTD t_mcutr(int r, int c, T_PTD p, int type)
{
  int rows, cols, i, pos, nrows, ncols;
  register double *imvt, *amvt;

  DBUG_ENTER("t_mcutr");
  rows = R_MVT(DESC(p), nrows, C_MATRIX);
  cols = R_MVT(DESC(p), ncols, C_MATRIX);
  nrows = rows - abs(r);
  ncols = cols - abs(c);
  if ((nrows == 0) || (ncols == 0))
  {
    INC_REFCNT(_nilmat);
    DBUG_RETURN(_nilmat);
  }
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
  GET_HEAP(nrows * ncols * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
  amvt = (double *)R_MVT(DESC(p), ptdv, C_MATRIX);
  if ((r >= 0) && (c >= 0))
  {
    if ((r > rows) || (c > cols))
      controlled_exit("dimensions don't fit in mcut");
    amvt += cols * r;
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX); 
    RES_HEAP;
    pos = 0;
    for (i = 0; i < nrows * cols; i++)
    {
      if (i % cols >= c) 
      {
        imvt[pos] = amvt[i];
        pos++;
      }
    }
    REL_HEAP;
  }
  else if ((r >= 0) && (c < 0))
  {
    if ((r > rows) || (-c > cols))
      controlled_exit("dimensions don't fit in mcut");
    amvt += cols * r;
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX); 
    RES_HEAP;
    pos = 0;
    c = cols + c;
    for (i = 0; i < nrows * cols; i++)
    {
      if (i % cols < c) 
      {
        imvt[pos] = amvt[i];
        pos++;
      }
    }
    REL_HEAP;
  }
  else if ((r < 0) && (c >= 0))
  {
    if ((-r > rows) || (c > cols))
      controlled_exit("dimensions don't fit in mcut");
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    RES_HEAP;
    pos = 0;
    for (i = 0; i < nrows * cols; i++)
    {
      if (i % cols >= c)        
      {
        imvt[pos] = amvt[i];
        pos++;
      }
    }
    REL_HEAP;
  }
  else        /* ((r < 0) && (c < 0)) */
  {
    if ((-r > rows) || (-c > cols))
      controlled_exit("dimensions don't fit in mcut");
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    RES_HEAP;
    pos = 0;
    c = cols + c;
    for (i = 0; i < nrows * cols; i++)
    {
      if (i % cols < c)        
      {
        imvt[pos] = amvt[i];
        pos++;
      }
    }
    REL_HEAP;
  }
  DBUG_RETURN(mvt_desc);
}

T_PTD t_transform(T_PTD a, int class, int type)
{
  int rows, cols, dim, i;
  register int *imvt;

  DBUG_ENTER("t_transform");
  MAKEDESC(mvt_desc, 1, C_LIST, TY_UNDEF);
  L_LIST((*mvt_desc), ptdd) = NULL;
  L_LIST((*mvt_desc), special) = NULL;
  rows = R_MVT(DESC(a), nrows, class);
  cols = R_MVT(DESC(a), ncols, class);
  dim = rows * cols;
  GET_HEAP(dim, A_LIST(*mvt_desc, ptdv));
  L_LIST(*mvt_desc, dim) = dim;
  imvt = (int *)R_MVT(DESC(a), ptdv, class);
  switch(type)
  {
    case TY_INTEGER:
      RES_HEAP;
      for (i = 0; i < dim; i++)
        L_LIST(*mvt_desc, ptdv)[i] = TAG_INT(imvt[i]);
      REL_HEAP;
      break;
    case TY_BOOL:
      RES_HEAP;
      for (i = 0; i < dim; i++)
        L_LIST(*mvt_desc, ptdv)[i] = (imvt[i] == SA_FALSE ? SA_FALSE : SA_TRUE);
      REL_HEAP;
      break;
    default:
      controlled_exit("unknown type in transform");
  }
  DBUG_RETURN(mvt_desc);
}

        
T_PTD t_transformr(T_PTD a, int class, int type)
{
  int rows, cols, dim, i;
  register double *imvt;
  PTR_DESCRIPTOR h_desc ;

  DBUG_ENTER("t_transformr");
  MAKEDESC(mvt_desc, 1, C_LIST, TY_UNDEF);
  L_LIST((*mvt_desc), ptdd) = NULL;
  L_LIST((*mvt_desc), special) = NULL;
  rows = R_MVT(DESC(a), nrows, class);
  cols = R_MVT(DESC(a), ncols, class);
  dim = rows * cols;
  GET_HEAP(dim, A_LIST(*mvt_desc, ptdv));
  L_LIST(*mvt_desc, dim) = dim;
  imvt = (double *)R_MVT(DESC(a), ptdv, class);
  for (i = 0; i < dim; i++)
  {
    MAKEDESC(h_desc, 1, C_SCALAR, TY_REAL);
    RES_HEAP;
    L_SCALAR(*h_desc, valr) = imvt[i];
    L_LIST(*mvt_desc, ptdv)[i] = (T_HEAPELEM)h_desc; 
    REL_HEAP;
  }
  DBUG_RETURN(mvt_desc);
}
  

T_PTD t_transpose_empty(T_PTD a)
{
  DBUG_ENTER("t_transpose_empty");
  INC_REFCNT(a);
  DBUG_RETURN(a);
}
  

T_PTD t_transpose_vt(T_PTD a, int class, int type)
{
  int dim, i;
  register int *imvt, *amvt;
  DBUG_ENTER("t_transpose_vt");
  switch(class)
  {
    case C_VECTOR:
      MAKEDESC(mvt_desc, 1, C_TVECTOR, type);
      L_MVT((*mvt_desc), ptdd, C_TVECTOR) = NULL;
      dim = R_MVT(DESC(a), ncols, C_VECTOR);
      L_MVT(*mvt_desc, nrows, C_TVECTOR) = dim;
      L_MVT(*mvt_desc, ncols, C_TVECTOR) = 1;
      GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_TVECTOR));
      amvt = (int *)R_MVT(DESC(a), ptdv, C_VECTOR);
      imvt = (int *)R_MVT(*mvt_desc, ptdv, C_TVECTOR);
      break;
    case C_TVECTOR:
      MAKEDESC(mvt_desc, 1, C_VECTOR, type);
      L_MVT((*mvt_desc), ptdd, C_VECTOR) = NULL;
      dim = R_MVT(DESC(a), nrows, C_TVECTOR);
      L_MVT(*mvt_desc, nrows, C_VECTOR) = 1;
      L_MVT(*mvt_desc, ncols, C_VECTOR) = dim;
      GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_VECTOR));
      amvt = (int *)R_MVT(DESC(a), ptdv, C_TVECTOR);
      imvt = (int *)R_MVT(*mvt_desc, ptdv, C_VECTOR);
      break;
    default:
      controlled_exit("unknown class in t_transpose");
  }
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i];
  REL_HEAP;
  if (0 == dim)
  {
    if (C_TVECTOR == class)
    {
      INC_REFCNT(_niltvect);
      mvt_desc = _niltvect;
    }
    else
    {
      INC_REFCNT(_nilvect);
      mvt_desc = _nilvect;
    }
  }
  DBUG_RETURN(mvt_desc);
}


T_PTD t_transpose_vtr(T_PTD a, int class, int type)
{
  int dim, i;
  register double *imvt, *amvt;
  DBUG_ENTER("t_transpose_vtr");
  switch(class)
  {
    case C_VECTOR:
      MAKEDESC(mvt_desc, 1, C_TVECTOR, type);
      L_MVT((*mvt_desc), ptdd, C_TVECTOR) = NULL;
      dim = R_MVT(DESC(a), ncols, C_VECTOR);
      L_MVT(*mvt_desc, nrows, C_TVECTOR) = dim;
      L_MVT(*mvt_desc, ncols, C_TVECTOR) = 1;
      GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_TVECTOR));
      amvt = (double *)R_MVT(DESC(a), ptdv, C_VECTOR);
      imvt = (double *)R_MVT(*mvt_desc, ptdv, C_TVECTOR);
      break;
    case C_TVECTOR:
      MAKEDESC(mvt_desc, 1, C_VECTOR, type);
      L_MVT((*mvt_desc), ptdd, C_VECTOR) = NULL;
      dim = R_MVT(DESC(a), nrows, C_TVECTOR);
      L_MVT(*mvt_desc, nrows, C_VECTOR) = 1;
      L_MVT(*mvt_desc, ncols, C_VECTOR) = dim;
      GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_VECTOR));
      amvt = (double *)R_MVT(DESC(a), ptdv, C_TVECTOR);
      imvt = (double *)R_MVT(*mvt_desc, ptdv, C_VECTOR);
      break;
    default:
      controlled_exit("unknown class in t_transpose");
  }
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i];
  REL_HEAP;
  if (0 == dim)
  {
    if (C_TVECTOR == class)
    {
      INC_REFCNT(_niltvect);
      mvt_desc = _niltvect;
    }
    else
    {
      INC_REFCNT(_nilvect);
      mvt_desc = _nilvect;
    }
  }
  DBUG_RETURN(mvt_desc);
}



T_PTD t_transpose_m(T_PTD a, int type)
{
  int dim, rows, cols, i, j;
  register int *imvt, *amvt;
  DBUG_ENTER("t_transpose_m");

  cols = R_MVT(DESC(a), ncols, C_MATRIX);
  rows = R_MVT(DESC(a), nrows, C_MATRIX);
  if ((0 == rows) || (0 == cols))
  {
    INC_REFCNT(_nilmat);
    mvt_desc = _nilmat;
  }
  else
  {
    MAKEDESC(mvt_desc, 1, C_MATRIX, type);
    L_MVT((*mvt_desc), ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = cols;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = rows;
    dim = rows * cols;
    GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_MATRIX));
    amvt = (int *)R_MVT(DESC(a), ptdv, C_MATRIX);
    imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    RES_HEAP;
    for (j = 0; j < rows; j++)
      for (i = 0; i < cols; i++)
        imvt[i * rows + j] = amvt[i + j * cols];
    REL_HEAP;
  }
  DBUG_RETURN(mvt_desc);
}



T_PTD t_transpose_mr(T_PTD a, int type)
{
  int dim, rows, cols, i, j;
  register double *imvt, *amvt;
  DBUG_ENTER("t_transpose_mr");

  cols = R_MVT(DESC(a), ncols, C_MATRIX);
  rows = R_MVT(DESC(a), nrows, C_MATRIX);
  if ((0 == rows) || (0 == cols))
  {
    INC_REFCNT(_nilmat);
    mvt_desc = _nilmat;
  }
  else
  {
    MAKEDESC(mvt_desc, 1, C_MATRIX, type);
    L_MVT((*mvt_desc), ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = cols;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = rows;
    dim = rows * cols;
    GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
    amvt = (double *)R_MVT(DESC(a), ptdv, C_MATRIX);
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    RES_HEAP;
    for (j = 0; j < rows; j++)
      for (i = 0; i < cols; i++)
        imvt[i * rows + j] = amvt[i + j * cols];
    REL_HEAP;
  }
  DBUG_RETURN(mvt_desc);
}

int t_mdim(int r_or_c, T_PTD a)
{
  DBUG_ENTER("t_mdim");
  if (1 == r_or_c)
    DBUG_RETURN(TAG_INT(R_MVT(DESC(a), nrows, C_MATRIX)));
  else if (2 == r_or_c)
    DBUG_RETURN(TAG_INT(R_MVT(DESC(a), ncols, C_MATRIX)));
  else
    controlled_exit("mdim: invalid first parameter");
  DBUG_RETURN(0);
}    

T_PTD t_mreplace_r(int a, T_PTD b, T_PTD c, int type)
{
  int rows, cols, vcols, dim, i;
  register int *imvt, *amvt;

  DBUG_ENTER("t_mreplace_r");
  cols = R_MVT(DESC(c), ncols, C_MATRIX);
  rows = R_MVT(DESC(c), nrows, C_MATRIX);
  vcols = R_MVT(DESC(b), ncols, C_VECTOR);
  dim = rows * cols;
  if (vcols != cols) 
    controlled_exit("dimensions don't fit in t_mreplace_r");
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT((*mvt_desc), ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = rows;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = cols;
  amvt = (int *)R_MVT(DESC(c), ptdv, C_MATRIX);
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_MATRIX));
  imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i];
  amvt = (int *)R_MVT(DESC(b), ptdv, C_VECTOR);
  imvt = &imvt[(a - 1) * cols];
  for (i = 0; i < vcols; i++)
    imvt[i] = amvt[i];
  
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_mreplace_rr(int a, T_PTD b, T_PTD c, int type)
{
  int rows, cols, vcols, dim, i;
  register double *imvt, *amvt;

  DBUG_ENTER("t_mreplace_rr");
  cols = R_MVT(DESC(c), ncols, C_MATRIX);
  rows = R_MVT(DESC(c), nrows, C_MATRIX);
  vcols = R_MVT(DESC(b), ncols, C_VECTOR);
  dim = rows * cols;
  if (vcols != cols) 
    controlled_exit("dimensions don't fit in t_mreplace_r");
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT((*mvt_desc), ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = rows;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = cols;
  amvt = (double *)R_MVT(DESC(c), ptdv, C_MATRIX);
  GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
  imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i];
  amvt = (double *)R_MVT(DESC(b), ptdv, C_VECTOR);
  imvt = &imvt[(a - 1) * cols];
  for (i = 0; i < vcols; i++)
    imvt[(a-1) * rows + i] = amvt[i];
  
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_mreplace_c(int a, T_PTD b, T_PTD c, int type)
{
  int rows, cols, vrows, dim, i;
  register int *imvt, *amvt;

  DBUG_ENTER("t_mreplace_c");
  cols = R_MVT(DESC(c), ncols, C_MATRIX);
  rows = R_MVT(DESC(c), nrows, C_MATRIX);
  vrows = R_MVT(DESC(b), nrows, C_TVECTOR);
  dim = rows * cols;
  if (vrows != rows) 
    controlled_exit("dimensions don't fit in t_mreplace_r");
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT((*mvt_desc), ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = rows;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = cols;
  amvt = (int *)R_MVT(DESC(c), ptdv, C_MATRIX);
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_MATRIX));
  imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i];
  amvt = (int *)R_MVT(DESC(b), ptdv, C_TVECTOR);
  for (i = 0; i < vrows; i++)
    imvt[(a - 1) + i * cols] = amvt[i];
  
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_mreplace_cr(int a, T_PTD b, T_PTD c, int type)
{
  int rows, cols, vrows, dim, i;
  register double *imvt, *amvt;

  DBUG_ENTER("t_mreplace_cr");
  cols = R_MVT(DESC(c), ncols, C_MATRIX);
  rows = R_MVT(DESC(c), nrows, C_MATRIX);
  vrows = R_MVT(DESC(b), nrows, C_TVECTOR);
  dim = rows * cols;
  if (vrows != rows) 
    controlled_exit("dimensions don't fit in t_mreplace_r");
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT((*mvt_desc), ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = rows;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = cols;
  amvt = (double *)R_MVT(DESC(c), ptdv, C_MATRIX);
  GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
  imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i];
  amvt = (double *)R_MVT(DESC(b), ptdv, C_TVECTOR);
  for (i = 0; i < vrows; i++)
    imvt[(a - 1) + cols * i] = amvt[i];
  
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}



T_PTD t_mrotate(int a, int b, T_PTD c, int type)
{
  register int dim, count;
  int rowrotation, colrotation, nrows, ncols, pos, newrow, newcol;
  int    *ipnew,*ip;

  DBUG_ENTER("t_mrotate");
  a = -a;
  nrows = R_MVT(DESC(c),nrows,C_MATRIX);
  ncols = R_MVT(DESC(c),ncols,C_MATRIX);
  if (ncols == 1) 
    if (0 == (a = a % nrows))
      goto nothingtodo;
    else 
    {
      dim = nrows;
      if (a > 0)
        pos = a;
      else
        pos = dim + a;
      goto alg1;
    }
  b = -b;
  if (1 == nrows)
    if (0 == (a % ncols))
      goto nothingtodo;
    else
    {
      dim = ncols;
      if (b > 0)
        pos = b;
      else
        pos = dim + b;
      goto alg1;
    }
  a = a % nrows;
  if (a < 0)
    a +=nrows;
  b = b % ncols;
  if (b < 0)
    b +=ncols;
  if (0 == b)
    if (0 == a)
      goto nothingtodo;
    else
    {
      dim = ncols * nrows;
      pos = a * ncols;
      goto alg1;
    }
    newrow = a * ncols;
    newcol = b;
    dim = nrows * ncols;
    MAKEDESC(mvt_desc, 1, C_MATRIX, type);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT((*mvt_desc),nrows,C_MATRIX) = nrows;
    L_MVT((*mvt_desc),ncols,C_MATRIX) = ncols;
    GET_HEAP(dim,A_MVT((*mvt_desc),ptdv,C_MATRIX));
    RES_HEAP;
    ipnew = (int *) R_MVT((*mvt_desc),ptdv,C_MATRIX);
    ip    = (int *) R_MVT(DESC(c),ptdv,C_MATRIX);
    for (count = 0; count < dim; count++)
    {
      ipnew[newrow + newcol] = ip[count];
      newcol++;
      if (newcol == ncols) newcol = 0;
      else if (newcol == b) 
      {
        newrow += ncols;
        if (newrow == dim)
          newrow = 0;
      }
    }
    REL_HEAP;
    goto success;

alg1:
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
  L_MVT((*mvt_desc),nrows,C_MATRIX) = nrows;
  L_MVT((*mvt_desc),ncols,C_MATRIX) = ncols;
  GET_HEAP(dim,A_MVT((*mvt_desc),ptdv,C_MATRIX));
  RES_HEAP;
  ipnew = (int *) R_MVT((*mvt_desc),ptdv,C_MATRIX);
  ip    = (int *) R_MVT(DESC(c),ptdv,C_MATRIX);
    for (count = 0; count < dim; count++) {
      ipnew[pos] = ip[count];
      ++pos;
      if (pos == dim) pos = 0;
    }
  REL_HEAP;
  goto success;

nothingtodo:
  INC_REFCNT(c);
  DBUG_RETURN(c);
success:
  DBUG_RETURN(mvt_desc);
}


T_PTD t_mrotate_r(int a, int b, T_PTD c, int type)
{
  register int dim, count;
  int rowrotation, colrotation, nrows, ncols, pos, newrow, newcol;
  double *ipnew,*ip;

  DBUG_ENTER("t_mrotate_r");
  a = -a;
  nrows = R_MVT(DESC(c),nrows,C_MATRIX);
  ncols = R_MVT(DESC(c),ncols,C_MATRIX);
  if (ncols == 1) 
    if (0 == (a = a % nrows))
      goto nothingtodo;
    else 
    {
      dim = nrows;
      if (a > 0)
        pos = a;
      else
        pos = dim + a;
      goto alg1;
    }
  b = -b;
  if (1 == nrows)
    if (0 == (a % ncols))
      goto nothingtodo;
    else
    {
      dim = ncols;
      if (b > 0)
        pos = b;
      else
        pos = dim + b;
      goto alg1;
    }
  a = a % nrows;
  if (a < 0)
    a +=nrows;
  b = b % ncols;
  if (b < 0)
    b +=ncols;
  if (0 == b)
    if (0 == a)
      goto nothingtodo;
    else
    {
      dim = ncols * nrows;
      pos = a * ncols;
      goto alg1;
    }
    newrow = a * ncols;
    newcol = b;
    dim = nrows * ncols;
    MAKEDESC(mvt_desc, 1, C_MATRIX, type);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT((*mvt_desc),nrows,C_MATRIX) = nrows;
    L_MVT((*mvt_desc),ncols,C_MATRIX) = ncols;
    GET_HEAP(dim * sizeof(double),A_MVT((*mvt_desc),ptdv,C_MATRIX));
    RES_HEAP;
    ipnew = (double *) R_MVT((*mvt_desc),ptdv,C_MATRIX);
    ip    = (double *) R_MVT(DESC(c),ptdv,C_MATRIX);
    for (count = 0; count < dim; count++)
    {
      ipnew[newrow + newcol] = ip[count];
      newcol++;
      if (newcol == ncols) newcol = 0;
      else if (newcol == b) 
      {
        newrow += ncols;
        if (newrow == dim)
          newrow = 0;
      }
    }
    REL_HEAP;
    goto success;

alg1:
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
  L_MVT((*mvt_desc),nrows,C_MATRIX) = nrows;
  L_MVT((*mvt_desc),ncols,C_MATRIX) = ncols;
  GET_HEAP(dim,A_MVT((*mvt_desc),ptdv,C_MATRIX));
  RES_HEAP;
  ipnew = (double *) R_MVT((*mvt_desc),ptdv,C_MATRIX);
  ip    = (double *) R_MVT(DESC(c),ptdv,C_MATRIX);
    for (count = 0; count < dim; count++) {
      ipnew[pos] = ip[count];
      ++pos;
      if (pos == dim) pos = 0;
    }
  REL_HEAP;
  goto success;

nothingtodo:
  INC_REFCNT(c);
  DBUG_RETURN(c);
success:
  DBUG_RETURN(mvt_desc);
}


T_PTD t_mreplace(int a, int b, int c, T_PTD d, int type)
{
  register int rows, cols, i, dim;
  DBUG_ENTER("t_mreplace");
  rows = R_MVT(DESC(d), nrows, C_MATRIX);
  cols = R_MVT(DESC(d), ncols, C_MATRIX);
  if ((a > rows) || (b > cols))
    controlled_exit("dimensions don't fit in t_mreplace");
  dim = rows * cols;
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(DESC(d), ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = rows;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = cols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_MATRIX));
  for (i = 0; i < dim; i++)
    L_MVT(*mvt_desc, ptdv, C_MATRIX)[i] = R_MVT(DESC(d), ptdv, C_MATRIX)[i];
  switch(type)
  {
    case TY_INTEGER:
      L_MVT(*mvt_desc, ptdv, C_MATRIX)[(a - 1) * cols + b - 1] = c;
      break;
    case TY_BOOL:
      L_MVT(*mvt_desc, ptdv, C_MATRIX)[(a - 1) * cols + b - 1] = (SA_FALSE == c ?
                                                                   FALSE : TRUE);
      break;
  }
  DBUG_RETURN(mvt_desc);
}


T_PTD t_mreplace_real(int a, int b, double c, T_PTD d, int type)
{
  register int rows, cols, i, dim;
  double *imvt, *amvt;
  DBUG_ENTER("t_mreplace_real");
  rows = R_MVT(DESC(d), nrows, C_MATRIX);
  cols = R_MVT(DESC(d), ncols, C_MATRIX);
  if ((a > rows) || (b > cols))
    controlled_exit("dimensions don't fit in t_mreplace");
  dim = rows * cols;
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(DESC(d), ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = rows;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = cols;
  GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
  amvt = (double *)R_MVT(DESC(d), ptdv, C_MATRIX);
  imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i];
  imvt[(a - 1) * cols + b - 1] = c;
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_not_bmat(T_PTD a, int class)
{
  int arows, acols, i, dim, *imvt, *amvt;
  DBUG_ENTER("t_not_bmat");
  arows = R_MVT(DESC(a), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);

  dim = arows * acols;
  if (0 == dim) 
    MAKEDESC(mvt_desc, 1, class, TY_UNDEF);
  else
    MAKEDESC(mvt_desc, 1, class, TY_BOOL);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  amvt = (int *)R_MVT(DESC(a), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i] ? FALSE : TRUE;
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_and_bmat(T_PTD a, T_PTD b, int class)
{
  int arows, brows, acols, bcols, i, dim, *imvt, *amvt, *bmvt;
  DBUG_ENTER("t_and_bmat");
  arows = R_MVT(DESC(a), nrows, class);
  brows = R_MVT(DESC(b), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  bcols = R_MVT(DESC(b), ncols, class);

  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, TY_BOOL);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  amvt = (int *)R_MVT(DESC(a), ptdv, class);
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i] && bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_or_bmat(T_PTD a, T_PTD b, int class)
{
  int arows, brows, acols, bcols, i, dim, *imvt, *amvt, *bmvt;
  DBUG_ENTER("t_or_bmat");
  arows = R_MVT(DESC(a), nrows, class);
  brows = R_MVT(DESC(b), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  bcols = R_MVT(DESC(b), ncols, class);

  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, TY_BOOL);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  amvt = (int *)R_MVT(DESC(a), ptdv, class);
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i] || bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_xor_bmat(T_PTD a, T_PTD b, int class)
{
  int arows, brows, acols, bcols, i, dim, *imvt, *amvt, *bmvt;
  DBUG_ENTER("t_xor_bmat");
  arows = R_MVT(DESC(a), nrows, class);
  brows = R_MVT(DESC(b), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  bcols = R_MVT(DESC(b), ncols, class);

  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, TY_BOOL);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  amvt = (int *)R_MVT(DESC(a), ptdv, class);
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = amvt[i] != bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_b_xor_bmat(int a, T_PTD b, int class)
{
  int brows, bcols, i, dim, *imvt, *bmvt, arg;
  DBUG_ENTER("t_b_xor_bmat");
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);

  dim = brows * bcols;
  arg = (SA_FALSE == a) ? FALSE : TRUE;
  MAKEDESC(mvt_desc, 1, class, TY_BOOL);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = brows;
  L_MVT(*mvt_desc, ncols, class) = bcols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = (arg != bmvt[i]);
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_b_and_bmat(int a, T_PTD b, int class)
{
  int brows, bcols, i, dim, *imvt, *bmvt, arg;
  DBUG_ENTER("t_b_and_bmat");
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);

  dim = brows * bcols;
  arg = (SA_FALSE == a) ? FALSE : TRUE;
  MAKEDESC(mvt_desc, 1, class, TY_BOOL);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = brows;
  L_MVT(*mvt_desc, ncols, class) = bcols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = arg && bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_b_or_bmat(int a, T_PTD b, int class)
{
  int brows, bcols, i, dim, *imvt, *bmvt, arg;
  DBUG_ENTER("t_b_or_bmat");
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);

  dim = brows * bcols;
  arg = (SA_FALSE == a) ? FALSE : TRUE;
  MAKEDESC(mvt_desc, 1, class, TY_BOOL);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = brows;
  L_MVT(*mvt_desc, ncols, class) = bcols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = arg || bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}

T_PTD t_min_mixmi(T_PTD a, T_PTD b, int class, int type)
{
  int arows, brows, acols, bcols, i, dim, *imvt, *amvt, *bmvt;

  DBUG_ENTER("t_min_mixmi");
  arows = R_MVT(DESC(a), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);
  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  amvt = (int *)R_MVT(DESC(a), ptdv, class);
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = (amvt[i] < bmvt[i]) ? amvt[i] : bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}



T_PTD t_min_mrxmi(T_PTD a, T_PTD b, int class, int type)
{
  int arows, brows, acols, bcols, i, dim, *bmvt;
  double *amvt, *imvt;
  DBUG_ENTER("t_min_mrxmi");
  arows = R_MVT(DESC(a), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);
  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim *sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  amvt = (double *)R_MVT(DESC(a), ptdv, class);
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (double *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = (amvt[i] < bmvt[i]) ? amvt[i] : (double)bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_min_mrxmr(T_PTD a, T_PTD b, int class, int type)
{
  int arows, brows, acols, bcols, i, dim;
  double *amvt, *bmvt, *imvt;
  DBUG_ENTER("t_min_mrxmi");
  arows = R_MVT(DESC(a), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);
  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim *sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  amvt = (double *)R_MVT(DESC(a), ptdv, class);
  bmvt = (double *)R_MVT(DESC(b), ptdv, class);
  imvt = (double *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = (amvt[i] < bmvt[i]) ? amvt[i] : bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_max_mixmi(T_PTD a, T_PTD b, int class, int type)
{
  int arows, brows, acols, bcols, i, dim, *imvt, *amvt, *bmvt;

  DBUG_ENTER("t_max_mixmi");
  arows = R_MVT(DESC(a), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);
  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, class));
  amvt = (int *)R_MVT(DESC(a), ptdv, class);
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (int *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = (amvt[i] > bmvt[i]) ? amvt[i] : bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}



T_PTD t_max_mrxmi(T_PTD a, T_PTD b, int class, int type)
{
  int arows, brows, acols, bcols, i, dim, *bmvt;
  double *amvt, *imvt;
  DBUG_ENTER("t_max_mrxmi");
  arows = R_MVT(DESC(a), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);
  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim *sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  amvt = (double *)R_MVT(DESC(a), ptdv, class);
  bmvt = (int *)R_MVT(DESC(b), ptdv, class);
  imvt = (double *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = (amvt[i] > (double)bmvt[i]) ? amvt[i] : (double)bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_max_mrxmr(T_PTD a, T_PTD b, int class, int type)
{
  int arows, brows, acols, bcols, i, dim;
  double *amvt, *bmvt, *imvt;
  DBUG_ENTER("t_max_mrxmi");
  arows = R_MVT(DESC(a), nrows, class);
  acols = R_MVT(DESC(a), ncols, class);
  brows = R_MVT(DESC(b), nrows, class);
  bcols = R_MVT(DESC(b), ncols, class);
  if ((acols != bcols) || (arows != brows))
    controlled_exit("dimensions don't fit in t_and_bmat");
  dim = arows * acols;
  MAKEDESC(mvt_desc, 1, class, type);
  L_MVT(*mvt_desc, ptdd, class) = NULL;
  L_MVT(*mvt_desc, nrows, class) = arows;
  L_MVT(*mvt_desc, ncols, class) = acols;
  GET_HEAP(dim *sizeof(double), A_MVT(*mvt_desc, ptdv, class));
  amvt = (double *)R_MVT(DESC(a), ptdv, class);
  bmvt = (double *)R_MVT(DESC(b), ptdv, class);
  imvt = (double *)R_MVT(*mvt_desc, ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim; i++)
    imvt[i] = (amvt[i] > bmvt[i]) ? amvt[i] : bmvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_to_mat(int a, int type)
{
  DBUG_ENTER("t_to_mat");
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = 1;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = 1;
  GET_HEAP(1, A_MVT(*mvt_desc, ptdv, C_MATRIX));
  if (type == TY_INTEGER)
    L_MVT(*mvt_desc, ptdv, C_MATRIX)[0] = a;
  else
    L_MVT(*mvt_desc, ptdv, C_MATRIX)[0] = (SA_FALSE == a) ? SA_FALSE : SA_TRUE;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_r_to_mat(double a, int type)
{
  double *rmvt;
  DBUG_ENTER("t_r_to_mat");
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
  L_MVT(*mvt_desc, nrows, C_MATRIX) = 1;
  L_MVT(*mvt_desc, ncols, C_MATRIX) = 1;
  GET_HEAP(1 * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
  rmvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
  rmvt[0] = a;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_v_to_mat(T_PTD a, int class, int type)
{
  int *amvt, *imvt, dim, i;
  DBUG_ENTER("t_v_to_mat");
  if (C_TVECTOR == class)
    dim = R_MVT(DESC(a), nrows, class);
  else
    dim = R_MVT(DESC(a), ncols, class);
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
  if (C_TVECTOR == class)
  {
    L_MVT(*mvt_desc, nrows, C_MATRIX) = dim;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = 1;
  }
  else
  {
    L_MVT(*mvt_desc, nrows, C_MATRIX) = 1;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = dim;
  }
  GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_MATRIX));
  imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
  amvt = (int *)R_MVT(DESC(a), ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim ; i++)
    imvt[i] = amvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}


T_PTD t_vr_to_mat(T_PTD a, int class, int type)
{
  double *amvt, *imvt;
  int dim, i;
  DBUG_ENTER("t_vr_to_mat");
  if (C_TVECTOR == class)
    dim = R_MVT(DESC(a), nrows, class);
  else
    dim = R_MVT(DESC(a), ncols, class);
  MAKEDESC(mvt_desc, 1, C_MATRIX, type);
  L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
  if (C_TVECTOR == class)
  {
    L_MVT(*mvt_desc, nrows, C_MATRIX) = dim;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = 1;
  }
  else
  {
    L_MVT(*mvt_desc, nrows, C_MATRIX) = 1;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = dim;
  }
  GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
  imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
  amvt = (double *)R_MVT(DESC(a), ptdv, class);
  RES_HEAP;
  for (i = 0; i < dim ; i++)
    imvt[i] = amvt[i];
  REL_HEAP;
  DBUG_RETURN(mvt_desc);
}

T_PTD t_munite_empty(T_PTD a)
{
  DBUG_ENTER("t_munite_empty");
  INC_REFCNT(a);
  DBUG_RETURN(a);
}

T_PTD t_munite_ii(int a, T_PTD b, T_PTD c, int class_b, int class_c, int restype)
{
  T_PTD help;
  int *bmvt, *cmvt, *imvt;
  int brows, bcols, crows, ccols, dim, nrows, ncols, i, j;
  DBUG_ENTER("t_munite_ii");
  
  brows = R_MVT(DESC(b), nrows, class_b);
  crows = R_MVT(DESC(c), nrows, class_c);
  bcols = R_MVT(DESC(b), ncols, class_b);
  ccols = R_MVT(DESC(c), ncols, class_c);
  if (TY_UNDEF == R_MVT(DESC(b), type, class_b))
    goto nilmat;
  if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
  {
    help = b;
    b = c;
    c = help;
    i = class_b;
    class_b = class_c;
    class_c = i;
    goto nilmat;
  }
  
  bmvt = (int *)R_MVT(DESC(b), ptdv, class_b);
  cmvt = (int *)R_MVT(DESC(c), ptdv, class_c);
  if (1 == a)
  {
    if (bcols != ccols)
      controlled_exit("dimensions don't fit in munite");
    ncols = bcols;
    nrows = brows + crows;
    dim = ncols * nrows;
    MAKEDESC(mvt_desc, 1, C_MATRIX, restype);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
    GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_MATRIX));
    RES_HEAP;
    imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    dim = ncols * brows;
    for (i = 0; i < dim; i++)
      *imvt++ = *bmvt++;
    dim = ncols * crows;
    for (i = 0; i < dim; i++)
      *imvt++ = *cmvt++;
    REL_HEAP;
    goto success;
  } /* if (1 == a) */
  else if (2 == a)
  {
    if (brows != crows)
      controlled_exit("dimensions don't fit in munite");
    ncols = bcols + ccols;
    nrows = brows;
    dim = ncols * nrows;
    MAKEDESC(mvt_desc, 1, C_MATRIX, restype);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
    GET_HEAP(dim, A_MVT(*mvt_desc, ptdv, C_MATRIX));
    RES_HEAP;
    imvt = (int *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    for (i = 0; i < nrows; i++)
    {
      for (j = 0; j < bcols; j++)
        *imvt++ = *bmvt++;
      for ( j = 0; j < ccols; j++)
        *imvt++ = *cmvt++;
    }     
    REL_HEAP;
    goto success;
  } /* if (2 == a) */
  else
    controlled_exit("first arg in munite is not 1 or 2");
nilmat:
/* argument 2 is empty mat */
  switch(class_c)
  {
    case C_MATRIX:
      if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
      {
        INC_REFCNT(_nilmat);
        mvt_desc = _nilmat;
        goto success;
      }
      else
      { 
        INC_REFCNT(c);
        mvt_desc = (PTR_DESCRIPTOR)gen_many_id(C_MATRIX, (PTR_DESCRIPTOR)c);
        goto success;
      }
      break;
    case C_VECTOR:
    case C_TVECTOR:
      if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
      {
        INC_REFCNT(_nilmat);
        mvt_desc = (PTR_DESCRIPTOR)_nilmat;
        goto success;
      }
      else
        mvt_desc = (PTR_DESCRIPTOR)gen_many_id(C_MATRIX, (PTR_DESCRIPTOR)c);
      break;
    default:
      controlled_exit("unknown class in munite");
  }
success:
  DBUG_RETURN(mvt_desc);
}



T_PTD t_munite_ri(int a, T_PTD b, T_PTD c, int class_b, int class_c, int restype)
{
  T_PTD help;
  double *bmvt, *imvt;
  int *cmvt;
  int brows, bcols, crows, ccols, dim, nrows, ncols, i, j;
  DBUG_ENTER("t_munite_ri");
  
  brows = R_MVT(DESC(b), nrows, class_b);
  crows = R_MVT(DESC(c), nrows, class_c);
  bcols = R_MVT(DESC(b), ncols, class_b);
  ccols = R_MVT(DESC(c), ncols, class_c);
  if (TY_UNDEF == R_MVT(DESC(b), type, class_b))
    goto nilmat;
  if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
  {
    help = b;
    b = c;
    c = help;
    i = class_b;
    class_b = class_c;
    class_c = i;
    goto nilmat;
  }
  
  bmvt = (double *)R_MVT(DESC(b), ptdv, class_b);
  cmvt = (int *)R_MVT(DESC(c), ptdv, class_c);
  if (1 == a)
  {
    if (bcols != ccols)
      controlled_exit("dimensions don't fit in munite");
    ncols = bcols;
    nrows = brows + crows;
    dim = ncols * nrows;
    MAKEDESC(mvt_desc, 1, C_MATRIX, restype);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
    GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
    RES_HEAP;
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    dim = ncols * brows;
    for (i = 0; i < dim; i++)
      *imvt++ = *bmvt++;
    dim = ncols * crows;
    for (i = 0; i < dim; i++)
      *imvt++ = (double)*cmvt++;
    REL_HEAP;
    goto success;
  } /* if (1 == a) */
  else if (2 == a)
  {
    if (brows != crows)
      controlled_exit("dimensions don't fit in munite");
    ncols = bcols + ccols;
    nrows = brows;
    dim = ncols * nrows;
    MAKEDESC(mvt_desc, 1, C_MATRIX, restype);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
    GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
    RES_HEAP;
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    for (i = 0; i < nrows; i++)
    {
      for (j = 0; j < bcols; j++)
        *imvt++ = *bmvt++;
      for ( j = 0; j < ccols; j++)
        *imvt++ = (double)*cmvt++;
    }     
    REL_HEAP;
    goto success;
  } /* if (2 == a) */
  else
    controlled_exit("first arg in munite is not 1 or 2");
nilmat:
/* argument 2 is empty mat */
  switch(class_c)
  {
    case C_MATRIX:
      if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
      {
        INC_REFCNT(_nilmat);
        mvt_desc = _nilmat;
        goto success;
      }
      else
      { 
        INC_REFCNT(c);
        mvt_desc = (PTR_DESCRIPTOR)gen_many_id(C_MATRIX, (PTR_DESCRIPTOR)c);
        goto success;
      }
      break;
    case C_VECTOR:
    case C_TVECTOR:
      if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
      {
        INC_REFCNT(_nilmat);
        mvt_desc = (PTR_DESCRIPTOR)_nilmat;
        goto success;
      }
      else
        mvt_desc = (PTR_DESCRIPTOR)gen_many_id(C_MATRIX, (PTR_DESCRIPTOR)c);
      break;
    default:
      controlled_exit("unknown class in munite");
  }
success:
  DBUG_RETURN(mvt_desc);
}


T_PTD t_munite_ir(int a, T_PTD b, T_PTD c, int class_b, int class_c, int restype)
{
  T_PTD help;
  double *cmvt, *imvt;
  int *bmvt;
  int brows, bcols, crows, ccols, dim, nrows, ncols, i, j;
  DBUG_ENTER("t_munite_ir");
  
  brows = R_MVT(DESC(b), nrows, class_b);
  crows = R_MVT(DESC(c), nrows, class_c);
  bcols = R_MVT(DESC(b), ncols, class_b);
  ccols = R_MVT(DESC(c), ncols, class_c);
  if (TY_UNDEF == R_MVT(DESC(b), type, class_b))
    goto nilmat;
  if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
  {
    help = b;
    b = c;
    c = help;
    i = class_b;
    class_b = class_c;
    class_c = i;
    goto nilmat;
  }
  
  bmvt = (int *)R_MVT(DESC(b), ptdv, class_b);
  cmvt = (double *)R_MVT(DESC(c), ptdv, class_c);
  if (1 == a)
  {
    if (bcols != ccols)
      controlled_exit("dimensions don't fit in munite");
    ncols = bcols;
    nrows = brows + crows;
    dim = ncols * nrows;
    MAKEDESC(mvt_desc, 1, C_MATRIX, restype);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
    GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
    RES_HEAP;
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    dim = ncols * brows;
    for (i = 0; i < dim; i++)
      *imvt++ = (double)*bmvt++;
    dim = ncols * crows;
    for (i = 0; i < dim; i++)
      *imvt++ = *cmvt++;
    REL_HEAP;
    goto success;
  } /* if (1 == a) */
  else if (2 == a)
  {
    if (brows != crows)
      controlled_exit("dimensions don't fit in munite");
    ncols = bcols + ccols;
    nrows = brows;
    dim = ncols * nrows;
    MAKEDESC(mvt_desc, 1, C_MATRIX, restype);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
    GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
    RES_HEAP;
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    for (i = 0; i < nrows; i++)
    {
      for (j = 0; j < bcols; j++)
        *imvt++ = (double)*bmvt++;
      for ( j = 0; j < ccols; j++)
        *imvt++ = *cmvt++;
    }     
    REL_HEAP;
    goto success;
  } /* if (2 == a) */
  else
    controlled_exit("first arg in munite is not 1 or 2");
nilmat:
/* argument 2 is empty mat */
  switch(class_c)
  {
    case C_MATRIX:
      if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
      {
        INC_REFCNT(_nilmat);
        mvt_desc = _nilmat;
        goto success;
      }
      else
      { 
        INC_REFCNT(c);
        mvt_desc = (PTR_DESCRIPTOR)gen_many_id(C_MATRIX, (PTR_DESCRIPTOR)c);
        goto success;
      }
      break;
    case C_VECTOR:
    case C_TVECTOR:
      if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
      {
        INC_REFCNT(_nilmat);
        mvt_desc = (PTR_DESCRIPTOR)_nilmat;
        goto success;
      }
      else
        mvt_desc = (PTR_DESCRIPTOR)gen_many_id(C_MATRIX, (PTR_DESCRIPTOR)c);
      break;
    default:
      controlled_exit("unknown class in munite");
  }
success:
  DBUG_RETURN(mvt_desc);
}



T_PTD t_munite_rr(int a, T_PTD b, T_PTD c, int class_b, int class_c, int restype)
{
  T_PTD help;
  double *bmvt, *cmvt, *imvt;
  int brows, bcols, crows, ccols, dim, nrows, ncols, i, j;
  DBUG_ENTER("t_munite_rr");
  
  brows = R_MVT(DESC(b), nrows, class_b);
  crows = R_MVT(DESC(c), nrows, class_c);
  bcols = R_MVT(DESC(b), ncols, class_b);
  ccols = R_MVT(DESC(c), ncols, class_c);
  if (TY_UNDEF == R_MVT(DESC(b), type, class_b))
    goto nilmat;
  if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
  {
    help = b;
    b = c;
    c = help;
    i = class_b;
    class_b = class_c;
    class_c = i;
    goto nilmat;
  }
  
  bmvt = (double *)R_MVT(DESC(b), ptdv, class_b);
  cmvt = (double *)R_MVT(DESC(c), ptdv, class_c);
  if (1 == a)
  {
    if (bcols != ccols)
      controlled_exit("dimensions don't fit in munite");
    ncols = bcols;
    nrows = brows + crows;
    dim = ncols * nrows;
    MAKEDESC(mvt_desc, 1, C_MATRIX, restype);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
    GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
    RES_HEAP;
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    dim = ncols * brows;
    for (i = 0; i < dim; i++)
      *imvt++ = *bmvt++;
    dim = ncols * crows;
    for (i = 0; i < dim; i++)
      *imvt++ = *cmvt++;
    REL_HEAP;
    goto success;
  } /* if (1 == a) */
  else if (2 == a)
  {
    if (brows != crows)
      controlled_exit("dimensions don't fit in munite");
    ncols = bcols + ccols;
    nrows = brows;
    dim = ncols * nrows;
    MAKEDESC(mvt_desc, 1, C_MATRIX, restype);
    L_MVT(*mvt_desc, ptdd, C_MATRIX) = NULL;
    L_MVT(*mvt_desc, nrows, C_MATRIX) = nrows;
    L_MVT(*mvt_desc, ncols, C_MATRIX) = ncols;
    GET_HEAP(dim * sizeof(double), A_MVT(*mvt_desc, ptdv, C_MATRIX));
    RES_HEAP;
    imvt = (double *)R_MVT(*mvt_desc, ptdv, C_MATRIX);
    for (i = 0; i < nrows; i++)
    {
      for (j = 0; j < bcols; j++)
        *imvt++ = *bmvt++;
      for ( j = 0; j < ccols; j++)
        *imvt++ = *cmvt++;
    }     
    REL_HEAP;
    goto success;
  } /* if (2 == a) */
  else
    controlled_exit("first arg in munite is not 1 or 2");
nilmat:
/* argument 2 is empty mat */
  switch(class_c)
  {
    case C_MATRIX:
      if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
      {
        INC_REFCNT(_nilmat);
        mvt_desc = _nilmat;
        goto success;
      }
      else
      { 
        INC_REFCNT(c);
        mvt_desc = (PTR_DESCRIPTOR)gen_many_id(C_MATRIX, (PTR_DESCRIPTOR)c);
        goto success;
      }
      break;
    case C_VECTOR:
    case C_TVECTOR:
      if (TY_UNDEF == R_MVT(DESC(c), type, class_c))
      {
        INC_REFCNT(_nilmat);
        mvt_desc = (PTR_DESCRIPTOR)_nilmat;
        goto success;
      }
      else
        mvt_desc = (PTR_DESCRIPTOR)gen_many_id(C_MATRIX, (PTR_DESCRIPTOR)c);
      break;
    default:
      controlled_exit("unknown class in munite");
  }
success:
  DBUG_RETURN(mvt_desc);
}
