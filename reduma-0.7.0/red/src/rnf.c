/* $Log: rnf.c,v $
 * Revision 1.9  1996/06/11  15:31:15  rs
 * some DBUG-output removed (sorry ... ;-)
 *
 * Revision 1.8  1996/06/11  15:27:16  rs
 * "mah BS"-calc_nf-bugfix (hopefully)
 *
 * Revision 1.7  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.6  1995/08/15  09:55:15  mah
 * brute force solution for
 * combined sub problem
 *
 * Revision 1.5  1995/06/22  11:06:40  mah
 * buf fix
 *
 * Revision 1.4  1994/10/23  15:08:30  mah
 * tilde bug fix
 *
 * Revision 1.3  1994/06/19  13:22:07  mah
 * Tilde version
 *
 * Revision 1.2  1992/12/16  12:50:21  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/*-----------------------------------------------------------*/
/*                                                           */
/*  ak                                    06.03.1992         */
/*                                                           */
/* file: rnf.c                                               */
/*                                                           */
/* Modul zur Reduktion eines Ausdrucks zur Normalform        */
/* ==> vollstaendige Realisierung des Lambda-Kalkuels        */
/*                                                           */
/* Iterative Version:                                        */
/* Der hilf-Stack und der i-Stack werden als "manuell"       */
/* verwalteter Laufzeitstack benutzt.                        */
/* Dabei werden auf dem i-Stack Returnadressen und auf dem   */
/* hilf-Stack Parameter und lokale Variable abgelegt.        */
/*-----------------------------------------------------------*/

/* externals:  (vgl. rnf.h)                                  */

/*             reduce_to_nf ()                               */

/* internals:                                                */

/*             calc_nf (expr_ptr)                            */
/*             vars_to_argframe (modus, to, from, namelist)  */
/*             create_freevar (modus, index, namelist)       */
/*             get_namelist (ptr_tysub_desc)                 */
/*             call_code (code_ptr)                          */
/*             reconstr_abstr (expr_ptr, body)               */
/*             get_abstraction_arity (heap_ptr)              */
/*             get_abstraction_namelist (heap_ptr)           */
/*             get_arg_expr (heap_ptr, n)                    */
/*             get_abstraction_code (heap_ptr)               */
/*             constr_abstr (expr_ptr, res)                  */
/*             reconstr_arg (nr, heap_ptr, res)              */
/*             set_get_bindings (modus, delta, var_desc)     */
/*             remove_bindings (from, to, namelist)          */
/*             set_bindings (from, to, namelist)             */
/*             free_S_v                                      */
/*             replace (nr, heap_ptr, res)                   */
/*             correct_namelist(namelist)                    */

/* Makros:                                                   */

/*             MAKEDESC(ptr,refcount,class,type)            */
/*             GET_HEAP(n,p)                                 */

#if RED_TO_NF

#include "rstdinc.h"    /* fuer START_MODUL, END_MODUL */
#include "rstackty.h"   /* fuer Stackoperationen */
#include "rstelem.h"    /* fuer Stackelemente */
#include "rheapty.h"    /* fuer Heapstrukturen */
#include "rinter.h"     /* Codeausfuehrung, Befehlssatz */
#include "dbug.h"

extern int Normalform;  /* Flag zur Kennzeichnung des Reduktionsziels */
                        /* NF = >0, WHNF = 0 */
extern StackDesc S_e;   /* Stack E/H */
extern StackDesc S_m;   /* Stack m */
extern StackDesc S_a;   /* Stack A */
extern StackDesc S_v;   /* Stack v */
extern StackDesc S_i;   /* Stack i */
extern StackDesc S_hilf;/* Stack hilf */
extern StackDesc S_m1;  /* Stack m1 (Inkarnationsstack) */
#if WITHTILDE
extern StackDesc S_tilde; /* Stack T */
#endif /* WITHTILDE */
extern _redcnt;         /* Reduktionszaehler */
extern BOOLEAN _count_reductions; /* Zaehlen von Reduktionen? */
extern int Changes_Default;   /* editor, wg. verpointertem wiederaufsetzen, 0:ja, 1:nie */
extern PTR_DESCRIPTOR newdesc ();      /* Anlegen eines Deskriptors (in rheap.c) */
extern post_mortem ();                 /* "geordneter" Abbruch */

/* RS 30/10/92 */ 
extern void stack_error();               /* rstack.c */
extern void inter();                     /* rinter.c */
extern int newheap();                    /* rheap.c */
extern void cutheap();                   /* rheap.c */
/* END of RS 30/10/92 */ 

/* RS 6.11.1992 */ 
extern char *get_heap();          /* rscavenge.c */
/* END of RS 6.11.1992 */ 

#if DEBUG
extern void start_modul(); /* TB, 4.11.1992 */        /* rbreak.c */
extern void end_modul(); /* TB, 4.11.1992 */          /* rbreak.c */
extern void DescDump(); /* TB, 4.11.1992 */           /* rdesc.c */
extern STACKELEM readstack(); /* TB, 4.11.1992 */     /* rstack.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */    /* rstack.c */
extern STACKELEM popstack(); /* TB, 4.11.1992 */      /* rstack.c */
extern void pushstack(); /* TB, 4.11.1992 */          /* rstack.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */    /* rheap.c */
#endif

static INSTR termination[] = { I_EXT };  /* Codesequenz zur Terminierung */

#ifdef NotUsed
/* dg 07.05.92 MAKEDESC und GET_HEAP in rheapty.h definiert */
/* Makro zum Anlegen eines Deskriptors (kopiert aus preproc.c) */
#define MAKEDESC(p,r,c,t) if ((p = newdesc()) != 0) { \
                             DESC_MASK(p,r,c,t); \
                           } \
                           else post_mortem("preproc: heap overflow")

/* Makro zum Allozieren eines neuen Heapsegments (kopiert aus rinter.c) */

#define GET_HEAP(n,p)     if ((newheap(n,p)) == 0) \
                            post_mortem("rnf: heap overflow (newheap)"); \
                          else
#endif /* NotUsed     auskommentiert RS 3.12.1992 */

/* Zugriffsmodi auf den Bindungslevel-Zaehler */

#define READING 0
#define WRITING 1

/* Booleans */

#define TRUE 1
#define FALSE 0

/* Sprunglabel (Returnadressen) */

#define T_LABEL char

#define L_EXIT      '0'
#define L_FUNC_COMB '1'
#define L_CLOS_COMB '2'
#define L_CLOS_COND '3'
#define L_CLOS_PRIM '4'
#define L_CLOS_CASE '5'
#define L_CLOS_CLOS '6'
#define L_EXPRE     '7'
#define L_LIST_UNDF '8'
#define L_CONS_HEAD '9'
#define L_CONS_TAIL 'a'

/* sollte wieder geloescht werden: */
void dummy()
{
  START_MODUL ("dummy");
  END_MODUL   ("dummy");
}

#if WITHTILDE
/*-----------------------------------------------------------*/
/* void correct_namelist                                     */
/*-----------------------------------------------------------*/

void correct_namelist(name)
PTR_HEAPELEM name;
{
  int last, i, strdesc, anzahl;

  last     = (int)name[0];
  strdesc  = (int)name[1];
  anzahl   = (int)name[strdesc]-1; /* anzahl der vom aeussersten sub gebundenen variablen */

/*  DEC_REFCNT(name[strdesc-1]); */

  if (anzahl == 1){        /* dieses sub verschwindet vollst"andig */
    /* name[0] position des letzten eintrags                         */
    /* name[1] position des ersten eintrags zur strukturbeschreibung */
    name[0]-=2;            /* 1 verweis auf namensdesciptor, 1 struktureintrag */
    name[1]--;             /* 1 verweis auf namensdesciptor */

#if 0
    /* jetzt ab dem zweiten eintrag einen eintrag vorr"ucken */
    for (i=3; i<strdesc; i++) {
      name[i-1] = name[i];
    }
#endif
    /* strukturbeschreibung zwei vorr"ucken:
       einen, weil der verweis auf die erste benutzte variable, der als
       letzte in der variablenliste abgelegt ist, gel"oscht werden muss.
       den zweiten, weil, ein sub wegf"allt und daf"ur keine struktur-
       beschreibung mehr ben"otigt wird */

    for (i=strdesc+1; i<=last; i++)
      name[i-2] = name[i];
  }
  else {
    name[strdesc]--;              /* eine var weniger gebunden       */
    for (i=strdesc; i<=last; i++) /* verweis auf letzte var l"oschen */
      name[i-1] = name[i];
    name[1]--;                    /* ein verweis weniger, deswegen
				     struktur ein feld weiter vorne  */
    name[0]--; 
  }
}
#endif /* WITHTILDE */

/*-----------------------------------------------------------*/
/* void free_S_v ()                                          */ 
/*                                                           */ 
/* Gibt den gesamten V-Stack wieder frei.                    */
/* Auf dem Stack liegt jeweils ein Variablen-Deskriptor,     */
/* gefolgt von einem Integerwert.                            */
/* Aufrufe in:                                               */
/*             reduce_to_nf ()                               */
/*-----------------------------------------------------------*/

void free_S_v ()
{
   STACKELEM x;

   START_MODUL ("free_S_v");

   do
   {
     x = READSTACK (S_v);
     PPOPSTACK (S_v); /* TB, 30.10.92 */
     if (x != KLAA)
       PPOPSTACK (S_v); /* TB, 30.10.92 */ 
   }
   while (x != KLAA);

   END_MODUL ("free_S_v");
}

/*-----------------------------------------------------------*/
/* int set_get_bindings (modus, delta, var_desc)             */
/*                                                           */
/* Addiert auf den Zaehler fuer vorhandene Bindungen den     */
/* Wert delta auf.                                           */
/* Resultat ist der aktuelle Zaehlerwert des Deskriptors     */
/*                  + delta                                  */
/* modus == READING : Kein Update des Zaehlers.              */
/* Aufrufe in:                                               */
/*             create_freevar ()                             */
/*             set_bindings ()                               */
/*             remove_bindings ()                            */
/*-----------------------------------------------------------*/

int set_get_bindings (modus, delta, var_desc)
int modus;
int delta;
STACKELEM var_desc;
{
   STACKELEM x; /* Variablendeskriptor */
   STACKELEM v=(STACKELEM)0; /* Bisheriger Wert */ /* Initialisierung von TB, 6.11.192 */
   STACKELEM h; /* Hilfsvariable */
   int endloop; /* Abbruchbedingung */

   START_MODUL ("set_get_bindings");

   PUSHSTACK (S_hilf, KLAA);
   endloop = FALSE;           

   x = READSTACK (S_v);
  
   while (!endloop)
   { 
      if (x == KLAA)
      {  /* Neuer Eintrag */

         if (modus == WRITING)
         {
           PUSHSTACK (S_v, TAG_INT(delta)); 
           PUSHSTACK (S_v, var_desc);
         }
         v = (STACKELEM)delta;
         endloop = TRUE;
      }
      else
        if (x == var_desc)
        { /* Deskriptor gefunden */

          PPOPSTACK (S_v); /* TB, 30.10.92 */
          v = READSTACK (S_v);
          PPOPSTACK (S_v); /* TB, 30.10.92 */
          h = v;
          v = (STACKELEM)((int)((VAL_INT(v)) + delta));
          if (modus == WRITING)
            PUSHSTACK (S_v, TAG_INT((int)v));
          else
            PUSHSTACK (S_v, h);
          PUSHSTACK (S_v, x);
          endloop = TRUE;
        }
        else
        { /* Suche weiter, rette Stackeiontraege auf hilf-Stack */

          h = POPSTACK (S_v);
          PUSHSTACK (S_hilf, h);
          h = POPSTACK (S_v);
          PUSHSTACK (S_hilf, h);
          x = READSTACK (S_v);
        }
   }

   x = POPSTACK (S_hilf);

   while (x != KLAA) /* Schiebe alle Elemente vom hilf-Stack zurueck auf v-Stack */
   {
      PUSHSTACK (S_v, x);
      x = POPSTACK (S_hilf);
      PUSHSTACK (S_v, x);
      x = POPSTACK (S_hilf);
   } 

   END_MODUL ("set_get_bindings");

   return ((int)v);
}

/*-----------------------------------------------------------*/
/* STACKELEM create_freevar (modus, index, namelist)         */
/*                                                           */
/* Erzeugt ein Stackelement zur Representation einer freien  */
/* Variablen. Der Eintrag nlabar wird auf den Level  gesetzt,*/
/* der den Bindungslevel der Variablen beschreibt.           */
/* (Anzahl der zu entfernenden Schutzstriche, wenn die       */
/* Variable als frei angesehen wird.)                        */
/* Ein Verweis auf den Namesdeskritor steht auf Position     */
/* index in namelist.                                        */
/* Der Namesdeskriptor einer gebundenen Variablen wird im    */
/* Sharing benutzt! (=> refcount inkrementieren)             */
/* Aufrufe in:                                               */
/*              vars_to_argframe ()                          */
/*-----------------------------------------------------------*/

STACKELEM create_freevar (modus, index, namelist)
int modus,
    index;
PTR_HEAPELEM namelist;
{
    PTR_DESCRIPTOR var_desc;
 
    START_MODUL ("create_freevar");
 
    MAKEDESC(var_desc, 1, C_CONSTANT, TY_VAR);

#if WITHTILDE
    L_VAR ((*var_desc), ptnd) = (T_PTD)namelist[index + 1];
#else
    L_VAR ((*var_desc), ptnd) = (T_PTD)namelist[index];    
#endif /* WITHTILDE */
    L_VAR ((*var_desc), nlabar) = -set_get_bindings (modus, 1, (STACKELEM)R_VAR((*var_desc), ptnd));
#if WITHTILDE
    T_INC_REFCNT (R_VAR ((*var_desc), ptnd)); /* Reference Counter erhoehen */
#else
    INC_REFCNT (R_VAR ((*var_desc), ptnd)); /* Reference Counter erhoehen */
#endif /* WITHTILDE */
    END_MODUL ("create_freevar");

    return ((STACKELEM)var_desc);
}

/*-----------------------------------------------------------*/
/* void vars_to_argframe (modus,from,to,namelist)            */
/*                                                           */
/* Legt auf dem I-Stack (to-from+1) freie Variable ab.       */
/* Der Eintrag nlabar im Deskriptor der Variablen wird       */
/* auf negativen Wert gesetzt, so dass urspruengliche        */ 
/* Bindung erhalten bleibt.                                  */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

void vars_to_argframe (modus, from, to, namelist)
int modus,
    from,
    to;
PTR_HEAPELEM namelist;
{
   int i;

   START_MODUL ("vars_to_argframe"); 

   for (i = from; i <= to; i++)
   {
      /* Lege Variable zunaechst auf den Stack A */
      /* Dies ist noetig, um die Bindungslevels  */
      /* korrekt berechnen zu koennen (Level     */
      /* werden von "aussen nach innen" berechnet*/
      /* vgl. set_get_bindings).                 */
      /* Innerste Bindungen stehen zuerst in der Namensliste */

      PUSHSTACK(S_a, create_freevar (modus, (to-i+1), namelist));
   }

   for (i = to; i >= from; i--)
   {
      /* Inkarnationsstack heisst m1! */

      PUSHSTACK(S_m1, READSTACK (S_a));
      PPOPSTACK (S_a); /* TB, 30.10.92 */
   }

   END_MODUL ("vars_to_argframe"); 
}

/*-----------------------------------------------------------*/
/* void set_bindings (from, to, namelist)                    */
/*                                                           */
/* Inkrmentiert den Zaehler fuer die Bindungen der           */
/* Deskriptoren in namelist.                                 */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

void set_bindings (from, to, namelist)
int from,
    to;  
PTR_HEAPELEM namelist;
{
   int i;
 
   START_MODUL ("set_bindings");
 
   for (i = from; i <= to; i++)
     set_get_bindings (WRITING, 1, (STACKELEM)namelist [i]);
 
   END_MODUL ("set_bindings");
}
 
/*-----------------------------------------------------------*/
/* void remove_bindings (from, to, namelist)                 */
/*                                                           */
/* Dekrementiert den Zaehler fuer die Bindungen der          */
/* Deskriptoren in namelist.                                 */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

void remove_bindings (from, to, namelist)
int from,
    to;
PTR_HEAPELEM namelist;
{
   int i;

   START_MODUL ("remove_bindings");

   for (i = from; i <= to; i++)
#if WITHTILDE
     set_get_bindings (WRITING, -1, (STACKELEM)namelist [i + 1]);
#else
     set_get_bindings (WRITING, -1, (STACKELEM)namelist [i]);
#endif /* WITHTILDE */

   END_MODUL ("remove_bindings");
}

/*-----------------------------------------------------------*/
/* PTR_HEAPELEM get_namelist (ptr_tysub_desc)                */
/*                                                           */
/* Liefert Verweis auf die Heapstruktur, die als             */
/* Liste die Verweise auf die Namensdeskriptoren             */
/* haelt.                                                    */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*             get_abstraction_namelist ()                   */
/*-----------------------------------------------------------*/

PTR_HEAPELEM get_namelist (ptr_tysub_desc)
T_PTD ptr_tysub_desc;
{

   PTR_HEAPELEM namelist;

   START_MODUL ("get_namelist");

   namelist = R_FUNC(*ptr_tysub_desc, namelist);  

   END_MODUL ("get_namelist");

   return (namelist);
}

/*-----------------------------------------------------------*/
/* void call_code (code_ptr)                                 */
/*                                                           */
/* Ausfuehrung der Codesequenz auf die der Pointer code_ptr  */
/* zeigt. Um die Ausfuehrung geordnet zu terminieren, ist    */
/* auf dem Return-Stack die Adresse einer EXIT-Instruktion   */
/* abzulegen.                                                */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

void call_code (code_ptr)
INSTR *code_ptr;
{
   START_MODUL ("call_code");

   PUSHSTACK(S_m, (INSTR *)termination);
   if (!_count_reductions) 
     post_mortem ("Reduce to NF works only with reduction counting! (redcnt < MaxRedcnt, see red.setup)");
   inter(code_ptr);      /* Fuehre Code aus (vgl. rinter.c) */

   END_MODUL ("call_code");
}

/*-----------------------------------------------------------*/
/* void replace (nr, heap_ptr, res)                          */
/*                                                           */
/* Ersetzt den Eintrag an Position nr in der Struktur        */
/* auf die heap_ptr zeigt, durch einen Verweis auf res.      */
/* Aufrufe in:                                               */
/*             reconstr_abstr ()                             */
/*             constr_abstr ()                               */
/*-----------------------------------------------------------*/

void replace (nr, heap_ptr, res)
int nr;
PTR_HEAPELEM heap_ptr;
STACKELEM res;
{
   T_PTD body_desc_ptr;        /* Rumpfausdruck (Pointer auf) ein Deskriptor */
 
   START_MODUL ("replace");

   if (T_POINTER(res))
   {
      body_desc_ptr = (T_PTD)res;
 
      if (R_DESC(*body_desc_ptr, class) == C_FUNC) /* Code-Deskriptor */
      {
         if (R_DESC(*body_desc_ptr, type) == TY_COMB) /* Abstraktion */
         {
           body_desc_ptr = R_COMB(*body_desc_ptr, ptd);
           heap_ptr[1] = (T_HEAPELEM)body_desc_ptr; 
           INC_REFCNT (body_desc_ptr);
         }
         else
         /* (R_DESC(*body_ptr, type) == TY_CLOS)    Abschluss (z.B. Conditional, part.Appl.primitiver Funktionen) */
         {  
           heap_ptr[1] = (T_HEAPELEM)res;
           INC_REFCNT ((T_PTD)res);
         }
      }
      else
      {
         heap_ptr[1] = (T_HEAPELEM)res;
         INC_REFCNT ((T_PTD)res);
      }
   }
   else
      heap_ptr[1] = (T_HEAPELEM)res;

   END_MODUL ("replace");
} 
 
/*-----------------------------------------------------------*/
/* T_PTD reconstr_abstr (expr_ptr, body)                     */
/*                                                           */
/* expr_ptr zeigt auf einen Code-Deskriptor (type = COMB)    */
/* der einen Verweis (ptd) auf einen Abstraktionsknoten      */
/* (type = TY_SUB) enthaelt. In diesem Abstraktionsknoten    */
/* wird der Verweis im Heap auf den Abstraktionsrumpf (pte)  */
/* durch einen Verweis auf den durch den Deskriptor body     */
/* repraesentireten Ausdruck ersetzt. Ist body ein Code-     */
/* Deskriptor, so wird dieser "uebersprungen" und stattdessen*/
/* der Ausdruck (ptd), der durch body bestimmt ist gewaehlt. */
/* Die Heapstruktur wird entsprechend verkuerzt.             */
/* Der Graph einer LETREC-gebundenen Abstraktion wird im     */
/* Sharing benutzt und darf daher nicht destruktiv ueber-    */
/* schrieben werden.                                         */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

T_PTD reconstr_abstr (expr_ptr, body)
T_PTD expr_ptr;
STACKELEM body;
{
   PTR_HEAPELEM heap_ptr;
   PTR_DESCRIPTOR func_desc;  /* Neuer Abstraktionsknoten */
   T_PTD res;
 

   START_MODUL ("reconstr_abstr");

   if (R_DESC(*(R_COMB(*expr_ptr, ptd)), type) == TY_LREC_IND)
   {
      MAKEDESC(func_desc, 1, C_EXPRESSION, TY_SUB);
 
      L_FUNC (*func_desc, special) = R_FUNC (*(R_LREC_IND(*(R_COMB (*expr_ptr, ptd)), ptf)), nargs);
      L_FUNC (*func_desc, nargs) = (R_FUNC (*(R_LREC_IND(*(R_COMB (*expr_ptr, ptd)), ptf)), nargs));
      L_FUNC (*func_desc, namelist) = R_FUNC (*(R_LREC_IND(*(R_COMB (*expr_ptr, ptd)), ptf)), namelist);
   
      GET_HEAP (2, (A_FUNC(*func_desc, pte)));
 
      heap_ptr = R_FUNC(*func_desc, pte); /* Rumpfausdruck */
 
      replace (1, heap_ptr, body);
 
      heap_ptr[0] = 1;

      res = (T_PTD)func_desc; /* Um Zyklen zu vermeiden, darf hier nicht der Code-Deskriptor zurueckgegeben werden. */
   }
   else
   { 
      heap_ptr = R_FUNC(*(R_COMB (*expr_ptr, ptd)), pte); /* "alter" Rumpfausdruck */

      replace (1, heap_ptr, body);

      cutheap (heap_ptr, 2); /* Verkuerze Heap-Struktur (vgl. rheap.c) */
      heap_ptr[0] = 1;

      res = expr_ptr;
   }

   END_MODUL ("reconstr_abstr");

   return (res);
}

/*-----------------------------------------------------------*/
/* STACKELEM constr_abstr (expr_ptr, res)                    */
/*                                                           */
/* Konstruiert einen Abstraktions-Deskriptor, der sich aus   */
/* einer partiellen Applikation einer Abstraktion auf        */
/* Argumente ergibt. Dabei verweist expr_ptr auf den         */
/* Code-Deskriptor der Closure und res auf den Resultat-     */
/* Ausdruck der Aufloesung der partiellen Applikation.       */
/* Achtung: Es wird kein Code-Deskriptor fuer den            */
/*          resultierenden Abstraktionsknoten angelegt.      */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

#if WITHTILDE
STACKELEM constr_abstr (expr_ptr, res, combined)
int combined;
#else
STACKELEM constr_abstr (expr_ptr, res)
#endif /* WITHTILDE */
T_PTD expr_ptr;
STACKELEM res;
{
   int avail_args;            /* Anzahl der vorhandenen Argumente */
   PTR_HEAPELEM heap_ptr;     /* Heapstruktur der Closure (pta) */
   PTR_DESCRIPTOR func_desc;  /* Neuer Abstraktionsknoten */
   T_PTD comb_desc;           /* Zeiger auf den Code-Deskriptor des Funktionsterms */

   START_MODUL ("constr_abstr"); 

   avail_args = R_CLOS (*expr_ptr, args);
   heap_ptr = R_CLOS (*expr_ptr, pta);
   comb_desc = (T_PTD)heap_ptr[0]; 


   MAKEDESC(func_desc, 1, C_EXPRESSION, TY_SUB);
 
   if (R_DESC(*(R_COMB(*comb_desc, ptd)), type) == TY_LREC_IND) 
   { /* res ist Resultat einer LETREC-gebundenen Abstraktion */
#if WITHTILDE
     if (combined == 0)
       L_FUNC (*func_desc, special) = 0;
     else 
       L_FUNC (*func_desc, special) = 2;
#else
      L_FUNC (*func_desc, special) = R_FUNC (*(R_LREC_IND(*(R_COMB (*comb_desc, ptd)), ptf)), nargs);
#endif /* WITHTILDE */
      L_FUNC (*func_desc, nargs) = (R_FUNC (*(R_LREC_IND(*(R_COMB (*comb_desc, ptd)), ptf)), nargs)) - avail_args;
      L_FUNC (*func_desc, namelist) = R_FUNC (*(R_LREC_IND(*(R_COMB (*comb_desc, ptd)), ptf)), namelist);
   }
   else
   {            
#if WITHTILDE
     if (combined == 0)
       L_FUNC (*func_desc, special) = 0;
     else
       L_FUNC (*func_desc, special) = 2;
#else
      L_FUNC (*func_desc, special) = R_FUNC (*(R_COMB (*comb_desc, ptd)), nargs);
#endif /* WITHTILDE */
      L_FUNC (*func_desc, nargs) = (R_FUNC (*(R_COMB (*comb_desc, ptd)), nargs)) - avail_args;
      L_FUNC (*func_desc, namelist) = R_FUNC (*(R_COMB (*comb_desc, ptd)), namelist);
   }
   GET_HEAP (2, (A_FUNC(*func_desc, pte)));
  
   heap_ptr = R_FUNC(*func_desc, pte); /* Rumpfausdruck */

   replace (1, heap_ptr, res);

   heap_ptr[0] = 1;
 
   END_MODUL ("constr_abstr"); 

   return ((STACKELEM)(func_desc));
}

/*-----------------------------------------------------------*/
/* void reconstr_arg (nr, heap_ptr, res)                     */
/*                                                           */
/* Ersetzt den Verweis an Position nr                        */
/* in der Heap-Struktur auf die der Zeiger verweist, auf     */
/* auf den ptr_heap_ptr zeigt, durch einen Verweis auf res.  */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

void reconstr_arg (nr, heap_ptr, res)
int nr;
PTR_HEAPELEM heap_ptr;
STACKELEM res;
{
   START_MODUL ("reconstr_arg");

   heap_ptr[nr] = (T_HEAPELEM)res;

   END_MODUL ("reconstr_arg");
}

/*-----------------------------------------------------------*/
/* int get_abstraction_arity (heap_ptr)                      */
/*                                                           */
/* Liefert die Stelligkeit einer Abstraktion.                */
/* Ein Verweis auf den Code-Deskriptor der Abstraktion findet*/ 
/* sich in der 1.Komponente der Heapstruktur auf die         */
/* heap_ptr zeigt.                                           */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

int get_abstraction_arity (heap_ptr)
PTR_HEAPELEM heap_ptr;
{
   T_PTD abstr_desc; /* Pointer auf den Code-Deskriptor der Abstraktion */

   START_MODUL ("get_abstraction_arity");

   abstr_desc = (T_PTD)heap_ptr[0];

   END_MODUL ("get_abstraction_arity");

   if (R_DESC(*(R_COMB(*abstr_desc, ptd)), type) == TY_LREC_IND) /* LETREC-gebunden */
      return (R_FUNC(*(R_LREC_IND(*(R_COMB(*abstr_desc, ptd)), ptf)), nargs));
   else 
      return (R_FUNC(*(R_COMB(*abstr_desc, ptd)), nargs));
}

/*-----------------------------------------------------------*/
/* PTR_HEAPELEM get_abstraction_namelist (heap_ptr)          */
/*                                                           */
/* Liefert einen Verweis auf die Heapstruktur, in der als    */
/* Liste Verweise auf die gebundenen Variablen einer         */
/* Abstraktion stehen. Ein Verweis auf den Code-Deskriptor   */
/* der Abstraktion befindet sich in der 1.Komponente         */
/* von heap_ptr.                                             */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

PTR_HEAPELEM get_abstraction_namelist (heap_ptr)
PTR_HEAPELEM heap_ptr;
{
   T_PTD abstr_desc; /* Pointer auf den Code-Deskriptor der Abstraktion */

   START_MODUL ("get_abstraction_namelist");

   abstr_desc = (T_PTD)heap_ptr[0];

   END_MODUL ("get_abstraction_namelist");

   if (R_DESC(*(R_COMB(*abstr_desc, ptd)), type) == TY_LREC_IND) /* LETREC-gebunden */
      return (get_namelist(R_LREC_IND(*(R_COMB(*abstr_desc, ptd)), ptf)));
   else
      return (get_namelist (R_COMB (*abstr_desc, ptd)));
}

/*-----------------------------------------------------------*/
/* INSTR *get_abstraction_code (heap_ptr)                    */
/*                                                           */
/* Liefert einen Verweis auf den Code einer Abstraktion.     */
/* heap_ptr ist ein Pointer auf eine Heap-Struktur, in deren */
/* ersten Komponente ein Verweis auf den Code-Deskriptor zu  */
/* finden ist.                                               */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */
/*-----------------------------------------------------------*/

INSTR *get_abstraction_code (heap_ptr)
PTR_HEAPELEM heap_ptr;
{
   T_PTD abstr_desc; /* Pointer auf den Code-Deskriptor der Abstraktion */

   START_MODUL ("get_abstraction_code");

   abstr_desc = (T_PTD)heap_ptr[0];

   END_MODUL ("get_abstraction_code");

   return (R_COMB(*abstr_desc, ptc));
}

/*-----------------------------------------------------------*/
/* STACKELEM get_arg_expr (heap_ptr, n)                      */
/*                                                           */
/* Liefert das n-te Argument aus einer Heapstruktur, die     */
/* eine Liste von Argumenten darstellt. An Position 1 der    */
/* Struktur befindet sich ein Verweis auf einen Funktionsterm*/
/* so dass sich das 1.Argument an 2.Position in der Heap-    */
/* Struktur befindet.                                        */
/* Aufrufe in:                                               */
/*             calc_nf ()                                    */ 
/*-----------------------------------------------------------*/

STACKELEM get_arg_expr (heap_ptr, n)
PTR_HEAPELEM heap_ptr;
int n;
{ 
   START_MODUL ("get_arg_expr");

   END_MODUL ("get_arg_expr");

   return ((STACKELEM)heap_ptr[n]);
}

/*-----------------------------------------------------------*/
/* void calc_nf ()                                           */
/*                                                           */
/* Reduktion eines Ausdrucks, der im Bindunsbereich          */
/* (mehrerer) Abstraktionen liegt.                           */
/* Resultat ist ein Stackelement.                            */
/* Aufrufe in:                                               */
/*             reduce_to_nf ()                               */
/*-----------------------------------------------------------*/
void calc_nf ()
{
   STACKELEM expr;
   STACKELEM res;  
   STACKELEM pred;
   STACKELEM arg;
   STACKELEM sub_expr;
   STACKELEM component;
   STACKELEM head;
   STACKELEM tail;
   T_LABEL label;
   T_PTD expr_ptr;
   PTR_HEAPELEM namelist; 
   PTR_HEAPELEM expr_list;
   int i;
   int arity;
   int len;

   START_MODUL ("calc_nf");

main_calc_nf:
   expr = POPSTACK(S_hilf);

   if (_redcnt > 0) /* Reduziere nur wenn der Reduktionszaehler */
                    /* noch nicht abgelaufen ist. */
   { 
      if (T_POINTER(expr))
      {
        expr_ptr = (T_PTD)expr;

	switch (R_DESC(*expr_ptr, class))
	{
	case C_FUNC       : {   /* Code-Deskriptor */

                                switch (R_DESC(*expr_ptr, type))
				{
				   case TY_COMB  : { /* Superkombinator */

				                     if (R_COMB(*expr_ptr, nargs) == 0)
						     {
                                                        /* constant applicative form */
                                                        /* dieser Ausdruck ist in NF */
							label = (T_LABEL)POPSTACK(S_i);
                                                     }
                                                     else /* Abstraktion (evtl. LETREC-gebunden) */
                                                     {
 
							PTR_HEAPELEM namelist; /* Verweis auf eine Liste der gebundenen Variablen */
                                                        STACKELEM res;         /* Zwischenergebnis der Reduktion des Rumpfes */
                                                           
                                                        if (R_DESC(*(R_COMB(*expr_ptr, ptd)), type) == TY_LREC_IND)
							  namelist = get_namelist (R_LREC_IND(*(R_COMB(*expr_ptr, ptd)), ptf));
                                                        else
							  namelist = get_namelist (R_COMB(*expr_ptr, ptd));
                                                        vars_to_argframe (WRITING, 1, R_COMB(*expr_ptr, nargs), namelist);
				        	        call_code (R_COMB(*expr_ptr, ptc)); 

                                                        /* bereite rekursiven Aufruf von calc_nf () vor: */
                                                        res = READSTACK (S_m1);
                                                        PPOPSTACK (S_m1); /* TB, 30.10.92 */
                                                        PUSHSTACK (S_hilf, (STACKELEM)expr_ptr); 
                                                        PUSHSTACK (S_hilf, (STACKELEM)namelist);
							PUSHSTACK (S_i, (STACKELEM)L_FUNC_COMB);
							PUSHSTACK (S_hilf, res);
							goto main_calc_nf;

                                                     func_comb:
							res      = POPSTACK(S_hilf);
							namelist = (PTR_HEAPELEM)POPSTACK(S_hilf);
							expr_ptr = (T_PTD)POPSTACK(S_hilf);
							label    = (T_LABEL)POPSTACK(S_i);

                                                        remove_bindings (1, R_COMB(*expr_ptr, nargs), namelist); 
                                                        expr_ptr = reconstr_abstr (expr_ptr, res);
							expr = (STACKELEM)expr_ptr; 
					             }
					             break;
						   }

				   case TY_CLOS  : { /* Abschluss */

                                                     switch (R_CLOS(*expr_ptr, ftype))
                                                     {
                                                        case TY_COMB: { /* Funktionsterm ist Abstraktion (evtl. aus LETREC) */

                                                                        arity = get_abstraction_arity (R_CLOS(*expr_ptr, pta));
                                                                        namelist = get_abstraction_namelist (R_CLOS(*expr_ptr, pta));

                                                                        /* Aufloesen einer partiellen Applikation = 1 Reduktionsschritt */
                                                                        _redcnt--;   

									/* Relativ freie Variablen werden auf dem Tildestack   */
									/* abgelegt und muessen nach der Ausfuehrung des Codes */
									/* wieder entfernt werden.                             */
#if WITHTILDE
									{
									  int nfv = R_CLOS(*expr_ptr, nfv);

									  if (nfv > 0) {
									    int i;
									    PTR_HEAPELEM pth;
									    
/* pth = (PTR_HEAPELEM)R_CLOS(*expr_ptr,pta); for (i=0; i<(arity+nfv+1); i++) DBUG_PRINT("DUSSEL", ("%d: %d hex %d", i, *(pth+i), *(pth+i)));  */

DBUG_PRINT("CALC_NF", ("R_CLOS(*expr_ptr,args): %d", R_CLOS(*expr_ptr,args)));

									    pth = (PTR_HEAPELEM)R_CLOS(*expr_ptr,pta) + R_CLOS(*expr_ptr,args) + 1 /* arity  *** BS from mah ?! */;
									    for (i=1; i<= nfv; i++) {
									      PUSHSTACK(S_tilde, *pth++);
									    }
									  }
									}
#endif /* WITHTILDE */

                                                                        /* Stelle nicht vorhandene Argumente kuenstlich bereit */
                                                                        /* Bindungslevel darf noch nicht veraendert werden,    */
                                                                        /* da Argumente noch ausgewertet werden muessen!       */
                                                                        vars_to_argframe (WRITING, 1, R_CLOS(*expr_ptr, nargs), namelist);

                                                                        /* Das Zuruecksetzen der Bindungslevel waere nur bei         */
                                                                        /* Reduktion der Argumente zur NF noetig, da in den          */
                                                                        /* evtl. weitere Abstraktionen auftreten koennen.            */
                                                                        /* remove_bindings (1, R_CLOS (*expr_ptr, nargs), namelist); */
     
                                                                        /* Reduziere die vorhandenen Argumente nicht zur NF, */
                                                                        /* da Code-Deskriptoren aufgeloest werden und fuer   */
                                                                        /* resultierende Abstraktionen kein neuer Code       */
                                                                        /* erzeugt wird. => Reduktion NICHT applicative order*/
                                                                        for (i = R_CLOS (*expr_ptr, args); i >= 1; i--)
                                                                        {
                                                                           STACKELEM arg;

                                                                           arg = (STACKELEM)get_arg_expr (R_CLOS(*expr_ptr, pta), i);
                                                                           PUSHSTACK (S_m1, arg);
                                                                        }

                                                                        /* Inkrementiere Bindungslevel fuer verbliebene Variable */
                                                                        /* Dies ist nur bei Reduktion der Argumente zur NF noetig*/
                                                                        /* set_bindings (1, R_CLOS (*expr_ptr, nargs), namelist); */
                          
                                                                        call_code (get_abstraction_code(R_CLOS(*expr_ptr, pta))); 
#if WITHTILDE
									/* Der Code ist ausgefuehrt. Ein eventuell vorhandener */
									/* Tildeframe muss abgeraeumt werden.                  */
									{
									  int nfv = R_CLOS(*expr_ptr, nfv);

									  if (nfv > 0) {
									    int i;
									    
									    for (i=1; i<= nfv; i++) {
									      PPOPSTACK(S_tilde);
									    }
									  }
									}
#endif /* WITHTILDE */
                                                                        
                                                                        /* bereite rekursiven Aufruf von calc_nf () vor */
                                                                        res = READSTACK (S_m1);
                                                                        PPOPSTACK (S_m1); /* TB, 30.10.92 */
									PUSHSTACK (S_hilf, (STACKELEM)expr_ptr);
#if WITHTILDE
									/* Der Tildepr"aprozessor f"ugt direkt aufeinander-
									   folgende subs zu einem einzelnen zusammen und h"alt
									   die urspr"ungliche Struktur in der Namensliste nach. */									
#if 1
									if (namelist[1] != 0) { /* die grobe Keule macht aus zusammengefasstem sub
												   ein mehrstelliges */
									  namelist[0] = namelist[1];
									  namelist[1] = 0;
									}
#else
									if (namelist[1] != 0) /* zusammengefasstes sub */
									  correct_namelist(namelist);
#endif
#endif /* WITHTILDE */
									PUSHSTACK (S_hilf, (STACKELEM)namelist);
									PUSHSTACK (S_i, (STACKELEM)L_CLOS_COMB);
									PUSHSTACK (S_hilf, res);      
									goto main_calc_nf;

                                                                     clos_comb:
									res      = POPSTACK (S_hilf);
									namelist = (PTR_HEAPELEM)POPSTACK (S_hilf);
									expr_ptr = (T_PTD)POPSTACK (S_hilf);
									label    = (T_LABEL)POPSTACK (S_i);

                                                                        remove_bindings (1, R_CLOS (*expr_ptr, nargs), namelist);
#if WITHTILDE
                                                                        expr = (STACKELEM)constr_abstr (expr_ptr, res, namelist[1]); 
#else
                                                                        expr = (STACKELEM)constr_abstr (expr_ptr, res); 
#endif /* WITHTILDE */

                                                                        break;
                                                                      }

                                                       case TY_CONDI: { /* Funktionsterm ist Conditional */ 
                                                                        /* hier muss der Praedikatausdruck reduziert werden */

									PUSHSTACK (S_hilf, (STACKELEM)expr_ptr);
									PUSHSTACK (S_i, (STACKELEM)L_CLOS_COND);
									PUSHSTACK (S_hilf, 
									   (STACKELEM)get_arg_expr (R_CLOS (*expr_ptr, pta), 1));
									goto main_calc_nf;

								     clos_cond:
                                                                        pred     = POPSTACK (S_hilf);
									expr_ptr = (T_PTD)POPSTACK (S_hilf);
									label    = (T_LABEL)POPSTACK (S_i);

                                                                        reconstr_arg (1, R_CLOS (*expr_ptr, pta), pred);
                                                                        expr = (STACKELEM)expr_ptr;
                                                                        break; 
                                                                      }

                                                        case TY_INTACT: /* Funktionsterm ist Interaktion  stt 11.09.95 */
                                                        case TY_PRIM: { /* Funktionsterm ist primitive Funktion */  
                                                                        /* hier muessen nur die Argumente reduziert werden. */

									i = 1;

							             prim_loop:
									if (i > (int) R_CLOS (*expr_ptr, args))
          /* int gecastet von RS 04/11/92 */ 
									   goto prim_exit;
									PUSHSTACK (S_hilf, (STACKELEM)expr_ptr);
									PUSHSTACK (S_hilf, (STACKELEM)i);
									PUSHSTACK (S_i, (STACKELEM)L_CLOS_PRIM); 
									PUSHSTACK (S_hilf, 
									   (STACKELEM)get_arg_expr (R_CLOS (*expr_ptr, pta), i));
                                                                        goto main_calc_nf;

						                     clos_prim:
									arg      = POPSTACK (S_hilf);
									i        = (int)POPSTACK (S_hilf);
									expr_ptr = (T_PTD)POPSTACK (S_hilf);

									reconstr_arg (i, R_CLOS (*expr_ptr, pta), arg);

									i++;
									goto prim_loop;

							             prim_exit:
									label    = (T_LABEL)POPSTACK (S_i); 
                                                                        expr = (STACKELEM)expr_ptr;

                                                                        break;
                                                                      }

                                                        case TY_CASE: { /* Funktionsterm ist Pattern-Match-Konstrukt */ 
                                                                        /* hier werden Argumente reduziert. */

									i = 1;

							            /* case_loop:                                        ach 05/11/92 */
									if (i > (int) R_CLOS (*expr_ptr, args))
    /* int gecastet von RS 04/11/92 */ 
									   goto case_exit;
									PUSHSTACK (S_hilf, (STACKELEM)expr_ptr);
									PUSHSTACK (S_hilf, (STACKELEM)i);
									PUSHSTACK (S_i, (STACKELEM)L_CLOS_CASE);
									PUSHSTACK (S_hilf,
									(STACKELEM)get_arg_expr (R_CLOS (*expr_ptr, pta), i));
									goto main_calc_nf;

							          /* clos_case: RS 30/10/92 */ 
									/* arg      = POPSTACK (S_hilf);
									i        = (int)POPSTACK (S_hilf);
									expr_ptr = (T_PTD)POPSTACK (S_hilf);

									reconstr_arg (i, R_CLOS (*expr_ptr, pta), arg);

									i++;
									goto case_loop;        auskommentiert von RS 04/11/92 */ 

							             case_exit:
									label    = (T_LABEL)POPSTACK (S_i);
                                                                        expr = (STACKELEM)expr_ptr;

                                                                        break;
                                                                      }

                                                        case TY_CLOS: { /* Funktionsterm ist wiederum ein Abschluss */ 
                                                                        /* Reduziere Funktionsterm und alle Argumente. */
                                                                        
									i = 0; 

							             clos_loop:  
									if (i > (int) R_CLOS (*expr_ptr, args)) 
   /* int gecastet von RS 04/11/92 */ 
									   goto clos_exit;
                                                                        PUSHSTACK (S_hilf, (STACKELEM)expr_ptr);
									PUSHSTACK (S_hilf, (STACKELEM)i); 
									PUSHSTACK (S_i, (STACKELEM)L_CLOS_CLOS); 
									PUSHSTACK (S_hilf, 
									   (STACKELEM)get_arg_expr (R_CLOS (*expr_ptr, pta), i)); 
									goto main_calc_nf; 

							             clos_clos:
									arg      = POPSTACK (S_hilf); 
									i        = (int)POPSTACK (S_hilf); 
									expr_ptr = (T_PTD)POPSTACK (S_hilf); 

									reconstr_arg (i, R_CLOS (*expr_ptr, pta), arg); 

									i++; 
									goto clos_loop; 

							             clos_exit:
									label    = (T_LABEL)POPSTACK (S_i); 
                                                                        expr = (STACKELEM)expr_ptr;
 
                                                                        break; 
                                                                      }

                                                        default:      /* ??? */
								      label = (T_LABEL)POPSTACK(S_i);
                                                                      break;
                                                     }
                                                     break;
                                                   }

				   case TY_CONDI : /* then und else Zweige werden nicht reduziert */
						   label = (T_LABEL)POPSTACK(S_i);
                                                   break;

                                   case TY_CASE  : /* Pattern-Match-Konstrukte werden nicht reduziert */
						   label = (T_LABEL)POPSTACK(S_i);
                                                   break;

				   default       : /* ??? */
						   label = (T_LABEL)POPSTACK(S_i);
			                           break;
			        }
			        break;
                            }
			
       case C_EXPRESSION  : { /* Bei der Applikation eines konstanten Terms auf Argumente */
                              /* z.B.: ap A to sub [X] in (1+2)                           */
                              /* wird kein Code-Deskriptor erzeugt. Daher muessen diese   */
                              /* Argumentausdruecke hier beruecksichtigt werden.          */

                              if (R_DESC(*expr_ptr, type) == TY_EXPR)
                              {
                                 expr_list = R_EXPR(*expr_ptr, pte); 
                                 len = (int)expr_list[0];
				 i = 1;

				 PUSHSTACK (S_hilf, (STACKELEM)expr_ptr);
		              expr_loop:
				 if (i > len)
				    goto expr_exit;
                                 PUSHSTACK (S_hilf, (STACKELEM)expr_list);
                                 PUSHSTACK (S_hilf, (STACKELEM)len);
				 PUSHSTACK (S_hilf, (STACKELEM)i);
				 PUSHSTACK (S_i, (STACKELEM)L_EXPRE);
				 PUSHSTACK (S_hilf,
				    (STACKELEM)get_arg_expr (expr_list, i));
                                 goto main_calc_nf;

                              expre:
                                 sub_expr  = POPSTACK (S_hilf);
				 i         = (int)POPSTACK (S_hilf);
                                 len       = (int)POPSTACK (S_hilf);
				 expr_list = (PTR_HEAPELEM)POPSTACK (S_hilf);

				 reconstr_arg (i, expr_list, sub_expr);

				 i++;
				 goto expr_loop;
				   
			      expr_exit:
				 expr_ptr = (T_PTD)POPSTACK (S_hilf);
				 label    = (T_LABEL)POPSTACK (S_i);
				 expr = (STACKELEM)expr_ptr;
                              }
			      else
				 label = (T_LABEL)POPSTACK(S_i);
                              break;
                           }

       case C_LIST       : { /* n-stellige Listen, Strings */
                             switch (R_LIST(*expr_ptr, type))
                             {
                                case TY_UNDEF: { /* n-stellige Listen */
                                                 
                                                 i = 1;

				              list_loop:
						 if (i > (int) R_LIST(*expr_ptr, dim))
   /* int gecastet von RS 04/11/92 */ 
						    goto list_exit;
                                                 PUSHSTACK (S_hilf, (STACKELEM)expr_ptr);
						 PUSHSTACK (S_hilf, (STACKELEM)i);
						 PUSHSTACK (S_i, (STACKELEM)L_LIST_UNDF);
						 PUSHSTACK (S_hilf,
						    (STACKELEM)get_arg_expr (R_LIST (*expr_ptr, ptdv), (i-1)));
                                                 goto main_calc_nf;

                                              list_undf:
						 component = POPSTACK (S_hilf);
						 i        = (int)POPSTACK (S_hilf);
						 expr_ptr = (T_PTD)POPSTACK (S_hilf);
                                                    
						 reconstr_arg ((i-1), R_LIST (*expr_ptr, ptdv), component);

						 i++;
						 goto list_loop;

				              list_exit:
						 label    = (T_LABEL)POPSTACK (S_i);
                                                 expr = (STACKELEM)expr_ptr;

                                                 break;
                                               }

                                default      : /* alle anderen strukturierten Objekte (z.B. Strings) */
                                               /* enthalten nur konstante Komponenten.               */
					       label = (T_LABEL)POPSTACK(S_i);
                                               break;
                             }
                             break; 
                           }

       case C_CONS       : { /* binaere Listen */ 

			     PUSHSTACK (S_hilf, (STACKELEM)expr_ptr);
			     PUSHSTACK (S_i, (STACKELEM)L_CONS_HEAD);
			     PUSHSTACK (S_hilf, (STACKELEM)R_CONS(*expr_ptr, hd));
			     goto main_calc_nf;

			  cons_head:
			     head = POPSTACK(S_hilf);
			     expr_ptr = (T_PTD)POPSTACK(S_hilf);
			     L_CONS(*expr_ptr, hd) = (T_PTD)head;

			     PUSHSTACK (S_hilf, (STACKELEM)expr_ptr); 
			     PUSHSTACK (S_i, (STACKELEM)L_CONS_TAIL); 
			     PUSHSTACK (S_hilf, (STACKELEM)R_CONS(*expr_ptr, tl)); 
			     goto main_calc_nf;

			  cons_tail:
			     tail = POPSTACK(S_hilf); 
			     expr_ptr = (T_PTD)POPSTACK(S_hilf); 
                             L_CONS(*expr_ptr, tl) = (T_PTD)tail;

			     label    = (T_LABEL)POPSTACK(S_i);
                             expr = (STACKELEM)expr_ptr;

                             break;
                           }
 

       default           : /* Ausdruck ist in Normalform */ 
			   label = (T_LABEL)POPSTACK(S_i);
			   break;
       }
    }
    else
       label = (T_LABEL)POPSTACK(S_i);
  }
  else
     label = (T_LABEL)POPSTACK(S_i);

  PUSHSTACK(S_hilf, expr);
  switch (label)
  {
     case L_FUNC_COMB: goto func_comb;
     		       /* break;   RS 3.12.1992 */
     case L_CLOS_COMB: goto clos_comb;
   		       /* break;   RS 3.12.1992 */
     case L_CLOS_COND: goto clos_cond;
		       /* break;   RS 3.12.1992 */
     case L_CLOS_PRIM: goto clos_prim;
		       /* break;   RS 3.12.1992 */
     case L_CLOS_CASE: goto clos_prim;
                       /* break;   RS 3.12.1992 */
     case L_CLOS_CLOS: goto clos_clos;
                       /* break;   RS 3.12.1992 */
     case L_EXPRE:     goto expre;
                       /* break;   RS 3.12.1992 */
     case L_LIST_UNDF: goto list_undf;
                       /* break;   RS 3.12.1992 */
     case L_CONS_HEAD: goto cons_head;
                       /* break;   RS 3.12.1992 */
     case L_CONS_TAIL: goto cons_tail;
		       /* break;   RS 3.12.1992 */
     case L_EXIT:      PUSHSTACK(S_e, expr);
                       PPOPSTACK(S_hilf); /* TB, 30.10.92 */
                       break;
     default:          post_mortem ("rnf: unknown label, confusion in stack management.");
                       break;
  }

  END_MODUL ("calc_nf");
}

/*-----------------------------------------------------------*/
/* void reduce_to_nf ()                                      */
/*                                                           */
/* Reduktion eines Ausdrucks zur Normalform.                 */
/* Der Ausdruck wird auf dem E-Stack erwartet.               */
/* Die Normalform wird auf dem E-Stack abgelegt.             */
/* Aufrufe in:                                               */
/*             file: rreduct.c                               */
/*-----------------------------------------------------------*/

void reduce_to_nf ()
{
      STACKELEM expr;  /* Oberstes Stackelement */

      START_MODUL ("reduce_to_nf");

      if (Normalform) {
        if (Normalform > 0)        /* Reduktion zur Normalform */
        {
          if (!Changes_Default)
            post_mortem ("reduce_to_nf: works only with Changes_default=1");
          PUSHSTACK (S_v, KLAA);
          expr = READSTACK(S_e);          
          PPOPSTACK(S_e); /* TB, 30.10.92 */
          PUSHSTACK(S_i, L_EXIT);
          PUSHSTACK(S_hilf, expr);
          calc_nf ();
          free_S_v ();
        }
      }

      END_MODUL ("reduce_to_nf");
}

#endif /* RED_TO_NF     auskommentiert RS 3.12.1992 */




