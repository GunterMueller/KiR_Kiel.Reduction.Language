/**********************************************************************************
***********************************************************************************
**
**   File        : marco-create.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 27.4.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for code transformation
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

#include "dbug.h"

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

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

NODE *GOALEXPR=NULL; /* MAIN Expression */

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

static void MkNODE2(int type,NODE *node,NODE *p1,NODE *p2)
{ /* Creates NODE tuple **/
 node->type=NT_NODE2|type;
 node->U.tuple=ALLOCNODE2();
 node->U.tuple->n1=p1;
 node->U.tuple->n2=p2;
} /*******/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

NODE *udi_Mk_ListHead(NODE *p1)
{ /* Make a new node list ********************************************************/
 NODE *result;

 result=ALLOCNODE();
 result->info=EDIT_DUMMY;
 
 result->type=NT_LIST;                                         /* NODE is a list */
 if (p1) {
  result->info=p1->info; /* Get info */
  if (NODE_TYPE(p1->type)==NT_DATA_LIST) {
   result->type|=p1->info;
  } else result->type|=p1->type;                           /* It's a list of ... */
  result->len=1;                                            /* Liststrlen is one */
  result->U.list=(NODEListHeader *)malloc(sizeof(NODEListHeader));
  result->U.list->first=result->U.list->last=ALLOCNODELIST();
  result->U.list->first->node=p1;                                  /* Store node */
  result->U.list->first->prev=NULL;                       /* No previous element */
  result->U.list->first->next=NULL;                           /* No next element */
 } else {
  result->len=0;                                                   /* Empty list */
  result->U.list=NULL;                                      /* No list available */
 }
 
 return(result);
} /*******************************************************************************/

NODE *udi_Mk_ListTail(NODE *p1,NODE *p2)
{ /* Add one element to a list of nodes ******************************************/
 
 if (p1==NULL) return(udi_Mk_ListHead(p2));                     /* New list */

 p1->len++;                                     /* One more element in this list */

 if (p2) {                                         /* Node to append available ? */
  NODEListHeader *lh=p1->U.list;
  lh->last->next=ALLOCNODELIST();
  lh->last->next->prev=lh->last; /* Previous list element */
  lh->last=lh->last->next;
  lh->last->node=p2;                                       /* Store node pointer */
  lh->last->next=NULL;                                      /* Last list element */
 }

 return(p1);
} /*******************************************************************************/

NODE *udi_Mk_SetListType(NODE *p1,int p2)
{ /* Set node type for this list **/
 if (p1) { /* Test node pointer */
  if ((p1->type&NT_LIST)==NT_LIST)
   if (p1->U.list&&p1->U.list->first&&p1->U.list->first->node) {
    NODE *N=p1->U.list->first->node;
    N->type=((N->type)&~63)|p2;
   }
 }
 return(p1);
} /*******/

NODE *udi_create_expr(int edit_info,NODE *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_EXPR;
 result->U.one=p1;
 
 return(result);
} /****/

NODE *udi_create_datalist(int edit_info,NODE *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_DATA_LIST;
 result->U.one=p1;
 
 return(result);
} /****/

NODE *udi_create_application(int edit_info,NODE *p1,NODE *p2)
{ /* Creates one application node **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_APPLICATION,result,p1,p2);
  
 return(result);
} /****/

NODE *udi_create_binlistconcat(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_BINLISTCONCAT,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_aspattern(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_ASPATTERN,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_binding(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_BINDING,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_case(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_CASE,result,p1,p2);
 
 return(result);
} /****/

NODE *udi_create_char(int edit_info,char p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_CHAR;
 result->U.character=p1;

 return(result);
} /****/

NODE *udi_create_class(int edit_info,int p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_CLASS|NT_ATOM;
 result->U.value=p1;

 return(result);
} /****/

NODE *udi_create_cond(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_COND,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_constant(int edit_info,int p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_CONST|NT_ATOM;
 result->U.value=p1;

 return(result);
} /****/

NODE *udi_create_define(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_DEFINE,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_error_message(int edit_info,NODE *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ERRORMESSAGE;
 result->U.one=p1;

 return(result);
} /****/

NODE *udi_create_expr_3(int edit_info,NODE *p1,NODE *p2,NODE *p3)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_NODE3|NT_EXPR;
 result->U.triple=ALLOCNODE3();
 result->U.triple->n1=p1;
 result->U.triple->n2=p2;
 result->U.triple->n3=p3;

 return(result);
} /****/

NODE *udi_create_funcall(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_FUNCALL,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_fundef(int edit_info,NODE *p1,NODE *p2,NODE *p3)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_NODE3|NT_FUNDEF;
 result->U.triple=ALLOCNODE3();
 result->U.triple->n1=p1;
 result->U.triple->n2=p2;
 result->U.triple->n3=p3;

 return(result);
} /****/

NODE *udi_create_ident(int edit_info,char *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_IDENTIFIER;
 result->len=strlen(p1);
 result->U.str=p1;

 return(result);
} /****/

NODE *udi_create_pattident(int edit_info,char *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_PATTIDENTIFIER;
 result->len=strlen(p1);
 result->U.str=p1;

 return(result);
} /****/

NODE *udi_create_ifcond(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_IFCOND,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_let(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_LET,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_letpar(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_LETPAR,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_letp(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_LETP,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_matrix(int edit_info,int p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_MATRIX;
 result->len=p1;
 result->U.one=p2;

 return(result);
} /****/

NODE *udi_create_number(int edit_info,char *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_NUMBER;
 result->U.str=p1;

 return(result);
} /****/

NODE *udi_create_patterns(int edit_info,NODE *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_PATTERNS;
 result->U.one=p1;

 return(result);
} /****/

NODE *udi_create_patternstring(int edit_info,NODE *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_PATTERNSTRING;
 result->U.one=p1;

 return(result);
} /****/

NODE *udi_create_patternstructure(int edit_info,NODE *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_PATTERNSTRUCTURE;
 result->U.one=p1;

 return(result);
} /****/

NODE *udi_create_primfunc(int edit_info,int p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_PRIMFUNC;
 result->U.value=p1;

 return(result);
} /****/

NODE *udi_create_prot_ident(int edit_info,int p1,char *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_PROTIDENT;
 result->len=p1;
 result->U.str=p2;

 return(result);
} /****/

NODE *udi_create_string(int edit_info,NODE *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_STRING;
 result->U.one=p1;

 return(result);
} /****/

NODE *udi_create_stringident(int edit_info,char *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_STRINGIDENT;
 result->U.str=p1;

 return(result);
} /****/

NODE *udi_create_substitute(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_SUBSTITUTE,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_symbol(int edit_info,int p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_SYMBOL;
 result->U.value=p1;

 return(result);
} /****/

NODE *udi_create_termsy(int edit_info,char *p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_TERMSY;
 result->U.str=p1;

 return(result);
} /****/

NODE *udi_create_tvector(int edit_info,int p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_TVECTOR;
 result->len=p1;
 result->U.one=p2;

 return(result);
} /****/

NODE *udi_create_type(int edit_info,int p1)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_ATOM|NT_TYPE;
 result->U.value=p1;

 return(result);
} /****/

NODE *udi_create_usercon(int edit_info,NODE *p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 MkNODE2(NT_USERCON,result,p1,p2);

 return(result);
} /****/

NODE *udi_create_vector(int edit_info,int p1,NODE *p2)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_VECTOR;
 result->len=p1;
 result->U.one=p2;

 return(result);
} /****/

NODE *udi_create_when(int edit_info,NODE *p1,NODE *p2,NODE *p3)
{ /* **/
 NODE *result;

 result=ALLOCNODE();
 result->info=edit_info;
 result->type=NT_NODE3|NT_WHEN;
 result->U.triple=ALLOCNODE3();
 result->U.triple->n1=p1;
 result->U.triple->n2=p2;
 result->U.triple->n3=p3;

 return(result);
} /****/

#ifndef NORMAL_MALLOC

typedef struct MEMINFOTag { /* Information for memory buffer **/
 NODE *ptr; /* Pointer to array */
 struct MEMINFOTag *next; /* Pointer to next info block */
} MEMINFO; /*******/

static MEMINFO *MI_FIRST=NULL; /* First memory buffer information pointer */
static MEMINFO *MI_ACTUAL=NULL; /* Actual memory buffer information pointer */
static MEMINFO *MI_AGAIN=NULL; /* Reuse memory buffer information pointer */

static NODE *NODE_Array=NULL; /* Actual array pointers */

static int Node_Size=0; /* Size of Array */
static int Node_Pos=0; /* Actual array position */        
static int Node_Reallocs=0; /* Number of new arrays */
static int Node_used=0; /* Number of nodes allocated */
static int Node_max=0; /* Number of nodes max allocated */

static NODE *MKNEWBLOCK(void)
{ /* Make new memory buffer block **/

 if (MI_AGAIN!=NULL) { /* Reuse arrays ? */
  MI_ACTUAL=MI_AGAIN; /* Use this array mem info again */
  MI_AGAIN=MI_AGAIN->next; /* Set pointer to next array */
  return(MI_ACTUAL->ptr); /* Return pointer to array */
 }

 if (MI_FIRST==NULL) { /* Make first mem info element ? */
  MI_FIRST=MI_ACTUAL=(MEMINFO *)malloc(sizeof(MEMINFO));
 } else {
  MI_ACTUAL->next=(MEMINFO *)malloc(sizeof(MEMINFO));
  MI_ACTUAL=MI_ACTUAL->next;
 } 
 DBUG_ASSERT(MI_ACTUAL); /* Test memory allocation */
 MI_ACTUAL->ptr=(NODE *)malloc(sizeof(NODE)*Node_Size);
 DBUG_ASSERT(MI_ACTUAL->ptr);
 MI_ACTUAL->next=NULL; /* Last element */
 return (MI_ACTUAL->ptr);
} /*****/

NODE *AllocNode(void)
{ /* Return pointer to one node **/
 if (Node_Pos==Node_Size) { /* Buffer empty ? Resize it ! */
  NODE_Array=MKNEWBLOCK();
  Node_Pos=0;
  Node_Reallocs++;
 }
 Node_used++;
 return ((NODE *)&(NODE_Array[Node_Pos++]));
} /******/

NODE2 *AllocNode2(void)
{ /* Return pointer to one node2 **/
 if (Node_Pos==Node_Size) { /* Buffer empty ? Resize it ! */
  NODE_Array=MKNEWBLOCK();
  Node_Pos=0;
  Node_Reallocs++;
 }
 Node_used++;
 return ((NODE2 *)&(NODE_Array[Node_Pos++]));
} /******/

NODE3 *AllocNode3(void)
{ /* Return pointer to one node3 **/
 if (Node_Pos==Node_Size) { /* Buffer empty ? Resize it ! */
  NODE_Array=MKNEWBLOCK();
  Node_Pos=0;
  Node_Reallocs++;
 }
 Node_used++;
 return ((NODE3 *)&(NODE_Array[Node_Pos++]));
} /******/

NODEList *AllocNodeList(void)
{ /* Return pointer to one nodeList **/
 if (Node_Pos==Node_Size) { /* Buffer empty ? Resize it ! */
  NODE_Array=MKNEWBLOCK();
  Node_Pos=0;
  Node_Reallocs++;
 }
 Node_used++;
 return ((NODEList *)&(NODE_Array[Node_Pos++]));
} /******/

int udi_AllocNodes(int nr_nodes)
{ /* Allocates node buffer for number elements **/
 int result=1;
 DBUG_ENTER("udi_AllocNodes");

 Node_Size=nr_nodes;

 NODE_Array=MKNEWBLOCK();

 Node_Pos=0;
 Node_Reallocs=0;
 Node_used=0;

 DBUG_RETURN(result);
}

void udi_ReUseNodes(void)
{ /* Sign all nodes to be free **/
 DBUG_ENTER("udi_ReUseNodes");

 DBUG_PRINT("BEGIN",(""));

 Node_max=Node_used;
 Node_Pos=0;
 MI_AGAIN=MI_FIRST; /* Use all nodearray again ! */
 udi_FreeNodes(0); /* Free submem */
 NODE_Array=MKNEWBLOCK();

 DBUG_PRINT("END",(""));

 DBUG_VOID_RETURN;
} /*****/

void udi_FreeNodes(int delete)
{ /* Free all nodes **/
 MEMINFO *info,*tmp;
 int i,max;
 NODE *node;
 DBUG_ENTER("udi_FreeNodes");

 DBUG_PRINT("BEGIN",(""));

 info=MI_FIRST;
 while (info) { /* Free memory */
  tmp=info;
  if (info->ptr) { /* Array available ? */
   max=((Node_used-Node_Size)>0) ? Node_Size : Node_used;
   DBUG_PRINT("Test",("Node_used=%d Node_size=%d max=%d",Node_used,Node_Size,max));
   for (i=0;i<max;i++) {
    node=(NODE *)(&((info->ptr)[i])); /* pointer to subnode */
    /*DBUG_PRINT("for",("i=%d type=%d",i,node->type));*/
    if (node) {
     if (node->type<NT_NODE2) { /* Is it a simple node ?*/
      if ((NODE_TYPE(node->type)>NT_FREE_STR)
          && (NODE_TYPE(node->type)<NT_LAST)) { /* contains node a substring ? */
       if (node->U.str) {
        DBUG_PRINT("for",("str=<%s>",node->U.str));
        free(node->U.str); /* Free substring */
        node->U.str=NULL;
       }
      }
     }
    }
   }
   Node_used-=max;
   if (delete) free(info->ptr); /* Free node array */
  }
  info=info->next;
  if (delete) free(tmp); /* Free meminfo struct */
 }

 Node_used=0; /* Number of nodes allocated */

 if (delete) {
  MI_FIRST=MI_ACTUAL=NULL;
  NODE_Array=NULL;
  Node_Size=0; /* Size of Array */
  Node_Reallocs=0; /* Number of new arrays */
  Node_max=0; /* Number of nodes max allocated */
 }

 DBUG_PRINT("END",(""));

 DBUG_VOID_RETURN;
} /*********/

#endif
