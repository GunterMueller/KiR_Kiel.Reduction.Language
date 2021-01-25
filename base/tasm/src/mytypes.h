#ifndef _MYTYPES
#define _MYTYPES

#include <stdio.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define MAXDELTAARGS    4
#define ALLTAGS         0xf

#define MKINT(x)        (((x)<<1) | 1)
#define GETINT(x)       ((x)>>1)
#define ISINT(x)        ((x) & 1)

/* Interaktion Tags, stt 13.02.96 */
#define IATAG           0x2
#define MKIA(x)         (((x)<<4) | IATAG)
#define GETIA(x)        ((x)>>4)
#define ISIA(x)         (((x) & ALLTAGS) == IATAG)

#define BOOLTAG         0x4
#define MKBOOL(x)       (((x)<<4) | BOOLTAG)
#define GETBOOL(x)      ((x)>>4)
#define ISBOOL(x)       (((x) & ALLTAGS) == BOOLTAG)

/* War gedacht als Differenzierung zu PRFTAG, wurde aber synonym verwendet.
   Deshalb wurden die drei Vorkommen von ISFUNC durch ISPRF ersetzt und
   FUNCTAG auskommentiert.
 #define FUNCTAG         PRFTAG
 #define MKFUNC(x)       (((x)<<4) | FUNCTAG)
 #define GETFUNC(x)      ((x)>>4)
 #define ISFUNC(x)       (((x) & ALLTAGS) == FUNCTAG)
*/

#define PRFTAG          0x8
#define MKPRF(x)        ((x << 4) | PRFTAG)
#define GETPRF(x)       (x >> 4)
#define ISPRF(x)        (((x) & ALLTAGS) == PRFTAG)

#define CHARTAG         0xc
#define MKCHAR(x)       (((x)<<16) | CHARTAG)
#define GETCHAR(x)      ((x)>>16)
#define ISCHAR(x)       (((x) & ALLTAGS) == CHARTAG)

#define MKPTR(x)        (x)
#define GETPTR(x)       (x)
#define ISPTR(x)        (((x) & ALLTAGS) == 0)

#define GEORG_GENJUMP 1
#define GEORG_GENRTF  2

#define ASM_SK        0x0
#define ASM_TILDE     0x1
#define ASM_DIST      0x2

#define ISASM_SK()      (asm_mode == ASM_SK) 
#define ISASM_TILDE()   (asm_mode & ASM_TILDE)
#define ISASM_DIST()    (asm_mode & ASM_DIST)

/************************************************************************/
/*  ACHTUNG: alle kleingeschrieben ausser Gamma                         */
/*  code_ok ist ein tag, das anzeigt, dass der anhaengenede Kram bereits*/
/*  C-Code ist und nicht mehr angefasst zu werden braucht               */
/************************************************************************/

typedef enum {
               append=15, apply, beginzf, beta,
               body, chkframe, cons, count, delta1,
               delta2, delta3, delta4, dimension, dist, distend, distb,
               dropp, duparg, end, endzf, ext, extract, fcons, first,
               freea, freer, freeswt, freet, freew,
               Gamma, gammabeta, guard, intact,
               jcond, jfalse, jtrue, jcond2, jfalse2, jtrue2,
               jump, label, le, lsel, makebool, makezflist, mcomrebon, 
               mcomreeon, mcomsebon, mcomseeon, mdescallon,
               mdescfreon, mhfreeon, mhpallon, mhpcmpon, mkap, mkbclos,
               mkcclos, mkdclos, mkgaclos, mkgclos, mkgsclos, mkiclos, mkilist,
               mklist, mksclos, mod, movear, moveaw, movetr,
               movetw, mprocreon, mprorunon, mproteron, mprowakon, mprsleeon,
               msdistck, msdistend, msdiston, mstckppon, mstckpuon, mstksegal,
               mstksegfr, msnodist, nomatch,
               poph, pushar, pushaw, pushr, pushr_p, pushtr, pushtw, 
               pushw, pushcw_pf, pushw_p, pushcw_i, pushcw_b, pushcr_i, 
               pushcr_b, pushcw_r, pushaux,
               pushh, pushret, rest, rtc, rtc_b, rtc_i, rtc_pf,
               i_rtc_b, i_rtc_i , i_rtf, i_rtt, i_rtp, i_rtm, i_rtc_pf, stflip,
               rtf, rtm, rtm_t, rtp, rtt, snap, tail,
               testfalse, testlist, testtrue, testzero, wait, when,
               code_ok, betanear, betafar,
               tailnear, tailfar, gammanear, gammafar, 
               gammabetanear, gammabetafar,
               incw, inca, incr, inct, tincw, tinca, tincr, tinct,
               decw, deca, decr, dect, tdecw, tdeca, tdecr, tdect,
               killw, killa, killr, killt, tkillw, tkilla, tkillr, tkillt,
               hashargs, hashtildeargs, hashrestype, hashsetref,
               advance, atend, atstart, bind, binds, bindsubl, Case, 
	       casenear, casefar, dereference,
               drop, endlist, endsubl, fetch, gammacase, 
	       gammacasenear, gammacasefar, initbt, matcharb,
               matcharbs, matchbool, matchin, matchint, matchlist,
               matchprim, matchstr, mkaframe, mkbtframe, mkcase, mkwframe,
               nestlist, pick, restorebt, restoreptr, rmbtframe, rmwframe,
               savebt, saveptr, startsubl, tguard, mkframe, mkslot, Inter,
               uses_aux_var, stack_op, ris_stack_op, lastcmd
             } COMMAND;

typedef enum {
    LABbt, LABcase, LABcond, LABcont, LABfail, LABfunc,
    LABinloop, LABjcond, LABundecided, LABdistarg, LABdistend,
    LABonefunc } labeltype;

typedef enum
    { SREFnone, SREFimplicit, SREFexplicit, SREFkeepimpl, SREFisexplicit } setrefmode;

/************************************************************************/
/* IMPORTANT:	D O   N O T   M I X !	T H E Y   A R E   S O R T E D !	*/
/************************************************************************/
typedef enum {
/*********************/
/* DYADIC FUNCTIONS  */
/*********************/
               p_plus, p_minus, p_mult, p_div, p_mod,
               p_or, p_and, p_xor, p_eq, p_ne, 
               p_lt, p_le, p_gt, p_ge, p_max, p_min, p_quot, 
/*********************/
/* MONADIC FUNCTIONS */
/*********************/
               p_not, p_abs, p_neg, p_trunc, p_floor, p_ceil,
               p_frac, p_empty, p_sin, p_cos, p_tan, p_ln, p_exp, 
/*****************/
/* INNER PRODUCT */
/*****************/
               p_ip,  
/***********/
/* QUERIES */
/***********/
               p_class, p_type, p_dim, p_vdim, p_mdim, p_ldim,
/***********/
/* GROUP 1 */
/***********/
               p_transpose, p_reverse, 
/***********/
/* GROUP 2 */
/***********/
               p_cut, p_mcut, p_vcut, p_ltransform, p_rotate, p_vrotate, p_mrotate,
               p_select, p_vselect, p_mselect, p_substr, p_lcut, p_transform, 
               p_lrotate, p_lselect, 
/***********/
/* GROUP 3 */
/***********/
               p_lreplace, p_repstr, p_replace, p_mre_c, p_mre_r, p_vreplace, 
               p_mreplace, 
/***********/
/* GROUP 7 */
/***********/
               p_unite, p_vunite, p_munite, p_lunite, 
/************************/
/* CONVERSION FUNCTIONS */
/************************/
               p_to_vect, p_to_tvect, p_to_mat, p_to_scal, p_to_list, p_to_field,
/************************/
/* ALONG A COORDINATE   */
/************************/
               p_c_max, p_vc_max, p_c_min, p_vc_min, p_c_plus, p_vc_plus, 
               p_c_minus, p_vc_minus, p_c_mult, p_vc_mult, p_c_div, p_vc_div,
/************************/
/* FIELD (NOT) EQUAL    */
/************************/
               p_f_eq, p_f_ne,
/* cr 22/03/95, kir(ff), START */
/************************/
/* FRAME FUNCTIONS      */
/************************/
               p_f_update, p_f_select, p_f_test, p_f_delete, p_f_slots,
/* cr 22/03/95, kir(ff), END */
               p_sprintf, p_sscanf, p_to_char, p_to_ord
               } PRIMF;


/* stt 13.02.96 */
/* The elements of IACTION are used as indexes for intact_tab[] in globals.h. */
/* Make sure that IACTION and intact_tab[] match each other.                  */
typedef enum {
               ia_fopen, ia_fclose, ia_fgetc, ia_fputc, ia_ungetc,
               ia_fgets, ia_read, ia_fputs, ia_fprintf, ia_fredirect,
               ia_fseek, ia_ftell, ia_exit, ia_eof, ia_bind, ia_unit,
               ia_finfo, ia_status, ia_remove, ia_rename, ia_copy,
               ia_mkdir, ia_rmdir, ia_chdir, ia_getwd, ia_dir,
               ia_get, ia_put
             } IACTION;


/* type for interaction nodes
 */

typedef  enum { INdone=0, INget, INput
              } INTERT;

/************************/
/* ALL  PURPOSE   TYPES */
/************************/
#define PRIM_MASK		0x0f
#define EXT_MASK		0x70
#define TYPE_MASK		0xff
#define OTHER_MASK		0x80
#define CALC_EXT(x)		((x)>>4)
#define DO_EXT(x)		((x)<<4)
#define TYPES2LONG(w,x,y,z)	((w)+((x) << 8)+((y) << 16)+((z) << 24))
#define NOTYPE                  TYPES2LONG(tp_none, tp_none, tp_none, tp_none)
#define MAKETYPE(ext, prim)     ((ext) | (prim))
#define MERGETYPE(ty1, ty2)     ((ty1) | (ty2))
#define DELTYPE(i, ty)          (ty & ~(TYPE_MASK<<(i*8)))
#define GETTYPE(i, ty)          (((ty)>>(i*8)) & TYPE_MASK)
#define SETTYPE(i, ty)          ((ty)<<(i*8))

/* list of type list is typed as list of type none */
/* DO NOT CHANGE THE ORDER OF TP_TYPES */
/* if you must change something, look at loctypes.h, too */
typedef enum
             {
             tp_nums=-100,tp_vals,tp_bools,tp_scals,tp_nscals,tp_vects,
             tp_nvects,tp_mats,tp_nmats,
	     tp_badrecur=-3,tp_returnadr,tp_illegal,
	     tp_none=0x00,tp_int,tp_real,tp_digit,tp_bool,tp_char,
             tp_empty,tp_atom,tp_var,
	     tp_list=0x10,tp_vect=0x20,tp_tvect=0x30,tp_matrix=0x40,
             tp_string=0x50,
             tp_prim=OTHER_MASK,tp_clos,tp_func
	     } TP_TYPES;


typedef enum {
               d_list, d_real, d_ptr, d_mat, d_vect, d_tvect, d_string, d_name,
               d_int, d_bool, d_undef
             } DATATAG;


typedef enum
             {
               DT_PRIMITIV, DT_CONDITIONAL, DT_COMBINATOR, DT_CASE
             } DESCTAG;


typedef struct tagc_code
                             {
                               char *code;
                               struct tagc_code *next;
                             } C_CODE ;


typedef struct tagprogram
                             {
                               struct tagfunction *function;
                               struct tagfundesc *desc;
                               struct tagdatanode *data;
                             } PROGRAM;

/********************************************************************/
/* Datentypen ohne Sinn und Verstand                                */
/********************************************************************/
typedef struct tagdatanode {
                               struct tagdatanode *next;
                               DATATAG tag;
                               int address;
                               union 
                               {
                                 double x;
                                 struct {
                                   int   size;
                                   int * data;
                                   } v;
                                 struct {
                                   int rows;
                                   int cols;
                                   DATATAG tag;
                                   union {
                                     int *idata;
                                     double *rdata;
                                     } m;
                                   } w;
                               } u; 
                             } DATANODE;

/********************************************************************/
/* ENDE Datentypen ohne Sinn und Verstand                           */
/********************************************************************/

typedef struct tagparameters {
                              int n; /* returnlabel for calls */
                              int m; 
                              int k;
                              int l; /* only used by bindsubl/initbt/dist */
                              int j; /* only used by initbt/dist */
                              double x;
                              char * label; /* bei farcalls targetfunktion */
                              char * ret; /* bei farcalls returnfunktion */
                              char *hash_str;
			      TP_TYPES *argtp;	/* argtypes for hash */
                              void *desc;   /* pointer here */
                              PRIMF primf;
                            } PARAMETERS;

typedef struct tagfunction  {
                              char * name;
                              struct tagfunction *next;
                              struct tagorder *order;
                              struct tagfundesc *desc;
                              int inlined;
                            } FUNCTION;

typedef struct tagorder     { 
                              COMMAND command;
                              PARAMETERS args;
                              struct tagorder *next;
                              struct tagorder *prev;
                              char *code;
			      int typestouched;
                              long types;
                              int branch;
                              int opti; 	/* FALSE if not optimized */
                            } ORDER;

typedef struct tagfundesc
                            {
                              int address;       /* first elem of desc */
                              DESCTAG tag;
                              int nfv;          /* # free vars */
                              int nv;           /* arity */
                              int graph;
                              char * label;
                              struct tagfundesc *next;
                            } FUNDESC;

typedef struct tagcode
                            {
                              char code[120];
                              struct tagcode *next;
                            } CODE;

extern int asm_mode;
extern int uses_apply;
extern int dogeorg, dotis,  doris, dotasm, dogcc, dostaticlink, useacc;
extern char * infile, * outfile, * tmpname;
extern FILE * yyin, * yyout;
extern PROGRAM * program_start;
struct tagfunc_body;
struct tagfunction;
struct tagorder;
struct tagfundesc;
struct tagcode;


#endif
