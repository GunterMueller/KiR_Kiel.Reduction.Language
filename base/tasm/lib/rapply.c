#define WITHTILDE 1
#include "rprimf.h"

#define PROLOG_EXTERN
#include "prolog.h"
#include "rmkclos.h"

/* rts() only for popfree_t */
#define rts()          DBUG_RETURN(kill_bits(pop(r)))

extern int call_func;


extern int lab;
extern int red_cnt;

#ifdef __sparc__
#if (0==NO_STACK_REG)
  register int *st_h asm ("g5");
  register int *st_w asm ("g7");
  register int *st_a asm ("g6");
#else
  extern int *st_h;
  extern int *st_w;
  extern int *st_a;
#endif
#else
  extern int *st_h;
  extern int *st_w;
  extern int *st_a;
#endif

extern int *st_t;
extern int *st_r;
extern PTR_DESCRIPTOR funcdesctable[];
extern char *functable[];
extern FCTPTR dealloc_tab[];
extern T_DESCRIPTOR *newdesc();
extern int newheap(int, T_HEAPELEM);
extern FCTPTR red_func_tab[];

int rest_args;

extern int withtilde;
extern int number_of_descs;
extern PTR_DESCRIPTOR _desc;

PTR_HEAPELEM pth; /* pointer to heapelement */
T_PTD ptdesc;     /* pointer to descriptor */
T_PTD help_pt;     /* pointer to descriptor */



/**********************************************************************/

int f_popfree_t(void)
{
  int killvar;
  DBUG_ENTER("f_popfree_t");
  
  killvar = pop(r);
  while (killvar > 0) 
  {
    ptdesc = (T_PTD)pop(t);
    T_DEC_REFCNT(ptdesc);
    killvar--;
  }
  rts();
}  

/**********************************************************************/
/* calls a primitive function from the reduma runtime library         */
/* n        : number of actual parameters (fits the needed parameters)*/ 
/* d_ptdesc : the primitive function to call                          */
/**********************************************************************/

void delta_reduct(int n, T_PTD d_ptdesc)
{
  int delta_ok = 1;

  DBUG_ENTER("delta_reduct");
  
  drop(w, 1);
  switch(n)
  {
  case 1:
  {
    if (!(*(red_func_tab[FUNC_INDEX((int)d_ptdesc)]))(top(w)))
    {
      f_mkdclos(-1, 1, 1, (int)d_ptdesc);
      delta_ok = 0;
    }
    break;
  }
  case 2:
  {
    if (!(*(red_func_tab[FUNC_INDEX((int)d_ptdesc)]))(top(w), peek(w,1)))
    {
      f_mkdclos(-1, 2, 2, (int)d_ptdesc);
      delta_ok = 0;
    }
    break;
  }
  case 3:
  {
    if (!(*(red_func_tab[FUNC_INDEX((int)d_ptdesc)]))(top(w), peek(w,1), \
                                                    peek(w,2)))
    {
      f_mkdclos(-1, 3, 3, (int)d_ptdesc);
      delta_ok = 0;
    }
    break;
  }
  case 4:
  {
    if (!(*(red_func_tab[FUNC_INDEX((int)d_ptdesc)]))(top(w), peek(w,1), \
                                                    peek(w,2), peek(w,3)))
    {
      f_mkdclos(-1, 4, 4, (int)d_ptdesc);
      delta_ok = 0;
    }
    break;
  }
  default:
  {
    fprintf(stderr, "f_apply: pf_arity out of range (apply)");
    exit(1);
  }
  } /* END switch(n) */
  
  if (delta_ok)
  {
    drop(w, n);
    push((int)_desc);
    _desc = NULL;
  }
  DBUG_VOID_RETURN;
}

/**********************************************************************/
/* n    : actual args  (on top of stack R)                            */
/*        return label (second on Stack R)                            */
/*        return function (third on Stack R)                          */
/* result : NULL if no (gamma)beta call, else address of function     */
/*          to call                                                   */
/* if func is a userdefined function, it is always a closure, even if */
/* there haven't been any actual arguments (closure with no args)     */
/**********************************************************************/

int f_apply(void)
{
  int arg, nfv, sargs;
  int n;

  DBUG_ENTER("f_apply");

    n = pop(r);

LabSwitch:
L0:  
  ptdesc = (T_PTD)top(w);
  
/********************************************************************/
/* test, if PRIM FUNC                            test, if PRIM FUNC */
/********************************************************************/
  if(T_FUNC((int) ptdesc))
  {
    if (0 == red_cnt) 
    { 
      drop(w, 1);
      f_mkdclos(-1, FUNC_ARITY((int)ptdesc), n, (int)ptdesc);
      flip_tail();                      /* avoid stackswitch */
      rtf();
    }
/* ENOUGH ARGUMENTS */          
    if (FUNC_ARITY((int)ptdesc) == n)
    {
      delta_reduct(n, ptdesc);
      flip_tail();                      /* avoid stackswitch */
      rtf();
    } /* END if (FUNC_ARITY((int)ptdesc) == n) */
    else 
/* NOT ENOUGH ARGUMENTS */          
    if (FUNC_ARITY((int)ptdesc) > n)
    {
      drop(w, 1);  /* 'cause f_mkdclos() pushes the function itself */
      f_mkdclos(-1, FUNC_ARITY((int)ptdesc), n, (int)ptdesc);
      flip_tail();                      /* avoid stackswitch */
      rtf();
    } /* END if (FUNC_ARITY((int)ptdesc) < n) */
/* TOO MANY ARGUMENTS */          
    else  /* FUNC_ARITY((int)ptdesc) < n */
    {
too_many_delta:
      if (LSELECT == (int)ptdesc)
      {
        delta_reduct(2, ptdesc);
        n -= 2; /* start a new apply with n - 2 act args */
        goto L0;
      }
      else
      {
        rest_args = n - FUNC_ARITY((int)ptdesc);

        delta_reduct(FUNC_ARITY((int)ptdesc), ptdesc);

        /* auslagern einer applikation vgl. store_e in rstpro.c */
        MAKEDESC(ptdesc, 1, C_EXPRESSION, TY_EXPR);
        GET_HEAP(rest_args + 3, (int)A_EXPR(*ptdesc, pte));
        RES_HEAP;
        pth    = (PTR_HEAPELEM)R_EXPR(*ptdesc, pte);
        *pth++ = (T_HEAPELEM)(rest_args + 2);             /* groesse     */
        *pth++ = (T_HEAPELEM)SET_ARITY(AP, (rest_args + 1)); /* konstruktor */
        pth += rest_args;
        do {
          *pth-- = (T_HEAPELEM)pop(w);
        } while (--rest_args >= 0);
        REL_HEAP;
        push((int)ptdesc);
      }        
      flip_tail();                      /* avoid stackswitch */
      rtf();
    }  /* END FUNC_ARITY((int)ptdesc) > n */
  } /* END if(T_FUNC((int) ptdesc)) */
/********************************************************************/
/* END IF PRIMFUNC                                  END IF PRIMFUNC */
/********************************************************************/

/********************************************************************/
/* test, if INTERACTION                        test, if INTERACTION */
/********************************************************************/
  else if(T_IAFUNC((int) ptdesc))
  {
    if (FUNC_ARITY((int)ptdesc) == n)           /* ENOUGH ARGUMENTS */
    {
      drop(w,1);
      mkintact((int) ptdesc);
      flip_tail();
      rtf();
    }
    else if (FUNC_ARITY((int)ptdesc) > n)   /* NOT ENOUGH ARGUMENTS */
    {
      f_mkiclos(n);
      flip_tail();
      rtf();
    }
    else                                      /* TOO MANY ARGUMENTS */
    {
      drop(w,1);
      mkintact((int) ptdesc);

      n -= FUNC_ARITY((int)ptdesc);
      goto T_mkap;
    }
  }
/********************************************************************/
/* END, if INTERACTION                          END, if INTERACTION */
/********************************************************************/

/********************************************************************/
/* test, if POINTER to closure                     test, if POINTER */
/********************************************************************/
  else if (T_POINTER(ptdesc))
  {
    if (C_FUNC == R_DESC(*ptdesc, class))
    {

/* test, if a closure */
      if (TY_CLOS == R_DESC(*ptdesc, type))
      {
        arg = R_CLOS(*ptdesc, nargs);
/* arg : needed arguments, nfv : free variables */
/********************************************************************/
/* test, if primitive function closure                              */
/********************************************************************/
        if (TY_PRIM == R_CLOS(*ptdesc, ftype))
        {
          if (0 == red_cnt)
          {
            dyn_mkbclos(n);
            flip_tail();                      /* avoid stackswitch */
            rtf();
          }
                                                                
/* ENOUGH ARGUMENTS */          
          if (n == arg)
          {
            nfv = 0;
            sargs = R_CLOS(*ptdesc, args);
            n += sargs;  /* prepare the GOTO L0; to execute the delta_red */
            drop(w, 1);  /* POP function */
            pth = R_CLOS(*ptdesc, pta);
            pth = pth + sargs + nfv;
            while (sargs-- > 0) /* restore the argument frame */
            {
              help_pt = (T_PTD)*pth--;
              T_INC_REFCNT(help_pt);
              push((int)help_pt);
            } 
            
/* NO stflip(a,w); 'cause it's now a DELTA-call */

            help_pt = ptdesc;              
            ptdesc = (T_PTD)*R_CLOS(*ptdesc, pta);
            DEC_REFCNT(help_pt);
            push((int)ptdesc);
            goto L0;
          } /* END (arg == n) in PRIM CLOS */
          else
/* NOT ENOUGH ARGUMENTS */          
          if (n < arg)
          {
            dyn_mkbclos(n);
            flip_tail();                      /* avoid stackswitch */
            rtf();
          } /* END (n < arg) */
/* TOO MANY ARGUMENTS */          
          else /* (n > arg) */
          {
            sargs = R_CLOS(*ptdesc, args);
            n += sargs;  /* prepare the GOTO L0; to execute the delta_red */
            drop(w, 1);  /* POP closure */
            pth = R_CLOS(*ptdesc, pta);
            pth += sargs;
            while (sargs-- > 0) /* restore the argument frame */
            {
              help_pt = (T_PTD)*pth--;
              T_INC_REFCNT(help_pt);
              push((int)help_pt);
            } 
/* NO stflip(a,w); 'cause it's now a DELTA-call */

            help_pt = ptdesc;              
            ptdesc = (T_PTD)*R_CLOS(*ptdesc, pta);
            DEC_REFCNT(help_pt);
            push((int)ptdesc);
            goto too_many_delta;
          } /* END n > arg */
        } 
/********************************************************************/
/* END primitive function closure                                   */
/********************************************************************/
          
/********************************************************************/
/* test, if interaction closure                                     */
/********************************************************************/
        else if (TY_INTACT == R_CLOS(*ptdesc, ftype))
        {
          /* Closure 'auspacken', n um Anzahl vorhandener Argumente */
          /* erhoehen und goto nach L0:                             */
          sargs = R_CLOS(*ptdesc, args);
          n += sargs;  /* prepare the GOTO L0; */
          drop(w, 1);  /* drop closure */
          pth = R_CLOS(*ptdesc, pta);
          pth = pth + sargs;
          while (sargs-- >= 0) /* restore the argument frame */
          {
            help_pt = (T_PTD)*pth--;
            T_INC_REFCNT(help_pt);
            push((int)help_pt);
          }
          DEC_REFCNT(ptdesc);
          goto L0;
        }
/********************************************************************/
/* END,  if interaction closure                                     */
/********************************************************************/

/********************************************************************/
/* test, if userdefined function closure                            */
/********************************************************************/
        else if ((TY_CASE == R_CLOS(*ptdesc, ftype)) ||
            (TY_COMB == R_CLOS(*ptdesc, ftype)))
        {
          if (0 == red_cnt)
          {
            dyn_mkbclos(n);
            flip_tail();                      /* avoid stackswitch */
            rtf();
          }

/* ENOUGH ARGUMENTS */          
          if (n == arg)
          {
            if (withtilde)
              nfv = R_CLOS(*ptdesc,nfv);
            else
              nfv = 0;
            sargs = R_CLOS(*ptdesc, args);
            drop(w, 1);
            pth = R_CLOS(*ptdesc, pta);
            pth = pth + sargs + nfv;
            if (withtilde)
            {
              int dummy = nfv;
              
              while (dummy-- > 0)  /* tilde frame */
              {
                help_pt = (T_PTD)*pth--;
                T_INC_REFCNT(help_pt);
                push_t((int)help_pt);
              }
              while (sargs-- > 0)  /* argument frame */
              {
                help_pt = (T_PTD)*pth--;
                T_INC_REFCNT(help_pt);
                push((int)help_pt);
              }
            }	    
            else 
            {
              while (sargs-- > 0)
              {
                help_pt = (T_PTD)*pth--;
                T_INC_REFCNT(help_pt);
                push((int)help_pt);
              } 
            }
            
            if (withtilde)
            {  
              if (nfv > 0) 
              {
                push_r(nfv);                 /* f_for popfree_t */
                push_r(((int)&f_popfree_t | FUNCTIONRETURN));    
              }
            }

            if (TY_COMB == R_CLOS(*ptdesc, ftype)) 
            {
              stflip(a,w);                     /* don't flip if CASE */
            }
            else
            {
              flip_tail();                    /* but avoid when returning */
            }
            help_pt = ptdesc; 
            { /* cr 11.03.96: first get the information (ptdesc,myptc),
                              then give away the reference (help_pt) */
              T_HEAPELEM *myptc;

              ptdesc = (T_PTD)*R_CLOS(*ptdesc, pta);

              if (TY_CASE == R_DESC(*ptdesc, type))
                myptc = ((int)(R_CASE(*ptdesc, ptc)));
              else
                myptc = ((int)(R_COMB(*ptdesc, ptc)));
              DEC_REFCNT(help_pt); 
              push_r(0); /* so that the called function knows
                            where to start (L0:) */
              DBUG_RETURN((int)myptc);
            }
            /* the fitting rtf() will find the return to the function
                which called the apply (if no other call is in between 
                or if there are free vars to kill it first will find the
                address of f_popfree_t() and f_popfree_t's rtf will find
                the way back  */
          } /* END (arg == n) */
          else
/* NOT ENOUGH ARGUMENTS */            
          if (n < arg)
          {
            dyn_mkbclos(n);
            flip_tail();                      /* avoid stackswitch */
            rtf();
          } /* END (arg < n) */
          else  /* arg > n */
/* TOO MANY ARGUMENTS */            
          {
            if (TY_COMB == R_CLOS(*ptdesc, ftype))
              rest_args = n - R_COMB(*ptdesc, nargs);
            else
              rest_args = n - R_CASE(*ptdesc, nargs);
            push_r(rest_args);
            push_r((int)&f_apply | FUNCTIONRETURN);
            if (TY_COMB == R_CLOS(*ptdesc, ftype))
              n  = R_COMB(*ptdesc, nargs);
            else
              n  = R_CASE(*ptdesc, nargs);

            goto L0;
          }  /* END FUNC_ARITY((int)ptdesc) > n */
        } 
/********************************************************************/
/* END if userdefined function closure Rand CASE                    */
/********************************************************************/
/********************************************************************/
/* test, if conditional closure                                     */
/********************************************************************/
        else if (TY_CONDI == R_CLOS(*ptdesc, ftype))
        {
/* ENOUGH ARGUMENTS */   
/* all COND descriptor accesses via X_COMB 
   `cause it`s saved in COMBINATOR format */
          if (n == arg)
          {
            if (withtilde)
              nfv = R_CLOS(*ptdesc,nfv);
            else
              nfv = 0;
            sargs = R_CLOS(*ptdesc, args);
            pth = R_CLOS(*ptdesc, pta);
            pth = pth + sargs + nfv;
            if (withtilde)
            {
              int dummy = nfv;
              
              while (dummy-- > 0)  /* tilde frame */
              {
                help_pt = (T_PTD)*pth--;
                T_INC_REFCNT(help_pt);
                push_t((int)help_pt);
              }
              while (sargs-- > 0)  /* argument frame */
              {
                help_pt = (T_PTD)*pth--;
                T_INC_REFCNT(help_pt);
                push_a((int)help_pt);
              }
            }	    
            else 
            {
              while (sargs-- > 0)
              {
                help_pt = (T_PTD)*pth--;
                T_INC_REFCNT(help_pt);
                push_a((int)help_pt);
              } 
            }
            if (withtilde)
            {  
              if (nfv > 0) 
              {
                push_r(nfv);                 /* f_for popfree_t */
                push_r(((int)&f_popfree_t | FUNCTIONRETURN));    
              }
            }
            help_pt = ptdesc; 
            ptdesc = (T_PTD)*R_CLOS(*ptdesc, pta);
            DEC_REFCNT(help_pt);
            drop(w, 1);
            flip_tail();       /* avoid stack_switch */
            push_r(0); /* so that the called function knows
                          where to start (L0:) */

            DBUG_RETURN((int)(R_CONDI(*ptdesc, ptc)));
            /* the fitting rtf() will find the return to the function
               which called the apply (if no other call is in between 
               or if there are free vars to kill it first will find the
               address of f_popfree_t() and f_popfree_t's rtf will find
               the way back  */
          } /* END (arg == n) */
          else
          if (n < arg)
          {
            rest_args = n - R_CONDI(*ptdesc, nargs);
            push_r(rest_args);
            push_r((int)&f_apply | FUNCTIONRETURN);
            n  = R_CONDI(*ptdesc, nargs);
            goto L0;
          }

        }
/********************************************************************/
/* END if conditional closure                                       */
/********************************************************************/
      }
    }
    /* All other (e.g. interaction descriptor) goto T_mkap */
    goto T_mkap;
  }
/********************************************************************/
/* END   if POINTER                                END   if POINTER */
/********************************************************************/
  else   /* something irreducible like "ap 5 to [7]" ... */
  {
T_mkap:    
    MAKEDESC(ptdesc, 1, C_EXPRESSION, TY_EXPR);
    GET_HEAP(n + 3, (int)A_EXPR(*ptdesc, pte));
    RES_HEAP;
    pth = (PTR_HEAPELEM)R_EXPR(*ptdesc, pte);
    *pth++ = (T_HEAPELEM)(n + 2);
    *pth++ = (T_HEAPELEM)SET_ARITY(AP,(n + 1));
    pth += n;
    do
    {
      *pth-- = (T_HEAPELEM)pop(w);
    } while (--n >= 0);
    REL_HEAP;
    push((int)ptdesc);
    flip_tail();                      /* avoid stackswitch */
    rtf();
  }
} /* END f_apply() */

