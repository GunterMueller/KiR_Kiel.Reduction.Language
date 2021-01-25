/* $Log: rmodulnr.h,v $
 * Revision 1.4  1994/01/04 12:33:37  mah
 * bug fix
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:17  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */

/*********************************************************************/
/* Fuer jedes  Modul muss in dieser Tabelle eine Nummer             */
/* eingetragen werden, unter der dann das Messystem die anfallenden  */
/* Messdaten abrechnet.                                              */
/* Es koennen auch mehrere Module unter derselben Nummer zusammen-   */
/* gefasst werden; so haben z.B. alle Algorithmen der Preprocessing  */
/* Phase die Nummer 1 und alle Postprocessing Algorithmen die        */
/* Nummer 2.                                                         */
/*                                                                   */
/*                                                                   */
/* >>>  Achtung: bei Updates der Zaehler- oder Routinen-Nummern  <<< */
/* >>>           unbedingt auch die Auswertungsprozedur          <<< */
/* >>>           druck_measure  in breakpoint:c aendern.         <<< */
/* >>>  Achtung: bei Einfuegen neuer Module auch fetch_modul()   <<< */
/* >>>           in breakpoint aendern.                              */
/*                                                                   */
/*********************************************************************/

#define              BIGGEST_NUM  121          /* last used number of messtab */
#define              NO_ALG       122          /* last number of messtab */

/*********************************************************************/
/* Preprocessing Algorithmen                                         */
/*********************************************************************/

#define                EA_CREATE        1
#define                SUBFUNC          1
#define                RECFUNC          1
#define                CLOSEFUNC        1
#define                RR1PR            1
#define                R1PR             1
#define                ST_STRING        1
#define                ST_COND          1
#define                ST_DEC           1
#define                ST_EXPR          1
#define                ST_MVT           1      /* matrix / vector / tvector */
#define                ST_SCAL          1
#define                ST_SET           1
#define                ST_LIST          1
#define                ST_VAR           1
#define                ST_VARE          1
#define                VALI             1
#define                VALR             1
#define                CHECK_LIST       1
#define                PM_OUT_BEFEHL    1
#define                PM_GEN           1
#define                PMPREPA          1
#define                PM_PATTERN       1
#define                PM_APATTER       1
#define                PM_COMP          1
#define                PM_PATVAR        1
#define                PM_ST_MATCH      1
#define                PM_ST_SWITCH     1
#define                PM_PUT           1
#define                PM_TRAV_SPEC     1
#define                PM_TRAV          1
#define                PM_PREP          1
#define                PM_OUT_BEFEHL    1


/*********************************************************************/
/* Postprocessing Algorithmen                                        */
/*********************************************************************/

#define                EA_RETRIEVE        2
#define                AE_RETRIEVE        2
#define                POSFUNC            2
#define                PART_SUB           2
#define                PART_REC           2
#define                R1EAPOS            2
#define                INC_EXPR           2
#define                RET_FUNC           2
#define                RET_COND           2
#define                RET_DEC            2
#define                RET_EXPR           2
#define                RET_SCAL           2
#define                RET_LIST           2
#define                RET_NAME           2
#define                RET_VAR            2
#define                RET_MVT            2    /* matrix / vector / tvector */
#define                CONVI              2
#define                CONVR              2
#define                PM_RET_SWITCH      2
#define                PM_RET_MATCH       2
#define                PM_RET_NOMATCH     2


/*********************************************************************/
/* Allgemeine Algorithmen                                            */
/*********************************************************************/

#define               TTRAV               3
#define               TTRAV_A_E           4
#define               TTRAV_A_HILF        5


/*********************************************************************/
/* Reduktions - Algorithmen                                          */
/*********************************************************************/

#define                EAR               10
#define                COLLECT_ARGS      11
#define                CLOSECOND         12
#define                RED_PLUS          13
#define                RED_MINUS         14
#define                RED_MULT          15
#define                RED_DIV           16
#define                RED_MOD           17
#define                RED_IP            18
#define                RED_ABS           19
#define                RED_TRUNC         20
#define                RED_FRAC          21
#define                RED_FLOOR         22
#define                RED_CEIL          23
#define                RED_C             24
#define                RED_EQ            25
#define                RED_LE            26
#define                RED_LT            27
#define                RED_NOT           28
#define                RED_OR            29
#define                RED_AND           30
#define                RED_XOR           31
#define                RED_SELECT        32
#define                RED_ROTATE        33
#define                RED_CUT           34
#define                RED_UNITE         35
#define                RED_TRANSPOSE     36
#define                RED_UNI_TREE      37
#define       RED_TRANSFORM     38
#define       RED_REPLACE       39
#define       REDUCT            40
#define       FAILURE           41
#define       RED_F_EQ          42
#define       RED_LSELECT       43
#define       RED_LCUT          44
#define       RED_LROTATE       45
#define       RED_LUNITE        46
#define       RED_LREPLACE      47
#define       RED_REVERSE       48
#define       RED_LDIM          49
#define       RED_LTRANSFORM    50
#define       RED_DIM           51
#define       RED_EMPTY         52
#define       RED_CLASS         53
#define       RED_TYPE          54
#define       RED_SIN           55
#define       RED_COS           56
#define       RED_TAN           57
#define       RED_LN            58
#define       RED_EXP           59
#define       RED_NEG           60
#define       RED_MIN           61
#define       LPA_SUB           62
#define       SET_PROTECT       63
#define       LDEL_EXPR         64
#define       LTE_FREEV         65
#define       LFUN_DEF          66
#define       CHECK_EA          67
#define       TRAV_GOAL         68
#define       LRECFUNC          69
#define       RET_NINT          70
#define       LST_LREC          71
#define       LREC_ARGS         72
#define       EARET_LREC        73
#define       VAR_DEF           74
#define       COMP_NAME         75
#define       AERET_LREC        76
#define       TEST_LREC         77
#define       CHECK_GOAL_EXPR   78
#define       CHECK_AE          79
#define       ST_NINT           80
#define       NEW_LREC          81
#define       CHE_PTDD          82
#define       CHE_PLARGS        83
#define       NEW_HEAPSTRUC     84
#define       TRAVAE            85
#define       STORE_E           86
#define       STORE_L           87
#define                PM_AM              88
#define                PM_DIMLI           89
#define                PM_CLOSE_NOMAT     90
#define                PM_CLOSESWITCH     91
#define                PM_SCAL_EQ         92
#define                PM_RED_F_EQ        93
#define                PM_RED_NOMAT       94
#define                PMMAT              95
#define                PMREDSWI           96
#define       RED_MCUT          97
#define       RED_MREPLACE      98
#define       RED_MRE_C         99   /* replace column */
#define       RED_MRE_R        100   /* replace row    */
#define       RED_MROTATE      101
#define       RED_MSELECT      102
#define       RED_MUNITE       103
#define       RED_VCUT         104
#define       RED_VSELECT      105
#define       RED_VREPLACE     106
#define       RED_VROTATE      107
#define       RED_VUNITE       108
#define       RED_VDIM         109
#define       RED_VC           110
#define       RED_TO_MAT       111
#define       RED_TO_VECT      112
#define       RED_TO_TVECT     113
#define       RED_TO_SCAL      114
#define       RED_MDIM         115
#define       RED_SUBSTR       116
#define       RED_REPSTR       117
#define       RED_TO_LIST      118
#define       RED_TO_FIELD     119
#define       RED_MK_STRING    120
#define       RED_QUOT         121

/* auskommentiert von RS 5.11.1992 */
#if 0
static
 char *name[BIGGEST_NUM+1] = {"INIT" , "PREPROC" , "POSTPROC" , "" , "" ,
            "" , " " , " " , " " , " " ,
           "EAR" ,
           "COLLECT_ARGS" ,
           "CLOSECOND" ,
           "RED_PLUS" ,
           "RED_MINUS" ,
           "RED_MULT" ,
           "RED_DIV" ,
           "RED_MOD" ,
           "RED_IP" ,
           "RED_ABS" ,
           "RED_TRUNC" ,
           "RED_FRAC" ,
           "RED_FLOOR" ,
           "RED_CEIL" ,
           "RED_C" ,
           "RED_EQ" ,
           "RED_LE" ,
           "RED_LT" ,
           "RED_NOT" ,
           "RED_OR" ,
           "RED_AND" ,
           "RED_XOR" ,
           "RED_SELECT" ,
           "RED_ROTATE" ,
           "RED_CUT" ,
           "RED_UNITE" ,
           "RED_TRANSPO" ,
           "RED_UNI_TR" ,
           "RED_TRANSFOR" ,
           "R_REPLACE" ,
           "REDUCT" ,
           "FAILURE" ,
           "RED_F_EQ" ,
           "RED_LSELECT" ,
           "RED_LCUT" ,
           "RED_LROTATE" ,
           "RED_LUNITE" ,
           "RED_LREPLACE" ,
           "RED_REVERSE" ,
           "RED_LDIM" ,
           "RED_LTRANSF" ,
           "RED_DIM" ,
           "RED_EMPTY" ,
           "RED_CLASS" ,
           "RED_TYPE" ,
           "RED_SIN" ,
           "RED_COS" ,
           "RED_TAN" ,
           "RED_LN" ,
           "RED_EXP" ,
           "RED_NEG" ,
           "RED_MIN" ,
           "LEDPA_SUB" ,
           "SET_PROT" ,
           "LDEL_EXPR" ,
           "LTE_FREEV" ,
           "LFUN_DEF" ,
           "CHECK_EA" ,
           "TRAV_GOAL" ,
           "LRECFUNC" ,
           "RET_NINT" ,
           "LST_LREC" ,
           "LREC_ARGS" ,
           "EARET_LREC" ,
           "VAR_DEF" ,
           "COMP_NAME" ,
           "AERET_LREC" ,
           "TEST_LREC" ,
           "CHECK_GOAL" ,
           "CHECK_AE" ,
           "ST_NINT" ,
           "NEW_LREC" ,
           "CHE_PTDD" ,
           "CHE_PLARGS" ,
           "NEW_HEAPST" ,
           "TRAVAE" ,
           "STORE_E" ,
           "STORE_L" ,
           "PM_AM" ,
           "PM_DIMLI" ,
           "PM_CLOSE_NOMAT" ,
           "PM_CLOSESWITCH" ,
           "PM_SCAL_EQ" ,
           "PM_RED_F_EQ" ,
           "PM_RED_NOMAT" ,
           "PMMAT" ,
           "PMREDSWI" ,
           "RED_MCUT",
           "RED_MREPLACE",
           "RED_MRE_C",
           "RED_MRE_R",
           "RED_MROTATE",
           "RED_MSELECT",
           "RED_MUNITE",
           "RED_VCUT",
           "RED_VSELECT",
           "RED_VREPLACE",
           "RED_VROTATE",
           "RED_VUNITE",
           "RED_VDIM",
           "RED_VC",
           "RED_TO_MAT",
           "RED_TO_VECT",
           "RED_TO_TVECT",
           "RED_TO_SCAL",
           "RED_MDIM",
           "RED_SUBSTR",
           "RED_REPSTR",
           "RED_TO_LIST",
           "RED_TO_FIELD",
           "RED_MK_STRING",
           "RED_QUOT",
          };
#endif /* 0 */

/*********************** end of file rmodulnr.h *****************************/
