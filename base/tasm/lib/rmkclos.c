#define WITHTILDE 1
#define PROLOG_EXTERN
#include "prolog.h"

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
extern int withtilde;
extern T_DESCRIPTOR *newdesc();
extern int newheap(int, T_HEAPELEM);
extern FCTPTR dealloc_tab[];


extern PTR_DESCRIPTOR _desc;

PTR_HEAPELEM pth; /* pointer to heapelement */
T_PTD ptdesc;     /* pointer to descriptor */


/**********************************************************************/
/* builds funcdesctable from stat_funcdesctable                       */
/**********************************************************************/
void build_dyn_funcdesctable(PTR_DESCRIPTOR *funcdesctable,T_DESCRIPTOR *desc_array, int n)
{
  int i;
  DBUG_ENTER("build_dyn_funcdesctable");
  for(i = 0; i< n; i++)
  {
    switch(R_DESC((desc_array[i]), type))
    {
      case TY_COMB:
        MAKEDESC(ptdesc, 1, C_FUNC, TY_COMB);
        L_COMB(*ptdesc, args) = R_COMB(desc_array[i], args);
        L_COMB(*ptdesc, nargs) = R_COMB(desc_array[i], nargs);
        L_COMB(*ptdesc, ptc) = R_COMB(desc_array[i], ptc);
        L_COMB(*ptdesc, ptd) = R_COMB(desc_array[i], ptd);
        break;
      case TY_CASE:
        MAKEDESC(ptdesc, 1, C_FUNC, TY_CASE);
        L_CASE(*ptdesc, args) = R_CASE(desc_array[i], args);
        L_CASE(*ptdesc, nargs) = R_CASE(desc_array[i], nargs);
        L_CASE(*ptdesc, ptc) = R_CASE(desc_array[i], ptc);
        L_CASE(*ptdesc, ptd) = R_CASE(desc_array[i], ptd);
        break;
      case TY_CONDI:
        MAKEDESC(ptdesc, 1, C_FUNC, TY_CONDI);
        L_CONDI(*ptdesc, args) = R_CONDI(desc_array[i], args);
        L_CONDI(*ptdesc, nargs) = R_CONDI(desc_array[i], nargs);
        L_CONDI(*ptdesc, ptc) = R_CONDI(desc_array[i], ptc);
        L_CONDI(*ptdesc, ptd) = R_CONDI(desc_array[i], ptd);
        break;
      default: controlled_exit(" unknown descriptor class in build_dyn_funcdesctable");
        break;
    }
    funcdesctable[i] = (PTR_DESCRIPTOR)ptdesc;
  }
  DBUG_VOID_RETURN;
}

/**********************************************************************/
/* builds something irreducible like 'ap 5 to 7'                      */
/* args : number actual parameters                                    */
/**********************************************************************/
void f_mkap(int args)
{
  DBUG_ENTER("f_mkap");

  MAKEDESC(ptdesc, 1, C_EXPRESSION, TY_EXPR);
  GET_HEAP(args + 3, (int)A_EXPR(*ptdesc, pte));
  RES_HEAP;
  pth = (PTR_HEAPELEM)R_EXPR(*ptdesc, pte);
  *pth++ = (T_HEAPELEM)(args + 2);
  *pth++ = (T_HEAPELEM)SET_ARITY(AP,(args + 1));
  pth += args;
  do
  {
    *pth-- = (T_HEAPELEM)pop(w);
  } while (--args >= 0);
  REL_HEAP;
  push((int)ptdesc);
  DBUG_VOID_RETURN;
}
/**********************************************************************/
/* builds a Closure with a primfunc                                   */
/* arity: arity of primfunc                                           */
/* args : number actual parameters                                    */
/* pfunc: which primfunc in reduma style (-1 if still on top of stack)*/
/* refcnt_set: 1 if explicite ref.counting (i.e. don't pop arg from W)*/
/*               (i.e. don't pop args from W and don't push result,   */
/*                'cause it's in _desc)                               */
/*            -1 pop args and push result                             */
/*             0 pop args and don't push result (it's kept in _desc)  */
/**********************************************************************/
void f_mkdclos(int refcnt_set, int arity, int args, int pfunc)
{
  int arg, i;
  int too_many_args;
  
  DBUG_ENTER("f_mkdclos");
  
  /* DIE GEWUENSCHTE PRIMFUNC vgl. rstelem.h ab Zeile 843 */
  if (-1 != pfunc) push((int)pfunc);  /* Prim. Fkt. muss oben auf Stack W liegen */
  too_many_args = args - arity;
  if (too_many_args > 0)
    arg = arity;
  else
    arg = args;
  

  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS); 
/* 
   _desc = pointer auf den erhaltenen Desc.
   1    = Ref.counter fuer erhaltenes Elem.
   C_FUNC = Klasse
   TY_CLOS = Type des Desk.
*/

  GET_HEAP(arg + 1, (int)A_CLOS(*_desc, pta));
/*
   Heap-speicher fuer Stackauszug holen
   Anzahl der benoetigten Elemente + 1
   pta (Element in ST_CLOS) zuweisen
*/

  L_CLOS(*_desc, args) = arg;
  L_CLOS(*_desc, nargs) = arity - arg;
  if (withtilde)
    L_CLOS(*_desc, nfv) = 0;
  L_CLOS(*_desc, ftype) = TY_PRIM;

  RES_HEAP;  /* Heap jetzt nur fuer mich */

  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Zeiger auf den Frameanfang */

  /* ab jetzt keine gamma-closure, also Funktion auf W */
  *pth++ = (T_HEAPELEM)pop(w); /* Funktion gepopt */

  for (i = 0; i < arg; i++)
  {
    if (0 < refcnt_set)
    {
      *pth++ = (T_HEAPELEM)peek(w, i); /* Arg-Frame holen */
    }
    else
    {
      *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
    }
  }
  REL_HEAP; /* Heap nicht mehr nur fuer mich */
  if (-1 == refcnt_set)
    push((int)_desc); /* Ergebnis,d.h. Zeiger auf Closure auf W legen */
  if (too_many_args > 0)
  {
    f_mkap(too_many_args);
  }
  
  DBUG_VOID_RETURN;
  
}
/**********************************************************************/
/* builds a Closure with an interaction                               */
/* The interactions lies on Top of W. f_mkiclos replaces it and the   */
/* <args> actual parameters with the Closure descriptor. Make sure    */
/* that args is less then the arity of the interaction.               */
/* args : number actual parameters                                    */
/**********************************************************************/
void f_mkiclos(int args)
{
  int intact;

  DBUG_ENTER("f_mkiclos");

  intact = (int) top(w);

  if (!T_IAFUNC(intact))
    controlled_exit("f_mkiclos: There is no interaction on top of W.");

  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  L_CLOS(*_desc, ftype) = TY_INTACT;
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = FUNC_ARITY((int)intact) - args;
  if (withtilde)
    L_CLOS(*_desc, nfv) = 0;
  GET_HEAP(args + 1, (int)A_CLOS(*_desc, pta));

  RES_HEAP;
  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Zeiger auf den Frameanfang */
  *pth++ = (T_HEAPELEM)pop(w);
  while (args-- > 0)
    *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
  REL_HEAP;

  push((int)_desc); /* Ergebnis,d.h. Zeiger auf Closure auf W legen */
  DBUG_VOID_RETURN;
}
/*************************************************************/
/* searches for the CASE desc. for a known WHEN              */
/*************************************************************/

T_PTD search_father_case(T_PTD ptd,T_PTD *funcdesctable)
{
  int i = 0;
  while (1)
  {
    if (TY_CASE == R_DESC(*(funcdesctable[i]), type))
      if ((T_PTD)R_CASE(*funcdesctable[i], ptd) == ptd) 
        return(funcdesctable[i]);
    i++;
  }
  return(NULL);
}

/*************************************************************/
/* builds an undecided closure if guard is undecided         */
/* addr : number of case descriptor in funcdesctable         */
/*************************************************************/
void f_guard_undecided(int addr,PTR_DESCRIPTOR *funcdesctable)
{
  register T_PTD ptd, ptd2, my_desc;
  int nfv, arity, hilf;
  
  DBUG_ENTER("f_guard_undecided");
  
  ptdesc = (funcdesctable[addr]);         /* WHEN desc */
  ptd2 = search_father_case(R_CASE(*ptdesc, ptd),funcdesctable);  /* CASE desc */
  nfv = R_CASE(*ptd2, args);
  
  MAKEDESC(ptd, 1, C_FUNC, TY_CLOS);
  L_CLOS(*ptd, ftype) = TY_NOMATBODY;
  L_CLOS(*ptd, nargs) = 0;
  L_CLOS(*ptd, args) = VALUE(pop(r));
  L_CLOS(*ptd, nfv) = nfv;
  GET_HEAP(nfv + R_CLOS(*ptd, args) + 4, (int)A_CLOS(*ptd, pta));
  /*             ^ # bound_var        */
  /*************************************************/
  pth = (PTR_HEAPELEM)R_CLOS(*ptd, pta);
  pth[0] = (T_HEAPELEM)R_CASE(*ptd2, ptd);      /* ptr to SWITCH-expression */
  pth[1] = VAL_INT((int)R_CASE(*ptdesc, ptc));    /* # of failed when */
  pth[2] = pop(w);                  /* guard */
  pth[3] = pop(w);                  /* match */
  pth += 4;

  hilf = R_CLOS(*ptd, args);
  while (hilf-- > 0)
    *pth++ = (T_HEAPELEM)pop(a);

  hilf = nfv;
  while (hilf > 0)
  {
    *pth++ = (T_HEAPELEM)peek(t, nfv - hilf);
    T_INC_REFCNT((PTR_DESCRIPTOR)peek(t, nfv - hilf));
    hilf--;
  }

  /**************************************** 
   X_CLOS(ptd, pta) wie folgt :
      ptsdes, #failed_when, pop(w)=guard, pop(w)=bound_match, bound_vars, free_vars
      ^       ^ R_CASE(*ptdesc, ptc)                         ^ #top(r)   ^ #nfv
      ^ R_CASE(MAIN*ptdesc, ptd)  ptr to SWITCH-expression
  *****************************************/
    /* now building a closure for the NOMATCH */
    MAKEDESC(my_desc, 1, C_FUNC, TY_CLOS);
    GET_HEAP(nfv+1, (int)A_CLOS(*my_desc,pta));
    L_CLOS(*my_desc,args) = 0;
    L_CLOS(*my_desc,nargs) = 0;
    L_CLOS(*my_desc,nfv) = nfv;
    L_CLOS(*my_desc,ftype) = TY_CASE;
    RES_HEAP;
    pth = (PTR_HEAPELEM)R_CLOS(*my_desc,pta);
    *pth++ = (T_HEAPELEM)ptd;
    {
      int old_nfv = nfv;

      while (nfv > 0)
      {
        *pth++ = (T_HEAPELEM)peek(t, old_nfv-nfv);
        T_INC_REFCNT((PTR_DESCRIPTOR)peek(t, old_nfv-nfv));
        nfv--;
      }
    }
  push((int)my_desc);
  
  DBUG_VOID_RETURN;
}
/*************************************************************/
/* builds a Closure with a CASE-construct                    */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of userdefined Function      */
/* Builds closures statically                                */
/*************************************************************/

void f_mkcase(int args, int ufunc, PTR_DESCRIPTOR *funcdesctable)
{
  int arity, arg, nfv;
  PTR_DESCRIPTOR my_desc;

  DBUG_ENTER("f_mkcase");
  if (T_KLAA(top(t)))    /* MATCH undecided */
  {
    pop(t);             /* KLAA */
    ptdesc = (funcdesctable[pop(t)]);
    MAKEDESC(_desc, 1, C_EXPRESSION, TY_NOMAT);
    L_NOMAT(*_desc, act_nomat) = VAL_INT((int)R_CASE(*ptdesc, ptc)); 
    ptdesc = search_father_case(R_CASE(*ptdesc, ptd),funcdesctable);
    nfv = R_CASE(*ptdesc, args);
    arity = R_CASE(*ptdesc, nargs);
    L_NOMAT(*_desc, ptsdes) = R_CASE(*ptdesc, ptd);
    GET_HEAP(4, (int)A_NOMAT(*_desc, guard_body));
    L_NOMAT(*_desc, guard_body)[0] = 3;          /* length */
    L_NOMAT(*_desc, guard_body)[1] = 0;          /* guard */
    L_NOMAT(*_desc, guard_body)[2] = 0;          /*body */
    L_NOMAT(*_desc, guard_body)[3] = pop(w);     /* argument */
    L_NOMAT(*_desc, reason) = 2;                 /* match undecided */
    /* now building a closure for the NOMATCH */
    MAKEDESC(my_desc, 1, C_FUNC, TY_CLOS);
    GET_HEAP(nfv+1, (int)A_CLOS(*my_desc,pta));
    L_CLOS(*my_desc,args) = 0;
    L_CLOS(*my_desc,nargs) = 0;
    L_CLOS(*my_desc,nfv) = nfv;
    L_CLOS(*my_desc,ftype) = TY_CASE;
    RES_HEAP;
    pth = (PTR_HEAPELEM)R_CLOS(*my_desc,pta);
    *pth++ = (T_HEAPELEM)_desc;
    {
      int old_nfv = nfv;
      
      while (nfv > 0) 
      {
        *pth++ = (T_HEAPELEM)peek(t, old_nfv-nfv);
        T_INC_REFCNT((PTR_DESCRIPTOR)peek(t, old_nfv-nfv));
        nfv--;
      }  
    }
    REL_HEAP;
    _desc = my_desc;
  }
  else
  {
    ptdesc = (funcdesctable[ufunc]);
    if (T_INT((int)R_CASE(*ptdesc, ptc)))
      ptdesc = search_father_case(R_CASE(*ptdesc, ptd),funcdesctable);
    if (-1 == args) args = R_CASE(*ptdesc, nargs);
    INC_REFCNT(ptdesc);
    push((int)ptdesc);
    arg = R_CASE(*ptdesc, nargs); /* number needed parameters */
    if (withtilde)
      nfv = R_CASE(*ptdesc, args);
    else
      nfv = 0;
    _desc=NULL;
    MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
    GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
    L_CLOS(*_desc, args) = args;
    L_CLOS(*_desc, nargs) = arg - args;
    if (withtilde)
      L_CLOS(*_desc, nfv) = nfv;
    L_CLOS(*_desc, ftype) = TY_CASE;
    
    RES_HEAP;  /* Heap jetzt nur fuer mich */

    pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Zeiger auf den Frameanfang */

    /* ab jetzt keine gamma-closure, also Funktion auf W */
    *pth++ = (T_HEAPELEM)pop(w); /* Funktion gepopt */
    while (args-- > 0)
    {
      *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
    }
    {
      int old_nfv = nfv;
      while (nfv > 0)
      {
        *pth++ = (T_HEAPELEM)peek(t, (old_nfv - nfv)); 
        T_INC_REFCNT((PTR_DESCRIPTOR)peek(t,(old_nfv - nfv)));
        nfv--;
      }
    }  
    REL_HEAP; /* Heap nicht mehr nur fuer mich */
  }  
  push((int)_desc); /* Ergebnis,d.h. Zeiger auf Closure auf W legen */
  _desc = NULL;
  DBUG_VOID_RETURN;
}


/*************************************************************/
/* builds a switch/case closure with free vars               */
/* arity: number actual parameters                           */
/* args : number needel parameters                           */
/* nfv  : number of free variables                           */
/*************************************************************/

void f_mksclos(int arity, int args, int nfv)
{
  DBUG_ENTER("f_mksclos");
  
  _desc=NULL;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + args + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = arity;
  L_CLOS(*_desc, nargs) = args - arity;
  L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_CASE;
  
  RES_HEAP;  /* Heap jetzt nur fuer mich */

  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Zeiger auf den Frameanfang */

  /* ab jetzt keine gamma-closure, also Funktion auf W */
  *pth++ = (T_HEAPELEM)pop(w); /* Funktion gepopt */
  while (arity-- > 0)
  {
    *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
  }
  {
    int old_nfv = nfv;
    while (nfv > 0)
    {
      *pth++ = (T_HEAPELEM)peek(t, (old_nfv - nfv)); 
      T_INC_REFCNT((PTR_DESCRIPTOR)peek(t,(old_nfv - nfv)));
      nfv--;
    }
  }  
  REL_HEAP; /* Heap nicht mehr nur fuer mich */
  
  push((int)_desc); /* Ergebnis,d.h. Zeiger auf Closure auf W legen */
  _desc = NULL;
  DBUG_VOID_RETURN;
  
}

/*************************************************************/
/* builds a Closure with a userdefined Function              */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of userdefined Function      */
/* Builds closures statically                                */
/*************************************************************/

int f_mkbclos_init(int args, T_PTD ufunc)
{
  int arg, nfv, help;
  
  DBUG_ENTER("f_mkbclos_init");
  
  INC_REFCNT(ufunc);
  help = (int)ufunc;
  arg = R_COMB(*ufunc, nargs); /* number needed parameters */
  nfv = 0;
  _desc=NULL;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = arg - args;
  if (withtilde)
    L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_COMB;
  
  RES_HEAP;  /* Heap only for me */

  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Pointer to start of frame */

  /* no gamma-closure => function on W */
  *pth++ = (T_HEAPELEM)help; /* pop funktion */
  while (args-- > 0)
  {
    *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
  }
  REL_HEAP; /* Heap nicht mehr nur fuer mich */
  
  DBUG_RETURN((int)_desc);
  
}
  
/*************************************************************/
/* builds a Closure with a userdefined Function              */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of userdefined Function      */
/* Builds closures statically                                */
/*************************************************************/

int f_mkbclos(int args, T_PTD ufunc)
{
  int arg, nfv, help;
  
  DBUG_ENTER("f_mkbclos");
  
  INC_REFCNT(ufunc);
  help = (int)ufunc;
  arg = R_COMB(*ufunc, nargs); /* number needed parameters */
  if (withtilde)
    nfv = R_COMB(*ufunc, args);
  else
    nfv = 0;
  _desc=NULL;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = arg - args;
  if (withtilde)
    L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_COMB;
  
  RES_HEAP;  /* Heap only for me */

  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Pointer to start of frame */

  /* no gamma-closure => function on W */
  *pth++ = (T_HEAPELEM)help; /* pop funktion */
  while (args-- > 0)
  {
    *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
  }
  {
    int old_nfv = nfv;
    while (nfv > 0)
    {
      *pth++ = (T_HEAPELEM)peek(t, (old_nfv - nfv)); 
      T_INC_REFCNT((PTR_DESCRIPTOR)peek(t,(old_nfv - nfv)));
      nfv--;
    }
  }  
  REL_HEAP; /* Heap nicht mehr nur fuer mich */
  
  DBUG_RETURN((int)_desc);
  
}
/*************************************************************/
/* builds a Closure with a conditional while Initialisation  */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of userdefined Function      */
/* Builds closures statically                                */
/*************************************************************/

int f_mkcondclos_init(int args, T_PTD ufunc)
{
  int arg, nfv;
  
  DBUG_ENTER("f_mkcondclos_init");
  
  INC_REFCNT(ufunc);
  arg = R_CONDI(*ufunc, nargs); /* number needed parameters */
  nfv = 0;
  _desc=NULL;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = arg - args;
  if (withtilde)
    L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_CONDI;
  
  RES_HEAP;  /* Heap only for me */

  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Pointer to start of frame */

  /* no gamma-closure => function on W */
  *pth++ = (T_HEAPELEM)ufunc; /* pop funktion */
  while (args-- > 0)
  {
    *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
  }
  REL_HEAP; /* Heap nicht mehr nur fuer mich */
  
  DBUG_RETURN((int)_desc);
  
}
/*************************************************************/
/* builds a Closure with a conditional while Initialisation  */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of userdefined Function      */
/* Builds closures statically                                */
/*************************************************************/

int f_mkcondclos(int args, T_PTD ufunc)
{
  int arg, nfv;
  
  DBUG_ENTER("f_mkcondclos");
  
  INC_REFCNT(ufunc);
  arg = R_CONDI(*ufunc, nargs); /* number needed parameters */
  if (withtilde)
    nfv = R_CONDI(*ufunc, args);
  else
    nfv = 0;
  _desc=NULL;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = arg - args;
  if (withtilde)
    L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_CONDI;
  
  RES_HEAP;  /* Heap only for me */

  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Pointer to start of frame */

  /* no gamma-closure => function on W */
  *pth++ = (T_HEAPELEM)ufunc; /* pop funktion */
  while (args-- > 0)
  {
    *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
  }
  {
    int old_nfv = nfv;
    while (nfv > 0)
    {
      *pth++ = (T_HEAPELEM)peek(t, (old_nfv - nfv)); 
      T_INC_REFCNT((PTR_DESCRIPTOR)peek(t,(old_nfv - nfv)));
      nfv--;
    }
  }  
  REL_HEAP; /* Heap nicht mehr nur fuer mich */
  
  DBUG_RETURN((int)_desc);
  
}

/*************************************************************/
/* builds a Closure with a CASE while Initialisation         */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of userdefined Function      */
/* Builds closures statically                                */
/*************************************************************/

int f_mkcaseclos_init(int args, T_PTD ufunc)
{
  int arg, nfv;
  
  DBUG_ENTER("f_mkcaseclos_init");
  
  INC_REFCNT(ufunc);
  arg = R_CASE(*ufunc, nargs); /* number needed parameters */
  nfv = 0;
  _desc=NULL;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = arg - args;
  if (withtilde)
    L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_CASE;
  
  RES_HEAP;  /* Heap only for me */

  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Pointer to start of frame */

  /* no gamma-closure => function on W */
  *pth++ = (T_HEAPELEM)ufunc; /* pop funktion */
  while (args-- > 0)
  {
    *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
  }
  REL_HEAP; /* Heap nicht mehr nur fuer mich */
  
  DBUG_RETURN((int)_desc);
  
}
/*************************************************************/
/* builds a Closure with a CASE while Initialisation         */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of userdefined Function      */
/* Builds closures statically                                */
/*************************************************************/

int f_mkcaseclos(int args, T_PTD ufunc)
{
  int arg, nfv;
  
  DBUG_ENTER("f_mkcaseclos");
  
  INC_REFCNT(ufunc);
  arg = R_CASE(*ufunc, nargs); /* number needed parameters */
  if (withtilde)
    nfv = R_CASE(*ufunc, args);
  else
    nfv = 0;
  _desc=NULL;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = arg - args;
  if (withtilde)
    L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_CASE;
  
  RES_HEAP;  /* Heap only for me */

  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta); /* Pointer to start of frame */

  /* no gamma-closure => function on W */
  *pth++ = (T_HEAPELEM)ufunc; /* pop funktion */
  while (args-- > 0)
  {
    *pth++ = (T_HEAPELEM)pop(w); /* Arg-Frame holen */
  }
  {
    int old_nfv = nfv;
    while (nfv > 0)
    {
      *pth++ = (T_HEAPELEM)peek(t, (old_nfv - nfv)); 
      T_INC_REFCNT((PTR_DESCRIPTOR)peek(t,(old_nfv - nfv)));
      nfv--;
    }
  }  
  REL_HEAP; /* Heap nicht mehr nur fuer mich */
  
  DBUG_RETURN((int)_desc);
  
}
  
/*************************************************************/
/* builds a Closure with a userdefined Function              */
/* with free variables                                       */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of userdefined Function      */
/* gamma_closure: shows if statically or dynamically built   */
/*                closure                                    */
/*                if > 0 closure is on R else on W           */
/* case_or_comb: 1 = TY_CASE 0 = TY_COMB                     */
/*************************************************************/

void f_mkgaclos(int args, T_PTD ufunc, int gamma_closure, int case_or_comb)
{
  int arg, nfv;
  
  DBUG_ENTER("f_mkgaclos");
  
/*  INC_REFCNT(ufunc);*/
/*  repl(0, w, (int)ufunc);*/
  if (0 == case_or_comb)
  {
    arg = R_COMB(*ufunc, nargs);
    nfv = R_COMB(*ufunc, args);
  }
  else
  {
    arg = R_CASE(*ufunc, nargs);
    nfv = R_CASE(*ufunc, args);
  }

  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = arg - args;
  L_CLOS(*_desc, nfv) = nfv;
  if (case_or_comb)
    L_CLOS(*_desc, ftype) = TY_CASE;
  else
    L_CLOS(*_desc, ftype) = TY_COMB;
  
  RES_HEAP;
  pth = (PTR_HEAPELEM)R_CLOS(*_desc,pta);
  
  if (gamma_closure > 0) 
  { STACKELEM   mystelem;
    *pth++ = (T_HEAPELEM)ufunc;
    INC_REFCNT(ufunc);
    mystelem = pop(r);
    DEC_REFCNT((T_PTD)mystelem);  /* Funktionszeiger liegt auf R */
    while (args-- > 0) 
    {
      *pth++ = (T_HEAPELEM)pop(w); /* DeBruinvariablen */
    }     

    if (gamma_closure == 1)    /* statisch erzeugtes MKGACLOS */
    {
      while (nfv > 0) 
      {
         /* gammaclosures poppen ihre argumente von r */
         /* da free_r direkt folgt                    */
         *pth++ = (T_HEAPELEM)pop(r);
         nfv--;
      }
    }                /* gamma_closure == 2, d.h. dynamische closure-bildung */
    else 
    {                            /* gamma_closure = 2 */
      int old_nfv = nfv;

      while (nfv > 0) 
      {
                                 /* tildevariablen werden vom freesw_t */
                                 /* freigegeben                        */
        *pth++ = (T_HEAPELEM)peek(r, old_nfv - nfv);
        T_INC_REFCNT((PTR_DESCRIPTOR)peek(r, old_nfv - nfv));
        nfv--;
      }
      stflip(t, r);               /* for freesw_t */
    }   /* ELSE gamma_closure == 1 */

    gamma_closure = 0;
  }
  else 
  {                                  /* gamma_closure = 0 */
    *pth++ = (T_HEAPELEM)pop(w);     /* Funktionszeiger liegt auf W */

    while (args-- > 0) 
    {
      *pth++ = (T_HEAPELEM)pop(w); /* DeBruinvariablen */
    }
    {
      int old_nfv = nfv;

      while (nfv > 0) 
      {
                             /* tildevariablen werden in der aufrufenden */
                             /* funktion freigegeben                     */
        *pth++ = (T_HEAPELEM)peek(t, old_nfv - nfv);
        T_INC_REFCNT((PTR_DESCRIPTOR)peek(t, old_nfv - nfv));
        nfv--;
      }
    }
  }     /* ELSE gamma_closure > 0 */
  
  REL_HEAP;
  
  push((int)_desc);
  _desc = NULL;
  
  DBUG_VOID_RETURN;
}

/*************************************************************/
/* builds a Closure with an unreducible conditional          */
/* args : number of     parameters                           */
/* ufunc: pointer to descriptor of conditional               */
/* all COND descriptor accesses via X_COMB                   */
/* `cause it`s saved in COMBINATOR format                    */
/*************************************************************/

void f_condfail(int args, T_PTD ufunc)
{
  int arity, nfv, keep_flag, help;

  DBUG_ENTER("f_condfail");

  INC_REFCNT(ufunc);
  arity = R_CONDI(*ufunc, nargs);
  if (withtilde)
    nfv = (R_CONDI(*ufunc, args) >> 2);
  else
    nfv = 0;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arity + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = arity;
  L_CLOS(*_desc, nargs) = 0;
  if (withtilde)
    L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_CONDI;
  RES_HEAP;
  pth = (PTR_HEAPELEM)R_CLOS(*_desc, pta);
  *pth++ = (T_HEAPELEM)ufunc;
  if (withtilde)
    keep_flag = R_CONDI(*ufunc, args) & 0x3;
  else
    keep_flag = R_CONDI(*ufunc, args);
  if (keep_flag)  /* argumente duerfen nicht aufgebraucht werden */
  {
    register T_PTD ptd;
    for (help = 0; --arity > 0; help++)
    {
      ptd = (T_PTD)peek(a, help);
      T_INC_REFCNT(ptd);
      *pth++ = (T_HEAPELEM)ptd;
    }
  }
  else        /* argumente duerfen aufgebraucht werden */
  {
    while (--arity > 0)
      *pth++ = (T_HEAPELEM)pop(a);
  }
  if (withtilde)
  {
    for (help = 0; nfv > 0; nfv--, help++)   /* Tildeframe einsammeln */
    {
      register T_PTD ptd;
      ptd = (T_PTD)peek(t, help);
      T_INC_REFCNT(ptd);
      *pth++ = (T_HEAPELEM)ptd;
    }
  }
    *pth = (T_HEAPELEM)pop(w);
    REL_HEAP;
    push((int)_desc);

  DBUG_VOID_RETURN;
}



/*************************************************************/
/* builds a Closure with a conditional                       */
/* args : number actual parameters                           */
/* ufunc: pointer to descriptor of conditional               */
/* all COND descriptor accesses via X_COMB                   */
/* `cause it`s saved in COMBINATOR format                    */
/*************************************************************/

void f_mkcclos(int args, T_PTD ufunc)
{
  int arg, nfv, keep_flag;
  register VOLATILE int help=0;         /* hilfsvariable          */

  DBUG_ENTER("f_mkccclos");
  
  INC_REFCNT(ufunc);
  push((int)ufunc);
  arg = R_CONDI(*ufunc, nargs);
  if (withtilde)
  {
    nfv = (R_CONDI(*ufunc, args) >> 2);
    if (0 == nfv)  /* are there really no free vars ? */
      nfv = (R_CONDI(DESC(R_CLOS(*ufunc, pta)[0]), args)) >> 2;
  }
  else
    nfv = 0;
  MAKEDESC(_desc, 1, C_FUNC, TY_CLOS);
  GET_HEAP(nfv + arg + 1, (int)A_CLOS(*_desc, pta));
  L_CLOS(*_desc, args) = args;
  L_CLOS(*_desc, nargs) = arg - args;
  if (withtilde)
    L_CLOS(*_desc, nfv) = nfv;
  L_CLOS(*_desc, ftype) = TY_CONDI;
  
  RES_HEAP;
  pth = (PTR_HEAPELEM)R_CLOS(*_desc,pta);
  *pth++ = (T_HEAPELEM)top(w);
  if (withtilde) 
    keep_flag = R_CONDI(*(T_PTD)top(w), args) & 0x3;
  else
    keep_flag = R_CONDI(*(T_PTD)top(w), args);
  if (keep_flag)
  {  
                         /* argumente duerfen nicht aufgebraucht werden */
    register T_PTD ptd;
    drop(w, 1);  /* pop function from W */
     
    for (help = 0; --args > 0 ; help++) 
    {
      ptd = (T_PTD)peek(w, help);
      T_INC_REFCNT(ptd);
      *pth++ = (T_HEAPELEM)ptd;
    }
  }
  else 
  {
                             /* argumente duerfen aufgebraucht werden */
    drop(w, 1);  /* pop function from W */
    while (args-- > 0) 
    {
      *pth++ = (T_HEAPELEM)pop(w);
    }
  }     /* ELSE (R_CONDI(*(T_PTD)top(w), args) & 0x3) */
  
  for ( help = 0; nfv > 0; nfv--,help++) 
  {                                       /* Tildeframe einsammeln */
    register T_PTD ptd;

    ptd = (T_PTD)peek(t, help);
    T_INC_REFCNT(ptd);
    *pth++ = (T_HEAPELEM)ptd;
  }
/*  *pth = (T_HEAPELEM)top(w);*/                     /* praedikat   */
  REL_HEAP;

  push((int)_desc);
  _desc = NULL;

  DBUG_VOID_RETURN;
}

/***********************************************************************/
/* builds the static closures for the static descriptors               */
/***********************************************************************/

void build_static_clos(int *funcclostable,PTR_DESCRIPTOR *funcdesctable,int n)
{
  int i;
  T_DESCRIPTOR *help;
  
  DBUG_ENTER("build_static_clos");

  for (i=0; i<n; i++) 
  { 
    if (TY_COMB == R_DESC(*(funcdesctable[i]), type))
    {
      funcclostable[i] = f_mkbclos_init(0, funcdesctable[i]);
/* Pointer to closure is now on stack W -> pop it */
      if (withtilde)
        L_CLOS(DESC(funcclostable[i]), nfv) = 0;
    }
    else if (TY_CONDI == R_DESC(*(funcdesctable[i]), type))
    {
      funcclostable[i] = f_mkcondclos_init(0, funcdesctable[i]);
/* Pointer to closure is now on stack W -> pop it */
      if (withtilde)
        L_CLOS(DESC(funcclostable[i]), nfv) = 0;
    }
    else if (TY_CASE == R_DESC(*(funcdesctable[i]), type))
    {
      funcclostable[i] = f_mkcaseclos_init(0, funcdesctable[i]);
/* Pointer to closure is now on stack W -> pop it */
      if (withtilde)
        L_CLOS(DESC(funcclostable[i]), nfv) = 0;
    }
  }
  DBUG_VOID_RETURN;
}

/*************************************************************/
/* builds a Closure with a userdefined Function              */
/* and a CASE-construct                                      */
/* args : number actual parameters                           */
/* assumes a closure on top of stack w                       */
/* Builds closures dynamically                               */
/* Also expands existing primfunc closures                   */
/*************************************************************/

void dyn_mkbclos(int args)
{
  int arg, nfv, i, func, no_targs = 0;  /* no_targs = 1 if tildeargs not yet saved */
  int ftype;
  DBUG_ENTER("dyn_mkbclos");
  
  ptdesc=(T_PTD)top(w);

  arg = R_CLOS(*ptdesc, nargs); /* number needed parameters */
  ftype = R_CLOS(*ptdesc, ftype);
  if (withtilde)
  {
    nfv = R_CLOS(*ptdesc, args);
    if (0 == nfv)  /* are there really no free vars ? */
    {
      if (TY_COMB == ftype)
        nfv = R_COMB(DESC(R_CLOS(*ptdesc, pta)[0]), args);
      else
        nfv = R_CASE(DESC(R_CLOS(*ptdesc, pta)[0]), args);
      if (nfv) no_targs = 1;
    }
  }
  else
    nfv = 0;

  if (1 == R_CLOS(*ptdesc, ref_count))
  {

    RES_HEAP;  /* Heap jetzt nur fuer mich */
    /* erst den tilde frame in sicherheit bringen */
    if (withtilde && !no_targs)
    {
      pth = (PTR_HEAPELEM)R_CLOS(*ptdesc, pta);
      pth += R_CLOS(*ptdesc, args);
      pth += nfv;
      for (i = nfv; i >= 0; i--, pth--)
      {
        *(pth + args) = *pth;
      }
    }
    pth = (PTR_HEAPELEM)R_CLOS(*ptdesc, pta);
    pth += R_CLOS(*ptdesc, args);
    for (i = args; --i >= 0;)
    {
      drop(w, 1);
      *++pth = top(w);
    }
    if (withtilde && no_targs)    /* save tildeargs now */
    {
      int old_nfv = nfv;
      while (nfv > 0)
      {
        *++pth = (T_HEAPELEM)peek(t, (old_nfv - nfv));
        T_INC_REFCNT((PTR_DESCRIPTOR)peek(t,(old_nfv - nfv)));
        nfv--;
      }
    }
    REL_HEAP;
    
    L_CLOS(*ptdesc, args) += args;
    L_CLOS(*ptdesc, nargs) -= args;
    repl(0, w, (int)ptdesc);
  }
  else /* neue closure bilden */
  {
    i = R_CLOS(*ptdesc,args);
    func = (int)ptdesc;              /* closure */
    MAKEDESC(ptdesc,1,C_FUNC,TY_CLOS);
    L_CLOS(*ptdesc,args) = i + args; /* mehr args */
    if (withtilde)
    {
      L_CLOS(*ptdesc,nfv) = nfv;
    }
    L_CLOS(*ptdesc, nargs) = arg - args;   /* weniger nargs */
    L_CLOS(*ptdesc, ftype) = R_CLOS(*(T_PTD)func, ftype);

    arg += i + 1;      /* laenge der arg.liste */

    if (withtilde)
    {
      if (newheap(arg+nfv,(int)A_CLOS(*ptdesc,pta))) 
      {
        register PTR_HEAPELEM p1,p2;
        RES_HEAP;
        p1 = (PTR_HEAPELEM)R_CLOS(*(T_PTD)func, pta);
        p2 = (PTR_HEAPELEM)R_CLOS(*ptdesc, pta);
        do {                   /* altes argument frame */
          T_INC_REFCNT((T_PTD)*p1);
          *p2++ = *p1++;
        } while (--i >= 0);
        drop(w, 1);                    /* alte closure */
        while (args-- > 0)             /* zusaetzliche argumente */
          *p2++ = (T_HEAPELEM)pop(w);
        if (no_targs)   /*save tilde args now */
        {
          int old_nfv = nfv;
          while (nfv > 0)
          {
            *p2++ = (T_HEAPELEM)peek(t, (old_nfv - nfv));
            T_INC_REFCNT((PTR_DESCRIPTOR)peek(t,(old_nfv - nfv)));
            nfv--;
          }
        }
        else
        {
          while (nfv-- > 0)  /* tilde frame */
          {
            T_INC_REFCNT((T_PTD)*p1);
            *p2++ = *p1++;
          }
        }
        REL_HEAP;
        /*  DEC_REFCNT((T_PTD)func); */    /* alte closure */
        push((int)ptdesc);             /* neue closure */
      }
    }
    else
    {
      if (newheap(arg,(int)A_CLOS(*ptdesc,pta))) 
      {
        register PTR_HEAPELEM p1,p2;
        RES_HEAP;
        p1 = (PTR_HEAPELEM)R_CLOS(*(T_PTD)func, pta);
        p2 = (PTR_HEAPELEM)R_CLOS(*ptdesc, pta);
        do {                   /* altes argument frame */
          T_INC_REFCNT((T_PTD)*p1);
          *p2++ = *p1++;
        } while (--i >= 0);
        drop(w, 1);                    /* alte closure */
        do {                   /* zusaetzliche argumente */
          *p2++ = (T_HEAPELEM)pop(w);
        } while (--args > 0);
        REL_HEAP;
        /* DEC_REFCNT((T_PTD)func);  */   /* alte closure */
        push((int)ptdesc);             /* neue closure */

      }
    }
  }   
  DBUG_VOID_RETURN;
}
