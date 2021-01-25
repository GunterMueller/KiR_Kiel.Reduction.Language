/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */


/*----------------------------------------------------------------------------
 * reduce:c external - reduce: Die Schnittstelle zum Editor.
 * reduce:c internal - restore: Interruptbehandlungsroutine.
 * reduce:c internal - install: Initializierung des Interrupthandlers.
 *----------------------------------------------------------------------------
 */

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rusedeb.h"
#include "cparm.h"
#include <math.h>
#include <signal.h>
#include <setjmp.h>

extern jmp_buf    _interruptbuf;         /* Sprungadresse bei Interrupts    */
extern BOOLEAN    _interrupt;            /* Interruptflag                   */

/**************************************************************************
 *  R E S T O R E
 *
 *  Behandlung von Interrupts durch longjump zur Sprungadresse in
 *  interruptbuf.
 *
 *  Parameter: die Nummer des Signals
 ***************************************************************************
 */
int restore(sig)
int sig;
{
  if (sig == SIGINT)
    post_mortem("reduction process interrupted");

  _interrupt = TRUE;
#if SINTRAN3
  /*       ND specials !  That's magic !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
  { int * tha1 # 7 , *tha, i1 # 8;

    tha = tha1;             /* der Longjump: */
    tha[70] = _interruptbuf[1];
    tha[72] = _interruptbuf[0];
  }
#else
  longjmp(_interruptbuf,0);
#endif
}

/****************************************************************************/
/*  I N S T A L L                                                           */
/*                                                                          */
/*  Initialisierung des Interupthandlers                                    */
/*                                                                          */
/*  Parameter : keine                                                       */
/*                                                                          */
/****************************************************************************/

static void install()
{
  signal( SIGINT , restore );     /* interupt                 */
#if SINTRAN3                       /* nicht in Unix bekannt:   */
  signal( SIGFPEO, restore );     /* floating point overflow  */
  signal( SIGZERO, restore );     /* divide by zero           */
#endif

  signal( SIGFPE , restore );     /* floating point exception */
}

/*****************************************************************************/
/*                                                                           */
/*   E X T E R N E  F U N K T I O N E N  (von reduce)                        */
/*                                                                           */
/*****************************************************************************/

extern void InitStackmanagement();       /* stack:c                          */
extern void FormatStack();               /* stack:c                          */
extern void InitHeapManagement();        /* heap:c                           */
extern void ReInitHeap();                /* heap:c                           */
extern void reduct();                    /* reduct:c                        */
/* extern void preproc();  */                 /* preproc.c                        */
extern void restore_state();             /* state:c                         */
extern int * save_state();               /* state:c                         */

#if DEBUG
extern int initvar();                    /* runtime:c                       */
#endif

/****************************************************************************/
/*                                                                          */
/*   E X T E R N E  V A R I A B L E N   (von reduce aus reduct )            */
/*                                                                          */
/****************************************************************************/

extern unsigned   _redcnt;               /*           der Reduktionszaehler */
extern BOOLEAN    _beta_count_only;      /*           Art der Zaehlung      */
extern BOOLEAN    _formated;             /*           Interne Darstellung   */
                                         /*           der Zahlen            */
extern int        _base;                 /*     Basis der Digitstrings      */
extern int        _maxnum;               /*     Groesste Ziffer derselben   */
extern int        _trunc;                /*           die Nachkommastellen  */
                                         /*       welche ausgegeben werden  */
extern int        _prec;                 /*           Genauigkeit           */
extern int        _prec_mult;            /*   Multiplikationsgenauigkeit    */
extern char *     _errmes;               /*           Errormessage          */


extern jmp_buf    _fatal_error_label;    /* Sprungadresse bei fatalen       */
                                         /* Fehlern                         */

extern BOOLEAN  _digit_recycling;        /* Steuerflag fuer Digitarithmetik */
extern BOOLEAN  _heap_reserved;          /* (Integer-Semaphore)             */

extern double tpre;            /* rreduct.c, zur Zeitmessung       */
extern double tpro;            /* rreduct.c, zur Zeitmessung       */
extern double tpos;            /* rreduct.c, zur Zeitmessung       */
extern double tges;            /* rreduct.c, zur Zeitmessung       */

/****************************************************************************/
/*   R E D U C E                                                            */
/*                                                                          */
/*   Schnittstelle zum Editor                                               */
/*                                                                          */
/*   Parameter :                                                            */
/*                                                                          */
/*      stack  :  Adresse des Uebergabebereichs                             */
/*                                                                          */
/*      top    :  Anzahl der uebergebenen Stackelemente                     */
/*                                                                          */
/*      p_parms:  Adresse der Parameterliste                                */
/*                                                                          */
/****************************************************************************/


#ifdef DEBUG_MALLOC

/* rename the following 2 procedures ....*/
reduce ( stack, top, p_parms)
STACKELEM      *stack;
int            *top;
PTR_UEBERGABE  p_parms;
{
docheck("vor Reduktion ");

xreduce(stack,top,p_parms);

docheck("nach Reduktion ");

}
      
#endif


reduce ( stack, top, p_parms)
STACKELEM      *stack;
int            *top;
PTR_UEBERGABE  p_parms;

{
 int * old_reduma_state;
#if DEBUG
  static int aufrufzaehler = 0;
#endif
  old_reduma_state = save_state();

  _prec10 = p_parms->precision;   /* save the value precision given from */
                                  /* EDITOR, store in global variable    */
                                  /* _prec10, for the use in rbibfunc.c  */

  
  _prec10_mult =                  /* save multiplicationprecision value given */
            p_parms->prec_mult;   /* from EDITOR; _prec10_mult is defined in  */
                                  /* global in rextern.h ;                    */

  _errmes = p_parms->errmes;  /* Platz fuer eine Fehlermeldung schaffen */
  _interrupt = FALSE;         /* Interruptflag zuruecksetzen,sicherheitshalber */
  install();                  /* Interruptbehandlungsroutinen bekannt machen */

  if (setjmp(_fatal_error_label)) /* Aufgerufen durch post_mortem */
        /* post_mortem kann bereits in der Initialisierungsphase  */
        /* aufgerufen werden. Deshalb muss seine Sprungadresse    */
  {     /* hier liegen.                                           */
    p_parms->errflag = TRUE;  /* Signal fuer den Editor */
    if ((p_parms->redcmd == RED_INIT) || (p_parms->redcmd == RED_RS))
    {
        restore_state(old_reduma_state);
        return;
    }

    /* der Editor erwartet einen @ auf im Uebergabebereich, deshalb: */
    /* die Stacks leeren */
    FormatStack();

    PUSHSTACK(S_e,KLAA);

    p_parms->redcmd = RED_NRP;     /* @ zurueckgeben */
  }

  setjmp(_interruptbuf);  /* Sprungadresse fuer Interrupts */
                          /* von reduce                    */
  if (_interrupt)
  {
    _interrupt = FALSE;
    post_mortem("reduce: Interrupt received");
  }

#if DEBUG
  printf("\n\n\r");
  switch(p_parms->redcmd)
  {
    case RED_INIT: printf("reduce: message RED_INIT received in call number %d\n\r",++aufrufzaehler); break;
    case RED_NP  : printf("reduce: message RED_NP   received\n\r"); break;
    case RED_RD  : printf("reduce: message RED_RD   received\n\r"); break;
    case RED_NRP : printf("reduce: message RED_NRP  received\n\r"); break;
    case RED_RS  : printf("reduce: message RED_RS   received\n\r"); break;
    case RED_LRP : printf("reduce: message RED_LRP  received\n\r"); break;
    case RED_PRE : printf("reduce: message RED_PRE  received\n\r"); break;
    default: ;
  }
#endif




  switch (p_parms->redcmd)
  {
    case RED_INIT:  /* Initialisierung  */
    {      
      _formated = p_parms->formated;
      _base     = p_parms->base;
      _maxnum   = _base - 1;
      /* externe Praezision in interne Praezision konvertieren */
      _prec   = (int) ((double)p_parms->precision * log((double)10) / log((double)_base) + 0.9999);

      _prec_mult   = (int) ((double)p_parms->prec_mult * log((double)10) / log((double)_base) + 0.9999);

      _heap_reserved = FALSE;
      _digit_recycling = TRUE;
#if DEBUG
      printf("\n\r Parameter: \n\r");
      printf(" qstacksize= %d \n\r", p_parms->qstacksize);
      printf(" mstacksize= %d \n\r", p_parms->mstacksize);
      printf(" istacksize= %d \n\r", p_parms->istacksize);
      printf(" heapsize  = %d \n\r", p_parms->heapsize);
      printf(" heapdes   = %d \n\r", p_parms->heapdes);
      printf(" format    = %s \n\r", (p_parms->formated != 0 ? "fix" : "var"));
      printf(" base      = %d \n\r", p_parms->base   );

      initvar();    /* Initialisieren des Runtime-Systems */
      printf(" *** Init Runtime ok! *** \n\r");
#endif
      InitStackmanagement(p_parms->qstacksize,p_parms->mstacksize,p_parms->istacksize);
      InitHeapManagement(p_parms->heapsize,p_parms->heapdes);
      restore_state(old_reduma_state);
      return ;
    }
    case RED_RS:
    {
#if DEBUG
         initvar();    /* Initialisieren des Runtime-Systems */
#endif
      _formated = p_parms->formated;
      _base     = p_parms->base;
      _maxnum   = _base - 1;

      /* externe Praezision in interne Praezision konvertieren */
      _prec   = (int) ((double)p_parms->precision * log((double)10) / log((double)_base) + 0.9999);

      _prec_mult   = (int) ((double)p_parms->prec_mult * log((double)10) / log((double)_base) + 0.9999);

      _heap_reserved = FALSE;
      _digit_recycling = TRUE;
         FormatStack();
         ReInitHeap();
         restore_state(old_reduma_state);
         return ;
    }

#if LARGE
    case RED_PRE :
    {
      register int i;
      register STACKELEM  *lauf = stack;

       IS_SPACE(S_a,(i = *top));
      for ( ; i > 0; i--)
      {
        PPUSHSTACK(S_a,(*lauf++)); /* Stackelemente auf den A-stack legen */
      }

      if (SIZEOFSTACK(S_a) == 0)
        post_mortem("reduce: length of expression is 0");

      /* Den Ausdruck richtig herum auf den E-Stack legen */

      for (i = SIZEOFSTACK(S_a); i > 0; i--)
      {
        PUSHSTACK(S_e,POPSTACK(S_a));
      }

      _redcnt = p_parms->redcnt;
      _beta_count_only = p_parms->beta_count;
      _formated = p_parms->formated;
      _base     = p_parms->base;
      _maxnum   = _base - 1;
   
      /* externe Praezision in interne Praezision konvertieren */

      _prec   = (int) ((double)p_parms->precision * log((double)10) / log((double)_base) + 0.9999);

      _prec_mult   = (int) ((double)p_parms->prec_mult * log((double)10) / log((double)_base) + 0.9999);

      _trunc  = p_parms->truncation;

      _heap_reserved = FALSE;
      _digit_recycling = TRUE;


      preproc( p_parms->file_desc ); 
     p_parms->redcnt = _redcnt;
      p_parms->preproc_time = tpre;
      p_parms->process_time = tpro;
      p_parms->postproc_time = tpos;
      p_parms->total_time = tges; 
      

      return;

    }
#endif


    case RED_NP:  /* Einlesen der naechsten Seite */
    case RED_RD:  /* Einlesen der letzten Seite und Aufruf von reduct. */
    {
      register int i;
      register STACKELEM  *lauf = stack;

      IS_SPACE(S_a,(i = *top));
      for ( ; i > 0; i--)
      {
        PPUSHSTACK(S_a,(*lauf++)); /* Stackelemente auf den A-stack legen */
      }

      if (p_parms->redcmd == RED_NP)
      {
         restore_state(old_reduma_state);
        return; /* es fehlt noch etwas */
      }
      /* es soll also reduziert werden */
 
      /* so geht das, Mr. Hurck ! das folgende sollte nie der Fall sein */
      if (SIZEOFSTACK(S_a) == 0)
        post_mortem("reduce: length of expression is 0");

      /* Den Ausdruck richtig herum auf den E-Stack legen */
      for (i = SIZEOFSTACK(S_a); i > 0; i--)
      {
        PUSHSTACK(S_e,POPSTACK(S_a));
      }

      /* Reduktion kann beginnen */
      _redcnt = p_parms->redcnt;
      _beta_count_only = p_parms->beta_count;
      _formated = p_parms->formated;
      _base     = p_parms->base;
      _maxnum   = _base - 1;

      /* externe Praezision in interne Praezision konvertieren */
      _prec   = (int) ((double)p_parms->precision * log((double)10) / log((double)_base) + 0.9999);

      _prec_mult   = (int) ((double)p_parms->prec_mult * log((double)10) / log((double)_base) + 0.9999);

      _trunc  = p_parms->truncation;

      _heap_reserved = FALSE;
      _digit_recycling = TRUE;


/*H*/       
      _debug_info = p_parms->debug_info ;
      _saved_redcnt = 0 ;
       stopped_on_stop=stopped_on_error = 0 ;
       stop_on_error= IS_INFO(D_ERROR, _debug_info);
       if (stop_on_stop = IS_INFO(D_STOP , _debug_info))
           stop_on_error = stop_on_stop ;
/*H*/
      
      reduct();

      p_parms->redcmd = RED_NRP;
      p_parms->preproc_time = tpre;
      p_parms->process_time = tpro;
      p_parms->postproc_time = tpos;
      p_parms->total_time = tges;
/*H*/
      if (stopped_on_stop||stopped_on_error)
       p_parms->redcnt = _saved_redcnt;
      else
       p_parms->redcnt = _redcnt;

      if (stopped_on_stop) 
           SET_INFO(D_STOPPED , p_parms->debug_info ) ;
      else
      if (stopped_on_error) 
        SET_INFO(D_ERRORED , p_parms->debug_info ) ;
/*H*/


      /* und gleich ausgeben */
    }
    case RED_NRP:   /* Ausgabe der naechsten Seite des reduzierten Ausdrucks */
    {
       register int           i  = 0;
       register int         size = SIZEOFSTACK(S_e);
       register STACKELEM  *lauf = stack;

        if (size == 0)   /* ein Fehlerfall der eigentlich nicht auftreten darf */
       {
         p_parms->redcmd = RED_LRP;
         *top = 0;
         restore_state(old_reduma_state);
         return;
       }
       while (i < p_parms->pg_size)
       {
         *lauf++ = POPSTACK(S_e);
         i++;
         if (i == size)   /* falls fertig */
         {
           p_parms->redcmd = RED_LRP;
           break;
         }
       }
       *top = i;
       restore_state(old_reduma_state);
       return;
    }
    default:
      post_mortem("Reduce: Unknown redcommand");
  }
}
/* end of       rreduce.c */
