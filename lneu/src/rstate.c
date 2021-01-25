/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*-----------------------------------------------------------------------------
 * state:c external - save_state, restore_state, init_state, call_ear;
 * state:c internal - none
 *----------------------------------------------------------------------------
 */

/* last update  by     why
   10.06.88     CS     letrec
   copied       by     to

*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rstelem.h"                                                   /* CS */
#include "rheapty.h"
#include <setjmp.h>
#include "list.h"

/*********************************************************************/
/* Definiton der globalen Variablen des Reduktionssystems            */
/*********************************************************************/
/* Verwendung der Variablen in den Programmteilen :   reduce|pre,pro,post */
/*                                                   (reduct)             */

unsigned _redcnt   ;      /* Reduktionszaehler            <i>|<->,<w>,<-> */

BOOLEAN  _beta_count_only;/* True means only beta reduc-  <i>|<->,<w>,<-> */
                          /* tions are counted          */
char *   _errmes   ;      /* Platz fuer Fehlermeldung     <w>|<w>,<w>,<w> */
                          /* fuer post_mortem           */
BOOLEAN  _formated ;      /* gewuenschte Interne Dar-     <i>|<r>,<r>,<r> */
                          /* stellung der Zahlen        */

int      _base     ;      /* Basis der internen Darstel-  <i>|<r>,<r>,<r> */
                          /* lung fuer Dezimalzahlen    */
int      _maxnum   ;      /* Groesster Wert einer Ziffer  <i>|<r>,<r>,<r> */

int      _prec      ;     /* Anzahl der Nachkommastellen (bzgl. relativem */
                          /* Fehler), die bei einer Division max.         */
                          /* erzeugt werden sollen.                       */

int      _prec_mult ;     /* Anzahl der Nachkommastellen (bzgl. relativem */
                          /* Fehler), die bei einer Multiplikation max.   */
                          /* erzeugt werden sollen (-1 = exakt).          */


int      _indent    ;     /* Flag fuer die normalisierte  <i>|<->,<->,<-> */
                          /* Konstruktor - Ausgabe      */
int      _trunc     ;     /* Anzahl der signifikanten     <i>|<->,<->,<r> */
                          /* Nachkommastellen.          */
int      _bindings  ;     /* Die Zahl der aktiven Bin-    <->|<w>,<->,<-> */
                          /* dungen.                    */
                 
int  _debug_info ; /* stop ? error ? */ 

int stop_on_stop;
int stop_on_error;
int stopped_on_stop;
int stopped_on_error;
unsigned int  _saved_redcnt ;

BOOLEAN          _preproc=FALSE;
list   *VAR ;
fvlist *FV;
arglist *ARG;
hlist *BVAR;
free_var_list *FV_LREC;



postlist *ILIST;
postlist *TLIST;


BOOLEAN            xxx = FALSE;
BOOLEAN       part_sub = FALSE;
int             anz_sub_bv = 0;
STACKELEM *argumentlist = NULL;
int                  otherwise; 


int      _bound     ;/* Die Zahl der aktiven Bin-    <->|<w>,<->,<-> *//* CS */
                     /* dungen in LREC's           */                  /* CS */
                     /* _bound <= _bindings        */                  /* CS */
int      _argsneeded ;    /* Die Zahl der benoetigten     <->|<->,<w>,<-> */
                          /* Argumente einer Funktion   */
BOOLEAN  _digit_recycling;/* Fuer Digit-Arithmetik        <i>|<r>,<w>,<r> */

PTR_DESCRIPTOR         PRE_LIST;
int                     anz_PRE;


PTR_DESCRIPTOR _old_expr_p = NULL;/* alte Version fuer    <w>|<->,<->,<-> */
                          /* verpointertes Wiederaufsetzen */

STACKELEM _sep_int = SET_VALUE(LET1,'!');                              /* CS */
                 /* internal separation symbol for function names */   /* CS */
STACKELEM _sep_ext = SET_VALUE(LET1,'_');                              /* CS */
                 /* external separation symbol for function names */   /* CS */

/* Variablen fuer die Interrupt- und Ausnahmebehandlung */
/* vornehmlich der Processing Phase:                    */
jmp_buf _interruptbuf;    /* Sprungadresse bei Interrupt  <w>|<w>,<w>,<w> */
BOOLEAN _interrupt;       /* Interruptflag                <i>|<w>,<w>,<w> */
jmp_buf _fatal_error_label;/* Sprungadresse fuer das      <w>|<r>,<r>,<r> */
                          /* Modul post_mortem          */

/* Variablen, welche benoetigt werden, um einen Interrupt sinnvoll */
/* abzufangen.                                                     */
PTR_DESCRIPTOR _desc;     /* Ergebnisdescriptor derselben <->|<->,<w>,<-> */

BOOLEAN  _heap_reserved;/* Lokale Variable des Processors <i>|<w>,<w>,<w> */
                        /* Sie wird nur verwendet, um im Interruptfall    */
                        /* eine evtl. notwendige rel_heap Operation       */
                        /* durchfuehren zu koennen. (siehe stdinc:h)      */

unsigned short idcnt;    /* counter-variable for indirect descriptors   */

unsigned short stdesccnt;/* counter-variable for the                    */
                         /* number of "standard-descriptors"            */

int      _prec10;         /* REDUMA-divisionprecision with base 10     */
                          /* (parameter from EDITOR)                   */
                          /* used in rbibfunc.c to eval.               */
                          /* sin(x), ... , ln(x)                       */
int      _prec10_mult;    /* REDUMA-multiplicationprecision with       */
                          /* base 10 (parameter from EDITOR)           */


/********************************************************/
/* Globale Variablen des Stackmanagers:                 */
/********************************************************/
extern StackDesc S_e   ;/*     <i,w>|<w>,<w>,<w>        */
extern StackDesc S_a   ;/*     <i,->|<w>,<w>,<w>        */
extern StackDesc S_hilf;/*     <i,w>|<w>,<w>,<w>        */
extern StackDesc S_m   ;/*     <i,->|<w>,<w>,<w>        */
extern StackDesc S_m1  ;/*     <i,->|<w>,<w>,<w>        */
extern StackDesc S_i   ;/*     <i,->|<w>,<w>,<w>        */             /* CS */
extern StackDesc S_v   ;/*     <i,->|<w>,<->,<w>        */             /* CS */
extern StackDesc S_tilde;/*     <i,->|<w>,<w>,<w>        */
extern StackDesc S_pm;   /*     <i,->|<w>,<w>,<w>        */

/********************************************************/
/*         der Zustand der Reduktionsmaschine           */
/********************************************************/

typedef struct
{
  unsigned redcnt;
  BOOLEAN  beta_count_only;
  char *   errmes;
  BOOLEAN  formated;
  int      base;
  int      maxnum;
  int      prec;  
  int      prec_mult;
  int      indent;
  int      trunc;
  int      bindings;
  int      bound;                                                      /* CS */
  int      argsneeded;
  BOOLEAN  digit_recycling;
  PTR_DESCRIPTOR old_expr_p;
  jmp_buf  interruptbuf;
  BOOLEAN  interrupt;
  jmp_buf  fatal_error_label;
  PTR_DESCRIPTOR desc;
  BOOLEAN   heap_reserved;
  STACKELEM * hilf_MarBotPtr;
} REDUMA_STATE;

typedef struct   /* der Zustand eines Processes zum Verteilungszeitpunkt */
{
  unsigned redcnt;
  jmp_buf  interruptbuf;
  STACKELEM * hilf_MarBotPtr;
} PROCESS_STATE;

/*---------------------------------------------------------------------------
 * save_state -- sichert alle globalen Variablen des gesammten Simulators.
 *               Nur die Variablen des Uberwachungssystems werden nicht
 *               erfasst. Ausserdem setzt des den MarBotPointer des Hilfstacks
 *               so hoch, das der Stack leer wird.
 *               Der Zustand wird als integerpointer zurueckgeben.
 *--------------------------------------------------------------------------
 */
int * save_state()
{
 REDUMA_STATE * state;
 extern post_mortem();

 state = (REDUMA_STATE *) malloc(sizeof(REDUMA_STATE));

 if (state == (REDUMA_STATE *) NULL)
   post_mortem("save_state: unable to alloc space for a state");

  state->redcnt          = _redcnt;
  state->beta_count_only = _beta_count_only;
  state->errmes          = _errmes;
  state->formated        = _formated;
  state->base            = _base;
  state->maxnum          = _maxnum;
  state->prec            = _prec;
  state->prec_mult       = _prec_mult;
  state->indent          = _indent;
  state->trunc           = _trunc;
  state->bindings        = _bindings;
  state->bound           = _bound;                                     /* CS */
  state->argsneeded      = _argsneeded;
  state->digit_recycling = _digit_recycling;
/*state->old_expr_p      = _old_expr_p;     geht so nicht Hurck */
  state->interruptbuf[1] = _interruptbuf[1];
  state->interruptbuf[0] = _interruptbuf[0];
  state->interrupt       = _interrupt;
  state->fatal_error_label[1] = _fatal_error_label[1];
  state->fatal_error_label[0] = _fatal_error_label[0];
  state->desc            = _desc;
  state->heap_reserved   = _heap_reserved;

  state->hilf_MarBotPtr  = S_hilf.MarBotPtr;
  S_hilf.MarBotPtr       = S_hilf.TopofStack;

  return( (int *) state);
}

/*---------------------------------------------------------------------------
 * restore_state -- restauriert die globalen Variablen des Reduktionssystems
 *---------------------------------------------------------------------------
 */
void restore_state(st)
int * st;
{
 REDUMA_STATE * state;
 extern post_mortem();

 state = (REDUMA_STATE *) st;

 if (state == (REDUMA_STATE *) NULL)
   post_mortem("restore_state: unable to restore a state");

  _redcnt          = state->redcnt;
  _beta_count_only = state->beta_count_only;
  _errmes          = state->errmes;
  _formated        = state->formated;
  _base            = state->base;
  _maxnum          = state->maxnum;
  _prec            = state->prec;
  _prec_mult       = state->prec_mult;
 _indent          = state->indent;
  _trunc           = state->trunc;
  _bindings        = state->bindings;
  _bound           = state->bound;                                     /* CS */
  _argsneeded      = state->argsneeded;
  _digit_recycling = state->digit_recycling;
/*_old_expr_p      = state->old_expr_p;     geht so nicht, Hurck */
  _interruptbuf[1] = state->interruptbuf[1];
  _interruptbuf[0] = state->interruptbuf[0];
  _interrupt       = state->interrupt;
  _fatal_error_label[1] = state->fatal_error_label[1];
  _fatal_error_label[0] = state->fatal_error_label[0];
  _desc            = state->desc;
  _heap_reserved   = state->heap_reserved;

  S_hilf.MarBotPtr = state->hilf_MarBotPtr;

  free((char *) state);
}

/*---------------------------------------------------------------------------
 * init_state --  ??????????????????????????????????
 *---------------------------------------------------------------------------
 */
void init_state()
{;}


/*-------------------------------------------------------------------------
 * call_ear -- reduziert einen Ausdruck auf dem E-Stack, und liefert
 *             den Ergebnisausdruck auf dem E-Stack ab. Als Wert liefert
 *             die Funktion die Zahl der nicht benoetigten Reduktions-
 *             schritte ab.
 * Parameter - die Anzahl der Reduktionsschritte, die durchgefuehrt werden
 *             sollen.
 * ruft auf  - ear, save_state, restore_state, post_mortem, trav_a_e;
 *----------------------------------------------------------------------
 */
extern void trav_a_e();
extern void ear();

int call_ear(redcnt)
int redcnt;
{
  int *state;
  if (_heap_reserved || (redcnt < 0))
    return(redcnt);

  state = save_state();
  _redcnt = redcnt;
  if (setjmp(_fatal_error_label))
  {
    restore_state(state);
    longjmp(_fatal_error_label,1);
  }
  ear();
  TRAV_A_E;
  restore_state(state);
}
/* end of file rstate.c */
