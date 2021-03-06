/* $Log: pmam.c,v $
 * Revision 1.3  1994/11/14  09:59:49  mah
 * *** empty log message ***
 * */
/*----------------------------------------------------------------*/
/*  include files                                                 */
/*                                                                */
/*----------------------------------------------------------------*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rinstr.h"
#include "case.h"



/*---------------------------------------------------------------*/
/* Typdefinitionen                                               */
/*---------------------------------------------------------------*/


typedef int INSTRUCTION;         /*  siehe pmcomp:c    */

/*-----------------------------------------------------------------*/
/* Variablen                                                       */
/*-----------------------------------------------------------------*/


INSTRUCTION * pm_pc;                      /* abzuarbeitenden Befehl */
/*STACKELEM pm_mv[MATCH_STACK_SIZE];      ueberfluessig          */
PTR_HEAPELEM pm_base;                  /*ptdv des bearbeiteten   */
                                          /*Listendeskriptors       */
int     pm_offset;                        /* offset, zeigt auf das gerade
                                             bearbeitete Listenelement */
int     pm_match_fail = TRUE;             /* zeigt an ob Matching
                                             fehlschlaegt und Backtracking
                                             notwendig ist */
int    pm_undecided  = FALSE;          /* ist auf TRUE gesetzt, falls*/
                                          /* das Matching eine Konstante*/
                                          /* oder einen Listenkonstruktor*/
                                          /* mit einer Variablen oder    */
                                          /* einer Applikation vergleicht*/
int     bt[SYSTEM_STACK_SIZE];            /* Backtrackingstack, speicher
                                             Informationen, die nach dem
                                             backtracking wieder
                                              aktualisiert werden muessen */
int     s[SYSTEM_STACK_SIZE];             /* Systenstack, speichert pm_base
                                             pm_offset bei verschachtelt en
                                             pattern  */
int     sl[SYSTEM_STACK_SIZE];            /* Slicestack, speichert die
                                              Informatonen zur Generierung
                                              der Listendeskriptoren fuer
                                              die Slices */
int     num_bt;                           /* Zaehler fuer bt-stack  */
int     num_s;                            /* Zahler fuer Systemstack */
int     num_sl;                           /* Zahler fuer Slicestack */
int     pm_nlv;                           /* Zahl der geladenen Varibalen
                                             instantierungen */
/*nichts*/
STACKELEM pm_arg;                         /* register, um Argument aufzu
                                             nehmen. */

/*-----------------------------------------------------------------*/
/* Makros                                                          */
/*-----------------------------------------------------------------*/

#define BT_SIZE             8


#define UNDECIDED_TEST(x) \
switch (R_DESC ((*(PTR_DESCRIPTOR) x), type))    \
{       \
    case TY_CLOS: \
    case TY_VAR :                              \
                  pm_undecided = TRUE;         \
                   break;                       \
    case TY_EXPR:                              \
                   if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)x),pte)[1])) \
                             {                                      \
                           pm_undecided = TRUE;                   \
                             }                                      \
                             else                                   \
                             {                                      \
                             pm_match_fail = FALSE;                 \
                             }                                      \
                  break;                                          \
    default:                                                      \
                  pm_match_fail = FALSE;                           \
}



#define L_UNDECIDED_TEST(x) \
switch (R_DESC ((*(PTR_DESCRIPTOR) x), type))    \
{                                              \
    case TY_VAR :                              \
                  pm_undecided = TRUE;         \
                                                \
                  /* pm_pc auf den naechsten Befehl setzen. Die */ \
                  /* nachfolgenden Befehle fuer das Matching der*/  \
                  /* Liste ueberspringen. An der Stelle TESTN +2*/   \
                  /* steht der Offset dazu                      */   \
                  pm_pc = pm_pc + (* (pm_pc + 2));                    \
                   break;                       \
    case TY_EXPR:                              \
                   if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)x),pte)[1])) \
                             {                                      \
                           pm_undecided = TRUE;                   \
                                                                             \
                          /* pm_pc auf den naechsten Befehl setzen. Die */    \
                          /* nachfolgenden Befehle fuer das Matching der*/    \
                          /* Liste ueberspringen. An der Stelle TESTN +2*/    \
                          /* steht der Offset dazu                      */    \
                           pm_pc = pm_pc + (* (pm_pc + 2));                   \
                             }                                      \
                             else                                   \
                             {                                      \
                                pm_match_fail = FALSE;              \
                                pm_pc +=2;                                \
                             }                                      \
                  break;                                          \
    default:                                                      \
                  pm_match_fail = FALSE;                           \
                  pm_pc +=2;                                       \
}
/*-----------------------------------------------------------------*/
/* externe Funktionen  und Variablen                               */
/*-----------------------------------------------------------------*/
/*nichts*/
extern  void red_ldim ();        /*   aus aus h.src/query */
extern  int pmdimli ();                   /*   aus pmdimli.c */
extern  PTR_DESCRIPTOR _desc;             /* damit fuer list N X die
                                            Routine red_ldim benutzt
                                              werden kann*/
extern  STACKELEM st_dec ();              /* h.src/st-dec.c   */
extern  void convi ();                    /* h.src/conv.c  */
extern  STACKELEM gen_id();       /* legt slice an */
extern  PTR_DESCRIPTOR newdesc();
extern  BOOLEAN pm_element();     /* rlogfunc.c */

/* Vergleichsroutinen fuer Konstante */
extern  BOOLEAN pm_bool_eq();
extern  BOOLEAN str_eq();         /* rlogfunc.c */
extern  BOOLEAN pm_scal_eq ();
extern  BOOLEAN pm_digit_eq ();
extern  BOOLEAN pm_red_f_eq ();

extern void DescDump();
extern void test_inc_refcnt();
extern void rel_heap();
extern void test_dec_refcnt();
extern void ppopstack();
extern void stack_error();

#if DEBUG
/* Debugging Variable  und Funktionen */
extern  FILE * pmdmpfile;
extern int  pm_debug ();
extern  void pm_test_monitor ();
extern int  outstep;
#endif
extern StackDesc *ps_w, *ps_a, *ps_r;

#define PUSH_W(x)  PUSHSTACK(*ps_w,x)
#define PUSH_PM(x) PUSHSTACK(*ps_a,x)
#define TOP_PM()   READSTACK(*ps_a)
#define POP_PM()   POPSTACK(*ps_a)
#define PPOP_PM()  PPOPSTACK(*ps_a)
#define PUSH_R(x)  PUSHSTACK(*ps_r,x)

#if !WITHTILDE
#define POP_PM_BLOCK(var)  for (;;) \
                            if (T_POINTER(var = POP_PM()))     \
                                DEC_REFCNT((PTR_DESCRIPTOR) var); \
                            else     \
                                if ((var) != KLAA) ; else break 
#define PUSH_PM_BLOCK(size)  if ((((*ps_a).TopofStack) + size) >= (*ps_a).MarTopPtr) \
                              post_mortem("Inkarnationstackoverflow");    \
                            else PUSHSTACK(*ps_a,KLAA)
#else
#define POP_PM_BLOCK     for ( ; pm_nlv > 0 ; pm_nlv--) \
                           if (T_POINTER(TOP_PM())) { \
                             DEC_REFCNT((PTR_DESCRIPTOR)TOP_PM()); \
                             PPOP_PM(); \
                           } \
                           else PPOP_PM()
#endif /* !WITHTILDE */

/*------------------------------------------------------------------*/
/* pm_backt restauriert Register von pm_am, falls das matching      */
/* failt , incr den offset mit dem das Matching zuvor an dieser     */
/* Stelle begonnen wurde  und berichtigt Variablenbindungen auf     */
/* dem pm-stack                                                     */
/*------------------------------------------------------------------*/
void pm_backt ()
/*$ END $*/
{
    int     i;                            /* Zaehlvariable */
    STACKELEM hilf;

    pm_base = (PTR_HEAPELEM) bt[num_bt];    /* base setzen */

 /* an der naechsten Stelle in der Argumentliste weiter versuchen. Dazu
    incr des offsets */
    pm_offset = bt[num_bt + 1] + 1;

    pm_pc = (INSTRUCTION *)bt[num_bt + 2];  /* PC setzen */

 /* Variablenbindungen rueckgaengig machen */
    for (i = 1; (i <= pm_nlv - bt[num_bt + 3]); i++)
                 {
                    hilf=POP_PM();
                   if (T_POINTER (hilf ))
                     DEC_REFCNT ((PTR_DESCRIPTOR) hilf );
                }

    pm_nlv = bt[num_bt + 3];       /* Zahl der bisher auf dem pm-stack
                                      liegenden variablen berichtigen */

 /* Variablenbindungen  fuer die Slices rueckgaengig machen */
    num_sl = bt[num_bt + 6];

 /*Systemstackzaehler berichtigen*/
    num_s = bt[num_bt + 7];

}
/****************************************************************/
/*                                                              */
/*Interpreter: Der Parameter code ist ein Vektor, in dem ein    */
/*             pattern bearbeitet durch pm_comp abgelegt ist.   */
/*             Im Parameter arg steht das zu matchende Argument */
/*             Das Argument wird mit Hilfe der Register pm_base */
/*             und pm_offset im Heap durchlaufen, falls es ein  */
/*             Listendeskriptor ist.                            */
/*             pm_am liefert den Wert PM_PASST, falls das       */
/*             Matching erfolgreich war; PM_PASST_NICHT, falls  */
/*             Matching des Pattern gegen das Argument nict     */
/*             moeglich war; und PM_UNDEC, falls eine Konstante */
/*             oder ein Listenkonstruktor gegen eine Variable   */
/*             oder einen Ap Konstruktor gematcht wurde.        */
/****************************************************************/





int pm_am (code, arg)

INSTRUCTION code[];                    /*Vektor in dem das analysierte*/
                                       /*Pattern steht                */
int     arg;                              /* das zu matchende Argument */

/*---------------------------------------------------------------*/
/* lokale Variablen                                              */
/*---------------------------------------------------------------*/

{
    int     i/*,arity*/;
#if !WITHTILDE
    STACKELEM var;                 /* fuer das POP_PM_BLOCK Makro */
#endif /* !WITHTILDE */
    int     pm_arity;
    int      pm_var_nr;             /*Anzahl der patternvariablen    */

/* INITIALISIERUNGEN ... */
    pm_undecided = FALSE;
    pm_match_fail = TRUE;
    num_s = 0;
    num_sl = 0;
    pm_arg = arg;
    num_bt = 0;                    /* erster Eintrag an Stelle 0 */
    pm_nlv = 0;                    /* Zahl der geladenen Varibalen ist zu
                                      Anfang gleich 0 */
    bt[num_bt + 4] = 0;         /*wird bei RETURN abgefragt */

/*------------------------------------------------------------ */
/*  Interpreter - Rumpf                                        */
/*                                                             */
/*-------------------------------------------------------------*/
               



    START_MODUL ("pm_am");
    pm_pc = code + OFFSETS;              /* Initialisierung des pc            */

    pm_offset = 0;                 /* Offset zeigt auf  Listeneintraege im
                                      ptdv  */
    /*Anzahl der Patternvariablen im Pattern*/
    pm_var_nr = (code + (int)code[NAMELIST])[0];

#if !WITHTILDE       
    /*Platz auf dem I-Stack + Klammeraffe*/
    PUSH_PM_BLOCK(pm_var_nr);
#endif /* !WITHTILDE */

/* I n t e r p r e t e r s c h l e i f e                       */
    while ((*pm_pc) != EOT)
    {
        if (pm_match_fail == FALSE)
    /* Backtracking falls pm_match false ist und auf dem bt-stack noch
       eine Ruecksprungaddresse steht. */
        {
            /* Falls Backtrackingstackelement noch nicht ganz*/
            /*ausgefuellt ist, weil kein SKIPSKIP erreicht   */
            /*wurde, muss bt-Element wieder abgebaut werden  */
            while ( ((bt[num_bt + 2]) == 0)
                    && (num_bt > 0) )
            {
                  num_bt -= BT_SIZE;  
            }

            /*backtracking falls num_bt noch Stackframe*/
            /*enthaelt                                 */
            if (num_bt > 0)
            {
                  pm_backt();
             }
             else
             {
                 break; /*Verlassen der Whileschleife*/
             }
          }
#if DEBUG
        if (outstep != 0)
            pm_debug ();
#endif

        switch (*pm_pc)
        {
        /*-----------L_LOAD_A----------------------------*/
        /* retten des offset  und des Listendescr  in    */
        /* den slice stack                               */
        /*-----------------------------------------------*/
            case L_LOAD_A:     
                 num_sl += 3;               /*  Slicestackframe anlegen */

        /* in bt[num_bt+5] steht der in TESTV gerettete
            Listendeskriptor */
                 if (num_s < 2)             /* noch keine Unterliste */
                     sl[num_sl] = arg;
                 else
                     /*---- Descriptor-Zeiger aus Systemstack holen: ----*/
                     sl[num_sl] = (int) *(((PTR_HEAPELEM)s[num_s-1]) + s[num_s-2]);

                 sl[num_sl + 1] = pm_offset;

                 /*nichts*/
                 pm_pc++;
                 break;

        /*-----------L_LOAD_B---------------------------*/
        /* Aufruf der Funktion pm_st_slice zur          */
        /* Erzeugung neuer  Listenkonstruktoren         */
        /* fuer einen Pattern-Ausdruck X AS ...         */
        /*----------------------------------------------*/
            case L_LOAD_B:

        /* offset, an dem des Backtracking erneut beginnt laden .
           Paramter last index fuer pm_gen_ind */
                 sl[num_sl + 2] = pm_offset - 1;

                 ++pm_pc;
                 break;

        /*-----------EOT-----------------------------*/
        /* Ende des Code erreicht                    */
        /*-------------------------------------------*/
            case EOT:
                 break;


        /*-----------PM_CUTT------ ------------------*/
        /* Backtrakingstackframe abbauen             */
        /*-------------------------------------------*/
            case PM_CUTT:
                     num_bt -= BT_SIZE;
                     pm_pc++;
                     break;

        /*-----------GEN_BLOCK-----------------------*/
        /* - Anlegen eines neuen bt-Stackframes      */
        /* - Ausfuellen des Stackframes analog zu    */
        /*   TESTV                                   */
        /*-------------------------------------------*/
            case GEN_BLOCK:

                 /* Anlegen eines neuen Stackframe*/
                 num_bt +=BT_SIZE;

                 /*Kopieren der Informationen, die TESTV*/
                 /*in den Stackframe geschrieben hat    */
                 bt[num_bt] = bt[num_bt - BT_SIZE];
                 bt[num_bt + 4] = bt[num_bt + 4 - BT_SIZE];
                 bt[num_bt + 5] = bt[num_bt  + 5 - BT_SIZE];
                 bt[num_bt + 2] = bt[num_bt  + 2 - BT_SIZE];
#if 0 /* ---> SBS <---*/
                 bt[num_bt + 7] = bt[num_bt  + 7 - BT_SIZE];
#else
				 bt[num_bt+7] = num_s;
#endif

                pm_pc++;
                break;


        /*-----------PM_DOLL-------------------------*/
        /* - Ueberpruefung ob die Zahl der restlichen*/
        /*   Listenelemente noch fuer das Matching   */
        /*   ausreicht.                              */
        /*   ja: Abarbeitung der weitern Befehle     */
        /*   nein: Abbau des bt-stackframes und      */
        /*         Aufruf der backtracking Funktion  */
        /* - Ausfuellen des Backtrackingstackelements*/
        /*-------------------------------------------*/

            case PM_DOLL:

                 if (pm_offset <= (bt[num_bt + 4] - *(++pm_pc)))
                 {
                     bt[num_bt + 1] = pm_offset;
                     bt[num_bt + 2] = (int)(pm_pc - 1);
            /* Rueckspungadresse ist DOLLAR */
                     bt[num_bt + 3] = pm_nlv;
                     bt[num_bt + 6] = num_sl;
                     bt[num_bt + 7] = num_s;


    /* pm_match_fail auf True,weil sonst .  nach L_LOAD wieder
       gebacktrackt wird */
                 pm_match_fail = TRUE;
                 }
                 else
                 {
                 /* Abbau des bt-stackframes, weil matching nicht mehr
                    moeglich ist */
                           num_bt -= BT_SIZE;

                     pm_match_fail = FALSE;

                 }
                 ++pm_pc;
                 break;

        /*-----------SKIPSKIP------------------------*/
        /* - Ueberpruefung ob die Zahl der restlichen */
        /*   Listenelemente noch fuer das Matching   */
        /*   ausreicht.                              */
        /*   ja: Abarbeitung der weitern Befehle     */
        /*   nein: Abbau des bt-stackframes und      */
        /*         Aufruf der backtracking Funktion  */
        /* - Ausfuellen des Backtrackingstackelements */
        /*-------------------------------------------*/

            case SKIPSKIP:

                 if (pm_offset <= (bt[num_bt + 4] - *(++pm_pc)))
                 {
                     bt[num_bt + 1] = pm_offset;
                     bt[num_bt + 2] = (int)(pm_pc - 1);
            /* Rueckspungadresse ist SKIPSKIP */
                     bt[num_bt + 3] = pm_nlv;
                     bt[num_bt + 6] = num_sl;
                     bt[num_bt + 7] = num_s;

    /* pm_match_fail auf True,weil sonst  nach L_LOAD wieder
               gebacktrackt wird */
                pm_match_fail = TRUE;
                 }
                 else
                 {
                 /* Abbau des bt-stackframes, weil matching nicht mehr
                    moeglich ist */
                       num_bt -= BT_SIZE;

                     pm_match_fail = FALSE;

                 }
                 ++pm_pc;
                 break;

        /*-----------IN_SKIPSKIP----------------------*/
        /* - Ueberpruefung ob die Zahl der restlichen */
        /*   Listenelemente noch fuer das Matching    */
        /*   ausreicht.                               */
        /*   ja: Belegung des neuen Backtracking-     */
        /*       Stackframes, u.a. mit Ruecksprungadr.*/
        /*       auf IN_TEST-Befehl; Abarbeitung der  */
        /*       weiteren Befehle;                    */
        /*   nein: Abbau des bt-stackframes           */
        /*--------------------------------------------*/

            case IN_SKIPSKIP:

                 if (pm_offset <= (bt[num_bt + 4] - *(++pm_pc)))
                 {
                     bt[num_bt + 1] = pm_offset;
                     bt[num_bt + 2] = (int)(pm_pc - 3);
            /* Ruecksprungadresse ist IN_TEST */
                     bt[num_bt + 3] = pm_nlv;
                     bt[num_bt + 6] = num_sl;

    /* pm_match_fail auf True,weil sonst  nach L_LOAD wieder
               gebacktrackt wird */
                pm_match_fail = TRUE;
                 }
                 else
                 {
                     if (num_bt > 0)
                     {
                       num_bt -= BT_SIZE;
                 /* Abbau des bt-stackframes, weil matching nicht mehr
                    moeglich ist */
/*                        pm_backt ();  */
                     }
                     pm_match_fail = FALSE;

                 }
                 ++pm_pc;
                 break;



        /*-----------LA_IN_SKIPSKIP-----------------------*/
        /* die letzten ... in einer Liste                 */
        /* - Abbbau des aktuellen bt-stackframes          */
        /* - Ist die Anzahl der Stellen in der Argument   */
        /*   liste ausreichend um die folgenden Listen-   */
        /*   elemente zu matchen ?                        */
        /*   Die Mindeststelligkeit folgt auf den Befehl  */
        /*   LA_IN_SKIPSKIP                               */
        /*   pm_match_fail = FALSE, falls Vergleich fehl- */
        /*   schlaegt oder aber die zu ueberspringenden   */
        /*   Elemente nicht in dem String vorhanden sind, */
        /*   dessen Ptr. direkt vor LA_IN_SKIPSKIP liegt. */
        /* - setzen des pm_offset auf das Elemet im ptdv  */
        /*   des listendeskriptors bei dem das Matching   */
        /*   fortgesetzt werden muss                      */
        /*------------------------------------------------*/

            case LA_IN_SKIPSKIP:
        /* falls Backtracking notwendig zurueck zu der Adresse, die vor
           dem Einstieg in die Unterliste gueltig war. Falls last_skip
           gesetzt ist, wird durch die Funktion Backtrack fuer einen Abbau
           des Backtracking Blocks gesorgt. */
             {
                 PTR_DESCRIPTOR string_ptr;
                 PTR_HEAPELEM arg_ptr,
                              start;

                 if (pm_offset <= (bt[num_bt + 4] - *(++pm_pc)))
            /* Setzen des offset auf die Stelle an der Matching
               fortgesetzt werden soll : Stelligkeit der Liste - Zahl der
               mindestens zu matchenden Listenelemente */
                 {
                     string_ptr = (PTR_DESCRIPTOR) *(pm_pc - 2); /* sollte dort
                                                                    liegen!   */
                     arg_ptr = pm_base + pm_offset;
                     pm_offset = bt[num_bt + 4] - *(pm_pc);
                     start = pm_base + pm_offset;

              /*---- alle Elemente vor 'start' auf Inklusion pruefen ----*/
                     for ( ; arg_ptr < start; arg_ptr++)
                         if ( !pm_element(*arg_ptr,string_ptr) )
                         {
                             pm_match_fail = FALSE;
                             break;
                         }
                     num_bt -= BT_SIZE;
                 }
                 else
                 {
                     num_bt -= BT_SIZE;
                     pm_match_fail = FALSE;
                 }
                 ++pm_pc;
                 break;
             }


        /*-----------IN_PM_DOLL----------------------*/
        /* - Ueberpruefung ob die Zahl der restlichen*/
        /*   Listenelemente noch fuer das Matching   */
        /*   ausreicht.                              */
        /*   ja: Ueberschreiben des Backtracking-    */
        /*       Stackframes, u.a. mit Ruecksprung   */
        /*       auf IN_TEST-Befehl; Abarbeitung de  */
        /*       weiteren Befehle;                   */
        /*   nein: Abbau des bt-stackframes und      */
        /*         Aufruf der backtracking Funktion  */
        /* - Ausfuellen des Backtrackingstackelements*/
        /*-------------------------------------------*/

            case IN_PM_DOLL:



                 if (pm_offset <= (bt[num_bt + 4] - *(++pm_pc)))
                 {
                     bt[num_bt + 1] = pm_offset;
                     bt[num_bt + 2] = (int)(pm_pc - 3);
              /*---- Rueckspungadresse ist IN_TEST ----*/
                     bt[num_bt + 3] = pm_nlv;
                     bt[num_bt + 6] = num_sl;

    /* pm_match_fail auf True,weil sonst .  nach L_LOAD wieder
       gebacktrackt wird */
                 pm_match_fail = TRUE;
                 }
                 else
                 {
                     if (num_bt > 0)
                     {
                           num_bt -= BT_SIZE;
                 /* Abbau des bt-stackframes, weil matching nicht mehr
                    moeglich ist */
/*                        pm_backt ();    */
                     }
                     pm_match_fail = FALSE;

                 }
                 ++pm_pc;
                 break;



        /*---------DEL_BLOCKS------------------------*/
        /* entfernt so viele bt-stackframes wie im   */
        /* nachfolgenden Parameter angegeben sind.   */
        /*-------------------------------------------*/

            case DEL_BLOCKS:

                 num_bt -= (BT_SIZE * *(++pm_pc));
                 ++pm_pc;
                 break;


        /*---------A_LI_CHECK------------------------*/
        /* prueft nach, ob sich an aktueller Argu-   */
        /* mentpos. ein Zeiger auf eine Liste befind.*/
        /*-------------------------------------------*/

            case A_LI_CHECK:


                 if ((T_POINTER(pm_arg)) &&
                     (R_DESC((*(PTR_DESCRIPTOR)pm_arg),type) == TY_STRING))
                     pm_match_fail = FALSE;
                 else 
                     ++pm_pc;
                 break;

        /*-----------LI_CHECK------------------------*/
        /* prueft nach, ob sich an aktueller Argu-   */
        /* mentpos. ein Zeiger auf eine Liste befind.*/
        /*-------------------------------------------*/

            case LI_CHECK:


                 if ((T_POINTER(*(pm_base + pm_offset))) && 
                     (R_DESC((*(PTR_DESCRIPTOR)*(pm_base + pm_offset)),type) == TY_STRING))
                     pm_match_fail = FALSE;
                 else 
                     ++pm_pc;
                 break;

        /*---------A_STR_CHECK------------------------*/
        /* prueft nach, ob sich an aktueller Argu-   */
        /* mentpos. ein Zeiger auf eine Liste befind.*/
        /*-------------------------------------------*/

            case A_STR_CHECK:

                 if ((T_POINTER(pm_arg)) &&
                     (R_DESC((*(PTR_DESCRIPTOR)pm_arg),type) == TY_UNDEF ))
                     pm_match_fail = FALSE;
                 else 
                     ++pm_pc;
                 break;

        /*-----------STR_CHECK------------------------*/
        /* prueft nach, ob sich an aktueller Argu-   */
        /* mentpos. ein Zeiger auf eine Liste befind.*/
        /*-------------------------------------------*/

            case STR_CHECK:

                 if ((T_POINTER(*(pm_base + pm_offset))) && 
                     (R_DESC((*(PTR_DESCRIPTOR)*(pm_base + pm_offset)),type) == TY_UNDEF ))
                     pm_match_fail = FALSE;
                 else 
                     ++pm_pc;
                 break;


        /*-----------TESTN-----------------------------*/
        /* - Steht in Argumentposition ein Pointer ?   */
        /* - hat der pointer den Typ Listendeskr. ?    */
        /* - Stelligkeit des Listendeskr gleich dem auf */
        /*   TESTN folgenden Parameter ?               */
        /* - obere Abfragen sind true:                 */
        /*     - Retten von pm_base und pm_offset in   */
        /*        den Systemstack                       */
        /*     - Setzen des neuen Wertes fuer das      */
        /*       Base Register und Initialisierung     */
        /*       des offset mit 0                      */
        /*     - pc incr. und break                    */
        /* - Steht im Argumentposition eine Variable ? */
        /*   oder Applikation ? --> pm_undecided setzen*/
        /* - pm_match_fail auf FALSE setzen, falls     */
        /*   einer der oberen Vergleiche fehlschlaegt  */
        /*---------------------------------------------*/
            case TESTN:
                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                     if ((R_DESC ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), class)) == C_LIST)

                    {
                          if (R_LIST ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), dim) == *(++pm_pc))
                          {        /* Argument steht in  *(  pm_base +offset
                                      und wird Listendeskriptor sein  die
                                      Stelligkeit, die dieser Konstr. haben
                                      soll steht im naechsten
                                      Inst.arrayeintrag  */


                     /* rettet der Register pm_base, pm_offset   in den
                        Systemstack vor                  Abstieg in das
                        matching einer Unterliste */
                              if (num_s + 2 < SYSTEM_STACK_SIZE)
                              {

                                   s[num_s++] = pm_offset;
                          /* offset fuer return            */
                                   s[num_s++] = (int)pm_base;
                          /*  base  fuer return            */

                          /*retten des Systemstack Zaehlers in den Backtracking*/
                          /*Stack. Falls nach einem Befehl TESTV ein Befehl    */
                          /*GEN_BLOCK folgt ist der Backtracking Stackframe    */
                          /*nicht vollstaendig ausgefuellt.                    */
#if 0 /* ---> SBS <--- */
                                   bt[num_bt + 7] = num_s;
#endif


                              }
                              else
                                   post_mortem ("pm_am: Systemstack zu klein \n");



                     /* setzt das register pm_base auf den ptdv des
                        Listendeskriptors, der gematcht werden soll
                        pm_offset wird mit 0 initialisiert          */
                              pm_base = R_LIST ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), ptdv);
                     /* pointer auf das Heapelement in das */
                     /* baseregister schreiben             */
                              pm_offset = 0;


                     /* Stelligkeit weiter, naechster Befehl */
                     /* offset Parameter ueberspringen       */
                              pm_pc +=2;

                              break;
                          }
                          else
                          {
                         /* Matching passt nicht pm_match_fail setzen  */
                           pm_match_fail = FALSE;
                           /*++pm_pc; */
                           break;
                          }
                       }
                       else
                       {
                       /*Prueft auf Undecided und setzt pm_pc auf den */
                       /*entsprechenden naechsten Eintrag             */
                       L_UNDECIDED_TEST(*(pm_base+ pm_offset));



                        break;
                      }
                 }
                 else
                 {
                /* Matching passt nicht pm_match_fail setzen  */
                 pm_match_fail = FALSE;
                /*pm_pc +=2; */
                 break;
                }


            case A_TESTN:
                 if (T_POINTER (pm_arg))
                 {
                   if ((R_DESC ((*(PTR_DESCRIPTOR) pm_arg), class)) == C_LIST)
                  {
                              if (R_LIST ((*(PTR_DESCRIPTOR) pm_arg), dim)
                                       == *(++pm_pc))
                              {    /* Argument steht in  pm_arg und ist Listendeskriptor */
                                   /* Die Stelligkeit folgt als erster Parameter und ein */
                                   /* offset zum Ueberspringen als zweiter Parameter     */
                                   
                                   /* Matching klappt naechster Befehl                    */

                                   pm_pc += 2;


                          /* Base und Offset Register setzen (altes
                             A_SETBASE) */
                                   pm_base = R_LIST ((*(PTR_DESCRIPTOR) pm_arg), ptdv);
                                   pm_offset = 0;



                                   break;
                              }
                              else
                              {
                            /* Matching passt nicht  */
                              pm_match_fail = FALSE;
                              /*++pm_pc; */
                              break;
                              }
                 }
                 else
                 {
                        L_UNDECIDED_TEST(pm_arg)
                        break;
                 }
             }
            else
             {
              /* Matching passt nicht  */
                 pm_match_fail = FALSE;
                /*pm_pc +=2; */
                 break;
             }





        /*-----------TESTV-----------------------------*/
        /* zum matchen von Listenkonstruktoren, in     */
        /* denen ... als Listenelemente auftauchen, d.h */
        /* Stelligkeit der zu matchenden Argumentliste */
        /* ist nicht durch das Pattern festgelegt      */
        /* Aktionen:                                   */
        /* - Steht in Argumentposition ein Pointer ?   */
        /* - hat der pointer den Typ Listendeskr. ?    */
        /* - Stelligkeit des Listendeskr groesser      */
        /*   gleich der Mindeststelligkeit, die das    */
        /*   Argument haben sollte.  Diese Mindest -   */
        /*   stelligkeit folgt als Parameter hinter    */
        /*   TESTN ?                                   */
        /*   Falls die Mindeststelligkeit null ist,    */
        /*   darf als Argument auch Nil sein .         */
        /* - Zaehler des backtracking Stacks erhoehen  */
        /* - Eintrag der Stelligkeit, des Listendescr. */
        /*   und des base in bt-stack.                 */
        /* - Retten von base und offset in den         */
        /*   Systemstack                               */
        /* - Setzen des neuen Wertes fuer das base     */
        /*   register und Initialisierung des offset   */
        /*   mit 0                                     */
        /* - Steht im Argumentposition eine Variable ? */
        /*   oder Applikation ? --> pm_undecided setzen*/
        /* - pm_match_fail auf FALSE setzen, falls     */
        /*   einer der oberen Vergleiche fehlschlaegt  */
        /*---------------------------------------------*/
            case TESTV:
                 if (T_POINTER (*(pm_base + pm_offset)))
                 {

                     if ((R_DESC ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), class)) == C_LIST)
                    {
                          if (R_LIST ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), dim) >= *(++pm_pc))
                          {        /* Argument steht in  *(  pm_base +offset
                                      und wird Listendeskriptor sein  die
                                      Stelligkeit, die dieser Konstr. haben
                                      soll steht im naechsten
                                      Inst.arrayeintrag  */
                     /* Zaehler fuer den Backtracking-stack erhoehen und
                        eintrag des ptdv, des Argumentdesckiptors und der
                        Stelligkeit */
                        num_bt +=BT_SIZE;
                              bt[num_bt] = /*pm_base merken */
        (int)(R_LIST ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), ptdv));
                              bt[num_bt + 5] = (int)*(pm_base + pm_offset);
                     /* merken des Listendesc falls fuer die slices noch
                        einmal der Listendeskriptor gebraucht wird */

                              bt[num_bt + 4] =
                     /* Stelligkeit der Liste merken */
                                   R_LIST ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), dim);
                               bt[ num_bt + 2] = 0;  /* Stelle fuer
                                                            pm_pc auf
                                                            0 setzen */
                                              


                     /* rettet der Register pm_base, pm_offset   in den
                        Systemstack vor                  Abstieg in das
                        matching einer Unterliste */
                              if (num_s + 2 < SYSTEM_STACK_SIZE)
                              {

                                   s[num_s++] = pm_offset;
                          /* offset fuer return            */
                                   s[num_s++] = (int)pm_base;
                          /*  base  fuer return            */

                          /*retten des Systemstack Zaehlers in den Backtracking*/
                          /*Stack. Falls nach einem Befehl TESTV ein Befehl    */
                          /*GEN_BLOCK folgt ist der Backtracking Stackframe    */
                          /*nicht vollstaendig ausgefuellt.                    */
#if 0 /* ---> SBS <--- */
                                   bt[num_bt + 7] = num_s;
#endif
                              }
                              else
                                   post_mortem ("pm_am: Systemstack zu klein \n");



                     /* setzt das register pm_base auf den ptdv des
                        Listendeskriptors, der gematcht werden soll
                        pm_offset wird mit 0 initialisiert          */
                              pm_base = R_LIST ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), ptdv);
                     /* pointer auf das Heapelement in das */
                     /* baseregister schreiben             */
                              pm_offset = 0;



                     /* pm_pc auf den naechsten Befehl setzen*/
                     /* offset Parameter ueberspringen       */
                         pm_pc +=2;
                              break;
                          }
                          else
                          {
                           /*Matching failt */
                          pm_match_fail = FALSE;
                          /*++pm_pc;*/
                          break;
                          }
                }
                 else
                 {
                     L_UNDECIDED_TEST(*(pm_base + pm_offset))
                     break;

                 }
              }
              else

              {

                 if ((*(++pm_pc) == 0) &&
                          (((*(pm_base + pm_offset)) & ~F_EDIT) == NIL))
            /* <...> enstspricht der leeren Liste nil */
                 {
            /* pm_pc auf den naechsten sinnvollen Befehl setzen*/
            /* Ueber offset, LA_SKIPSKIP, Parameter, RETURN    */
            /* springen                                        */
                     pm_pc += 5;   

                     break;
                 }
                 else
                {
                 /*Matching failt */
                  pm_match_fail = FALSE;
                  /*++pm_pc;*/
                  break;
                 }
                 }

            case A_TESTV:
                 if (T_POINTER (pm_arg))
                 {
                   if ((R_DESC ((*(PTR_DESCRIPTOR) pm_arg), class)) == C_LIST)
                          {
                              pm_arity = R_LIST ((*(PTR_DESCRIPTOR) pm_arg), dim);

                              if (pm_arity
                                       >= *(++pm_pc))
                              {    /* Argument steht in  pm_arg und wird
                                      Listendeskriptor sein die Stelligkeit,
                                      die dieser Konstr.haben soll steht im
                                      naechsten Inst.arrayeintrag Stelligkeit
                                      weiter, naechster Befehl   */
                          /* Zaehler fuer den Backtracking-stack erhoehen
                             und eintrag des ptdv, des Argumentdesckiptors
                             und der Stelligkeit */
                             num_bt += BT_SIZE;
                                   bt[num_bt] =    /*pm_base merken */
                                   (int)(R_LIST ((*(PTR_DESCRIPTOR) pm_arg), ptdv));
                                   bt[num_bt + 5] = (int)pm_arg;
                          /* merken des Listendesc falls fuer die slices
                             noch einmal der Listendeskriptor gebraucht wird
                             */

                                   bt[num_bt + 4] =
                          /* Stelligkeit der Liste merken */
                                       R_LIST ((*(PTR_DESCRIPTOR) pm_arg), dim);


                               bt[ num_bt + 2] = 0;  /* Stelle fuer
                                                            pm_pc auf
                                                            0 setzen */



                          /* Base und Offset Register setzen (altes
                             A_SETBASE) */
                                   pm_base = R_LIST ((*(PTR_DESCRIPTOR) pm_arg), ptdv);
                                   pm_offset = 0;

                          /* naester Befehl, offset Parameter ueberspringen*/
                                  pm_pc += 2;
                                   break;
                              }
                              else
                              {
                               pm_match_fail = FALSE;
                               /*++pm_pc;*/
                               break;
                              }
                          }
                          else
                          {
                           L_UNDECIDED_TEST(pm_arg)
                           break;
                          }
                 }
                 else
                 {

                 if ((*(++pm_pc) == 0) &&
                          ((pm_arg & ~F_EDIT) == NIL))
                 {
                  /* pm_pc auf den naechsten sinnvollen Befehl setzen*/
                  /* Ueber offset, LA_SKIPSKIP, Parameter, RETURN    */
                  /* springen                                        */
                     pm_pc += 5;   
                     break;
                 }
                 else
                 {
                   pm_match_fail = FALSE;
                   /*++pm_pc;*/
                   break;
                 }
                 }

        /*-----------LA_SKIPSKIP--------------------------*/
        /* die letzten ... in einer Liste                 */
        /* - Abbbau des aktuellen bt-stackframes          */
        /* - Ist die Anzahl der Stellen in der Argument   */
        /*   liste ausreichend um die folgenden Listen-   */
        /*   elemente zu matchen ?                        */
        /*   Die Mindeststelligkeit folgt auf den Befehl  */
        /*   LA_SKIPSKIP                                  */
        /*   pm_match_fail = FALSE, falls Vergleich fehl- */
        /*   schlaegt.                                    */
        /* - setzen des pm_offset auf das Elemet im ptdv  */
        /*   des listendeskriptors bei dem das Matching   */
        /*   fortgesetzt werden muss                      */
        /*------------------------------------------------*/

            case LA_SKIPSKIP:
        /* falls Backtracking notwendig zurueck zu der Adresse, die vor
           dem Einstieg in die Unterliste gueltig war falls last_skip
           gesetzt ist wird durch die Funktion Backtrack fuer einen Abbau
           des Backtracking blocks gesorgt. */

                 if (pm_offset <= (bt[num_bt + 4] - *(++pm_pc)))
            /* Setzen des offset auf die Stelle an der Matching
               fortgesetzt werden soll : Stelligkeit der Liste - Zahl der
               mindestens zu matchenden Listenelemente */
                 {
                     pm_offset = bt[num_bt + 4] - *(pm_pc);
                     num_bt -= BT_SIZE;

                 }
                 else
                 {
                     num_bt -= BT_SIZE;
                     pm_match_fail = FALSE;


                 }
                 ++pm_pc;
                 break;



/*******************************************************************/
/* Testbefehle fuer Konstanten  :                                  */
/*                                                                 */
        /*-----------<Praefix>_TEST--------------------*/
        /* - Steht an Argumentposition ein Pointer ?   */
        /* - Typ des Pointers an Argumentposition      */
        /*   gleich  Typ des Parameter der auf den     */
        /*   Testbefehl folgt ?                        */
        /* - Aufruf der entsprechenden Vergleichs-     */
        /*   routine mit dem Deskriptor an Argument-   */
        /*   position und Parameterdeskriptor und      */
        /*   setzen des registers pm_match_fail        */
        /* - Steht im Argumentposition eine Variable ? */
        /*   oder Applikation ? --> pm_undecided setzen*/
        /*---------------------------------------------*/
/*******************************************************************/

            case STR_TEST:

                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                  switch (R_DESC ((*(PTR_DESCRIPTOR) (*(pm_base+pm_offset))), type))
                  {
                     case TY_VAR :
                         pm_undecided = TRUE;
                           pm_pc  +=2;
                           ++pm_offset;
                          break;
                     case TY_EXPR:
                          if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)(*(pm_base+pm_offset))),pte)[1]))
                             {
                           pm_undecided = TRUE;
                             pm_pc +=2;
                             ++pm_offset;
                             }
                             else
                             {
                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             /*++pm_offset;*/
                             }
                         break;
                     case TY_STRING:
                          pm_match_fail = str_eq(*(pm_base + pm_offset), *(++pm_pc));
                          ++pm_pc;
                          ++pm_offset;
                        break;

                    default:

                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             /*++pm_offset;*/
                   }
                          break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     /*pm_pc +=2;*/
                     break;
                 }

            case DIG_TEST:

                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                  switch (R_DESC ((*(PTR_DESCRIPTOR) (*(pm_base+pm_offset))), type))
                  {
                     case TY_VAR :
                         pm_undecided = TRUE;
                           pm_pc  +=2;
                           ++pm_offset;
                          break;
                     case TY_EXPR:
                          if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)(*(pm_base+pm_offset))),pte)[1]))
                             {
                           pm_undecided = TRUE;
                             pm_pc +=2;
                             ++pm_offset;
                             }
                             else
                             {
                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             /*++pm_offset;*/
                             }
                         break;
                      case TY_DIGIT:
                          pm_match_fail = pm_digit_eq (*(pm_base + pm_offset), *(++pm_pc));
                          ++pm_pc;
                          ++pm_offset;
                        break;

                    default:

                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             /*++pm_offset;*/
                   }

                    break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     /*pm_pc +=2;*/
                     break;
                 }


            case SCAL_TEST:

                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                  switch (R_DESC ((*(PTR_DESCRIPTOR) (*(pm_base+pm_offset))), type))
                  {
                     case TY_VAR :
                         pm_undecided = TRUE;
                           pm_pc  +=2;
                           ++pm_offset;
                          break;
                     case TY_EXPR:
                          if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)(*(pm_base+pm_offset))),pte)[1]))
                             {
                           pm_undecided = TRUE;
                             pm_pc +=2;
                             ++pm_offset;
                             }
                             else
                             {
                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             /*++pm_offset;*/
                             }
                         break;
                    default:
                      pm_match_fail = pm_scal_eq (*(pm_base + pm_offset), *(++pm_pc));
                     ++pm_pc;
                      ++pm_offset;

                   }

                   break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     /*pm_pc +=2;*/
                     break;
                 }

            case MATRIX_TEST :
            case VECTOR_TEST :
            case TVECTOR_TEST:


                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                  switch (R_DESC ((*(PTR_DESCRIPTOR) (*(pm_base+pm_offset))), type))
                  {
                     case TY_VAR :
                         pm_undecided = TRUE;
                           pm_pc  +=2;
                           ++pm_offset;
                          break;
                     case TY_EXPR:
                          if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)(*(pm_base+pm_offset))),pte)[1]))
                             {
                           pm_undecided = TRUE;
                             pm_pc +=2;
                             ++pm_offset;
                             }
                             else
                             {
                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             /*++pm_offset;*/
                             }
                         break;
                    default:
                       pm_match_fail = pm_red_f_eq (*(pm_base + pm_offset), *(++pm_pc));
                       ++pm_pc;
                       ++pm_offset;

                   }

                    break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     /*pm_pc +=2;*/
                     break;
                 }

            case BOOL_TEST:
            case ATOM_TEST:
#if WITHTILDE
		 if ((*++pm_pc == *(pm_base + pm_offset) && T_INT(*pm_pc))
		     || ((*(pm_base + pm_offset) & ~F_EDIT) == (*pm_pc & ~F_EDIT)))
#else
                 if (    ((*(pm_base + pm_offset)) & ~F_EDIT)
                      == ((*(++pm_pc)) & ~F_EDIT)
                    )
#endif /* WITHTILDE */
                 {
                     ++pm_pc;
                     ++pm_offset;
                     break;
                 }
                 else
                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                     UNDECIDED_TEST(*(pm_base + pm_offset))
                     ++pm_pc;
                     ++pm_offset;
                     break;
                 }
                 else
                 {
                     pm_match_fail = FALSE ;
                     /*++pm_pc;*/
                     /*++pm_offset;*/
                     break;
                 }
            case A_BOOL_TEST:
            case A_ATOM_TEST:
#if WITHTILDE
                 if ((*++pm_pc == pm_arg && T_INT(pm_arg))
                  || ((pm_arg & ~F_EDIT) == (*pm_pc & ~F_EDIT))) 
#else
                 if ((pm_arg & ~F_EDIT) == (*(++pm_pc) & ~F_EDIT) )
#endif /* WITHTILDE */
                 {
                     pm_pc++;
                     break;
                 }
                else
                if (T_POINTER (pm_arg))
                {
                    UNDECIDED_TEST(pm_arg)
                    pm_pc++;
		    break;
                }
                else
                {
                    pm_match_fail = FALSE;
                    /* pm_pc++; */
                    break;
                }
            case NIL_TEST:                /* NIL ist Standarddeskriptor (sj) */
            case NILSTRING_TEST:          /* NILSTRING ist Standarddesk.(sj) */
                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                   switch (R_DESC ((*(PTR_DESCRIPTOR)
                                           (*(pm_base + pm_offset))), type))
                   {
                   case TY_VAR :          /* an Argumentpos. steht Variable */
                        pm_undecided = TRUE;
                        ++pm_pc;
                        ++pm_offset;
                        break;
                   case TY_EXPR:
                        if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)
                                               (*(pm_base+pm_offset))),pte)[1]))
                        /*---- an Argumentpos. steht eine Applikation ----*/
                        {
                            pm_undecided = TRUE;
                            ++pm_pc;
                            ++pm_offset;
                        }
                        else
                        {
                            pm_match_fail = FALSE;
                            /*++pm_pc;*/
                            /*++pm_offset;*/
                        }
                        break;
                   default:
                        /*---- vielleicht NIL / NILSTRING an Argumentpos. ----*/
                        pm_match_fail = ((T_PTD) *(pm_base + pm_offset) ==
                                     ((*pm_pc == NIL_TEST)? _nil : _nilstring));
                        ++pm_pc;
                        ++pm_offset;
                        break;
                   }
                   break;
                 }
                 else                     /* kein Zeiger an Argumentposition */
                 {
                     pm_match_fail = FALSE;
                     ++pm_pc;
                     break;
                 }
            case A_NIL_TEST:               /* NIL ist Standarddeskriptor (sj) */
            case A_NILSTRING_TEST:         /* NILSTRING ist Standarddesk.(sj) */
                 if (T_POINTER (pm_arg))
                 {
                   switch (R_DESC ((*(PTR_DESCRIPTOR) pm_arg), type))
                   {
                   case TY_VAR :           /* an Argumentpos. steht Variable */
                        pm_undecided = TRUE;
                        ++pm_pc;
                        break;
                   case TY_EXPR:
                        if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)pm_arg),pte)[1]))
                        /*---- an Argumentpos. steht eine Applikation ----*/
                        {
                            pm_undecided = TRUE;
                            ++pm_pc;
                        }
                        else
                        {
                            pm_match_fail = FALSE;
                            /*++pm_pc;*/
                        }
                        break;
                   default:
                        /*---- vielleicht NIL / NILSTRING an Argumentpos. ----*/
                        pm_match_fail = ( ((PTR_DESCRIPTOR) pm_arg) ==
                                   ((*pm_pc == A_NIL_TEST)? _nil : _nilstring));
                        ++pm_pc;
                   }
                   break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     ++pm_pc;
                     break;
                 }


        /*-----------TY_TEST-----------------------------*/
        /* fuer  den Type Test in Pattern der Form       */
        /* as Bool X                                     */
        /* - steht an Argumentposition eine Variable ?   */
        /* - ist der Typ des Arguments gleich dem als    */
        /*   Parameter folgenden Typ im Pattern-vektor   */
        /*-----------------------------------------------*/
            case TY_TEST:

                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                 /*Test ob eine im fixformat Zahl vorliegt, die extern*/
                 /*den Typ Decimal hat                                */

                 if(    ( *(pm_pc +1) == TY_DIGIT )
                    &&  (
                             ( (R_DESC ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), type)) == TY_REAL)
                         ||  ( (R_DESC ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), type)) == TY_INTEGER)
                         )
                    )
                 {                       
                          pm_pc+=2;
                          break;
                 }
                     if ((R_DESC ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), type)) == (*(++pm_pc)))

                     {
                          ++pm_pc;
                          break;
                     }
                     else
                     {
                         UNDECIDED_TEST(*(pm_base + pm_offset))
                          ++pm_pc;
                          break;
                     }

                 }
                 else
#if WITHTILDE
                     if ((T_INT(*(pm_base + pm_offset)) && *++pm_pc == TY_DIGIT)
                      || (T_BOOLEAN(*(pm_base + pm_offset)) && *++pm_pc == TY_BOOL))
#else		   
                     if ((T_BOOLEAN (*(pm_base + pm_offset))) && ((*(++pm_pc))
                                   == TY_BOOL))
#endif /* WITHTILDE */
                     {
                          ++pm_pc;
                          break;
                     }
                     else
                     {
                          pm_match_fail = FALSE;
                          /*++pm_pc;*/
                         break;
                     }


            case A_STR_TEST:

        if (T_POINTER (pm_arg))
                 {
                  switch (R_DESC ((*(PTR_DESCRIPTOR) pm_arg), type))
                  {
                     case TY_VAR :
                         pm_undecided = TRUE;
                          pm_pc +=2;
                          break;
                     case TY_EXPR:
                          if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)pm_arg),pte)[1]))
                             {
                           pm_undecided = TRUE;
                            pm_pc +=2;
                             }
                             else
                             {
                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             }
                         break;
                     case TY_STRING:
                        pm_match_fail = str_eq (pm_arg, *(++pm_pc));
                        ++pm_pc;
                        break;

                    default:
                        pm_match_fail = FALSE ;
                        /*pm_pc +=2;*/

                   }

                    break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     /*pm_pc +=2;*/
                     break;

                 }
            case A_DIG_TEST:

        if (T_POINTER (pm_arg))
                 {
                  switch (R_DESC ((*(PTR_DESCRIPTOR) pm_arg), type))
                  {
                     case TY_VAR :
                         pm_undecided = TRUE;
                          pm_pc +=2;
                          break;
                     case TY_EXPR:
                          if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)pm_arg),pte)[1]))
                             {
                           pm_undecided = TRUE;
                            pm_pc +=2;
                             }
                             else
                             {
                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             }
                         break;
                     case TY_DIGIT:
                        pm_match_fail = pm_digit_eq (pm_arg, *(++pm_pc));
                        ++pm_pc;
                        break;

                    default:
                        pm_match_fail = FALSE ;
                        /*pm_pc +=2;*/
                   }

                    break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     /*pm_pc +=2;*/
                     break;

                 }

            case A_SCAL_TEST:

        if (T_POINTER (pm_arg))
                 {
                  switch (R_DESC ((*(PTR_DESCRIPTOR) pm_arg), type))
                  {
                     case TY_VAR :
                         pm_undecided = TRUE;
                          pm_pc +=2;
                          break;
                     case TY_EXPR:
                          if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)pm_arg),pte)[1]))
                             {
                           pm_undecided = TRUE;
                            pm_pc +=2;
                             }
                             else
                             {
                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             }
                         break;
                    default:
                       pm_match_fail = pm_scal_eq (pm_arg, *(++pm_pc));
                       ++pm_pc;

                   }

                    break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     /*pm_pc +=2;*/
                     break;

                 }

            case A_MATRIX_TEST :
            case A_VECTOR_TEST :
            case A_TVECTOR_TEST:


        if (T_POINTER (pm_arg))
                 {
                  switch (R_DESC ((*(PTR_DESCRIPTOR) pm_arg), type))
                  {
                     case TY_VAR :
                         pm_undecided = TRUE;
                          pm_pc +=2;
                          break;
                     case TY_EXPR:
                          if (T_AP( R_EXPR((*(PTR_DESCRIPTOR)pm_arg),pte)[1]))
                             {
                           pm_undecided = TRUE;
                            pm_pc +=2;
                             }
                             else
                             {
                             pm_match_fail = FALSE;
                             /*pm_pc +=2;*/
                             }
                         break;
                    default:
                        pm_match_fail = pm_red_f_eq (pm_arg, *(++pm_pc));
                        ++pm_pc;

                   }

                    break;
                 }
                 else
                 {
                     pm_match_fail = FALSE;
                     pm_pc +=2;
                     break;

                 }

            case A_TY_TEST:
        if (T_POINTER (pm_arg))
                 {
                 /*Test ob eine im fixformat Zahl vorliegt, die extern*/
                 /*den Typ Decimal hat                                */
                 if (
                    ( *(pm_pc +1) == TY_DIGIT ) &&
                         ( ( (R_DESC ((*(PTR_DESCRIPTOR)  pm_arg), type)) == TY_REAL)
                         ||  ( (R_DESC ((*(PTR_DESCRIPTOR)  pm_arg), type)) == TY_INTEGER)
                         )
                    )
                 {                       
                          pm_pc+=2;
                          break;
                 }
                          if ((R_DESC ((*(PTR_DESCRIPTOR) pm_arg), type)) == (*(++pm_pc)))

                          {
                              ++pm_pc;
                             break;
                          }
                          else
                          {
                              ++pm_pc;
                             UNDECIDED_TEST(pm_arg)
                             break;
                          }
                     }
#if WITHTILDE
		 if ((T_INT(pm_arg) && *++pm_pc == TY_DIGIT)
		     || (T_BOOLEAN(pm_arg) && *++pm_pc == TY_BOOL))
#else
		 if ((T_BOOLEAN ((STACKELEM) pm_arg)) && ((*(++pm_pc))
                                       == TY_BOOL))
#endif /* WITHTILDE */
                          {
                              ++pm_pc;
                             break;
                          }
                          else
                          {
                              pm_match_fail = FALSE;
                              ++pm_pc;
                             break;
                          }

            case A_KTEST:



        /*    pm_match_fail =  pm_red_eq (pm_arg, *(++pm_pc)); */


                 ++pm_pc;
                 ++pm_offset;
                 break;

        /*-----------IN_TEST--------------------------*/
        /* prueft nach, ob das Objekt an der Stelle   */
        /* *(pm_base + pm_offset) in dem String ent-  */
        /* halten ist, auf den der Pointer direkt     */
        /* hinter IN_TEST zeigt.                      */
        /* ja: weitermachen (ohne pm_offset++)        */
        /* nein: Abbau des bt-stackframes             */
        /*--------------------------------------------*/
            case IN_TEST:


                 if ( pm_element( *(pm_base + pm_offset - 1),
                                  *(++pm_pc) )             /* Ptr. auf String */
                    )
                 {
                     pm_match_fail = TRUE;
                     ++pm_pc;
                     break;
                 }
                 else
                 {
                    pm_match_fail = FALSE ;
                    num_bt -= BT_SIZE;
                    /*++pm_pc;*/
                    break;
                 }



        /*-----------IN_TEST1-------------------------*/
        /* prueft nach, ob das Objekt an der Stelle   */
        /* *(pm_base + pm_offset) in dem String ent-  */
        /* halten ist, auf den der Pointer direkt     */
        /* hinter IN_TEST zeigt.                      */
        /*--------------------------------------------*/
            case IN_TEST1:


                 if ( pm_element( *(pm_base + pm_offset),
                                  *(++pm_pc) )             /* Ptr. auf String */
                    )
                 {
                     pm_match_fail = TRUE;
                     ++pm_pc;
                     ++pm_offset;
                     break;
                 }
                 else
                 {
                    pm_match_fail = FALSE ;
                    /*++pm_pc;*/
                    break;
                 }



        /*-----------LOAD-----------------------------*/
        /* legt Objekt an der Stelle *(pm_base +      */
        /* pm_offset) auf den pm-stack                */
        /* Die Zahl der Variablen  auf dem pm-stack   */
        /* wird fuer das Backtracking in der Variablen*/
        /* pm_nlv mitgezaehlt.                        */
        /*--------------------------------------------*/
            case LOAD:

                 {
/*                   PTR_DESCRIPTOR desc; */
                   T_HEAPELEM arg;

                   arg = *(pm_base + pm_offset);

                   if (T_STR_CHAR( (STACKELEM) arg))
            /*---- Character duerfen nicht an Variablen gebunden werden; ----*/
            /*----             kann der Editor nicht  !!!                ----*/
                   {
                     pm_match_fail = FALSE;
                     ++pm_pc;
                     break;
                   }
                   else
                   {
                     if (T_POINTER (arg))
                         INC_REFCNT ((PTR_DESCRIPTOR) arg);
                     PUSH_PM(arg);
                   }
                   ++pm_nlv;         /* eine Variable  mehr auf dem pm-stack */
                   pm_pc++;
                   ++pm_offset;
                   break;
                 }
        /*-----------PM_PUSH--------------------------*/
        /* legt Argument das auf den Befehl kommt auf */
        /* den pm-stack: wird gebraucht um den        */
        /* rekursiven Parameter eines caserec zu      */
        /* uebergeben                                 */
        /*--------------------------------------------*/
            case PM_PUSH:


                 /* pc auf das Argument, den rekusiven Parameter*/
                 /* setzen                                      */
                 ++pm_pc;

                 /* Argument auf den i-stack */
                 PUSH_PM(*pm_pc);

                 if (T_POINTER (TOP_PM()))
                     INC_REFCNT ((PTR_DESCRIPTOR) TOP_PM());

                 /* eine Variable  mehr auf dem i-stack */
                 ++pm_nlv;      

                 /* den rekursiven Parameter fuer das Postprocessing*/
                 /* ueberspringen                                   */
                 ++pm_pc;

                 pm_pc++;
                 break;

        /*-----------INCR-----------------------------*/
        /* erhoeht offset um eins. Wird abgesetzt,    */
        /* im Pattern ein Skip steht                  */
        /*--------------------------------------------*/
            case INCR:
                 pm_pc++;
                 ++pm_offset;
                 break;

        /*-----------JMP2-----------------------------*/
        /* ueberspringt die naechsten beiden Befehle  */
        /* im Code-Vektor; wird abgesetzt,wenn im     */
        /* Pattern ein 'in' steht;                    */
        /*--------------------------------------------*/
            case JMP2:
                 pm_pc += 3;
                 break;

        /*-----------AS_LOAD--------------------------*/
        /* legt Objekt an der Stelle *(pm_base +      */
        /* pm_offset) auf den pm-stack                */
        /* Die Zahl der Variablen  auf dem pm-stack   */
        /* wird fuer das Backtracking in der Variablen*/
        /* pm_nlv mitgezaehlt.                        */
        /* Im Gegensatz zum Load wird aber der offset */
        /* nicht incrementiert sondern das Objekt     */
        /* an der Stelle base + offset - 1 geladen    */
        /* -1, weil der Return Befehl den offset schon*/
        /* Inkrementiert hat                          */
        /*--------------------------------------------*/
            case AS_LOAD:


                 {
/*                   PTR_DESCRIPTOR desc; */
                   T_HEAPELEM arg;

                   arg = *(pm_base + pm_offset - 1);

                   if (T_STR_CHAR( (STACKELEM) arg))
            /*---- Character duerfen nicht an Variablen gebunden werden; ----*/
            /*----             kann der Editor nicht  !!!                ----*/
                   {
                     pm_match_fail = FALSE;
                     ++pm_pc;
                     break;
                   }
                   else
                   {
                     if (T_POINTER (arg))
                         INC_REFCNT ((PTR_DESCRIPTOR) arg);
                     PUSH_PM(arg);
                   }
                   ++pm_nlv;         /* eine Variable  mehr auf dem pm-stack */
                   pm_pc++;
                   break;
                 }

            case A_LOAD:

                 if (T_POINTER (arg))
		   INC_REFCNT ((PTR_DESCRIPTOR) arg);
                 PUSH_PM(arg);

                 ++pm_nlv;         /* eine Varibale mehr auf i-stack  */
                 pm_pc++;
                 break;




        /*-----------LOAD_ARITY--------------------------*/
        /* legt einen neuen Deskriptor an , in den die   */
        /* Stelligkeit des Deskriptors an der Stelle     */
        /* *(pm_base + offset ) eingetragen wird und     */
        /* legt den neuen deskriptor auf den pm-stack    */
        /* Aktionen:                                     */
        /* - Steht an Argumentposition eine Variable ?   */
        /* - Steht an Argumentposition ein Pointer auf   */
        /*   einen Listendeskr. ?                        */
        /* - Anlegen des neuen Descriptors mit convi     */
        /* - neuer deskriptor auf den pm-stack und incr  */
        /*   der Variablem  pm_nlv                       */
        /*-----------------------------------------------*/

            case LOAD_ARITY:

                 if (T_POINTER (*(pm_base + pm_offset)))
                 {
                     if (R_DESC ((*(PTR_DESCRIPTOR) * (pm_base + pm_offset)), class) == C_LIST)
                     {
                    if(( pmdimli( *(pm_base + pm_offset))) == 0)
                           post_mortem ("pm_am kein Platz um Deskriptor auszulagern");
                          ++pm_nlv;
                 /* eine Varibale mehr auf pm-stack  */

                     }
                     else
                     {
                         UNDECIDED_TEST(*(pm_base + pm_offset))

                     }
                 }
                 else
                     pm_match_fail = FALSE;
                 ++pm_pc;
                 break;

            case A_LOAD_ARITY:
        if (T_POINTER (pm_arg))
                 {
                   if (R_DESC ((*(PTR_DESCRIPTOR) pm_arg), class) == C_LIST)
                 {
/* Dimension der Liste auf den Stack umd mit st_dec auslagern */
                              convi (R_LIST ((*(PTR_DESCRIPTOR) pm_arg), dim));
                              PUSH_PM(st_dec ());
                     /* red_ldim ruft conv_int_digit auf und diese Funktion
                        decr. pm_arg . Ist so nicht zu gebrauchen    */
                     /*                red_ldim(pm_arg);         */
                     /*                PUSHSTACK(S_pm, _desc);i *//* red_ldim
                        legt neuen Deskriptor in _desc ab */

                              ++pm_nlv;
                     /* eine Varibale mehr auf i-stack  */
                 ++pm_pc;
                 break;
                          }
          else
          {
          UNDECIDED_TEST(pm_arg)
          }
        }
        else
            pm_match_fail = FALSE;

        ++pm_pc;
        break;


        /*-----------RETURN-----------------------------*/
        /* restauriert die Parameter pm_base, pm_offset */
        /* die vor dem matching einer                   */
        /* Unterliste gueltig waren                     */
        /*----------------------------------------------*/
            case RETURN:

                 /*Ruecksetzen der Werte fuer pm_base und offset*/
                 pm_base = (PTR_HEAPELEM)(s[--num_s]);
                 pm_offset = s[--num_s];

                 pm_pc++;
                 ++pm_offset;
                 break;

        /*-----------END_DOLL---------------------------*/
        /* prueft ob das Listenende erreicht ist und    */
        /* baut ein Stackframe des Backtrackingstack ab */
        /*----------------------------------------------*/
            case END_DOLL:
                 /*Pruefen ob wirklich das Listenende erreicht ist */
                 /*in bt[num_bt + 4] steht die Laenge des Arguments*/
                 /*eingefuehrt wegen dem Pattern <1,$,2>, das sonst*/
                 /*auch gegen <1,1,2,1,1,1> passt                  */
                    if  (( bt[num_bt + 4]  ) == pm_offset )
                        pm_match_fail = TRUE;
                        else
                        pm_match_fail = FALSE;

                         num_bt -= BT_SIZE;



                 pm_pc++;
                 break;

            default:
                 post_mortem ("pm_am: falscher Eintrag im Instructionarray");

        }                          /* ende des switch */

    }                              /* ende while Schleife */

#if DEBUG
    pm_debug ();
#endif

    if (pm_match_fail == TRUE)
    {
        if (pm_undecided == TRUE)
        {
#if DEBUG
pm_debug();
#endif
#if WITHTILDE
        POP_PM_BLOCK;
#else
        POP_PM_BLOCK(var);
#endif /* WITHTILDE */
        REL_HEAP;
        END_MODUL("pm_am");
        PUSH_W(KLAA); /* mah 040294 */
        return(PM_UNDEC);
        }
        else
        {
        /* Freigabe des Heaps. Reservierung ist in pmmat erfolgt*/
        REL_HEAP;

       /* Ein Dollar unter alle AP_1 auf dem M-Stack legen */

#if 0
/*        for (arity = 0; T_AP_1(READSTACK(S_m));
             PPOPSTACK(S_m),arity++);*/

 /*       if (T_DOLLAR(READSTACK(S_m)))
           POP_PM_BLOCK(var);
        else                                  */
           PUSHSTACK(S_m,DOLLAR);

/*        for ( ; arity > 0; --arity)
            PUSHSTACK(S_m,(STACKELEM) AP_1);*/
#endif /* 0: mah 080294 */

        for (i =  3; i <= num_sl;)/* Anlegen der indirekten
                                      Listendeskriptoren */
        {
            if( sl[i + 1] <= sl[i + 2])
            /* es muss eine Liste (ein String) angelegt werden */
            {
                /* sl[i]   : Listendeskriptor  */
                /* sl[i+1] : Anfang            */
                /* sl[i+2] :   Ende            */
                if ((pm_arg = gen_id(sl[i+1]+1, sl[i+2]+1, sl[i]) ) == 0)
                    post_mortem("pm_am: kein Deskriptor fuer slice available.");
                else PUSH_PM(pm_arg);
            }
            else                  /* Differnz ist kleiner gleich Null, d.h. */
            {                     /* leere Liste (oder String) zurueckgeben */
                if (R_DESC((*((PTR_DESCRIPTOR) sl[i])),type) == TY_STRING)
                {
                    PUSH_PM((STACKELEM) _nilstring);
                    INC_REFCNT(_nilstring);
                }
                else
                {
                    PUSH_PM((STACKELEM) _nil);
                    INC_REFCNT(_nil);
                }
            }
            i += 3;
	    pm_nlv++; /* mah 230294 adjustment to lred compiler */
        }
        END_MODUL ("pm_am");
	PUSH_W(SA_TRUE); /* mah 040294 */
#if WITHTILDE
	PUSH_R(SET_VALUE(DOLLAR,pm_nlv));
#endif /* WITHTILDE */
        return (PM_PASST);
        }
    }
    else
    {                                                     
#if WITHTILDE
      POP_PM_BLOCK;
#else
      POP_PM_BLOCK(var);              /* angelegten Block wieder beseitigen */
#endif /* WITHTILDE */
      END_MODUL ("pm_am");
      REL_HEAP;
      PUSH_W(HASH); /* mah 040294 */
      return (PM_PASST_NICHT);
    }

}                                         /* ende pm_am */
