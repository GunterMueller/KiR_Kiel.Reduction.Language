/**********************************************************************************
***********************************************************************************
**
**   File        : trans-tbl.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 7.4.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for transformation
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "gen-ccode.h"
#include "list-modu.h"
#include "trans-tbl.h"

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

static int CmpTransTbl();
static int MyStrcmp();

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static char *EditInfo_TBL[] = {
 "EDIT_ASIN_SPAT",
 "EDIT_AS_PAT",
 "EDIT_AS_SPAT",
 "EDIT_AS_STRPAT",
 "EDIT_BOOL_MAT",
 "EDIT_BOOL_TVECT",
 "EDIT_BOOL_VECT",
 "EDIT_CASE",
 "EDIT_CASE_OTHERWISE",
 "EDIT_DUMMY",
 "EDIT_EXPR_LIST",
 "EDIT_IN_SPAT",
 "EDIT_MATRIX_MAT",
 "EDIT_NUMBER_MAT",
 "EDIT_NORMAL_BINDING",
 "EDIT_NORMAL_USERCON",
 "EDIT_NUMBER_TVECT",
 "EDIT_NUMBER_VECT",
 "EDIT_PATSTR_LIST",
 "EDIT_PATSTR_MATRIX",
 "EDIT_PATSTR_VECTOR",
 "EDIT_PATSTR_STRING",
 "EDIT_PATTERN",
 "EDIT_PATTERN_BINDING",
 "EDIT_PATTERN_LIST",
 "EDIT_PATTERN_USERCON",
 "EDIT_PREFIX_FC",
 "EDIT_PRIMITIVE_FC",
 "EDIT_STRING_MAT",
 "EDIT_STRING_TVECT",
 "EDIT_STRING_VECT",
 "EDIT_USER_FC"
};


static CharValue NTInfo_TBL[] = {
 {"NT_APPLICATION",NT_APPLICATION},
 {"NT_ASPATTERN",NT_ASPATTERN},
 {"NT_BINDING",NT_BINDING},
 {"NT_BINLISTCONCAT",NT_BINLISTCONCAT},
 {"NT_BOOL_LIST",NT_BOOL_LIST},
 {"NT_CASE",NT_CASE},
 {"NT_CHAR",NT_CHAR},
 {"NT_CLASS",NT_CLASS},
 {"NT_COND",NT_COND},
 {"NT_CONST",NT_CONST},
 {"NT_DATA_LIST",NT_DATA_LIST},
 {"NT_DEFINE",NT_DEFINE},
 {"NT_ERRORMESSAGE",NT_ERRORMESSAGE},
 {"NT_EXPR",NT_EXPR},
 {"NT_FUNCALL",NT_FUNCALL},
 {"NT_FUNDEF",NT_FUNDEF},
 {"NT_IDENTIFIER",NT_IDENTIFIER},
 {"NT_IFCOND",NT_IFCOND},
 {"NT_LET",NT_LET},
 {"NT_LETP",NT_LETP},
 {"NT_LETPAR",NT_LETPAR},
 {"NT_MATRIX",NT_MATRIX},
 {"NT_NUMBER",NT_NUMBER},
 {"NT_NUMBER_LIST",NT_NUMBER_LIST},
 {"NT_PATTBINDING",NT_PATTBINDING},
 {"NT_PATTERNS",NT_PATTERNS},
 {"NT_PATTERNSTRING",NT_PATTERNSTRING},
 {"NT_PATTERNSTRUCTURE",NT_PATTERNSTRUCTURE},
 {"NT_PATTIDENTIFIER",NT_PATTIDENTIFIER},
 {"NT_PRIMFUNC",NT_PRIMFUNC},
 {"NT_PROTIDENT",NT_PROTIDENT},
 {"NT_STRING",NT_STRING},
 {"NT_STRINGIDENT",NT_STRINGIDENT},
 {"NT_STRING_LIST",NT_STRING_LIST},
 {"NT_SUBSTITUTE",NT_SUBSTITUTE},
 {"NT_SYMBOL",NT_SYMBOL},
 {"NT_TERMSY",NT_TERMSY},
 {"NT_TVECTOR",NT_TVECTOR},
 {"NT_TYPE",NT_TYPE},
 {"NT_USERCON",NT_USERCON},
 {"NT_VECTOR",NT_VECTOR},
 {"NT_WHEN",NT_WHEN}
};

static SimpleTrans SimpleTrans_TBL[] = {
 {"COPY","$$=$1",FCTID_FIRST_RULE},
 {"DUMMY","$$=NULL",FCTID_NONE}
};

static CharValue Type_TBL[] = {
 {"RED_TYPE_BOOL",RED_TYPE_BOOL},
 {"RED_TYPE_NUMBER",RED_TYPE_NUMBER},
 {"RED_TYPE_PRIMFUNC",RED_TYPE_PRIMFUNC},
 {"RED_TYPE_STRING",RED_TYPE_STRING},
 {"RED_TYPE_USERFUNC",RED_TYPE_USERFUNC},
};

static CharValue Class_TBL[] = {
 {"RED_CLASS_FUNCTION",RED_CLASS_FUNCTION},
 {"RED_CLASS_LIST",RED_CLASS_LIST},
 {"RED_CLASS_MATRIX",RED_CLASS_MATRIX},
 {"RED_CLASS_SCALAR",RED_CLASS_SCALAR},
 {"RED_CLASS_TVECTOR",RED_CLASS_TVECTOR},
 {"RED_CLASS_VECTOR",RED_CLASS_VECTOR}
};

static CharValue Constant_TBL[] = {
 {"RED_BOOL_FALSE",RED_BOOL_FALSE},
 {"RED_BOOL_TRUE",RED_BOOL_TRUE},
 {"RED_CONST_EMPTYLIST",RED_CONST_EMPTYLIST},
 {"RED_CONST_EMPTYSTRING",RED_CONST_EMPTYSTRING},
 {"RED_CONST_FAIL",RED_CONST_FAIL},
 {"RED_CONST_OK",RED_CONST_OK},
 {"RED_CONST_NILMATRIX",RED_CONST_NILMATRIX},
 {"RED_CONST_NILTVECTOR",RED_CONST_NILTVECTOR},
 {"RED_CONST_NILVECTOR",RED_CONST_NILVECTOR}
};

static CharValue Primfunc_TBL[] = {
 {"ABS",RED_IPF_ABS},
 {"AND",RED_IPF_AND},
 {"CDIV",RED_IPF_CDIV},
 {"CEIL",RED_IPF_CEIL},
 {"CLASS",RED_IPF_CLASS},
 {"CMINUS",RED_IPF_CMINUS},
 {"CMULT",RED_IPF_CMULT},
 {"COS",RED_IPF_COS},
 {"CPLUS",RED_IPF_CPLUS},
 {"DIM",RED_IPF_DIM},
 {"DIV",RED_IPF_DIV},
 {"EMPTY",RED_IPF_EMPTY},
 {"EQ",RED_IPF_EQ},
 {"EXP",RED_IPF_EXP},
 {"FEQ",RED_IPF_FEQ},
 {"FLOOR",RED_IPF_FLOOR},
 {"FNE",RED_IPF_FNE},
 {"FRAC",RED_IPF_FRAC},
 {"GE",RED_IPF_GE},
 {"GT",RED_IPF_GT},
 {"HEAD",RED_IPF_HEAD},
 {"IP",RED_IPF_IP},
 {"LCUT",RED_IPF_LCUT},
 {"LDIM",RED_IPF_LDIM},
 {"LE",RED_IPF_LE},
 {"LN",RED_IPF_LN},
 {"LROTATE",RED_IPF_LROTATE},
 {"LSELECT",RED_IPF_LSELECT},
 {"LT",RED_IPF_LT},
 {"LTRANSFORM",RED_IPF_LTRANSFORM},
 {"LUNITE",RED_IPF_LUNITE},
 {"MAX",RED_IPF_MAX},
 {"MCUT",RED_IPF_MCUT},
 {"MDIM",RED_IPF_MDIM},
 {"MIN",RED_IPF_MIN},
 {"MINUS",RED_IPF_MINUS},
 {"MOD",RED_IPF_MOD},
 {"MREPLACE",RED_IPF_MREPLACE},
 {"MREPLACE_C",RED_IPF_MREPLACE_C},
 {"MREPLACE_R",RED_IPF_MREPLACE_R},
 {"MROTATE",RED_IPF_MROTATE},
 {"MSELECT",RED_IPF_MSELECT},
 {"MULT",RED_IPF_MULT},
 {"MUNITE",RED_IPF_MUNITE},
 {"NE",RED_IPF_NE},
 {"NEG",RED_IPF_NEG},
 {"OR",RED_IPF_OR},
 {"PLUS",RED_IPF_PLUS},
 {"PLUSPLUS",RED_IPF_PLUSPLUS},
 {"REPSTR",RED_IPF_REPSTR},
 {"REVERSE",RED_IPF_REVERSE},
 {"QUOT",RED_IPF_QUOT},
 {"SIN",RED_IPF_SIN},
 {"TAIL",RED_IPF_TAIL},
 {"TAN",RED_IPF_TAN},
 {"TOMAT",RED_IPF_TOMAT},
 {"TOSCALAR",RED_IPF_TOSCALAR},
 {"TOTVECT",RED_IPF_TOTVECT},
 {"TOVECT",RED_IPF_TOVECT},
 {"TRANSPOSE",RED_IPF_TRANSPOSE},
 {"TRUNC",RED_IPF_TRUNC},
 {"TYPE",RED_IPF_TYPE},
 {"VCUT",RED_IPF_VCUT},
 {"VDIM",RED_IPF_VDIM},
 {"VECTORDIV",RED_IPF_VECTORDIV},
 {"VECTORMINUS",RED_IPF_VECTORMINUS},
 {"VECTORMULT",RED_IPF_VECTORMULT},
 {"VECTORPLUS",RED_IPF_VECTORPLUS},
 {"VROTATE",RED_IPF_VROTATE},
 {"VSELECT",RED_IPF_VSELECT},
 {"VUNITE",RED_IPF_VUNITE},
 {"XOR",RED_IPF_XOR},
 {"LREPLACE",RED_IPF_LREPLACE},
 {"NOT",RED_IPF_NOT},
 {"CUT",RED_IPF_CUT},
 {"SELECT",RED_IPF_SELECT},
 {"ROTATE",RED_IPF_ROTATE},
 {"UNITE",RED_IPF_UNITE},
 {"SUBSTR",RED_IPF_SUBSTR},
 {"TOLIST",RED_IPF_TOLIST},
 {"TOFIELD",RED_IPF_TOFIELD},
 {"SUBSTRING",RED_IPF_SUBSTRING},
 {"CMAX",RED_IPF_CMAX},
 {"CMIN",RED_IPF_CMIN},
 {"CONS",RED_IPF_CONS},
 {"BINHEAD",RED_IPF_BINHEAD},
 {"BINTAIL",RED_IPF_BINTAIL},
 {"ARCSIN",RED_IPF_ARCSIN},
 {"ARCCOS",RED_IPF_ARCCOS},
 {"ARCTAN",RED_IPF_ARCTAN},
 {"MKSTRING",RED_IPF_MKSTRING}
};

static CharValue Symbol_TBL[] = {
 {"RED_PATTERN_DOT",RED_PATTERN_DOT},
 {"RED_PATTERN_DOTDOTDOT",RED_PATTERN_DOTDOTDOT},
 {"RED_PATTERN_DOTPLUS",RED_PATTERN_DOTPLUS},
 {"RED_PATTERN_STR_DOT",RED_PATTERN_STR_DOT},
 {"RED_PATTERN_STR_DOTDOTDOT",RED_PATTERN_STR_DOTDOTDOT},
 {"RED_PATTERN_STR_DOTPLUS",RED_PATTERN_STR_DOTPLUS}
};

static ParameterTypes Class_PT[] = {
 {FP_CHAR_VAL_TBL,&Class_TBL}
};

static ParameterTypes Constant_PT[] = {
 {FP_CHAR_VAL_TBL,&Constant_TBL}
};

static ParameterTypes Primfunc_PT[] = {
 {FP_CHAR_VAL_TBL,&Primfunc_TBL}
};

static ParameterTypes Type_PT[] = {
 {FP_CHAR_VAL_TBL,&Type_TBL}
};

static ParameterTypes Symbol_PT[] = {
 {FP_CHAR_VAL_TBL,&Symbol_TBL}
};

static ParameterTypes Termsy_PT[] = {{0,NULL}};
static ParameterTypes Define_PT[] = {{0,NULL}};
static ParameterTypes Application_PT[] = {{0,NULL}};
static ParameterTypes Binding_PT[] = {{0,NULL}};
static ParameterTypes Case_PT[] = {{0,NULL}};
static ParameterTypes Char_PT[] = {{0,NULL}};
static ParameterTypes Cond_PT[] = {{0,NULL}};
static ParameterTypes ErrorMessage_PT[] = {{0,NULL}};
static ParameterTypes Expr_PT[] = {{0,NULL}};
static ParameterTypes Expr3_PT[] = {{0,NULL}};
static ParameterTypes Funcall_PT[] = {{0,NULL}};
static ParameterTypes Fundef_PT[] = {{0,NULL}};
static ParameterTypes Ident_PT[] = {{0,NULL}};
static ParameterTypes Ifcond_PT[] = {{0,NULL}};
static ParameterTypes Let_PT[] = {{0,NULL}};
static ParameterTypes Letp_PT[] = {{0,NULL}};
static ParameterTypes Letpar_PT[] = {{0,NULL}};
static ParameterTypes Matrix_PT[] = {{0,NULL}};
static ParameterTypes Number_PT[] = {{0,NULL}};
static ParameterTypes Patternstring_PT[] = {{0,NULL}};
static ParameterTypes Protident_PT[] = {{0,NULL}};
static ParameterTypes String_PT[] = {{0,NULL}};
static ParameterTypes Substitute_PT[] = {{0,NULL}};
static ParameterTypes Tvector_PT[] = {{0,NULL}};
static ParameterTypes Usercon_PT[] = {{0,NULL}};
static ParameterTypes Vector_PT[] = {{0,NULL}};
static ParameterTypes When_PT[] = {{0,NULL}};
static ParameterTypes Patterns_PT[] = {{0,NULL}};
static ParameterTypes Aspattern_PT[] = {{0,NULL}};
static ParameterTypes Stringident_PT[] = {{0,NULL}};
static ParameterTypes Pattident_PT[] = {{0,NULL}};
static ParameterTypes Binlistconcat_PT[] = {{0,NULL}};
static ParameterTypes Patternstructure_PT[] = {{0,NULL}};
static ParameterTypes Datalist_PT[] ={{0,NULL}};

static TransformTable Create_TBL[] = {
 {NULL,"application",NT_APPLICATION,3,NODE_NODE2,Application_PT,1,1},
 {NULL,"aspattern",NT_ASPATTERN,3,NODE_NODE2,Aspattern_PT,-1,1},
 {NULL,"binding",NT_BINDING,3,NODE_NODE2,Binding_PT,-1,1},
 {NULL,"binlistconcat",NT_BINLISTCONCAT,3,NODE_NODE2,Binlistconcat_PT,1,1},
 {NULL,"case",NT_CASE,3,NODE_NODE2,Case_PT,-1,1},
 {NULL,"char",NT_CHAR,2,NODE_CHAR,Char_PT,1,1},
 {NULL,"class",NT_CLASS,2,NODE_VALUE,Class_PT,-1,1},
 {NULL,"cond",NT_COND,3,NODE_NODE2,Cond_PT,1,1},
 {NULL,"constant",NT_CONST,2,NODE_VALUE,Constant_PT,-1,1},
 {NULL,"datalist",NT_DATA_LIST,2,NODE_NODE,Datalist_PT,-1,1},
 {NULL,"define",NT_DEFINE,3,NODE_NODE2,Define_PT,1,1},
 {NULL,"error_message",NT_ERRORMESSAGE,2,NODE_NODE,ErrorMessage_PT,1,1},
 {NULL,"expr",NT_EXPR,2,NODE_NODE,Expr_PT,1,1},
 {NULL,"expr_3",NT_EXPR,4,NODE_NODE3,Expr3_PT,1,1},
 {NULL,"funcall",NT_FUNCALL,3,NODE_NODE2,Funcall_PT,-1,1},
 {NULL,"fundef",NT_FUNDEF,4,NODE_NODE3,Fundef_PT,1,1},
 {NULL,"ident",NT_IDENTIFIER,2,NODE_LEN|NODE_STRING,Ident_PT,1,1},
 {NULL,"ifcond",NT_IFCOND,3,NODE_NODE2,Ifcond_PT,1,1},
 {NULL,"let",NT_LET,3,NODE_NODE2,Let_PT,1,1},
 {NULL,"letp",NT_LETP,3,NODE_NODE2,Letp_PT,1,1},
 {NULL,"letpar",NT_LETPAR,3,NODE_NODE2,Letpar_PT,1,1},
 {NULL,"matrix",NT_MATRIX,3,NODE_LEN|NODE_NODE,Matrix_PT,-1,1},
 {NULL,"number",NT_NUMBER,2,NODE_STRING,Number_PT,1,1},
 {NULL,"patterns",NT_PATTERNS,2,NODE_NODE,Patterns_PT,-1,1},
 {NULL,"patternstring",NT_PATTERNSTRING,2,NODE_NODE,Patternstring_PT,1,1},
 {NULL,"patternstructure",NT_PATTERNSTRUCTURE,2,NODE_NODE,Patternstructure_PT,-1,1},
 {NULL,"pattident",NT_PATTIDENTIFIER,2,NODE_LEN|NODE_STRING,Pattident_PT,1,1},
 {NULL,"primfunc",NT_PRIMFUNC,2,NODE_VALUE,Primfunc_PT,-1,1},
 {NULL,"prot_ident",NT_PROTIDENT,3,NODE_LEN|NODE_STRING,Protident_PT,1,1},
 {NULL,"string",NT_STRING,2,NODE_NODE,String_PT,1,1},
 {NULL,"stringident",NT_STRINGIDENT,2,NODE_STRING,Stringident_PT,1,1},
 {NULL,"substitute",NT_SUBSTITUTE,3,NODE_NODE2,Substitute_PT,1,1},
 {NULL,"symbol",NT_SYMBOL,2,NODE_VALUE,Symbol_PT,-1,1},
 {NULL,"termsy",NT_TERMSY,2,NODE_STRING,Termsy_PT,-1,1},
 {NULL,"tvector",NT_TVECTOR,3,NODE_LEN|NODE_NODE,Tvector_PT,-1,1},
 {NULL,"type",NT_TYPE,2,NODE_VALUE,Type_PT,-1,1},
 {NULL,"usercon",NT_USERCON,3,NODE_NODE2,Usercon_PT,-1,1},
 {NULL,"vector",NT_VECTOR,3,NODE_LEN|NODE_NODE,Vector_PT,-1,1},
 {NULL,"when",NT_WHEN,4,NODE_NODE3,When_PT,1,1}
};

static int Create_TBL_LEN=sizeof(Create_TBL)/sizeof(TransformTable);
static int EditInfo_TBL_LEN=sizeof(EditInfo_TBL)/sizeof(char *);
static int NTInfo_TBL_LEN=sizeof(NTInfo_TBL)/sizeof(CharValue);
static int SimpleTrans_TBL_LEN=sizeof(SimpleTrans_TBL)/sizeof(SimpleTrans);

static CodeInfo *ListCodeInfo=NULL;
static CodeInfo *ExprCodeInfo=NULL;
static char BUF[256];
static TransformTable TT_tmp={NULL,NULL,0,0,0,NULL,0,0};
static SimpleTrans ST_tmp={NULL,NULL};

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

char *GetNT_Text(int id)
{ /* Return stringtext for a given nt identifier **/
 int i;
 for (i=0;i<NTInfo_TBL_LEN;i++)
  if (id==NTInfo_TBL[i].value) return (NTInfo_TBL[i].string);
 return (NULL);
} /******/

int IsEditInfo(char *name)
{ /* Tests, if name is legal edit info value *************************************/
 /*
 return (
  (bsearch(
   name,(char *)EditInfo_TBL,EditInfo_TBL_LEN,sizeof(char *),strcmp
  )==NULL) ? 0 : 1
 );
 */
 int i;
 for (i=0;i<EditInfo_TBL_LEN;i++) if (strcmp(name,EditInfo_TBL[i])==0) return (1);
 for (i=0;i<NTInfo_TBL_LEN;i++) if (strcmp(name,NTInfo_TBL[i].string)==0) return (1);
 return (0);
} /*******************************************************************************/

TransformTable *IsCreateFunc(char *name)
{ /* Tests, if name is a legal create function ***********************************/
 TT_tmp.name=name;
 return ((TransformTable*)bsearch(
  (char*)&TT_tmp,
  (char*)Create_TBL,
  Create_TBL_LEN,sizeof(TransformTable),
  CmpTransTbl)
 );
} /*******************************************************************************/

SimpleTrans *IsSimpleTransFunc(char *name)
{
 ST_tmp.name=name;
 return(
  (SimpleTrans *)bsearch(
   (char *)&ST_tmp,(char *)SimpleTrans_TBL,
   SimpleTrans_TBL_LEN,sizeof(SimpleTrans),MyStrcmp
  )
 );
}

int CheckFunctionCall(char *name,CCSymbolList *list,char *msg)
{
 if (name==NULL) {
  sprintf(msg,"Functionname ist missing.\n");
  return 0;
 }
 if ((strcmp(name,"MKLIST")==0) && (GetDimOfList(list)==2)) return 1;
 else {
  char *hlp=name;
  while (*hlp) {
   if (*hlp=='_') {
    TransformTable *tmp;
    hlp++;
    tmp=IsCreateFunc(hlp);
    if (tmp==NULL) break;
    else {
     if (GetDimOfList(list)!=tmp->nr_param) {
      sprintf(msg,"Functions <%s> needs %d parameters.\n",name,tmp->nr_param);
      return 0;
     } else return 1;
    }
   }
   hlp++;
  } 
  sprintf(msg,"<%s> unknow create function.\n",name);
  return 0;
 }
}

void AddCodeLine(CodeInfo *info,char *line,int value)
{ /* Add one code line to structure **********************************************/
 DBUG_ENTER("AddCodeLine");
 if (info) {                                     /* Exists this info structure ? */
  Code *result=(Code *)malloc(sizeof(Code));
  if (line==NULL) {                                      /* no string constant ? */
   result->ltype=LT_INT;                                     /* integer constant */
   result->U.value=value;
  } else {
   result->ltype=LT_STR;                                      /* string constant */
   result->U.line=(char *)my_strdup(line);                           /* Copy line ! */
  }
  result->next=NULL;                                             /* last element */
  info->nr_lines++;                             /* Increase number of code lines */
  if (info->last==NULL) {                                     /* First element ? */
   info->last=info->lines=result;                          /* First=Last element */
  } else {
   info->last->next=result;                                     /* chain to list */
   info->last=result;                          /* Actual element is last element */
  }
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void SetInsertCodeLine(CodeInfo *info)
{ /* Set insert pointer at actual last element ***********************************/
 DBUG_ENTER("SetInsertCodeLine");
 if (info) {                                              /* Infopointer valid ? */
  info->insertline=info->last;                /* Insert at last element (actual) */
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

void InsertCodeLine(CodeInfo *info,char *line,int value)
{ /* Insert one code line to structure *******************************************/
 DBUG_ENTER("InsertCodeLine");
 if (info) {                                     /* Exists this info structure ? */
  Code *result=(Code *)malloc(sizeof(Code));
  if (line==NULL) {                                      /* no string constant ? */
   result->ltype=LT_INT;                                     /* integer constant */
   result->U.value=value;
  } else {
   result->ltype=LT_STR;                                      /* string constant */
   result->U.line=(char *)my_strdup(line);                           /* Copy line ! */
  }
  result->next=info->insertline->next;                          /* chain element */
  info->insertline->next=result;
  info->nr_lines++;                             /* Increase number of code lines */
  if (info->last==info->insertline) {                          /* Last element ? */
   info->last=result;
  }
 }
 DBUG_VOID_RETURN;
} /*******************************************************************************/

CodeInfo *MkCodeInfo(void)
{ /* Make code info header *******************************************************/
 CodeInfo *result;
 DBUG_ENTER("MkCodeInfo");

 result=(CodeInfo *)malloc(sizeof(CodeInfo));
 result->nr_lines=0;                                                 /* no lines */
 result->last=result->lines=result->insertline=NULL;

 DBUG_RETURN(result);
} /*******************************************************************************/

void AddListCase(SyntaxRule *rule,char *listname,char *delimiter)
{
 CodeInfo *info;
 
 if (ListCodeInfo==NULL) ListCodeInfo=MkCodeInfo();

 if (strcmp(listname,"NT_EXPR")==0) {
  info=ExprCodeInfo=MkCodeInfo();
  strcpy(BUF,"default : { /* Expressionlist */");
 } else { 
  info=ListCodeInfo;  
  sprintf(BUF,"case %s : {",listname);
 }

 if (rule) {
  if (rule->format&&rule->format->format) {
   CCSymbolList *flist;
   CCSymbol *sy;
   flist=rule->format->format;                          /* Pointer to formatlist */
   AddCodeLine(info,BUF,0);
   while (flist) {
    sy=flist->symbol;
    if (sy) {
     switch (sy->type) {
      case CCTYPE_TABSY :
      case CCTYPE_NEWLINESY :
      case CCTYPE_SETTABSY :
      case CCTYPE_CLEARTABSY :
      case CCTYPE_INDENTSY :
      case CCTYPE_BINDSY :
       AddCodeLine(info,NULL,sy->type);
       break;
      case CCTYPE_LISTELEM :
       AddCodeLine(info," print_NODE(list->node);",0);
       break;
      case CCTYPE_LISTDELI :
       if (strlen(delimiter)>0) {
        sprintf(BUF," if (list->next) print_Delimiter(%c%s%c,%d);",
         34,delimiter,34,(int)strlen(delimiter));
        AddCodeLine(info,BUF,0);
       }
       break;
      default : break;
     }
    }
    flist=flist->next;
   }
   AddCodeLine(info," break;",0);
   AddCodeLine(info,"}",0);
  }
 }
} /*******************************************************************************/

static void WriteFFHeader(FILE *FH,char *name,char *variables)
{ /* Write header of one format function *****************************************/
 fprintf(FH,"void print_%s(NODE *node)\n",name);
 fprintf(FH,"{ /* Generated format function ! */\n");
 fprintf(FH,"%s",variables);
 fprintf(FH," DBUG_ENTER(%cprint_%s%c);\n\n",34,name,34);
 fprintf(FH," if (node) {\n");
} /*******************************************************************************/

static void WriteFFFooter(FILE *FH,char *name)
{ /* Write footer of one format function *****************************************/
 fprintf(FH," }\n");
 fprintf(FH,"\n DBUG_VOID_RETURN;\n");
 fprintf(FH,"} /* End of generated format function %s */\n",name);
} /*******************************************************************************/

static void WriteDummyFormatFunction(FILE *FH,TransformTable *tt)
{ /* Write one format function to file FH ****************************************/
 DBUG_ENTER("WriteDummyFormatFunction");

 DBUG_PRINT("Dummy",("Name=%s",tt->name));
 WriteFFHeader(FH,tt->name,"");
 fprintf(FH,"  print_NORMAL(node);\n");
 WriteFFFooter(FH,tt->name);

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static char *FFunctionString(int type)
{ /* Return functioncall string **************************************************/
 switch (type) {
  case CCTYPE_TABSY : return(" print_Tab();");break;
  case CCTYPE_NEWLINESY : return(" print_CarriageReturn();");break;
  case CCTYPE_SETTABSY : return(" print_SetTab();");break;
  case CCTYPE_CLEARTABSY :return(" print_ClearTab();");break;
  case CCTYPE_BINDSY :return(" print_BindSy();");break;
  case CCTYPE_INDENTSY : return(" print_Indent();");break;
  default : return(" /* Something going wrong */");break;
 }
} /*******************************************************************************/

static void WriteFormatFunction(FILE *FH,TransformTable *tt)
{ /* Write one format function to file FH ****************************************/
 DBUG_ENTER("WriteFormatFunction");

 if (tt->code->nr_lines>0) {                                     /* Code lines ? */
  Code *list;
  char *str;
  WriteFFHeader(FH,tt->name,"");

  list=tt->code->lines;                                         /* Start of list */
  while (list) {                                  /* traverse list of code lines */
   if (list->ltype==LT_INT) { /* Symbolic integer value ? */ 
    str=FFunctionString(list->U.value);
   } else str=list->U.line;
   fprintf(FH,"  %s\n",str);                               /* Write line to file */
   list=list->next;
   if (list==NULL) {
    fprintf(FH,"  else printf(%c** UNKNOWN NODEINFO (%%d) IN <%s> **%c,node->info);"
     ,34,tt->name,34);
   }
  }

  WriteFFFooter(FH,tt->name);
 } else WriteDummyFormatFunction(FH,tt);

 DBUG_VOID_RETURN;
} /*******************************************************************************/

static void WriteFormatListFunction(FILE *FH)
{ /* Writes the print node list function *****************************************/

 if ((ListCodeInfo==NULL)||(ListCodeInfo->lines==NULL)) { /* Write a dummy fct ? */
  WriteFFHeader(FH,"LIST"," NODEList *list;\n");
  WriteFFFooter(FH,"LIST");
  return;
 } else {
 Code *list=ListCodeInfo->lines;
 char *str;

 WriteFFHeader(FH,"LIST"," NODEList *list;\n");

 fprintf(FH,
  "  DBUG_PRINT(%cNODE:%c,(%cinfo=%%d , type=%%d%c,node->info,node->type));",
  34,34,34,34);
 fprintf(FH,"\n  if (node->U.list) {\n");
 fprintf(FH,"   int type=0;\n");
 fprintf(FH,"   list=node->U.list->first; /* First list element */\n");
 fprintf(FH,"   if (list) {\n");
 fprintf(FH,"    type=NODE_TYPE(list->node->type);\n");
 fprintf(FH,"    if (type==NT_DATA_LIST) type=list->node->info;\n");
 fprintf(FH,"   }\n");
 fprintf(FH,"   while (list) {\n");
 fprintf(FH,"    if (list->node) {\n");
 fprintf(FH,"     switch (type) {\n");

 while (list) {                                 /* Write code for all list cases */
  if (list->ltype==LT_INT) {                         /* Symbolic integer value ? */
   sprintf(BUF,"%s",FFunctionString(list->U.value));
   str=BUF;
  } else str=list->U.line;
  fprintf(FH,"      %s\n",str);
  list=list->next;
 }

 /* Special case for patternstrings (internal : list without delimiter) **********/
 fprintf(FH,"      case NT_STRINGIDENT : case NT_ASPATTERN : {\n");
 fprintf(FH,"       print_NODE(list->node);\n");
 fprintf(FH,"       break;\n");
 fprintf(FH,"      }\n");

 /* Write code for default case in list switch ***********************************/
 if (ExprCodeInfo) {                               /* Expression list definied ? */
  list=ExprCodeInfo->lines;              /* Make switch default case : expr-list */
  while (list) {                             /* Write code for exprlist handling */
   if (list->ltype==LT_INT) {                        /* Symbolic integer value ? */
    sprintf(BUF,"%s",FFunctionString(list->U.value));
    str=BUF;
   } else str=list->U.line;
   fprintf(FH,"      %s\n",str);
   list=list->next;
  }
 } else {                  /* No expression list definied. Write normallist code */
  fprintf(FH,"      default : {\n");
  fprintf(FH,"       printf(%c*DEFLIST*%c);\n",34,34);
  fprintf(FH,"       print_NODE(list->node);\n");
  fprintf(FH,"      }\n");
 }

 /* Write closing brackets, etc ... **********************************************/
 fprintf(FH,"     } /* End : switch */\n");
 fprintf(FH,"    } /* End : if */\n");
 fprintf(FH,"    list=list->next; /* Next list element */\n");
 fprintf(FH,"   } /* End : while */\n");
 fprintf(FH,"  } /* End : if */\n");
 WriteFFFooter(FH,"LIST");

 }
} /*******************************************************************************/

void WriteFormatMainNodeFunction(FILE *FH)
{ /* Generate main node for backtransformation **/
 int i;
 fprintf(FH,"\nvoid print_NODE(NODE *node)\n{\n");
 fprintf(FH," if (node) { /* Test node */\n");
 fprintf(FH,"  if ((node->type&NT_LIST)==NT_LIST) print_LIST(node); /* ExprList ? */\n");
 fprintf(FH,"  else {\n");
 fprintf(FH,"   switch (NODE_TYPE(node->type)) { /* Switch over all different node types */\n");

 for (i=0;i<Create_TBL_LEN;i++) {                /* Iterate all create functions */
  if (strcmp(Create_TBL[i].name,"expr_3")) /* Not expr_3 ? */
   fprintf(FH,"    case %s : print_%s(node);break;\n",
    GetNT_Text(Create_TBL[i].fct_id),Create_TBL[i].name);
 }

 fprintf(FH,"    default : printf(%c** UNKNOWN NODE (%%d) **%c,node->type);break;\n",34,34);
 fprintf(FH,"   }\n");
 fprintf(FH,"  }\n");
 fprintf(FH," }\n");
 fprintf(FH,"}\n");
} /*******/

void WriteFormatFunctions(char *filename)
{ /* Writes all format function to file filename *********************************/
 FILE *FH;
 char buf1[256],buf2[256];
 int i;
 DBUG_ENTER("WriteFormatFunctions");
 
 sprintf(buf1,"%s.format.h",filename);
 if ((FH=fopen(buf1,"w"))==NULL) {                    /* Access to file not ok ? */
  fprintf(stderr,"Can not open format output file <%s> !\n", buf1);
  DBUG_VOID_RETURN;
 }

 fprintf(FH,"#ifndef _COMP_FORMAT_\n");
 fprintf(FH,"#define _COMP_FORMAT_ 1\n"); 
 fprintf(FH,"#include %cmarco-create.h%c\n",34,34);
 fprintf(FH,"#include %cmarco-print.h%c\n\n",34,34);
 for (i=0;i<Create_TBL_LEN;i++) {                /* Iterate all create functions */
  fprintf(FH,"extern void print_%s(NODE *node);\n",Create_TBL[i].name);
 }
 fprintf(FH,"extern void print_LIST(NODE *node);\n");
 fprintf(FH,"extern void print_NODE(NODE *node);\n");
 fprintf(FH,"\n#endif\n");
 fclose(FH);

 sprintf(buf2,"%s.format.c",filename);
 if ((FH=fopen(buf2,"w"))==NULL) {                    /* Access to file not ok ? */
  fprintf(stderr,"Can not open format output file <%s> !\n", buf2);
  DBUG_VOID_RETURN;
 }

 fprintf(FH,"#include %cdbug.h%c\n\n",34,34);
 fprintf(FH,"#include %c%s%c\n\n",34,buf1,34);
 for (i=0;i<Create_TBL_LEN;i++) {                /* Iterate all create functions */
  if (Create_TBL[i].fct_id!=NT_PROTIDENT) {
   if (Create_TBL[i].code!=NULL) WriteFormatFunction(FH,&Create_TBL[i]);
   else WriteDummyFormatFunction(FH,&Create_TBL[i]);
  }
  fprintf(FH,"\n");                                       /* Add one newline ;-) */
 }

 WriteFormatListFunction(FH);
 WriteFormatMainNodeFunction(FH);
 fclose(FH);

 DBUG_VOID_RETURN;
} /*******************************************************************************/

/*********************************************************************************/
/********** LOCAL USED HELPFUNCTIONS *********************************************/ 
/*********************************************************************************/

static int CmpTransTbl(TransformTable *t1,TransformTable *t2)
{ /* Helpfunction to compare two TransformTable's ********************************/
 return strcmp(t1->name,t2->name);
} /*******************************************************************************/

static int MyStrcmp(SimpleTrans *s1,SimpleTrans *s2)
{
 return (strcmp(s1->name,s2->name));
}
