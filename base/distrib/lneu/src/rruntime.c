


/*#####################################################################*/
/*                     R U N T I M E . C                               */
/*               -  Monitor / Display - Tool -                         */
/*                                                                     */
/* In dieser Datei sind fast alle Prozeduren zusammengefasst, die fuer */
/* das Laufzeitsystem des Debugfalls benoetigt werden. Die restlichen  */
/* stehen in breakpoint.c                                              */
/*                                                                     */
/*#####################################################################*/

/* last update  by     why
   18.05.88     hs
   21.06.88     chr

   copied       by     to

 */

/* runtime.c --  post_mortem(), initvar(), setmonitor(), setdisplay(),
                 display_stack(), whichstacks(), which(), format(),
                 druckstack(), druckstack2(), pop_dr(), write_dr, push_dr(),
                 which_nr(), clear(), get_index(), set_ful(), printsteps(),
                 getstack(), slow(), dr(), test_monitor(), getzeile(),
                 ispstring(), isstring(), isint(), ishex(), getstring(),
                 isstackname(), getstacknr(), getstackindex(), monitor_call()
*/

#define RUNTIME /* um abzufangen, dass post_mortem nicht external ist */


/***********************************************************************/
/*
/*  INCLUDE-FILES
/*
/*********************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rruntime.h"
/* #include "ctermio.h"      -ca 14.06.88 second try    !!!  */
#include <ctype.h>
#include <setjmp.h>

#undef FIELD

extern char *   _errmes;                  /* state.c     */
extern  jmp_buf _fatal_error_label;       /* reduce.c     */
extern StackDesc    S_pm;

/*-----------------------------------------------------------------------------
 *  post_mortem  --  post-mortem-dump, d.h. Fehlermeldung bei toedlichen
 *                   Laufzeitfehlern wird ausgegeben und der Simulator wird
 *                   beendet.
 *------------------------------------------------------------------------------
 */
 post_mortem(text)
 char * text;                      /* Fehlertext */
 {
    int i;
#if DEBUG
    printf("\n*** Reduction stopped !!! ***\n");
    printf("Error: %s\n",text);

    monitor_call();                /* fordert Befehl fuer das Debugsystem an */
#endif                             /* (command-Schleife)  */

    strcpy(_errmes, text);         /* fuer Nachricht an Editor */

    longjmp(_fatal_error_label,1); /* nach reduce */
}

/*-----------------------------------------------------------------------------
 *  post_mortem2  --  Der Simulator wird beendet und eine Meldung an den
 *                    Editor uebergeben. Es erfolgt jedoch kein Monitoraufruf.
 *------------------------------------------------------------------------------
 */
 post_mortem2(text)
 char * text;                      /* Fehlertext */
 {
    int i;

    strcpy(_errmes, text);         /* fuer Nachricht an Editor */
    longjmp(_fatal_error_label,1); /* nach reduce */
}

#if DEBUG  /* der Rest wird nur im Debugfall benoetigt */


 FILE        * listfile;

 char line[81], *p_line;            /* Kommandoeingabe */
 int             slowfac;           /* slow for stackdisplay */
 FILE        * pmdmpfile = stdout ;
 int           pmdmpopen = 0 ;
 int           pmdmpcrs  = 1 ;
 int             debdisplay = 0;    /* debug display procedures*/
 int             showdisplay = 0;
 int             debconf ;          /* for arithmetic debug  */
 int             stopheap,          /* for simulating full heap */
    init ,                          /* init for runtime   */
    mode ;                          /* runtime mode       */
 int             outbefehl = 0;     /*pmcomp:c, veranlasst Ausgabe der */
                                    /* erzeugten Befehle               */

 int             outstep  = 0;      /* pmam:c, veranlasst Ausgabe von  */
                                    /* Registerinhalten nach jedem Interpreter*/
 int debcnt;                        /* for heap debug     */
 int debdesc;                       /* single desc debug  */
 static int      debstack;
 static int      stepdifstack;      /* counter stacksteps */
 static int      pm_stepdifam;      /* counter pmam steps */
 static int      stackstep;         /* stack step counter */
 static int      pm_amstep;         /* pmam step counter */
 static int      screenupdate;      /* screen update ?    */
 static int      debrefcount;       /* zeige refcounts    */
 static int      start;             /* initialisieren?    */
        int      breakred;          /* breakpoint redcnt  */

 static int      maxanz ,           /* Display stackelem. */
       normanz,                     /* = maxanz / 2       */
       indexline,                   /* Stackindex + Top   */
       lastline ;                   /* Last Display line  */
 static  int     stackanz ;         /* stacks to show     */
 static  int     stackanzahl ;      /* max. stacks to show */
 static  int     fullstack;         /* Nr des Fullstacks  */
 static  int     screen_descnt;     /* angezeigte desc's  */

 int             i_pop[4];          /* Zaehler fuer Stackop's */
 int             i_push[4];         /* 0 ~ Preprocessing      */
 int             i_read[4];         /* 1 ~ Processing         */
 int             i_write[4];        /* 2 ~ Postprocessing     */
 int             i_status;          /* 3 ~ unused             */
                                    /* i_status: status       */

 static  int top   [MAXSTACKS];        /* actual top index   */
 static  int index [MAXSTACKS];        /* actual display pos.*/
 static  int spalte[MAXSTACKS];        /* actual screen col. */
 static  int zeile [MAXSTACKS];        /* actual screen line */
 static  int stacknr[MAXSTACKS];       /* display no -> desc.*/
 static  int stackneu[MAXSTACKS];      /* display no -> desc.*/
 int full = 1;                         /* short - printstel  */
 static BOOLEAN flagheap;              /* first or not first */
 static BOOLEAN flagstack;             /* dump of heap, stack*/
 static BOOLEAN flagdump;              /* or all             */

 StackDesc * stdesc[20];

 static  FILE * fp;
 FILE * DumpFile;


/*********************************************************************/
 /*
/*  EXTERN - VAR
 /*
/*********************************************************************/

 extern PTR_DESCRIPTOR EnHeapD;              /* heap.c      */
 extern PTR_DESCRIPTOR StHeapD;              /* heap.c      */
 extern int descnt;
 extern int _bindings;                       /* state.c    */
 extern int _bound;                          /* state.c    */
 extern int _argsneeded;                     /* state.c    */
 extern unsigned int _redcnt;                /* state.c    */
 extern int _prec;                           /* state.c    */
 extern int _trunc;                          /* state.c    */
 extern int _indent;                         /* state.c    */
 extern int _maxnum;                         /* state.c    */
 extern int _base;                           /* state.c    */
 extern BOOLEAN _digit_recycling;            /* state.c    */
 extern BOOLEAN _formated;                   /* state.c    */
 extern BOOLEAN _heap_reserved;              /* state.c    */

 /* fuer die Interruptbehandlung: */
 extern BOOLEAN _interrupt;                  /* state.c     */
 extern PTR_DESCRIPTOR _desc;

 extern char modullist[50][20];             /* breakpoint.c   */
#if MEASURE
 extern int messtab[81][20];                 /* breakpoint.c  */
#endif
 extern int modulnr;                         /* breakpoint.c  */
 extern int breaknr;                         /* breakpoint.c  */
 extern int co;                              /* claus         */
 extern int li;                              /* claus         */
/*********************************************************************/
 /*
/*  EXTERN - FUNKTIONEN
 /*
/*********************************************************************/

 extern int * save_state();                 /* state.c       */
 extern void restore_state();                /* state.c       */
 extern void      getstack();                /* stack.c       */
 extern int       sizeofstack();             /* stack.c       */
 extern void      list_moduls();             /* breakpoint.c  */
 extern void      list_breakpoint();         /* breakpoint.c  */

 extern DescDump();                          /* descdump.c    */
 extern ReInitHeap();                        /* heap.c        */





                                             /*$P                  */
/*********************************************************************/
/*  INITVAR : wird vor dem ersten call des Monitors aufgerufen
/*            und initialisiert die Debug- und Steps- Variablen
 /*
/*********************************************************************/

 initvar()
 {
    int i,j;

    debdisplay=0;
    debconf =0;                         /* for conform check  */
    stopheap = -1;                      /* no full-heap simu- */
    start = 1;                          /* init for Stacks    */
                                        /* lation             */
    slowfac=0;                          /* no slow            */
    init = 1;                           /* initialisierung    */
    mode = CMD_DISPLAY;                 /* default mode       */
    stacknr[0] = 0;
    stackanzahl = 0;                    /* max. Stackzahl     */
                                        /* fuer Bildschirm    */
    modulnr = 0;                        /* kein Modul aktiv   */
    debrefcount = FALSE;                /* kein refcount zeigen */
    for (i_status = 0; i_status <= 3; i_status++)
    {
       i_pop[i_status] = 0;                 /* bisher keine Stackop's */
       i_push[i_status] = 0;
       i_read[i_status] = 0;
       i_write[i_status] = 0;
    }
    i_status = 0;                           /* 0 ~ Preprocessing */
#if MEASURE
    m_init();    /* Messtabelle initialisieren */
#endif

   flagheap = FALSE;                         /* no dump up to now */
   flagstack = FALSE;
   flagdump = FALSE;
 }

/*********************************************************************/
/*  SETMONITOR  setzt die Variablen fuer den MONITOR-Mode            */
/*                                                                   */
/*********************************************************************/
 static setmonitor ()

 {
    full = 1;                                /* set short display  */
    mode = CMD_MONITOR;
    debdisplay=0;                            /* stacks nicht       */
    screenupdate = FALSE;                    /* no screenupdate    */
    debrefcount = FALSE;
                                             /* anzeigen           */
    clearscreen();                           /* screen             */
 }


/*********************************************************************/
/*  SETDISPLAY  setzt die Variablen fuer den DISPLAY-Mode            */
/*                                                                   */
/*                                                                   */
/*********************************************************************/
 setdisplay ()

 {
    full = 1;                                  /* set short display  */
    mode = CMD_DISPLAY;

    debdisplay = 1;

 /* Display Konstanten fuer DISPLAY-MODE justieren */
    lastline = 2;
    indexline = NAMELINE + 1;
    maxanz = (NAMELINE - 5);                      /* von Stackelementen */
    normanz = maxanz/2 + 1;                       /* von Stackelementen */
    screenupdate = FALSE;
    debrefcount = FALSE;
    if (start)
    {
       whichstacks();
       start = 0;
    }
    else
       format();
 }
                                             /*$P                  */

/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   DISPLAY_STACK                              */
/*                                                                   */
/*   <parameter>      :    3                                         */
/*                                                                   */
/*      desc : descriptor des Stacks fuer den eine action            */
/*             auf dem Bildschirm dargestellt werden soll            */
/*      sw   : POP/PUSH/WRITE action die dargestellt werden          */
/*             soll.                                                 */
/*      elem : Element das gepushed wird.                            */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*      stackstep            (include)runtimedef.h                   */
/*                                                                   */
/*    <r>             :                                              */
/*       stdesc              runtime.c                               */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*      pop_dr              (runtime.c)                              */
/*      write_dr            (runtime.c)                              */
/*      reset_dr            (runtime.c)                              */
/*      push_dr             (runtime.c)                              */
/*      read_dr             (runtime.c)                              */
/*      get_index           (runtime.c)                              */
/*      post_mortem         (runtime.c)                              */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*       popstack                         stack.c                    */
/*       pushstack                        stack.c                    */
/*       writestack                       stack.c                    */
/*       readstack                        stack.c                    */
/*                                                                   */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*   display_stacks stellt die angegebene action auf dem Stack dar   */
/*   und zaehlt den stackaction counter hoch. Danach wird der        */
/*   monitor aufgerufen und falls die angegebene Zahl von Stack-     */
/*   actions ausgefuehrt wurde in die command-Schleife verzweigt.    */
/*   Entsprechendes gilt fuer den breakpoint des Reduktionszaehlers  */
/*   Ferner werden die Stackoperationen gezaehlt.                    */
/*                                                                   */
/*###################################################################*/


 display_stack(desc,sw,elem)

 StackDesc * desc;                           /* stack to display   */
                                             /* referenced by      */
                                             /* descriptor         */
 int sw;                                     /* what is to be      */
                                             /* displayed          */
 STACKELEM elem;                             /* element to be      */
                                             /* pushed             */

 {
    int register number;                     /* hilfsvariable      */
    int i;                                   /* hilfsvariable fuer slow */

          if (breakred)                      /* test, ob breakpoint */
             if (_redcnt == breakred)        /* auf redcount       */
             {                               /* breakpoint loeschen */
                breakred = 0;
                WRONGINPUT(breakpoint red reached);
                monitor_call();
             }                               /* command-Schleife  */

    if (sw==CREATE)
    {
       stacknr[0]++;
       stdesc[stacknr[0]] = desc;
       if (++stackanzahl > co/16) stackanzahl--;
    }
    else                                       /* sw == CREATE       */
    {

       if (!showdisplay)                       /* keine Bildschirmsteuerung */
       {
          return;
       }

       number = get_index(desc);                 /* compute index of   */
                                                 /* stack referenced   */
       switch (sw)                               /* switch only for    */
       {                                         /* measure            */
         case POP :
          i_pop[i_status]++;                     /* Stackop's werden 2mal gezaehlt */
#if MEASURE                                      /* in i_pop unabh. von MEASURE */
            if (desc->Name[0] == 'i')               /* Inkarnationsstack */
               measure(MS_IPOP  );
            else
               measure(MS_POP  );                /* sonstiger Stack */
#endif
          break;


         case PUSH :
          i_push[i_status]++;                     /* siehe oben */
#if MEASURE
            if (desc->Name[0] == 'i')
               measure(MS_IPUSH );
            else
               measure(MS_PUSH );
#endif
          break;


         case READ :
          i_read[i_status]++;                        /* siehe oben */
#if MEASURE
            if (desc->Name[0] == 'i')
               measure(MS_IREAD );
            else
               measure(MS_READ );
#endif
          break;


         case WRITE :                                /* siehe oben */
          i_write[i_status]++;
#if MEASURE
            if (desc->Name[0] == 'i')
               measure(MS_IWRITE);
            else
               measure(MS_WRITE);
#endif
          break;

         default : ;
       }

       if (!debdisplay) return;                  /* by desc in data-   */
       if (number ==0) return;                   /* stack not traced   */
       else                                      /* stack is traced    */





          switch (sw)
       {
          break;
         case WRITE :                          /* display write      */
          write_dr(number,elem);               /* write              */
          test_monitor();                      /* back to monitor?   */
                                               /* to check whether   */
                                               /* ready with work    */
                                               /* to ask for further */
                                               /* commands           */
          break;
         case POP   :                          /* display pop        */
          pop_dr(number);                      /* pop                */
          test_monitor();                      /* back to monitor?   */
                                               /* to check whether   */
                                               /* ready with work    */
                                               /* to ask for further */
                                               /* commands           */
          break;
         case PUSH  :                          /* display push       */
          push_dr(number,elem);                /* push               */
          test_monitor();                      /* s.o.               */
          break;

         default    : ;                        /* unknown stackaction*/
       }                                       /* switch             */
       if (stepdifstack)                       /* slow factor set    */
          for (i = 1; i<=slowfac; i++)
             slow();
    }                                          /* else CREATE        */
 }                                             /* displaystacks      */

                                             /*$P                  */
/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   WHICHSTACKS                                */
/*                                                                   */
/*   <parameter>      :    %                                         */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*      stackanz                                                     */
/*      stacknr[]                                                    */
/*                                                                   */
/*    <r>             :                                              */
/*      stdesc              runtime.c                                */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*      format              (runtime.c)                              */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      setdisplay           runtime.c                               */
/*      initvar                                                      */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*   whichstacks belegt das Array stacknr[], in dem die Nummern der  */
/*   darzustellenden stacks (= index im sourcecode des jeweiligen    */
/*   stackalgorithmus) abgelegt sind. Ausserdem wird stackanz =      */
/*   Anzahl der darzustellenden Stacks belegt: die ersten            */
/*   5 Stacks des Algorithmus.                                       */
/*   Danach werden diese Stacks mittels format auf dem               */
/*   Bildschirm gelistet.                                            */
/*                                                                   */
/*###################################################################*/

 whichstacks()
 {

    int register i;                          /* Laufvariable       */

    stackanz = (stacknr[0] > co/16) ? co/16 : stacknr[0];

    for (i=1;i<=stackanz;i++)
       stacknr[i] = i;                       /* set index trivial  */

    format();                                /* build new format   */
 }                                           /* end whichstacks    */

/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   WHICH                                      */
/*                                                                   */
/*   <parameter>      :    %                                         */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*      stackanz                                                     */
/*      stacknr[]                                                    */
/*                                                                   */
/*    <r>             :                                              */
/*      stdesc                runtime.c                              */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*      format              (runtime.c)                              */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      call_monitor            (schup)runtime.c                     */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*   whichstacks belegt das Array stacknr[], in dem die Nummern der  */
/*   darzustellenden stacks (= index im sourcecode des jeweiligen    */
/*   stackalgorithmus) abgelegt sind. Ausserdem wird stackanz =      */
/*   Anzahl der darzustellenden Stacks belegt: sind im aktuellen     */
/*   Algorithmus weniger als STACKANZ stacks werden alle dargestellt */
/*   sonst im Dialog die Zahl und Indizes der gewuenschten Stacks    */
/*   erfragt. Danach werden diese Stacks mittels format auf dem      */
/*   Bildschirm gelistet.                                            */
/*                                                                   */
/*###################################################################*/

 which()

 {

    BOOLEAN wrong ;                             /* Hilfsvariable      */
    int register i;                             /* Laufvariable       */

    clearscreen();                              /* screen             */
    NEWMODUL;                                   /* refresh Modul-Feld */
    NEWMODE ;                                   /* refresh Mode-Feld  */
    NEWDES;

    if (stacknr[0] > STACKANZ)                  /* mehr stacks im     */
                                                /* Stackalgorithmus   */
                                                /* als darstellbar    */
    {
       POS(CMDSPALTE, CMDLINE + 2);
       printf("\n\n\n there are following stacks : \n");/* Liste der Stacks   */
                                                   /* im aktuellen Stack-*/
                                                   /* algorithmus        */
       for (i=1;i<=stacknr[0];i++)
          printf("Stack <%s> \n",stdesc[i]->Name);
       stackanz = STACKANZ +1;                     /* Abbruchbedingung no */
       while ((stackanz > STACKANZ) || (stackanz <=0))/* correct anz lesen */
                                                  /* stackanz = anz der  */
                                                  /*dargestellten stacks */
       {
          POS(CMDSPALTE,CMDLINE + 1);
          printf("Wie viele Stacks:");            /* Wiederhole bei     */
          while (1)                               /* falscher Eingabe   */
          {
             if ((isint(&stackanz) ) && (stackanz > 0) &&
                 (stackanz <= stacknr[0]) && (stackanz <= STACKANZ))
                break;                             /* Ist Input integer und */
             POS(CMDSPALTE,CMDLINE + 1);           /* 0 <= Input <= maxAnzahl */
             printf("Fehlerhafte Eingabe! Wieviele Stacks ?: ");
          }
       }

       for (i=1;i<=stackanz;i++)                   /* Stacknamen eingeben */
       {
          wrong = TRUE;
          while  (wrong)
          {
            POS(CMDSPALTE,CMDLINE + 1);
            printf("Enter %d stacks   :",stackanz-i+1); /* Anzahl der einzu- */
                                                        /* gebenden Stacks   */
            {
               int register j;

               while (1)
               {
                  if (( stackneu[i] = getstackindex() ) > 0) break;
                  POS(1,CMDLINE + 2);
                  printf("                                             \n");
                  printf("                                             \n");
                  POS(1,CMDLINE + 2);
                  printf("Bitte Eingabe wiederholen!\n");
               }
               NEWERR;                                 /* alte Fehlermeldung */
                                                       /* loeschen           */

               if ((stackneu[i] <= stacknr[0] )  && (stackneu[i] > 0) )
                                                    /* zulaessige stacknr */
                  wrong = FALSE;

               for (j=1;j<i;j++)
                  if ( stackneu[j] == stackneu[i] )
                                                 /* schon mal angegeben*/
                                                 /* lese neu           */
                     wrong = TRUE;

               if (wrong)
               {
                  POS(CMDSPALTE,CMDLINE + 1);
                  printf("Stackname unbekannt! Enter %d stacks   :",stackanz-i+1);
                               /* Anzahl der einzugebenden Stacks   */
               }
            }
          }                                    /* while wrong        */
       }                                       /* for                */
       for (i = 1; i<= stackanz; i++)
          stacknr[i] = stackneu[i];
    }                                          /* if                 */
    else                                       /* anz <= STACKANZ    */
    {
       stackanz = stacknr[0];                  /* alle stacks tracen */
       for (i=1;i<=stackanz;i++)
          stacknr[i] = i;                      /* set index trivial  */
    }
    screenupdate = FALSE;
    setdisplay();                             /* build new screen   */
 }                                            /* end whichstacks    */

                                             /*$P                  */
/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   FORMAT                                     */
/*                                                                   */
/*   <parameter>      :    %                                         */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*      zeile                                                        */
/*      top                                                          */
/*      index                                                        */
/*      spalte                                                       */
/*      full                                                         */
/*                                                                   */
/*    <r>             :                                              */
/*      stackanz                                                     */
/*      maxanz                                                       */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*      druckstack          (runtime.c)                              */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      whichstacks               (schup)runtime.c                   */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*   format berechnet die Werte zur Darstellung der Stacks auf dem   */
/*   screen und druckt mittels druckstack alle im Moment getraceten  */
/*   stacks aus.                                                     */
/*                                                                   */
/*###################################################################*/

 static format()

 {
    int register i;                           /* Laufvariable       */

    for (i=1;i<=stackanz;i++)                 /* Initialisierung    */
    {
       int abstand;                           /* der stacks auf dem */
                                              /* Bildschirm         */

       zeile[i] = NULLINE;                    /* beginn unten       */
       top[i] = 0;                            /* stack leer         */
       index[i] = -maxanz-1;                  /* top zeigen !!      */
       abstand = LINELENGTH / stackanz;       /* abstand berechnen  */
       spalte[i] = 1 + abstand * (i-1) ;      /* spalte setzen      */

    }                                         /* for                */
    full = 1;                                 /* FULLMODE aus!      */

    clearscreen();                            /* Bildschirm         */
    fflush(stdout);

    NEWMODUL;
    NEWMODE ;
    NEWDES;

    POS(1,XLINE);
    for (i=1;i<=LINELENGTH;i++) printf("-"); printf("\n");
    fflush(stdout);
    for (i=1;i<=stackanz;i++)
    {
       index[i] = -maxanz-1;                  /* top drucken        */
       druckstack(i,normanz);                 /* normanz Elemente   */
                                              /* von oben           */
    }                                         /* for                */
 }                                            /* format             */

                                             /*$P                  */
/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   DRUCKSTACK                                 */
/*                                                                   */
/*   <parameter>      :    2                                         */
/*                                                                   */
/*      i    : Index des Stacks der gelistet werden soll             */
/*      zahl : Anzahl der Elemente von Stack nr i die gelistet       */
/*             werden sollen.                                        */
/*                                                                   */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*      index                                                        */
/*      top                                                          */
/*      zeile                                                        */
/*                                                                   */
/*    <r>             :   %                                          */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*      sizeofstack                      stack.c                     */
/*      getstack                        stack.c                      */
/*      druck, druck2                  runtime.c                     */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      format                    runtime.c                          */
/*                                                                   */
/*                                                                   */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*   druckstack holt sich durch Stackmanager Aufrufe den i-ten       */
/*   Stack und druckt ihn mittels druck auf den Screen.              */
/*   Der Stack wird von der Position index[i] an gelistet und es     */
/*   werden zahl Elemente des Stacks ausgegeben auf die Bildschirm-  */
/*   position <zeile[i],spalte[i]>.                                  */
/*   index,top,zeile,zahl werden auf Gueltigkeit ueberprueft und     */
/*   gegebenenfalls veraendert.                                      */
/*                                                                   */
/*###################################################################*/

 static druckstack(i,zahl)

    int i;                                      /* stacknr  to list   */
    int zahl;                                   /* anz elemente       */
                                                /* to list            */

 {

    STACKELEM * * p;                            /* adr(adr(beginn)) of*/
                                                /* stack              */
    STACKELEM * hi;                             /* adr(beginn) of     */
                                                /* stack              */
    char * frptr;                               /* free pointer       */
    int  x ;                                    /* Hilfsvariable      */
    int register j;                             /* laufvariable       */


    zeile[i] = NULLINE;                         /* Zeile ganz unten   */

    x = sizeofstack(stdesc[stacknr[i]]);        /* stackgroesse       */
                                                /* = anz Elemente     */
    if (x==0)                                   /* stack empty        */
    {
       POS(spalte[i],NULLINE);
       EL_CLEAR;

       POS(spalte[i],indexline);
       printf("<%d> Top:<%d>",stacknr[i],(x) ? x   : 0);/*index und tiefe */

       POS(spalte[i],(NAMELINE));
       printf("%s",(stdesc[stacknr[i]])->Name);        /* name  */
       fflush(stdout);

       top[i] = x;                              /* setze top          */
       index[i] = 0;
    }
    else
    {
       p = (STACKELEM * *)&hi;                  /* adr laden          */
                                                /* wert von hi wird   */
                                                /* getstack veraendert*/
       *p = (stdesc[stacknr[i]]->MarBotPtr) + 1;

       frptr = (char *) hi;                     /*adr belegter bereich*/

       POS(spalte[i],indexline);
       printf("<%d> Top:<%d> ",stacknr[i],(x) ? x   : 0);
                                                /* index und tiefe    */

       POS(spalte[i],(NAMELINE)); fflush(stdout);
       printf("%s",(stdesc[stacknr[i]])->Name); /* name               */

       top[i] = x;                            /* setze top          */

       if (index[i] == -maxanz-1)             /* top soll ausgegeben*/
                                              /* werden             */
       {
          if (zahl <= x)                      /* weniger Elemente   */
                                              /* gefordert als vor- */
                                              /* handen             */
             index[i] = x - zahl ;            /* oberste zahl elems */
                                              /* ausgeben           */
          else                                /* stack hat nicht    */
                                              /* soviele Elemente   */
                                              /* wie ausgegeben     */
                                              /* werden sollen      */
          {
             zahl = x;                        /* alle ausgeben      */
             index[i] = 0;                    /* bottom of stack    */
          }                                   /* else               */
       }                                      /* top ausgabe        */
       else
       {
          if ( zahl > x) zahl = x;             /* mehr elemente ge-  */
                                               /*fordert als im stack*/
          if (index[i] < -1) index[i] = 0;     /* nicht tiefer als   */
                                               /* bottom             */
          if (index[i] > x) index[i] = x - zahl;  /* oberste elemente   */
          if ((index[i] + zahl) > x) index[i] = x - zahl;/* oberste Elemente */
       }


       POS(spalte[i],NULLINE);                 /* Base of stack i    */
       if (debrefcount)
          for (j=0;j<zahl;j++)                 /* ausgebe von zahl   */
             druck2(i,*p+index[i]);            /* Elementen          */
                                               /* alle Deskriptoren */
                                               /* durch refcount     */
       else                                    /* ersetzen           */
          for (j=0;j<zahl;j++)                 /* ausgebe von zahl   */
             druck(i,*p+index[i]);             /* Elementen          */
                                               /* legt : freigeben   */
    }                                         /* else x ==0         */
 }                                             /* druckstack         */



                                             /*$P                  */
/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   DRUCK ,  DRUCK2                            */
/*                                                                   */
/*   <parameter>      :    2                                         */
/*                                                                   */
/*      i    : index des Stacks, dessen Element p ausgegeben         */
/*             werden soll. Unter dem Index i sind die               */
/*             Positionierungsinformationen abgelegt.                */
/*      p    : Zeiger auf ein Stackelemnet das ausgegeben werden     */
/*             soll.                                                 */
/*                                                                   */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*      zeile                                                        */
/*      index                                                        */
/*                                                                   */
/*    <r>             :                                              */
/*      full                                                         */
/*      spalte                                                       */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*      printstel               (include)lib                         */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      druckstack                runtime.c                          */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*   druck gibt ein Stackelement an die entsprechende Position aus.  */
/*   In Abhaengigkeit von full wird ein langes oder kurzes           */
/*   Listing erzeugt.                                                */
/*                                                                   */
/*###################################################################*/

 static druck(i,p)

    int i;                                      /* stacknr            */
 STACKELEM * p;                                 /* element-pointer    */
 {

    POS (spalte[i],(zeile[i])--);               /* positionieren +    */
                                                /* zeile hoch!setzen  */
    printf("%3.3d:",index[i]++ + 1);          /* index = hoehe im   */
                                                /* stack ausgeben     */
    PRINTSTEL(*p,full);                         /* full=1 : short     */
                                                /* full=0 : long      */
 }                                              /* druck              */
                                                /* siehe printstel.c */
 static druck2(i,p)


    int i;                                      /* stacknr            */
 STACKELEM * p;                                 /* element-pointer    */
 {
    PTR_DESCRIPTOR ptr;

    POS (spalte[i],(zeile[i])--);               /* positionieren +    */
                                                /* zeile hoch!setzen  */
    printf("%3.3d:",index[i]++ + 1);            /* index = hoehe im   */
                                                /* stack ausgeben     */
    if (T_POINTER(*p) )
    {
       ptr = (PTR_DESCRIPTOR) (*p);             /* print refcount     */
       printf(" %7d R ",ptr->ref_count );
    }
    else
       PRINTSTEL(*p,full);                      /* full=1 : short     */
                                                /* full=0 : long      */
 }                                              /* druck              */
                                                /* siehe printstel.c  */


/*################################################################*/
/*                                                                */
/*   <sourcefile>     :   runtime.c                               */
/*                                                                */
/*   <procedure>      :   POP_DR                                  */
/*                                                                */
/*   <parameter>      :    1                                      */
/*                                                                */
/*      i    : Index des Stacks der gepopped wird                 */
/*                                                                */
/*                                                                */
/*   <global var>     :                                           */
/*                                                                */
/*    <r/w>           :                                           */
/*      index                                                     */
/*      zeile                                                     */
/*      top                                                       */
/*                                                                */
/*    <r>             :                                           */
/*      spalte                                                    */
/*      normanz                                                   */
/*                                                                */
/*   <returns>        :   %                                       */
/*                                                                */
/*   <calls>          :                                           */
/*                                                                */
/*      druckstack          (runtime.c)                           */
/*                                                                */
/*   <called by/in>   :                                           */
/*                                                                */
/*      popstack                         stack.c                  */
/*                                                                */
/*   <function>       :                                           */
/*                                                                */
/*   pop_dr  stellt das poppen eines Elements von Stack i dar.    */
/*           index[i], top[i] und zeile[i] werden entsprechend    */
/*           upgedatet.                                           */
/*                                                                */
/*################################################################*/

 pop_dr(i)

 int i;                                        /* stacknr            */

 {
    if (zeile [i] ==  NULLINE - 1)             /* letztes Element    */
                                               /* auf dem screen     */
    {
       index[i] -= normanz;                    /* um Normanz hoch-   */
                                               /* hochschieben       */
       index[i]--;                             /* index korrigieren  */
       zeile[i] = NULLINE;                     /* bottomline         */
       druckstack(i,normanz);                  /* ausgabe            */
    }
    else                                       /* noch Platz         */
    {
       POS (spalte[i],++zeile[i]);             /* auf letztes Element*/
                                               /* = top of stack     */
       EL_CLEAR;                               /* loesche Element    */
       POS(spalte[i],indexline);               /* aendere top in     */
                                               /* nameline           */
       printf("<%d> Top:<%d> ",stacknr[i],(--top[i]) ? top[i]   : 0 );
       fflush(stdout);
       index[i]--;
    }                                          /* else               */
 }                                             /* pop_dr             */



                                             /*$P                  */
/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   WRITE_DR                                   */
/*                                                                   */
/*   <parameter>      :    2                                         */
/*                                                                   */
/*      i    : Index des Stacks der geschrieben wird                 */
/*      p    : Stackelement, das    geschrieben wird                 */
/*                                                                   */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*      index                                                        */
/*      zeile                                                        */
/*      top                                                          */
/*                                                                   */
/*    <r>             :                                              */
/*      spalte                                                       */
/*      normanz                                                      */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*      druckstack          (runtime.c)                              */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      writestack                        stack.c                    */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/* write_dr  stellt das schreiben des Elements p auf Stack i dar.    */
/*           index[i], top[i] und zeile[i] werden entsprechend       */
/*           upgedatet.                                              */
/*                                                                   */
/*###################################################################*/

 write_dr(i,p)
 int i;                                      /* stacknr            */
 STACKELEM  p;                               /* element to push    */
 {
    POS (spalte[i],(zeile[i]+1));            /* position to top    */
    printf("%3.3d:",index[i]  );             /* topindex           */
    PRINTSTEL(p,1);                          /* Element            */
 }                                           /* write_dr           */

/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   PUSH_DR                                    */
/*                                                                   */
/*   <parameter>      :    2                                         */
/*                                                                   */
/*      i    : Index des Stacks der gepushed wird                    */
/*      p    : Stackelement, das    gepushed wird                    */
/*                                                                   */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*      index                                                        */
/*      zeile                                                        */
/*      top                                                          */
/*                                                                   */
/*    <r>             :                                              */
/*      spalte                                                       */
/*      normanz                                                      */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*      druckstack          (runtime.c)                              */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*       pushstack                        stack.c                    */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*  push_dr  stellt das pushen des Elements p auf Stack i dar.       */
/*           index[i], top[i] und zeile[i] werden entsprechend       */
/*           upgedatet.                                              */
/*                                                                   */
/*###################################################################*/

 push_dr(i,p)
 int i;                                      /* stacknr            */
 STACKELEM  p;                               /* element to push    */
 {
    if (zeile [i] <= lastline)               /* kein Platz mehr    */
                                             /* auf Bildschirm     */
    {
       index[i] -= normanz;                  /* runterschieben     */
       index[i]++;
       zeile[i] = NULLINE;                   /* bottom line        */
       clear(i);                             /* clear screen fuer  */
                                             /* stack i            */
       druckstack(i,normanz);
    }
    else                                     /* noch Platz         */
    {
       POS (spalte[i],(zeile[i])--);        /* position to top    */
       printf("%3.3d:",index[i]++ + 1);     /* topindex           */

       PRINTSTEL(p,1);                       /* Element            */
       POS(spalte[i],indexline);             /* Unterschrift update*/
       printf("<%d> Top:<%d> ",stacknr[i],(++top[i]) ? top[i]   : 0 );
       fflush(stdout);
    }                                        /* else               */
 }                                           /* push_dr            */

                                             /*$P                  */
/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   WHICH_NR                                   */
/*                                                                   */
/*   <parameter>      :    1                                         */
/*                                                                   */
/*      nr   : nr des Stacks, dessen Index gesucht wird              */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*                                                                   */
/*    <r>             :                                              */
/*      stacknr                                                      */
/*                                                                   */
/*   <returns>        :   index in Arrays, unter dem die             */
/*                        Informationen fuer sourcestack nr          */
/*                        abgelegt sind.                             */
/*                                                                   */
/*   <calls>          :   %                                          */
/*                                                                   */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      whichstacks                                                  */
/*                                                                   */
/*                                                                   */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*   whichnr gibt den Index zu sourcestack nr zurueck falls          */
/*   der stack bekannt ist, sonst -1                                 */
/*                                                                   */
/*###################################################################*/

 static  which_nr(nr)

    int nr;                                     /* stacknr            */
 {
    int register i;                             /* Laufvar            */

    for (i=1;i<=stackanz;i++)
       if (stacknr[i] == nr) return(i);        /* gefunden           */
    return(-1);                                /* nicht getraced     */
 }
                                             /*$P                  */
/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   CLEAR                                      */
/*                                                                   */
/*   <parameter>      :    1                                         */
/*                                                                   */
/*      i    : Index des Stack,dessen screen-Bereich                 */
/*             gecleared wird                                        */
/*                                                                   */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*                                                                   */
/*    <r>             :                                              */
/*      spalte                                                       */
/*      lastline                                                     */
/*      normanz                                                      */
/*                                                                   */
/*   <returns>        :   %                                          */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      push_dr                                                      */
/*                                                                   */
/*                                                                   */
/*                                                                   */
/*   <function>       :   loescht Stacks auf dem Bildschirm          */
/*                                                                   */
/*###################################################################*/

 static clear(i)
    int i;
 {
    int register j;                            /* Laufvar            */
    for (j=NULLINE-normanz+1;j>=lastline;j--)
    {
       POS(spalte[i],j);                       /* positionieren      */
       EL_CLEAR;                               /* Element loeschen   */
    }                                          /* for                */
 }                                             /* clear              */

                                             /*$P                  */
/*###################################################################*/
/*                                                                   */
/*   <sourcefile>     :   runtime.c                                  */
/*                                                                   */
/*   <procedure>      :   GET_INDEX                                  */
/*                                                                   */
/*   <parameter>      :    1                                         */
/*                                                                   */
/*      desc : descriptor, dessen index in der Datenstruktur         */
/*             gesucht und zurueckgegeben werden soll                */
/*                                                                   */
/*                                                                   */
/*   <global var>     :                                              */
/*                                                                   */
/*    <r/w>           :                                              */
/*                                                                   */
/*    <r>             :                                              */
/*      stdesc                                                       */
/*      stacknr                                                      */
/*      stackanz                                                     */
/*                                                                   */
/*   <returns>        :   index in Arrays, der zu Stackdescriptor    */
/*                        desc gehoert                               */
/*                                                                   */
/*   <calls>          :                                              */
/*                                                                   */
/*                                                                   */
/*   <called by/in>   :                                              */
/*                                                                   */
/*      display_stack                                                */
/*                                                                   */
/*                                                                   */
/*                                                                   */
/*   <function>       :                                              */
/*                                                                   */
/*    Zuordnung des Stackdescriptors zur Indexnummer auf dem         */
/*    Bildschirm.                                                    */
/*                                                                   */
/*###################################################################*/

 static get_index(desc)

    StackDesc * desc;                           /* stackdescriptor */

 {
    int register i;                             /* Hilfsvariable   */

    for (i=1;i<=stackanz;i++)
       if (stdesc[stacknr[i]] == desc)          /* found           */
    {
       return(i);
    }

    for (i=1;i<=stacknr[0];i++)                /*gibts den ueberhaupt*/
       if (stdesc[i] == desc) return (0);      /* gibts, aber nicht  */
                                               /* getraced           */
    post_mortem("get_index: Stack existiert nicht");  /* gibts nicht */

 }                                             /* get_index          */


/*----------------------------------------------------------------------------
 *   set_full :    zeigt einen Stack im Full-Mode
 *--------------------------------------------------------------------------
 */
 static set_full(st)
    int st;
 {
    register int j;

    full = 0;
    screenupdate = TRUE;                    /* Bildschirmupdate vor der */
    mode = CMD_DISPLAY;                     /* naechsten Aktion         */

    clearscreen();                          /* Bildschirm               */
    NEWMODUL;
    NEWMODE;
    NEWDES;
    POS(1,XLINE);
    for (j=0; j<=79; j++) printf("-");      /* Stack drucken            */
    spalte[st] = 1;
    index[st] = -maxanz - 1;
    druckstack(st,maxanz);
 }


/*--------------------------------------------------------------------------
 *  getstack  --  bestimmt die Hoehe und Boden des verlangten Stacks
 *-------------------------------------------------------------------------
 */
 void getstack(bottom,top,stackdesc)
    STACKELEM **bottom;
 int * top;
 StackDesc * stackdesc;
 {
    int i;

    i = (int) (stackdesc->TopofStack - stackdesc->MarBotPtr);
    *top = ((*top != 0) && (*top < i))
    ? *top : i;
    if (*top == 0)
    {
       *top = 0;
       *bottom = NULL;
    }
    else
       *bottom = stackdesc->MarBotPtr;
    return;
 }
                                             /*$P                  */

/*---------------------------------------------------------------------------
 *  printsteps  --  gibt die Anzahl der bisher durchgefuehrten Stackop's aus
                    Diese Zaehlung erfolgt unabhaengig von MEASURE
 *---------------------------------------------------------------------------
 */
 printsteps(fp)
 FILE * fp;
 {
    int i, status;

    fprintf(fp,"     Anzahl der bisher durchgefuehrten Stackoperationen:\n\n");

    fprintf(fp,"Preprocessing:\n");
    fprintf(fp,"pop:     %d\n",i_pop[0]);
    fprintf(fp,"push:    %d\n",i_push[0]);
    fprintf(fp,"read:    %d\n",i_read[0]);
    fprintf(fp,"write:   %d\n",i_write[0]);
    i = i_pop[0] + i_push[0] + i_read[0] + i_write[0];
    fprintf(fp,"gesamt:  %d\n\n",i);

    fprintf(fp,"Processing:\n");
    fprintf(fp,"pop:     %d\n",i_pop[1]);
    fprintf(fp,"push:    %d\n",i_push[1]);
    fprintf(fp,"read:    %d\n",i_read[1]);
    fprintf(fp,"write:   %d\n",i_write[1]);
    i = i_pop[1] + i_push[1] + i_read[1] + i_write[1];
    fprintf(fp,"gesamt:  %d\n\n",i);

    fprintf(fp,"Postprocessing:\n");
    fprintf(fp,"pop:     %d\n",i_pop[2]);
    fprintf(fp,"push:    %d\n",i_push[2]);
    fprintf(fp,"read:    %d\n",i_read[2]);
    fprintf(fp,"write:   %d\n",i_write[2]);
    i = i_pop[2] + i_push[2] + i_read[2] + i_write[2];
    fprintf(fp,"gesamt:  %d\n",i);
 }


/*----------------------------------------------------------------------------
 *  slow  --  Verlangsamungsfaktor fuer die bildliche Stackdarstellung
 *----------------------------------------------------------------------------
 */
 slow()
 {
    float x[21];
    int i,j;

    for (i = 1; i < 100; i++)               /* Leerlaufhandlung */
       for ( j = 1; j <=20 ;j++)
          x[j] = i / j;
 }


 /*---------------------------------------------------------------------------
  *   dr  --  druckt Hardcopy der Stacks in einen File
  *--------------------------------------------------------------------------
  */
 static dr(listfile)
    FILE * listfile;
 {

    STACKELEM * hi[10];                         /* adr(beginn) of     */
    char        line [120];
    char        line1 [20];
    int max;
                                                /* stack              */
    int register j;                             /* laufvariable       */
    int register i;                             /* laufvariable       */


    fprintf(listfile,"H A R D C O P Y of stacks in %s\n\n",modullist[modulnr-1]);

    max = 0;
    for (i=1;i<=stacknr[0];i++)
    {
       hi[i] = (stdesc[i]->MarBotPtr) + 1;
       max = (max > SIZEOFSTACK(*stdesc[i])) ? max : SIZEOFSTACK(*stdesc[i]);
    }

    for (j=max;j>=0;j--)
    {
       for (i=1;i<=stacknr[0];i++)
          if ( j >= SIZEOFSTACK(*stdesc[i])) fprintf(listfile,"%17s"," ");
          else
       {
          sprintf(line    ,"%3.3d:",j);          /* index = hoehe im   */
                                                 /* stack ausgeben     */
          s_prstel(line1   ,hi[i][j],1);         /* full=1 : short     */
          strcat(line,line1);                    /* printstel.c        */
          fprintf(listfile,"%-17s",line);
       }
       fprintf(listfile,"\n");
    }
    for (i=1;i<=(int)stdesc[0];i++)

       for (i=1;i<=130;i++) fprintf(listfile,"-");

    fprintf(listfile,"\n");

    for (i=1;i<=(int)stdesc[0];i++)
    {
       sprintf(line,"<%d> Top:<%d>",i,(SIZEOFSTACK(*stdesc[i])) ? SIZEOFSTACK(*stdesc[i]) : 0);
       fprintf(listfile,"%-17s",line);
    }
    fprintf(listfile,"\n");
                                                /*index und tiefe     */
    for (i=1;i<=stacknr[0];i++)
       fprintf(listfile,"%-17s",stdesc[i]->Name);   /* name               */

    fprintf(listfile,"\n\n");
 }

 /*-----------------------------------------------------------------------------
  *  test_monitor  --  prueft anhand der durchgefuehrten stacksteps, ob
  *                    monitor_call zur Befehlseingabe aufgerufen werden soll.
  *                    Falls erforderlich, wird Deskriptoranzeige erneuert.
  *-----------------------------------------------------------------------------
  */
 test_monitor()
 {
    if (mode == CMD_DISPLAY)
        if (descnt != screen_descnt)        /* desc-Zahl nicht aktuell? */
           NEWDES;
    if (stepdifstack)                       /* soll kontrolliert werden? */
       if (++stackstep >= stepdifstack)     /* ist Anzahl abgelaufen ?   */
          monitor_call();                   /* command-Schleife          */
 }

 /*----------------------------------------------------------*/
 /*  pm_test_monitor  --  prueft anhand der durchgefuehrten    */
 /*            pm_amsteps, ob monitor_call zur Befehlseingabe   */
 /*aufgerufen werden soll.                                      */
 /*-----------------------------------------------------------*/
 pm_test_monitor()
 {
    if (pm_stepdifam)                       /* soll kontrolliert werden? */
       if (++pm_amstep >= pm_stepdifam)     /* ist Anzahl abgelaufen ?   */
          monitor_call();                   /* command-Schleife          */
 }

 /*------------------------------------------------------------------------------
  *  getzeile  --  holt eine neue Eingabezeile vom Bildschirm. Dazu wird der
  *                Text p (z.B. <cmd>) geschrieben und der curser auf dessen
  *                Anfang positioniert. p ist fuer den Benutzer eine Hilfe-
  *                stellung, was erwartet wird. Dann wird bis zum ersten
  *                '\n' eingelesen in den Puffer line
  *-------------------------------------------------------------------------------
  */
 int getzeile(p)
    char * p;
 {
    int i;

    NEWCMD;
    printf("%s",p);                                /* Hilfstext schreiben */
    fflush(stdout);
    if (mode == CMD_DISPLAY)
       POS(INPSPALTE,CMDLINE);
    else
/*  {  putchar('\n'); cursor_up();  }   $$$ */      /* Monitormode  */
    {  printf(":"); fflush(stdout); }

    for (i=0; (line[i++]=getchar() ) != '\n'; )      /* Zeile holen */
       ;
    line[i] = '\0';
    p_line = line;                          /* Pointer auf Pufferanfang */
    if (mode == CMD_DISPLAY)   NEWERR;      /* ev. alte Fehlermeldung   */
    return(1);                              /* loeschen  */
 }


 /*---------------------------------------------------------------------------
  *  ispstring  --  prueft,ob *pz der naechste Ausdruck in der Eingabezeile
  *                (line[]) ist. Dabei kann *pz bis auf zahl Zeichen
  *                abgekuerzt sein. Bei erfolgreichen Lesen wird der Ausdruck
  *                entfernt, sonst nicht!!!  Dabei simuliert das Weiter-
  *                schieben von p_line das Entfernen. Steht p_line auf
  *                '\n', so ist der Puffer logisch leer. a enthaelt den
  *                Text, den der Benutzer erhaelt, wenn er eine weitere
  *                Eingabe machen muss
  *---------------------------------------------------------------------------
  */
 int ispstring(pz,zahl,a)
    char * pz;                     /* gesuchte Eingabe  */
 int zahl;                         /* Mindestlaenge     */
 char * a;                         /* Ausgabe in Kommandozeile */
 {
    char *s, *t;

    for ( ; isspace( *p_line); )   /* nix da? Blanks entfernen */
       if (*p_line++  ==  '\n')
          getzeile(a);
    for (s = p_line,  t = pz;  *s == *t; s++,t++)    /* lauf bis zum ersten */
       ;                                             /* Unterschied         */
    if ( !isspace(*s))
       return(0);
    if ((s - p_line) < zahl)                         /* pz richtig, aber zu */
       return (0);                                   /* kurz                */
    p_line = s;                                      /* Ausdruck aus Zeile  */
    return(1);                                       /* loeschen            */
 }


 /*----------------------------------------------------------------------------
  *  isstring  --  ruft ispstring auf und versieht es mit der Standart-
  *                eingabeaufforderung "<cmd>" als Hilfestellung. Sonst
  *                muesste in monitor_call in jedem isstring "<cmd>"
  *                als 3. Parameter ergaenzt werden.
  *----------------------------------------------------------------------------
  */
 isstring(pz,zahl)
 char * pz;               /* gewuenschte Eingabe */
 int zahl;
 {
    return(ispstring(pz,zahl,"<cmd>"));
 }


 /*----------------------------------------------------------------------------
  *  isint  --  prueft, ob in der Eingabezeile eine nat. Zahl vorliegt.
  *             Wenn ja, wird diese Zahl in zahl zurueckgegeben und in der
  *             Eingabezeile entfernt. Liegt keine Zahl vor, so wird die
  *             Eingabe entfernt.
  *----------------------------------------------------------------------------
  */
 int isint(zahl)
    int * zahl;
 {
    char register *s;
    BOOLEAN minus = FALSE;

    *zahl = 0;
    for ( ; isspace(*p_line); )
       if (*p_line++  == '\n')     /* nix da? Blanks ueberlesen */
          getzeile("<int>");       /* neue Eingabe anfordern mit der */
    s = p_line;                    /* Meldung ,dass ein int erwartet */
    if (*s == '-')                 /* wird */
    {
       minus = TRUE;               /* Zahl ist negativ */
       s++;
    }
    while (!isspace(*s))           /* wenn keine Ziffer */
       if (!isdigit(*s))
    {
       for ( ; ;s++)
          if (isspace(*s)) break;  /* Eingabe entfernen bis zum */
       p_line = s;                 /* naechsten blank */
       return(0);                  /* gescheitert */
    }
    else
       *zahl = *zahl * 10  +  (*s++ - '0');    /* naechste Ziffer */
    p_line = s;                    /* alte Zahl um eine Stelle nach links */
    if (minus)  *zahl = -*zahl;    /* schieben, neue Ziffer hinzuaddieren */
    return(1);                     /* int erfolgreich gelesen */
 }

 /*----------------------------------------------------------------------------
  *  ishex  --  prueft, ob in der Eingabezeile eine hex. Zahl vorliegt.
  *             Wenn ja, wird diese Zahl in zahl zurueckgegeben und in der
  *             Eingabezeile entfernt. Liegt keine Zahl vor, wird die Eingabe
  *             entfernt.
  *----------------------------------------------------------------------------
  */
 int ishex(zahl)
    int * zahl;
 {
    char register *s;

    *zahl = 0;
    for ( ; isspace(*p_line); )
       if (*p_line++  == '\n')     /* nix da? Blanks entfernen */
          getzeile("<hex>");       /* neue Eingabe anfordern mit der Meldung */
    s = p_line;                    /* dass eine Hexzahl erwartet wird */
    while (!isspace(*s))           /* keine Hexziffer ? */
       if (!isdigit(*s) && ( (*s < 'a') || (*s > 'f') ) )
    {
       for ( ; ;s++)
          if (isspace(*s)) break;  /* Eingabe entfernen bis zum naechsten blank */
       p_line = s;
       return(0);                  /* gescheitert */
    }
    else
    {
       *zahl = *zahl * 16;         /* naechste Hexziffer */
       if (isdigit(*s) )           /* alte Zahl eine Stelle nach links */
          *zahl += (*s++ - '0');   /* schieben, neue Ziffer hinzuaddieren */
       else
          *zahl += (*s++ - 'a' + 10);
    }
    p_line = s;                    /* Zahl aus Puffer loeschen */
    return(1);                     /* Hexzahl erfolgreich eingelesen */
 }



 /*----------------------------------------------------------------------------
  *  getstring  --  liest das naechste Wort aus der Eingabezeile und gibt es
  *                 zurueck. Es wird keine Kontrolle durchgefuehrt, worum
  *                 es sich handelt.
  *----------------------------------------------------------------------------
  */
 char * getstring(p)
 char * p;                         /* notfalls auszugebender Hilfstext */
 {
    char *u;
    char register *s;
    char register *t;

    for ( ;isspace(*p_line); )     /* blanks entfernen */
       if (*(p_line++)  ==  '\n')  /* nix da?  */
          getzeile(p);             /* neue eingabe anfordern */
    if (( t = u = (char *) calloc(25,sizeof(char))) == NULL)    /* Platz holen */
       printf          ("getstring: calloc fehlgeschlagen\n");
    for (s = p_line; !isspace(*t = *s); s++, t++)
       ;                           /* gewuenschtes Wort kopieren */

    *t = '\0';
    p_line = s;                    /* Wort aus Eingabezeile 'entfernen' */
    return(u);
 }



 /*------------------------------------------------------------------------------
  *  isstackname  --  prueft, welcher Stackname in der Eingabe vorliegt.
  *                   Ist kein bekannter Stack vorhanden, wird die Eingabe
  *                   ueberlesen
  *-----------------------------------------------------------------------------
  */
 StackDesc * isstackname()
 {
    if (ispstring("e",1,"<stack>"))  return(&S_e);   /* Ist 'e' naechstes Wort im Puffer? */
    if (ispstring("a",1,"<stack>"))  return(&S_a);
    if (ispstring("m",1,"<stack>"))  return(&S_m);
    if (ispstring("m1",2,"<stack>")) return(&S_m1);
    if (ispstring("v",1,"<stack>")) return(&S_v);
    if (ispstring("hilf",1,"<stack>")) return(&S_hilf);
    if (ispstring("i",1,"<stack>")) return(&S_i);

    if (ispstring("pm",1,"<stack>"))  return(&S_pm);
    WRONGINPUT(isstackname: Stack unbekannt);
    for ( ; !isspace(*p_line); p_line++)             /* alte Eingabe entfernen */
       ;
    return(NULL);
 }


/*-------------------------------------------------------------------------------
 *   getstacknr  --  gibt zu einen Stacknamen die zugehoerige aktuelle Nummer an
 *                   Sie entspricht der aktuellen Reihenfolge auf dem Bildschirm
 *-------------------------------------------------------------------------------
 */
 getstacknr()
 {
    char register * cp;
    int register i;

    cp = getstring("<stack>");     /* Stackname holen */
    for (i = 1; i <= stacknr[0]; i++)
       if ( strcmp(cp,stdesc[stacknr[i]]->Name) == 0)  /* Name mit vorhandenem */
    {                                                  /* Namen vergleichen    */
       free(cp);
       return(i);
    }
    return(0);
 }

/*---------------------------------------------------------------------------
 *   getstackindex  --  gibt zu einen Stacknamen die zugehoerige Nummer an
 *                      die der internen Reihenfolge entspricht
 *---------------------------------------------------------------------------
 */
 getstackindex()
 {
    char register * cp;
    int register i;

    cp = getstring("<stack>");              /* Stacknamen holen */
    for (i = 1; i <= stacknr[0]; i++)
       if ( strcmp(cp,stdesc[i]->Name) == 0)  /* Name mit vorhandenen Namen */
    {                                         /* vergleichen */
       free(cp);                   /* Stacknamen freigeben */
       return(i);
    }
    return(0);
 }

/*-----------------------------------------------------------------------------
 *  monitor_call  --  nimmt Befehle entgegen und steuert die Ausfuehrung!
 *                    also die command-Schleife
 *-----------------------------------------------------------------------------
 */
 monitor_call()
 {
    char c;
    char *p, *getstring();
    STACKELEM data;
    StackDesc  *st, *isstackname();
    int i, si, si1, si2;

    pm_amstep = 0;
    stackstep = 0;
    pm_stepdifam = 0;
    stepdifstack = 0;
    getzeile("<cmd>");                      /* neuen Befehl anfordern */
    while (1)
/**********************************************************************/
/*   return :   monitor_call verlassen, die Reduktion laeuft weiter   */
/**********************************************************************/
       if (isstring("return",3))
    {
       if (screenupdate) setdisplay();
       return;                              /* ev. Bildschirm updaten */
    }

/**********************************************************************/
/*    cat :   listet eine Datei auf dem Bildschirm                    */
/**********************************************************************/
    else if  (isstring("cat",2))
    {
       setmonitor();                                 /* Monitormode */
       if ((p = getstring("<datei>")) != NULL)       /* Dateinamen holen */
          if ((fp = fopen(p,"r")) != NULL)
          {
             while ((c = getc(fp)) != EOF) putchar(c); /* Datei auf Bildschirm */
             fclose(fp);                               /* schreiben */
             free(p);                                /* Dateinamen */
          }
          else
             WRONGINPUT(Eingabedatei nicht geoeffnet);
       else
          WRONGINPUT(ungueltiger Dateiname);
    }
/**********************************************************************/
/*    pmdump :   listet eine Datei auf dem Bildschirm                    */
/**********************************************************************/
    else if  (isstring("pmdump",2))
    { char * q ;
       setmonitor();                                 /* Monitormode */
       if ((q = p = getstring("<datei>")) != NULL)       /* Dateinamen holen */
       {
        if (p[0] == '%') {
                 p = q + 1 ;
                pmdmpcrs  = 1 ;
        }
        else {
        pmdmpcrs = 0 ;
        }
        if (!strcmp(p,"stdout"))
        {
          if (pmdmpopen) fclose(pmdmpfile);
          pmdmpopen = 0 ;
          pmdmpfile = stdout ;
         }
         else
          if ((fp = fopen(p,"w")) != NULL)
          {
                 if (pmdmpopen) fclose(pmdmpfile) ;
                 pmdmpfile  = fp ;
                pmdmpopen    = 1 ;
          }
          else
            {
             WRONGINPUT(Unable to open Dumpfile  );
          if (pmdmpopen) fclose(pmdmpfile);
          pmdmpopen = 0 ;
          pmdmpfile = stdout ;
        }
        free(q);
    }
       else
          WRONGINPUT(ungueltiger Dateiname);
  }
/************************************************************************/

/************************************************************************/
/*    continue :    fortfahren im Monitor-Mode                          */
/************************************************************************/
    else if  (isstring("continue",2))
    {
       setmonitor();               /* Monitormode */
       return;
    }

/************************************************************************/
/*   break :   setzen eines Breakpoints auf Modul oder redcount         */
/************************************************************************/
    else if  (isstring("break",2))
    {
       p = getstring("<modul>");            /* hole Modulname */
       if (strcmp(p,"red") == 0)            /* break auf redcount */
          isint(&breakred);                 /* Zahl holen */
       else
          set_breakpoint(p);                /* breakpint.c */
       free(p);                            /* Modulname */
    }

/************************************************************************/
/*   delete :   loeschen eines Breakpoints                              */
/************************************************************************/
    else if  (isstring("delete",3))
        {
       if (ispstring("all",3,"<zu loeschen>"))      /* alle Breakpoints */
          clear_breakpoints();              /* breakpoint.c */
       else
           {
            p = getstring("<modul>");
            del_breakpoint(p);              /* breakpoint.c */
            free(p);                       /* Modulname */
           }
         }

/************************************************************************/
/*   listbreakpoints :    aktuelle Breakpoints zeigen                   */
/************************************************************************/
    else if  (isstring("listbreakpoints",3))
    {
       screenupdate = TRUE;
       list_breakpoint();                   /* breakpoint.c */
    }

/************************************************************************/
/*   module :    aktuelle Aufrufhierarchie zeigen                       */
/************************************************************************/
    else if  (isstring("module",3))
    {
       screenupdate = TRUE;
       list_moduls(stdout);                 /* breakpoints.c */
    }

/************************************************************************/
/*   display :     Displaymodus einschalten und Stackbild neu aufbauen  */
/************************************************************************/
    else if  (isstring("display",2))
    {
       screenupdate = TRUE;
       setdisplay();
    }

/*************************************************************************/
/*   messwerte : gibt die Anzahl der bisher durchgefuehrten Stackop's an */
/*               und zwar unabhaengig von MEASURE                        */
/*************************************************************************/
    else if  (isstring("messwerte",3))
    {
       setmonitor();               /* Monitormode */
       printsteps(stdout);
    }

/************************************************************************/
/*   prmesswerte:  gibt Anzahl Stackop's in stack.dump aus und zwar     */
/*                 unabhaengig von MEASURE
/************************************************************************/
    else if  (isstring("prmesswerte",3))
       if (!flagstack)                    /* stack:dump schonmal benutzt? */
          if ((fp = fopen("stack.dump","w")) != NULL)
          {
             setmonitor();                  /* Monitormode */
             printsteps(fp);
             fclose(fp);
             flagstack = TRUE;              /* jetzt ist stack:dump benutzt */
          }
          else
             WRONGINPUT(Datei stack.dump konnte nicht geoeffnet werden);
       else
          if ((fp = fopen("stack.dump","a")) != NULL)
          {
             setmonitor();                  /* Monitormode */
             printsteps(fp);
             fclose(fp);
          }
          else
             WRONGINPUT(Datei stack.dump konnte nicht geoeffnet werden);

/******************************************************************************/
/*   protokoll :   druckt Messergebnisse des Messsystemsin protokoll:mess aus */
/******************************************************************************/
   else if  (isstring("protokoll",3))
#if MEASURE
        druck_measure();                    /* breakpoint.c */
#else
        WRONGINPUT(Messystem nicht dazugebunden!);
#endif


/**************************************************************************/
/*    countdesc :   zeigt die Anzahl der zur Zeit vergebenen Deskriptoren */
/*                  es zaehlen aber nur die durch den Benutzer verur-     */
/*                  sachten Deskriptoren                                  */
/**************************************************************************/
   else if  (isstring("countdesc",3))
   {
      if (mode == CMD_DISPLAY)
         POS(ERRSPALTE,ERRLINE);
      printf("zur Zeit sind %d Deskriptoren verbraucht\n",descnt - stdesccnt);
      if (mode == CMD_MONITOR) printf("\n");
   }


/************************************************************************/
/*    refcount :    zeigt im Stackbild statt der Pointer deren Refcount */
/************************************************************************/
    else if  (isstring("refcount",3))
    {
       screenupdate = TRUE;        /* Bildschirmupdate */
       debrefcount = TRUE;         /* refcount zeigen */
       format();                   /* Stackbild neu aufbauen */
    }

/************************************************************************/
/*   pop :    Durchfuehren einer pop-Operation                          */
/************************************************************************/
    else if  (isstring("pop",2))
    {
       if (mode != CMD_MONITOR)
          if ((st = isstackname() ) != NULL)       /* Stackname holen */
             POPSTACK(*st);
          else;
       else WRONGINPUT(nur im Displaymode erlaubt);
    }

/***********************************************************************/
/*   full :    einen Stack im Fullmodus zeigen                         */
/***********************************************************************/
    else if  (isstring("full",2))
       if (( fullstack = getstacknr() ))    /* Stacknummer holen */
          set_full(fullstack);
       else WRONGINPUT(es koennen nur Stacks vom Bildschirm gezeigt werden);

/************************************************************************/
/*   run :    n Stackoperationen durchfuehren. Die Kontrolle uebernimmt */
/*            test_monitor                                              */
/************************************************************************/
    else if  (isstring("run",1))
       if (isint(&stepdifstack))            /* Schrittzahl holen */
       {
          if (screenupdate || mode != CMD_DISPLAY) setdisplay();
          return;                           /* ev. Displaymode einschalten */
       }
       else
          WRONGINPUT(keine Zahl);

/************************************************************************/
/*   s :    Singlestepmodus: fuehre eine Stackoperation durch. Die      */
/*          Kontrolle uebernimmt test_monitor                           */
/************************************************************************/
    else if  (isstring("s",1))
    {
       if (screenupdate ||  mode != CMD_DISPLAY) setdisplay();
       stepdifstack = 1;
       return;
    }
/************************************************************************/
/*   am: :    Singlestepmodus: fuehre eine Mascinenbefehl durch. Die      */
/*          Kontrolle uebernimmt test_monitor                           */
/************************************************************************/
    else if  (isstring("a",1))
    {
       if (screenupdate ||  mode != CMD_DISPLAY) setdisplay();
       pm_stepdifam = 1;
       return;
    }


/************************************************************************/
/*   bottom :   unteres Ende eines Stacks zeigen                        */
/************************************************************************/
    else if  (isstring("bottom",2))
       if (mode != CMD_MONITOR)
          if (i = getstacknr())    /* Stack holen */
          {
             if (mode != CMD_DISPLAY) setdisplay();
             index[i] = 0;                          /* Positionieren */
             druckstack(i,maxanz);
             screenupdate = TRUE;
          }
          else WRONGINPUT(Stack unbekannt);
       else WRONGINPUT(im Monitor-Mode nicht erlaubt);

/************************************************************************/
/*   top :     zeigt den Top eines Stacks                               */
/************************************************************************/
    else if  (isstring("top",2))
       if (mode != CMD_MONITOR)
          if (i = getstacknr() )            /* Stack holen */
          {
             if (mode != CMD_DISPLAY) setdisplay();
             index[i] = -maxanz - 1;                 /* Positionieren */
             druckstack(i,maxanz);
          }
          else WRONGINPUT(Stack unbekannt);
       else WRONGINPUT(im Monitor-Mode nicht erlaubt);

/***********************************************************************/
/*   down :    stack um <n> nach unten schieben                        */
/***********************************************************************/
    else if  (isstring("down",2))
       if (mode != CMD_MONITOR)
          if (si = getstacknr() )           /* Stacknummer holen */
             if (isint(&i))
             {
                if (mode != CMD_DISPLAY) setdisplay();
                index[si] -= (i + maxanz);           /* Positionieren */
                if (index[si] < 0) index[si] = 0;
                druckstack(si,maxanz);
             }
             else WRONGINPUT(keine Zahl fuer down);
          else WRONGINPUT(Stack unbekannt);
       else WRONGINPUT(im Monitor-Mode nicht erlaubt);

/***********************************************************************/
/*    up :    stack um <n> nach oben schieben.                         */
/***********************************************************************/
    else if  (isstring("up",2))
       if (mode != CMD_MONITOR)
          if (si = getstacknr() )           /* Stacknummer holen */
             if (isint(&i))
             {
                if (mode != CMD_DISPLAY) setdisplay();
                index[si] += (i - maxanz);           /* Positionieren */
                druckstack(si,maxanz);
             }
             else WRONGINPUT(keine Zahl fuer up);
          else WRONGINPUT(Stack unbekannt);
       else WRONGINPUT(im Monitor-Mode nicht erlaubt);

/************************************************************************/
/*   trav :    traversiert einen Ausdruck                               */
/************************************************************************/
    else if  (isstring("trav",2))
    {
       si1 = getstacknr();                  /* Stacknummern holen */
       si2 = getstacknr();
       if (( si1  ) && (si2 ))
       {
          if (screenupdate) format();
          trav(stdesc[stacknr[si1]], stdesc[stacknr[si2]]);   /* traversieren */
       }
       else WRONGINPUT(Fehler: Unbekannter Stack);
    }

/************************************************************************/
/*    which :     Displayfolge der Stacks neu bestimmen                 */
/************************************************************************/
    else if  (isstring("which",2))
       if (mode == CMD_DISPLAY)
          which();                 /* neue Stacks abfragen */
       else
       WRONGINPUT(nicht erlaubt im Displaymode);

/************************************************************************/
/*   slow :     Verlangsamung der Bildschirmoperationen                 */
/************************************************************************/
    else if (isstring("slow",2))
       if (isint(&slowfac)) ; else WRONGINPUT(keine Zahl);

/***********************************************************************/
/*  set :    setzt Debug-variablen                                     */
/***********************************************************************/
    else if  (isstring("set",2))
    {
       if (ispstring("debconf",5,"<debvar>")) debconf = 1;
       else if (ispstring("debcnt",5,"<debvar>")) debcnt = 1;

       else if (ispstring("outbefehl",9,"<debvar>")) outbefehl = 1;  /* P A T T E R N   M A T C H I N G */

       else if (ispstring("outstep",5,"<debvar>")) outstep = 1;      /* P A T T E R N  M A T C H I N G */


    }




/************************************************************************/
/*   reset :    setzt Debugvariable zurueck                             */
/************************************************************************/
    else if  (isstring("reset",3))
    {
       if  (ispstring("debconf",5,"<debvar>")) debconf = 0;
       else if  (ispstring("debcnt",5,"<debvar>")) debcnt = 0;

       else if (ispstring("outbefehl",9,"<debvar>")) outbefehl = 0;  /* P A T T E R N   M A T C H I N G */

       else if (ispstring("outstep",5,"<debvar>")) outstep = 0;      /* P A T T E R N  M A T C H I N G */

       else if  (ispstring("all",2,"<debvar>"))
       {
          debconf = 0;
          debcnt = 0;
       }
    }

/************************************************************************/
/*   desc :    Zeigen eines Deskriptorinhaltes                          */
/************************************************************************/
    else if  (isstring("desc",2))
    {
       PTR_DESCRIPTOR pdesc;
       int k;

       if ( ishex(&pdesc) != 0 )            /* Deskriptor holen */
       {
          if ( (pdesc > EnHeapD)  || (pdesc < StHeapD) || !T_POINTER((STACKELEM)pdesc) )
             WRONGINPUT(Illegal Descriptor);    /* wirklich ein Deskriptor? */
          else
             if (isint(&k) == 0)                     /* Ausgabetiefe */
                WRONGINPUT(Tiefe falsch angegeben);
             else
             {
                setmonitor();                        /* Monitormode */
                DescDump(stdout,pdesc,k);            /* descdump.c */
             }
       }
       else WRONGINPUT(keine Hexzahl);
    }

/************************************************************************/
/* stdesc :    Zeigen eines Deskriptorinhaltes, wobei das oberste Stack-*/
/*             element als Deskriptor betrachtet wird.
/************************************************************************/
    else if  (isstring("stdesc",2))
    {
       PTR_DESCRIPTOR pdesc;
       int k;

       if (( st = isstackname() ) != NULL)  /* Stack holen */
          if (SIZEOFSTACK(*st) && T_POINTER((data = READSTACK(*st))))
          {                                 /* liegt Pointer auf Stack? */
             if ( (data > (STACKELEM)EnHeapD)  || (data < (STACKELEM)StHeapD) )
                WRONGINPUT(Illegal Descriptor);      /* ist Pointer Deskriptor? */
             else
                if (isint(&k) == 0)         /* Ausgabetiefe */
                   WRONGINPUT(Tiefe falsch angegeben);
                else
                {
                   setmonitor();            /* Monitormode */
                   DescDump(stdout,data,k); /* descdump.c */
                }
          }
          else WRONGINPUT(Stacktop ist kein Pointer oder nicht vorhanden);
       else WRONGINPUT(unbekannter Stack);
    }

/************************************************************************/
/*   pdesc :    Zeigen eines Deskriptorinhaltes auf Datei               */
/************************************************************************/
    else if  (isstring("pdesc",2))
    {
       PTR_DESCRIPTOR pdesc;
       int k;

       if ( ishex(&pdesc) != 0 )            /* Deskriptor holen */
       {
          if ( (pdesc > EnHeapD)  || (pdesc < StHeapD) )
             WRONGINPUT(Illegal Descriptor);       /* wirklich Deskriptor? */
          else
             if (isint(&k) == 0)                   /* Asugabetiefe */
                WRONGINPUT(Tiefe falsch angegeben);
             else
                if ((p = getstring("<datei>")) != NULL)       /* Dateiname holen */
                   if ((fp = fopen(p,"a")) != NULL)
                   {
                      DescDump(fp,pdesc,k);          /* descdump.c */
                      free(p);                       /* Dateiname */
                      fclose(fp);
                   }
                   else
                      WRONGINPUT(Datei nicht eroeffnet);
                else
                   WRONGINPUT(Dateiname nicht eingelesen);
       }
       else WRONGINPUT(keine Hexzahl);
    }

/************************************************************************/
/*      pte :     ein Stackexpression im heap drucken                   */
/************************************************************************/
    else if  (isstring("pte",2))
    {
       int *adr;
       int  i,j;

       setmonitor();               /* Monitormode */
       if ( ishex(&j))             /* Adresse holen */
       {
          adr = (int *)j;
          for (i=1; i <= adr[0]; ) /* einzelne Elemente drucken */
             printstel(adr[i++],0);         /* printstel.c */
       }
       else
          WRONGINPUT(keine Hexzahl);
    }

/***********************************************************************/
/*   test :    Test, ob heapchain und Descriptorchain in Ordnung sind  */
/***********************************************************************/
    else if  (isstring("test",2))
    {
       hpchain();                  /* heap.c */
       deschain();                 /* heap.c */
       WRONGINPUT(Heap ist ok);
    }

/***********************************************************************/
/*   compact :   fuehrt heap-compactation durch                        */
/***********************************************************************/
    else if  (isstring("compact",2))
       if (compheap() )                                       /* heap.c */
          WRONGINPUT(compact ordentlich durchgefuehrt);
       else
       WRONGINPUT(Heap war bereits kompakt);

/***********************************************************************/
/*    isdesc :    prueft, ob Pointer ein Deskriptorpointer ist         */
/***********************************************************************/
    else if  (isstring("isdesc",3))
       if (ishex(&i))                       /* Deskriptor holen */
          if (isdesc(i))                    /* heap.c         */
             WRONGINPUT(ist Pointer auf einen Deskriptor);
          else
             if (inchain(i))                /* heap.c */
                WRONGINPUT(ist Pointer auf freien Deskriptor);
       else
       WRONGINPUT(ist kein Pointer auf einen Deskriptor);
    else
       WRONGINPUT(keine Hexzahl);

/***********************************************************************/
/*    isheap :    prueft, ob Pointer ein Heappointer ist               */
/***********************************************************************/
    else if  (isstring("isheap",3))
       if (ishex(&i))                       /* Adresse holen */
          if (isheap(i - 1))                /* heap.c */
             WRONGINPUT(ist Pointer in den Heap);
          else
          WRONGINPUT(ist kein Pointer in den Heap);
       else
       WRONGINPUT(keine Hexzahl);

/***********************************************************************/
/*    heap :    erzeugt Dump des heaps auf der Datei heap.dump         */
/***********************************************************************/
    else if  (isstring("heap",2))
    {
       if (flagheap)      /* wurde heap:dump schon benutzt? */
       {
          if ((DumpFile = fopen("heap.dump","a")) != NULL)
          {
             HeapDump("Testphase");         /* heap.c */
             fclose(DumpFile);
          }
          else
             WRONGINPUT(heap:dump konnte nicht geoeffnet werden);
       }
       else
       {
          if ((DumpFile = fopen("heap.dump","w")) != NULL)
          {
             HeapDump("Testphase");         /* heap.c */
             fclose(DumpFile);
             flagheap = TRUE;               /* jetzt ist heap:dump benutzt */
          }
          else
             WRONGINPUT(heap:dump konnte nicht geoeffnet werden);
       }
    }

/***********************************************************************/
/*     stacks  :     Ausgabe der Stacks auf Datei                      */
/***********************************************************************/
    else if  (isstring("stacks",3))
       if (flagstack)              /* stack:dump schon benutzt? */
       {
          if (( fp = fopen("stack.dump","a")) != NULL)
          {
             dr(fp);               /* Stacks drucken */
             fclose(fp);
          }
          else WRONGINPUT(Datei konnte nicht geoeffnet werden);
       }
       else
       {
          if (( fp = fopen("stack.dump","a")) != NULL)
          {
             dr(fp);               /* Stacks drucken */
             fclose(fp);
             flagstack = TRUE;     /* jetzt ist stack:dump benutzt */
          }
          else WRONGINPUT(Datei konnte nicht geoeffnet werden);
       }

/*************************************************************************/
/*     dump :   protokolliert heap, stack und Aufrufhierarchie auf Datei */
/*************************************************************************/
    else if  (isstring("dump",2))
       if (flagdump)                        /* ist dump:dump schon benutzt? */
       {
          if ((DumpFile = fopen("dump.dump","a")) != NULL)
          {
             list_moduls(DumpFile);    /* Aufrufhierarchie (breakpoint.c) */
             printsteps(DumpFile);          /* Anzahl Stackschritte */
             fprintf(DumpFile,"\n\n\n");
             dr(DumpFile);                  /* Stacks drucken */
             fprintf(DumpFile,"\n\n\n");
             HeapDump("Dump");              /* Heap drucken (heap.c) */
             fclose(DumpFile);
          }
          else WRONGINPUT(dump.dump kann nicht geoeffnet werden);
       }
       else
       {
          if ((DumpFile = fopen("dump.dump","w")) != NULL)
          {
             list_moduls(DumpFile);     /* Aufrufhierarchie (breakpoint.c) */
             printsteps(DumpFile);          /* Anzahl Stackschritte */
             fprintf(DumpFile,"\n\n\n");
             dr(DumpFile);                  /* Stacks drucken */
             fprintf(DumpFile,"\n\n\n");
             HeapDump("Dump");              /* Heap drucken (heap.c) */
             fclose(DumpFile);
             flagdump = TRUE;               /* jetzt ist dump:dump benutzt */
          }
          else WRONGINPUT(dump.dump kann nicht geoeffnet werden);
       }

/*************************************************************************/
/*   define :   setzen von globalen Variablen                            */
/*************************************************************************/
    else if  (isstring("define",2))
    {
       if (ispstring("redcnt",3,"<var>"))            /* Reduktionszaehler */
          if (isint(&i))
             _redcnt = i;
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("digit_recycling",3,"<var>"))
          if (isint(&i))                             /* duerfen digit-Deskriptoren */
             _digit_recycling = i;                   /* recycelt werden? */
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("formated",3,"<var>"))     /* welches Zahlensystem? */
          if (isint(&i))
             _formated = i;
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("trunc",3,"<var>"))        /* wieviel Nachkommastellen */
          if (isint(&i))                             /* darstellen */
             _trunc = i;
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("prec",3,"<var>"))         /* Wie genau dividieren? */
          if (isint(&i))
             _prec = i;
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("li",2,"<var>"))
          if (isint(&i))
             li = i;
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("co",2,"<var>"))
          if (isint(&i))
          {
             co = i;
             stackanzahl = (stacknr[0] > co/16) ? co/16 : stacknr[0];
             stackanz = (stacknr[0] > co/16) ? co/16 : stacknr[0];
          }
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("stopheap",3,"<var>"))     /* Nach wieviel Anforder- */
          if (isint(&i))                             /* ungen vollen Heap  */
             stopheap = i;                           /* simulieren? */
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("maxnum",3,"<var>"))       /* siehe base */
          if (isint(&i))
             _maxnum = i;
          else
             WRONGINPUT(int erwartet);
       else if (ispstring("base",3,"<var>"))         /* Basis des Zahlensystems */
          if (isint(&i))
          {
             _base = i;
             _maxnum = _base - 1;
             WRONGINPUT(neues Zahlensystem ordnungsgemaess definiert);
          }
          else
             WRONGINPUT(int erwartet);
       else
       {
          WRONGINPUT(unbekannte Variable);
          getzeile("<cmd>");
       }
     }

/*************************************************************************/
/*    globals :    globale Variablen anzeigen                            */
/*************************************************************************/
    else if (isstring("globals",2) || isstring("redparms",4) )
    {
       setmonitor();
       printf("_redcnt:   %d\n",_redcnt);
       printf("_prec:     %d\n",_prec);
       printf("_trunc:    %d\n",_trunc);
       printf("_indent:   %d\n",_indent);
       printf("_formated: %s\n",(_formated ? "Fix" : "Var"));
       printf("_bindings:        %d\n",_bindings);
       printf("_bound:           %d\n",_bound);
       printf("_argsneeded:      %d\n",_argsneeded);
       printf("_digit_recycling: %s\n",(_digit_recycling ? "Yes" : "No"));
       printf("_maxnum:          %d\n",_maxnum);
       printf("_base:            %d\n",_base);
       printf("Interruptbehandlung: \n");
       printf("_interrupt        %s\n",(_interrupt ? "Yes" : "No"));
       printf("_heap_reserved    %s\n",(_heap_reserved ? "Yes" : "No"));
       printf("_desc:            %x\n",_desc);
       printf("li:               %d\n",li);
       printf("co:               %d\n",co);
    }

/**********************************************************************/
/*    monitor :     Monitormodus setzen                               */
/**********************************************************************/
    else if  (isstring("monitor",3))
       setmonitor();               /* Monitormode */

/**********************************************************************/
/*   reduce: Rekursiver Aufruf der Reduktionseiheit                   */
/**********************************************************************/
     else if  (isstring("reduce",3))
     {
        int redcnt;

        if (!isint(&redcnt))
        {
          WRONGINPUT(Reduktionszaehlerstand erwartet);
        }
        else if (redcnt < 0)
        {
          WRONGINPUT(Positiver Reduktionszaehler erwartet);
        }
        else
          call_ear(redcnt);
     }
/**********************************************************************/
/*   exit :    beenden der Reduktionsmaschine (zurueck zum Editor)    */
/**********************************************************************/
    else if  (isstring("exit",2))
    {
       post_mortem2("exit order given");
    }

/************************************************************************/
/*   help  || ? :     alle Befehle auflisten                            */
/************************************************************************/
    else if  (isstring("help",1) || isstring("?",1))
    {
       setmonitor();      /* Monitor_mode */
       clearscreen();     /* Bildschirm */
       cursorxy(1,1);     /* Cursor positionieren (terminal:h) */
       printf("implementierte Befehle:\n  (<Befehl>     <max. Abkuerzung>)\n");
       printf("  return                 (3)  Kommandoeingabe beenden\n");
       printf("  continue               (2)  Im Monitor-Mode weitermachen\n");
       printf("  break <name>           (2)  Breakpoint setzen\n");
       printf("  break red <int>        (2)  Breakpoint setzen auf redcnt\n");
       printf("  listbreakpoints        (3)  gesetzte Breakpoints zeigen\n");
       printf("  module                 (3)  Aufrufhierarchie listen\n");
       printf("  delete <name>          (3)  Breakpoint loeschen\n");
       printf("  display                (2)  Display-Modus setzen\n");
       printf("  full <stack>           (2)  <stack> im Fullmodus zeigen \n");
       printf("  run <n>                (1)  <n> Stackoperationen durchfuehren\n");
       printf("  am                     (2)  pattern-match single step modus  \n");
       printf("  s                      (1)  single step modus\n");
       printf("  pop <stack>            (2)  Popoperation auf Stack <stack>\n");
       printf("  bottom <stack>         (2)  zeigt unteres Ende von <stack>\n");
       printf("  top <stack>            (2)  zeigt oberes Ende von <stack>\n");
       printf("  down <stack> <n>       (2)  <stack> um <n> nach unten schieben\n");
       printf("  up   <stack> <n>       (2)  <stack> um <n> nach oben schieben\n");
       printf("  slow <n>               (2)  Slow-Faktor <n> setzen\n");
       printf("  monitor                (3)  Monitor-Modus einschalten\n");
       printf("  trav <stack1> <stack2> (3)  Traversieren von <stack1> nach <stack2>\n");
       printf("  exit                   (2)  Programm beenden\n");
       printf("\nBitte return eingeben zum Fortsetzen:");
       c = getchar();
       clearscreen();
       printf("  which                  (2)  neue Reihenfolge der Stacks definieren\n");
       printf("  cat                    (2)  listet eine Datei auf dem Bildschirm\n");
       printf("  desc <ptr> <n>         (2)  listet Deskriptor <ptr> mit Tiefe <n>\n");
       printf("  stdesc <stack> <n>     (2)  listet Top-Deskriptor von <stack> mit Tiefe <n>\n");
       printf("  pdesc <ptr> <n> <datei> (2) listet Deskriptor <ptr> mit Tiefe <n> auf <datei>\n");
       printf("  pte <ptr>              (2)  zeigt einen Stackexpression im heap\n");
       printf("  refcount               (3)  ersetzt alle Pointer durch ihren Refcount\n");
       printf("  compact                (2)  Der Heap wird kompaktifiziert\n");
       printf("  test                   (2)  Test, ob die Heapchains in Ordnung sind\n");
       printf("  isdesc <hex>           (3)  prueft, ob <hex> Pointer auf Descriptor ist\n");
       printf("  isheap <hex>           (3)  prueft, ob <hex> ein Pointer in den Heap ist\n");
       printf("  heap                   (2)  erzeugt einen Dump des heap's auf heap.dump\n");
       printf("  stack                  (3)  erzeugt Dump der Stack's auf stack.dump\n");
       printf("  dump                   (2)  erzeugt einen allgemeinen Dump auf dump.dump\n");
       printf("  set <debvar>           (2)  setzt Debug-Variable\n");
       printf("             zur Zeit implementiert: debconf, debcnt\n");
       printf("                                     debdesc <desc>\n");
       printf("                                     outstep    (pattern-match)\n");
       printf("                                     outbefehl  (pattern-match)\n");
       printf("  reset <debvar>         (2)  setzt Debug-Variable zurueck (siehe oben)\n");
       printf("  reset all              (2)  setzt alle Debug-Variable zurueck\n");
       printf("  messwerte              (3)  druckt aktuelle Anzahl der Stackop's aus\n");
       printf("  prmesswerte            (3)  druckt obiges in die Datei stack.dump\n");
       printf("  protokoll              (3)  druckt Ergebnis Messystem in protokoll:mess\n");
       printf("\nBitte return eingeben zum Fortsetzen:");
       scanf("%c",&c);
/*     c = getchar();  */
       clearscreen();
       printf("  globals                (2)  zeigt globale Variable\n");
       printf("  redparms               (4)  zeigt globale Variable\n");
       printf("  define <var> <int>     (3)  setzt globale Variable\n");
       printf("       zur Zeit implementiert: redcnt, digit_recycling, base, maxnum\n");
       printf("                               prec, trunc, \n");
       printf("                               formated (0 = Digitstring, 1 = int bzw double\n");
       printf("                               Es sind Yes als 1  und No als 0 einzugeben\n");
       printf("  define stopheap <int>  (3)  simuliert vollen Heap nach <int> Anforderungen\n");
       printf("                                 Zuruecksetzen mit negativer Zahl\n");
       printf("  countdesc              (3)  gibt die Zahl der benutzten Deskriptoren an\n");
       printf("  help                   (1)  listet alle Kommandos\n");
       printf("  reduce  <int>          (3)  Aufruf einer neuen Reduktionseinheit\n");
       printf("  ?                      (1)  listet alle Kommandos\n");
    }
    else
    {
       WRONGINPUT(die Eingabe war leider nicht zu dechiffrieren. Bitte nochmal!);
       getzeile("<cmd>");
    }
 }
#endif /* DEBUG */
