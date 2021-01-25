%{
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include "mytypes.h"
#include "support.h"
#include "buildtree.h"
#include "error.h"

extern FILE *yyin, 
            *yyout;

int yylex();
void yyunput();

FUNCTION * first_function = NULL;
PROGRAM * program_start;
FUNCTION ** pfuns;
FUNDESC ** pdescs;
DATANODE ** pdata;
int * iptr;
double * rptr;

char act_func[20]="";
char order_str[128]="";
unsigned int label_count = 1;
int i, j, outfunc;

#define DISTSTACKSIZE 16
int diststacktop;
struct { int tag;
ORDER * order; }
diststack[DISTSTACKSIZE];

%}

%union {
         int intv;
         double realv;
         int * data;
         char * func_lab;
         FUNDESC *fundesc;
         FUNCTION *function;
         ORDER *order_list;
         TP_TYPES * typelist;
         int types;
         struct {
           PRIMF prf;
           int   types;
           } primf;
         char *helpstring;
       }

%type  <order_list> stmt_list stmt param_stmt asis_stmt base_stmt hash_stmt dist_cmd
                    func_end start_desc distcom
%type  <order_list> func_call data string
%type  <function>  func_list func
%type  <primf> prf1 prf2 prf3 prf4 interact
%type  <intv>  elem inter_type selem 
%type  <typelist> onetype types
%type  <data> list slist 
%token <intv> INTV HEX_NUM SELEM
%token <realv> REALV 
%token <func_lab> FUNC_LABEL COND_LABEL GOAL_LABEL JCOND_LABEL CONT_LABEL
%token <func_lab> CASE_LABEL FAIL_LABEL UND_LABEL BT_LABEL INLOOP_LABEL
%token <func_lab> DISTARG_LABEL DISTEND_LABEL
%token <helpstring> CLIST

%token D_REAL, D_VECT, D_TVECT, D_MAT D_LIST D_STRING D_NAME
%token INT REAL BOOL DIGIT CHAR STRING NONE TY_BO TY_RE TY_INT TY_UN
%token SEMICOLON COLON PTD TRUE_Y FALSE_Y
%token LBRACKET RBRACKET C_LBRACK C_RBRACK LBRACK RBRACK LQUOTE RQUOTE
%token FUN_DESC D_FUNC D_COMB D_CONDI D_CASE
%token RTT RTF RTC RTP RTM EXIT END MOVEAW MOVEAR MOVETW MOVETR PUSHAW0 TAIL
%token SNAP APPLY FREE1R FREER FREEA FREE1A FREESWT FREET FREEW PUSHAW PUSHTW 
%token PUSHAR PUSHTR PUSHCW
%token FREE1SWT PUSHAW0 PUSHTW0 PUSHAR0 PUSHTR0 PUSHRET PUSHAUX
%token MKDCLOS MKICLOS MKCCLOS MKSCLOS MKBCLOS MKGSCLOS MKGACLOS MKLIST MKILIST
%token PUSHW PUSHR
%token JUMP MKAP
%token DELTA1 DELTA2 DELTA3 DELTA4 INTACT BETA BETA0UPDATE GAMMA GAMMABETA
%token INCA INCR INCT INCW TINCA TINCR TINCT TINCW
%token DECA DECR DECT DECW TDECA TDECR TDECT TDECW 
%token KILLA KILLR KILLT KILLW TKILLA TKILLR TKILLT TKILLW 
%token ADVANCE ATEND ATSTART BACKUP BIND BINDS BINDSUBL CASE DEREFERENCE
%token DROP ENDLIST ENDSUBL FETCH GAMMACASE INITBT
%token MKAFRAME MKBTFRAME MKCASE MKWFRAME NESTLIST PICK RESTOREBT RESTOREPTR
%token RMBTFRAME RMWFRAME SAVEBT SAVEPTR STARTSUBL TGUARD
%token MKFRAME MKSLOT INTER
%token PUSHH COUNT DIST DISTB DISTEND WAIT POPH MSDISTEND MSNODIST
%token HASHARGS HASHTILDEARGS HASHRESTYPE HASHSETREF HASHLOCATION HASHUPDATE

%token <types> MATCHARB MATCHARBS
%token <types> MATCHBOOL MATCHC MATCHFALSE MATCHIN MATCHINT MATCHLIST
%token <types> MATCHNIL MATCHNILS MATCHPRIM MATCHSTR MATCHTRUE

%token <primf> JFALSE JTRUE JCOND JFALSE2 JTRUE2 JCOND2
%token <primf> ABS NEG NOT EXP LN SIN COS TAN FLOOR CEIL FRAC TRUNC VC_PLUS
%token <primf> VC_MINUS VC_MULT VC_DIV VC_MIN VC_MAX
%token <primf> LDIM VDIM MDIM CLASS TYPE EMPTY
%token <primf> TRANSPOSE REVERSE TO_LIST TO_SCAL TO_VECT TO_TVECT TO_MAT
%token <primf> PLUS MINUS MULT QUOT DIV MOD IP MAX MIN AND OR XOR EQ NE
%token <primf> C_PLUS C_MINUS C_MULT C_DIV C_MAX C_MIN
%token <primf> F_EQ F_NE GE GT LE LT LSELECT LCUT LROTATE LUNITE LREPLACE
%token <primf> VSELECT VCUT VROTATE VUNITE MSELECT MCUT MROTATE MUNITE
%token <primf> MRE_R MRE_C MREPLACE VREPLACE
%token <primf> LTRANSFORM REPSTR TRANSFORM
%token <primf> SPRINTF SSCANF TO_CHAR TO_ORD  /* stt, 09.05.96 */
/* cr 22/03/95, kir(ff), START */
%token <primf> FSELECT UPDATE TEST DELETE SLOTS
/* cr 22/03/95, kir(ff), END */
%token DONE GET PUT IMPLICIT EXPLICIT KEEPIMPL

%token <primf> IA_FOPEN IA_FCLOSE IA_FGETC IA_FPUTC IA_UNGETC IA_FGETS
%token <primf> IA_READ IA_FPUTS IA_FPRINTF IA_FREDIRECT IA_FSEEK IA_FTELL
%token <primf> IA_EXIT IA_EOF IA_BIND IA_UNIT IA_FINFO IA_STATUS
%token <primf> IA_REMOVE IA_RENAME IA_COPY IA_MKDIR IA_RMDIR IA_CHDIR
%token <primf> IA_GETWD IA_DIR IA_GET IA_PUT

%start asm_prog

%%


asm_prog:     { program_start=conc_desc_fun(NULL, NULL, NULL);
                pfuns=&program_start->function;
                pdescs=&program_start->desc;
                pdata=&program_start->data;
                diststacktop = -1;
              }
             func_list
              { *pfuns=$2; }
           ;

func_list:    func_list func
               {
               if ($1 && $2)
                 $$=conc_function($1, $2);
               else
                 $$=($1 ? $1 : $2);
               }
           | func
               {
                 $$=$1;
               }
           ;

func:        FUNC_LABEL COLON { outfunc = 0; } stmt_list func_end
               {
                 $$=new_function($1, conc_order($4, $5));
                 free($1);
               }
           | GOAL_LABEL COLON { outfunc = 0; } stmt_list func_end
               {
                 $$=new_function("goal", conc_order($4, $5));
                 free($1);
               }
           | JCOND_LABEL COLON { outfunc = 1; } stmt_list func_end
               {
                 char * labelv;
                 /* generate label jcond_<adr> => func_<adr> */
                 if ((labelv = (char *)malloc(strlen($1))) == NULL)
                   yyfail("memory allocation failure");
                 strcpy(labelv, "func"); strcpy(&labelv[4], &($1[5]));
                 $$=new_function(labelv, conc_order(conc_order(new_order(label, LABjcond, $1), $4), $5));
                 free($1); free(labelv);
               }
           | CASE_LABEL COLON { outfunc = 0; } stmt_list func_end
               {
                 $$=new_function($1, conc_order($4, $5));
                 free($1);
               }
           | start_desc { $$=NULL; }
           | data { $$ = NULL; }
           ;

func_end:    END LBRACKET RBRACKET SEMICOLON
                {
                  $$=new_order(end);
                }
           ;

stmt_list:   stmt_list stmt
                {
                  if ($1 && $2)
                    $$=conc_order($1, $2);
                  else
                    $$=($1 ? $1 : $2);
                }
           | stmt
                {
                  $$=$1;
                }
           ;

stmt:        param_stmt { $$=$1;}
           | asis_stmt  { $$=$1;}
           | base_stmt  { $$=$1;}
           | dist_cmd   { $$=$1;}
           | func_call  { $$=$1;}
           | data       { $$=NULL;}
           | hash_stmt  { $$=$1;}
           | start_desc { $$=NULL;}
           | COND_LABEL COLON { $$=new_order(label, LABcond, $1); free($1);}
           | JCOND_LABEL COLON { $$=new_order(label, LABjcond, $1); free($1);}
           | CONT_LABEL COLON { $$=new_order(label, LABcont, $1); free($1);}
           | FAIL_LABEL COLON { asm_mode |= ASM_TILDE; 
                                $$=new_order(label, LABfail, $1); free($1);}
           | UND_LABEL COLON { asm_mode |= ASM_TILDE; 
                               $$=new_order(label, LABundecided, $1); free($1);}
           | BT_LABEL COLON { asm_mode |= ASM_TILDE; 
                              $$=new_order(label, LABbt, $1); free($1);}
           | INLOOP_LABEL COLON { asm_mode |= ASM_DIST; 
                                  $$=new_order(label, LABinloop, $1); free($1);}
           | DISTARG_LABEL COLON { asm_mode |= ASM_DIST; 
                                  $$=new_order(label, LABdistarg, $1); free($1);}
           | DISTEND_LABEL COLON { asm_mode |= ASM_DIST; 
                                  $$=new_order(label, LABdistend, $1); free($1);}
           ;

start_desc: FUN_DESC LBRACKET HEX_NUM ',' D_FUNC ',' D_COMB ',' INTV ',' INTV ','
             HEX_NUM ',' FUNC_LABEL RBRACKET SEMICOLON
               {
               *pdescs=conc_desc(new_desc(DT_COMBINATOR, $3, $9, $11, $13, $15), NULL);
               pdescs=&((*pdescs)->next);
               free($15);
               $$=NULL;
               } 
           | FUN_DESC LBRACKET HEX_NUM ',' D_FUNC ',' D_CASE ',' INTV ',' INTV ','
             HEX_NUM ',' CASE_LABEL RBRACKET SEMICOLON
               {
               asm_mode |= ASM_TILDE; 
               *pdescs=conc_desc(new_desc(DT_CASE, $3, $9, $11, $13, $15), NULL);
               pdescs=&((*pdescs)->next);
               free($15);
               $$=NULL;
               }
           | FUN_DESC LBRACKET HEX_NUM ',' D_FUNC ',' D_CONDI ',' INTV ',' INTV ','
             HEX_NUM ',' JCOND_LABEL RBRACKET SEMICOLON
               {
               char * labelv;
               if ((labelv = (char *)malloc(strlen($15))) == NULL)
                 yyfail("memory allocation failure");
               if (outfunc) {
                 /* generate label jcond_<adr> => func_<adr> */
                 strcpy(labelv, "func"); strcpy(&labelv[4], &($15[5]));
                 *pdescs=conc_desc(new_desc(DT_COMBINATOR, 
                                   (int)$3+1, $9>>2, $11-1, $13, labelv), NULL);
                 pdescs=&((*pdescs)->next);
                 }
               *pdescs=conc_desc(new_desc(DT_CONDITIONAL, 
                                 (int)$3, $9, $11, $13, $15), NULL);
               pdescs=&((*pdescs)->next);
               free($15); free(labelv);
               $$=NULL;
               }
           ;

data:         D_REAL LBRACKET HEX_NUM ',' REALV RBRACKET SEMICOLON
               { *pdata = conc_data(new_data(d_real, $3, $5), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_LIST LBRACKET HEX_NUM ',' INTV ',' list RBRACKET SEMICOLON
               { int * p;
                 if ($5 != $7[0])
                   yyerror("element count mismatch, %d elements supplied", $7[0]);
                 p = (int *)malloc($7[0]*sizeof(void *));
                 memcpy(p, &$7[1], $7[0]*sizeof(void *));
                 free((void *)$7);
                 *pdata = conc_data(new_data(d_list, $3, $5, p), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_LIST LBRACKET HEX_NUM ',' INTV RBRACKET SEMICOLON
               { 
                 if ($5)
                   yyerror("no data supplied");
                 *pdata = conc_data(new_data(d_list, $3, $5, NULL), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | string SEMICOLON
               { $$ = NULL; }
           | D_NAME LBRACKET HEX_NUM ',' INTV ',' CLIST RBRACKET SEMICOLON
               { *pdata = conc_data(new_data(d_name, $3, $5, $7), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_VECT LBRACKET HEX_NUM ',' TY_BO ',' INTV ',' INTV
               { iptr = malloc( $7 * $9 * sizeof(int));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); iptr[i] = yylval.intv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_vect, $3, $7, $9, iptr, d_bool), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_VECT LBRACKET HEX_NUM ',' TY_RE ',' INTV ',' INTV
               { rptr = malloc( $7 * $9 * sizeof(double));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); rptr[i] = yylval.realv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_vect, $3, $7, $9, rptr, d_real), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_VECT LBRACKET HEX_NUM ',' TY_INT ',' INTV ',' INTV
               { iptr = malloc( $7 * $9 * sizeof(int));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); iptr[i] = yylval.intv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_vect, $3, $7, $9, iptr, d_int), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_TVECT LBRACKET HEX_NUM ',' TY_UN ',' INTV ',' INTV RBRACKET SEMICOLON
               { *pdata = conc_data(new_data(d_tvect, $3, $7, $9, NULL, d_undef), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_VECT LBRACKET HEX_NUM ',' TY_UN ',' INTV ',' INTV RBRACKET SEMICOLON
               { *pdata = conc_data(new_data(d_vect, $3, $7, $9, NULL, d_undef), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_MAT LBRACKET HEX_NUM ',' TY_UN ',' INTV ',' INTV RBRACKET SEMICOLON
               { *pdata = conc_data(new_data(d_mat, $3, $7, $9, NULL, d_undef), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           
           | D_TVECT LBRACKET HEX_NUM ',' TY_BO ',' INTV ',' INTV
               { iptr = malloc( $7 * $9 * sizeof(int));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); iptr[i] = yylval.intv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_tvect, $3, $7, $9, iptr, d_bool), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_TVECT LBRACKET HEX_NUM ',' TY_RE ',' INTV ',' INTV
               { rptr = malloc( $7 * $9 * sizeof(double));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); rptr[i] = yylval.realv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_tvect, $3, $7, $9, rptr, d_real), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_TVECT LBRACKET HEX_NUM ',' TY_INT ',' INTV ',' INTV
               { iptr = malloc( $7 * $9 * sizeof(int));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); iptr[i] = yylval.intv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_tvect, $3, $7, $9, iptr, d_int), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_MAT LBRACKET HEX_NUM ',' TY_BO ',' INTV ',' INTV
               { iptr = malloc( $7 * $9 * sizeof(int));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); iptr[i] = yylval.intv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_mat, $3, $7, $9, iptr, d_bool), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_MAT LBRACKET HEX_NUM ',' TY_RE ',' INTV ',' INTV
               { rptr = malloc( $7 * $9 * sizeof(double));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); rptr[i] = yylval.realv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_mat, $3, $7, $9, rptr, d_real), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           | D_MAT LBRACKET HEX_NUM ',' TY_INT ',' INTV ',' INTV  
               { iptr = malloc( $7 * $9 * sizeof(int));
                 for (i  = 0; i < $7 * $9; i++)
                 {yylex(); yylex(); iptr[i] = yylval.intv;}
               }
             RBRACKET SEMICOLON
               {
                 *pdata = conc_data(new_data(d_mat, $3, $7, $9, iptr, d_int), NULL);
                 pdata = &((* pdata)->next);
                 $$ = NULL;
               }
           ;

list:        list ',' elem
               { $$ = (int *)malloc((2+$1[0])*sizeof(void *));
                 memcpy($$, $1, ($1[0]+1)*sizeof(void *));
                 $$[0]++;
                 $$[$$[0]] = $3;
                 free((void *)$1);
               }
           | elem
               { $$ = (int *)malloc(2*sizeof(void *));
                 $$[0] = 1;
                 $$[1] = $1;
               }
       ;

elem:        INT LBRACKET INTV RBRACKET
               { $$ = MKINT($3);
               }
           | BOOL LBRACKET TRUE_Y RBRACKET
               { $$ = MKBOOL(TRUE);
               }
           | BOOL LBRACKET FALSE_Y RBRACKET
               { $$ = MKBOOL(FALSE);
               }
           | PTD LBRACKET HEX_NUM RBRACKET
               { $$ = (int)MKPTR($3);
               }
           | D_REAL LBRACKET HEX_NUM ',' REALV RBRACKET
               { *pdata = conc_data(new_data(d_real, $3, $5), NULL);
                 pdata = &((* pdata)->next);
                 $$ = (int)MKPTR($3);
               }
           | D_LIST LBRACKET HEX_NUM ',' INTV ',' list RBRACKET
               { int * p;
                 p = (int *)malloc($7[0]*sizeof(void *));
                 memcpy(p, &$7[1], $7[0]*sizeof(void *));
                 free((void *)$7);
                 *pdata = conc_data(new_data(d_list, $3, $5, p), NULL);
                 pdata = &((* pdata)->next);
                 $$ = (int)MKPTR($3);
               }
           | D_NAME LBRACKET HEX_NUM ',' INTV ',' CLIST RBRACKET
               { *pdata = conc_data(new_data(d_name, $3, $5, $7), NULL);
                 pdata = &((* pdata)->next);
                 $$ = (int)MKPTR($3);
               }
           | D_LIST LBRACKET HEX_NUM ',' INTV RBRACKET
               { *pdata = conc_data(new_data(d_list, $3, $5, NULL), NULL);
                 pdata = &((* pdata)->next);
                 $$ = (int)MKPTR($3);
               }
           | string
               { $$ = (int)MKPTR($1); }
           | prf1
               { $$ = MKPRF($1.prf); }
           | prf2
               { $$ = MKPRF($1.prf); }
           | prf3
               { $$ = MKPRF($1.prf); }
           | prf4
               { $$ = MKPRF($1.prf); }
           | interact
               { $$ = MKIA($1.prf); }
           ;

string:      D_STRING LBRACKET HEX_NUM ',' INTV ',' slist RBRACKET
               { int *p;
                 if ($5 != $7[0])
                   yyerror("element count mismatch, %d elements supplied", $7[0]);
                 p = (int *)malloc($7[0]*sizeof(void *));
                 memcpy(p, &$7[1], $7[0]*sizeof(void *));
                 free((void *)$7);
                 *pdata = conc_data(new_data(d_string, $3, $5, p), NULL);
                 pdata = &((* pdata)->next);
                 $$ = (ORDER *)$3;
               }
           | D_STRING LBRACKET HEX_NUM ',' INTV RBRACKET
               { 
                 if ($5)
                   yyerror("no data supplied");
                 *pdata = conc_data(new_data(d_string, $3, $5, NULL), NULL);
                 pdata = &((* pdata)->next);
                 $$ = (ORDER *)$3;
               }
               ;

slist:        slist ',' selem
               { $$ = (int *)malloc((2+$1[0])*sizeof(void *));
                 memcpy($$, $1, ($1[0]+1)*sizeof(void *));
                 $$[0]++;
                 $$[$$[0]] = $3;
                 free((void *)$1);
               }
           | selem
               { $$ = (int *)malloc(2*sizeof(void *));
                 $$[0] = 1;
                 $$[1] = $1;
               }
       ;

selem:       SELEM { $$=MKCHAR($1); }
           | PTD LBRACKET HEX_NUM RBRACKET { $$ = (int)MKPTR($3); }
           | string { $$=(int)MKPTR($1); }
           ;

hash_stmt:   HASHARGS types
               { void * p = malloc($2[0]*sizeof(TP_TYPES));
                 memcpy(p, &$2[1], $2[0]*sizeof(TP_TYPES));
                 $$=new_order(hashargs,  $2[0], p);
                 free($2); }
           | HASHTILDEARGS types
               { void * p = malloc($2[0]*sizeof(TP_TYPES));
                 memcpy(p, &$2[1], $2[0]*sizeof(TP_TYPES));
                 $$=new_order(hashtildeargs, $2[0], p);
                 free($2); }
           | HASHRESTYPE onetype
               { void * p = malloc($2[0]*sizeof(TP_TYPES));
                 memcpy(p, &$2[1], $2[0]*sizeof(TP_TYPES));
                 $$=new_order(hashrestype, $2[0], p);
                 free($2); }
           | HASHSETREF IMPLICIT
               { $$=new_order(hashsetref, SREFimplicit); }
           | HASHSETREF EXPLICIT
               { $$=new_order(hashsetref, SREFexplicit); }
           | HASHSETREF KEEPIMPL
               { $$=new_order(hashsetref, SREFkeepimpl); }
           | HASHLOCATION HEX_NUM distcom SEMICOLON
               { if (++diststacktop == DISTSTACKSIZE)
                   yyfail("internal stack overflow");
                 diststack[diststacktop].tag = $2;
                 diststack[diststacktop].order = $3;
                 $$=$3; }
           | HASHUPDATE HEX_NUM distcom SEMICOLON
               { if ($2 == diststack[diststacktop].tag) {
                   if (diststacktop < 0)
                     yyfail("internal stack underflow");
                   insert_order_list($3, diststack[diststacktop].order);
                   delete_order(diststack[diststacktop].order);
                   diststacktop--;
                   }
                 else
                   yywarn("internal stack messed up");
                 $$=NULL;
               }
           ;

distcom:     DIST LBRACKET DISTARG_LABEL ',' DISTEND_LABEL ',' INTV ',' INTV RBRACKET
               { if (ISASM_TILDE())
                   yywarn("dist requires another argument");
                 asm_mode |= ASM_DIST; $$ = new_order(dist, $3, $5, $7, $9, 0, 0);
               }
           | DISTB LBRACKET DISTARG_LABEL ',' DISTEND_LABEL ',' INTV ',' INTV RBRACKET
               { if (ISASM_TILDE())
                   yywarn("distb requires another argument");
                 asm_mode |= ASM_DIST; $$ = new_order(distb, $3, $5, $7, $9, 0, 0);
               }
           | DIST LBRACKET DISTARG_LABEL ',' DISTEND_LABEL ',' INTV ',' INTV ',' INTV ',' INTV RBRACKET
               { asm_mode |= ASM_TILDE | ASM_DIST;
                 $$ = new_order(dist, $3, $5, $7, $9, $11, $13);
                }
           | DISTB LBRACKET DISTARG_LABEL ',' DISTEND_LABEL ',' INTV ',' INTV ',' INTV ',' INTV RBRACKET
               { asm_mode |= ASM_TILDE | ASM_DIST;
                 $$ = new_order(distb, $3, $5, $7, $9, $11, $13);
               }

/* Dies sind parameterlose Befehle */
asis_stmt:   RTT LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE;
                  $$=new_order(rtt);
                }
           | RTF LBRACKET RBRACKET SEMICOLON
                { $$=new_order(rtf); }
           | RTM LBRACKET RBRACKET SEMICOLON
                { $$=new_order(rtm); }
           | EXIT LBRACKET RBRACKET SEMICOLON
                { $$=new_order(ext); }
           | MOVEAW LBRACKET RBRACKET SEMICOLON
                { $$=new_order(moveaw); }
           | MOVEAR LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(movear); }
           | MOVETW LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(movetw); }
           | MOVETR LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(movetr); }
           | PUSHAW0 LBRACKET RBRACKET SEMICOLON
                { $$=new_order(pushaw, 0); }
           | PUSHTR0 LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(pushtr, 0); }
           | PUSHAR0 LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(pushar, 0); }
           | PUSHTW0 LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(pushtw, 0); }
           | FREE1A LBRACKET RBRACKET SEMICOLON
                { $$=new_order(freea, 1); }
           | FREE1R LBRACKET RBRACKET SEMICOLON
                { $$=new_order(freer, 1); }
           | FREE1SWT LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(freeswt, 1); }
           | PUSHAUX LBRACKET RBRACKET SEMICOLON
                { $$ = new_order(pushaux); }
           ;


base_stmt:    PUSHCW LBRACKET INT LBRACKET INTV RBRACKET RBRACKET SEMICOLON
                { $$=new_order(pushcw_i, MKINT($5)); }
           | PUSHCW LBRACKET BOOL LBRACKET FALSE_Y RBRACKET RBRACKET SEMICOLON
                { $$=new_order(pushcw_b, MKBOOL(FALSE)); }
           | PUSHCW LBRACKET BOOL LBRACKET TRUE_Y RBRACKET RBRACKET SEMICOLON
                { $$=new_order(pushcw_b, MKBOOL(TRUE)); }
           | PUSHCW LBRACKET prf1 RBRACKET SEMICOLON
                { $$ = new_order(pushcw_pf, MKPRF($3.prf), $3.types, 1); }
           | PUSHCW LBRACKET prf2 RBRACKET SEMICOLON
                { $$ = new_order(pushcw_pf, MKPRF($3.prf), $3.types, 2); }
           | PUSHCW LBRACKET prf3 RBRACKET SEMICOLON
                { $$ = new_order(pushcw_pf, MKPRF($3.prf), $3.types, 3); }
           | PUSHCW LBRACKET prf4 RBRACKET SEMICOLON
                { $$ = new_order(pushcw_pf, MKPRF($3.prf), $3.types, 4); }
           | PUSHCW LBRACKET interact RBRACKET SEMICOLON
                { $$ = new_order(pushcw_pf, MKIA($3.prf), $3.types,
                                                 get_ia_arity($3.prf) ); }
           | PUSHAW LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(pushaw, $3); }
           | PUSHTW LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(pushtw, $3); }
           | PUSHAR LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(pushar, $3); }
           | PUSHTR LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(pushtr, $3); }
           | FREESWT LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(freeswt, $3); }
           | FREEA LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(freea, $3); }
           | FREER LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(freer, $3); }
           | FREET LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(freet, $3); }
           | FREEW LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(freew, $3); }
           | APPLY LBRACKET INTV RBRACKET SEMICOLON
                { uses_apply = TRUE; $$=new_order(apply, $3); }
           | MKAP LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(mkap, $3); }
           | MKLIST LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(mklist, $3); }
           | MKILIST LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(mkilist, $3); }
           | MKDCLOS LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { $$=new_order(mkdclos, $3, $5); }
           | MKICLOS LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { $$=new_order(mkiclos, $3, $5); }
           | MKCCLOS LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { if (ISASM_TILDE())
                    yywarn("mkcclos requires third argument");
                  $$=new_order(mkcclos, $3, $5, 0);
                }
           | MKBCLOS LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { if (ISASM_TILDE())
                    yywarn("mkbclos requires third argument");
                  $$=new_order(mkbclos, $3, $5, 0);
                }
           | MKSCLOS LBRACKET INTV ',' INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(mksclos, $3, $5, $7); }
           | MKCCLOS LBRACKET INTV ',' INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(mkcclos, $3, $5, $7); }
           | MKBCLOS LBRACKET INTV ',' INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(mkbclos, $3, $5, $7); }
           | MKGACLOS LBRACKET INTV ',' INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(mkgaclos, $3, $5, $7); }
           | MKGSCLOS LBRACKET INTV ',' INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(mkgsclos, $3, $5, $7); }
           | RTC LBRACKET INT LBRACKET INTV RBRACKET RBRACKET SEMICOLON
                { $$=new_order(rtc_i, MKINT($5)); }
           | RTC LBRACKET BOOL LBRACKET FALSE_Y RBRACKET RBRACKET SEMICOLON
                { $$=new_order(rtc_b, MKBOOL(FALSE)); }
           | RTC LBRACKET BOOL LBRACKET TRUE_Y RBRACKET RBRACKET SEMICOLON
                { $$=new_order(rtc_b, MKBOOL(TRUE)); }
           | RTC LBRACKET prf1 RBRACKET SEMICOLON
                { $$=new_order(rtc_pf, MKPRF($3.prf)); } 
           | RTC LBRACKET prf2 RBRACKET SEMICOLON
                { $$=new_order(rtc_pf, MKPRF($3.prf)); } 
           | RTC LBRACKET prf3 RBRACKET SEMICOLON
                { $$=new_order(rtc_pf, MKPRF($3.prf)); } 
           | RTC LBRACKET prf4 RBRACKET SEMICOLON
                { $$=new_order(rtc_pf, MKPRF($3.prf)); }
           | RTC LBRACKET interact RBRACKET SEMICOLON
                { $$=new_order(rtc_pf, MKIA($3.prf)); }
           | RTP LBRACKET PTD LBRACKET HEX_NUM RBRACKET RBRACKET SEMICOLON
                { $$=new_order(rtp, MKPTR($5)); }
           | INCA LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { $$=new_order(inca, $3, $5); }
           | INCR LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(incr, $3, $5); }
           | INCT LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(inct, $3, $5); }
           | INCW LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { $$=new_order(incw, $3, $5); }
           | TINCA LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { $$=new_order(tinca, $3, $5); }
           | TINCR LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(tincr, $3, $5); }
           | TINCT LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(tinct, $3, $5); }
           | TINCW LBRACKET INTV ',' INTV RBRACKET SEMICOLON
                { $$=new_order(tincw, $3, $5); }
           | DECA LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(deca, $3); }
           | DECR LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(decr, $3); }
           | DECT LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(dect, $3); }
           | DECW LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(decw, $3); }
           | TDECA LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(tdeca, $3); }
           | TDECR LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(tdecr, $3); }
           | TDECT LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(tdect, $3); }
           | TDECW LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(tdecw, $3); }
           | KILLA LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(killa, $3); }
           | KILLR LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(killr, $3); }
           | KILLT LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(killt, $3); }
           | KILLW LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(killw, $3); }
           | TKILLA LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(tkilla, $3); }
           | TKILLR LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(tkillr, $3); }
           | TKILLT LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(tkillt, $3); }
           | TKILLW LBRACKET INTV RBRACKET SEMICOLON
                { $$=new_order(tkillw, $3); }
           /* PM-instructions */
           | ADVANCE LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(advance, $3); }
           | ATEND LBRACKET INTV ',' FAIL_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(atend, $3, $5); free($5); }
           | ATSTART LBRACKET INTV ',' FAIL_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(atstart, $3, $5); free($5); }
           | ATSTART LBRACKET INTV ',' CONT_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(atstart, $3, $5); free($5); }
           | BACKUP LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(advance, -$3); }
           | BIND LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(bind, $3); }
           | BINDS LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(binds, $3); }
           | BINDSUBL LBRACKET INTV ',' INTV ',' INTV ',' INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(bindsubl, $3, $5, $7, $9); }
           | CASE LBRACKET CASE_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(Case, LABcase, $3); free($3); }
           | GAMMACASE LBRACKET CASE_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(gammacase, LABcase, $3); free($3); }
           | DEREFERENCE LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(dereference); }
           | DROP LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(drop); }
           | ENDLIST LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(endlist); }
           | ENDSUBL LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(endsubl, $3); }
           | FETCH LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(fetch); }
           | INITBT LBRACKET INTV ',' INTV ',' INTV ',' INTV ',' INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(initbt, $3, $5, $7, $9, $11); }
           | MATCHARB LBRACKET INTV ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matcharb, $3, $5, $7, $1); free($5); free($7); }
           | MATCHARBS LBRACKET INTV ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matcharbs, $3, $5, $7, $1); free($5); free($7); }
           | MATCHBOOL LBRACKET BOOL LBRACKET FALSE_Y RBRACKET ','
                 FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchbool, MKBOOL(FALSE), $8, $10, $1); 
                  free($8); free($10); }
           | MATCHBOOL LBRACKET BOOL LBRACKET TRUE_Y RBRACKET ','
                 FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchbool, MKBOOL(TRUE), $8, $10, $1); 
                  free($8); free($10); }
           | MATCHC LBRACKET INTV ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; yyerror("MATCHC not implemented"); }
           | MATCHFALSE LBRACKET FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchbool, MKBOOL(FALSE), $3, $5, $1); free($3); free($5); }
           | MATCHIN LBRACKET PTD LBRACKET HEX_NUM RBRACKET ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchin, MKPTR($5), $8, $10, $1); free($8); free($10); }
           | MATCHIN LBRACKET PTD LBRACKET HEX_NUM RBRACKET ',' FAIL_LABEL ',' FAIL_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchin, MKPTR($5), $8, $10, $1); free($8); free($10); }
           | MATCHINT LBRACKET INT LBRACKET INTV RBRACKET ','
                 FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchint, MKINT($5), $8, $10, $1); free($8); free($10); }
           | MATCHLIST LBRACKET INTV ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchlist, $3, $5, $7, $1); free($5); free($7); }
           | MATCHNIL LBRACKET FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchlist, 0, $3, $5, $1); free($3); free($5); }
           | MATCHNILS LBRACKET FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchstr, 0, $3, $5, $1); free($3); free($5); }
           | MATCHPRIM LBRACKET SELEM ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchprim, MKCHAR($3), $5, $7, $1); free($5); free($7); }
           | MATCHPRIM LBRACKET prf1 ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchprim, MKPRF($3.prf), $5, $7, $1); free($5); free($7); }
           | MATCHPRIM LBRACKET prf2 ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchprim, MKPRF($3.prf), $5, $7, $1); free($5); free($7); }
           | MATCHPRIM LBRACKET prf3 ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchprim, MKPRF($3.prf), $5, $7, $1); free($5); free($7); }
           | MATCHPRIM LBRACKET prf4 ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchprim, MKPRF($3.prf), $5, $7, $1); free($5); free($7); }
           | MATCHPRIM LBRACKET interact ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE;
                  $$=new_order(matchprim, MKIA($3.prf), $5, $7, $1); free($5); free($7); }
           | MATCHSTR LBRACKET INTV ',' FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchstr, $3, $5, $7, $1); free($5); free($7); }
           | MATCHTRUE LBRACKET FAIL_LABEL ',' UND_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; 
                  $$=new_order(matchbool, MKBOOL(TRUE), $3, $5, $1); free($3); free($5); }
           | MKAFRAME LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(mkaframe, $3); }
           | MKBTFRAME LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(mkbtframe, $3); }
           | MKCASE LBRACKET PTD LBRACKET HEX_NUM RBRACKET RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(mkcase, MKPTR($5)); }
           | MKWFRAME LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(mkwframe, $3); }
           | NESTLIST LBRACKET RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(nestlist); }
           | PICK LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(pick, $3); }
           | RESTOREBT LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(restorebt, $3); }
           | RESTOREPTR LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(restoreptr, $3); }
           | RMBTFRAME LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(rmbtframe, $3); }
           | RMWFRAME LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(rmwframe, $3); }
           | SAVEBT LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(savebt, $3); }
           | SAVEPTR LBRACKET INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(saveptr, $3); }
           | STARTSUBL LBRACKET INTV ',' INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; $$=new_order(startsubl, $3, $5); }
           | TGUARD LBRACKET FAIL_LABEL ',' PTD LBRACKET HEX_NUM RBRACKET ','
                 INTV ',' INTV ',' INTV RBRACKET SEMICOLON
               { asm_mode |= ASM_TILDE; 
                 $$=new_order(tguard, $3, MKPTR($7), $10, $12, $14); free($3); }
           | MKFRAME LBRACKET INTV RBRACKET SEMICOLON
               { $$=new_order(mkframe, $3); }
           | MKSLOT LBRACKET RBRACKET SEMICOLON
               { $$=new_order(mkslot); }
           | INTER LBRACKET inter_type RBRACKET SEMICOLON
               { $$=new_order(Inter, $3); }
           | INTACT LBRACKET interact RBRACKET SEMICOLON
               { $$=new_order(intact, MKIA($3.prf), $3.types); }
           ;

param_stmt:  PUSHW LBRACKET PTD LBRACKET HEX_NUM RBRACKET RBRACKET SEMICOLON
                { $$=new_order(pushw_p, MKPTR($5)); }
           | PUSHR LBRACKET PTD LBRACKET HEX_NUM RBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(pushr_p, MKPTR($5)); }
           ;

func_call:   PUSHRET LBRACKET CONT_LABEL RBRACKET SEMICOLON
                { $$= new_order(pushret, LABcont, $3); free($3); }
           | JUMP LBRACKET CONT_LABEL RBRACKET SEMICOLON
                { $$=new_order(jump, LABcont, $3); free($3); }
           | JUMP LBRACKET BT_LABEL RBRACKET SEMICOLON
                { $$=new_order(jump, LABbt, $3); free($3); }
           | JUMP LBRACKET INLOOP_LABEL RBRACKET SEMICOLON
                { $$=new_order(jump, LABinloop, $3); free($3); }
           | JFALSE LBRACKET COND_LABEL RBRACKET SEMICOLON
                { $$=new_order(jfalse, LABcond, $3, $1.types); free($3); }
           | JTRUE LBRACKET COND_LABEL RBRACKET SEMICOLON
                { $$=new_order(jtrue, LABcond, $3, $1.types); free($3); }
           | JCOND LBRACKET COND_LABEL RBRACKET SEMICOLON
                { $$=new_order(jcond, LABcond, $3, $1.types); free($3); }
           | JFALSE2 LBRACKET COND_LABEL ',' CONT_LABEL RBRACKET SEMICOLON
                { $$=new_order(jfalse2, LABcond, LABcont, $3, $5, $1.types);
                  free($3); free($5); }
           | JTRUE2 LBRACKET COND_LABEL ',' CONT_LABEL RBRACKET SEMICOLON
                { $$=new_order(jtrue2, LABcond, LABcont, $3, $5, $1.types);
                  free($3); free($5); }
           | JCOND2 LBRACKET COND_LABEL ',' CONT_LABEL RBRACKET SEMICOLON
                { $$=new_order(jcond2, LABcond, LABcont, $3, $5, $1.types);
                  free($3); free($5); }
           | prf1 LBRACKET RBRACKET SEMICOLON
                { $$=new_order(delta1, MKPRF($1.prf), $1.types); }
           | prf2 LBRACKET RBRACKET SEMICOLON
                { $$=new_order(delta2, MKPRF($1.prf), $1.types); }
           | prf3 LBRACKET RBRACKET SEMICOLON
                { $$=new_order(delta3, MKPRF($1.prf), $1.types); }
           | prf4 LBRACKET RBRACKET SEMICOLON
                { $$=new_order(delta4, MKPRF($1.prf), $1.types); }
           | DELTA1 LBRACKET prf1 RBRACKET SEMICOLON
                { $$=new_order(delta1, MKPRF($3.prf), $3.types); }
           | DELTA2 LBRACKET prf2 RBRACKET SEMICOLON
                { $$=new_order(delta2, MKPRF($3.prf), $3.types); }
           | DELTA3 LBRACKET prf3 RBRACKET SEMICOLON
                { $$=new_order(delta3, MKPRF($3.prf), $3.types); }
           | DELTA4 LBRACKET prf4 RBRACKET SEMICOLON
                { $$=new_order(delta4, MKPRF($3.prf), $3.types); }
           | BETA LBRACKET FUNC_LABEL RBRACKET SEMICOLON
                { $$=new_order(beta, LABfunc, $3); free($3); }
           | BETA0UPDATE LBRACKET FUNC_LABEL RBRACKET SEMICOLON
                { $$=NULL; }
           | GAMMA LBRACKET FUNC_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(Gamma, LABfunc, $3); free($3); }
           | GAMMABETA LBRACKET FUNC_LABEL RBRACKET SEMICOLON
                { asm_mode |= ASM_TILDE; $$=new_order(gammabeta, LABfunc, $3); free($3); }
           | TAIL LBRACKET FUNC_LABEL RBRACKET SEMICOLON
                { $$=new_order(tail, LABfunc, $3); free($3); }
           ;

dist_cmd:    PUSHH LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_DIST; $$=new_order(pushh, $3); }
           | COUNT LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_DIST; $$=new_order(count, $3); }
           | DISTEND LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_DIST; $$=new_order(distend); }
           | WAIT LBRACKET INTV RBRACKET SEMICOLON
                { asm_mode |= ASM_DIST; $$=new_order(wait, $3); }
           | POPH LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_DIST; $$=new_order(poph); }
           | MSDISTEND LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_DIST; $$=new_order(msdistend); }
           | MSNODIST LBRACKET RBRACKET SEMICOLON
                { asm_mode |= ASM_DIST; $$=new_order(msnodist); }
           | distcom SEMICOLON
                { asm_mode |= ASM_DIST; $$=$1; }
           ;
/* einstellige primitive Funktionen */

prf1:        ABS        { $$.prf=p_abs; $$.types=$1.types;}
           | NEG        { $$.prf=p_neg; $$.types=$1.types; }
           | NOT        { $$.prf=p_not; $$.types=$1.types; }
           | EXP        { $$.prf=p_exp; $$.types=$1.types; }
           | LN         { $$.prf=p_ln; $$.types=$1.types; }
           | SIN        { $$.prf=p_sin; $$.types=$1.types; }
           | COS        { $$.prf=p_cos; $$.types=$1.types; }
           | TAN        { $$.prf=p_tan; $$.types=$1.types; }
           | FLOOR      { $$.prf=p_floor; $$.types=$1.types; }
           | CEIL       { $$.prf=p_ceil; $$.types=$1.types; }
           | FRAC       { $$.prf=p_frac; $$.types=$1.types; }
           | TRUNC      { $$.prf=p_trunc; $$.types=$1.types; }
           | VC_PLUS    { $$.prf=p_vc_plus; $$.types=$1.types; }
           | VC_MINUS   { $$.prf=p_vc_minus; $$.types=$1.types; }
           | VC_MULT    { $$.prf=p_vc_mult; $$.types=$1.types; }
           | VC_DIV     { $$.prf=p_vc_div; $$.types=$1.types; }
           | VC_MIN     { $$.prf=p_vc_min; $$.types=$1.types; }
           | VC_MAX     { $$.prf=p_vc_max; $$.types=$1.types; }
           | LDIM       { $$.prf=p_ldim; $$.types=$1.types; }
           | VDIM       { $$.prf=p_vdim; $$.types=$1.types; }
           | CLASS      { $$.prf=p_class; $$.types=$1.types; }
           | TYPE       { $$.prf=p_type; $$.types=$1.types; }
           | EMPTY      { $$.prf=p_empty; $$.types=$1.types; }
           | TRANSPOSE  { $$.prf=p_transpose; $$.types=$1.types; }
           | REVERSE    { $$.prf=p_reverse; $$.types=$1.types; }
           | TO_LIST    { $$.prf=p_to_list; $$.types=$1.types; }
           | TO_SCAL    { $$.prf=p_to_scal; $$.types=$1.types; }
           | TO_VECT    { $$.prf=p_to_vect; $$.types=$1.types; }
           | TO_TVECT   { $$.prf=p_to_tvect; $$.types=$1.types; }
           | TO_MAT     { $$.prf=p_to_mat; $$.types=$1.types; }
           | TRANSFORM  { $$.prf=p_transform; $$.types=$1.types; }
           | SLOTS      { $$.prf=p_f_slots; $$.types=$1.types; }
           | TO_CHAR    { $$.prf=p_to_char; $$.types=$1.types; }
           | TO_ORD     { $$.prf=p_to_ord; $$.types=$1.types; }
           ;

/* zweistellige primitive Funktionen */

prf2:        PLUS    { $$.prf=p_plus; $$.types=$1.types; }
           | MINUS   { $$.prf=p_minus; $$.types=$1.types; }
           | MULT    { $$.prf=p_mult; $$.types=$1.types; }
           | DIV     { $$.prf=p_div; $$.types=$1.types; }
           | QUOT    { $$.prf=p_quot; $$.types=$1.types; }
           | MOD     { $$.prf=p_mod; $$.types=$1.types; }
           | IP      { $$.prf=p_ip; $$.types=$1.types; }
           | MAX     { $$.prf=p_max; $$.types=$1.types; }
           | MIN     { $$.prf=p_min; $$.types=$1.types; }
           | AND     { $$.prf=p_and; $$.types=$1.types; }
           | OR      { $$.prf=p_or; $$.types=$1.types; }
           | XOR     { $$.prf=p_xor; $$.types=$1.types; }
           | EQ      { $$.prf=p_eq; $$.types=$1.types; }
           | NE      { $$.prf=p_ne; $$.types=$1.types; }
           | F_EQ    { $$.prf=p_f_eq; $$.types=$1.types; }
           | F_NE    { $$.prf=p_f_ne; $$.types=$1.types; }
           | GE      { $$.prf=p_ge; $$.types=$1.types; }
           | GT      { $$.prf=p_gt; $$.types=$1.types; }
           | LE      { $$.prf=p_le; $$.types=$1.types; }
           | LT      { $$.prf=p_lt; $$.types=$1.types; }
           | LSELECT { $$.prf=p_lselect; $$.types=$1.types; }
           | LCUT    { $$.prf=p_lcut; $$.types=$1.types; }
           | LROTATE { $$.prf=p_lrotate; $$.types=$1.types; }
           | LUNITE  { $$.prf=p_lunite; $$.types=$1.types; }
           | VSELECT { $$.prf=p_vselect; $$.types=$1.types; }
           | VCUT    { $$.prf=p_vcut; $$.types=$1.types; }
           | VROTATE { $$.prf=p_vrotate; $$.types=$1.types; }
           | VUNITE  { $$.prf=p_vunite; $$.types=$1.types; }
           | C_PLUS  { $$.prf=p_c_plus; $$.types=$1.types; }
           | C_MINUS { $$.prf=p_c_minus; $$.types=$1.types; }
           | C_MULT  { $$.prf=p_c_mult; $$.types=$1.types; }
           | C_DIV   { $$.prf=p_c_div; $$.types=$1.types; }
           | C_MAX   { $$.prf=p_c_max; $$.types=$1.types; }
           | C_MIN   { $$.prf=p_c_min; $$.types=$1.types; }
           | MDIM    { $$.prf=p_mdim; $$.types=$1.types; }
/* cr 22/03/95, kir(ff), START */
           | FSELECT { $$.prf=p_f_select; $$.types=$1.types; }
           | TEST    { $$.prf=p_f_test; $$.types=$1.types; }
           | DELETE  { $$.prf=p_f_delete; $$.types=$1.types; }
/* cr 22/03/95, kir(ff), END */
           | SPRINTF { $$.prf=p_sprintf; $$.types=$1.types; }
           | SSCANF  { $$.prf=p_sscanf; $$.types=$1.types; }
           ;

/* dreistellige primitive Funktionen */

prf3:        LREPLACE     { $$.prf=p_lreplace; $$.types=$1.types; }
           | VREPLACE     { $$.prf=p_vreplace; $$.types=$1.types; }
           | MSELECT      { $$.prf=p_mselect; $$.types=$1.types; }
           | MCUT         { $$.prf=p_mcut; $$.types=$1.types; }
           | MROTATE      { $$.prf=p_mrotate; $$.types=$1.types; }
           | MUNITE       { $$.prf=p_munite; $$.types=$1.types; }
           | MRE_R        { $$.prf=p_mre_r; $$.types=$1.types; }
           | MRE_C        { $$.prf=p_mre_c; $$.types=$1.types; }
           | LTRANSFORM   { $$.prf=p_ltransform; $$.types=$1.types; }
/* cr 22/03/95, kir(ff), START */
           | UPDATE       { $$.prf=p_f_update; $$.types=$1.types; }
/* cr 22/03/95, kir(ff), END */
           ;

/* vierstellige primitive Funktionen */

prf4:        MREPLACE    { $$.prf=p_mreplace; $$.types=$1.types; }
           | REPSTR      { $$.prf=p_repstr; $$.types=$1.types; }
           ;

/* stt, 13.02.96 */
interact:  IA_FOPEN   {$$.prf=ia_fopen;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_FCLOSE  {$$.prf=ia_fclose;  $$.types=$1.types; uses_apply=TRUE;}
         | IA_FGETC   {$$.prf=ia_fgetc;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_FPUTC   {$$.prf=ia_fputc;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_UNGETC  {$$.prf=ia_ungetc;  $$.types=$1.types; uses_apply=TRUE;}
         | IA_FGETS   {$$.prf=ia_fgets;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_READ    {$$.prf=ia_read;    $$.types=$1.types; uses_apply=TRUE;}
         | IA_FPUTS   {$$.prf=ia_fputs;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_FPRINTF {$$.prf=ia_fprintf; $$.types=$1.types; uses_apply=TRUE;}
         | IA_FREDIRECT {$$.prf=ia_fredirect; $$.types=$1.types; uses_apply=TRUE;}
         | IA_FSEEK   {$$.prf=ia_fseek;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_FTELL   {$$.prf=ia_ftell;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_EXIT    {$$.prf=ia_exit;    $$.types=$1.types; uses_apply=TRUE;}
         | IA_EOF     {$$.prf=ia_eof;     $$.types=$1.types; uses_apply=TRUE;}
         | IA_BIND    {$$.prf=ia_bind;    $$.types=$1.types; uses_apply=TRUE;}
         | IA_UNIT    {$$.prf=ia_unit;    $$.types=$1.types; uses_apply=TRUE;}
         | IA_FINFO   {$$.prf=ia_finfo;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_STATUS  {$$.prf=ia_status;  $$.types=$1.types; uses_apply=TRUE;}
         | IA_REMOVE  {$$.prf=ia_remove;  $$.types=$1.types; uses_apply=TRUE;}
         | IA_RENAME  {$$.prf=ia_rename;  $$.types=$1.types; uses_apply=TRUE;}
         | IA_COPY    {$$.prf=ia_copy;    $$.types=$1.types; uses_apply=TRUE;}
         | IA_MKDIR   {$$.prf=ia_mkdir;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_RMDIR   {$$.prf=ia_rmdir;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_CHDIR   {$$.prf=ia_chdir;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_GETWD   {$$.prf=ia_getwd;   $$.types=$1.types; uses_apply=TRUE;}
         | IA_DIR     {$$.prf=ia_dir;     $$.types=$1.types; uses_apply=TRUE;}
         | IA_GET     {$$.prf=ia_get;     $$.types=$1.types; uses_apply=TRUE;}
         | IA_PUT     {$$.prf=ia_put;     $$.types=$1.types; uses_apply=TRUE;}
         ;


inter_type:  DONE        { $$=INdone; }
           | GET         { $$=INget; }
           | PUT         { $$=INput; }
           ;

types:       onetype types { $$ = (TP_TYPES *)malloc(($2[0]+2)*sizeof(TP_TYPES));
                           $$[0] = $2[0]+1; $$[1] = $1[1];
                           memcpy(&$$[2], &$2[1], $2[0]*sizeof(TP_TYPES));
                           free($1); free($2); }
           | onetype     { $$ = $1; }
           ;

onetype:     INT         { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                           $$[0] = 1; $$[1] = MAKETYPE(tp_none, tp_int); }
           | REAL        { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                           $$[0] = 1; $$[1] = MAKETYPE(tp_none, tp_real); }
           | BOOL        { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                           $$[0] = 1; $$[1] = MAKETYPE(tp_none, tp_bool); }
           | CHAR        { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                           $$[0] = 1; $$[1] = MAKETYPE(tp_none, tp_char); }
           | DIGIT       { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                           $$[0] = 1; $$[1] = MAKETYPE(tp_none, tp_digit); }
           | LQUOTE onetype RQUOTE
                         { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                           if (($2[1] != tp_char) && $2[1] != tp_none)
                             yywarn("illegal nested string type");
                           $$[0] = 1; $$[1] = MAKETYPE(tp_string, $2[1] & PRIM_MASK);
                           free($2); }
           | NONE        { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                           $$[0] = 1; $$[1] = MAKETYPE(tp_none, tp_none); }
           | LBRACK onetype RBRACK
                         { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                           $$[0] = 1; $$[1] = MAKETYPE(tp_list, $2[1] & PRIM_MASK);
                           free($2); }
           | C_LBRACK onetype C_RBRACK
                          { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                          $$[0] = 1; $$[1] = MAKETYPE(tp_vect, $2[1] & PRIM_MASK);
                          free($2); }
           | C_LBRACK COLON onetype COLON C_RBRACK 
                          { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                          $$[0] = 1; $$[1] = MAKETYPE(tp_matrix, $3[1] & PRIM_MASK);
                          free($3); }
           | C_RBRACK onetype C_LBRACK
                          { $$ = (TP_TYPES *)malloc(2*sizeof(TP_TYPES));
                          $$[0] = 1; $$[1] = MAKETYPE(tp_tvect, $2[1] & PRIM_MASK);
                          free($2); }
           ;
%%
