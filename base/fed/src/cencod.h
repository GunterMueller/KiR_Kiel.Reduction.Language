/*	cencod.h */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* was ist mit der orginalen stelemdef.h ?  */

/*
#ifdef STELEMDEF
#include "stelemdef.h"
#endif
*/

/**************************************************************************/
/*  Stackelement-Masken	*/
/**************************************************************************/

#define CONAT_FLAG		0x00000008L
#define FIXVAR_FLAG		0x00000004L
#define CHAIN_FLAG		0x00000002L
#define SASTR_FLAG		0x00000004L
#define DIGLET_FLAG		0x00000002L
#define MARK_FLAG		0x00000001L

#define LOWBIT			0x00000001L	/* fuer interne AP's (reduma) */

#define DSPLD_FLAG		0x00008000L
#define ABBREV_FLAG		0x00004000L
#define DSPLD			0x0080	/* fuer todispl(.)               */
#define ABBREV			0x0040
#define DISP_FIELD		0x0000c000L

#define VALUE_FIELD		0xffff0000L
#define O_VALUE_FIELD		16
#define P_VALUE_FIELD		0x0000ffffL

#define EDIT_FIELD		0x0000ff00L
#define EDT6_FIELD		0x00003f00L
#define O_EDIT_FIELD		8
#define P_EDIT_FIELD		0x000000ffL
#define P_EDT6_FIELD		0x0000003fL

#define CLASS_FIELD		0x000000ffL
#define O_CLASS_FIELD		0
#define P_CLASS_FIELD		0x000000ffL

#define FUNC_FIELD		0x000000f0L
#define O_FUNC_FIELD		4
#define P_FUNC_FIELD		0x0000000fL

#define KONSTANT		0x00000003L
#define DSTR0			0x00000006L
#define DSTR1			0x00000007L
#define LSTR0			0x00000004L
#define LSTR1			0x00000005L

#define NSTRBIT			0x00000010L

#define ARITY0			0x00000000L
#define ARITY1			0x00010000L
#define ARITY2			0x00020000L
/*****************************************************************************/
/*  spezielle Definitionen fuer meinen Editor	*/
/*****************************************************************************/

#define NDISP_FLAG		0x00000001L     /* not display, only constr     */

/* #ifndef STELEMDEF      */
#define FIX_CONSTR		0x0000000cL     /* conat | fixvar	*/
#define VAR_CONSTR		0x00000008L     /* conat	*/

#define LETDIG			0x0000000eL     /* fuer testen letter, digit (s.u.) */
#define MY_FIELD		0x0000000fL     /* fuer notdisp	(s.u.) */
/* letdig == lstr0 | dstr0 | conat     */
#define is_letter(m)    (((m)&LETDIG)== LSTR0)
#define is_digit(m)     (((m)&LETDIG)== DSTR0)
/* makros	*/
#define mark1(m)	((m) | MARK_FLAG)
#define mark0(m)	((m) & ~MARK_FLAG)
#define markstr(m)      (((m) & ~MARK_FLAG) | NSTRBIT)
#define clrstr(m)	((m) & ~NSTRBIT)
#define is_r_str(m)     (((m) & NSTRBIT) != 0L)
#define novalue(m)      ((STACKELEM)(m) & ~VALUE_FIELD)
#define noedit(m)	((STACKELEM)(m) & ~EDIT_FIELD)
#define nodispl(m)      ((STACKELEM)(m) & ~DISP_FIELD)
#define notdisp(m)      (((m)&MY_FIELD) == (CONAT_FLAG | NDISP_FLAG))

/* #endif	*/

/**************************************************************************/
/* Makros	*/
/**************************************************************************/

#define NFUNCTION		0x80FFFFFDL	/* alte unterscheidung      */
                                                /* 8 fuer Interaktionen stt */
#define FUNCTION		0x00000002L	/* alte unterscheidung      */
#define NEW_FCT			0x00000001L	/* struct/valfunc           */
#define new_fct(m)	(((m) & NFUNCTION) | NEW_FCT)
#define is_func(m)	((((m) & P_FUNC_FIELD)==FUNCTION)||(((m) & P_FUNC_FIELD)==NEW_FCT))

#define isconstr(m)	(((m) & CONAT_FLAG) != 0L)
#define isatom(m)	(((m) & CONAT_FLAG) == 0L)
#define isfixcon(m)	(((m) & FIXVAR_FLAG) != 0L)
#define isvarcon(m)	(((m) & FIXVAR_FLAG) == 0L)
#define ischained(m)	(((m) & CHAIN_FLAG) != 0L)
#define isunchained(m)	(((m) & CHAIN_FLAG) == 0L)
#define multi_atom(m)	(((m) & SASTR_FLAG) != 0L)
#define multiatom(m)	(((m) & (CONAT_FLAG | SASTR_FLAG)) == SASTR_FLAG)
#define single_atom(m)	(((m) & SASTR_FLAG) == 0L)
#define singleatom(m)	(((m) & (CONAT_FLAG | SASTR_FLAG)) == 0L)
#define isletstr(m)	(((m) & (CONAT_FLAG | SASTR_FLAG | DIGLET_FLAG)) == SASTR_FLAG)
#define isdigstr(m)	(((m) & (CONAT_FLAG | SASTR_FLAG | DIGLET_FLAG)) == (SASTR_FLAG | DIGLET_FLAG))
#define letterbit(m)	(((m) & DIGLET_FLAG) == 0L)
#define marked1(m)	(((m) & MARK_FLAG) != 0L)
#define marked0(m)	(((m) & MARK_FLAG) == 0L)

#define value(m)	(((m) >> O_VALUE_FIELD) & P_VALUE_FIELD)
#define arity(m)	(((m) >> O_VALUE_FIELD) & P_VALUE_FIELD)
#define func_arity(m)	((int)(((m) >> O_FUNC_FIELD) & P_FUNC_FIELD))
#define edit(m)		(((m) >> O_EDIT_FIELD) & P_EDIT_FIELD)
#define edit6(m)	(((m) >> O_EDIT_FIELD) & P_EDT6_FIELD)
#define class(m)	(((m) >> O_CLASS_FIELD) & P_CLASS_FIELD)
#define tovalue(m)	(((STACKELEM)(m) & P_VALUE_FIELD) << O_VALUE_FIELD)
#define toarity(m)	(((STACKELEM)(m) & P_VALUE_FIELD) << O_VALUE_FIELD)
#define setarity(m,n)	(((STACKELEM)(m) & ~VALUE_FIELD) | toarity(n))
#define toedit(m)	(((STACKELEM)(m) & P_EDIT_FIELD) << O_EDIT_FIELD)
#define todispl(m)	(((STACKELEM)(m) & P_EDIT_FIELD) << O_EDIT_FIELD)
#define toedit6(m)	(((STACKELEM)(m) & P_EDT6_FIELD) << O_EDIT_FIELD)
#define toclass(m)	(((STACKELEM)(m) & P_CLASS_FIELD) << O_CLASS_FIELD)
#define marking(m)	((m) & MARK_FLAG)

#define isdispld(m)	(((m) & DSPLD_FLAG) != 0L)
#define isabbrev(m)	(((m) & ABBREV_FLAG) != 0L)

#define AR0(m)	((m) & ~VALUE_FIELD)

/**************************************************************************/
/* Var-Konstruktoren      Stackelemente 0x00000008 - 0x000000f8	*/
/**************************************************************************/
/* #ifndef STELEMDEF      */
#define AP		0x00020008L
#define SYSAP		(AP | LOWBIT)
#define IF		0x00020018L	/* editor         */
#define APAL		0x00030028L	/* editor         */
#define SUB		0x00020038L
#ifdef UnBenutzt
#define APFNC		0x00020048L	/* editor         */
#define APVAR		0x00020058L	/* editor         */
#endif
#define LDEC		(0x00020048L | NDISP_FLAG)
#define PDEC		(0x00020058L | NDISP_FLAG)

#define LIST		0x00010068L
#define FIELD		0x00010078L
#define FDEC		(0x00000078L | NDISP_FLAG)
#define PM_SWITCH	0x00000088L	/* H reduma       */
#define DECL		0x00000088L	/* editor         */
#define LREC		0x00000098L	/* c reduma       */
#define NUMBFIELD	0x00010098L	/* editor         */
#define CALL		(0x00000098L | NDISP_FLAG)   /* editor	*/
#define PM_AS		0x000000a8L	/* H reduma       */
#define BOOLFIELD	0x000100a8L	/* editor         */
#define CALL1		(0x000200a8L | NDISP_FLAG)   /* editor	*/
#define PM_LIST		0x000000b8L	/* H reduma       */
#define STRFIELD	0x000100b8L	/* editor         */
#define CALL2		(0x000300b8L | NDISP_FLAG)   /* editor	*/
#define PM_REC		0x000000c8L	/* H reduma       */
#define FIELDFIELD	0x000100c8L	/* editor         */
#define CALL3		(0x000400c8L | NDISP_FLAG)   /* editor	*/
#define CALL4		(0x000500d8L | NDISP_FLAG)   /* editor	*/
#define EPM_REC		0x000000d8L	/* H editor       */
#define PM_NCASE	0x000000d8L	/* H reduma       */
#define US_KON		(0x000000e8L | NDISP_FLAG)   /* editor	*/
#define FILRED		0x000100e8L	/* editor         */
#define PM_FCASE	0x000000e8L	/* H reduma       */
#define FILNAM		0x000100f8L	/* editor         */
#define PM_ECASE	0x000000f8L	/* H reduma       */

/* #endif STELEMDEF	*/
/* in Editfeld markierte Konstruktoren */
#define AP_IF		0x00020108L
#define AP_AL		0x00030208L
#define AP_FNC		0x00020408L
#define AP_VAR		0x00020508L
#define AP_CALL		0x00020708L /* neu */
#define AP_PRED		0x00020808L /* neu */
#define AP_LET		0x00020a08L /* neu:fuer LET	*/
#define AP_LETP		0x00020b08L /* neu:fuer LETP	*/
#define AP_SUCC		0x00040e08L /* neu:fuer SUCCESS */
#define AP_NORM		0x00040f08L /* neu:normal-order fuer DISON */
#define P_LIST		0x00000868L /* neu: fuer pattern-list */
#define US_LIST		0x00000968L /* neu: fuer benutzerdef. konstr. */

/************************************************************************/
/* Konstruktoren      Stackelemente 0x0000000a - 0x000000fa		*/
/************************************************************************/

#define LETPAR		0x0000000aL	/* editor         */
#define AP_LETPAR	0x0000000aL	/* neu:fuer LETPAR (reduma)	*/

#define EPM_NDCASE      0x0000008aL
#define PM_NDCASE       0x0000008aL

#if ClausR
#define N_CR		0x0000004aL	/* editor und reduma         */
#define SUB_CR		0x0001005aL	/* editor ?	*/
#define CASE_CR		0x0000006aL	/* editor und reduma         */
#define DECL_CR		(0x0000007aL | NDISP_FLAG )	/* editor und reduma         */
#define WHEN_CR		0x0000008aL	/* editor und reduma         */
#define PROT_CR		0x0000009aL	/* editor und reduma         */
#define STRUCT_CR	0x000000aaL	/* editor und reduma         */
#define WHENST_CR	0x000000baL	/* editor und reduma         */
#endif /* ClausR	*/
#define ZF_UH		0x000000caL	/* editor und reduma         */
#define ZFIN_UH		0x000000daL	/* editor und reduma         */
#if ClausR
#define AP_CR		0x000200eaL	/* reduma	*/
#define OBJ_CR		0x000000faL	/* editor und reduma         */
#endif /* ClausR	*/

/**************************************************************************/
/* Var-Konstruktoren      Stackelemente 0x0000000b - 0x000000fb	*/
/**************************************************************************/
#define FILFKT		0x0000000bL	/* editor         */

#define NVECT		0x0000001bL
#define BVECT		0x0000002bL
#define SVECT		0x0000003bL
#define NTVECT		0x0000004bL
#define BTVECT		0x0000005bL
#define STVECT		0x0000006bL
#define FILPRE		0x0001007bL
#define E_DOT		0x0002008bL

#define EPM_LIST	0x0000009bL	/* H editor       */
#define EPM_AS		0x000000abL	/* H editor       */
#define EPM_PATTERN	0x000000bbL	/* H editor       */
#define EPM_MATCH	0x000300cbL	/* H editor       */
#define EPM_SWITCH	0x000000dbL	/* H editor       */
#define EPM_OTHERWISE	0x000100ebL	/* H editor       */
#define EPM_WHEN	0x000300fbL	/* H editor       */

/**************************************************************************/
/* Fix-Konstruktoren   Stackelemente 0x0000001c - 0x000000fc	*/
/**************************************************************************/
/* #ifndef STELEMDEF	*/
#define PRELIST		0x0001000cL	/* reduma ?	*/
#define PROTECT		0x0001001cL
#define PLUS		0x0001002cL
#define MINUS		0x0001003cL
#define SET		0x0001004cL
#define REC		0x0002005cL
#define COND		0x0002006cL
#define R_DOT		0x0002016cL
#define SCALAR		0x0002007cL
#define VECTOR		0x0002008cL
#define TVECTOR		0x0002009cL
#define MATRIX		0x000300acL
#define EXP		0x000200bcL
/* #define F		0x000200ccL /* was ist das ? */
#define PM_NOT		0x000200ccL /* was ist das ? */
#define PM_OTHERWISE	0x000100dcL /* H fuer reduma !	*/
#define PM_WHEN		0x000300ecL /* H */
#define PM_MATCH	0x000300fcL /* H */
/* #endif STELEMDEF	*/
#define THEN	COND
#define CONS    SET

/**************************************************************************/
/* Datentyp-Konstruktoren (Editor) Stackelemente 0x0000001d - 0x000000fd  */
/**************************************************************************/

#define EPM_NOT		0x0001000dL	/* H editor       */
#define BOOLVEC		0x0001001dL
#define NUMBVEC		0x0001002dL
#define STRVEC		0x0001003dL
#define TBOOLVEC	0x0001004dL
#define TNUMBVEC	0x0001005dL
#define TSTRVEC		0x0001006dL
#define BOOLMAT		0x0001007dL
#define NUMBMAT		0x0001008dL
#define STRMAT		0x0001009dL

#define EPM_NCASE	0x000200adL
#define EPM_FCASE	0x000200bdL
#define EPM_ECASE	0x000200cdL
#define EPM_NREC	0x000200ddL	/* H editor       */
#define EPM_FREC	0x000200edL	/* H editor       */
#define EPM_EREC	0x000200fdL	/* H editor       */
#define PM_NREC		0x0002000dL	/* H reduma       */
#define PM_FREC		0x0002001dL	/* H reduma       */
#define PM_EREC		0x0002002dL	/* H reduma       */


/**************************************************************************/
/* Var-Konstruktoren      Stackelemente 0x0000000e - 0x000000fe	*/
/**************************************************************************/

#define KQUOTE		0x0000000eL	/* editor und reduma         */
#if N_STR
#define EPM_IN		0x0000001eL	/* editor und reduma         */
#define NPATT		0x0000002eL	/* editor         */
#define E_SUCCESS	0x0001003eL	/* editor         */
#define EPM_VAR		0x0000004eL	/* editor         */
#define EPM_SAS		0x0000005eL	/* editor         */
#define E_COMM		0x0000006eL	/* editor und reduma         */
/* neu: zusaetzlicher parameter arity 3 -> 4 (25.4.90)	*/
#define E_DISON		0x0004007eL	/* editor */
#define PM_VARC		0x0001008eL	/* editor und reduma	*/
#define E_LET		0x0003009eL	/* editor */
#define E_ERROR		0x000100aeL	/* editor und reduma	*/
#define E_STOP		0x000100beL	/* editor und reduma	*/
#define E_LETP		0x000300ceL	/* editor */
#define E_MAP		0x000200deL	/* editor */
#define E_MAP_		0x000200eeL	/* editor */
#define E_ONEOF		0x000100feL	/* editor */
#endif

/************************************************************************/
/* Konstruktoren      Stackelemente 0x0000000f - 0x000000ff		*/
/************************************************************************/

#define E_ALLOC		0x0000000fL	/* editor         */
#define E_ASSIGN	0x0000001fL	/* editor         */
#define E_SEQ		0x0000002fL	/* editor         */
#define E_ARRAY		0x0000003fL	/* editor         */
#define E_SEL		0x0002004fL	/* editor         */


/**************************************************************************/
/* Codierung der atomic-expr	*/
/**************************************************************************/

#define STBOTTOM		0x00000000L

/* #ifndef STELEMDEF      */
#define DOLLAR		0x01000003L
#define PLACEH		0x02000003L
#define HASH		0x03000003L
#define KLAA		0x04000003L
#define NOVAL		0x05000003L
#define NOPAR		0x06000003L
#if N_STR
#define DNOPAR		0x06020003L
#define DQUOTE		0x07000003L	/* editor         */
#define DHASH		0x08000003L	/* editor         */
#define DPERC		0x09000003L	/* editor         */
#define DSINGLE		0x0a000003L	/* editor         */
#define DBACK		0x0b000003L	/* editor         */
#define DBSLASH         0x0c000003L	/* editor, stt 14.11.95 */
#endif
#define ESET		0x01010003L
#define NIL		0x02010003L
#define EXPRESSION	0x00ce0003L
#define CL_FUNCTION	0x01020003L
#define CL_SCALAR	0x02020003L
#define CL_VECTOR	0x03020003L
#define CL_TVECTOR	0x04020003L
#define CL_MATRIX	0x05020003L
#define CL_SET		0x06030003L
#define CL_TREE		0x07020003L
#define CL_DVECTOR	0x08020003L
#define CL_CVECTOR	0x09020003L
#define BOOL		0x01030003L
#define INTEGER		0x02030003L
#define REAL		0x03030003L
#define DECIMAL		0x04030003L
#define CHAR		0x05030003L
#define LAMBDA		0x06030003L
#define CONDITIONAL	0x07030003L
#define PRIMFUNC	0x08030003L
#define AQUOTE		0x01040003L
#define FIL_AQUOTE	0x01040103L /* claus fuer nichtlesbare filenames */
#define EQUOTE		0x02040003L
#define SA_FALSE	0x01050003L
#define R_HEAD		0x02050103L
#define R_TAIL		0x01050103L
#define SA_TRUE		0x02050003L
#define SA_NULL		0x01060003L	/* fuer graph     */
#define SA_TERMINATE	0x02060003L	/* fuer graph     */

#define NILFIELD	0x03010003L
#define NILVECT		0x04010003L
#define NILTVECT	0x05010003L
#define NILSTR		0x06010003L
#define NILARRAY	0x07010003L
#define NILSTRUCT	0x08010003L /* empty frame, cr 11.12.95 */

#define TFIELD		0x09030003L
#define EXPR		0x0a030003L
#define USERFUNC	0x0b030003L

#define PM_FAIL		0x03050003L	/* H */
#define PM_SUCC		0x04050003L	/* H */
#define PM_END		0x05050003L	/* H */
#define PM_SKIP		0x06050003L	/* H */
#define PM_SKSKIP	0x07050003L	/* H */
#define PM_DOLLAR	0x08050003L	/* H */
#define PM_SSKIP	0x09050003L	/* editor */
#define PM_SSKSKIP	0x0a050003L	/* editor */
#define PM_SDOLLAR	0x0b050003L	/* editor */
#define PM_SSKPLUS	0x0c050003L	/* editor */
#define PM_SKPLUS	0x0d050003L	/* H */
#define PM_SSKSTAR	0x0e050003L	/* editor */
#define PM_SKSTAR	0x0f050003L	/* editor */
/* #endif STELEMDEF */

#define SELF_S	0x00070003L
#define SELF_V	0x80070003L
#define DISI_S	0x01070003L
#define DISI_V	0x81070003L
#define DISE_S	0x02070003L
#define DISE_V	0x82070003L
#define SYS_S	0x03070003L
#define SYS_V	0x83070003L

#define NUMBER	DECIMAL
#define STRING	CHAR

/* #ifndef STELEMDEF      */

/**************************************************************************/
/* Dyadische Funktionen	*/
/**************************************************************************/

#if N_MAT
#define D_PLUS		0x00010021L
#define D_MINUS		0x00020021L
#define D_MULT		0x00030021L
#define D_DIV		0x00040021L
#define D_MOD		0x00050021L
#define D_OR		0x00060021L
#define D_AND		0x00070021L
#define D_EXOR		0x00080021L
#define D_EQ		0x00090021L
#define D_NEQ		0x000a0021L
#define D_LESS		0x000b0021L
#define D_LEQ		0x000c0021L
#define D_GREAT		0x000d0021L
#define D_GEQ		0x000e0021L
#define D_MAX		0x000f0021L
#define D_MIN		0x00100021L

/**************************************************************************/
/* Monadische Funktionen	*/
/**************************************************************************/

#define M_NOT		0x00110011L
#define M_ABS		0x00120011L
#define M_TRUNCATE	0x00130011L
#define M_FLOOR		0x00140011L
#define M_CEIL		0x00150011L
#define M_FRAC		0x00160011L
#define M_EMPTY		0x00170011L

#define IP_PLUS		0x00180021L
/* Query-Funktionen	*/
#define CLASS		0x00190011L
#define TYPE		0x001a0011L
#define RANK		0x001b0011L
#define DIMENSION	0x001c0021L

#define TRANSPOSE	0x001d0011L
#define CUT		0x001e0031L
#define TRANSFORM	0x001f0011L
#define ROTATE		0x00200031L
#define SELECT		0x00210031L
#define REPLACE		0x00220041L
#define REPLACE_C	0x00230031L
#define REPLACE_R	0x00240031L
#if ClausR
#define FILTER_CR	0x00250021L
#endif /* ClausR	*/
#define UNI		0x00260031L
#define VC_MAX		0x00270011L
#define VC_MIN		0x00280011L
#define VC_PLUS		0x00290011L
#define VC_MINUS	0x002a0011L
#define VC_MULT		0x002b0011L
#define VC_DIV		0x002c0011L

#define C_MAX		0x002d0021L
#define C_MIN		0x002e0021L
#define C_PLUS		0x002f0021L
#define C_MINUS		0x00300021L
#define C_MULT		0x00310021L
#define C_DIV		0x00320021L

#define F_EQ		0x00330021L
#define F_NEQ		0x00340021L

#define M_NEG		0x00350011L

#define VCUT		0x00360021L
#define VROTATE		0x00370021L
#define VSELECT		0x00380021L
#define VREPLACE	0x00390031L
#define VUNI		0x003a0021L
#define VDIM		0x003b0011L
#define MDIM		0x003c0021L
#define SUBSTR		0x003d0031L

#define LGEN_UH		0x003e0031L
#if ClausR
#define UNIFY_CR	0x003f0021L
#endif /* ClausR	*/


#define TO_VECT		0x00400011L
#define TO_TVECT	0x00410011L
#define TO_MAT		0x00420011L
#define TO_SCAL		0x00430011L

#define M_FIRST		0x00440011L
#define M_REST		0x00450011L

#define LCUT		0x00460021L
#define LSELECT		0x00470021L
#define LROTATE		0x00480021L
#define LUNI		0x00490021L
#define LREPLACE	0x004a0031L
#define REVERSE		0x004b0011L
#define LTRANSFORM	0x004c0031L

#define LDIMENSION	0x004d0011L
#define D_CONS		0x004e0021L
#define D_QUOT		0x004f0021L


#define M_SIN		0x00500011L	/* struck         */
#define M_COS		0x00510011L
#define M_TAN		0x00520011L
#define M_LN		0x00530011L
#define M_EXP		0x00540011L

#define MCUT		0x00550031L
#define MSELECT		0x00560031L
#define MROTATE		0x00570031L
#define MUNI		0x00580031L
#define MREPLACE	0x00590041L
#define REPSTR		0x005a0041L

#define TOLIST		0x005b0011L
#define FROMLIST	0x005c0011L
#define DISON		0x005d0031L
#define MKSTRING	0x005e0011L

#define MAP		0x005f0031L
#define MAP_		0x00600031L
#define ONEOF		0x00610031L
#define _SUCCESS	0x00620031L

#define M_HEAD		0x00630011L
#define M_TAIL		0x00640011L
/* keine frei : (mom. dez. 100, s. rear.c !)	*/
#if ClausR
#define OR_CR		0x00650011L
#define ISN_CR		0x00660011L
#define LEFT_CR		0x00670011L
#define RIGHT_CR	0x00680011L
#define MAP_CR		0x00690021L
#define AND_CR		0x006a0011L
#define FR_TEST 	0x006b0021L
#define FR_SELECT	0x006c0021L  
#define FR_UPDATE	0x006d0031L
#define FR_SLOTS	0x006e0011L /* cr 04.12.95 */
#define FR_DELETE	0x006f0021L /* cr 04.12.95 */

#endif /* ClausR	*/

/* Zahl-String-Wandlung,       stt, 28.02.96 */
#define TO_CHAR         0x00700011L
#define TO_ORD          0x00710011L
#define SPRINTF         0x00720021L
#define SSCANF          0x00730021L

#define SPECMAP         0x00740021L

/**************************************************************************/
/* Interaktionen                                             stt  6.09.95 */
/**************************************************************************/
/* Aenderung der Stelligkeit fuer monadic based I/O.    stt 8.1.96 */
#define IA_FOPEN        0x80000021L
#define IA_FCLOSE       0x80010011L
#define IA_FGETC        0x80020011L
#define IA_FPUTC        0x80030021L
#define IA_UNGETC       0x80040021L
#define IA_FGETS        0x80050011L
#define IA_READ         0x80060021L
#define IA_FPUTS        0x80070021L
#define IA_FREDIRECT    0x80090021L
#define IA_FSEEK        0x800a0031L
#define IA_FTELL        0x800b0011L
#define IA_EXIT         0x800c0011L
#define IA_EOF          0x800d0011L
#define IA_BIND         0x800e0021L
#define IA_UNIT         0x800f0011L
#define IA_FINFO        0x80100021L
#define IA_STATUS       0x80110011L
#define IA_REMOVE       0x80120011L
#define IA_RENAME       0x80130021L
#define IA_COPY         0x80140021L
#define IA_MKDIR        0x80150011L
#define IA_RMDIR        0x80160011L
#define IA_CHDIR        0x80170011L
#define IA_GETWD        0x80180001L
#define IA_DIR          0x80190011L
#define IA_GET          0x801a0011L
#define IA_PUT          0x801b0021L

/* additional interactions for the model railroad */

#define IA_SET_SPEED       0x801c0021L
#define IA_GET_SPEED       0x801d0011L
#define IA_SET_DIRECTION   0x801e0021L
#define IA_GET_DIRECTION   0x801f0011L
#define IA_SET_POINT       0x80200021L
#define IA_GET_POINT       0x80210011L
#define IA_SET_RELAY       0x80220021L
#define IA_GET_RELAY       0x80230011L
#define IA_SET_SIGNAL      0x80240021L
#define IA_GET_SIGNAL      0x80250011L
#define IA_READ_M1         0x80260011L
#define IA_READ_M2         0x80270011L

#else   /* !N_MAT	*/

#define D_PLUS		0x0a010021L
#define D_MINUS		0x0a020021L
#define D_MULT		0x0a030021L
#define D_DIV		0x0a040021L
#define D_MOD		0x0a050021L
#define D_OR		0x0a060021L
#define D_AND		0x0a070021L
#define D_EXOR		0x0a080021L
#define D_EQ		0x0a090021L
#define D_NEQ		0x0a0a0021L
#define D_LESS		0x0a0b0021L
#define D_LEQ		0x0a0c0021L
#define D_GREAT		0x0a0d0021L
#define D_GEQ		0x0a0e0021L
#define D_MAX		0x0a0f0021L
#define D_MIN		0x0a100021L

/**************************************************************************/
/* Monadische Funktionen	*/
/**************************************************************************/

#define M_NOT		0x1e110011L
#define M_ABS		0x1e120011L
#define M_TRUNCATE	0x1e130011L
#define M_FLOOR		0x1e140011L
#define M_CEIL		0x1e150011L
#define M_FRAC		0x1e160011L
#define M_EMPTY		0x1e170011L

#define IP_PLUS		0x3c180021L
/* Query-Funktionen	*/
#define CLASS		0x5f190011L
#define TYPE		0x5f1a0011L
#define RANK		0x5f1b0011L
#define DIMENSION	0x5f1c0021L

#define TRANSPOSE	0x011d0012L
#define CUT		0x021e0032L
#define TRANSFORM	0x021f0012L
#define ROTATE		0x02200032L
#define SELECT		0x02210032L
#define REPLACE		0x03220042L
/* was ist mit 23: s. rear !? */
#define UNI		0x07260032L

#define C_MAX		0x962d0021L
#define C_MIN		0x962e0021L
#define C_PLUS		0x962f0021L
#define C_MINUS		0x96300021L
#define C_MULT		0x96310021L
#define C_DIV		0x96320021L

#define F_EQ		0x96330021L
#define F_NEQ		0x96340021L

#define M_NEG		0x1e350011L

/* frei : 3d - 45 !	*/

#define LCUT		0x02460022L
#define LSELECT		0x02470022L
#define LROTATE		0x02480022L
#define LUNI		0x07490022L
#define LREPLACE	0x034a0032L
#define REVERSE		0x024b0012L
#define LTRANSFORM	0x024c0032L

#define LDIMENSION	0x5f4d0011L
/* frei : 4e - 4f	*/
#define M_SIN		0x1e500011L	/* struck         */
#define M_COS		0x1e510011L
#define M_TAN		0x1e520011L
#define M_LN		0x1e530011L
#define M_EXP		0x1e540011L
/* frei : 55 - ? (mom. dez. 100, s. rear.c !)	*/

#endif

/* Mucha special
#define D_EQELEM	0x0a500021L
#define D_INSELEM	0x0a510021L
#define M_FIRST		0x1e520011L
#define M_REST		0x1e530011L
#define M_SETEMPTY	0x1e540011L
#define D_MEMBER	0x0a550021L
#define D_UNION		0x0a560021L
#define D_INTERSEC	0x0a570021L
#define D_DELELEM	0x0a580021L
*/

#ifdef MEINE_REDUMA
# include "cstmy.h"
#endif

/* end of   cencod.h */
