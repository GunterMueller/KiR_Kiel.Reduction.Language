/*                                            cetab.c   */
/* letzte aenderung:      11.12.88                      */

#define isetptb   1

#include "cswitches.h"
#include "cportab.h"
#include "cedit.h"
#include "cetb.h"

struct etps etptb[] = {
  B_EXPR,	E_EXPR,		"<expr>",		/*  0 */
  L_EXPR,	E_EXPR,		"<expr_list>",		/*  1 */
  B_VAR,	E_VAR,		"<var>",		/*  2 */
  L_VAR,	E_VAR,		"<var_list>",		/*  3 */
  B_COND,	E_COND,		"<cond_expr>",		/*  4 */
  B_LBAR,	E_LBAR,		"<lambdabar_expr>",	/*  5 */
  B_SETEL,	E_SETEL,	"<setelement>",		/*  6 */
  L_SETEL,	E_SETEL,	"<setelem_list>",	/*  7 */
  B_NUMBVAL,	E_NUMBVAL,	"<number>",		/* 10 */
  L_NUMBVAL,	E_NUMBVAL,	"<number_list>",	/* 11 */
  B_BOOLVAL,	E_BOOLVAL,	"<bool>",		/* 12 */
  L_BOOLVAL,	E_BOOLVAL,	"<bool_list>",		/* 13 */
  B_STRVAL,	E_STRVAL,	"<string>",		/* 14 */
  L_STRVAL,	E_STRVAL,	"<string_list>",	/* 15 */
  B_SETOBJ,	E_SETOBJ,	"<set>",		/* 16 */
  L_NMATRL,	E_NMATRL,	"<number_mat_rows>",	/* 17 */
  L_BMATRL,	E_BMATRL,	"<bool_mat_rows>",	/* 20 */
  L_SMATRL,	E_SMATRL,	"<string_mat_rows>",	/* 21 */
  L_NMATEL,	E_NMATEL,	"<number_mat_elements>",	/* 22 */
  L_BMATEL,	E_BMATEL,	"<bool_mat_elements>",	/* 23 */
  L_SMATEL,	E_SMATEL,	"<string_mat_elements>",	/* 24 */
  L_FMATEL,	E_FMATEL,	"<mat_mat_elements>",	/* 25 */
  L_FMATRL,	E_FMATRL,	"<mat_mat_row_list>",	/* 26 */
  B_FDEC,	E_FDEC,		"<fdec>",		/* 27 */
  L_FDEC,	E_FDEC,		"<fdec_list>",		/* 30 */
  B_DECL,	E_DECL,		"<def>",		/* 31 */
  B_FNAM,	E_FNAM,		"<var | fdec>",		/* 32 */
  B_FDEX,	E_FDEX,		"<expr | fdec>",	/* 33 */
  B_CALL,	E_CALL,		"<expr>",		/* 34 */
  B_FCN,	E_FCN,		"<fcall>",		/* 35 */
  B_DCEX,	E_DCEX,		"<expr | def>"		/* 36 */
 ,B_WHEN,	E_WHEN,		"<when>"		/* 37 */
 ,L_WHEN,	E_WHEN,		"<when_list>"		/* 40 */
 ,B_OTHER,	E_OTHER,	"<otherwise | end >"	/* 41 */
 ,B_PATTERN,	E_PATTERN,	"<pattern>"		/* 42 */
 ,L_PATT1,	E_PATT1,	"<pattern_list>"	/* 43 */
 ,B_GUARD,	E_GUARD,	"<guarded_expr>"	/* 44 */
 ,B_DO,		E_DO,		"<do_expr>"		/* 45 */
 ,B_USKON,	E_USKON,	"<expr>"		/* 46 */
 ,B_USKONP,	E_USKONP,	"<pattern>"		/* 47 */
#if N_STR
 ,B_STRPATT,	E_STRPATT,	"<string_pattern>"	/* 50 */
 ,L_STRPATT,	E_STRPATT,	"<str_pattern_list>"	/* 51 */
 ,B_CHAR,	E_CHAR,		"<string>"		/* 52 */
 ,L_CHAR,	E_CHAR,		"<string_elem>"		/* 53 */
 ,B_N_PATT,	E_N_PATT,	"<n_pattern>"		/* 54 */
 ,B_PATT2,	E_PATT2,	"<patt2>"		/* 55 */
 ,L_PATT2,	E_PATT2,	"<patt2_list>"		/* 56 */
 ,B_PATT1,	E_PATT1,	"<pattern1>"		/* 57 */
 ,B_SKIP,	E_SKIP,		"<skips>"		/* 60 */
 ,B_LIST,	E_LIST,		"<list>"		/* 61 */
 ,B_LDEC,	E_LDEC,		"<let>"			/* 62 */
 ,L_LDEC,	E_LDEC,		"<let_list>"		/* 63 */
 ,B_LNAM,	E_LNAM,		"<var | let>"		/* 64 */
#if LET_P
 ,B_PDEC,	E_PDEC,		"<letp>"		/* 65 */
 ,L_PDEC,	E_PDEC,		"<letp_list>"		/* 66 */
 ,B_PNAM,	E_PNAM,		"<pattern | letp>"	/* 67 */
#endif
#if ClausR
 ,B_WHEN_CR,	E_WHEN_CR,		"<when>"		/* 70 */
 ,L_WHEN_CR,	E_WHEN_CR,		"<when_list>"		/* 71 */
 ,B_WHENST_CR,	E_WHENST_CR,		"<entry>"		/* 72 */
 ,L_WHENST_CR,	E_WHENST_CR,		"<entry_list>"		/* 73 */
#endif /* ClausR	*/
 ,B_VARSEL,	E_VARSEL,		"<var | sel>"		/* 74 */
#endif
  };

/* entsprechend etptb aufgebaut (gleiche indizes) !!    */
/* \077 = EOPARSE (s. cinput.c) ende der zeile   */

char *sct[] = {
        "\34\46\077",	/* expr     0 : call    */
        "\34\46\077",	/* expr_lst 1 : call    */
        "\077",		/*  2   */
        "\077",		/*  3   */
        "\077",		/*  4   */
        "\077",		/*  5   */
        "\077",		/*  6   */
        "\077",		/*  7   */
        "\077",		/* 10   */
        "\077",		/* 11   */
        "\077",		/* 12   */
        "\077",		/* 13   */
        "\077",		/* 14   */
        "\077",		/* 15   */
        "\077",		/* 16   */
        "\077",		/* 17   */
        "\077",		/* 20   */
        "\077",		/* 21   */
        "\077",		/* 22   */
        "\077",		/* 23   */
        "\077",		/* 24   */
        "\077",		/* 25   */
        "\077",		/* 26   */
        "\077",		/* fdec	: 27                */
        "\077",		/* fdeclst	: 30                */
        "\077",		/* decl	: 31                */
        "\077",		/* var_fdec	: 32                */
        "\34\46\30\077",	/* expr_fdec	: 33 call,fdeclst   */
        "\077",		/* call	: 34                */
        "\077",		/* fcall	: 35                */
        "\34\46\31\077"	/* expr_decl	: 36 call,decl      */
       ,"\077"		/* when	: 37    */
       ,"\077"		/* whenlst	: 40    */
       ,"\077"		/* other	: 41    */
       ,"\47\077"	/* pattern	: 42    */
       ,"\47\077"	/* pattern_list : 43    */
       ,"\077"		/* guarded_expr : 44    */
       ,"\077"		/* do_expr	: 45    */
       ,"\077"		/* us_kon	: 46    */
       ,"\077"		/* us_konp	: 47    */
#if N_STR
       ,"\077"		/* strpattern	: 50    */
       ,"\077"		/* str_patt_list: 51    */
       ,"\077"		/* char	: 52    */
       ,"\077"		/* string	: 53    */
       ,"\077"		/* n_patt	: 54    */
       ,"\077"		/* n_patt2	: 55    */
       ,"\077"		/* n_patt2_lst	: 56    */
       ,"\47\077"	/* pattern1	: 57    */
       ,"\077"		/* skip	: 60    */
       ,"\077"		/* list	: 61    */
       ,"\077"		/* ldec	: 62    */
       ,"\077"		/* let-list	: 63    */
       ,"\077"		/* lnam	: 64    */
#if LET_P
       ,"\077"		/* pdec	: 65    */
       ,"\077"		/* letp-list	: 66    */
       ,"\42\47\077"		/* pnam	: 67    */
#endif
#if ClausR
       ,"\077"		/* when	: 70    */
       ,"\077"		/* whenlst	: 71    */
       ,"\077"		/* when	: 72    */
       ,"\077"		/* whenlst	: 73    */
#endif /* ClausR	*/
       ,"\077"		/* var | sel	: 74    */
#endif
};
/* nur noch ein eintrag ?, ev. loeschen und manuell loesen      */
char *sct0[] = {
        "\077",		/*  0   */
        "\077",		/*  1   */
        "\077",		/*  2   */
        "\077",		/*  3   */
        "\077",		/*  4   */
        "\077",		/*  5   */
        "\077",		/*  6   */
        "\077",		/*  7   */
        "\077",		/* 10   */
        "\077",		/* 11   */
        "\077",		/* 12   */
        "\077",		/* 13   */
        "\077",		/* 14   */
        "\077",		/* 15   */
        "\077",		/* 16   */
        "\077",		/* 17   */
        "\077",		/* 20   */
        "\077",		/* 21   */
        "\077",		/* 22   */
        "\077",		/* 23   */
        "\077",		/* 24   */
        "\077",		/* 25   */
        "\077",		/* 26   */
        "\077",		/* fdec	: 27    */
        "\077",		/* fdeclst	: 30    */
        "\077",		/* decl	: 31    */
        "\30\077",	/* var_fdec	: 32    */
        "\077",		/* expr_fdec	: 33    */
        "\077",		/* call	: 34    */
        "\077",		/* fcn	: 35    */
        "\077"		/* expr_decl	: 36    */
       ,"\077"		/* when	: 37    */
       ,"\077"		/* whenlst	: 40    */
       ,"\077"		/* other	: 41    */
       ,"\077"		/* pattern	: 42    */
       ,"\077"		/* pattern_list : 43    */
       ,"\077"		/* guarded_expr : 44    */
       ,"\077"		/* do_expr	: 45    */
       ,"\077"		/* us_kon	: 46    */
       ,"\077"		/* us_konp	: 47    */
#if N_STR
       ,"\077"		/* strpattern	: 50    */
       ,"\077"		/* str_patt_list: 51    */
       ,"\077"		/* char	: 52    */
       ,"\077"		/* string	: 53    */
       ,"\077"		/* n_patt	: 54    */
       ,"\077"		/* patt2	: 55    */
       ,"\077"		/* patt2_lst	: 56    */
       ,"\077"		/* pattern1	: 57    */
       ,"\077"		/* skip	: 60    */
       ,"\077"		/* list	: 61    */
       ,"\077"		/* ldec	: 62    */
       ,"\077"		/* let-list	: 63    */
       ,"\63\077"		/* lnam	: 64    */
#if LET_P
       ,"\077"		/* pdec	: 65    */
       ,"\077"		/* letp-list	: 66    */
       ,"\66\077"		/* pnam	: 67    */
#endif
#if ClausR
       ,"\077"		/* when	: 70    */
       ,"\077"		/* whenlst	: 71    */
       ,"\077"		/* when	: 72    */
       ,"\077"		/* whenlst	: 73    */
#endif /* ClausR	*/
       ,"\077"		/* var | sel	: 74    */
#endif
};
int newetb[] = {
 0	/* 0 */
,1	/* 1 */
,2	/* 2 */
,3	/* 3 */
,4	/* 4 */
,5	/* 5 */
,6	/* 6 */
,7	/* 7 */
,8	/* 10 */
,9	/* 11 */
,10	/* 12 */
,11	/* 13 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,12	/* 14 */
,13	/* 15 */
,14	/* 16 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,15	/* 17 */
,16	/* 20 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,17	/* 21 */
,18	/* 22 */
,19	/* 23 */
,(-1)	/* 377 */
,(-1)	/* 377 */
,20	/* 24 */
,21	/* 25 */
,22	/* 26 */
,23	/* 27 */
,24	/* 30 */
,25	/* 31 */
,26	/* 32 */
,27	/* 33 */
,28	/* 34 */
,29	/* 35 */
,30	/* 36 */
,31	/* 37 */
,32	/* 40 */
,33	/* 41 */
,34	/* 42 */
,35	/* 43 */
,36	/* 44 */
,37	/* 45 */
,38	/* 46 */
,39	/* 47 */
};
/* end of      cetab.c */

