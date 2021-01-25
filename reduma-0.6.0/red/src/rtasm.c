#include <stdio.h>
#include <stdlib.h>
#include "dbug.h"
#include "rstdinc.h"
#include "rstelem.h"
#include "rheapty.h"
#include "rstackty.h"

#define DESC(x)        (* (PTR_DESCRIPTOR) x)

/*******************************************************************************/
/*                                                                             */
/* Encoding of the Output sending back to reduma                               */
/*                                                                             */
/* output : tag data                                                           */
/*                                                                             */
/* with                                                                        */
/*                                                                             */
/* data   : length_of_list output* (in case of ri_list)                        */
/*        : tagged_integer | tagged_boolean  (in case of ri_int or ri_bool)    */
/*        : value_of_real (in case of ri_real)                                 */
/*        : arity arg graph_adr  (in case of ri_case, ri_comb or ri_cond)      */
/*        : act_nomat reason ptsdes guard_body[0] guard_body[1] guard_body[2]  */
/*          guard_body[3]                                                      */
/*        : arity arg (in case of ri_sub)                                      */
/*        : tag arity arg nfv (in case of closure) ...                         */
/*                                                                             */
/* act_nomat : integer                                                         */
/* reason    : integer                                                         */
/* arity     : integer                                                         */
/* arg       : integer                                                         */
/* graph_adr : integer                                                         */
/* guard_body[0] : integer                                                     */
/* guard_body[1] : output                                                      */
/* guard_body[2] : output                                                      */
/* guard_body[3] : output                                                      */
/*                                                                             */
/* tag    : ri_int | ri_real | ri_list | ri_func                               */
/*        | ri_mat | ri_vect | ri_tvect | ri_cons | ri_prim                    */
/*                                                                             */
/*                                                                             */
/*******************************************************************************/

typedef enum {
                ri_bool = 0, ri_int = 1, ri_real = 2, ri_list = 3, ri_clos = 4, 
                ri_mat = 5, ri_vect = 6, ri_tvect = 7, ri_cons = 8, ri_prim = 9,
                ri_comb = 10, ri_cond = 11, ri_expr = 12, ri_case = 13,
                ri_CAVE = 14, ri_nomat = 15, ri_sub = 16, ri_dyncomb = 17,
                ri_char = 18, ri_string = 19, ri_var = 20, ri_const = 21,
                ri_name = 22, ri_constant = 23,
              } RED_INPUT;
/*******************************************************************************/

void load_expr(), trav_e_a(), trav_a_hilf(), DescDump(), res_heap(), rel_heap();
int newdesc(), post_mortem(), newheap(), mvheap();
extern char Tasm2KiR[80];
extern PTR_DESCRIPTOR _desc;
extern PTR_DESCRIPTOR  _nil;
extern PTR_DESCRIPTOR  _nilstring;
T_PTD ptdesc;

FILE *tasm2kir;

int go_get_it(void)
{
  PTR_DESCRIPTOR my_desc;
  int t_signal;
  int return_value, dim, i;
  PTR_HEAPELEM pth; /* pointer to heapelement */

  DBUG_ENTER("go_get_it");
  fscanf(tasm2kir, "%d", &t_signal);
  switch(t_signal)
  {
    case ri_bool:
    case ri_prim:
    case ri_char:
    case ri_constant:
    case ri_int:  fscanf(tasm2kir, "%d", &return_value); break;
    case ri_real: MAKEDESC(my_desc, 1, C_SCALAR, TY_REAL);
                  fscanf(tasm2kir, "%lf", A_SCALAR(*my_desc, valr));
                  return_value = (int)my_desc;
                  break;
    case ri_mat:  
    case ri_vect:
    case ri_tvect:{ int nrows, ncols, type, idata;
                    double rdata, *rv;

                    MAKEDESC(my_desc, 1, C_MATRIX, TY_INTEGER);
                    return_value = (int)my_desc;
                    L_MVT(*my_desc, ptdd, C_MATRIX) = NULL;
                    if (ri_mat == t_signal)
                      L_MVT(*my_desc, class, C_MATRIX) = C_MATRIX;
                    else if (ri_vect == t_signal)
                      L_MVT(*my_desc, class, C_MATRIX) = C_VECTOR;
                    else if (ri_tvect == t_signal)
                      L_MVT(*my_desc, class, C_MATRIX) = C_TVECTOR;
                    fscanf(tasm2kir, "%d %d %d", &type, &nrows, &ncols);
                    if (ri_int == type)
                      L_MVT(*my_desc, type, C_MATRIX) = TY_INTEGER;
                    else if (ri_bool == type)
                      L_MVT(*my_desc, type, C_MATRIX) = TY_BOOL;
                    else if (ri_real == type)
                      L_MVT(*my_desc, type, C_MATRIX) = TY_REAL;
                    L_MVT(*my_desc, nrows, C_MATRIX) = nrows;
                    L_MVT(*my_desc, ncols, C_MATRIX) = ncols;
                    if ((ri_int == type) || (ri_bool == type))
                    {
                      GET_HEAP(nrows * ncols, A_MVT((*my_desc), ptdv, C_MATRIX));
                      for(i = 0; i < nrows * ncols; i++)
                      {
                        fscanf(tasm2kir, "%d", &idata);
                        L_MVT(*my_desc, ptdv, C_MATRIX)[i] = idata; 
                      }
                    }
                    else
                    {
                      GET_HEAP(nrows * ncols * sizeof(double) / sizeof(T_HEAPELEM), 
                          A_MVT(*my_desc, ptdv, C_MATRIX));
                      rv = (double *)R_MVT(*my_desc, ptdv, C_MATRIX);
                      for(i = 0; i < nrows * ncols; i++)
                      {
                        fscanf(tasm2kir, "%lf", &rdata);
                        rv[i] = rdata;
                      }
                      L_MVT(*my_desc, ptdv, C_MATRIX) = (PTR_HEAPELEM)rv;
                    } 
                    break;
                  }
    case ri_string: fscanf(tasm2kir, "%d", &dim);
                    MAKEDESC(my_desc, 1, C_LIST, TY_STRING);
                    L_LIST(*my_desc, ptdd) = NULL;
                    return_value = (int)my_desc;
                    L_LIST((*my_desc), dim) = dim;
                    if (dim > 0)
                    {
                      GET_HEAP(dim, A_LIST((*my_desc), ptdv));
                      pth = R_LIST(*my_desc,ptdv);
                      for (i = 0; i < dim; i++)
                        pth[i] = go_get_it();
                    }
                  break;
    case ri_cons: fscanf(tasm2kir, "%d", &dim);
                  if (0 == dim) return_value = (int)_nil;       
                  else 
                  {
                    MAKEDESC(my_desc, 1, C_LIST, TY_UNDEF);
                    L_LIST(*my_desc, ptdd) = NULL;
                    return_value = (int)my_desc;
                    L_LIST((*my_desc), dim) = dim;
                    L_LIST((*my_desc), special) = 0;
                    if (dim > 0)
                    {
                      GET_HEAP(dim, A_LIST((*my_desc), ptdv));
                      pth = R_LIST(*my_desc,ptdv);
                      for (i = 0; i < dim; i++)
                        pth[i] = go_get_it();
                    }
                    ptdesc = (T_PTD)R_LIST(*my_desc, ptdv)[0];
                    pth = R_LIST(*ptdesc, ptdv);
                    dim = R_LIST(*my_desc, dim);
                    pth[1] |= SET_EDIT(-99);
                    pth[dim -1] |= SET_EDIT(-99);
                    if (4 == dim)
                      pth[2] |= SET_EDIT(2);
                  }
                  break;
    case ri_list: fscanf(tasm2kir, "%d", &dim);
                  if (0 == dim) return_value = (int)_nil;       
                  else 
                  {
                    MAKEDESC(my_desc, 1, C_LIST, TY_UNDEF);
                    L_LIST(*my_desc, ptdd) = NULL;
                    return_value = (int)my_desc;
                    L_LIST((*my_desc), dim) = dim;
                    if (dim > 0)
                    {
                      GET_HEAP(dim, A_LIST((*my_desc), ptdv));
                      pth = R_LIST(*my_desc,ptdv);
                      for (i = 0; i < dim; i++)
                        pth[i] = go_get_it();
                    }
                  }
                  break;
    case ri_comb: {
                    int arity; /* used args */
                    int arg;   /* needed args */
                    int graph_adr; /* zeiger auf letrecdescr. */

                    fscanf(tasm2kir, " %d %d %d", &arity, &arg, &graph_adr);
                    MAKEDESC(my_desc,1,C_FUNC,TY_COMB);
                    return_value = (int)my_desc;
                    L_COMB(*my_desc, nargs) = arity;
                    L_COMB(*my_desc, args) = arg;
                    L_COMB(*my_desc, ptd) = (T_PTD)graph_adr;
                    break;
                  }
    case ri_dyncomb: {
                    int arity; /* used args */
                    int arg;   /* needed args */

                    fscanf(tasm2kir, " %d %d", &arity, &arg);
                    MAKEDESC(my_desc,1,C_FUNC,TY_COMB);
                    return_value = (int)my_desc;
                    L_COMB(*my_desc, nargs) = arity;
                    L_COMB(*my_desc, args) = arg;
                    L_COMB(*my_desc, ptd) = (T_PTD)go_get_it();
                    break;
                  }
#if WITHTILDE                  
    case ri_case: {
                    int arity; /* used args */
                    int arg;   /* needed args */
                    int graph_adr; /* zeiger auf letrecdescr. */

                    fscanf(tasm2kir, " %d %d %d", &arity, &arg, &graph_adr);
                    MAKEDESC(my_desc,1,C_FUNC,TY_CASE);
                    return_value = (int)my_desc;
                    L_COMB(*my_desc, nargs) = arity;
                    L_COMB(*my_desc, args) = arg;
                    L_COMB(*my_desc, ptd) = (T_PTD)graph_adr;
                    break;
                  }
    case ri_nomat:{
                    int act_nomat, reason, ptsdes, len;

                    fscanf(tasm2kir, " %d %d %d %d", &act_nomat, &reason, &ptsdes, &len);
                    MAKEDESC(my_desc, 1, C_EXPRESSION, TY_NOMAT);
                    return_value = (int)my_desc;
                    L_NOMAT(*my_desc, act_nomat) = act_nomat;
                    GET_HEAP(4, A_NOMAT(*my_desc, guard_body));
                    L_NOMAT(*my_desc, reason) = reason;
                    L_NOMAT(*my_desc, ptsdes) = (T_PTD)ptsdes;
                    L_NOMAT(*my_desc, guard_body)[0] = len;
                    L_NOMAT(*my_desc, guard_body)[1] = go_get_it(); /* guard */ 
                    L_NOMAT(*my_desc, guard_body)[2] = go_get_it(); /* closure */
                    L_NOMAT(*my_desc, guard_body)[3] = go_get_it(); /* arg from match */
                    ptdesc = (T_PTD)R_SWITCH(*(T_PTD)ptsdes, ptse)[act_nomat];
                    load_expr(A_MATCH(*ptdesc, body));
                    trav_e_a();
                    trav_a_hilf();
                    if (NULL == mvheap(A_FUNC(*ptdesc, pte))) 
                      post_mortem("rtasm.c: go_get_it(): Heap out of space");
                    break;
                  }
#endif
    case ri_cond: {
                    int arity; /* used args */
                    int arg;   /* needed args */
                    int graph_adr; /* zeiger auf letrecdescr. */

                    fscanf(tasm2kir, " %d %d %d", &arity, &arg, &graph_adr);
                    MAKEDESC(my_desc, 1, C_FUNC, TY_CONDI);
                    return_value = (int)my_desc;
                    L_CONDI(*my_desc, nargs) = arity;
                    L_CONDI(*my_desc, args) = arg;
                    L_CONDI(*my_desc, ptd) = (T_PTD)graph_adr;
                    break;
                  }
    case ri_const: {
                    int a;
                    fscanf(tasm2kir, " %d", &a);               /* reading ri_var */ 
                    MAKEDESC(my_desc, 1, C_CONSTANT, TY_VAR);
                    return_value = (int)my_desc;
                    fscanf(tasm2kir, " %d", &a);
                    L_VAR(*my_desc, nlabar) = a;
                    L_VAR(*my_desc, ptnd) = (T_PTD)go_get_it();
                    break;
                  }
    case ri_expr: {
                    
                    int arity; /* used args + 1 */
                    int arg; 
                    int what_is_it; /* what kind of expression */
                    
                    fscanf(tasm2kir, " %d", &what_is_it);
                    switch(what_is_it)
                    {
#if WITHTILDE

                      case ri_name:
                      {
                        fscanf(tasm2kir, " %d", &arg); /* # of following elements */
                        MAKEDESC(my_desc, 1, C_EXPRESSION, TY_NAME);
                        return_value = (int)my_desc;
                        GET_HEAP(arg + 1, A_NAME(*my_desc, ptn));
                        pth = R_NAME(*my_desc, ptn);
                        *pth++ = arg;
                        RES_HEAP;
                        for (i = 0; i < arg; i++)
                        {
                          fscanf(tasm2kir, " %d", &arity);
                          pth[i] = arity;
                        }
                        REL_HEAP;
                        break;
                      }
                      
                      case ri_expr:
                      {
                        
                        MAKEDESC(my_desc, 1, C_EXPRESSION, TY_EXPR);
                        return_value = (int)my_desc;
                        fscanf(tasm2kir, " %d %d", &arity, &arg);
                        GET_HEAP(arity + 1, (int)A_EXPR(*my_desc, pte));
                        pth = (PTR_HEAPELEM)R_EXPR(*my_desc, pte);
                        *pth++ = arity;
                        *pth++ = arg;
                        for (i = 0; i < arity - 1; i++)
                          pth[i] = go_get_it();
                        break;
                      }
                      case ri_nomat:
                      {
                        int act_nomat, reason, ptsdes;
                        int guard_body_0, guard_body_1, guard_body_2;
                        fscanf(tasm2kir, " %d %d %d %d", &act_nomat, &reason,
                                                         &ptsdes, &guard_body_0);
                        fscanf(tasm2kir, " %d %d", &guard_body_1, &guard_body_2);
                        MAKEDESC(my_desc, 1, C_EXPRESSION, TY_NOMAT);
                        return_value = (int)my_desc;
                        L_NOMAT(*my_desc, act_nomat) = act_nomat;
                        L_NOMAT(*my_desc, reason) = reason;
                        L_NOMAT(*my_desc, ptsdes) = (T_PTD)ptsdes;
                        GET_HEAP(4,A_NOMAT(*my_desc, guard_body)); 
                        L_NOMAT(*my_desc, guard_body)[0] = guard_body_0;
                        L_NOMAT(*my_desc, guard_body)[1] = guard_body_1;
                        L_NOMAT(*my_desc, guard_body)[2] = guard_body_2;
                        L_NOMAT(*my_desc, guard_body)[3] = go_get_it();
                        break;
                      }
                      break;
#endif
                      case ri_sub:
                      {
                        fscanf(tasm2kir, " %d %d", &arity, &arg);
                        MAKEDESC(my_desc, 1, C_EXPRESSION, TY_SUB);
                        return_value = (int)my_desc;
                        GET_HEAP(arity + 1, (int)A_EXPR(*my_desc, pte));
                        pth = (PTR_HEAPELEM)R_EXPR(*my_desc,pte);
                        *pth++ = arity;
                        *pth++ = arg;
                        for (i = 0; i < arity - 1; i++)
                          pth[i] = go_get_it();
                        break;
                      }
                  }
                  break;
                }

    case ri_clos: {
                    int arity; /* used args */
                    int arg;   /* needed args */
                    int primf; /* the tagged function itself */
                    int what_is_it; /* what kind of closure */
                    int j;     /* only for to count */

                    fscanf(tasm2kir, " %d", &what_is_it);
                    switch(what_is_it)
                    {
#if WITHTILDE
                      case ri_nomat:
                      {
                        int nfv, ptsdes, failed_when, i;
                        T_PTD ptd_nomat, ptd_sub, ptd_comb, ptd;

                        fscanf(tasm2kir, " %d %d %d %d %d", 
                                           &arity, &arg, &nfv, &ptsdes, &failed_when);
                        MAKEDESC(my_desc,2,C_FUNC,TY_CLOS);
                        GET_HEAP(arity + nfv + 1, A_CLOS(*my_desc,pta));
                        L_CLOS(*my_desc,args) = arity;
                        L_CLOS(*my_desc,nargs) = arg;
                        L_CLOS(*my_desc,nfv) = nfv;
                        L_CLOS(*my_desc,ftype) = TY_NOMATBODY;
                        
                        MAKEDESC(ptd_nomat, 1, C_EXPRESSION, TY_NOMAT);
                        return_value = (int)ptd_nomat;
                        L_NOMAT(*ptd_nomat, act_nomat) = failed_when;
                        L_NOMAT(*ptd_nomat, ptsdes) = (T_PTD)ptsdes;
                        GET_HEAP(4, A_NOMAT(*ptd_nomat, guard_body));
                        L_NOMAT(*ptd_nomat, guard_body)[0] = 3;   /* 3 following elems */
                        L_NOMAT(*ptd_nomat, guard_body)[2] = (int)my_desc;/* ptr to clos */
                        L_NOMAT(*ptd_nomat, guard_body)[1] = go_get_it();  /* guard */
                        L_NOMAT(*ptd_nomat, guard_body)[3] = go_get_it();  /* match */
                        L_NOMAT(*ptd_nomat, reason) = 3; /* guard neither true nor false*/

                        MAKEDESC(ptd_comb, 2, C_FUNC, TY_COMB);
                        L_COMB(*ptd_comb, args) = 0;
                        L_COMB(*ptd_comb, nargs) = arity;
                        L_COMB(*ptd_comb, ptc) = (INSTR *)0;
                        L_CLOS(*my_desc, pta)[0] = (int)ptd_comb; /* put the COMB 
                                                                desc into the CLOS desc */

                        MAKEDESC(ptd_sub, 1, C_EXPRESSION, TY_SUB);
                        L_COMB(*ptd_comb, ptd) = ptd_sub; /* put the SUB desc into the 
                                                             COMB desc */
                        L_FUNC(*ptd_sub, nargs) = arity;
                        L_FUNC(*ptd_sub, special) = 0;
                        L_FUNC(*ptd_sub, namelist) = 0;
/* get the expression pointed to by ptd */
                        ptd = (T_PTD)R_SWITCH(*(T_PTD)ptsdes, ptse)[failed_when];
                        load_expr(A_MATCH(*ptd, body));
                        trav_e_a();
                        trav_a_hilf();
                        if (NULL == mvheap(A_FUNC(*ptd_sub, pte)))
                          post_mortem("rtasm.c (go_get_it()): Heap out of space");
/* now build the rest of the arg.vector in CLOSURE */
                        pth = (PTR_HEAPELEM)R_CLOS(*my_desc, pta);
                        pth++;
                        for (i = 0; i < arity + nfv; i++)
                          pth[i] = go_get_it();
                        break;
                      }
#endif                      
                      case ri_comb:
                      {
                        int nfv;

                        fscanf(tasm2kir, " %d %d %d", &arity, &arg, &nfv);
                        MAKEDESC(my_desc,1,C_FUNC,TY_CLOS);
                        return_value = (int)my_desc;
                        GET_HEAP(arity + nfv + 1, A_CLOS(*my_desc,pta));
                        L_CLOS(*my_desc,args) = arity;
                        L_CLOS(*my_desc,nargs) = arg;
#if WITHTILDE
                        L_CLOS(*my_desc,nfv) = nfv;
#endif
                        L_CLOS(*my_desc,ftype) = TY_COMB;
                        pth = (PTR_HEAPELEM)R_CLOS(*my_desc, pta);
                        j = arity + nfv + 1;
                        for (i = 0; i < j; i++)
                          pth[i] = go_get_it();
                        break;
                      }
                      case ri_case:
                      { 
                        int nfv;

                        fscanf(tasm2kir, " %d %d %d", &arity, &arg, &nfv);
                        MAKEDESC(my_desc,1,C_FUNC,TY_CLOS);
                        return_value = (int)my_desc;
                        GET_HEAP(arity + nfv + 1, A_CLOS(*my_desc,pta));
                        L_CLOS(*my_desc,args) = arity;
                        L_CLOS(*my_desc,nargs) = arg;
#ifdef WITHTILDE
                        L_CLOS(*my_desc,nfv) = nfv;
#endif
                        L_CLOS(*my_desc,ftype) = TY_CASE;
                        pth = (PTR_HEAPELEM)R_CLOS(*my_desc,pta);
                        j = arity + nfv + 1;
                        for (i = 0; i < j; i++)
                          pth[i] = go_get_it();
                        break;
                      }

                      case ri_cond:
                      {
                        int nfv;
     
                        fscanf(tasm2kir, " %d %d %d", &arity, &arg, &nfv);
                        MAKEDESC(my_desc,1,C_FUNC,TY_CLOS);
                        return_value = (int)my_desc;
                        GET_HEAP(arity + nfv + 1, A_CLOS(*my_desc,pta));
                        L_CLOS(*my_desc,args) = arity;
                        L_CLOS(*my_desc,nargs) = arg;
#ifdef WITHTILDE
                        L_CLOS(*my_desc,nfv) = nfv;
#endif
                        L_CLOS(*my_desc,ftype) = TY_CONDI;
                        pth = (PTR_HEAPELEM)R_CLOS(*my_desc,pta);
                        j = arity + nfv + 1;
                        for (i = 0; i < j; i++)
                          pth[i] = go_get_it();
                        break;
                      }
                      
                      case ri_prim:
                      {
                        fscanf(tasm2kir, " %d %d %d", &primf, &arity, &arg);
                        MAKEDESC(my_desc,1,C_FUNC,TY_CLOS);
                        return_value = (int)my_desc;
                        GET_HEAP(arity + 1, A_CLOS(*my_desc,pta));
                        L_CLOS(*my_desc,args) = arity;
                        L_CLOS(*my_desc,nargs) = arg;
                        L_CLOS(*my_desc,ftype) = TY_PRIM;
                        pth = (PTR_HEAPELEM)R_CLOS(*my_desc,pta);
                        *pth++ = primf;
                        for (i = 0; i < arity - arg; i++)
                          pth[i] = go_get_it();
                        break;
                      }
                      default:  post_mortem("get_tasm_output: unknown tag");
                    }
                    break;
                  }
                    
    default:  post_mortem("get_tasm_output: unknown tag"); 
  }               
  DBUG_RETURN(return_value);
}

int get_tasm_output(void)
{
  DBUG_ENTER(get_tasm_output);
  if (NULL == (tasm2kir = fopen(Tasm2KiR, "rt")))
    post_mortem("get_tasm_output: unable to open inputfile");
  DBUG_RETURN(go_get_it());
}
