/**********************************************************************************
***********************************************************************************
**
**   File        : marco-retrans.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 21.5.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for retransformation
**                 from Orel0 to source code
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

/*********************************************************************************/
/********** INTERNAL PROGRAM INFORMATIONS FOR THIS MODULE ************************/
/*********************************************************************************/

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dbug.h"

#include "marco-options.h"
#include "marco-misc.h"
#include "marco-create.h"
#include "marco-retrans.h"
#include "marco-prnode.h"
#include "marco-print.h"

#include "rstelem.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define DEF_BUFFER_SIZE		10240
#define DEF_BUFFER_ADD		1024

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

#define ACT_ELEM (AS[as]&(~F_EDIT))

#define CONSTRUCTOR_NAME(elem) ((elem&F_CONSTR_NAME)>>O_CONSTR_NAME)

#define SET_RT_TYPE(type) (type)

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

static NODE *MK_EXPR(void);                               /* Forward declaration */
extern void print_NODE(NODE *node);

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char *BUFFER=NULL;            /* Global string/identifier/number - buffer */
static int BUFFER_SIZE=0;                               /* Size of global buffer */

static int ERROR=0;                                         /* Global error flag */
static STACKELEM *AS=NULL;                               /* Actuell stackelement */
static int as=0;                               /* offset to actuell stackelement */
static int nr_elems=0;                 /* Number of elements to process on stack */
static int PATTERN_MODE=0;                       /* Actual expression is pattern */
static int PATTERN_STRING_MODE=0;                /* Actual expression is pattern */
static int MATRIX_MODE=0;                                         /* Scan matrix */

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static char *TEST_BUFFER(int size)
{ /* Resize buffer if needed *****************************************************/
 if (size>=BUFFER_SIZE) {                                     /* Resize buffer ? */
  BUFFER_SIZE=((size/DEF_BUFFER_ADD)+1)*DEF_BUFFER_ADD+BUFFER_SIZE;
  BUFFER=(char *)realloc(BUFFER,BUFFER_SIZE);
  DBUG_ASSERT(BUFFER);                                      /* Test reallocation */
 }
 return (BUFFER);                                       /* Return buffer pointer */
} /*******************************************************************************/

static void BACK_ELEM(void)
{ /* Set pointer to element one step back ****************************************/
 nr_elems++;                                        /* One element more on stack */
 AS--;                                                 /* Stackpointer minus one */
} /*******************************************************************************/

static int NEXT_ELEM(void)
{ /* Set pointers to next element ************************************************/
 nr_elems=nr_elems-as-1;                          /* Decrease number of elements */
 as=0;                                                              /* No offset */
 if (nr_elems==0) return 0;                                      /* End of stack */
 else if (nr_elems<0) {                   /* Read more elements than available ? */
  fprintf(stderr,"RE_TRANS ERROR : Read beyond scope of stack !\n");
  ERROR++;                                          /* Increase number of errors */
  return (0);                                                 /* Stop processing */
 }
 AS=AS+as+1;                                            /* Increase stackpointer */
 return (1);                        /* Okay, more elements are on programstack ! */
} /*******************************************************************************/

static NODE *MK_NEXT_EXPR(void)
{ /* Make next expression node ***************************************************/
 NEXT_ELEM();
 return (MK_EXPR());
} /*******************************************************************************/

/* Make internal node representation functions (MK-functions) ********************/

static NODE *MK_Ident(void)
{ /* Make a internal node for an identifier **************************************/
 NODE *node;
 int i=0;
 STACKELEM elem;
 DBUG_ENTER("MK_Ident");

 node=ALLOCNODE();
 node->type=SET_RT_TYPE(NT_IDENTIFIER|NT_ATOM);
 node->info=EDIT_DUMMY;

 for (;;) {                                                      /* loop forever */
  if (NEXT_ELEM()==0) break;                                    /* Stack EMPTY ! */
  BUFFER=TEST_BUFFER(i);
  elem=ACT_ELEM;
 
  if ((elem&P_LET1)==LET1) {                   /* Letter, but not the last one ? */
   BUFFER[i]=VALUE(elem);
   i++; /* Increase number of characters */ 
  } else if ((elem&P_LET1)==LET0) {                             /* Last letter ? */
   BUFFER[i]=VALUE(elem);
   BUFFER[i+1]='\0';                            /* Set nullbyte at end of string */
   i++; /* Increase number of characters */ 
   break;                                   /* Last character found, end of loop */
  } else {                                         /* Error : unexpected element */
   fprintf(stderr,"ERROR : unexpected element in Identifier !\n");
  }
 }

 node->len=i;                                  /* set string (identifier) length */
 node->U.str=(char *)malloc(i+1);                   /* Space for string constant */
 memcpy(node->U.str,BUFFER,i+1);                                  /* Copy string */

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Number(void)
{ /* Make a internal node for a number *******************************************/
 NODE *node;
 int i=0;
 STACKELEM elem;
 DBUG_ENTER("MK_Number");

 node=ALLOCNODE();
 node->type=SET_RT_TYPE(NT_NUMBER|NT_ATOM);
 node->info=EDIT_DUMMY;

 if (CONSTRUCTOR_NAME(ACT_ELEM)==_MINUS) {
  BUFFER[0]='~'; /* Store minus sign */
  i=1;
 }

 for (;;) {                                                      /* loop forever */
  if (NEXT_ELEM()==0) break;                                    /* Stack EMPTY ! */
  BUFFER=TEST_BUFFER(i);
  elem=ACT_ELEM;
 
  if ((elem&P_LET1)==DIG1) {                    /* Digit, but not the last one ? */
   BUFFER[i]=VALUE(elem);
   i++; /* Increase number of characters */ 
  } else if ((elem&P_LET1)==DIG0) {                              /* Last digit ? */
   if (VALUE(elem)!='.') {                               /* Last digit a point ? */
    BUFFER[i]=VALUE(elem);
    i++; /* Increase number of characters */ 
   }
   BUFFER[i]='\0';                              /* Set nullbyte at end of string */
   break;                                   /* Last character found, end of loop */
  } else {                                         /* Error : unexpected element */
   fprintf(stderr,"ERROR : unexpected element in number !\n");
  }
 }

 node->len=i;                                  /* set string (identifier) length */
 node->U.str=(char *)malloc(i+1);                   /* Space for string constant */
 memcpy(node->U.str,BUFFER,i+1);                                  /* Copy string */

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Define(void)
{ /* Make node for a define construct ********************************************/
 NODE *node,                                     /* result of define constructor */
      *N,                                                           /* temp node */
      *FDL=NULL;                                      /* function definiton list */
 NODE **FNames=NULL;                                   /* Array of functionnames */
 NODE **Paras=NULL;                               /* Array of function-parameter */
 NODE **Bodies=NULL;                                 /* Array of function-bodies */
 int i,j,len;
 STACKELEM elem;                                          /* actual stackelement */
 DBUG_ENTER("MK_Define");

 NEXT_ELEM();                                     /* Ignore result (just a hack) */
 elem=ACT_ELEM;                          /* First element of function definition */
 if (CONSTRUCTOR_NAME(elem)!=_LIST) {                         /* List expected ! */
  fprintf(stderr,"ERROR : No functionnames at beginning of letrec !\n");
  DBUG_RETURN((NODE *)NULL);
 }

 i=0;
 len=VALUE(elem);                                 /* Get number of functionnames */
 FNames=(NODE **)malloc(sizeof(NODE *)*len);
 while (i<len) {                                    /* Collect all functionnames */
  FNames[i]=MK_Ident();                                      /* Get functionname */
  i++;
 }

 NEXT_ELEM();                                     /* Ignore result (just a hack) */
 elem=ACT_ELEM;                          /* First element of function definition */
 if (CONSTRUCTOR_NAME(elem)!=_LIST) {                         /* List expected ! */
  fprintf(stderr,"ERROR : No functiondefinitions in letrec !\n");
  DBUG_RETURN((NODE *)NULL);
 }

 i=0;
 len=VALUE(elem);                        /* Get number of functionparameterlists */
 Paras=(NODE **)malloc(sizeof(NODE *)*len);
 Bodies=(NODE **)malloc(sizeof(NODE *)*len);
 while (i<len) {                                  /* Collect all parameter lists */
  NEXT_ELEM();                                    /* Ignore result (just a hack) */
  elem=ACT_ELEM;                         /* First element of function definition */

  if (CONSTRUCTOR_NAME(elem)!=_SUB) {                          /* No parameter ? */
   N=NULL;                                                       /* No arguments */
  } else {                                      /* Make arguments/parameter list */
   j=VALUE(elem)-2;
   N=udi_Mk_ListHead(MK_Ident());                              /* First argument */
   while (j>0) {                                          /* Parse all arguments */
    N=udi_Mk_ListTail(N,MK_Ident());                            /* Next argument */
    j--;
   }
  }
 
  Paras[i]=N;                                    /* Parameterlist for function i */
  if (N!=NULL) Bodies[i]=MK_NEXT_EXPR(); /* Functionbody for function i */ 
  else Bodies[i]=MK_EXPR();                       /* Functionbody for function i */

  i++;                                                          /* Next function */
 }
 
 FDL=NULL;
 i=0;                                                /* Generate list of fundefs */
 while (i<len) {                                  /* for all functiondefinitions */
  FDL=udi_Mk_ListTail(
   FDL,                                                    /* Add fundef to list */
   udi_create_fundef(EDIT_DUMMY,FNames[i],Paras[i],Bodies[i])
  );
  i++;
 }

 node=udi_create_define(
  EDIT_DUMMY,
  FDL,                                        /* First define-node : fundef-list */
  MK_NEXT_EXPR()                        /* Second define-node : Goal expresssion */
 );

 free(FNames);                                           /* Free fct-names array */
 free(Paras);                                            /* Free fct-paras array */
 free(Bodies);                                          /* Free fct-bodies array */

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_AppConstructor(STACKELEM elem)
{ /* Make application and tries to find correct internal node structure **********/
 NODE *node=NULL;
 NODE *args=NULL;
 NODE **Apps;                                /* Array of application expressions */
 int i,len;
 int ap_type=elem;
 DBUG_ENTER("MK_AppConstructor");

 i=0;
 len=VALUE(elem);                                   /* Arity of this application */
 Apps=(NODE **)malloc(sizeof(NODE *)*len);
 while (i<len) {                                  /* Scan all application expr's */
  Apps[i++]=MK_NEXT_EXPR();                                 /* Get application i */
 }

 len--;                                        /* Last used Applications element */

 if (len==1) {                                       /* Test special case : ap 2 */
  if (Apps[1]) {                                                /* Exists node ? */
   if (NODE_TYPE(Apps[1]->type)==NT_COND)                      /* if then else ? */
    node=udi_create_ifcond(EDIT_DUMMY,Apps[0],Apps[1]);
  }
 } 

 if (node==NULL) {             /* Make node for application arity greater then 2 */
  if (Apps[len]) {                                         /* Exists last node ? */
   switch (NODE_TYPE(Apps[len]->type)) {                       /* Test node type */
    case NT_IDENTIFIER :                                     /* Userfunctioncall */
     i=len-1;                                                   /* Last argument */
     while (i>=0) args=udi_Mk_ListTail(args,Apps[i--]);    /* Make argument list */
     node=udi_create_funcall(
      EDIT_USER_FC,                                     /* User defined function */
      Apps[len],                                                /* Function name */
      args                                                      /* Argument list */
     );
     break;
    case NT_PRIMFUNC :                                 /* Primitive functioncall */
     if ((len==2) && (ExistsInfixPF(Apps[2]->U.value))) {   /* Infix operation ? */
      node=udi_create_expr_3(EDIT_DUMMY,Apps[1],Apps[2],Apps[0]);
     } else {                                        /* Make prefix functioncall */
      if (!((len!=2) && (ExistsInfixPF(Apps[len]->U.value))))
       Apps[len]->U.value|=_PREFIX_PF_;               /* Sign as prefix function */
      i=len-1;                                                  /* Last argument */
      while (i>=0) args=udi_Mk_ListTail(args,Apps[i--]);   /* Make argument list */
      node=udi_create_funcall(
       EDIT_PREFIX_FC,                              /* prefix primitive function */
       Apps[len],                                               /* Function name */
       args                                                     /* Argument list */
      );
     }
     break;
    case NT_SUBSTITUTE : {                              /* let binding in expr ? */
     NODE *binding_list=NULL;
     NODEList *sublist;
     sublist=Apps[len]->U.tuple->n1->U.list->first;        /* List of identifier */
     i=len-1;                                                   /* Last argument */
     while ((sublist)&&(i>=0)) {
      binding_list=udi_Mk_ListTail(                    /* Append binding to list */
       binding_list,
       udi_create_binding(                                 /* Create new binding */
        EDIT_NORMAL_BINDING,
        sublist->node,                                       /* First identifier */
        Apps[i--]                                                  /* Expression */
       )
      );
      sublist=sublist->next;                                  /* Next identifier */
     }
     if (T_DAP(ap_type)) {
      node=udi_create_letpar(              /* Create letpar expression structure */
       EDIT_DUMMY,
       binding_list,                                     /* List of var bindings */
       Apps[len]->U.tuple->n2                                   /* IN expression */
      );
     } else { 
      node=udi_create_let(                    /* Create let expression structure */
       EDIT_DUMMY,
       binding_list,                                     /* List of var bindings */
       Apps[len]->U.tuple->n2                                   /* IN expression */
      );
     }
     break;
    }
    default :                                              /* Normal application */
     i=len-1;                                                   /* Last argument */
     while (i>=0) args=udi_Mk_ListTail(args,Apps[i--]);    /* Make argument list */
     node=udi_create_application(NT_APPLICATION,Apps[len],args);
     break; 
   }
  }
 }
 
 free(Apps);                          /* Release array of application expression */

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Condition(void)
{ /* Make internal node for conditional code : then expr else expr ***************/
 NODE *node=NULL;
 DBUG_ENTER("MK_Condition");
 
 node=ALLOCNODE();
 node->type=SET_RT_TYPE(NT_COND|NT_NODE2);
 node->info=EDIT_DUMMY;
 node->U.tuple=ALLOCNODE2();
 node->U.tuple->n1=MK_NEXT_EXPR();                            /* then expression */
 node->U.tuple->n2=MK_NEXT_EXPR();                            /* else expression */
 
 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_MultiAtom(STACKELEM elem)
{ /* Transform a multi atomar expression to source (strings, numbers ...) ********/
 NODE *node=NULL;
 DBUG_ENTER("MK_MultiAtom");

 if (T_STR_CHAR(elem))                             /* Is it a string character ? */
  node=udi_create_char(EDIT_DUMMY,VALUE(elem));
 else {
  switch (elem&P_LET1) {                                         /* Test element */
   case LET0 : case LET1 :                                         /* Identifier */
    BACK_ELEM();
    node=MK_Ident();
    break;
   case DIG0 : case DIG1 :                                             /* Number */
    BACK_ELEM();
    node=MK_Number();
    break;
   default :
    fprintf(stderr,"** UNKNOWN MA (%d) **",elem);
    break;
  }
 }

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_PrimFunction(STACKELEM elem)
{ /* Generate a internal for a primfunction **************************************/
 NODE *node=NULL;
 int IPF;
 DBUG_ENTER("MK_PrimFunction");

 switch (elem) {                              /* Test which primfunction is used */
  case D_QUOT       : IPF=RED_IPF_QUOT ; break ;
  case M_NOT        : IPF=RED_IPF_NOT ; break ; 
  case M_ABS        : IPF=RED_IPF_ABS ; break ; 
  case D_AND        : IPF=RED_IPF_AND ; break ; 
  case C_DIV        : IPF=RED_IPF_CDIV ; break ; 
  case M_CEIL       : IPF=RED_IPF_CEIL ; break ; 
  case C_MINUS      : IPF=RED_IPF_CMINUS ; break ; 
  case C_MULT       : IPF=RED_IPF_CMULT ; break ; 
  case M_COS        : IPF=RED_IPF_COS ; break ; 
  case C_PLUS       : IPF=RED_IPF_CPLUS ; break ; 
  case DIMENSION    : IPF=RED_IPF_DIM ; break ; 
  case D_DIV        : IPF=RED_IPF_DIV ; break ; 
  case M_EMPTY      : IPF=RED_IPF_EMPTY ; break ; 
  case D_EQ         : IPF=RED_IPF_EQ ; break ; 
  case M_EXP        : IPF=RED_IPF_EXP ; break ; 
  case F_EQ         : IPF=RED_IPF_FEQ ; break ; 
  case M_FLOOR      : IPF=RED_IPF_FLOOR ; break ; 
  case F_NE         : IPF=RED_IPF_FNE ; break ; 
  case M_FRAC       : IPF=RED_IPF_FRAC ; break ; 
  case D_GEQ        : IPF=RED_IPF_GE ; break ; 
  case D_GREAT      : IPF=RED_IPF_GT ; break ; 
  case M_FIRST      : IPF=RED_IPF_HEAD ; break ; 
  case IP           : IPF=RED_IPF_IP ; break ; 
  case LCUT         : IPF=RED_IPF_LCUT ; break ; 
  case LDIMENSION   : IPF=RED_IPF_LDIM ; break ; 
  case D_LEQ        : IPF=RED_IPF_LE ; break ; 
  case M_LN         : IPF=RED_IPF_LN ; break ; 
  case LREPLACE     : IPF=RED_IPF_LREPLACE ; break ; 
  case LROTATE      : IPF=RED_IPF_LROTATE ; break ; 
  case LSELECT      : IPF=RED_IPF_LSELECT ; break ; 
  case D_LESS       : IPF=RED_IPF_LT ; break ; 
  case LTRANSFORM   : IPF=RED_IPF_LTRANSFORM ; break ; 
  case D_MAX        : IPF=RED_IPF_MAX ; break ; 
  case MCUT         : IPF=RED_IPF_MCUT ; break ; 
  case MDIMENSION   : IPF=RED_IPF_MDIM ; break ; 
  case D_MIN        : IPF=RED_IPF_MIN ; break ; 
  case D_MINUS      : IPF=RED_IPF_MINUS ; break ; 
  case D_MOD        : IPF=RED_IPF_MOD ; break ; 
  case MREPLACE     : IPF=RED_IPF_MREPLACE ; break ; 
  case MREPLACE_C   : IPF=RED_IPF_MREPLACE_C ; break ; 
  case MREPLACE_R   : IPF=RED_IPF_MREPLACE_R ; break ; 
  case MROTATE      : IPF=RED_IPF_MROTATE ; break ; 
  case MSELECT      : IPF=RED_IPF_MSELECT ; break ; 
  case D_MULT       : IPF=RED_IPF_MULT ; break ; 
  case MUNI         : IPF=RED_IPF_MUNITE ; break ; 
  case D_NEQ        : IPF=RED_IPF_NE ; break ; 
  case M_NEG        : IPF=RED_IPF_NEG ; break ; 
  case D_OR         : IPF=RED_IPF_OR ; break ; 
  case D_PLUS       : IPF=RED_IPF_PLUS ; break ; 
  case LUNI         : IPF=RED_IPF_LUNITE ; break ; 
  case REPSTR       : IPF=RED_IPF_REPSTR ; break ; 
  case REVERSE      : IPF=RED_IPF_REVERSE ; break ; 
  case M_SIN        : IPF=RED_IPF_SIN ; break ; 
  case M_REST       : IPF=RED_IPF_TAIL ; break ; 
  case M_TAN        : IPF=RED_IPF_TAN ; break ; 
  case TO_MAT       : IPF=RED_IPF_TOMAT ; break ; 
  case TO_SCAL      : IPF=RED_IPF_TOSCALAR ; break ; 
  case TO_TVECT     : IPF=RED_IPF_TOTVECT ; break ; 
  case TO_VECT      : IPF=RED_IPF_TOVECT ; break ; 
  case TRANSPOSE    : IPF=RED_IPF_TRANSPOSE ; break ; 
  case M_TRUNCATE   : IPF=RED_IPF_TRUNC ; break ; 
  case VCUT         : IPF=RED_IPF_VCUT ; break ; 
  case VDIMENSION   : IPF=RED_IPF_VDIM ; break ; 
  case VROTATE      : IPF=RED_IPF_VROTATE ; break ; 
  case VSELECT      : IPF=RED_IPF_VSELECT ; break ; 
  case VUNI         : IPF=RED_IPF_VUNITE ; break ; 
  case D_XOR        : IPF=RED_IPF_XOR ; break ; 
  case TYPE         : IPF=RED_IPF_TYPE ; break ; 
  case CLASS        : IPF=RED_IPF_CLASS ; break ; 
  case VC_DIV       : IPF=RED_IPF_VECTORDIV ; break ; 
  case VC_MINUS     : IPF=RED_IPF_VECTORMINUS ; break ; 
  case VC_MULT      : IPF=RED_IPF_VECTORMULT ; break ; 
  case VC_PLUS      : IPF=RED_IPF_VECTORPLUS ; break ; 
  case CUT          : IPF=RED_IPF_CUT ; break ; 
  case SELECT       : IPF=RED_IPF_SELECT ; break ; 
  case UNI          : IPF=RED_IPF_UNITE ; break ; 
  case SUBSTR       : IPF=RED_IPF_SUBSTR ; break ; 
  case TO_LIST      : IPF=RED_IPF_TOLIST ; break ; 
  case TO_FIELD     : IPF=RED_IPF_TOFIELD ; break ; 
  case MK_STRING    : IPF=RED_IPF_MKSTRING ; break ; 
  case C_MAX        : IPF=RED_IPF_CMAX ; break ; 
  case C_MIN        : IPF=RED_IPF_CMIN ; break ; 
  case D_CONS       : IPF=RED_IPF_CONS ; break ; 
  case SA_F(99,1)   : IPF=RED_IPF_BINHEAD ; break ; 
  case SA_F(100,1)  : IPF=RED_IPF_BINTAIL ; break ; 
  default :
   fprintf(stderr,"** UNKNOWN PRIMFUNCTION (%d) **",elem);fflush(stderr);
   IPF=0;
   break;
 }

 if (IPF>0) {                                            /* Known primfunction ? */
  node=ALLOCNODE();
  node->type=NT_PRIMFUNC;
  node->info=EDIT_DUMMY;
  node->U.value=IPF;                                  /* Set primfunction number */
 }

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Constant(STACKELEM elem)
{ /* Make internal node for constant expression **********************************/
 NODE *node=NULL;
 DBUG_ENTER("MK_Constant");

 if ((elem&P_DOLLAR)==DOLLAR) { 
  fprintf(stderr,"**DOLLAR**");fflush(stdout);
 } else if ((elem&P_NUM)==NUM) {
  fprintf(stderr,"**NUM**");fflush(stdout);
 } else {                                                /* Main constant switch */
  switch (elem) {                                    /* Test constant expression */
   /* Make node for empty list,vectors,strings,matrix .. *************************/
   case NIL       :
    node=udi_create_constant(EDIT_DUMMY,RED_CONST_EMPTYLIST);break;
   case NILMAT    :
    node=udi_create_constant(EDIT_DUMMY,RED_CONST_NILMATRIX);break;
   case NILSTRING :
    node=udi_create_constant(EDIT_DUMMY,RED_CONST_EMPTYSTRING);break;
   case NILTVECT  :
    node=udi_create_constant(EDIT_DUMMY,RED_CONST_NILTVECTOR);break;
   case NILVECT   :
    node=udi_create_constant(EDIT_DUMMY,RED_CONST_NILVECTOR);break;
   /* Make node for boolean constants ********************************************/
   case SA_FALSE :
    node=udi_create_constant(EDIT_DUMMY,RED_BOOL_FALSE);break;
   case SA_TRUE :
    node=udi_create_constant(EDIT_DUMMY,RED_BOOL_TRUE);break;
   /* Make node for class code value *********************************************/
   case CL_FUNCTION :
    node=udi_create_class(EDIT_DUMMY,RED_CLASS_FUNCTION);break;
   case CL_SCALAR :
    node=udi_create_class(EDIT_DUMMY,RED_CLASS_SCALAR);break;
   case CL_SET :
    node=udi_create_class(EDIT_DUMMY,RED_CLASS_LIST);break;
   case CL_MATRIX :
    node=udi_create_class(EDIT_DUMMY,RED_CLASS_MATRIX);break;
   case CL_TVECTOR :
    node=udi_create_class(EDIT_DUMMY,RED_CLASS_TVECTOR);break;
   case CL_VECTOR :
    node=udi_create_class(EDIT_DUMMY,RED_CLASS_VECTOR);break;
   /* Make node for pattern symbols **********************************************/
   case PM_SKIP :
    node=udi_create_symbol(
          EDIT_DUMMY,
          PATTERN_STRING_MODE ? RED_PATTERN_STR_DOT : RED_PATTERN_DOT
         );
    break;
   case PM_SKSKIP :
    node=udi_create_symbol(
          EDIT_DUMMY,
          PATTERN_STRING_MODE ? RED_PATTERN_STR_DOTDOTDOT : RED_PATTERN_DOTDOTDOT
         );
    break;
   case PM_PT_PLUS :
    node=udi_create_symbol(
          EDIT_DUMMY,
          PATTERN_STRING_MODE ? RED_PATTERN_STR_DOTPLUS : RED_PATTERN_DOTPLUS
         );
    break;
   /* Make node for type symbols *************************************************/
   case BOOL :
    node=udi_create_type(EDIT_DUMMY,RED_TYPE_BOOL);break;
   case CHAR :
    node=udi_create_type(EDIT_DUMMY,RED_TYPE_STRING);break;
   case DECIMAL :
    node=udi_create_type(EDIT_DUMMY,RED_TYPE_NUMBER);break;
   case PRIMFUNC :
    node=udi_create_type(EDIT_DUMMY,RED_TYPE_PRIMFUNC);break;
   case USERFUNC :
    node=udi_create_type(EDIT_DUMMY,RED_TYPE_USERFUNC);break;
   case MATRIX :
    node=udi_create_type(EDIT_DUMMY,RED_TYPE_MATRIX);break;

   case PM_FAIL :
    node=udi_create_constant(EDIT_DUMMY,RED_CONST_FAIL);
    break;
   case PM_SUCC :
    node=udi_create_constant(EDIT_DUMMY,RED_CONST_OK);
    break;


   case CONDITIONAL :
    fprintf(stderr,"**CONDITIONAL**");fflush(stdout);
    break;
   case DOL_CONST :
    fprintf(stderr,"**DOL_CONST**");fflush(stdout);
    break;
   case ESET :
    fprintf(stderr,"**ESET**");fflush(stdout);
    break;
   case EXPR :
    fprintf(stderr,"**EXPR**");fflush(stdout);
    break;
   case HASH :
    fprintf(stderr,"**HASH**");fflush(stdout);
    break;
   case KLAA :
    fprintf(stderr,"**KLAA**");fflush(stdout);
    break;
   case LAMBDA :
    fprintf(stderr,"**LAMBDA**");fflush(stdout);
    break;
   case PLACEH :
    fprintf(stderr,"**PLACEH**");fflush(stdout);
    break;
   case PM_DOLLAR :
    fprintf(stderr,"**PM_DOLLAR**");fflush(stdout);
    break;
   case PM_END :
    fprintf(stderr,"**PM_END**");fflush(stdout);
    break;
   case PM_GGG :
    fprintf(stderr,"**PM_GGG**");fflush(stdout);
    break;
   case PM_GGG_E :
    fprintf(stderr,"**PM_GGG_E**");fflush(stdout);
    break;
   case PM_GGG_N :
    fprintf(stderr,"**PM_GGG_N**");fflush(stdout);
    break;
   case TERMINATE :
    fprintf(stderr,"**TERMINATE**");fflush(stdout);
    break;
   case TMATRIX :
    fprintf(stderr,"**TMATRIX**");fflush(stdout);
    break;
  default :
   fprintf(stderr,"** UNKNOWN CONSTANT-EXPRESSION (%08lx) **\n",(unsigned long)elem);
   fflush(stderr);
   break;
  } /* End of switch *************************************************************/
 } /* End of else part ***********************************************************/

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Singleatom(STACKELEM elem)
{ /* Retransform a single atom to source code ************************************/
 NODE *node=NULL;
 DBUG_ENTER("MK_Singleatom");

 if (T_CONSTANT(elem)) {                                /* Constant expression ? */
  node=MK_Constant(elem);
 } else if (T_FUNC(elem)) {                              /* Primitive function ? */
  node=MK_PrimFunction(elem);
 } else {                                        /* Something else : main switch */
  fprintf(stderr,"** OTHERS **");fflush(stdout);
 }

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_When(STACKELEM elem)
{ /* Make internal node for when expressions *************************************/
 NODE *node=NULL;
 int len;
 DBUG_ENTER("MK_When");

 len=VALUE(elem); /* Test arity of when expression */ 
 if (len==3) {                        /* Normal single pattern when expression ? */
  PATTERN_MODE++;
  node=udi_create_patterns(NT_PATTERNS,MK_NEXT_EXPR());
  PATTERN_MODE--;
  node=udi_create_when( 
   EDIT_DUMMY,                                            /* No edit info needed */
   node,                                                   /* pattern expression */
   MK_NEXT_EXPR(),                                           /* guard expression */
   MK_NEXT_EXPR()                                               /* do expression */
  );
 } else {                                                        /* Pattern list */
  NODE *pattern_list=NULL;
  PATTERN_MODE++;
  while (len>2) {                                 /* Iterate pattern expressions */
   pattern_list=udi_Mk_ListTail(
    pattern_list,
    udi_create_patterns(NT_PATTERNS,MK_NEXT_EXPR())
   );
   len--;
  }
  PATTERN_MODE--;
  pattern_list=udi_create_patterns(EDIT_PATTERN_LIST,pattern_list);
  node=udi_create_when( 
   EDIT_DUMMY,                                            /* No edit info needed */
   pattern_list,                                           /* pattern expression */
   MK_NEXT_EXPR(),                                           /* guard expression */
   MK_NEXT_EXPR()                                               /* do expression */
  );
 }

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Case(STACKELEM elem)
{ /* Make internal node for a case statement *************************************/
 NODE *node=NULL;
 NODE *when_list,*otherwise_expr;
 int info;
 DBUG_ENTER("MK_Case");

 info=VALUE(elem);                     /* Number of when statements in case expr */
 when_list=NULL;                                           /* No when expression */
 while (info>1) {                                        /* Get when expressions */
  when_list=udi_Mk_ListTail(when_list,MK_NEXT_EXPR());
  info--;
 }

 NEXT_ELEM();                               /* Set stack pointer to next element */
 if (ACT_ELEM==PM_END) {                            /* No otherwise expression ? */
  info=EDIT_CASE;                                 /* Case without otherwise part */
  otherwise_expr=NULL;
 } else {
  info=EDIT_CASE_OTHERWISE;                          /* case with otherwise part */
  otherwise_expr=MK_EXPR();                     /* Make (otherwise) expression); */
 }
 
 node=udi_create_case(info,when_list,otherwise_expr);

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Substitute(STACKELEM elem)
{ /* Make internal node for a nameless function (substitution) *******************/
 NODE *node=NULL;
 NODE *var_list;
 int len;
 DBUG_ENTER("MK_Substitute");

 len=VALUE(elem);                           /* Number of variables to substiture */
 var_list=NULL;
 while (len>1) {                                           /* Make variable list */
  var_list=udi_Mk_ListTail(var_list,MK_Ident());
  len--;
 }

 node=udi_create_substitute(EDIT_DUMMY,var_list,MK_NEXT_EXPR());

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Userconstructor(STACKELEM elem,int list_len)
{ /* Make internal node for a user constructor ***********************************/
 NODE *node;
 NODE *nodelist;
 int i,len;
 DBUG_ENTER("MK_Userconstructor");

 len=VALUE(elem)-1;                   /* Get string(userconstructor-name) length */
 NEXT_ELEM();                                  /* Stackpointer to next element _ */
 i=0;
 while (i<len) {                                     /* Get Userconstructor name */
  NEXT_ELEM();                                                 /* Next character */
  BUFFER[i]=VALUE(ACT_ELEM);
  i++;
 }
 BUFFER[i]='\0';                                       /* Set end of string sign */

 node=udi_create_ident(EDIT_DUMMY,my_strdup(BUFFER));

 i=list_len-1;                      /* Number of expressions in user constructor */
 nodelist=NULL;
 while (i>0) {                                                /* Get expressions */
  nodelist=udi_Mk_ListTail(nodelist,MK_NEXT_EXPR());
  i--; 
 }

 i=PATTERN_MODE ? EDIT_PATTERN_USERCON : EDIT_NORMAL_USERCON;
 node=udi_create_usercon(
  i,                              /* Type of userconstructor (pattern or normal) */
  node,                                                  /* Userconstructor name */
  nodelist                                                   /* Constructor list */
 );

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_String(STACKELEM elem)
{ /* Make internal node for a nested string constant *****************************/
 NODE *node;
 int i;
 DBUG_ENTER("MK_String");

 if (PATTERN_MODE) PATTERN_STRING_MODE++;                            /* Set flag */
 
 i=VALUE(elem);                                                 /* String length */
 node=NULL;                                                      /* Empty string */
 while (i>0) {                                   /* Get substrings or characters */
  node=udi_Mk_ListTail(node,MK_NEXT_EXPR());
  i--;
 }

 if (PATTERN_MODE) {
  node=udi_create_patternstring(EDIT_DUMMY,node);
  if (PATTERN_STRING_MODE==1) {                         /* First patternstring ? */
   node=udi_create_patternstructure(
    EDIT_PATSTR_STRING,                                         /* Patternstring */
    node 
   );
  }
  PATTERN_STRING_MODE--;
 } else node=udi_create_string(EDIT_DUMMY,node);

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_InCharskip(STACKELEM elem)
{ /* Make internal node for a pattern match constructor **************************/
 NODE *node=NULL;
 DBUG_ENTER("MK_InCharskip");

 node=udi_create_aspattern(EDIT_IN_SPAT,MK_NEXT_EXPR(),MK_NEXT_EXPR());

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Charskip(STACKELEM elem)
{ /* Make internal node for a pattern match constructor **************************/
 NODE *node=NULL;
 NODE *node1,*node2;
 int info;
 int i;
 DBUG_ENTER("MK_Charskip");

 NEXT_ELEM();
 if (CONSTRUCTOR_NAME(ACT_ELEM)==_PM_IN) info=EDIT_ASIN_SPAT;
 else {
  if (PATTERN_STRING_MODE) info=EDIT_AS_SPAT;
  else info=EDIT_AS_PAT;
 }
 BACK_ELEM();
 
 node1=MK_NEXT_EXPR();

 if (info==EDIT_AS_PAT) {
  node2=ALLOCNODE();                               /* Alloc space for identifier */
  node2->type=SET_RT_TYPE(NT_IDENTIFIER|NT_ATOM);
  node2->info=EDIT_DUMMY;
  i=0;
 } else {                               /* node = EDIT_ASIN_SPAT or EDIT_AS_SPAT */
  NEXT_ELEM();                                            /* Skip next character */
  node2=ALLOCNODE();                        /* Alloc space for string identifier */
  node2->type=SET_RT_TYPE(NT_STRINGIDENT|NT_ATOM);
  node2->info=EDIT_DUMMY;
  BUFFER[0]='"';                             /* First stringidentifier character */
  i=1;
 }
 
 for (;;) {                                                      /* loop forever */
  if (NEXT_ELEM()==0) break;                                    /* Stack EMPTY ! */
  BUFFER=TEST_BUFFER(i);
  elem=ACT_ELEM;
 
  if ((elem&P_LET1)==LET1) {                   /* Letter, but not the last one ? */
   BUFFER[i]=VALUE(elem);
   i++; /* Increase number of characters */ 
  } else if ((elem&P_LET1)==LET0) {                             /* Last letter ? */
   BUFFER[i]=VALUE(elem);
   if (info!=EDIT_AS_PAT) {
     i++;
     BUFFER[i]='"';                           /* Last stringidentifier character */
   }
   BUFFER[i+1]='\0';                            /* Set nullbyte at end of string */
   i++;
   break;                                   /* Last character found, end of loop */
  } else {                                         /* Error : unexpected element */
   fprintf(stderr,"ERROR : unexpected element in Identifier !\n");
  }
 }

 if (info==EDIT_AS_PAT) node2->len=0;    /* Patternidentifier dont need len info */
 else node2->len=i;                            /* set string (identifier) length */
 node2->U.str=(char *)malloc(i+1);                  /* Space for string constant */
 memcpy(node2->U.str,BUFFER,i+1);                                 /* Copy string */

 node=udi_create_aspattern(info,node1,node2);

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_List(STACKELEM elem)
{ /* Make different internal nodes dependend on next element *********************/
 NODE *node=NULL;
 int len;
 STACKELEM elem2;
 DBUG_ENTER("MK_List");

 len=VALUE(elem); /* List length */ 
 NEXT_ELEM();                               /* Set stack pointer to next element */
 elem=ACT_ELEM;                                            /* Get actual element */
 NEXT_ELEM();                                                     /* Lookahead 2 */
 elem2=ACT_ELEM;
 BACK_ELEM();                                          /* Set stack pointer back */

 if (T_STRING(elem) && (VALUE(elem2)=='_'))
  node=MK_Userconstructor(elem,len); 
 else {
  NODE *listelement;
  BACK_ELEM(); /* Put one element back to stack */ 
  while (len>0) {                                 /* Get expression/pattern list */
   if (PATTERN_MODE) listelement=udi_create_patterns(EDIT_DUMMY,MK_NEXT_EXPR());
   else listelement=MK_NEXT_EXPR();
   node=udi_Mk_ListTail(node,listelement);
   len--;
  }
  if (PATTERN_MODE) {
   node=udi_create_patternstructure(
    EDIT_PATSTR_LIST,                                        /* List of patterns */
    udi_create_datalist(NT_PATTERNS,node)
   );
  } else {
   if (MATRIX_MODE==0) node=udi_create_datalist(NT_EXPR,node);
  }
 }

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_ProtectIdent(STACKELEM elem)
{ /* Make internal node for a protected identifier *******************************/
 NODE *node=NULL;
 int i,level=0;
 DBUG_ENTER("MK_ProtectIdent");

 for (;;) {                                                 /* Get protect level */
  if (CONSTRUCTOR_NAME(ACT_ELEM)==_PROTECT) level++;
  else break;
  NEXT_ELEM();
 }

 i=0;
 for (;;) {                                                      /* loop forever */
  elem=ACT_ELEM;
  if ((elem&P_LET1)==LET1) {                   /* Letter, but not the last one ? */
   BUFFER[i]=VALUE(elem);
   i++; /* Increase number of characters */ 
  } else {                                                      /* Last letter ? */
   BUFFER[i]=VALUE(elem);
   BUFFER[i+1]='\0';                            /* Set nullbyte at end of string */
   i++;
   break;                                   /* Last character found, end of loop */
  }
  NEXT_ELEM();
 }
 
 node=udi_create_prot_ident(EDIT_DUMMY,level,my_strdup(BUFFER));

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Vector(STACKELEM elem)
{ /* Make internal node structure for a typed vector *****************************/
 NODE *node=NULL;
 NODE *list;
 int listtype,
     type,
     info,
     vectype;
 DBUG_ENTER("MK_Vector");

 MATRIX_MODE++;

 vectype=T_VECTOR(elem) ? 1 : 0;               /* Tests if is it a normal vector */
 NEXT_ELEM();                                           /* Get next stackelement */
 switch (ACT_ELEM) {                                 /* Test type of this matrix */
  case DECIMAL :
   listtype=NT_NUMBER;
   type=RED_TYPE_NUMBER;
   info=vectype ? EDIT_NUMBER_VECT : EDIT_NUMBER_TVECT;
   break;
  case BOOL :
   listtype=NT_CONST;
   type=RED_TYPE_BOOL;
   info=vectype ? EDIT_BOOL_VECT : EDIT_BOOL_TVECT;
   break;
  case CHAR :
   listtype=NT_STRING;
   type=RED_TYPE_STRING;
   info=vectype ? EDIT_STRING_VECT : EDIT_STRING_TVECT;
   break;
  default :
   listtype=NT_NUMBER;
   type=RED_TYPE_NUMBER;
   info=vectype ? EDIT_NUMBER_VECT : EDIT_NUMBER_TVECT;
   break;
 }

 list=MK_NEXT_EXPR();
 list=udi_Mk_SetListType(list,listtype);

 node=vectype ? udi_create_vector(info,type,list) 
              : udi_create_tvector(info,type,list) ;

 MATRIX_MODE--;

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Matrix(STACKELEM elem)
{ /* Make internal node structure for a typed matrix *****************************/
 NODE *node=NULL;
 NODE *list;
 NODE *matrixlist;
 int dim,
     sublisttype,
     listtype,
     type,
     info;
 DBUG_ENTER("MK_Matrix");

 MATRIX_MODE++;

 dim=VALUE(elem)-1;                                          /* Matrix dimension */
 NEXT_ELEM();                                           /* Get next stackelement */
 switch (ACT_ELEM) {                                 /* Test type of this matrix */
  case TMATRIX :
   sublisttype=NT_MATRIX;
   listtype=NT_MATRIX;
   type=RED_TYPE_MATRIX;
   info=EDIT_MATRIX_MAT;
   break;
  case DECIMAL :
   sublisttype=NT_NUMBER;
   listtype=NT_NUMBER_LIST;
   type=RED_TYPE_NUMBER;
   info=EDIT_NUMBER_MAT;
   break;
  case BOOL    :
   sublisttype=NT_CONST;
   listtype=NT_BOOL_LIST;
   type=RED_TYPE_BOOL;
   info=EDIT_BOOL_MAT;
   break;
  case CHAR    :
   sublisttype=NT_STRING;
   listtype=NT_STRING_LIST;
   type=RED_TYPE_STRING;
   info=EDIT_STRING_MAT;
   break;
  default      :
   sublisttype=NT_NUMBER;
   listtype=NT_NUMBER_LIST;
   type=RED_TYPE_NUMBER;
   info=EDIT_NUMBER_MAT;
   break;
 }

 matrixlist=NULL;
 while (dim>0) {                                          /* Get all matrix rows */
  list=MK_NEXT_EXPR();
  list=udi_Mk_SetListType(list,sublisttype);
  matrixlist=udi_Mk_ListTail(matrixlist,list);
  dim--;
 }
 matrixlist=udi_Mk_SetListType(matrixlist,listtype);

 node=udi_create_matrix(info,type,matrixlist);

 MATRIX_MODE--;

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_VarConstructor(STACKELEM elem)
{ /* Retransform a var constructor to internal node structure ********************/
 NODE *node=NULL;
 DBUG_ENTER("MK_VarConstructor");

 if (T_DAP(elem)) node=MK_AppConstructor(elem);/* Distributed application letpar */
 else
 switch (CONSTRUCTOR_NAME(elem)) {                            /* Test construtor */
  case _LREC :             /* LetRec constructor : define f[..]=expr ... in expr */
   node=MK_Define();   /* Make define construct (internal define node structure) */
   break;
  case _AP :
   node=MK_AppConstructor(elem);        /* Try to build correct application node */
   break;
  case _SUB :
   node=MK_Substitute(elem);
   break;
  case _PM_SWITCH :                                                      /* Case */
   node=MK_Case(elem);
   break;
  case _PM_AS :                                       /* Match pattern as/in ... */
   node=MK_Charskip(elem);
   break;
  case _LIST :             /* Difficult case : userconstructor, pattern list ... */
   node=MK_List(elem);
   break;
  case _MATRIX :                                /* Make internal node for matrix */
   node=MK_Matrix(elem);


  case _PM_LIST :
   fprintf(stderr,"**PM_LIST**");fflush(stdout);
   break;

  default :
   fprintf(stderr,"** UNKNOWN VAR-CONSTRUCTOR (%08lx) **\n",(unsigned long)elem);
   fflush(stderr);
   break;
 }

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_FixConstructor(STACKELEM elem)
{ /* Retransforma a fix constructor to internal node structure *******************/
 NODE *node=NULL;
 DBUG_ENTER("MK_FixConstructor");

 if ((elem&P_UNCHAINED_CONSTR)==GROUP1_FIXCON) {    /* Group 1 Fix Constructor ? */
  switch (CONSTRUCTOR_NAME(elem)) {                           /* Test construtor */
   case _PLUS :                                 /* Make node for positive number */
   case _MINUS :
    node=MK_Number();
    break;
   case _COND :                             /* Make node for then expr else expr */
    node=MK_Condition(); 
    break;
   case _PM_OTHERWISE :                /* Otherwise expression in case statement */
    node=MK_NEXT_EXPR();
    break;
   case _PM_WHEN :                 /* Make node for when expr guard expr do expr */
    node=MK_When(elem);
    break;
   case _VECTOR : case _TVECTOR :                        /* Make node for vector */
    node=MK_Vector(elem);
    break;
   case _PROTECT :                                         /* Protect identifier */
    node=MK_ProtectIdent(elem);
    break;
   case _SET :
    node=udi_create_binlistconcat(EDIT_DUMMY,MK_NEXT_EXPR(),MK_NEXT_EXPR());
    break;

   case _EXP : fprintf(stderr,"** _EXP **");fflush(stdout);break;
   case _PM_MATCH : fprintf(stderr,"** _PM_MATCH **");fflush(stdout);break;
   case _REC : fprintf(stderr,"** _REC **");fflush(stdout);break;

   default :
    fprintf(stderr,"** UNKNOWN GROUP 1 FIX-CONSTRUCTOR (%08lx) **\n",(unsigned long)elem);
    fflush(stderr);
    break;
  }
 } else {                                          /* No group 1 fix constructor */
  switch (CONSTRUCTOR_NAME(elem)) {                          /* Test constructor */
   case _STRING :                                /* Make node for nested strings */
    node=MK_String(elem);
    break;
   case _PM_IN :
    node=MK_InCharskip(elem);
    break;

   case _UNPROTECT : fprintf(stderr,"** _UNPROTECT **");fflush(stdout);break;

   default :
    fprintf(stderr,"** UNKNOWN FIX-CONSTRUCTOR (%08lx) **\n",(unsigned long)elem);
    fflush(stderr);
    break;
  }
 }

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_Constructor(STACKELEM elem)
{ /* Retransform a constructor to source code ************************************/
 NODE *node;
 DBUG_ENTER("MK_Constructor");

 node=T_VAR_CONSTR(elem) ? MK_VarConstructor(elem) : MK_FixConstructor(elem);

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static NODE *MK_EXPR(void)
{ /* Retransform Orel0 code to high level source code ****************************/
 STACKELEM elem;
 NODE *node=NULL;
 DBUG_ENTER("MK_EXPR");

 elem=ACT_ELEM;                                          /* Actuell stackelement */

 /* Test top of stack element ****************************************************/
 if (IS_POINTER(elem)) {                                    /* Is it a pointer ? */
  fprintf(stderr,"RETRANSFORM-ERROR : Pointer on stack not expected !\n");
  ERROR++;
 } else {                                                        /* Test element */
  if (T_CON(elem)) {                                    /* Is it a constructor ? */
   node=MK_Constructor(elem);
  } else if (T_MA(elem)) {                                /* Is it a mult atom ? */
   node=MK_MultiAtom(elem);
  } else {                                    /* It must be an atomic expression */
   node=MK_Singleatom(elem);
  }
 }

 DBUG_ASSERT(node);DBUG_RETURN(node);
} /*******************************************************************************/

static void Init_Module(void)
{ /* Do all init module depend initialisation ************************************/
 BUFFER=NULL;                        /* Global string/identifier/number - buffer */
 BUFFER_SIZE=0;                                         /* Size of global buffer */
 ERROR=0;                                                   /* Global error flag */
 AS=NULL;                                                /* Actuell stackelement */
 as=0;                                         /* offset to actuell stackelement */
 nr_elems=0;                           /* Number of elements to process on stack */
 PATTERN_MODE=0;                                 /* Actual expression is pattern */
 PATTERN_STRING_MODE=0;                          /* Actual expression is pattern */
 MATRIX_MODE=0;                                                   /* Scan matrix */
} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

int udi_Orel0_2_Source(void)
{ /* Transforms Orel0 code to source code ****************************************/
 NODE *result;
 DBUG_ENTER("udi_Orel0_2_Source");

 ERROR=0;                                                     /* Assume no error */

 Init_Module();                                    /* Setup module for execution */

 if (OPTIONS.stacksize<=0) {                                /* Stacksize empty ? */
  fprintf(stderr,"ERROR : Empty programmstack !\n");
  ERROR++;
 } else if (OPTIONS.stackelems<=0) {                    /* No program on stack ? */
  fprintf(stderr,"ERROR : No Orel0 program on programstack !\n");
  ERROR++;
 } else if (OPTIONS.stack==NULL) {                          /* No programstack ? */
  fprintf(stderr,"ERROR : No programstack available !\n");
  ERROR++;
 } else {                 /* Ok, lets try to retransform programm to source code */
  AS=OPTIONS.stack;                                          /* set top of stack */
  as=0;                                                             /* No offset */
  nr_elems=OPTIONS.result_stacksize;      /* number of elements of programmstack */

  BUFFER=(char *)malloc(DEF_BUFFER_SIZE);                 /* Alloc global buffer */
  BUFFER_SIZE=DEF_BUFFER_SIZE;                                /* Set buffer size */

  result=MK_EXPR();                          /* Generate internal node structure */
#ifdef INTERNAL_UDI_DEBUG
  if (OPTIONS.test) {
   printf("\n\n-------------- INTERNAL NODE STRUCTURE ---------------\n");
   udi_PrTree(result);
   printf("\n-------------------------------------------------------\n");
  }
#endif
  Init_PrintModul(OPTIONS.output_fh);
  print_NODE(result);
  fprintf(OPTIONS.output_fh,"\n");
 }

 free(BUFFER);                                    /* Release global buffer space */

 DBUG_RETURN(ERROR);
} /*******************************************************************************/
