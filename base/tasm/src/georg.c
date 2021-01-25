#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "main.h"
#include "mytypes.h"
#include "buildtree.h"
#include "error.h"
#include "loctypes.h"
#include "tiny.h"
#include "stack.h"
#include "dbug.h"

char * vgeorg = VGEORG;

static ORDER * find_label_backwards(ORDER * p, const char * str)
  {
  char * s;

  s = malloc(strlen(str)+2);
  s[0]='j';
  strcpy(&s[1], str);
  while(p && (p->command != label) && strcmp(s, p->args.label))
    p = GETPREVORDER(p);
  free(s);
  return p;
  }

void explode_rtf(ORDER * p)
  {
  switch(p->command) {
    case rtc_i:
      DBUG_PRINT("GEORG", ("inserting PUSHC_W"));
      insert_order_list(new_order(pushcw_i, p->args.n), p);
      break;
    case rtc_b:
      DBUG_PRINT("GEORG", ("inserting PUSHC_W"));
      insert_order_list(new_order(pushcw_b, p->args.n), p);
      break;
    case rtc_pf:
      DBUG_PRINT("GEORG", ("inserting PUSHC_W"));
      insert_order_list(new_order(pushcw_pf, p->args.n), p);
      break;
    case rtf:
      DBUG_PRINT("GEORG", ("inserting nothing"));
      break;
    case rtm:
      DBUG_PRINT("GEORG", ("inserting MOVE_AW"));
      insert_order_list(new_order(moveaw), p);
      break;
    case rtp:
      DBUG_PRINT("GEORG", ("inserting PUSH_W"));
      insert_order_list(new_order(pushw_p, p->args.desc), p);
      break;
    default:
      yyfail("illegal return instruction detected");
    }
  }

/* georg: converts the ASM code for conditionals
 * 1) conditionals may be build by using sequences like:
 *    pushret jcond ... rtf ... rtf
 * 2) or alternatively by elimination the use of the return
 *    instructions: jcond ... jump ...
 * georg converts 1) in 2) and vs.
 */

int georg(int dogeorg)
  {
  int i, j, cmdid;
  stack x;
  char * str;
  FUNCTION * fun;
  ORDER * cmd, * p;
  astack(s);

  DBUG_ENTER("georg");
  initstack(s, STACKSIZE);
  push(s, BOTTOM);
  for(fun = GETFIRSTFUNCTION(program_start); fun; fun = GETNEXTFUNCTION(fun)) {
    /* always uses triples on stack:
     * (cont)-label name | ident | ref. to order
     */
    push(s, 0); push(s, 0); push(s, 0);
    DBUG_PRINT("GEORG", ("starting function %s", (int)GETNAME(fun)));
    /* do reorganizing for all functions */
    for(cmd = GETFIRSTORDER(fun); cmd; ) {
      p = cmd; cmdid=0;
      cmd = GETNEXTORDER(cmd);
      switch(p->command) {
        case ext:
          /* keeps goal analogously to every other function
           */
          drop(s, 3);
          break;
        case pushret: {
          /* PUSHRET is removed and the label is stored on top of stack
           */
          str = (char *)malloc(strlen(p->args.label));
          strcpy(str, p->args.label);
          if (dogeorg == GEORG_GENJUMP) {
            DBUG_PRINT("GEORG", ("ignored PUSHRET(%p:%s)", str, str));
            delete_order(p);
            }
          push(s, (stack)NULL);
          push(s, (stack)pushret);
          push(s, (stack)str);
          break;
          }
        case jcond: if (cmdid == 0) cmdid = jcond2;
        case jfalse: if (cmdid == 0) cmdid = jfalse2;
        case jtrue: if (cmdid == 0) cmdid = jtrue2;
          {
          /* An unary conditional branch is converted into
           * a binary. The second label is obtained from top of stack
           */
          str = (char *)top(s);
          DBUG_PRINT("GEORG", ("COND*(%p:%s)", str, str));
          if ((dogeorg == GEORG_GENJUMP)
              && (peek(s, 1) == (stack)pushret)
              && (strcmp(&str[4], &p->args.label[4]) == 0)) {
            DBUG_PRINT("GEORG", ("replacing by COND2*(%s,%s)", p->args.label, str));
            insert_order_list(new_order(cmdid, LABcond, LABcont, p->args.label, str), p);
            delete_order(p);
            }
          push(s, (stack)NULL);
          if (peek(s, 2)) {
            char * s = str;
            push(s, (stack)jcond);
            str = (char *)malloc(strlen(s)+1);
            strcpy(str, s);
            }
          else {
            push(s, (stack)0);
            str = (char *)malloc(strlen(p->args.label));
            strcpy(str, p->args.label);
            str[3] = 't';
            }
          push(s, (stack)str);
          break;
          }
        case rtc_i:
        case rtc_b:
        case rtc_pf:
        case rtf:
        case rtm:
        case rtp: {
          /* convert the first return instruction (terminating conditionals)
           * into a unconditional jump instruction and remove the
           * second return instruction.
           * optimized return instructions have to be split up into
           * two simpler instructions
           */
          str = (char *)pop(s);
          DBUG_PRINT("GEORG", ("return"));
          switch((COMMAND)pop(s)) {
            case pushret: /* second return in reduma built conditional */
              DBUG_PRINT("GEORG", ("found PUSHRET"));
              if (dogeorg == GEORG_GENJUMP) {
                explode_rtf(p);
                delete_order(p);
                }
              free(str);
              drop(s, 1);
              break;
            case jcond: /* first return in reduma built embedded conditional */
              DBUG_PRINT("GEORG", ("found JCOND"));
              if (dogeorg == GEORG_GENJUMP) {
                ORDER * ppp;
                explode_rtf(p);
                DBUG_PRINT("GEORG", ("inserting JUMP(%s)", str));
                ppp=new_order(jump, LABcont, str);
                insert_order_list(ppp, p);
                delete_order(p);
                free(str);
                }
              drop(s, 1);
              break;
            case rtf: /* first return in reduma build function body conditional */
              DBUG_PRINT("GEORG", ("found RTF"));
              drop(s, 1); free(str);
              break;
            case jcond2:
              DBUG_PRINT("GEORG", ("found JCOND2"));
              drop(s,1); free(str);
              break;
            case Case: /* return from when-clause */
              /* DO NOT remove markers from stack, this is done by
               * the adjacent label. Reason: multiple labels may
               * prefix the same code segment
               */
              DBUG_PRINT("GEORG", ("found CASE"));
              push(s, Case); push(s, (stack)str);
              break;
            case 0: /* last return of function encountered */
              DBUG_PRINT("GEORG", ("found 0"));
              drop(s, 1); free(str);
              break;
            default:
              yyfail("INTERNAL ERROR: georg: illegal command id on stack");
            }
          /* now, remove all code lines below the return instruction
           * up to the next END or label
           */
          while((GETORDERENTRY(cmd, command) != end)
              && ( GETORDERENTRY(cmd, command) != label)) {
            DBUG_PRINT("GEORG", ("removing dead code"));
            p = cmd; cmd = GETNEXTORDER(cmd);
            delete_order(p);
            }
          break;
          }
        case rtt: {
          drop(s, 3);
          break;
          }
        case jcond2:
        case jfalse2:
        case jtrue2: {
          /* convert binary conditional branch instruction into unary,
           * placing the continuation label on top of stack and
           * inserting a PUSHRET instruction just before the
           * conditional label
           */
          str = (char *)malloc(strlen(p->args.ret));
          strcpy(str, p->args.ret);
          DBUG_PRINT("GEORG", ("COND2*(%p:%s)", str, str));
          push(s, (stack)NULL); push(s, (stack)0); push(s, (stack)str);
          str = (char *)malloc(strlen(p->args.ret));
          strcpy(str, p->args.ret);
          push(s, (stack)p); push(s, (stack)jcond2); push(s, (stack)str);
          break;
          }
        case jump: {
          /* if the branch destination is on top of stack
           * convert to unconditional branch into a return
           * instruction (the destination stays on top of stack)
           */
          ORDER * q, * qq;
          str = (char *)top(s);
          DBUG_PRINT("GEORG", ("JUMP(%p:%s)", str, str));
          if (str && (strcmp(p->args.label,str) == 0))
            if (dogeorg == GEORG_GENRTF) {
              if ((COMMAND)peek(s, 1) == jcond2) {
                DBUG_PRINT("GEORG", ("replacing JCOND2*"));
                drop(s, 2); q = (ORDER *)pop(s);
                switch(GETORDERENTRY(q, command)) {
                  case jcond2: GETORDERENTRY(q, command) = jcond; break;
                  case jtrue2: GETORDERENTRY(q, command) = jtrue; break;
                  case jfalse2: GETORDERENTRY(q, command) = jfalse; break;
                  default: yyfail("INTERNAL ERROR: georg: illegal command id on stack");
                  } 
                str = q->args.ret;
                if ((qq = find_label_backwards(q, q->args.label))) {
                  DBUG_PRINT("GEORG", ("inserting PUSHRET before label %s", q->args.label));
                  insert_order_list(new_order(pushret, LABcont, str), qq);
                  }
                else if ((qq = find_label_backwards(p, q->args.label))) {
                  DBUG_PRINT("GEORG", ("inserting PUSHRET before JCOND"));
                  insert_order_list(new_order(pushret, LABcont, str), q);
                  }
                else
                  yyerror("missing label j%s", q->args.label);
                }
              else
                drop(s, 3);
              DBUG_PRINT("GEORG", ("inserting RTF"));
              insert_order_list(new_order(rtf), p);
              delete_order(p);
              }
            else
              drop(s, 3);
          break;
          }
        case label: {
          /* if the top of stack label if equal to the encountered
           * label a conditional is complete and the alternative has to
           * be terminated by a return
           * The label is removed from stack
           * labels identifying PM are always removed from stack
           */
          DBUG_PRINT("GEORG", ("%s: LABEL(%p:%s)", p->args.label, str, str));
          str = (char *)top(s);
          if ((stack)str == BOTTOM) {
            DBUG_PRINT("GEORG", ("unexpected label, must be dead code"));
            while(GETORDERENTRY(p, command) != end) {
              DBUG_PRINT("GEORG", ("removing dead code"));
              delete_order(p);
              p = cmd; cmd = GETNEXTORDER(cmd);
              }
            }
          else {
            if (str && ((strcmp(GETORDERARGS(p, label), str) == 0) ||
                        (GETORDERARGS(p, n) == LABfail) ||
                        (GETORDERARGS(p, n) == LABbt) ||
                        (GETORDERARGS(p, n) == LABinloop) ||
                        (GETORDERARGS(p, n) == LABundecided))) {
              if (dogeorg == GEORG_GENRTF) {
                DBUG_PRINT("GEORG", ("inserting RTF"));
                insert_order_list(new_order(rtf), p);
                }
              free((void *)pop(s));
              drop(s, 2);
              }
            }
          break;
          }
        case matcharb:
        case matcharbs:
        case matchbool:
        case matchin:
        case matchint:
        case matchlist:
        case matchprim:
        case matchstr: { /* prepare new exit for undecided branch */
          DBUG_PRINT("GEORG", ("MATCH* encountered"));
          for(i = j = 0; peek(s, i) != BOTTOM; i += 3)
            if (peek(s, i) && (strcmp((char *)peek(s, i), GETORDERARGS(p, label)) == 0))
              { j = 1; break; }
          if (j == 0) {
            str = (char *)malloc(strlen(p->args.label));
            strcpy(str, p->args.label);
            push(s, (stack)NULL); push(s, Case); push(s, (stack)str);
            DBUG_PRINT("GEORG", ("%s pushed", (char *)top(s)));
            }
          for(i = j = 0; peek(s, i) != BOTTOM; i += 3)
            if (peek(s, i) && (strcmp((char *)peek(s, i), GETORDERARGS(p, ret)) == 0))
              { j = 1; break; }
          if (j == 0) {
            str = (char *)malloc(strlen(p->args.ret));
            strcpy(str, p->args.ret);
            push(s, (stack)NULL); push(s, Case); push(s, (stack)str);
            DBUG_PRINT("GEORG", ("%s pushed", (char *)top(s)));
            }
          break;
          }
        case atend:
        case atstart:
        case tguard: { /* prepare an exit for the failt branch */
          DBUG_PRINT("GEORG", ("ATEND/ATSTART/TGUARD/MATCH* encountered"));
          for(i = j = 0; peek(s, i) != BOTTOM; i += 3)
            if (peek(s, i) && (strcmp((char *)peek(s, i), GETORDERARGS(p, label)) == 0))
              { j = 1; break; }
          if (j == 0) {
            str = (char *)malloc(strlen(p->args.label));
            strcpy(str, p->args.label);
            push(s, (stack)NULL); push(s, Case); push(s, (stack)str);
            DBUG_PRINT("GEORG", ("%s pushed", (char *)top(s)));
            }
          break;
          }
        default:
          /* in all other cases keep the proram unchanged */
        }
      }
    if (top(s) != BOTTOM) {
      while((x = pop(s)) != BOTTOM) {
        fprintf(stderr, "%x:%s:%d\n", x, (x ? (char *)x : "(null)"), top(s));
        drop(s, 2);
        }
      yyfail("internal error: stack not empty");
      }
    }
  DBUG_RETURN(0);
  }
