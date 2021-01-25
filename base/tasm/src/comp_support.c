#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mytypes.h"
#include "main.h"
#include "dbug.h"
#include "buildtree.h"
#include "optimizer.h"

extern CODE *functable;
extern char *primf_tab[];
extern PROGRAM *program_start;
extern int ref_inferred;

char *gimme_code();
void compile_typed_primf();
FUNDESC *find_desc(FUNDESC *, char *);

char *get_str_space();
extern int yyfail();

/*************************************************************************************/
/* duplicates is function from argument which is a ptr to ORDER until end of func    */
/* if flag!=0 all typeinformations will be cleared.				     */ 
/*************************************************************************************/

ORDER *dup_func(ORDER *dup_me,int flag)
{
  ORDER *to_be_delivered, *act, *last, *help;
  
  DBUG_ENTER("dup_func");
  while ((NULL != dup_me) &&
	 ((hashargs == GETORDERENTRY(dup_me, command)) ||
	  (hashtildeargs == GETORDERENTRY(dup_me, command)) ||
	  (hashrestype == GETORDERENTRY(dup_me, command))) &&
	 (0 != flag))
    dup_me = GETNEXTORDER(dup_me);
  if (NULL != dup_me)
  {
    if (NULL == (to_be_delivered = (ORDER *)malloc(sizeof(ORDER))))
      yyfail("unable to allocate memory");
    memcpy(to_be_delivered, dup_me, sizeof(ORDER));
    if (NULL != GETORDERARGS(dup_me, label))
    {
      if (NULL == (SETORDERARGS(to_be_delivered, label) = 
                    (char *)malloc(strlen(GETORDERARGS(dup_me, label)) + 1))) 
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(to_be_delivered, label), GETORDERARGS(dup_me, label));
    }
    if (NULL != GETORDERARGS(dup_me, ret))
    {
      if (NULL == (SETORDERARGS(to_be_delivered, ret) = 
                    (char *)malloc(strlen(GETORDERARGS(dup_me, ret)) + 1))) 
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(to_be_delivered, ret), GETORDERARGS(dup_me, ret));
    }
    if (0!=flag)
    {
      SETORDERENTRY(to_be_delivered, types)=0;
      SETORDERENTRY(to_be_delivered, typestouched)=0;
    }
    SETPREVORDER(to_be_delivered) = NULL;
    SETNEXTORDER(to_be_delivered) = NULL;
    dup_me = GETNEXTORDER(dup_me);
    while ((NULL != dup_me) &&
	   ((hashargs == GETORDERENTRY(dup_me, command)) ||
	    (hashtildeargs == GETORDERENTRY(dup_me, command)) ||
	    (hashrestype == GETORDERENTRY(dup_me, command))) &&
	   (0 != flag))
      dup_me=GETNEXTORDER(dup_me);
    act = dup_me;
    last = to_be_delivered;
  }
  while (NULL != act)
  {
    if (NULL == (help = (ORDER *)malloc(sizeof(ORDER))))
      yyfail("unable to allocate memory");
    memcpy(help, act, sizeof(ORDER));
    if (NULL != GETORDERARGS(act, label))
    {
      if (NULL == (SETORDERARGS(help, label) = 
                    (char *)malloc(strlen(GETORDERARGS(act, label)) + 1))) 
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, label), GETORDERARGS(act, label));
    }
    if (NULL != GETORDERARGS(act, ret))
    {
      if (NULL == (SETORDERARGS(help, ret) = 
                    (char *)malloc(strlen(GETORDERARGS(act, ret)) + 1))) 
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, ret), GETORDERARGS(act, ret));
    }
    if (0!=flag)
    {
      SETORDERENTRY(help, types)=0;
      SETORDERENTRY(help, typestouched)=0;
    }
    SETNEXTORDER(last) = help; 
    SETPREVORDER(help) = last;
    SETNEXTORDER(help) = NULL;
    last = help;
    act = GETNEXTORDER(act);
    while ((NULL != act) &&
	   ((hashargs == GETORDERENTRY(act, command)) ||
	    (hashtildeargs == GETORDERENTRY(act, command)) ||
	    (hashrestype == GETORDERENTRY(act, command))) &&
	   (0 != flag))
      act = GETNEXTORDER(act);
  }
  DBUG_RETURN(to_be_delivered);
}


/*************************************************************************/
/* converts a stack_w-access to code and sets the code_ok-flag           */
/* primarily used by compile_typed_primf                                 */
/*************************************************************************/

char *gimme_code(ORDER *o)
{
  char text[80];

  DBUG_ENTER("gimme_code");
  
  switch(GETORDERENTRY(o, command))
  {
  case pushaw:
  {
    sprintf(text,"peek(a,%d)",GETORDERARGS(o, n));
    break;
  }
  case pushtw:
  {
    sprintf(text,"peek(t,%d)",GETORDERARGS(o, n));
    break;
  }
  case moveaw:
  {
    sprintf(text,"pop(a)");
    break;
  }
  case movetw:
  {
    sprintf(text,"pop(t)");
    break;
  }
  case pushcw_i:
  {
    sprintf(text,"%d",GETORDERARGS(o, n));
    break;
  }
  case pushcw_b:
  {
    if (GETORDERARGS(o, n)) 
      sprintf(text,"SA_TRUE");
    else 
      sprintf(text,"SA_FALSE");
    break;
  }
  default:
    fprintf(stderr,"Implement me in gimme_code !\n");
  }
  SETORDERENTRY(o, code) = get_str_space(text);
  strcpy(GETORDERENTRY(o, code), text);
  DBUG_RETURN(GETORDERENTRY(o, code));
}  


void make_one_function(PROGRAM *p)
{
  FUNCTION *func, *goal;
  ORDER *o1, *o2;

  DBUG_ENTER("make_one_function");
  goal = GETFIRSTFUNCTION(p);
  o1 = new_order(label, LABonefunc, "goal");
  insert_order_list(o1, GETFIRSTORDER(goal));
  func = GETNEXTFUNCTION(goal);
  while(NULL != func)
  {
    if (strcmp(GETNAME(func), "apply"))
    {
      /* first make a label and insert it*/
      o2 = new_order(label, LABonefunc, GETNAME(func));
      insert_order_list(o2, GETFIRSTORDER(func));
      /* second insert function */
      conc_order(o1, o2);
    }
    func = GETNEXTFUNCTION(func);
  }

  DBUG_VOID_RETURN;
}
/*************************************************************************/
/* counts the number of ASM-commands in a function                       */
/* except the function label                                             */
/*************************************************************************/

int count_command(FUNCTION *func) 
{
  int i = 0;
  ORDER *o;
  DBUG_ENTER("count_command");
  o = GETFIRSTORDER(func);
  while (o != NULL)
  {
    i++;
    o = GETNEXTORDER(o);
  }
  DBUG_RETURN(i);
}         
/*************************************************************************/
/* compiles all primitive functions without any type information         */
/* and converts all to reduma library calls                              */
/*************************************************************************/

void compile_delta(FUNCTION *f)
{
  ORDER *o;
  char text[160], help[80], *s;
  int refcnt_set;
  
  DBUG_ENTER("compile_delta");

  o = GETFIRSTORDER(f);
  while (NULL!=o)
  {
    refcnt_set = 0;
    switch(GETORDERENTRY(o, command))
    {
    case delta1:
    {
      if (uses_aux_var == GETORDERENTRY(GETPREVORDER(o), command))
        refcnt_set = 1; 
      switch(GETPRF(GETORDERARGS(o, primf)))
      {
      case p_to_field:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_to_field(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_to_field(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_to_list:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_to_list(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_to_list(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1, 
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_transform:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_transform(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_transform(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1, 
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_not:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_not(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_not(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_abs:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_abs(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_abs(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_neg:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_neg(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_neg(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
        if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_exp:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_exp(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_exp(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
        if (1 != refcnt_set)
         strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_ln:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_ln(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_ln(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_sin:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_sin(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_sin(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_cos:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_cos(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_cos(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_tan:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_tan(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_tan(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_floor:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_floor(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_floor(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_ceil:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_ceil(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_ceil(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_frac:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_frac(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_frac(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_trunc:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_trunc(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_trunc(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vc_plus:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vc_plus(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_vc_plus(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vc_minus:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vc_minus(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_vc_minus(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vc_mult:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vc_mult(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_vc_mult(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vc_div:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vc_div(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_vc_div(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vc_min:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vc_min(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_vc_min(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vc_max:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vc_max(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_vc_max(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_ldim:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_ldim(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_ldim(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vdim:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vdim(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_vdim(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_class:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_class(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_class(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_type:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_type(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_type(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_empty:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_empty(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_empty(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_transpose:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_transpose(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_transpose(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_reverse:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_reverse(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_reverse(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_to_scal:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_to_scal(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_to_scal(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_to_vect:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_to_vect(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_to_vect(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_to_tvect:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_to_tvect(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_to_tvect(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_to_mat:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_to_mat(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_to_mat(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_f_slots:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_slots(top(w)))\n");
	else
	  strcpy(text,"if (0 == red_slots(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_to_char:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
          strcpy(text,"if (0 == nr_red_to_char(top(w)))\n");
        else
          strcpy(text,"if (0 == red_to_char(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
        if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_to_ord:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
          strcpy(text,"if (0 == nr_red_to_ord(top(w)))\n");
        else
          strcpy(text,"if (0 == red_to_ord(top(w)))\n");
        sprintf(help, "\tf_mkdclos(%d, 1, 1, %s);\n", refcnt_set ? 1 : -1,
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
        if (1 != refcnt_set)
          strcat(text,"else repl(0, w, (int)_desc);\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      default:
      {
        yyfail("unknown primitive function with one arg in function %s.", 
               GETNAME(f));
      }
        ;
      } /* SWITCH(O->ARGS.PRIMF) */
      break;
    } /* CASE DELTA1: */
    case delta2:
    {
      if (uses_aux_var == GETORDERENTRY(GETPREVORDER(o), command))
        refcnt_set = 1; 
      switch(GETPRF(GETORDERARGS(o, primf)))
      {
      case p_mdim:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mdim(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_mdim(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_eq:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_eq(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_eq(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_ne:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_neq(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_neq(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_f_eq:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_f_eq(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_f_eq(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_f_ne:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_f_ne(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_f_ne(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_ge:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_geq(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_geq(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_gt:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_gt(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_gt(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_lt:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_lt(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_lt(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_le:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_le(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_le(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_minus:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_minus(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_minus(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_plus:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_plus(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_plus(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_mult:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mult(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_mult(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_div:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_div(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_div(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_mod:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mod(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_mod(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_ip:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_ip(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_ip(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_max:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_max(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_max(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_min:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_min(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_min(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_and:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_and(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_and(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_or:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_or(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_or(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_xor:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_xor(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_xor(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_lselect:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_lselect(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_lselect(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_lcut:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_lcut(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_lcut(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_lrotate:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_lrotate(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_lrotate(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_lunite:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_lunite(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_lunite(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vselect:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vselect(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_vselect(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vcut:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vcut(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_vcut(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vrotate:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vrotate(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_vrotate(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_vunite:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vunite(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_vunite(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_c_div:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_c_div(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_c_div(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_c_min:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_c_min(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_c_min(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_c_max:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_c_max(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_c_max(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_c_minus:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_c_minus(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_c_minus(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_c_mult:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_c_mult(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_c_mult(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
           strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_c_plus:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_c_plus(top(w), peek(w,1)))\n\n");
	else
	  strcpy(text,"if (0 == red_c_plus(top(w), peek(w,1)))\n\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
/* cr 22/03/95, kir(ff), START */
      case p_f_select:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_fselect(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_fselect(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_f_test:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_test(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_test(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_f_delete:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_delete(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_delete(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_quot:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_quot(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_quot(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_dim:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_dim(top(w), peek(w,1)))\n");
	else
	  strcpy(text,"if (0 == red_dim(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,     
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
/* cr 22/03/95, kir(ff), END */
      case p_sprintf:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
          strcpy(text,"if (0 == nr_red_sprintf(top(w), peek(w,1)))\n");
        else
          strcpy(text,"if (0 == red_sprintf(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
        if (1 != refcnt_set)
          strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_sscanf:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
          strcpy(text,"if (0 == nr_red_sscanf(top(w), peek(w,1)))\n");
        else
          strcpy(text,"if (0 == red_sscanf(top(w), peek(w,1)))\n");
        sprintf(help, "\tf_mkdclos(%d, 2, 2, %s);\n", refcnt_set ? 1 : -1,
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
        if (1 != refcnt_set)
          strcat(text,"else { drop(w, 1); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      default:
      {
        yyfail("unknown primitive function with two args in function %s.",
               GETNAME(f));
      }
        ;
      } /* SWITCH(O->ARGS.PRIMF) */
      break;
    } /* CASE DELTA2: */
      
    case delta3:
    {
      if (uses_aux_var == GETORDERENTRY(GETPREVORDER(o), command))
        refcnt_set = 1; 
      switch(GETPRF(GETORDERARGS(o, primf)))
      {
      case p_cut:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_cut(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_cut(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_substr:
      {      
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_substr(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_substr(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1, 
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;      
      }      
      case p_vreplace:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_vreplace(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_vreplace(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1, 
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_lreplace:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_lreplace(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_lreplace(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_mselect:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mselect(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_mselect(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_mcut:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mcut(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_mcut(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_mrotate:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mrotate(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_mrotate(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_munite:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_munite(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_munite(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_mre_r:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mre_r(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_mre_r(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_mre_c:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mre_c(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_mre_c(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_select:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_select(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_select(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1, 
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_rotate:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_rotate(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_rotate(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1, 
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_unite:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_unite(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_unite(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1, 
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_ltransform:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_ltransform(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_ltransform(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
/* cr 22/03/95, kir(ff), START */
      case p_f_update:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_update(top(w), peek(w,1), peek(w,2)))\n");
	else
	  strcpy(text,"if (0 == red_update(top(w), peek(w,1), peek(w,2)))\n");
        sprintf(help, "\tf_mkdclos(%d, 3, 3, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 2); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
/* cr 22/03/95, kir(ff), END */
      default:
      {
        yyfail("unknown primitive function with three args in function %s.",
               GETNAME(f));
      }
        ;  
      } /* SWITCH(O->COMMAND) */
      break;     
    } /* CASE DELTA3: */


    case delta4:
    {
      if (uses_aux_var == GETORDERENTRY(GETPREVORDER(o), command))
        refcnt_set = 1; 
      switch(GETPRF(GETORDERARGS(o, primf)))
      {
      case p_replace:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_replace(top(w), peek(w,1), peek(w,2), peek(w,3)))\n");
	else
	  strcpy(text,"if (0 == red_replace(top(w), peek(w,1), peek(w,2), peek(w,3)))\n");
        sprintf(help, "\tf_mkdclos(%d, 4, 4, %s);\n", refcnt_set ? 1 : -1, 
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)
          strcat(text,"else { drop(w, 3); repl(0, w, (int)_desc); }\n");
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_replace(top(w),peek(w,1),peek(w,2),peek(w,3));drop(w,4))");
	else
	  strcpy(text,"if (0 == red_replace(top(w),peek(w,1),peek(w,2),peek(w,3));drop(w,4); \npush((int)_desc);");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_mreplace:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mreplace(top(w), peek(w,1), peek(w,2), peek(w,3)))\n");
	else
	  strcpy(text,"if (0 == red_mreplace(top(w), peek(w,1), peek(w,2), peek(w,3)))\n");
        sprintf(help, "\tf_mkdclos(%d, 4, 4, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 3); repl(0, w, (int)_desc); }\n");
        if (ref_inferred)
	  strcpy(text,"if (0 == nr_red_mreplace(top(w),peek(w,1),peek(w,2),peek(w,3));drop(w,4);\npush((int)_desc);");
	else
	  strcpy(text,"if (0 == red_mreplace(top(w),peek(w,1),peek(w,2),peek(w,3));drop(w,4);\npush((int)_desc);");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      case p_repstr:
      {
        SETORDERENTRY(o, command) = code_ok;
        if (ref_inferred)
	  strcpy(text,"if (0== nr_red_repstr(top(w), peek(w,1), peek(w,2), peek(w,3)))\n");
	else
	  strcpy(text,"if (0 == red_repstr(top(w), peek(w,1), peek(w,2), peek(w,3)))\n");
        sprintf(help, "\tf_mkdclos(%d, 4, 4, %s);\n", refcnt_set ? 1 : -1,    
                      primf_tab[GETPRF(GETORDERARGS(o, primf))]);
        strcat(text, help);
         if (1 != refcnt_set)  
          strcat(text,"else { drop(w, 3); repl(0, w, (int)_desc); }\n");
        s=get_str_space(text);
        strcpy(s,text);
        SETORDERENTRY(o, code) = s;
        break;
      }
      default:
      {
        yyfail("unknown primitive function with four args in function %s.",
               GETNAME(f));
      }
        ;  
      } /* SWITCH(O->COMMAND) */     
    } /* CASE DELTA4: */
    case hashsetref:
      if (SREFexplicit == GETORDERARGS(o, n))
        ref_inferred = TRUE;
      if ((SREFimplicit == GETORDERARGS(o, n)) || (SREFkeepimpl == GETORDERARGS(o, n)))
        ref_inferred = FALSE;
      break; 
    default:
      ;
      
    } /* SWITCH(O->COMMAND) */ 
    o=GETNEXTORDER(o);   
  } /* WHILE (NULL!=o) */

  DBUG_VOID_RETURN;
}

  

/*************************************************************************/
/* Inserts a case for each label put into function and builds the        */
/* complete switchtable at the beginning of the function                 */
/*************************************************************************/


void insert_switchtable(FUNCTION *f, int i, ORDER *before_me)
{
  char *s, text[256], help[80];
  int j;
  
  
  DBUG_ENTER("insert_switch_table");
  insert_order_list(new_order(code_ok,"lab=kill_bits(pop(r));"),before_me);
  insert_order_list(new_order(code_ok,"LabSwitch:"),before_me);
  if (i > 4)
  {
    insert_order_list(new_order(code_ok,"switch(lab)"),before_me);
    insert_order_list(new_order(code_ok,"{"),before_me);

    for (j=0;j<i;j+=4)
    {
      sprintf(text,"case %d: goto L%d;",j,j);
      s=(char *)malloc(strlen(text)+1);
      if (NULL==s)
        yyfail("%s","Couldn't allocate memory !");
      strcpy(s,text);
      insert_order_list(new_order(code_ok,s),before_me);
    }
    sprintf(text, "default: fprintf(stderr, \"In function %s:\\n\");\n", GETNAME(f));
    strcat(text,"         fprintf(stderr,\"  Wrong Label: L%d;\\n\", lab);\n");
    sprintf(help, "controlled_exit(\"FATAL ERROR: Wrong Label in function %s\");\n", GETNAME(f));
    strcat(text, help);
    strcat(text, "exit(0);\n");
    s=(char *)malloc(strlen(text)+1);
    if (NULL==s)
      yyfail("%s","Couldn't allocate memory !");
    strcpy(s,text);
    insert_order_list(new_order(code_ok,s),before_me);
    
    insert_order_list(new_order(code_ok,"}"),before_me);
    
    DBUG_VOID_RETURN;
  }
}

/*************************************************************************/
/* Converts tail-jumps to tailnear or tailfar dependent if selftail-     */
/* recursive or not.                                                     */
/*************************************************************************/

void convert_tail(FUNCTION *f)
{
  char *act_func=GETNAME(f);
  ORDER *help=GETFIRSTORDER(f);
  
  DBUG_ENTER("convert_tail");
  while (NULL!=help)
  {
    switch(GETORDERENTRY(help, command))
    {
    case tail:
    {
/* check wether call to actual function **********/
      if (!strcmp(act_func, GETORDERARGS(help, label)))
      {
        SETORDERENTRY(help, command) = tailnear;
        SETORDERARGS(help, n) = 0;
      }
      else
      {
        SETORDERENTRY(help, command) = tailfar;
      }
      break;
    }
    default:
      ;
    }
    help=GETNEXTORDER(help);
  }
  DBUG_VOID_RETURN;
}

/*************************************************************************/
/* Traverses a whole function and inserts labels after each beta-call    */
/* Also checks if local or far jump and converts betaX and gammax-calls  */
/* to betaXfar|near, gammaXfar|near calls.                               */
/* Assumes NO code at beginning of function to insert LABEL0             */
/* Apply gets the return label in ORDER->args.m not n like the others    */
/*************************************************************************/
ORDER *search_last_delta(ORDER *o)
{
  while (((delta1 != GETORDERENTRY(o, command)) && 
          (stack_op != GETORDERENTRY(o, command)) && 
          (ris_stack_op != GETORDERENTRY(o, command))) && 
         ((delta2 != GETORDERENTRY(o, command)) && 
          (stack_op != GETORDERENTRY(o, command)) && 
          (ris_stack_op != GETORDERENTRY(o, command))) &&
         ((delta3 != GETORDERENTRY(o, command)) && 
          (stack_op != GETORDERENTRY(o, command)) &&
          (ris_stack_op != GETORDERENTRY(o, command))) &&
         ((delta4 != GETORDERENTRY(o, command)) && 
          (stack_op != GETORDERENTRY(o, command)) &&
          (ris_stack_op != GETORDERENTRY(o, command))))
    o = GETPREVORDER(o);
  return(o);
}

void insert_labels(FUNCTION *f)
{
  int i=4, j;
  ORDER *help, *nhelp, *help2, *insert_here;
  char *act_func=GETNAME(f),
       s[120]="", sp[64];
  
  DBUG_ENTER("insert_labels");

  help=GETFIRSTORDER(f);
  nhelp=new_order(code_ok,"L0:");
  insert_here=nhelp;
  insert_order_list(nhelp,help);  
  if (strncmp("case", GETNAME(f), 4) == 0)
    for(j = 0; j < GETDESCENTRY(GETDESC(f), nv); j++) {
      sprintf(sp, "sprintw(%d);", j);
      insert_order_list(new_order(code_ok,sp), help);
      }
  else
    for(j = 0; j < GETDESCENTRY(GETDESC(f), nv); j++) {
      sprintf(sp, "sprinta(%d);", j);
      insert_order_list(new_order(code_ok,sp), help);
      }
  for(j = 0; j < GETDESCENTRY(GETDESC(f), nfv); j++) {
    sprintf(sp, "sprintt(%d);", j);
    insert_order_list(new_order(code_ok,sp), help);
    }
  
  while ((code_ok == GETORDERENTRY(help, command)) || 
         (stack_op == GETORDERENTRY(help, command)))
    help=GETNEXTORDER(help);
  
  while (NULL!=help)
  {
    switch (GETORDERENTRY(help, command))
    {
    case pushaux:
      nhelp = new_order(uses_aux_var);
      help2 = search_last_delta(help);
      insert_order_list(nhelp, help2);
      break;
    case Case:
/* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc, s);
      insert_order_list(nhelp,GETNEXTORDER(help));
/* second: check if call to actual function **********/
      if (!strcmp(act_func,GETORDERARGS(help, label)))
      {
        SETORDERENTRY(help, command) = casenear;
      }
      else
      {
        SETORDERENTRY(help, command) = casefar;
      }
      if (NULL == (SETORDERARGS(help, ret) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, ret), act_func);
      SETORDERARGS(help, n) = i;
      i+=4;
      break;                  
    case gammacase:
/* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc, s);
      insert_order_list(nhelp,GETNEXTORDER(help));
/* second: check if call to actual function **********/
      if (!strcmp(act_func,GETORDERARGS(help, label)))
      {
        SETORDERENTRY(help, command) = gammacasenear;
      }
      else
      {
        SETORDERENTRY(help, command) = gammacasefar;
      }
      if (NULL == (SETORDERARGS(help, ret) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, ret), act_func);
      SETORDERARGS(help, n) = i;
      i+=4;
      break;                  
    case beta:
/* first: set the label behind the call **************/
        sprintf(s,"L%d",i);
        nhelp=new_order(label, LABfunc, s);
        insert_order_list(nhelp,GETNEXTORDER(help));
/* second: check if call to actual function **********/
        if (!strcmp(act_func,GETORDERARGS(help, label)))
        {
          SETORDERENTRY(help, command) = betanear;
        }
        else
        {
          SETORDERENTRY(help, command) = betafar;
        }
        if (NULL == (SETORDERARGS(help, ret) = (char *)malloc(strlen(act_func)+1)))
          yyfail("memory allocation failure");
        strcpy(SETORDERARGS(help, ret),act_func);
        SETORDERARGS(help, n) = i;
        i+=4;
      break;
    case tail:
/* check if call to actual function **********/
      if (!strcmp(act_func,GETORDERARGS(help, label)))
      {
        SETORDERENTRY(help, command) = tailnear;
      }
      else
      {
        SETORDERENTRY(help, command) = tailfar;
      }
      if (NULL == (SETORDERARGS(help, ret) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, ret),act_func);
      SETORDERARGS(help, n) = i;
      if ((rtf == GETORDERENTRY(GETNEXTORDER(help), command)) ||
          (rtt == GETORDERENTRY(GETNEXTORDER(help), command)) ||
          (rtc == GETORDERENTRY(GETNEXTORDER(help), command)) ||
          (rtp == GETORDERENTRY(GETNEXTORDER(help), command)))
        delete_order(GETNEXTORDER(help));

      break;

  case Gamma:
    {
         /* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc, s);
      insert_order_list(nhelp,GETNEXTORDER(help));
         /* second: check if call to actual function **********/
      if (!strcmp(act_func,GETORDERARGS(help, label)))
      {
        SETORDERENTRY(help, command) = gammanear;
      }
      else
      {
        SETORDERENTRY(help, command) = gammafar;
      }
      if (NULL == (SETORDERARGS(help, ret) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, ret), act_func);
      SETORDERARGS(help, n) = i;
      i+=4;
      break;
    }
  case gammabeta:
    {
/* first: set the label behind the call **************/
/* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp,GETNEXTORDER(help));
/* second: check if call to actual function **********/
      if (!strcmp(act_func,GETORDERARGS(help, label)))
      {
        SETORDERENTRY(help, command) = gammabetanear;
      }
      else
      {
        SETORDERENTRY(help, command) = gammabetafar;
      }
      if (NULL == (SETORDERARGS(help, ret) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, ret), act_func);
      SETORDERARGS(help, n) = i;
      i+=4;
      break;
    }

    case apply:
    {
/* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp,GETNEXTORDER(help));
/* second: check if call to actual function **********/
      if (NULL == (SETORDERARGS(help, ret) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, ret), act_func);
      SETORDERARGS(help, m)=i;
      i+=4;
      break;
    }
    case distend:
      sprintf(s,"L%d",i); 
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp, GETNEXTORDER(help));
      if (NULL == (SETORDERENTRY(help, code) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERENTRY(help, code), act_func);
      SETORDERARGS(help, j)=i;
      i+=4;
      break;
    case distb:
      sprintf(s,"L%d",i); 
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp, GETNEXTORDER(help));
      if (NULL == (SETORDERENTRY(help, code) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERENTRY(help, code), act_func);
      SETORDERARGS(help, j)=i;
      i+=4;
      break;
    case wait:
      sprintf(s,"L%d",i); 
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp, GETNEXTORDER(help));
      if (NULL == (SETORDERENTRY(help, code) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERENTRY(help, code), act_func);
      SETORDERARGS(help, j)=i;
      i+=4;
      break;



    default: 
      ;
    }   /* SWITCH */
    help=GETNEXTORDER(help);
  }  /* WHILE */
  reset_inlined_flags();
  insert_switchtable(f,i, insert_here);
  
  DBUG_VOID_RETURN;
}  


void insert_labels_one_function(FUNCTION *f)
{
  int i=4, j;
  ORDER *help, *nhelp, *help2, *insert_here;
  char *act_func=GETNAME(f),
       s[120]="", sp[64];
  
  DBUG_ENTER("insert_labels_one_function");

  help=GETFIRSTORDER(f);
  nhelp=new_order(code_ok,"L0:");
  insert_here=nhelp;
  insert_order_list(nhelp,help);  
  if (strncmp("case", GETNAME(f), 4) == 0)
    for(j = 0; j < GETDESCENTRY(GETDESC(f), nv); j++) {
      sprintf(sp, "sprintw(%d);", j);
      insert_order_list(new_order(code_ok,sp), help);
      }
  else
    for(j = 0; j < GETDESCENTRY(GETDESC(f), nv); j++) {
      sprintf(sp, "sprinta(%d);", j);
      insert_order_list(new_order(code_ok,sp), help);
      }
  for(j = 0; j < GETDESCENTRY(GETDESC(f), nfv); j++) {
    sprintf(sp, "sprintt(%d);", j);
    insert_order_list(new_order(code_ok,sp), help);
    }
  
  while ((code_ok == GETORDERENTRY(help, command)) || 
         (stack_op == GETORDERENTRY(help, command)))
    help=GETNEXTORDER(help);
  
  while (NULL!=help)
  {
    switch (GETORDERENTRY(help, command))
    {
    case label:
      if (LABonefunc == GETORDERARGS(help, n))
      {
        FUNDESC *des;
        sprintf(s,"%d",i);
	if (strcmp(GETORDERARGS(help, label), "goal"))
	  des = find_desc(GETFIRSTDESC(program_start), GETORDERARGS(help, label));
	else
	  des = GETFIRSTDESC(program_start);
	free(SETDESCENTRY(des, label));
	SETDESCENTRY(des, label) = (char *)malloc(strlen(s) + 3);
	strcpy(SETDESCENTRY(des,label), s);
        sprintf(s,"L%d",i);
        insert_order_list(new_order(label, LABfunc, s), help);
        i+=4;
      }
      break;
    case pushaux:
      nhelp = new_order(uses_aux_var);
      help2 = search_last_delta(help);
      insert_order_list(nhelp, help2);
      break;


    case Case:
    case gammacase:
/* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc, s);
      insert_order_list(nhelp,GETNEXTORDER(help));
      SETORDERARGS(help, n) = i;
      i+=4;
      break;                  

    case beta:
/* first: set the label behind the call **************/
        sprintf(s,"L%d",i);
        nhelp=new_order(label, LABfunc, s);
        insert_order_list(nhelp,GETNEXTORDER(help));
        SETORDERARGS(help, n) = i;
        i+=4;
      break;
    case tail:
/* check if call to actual function **********/
      SETORDERARGS(help, n) = i;
      if ((rtf == GETORDERENTRY(GETNEXTORDER(help), command)) ||
          (rtt == GETORDERENTRY(GETNEXTORDER(help), command)) ||
          (rtc == GETORDERENTRY(GETNEXTORDER(help), command)) ||
          (rtp == GETORDERENTRY(GETNEXTORDER(help), command)))
        delete_order(GETNEXTORDER(help));

      break;

  case Gamma:
    {
         /* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc, s);
      insert_order_list(nhelp,GETNEXTORDER(help));
      break;
    }
  case gammabeta:
    {
/* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp,GETNEXTORDER(help));
      SETORDERARGS(help, n) = i;
      i+=4;
      break;
    }

    case apply:
    {
/* first: set the label behind the call **************/
      sprintf(s,"L%d",i);
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp,GETNEXTORDER(help));
/* second: check if call to actual function **********/
      if (NULL == (SETORDERARGS(help, ret) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERARGS(help, ret), act_func);
      SETORDERARGS(help, m)=i;
      i+=4;
      break;
    }
    case distend:
      sprintf(s,"L%d",i); 
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp, GETNEXTORDER(help));
      if (NULL == (SETORDERENTRY(help, code) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERENTRY(help, code), act_func);
      SETORDERARGS(help, j)=i;
      i+=4;
      break;
    case distb:
      sprintf(s,"L%d",i); 
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp, GETNEXTORDER(help));
      if (NULL == (SETORDERENTRY(help, code) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERENTRY(help, code), act_func);
      SETORDERARGS(help, j)=i;
      i+=4;
      break;
    case wait:
      sprintf(s,"L%d",i); 
      nhelp=new_order(label, LABfunc,s);
      insert_order_list(nhelp, GETNEXTORDER(help));
      if (NULL == (SETORDERENTRY(help, code) = (char *)malloc(strlen(act_func)+1)))
        yyfail("memory allocation failure");
      strcpy(SETORDERENTRY(help, code), act_func);
      SETORDERARGS(help, j)=i;
      i+=4;
      break;



    default: 
      ;
    }   /* SWITCH */
    help=GETNEXTORDER(help);
  }  /* WHILE */
  reset_inlined_flags();
  insert_switchtable(f,i, insert_here);
  
  DBUG_VOID_RETURN;
}  

ORDER *last_push_r_before(ORDER *search_me)
{
  ORDER *found = search_me;
  
  DBUG_ENTER("last_push_r_before");
  while (pushr_p != GETORDERENTRY(found, command))
    found = GETPREVORDER(found);
  DBUG_RETURN(found);
}
