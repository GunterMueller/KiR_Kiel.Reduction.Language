#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "mytypes.h"
#include "loctypes.h"
#include "tiny.h"
#include "tis.h"
#include "buildtree.h"
#include "comp_support.h"
#include "dbug.h"
#include "error.h"

extern PROGRAM *program_start;
extern char *primf_tab[];
const char *prim_type_names[]={"none","int","real","digit","bool","char","empty",
			       "atom","freevar",NULL};
const char *s_prim_type_names[]={"?","i","r","d","b","$","e","a","v"};
const char *ext_type_names[]={"scalar","list","vect","tvect","matrix","string",NULL};
const char *s_ext_type_names[]={NULL,"l","v","t","m","s"};
const char *other_type_names[]={"prim","clos","func",NULL};
const char *s_other_type_names[]={"p","c","f"};

/* #ifndef DBUG_OFF */
/************************************************************************/
/* displays stacks all given stacks != NULL				*/
/* (function is not compiled if debugging is off)			*/
/************************************************************************/
void stack_display()
{
DBUG_ENTER("stack_display");
if (_db_keyword_("STACKS"))			/* option -#d,STACKS set */
  {
  printf("w:\t");				/* print w stack ... */
  if (NULL!=w)
    stelm_display(GETSTACKENTRY(w,stack),GETTOPOFSTACK(w));
  printf("\na:\t");
  if (NULL!=a)
    stelm_display(GETSTACKENTRY(a,stack),GETTOPOFSTACK(a));
  printf("\nt:\t");
  if (NULL!=t)
    stelm_display(GETSTACKENTRY(t,stack),GETTOPOFSTACK(t));
  printf("\nr:\t");
  if (NULL!=r)
    stelm_display(GETSTACKENTRY(r,stack),GETTOPOFSTACK(r));
  printf("\n");
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* displays i elements form array of STELM wich is referenced by stelm	*/
/* (function is not compiled if debugging is off)			*/
/************************************************************************/
void stelm_display(STELM *stelm, long i)
{
long j;

DBUG_ENTER("stelm_display");
for (j=0; j<i; j++)				/* all STELMs of stelm */
  {
  switch (stelm[j].type)
    {
    case tp_badrecur:
    case tp_illegal:
      {
      printf(" ([illegal])");
      break;
      }
    case tp_char:
    case tp_none:
    case tp_int:
    case tp_bool:
    case tp_real:
      {
      printf(" (%s)",prim_type_names[stelm[j].type]);
      break;
      }
    case tp_func:
      {
      if (NULL==stelm[j].dat.desc)
	printf(" (function)");			/* it's a unknown function */
      else
	printf(" (%s)",GETDESCENTRY(stelm[j].dat.desc,label));
      break;
      }
    case tp_prim:
      {
      printf(" (%s)",primf_tab[GETPRF(stelm[j].dat.prim)]);
      break;
      }
    case tp_clos:
      {
      if (_db_keyword_("CLOSURE"))	/* if -#d,CLOSURE then display frames */
	{
        printf(" (closure[w:");
	stelm_display(stelm[j].dat.clos->w,stelm[j].dat.clos->i);
	if (NULL!=stelm[j].dat.clos->desc)
	  {
	  printf(",t:");
	  stelm_display(stelm[j].dat.clos->t,stelm[j].dat.clos->k);
	  }
	printf("])");
	}
      else
	printf(" (closure[...])");
      break;
      }
    case tp_string:				/* string is shown as '...` */
      {
      printf(" '%s`",(tp_none!=stelm[j].subtype.type & PRIM_MASK) ?
	     prim_type_names[stelm[j].subtype.type & PRIM_MASK] :
	     ext_type_names[(stelm[j].subtype.type & EXT_MASK)>>4]);
      break;
      }
    case tp_list:				/* list is shown as [...] */
      {
      printf(" [%s]",(tp_none!=stelm[j].subtype.type & PRIM_MASK) ?
	     prim_type_names[stelm[j].subtype.type & PRIM_MASK] :
	     ext_type_names[(stelm[j].subtype.type & EXT_MASK)>>4]);
      break;
      }
    case tp_vect:				/* vect is shown as <...> */
      {
      printf(" <%s>",(tp_none!=stelm[j].subtype.type & PRIM_MASK) ?
	     prim_type_names[stelm[j].subtype.type & PRIM_MASK] :
	     ext_type_names[(stelm[j].subtype.type & EXT_MASK)>>4]);
      break;
      }
    case tp_tvect:				/* tvect is shown as >...< */
      {
      printf(" >%s<",(tp_none!=stelm[j].subtype.type & PRIM_MASK) ?
	     prim_type_names[stelm[j].subtype.type & PRIM_MASK] :
	     ext_type_names[(stelm[j].subtype.type & EXT_MASK)>>4]);
      break;
      }
    case tp_matrix:				/* matrix is shown as <:...:> */
      {
      printf(" <:%s:>",(tp_none!=stelm[j].subtype.type & PRIM_MASK) ?
	     prim_type_names[stelm[j].subtype.type & PRIM_MASK] :
	     ext_type_names[(stelm[j].subtype.type & EXT_MASK)>>4]);
      break;
      }
    case tp_var:
      {
      printf(" var");
      break;
      }
    default:
      yyfail("%s","unknown type for stelm_output");
    }
  }
DBUG_VOID_RETURN;
}
/* #endif */

/************************************************************************/
/* calculates the duplication index of the n given arguments on a STACK	*/
/* returns float value representing the arguments.			*/
/************************************************************************/
float calc_args_val(STELM *a,int n)
{
int i;
float res=0;

DBUG_ENTER("calc_args_val");
for (i=0; i<n; i++,a++)			/* with all arguments calculate*/
  {					/* duplication index */
  switch(a->type)
    {
    case tp_int:
      res+=VTP_INT;
      break;
    case tp_bool:
      res+=VTP_BOOL;
      break;
    case tp_prim:
      res+=VTP_PRIM;
      break;
    case tp_real:
      res+=VTP_REAL;
      break;
    case tp_func:
      res+=VTP_FUNC;
      break;
    case tp_list:
      res+=VTP_LIST;
      break;
    case tp_clos:
      res+=VTP_CLOS;
      break;
    case tp_vect:
    case tp_tvect:
      res+=VTP_VECT;
      break;
    case tp_matrix:
      res+=VTP_MATRIX;
      break;
    case tp_string:
      res+=VTP_STRING;
      break;
    case tp_atom:
      res+=VTP_ATOM;
      break;
    case tp_var:
      res+=VTP_VAR;
      break;
    case tp_none:
      res+=VTP_NONE;
      break;
    default:
      yyfail("%s","unknown type for argumentvalue calculation");
      break;
    }
  }
res/=n;						/* calculates average */
DBUG_RETURN(res);
}

/************************************************************************/
/* searches for ord on ret-STACK and sets ret(branch) to ord(branch)	*/
/* and clears ord(branch).						*/
/************************************************************************/
void backupoldjcond()
{
int i;

DBUG_ENTER("backupoldjcond");
for (i=GETTOPOFSTACK(ret)-1;i>=0;i--)
  {
  if (JCONDS(GETORDERENTRY(GETSTACKENTRY(ret,stack)[i].dat.ret->ord,command)) &&
      (0==GETSTACKENTRY(ret,stack)[i].dat.ret->branch))
    					/* if command == jcond and no branch is */
    					/* stored in returnadress, the executed */
    					/* branches get stored in returnadress */
    {
    GETSTACKENTRY(ret,stack)[i].dat.ret->branch=
      GETORDERENTRY(GETSTACKENTRY(ret,stack)[i].dat.ret->ord,branch);
    SETORDERENTRY(GETSTACKENTRY(ret,stack)[i].dat.ret->ord,branch)=0;
    }
  if (ord==GETSTACKENTRY(ret,stack)[i].dat.ret->ord)	/* actuell order reached */
    break;
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* checks if function/closure stelm has to be dumped and builds up	*/
/* enviroment if needed.						*/
/************************************************************************/
void dump_list_func(STELM *stelm)
{
CLOSURE *clos;
FUNDESC *des;
RETURNADR *rt;
int j;

DBUG_ENTER("dump_list_func");
if (tp_func==GETSTELMTYPE(stelm))		/* do we have a function ? */
  {
  if (NULL==(rt=malloc(sizeof(RETURNADR))))	/* get mem for returnadress */
    yyfail("%s","no memory for returnadress!");
  rt->a=NULL;					/* all stacks empty */
  rt->w=NULL;
  rt->t=NULL;
  rt->r=NULL;
  rt->status=CLOS_EXE;
  rt->branch=0;
  if (DT_CONDITIONAL==GETDESCENTRY(GETSTELMDATA(stelm,desc),tag))
    {
    for (j=0;j<GETSTELMDATA(stelm,desc)->nv;j++)
      rt->a=st_push(rt->a,&CONST_NONE);
    rt->w=st_push(rt->w,&CONST_NONE);
    }
  else						/* common function */
    {
    for (j=0;j<GETSTELMDATA(stelm,desc)->nv;j++)
      rt->w=st_push(rt->w,&CONST_NONE);
    }
  rt->w=st_push(rt->w,stelm);			/* store function label */
  rt->ord=new_order(apply,GETSTELMDATA(stelm,desc)->nv); /* create apply */
  rt->ord->next=&CONST_EXT;			/* and ext */
  rt->ord->prev=NULL;
  func_dump=st_push(func_dump,new_stelm(tp_returnadr,rt)); /* save on func_dump */
  }
if ((tp_clos==GETSTELMTYPE(stelm)) &&	    	/* closure, ... */
    (NULL!=GETSTELMDATA(stelm,clos)->desc))	/* ... but no deltaclos */
  {
  if (NULL==(rt=malloc(sizeof(RETURNADR))))	/* get mem for returnadress */
    yyfail("%s","no memory for returnadress!");
  rt->a=NULL;					/* all stacks empty */
  rt->w=NULL;
  rt->t=NULL;
  rt->r=NULL;
  rt->status=CLOS_EXE;
  rt->branch=0;
  clos=GETSTELMDATA(stelm,clos);
  des=clos->desc;
  if (DT_CONDITIONAL==GETDESCENTRY(des,tag))	/* createing missing arguments */
    {
    for (j=1;j<des->nv-clos->i;j++)
      rt->a=st_push(rt->a,&CONST_NONE);
    rt->w=st_push(rt->w,&CONST_NONE);
    }
  else						/* common [gamma]betaclos */
    {
    for (j=0;j<des->nv-clos->i;j++)
      rt->w=st_push(rt->w,&CONST_NONE);
    }
  rt->w=st_push(rt->w,stelm);			/* store closure */
  rt->ord=new_order(apply,des->nv-clos->i);	/* create apply */
  rt->ord->next=&CONST_EXT;			/* and ext */
  rt->ord->prev=NULL;
  func_dump=st_push(func_dump,new_stelm(tp_returnadr,rt)); /* save on func_dump */
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* checks on ret-stack for previous match with same fail-lable		*/
/* returns NULL if no previous match with same fail-label found else    */
/* returns returnadress of previous match				*/
/************************************************************************/
STELM *gotofail()
{
STELM *retur;
ORDER *ord1;

DBUG_ENTER("gotofail");
ord1=find_label(GETNEXTORDER(ord),GETORDERARGS(ord,label));
retur=st_read(ret,1);		/* search on returnstack for the same label */
if (MATCHX(GETORDERENTRY(retur->dat.ret->ord,command)) &&
    (ord1==find_label(retur->dat.ret->ord,
		       GETORDERARGS(retur->dat.ret->ord,label))))
  {
  SETORDERENTRY(ord,branch)=0;			/* clear executed branches */
  st_pop(ret);
  retur=st_pop(ret);
  DBUG_RETURN(retur);				/* returns adress to return */
  }
DBUG_RETURN((STELM *)NULL);			/* label not found on ret-STACK */
}

/************************************************************************/
/* clears functionresultentries for all depending not cleared functions */
/************************************************************************/
void clear_cache(FUNTYPES *ft)
{
DEPEND *dep;

DBUG_ENTER("clear_cache");
if ((kn_clear!=ft->knowledge) || (tp_none!=ft->result.type))
  					/* if cache-entry isn't cleared or */
					/* of type none */
  {
  ft->knowledge=kn_clear;		/* clear cache entry */
  while (NULL!=ft->depend)		/* clear depending entry, too */
    {
    clear_cache(ft->depend->func);
    dep=ft->depend->next;
    free(ft->depend);
    ft->depend=dep;
    }
  ft->knowledge=kn_no;			/* no result stored */
  ft->result.type=tp_illegal;
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* pops entries of RETURNADR-STACK until stelm is reached - clears all	*/
/* depending function-cache entries					*/
/************************************************************************/
void return_to_call(STELM *stelm)
{
RETURNADR *radr;
FUNDESC *des;
FUNLIST *fl;
FUNTYPES *ft;
STELM result,*dummy;

DBUG_ENTER("return_to_call");
while (st_read(ret,0)!=stelm)
  {
  dummy=st_pop(ret);
  if (JCONDS(dummy->dat.ret->ord->command))	
    				/* if poped ORDER is JCONDS then clear branches */
    dummy->dat.ret->ord->branch=0;
  }
radr=stelm->dat.ret;
des=find_desc(GETFIRSTDESC(program_start),radr->ord->args.label);
fl=searchfunlist(des->label);		/* searches for function-cache entry */
if (apply!=radr->ord->command)
  ft=searchftypes(fl->funtypes,(0==des->nv ) ? NULL : st_read(radr->w,des->nv-1),
		               (0==des->nfv) ? NULL : st_read(radr->t,des->nfv-1),
		  des->nv,des->nfv);
else
  ft=searchftypes(fl->funtypes,(0==des->nv ) ? NULL : st_read(radr->w,des->nv),
		               (0==des->nfv) ? NULL : st_read(radr->t,des->nfv-1),
		  des->nv,des->nfv);
stelm_cpy(&result,&ft->result,1);		/* backup stored result */
clear_cache(ft);				/* clear cache entries */
stelm_cpy(&ft->result,&result,1);		/* store back result */
DBUG_VOID_RETURN;
}

/************************************************************************/
/* sets depend pointer in FUNLIST of des to function old_func		*/
/************************************************************************/
void set_depend(FUNDESC *des,STACK *w, STACK *t,STELM *old_func)
{
FUNLIST *fl;
FUNTYPES *fts,*ftd;
RETURNADR *radr;
FUNDESC *s_des;
DEPEND *dep;

DBUG_ENTER("set_depend");
DBUG_PRINT("ORDERS",("calling set_depend ..."));
if ((NULL==old_func) || 
    (0==strcmp(des->label,old_func->dat.ret->ord->args.label))) /* selfdepend */
  DBUG_VOID_RETURN;
fl=searchfunlist(des->label);			/* get FUNLIST entry of des */
DBUG_ASSERT(NULL!=fl,"set_depend failed!");
if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
  ftd=searchftypes(fl->funtypes,(0==des->nv ) ? NULL : st_read(w,des->nv-1),
		                (0==des->nfv) ? NULL : st_read(t,des->nfv-1),
		   des->nv,des->nfv);
else						/* it a conditional */
  ftd=searchftypes(fl->funtypes,
		   (0==des->nv-1 ) ? NULL : st_read(w,des->nv-2),
		   (0==des->nfv/4) ? NULL : st_read(t,(des->nfv/4)-1),
		   des->nv-1,des->nfv/4);  
DBUG_ASSERT(NULL!=ftd,"set_depend failed!");
radr=old_func->dat.ret;
s_des=find_desc(GETFIRSTDESC(program_start),radr->ord->args.label);
fl=searchfunlist(s_des->label);
DBUG_ASSERT(NULL!=fl,"set_depend failed!");
if (apply==radr->ord->command)
  {
  if (DT_CONDITIONAL!=GETDESCENTRY(s_des,tag))
    {
    fts=searchftypes(fl->funtypes,
		     (0==s_des->nv ) ? NULL : st_read(radr->w,s_des->nv-1+1),
		     (0==s_des->nfv) ? NULL : st_read(radr->t,s_des->nfv-1),
		     s_des->nv,s_des->nfv);
    }
  else
    {
    fts=searchftypes(fl->funtypes,
		     (0==s_des->nv-1 ) ? NULL : st_read(radr->w,s_des->nv-2+1),
		     (0==s_des->nfv/4) ? NULL : st_read(radr->t,(s_des->nfv/4)-1),
		     s_des->nv-1,s_des->nfv/4);
    }
  }
else						/* must be [gamma]beta */
  {
  fts=searchftypes(fl->funtypes,
		   (0==s_des->nv ) ? NULL : st_read(radr->w,s_des->nv-1),
		   (0==s_des->nfv) ? NULL : st_read(radr->t,s_des->nfv-1),
		   s_des->nv,s_des->nfv);
  }
DBUG_ASSERT(NULL!=fts,"set_depend failed!");
dep=ftd->depend;				/* set dependency */
while (NULL!=dep)				/* follow dependencies */
  {
  if (fts==dep->func)
    DBUG_VOID_RETURN;
  dep=dep->next;
  }
if (NULL==(dep=malloc(sizeof(DEPEND))))
  yyfail("%s","no memory for dependecies!");
dep->next=ftd->depend;				/* append new dependency */
dep->func=fts;
ftd->depend=dep;
DBUG_VOID_RETURN;
}

/************************************************************************/
/* displays n elements form array of TP_TYPES				*/
/************************************************************************/
void print_types(TP_TYPES * types, int n)
  {
  int i;
  TP_TYPES *dummy;
  
  DBUG_ENTER("print_types");
  for (i = 0; i < n; i++)
    {
    switch (types[i] & (EXT_MASK | OTHER_MASK))
      {
      case tp_none:				/* prim_types */
        {
	fprintf(yyout, "%s",prim_type_names[types[i]]);
        break;
	}
      case tp_list:
        {
	fprintf(yyout, "[");
	if (NULL==(dummy=malloc(sizeof(TP_TYPES))))
	  yyfail("%s","can't allocmem for temporary type!");
	*dummy=types[i]^tp_list;
	print_types(dummy,1);
	free(dummy);
	fprintf(yyout, "]");
        break;
	}
      case (tp_list|OTHER_MASK):		/* list is shown as [...] */
	{
	fprintf(yyout, "[%s]",other_type_names[types[i]^(tp_list|OTHER_MASK)]);
	break;
	}
      case tp_string:				/* string is shown as '...` */
	{
	fprintf(yyout, "'");
	if (NULL==(dummy=malloc(sizeof(TP_TYPES))))
	  yyfail("%s","can't allocmem for temporary type!");
	*dummy=types[i]^tp_string;
	print_types(dummy,1);
	free(dummy);
	fprintf(yyout, "`");
        break;
	}	
      case tp_vect:				/* vect is shown as <...> */
	{
	fprintf(yyout,"<");
	if (NULL==(dummy=malloc(sizeof(TP_TYPES))))
	  yyfail("%s","can't allocmem for temporary type!");
	*dummy=types[i]^tp_vect;
	print_types(dummy,1);
	free(dummy);
	fprintf(yyout, ">");
	break;
	}
      case tp_tvect:				/* tvect is shown as >...< */
	{
	fprintf(yyout,">");
	if (NULL==(dummy=malloc(sizeof(TP_TYPES))))
	  yyfail("%s","can't allocmem for temporary type!");
	*dummy=types[i]^tp_tvect;
	print_types(dummy,1);
	free(dummy);
	fprintf(yyout, "<");
	break;
	}
      case tp_matrix:				/* matrix is shown as <:...:> */
	{
	fprintf(yyout,"<:");
	if (NULL==(dummy=malloc(sizeof(TP_TYPES))))
	  yyfail("%s","can't allocmem for temporary type!");
	*dummy=types[i]^tp_matrix;
	print_types(dummy,1);
	free(dummy);
	fprintf(yyout, ":>");
	break;
	}
      case tp_prim:
      case tp_func:
      case tp_clos:
	{
	fprintf(yyout, "%s",other_type_names[types[i] & (PRIM_MASK | EXT_MASK)]);
	break;
	}
      default:
        yyfail("%s","illegal type");
      }
    if (i+1 < n)
      fprintf(yyout, " ");
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* searchs on ret STACK for ord						*/
/* returns NULL if ord not found, ord if no legal jcond found until ord */
/* or returns pointer to legal found jcond				*/
/************************************************************************/
ORDER *search_ret(ORDER *ord)
{
long i,found=0;

DBUG_ENTER("search_ret");
if ((NULL==ret) || (1==new_params))
  DBUG_RETURN((ORDER *)NULL);
for (i=0; i<GETTOPOFSTACK(ret); i++)		/* search for ord on return-STACK */
  {
  if (ord==GETSTACKENTRY(ret,stack)[i].dat.ret->ord)
    found=1;
  }
if (0!=found)
  {
  for (i=GETTOPOFSTACK(ret)-1; i>=0; i--)	/* search for still executable */
    						/* JCONDS on ret-STACK */
    {
    if JCONDS(GETORDERENTRY(GETSTACKENTRY(ret,stack)[i].dat.ret->ord,command))
      {
      if (1==GETSTACKENTRY(ret,stack)[i].dat.ret->branch)	/* found */
	DBUG_RETURN(GETSTACKENTRY(ret,stack)[i].dat.ret->ord);
      if (1==GETORDERENTRY(GETSTACKENTRY(ret,stack)[i].dat.ret->ord,branch))
								/* found */
	DBUG_RETURN(GETSTACKENTRY(ret,stack)[i].dat.ret->ord);
      }
    }
  DBUG_RETURN(ord);				/* no JCONDS found */
  }
DBUG_RETURN((ORDER *)NULL);
}

/************************************************************************/
/* searchs on ret STACK for functioncall with label "label"		*/
/************************************************************************/
STACK *findfcall(STELM *stelm)
{
int i=0;
char *label;
FUNDESC *des;
STACK *dummy;
STELM *result;

DBUG_ENTER("findlabel");
if (NULL==(label=malloc(strlen(GETORDERARGS(GETSTELMDATA(stelm,ret)->ord,label))+1)))
  yyfail("%s","no memory for label!");
strcpy(label,GETORDERARGS(GETSTELMDATA(stelm,ret)->ord,label));
while ((i<GETTOPOFSTACK(ret)) && 
       ((NULL==GETORDERARGS(GETSTELMDATA(st_read(ret,i),ret)->ord,label)) ||
       (0!=strcmp(label,GETORDERARGS(GETSTELMDATA(st_read(ret,i),ret)->ord,label)))))
  i++;
if (i!=GETTOPOFSTACK(ret))			/* found something ? */
  {
  if (apply==GETORDERENTRY(GETSTELMDATA(st_read(ret,i),ret)->ord,command))
    {
    des=find_desc(GETFIRSTDESC(program_start),label);
    dummy=st_make(GETSTELMDATA(st_read(ret,i),ret)->w,GETDESCENTRY(des,nv)+1);
    st_pop(dummy);				/* remove function form w STACK */
    result=get_result(label,dummy,GETDESCENTRY(des,nv),
		      GETSTELMDATA(st_read(ret,i),ret)->t,GETDESCENTRY(des,nfv));
    st_free(dummy);
    if (NULL==result)				/* no result stored ? */
      {
      free(label);
      DBUG_RETURN((STACK *)NULL);
      }
    }
  else						/* [gamma]beta */
    {
    des=find_desc(GETFIRSTDESC(program_start),label);
    result=get_result(label,GETSTELMDATA(st_read(ret,i),ret)->w,GETDESCENTRY(des,nv),
		      GETSTELMDATA(st_read(ret,i),ret)->t,GETDESCENTRY(des,nfv));
    if (NULL==result)				/* no result stored */
      {
      free(label);
      DBUG_RETURN((STACK *)NULL);
      }
    }
  while (i>0)
    {
    result=st_pop(ret);
    if (JCONDS(GETORDERENTRY(GETSTELMDATA(result,ret)->ord,command)))
      					/* for all poped JCONDS clear branch */
      SETORDERENTRY(GETSTELMDATA(result,ret)->ord,branch)=0;
    st_free(GETSTELMDATA(result,ret)->w);	/* remove stored STACKs */
    st_free(GETSTELMDATA(result,ret)->a);
    st_free(GETSTELMDATA(result,ret)->r);
    st_free(GETSTELMDATA(result,ret)->t);
    i--;
    }
  free(label);
  DBUG_RETURN(ret);			/* return new ret STACK */
  }
free(label);
yyfail("%s","bad label for findcall!");
DBUG_RETURN((STACK *)NULL);
}

/************************************************************************/
/* searchs on ret STACK for last function call				*/
/* returns STELM of ret from last function call				*/
/************************************************************************/
STELM *getfunc_ret()
{
long i;

DBUG_ENTER("getfunc_ret");
for (i=GETTOPOFSTACK(ret)-1; i>=0; i--)
  {
  if BETAFUNCS(GETORDERENTRY(GETSTACKENTRY(ret,stack)[i].dat.ret->ord,command))
    DBUG_RETURN(&(GETSTACKENTRY(ret,stack)[i]));
  }
DBUG_RETURN((STELM *)NULL);
}

/************************************************************************/
/* restores enviroment of function ord and cleans ret			*/
/************************************************************************/
void restore_ret(ORDER *old_ord)
{
STELM *elm;

DBUG_ENTER("restore_ret");
elm=st_pop(ret);
while ((GETSTELMDATA(elm,ret)->ord!=old_ord) || 
       ((0==GETSTELMDATA(elm,ret)->branch) && 
	(3==GETORDERENTRY(GETSTELMDATA(elm,ret)->ord,branch))))
  {
  st_free(GETSTELMDATA(elm,ret)->w);		/* free stored STACKS */
  st_free(GETSTELMDATA(elm,ret)->a);
  st_free(GETSTELMDATA(elm,ret)->t);
  st_free(GETSTELMDATA(elm,ret)->r);
  elm=st_pop(ret);
  }
ord=elm->dat.ret->ord;				/* restore ORDER */
clos_exe=(GETSTELMDATA(elm,ret)->status&CLOS_EXE)/CLOS_EXE;
new_params=(GETSTELMDATA(elm,ret)->status&NEW_PARAMS)/NEW_PARAMS;
if (0!=GETSTELMDATA(elm,ret)->branch)		/* branch stored ? */
  SETORDERENTRY(ord,branch)=GETSTELMDATA(elm,ret)->branch;
w=elm->dat.ret->w;				/* restore STACKS */
a=elm->dat.ret->a;
if (((gammabeta==GETORDERENTRY(ord,command)) ||
     (gammacase==GETORDERENTRY(ord,command))) ||
    (Gamma==GETORDERENTRY(ord,command)))
  {						/* STACK switch */
  r=elm->dat.ret->t;
  t=elm->dat.ret->r;
  }
else
  {
  r=elm->dat.ret->r;
  t=elm->dat.ret->t;
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* returns const pointer of short-type-string of type			*/
/************************************************************************/
const char *short_type_names(TP_TYPES type)
{
char *dummy;

DBUG_ENTER("short_type_names");
if ((0<=type) && (OTHER_MASK>type))		/* normal prim or ext type */
  {
  if (PRIM_MASK<type)				/* extended type */
    {
    if ((tp_list==(type & EXT_MASK)) ||		/* check for list */
	(tp_string==(type & EXT_MASK)))
      DBUG_RETURN(s_ext_type_names[CALC_EXT(type)]);
    else				/* it must be vect, tvect or matrix */
      {
      if (NULL==(dummy=malloc(3)))		/* mem for two chars + \0 */
	yyfail("%s","can't alloc mem for temporary string!");
      dummy[0]=s_ext_type_names[CALC_EXT(type)][0];
      dummy[1]=s_prim_type_names[type & PRIM_MASK][0];
      dummy[2]=0;
      DBUG_RETURN(dummy);			/* returning string */
      }
    }
  else						/* prim type */
    DBUG_RETURN(s_prim_type_names[type]);
  }
else						/* other or special type */
  {
  if (OTHER_MASK<type)
    {
    if (tp_list==(type & tp_list))		/* that a list */
      DBUG_RETURN(s_ext_type_names[CALC_EXT(type & tp_list)]);
    else
      yyfail("%s","dunno type!");
    }
  else
    {
    if (0>type)						/* special type */
      yyfail("%s","type is not printable");
    else						/* other type */
      DBUG_RETURN(s_other_type_names[type-OTHER_MASK]);
    }
  }
DBUG_RETURN((char *)NULL);
}

/************************************************************************/
/* returns const pointer of type-string	of type				*/
/************************************************************************/
const char *long_type_names(TP_TYPES type)
{
DBUG_ENTER("long_type_names");
if ((0<=type) && (OTHER_MASK>type))		/* normal prim or ext type */
  {
  if (PRIM_MASK<type)				/* extended type */
    DBUG_RETURN(ext_type_names[CALC_EXT(type)]);
  else						/* prim type */
    DBUG_RETURN(prim_type_names[type]);
  }
else						/* other or special type */
  if (0>type)					/* special type */
    yyfail("%s","type is not printable");
  else						/* other type */
    DBUG_RETURN(other_type_names[type-OTHER_MASK]);
DBUG_RETURN((char *)NULL);
}

/************************************************************************/
/* searches for object address in FUNLIST and DATA			*/
/* returns pointer to new build STELM					*/
/************************************************************************/
STELM *find_address(void *adr)
{
STELM *res=NULL;
FUNDESC *froot;
DATANODE *data;

DBUG_ENTER("find_address");
froot=GETFIRSTDESC(program_start);			/* start with FUNDESCs */
while ((NULL!=froot) && (NULL==res))
  {
  if (GETDESCENTRY(froot,address)==(int)adr)		/* function found */
    res=new_stelm(tp_func,froot);		/* remember FUNDESC in STELM */
  else
    froot=GETNEXTDESC(froot);
  }
data=GETFIRSTDATA(program_start);			/* first DATANODE */
while ((NULL!=data) && (NULL==res))
  {
  if (GETDATAENTRY(data,address)==(int)adr)		/* dataadress found */
    {
    switch (GETDATAENTRY(data,tag))
      {
      case d_name:					/* free variable */
	{
	res=new_stelm(tp_var,NULL);
	break;
        }
      case d_real:					/* real value */
	{
        res=new_stelm(tp_real,NULL);
	break;
	}
      case d_list:					/* list */
	{
	int size,help,j;
	SUBTYPE tp={tp_empty,NULL};		/* init with empty subtype */
	STELM *dummy;
	
	size=GETDATAENTRY(data,u.v.size);		
	for (j=0; j<size; j++)				/* explore all elements */
	  {
	  help=GETDATAENTRY(data,u.v.data[j]);
	  if ((ISINT(help)) && 				/* int element */
	      ((tp_empty==tp.type) || (tp_int==tp.type))) /* list empty or */
							  /* int untill now */
	    tp.type=tp_int;
          else 
	    {
	    if ((ISBOOL(help)) && ((tp_empty==tp.type) || (tp_bool==tp.type)))
	      tp.type=tp_bool;				/* s.o. with bool */
            else
	      {
	      if ((ISPRF(help)) && ((tp_empty==tp.type) || (tp_prim==tp.type)))
		tp.type=tp_prim;			/* s.o. prim */
	      else 					/* build up complex type */
		{
		dummy=find_address((void *)help);
		if ((NULL!=dummy) && ((tp.type==GETSTELMTYPE(dummy)) ||
				      (tp_empty==tp.type)))
		  {
		  if ((((tp_list==GETSTELMTYPE(dummy)) ||
		        (tp_vect==GETSTELMTYPE(dummy))) ||
		       ((tp_tvect==GETSTELMTYPE(dummy)) ||
		        (tp_matrix==GETSTELMTYPE(dummy)))) ||
		      (tp_string==GETSTELMTYPE(dummy)))
		    {
		    if (tp_empty==tp.type)
		      {
		      tp.type=GETSTELMTYPE(dummy);
		      tp.next=subtype_cpy(&dummy->subtype);
		      }
		    else
		      {
		      tp.type=GETSTELMTYPE(dummy);
		      subtypecmp(tp.next,&dummy->subtype);
		      }
		    }
		  else
		    tp.type=GETSTELMTYPE(dummy);
		  }
                else
		  {
		  SUBTYPE *subt,*dsubt;
		  
                  tp.type=tp_none;
		  subt=tp.next;
		  while (NULL!=subt)			/* free whole structure */
		    {
		    dsubt=subt->next;
		    free(subt);
		    subt=dsubt;
		    }
		  tp.next=NULL;
		  }
		}
	      }
	    }
	  }
	res=new_stelm(tp_list,(void *)&tp);
	break;
	}
      case d_string:					/* string */
	{
	int size,help,j;
	SUBTYPE tp={tp_empty,NULL};
	
	size=GETDATAENTRY(data,u.v.size);
	for (j=0; j<size; j++)
	  {
	  help=GETDATAENTRY(data, u.v.data[j]);
	  if ((ISCHAR(help)) && ((tp_empty==tp.type) || (tp_char==tp.type)))
	    tp.type=tp_char;
          else
            tp.type=tp_none;
	  }
	res=new_stelm(tp_string,(void *)&tp);
	break;
	}
      case d_mat:					/* matrix */
      case d_vect:					/* vect */
      case d_tvect:					/* tvect */
	{
	SUBTYPE tp={tp_empty,NULL};
	
	switch (GETDATAENTRY(data,u.w.tag))
	  {
	  case d_int:
	    tp.type=tp_int;
	    break;
	  case d_bool:
	    tp.type=tp_bool;
	    break;
	  case d_string:
	    tp.type=tp_string;
	    break;
	  case d_real:
	    tp.type=tp_real;
	    break;
	  case d_undef:
	    break;
	  default:
	    yyfail("%s","unknown type in datanode");
	    break;
	  }
	if (d_mat==GETDATAENTRY(data,tag))
	  res=new_stelm(tp_matrix,(void *)&tp);
	else
	  {
	  if (d_vect==GETDATAENTRY(data,tag))
	    res=new_stelm(tp_vect,(void *)&tp);
	  else
	    {
	    if (d_tvect==GETDATAENTRY(data,tag))
	      res=new_stelm(tp_tvect,(void *)&tp);
	    else
	      yyfail("%s","unknown type in datanode");
	    }
	  }
	break;
	}
      default:
	{
        yyfail("%s","unknown type in datanode");
	break;
	}
      }
    }
  else
    data=GETNEXTDATA(data);
  }
DBUG_RETURN(res);			/* return found and translated data */
}

/************************************************************************/
/* checks for args hash                                                 */
/* returns 0 for false and 1 for true                                   */
/************************************************************************/
long check_hash_args(ORDER *ord)
{
DBUG_ENTER("check_hash_args");
if ((hashargs==GETORDERENTRY(ord,command)) || 
    (hashtildeargs==GETORDERENTRY(ord,command)))
  DBUG_RETURN(1);
DBUG_RETURN(0);
}

/************************************************************************/
/* inserts args as hash in orderlist					*/
/************************************************************************/
void insert_args(STACK *w,long n,STACK *t,long nf,ORDER *ord)
{
long i;
TP_TYPES *argtp;

DBUG_ENTER("insert_args");
if (0==check_hash_args(ord))			/* hash_args allready present */
  {
  if (0<n)					/* if there are args */
    {
    if (NULL==(argtp=malloc(n*sizeof(TP_TYPES))))  /* alloc type array */
      yyfail("%s","no memory for argtypes of function!\n");
    for (i=0; i<n; i++)				/* for all args */
      argtp[i]=(GETSTELMTYPE(st_read(w,i)))|(st_read(w,i)->subtype.type);
    insert_order_list(new_order(hashargs,n,argtp),ord);
    }
  if (0<nf)					/* if there are tildeargs */
    {
    if (NULL==(argtp=malloc(nf*sizeof(TP_TYPES)))) /* alloc type array */
      yyfail("%s","no memory for tildeargtypes of function!\n");
    for (i=0; i<nf; i++)			/* for all tildeargs */
      argtp[i]=(GETSTELMTYPE(st_read(t,i)))|(st_read(t,i)->subtype.type);
    insert_order_list(new_order(hashtildeargs,nf,argtp),ord);
    }
  }
else						/* args hash present */
  {
  if (0<n)					/* if there are args */
    {
    if (NULL==(argtp=malloc(n*sizeof(TP_TYPES))))
      yyfail("%s","no memory for argtypes of function!\n");
    for (i=0; i<n; i++)				/* for all args */
      {
      argtp[i]=(GETSTELMTYPE(st_read(w,i)))|(st_read(w,i)->subtype.type);
      if (argtp[i]!=GETORDERARGS(ord,argtp[i])) /* do types differ ? */
	{
	if ((tp_list==GETSTELMTYPE(st_read(w,i))) &&
	    (tp_empty==st_read(w,i)->subtype.type) &&
	    (tp_list==(GETORDERARGS(ord,argtp[i]) & tp_list)))
	  {
	  argtp[i]=GETORDERARGS(ord,argtp[i]);
	  st_read(w,i)->subtype.type=GETORDERARGS(ord,argtp[i])-tp_list;
	  }
	else
	  {
	  if ((tp_list!=GETSTELMTYPE(st_read(w,i))) ||
	      (tp_empty!=GETORDERARGS(ord,argtp[i])-tp_list) ||
	      (tp_list!=(GETORDERARGS(ord,argtp[i]) & tp_list)))
	    argtp[i]=tp_none;			/* then type is none */
	  }
	}
      }
    free(GETORDERARGS(ord,argtp));		/* free old args */
    SETORDERARGS(ord,argtp)=argtp;		/* store new args */
    ord=GETNEXTORDER(ord);
    }
  if (0<nf)					/* if there are tildeargs */
    {
    if (NULL==(argtp=malloc(nf*sizeof(TP_TYPES))))
      yyfail("%s","no memory for argtypes of function!\n");
    for (i=0; i<nf; i++)			/* for all tildeargs */
      {
      argtp[i]=(GETSTELMTYPE(st_read(t,i)))|(st_read(t,i)->subtype.type);
      if (argtp[i]!=GETORDERARGS(ord,argtp[i]))	/* do types differ ? */
	{
	if ((tp_list==GETSTELMTYPE(st_read(t,i))) &&
	    (tp_empty==st_read(t,i)->subtype.type) &&
	    (tp_list==(GETORDERARGS(ord,argtp[i]) & tp_list)))
	  {
	  argtp[i]=GETORDERARGS(ord,argtp[i]);
	  st_read(t,i)->subtype.type=GETORDERARGS(ord,argtp[i])-tp_list;
	  }
	else
	  {
	  if ((tp_list!=GETSTELMTYPE(st_read(t,i))) ||
	      (tp_empty!=GETORDERARGS(ord,argtp[i])-tp_list) ||
	      (tp_list!=(GETORDERARGS(ord,argtp[i]) & tp_list)))
	    argtp[i]=tp_none;			/* then type is none */
	  }
	}
      }
    free(GETORDERARGS(ord,argtp));		/* free old tildeargs */
    SETORDERARGS(ord,argtp)=argtp;		/* store new tildeargs */
    }
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* inserts restype as hash in orderlist					*/
/************************************************************************/
void insert_restype(STELM *res, ORDER *o)
{
ORDER *ord;
TP_TYPES *argtp;

DBUG_ENTER("insert_restype");
if (NULL==o->prev)				/* is there no previous ORDER ? */
  DBUG_VOID_RETURN;
if (hashrestype!=GETORDERENTRY(GETORDERENTRY(o,prev),command))
  {
  if ((tp_list==GETSTELMTYPE(res)) || (tp_string==GETSTELMTYPE(res)) ||
      (tp_vect==GETSTELMTYPE(res)) || (tp_tvect==GETSTELMTYPE(res)) ||
      (tp_matrix==GETSTELMTYPE(res)))
    {
    if (NULL==(argtp=malloc(sizeof(TP_TYPES))))
      yyfail("%s","no memory for restype!");
    argtp[0]=GETSTELMTYPE(res) | res->subtype.type;
    ord=new_order(hashrestype,1,argtp);			/* create hash ORDER */
    insert_order_list(ord,o);
    }
  else
    {
    if (NULL==(argtp=malloc(sizeof(TP_TYPES))))
      yyfail("%s","no memory for restype!");
    argtp[0]=GETSTELMTYPE(res);
    ord=new_order(hashrestype,1,argtp);			/* create hash ORDER */
    insert_order_list(ord,o);
    }
  }
else						/* restype allready set */
  {
  o=GETPREVORDER(o);
  SETORDERARGS(o,argtp[0])=GETSTELMTYPE(res)|res->subtype.type;
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* searches for matching functionresult in FUNLIST			*/
/* returns pointer to matching STELM located in FUNTYPE			*/
/* sets global variable 'know' to stored KNOWLEDGE			*/
/************************************************************************/
STELM *get_result(char *s, STACK *a, long n, STACK *t, long nt)
{
STELM *res=NULL;				/* preset to nothing found */
FUNLIST *flp;
FUNTYPES *ft;

DBUG_ENTER("get_result");
DBUG_PRINT("FUNLIST",("->Fun: %s Par.: %d TPar.: %d",s,n,nt));
flp=searchfunlist(s);				/* search for cache-entry */
if (NULL!=flp)						/* is there a match */
  {
  ft=searchftypes(flp->funtypes,(0==n ) ? NULL : st_read(a,n-1),
		                (0==nt) ? NULL : st_read(t,nt-1),
		  n,nt);		/* is there an entry with this args ? */
  if (NULL!=ft)						/* found match */
    {
    res=&(ft->result);					/* set stored result */
    know=ft->knowledge;					/* set knowledge */
    if ((apply!=GETORDERENTRY(ord,command)) && 		/* BETAFUNCS but no apply */
	BETAFUNCS(GETORDERENTRY(ord,command)))
      {
      free(SETORDERARGS(ord,label));			/* free old label */
      if (NULL==(SETORDERARGS(ord,label)=malloc(strlen(ft->label)+1)))
        yyfail("%s","no memory for label!");
      strcpy(SETORDERARGS(ord,label),ft->label); /* set new label for these args */
      }
    new_params=0;					/* no new parameter */
    }
  else
    {
    insert_functype(s,a,n,t,nt,NULL,kn_no);		/* init function cache */
    know=kn_no;
    new_params=1; 					/* new parameter */
    }
  }
else							/* create new cache entry */
  {
  insert_functype(s,a,n,t,nt,NULL,kn_no);
  know=kn_no;
  new_params=0;
  }
DBUG_PRINT("FUNLIST",("<-Result: %d",(NULL==res) ? tp_illegal : GETSTELMTYPE(res)));
DBUG_RETURN(res);					/* return cached result */
}

/************************************************************************/
/* frees subtypes structure						*/
/************************************************************************/
void free_subtype(STELM *stelm)
{
SUBTYPE *st,*dst;

DBUG_ENTER("free_subtype");
st=stelm->subtype.next;				/* goto fist dynamic substruct */
while (NULL!=st)				/* free whole structure */
  {
  dst=st->next;
  free(st);
  st=dst;
  }
stelm->subtype.next=NULL;
DBUG_VOID_RETURN;
}

/************************************************************************/
/* copies subtypes to new created subtype and				*/
/* returns pointer to new subtype or NULL if empty			*/
/************************************************************************/
SUBTYPE *subtype_cpy(SUBTYPE *st)
{
SUBTYPE *dst,*ost;

DBUG_ENTER("subtype_cpy");
if (NULL==st)						/* no subtype */
  DBUG_RETURN((SUBTYPE *)NULL);
if (NULL==(ost=(dst=malloc(sizeof(SUBTYPE)))))
  yyfail("%s","no memory for subtypestructure");
while (NULL!=st->next)					/* goto beginning */
  {
  dst->type=st->type;					/* copy type */
  if (NULL==(dst->next=malloc(sizeof(SUBTYPE))))
    yyfail("%s","no memory for subtypestructure");
  st=st->next;						/* goto next entry */
  dst=dst->next;
  }
dst->type=st->type;
dst->next=NULL;						/* the end my friend */
DBUG_RETURN(ost);					/* return copy */
}

/************************************************************************/
/* compares two subtypes of lists, removes all unmatched subtypes of 	*/ 
/* subtype one								*/
/************************************************************************/
void subtypecmp(SUBTYPE *st1,SUBTYPE *st2)
{
DBUG_ENTER("subtypecmp");
if (NULL!=st1)					/* is there a subtype ? */
  {
  while ((NULL!=st1) && (NULL!=st2))		/* while data present */
    {
    if (st1->type!=st2->type)			/* types do not match */
      {
      st1->type=tp_none;			/* set type to none */
      st2=st1->next;
      st1->next=NULL;				/* end of subtype */
      while (NULL!=st2)				/* clear the tail */
	{
	st1=st2->next;
	free(st2);
	st2=st1;
	}
      }
    else
      {
      st1=st1->next;				/* advance to next type */
      st2=st2->next;
      }
    }
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* compares two arrays of STELM	type					*/
/* returns 0 if both arrays are equal, else -1				*/
/************************************************************************/
int stelmcmp(STELM *stelm1,STELM *stelm2,int n)
{
int i;

DBUG_ENTER("stelmcmp");
for (i=0; i<n; i++)				/* for all args do ... */
  {
  if (stelm1[i].type!=stelm2[i].type)		/* if types aren't equal */
    DBUG_RETURN(-1);
  else						/* checking subtypes */
    {
    switch (stelm1[i].type)
      {      
      case tp_func:
	{
	if (stelm1[i].dat.desc!=stelm2[i].dat.desc) /* the same function */
	  DBUG_RETURN(-1);
	break;
	}
      case tp_vect:
      case tp_tvect:
      case tp_matrix:
      case tp_string:
      case tp_list:
	{
	if (stelm1[i].subtype.type!=stelm2[i].subtype.type)
	  DBUG_RETURN(-1);
	break;
	}
      case tp_clos:
	{
	if (stelm1[i].dat.clos->desc!=stelm2[i].dat.clos->desc)
	  					/* closure of same function */
	  DBUG_RETURN(-1);
	if (((stelm1[i].dat.clos->i!=stelm2[i].dat.clos->i) ||
	     (stelm1[i].dat.clos->j!=stelm2[i].dat.clos->j)) ||
	    (stelm1[i].dat.clos->k!=stelm2[i].dat.clos->k))
	  				/* same number of stored args & tildeargs */
	  DBUG_RETURN(-1);
	if (0!=stelmcmp(stelm1[i].dat.clos->w,stelm2[i].dat.clos->w,
		         stelm1[i].dat.clos->i))	/* do the args match */
	  DBUG_RETURN(-1);
	if ((NULL!=stelm1[i].dat.clos->desc) &&
	    (0!=stelmcmp(stelm1[i].dat.clos->t,stelm2[i].dat.clos->t,
			 stelm1[i].dat.clos->k)))	/* do the tildeargs match */
	  DBUG_RETURN(-1);
	break;
	}
      case tp_var:
      case tp_illegal:
      case tp_badrecur:
      case tp_int:
      case tp_real:
      case tp_bool:
      case tp_prim:
      case tp_none:
      case tp_returnadr:
      case tp_empty:
      case tp_char:
	break;						/* nothing to do */
      default:
	yyfail("%s","unknown stelmtype!");
      }
    }
  }
DBUG_RETURN(0);						/* args do match */
}

/************************************************************************/
/* searches for function in functionlist with matching a & t frames	*/
/* returns pointer to matching funtype or NULL for no match		*/
/************************************************************************/
FUNTYPES *searchftypes(FUNTYPES *ft, STELM *a, STELM *t, long n, long nt)
{
char found=0;

DBUG_ENTER("searchftypes");
DBUG_PRINT("FUNLIST",("->Par.: %d TPar.: %d",n,nt));
while ((NULL!=ft) && (0==found))
  {
  if (((0==n ) || (0==stelmcmp(a,ft->params,n))) &&
      ((0==nt) || (0==stelmcmp(t,ft->tparams,nt))))
    found=1;						/* we have a match */
  else
    ft=ft->next;					/* goto next entry */
  }
DBUG_PRINT("FUNLIST",("<-Res.: %s",(NULL==ft) ? "not found" : "found"));
DBUG_RETURN(ft);
}

/************************************************************************/
/* searches for matching name in FUNLIST				*/
/* returns pointer to matching FUNLIST entry or NULL			*/
/************************************************************************/
FUNLIST *searchfunlist(char *name)
{
char found=0;
FUNLIST *root=flbase;

DBUG_ENTER("searchfunlist");
DBUG_PRINT("FUNLIST",("->Fun. Name: %s",name));
while ((NULL!=root) && (0==found))
  {
  if (0==strcmp(root->name,name))		/* name comparison */
    found=1;					/* we have a match */
  else
    root=root->next;				/* goto next entry */
  }
DBUG_PRINT("FUNLIST",("<-Res.: %s",(NULL==root) ? "not found" : "found"));
if ((NULL!=root) && (NULL!=root->org))	/* if root exists goto substructure */
  root=root->org;
DBUG_RETURN(root);
}

/************************************************************************/
/* inserts new/old functiontype in FUNLIST or modifies storage		*/
/* return int != 0 if typeclash occures					*/
/************************************************************************/
int insert_functype(char *name,STACK *a,long n,STACK *t,long nt,STELM *res,
		    KNOWLEDGE kn)
{
FUNLIST *flp,*flp_dup;
FUNTYPES *ft;
FUNDESC *des;
int sd;
float val;

DBUG_ENTER("insert_functype");
DBUG_PRINT("FUNLIST",("->Fun. Name: %s Par.: %d TPar.: %d Res.: %d",name,n,nt,
		      (NULL==res) ? tp_illegal : GETSTELMTYPE(res)));
flp=searchfunlist(name);			/* search for FUNLIST entry */
if (NULL==flp)					/* no entry -> build new entry */
  {
  if (NULL==(flp=malloc(sizeof(FUNLIST))))
    yyfail("%s","no memory for functionlist!");
  if (NULL==(flp->name=malloc(strlen(name)+1)))
    yyfail("%s","no memory for funtion name!");
  strcpy(flp->name,name);			/* store funcname */
  flp->a=n;					/* store arity */
  flp->t=nt;					/* store tildearity */
  flp->org=NULL;				/* it's the original */
  flp->next=flbase;
  flbase=flp;
  flp->funtypes=NULL;				/* no types set until now */
  }
ft=searchftypes(flp->funtypes,(NULL==a) ? NULL : st_read(a,n-1),
		              (NULL==t) ? NULL : st_read(t,nt-1),
		n,nt);
if (NULL==ft)					/* no matching type */
  {
  if (NULL==(ft=malloc(sizeof(FUNTYPES))))
    yyfail("%s","no memory for function parameters!");
  ft->next=flp->funtypes;			/* insert new type */
  ft->depend=NULL;				/* no dependecies */
  des=find_desc(GETFIRSTDESC(program_start),name);
  val= (NULL==a) ? 0.0 : calc_args_val(st_read(a,n-1),n)*2;
  val+=(NULL==t) ? 0.0 : calc_args_val(st_read(t,nt-1),nt);
  val=val/3;
  if (((0!=do_dupe) && (apply!=GETORDERENTRY(ord,command)) && (0<des->nv)) &&
      ((0==arg_check) || (1.0>=val)))
    					/* is there any code to be duplicated */
    {
    if (NULL==(ft->label=malloc(strlen(name)+4))) /* get mem for new label */
      yyfail("%s","no memory for label!");
    sd=((long)searchforfuncname(name))-((long)name);
    strncpy(ft->label,name,sd);
    sprintf(&(ft->label[sd]),"%d",dup_count);
    sprintf(&(ft->label[strlen(ft->label)]),
	    "_%s",searchfor_(name));		/* new name created */
    dup_count++;				/* duplication counter */
    if (dup_count==dup_num)			/* if max reached */
      do_dupe=0;				/* set no further duplication */
    conc_desc(GETFIRSTDESC(program_start),new_desc(des->tag,
	      max_desc_adr+(dup_count*16),des->nfv,des->nv,des->graph,ft->label));
    						/* new desc build up */
    ft->ord=dup_func(find_func(GETFIRSTFUNCTION(program_start),name),1);
    conc_function(GETFIRSTFUNCTION(program_start),new_function(ft->label,ft->ord));
    if (NULL==(SETORDERARGS(ord,label)=malloc(strlen(ft->label)+1)))
      yyfail("%s","no memory for label!");
    strcpy(GETORDERARGS(ord,label),ft->label);
    if (NULL==(flp_dup=malloc(sizeof(FUNLIST))))
      yyfail("%s","no memory for functionlist!");
    if (NULL==(flp_dup->name=malloc(strlen(ft->label)+1)))
      yyfail("%s","no memory for funtion name!");
    strcpy(flp_dup->name,ft->label);		/* store funcname */
    flp_dup->a=n;				/* store arity */
    flp_dup->t=nt;				/* store tildearity */
    flp_dup->org=flp;				/* there is the original */
    flp_dup->next=flbase;
    flbase=flp_dup;
    flp_dup->funtypes=NULL;			/* no types set until now */
    }
  else						/* no duplication */
    {
    if (NULL==(ft->label=malloc(strlen(name)+1)))
      yyfail("%s","no memory for label!");
    strcpy(ft->label,name);			/* save function name */
    }
  flp->funtypes=ft;
  know=kn;					/* set knowledge */
  if (0<n)					/* get memory for a frame */
    {
    if (NULL==(ft->params=malloc(n*sizeof(STELM))))
      yyfail("%s","no memory for function parameters!");
    stelm_cpy(ft->params,st_read(a,n-1),n); 	/* copy frame */
    }
  else						/* no frame */
    ft->params=NULL;
  if (0<nt)					/* get memory for t frame */
    {
    if (NULL==(ft->tparams=malloc(nt*sizeof(STELM))))
      yyfail("%s","no memory for function parameters!");
    stelm_cpy(ft->tparams,st_read(t,nt-1),nt); /* copy frame */
    }
  else						/* no frame */
    ft->tparams=NULL;
  if (NULL!=res)
    stelm_cpy(&ft->result,res,1);		/* store result */
  else						/* no result to store */
   {
   ft->result.type=tp_illegal;
   ft->result.subtype.type=tp_none;
   ft->result.subtype.next=NULL;
   }
  DBUG_PRINT("FUNLIST",("<-new entry in FUNLIST %s %d %d %d",name,n,nt,
			(NULL==res) ? tp_illegal : GETSTELMTYPE(res)));
  }
else						/* change searchfuntype */
  {
  if (tp_illegal!=ft->result.type)		/* prev stored type exists */
    {
    if ((0!=stelmcmp(&(ft->result),res,1)) &&
	(tp_none!=ft->result.type))		/* old restype != newrestype */
      {
      if (tp_clos==GETSTELMTYPE(res))		/* closure must be executed */
	{
	dump_list_func(res);
	if (tp_clos==ft->result.type)
	  dump_list_func(&(ft->result));
	}
      if ((ft->result.type==res->type) && 
	  ((tp_list==res->type) || (tp_matrix==res->type) ||
	   (tp_tvect==res->type) || (tp_vect==res->type)))
	{
	if (tp_empty==ft->result.subtype.type)
	  {
	  ft->result.subtype.type=res->subtype.type;
	  ft->result.subtype.next=NULL;
	  ft->result.dat.desc=GETSTELMDATA(res,desc);
	  }
	else
	  {
	  if (tp_empty!=res->subtype.type)
	    {
	    ft->result.type=tp_none;
      	    ft->result.dat.desc=GETSTELMDATA(res,desc);
            know=kn_sure;
            DBUG_RETURN(1);
	    }
	  }
	}
      else
	{
        ft->result.type=tp_none;
        ft->result.dat.desc=GETSTELMDATA(res,desc);
        know=kn_sure;
        DBUG_RETURN(1);
	}
      }
    }
  else						/* no type stored */
    {
    if (NULL!=res)
      {
      stelm_cpy(&ft->result,res,1);		/* store result */
      know=kn;
      }
    else
      {
      ft->result.type=tp_illegal;   
      know=kn_no;
      }
    }
  }
DBUG_RETURN(0);
}

/************************************************************************/
/* searches FUNLIST for function named s				*/
/* returns first ORDER of function or NULL				*/
/************************************************************************/
ORDER *find_func(FUNCTION *fun,char *s)
{
DBUG_ENTER("find_func");
while ((NULL!=fun) && (0!=strcmp(s,GETNAME(fun))))
  fun=GETNEXTFUNCTION(fun);
DBUG_RETURN(GETFIRSTORDER(fun));
}

/************************************************************************/
/* searches for function named label in functiondescriptorlist 		*/
/* returns pointer of FUNDESC or NULL					*/
/************************************************************************/
FUNDESC *find_desc(FUNDESC *des,char *label)
{
char found=0;

DBUG_ENTER("finddesc");
while ((NULL!=des) && (0==found))
  {
  if (0==strcmp(label, GETDESCENTRY(des,label)))  /* function names match */
    found=1;					/* we have a match */
  else
    des=GETNEXTDESC(des);			/* goto next entry */
  }
DBUG_RETURN(des);
}

/************************************************************************/
/* searches for functionlabel s in given orderlist ord			*/
/* returns pointer of first order after label s				*/
/************************************************************************/
ORDER *find_label(ORDER *ord, char *s)
{
char found=0;
ORDER *d;

DBUG_ENTER("find_label");
d=ord;
while ((0==found) && (end!=GETORDERENTRY(ord,command)))	/* search forward */
  {
  if ((label==GETORDERENTRY(ord,command)) && (0==strcmp(s,GETORDERARGS(ord,label))))
    found=1;						/* we have a match */
  ord=GETNEXTORDER(ord);				/* advance to next ORDER */
  }
while (0==found)					/* search backward */
  {
  if ((label==GETORDERENTRY(ord,command)) && (0==strcmp(s,GETORDERARGS(ord,label))))
    found=1;						/* we have a match */
  else
    ord=GETPREVORDER(ord);				/* back to last ORDER */
  }  
while (label==GETORDERENTRY(ord,command))	/* goto first "real" ORDER */
  ord=GETNEXTORDER(ord);
DBUG_RETURN(ord);
}

/************************************************************************/
/* allocates and inititialises new stack element			*/
/* returns pointer of new STELM						*/
/************************************************************************/
STELM *new_stelm(TP_TYPES tp,void *data)
{
STELM *help;

DBUG_ENTER("new_stelm");
if (NULL==(help=malloc(sizeof(STELM))))			/* get memory */
  yyfail("couldn't allocate memory for stack element");
SETSTELMTYPE(help)=tp;					/* set type of STELM */
help->subtype.type=tp_none;				/* clear subtype */
help->subtype.next=NULL;
switch (tp)
  {
  case tp_func:
    {
    SETSTELMDATA(help,desc)=data;			/* set FUNDESC of STELM */
    break;
    }
  case tp_clos:
    {
    SETSTELMDATA(help,clos)=data;			/* set CLOSURE of STELM */
    break;
    }
  case tp_illegal:
  case tp_badrecur:
  case tp_int:
  case tp_real:
  case tp_bool:
  case tp_empty:
  case tp_prim:
  case tp_none:
  case tp_char:
  case tp_var:
    {
    SETSTELMDATA(help,desc)=data;			/* set additional data */
    break;
    }
  case tp_returnadr:
    {
    SETSTELMDATA(help,ret)=data;
    break;
    }
  case tp_string:
  case tp_vect:
  case tp_tvect:
  case tp_matrix:
  case tp_list:
    {    
    help->subtype.type=((SUBTYPE *)data)->type;
    help->subtype.next=((SUBTYPE *)data)->next;
    free(data);
    break;
    }
  default:
    {
    yyfail("%s","wrong type for stackelement");
    break;
    }
  }
DBUG_RETURN(help);
}

/************************************************************************/
/* copies stackelements with all possible structures			*/
/************************************************************************/
void stelm_cpy(STELM *stelm1,STELM *stelm2,long n)
{
long i;

DBUG_ENTER("stelm_cpy");
memcpy(stelm1, stelm2,n*sizeof(STELM));
for (i=0; i<n;i++)				/* for all args do ... */
  {
  switch (stelm2[i].type)
    {
    case tp_returnadr:				/* returnadress */
      {
      if (NULL==(stelm1->dat.ret=malloc(sizeof(RETURNADR))))
	yyfail("%s","no memory for returndata");
      SETSTELMDATA(stelm1,ret)->w=GETSTELMDATA(stelm2,ret)->w;	/* copy STACKs */
      SETSTELMDATA(stelm1,ret)->a=GETSTELMDATA(stelm2,ret)->a;
      SETSTELMDATA(stelm1,ret)->r=GETSTELMDATA(stelm2,ret)->r;
      SETSTELMDATA(stelm1,ret)->t=GETSTELMDATA(stelm2,ret)->t;
      SETSTELMDATA(stelm1,ret)->ord=GETSTELMDATA(stelm2,ret)->ord;      
      SETSTELMDATA(stelm1,ret)->status=GETSTELMDATA(stelm2,ret)->status;
      SETSTELMDATA(stelm1,ret)->branch=GETSTELMDATA(stelm2,ret)->branch;
      break;
      }
    case tp_none:				/* nothing to do */
    case tp_char:
    case tp_int:
    case tp_bool:
    case tp_prim:
    case tp_real:
    case tp_illegal:
    case tp_var:
      break;
    case tp_func:				/* function */
      {
      stelm1[i].dat.desc=stelm2[i].dat.desc;
      break;
      }
    case tp_clos:				/* closure */
      {
      CLOSURE *clos;
      
      if (NULL!=stelm2[i].dat.clos)		/* if there are any data */
	{
	if (NULL==(clos=malloc(sizeof(CLOSURE))))
	  yyfail("%s","no memory for new closure");
	stelm1[i].dat.clos=clos;		/* save pointer of clos in STELM */
	if (NULL==(clos->w=malloc(stelm2[i].dat.clos->i*sizeof(STELM))))
	  yyfail("%s","no memory for new closure");
	stelm_cpy(clos->w,stelm2[i].dat.clos->w,
		  stelm2[i].dat.clos->i); 	/* copy A frame */
	clos->i=stelm2[i].dat.clos->i;
	clos->j=stelm2[i].dat.clos->j;
	clos->k=stelm2[i].dat.clos->k;
	clos->desc=stelm2[i].dat.clos->desc;
	if (NULL!=clos->desc)			/* clos->desc!=NULL => func */
	  {
	  if (0!=stelm2[i].dat.clos->k)
	    {
	    if (NULL==(clos->t=malloc(stelm2[i].dat.clos->k*sizeof(STELM))))
	      yyfail("%s","no memory for new closure");
	    stelm_cpy(clos->t,stelm2[i].dat.clos->t,
		      stelm2[i].dat.clos->k);	/* copy T frame */
	    }
	  else					/* no tilde frame */
	    clos->t=NULL;
	  }
	else					/* prim func */
	  clos->t=NULL;
	}
      break;
      }
    case tp_string:
    case tp_vect:
    case tp_tvect:
    case tp_matrix:
      {
      stelm1[i].subtype.type=stelm2[i].subtype.type;
      stelm1[i].subtype.next=NULL;
      break;
      }
    case tp_list:				/* list */
      {
      SUBTYPE *sb1,*sb2;
      
      stelm1[i].subtype.type=stelm2[i].subtype.type;
      sb2=stelm2[i].subtype.next;
      sb1=&(stelm1[i].subtype);
      while (NULL!=sb2)				/* copy subtype structure */
	{
	if (NULL==(sb1->next=malloc(sizeof(SUBTYPE))))
	  yyfail("%s","no momory for subtype");
	sb1=sb1->next;
	sb1->type=sb2->type;
	sb2=sb2->next;
	}
      sb1->next=NULL;				/* set end of structure */
      break;
      }
    default:
      yyfail("%s","no known STELM type");
    }
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* frees stackelements with all possible substructures			*/
/************************************************************************/
void stelm_free(STELM *stelm)
{
DBUG_ENTER("stelm_free");
switch (GETSTELMTYPE(stelm))
  {
  case tp_none:					/* nothing to do */
  case tp_int:
  case tp_bool:
  case tp_func:
  case tp_prim:
  case tp_illegal:
  case tp_badrecur:
  case tp_real:
  case tp_empty:
  case tp_string:
  case tp_vect:
  case tp_tvect:
  case tp_matrix:
  case tp_char:
  case tp_var:
    break;
  case tp_list:					/* list */
    {
    free_subtype(stelm);			/* free subtype structure */
    break;
    }
  case tp_returnadr:				/* returnadress */
    {
    free(GETSTELMDATA(stelm,ret));
    break;
    }
  case tp_clos:				/* stelm_free of all frame elements */
    {
    CLOSURE *clos;
    long i;
    
    if (NULL!=stelm->dat.clos)			/* if data stored */
      {
      clos=stelm->dat.clos;
      if (NULL!=clos->w)			/* if W frame */
	for (i=0; i<clos->i; i++)		/* free all elements */
	  stelm_free(&(clos->w[i]));
      if (NULL!=clos->t)			/* if T frame */
	for (i=0; i<clos->k; i++)		/* free all elements */
	  stelm_free(&(clos->t[i]));
      free(clos);				/* free closure data */
      }
    break;
    }
  default:
    yyfail("%s","illegal stackelement found");
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* frees stack with all elements 					*/
/************************************************************************/
void st_free(STACK *st)
{
long i;

DBUG_ENTER("st_free");
if (NULL!=st)					/* no stack allocated */
  {
  for (i=0; i<GETTOPOFSTACK(st); i++)		/* for all STACKelements */
    stelm_free(&(st->stack[i]));		/* free STELMs of stack */
  free(st->stack);				/* free structure */
  free(st);
  st=NULL;					/* set STACK to NULL means empty */
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* builds a stack with a copy of the first n elements of st		*/
/* returns new allocated an initialiesed STACK				*/
/************************************************************************/
STACK *st_make(STACK *st, long n)
{
STACK *nst=NULL;
long l;

DBUG_ENTER("st_make");
if (0!=n)						/* no STELMs to copy */
  {
  if (NULL==(nst=malloc(sizeof(STACK))))
    yyfail("%s","no memory for stack creation!");
  l=MIN_STACK_SIZE;					/* set initial STACKsize */
  while (l<=n)						/* calc needed size */
    l*=4;
  if (NULL==(SETSTACKENTRY(nst,stack)=malloc(l*sizeof(STELM))))
    yyfail("%s","no memory for local stack!");
  SETSTACKENTRY(nst,length)=l;				/* store size */
  SETSTACKENTRY(nst,topofstack)=n;		/* store number of elements */	
  stelm_cpy(GETSTACKENTRY(nst,stack),st_read(st,n-1),n); 	/* copy STELMs */
  }
DBUG_RETURN(nst);					/* return new STACK */
}

/************************************************************************/
/* pops elemet top of stack st						*/
/* returns pointer of STELM top of stack st				*/
/************************************************************************/
STELM *st_pop(STACK *st)
{
STELM *stelm;

DBUG_ENTER("st_pop");
SETTOPOFSTACK(st)--;			/* decrement top of stack pointer */
stelm=&(GETSTACKENTRY(st,stack)[GETTOPOFSTACK(st)]);
DBUG_RETURN(stelm);			/* return pointer to element */
}

/************************************************************************/
/* reads stack element number i counted form top of stack st		*/
/* returns pointer of STELM i counted form top of stack st		*/
/************************************************************************/
STELM *st_read(STACK *st, long i)
{
STELM *stelm;

DBUG_ENTER("st_read");
stelm=&(GETSTACKENTRY(st,stack)[GETTOPOFSTACK(st)-i-1]);
DBUG_RETURN(stelm);				/* return pointer to element */
}

/************************************************************************/
/* pushes STELM vt on top of STACK st					*/
/* returns pointer of STACK st (may be changed)				*/
/************************************************************************/
STACK *st_push(STACK *st, STELM *vt)
{
DBUG_ENTER("st_push");
if (NULL==st)						/* empty STACK */
  {
  if (NULL==(st=malloc(sizeof(STACK))))
    yyfail("%s","no memory for new stack!");
  if (NULL==(SETSTACKENTRY(st,stack)=malloc(MIN_STACK_SIZE*sizeof(STELM))))
    yyfail("%s","no memory for new stack!");
  SETSTACKENTRY(st,length)=MIN_STACK_SIZE;		/* set initial size */
  SETTOPOFSTACK(st)=0;
  }
else							/* stack exists */
  {
  if (GETSTACKENTRY(st,length)==GETTOPOFSTACK(st))	/* modify stacklength */
    {
    if (NULL==(SETSTACKENTRY(st,stack)=realloc(GETSTACKENTRY(st,stack),
			    sizeof(STELM)*4*GETSTACKENTRY(st,length))))
      yyfail("%s","no memory for bigger stack!");
    SETSTACKENTRY(st,length)*=4;			/* set new STACKsize */
    }
  }
stelm_cpy(&(GETSTACKENTRY(st,stack)[GETTOPOFSTACK(st)]),vt,1);
						/* write STELM to STACK */
SETTOPOFSTACK(st)++;			/* increment top of stack pointer */
DBUG_RETURN(st);				/* return pointer of (new) STACK */
}

/************************************************************************/
/* converts hexadecimal number placed in a string into integer		*/
/* returns converted int						*/
/************************************************************************/
int atox(const char * s)
{
int n = 0;
extern int toupper();

DBUG_ENTER("atox");
while(*s)				/* for all chars in string do */
  {
  n=n*16+(isdigit(*s) ? *s-'0' : toupper(*s)-'A'+10);
  s++;						/* next char */
  }
DBUG_RETURN(n);					/* return int */
}

/************************************************************************/
/* searches for '9876543210_' in given string search			*/
/* returns pointer of first char befor '9876543210_'			*/
/************************************************************************/
char *searchforfuncname(char *search)
{
DBUG_ENTER("searchforfuncname");
while (('_'!=search[0]) && (('0'>search[0]) || ('9'<search[0])))
  search++;					/* next char */
DBUG_RETURN(search);
}

/************************************************************************/
/* searches for '_' in given string search				*/
/* returns pointer of first char behind '_'				*/
/************************************************************************/
char *searchfor_(char *search)
{
DBUG_ENTER("searchfor_");
while (search[0]!='_')
  search++;					/* goto next char */
search++;					/* goto char behind '_' */
DBUG_RETURN(search);
}

/************************************************************************/
/* checks if TP_TYPES are tp_int or tp_real and returns most general 	*/
/* type of this two TP_TYPES						*/
/************************************************************************/
static TP_TYPES check_int_real(const TP_TYPES tp0,const TP_TYPES tp1)
{
DBUG_ENTER("check_int_real");
if ((tp_empty==tp0) && (tp0==tp1))
  DBUG_RETURN(tp_empty);
if ((((tp_int==tp0) && (tp0==tp1)) || ((tp_int==tp0) && (tp_empty==tp1))) ||
    ((tp_int==tp1) && (tp_empty==tp0)))
  DBUG_RETURN(tp_int);
if ((((tp_int==tp0) || (tp_real==tp0)) || (tp_empty==tp0)) &&
    (((tp_int==tp1) || (tp_real==tp1)) || (tp_empty==tp1)))
  DBUG_RETURN(tp_real);
DBUG_RETURN(tp_none);
}

/************************************************************************/
/* checks if TP_TYPES tp are tp_vect or tp_vect and tp_int or tp_real	*/
/* returns most general of lt and tp					*/
/************************************************************************/
static TP_TYPES check_vect_num(const TP_TYPES tp0,const TP_TYPES lt0,
			       const TP_TYPES tp1,const TP_TYPES lt1)
{
TP_TYPES dt;

DBUG_ENTER("check_vect_num");
if ((tp_vect==tp0) && (tp0==tp1))
  {
  dt=check_int_real(lt0,lt1);
  DBUG_RETURN(dt);
  }  
if (tp_vect==tp0)
  {
  dt=check_int_real(lt0,tp1);
  DBUG_RETURN(dt);
  }
if (tp_vect==tp1)
  {
  dt=check_int_real(lt1,tp0);
  DBUG_RETURN(dt);
  }
DBUG_RETURN(tp_none);
}

/************************************************************************/
/* checks if TP_TYPES tp are tp_vect or tp_vect and tp_bool returns	*/
/* most general of lt and tp						*/
/************************************************************************/
static TP_TYPES check_vect_bool(const TP_TYPES tp0,const TP_TYPES lt0,
			        const TP_TYPES tp1,const TP_TYPES lt1)
{
DBUG_ENTER("check_vect_num");
if ((tp_vect==tp0) && (tp0==tp1))
  {
  if (tp_bool==(lt0==lt1))
    DBUG_RETURN(tp_bool);
  }  
if (tp_vect==tp0)
  {
  if (tp_bool==(lt0==tp1))
    DBUG_RETURN(tp_bool);
  }
if (tp_vect==tp1)
  {
  if ((tp_bool==tp0) && (lt1==tp0))
    DBUG_RETURN(tp_bool);
  }
DBUG_RETURN(tp_none);
}

/************************************************************************/
/* checks if TP_TYPES tp are tp_tvect or tp_tvect and tp_int or tp_real	*/
/* returns most general of lt and tp					*/
/************************************************************************/
static TP_TYPES check_tvect_num(const TP_TYPES tp0,const TP_TYPES lt0,
			        const TP_TYPES tp1,const TP_TYPES lt1)
{
TP_TYPES dt;

DBUG_ENTER("check_tvect_num");
if ((tp_tvect==tp0) && (tp0==tp1))
  {
  dt=check_int_real(lt0,lt1);
  DBUG_RETURN(dt);
  }  
if (tp_tvect==tp0)
  {
  dt=check_int_real(lt0,tp1);
  DBUG_RETURN(dt);
  }
if (tp_tvect==tp1)
  {
  dt=check_int_real(lt1,tp0);
  DBUG_RETURN(dt);
  }
DBUG_RETURN(tp_none);
}

/************************************************************************/
/* checks if TP_TYPES tp are tp_tvect or tp_tvect and tp_bool returns	*/
/* most general of lt and tp						*/
/************************************************************************/
static TP_TYPES check_tvect_bool(const TP_TYPES tp0,const TP_TYPES lt0,
			         const TP_TYPES tp1,const TP_TYPES lt1)
{
DBUG_ENTER("check_vect_num");
if ((tp_tvect==tp0) && (tp0==tp1))
  {
  if ((tp_bool==lt0) && (lt0==lt1))
    DBUG_RETURN(tp_bool);
  }  
if (tp_tvect==tp0)
  {
  if ((tp_bool==tp1) && (lt0==tp1))
    DBUG_RETURN(tp_bool);
  }
if (tp_tvect==tp1)
  {
  if ((tp_bool==tp0) && (lt1==tp0))
    DBUG_RETURN(tp_bool);
  }
DBUG_RETURN(tp_none);
}

/************************************************************************/
/* checks if TP_TYPES tp are tp_matrix or tp_matrix and tp_int or	*/
/* tp_real returns most general of lt and tp				*/
/************************************************************************/
static TP_TYPES check_mat_num(const TP_TYPES tp0,const TP_TYPES lt0,
		              const TP_TYPES tp1,const TP_TYPES lt1)
{
TP_TYPES dt;

DBUG_ENTER("check_mat_num");
if ((tp_matrix==tp0) && (tp0==tp1))
  {
  dt=check_int_real(lt0,lt1);
  DBUG_RETURN(dt);
  }  
if (tp_matrix==tp0)
  {
  dt=check_int_real(lt0,tp1);
  DBUG_RETURN(dt);
  }
if (tp_matrix==tp1)
  {
  dt=check_int_real(lt1,tp0);
  DBUG_RETURN(dt);
  }
DBUG_RETURN(tp_none);
}

/************************************************************************/
/* checks if TP_TYPES tp are tp_matrix or tp_matrix and tp_bool returns	*/
/* most general of lt and tp						*/
/************************************************************************/
static TP_TYPES check_mat_bool(const TP_TYPES tp0,const TP_TYPES lt0,
		               const TP_TYPES tp1,const TP_TYPES lt1)
{
DBUG_ENTER("check_mat_num");
if ((tp_matrix==tp0) && (tp0==tp1))
  {
  if ((tp_bool==lt0) && (lt0==lt1))
    DBUG_RETURN(tp_bool);
  }  
if (tp_matrix==tp0)
  {
  if ((tp_bool==tp1) && (lt0==tp1))
    DBUG_RETURN(tp_bool);
  }
if (tp_matrix==tp1)
  {
  if ((tp_bool==tp0) && (tp0==lt1))
    DBUG_RETURN(tp_bool);
  }
DBUG_RETURN(tp_none);
}

/************************************************************************/
/* executes primitiv function primf					*/
/************************************************************************/
void exe_prim1(PRIMF primf)
{
STELM d_stelm,*tp0;
TP_TYPES t=tp_none;

DBUG_ENTER("exe_prim1");
tp0=st_pop(w);						/* get parameter */
if (GETORDERENTRY(ord,typestouched)==0)
  {
  if (tp_list==GETSTELMTYPE(tp0))
    t=GETSTELMTYPE(tp0);
  else
    t=GETSTELMTYPE(tp0) | tp0->subtype.type;
  SETORDERENTRY(ord,types)=TYPES2LONG(t,tp_none,tp_none,tp_none);
  SETORDERENTRY(ord,typestouched)=1;
  }
else
  {
  if (tp_list==GETTYPE(0,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp0)!=GETTYPE(0,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp0)|tp0->subtype.type)
        !=GETTYPE(0,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
    }
  }
stelm_cpy(&d_stelm,&CONST_NONE,1);
switch (GETPRF(primf))
  {
  case p_not:
    {
    if (tp_bool==GETSTELMTYPE(tp0))
      d_stelm.type=tp_bool;
    else
      {
      if (((tp_vect==GETSTELMTYPE(tp0)) || (tp_matrix==GETSTELMTYPE(tp0)) ||
	   (tp_tvect==GETSTELMTYPE(tp0)))
	  && ((tp_bool==tp0->subtype.type) || (tp_empty==tp0->subtype.type)))
	stelm_cpy(&d_stelm,tp0,1);
      else
        d_stelm.type=tp_none;
      }
    break;
    }
  case p_floor:
  case p_ceil:
  case p_trunc:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0)))
      {
      d_stelm.type=tp_int;
      d_stelm.subtype.type=tp_none;
      d_stelm.subtype.next=NULL;
      }
    else
      {
      if ((((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0))) ||
	  (tp_matrix==GETSTELMTYPE(tp0))) &&
	 ((tp_int==tp0->subtype.type) || (tp_real==tp0->subtype.type) ||
	  (tp_empty==tp0->subtype.type)))
        {
        d_stelm.type=GETSTELMTYPE(tp0);
        d_stelm.subtype.type=tp_int;
        d_stelm.subtype.next=NULL;
        }
      else
        d_stelm.type=tp_none;
      }
    break;
    }
  case p_frac:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) ||
	((((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0))) ||
	  (tp_matrix==GETSTELMTYPE(tp0))) &&
	 (((tp_int==tp0->subtype.type) || (tp_real==tp0->subtype.type) ||
	   (tp_empty==tp0->subtype.type)))))
      {
      d_stelm.type=GETSTELMTYPE(tp0);
      d_stelm.subtype.type=tp0->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_vdim:
    {
    if ((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0)))
      d_stelm.type=tp_int;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_transpose:
    {
    if (((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0))) ||
	(tp_matrix==GETSTELMTYPE(tp0)))
      {
      if (tp_vect==GETSTELMTYPE(tp0))
	d_stelm.type=tp_tvect;
      else
	{
	if (tp_tvect==GETSTELMTYPE(tp0))
	  d_stelm.type=tp_vect;
	else
	  d_stelm.type=tp_matrix;
	}
      d_stelm.subtype.type=tp0->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_reverse:
    {
    if ((tp_list==GETSTELMTYPE(tp0)) || (tp_string==GETSTELMTYPE(tp0)))
      stelm_cpy(&d_stelm,tp0,1);
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_transform:
    {
    if (((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0))) ||
	(tp_matrix==GETSTELMTYPE(tp0)))
      {
      d_stelm.type=tp_list;
      d_stelm.subtype.type=tp0->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }    
  case p_to_vect:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) ||
	(tp_bool==GETSTELMTYPE(tp0)))
      {
      d_stelm.type=tp_vect;
      d_stelm.subtype.type=tp0->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_to_tvect:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) ||
	(tp_bool==GETSTELMTYPE(tp0)))
      {
      d_stelm.type=tp_tvect;
      d_stelm.subtype.type=tp0->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_to_mat:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) ||
	(tp_bool==GETSTELMTYPE(tp0)))
      {
      d_stelm.type=tp_matrix;
      d_stelm.subtype.type=GETSTELMTYPE(tp0);
      d_stelm.subtype.next=NULL;
      }
    else
      {
      if ((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0)))
	{
	d_stelm.type=tp_matrix;
	d_stelm.subtype.type=tp0->subtype.type;
	d_stelm.subtype.next=NULL;
	}
      else
        d_stelm.type=tp_none;
      }
    break;
    }
  case p_to_scal:
    {
    d_stelm.type=tp_none;
    break;
    }
  case p_to_list:
    {
    if ((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0)))
      {
      d_stelm.type=tp_list;
      d_stelm.subtype.type=tp0->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      {
      if (tp_matrix==GETSTELMTYPE(tp0))
	{
	d_stelm.type=tp_list;
	if (tp_empty==tp0->subtype.type)
	  {
	  d_stelm.subtype.type=tp_empty;
	  d_stelm.subtype.next=NULL;
	  }
	else
	  {
	  d_stelm.subtype.type=tp_list;
	  if (NULL==(d_stelm.subtype.next=malloc(sizeof(SUBTYPE))))
	    yyfail("%s","no memory for subtype!");
	  d_stelm.subtype.next->type=tp0->subtype.type;
	  d_stelm.subtype.next->next=NULL;
	  }
	}
      else
        d_stelm.type=tp_none;
      }
    break;
    }
  case p_type:
  case p_class:
    {
    if ((tp_func==GETSTELMTYPE(tp0)) || (tp_clos==GETSTELMTYPE(tp0)))
      d_stelm.type=tp_none;
    else
      d_stelm.type=tp_atom;
    break;
    }
  case p_to_field:
    {
    if ((tp_list==GETSTELMTYPE(tp0)) && (((tp_bool==tp0->subtype.type) ||
	(tp_int==tp0->subtype.type)) || (tp_real==tp0->subtype.type)))
      {
      d_stelm.type=tp_vect;
      d_stelm.subtype.type=tp0->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_vc_max:
  case p_vc_min:
    {
    if (((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0))) &&
        (((tp_int==tp0->subtype.type) || (tp_real==tp0->subtype.type)) ||
	 (tp_bool==tp0->subtype.type) || (tp_empty==tp0->subtype.type)))
      {
      if (tp_empty==tp0->subtype.type)
	d_stelm.type=tp_int;
      else
        d_stelm.type=tp0->subtype.type;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_vc_plus:
  case p_vc_minus:
  case p_vc_mult:
    {
    if (((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0))) &&
        ((tp_int==tp0->subtype.type) || (tp_real==tp0->subtype.type) ||
	 (tp_empty==tp0->subtype.type)))
      {
      if (tp_empty==tp0->subtype.type)
	d_stelm.type=tp_int;
      else
        d_stelm.type=tp0->subtype.type;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_vc_div:
    {
    if (((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0))) &&
        ((tp_int==tp0->subtype.type) || (tp_real==tp0->subtype.type) ||
	 (tp_empty==tp0->subtype.type)))
      {
      if (tp_empty==tp0->subtype.type)
	d_stelm.type=tp_int;
      else
        d_stelm.type=tp_real;
      }
    else
      d_stelm.type=tp_none;
    break;
    }    
  case p_empty:
    {
    if ((tp_list==GETSTELMTYPE(tp0)) || (tp_string==GETSTELMTYPE(tp0)))
      d_stelm.type=tp_bool;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_tan:
  case p_ln:
  case p_exp:
  case p_sin:
  case p_cos:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0)))
      d_stelm.type=tp_real;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_abs:
  case p_neg:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0)))
      d_stelm.type=GETSTELMTYPE(tp0);
    else
      {
      if (((tp_vect==GETSTELMTYPE(tp0)) || (tp_tvect==GETSTELMTYPE(tp0)) ||
           (tp_matrix==GETSTELMTYPE(tp0))) &&
          ((tp_real==tp0->subtype.type) || (tp_int==tp0->subtype.type) ||
	   (tp_empty==tp0->subtype.type)))
        {
        d_stelm.type=GETSTELMTYPE(tp0);
        d_stelm.subtype.type=tp0->subtype.type;
        d_stelm.subtype.next=NULL;
        }
      else
        d_stelm.type=tp_none;
      }
    break;
    }
  case p_to_char:
    {
    if (tp_int==GETSTELMTYPE(tp0))
      d_stelm.type=tp_string;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_to_ord:
    {
    if (tp_string==GETSTELMTYPE(tp0))
      d_stelm.type=tp_int;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_ldim: 
    {
    if ((tp_list==GETSTELMTYPE(tp0)) || (tp_string==GETSTELMTYPE(tp0)))
      d_stelm.type=tp_int;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_f_slots: /* cr 19.02.96 */
      d_stelm.type=tp_none;
      break;
  default:
    {
    yyfail("%s","that's no known unary primitive function");
    break;
    }
  }
w=st_push(w,&d_stelm);					/* result to W STACK */
insert_restype(&d_stelm,ord);				/* insert hash restype */
DBUG_VOID_RETURN;
}
  
/************************************************************************/
/* executes primitiv function primf					*/
/************************************************************************/
void exe_prim2(PRIMF primf)
{
STELM d_stelm,*tp0,*tp1;
TP_TYPES t0=tp_none,t1=tp_none;

DBUG_ENTER("exe_prim2");
tp0=st_pop(w);
tp1=st_pop(w);
if (GETORDERENTRY(ord,typestouched)==0)
  {
  if (tp_list==GETSTELMTYPE(tp0))
    t0=GETSTELMTYPE(tp0);
  else
    t0=GETSTELMTYPE(tp0) | tp0->subtype.type;
  if (tp_list==GETSTELMTYPE(tp1))
    t1=GETSTELMTYPE(tp1);
  else
    t1=GETSTELMTYPE(tp1) | tp1->subtype.type;
  SETORDERENTRY(ord,types)=TYPES2LONG(t0,t1,tp_none,tp_none);	/* internal types */
  SETORDERENTRY(ord,typestouched)=1;
  }
else
  {
  if (tp_list==GETTYPE(0,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp0)!=GETTYPE(0,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp0)|tp0->subtype.type)
        !=GETTYPE(0,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
    }
  if (tp_list==GETTYPE(1,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp1)!=GETTYPE(1,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(1,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp1)|tp1->subtype.type)
        !=GETTYPE(1,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(1,tp_none);
    }
  }
stelm_cpy(&d_stelm,&CONST_NONE,1);
switch (GETPRF(primf))
  {        
  case p_plus:
  case p_minus:
  case p_mult:
    {
    d_stelm.type=check_int_real(GETSTELMTYPE(tp0),GETSTELMTYPE(tp1));
    if (tp_none==d_stelm.type)
      {
      d_stelm.type=check_vect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				  GETSTELMTYPE(tp1),tp1->subtype.type);
      if (tp_none!=d_stelm.type)
	{
	if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	  {
	  d_stelm.subtype.type=d_stelm.type;
	  d_stelm.type=tp_vect;
	  d_stelm.subtype.next=NULL;
	  }
	else
	  d_stelm.type=tp_none;
	}
      else
	{
        d_stelm.type=check_tvect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				     GETSTELMTYPE(tp1),tp1->subtype.type);
        if (tp_none!=d_stelm.type)
	  {
	  if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	    {
	    d_stelm.subtype.type=d_stelm.type;
	    d_stelm.subtype.next=NULL;
	    d_stelm.type=tp_tvect;
	    }
	  else
	    d_stelm.type=tp_none;
	  }
	else
	  {
	  d_stelm.type=check_mat_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				     GETSTELMTYPE(tp1),tp1->subtype.type);
	  if (tp_none!=d_stelm.type)
	    {
	    if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	      {
	      d_stelm.subtype.type=d_stelm.type;
	      d_stelm.subtype.next=NULL;
	      d_stelm.type=tp_matrix;
	      }
	    else
	      d_stelm.type=tp_none;
	    }
	  }
	}
      }
    break;
    }
  case p_div:
    {
    d_stelm.type=check_int_real(GETSTELMTYPE(tp0),GETSTELMTYPE(tp1));
    if (tp_none!=d_stelm.type)
      d_stelm.type=tp_real;
    else
      {
      d_stelm.type=check_vect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				  GETSTELMTYPE(tp1),tp1->subtype.type);
      if (tp_none!=d_stelm.type)
	{
	if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	  {
	  d_stelm.subtype.type=tp_real;
	  d_stelm.subtype.next=NULL;
	  d_stelm.type=tp_vect;
	  }
	else
	  d_stelm.type=tp_none;
	}
      else
	{
        d_stelm.type=check_tvect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				     GETSTELMTYPE(tp1),tp1->subtype.type);
        if (tp_none!=d_stelm.type)
	  {
	  if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	    {
	    d_stelm.subtype.type=tp_real;
	    d_stelm.subtype.next=NULL;
	    d_stelm.type=tp_tvect;
	    }
	  else
	    d_stelm.type=tp_none;
	  }
	else
	  {
	  d_stelm.type=check_mat_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				     GETSTELMTYPE(tp1),tp1->subtype.type);
	  if (tp_none!=d_stelm.type)
	    {
	    if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	      {
	      d_stelm.subtype.type=tp_real;
	      d_stelm.subtype.next=NULL;
	      d_stelm.type=tp_matrix;
	      }
	    else
	      d_stelm.type=tp_none;
	    }
	  }
	}
      }
    break;
    }
  case p_mod:
    {
    d_stelm.type=check_int_real(GETSTELMTYPE(tp0),GETSTELMTYPE(tp1));
    if (tp_none==d_stelm.type)
      {
      d_stelm.type=check_vect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				  GETSTELMTYPE(tp1),tp1->subtype.type);
      if (tp_none!=d_stelm.type)
	{
	if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	  {
	  d_stelm.subtype.type=tp_int;
	  d_stelm.subtype.next=NULL;
	  d_stelm.type=tp_vect;
	  }
	else
	  d_stelm.type=tp_none;
	}
      else
	{
        d_stelm.type=check_tvect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				     GETSTELMTYPE(tp1),tp1->subtype.type);
        if (tp_none!=d_stelm.type)
	  {
	  if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	    {
	    d_stelm.subtype.type=tp_int;
	    d_stelm.subtype.next=NULL;
	    d_stelm.type=tp_tvect;
	    }
	  else
	    d_stelm.type=tp_none;
	  }
	else
	  {
	  d_stelm.type=check_mat_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				     GETSTELMTYPE(tp1),tp1->subtype.type);
	  if (tp_none!=d_stelm.type)
	    {
	    if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	      {
	      d_stelm.subtype.type=tp_int;
	      d_stelm.subtype.next=NULL;
	      d_stelm.type=tp_matrix;
	      }
	    else
	      d_stelm.type=tp_none;
	    }
	  }
	}
      }
    break;
    }
  case p_or:
  case p_and:
  case p_xor:
    {
    if ((tp_bool==GETSTELMTYPE(tp0)) && (GETSTELMTYPE(tp0)==GETSTELMTYPE(tp1)))
      d_stelm.type=tp_bool;
    else
      {
      d_stelm.type=check_vect_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
				   GETSTELMTYPE(tp1),tp1->subtype.type);
      if (tp_none!=d_stelm.type)
	{
	if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	  {
	  d_stelm.type=tp_vect;
	  d_stelm.subtype.type=tp_bool;
	  d_stelm.subtype.next=NULL;
	  }
	else
	  d_stelm.type=tp_none;
	}
      else
	{
	d_stelm.type=check_tvect_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
				      GETSTELMTYPE(tp1),tp1->subtype.type);
        if (tp_none!=d_stelm.type)
	  {
	  if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	    {
	    d_stelm.type=tp_tvect;
	    d_stelm.subtype.type=tp_bool;
	    d_stelm.subtype.next=NULL;
	    }
	  else
	    d_stelm.type=tp_none;
	  }
        else
	  {
	  d_stelm.type=check_mat_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
				      GETSTELMTYPE(tp1),tp1->subtype.type);
	  if (tp_none!=d_stelm.type)
	    {
	    if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	      {
	      d_stelm.type=tp_matrix;
	      d_stelm.subtype.type=tp_bool;
	      d_stelm.subtype.next=NULL;
	      }
	    else
	      d_stelm.type=tp_none;
	    }
	  }
	}
      }
    break;
    }
  case p_eq:
  case p_ne:
    {
    if ((tp_clos==GETSTELMTYPE(tp0)) || (tp_clos==GETSTELMTYPE(tp1)) ||
	(tp_none==GETSTELMTYPE(tp0)) || (tp_none==GETSTELMTYPE(tp1)) ||
	((tp_list==GETSTELMTYPE(tp0)) && (tp_list==GETSTELMTYPE(tp1))))
      d_stelm.type=tp_none;
    else
      d_stelm.type=tp_bool;
    break;
    }
  case p_lt:
  case p_le:
  case p_gt:
  case p_ge:
    {
    d_stelm.type=check_int_real(GETSTELMTYPE(tp0),GETSTELMTYPE(tp1));
    if (tp_none!=d_stelm.type)
      d_stelm.type=tp_bool;
    else
      {
      if ((GETSTELMTYPE(tp0)==GETSTELMTYPE(tp1)) &&
	  ((tp_bool==GETSTELMTYPE(tp0)) ||
	    (tp_string==GETSTELMTYPE(tp0))))
	d_stelm.type=tp_bool;
      else
	{
	d_stelm.type=check_vect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				    GETSTELMTYPE(tp1),tp1->subtype.type);
        if (tp_none!=d_stelm.type)
	  {
	  if ((tp_empty==tp0->subtype.type) || (tp_empty==tp1->subtype.type))
	    d_stelm.type=tp_none;
	  else
	    {
	    d_stelm.subtype.type=tp_bool;
	    d_stelm.subtype.next=NULL;
	    d_stelm.type=tp_vect;
	    }
	  }
        else
	  {
	  d_stelm.type=check_tvect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				       GETSTELMTYPE(tp1),tp1->subtype.type);
          if (tp_none!=d_stelm.type)
	    {
	    if ((tp_empty==tp0->subtype.type) || (tp_empty==tp1->subtype.type))
	      d_stelm.type=tp_none;
	    else
	      {
	      d_stelm.subtype.type=tp_bool;
	      d_stelm.subtype.next=NULL;
	      d_stelm.type=tp_tvect;
	      }
	    }
	  else
	    {
	    d_stelm.type=check_mat_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				       GETSTELMTYPE(tp1),tp1->subtype.type);
	    if (tp_none!=d_stelm.type)
	      {
	      if ((tp_empty==tp0->subtype.type) || (tp_empty==tp1->subtype.type))
	        d_stelm.type=tp_none;
	      else
	        {
	        d_stelm.subtype.type=tp_bool;
	        d_stelm.subtype.next=NULL;
	        d_stelm.type=tp_matrix;
		}
	      }
	    else
	      {
	      d_stelm.type=check_vect_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
					   GETSTELMTYPE(tp1),tp1->subtype.type);
	      if (tp_none!=d_stelm.type)
		{
		if ((tp_empty==tp0->subtype.type) || (tp_empty==tp1->subtype.type))
	          d_stelm.type=tp_none;
	        else
	          {
	          d_stelm.type=tp_vect;
		  d_stelm.subtype.type=tp_bool;
		  d_stelm.subtype.next=NULL;
		  }
		}
	      else
		{
		d_stelm.type=check_tvect_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
					      GETSTELMTYPE(tp1),tp1->subtype.type);
		if (tp_none!=d_stelm.type)
		  {
		  if ((tp_empty==tp0->subtype.type) || (tp_empty==tp1->subtype.type))
	            d_stelm.type=tp_none;
	          else
	            {
	            d_stelm.type=tp_tvect;
		    d_stelm.subtype.type=tp_bool;
		    d_stelm.subtype.next=NULL;
		    }
		  }
		else
		  {
		  d_stelm.type=check_mat_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
					      GETSTELMTYPE(tp1),tp1->subtype.type);
		  if (tp_none!=d_stelm.type)
		    {
		    if ((tp_empty==tp0->subtype.type) || (tp_empty==tp1->subtype.type))
	              d_stelm.type=tp_none;
	            else
	              {
	              d_stelm.type=tp_matrix;
		      d_stelm.subtype.type=tp_bool;
		      d_stelm.subtype.next=NULL;
		      }
		    }
		  }
		}
	      }
	    }
     	  }
	}
      }
    break;
    }
  case p_lcut:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	(tp_list==GETSTELMTYPE(tp1)))
      {
      stelm_cpy(&d_stelm,tp1,1);
      if (tp_empty==tp1->subtype.type)
        d_stelm.type=tp_none;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_lselect:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	(tp_list==GETSTELMTYPE(tp1)))
      {
      if (tp_empty!=tp1->subtype.type)
	d_stelm.type=tp1->subtype.type;
      else
	d_stelm.type=tp_none;
      if ((((tp_list==d_stelm.type) || (tp_vect==d_stelm.type)) ||
	   ((tp_tvect==d_stelm.type) || (tp_matrix==d_stelm.type))) ||
	  (tp_string==d_stelm.type))
	{
	d_stelm.subtype.type=tp1->subtype.next->type;
	if (NULL!=tp1->subtype.next->next)
	  d_stelm.subtype.next=subtype_cpy(tp1->subtype.next->next);
	else
	  d_stelm.subtype.next=NULL;
	}
      }
    else
      {
      if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	  (tp_string==GETSTELMTYPE(tp1)))
	{
	d_stelm.type=tp_string;
	d_stelm.subtype.type=tp1->subtype.type;
	}
      else
        d_stelm.type=tp_none;
      }
    break;
    }
  case p_lunite:				/* fix subtype of empty-list args */
    {
    if ((tp_list==GETSTELMTYPE(tp0)) && (tp_list==GETSTELMTYPE(tp1)))
      {
      stelm_cpy(&d_stelm,tp0,1);
      if ((tp_empty!=tp0->subtype.type) &&
	  (tp_empty!=tp1->subtype.type))
	subtypecmp(&(d_stelm.subtype),&(tp1->subtype));
      else
	{
        if (tp_empty==tp0->subtype.type)
	  stelm_cpy(&d_stelm,tp1,1);
	else
          stelm_cpy(&d_stelm,tp0,1);
	}
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_min:
  case p_max:
    {
    if ((GETSTELMTYPE(tp0)==GETSTELMTYPE(tp1)) &&
	((((tp_int==GETSTELMTYPE(tp0)) ||
	(tp_real==GETSTELMTYPE(tp0))) ||
 	( /* (tp_string==GETSTELMTYPE(tp0)) || */
	(tp_bool==GETSTELMTYPE(tp0)))) ||
	(((tp_vect==GETSTELMTYPE(tp0)) ||
	(tp_tvect==GETSTELMTYPE(tp0))) ||
	(tp_matrix==GETSTELMTYPE(tp0)))))
      {
      if ((tp_empty!=tp0->subtype.type) && (tp_empty!=tp1->subtype.type))
	{
	d_stelm.type=GETSTELMTYPE(tp0);
        d_stelm.subtype.type=tp0->subtype.type;
        d_stelm.subtype.next=NULL;
	}
      else
	d_stelm.type=tp_none;
      }
    else
      {
      d_stelm.type=check_vect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				  GETSTELMTYPE(tp1),tp1->subtype.type);
      if (tp_none!=d_stelm.type)
	{
	d_stelm.type=tp_vect;
	if ((tp_vect==GETSTELMTYPE(tp0)) && 
	    (GETSTELMTYPE(tp1)==tp0->subtype.type))
	  {
	  d_stelm.subtype.type=GETSTELMTYPE(tp1);
	  d_stelm.subtype.next=NULL;
	  }
	else
	  {
	  d_stelm.subtype.next=NULL;
	  if ((tp_vect==GETSTELMTYPE(tp1)) && 
	      (GETSTELMTYPE(tp0)==tp1->subtype.type))
	    d_stelm.subtype.type=GETSTELMTYPE(tp0);
	  else
	    d_stelm.subtype.type=tp_none;
	  }
	}
      else
	{
	d_stelm.type=check_tvect_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				     GETSTELMTYPE(tp1),tp1->subtype.type);
        if (tp_none!=d_stelm.type)
	  {
	  d_stelm.type=tp_tvect;
	  if ((tp_tvect==GETSTELMTYPE(tp0)) && 
	      (GETSTELMTYPE(tp1)==tp0->subtype.type))
	    {
	    d_stelm.subtype.type=GETSTELMTYPE(tp1);
	    d_stelm.subtype.next=NULL;
	    }
	  else
	    {
	    d_stelm.subtype.next=NULL;
	    if ((tp_tvect==GETSTELMTYPE(tp1)) && 
		(GETSTELMTYPE(tp0)==tp1->subtype.type))
	      d_stelm.subtype.type=GETSTELMTYPE(tp0);
	    else
	      d_stelm.subtype.type=tp_none;
	    }
	  }
        else
	  {
	  d_stelm.type=check_vect_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
				       GETSTELMTYPE(tp1),tp1->subtype.type);
          if (tp_none!=d_stelm.type)
	    {
	    d_stelm.subtype.type=tp_bool;
	    d_stelm.subtype.next=NULL;
	    d_stelm.type=tp_vect;
	    }
	  else
	    {
	    d_stelm.type=check_tvect_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
				          GETSTELMTYPE(tp1),tp1->subtype.type);
	    if (tp_none!=d_stelm.type)
	      {
	      d_stelm.subtype.type=tp_bool;
	      d_stelm.subtype.next=NULL;
	      d_stelm.type=tp_tvect;
	      }
	    else
	      {
	      d_stelm.type=check_mat_num(GETSTELMTYPE(tp0),tp0->subtype.type,
				         GETSTELMTYPE(tp1),tp1->subtype.type);
	      if (tp_none!=d_stelm.type)
		{
		d_stelm.type=tp_matrix;
		d_stelm.subtype.next=NULL;
		if ((tp_matrix==GETSTELMTYPE(tp0)) &&
		   (tp0->subtype.type==GETSTELMTYPE(tp1)))
		  d_stelm.subtype.type=tp0->subtype.type;
		else
		  {
		  if ((tp_matrix==GETSTELMTYPE(tp1)) &&
		     (tp1->subtype.type==GETSTELMTYPE(tp0)))
		    d_stelm.subtype.type=tp1->subtype.type;
		  else
		    d_stelm.subtype.type=tp_none;
		  }
		}
	      else
		{
		d_stelm.type=check_mat_bool(GETSTELMTYPE(tp0),tp0->subtype.type,
				            GETSTELMTYPE(tp1),tp1->subtype.type);
		if (tp_none!=d_stelm.type)
		  {
		  d_stelm.type=tp_matrix;
		  d_stelm.subtype.type=tp_bool;
		  d_stelm.subtype.next=NULL;
		  }
		}
	      }
	    }
	  }
	}
      }
    break;
    }
  case p_quot:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) && (GETSTELMTYPE(tp0)==GETSTELMTYPE(tp1)))
      d_stelm.type=tp_int;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_ip:
    {
    if ((tp_matrix==GETSTELMTYPE(tp0)) || (tp_empty!=tp0->subtype.type))
      {
      if ((tp_matrix==GETSTELMTYPE(tp1)) || (tp_empty!=tp1->subtype.type))
	{
	d_stelm.type=tp_matrix;
	d_stelm.subtype.type=check_int_real(tp0->subtype.type,tp1->subtype.type);
	d_stelm.subtype.next=NULL;
	}
      else
	{
	if ((tp_tvect==GETSTELMTYPE(tp1))  || (tp_empty!=tp1->subtype.type))
	  {
	  d_stelm.type=tp_tvect;
	  d_stelm.subtype.type=check_int_real(tp0->subtype.type,tp1->subtype.type);
	  d_stelm.subtype.next=NULL;
	  }
	else
	  d_stelm.type=tp_none;
	}
      }
    else
      {
      if ((tp_vect==GETSTELMTYPE(tp0)) || (tp_empty!=tp0->subtype.type))
	{
	if ((tp_tvect==GETSTELMTYPE(tp1)) || (tp_empty!=tp1->subtype.type))
	  d_stelm.type=check_int_real(tp0->subtype.type,tp1->subtype.type);
	else
	  {
	  if ((tp_matrix==GETSTELMTYPE(tp1)) || (tp_empty!=tp1->subtype.type))
	    {
	    d_stelm.type=tp_vect;
	    d_stelm.subtype.type=check_int_real(tp0->subtype.type,tp1->subtype.type);
	    d_stelm.subtype.next=NULL;
	    }
	  else
	    d_stelm.type=tp_none;
	  }
	}
      else
	{
	if ((tp_tvect==GETSTELMTYPE(tp0)) || (tp_empty!=tp0->subtype.type))
	  {
	  if ((tp_vect==GETSTELMTYPE(tp1)) || (tp_empty!=tp1->subtype.type))
	    {
	    d_stelm.type=tp_matrix;
	    d_stelm.subtype.type=check_int_real(tp0->subtype.type,tp1->subtype.type);
	    d_stelm.subtype.next=NULL;
	    }
	  else
	    d_stelm.type=tp_none;
	  }
	else
	  d_stelm.type=tp_none;
	}
      }
    break;
    }
  case p_dim:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) &&
	((((tp_list==GETSTELMTYPE(tp1)) || (tp_string==GETSTELMTYPE(tp1))) ||
	  ((tp_vect==GETSTELMTYPE(tp1)) || (tp_tvect==GETSTELMTYPE(tp1)))) ||
	(tp_matrix==GETSTELMTYPE(tp1))))
      d_stelm.type=tp_int;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_mdim:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) && (tp_matrix==GETSTELMTYPE(tp1)))
      d_stelm.type=tp_int;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_vrotate:
  case p_vcut:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) && ((tp_vect==GETSTELMTYPE(tp1)) ||
					(tp_tvect==GETSTELMTYPE(tp1))))
      {
      d_stelm.type=GETSTELMTYPE(tp1);
      d_stelm.subtype.type=tp1->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_vselect:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) && ((tp_vect==GETSTELMTYPE(tp1)) ||
					(tp_tvect==GETSTELMTYPE(tp1))) &&
	(tp_empty!=tp1->subtype.type))				/* no empty vect */
      d_stelm.type=tp1->subtype.type;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_lrotate:
    {
    if ((tp_int==GETSTELMTYPE(tp0)) && (tp_list==GETSTELMTYPE(tp1)))
      stelm_cpy(&d_stelm,tp1,1);
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_vunite:
    {
    if (((tp_vect==GETSTELMTYPE(tp0)) && (GETSTELMTYPE(tp0)==GETSTELMTYPE(tp1))) ||
	((tp_tvect==GETSTELMTYPE(tp0)) && (GETSTELMTYPE(tp0)==GETSTELMTYPE(tp1))))
      {
      stelm_cpy(&d_stelm,tp1,1);
      if (tp_empty==tp1->subtype.type)
	d_stelm.subtype.type=tp0->subtype.type;
      else
	{
	if (tp_empty!=tp0->subtype.type)
          subtypecmp(&(d_stelm.subtype),&(tp0->subtype));
	}
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_c_max:
  case p_c_min:
  case p_c_plus:
  case p_c_minus:
  case p_c_mult:
  case p_c_div: 
    {
    d_stelm.type=tp_none;				/* that's bad !!! */
    break;
    }
  case p_f_eq:
  case p_f_ne:
    {
    if (((GETSTELMTYPE(tp0)==GETSTELMTYPE(tp1)) &&
	(tp0->subtype.type==tp1->subtype.type)) &&
	(((tp_matrix==GETSTELMTYPE(tp0)) || (tp_vect==GETSTELMTYPE(tp0))) ||
	 (tp_tvect==GETSTELMTYPE(tp0))))
      {
      d_stelm.type=GETSTELMTYPE(tp0);
      d_stelm.subtype.type=tp_bool;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_f_delete: /* cr 19.02.96 */
      d_stelm.type=tp_none;
      break;
  case p_f_test: /* cr 19.02.96 */
      d_stelm.type=tp_none;
      break;
  case p_f_select: /* cr 19.02.96 */
      d_stelm.type=tp_none;
      break;
  case p_sprintf:
    {
    if ((tp_string==GETSTELMTYPE(tp0)) &&
        ((tp_int==GETSTELMTYPE(tp1)) || (tp_real==GETSTELMTYPE(tp1))) )
      d_stelm.type=tp_string;
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_sscanf:
    {
    d_stelm.type=tp_none;
    break;
    }
  default:
    {
    yyfail("%s","that's no known binary primitive function");
    break;
    }
  }
w=st_push(w,&d_stelm);					/* result to W STACK */
insert_restype(&d_stelm,ord);				/* insert hash restype */
DBUG_VOID_RETURN;
}

/************************************************************************/
/* executes primitiv function primf					*/
/************************************************************************/
void exe_prim3(PRIMF primf)
{
STELM d_stelm,*tp0,*tp1,*tp2;
TP_TYPES t0=tp_none,t1=tp_none,t2=tp_none;

DBUG_ENTER("exe_prim3");
tp0=st_pop(w);
tp1=st_pop(w);
tp2=st_pop(w);
if (GETORDERENTRY(ord,typestouched)==0)
  {
  if (tp_list==GETSTELMTYPE(tp0))
    t0=GETSTELMTYPE(tp0);
  else
    t0=GETSTELMTYPE(tp0) | tp0->subtype.type;
  if (tp_list==GETSTELMTYPE(tp1))
    t1=GETSTELMTYPE(tp1);
  else
    t1=GETSTELMTYPE(tp1) | tp1->subtype.type;
  if (tp_list==GETSTELMTYPE(tp2))
    t2=GETSTELMTYPE(tp2);
  else
    t2=GETSTELMTYPE(tp2) | tp2->subtype.type;
  SETORDERENTRY(ord,types)=TYPES2LONG(t0,t1,t2,tp_none);	/* internal types */
  SETORDERENTRY(ord,typestouched)=1;
  }
else
  {
  if (tp_list==GETTYPE(0,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp0)!=GETTYPE(0,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp0)|tp0->subtype.type)
        !=GETTYPE(0,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
    }
  if (tp_list==GETTYPE(1,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp1)!=GETTYPE(1,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(1,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp1)|tp1->subtype.type)
        !=GETTYPE(1,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(1,tp_none);
    }
  if (tp_list==GETTYPE(2,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp2)!=GETTYPE(2,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(2,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp2)|tp1->subtype.type)
        !=GETTYPE(2,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(2,tp_none);
    }
  }
stelm_cpy(&d_stelm,&CONST_NONE,1);
switch (GETPRF(primf))
  {
  case p_lreplace:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0)))
	&& (tp_list==GETSTELMTYPE(tp2)))
      {
      if (tp_empty==tp2->subtype.type)
        d_stelm.type=tp_none;
      if (GETSTELMTYPE(tp1)==tp2->subtype.type)
	stelm_cpy(&d_stelm,tp2,1);
      else
	{
	d_stelm.type=tp_list;
	d_stelm.subtype.type=tp_none;
	d_stelm.subtype.next=NULL;
	}
      }
    else
      {
      if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0)))
	  && (tp_string==GETSTELMTYPE(tp2)) && (tp_string==GETSTELMTYPE(tp1)))
	{
	if (tp1->subtype.type==tp2->subtype.type)
	  stelm_cpy(&d_stelm,tp2,1);
        else
	  {
	  d_stelm.type=tp_string;
	  d_stelm.subtype.type=tp_none;
	  d_stelm.subtype.next=NULL;
	  }
	}
      else
	d_stelm.type=tp_none;
      }
    break;
    }
  case p_rotate:
  case p_cut:
  case p_substr:
    {
    d_stelm.type=tp_none;
    break;
    }
  case p_mrotate: 
  case p_mcut:
    {
    if ((((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	 ((tp_int==GETSTELMTYPE(tp1)) || (tp_real==GETSTELMTYPE(tp1)))) &&
	(tp_matrix==GETSTELMTYPE(tp2)))
      stelm_cpy(&d_stelm,tp2,1);
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_ltransform:
    {    
    if (((((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	  ((tp_int==GETSTELMTYPE(tp1)) || (tp_real==GETSTELMTYPE(tp1)))) &&
	 (tp_list==GETSTELMTYPE(tp2))) &&
	(((tp_vect!=tp2->subtype.type) && (tp_tvect!=tp2->subtype.type)) &&
	 ((tp_matrix!=tp2->subtype.type) && (tp_none!=tp2->subtype.type) &&
	  (tp_empty!=tp2->subtype.type))))
      {
      d_stelm.type=tp_matrix;
      d_stelm.subtype.type=tp2->subtype.type;
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_select: 
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	  ((tp_int==GETSTELMTYPE(tp1)) || (tp_real==GETSTELMTYPE(tp1))))
      {
      if (((tp_list==GETSTELMTYPE(tp2)) || (tp_vect==GETSTELMTYPE(tp2))) ||
	  (tp_tvect==GETSTELMTYPE(tp2)))
	{
	if (tp_empty==tp2->subtype.type)
	  d_stelm.type=tp_none;
        else
          {
	  d_stelm.type=GETSTELMTYPE(tp2);
	  d_stelm.subtype.type=tp2->subtype.type;
	  d_stelm.subtype.next=NULL;
          }
	}
      else
	d_stelm.type=tp_none;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_vreplace:
    {
    if ((((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	 ((tp_vect==GETSTELMTYPE(tp2)) || (tp_tvect==GETSTELMTYPE(tp2)))) &&
	(tp2->subtype.type==GETSTELMTYPE(tp1)))
      stelm_cpy(&d_stelm,tp2,1);
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_mselect:
  case p_unite:
  case p_munite:
    {
    d_stelm.type=tp_none;
    break;
    }
  case p_mre_c:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	((tp_tvect==GETSTELMTYPE(tp1)) && (tp_matrix==GETSTELMTYPE(tp2))))
      {
      d_stelm.type=tp_matrix;
      d_stelm.subtype.type=check_int_real(tp1->subtype.type,tp2->subtype.type);
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  case p_mre_r:
    {
    if (((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	((tp_vect==GETSTELMTYPE(tp1)) && (tp_matrix==GETSTELMTYPE(tp2))))
      {
      d_stelm.type=tp_matrix;
      d_stelm.subtype.type=check_int_real(tp1->subtype.type,tp2->subtype.type);
      d_stelm.subtype.next=NULL;
      }
    else
      d_stelm.type=tp_none;    
    break;
    }
  case p_f_update: /* cr 19.02.96 */
      d_stelm.type=tp_none;
      break;
  default:
    {
    yyfail("%s","that's no known 3-ary primitive function");
    break;
    }
  }
w=st_push(w,&d_stelm);					/* result to W STACK */
insert_restype(&d_stelm,ord);				/* insert hash restype */
DBUG_VOID_RETURN;
}

/************************************************************************/
/* executes primitiv function primf					*/
/************************************************************************/
void exe_prim4(PRIMF primf)
{
STELM d_stelm,*tp0,*tp1,*tp2,*tp3;
TP_TYPES t0=tp_none,t1=tp_none,t2=tp_none,t3=tp_none;

DBUG_ENTER("exe_prim4");
tp0=st_pop(w);
tp1=st_pop(w);
tp2=st_pop(w);
tp3=st_pop(w);
if (GETORDERENTRY(ord,typestouched)==0)
  {
  if (tp_list==GETSTELMTYPE(tp0))
    t0=GETSTELMTYPE(tp0);
  else
    t0=GETSTELMTYPE(tp0) | tp0->subtype.type;
  if (tp_list==GETSTELMTYPE(tp1))
    t1=GETSTELMTYPE(tp1);
  else
    t1=GETSTELMTYPE(tp1) | tp1->subtype.type;
  if (tp_list==GETSTELMTYPE(tp2))
    t2=GETSTELMTYPE(tp2);
  else
    t2=GETSTELMTYPE(tp2) | tp2->subtype.type;
  if (tp_list==GETSTELMTYPE(tp3))
    t3=GETSTELMTYPE(tp3);
  else
    t3=GETSTELMTYPE(tp3) | tp3->subtype.type;
  SETORDERENTRY(ord,types)=TYPES2LONG(t0,t1,t2,t3);		/* internal types */
  SETORDERENTRY(ord,typestouched)=1;
  }
else
  {
  if (tp_list==GETTYPE(0,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp0)!=GETTYPE(0,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp0)|tp0->subtype.type)
        !=GETTYPE(0,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
    }
  if (tp_list==GETTYPE(1,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp1)!=GETTYPE(1,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(1,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp1)|tp1->subtype.type)
        !=GETTYPE(1,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(1,tp_none);
    }
  if (tp_list==GETTYPE(2,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp2)!=GETTYPE(2,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(2,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp2)|tp2->subtype.type)
        !=GETTYPE(2,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(2,tp_none);
    }
  if (tp_list==GETTYPE(3,GETORDERENTRY(ord,types)))
    {
    if (GETSTELMTYPE(tp3)!=GETTYPE(3,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(3,tp_none);
    }
  else
    {
    if ((GETSTELMTYPE(tp3)|tp3->subtype.type)
        !=GETTYPE(3,GETORDERENTRY(ord,types)))
      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(3,tp_none);
    }
  }
stelm_cpy(&d_stelm,&CONST_NONE,1);
switch (GETPRF(primf))
  {
  case p_repstr:
    {
    d_stelm.type=tp_none;
    break;
    }
  case p_replace:
    {
    d_stelm.type=tp_none;
    break;
    }
  case p_mreplace:
    {
    if ((((tp_int==GETSTELMTYPE(tp0)) || (tp_real==GETSTELMTYPE(tp0))) &&
	((tp_int==GETSTELMTYPE(tp1)) || (tp_real==GETSTELMTYPE(tp1)))) &&
	(tp_matrix==GETSTELMTYPE(tp3)))
      {
      d_stelm.type=tp_matrix;
      d_stelm.subtype.next=NULL;
      if (tp3->subtype.type==GETSTELMTYPE(tp2))
	d_stelm.subtype.type=tp3->subtype.type;
      else
	{
	d_stelm.type=tp_none;
	d_stelm.subtype.type=tp_none;
	}
      }
    else
      d_stelm.type=tp_none;
    break;
    }
  default:
    {
    yyfail("%s","that's no known 4-ary primitive function");
    break;
    }
  }
w=st_push(w,&d_stelm);					/* result to W STACK */
insert_restype(&d_stelm,ord);				/* insert hash restype */
DBUG_VOID_RETURN;
}
