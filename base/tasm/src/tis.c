#include <malloc.h>
#include <string.h>
#include "mytypes.h"
#include "loctypes.h"
#include "dbug.h"
#include "buildtree.h"
#include "support.h"
#include "tiny.h"
#include "tis.h"
#include "error.h"
#include "main.h"

FUNLIST *flbase=NULL;
char *vtis=VTIS;
STACK *func_dump;
/* constant ASM commands */
ORDER CONST_EXT={ext,{NULL},NULL,NULL,NULL,0,0},
      CONST_RTF	={rtf,{NULL},NULL,NULL,NULL,0,0,0,0},
      CONST_CCLOS={mkcclos,{0,0,0,0,0,0.0,"","",NULL,NULL,NULL,0},
		       NULL,NULL,NULL,0,0,0,0};
/* constant STackELeMent */
STELM CONST_NONE={tp_none,{tp_none,NULL},{NULL}};
KNOWLEDGE know=kn_sure;
int tis_state=0,do_dupe,dup_count=0,dup_num,max_desc_adr=0,new_params=0,
    clos_exe=0,arg_check=0;
ORDER *ord;
STACK *w,*a,*t,*r,*ret;

/************************************************************************/
/* returns type of executed function					*/
/* (used by execution)							*/
/************************************************************************/
STELM *exe_func()
{
STELM d_stelm;
long i;

DBUG_ENTER("exe_func");
while (TRUE)
  {
  if (_db_keyword_("STACKS"))
    stack_display();
  switch (GETORDERENTRY(ord,command))		/* command to be executed */
    {
    case pushcw_i:
      {
      DBUG_ORDERS("pushcw_i\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      d_stelm.type=tp_int;			/* set type int */
      d_stelm.subtype.type=tp_none;		/* set clear subtype */
      d_stelm.subtype.next=NULL;
      w=st_push(w,&d_stelm);
      ord=GETNEXTORDER(ord);
      break;
      }
    case pushcw_b:
      {
      DBUG_ORDERS("pushcw_b\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      d_stelm.type=tp_bool;			/* set type bool */
      d_stelm.subtype.type=tp_none;		/* set clear subtype */
      d_stelm.subtype.next=NULL;
      w=st_push(w,&d_stelm);
      ord=GETNEXTORDER(ord);
      break;
      }      
    case pushcw_pf:
      {
      DBUG_ORDERS("pushcw_pf\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      if (ISIA(GETORDERARGS(ord,primf)))
        w=st_push(w,&CONST_NONE);
      else
      {
        d_stelm.type=tp_prim;			/* set type prim */
        d_stelm.dat.prim=GETORDERARGS(ord,n);	/* which prim. function */
        d_stelm.subtype.type=tp_none;		/* set clear subtype */
        d_stelm.subtype.next=NULL;
        w=st_push(w,&d_stelm);
      }
      ord=GETNEXTORDER(ord);
      break;
      }
    case pushaw:
      {
      DBUG_ORDERS("pushaw\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      w=st_push(w,st_read(a,GETORDERARGS(ord,n))); /* copy element */ 
      ord=GETNEXTORDER(ord);
      break;
      }
    case pushtw:
      {
      DBUG_ORDERS("pushtw\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      w=st_push(w,st_read(t,GETORDERARGS(ord,n)));  /* copy element */ 
      ord=GETNEXTORDER(ord);
      break;
      }
    case pushtr:
      {
      DBUG_ORDERS("pushtr\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      r=st_push(r,st_read(t,GETORDERARGS(ord,n)));  /* copy element */ 
      ord=GETNEXTORDER(ord);
      break;
      }
      
    case pushar:
      {
      DBUG_ORDERS("pushar\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      r=st_push(r,st_read(a,GETORDERARGS(ord,n)));  /* copy element */ 
      ord=GETNEXTORDER(ord);
      break;
      }
    case pushr_p:
      {
      STELM *arg;
      
      DBUG_ORDERS("pushr_p\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      arg=find_address(GETORDERARGS(ord,desc));	/* search for adress */
      r=st_push(r,arg);				/* push arg to r STACK */
      stelm_free(arg);				/* free buffer */
      ord=GETNEXTORDER(ord);
      break;
      }
    case pushw_p:
      {
      STELM *arg;
      
      DBUG_ORDERS("pushw_p\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      arg=find_address(GETORDERARGS(ord,desc));	/* search for adress */
      w=st_push(w,arg);				/* push arg to w STACK */
      stelm_free(arg);				/* free buffer */
      ord=GETNEXTORDER(ord);
      break;
      }
    case pushaux:
      {
      DBUG_ORDERS("pushaux\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      						/* do nothing */
      ord=GETNEXTORDER(ord);			
      break;
      }
    case moveaw:
      {
      STELM *arg;
      
      DBUG_ORDERS("moveaw\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      arg=st_pop(a);				/* pop element from a STACK */
      w=st_push(w,arg);				/* push element to w STACK */
      stelm_free(arg);				/* free buffer */
      ord=GETNEXTORDER(ord);
      break;
      }
    case movear:
      {
      STELM *arg;
      
      DBUG_ORDERS("movear\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      arg=st_pop(a);				/* pop element from a STACK */
      r=st_push(r,arg);				/* push element to r STACK */
      stelm_free(arg);				/* free buffer */
      ord=GETNEXTORDER(ord);
      break;
      }
    case apply:
      {
      STELM *func,res;
      long arity;
      CLOSURE *clos;
      
      DBUG_ORDERS("apply\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      func=st_pop(w);				/* apply what ? */
      switch (GETSTELMTYPE(func))				
	{
	case tp_func:				/* we have a function! */
	  {
	  FUNDESC *des;
	  STELM *result,*old_func;
	  long i;
	  RETURNADR *rt;
	  ORDER *old_ord;
	  
	  des=GETSTELMDATA(func,desc);		/* get function descriptor */
          if (0!=GETORDERARGS(ord,m))
	    arity=GETORDERARGS(ord,m);		/* restore prev set apply-arity */
	  else
	    arity=GETORDERARGS(ord,n);		/* set normal apply-arity */
	  if (NULL==des)			/* no desciptor */
	    {
	    for (i=0; i<arity; i++)		/* remove all args from w STACK */
	      st_pop(w);
	    w=st_push(w,&CONST_NONE);		/* push result none */
	    SETORDERARGS(ord,m)=0;		/* set no args left */
	    insert_restype(&CONST_NONE,ord);	/* inser result type */
	    ord=GETNEXTORDER(ord);
	    }
	  else					/* there is a function desciptor */
	    {
	    if (NULL==(SETORDERARGS(ord,label)=
		malloc(strlen(GETDESCENTRY(des,label))+1))) /* store functionlable */
	      yyfail("%s","no memory for label!");
	    strcpy(SETORDERARGS(ord,label),GETDESCENTRY(des,label)); 
	    					/* remember last functioncall */
	    if (GETDESCENTRY(des,nv)<=arity)		/* if execution possible */
	      {
	      if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
		result=get_result(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
				  t,GETDESCENTRY(des,nfv));
	      else				/* we have a conditional */
		result=get_result(GETDESCENTRY(des,label),a,GETDESCENTRY(des,nv)-1,
				  t,GETDESCENTRY(des,nfv)/4);
	      if ((NULL!=ret) && (0<GETTOPOFSTACK(ret))) /* returnstack not empty */
		{
		old_func=getfunc_ret();			/* gets last functioncall */
		if (DT_CONDITIONAL!=GETDESCENTRY(des,tag)) /* sets dependencies */
		  set_depend(des,w,t,old_func);
		else
		  set_depend(des,a,t,old_func);
		}
	      if (((NULL!=result) && (tp_illegal!=result->type)) && 
		  				/* do we have a valid result ? */
		  (0==tis_state))	/* old result stored or second try */
		{
		if (arity==GETDESCENTRY(des,nv))/* if arity fits the need */
		  insert_restype(result,ord);	/* insert result type */
		if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
		  				/* remove args from w STACK */
		  {
		  for (i=0; i<GETDESCENTRY(des,nv); i++)
		    stelm_free(st_pop(w));
		  }
		else				/* for conditional one args less */
		  {
		  for (i=1; i<GETDESCENTRY(des,nv); i++)
		    stelm_free(st_pop(w));
		  }
		w=st_push(w,result);		/* push result */
		arity-=GETDESCENTRY(des,nv);	/* overloaded apply ? */
		SETORDERARGS(ord,m)=arity;	/* calc new arity */
		if (0==arity)
		  {
		  SETORDERARGS(ord,k)=-1;	/* order executed */
		  ord=GETNEXTORDER(ord);
		  }
		}
	      else				/* result unknown unitl now */
		{
		tis_state=0;
		old_ord=search_ret(ord);
		if (NULL==old_ord)		/* first call from here! */
		  {
		  if (NULL==(rt=malloc(sizeof(RETURNADR))))
		    yyfail("%s","no memory for returnadress!");
		  rt->ord=ord;			/* remember enviroment of func */
		  w=st_push(w,func);		/* save function */
		  rt->w=w;			/* store STACKs */
		  rt->a=a;
		  rt->t=t;
		  rt->r=r;
		  rt->status=(clos_exe*CLOS_EXE)+(new_params*NEW_PARAMS);
		  ret=st_push(ret,new_stelm(tp_returnadr,rt));
		  /* returndata stored */
		  if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))	/* COMBinator */
		    {
		    if (DT_CASE!=GETDESCENTRY(des,tag))		/* no CASE */
		      {
		      a=st_make(w,GETDESCENTRY(des,nv)+1);
		      				/* build new frames; +1 for func */
		      st_pop(a);			/* remove func from stack */
		      w=NULL;
		      t=st_make(t,GETDESCENTRY(des,nfv));
		      r=NULL;
		      SETORDERARGS(ord,m)=arity-GETDESCENTRY(des,nv);
		      					/* store overload arity */
		      ord=find_func(GETFIRSTFUNCTION(program_start),
				    GETDESCENTRY(des,label));
		      insert_args(a,GETDESCENTRY(des,nv),t,GETDESCENTRY(des,nfv),ord);
		      }
		    else				/* it is a CASE */
		      {
		      w=st_make(w,GETDESCENTRY(des,nv)+1);
		      				/* build new frames; +1 for func */
		      st_pop(w);			/* remove func from stack */
		      a=NULL;
		      t=st_make(t,GETDESCENTRY(des,nfv));
		      r=NULL;
		      SETORDERARGS(ord,m)=arity-GETDESCENTRY(des,nv);
		      					/* store overload arity */
		      ord=find_func(GETFIRSTFUNCTION(program_start),
				  GETDESCENTRY(des,label));
		      insert_args(w,GETDESCENTRY(des,nv),t,GETDESCENTRY(des,nfv),ord);
		      }
		    }
		  else					/* CONDItional */
		    {
		    char *str;
		    
		    w=st_make(w,2);			/* frame = bool + CONDI */
		    st_pop(w);				/* remove func from stack */
		    a=st_make(a,GETDESCENTRY(des,nv)-1); /* argument frame */
		    t=st_make(t,GETDESCENTRY(des,nfv)/4); /* tilde frame */
		    r=NULL;
		    SETORDERARGS(ord,m)=arity-GETDESCENTRY(des,nv);
		    					/* store overload arity */
		    if (NULL==(str=malloc(strlen(GETDESCENTRY(des,label)))))
		      yyfail("%s","no memory for label!");
		    strcpy(str,"func_");		/* build function label */
		    strcpy(&str[5],&(GETDESCENTRY(des,label)[6]));
		    ord=find_func(GETFIRSTFUNCTION(program_start),str);
		    if (1<GETDESCENTRY(des,nv))		/* are there any args */
		      insert_args(a,GETDESCENTRY(des,nv)-1,t,
				  GETDESCENTRY(des,nfv)/4,GETNEXTORDER(ord));
		    free(str);
		    }
		  }
		else					/* have been here before */
		  {
		  if (old_ord!=ord)			/* old_ord must be jcond */
		    {
		    if (NULL==(rt=malloc(sizeof(RETURNADR))))
		      yyfail("%s","no memory for returnadress!");
		    rt->ord=ord;			/* remember enviroment of func */
		    rt->status=(clos_exe*CLOS_EXE)+MUST_EXE; /* must execute */
		    rt->w=w;				/* store STACKs */
		    rt->w=st_push(w,func);
		    rt->a=a;
		    rt->t=t;
		    rt->r=r;
		    func_dump=st_push(func_dump,new_stelm(tp_returnadr,rt)); 
		    					/* returndata stored */
		    restore_ret(old_ord);		/* restors state of old_ord */
		    SETORDERENTRY(old_ord,branch)=
		      -GETORDERENTRY(old_ord,branch); /* then-case without result */
		    }
		  else					/* bad recursion */
		    {
		    if (((gammabeta==GETORDERENTRY(ord,command)) ||
			 (gammacase==GETORDERENTRY(ord,command))) ||
			(Gamma==GETORDERENTRY(ord,command)))
		      insert_functype(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
				      r,GETDESCENTRY(des,nfv),&CONST_NONE,kn_sure);
		    else
		      insert_functype(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
				      t,GETDESCENTRY(des,nfv),&CONST_NONE,kn_sure);
		    for (i=0; i<GETDESCENTRY(des,nv); i++) /* remove args from stack */
		      stelm_free(st_pop(w));
		    w=st_push(w,&CONST_NONE);		/* set result none */
		    ord=GETNEXTORDER(ord);
		    }
		  }
		}	  
	      }
	    else					/* build closure */
	      {
	      if (NULL==(clos=malloc(sizeof(CLOSURE))))
		yyfail("%s","no memory for closure");
	      if (NULL==(clos->desc=malloc(sizeof(FUNDESC))))
		yyfail("%s","no memory for descriptor!");
	      clos->desc=des;				/* set closure data */
	      clos->i=arity;
	      clos->j=GETDESCENTRY(clos->desc,nv);
	      clos->k=GETDESCENTRY(clos->desc,nfv);
	      if (0!=clos->i)				/* are there any args ? */
		{
		if (NULL==(clos->w=malloc(sizeof(STELM)*(clos->i))))
		  yyfail("%s","no memory for closure");
		stelm_cpy(clos->w,st_read(w,clos->i-1),clos->i);
		for (i=0; i<clos->i; i++)		/* copy args */
		  stelm_free(st_pop(w));
		}
	      else					/* no args */
		clos->w=NULL;
	      if (0!=clos->k)				/* are there any targs ? */
		{
		if (NULL==(clos->t=malloc(sizeof(STELM)*(clos->k))))
		  yyfail("%s","no memory for closure");
		stelm_cpy(clos->t,st_read(t,clos->k-1),clos->k);
		for (i=0; i<clos->k; i++)		/* copy args */
		  stelm_free(st_pop(t));
		}
	      else					/* no targs */
		clos->t=NULL;
	      res.type=tp_clos;
	      res.dat.clos=clos;
	      w=st_push(w,&res);			/* push closure on W Stack */	
	      stelm_free(func);
	      insert_restype(&res,ord);			/* set restype */
	      ord=GETNEXTORDER(ord);
	      }
	    }
	  break;
	  }
	case tp_prim:
	  {
	  int arity;
	  
	  if (0!=GETORDERARGS(ord,m))
	    arity=GETORDERARGS(ord,m);		/* restore prev set apply-value */
	  else
	    arity=GETORDERARGS(ord,n);		/* set normal apply-value */
          switch (GETPRF(GETSTELMDATA(func,prim)))/* unary primitiv function */
	    {
 	    case p_not:
	    case p_abs:
	    case p_trunc:
	    case p_floor:
	    case p_ceil:
	    case p_frac:
	    case p_tan:
	    case p_ln:
	    case p_exp:
	    case p_class: 
	    case p_vdim:
	    case p_type:
	    case p_transpose:
	    case p_reverse:
	    case p_transform:
	    case p_to_vect:
	    case p_to_tvect:
	    case p_to_mat:
	    case p_to_scal:
	    case p_to_list:
	    case p_to_field:
	    case p_vc_max:
	    case p_vc_min:
	    case p_vc_plus:
	    case p_vc_minus:
	    case p_vc_mult:
	    case p_vc_div:
	    case p_empty:
	    case p_sin:
	    case p_cos:
	    case p_neg:
	    case p_ldim: 
            case p_to_char:
            case p_to_ord:
	      {
	      if (1==arity)				/* arity of ap matches */
		exe_prim1(GETSTELMDATA(func,prim));	/* execute primfunction */
	      else
		{					/* this makes no sense */
		while (0<arity)				/* remove args */
		  {
		  st_pop(w);
		  arity--;
		  }
		w=st_push(w,&CONST_NONE);		/* push NONE as result */
		}
	      break;
	      }
	    case p_lselect:			/* very special prim.func. */
	      {					/* result can be function!!! */
	      CLOSURE *clos;
	      
	      if (2<=arity)			/* check arity */
		{
		exe_prim2(GETSTELMDATA(func,prim));	/* execute primfunction */
		SETORDERARGS(ord,m)=arity-2;
		if (0!=GETORDERARGS(ord,m))
		  {
		  delete_order(GETPREVORDER(ord));
		  ord=GETPREVORDER(ord);	/* back because of orderadvancement */
		  }
		}
	      else					/* closure */
		{
		if (NULL==(clos=malloc(sizeof(CLOSURE))))
		  yyfail("%s","no memory for closure");
		clos->desc=NULL;			/* remember primfunc */
		clos->k=GETSTELMDATA(func,prim);
		clos->i=GETORDERARGS(ord,n);
		clos->j=GETORDERARGS(ord,m);
		if (0!=clos->i)				/* there ara aruments */
		  {
		  if (NULL==(clos->w=malloc(sizeof(STELM)*(clos->i))))
		    yyfail("%s","no memory for closure");
		  stelm_cpy(clos->w,st_read(w,clos->i-1),clos->i);
		  for (i=0; i<clos->i; i++)
		    stelm_free(st_pop(w));
		  }
		else					/* no args  */
		  clos->w=NULL;
		d_stelm.type=tp_clos;
		d_stelm.dat.clos=clos;
		w=st_push(w,&d_stelm);
		}
	      break;
	      }
	    case p_plus:
	    case p_minus:
	    case p_mult:
	    case p_div:
	    case p_mod:
	    case p_or:
	    case p_and:
	    case p_xor:
	    case p_eq:
	    case p_ne:
	    case p_lt:
	    case p_le:
	    case p_gt:
	    case p_ge:
	    case p_lcut:
	    case p_lunite:
	    case p_max:
	    case p_min:
	    case p_quot:
	    case p_ip:
	    case p_dim: 
	    case p_mdim:
	    case p_vcut:
	    case p_vrotate:
	    case p_vselect:
	    case p_lrotate:
	    case p_vunite:
	    case p_c_max:
	    case p_c_min:
	    case p_c_plus:
	    case p_c_minus:
	    case p_c_mult:
	    case p_c_div: 
	    case p_f_eq:
	    case p_f_ne:
            case p_sprintf:
            case p_sscanf:
	      {
	      CLOSURE *clos;
	      
	      if (2==arity)
		exe_prim2(GETSTELMDATA(func,prim));	/* execute primfunction */
	      else
		{
		if (2<arity)
		  {					/* this makes no sense */
		  while (0<arity)
		    {					/* remove arguments */
		    st_pop(w);
		    arity--;
		    }
		  w=st_push(w,&CONST_NONE);
		  }
		else					/* make closure */
		  {
		  if (NULL==(clos=malloc(sizeof(CLOSURE))))
		    yyfail("%s","no memory for closure");
		  clos->desc=NULL;			/* remember primfunc */
		  clos->k=GETSTELMDATA(func,prim);
		  clos->i=GETORDERARGS(ord,n);
		  clos->j=GETORDERARGS(ord,m);
		  if (0!=clos->i)			/* there are arguments */
		    {
		    if (NULL==(clos->w=malloc(sizeof(STELM)*(clos->i))))
		      yyfail("%s","no memory for closure");
		    stelm_cpy(clos->w,st_read(w,clos->i-1),clos->i);
		    for (i=0; i<clos->i; i++)
		      stelm_free(st_pop(w));
		    }
		  else
		    clos->w=NULL;			/* no arguments */
		  d_stelm.type=tp_clos;
		  d_stelm.dat.clos=clos;
		  w=st_push(w,&d_stelm);
		  }
		}
	      break;
	      }
	    case p_lreplace:
	    case p_cut:
	    case p_mcut:
	    case p_ltransform:
	    case p_rotate:
	    case p_mrotate: 
	    case p_select: 
	    case p_mselect: 
	    case p_substr:
	    case p_vreplace:
	    case p_unite:
	    case p_munite:
	    case p_mre_c:
	    case p_mre_r:
	      {
	      CLOSURE *clos;
	      
	      if (3==arity)
		exe_prim3(GETSTELMDATA(func,prim));	/* execute primfunction */
	      else
		{
		if (3<arity)
		  {					/* this makes no sense */
		  while (0<arity)
		    {
		    st_pop(w);
		    arity--;
		    }
		  w=st_push(w,&CONST_NONE);
		  }
		else					/* make closure */
		  {
		  if (NULL==(clos=malloc(sizeof(CLOSURE))))
		    yyfail("%s","no memory for closure");
		  clos->desc=NULL;			/* remember primfunc */
		  clos->k=GETSTELMDATA(func,prim);
		  clos->i=GETORDERARGS(ord,n);
		  clos->j=GETORDERARGS(ord,m);
		  if (0!=clos->i)
		    {
		    if (NULL==(clos->w=malloc(sizeof(STELM)*(clos->i))))
		      yyfail("%s","no memory for closure");
		    stelm_cpy(clos->w,st_read(w,clos->i-1),clos->i);
		    for (i=0; i<clos->i; i++)
		      stelm_free(st_pop(w));
		    }
		  else
		    clos->w=NULL;
		  d_stelm.type=tp_clos;
		  d_stelm.dat.clos=clos;
		  w=st_push(w,&d_stelm);
		  }
		}
	      break;
	      }
	    case p_repstr:
	    case p_replace:
	    case p_mreplace:
	      {
	      CLOSURE *clos;
	      
	      if (4==arity)
		exe_prim4(GETSTELMDATA(func,prim));	/* execute primfunction */
	      else
		{
		if (4<arity)
		  {					/* this makes no sense */
		  while (0<arity)
		    {
		    st_pop(w);
		    arity--;
		    }
		  w=st_push(w,&CONST_NONE);
		  }
		else					/* make closure */
		  {
		  if (NULL==(clos=malloc(sizeof(CLOSURE))))
		    yyfail("%s","no memory for closure");
		  clos->desc=NULL;			/* remember primfunc */
		  clos->k=GETSTELMDATA(func,prim);
		  clos->i=GETORDERARGS(ord,n);
		  clos->j=GETORDERARGS(ord,m);
		  if (0!=clos->i)
		    {
		    if (NULL==(clos->w=malloc(sizeof(STELM)*(clos->i))))
		      yyfail("%s","no memory for closure");
		    stelm_cpy(clos->w,st_read(w,clos->i-1),clos->i);
		    for (i=0; i<clos->i; i++)
		      stelm_free(st_pop(w));
		    }
		  else
		    clos->w=NULL;			/* there are no arguments */
		  d_stelm.type=tp_clos;
		  d_stelm.dat.clos=clos;
		  w=st_push(w,&d_stelm);
		  }
		}
	      break;
	      }
	    default:				/* primitive function not known */
	      {
	      yyfail("%s","unknown primitive function!");
	      break;
	      }
	    }
	  ord=GETNEXTORDER(ord);
	  break;
	  }
        case tp_clos:
	  {
	  long i,j;
	  STELM dummy;
	  CLOSURE *clos;
	  FUNDESC *des;
	  PRIMF primf;
	  STACK *arg;
	  
	  if (NULL==GETSTELMDATA(func,clos))
	    {
	    if (0!=GETORDERARGS(ord,m))
	      arity=GETORDERARGS(ord,m);		/* restore prev set apply-value */
	    else
	      arity=GETORDERARGS(ord,n);		/* set normal apply-value */
	    for (i=0;i<arity;i++)
	      st_pop(w);
	    w=st_push(w,&CONST_NONE);
	    insert_restype(&CONST_NONE,ord);
	    ord=GETNEXTORDER(ord);
	    }
	  else
	    {
	    clos=GETSTELMDATA(func,clos);		/* get closure data */
	    des=clos->desc;
	    primf=clos->k;
	    if (0!=GETORDERARGS(ord,m))
	      arity=GETORDERARGS(ord,m);		/* restore prev set apply-value */
	    else
	      arity=GETORDERARGS(ord,n);		/* set normal apply-value */
	    if ((NULL!=des) && 			/* if CONDI unfold to A STACK */
		(DT_CONDITIONAL==GETDESCENTRY(des,tag)))
	      arg=a;
	    else
	      arg=w;
	    for (i=0; i<clos->i; i++)			/* unfold W-frame */
	      {
	      arg=st_push(arg,&(clos->w[i]));
	      stelm_free(&(clos->w[i]));
	      }
	    if (NULL!=clos->w)				/* free stored frame */
	      free(clos->w);
	    i=clos->i+arity;				/* calc no. of args */	
	    if (clos->j<=i) 				/* arity <= no. of args */
	      {
	      if (NULL!=des)				/* got FUNDESC */
		{
		for (j=0; j<clos->k; j++)		/* unfold T-frame */
		  {
		  t=st_push(t,&(clos->t[j]));
		  stelm_free(&(clos->t[j]));
		  }
		if (NULL!=clos->t)			/* free stored frame */
		  free(clos->t);
		dummy.type=tp_func;
		dummy.dat.desc=des;
		}
	      else					/* primitiv function */
		{
		dummy.type=tp_prim;
		dummy.dat.prim=primf;
		}
	      if ((NULL!=des) && (DT_CONDITIONAL==GETDESCENTRY(des,tag)))
		a=arg;
	      else
		w=arg;
	      w=st_push(w,&dummy);			/* store function */
	      SETORDERARGS(ord,m)=i;
	      }
	    else					/* new CLOSURE to build */
	      {
	      clos->i=i;				/* new no of args stored */
	      if(NULL==(clos->w=malloc(sizeof(STELM)*i)))
		yyfail("%s","no memory for new closure");
	      stelm_cpy(clos->w,&(arg->stack[arg->topofstack-i]),i); /* store args */
	      for (i=0; i<clos->i; i++)			/* free frame on stack */
		stelm_free(st_pop(arg));
	      dummy.type=tp_clos;
	      dummy.dat.clos=clos;
	      if (DT_CONDITIONAL==GETDESCENTRY(des,tag))
		a=arg;
	      else
		w=arg;
	      w=st_push(w,&dummy);			/* push closure on stack */
	      insert_restype(&dummy,ord);
	      ord=GETNEXTORDER(ord);
	      }
	    }
	  break;
	  }
	default:				/* illegal function call */
	  {					/* no valid function */
	  int j;
	  
	  if (0!=GETORDERARGS(ord,m))
	    j=GETORDERARGS(ord,m);
	  else
	    j=GETORDERARGS(ord,n);
	  for (i=0; i<j;i++)			/* remove arguments */
	    st_pop(w);
	  w=st_push(w,&CONST_NONE);		/* result of function is unknown */
	  insert_restype(&CONST_NONE,ord);
	  ord=GETNEXTORDER(ord);
	  break;
	  }
	}
      stelm_free(func);
      break;
      }
    case gammabeta:
    case tail:
    case beta:
    case Case:
    case gammacase:
    case Gamma:
      {
      STELM *result,*old_func;
      FUNDESC *des;
      ORDER *old_ord;
      RETURNADR *rt;
      
      DBUG_ORDERS("[Gamma]beta|case\\tail\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      des=find_desc(GETFIRSTDESC(program_start),GETORDERARGS(ord,label));
      DBUG_PRINT("ORDERS",("%d %s\n",ord->command, GETDESCENTRY(des,label)));
      /* what kind of functioncall do we have? do we know the result? */
      if (((gammabeta==GETORDERENTRY(ord,command)) ||
	   (gammacase==GETORDERENTRY(ord,command))) ||
	  (Gamma==GETORDERENTRY(ord,command)))
	result=get_result(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
			  r,GETDESCENTRY(des,nfv));
      else
	result=get_result(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
			  t,GETDESCENTRY(des,nfv));
      des=find_desc(GETFIRSTDESC(program_start),GETORDERARGS(ord,label));
      if ((NULL!=ret) && (0<GETTOPOFSTACK(ret)))
	{
	old_func=getfunc_ret();
        if (((gammabeta==GETORDERENTRY(ord,command)) ||
	     (gammacase==GETORDERENTRY(ord,command))) ||
	    (Gamma==GETORDERENTRY(ord,command)))
	  set_depend(des,w,r,old_func);
        else
	  set_depend(des,w,t,old_func);
	}
      if (((NULL!=result) && (tp_illegal!=result->type)) &&
	  (0==tis_state)) 		/* old result stored or second try */
	{
	insert_restype(result,ord);
	SETORDERARGS(ord,k)=-1;			/* order executed */
	for (i=0; i<GETDESCENTRY(des,nv); i++)
	  stelm_free(st_pop(w));
	if (((gammabeta==GETORDERENTRY(ord,command)) ||	/* stack switch r<->t */
	     (gammacase==GETORDERENTRY(ord,command))) ||
	    (Gamma==GETORDERENTRY(ord,command)))
	  {
	  STACK *dummy;
	  
	  dummy=r;				/* stack switch with dummy */
	  r=t;
	  t=dummy;
	  }
	w=st_push(w,result);
	ord=GETNEXTORDER(ord);
	}
      else
        {
	tis_state=0;
	old_ord=search_ret(ord);
	if (NULL==old_ord)			/* first call from here */
	  {
	  if (NULL==(rt=malloc(sizeof(RETURNADR))))
	    yyfail("%s","no memory for returnadress!");
	  rt->ord=ord;				/* remember enviroment of func */
	  rt->status=(clos_exe*CLOS_EXE)+(new_params*NEW_PARAMS);
	  rt->w=w;
	  rt->a=a;
	  if (((gammabeta==GETORDERENTRY(ord,command)) || /* stack switch r<->t */
	       (gammacase==GETORDERENTRY(ord,command))) ||
	      (Gamma==GETORDERENTRY(ord,command)))
	    {
	    rt->t=r;
	    rt->r=t;
	    }
	  else
	    {
	    rt->t=t;
	    rt->r=r;
	    }
	  ret=st_push(ret,new_stelm(tp_returnadr,rt)); /* returndata stored */
	  if (((Case==GETORDERENTRY(ord,command)) ||
	       (gammacase==GETORDERENTRY(ord,command))) ||
	      (Gamma==GETORDERENTRY(ord,command)))
	    {
	    w=st_make(w,GETDESCENTRY(des,nv));		/* build new frames */
	    a=NULL;
	    }
	  else
	    {
	    a=st_make(w,GETDESCENTRY(des,nv));		/* build new frames */
	    w=NULL;
	    }	    
	  if (((gammabeta==GETORDERENTRY(ord,command)) ||
	       (gammacase==GETORDERENTRY(ord,command))) ||
	      (Gamma==GETORDERENTRY(ord,command)))
	    t=st_make(r,GETDESCENTRY(des,nfv));
	  else
	    t=st_make(t,GETDESCENTRY(des,nfv));
	  r=NULL;
	  old_ord=ord;
	  ord=find_func(GETFIRSTFUNCTION(program_start),GETDESCENTRY(des,label));
	  if (((Case==GETORDERENTRY(old_ord,command)) ||
	       (gammacase==GETORDERENTRY(old_ord,command))) ||
	      (Gamma==GETORDERENTRY(old_ord,command)))
	    insert_args(w,GETDESCENTRY(des,nv),t,GETDESCENTRY(des,nfv),ord);
	  else
	    insert_args(a,GETDESCENTRY(des,nv),t,GETDESCENTRY(des,nfv),ord);
	  }
	else					/* have been here before */
	  {
	  if (old_ord!=ord)			/* old_ord must be jcond */
	    {
	    if (NULL==(rt=malloc(sizeof(RETURNADR))))
	      yyfail("%s","no memory for returnadress!");
	    rt->ord=ord;			/* remember enviroment of func */
	    rt->status=(clos_exe*CLOS_EXE)+MUST_EXE;
	    rt->w=w;
	    rt->a=a;
	    if (((Case==GETORDERENTRY(old_ord,command)) ||
	       (gammacase==GETORDERENTRY(old_ord,command))) ||
	      (Gamma==GETORDERENTRY(old_ord,command)))
	      {
	      rt->t=r;
	      rt->r=t;
	      }
	    else
	      {
	      rt->t=t;
	      rt->r=r;
	      }
	    func_dump=st_push(func_dump,new_stelm(tp_returnadr,rt)); 
	    /* returndata stored */
	    restore_ret(old_ord);		/* restors state of old_ord */
	    SETORDERENTRY(old_ord,branch)=-GETORDERENTRY(old_ord,branch); 
	    					/* then-case without result */
	    }
	  else					/* bad recursion */
	    {
	    if (((gammabeta==GETORDERENTRY(ord,command)) ||
		 (gammacase==GETORDERENTRY(ord,command))) ||
		(Gamma==GETORDERENTRY(ord,command)))
	      insert_functype(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
			      r,GETDESCENTRY(des,nfv),&CONST_NONE,kn_sure);
	    else
	      insert_functype(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
			      t,GETDESCENTRY(des,nfv),&CONST_NONE,kn_sure);
	    for (i=0; i<GETDESCENTRY(des,nv); i++)
	      stelm_free(st_pop(w));
	    if (((gammabeta==GETORDERENTRY(ord,command)) ||
		 (gammacase==GETORDERENTRY(ord,command))) ||
		(Gamma==GETORDERENTRY(ord,command)))
              {
              STACK *dstack;

              dstack=r;
              r=t;
              t=dstack;
              }
	    w=st_push(w,&CONST_NONE);
	    ord=GETNEXTORDER(ord);
	    }
	  }
	}
      break;
      }
    case mkap:						/* illegal function call */
      {
      int i;
      
      DBUG_ORDERS("mkap\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      for (i=0; i<=ord->args.n;i++)			/* remove args from STACK */
	st_pop(w);
      w=st_push(w,&CONST_NONE);			/* result useless closure == none */
      ord=GETNEXTORDER(ord);
      break;
      }	
    case mkiclos:					/* just an other closure */
      {
      int i;
    
      for(i=0; i<=ord->args.n;i++)			/* pop arguments */
        st_pop(w);
      w=st_push(w,&CONST_NONE);			/* result ist NONE */
      ord=GETNEXTORDER(ord);
      break;
      }
    case mkdclos:
      {
      STELM *prim,dummy;
      CLOSURE *clos;
      
      DBUG_ORDERS("mkdclos\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      prim=st_pop(w);					/* get prim function */
      if (NULL==(clos=malloc(sizeof(CLOSURE))))
	yyfail("%s","no memory for closure");
      clos->desc=NULL;					/* remember primfunc */
      clos->k=GETSTELMDATA(prim,prim);			/* store function */
      clos->i=GETORDERARGS(ord,n);
      clos->j=GETORDERARGS(ord,m);
      if (0!=clos->i)				/* do we have any arguments */
	{
	if (NULL==(clos->w=malloc(sizeof(STELM)*(clos->i))))
	  yyfail("%s","no memory for closure");
	stelm_cpy(clos->w,st_read(w,clos->i-1),clos->i); /* store all args */
	for (i=0; i<clos->i; i++)			/* free STACK elements */
	  stelm_free(st_pop(w));
	}
      else
	clos->w=NULL;					/* no args there */
      dummy.type=tp_clos;				/* init closure data */
      dummy.dat.clos=clos;
      dummy.subtype.type=tp_none;			/* set no subtype */
      dummy.subtype.next=NULL;
      stelm_free(prim);
      w=st_push(w,&dummy);				/* push closure */
      ord=GETNEXTORDER(ord);
      break;
      }
    case mkilist:					/* build an inverted list */
    case mklist:					/* build a list */
      {
      int i;
      STELM *stelm;
      
      DBUG_ORDERS("mklist\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      d_stelm.type=tp_list;				/* we have a list */
      d_stelm.subtype.type=tp_empty;			/* start with empty */
      d_stelm.subtype.next=NULL;
      for (i=0; i<GETORDERARGS(ord,n); i++)		/* check all elements */
	{
	stelm=st_pop(w);				/* get element */
        dump_list_func(stelm);				/* if function or closure, */
							/* they must be executed */
   	if ((tp_empty!=d_stelm.subtype.type) &&	/* if stored type do not match */
	    (d_stelm.subtype.type!=GETSTELMTYPE(stelm))) /* set type to none */
	  {
	  SUBTYPE *subt,*dsubt;
		  
          d_stelm.subtype.type=tp_none;
	  subt=d_stelm.subtype.next;
	  while (NULL!=subt)				/* free whole structure */
	    {
	    dsubt=subt->next;
	    free(subt);
	    subt=dsubt;
	    }
	  d_stelm.subtype.next=NULL;
	  }
        else						/* types match */
	  {
	  if (tp_empty==d_stelm.subtype.type)
	    {
	    d_stelm.subtype.type=GETSTELMTYPE(stelm);
	    if (((tp_list==GETSTELMTYPE(stelm)) || (tp_matrix==GETSTELMTYPE(stelm))) ||
		((tp_vect==GETSTELMTYPE(stelm)) || (tp_tvect==GETSTELMTYPE(stelm))))
	      d_stelm.subtype.next=subtype_cpy(&stelm->subtype);
	    else
	      {
	      SUBTYPE *subt,*dsubt;
		  
              subt=d_stelm.subtype.next;
	      while (NULL!=subt)			/* free whole structure */
	        {
		dsubt=subt->next;
		free(subt);
		subt=dsubt;
	    	}
	      d_stelm.subtype.next=NULL;
	      }
	    }
	  else
	    subtypecmp(d_stelm.subtype.next,&(stelm->subtype));
	  }
	stelm_free(stelm);
	}
      w=st_push(w,&d_stelm);
      ord=GETNEXTORDER(ord);
      break;
      }
    case mkframe: 				/* cr 19.02.96 */
      {
      DBUG_ORDERS("mkframe\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      for (i=0; i<GETORDERARGS(ord,n); i++)     /* check all elements */
	{
          st_pop(w);                            /* get element */
        }
      w=st_push(w,&CONST_NONE);
      ord=GETNEXTORDER(ord);
      break;
      }
    case mkslot: /* cr 19.02.96 */
      {
      DBUG_ORDERS("mkslot\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      st_pop(w);
      st_pop(w);
      w=st_push(w,&CONST_NONE);
      ord=GETNEXTORDER(ord);
      break;
      }
    case mkcase:				/* nothing to do */
      {
      DBUG_ORDERS("mkcase\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      st_pop(w);
      w=st_push(w,&CONST_NONE);
      ord=GETNEXTORDER(ord);
      break;
      }
    case mkgaclos:
      {
      STACK *d;
      STELM *arg;
      d=t;					/* switch STACK T & R */
      t=r;
      r=d;
      arg=st_pop(t);
      w=st_push(w,arg);				/* move function to W STACK */
      stelm_free(arg);
      }
    case mkcclos:
    case mkbclos:				/* build up closure */
    case mksclos:
      {
      STELM *func,res;
      CLOSURE *clos;
      
      DBUG_ORDERS("mkgaclos\\mkcclos\\mkbclos\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      func=st_pop(w);				/* get function */
      if (NULL==(clos=malloc(sizeof(CLOSURE))))
	yyfail("%s","no memory for closure");
      clos->desc=GETSTELMDATA(func,desc);	/* function data */
      clos->i=GETORDERARGS(ord,n);
      clos->j=GETORDERARGS(ord,m);
      clos->k=GETORDERARGS(ord,k);
      if (0<clos->i)				/* save args */
	{
	if (NULL==(clos->w=malloc(sizeof(STELM)*(clos->i))))
	  yyfail("%s","no memory for closure");
	stelm_cpy(clos->w,st_read(w,clos->i-1),clos->i);
	for (i=0; i<clos->i; i++)
	  stelm_free(st_pop(w));
	}
      else
	clos->w=NULL;
      if (0<clos->k)				/* save tildeargs */
	{
	if (NULL==(clos->t=malloc(sizeof(STELM)*(clos->k))))
	  yyfail("%s","no memory for closure");
	stelm_cpy(clos->t,st_read(t,clos->k-1),clos->k);
	}
      else
	clos->t=NULL;
      res.type=tp_clos;				/* build up result of command */
      res.dat.clos=clos;
      res.subtype.type=tp_none;
      res.subtype.next=NULL;
      w=st_push(w,&res);
      if (1==clos_exe)
	dump_list_func(&res);
      ord=GETNEXTORDER(ord);
      break;
      }
    case delta1:				/* execute prim.funcs. */
      DBUG_ORDERS("delta1\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      exe_prim1(GETORDERARGS(ord,primf));
      ord=GETNEXTORDER(ord);
      break;
    case delta2:
      DBUG_ORDERS("delta2\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      exe_prim2(GETORDERARGS(ord,primf));
      ord=GETNEXTORDER(ord);
      break;
    case delta3:
      {
      DBUG_ORDERS("delta3\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      exe_prim3(GETORDERARGS(ord,primf));
      ord=GETNEXTORDER(ord);
      break;
      }
    case delta4:
      {
      DBUG_ORDERS("delta4\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      exe_prim4(GETORDERARGS(ord,primf));
      ord=GETNEXTORDER(ord);
      break;
      }
    case intact:				/* interaction */
      {
      int i, dim;
    
      dim = get_ia_arity(GETIA(GETORDERARGS(ord,primf)));
      for(i=0; i<dim;i++)
        st_pop(w);
      w=st_push(w,&CONST_NONE);
      ord=GETNEXTORDER(ord);
      break;
      }
    case jump:					/* direct jump */
      {
      STELM *res;
      
      DBUG_ORDERS("jump()\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      if (LABbt==GETORDERARGS(ord,n))		/* get label */
	{
	res=st_pop(ret);
	w=GETSTELMDATA(res,ret)->w;		/* restore stacks */
        a=GETSTELMDATA(res,ret)->a;
        t=GETSTELMDATA(res,ret)->t;
        r=GETSTELMDATA(res,ret)->r;
        ord=GETSTELMDATA(res,ret)->ord;
	if (0!=GETSTELMDATA(res,ret)->branch)	/* check for other branch */
	  SETORDERENTRY(ord,branch)=GETSTELMDATA(res,ret)->branch;
	clos_exe=(GETSTELMDATA(res,ret)->status&CLOS_EXE)/CLOS_EXE;
	new_params=(GETSTELMDATA(res,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	}
      else
        ord=find_label(GETNEXTORDER(ord),GETORDERARGS(ord,label));
      break;
      }
    case jcond2:				/* conditional jumps */
    case jcond:
    case jfalse:
    case jfalse2:
    case jtrue:
    case jtrue2:
      {
      STELM *res;
      ORDER *old_ord;
      RETURNADR *rt;
      FUNDESC *des;
      int dep=0;
      STELM *freturn;
      
      DBUG_ORDERS("jfalse\\jcond\\jcond2\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      old_ord=search_ret(ord);
      if (NULL==old_ord)			/* first call from here */
	{
	if (1==new_params)
	  backupoldjcond();
	if (3==GETORDERENTRY(ord,branch))	/* jcond finished */
	  {
	  SETORDERENTRY(ord,branch)=0;
          st_free(w);
          st_free(a);
          st_free(r);
          st_free(t);
          res=st_pop(ret);			/* gets return ord */
          w=GETSTELMDATA(res,ret)->w;		/* restore stacks */
          a=GETSTELMDATA(res,ret)->a;
          t=GETSTELMDATA(res,ret)->t;
          r=GETSTELMDATA(res,ret)->r;
          ord=GETSTELMDATA(res,ret)->ord;
	  if (0!=GETSTELMDATA(res,ret)->branch)
	    SETORDERENTRY(ord,branch)=GETSTELMDATA(res,ret)->branch;
	  clos_exe=(GETSTELMDATA(res,ret)->status&CLOS_EXE)/CLOS_EXE;
	  new_params=(GETSTELMDATA(res,ret)->status&NEW_PARAMS)/NEW_PARAMS;
          stelm_free(res);
          if BETAFUNCS(GETORDERENTRY(ord,command))
            {
             if (apply==GETORDERENTRY(ord,command))
	       {
	       res=st_pop(w);
	       des=GETSTELMDATA(res,desc);
	       }
             else
	       des=find_desc(GETFIRSTDESC(program_start),GETORDERARGS(ord,label));
	    if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
	      {
	      res=get_result(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
			     t,GETDESCENTRY(des,nfv));
	      for (i=0; i<GETDESCENTRY(des,nv); i++)
	        stelm_free(st_pop(w));
              w=st_push(w,res);
	      }
	    else
	      {
	      res=get_result(GETDESCENTRY(des,label),a,GETDESCENTRY(des,nv)-1,
			     t,GETDESCENTRY(des,nfv)/4);
	      stelm_free(st_pop(w));
	      for (i=0; i<GETDESCENTRY(des,nv)-1; i++)
	        stelm_free(st_pop(a));
              w=st_push(w,res);
	      }
	    ord=GETNEXTORDER(ord);
	    }
          }
	else
	  {
	  if (NULL==(rt=malloc(sizeof(RETURNADR))))
	    yyfail("%s","no memory for returnadress!");
	  rt->ord=ord;				/* remember enviroment of func */
	  rt->status=(clos_exe*CLOS_EXE)+(new_params*NEW_PARAMS);
	  rt->branch=0;
	  rt->w=w;
	  rt->a=a;
	  rt->t=t;
	  rt->r=r;
	  freturn=new_stelm(tp_returnadr,rt);
	  ret=st_push(ret,freturn);			/* returndata stored */
	  stelm_free(freturn);
	  if (NULL!=w)
	    w=st_make(w,GETTOPOFSTACK(w));
	  if (NULL!=a)
	    a=st_make(a,GETTOPOFSTACK(a));
	  if (NULL!=t)
	    t=st_make(t,GETTOPOFSTACK(t));
	  if (NULL!=r)
	    r=st_make(r,GETTOPOFSTACK(r));
	  res=st_pop(w);
	  if (tp_bool!=GETSTELMTYPE(res))		/* no bool found */
	    {
	    stelm_free(res);
	    freturn=getfunc_ret();			/* gets last function */
	    if (NULL!=freturn)
	      {
	      des=find_desc(GETFIRSTDESC(program_start),
			    freturn->dat.ret->ord->args.label);
	      if (apply==freturn->dat.ret->ord->command)
		{
		STACK *dummy;
		
		if (DT_CONDITIONAL==GETDESCENTRY(des,tag))
		  dummy=st_make(freturn->dat.ret->w,2);
		else
		  dummy=st_make(freturn->dat.ret->w,des->nv+1);
		st_pop(dummy);
		if ((DT_COMBINATOR==des->tag) || (DT_CASE==des->tag))
		  dep=insert_functype(des->label,dummy,des->nv,freturn->dat.ret->t,
				      des->nfv,&CONST_NONE,kn_sure);
		else
		  dep=insert_functype(des->label,a,des->nv-1,freturn->dat.ret->t,
				      des->nfv/4,&CONST_NONE,kn_sure);
		st_free(dummy);
		}
	      else
		dep=insert_functype(des->label,freturn->dat.ret->w,des->nv,
				    freturn->dat.ret->t,des->nfv,&CONST_NONE,kn_sure);
	      }
	    SETORDERENTRY(ord,types)=tp_none;
	    SETORDERENTRY(ord,typestouched)=1;
	    }
	  else
	    {
	    if (0==GETORDERENTRY(ord,typestouched))
	      {
	      SETORDERENTRY(ord,types)=tp_bool;
	      SETORDERENTRY(ord,typestouched)=1;
	      }
	    }
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
    	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    if (((gammabeta==GETORDERENTRY(ord,command)) ||
		 (gammacase==GETORDERENTRY(ord,command))) ||
		(Gamma==GETORDERENTRY(ord,command)))
	      {
	      r=freturn->dat.ret->t;
	      t=freturn->dat.ret->r;
	      }
	    else
	      {
	      r=freturn->dat.ret->r;
	      t=freturn->dat.ret->t;
	      }
	    st_pop(ret);
            break;
	    }
	  switch (GETORDERENTRY(ord,branch))
	    {
	    case 0:
	    case 2:
	      {
	      SETORDERENTRY(ord,branch)=GETORDERENTRY(ord,branch) | 1;
	      ord=GETNEXTORDER(ord);
	      break;
	      }
	    case 1:
	      {
	      SETORDERENTRY(ord,branch)=3;
	      ord=find_label(GETNEXTORDER(ord),GETORDERARGS(ord,label));
	      break;
	      }
	    case -1:
	      {
	      SETORDERENTRY(ord,branch)=2;
	      ord=find_label(GETNEXTORDER(ord),GETORDERARGS(ord,label));
	      break;
	      }
	    case -2:
	    case -3:
	      {						/* jcond with no result */
	      SETORDERENTRY(ord,branch)=0;		/* jcond finished */
	      w=st_push(w,&CONST_NONE);
	      ord=&CONST_RTF;				/* goto rtf */
	      break;
	      }
	    default:
	      {
	      yyfail("%s","illegal jcond construction!");
	      break;
	      }
	    }
	  }
	}
      else					/* was here before */
        {
	if (((3==GETORDERENTRY(ord,branch)) || (2==GETORDERENTRY(ord,branch))) && 
	    (old_ord==ord))
	  {
	  STELM *label;
	  STACK *dummy;
	  
	  label=st_pop(ret);
	  while (!BETAFUNCS(GETORDERENTRY(GETSTELMDATA(label,ret)->ord,command)))
	    {
	    st_free(GETSTELMDATA(label,ret)->w);
	    st_free(GETSTELMDATA(label,ret)->a);
	    st_free(GETSTELMDATA(label,ret)->r);
	    st_free(GETSTELMDATA(label,ret)->t);
	    SETORDERENTRY(GETSTELMDATA(label,ret)->ord,branch)=0;
	    stelm_free(label);
	    label=st_pop(ret);
	    }
	  dummy=findfcall(label);		/* searches for functioncall */
	  if (NULL!=dummy)
	    {
	    st_free(w);
	    clos_exe=(GETSTELMDATA(label,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(label,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    ord=GETSTELMDATA(label,ret)->ord;
	    if (0!=GETSTELMDATA(label,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(label,ret)->branch;
            w=GETSTELMDATA(label,ret)->w;
	    st_free(a);
	    a=GETSTELMDATA(label,ret)->a;
	    st_free(r);
	    st_free(t);
	    if (((gammabeta==GETORDERENTRY(ord,command)) ||
		 (gammacase==GETORDERENTRY(ord,command))) ||
		(Gamma==GETORDERENTRY(ord,command)))
	      {
	      r=GETSTELMDATA(label,ret)->t;
	      t=GETSTELMDATA(label,ret)->r;
	      }
	    else
	      {
	      r=GETSTELMDATA(label,ret)->r;
	      t=GETSTELMDATA(label,ret)->t;
	      }
	    }
	  else
	    {
	    ret=st_push(ret,label);
	    SETORDERENTRY(old_ord,branch)=-GETORDERENTRY(old_ord,branch);
	    st_free(w);
	    st_free(a);
	    st_free(r);
	    st_free(t);
	    while (old_ord!=(GETSTELMDATA((res=st_pop(ret)),ret)->ord))
	      {
	      if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command))
		{
		if ((gammabeta==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)) ||
		    (gammacase==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)) ||
		    (Gamma==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)))
		  {
		  STACK *d;
		  
		  d=GETSTELMDATA(res,ret)->t;
		  GETSTELMDATA(res,ret)->t=GETSTELMDATA(res,ret)->r;
		  GETSTELMDATA(res,ret)->r=d;
		  }
		func_dump=st_push(func_dump,res);
		}
	      else
		{
		SETORDERENTRY(GETSTELMDATA(res,ret)->ord,branch)=0;
		st_free(GETSTELMDATA(res,ret)->w);
		st_free(GETSTELMDATA(res,ret)->a);
		st_free(GETSTELMDATA(res,ret)->r);
		st_free(GETSTELMDATA(res,ret)->t);
		}
	      stelm_free(res);
	      }
	    clos_exe=(GETSTELMDATA(res,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(res,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    ord=GETSTELMDATA(res,ret)->ord;
	    if (0!=GETSTELMDATA(res,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(res,ret)->branch;
	    w=GETSTELMDATA(res,ret)->w;
	    a=GETSTELMDATA(res,ret)->a;
	    if (((gammabeta==GETORDERENTRY(ord,command)) ||
		 (gammacase==GETORDERENTRY(ord,command))) ||
		(Gamma==GETORDERENTRY(ord,command)))
	      {
	      r=GETSTELMDATA(res,ret)->t;
	      t=GETSTELMDATA(res,ret)->r;
	      }
	    else
	      {
	      r=GETSTELMDATA(res,ret)->r;
	      t=GETSTELMDATA(res,ret)->t;
	      }
	    stelm_free(res);
	    }
	  }
	else						/* standart returning */
	  {
	  FUNDESC *des;
	  STELM *result;
	  
          SETORDERENTRY(old_ord,branch)=-GETORDERENTRY(old_ord,branch);
	  st_free(w);
	  st_free(a);
	  st_free(r);
	  st_free(t);
	  while (old_ord!=(GETSTELMDATA((res=st_pop(ret)),ret)->ord))
	    {
	    if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command))
	      {
	      des=find_desc(GETFIRSTDESC(program_start),
			    res->dat.ret->ord->args.label);
	      result=get_result(des->label,res->dat.ret->w,des->nv,
				res->dat.ret->t,des->nfv);
	      if ((NULL!=result) && (tp_illegal!=result->type))
		break;
	      else
		{
		if ((gammabeta==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)) ||
		    (gammacase==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)) ||
		    (Gamma==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)))
		  {
		  STACK *d;
		  
		  d=GETSTELMDATA(res,ret)->t;
		  GETSTELMDATA(res,ret)->t=GETSTELMDATA(res,ret)->r;
		  GETSTELMDATA(res,ret)->r=d;
		  }
		func_dump=st_push(func_dump,res);
		}
	      }
	    else
	      {
	      SETORDERENTRY(GETSTELMDATA(res,ret)->ord,branch)=0;
	      st_free(GETSTELMDATA(res,ret)->w);
	      st_free(GETSTELMDATA(res,ret)->a);
	      st_free(GETSTELMDATA(res,ret)->r);
	      st_free(GETSTELMDATA(res,ret)->t);
	      }
	    stelm_free(res);
	    }
	  clos_exe=(GETSTELMDATA(res,ret)->status&CLOS_EXE)/CLOS_EXE;
	  new_params=(GETSTELMDATA(res,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	  w=GETSTELMDATA(res,ret)->w;
	  a=GETSTELMDATA(res,ret)->a;
	  r=GETSTELMDATA(res,ret)->r;
	  t=GETSTELMDATA(res,ret)->t;
	  if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command))
	    {
	    for (i=0; i<des->nv; i++)
	      st_pop(w);
	    w=st_push(w,result);
	    ord=GETSTELMDATA(res,ret)->ord->next;
	    }
	  else
	    ord=GETSTELMDATA(res,ret)->ord;
	  if (0!=GETSTELMDATA(res,ret)->branch)
	    SETORDERENTRY(ord,branch)=GETSTELMDATA(res,ret)->branch;
	  stelm_free(res);
	  }
	}
      break;
      }
    case freea:
      {
      DBUG_ORDERS("freea\t\t\tw:%d, a:%d, r:%d. t:%d, ret:%d");
      for (i=0; i<GETORDERARGS(ord,n); i++)
	stelm_free(st_pop(a));
      ord=GETNEXTORDER(ord);
      break;
      }
    case freer:
      {
      DBUG_ORDERS("freer\t\t\tw:%d, a:%d, r:%d. t:%d, ret:%d");
      for (i=0; i<GETORDERARGS(ord,n); i++)
	stelm_free(st_pop(r));
      ord=GETNEXTORDER(ord);
      break;
      }
    case freeswt:
      {
      STACK *d;
      
      DBUG_ORDERS("freeswt\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      for (i=0; i<GETORDERARGS(ord,n); i++)
	stelm_free(st_pop(t));
      d=r;
      r=t;
      t=d;
      ord=GETNEXTORDER(ord);
      break;
      }
    case rtm:
    case rtt:
      {
      STELM *arg;
      
      if (rtm==ord->command)
	{
	arg=st_pop(a);
        w=st_push(w,arg);				/* moveaw */
        stelm_free(arg);
	}
      }
    case rtf:
      {
      STELM res,*freturn;
      FUNDESC *des;
      KNOWLEDGE kn;
      int dep;
      
      DBUG_ORDERS("rtf\\rtm\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(&res,st_pop(w),1);
      st_free(w);
      st_free(a);
      st_free(r);
      st_free(t);
      freturn=getfunc_ret();			/* gets last function */
      if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(st_read(ret,0),ret)->ord,command))
	kn=kn_sure;
      else
	kn=kn_partly;
      if (NULL==freturn)			/* dump execution end */
	DBUG_RETURN((STELM *)NULL);
      if (apply==GETORDERENTRY(GETSTELMDATA(freturn,ret)->ord,command))
	{
	STACK *dummy;
	
	des=GETSTELMDATA((st_read(GETSTELMDATA(freturn,ret)->w,0)),desc);
	if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
	  {
	  dummy=st_make(GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv)+1);
	  					/* get copy of stack */
	  st_pop(dummy);			/* remove function from stack */
          dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv),
		              GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv),
			      &res,kn);
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    st_pop(ret);
	    break;
	    }
	  stelm_cpy(&res,get_result(GETDESCENTRY(des,label),dummy,
		    GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
				    GETDESCENTRY(des,nfv)),1);
	  }
	else
	  {
	  dummy=st_make(GETSTELMDATA(freturn,ret)->a,GETDESCENTRY(des,nv)-1);
	  					/* get copy of stack */
          dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv)-1,
			      GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)/4,
			      &res,kn);	  
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    st_pop(ret);
            break;
	    }
	  stelm_cpy(&res,get_result(GETDESCENTRY(des,label),dummy,
		    GETDESCENTRY(des,nv)-1,GETSTELMDATA(freturn,ret)->t,
		    GETDESCENTRY(des,nfv)/4),1);
	  }
	st_free(dummy);					/* free dummy stack */
	}
      else
	{
	des=find_desc(GETFIRSTDESC(program_start),
		      GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,label));
	dep=insert_functype(GETDESCENTRY(des,label),GETSTELMDATA(freturn,ret)->w,
			    GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
			    GETDESCENTRY(des,nfv),&res,kn);
	if (0!=dep)
	  {
	  return_to_call(freturn);
	  tis_state=1;
	  ord=freturn->dat.ret->ord;
	  w=freturn->dat.ret->w;
	  a=freturn->dat.ret->a;
	  if (((gammabeta!=GETORDERENTRY(ord,command)) &&
	       (gammacase!=GETORDERENTRY(ord,command))) &&
	      (Gamma!=GETORDERENTRY(ord,command))) 
	    {
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    }
	  else
	    {
	    r=freturn->dat.ret->t;
	    t=freturn->dat.ret->r;
	    }
	  st_pop(ret);
          break;
	  }
        stelm_cpy(&res,get_result(GETDESCENTRY(des,label),
		  GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv),
		  GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)),1);
	}
      if (0==GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,m))
	insert_restype(&res,GETSTELMDATA(freturn,ret)->ord);
      SETORDERARGS(GETSTELMDATA(freturn,ret)->ord,k)=-1; /* order executed */
      freturn=st_pop(ret);			/* restors former state */
      ord=GETSTELMDATA(freturn,ret)->ord;
      if (0!=GETSTELMDATA(freturn,ret)->branch)
	SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
      clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
      new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
      w=GETSTELMDATA(freturn,ret)->w;		/* restore stacks */
      a=GETSTELMDATA(freturn,ret)->a;
      t=GETSTELMDATA(freturn,ret)->t;
      r=GETSTELMDATA(freturn,ret)->r;
      stelm_free(freturn);
      if BETAFUNCS(GETORDERENTRY(ord,command))
        {
	for (i=0; i<GETDESCENTRY(des,nv); i++)
	  stelm_free(st_pop(w));
	if (apply==GETORDERENTRY(ord,command))
	  {
	  stelm_free(st_pop(w));
          w=st_push(w,&res);
	  if (0==GETORDERARGS(ord,m))
	    ord=GETNEXTORDER(ord);
	  }
	else
	  {
	  if (Gamma==ord->command)
	    {
	    STACK *dummy;
	    
	    dummy=r;
	    r=t;
	    t=dummy;
	    }
	  w=st_push(w,&res);
	  ord=GETNEXTORDER(ord);
	  }
	}
      break;
      }
    case rtp:
      {
      STELM *res,*freturn;
      FUNDESC *des;
      KNOWLEDGE kn;
      int dep;
      
      DBUG_ORDERS("rtp\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      res=find_address(GETORDERARGS(ord,desc));
      st_free(w);
      st_free(a);
      st_free(r);
      st_free(t);
      freturn=getfunc_ret();			/* gets last function */
      if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(st_read(ret,0),ret)->ord,command))
	kn=kn_sure;
      else
	kn=kn_partly;
      if (NULL==freturn)			/* dump execution end */
	DBUG_RETURN((STELM *)NULL);
      if (apply==GETORDERENTRY(GETSTELMDATA(freturn,ret)->ord,command))
	{
	STACK *dummy;
	
	des=GETSTELMDATA((st_read(GETSTELMDATA(freturn,ret)->w,0)),desc);
	if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
	  {
	  dummy=st_make(GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv)+1);
	  					/* get copy of stack */
	  st_pop(dummy);			/* remove function from stack */
          dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv),
			      GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv),
			      res,kn);
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    st_pop(ret);
            break;
	    }
	  stelm_cpy(res,get_result(GETDESCENTRY(des,label),dummy,
		    GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
				   GETDESCENTRY(des,nfv)),1);
	  }
	else
	  {
	  dummy=st_make(GETSTELMDATA(freturn,ret)->a,GETDESCENTRY(des,nv)-1);
	  					/* get copy of stack */
          dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv)-1,
			      GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)/4,
			      res,kn);	  
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    st_pop(ret);
            break;
	    }
	  stelm_cpy(res,get_result(GETDESCENTRY(des,label),dummy,
		    GETDESCENTRY(des,nv)-1,GETSTELMDATA(freturn,ret)->t,
				   GETDESCENTRY(des,nfv)/4),1);
	  }
	st_free(dummy);					/* free dummy stack */
	}
      else
	{
	des=find_desc(GETFIRSTDESC(program_start),
		      GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,label));
	dep=insert_functype(GETDESCENTRY(des,label),GETSTELMDATA(freturn,ret)->w,
			    GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
			    GETDESCENTRY(des,nfv),res,kn);
	if (0!=dep)
	  {
	  return_to_call(freturn);
	  tis_state=1;
	  ord=freturn->dat.ret->ord;
	  if (0!=GETSTELMDATA(freturn,ret)->branch)
	    SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	  clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	  new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	  w=freturn->dat.ret->w;
	  a=freturn->dat.ret->a;
	  r=freturn->dat.ret->r;
	  t=freturn->dat.ret->t;
	  st_pop(ret);
          break;
	  }
	stelm_cpy(res,get_result(GETDESCENTRY(des,label),
		  GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv),
		  GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)),1);
	}
      if (0==GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,m))
	insert_restype(res,GETSTELMDATA(freturn,ret)->ord);
      SETORDERARGS(GETSTELMDATA(freturn,ret)->ord,k)=-1;  /* order executed */
      freturn=st_pop(ret);			/* restors former state */
      ord=GETSTELMDATA(freturn,ret)->ord;
      if (0!=GETSTELMDATA(freturn,ret)->branch)
	SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
      clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
      new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
      w=GETSTELMDATA(freturn,ret)->w;		/* restore stacks */
      a=GETSTELMDATA(freturn,ret)->a;
      t=GETSTELMDATA(freturn,ret)->t;
      r=GETSTELMDATA(freturn,ret)->r;
      stelm_free(freturn);
      if BETAFUNCS(GETORDERENTRY(ord,command))
	{
        for (i=0; i<GETDESCENTRY(des,nv); i++)
	  stelm_free(st_pop(w));
	if (apply==GETORDERENTRY(ord,command))
	  {
	  stelm_free(st_pop(w));
          w=st_push(w,res);
	  if (0==GETORDERARGS(ord,m))
	    ord=GETNEXTORDER(ord);
	  }
	else
	  {
	  w=st_push(w,res);
	  ord=GETNEXTORDER(ord);
	  }
	}
      break;
      }
    case rtc_pf:
      {
      STELM res={tp_prim,{tp_none,NULL},{NULL}}, *freturn;
      FUNDESC *des;
      KNOWLEDGE kn;
      int dep;
      
      DBUG_ORDERS("rtc_pf\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      if (ISIA(GETORDERARGS(ord,primf)))
        {
        w=st_push(w,&CONST_NONE);
        ord=GETNEXTORDER(ord);
        }
      else
        {
        res.dat.prim=GETORDERARGS(ord,n);		/* which prim. function */
        st_free(w);
        st_free(a);
        st_free(r);
        st_free(t);
        freturn=getfunc_ret();			/* gets last function */
        if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(st_read(ret,0),ret)->ord,command))
          kn=kn_sure;
        else
          kn=kn_partly;
        if (NULL!=freturn)			/* no dump execution end */
          {
          if (apply==GETORDERENTRY(GETSTELMDATA(freturn,ret)->ord,command))
            {
            STACK *dummy;
            
            des=GETSTELMDATA((st_read(GETSTELMDATA(freturn,ret)->w,0)),desc);
            if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
              {
              dummy=st_make(GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv)+1);
              /* get copy of stack */
              st_pop(dummy);			/* remove function from stack */
              dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv),
                                  GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv),
                                  &res,kn);
              if (0!=dep)
                {
                return_to_call(freturn);
                tis_state=1;
                ord=freturn->dat.ret->ord;
                if (0!=GETSTELMDATA(freturn,ret)->branch)
                  SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
                clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
                new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
                w=freturn->dat.ret->w;
                a=freturn->dat.ret->a;
                r=freturn->dat.ret->r;
                t=freturn->dat.ret->t;
                st_pop(ret);
                break;
                }
              stelm_cpy(&res,get_result(GETDESCENTRY(des,label),dummy,
                                        GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
                                        GETDESCENTRY(des,nfv)),1);
              }
            else
              {
              dummy=st_make(GETSTELMDATA(freturn,ret)->a,GETDESCENTRY(des,nv)-1);
              /* get copy of stack */
              dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv)-1,
                                  GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)/4,
                                  &res,kn);	  
              if (0!=dep)
                {
                return_to_call(freturn);
                tis_state=1;
                ord=freturn->dat.ret->ord;
                if (0!=GETSTELMDATA(freturn,ret)->branch)
                  SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
                clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
                new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
                w=freturn->dat.ret->w;
                a=freturn->dat.ret->a;
                r=freturn->dat.ret->r;
                t=freturn->dat.ret->t;
                st_pop(ret);
                break;
                }
              stelm_cpy(&res,get_result(GETDESCENTRY(des,label),dummy,
                                        GETDESCENTRY(des,nv)-1,GETSTELMDATA(freturn,ret)->t,
                                        GETDESCENTRY(des,nfv)/4),1);
              }
            st_free(dummy);					/* free dummy stack */
            }
          else
            {
            des=find_desc(GETFIRSTDESC(program_start),
                          GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,label));
            dep=insert_functype(GETDESCENTRY(des,label),GETSTELMDATA(freturn,ret)->w,
                                GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
                                GETDESCENTRY(des,nfv),&res,kn);
            if (0!=dep)
              {
              return_to_call(freturn);
              tis_state=1;
              ord=freturn->dat.ret->ord;
              if (0!=GETSTELMDATA(freturn,ret)->branch)
                SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
              clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
              new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
              w=freturn->dat.ret->w;
              a=freturn->dat.ret->a;
              r=freturn->dat.ret->r;
              t=freturn->dat.ret->t;
              st_pop(ret);
              break;
              }
            stelm_cpy(&res,get_result(GETDESCENTRY(des,label),
                                      GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv),
                                      GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)),1);
            }
          if (0==GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,m))
            insert_restype(&res,GETSTELMDATA(freturn,ret)->ord);
          SETORDERARGS(GETSTELMDATA(freturn,ret)->ord,k)=-1;  /* order executed */
          }
        freturn=st_pop(ret);			/* restors former state */
        ord=GETSTELMDATA(freturn,ret)->ord;
        if (0!=GETSTELMDATA(freturn,ret)->branch)
          SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
        clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
        new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
        w=GETSTELMDATA(freturn,ret)->w;		/* restore stacks */
        a=GETSTELMDATA(freturn,ret)->a;
        t=GETSTELMDATA(freturn,ret)->t;
        r=GETSTELMDATA(freturn,ret)->r;
        stelm_free(freturn);
        if BETAFUNCS(GETORDERENTRY(ord,command))
          {
          for (i=0; i<GETDESCENTRY(des,nv); i++)
            stelm_free(st_pop(w));
          if (apply==GETORDERENTRY(ord,command))
            {
            stelm_free(st_pop(w));
            w=st_push(w,&res);
            if (0==GETORDERARGS(ord,m))
              ord=GETNEXTORDER(ord);
            }
          else
            {
            w=st_push(w,&res);
            ord=GETNEXTORDER(ord);
            }
          }
        }
      break;
      }
    case rtc_i:
      {
      STELM res={tp_int,{tp_none,NULL},{NULL}},*freturn;
      FUNDESC *des;
      KNOWLEDGE kn;
      int dep;
      
      DBUG_ORDERS("rtc_i\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      st_free(w);
      st_free(a);
      st_free(r);
      st_free(t);
      freturn=getfunc_ret();			/* gets last function */
      if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(st_read(ret,0),ret)->ord,command))
	kn=kn_sure;
      else
	kn=kn_partly;
      if (NULL!=freturn)			/* no dump execution end */
	{
	if (apply==GETORDERENTRY(GETSTELMDATA(freturn,ret)->ord,command))
	  {
	  STACK *dummy;
	  
	  des=GETSTELMDATA((st_read(GETSTELMDATA(freturn,ret)->w,0)),desc);
	  if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
	    {
	    dummy=st_make(GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv)+1);
	    /* get copy of stack */
	    st_pop(dummy);			/* remove function from stack */
	    dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv),
				GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv),
				&res,kn);
	    if (0!=dep)
	      {
	      return_to_call(freturn);
	      tis_state=1;
	      ord=freturn->dat.ret->ord;
	      if (0!=GETSTELMDATA(freturn,ret)->branch)
	        SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	      clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	      new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	      w=freturn->dat.ret->w;
	      a=freturn->dat.ret->a;
	      r=freturn->dat.ret->r;
	      t=freturn->dat.ret->t;
	      st_pop(ret);
	      break;
	      }
	    stelm_cpy(&res,get_result(GETDESCENTRY(des,label),dummy,
				      GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
				      GETDESCENTRY(des,nfv)),1);
	    }
	  else
	    {
	    dummy=st_make(GETSTELMDATA(freturn,ret)->a,GETDESCENTRY(des,nv)-1);
	    /* get copy of stack */
	    dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv)-1,
				GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)/4,
				&res,kn);	  
	    if (0!=dep)
	      {
	      return_to_call(freturn);
	      tis_state=1;
	      ord=freturn->dat.ret->ord;
	      if (0!=GETSTELMDATA(freturn,ret)->branch)
	        SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	      clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	      new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	      w=freturn->dat.ret->w;
	      a=freturn->dat.ret->a;
	      r=freturn->dat.ret->r;
	      t=freturn->dat.ret->t;
	      st_pop(ret);
	      break;
	      }
	    stelm_cpy(&res,get_result(GETDESCENTRY(des,label),dummy,
				      GETDESCENTRY(des,nv)-1,GETSTELMDATA(freturn,ret)->t,
				      GETDESCENTRY(des,nfv)/4),1);
	    }
	  st_free(dummy);					/* free dummy stack */
	  }
	else
	  {
	  des=find_desc(GETFIRSTDESC(program_start),
			GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,label));
	  dep=insert_functype(GETDESCENTRY(des,label),GETSTELMDATA(freturn,ret)->w,
			      GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
			      GETDESCENTRY(des,nfv),&res,kn);
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    st_pop(ret);
            break;
	    }
	  stelm_cpy(&res,get_result(GETDESCENTRY(des,label),
				    GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv),
				    GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)),1);
	  }
	if (0==GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,m))
	  insert_restype(&res,GETSTELMDATA(freturn,ret)->ord);
        SETORDERARGS(GETSTELMDATA(freturn,ret)->ord,k)=-1;  /* order executed */
	}
      freturn=st_pop(ret);			/* restors former state */
      ord=GETSTELMDATA(freturn,ret)->ord;
      if (0!=GETSTELMDATA(freturn,ret)->branch)
	SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
      clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
      new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
      w=GETSTELMDATA(freturn,ret)->w;		/* restore stacks */
      a=GETSTELMDATA(freturn,ret)->a;
      t=GETSTELMDATA(freturn,ret)->t;
      r=GETSTELMDATA(freturn,ret)->r;
      stelm_free(freturn);
      if BETAFUNCS(GETORDERENTRY(ord,command))
	{
        for (i=0; i<GETDESCENTRY(des,nv); i++)
	  stelm_free(st_pop(w));
	if (apply==GETORDERENTRY(ord,command))
	  {
	  stelm_free(st_pop(w));
          w=st_push(w,&res);
	  if (0==GETORDERARGS(ord,m))
	    ord=GETNEXTORDER(ord);
	  }
	else
	  {
	  w=st_push(w,&res);
	  ord=GETNEXTORDER(ord);
	  }
	}
      break;
      }
    case rtc_b:
      {
      STELM res={tp_bool,{tp_none,NULL},{NULL}},*freturn;
      FUNDESC *des;
      KNOWLEDGE kn;
      int dep;
      
      DBUG_ORDERS("rtc_i\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      st_free(w);
      st_free(a);
      st_free(r);
      st_free(t);
      freturn=getfunc_ret();			/* gets last function */
      if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(st_read(ret,0),ret)->ord,command))
	kn=kn_sure;
      else
	kn=kn_partly;
      if (NULL==freturn)			/* dump execution end */
	DBUG_RETURN((STELM *)NULL);
      if (apply==GETORDERENTRY(GETSTELMDATA(freturn,ret)->ord,command))
	{
	STACK *dummy;
	
	des=GETSTELMDATA((st_read(GETSTELMDATA(freturn,ret)->w,0)),desc);
	if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
	  {
	  dummy=st_make(GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv)+1);
	  					/* get copy of stack */
	  st_pop(dummy);			/* remove function from stack */
          dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv),
			      GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv),
			      &res,kn);
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    st_pop(ret);
            break;
	    }
	  stelm_cpy(&res,get_result(GETDESCENTRY(des,label),dummy,
		    GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
		    GETDESCENTRY(des,nfv)),1);
	  }
	else
	  {
	  dummy=st_make(GETSTELMDATA(freturn,ret)->a,GETDESCENTRY(des,nv)-1);
	  					/* get copy of stack */
          dep=insert_functype(GETDESCENTRY(des,label),dummy,GETDESCENTRY(des,nv)-1,
			      GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)/4,
			      &res,kn);	  
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    st_pop(ret);
            break;
	    }
	  stelm_cpy(&res,get_result(GETDESCENTRY(des,label),dummy,
		    GETDESCENTRY(des,nv)-1,GETSTELMDATA(freturn,ret)->t,
		    GETDESCENTRY(des,nfv)/4),1);
	  }
	st_free(dummy);					/* free dummy stack */
	}
      else
	{
	des=find_desc(GETFIRSTDESC(program_start),
		      GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,label));
	dep=insert_functype(GETDESCENTRY(des,label),GETSTELMDATA(freturn,ret)->w,
			    GETDESCENTRY(des,nv),GETSTELMDATA(freturn,ret)->t,
			    GETDESCENTRY(des,nfv),&res,kn);
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    ord=freturn->dat.ret->ord;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    r=freturn->dat.ret->r;
	    t=freturn->dat.ret->t;
	    st_pop(ret);
            break;
	    }
	stelm_cpy(&res,get_result(GETDESCENTRY(des,label),
		  GETSTELMDATA(freturn,ret)->w,GETDESCENTRY(des,nv),
	          GETSTELMDATA(freturn,ret)->t,GETDESCENTRY(des,nfv)),1);
	}
      if (0==GETORDERARGS(GETSTELMDATA(freturn,ret)->ord,m))
	insert_restype(&res,GETSTELMDATA(freturn,ret)->ord);
      SETORDERARGS(GETSTELMDATA(freturn,ret)->ord,k)=-1;  /* order executed */
      freturn=st_pop(ret);			/* restors former state */
      ord=GETSTELMDATA(freturn,ret)->ord;
      if (0!=GETSTELMDATA(freturn,ret)->branch)
	SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
      clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
      new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
      w=GETSTELMDATA(freturn,ret)->w;		/* restore stacks */
      a=GETSTELMDATA(freturn,ret)->a;
      t=GETSTELMDATA(freturn,ret)->t;
      r=GETSTELMDATA(freturn,ret)->r;
      stelm_free(freturn);
      if BETAFUNCS(GETORDERENTRY(ord,command))
	{
        for (i=0; i<GETDESCENTRY(des,nv); i++)
	  stelm_free(st_pop(w));
	if (apply==GETORDERENTRY(ord,command))
	  {
	  stelm_free(st_pop(w));
          w=st_push(w,&res);
	  if (0==GETORDERARGS(ord,m))
	    ord=GETNEXTORDER(ord);
	  }
	else
	  {
	  w=st_push(w,&res);
	  ord=GETNEXTORDER(ord);
	  }
	}
      break;
      }
    case ext:
      {
      STELM *res,*d;
      
      DBUG_ORDERS("ext\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      res=st_pop(w);
      if ((NULL!=t) && (0!=GETTOPOFSTACK(t)) && (0==GETTOPOFSTACK(ret)))
	fprintf(stderr,"exit with elements on T-stack!\n");
      if ((NULL!=r) && (0!=GETTOPOFSTACK(r)) && (0==GETTOPOFSTACK(ret)))
	fprintf(stderr,"exit with elements on R-stack!\n");
      if ((NULL!=a) && (0!=GETTOPOFSTACK(a)) && (0==GETTOPOFSTACK(ret)))
	fprintf(stderr,"exit with elements on A-stack!\n");
      if ((NULL!=w) && (0!=GETTOPOFSTACK(w)) && (0==GETTOPOFSTACK(ret)))
	fprintf(stderr,"exit with >1 elements on W-stack!\n");
      insert_restype(res,ord);
      ord->types=TYPES2LONG(GETSTELMTYPE(res),tp_none,tp_none,tp_none);
      if ((NULL!=ret) && (0<GETTOPOFSTACK(ret)))	/* still sometings to do */
	{
	d=st_pop(ret);
	if JCONDS(GETORDERENTRY(GETSTELMDATA(d,ret)->ord,command))
	  						/* command == jcond ? */
	  {
	  if (3==GETORDERENTRY(GETSTELMDATA(d,ret)->ord,branch))
	    						/* jcond already executed */
	    DBUG_RETURN(res);
	  ord=GETSTELMDATA(d,ret)->ord;			/* restore prev. state */
	  if (0!=GETSTELMDATA(d,ret)->branch)
	    SETORDERENTRY(ord,branch)=GETSTELMDATA(d,ret)->branch;
	  clos_exe=(GETSTELMDATA(d,ret)->status&CLOS_EXE)/CLOS_EXE;
	  new_params=(GETSTELMDATA(d,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	  w=GETSTELMDATA(d,ret)->w;
	  a=GETSTELMDATA(d,ret)->a;
	  r=GETSTELMDATA(d,ret)->r;
	  t=GETSTELMDATA(d,ret)->t;
	  }
	else 
	  {
	  if (ext!=GETORDERENTRY(GETSTELMDATA(d,ret)->ord,command))
	    yyfail("%s","illegal returnadress at ext!\n");	/* error */
	  else
	    DBUG_RETURN(res);
	  }
	}
      else						/* the end */
        DBUG_RETURN(res);
      break;
      }
    case label:
    case hashargs:
    case hashtildeargs:
    case hashrestype:
    case hashsetref:
      ord=GETNEXTORDER(ord);
      break;
    case end:
      DBUG_ORDERS("end\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      break;
    case tguard:
    case matcharbs:
    case matchbool:
    case matchin:
    case matchprim:
    case matchstr:
    case matchlist:
    case matchint:
    case matcharb:
      {
      STELM *tow,*freturn,*res;
      RETURNADR *rt;
      ORDER *old_ord;
      FUNDESC *des;
      int dep=0;
      
      DBUG_ORDERS("matchX//tguard\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      old_ord=search_ret(ord);			/* first call form here ? */
      if (NULL==old_ord)
	{
	if (1==new_params)			/* call with new params */
	  backupoldjcond();
	if (3==GETORDERENTRY(ord,branch))
	  {
	  SETORDERENTRY(ord,branch)=0;
	  st_free(w);
	  st_free(a);
	  st_free(t);
	  st_free(r);
	  res=st_pop(ret);
          w=GETSTELMDATA(res,ret)->w;		/* restore stacks */
          a=GETSTELMDATA(res,ret)->a;
          t=GETSTELMDATA(res,ret)->t;
          r=GETSTELMDATA(res,ret)->r;
          ord=GETSTELMDATA(res,ret)->ord;
          if (0!=GETSTELMDATA(res,ret)->branch)
	    SETORDERENTRY(ord,branch)=GETSTELMDATA(res,ret)->branch;
	  clos_exe=(GETSTELMDATA(res,ret)->status&CLOS_EXE)/CLOS_EXE;
	  new_params=(GETSTELMDATA(res,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	  stelm_free(res);
          if BETAFUNCS(GETORDERENTRY(ord,command))
            {
            if (apply==GETORDERENTRY(ord,command))
	      {
	      res=st_pop(w);
	      des=GETSTELMDATA(res,desc);
	      }
            else
	      des=find_desc(GETFIRSTDESC(program_start),GETORDERARGS(ord,label));
	    if (DT_CONDITIONAL!=GETDESCENTRY(des,tag))
	      {
	      res=get_result(GETDESCENTRY(des,label),w,GETDESCENTRY(des,nv),
			     t,GETDESCENTRY(des,nfv));
	      for (i=0; i<GETDESCENTRY(des,nv); i++)
	        stelm_free(st_pop(w));
              w=st_push(w,res);
	      }
	    else
	      {
	      res=get_result(GETDESCENTRY(des,label),a,GETDESCENTRY(des,nv)-1,
			     t,GETDESCENTRY(des,nfv)/4);
	      stelm_free(st_pop(w));
	      for (i=0; i<GETDESCENTRY(des,nv)-1; i++)
	        stelm_free(st_pop(a));
	      w=st_push(w,res);
	      }
	    ord=GETNEXTORDER(ord);
	    }
	  }
	else					/* at least one branch to go */
	  {
	  if (NULL==(rt=malloc(sizeof(RETURNADR))))
	    yyfail("%s","no memory for returnadress!");
	  rt->ord=ord;
	  rt->status=(clos_exe*CLOS_EXE)+(new_params*NEW_PARAMS);
	  rt->branch=0;
	  rt->w=w;
	  rt->a=a;
	  rt->t=t;
	  rt->r=r;
	  tow=new_stelm(tp_returnadr,rt);
	  ret=st_push(ret,tow);
	  stelm_free(tow);
	  if (NULL!=w)
	    w=st_make(w,GETTOPOFSTACK(w));	/* save complete STACK */
	  if (NULL!=a)
	    a=st_make(a,GETTOPOFSTACK(a));	/* save complete STACK */
	  if (NULL!=t)
	    t=st_make(t,GETTOPOFSTACK(t));	/* save complete STACK */
	  if (NULL!=r)
	    r=st_make(r,GETTOPOFSTACK(r));	/* save complete STACK */
          tow=st_read(w,0);		/* get pointer of top element of stack w */
	  if (0==GETORDERENTRY(ord,typestouched))
	    {
	    SETORDERENTRY(ord,types)=TYPES2LONG(GETSTELMTYPE(tow),tp_none,
						tp_none,tp_none); /* internal types */
	    SETORDERENTRY(ord,typestouched)=1;
	    }
	  else
	    if (GETSTELMTYPE(tow)!=GETTYPE(0,GETORDERENTRY(ord,types)))
	      SETORDERENTRY(ord,types)=GETORDERENTRY(ord,types) & SETTYPE(0,tp_none);
	  if (((tp_clos==GETSTELMTYPE(tow)) || (tp_var==GETSTELMTYPE(tow))) ||
	      (tp_none==GETSTELMTYPE(tow)))		/* undecided possible */
	    {
	    freturn=getfunc_ret();
	    des=find_desc(GETFIRSTDESC(program_start),freturn->dat.ret->ord->args.label);
	    if (apply==freturn->dat.ret->ord->command)
	      {
	      STACK *dummy;
	      
	      dummy=st_make(freturn->dat.ret->w,des->nv+1);
	      st_pop(dummy);
	      if ((DT_COMBINATOR==des->tag) || (DT_CASE==des->tag))
		dep=insert_functype(des->label,dummy,des->nv,freturn->dat.ret->t,
				    des->nfv,&CONST_NONE,kn_sure);
	      else
		dep=insert_functype(des->label,a,des->nv-1,freturn->dat.ret->t,
				    des->nfv/4,&CONST_NONE,kn_sure);
	      st_free(dummy);
	      }
	    else
	      dep=insert_functype(des->label,freturn->dat.ret->w,des->nv,
			      freturn->dat.ret->t,des->nfv,&CONST_NONE,kn_sure);
	    }
	  if (0!=dep)
	    {
	    return_to_call(freturn);
	    tis_state=1;
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    w=freturn->dat.ret->w;
	    a=freturn->dat.ret->a;
	    if (((gammabeta==GETORDERENTRY(ord,command)) ||  /* only gammacase */
		 (gammacase==GETORDERENTRY(ord,command))) ||
		(Gamma==GETORDERENTRY(ord,command)))
	      {
	      t=freturn->dat.ret->r;
	      r=freturn->dat.ret->t;
	      }
	    else
	      {
	      t=freturn->dat.ret->t;
	      r=freturn->dat.ret->r;
	      }
	    st_pop(ret);
	    break;					/* end matchX */
	    }
	  switch (GETORDERENTRY(ord,command))
	    {
	    case matchlist:
	    case matcharb:
	      {
	      if ((tp_list!=GETSTELMTYPE(tow)) &&	/* definitly fail */
		  (tp_none!=GETSTELMTYPE(tow)))
		SETORDERENTRY(ord,branch)=1;
	      break;
	      }
	    case matchin:
	    case matchstr:
	    case matcharbs:
	      {
	      if ((tp_string!=GETSTELMTYPE(tow)) &&	/* definitly fail */
		  (tp_none!=GETSTELMTYPE(tow)))
		SETORDERENTRY(ord,branch)=1;
	      break;
	      }
	    case matchprim:
	      {
	      if ((tp_none!=GETSTELMTYPE(tow)) &&	/* definitly fail */
		  (((ISCHAR(GETORDERARGS(ord,n))) && (tp_char!=GETSTELMTYPE(tow))) ||
		   ((ISPRF(GETORDERARGS(ord,n))) && (tp_prim!=GETSTELMTYPE(tow)))))
		SETORDERENTRY(ord,branch)=1;
	      break;
	      }
	    case matchint:
	      {
	      if ((tp_int!=GETSTELMTYPE(tow)) &&	/* definitly fail */
		  (tp_none!=GETSTELMTYPE(tow)))
		SETORDERENTRY(ord,branch)=1;
	      break;
	      }
	    case matchbool:
	      {
	      if ((tp_bool!=GETSTELMTYPE(tow)) &&	/* definitly fail */
		  (tp_none!=GETSTELMTYPE(tow)))
		SETORDERENTRY(ord,branch)=1;
	      break;
	      }
	    case tguard:
	      {
	      if ((tp_bool!=GETSTELMTYPE(tow)) &&	/* definitly fail */
		  (tp_none!=GETSTELMTYPE(tow)) &&
		  (tp_var!=GETSTELMTYPE(tow)) &&
		  (tp_clos!=GETSTELMTYPE(tow)))
		SETORDERENTRY(ord,branch)=1;
	      break;
	      }
	    default:
	      {
	      yyfail("%s","illegal command for match!");
	      break;
	      }
	    }
	  if ((tp_clos!=GETSTELMTYPE(tow)) && (tp_var!=GETSTELMTYPE(tow)))
	    {					/* NOT undecided */
	    switch (GETORDERENTRY(ord,branch))
	      {
	      case 0:
	      case 2:
		{
		SETORDERENTRY(ord,branch)=GETORDERENTRY(ord,branch) | 1;
		ord=GETNEXTORDER(ord);
		break;
		}
	      case 1:
		{
		SETORDERENTRY(ord,branch)=3;
		if (NULL==(tow=gotofail()))
		  ord=find_label(GETNEXTORDER(ord),GETORDERARGS(ord,label));
		else
		  {
		  ord=tow->dat.ret->ord;
		  if (0!=GETSTELMDATA(tow,ret)->branch)
		    SETORDERENTRY(ord,branch)=GETSTELMDATA(tow,ret)->branch;
		  clos_exe=(GETSTELMDATA(tow,ret)->status&CLOS_EXE)/CLOS_EXE;
		  new_params=(GETSTELMDATA(tow,ret)->status&NEW_PARAMS)/NEW_PARAMS;
		  w=tow->dat.ret->w;
		  a=tow->dat.ret->a;
		  t=tow->dat.ret->t;
		  r=tow->dat.ret->r;
		  }
		break;
		}
	      case -1:
		{
		SETORDERENTRY(ord,branch)=2;
		ord=find_label(GETNEXTORDER(ord),GETORDERARGS(ord,label));
		break;
		}
	      case -2:
		{
		SETORDERENTRY(ord,branch)=0;
		w=st_push(w,&CONST_NONE);
		ord=&CONST_RTF;
		break;
		}
	      default:
		{
		yyfail("%s","illegal match construction!");
		break;
		}
	      }	    
	    }
	  else					/* undecided */
	    {
	    ord=freturn->dat.ret->ord;
	    if (0!=GETSTELMDATA(freturn,ret)->branch)
	      SETORDERENTRY(ord,branch)=GETSTELMDATA(freturn,ret)->branch;
	    clos_exe=(GETSTELMDATA(freturn,ret)->status&CLOS_EXE)/CLOS_EXE;
	    new_params=(GETSTELMDATA(freturn,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	    st_free(w);
	    w=freturn->dat.ret->w;
	    st_free(a);
	    a=freturn->dat.ret->a;
	    st_free(t);
	    t=freturn->dat.ret->t;
	    st_free(r);
	    r=freturn->dat.ret->r;
	    for (i=0;i<des->nv;i++)
	      st_pop(w);
	    w=st_push(w,&CONST_NONE);
	    ord=GETNEXTORDER(ord);
	    }
	  }
	}
      else						/* was here before */
	{
	if (((3==GETORDERENTRY(ord,branch)) || (2==GETORDERENTRY(ord,branch))) && 
	    (old_ord==ord))
	  {
	  STELM *label;
	  
	  label=st_pop(ret);
	  while ((GETSTELMDATA(label,ret)->ord)!=old_ord)
	    {
	    st_free(GETSTELMDATA(label,ret)->w);
	    st_free(GETSTELMDATA(label,ret)->a);
	    st_free(GETSTELMDATA(label,ret)->r);
	    st_free(GETSTELMDATA(label,ret)->t);
	    SETORDERENTRY(GETSTELMDATA(label,ret)->ord,branch)=0;
	    stelm_free(label);
	    label=st_pop(ret);
	    }
	  label=st_pop(ret);
	  clos_exe=(GETSTELMDATA(label,ret)->status&CLOS_EXE)/CLOS_EXE;
	  new_params=(GETSTELMDATA(label,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	  st_free(w);
	  w=GETSTELMDATA(label,ret)->w;
	  st_free(a);
	  a=GETSTELMDATA(label,ret)->a;
	  st_free(r);
	  r=GETSTELMDATA(label,ret)->r;
	  st_free(t);
	  t=GETSTELMDATA(label,ret)->t;
	  ord=GETSTELMDATA(label,ret)->ord;
	  if (0!=GETSTELMDATA(label,ret)->branch)
	    SETORDERENTRY(ord,branch)=GETSTELMDATA(label,ret)->branch;
	  if (((gammabeta==GETORDERENTRY(ord,command)) ||
		 (gammacase==GETORDERENTRY(ord,command))) ||
		(Gamma==GETORDERENTRY(ord,command)))
	    {
	    STACK *dummy;
	    
	    dummy=t;
	    t=r;
	    r=dummy;
	    }
	  }
	else						/* standart returning */
	  {
	  FUNDESC *des;
	  STELM *result;
	  
          SETORDERENTRY(old_ord,branch)=-GETORDERENTRY(old_ord,branch);
	  st_free(w);
	  st_free(a);
	  st_free(r);
	  st_free(t);
	  while (old_ord!=(GETSTELMDATA((res=st_pop(ret)),ret)->ord))
	    {
	    if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command))
	      {
	      des=find_desc(GETFIRSTDESC(program_start),
			    res->dat.ret->ord->args.label);
	      result=get_result(des->label,res->dat.ret->w,des->nv,
				res->dat.ret->t,des->nfv);
	      if ((NULL!=result) && (tp_illegal!=result->type))
		break;
	      else
		{
		if ((gammabeta==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)) ||
		    (gammacase==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)) ||
		    (Gamma==GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command)))
		  {
		  STACK *d;
		  
		  d=GETSTELMDATA(res,ret)->t;
		  GETSTELMDATA(res,ret)->t=GETSTELMDATA(res,ret)->r;
		  GETSTELMDATA(res,ret)->r=d;
		  }
		func_dump=st_push(func_dump,res);
		}
	      }
	    else
	      {
	      SETORDERENTRY(GETSTELMDATA(res,ret)->ord,branch)=0;
	      st_free(GETSTELMDATA(res,ret)->w);
	      st_free(GETSTELMDATA(res,ret)->a);
	      st_free(GETSTELMDATA(res,ret)->r);
	      st_free(GETSTELMDATA(res,ret)->t);
	      }
	    stelm_free(res);
	    }
	  clos_exe=(GETSTELMDATA(res,ret)->status&CLOS_EXE)/CLOS_EXE;
	  new_params=(GETSTELMDATA(res,ret)->status&NEW_PARAMS)/NEW_PARAMS;
	  w=GETSTELMDATA(res,ret)->w;
	  a=GETSTELMDATA(res,ret)->a;
	  r=GETSTELMDATA(res,ret)->r;
	  t=GETSTELMDATA(res,ret)->t;
	  if BETAFUNCS(GETORDERENTRY(GETSTELMDATA(res,ret)->ord,command))
	    {
	    for (i=0; i<des->nv; i++)
	      st_pop(w);
	    w=st_push(w,result);
	    ord=GETSTELMDATA(res,ret)->ord->next;
	    }
	  else
	    ord=GETSTELMDATA(res,ret)->ord;
	  if (0!=GETSTELMDATA(res,ret)->branch)
	    SETORDERENTRY(ord,branch)=GETSTELMDATA(res,ret)->branch;
	  stelm_free(res);
	  }	
	}
      break;
      }
    case mkaframe:
      {
      int i;
      
      DBUG_ORDERS("mkaframe\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      for (i=0;i<GETORDERARGS(ord,n);i++)
	a=st_push(a,&CONST_NONE);
      ord=GETNEXTORDER(ord);
      break;
      }
    case mkbtframe:
      {
      int i;
      
      DBUG_ORDERS("mkbtframe\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      for (i=0;i<GETORDERARGS(ord,n);i++)
	r=st_push(r,&CONST_NONE);
      ord=GETNEXTORDER(ord);
      break;
      }
    case mkwframe:
      {
      int i;
      
      DBUG_ORDERS("mkwframe\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      for (i=0;i<GETORDERARGS(ord,n);i++)
	w=st_push(w,&CONST_NONE);
      w=st_push(w,st_read(w,GETORDERARGS(ord,n)));
      ord=GETNEXTORDER(ord);
      break;
      }      
    case saveptr:
      {
      DBUG_ORDERS("saveptr\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(w,GETORDERARGS(ord,n)),st_read(w,1),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case nestlist:
      {
      DBUG_ORDERS("nestlist\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(w,1),st_read(w,0),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case advance:
    case endlist:
      {
      DBUG_ORDERS("advance//endlist\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      ord=GETNEXTORDER(ord);
      break;
      }
    case decw:
    case deca:
    case dect:
    case decr:
    case tdecw:
    case tdeca:
    case tdect:
    case tdecr:
      {
      DBUG_ORDERS("[t]dec(w|a|t|r)\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      ord=GETNEXTORDER(ord);
      break;
      }
    case incw:
    case inca:
    case inct:
    case incr:
    case tincw:
    case tinca:
    case tinct:
    case tincr:
      {
      DBUG_ORDERS("[t]inc(w|a|t|r)\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      ord=GETNEXTORDER(ord);
      break;
      }
    case dereference:
      {
      STELM *stelm;
      
      DBUG_ORDERS("dereference\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      if ((tp_list==GETSTELMTYPE(st_read(w,1))) || 
	  (tp_string==GETSTELMTYPE(st_read(w,1))))
	{
	if (tp_empty==st_read(w,1)->subtype.type)
	  stelm=&CONST_NONE;
	else
	  {
	  if (NULL!=st_read(w,1)->subtype.next)
	    stelm=new_stelm(st_read(w,1)->subtype.type,
			    subtype_cpy(st_read(w,1)->subtype.next));
	  else
	    stelm=new_stelm(st_read(w,1)->subtype.type,NULL);
	  }
	}
      else
	stelm=&CONST_NONE;
      stelm_cpy(st_read(w,0),stelm,1);
      insert_restype(stelm,ord);
      stelm_free(stelm);
      ord=GETNEXTORDER(ord);
      break;
      }
    case pick:
      {
      DBUG_ORDERS("pick\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(w,0),st_read(w,GETORDERARGS(ord,n)),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case startsubl:
      {
      DBUG_ORDERS("startsubl\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(r,GETORDERARGS(ord,n)),st_read(w,0),1);
      stelm_cpy(st_read(r,GETORDERARGS(ord,m)),st_read(w,1),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case endsubl:
      {
      DBUG_ORDERS("endsubl\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(r,GETORDERARGS(ord,n)),st_read(w,1),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case restoreptr:
      {
      DBUG_ORDERS("restoreptr\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(w,1),st_read(w,GETORDERARGS(ord,n)),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case bindsubl:
      {
      DBUG_ORDERS("bindsubl\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(a,GETORDERARGS(ord,l)),st_read(r,GETORDERARGS(ord,n)),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case rmwframe:
      {
      int i;
      
      DBUG_ORDERS("rmwframe\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      for (i=0; i<GETORDERARGS(ord,n);i++)
	st_pop(w);
      ord=GETNEXTORDER(ord);
      break;
      }
    case rmbtframe:
      {
      int i;
      
      DBUG_ORDERS("rmbtframe\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      for (i=0; i<GETORDERARGS(ord,n);i++)
	st_pop(r);
      ord=GETNEXTORDER(ord);
      break;
      }
    case drop:
      {
      DBUG_ORDERS("drop\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      st_pop(w);
      ord=GETNEXTORDER(ord);
      break;
      }
    case binds:
    case bind:
      {
      DBUG_ORDERS("bind[s]\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(a,GETORDERARGS(ord,n)),st_read(w,0),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case initbt:
      {
      STELM *stelm;
      
      DBUG_ORDERS("initbt\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(r,GETORDERARGS(ord,m)),st_read(w,1),1);
      stelm_cpy(st_read(r,GETORDERARGS(ord,k)),st_read(w,1),1);
      stelm_cpy(st_read(r,GETORDERARGS(ord,l)),st_read(w,0),1);
      stelm=new_stelm(st_read(w,0)->subtype.type,st_read(w,0)->subtype.next);
      stelm_cpy(st_read(r,GETORDERARGS(ord,j)),stelm,1);
      stelm_free(stelm);
      ord=GETNEXTORDER(ord);
      break;
      }
    case atstart:
    case atend:
      {
      DBUG_ORDERS("atend//atstart\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      ord=GETNEXTORDER(ord);
      break;
      }
    case savebt:
      {
      DBUG_ORDERS("savebt\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(r,GETORDERARGS(ord,n)),st_read(w,1),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case restorebt:
      {
      DBUG_ORDERS("restorebt\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      stelm_cpy(st_read(w,1),st_read(r,GETORDERARGS(ord,n)),1);
      ord=GETNEXTORDER(ord);
      break;
      }
    case fetch:
      {
      STELM *stelm;
      
      DBUG_ORDERS("fetch\t\t\tw:%d, a:%d, r:%d, t:%d, ret:%d");
      if ((tp_list==GETSTELMTYPE(st_read(w,0))) || 
	  (tp_string==GETSTELMTYPE(st_read(w,0))))
	stelm=new_stelm(st_read(w,0)->subtype.type,
			subtype_cpy(st_read(w,0)->subtype.next));
      else
	stelm=new_stelm(tp_none,NULL);
      stelm_cpy(st_read(w,0),stelm,1);
      insert_restype(stelm,ord);
      stelm_free(stelm);
      ord=GETNEXTORDER(ord);
      break;
      }
    case pushh:
    case poph:
    case wait:
    case dist:
    case distb:
    case distend:
    case count:
    case msdistend:
    case msnodist:
      {
      ord=GETNEXTORDER(ord);
      break;
      }
    default:
      {
      fprintf(stderr,"unknown command!");
      exit(0);
      break;
      }
    }
  }
DBUG_RETURN((STELM *)NULL);				/* code never reached */
}

/************************************************************************/
/* controls execution of asmt-code for typechecking			*/
/* (used by tis)							*/
/************************************************************************/
void execution()
{
STELM *goaltype,*dummy;
RETURNADR NULL_ret={NULL,NULL,NULL,NULL,NULL};
STACK *d;
FUNDESC *des;

DBUG_ENTER("execution");
des=GETFIRSTDESC(program_start);
while (NULL!=des)
  {
  if (max_desc_adr<des->address)
    max_desc_adr=des->address;
  des=des->next;
  }
ord=GETFIRSTFUNCTION(program_start)->order;
w=(STACK *)NULL;
a=(STACK *)NULL;
t=(STACK *)NULL;
r=(STACK *)NULL;
ret=(STACK *)NULL;
goaltype=exe_func();
DBUG_PRINT("ORDERS",("executing dump functions ..."));
while ((NULL!=func_dump) && (0<GETTOPOFSTACK(func_dump)))
  {
  dummy=st_pop(func_dump);
  if ((-1!=GETORDERARGS(GETSTELMDATA(dummy,ret)->ord,k)) ||
      (MUST_EXE==GETORDERENTRY(GETSTELMDATA(dummy,ret),status)))
    {
    NULL_ret.ord=&CONST_EXT;
    d=st_push(NULL,new_stelm(tp_returnadr,&NULL_ret));
    ord=GETSTELMDATA(dummy,ret)->ord;
    if (0!=GETSTELMDATA(dummy,ret)->branch)
      SETORDERENTRY(ord,branch)=GETSTELMDATA(dummy,ret)->branch;
    w=GETSTELMDATA(dummy,ret)->w;
    a=GETSTELMDATA(dummy,ret)->a;
    clos_exe=(GETSTELMDATA(dummy,ret)->status&CLOS_EXE)/CLOS_EXE;
    new_params=(GETSTELMDATA(dummy,ret)->status&NEW_PARAMS)/NEW_PARAMS;
    t=GETSTELMDATA(dummy,ret)->t;
    r=GETSTELMDATA(dummy,ret)->r;
    ret=d;
    dummy=exe_func();
    }
  }
DBUG_VOID_RETURN;
}

/************************************************************************/
/* Is a kind of MAIN for the tis project				*/
/* (used by main)							*/
/************************************************************************/
int tis()
{
DBUG_ENTER("tis");
execution();
DBUG_RETURN(0);
}
