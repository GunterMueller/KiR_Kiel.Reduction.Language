/* $Log: rops.h,v $
 * Revision 1.1  1992/11/04 18:12:30  base
 * Initial revision
 * */


/*****************************************************************************/
/* rops.h                                                                    */
/* Definition der Operationen fuer  rval-int.c, rvalfunc.c                   */
/*                                  rlog-mvt.c, rlogfunc.c                   */
/*****************************************************************************/

/* ------------------- rlog-mvt.c, rlogfunc.c ------------------------------ */
#define OP_EQ    1                           /* compare operations           */
#define OP_NEQ   2
#define OP_GT    3
#define OP_GE    4
#define OP_LE    5
#define OP_LT    6
#define OP_MAX   7
#define OP_MIN   8

#define OP_NOT    9                          /* boolmvt operations           */
#define OP_TRUE  10
#define OP_FALSE 11

#define OP_OR    12                          /* boolmvt - boolmvt operations */
#define OP_AND   13
#define OP_XOR   14


/* ------------------  rval-int.c, rvalfunc.c  ----------------------------- */
#define OP_ADD    1                         /* dyadische Operationen         */
#define OP_SUB    2
#define OP_SUBI   3                         /* das I bei zB. SUBI bedeutet   */
#define OP_MULT   4                         /* das die Argumente vertauscht  */
#define OP_DIV    5                         /* wurden.                       */
#define OP_DIVI   6
#define OP_MOD    7
#define OP_MODI   8


#define OP_ABS   15                         /* monadische Operationen       */
#define OP_TRUNC 16
#define OP_FRAC  17
#define OP_FLOOR 18
#define OP_CEIL  19
#define OP_NEG   20

/************************  end of file rops.h ********************************/
