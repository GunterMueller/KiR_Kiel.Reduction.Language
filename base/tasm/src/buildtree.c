/***********************************************************************/
/* Builds syntaxtree for ASM~-program, used by tasm2c.y.               */
/***********************************************************************/
/* Last change: 17.10.94                                               */
/***********************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "dbug.h"
#include "mytypes.h"
#include "buildtree.h"
#include "main.h"
#include "tiny.h"
#include "error.h"

/***********************************************************************/
/* Frees allocated memory of an order.                                 */
/***********************************************************************/

static void free_order(ORDER *d)
{
  DBUG_ENTER("free_order");
  if (NULL!=d->code) free(d->code);
  if (GETORDERARGS(d, label)) free(GETORDERARGS(d, label));
  if (GETORDERARGS(d, ret)) free(GETORDERARGS(d, ret));
  free(d);
  DBUG_VOID_RETURN;
}

/***********************************************************************/
/* Deletes a given order. (uses free_order())                          */
/***********************************************************************/

void delete_order(ORDER *d)
{
  ORDER *p, *n;
  FUNCTION *f;
  
  DBUG_ENTER("delete_order");
  p=d->prev;
  n=d->next;
  if (NULL==p)
  {
    f=program_start->function;
    while (NULL!=f)
    {
      if (f->order==d)
        break;
      f=f->next;
    }
    f->order=n;
    f->order->prev=NULL;
  }
  else
  {
    if (NULL!=n)
      n->prev=p;
    p->next=n;
  }
  free_order(d);
  DBUG_VOID_RETURN;
}

/***********************************************************************/
/* searches a function handle by name                                  */
/***********************************************************************/

FUNCTION *find_function(char *s)
{
  FUNCTION *fun = GETFIRSTFUNCTION(program_start);

  DBUG_ENTER("find_function");
  while ((NULL!=fun) && (0!=strcmp(s,GETNAME(fun))))
    fun=GETNEXTFUNCTION(fun);
  DBUG_RETURN(fun);
}

/***********************************************************************/
/* Deletes a whole order list and puts another instead of the killed   */
/* one into the tree                                                   */
/***********************************************************************/
void overwrite_func(char *overwrite_me, ORDER *new_order_list)
{
  FUNCTION *fun = GETFIRSTFUNCTION(program_start);
  ORDER *ord, *help;
  
  DBUG_ENTER("overwrite_func");
  while ((NULL!=fun) && (0!=strcmp(overwrite_me, GETNAME(fun))))
    fun=GETNEXTFUNCTION(fun);
  ord = GETFIRSTORDER(fun);  
  while (NULL != ord)
  {
    help = GETNEXTORDER(ord);
    delete_order(ord);
    ord = help;
  }
  SETFIRSTORDER(fun) = new_order_list;

  DBUG_VOID_RETURN;
}
/***********************************************************************/
/* Builds a new structure for an ASM~-date    in the parsetree and     */
/* returns pointer to that structure.                                  */
/***********************************************************************/
/* The ellipse takes the different types of the data                   */
/* like real, mat, vect, string, list, ...                             */
/***********************************************************************/

DATANODE *new_data(DATATAG whatami, int adr, ...)
{
  va_list ap;
  DATANODE *help;

  DBUG_ENTER("new_data");
  help=(DATANODE *)malloc(sizeof(DATANODE));
  if (NULL==help)
    yyfail("couldn't allocate memory");
  help->next=NULL;
  help->tag=whatami;
  help->address = adr;
  va_start(ap, adr);
  switch (whatami)
  {
  case d_real:
    help->u.x=va_arg(ap, double);
    break;
  case d_list:
  case d_string:
    help->u.v.size = va_arg(ap, int);
    help->u.v.data = va_arg(ap, int *);
    break;
  case d_name:
    help->u.v.size = va_arg(ap, int);
    help->u.v.data = (int *)va_arg(ap, char *);
    break;
  case d_mat:
  case d_vect:
  case d_tvect:
    help->u.w.rows = va_arg(ap, int);    
    help->u.w.cols = va_arg(ap, int);
    help->u.w.m.idata = va_arg(ap, int *);
    help->u.w.tag = va_arg(ap, DATATAG); 
    break;
  default:
    yyfail("internal error: illegal datatype request");
    ;
  } /* SWITCH */
  
  DBUG_RETURN(help);  
}

/***********************************************************************/
/* Concatenates an ASM~-data to an existing datalist and               */
/* returns pointer to that list.                                       */
/***********************************************************************/
      
DATANODE * conc_data(DATANODE *list, DATANODE *data)
{
  DATANODE * help;
  
  DBUG_ENTER("conc_data");
  help=list;
  while (NULL!=help->next) 
    help=help->next;
  help->next=data;
  DBUG_RETURN(list);
}  

/***********************************************************************/
/* Builds a new structure for an ASM~-command in the parsetree and     */
/* returns pointer to that structure.                                  */
/***********************************************************************/
/* The ellipse takes the different parameters of the different ASM~-   */
/* commands like integers, booleans, pointers, names.                  */
/***********************************************************************/

ORDER *new_order(COMMAND add_me, ...)
{
  va_list ap;
  char * s;
  ORDER *help;

  DBUG_ENTER("new_order");
  
  help=(ORDER *)malloc(sizeof(ORDER));
  if (NULL==help)
    yyfail("couldn't allocate memory");
  help->command=add_me;
  help->opti=FALSE;
  help->typestouched=0;
  help->next=NULL;
  help->prev=NULL;
  help->code=NULL;
  help->branch=0;
  help->args.n=0;
  help->args.m=0;
  help->args.k=0;
  help->args.l=0;
  help->args.j=0;
  help->args.x=0;
  help->args.label=NULL;
  help->args.ret=NULL;
  help->args.hash_str=NULL;
  help->args.argtp=NULL;
  help->args.desc=NULL;
  help->args.primf=-1;
  help->types=NOTYPE;
  
  va_start(ap, add_me);
  switch(add_me)
  {
/* Commands without any arguments are not treated */
/* Commands with only one integer argument */
  case pushcw_i:
  case pushaw:
  case pushtw:
  case pushar:
  case pushtr:
  case pushh:
  case count:
  case freeswt:
  case freea:
  case freer:
  case freet:
  case freew:
  case snap:
  case apply:
  case rtc_i:
  case mklist:  
  case mkilist:  
  case mkap:  
/* Commands with only one bool argument, treated as integer */
  case pushcw_b:
  case rtc_b:
  case rtc_pf:
  case stflip:
  case wait:
  {
    help->args.n=va_arg(ap, int);
    break;
  }
/* Commands with only one pointer argument */
  case pushw_p:
  case pushr_p:
  case rtp:
  {
    help->args.desc = (void *)va_arg(ap, int);
    break;
  }
/* Commands with two integer arguments */
  case pushcw_pf:
  {
    help->args.n=va_arg(ap, PRIMF);  /* primfunc */
    help->types=va_arg(ap, int);     /* types */
    help->args.m=va_arg(ap, int);    /* arity */
    break;
  }
  case mkdclos: 
  case mkiclos: 
  {
    help->args.n=va_arg(ap, int);
    help->args.m=va_arg(ap, int);
    break;
  }
/* Commands with three integer arguments */
  case mkgaclos:
  case mkgsclos:
  case mkbclos:
  case mksclos:
  case mkcclos:
  {
    help->args.n=va_arg(ap, int);
    help->args.m=va_arg(ap, int);
    if (ISASM_TILDE())
      help->args.k=va_arg(ap, int);
    break;
  }
    
/* Commands with a label as an argument */
  case jfalse:
  case jtrue:
  case jcond:
  {
    SETORDERARGS(help, n)=va_arg(ap, labeltype);
    s = va_arg(ap, char *);
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    help->types = va_arg(ap, int);
    break;
  }
  case pushret:
  case jump:
  case beta:
  case Gamma:
  case gammabeta:
  case tail:
  {
    SETORDERARGS(help, n)=va_arg(ap, labeltype);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    break;
  }
  case jfalse2:
  case jtrue2:
  case jcond2:
  {
    SETORDERARGS(help, n)=va_arg(ap, labeltype);
    SETORDERARGS(help, m)=va_arg(ap, labeltype);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, ret) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, ret), s);
    help->types = va_arg(ap, int);
    break;
  }
  case hashargs:
  case hashtildeargs:
  case hashrestype:
  {
    help->args.n=va_arg(ap, int);               /* no of args */
    help->args.argtp=va_arg(ap, TP_TYPES *);     /* args */
    break;
  }
  case hashsetref:
  {
    help->args.n=va_arg(ap, int);               /* new mode */
    break;
  }

/* Commands with a primitive function as an argument */    
  case delta1:
  case delta2:
  case delta3:
  case delta4:
  case intact:
  {
    help->args.primf=va_arg(ap, PRIMF);
    help->types=va_arg(ap, int);
    break;
  }
  case label:
  {
    SETORDERARGS(help, n)=va_arg(ap, int);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    break;
  }
  case stack_op:
  case ris_stack_op:
  {
    int n = va_arg(ap, int);
    int m = va_arg(ap, int);
    int k = va_arg(ap, int);
    char *s=va_arg(ap, char *);
    
    SETORDERARGS(help, n) = n;   /* # of a entries to kill */
    SETORDERARGS(help, m) = m;   /* # of w entries to kill */
    SETORDERARGS(help, k) = k;   /* # of vars to kill */
    help->code=(char *)malloc(strlen(s)+1);
    if (NULL==help->code) 
      yyfail("couldn't allocate memory");
    strcpy(help->code,s);
    break;
  }
  case code_ok:
  {
    char *s=va_arg(ap, char *);
    
    help->code=(char *)malloc(strlen(s)+1);
    if (NULL==help->code)
      yyfail("couldn't allocate memory");
    strcpy(help->code,s);
    break;
  }
  case movear:
  case moveaw:
  case movetr:
  case movetw:
  case pushaux:
  case uses_aux_var:
  case rtf:
  case rtm:
  case rtt:
  case ext:
  case end:
  case poph:
  case distend:
  case msdistend:
  case msnodist:
    break;
  case inca:
  case incw:
  case incr:
  case inct:
  case tinca:
  case tincw:
  case tincr:
  case tinct:
    SETORDERARGS(help, n) = va_arg(ap, int);
    SETORDERARGS(help, m) = va_arg(ap, int);
    break;
  case deca:
  case decr:
  case dect:
  case decw:
  case tdeca:
  case tdecw:
  case tdecr:
  case tdect:
  case killa:
  case killr:
  case killt:
  case killw:
  case tkilla:
  case tkillw:
  case tkillr:
  case tkillt:
    SETORDERARGS(help, n) = va_arg(ap, int);
    break;
  /* instructions for ASm-PM */
  case gammacase:
  case Case:
    SETORDERARGS(help, n)=va_arg(ap, labeltype);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    break;
  case dereference:
  case drop:
  case endlist:
  case fetch:
  case nestlist:
    break;
  case advance:
  case bind:
  case binds:
  case endsubl:
  case mkaframe:
  case mkbtframe:
  case mkwframe:
  case pick:
  case restorebt:
  case restoreptr:
  case rmbtframe:
  case rmwframe:
  case savebt:
  case saveptr:
    SETORDERARGS(help, n) = va_arg(ap, int);
    break;
  case atend:
  case atstart:
    SETORDERARGS(help, n) = va_arg(ap, int);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    break;
  case bindsubl:
    SETORDERARGS(help, n) = va_arg(ap, int);
    SETORDERARGS(help, m) = va_arg(ap, int);
    SETORDERARGS(help, k) = va_arg(ap, int);
    SETORDERARGS(help, l) = va_arg(ap, int);
    break;
  case initbt:
    SETORDERARGS(help, n) = va_arg(ap, int);
    SETORDERARGS(help, m) = va_arg(ap, int);
    SETORDERARGS(help, k) = va_arg(ap, int);
    SETORDERARGS(help, l) = va_arg(ap, int);
    SETORDERARGS(help, j) = va_arg(ap, int);
    break;
  case matcharb:
  case matcharbs:
  case matchbool:
  case matchin:
  case matchint:
  case matchlist:
  case matchstr:
    SETORDERARGS(help, n) = va_arg(ap, int);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, ret) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, ret), s);
    SETORDERENTRY(help, types) = va_arg(ap, int);
    break;
  case matchprim:
    help->args.n=va_arg(ap, PRIMF);  /* primfunc */
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, ret) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, ret), s);
    break;
  case mkcase:
    SETORDERARGS(help, desc) = (void *)va_arg(ap, int);
    break;
  case startsubl:
    SETORDERARGS(help, n) = va_arg(ap, int);
    SETORDERARGS(help, m) = va_arg(ap, int);
    break;
  case tguard:
    s = va_arg(ap, char *); 
    if ((SETORDERARGS(help, label) = (char *)malloc(strlen(s)+1)) == NULL)
      yyfail("memory allocation failure");
    strcpy(SETORDERARGS(help, label), s);
    SETORDERARGS(help, desc) = (void *)va_arg(ap, int);
    SETORDERARGS(help, n) = va_arg(ap, int);
    SETORDERARGS(help, m) = va_arg(ap, int);
    SETORDERARGS(help, l) = va_arg(ap, int);
    break;
  /* ASM-FF */
  case mkframe:
  case Inter:
    SETORDERARGS(help, n) = va_arg(ap, int);
    break;
  case mkslot:
    break;
  case dist:
  case distb:
    SETORDERARGS(help, label) = va_arg(ap, char *);
    SETORDERARGS(help, ret) = va_arg(ap, char *);
    SETORDERARGS(help, n) = va_arg(ap, int);
    SETORDERARGS(help, m) = va_arg(ap, int);
    SETORDERARGS(help, k) = va_arg(ap, int);
    SETORDERARGS(help, l) = va_arg(ap, int);
    break;
  default:
    yyfail("internal error: unknown order received");
    break;    
}

  va_end(ap);
  DBUG_RETURN(help);
}



/***********************************************************************/
/* Includes an ASM~-command into an existing commandlist and           */
/* returns pointer to that list.                                       */
/***********************************************************************/
      
void insert_order_list(ORDER *insert_me, ORDER *before_me)
{
  ORDER *help;
  FUNCTION *fhelp;
  
  DBUG_ENTER("insert_order_list");
  help=insert_me;
  if (NULL==before_me->prev)
  {
    while (NULL!=help->next) help=help->next;
    help->next=before_me;
    before_me->prev=help;
    fhelp=program_start->function;
    while (NULL!=fhelp)
    {
      if (fhelp->order==before_me)
        break;
      fhelp=fhelp->next;
    }
    fhelp->order=insert_me;
  }
  else
  {
    before_me->prev->next=insert_me;
    insert_me->prev=before_me->prev;
    while (NULL!=insert_me->next) insert_me=insert_me->next;
    insert_me->next=before_me;
    before_me->prev=insert_me;  
  }
  DBUG_VOID_RETURN;
}  

/***********************************************************************/
/* Concatenates an ASM~-command to an existing commandlist and         */
/* returns pointer to that list.                                       */
/***********************************************************************/
      
ORDER *conc_order(ORDER *list, ORDER *order)
{
  ORDER *help;
  
  DBUG_ENTER("conc_order");
  help=list;
  
  while (NULL!=help->next) 
    help=help->next;
  order->prev=help;
  help->next=order;
  DBUG_RETURN(list);
}  

/***********************************************************************/
/* Builds a new structure for an ASM~-function in the parsetree and    */
/* returns pointer to that structure.                                  */
/***********************************************************************/

FUNCTION *new_function(const char *name, ORDER *order)
{
  FUNCTION *help;
  
  DBUG_ENTER("new_function");
  help=(FUNCTION *)malloc(sizeof(FUNCTION));
  if (NULL==help)
    yyfail("couldn't allocate memory");
  help->next=NULL;
  help->inlined = 1;
  if ((help->name = (char *)malloc(strlen(name)+1)) == NULL)
    yyfail("memory allocation failure");
  strcpy(help->name,name);
  if (strcmp(name, "goal") == 0)
    help->desc = program_start->desc;
  else
    if ((help->desc = find_desc(program_start->desc, help->name)) == NULL)
      yyerror("illegal function identifier '%s'", name);
  help->order=order;
  DBUG_RETURN(help);
}

/***********************************************************************/
/* Includes an ASM~-function into an existing commandlist and          */
/* returns pointer to that list.                                       */
/***********************************************************************/

FUNCTION *conc_function(FUNCTION *list, FUNCTION *add_me)
{
  FUNCTION *help;
  
  DBUG_ENTER("conc_function");
  help=list;
    while (NULL!=help->next) help=help->next;
  help->next=add_me;
  DBUG_RETURN(list);
}

/***********************************************************************/
/* Builds a new structure for an ASM~-functiondescriptor in the and    */
/* parsetree returns pointer to that structure.                        */
/***********************************************************************/

FUNDESC *new_desc(DESCTAG tag, int adr, int n, int m, int graph_adr, const char *label)
{
  FUNDESC *help;
  
  DBUG_ENTER("new_desc");
  help=(FUNDESC *)malloc(sizeof(FUNDESC));
  if (NULL==help)
    yyfail("couldn't allocate memory");
  help->next=NULL;
  help->tag=tag;
  help->address = adr;
  help->nfv=n;
  help->nv=m;
  help->graph = graph_adr;
  if ((SETDESCENTRY(help, label) = (char *)malloc(strlen(label)+1)) == NULL)
    yyfail("memory allocation failure");
  strcpy(SETDESCENTRY(help, label), label);
  DBUG_RETURN(help);
}

/***********************************************************************/
/* Includes an ASM~-descriptor into an existing descriptorlist and     */
/* returns pointer to that list.                                       */
/***********************************************************************/
      
FUNDESC *conc_desc(FUNDESC *list, FUNDESC *desc)
{
  FUNDESC *help;

  DBUG_ENTER("conc_desc");
  help=list;
  while (NULL!=help->next) help=help->next;
  help->next=desc;
  DBUG_RETURN(list);
}  

/***********************************************************************/
/* Concatenates functiondescriptorlist and functionlist and            */
/* returns pointer to that resulting structure.                        */
/***********************************************************************/

PROGRAM *conc_desc_fun(FUNDESC *desc, DATANODE * data, FUNCTION *func)
{
  PROGRAM *help;
  
  DBUG_ENTER("conc_desc_fun");
  help=(PROGRAM *)malloc(sizeof(PROGRAM));
  if (NULL==help)
    yyfail("couldn't allocate memory");
  help->function=func;
  help->data=data;
  help->desc=desc;
  DBUG_RETURN(help);
}
