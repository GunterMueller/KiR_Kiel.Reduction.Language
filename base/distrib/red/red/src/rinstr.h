/* $Log: rinstr.h,v $
 * Revision 1.4  1996/03/29 16:00:12  stt
 * unused definition of POP_PM_BLOCK deleted
 *
 * Revision 1.3  1994/03/08  18:56:51  mah
 * tilde version
 *
 * Revision 1.2  1993/11/19  15:17:08  rs
 * SYSTEM_STACK_SIZE increased
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */




/*--------------------------------------------------------------------*/
/*                                                                    */
/* Kodierung der Befehle  der abstrakten Maschine (Interpreter)       */
/*                                                                   */
/*                                                                   */
/*-------------------------------------------------------------------*/





#define JMP                0x01000001           /* Sprung an die im
                                                   Instruction  array an
                                                   der naechsten Stelle
                                                   angebene Adresse
                                                */
#define JMP2               0x01000002           /* Ueberspringen der naechsten
                                                   zwei Positionen(also relativ)
                                                   (fuer In-Konstrukt benoetigt)
                                                */

#define SETBASE            0x02000001           /* Adresse eines
                                                   Listendeskriptors
                                                */

#define A_SETBASE          0x02000011           /* Adresse eines
                                                   Listendes-       */

#define INCR               0x03000001           /* ++ pm_offset
                                                */

#define DECR               0x04000001           /* -- pm_offset
                                                */

#define TESTN              0x05010001           /* testet ob
                                                   Listenkonstruktor
                                                   Stelligkeit n hat
                                                */


#define A_TESTN            0x05010011           /* testet ob
                                                   Listenkonstruktor
                                                   Stelligkeit n hat
                                                */
#define TESTV              0x05020001           /* testet ob
                                                   Listenkonstruktor
                                               mindestens
                                                   Stelligkeit n hat
                                                */


#define A_TESTV            0x05020011           /* testet ob
                                                   Listenkonstruktor
                                               mindestens
                                                   Stelligkeit n hat
                                                */

#define LOAD               0x07000001           /* Lade Variable
                                                */

#define A_LOAD             0x07000011           /* Lade Variable
                                                */
#define SUBR               0x08000001           /* rette base und offset
                                                   register
                                                */

#define RETURN             0x09000001           /* ueberschreibe base und
                                                   offsetregister mit ge-
                                                   retteten Werten,
                                                   Behandlung  von
                                                   match_fail Register
                                                */
#define EOT               0x10000001                     /* ende des instr. array
                                                */


/* K o n s t a n t e n t e s t b e f e h l e */

#define KTEST              0x11090001           /* Test auf Konstante
                                                */
#define A_KTEST            0x11090011           /* Test auf Konstante
                                                */

       /* Testbefehle in Listen*/

#define STR_TEST           0x11010001

#define DIG_TEST           0x11020001

#define SCAL_TEST          0x11030001

#define MATRIX_TEST        0x11040001

#define BOOL_TEST          0x11050001

#define TY_TEST            0x11060001

#define NIL_TEST           0x11070001

#define NILM_TEST          0x11080001

#define VECTOR_TEST        0x11100001

#define TVECTOR_TEST       0x11110001

#define NILV_TEST          0x11120001

#define NILTV_TEST         0x11130001

#define NILSTRING_TEST     0x11140001

#define IN_TEST            0x11150001

#define IN_TEST1           0x11160001

    /* Testbefehle fuer Konstanten auf oberster Ebene */
#define A_STR_TEST         0x11010011

#define A_DIG_TEST         0x11020011

#define A_SCAL_TEST        0x11030011

#define A_MATRIX_TEST      0x11040011

#define A_BOOL_TEST        0x11050011

#define A_TY_TEST          0x11060011

#define A_NIL_TEST         0x11070011

#define A_NILM_TEST        0x11080011

#define A_VECTOR_TEST      0x11100011

#define A_TVECTOR_TEST     0x11110011

#define A_NILV_TEST        0x11120011

#define A_NILTV_TEST       0x11130011

#define A_NILSTRING_TEST   0x11140011

    /* Testbefehle fuer String-/Listen-Konstruktoren */         
#define A_LI_CHECK         0x11150011

#define LI_CHECK           0x11160011

#define A_STR_CHECK        0x11170011

#define STR_CHECK          0x11180011



/* Load Befehl fuer die Dimension einer Liste  */

#define LOAD_ARITY         0x12000001

#define A_LOAD_ARITY       0x12000011



/* fuer die ... in den Pattern      */
#define SKIPSKIP           0x13000001

#define IN_SKIPSKIP        0x13000002

#define LA_SKIPSKIP        0x14000001  /* fuer die letzten
                                          ... in einer liste */
#define LA_IN_SKIPSKIP     0x14000002  /* fuer die letzten
                                          ... in einer liste
                                          (im In-Konstrukt)  */

/* Laden einer Unterliste fuer <A, X as ... , 1,2>  */
#define L_LOAD_B           0x15000001
/* Retten des Offset in bt-stack und die
 Variable pm_o_offs  */
#define L_LOAD_A           0x16000001

/* Laden einer Variablen ohne anschliesendes Incrementieren
   des offset */
#define AS_LOAD            0x17000001

#define  PM_DOLL           0x18000001

#define  IN_PM_DOLL        0x18000002

#define  PM_CUTT           0x19000001

#define  END_DOLL          0x20000001

#define  GEN_BLOCK         0x21000001
#define  PM_PUSH           0x22000001
/* Befehle fuer Auftauchen eines sonstigen Atoms im Pattern */
#define  ATOM_TEST         0x23000001

#define  A_ATOM_TEST       0x24000001

#define  DEL_BLOCKS        0x25000001         /* n BS-frames freigeben */
/*-------------------------------------------------------------------*/
/* Konstanten ,die in pmear:c und pmam:c verwendet wird              */
/*---------------------------------------------------------------*/

#define MATCH_STACK_SIZE        10     /* Array das als
                                                   Parameter an     pm_an
                                                   uebergeben wird und
                                                   die
                                                   Variableninstantiierung
                                                   en auf- nimmt
                                                */
#define SYSTEM_STACK_SIZE      500

#define BT_STACK_SIZE      100

#define PM_PASST               1       /* pattern matcht */


#define PM_PASST_NICHT         0       /* pattern passt nicht */


#define PM_UNDEC               3       /* im argument steht eine
                                                   variable, die gegen
                                                   konstruktor verglichen

                                          wird */








/* Maske zum Test ob ein Test auf eine Konstante
    oder einen Listenkonstruktor vorliegt  */


#define  A_FLAG                    0x00000010

#define PM_KONS                    0x00FF0000

#define PM_T_KONS(x)        (  ( x & PM_KONS)  != 0)

#define PM_T_A_INSTR(x)       ( ( x & A_FLAG)  == 1)

