/* $Log: rreduce.c,v $
 * Revision 1.9  1995/03/07  14:42:13  sf
 * reduce(void) -> reduce(PTR_UEBERGABE)
 *
 * Revision 1.8  1995/02/13  13:58:31  rs
 * ok, signal handling for SIGSEGV and SIGBUS only in QUICK versions
 *
 * Revision 1.7  1995/02/13  09:24:45  rs
 * SIGSEGV and SIGBUS signal handlers installed
 *
 * Revision 1.6  1995/01/03  15:17:40  rs
 * the implementation of UH PM WILL USE FRED FISH DEBUG,
 * why is nobody els using it ? :-(
 * err....make that else, not els ! :)
 *
 * Revision 1.5  1993/10/29  08:27:09  ach
 * extern declaration of exit_ncube() added
 *
 * Revision 1.4  1993/10/27  12:51:39  ach
 * RED_STOP implemented
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:44  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



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
#include "rmeasure.h"
#include "cparm.h"
#include "dbug.h"
#include <math.h>
#include <signal.h>
#include <setjmp.h>

#if D_MESS
#include "d_mess_io.h"
extern unsigned int m_redcnt;
#endif

extern void stack_error(); /* TB & RS, 28.10.92 */
extern void readsetups(); /* TB & RS, 28.10.92 */
extern void clearscreen(); /* TB, 4.11.1992 */
#ifdef DEBUG     /* RS 7.12.1992 */
extern int sleep();
#endif

#if D_DIST
extern void init_ncube();
extern void wait_for_ncube();
extern void exit_ncube();
#endif

extern jmp_buf    _interruptbuf;         /* Sprungadresse bei Interrupts    */
extern BOOLEAN    _interrupt;            /* Interruptflag                   */
extern int        MaxRedcnt;
extern int Changes_Default;     /* editor, wg. verpointertem wiederaufsetzen, 0:ja, 1:nie */
#if D_DIST
extern int cube;                    /* nCube filedescriptor */
extern int cube_dim;                /* dimension of the subcube */
extern int cube_stack_size;
int nc_heaps;
int nc_heapd;
int nc_stacks;
#endif

/* stackelement conversion routines */

static STACKELEM new_encod();
static STACKELEM old_encod();

/* tabelle fuer die convertierung der stackelemente aus */
/* der alten in die neue codierung                      */

static STACKELEM new_stel_tab[] = {
   POINTER         /* 0 -> 0x00000000 ~ 0..0 0000 0000 */
  ,FUNC            /* 1 -> 0x00000008 ~ 0..0 0000 1000 */
  ,FUNC            /* 2 -> 0x00000008 ~ 0..0 0000 1000 */
  ,NORMAL_CONST    /* 3 -> 0x00000074 ~ 0..0 0111 0100 */
  ,LET0            /* 4 -> 0x0000000c ~ 0..0 0000 1100 */
  ,LET1            /* 5 -> 0x0000002c ~ 0..0 0010 1100 */
  ,DIG0            /* 6 -> 0x0000004c ~ 0..0 0100 1100 */
  ,DIG1            /* 7 -> 0x0000006c ~ 0..0 0110 1100 */
  ,VAR_CONSTR      /* 8 -> 0x00000002 ~ 0..0 0000 0010 */
  ,VAR_CCONSTR     /* 9 -> 0x0000000a ~ 0..0 0000 1010 */
  ,VAR_CCONSTR     /* a -> 0x0000000a ~ 0..0 0000 1010 */
  ,VAR_CCONSTR     /* b -> 0x0000000a ~ 0..0 0000 1010 */
  ,FIX_CONSTR      /* c -> 0x00000006 ~ 0..0 0000 0110 */
  ,FIX_CONSTR      /* d -> 0x00000006 ~ 0..0 0000 1110 */
  ,FIX_CCONSTR     /* e -> 0x0000000e ~ 0..0 0000 0110 */
  ,FIX_CCONSTR     /* f -> 0x0000000e ~ 0..0 0000 1110 */
};

/* tabelle fuer die convertierung der stackelemente aus */
/* der neuen in die alte codierung                      */

static STACKELEM old_stel_tab[] = {
   OLD_POINTER     /* 0: 0000 0000 POINTER     */
  ,1               /* 1: 0000 0001 INT         */
  ,OLD_VAR_CONSTR  /* 2: 0000 1000 VAR_CONSTR  */
  ,3               /* 3: 0000 0011 INT         */
  ,OLD_CONSTANT    /* 4: 0000 0011 CONSTANT    */
  ,5               /* 5: 0000 0101 INT         */
  ,OLD_FIX_CONSTR  /* 6: 0000 1100 FIX_CONSTR  */
  ,7               /* 7: 0000 0111 INT         */
  ,OLD_FUNC        /* 8: 0000 0001 FUNC        */
  ,9               /* 9: 0000 1001 INT         */
  ,OLD_VAR_CCONSTR /* a: 0000 1010 VAR_CCONSTR */
  ,11              /* b: 0000 1011 INT         */
  ,OLD_STRING      /* c: 0000 0100 STRING      */
  ,13              /* d: 0000 1101 INT         */
  ,OLD_FIX_CCONSTR /* e: 0000 1110 FIX_CCONSTR */
  ,15              /* f: 0000 1111 INT         */
};

static STACKELEM new_encod(x)
STACKELEM x;
{
  STACKELEM y = x & F_TYPE;

  if ((x & ~F_EDIT) == OLD_SA_FALSE) return(SA_FALSE);
  if ((x & ~F_EDIT) == OLD_SA_TRUE)  return(SA_TRUE);
  return(new_stel_tab[y] | (x & ~F_TYPE));
}
 
static STACKELEM old_encod(x)
STACKELEM x;
{
  STACKELEM y = x & F_TYPE;
 
  if (T_MA(x)) /* string? */
    return(old_stel_tab[y] | (x & OLD_MA_MASK) |
           ((x & (F_MARK_FLAG | F_DIGLET_FLAG)) >> O_MARK_FLAG));
  if (T_CONSTANT(x)) {
    if (T_SA_FALSE(x)) return(OLD_SA_FALSE);
    if (T_SA_TRUE(x))  return(OLD_SA_TRUE);
#ifdef NotUsed
    if (T_NUM(x))      return( ? );
    if (T_DOLLAR(x))   return( ? );
#endif /* NotUsed */
    return(old_stel_tab[y] | ( x & ~F_CLASS));
  }
  /* sonstige */
  return(old_stel_tab[y] | (x & ~F_TYPE));
}
/* end of conversion routines */

/**************************************************************************
 *  R E S T O R E
 *
 *  Behandlung von Interrupts durch longjump zur Sprungadresse in
 *  interruptbuf.
 *
 *  Parameter: die Nummer des Signals
 ***************************************************************************
 */
void /*int*/ restore(sig)     /* RS 04/11/92 */
int sig;
{
  if (sig == SIGINT) {
    post_mortem("reduction process interrupted"); 
    }

  if (sig == SIGSEGV) {
    post_mortem("segmentation fault: please email the example program to base@informatik.uni-kiel.d400.de");
    }

  if (sig == SIGBUS) {
    post_mortem("bus error: please email the example program to base@informatik.uni-kiel.d400.de");
    }

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

  return/*(0)*/; /* TB & RS, 28.10.92 */    /* RS 04/11/92 */ 
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
#if !DEBUG
  signal( SIGSEGV , restore);       /* segmentation violation */
  signal( SIGBUS , restore);       /* bus error */
#endif /* DEBUG */
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
extern BOOLEAN    _count_reductions;     /* falls 0: nicht zaehlen          */
extern BOOLEAN    _delta_count;          /* falls 1: delta red zaehlen      */
extern BOOLEAN    _formated;             /*           Interne Darstellung   */
                                         /*           der Zahlen            */
extern int        _base;                 /*     Basis der Digitstrings      */
extern int        _maxnum;               /*     Groesste Ziffer derselben   */
extern int        _trunc;                /*           die Nachkommastellen  */
                                         /*       welche ausgegeben werden  */
extern int        _prec;                 /*      Divisionsgenauigkeit       */
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

void /* TB & RS, 28.10.92 */
reduce ( stack, top, p_parms)
STACKELEM      *stack;
int            *top;
VOLATILE PTR_UEBERGABE  p_parms;
{
  int * old_reduma_state;
#if DEBUG
  static int aufrufzaehler = 0;
#endif

#if (D_MESS || !D_DIST || UH_ZF_PM)
 DBUG_PUSH("d:o,DBUG-output:t");            /* FRED FISH DEBUG PAKET INITIALISIEREN */
 DBUG_PRINT ("REDUCE", ("Habe DBUG-Output enabled !"));
#endif

  old_reduma_state = save_state();

  _prec10 = p_parms->precision;   /* save divisionprecision value given from  */
                                  /* EDITOR; _prec10 is defined global in     */
                                  /* rextern.h (used in rbibfunc.c);          */

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
  switch(p_parms->redcmd)
  {
    case RED_INIT: fflush(stdout);
                   clearscreen();
                   printf("\n\n\n\n\n");
                   printf(" reduce: message RED_INIT received (call %d) \n",++aufrufzaehler); break;
    case RED_NP  : printf(" reduce: message RED_NP   received \n"); break;
    case RED_RD  : printf(" reduce: message RED_RD   received \n"); break;
    case RED_NRP : printf(" reduce: message RED_NRP  received \n"); break;
    case RED_RS  : printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
                   printf(" reduce: message RED_RS   received \n"); break;
    case RED_LRP : printf(" reduce: message RED_LRP  received \n"); break;
    case RED_STOP: printf(" reduce: message RED_STOP received \n"); break;
    default: ;
  }
#endif
  switch (p_parms->redcmd)
  {
    case RED_INIT:  /* Initialisierung  */
    {
      _formated  = p_parms->formated;
      _base      = p_parms->base;
      _maxnum    = _base - 1;
      /* externe Praezision in interne Praezision konvertieren */
      _prec      = (int) ((double)p_parms->precision * log((double)10) / log((double)_base) + 0.9999);
      _prec_mult = (int) ((double)p_parms->prec_mult * log((double)10) / log((double)_base) + 0.9999);
      _heap_reserved   = FALSE;
      _digit_recycling = TRUE;
#if DEBUG
      printf(" Parameters: \n");
      printf(" qstacksize = %d \n", p_parms->qstacksize);
      printf(" mstacksize = %d \n", p_parms->mstacksize);
      printf(" istacksize = %d \n", p_parms->istacksize);
      printf(" heapsize   = %d \n", p_parms->heapsize);
      printf(" heapdes    = %d \n", p_parms->heapdes);
      printf(" format     = %s \n", (p_parms->formated != 0 ? "fix" : "var"));
      printf(" base       = %d \n", p_parms->base   );
      initvar();    /* Initialisieren des Runtime-Systems */
      printf(" *** Init Runtime ok! *** \n");
#endif
#if !NOSETUPS
      readsetups();	 /* dg *//* read global setups */
#endif
#if D_DIST
      if (!cube)
       {DBUG_PRINT ("HOST", ("Initializing..."));
        fflush (stdout);
        clearscreen(); 
        printf (" reduce: initializing nCUBE ...\n");
        fflush (stdout);
	init_ncube(cube_dim);
        wait_for_ncube(cube_dim);
	printf(" nCUBE ready...\n");
	fflush (stdout);
        DBUG_PRINT ("HOST", ("nCUBE ready !"));}
#endif
#if PI_RED_PLUS   
#     ifndef P_PI_RED_PLUS
#     define P_PI_RED_PLUS 1
#     define P_CHANGES_DEFAULT 2
#     endif
      SET_PAR1(p_parms,P_PI_RED_PLUS);       /* set pi_red_plus flag */
      if (Changes_Default)
        SET_PAR1(p_parms,P_CHANGES_DEFAULT); /* set changes_default flag */
      else
        CLR_PAR1(p_parms,P_CHANGES_DEFAULT); /* clr changes_default flag */
#endif
      InitStackmanagement(p_parms->qstacksize,p_parms->mstacksize,p_parms->istacksize);
      InitHeapManagement(p_parms->heapsize,p_parms->heapdes);

#if D_MESS
      m_heapsize = p_parms->heapsize;
      m_heapdes = p_parms->heapdes;
#endif
#if D_DIST
      nc_heaps = p_parms->heapsize;
      nc_heapd = p_parms->heapdes;
      nc_stacks = (cube_stack_size > 0)?cube_stack_size:p_parms->qstacksize*4+p_parms->mstacksize; 
#endif

#if DEBUG
      fflush(stdout);
#endif /* DEBUG */
      restore_state(old_reduma_state);
      return ;
    }
    case RED_RS:
#if DEBUG
      initvar();        /* Initialisieren des Runtime-Systems */
#endif
#if !NOSETUPS
      readsetups();	/* dg *//* read global setups */
#endif
      SET_PAR1(p_parms,P_PI_RED_PLUS);       /* set pi_red_plus flag */
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
#if DEBUG
      /* printf("\n wait 1 seconds ... \n"); */
      fflush(stdout);
      sleep(1);
#endif /* DEBUG */
         restore_state(old_reduma_state);
         return ;
    case RED_NP:  /* Einlesen der naechsten Seite */
    case RED_RD:  /* Einlesen der letzten Seite und Aufruf von reduct. */
    {
      register int i;
      register STACKELEM  *lauf = stack;

      IS_SPACE(S_a,(i = *top));
      for ( ; i > 0; i--)
      {
        /* Stackelemente auf den A-stack legen */
        PPUSHSTACK(S_a,new_encod(*lauf));
        lauf++;
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
#if D_MESS
      m_redcnt = p_parms->redcnt;
#endif
      _beta_count_only = p_parms->beta_count;
      _count_reductions = (_redcnt < MaxRedcnt); /* zaehlen? */
      _delta_count      = !_beta_count_only;
      _formated = p_parms->formated;
      _base     = p_parms->base;
      _maxnum   = _base - 1;
      /* externe Praezision in interne Praezision konvertieren */
      _prec   = (int) ((double)p_parms->precision * log((double)10) / log((double)_base) + 0.9999);

      _prec_mult   = (int) ((double)p_parms->prec_mult * log((double)10) / log((double)_base) + 0.9999);
      _trunc  = p_parms->truncation;

      _heap_reserved = FALSE;
      _digit_recycling = TRUE;

#if D_MESS
      DBUG_PRINT("rreduce",("Neue Reduktion..."));
      DBUG_PRINT("rreduce",("errmes-Eintrag: %s", p_parms->errmes));
      d_read_init();
#endif
      /* p_parms an reduct uebergeben, vorher void [sf 23.02.95] */
      reduct(p_parms);

      p_parms->redcnt = _redcnt;
      p_parms->redcmd = RED_NRP;
      p_parms->preproc_time = tpre;
      p_parms->process_time = tpro;
      p_parms->postproc_time = tpos;
      p_parms->total_time = tges;

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
         *lauf = POPSTACK(S_e);
         *lauf = old_encod(*lauf);
         lauf++;
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

  case RED_STOP:

#if D_DIST
    if (cube)
      exit_ncube();
#endif

    return;

    default:
      post_mortem("Reduce: Unknown redcommand");
  }
}
/* end of       rreduce.c */




