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
extern PTR_DESCRIPTOR _nilmat; 
extern PTR_DESCRIPTOR _nilstring;

int newdesc(), post_mortem(), newheap(), controlled_exit();

PTR_DESCRIPTOR l_desc;
PTR_DESCRIPTOR h_desc;
  
extern int AsciiOutput, KiROutput, withtilde, number_of_descs;
#define DESC(x)  (* (PTR_DESCRIPTOR) x)

T_PTD t_reverse(T_PTD a, int type)
{
  int i, dim;
  PTR_HEAPELEM pth;
  DBUG_ENTER("t_reverse");

  if (0 == R_LIST(DESC(a), dim))
  {
    if (TY_UNDEF == type)
      l_desc = _nil;
    else
      l_desc = _nilstring;
    INC_REFCNT(l_desc);
  }
  else
  {
    MAKEDESC(l_desc, 1, C_LIST, type);
    dim = R_LIST(DESC(a), dim);
    L_LIST(*l_desc, dim) = dim; 
    GET_HEAP(dim, A_LIST((*l_desc), ptdv));
    RES_HEAP;
    pth = R_LIST(*l_desc,ptdv);
    for (i = 0; i < dim; i++)
    {
      pth[dim - i - 1] = R_LIST(DESC(a), ptdv)[i];
      T_INC_REFCNT((T_PTD)pth[dim - i - 1]);
    }
    L_LIST((*l_desc), ptdd) = NULL;
    L_LIST((*l_desc), special) = NULL;
    REL_HEAP;
  }
  DBUG_RETURN(l_desc);
}

T_PTD t_lunite(T_PTD a, T_PTD b, int type)
{
  int i, dim, h_dim;
  register PTR_HEAPELEM old;
  register PTR_HEAPELEM pth;
  DBUG_ENTER("t_lunite");

  dim = R_LIST(DESC(a), dim) + R_LIST(DESC(b), dim);

  if (0 == dim)
  {
    if (TY_UNDEF == type)
      l_desc = _nil;
    else
      l_desc = _nilstring;
    INC_REFCNT(l_desc);
  }
  else
  {
    MAKEDESC(l_desc, 1, C_LIST, type);
    L_LIST(*l_desc, dim) = dim; 
    GET_HEAP(dim, A_LIST((*l_desc), ptdv));
    RES_HEAP;
    old = R_LIST(DESC(a), ptdv);
    pth = R_LIST(*l_desc,ptdv);
    h_dim = (int)R_LIST(DESC(a), dim);
    for (i = 0; i < h_dim; i++)
    {
      T_INC_REFCNT((T_PTD)old[0]);
      *pth++ = *old++;
    }
    old = R_LIST(DESC(b), ptdv);
    h_dim = (int)R_LIST(DESC(b), dim);
    for (i = 0; i < h_dim; i++)
    {
      T_INC_REFCNT((T_PTD)old[0]);
      *pth++ = *old++;
    }
    L_LIST((*l_desc), ptdd) = NULL;
    L_LIST((*l_desc), special) = NULL;
    REL_HEAP;
  }
  DBUG_RETURN(l_desc);
}
 
T_PTD t_lselect(int a, T_PTD b)
{
  DBUG_ENTER("t_lselect");
  if ((a < 0) || (a >= (int)R_LIST(DESC(b), dim)))
    controlled_exit("lselect: list too short or index <= 0");
  T_INC_REFCNT((T_PTD)R_LIST(DESC(b), ptdv)[a]);
  DBUG_RETURN((T_PTD)R_LIST(DESC(b), ptdv)[a]);
}

T_PTD t_sselect(int a, T_PTD b)
{
  DBUG_ENTER("s_lselect");
  if ((a < 0) || (a >= (int)R_LIST(DESC(b), dim)))
    controlled_exit("lselect: string too short or index <= 0");
  DBUG_RETURN((T_PTD)gen_ind(a, a, b));
}

T_PTD t_lcut(int a, T_PTD b)
{
  int dim = R_LIST(DESC(b), dim);

  DBUG_ENTER("t_lcut");
  if ((-dim < a) && (a < dim))
    DBUG_RETURN((a >= 0) ? (T_PTD)gen_ind(a, dim - 1,(PTR_DESCRIPTOR)b)     \
                         : (T_PTD)gen_ind(0, dim + a - 1, (PTR_DESCRIPTOR)b));
  if ((-dim == a) || (dim == a))
  {
    INC_REFCNT(_nil);
    DBUG_RETURN(_nil);
  }
  controlled_exit("lcut: list too short");
}

T_PTD t_lrotate(int a, T_PTD b)
{
  int dim = R_LIST(DESC(b), dim);
  int *ipnew, *ip, pos, elem, i;
  
  DBUG_ENTER("t_lrotate");
  if (0 == dim)
  {
    INC_REFCNT(b);
    DBUG_RETURN((b));
  } 
  a = -a;
  if (0 == (a = a % dim))
  {
    INC_REFCNT(b);
    DBUG_RETURN((b));
  }
  MAKEDESC(l_desc, 1, C_LIST, TY_UNDEF);
  L_LIST(*l_desc, dim) = dim;
  if (R_LIST(DESC(b), ptdd) != NULL)
    L_LIST(*l_desc, special) = R_LIST(*R_LIST(DESC(b), ptdd), special);
  else
    L_LIST(*l_desc, special) = R_LIST(DESC(b), special);
  GET_HEAP(dim, A_LIST((*l_desc), ptdv));
  RES_HEAP;
  ipnew = (int *) R_LIST(*l_desc, ptdv);
  ip = (int *) R_LIST(DESC(b), ptdv);
  if (a > 0)
    pos = a;
  else
    pos = dim + a;
  for (i = 0; i < dim; i++)
  {
    ipnew[pos] = elem = ip[i];
    if T_POINTER(elem)
      INC_REFCNT((PTR_DESCRIPTOR)elem);
    pos++;
    if (pos == dim)
      pos = 0;
  }
  REL_HEAP;
  DBUG_RETURN(l_desc);
}

T_PTD my_nr_ln(T_PTD a)
{
  DBUG_ENTER("my_ln");
  if ((int)a > 0)
  {
    if (0 == nr_red_ln((a)))
      f_mkdclos(0, 1, 1, M_LN);
    else
      DBUG_RETURN((T_PTD)_desc);
  }
  else
    controlled_exit("arg of ln <= 0");
  DBUG_RETURN((T_PTD)0);
}

T_PTD my_nr_exp(T_PTD a)
{
  DBUG_ENTER("my_exp");
  if (0 == nr_red_exp((a)))
    f_mkdclos(0, 1, 1, M_EXP);
  else
    DBUG_RETURN((T_PTD)_desc);
}

T_PTD my_ln(T_PTD a)
{
  DBUG_ENTER("my_ln");
  if ((int)a > 0)
  {
    if (0 == red_ln((a)))
      f_mkdclos(0, 1, 1, M_LN);
    else
      DBUG_RETURN((T_PTD)_desc);
  }
  else
    controlled_exit("arg of ln <= 0");
  DBUG_RETURN((T_PTD)0);
}

T_PTD my_exp(T_PTD a)
{
  DBUG_ENTER("my_exp");
  if (0 == red_exp((a)))
    f_mkdclos(0, 1, 1, M_EXP);
  else
    DBUG_RETURN((T_PTD)_desc);
}

T_PTD t_lreplace(int a, T_PTD b, T_PTD c)
{
  int i, dim, *ip, *ipnew, index;

  DBUG_ENTER("t_i_lreplace");
  dim = R_LIST(*c, dim);
  if (TY_STRING == R_LIST(DESC(c), type))
    if (T_POINTER(b))
      if (TY_STRING == R_DESC(DESC(b), type))
        if (1 < R_LIST(DESC(b), dim))
          controlled_exit("attempt to replace a string with more than one element");
  if ((a > 0) && (a <= dim))
  {
    a--;
    T_INC_REFCNT((PTR_DESCRIPTOR)b);
    MAKEDESC(l_desc, 1, C_LIST, TY_UNDEF);
    L_LIST(*l_desc, dim) = dim;
    L_LIST(*l_desc, special) = 0;
    L_LIST(*l_desc, ptdd) = NULL;
    GET_HEAP(dim, A_LIST((*l_desc), ptdv));
    ip    = (int *) R_LIST(DESC(c),ptdv);
    ipnew = (int *) R_LIST(*l_desc,ptdv); 
    RES_HEAP;
    for (i = 0; i < a; i++)
      if T_POINTER((ipnew[i] = ip[i]))
        INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
    ipnew[i++] = (T_HEAPELEM)b;
    for ( ;i < dim; i++)
      if T_POINTER((ipnew[i] = ip[i]))
        INC_REFCNT((PTR_DESCRIPTOR)ip[i]);
    REL_HEAP;
  }
  else
  {
    controlled_exit("lreplace: index out of range");
  }
  DBUG_RETURN((T_PTD)(l_desc));
}


T_PTD t_ltransform(int a, int b, T_PTD c)
{
  register int dim, i, *imvt;
  double *rmvt;
  int type;
  DBUG_ENTER("t_ltransform");
  dim = R_LIST(DESC(c), dim);
  if (a * b != dim)
    controlled_exit("dimensions don't fit in t_ltransform");
  if (0 == dim)
  {
    INC_REFCNT(_nilmat);
    DBUG_RETURN(_nilmat);
  }
  h_desc = (PTR_DESCRIPTOR)R_LIST(DESC(c), ptdv)[0];
  if (T_INT((int)h_desc))
    type = TY_INTEGER;
  else if (T_BOOLEAN((int)h_desc))
    type = TY_BOOL;
  else if (T_POINTER(h_desc))
    if ((C_SCALAR == R_DESC(*h_desc, class)) && (TY_REAL == R_DESC(*h_desc, type)))
      type = TY_REAL;
  else
    controlled_exit("unknown type in t_ltransform");
  MAKEDESC(l_desc, 1, C_MATRIX, type);
  L_MVT(*l_desc, ptdd, C_MATRIX) = NULL;
  L_MVT(*l_desc, nrows, C_MATRIX) = a;
  L_MVT(*l_desc, ncols, C_MATRIX) = b;
  switch(type)
  {
    case TY_INTEGER:
      GET_HEAP(dim, A_MVT(*l_desc, ptdv, C_MATRIX));
      for (i = 0; i< dim; i++)
        L_MVT(*l_desc, ptdv, C_MATRIX)[i] = VAL_INT(R_LIST(DESC(c), ptdv)[i]);
      break;
    case TY_BOOL:
      GET_HEAP(dim, A_MVT(*l_desc, ptdv, C_MATRIX));
      for (i = 0; i< dim; i++)
        L_MVT(*l_desc, ptdv, C_MATRIX)[i] = (R_LIST(DESC(c), ptdv)[i] == SA_FALSE) 
                                                          ? FALSE : TRUE;
      break;
    case TY_REAL:
      GET_HEAP(dim * sizeof(double), A_MVT(*l_desc, ptdv, C_MATRIX));
      rmvt = (double *)R_MVT(*l_desc, ptdv, C_MATRIX);
      RES_HEAP;
      for (i = 0; i< dim; i++)
        rmvt[i] =R_SCALAR(DESC((R_LIST(DESC(c), ptdv)[i])), valr);
      REL_HEAP;
      break;
  }
  DBUG_RETURN(l_desc);
}

T_PTD str_max(T_PTD a, T_PTD b)
{
  int alen, blen, len, diff = 0, i;
  T_PTD res;
  DBUG_ENTER("str_max");
  alen = R_LIST(DESC(a), dim);
  blen = R_LIST(DESC(b), dim);
  len = (alen < blen) ? alen : blen ;
  for (i = 0; i < len; i++)
  { 
    if (R_LIST(DESC(a), ptdv)[i] != R_LIST(DESC(b), ptdv)[i])
    {
      diff = 1;
      break;
    }
  }
  if (0 == diff)
    res = (alen <= blen) ? b : a ;
  else
  {
    if (R_LIST(DESC(a), ptdv)[i] < R_LIST(DESC(b), ptdv)[i])
      res = b;
    else
      res = a;
  }
    
  INC_REFCNT(res);
  DBUG_RETURN(res);
}


T_PTD str_min(T_PTD a, T_PTD b)
{
  int alen, blen, len, diff = 0, i;
  T_PTD res;
  DBUG_ENTER("str_min");
  alen = R_LIST(DESC(a), dim);
  blen = R_LIST(DESC(b), dim);
  len = (alen < blen) ? alen : blen ;
  for (i = 0; i < len; i++)
  { 
    if (R_LIST(DESC(a), ptdv)[i] != R_LIST(DESC(b), ptdv)[i])
    {
      diff = 1;
      break;
    }
  }
  if (0 == diff)
    res = (alen <= blen) ? a : b ;
  else
  {
    if (R_LIST(DESC(a), ptdv)[i] < R_LIST(DESC(b), ptdv)[i])
      res = a;
    else
      res = b;
  }
    
  INC_REFCNT(res);
  DBUG_RETURN(res);
}
