/**********************************************************************************
***********************************************************************************
**
**   File        : marco-gencode.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 27.4.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for Orel0 code generation
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

/*********************************************************************************/
/********** INTERNAL PROGRAM INFORMATIONS FOR THIS MODULE ************************/
/*********************************************************************************/

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "marco-create.h"
#include "marco-prnode.h"
#include "marco-options.h"
#include "marco-misc.h"

#include "dbug.h"

/* For editor informations, see fed/cencod.h for further informations **/
#ifdef USE_EDITINFO
#define MY_SET_EDIT(elem,ei)	(SET_EDIT(ei)|elem)
#define EI_AP_IF	1
#define EI_AP_AL	2
#define EI_AP_FNC	4
#define EI_AP_VAR	5
#define EI_AP_CALL	7
#define EI_AP_PRED	8
#define EI_AP_LET	10
#define EI_AP_LETP	11
#else
#define MY_SET_EDIT(elem,ei)	elem
#endif

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

static void GENCODE(long code);

static void GC_NODE(NODE *node);
static void GC_NODE2(int type,NODE *node);
static void GC_NODE3(int type,NODE *node);
static void GC_LIST(int type,NODE *node);
static void GC_NORMAL(int type,NODE *node);

static void GC_Patterns(NODE *node);

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void GENCODE(long code)
{ /* Generates code (output) *****************************************************/

 if (OPTIONS.conv2old==1) code=misc_conv2old(code);

 if ((OPTIONS.reduce)||(OPTIONS.outputformat==OF_SRC)) {        /* Call reduma ? */
  if (OPTIONS.stackelems==OPTIONS.stacksize) {                 /* Out of space ? */
   OPTIONS.stacksize+=DEFAULT_STACK_SIZE_ADD;                   /* New stacksize */
   OPTIONS.stack=(STACKELEM *)realloc(
    OPTIONS.stack,sizeof(STACKELEM)*OPTIONS.stacksize
   );
  }
  OPTIONS.stack[OPTIONS.stackelems++]=code;                /* Store stackelement */
 } else if (OPTIONS.outputformat==OF_OREL0) {  /* Format code to Orel0 debuginfo */
  prstel(OPTIONS.output_fh,(int)misc_conv2new(code),0);
 } else if (OPTIONS.outputformat==OF_STACKELEMS) {     /* Print values to output */
  fprintf(OPTIONS.output_fh,"%08lx\n",code);
 }

} /*******************************************************************************/

static int CalcNodeKind(int type)
{ /* Calculates kind of a node ***************************************************/
 int kind;
 DBUG_ENTER("CalcNodeKind");

 if ((type&NT_LIST)==NT_LIST) {
  kind=NT_LIST;
 } else if ((type&NT_ATOM)==NT_ATOM) {
  kind=NT_ATOM; 
 } else if ((type&NT_NODE2)==NT_NODE2) {
  kind=NT_NODE2;
 } else if ((type&NT_NODE3)==NT_NODE3) {
  kind=NT_NODE3;
 } else kind=0;

 DBUG_RETURN (kind);
} /*******************************************************************************/

static void GC_FunctionCall(NODE *node)
{ /* Generate code for functioncall **********************************************/
 NODE *N;
 DBUG_ENTER("GC_Functioncall");

 N=node->U.tuple->n2;                                       /* Functionarguments */
 if (N) {                                                         /* Arguments ? */
  NODEList *list;
  if (node->info==EDIT_USER_FC) {
   GENCODE(MY_SET_EDIT(SET_VALUE(AP,(N->len+1)),EI_AP_CALL));  /* App len+1 args */
  } else {
   GENCODE(MY_SET_EDIT(SET_VALUE(AP,(N->len+1)),EI_AP_PRED));  /* App len+1 args */
  }
  list=N->U.list->last;                                               /* Listend */
  while (list) {
   GC_NODE(list->node);
   list=list->prev;
  }
 }
 
 GC_NODE(node->U.tuple->n1);                                     /* Functionname */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Letexpr(NODE *node)
{ /* Generate code for a letexpression *******************************************/
 NODE *N;
 NODEList *list;
 DBUG_ENTER("GC_Letexpr"); 

 N=node->U.tuple->n1;                                         /* Bindinglistnode */
 GENCODE(MY_SET_EDIT(SET_VALUE(AP,(N->len+1)),EI_AP_LET));   /* Let-expr (appli) */

 list=N->U.list->last;                                                /* Listend */
 while (list) {                         /* Create revers list of let-expressions */
  GC_NODE(list->node->U.tuple->n2);
  list=list->prev;
 }
 
 GENCODE(SET_VALUE(SUB,(N->len+1)));

 list=N->U.list->first;                                              /* Listhead */
 while (list) {                                    /* Create list of identifiers */
  GC_NODE(list->node->U.tuple->n1);
  list=list->next;
 }

 GC_NODE(node->U.tuple->n2);          /* Generate code for goal (let) expression */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Letparexpr(NODE *node)
{ /* Generate code for a letparexpression ****************************************/
 NODE *N;
 NODEList *list;
 DBUG_ENTER("GC_Letparexpr"); 

 N=node->U.tuple->n1;                                         /* Bindinglistnode */
 GENCODE(SET_VALUE(P_DAP,(N->len+1)));                 /* Let-expr (application) */

 list=N->U.list->last;                                                /* Listend */
 while (list) {                         /* Create revers list of let-expressions */
  GC_NODE(list->node->U.tuple->n2);
  list=list->prev;
 }
 
 GENCODE(SET_VALUE(SUB,(N->len+1)));

 list=N->U.list->first;                                              /* Listhead */
 while (list) {                                    /* Create list of identifiers */
  GC_NODE(list->node->U.tuple->n1);
  list=list->next;
 }

 GC_NODE(node->U.tuple->n2);          /* Generate code for goal (let) expression */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_LetPexpr(NODE *node)
{ /* Generate code for a let pattern binding expression **************************/
 NODE *N;
 NODEList *list;
 DBUG_ENTER("GC_Letexpr"); 

 N=node->U.tuple->n1;                                         /* Bindinglistnode */
 GENCODE(MY_SET_EDIT(SET_VALUE(AP,(N->len+1)),EI_AP_LETP));  /* Let-expr (appli) */

 list=N->U.list->last;                                                /* Listend */
 while (list) {                         /* Create revers list of let-expressions */
  GC_NODE(list->node->U.tuple->n2);
  list=list->prev;
 }
 
 GENCODE(SET_VALUE(PM_WHEN,(N->len+2)));

 list=N->U.list->first;                                              /* Listhead */
 while (list) {                                    /* Create list of identifiers */
  GC_NODE(list->node->U.tuple->n1);
  list=list->next;
 }
 
 GENCODE(SA_TRUE);                                          /* Guard always true */
 GC_NODE(node->U.tuple->n2);          /* Generate code for goal (let) expression */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Application(NODE *node)
{ /* Generate code for an application ********************************************/
 NODE *N;
 NODEList *list;
 DBUG_ENTER("GC_Application");

 N=node->U.tuple->n2;                                          /* Expressionlist */
 if (N) {                                                     /* Exists a list ? */
  if (N->len==2) { /* ( expr expr expr ) */
   GENCODE(MY_SET_EDIT(SET_VALUE(AP,(N->len+1)),EI_AP_AL));   /* Exprlist (appl) */
  } else {
   GENCODE(SET_VALUE(AP,(N->len+1)));                         /* Exprlist (appl) */
  }
  list=N->U.list->last;                                               /* Listend */
  while (list) {                        /* Create revers list of let-expressions */
   GC_NODE(list->node);
   list=list->prev;
  }
 }
 
 GC_NODE(node->U.tuple->n1);               /* Generate code for application-body */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_FundefList(NODE *node)
/*********************************************************************************/
/* Codegeneration for function definitions                                      **/
/* NODE is a triple of :                                                        **/
/* n1 = Functionname                                                            **/
/* n2 = Functionarguments (may be empty (NULL))                                 **/
/* n3 = Functionbody                                                            **/
/*********************************************************************************/
{ /* Generates code for a list of functiondefinitions ****************************/
 NODEList *list,*list2;
 NODE *N;
 DBUG_ENTER("GC_FundefList");

 list=node->U.list->first;                                           /* Listhead */
 while (list) {                           /* First write a list of functionnames */
  GC_NODE(list->node->U.triple->n1);                           /* Write fctnames */
  list=list->next;                                              /* Next function */
 }

 GENCODE(SET_VALUE(LIST,node->len));                /* Write number of functions */

 list=node->U.list->first;                                           /* Listhead */
 while (list) {                                        /* Gen code for functions */
  N=list->node->U.triple->n2;                                    /* Argumentlist */
  if (N) {                                                        /* Arguments ? */
   GENCODE(SET_VALUE(SUB,(N->len+1)));                  /* Number of args plus 1 */
   list2=N->U.list->first;                     /* First argument (start of list) */
   while (list2) {                                      /* Gen list of arguments */
    GC_NODE(list2->node);
    list2=list2->next;                                          /* Next function */
   }
  }

  GC_NODE(list->node->U.triple->n3);                       /* Functionexpression */

  list=list->next;                                              /* Next function */
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_LIST(int type,NODE *node)
{ /* Traverse a list of nodes ****************************************************/
 int ltype,lkind;
 NODEList *list;
 DBUG_ENTER("GC_LIST");

 list=node->U.list->first;                                           /* Listhead */
 lkind=CalcNodeKind(type);                         /* Kind of FIRST list element */
 ltype=CalcNodeType(type,lkind);                   /* Type of FIRST list element */

 GENCODE(SET_VALUE(LIST,node->len));
 
 if (ltype==NT_FUNDEF) GC_FundefList(node);
 else {
  while (list) {
   GC_NODE(list->node);
   list=list->next;
  }
 }
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_GenerateNumber(NODE *node)
{ /* Generate Orel0 for a number (JUST A HACK !!!!!!!) ***************************/
 char *str;
 DBUG_ENTER("GC_GenerateNumber");

 str=node->U.str;                                   /* Stringpointer with number */
 if (*str=='~') {
  GENCODE(SET_VALUE(MINUS,1));
  str++;
 } else GENCODE(SET_VALUE(PLUS,1));

 while (*str) {                                                    /* All digits */
  GENCODE(SET_VALUE(DIG1,*str));
  str++;                                                           /* Next digit */
 }
 GENCODE(DECPT);                                               /* Last digit (.) */
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Primfunction(int pf)
{ /* Generates code for a primfunction *******************************************/
 DBUG_ENTER("GC_Primfunction");

 switch (PF_FUNCTION(pf)) {
  case RED_IPF_QUOT		: GENCODE(D_QUOT);break;
  case RED_IPF_NOT		: GENCODE(M_NOT);break;
  case RED_IPF_ABS		: GENCODE(M_ABS);break;
  case RED_IPF_AND		: GENCODE(D_AND);break;
  case RED_IPF_CDIV		: GENCODE(C_DIV);break;
  case RED_IPF_CEIL		: GENCODE(M_CEIL);break;
  case RED_IPF_CMINUS		: GENCODE(C_MINUS);break;
  case RED_IPF_CMULT		: GENCODE(C_MULT);break;
  case RED_IPF_COS		: GENCODE(M_COS);break;
  case RED_IPF_CPLUS		: GENCODE(C_PLUS);break;
  case RED_IPF_DIM		: GENCODE(DIMENSION);break;
  case RED_IPF_DIV		: GENCODE(D_DIV);break;
  case RED_IPF_EMPTY		: GENCODE(M_EMPTY);break;
  case RED_IPF_EQ		: GENCODE(D_EQ);break;
  case RED_IPF_EXP		: GENCODE(M_EXP);break;
  case RED_IPF_FEQ		: GENCODE(F_EQ);break;
  case RED_IPF_FLOOR		: GENCODE(M_FLOOR);break;
  case RED_IPF_FNE		: GENCODE(F_NE);break;
  case RED_IPF_FRAC		: GENCODE(M_FRAC);break;
  case RED_IPF_GE		: GENCODE(D_GEQ);break;
  case RED_IPF_GT		: GENCODE(D_GREAT);break;
  case RED_IPF_HEAD		: GENCODE(M_FIRST);break;
  case RED_IPF_IP		: GENCODE(IP);break;
  case RED_IPF_LCUT		: GENCODE(LCUT);break;
  case RED_IPF_LDIM		: GENCODE(LDIMENSION);break;
  case RED_IPF_LE		: GENCODE(D_LEQ);break;
  case RED_IPF_LN		: GENCODE(M_LN);break;
  case RED_IPF_LREPLACE		: GENCODE(LREPLACE);break;
  case RED_IPF_LROTATE		: GENCODE(LROTATE);break;
  case RED_IPF_LSELECT		: GENCODE(LSELECT);break;
  case RED_IPF_LT		: GENCODE(D_LESS);break;
  case RED_IPF_LTRANSFORM	: GENCODE(LTRANSFORM);break;
  case RED_IPF_LUNITE		: GENCODE(LUNI);break;
  case RED_IPF_MAX		: GENCODE(D_MAX);break;
  case RED_IPF_MCUT		: GENCODE(MCUT);break;
  case RED_IPF_MDIM		: GENCODE(MDIMENSION);break;
  case RED_IPF_MIN		: GENCODE(D_MIN);break;
  case RED_IPF_MINUS		: GENCODE(D_MINUS);break;
  case RED_IPF_MOD		: GENCODE(D_MOD);break;
  case RED_IPF_MREPLACE		: GENCODE(MREPLACE);break;
  case RED_IPF_MREPLACE_C	: GENCODE(MREPLACE_C);break;
  case RED_IPF_MREPLACE_R	: GENCODE(MREPLACE_R);break;
  case RED_IPF_MROTATE		: GENCODE(MROTATE);break;
  case RED_IPF_MSELECT		: GENCODE(MSELECT);break;
  case RED_IPF_MULT		: GENCODE(D_MULT);break;
  case RED_IPF_MUNITE		: GENCODE(MUNI);break;
  case RED_IPF_NE		: GENCODE(D_NEQ);break;
  case RED_IPF_NEG		: GENCODE(M_NEG);break;
  case RED_IPF_OR		: GENCODE(D_OR);break;
  case RED_IPF_PLUS		: GENCODE(D_PLUS);break;
  case RED_IPF_PLUSPLUS		: GENCODE(LUNI);break;
  case RED_IPF_REPSTR		: GENCODE(REPSTR);break;
  case RED_IPF_REVERSE		: GENCODE(REVERSE);break;
  case RED_IPF_SIN		: GENCODE(M_SIN);break;
  case RED_IPF_TAIL		: GENCODE(M_REST);break;
  case RED_IPF_TAN		: GENCODE(M_TAN);break;
  case RED_IPF_TOMAT		: GENCODE(TO_MAT);break;
  case RED_IPF_TOSCALAR		: GENCODE(TO_SCAL);break;
  case RED_IPF_TOTVECT		: GENCODE(TO_TVECT);break;
  case RED_IPF_TOVECT		: GENCODE(TO_VECT);break;
  case RED_IPF_TRANSPOSE	: GENCODE(TRANSPOSE);break;
  case RED_IPF_TRUNC		: GENCODE(M_TRUNCATE);break;
  case RED_IPF_VCUT		: GENCODE(VCUT);break;
  case RED_IPF_VDIM		: GENCODE(VDIMENSION);break;
  case RED_IPF_VROTATE		: GENCODE(VROTATE);break;
  case RED_IPF_VSELECT		: GENCODE(VSELECT);break;
  case RED_IPF_VUNITE		: GENCODE(VUNI);break;
  case RED_IPF_XOR		: GENCODE(D_XOR);break;
  case RED_IPF_TYPE		: GENCODE(TYPE);break;
  case RED_IPF_CLASS		: GENCODE(CLASS);break;
  case RED_IPF_VECTORDIV	: GENCODE(VC_DIV);break;
  case RED_IPF_VECTORMINUS	: GENCODE(VC_MINUS);break;
  case RED_IPF_VECTORMULT	: GENCODE(VC_MULT);break;
  case RED_IPF_VECTORPLUS	: GENCODE(VC_PLUS);break;
  case RED_IPF_CUT		: GENCODE(CUT);break;
  case RED_IPF_SELECT		: GENCODE(SELECT);break;
  case RED_IPF_UNITE		: GENCODE(UNI);break;
  case RED_IPF_SUBSTR		: GENCODE(SUBSTR);break;
  case RED_IPF_TOLIST		: GENCODE(TO_LIST);break;
  case RED_IPF_TOFIELD		: GENCODE(TO_FIELD);break;
  case RED_IPF_MKSTRING		: GENCODE(MK_STRING);break;
  case RED_IPF_CMAX		: GENCODE(C_MAX);break;
  case RED_IPF_CMIN		: GENCODE(C_MIN);break;
  case RED_IPF_CONS		: GENCODE(D_CONS);break;
  case RED_IPF_BINHEAD		: GENCODE(SA_F(99,1));break;
  case RED_IPF_BINTAIL		: GENCODE(SA_F(100,1));break;
  default : fprintf(stderr,"** UNKNOWN PRIMFUNCTION (%d) **",pf);
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Constant(NODE *node)
{ /* Generate code for a nil or boolean constant *********************************/
 DBUG_ENTER("GC_Constant");

 switch (node->U.value) {
  case RED_CONST_FAIL           : GENCODE(PM_FAIL);break;
  case RED_CONST_OK		: GENCODE(PM_SUCC);break;
  case RED_CONST_NILMATRIX	: GENCODE(NILMAT);break;
  case RED_CONST_NILVECTOR	: GENCODE(NILVECT);break;
  case RED_CONST_NILTVECTOR	: GENCODE(NILTVECT);break;
  case RED_CONST_EMPTYLIST	: GENCODE(NIL);break;
  case RED_CONST_EMPTYSTRING	: GENCODE(NILSTRING);break;
  case RED_BOOL_FALSE		: GENCODE(SA_FALSE);break;
  case RED_BOOL_TRUE		: GENCODE(SA_TRUE);break;
  default : fprintf(stderr,"** UNKNOWN CONSTANT (%d) **",node->U.value);
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Matrix(NODE *node)
{ /* Generate code for matrix ****************************************************/
 NODE *N=node->U.one;
 NODEList *list;
 DBUG_ENTER("GC_Matrix");

 GENCODE(SET_VALUE(MATRIX,(N->len+1)));                         /* Dim of matrix */
 switch (node->len) {                                             /* Matrix-type */
  case RED_TYPE_MATRIX : GENCODE(TMATRIX);break;
  case RED_TYPE_NUMBER : GENCODE(DECIMAL);break;
  case RED_TYPE_BOOL   : GENCODE(BOOL);break;
  case RED_TYPE_STRING : GENCODE(CHAR);break;
  default : fprintf(stderr,"** UNKNOWN MATRIX TYPE (%d) **",node->len);
 }
 list=N->U.list->first;                                              /* Listhead */
 while (list) {
  GC_NODE(list->node);
  list=list->next;
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Case(NODE *node)
{ /* Generate code for an case expression ****************************************/
 NODE *N;
 DBUG_ENTER("GC_Case");

 N=node->U.tuple->n1;                                     /* When-Expressionlist */
 if (N) {                                                /* Exprlist available ? */
  NODEList *list=N->U.list->first;                                  /* Liststart */
  GENCODE(SET_VALUE(PM_SWITCH,(N->len+1)));
  while (list) {                                 /* Generate code for whole list */
   GC_NODE(list->node);
   list=list->next;
  }
 }

 N=node->U.tuple->n2;                  /* Generate code for otherwise expression */
 if (N) {                                             /* Exists Otherwise part ? */
  GENCODE(SET_VALUE(PM_OTHERWISE,1));
  GC_NODE(N);                                                   /* Generate expr */
 } else {                                          /* Generate constant end sign */
  GENCODE(PM_END);
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Substitute(NODE *node)
{ /*  Generate code for a substitute *********************************************/
 NODE *N;
 DBUG_ENTER("GC_Substitute");  

 N=node->U.tuple->n1;                                            /* Argumentlist */
 if (N) {                                                         /* Arguments ? */
  NODEList *list;
  GENCODE(SET_VALUE(SUB,(N->len+1)));                   /* Number of args plus 1 */
  list=N->U.list->first;                       /* First argument (start of list) */
  while (list) {                                        /* Gen list of arguments */
   GC_NODE(list->node);
   list=list->next;                                             /* Next function */
  }
 }

 GC_NODE(node->U.tuple->n2);                    /* Generate code for sub in expr */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Patternstring(NODE *node)
{ /* Generate code for a patternstring *******************************************/
 NODE *N;
 NODEList *list;
 DBUG_ENTER("GC_Patternstring");

 N=node->U.one;
 GENCODE(SET_VALUE(STRING,(N->len)));                   /* Number of args plus 1 */
 list=N->U.list->first;                        /* First argument (start of list) */
 while (list) {                                         /* Gen list of arguments */
  GC_NODE(list->node);
  list=list->next;                                              /* Next function */
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Patterns(NODE *node)
{ /* Generate code for a single pattern or a pattern-list ************************/
 int lkind,ltype;
 DBUG_ENTER("GC_Patterns");
 
 lkind=CalcNodeKind(node->type);                                 /* Pattern-type */
 ltype=CalcNodeType(node->type,lkind);                           /* Pattern-kind */

 if (lkind==NT_LIST) {                                         /* Pattern list ? */
  GC_LIST(ltype,node);
 } else if (lkind==NT_ATOM) {                             /* Atomic expression ? */
  GC_NORMAL(ltype,node);              /* Atomic code generation switch in Normal */
 } else if (lkind==NT_NODE2) {                             /* Node2 expression ? */
  GC_NODE2(ltype,node);
 } else {                                                      /* Single pattern */
  switch (ltype) {                                        /* Select pattern code */
   case NT_PATTERNSTRUCTURE :
    GC_Patterns(node->U.one);
    break;
   case NT_PATTERNSTRING :
    GC_Patternstring(node);
    break;
   default :                                            /* Normal codegeneration */
    GC_NODE(node->U.one);                          /* Make code for this subnode */
    break;
  }
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_When(NODE *node)
{ /* Generate code for a when patternmatch expr **********************************/
 NODE *N;
 DBUG_ENTER("GC_When");

 N=node->U.triple->n1->U.one;                    /* Pattern-Expr (may be a list) */

 if ((N->type&NT_LIST)==NT_LIST) {                 /* is it a list of patterns ? */
  NODEList *list;
  GENCODE(SET_VALUE(PM_WHEN,(2+N->len)));                    /* List of patterns */
  list=N->U.list->first;                                 /* Select start of list */
  while (list) {                                            /* Iterate full list */
   GC_NODE(list->node);
   list=list->next;                                              /* Next pattern */
  }
 } else {
  GENCODE(SET_VALUE(PM_WHEN,3));                            /* It's not a list ! */
  GC_NODE(N);                              /* Generate code for a single pattern */
 }

 GC_NODE(node->U.triple->n2);                                      /* Guard-Expr */
 GC_NODE(node->U.triple->n3);                                         /* Do-Expr */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Usercon(NODE *node)
{ /* Generate OREL0 code for a Userconstructor ***********************************/
 NODEList *list;
 NODE *N;
 char *str;
 int i;
 DBUG_ENTER("GC_Usercon");

 if (node->U.tuple->n2) {           /* Exists a list for this user constructor ? */
  GENCODE(SET_VALUE(LIST,(node->U.tuple->n2->len+1)));     /* Constructor lenght */
 } else {
  GENCODE(SET_VALUE(LIST,1));                    /* User constructor is a list 1 */
 }

 N=node->U.tuple->n1;                       /* Make code for Userconstructorname */
 str=N->U.str;                                      /* Pointer to identifiername */

 if (*str=='_') {                                   /* Special userconstructor ? */
   while (str[1]) {                                          /* Last character ? */
    GENCODE(SET_VALUE(LET1,*str));
    str++;
   }
   GENCODE(SET_VALUE(LET0,*str)); 
 } else {
  GENCODE(SET_VALUE(STRING,(1+N->len)));                   /* Write Stringlength */
  GENCODE(SET_VALUE(STR_CHAR,'_'));            /* Underscore for userconstructor */
  for (i=0;i<N->len;i++) GENCODE(SET_VALUE(STR_CHAR,str[i]));
 }

 if (node->U.tuple->n2) {           /* Exists a list for this user constructor ? */
  list=node->U.tuple->n2->U.list->first;                         /* Head of list */
  while (list) {                                 /* List of patterns not empty ? */
   GC_NODE(list->node);                                /* Generate code for node */
   list=list->next;                                         /* Next list element */
  }

 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_TypeSymbol(int symbol)
{ /* Transform symbol value to orel0 symbol code *********************************/
 switch (symbol) {                                                 /* Test value */
  case RED_TYPE_BOOL : GENCODE(BOOL);break;
  case RED_TYPE_NUMBER : GENCODE(DECIMAL);break;
  case RED_TYPE_PRIMFUNC : GENCODE(PRIMFUNC);break;
  case RED_TYPE_STRING : GENCODE(CHAR);break;
  case RED_TYPE_USERFUNC : GENCODE(USERFUNC);break;
  case RED_TYPE_MATRIX : GENCODE(MATRIX);break;
  default : fprintf(stderr,"** ERROR UNKNOW TYPE-SYMBOL (%d) **",symbol);
 }
} /*******************************************************************************/

static void GC_ClassSymbol(int symbol)
{ /* Transform symbol value to orel0 symbol code *********************************/
 switch (symbol) {                                                 /* Test value */
  case RED_CLASS_FUNCTION : GENCODE(CL_FUNCTION);break;
  case RED_CLASS_LIST : GENCODE(CL_SET);break;                 /* DONT KNOW ???? */
  case RED_CLASS_MATRIX : GENCODE(CL_MATRIX);break;
  case RED_CLASS_SCALAR : GENCODE(CL_SCALAR);break;
  case RED_CLASS_TVECTOR : GENCODE(CL_TVECTOR);break;
  case RED_CLASS_VECTOR : GENCODE(CL_VECTOR);break;
  default : fprintf(stderr,"** ERROR UNKNOW CLASS-SYMBOL (%d) **",symbol);
 }
} /*******************************************************************************/

static void GC_Symbol(int symbol)
{ /* Transform symbol value to orel0 symbol code *********************************/
 switch (symbol) {                                                 /* Test value */
  case RED_PATTERN_DOT : case RED_PATTERN_STR_DOT :
   GENCODE(PM_SKIP); break;
  case RED_PATTERN_DOTDOTDOT : case RED_PATTERN_STR_DOTDOTDOT :
   GENCODE(PM_SKSKIP); break;
  case RED_PATTERN_DOTPLUS : case RED_PATTERN_STR_DOTPLUS :
   GENCODE(PM_PT_PLUS);break;
  default : fprintf(stderr,"** ERROR UNKNOW SYMBOL (%d) **",symbol);
 }
} /*******************************************************************************/

static void GC_Vector(NODE *node)
{ /* Generate code for vector ****************************************************/
 DBUG_ENTER("GC_Vector");
 
 GENCODE(SET_VALUE(VECTOR,2));                                /* Code for vector */
 GC_TypeSymbol(node->len);
 GC_NODE(node->U.one);                                           /* List of type */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_TVector(NODE *node)
{ /* Generate code for transposed vector *****************************************/
 DBUG_ENTER("GC_TVector");
 
 GENCODE(SET_VALUE(TVECTOR,2));                    /* Code for transposed vector */
 GC_TypeSymbol(node->len);
 GC_NODE(node->U.one);                                           /* List of type */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Termsymbol(char *symbol)
{ /* Generate a constant token for special strings or create string constant *****/
 DBUG_ENTER("GC_Termsymbol");
 
 if (symbol) {                                                 /* Symbolstring ? */
  if (strcmp(symbol,"ok")==0) GENCODE(PM_SUCC);
  else if (strcmp(symbol,"fail")==0) GENCODE(PM_FAIL);
  else {                                      /* Write string as string constant */
   int len=strlen(symbol);
   GENCODE(SET_VALUE(STRING,len));
   while (*symbol) {                                          /* End of string ? */
    GENCODE(SET_VALUE(STR_CHAR,*symbol));
    symbol++;                                                  /* Next character */
   }
  }
 }
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Pattident(NODE *node)
{ /* Patternstring Identifier ('"A"`) ********************************************/
 char *str;
 DBUG_ENTER("GC_Pattident");
 
 if ((node->len>0)&&(node->type==NT_STRINGIDENT))
  GENCODE(SET_VALUE(STRING,node->len));       /* variable lenght */
 GENCODE(SET_VALUE(LET1,'%'));                                  /* Variable sign */
 str=node->U.str;
 while (*str) {
  if (*str!='"') {
   if ((str[1]=='"')||(str[1]==0)) GENCODE(SET_VALUE(LET0,*str)); /* Last letter */
   else GENCODE(SET_VALUE(LET1,*str));                          /* normal letter */
  }
  str++;
 }
 
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Charskip(NODE *node)
{ /* Generate code for string pattern char skip **********************************/
 DBUG_ENTER("GC_Charskip");

 if (node->info==EDIT_IN_SPAT) GENCODE(SET_VALUE(PM_IN,2));
 else GENCODE(SET_VALUE(PM_AS,2)); 
 GC_NODE(node->U.tuple->n1);                                  /* Code for symbol */
 GC_NODE(node->U.tuple->n2);                        /* Code for stringidentifier */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_Identifier(NODE *node)
{ /* Generates Orel0 code for a identifier ***************************************/
 char *str;
 int i;
 DBUG_ENTER("GC_Identifier");

 str=node->U.str;
 for (i=0;i<node->len-1;i++) GENCODE(SET_VALUE(LET1,str[i]));
 GENCODE(SET_VALUE(LET0,str[i]));                                 /* Last letter */

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_ProtIdentifier(NODE *node)
{ /* Generate Orel0 for a protect identifier *************************************/
 int i;
 char *str;
 DBUG_ENTER("GC_ProtIdentifier");

 for (i=0;i<node->len;i++) GENCODE(SET_VALUE(PROTECT,1));          /* Protectors */
 str=node->U.str;                                            /* Identifierstring */
 while (*str) {                                            /* String not empty ? */
  if (str[1]==0) GENCODE(SET_VALUE(LET0,*str));                   /* Last letter */
  else GENCODE(SET_VALUE(LET1,*str));                         /* Not last letter */
  str++;                                            /* Next identifier character */
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_String(NODE *node)
{ /* Generate Orel0 for a 'maybe' nested stringconstant **************************/
 NODEList *list;
 DBUG_ENTER("GC_String");

 GENCODE(SET_VALUE(STRING,node->U.one->len));              /* Write Stringlength */
 list=node->U.one->U.list->first;                        /* Start of string list */
 while (list) {
  GC_NODE(list->node);
  list=list->next;
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_NORMAL(int type,NODE *node)
{ /* Switch for almost all nodes types *******************************************/
 DBUG_ENTER("GC_NORMAL");

 switch (type) {
  case NT_DATA_LIST :
   GC_NODE(node->U.one);
   break;
  case NT_STRINGIDENT :              /* String identifier in a pattern construct */
   GC_Pattident(node);
   break;
  case NT_CLASS :                              /* Generate code for class symbol */
   GC_ClassSymbol(node->U.value);
   break;
  case NT_PRIMFUNC :                     /* Generate code for primitive function */
   GC_Primfunction(node->U.value);
   break;
  case NT_CONST :                    /* Transform constantsymbol to OREL0 symbol */
   GC_Constant(node);                      /* Code for a nil or boolean constant */
   break;
  case NT_MATRIX :                                   /* Generate code for matrix */
   GC_Matrix(node);
   break;
  case NT_TVECTOR :                     /* Generate code for a transposed vector */
   GC_TVector(node);
   break;
  case NT_VECTOR :                          /* Generate code for a normal vector */
   GC_Vector(node);
   break;
  case NT_CHAR :
   GENCODE(SET_VALUE(STR_CHAR,node->U.character));
   break;
  case NT_EXPR :
   GC_NODE(node->U.one);
   break;
  case NT_IDENTIFIER :
   GC_Identifier(node);
   break;
  case NT_NUMBER :
   GC_GenerateNumber(node);                            /* Make code for a number */
   break;
  case NT_PATTERNS :
   GC_Patterns(node->U.one);                           /* Make code for patterns */
   break;
  case NT_PROTIDENT :          /* Generate code for lambda protected identifiers */
   GC_ProtIdentifier(node);
   break;
  case NT_ERRORMESSAGE : 
   GC_NODE(node->U.one);                    /* Generate a normal string constant */
   break;
  case NT_STRING :
   GC_String(node);
   break;
  case NT_SYMBOL :                                 /* Generate code for a symbol */
   GC_Symbol(node->U.value);
   break;
  case NT_TERMSY :                            /* Termsymbol is a string constant */
   GC_Termsymbol(node->U.str);
   break;
  case NT_TYPE :                              /* Generate code for a type symbol */
   GC_TypeSymbol(node->U.value);
   break;
  case NT_PATTIDENTIFIER :            /* Generate code for a string in a pattern */
   GC_Pattident(node);
   break;
  case NT_PATTERNSTRUCTURE :
   GC_Patterns(node->U.one);
   break;
  case NT_PATTERNSTRING :
   GC_Patternstring(node);
   break;
  default :
   fprintf(stderr,"** UNKNOWN NORMAL NODE <%d> **",type);
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_NODE(NODE *node)
{ /* Main node code generation ***************************************************/
 DBUG_ENTER("GC_NODE");

 if (node) {
  int kind,type;
  kind=CalcNodeKind(node->type);
  type=CalcNodeType(node->type,kind);
  switch (kind) {
   case NT_NODE2 : 
    GC_NODE2(type,node);
    break;
   case NT_NODE3 :
    GC_NODE3(type,node);
    break;
   case NT_LIST :
    GC_LIST(type,node);
    break;
   default :
    GC_NORMAL(type,node);
    break;
  }
 } else fprintf(stderr,"< ***** EMPTY-NODE ***** >");
  
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_NODE2(int type,NODE *node)
{ /* Generates Orel-0 code for node tuple ****************************************/
 DBUG_ENTER("GC_NODE2");
                      /* GC_NODE(node->U.tuple->n1); GC_NODE(node->U.tuple->n2); */
 switch (type) {
  case NT_APPLICATION :
   GC_Application(node);
   break;
  case NT_ASPATTERN :                                 /* Code for as pattern Var */
   GC_Charskip(node);
   break;
  case NT_CASE :
   GC_Case(node);                         /* Generate code for an case statement */
   break;
  case NT_COND :
   GENCODE(SET_VALUE(COND,2));
   GC_NODE(node->U.tuple->n1);                                      /* Then-Expr */
   GC_NODE(node->U.tuple->n2);                                      /* Else-Expr */
   break;
  case NT_DEFINE :
   GENCODE(SET_VALUE(LREC,3));
   GC_NODE(node->U.tuple->n1);               /* Functiondeclarations/definitions */
   GC_NODE(node->U.tuple->n2);                            /* Functionapplication */
   break;
  case NT_FUNCALL :
   GC_FunctionCall(node);                    /* Generate code for a functioncall */
   break;
  case NT_IFCOND :
   GENCODE(MY_SET_EDIT(AP_2,EI_AP_IF));
   GC_NODE(node->U.tuple->n1);                                  /* Conditioncode */
   GC_NODE(node->U.tuple->n2);                         /* then/else conditoncode */
   break;
  case NT_LET :
   GC_Letexpr(node);                          /* Create code for a letexpression */
   break;
  case NT_LETPAR :
   GC_Letparexpr(node);                    /* Create code for a letparexpression */
   break;
  case NT_LETP :
   GC_LetPexpr(node);
   break;
  case NT_SUBSTITUTE :
   GC_Substitute(node);                     /* Create for a unnamed substitution */
   break;
  case NT_USERCON :                   /* Node2 : Node1 (Variable) , Node2 (List) */
   GC_Usercon(node);                         /* Code for a normaluserconstructor */
   break;
  case NT_BINLISTCONCAT :
   GENCODE(SET_VALUE(SET,2));
   GC_NODE(node->U.tuple->n1);
   GC_NODE(node->U.tuple->n2);
   break;
  default :                                            /* Illegal option : ERROR */
   fprintf(stderr,"** UNKNOWN NODE2 (%d) **\n",type);
   break;
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void GC_NODE3(int type,NODE *node)
{ /* Orel0 code switch for node->U.triple->{n1,n2,n3} ****************************/
 DBUG_ENTER("GC_NODE3");

 switch (type) {
  case NT_WHEN :
   GC_When(node);     /* Generate code for a when expr guard expr do expr triple */
   break;
  case NT_EXPR :                                  /* Code for ( expr expr expr ) */
   GENCODE(SET_ARITY(AP,3));               /* Application arguments revers order */
   GC_NODE(node->U.triple->n3);                              /* Third expression */
   GC_NODE(node->U.triple->n1);                              /* First expression */
   GC_NODE(node->U.triple->n2);                  /* Second expression (function) */
   break;
  case NT_FUNDEF :
   GC_FundefList(node);
   break;
  default :                                            /* Illegal option : ERROR */
   fprintf(stderr,"** UNKNOWN NODE3 (%d) **\n",type);
   break;
 }

 DBUG_VOID_RETURN;
} /*******************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

int udi_GenerateOrel0(void)
{ /* Start function for Orel0 code generation ************************************/
 DBUG_ENTER("udi_GenerateOrel0");
  GC_NODE(GOALEXPR);
  OPTIONS.result_stacksize=OPTIONS.stackelems;
 DBUG_RETURN(1);
} /*******************************************************************************/

int udi_ReadREDFile(char *filename)
{ /* Loads RED file to internal stack **/
 long code;
 FILE *infile;
 DBUG_ENTER("udi_ReadREDFile");
 
 if (filename==NULL) {
  fprintf(stderr,"No filename given. Unable to read a Orel0 file !\n");
  DBUG_RETURN(0);
 }

 if (strcmp(filename,"STDIN")==0) infile=stdin;
 else {
  if ((infile=fopen(filename,"r"))==NULL) { /* Exists this file ? */
   fprintf(stderr,"Unable to read this Orel0 file <%s> !\n",filename);
   DBUG_RETURN(0);
  }
 }

 while (fscanf(infile,"%08lx\n",&code)!=EOF) { /* End of file reached ? */
  if (OPTIONS.conv2old==2) {
   code=misc_conv2new(code);
  } else {
   if ((code&0x00ff)==9) code=code&0xfffffffe;
  }
  GENCODE(code); /* Store stackelement */
 }

 OPTIONS.result_stacksize=OPTIONS.stackelems;
 if (infile!=stdin) fclose(infile);

 DBUG_RETURN(1);
} /*********/
