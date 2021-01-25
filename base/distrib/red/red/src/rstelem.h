/* $Log: rstelem.h,v $
 * Revision 1.26  1998/06/17 15:27:06  rs
 * support for train primitives as interactions
 *
 * Revision 1.25  1998/02/18  13:34:36  rs
 * modifications for speculative evaluations regarding
 * meaning preserving transformations
 *
 * Revision 1.24  1997/06/06  09:13:11  rs
 * some more (complete) modifications concerning speculative evaluation
 * (don't say you have missed my talk ? ;-)
 *
 * Revision 1.23  1997/02/17  16:19:13  stt
 * unused element FSCANF replaced by FREDIRECT
 *
 * Revision 1.22  1997/02/17  13:18:12  rs
 * some additions supporting speculative evaluation
 *
 * Revision 1.21  1996/03/29  16:00:12  stt
 * Macro changed: T_POINTER != NULL for !WITHTILDE
 *
 * Revision 1.20  1996/02/28  11:02:30  stt
 * stack elements TO_CHAR, TO_ORD, SPRINTF and SSCANF added.
 *
 * Revision 1.19  1996/02/21  18:17:22  cr
 * prototype implementation of interactions for arbitrary expressions
 * uses modified send/receive-routines from rmessage.c
 * compile with -DSTORE=1 to get the prototype (interactions get/put)
 * or with -DSTORE=0 to get the original files.
 * involved files so far: rheap.c riafunc.c rstelem.h rmessage.c
 * rmessage.c has to be compiled once with -DSTORE=1 to get the
 * modified send/receive-routines, and perhaps a second time with
 * -DSTORE=0 to get its original functionality for the distributed
 * versions.
 *
 * Revision 1.18  1996/02/20  13:26:03  cr
 * added interactions get and put
 *
 * Revision 1.17  1996/02/07  18:22:35  stt
 * GET_INT macro added.
 *
 * Revision 1.16  1996/01/08  15:36:59  stt
 * new interactions bind and unit inserted
 * fstatus renamed to finfo
 * arity of interactions decreased for monadic i/o
 *
 * Revision 1.15  1995/12/11  18:25:10  cr
 * support for empty frames: NILSTRUCT
 *
 * Revision 1.14  1995/12/11  13:13:46  cr
 * two more operations on frames: FR_SLOTS,FR_DELETE
 *
 * Revision 1.13  1995/11/15  15:12:46  cr
 * introduced frames (STRUCT), slots (ENTRY) and operations on frames
 * FR_UPDATE,FR_TEST,FR_SELECT moved to positions specified in cencod.h
 *
 * Revision 1.12  1995/10/25  10:56:57  stt
 * arity of IA_EOF corrected.
 * Macro for tagging a char as a string element.
 *
 * Revision 1.11  1995/09/20  14:45:12  stt
 * set arity of IA_EXIT to 1
 *
 * Revision 1.10  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.9  1995/08/18  11:38:26  stt
 * description of constructors included
 *
 * Revision 1.8  1995/03/23  13:25:50  cr
 * renamed: F_UPDATE,F_SELECT,F_TEST to FR_UPDATE,FR_SELECT,FR_TEST
 * (to avoid name clash with gcclib/.../includes/...)
 *
 * Revision 1.7  1995/03/22  15:04:02  cr
 * primitive functions on frames: F_UPDATE,F_SELECT,F_TEST
 *
 * Revision 1.6  1994/06/11  12:59:32  ach
 * support for some strange constructions from old preprocessor added
 *
 * Revision 1.5  1994/03/29  18:23:36  mah
 * GUARD and NOMAT adjusted
 *
 * Revision 1.4  1994/01/18  14:37:58  mah
 * POST_SNAP added
 *
 * Revision 1.3  1994/01/04  13:59:06  mah
 * dollar_tilde added
 * some defines for lneu postprocessor added
 *
 * Revision 1.2  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/*****************************************************************************/
/* file rstelem.h   encoding of stackelements                                */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* new coding of stackelements: (dg)                                         */
/*                                                                           */
/*                 31        16 15       8 7          0                      */
/*                +------------+----------+------------+                     */
/* stackelement:  | value-field edit-field class-field |                     */
/*                +------------+----------+------------+                     */
/*                                                                           */
/*                 31                            3  0                        */
/*                +-----------------------------+----+                       */
/* pointer:       |           address            0000|                       */
/*                +-----------------------------+----+                       */
/*                                                                           */
/*                 31                             1 0                        */
/*                +--------------------------------+-+                       */
/* integers:      |         integer-value           1|                       */
/*                +--------------------------------+-+                       */
/*                                                                           */
/*                 31        16 15       8 7   4 3  0                        */
/*                +------------+----------+-----+----+                       */
/* constructor:   |   arity        edit    name  cf10|  c: chain flag        */
/*                +------------+----------+-----+----+  f: fixvar flag       */
/*                                                                           */
/*                 31 24 23  16 15       8 7   4 3  0                        */
/*                +-----+------+----------+-----+----+ xyz=00*: false/true   */
/* constants:     |group  name     edit     0xyz 0100| xyz=01*: num/dollar   */
/*                +-----+------+----------+-----+----+ xyz=111: normal_const */
/*                                                                           */
/*                 31 24 23  16 15       8 7   4 3  0                        */
/*                +-----+------+----------+-----+----+ Bit 31:               */
/* functions:     |       name     edit    arity 1000| 0: primitive Funk.    */
/*                +-----+------+----------+-----+----+ 1: Interaktion        */
/*                                                                           */
/*                 31        16 15       8 7   4 3  0                        */
/*                +------------+----------+-----+----+ x: letter/digit       */
/* strings:       | ASCII-value    edit     0xyz 1100| y: last/notlast       */
/*                +------------+----------+-----+----+ z: variable/string    */
/*                                                                           */
/*****************************************************************************/

/* defines: REAL and INTEGER are not stackelements */

#define INTEGER            0
#define REAL               1

/*********************************************************************/
/*                                                                   */
/*        V A L U E - F E L D   M A S K E N                          */
/*                                                                   */
/*********************************************************************/

#define F_VALUE            0xFFFF0000     /* Bit 16-31 */
#define O_VALUE            16             /* Offset 16 */

#define VALUE(x)           ((x) >> O_VALUE)
#define SET_VALUE(x,n)     (((x) & ~F_VALUE) | ((n) << O_VALUE))

#define EXTID(x)           ((x) & ~F_VALUE)

/*********************************************************************/
/*                                                                   */
/*              E D I T - F E L D   M A S K E N                      */
/*                                                                   */
/*********************************************************************/

#define F_EDIT             0x0000FF00     /* Bits 15-8 */
#define O_EDIT             8              /* Offset 8  */

#define SET_EDIT(x)        (((x) << O_EDIT) & F_EDIT) /* set edit-field  */
#define EXTR_EDIT(x)       (((x) & F_EDIT) >> O_EDIT) /* extract edit-field */

/*********************************************************************/
/*                                                                   */
/*            C L A S S - F E L D   M A S K E N                      */
/*                                                                   */
/*********************************************************************/

#define F_CLASS            0x000000FF     /* Bits 7-0  */
#define F_TYPE             0x0000000F     /* Bits 3-0  */
#define O_TYPE             0              /* Offset 0  */

/*********************************************************************/
/*                                                                   */
/* tags, tag-fields, test-makros                                     */
/*                                                                   */
/*********************************************************************/

/*********************************************************************/
/* Allgemeiner Test der Stackelemente(STE) auf ihren Typ             */
/*********************************************************************/

#define STE_INT			0
#define STE_BOOL		1
#define STE_POINTER		2
#define STE_OTHER		3

#define TEST_TYPE(x)	   (T_INT(x)     ? STE_INT : \
                           (T_BOOLEAN(x) ? STE_BOOL : \
                           (T_POINTER(x) ? STE_POINTER : STE_OTHER )))

/**********************************************************************/
/* Konvertierung der Stackelementdarstellung; einige alte Kodierungen */
/* siehe new_encod() und old_encod() in rreduce.c                     */
/**********************************************************************/

#define OLD_SA_FALSE    0x01050003L
#define OLD_SA_TRUE     0x02050003L

#define OLD_MA_MASK     0xffffff10L

#define OLD_POINTER     0x00000000L
#define OLD_FIX_CONSTR  0x0000000cL
#define OLD_VAR_CONSTR  0x00000008L
#define OLD_FIX_CCONSTR 0x0000000eL
#define OLD_VAR_CCONSTR 0x0000000aL
#define OLD_CONSTANT    0x00000003L
#define OLD_FUNC        0x00000001L
#define OLD_STRING      0x00000004L


/*************************************/
/* pointers, tag: 0000               */
/*************************************/

#define POINTER            0x00000000
#define P_POINTER          0x0000000f

#ifndef T_POINTER
/* mah 111193: NULL ist kein Pointer! */
/* t_pointer ist auch in rstdinc,h definiert und wird i.d.R. von dort */
/* genommen. Keine Unterscheidung mehr nach WITHTILDE.  stt 29.03.96 */
#define T_POINTER(x)   (((int)(x) != 0) && (((int)(x) & P_POINTER) == POINTER))
#endif

#define IS_POINTER(x)      (((int)(x) & P_POINTER) == POINTER)

/*************************************/
/* integers, tag: 1                  */
/*************************************/

#define INT                0x00000001
#define P_INT              0x00000001
#define T_INT(x)           (((x) & P_INT) == INT)
#define T_2INT(x,y)        (((x) & (y) & P_INT) == INT)

#define O_INT              1
#define FAC_INT            2

#define TAG_INT(x)         (((x) * FAC_INT) + INT)
#define GET_INT(x)         ((int)((x) - INT) / FAC_INT)  /* stt 7.2.96 */

/* integer constants *//* BM */

#define MININT		   0xc0000000
#define MAXINT		   0x3fffffff
#define ZERO_INT           TAG_INT(0)
#define T_ZERO(x)          ((x) == ZERO_INT)
#define ONE_INT            TAG_INT(1)
#define T_ONE(x)           ((x) == ONE_INT)

/* integer operations */

#define NEG_INT(x)         ((2 * INT) - (x))
#define ADD_INT(x,y)       ((x) + ((y) ^ P_INT))
#define SUB_INT(x,y)       ((x) - ((y) ^ P_INT))
/* caution: some systems shift right (>>) logical! therefor: */
#if (((-1) >> 1) == -1)
/* arithmetic shift right! */
#define VAL_INT(x)         ((x) >> O_INT)
#define MUL_INT(x,y)       ((((x) ^ INT) * ((y) >> O_INT)) + INT)
#define DIV_INT(x,y)       (((double) VAL_INT(x)) / ((double) VAL_INT(y)))
#define MOD_INT(x,y)       TAG_INT(((x) >> O_INT) % ((y) >> O_INT))
#else
#define Attention_Logic_Shift_Right 1
#define Attention_Logic_Shift_Right 2
#define VAL_INT(x)         ((x) / FAC_INT)
#define MUL_INT(x,y)       (((x) ^ INT) * (((y) ^ INT) / FAC_INT) + INT)
/* #define DIV_INT(x,y)       TAG_INT(((x) / FAC_INT) / ((y) / FAC_INT)) */
#define DIV_INT(x,y)       (((double) VAL_INT(x))  /  ((double) VAL_INT(y)))
#define MOD_INT(x,y)       TAG_INT(((x) / FAC_INT) % ((y) / FAC_INT))
#endif
#define EQ_INT(x,y)        ((x) == (y)) ? SA_TRUE : SA_FALSE
#define NE_INT(x,y)        ((x) != (y)) ? SA_TRUE : SA_FALSE
#define LT_INT(x,y)        ((x) <  (y)) ? SA_TRUE : SA_FALSE
#define LE_INT(x,y)        ((x) <= (y)) ? SA_TRUE : SA_FALSE
#define GT_INT(x,y)        ((x) >  (y)) ? SA_TRUE : SA_FALSE
#define GE_INT(x,y)        ((x) >= (y)) ? SA_TRUE : SA_FALSE
#define MAX_INT(x,y)       ((x) >  (y)) ? (x) : (y)
#define MIN_INT(x,y)       ((x) <  (y)) ? (x) : (y)
/* bitweise verknuepfungen auf getagten integers (don't ask me why, ask wb) */
#define AND_INT(x,y)       ((x) & (y))
#define OR_INT(x,y)        ((x) | (y))
#define XOR_INT(x,y)       (((x) ^ (y)) | INT)
#define NAND_INT(x,y)      (((x) & (y)) ^ ~INT)
#define NOR_INT(x,y)       (((x) | (y)) ^ ~INT)
#define NOT_INT(x)         ((x) ^ ~INT)

/*************************************/
/* constructors, tag: 10             */
/*************************************/

#define CON                0x00000002
#define P_CON              0x00000003
#define T_CON(x)           (((x) & P_CON) == CON)

#define F_ARITY            0xffff0000     /* Bit 16-31 */
#define O_ARITY            16             /* Offset 16 */

#define F_CONSTR_NAME      0x000000f0     /* Bits 7-4  */
#define O_CONSTR_NAME      4              /* Offset 4  */

#define F_CHAIN_FLAG       0x00000008     /* Bit 3     */
#define O_CHAIN_FLAG       3              /* Offset 3  */

#define F_FIXVAR_FLAG      0x00000004     /* Bit 2     */
#define O_FIXVAR_FLAG      2              /* Offset 2  */

/* constructor with arity set */

#define CON_1              SET_ARITY(CON,1)
#define CON_2              SET_ARITY(CON,2)
#define P_CON_1            (P_CON | F_ARITY)
#define P_CON_2            (P_CON | F_ARITY)

/* atom <=> not constructor */

#define T_ATOM(x)          (((x) & P_CON) != CON)

/* var / fix constructors */

#define _VAR_CONSTR        0
#define _FIX_CONSTR        1

#define VAR_CONSTR         ((_VAR_CONSTR << O_FIXVAR_FLAG) | CON)
#define FIX_CONSTR         ((_FIX_CONSTR << O_FIXVAR_FLAG) | CON)

#define P_VAR_CONSTR       (P_CON | F_FIXVAR_FLAG)
#define P_FIX_CONSTR       (P_CON | F_FIXVAR_FLAG)

#define T_VAR_CONSTR(x)    (((x) & P_VAR_CONSTR) == VAR_CONSTR)
#define T_FIX_CONSTR(x)    (((x) & P_FIX_CONSTR) == FIX_CONSTR)

/* unchained / chained constructors */

#define _UNCHAINED_CONSTR  0
#define _CHAINED_CONSTR    1

#define UNCHAINED_CONSTR   ((_UNCHAINED_CONSTR << O_CHAIN_FLAG) | VAR_CONSTR)
#define CHAINED_CONSTR     ((_CHAINED_CONSTR << O_CHAIN_FLAG) | VAR_CONSTR)

#define P_UNCHAINED_CONSTR (P_VAR_CONSTR | F_CHAIN_FLAG)
#define P_CHAINED_CONSTR   (P_VAR_CONSTR | F_CHAIN_FLAG)

#define T_UNCHAINED_CONSTR(x) (((x) & P_UNCHAINED_CONSTR) == UNCHAINED_CONSTR)
#define T_CHAINED_CONSTR(x)   (((x) & P_CHAINED_CONSTR)   == CHAINED_CONSTR)

#define GROUP1_FIXCON      ((_UNCHAINED_CONSTR << O_CHAIN_FLAG) | FIX_CONSTR)
#define GROUP2_FIXCON      ((_CHAINED_CONSTR << O_CHAIN_FLAG) | FIX_CONSTR)

/* Masken fuer Konstruktoren */

#define P_FIXCOMPL         (F_CONSTR_NAME | P_FIX_CONSTR)
#define P_VARCOMPL         (F_CONSTR_NAME | P_VAR_CONSTR)

#define P_CFIXCOMPL        (P_FIXCOMPL | P_CHAINED_CONSTR)
#define P_CVARCOMPL        (P_VARCOMPL | P_CHAINED_CONSTR)

#define VAR_CCONSTR        (VAR_CONSTR | CHAINED_CONSTR)
#define FIX_CCONSTR        (FIX_CONSTR | CHAINED_CONSTR)

/* generator makros for unchained fix/var constructors */

#define FIXCONSTR(nr,ar)   (FIX_CONSTR | ((nr) << O_CONSTR_NAME))
#define VARCON(nr)         (VAR_CONSTR | ((nr) << O_CONSTR_NAME))

/* generator makros for chained fix/var constructors */

#define CFIXCONSTR(nr,ar)  (FIX_CCONSTR | ((nr) << O_CONSTR_NAME))
#define CVARCON(nr)        (VAR_CCONSTR | ((nr) << O_CONSTR_NAME))

/* other makros */

#define DEC(x)             (x - (1 << O_ARITY))
#define INC(x)             (x + (1 << O_ARITY))
#define SET_ARITY(x,n)     (((x) & ~F_ARITY) | ((n) << O_ARITY))
#define ARITY(x)           ((x) >> O_ARITY)
#define ARITY_GT_1(x)      ((x) & 0xFFFE000)

/*************************************/
/* constants, tag: 0100              */
/*************************************/

#define CONSTANT          0x00000004
#define P_CONSTANT        0x0000000f
#define T_CONSTANT(x)     (((x) & P_CONSTANT) == CONSTANT)

#define F_CNST_GROUP      0xff000000
#define O_CNST_GROUP      24

#define F_CNST_NAME       0x00ff0000
#define O_CNST_NAME       16

/* special constants */

#define F_BOOL_FLAG        0x00000010

#define SA_FALSE           CONSTANT
#define SA_TRUE            (SA_FALSE | F_BOOL_FLAG)
#define NUM                (0x00000020 | CONSTANT)
#define DOLLAR             (0x00000030 | CONSTANT)
#if WITHTILDE
#define DOLLAR_TILDE       (0x00000040 | CONSTANT)
#define TILDE              (0x00000060 | CONSTANT)
#endif /* WITHTILDE */
#define NORMAL_CONST       (0x00000070 | CONSTANT)

#define P_BOOLEAN          (0x00000020 | P_CONSTANT)
#define P_TRUE_FALSE       (0x00000030 | P_CONSTANT)
#define P_NUM              (0x00000070 | P_CONSTANT)
#define P_DOLLAR           (0x00000070 | P_CONSTANT)
#if WITHTILDE
#define P_DOLLAR_TILDE     (0x00000070 | P_CONSTANT)
#define P_TILDE            (0x00000070 | P_CONSTANT)
#endif /* WITHTILDE */
#define P_NORMAL_CONST     (0x00000070 | P_CONSTANT)

#define T_BOOLEAN(x)       (((x) & P_BOOLEAN) == SA_FALSE)
#define T_2BOOLEAN(x,y)    (((x) & (y) & P_BOOLEAN) == SA_FALSE)
#define T_SA_FALSE(x)      (((x) & P_TRUE_FALSE) == SA_FALSE)
#define T_SA_TRUE(x)       (((x) & P_TRUE_FALSE) == SA_TRUE)
#define T_NUM(x)           (((x) & P_NUM) == NUM)
#define T_DOLLAR(x)        (((x) & P_DOLLAR) == DOLLAR)
#if WITHTILDE
#define T_DOLLAR_TILDE(x)  (((x) & P_DOLLAR_TILDE) == DOLLAR_TILDE)
#define T_TILDE(x)         (((x) & P_TILDE) == TILDE)
#endif /* WITHTILDE */
#define T_NORMAL_CONST(x)  (((x) & P_NORMAL_CONST) == NORMAL_CONST)

#define IS_FALSE(x)        (((x) ^ SA_FALSE) == 0)
#define IS_TRUE(x)         (((x) ^ SA_TRUE) == 0)

/* operations on booleans */

#define NOT_BOOL(x)        ((x) ^ F_BOOL_FLAG)
#define AND_BOOL(x,y)      ((x) & (y))
#define ANDNT_BOOL(x,y)    ((x) & ~(y))
#define OR_BOOL(x,y)       ((x) | (y))
#define XOR_BOOL(x,y)      (((x) ^ (y)) | SA_FALSE)

#define EQ_BOOL(x,y)       ((x) == (y)) ? SA_TRUE : SA_FALSE
#define NE_BOOL(x,y)       ((x) != (y)) ? SA_TRUE : SA_FALSE
#define LT_BOOL(x,y)       ((x) <  (y)) ? SA_TRUE : SA_FALSE
#define LE_BOOL(x,y)       ((x) <= (y)) ? SA_TRUE : SA_FALSE
#define GT_BOOL(x,y)       ((x) >  (y)) ? SA_TRUE : SA_FALSE
#define GE_BOOL(x,y)       ((x) >= (y)) ? SA_TRUE : SA_FALSE
#define MAX_BOOL(x,y)      ((x) >  (y)) ? (x) : (y)
#define MIN_BOOL(x,y)      ((x) <  (y)) ? (x) : (y)

/* SA_C generates a normal constant with name x */

#define SA_C(x)            (NORMAL_CONST | ((x) << O_VALUE))

#define GROUP(x)           (((x) & F_CNST_GROUP) >> O_CNST_GROUP)
#define P_CONSTCOMPL       (F_CNST_GROUP | F_CNST_NAME | P_NUM | P_CONSTANT)

/*************************************/
/* functions, tag 1000               */
/*************************************/
/* Da im F_TYPE-Feld kein Platz mehr fuer eine zusaetzliche Codierung war,
 * werden die Interaktionen als Funktionen (1000) angesehen. Um primitive
 * Funktionen und Interaktionen unterscheiden zu koennen wird bei Interaktionen
 * das bisher ungenutzte Bit 31 gesetzt. Die Test- und Lesemakros wurden
 * entsprechend erweitert.                                stt 11.09.95
 */

#define IAFUNC            0x80000008
#define P_IAFUNC          0x8000000f
#define T_IAFUNC(x)       (((x) & P_IAFUNC) == IAFUNC)

#define FUNC              0x00000008
#define P_FUNC            0x0000000f
#define T_FUNC(x)         (((x) & P_IAFUNC) == FUNC) 

#define F_FUNC_NAME        0x00FF0000     /* Bit 16-23 */
#define O_FUNC_NAME        16             /* Offset 16 */

#define F_FUNC_ARITY       0x000000F0     /* Bits 7-4  */
#define O_FUNC_ARITY       4              /* Offset 4  */

#define FUNC_ARITY(x)      (((x) & F_FUNC_ARITY) >> O_FUNC_ARITY)
#define FUNC_INDEX(x)      (((x) & F_FUNC_NAME) >> O_FUNC_NAME)
#define FNAME(x)           (((x) & F_FUNC_NAME) >> O_FUNC_NAME)

/* SA_F generates a primitive function with name id and arity ar */

#define SA_F(id,ar)     (FUNC | ((ar) << O_FUNC_ARITY) | ((id) << O_FUNC_NAME))
#define P_FUNCCOMPL     (F_FUNC_NAME | F_FUNC_ARITY | P_FUNC)

/* SA_I generates an interaction with name id and arity ar      stt */
#define SA_I(id,ar)   (IAFUNC | ((ar) << O_FUNC_ARITY) | ((id) << O_FUNC_NAME))
#define P_IFUNCCOMPL  (F_FUNC_NAME | F_FUNC_ARITY | P_IAFUNC)

/*************************************/
/* strings (multi-atoms), tag: 1100  */
/*************************************/

#define STR               0x0000000c
#define P_STR             0x0000000f
#define T_STR(x)          (((x) & P_STR) == STR)

#define MA                 STR
#define P_MA               P_STR
#define T_MA(x)            (((x) & P_MA) == MA)

#define F_DIGLET_FLAG      0x00000040     /* Bit 6     */
#define O_DIGLET_FLAG      6              /* Offset 6  */

#define F_MARK_FLAG        0x00000020     /* Bit 5     */
#define O_MARK_FLAG        5              /* Offset 5  */

#define P_MARK             F_MARK_FLAG

#define F_VARSTR_FLAG      0x00000010     /* Bit 4     */
#define O_VARSTR_FLAG      4              /* Offset 4  */

#define SWITCH(x)          ((x) ^ F_MARK_FLAG)
#define TOGGLE(x)          ((x) ^ F_MARK_FLAG)

/* single-atom */

#define T_SA(x)            (T_ATOM(x) && !T_MA(x))

/* letter-strings / digit-strings */

#define _LETTER            0
#define _DIGIT             1

#define LETTER             ((_LETTER << O_DIGLET_FLAG) | MA)
#define DIGIT              ((_DIGIT  << O_DIGLET_FLAG) | MA)

#define P_LETTER           (P_MA | F_DIGLET_FLAG)
#define P_DIGIT            (P_MA | F_DIGLET_FLAG)

#define T_LETTER(x)        (((x) & P_LETTER) == LETTER)
#define T_DIGIT(x)         (((x) & P_DIGIT) == DIGIT)

/* variable-Characters / string-characters */

#define _VAR_CHAR          0
#define _STR_CHAR          1

#define VAR_CHAR           ((_VAR_CHAR << O_VARSTR_FLAG) | LETTER)
#define STR_CHAR           ((_STR_CHAR << O_VARSTR_FLAG) | LETTER)

#define P_VAR_CHAR         (P_LETTER | F_VARSTR_FLAG)
#define P_STR_CHAR         (P_LETTER | F_VARSTR_FLAG)

#define T_VAR_CHAR(x)      (((x) & P_VAR_CHAR) == VAR_CHAR)
#define T_STR_CHAR(x)      (((x) & P_STR_CHAR) == STR_CHAR)

/* last / notlast letter */

#define _LET0              0                     /* last letter */
#define _LET1              1

#define LET0               ((_LET0 << O_MARK_FLAG) | VAR_CHAR)
#define LET1               ((_LET1 << O_MARK_FLAG) | VAR_CHAR)

#define P_LET0             (P_VAR_CHAR | F_MARK_FLAG)
#define P_LET1             (P_VAR_CHAR | F_MARK_FLAG)

#define T_LET0(x)          (((x) & P_LET0) == LET0)
#define T_LET1(x)          (((x) & P_LET1) == LET1)

/* last / notlast digit */

#define _DIG0              0
#define _DIG1              1

#define DIG0               ((_DIG0 << O_MARK_FLAG) | DIGIT)
#define DIG1               ((_DIG1 << O_MARK_FLAG) | DIGIT)

#define P_DIG0             (P_DIGIT | F_MARK_FLAG)
#define P_DIG1             (P_DIGIT | F_MARK_FLAG)

#define T_DIG0(x)          (((x) & P_DIG0) == DIG0)
#define T_DIG1(x)          (((x) & P_DIG1) == DIG1)

/* last /notlast char */

#define _STR0              0
#define _STR1              1

#define STR0               ((_STR0 << O_MARK_FLAG) | MA)
#define STR1               ((_STR1 << O_MARK_FLAG) | MA)

#define P_STR0             (P_MA | F_MARK_FLAG)
#define P_STR1             (P_MA | F_MARK_FLAG)

#define T_STR0(x)          (((x) & P_STR0) == STR0)
#define T_STR1(x)          (((x) & P_STR1) == STR1)

/* stt 25.10.95:  make string element */
#define TAG_STR_CHAR(x)    (((x) << O_VALUE) | STR_CHAR)

/* decimal point */

#define DECPT              (DIGIT | '.' << O_VALUE)
#define P_DECPT            (P_DIGIT | F_VALUE)
#define T_DECPT(x)         (((x) & P_DECPT) == DECPT)

#if (D_DIST || STORE)

/* special constants for the message routines */
/* ach 03/03/93 */
#define ST_END_MARKER        0x42424242
#define ST_DUMMY             2
#define IS_DUMMY(elem)       (((elem) & 0xf) == ST_DUMMY)        
#define DIST_FLAG            0x44495354
#define T_STAT_POINTER(elem) (((elem) & 0xf) == 0xa)
#define T_EXT_POINTER(elem)  (T_POINTER(elem) || T_STAT_POINTER(elem) || T_DESC_INDEX(elem) || T_NULL_POINTER(elem))
#define T_DESC_INDEX(elem)   (((elem) & 0xf) == 0xe)
#define T_NULL_POINTER(elem) (((elem) & 0xf) == 0x1)

#endif

/*********************************************************************/
/*                                                                   */
/*  F I X - C O N S T R U C T O R S                                  */
/*                                                                   */
/*********************************************************************/

#define _PRELIST           0
#define P_PRELIST          P_CFIXCOMPL
#define PRELIST            FIXCONSTR(_PRELIST,2)
#define T_PRELIST(x)       (((x) & P_PRELIST) == PRELIST)

#define _PROTECT           1
#define P_PROTECT          P_CFIXCOMPL
#define PROTECT            FIXCONSTR(_PROTECT,1)
#define T_PROTECT(x)       (((x) & P_PROTECT) == PROTECT)

#define _PLUS              2
#define P_PLUS             P_CFIXCOMPL
#define PLUS               FIXCONSTR(_PLUS,1)
#define T_PLUS(x)          (((x) & P_PLUS) == PLUS)

#define _MINUS             3
#define P_MINUS            P_CFIXCOMPL
#define MINUS              FIXCONSTR(_MINUS,1)
#define T_MINUS(x)         (((x) & P_MINUS) == MINUS)

#define _SET               4
#define P_SET              P_CFIXCOMPL
#define SET                FIXCONSTR(_SET,1)
#define T_SET(x)           (((x) & P_SET) == SET)

#define _CONS              4
#define P_CONS             P_CFIXCOMPL
#define CONS               FIXCONSTR(_CONS,2)
#define T_CONS(x)          (((x) & P_CONS) == CONS)

#define _REC               5
#define P_REC              P_CFIXCOMPL
#define REC                FIXCONSTR(_REC,2)
#define T_REC(x)           (((x) & P_REC) == REC)

#define _COND              6
#define P_COND             P_CFIXCOMPL
#define COND               FIXCONSTR(_COND,2)
#define T_COND(x)          (((x) & P_COND) == COND)

#define _SCALAR            7
#define P_SCALAR           P_CFIXCOMPL
#define SCALAR             FIXCONSTR(_SCALAR,2)
#define T_SCALAR(x)        (((x) & P_SCALAR) == SCALAR)

#define _VECTOR            8
#define P_VECTOR           P_CFIXCOMPL
#define VECTOR             FIXCONSTR(_VECTOR,2)
#define T_VECTOR(x)        (((x) & P_VECTOR) == VECTOR)

#define _TVECTOR           9
#define P_TVECTOR          P_CFIXCOMPL
#define TVECTOR            FIXCONSTR(_TVECTOR,2)
#define T_TVECTOR(x)       (((x) & P_TVECTOR) == TVECTOR)

#define _EXP               11
#define P_EXP              P_CFIXCOMPL
#define EXP                FIXCONSTR(_EXP,2)
#define T_EXP(x)           (((x) & P_EXP) == EXP)

#define _F                 12
#define P_F                P_CFIXCOMPL
#define F                  FIXCONSTR(_F,2)
#define T_F(x)             (((x) & P_F) == F)

/* H */

#define _PM_OTHERWISE      13
#define P_PM_OTHERWISE     P_CFIXCOMPL
#define PM_OTHERWISE       FIXCONSTR(_PM_OTHERWISE,1)
#define T_PM_OTHERWISE(x)  (((x) & P_PM_OTHERWISE) == PM_OTHERWISE)

#define _PM_WHEN           14
#define P_PM_WHEN          P_CFIXCOMPL
#define PM_WHEN            FIXCONSTR(_PM_WHEN,3)
#define T_PM_WHEN(x)       (((x) & P_PM_WHEN) == PM_WHEN)

#define _PM_MATCH          15
#define P_PM_MATCH         P_CFIXCOMPL
#define PM_MATCH           FIXCONSTR(_PM_MATCH,3)
#define T_PM_MATCH(x)      (((x) & P_PM_MATCH) == PM_MATCH)

/*********************************************************************/
/*  F I X - C O N S T R U C T O R S        ++ chained flag set       */
/*********************************************************************/

#define _STRING            0  /* STRING ist ein var-Konstruktor */
#define P_STRING           P_CVARCOMPL
#define STRING             CFIXCONSTR(_STRING,0)
#define T_STRING(X)        (((X) & P_STRING) == STRING)

#define _PM_IN             1
#define P_PM_IN            P_CFIXCOMPL
#define PM_IN              CFIXCONSTR(_PM_IN,2)
#define T_PM_IN(X)         (((X) & P_PM_IN) == PM_IN)

#define _K_COMM            6
#define P_K_COMM           P_CFIXCOMPL
#define K_COMM             CFIXCONSTR(_K_COMM,2)
#define T_K_COMM(X)        (((X) & P_K_COMM) == K_COMM)

#define _UNPROTECT         15             /* ak *//* UNPROTECTS verhindern den Aufbau von PROTECTS */
#define P_UNPROTECT        P_CFIXCOMPL
#define UNPROTECT          CFIXCONSTR(_UNPROTECT,1)
#define T_UNPROTECT(x)     (((x) & P_UNPROTECT) == UNPROTECT)
 
/*********************************************************************/
/*                                                                   */
/*  V A R - C O N S T R U C T O R S                                  */
/*                                                                   */
/*********************************************************************/

#define _AP                0
#define P_AP               P_CVARCOMPL
#define AP                 VARCON(_AP)
#define T_AP(x)            (((x) & P_AP) == AP)

#define P_AP_1             (P_CVARCOMPL | F_ARITY)
#define AP_1               SET_ARITY(AP, 1)
#define T_AP_1(x)          (((x) & P_AP_1) == AP_1)

#define P_AP_2             (P_CVARCOMPL | F_ARITY)
#define AP_2               SET_ARITY(AP, 2)
#define T_AP_2(x)          (((x) & P_AP_2) == AP_2)

#define _SNAP              1
#define P_SNAP             P_CVARCOMPL
#define SNAP               VARCON(_SNAP)
#define T_SNAP(x)          (((x) & P_SNAP) == SNAP)

#define P_SNAP_1           (P_CVARCOMPL | F_ARITY)
#define SNAP_1               SET_ARITY(SNAP, 1)
#define T_SNAP_1(x)          (((x) & P_SNAP_1) == SNAP_1)

#define _AP_TIC            2
#define P_AP_TIC           P_CVARCOMPL
#define AP_TIC             VARCON(_AP_TIC)
#define T_AP_TIC(x)        (((x) & P_AP) == AP)

#define _SUB               3
#define P_SUB              P_CVARCOMPL
#define SUB                VARCON(_SUB)
#define T_SUB(x)           (((x) & P_SUB) == SUB)

#define SUB_1              SET_ARITY(SUB, 1)
#define SUB_2              SET_ARITY(SUB, 2)
#define P_SUB_1            (P_CVARCOMPL | F_ARITY)
#define P_SUB_2            (P_CVARCOMPL | F_ARITY)

#define _SNSUB             4
#define P_SNSUB            P_CVARCOMPL
#define SNSUB              VARCON(_SNSUB)
#define T_SNSUB(x)        (((x) & P_SNSUB) == SNSUB)

#define _POST_SNAP         5
#define P_POST_SNAP        P_CVARCOMPL
#define POST_SNAP          VARCON(_POST_SNAP)
#define T_POST_SNAP(x)     (((x) & P_POST_SNAP) == POST_SNAP)

#define _LIST              6
#define P_LIST             P_VARCOMPL
#define LIST               VARCON(_LIST)
#define T_LIST(x)          (((x) & P_LIST) == LIST)

#define _MATRIX            7
#define P_MATRIX           P_CVARCOMPL
#define MATRIX             VARCON(_MATRIX)
#define T_MATRIX(x)        (((x) & P_MATRIX) == MATRIX)

#define _PM_SWITCH         8
#define P_PM_SWITCH        P_VARCOMPL
#define PM_SWITCH          VARCON(_PM_SWITCH)
#define T_PM_SWITCH(x)     (((x) & P_PM_SWITCH) == PM_SWITCH)

#define _LREC              9
#define P_LREC             P_CVARCOMPL
#define LREC               VARCON(_LREC)
#define T_LREC(x)          (((x) & P_LREC) == LREC)

#define _PM_AS             10
#define P_PM_AS            P_CVARCOMPL
#define PM_AS              VARCON(_PM_AS)
#define T_PM_AS(x)         (((x) & P_PM_AS) == PM_AS)

#define _PM_LIST           11
#define P_PM_LIST          P_CVARCOMPL
#define PM_LIST            VARCON(_PM_LIST)
#define T_PM_LIST(x)       (((x) & P_PM_LIST) == PM_LIST)

#define _PM_REC            12
#define P_PM_REC           P_CVARCOMPL
#define PM_REC             VARCON(_PM_REC)
#define T_PM_REC(x)        (((x) & P_PM_REC) == PM_REC)

#define _PM_NCASE          13
#define P_PM_NCASE         P_CVARCOMPL
#define PM_NCASE           VARCON(_PM_NCASE)
#define T_PM_NCASE(x)      (((x) & P_PM_NCASE) == PM_NCASE)


#if !WITHTILDE

#define _PM_FCASE          14
#define P_PM_FCASE         P_CVARCOMPL
#define PM_FCASE           VARCON(_PM_FCASE)
#define T_PM_FCASE(x)      (((x) & P_PM_FCASE) == PM_FCASE)

#define _PM_ECASE          15
#define P_PM_ECASE         P_CVARCOMPL
#define PM_ECASE           VARCON(_PM_ECASE)
#define T_PM_ECASE(x)      (((x) & P_PM_ECASE) == PM_ECASE)

#define T_PM_CASE(x)       (T_PM_SWITCH(x) || T_PM_NCASE(x) || \
                            T_PM_FCASE(x) || T_PM_ECASE(x) || T_PM_REC(x))
#else

/* GUARD and NOMAT are used by the tilde postprocessor */

#define  _PM_GUARD            14
#define  P_PM_GUARD           P_CVARCOMPL
#define  PM_GUARD             CVARCON( _PM_GUARD  )
#define  T_PM_GUARD(x)        ( ( (x) & P_PM_GUARD ) == PM_GUARD )

#define  _PM_NOMAT            15
#define  P_PM_NOMAT           P_CVARCOMPL
#define  PM_NOMAT             CVARCON( _PM_NOMAT  )
#define  T_PM_NOMAT(x)        ( ( (x) & P_PM_NOMAT ) == PM_NOMAT )

#define T_PM_CASE(x)          (T_PM_SWITCH(x) || T_PM_NCASE(x) || T_PM_REC(x))

#endif /* !WITHTILDE */

/* Neuer Konstruktor fuer LETPAR- und Verteilungsgeschichten !  RS 25.1.1993 */

#if D_DIST

#define _DAP               0 
#define P_DAP              0x0000000aL          
#define DAP                VARCON(_DAP)
#define T_DAP(x)           (((x) & 0x000000faL) == P_DAP)

#define T_DAP_1(x)         ((x & 0xffff00ff) == 0x0001000a)

/* for speculative PMs */

#endif

#define T_SPEC_CASE(x)     (((x&0x0000000aL)==0x0000000aL) && T_PM_CASE(x))

#define _SPECLIST1              6
#define P_SPECLIST1             0x0000006aL
#define SPECLIST1               VARCON(_SPECLIST1)
#define T_SPECLIST1(x)          (((x) & 0x000000ffL) == P_SPECLIST1)

#define _SPECLIST2              7
#define P_SPECLIST2             0x0000007eL
#define SPECLIST2               VARCON(_SPECLIST2)
#define T_SPECLIST2(x)          (((x) & 0x000000ffL) == P_SPECLIST2)



/*********************************************************************/
/*  V A R - C O N S T R U C T O R S     ++ chained flag set          */
/*********************************************************************/

/*
#define _SYSAP             0
#define P_SYSAP            P_CVARCOMPL
#define SYSAP              CVARCON(SYSAP)
#define T_SYSAP(x)         (((x) & P_SYSAP) == SYSAP)
*/

#define F_SYSAP            0x00000008
#define P_SYSAP            0x00000008
#define V_SYSAP            0x00000008
#define SET_AP(x)          (x |= F_SYSAP )
#define CLEAR_AP(x)        (x &= ~F_SYSAP )
#define T_SYSAP(x)         ((x & P_SYSAP) == V_SYSAP)

#define _NEXT_VAR          1
#define P_NEXT_VAR         P_CVARCOMPL
#define NEXT_VAR           CVARCON(_NEXT_VAR)
#define T_NEXT_VAR(x)      (((x) & P_NEXT_VAR) == NEXT_VAR)

/* frames (structs) and slots (entries) , cr 08.11.95 */

#define _STRUCT            10
#define P_STRUCT           P_CVARCOMPL
#define STRUCT             CVARCON(_STRUCT)
#define T_STRUCT(X)        (((X) & P_STRUCT) == STRUCT)

#define _ENTRY             11
#define P_ENTRY            P_CVARCOMPL
#define ENTRY              CVARCON(_ENTRY)
#define T_ENTRY(X)         (((X) & P_ENTRY) == ENTRY)

#define _ZF_UH             12
#define P_ZF_UH            P_CVARCOMPL
#define ZF_UH              CVARCON(_ZF_UH)
#define T_ZF_UH(X)         (((X) & P_ZF_UH) == ZF_UH)

#define _ZFIN_UH           13
#define P_ZFIN_UH          P_CVARCOMPL
#define ZFIN_UH            CVARCON(_ZFIN_UH)
#define T_ZFIN_UH(X)       (((X) & P_ZFIN_UH) == ZFIN_UH)

/* Interner ZF Konstruktor */
#define _ZFINT_UH          14
#define P_ZFINT_UH         P_CVARCOMPL
#define ZFINT_UH           CVARCON(_ZFINT_UH)
#define T_ZFINT_UH(X)      (((X) & P_ZFINT_UH) == ZFINT_UH)


#ifdef NotUsed
/*********************************************************************/
/*  Value - Belegungen fuer Single Atoms   !wozu das??? (dg)         */
/*********************************************************************/

#define _STF1              1
#define _STF2              2
#define _STF3              3
#define _STF4              4
#define _STF5              5
#define _STF6              6
#define _STF7              7
#define _STF8              8

#define DYFUNC             10
#define MONFUNC            30
#define CFUNC              40
#define IPFUNC             60
#define CPFUNC             80
#define QUERY              95
#define SPEC_FUNC          120
#define COOR_FUNC          150
#endif

/*********************************************************************/
/*  Dyadische Funktionen                                             */
/*********************************************************************/

#define P_D_PLUS           P_FUNCCOMPL
#define _D_PLUS            1
#define D_PLUS             SA_F(_D_PLUS ,2)
#define T_D_PLUS(x)        (((x) & P_D_PLUS) == D_PLUS)

#define P_D_MINUS          P_FUNCCOMPL
#define _D_MINUS           2
#define D_MINUS            SA_F(_D_MINUS ,2)
#define T_D_MINUS(x)       (((x) & P_D_MINUS) == D_MINUS)

#define P_D_MULT           P_FUNCCOMPL
#define _D_MULT            3
#define D_MULT             SA_F(_D_MULT ,2)
#define T_D_MULT(x)        (((x) & P_D_MULT) == D_MULT)

#define P_D_DIV            P_FUNCCOMPL
#define _D_DIV             4
#define D_DIV              SA_F(_D_DIV ,2)
#define T_D_DIV(x)         (((x) & P_D_DIV) == D_DIV)

#define P_D_MOD            P_FUNCCOMPL
#define _D_MOD             5
#define D_MOD              SA_F(_D_MOD ,2)
#define T_D_MOD(x)         (((x) & P_D_MOD) == D_MOD)

#define P_D_OR             P_FUNCCOMPL
#define _D_OR              6
#define D_OR               SA_F(_D_OR ,2)
#define T_D_OR(x)          (((x) & P_D_OR) == D_OR)

#define P_D_AND            P_FUNCCOMPL
#define _D_AND             7
#define D_AND              SA_F(_D_AND ,2)
#define T_D_AND(x)         (((x) & P_D_AND) == D_AND)

#define P_D_XOR            P_FUNCCOMPL
#define _D_XOR             8
#define D_XOR              SA_F(_D_XOR ,2)
#define T_D_XOR(x)         (((x) & P_D_XOR) == D_XOR)

#define P_D_EQ             P_FUNCCOMPL
#define _D_EQ              9
#define D_EQ               SA_F(_D_EQ ,2)
#define T_D_EQ(x)          (((x) & P_D_EQ) == D_EQ)

#define P_D_NEQ            P_FUNCCOMPL
#define _D_NEQ             10
#define D_NEQ              SA_F(_D_NEQ ,2)
#define T_D_NEQ(x)         (((x) & P_D_NEQ) == D_NEQ)

#define P_D_LESS           P_FUNCCOMPL
#define _D_LESS            11
#define D_LESS             SA_F(_D_LESS ,2)
#define T_D_LESS(x)        (((x) & P_D_LESS) == D_LESS)

#define P_D_LEQ            P_FUNCCOMPL
#define _D_LEQ             12
#define D_LEQ              SA_F(_D_LEQ ,2)
#define T_D_LEQ(x)         (((x) & P_D_LEQ) == D_LEQ)

#define P_D_GREAT          P_FUNCCOMPL
#define _D_GREAT           13
#define D_GREAT            SA_F(_D_GREAT ,2)
#define T_D_GREAT(x)       (((x) & P_D_GREAT) == D_GREAT)

#define P_D_GEQ            P_FUNCCOMPL
#define _D_GEQ             14
#define D_GEQ              SA_F(_D_GEQ ,2)
#define T_D_GEQ(x)         (((x) & P_D_GEQ) == D_GEQ)

#define P_D_MAX            P_FUNCCOMPL
#define _D_MAX             15
#define D_MAX              SA_F(_D_MAX ,2)
#define T_D_MAX(x)         (((x) & P_D_MAX) == D_MAX)

#define P_D_MIN            P_FUNCCOMPL
#define _D_MIN             16
#define D_MIN              SA_F(_D_MIN ,2)
#define T_D_MIN(x)         (((x) & P_D_MIN) == D_MIN)

#define P_D_QUOT           P_FUNCCOMPL
#define _D_QUOT            79
#define D_QUOT             SA_F(_D_QUOT ,2)
#define T_D_QUOT(x)         (((x) & P_D_QUOT) == D_QUOT)

/*********************************************************************/
/*  Monadische Funktionen                                            */
/*********************************************************************/

#define P_M_NOT            P_FUNCCOMPL
#define _M_NOT             17
#define M_NOT              SA_F(_M_NOT ,1)
#define T_M_NOT(x)         (((x) & P_M_NOT) == M_NOT)

#define P_M_ABS            P_FUNCCOMPL
#define _M_ABS             18
#define M_ABS              SA_F(_M_ABS ,1)
#define T_M_ABS(x)         (((x) & P_M_ABS) == M_ABS)

#define P_M_NEG            P_FUNCCOMPL
#define _M_NEG             53
#define M_NEG              SA_F(_M_NEG ,1)
#define T_M_NEG(x)         (((x) & P_M_NEG) == M_NEG)

#define P_M_TRUNCATE       P_FUNCCOMPL
#define _M_TRUNCATE        19
#define M_TRUNCATE         SA_F(_M_TRUNCATE ,1)
#define T_M_TRUNCATE(x)    (((x) & P_M_TRUNCATE) == M_TRUNCATE)

#define P_M_FLOOR          P_FUNCCOMPL
#define _M_FLOOR           20
#define M_FLOOR            SA_F(_M_FLOOR ,1)
#define T_M_FLOOR(x)       (((x) & P_M_FLOOR) == M_FLOOR)

#define P_M_CEIL           P_FUNCCOMPL
#define _M_CEIL            21
#define M_CEIL             SA_F(_M_CEIL ,1)
#define T_M_CEIL(x)        (((x) & P_M_CEIL) == M_CEIL)

#define P_M_FRAC           P_FUNCCOMPL
#define _M_FRAC            22
#define M_FRAC             SA_F(_M_FRAC ,1)
#define T_M_FRAC(x)        (((x) & P_M_FRAC) == M_FRAC)

#define P_M_EMPTY          P_FUNCCOMPL
#define _M_EMPTY           23
#define M_EMPTY            SA_F(_M_EMPTY ,1)
#define T_M_EMPTY(x)       (((x) & P_M_EMPTY) == M_EMPTY)

#define P_M_SIN            P_FUNCCOMPL
#define _M_SIN             80
#define M_SIN              SA_F(_M_SIN ,1)
#define T_M_SIN(x)         (((x) & P_M_SIN) == M_SIN)

#define P_M_COS            P_FUNCCOMPL
#define _M_COS             81
#define M_COS              SA_F(_M_COS ,1)
#define T_M_COS(x)         (((x) & P_M_COS) == M_COS)

#define P_M_TAN            P_FUNCCOMPL
#define _M_TAN             82
#define M_TAN              SA_F(_M_TAN ,1)
#define T_M_TAN(x)         (((x) & P_M_TAN) == M_TAN)

#define P_M_LN             P_FUNCCOMPL
#define _M_LN              83
#define M_LN               SA_F(_M_LN ,1)
#define T_M_LN(x)          (((x) & P_M_LN) == M_LN)

#define P_M_EXP            P_FUNCCOMPL
#define _M_EXP             84
#define M_EXP              SA_F(_M_EXP ,1)
#define T_M_EXP(x)         (((x) & P_M_EXP) == M_EXP)

/*********************************************************************/
/*  Inneres Produkt                                                  */
/*********************************************************************/

#define P_IP               P_FUNCCOMPL
#define _IP                24
#define IP                 SA_F(_IP,2)
#define T_IP_(x)           (((x) & P_IP) == IP)

/*********************************************************************/
/*  Cartesisches Produkt                                             */
/*********************************************************************/

/*********************************************************************/
/* Queries                                                           */
/*********************************************************************/

#define P_CLASS            P_FUNCCOMPL
#define _CLASS             25
#define CLASS              SA_F(_CLASS ,1)
#define T_CLASS(x)         (((x) & P_CLASS) == CLASS)

#define P_TYPE             P_FUNCCOMPL
#define _TYPE              26
#define TYPE               SA_F(_TYPE ,1)
#define T_TYPE(x)          (((x) & P_TYPE) == TYPE)

#define P_DIMENSION        P_FUNCCOMPL
#define _DIMENSION         28
#define DIMENSION          SA_F(_DIMENSION ,2)
#define T_DIMENSION(x)     (((x) & P_DIMENSION) == DIMENSION)

#define P_VDIMENSION       P_FUNCCOMPL
#define _VDIMENSION        59
#define VDIMENSION         SA_F(_VDIMENSION ,1)
#define T_VDIMENSION(x)    (((x) & P_VDIMENSION) == VDIMENSION)

#define P_MDIMENSION       P_FUNCCOMPL
#define _MDIMENSION        60
#define MDIMENSION         SA_F(_MDIMENSION ,2)
#define T_MDIMENSION(x)    (((x) & P_MDIMENSION) == MDIMENSION)

#define P_LDIMENSION       P_FUNCCOMPL
#define _LDIMENSION        77
#define LDIMENSION         SA_F(_LDIMENSION ,1)
#define T_LDIMENSION(x)    (((x) & P_LDIMENSION) == LDIMENSION)

/*********************************************************************/
/*  Strukturierungsfunktionen                                        */
/*********************************************************************/

/*********************************************************************/
/* Group 1                                                           */
/*********************************************************************/

#define P_TRANSPOSE        P_FUNCCOMPL
#define _TRANSPOSE         29
#define TRANSPOSE          SA_F(_TRANSPOSE ,1)
#define T_TRANSPOSE(x)     (((x) & P_TRANSPOSE) == TRANSPOSE)

#define P_REVERSE          P_FUNCCOMPL
#define _REVERSE           75
#define REVERSE            SA_F(_REVERSE ,1)
#define T_REVERSE(x)       (((x) & P_REVERSE) == REVERSE)


/*********************************************************************/
/* Group 2                                                           */
/*********************************************************************/

#define P_CUT              P_FUNCCOMPL
#define _CUT               30
#define CUT                SA_F(_CUT ,3)
#define T_CUT(x)           (((x) & P_CUT) == CUT)

#define P_VCUT             P_FUNCCOMPL
#define _VCUT              54
#define VCUT               SA_F(_VCUT ,2)
#define T_VCUT(x)          (((x) & P_VCUT) == VCUT)

#define P_MCUT             P_FUNCCOMPL
#define _MCUT              85
#define MCUT               SA_F(_MCUT ,3)
#define T_MCUT(x)          (((x) & P_MCUT) == MCUT)

#define P_LTRANSFORM       P_FUNCCOMPL
#define _LTRANSFORM        76
#define LTRANSFORM         SA_F(_LTRANSFORM ,3)
#define T_LTRANSFORM(x)    (((x) & P_LTRANSFORM) == LTRANSFORM)

#define P_ROTATE           P_FUNCCOMPL
#define _ROTATE            32
#define ROTATE             SA_F(_ROTATE ,3)
#define T_ROTATE(x)        (((x) & P_ROTATE) == ROTATE)

#define P_VROTATE          P_FUNCCOMPL
#define _VROTATE           55
#define VROTATE            SA_F(_VROTATE ,2)
#define T_VROTATE(x)       (((x) & P_VROTATE) == VROTATE)

#define P_MROTATE          P_FUNCCOMPL
#define _MROTATE           87
#define MROTATE            SA_F(_MROTATE ,3)
#define T_MROTATE(x)       (((x) & P_MROTATE) == MROTATE)

#define P_SELECT           P_FUNCCOMPL
#define _SELECT            33
#define SELECT             SA_F(_SELECT ,3)
#define T_SELECT(x)        (((x) & P_SELECT) == SELECT)

#define P_VSELECT          P_FUNCCOMPL
#define _VSELECT           56
#define VSELECT            SA_F(_VSELECT ,2)
#define T_VSELECT(x)       (((x) & P_VSELECT) == VSELECT)

#define P_MSELECT          P_FUNCCOMPL
#define _MSELECT           86
#define MSELECT            SA_F(_MSELECT ,3)
#define T_MSELECT(x)       (((x) & P_MSELECT) == MSELECT)

#define P_SUBSTR           P_FUNCCOMPL
#define _SUBSTR            61
#define SUBSTR             SA_F(_SUBSTR ,3)
#define T_SUBSTR(x)        (((x) & P_SUBSTR) == SUBSTR)

#define P_LGEN_UH          P_FUNCCOMPL
#define _LGEN_UH           62
#define LGEN_UH            SA_F(_LGEN_UH,3)
#define T_LGEN_UH(x)       (((x) & P_LGEN_UH) == LGEN_UH)

#define P_LCUT             P_FUNCCOMPL
#define _LCUT              70
#define LCUT               SA_F(_LCUT ,2)
#define T_LCUT(x)          (((x) & P_LCUT) == LCUT)

#define P_TRANSFORM        P_FUNCCOMPL
#define _TRANSFORM         31
#define TRANSFORM          SA_F(_TRANSFORM ,1)
#define T_TRANSFORM(x)     (((x) & P_TRANSFORM) == TRANSFORM)

#define P_LROTATE          P_FUNCCOMPL
#define _LROTATE           72
#define LROTATE            SA_F(_LROTATE ,2)
#define T_LROTATE(x)       (((x) & P_LROTATE) == LROTATE)

#define P_LSELECT          P_FUNCCOMPL
#define _LSELECT           71
#define LSELECT            SA_F(_LSELECT ,2)
#define T_LSELECT(x)       (((x) & P_LSELECT) == LSELECT)

#define P_D_CONS           P_FUNCCOMPL
#define _D_CONS            78
#define D_CONS             SA_F(_D_CONS ,2)
#define T_D_CONS(x)        (((x) & P_D_CONS) == D_CONS)

/*********************************************************************/
/* Group 3                                                           */
/*********************************************************************/

#define P_LREPLACE         P_FUNCCOMPL
#define _LREPLACE          74
#define LREPLACE           SA_F(_LREPLACE ,3)
#define T_LREPLACE(x)      (((x) & P_LREPLACE) == LREPLACE)

#define P_REPSTR           P_FUNCCOMPL
#define _REPSTR            90 /* replace sub-structure */
#define REPSTR             SA_F(_REPSTR ,4)
#define T_REPSTR(x)        (((x) & P_REPSTR) == REPSTR)

#define P_REPLACE          P_FUNCCOMPL
#define _REPLACE           34
#define REPLACE            SA_F(_REPLACE ,4)
#define T_REPLACE(x)       (((x) & P_REPLACE) == REPLACE)

#define P_MREPLACE_C       P_FUNCCOMPL
#define _MREPLACE_C        35
#define MREPLACE_C         SA_F(_MREPLACE_C ,3)
#define T_MREPLACE_C(x)    (((x) & P_MREPLACE_C) == MREPLACE_C)

#define P_MREPLACE_R       P_FUNCCOMPL
#define _MREPLACE_R        36
#define MREPLACE_R         SA_F(_MREPLACE_R ,3)
#define T_MREPLACE_R(x)    (((x) & P_MREPLACE_R) == MREPLACE_R)

#define P_VREPLACE         P_FUNCCOMPL
#define _VREPLACE          57
#define VREPLACE           SA_F(_VREPLACE ,3)
#define T_VREPLACE(x)      (((x) & P_VREPLACE) == VREPLACE)

#define P_MREPLACE         P_FUNCCOMPL
#define _MREPLACE          89
#define MREPLACE           SA_F(_MREPLACE ,4)
#define T_MREPLACE(x)      (((x) & P_MREPLACE) == MREPLACE)

/*********************************************************************/
/* Group 4                                                           */
/*********************************************************************/

#define P_UNI_VECTOR       P_FUNCCOMPL
#define _UNI_VECTOR        35
#define UNI_VECTOR         SA_F(_UNI_VECTOR ,2)
#define T_UNI_VECTOR(x)    (((x) & P_UNI_VECTOR) == UNI_VECTOR)

/*********************************************************************/
/* Group 5                                                           */
/*********************************************************************/

#define P_UNI_MATRIX       P_FUNCCOMPL
#define _UNI_MATRIX        36
#define UNI_MATRIX         SA_F(_UNI_MATRIX,2)
#define T_UNI_MATRIX(x)    (((x) & P_UNI_MATRIX) == UNI_MATRIX)

/*********************************************************************/
/* Group 6                                                           */
/*********************************************************************/

#define P_UNI_TREE         P_FUNCCOMPL
#define _UNI_TREE          37
#define UNI_TREE           SA_F(_UNI_TREE ,2)
#define T_UNI_TREE(x)      (((x) & P_UNI_TREE) == UNI_TREE)

/*********************************************************************/
/* Group 7                                                           */
/*********************************************************************/

#define P_UNI              P_FUNCCOMPL
#define _UNI               38
#define UNI                SA_F(_UNI ,3)
#define T_UNI(x)           (((x) & P_UNI) == UNI)

#define P_VUNI             P_FUNCCOMPL
#define _VUNI              58
#define VUNI               SA_F(_VUNI ,2)
#define T_VUNI(x)          (((x) & P_VUNI) == VUNI)

#define P_MUNI             P_FUNCCOMPL
#define _MUNI              88
#define MUNI               SA_F(_MUNI ,3)
#define T_MUNI(x)          (((x) & P_MUNI) == MUNI)

#define P_LUNI             P_FUNCCOMPL
#define _LUNI              73
#define LUNI               SA_F(_LUNI ,2)
#define T_LUNI(x)          (((x) & P_LUNI) == LUNI)

/*********************************************************************/
/* Group 8    not used!!!                                            */
/*********************************************************************/

#define P_TAKE             P_FUNCCOMPL
#define _TAKE              39
#define TAKE               SA_F(_TAKE ,4)
#define T_TAKE(x)          (((x) & P_TAKE) == TAKE)

#define P_DROP             P_FUNCCOMPL
#define _DROP              40
#define DROP               SA_F(_DROP ,4)
#define T_DROP(x)          (((x) & P_DROP) == DROP)

/*********************************************************************/
/* Konversionsfunktionen                                             */
/*********************************************************************/

#define P_TO_VECT          P_FUNCCOMPL
#define _TO_VECT           64
#define TO_VECT            SA_F(_TO_VECT ,1)
#define T_TO_VECT(x)       (((x) & P_TO_VECT) == TO_VECT)

#define P_TO_TVECT         P_FUNCCOMPL
#define _TO_TVECT          65
#define TO_TVECT           SA_F(_TO_TVECT ,1)
#define T_TO_TVECT(x)      (((x) & P_TO_TVECT) == TO_TVECT)

#define P_TO_MAT           P_FUNCCOMPL
#define _TO_MAT            66
#define TO_MAT             SA_F(_TO_MAT ,1)
#define T_TO_MAT(x)        (((x) & P_TO_MAT) == TO_MAT)

#define P_TO_SCAL          P_FUNCCOMPL
#define _TO_SCAL           67
#define TO_SCAL            SA_F(_TO_SCAL ,1)
#define T_TO_SCAL(x)       (((x) & P_TO_SCAL) == TO_SCAL)

#define P_TO_LIST          P_FUNCCOMPL
#define _TO_LIST           91
#define TO_LIST            SA_F(_TO_LIST ,1)
#define T_TO_LIST(x)       (((x) & P_TO_LIST) == TO_LIST)

#define P_TO_FIELD         P_FUNCCOMPL
#define _TO_FIELD          92
#define TO_FIELD           SA_F(_TO_FIELD ,1)
#define T_TO_FIELD(x)      (((x) & P_TO_FIELD) == TO_FIELD)

#define P_MK_STRING        P_FUNCCOMPL
#define _MK_STRING         94
#define MK_STRING          SA_F(_MK_STRING , 1)
#define T_MK_STRING(x)     (((x) & P_MK_STRING) == MK_STRING)

#define P_M_FIRST          P_FUNCCOMPL
#define _M_FIRST           68
#define M_FIRST            SA_F(_M_FIRST , 1)
#define T_M_FIRST(x)       (((x) & P_M_FIRST) == M_FIRST)

#define P_M_REST           P_FUNCCOMPL
#define _M_REST            69
#define M_REST             SA_F(_M_REST , 1)
#define T_M_REST(x)        (((x) & P_M_REST) == M_REST)

/*********************************************************************/
/*  entlang einer Koordinate                                         */
/*********************************************************************/

#define P_C_MAX            P_FUNCCOMPL
#define _C_MAX             45
#define C_MAX              SA_F(_C_MAX,2)
#define T_C_MAX(x)         (((x) & P_C_MAX) == C_MAX)

#define P_VC_MAX           P_FUNCCOMPL
#define _VC_MAX            39
#define VC_MAX             SA_F(_VC_MAX,1)
#define T_VC_MAX(x)        (((x) & P_VC_MAX) == VC_MAX)

#define P_C_MIN            P_FUNCCOMPL
#define _C_MIN             46
#define C_MIN              SA_F(_C_MIN,2)
#define T_C_MIN(x)         (((x) & P_C_MIN) == C_MIN)

#define P_VC_MIN           P_FUNCCOMPL
#define _VC_MIN            40
#define VC_MIN             SA_F(_VC_MIN,1)
#define T_VC_MIN(x)        (((x) & P_VC_MIN) == VC_MIN)

#define P_C_PLUS           P_FUNCCOMPL
#define _C_PLUS            47
#define C_PLUS             SA_F(_C_PLUS,2)
#define T_C_PLUS(x)        (((x) & P_C_PLUS) == C_PLUS)

#define P_VC_PLUS          P_FUNCCOMPL
#define _VC_PLUS           41
#define VC_PLUS            SA_F(_VC_PLUS,1)
#define T_VC_PLUS(x)       (((x) & P_VC_PLUS) == VC_PLUS)

#define P_C_MINUS          P_FUNCCOMPL
#define _C_MINUS           48
#define C_MINUS            SA_F(_C_MINUS,2)
#define T_C_MINUS(x)       (((x) & P_C_MINUS) == C_MINUS)

#define P_VC_MINUS         P_FUNCCOMPL
#define _VC_MINUS          42
#define VC_MINUS           SA_F(_VC_MINUS,1)
#define T_VC_MINUS(x)  (((x) & P_VC_MINUS) == VC_MINUS)

#define P_C_MULT           P_FUNCCOMPL
#define _C_MULT            49
#define C_MULT             SA_F(_C_MULT,2)
#define T_C_MULT(x)        (((x) & P_C_MULT) == C_MULT)

#define P_VC_MULT          P_FUNCCOMPL
#define _VC_MULT           43
#define VC_MULT            SA_F(_VC_MULT,1)
#define T_VC_MULT(x)       (((x) & P_VC_MULT) == VC_MULT)

#define P_C_DIV            P_FUNCCOMPL
#define _C_DIV             50
#define C_DIV              SA_F(_C_DIV,2)
#define T_C_DIV(x)         (((x) & P_C_DIV) == C_DIV)

#define P_VC_DIV           P_FUNCCOMPL
#define _VC_DIV            44
#define VC_DIV             SA_F(_VC_DIV,1)
#define T_VC_DIV(x)        (((x) & P_VC_DIV) == VC_DIV)

/*******************************************************************/
/*  Feldgleichheit und Ungleichheit                                */
/*******************************************************************/

#define P_F_EQ             P_FUNCCOMPL
#define _F_EQ              51
#define F_EQ               SA_F(_F_EQ,2)
#define T_F_EQ(x)          (((x) & P_F_EQ) == F_EQ)

#define P_F_NE             P_FUNCCOMPL
#define _F_NE              52
#define F_NE               SA_F(_F_NE,2)
#define T_F_NE(x)          (((x) & P_F_NE) == F_NE)

/* cr 22/03/95, kir(ff), START */
/* numbers adapted to editor-encoding fed/src/cencod.h , cr 13.11.95 */
/*********************************************************************/
/* Frame-Funktionen                                                  */
/*********************************************************************/

#define P_FR_TEST             P_FUNCCOMPL
#define _FR_TEST              107
#define FR_TEST               SA_F(_FR_TEST,2)
#define T_FR_TEST(x)          (((x) & P_FR_TEST) == FR_TEST)

#define P_FR_SELECT             P_FUNCCOMPL
#define _FR_SELECT              108
#define FR_SELECT               SA_F(_FR_SELECT,2)
#define T_FR_SELECT(x)          (((x) & P_FR_SELECT) == FR_SELECT)

#define P_FR_UPDATE             P_FUNCCOMPL
#define _FR_UPDATE              109
#define FR_UPDATE               SA_F(_FR_UPDATE,3)
#define T_FR_UPDATE(x)          (((x) & P_FR_UPDATE) == FR_UPDATE)

/* cr 08.12.95 */
#define P_FR_SLOTS             P_FUNCCOMPL
#define _FR_SLOTS              110
#define FR_SLOTS               SA_F(_FR_SLOTS,1)
#define T_FR_SLOTS(x)          (((x) & P_FR_SLOTS) == FR_SLOTS)

/* cr 08.12.95 */
#define P_FR_DELETE             P_FUNCCOMPL
#define _FR_DELETE              111
#define FR_DELETE               SA_F(_FR_DELETE,2)
#define T_FR_DELETE(x)          (((x) & P_FR_DELETE) == FR_DELETE)

/* cr 22/03/95, kir(ff), END */

/*********************************************************************/
/* Charakter und Zahltransformationen                stt  28.02.96   */
/*********************************************************************/

#define P_TO_CHAR             P_FUNCCOMPL
#define _TO_CHAR              112
#define TO_CHAR               SA_F(_TO_CHAR,1)
#define T_TO_CHAR(x)          (((x) & P_TO_CHAR) == TO_CHAR)

#define P_TO_ORD              P_FUNCCOMPL
#define _TO_ORD               113
#define TO_ORD                SA_F(_TO_ORD,1)
#define T_TO_ORD(x)           (((x) & P_TO_ORD) == TO_ORD)

#define P_SPRINTF             P_FUNCCOMPL
#define _SPRINTF              114
#define SPRINTF               SA_F(_SPRINTF,2)
#define T_SPRINTF(x)          (((x) & P_SPRINTF) == SPRINTF)

#define P_SSCANF              P_FUNCCOMPL
#define _SSCANF               115
#define SSCANF                SA_F(_SSCANF,2)
#define T_SSCANF(x)           (((x) & P_SSCANF) == SSCANF)

/* for speculative evaluation */

#define P_SPECMAP             P_FUNCCOMPL
#define _SPECMAP              116
#define SPECMAP               SA_F(_SPECMAP,2)
#define T_SPECMAP(x)          (((x) & P_SPECMAP) == SPECMAP)

/*********************************************************************/

/*                                                                   */
/* Sonderzeichen   und  Typen                                        */
/*                                                                   */
/*********************************************************************/

/*********************************************************************/
/* Klasse  0                                                         */
/*********************************************************************/

#define P_DOL_CONST        P_CONSTCOMPL
#define _DOL_CONST         0 | (1 <<8)
#define DOL_CONST          SA_C(_DOL_CONST)
#define T_DOL_CONST(x)     (((x) & P_DOL_CONST) == DOL_CONST)

#define P_PLACEH           P_CONSTCOMPL
#define _PLACEH            0 | (2<<8)            /* Platzhalter        */
#define PLACEH             SA_C(_PLACEH)
#define T_PLACEH(x)        (((x) & P_PLACEH) == PLACEH)

#define P_HASH             P_CONSTCOMPL
#define _HASH              0 | (3<<8)
#define HASH               SA_C(_HASH)
#define T_HASH(x)          (((x) & P_HASH) == HASH)

#define P_KLAA             P_CONSTCOMPL
#define _KLAA              0 | (4<<8)            /* KLAmmerAffe @      */
#define KLAA               SA_C(_KLAA)
#define T_KLAA(x)          (((x) & P_KLAA) == KLAA)

#define P_NOVAL            P_CONSTCOMPL
#define _NOVAL             0 | (5<<8)
#define NOVAL              SA_C(_NOVAL)
#define T_NOVAL(x)         (((x) & P_NOVAL) == NOVAL)

/*********************************************************************/
/* Klasse  1                                                         */
/*********************************************************************/

#define P_ESET             P_CONSTCOMPL
#define _ESET              1 | (1<<8)
#define ESET               SA_C(_ESET)
#define T_ESET(x)          (((x) & P_ESET) == ESET)

#define P_NIL              P_CONSTCOMPL
#define _NIL               1 | (2<<8)
#define NIL                SA_C(_NIL)
#define T_NIL(x)           (((x) & P_NIL) == NIL)

#define P_NILMAT           P_CONSTCOMPL
#define _NILMAT            1 | (3<<8)
#define NILMAT             SA_C(_NILMAT)
#define T_NILMAT(x)        (((x) & P_NILMAT) == NILMAT)

#define P_NILVECT          P_CONSTCOMPL
#define _NILVECT           1 | (4<<8)
#define NILVECT            SA_C(_NILVECT)
#define T_NILVECT(x)       (((x) & P_NILVECT) == NILVECT)

#define P_NILTVECT         P_CONSTCOMPL
#define _NILTVECT          1 | (5<<8)
#define NILTVECT           SA_C(_NILTVECT)
#define T_NILTVECT(x)      (((x) & P_NILTVECT) == NILTVECT)

#define P_NILSTRING        P_CONSTCOMPL
#define _NILSTRING         1 | (6<<8)
#define NILSTRING          SA_C(_NILSTRING)
#define T_NILSTRING(x)     (((x) & P_NILSTRING) == NILSTRING)

/* empty frame, cr 11.12.95 */
#define P_NILSTRUCT        P_CONSTCOMPL
#define _NILSTRUCT         1 | (8<<8)
#define NILSTRUCT          SA_C(_NILSTRUCT)
#define T_NILSTRUCT(x)     (((x) & P_NILSTRUCT) == NILSTRUCT)

/*********************************************************************/
/* Klasse  2                                                         */
/*********************************************************************/

#define P_CL_FUNCTION      P_CONSTCOMPL
#define _CL_FUNCTION       2 | (1<<8)
#define CL_FUNCTION        SA_C(_CL_FUNCTION)
#define T_CL_FUNCTION(x)   (((x) & P_CL_FUNCTION) == CL_FUNCTION)

#define P_CL_SCALAR        P_CONSTCOMPL
#define _CL_SCALAR         2 | (2<<8)
#define CL_SCALAR          SA_C(_CL_SCALAR)
#define T_CL_SCALAR(x)     (((x) & P_CL_SCALAR) == CL_SCALAR)

#define P_CL_VECTOR        P_CONSTCOMPL
#define _CL_VECTOR         2 | (3<<8)
#define CL_VECTOR          SA_C(_CL_VECTOR)
#define T_CL_VECTOR(x)     (((x) & P_CL_VECTOR) == CL_VECTOR)

#define P_CL_TVECTOR       P_CONSTCOMPL
#define _CL_TVECTOR        2 | (4<<8)
#define CL_TVECTOR         SA_C(_CL_TVECTOR)
#define T_CL_TVECTOR(x)    (((x) & P_CL_TVECTOR) == CL_TVECTOR)

#define P_CL_MATRIX        P_CONSTCOMPL
#define _CL_MATRIX         2 | (5<<8)
#define CL_MATRIX          SA_C(_CL_MATRIX)
#define T_CL_MATRIX(x)     (((x) & P_CL_MATRIX) == CL_MATRIX)

#define P_CL_SET           P_CONSTCOMPL
#define _CL_SET            2 | (6<<8)
#define CL_SET             SA_C(_CL_SET)
#define T_CL_SET(x)        (((x) & P_CL_SET) == CL_SET)

#define P_CL_TREE          P_CONSTCOMPL
#define _CL_TREE           2 | (7<<8)
#define CL_TREE            SA_C(_CL_TREE)
#define T_CL_TREE(x)       (((x) & P_CL_TREE) == CL_TREE)

#define P_CL_DVECTOR       P_CONSTCOMPL
#define _CL_DVECTOR        2 | (8<<8)
#define CL_DVECTOR         SA_C(_CL_DVECTOR)
#define T_CL_DVECTOR(x)    (((x) & P_CL_DVECTOR) == CL_DVECTOR)

#define P_CL_CVECTOR       P_CONSTCOMPL
#define _CL_CVECTOR        2 | (9<<8)
#define CL_CVECTOR         SA_C(_CL_CVECTOR)
#define T_CL_CVECTOR(x)    (((x) & P_CL_CVECTOR) == CL_CVECTOR)

/*********************************************************************/
/* Klasse  3                                                         */
/*********************************************************************/

#define P_BOOL             P_CONSTCOMPL
#define _BOOL              3 | (1<<8)
#define BOOL               SA_C(_BOOL)
#define T_BOOL(x)          (((x) & P_BOOL) == BOOL)

#define P_DECIMAL          P_CONSTCOMPL
#define _DECIMAL           3 | (4<<8)
#define DECIMAL            SA_C(_DECIMAL)
#define T_DECIMAL(x)       (((x) & P_DECIMAL) == DECIMAL)

#define P_CHAR             P_CONSTCOMPL
#define _CHAR              3 | (5<<8)
#define CHAR               SA_C(_CHAR)
#define T_CHAR(x)          (((x) & P_CHAR) == CHAR)

#define P_LAMBDA           P_CONSTCOMPL
#define _LAMBDA            3 | (6<<8)
#define LAMBDA             SA_C(_LAMBDA)
#define T_LAMBDA(x)        (((x) & P_LAMBDA) == LAMBDA)

#define P_CONDITIONAL      P_CONSTCOMPL
#define _CONDITIONAL       3 | (7<<8)
#define CONDITIONAL        SA_C(_CONDITIONAL)
#define T_CONDITIONAL(x)   (((x) & P_CONDITIONAL) == CONDITIONAL)

#define P_PRIMFUNC         P_CONSTCOMPL
#define _PRIMFUNC          3 | (8<<8)
#define PRIMFUNC           SA_C(_PRIMFUNC)
#define T_PRIMFUNC(x)      (((x) & P_PRIMFUNC) == PRIMFUNC)

#define P_TMATRIX          P_CONSTCOMPL
#define _TMATRIX           3 | (9<<8)
#define TMATRIX            SA_C(_TMATRIX)
#define T_TMATRIX(x)       (((x) & P_TMATRIX) == TMATRIX)

#define P_EXPR             P_CONSTCOMPL
#define _EXPR              3 | (10<<8)
#define EXPR               SA_C(_EXPR)
#define T_EXPR(x)          (((x) & P_EXPR) == EXPR)

#define P_USERFUNC         P_CONSTCOMPL
#define _USERFUNC          3 | (11<<8)
#define USERFUNC           SA_C(_USERFUNC)
#define T_USERFUNC(x)      (((x) & P_USERFUNC) == USERFUNC)

/*********************************************************************/
/* Klasse  4                                                         */
/*********************************************************************/

/*********************************************************************/
/* Klasse  5                                                         */
/*********************************************************************/

#define P_PM_FAIL          P_CONSTCOMPL
#define _PM_FAIL           5 | (3<<8)
#define PM_FAIL            SA_C(_PM_FAIL)
#define T_PM_FAIL(x)       (((x) & P_PM_FAIL) == PM_FAIL)

#define P_PM_SUCC          P_CONSTCOMPL
#define _PM_SUCC           5 | (4<<8)
#define PM_SUCC            SA_C(_PM_SUCC)
#define T_PM_SUCC(x)       (((x) & P_PM_SUCC) == PM_SUCC)

#define P_PM_END           P_CONSTCOMPL
#define _PM_END            5 | (5<<8)
#define PM_END             SA_C(_PM_END)
#define T_PM_END(x)        (((x) & P_PM_END) == PM_END)

#define P_PM_SKIP          P_CONSTCOMPL
#define _PM_SKIP           5 | (6<<8)
#define PM_SKIP            SA_C(_PM_SKIP)
#define T_PM_SKIP(x)       (((x) & P_PM_SKIP) == PM_SKIP)

#define P_PM_SKSKIP        P_CONSTCOMPL
#define _PM_SKSKIP         5 | (7<<8)
#define PM_SKSKIP          SA_C(_PM_SKSKIP)
#define T_PM_SKSKIP(x)     (((x) & P_PM_SKSKIP) == PM_SKSKIP)

#define P_PM_DOLLAR        P_CONSTCOMPL
#define _PM_DOLLAR         5 | (8<<8)
#define PM_DOLLAR          SA_C(_PM_DOLLAR)
#define T_PM_DOLLAR(x)     (((x) & P_PM_DOLLAR) == PM_DOLLAR)

#define P_PM_UDC           P_CONSTCOMPL
#define _PM_UDC            5 | (9<<8)
#define PM_UDC             SA_C(_PM_UDC)
#define T_PM_UDC(x)        (((x) & P_PM_UDC) == PM_UDC)

#define P_PM_GGG           P_CONSTCOMPL
#define _PM_GGG            5 | (10<<8)
#define PM_GGG             SA_C(_PM_GGG)
#define T_PM_GGG(x)        (((x) & P_PM_GGG) == PM_GGG)

#define P_PM_GGG_N         P_CONSTCOMPL
#define _PM_GGG_N          5 | (11<<8)
#define PM_GGG_N           SA_C(_PM_GGG_N)
#define T_PM_GGG_N(x)      (((x) & P_PM_GGG_N) == PM_GGG_N)

#define P_PM_GGG_E         P_CONSTCOMPL
#define _PM_GGG_E          5 | (12<<8)
#define PM_GGG_E           SA_C(_PM_GGG_E)
#define T_PM_GGG_E(x)      (((x) & P_PM_GGG_E) == PM_GGG_E)

#define P_PM_PT_PLUS       P_CONSTCOMPL
#define _PM_PT_PLUS        5 | (13<<8)
#define PM_PT_PLUS         SA_C(_PM_PT_PLUS)
#define T_PM_PT_PLUS(x)    (((x) & P_PM_PT_PLUS) == PM_PT_PLUS)

#define P_PM_SKSTAR        P_CONSTCOMPL
#define _PM_SKSTAR         5 | (15<<8)
#define PM_SKSTAR          SA_C(_PM_SKSTAR)
#define T_PM_SKSTAR(x)     (((x) & P_PM_SKSTAR) == PM_SKSTAR)

/*****************************************************************************/
/* Klasse  6                                                                 */
/*****************************************************************************/

#define P_GNULL            P_CONSTCOMPL
#define _GNULL             6 | (1<<8)
#define GNULL              SA_C(_GNULL)
#define T_GNULL(x)         (((x) & P_GNULL) == GNULL)

#define P_TERMINATE        P_CONSTCOMPL
#define _TERMINATE         6 | (2<<8)
#define TERMINATE          SA_C(_TERMINATE)
#define T_TERMINATE(x)     (((x) & P_TERMINATE) == TERMINATE)

/*********************************************************************/
/* Interaktionen fuer KiR-I/O                       stt  11.09.95    */
/*********************************************************************/
/* Angabe der Stelligkeit ( SA_I(name,arity) ) gilt fuer monadic I/O */

/* Lesen/Schreiben */
#define P_IA_FOPEN      P_IFUNCCOMPL
#define _IA_FOPEN       0x8000
#define IA_FOPEN        SA_I(_IA_FOPEN,2)
#define T_IA_FOPEN(x)   (((x) & P_IA_FOPEN) == IA_FOPEN)

#define P_IA_FCLOSE     P_IFUNCCOMPL
#define _IA_FCLOSE      0x8001
#define IA_FCLOSE       SA_I(_IA_FCLOSE,1)
#define T_IA_FCLOSE(x)  (((x) & P_IA_FCLOSE) == IA_FCLOSE)

#define P_IA_FGETC      P_IFUNCCOMPL
#define _IA_FGETC       0x8002
#define IA_FGETC        SA_I(_IA_FGETC,1)
#define T_IA_FGETC(x)   (((x) & P_IA_FGETC) == IA_FGETC)

#define P_IA_FPUTC      P_IFUNCCOMPL
#define _IA_FPUTC       0x8003
#define IA_FPUTC        SA_I(_IA_FPUTC,2)
#define T_IA_FPUTC(x)   (((x) & P_IA_FPUTC) == IA_FPUTC)

#define P_IA_UNGETC     P_IFUNCCOMPL
#define _IA_UNGETC      0x8004
#define IA_UNGETC       SA_I(_IA_UNGETC,2)
#define T_IA_UNGETC(x)  (((x) & P_IA_UNGETC) == IA_UNGETC)

#define P_IA_FGETS      P_IFUNCCOMPL
#define _IA_FGETS       0x8005
#define IA_FGETS        SA_I(_IA_FGETS,1)
#define T_IA_FGETS(x)   (((x) & P_IA_FGETS) == IA_FGETS)

#define P_IA_READ       P_IFUNCCOMPL
#define _IA_READ        0x8006
#define IA_READ         SA_I(_IA_READ,2)
#define T_IA_READ(x)    (((x) & P_IA_READ) == IA_READ)

#define P_IA_FPUTS      P_IFUNCCOMPL
#define _IA_FPUTS       0x8007
#define IA_FPUTS        SA_I(_IA_FPUTS,2)
#define T_IA_FPUTS(x)   (((x) & P_IA_FPUTS) == IA_FPUTS)

#define P_IA_FPRINTF    P_IFUNCCOMPL
#define _IA_FPRINTF     0x8008
#define IA_FPRINTF      SA_I(_IA_FPRINTF,3)
#define T_IA_FPRINTF(x) (((x) & P_IA_FPRINTF) == IA_FPRINTF)

#define P_IA_FREDIRECT     P_IFUNCCOMPL
#define _IA_FREDIRECT      0x8009
#define IA_FREDIRECT       SA_I(_IA_FREDIRECT,2)
#define T_IA_FREDIRECT(x)  (((x) & P_IA_FREDIRECT) == IA_FREDIRECT)

#define P_IA_FSEEK      P_IFUNCCOMPL
#define _IA_FSEEK       0x800a
#define IA_FSEEK        SA_I(_IA_FSEEK,3)
#define T_IA_FSEEK(x)   (((x) & P_IA_FSEEK) == IA_FSEEK)

#define P_IA_FTELL      P_IFUNCCOMPL
#define _IA_FTELL       0x800b
#define IA_FTELL        SA_I(_IA_FTELL,1)

#define P_IA_EXIT       P_IFUNCCOMPL
#define _IA_EXIT        0x800c
#define IA_EXIT         SA_I(_IA_EXIT,1)
#define T_IA_EXIT(x)    (((x) & P_IA_EXIT) == IA_EXIT)

#define P_IA_EOF        P_IFUNCCOMPL
#define _IA_EOF         0x800d
#define IA_EOF          SA_I(_IA_EOF,1)
#define T_IA_EOF(x)     (((x) & P_IA_EOF) == IA_EOF)

#define P_IA_BIND       P_IFUNCCOMPL
#define _IA_BIND        0x800e
#define IA_BIND         SA_I(_IA_BIND,2)
#define T_IA_BIND(x)    (((x) & P_IA_BIND) == IA_BIND)

#define P_IA_UNIT       P_IFUNCCOMPL
#define _IA_UNIT        0x800f
#define IA_UNIT         SA_I(_IA_UNIT,1)
#define T_IA_UNIT(x)    (((x) & P_IA_UNIT) == IA_UNIT)

/* Dateiverwaltung */

#define P_IA_FINFO      P_IFUNCCOMPL
#define _IA_FINFO       0x8010
#define IA_FINFO        SA_I(_IA_FINFO,2)
#define T_IA_FINFO(x)   (((x) & P_IA_FINFO) == IA_FINFO)

#define P_IA_STATUS     P_IFUNCCOMPL
#define _IA_STATUS      0x8011
#define IA_STATUS       SA_I(_IA_STATUS,1)
#define T_IA_STATUS(x)  (((x) & P_IA_STATUS) == IA_STATUS)

#define P_IA_REMOVE     P_IFUNCCOMPL
#define _IA_REMOVE      0x8012
#define IA_REMOVE       SA_I(_IA_REMOVE,1)
#define T_IA_REMOVE(x)  (((x) & P_IA_REMOVE) == IA_REMOVE)

#define P_IA_RENAME     P_IFUNCCOMPL
#define _IA_RENAME      0x8013
#define IA_RENAME       SA_I(_IA_RENAME,2)
#define T_IA_RENAME(x)  (((x) & P_IA_RENAME) == IA_RENAME)

#define P_IA_COPY       P_IFUNCCOMPL
#define _IA_COPY        0x8014
#define IA_COPY         SA_I(_IA_COPY,2)
#define T_IA_COPY(x)    (((x) & P_IA_COPY) == IA_COPY)

#define P_IA_MKDIR      P_IFUNCCOMPL
#define _IA_MKDIR       0x8015
#define IA_MKDIR        SA_I(_IA_MKDIR,1)
#define T_IA_MKDIR(x)   (((x) & P_IA_MKDIR) == IA_MKDIR)

#define P_IA_RMDIR      P_IFUNCCOMPL
#define _IA_RMDIR       0x8016
#define IA_RMDIR        SA_I(_IA_RMDIR,1)
#define T_IA_RMDIR(x)   (((x) & P_IA_RMDIR) == IA_RMDIR)

#define P_IA_CHDIR      P_IFUNCCOMPL
#define _IA_CHDIR       0x8017
#define IA_CHDIR        SA_I(_IA_CHDIR,1)
#define T_IA_CHDIR(x)   (((x) & P_IA_CHDIR) == IA_CHDIR)

#define P_IA_GETWD      P_IFUNCCOMPL
#define _IA_GETWD       0x8018
#define IA_GETWD        SA_I(_IA_GETWD,0)
#define T_IA_GETWD(x)   (((x) & P_IA_GETWD) == IA_GETWD)

#define P_IA_DIR        P_IFUNCCOMPL
#define _IA_DIR         0x8019
#define IA_DIR          SA_I(_IA_DIR,1)
#define T_IA_DIR(x)     (((x) & P_IA_DIR) == IA_DIR)

#define P_IA_GET        P_IFUNCCOMPL
#define _IA_GET         0x801a
#define IA_GET          SA_I(_IA_GET,1)
#define T_IA_GET(x)     (((x) & P_IA_GET) == IA_GET)

#define P_IA_PUT        P_IFUNCCOMPL
#define _IA_PUT         0x801b
#define IA_PUT          SA_I(_IA_PUT,2)
#define T_IA_PUT(x)     (((x) & P_IA_PUT) == IA_PUT)

#define P_IA_SET_SPEED  P_IFUNCCOMPL
#define _IA_SET_SPEED   0x801c
#define IA_SET_SPEED    SA_I(_IA_SET_SPEED,2)
#define T_IA_SET_SPEED(x)     (((x) & P_IA_SET_SPEED) == IA_SET_SPEED)

#define P_IA_GET_SPEED  P_IFUNCCOMPL
#define _IA_GET_SPEED   0x801d
#define IA_GET_SPEED    SA_I(_IA_GET_SPEED,1)
#define T_IA_GET_SPEED(x)     (((x) & P_IA_GET_SPEED) == IA_GET_SPEED)

#define P_IA_SET_DIRECTION  P_IFUNCCOMPL
#define _IA_SET_DIRECTION   0x801e
#define IA_SET_DIRECTION    SA_I(_IA_SET_DIRECTION,2)
#define T_IA_SET_DIRECTION(x)     (((x) & P_IA_SET_DIRECTION) == IA_SET_DIRECTION)

#define P_IA_GET_DIRECTION  P_IFUNCCOMPL
#define _IA_GET_DIRECTION   0x801f
#define IA_GET_DIRECTION    SA_I(_IA_GET_DIRECTION,1)
#define T_IA_GET_DIRECTION(x)     (((x) & P_IA_GET_DIRECTION) == IA_GET_DIRECTION)

#define P_IA_SET_POINT  P_IFUNCCOMPL
#define _IA_SET_POINT   0x8020
#define IA_SET_POINT    SA_I(_IA_SET_POINT,2)
#define T_IA_SET_POINT(x)     (((x) & P_IA_SET_POINT) == IA_SET_POINT)

#define P_IA_GET_POINT  P_IFUNCCOMPL
#define _IA_GET_POINT   0x8021
#define IA_GET_POINT    SA_I(_IA_GET_POINT,1)
#define T_IA_GET_POINT(x)     (((x) & P_IA_GET_POINT) == IA_GET_POINT)

#define P_IA_SET_RELAY  P_IFUNCCOMPL
#define _IA_SET_RELAY   0x8022
#define IA_SET_RELAY    SA_I(_IA_SET_RELAY,2)
#define T_IA_SET_RELAY(x)     (((x) & P_IA_SET_RELAY) == IA_SET_RELAY)

#define P_IA_GET_RELAY  P_IFUNCCOMPL
#define _IA_GET_RELAY   0x8023
#define IA_GET_RELAY    SA_I(_IA_GET_RELAY,1)
#define T_IA_GET_RELAY(x)     (((x) & P_IA_GET_RELAY) == IA_GET_RELAY)

#define P_IA_SET_SIGNAL  P_IFUNCCOMPL
#define _IA_SET_SIGNAL   0x8024
#define IA_SET_SIGNAL    SA_I(_IA_SET_SIGNAL,2)
#define T_IA_SET_SIGNAL(x)     (((x) & P_IA_SET_SIGNAL) == IA_SET_SIGNAL)

#define P_IA_GET_SIGNAL  P_IFUNCCOMPL
#define _IA_GET_SIGNAL   0x8025
#define IA_GET_SIGNAL   SA_I(_IA_GET_SIGNAL,1)
#define T_IA_GET_SIGNAL(x)     (((x) & P_IA_GET_SIGNAL) == IA_GET_SIGNAL)

#define P_IA_READ_M1  P_IFUNCCOMPL
#define _IA_READ_M1   0x8026
#define IA_READ_M1    SA_I(_IA_READ_M1,1)
#define T_IA_READ_M1(x)     (((x) & P_IA_READ_M1) == IA_READ_M1)

#define P_IA_READ_M2  P_IFUNCCOMPL
#define _IA_READ_M2   0x8027
#define IA_READ_M2    SA_I(_IA_READ_M2,1)
#define T_IA_READ_M2(x)     (((x) & P_IA_READ_M2) == IA_READ_M2)



/* end of stelemdef.h */
