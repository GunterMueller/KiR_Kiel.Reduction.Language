/**********************************************************************************
***********************************************************************************
**
**   File        : marco-create.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 26.4.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Definitions for standart transformation functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

#ifndef _UDICREATE_HEADER_
#define _UDICREATE_HEADER_ 1

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

typedef struct NODETag NODE;                              /* Forward declaration */

typedef struct NODEListTag { /* List of syntax nodes ****************************/
 NODE *node;                                              /* Actual list element */
 struct NODEListTag *prev;                   /* Pointer to previous list element */
 struct NODEListTag *next;                      /* Pointer to next list element */
} NODEList; /********************************************************************/

typedef struct NODEListHeaderTag { /* Information for a node list ****************/
 NODEList *first;                                          /* First list element */
 NODEList *last;                                            /* Last list element */
} NODEListHeader; /***************************************************************/

typedef struct NODE2Tag { /* Two Nodes in One ************************************/
 NODE *n1;
 NODE *n2;
} NODE2; /************************************************************************/

typedef struct NODE3Tag { /* Three Nodes in One **********************************/
 NODE *n1;
 NODE *n2;
 NODE *n3;
} NODE3; /************************************************************************/

struct NODETag { /* Syntax node **************************************************/
 unsigned short type;                                               /* Node type */
 unsigned short info;                             /* Additional node information */
 int len;                   /* Length of union member (e.g. list len,string len) */
 union {                                                        /* Node value(s) */
  char character;                                               /* One character */
  char *str;                                          /* Characterstring pointer */
  int value;                                                     /* Number value */
  NODEListHeader *list;                                         /* List of nodes */
  NODE  *one;                                                        /* one NODE */
  NODE2 *tuple;                                                     /* two NODEs */
  NODE3 *triple;                                                  /* three NODEs */
 } U;                                                            /* Union - name */
}; /******************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

/* For the distributed version : *************************************************/
#ifndef D_DIST
#define _DAP               0 
#define P_DAP              0x0000000aL        
#define DAP                VARCON(_DAP)
#define T_DAP(x)           (((x) & P_DAP) == P_DAP)

#define T_DAP_1(x)         ((x & 0xffff00ff) == 0x0001000a)
#endif
/**********/

#define DEF_NR_NODES	25000

#ifdef NORMAL_MALLOC
#define ALLOCNODES(nr_nodes) 1
#define REUSENODES()	
#define FREENODES()	
#define ALLOCNODE()	(NODE *)malloc(sizeof(NODE))
#define ALLOCNODE2() 	(NODE2 *)malloc(sizeof(NODE2))
#define ALLOCNODE3() 	(NODE3 *)malloc(sizeof(NODE3))
#define ALLOCNODELIST()	(NODEList *)malloc(sizeof(NODEList))
#else
#define ALLOCNODES(nr_nodes) udi_AllocNodes(nr_nodes)
#define REUSENODES()	udi_ReUseNodes()
#define FREENODES()	udi_FreeNodes(1)
#define ALLOCNODE() 	AllocNode()
#define ALLOCNODE2() 	AllocNode2()
#define ALLOCNODE3() 	AllocNode3()
#define ALLOCNODELIST()	AllocNodeList()
#endif

/*********************************************************************************/

#define CalcNodeType(type,kind) type&~kind
#define NODE_TYPE(type)		(type&63)

/*********************************************************************************/

#define NT_ATOM			64
#define NT_NODE2		128
#define NT_NODE3		256
#define NT_LIST			512

#define NT_FIRST		0

#define NT_CLASS		1
#define NT_PRIMFUNC		2
#define NT_CONST		3
#define NT_MATRIX		4
#define NT_TVECTOR		5
#define NT_VECTOR		6
#define NT_APPLICATION		7
#define NT_ASPATTERN		8
#define NT_BINDING		9
#define NT_CASE			10
#define NT_CHAR			11
#define NT_COND			12
#define NT_DEFINE		13
#define NT_ERRORMESSAGE		14
#define NT_EXPR			15
#define NT_FUNCALL		16
#define NT_FUNDEF		17
#define NT_IFCOND		18
#define NT_LET			19
#define NT_LETP			20
#define NT_PATTERNS		21
#define NT_PATTERNSTRING	22
#define NT_PATTERNSTRUCTURE	23
#define NT_STRING		24
#define NT_SUBSTITUTE		25
#define NT_SYMBOL		26
#define NT_TYPE			27
#define NT_WHEN			28
#define NT_USERCON		29
#define NT_BINLISTCONCAT	30
#define NT_PATTBINDING		31
#define NT_NUMBER_LIST		32
#define NT_BOOL_LIST		33
#define NT_STRING_LIST		34
#define NT_DATA_LIST		35
#define NT_LETPAR		36
#define NT_FREE_STR		40
#define NT_IDENTIFIER		41
#define NT_NUMBER		42
#define NT_PATTIDENTIFIER	43
#define NT_STRINGIDENT		44
#define NT_PROTIDENT		45
#define NT_TERMSY		46
#define NT_LAST			47

/*********************************************************************************/

#define RED_CONST_NILMATRIX		1
#define RED_CONST_NILVECTOR 		2
#define RED_CONST_NILTVECTOR		3
#define RED_CONST_EMPTYLIST		4
#define RED_CONST_EMPTYSTRING		5
#define RED_BOOL_FALSE			6
#define RED_BOOL_TRUE			7
#define RED_CONST_FAIL			8
#define RED_CONST_OK			9

#define RED_TYPE_BOOL			1
#define RED_TYPE_NUMBER			2
#define RED_TYPE_PRIMFUNC		3
#define RED_TYPE_STRING			4
#define RED_TYPE_USERFUNC		5
#define RED_TYPE_MATRIX			6

#define RED_CLASS_FUNCTION		1
#define RED_CLASS_LIST			2
#define RED_CLASS_MATRIX		3
#define RED_CLASS_SCALAR		4
#define RED_CLASS_TVECTOR		5
#define RED_CLASS_VECTOR		6

/*********************************************************************************/

#define PF_INFIX	1
#define PF_PREFIX	2
#define PF_POSTFIX	4

#define _PREFIX_PF_	512
#define _SUFFIX_PF_	1024
#define PF_FUNCTION(function)	(function&(~(_PREFIX_PF_|_SUFFIX_PF_)))

#define RED_IPF_ABS 1
#define RED_PPF_ABS (_PREFIX_PF_|RED_IPF_ABS)
#define RED_SPF_ABS (_SUFFIX_PF_|RED_IPF_ABS)
#define RED_IPF_AND 2
#define RED_PPF_AND (_PREFIX_PF_|RED_IPF_AND)
#define RED_SPF_AND (_SUFFIX_PF_|RED_IPF_AND)
#define RED_IPF_ARCCOS 3
#define RED_PPF_ARCCOS (_PREFIX_PF_|RED_IPF_ARCCOS)
#define RED_SPF_ARCCOS (_SUFFIX_PF_|RED_IPF_ARCCOS)
#define RED_IPF_ARCSIN 4
#define RED_PPF_ARCSIN (_PREFIX_PF_|RED_IPF_ARCSIN)
#define RED_SPF_ARCSIN (_SUFFIX_PF_|RED_IPF_ARCSIN)
#define RED_IPF_ARCTAN 5
#define RED_PPF_ARCTAN (_PREFIX_PF_|RED_IPF_ARCTAN)
#define RED_SPF_ARCTAN (_SUFFIX_PF_|RED_IPF_ARCTAN)
#define RED_IPF_BINHEAD 6
#define RED_PPF_BINHEAD (_PREFIX_PF_|RED_IPF_BINHEAD)
#define RED_SPF_BINHEAD (_SUFFIX_PF_|RED_IPF_BINHEAD)
#define RED_IPF_BINTAIL 7
#define RED_PPF_BINTAIL (_PREFIX_PF_|RED_IPF_BINTAIL)
#define RED_SPF_BINTAIL (_SUFFIX_PF_|RED_IPF_BINTAIL)
#define RED_IPF_CDIV 8
#define RED_PPF_CDIV (_PREFIX_PF_|RED_IPF_CDIV)
#define RED_SPF_CDIV (_SUFFIX_PF_|RED_IPF_CDIV)
#define RED_IPF_CEIL 9
#define RED_PPF_CEIL (_PREFIX_PF_|RED_IPF_CEIL)
#define RED_SPF_CEIL (_SUFFIX_PF_|RED_IPF_CEIL)
#define RED_IPF_CLASS 10
#define RED_PPF_CLASS (_PREFIX_PF_|RED_IPF_CLASS)
#define RED_SPF_CLASS (_SUFFIX_PF_|RED_IPF_CLASS)
#define RED_IPF_CMAX 11
#define RED_PPF_CMAX (_PREFIX_PF_|RED_IPF_CMAX)
#define RED_SPF_CMAX (_SUFFIX_PF_|RED_IPF_CMAX)
#define RED_IPF_CMIN 12
#define RED_PPF_CMIN (_PREFIX_PF_|RED_IPF_CMIN)
#define RED_SPF_CMIN (_SUFFIX_PF_|RED_IPF_CMIN)
#define RED_IPF_CMINUS 13
#define RED_PPF_CMINUS (_PREFIX_PF_|RED_IPF_CMINUS)
#define RED_SPF_CMINUS (_SUFFIX_PF_|RED_IPF_CMINUS)
#define RED_IPF_CMULT 14
#define RED_PPF_CMULT (_PREFIX_PF_|RED_IPF_CMULT)
#define RED_SPF_CMULT (_SUFFIX_PF_|RED_IPF_CMULT)
#define RED_IPF_CONS 15
#define RED_PPF_CONS (_PREFIX_PF_|RED_IPF_CONS)
#define RED_SPF_CONS (_SUFFIX_PF_|RED_IPF_CONS)
#define RED_IPF_COS 16
#define RED_PPF_COS (_PREFIX_PF_|RED_IPF_COS)
#define RED_SPF_COS (_SUFFIX_PF_|RED_IPF_COS)
#define RED_IPF_CPLUS 17
#define RED_PPF_CPLUS (_PREFIX_PF_|RED_IPF_CPLUS)
#define RED_SPF_CPLUS (_SUFFIX_PF_|RED_IPF_CPLUS)
#define RED_IPF_CUT 18
#define RED_PPF_CUT (_PREFIX_PF_|RED_IPF_CUT)
#define RED_SPF_CUT (_SUFFIX_PF_|RED_IPF_CUT)
#define RED_IPF_DIM 19
#define RED_PPF_DIM (_PREFIX_PF_|RED_IPF_DIM)
#define RED_SPF_DIM (_SUFFIX_PF_|RED_IPF_DIM)
#define RED_IPF_DIV 20
#define RED_PPF_DIV (_PREFIX_PF_|RED_IPF_DIV)
#define RED_SPF_DIV (_SUFFIX_PF_|RED_IPF_DIV)
#define RED_IPF_EMPTY 21
#define RED_PPF_EMPTY (_PREFIX_PF_|RED_IPF_EMPTY)
#define RED_SPF_EMPTY (_SUFFIX_PF_|RED_IPF_EMPTY)
#define RED_IPF_EQ 22
#define RED_PPF_EQ (_PREFIX_PF_|RED_IPF_EQ)
#define RED_SPF_EQ (_SUFFIX_PF_|RED_IPF_EQ)
#define RED_IPF_EXP 23
#define RED_PPF_EXP (_PREFIX_PF_|RED_IPF_EXP)
#define RED_SPF_EXP (_SUFFIX_PF_|RED_IPF_EXP)
#define RED_IPF_FEQ 24
#define RED_PPF_FEQ (_PREFIX_PF_|RED_IPF_FEQ)
#define RED_SPF_FEQ (_SUFFIX_PF_|RED_IPF_FEQ)
#define RED_IPF_FLOOR 25
#define RED_PPF_FLOOR (_PREFIX_PF_|RED_IPF_FLOOR)
#define RED_SPF_FLOOR (_SUFFIX_PF_|RED_IPF_FLOOR)
#define RED_IPF_FNE 26
#define RED_PPF_FNE (_PREFIX_PF_|RED_IPF_FNE)
#define RED_SPF_FNE (_SUFFIX_PF_|RED_IPF_FNE)
#define RED_IPF_FRAC 27
#define RED_PPF_FRAC (_PREFIX_PF_|RED_IPF_FRAC)
#define RED_SPF_FRAC (_SUFFIX_PF_|RED_IPF_FRAC)
#define RED_IPF_GE 28
#define RED_PPF_GE (_PREFIX_PF_|RED_IPF_GE)
#define RED_SPF_GE (_SUFFIX_PF_|RED_IPF_GE)
#define RED_IPF_GT 29
#define RED_PPF_GT (_PREFIX_PF_|RED_IPF_GT)
#define RED_SPF_GT (_SUFFIX_PF_|RED_IPF_GT)
#define RED_IPF_HEAD 30
#define RED_PPF_HEAD (_PREFIX_PF_|RED_IPF_HEAD)
#define RED_SPF_HEAD (_SUFFIX_PF_|RED_IPF_HEAD)
#define RED_IPF_IP 31
#define RED_PPF_IP (_PREFIX_PF_|RED_IPF_IP)
#define RED_SPF_IP (_SUFFIX_PF_|RED_IPF_IP)
#define RED_IPF_LCUT 32
#define RED_PPF_LCUT (_PREFIX_PF_|RED_IPF_LCUT)
#define RED_SPF_LCUT (_SUFFIX_PF_|RED_IPF_LCUT)
#define RED_IPF_LDIM 33
#define RED_PPF_LDIM (_PREFIX_PF_|RED_IPF_LDIM)
#define RED_SPF_LDIM (_SUFFIX_PF_|RED_IPF_LDIM)
#define RED_IPF_LE 34
#define RED_PPF_LE (_PREFIX_PF_|RED_IPF_LE)
#define RED_SPF_LE (_SUFFIX_PF_|RED_IPF_LE)
#define RED_IPF_LN 35
#define RED_PPF_LN (_PREFIX_PF_|RED_IPF_LN)
#define RED_SPF_LN (_SUFFIX_PF_|RED_IPF_LN)
#define RED_IPF_LREPLACE 36
#define RED_PPF_LREPLACE (_PREFIX_PF_|RED_IPF_LREPLACE)
#define RED_SPF_LREPLACE (_SUFFIX_PF_|RED_IPF_LREPLACE)
#define RED_IPF_LROTATE 37
#define RED_PPF_LROTATE (_PREFIX_PF_|RED_IPF_LROTATE)
#define RED_SPF_LROTATE (_SUFFIX_PF_|RED_IPF_LROTATE)
#define RED_IPF_LSELECT 38
#define RED_PPF_LSELECT (_PREFIX_PF_|RED_IPF_LSELECT)
#define RED_SPF_LSELECT (_SUFFIX_PF_|RED_IPF_LSELECT)
#define RED_IPF_LT 39
#define RED_PPF_LT (_PREFIX_PF_|RED_IPF_LT)
#define RED_SPF_LT (_SUFFIX_PF_|RED_IPF_LT)
#define RED_IPF_LTRANSFORM 40
#define RED_PPF_LTRANSFORM (_PREFIX_PF_|RED_IPF_LTRANSFORM)
#define RED_SPF_LTRANSFORM (_SUFFIX_PF_|RED_IPF_LTRANSFORM)
#define RED_IPF_LUNITE 41
#define RED_PPF_LUNITE (_PREFIX_PF_|RED_IPF_LUNITE)
#define RED_SPF_LUNITE (_SUFFIX_PF_|RED_IPF_LUNITE)
#define RED_IPF_MAX 42
#define RED_PPF_MAX (_PREFIX_PF_|RED_IPF_MAX)
#define RED_SPF_MAX (_SUFFIX_PF_|RED_IPF_MAX)
#define RED_IPF_MCUT 43
#define RED_PPF_MCUT (_PREFIX_PF_|RED_IPF_MCUT)
#define RED_SPF_MCUT (_SUFFIX_PF_|RED_IPF_MCUT)
#define RED_IPF_MDIM 44
#define RED_PPF_MDIM (_PREFIX_PF_|RED_IPF_MDIM)
#define RED_SPF_MDIM (_SUFFIX_PF_|RED_IPF_MDIM)
#define RED_IPF_MIN 45
#define RED_PPF_MIN (_PREFIX_PF_|RED_IPF_MIN)
#define RED_SPF_MIN (_SUFFIX_PF_|RED_IPF_MIN)
#define RED_IPF_MINUS 46
#define RED_PPF_MINUS (_PREFIX_PF_|RED_IPF_MINUS)
#define RED_SPF_MINUS (_SUFFIX_PF_|RED_IPF_MINUS)
#define RED_IPF_MKSTRING 47
#define RED_PPF_MKSTRING (_PREFIX_PF_|RED_IPF_MKSTRING)
#define RED_SPF_MKSTRING (_SUFFIX_PF_|RED_IPF_MKSTRING)
#define RED_IPF_MOD 48
#define RED_PPF_MOD (_PREFIX_PF_|RED_IPF_MOD)
#define RED_SPF_MOD (_SUFFIX_PF_|RED_IPF_MOD)
#define RED_IPF_MREPLACE 49
#define RED_PPF_MREPLACE (_PREFIX_PF_|RED_IPF_MREPLACE)
#define RED_SPF_MREPLACE (_SUFFIX_PF_|RED_IPF_MREPLACE)
#define RED_IPF_MREPLACE_C 50
#define RED_PPF_MREPLACE_C (_PREFIX_PF_|RED_IPF_MREPLACE_C)
#define RED_SPF_MREPLACE_C (_SUFFIX_PF_|RED_IPF_MREPLACE_C)
#define RED_IPF_MREPLACE_R 51
#define RED_PPF_MREPLACE_R (_PREFIX_PF_|RED_IPF_MREPLACE_R)
#define RED_SPF_MREPLACE_R (_SUFFIX_PF_|RED_IPF_MREPLACE_R)
#define RED_IPF_MROTATE 52
#define RED_PPF_MROTATE (_PREFIX_PF_|RED_IPF_MROTATE)
#define RED_SPF_MROTATE (_SUFFIX_PF_|RED_IPF_MROTATE)
#define RED_IPF_MSELECT 53
#define RED_PPF_MSELECT (_PREFIX_PF_|RED_IPF_MSELECT)
#define RED_SPF_MSELECT (_SUFFIX_PF_|RED_IPF_MSELECT)
#define RED_IPF_MULT 54
#define RED_PPF_MULT (_PREFIX_PF_|RED_IPF_MULT)
#define RED_SPF_MULT (_SUFFIX_PF_|RED_IPF_MULT)
#define RED_IPF_MUNITE 55
#define RED_PPF_MUNITE (_PREFIX_PF_|RED_IPF_MUNITE)
#define RED_SPF_MUNITE (_SUFFIX_PF_|RED_IPF_MUNITE)
#define RED_IPF_NE 56
#define RED_PPF_NE (_PREFIX_PF_|RED_IPF_NE)
#define RED_SPF_NE (_SUFFIX_PF_|RED_IPF_NE)
#define RED_IPF_NEG 57
#define RED_PPF_NEG (_PREFIX_PF_|RED_IPF_NEG)
#define RED_SPF_NEG (_SUFFIX_PF_|RED_IPF_NEG)
#define RED_IPF_NOT 58
#define RED_PPF_NOT (_PREFIX_PF_|RED_IPF_NOT)
#define RED_SPF_NOT (_SUFFIX_PF_|RED_IPF_NOT)
#define RED_IPF_OR 59
#define RED_PPF_OR (_PREFIX_PF_|RED_IPF_OR)
#define RED_SPF_OR (_SUFFIX_PF_|RED_IPF_OR)
#define RED_IPF_PLUS 60
#define RED_PPF_PLUS (_PREFIX_PF_|RED_IPF_PLUS)
#define RED_SPF_PLUS (_SUFFIX_PF_|RED_IPF_PLUS)
#define RED_IPF_PLUSPLUS 61
#define RED_PPF_PLUSPLUS (_PREFIX_PF_|RED_IPF_PLUSPLUS)
#define RED_SPF_PLUSPLUS (_SUFFIX_PF_|RED_IPF_PLUSPLUS)
#define RED_IPF_REPSTR 62
#define RED_PPF_REPSTR (_PREFIX_PF_|RED_IPF_REPSTR)
#define RED_SPF_REPSTR (_SUFFIX_PF_|RED_IPF_REPSTR)
#define RED_IPF_REVERSE 63
#define RED_PPF_REVERSE (_PREFIX_PF_|RED_IPF_REVERSE)
#define RED_SPF_REVERSE (_SUFFIX_PF_|RED_IPF_REVERSE)
#define RED_IPF_ROTATE 64
#define RED_PPF_ROTATE (_PREFIX_PF_|RED_IPF_ROTATE)
#define RED_SPF_ROTATE (_SUFFIX_PF_|RED_IPF_ROTATE)
#define RED_IPF_SELECT 65
#define RED_PPF_SELECT (_PREFIX_PF_|RED_IPF_SELECT)
#define RED_SPF_SELECT (_SUFFIX_PF_|RED_IPF_SELECT)
#define RED_IPF_SIN 66
#define RED_PPF_SIN (_PREFIX_PF_|RED_IPF_SIN)
#define RED_SPF_SIN (_SUFFIX_PF_|RED_IPF_SIN)
#define RED_IPF_SUBSTR 67
#define RED_PPF_SUBSTR (_PREFIX_PF_|RED_IPF_SUBSTR)
#define RED_SPF_SUBSTR (_SUFFIX_PF_|RED_IPF_SUBSTR)
#define RED_IPF_SUBSTRING 68
#define RED_PPF_SUBSTRING (_PREFIX_PF_|RED_IPF_SUBSTRING)
#define RED_SPF_SUBSTRING (_SUFFIX_PF_|RED_IPF_SUBSTRING)
#define RED_IPF_TAIL 69
#define RED_PPF_TAIL (_PREFIX_PF_|RED_IPF_TAIL)
#define RED_SPF_TAIL (_SUFFIX_PF_|RED_IPF_TAIL)
#define RED_IPF_TAN 70
#define RED_PPF_TAN (_PREFIX_PF_|RED_IPF_TAN)
#define RED_SPF_TAN (_SUFFIX_PF_|RED_IPF_TAN)
#define RED_IPF_TOFIELD 71
#define RED_PPF_TOFIELD (_PREFIX_PF_|RED_IPF_TOFIELD)
#define RED_SPF_TOFIELD (_SUFFIX_PF_|RED_IPF_TOFIELD)
#define RED_IPF_TOLIST 72
#define RED_PPF_TOLIST (_PREFIX_PF_|RED_IPF_TOLIST)
#define RED_SPF_TOLIST (_SUFFIX_PF_|RED_IPF_TOLIST)
#define RED_IPF_TOMAT 73
#define RED_PPF_TOMAT (_PREFIX_PF_|RED_IPF_TOMAT)
#define RED_SPF_TOMAT (_SUFFIX_PF_|RED_IPF_TOMAT)
#define RED_IPF_TOSCALAR 74
#define RED_PPF_TOSCALAR (_PREFIX_PF_|RED_IPF_TOSCALAR)
#define RED_SPF_TOSCALAR (_SUFFIX_PF_|RED_IPF_TOSCALAR)
#define RED_IPF_TOTVECT 75
#define RED_PPF_TOTVECT (_PREFIX_PF_|RED_IPF_TOTVECT)
#define RED_SPF_TOTVECT (_SUFFIX_PF_|RED_IPF_TOTVECT)
#define RED_IPF_TOVECT 76
#define RED_PPF_TOVECT (_PREFIX_PF_|RED_IPF_TOVECT)
#define RED_SPF_TOVECT (_SUFFIX_PF_|RED_IPF_TOVECT)
#define RED_IPF_TRANSPOSE 77
#define RED_PPF_TRANSPOSE (_PREFIX_PF_|RED_IPF_TRANSPOSE)
#define RED_SPF_TRANSPOSE (_SUFFIX_PF_|RED_IPF_TRANSPOSE)
#define RED_IPF_TRUNC 78
#define RED_PPF_TRUNC (_PREFIX_PF_|RED_IPF_TRUNC)
#define RED_SPF_TRUNC (_SUFFIX_PF_|RED_IPF_TRUNC)
#define RED_IPF_TYPE 79
#define RED_PPF_TYPE (_PREFIX_PF_|RED_IPF_TYPE)
#define RED_SPF_TYPE (_SUFFIX_PF_|RED_IPF_TYPE)
#define RED_IPF_UNITE 80
#define RED_PPF_UNITE (_PREFIX_PF_|RED_IPF_UNITE)
#define RED_SPF_UNITE (_SUFFIX_PF_|RED_IPF_UNITE)
#define RED_IPF_VCUT 81
#define RED_PPF_VCUT (_PREFIX_PF_|RED_IPF_VCUT)
#define RED_SPF_VCUT (_SUFFIX_PF_|RED_IPF_VCUT)
#define RED_IPF_VDIM 82
#define RED_PPF_VDIM (_PREFIX_PF_|RED_IPF_VDIM)
#define RED_SPF_VDIM (_SUFFIX_PF_|RED_IPF_VDIM)
#define RED_IPF_VECTORDIV 83
#define RED_PPF_VECTORDIV (_PREFIX_PF_|RED_IPF_VECTORDIV)
#define RED_SPF_VECTORDIV (_SUFFIX_PF_|RED_IPF_VECTORDIV)
#define RED_IPF_VECTORMINUS 84
#define RED_PPF_VECTORMINUS (_PREFIX_PF_|RED_IPF_VECTORMINUS)
#define RED_SPF_VECTORMINUS (_SUFFIX_PF_|RED_IPF_VECTORMINUS)
#define RED_IPF_VECTORMULT 85
#define RED_PPF_VECTORMULT (_PREFIX_PF_|RED_IPF_VECTORMULT)
#define RED_SPF_VECTORMULT (_SUFFIX_PF_|RED_IPF_VECTORMULT)
#define RED_IPF_VECTORPLUS 86
#define RED_PPF_VECTORPLUS (_PREFIX_PF_|RED_IPF_VECTORPLUS)
#define RED_SPF_VECTORPLUS (_SUFFIX_PF_|RED_IPF_VECTORPLUS)
#define RED_IPF_VROTATE 87
#define RED_PPF_VROTATE (_PREFIX_PF_|RED_IPF_VROTATE)
#define RED_SPF_VROTATE (_SUFFIX_PF_|RED_IPF_VROTATE)
#define RED_IPF_VSELECT 88
#define RED_PPF_VSELECT (_PREFIX_PF_|RED_IPF_VSELECT)
#define RED_SPF_VSELECT (_SUFFIX_PF_|RED_IPF_VSELECT)
#define RED_IPF_VUNITE 89
#define RED_PPF_VUNITE (_PREFIX_PF_|RED_IPF_VUNITE)
#define RED_SPF_VUNITE (_SUFFIX_PF_|RED_IPF_VUNITE)
#define RED_IPF_XOR 90
#define RED_PPF_XOR (_PREFIX_PF_|RED_IPF_XOR)
#define RED_SPF_XOR (_SUFFIX_PF_|RED_IPF_XOR)
#define RED_IPF_QUOT 91
#define RED_PPF_QUOT (_PREFIX_PF_|RED_IPF_QUOT)
#define RED_SPF_QUOT (_SUFFIX_PF_|RED_IPF_QUOT)

/*********************************************************************************/

#define RED_PATTERN_DOT			1
#define RED_PATTERN_DOTDOTDOT		2
#define RED_PATTERN_DOTPLUS		3
#define RED_PATTERN_STR_DOT		4
#define RED_PATTERN_STR_DOTDOTDOT	5
#define RED_PATTERN_STR_DOTPLUS		6

/*********************************************************************************/

#define EDIT_DUMMY			0
#define EDIT_PREFIX_FC			1
#define EDIT_PRIMITIVE_FC		2
#define EDIT_USER_FC			3
#define EDIT_BOOL_MAT			4
#define EDIT_BOOL_TVECT			5
#define EDIT_BOOL_VECT			6
#define EDIT_MATRIX_MAT			7
#define EDIT_NUMBER_MAT			8
#define EDIT_NUMBER_TVECT		9
#define EDIT_NUMBER_VECT		10
#define EDIT_STRING_MAT			11
#define EDIT_STRING_TVECT		12
#define EDIT_STRING_VECT		13
#define EDIT_PATTERN_BINDING		14
#define EDIT_NORMAL_BINDING		15
#define EDIT_ASIN_SPAT			16
#define EDIT_AS_PAT			17
#define EDIT_PATSTR_LIST		18
#define EDIT_PATSTR_MATRIX		19
#define EDIT_PATSTR_VECTOR		20
#define EDIT_PATSTR_STRING		21
#define EDIT_PATTERN			22
#define EDIT_PATTERN_LIST		23
#define EDIT_CASE			24
#define EDIT_CASE_OTHERWISE		25
#define EDIT_EXPR_LIST			26
#define EDIT_NORMAL_USERCON		27
#define EDIT_PATTERN_USERCON		28
#define EDIT_AS_STRPAT			29
#define EDIT_AS_SPAT			30
#define EDIT_IN_SPAT			31

/*********************************************************************************/
/********** GLOBAL MODULE VARIABLES **********************************************/
/*********************************************************************************/

extern NODE *GOALEXPR;                                         /* Mainexpression */

/*********************************************************************************/
/********** PROTOTYPES FOR EXTERNAL FUNCTIONS ************************************/
/*********************************************************************************/

/* Allocates node buffer for number elements *************************************/
int udi_AllocNodes(int nr_nodes);

extern NODE *AllocNode(void);
extern NODE2 *AllocNode2(void);
extern NODE3 *AllocNode3(void);
extern NODEList *AllocNodeList(void);

extern void udi_ReUseNodes(void);
extern void udi_FreeNodes(int delete);

extern NODE *udi_Mk_ListHead (NODE *p1) ;
extern NODE *udi_Mk_ListTail (NODE *p1,NODE *p2) ;
extern NODE *udi_Mk_SetListType(NODE *P1,int type);

extern NODE *udi_create_application (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_aspattern (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_binding (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_case (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_char (int edit_info,char p1) ;
extern NODE *udi_create_class (int edit_info,int p1) ;
extern NODE *udi_create_cond (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_constant (int edit_info,int p1) ;
extern NODE *udi_create_define (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_datalist (int edit_info,NODE *p1) ;
extern NODE *udi_create_error_message (int edit_info,NODE *p1) ;
extern NODE *udi_create_expr (int edit_info,NODE *p1) ;
extern NODE *udi_create_expr_3 (int edit_info,NODE *p1,NODE *p2,NODE *p3) ;
extern NODE *udi_create_funcall (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_fundef (int edit_info,NODE *p1,NODE *p2,NODE *p3) ;
extern NODE *udi_create_ident (int edit_info,char *p1) ;
extern NODE *udi_create_patident (int edit_info,char *p1) ;
extern NODE *udi_create_ifcond (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_let(int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_letp(int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_letpar(int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_matrix (int edit_info,int p1,NODE *p2) ;
extern NODE *udi_create_number (int edit_info,char *p1) ;
extern NODE *udi_create_patterns (int edit_info,NODE *p1) ;
extern NODE *udi_create_patternstring (int edit_info,NODE *p1) ;
extern NODE *udi_create_patternstructure (int edit_info,NODE *p1) ;
extern NODE *udi_create_primfunc (int edit_info,int p1) ;
extern NODE *udi_create_prot_ident (int edit_info,int p1,char *p2) ;
extern NODE *udi_create_string (int edit_info,NODE *p1) ;
extern NODE *udi_create_stringident (int edit_info,char *p1) ;
extern NODE *udi_create_substitute (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_symbol (int edit_info,int p1) ;
extern NODE *udi_create_termsy (int edit_info,char *p1) ;
extern NODE *udi_create_tvector (int edit_info,int p1,NODE *p2) ;
extern NODE *udi_create_type (int edit_info,int p1) ;
extern NODE *udi_create_vector (int edit_info,int p1,NODE *p2) ;
extern NODE *udi_create_when (int edit_info,NODE *p1,NODE *p2,NODE *p3) ;
extern NODE *udi_create_usercon (int edit_info,NODE *p1,NODE *p2) ;
extern NODE *udi_create_binlistconcat (int edit_info,NODE *p1,NODE *p2) ;

#endif
