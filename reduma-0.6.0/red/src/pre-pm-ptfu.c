/* $Log: pre-pm-ptfu.c,v $
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 * */

/*****************************************************************************/
/*                                                                           */
/*  in diesem File:                                                          */
/*          pm_comp: der Compiler, der aus Pattern Matchcode erzeugt         */
/*          pm_pattern : wird von pm_comp aufgerufen und erzeugt Code        */
/*                       fuer ein Pattern                                    */
/*          pm_gen: Generiert das Instruction Array                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*----------------------------------------------------------------------------*/
/*  include files                                                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "pminstr.h"
#include <setjmp.h>
#include "case.h"
/*---------------------------------------------------------------------------*/
/* Externe Routinen und Variablen                                            */
/*---------------------------------------------------------------------------*/

extern int  newheap ();     /* pmheap:c   */
STACKELEM  store_expr();    /* rstpro.c   */
extern void pm_patvar();    /* pmpatvar.c */
extern void load_expr();    /* pmpatvar.c */
extern BOOLEAN   check_list_for_store();    /* hfunc.c   */
extern void test_dec_refcnt();
extern void DescDump();
extern void test_inc_refcnt();
extern void ppopstack();
extern void trav_e_a();
extern void res_heap();
extern void rel_heap();
extern void ret_list();
extern STACKELEM st_list();
extern STACKELEM st_expr();
extern T_HEAPELEM * newbuff();
extern int freeheap();
extern void stack_error();

#if DEBUG
extern  FILE * pmdmpfile;
#endif

extern  StackDesc S_pm;          
/*---------------------------------------------------------------------------*/

/* Konstanten                                                                */
/*---------------------------------------------------------------------------*/

#define INSTR_ARRAY_SIZE       300                     /* max. Groesse
                                                                    des Instr.
                                                                    Arrays */
 /* das durch pm_gen angelegt wird */



/*---------------------------------------------------------------------------*/
/* Typdefinitionen                                                           */
/*---------------------------------------------------------------------------*/


typedef int INSTRUCTION;

static  INSTRUCTION code[INSTR_ARRAY_SIZE];             /*
                                                                  Instruction-a
                                                                  rray
                                                                  Definition
                                                                  */

static int  index;                                               /* Index fuer
                                                                    Instruction-A
                                							      rray */
static int nskipvar;
static int nvar;

/*---------------------------------------------------------------------------*/
/* Debugvariablen                                                            */
/*---------------------------------------------------------------------------*/

#if DEBUG
extern int  outbefehl;
#endif






/*****************************************************************************/
/*                                                                           */
/* pm_out_befehl gibt einen Befehl aus, nachdem er durch die Funktion        */
/* pm_gen generiert wurde.                                                   */
/*                                                                           */
/*****************************************************************************/
#if DEBUG
void pm_out_befehl (index)

int     index;

{


    START_MODUL ("pm_out_befehl");

    switch (code[index])
    {
        case PM_PUSH:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: PM_PUSH \n", index);
            break;

        case JMP:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: JMP \n", index);
            break;





        case ATOM_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: SA_TEST \n", index);
            break;

        case A_ATOM_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_SA_TEST \n", index);
            break;

        case DECR:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: DECR \n", index);
            break;

        case A_TESTN:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_TESTN \n", index);
            break;

        case TESTN:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: TESTN \n", index);
            break;

        case STR_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: STR_TEST \n", index);
            break;
        case DIG_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: DIG_TEST \n", index);
            break;
        case SCAL_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: SCAL_TEST \n", index);
            break;
        case MATRIX_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: MATRIX_TEST \n", index);
            break;
        case VECTOR_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: VECTOR_TEST \n", index);
            break;
        case TVECTOR_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: TVECTOR_TEST \n", index);
            break;

        case LOAD_ARITY:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: LOAD_ARITY \n", index);
            break;

        case A_LOAD_ARITY:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_LOAD_ARITY \n", index);
            break;

        case L_LOAD_B:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: L_LOAD_B \n", index);
            break;

        case L_LOAD_A:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: L_LOAD_A \n", index);
            break;

        case BOOL_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: BOOL_TEST \n", index);
            break;

        case NIL_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: NIL_TEST \n", index);
            break;

        case NILSTRING_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: NILSTRING_TEST \n", index);
            break;

        case A_NILSTRING_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_NILSTRING_TEST \n", index);
            break;

        case A_NIL_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_NIL_TEST \n", index);
            break;

        case NILM_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: NILM_TEST \n", index);
            break;

        case A_NILM_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_NILM_TEST \n", index);
            break;

        case NILV_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: NILV_TEST \n", index);
            break;

        case A_NILV_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_NILV_TEST \n", index);
            break;

        case NILTV_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: NILTV_TEST \n", index);
            break;

        case A_NILTV_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_NILTV_TEST \n", index);
            break;


        case TY_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: TY_TEST \n", index);
            break;


        case A_STR_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_STR_TEST \n", index);
            break;
        case A_DIG_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_DIG_TEST \n", index);
            break;
        case A_SCAL_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_SCAL_TEST \n", index);
            break;
        case A_MATRIX_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_MATRIX_TEST \n", index);
            break;
        case A_VECTOR_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_VECTOR_TEST \n", index);
            break;
        case A_TVECTOR_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_TVECTOR_TEST \n", index);
            break;

        case A_BOOL_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_BOOL_TEST \n", index);
            break;

        case A_TY_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_TY_TEST \n", index);
            break;

        case LOAD:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: LOAD \n", index);
            break;

        case INCR:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: INCR \n", index);
            break;

        case AS_LOAD:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: AS_LOAD \n", index);
            break;

        case A_KTEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_KTEST \n", index);
            break;

        case A_LOAD:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_LOAD \n", index);
            break;


        case RETURN:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: RETURN\n", index);
            break;

        case END_DOLL:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: END_DOLL\n", index);
            break;

        case EOT:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: EOT \n", index);
            break;

        case PM_CUTT:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: PM_CUTT \n", index);
            break;

        case PM_DOLL:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: PM_DOLL \n", index);
            break;

        case GEN_BLOCK:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: GEN_BLOCK \n", index);
            break;

        case SKIPSKIP:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: SKIPSKIP \n", index);
            break;

        case LA_SKIPSKIP:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: LA_SKIPSKIP \n", index);
            break;

        case A_TESTV:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_TESTV \n", index);
            break;

        case TESTV:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: TESTV \n", index);
            break;

        case JMP2:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: JMP2 \n", index);
            break;

        case IN_TEST:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: IN_TEST \n", index);
            break;

        case IN_TEST1:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: IN_TEST1 \n", index);
            break;

        case IN_SKIPSKIP:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: IN_SKIPSKIP \n", index);
            break;

        case LA_IN_SKIPSKIP:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: LA_IN_SKIPSKIP \n", index);
            break;

        case IN_PM_DOLL:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: IN_PM_DOLL \n", index);
            break;

        case DEL_BLOCKS:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: DEL_BLOCKS \n", index);
            break;

        case A_LI_CHECK:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_LI_CHECK \n", index);
            break;

        case LI_CHECK:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: LI_CHECK \n", index);
            break;

        case A_STR_CHECK:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: A_STR_CHECK \n", index);
            break;

        case STR_CHECK:
            fprintf (pmdmpfile, "Index: %5.5u Befehl: STR_CHECK \n", index);
            break;

        default:

            if (T_POINTER ((STACKELEM) code[index]))
            {
                 fprintf (pmdmpfile, "Index: %5.5u Parameter:  %8.8x \n", index, code[index]);
            }
            else
            {
                 fprintf (pmdmpfile, "Index: %5.5u Parameter:  %5.5u \n", index, code[index]);
            }

            break;
    }                              /* ende des switch */

    END_MODUL ("pm_out_befehl");

    return;
}
#endif
/*****************************************************************************/
/*                                                                           */
/* pm_gen. Generiert das Instruction-Array                                   */
/*                                                                           */
/*****************************************************************************/



void pm_gen (befehl)

INSTRUCTION befehl;

{
    START_MODUL ("pm_gen");

    if (index > INSTR_ARRAY_SIZE)
        post_mortem ("pm_gen: Instruction-Array zu klein");

    code[index] = befehl;

# if DEBUG
    if (outbefehl)
        pm_out_befehl (index);
# endif

    ++index;

    END_MODUL ("pm_gen");

    return;
}




/******************************************************************************/
/* pm_pattern() arbeitet auf dem globalen Array code und generiert Code fuer  */
/* ein Pattern. Das Pattern liegt auf dem E-Stack und wird zur Codegenerierung*/
/* traversiert. Eventuell  eingelagerte Listen werden dabei wieder auf den    */
/* E_Stack eingelagert. Anschliessend erfolgt rekursiver Aufruf von pattern   */
/******************************************************************************/



void pm_pattern ()

{

    STACKELEM x,
              y;                       /*    y fuer In-Konstrukt benoetigt    */

    int     i,                         /*   Laufvariable fuer for Schleife    */
            del_blocks = 0;            /* gibt die Anzahl der BS-frames an,
                                          die durch "in ... ' ? `" angelegt
                                          wurden. Diese muessen bei Verlassen
                                          der Liste wieder abgebaut werden.   */

    STACKELEM pm_h1,                   /*   Hilfsvariablen fuer as ... x      */
              pm_h2,                   /*      und as in ... ' ? ` x          */
              pm_h3;

    int pm_arity;                      /* Stelligkeit eines Listenkonstruktors*/
    int pm_tab_i;                      /* Index fuer pm_tab                   */
    int pm_pluscount = 0;              /* Anzahl der PLUS-Operatoren          */
    T_HEAPELEM *pm_tab;                /* Tabelle die Stellen des Codevektors
                                          aufnimmt an denen nach Durchlauf
                                          aller Listeneintraege nachtraeglich
                                          Eintragungen gemacht werden muessen */
    BOOLEAN pm_dollar_flag = FALSE;

    START_MODUL ("pm_pattern");


    x = POPSTACK (S_e);

  /*------------------*/
    if (T_POINTER (x))
  /*------------------*/
    {
      if ((R_DESC ((*(PTR_DESCRIPTOR) x), type)) == TY_NAME) 
          pm_gen(LOAD);
      else
         {
        /*---- beim Einlagern durch ret_list wurde  der Refcnt ----*/
        /*-    fuer die Listenelemente erhoeht                    -*/
        /*---- jetzt wieder dekrementieren                     ----*/
        DEC_REFCNT((PTR_DESCRIPTOR) x);

        switch (R_DESC ((*(PTR_DESCRIPTOR) x), class))
        {
        /* ---- strings nun mit class = C_LIST  (sj) ---- */
        /* ---------------------------------------------- */


        case C_SCALAR:        /* eine Zahl, fixformat */

            pm_gen (SCAL_TEST);

            pm_gen (x);       /* den descr. in das Instr. Array */
            break;


        case C_DIGIT:         /* eine Zahl, varformat */

            pm_gen (DIG_TEST);

            pm_gen (x);       /* den descr. in das Instr. Array */
            break;


        case C_MATRIX:        /* eine Matrix */

            pm_gen (MATRIX_TEST);

            pm_gen (x);       /* den descr. in das Instr. Array */
            break;

        case C_VECTOR:        /* ein Vektor */

            pm_gen (VECTOR_TEST);

            pm_gen (x);       /* den descr. in das Instr. Array */
            break;

        case C_TVECTOR:         /* ein TVektor */

            pm_gen (TVECTOR_TEST);

            pm_gen (x);       /* den descr. in das Instr. Array */
            break;

        case C_LIST:
            if ((pm_arity = R_LIST((*(PTR_DESCRIPTOR) x), dim)) == 0)
            {
                if ((R_DESC((*(PTR_DESCRIPTOR) x), type)) == TY_STRING)
                    pm_gen(NILSTRING_TEST);
                else
                    pm_gen(NIL_TEST);
                break;
            }
            if (( pm_tab = (T_HEAPELEM *)newbuff( pm_arity + 1)) == 0)
                post_mortem("pmcomp: Heap out of space ");
                /*---- pm_tab braucht wegen < ...> mindestens einen Eintrag --*/
                /*---- fuer < und einen zweiten fuer ... --> pm_arity+1   ----*/

            if (R_DESC((*(PTR_DESCRIPTOR) x), type) == TY_STRING)
                pm_gen(STR_CHECK);             /* Unterscheidung von String- */
            else
                pm_gen(LI_CHECK);              /* und Listen-Konstruktoren   */

            pm_gen (TESTN);

            pm_tab_i = 0;                 /* Initialisierung
                                             des Zaehlers fuer
                                             pm_tab */
            pm_tab[ pm_tab_i] = index;    /* Merken der Stelle
                                             an der spaeter TESTN
                                             und Parameter fuer
                                             die Mindeststelligkeit
                                             eingetragen werden muss */
            ++ pm_tab_i;
            pm_gen (pm_arity );


          /* zusaetlicher Parameter, der einen offset fuer*/
          /* die Zahl der generierten Befehle  der Liste  */
          /* angibt; wird beim Backpatching eingetragen   */
            pm_gen( index);

            /*---- refcnt erhoehen, da ret_list refcnt dekrementiert ----*/
            INC_REFCNT((PTR_DESCRIPTOR) x);

            ret_list (x);             /* Liste auf E-Stack     */

            PPOPSTACK (S_e);          /* Pop fuer den Listenkonstruktor */

     /*---- Listenkonstruktor wird abgearbeitet ----*/

            for (i = 0; i < pm_arity; i++)
            {
                pm_h1 = POPSTACK(S_e);
                pm_h2 = READSTACK(S_e);
                if ( T_PM_SKSKIP( pm_h1))
                {
                    pm_gen(SKIPSKIP);
                    pm_tab[pm_tab_i] = index;
                    pm_gen(i - pm_tab_i +1);  /* Zahl
                                                 der bisher gefundenen
                                                 Listenelemente  ohne
                                                 ... */
                    pm_tab_i++;
                }
                else
                if ((T_PM_AS( pm_h1)) && (T_PM_SKSKIP(pm_h2)))
                /*---- ein as gefolgt von ... auf dem E-stack ----*/
                {
                    pm_gen(L_LOAD_A);
                    PPOPSTACK(S_e);    /* das pm_hs = ... */
                    pm_gen(SKIPSKIP);
                    pm_tab[pm_tab_i] = index;
                    pm_gen(i - pm_tab_i + 1);
                    pm_tab_i++;
                    pm_gen(L_LOAD_B);
                    PPOPSTACK(S_e);    /*die Variable */
                }
                else
                if ( T_PM_DOLLAR( pm_h1))
                {
                    pm_gen(GEN_BLOCK);
                    pm_gen(PM_DOLL);
                    pm_tab[pm_tab_i] = index;
                    pm_gen(i - pm_tab_i +1);  /* Zahl
                                                 der bisher gefundenen
                                                 Listenelemente  ohne
                                                 ... */
                    pm_tab_i++;
                    if (i <= pm_arity -2)
                    /*---- den Fall, dass ein Dollar an letzter Stelle ---*/
                    /*---- steht ausschliessen                         ---*/
                    {
                        pm_pattern();
                        pm_gen(PM_CUTT);
                        i++;          /* Zahl der Aufrufe von pm_pattern
                                         berichtigen */
                    }
                }
                else
                if ((T_PM_AS( pm_h1)) && (T_PM_DOLLAR(pm_h2)))
                /*---- ein as gefolgt von einem $ auf dem E-stack ----*/
                {
                    pm_gen(GEN_BLOCK);
                    pm_gen(L_LOAD_A);
                    PPOPSTACK(S_e);     /* das pm_hs = ... */
                    pm_gen(PM_DOLL);
                    pm_tab[pm_tab_i] = index;
                    pm_gen(i - pm_tab_i + 1);
                    pm_tab_i++;
                    pm_gen(L_LOAD_B);
                    PPOPSTACK(S_e);     /*die Variable */
                    if (i <= pm_arity -2)
                    /*---- den Fall, dass ein Dollar an letzter Stelle ---*/
                    /*---- steht ausschliessen                         ---*/
                    {
                        pm_pattern();
                        pm_gen(PM_CUTT);
                        i++;          /* Zahl der Aufrufe von pm_pattern
                                         berichtigen */
                    }
                }
                else
                if ((T_PM_AS( pm_h1)) && (T_PM_IN(pm_h2)))
                /*---- ein as gefolgt von einem in auf dem E-stack ----*/
                {
                    PPOPSTACK(S_e);              /* um naechstes Stackelement
                                                    inspizieren zu koennen    */
                    pm_h3 = READSTACK(S_e);

                    if (T_PM_SKIP(pm_h3))
                    /*---- ein . auf dem E-stack ----*/
                    {
                        pm_gen(L_LOAD_A);

                        pm_gen(IN_TEST1);
                        PPOPSTACK(S_e);           /*        SKIP poppen       */
                        y = READSTACK(S_e);  
                        if (T_STRING(y))         /* string auslagern */
                           {
                            TRAV_E_A;          
                            if (check_list_for_store())  /* darf String ausgelagert werden? */
                               {
                                if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                    post_mortem("pm_pattern: heap out of space in st_list");
                               }
                           }
                        else                            
                           {
                             PPOPSTACK(S_e);
                             DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                           }                        /*tern Refcount zu hoch;   */
                        pm_gen(y);

                        pm_gen(L_LOAD_B);
                        PPOPSTACK(S_e);           /*        die Variable       */
                    }
                    else
                    if ( (T_PM_SKSKIP(pm_h3)) || (T_PM_PT_PLUS(pm_h3)) )
                    /*---- ... oder .+ auf dem E-stack ----*/
                    {
                        pm_gen(L_LOAD_A);

                        pm_gen(GEN_BLOCK);
                        if (T_PM_PT_PLUS(pm_h3))
                        {
                            pm_gen(INCR);
                            pm_pluscount++;       /* Anz. PLUS-Op. festhalten */
                        }
                        else
                            pm_gen(JMP2);
                        pm_gen(IN_TEST);
                        PPOPSTACK(S_e);           /*      SKIPSKIP poppen     */
                        y = READSTACK(S_e);  
                        if (T_STRING(y))         /* string auslagern */
                           {
                            TRAV_E_A;          
                            if (check_list_for_store())  /* darf String ausgelagert werden? */
                               {
                                if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                    post_mortem("pm_pattern: heap out of space in st_list");
                               } 
                           }
                        else                            
                           {
                             PPOPSTACK(S_e);
                             DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                           }                        /*tern Refcount zu hoch;   */
                        pm_gen(y);
                        pm_gen(IN_SKIPSKIP);
                        pm_tab[pm_tab_i] = index; /* Pos. fuer Param. merken  */
                        pm_gen(i - pm_tab_i++ +1);/* Zahl der bisher gefunden-
                                                     nen Listenelemente ohne
                                                     ... ;                    */
                        del_blocks++;             /*  ein Block mehr abbauen  */

                        pm_gen(L_LOAD_B);
                        PPOPSTACK(S_e);            /*       die Variable       */
                    }
                    else
                    if (T_PM_DOLLAR(pm_h3))
                    /*---- ein $ auf dem E-stack ----*/
                    {
                        pm_gen(GEN_BLOCK);
                        pm_gen(L_LOAD_A);
                        pm_gen(JMP2);
                        pm_gen(IN_TEST);
                        PPOPSTACK(S_e);           /*       DOLLAR  poppen     */
                        y = READSTACK(S_e);  
                        if (T_STRING(y))         /* string auslagern */
                           {
                            TRAV_E_A;          
                            if (check_list_for_store())  /* darf String ausgelagert werden? */
                               {
                                if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                    post_mortem("pm_pattern: heap out of space in st_list");
                               }
                           }
                        else                            
                           {
                             PPOPSTACK(S_e);
                             DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                           }                        /*tern Refcount zu hoch;   */
                        pm_gen(y);
                        pm_gen(IN_PM_DOLL);
                        pm_tab[pm_tab_i] = index;
                        pm_gen(i - pm_tab_i + 1);
                        pm_tab_i++;
                        pm_gen(L_LOAD_B);
                        PPOPSTACK(S_e);            /*       die Variable       */
                        if (i <= pm_arity -2)
                        /*---- den Fall, dass ein Dollar an letzter Stelle ---*/
                        /*---- steht, ausschliessen                        ---*/
                        {
                            pm_pattern();
                            pm_gen(PM_CUTT);
                            i++;                  /* Zahl der Aufrufe von
                                                     pm_pattern berichtigen;  */
                        }
                    }
                }  /* Ende der "as in"-Kombinationen */
                else
                if ((T_PM_IN( pm_h1)) && (T_PM_SKIP(pm_h2)))
                /*---- ein in gefolgt von einem . auf dem A-Stack ----*/
                {
                    pm_gen(IN_TEST1);
                    PPOPSTACK(S_e);               /*        SKIP poppen       */
                    y = READSTACK(S_e);  
                    if (T_STRING(y))         /* string auslagern */
                       {
                        TRAV_E_A;          
                        if (check_list_for_store())  /* darf String ausgelagert werden? */
                           {
                            if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                post_mortem("pm_pattern: heap out of space in st_list");
                           }
                       }
                    else                            
                       {
                         PPOPSTACK(S_e);
                         DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                       }                        /*tern Refcount zu hoch;   */
                    pm_gen(y);
                }
                else
                if ( (T_PM_IN( pm_h1)) &&
                     ((T_PM_SKSKIP(pm_h2)) || (T_PM_PT_PLUS(pm_h2))) )
                /*---- ein in gefolgt von ... oder .+ auf dem A-Stack ----*/
                {
                    pm_gen(GEN_BLOCK);
                    if (T_PM_PT_PLUS(pm_h2))
                    {
                        pm_gen(INCR);
                        pm_pluscount++;           /* Anz. PLUS-Op. festhalten */
                    }
                    else
                        pm_gen(JMP2);                    pm_gen(IN_TEST);
                    PPOPSTACK(S_e);               /*      SKIPSKIP poppen     */
                    y = READSTACK(S_e);  
                    if (T_STRING(y))         /* string auslagern */
                       {
                        TRAV_E_A;          
                        if (check_list_for_store())  /* darf String ausgelagert werden? */
                           {
                            if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                post_mortem("pm_pattern: heap out of space in st_list");
                           }
                       }
                    else                            
                       {
                         PPOPSTACK(S_e);
                         DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                       }                        /*tern Refcount zu hoch;   */
                    pm_gen(y);
                    pm_gen(IN_SKIPSKIP);
                    pm_tab[pm_tab_i] = index;     /* Pos. fuer Param. merken  */
                    pm_gen(i - pm_tab_i++ +1);    /* Zahl der bisher gefunden-
                                                     nen Listenelemente ohne
                                                     ... ;                    */
                    del_blocks++;                 /*  ein Block mehr abbauen  */
                }
                else
                if ((T_PM_IN( pm_h1)) && (T_PM_DOLLAR(pm_h2)))
                /*---- ein in gefolgt von einem $ auf dem A-Stack ----*/
                {
                    pm_gen(GEN_BLOCK);
                    pm_gen(JMP2);
                    pm_gen(IN_TEST);
                    PPOPSTACK(S_e);               /*      SKIPSKIP poppen     */
                    y = READSTACK(S_e);  
                    if (T_STRING(y))         /* string auslagern */
                       {
                        TRAV_E_A;          
                        if (check_list_for_store())  /* darf String ausgelagert werden? */
                           {
                            if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                post_mortem("pm_pattern: heap out of space in st_list");
                           }
                       }
                    else                            
                       {
                         PPOPSTACK(S_e);
                         DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                       }                        /*tern Refcount zu hoch;   */
                    pm_gen(y);
                    pm_gen(IN_PM_DOLL);
                    pm_tab[pm_tab_i] = index;     /* Pos. fuer Param. merken  */
                    pm_gen(i - pm_tab_i++ +1);    /* Zahl der bisher gefunden-
                                                     nen Listenelemente ohne
                                                     ... ;                    */
                    if (i <= pm_arity -2)
                               /* den Fall, dass ein Dollar an letzter Stelle */
                               /* steht, ausschliessen                        */
                    {
                        pm_pattern();
                        pm_gen(PM_CUTT);
                        i++;                      /* Zahl der Aufrufe von
                                                     pm_pattern berichtigen   */
                    }
                }
                else
                if (   ((R_DESC((*(PTR_DESCRIPTOR) x), type)) == TY_STRING) &&
                       ( (T_POINTER(pm_h1)) &&
                         ( R_DESC((*(PTR_DESCRIPTOR)pm_h1),type) == TY_NAME) ) )
                {
                    pm_gen(L_LOAD_A);
                    pm_gen(INCR); 
                    pm_gen(L_LOAD_B);
                }
                else
                if (   ((R_DESC((*(PTR_DESCRIPTOR) x), type)) == TY_STRING) &&
                       ((T_PM_AS(pm_h1)) && (T_PM_SKIP(pm_h2)))   )
                {
                    pm_gen(L_LOAD_A);
                    PPOPSTACK(S_e);           /* der  .       */
                    pm_gen(INCR);
                    pm_gen(L_LOAD_B);
                    PPOPSTACK(S_e);           /* die Variable */
                }
                else
                {
                    PUSHSTACK(S_e, pm_h1);
                    pm_pattern ();
                }
            }  /* Ende der for-Schleife */

            --pm_tab_i;               /* beim letzten Eintrag
                                         wurde pm_tab_i um eins
                                         erhoeht */
            if (pm_tab_i >  0)        /* d.h. in der Tabelle steht
                                         nicht nur die Stelle
                                         an der A_TESTN eingetragen
                                         wurde also enthaelt die
                                         Liste ... */
            {
            /*---- Fuer das lezte SKSKIP ist kein Backtracking ----*/
            /*-    notwendig, deshalb Unterscheidung von anderen  -*/
            /*---- SkSKIP durch den Befehl LA_SKSKIP           ----*/
                if (code[pm_tab [pm_tab_i] -1] == SKIPSKIP)
                    code[pm_tab[ pm_tab_i ] - 1] = LA_SKIPSKIP;
                else
                if (code[pm_tab [pm_tab_i] -1] == IN_SKIPSKIP)
                    code[pm_tab[ pm_tab_i ] - 1] = LA_IN_SKIPSKIP;

                /*---- Berechnung der Stelligkeit, die die zu      ----*/
                /*-  matchende Liste an dieser Stelle noch haben      -*/
                /*-  muss: Zahl der Listenelemente - Zahl der         -*/
                /*-        gefundenen SKSKIPS bis zu dieser Stelle    -*/
                /*----     - Zahl der schon gematchten Listenelemente -*/
                else
                if ((code[pm_tab [pm_tab_i] -1] == PM_DOLL)   ||
                    (code[pm_tab [pm_tab_i] -1] == IN_PM_DOLL)  )
                /*---- an letzter Stelle der ... und $ steht ein Dollar --*/
                    pm_dollar_flag = TRUE;
                for ( i =  pm_tab_i; i > 0; i--)
                {

                    code[pm_tab[i ]] =
                          pm_arity - pm_tab_i   - code[pm_tab[i  ]];

                }

                code[pm_tab[ 0 ] -1] = TESTV;     /* TESTV statt TESTN */
                code[pm_tab[ 0]] =  pm_arity - pm_tab_i + pm_pluscount; /* Mindestarity
                                                                           statt arity
                                                                           angeben */
            }

            if (pm_dollar_flag)
                pm_gen(END_DOLL);

            if (del_blocks > 0)
            {
                pm_gen(DEL_BLOCKS);
                pm_gen(del_blocks);
            }
            pm_gen (RETURN);
                 /*offset fuer den Code des Matching der Listenelemente als zweiten   */
                 /*Parameter hinter TESTV oder TESTN schreiben. Inhalt von pm_tab[0]+1*/
                 /*ist zunaechst der Index . Nach Erzeugung des Codes fuer die Liste  */
                 /*erfolgt dann die Berechnung des offsets . + 2 weil pm_pc noch auf  */
                 /*TESTN zeigt wenn L_UNDECIDED aufgerufen wird                       */    
                 code[pm_tab[ 0 ] +1] =  index - code[pm_tab[0] + 1] + 2;

                 freeheap(pm_tab); /* Freigabe der Tabelle der ... */
            break;


        default:
            printf("Stackelement in Hexdarstellung %8.8x \n", x);
            post_mortem ("pm_comp: Unzulaessiger Ausdruck im Pattern");

        }                          /* ende des Switch */
     }

    }                              /* ende von T_POINTER(x) */

    else
  /*---------------*/
    if( (T_POINTER(x)) &&
        ( R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME ) )
      /* eine Patternvariable */
  /*---------------*/
        pm_gen (LOAD);

    else
  /*---------------*/
    if (T_PM_SKIP (x))                     /*  ein Skip, dann nichts machen  */
  /*---------------*/
        pm_gen(INCR);

    else
  /*---------------------------------*/
    if ( (T_LIST(x))||(T_STRING(x)) )      /* tritt auf, wenn Liste (String) */
  /*---------------------------------*/    /* als expression abgespeichert   */
    {                                      /* wurde;                         */
        pm_arity = ARITY(x);

        if (( pm_tab = (T_HEAPELEM *)newbuff( pm_arity + 1)) == 0)
            post_mortem("pmcomp: Heap out of space "); /* pm_tab
              braucht wegen < ...> mindestens einen Eintrag
              fuer < und einen zweiten fuer ... --> pm_arity+1*/

                if (T_STRING(x))
                    pm_gen(STR_CHECK);         /* Unterscheidung von String- */
                else
                    pm_gen(LI_CHECK);          /* und Listen-Konstruktoren   */

        pm_gen (TESTN);

        pm_tab_i = 0;                     /* Initialisierung
                                             des Zaehlers fuer
                                             pm_tab */
        pm_tab[ pm_tab_i] = index;        /* Merken der Stelle
                                             an der spaeter TESTN
                                             und Parameter fuer
                                             die Mindeststelligkeit
                                             eingetragen werden muss */
        ++ pm_tab_i;
        pm_gen (pm_arity );

       /* zusaetlicher Parameter, der einen offset fuer*/
       /* die Zahl der generierten Befehle  der Liste  */
       /* angibt; wird beim Backpatching eingetragen   */
        pm_gen( index);


 /*---- Listenkonstruktor wird abgearbeitet ----*/

        for (i = 0 ; i < pm_arity; i++)
        {
            pm_h1 = POPSTACK(S_e);
            pm_h2 = READSTACK(S_e);
            if ( T_PM_SKSKIP( pm_h1))
            {
                pm_gen(SKIPSKIP);
                pm_tab[pm_tab_i] = index;
                pm_gen(i - pm_tab_i +1);  /* Zahl der bisher gefundenen
                                             Listenelemente  ohne ... */
                pm_tab_i++;
            }
            else
            if ((T_PM_AS( pm_h1)) && (T_PM_SKSKIP(pm_h2)))
                     /* ein as gefolgt von ... auf dem E-stack */
            {
                pm_gen(L_LOAD_A);
                PPOPSTACK(S_e);            /* das pm_hs = ... */
                pm_gen(SKIPSKIP);
                pm_tab[pm_tab_i] = index;
                pm_gen(i - pm_tab_i + 1);
                pm_tab_i++;
                pm_gen(L_LOAD_B);
                PPOPSTACK(S_e);            /*die Variable */
            }
            else
            if ( T_PM_DOLLAR( pm_h1))
            {
                pm_gen(GEN_BLOCK);
                pm_gen(PM_DOLL);
                pm_tab[pm_tab_i] = index;
                pm_gen(i - pm_tab_i +1);  /* Zahl der bisher gefundenen
                                             Listenelemente  ohne ... */
                pm_tab_i++;
                if (i <= pm_arity -2)
                        /* den Fall, dass ein Dollar an letzter Stelle */
                        /* steht ausschliessen                         */
                {
                    pm_pattern();
                    pm_gen(PM_CUTT);
                    i++;                  /* Zahl der Aufrufe von pm_pattern
                                             berichigen */
                }
            }
            else
            if ((T_PM_AS( pm_h1)) && (T_PM_DOLLAR(pm_h2)))
                     /* ein as gefolgt von einem $ auf dem E-stack */
            {
                pm_gen(GEN_BLOCK);
                pm_gen(L_LOAD_A);
                PPOPSTACK(S_e);            /* das pm_hs = ... */
                pm_gen(PM_DOLL);
                pm_tab[pm_tab_i] = index;
                pm_gen(i - pm_tab_i + 1);
                pm_tab_i++;
                pm_gen(L_LOAD_B);
                PPOPSTACK(S_e);            /* die Variable */
                if (i <= pm_arity -2)
                        /* den Fall, dass ein Dollar an letzter Stelle */
                        /* steht ausschliessen                         */
                {
                    pm_pattern();
                    pm_gen(PM_CUTT);
                    i++;                  /* Zahl der Aufrufe von pm_pattern
                                             berichtigen */
                }
            }
            else
            if ((T_PM_AS( pm_h1)) && (T_PM_IN(pm_h2)))
            /*---- ein as gefolgt von einem in auf dem E-stack ----*/
            {
                PPOPSTACK(S_e);              /* um naechstes Stackelement
                                                inspizieren zu koennen    */
                pm_h3 = READSTACK(S_e);

                if (T_PM_SKIP(pm_h3))
                /*---- ein . auf dem E-stack ----*/
                {
                    pm_gen(L_LOAD_A);

                    pm_gen(IN_TEST1);
                    PPOPSTACK(S_e);           /*        SKIP poppen       */
                    y = READSTACK(S_e);  
                    if (T_STRING(y))         /* string auslagern */
                       {
                        TRAV_E_A;          
                        if (check_list_for_store())  /* darf String ausgelagert werden? */
                           {
                            if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                post_mortem("pm_pattern: heap out of space in st_list");
                           }
                       }
                    else                            
                       {
                         PPOPSTACK(S_e);
                         DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                       }                        /*tern Refcount zu hoch;   */
                    pm_gen(y);

                    pm_gen(L_LOAD_B);
                    PPOPSTACK(S_e);           /*        die Variable       */
                }
                else
                if ( (T_PM_SKSKIP(pm_h3)) || (T_PM_PT_PLUS(pm_h3)) )
                /*---- ... oder .+ auf dem E-stack ----*/
                {
                    pm_gen(L_LOAD_A);

                    pm_gen(GEN_BLOCK);
                    if (T_PM_PT_PLUS(pm_h3))
                    {
                        pm_gen(INCR);
                        pm_pluscount++;       /* Anz. PLUS-Op. festhalten */
                    }
                    else
                        pm_gen(JMP2);
                    pm_gen(IN_TEST);
                    PPOPSTACK(S_e);           /*      SKIPSKIP poppen     */
                    y = READSTACK(S_e);  
                    if (T_STRING(y))         /* string auslagern */
                       {
                        TRAV_E_A;          
                        if (check_list_for_store())  /* darf String ausgelagert werden? */
                           {
                            if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                post_mortem("pm_pattern: heap out of space in st_list");
                           }
                       }
                    else                            
                       {
                         PPOPSTACK(S_e);
                         DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                       }                        /*tern Refcount zu hoch;   */
                    pm_gen(y);
                    pm_gen(IN_SKIPSKIP);
                    pm_tab[pm_tab_i] = index; /* Pos. fuer Param. merken  */
                    pm_gen(i - pm_tab_i++ +1);/* Zahl der bisher gefunden-
                                                 nen Listenelemente ohne
                                                 ... ;                    */
                    del_blocks++;             /*  ein Block mehr abbauen  */

                    pm_gen(L_LOAD_B);
                    PPOPSTACK(S_e);            /*       die Variable       */
                }
                else
                if (T_PM_DOLLAR(pm_h3))
                /*---- ein $ auf dem E-stack ----*/
                {
                    pm_gen(GEN_BLOCK);
                    pm_gen(L_LOAD_A);
                    pm_gen(JMP2);
                    pm_gen(IN_TEST);
                    PPOPSTACK(S_e);           /*       DOLLAR  poppen     */
                    y = READSTACK(S_e);  
                    if (T_STRING(y))         /* string auslagern */
                       {
                        TRAV_E_A;          
                        if (check_list_for_store())  /* darf String ausgelagert werden? */
                           {
                            if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                               post_mortem("pm_pattern: heap out of space in st_list");
                           }
                        }
                    else                            
                       {
                         PPOPSTACK(S_e);
                         DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                       }                        /*tern Refcount zu hoch;   */
                    pm_gen(y);
                    pm_gen(IN_PM_DOLL);
                    pm_tab[pm_tab_i] = index;
                    pm_gen(i - pm_tab_i + 1);
                    pm_tab_i++;
                    pm_gen(L_LOAD_B);
                    PPOPSTACK(S_e);            /*       die Variable       */
                    if (i <= pm_arity -2)
                    /*---- den Fall, dass ein Dollar an letzter Stelle ---*/
                    /*---- steht, ausschliessen                        ---*/
                    {
                        pm_pattern();
                        pm_gen(PM_CUTT);
                        i++;                  /* Zahl der Aufrufe von
                                                 pm_pattern berichtigen;  */
                    }
                }
            }  /* Ende der "as in"-Kombinationen */
            else
            if ((T_PM_IN( pm_h1)) && (T_PM_SKIP(pm_h2)))
            /*---- ein in gefolgt von einem . auf dem A-Stack ----*/
            {
                pm_gen(IN_TEST1);
                PPOPSTACK(S_e);               /*        SKIP poppen       */
                y = READSTACK(S_e);  
                if (T_STRING(y))         /* string auslagern */
                   {
                    TRAV_E_A;          
                    if (check_list_for_store())  /* darf String ausgelagert werden? */
                       {
                        if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                            post_mortem("pm_pattern: heap out of space in st_list");
                       }
                   }  
                else                            
                   {
                     PPOPSTACK(S_e);
                     DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                   }                        /*tern Refcount zu hoch;   */
                pm_gen(y);
            }
            else
            if ( (T_PM_IN( pm_h1)) &&
                 ((T_PM_SKSKIP(pm_h2)) || (T_PM_PT_PLUS(pm_h2))) )
            /*---- ein in gefolgt von ... oder .+ auf dem A-Stack ----*/
            {
                pm_gen(GEN_BLOCK);
                if (T_PM_PT_PLUS(pm_h2))
                {
                    pm_gen(INCR);
                    pm_pluscount++;           /* Anz. PLUS-Op. festhalten */
                }
                else
                    pm_gen(JMP2);                pm_gen(IN_TEST);
                PPOPSTACK(S_e);               /*      SKIPSKIP poppen     */
                y = READSTACK(S_e);  
                if (T_STRING(y))         /* string auslagern */
                   {
                    TRAV_E_A;          
                    if (check_list_for_store())  /* darf String ausgelagert werden? */
                       {
                        if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                            post_mortem("pm_pattern: heap out of space in st_list");
                       }
                   }  
                else                            
                   {
                     PPOPSTACK(S_e);
                     DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                   }                        /*tern Refcount zu hoch;   */
                pm_gen(y);
                pm_gen(IN_SKIPSKIP);
                pm_tab[pm_tab_i] = index;     /* Pos. fuer Param. merken  */
                pm_gen(i - pm_tab_i++ +1);    /* Zahl der bisher gefunden-
                                                 nen Listenelemente ohne
                                                 ... ;                    */
                del_blocks++;                 /*  ein Block mehr abbauen  */
            }
            else
            if ((T_PM_IN( pm_h1)) && (T_PM_DOLLAR(pm_h2)))
            /*---- ein in gefolgt von einem $ auf dem A-Stack ----*/
            {
                pm_gen(GEN_BLOCK);
                pm_gen(JMP2);
                pm_gen(IN_TEST);
                PPOPSTACK(S_e);               /*      SKIPSKIP poppen     */
                y = READSTACK(S_e);  
                if (T_STRING(y))         /* string auslagern */
                   {
                    TRAV_E_A;          
                    if (check_list_for_store())  /* darf String ausgelagert werden? */
                       {
                        if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                            post_mortem("pm_pattern: heap out of space in st_list");
                       }
                   }  
                else                            
                   {
                     PPOPSTACK(S_e);
                     DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                   }                        /*tern Refcount zu hoch;   */
                pm_gen(y);
                pm_gen(IN_PM_DOLL);
                pm_tab[pm_tab_i] = index;     /* Pos. fuer Param. merken  */
                pm_gen(i - pm_tab_i++ +1);    /* Zahl der bisher gefunden-
                                                 nen Listenelemente ohne
                                                 ... ;                    */
                if (i <= pm_arity -2)
                           /* den Fall, dass ein Dollar an letzter Stelle */
                           /* steht, ausschliessen                        */
                {
                    pm_pattern();
                    pm_gen(PM_CUTT);
                    i++;                      /* Zahl der Aufrufe von
                                                 pm_pattern berichtigen   */
                }
            }
            else
            if ((T_STRING(x)) && 
               ( (T_POINTER(pm_h1)) &&
                 ( R_DESC((*(PTR_DESCRIPTOR)pm_h1),type) == TY_NAME ) ) )
            {
                pm_gen(L_LOAD_A);
                pm_gen(INCR);
                pm_gen(L_LOAD_B);
            }
            else
            if ((T_STRING(x)) && (T_PM_AS(pm_h1)) && (T_PM_SKIP(pm_h2)))
            {
                pm_gen(L_LOAD_A);
                PPOPSTACK(S_e);           /* der  .       */
                pm_gen(INCR);
                pm_gen(L_LOAD_B);
                PPOPSTACK(S_e);           /* die Variable */
            }
            else
            {
                PUSHSTACK(S_e, pm_h1);
                pm_pattern ();
            }
        }     /* Ende der for-Schleife */

        --pm_tab_i;                       /* beim letzten Eintrag
                                             wurde pm_tab_i um eins
                                             erhoeht */
        if (pm_tab_i >  0)                /* d.h. in der Tabelle steht
                                             nicht nur die Stelle
                                             an der A_TESTN eingetragen
                                             wurde also enthaelt die
                                             Liste ... */
        {
            /*---- Fuer das lezte SKSKIP ist kein Backtracking   ----*/
            /*-    notwendig, deshalb Unterscheidung von anderen    -*/
            /*---- SKSKIP durch den Befehl LA_SKSKIP             ----*/

            if (code[pm_tab [pm_tab_i] -1] == SKIPSKIP)
                code[pm_tab[ pm_tab_i ] - 1] = LA_SKIPSKIP;
            else
            if (code[pm_tab [pm_tab_i] -1] == IN_SKIPSKIP)
                code[pm_tab[ pm_tab_i ] - 1] = LA_IN_SKIPSKIP;

                /*---- Berechnung der Stelligkeit, die die zu           ----*/
                /*-    matchende Liste an dieser Stelle noch haben         -*/
                /*-    muss: Zahl der Listenelemente - Zahl der            -*/
                /*-          gefundenen SKSKIPS bis zu dieser Stelle       -*/
                /*----       - Zahl der schon gematchten Listenelemente ----*/
             else
                 if ((code[pm_tab [pm_tab_i] -1] == PM_DOLL)   ||
                     (code[pm_tab [pm_tab_i] -1] == IN_PM_DOLL)  )
                     /*---- an letzter Stelle der ... und $ steht ein Dollar ----*/
                     pm_dollar_flag = TRUE;
             for ( i =  pm_tab_i; i > 0; i--)
             {
                  code[pm_tab[i ]] =
                  pm_arity - pm_tab_i   - code[pm_tab[i  ]];
             }
             code[pm_tab[ 0 ] -1] = TESTV;            /* TESTV statt TESTN */
             code[pm_tab[ 0]] =  pm_arity - pm_tab_i + pm_pluscount; /* Mindestarity
                                                                            statt arity
                                                                            angeben */
         }

         if (pm_dollar_flag)
             pm_gen(END_DOLL);

         if (del_blocks > 0)
         {
             pm_gen(DEL_BLOCKS);
             pm_gen(del_blocks);
         }
         pm_gen (RETURN);

                 /*offset fuer den Code des Matching der Listenelemente als zweiten   */
                /*Parameter hinter TESTV oder TESTN schreiben. Inhalt von pm_tab[0]+1*/
                 /*ist zunaechst der Index . Nach Erzeugung des Codes fuer die Liste  */
                 /*erfolgt dann die Berechnung des offsets . + 2 weil pm_pc noch auf  */
                 /*TESTN zeigt wenn L_UNDECIDED aufgerufen wird                       */    
                 code[pm_tab[ 0 ] +1] =  index - code[pm_tab[0] + 1] + 2;

         freeheap(pm_tab);                /* Freigabe der Tabelle der ... */
     }
     else
   /*------------------*/
     if (T_BOOLEAN (x))
   /*------------------*/
     {
         pm_gen (BOOL_TEST);
         pm_gen (x);
     }
     else
   /*----------------*/
     if (T_PM_AS (x))
   /*----------------*/
     {
         x = POPSTACK(S_e);
         switch (x & ~F_EDIT)
         {
         case BOOL:
             pm_gen (TY_TEST);
             pm_gen(TY_BOOL);
             PPOPSTACK(S_e);   /* die Variabale des as */
             pm_gen(LOAD);
             break;
         case INTEGER:
             pm_gen (TY_TEST);
             pm_gen(TY_INTEGER);
             PPOPSTACK(S_e);   /* die Variabale des as*/
             pm_gen(LOAD);
             break;
         case REAL:
             pm_gen (TY_TEST);
             pm_gen(TY_REAL);
             PPOPSTACK(S_e);   /* die Variabale des as*/
             pm_gen(LOAD);
             break;
         case DECIMAL:
             pm_gen (TY_TEST);
             pm_gen(TY_DIGIT);
             PPOPSTACK(S_e);   /* die Variabale des as*/
             pm_gen(LOAD);
             break;
         case CHAR:
             pm_gen (TY_TEST);
             pm_gen(TY_STRING);
             PPOPSTACK(S_e);   /* die Variabale des as*/
             pm_gen(LOAD);
             break;
         default:             /* an der Stelle steht hoffentlich
                                 ein Pattern */
             PUSHSTACK(S_e, x);
             pm_pattern ();
             PPOPSTACK(S_e);   /* die Variable */
             pm_gen (AS_LOAD);
         }
     }
     else
   /*-----------------*/
     if (T_PM_LIST(x))
   /*-----------------*/
     {
         pm_gen(LOAD_ARITY);
         PPOPSTACK(S_e);
         pm_gen(LOAD);
         PPOPSTACK(S_e);
     }
     else
   /*-----------------*/
     if (T_ATOM(x))
   /*-----------------*/
     {
         pm_gen(ATOM_TEST);
         pm_gen(x);
     }

     else
     {
         printf("Stackelement in Hexdarstellung %8.8x \n", x);
         post_mortem ("pm_pattern: unzulaessiger Ausdruck im Pattern");
     }

     END_MODUL ("pm_pattern");

     return;
}


/******************************************************************************/
/* pm_Apatter() arbeitet auf dem globalen Array code und generiert Code fuer  */
/* ein Pattern. Das Pattern liegt auf dem E-Stack und wird zur Codegenerierung*/
/* traversiert. Eventuell  eingelagerte Listen werden dabei wieder auf den    */
/* E_Stack eingelagert. Anschliessend erfolgt rekursiver Aufruf von pattern   */
/******************************************************************************/



void pm_Apatter ()

{

    STACKELEM x,
              y;                          /* y fuer In-Konstrukt benoetigt  */

    int     i,                         /*   Laufvariable fuer for Schleife    */
            del_blocks = 0;            /* gibt die Anzahl der BS-frames an,
                                          die durch "in ... ' ? `" angelegt
                                          wurden. Diese muessen bei Verlassen
                                          der Liste wieder abgebaut werden.   */

    STACKELEM pm_h1,                   /*   Hilfsvariablen fuer as ... x      */
              pm_h2,                   /*      und as in ... 'abc` x          */
              pm_h3;

    int pm_arity;                      /* Stelligkeit eines Listenkonstruktors*/
    int pm_tab_i;                      /* Index fuer pm_tab                   */
    int pm_pluscount = 0;              /* Anzahl der PLUS-Operatoren          */
    T_HEAPELEM *pm_tab;                /* Tabelle die Stellen des Codevektors
                                          aufnimmt an denen nach Durchlauf
                                          aller Listeneintraege nachtraeglich
                                          Eintragungen gemacht werden muessen */
    BOOLEAN pm_dollar_flag = FALSE;

    START_MODUL ("pm_Apatter");


    x = POPSTACK (S_e);

  /*-----------------*/
    if (T_POINTER (x))
  /*-----------------*/
    {
      if ((R_DESC ((*(PTR_DESCRIPTOR) x), type)) == TY_NAME) 
         {
          pm_gen(A_LOAD);
          DEC_REFCNT((PTR_DESCRIPTOR)x);
          /* da x vom E-Stack entfernt wurde */ 
         } 
      else
        switch (R_DESC ((*(PTR_DESCRIPTOR) x), class))
        {
        /* ---- strings nun mit class = C_LIST  (sj) ---- */
        /* ---------------------------------------------- */

        case C_SCALAR:         /* eine Zahl, fixformat */

            pm_gen (A_SCAL_TEST);

            pm_gen (x);        /* den descr. in das Instr. Array */
            break;

        case C_DIGIT:          /* eine Zahl, varformat */

            pm_gen (A_DIG_TEST);

            pm_gen (x);        /* den descr. in das Instr. Array */
            break;

        case C_MATRIX:         /* eine Matrix */

            pm_gen (A_MATRIX_TEST);

            pm_gen (x);        /* den descr. in das Instr. Array */
            break;

        case C_VECTOR:         /* ein VECTOR */

            pm_gen (A_VECTOR_TEST);

            pm_gen (x);        /* den descr. in das Instr. Array */
            break;

        case C_TVECTOR:        /* ein TVECTOR */

            pm_gen (A_TVECTOR_TEST);

            pm_gen (x);        /* den descr. in das Instr. Array */
            break;

        case C_LIST:
            if ((pm_arity = R_LIST((*(PTR_DESCRIPTOR) x), dim)) == 0)
            {
                if ((R_DESC((*(PTR_DESCRIPTOR) x), type)) == TY_STRING)
                    pm_gen(A_NILSTRING_TEST);
                else
                    pm_gen(A_NIL_TEST);
                break;
            }
            if (( pm_tab = (T_HEAPELEM *)newbuff( pm_arity + 1)) == 0)
                post_mortem("pmcomp: Heap out of space ");
                /*---- pm_tab braucht wegen < ...> mindestens einen Eintrag --*/
                /*---- fuer < und einen zweiten fuer ... --> pm_arity+1     --*/

            if (R_DESC((*(PTR_DESCRIPTOR) x), type) == TY_STRING)
                pm_gen(A_STR_CHECK);            /* Unterscheidung von String- */
            else
                pm_gen(A_LI_CHECK);             /* und Listen-Konstruktoren   */

             pm_gen (A_TESTN);

             pm_tab_i = 0;                 /* Initialisierung
                                              des Zaehlers fuer
                                              pm_tab */
             pm_tab[ pm_tab_i] = index;    /* Merken der Stelle
                                              an der spaeter A_TESTN
                                              und Parameter fuer
                                              die Mindeststelligkeit
                                              eingetragen werden muss */
             ++ pm_tab_i;
             pm_gen (pm_arity );

            /* zusaetlicher Parameter, der einen offset fuer*/
            /* die Zahl der generierten Befehle  der Liste  */
            /* angibt; wird beim Backpatching eingetragen   */
             pm_gen( index);

             /*---- refcnt erhoehen, da ret_list refcnt dekrementiert ----*/
             INC_REFCNT((PTR_DESCRIPTOR) x);

             ret_list (x);             /* Liste auf E-Stack     */

             PPOPSTACK (S_e);          /* Pop fuer den Listenkonstruktor */

      /*---- Listenkonstruktor wird abgearbeitet ----*/

             for (i = 0 ; i < pm_arity; i++)
             {
                 if ( T_PM_SKSKIP( READSTACK(S_e)))
                 {
                     pm_gen(SKIPSKIP);
                     pm_tab[pm_tab_i] = index;
                     pm_gen(i - pm_tab_i +1);  /* Zahl
                                                  der bisher gefundenen
                                                  Listenelemente  ohne
                                                  ... */
                     pm_tab_i++;
                     PPOPSTACK(S_e);
                 }
                 else
                 if ( T_PM_DOLLAR( READSTACK(S_e)))
                 {
                     pm_gen(GEN_BLOCK);
                     pm_gen(PM_DOLL);
                     pm_tab[pm_tab_i] = index;
                     pm_gen(i - pm_tab_i +1);  /* Zahl
                                                  der bisher gefundenen
                                                  Listenelemente  ohne
                                                  ... */
                     pm_tab_i++;
                     PPOPSTACK(S_e);
                     if (i <= pm_arity -2)
                     /*---- den Fall, dass ein Dollar an letzter Stelle ---*/
                     /*---- steht ausschliessen                         ---*/
                     {
                         pm_pattern();
                         pm_gen(PM_CUTT);
                         i++;          /* Zahl der Aufrufe von pm_pattern
                                          berichtigen */
                     }
                 }
                 else
                 {
                     pm_pattern ();
                 }
             }  /* Ende der for-Schleife */

             --pm_tab_i;               /* beim letzten Eintrag
                                          wurde pm_tab_i um eins
                                          erhoeht */
             if (pm_tab_i >  0)        /* d.h. in der Tabelle steht
                                          nicht nur die Stelle
                                          an der A_TESTN eingetragen
                                          wurde also enthaelt die
                                          Liste ... */
             {
             /*---- Fuer das lezte SKSKIP ist kein Backtracking ----*/
             /*-    notwendig, deshalb Unterscheidung von anderen  -*/
             /*---- SkSKIP durch den Befehl LA_SKSKIP           ----*/
                 if (code[pm_tab [pm_tab_i] -1] == SKIPSKIP)
                     code[pm_tab[ pm_tab_i ] - 1] = LA_SKIPSKIP;
                 else
                 if (code[pm_tab [pm_tab_i] -1] == IN_SKIPSKIP)
                     code[pm_tab[ pm_tab_i ] - 1] = LA_IN_SKIPSKIP;

                 /*---- Berechnung der Stelligkeit, die die zu      ----*/
                 /*-  matchende Liste an dieser Stelle noch haben      -*/
                 /*-  muss: Zahl der Listenelemente - Zahl der         -*/
                 /*-        gefundenen SKSKIPS bis zu dieser Stelle    -*/
                 /*----     - Zahl der schon gematchten Listenelemente -*/

                 else
                 if ( (code[pm_tab [pm_tab_i] -1] == PM_DOLL)  ||
                      (code[pm_tab [pm_tab_i] -1] == IN_PM_DOLL) )
                 /*---- an letzter Stelle der ... und $ steht ein Dollar --*/
                     pm_dollar_flag = TRUE;
                 for ( i =  pm_tab_i; i > 0; i--)
                 {

                      code[pm_tab[i ]] =
                           pm_arity - pm_tab_i   - code[pm_tab[i  ]];

                 }
                 code[pm_tab[ 0 ] -1] = A_TESTV;  /* A_TESTV statt A_TESTN */
                 code[pm_tab[ 0]] =  pm_arity - pm_tab_i; /* Mindestarity
                                                             statt arity
                                                             angeben */
             }
             if (pm_dollar_flag)
                 pm_gen(END_DOLL);

             if (del_blocks > 0)
             {
                 pm_gen(DEL_BLOCKS);
                 pm_gen(del_blocks);
             }
                 /*offset fuer den Code des Matching der Listenelemente als zweiten*/
                 /*Parameter hinter TESTV oder TESTN schreiben. Inhalt von pm_tab[0]+1*/
                 /*ist zunaechst der Index . Nach Erzeugung des Codes fuer die Liste*/
                 /*erfolgt dann die Berechnung des offsets  , + 2 weil pc auf TESTN  */
                 /*zeigt, falls L_UNDECIDED aufgerufen wird                          */    
                 code[pm_tab[ 0 ] +1] =  index - code[pm_tab[0] + 1] + 2;

                 freeheap(pm_tab); /* Freigabe der Tabelle der ... */

             break;

         default:
             printf("Stackelement in Hexdarstellung %8.8x \n", x);
             post_mortem ("pm_Apatter: Unzulaessiger Ausdruck im Pattern");

         }                                /* ende des Switch */

     }                                    /* ende von T_POINTER(x) */

     else
   /*--------------*/
     if ( (T_POINTER(x)) &&
        ( R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_NAME ) ) 
     /* eine Patternvariable   */
   /*--------------*/
         pm_gen (A_LOAD);

     else
   /*------------------*/
     if (T_PM_SKIP (x))                   /* ein Skip, dann nichts machen */
   /*------------------*/
         pm_gen(INCR);

     else
   /*---------------------------------*/
     if ( (T_LIST(x))||(T_STRING(x)) )     /* tritt auf, wenn Liste (String) */
   /*---------------------------------*/   /* als expression abgespeichert   */
     {                                     /* wurde;                         */
         pm_arity = ARITY(x);

         if (( pm_tab = (T_HEAPELEM *)newbuff( pm_arity + 1)) == 0)
             post_mortem("pmcomp: Heap out of space ");
             /*---- pm_tab braucht wegen < ...> mindestens einen Eintrag ----*/
             /*---- fuer < und einen zweiten fuer ... --> pm_arity+1     ----*/

         if (T_STRING(x))
             pm_gen(A_STR_CHECK);         /* Unterscheidung von String- */
         else
             pm_gen(A_LI_CHECK);          /* und Listen-Konstruktoren   */

         pm_gen (A_TESTN);

         pm_tab_i = 0;                    /* Initialisierung
                                             des Zaehlers fuer
                                             pm_tab */
         pm_tab[ pm_tab_i] = index;       /* Merken der Stelle
                                             an der spaeter A_TESTN
                                             und Parameter fuer
                                             die Mindeststelligkeit
                                             eingetragen werden muss */
         ++ pm_tab_i;
         pm_gen (pm_arity );

        /* zusaetlicher Parameter, der einen offset fuer*/
        /* die Zahl der generierten Befehle  der Liste  */
        /* angibt; wird beim Backpatching eingetragen   */
         pm_gen( index);


  /*---- Listenkonstruktor wird abgearbeitet ----*/

         for (i = 0 ; i < pm_arity; i++)
         {
             pm_h1 = POPSTACK(S_e);
             pm_h2 = READSTACK(S_e);
             if ( T_PM_SKSKIP( pm_h1))
             {
                 pm_gen(SKIPSKIP);
                 pm_tab[pm_tab_i] = index;
                 pm_gen(i - pm_tab_i +1);  /* Zahl der bisher gefundenen
                                              Listenelemente  ohne ... */
                 pm_tab_i++;
             }
             else
             if ((T_PM_AS( pm_h1)) && (T_PM_SKSKIP(pm_h2)))
             /*---- ein as gefolgt von ... auf dem E-stack ----*/
             {
                 pm_gen(L_LOAD_A);
                 PPOPSTACK(S_e);           /* das pm_hs = ... */
                 pm_gen(SKIPSKIP);
                 pm_tab[pm_tab_i] = index;
                 pm_gen(i - pm_tab_i + 1);
                 pm_tab_i++;
                 pm_gen(L_LOAD_B);
                 PPOPSTACK(S_e);           /* die Variable */
             }
             else
             if ( T_PM_DOLLAR( pm_h1))
             {
                 pm_gen(GEN_BLOCK);
                 pm_gen(PM_DOLL);
                 pm_tab[pm_tab_i] = index;
                 pm_gen(i - pm_tab_i +1);  /* Zahl der bisher gefundenen
                                              Listenelemente  ohne ... */
                 pm_tab_i++;
                 if (i <= pm_arity -2)
                 /*---- den Fall, dass ein Dollar an letzter Stelle ----*/
                 /*---- steht ausschliessen                         ----*/
                 {
                     pm_pattern();
                     pm_gen(PM_CUTT);
                     i++;                 /* Zahl der Aufrufe von pm_pattern
                                             berichtigen */
                 }
             }
             else
             if ((T_PM_AS( pm_h1)) && (T_PM_DOLLAR(pm_h2)))
             /*---- ein as gefolgt von einem $ auf dem E-stack ----*/
             {
                 pm_gen(GEN_BLOCK);
                 pm_gen(L_LOAD_A);
                 PPOPSTACK(S_e);           /* das pm_hs = ... */
                 pm_gen(PM_DOLL);
                 pm_tab[pm_tab_i] = index;
                 pm_gen(i - pm_tab_i + 1);
                 pm_tab_i++;
                 pm_gen(L_LOAD_B);
                 PPOPSTACK(S_e);           /* die Variable */
                 if (i <= pm_arity -2)
                 /*---- den Fall, dass ein Dollar an letzter Stelle ----*/
                 /*---- steht, ausschliessen;                       ----*/
                 {
                     pm_pattern();
                     pm_gen(PM_CUTT);
                     i++;                 /* Zahl der Aufrufe von pm_pattern
                                             berichtigen */
                 }
             }
             else
             if ((T_PM_AS( pm_h1)) && (T_PM_IN(pm_h2)))
             /*---- ein as gefolgt von einem in auf dem E-stack ----*/
             {
                 PPOPSTACK(S_e);             /* um naechstes Stackelement
                                                inspizieren zu koennen    */
                 pm_h3 = READSTACK(S_e);

                 if (T_PM_SKIP(pm_h3))
                 /*---- ein . auf dem E-stack ----*/
                 {
                     pm_gen(L_LOAD_A);

                     pm_gen(IN_TEST1);
                     PPOPSTACK(S_e);          /*        SKIP poppen       */
                     y = READSTACK(S_e);  
                     if (T_STRING(y))         /* string auslagern */
                        {
                         TRAV_E_A;          
                         if (check_list_for_store())  /* darf String ausgelagert werden? */
                            {
                             if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                 post_mortem("pm_pattern: heap out of space in st_list");
                            }
                        }  
                     else                            
                        {
                          PPOPSTACK(S_e);
                          DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                        }                        /*tern Refcount zu hoch;   */
                     pm_gen(y);

                     pm_gen(L_LOAD_B);
                     PPOPSTACK(S_e);           /*       die Variable       */
                 }
                 else
                 if ( (T_PM_SKSKIP(pm_h3)) || (T_PM_PT_PLUS(pm_h3)) )
                 /*---- ... oder .+ auf dem E-stack ----*/
                 {
                     pm_gen(L_LOAD_A);

                     pm_gen(GEN_BLOCK);
                     if (T_PM_PT_PLUS(pm_h3))
                     {
                         pm_gen(INCR);
                         pm_pluscount++;      /* Anz. PLUS-Op. festhalten */
                     }
                     else
                         pm_gen(JMP2);
                     pm_gen(IN_TEST);
                     PPOPSTACK(S_e);          /*      SKIPSKIP poppen     */
                     y = READSTACK(S_e);  
                     if (T_STRING(y))         /* string auslagern */
                        {
                         TRAV_E_A;          
                         if (check_list_for_store())  /* darf String ausgelagert werden? */
                            {
                             if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                 post_mortem("pm_pattern: heap out of space in st_list");
                            }
                        }  
                     else                            
                        {
                          PPOPSTACK(S_e);
                          DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                        }                        /*tern Refcount zu hoch;   */
                     pm_gen(y);
                     pm_gen(IN_SKIPSKIP);
                     pm_tab[pm_tab_i] = index;/* Pos. fuer Param. merken  */
                     pm_gen(i - pm_tab_i++ +1);/*Zahl der bisher gefunden-
                                                 nen Listenelemente ohne
                                                 ... ;                    */
                     del_blocks++;            /*  ein Block mehr abbauen  */

                     pm_gen(L_LOAD_B);
                     PPOPSTACK(S_e);           /*       die Variable       */
                 }
                 else
                 if (T_PM_DOLLAR(pm_h3))
                 /*---- ein $ auf dem E-stack ----*/
                 {
                     pm_gen(GEN_BLOCK);
                     pm_gen(L_LOAD_A);
                     pm_gen(JMP2);
                     pm_gen(IN_TEST);
                     PPOPSTACK(S_e);          /*       DOLLAR  poppen     */
                     y = READSTACK(S_e);  
                     if (T_STRING(y))         /* string auslagern */
                        {
                         TRAV_E_A;          
                         if (check_list_for_store())  /* darf String ausgelagert werden? */
                            {
                             if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                                 post_mortem("pm_pattern: heap out of space in st_list");
                            }
                        }  
                     else                            
                        {
                          PPOPSTACK(S_e);
                          DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                        }                        /*tern Refcount zu hoch;   */
                     pm_gen(y);
                     pm_gen(IN_PM_DOLL);
                     pm_tab[pm_tab_i] = index;
                     pm_gen(i - pm_tab_i + 1);
                     pm_tab_i++;
                     pm_gen(L_LOAD_B);
                     PPOPSTACK(S_e);           /*       die Variable       */
                     if (i <= pm_arity -2)
                     /*---- den Fall, dass ein Dollar an letzter Stelle ---*/
                     /*---- steht, ausschliessen                        ---*/
                     {
                         pm_pattern();
                         pm_gen(PM_CUTT);
                         i++;                 /* Zahl der Aufrufe von
                                                 pm_pattern berichtigen;  */
                     }
                 }
             }  /* Ende der "as in"-Kombinationen */
             else
             if ((T_PM_IN( pm_h1)) && (T_PM_SKIP(pm_h2)))
             /*---- ein in gefolgt von einem . auf dem A-Stack ----*/
             {
                 pm_gen(IN_TEST1);
                 PPOPSTACK(S_e);               /*        SKIP poppen       */
                 y = READSTACK(S_e);  
                 if (T_STRING(y))         /* string auslagern */
                    {
                     TRAV_E_A;          
                     if (check_list_for_store())  /* darf String ausgelagert werden? */
                        {
                         if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                             post_mortem("pm_pattern: heap out of space in st_list");
                        }
                    }  
                 else                            
                    {
                      PPOPSTACK(S_e);
                      DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                    }                        /*tern Refcount zu hoch;   */
                 pm_gen(y);
             }
             else
             if ( (T_PM_IN( pm_h1)) &&
                  ((T_PM_SKSKIP(pm_h2)) || (T_PM_PT_PLUS(pm_h2))) )
             /*---- ein in gefolgt von ... oder .+ auf dem A-Stack ----*/
             {
                 pm_gen(GEN_BLOCK);
                 if (T_PM_PT_PLUS(pm_h2))
                 {
                     pm_gen(INCR);
                     pm_pluscount++;           /* Anz. PLUS-Op. festhalten */
                 }
                 else
                     pm_gen(JMP2);                 pm_gen(IN_TEST);
                 PPOPSTACK(S_e);               /*      SKIPSKIP poppen     */
                 y = READSTACK(S_e);  
                 if (T_STRING(y))         /* string auslagern */
                    {
                      TRAV_E_A;          
                      if (check_list_for_store())  /* darf String ausgelagert werden? */
                         {
                           if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                              post_mortem("pm_pattern: heap out of space in st_list");
                         }
                     }  
                 else                            
                    {
                      PPOPSTACK(S_e);
                      DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                    }                        /*tern Refcount zu hoch;   */
                 pm_gen(y);
                 pm_gen(IN_SKIPSKIP);
                 pm_tab[pm_tab_i] = index;     /* Pos. fuer Param. merken  */
                 pm_gen(i - pm_tab_i++ +1);    /* Zahl der bisher gefunden-
                                                  nen Listenelemente ohne
                                                  ... ;                    */
                 del_blocks++;                 /*  ein Block mehr abbauen  */
             }
             else
             if ((T_PM_IN( pm_h1)) && (T_PM_DOLLAR(pm_h2)))
             /*---- ein in gefolgt von einem $ auf dem A-Stack ----*/
             {
                 pm_gen(GEN_BLOCK);
                 pm_gen(JMP2);
                 pm_gen(IN_TEST);
                 PPOPSTACK(S_e);               /*      SKIPSKIP poppen     */
                 y = READSTACK(S_e);  
                 if (T_STRING(y))         /* string auslagern */
                    {
                     TRAV_E_A;          
                     if (check_list_for_store())  /* darf String ausgelagert werden? */
                        {
                         if ((y = st_list() ) == 0)   /* Wenn ja, auslagern */
                             post_mortem("pm_pattern: heap out of space in st_list");
                        }
                     }  
                 else                            
                    {
                      PPOPSTACK(S_e);
                      DEC_REFCNT((T_PTD) y); /* durch Einlagern des Pat-*/
                    }                        /*tern Refcount zu hoch;   */
                 pm_gen(y);
                 pm_gen(IN_PM_DOLL);
                 pm_tab[pm_tab_i] = index;     /* Pos. fuer Param. merken  */
                 pm_gen(i - pm_tab_i++ +1);    /* Zahl der bisher gefunden-
                                                  nen Listenelemente ohne
                                                  ... ;                    */
                 if (i <= pm_arity -2)
                            /* den Fall, dass ein Dollar an letzter Stelle */
                            /* steht, ausschliessen                        */
                 {
                     pm_pattern();
                     pm_gen(PM_CUTT);
                     i++;                      /* Zahl der Aufrufe von
                                                  pm_pattern berichtigen   */
                 }
             }
             else
             if ((T_STRING(x)) &&
                 ( (T_POINTER(pm_h1)) &&
                   ( R_DESC((*(PTR_DESCRIPTOR)pm_h1),type) == TY_NAME ) ) )
             {
                 pm_gen(L_LOAD_A);
                 pm_gen(INCR);
                 pm_gen(L_LOAD_B);
             }
             else
             if ((T_STRING(x)) && (T_PM_AS(pm_h1)) && (T_PM_SKIP(pm_h2)))
             {
                 pm_gen(L_LOAD_A);
                 PPOPSTACK(S_e);           /* der  .       */
                 pm_gen(INCR);
                 pm_gen(L_LOAD_B);
                 PPOPSTACK(S_e);           /* die Variable */
             }
             else
             {
                 PUSHSTACK(S_e, pm_h1);
                 pm_pattern ();
             }
         }     /* Ende der for-Schleife */

         --pm_tab_i;                     /* beim letzten Eintrag
                                            wurde pm_tab_i um eins
                                            erhoeht */
         if (pm_tab_i >  0)              /* d.h. in der Tabelle steht
                                            nicht nur die Stelle
                                            an der A_TESTN eingetragen
                                            wurde also enthaelt die
                                            Liste ... */
         {
         /*---- Fuer das lezte SKSKIP ist kein Backtracking ----*/
         /*-    notwendig, deshalb Unterscheidung von anderen  -*/
         /*---- SkSKIP durch den Befehl LA_SKSKIP           ----*/
             if (code[pm_tab [pm_tab_i] - 1] == SKIPSKIP)
                 code[pm_tab[ pm_tab_i ] - 1] = LA_SKIPSKIP;
             else
             if (code[pm_tab [pm_tab_i] -1] == IN_SKIPSKIP)
                 code[pm_tab[ pm_tab_i ] - 1] = LA_IN_SKIPSKIP;

             /*---- Berechnung der Stelligkeit, die die zu        ----*/
             /*-    matchende Liste an dieser Stelle noch haben      -*/
             /*-    muss: Zahl der Listenelemente - Zahl der         -*/
             /*-          gefundenen SKSKIPS bis zu dieser Stelle    -*/
             /*----       - Zahl der schon gematchten Listenelemente -*/

             else
                 if ( (code[pm_tab [pm_tab_i] -1] == PM_DOLL)  ||
                      (code[pm_tab [pm_tab_i] -1] == IN_PM_DOLL) )
                 /*---- an letzter Stelle der ... und $ steht ein Dollar ----*/
                     pm_dollar_flag = TRUE;

             for ( i =  pm_tab_i; i > 0; i--)
             {
                 code[pm_tab[i ]] =
                        pm_arity - pm_tab_i   - code[pm_tab[i  ]];
             }

             code[pm_tab[ 0 ] -1] = A_TESTV;     /* A_TESTV statt A_TESTN */
             code[pm_tab[ 0]] =  pm_arity - pm_tab_i + pm_pluscount; /* Mindestarity
                                                                            statt arity
                                                                            angeben */
         }
         if (pm_dollar_flag)
             pm_gen(END_DOLL);

         if (del_blocks > 0)
         {
             pm_gen(DEL_BLOCKS);
             pm_gen(del_blocks);
         }
                 /*offset fuer den Code des Matching der Listenelemente als zweiten*/
                 /*Parameter hinter TESTV oder TESTN schreiben. Inhalt von pm_tab[0]+1*/
                 /*ist zunaechst der Index . Nach Erzeugung des Codes fuer die Liste*/
                 /*erfolgt dann die Berechnung des offsets  , + 2 weil pc auf TESTN  */
                 /*zeigt, falls L_UNDECIDED aufgerufen wird                          */    
                 code[pm_tab[ 0 ] +1] =  index - code[pm_tab[0] + 1] + 2;

                 freeheap(pm_tab); /* Freigabe der Tabelle der ... */
     }    /* Ende von 'if T_LIST' */
     else
   /*------------------*/
     if (T_BOOLEAN (x))
   /*------------------*/
     {
         pm_gen (A_BOOL_TEST);
         pm_gen (x);
     }
     else
   /*------------------*/
     if (T_PM_AS (x))
   /*------------------*/
     {
         x = POPSTACK(S_e);
         switch (x & ~F_EDIT)
         {
         case BOOL:
             pm_gen (A_TY_TEST);
             pm_gen(TY_BOOL);
             PPOPSTACK(S_e);               /* die Variabale des as*/
             pm_gen(A_LOAD);
             break;
         case INTEGER:
             pm_gen (A_TY_TEST);
             pm_gen(TY_INTEGER);
             PPOPSTACK(S_e);               /* die Variable des as */
             pm_gen(A_LOAD);
             break;
         case REAL:
             pm_gen (A_TY_TEST);
             pm_gen(TY_REAL);
             PPOPSTACK(S_e);               /* die Variable des as */
             pm_gen(A_LOAD);
             break;
         case DECIMAL:
             pm_gen (A_TY_TEST);
             pm_gen(TY_DIGIT);
             PPOPSTACK(S_e);               /* die Variable des as */
             pm_gen(A_LOAD);
             break;
         case CHAR:
             pm_gen (A_TY_TEST);
             pm_gen(TY_STRING);
             PPOPSTACK(S_e);               /* die Variable des as */
             pm_gen(A_LOAD);
             break;

         default:                         /* an der Stelle steht hoffentlich
                                             ein Pattern */
             PUSHSTACK(S_e,x);
             pm_Apatter ();
             PPOPSTACK(S_e);               /* die Variable */
             pm_gen (A_LOAD);
         }
     }
     else
   /*-----------------*/
     if (T_PM_LIST(x))
   /*-----------------*/
     {
         pm_gen(A_LOAD_ARITY);
         PPOPSTACK(S_e);
         pm_gen(A_LOAD);
         PPOPSTACK(S_e);
     }
     else
   /*-----------------*/
     if (T_ATOM(x))
   /*-----------------*/
     {
         pm_gen(A_ATOM_TEST);
         pm_gen(x);
     }
     else
     {
         printf("Stackelement in Hexdarstellung %8.8x \n", x);
         post_mortem ("pm_Apatter: unzulaessiger Ausdruck im Pattern");
     }


    END_MODUL ("pm_Apatter");

    return;
}

/*****************************************************************************/
/*                                                                           */
/*   pm_ptfu  steuert die Uebersetzung der Pattern, traegt als Ergebnis      */
/*   einen Zeiger auf den Anfang eines Codearrays in den Deskriptor ein      */
/*****************************************************************************/


void pm_ptfu (desc,anz)	
     /* pm_ptfu traegt Zeiger auf Instruction- */
     /* array in Parameter desc ein            */
PTR_DESCRIPTOR desc;  
int anz;  /* anz = Stelligkeit der case- bzw. when-Funktion */

{

    int     i;		/* Laufvariable in for-Schleifen   */

    register STACKELEM  x;

    STACKELEM  org_pat;
    PTR_HEAPELEM bcode;

    START_MODUL ("pm_ptfu");



    index = OFFSETS;	

    /* Bestimmung der Patternvariablen , dabei werden die Slice-   */
    /* variablen auf den I-Stack und die anderen Variablen auf den */
    /* Pm-Stack gelegt:                                            */
    pm_patvar();

    /* Auslagern des Originalpatterns und eintragen */
    /* in den code-Vektor:                          */

    if ((org_pat = st_expr()) != 0)
       /* Pattern wieder auf E: */
       load_expr((STACKELEM **)A_EXPR((*(PTR_DESCRIPTOR)org_pat),pte));
    else
        post_mortem("pm_ptfu: Pattern kann nicht augelagert werden");


    pm_Apatter ();  /* Einstieg in die Analyse des Pattern */

    pm_gen (EOT);



    nskipvar = SIZEOFSTACK(S_i);
         /* = Anzahl der Slice-Variablen */
 
    nvar = SIZEOFSTACK(S_pm);
         /* = Anzahl der anderen Variablen */

    --index;			/* wg. pm_gen ist Index jeweils eins zu gross */

    /* Platz fuer den code-Vektor bereitstellen: in den Vektor wird folgendes  */
    /* eingetragen: an Stelle 0 wird Stelligkeit der Funktion, an Stelle 1 die */
    /* Laenge des Vektors, an 2 der Offset fuer den Zugriff auf die  namelist, */
    /* an 3 das pattern und an 4 die Laenge des code. Danach wird der code     */
    /* ( index - 3 Elemente ) eingetragen, dann die Anzahl der Variablen und   */
    /* zum Schluss Verweise auf die Variablen ( Anzahl = nvar + nskipvar )     */

/*    if (newheap (( OFFSETS + ( index - ( OFFSETS - 1 ) ) + ( nskipvar + nvar + 1 )),A_MATCH((*desc),code)) == 0)*/
    if (newheap ( 1 + index + nskipvar + nvar + 1, A_MATCH((*desc),code)) == 0)
        post_mortem ("pm_ptfu: Heap out of space");


    RES_HEAP;
    bcode = R_MATCH((*desc),code);

                          
    /* Eintragen der Slicevariablen in die "namelist" */
    for (i = nskipvar ; i > 0; i--)
    {
/*    bcode[OFFSETS + (index - (OFFSETS -1)) + 1 + ( nskipvar - i )] = POPSTACK(S_i);*/
	     bcode[ index + 1 + ( nskipvar - i + 1 )] = POPSTACK(S_i);
    }
    /* Eintragen der anderen Variablen in die namelist */
    for (i = nvar  ; i > 0; i--)
        {                                          
         x=POPSTACK(S_pm);
	 bcode[ index + 1 + nskipvar + ( nvar - i + 1 ) ] = x;
        }

    /* Stelligkeit der pi-Abstraktion: */
    bcode[STELLIGKEIT] = anz;

    /* Groesse des Vektors eintragen */
    bcode[VEKTORLAENGE] = 1 + index + 1 + ( nskipvar + nvar );

    /* Eintrag des geretteten  Patterns in Codevektor */
    bcode[OPAT] = org_pat;            

    /* offset fuer die namelist eintragen */
    bcode[NAMELIST] = index + 1;

    /* Laenge des code eintragen */
    bcode[CODELAENGE] = index - (OFFSETS - 1);

    /* Laenge der namelist eintragen */
    bcode[ index + 1 ] = nskipvar + nvar;

    /* generierten Code in den Descriptor kopieren */

    for ( i = OFFSETS; i <= index; i++ )
    {
      bcode[ i ] = code[ i ];  
    }

    REL_HEAP;
                    
    END_MODUL ("pm_ptfu");

    return;
}
