/***********************************************************************/
/* Optimizes syntaxtree built by tasm2c.y. Call from compile.c.        */
/***********************************************************************/

#include <stdarg.h>
#include <varargs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dbug.h"
#include "mytypes.h"
#include "buildtree.h"
#include "comp_support.h"
#include "error.h"
#include "c_output.h"

extern char *modulfile;
extern char *opt_primf_tab[];
extern char *get_str_space(char *);
extern int optimize;
int inline_label = 0;
extern char *strrepl(char *, char *, int);
extern int find_substr(char *, char *);
#define OPT_BOOL 1

extern PROGRAM *program_start;
extern char *primf_tab[];


ORDER *getnextcond(ORDER *o)
{
  ORDER *erg = NULL;
  int fertig = 0;
  
  DBUG_ENTER("getnextcond");
  while ((NULL != o) && (0 == fertig))
  {
    switch (GETORDERENTRY(o, command))
    {
    case label: break;
    case jcond:
    case jfalse:
    case jtrue:
    case jcond2:
    case jfalse2: 
    case jtrue2: 
    { 
      if (tp_bool == GETORDERENTRY(o, types))
      {
        erg = o; 
        fertig = 1; 
      }
      else 
      {
        erg = NULL; 
        fertig = 1;
      }
      break;
    }        
    case delta1:
    case delta2:
    case delta3:
    case delta4:
    case beta: 
    case gammabeta: { erg = NULL; fertig = 1; break;}
    default:
      ;
    }
    o = GETNEXTORDER(o);
  }
  DBUG_RETURN(erg);
}
/***********************************************************/ 
/* searches for the last real order (no hash, no C-code)   */              
/***********************************************************/ 
ORDER *find_last_order(ORDER *o)
{
  ORDER *h;
  h = GETPREVORDER(o);
  while (NULL != h)
    switch(GETORDERENTRY(h, command))
    {
      case hashargs:
      case hashtildeargs:
      case hashrestype:
      case hashsetref:
      case freea:
      case code_ok:     h = GETPREVORDER(h); break;
      default: return(h);
    }
  return((ORDER *)NULL);
}

ORDER *find_free(ORDER *o, ORDER *bound)
{
  ORDER *h;
  h = GETPREVORDER(o);
  while ((NULL != h) && (h != bound))
    switch(GETORDERENTRY(h, command))
    {
      case hashargs:
      case hashtildeargs:
      case hashrestype:
      case hashsetref: h = GETPREVORDER(h); break;
      case freet:
      case freea: return(h);
      default: return((ORDER *)NULL);
    }
  return((ORDER *)NULL);
}

ORDER *search_label(char *s, ORDER *o)
{
  DBUG_ENTER("search_label");
  while (NULL != o)
  {
    if (label == GETORDERENTRY(o, command))
      if (!strcmp(s, GETORDERARGS(o,label))) DBUG_RETURN(o);
    o = GETNEXTORDER(o);
  }
  DBUG_RETURN((ORDER *)NULL);
}
/***********************************************************/ 
/* delivers a string representing the argument type        */              
/***********************************************************/ 
char *get_arg_types(ORDER *o, int field)
{
  static char s[20];
  DBUG_ENTER("get_arg_types");
  if (GETTYPE(field, GETORDERENTRY(o, types)) & EXT_MASK)
  switch(GETTYPE(field, GETORDERENTRY(o, types)) & EXT_MASK)
  {
    case tp_string:
      strcpy(s, "s");
      break;
    case tp_clos:
      strcpy(s, "c");
      break;
    case tp_func:
      strcpy(s, "f");
      break;
    case tp_list:
      strcpy(s, "l");
      break;
    case tp_vect:
      strcpy(s, "v");
      switch(GETTYPE(field, GETORDERENTRY(o, types)) & PRIM_MASK)
      {
        case tp_int:
          strcat(s, "i"); 
          break;
        case tp_bool:
          strcat(s, "b"); 
          break;
        case tp_real:
          strcat(s, "r"); 
          break;
        case tp_empty:
          strcat(s, "e"); 
          break;
        default:
          yyfail("unknown primitiv result type in get_arg_types");
          break;
      }
      break;
    case tp_tvect:
      strcpy(s, "t");
      switch(GETTYPE(field, GETORDERENTRY(o, types)) & PRIM_MASK)
      {
        case tp_int:
          strcat(s, "i"); 
          break;
        case tp_bool:
          strcat(s, "b"); 
          break;
        case tp_real:
          strcat(s, "r"); 
          break;
        case tp_empty:
          strcat(s, "e"); 
          break;
        default:
          yyfail("unknown primitiv result type in get_arg_types");
          break;
      }
      break;
    case tp_matrix:
      strcpy(s, "m");
      switch(GETTYPE(field, GETORDERENTRY(o, types)) & PRIM_MASK)
      {
        case tp_int:
          strcat(s, "i"); 
          break;
        case tp_bool:
          strcat(s, "b"); 
          break;
        case tp_real:
          strcat(s, "r"); 
          break;
        case tp_empty:
          strcat(s, "e"); 
          break;
        default:
          yyfail("unknown primitiv result type in get_arg_types");
          break;
      }
      break;
    default:
      yyfail("unknown extended result type in get_arg_types");
      break;      
  }
  else
  switch(GETTYPE(field, GETORDERENTRY(o, types)))
  {
    case tp_int:
      strcpy(s, "i");
      break;
    case tp_real:
      strcpy(s, "r");
      break;
    case tp_digit:
      strcpy(s, "d");
      break;
    case tp_bool:
      strcpy(s, "b");
      break;
    case tp_char:
      strcpy(s, "c");
      break;
    case tp_var:
      strcpy(s, "var");
      break;
    default:
      yyfail("unknown primitive result type in get_arg_types");
      break;      
  }
  DBUG_RETURN(s);
}

int isnt_atom(ORDER *h)
{
  int result = 0;
  DBUG_ENTER("isnt_atom");
  if (GETORDERARGS(h, argtp[0]) & EXT_MASK)
    result = 1;
  else 
    switch(GETORDERARGS(h, argtp[0]) & PRIM_MASK)
    {
      case tp_int:
      case tp_bool: break;
      default: result = 1;
    }
  DBUG_RETURN(result);
}
int is_it_a_structure_result(PRIMF p)
{
  int erg = 0;
  DBUG_ENTER("is_it_a_structure_result");
  switch(p)
  {
    case p_sprintf:
    case p_transpose:
    case p_ln:
    case p_exp:
    case p_reverse:
    case p_cut:
    case p_vcut:
    case p_mcut:
    case p_lcut:
    case p_transform:
    case p_rotate:
    case p_lrotate:
    case p_vrotate:
    case p_mrotate:
    case p_select:
    case p_vselect:
    case p_lselect:
    case p_mselect:
    case p_substr:
    case p_replace:
    case p_lreplace:
    case p_vreplace:
    case p_mreplace:
    case p_repstr:
    case p_mre_c:
    case p_mre_r: 
    case p_unite: 
    case p_vunite: 
    case p_munite: 
    case p_lunite:
    case p_to_vect: 
    case p_to_tvect: 
    case p_to_mat: 
    case p_to_scal: 
    case p_to_list: 
    case p_to_field: erg =1; break;
    
    default:;
  }
  DBUG_RETURN(erg);
}
/***********************************************************/ 
/* delivers a string representing the result type          */              
/***********************************************************/ 
char *get_res_fmt(ORDER *h)
{
  static char erg[80];
  DBUG_ENTER("get_res_fmt");
  if (GETORDERARGS(h, argtp[0]) & EXT_MASK)
  switch(GETORDERARGS(h, argtp[0]) & EXT_MASK)
  {
    case tp_list:
      strcpy(erg, "MkList(%s, %s)");
      break;
    case tp_vect:
      strcpy(erg, "MkVect(%s, %s)");
      break;
    case tp_tvect:
      strcpy(erg, "MkTVect(%s, %s)");
      break;
    case tp_matrix:
      strcpy(erg, "MkMatrix(%s, %s)");
      break;
    case tp_string:
      strcpy(erg, "MkString(%s, %s)");
      break;
    default:
      yyfail("unknown extended result type in get_res_fmt");
  }
  else if (GETORDERARGS(h, argtp[0]) & PRIM_MASK)
  switch(GETORDERARGS(h, argtp[0]))
  {
    case tp_int:
      strcpy(erg, "MkInt(%s, %s)");
      break;
    case tp_real:
      strcpy(erg, "MkReal(%s, %s)");
      break;
    case tp_digit:
      strcpy(erg, "MkDigit(%s, %s)");
      break;
    case tp_bool:
      strcpy(erg, "MkBool(%s, %s)");
      break;
    case tp_char:
      strcpy(erg, "MkChar(%s, %s)");
      break;
    case tp_var:
      strcpy(erg, "MkVar(%s, %s)");
      break;
    default:
      yyfail("unknown primitive result type in get_res_fmt");
  }        
DBUG_RETURN(erg);
}

/************************************************************/
/* optimizes typed delta calls                              */
/************************************************************/
void optimize_delta(FUNCTION *f)
{
  ORDER *o, *h, *h1, *h2, *next, *new, *bound;
        /* bound is the first order which is not examined while searching for a freea */
  char  *res_fmt, very_temp[1024], tmpstr[1024], 
       v[1024], u[1024], t[1024], s[1024], resstr[1024], *fmt;
       char *MkElem = "MkElem(%s, %s)";
  int n, i, kill_a, kill_w, dont_opt,tv_to_kill, needless_w, opti, delta;
      /* i =      #moveaw to hold where to peek on a, 
         kill_a = #entries to removed from a after action,
         kill_w = #entries to removed from w after action,
         dont_opt = 1 if userdefined function call or mklist found 
         tv_to_kill = #of temporary results to kill 
         needless_w = # of no longer needed w entries  
         opti = 0 if only atom types used */
  
  DBUG_ENTER("optimize_delta");
  o = GETFIRSTORDER(f);
  while (NULL != o)
  {
    next = NULL;
    h = o;
    i = 0;
    kill_a = 0;
    kill_w = 0;
    dont_opt = 0;
    tv_to_kill = 0;
    opti = 0;
    delta = 0;
    switch(GETORDERENTRY(o, command))
    {
    case jtrue:
    case jtrue2:
    case jfalse:
    case jfalse2:
    case jcond:
    case jcond2:
      if (tp_bool == GETTYPE(0, GETORDERENTRY(o, types)))
      {
        h = find_last_order(o);
        tv_to_kill = GETORDERARGS(h, k);   
        if (label == GETORDERENTRY(h, command))
          h = find_last_order(h);
          if (0 == doris)
          {
            switch(GETORDERENTRY(h, command))
            {
              case stack_op:
                sprintf(s, "(%s)", GETORDERENTRY(h, code));
                kill_a += GETORDERARGS(h, n);
                kill_w += GETORDERARGS(h, m);
                tv_to_kill += GETORDERARGS(h, k);
                opti += GETORDERENTRY(h, opti);
                delete_order(h);
                break;
              case pushaw:
                sprintf(s, "peek(a, %d)", GETORDERARGS(h, n));
                delete_order(h);
                break;
              case pushtw:
                sprintf(s, "peek(t, %d)", GETORDERARGS(h, n));
                delete_order(h);
                break;
              case moveaw:
                sprintf(s, "peek(a, %d)", i + kill_a);
                i++;
                delete_order(h);
                break;
              case movetw:
                sprintf(s, "top(t)");
                delete_order(h);
                break;
              case pushcw_b:
                sprintf(s, "%d", GETORDERARGS(h, n));
                delete_order(h);
                break;
              default:
                sprintf(s, "top(w)");
                kill_w++;
                break;
            }
          }
          else         /* ris used */
            sprintf(s, "pop(w)");
        strcpy(t, GETORDERARGS(o, label)); /* remove the tailing ":" */
        if (-1 != find_substr(t, ":"))
          strrepl(&t[find_substr(t, ":")], " ", 1);
        if ((jtrue == GETORDERENTRY(o, command)) || (jtrue2 == GETORDERENTRY(o, command)))
          sprintf(very_temp, "if (SA_TRUE == (%s)) goto %s;", s, t);
        else
          sprintf(very_temp, "if (SA_FALSE == (%s)) goto %s;", s, t);
        if (0 != kill_w)
        {
          sprintf(t, "\nfreew(%d);\n", kill_w);
          strcat(very_temp, t);
        }
        if (0 != kill_a)
        {
          sprintf(t, "\nfreea(%d);\n", kill_a);
          strcat(very_temp, t);
        }
        new = new_order(code_ok, very_temp);
        SETORDERARGS(new, k) = tv_to_kill;
        SETORDERENTRY(new, opti) = opti;
        insert_order_list(new, o);
        SETORDERARGS(search_label(GETORDERARGS(o, label), o), l) = kill_a;
        SETORDERARGS(search_label(GETORDERARGS(o, label), o), m) = kill_w;
        SETORDERARGS(search_label(GETORDERARGS(o, label), o), k) = tv_to_kill;
        SETORDERARGS(search_label(GETORDERARGS(o, label), o), k) = opti;
        delete_order(o);
        h = new;
      }
      break;
    case delta1:
      if (tp_none != GETTYPE(0, GETORDERENTRY(o, types)) &&
          (tp_none != GETORDERARGS(GETPREVORDER(o), argtp[0])))   /* i.e. type inferred */
      {
        h = find_last_order(o);
        bound = GETPREVORDER(h);
        needless_w = 0;
        if (0 == doris)
        {
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(s, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;   
              opti += GETORDERENTRY(h, opti);   
              delete_order(h);
              break;
            case pushaw:
              sprintf(s, "peek(a, %d)", GETORDERARGS(h, n));
              delete_order(h);
              break;
            case pushtw:
              sprintf(s, "peek(t, %d)", GETORDERARGS(h, n));
              delete_order(h);
              break;
            case moveaw:
              sprintf(s, "peek(a, %d)", i + kill_a);
              i++;
              delete_order(h);
              break;
            case movetw:
              sprintf(s, "top(t)");
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(s, "%d", GETORDERARGS(h, n));
              delete_order(h);  
              break;
            case pushw_p:
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(s, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(s, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              delete_order(h);
              break;
           default:
              sprintf(s, "top(w)");
              kill_w++;
              break;
          }
      }
      else                     /* ris used */
        sprintf(s, "top(w)");
      fmt = opt_primf_tab[GETPRF(GETORDERARGS(o, primf))];
      sprintf(resstr, fmt, s);
      strcpy(very_temp, get_arg_types(o, 0)); 
      strcat(very_temp, "_");
      strcat(very_temp, resstr);
      if (0 == is_it_a_structure_result(GETPRF(GETORDERARGS(o, primf))))
        res_fmt = get_res_fmt(GETPREVORDER(o));
      else
        res_fmt = MkElem;
      tv_to_kill++;
      sprintf(tmpstr, res_fmt, very_temp, "#####");
      if (0 == doris)
        new = new_order(stack_op, kill_a + i, kill_w, tv_to_kill, tmpstr); 
      else
        new = new_order(ris_stack_op, kill_a + i, kill_w, tv_to_kill, tmpstr);
      SETORDERENTRY(new, opti) = opti + isnt_atom(GETPREVORDER(o));
      insert_order_list(new, o);
      delete_order(o);  
      h = find_free(new, bound);
      if (NULL != h)
      {
        h2 = new_order(freea, GETORDERARGS(h, n));
        insert_order_list(h2, GETNEXTORDER(new));
        delete_order(h);
      }
      h = new; 
      
      }
      break;
    case delta2:
      if ((tp_none != GETTYPE(0, GETORDERENTRY(o, types))) &&
           (tp_none != GETTYPE(1, GETORDERENTRY(o, types))) &&
           (tp_none != GETORDERARGS(GETPREVORDER(o), argtp[0])))  /* i.e. types inferred */
      {
        h = find_last_order(o);
        needless_w = 0;
        if (0 == doris)
        {
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(s, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;   
              opti += GETORDERENTRY(h, opti);
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushaw:
              sprintf(s, "peek(a, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushtw:
              sprintf(s, "peek(t, %d)", GETORDERARGS(h, n));
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case moveaw:
              sprintf(s, "peek(a, %d)", i + kill_a);
              needless_w++;
              i++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case movetw:
              sprintf(s, "top(t)");
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(s, "%d", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushw_p:
              needless_w++;
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(s, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(s, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case beta:
            case Gamma:
            case tail:
            case gammabeta:
            case Case:
            case delta1:
            case delta2:
            case delta3:
            case delta4:
            case mklist:
            case mkilist:
              dont_opt = 1;
           default:
              sprintf(s, "top(w)");
              kill_w++;
              h1 = find_last_order(h);
              break;
          } /* switch */
          h = h1;
          bound = GETPREVORDER(h);
          if (1 == dont_opt)
          {
            sprintf(t, "peek(w, 1)");
            kill_w++;
          }
          else
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(t, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;
              opti += GETORDERENTRY(h, opti);
              delete_order(h);
              break;
            case pushaw:
              sprintf(t, "peek(a, %d)", GETORDERARGS(h, n));
              delete_order(h);
              break;
            case pushtw: 
              sprintf(t, "peek(t, %d)", GETORDERARGS(h, n));
              delete_order(h);
              break;
            case moveaw: 
              sprintf(t, "peek(a, %d)", i + kill_a);
              i++;
              delete_order(h);
              break;
            case movetw: 
              sprintf(t, "top(t)");
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(t, "%d", GETORDERARGS(h, n));
              delete_order(h);
              break;
            case pushw_p:
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(t, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(t, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              delete_order(h);
              break;
            default:
              sprintf(t, "peek(w, %d)", kill_w);  /* war 1 - needless_w */
              kill_w++;
              break;
          } /* switch */

        } /* if (0 == doris) */
        else                         /* ris used */
        {
          sprintf(s, "top(w)");
          sprintf(t, "peek(w, 1)");
          kill_w += 2;
        }
        fmt = opt_primf_tab[GETPRF(GETORDERARGS(o, primf))];
        sprintf(resstr, fmt, s, t);
        strcpy(very_temp, get_arg_types(o, 0));
        strcat(very_temp, "x");
        strcat(very_temp, get_arg_types(o, 1));
        strcat(very_temp, "_");
        strcat(very_temp, resstr);
        if (0 == is_it_a_structure_result(GETPRF(GETORDERARGS(o, primf))))
          res_fmt = get_res_fmt(GETPREVORDER(o));
        else
          res_fmt = MkElem;
        tv_to_kill++;
        sprintf(tmpstr, res_fmt, very_temp, "#####");
        if (0 == doris)
          new = new_order(stack_op, kill_a + i, kill_w, tv_to_kill, tmpstr);
        else
          new = new_order(ris_stack_op, kill_a + i, kill_w, tv_to_kill, tmpstr);
        SETORDERENTRY(new, opti) = opti + isnt_atom(GETPREVORDER(o));
        insert_order_list(new, o);
        delete_order(o);
        h = find_free(new, bound);
        if (NULL != h)
        {
          h2 = new_order(freea, GETORDERARGS(h, n));
          insert_order_list(h2, GETNEXTORDER(new));
          delete_order(h);
        }
        h = new;
      } /* if types inferred */
      break;
    case delta3:
      if ((tp_none != GETTYPE(0, GETORDERENTRY(o, types))) &&
           (tp_none != GETTYPE(1, GETORDERENTRY(o, types))) &&
           (tp_none != GETTYPE(2, GETORDERENTRY(o, types))) &&
           (tp_none != GETORDERARGS(GETPREVORDER(o), argtp[0])))  /* i.e. types inferred */
      {
        h = find_last_order(o);
        needless_w = 0;
        if (0 == doris)
        {
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(s, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;
              opti += GETORDERENTRY(h, opti);
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushaw:
              sprintf(s, "peek(a, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushtw:
              sprintf(s, "peek(t, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case moveaw:
              sprintf(s, "peek(a, %d)", i + kill_a);
              needless_w++;
              i++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case movetw:
              sprintf(s, "top(t)");
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(s, "%d", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushw_p:
              needless_w++;
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(s, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(s, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case beta:
            case Gamma:
            case tail:
            case gammabeta:
            case Case:
            case delta1:
            case delta2:
            case delta3:
            case delta4:
            case mklist:
            case mkilist:
              dont_opt = 1;
            default:
              sprintf(s, "top(w)");
              kill_w++;
              h1 = find_last_order(h);
              break;
          } /* switch */
          h = h1;
          if (1 == dont_opt)
          {
            kill_w++;
            sprintf(t, "peek(w, 1)");
          }
          else            
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(t, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;
              opti += GETORDERENTRY(h, opti);
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushaw:
              sprintf(t, "peek(a, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushtw: 
              sprintf(t, "peek(t, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case moveaw: 
              sprintf(t, "peek(a, %d)", i + kill_a);
              needless_w++;
              i++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case movetw: 
              sprintf(t, "top(t)");
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(t, "%d", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushw_p:
              needless_w++;
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(t, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(t, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case beta:
            case Gamma:
            case tail:
            case gammabeta:
            case Case:
            case delta1:
            case delta2:
            case delta3:
            case delta4:
            case mklist:
            case mkilist:
              dont_opt = 1;
            default:
              sprintf(t, "peek(w, %d)", kill_w); 
              kill_w++;
              h1 = find_last_order(h);
              break;
          } /* switch */
          h = h1;
          bound = GETPREVORDER(h);
          if (1 == dont_opt)
            sprintf(t, "peek(w, 2)");
          else             
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(u, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;
              opti += GETORDERENTRY(h, opti);
              delete_order(h);
              break;
            case pushaw:
              sprintf(u, "peek(a, %d)", GETORDERARGS(h, n));
              needless_w++;
              delete_order(h);
              break;
            case pushtw: 
              sprintf(u, "peek(t, %d)", GETORDERARGS(h, n));
              needless_w++;
              delete_order(h);
              break;
            case moveaw: 
              sprintf(u, "peek(a, %d)", i + kill_a);
              needless_w++;
              i++;
              delete_order(h);
              break;
            case movetw: 
              sprintf(u, "top(t)");
              needless_w++;
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(u, "%d", GETORDERARGS(h, n));
              needless_w++;
              delete_order(h);
              break;
            case pushw_p:
              needless_w++;
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(u, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(u, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              delete_order(h);
              break;
            default:
              sprintf(u, "peek(w, %d)", kill_w);    
              kill_w++;
              break;
          } /* switch */

        } /* if (0 == doris) */
        else                         /* ris used */
        {
          sprintf(s, "top(w)");
          sprintf(t, "peek(w, 1)");
          sprintf(u, "peek(w, 2)");
          kill_w += 3;
        }
        fmt = opt_primf_tab[GETPRF(GETORDERARGS(o, primf))];
        sprintf(resstr, fmt, s, t, u);
        strcpy(very_temp, get_arg_types(o, 0));
        strcat(very_temp, "x");
        strcat(very_temp, get_arg_types(o, 1));
        strcat(very_temp, "x");
        strcat(very_temp, get_arg_types(o, 2));
        strcat(very_temp, "_");
        strcat(very_temp, resstr);
        tv_to_kill++;
        if (0 == is_it_a_structure_result(GETPRF(GETORDERARGS(o, primf))))
          res_fmt = get_res_fmt(GETPREVORDER(o));
        else
          res_fmt = MkElem;
        sprintf(tmpstr, res_fmt, very_temp, "#####");
        if (0 == doris)
          new = new_order(stack_op, kill_a + i, kill_w, tv_to_kill, tmpstr);
        else
          new = new_order(ris_stack_op, kill_a + i, kill_w, tv_to_kill, tmpstr);
        SETORDERENTRY(new, opti) = opti + isnt_atom(GETPREVORDER(o));
        insert_order_list(new, o);
        delete_order(o);
        h = find_free(new, bound);
        if (NULL != h)
        {
          h2 = new_order(freea, GETORDERARGS(h, n));
          insert_order_list(h2, GETNEXTORDER(new));
          delete_order(h);
        }
        h = new;
      } /* if types inferred */
      break;
    case delta4:
      if ((tp_none != GETTYPE(0, GETORDERENTRY(o, types))) &&
           (tp_none != GETTYPE(1, GETORDERENTRY(o, types))) &&
           (tp_none != GETTYPE(2, GETORDERENTRY(o, types))) &&
           (tp_none != GETTYPE(3, GETORDERENTRY(o, types))) &&
           (tp_none != GETORDERARGS(GETPREVORDER(o), argtp[0]))) /* i.e. types inferred */
      {
        h = find_last_order(o);
        needless_w = 0;
        if (0 == doris)
        {
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(s, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;   
              opti += GETORDERENTRY(h, opti);
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushaw:
              sprintf(s, "peek(a, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushtw:
              sprintf(s, "peek(t, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case moveaw:
              sprintf(s, "peek(a, %d)", i + kill_a);
              needless_w++;
              i++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case movetw:
              sprintf(s, "top(t)");
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(s, "%d", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushw_p:
              needless_w++;
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(s, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(s, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case beta:
            case Gamma:
            case tail:
            case gammabeta:
            case Case:
            case delta1:
            case delta2:
            case delta3:
            case delta4:
            case mklist:
            case mkilist:
              dont_opt = 1;
            default:
              sprintf(s, "top(w)");
              h1 = find_last_order(h);
              kill_w++;
              break;
          } /* switch */
          h = h1;
          if (1 == dont_opt)
          {
            kill_w++;
            sprintf(t, "peek(w, 1)");
          }
          else            
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(t, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;
              opti += GETORDERENTRY(h, opti);
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushaw:
              sprintf(t, "peek(a, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushtw: 
              sprintf(t, "peek(t, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case moveaw: 
              sprintf(t, "peek(a, %d)", i + kill_a);
              needless_w++;
              i++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case movetw: 
              sprintf(t, "top(t)");
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(t, "%d", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushw_p:
              needless_w++;
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(t, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(t, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case beta:
            case Gamma:
            case tail:
            case gammabeta:
            case Case:
            case delta1:
            case delta2:
            case delta3:
            case delta4:
            case mklist:
            case mkilist:
              dont_opt = 1;
            default:
              sprintf(t, "peek(w, %d)", kill_w);    
              h1 = find_last_order(h);
              break;
          } /* switch */
          h = h1;
          if (1 == dont_opt)
          {
            kill_w++;
            sprintf(t, "peek(w, 2)");
          }
          else            
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(u, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;
              opti += GETORDERENTRY(h, opti);
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushaw:
              sprintf(u, "peek(a, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushtw: 
              sprintf(u, "peek(t, %d)", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case moveaw: 
              sprintf(u, "peek(a, %d)", i + kill_a);
              needless_w++;
              i++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case movetw: 
              sprintf(u, "top(t)");
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(u, "%d", GETORDERARGS(h, n));
              needless_w++;
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case pushw_p:
              needless_w++;
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(u, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(u, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              h1 = find_last_order(h);
              delete_order(h);
              break;
            case beta:
            case Gamma:
            case tail:
            case gammabeta:
            case Case:
            case delta1:
            case delta2:
            case delta3:
            case delta4:
            case mklist:
            case mkilist:
              dont_opt = 1;
            default:
              sprintf(u, "peek(w, %d)", kill_w);    
              kill_w++;
              break;
          } /* switch */
          h = h1;
          bound = GETPREVORDER(h);
          if (1 == dont_opt)
          {
            kill_w++;
            sprintf(t, "peek(w, 3)");
          }
          else            
          switch(GETORDERENTRY(h, command))
          {
            case stack_op:
              sprintf(v, "(##### = (int)(%s))", GETORDERENTRY(h, code));
              kill_a += GETORDERARGS(h, n);
              kill_w += GETORDERARGS(h, m);
              tv_to_kill = tv_to_kill + GETORDERARGS(h, k) + 1;
              opti += GETORDERENTRY(h, opti);
              delete_order(h);
              break;
            case pushaw:
              sprintf(v, "peek(a, %d)", GETORDERARGS(h, n));
              needless_w++;
              delete_order(h);
              break;
            case pushtw: 
              sprintf(v, "peek(t, %d)", GETORDERARGS(h, n));
              needless_w++;
              delete_order(h);
              break;
            case moveaw: 
              sprintf(v, "peek(a, %d)", i + kill_a);
              needless_w++;
              i++;
              delete_order(h);
              break;
            case movetw: 
              sprintf(v, "top(t)");
              needless_w++;
              delete_order(h);
              break;
            case pushcw_i:
            case pushcw_b:
              sprintf(v, "%d", GETORDERARGS(h, n));
              needless_w++;
              delete_order(h);
              break;
            case pushw_p:
              needless_w++;
              n = search_funcdesc((int)GETORDERARGS(h, desc)); 
              if (-1 != n)
                sprintf(v, "(T_PTD)%sfuncclostable[%d]", modulfile, n);
              else
              {
                n = searchdata((int)GETORDERARGS(h, desc));
                if (-1 != n)
                  sprintf(v, "(T_PTD)%sdata_table[%d]", modulfile, n);
                else 
                {
                  if (stdout != yyout)
                  {
                    fclose(yyout);
                    remove(outfile);
                    yyfail("0x%08x unrecognized pointer\n%s removed",
                           (int)GETORDERARGS(h, desc), outfile);
                  }
                yyfail("0x%08x unrecognized pointer",
                       (int)GETORDERARGS(h, desc));
                }
              }
              delete_order(h);
              break;
            case beta:
            case Gamma:
            case tail:
            case gammabeta:
            case Case:
            case delta1:
            case delta2:
            case delta3:
            case delta4:
            case mklist:
            case mkilist:
              dont_opt = 1;
            default:
              kill_w++;
              sprintf(v, "peek(w, %d)", kill_w);    
              break;
          } /* switch */
        } /* if (0 == doris) */
        else                         /* ris used */
        {
          sprintf(s, "top(w)");
          sprintf(t, "peek(w, 1)");
          sprintf(u, "peek(w, 2)");
          sprintf(v, "peek(w, 3)");
          kill_w += 4;
        }
        fmt = opt_primf_tab[GETPRF(GETORDERARGS(o, primf))];
        sprintf(resstr, fmt, s, t, u, v);
        strcpy(very_temp, get_arg_types(o, 0));
        strcat(very_temp, "x");
        strcat(very_temp, get_arg_types(o, 1));
        strcat(very_temp, "x");
        strcat(very_temp, get_arg_types(o, 2));
        strcat(very_temp, "x");
        strcat(very_temp, get_arg_types(o, 3));
        strcat(very_temp, "_");
        strcat(very_temp, resstr);
        if (0 == is_it_a_structure_result(GETPRF(GETORDERARGS(o, primf))))
          res_fmt = get_res_fmt(GETPREVORDER(o));
        else
          res_fmt = MkElem;
        tv_to_kill++;
        sprintf(tmpstr, res_fmt, very_temp, "#####");
        if (0 == doris)
          new = new_order(stack_op, kill_a + i, kill_w, tv_to_kill, tmpstr);
        else
          new = new_order(ris_stack_op, kill_a + i, kill_w, tv_to_kill, tmpstr);
        SETORDERENTRY(new, opti) = opti + isnt_atom(GETPREVORDER(o));
        insert_order_list(new, o);
        delete_order(o);
        h = find_free(new, bound);
        if (NULL != h)
        {
          h2 = new_order(freea, GETORDERARGS(h, n));
          insert_order_list(h2, GETNEXTORDER(new));
          delete_order(h);
        }
        h = new;
      } /* if types inferred */
      break;
    default:
    }
      ;
  o = GETNEXTORDER(h);
  }
  
  DBUG_VOID_RETURN;
}

/************************************************************/
/* checks whether a function can be inlined into another    */
/************************************************************/

int am_i_insertable(FUNCTION *f)
{
  ORDER *help = GETFIRSTORDER(f);
  int erg = 1;
   
  DBUG_ENTER("am_i_insertable");
 
  if (NULL == GETFUNCINLINE(f))
    erg = 0;
  else
  {
    while (NULL != help)
    {
      switch(GETORDERENTRY(help, command))
      {
        case gammacase:
        case tail:
        case apply:  erg = 0; break;
        default:;
      }
      help = GETNEXTORDER(help);
    }
  }
  DBUG_RETURN(erg);
}

void reset_inlined_flags(void)
{
  FUNCTION *f = GETFIRSTFUNCTION(program_start);
  DBUG_ENTER("reset_inlined_flags");
  while (NULL != f)
  {
    SETFUNCINLINE(f) = 1;
    f= GETNEXTFUNCTION(f);
  }
  DBUG_VOID_RETURN;
}

void del_order(ORDER *o)
{
  ORDER *p, *n;
  DBUG_ENTER("del_order");
  p = GETPREVORDER(o);
  n = GETNEXTORDER(o);
  if (NULL != p)
    SETNEXTORDER(p) = n;
  if (NULL != n)
    SETPREVORDER(n) = p;
  if (NULL!=o->code) free(o->code);
  free(o);
  DBUG_VOID_RETURN;
}

void inline_function(FUNCTION *inline_me, ORDER *inline_before_me, int ret_lab)
{
  ORDER *dup, *help, *xhelp;
  DBUG_ENTER("inline_function");
  dup = dup_func(GETFIRSTORDER(inline_me), 0);
  SETFUNCINLINE(inline_me) = GETFUNCINLINE(inline_me) - 1;
  help = dup;
  while (NULL != dup)
  {
    xhelp = (ORDER *)NULL;
    switch(GETORDERENTRY(dup, command))
    {
      case code_ok:
        xhelp = GETNEXTORDER(dup);
        del_order(dup);
        break;
      case rtc_i: 
        SETORDERENTRY(dup, command) = i_rtc_i;
        SETORDERARGS(dup, m) = ret_lab;
        break;
      case rtc_b: 
        SETORDERENTRY(dup, command) = i_rtc_b;
        SETORDERARGS(dup, m) = ret_lab;
        break;
      case rtc_pf: 
        SETORDERENTRY(dup, command) = i_rtc_pf;
        SETORDERARGS(dup, m) = ret_lab;
        break;
      case rtm:
        SETORDERENTRY(dup, command) = i_rtm;
        SETORDERARGS(dup, m) = ret_lab;
        break;
      case rtp:
        SETORDERENTRY(dup, command) = i_rtp;
        SETORDERARGS(dup, m) = ret_lab;
        break;
      case rtt:
        SETORDERENTRY(dup, command) = i_rtt;
        SETORDERARGS(dup, m) = ret_lab;
        break;
      case rtf:
        SETORDERENTRY(dup, command) = i_rtf;
        SETORDERARGS(dup, m) = ret_lab;
        break;
      default:;
    }
    if (NULL == xhelp) dup = GETNEXTORDER(dup);
    else dup = xhelp;
  }
  switch(GETORDERENTRY(inline_before_me, command))
  {
    case Case:
      dup = new_order(stflip, 0); break; /* do not switch any stack */
    case beta:
      dup = new_order(stflip, 1); break;  /* switch only A and W */
    case tail:                            /* switch only A and W and switch tailflag */
      yyfail("TAIL not really implemented in inline_function !!!");
      break; 
    case Gamma:
    case gammacase:
      dup = new_order(stflip, 2); break; /* switch only R and T */
    case gammabeta: 
      dup = new_order(stflip, 3); break; /* switch A and W and R and T */
    default:
      yyfail("inline error");
  }
       
  SETNEXTORDER(dup) =  help;
  SETPREVORDER(help) = dup;
  insert_order_list(dup, inline_before_me);
  
  DBUG_VOID_RETURN;
}

void insert_functions(FUNCTION *f)
{
  char *act_func=GETNAME(f), s[120]="";
  ORDER *help, *nhelp;

  help=GETFIRSTORDER(f);
  while (code_ok == GETORDERENTRY(help, command)) help=GETNEXTORDER(help);
  while (NULL!=help)
  {
    switch (GETORDERENTRY(help, command))
    {
      case beta:
        if ((optimize > 0) && (am_i_insertable(find_function(GETORDERARGS(help, label))))
                               && (strcmp(act_func,GETORDERARGS(help, label))))
        {
          ORDER *ohelp = GETPREVORDER(help);
          sprintf(s,"I%d", inline_label);
          nhelp=new_order(label, s);
          insert_order_list(nhelp,GETNEXTORDER(help));
          inline_function(find_function(GETORDERARGS(help, label)), help, inline_label++);
          delete_order(help);
          help = ohelp;
        }
        break;
      case gammabeta:
        if ((optimize > 0) && (am_i_insertable(find_function(GETORDERARGS(help, label))))
                           && (strcmp(act_func, GETORDERARGS(help, label))))
        {
          ORDER *ohelp = GETPREVORDER(help);
          sprintf(s,"I%d", inline_label);
          nhelp=new_order(label, s);
          insert_order_list(nhelp,GETNEXTORDER(help));
          inline_function(find_function(GETORDERARGS(help, label)), help, inline_label++);
          delete_order(help);
          help = ohelp;
        }
        break;
      default:;
    }
    help = GETNEXTORDER(help);
  }
}

