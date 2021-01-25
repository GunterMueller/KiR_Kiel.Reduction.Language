#include <stdio.h>
#include <string.h>
#include "ris.h"
#include "main.h"
#include "mytypes.h"
#include "buildtree.h"
#include "support.h"
#include "tiny.h"
#include "heap.h"
#include "stack.h"
#include "error.h"
#include "dbug.h"

char * vris = VRIS;
static setrefmode refmode;
/* refinserted mechanism (only used for PM):
 *   after leaving the match code the reference counts of any
 *   freshly bound variables are NOT modified. The internal increment
 *   counter is set to 1 (unless shared). During execution of the
 *   guard increment instruction may be issued which also include
 *   the above modification. If the guard fails the modification
 *   has to be undone, because there is only _one_ destination
 *   for a failed clause.
 * The approach taken here is the following:
 *   if ref-count instructions are to be issued for guard code
 *   the modification is offset by 1 (for the bind).
 * refinserted is set to -1 if no PM is done (or the guard is surely left);
 *   0 if in PM code (perhaps in guard), and 1 if a instruction
 *   has already been issued in guard (this cause a complete syncronisation
 *   at the tguard instruction with the above offset)
 */
static int refinserted;

astack(W); astack(A); astack(R); astack(T); astack(H); astack(H1); astack (H2);

#ifndef DBUG_OFF

/* prstack: prints a stack in decoded form to stderr
 */

static void prstack(const char * str, argstack(S), const int m)
  {
  int i, j = 0;
  desc * p;

  fprintf(stderr, "%s: ", str);
  for(i = 0; peek(S, i) != BOTTOM; i++)
    if (m && (j == 0)) {
      j = peek(S, i);
      fprintf(stderr, "#%d ", j);
      }
    else if (peek(S, i) < lastcmd) {
      if (strcmp(str, "H") == 0)
        switch((doneid)peek(S, i)) {
          case FSYNC: fprintf(stderr, "FS "); break;
          case SYNC: fprintf(stderr, "S "); break;
          case SYNCMERGE: fprintf(stderr, "SM "); break;
          case FSYNCMERGE: fprintf(stderr, "FSM "); break;
          case NOOP: fprintf(stderr, "N "); break;
          case CSYNC: fprintf(stderr, "CS "); break;
          case CSYNCMERGE: fprintf(stderr, "CSM "); break;
          default: fprintf(stderr, "**%d** ", peek(S, i));
          }
      else
        fprintf(stderr, "%d ", peek(S, i));
      j--;
      }
    else {
      p = (desc *)peek(S, i);
      j--;
      fprintf(stderr, "%p(%c%c%d) ", p,
        (rnode(p,type) == TYshared ? 's' : '-'),
        (rnode(p,mode) == CLnoref ? 'n' : rnode(p,mode) == CLref ? 'r' : 't'),
        rnode(p, refcnt));
      }
  fprintf(stderr, "\n");
  }

#endif /* DBUG_OFF */

/* risk determines if a decrement operation may be moved over
 * the corresponding instruction
 * THIS IS A POSITIVE INDEX, OTHER ENTRIES ARE DANGEROUS!
 */
static int norisk[lastcmd];
static int isterm[lastcmd];

void initnorisk()
  {
  norisk[delta1] = 1; norisk[delta2] = 1; norisk[delta3] = 1; norisk[delta4] = 1;
  norisk[freea] = 1; norisk[freer] = 1; norisk[freeswt] = 1; norisk[freet] = 1;
  norisk[freew] = 1; norisk[label] = 1; norisk[mkap] = 1; norisk[mkbclos] = 1;
  norisk[mkcclos] = 1; norisk[mkdclos] = 1; norisk[mkgaclos] = 1; norisk[mkgclos] = 1;
  norisk[mkgsclos] = 1; norisk[mkilist] = 1; norisk[mklist] = 1; norisk[mksclos] = 1;
  norisk[mkiclos] = 1; norisk[intact] = 1;
  norisk[movear] = 1; norisk[moveaw] = 1; norisk[movetr] = 1; norisk[movetw] = 1;
  norisk[pushar] = 1; norisk[pushaw] = 1; norisk[pushr] = 1; norisk[pushr_p] = 1;
  norisk[pushtr] = 1; norisk[pushtw] = 1; norisk[pushw] = 1; norisk[pushcw_pf] = 1;
  norisk[pushw_p] = 1; norisk[pushcw_i] = 1; norisk[pushcw_b] = 1; norisk[pushcr_i] = 1;
  norisk[pushcr_b] = 1; norisk[pushcw_r] = 1; norisk[pushaux] = 1; norisk[pushh] = 1;
  norisk[pushret] = 1; norisk[incw] = 1; norisk[inca] = 1; norisk[incr] = 1;
  norisk[inct] = 1; norisk[tincw] = 1; norisk[tinca] = 1; norisk[tincr] = 1;
  norisk[tinct] = 1; norisk[decw] = 1; norisk[deca] = 1; norisk[decr] = 1;
  norisk[dect] = 1; norisk[tdecw] = 1; norisk[tdeca] = 1; norisk[tdecr] = 1;
  norisk[tdect] = 1; norisk[killw] = 1; norisk[killa] = 1; norisk[killr] = 1;
  norisk[killt] = 1; norisk[tkillw] = 1; norisk[tkilla] = 1; norisk[tkillr] = 1;
  norisk[tkillt] = 1; norisk[hashargs] = 1; norisk[hashtildeargs] = 1;
  norisk[hashrestype] = 1; norisk[hashsetref] = 1; norisk[advance] = 1;
  norisk[bind] = 1; norisk[binds] = 1; norisk[bindsubl] = 1; norisk[dereference] = 1;
  norisk[drop] = 1; norisk[endlist] = 1; norisk[endsubl] = 1; norisk[fetch] = 1;
  norisk[initbt] = 1; norisk[mkcase] = 1; norisk[mkwframe] = 1;
  norisk[nestlist] = 1; norisk[pick] = 1; norisk[restorebt] = 1; norisk[restoreptr] = 1;
  norisk[rmbtframe] = 1; norisk[rmwframe] = 1; norisk[savebt] = 1;
  norisk[saveptr] = 1; norisk[startsubl] = 1;

  isterm[rtc] = 1; isterm[rtc_b] = 1; isterm[rtc_i] = 1; isterm[rtc_pf] = 1;
  isterm[i_rtc_b] = 1; isterm[i_rtc_i] = 1; isterm[i_rtf] = 1; isterm[i_rtt] = 1;
  isterm[i_rtp] = 1; isterm[i_rtm] = 1; isterm[i_rtm] = 1; isterm[i_rtc_pf] = 1;
  isterm[rtf] = 1; isterm[rtm] = 1; isterm[rtm_t] = 1; isterm[rtp] = 1;
  isterm[rtt] = 1; isterm[end] = 1; isterm[tguard] = 1;
  }

/* test if instruction is in guard of PM
 * e.g. is followed by a tguard instruction instead of an rtf or end
 */

static int testguard(ORDER * cmd)
  {
  while(isterm[GETORDERENTRY(cmd, command)] == 0)
    cmd = GETNEXTORDER(cmd);
  return GETORDERENTRY(cmd, command) == tguard;
  }
    
/* insert an increment refcount instruction before intruction p,
 * stack may be defined on invocation
 */
static void mkinc(ORDER * cmd, desc * p, const stackid stid, const int off)
  {
  int i;
  COMMAND cmdid;

  DBUG_ENTER("mkinc");
  if ((int)p < lastcmd)
    DBUG_VOID_RETURN;
  DBUG_PRINT("REF", ("INC: refcnt=%d", rnode(p, refcnt)));
  /* test if ref-count operation is done in guard of PM
   */
  if ((refinserted == 1) || ((refinserted == 0) && testguard(cmd))) {
    for(i = 0; i < top(A); i++)
      if (peek(A, i+1) > lastcmd)
        lnode((desc *)peek(A, i+1), refcnt)--;
    refinserted = 1;
    DBUG_PRINT("RIS", ("trying reduced INC*"));
    }
  if ((rnode(p, refcnt) > 0) && (rnode(p, mode) != CLnoref)) {
    DBUG_PRINT("REF", ("INC: refcnt=%d", rnode(p, refcnt)));
    if (rnode(p, prev))
      /* update previous increment operation */
      GETORDERARGS(rnode(p, prev),m) += rnode(p, refcnt);
    else {
      switch(stid) {
        case ST_A: if (rnode(p, mode) == CLref) cmdid = inca; else cmdid = tinca;
          break;
        case ST_R: if (rnode(p, mode) == CLref) cmdid = incr; else cmdid = tincr;
          break;
        case ST_T: if (rnode(p, mode) == CLref) cmdid = inct; else cmdid = tinct;
          break;
        case ST_W: if (rnode(p, mode) == CLref) cmdid = incw; else cmdid = tincw;
          break;
        default:
          yyfail("internal error: illegal stack");
        }
      insert_order_list(rnode(p, prev) = new_order(cmdid, off, rnode(p, refcnt)), cmd);
      DBUG_PRINT("RIS", ("INC* inserted"));
      }
    lnode(p, type) = TYshared;
    lnode(p, refcnt) = 0;
    if (refinserted == 1)
      for(i = 0; i < top(A); i++)
        if (peek(A, i+1) > lastcmd)
          lnode((desc *)peek(A, i+1), refcnt)++;
    }
  else
    lnode(p, refcnt) = 0;
  DBUG_VOID_RETURN;
  }

/* insert an decrement refcount instruction before intruction p,
 * stack may be defined on invocation
 */
static void mkdec(ORDER * cmd, desc * p, const stackid stid, const int off)
  {
  int f = 0;
  ORDER * pc;
  COMMAND cmdid;

  DBUG_ENTER("mkdec");
  if ((int)p < lastcmd)
    DBUG_VOID_RETURN;
  DBUG_PRINT("REF", ("DEC: refcnt=%d", rnode(p, refcnt)));
  if ((rnode(p, refcnt) == 0) && (rnode(p, mode) != CLnoref))
    { 
    if (rnode(p, prev))
      for(pc = cmd, f = 1; pc && (pc != rnode(p, prev)); pc = GETPREVORDER(pc))
        f &= norisk[GETORDERENTRY(pc, command)];
    if (f && (GETORDERARGS(rnode(p, prev),m) > 1))
      GETORDERARGS(rnode(p, prev),m)--;
    else {
      switch(stid) {
        case ST_A: if (rnode(p, mode) == CLref) cmdid = deca; else cmdid = tdeca;
          break;
        case ST_R: if (rnode(p, mode) == CLref) cmdid = decr; else cmdid = tdecr;
          break;
        case ST_T: if (rnode(p, mode) == CLref) cmdid = dect; else cmdid = tdect;
          break;
        case ST_W: if (rnode(p, mode) == CLref) cmdid = decw; else cmdid = tdecw;
          break;
        default:
          yyfail("internal error: illegal stack");
        }
      insert_order_list(new_order(cmdid, off), cmd);
      lnode(p, refcnt) = 0;
      DBUG_PRINT("RIS", ("DEC* inserted"));
      }
    }
  else
    rnode(p, refcnt) -= 1;
  DBUG_PRINT("REF", ("DEC: refcnt=%d", rnode(p, refcnt)));
  DBUG_VOID_RETURN;
  }

/* converts an external type into CLref/CLnoref/CLtest
 */

static typeclass convtype(const TP_TYPES t)
  {
  switch(t & EXT_MASK) {
    case 0: switch(t & PRIM_MASK) {
      case tp_int:
      case tp_bool: return CLnoref;
      case tp_real:
      case tp_digit:
      case tp_string: return CLref;
      }
      break;
    case tp_list:
    case tp_vect:
    case tp_tvect:
    case tp_matrix: return CLref;
    }
  return CLtest;
  }

/* copy argument frame 
 * mode :
 *   if 0 then top of stack is frame size, and top element is just duplicated
 *   otherwise size is determined by (null) or (bottom) element in stack
 * make copies of all members in frame
 * if mode==0 then copy all references afterwards
 */
static stack * cpframe(argstack(S), int m)
  {
  int i, j, n, h = 0;
  desc * p, * q;

  DBUG_ENTER("cpframe");
  if (m == 0)
    n = top(S);
  else {
    push(S, (stack)NULL);
    for(n = 1; ((peek(S, n) != BOTTOM) && (peek(S, n) != 0)); n++);
    n--;
    }
  DBUG_PRINT("RISAUX", ("cpframe size %d", n));
  for(i = 0; i < n;  i++) {
    p = (desc *)peek(S, n); q = NULL;
    if ((int)p < lastcmd)
      push(S, (stack)p);
    else {
      for(j = 0; peek(H1, j) != BOTTOM; j++)
        if ((desc *)peek(H1, j) == p)
          q = (desc *)peek(H2, j);
      if (q)
        push(S, (stack)q);
      else {
        push(S, (stack)ndesc(rnode(p, type), rnode(p, refcnt), rnode(p, mode)));
        push(H1, (stack)p); push(H2, top(S)); h++;
        lnode(p, prev) = NULL;
        }
      }
    }
  push(S, (m ? 0 : n));
  for(i = 0; i < n;  i++) {
    p = (desc *)peek(S, 2*n+1);
    push(S, (stack)p);
    }
  if (m == 0)
    push(S, n);
  drop(H1, h), drop(H2, h);
  DBUG_RETURN(getstack(S));
  }

/* restore argument frame from copy after completing
 * one alternative of conditional
 */
static stack * restoreframe(argstack(S), int m)
  {
  int i, n;
  desc * p, * q;

  DBUG_ENTER("restoreframe");
  if (m)
    for(n = 0; peek(S, n) != 0; n++) ;
  else
    n = pop(S);
  for(i = 0; i < n;  i++) {
    p = (desc *)pop(S);
    q = (desc *)peek(S, n);
    DBUG_PRINT("RISAUX", ("%d/%d p=%p, q=%p", i, n, p, q));
    if (((int)p > lastcmd) && ((int)q > lastcmd)) {
      lnode(q, type) = rnode(p, type);
      lnode(q, refcnt) = rnode(p, refcnt);
      lnode(q, mode) = rnode(p, mode);
      lnode(q, prev) = NULL;
      }
    }
  if (m)
    drop(S, 1);
  DBUG_RETURN(getstack(S));
  }

/* cmpres: compares the types of the both topmost
 * stack entries and replaces them by a element
 * created with the least specific entries.
 */
static stack * cmpres(argstack(S))
  {
  desc * p, * q;

  DBUG_ENTER("cmpres");
  p = (desc *)pop(S); q = (desc *)pop(S);
  if (rnode(p, refcnt) && rnode(q, refcnt))
    yywarn("INTERNAL ERROR: reference count not zero");
  push(S, (stack)ndesc(
      ((rnode(p, type) == TYdetructive) && (rnode(q, type) == TYdetructive) ?
          TYdetructive : TYshared),
      0,
      ((rnode(p, mode) == CLref) && (rnode(q, mode) == CLref) ? CLref :
          ((rnode(p, mode) == CLnoref) && (rnode(q, mode) == CLnoref) ?
              CLnoref : CLtest))
      ));
  DBUG_RETURN(getstack(S));
  }

/* performs all clean up on the 5 stacks
 * synchronisation of reference count has been done
 * outside of cleanstacks
 * called for rtf/jump/label
 */
static void cleanstacks(ORDER * order, COMMAND cmd)
  {
  int i;

  DBUG_ENTER("cleanstacks");
  /* always clear 'previous' refences on stack T
   */
  for(i = 0; i < top(T); i++)
    lnode((desc *)peek(T, i+1), prev) = NULL;
  switch((doneid)top(H)) {
    case FSYNC:
      /* clear all stacks
       *   remove argument frame, remove result
       */
      mkinc(order, (desc *)peek(W, 0), ST_W, 0);
      drop(W, 1);
      freeframe(A, order);
      break;
    case FSYNCMERGE:
      /* clear all stacks
       *   remove argument frame, remove results of two branches
       *   (correct refcount only for first one)
       */
      mkinc(order, (desc *)peek(W, 0), ST_W, 0);
      freeframe(A, order);
      while((doneid)top(H) == FSYNCMERGE) {
        drop(W, 1);
        drop(H, 1);
        }
      drop(W, 1);
      push(H, FSYNC);
      break;
    case CSYNC:
    case CSYNCMERGE:
      /* if CSYNC(MERGE) is carried out the stacks are like:
       *   W the result entry
       *   A a removed frame (should be empty!)
       *   R removed frame (means a seperator)
       */
      mkinc(order, (desc *)peek(W, 0), ST_W, 0);
      while(top(W)) drop(W, 1); drop(W, 1);
      while(top(R)) drop(R, 1); drop(R, 1);
      drop(A, top(A)+1);
      getstack(W) = restoreframe(getstack(W), 1);
      getstack(R) = restoreframe(getstack(R), 1);
      getstack(A) = restoreframe(getstack(A), 0);
      drop(H, 1);
      if (cmd == label) {
        drop(H, 1);
        }
      break;
    case SYNC:
      /* does reference counting operations on top of stack W;
       * then removes top frame from stack A and rebuilds next frame
       */
      drop(H, 1);
      mkinc(order, (desc *)peek(W, 0), ST_W, 0);
      freeframe(A, order);
      getstack(A) = restoreframe(getstack(A), 0);
      break;
    case SYNCMERGE:
      /* does reference counting operations on top of stack W,
       * compares to both top most entries on stack W; 
       *   and leaves a merged entry 
       * then removes top frame from stack A and rebuilds next frame
       */
      mkinc(order, (desc *)peek(W, 0), ST_W, 0);
      freeframe(A, order);
      getstack(A) = restoreframe(getstack(A), 0);
      if ((doneid)top(H) == SYNCMERGE)
        while((doneid)top(H) == SYNCMERGE) {
          getstack(W) = cmpres(getstack(W));
          drop(H, 1);
          }
      else
        drop(H, 1);
      /* if not rtf followed by cont_* label (cr style ASM code)
       * remove next marker from stack H
       * due to the carried out synchronisation within the
       * branches all reference count are set to zero
       */
      if ((cmd == label) && (GETORDERARGS(order, n) == LABcont)) {
        for(i = 1; i <= top(A); i++)
          lnode((desc *)peek(A, i), refcnt) = 0;
        drop(H, 1);
        }
      break;
    case NOOP:
      /* do nothing */
      drop(H, 1);
      break;
    default:
      yyfail("INTERNAL ERROR: unknown control symbol on control stack");
    }
  DBUG_VOID_RETURN;
  }

/* ris: inserts explicit reference counting instructions
 *      that replace the implicitoperations
 */

int ris()
  {
  int i, n, restypecnt;
  char * str;
  stack el;
  FUNCTION * fun;
  ORDER * cmd, * actcmd, * restype;
  FUNDESC * f;

  DBUG_ENTER("ris");
  initstack(W, stacksize);
  initstack(A, stacksize);
  initstack(R, stacksize);
  initstack(T, stacksize);
  initstack(H, stacksize);
  initstack(H1, stacksize);
  initstack(H2, stacksize);
  initheap(heapsize);
  initnorisk();
  for(fun = GETFIRSTFUNCTION(program_start); fun; fun = GETNEXTFUNCTION(fun)) {
    refmode = SREFnone;
    insert_order_list(new_order(hashsetref, SREFimplicit), GETFIRSTORDER(fun));
    /* install argument frame for function
     */
    DBUG_EXECUTE("STACKINI",
      prstack("W", getstack(W), 0);
      prstack("A", getstack(A), 1);
      prstack("R", getstack(R), 0);
      prstack("T", getstack(T), 1);
      prstack("H", getstack(H), 0);
      fprintf(stderr, "---------------------------\n");
      )
    DBUG_PRINT("RIS", ("  installing argument frame for %s: %d arg(s), %d tilde arg(s)",
        GETDESCENTRY(GETDESC(fun), label),
        GETDESCENTRY(GETDESC(fun), nv), GETDESCENTRY(GETDESC(fun), nfv)));
    if (GETDESCENTRY(GETDESC(fun), tag) == DT_CASE) {
      /* PM abstractions expect arguments on stack W! */
      for(i = 0; i < GETDESCENTRY(GETDESC(fun), nv); i++)
        push(W, (stack)ndesc(TYshared, 0, CLtest));
      push(A, 0);
      refinserted = 0;
      }
    else {
        /* all other abstractions (combinators) expects arguments on stack A */
      for(i = 0; i < GETDESCENTRY(GETDESC(fun), nv); i++)
        push(A, (stack)ndesc(TYshared, 0, CLtest));
      push(A, GETDESCENTRY(GETDESC(fun), nv));
      refinserted = -1;
      }
    /* install tilde argument frame for function
     */
    for(i = 0; i < GETDESCENTRY(GETDESC(fun), nfv); i++)
      push(T, (stack)ndesc(TYshared, 0, CLtest));
    push(T, GETDESCENTRY(GETDESC(fun), nfv));
    push(H, FSYNC);
    restypecnt = 0; restype = NULL;
    for(cmd = GETFIRSTORDER(fun); cmd; ) {
      DBUG_EXECUTE("STACK",
        prstack("W", getstack(W), 0);
        prstack("A", getstack(A), 1);
        prstack("R", getstack(R), 0);
        prstack("T", getstack(T), 1);
        prstack("H", getstack(H), 0);
        fprintf(stderr, "---------------------------\n");
        )
      actcmd = cmd;
      cmd = GETNEXTORDER(cmd);
      switch(GETORDERENTRY(actcmd, command)) {
        case pushcw_b:
        case pushcw_i:
        case pushcw_pf: 
          /* unboxed object; needs no reference counting */
          DBUG_PRINT("RIS", ("PUSHCW_B/PUSHCW_I/PUSHCW_PF"));
            push(W, (stack)ndesc(TYshared, 1, CLnoref));
          break;
        case pushw_p:
          /* boxed object; needs reference counting */
          DBUG_PRINT("RIS", ("PUSHW_P"));
            push(W, (stack)ndesc(TYshared, 1, CLref));
          break;
        case pushr:
          /* increment reference count */
          DBUG_PRINT("RIS", ("PUSH_R"));
            push(R, (stack)ndesc(TYshared, 1, CLref));
          break;
        case pushr_p:
          /* boxed object; needs reference counting */
          DBUG_PRINT("RIS", ("PUSHR_P"));
            push(R, (stack)ndesc(TYshared, 1, CLref));
          break;
        case pushar:
          /* increment reference count */
          DBUG_PRINT("RIS", ("PUSH_AR"));
          push(R, peek(A, GETORDERARGS(actcmd, n)+1));
            lnode((desc *)top(R), refcnt) += 1;
          break;
        case pushaw:
          /* increment reference count */
          DBUG_PRINT("RIS", ("PUSH_AW"));
          push(W, peek(A, GETORDERARGS(actcmd, n)+1));
            lnode((desc *)top(W), refcnt) += 1;
          break;
        case pushtr:
          /* increment reference count */
          DBUG_PRINT("RIS", ("PUSH_TR"));
          push(R, peek(T, GETORDERARGS(actcmd, n)+1));
            lnode((desc *)top(R), refcnt) += 1;
          break;
        case pushtw:
          /* increment reference count */
          DBUG_PRINT("RIS", ("PUSH_TW"));
          push(W, peek(T, GETORDERARGS(actcmd, n)+1));
            lnode((desc *)top(W), refcnt) += 1;
          break;
        case movear:
          /* move top-element from A to R, do not increment any ref-counts,
             adjust frame size on A
           */
          DBUG_PRINT("RIS", ("MOVEAR"));
          i = (int)pop(A)-1; push(R, pop(A)); push(A, (stack)i);
          break;
        case moveaw:
          /* move top-element from A to W, do not increment any ref-counts,
             adjust frame size on A
           */
          DBUG_PRINT("RIS", ("MOVEAW"));
          i = (int)pop(A)-1; push(W, pop(A)); push(A, (stack)i);
          break;
        case movetr:
          /* move top-element from T to R, do not increment any ref-counts,
             adjust frame size on T
           */
          DBUG_PRINT("RIS", ("MOVETR"));
          i = (int)pop(T)-1; push(R, pop(T)); push(T, (stack)i);
          break;
        case movetw:
          /* move top-element from T to W, do not increment any ref-counts,
           * adjust frame size on T
           */
          DBUG_PRINT("RIS", ("MOVETW"));
          i = (int)pop(T)-1; push(W, pop(T)); push(T, (stack)i);
          break;
        case tdeca:
        case deca:
          /* increment reference count */
          DBUG_PRINT("RIS", ("(T)DECA"));
          lnode((desc *)peek(A, GETORDERARGS(actcmd, n)+1), refcnt) -= 1;
          break;
        case tdecr:
        case decr:
          /* increment reference count */
          DBUG_PRINT("RIS", ("(T)DECR"));
          lnode((desc *)peek(R, GETORDERARGS(actcmd, n)), refcnt) -= 1;
          break;
        case tdect:
        case dect:
          /* increment reference count */
          DBUG_PRINT("RIS", ("(T)DECT"));
          lnode((desc *)peek(T, GETORDERARGS(actcmd, n)+1), refcnt) -= 1;
          break;
        case tdecw:
        case decw:
          /* increment reference count */
          DBUG_PRINT("RIS", ("(T)DECW"));
          lnode((desc *)peek(W, GETORDERARGS(actcmd, n)), refcnt) -= 1;
          break;
        case tinca:
        case inca:
          /* increment reference count */
          DBUG_PRINT("RIS", ("(T)INCA"));
          lnode((desc *)peek(A, GETORDERARGS(actcmd, n)+1), refcnt)
              += peek(A, GETORDERARGS(actcmd, m)+1);
          break;
        case tincr:
        case incr:
          /* increment reference count */
          DBUG_PRINT("RIS", ("(T)INCR"));
          lnode((desc *)peek(R, GETORDERARGS(actcmd, n)), refcnt)
              += peek(R, GETORDERARGS(actcmd, m)+1);
          break;
        case tinct:
        case inct:
          /* increment reference count */
          DBUG_PRINT("RIS", ("(T)INCT"));
          lnode((desc *)peek(T, GETORDERARGS(actcmd, n)+1), refcnt)
              += peek(T, GETORDERARGS(actcmd, m)+1);
          break;
        case tincw:
        case incw:
          /* increment reference count */
          DBUG_PRINT("RIS", ("(T)INCW"));
          lnode((desc *)peek(W, GETORDERARGS(actcmd, n)), refcnt)
              += peek(W, GETORDERARGS(actcmd, m)+1);
          break;
        case pushaux:
          /* push primitiv function result */
          DBUG_PRINT("RIS", ("PUSHAUX"));
          push(W, (stack)ndesc(TYdetructive, 0,
               (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
          break;
        case freea:
          /* drop arguments from stack A, therefore decrement reference counts
           * by 1
           */
          DBUG_PRINT("RIS", ("FREEA"));
          for(i = 0; i < GETORDERARGS(actcmd, n); i++)
            mkdec(actcmd, (desc *)peek(A, i+1), ST_A, i);
          n = pop(A);
          drop(A, GETORDERARGS(actcmd, n));
          push(A, n-GETORDERARGS(actcmd, n));
          break;
        case freeswt:
          /* drop arguments from stack T
           * because gammabeta does not exchange stack R and T the
           * entries are popped off stack R (instaed of T)
           * reference counts are adjusted accordingly
           */
          DBUG_PRINT("RIS", ("FREESW_T"));
          if (refmode != SREFisexplicit)
            for(i = 0; i < GETORDERARGS(actcmd, n); i++)
              mkdec(actcmd, (desc *)peek(R, i), ST_T, i);
          drop(R, GETORDERARGS(actcmd, n));
          break;
        case freer:
          /* drop arguments from stack R, therefore decrement reference counts
           * by 1
           * freeswt does not exchange stacks R and T (see gammabeta)
           */
          DBUG_PRINT("RIS", ("FREER"));
          if (refmode != SREFisexplicit)
            for(i = 0; i < GETORDERARGS(actcmd, n); i++)
              mkdec(actcmd, (desc *)peek(R, i), ST_R, i);
          drop(R, GETORDERARGS(actcmd, n));
          break;
        case freew:
          /* drop arguments from stack W, therefore decrement reference counts by 1
           */
          DBUG_PRINT("RIS", ("FREEW"));
          if (refmode != SREFisexplicit) {
            /* should not occur */
            yywarn("freew inside (keep)implicit environment");
            for(i = 0; i < GETORDERARGS(actcmd, n); i++)
              mkdec(actcmd, (desc *)peek(W, i), ST_W, i);
            }
          drop(W, GETORDERARGS(actcmd, n));
          break;
        case ext:
          DBUG_PRINT("RIS", ("EXT"));
          cleanstacks(actcmd, rtf);
          break;
        case end:
          /* release argument frame on stack T
           * frame on A is released by ext or return instruction
           */
          DBUG_PRINT("RIS", ("END"));
          freeframe(T, actcmd);
          if ((doneid)top(H) == FSYNC)
            drop(H, 1);
          else
            yywarn("INTERNAL ERROR: control stack messed up");
          if (top(W) != BOTTOM) yywarn("INTERNAL ERROR: stack W messed up");
          if (top(A) != BOTTOM) yywarn("INTERNAL ERROR: stack A messed up");
          if (top(R) != BOTTOM) yywarn("INTERNAL ERROR: stack R messed up");
          if (top(T) != BOTTOM) yywarn("INTERNAL ERROR: stack T messed up");
          break;
        case delta1:
        case delta2:
        case delta3:
        case delta4:
          /* decrement reference count for arguments only if argument
           * is freed, if destructable object emit *kill* instruction
           * otherwise emit *dec*,
           * and create updateable entry  for result with inferred
           * <restype> (found on previous line)
           */
          switch(GETORDERENTRY(actcmd, command)) {
            case delta1: n = 1; break;
            case delta2: n = 2; break;
            case delta3: n = 3; break;
            case delta4: n = 4; break;
            default:
            }
          /* build up the following structure:
           *   DELTA<n>  which creates a result with adequate reference count
           *             arguments are kept on stack and are not ref. counted
           *   optional DEC/KILL instruction for the arguments
           *   FREE_W<n> to remove the Arguments from stack
           *   PUSHAUX   to place the result on top of stack W
           */
          DBUG_PRINT("RIS", ("DELTA%d", n));
          if (refmode != SREFisexplicit)
            for(i = 0; i < n; i++)
              mkdec(cmd, (desc *)peek(W, i), ST_W, i);
          if (refmode == SREFexplicit) {
            insert_order_list(new_order(freew, n), cmd);
            insert_order_list(new_order(pushaux), cmd);
            drop(W, n);
            push(W, (stack)ndesc(TYdetructive, 0,
                 (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
            }
          break;
        case jcond:
        case jtrue:
        case jfalse:
          /* encountered conditional which makes up the complete
           * body of a function
           */
          DBUG_PRINT("RIS", ("JCOND/JTRUE/JFALSE"));
          /* make two (four) copies the actual argument frame,
           * the upper one is the original one, the lower one
           * consists of newly allocated nodes so all modifications
           * on stack objects from the first alternative can be undone
           * this has to be done twice
           */
          lnode((desc *)top(W), refcnt)--;
          getstack(A) = cpframe(getstack(A), 0);
          /* 2nd exit from conditional */
          switch((doneid)(el = top(H))) {
            case SYNC: set(H, 0, (stack)SYNCMERGE); break;
            case SYNCMERGE: push(H, (stack)SYNCMERGE); break;
            case FSYNC: set(H, 0, (stack)FSYNCMERGE); break;
            case FSYNCMERGE: push(H, (stack)FSYNCMERGE); break;
            case CSYNC: set(H, 0, (stack)CSYNCMERGE); break;
            case CSYNCMERGE: push(H, (stack)CSYNCMERGE); break;
            default:
              yywarn("illegal situation for conditional encoutered");
            }
          push(H, (stack)NOOP); /* cond_* label */
          push(H, (stack)SYNC); /* 1st exit from conditional */
          /* no reference count corrections required this is implicitly
           * done by cond or not necessary in case of typed conditional
           */
          drop(W, 1);
          break;
        case jcond2:
        case jtrue2:
        case jfalse2: {
          /* encountered embedded conditional
           */
          DBUG_PRINT("RIS", ("JCOND2/JTRUE2/JFALSE2"));
          /* make up to two copies the actual argument frame
           * (each requires a backup copy),
           * the upper one is the original one, the lower one
           * consists of newly allocated nodes so all modifications
           * on stack objects from the first alternative can be undone
           * the second copy is only made if the keep flag is set or
           * it's a return turn conditional (cr code)!
           */
          lnode((desc *)top(W), refcnt)--;
          getstack(A) = cpframe(getstack(A), 0);
          getstack(A) = cpframe(getstack(A), 0);
          push(H, (stack)NOOP); /* cont_* label or removed by SYNCMERGE */
          push(H, (stack)SYNCMERGE); /* either rtf or cont_* label */
          push(H, (stack)NOOP); /* cond_* label */
          push(H, (stack)SYNC); /* either rtf or jump */
          /* no reference count corrections required this is implicitly
           * done by cond or not necessary in case of typed conditional
           */
          drop(W, 1);
          break;
          }
        case Gamma:
        case gammanear:
        case gammafar:
          /* do not increment reference count for tilde arguments (done
           * by following freer);
           * create non updateable entry  for result with inferred
           * <restype> (found on previous line)
           */
          DBUG_PRINT("RIS", ("GAMMA"));
          if ((f = find_desc(GETFIRSTDESC(program_start), GETORDERARGS(actcmd, label)))) {
            push(W, (stack)ndesc(TYshared, 0,
                (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
            }
          else
            yyerror("illegal function identifier %s.", GETORDERARGS(actcmd, label));
          break;
        case gammabeta:
          /* increment reference count for combinator but not for tilde arguments
           * (this is done by following freeswt) and create non updateable
           * entry  for result with inferred <restype> (found on previous line)
           * gammabeta does NOT exchange the stacks R and T
           */
          DBUG_PRINT("RIS", ("GAMMABETA"));
          if ((f = find_desc(GETFIRSTDESC(program_start), GETORDERARGS(actcmd, label)))) {
            if (refmode != SREFisexplicit)
              for(i = 0; i < GETDESCENTRY(f,nv); i++)
                mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
            drop(W, GETDESCENTRY(f, nv));
            push(W, (stack)ndesc(TYshared, 0,
                (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
            }
          else
            yyerror("illegal function identifier %s.", GETORDERARGS(actcmd, label));
          break;
        case beta:
        case tail:
          /* increment reference count for all arguments (if necessary)
           * and create non updateable entry  for result with inferred
           * <restype> (found on previous line)
           */
          DBUG_PRINT("RIS", ("BETA"));
          if ((f = find_desc(GETFIRSTDESC(program_start), GETORDERARGS(actcmd, label)))) {
            if (refmode != SREFisexplicit)
              for(i = 0; i < GETDESCENTRY(f,nv); i++)
                mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
            drop(W, GETDESCENTRY(f, nv));
            push(W, (stack)ndesc(TYshared, 0,
                (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
            }
          else
            yyerror("illegal function identifier %s.", GETORDERARGS(actcmd, label));
          break;
        case intact:
          /* increment reference count for all arguments (if necessary)
           * and create non updateable entry  for result with inferred
           * <restype> (found on previous line)
           */
          DBUG_PRINT("RIS", ("INTACT"));
          n = get_ia_arity(GETIA(GETORDERARGS(actcmd, primf)));
          if (refmode != SREFisexplicit)
            for(i = 0; i < n; i++)
              mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
          drop(W, n);
          push(W, (stack)ndesc(TYshared, 0, CLref));
          break;
        case mkbclos:
        case mkcclos:
        case mksclos:
          /* increment reference count for all given arguments (if necessary)
           * and create updateable entry for closure
           */
          DBUG_PRINT("RIS", ("MKBCLOS/MKCCLOS/MKSCLOS"));
          if (refmode != SREFisexplicit)
            for(i = 0; i < GETORDERARGS(actcmd, n)+1; i++)
              mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
          if (ISASM_TILDE()) {
            if (refmode != SREFisexplicit)
              for(i = 0; i < GETORDERARGS(actcmd, k); i++)
                mkinc(actcmd, (desc *)peek(T, i+1), ST_T, i);
            }
          drop(W, GETORDERARGS(actcmd, n)+1);
          push(W, (stack)ndesc(TYdetructive, 0, CLref));
          break;
        case mkgaclos:
          /* increment reference count for all given arguments (if necessary)
           * and create updateable entry for closure
           * function is found on top of R
           */
          DBUG_PRINT("RIS", ("MKGACLOS"));
          if (refmode != SREFisexplicit)
            for(i = 0; i < GETORDERARGS(actcmd, n); i++)
              mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
          if (ISASM_TILDE()) {
            if (refmode != SREFisexplicit)
              for(i = 0; i < GETORDERARGS(actcmd, k)+1; i++)
                mkinc(actcmd, (desc *)peek(R, i), ST_R, i);
            }
          drop(W, GETORDERARGS(actcmd, n));
          drop(R, GETORDERARGS(actcmd, k)+1);
          push(W, (stack)ndesc(TYdetructive, 0, CLref));
          break;
        case apply:
        case mkap:
        case mkdclos:
        case mkiclos:
          /* increment reference count for all given arguments (if necessary)
           * and create updateable entry for closure
           */
          DBUG_PRINT("RIS", ("APPLY/MKAP/MKDCLOS/MKICLOS"));
          if (refmode != SREFisexplicit)
            for(i = 0; i < GETORDERARGS(actcmd, n)+1; i++)
              mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
          drop(W, GETORDERARGS(actcmd, n)+1);
          if (GETORDERENTRY(actcmd, command) == apply)
            push(W, (stack)ndesc(TYdetructive, 0,
                (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
          else
            push(W, (stack)ndesc(TYdetructive, 0, CLref));
          break;
        case mklist:
        case mkilist:
        case mkframe:
          /* carry out increments of list/frame elements
           * create list/frame descriptor with ref-count 0
           */
          DBUG_PRINT("RIS", ("MKLIST/MKFRAME"));
          if (refmode != SREFisexplicit)
            for(i = 0; i < GETORDERARGS(actcmd, n); i++)
              mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
          drop(W, GETORDERARGS(actcmd, n));
          push(W, (stack)ndesc(TYshared, 0, CLref));
          break;
        case mkslot:
          /* carry out increments of 2 stack items
           * create slto descriptor with ref-count 0
           */
          DBUG_PRINT("RIS", ("MKSLOT"));
          if (refmode != SREFisexplicit)
            for(i = 0; i < 2; i++)
              mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
          drop(W, 2);
          push(W, (stack)ndesc(TYshared, 0, CLref));
          break;
        case jump:
          /* handled similarily to return instruction
           * if terminal instruction of conditional part
           */
          DBUG_PRINT("RIS", ("JUMP"));
          cleanstacks(actcmd, jump);
          break;
        case rtc:
        case rtc_i:
        case rtc_b:
        case rtc_pf:
        case rtf:
        case rtm:
        case rtp:
          /* always free the active argument frame on A stack
           * leave return value on W stack if return from inner
           * conditional, otherwise delete it
           * always increment ref-counts (W and A stack) if necessary
           */
          switch(GETORDERENTRY(actcmd, command)) {
            case rtf:
              DBUG_PRINT("RIS", ("RTF"));
              if (refmode != SREFisexplicit)
                mkinc(actcmd, (desc *)peek(W, 0), ST_W, 0);
              break;
            case rtm:
              DBUG_PRINT("RIS", ("RTM"));
              if (refmode != SREFisexplicit)
                mkinc(actcmd, (desc *)peek(A, 1), ST_A, 0);
              i = (int)pop(A)-1; push(W, pop(A)); push(A, (stack)i);
              break;
            default:
              DBUG_PRINT("RIS", ("RTC/RTP"));
              push(W, (stack)ndesc(TYshared, 0,
                  (GETORDERENTRY(actcmd, command) == rtp ? CLref : CLnoref)));
            }
          /* do all the clean up operations on the various stacks */
          cleanstacks(actcmd, rtf);
          break;
        case rtt:
          DBUG_PRINT("RIS", ("RTF"));
          if (refmode != SREFisexplicit)
            mkinc(actcmd, (desc *)peek(W, 0), ST_W, 0);
          break;
        case hashargs:
          /* replace argument frame on argument stack A/W */
          DBUG_PRINT("RIS", ("%s", GETORDERARGS(actcmd, hash_str)));
          str = GETORDERARGS(actcmd, hash_str);
          DBUG_PRINT("RIS", ("  replace argument frame on argument stack A/W"));
          if (GETDESCENTRY(GETDESC(fun), tag) == DT_CASE)
            for(i = 0; i < GETORDERARGS(actcmd, n); i++) {
              DBUG_PRINT("RIS", ("  type %d->%d",
                  GETORDERARGS(actcmd, argtp)[i],
                  convtype(GETORDERARGS(actcmd, argtp)[i])));
              lnode((desc *)peek(W, i), mode) =
                  convtype(GETORDERARGS(actcmd, argtp)[i]);
              }
          else
            if (GETORDERARGS(actcmd, n) == top(A))
              for(i = 0; i < GETORDERARGS(actcmd, n); i++) {
                DBUG_PRINT("RIS", ("  type %d->%d",
                    GETORDERARGS(actcmd, argtp)[i],
                    convtype(GETORDERARGS(actcmd, argtp)[i])));
                lnode((desc *)peek(A, i+1), mode) =
                    convtype(GETORDERARGS(actcmd, argtp)[i]);
                }
          else
            yyerror("illegal number of arguments on stack A/W (is %d should be %d)",
              top(A), GETORDERARGS(actcmd, n));
          break;
        case hashtildeargs:
          /* replace argument frame on argument stack T */
          DBUG_PRINT("RIS", ("%s", GETORDERARGS(actcmd, hash_str)));
          str = GETORDERARGS(actcmd, hash_str);
          DBUG_PRINT("RIS", ("  replace argument frame on argument stack T"));
          if (GETORDERARGS(actcmd, n) == top(T))
            for(i = 0; i < GETORDERARGS(actcmd, n); i++) {
              DBUG_PRINT("RIS", ("  type %d->%d",
                  GETORDERARGS(actcmd, argtp)[i],
                  convtype(GETORDERARGS(actcmd, argtp)[i])));
              lnode((desc *)peek(T, i+1), mode) =
                  convtype(GETORDERARGS(actcmd, argtp)[i]);
              }
          else
            yyerror("illegal number of arguments on stack T (is %d should be %d)",
              top(T), GETORDERARGS(actcmd, n));
          break;
        case hashrestype:
          /* store type for later use */
          DBUG_PRINT("RIS", ("%s", GETORDERARGS(actcmd, hash_str)));
          DBUG_PRINT("RIS", ("  got return type %d", GETORDERARGS(actcmd, argtp)[0]));
          restype = actcmd; restypecnt = 1;
          break;
        case hashsetref:
          /* switch mode, synchronisation may be necessary */
          DBUG_PRINT("RIS", ("#setref %d", GETORDERARGS(actcmd, n)));
          switch(refmode) {
            case SREFnone:
              /* This mode may only occur at start of function
               */
              switch(GETORDERARGS(actcmd, n)) {
                case SREFimplicit:
                  insert_order_list(new_order(hashsetref, SREFexplicit), actcmd);
                  delete_order(actcmd);
                  refmode = SREFexplicit;
                  break;
                case SREFexplicit:
                  refmode = SREFisexplicit;
                  break;
                case SREFkeepimpl:
                  refmode = SREFkeepimpl;
                  break;
                default: yywarn("RIS: unknown mode found");
                }
              break;
            case SREFexplicit:
              switch(GETORDERARGS(actcmd, n)) {
                case SREFexplicit:
                  refmode = SREFisexplicit;
                case SREFimplicit:
                  delete_order(actcmd);
                  break;
                case SREFkeepimpl:
                  for(i = 0; peek(W, i) && (peek(W, i) != BOTTOM); i++)
                    mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
                  for(i = 0; i < top(A); i++)
                    mkinc(actcmd, (desc *)peek(A, i+1), ST_A, i);
                  refmode = SREFkeepimpl;
                  break;
                default: yywarn("RIS: unknown mode found");
                }
              break;
            case SREFkeepimpl:
              switch(GETORDERARGS(actcmd, n)) {
                case SREFimplicit:
                  insert_order_list(new_order(hashsetref, SREFexplicit), cmd);
                  delete_order(actcmd);
                  refmode = SREFexplicit;
                  break;
                case SREFexplicit:
                  refmode = SREFisexplicit;
                  break;
                case SREFkeepimpl:
                  break;
                default: yywarn("RIS: unknown mode found");
                }
              break;
            default:
              yyfail("reference count inference system looses state");
            }
          break;
        case label:
          DBUG_PRINT("RIS",("LABEL %s:", GETORDERARGS(actcmd, label)));
          if ((GETORDERARGS(actcmd, n) == LABcond) ||
              (GETORDERARGS(actcmd, n) == LABcont) ||
              (GETORDERARGS(actcmd, n) == LABfail) ||
              (GETORDERARGS(actcmd, n) == LABundecided))
            cleanstacks(actcmd, label);
          if (GETORDERARGS(actcmd, n) == LABfail)
            refinserted = 0;
          break;
        /*
         * PM instructions
         */
        case advance:
          DBUG_PRINT("RIS", ("ADVANCE")); break;
        case bind:
          /* copy top element of W to the addressed position on stack A,
           * reference count of overwritten object has to be decremented
           */
          DBUG_PRINT("RIS", ("BIND"));
          mkdec(actcmd, (desc *)peek(A, GETORDERARGS(actcmd, n)+1),
              ST_A, GETORDERARGS(actcmd, n));
          set(A, GETORDERARGS(actcmd, n)+1, top(W));
          lnode((desc *)top(W), refcnt) += 1;
          break;
        case binds:
          /* write new string descriptor to l-th stack position of A
           */
          DBUG_PRINT("RIS", ("BINDS"));
          mkdec(actcmd, (desc *)peek(A, GETORDERARGS(actcmd, n)+1),
              ST_A, GETORDERARGS(actcmd, n));
          set(A, GETORDERARGS(actcmd, n)+1, (stack)ndesc(TYdetructive, 0, CLref));
          break;
        case bindsubl:
          /* write new list descriptor to l-th stack position of A
           */
          DBUG_PRINT("RIS", ("BINDSUBL"));
          set(A, GETORDERARGS(actcmd, l)+1, (stack)ndesc(TYdetructive, 0, CLref));
          break;
        case Case:
          /* call PM abstraction */
          DBUG_PRINT("RIS", ("CASE"));
          if ((f = find_desc(GETFIRSTDESC(program_start), GETORDERARGS(actcmd, label)))) {
            for(i = 0; i < GETDESCENTRY(f,nv); i++)
              mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
            drop(W, GETDESCENTRY(f, nv));
            push(W, (stack)ndesc(TYshared, 0,
                (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
            }
          else
            yyerror("illegal function identifier %s.", GETORDERARGS(actcmd, label));
          break;
        case dereference:
          /* replace top most entry on stack W by dereferenced value */
          DBUG_PRINT("RIS", ("DEREFERENCE"));
          set(W, 0, (stack)ndesc(TYshared, 0,
              (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
          break;
        case drop: {
          /* drop the top most element of stack W */
          int j;
          DBUG_PRINT("RIS", ("DROP"));
          /* increment reference counts for all bound variables
           */
          for(i = j = 0; i < top(A); i++) {
            if ((desc *)peek(A, i+1) == (desc *)top(W))
              j = 1;
            else
              mkinc(actcmd, (desc *)peek(A, i+1), ST_A, i);
            }
          if (j == 0)
            mkdec(actcmd, (desc *)pop(W), ST_W, 0);
          else {
            /* dropped entry also on stack A
             * refcount > 0
             */
            lnode((desc *)top(W), refcnt)--;
            drop(W, 1);
            }
          break;
          }
        case endlist:
        case endsubl:
          DBUG_PRINT("RIS", ("ENDLIST/ENDSUBL"));
          break;
        case fetch:
          /* replace top of W 
           * top of W was pointer to heap element, therefore
           * refcounting is not necessary
           */
          DBUG_PRINT("RIS", ("FETCH"));
          set(W, 0, (stack)ndesc(TYshared, 1,
              (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
          break;
        case gammacase:
          /* increment reference count for combinator but not for tilde arguments
           * (this is done by following freeswt) and create non updateable
           * entry  for result with inferred <restype> (found on previous line)
           * gammacase does NOT exchange the stacks R and T
           */
          DBUG_PRINT("RIS", ("GAMMACASE"));
          if ((f = find_desc(GETFIRSTDESC(program_start), GETORDERARGS(actcmd, label)))) {
            if (refmode != SREFisexplicit)
              for(i = 0; i < GETDESCENTRY(f,nv); i++)
                mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
            drop(W, GETDESCENTRY(f, nv));
            push(W, (stack)ndesc(TYshared, 0,
                (restype ? convtype(GETORDERARGS(restype, argtp)[0]) : CLtest)));
            }
          else
            yyerror("illegal function identifier %s.", GETORDERARGS(actcmd, label));
          break;
        case initbt:
          /* install a 4 entries large back-tracking frame with will not
           * be refcounted
           */
          DBUG_PRINT("RIS", ("INITBT"));
          set(R, 0, (stack)1);
          set(R, 1, (stack)1);
          set(R, 2, top(W));
          set(R, 3, (stack)1);
          break;
        case matcharb:
        case matcharbs:
        case matchbool:
        case matchin:
        case matchint:
        case matchlist:
        case matchprim:
        case matchstr: {
          /* prepare two copies of the actual argument frame
           * (each requires a backup copy),
           * the upper one is the original one, the lower one
           * consists of newly allocated nodes so all modifications
           * on stack objects from the first alternative can be undone
           */
          stack el;
          DBUG_PRINT("RIS", ("MATCH*"));
          n = top(A);
          getstack(A) = cpframe(getstack(A), 0);
          for(i = 2*n+1; i >= 0; i--) {
            el = peek(A, 2*n+1); push(A, el);
            }
          /* also prepare two copies of the _complete_ workspace frame
           */
          getstack(W) = cpframe(getstack(W), 1);
          getstack(W) = cpframe(getstack(W), 1);
          /* as well as the R stack frame
           */
          getstack(R) = cpframe(getstack(R), 1);
          getstack(R) = cpframe(getstack(R), 1);
          push(H, (stack)NOOP);
          push(H, (stack)CSYNC);
          push(H, (stack)NOOP);
          push(H, (stack)CSYNC);
          break;
          }
        case mkaframe:
          /* allocate a new Frame on stack A and fill it with dummy values */
          DBUG_PRINT("RIS", ("MKAFRAME"));
          n = pop(A);
          for(i = 0; i < GETORDERARGS(actcmd, n); i++)
            push(A, (stack)1);
          push(A, GETORDERARGS(actcmd, n)+n);
          break;
        case mkbtframe:
          /* allocate a new Frame on stack R and fill it with dummy values */
          DBUG_PRINT("RIS", ("MKBTFRAME"));
          for(i = 0; i < GETORDERARGS(actcmd, n); i++)
            push(R, (stack)1);
          break;
        case mkcase:
          DBUG_PRINT("RIS", ("MKCASE"));
          drop(W, 1);
          push(W, (stack)ndesc(TYshared, 0, CLref));
          break;
        case mkwframe: {
          /* allocate a frame on stack W and copy to actual
           * match pointer on top
           */
          stack el;
          DBUG_PRINT("RIS", ("MKWFRAME"));
          for(i = 0; i < GETORDERARGS(actcmd, n); i++)
            push(W, 1);
          el =  peek(W, GETORDERARGS(actcmd, n)); push(W, el);
          break;
          }
        case nestlist:
          DBUG_PRINT("RIS", ("NESTLIST"));
          break;
        case pick: {
          /* copies the n-th element of stack W to top
           */
          stack el;
          DBUG_PRINT("RIS", ("PICK"));
          el =  peek(W, GETORDERARGS(actcmd, n));
          set(W, 0, el);
          break;
          }
        case restorebt:
        case restoreptr:
          DBUG_PRINT("RIS", ("RESTORE[BT/PTR]")); break;
        case rmbtframe:
          /* remove frame from stack R
           */
          DBUG_PRINT("RIS", ("RMBTFRAME"));
          drop(R, GETORDERARGS(actcmd, n));
          break;
        case rmwframe:
          /* remove frame from stack W
           */
          DBUG_PRINT("RIS", ("RMWFRAME"));
          drop(W, GETORDERARGS(actcmd, n));
          break;
        case savebt:
        case startsubl:
          DBUG_PRINT("RIS", ("SAVEBT/STARTSUBL"));
          break;
        case tguard:
          /* prepare a copy of the actual argument frame
           * (which requires a backup copy),
           * the upper one is the original one, the lower one
           * consists of newly allocated nodes so all modifications
           * on stack objects from the first alternative can be undone
           */
          /* first the tested argument is removed (ref-counting done
           * by instruction)
           */
          rnode((desc *)top(W), refcnt)--;
          if (refinserted == 1) {
            refinserted = -1;
            for(i = 0; i < top(A); i++)
              lnode((desc *)peek(A, i+1), refcnt)--;
            for(i = 0; i < top(A); i++)
              mkinc(actcmd, (desc *)peek(A, i+1), ST_A, i);
            for(i = 0; i < top(A); i++)
              lnode((desc *)peek(A, i+1), refcnt)++;
            }
          else
            refinserted = -1;
          drop(W, 1);
        case atend:
        case atstart:
          /* do the same stuff as for tguard, except don't remove
           * guard from stack W
           */
          DBUG_PRINT("RIS", ("TGUARD/ATEND/ATSTART"));
          /* make a full synchronisation of new argument frame
           * only if any reference count instructions are
           * inserted above
           */
          getstack(A) = cpframe(getstack(A), 0);
          /* also prepare a copies of the _complete_ workspace frame
           */
          getstack(W) = cpframe(getstack(W), 1);
          /* as well as the R stack frame
           */
          getstack(R) = cpframe(getstack(R), 1);
          push(H, (stack)NOOP);
          push(H, (stack)CSYNC);
          break;
        case saveptr:
          DBUG_PRINT("RIS", ("SAVEPTR"));
          break;
        case pushh:
        case poph:
        case wait:
	case count:
	case msnodist:
	case msdistend:
          break;
        case dist:
          /* force synchronisation
           */
          for(i = 0; i < GETORDERARGS(actcmd, n); i++)
            mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
          break;
        case distb:
          /* force synchronisation
           */
          for(i = 0, n = GETORDERARGS(actcmd, n); n; i++, n /= 2)
            if (n%1)
              mkinc(actcmd, (desc *)peek(W, i), ST_W, i);
          break;
        case distend:
          /* force synchronisation
           */
          if (refmode != SREFisexplicit)
            mkinc(actcmd, (desc *)peek(W, 0), ST_W, 0);
          break;
        default: yyfail("ris: illegal order received");
        }
        if (restypecnt) restypecnt--; else restype = NULL;
      }
    }
  freestack(W);
  freestack(A);
  freestack(R);
  freestack(T);
  freestack(H);
  freestack(H1);
  freestack(H2);
  freeheap();
  DBUG_RETURN(0);
  }
