/* $Log: rdesc.c,v $
 * Revision 1.2  1992/12/16  12:49:32  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/*****************************************************************************/
/*                        MODULE FUER DIE DEBUG-VERSION                      */
/*---------------------------------------------------------------------------*/
/* rdesc.c -- external:   DescDump.                                          */
/*         -- internal:   dir_sc,   dir_li,     dir_mvt,     dir_e,          */
/*                        e_func,   e_cond,     e_expr,      e_name,  e_var, */
/*                        e_lrec,   e_lrec_ind, e_lrec_args,                 */
/*                        dir_fu,   fu_e_comb,  fu_e_clos,   fu_e_condi,     */
/*                        fu_e_cwgb,                                         */
/*                        pte,      ptdv,       ptc,                         */
/*                        outint,   outdouble,  out_hex2,    out_hex, out,   */
/*                        hexdump                                            */
/*****************************************************************************/

#define RDESC "file rdesc.c"

#include <stdio.h>        /* RS 02/11/92 */ 
#include "rstdinc.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rmeasure.h"
#include "rinter.h"

#if DEBUG        /* in der nodebug-Version wird die Datei (fast) leer. */
#include "rinstr.h"

/*--------------------------------------------------------------------------
 * Externe Funktionen:
 *--------------------------------------------------------------------------
 */
extern int isdesc();  /* heap.c */
extern prstel();      /* print.c */
#define SHORT 1
/* Flag fuer prstel, Kurze Stackelementdarstellung gewuenscht. */

/************************************************************************/
/* Prototypen fuer die im folgenden von uns vereinbarten Funktionen  BM */
/************************************************************************/
static void	dir_fu() ;   /* static, RS 7.12.1992 */
static void	ptc() ;      /* s.o. */

/* RS 04/11/92 und 7.12.1992 */ 
static void outhex();
static void outint();
static void out();
static void outhex2();
static void hexdump();
static void dir_sc();
static void dir_di();
static void dir_li();
static void dir_e();
extern void slow();  /* rruntime.c */
static void dir_mvt(); 
static void outdouble();
static void ptdv();
static void e_func();
static void e_zfc();
static void e_cond();
static void e_var();
static void e_expr();
static void e_name();
static void pm_e_switch();
static void pm_e_nomat();
static void pm_e_match();
static void e_lrec();
static void e_lrec_ind();
static void e_lrec_args();
static void pte();
static void pm_pt_match();
/* END of RS 04/11/92 */ 

/*-------------------------------------------------------------------------
 * Lokale globale Variablen:
 *-------------------------------------------------------------------------
 */
static  FILE *  IntDumpFile = stdout;

static int mode; /* gibt an bis zu welcher Tiefe ein Deskriptorenbaum */
                 /* ausgegeben werden soll.                           */
/*-------------------------------------------------------------------------*/
/* Makro   if pointer with NIL value , don't break Programm     SJ-ID      */
/*-------------------------------------------------------------------------*/

#define  PWNILVAL(adr)                                                                                 \
       do {if (adr == NULL) {                                                                               \
         fprintf(IntDumpFile,"DescDump : no complete descriptor received or pointer with NIL value\n"); \
         return;                                                                                        \
       }} while (0)



/*********************************************************************/
/*                                                                   */
/* DescDump -- hat die Aufgabe einen hinter einem Deskriptor liegen- */
/*             den Baum in eine Datei zu schreiben. Der Parameter    */
/*             Xmode spezifiziert dabei die Tiefe, bis zu der das    */
/*             geschehen soll. Xmode gleich Null bedeutet: nur den   */
/*             Deskriptor ausgeben.                                  */
/*********************************************************************/

void /* TB, 3.11.1992 */
DescDump (XFile,XDesc,Xmode)
FILE  * XFile ;
PTR_DESCRIPTOR  XDesc ;
int   Xmode;
{
  char  class1;
  T_DESCRIPTOR  Desc;
  int oldmode ;

  Desc        = *XDesc;
  IntDumpFile = XFile;
  oldmode     = mode;    /* rette alten value  */
  mode        = (Xmode <= 0 ) ? 0 : Xmode;

  if (XDesc==NULL || !T_POINTER(XDesc)) {
    return;
  }

  fprintf(IntDumpFile,"%s","\n");
  outhex("Descriptor located at",XDesc);
  outint("Size of Descriptor",sizeof(T_DESCRIPTOR));

  switch(class1 = R_DESC(Desc,class))
  {
        case C_SCALAR : out("Class","C_SCALAR"); break;
        case C_DIGIT  : out("Class","C_DIGIT "); break;
        case C_LIST   : out("Class","C_LIST"); break;
        case C_MATRIX : out("Class","C_MATRIX"); break;
	case C_FUNC   : out("Class","C_FUNC"); break ;
	case C_CONS   : out("Class","C_CONS"); break ;
        case C_VECTOR : out("Class","C_VECTOR"); break;
        case C_TVECTOR: out("Class","C_TVECTOR"); break;
        case C_EXPRESSION
                      : out("Class","C_EXPRESSION"); break;
        case C_CONSTANT: out("Class","C_CONSTANT");break;
        default       : outhex2("illegal Class",R_DESC(Desc,class));
          /*            return(hexdump(Desc));    RS 04/11/92 */ 
                        hexdump(Desc);  /* RS 04/11/92 */ 
                        return;     /* RS 04/11/92 */ 
  }

  switch(R_DESC(Desc,type))

        {/* type switch        */

        case TY_REC   : out("Type","TY_REC" ); break;
        case TY_SUB   : out("Type","TY_SUB" ); break;
        case TY_ZF    : out("Type","TY_ZF" ); break;
        case TY_ZFCODE: out("Type","TY_ZFCODE" ); break;
        case TY_COND  : out("Type","TY_COND" ); break;
        case TY_EXPR  : out("Type","TY_EXPR" ); break;
        case TY_VAR   : out("Type","TY_VAR" ); break;
        case TY_NAME  : out("Type","TY_NAME" ); break;
        case TY_INTEGER :
                        out("Type","TY_INTEGER" ); break;
        case TY_REAL  : out("Type","TY_REAL" ); break;
        case TY_BOOL  : out("Type","TY_BOOL" ); break;
        case TY_STRING: out("Type","TY_STRING" ); break;
        case TY_DIGIT : out("Type","TY_DIGIT" ); break;
        case TY_UNDEF  :out("Type","UNDEF" ); break;
        case TY_MATRIX  :out("Type","MATRIX" ); break;
        case TY_SWITCH : out("Type","SWITCH");         break;
        case TY_MATCH  : out("Type","MATCH");          break;
        case TY_NOMAT  : out("Type", "NOMAT");         break;
        case TY_LREC   :out("Type","LREC" ); break;                    /* CS */
        case TY_LREC_IND :out("Type","LREC_IND" ); break;              /* CS */
        case TY_LREC_ARGS :out("Type","LREC_ARGS" ); break;            /* CS */
	case TY_COMB      : out("Type","COMB"); break ;                /* BM */ 
	case TY_CLOS      : out("Type","CLOS"); break ;                /* BM */ 
	case TY_CONDI     : out("Type","CONDI"); break ;               /* BM */ 
	case TY_CONS      : out("Type","CONS"); break ;                /* BM */ 
	case TY_CASE      : out("Type","CASE"); break ;                /* BM */ 
	case TY_WHEN      : out("Type","WHEN"); break ;                /* BM */ 
	case TY_GUARD     : out("Type","GUARD"); break ;               /* BM */ 
	case TY_BODY      : out("Type","BODY"); break ;                /* BM */ 
        default       : outhex2("illegal Type",R_DESC(Desc,type));
                 /*     return(hexdump(Desc));     RS 04/11/92 */ 
                        hexdump(Desc);  /* RS 04/11/92 */ 
                        return;   /* RS 04/11/92 */ 
        }/* type switch        */

  outint("Refcount", R_DESC(Desc,ref_count));

  switch(class1)             /* eigentliche Deskriptorenausgabe: */
  {
        case C_SCALAR : dir_sc(Desc); break;
        case C_DIGIT  : dir_di(Desc); break;
        case C_LIST   : dir_li(Desc); break;
        case C_FUNC   : dir_fu(Desc) ; break ;                         /* BM */
        case C_CONS   : dir_fu(Desc) ; break ;                         /* BM */
        case C_MATRIX :
        case C_VECTOR :
        case C_TVECTOR: dir_mvt(Desc,class1); break;
        case C_EXPRESSION
                      : dir_e (Desc);
                        break;
        case C_CONSTANT:dir_e (Desc); break;
        default       : outhex2("illegal class",
                                R_DESC(Desc,class));
	                hexdump(Desc);
                        /* return; *//*(hexdump(Desc))*/; /* TB, 6.11.192 */ /* not reached RS 7.12.1992 */
 }

 mode  = oldmode; /* restore old value  */
 fprintf(IntDumpFile,"\n");
 fflush(IntDumpFile);
 slow();
}

/*********************************************************************/
/*                                                                   */
/*      dir_sc -- gibt eine scalar-Deskriptor aus.                   */
/*                                                                   */
/*********************************************************************/
static void dir_sc  (Desc)      /* void eingefuegt von RS 04/11/92 */

T_DESCRIPTOR Desc;

{

  switch(R_DESC(Desc,type))

        {
        case TY_INTEGER    : outint("Value",R_SCALAR(Desc,vali));
                             break;
        case TY_REAL       : outdouble("Value",R_SCALAR(Desc,valr));
                             break;
        default       : outhex2("illegal Scalar Type",
                                 R_DESC(Desc,type));
                    /*  return(hexdump(Desc));      RS 04/11/92 */ 
                        hexdump(Desc);    /* RS 04/11/92 */ 
                        return;           /* RS 04/11/92 */ 
        }
}


/*********************************************************************/
/*                                                                   */
/*      dir_di -- gibt eine digit-Deskriptor aus.                    */
/*                                                                   */
/*********************************************************************/
static void dir_di (Desc)  /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
  int i;

  if (R_DIGIT(Desc,sign) == 0)
     out("Sign","+");
  else
     out("Sign","-");

  outint("Exponent",R_DIGIT(Desc,Exp));
  outint("# Digit",R_DIGIT(Desc,ndigit));
        /* Daten ausgeben     */
  outhex("PTDV @ to Data Vector",R_DIGIT(Desc,ptdv));
  if (mode)
  {
    for (i = R_DIGIT(Desc,ndigit); --i >= 0; )
      fprintf(IntDumpFile,"%5d ",R_DIGIT(Desc,ptdv)[i]);
    fprintf(IntDumpFile,"\n");
  }
  return; /* RS 04/11/92 */ 
} /**** end of function dir_di ****/




/*********************************************************************/
/*                                                                   */
/*      dir_li -- gibt einen Listendeskriptor mit TY_UNDEF   und     */
/*                                                TY_STRING  aus.    */
/*********************************************************************/
static void dir_li(Desc)   /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;
{
    outint("Dimension",R_LIST(Desc,dim));

    if ((R_LIST(Desc,ptdd)) == (T_PTD)0) {
      outint("Edit",R_LIST(Desc,special));
      outhex("PTDD @ to Direct Descriptor",R_LIST(Desc,ptdd));
    }
    else {
      outint("Offset",R_LIST(Desc,special));
      outhex("PTDD @ to Direct Descriptor",R_LIST(Desc,ptdd));
    }

    outhex("PTDV @ to Data Vector",R_LIST(Desc,ptdv));
    if (mode)
    ptdv(R_LIST(Desc,ptdv),R_DESC(Desc,type),R_LIST(Desc,dim));
    return; /* RS 04/11/92 */ 
} /**** end of function dir_li ****/


/*********************************************************************/
/*                                                                   */
/*      dir_mvt -- gibt einen Matrizen- bzw. einen Vektor-           */
/*                 transponierten Vektor Deskiptor aus.              */
/*                                                                   */
/*********************************************************************/
static void dir_mvt(Desc,class1)   /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;
        char class1;
{
  outint("nrows",R_MVT(Desc,nrows,class1));
  outint("ncols",R_MVT(Desc,ncols,class1));
  outhex("PTDD @ to Direct Descriptor",R_MVT(Desc,ptdd,class1));
  outhex("PTDV @ to Data Vector",R_MVT(Desc,ptdv,class1));
  if (mode)
     ptdv(R_MVT(Desc,ptdv,class1),R_DESC(Desc,type),
          R_MVT(Desc,nrows,class1) * R_MVT(Desc,ncols,class1));
  return;   /* RS 04/11/92 */ 
} /**** end of function dir_mvt ****/



/*********************************************************************/
/*                                                                   */
/*      dir_e -- steuert die Ausgabe eines Deskriptors der Klasse    */
/*               C_EXPRESSION.                                       */
/*                                                                   */
/*********************************************************************/
static void dir_e   (Desc)   /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
  switch(R_DESC(Desc,type))

        {
        case TY_REC   :

        case TY_ZF    :
        case TY_SUB   : e_func(Desc);
                        break;
        case TY_ZFCODE: e_zfc(Desc); break; 
        case TY_COND  :  e_cond(Desc);
                        break;
        case TY_VAR   :  e_var(Desc);
                        break;
        case TY_EXPR  :  e_expr(Desc);
                        break;
        case TY_NAME  :  e_name(Desc);
                        break;
        case TY_SWITCH:  pm_e_switch(Desc);
                        break;
        case TY_NOMAT:   pm_e_nomat(Desc);
                        break;
        case TY_MATCH:   pm_e_match(Desc);
                        break;
        case TY_LREC  :  e_lrec(Desc);                                 /* CS */
                        break;                                         /* CS */
        case TY_LREC_IND  :  e_lrec_ind(Desc);                         /* CS */
                        break;                                         /* CS */
        case TY_LREC_ARGS  :  e_lrec_args(Desc);                       /* CS */
                        break;                                         /* CS */
        default       : outhex2(" illegal Type", R_DESC(Desc,type));
                     /* return(hexdump(Desc));    RS 04/11/92 */ 
                        hexdump(Desc);  /* RS 04/11/92 */ 
                        return;   /* RS 04/11/92 */ 
        }
 return; /*  RS 04/11/92 */
}
/*********************************************************************/
/*                                                                   */
/*      pm_e_switch -- gibt einen switch-Deskriptor aus.              */
/*                                                                   */
/*********************************************************************/
 static void pm_e_switch    (Desc)    /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
 outint("Zahl der when-ausdruecke", R_SWITCH(Desc, nwhen));
 outint("case_type ist ", R_SWITCH(Desc, case_type));
 outhex("PTE @ Pointer to data vector", R_SWITCH(Desc,ptse));
 if (mode)
 pte(R_SWITCH(Desc,ptse));

 return; /* RS 04/11/92 */ 
}



/*********************************************************************/
/*                                                                   */
/*      pm_e_match -- gibt einen match-Deskriptor aus.              */
/*                                                                   */
/*********************************************************************/
 static void pm_e_match    (Desc)   /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{

 outhex("guard expresssion",R_MATCH(Desc,guard));
 if (mode)
 pte(R_MATCH(Desc,guard))  ;

 outhex("body expresssion",R_MATCH(Desc,body));
 if (mode)
 pte(R_MATCH(Desc,body))  ;

 outhex("Matchcode" ,R_MATCH(Desc, code));
   if (   ( R_MATCH((Desc), code) != 0  )  
        && mode
       )
   pm_pt_match(R_MATCH(Desc, code))  ;
    
 return; /* RS 04/11/92 */ 

}


/*********************************************************************/
/*                                                                   */
/*      pm_e_nomat -- gibt einen Nomatchdeskriptor aus.              */
/*                                                                   */
/*********************************************************************/ 
static void 
pm_e_nomat(Desc)            /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{

 outint("act_nomat",R_NOMAT(Desc,act_nomat));

 outhex("zeigt auf guard und body . ",R_NOMAT(Desc,guard_body));
 
 outhex("zeigt auf Swi.deskr. ",R_NOMAT(Desc,ptsdes));

 if(R_NOMAT(Desc,guard_body)!= 0)
   if (mode)
     pte(R_NOMAT(Desc,guard_body));

 if (isdesc(R_NOMAT(Desc, ptsdes)))
   if (mode)
     DescDump(IntDumpFile,R_NOMAT(Desc,ptsdes),mode-1);

 return; /* RS 04/11/92 */ 
}


/*********************************************************************/
/*                                                                   */
/*      e_func -- gibt einen Funktionsdeskriptor aus.                */
/*                                                                   */
/*********************************************************************/

static void  
e_func(Desc)  /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
 outint("Edit",R_FUNC(Desc,special));
 outint("nargs",R_FUNC(Desc,nargs));
 outhex("Pointer to bound variable names",R_FUNC(Desc,namelist));
 outhex("Pointer to expression",R_FUNC(Desc,pte));
 
 if (mode)
  {
   int save=0;	                                            /* uh 08Aug90 */
   if ((R_FUNC(Desc,nargs)!=0) && R_FUNC(Desc,namelist)){   /* uh 17Dec90 */
     save=R_FUNC(Desc,namelist)[0];                         /* uh 08Aug90 */
     L_FUNC(Desc,namelist)[0]=R_FUNC(Desc,nargs);           /* uh 08Aug90 */
   }
   if (R_FUNC(Desc,namelist)) pte(R_FUNC(Desc,namelist));   /* uh 17Dec90 */
   pte(R_FUNC(Desc,pte));
   if (R_FUNC(Desc,namelist)) L_FUNC(Desc,namelist)[0]=save;/* uh 08Aug90 */
  }

 return;   /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/*      e_zfc -- gibt einen ZF-Codedescriptor aus                    */
/*                                                                   */
/*********************************************************************/

 static void e_zfc     (Desc)  /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
 outint("nargs",R_ZFCODE(Desc,nargs));
 outhex("Pointer to expression",R_ZFCODE(Desc,ptd));
 if (mode)
   DescDump(IntDumpFile,R_ZFCODE(Desc,ptd),mode-1);    

 outhex("Pointer to Variable-names",R_ZFCODE(Desc,ptd));
 if (mode)
   pte(R_ZFCODE(Desc,varnames));
 return;   /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/*      e_cond -- gibt einen cond-Deskriptor aus.                    */
/*                                                                   */
/*********************************************************************/

static void
e_cond(Desc)   /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
 outhex("PTTE @ Pointer to then expression",R_COND(Desc,ptte));
 outhex("PTEE @ Pointer to else expression",R_COND(Desc,ptee));

 if (mode)
  {
   pte(R_COND(Desc,ptte));
   pte(R_COND(Desc,ptee));
  }
 return;  /* RS 04/11/92 */ 

}


/*********************************************************************/
/*                                                                   */
/*      e_expr -- gibt einen expression-Deskriptor aus.              */
/*                                                                   */
/*********************************************************************/
 static  void e_expr    (Desc)  /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
 outhex("PTE @ Pointer to expression",R_EXPR(Desc,pte));
 if (mode)
 pte(R_EXPR(Desc,pte));
 return; /* RS 04/11/92 */ 

}


/*********************************************************************/
/*                                                                   */
/*      e_name -- gibt einen Namensdeskriptor aus.                   */
/*                                                                   */
/*********************************************************************/
 static  void e_name    (Desc)  /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
 outhex("PTN @ Pointer to name",R_NAME(Desc,ptn));
 if (mode)
 pte(R_NAME(Desc,ptn));
 return; /* RS 04/11/92 */ 

}


/*********************************************************************/
/*                                                                   */
/*      e_var -- gibt einen Variablendeskriptor aus.                 */
/*                                                                   */
/*********************************************************************/
static void e_var     (Desc)   /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR Desc;

{
 outint("nlabar",R_VAR(Desc,nlabar));

 outhex("PTND @ Pointer to Namedescriptor",R_VAR(Desc,ptnd));
 if (mode)
     DescDump(IntDumpFile,R_VAR(Desc,ptnd),mode-1);
 return; /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/*  e_lrec -- gibt einen LETREC-Deskriptor aus.              * CS *  */
/*                                                                   */
/*********************************************************************/
                                                                       /* CS */
static
/* void eingefuegt von RS 04/11/92 */
void e_lrec(Desc)                                                           /* CS */ 
                                                                       /* CS */
T_DESCRIPTOR Desc;                                                     /* CS */
                                                                       /* CS */
{                                                                      /* CS */
 outint("nfuncs",R_LREC(Desc,nfuncs));                                 /* CS */
 outhex("Pointer to LETREC variable names",R_LREC(Desc,namelist));     /* CS */
 outhex("Pointer to function definitions",R_LREC(Desc,ptdv));          /* CS */

 if (mode)                                                             /* CS */
  {
   pte(R_LREC(Desc,namelist));                                         /* CS */
   ptdv(R_LREC(Desc,ptdv),R_DESC(Desc,type),R_LREC(Desc,nfuncs));      /* CS */
  }
                                                                       /* CS */
 return; /* RS 04/11/92 */ 
}                                                                      /* CS */

/*********************************************************************/
/*                                                                   */
/*  e_lrec_ind -- gibt einen LETREC_ind-Deskriptor aus.      * CS *  */
/*                                                                   */
/*********************************************************************/
                                                                       /* CS */
static
void/* void eingefuegt von RS 04/11/92 */ 
e_lrec_ind(Desc)                                                       /* CS */
                                                                       /* CS */
T_DESCRIPTOR Desc;                                                     /* CS */
                                                                       /* CS */
{                                                                      /* CS */
 outint("index",R_LREC_IND(Desc,index));                               /* CS */
 outhex("Pointer to LETREC",R_LREC_IND(Desc,ptdd));                    /* CS */
 outhex("Pointer to function definition",R_LREC_IND(Desc,ptf));        /* CS */

 if (mode)                                                             /* CS */
  {
   DescDump(IntDumpFile,R_LREC_IND(Desc,ptdd),mode-1);                 /* CS */
   DescDump(IntDumpFile,R_LREC_IND(Desc,ptf),mode-1);                  /* CS */
  }

 return; /* RS 04/11/92 */ 
}                                                                      /* CS */

/*********************************************************************/
/*                                                                   */
/*  e_lrec_args -- gibt einen LETREC_args-Deskriptor aus.    * CS *  */
/*                                                                   */
/*********************************************************************/
                                                                       /* CS */
static 
void  /* void eingefuegt von RS 04/11/92 */ 
e_lrec_args(Desc)                                                      /* CS */
                                                                       /* CS */
T_DESCRIPTOR Desc;                                                     /* CS */
                                                                       /* CS */
{                                                                      /* CS */
 outint("nargs",R_LREC_ARGS(Desc,nargs));                              /* CS */
 outhex("Pointer to arguments",R_LREC_ARGS(Desc,ptdv));                /* CS */
 outhex("Pointer to LETREC",R_LREC_ARGS(Desc,ptdd));                   /* CS */

 if (mode)                                                             /* CS */
  {
   ptdv(R_LREC_ARGS(Desc,ptdv),                                        /* CS */
        R_DESC(Desc,type),R_LREC_ARGS(Desc,nargs));                    /* CS */
   DescDump(IntDumpFile,R_LREC_ARGS(Desc,ptdd),mode-1);                /* CS */
  }

 return; /* RS 04/11/92 */ 
}                                                                      /* CS */


/*********************************************************************/
/*                                                                   */
/*********************************************************************/

/*********************************************************************/
/*      dir_fu steuert die Ausgabe der FUNC-Descriptor-Klasse   * BM */ /* RS 02/11/92 */ 
/*********************************************************************/

static void
dir_fu (Desc)
T_DESCRIPTOR Desc ;
{
  switch(R_DESC(Desc,type)) {
    case TY_COMB :
           outint("args",R_COMB(Desc,args)) ;
           outint("nargs",R_COMB(Desc,nargs)) ;
           outhex("Pointer to code",R_COMB(Desc,ptc)) ;
           outhex("Pointer to Descriptor",R_COMB(Desc,ptd)) ;
           if (mode) {
             ptc(R_COMB(Desc,ptc)) ;
             DescDump(IntDumpFile,R_COMB(Desc,ptd),mode-1) ;
           }
           break;
    case TY_CLOS :
           outint("args",R_CLOS(Desc,args)) ;
           outint("nargs",R_CLOS(Desc,nargs)) ;
           switch (R_CLOS(Desc,ftype)) {
             case TY_COMB : out("Closure Type","COMBINATOR") ; break ;
             case TY_CONDI: out("Closure Type","CONDITIONAL") ; break ;
             case TY_CASE : out("Closure Type","CASE") ; break ;
             case TY_CLOS : out("Closure Type","CLOSURE") ; break ;
             case TY_PRIM : out("Closure Type","PRIMITIVE FUNCTION") ; break;
             case TY_SELECTION : out ("Closure Type","SELECTION"); break;
             default      : out("Closure Type","INVALID CLOSURE TYPE");
           }
           outhex("Pointer to argument vector",R_CLOS(Desc,pta)) ;
           if (mode) {
             ptdv(R_CLOS(Desc,pta),R_DESC(Desc,type),R_CLOS(Desc,args)+1)   ;
           }
           break;
    case TY_CONDI :
           outint("args",R_CONDI(Desc,args)) ;
           outint("nargs",R_CONDI(Desc,nargs)) ;
           outhex("Pointer to descriptor",R_CONDI(Desc,ptd)) ;
           outhex("Pointer to code",R_CONDI(Desc,ptc)) ;
           if (mode) {
             ptc(R_CONDI(Desc,ptc)) ;
           }
           break;
    case TY_CASE : 
    case TY_WHEN :
    case TY_GUARD: 
    case TY_BODY :
           outint("args",R_CASE(Desc,args)) ;
           outint("nargs",R_CASE(Desc,nargs)) ;
           outhex("Pointer to code",R_CASE(Desc,ptc)) ;
           outhex("Pointer to CASE-Descriptor",R_CASE(Desc,ptd)) ;
           if (mode) {
             ptc(R_CASE(Desc,ptc)) ;
             DescDump(IntDumpFile,R_CASE(Desc,ptd),mode-1) ;
           }
           break;
    case TY_CONS :
           outhex("Head",R_CONS(Desc,hd)) ;
           outhex("Tail",R_CONS(Desc,tl)) ;
           if (mode) {
             DescDump(IntDumpFile,R_CONS(Desc,hd),mode-1);
             DescDump(IntDumpFile,R_CONS(Desc,tl),mode-1);
           }
           break;
    default       : outhex2("Illegal Type",R_DESC(Desc,type)) ;
  }
 return; /* RS 04/11/92 */ 
}

/*********************************************************************/
/*      ptc -- gibt einen Code-Vektor aus                       * BM */ /* RS 02/11/92 */ 
/*********************************************************************/

static void ptc(adr)
INSTR *adr ;
{
  extern INSTR_DESCR instr_tab[];
  extern char *prinstr();
  if (adr) {
    while (*adr != I_END) {
      fprintf(IntDumpFile,"%x: %s\n",(unsigned int)adr,prinstr(adr));
      adr += instr_tab[*adr].nparams + 1;
    }
  }
 return; /* RS 04/11/92 */ 
}

/*********************************************************************/
/*      pte -- gibt einen beliegen Ausdruck aus dem Heap, dem        */
/*             dessen Laenge vorangestellt ist, aus.                 */
/*********************************************************************/
 static void pte         (adr)   /* void eingefuegt von RS 04/11/92 */ 

int  *adr;

{

int i;

PWNILVAL(adr);

/* ak *//* Ausgabe der Laenge einer Heapstruktur */

        fprintf (IntDumpFile, " at %x: %d heap entries\n",(unsigned int)adr,*adr);

for (i = 1; i <= adr[0]; )   /* Ausgabe in Stackelementdarstellung */
{
 prstel(IntDumpFile,adr[i++],SHORT);
 fprintf(IntDumpFile,"\n");
}

for (i = 1; i <= adr[0];i++ ) /* Rekursives Absteigen im Deskriptorenbaum */
  if (T_POINTER(adr[i]) && (isdesc(adr[i])))
     DescDump(IntDumpFile,(PTR_DESCRIPTOR)adr[i],mode-1);   /* cast eingefuegt RS 7.12.1992 */
return; /* RS 04/11/92 */ 
}


/*********************************************************************/
/*                                                                   */
/*      ptdv -- gibt eine Liste von Elementen aus.                   */
/*                                                                   */
/* Parameter:   adr - Heapadresse der Liste,                         */
/*              typ - Typ der Listenelemente,                        */
/*              zahl - Laenge der Liste.                             */
/*                                                                   */
/*********************************************************************/
static void ptdv    (adr,typ,zahl)  /* void eingefuegt von RS 04/11/92 */ 
int  *adr;
int  typ;
int  zahl;
{
  int *i,counter;
  double *f;

  PWNILVAL(adr);
  switch(typ)
  {
  case TY_INTEGER:        /* eine Liste von Zahlen */
       for (i=adr;i<adr+zahl;i++)
        fprintf(IntDumpFile,"  %d  ",(*i));
      fprintf(IntDumpFile,"\n");
      break;
   case TY_BOOL:          /* eine Liste von Wahrheitswerten, wobei ein True   */
       for (i=adr;i<adr+zahl;i++) /* durch Werte ungleich Null repraesentiert */
       if (*i)                    /* werden.                                  */
        fprintf(IntDumpFile,"  TRUE  ");
       else
        fprintf(IntDumpFile,"  FALSE  ");
      fprintf(IntDumpFile,"\n");
      break;
  case TY_REAL:           /* eine Liste von double-precision-real */
       f = (double *) adr;
       for (counter = 0; counter < zahl; counter++)
        fprintf(IntDumpFile,"  %f  ",f[counter]);
       fprintf(IntDumpFile,"\n");
       break;
  case TY_DIGIT :         /* eine Liste von digit-Deskriptoren */
       for (i=adr;i<adr+zahl;i++)
         fprintf(IntDumpFile,"%x ",*i);
       fprintf(IntDumpFile,"\n");
       if (mode)
         for (i=adr;i<adr+zahl;i++)
           DescDump(IntDumpFile,(PTR_DESCRIPTOR)*i,mode-1);  /* cast RS 7.12.1992 */
      break;
  case TY_MATRIX    :     /* eine Liste von Matrizen */
      for (i=adr;i<adr+zahl;i++)
      DescDump(IntDumpFile,(PTR_DESCRIPTOR)*i,mode-1);  /* cast RS 7.12.1992 */
      fprintf(IntDumpFile,"\n");
      break;
  case TY_LREC:                                                        /* CS */
  case TY_LREC_ARGS:                                                   /* CS */
  case TY_STRING :
  case TY_UNDEF:          /* eine Liste von Elementen unbekannten Typs */
  default :
       for (i=adr;i<adr+zahl;i++)
          prstel(IntDumpFile,*i,SHORT);
       fprintf(IntDumpFile,"\n");
       for (i=adr;i<adr+zahl; i++)
         if (T_POINTER(*i) && (isdesc(*i)))
           DescDump(IntDumpFile,(PTR_DESCRIPTOR)*i,mode-1);  /* cast RS 7.12.1992 */
      break;
  }
 return; /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/*      outint -- druckt einen kommentierten Integerwert aus.        */
/*                                                                   */
/*********************************************************************/

static void outint(s1,i)     /* void eingefuegt von RS 04/11/92 */

char * s1 ;
int i;
{
fprintf(IntDumpFile,"%40.40s : %d \n",s1,i);
return; /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/*      outdouble -- druckt einen kommentierten Doublewert aus.       */
/*                                                                   */
/*********************************************************************/

static void outdouble(s1,i)    /* void eingefuegt von RS 04/11/92 */ 

char * s1 ;
double i;
{
fprintf(IntDumpFile,"%40.40s : %f \n",s1,i);
return;      /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/*      outhex2 -- druckt die kommentierten letzten zwei Hexziffern  */
/*                 einer Zahl aus.                                   */
/*********************************************************************/

static void outhex2(s1,x)     /* void eingefuegt von RS 04/11/92 */ 

char * s1 ;
int  x;
{
fprintf(IntDumpFile,"%40.40s : %2.2x \n",s1,x&0x000000ff);
return;   /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/*      outhex -- druckt einen kommentierten Hexadezimalwert aus.    */
/*                                                                   */
/*********************************************************************/

static void outhex(s1,x)    /* void eingefuegt von RS 04/11/92 */ 

char * s1 ;
int x;
{
fprintf(IntDumpFile,"%40.40s : %8.8x \n",s1,x);
return;  /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/*      out -- druckt einen kommentieren String aus.                 */
/*                                                                   */
/*********************************************************************/

static void out(s1,s2)            /* void eingefuegt von RS 04/11/92 */ 

char * s1 ;
char * s2 ;
{
fprintf(IntDumpFile,"%40.40s : %-25s\n",s1,s2);
return;     /* RS 04/11/92 */ 
}

/*********************************************************************/
/*                                                                   */
/* hexdump -- Die 16 Byte eines Deskriptors werden hexadezimal aus-  */
/*            gedruckt, da der Deskriptor nicht identifizierbar war. */
/*                                                                   */
/*********************************************************************/

static void hexdump   (Desc  )     /* void eingefuegt von RS 04/11/92 */ 

T_DESCRIPTOR  Desc ;
{
register int j ;
register int  *p;

/* loop and dump via integer-pointer */
p= (int *) &Desc;
printf("\n");

for(j=0;j<4;j++)
{
  fprintf(IntDumpFile,"  %8.8x  ",*p);
  p++;
}
 fprintf(IntDumpFile,"\n") ;
 return;    /* RS 04/11/92 */ 
}

static void pm_pt_match(adr)         /* void eingefuegt von RS 04/11/92 */ 

int *adr;

{
  int i;

  fprintf (IntDumpFile, " at %x: %d heap entries\n",(unsigned int)adr,*adr); /* dg 10.08.92 */ 
  for (i= 1; i <= adr[0]; i++ )
  {
    fprintf(IntDumpFile, " %3d: ",i); /* dg 10.08.92 */
    switch (adr[i])
    {
      case LI_CHECK:
         fprintf(IntDumpFile, "LI_CHECK \n"    );
         break;

      case A_LI_CHECK:
         fprintf(IntDumpFile, "A_LI_CHECK \n"    );
         break;

      case SKIPSKIP:
         fprintf(IntDumpFile, "SKIPSKIP \n"    );
         break;

      case PM_DOLL:
         fprintf(IntDumpFile, "PM_DOLL \n"    );
         break;

      case LA_SKIPSKIP:
         fprintf(IntDumpFile, "LA_SKIPSKIP \n"    );

         break;

      case A_TESTV:
         fprintf(IntDumpFile, "A_TESTV \n"    );
         break;

      case ATOM_TEST:
         fprintf(IntDumpFile, "ATOM_TEST \n"    );
         break;

      case A_ATOM_TEST:
         fprintf(IntDumpFile, "A_ATOM_TEST \n"    );
         break;

      case TESTV:
         fprintf(IntDumpFile, "TESTV \n"    );
         break;

      case JMP:
         fprintf(IntDumpFile, "JMP \n"    );
         break;

      case GEN_BLOCK:
         fprintf(IntDumpFile, "GEN_BLOCK \n"    );
         break;

      case A_LOAD_ARITY :
         fprintf(IntDumpFile, "A_LOAD_ARITY \n" );
         break;

      case LOAD_ARITY :
         fprintf(IntDumpFile, "LOAD_ARITY \n" );
         break;

      case L_LOAD_A :
         fprintf(IntDumpFile, "L_LOAD_A \n" );
         break;

      case A_SETBASE:
         fprintf(IntDumpFile, "A_SETBASE \n" );
         break;

      case SETBASE:
         fprintf(IntDumpFile, "SETBASE \n" );
         break;

      case INCR:
         fprintf(IntDumpFile, "INCR \n");
         break;

      case DECR:
         fprintf(IntDumpFile, "DECR \n" );
         break;

      case TESTN:
         fprintf(IntDumpFile, "TESTN \n");
         break;

      case STR_TEST:
         fprintf(IntDumpFile, "STR_TEST \n");
         break;
      case NILSTRING_TEST:
         fprintf(IntDumpFile, "NILSTRING_TEST \n");
         break;
      case JMP2 :
         fprintf(IntDumpFile, "JMP2 \n");
         break;
      case IN_TEST:
         fprintf(IntDumpFile, "IN_TEST \n");
         break;
      case IN_TEST1:
         fprintf(IntDumpFile, "IN_TEST1\n");
         break;
      case IN_SKIPSKIP:
         fprintf(IntDumpFile, "IN_SKIPSKIP \n");
         break;
      case LA_IN_SKIPSKIP:
         fprintf(IntDumpFile, "LA_IN_SKIPSKIP \n");
         break;
      case IN_PM_DOLL:
         fprintf(IntDumpFile, "IN_PM_DOLL \n");
         break;
      case DEL_BLOCKS:
         fprintf(IntDumpFile, "DEL_BLOCKS \n");
         break;

      case DIG_TEST:
         fprintf(IntDumpFile, "DIG_TEST \n");
         break;

      case SCAL_TEST:
         fprintf(IntDumpFile, "SCAL_TEST \n");
         break;

      case MATRIX_TEST:
         fprintf(IntDumpFile, "MATRIX_TEST \n");
         break;

      case VECTOR_TEST:
         fprintf(IntDumpFile, "VECTOR_TEST \n");
         break;

      case TVECTOR_TEST:
         fprintf(IntDumpFile, "TVECTOR_TEST \n");
         break;

      case BOOL_TEST:
         fprintf(IntDumpFile, "BOOL_TEST \n");
         break;

      case TY_TEST:
         fprintf(IntDumpFile, "TY_TEST \n");
         break;

      case NIL_TEST:
         fprintf(IntDumpFile, "NIL_TEST \n");
         break;

      case NILM_TEST:
         fprintf(IntDumpFile, "NILM_TEST \n");
         break;

      case NILV_TEST:
         fprintf(IntDumpFile, "NILV_TEST \n");
         break;

      case NILTV_TEST:
         fprintf(IntDumpFile, "NILTV_TEST \n");
         break;

      case A_STR_TEST:
         fprintf(IntDumpFile, "A_STR_TEST \n");
         break;
      case A_NILSTRING_TEST:
         fprintf(IntDumpFile, "A_NILSTRING_TEST\n");
         break;

      case A_DIG_TEST:
         fprintf(IntDumpFile, "A_DIG_TEST \n");
         break;

      case A_SCAL_TEST:
         fprintf(IntDumpFile, "A_SCAL_TEST \n");
         break;

      case A_MATRIX_TEST:
         fprintf(IntDumpFile, "A_MATRIX_TEST \n");
         break;
      case A_VECTOR_TEST:
         fprintf(IntDumpFile, "A_VECTOR_TEST \n");
         break;
      case A_TVECTOR_TEST:
         fprintf(IntDumpFile, "A_TVECTOR_TEST \n");
         break;

      case A_BOOL_TEST:
         fprintf(IntDumpFile, "A_BOOL_TEST \n");
         break;

      case A_TY_TEST:
         fprintf(IntDumpFile, "A_TY_TEST \n");
         break;

      case A_NIL_TEST:
         fprintf(IntDumpFile, "A_NIL_TEST \n");
         break;

      case A_NILM_TEST:
         fprintf(IntDumpFile, "A_NILM_TEST \n");
         break;
      case A_NILV_TEST:
         fprintf(IntDumpFile, "A_NILV_TEST \n");
         break;
      case A_NILTV_TEST:
         fprintf(IntDumpFile, "A_NILTV_TEST \n");
         break;

      case LOAD:
         fprintf(IntDumpFile, "LOAD \n");
         break;

      case AS_LOAD:
         fprintf(IntDumpFile, "AS_LOAD \n");
         break;

      case L_LOAD_B:
         fprintf(IntDumpFile, "L_LOAD_B \n");
         break;

      case A_TESTN:
         fprintf(IntDumpFile, "A_TESTN \n");
         break;

      case A_KTEST:
         fprintf(IntDumpFile, "A_KTEST \n");
         break;

      case A_LOAD:
         fprintf(IntDumpFile, "A_LOAD \n");
         break;

      case SUBR:
         fprintf(IntDumpFile, "SUBR \n");
         break;

      case RETURN:
         fprintf(IntDumpFile, "RETURN\n" );
         break;

      case END_DOLL:
         fprintf(IntDumpFile, "END_DOLL\n" );
         break;

      case PM_CUTT:
         fprintf(IntDumpFile, "PM_CUTT \n");
         break;

      case EOT:
         fprintf(IntDumpFile, "EOT \n");
         break;


      default:

         if ( isdesc(adr[i]) )
         {
            fprintf(IntDumpFile, "%8.8x \n"  ,  adr[i]  );
         }
         else
         {
            fprintf(IntDumpFile, "%8.8u \n"  , adr[i]  );
         }

         break;
    }/*ende des switch */
  }/* ende der for schleife */

   if (isdesc(adr[1]) )
      DescDump(IntDumpFile, (PTR_DESCRIPTOR)adr[1], mode-1);     /* Ausgabe des urspruengl. pattern*/ /* cast RS 7.12.1992 */

return;
}

#else
#if !sun
/*static int dummy(){return (0);}*/   /* RS 02/11/92 */
#endif	/* !sun	*/
#endif /* DEBUG */

/* end of  rdesc.c */
