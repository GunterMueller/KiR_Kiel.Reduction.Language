/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*---------------------------------------------------------------------------
 * MODULE FUER DIE DEBUG-VERSION
 * rdesc.c external -- DescDump.
 * rdesc.c internal -- dir_sc, dir_st, dir_li, dir_fi, dir_e,
 *                        e_func, e_cond, e_expr, e_name, e_var,
 *                        e_lrec, e_lrec_ind, e_lrec_args,              * CS *
 *                        pte, ptdv,
 *                        outint, outdouble, out_hex2, out_hex, out, hexdump.
 *----------------------------------------------------------------------------
 */
/* last update  by     why
   08.06.88     ca     letrec hinzugefuegt
   copied       by     to
   09.06.88     sj     (struck)idesc.c
*/
#if DEBUG        /* in der nodebug-Version wird die Datei (fast) leer. */

#include "rstdinc.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rstackty.h"
#include "pminstr.h"
#include "case.h"

/*--------------------------------------------------------------------------
 * Externe Funktionen:
 *--------------------------------------------------------------------------
 */
extern int isdesc();  /* heap.c */
extern prstel();      /* printstel.c */
extern int isdesc();  /* heap.c */
#define SHORT 1
/* Flag fuer prstel, Kurze Stackelementdarstellung gewuenscht. */

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
       if (adr == NULL) {                                                                               \
         fprintf(IntDumpFile,"DescDump : no complete descriptor received or pointer with NIL value\n"); \
         return;                                                                                        \
       }  else



/*********************************************************************/
/*                                                                   */
/* DescDump -- hat die Aufgabe einen hinter einem Deskriptor liegen- */
/*             den Baum in eine Datei zu schreiben. Der Parameter    */
/*             Xmode spezifiziert dabei die Tiefe, bis zu der das    */
/*             geschehen soll. Xmode gleich Null bedeutet: nur den   */
/*             Deskriptor ausgeben.                                  */
/*********************************************************************/

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

  if (XDesc==NULL)  {
          fprintf(IntDumpFile,"\n DescDump: no descriptor received. \n");
          return ;
  }

  fprintf(IntDumpFile,"%s","\n");
  outhex("Descriptor located at",XDesc);
  outint("Size of Descriptor",sizeof(T_DESCRIPTOR));

  switch(class1 = R_DESC(Desc,class))
  {
        case C_SCALAR : out("Class","C_SCALAR"); break;
        case C_LIST   : out("Class","C_LIST"); break;
#if LARGE
        case C_DIGIT  : out("Class","C_DIGIT "); break;
        case C_MATRIX : out("Class","C_MATRIX"); break;
        case C_VECTOR : out("Class","C_VECTOR"); break;
        case C_TVECTOR: out("Class","C_TVECTOR"); break;
#endif 
        case C_EXPRESSION
                      : out("Class","C_EXPRESSION"); break;
        case C_CONSTANT: out("Class","C_CONSTANT");break;
        default       : outhex2("illegal Class",R_DESC(Desc,class));
                        return(hexdump(Desc));
  }

  switch(R_DESC(Desc,type))

        {/* type switch        */

        case TY_REC   : out("Type","TY_REC" ); break;
        case TY_SUB   : out("Type","TY_SUB" ); break;
        case TY_CLOS  : out("Type","TY_CLOS" );break;
        case TY_P_CLOS  : out("Type","TY_P_CLOS" );break;
        case TY_COND  : out("Type","TY_COND" ); break;
        case TY_EXPR  : out("Type","TY_EXPR" ); break;
        case TY_VAR   : out("Type","TY_VAR" ); break;
        case TY_NAME  : out("Type","TY_NAME" ); break;
        case TY_FNAME : out("Type","TY_FNAME" ); break;
        case TY_INTEGER :
                        out("Type","TY_INTEGER" ); break;
        case TY_REAL  : out("Type","TY_REAL" ); break;
        case TY_BOOL  : out("Type","TY_BOOL" ); break;
        case TY_STRING: out("Type","TY_STRING" ); break;
        case TY_UNDEF  :out("Type","UNDEF" ); break;
#if LARGE
        case TY_DIGIT : out("Type","TY_DIGIT" ); break;
        case TY_MATRIX  :out("Type","MATRIX" ); break;
#endif 
        case TY_SWITCH : out("Type","SWITCH");         break;
        case TY_MATCH  : out("Type","MATCH");          break;
        case TY_NOMAT  : out("Type", "NOMAT");         break;
        case TY_LREC   :out("Type","LREC" ); break;                    /* CS */
        case TY_LREC_IND :out("Type","LREC_IND" ); break;              /* CS */
        default       : outhex2("illegal Type",R_DESC(Desc,type));
                        return(hexdump(Desc));
        }/* type switch        */

  outint("Refcount", R_DESC(Desc,ref_count));

  switch(class1) /* eigentliche Deskriptorenausgabe: */
  {
        case C_SCALAR : dir_sc(Desc); break;
        case C_LIST   : dir_li(Desc); break;
#if LARGE
        case C_DIGIT  : dir_di(Desc); break;
        case C_MATRIX :
        case C_VECTOR :
        case C_TVECTOR: dir_mvt(Desc,class1); break;
#endif 
        case C_EXPRESSION
                      : dir_e (Desc); break;
        case C_CONSTANT:dir_e (Desc); break;
        default       : outhex2("illegal class",
                                R_DESC(Desc,class));
                        return(hexdump(Desc));
 }

 mode  = oldmode; /* restore old value  */
 fprintf(IntDumpFile,"\n\n");
}

/*********************************************************************/
/*                                                                   */
/*      dir_sc -- gibt eine scalar-Deskriptor aus.                   */
/*                                                                   */
/*********************************************************************/
static dir_sc  (Desc)

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
                        return(hexdump(Desc));
        }
}
#if LARGE


/*********************************************************************/
/*                                                                   */
/*      dir_di -- gibt eine digit-Deskriptor aus.                    */
/*                                                                   */
/*********************************************************************/
static dir_di (Desc)

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
  if (mode && (R_DESC(Desc,ref_count) > 0 ))
  {
    for (i = R_DIGIT(Desc,ndigit); --i >= 0; )
      fprintf(IntDumpFile,"%5d ",R_DIGIT(Desc,ptdv)[i]);
    fprintf(IntDumpFile,"\n");
  }
}
#endif 


/*********************************************************************/
/*                                                                   */
/*      dir_li -- gibt einen Listendeskriptor mit TY_UNDEF   und     */
/*                                                TY_STRING  aus.    */
/*********************************************************************/
static dir_li(Desc)

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
} /**** end of function dir_li ****/
#if LARGE

/*********************************************************************/
/*                                                                   */
/*      dir_mvt -- gibt einen Matrizen- bzw. einen Vektor-           */
/*                 transponierten Vektor Deskiptor aus.              */
/*                                                                   */
/*********************************************************************/
static dir_mvt(Desc,class1)

T_DESCRIPTOR Desc;
        char class1;
{
  outint("nrows",R_MVT(Desc,nrows,class1));
  outint("ncols",R_MVT(Desc,ncols,class1));
  outhex("PTDD @ to Direct Descriptor",R_MVT(Desc,ptdd,class1));
  outhex("PTDV @ to Data Vector",R_MVT(Desc,ptdv,class1));
  if (mode && (R_DESC(Desc,ref_count) > 0 ))
     ptdv(R_MVT(Desc,ptdv,class1),R_DESC(Desc,type),
          R_MVT(Desc,nrows,class1) * R_MVT(Desc,ncols,class1));
} /**** end of function dir_mvt ****/
#endif 




/*********************************************************************/
/*                                                                   */
/*      dir_e -- steuert die Ausgabe eines Deskriptors der Klasse    */
/*               C_EXPRESSION.                                       */
/*                                                                   */
/*********************************************************************/
static dir_e   (Desc)

T_DESCRIPTOR Desc;

{
  switch(R_DESC(Desc,type))

        {

        case TY_SUB   : e_func(Desc);
                        break;
        case TY_CLOS  : e_clos(Desc);
                        break;
        case TY_P_CLOS  : e_prim_clos(Desc);
                        break;
        case TY_COND  :  e_cond(Desc);
                        break;
        case TY_VAR   :  e_var(Desc);
                        break;
        case TY_EXPR  :  e_expr(Desc);
                        break;
        case TY_NAME  :  e_name(Desc);
                        break;
        case TY_FNAME  :  e_fname(Desc);
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
        default       : outhex2(" illegal Type", R_DESC(Desc,type));
                        return(hexdump(Desc));
        }
}
/*********************************************************************/
/*                                                                   */
/*      pm_e_switch -- gibt einen switch-Deskriptor aus.              */
/*                                                                   */
/*********************************************************************/ 
 static pm_e_switch    (Desc)

T_DESCRIPTOR Desc;

{
 STACKELEM type;

 type = R_SWITCH(Desc,casetype);

 if (T_PM_SWITCH(type))
    out("Konstruktor","case");
 else if (T_PM_REC(type))
    out("Konstruktor","case_rec");
 else if (T_PM_ECASE(type))
    out("Konstruktor","case_e");
 else if (T_PM_NCASE(type))
    out("Konstruktor","n-stelliges case");
 else if (T_PM_FCASE(type))
    out("Konstruktor","case_f");
 else out("Konstruktor","unknown_case");

 outint("Zahl der when-ausdruecke", R_SWITCH(Desc, nwhen));
 outint("Anzahl der benoetigten Argumente", R_SWITCH(Desc, anz_args));
 outhex("PTE @ Pointer to data vector", R_SWITCH(Desc,ptse));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte_s(R_SWITCH(Desc,ptse));

}


/*********************************************************************/
/*                                                                   */
/*                                                                   */
/*********************************************************************/
 static pte_s         (adr)

int  *adr;

{

int i;

PWNILVAL(adr);

for (i = 1; i <= ( adr[0] - 2 ); )   /* Ausgabe in Stackelementdarstellung */
{
 prstel(IntDumpFile,adr[i++],SHORT);
 fprintf(IntDumpFile,"\n");
}

for (i = 1; i <= ( adr[0] - 2 ) ;i++ ) /* Rekursives Absteigen im Deskriptorenbaum */
  if (T_POINTER(adr[i]) && (isdesc(adr[i])))
     DescDump(IntDumpFile,adr[i],mode-1);
}


/*********************************************************************/ 
/*                                                                   */
/*      pm_e_match -- gibt einen match-Deskriptor aus.              */
/*                                                                   */
/*********************************************************************/ 
 static pm_e_match    (Desc)

T_DESCRIPTOR Desc;

{

 if (R_MATCH(Desc,guard) != 0 )
    {
      outhex("guard expresssion",R_MATCH(Desc,guard));
      if (mode && (R_DESC(Desc,ref_count) > 0 ))
         pte(R_MATCH(Desc,guard))  ;
    }

 outhex("body expresssion",R_MATCH(Desc,body));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
     pte(R_MATCH(Desc,body))  ;

 if (R_MATCH(Desc,code) != 0 )
    {
     outhex("Matchcode" ,R_MATCH(Desc, code));
     if (mode && (R_DESC(Desc,ref_count) > 0 ))
         pm_pt_match(R_MATCH(Desc, code))  ;
    }


}


/*********************************************************************/ 
/*                                                                   */
/*      pm_e_nomat -- gibt einen Nomatchdeskriptor aus.              */
/*                                                                   */
/*********************************************************************/ 
static pm_e_nomat     (Desc)

T_DESCRIPTOR Desc;

{

 outint("act_nomat",R_NOMAT(Desc,act_nomat));


 switch (R_NOMAT(Desc,reason))
 {
   case 1: out("reason","kein Pattern passt oder guard ist FALSE");                    break;
   case 2: out("reason","Matching war UNDECIDED");                                     break;
   case 3: out("reason","guard konnte weder zu TRUE noch zu FALSE reduziert werden");  break;
   case 4: out("reason","Reduktionszaehler waehrend der guard-Berechnung abgelaufen"); break;
   default: out("reason","no reason?");                                                break;
 }

 outhex("zeigt auf guard, body und Argument ",R_NOMAT(Desc,guard_body));
     if(R_NOMAT(Desc,guard_body)!= 0)
               if (mode && (R_DESC(Desc,ref_count) > 0 ))
                    pte(R_NOMAT(Desc,guard_body));

 outhex("zeigt auf Swi.deskr. ",R_NOMAT(Desc,ptsdes));
 if (isdesc(R_NOMAT(Desc, ptsdes)))
    if (mode && (R_DESC(Desc,ref_count) > 0 ))
        DescDump(IntDumpFile,R_NOMAT(Desc,ptsdes),mode-1);


}


/*********************************************************************/
/*                                                                   */
/*      e_func -- gibt einen Funktionsdeskriptor aus.                */
/*                                                                   */
/*********************************************************************/

 static e_func     (Desc)

T_DESCRIPTOR Desc;

{
 outint("Edit",R_FUNC(Desc,special));
 outint("nargs",R_FUNC(Desc,nargs));

 outhex("Pointer to bound variable names",R_FUNC(Desc,namelist));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte_s(R_FUNC(Desc,namelist));

 outhex("Pointer to expression",R_FUNC(Desc,pte));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte(R_FUNC(Desc,pte));

}


/*********************************************************************/ 
/*                                                                   */
/*      e_clos -- gibt einen Closuredeskriptor aus.                */
/*                                                                   */
/*********************************************************************/ 

 static e_clos     (Desc)

T_DESCRIPTOR Desc;

{
 COUNT       ndargs ;

 outint("ndargs",R_CLOS(Desc,ndargs));
 outint("nfv",R_CLOS(Desc,nfv));

 outhex("Pointer to arguments",R_CLOS(Desc,pta));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
/* Die vorhandenen Argumente stehen an der Stelle                  */
/* pta[0] (=Anzahl der gebundenen und der relativ freien Variablen)*/
/* - ndargs (= Anzahl 'fehlender' Argumente)                       */
/* - nfv(= Anzahl der relativ freien Variablen).                   */   
/* Die Argumente wurden 'von hinten' in die Liste eingetragen.     */


/* ist ndargs negativ , so wurde die Closure angelegt , um ein     */
/* Conditional zu schliessen   => die gesamte pta - Liste muss     */
/* ausgegeben werden                                               */

    {
     ndargs=R_CLOS(Desc,ndargs);

     ptdv(R_CLOS(Desc,pta) + ndargs+1,R_DESC(Desc,type),R_CLOS(Desc,pta)[0] - ndargs - R_CLOS(Desc,nfv));

   }

 if (R_CLOS(Desc,nfv) != 0)
    {
    outhex("free variable list",R_CLOS(Desc,pta));
     if (mode && (R_DESC(Desc,ref_count) > 0 ))
   /* Die relativ freien Variablen stehen an der Stelle               */
   /* pta[0] (=Anzahl der gebundenen und der relativ freien Variablen)*/
   /* - nfv(= Anzahl der relativ freien Variablen) + 1.               */   

     ptdv(R_CLOS(Desc,pta) + (R_CLOS(Desc,pta)[0] - R_CLOS(Desc,nfv) + 1),R_DESC(Desc,type),R_CLOS(Desc,nfv));
    }

 outhex("Pointer to function descriptor",R_CLOS(Desc,ptfd));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
    if (isdesc(R_CLOS(Desc,ptfd)))
       DescDump(IntDumpFile,R_CLOS(Desc,ptfd),mode-1);

}

/*********************************************************************/ 
/*                                                                   */
/*      e_prim_clos -- gibt einen Closure-Descriptor fuer            */
/*                     primitive Funktionen aus.                     */
/*                                                                   */
/*********************************************************************/ 

 static e_prim_clos     (Desc)

T_DESCRIPTOR Desc;

{
 COUNT           ndargs;
 STACKELEM  func,*p_arg;

 outint("ndargs",R_P_CLOS(Desc,ndargs));

 outhex("Pointer to arguments",R_P_CLOS(Desc,pta));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
/* Die vorhandenen Argumente stehen an der Stelle                  */
/* pta[0] (=Anzahl der gebundenen und der relativ freien Variablen)*/
/* - ndargs (= Anzahl 'fehlender' Argumente)                       */
/* - nfv(= Anzahl der relativ freien Variablen).                   */   
/* Die Argumente wurden 'von hinten' in die Liste eingetragen.     */

     {
       ndargs=R_P_CLOS(Desc,ndargs);

       ptdv(R_P_CLOS(Desc,pta) + ndargs+1,R_DESC(Desc,type),R_P_CLOS(Desc,pta)[0] - ndargs );
     }

 p_arg = R_P_CLOS((Desc),pta);

 func = (STACKELEM)p_arg[1];
/* func ist das Stackelement,das die primitive Funktion bezeichnet */
/* Ausgabe des Namens der hinter der Closure liegenden Funktion:   */

 prstel(IntDumpFile,func,SHORT);
 fprintf(IntDumpFile,"\n");


}

/*********************************************************************/
/*                                                                   */
/*      e_cond -- gibt einen cond-Deskriptor aus.                    */
/*                                                                   */
/*********************************************************************/

 static e_cond    (Desc)

T_DESCRIPTOR Desc;

{
 outhex("PTTE @ Pointer to then expression",R_COND(Desc,ptte));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte(R_COND(Desc,ptte));

 outhex("PTEE @ Pointer to else expression",R_COND(Desc,ptee));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte(R_COND(Desc,ptee));

}

/*********************************************************************/
/*                                                                   */
/*      e_expr -- gibt einen expression-Deskriptor aus.              */
/*                                                                   */
/*********************************************************************/
 static e_expr    (Desc)

T_DESCRIPTOR Desc;

{
 outhex("PTE @ Pointer to expression",R_EXPR(Desc,pte));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte(R_EXPR(Desc,pte));

}


/*********************************************************************/
/*                                                                   */
/*      e_name -- gibt einen Namensdeskriptor aus.                   */
/*                                                                   */
/*********************************************************************/
 static e_name    (Desc)

T_DESCRIPTOR Desc;

{
 outhex("PTN @ Pointer to name",R_NAME(Desc,ptn));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte(R_NAME(Desc,ptn));

}

/*********************************************************************/
/*                                                                   */
/*      e_fname -- gibt einen Namensdeskriptor aus.                   */
/*                                                                   */
/*********************************************************************/
 static e_fname    (Desc)

T_DESCRIPTOR Desc;

{
 outhex("PTN @ Pointer to name",R_FNAME(Desc,ptn));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte(R_FNAME(Desc,ptn));

}


/*********************************************************************/
/*                                                                   */
/*      e_var -- gibt einen Variablendeskriptor aus.                 */
/*                                                                   */
/*********************************************************************/
static e_var     (Desc)

T_DESCRIPTOR Desc;

{
 outint("nlabar",R_VAR(Desc,nlabar));

 outhex("PTND @ Pointer to Namedescriptor",R_VAR(Desc,ptnd));
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
     DescDump(IntDumpFile,R_VAR(Desc,ptnd),mode-1);
}

/*********************************************************************/
/*                                                                   */
/*  e_lrec -- gibt einen LETREC-Deskriptor aus.              * CS *  */
/*                                                                   */
/*********************************************************************/
                                                                       /* CS */
 static e_lrec     (Desc)                                              /* CS */
                                                                       /* CS */
T_DESCRIPTOR Desc;                                                     /* CS */
                                                                       /* CS */
{                                                                      /* CS */
 outint("nfuncs",R_LREC(Desc,nfuncs));                                 /* CS */
                                                                       /* CS */
 outhex("Pointer to LETREC variable names",R_LREC(Desc,namelist));     /* CS */
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 pte(R_LREC(Desc,namelist));                                           /* CS */
                                                                       /* CS */
 outhex("Pointer to function definitions",R_LREC(Desc,ptdv));          /* CS */
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
 ptdv(R_LREC(Desc,ptdv)+1,R_DESC(Desc,type),R_LREC(Desc,nfuncs));        /* CS */
                                                                       /* CS */
}                                                                      /* CS */

/*********************************************************************/
/*                                                                   */
/*  e_lrec_ind -- gibt einen LETREC_ind-Deskriptor aus.      * CS *  */
/*                                                                   */
/*********************************************************************/
                                                                       /* CS */
 static e_lrec_ind     (Desc)                                          /* CS */
                                                                       /* CS */
T_DESCRIPTOR Desc;                                                     /* CS */
                                                                       /* CS */
{                                                                      /* CS */
 outint("index",R_LREC_IND(Desc,index));                               /* CS */
                                                                       /* CS */
 outhex("Pointer to LETREC",R_LREC_IND(Desc,ptdd));                    /* CS */
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
     DescDump(IntDumpFile,R_LREC_IND(Desc,ptdd),mode-1);               /* CS */

 outhex("Pointer to FUNCTION",R_LREC_IND(Desc,ptf));                    
 if (mode && (R_DESC(Desc,ref_count) > 0 ))
     DescDump(IntDumpFile,R_LREC_IND(Desc,ptf),mode-1);               /* CS */

}                                                                      /* CS */

/*********************************************************************/
/*                                                                   */
/*      pte -- gibt einen beliegen Ausdruck aus dem Heap, dem        */
/*             dessen Laenge vorangestellt ist, aus.                 */
/*                                                                   */
/*********************************************************************/
 static pte         (adr)

int  *adr;

{

int i;

PWNILVAL(adr);

for (i = 1; i <= adr[0]; )   /* Ausgabe in Stackelementdarstellung */
{
 prstel(IntDumpFile,adr[i++],SHORT);
 fprintf(IntDumpFile,"\n");
}

for (i = 1; i <= adr[0];i++ ) /* Rekursives Absteigen im Deskriptorenbaum */
  if (T_POINTER(adr[i]) && (isdesc(adr[i])))
     DescDump(IntDumpFile,adr[i],mode-1);
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
static ptdv    (adr,typ,zahl)
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
#if LARGE
  case TY_DIGIT :         /* eine Liste von digit-Deskriptoren */
       for (i=adr;i<adr+zahl;i++)
         fprintf(IntDumpFile,"%x ",*i);
       fprintf(IntDumpFile,"\n");
       if (mode)
         for (i=adr;i<adr+zahl;i++)
           DescDump(IntDumpFile,*i,mode-1);
      break;
  case TY_MATRIX    :     /* eine Liste von Matrizen */
      for (i=adr;i<adr+zahl;i++)
      DescDump(IntDumpFile,*i,mode-1);
      fprintf(IntDumpFile,"\n");
      break;
#endif 
  case TY_LREC:                                                        /* CS */
  case TY_UNDEF:          /* eine Liste von Elementen unbekannten Typs */
  case TY_STRING:
  case TY_CLOS:
  case TY_P_CLOS:

       for (i=adr;i<adr+zahl;i++)
          prstel(IntDumpFile,*i,SHORT);
       fprintf(IntDumpFile,"\n");
       for (i=adr;i<adr+zahl; i++)
         if (T_POINTER(*i) && (isdesc(*i)))
           DescDump(IntDumpFile,*i,mode-1);
      break;
  }
}

/*********************************************************************/
/*                                                                   */
/*      outint -- druckt einen kommentierten Integerwert aus.        */
/*                                                                   */
/*********************************************************************/

static outint(s1,i)

char * s1 ;
int i;
{
fprintf(IntDumpFile,"%40.40s : %d \n",s1,i);
}

/*********************************************************************/
/*                                                                   */
/*      outdouble -- druckt einen kommentierten Doublewert aus.       */
/*                                                                   */
/*********************************************************************/

static outdouble(s1,i)

char * s1 ;
double i;
{
fprintf(IntDumpFile,"%40.40s : %f \n",s1,i);
}

/*********************************************************************/
/*                                                                   */
/*      outhex2 -- druckt die kommentierten letzten zwei Hexziffern  */
/*                 einer Zahl aus.                                   */
/*********************************************************************/

static outhex2(s1,x)

char * s1 ;
int  x;
{
fprintf(IntDumpFile,"%40.40s : %2.2x \n",s1,x&0x000000ff);
}

/*********************************************************************/
/*                                                                   */
/*      outhex -- druckt einen kommentierten Hexadezimalwert aus.    */
/*                                                                   */
/*********************************************************************/

static outhex(s1,x)

char * s1 ;
int x;
{
fprintf(IntDumpFile,"%40.40s : %8.8x \n",s1,x);
}

/*********************************************************************/
/*                                                                   */
/*      out -- druckt einen kommentieren String aus.                 */
/*                                                                   */
/*********************************************************************/

static out(s1,s2)

char * s1 ;
char * s2 ;
{
fprintf(IntDumpFile,"%40.40s : %-25s\n",s1,s2);
}

/*********************************************************************/
/*                                                                   */
/* hexdump -- Die 16 Byte eines Deskriptors werden hexadezimal aus-  */
/*            gedruckt, da der Deskriptor nicht identifizierbar war. */
/*                                                                   */
/*********************************************************************/

static hexdump   (Desc  )

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
}      


/***********************************************************************/
static pm_pt_match(adr)

int *adr;

{
  int i;


 outint("Anzahl der benoetigten Argumente", adr[STELLIGKEIT]);

  for (i= 1; i <= adr[VEKTORLAENGE]; i++ )
  {
    switch (adr[i])
    {
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


return;
}

#else
rdescdummy(){;}
#endif /* DEBUG */

/* end of  rdesc.c */
