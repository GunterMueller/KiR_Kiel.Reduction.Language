/* $Log: r-debug.c,v $
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:49:15  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*----------------------------------------------------------*/
/*  pm_debug enthaelt Funktion ,die fuer die Ausgabe der    */
/*           Schritte der abstrakten Maschine sorgt         */
/*                                                          */
/*----------------------------------------------------------*/


/*----------------------------------------------------------*/
/*  include files                                           */
/*                                                          */
/*----------------------------------------------------------*/


#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "rinstr.h"

/*----------------------------------------------------------*/
/* external Definitionen                                    */
/*----------------------------------------------------------*/

#if DEBUG
extern  FILE * pmdmpfile;
extern int  pmdmpopen;
extern int  pmdmpcrs;
extern void  pm_test_monitor();

extern int cursorxy(); /* TB, 4.11.1992 */
extern void monitor_call(); /* TB, 4.11.1992 */ /* rruntime.c */
#endif


/*----------------------------------------------------------*/
/* Typdefinitionen                                          */
/*----------------------------------------------------------*/


typedef int INSTRUCTION;                                /* pmcomp:c
                                                                 */

extern  INSTRUCTION * pm_pc;                                     /* zeigt auf
                                                                    Eintrag im
                                                                    Instr. Array
                                                                 */

extern  PTR_HEAPELEM pm_base;                                    /* enhaelt den
                                                                    Pointer auf
                                                                    das gerade
                                                                    bearbeitete
                                                                    Heapelement
                                                                 */

extern int  pm_offset;                                           /* enthaelt die
                                                                    Nummer des
                                                                    gerade
                                                                    bearbeiteten
                                                                    Listenelement
                                                                    s        */

extern int  pm_match_fail;                                       /* true falls
                                                                    Muster noch
                                                                    passen kann
                                                                    fail falls
                                                                    Match nicht
                                                                    erfolgreich
                                                                 */
extern int  s[];                                        /* Systenstack,
                                                                    speichert
                                                                    pm_base
                                                                    pm_offset
                                                                    bei
                                                                    verschachtelt
                                                                    en   pattern
                                                                        */

extern int  num_s;                                               /* Zahler fuer
                                                                    Systemstack
                                                                 */


extern int bt[];
extern int num_bt;

extern  STACKELEM pm_arg;                               /* register, um
                                                                    Argumente
                                                                    aufzunehmen
                                                                    d.h.
                                                                    Deskriptor
                                                                    oder
                                                                    Konstante,
                                                                    die  gerade
                                                                    ueberprueft
                                                                    werden soll
                                                                 */

/*extern char *pm_translate (); */


/*------------------------------------------------------------------
 * pm_translate
 *------------------------------------------------------------------
 */


static char line[100];

char   *pm_translate (x)
int     x;
{
    switch (x)
    {
        case JMP:
            return (" JMP   ");
        case JMP2:
            return (" JMP2  ");

        case SETBASE:
            return ("  SETBASE   ");

        case GEN_BLOCK:
            return ("GEN_BLOCK   ");

        case A_SETBASE:
            return ("  A_SETBASE   ");

        case INCR:
            return ("  INCR   ");

        case DECR:
            return ("  DECR   ");

        case TESTN:
            return ("  TESTN   ");

        case ATOM_TEST:
            return (" ATOM_TEST  ");

        case A_ATOM_TEST:
            return ("A_ATOM_TEST");

        case STR_TEST:
            return ("  STR_TEST   ");

        case DIG_TEST:
            return ("  DIG_TEST   ");
        case SCAL_TEST:
            return ("  SCAL_TEST   ");

        case MATRIX_TEST:
            return ("  MATRIX_TEST  ");

        case VECTOR_TEST:
            return ("  VECTOR_TEST  ");

        case TVECTOR_TEST:
            return ("  TVECTOR_TEST  ");

        case BOOL_TEST:
            return ("  BOOL_TEST   ");

        case NIL_TEST:
            return ("   NIL_TEST   ");
        case NILM_TEST:
            return ("  NILM_TEST   ");
        case NILV_TEST:
            return ("  NILV_TEST   ");
        case NILSTRING_TEST:
            return ("NILSTRING_TEST");
        case IN_TEST:
            return ("  IN_TEST     ");
        case IN_TEST1:
            return ("  IN_TEST1    ");
        case DEL_BLOCKS:
            return ("  DEL_BLOCKS  ");
        case IN_SKIPSKIP:
            return ("  IN_SKIPSKIP ");
        case LA_IN_SKIPSKIP:
            return ("LA_IN_SKIPSKIP");
        case IN_PM_DOLL:
            return ("  IN_PM_DOLL  ");

        case TY_TEST:
            return ("  TY_TEST   ");

        case A_STR_TEST:
            return ("  A_STR_TEST   ");

        case A_DIG_TEST:
            return ("  A_DIG_TEST   ");

        case A_SCAL_TEST:
            return ("  A_SCAL_TEST   ");

        case A_MATRIX_TEST:
            return ("  A_MATRIX_TEST  ");

        case A_VECTOR_TEST:
            return ("  A_VECTOR_TEST  ");

        case A_TVECTOR_TEST:
            return ("  A_TVECTOR_TEST  ");

        case A_NIL_TEST:
            return ("  A_NIL_TEST   ");

        case A_NILM_TEST:
            return ("  A_NILM_TEST   ");

        case A_NILV_TEST:
            return ("  A_NILV_TEST   ");

        case A_NILTV_TEST:
            return ("  A_NILTV_TEST  ");
        case A_NILSTRING_TEST:
            return ("A_NILSTRING_TEST");

        case A_BOOL_TEST:
            return ("  A_BOOL_TEST   ");

        case A_TY_TEST:
            return ("  A_TY_TEST   ");

        case LOAD:
            return ("  LOAD   ");

        case AS_LOAD:
            return ("  AS_LOAD   ");

        case L_LOAD_A:
            return ("  L_LOAD_A   ");

        case L_LOAD_B:
            return ("  L_LOAD_B   ");

        case LOAD_ARITY:
            return ("  LOAD_ARITY   ");

        case A_TESTN:
            return ("  A_TESTN   ");

        case A_KTEST:
            return ("  A_KTEST   ");

        case A_LOAD:
            return ("  A_LOAD   ");

        case A_LOAD_ARITY:
            return ("  A_LOAD_ARITY   ");
        case SUBR:
            return ("  SUBR   ");

        case RETURN:
            return ("  RETURN   ");

        case END_DOLL:
            return ("  END_DOLL   ");

        case EOT:
            return ("  EOT   ");

        case PM_CUTT:
            return ("  PM_CUTT   ");

        case SKIPSKIP:
            return ("  SKIPSKIP    ");

        case PM_DOLL:
            return ("  PM_DOLL    ");

        case LA_SKIPSKIP:
            return ("  LA_SKIPSKIP   ");

        case TESTV:
            return ("  TESTV   ");

        case A_TESTV:
            return ("  A_TESTV   ");

        case A_LI_CHECK:
            return ("  A_LI_CHECK");

        case LI_CHECK:
            return ("  LI_CHECK  ");

        case A_STR_CHECK:
            return ("  A_STR_CHECK");

        case STR_CHECK:
            return ("  STR_CHECK  ");

        default:

            sprintf (line, "%-8.8x ", x);
            return (line);
    }                              /* ende des switch */

}


/*----------------------------------------------------------*/
/* Debug Makro + Deklarationen                              */
/*----------------------------------------------------------*/


#if DEBUG

extern int  outstep;                                             /* falls
                                                                    gesetzt,
                                                                    gibt der
                                                                    Interpreter
                                                                    nach jedem
                                                                    Schritt
                                                                    Registerinhal
                                                                    te aus   */

#endif


#if DEBUG
 /* HARALD : Zur Vermeidung von undefined Variables in pm.o */

void /* TB, 4.11.1992 */
pm_debug ()
{
    int     row,
            col;



    if (outstep)
    {
        row = 3;
        col = 45;

    pm_test_monitor();  /* zur  Einzelschrittkontrolle der abstrakten
                             Maschine    */
        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile, "/*------------------*/\n");

        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile,
                 "/* PC:    %-8.8x  */\n", (unsigned int)pm_pc);

        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile,
                 "/* IR: %11.11s  */\n", pm_translate (*pm_pc));

        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile,
                 "/* BASE:  %-8.8x  */\n", (unsigned int)pm_base);


        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile,
                 "/* OFF:      %-3.3d    */\n", pm_offset);


        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile,
                 "/* AR:    %-8.8x  */\n", pm_arg);

        if (pmdmpcrs)
            cursorxy (row++, col);


        if (pm_match_fail)
            fprintf (pmdmpfile,
                     "/* MA_F:       OK    */\n");
        else
            fprintf (pmdmpfile,
                     "/* MA_F:     FAIL      */\n");


        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile,
                 "/*---System Stack---*/\n");
        if (pmdmpcrs)
            cursorxy (row++, col);

        if ((num_s - 2) >= 0)
        {
            fprintf (pmdmpfile,
                     "/* BASE:  %-8.8x  */\n", s[num_s - 1]);
            if (pmdmpcrs)
                 cursorxy (row++, col);
            fprintf (pmdmpfile,
                     "/* OFF:   %-8.8x  */\n", s[num_s - 2]);
        }
        else
        {
            fprintf (pmdmpfile,
                     "/* BASE:  %-8.8x  */\n", -1);
            if (pmdmpcrs)
                 cursorxy (row++, col);
            fprintf (pmdmpfile,
                     "/* OFF:   %-8.8x  */\n", -1);
        }


        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile,
                 "/*---BACKTR Stack---*/\n");
        if (pmdmpcrs)
            cursorxy (row++, col);

            fprintf (pmdmpfile,
                     "/* BASE:   %-8.8x  */\n", bt[num_bt ]);
            if (pmdmpcrs)
                 cursorxy (row++, col);
            fprintf (pmdmpfile,
                     "/* OFFS:  %-8.8x  */\n", bt[num_bt + 1]);
            if (pmdmpcrs)
                 cursorxy (row++, col);
            fprintf (pmdmpfile,
                     "/* PC :   %-8.8x  */\n", bt[num_bt + 2]);
            if (pmdmpcrs)
                 cursorxy (row++, col);
            fprintf (pmdmpfile,
                     "/* NVAR:  %-8.8x  */\n", bt[num_bt + 3]);

            if (pmdmpcrs)
                 cursorxy (row++, col);
            fprintf (pmdmpfile,
                     "/* ARITY:   %-8.8x */\n", bt[num_bt + 4]);

            if (pmdmpcrs)
                 cursorxy (row++, col);
            fprintf (pmdmpfile,
                     "/* ARG:    %-8.8x */\n", bt[num_bt + 5]);

            if (pmdmpcrs)
                 cursorxy (row++, col);
            fprintf (pmdmpfile,
                     "/* NSVAR:   %-8.8x */\n", bt[num_bt + 6]);

        if (pmdmpcrs)
            cursorxy (row++, col);
        fprintf (pmdmpfile, "/*------------------*/\n");


if (outstep != 0)
{
       if (pm_match_fail ==  FALSE)

                            monitor_call();
       if ((*pm_pc) ==  EOT)
                            monitor_call();
    }
    }

  return; /* TB, 4.11.1992 */
}

/*****************************************************************************/
/*                                                                           */
/* pm_out_instr gibt bei Eingabe einer Hexzahl den dazugehoerigen Befehl aus */
/*                                                                           */
/*****************************************************************************/

void pm_out_instr (x)

int     x;
{
    fprintf (stdout, " %s \n", pm_translate (x));
}




#endif
