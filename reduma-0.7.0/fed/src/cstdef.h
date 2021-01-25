/******************************************************* File "encod.h" ***/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

#ifdef STELEMDEF
#include "(hurck)stelemdef.h"
#endif

/**************************************************************************/
/*  Stackelement-Masken                                                   */
/**************************************************************************/

#define CONAT_FLAG      0x00000008L
#define FIXVAR_FLAG     0x00000004L
#define CHAIN_FLAG      0x00000002L
#define SASTR_FLAG      0x00000004L
#define DIGLET_FLAG     0x00000002L
#define MARK_FLAG       0x00000001L

#ifndef PRED_FKT
#define CONFNC_FLAG     0x00000002L
#define CONFNC_FIELD    0x00000003L
#define CONFNK_FIELD    0x0000000eL   /* neu, wg neuem isfunccon !     */
#define CONFNH_FIELD    0x0000000fL   /* neu, wg neuem isfunccon !     */
#endif

#define DSPLD_FLAG      0x00008000L
#define ABBREV_FLAG     0x00004000L
#define DSPLD           0x0080        /* fuer todispl(.)               */
#define ABBREV          0x0040
#define DISP_FIELD      0x0000c000L

#define VALUE_FIELD     0xffff0000L
#define O_VALUE_FIELD           16
#define P_VALUE_FIELD   0x0000ffffL

#define EDIT_FIELD      0x0000ff00L
#define EDT6_FIELD      0x00003f00L
#define O_EDIT_FIELD             8
#define P_EDIT_FIELD    0x000000ffL
#define P_EDT6_FIELD    0x0000003fL

#define CLASS_FIELD     0x000000ffL
#define O_CLASS_FIELD            0
#define P_CLASS_FIELD   0x000000ffL

#define FUNC_FIELD      0x000000f0L
#define O_FUNC_FIELD             4
#define P_FUNC_FIELD    0x0000000fL

#define KONSTANT        0x00000003L
#define DSTR0           0x00000006L
#define DSTR1           0x00000007L
#define LSTR0           0x00000004L
#define LSTR1           0x00000005L

#define ARITY0          0x00000000L
#define ARITY1          0x00010000L
#define ARITY2          0x00020000L
/*****************************************************************************/
/*  spezielle Definitionen fuer Claus' Editor                                */
/*****************************************************************************/

#ifdef FKT_VERSION

#define NDISP_FLAG      0x00000001L     /* not display, only constr     */

#ifndef STELEMDEF
#define FIX_CONSTR      0x0000000cL     /* conat | fixvar       */
#define VAR_CONSTR      0x00000008L     /* conat                */
#endif

#define LETDIG          0x0000000eL     /* fuer testen letter, digit (s.u.) */
#define MY_FIELD        0x0000000fL     /* fuer notdisp              (s.u.) */
/* letdig == lstr0 | dstr0 | conat     */
#define is_letter(m)    (((m)&LETDIG)== LSTR0)
#define is_digit(m)     (((m)&LETDIG)== DSTR0)
/* makros       */
#define mark1(m)        ((m) | MARK_FLAG)
#define mark0(m)        ((m) & ~MARK_FLAG)
#define novalue(m)      ((STACKELEM)(m) & ~VALUE_FIELD)
#define noedit(m)       ((STACKELEM)(m) & ~EDIT_FIELD)
#define nodispl(m)      ((STACKELEM)(m) & ~DISP_FIELD)
#define notdisp(m)      (((m)&MY_FIELD) == (CONAT_FLAG | NDISP_FLAG))

#endif

/**************************************************************************/
/* Makros                                                                 */
/**************************************************************************/

#define isconstr(m)     (((m) & CONAT_FLAG) != 0L)
#define isatom(m)       (((m) & CONAT_FLAG) == 0L)
#define isfixcon(m)     (((m) & FIXVAR_FLAG) != 0L)
#define isvarcon(m)     (((m) & FIXVAR_FLAG) == 0L)
#define ischained(m)    (((m) & CHAIN_FLAG) != 0L)
#define isunchained(m)  (((m) & CHAIN_FLAG) == 0L)
#define multi_atom(m)   (((m) & SASTR_FLAG) != 0L)
#define multiatom(m)    (((m) & (CONAT_FLAG | SASTR_FLAG)) == SASTR_FLAG)
#define single_atom(m)  (((m) & SASTR_FLAG) == 0L)
#define singleatom(m)   (((m) & (CONAT_FLAG | SASTR_FLAG)) == 0L)
#define isletstr(m)     (((m) & (CONAT_FLAG | SASTR_FLAG | DIGLET_FLAG)) == SASTR_FLAG)
#define isdigstr(m)     (((m) & (CONAT_FLAG | SASTR_FLAG | DIGLET_FLAG)) == (SASTR_FLAG | DIGLET_FLAG))
#define letterbit(m)    (((m) & DIGLET_FLAG) == 0L)
#define marked1(m)      (((m) & MARK_FLAG) != 0L)
#define marked0(m)      (((m) & MARK_FLAG) == 0L)

#define value(m)        (((m) >> O_VALUE_FIELD) & P_VALUE_FIELD)
#define arity(m)        (((m) >> O_VALUE_FIELD) & P_VALUE_FIELD)
#define func_arity(m)   ((int)(((m) >> O_FUNC_FIELD) & P_FUNC_FIELD))
#define edit(m)         (((m) >> O_EDIT_FIELD) & P_EDIT_FIELD)
#define edit6(m)        (((m) >> O_EDIT_FIELD) & P_EDT6_FIELD)
#define class(m)        (((m) >> O_CLASS_FIELD) & P_CLASS_FIELD)
#define tovalue(m)      (((STACKELEM)(m) & P_VALUE_FIELD) << O_VALUE_FIELD)
#define toarity(m)      (((STACKELEM)(m) & P_VALUE_FIELD) << O_VALUE_FIELD)
#define setarity(m,n)   (((STACKELEM)(m) & ~VALUE_FIELD) | toarity(n))
#define toedit(m)       (((STACKELEM)(m) & P_EDIT_FIELD) << O_EDIT_FIELD)
#define todispl(m)      (((STACKELEM)(m) & P_EDIT_FIELD) << O_EDIT_FIELD)
#define toedit6(m)      (((STACKELEM)(m) & P_EDT6_FIELD) << O_EDIT_FIELD)
#define toclass(m)      (((STACKELEM)(m) & P_CLASS_FIELD) << O_CLASS_FIELD)
#define marking(m)      ((m) & MARK_FLAG)

#define isdispld(m)     (((m) & DSPLD_FLAG) != 0L)
#define isabbrev(m)     (((m) & ABBREV_FLAG) != 0L)

#define AR0(m)          ((m) & ~VALUE_FIELD)

#ifndef PRED_FKT
/* achtung: m nur ohne seiteneffekte !!!!!!!!!!!!!! */
#define isfunccon(m)    (((m)&CONFNK_FIELD)==CONFNK_FIELD ||  \
                         ((m)&CONFNH_FIELD)==0x0000000aL)
#endif /* PRED_FKT */

/**************************************************************************/
/* Fix-Konstruktoren   Stackelemente 0x0000001c - 0x000000cc              */
/**************************************************************************/

#ifndef STELEMDEF
#define PRELIST         0x0001000cL
#define PROTECT         0x0001001cL
#define PLUS            0x0001002cL
#define MINUS           0x0001003cL
#define SET             0x0001004cL
#define REC             0x0002005cL
#define COND            0x0002006cL
#define SCALAR          0x0002007cL
#define VECTOR          0x0002008cL
#define TVECTOR         0x0002009cL
#define MATRIX          0x000300acL
#define EXP             0x000200bcL
#define F               0x000200ccL

/* Harald special */


#define PM_OTHERWISE    0x000100dcL /* H */
#define PM_WHEN         0x000300ecL /* H */
#define PM_MATCH        0x000300fcL /* H */

#endif STELEMDEF

#define THEN            COND

/**************************************************************************/
/* Datentyp-Konstruktoren (Editor) Stackelemente 0x000000dc - 0x000000fd  */
/**************************************************************************/

#define DECSCAL         0x000100dcL
#define INTSCAL         0x000100ecL
#define REALSCAL        0x000100fcL

#define INTVEC          0x0001001dL
#define REALVEC         0x0001002dL
#define BOOLVEC         0x0001003dL
#define NUMBVEC         0x0001004dL
#define STRVEC          0x0001005dL

#define TINTVEC         0x0001006dL
#define TREALVEC        0x0001007dL
#define TBOOLVEC        0x0001008dL
#define TNUMBVEC        0x0001009dL
#define TSTRVEC         0x000100adL

#define INTMAT          0x000100bdL
#define REALMAT         0x000100cdL
#define BOOLMAT         0x000100ddL
#define NUMBMAT         0x000100edL
#define STRMAT          0x000100fdL


#ifndef PRED_FKT
/**************************************************************************/
/* Funktions-Konstruktoren (Editor) Stackelemente 0x0000000e - 0x000000ff */
/*                                                0x0000000a - 0x000000fa */
/**************************************************************************/

#define K_NOT           0x0001000eL
#define K_ABS           0x0001001eL
#define K_TRUNCATE      0x0001002eL
#define K_FLOOR         0x0001003eL
#define K_CEIL          0x0001004eL
#define K_FRAC          0x0001005eL
#define K_EMPTY         0x0001006eL
#define K_CLASS         0x0001007eL
#define K_TYPE          0x0001008eL
#define K_RANK          0x0001009eL
#define K_DIMENSION     0x000200aeL
#define K_TRANSPOSE     0x000100beL
#define K_CUTFIRST      0x000300ceL
#define K_CUTLAST       0x000300deL
#define K_ROTATE        0x000300eeL
#define K_SELECT        0x000300feL
#define K_INSERT        0x0004000fL
#define K_UNI_VECTOR    0x0002001fL
#define K_UNI_MATRIX    0x0002002fL
#define K_UNI_TREE      0x0002003fL
#define K_UNI           0x0003004fL
#define K_TAKE          0x0004005fL
#define K_DROP          0x0004006fL
#define K_INVERT        0x0001007fL
#define K_SOLVE         0x0002008fL
#define K_ZS_NORM       0x0001009fL
#define K_LUB_NORM      0x000100afL
#define K_MIN           0x000200bfL
#define K_MAX           0x000200cfL
#define K_C_MIN         0x000200dfL
#define K_C_MAX         0x000200efL
/* Hurck/Schuppius special */
#define K_NEG           0x000100ffL
#define K_CUT           K_CUTFIRST
#define K_TRANSFORM     K_CUTLAST
#define K_REPLACE       K_INSERT
#define K_LDIMENSION    0x0001000aL
#define K_LCUT          0x0002001aL
#define K_LSELECT       0x0002002aL
#define K_LROTATE       0x0002003aL
#define K_LUNI          0x0002004aL
#define K_LREPLACE      0x0003005aL
#define K_LREVERSE      0x0001006aL
#define K_LTRANSFORM    0x0003007aL
/* Ende Hurck/Schuppius */
/* 0x0000008a - 0x000000ba reserviert fur Funktionskonstruktoren
#define K_              0x0002008aL
#define K_              0x0002009aL
#define K_              0x000200aaL
#define K_              0x000200baL
*/
#define K_C_PLUS        0x000200caL
#define K_C_MINUS       0x000200daL
#define K_C_MULT        0x000200eaL
#define K_C_DIV         0x000200faL

/* Mucha special */
#define K_EQELEM        0x0002000bL
#define K_INSELEM       0x0002001bL
#define K_FIRST         0x0001002bL
#define K_REST          0x0001003bL
#define K_SETEMPTY      0x0001004bL
#define K_MEMBER        0x0002005bL
#define K_UNION         0x0002006bL
#define K_INTERSEC      0x0002007bL
#define K_DELELEM       0x0002008bL
/* 0x0000009b - 0x000000fb reserviert fuer Funktionskonstruktoren
#define K_              0x0002009bL
#define K_              0x000200abL
#define K_              0x000200bbL
#define K_              0x000200cbL
#define K_              0x000200dbL
#define K_              0x000200ebL
#define K_              0x000200fbL
*/
#endif /* PRED_FKT */

/**************************************************************************/
/* Var-Konstruktoren      Stackelemente 0x00000008 - 0x00000078 ?         */
/**************************************************************************/

#ifndef STELEMDEF
#define AP              0x00020008L
#define SUB             0x00020038L
#define LIST            0x00010068L
#define FIELD           0x00010078L

/* Harald special */
#define PM_SWITCH       0x00000088L /* H */
#define LREC            0x00000098L /* c */
#define PM_AS           0x000000a8L /* H */
#define PM_LIST         0x000000b8L /* H */

#endif STELEMDEF

/**************************************************************************/
/* Spezielle Applikatoren (Editor)                                        */
/**************************************************************************/

#define IF              0x00020018L
#define APAL            0x00030028L
#define APFNC           0x00020048L
#define APVAR           0x00020058L

#ifdef FKT_VERSION
#define FDEC            (0x00000078L | NDISP_FLAG)
#define DECL             0x00000088L
#define CALL            (0x00000098L | NDISP_FLAG)
#define CALL1           (0x000200a8L | NDISP_FLAG)
#define CALL2           (0x000300b8L | NDISP_FLAG)
#define CALL3           (0x000400c8L | NDISP_FLAG)
#define CALL4           (0x000500d8L | NDISP_FLAG)
#define US_KON          (0x000000e8L | NDISP_FLAG)

#define NOPAR           0x06000003L
#endif /* FKT_VERSION */

#define CASE            0x000300d8L /* neu */

#define FILNAM          0x000100f8L

/* Harald special */
#define EPM_LIST            0x0000009bL /* H */
#define EPM_AS              0x000000abL /* H */
#define EPM_PATTERN         0x000000bbL /* H */
#define EPM_MATCH           0x000300cbL /* H */
#define EPM_SWITCH          0x000000dbL /* H */
#define EPM_OTHERWISE       0x000100ebL /* H */
#define EPM_WHEN            0x000300fbL /* H */

/* noch frei : 0x00000039 - 0x00000069 */

/* in Editfeld markierte Applikatoren */
#define AP_IF           0x00020108L
#define AP_AL           0x00030208L
#define AP_FNC          0x00020408L
#define AP_VAR          0x00020508L
#define AP_CASE         0x00020608L /* neu */
#define AP_CALL         0x00020708L /* neu */
#define AP_PRED         0x00020808L /* neu */

#define P_LIST          0x00000868L /* neu: fuer pattern-list */
#define US_LIST         0x00000968L /* neu: fuer benutzerdef. konstr. */

/* Hurck/Schuppius special */
#define INTFIELD        0x00010078L
#define NUMBFIELD       0x00010098L
#define BOOLFIELD       0x000100a8L
#define STRFIELD        0x000100b8L
#define FIELDFIELD      0x000100c8L
#define REALFIELD       0x000100e8L /* modified,Claus */
/* Hurck/Schuppius special */

/**************************************************************************/
/* Codierung der atomic-expr                                              */
/**************************************************************************/

#define STBOTTOM        0x00000000L

#ifndef STELEMDEF
#define DOLLAR          0x01000003L
#define PLACEH          0x02000003L
#define HASH            0x03000003L
#define KLAA            0x04000003L
#define NOVAL           0x05000003L
#define ESET            0x01010003L
#define NIL             0x02010003L
#define EXPRESSION      0x00ce0003L
#define CL_FUNCTION     0x01020003L
#define CL_SCALAR       0x02020003L
#define CL_VECTOR       0x03020003L
#define CL_TVECTOR      0x04020003L
#define CL_MATRIX       0x05020003L
#define CL_SET          0x06020003L
#define CL_TREE         0x07020003L
#define CL_DVECTOR      0x08020003L
#define CL_CVECTOR      0x09020003L
#define BOOL            0x01030003L
#define INTEGER         0x02030003L
#define REAL            0x03030003L
#define DECIMAL         0x04030003L
#define CHAR            0x05030003L
#define LAMBDA          0x06030003L
#define CONDITIONAL     0x07030003L
#define PRIMFUNC        0x08030003L
#define AQUOTE          0x01040003L
#define FIL_AQUOTE      0x01040103L /* claus fuer nichtlesbare filenames */
#define EQUOTE          0x02040003L
#define SA_FALSE        0x01050003L
#define SA_TRUE         0x02050003L

#define PM_FAIL         0x03050003L /* H */
#define PM_SUCC         0x04050003L /* H */
#define PM_END          0x05050003L /* H */
#define PM_SKIP         0x06050003L /* H */
#define PM_SKSKIP       0x07050003L /* H */
#endif /* STELEMDEF */

#define NUMBER          DECIMAL
#define STRING          CHAR

#ifndef STELEMDEF

/**************************************************************************/
/* Dyadische Funktionen                                                   */
/**************************************************************************/

#define D_PLUS          0x0a010021L
#define D_MINUS         0x0a020021L
#define D_MULT          0x0a030021L
#define D_DIV           0x0a040021L
#define D_MOD           0x0a050021L
#define D_OR            0x0a060021L
#define D_AND           0x0a070021L
#define D_EXOR          0x0a080021L
#define D_EQ            0x0a090021L
#define D_NEQ           0x0a0a0021L
#define D_LESS          0x0a0b0021L
#define D_LEQ           0x0a0c0021L
#define D_GREAT         0x0a0d0021L
#define D_GEQ           0x0a0e0021L
#define D_MAX           0x0a0f0021L
#define D_MIN           0x0a100021L

/**************************************************************************/
/* Monadische Funktionen                                                  */
/**************************************************************************/

#define M_NOT           0x1e110011L
#define M_ABS           0x1e120011L
#define M_TRUNCATE      0x1e130011L
#define M_FLOOR         0x1e140011L
#define M_CEIL          0x1e150011L
#define M_FRAC          0x1e160011L
#define M_EMPTY         0x1e170011L
#define M_SIN           0x1e500011L         /* struck         */
#define M_COS           0x1e510011L
#define M_TAN           0x1e520011L
#define M_LN            0x1e530011L
#define M_EXP           0x1e540011L
/*
#define M_ARCSIN        0x1e550011L
#define M_ARCCOS        0x1e560011L
#define M_ARCTAN        0x1e570011L
*/
/**************************************************************************/
/* Innerprodukt-Funktionen                                                */
/**************************************************************************/

#define IP_PLUS         0x3c180021L

/**************************************************************************/
/* Query-Funktionen                                                       */
/**************************************************************************/

#define CLASS           0x5f190011L
#define TYPE            0x5f1a0011L
#define RANK            0x5f1b0011L
#define DIMENSION       0x5f1c0021L

/**************************************************************************/
/* Strukturierungs-Funktionen                                             */
/**************************************************************************/

#define TRANSPOSE       0x011d0012L
#define CUTFIRST        0x021e0032L
#define CUTLAST         0x021f0032L
#define ROTATE          0x02200032L
#define SELECT          0x02210032L
#define INSERT          0x03220042L
#define UNI_VECTOR      0x04230022L
#define UNI_MATRIX      0x05240022L
#define UNI_TREE        0x06250022L
#define UNI             0x07260032L
#define TAKE            0x08270042L
#define DROP            0x08280042L
#define INVERT          0x78290011L
#define SOLVE           0x782a0021L
#define ZS_NORM         0x782b0011L
#define LUB_NORM        0x782c0011L

/**************************************************************************/
/* Koordinaten-Funktionen                                                 */
/**************************************************************************/

#define C_MAX           0x962d0021L
#define C_MIN           0x962e0021L
#define C_PLUS          0x962f0021L
#define C_MINUS         0x96300021L
#define C_MULT          0x96310021L
#define C_DIV           0x96320021L

/**************************************************************************/
/* Sonderwuerschte                                                        */
/**************************************************************************/

/* Hurck/Schuppius special */
#define NILFIELD        0x03010003L
#define TFIELD          0x09030003L
#define EXPR            0x0a030003L
#define USERFUNC        0x0b030003L

#define CUT             0x021e0032L /* CUTFIRST */
#define TRANSFORM       0x021f0012L /* CUTLAST  */
#define REPLACE         0x03220042L /* INSERT   */

#define F_EQ            0x96330021L
#define F_NEQ           0x96340021L
#define M_NEG           0x1e350011L

#define LCUT            0x02460022L
#define LSELECT         0x02470022L
#define LROTATE         0x02480022L
#define LUNI            0x07490022L
#define LREPLACE        0x034a0032L
#define REVERSE         0x024b0012L
#define LTRANSFORM      0x024c0032L
#define LDIMENSION      0x5f4d0011L

#endif /* STELEMDEF */

/* Mucha special */
#define D_EQELEM        0x0a500021L
#define D_INSELEM       0x0a510021L
#define M_FIRST         0x1e520011L
#define M_REST          0x1e530011L
#define M_SETEMPTY      0x1e540011L
#define D_MEMBER        0x0a550021L
#define D_UNION         0x0a560021L
#define D_INTERSEC      0x0a570021L
#define D_DELELEM       0x0a580021L

/******************************************************* End of file *******/


