/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
/*****************************************************************************/
/*                           INLCUDE -  FILE                                 */
/*---------------------------------------------------------------------------*/
/*  rstelem.h                                                                */
/*  In dieser Datei wird das Format der Felder in den Stackelementen         */
/*  durch entsprechende Masken festgelegt.                                   */
/*  Diese Datei bildet die Schnittstelle zum Editor, da dort die             */
/*  Stackelemente mit dem hier bestimmten Format erzeugt werden .            */
/*                                                                           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/*                              defines                                      */
/*---------------------------------------------------------------------------*/
/* ----  REAL and INTEGER are not stackelements -------*/
#define   INTEGER       0
#define   REAL          1

/*********************************************************************/
/*                                                                   */
/*        V A L U E - F E L D   M A S K E N                          */
/*                                                                   */
/*********************************************************************/

#define  F_VALUE                0xFFFF0000     /* Bit 16-31          */
#define  O_VALUE                16             /* Offset 16          */

#define  F_DL_VALUE             F_VALUE        /* Bit 16-31          */
#define  O_DL_VALUE             O_VALUE        /* Offset 16          */

#define  F_ARITY                F_VALUE        /* Bit 16-31          */
#define  O_ARITY                O_VALUE        /* Offset 16          */

/*********************************************************************/
/*  VALUE-HIGH-Byte      (Bits 31-24)                                */
/*********************************************************************/

#define  F_FUNC_GROUP           0xFF000000     /* Bit 24-31          */
#define  O_FUNC_GROUP           24             /* Offset 24          */


/*********************************************************************/
/*  VALUE-LOW-Byte       (Bits 23-17)                                */
/*********************************************************************/

#define  F_FUNC_NAME            0x00FF0000     /* Bit 16-23          */
#define  O_FUNC_NAME            16             /* Offset 16          */



/*********************************************************************/
/*                                                                   */
/*              E D I T - F E L D   M A S K E N                      */
/*                                                                   */
/*********************************************************************/


#define  F_EDIT                 0x0000FF00     /* Bit 8-15           */
#define  O_EDIT                 8              /* Offset 8           */
#define  EXTR_EDIT(x)  (((x)&F_EDIT)>>O_EDIT)  /* extract edit-field */
#define  SET_EDIT(x)   (((x) << O_EDIT) & F_EDIT) /* set edit-field  */

/*********************************************************************/
/*                                                                   */
/*            C L A S S - F E L D   M A S K E N                      */
/*                                                                   */
/*********************************************************************/


/*********************************************************************/
/*  CLASS-HIGH-Halbbyte  (Bits 7-4)                                  */
/*********************************************************************/

#define  F_CONSTR_NAME          0x000000F0     /* Bit 4-7            */
#define  O_CONSTR_NAME          4              /* Offset 4           */

#define  F_FUNC_ARITY           0x000000F0     /* Bit 4-7            */
#define  O_FUNC_ARITY           4              /* Offset 4           */

#define  F_NUM                  0x00000010     /* Bit 4              */
#define  O_NUM                  4              /* Offset 4           */

#define  F_DOLLAR               0x00000020     /* Bit 5              */
#define  O_DOLLAR               5              /* Offset 5           */

#define  F_VARSTR_FLAG          0x00000010     /* Bit 4              */
#define  O_VARSTR_FLAG          4              /* Offset 4           */

#define  F_TILDE                0x00000040     /* Bit 6              */
#define  O_TILDE                6              /* Offset 6           */


/*********************************************************************/
/*  CLASS-LOW- Halbbyte  (Bits 3-0)                                  */
/*********************************************************************/

#define  F_TYPE                 0x0000000F     /* Bit 0-3            */
#define  O_TYPE                 0              /* Offset 0           */


/*********************************************************************/
/*  CLASS-LOW-Bit 3                                                  */
/*********************************************************************/

#define  F_CONAT_FLAG           0x00000008     /* Bit 3              */
#define  O_CONAT_FLAG           3              /* Offset 3           */


/*********************************************************************/
/*  CLASS-LOW-Bit 2                                                  */
/*********************************************************************/

/* Bit 2 atom  0xxx                                                  */
#define  F_ATOM_TYPE_FLAG       0x00000004     /* Bit 2              */
#define  O_ATOM_TYPE_FLAG       2              /* Offset 2           */

/* Bit 2 constructor 1xxx                                             */
#define  F_FIXVAR_FLAG          0x00000004     /* Bit 2              */
#define  O_FIXVAR_FLAG          2              /* Offset 2           */


/*********************************************************************/
/*  CLASS-LOW-Bit 1                                                  */
/*********************************************************************/

/* Bit 1 s - atoms   00xx                                             */
#define  F_SINGLE_ATOM_TYPE     0x00000003     /* Bit 0-1            */
#define  O_SINGLE_ATOM_TYPE     0              /* Offset 0           */

/* Bit 1 m - atoms   01xx                                             */
#define  F_DIGLET_FLAG          0x00000002     /* Bit 1              */
#define  O_DIGLET_FLAG          1              /* Offset 1           */

/* Bit 1 fixconstr   10xx    U N U S E D                              */

/* Bit 1 varconstr   11xx                                             */
#define  F_CHAIN_FLAG           0x00000002     /* Bit 1              */
#define  O_CHAIN_FLAG           1              /* Offset 1           */


/*********************************************************************/
/*  CLASS-LOW-Bit 0                                                  */
/*********************************************************************/

/* Bit 0 m - atoms   011x       (digit/letter)                        */
#define  F_MARK_FLAG            0x00000001     /* Bit 0              */
#define  O_MARK_FLAG            0              /* Offset 0           */

/* Bit 0 s - atoms   00xx    SINGLE_ATOM_TYPE !!!                     */
/* Bit 0 fixconstr   10xx    U N U S E D                              */
#define  F_SYSAP               01
#define  P_SYSAP               01
#define  V_SYSAP               01
#define  SET_AP(x)             (x |=1)
#define  CLEAR_AP(x)           (x &= ~(01))
#define  T_SYSAP(x)            ((x & P_SYSAP) == V_SYSAP)




/*$P*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                                                                   */
/*  Basis   _ - , P - und Namens - M A S K E N                       */
/*                                                                   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*********************************************************************/
/*                                                                   */
/*   C L A S S - F E L D                                             */
/*                                                                   */
/*********************************************************************/

/*********************************************************************/
/*    STACKELEMENT  Bit 3                                            */
/*********************************************************************/


#define  _ATOM                  0
#define  _CON                   1              /* gleich EINS        */

#define  P_ATOM                 F_CONAT_FLAG   /* Konstruktor flag   */
#define  P_CON                  F_CONAT_FLAG   /* Konstruktor flag   */

#define  ATOM                   (_ATOM   << O_CONAT_FLAG )
#define  CON                    (_CON    << O_CONAT_FLAG )

#define  T_ATOM(x)              ( ( (x) & P_ATOM ) == ATOM )
#define  T_CON(x)               ( ( (x) & P_CON) == CON)

/*********************************************************************/
/*  mit gesetzten Arity-Werten                                       */
/*********************************************************************/

#define  P_CON_1                (F_CONAT_FLAG | F_ARITY)
#define  CON_1                  (SET_ARITY((_CON<<O_CONAT_FLAG),1))

#define  P_CON_2                (F_CONAT_FLAG | F_ARITY)
#define  CON_2                  (SET_ARITY((_CON<<O_CONAT_FLAG),2))

/*********************************************************************/
/*    ATOMS          Bit 2  (3-2)                                    */
/*********************************************************************/

#define  _SA                    0
#define  _MA                    1

#define  P_SA                   (P_ATOM   | F_ATOM_TYPE_FLAG)
#define  P_MA                   (P_ATOM   | F_ATOM_TYPE_FLAG)

#define  SA                     ((_SA << O_ATOM_TYPE_FLAG) | ATOM   )
#define  MA                     ((_MA << O_ATOM_TYPE_FLAG) | ATOM   )

#define  T_SA(x)                ( ( (x) & P_SA ) == SA)
#define  T_MA(x)                ( ( (x) & P_MA ) == MA)

/*********************************************************************/
/*    MA             Bit 1  (3-1)                                    */
/*********************************************************************/

#define  _LETTER                0
#define  _DIGIT                 1

#define  P_LETTER               (P_MA     | F_DIGLET_FLAG)
#define  P_DIGIT                (P_MA     | F_DIGLET_FLAG)

#define  LETTER                 ((_LETTER << O_DIGLET_FLAG) | MA     )
#define  DIGIT                  ((_DIGIT  << O_DIGLET_FLAG) | MA     )

#define  T_LETTER(x)            ( ( (x) & P_LETTER ) == LETTER)
#define  T_DIGIT(x)             ( ( (x) & P_DIGIT ) == DIGIT)

/*********************************************************************/
/* CHARACTER-TYPE    Bit 4  (4-1)                                    */
/*********************************************************************/

#define  _VAR_CHAR              0              /* variable character */
#define  _STR_CHAR              1              /*   string character */

#define  P_VAR_CHAR             (P_LETTER | F_VARSTR_FLAG)
#define  P_STR_CHAR             (P_LETTER | F_VARSTR_FLAG)

#define  VAR_CHAR               ((_VAR_CHAR << O_VARSTR_FLAG) | LETTER )
#define  STR_CHAR               ((_STR_CHAR << O_VARSTR_FLAG) | LETTER )

#define  T_VAR_CHAR(x)          ( ( (x) & P_VAR_CHAR ) == VAR_CHAR)
#define  T_STR_CHAR(x)          ( ( (x) & P_STR_CHAR ) == STR_CHAR)

/*********************************************************************/
/*    LETTER         Bit 0  (4-0)    (only letter in variable)       */
/*********************************************************************/

#define  _LET0                  0                     /* last letter */
#define  _LET1                  1

#define  P_LET0                 (P_VAR_CHAR | F_MARK_FLAG)
#define  P_LET1                 (P_VAR_CHAR | F_MARK_FLAG)

#define  LET0                   ((_LET0 << O_MARK_FLAG) | VAR_CHAR )
#define  LET1                   ((_LET1 << O_MARK_FLAG) | VAR_CHAR )

#define  T_LET0(x)                ( ( (x) & P_LET0 ) == LET0)
#define  T_LET1(x)                ( ( (x) & P_LET1 ) == LET1)

/*********************************************************************/
/*    DIGIT          Bit 0  (3-0)                                    */
/*********************************************************************/

#define  _DIG0                  0
#define  _DIG1                  1

#define  P_DIG0                 (P_DIGIT  | F_MARK_FLAG)
#define  P_DIG1                 (P_DIGIT  | F_MARK_FLAG)

#define  DIG0                   ((_DIG0 << O_MARK_FLAG) | DIGIT  )
#define  DIG1                   ((_DIG1 << O_MARK_FLAG) | DIGIT  )

#define  T_DIG0(x)                ( ( (x) & P_DIG0 ) == DIG0)
#define  T_DIG1(x)                ( ( (x) & P_DIG1 ) == DIG1)

/*********************************************************************/
/*    SA             Bit 1-0  (3-0)                                  */
/*********************************************************************/

#define  _POINTER               0
#define  _FUNC                  1            /* structfunc & valfunc */
#define  _CONSTANT              3

#define  P_POINTER              (P_SA     | F_SINGLE_ATOM_TYPE)
#define  P_FUNC                 (P_SA     | F_SINGLE_ATOM_TYPE)
#define  P_CONSTANT             (P_SA     | F_SINGLE_ATOM_TYPE)

#define  POINTER                ((_POINTER << O_SINGLE_ATOM_TYPE) | SA )
#define  FUNC                   ((_FUNC << O_SINGLE_ATOM_TYPE) | SA )
#define  CONSTANT               ((_CONSTANT << O_SINGLE_ATOM_TYPE) | SA)

/* define  T_POINTER(x)           ( ( (x) & P_POINTER ) == POINTER) */
#define  T_FUNC(x)              ( ( (x) & P_FUNC ) == FUNC)
#define  T_CONSTANT(x)          ( ( (x) & P_CONSTANT ) == CONSTANT)

/*********************************************************************/
/*    CONSTRUCTORS   Bit 2  (3-2)                                    */
/*********************************************************************/

#define  _VAR_CONSTR            0
#define  _FIX_CONSTR            1

#define  P_VAR_CONSTR           (P_CON | F_FIXVAR_FLAG )
#define  P_FIX_CONSTR           (P_CON | F_FIXVAR_FLAG )

#define  P_VAR_CCONSTR           (P_CON | F_FIXVAR_FLAG | F_CHAIN_FLAG)
#define  P_FIX_CCONSTR           (P_CON | F_FIXVAR_FLAG | F_CHAIN_FLAG)

#define  VAR_CONSTR             ((_VAR_CONSTR << O_FIXVAR_FLAG) |CON)
#define  FIX_CONSTR             ((_FIX_CONSTR << O_FIXVAR_FLAG) |CON)

#define  T_VAR_CONSTR(x)        ( ( (x) & P_VAR_CONSTR ) == VAR_CONSTR)
#define  T_FIX_CONSTR(x)        ( ( (x) & P_FIX_CONSTR ) == FIX_CONSTR)

#define  T_VAR_CCONSTR(x)        ( ( (x) & P_VAR_CCONSTR ) == VAR_CCONSTR)
#define  T_FIX_CCONSTR(x)        ( ( (x) & P_FIX_CCONSTR ) == FIX_CCONSTR)

/*********************************************************************/
/*    VAR_CONSTRUCTORS   Bit 1  (3-1)                                */
/*********************************************************************/

#define  _UNCHAINED_CONSTR      0
#define  _CHAINED_CONSTR        1

#define  P_UNCHAINED_CONSTR     (P_VAR_CONSTR | F_CHAIN_FLAG )
#define  P_CHAINED_CONSTR       (P_VAR_CONSTR | F_CHAIN_FLAG )

#define  UNCHAINED_CONSTR       ((_UNCHAINED_CONSTR << O_CHAIN_FLAG) | \
                                VAR_CONSTR )
#define  CHAINED_CONSTR         ((_CHAINED_CONSTR << O_CHAIN_FLAG) |   \
                                VAR_CONSTR )

#define GROUP1_FIXCON (( _UNCHAINED_CONSTR << O_CHAIN_FLAG) | FIX_CONSTR )

#define GROUP2_FIXCON (( _CHAINED_CONSTR << O_CHAIN_FLAG) | FIX_CONSTR )

#define  T_UNCHAINED_CONSTR(x)  ( ( (x) & P_UNCHAINED_CONSTR )   \
                                           == UNCHAINED_CONSTR)
#define  T_CHAINED_CONSTR(x)    ( ( (x) & P_CHAINED_CONSTR )   \
                                           == CHAINED_CONSTR)

/*********************************************************************/
/*    VAR_CONSTRUCTORS   Bit 0  (3-0)                                */
/*********************************************************************/

/*    Bit 0 unused for Var_constructors                              */


/*********************************************************************/
/*    FIX_CONSTRUCTORS   Bit 1-0  (3-0)                              */
/*********************************************************************/

/*    Bit 1-0 unused for Fix_constructors                            */


/*********************************************************************/
/*    STACKELEMENT  Bit 4 & 5                                        */
/*********************************************************************/

#define  _NORMAL_CONST          0              /* gleich EINS        */
#define  _NUM                   1
#define  _DOLLAR                1
#define  _TILDE                 1

#define  P_NUM                  (F_NUM    | P_CONSTANT )
#define  P_NORMAL_CONST         (F_NUM    | P_CONSTANT )
#define  P_DOLLAR               (F_DOLLAR | P_CONSTANT |  F_NUM    )
#define  P_TILDE                (F_TILDE | P_CONSTANT |  F_NUM    )

#define  NUM                    ((_NUM    << O_NUM    ) | CONSTANT )
#define  NORMAL_CONST           ((_NORMAL_CONST<<O_NUM    )|CONSTANT )
#define  DOLLAR     ((_DOLLAR<<O_DOLLAR )|CONSTANT  | (_NORMAL_CONST<<O_NUM    ) )
#define  TILDE      ((_TILDE<<O_TILDE )|CONSTANT  | (_NORMAL_CONST<<O_NUM    ) )


#define  T_NORMAL_CONST(x)     ( ( (x) & P_NORMAL_CONST ) == NORMAL_CONST)
#define  T_NUM(x)              ( ( (x) & P_NUM    ) == NUM   )
#define  T_DOLLAR(x)           ( ( (x) & P_DOLLAR ) == DOLLAR)
#define  T_TILDE(x)            ( ( (x) & P_TILDE ) == TILDE)

/*********************************************************************/
/*    SPECIALS                                                       */
/*********************************************************************/

/*********************************************************************/
/*    String                                                         */
/*********************************************************************/


#define  P_STR0                 (P_MA     | F_MARK_FLAG)
#define  _STR0                  0
#define  STR0                   ((_STR0 << O_MARK_FLAG) | MA     )
#define  T_STR0(x)              ( ( (x) & P_STR0 ) == STR0)

#define  P_STR1                 (P_MA     | F_MARK_FLAG)
#define  _STR1                  1
#define  STR1                   ((_STR1 << O_MARK_FLAG) | MA     )
#define  T_STR1(x)              ( ( (x) & P_STR1 ) == STR1)

#define  P_STR                  P_MA
#define  STR                    MA
#define  T_STR(x)               ( ( (x) & P_STR ) == STR)

#define  P_DECPT                ( P_DIGIT | F_VALUE )
#define  DECPT                  ( DIGIT   | '.' << O_VALUE  )
#define  T_DECPT(x)             ( ( (x) & P_DECPT ) == DECPT)

#define  P_MARK                 (F_MARK_FLAG)


/*********************************************************************/
/*               T E S T - M A K R O S                               */
/*********************************************************************/

#define  IS_POINTER(x)        (!(x & P_POINTER ))
#define  DEC(x)               (x - (1 << O_VALUE))
#define  INC(x)               (x + (1 << O_VALUE))
#define  SET_ARITY(x,n)       ( ( (x) & ( ~ F_ARITY ) ) |       \
                                (n << O_ARITY) )
#define  ARITY(x)             ( (x) >> O_ARITY )

#define  ARITY_GT_1(x)        ( (x) & 0xFFFE000 )

#define  SET_VALUE(x,n)       (( (x) & (~F_VALUE)) |  \
                               (n << O_VALUE))
#define   VALUE(x)            ( (x) >> O_VALUE)
#define   EXTID(x)            ( (x) & ~F_VALUE)

#define  SWITCH(x)            (  ( (x) & F_MARK_FLAG )     \
                               ?  (x) & ~(1 << O_MARK_FLAG ) \
                               :  x | (1 << O_MARK_FLAG ) \
                              )
#define  TOGGLE(x)            (  ( (x) & F_MARK_FLAG )     \
                               ?  (x) & ~(1 << O_MARK_FLAG ) \
                               :  x | (1 << O_MARK_FLAG ) \
                              )

#define  FUNC_INDEX(x)        ( ( (x) & F_FUNC_NAME ) >> O_FUNC_NAME )

#define  FUNC_ARITY(x)        ( ( (x) & F_FUNC_ARITY) >> O_FUNC_ARITY)

/*********************************************************************/
/*  Masken fuer Konstruktoren                                        */
/*********************************************************************/

#define P_FIXCOMPL  ( F_CONSTR_NAME | P_FIX_CONSTR)
#define P_VARCOMPL  ( F_CONSTR_NAME | P_VAR_CONSTR)

/*********************************************************************/
/*  Erzeugt den fixstelligen Konstruktor der Nummer 'nr' und der     */
/*  Stelligkeit 'ar' als Stackelement in val.                        */
/*********************************************************************/

#define  FIXCONSTR(nr,ar) \
         ( FIX_CONSTR | ((nr) << O_CONSTR_NAME) )

/*********************************************************************/
/*  Erzeugt den variabel-stelligen Konstruktor der Nummer 'nr'       */
/*  Die Stelligkeit bleibt unberuecksichtigt (auch in der P_Maske)   */
/*********************************************************************/

#define  VARCON(nr)  \
         ( VAR_CONSTR | ((nr) << O_CONSTR_NAME) )


/*********************************************************************/
/*                                                                   */
/*  F I X - C O N S T R U C T O R S                                  */
/*                                                                   */
/*********************************************************************/

#define  _PRELIST       0
#define  P_PRELIST      P_CFIXCOMPL
#define  PRELIST        FIXCONSTR( _PRELIST,2 )
#define  T_PRELIST(x)   (( (x) & P_PRELIST) == PRELIST)

#define  _PROTECT       1
#define  P_PROTECT      P_CFIXCOMPL
#define  PROTECT        FIXCONSTR( _PROTECT,1  )
#define  T_PROTECT(x)   ( ( (x) & P_PROTECT ) == PROTECT )

#define  _PLUS          2
#define  P_PLUS         P_CFIXCOMPL
#define  PLUS           FIXCONSTR( _PLUS,1 )
#define  T_PLUS(x)      ( ( (x) & P_PLUS ) == PLUS )

#define  _MINUS         3
#define  P_MINUS        P_CFIXCOMPL
#define  MINUS          FIXCONSTR( _MINUS,1 )
#define  T_MINUS(x)     ( ( (x) & P_MINUS ) == MINUS )

#define  _SET           4
#define  P_SET          P_CFIXCOMPL
#define  SET            FIXCONSTR( _SET,1 )
#define  T_SET(x)       ( ( (x) & P_SET ) == SET )

#define  _REC           5
#define  P_REC          P_CFIXCOMPL
#define  REC            FIXCONSTR( _REC,2  )
#define  T_REC(x)       ( ( (x) & P_REC ) == REC )

#define  _COND          6
#define  P_COND         P_CFIXCOMPL
#define  COND           FIXCONSTR( _COND,2 )
#define  T_COND(x)      ( ( (x) & P_COND ) == COND )

#define  _SCALAR        7
#define  P_SCALAR       P_CFIXCOMPL
#define  SCALAR         FIXCONSTR( _SCALAR,2 )
#define  T_SCALAR(x)    ( ( (x) & P_SCALAR ) == SCALAR )

#define  _VECTOR        8
#define  P_VECTOR       P_CFIXCOMPL
#define  VECTOR         FIXCONSTR( _VECTOR,2 )
#define  T_VECTOR(x)    ( ( (x) & P_VECTOR ) == VECTOR )

#define  _TVECTOR       9
#define  P_TVECTOR      P_CFIXCOMPL
#define  TVECTOR        FIXCONSTR( _TVECTOR,2 )
#define  T_TVECTOR(x)   ( ( (x) & P_TVECTOR ) == TVECTOR )

#define  _EXP           11
#define  P_EXP          P_CFIXCOMPL
#define  EXP            FIXCONSTR( _EXP,2 )
#define  T_EXP(x)       ( ( (x) & P_EXP ) == EXP )

#define  _F             12
#define  P_F            P_CFIXCOMPL
#define  F              FIXCONSTR( _F,2 )
#define  T_F(x)         ( ( (x) & P_F) == F )

/* H */

#define  _PM_OTHERWISE          13
#define  P_PM_OTHERWISE         P_CFIXCOMPL
#define  PM_OTHERWISE           FIXCONSTR( _PM_OTHERWISE,1 )
#define  T_PM_OTHERWISE(x)      ( ( (x) & P_PM_OTHERWISE) == PM_OTHERWISE)

#define  _PM_WHEN          14
#define  P_PM_WHEN         P_CFIXCOMPL
#define  PM_WHEN           FIXCONSTR( _PM_WHEN,3 )
#define  T_PM_WHEN(x)      ( ( (x) & P_PM_WHEN) == PM_WHEN)

#define  _PM_MATCH          15
#define  P_PM_MATCH         P_CFIXCOMPL
#define  PM_MATCH           FIXCONSTR( _PM_MATCH,3 )
#define  T_PM_MATCH(x)      ( ( (x) & P_PM_MATCH) == PM_MATCH)


/*********************************************************************/
/*                                                                   */
/*  V A R - C O N S T R U C T O R S                                  */
/*                                                                   */
/*********************************************************************/

#define  P_AP_1          ( P_CVARCOMPL | F_ARITY )
#define  AP_1            ( SET_ARITY(VARCON( _AP  ) , 1 ))
#define  T_AP_1(x)       ( ( (x) & P_AP_1 ) == AP_1 )

#define  P_AP_2          ( P_CVARCOMPL | F_ARITY )
#define  AP_2            ( SET_ARITY(VARCON( _AP  ) , 2 ))
#define  T_AP_2(x)       ( ( (x) & P_AP_2 ) == AP_2 )

#define  _AP            0
#define  P_AP           P_CVARCOMPL
#define  AP             VARCON( _AP  )
#define  T_AP(x)        ( ( (x) & P_AP ) == AP )

#define  _SNAP          1
#define  P_SNAP         P_CVARCOMPL
#define  SNAP           VARCON( _SNAP  )
#define  T_SNAP(x)      ( ( (x) & P_SNAP ) == SNAP )

#define  _AP_TIC        2
#define  P_AP_TIC       P_CVARCOMPL
#define  AP_TIC         VARCON( _AP_TIC  )
#define  T_AP_TIC(x)    ( ( (x) & P_AP ) == AP )

#define  _SUB           3
#define  P_SUB          P_CVARCOMPL
#define  SUB            VARCON( _SUB  )
#define  T_SUB(x)       ( ( (x) & P_SUB ) == SUB )

#define  P_SUB_1         ( P_CVARCOMPL | F_ARITY )
#define  SUB_1           ( SET_ARITY(VARCON( _SUB  ) , 1 ))

#define  P_SUB_2         ( P_CVARCOMPL | F_ARITY )
#define  SUB_2           ( SET_ARITY(VARCON( _SUB  ) , 2 ))

#define  _CAP           4 
#define  P_CAP          P_CVARCOMPL
#define  CAP            VARCON( _CAP  )
#define  T_CAP(x)       ( ( (x) & P_CAP ) == CAP )

#define  _SUBPOST       5
#define  P_SUBPOST      P_CVARCOMPL
#define  SUBPOST        VARCON( _SUBPOST  )
#define  T_SUBPOST(x)   ( ( (x) & P_SUBPOST ) == SUBPOST )

#define  _LIST          6
#define  P_LIST          P_VARCOMPL
#define  LIST           VARCON( _LIST  )
#define  T_LIST(x)      ( ( (x) & P_LIST ) == LIST )

#define  _MATRIX          7
#define  P_MATRIX         P_CVARCOMPL
#define  MATRIX           VARCON( _MATRIX  )
#define  T_MATRIX(x)      ( ( (x) & P_MATRIX ) == MATRIX )

#define  _PM_SWITCH          8
#define  P_PM_SWITCH          P_VARCOMPL
#define  PM_SWITCH           VARCON( _PM_SWITCH  )
#define  T_PM_SWITCH(x)      ( ( (x) & P_PM_SWITCH ) == PM_SWITCH )

#define  _LREC          9                                              /* CS */
#define  P_LREC         P_CVARCOMPL                                    /* CS */
#define  LREC           VARCON( _LREC  )                               /* CS */
#define  T_LREC(x)      ( ( (x) & P_LREC ) == LREC )                   /* CS */

#define  _PM_AS          10
#define  P_PM_AS         P_CVARCOMPL
#define  PM_AS           VARCON( _PM_AS  )
#define  T_PM_AS(x)      ( ( (x) & P_PM_AS ) == PM_AS )

#define  _PM_LIST          11
#define  P_PM_LIST         P_CVARCOMPL
#define  PM_LIST           VARCON( _PM_LIST  )
#define  T_PM_LIST(x)      ( ( (x) & P_PM_LIST ) == PM_LIST )

#define  _PM_REC          12
#define  P_PM_REC         P_CVARCOMPL
#define  PM_REC           VARCON( _PM_REC  )
#define  T_PM_REC(x)      ( ( (x) & P_PM_REC ) == PM_REC )
#define  _PM_NCASE          13
#define  P_PM_NCASE         P_CVARCOMPL
#define  PM_NCASE           VARCON( _PM_NCASE  )
#define  T_PM_NCASE(x)      ( ( (x) & P_PM_NCASE ) == PM_NCASE )

#define  _PM_FCASE          14
#define  P_PM_FCASE         P_CVARCOMPL
#define  PM_FCASE           VARCON( _PM_FCASE  )
#define  T_PM_FCASE(x)      ( ( (x) & P_PM_FCASE ) == PM_FCASE )

#define  _PM_ECASE          15
#define  P_PM_ECASE         P_CVARCOMPL
#define  PM_ECASE           VARCON( _PM_ECASE  )
#define  T_PM_ECASE(x)      ( ( (x) & P_PM_ECASE ) == PM_ECASE )

#define T_PM_CASE(x)        (  T_PM_SWITCH(x) || T_PM_NCASE(x) || \
                            T_PM_FCASE(x) || T_PM_ECASE(x) || T_PM_REC(x)   )
/*********************************************************************/
/*  Value - Belegungen fuer Single Atoms                             */
/*********************************************************************/

#define  _STF1        1
#define  _STF2        2
#define  _STF3        3
#define  _STF4        4
#define  _STF5        5
#define  _STF6        6
#define  _STF7        7
#define  _STF8        8

#define  DYFUNC        10
#define  MONFUNC       30
#define  CFUNC         40
#define  IPFUNC        60
#define  CPFUNC        80
#define  QUERY         95
#define  SPEC_FUNC    120
#define  COOR_FUNC    150

#define P_FUNCCOMPL   ( F_FUNC_NAME | P_FUNC | F_FUNC_ARITY )
#define P_CONSTCOMPL  ( F_FUNC_GROUP | F_FUNC_NAME | P_CONSTANT   \
                       | F_NUM   )

/*********************************************************************/
/*  Erzeugt die Bitdarstellung fuer eine Funktion 'id'               */
/*  der Stelligkeit 'ar'                                             */
/*********************************************************************/

#define  SA_F(id,ar)   \
          ( FUNC | ((ar) << O_FUNC_ARITY) | ((id) << O_FUNC_NAME) )

/*********************************************************************/
/*  Liefert den Wert des Namenfeldes einer Funktion                  */
/*  Bsp.: FNAME(CUTFIRST)   -->  120                                 */
/*********************************************************************/

#define  FNAME(x)   \
          ( ((x) & F_FUNC_NAME  ) >> O_FUNC_NAME  )

/*********************************************************************/
/*  Liefert den Wert des Gruppenfeldes fuer eine Funktion.           */

/*  Bsp.: GROUP(TRANSPOSE)  -->  1                                   */
/*********************************************************************/

#define  GROUP(x)   \
          ( ((x) & F_FUNC_GROUP ) >> O_FUNC_GROUP )

/*********************************************************************/
/*  Erzeugt ein single atom fuer eine Konstante mit dem Namen 'x'    */
/*  als Stackelement                                                 */
/*********************************************************************/

#define  SA_C(x)   \
          ( NORMAL_CONST | ((x)<<O_VALUE) )




/*********************************************************************/
/*  next FunctionConstant is  --- 40 ---                             */
/*********************************************************************/


/*********************************************************************/
/*  Dyadische Funktionen                                             */
/*********************************************************************/

#define   P_D_PLUS      P_FUNCCOMPL
#define   _D_PLUS       1
#define   D_PLUS        SA_F(_D_PLUS ,2)
#define   T_D_PLUS(x)   ( ( (x) & P_D_PLUS ) == D_PLUS )

#define   P_D_MINUS     P_FUNCCOMPL
#define   _D_MINUS      2
#define   D_MINUS       SA_F(_D_MINUS ,2)
#define   T_D_MINUS(x)  ( ( (x) & P_D_MINUS ) == D_MINUS )

#define   P_D_MULT      P_FUNCCOMPL
#define   _D_MULT       3
#define   D_MULT        SA_F(_D_MULT ,2)
#define   T_D_MULT(x)   ( ( (x) & P_D_MULT ) == D_MULT )

#define   P_D_DIV       P_FUNCCOMPL
#define   _D_DIV        4
#define   D_DIV         SA_F(_D_DIV ,2)
#define   T_D_DIV(x)    ( ( (x) & P_D_DIV ) == D_DIV )

#define   P_D_MOD       P_FUNCCOMPL
#define   _D_MOD        5
#define   D_MOD         SA_F(_D_MOD ,2)
#define   T_D_MOD(x)    ( ( (x) & P_D_MOD ) == D_MOD )

#define   P_D_OR         P_FUNCCOMPL
#define   _D_OR         6
#define   D_OR          SA_F(_D_OR ,2)
#define   T_D_OR(x)     ( ( (x) & P_D_OR ) == D_OR )

#define   P_D_AND        P_FUNCCOMPL
#define   _D_AND        7
#define   D_AND         SA_F(_D_AND ,2)
#define   T_D_AND(x)    ( ( (x) & P_D_AND ) == D_AND )

#define   P_D_XOR       P_FUNCCOMPL
#define   _D_XOR        8
#define   D_XOR         SA_F(_D_XOR ,2)
#define   T_D_XOR(x)    ( ( (x) & P_D_XOR ) == D_XOR )

#define   P_D_EQ         P_FUNCCOMPL
#define   _D_EQ         9
#define   D_EQ          SA_F(_D_EQ ,2)
#define   T_D_EQ(x)     ( ( (x) & P_D_EQ ) == D_EQ )

#define   P_D_NEQ        P_FUNCCOMPL
#define   _D_NEQ        10
#define   D_NEQ         SA_F(_D_NEQ ,2)
#define   T_D_NEQ(x)    ( ( (x) & P_D_NEQ ) == D_NEQ )

#define   P_D_LESS       P_FUNCCOMPL
#define   _D_LESS       11
#define   D_LESS        SA_F(_D_LESS ,2)
#define   T_D_LESS(x)   ( ( (x) & P_D_LESS ) == D_LESS )

#define   P_D_LEQ        P_FUNCCOMPL
#define   _D_LEQ        12
#define   D_LEQ         SA_F(_D_LEQ ,2)
#define   T_D_LEQ(x)    ( ( (x) & P_D_LEQ ) == D_LEQ )

#define   P_D_GREAT      P_FUNCCOMPL
#define   _D_GREAT      13
#define   D_GREAT       SA_F(_D_GREAT ,2)
#define   T_D_GREAT(x)  ( ( (x) & P_D_GREAT ) == D_GREAT )

#define   P_D_GEQ        P_FUNCCOMPL
#define   _D_GEQ        14
#define   D_GEQ         SA_F(_D_GEQ ,2)
#define   T_D_GEQ(x)    ( ( (x) & P_D_GEQ ) == D_GEQ )

#define   P_D_MAX       P_FUNCCOMPL
#define   _D_MAX        15
#define   D_MAX         SA_F(_D_MAX ,2)
#define   T_D_MAX(x)    ( ( (x) & P_D_MAX ) == D_MAX )

#define   P_D_MIN       P_FUNCCOMPL
#define   _D_MIN        16
#define   D_MIN         SA_F(_D_MIN ,2)
#define   T_D_MIN(x)    ( ( (x) & P_D_MIN ) == D_MIN )

/*********************************************************************/
/*  Monadische Funktionen                                            */
/*********************************************************************/

#define   P_M_NOT       P_FUNCCOMPL
#define   _M_NOT        17
#define   M_NOT          SA_F (_M_NOT ,1)
#define   T_M_NOT(x)    ( ( (x) & P_M_NOT ) == M_NOT )

#define   P_M_ABS       P_FUNCCOMPL
#define   _M_ABS        18
#define   M_ABS          SA_F (_M_ABS ,1)
#define   T_M_ABS(x)    ( ( (x) & P_M_ABS ) == M_ABS )

#define   P_M_NEG       P_FUNCCOMPL
#define   _M_NEG        53
#define   M_NEG          SA_F (_M_NEG ,1)
#define   T_M_NEG(x)    ( ( (x) & P_M_NEG ) == M_NEG )

#define   P_M_TRUNCATE  P_FUNCCOMPL
#define   _M_TRUNCATE   19
#define   M_TRUNCATE     SA_F (_M_TRUNCATE ,1)
#define   T_M_TRUNCATE(x) ( ( (x) & P_M_TRUNCATE ) == M_TRUNCATE )

#define   P_M_FLOOR     P_FUNCCOMPL
#define   _M_FLOOR      20
#define   M_FLOOR       SA_F(_M_FLOOR ,1)
#define   T_M_FLOOR(x)  ( ( (x) & P_M_FLOOR ) == M_FLOOR )

#define   P_M_CEIL      P_FUNCCOMPL
#define   _M_CEIL       21
#define   M_CEIL        SA_F(_M_CEIL ,1)
#define   T_M_CEIL(x)   ( ( (x) & P_M_CEIL ) == M_CEIL )

#define   P_M_FRAC      P_FUNCCOMPL
#define   _M_FRAC       22
#define   M_FRAC        SA_F(_M_FRAC ,1)
#define   T_M_FRAC(x)   ( ( (x) & P_M_FRAC ) == M_FRAC )

#define   P_M_EMPTY      P_FUNCCOMPL
#define   _M_EMPTY       23
#define   M_EMPTY       SA_F(_M_EMPTY ,1)
#define   T_M_EMPTY(x)  ( ( (x) & P_M_EMPTY ) == M_EMPTY )
/* struck */
#define   P_M_SIN      P_FUNCCOMPL
#define   _M_SIN       80
#define   M_SIN       SA_F(_M_SIN ,1)
#define   T_M_SIN(x)  ( ( (x) & P_M_SIN ) == M_SIN )

#define   P_M_COS      P_FUNCCOMPL
#define   _M_COS       81
#define   M_COS       SA_F(_M_COS ,1)
#define   T_M_COS(x)  ( ( (x) & P_M_COS ) == M_COS )

#define   P_M_TAN      P_FUNCCOMPL
#define   _M_TAN       82
#define   M_TAN       SA_F(_M_TAN ,1)
#define   T_M_TAN(x)  ( ( (x) & P_M_TAN ) == M_TAN )

#define   P_M_LN      P_FUNCCOMPL
#define   _M_LN       83
#define   M_LN       SA_F(_M_LN ,1)
#define   T_M_LN(x)  ( ( (x) & P_M_LN ) == M_LN )

#define   P_M_EXP      P_FUNCCOMPL
#define   _M_EXP       84
#define   M_EXP       SA_F(_M_EXP ,1)
#define   T_M_EXP(x)  ( ( (x) & P_M_EXP ) == M_EXP )



/*********************************************************************/
/*  Inneres Produkt                                                  */
/*********************************************************************/

#define   P_IP          P_FUNCCOMPL
#define   _IP           24
#define   IP            SA_F (_IP,2)
#define   T_IP_(x)      ( ( (x) & P_IP ) == IP )

/*********************************************************************/
/*  Cartesisches Produkt                                             */
/*********************************************************************/

/*********************************************************************/
/* Queries                                                           */
/*********************************************************************/

#define   P_CLASS       P_FUNCCOMPL
#define   _CLASS        25
#define   CLASS         SA_F (_CLASS ,1)
#define   T_CLASS(x)    ( ( (x) & P_CLASS ) == CLASS )

#define   P_TYPE         P_FUNCCOMPL
#define   _TYPE         26
#define   TYPE          SA_F (_TYPE ,1)
#define   T_TYPE(x)     ( ( (x) & P_TYPE ) == TYPE )

#define   P_DIMENSION    P_FUNCCOMPL
#define   _DIMENSION    28
#define   DIMENSION     SA_F (_DIMENSION ,2)
#define   T_DIMENSION(x) (( (x) & P_DIMENSION ) == DIMENSION )

#define   P_VDIMENSION    P_FUNCCOMPL
#define   _VDIMENSION    59
#define   VDIMENSION     SA_F (_VDIMENSION ,1)
#define   T_VDIMENSION(x) (( (x) & P_VDIMENSION ) == VDIMENSION )

#define   P_MDIMENSION    P_FUNCCOMPL
#define   _MDIMENSION    60
#define   MDIMENSION     SA_F (_MDIMENSION ,2)
#define   T_MDIMENSION(x) (( (x) & P_MDIMENSION ) == MDIMENSION )

#define   P_LDIMENSION    P_FUNCCOMPL
#define   _LDIMENSION    77
#define   LDIMENSION     SA_F (_LDIMENSION ,1)
#define   T_LDIMENSION(x) (( (x) & P_LDIMENSION ) == LDIMENSION )


/*********************************************************************/
/*  Strukturierungsfunktionen                                        */
/*********************************************************************/

/*********************************************************************/
/* Group 1                                                           */
/*********************************************************************/

#define   P_TRANSPOSE   P_FUNCCOMPL
#define   _TRANSPOSE    29
#define   TRANSPOSE     SA_F (_TRANSPOSE ,1)
#define   T_TRANSPOSE(x)        ( ( (x) & P_TRANSPOSE ) == TRANSPOSE )

#define   P_REVERSE   P_FUNCCOMPL
#define   _REVERSE    75
#define   REVERSE     SA_F (_REVERSE ,1)
#define   T_REVERSE(x)        ( ( (x) & P_REVERSE ) == REVERSE )


/*********************************************************************/
/* Group 2                                                           */
/*********************************************************************/

#define   P_CUT         P_FUNCCOMPL
#define   _CUT          30
#define   CUT           SA_F (_CUT ,3)
#define   T_CUT(x)      ( ( (x) & P_CUT ) == CUT )

#define   P_VCUT         P_FUNCCOMPL
#define   _VCUT          54
#define   VCUT           SA_F (_VCUT ,2)
#define   T_VCUT(x)      ( ( (x) & P_VCUT ) == VCUT )

#define   P_MCUT         P_FUNCCOMPL
#define   _MCUT          85
#define   MCUT           SA_F (_MCUT ,3)
#define   T_MCUT(x)      ( ( (x) & P_MCUT ) == MCUT )

#define   P_LTRANSFORM    P_FUNCCOMPL
#define   _LTRANSFORM     76
#define   LTRANSFORM      SA_F (_LTRANSFORM ,3)
#define   T_LTRANSFORM(x) ( ( (x) & P_LTRANSFORM ) == LTRANSFORM )

#define   P_ROTATE      P_FUNCCOMPL
#define   _ROTATE       32
#define   ROTATE        SA_F (_ROTATE ,3)
#define   T_ROTATE(x)   ( ( (x) & P_ROTATE ) == ROTATE )

#define   P_VROTATE      P_FUNCCOMPL
#define   _VROTATE       55
#define   VROTATE        SA_F (_VROTATE ,2)
#define   T_VROTATE(x)   ( ( (x) & P_VROTATE ) == VROTATE )

#define   P_MROTATE      P_FUNCCOMPL
#define   _MROTATE       87
#define   MROTATE        SA_F (_MROTATE ,3)
#define   T_MROTATE(x)   ( ( (x) & P_MROTATE ) == MROTATE )

#define   P_SELECT      P_FUNCCOMPL
#define   _SELECT       33
#define   SELECT        SA_F (_SELECT ,3)
#define   T_SELECT(x)   ( ( (x) & P_SELECT ) == SELECT )

#define   P_VSELECT      P_FUNCCOMPL
#define   _VSELECT       56
#define   VSELECT        SA_F (_VSELECT ,2)
#define   T_VSELECT(x)   ( ( (x) & P_VSELECT ) == VSELECT )

#define   P_MSELECT      P_FUNCCOMPL
#define   _MSELECT       86
#define   MSELECT        SA_F (_MSELECT ,3)
#define   T_MSELECT(x)   ( ( (x) & P_MSELECT ) == MSELECT )

#define   P_SUBSTR      P_FUNCCOMPL
#define   _SUBSTR       61
#define   SUBSTR        SA_F (_SUBSTR ,3)
#define   T_SUBSTR(x)   ( ( (x) & P_SUBSTR ) == SUBSTR )

#define   P_LCUT         P_FUNCCOMPL
#define   _LCUT          70
#define   LCUT           SA_F (_LCUT ,2)
#define   T_LCUT(x)      ( ( (x) & P_LCUT ) == LCUT )

#define   P_TRANSFORM    P_FUNCCOMPL
#define   _TRANSFORM     31
#define   TRANSFORM      SA_F (_TRANSFORM ,1)
#define   T_TRANSFORM(x) ( ( (x) & P_TRANSFORM ) == TRANSFORM )

#define   P_LROTATE      P_FUNCCOMPL
#define   _LROTATE       72
#define   LROTATE        SA_F (_LROTATE ,2)
#define   T_LROTATE(x)   ( ( (x) & P_LROTATE ) == LROTATE )

#define   P_LSELECT      P_FUNCCOMPL
#define   _LSELECT       71
#define   LSELECT        SA_F (_LSELECT ,2)
#define   T_LSELECT(x)   ( ( (x) & P_LSELECT ) == LSELECT )

/* Dietmar */
#define   P_MKLIST       P_FUNCCOMPL
#define   _MKLIST        78
#define   MKLIST         SA_F (_MKLIST ,2)
#define   T_MKLIST(x)   ( ( (x) & P_MKLIST ) == MKLIST )
/* Dietmar */

/*********************************************************************/
/* Group 3                                                           */
/*********************************************************************/

#define   P_LREPLACE      P_FUNCCOMPL
#define   _LREPLACE       74
#define   LREPLACE        SA_F (_LREPLACE ,3)
#define   T_LREPLACE(x)   ( ( (x) & P_LREPLACE ) == LREPLACE )

#define   P_REPSTR        P_FUNCCOMPL
#define   _REPSTR         90                        /* replace sub-structure */
#define   REPSTR          SA_F (_REPSTR ,4)
#define   T_REPSTR(x)     ( ( (x) & P_REPSTR ) == REPSTR ) 

#define   P_REPLACE      P_FUNCCOMPL
#define   _REPLACE       34
#define   REPLACE        SA_F (_REPLACE ,4)
#define   T_REPLACE(x)   ( ( (x) & P_REPLACE ) == REPLACE )

#define   P_MREPLACE_C      P_FUNCCOMPL
#define   _MREPLACE_C       35
#define   MREPLACE_C        SA_F (_MREPLACE_C ,3)
#define   T_MREPLACE_C(x)   ( ( (x) & P_MREPLACE_C ) == MREPLACE_C )

#define   P_MREPLACE_R      P_FUNCCOMPL
#define   _MREPLACE_R       36
#define   MREPLACE_R        SA_F (_MREPLACE_R ,3)
#define   T_MREPLACE_R(x)   ( ( (x) & P_MREPLACE_R ) == MREPLACE_R )

#define   P_VREPLACE      P_FUNCCOMPL
#define   _VREPLACE       57
#define   VREPLACE        SA_F (_VREPLACE ,3)
#define   T_VREPLACE(x)   ( ( (x) & P_VREPLACE ) == VREPLACE )

#define   P_MREPLACE      P_FUNCCOMPL
#define   _MREPLACE       89
#define   MREPLACE        SA_F (_MREPLACE ,4)
#define   T_MREPLACE(x)   ( ( (x) & P_MREPLACE ) == MREPLACE )

/*********************************************************************/
/* Group 4                                                           */
/*********************************************************************/

#define   P_UNI_VECTOR   P_FUNCCOMPL
#define   _UNI_VECTOR    35
#define   UNI_VECTOR     SA_F (_UNI_VECTOR ,2)
#define   T_UNI_VECTOR(x)       ( ( (x) & P_UNI_VECTOR ) == UNI_VECTOR )

/*********************************************************************/
/* Group 5                                                           */
/*********************************************************************/

#define   P_UNI_MATRIX   P_FUNCCOMPL
#define   _UNI_MATRIX    36
#define   UNI_MATRIX     SA_F (_UNI_MATRIX,2)
#define   T_UNI_MATRIX(x)       ( ( (x) & P_UNI_MATRIX ) == UNI_MATRIX )

/*********************************************************************/
/* Group 6                                                           */
/*********************************************************************/

#define   P_UNI_TREE     P_FUNCCOMPL
#define   _UNI_TREE      37
#define   UNI_TREE       SA_F (_UNI_TREE ,2)
#define   T_UNI_TREE(x)  ( ( (x) & P_UNI_TREE ) == UNI_TREE )

/*********************************************************************/
/* Group 7                                                           */
/*********************************************************************/

#define   P_UNI       P_FUNCCOMPL
#define   _UNI        38
#define   UNI         SA_F (_UNI ,3)
#define   T_UNI(x)      ( ( (x) & P_UNI ) == UNI )

#define   P_VUNI       P_FUNCCOMPL
#define   _VUNI        58
#define   VUNI         SA_F (_VUNI ,2)
#define   T_VUNI(x)      ( ( (x) & P_VUNI ) == VUNI )

#define   P_MUNI       P_FUNCCOMPL
#define   _MUNI        88
#define   MUNI         SA_F (_MUNI ,3)
#define   T_MUNI(x)      ( ( (x) & P_MUNI ) == MUNI )

#define   P_LUNI       P_FUNCCOMPL
#define   _LUNI        73
#define   LUNI         SA_F (_LUNI ,2)
#define   T_LUNI(x)      ( ( (x) & P_LUNI ) == LUNI )

/*********************************************************************/
/* Group 8                                                           */
/*********************************************************************/

#define   P_TAKE        P_FUNCCOMPL
#define   _TAKE         39
#define   TAKE          SA_F (_TAKE ,4)
#define   T_TAKE(x)     ( ( (x) & P_TAKE ) == TAKE )

#define   P_DROP        P_FUNCCOMPL
#define   _DROP         40
#define   DROP          SA_F (_DROP ,4)
#define   T_DROP(x)     ( ( (x) & P_DROP ) == DROP )

/*********************************************************************/
/* Konversionsfunktionen                                             */
/*********************************************************************/

#define   P_TO_VECT        _FUNCCOMPL
#define   _TO_VECT         64
#define   TO_VECT          SA_F (_TO_VECT ,1)
#define   T_TO_VECT(x)     ( ( (x) & P_TO_VECT ) == TO_VECT )

#define   P_TO_TVECT        _FUNCCOMPL
#define   _TO_TVECT         65
#define   TO_TVECT          SA_F (_TO_TVECT ,1)
#define   T_TO_TVECT(x)     ( ( (x) & P_TO_TVECT ) == TO_TVECT )

#define   P_TO_MAT        _FUNCCOMPL
#define   _TO_MAT         66
#define   TO_MAT          SA_F (_TO_MAT ,1)
#define   T_TO_MAT(x)     ( ( (x) & P_TO_MAT ) == TO_MAT )

#define   P_TO_SCAL        _FUNCCOMPL
#define   _TO_SCAL         67
#define   TO_SCAL          SA_F (_TO_SCAL ,1)
#define   T_TO_SCAL(x)     ( ( (x) & P_TO_SCAL ) == TO_SCAL )

#define   P_TO_LIST        _FUNCCOMPL
#define   _TO_LIST         91
#define   TO_LIST          SA_F (_TO_LIST ,1)
#define   T_TO_LIST(x)     ( ( (x) & P_TO_LIST ) == TO_LIST )

#define   P_TO_FIELD       _FUNCCOMPL
#define   _TO_FIELD        92
#define   TO_FIELD         SA_F (_TO_FIELD ,1)
#define   T_TO_FIELD(x)    ( ( (x) & P_TO_FIELD ) == TO_FIELD )
 
#define   P_MK_STRING      _FUNCCOMPL
#define   _MK_STRING       94
#define   MK_STRING        SA_F(_MK_STRING , 1)
#define   T_MK_STRING(x)   ( ( (x) & P_MK_STRING ) == MK_STRING )

/*********************************************************************/
/*  entlang einer Koordinate                                         */
/*********************************************************************/

#define   P_C_MAX       P_FUNCCOMPL
#define   _C_MAX        45
#define   C_MAX         SA_F(_C_MAX,2)
#define   T_C_MAX(x)    ( ( (x) & P_C_MAX) == C_MAX)

#define   P_VC_MAX       P_FUNCCOMPL
#define   _VC_MAX        39
#define   VC_MAX         SA_F(_VC_MAX,1)
#define   T_VC_MAX(x)    ( ( (x) & P_VC_MAX) == VC_MAX)

#define   P_C_MIN       P_FUNCCOMPL
#define   _C_MIN        46
#define   C_MIN         SA_F(_C_MIN,2)
#define   T_C_MIN(x)    ( ( (x) & P_C_MIN) == C_MIN)

#define   P_VC_MIN       P_FUNCCOMPL
#define   _VC_MIN        40
#define   VC_MIN         SA_F(_VC_MIN,1)
#define   T_VC_MIN(x)    ( ( (x) & P_VC_MIN) == VC_MIN)

#define   P_C_PLUS       P_FUNCCOMPL
#define   _C_PLUS        47
#define   C_PLUS         SA_F(_C_PLUS,2)
#define   T_C_PLUS(x)   ( ( (x) & P_C_PLUS) == C_PLUS)

#define   P_VC_PLUS       P_FUNCCOMPL
#define   _VC_PLUS        41
#define   VC_PLUS         SA_F(_VC_PLUS,1)
#define   T_VC_PLUS(x)   ( ( (x) & P_VC_PLUS) == VC_PLUS)

#define   P_C_MINUS       P_FUNCCOMPL
#define   _C_MINUS        48
#define   C_MINUS         SA_F(_C_MINUS,2)
#define   T_C_MINUS(x)  ( ( (x) & P_C_MINUS) == C_MINUS)

#define   P_VC_MINUS       P_FUNCCOMPL
#define   _VC_MINUS        42
#define   VC_MINUS         SA_F(_VC_MINUS,1)
#define   T_VC_MINUS(x)  ( ( (x) & P_VC_MINUS) == VC_MINUS)

#define   P_C_MULT       P_FUNCCOMPL
#define   _C_MULT        49
#define   C_MULT         SA_F(_C_MULT,2)
#define   T_C_MULT(x)   ( ( (x) & P_C_MULT) == C_MULT)

#define   P_VC_MULT       P_FUNCCOMPL
#define   _VC_MULT        43
#define   VC_MULT         SA_F(_VC_MULT,1)
#define   T_VC_MULT(x)   ( ( (x) & P_VC_MULT) == VC_MULT)

#define   P_C_DIV       P_FUNCCOMPL
#define   _C_DIV        50
#define   C_DIV         SA_F(_C_DIV,2)
#define   T_C_DIV(x)    ( ( (x) & P_C_DIV) == C_DIV)

#define   P_VC_DIV       P_FUNCCOMPL
#define   _VC_DIV        44
#define   VC_DIV         SA_F(_VC_DIV,1)
#define   T_VC_DIV(x)    ( ( (x) & P_VC_DIV) == VC_DIV)

/*******************************************************************/
/*  Feldgleichheit und Ungleichheit                                */
/*******************************************************************/

#define   P_F_EQ       P_FUNCCOMPL
#define   _F_EQ        51
#define   F_EQ         SA_F(_F_EQ,2)
#define   T_F_EQ(x)   ( ( (x) & P_F_EQ) == F_EQ)

#define   P_F_NE       P_FUNCCOMPL
#define   _F_NE        52
#define   F_NE         SA_F(_F_NE,2)
#define   T_F_NE(x)    ( ( (x) & P_F_NE) == F_NE)

/*********************************************************************/
/*  Verteilungsfunkion fuers verteilte System                        */
/*********************************************************************/

#define   _NORM          0x00040F08L

#define   P_DIS$ON       P_FUNCCOMPL
#define   _DIS$ON        0x5D
#define   DIS$ON         SA_F( _DIS$ON, 3 )
#define   T_DIS$ON(x)    ( ( (x) & P_DIS$ON ) == DIS$ON )

/*********************************************************************/
/*                                                                   */
/* Sonderzeichen   und  Typen                                        */
/*                                                                   */
/*********************************************************************/
/* Klasse  0                                                         */
/*********************************************************************/

#define   P_DOL_CONST   P_CONSTCOMPL
#define   _DOL_CONST    0 | (1 <<8)
#define   DOL_CONST     SA_C ( _DOL_CONST )
#define   T_DOL_CONST(x) ( ( (x) & P_DOL_CONST ) == DOL_CONST )

#define   P_PLACEH      P_CONSTCOMPL
#define   _PLACEH       0 | (2<<8)            /* Platzhalter        */
#define   PLACEH        SA_C ( _PLACEH )
#define   T_PLACEH(x)   ( ( (x) & P_PLACEH ) == PLACEH )

#define   P_HASH        P_CONSTCOMPL
#define   _HASH         0 | (3<<8)
#define   HASH          SA_C ( _HASH )
#define   T_HASH(x)     ( ( (x) & P_HASH ) == HASH )

#define   P_KLAA        P_CONSTCOMPL
#define   _KLAA         0 | (4<<8)            /* KLAmmerAffe @      */
#define   KLAA          SA_C ( _KLAA )
#define   T_KLAA(x)     ( ( (x) & P_KLAA ) == KLAA )

#define   P_NOVAL        P_CONSTCOMPL
#define   _NOVAL         0 | (5<<8)
#define   NOVAL          SA_C ( _NOVAL )
#define   T_NOVAL(x)     ( ( (x) & P_NOVAL ) == NOVAL )

#define   P_ENDE         P_CONSTCOMPL
#define   _ENDE          0 | (6<<8)
#define   ENDE           SA_C ( _ENDE )
#define   T_ENDE(x)      ( ( (x) & P_ENDE ) == ENDE )

/*********************************************************************/
/* Klasse  1                                                         */
/*********************************************************************/

#define   P_ESET        P_CONSTCOMPL
#define   _ESET         1 | (1<<8)
#define   ESET          SA_C ( _ESET )
#define   T_ESET(x)     ( ( (x) & P_ESET ) == ESET )

#define   P_NIL         P_CONSTCOMPL
#define   _NIL          1 | (2<<8)
#define   NIL           SA_C ( _NIL )
#define   T_NIL(x)      ( ( (x) & P_NIL ) == NIL )

#define   P_NILMAT    P_CONSTCOMPL
#define   _NILMAT     1 | (3<<8)
#define   NILMAT      SA_C ( _NILMAT )
#define   T_NILMAT(x) ( ( (x) & P_NILMAT ) == NILMAT )

#define   P_NILVECT    P_CONSTCOMPL
#define   _NILVECT     1 | (4<<8)
#define   NILVECT      SA_C ( _NILVECT )
#define   T_NILVECT(x) ( ( (x) & P_NILVECT ) == NILVECT )

#define   P_NILTVECT    P_CONSTCOMPL
#define   _NILTVECT     1 | (5<<8)
#define   NILTVECT      SA_C ( _NILTVECT )
#define   T_NILTVECT(x) ( ( (x) & P_NILTVECT ) == NILTVECT )

#define   P_NILSTRING  P_CONSTCOMPL
#define   _NILSTRING    1 | (6<<8)
#define   NILSTRING     SA_C ( _NILSTRING )
#define   T_NILSTRING(x) ( ( (x) & P_NILSTRING ) == NILSTRING )

/*********************************************************************/
/* Klasse  2                                                         */
/*********************************************************************/

#define   P_CL_FUNCTION  P_CONSTCOMPL
#define   _CL_FUNCTION   2 | (1<<8)
#define   CL_FUNCTION    SA_C ( _CL_FUNCTION )
#define   T_CL_FUNCTION(x) ( ( (x) & P_CL_FUNCTION ) == CL_FUNCTION )

#define   P_CL_SCALAR   P_CONSTCOMPL
#define   _CL_SCALAR    2 | (2<<8)
#define   CL_SCALAR     SA_C ( _CL_SCALAR )
#define   T_CL_SCALAR(x)        ( ( (x) & P_CL_SCALAR ) == CL_SCALAR )

#define   P_CL_VECTOR   P_CONSTCOMPL
#define   _CL_VECTOR    2 | (3<<8)
#define   CL_VECTOR     SA_C ( _CL_VECTOR )
#define   T_CL_VECTOR(x)        ( ( (x) & P_CL_VECTOR ) == CL_VECTOR )

#define   P_CL_TVECTOR  P_CONSTCOMPL
#define   _CL_TVECTOR   2 | (4<<8)
#define   CL_TVECTOR    SA_C ( _CL_TVECTOR )
#define   T_CL_TVECTOR(x)       ( ( (x) & P_CL_TVECTOR ) == CL_TVECTOR )

#define   P_CL_MATRIX   P_CONSTCOMPL
#define   _CL_MATRIX    2 | (5<<8)
#define   CL_MATRIX     SA_C ( _CL_MATRIX )
#define   T_CL_MATRIX(x)        ( ( (x) & P_CL_MATRIX ) == CL_MATRIX )

#define   P_CL_SET      P_CONSTCOMPL
#define   _CL_SET       2 | (6<<8)
#define   CL_SET        SA_C ( _CL_SET )
#define   T_CL_SET(x)   ( ( (x) & P_CL_SET ) == CL_SET )

#define   P_CL_TREE     P_CONSTCOMPL
#define   _CL_TREE      2 | (7<<8)
#define   CL_TREE       SA_C ( _CL_TREE )
#define   T_CL_TREE(x)  ( ( (x) & P_CL_TREE ) == CL_TREE )

#define   P_CL_DVECTOR  P_CONSTCOMPL
#define   _CL_DVECTOR   2 | (8<<8)
#define   CL_DVECTOR    SA_C ( _CL_DVECTOR )
#define   T_CL_DVECTOR(x)       ( ( (x) & P_CL_DVECTOR ) == CL_DVECTOR )

#define   P_CL_CVECTOR  P_CONSTCOMPL
#define   _CL_CVECTOR   2 | (9<<8)
#define   CL_CVECTOR    SA_C ( _CL_CVECTOR )
#define   T_CL_CVECTOR(x)       ( ( (x) & P_CL_CVECTOR ) == CL_CVECTOR )

/*********************************************************************/
/* Klasse  3                                                         */
/*********************************************************************/

#define   P_BOOL        P_CONSTCOMPL
#define   _BOOL         3 | (1<<8)
#define   BOOL          SA_C ( _BOOL )
#define   T_BOOL(x)     ( ( (x) & P_BOOL ) == BOOL )


#define   P_DECIMAL     P_CONSTCOMPL
#define   _DECIMAL      3 | (4<<8)
#define   DECIMAL       SA_C ( _DECIMAL )
#define   T_DECIMAL(x)  ( ( (x) & P_DECIMAL ) == DECIMAL )

#define   P_CHAR        P_CONSTCOMPL
#define   _CHAR         3 | (5<<8)
#define   CHAR          SA_C ( _CHAR )
#define   T_CHAR(x)     ( ( (x) & P_CHAR ) == CHAR )

#define   P_LAMBDA      P_CONSTCOMPL
#define   _LAMBDA       3 | (6<<8)
#define   LAMBDA        SA_C ( _LAMBDA)
#define   T_LAMBDA(x)   ( ( (x) & P_LAMBDA) == LAMBDA)

#define   P_CONDITIONAL  P_CONSTCOMPL
#define   _CONDITIONAL   3 | (7<<8)
#define   CONDITIONAL    SA_C ( _CONDITIONAL )
#define   T_CONDITIONAL(x) ( ( (x) & P_CONDITIONAL ) == CONDITIONAL )

#define   P_PRIMFUNC          P_CONSTCOMPL
#define   _PRIMFUNC           3 | (8<<8)
#define   PRIMFUNC            SA_C ( _PRIMFUNC )
#define   T_PRIMFUNC(x) ( ( (x) & P_PRIMFUNC ) == PRIMFUNC )

#define   P_TMATRIX          P_CONSTCOMPL
#define   _TMATRIX           3 | (9<<8)
#define   TMATRIX            SA_C ( _TMATRIX )
#define   T_TMATRIX(x) ( ( (x) & P_TMATRIX ) == TMATRIX )

#define   P_EXPR          P_CONSTCOMPL
#define   _EXPR           3 | (10<<8)
#define   EXPR            SA_C ( _EXPR )
#define   T_EXPR(x)       ( ( (x) & P_EXPR ) == EXPR )

#define   P_USERFUNC          P_CONSTCOMPL
#define   _USERFUNC           3 | (11<<8)
#define   USERFUNC            SA_C ( _USERFUNC )
#define   T_USERFUNC(x)       ( ( (x) & P_USERFUNC ) == USERFUNC )

/*****************************************************************************/
/* Klasse  4                                                                 */
/*****************************************************************************/

/*********************************************************************/
/* Klasse  5                                                         */
/*********************************************************************/

#define   P_SA_FALSE    P_CONSTCOMPL
#define   _SA_FALSE     5 | (1<<8)
#define   SA_FALSE      SA_C ( _SA_FALSE )
#define   T_SA_FALSE(x) ( ( (x) & P_SA_FALSE ) == SA_FALSE )

#define   P_SA_TRUE     P_CONSTCOMPL
#define   _SA_TRUE      5 | (2<<8)
#define   SA_TRUE       SA_C ( _SA_TRUE)
#define   T_SA_TRUE(x)  ( ( (x) & P_SA_TRUE ) == SA_TRUE )

#define   P_BOOLEAN        P_CONSTCOMPL
#define   _BOOLEAN         5 | (0<<8)
#define   T_BOOLEAN(x)     ( ( (x) & P_BOOLEAN & ~0x03000000) == SA_C ( _BOOLEAN ) )

/* H */

#define   P_PM_FAIL    P_CONSTCOMPL
#define   _PM_FAIL     5 | (3<<8)
#define   PM_FAIL      SA_C ( _PM_FAIL )
#define   T_PM_FAIL(x) ( ( (x) & P_PM_FAIL ) == PM_FAIL )

#define   P_PM_SUCC     P_CONSTCOMPL
#define   _PM_SUCC      5 | (4<<8)
#define   PM_SUCC       SA_C ( _PM_SUCC)
#define   T_PM_SUCC(x)  ( ( (x) & P_PM_SUCC ) == PM_SUCC )

#define   P_PM_END     P_CONSTCOMPL
#define   _PM_END      5 | (5<<8)
#define   PM_END       SA_C ( _PM_END)
#define   T_PM_END(x)  ( ( (x) & P_PM_END ) == PM_END )


#define   P_PM_SKIP     P_CONSTCOMPL
#define   _PM_SKIP      5 | (6<<8)
#define   PM_SKIP       SA_C ( _PM_SKIP)
#define   T_PM_SKIP(x)  ( ( (x) & P_PM_SKIP ) == PM_SKIP )



#define   P_PM_SKSKIP     P_CONSTCOMPL
#define   _PM_SKSKIP      5 | (7<<8)
#define   PM_SKSKIP       SA_C ( _PM_SKSKIP)
#define   T_PM_SKSKIP(x)  ( ( (x) & P_PM_SKSKIP ) == PM_SKSKIP )

#define   P_PM_DOLLAR     P_CONSTCOMPL
#define   _PM_DOLLAR      5 | (8<<8)
#define   PM_DOLLAR       SA_C ( _PM_DOLLAR)
#define   T_PM_DOLLAR(x)  ( ( (x) & P_PM_DOLLAR ) == PM_DOLLAR )

#define   P_PM_UDC     P_CONSTCOMPL
#define   _PM_UDC      5 | (9<<8)
#define   PM_UDC       SA_C ( _PM_UDC)
#define   T_PM_UDC(x)  ( ( (x) & P_PM_UDC ) == PM_UDC )

#define   P_PM_PT_PLUS     P_CONSTCOMPL
#define   _PM_PT_PLUS      5 | (13<<8)
#define   PM_PT_PLUS       SA_C ( _PM_PT_PLUS)
#define   T_PM_PT_PLUS(x)  ( ( (x) & P_PM_PT_PLUS ) == PM_PT_PLUS )

/*********************************************************************/
/*  Masken fuer Konstruktoren                                        */
/*********************************************************************/

#define P_CFIXCOMPL  ( F_CONSTR_NAME | P_FIX_CONSTR | P_CHAINED_CONSTR)
#define P_CVARCOMPL  ( F_CONSTR_NAME | P_VAR_CONSTR | P_CHAINED_CONSTR)

#define  VAR_CCONSTR    (VAR_CONSTR | CHAINED_CONSTR)
#define  FIX_CCONSTR    (FIX_CONSTR | CHAINED_CONSTR)


/*********************************************************************/
/*  Erzeugt den fixstelligen Konstruktor der Nummer 'nr' und der     */
/*  Stelligkeit 'ar' als Stackelement in val.     ++ CHAINED FIELD   */
/*********************************************************************/

#define  CFIXCONSTR(nr,ar) \
         ( FIX_CCONSTR | ((nr) << O_CONSTR_NAME) )

/*********************************************************************/
/*  Erzeugt den variabel-stelligen Konstruktor der Nummer 'nr'       */
/*  Die Stelligkeit bleibt unberuecksichtigt (auch in der P_Maske)   */
/*********************************************************************/
                                                /* ++ CHAINED FIELD  */
#define  CVARCON(nr)  \
         ( VAR_CCONSTR | ((nr) << O_CONSTR_NAME) )



/*********************************************************************/
/*                                                                   */
/*  F I X - C O N S T R U C T O R S                  (++)                */
/*                                                                   */
/*********************************************************************/

#define  _K_COMM          6
#define  P_K_COMM         P_CFIXCOMPL
#define  K_COMM           CFIXCONSTR( _K_COMM,2 )
#define  T_K_COMM(X)      (( (X) & P_K_COMM) == K_COMM)


#define  _K_ERROR          10
#define  P_K_ERROR         P_CFIXCOMPL
#define  K_ERROR           CFIXCONSTR( _K_ERROR,1)
#define  T_K_ERROR(X)      (( (X) & P_K_ERROR) == K_ERROR)


#define  _K_STOP          11
#define  P_K_STOP         P_CFIXCOMPL
#define  K_STOP           CFIXCONSTR( _K_STOP,1 )
#define  T_K_STOP(X)      (( (X) & P_K_STOP) == K_STOP)



#define  _STRING         0                 /* STRING ist ein var-Konstruktor */
#define  P_STRING        P_CVARCOMPL
#define  STRING          CFIXCONSTR( _STRING,0 )
#define  T_STRING(X)     (( (X) & P_STRING) == STRING)

#define  _PM_IN          1
#define  P_PM_IN         P_CFIXCOMPL
#define  PM_IN           CFIXCONSTR( _PM_IN,2 )
#define  T_PM_IN(X)      (( (X) & P_PM_IN) == PM_IN)


/*********************************************************************/
/*                                                                   */
/*  V A R - C O N S T R U C T O R S                  (++)            */
/*                                                                   */
/*********************************************************************/


#define  _NEXT_VAR            0
#define  P_NEXT_VAR           P_CVARCOMPL
#define  NEXT_VAR             CVARCON( _NEXT_VAR  )
#define  T_NEXT_VAR(x)        ( ( (x) & P_NEXT_VAR ) == NEXT_VAR )

#define  _PM_GUARD            1
#define  P_PM_GUARD           P_CVARCOMPL
#define  PM_GUARD             CVARCON( _PM_GUARD  )
#define  T_PM_GUARD(x)        ( ( (x) & P_PM_GUARD ) == PM_GUARD )

#define  _PM_EAGER            2
#define  P_PM_EAGER           P_CVARCOMPL
#define  PM_EAGER             CVARCON( _PM_EAGER  )
#define  T_PM_EAGER(x)        ( ( (x) & P_PM_EAGER ) == PM_EAGER )

#define  _PM_NOMAT            3
#define  P_PM_NOMAT           P_CVARCOMPL
#define  PM_NOMAT             CVARCON( _PM_NOMAT  )
#define  T_PM_NOMAT(x)        ( ( (x) & P_PM_NOMAT ) == PM_NOMAT )

#define  _PM_EREC             4
#define  P_PM_EREC            P_CVARCOMPL
#define  PM_EREC              VARCON( _PM_EREC  )
#define  T_PM_EREC(x)         ( ( (x) & P_PM_EREC ) == PM_EREC ) 


/* end of stelemdef.h */


