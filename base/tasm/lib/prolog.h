/********************************************************************/
/* RETURNADDRESS on returnstack coded as follows:                   */
/* BIT 0 :  tail-flag  (switch a and w if not set)                  */    
/* BIT 1 :  func-label-flag (following address is a funktion if set)*/
/* In case of Distribution on ncube                                 */
/* BIT 30:  tail-flag  (switch a and w if not set)                  */    
/* BIT 31:  func-label-flag (following address is a funktion if set)*/
/********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include "dbug.h"
#include "rstdinc.h"
#include "rheapty.h"
#if D_SLAVE
#include "rncstack.h"
#include "rncheap.h"
#else /* D_SLAVE */
#include "rstackty.h"
#endif /* D_SLAVE */
/*
#include "roptasm.h"
*/
#include "rmkclos.h"
#include "rapply.h"
#include "rstelem.h"
#include "rintact.h"
#if D_SLAVE
#include "rncsched.h"
#include "rncmessage.h"



double stime, btime, etime;
int cube_dim, proc_id, * code;
BOOLEAN _count_reductions, _trunc, _beta_count_only, _digit_recycling, LazyLists, _formated;
char post_mortem_msg[256];

#if D_SLAVE
extern int sig_msg_flag;
extern void msg_check(void);
#endif /* D_SLAVE */

extern PROCESS_CONTEXT* curr_pid;
extern int proc_id;
extern INSTR *code;
extern int sig_msg_flag;
int     _redcnt;
DStackDesc     *ps_w, *ps_a, *ps_r, *ps_t, *ps_h;
#endif /* D_SLAVE */

#if (D_SLAVE && D_MESS && D_MSCHED)
#define D_DISTRIBUTE (*d_m_distribute)
#else
#define D_DISTRIBUTE distribute
#endif

extern PTR_DESCRIPTOR _nil;
extern PTR_DESCRIPTOR _nilmat;
extern PTR_DESCRIPTOR _nilvect;
extern PTR_DESCRIPTOR _niltvect;
extern PTR_DESCRIPTOR _nilstring;
extern  T_DESCRIPTOR *highdesc;
extern int static_heap_upper_border;


typedef int (* FUNPTR)(void);

#define reduce_red_cnt() red_cnt--
#define MkInt(x, v)    (x)
#define MkBool(x, v)   (x)
#define MkElem(x, v)   (x)
#define MkVect(x, v)   (x)
#define MkTVect(x, v)  (x)
#define MkMatrix(x, v) (x)

#ifdef USES_GCC
#define MkReal(x,v)    (({MAKEDESC((PTR_DESCRIPTOR)v, 1, C_SCALAR, TY_REAL);}), \
                       L_SCALAR(*(PTR_DESCRIPTOR)v, valr) = (x),  \
                       (PTR_DESCRIPTOR)v)
#else /* USES_GCC */
#define MkReal(x,v)    initdata_new_scalar(x, (int)v)
#endif /* USES_GCC */

#define MAKE_CHAR(x)  (((x) << 16) | 0x0000001c)
#define GET_CHAR(x)   ((x & 0x00FF0000) >> 16)
#define IS_VAR_OR_CLOS_DESC(x)  ( (x)==C_EXPRESSION || (x)==C_CONSTANT)
#define DESC(x)        (* (PTR_DESCRIPTOR) x)

#if D_SLAVE
#define FUNCTIONRETURN 0x80000000
#define TAIL_FLAG      0x40000000
#else  /* D_SLAVE */
#define FUNCTIONRETURN 2
#define TAIL_FLAG      1
#endif /* D_SLAVE */

#define RT_FLAGS       (FUNCTIONRETURN | TAIL_FLAG)
#define TRUE           1
#define FALSE          0
#define end()          ; 
#define BITSTOKILLMASK ~RT_FLAGS /* see comment on top of file */
#define kill_bits(f)   ((f) & BITSTOKILLMASK) /* deletes FUNCTIONRETURN, TAIL_FLAG */

#define sprintw(n)
#define sprinta(n)
#define sprintr(n)
#define sprintt(n)

/*********** REFCOUNTING **********************/
#define SET_REFCNT_EXPL() ;
#define SET_REFCNT_IMPL() ;

#define nr_pushaw(n)      push(peek(a, n))
#define nr_pushar(n)      push_r(peek(a, n))
#define nr_pushtw(n)      push(peek(t, n))
#define nr_pushtr(n)      push_r(peek(t, n))
#define nr_push_p(x)      push((int)x)
#define nr_pushr_p(x)     push_r((int)x)

#define nr_freea(n)    drop(a, n)
#define nr_freeswt(n)  drop(t, n); stflip(t,r)

#define decw(n)        DEC_REFCNT((T_PTD)peek(w,n))
#define deca(n)        DEC_REFCNT((T_PTD)peek(a,n))
#define decr(n)        DEC_REFCNT((T_PTD)peek(r,n))
#define dect(n)        DEC_REFCNT((T_PTD)peek(t,n))
#define tdecw(n)       T_DEC_REFCNT((T_PTD)peek(w,n))
#define tdeca(n)       T_DEC_REFCNT((T_PTD)peek(a,n))
#define tdecr(n)       T_DEC_REFCNT((T_PTD)peek(r,n))
#define tdect(n)       T_DEC_REFCNT((T_PTD)peek(t,n))

#define incw(n)     INC_REFCNT((T_PTD)peek(w,n))
#define inca(n)     INC_REFCNT((T_PTD)peek(a,n))
#define incr(n)     INC_REFCNT((T_PTD)peek(r,n))
#define inct(n)     INC_REFCNT((T_PTD)peek(t,n))
#define tincw(n)    T_INC_REFCNT((T_PTD)peek(w,n))
#define tinca(n)    T_INC_REFCNT((T_PTD)peek(a,n))
#define tincr(n)    T_INC_REFCNT((T_PTD)peek(r,n))
#define tinct(n)    T_INC_REFCNT((T_PTD)peek(t,n))

#define killw(n)     (* dealloc_tab[R_DESC(((T_PTD)peek(w,n)),class)])((T_PTD)peek(w,n))
#define killa(n)     (* dealloc_tab[R_DESC(((T_PTD)peek(a,n)),class)])((T_PTD)peek(a,n))
#define killr(n)     (* dealloc_tab[R_DESC(((T_PTD)peek(r,n)),class)])((T_PTD)kill(r,n))
#define killt(n)     (* dealloc_tab[R_DESC(((T_PTD)peek(t,n)),class)])((T_PTD)peek(t,n))
#define tkillw(n)       if T_POINTER(peek(w,n)) \
                     (* dealloc_tab[R_DESC(((T_PTD)peek(w,n)),class)])((T_PTD)peek(w,n))
#define tkilla(n)       if T_POINTER(peek(a,n)) \
                     (* dealloc_tab[R_DESC(((T_PTD)peek(a,n)),class)])((T_PTD)peek(a,n))
#define tkillr(n)       if T_POINTER(peek(r,n)) \
                     (* dealloc_tab[R_DESC(((T_PTD)peek(r,n)),class)])((T_PTD)peek(r,n))
#define tkillt(n)       if T_POINTER(peek(t,n)) \
                     (* dealloc_tab[R_DESC(((T_PTD)peek(t,n)),class)])((T_PTD)peek(t,n))

/********END REFCOUNTING **********************/

#if D_SLAVE
#include "rncstack.h"
#define drop(st, n)     { int i; for(i=n; i; i--) pop(st); }
#define peek(st, n)     D_MIDSTACK(*ps_ ## st, n)
#define top(st)         D_READSTACK(*ps_ ## st)
#define repl(n, st, x)  D_UPDATESTACK(*ps_ ## st, n, x)
#define push(x)         D_PUSHSTACK(*ps_w, x)
#define push_a(x)       D_PUSHSTACK(*ps_a, x)
#define push_r(x)       D_PUSHSTACK(*ps_r, x)
#define push_t(x)       D_PUSHSTACK(*ps_t, x)
#define push_h(x)       D_PUSHSTACK(*ps_r, x)
#define pop(st)         D_POPSTACK(*ps_ ## st)
#define stflip(s, t)    { DStackDesc * x; x = ps_ ## s; ps_ ## s = ps_ ## t; ps_ ## t = x; }
#else /* D_SLAVE */
#define drop(s,n)      (st_##s-=n)
#define peek(s,n)      (st_##s[-(n)])
#define top(s)         peek(s,0)
#define repl(n,s,x)    st_##s[-n]=(x)         

#if TEST_STACK
#define push(x)        (ta_stacksize > (int *)st_w - (int *)_st_w ? (*++st_w = (x))   : \
                                          controlled_exit("stackoverflow on stack w"))
#define push_a(x)      (ta_stacksize > (int *)st_a - (int *)_st_a ? (*++st_a = (x))   : \
                                          controlled_exit("stackoverflow on stack a"))
#define push_r(x)      (ta_stacksize > (int *)st_r - (int *)_st_r ? (*++st_r = (x))   : \
                                          controlled_exit("stackoverflow on stack r"))
#define push_t(x)      (ta_stacksize > (int *)st_t - (int *)_st_t ? (*++st_t = (x))   : \
                                          controlled_exit("stackoverflow on stack t"))
#else  /* TEST_STACK */
#define push(x)        (*++st_w = (x))
#define push_a(x)      (*++st_a = (x))
#define push_r(x)      (*++st_r = (x))
#define push_t(x)      (*++st_t = (x))
#endif  /* TEST_STACK */


#define pop(s)         (*st_##s--)
#if TEST_STACK
#define stflip(s,t)    st_h=st_##s; st_##s=st_##t; st_##t=st_h; \
                       st_h=_st_##s; _st_##s=_st_##t; _st_##t=st_h
#else /* TEST_STACK */
#define stflip(s,t)    st_h=st_##s; st_##s=st_##t; st_##t=st_h
#endif /* TEST_STACK */
#endif /* D_SLAVE */

#define mkap(n)        f_mkap(n)
/* cr 22/03/95, kir(ff), START */
#define mkframe(n)      if (0 == initdata_frame(n, &st_w[1-n]))       \
                       {                                                 \
                         fprintf(stderr, "FATAL ERROR in mkframe(%d).\n", n);  \
                         exit(1);             \
                       }                      \
                       drop(w, n);                               \
                       push((int)_desc)
#define mkslot()        if (0 == initdata_slot(2, &st_w[1-2]))       \
                       {                                                 \
                         fprintf(stderr, "FATAL ERROR in mkslot().\n");  \
                         exit(1);             \
                       }                      \
                       drop(w, 2);                               \
                       push((int)_desc)
#define inter(type)    { int n=(type==TY_PUT? 3 : (type==TY_GET? 2 : 0)); \
                         if (0 == initdata_inter(type, n, &st_w[1-n]))       \
                         {                                                 \
                           fprintf(stderr, "FATAL ERROR in mkinter(%d).\n", type);  \
                           exit(1);             \
                         }                      \
                         drop(w, n);                               \
                         push((int)_desc);      \
                       }
/* cr 22/03/95, kir(ff), END */
#define mklist(n)      if (0 == initdata_list(n))       \
                       {                                                 \
                         fprintf(stderr, "FATAL ERROR in mklist(%d).\n", n);  \
                         exit(1);             \
                       }                      \
                       drop(w, n);                               \
                       push((int)_desc)
#define mkilist(n)     if (0 == initdata_i_list(n))       \
                       {                                                 \
                         fprintf(stderr, "FATAL ERROR in mkilist(%d).\n", n);  \
                         exit(1);             \
                       }                      \
                       drop(w, n);                               \
                       push((int)_desc)
/* here n is arity, l the label and f the function to return to */
#define ap(n,l,f)      push_r(l); \
                       push_r(((int)f | FUNCTIONRETURN)); \
                       push_r(n);          \
                       DBUG_RETURN((int)&f_apply)

/* here r is the label to return to */
#define beta(func, r)  push_r(r); \
                       stflip(a,w);  \
		       goto func
/* here r is the label to return to */
#if D_SLAVE
#define betanear(r,f,t) betafar(r,f,t)
#else
#define betanear(r,f,t) push_r(r); \
                       stflip(a,w);  \
                       goto L0 
#endif /* D_SLAVE */

/* here r is the label to return to, f the func to return to, t the called func */
#define betafar(r,f,t) push_r(r);    /* label to return to */           \
                       push_r(((int)f | FUNCTIONRETURN)); /*func to return to*/\
                       push_r(0);               \
                       stflip(a,w);  \
                       DBUG_RETURN((int)(t)) /* func to call */
/* here r is the label to return to */
#define gammabeta(func, ret)   stflip(a,w);  \
                               stflip(t,r);  \
                               push_r(ret); \
                               goto func 
/* here r is the label to return to */
#if D_SLAVE
#define gammabetanear(b,f,z) gammabetafar(b,f,z)
#else
#define gammabetanear(b,f,z)  stflip(a,w);  \
                            stflip(t,r);  \
                            push_r(z); \
                            goto L0 
#endif /* D_SLAVE */
/* here b is the label to return to, f the func to return to, z the called func */
#define gammabetafar(b,f,z) stflip(a,w);  \
                            stflip(t,r);  \
                            push_r(b);    \
                            push_r(((int)f | FUNCTIONRETURN)); \
                            /*func to return to*/   \
                            push_r(0);               \
                            DBUG_RETURN((int)(z)) /* func to call */
#define pushaux()      push((int)_desc)
#define t_push(x)      push((int)x); T_INC_REFCNT((T_PTD)top(w)) 
#define push_p(x)      INC_REFCNT(x); push((int)x)
#define pushr_p(x)     INC_REFCNT(x); push_r((int)x)
#define pop_p(s)       DEC_REFCNT((T_PTD)top(s)); pop(s)

#define pushaw(n)      push(peek(a, n)); T_INC_REFCNT((T_PTD)top(w))
#define pushar(n)      push_r(peek(a, n)); T_INC_REFCNT((T_PTD)top(r))
#define pushtw(n)      push(peek(t, n)); T_INC_REFCNT((T_PTD)top(w))
#define pushtr(n)      push_r(peek(t, n)); T_INC_REFCNT((T_PTD)top(r))

#define movetr()       push_r(pop(t))
#define movear()       push_r(pop(a))
#define movetw()       push(pop(t))
#define moveaw()       push(pop(a))
#define freew(n)       drop(w, n)
#define freea(n)       {int i;for(i=0;i<n;i++) T_DEC_REFCNT((T_PTD)peek(a,i));}; drop(a, n)
#define freerfw(n)     {int i;for(i=0;i<n;i++) T_DEC_REFCNT((T_PTD)peek(w,i));}; drop(w, n)
#define freeswt(n)     {int i;for(i=0;i<n;i++) T_DEC_REFCNT((T_PTD)peek(t,i));};   \
                       drop(t, n); stflip(t,r)
#define push_label(l)  push_r((l))
#define push_tail(t)   push_r((t))
#define flip_tail()    repl(0, r, top(r) ^ TAIL_FLAG)
#define jfalse(l,i)    if (!(T_BOOLEAN(top(w))))       \
                       {                        \
                          f_condfail(1, i);        \
                          rtf();               \
                       }                        \
                       if (IS_FALSE(pop(w))) goto l
#define jtrue(l,i)     if (!(T_BOOLEAN(top(w))))       \
                       {                        \
                          f_condfail(1, i);        \
                          rtf();               \
                       }                        \
                       if (!IS_FALSE(pop(w))) goto l
#define jcond(l,i)     if (!(T_BOOLEAN(top(w))))       \
                       {                        \
                          f_condfail(1, i);        \
                          rtf();               \
                       }                        \
                       if (IS_FALSE(pop(w))) goto l
#define mksclos(a,b,c) dyn_mkbclos(a);  \
                       L_CLOS(DESC(top(w)), ftype) = TY_CASE
#define mkcclos(i,j)   pop(w);  \
                       f_mkcclos(i,j)
/* in jcond2: l:ELSE-label, c:CONTINUATION-label, i:index of cond in table */
#define jcond2(l,c,i)  if (!(T_BOOLEAN(top(w))))       \
                       {                        \
                          f_condfail(1, i);        \
                          goto c;               \
                       }                        \
                       if (IS_FALSE(pop(w))) goto l
#define tail(func)     flip_tail(); \
                       stflip(a,w);  \
		       goto func
#define tailnear()     flip_tail();       \
                       stflip(a, w);       \
                       goto L0
#define tailfar(f,b)   stflip(a, w);       \
                       if (0 == (top(r) & FUNCTIONRETURN))  \
                       { push_r((int)b | FUNCTIONRETURN);  \
                         if (0 != (peek(r,1) & TAIL_FLAG)) flip_tail();} \
                       flip_tail();       \
                       push_r(0);       \
                       DBUG_RETURN((int)(f))
/* following i_rtx-instructions are used if a function was inlined into another */
#define i_rtc(x, l)    stflip(a, w);        \
                       push((int)(x));      \
                       goto I##l
#define i_rtp(x, l)    stflip(a, w);        \
                       INC_REFCNT((T_PTD)(x)); \
                       push((int)(x));      \
                       goto I##l
#define i_rtm(l)       stflip(a, w);        \
                       goto I##l
#define i_rtf(l)       stflip(a, w);        \
                       moveaw();            \
                       goto I##l

#if ONE_FUNCTION

#define rtc(x)         switch(top(r) & TAIL_FLAG)       \
                       {                        \
                         case TAIL_FLAG:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            push((int)x);          \
                            goto LabSwitch;     \
                         }                      \
                         default:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            stflip(a, w);       \
                            push((int)x);          \
                            goto LabSwitch;     \
                         }                      \
                       }
#define rtp(x)         switch(top(r) & TAIL_FLAG)       \
                       {                        \
                         case TAIL_FLAG:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            INC_REFCNT(x); push((int)x);          \
                            goto LabSwitch;     \
                         }                      \
                         default:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            stflip(a,w);       \
                            INC_REFCNT(x); push((int)x);          \
                            goto LabSwitch;     \
                         }                      \
                       }
#define rtm()          switch(top(r) & TAIL_FLAG)       \
                       {                        \
                         case TAIL_FLAG:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            moveaw();          \
                            goto LabSwitch;     \
                         }                      \
                         default:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            stflip(a,w);       \
                            goto LabSwitch;     \
                         }                      \
                       }

                       
#define rtf()          switch(top(r) & TAIL_FLAG)       \
                       {                        \
                         case TAIL_FLAG:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            goto LabSwitch;     \
                         }                      \
                         default:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            stflip(a, w);       \
                            moveaw();           \
                            goto LabSwitch;     \
                         }                      \
                       }

#else /* ONE_FUNCTION */

#define rtc(x)         switch(top(r) & RT_FLAGS)       \
                       {                        \
                         case 0:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            stflip(a, w);       \
                            push((int)x);          \
                            goto LabSwitch;     \
                         }                      \
                         case TAIL_FLAG:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            push((int)x);          \
                            goto LabSwitch;     \
                         }                      \
                         case FUNCTIONRETURN:                \
                         {                      \
                           stflip(a, w);        \
                           push((int)x);          \
                           DBUG_RETURN(kill_bits(pop(r))); \
                         }                      \
                         default:                \
                         {                      \
                           push((int)x);          \
                           DBUG_RETURN(kill_bits(pop(r))); \
                         }                     \
                       }
#define rtp(x)         switch(top(r) & RT_FLAGS)       \
                       {                        \
                         case 0:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            stflip(a,w);       \
                            INC_REFCNT(x); push((int)x);          \
                            goto LabSwitch;     \
                         }                      \
                         case TAIL_FLAG:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            INC_REFCNT(x); push((int)x);          \
                            goto LabSwitch;     \
                         }                      \
                         case FUNCTIONRETURN:                \
                         {                      \
                           stflip(a,w);        \
                           INC_REFCNT(x); push((int)x);          \
                           DBUG_RETURN(kill_bits(pop(r))); \
                         }                      \
                         default:                \
                         {                      \
                           INC_REFCNT(x); push((int)x);          \
                           DBUG_RETURN(kill_bits(pop(r))); \
                         }                     \
                       }
#define rtm()          switch(top(r) & RT_FLAGS)       \
                       {                        \
                         case 0:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            stflip(a,w);       \
                            goto LabSwitch;     \
                         }                      \
                         case TAIL_FLAG:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            moveaw();          \
                            goto LabSwitch;     \
                         }                      \
                         case FUNCTIONRETURN:                \
                         {                      \
                           stflip(a,w);        \
                           DBUG_RETURN(kill_bits(pop(r))); \
                         }                      \
                         default:                \
                         {                      \
                           moveaw();          \
                           DBUG_RETURN(kill_bits(pop(r))); \
                         }                     \
                       }

                       
#define rtf()          switch(top(r) & RT_FLAGS)       \
                       {                        \
                         case 0:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            stflip(a, w);       \
                            moveaw();           \
                            goto LabSwitch;     \
                         }                      \
                         case TAIL_FLAG:                \
                         {                      \
                            lab=kill_bits(pop(r));         \
                            goto LabSwitch;     \
                         }                      \
                         case FUNCTIONRETURN:                \
                         {                      \
                           stflip(a, w);        \
                           moveaw();            \
                           DBUG_RETURN(kill_bits(pop(r))); \
                         }                      \
                         default:                \
                         {                      \
                           DBUG_RETURN(kill_bits(pop(r))); \
                         }                     \
                       }
#endif /* ONE_FUNCTION */
#if D_SLAVE
#define ext()          proc_retval=IR_EXT;DBUG_RETURN(kill_bits(pop(r)))
#else /* D_SLAVE */
#define ext()          DBUG_RETURN(kill_bits(pop(r)))
#endif /* D_SLAVE */

/* Build an interaction descriptor. stt 13.02.96 */
#define mkintact(x)  if (0 == initdata_intact(x))       \
                     {                                     \
                       fprintf(stderr, "FATAL ERROR in mkintact(%d).\n", x); \
                       exit(1);             \
                     }                      \
                     drop(w,FUNC_ARITY(x)); \
                     push((int)_desc)

/************************************************************/
/* PATTERN MATCH MACROS */
/************************************************************/
/* here r is the label to return to, f the func to return to, t the called func */

/* here r is the label to return to */
#define Case(func, r)  push_r(r); \
                       flip_tail();  \
		       goto func
/* here r is the label to return to */
#if D_SLAVE
#define casenear(r,f,t) casefar(r,f,t)
#else
#define casenear(r,f,t) push_r(r); \
                       flip_tail();  \
                       goto L0 
#endif /* D_SLAVE */
/* here r is the label to return to, f the func to return to, t the called func */
#define casefar(r,f,t)    push_r(r);    /* label to return to */           \
                          push_r(((int)f | FUNCTIONRETURN)); /*func to return to*/\
                          flip_tail();  \
                          push_r(0);               \
                          DBUG_RETURN((int)(t)) /* func to call */
#define gammacase(func, ret)    stflip(r, t);    \
                       push_r(ret); \
                       flip_tail();  \
                       goto func 
#if D_SLAVE
#define gammacasenear(ret,f,target) gammacasefar(ret,f,target)
#else
#define gammacasenear(ret,f,target)   \
                       stflip(r, t);    \
                       push_r(ret); \
                       flip_tail();  \
                       goto L0 
#endif /* D_SLAVE */
#define gammacasefar(ret, f, target)    \
                       stflip(r, t);    \
                       push_r(ret);    /* label to return to */           \
                       push_r(((int)f | FUNCTIONRETURN)); /*func to return to*/\
                       flip_tail();  \
                       push_r(0);               \
                       DBUG_RETURN((int)(target)) /* func to call */
#define mkcase(d,funcdesctable)      f_mkcase(-1, (d),funcdesctable)
#define atend(n,label) if (peek(r, n) < peek(w, 1)) goto label
#define atstart(n,label) if (peek(r, n) > peek(w, 1)) goto label
#define fetch()        if ((T_PTD)(int)*(PTR_HEAPELEM)top(w) && T_POINTER((int)*(PTR_HEAPELEM)top(w))) \
                         INC_REFCNT((T_PTD)*(PTR_HEAPELEM)top(w));       \
                       repl(0, w, (int)*(PTR_HEAPELEM)top(w))
#define mkwframe(n)    {int help; \
                        for (help = n; help > 0; help--) push(0);  }    \
                       push(peek(w, (n)+1))
#define mkaframe(n)    { int i; for (i = 0; i < n; i++) push_a(1);}  
#define matchprim(n, label1, label2, failed_when)            \
                       if(n != top(w))            \
                       {             \
                         if (T_POINTER(top(w) & ~F_EDIT))   \
                           if (IS_VAR_OR_CLOS_DESC(R_DESC(*(T_PTD)(top(w) & ~F_EDIT),class))) { push_t(failed_when); push_t(KLAA); goto label2;}           \
                       goto label1;       \
                       }
#define matchint(n, label1, label2, failed_when)            \
                       if(n != top(w))            \
                       {                          \
                         if (T_POINTER(top(w)))   \
                           if  (IS_VAR_OR_CLOS_DESC(R_DESC(*(T_PTD)top(w),class)))  \
                           { push_t(failed_when); push_t(KLAA); goto label2;}           \
                           goto label1;             \
                       }
#define matchbool(n, label1, label2, failed_when)            \
                       if(n != top(w))            \
                       {                          \
                         if (T_POINTER(top(w)))   \
                           if  (IS_VAR_OR_CLOS_DESC(R_DESC(*(T_PTD)top(w),class)))  \
                             { push_t(failed_when); push_t(KLAA); goto label2;}           \
                           goto label1;             \
                       }
#define matchlist(n, label1, label2, failed_when)           \
                       if (T_POINTER(top(w)))   \
                       {                                                  \
                         if (R_DESC(*(T_PTD)top(w), class) != C_LIST ||    \
                            (R_DESC(*(T_PTD)top(w), type) != TY_UNDEF) ||    \
                            (R_LIST(*(T_PTD)top(w), dim) != n))          \
                            {       \
                              if (IS_VAR_OR_CLOS_DESC(R_DESC(*(T_PTD)top(w), class))) \
                                { push_t(failed_when); push_t(KLAA); goto label2;}    \
                              else goto label1;                 \
                            }               \
                       }              \
                       else goto label1
#define matcharb_0(n, label1, label2, failed_when)           \
                       if (T_POINTER(top(w)))   \
                       {                                                  \
                         if (!(R_DESC(*(T_PTD)top(w), class) == C_LIST &&    \
                            (R_DESC(*(T_PTD)top(w), type) == TY_UNDEF)))          \
                            {       \
                              if (IS_VAR_OR_CLOS_DESC(R_DESC(*(T_PTD)top(w), class))) \
                            { push_t(failed_when); push_t(KLAA); goto label2;}      \
                              else goto label1;                 \
                            }               \
                       }              \
                       else goto label1
#define matcharb(n, label1, label2, failed_when)           \
                       if (T_POINTER(top(w)))   \
                       {                                                  \
                         if (!(R_DESC(*(T_PTD)top(w), class) == C_LIST &&    \
                            (R_DESC(*(T_PTD)top(w), type) == TY_UNDEF) &&    \
                            (R_LIST(*(T_PTD)top(w), dim) >= n)))          \
                            {       \
                              if (IS_VAR_OR_CLOS_DESC(R_DESC(*(T_PTD)top(w), class))) \
                            { push_t(failed_when); push_t(KLAA); goto label2;}      \
                              else goto label1;                 \
                            }               \
                       }              \
                       else goto label1
#define matchstr(n, label1, label2, failed_when)              \
                       if (T_POINTER(top(w)))   \
                       {                                                  \
                         if (R_DESC(*(T_PTD)top(w), class) != C_LIST ||    \
                            (R_DESC(*(T_PTD)top(w), type) != TY_STRING) ||    \
                            (R_LIST(*(T_PTD)top(w), dim) != n))          \
                            {       \
                              if (IS_VAR_OR_CLOS_DESC(R_DESC(*(T_PTD)top(w), class))) \
                               { push_t(failed_when); push_t(KLAA); goto label2;}           \
                              else goto label1;                             \
                            }            \
                       }              \
                       else goto label1
#define matcharbs(n, label1, label2, failed_when)           \
                       if (T_POINTER(top(w)))   \
                       {                                                  \
                         if (R_DESC(*(T_PTD)top(w), class) != C_LIST ||    \
                            (R_DESC(*(T_PTD)top(w), type) != TY_STRING) ||    \
                            (R_LIST(*(T_PTD)top(w), dim) < n))          \
                            {       \
                              if (IS_VAR_OR_CLOS_DESC(R_DESC(*(T_PTD)top(w), class))) \
                                { push_t(failed_when); push_t(KLAA); goto label2;}           \
                              else goto label1;                 \
                            }               \
                       }              \
                       else goto label1
#define matchin(arg, label1, label2, failed_when)   \
                       if (T_POINTER(top(w)) &&   \
                          ((R_DESC(*(T_PTD)top(w), type) == TY_EXPR) ||        \
                           (R_DESC(*(T_PTD)top(w), type) == TY_NAME))) \
                           { push_t(failed_when); push_t(KLAA); goto label2;}           \
                       if (T_LETTER(top(w)))             \
                       {                 \
                         int i;                \
                         for (i = 0; i < R_LIST(*(T_PTD)arg, dim); i++)   \
                           if ((top(w) & F_VALUE) ==     \
                             ((R_LIST(*(T_PTD)arg,ptdv))[i] & F_VALUE)) \
                             break;     \
                             if (R_LIST(*(T_PTD)arg,dim) == i)   goto label1;     \
                        }                                  \
                        else goto label1
#define saveptr(n)     repl((n), w, peek(w, 1))
#define nestlist()     if (0 != R_LIST(*(T_PTD)top(w), dim))      \
                       {                                        \
                         repl(1, w, (int) R_LIST(*(T_PTD)top(w), ptdv));     \
                       }        \
                       else     \
                         repl(1, w, (int)_nil)
                       
#define dereference()  repl(0, w, *((PTR_HEAPELEM)peek(w, 1)))
#define bind(n)        if (T_POINTER(peek(a, (n))))     \
                         DEC_REFCNT((T_PTD)peek(a, (n)));     \
                       if (T_POINTER(top(w)))     \
                         INC_REFCNT((T_PTD)top(w));     \
                       repl(n, a, top(w))
#define advance(n)     repl(1, w, (int)((PTR_HEAPELEM)peek(w, 1) + n) )
#define restoreptr(n)  repl(1, w, peek(w, n))
/* address is the index of the actual when in funcdesctable */
#define tguard(label, address, wsize, rsize, psize,funcdesctable)    \
                       if (IS_TRUE(top(w))) pop(w);                 \
                       else               \
                         if (IS_FALSE(top(w))) { pop(w); goto label;} \
                       else                                         \
                       {                                            \
                         int i, arg;            \
                         arg = pop(w);       \
                         for(i = wsize; i > 0; i--)         \
                         {        \
                           pop(w);          \
                         }        \
                         for(i = rsize; i > 0; i--)         \
                         {        \
                           pop(r);          \
                         }        \
                         push_r(SET_VALUE(DOLLAR, psize));    \
                         push((int)arg);                \
                         guard_undecided(address,funcdesctable);      \
                         rtf();    \
                       }
/* ad is the index of the actual when in funcdesctable */
#define guard_undecided(ad,funcdesctable) f_guard_undecided(ad,funcdesctable)
#define rmwframe(n)   { int i; for (i = 0; i < n; i++) { \
                        drop(w, 1);  }}
#define startsubl(n, m) repl(n, r, top(w));         \
                      repl(m, r, peek(w, 1))
#define pick(n)       repl(0, w, peek(w, n))
#define initbt(n, i1, i2, i3, i4)     \
                      repl(i1, r, peek(w, 1)); \
                      repl(i2, r, peek(w, 1)); \
                      repl(i3, r, top(w));     \
                      repl(i4, r, (int)((PTR_HEAPELEM)R_LIST(*(T_PTD)top(w), ptdv)     \
                                  + R_LIST(*(T_PTD)top(w), dim) - n))
#define restorebt(n)  repl(1, w, peek(r, n))
#define savebt(n)     repl((n), r, peek(w, 1))
#define rmbtframe(n)  { int i; for (i = 0; i < n; i++) { \
                      drop(r, 1);  }}
#define endsubl(n)    repl(n, r, peek(w, 1))
#define endlist()     repl(1, w, (int)(R_LIST(*(T_PTD)top(w), ptdv) + \
                         R_LIST(*(T_PTD)top(w), dim)))
#define binds(n)      if (T_POINTER(peek(a, n))) DEC_REFCNT((PTR_DESCRIPTOR)peek(a, n)); \
                      if (T_POINTER(top(w))) INC_REFCNT((PTR_DESCRIPTOR)top(w));    \
                      MAKEDESC(_desc, 1, C_LIST, TY_STRING);           \
                      L_LIST(*_desc, ptdd) = 0;                         \
                      GET_HEAP(1, A_LIST(*(T_PTD)_desc, ptdv));      \
                      L_LIST(*(T_PTD)_desc, dim) = 1;              \
                      RES_HEAP;            \
                      L_LIST(*(T_PTD)_desc, ptdv)[0] = top(w);           \
                      REL_HEAP;               \
                      repl(n, a, (int)_desc)
#define bindsubl(n, m, l, k)  \
                      { T_PTD ptdesc;     \
                        int f, h;         \
                        f = (PTR_HEAPELEM)peek(r,m) - R_LIST(*(T_PTD)peek(r,n), ptdv);   \
                        h = (PTR_HEAPELEM)peek(r,l) - R_LIST(*(T_PTD)peek(r,n),ptdv) - 1; \
                        if (h < f)              \
                        {             \
                          if (R_DESC(*(T_PTD)peek(r, n), type) == TY_STRING)       \
                          {             \
                            repl(k, a,  (int)_nilstring);        \
                            INC_REFCNT(_nilstring);      \
                          }             \
                          else      \
                          {         \
                            repl(k, a, (int)_nil);      \
                            INC_REFCNT(_nil);   \
                          }        \
                        }       \
                        else    \
                          repl(k, a, gen_ind(f, h, (T_PTD)peek(r, n)));    \
                      }
#define mkbtframe(n)  { int i; for (i = 0; i < n; i++) push_r(1);}  
                          
/************************************************************/
/* PATTERN MATCH MACROS WITHOUT REFERENCE COUNTING */
/************************************************************/

#define nr_fetch()      repl(0, w, (int)*(PTR_HEAPELEM)top(w))                        

#define nr_bind(n)     repl(n, a, top(w))

#define nr_binds(n)   MAKEDESC(_desc, 1, C_LIST, TY_STRING);           \
                      L_LIST(*_desc, ptdd) = 0;                         \
                      GET_HEAP(1, A_LIST(*(T_PTD)_desc, ptdv));      \
                      L_LIST(*(T_PTD)_desc, dim) = 1;              \
                      RES_HEAP;            \
                      L_LIST(*(T_PTD)_desc, ptdv)[0] = top(w);           \
                      REL_HEAP;               \
                      repl(n, a, (int)_desc)

#define nr_bindsubl(n, m, l, k)  \
                      { T_PTD ptdesc;     \
                        int f, h;         \
                        f = (PTR_HEAPELEM)peek(r,m) - R_LIST(*(T_PTD)peek(r,n), ptdv);   \
                        h = (PTR_HEAPELEM)peek(r,l) - R_LIST(*(T_PTD)peek(r,n),ptdv) - 1; \
                        if (h < f)              \
                        {             \
                          if (R_DESC(*(T_PTD)peek(r, n), type) == TY_STRING)       \
                          {             \
                            repl(k, a,  (int)_nilstring);        \
                            INC_REFCNT(_nilstring);      \
                          }             \
                          else      \
                          {         \
                            repl(k, a, (int)_nil);      \
                            INC_REFCNT(_nil);   \
                          }        \
                        }       \
                        else    \
                          repl(k, a, gen_ind(f, h, (T_PTD)peek(r, n)));    \
                      }

/************************************************************/
/* END OF PATTERN MATCH MACROS */
/************************************************************/

/************************************************************/
/* START OF INSTRUCTIONS FOR DISTRIBUTED COMPUTED */
/************************************************************/

#if D_SLAVE
#define map(f, l)               ((((int)f)<<16)|(int)l)
#define unmapl(x)               ((x) & 0xffff)
#define unmapf(x)               ((x) >> 16)

/* for push_h see stack operations
 * #define push_h(n)
 */
#define pop_h()                pop(r)
#define count(x)               MPRINT_COUNT (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, x);
#define msdistend()            if ((top(r)) >= 0) { \
                                 DBUG_PRINT ("INTER", ("DISTEND zurueck an Prozessor %d !", top(r)>>1)); \
                                 MPRINT_PROC_DISTEND(D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID, (int)curr_pid->ppid, top(r)>>1); }
#define msnodist()             MPRINT_PROC_NODIST (D_MESS_FILE, M_TIMER, PROCESS_NR, PROCESS_PID);

#if WITHTILDE
#define dist(f, l1, l2, b, r, t, tf)  lab =  unmapl((int)D_DISTRIBUTE(map(f,l1),map(f,l2),b,r,t,tf, DIST));\
                               DBUG_PRINT("DIST",("distribute lab=%d",lab));\
                               goto LabSwitch
#define distb(f, l1, l2, b, u, t, tf) DBUG_PRINT("DIST",("distribute count=%d", top(r))); \
                               lab =  unmapl((int)D_DISTRIBUTE(map(f,l1),map(f,l2),b,u,t,tf, DIST_B));\
                               DBUG_PRINT("DIST",("distribute lab=%x",lab));\
                               goto LabSwitch
#define distend()              if ((top(r)) >= 0) {            \
                                 DBUG_PRINT("DISTEND", ("sending back to %d", top(r)>>1));\
                                 msg_send(pop(r)>>1,MT_RESULT);   \
                                 proc_retval = IR_DIST_END;    \
                                 DBUG_RETURN (0);              \
                                 }                             \
                               else {                           \
                                 pop(r); \
                                 repl(0, r, top(r)-1);         \
                                 DBUG_PRINT("DIST",("distend count=%d",top(r))); }

#else /* WITHTILDE */
#define dist(f, l1, l2, b, r)  lab =  unmapl((int)D_DISTRIBUTE(map(f,l1),map(f,l2),b,r, DIST));\
                               DBUG_PRINT("DIST",("distribute lab=%x",lab));\
                               goto LabSwitch
#define distb(f, l1, l2, b, r) lab =  unmapl((int)D_DISTRIBUTE(map(f,l1),map(f,l2),b,r, DIST_B));\
                               DBUG_PRINT("DIST",("distribute lab=%x",lab));\
                               goto LabSwitch
#define distend()              if ((top(r)) >= 0) {            \
			         DBUG_PRINT("DISTEND", ("sending back to %d", top(r)));\
                                 msg_send(pop(r),MT_RESULT);   \
                                 proc_retval = IR_DIST_END;    \
                                 DBUG_RETURN (0);              \
                                 }                             \
                               else {                           \
			         pop(r); \
                                 repl(0, r, top(r)-1);         \
                                 DBUG_PRINT("DIST",("distend count=%d",top(r))); }
#endif /* WITHTILDE */
# define wait(f, l, n)         repl(0, r, top(r)-n);           \
                               code=map(f,(int *)l);           \
			       DBUG_PRINT("DIST",("wait count=%d",top(r)));\
                               if (top(r)) {                   \
                                 proc_retval = IR_WAIT;        \
                                 DBUG_RETURN (0);              \
                                 }

#endif /* D_SLAVE */

/************************************************************/
/* END OF INSTRUCTIONS FOR DISTRIBUTED COMPUTED */
/************************************************************/

#include "tprimf.h"

#ifndef PROLOG_EXTERN

int call_func;
int proc_retval;
/* KiROutput : Output form for to retranslate (into file tasm2kir); 
   AsciiOutput : readable Output (stdout) 
*/
int KiROutput = 0, AsciiOutput = 1;
/* sizes for InitHeapManagement(), used in c_main.c */
char ri_tasmoutfile[120] = "__default";

int *_st_a;
int *_st_w;
int *_st_t;
int *_st_r;
extern PTR_DESCRIPTOR _desc;
extern FCTPTR dealloc_tab[];
extern int _base;
void initdata();

#ifdef __sparc__ 
#if !NO_STACK_REG
  register int *st_h asm ("g5");
  register int *st_w asm ("g7");
  register int *st_a asm ("g6");
#else
  int *st_h;
  int *st_w;
  int *st_a;
#endif
#else
  int *st_h;
  int *st_w;
  int *st_a;
#endif

int *st_t;
int *st_r;
int lab; 
int tail_flag = 0;
#else /* PROLOG_EXTERN */
extern int call_func, proc_retval;
/* KiROutput : Output form for to retranslate (into file tasm2kir);
   AsciiOutput : readable Output (stdout)
*/
extern int KiROutput, AsciiOutput;
/* sizes for InitHeapManagement(), used in c_main.c */
extern char ri_tasmoutfile[120];
extern int *_st_a;
extern int *_st_w;
extern int *_st_t;
extern int *_st_r;
extern PTR_DESCRIPTOR _desc;
extern FCTPTR dealloc_tab[];
extern int _base;
void initdata();

#ifdef __sparc__
#if !NO_STACK_REG
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
extern int lab;
extern int tail_flag;

#endif  /* PROLOG_EXTERN */
