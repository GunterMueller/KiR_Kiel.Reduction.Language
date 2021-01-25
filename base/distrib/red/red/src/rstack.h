/* $Log: rstack.h,v $
 * Revision 1.1  1995/12/14 16:59:49  rs
 * Initial revision
 *
 * */

/****************************************************************************/
/* file rstack.h                                                            */
/*                                                                          */
/* - macros for stack access (processing only)                              */
/*                                                                          */
/****************************************************************************/

#ifndef RSTACK_H_INCLUDED
#define RSTACK_H_INCLUDED

#include "rextern.h"
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif

#if D_SLAVE
extern DStackDesc *ps_w,*ps_a,*ps_t,*ps_r;
#else
extern StackDesc *ps_w,*ps_a,*ps_t,*ps_r;
#endif

#ifdef  PUSH_A
#undef  PUSH_A
#endif

#if D_SLAVE 
#define POP_W()           D_POPSTACK(*ps_w)
#define PPOP_W()          D_PPOPSTACK(*ps_w)
#define TOP_W()           D_READSTACK(*ps_w)
#define PUSH_W(x)         D_PUSHSTACK(*ps_w,(x))
#define PPUSH_W(x)        D_PPUSHSTACK(*ps_w,(x))
#define WRITE_W(x)        D_WRITESTACK(*ps_w,(x))
#define MID_W(n)          D_MIDSTACK(*ps_w,n)
#define UPDATE_W(n,x)     D_UPDATESTACK(*ps_w,n,(x))

#define POP_A()           D_POPSTACK(*ps_a)
#define PPOP_A()          D_PPOPSTACK(*ps_a)
#define TOP_A()           D_READSTACK(*ps_a)
#define PUSH_A(x)         D_PUSHSTACK(*ps_a,(x))
#define PPUSH_A(x)        D_PPUSHSTACK(*ps_a,(x))
#define WRITE_A(x)        D_WRITESTACK(*ps_a,(x))
#define MID_A(x)          D_MIDSTACK(*ps_a,x)
#define UPDATE_A(n,x)     D_UPDATESTACK(*ps_a,n,(x))

#if WITHTILDE
#define POP_R()           D_POPSTACK(*ps_r)
#define PPOP_R()          D_PPOPSTACK(*ps_r)
#define TOP_R()           D_READSTACK(*ps_r)
#define PUSH_R(x)         D_PUSHSTACK(*ps_r,(x))
#define PPUSH_R(x)        D_PPUSHSTACK(*ps_r,(x))
#define WRITE_R(x)        D_WRITESTACK(*ps_r,(x))
#define MID_R(x)          D_MIDSTACK(*ps_r,(x))
#define UPDATE_R(n,x)     D_UPDATESTACK(*ps_r,n,(x))
#else
#define POP_R()           D_POPSTACK(D_S_R)
#define PPOP_R()          D_PPOPSTACK(D_S_R)
#define TOP_R()           D_READSTACK(D_S_R)
#define PUSH_R(x)         D_PUSHSTACK(D_S_R,(x))
#define PPUSH_R(x)        D_PPUSHSTACK(D_S_R,(x))
#define WRITE_R(x)        D_WRITESTACK(D_S_R,(x))
#define MID_R(x)          D_MIDSTACK(D_S_R,(x))
#define UPDATE_R(n,x)     D_UPDATESTACK(D_S_R,n,(x))
#endif /* WITHTILDE */

#define POP_E()           D_POPSTACK(D_S_H)
#define PPOP_E()          D_PPOPSTACK(D_S_H)
#define TOP_E()           D_READSTACK(D_S_H)
#define PUSH_E(x)         D_PUSHSTACK(D_S_H,(x))
#define PPUSH_E(x)        D_PPUSHSTACK(D_S_H,(x))
#define WRITE_E(x)        D_WRITESTACK(D_S_H,(x))
#define MID_E(x)          D_MIDSTACK(D_S_H,(x))
#define UPDATE_E(n,x)     D_UPDATESTACK(D_S_H,n,(x))

#if WITHTILDE
#define POP_T()           D_POPSTACK(*ps_t)
#define PPOP_T()          D_PPOPSTACK(*ps_t)
#define TOP_T()           D_READSTACK(*ps_t)
#define PUSH_T(x)         D_PUSHSTACK(*ps_t,(x))
#define PPUSH_T(x)        D_PPUSHSTACK(*ps_t,(x))
#define WRITE_T(x)        D_WRITESTACK(*ps_t,(x))
#define MID_T(x)          D_MIDSTACK(*ps_t,(x))
#define UPDATE_T(n,x)     D_UPDATESTACK(*ps_t,n,(x))
#endif /* WITHTILDE */

#else

#define POP_W()           POPSTACK(*ps_w)
#define PPOP_W()          PPOPSTACK(*ps_w)
#define TOP_W()           READSTACK(*ps_w)
#define PUSH_W(x)         PUSHSTACK(*ps_w,(x))
#define PPUSH_W(x)        PPUSHSTACK(*ps_w,(x))
#define WRITE_W(x)        WRITESTACK(*ps_w,(x))
#define MID_W(n)          MIDSTACK(*ps_w,n)
#define UPDATE_W(n,x)     UPDATESTACK(*ps_w,n,(x))

#define POP_A()           POPSTACK(*ps_a)
#define PPOP_A()          PPOPSTACK(*ps_a)
#define TOP_A()           READSTACK(*ps_a)
#define PUSH_A(x)         PUSHSTACK(*ps_a,(x))
#define PPUSH_A(x)        PPUSHSTACK(*ps_a,(x))
#define WRITE_A(x)        WRITESTACK(S_i,(x))
#define MID_A(x)          MIDSTACK(*ps_a,x)
#define UPDATE_A(n,x)     UPDATESTACK(*ps_a,n,(x))

#if WITHTILDE
#define POP_R()           POPSTACK(*ps_r)
#define PPOP_R()          PPOPSTACK(*ps_r)
#define TOP_R()           READSTACK(*ps_r)
#define PUSH_R(x)         PUSHSTACK(*ps_r,(x))
#define PPUSH_R(x)        PPUSHSTACK(*ps_r,(x))
#define WRITE_R(x)        WRITESTACK(*ps_r,(x))
#define MID_R(x)          MIDSTACK(*ps_r,(x))
#define UPDATE_R(n,x)     UPDATESTACK(*ps_r,n,(x))
#else
#define POP_R()           POPSTACK(S_m)
#define PPOP_R()          PPOPSTACK(S_m)
#define TOP_R()           READSTACK(S_m)
#define PUSH_R(x)         PUSHSTACK(S_m,(x))
#define PPUSH_R(x)        PPUSHSTACK(S_m,(x))
#define WRITE_R(x)        WRITESTACK(S_m,(x))
#define MID_R(x)          MIDSTACK(S_m,(x))
#define UPDATE_R(n,x)     UPDATESTACK(S_m,n,(x))
#endif /* WITHTILDE */

#define POP_E()           POPSTACK(S_e)
#define PPOP_E()          PPOPSTACK(S_e)
#define TOP_E()           READSTACK(S_e)
#define PUSH_E(x)         PUSHSTACK(S_e,(x))
#define PPUSH_E(x)        PPUSHSTACK(S_e,(x))
#define WRITE_E(x)        WRITESTACK(S_e,(x))
#define MID_E(x)          MIDSTACK(S_e,(x))
#define UPDATE_E(n,x)     UPDATESTACK(S_e,n,(x))

/* mah 261093 */
#if WITHTILDE
#define POP_T()           POPSTACK(*ps_t)
#define PPOP_T()          PPOPSTACK(*ps_t)
#define TOP_T()           READSTACK(*ps_t)
#define PUSH_T(x)         PUSHSTACK(*ps_t,(x))
#define PPUSH_T(x)        PPUSHSTACK(*ps_t,(x))
#define WRITE_T(x)        WRITESTACK(*ps_t,(x))
#define MID_T(x)          MIDSTACK(*ps_t,(x))
#define UPDATE_T(n,x)     UPDATESTACK(*ps_t,n,(x))
#endif /* WITHTILDE */

#endif /* D_SLAVE */

#endif
