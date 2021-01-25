/*
 * $Log: rstate.c,v $
 * Revision 1.6  2001/07/02 14:53:33  base
 * compiler warnings eliminated
 *
 * Revision 1.5  1994/03/08 19:38:42  mah
 * bug fix
 *
 * Revision 1.4  1994/03/08  19:10:52  mah
 * tilde version
 *
 * Revision 1.2  1992/12/16  12:51:05  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
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
#include "rmeasure.h"
#if WITHTILDE
#include "list.h"                        /* fuer den Tildestackpraeprozessor */
#endif /* WITHTILDE */
#include <setjmp.h>
#if (!APOLLO)
#include <malloc.h>
#endif

/* ach 03/11/92 */
#if DEBUG
extern void start_modul();
extern void end_modul();
#endif
/* end of ach */

/*********************************************************************/
/* Definiton der globalen Variablen des Reduktionssystems            */
/*********************************************************************/
/* Verwendung der Variablen in den Programmteilen :   reduce|pre,pro,post */
/*                                                   (reduct)             */


/*
 * Verwendung fuer Tildestackpraeprozessor
 */
#if WITHTILDE

list   *VAR ;
fvlist *FV;
arglist *ARG;
hlist *BVAR;
free_var_list *FV_LREC;
postlist *TLIST;
postlist *ILIST;
int             anz_sub_bv = 0;
BOOLEAN            xxx = FALSE;
STACKELEM *argumentlist = NULL;
int                  otherwise;
BOOLEAN   part_sub_var = FALSE;

#endif /* WITHTILDE */

unsigned _redcnt   ;      /* Reduktionszaehler            <i>|<->,<w>,<-> */

BOOLEAN  _beta_count_only;/* True means only beta reduc-  <i>|<->,<w>,<-> */
                          /* tions are counted          */
BOOLEAN  _count_reductions;/* self explanatory            <i>|<->,<->,<-> */
BOOLEAN  _delta_count;    /* count delta reductions       <i>|<->,<->,<-> */
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

int      _trunc     ;     /* Anzahl der signifikanten     <i>|<->,<->,<r> */
                          /* Nachkommastellen.          */
int      _bindings  ;     /* Die Zahl der aktiven Bin-    <->|<w>,<->,<-> */
                          /* dungen.                    */
int      _bound     ;/* Die Zahl der aktiven Bin-    <->|<w>,<->,<-> *//* CS */
                     /* dungen in LREC's           */                  /* CS */
                     /* _bound <= _bindings        */                  /* CS */
int      _argsneeded ;    /* Die Zahl der benoetigten     <->|<->,<w>,<-> */
                          /* Argumente einer Funktion   */
BOOLEAN  _digit_recycling;/* Fuer Digit-Arithmetik        <i>|<r>,<w>,<r> */

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
extern StackDesc S_pm  ; 
extern StackDesc S_tilde;

/********************************************************/
/*         der Zustand der Reduktionsmaschine           */
/********************************************************/

typedef struct
{
  unsigned redcnt;
  BOOLEAN  beta_count_only;
  BOOLEAN  count_reductions;
  BOOLEAN  delta_count;
  char *   errmes;
  BOOLEAN  formated;
  int      base;
  int      maxnum;
  int      prec;
  int      prec_mult;
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
 extern int post_mortem();
 int i;

 state = (REDUMA_STATE *) malloc(sizeof(REDUMA_STATE));

 if (state == (REDUMA_STATE *) NULL)
   post_mortem("save_state: unable to alloc space for a state");

  state->redcnt          = _redcnt;
  state->beta_count_only = _beta_count_only;
  state->count_reductions = _count_reductions;
  state->delta_count = _delta_count;
  state->errmes          = _errmes;
  state->formated        = _formated;
  state->base            = _base;
  state->maxnum          = _maxnum;
  state->prec            = _prec;
  state->prec_mult       = _prec_mult;
  state->trunc           = _trunc;
  state->bindings        = _bindings;
  state->bound           = _bound;                                     /* CS */
  state->argsneeded      = _argsneeded;
  state->digit_recycling = _digit_recycling;
/*state->old_expr_p      = _old_expr_p;     geht so nicht Hurck */
  for (i = 0 ; i < sizeof(jmp_buf)/sizeof(_interruptbuf[0]) ; i++) {
    state->interruptbuf[i] = _interruptbuf[i];
    state->fatal_error_label[i] = _fatal_error_label[i];
  }
  state->interrupt       = _interrupt;
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
 extern int post_mortem();
 int i;

 state = (REDUMA_STATE *) st;

 if (state == (REDUMA_STATE *) NULL)
   post_mortem("restore_state: unable to restore a state");

  _redcnt          = state->redcnt;
  _beta_count_only = state->beta_count_only;
  _count_reductions = state->count_reductions;
  _delta_count     = state->delta_count;
  _errmes          = state->errmes;
  _formated        = state->formated;
  _base            = state->base;
  _maxnum          = state->maxnum;
  _prec            = state->prec;
  _prec_mult       = state->prec_mult;
  _trunc           = state->trunc;
  _bindings        = state->bindings;
  _bound           = state->bound;                                     /* CS */
  _argsneeded      = state->argsneeded;
  _digit_recycling = state->digit_recycling;
/*_old_expr_p      = state->old_expr_p;     geht so nicht, Hurck */
  for (i = 0 ; i < sizeof(jmp_buf)/sizeof(_interruptbuf[0]) ; i++) {
    _interruptbuf[i] = state->interruptbuf[i];
    _fatal_error_label[i] = state->fatal_error_label[i];
  }
  _interrupt       = state->interrupt;
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

  return (0); /* RS 30/10/92 */
}
/* end of file rstate.c */
