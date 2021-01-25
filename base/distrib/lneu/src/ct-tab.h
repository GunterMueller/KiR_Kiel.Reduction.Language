#if LARGE
extern red_dummy(); 

extern red_plus(); 
extern red_minus();
extern red_mult();
extern red_div();
extern red_mod();
extern red_or();
extern red_and();
extern int red_xor();
extern red_eq();
extern red_neq();
extern red_lt();
extern red_ge();
extern red_le();
extern red_gt();
extern red_min();
extern red_max();
extern red_not();

extern red_f_eq();
extern red_f_ne();
extern red_abs();
extern red_neg();
extern red_trunc();
extern red_floor();
extern red_ceil();
extern red_frac();

extern red_empty();
extern int red_ip();
extern red_class();
extern red_type();
extern red_dim();

extern int red_transpose();
extern int red_transform();
extern int red_cut();        /* uni - function */
extern int red_rotate();     /* uni - function */
extern int red_select();     /* uni - function */
extern int red_unite();      /* uni - function */
extern int red_replace();    /* uni - function */

extern int red_mcut();
extern int red_mreplace();
extern int red_mre_r();      /* replace row    */
extern int red_mre_c();      /* replace column */
extern int red_mrotate();
extern int red_mselect();
extern int red_munite();
extern int red_mdim();

extern int red_c();
extern int red_vc();

extern int red_vcut();
extern int red_vrotate();
extern int red_vselect();
extern int red_vreplace();
extern int red_vunite();
extern int red_vdim();

extern int red_to_mat();
extern int red_to_vect();
extern int red_to_tvect();
extern int red_to_scal();
extern int red_to_field();
extern int red_to_list();
extern int red_mk_string();

extern int red_lcut();
extern int red_ltransform();
extern int red_lselect();
extern int red_lrotate();
extern int red_lunite();
extern int red_lreplace();
extern int red_reverse();
extern int red_ldim();
extern int red_substr();
extern int red_repstr();    /* replace-sub-structure */

extern int red_sin();
extern int red_cos();
extern int red_tan();
extern int red_ln();
extern int red_exp();

#ifdef EAR

FCTPTR  red_func_tab1[100] = 
  { (FCTPTR)0,
    red_plus     ,/* D_PLUS ,   /*   PLUS           1   */
    red_minus    ,/* D_MINUS ,    /* MINUS          2   */
    red_mult     ,/* D_MULT ,     /* MULT           3   */
    red_div      ,/* D_DIV ,      /* DIV            4   */
    red_mod      ,/* D_MOD ,      /* MOD            5   */
    red_or       ,/* D_OR ,       /* OR             6   */
    red_and      ,/* D_AND ,      /* AND            7   */
    red_xor      ,/* D_XOR ,      /* XOR            8   */
    red_eq       ,/* D_EQ ,       /* EQ             9   */
    red_neq      ,/* D_NEQ ,      /* NEQ            10  */
    red_lt       ,/* D_LESS ,     /* LT             11  */
    red_le       ,/* D_LEQ ,      /* LE             12  */
    red_gt       ,/* D_GREAT ,    /* GT             13  */
    red_ge       ,/* D_GEQ ,      /* GEQ            14  */
    red_max      ,/* D_MAX ,      /* MAX            15  */
    red_min      ,/* D_MIN ,      /* MIN            16  */

    red_not      ,/* M_NOT ,      /* NOT            17  */
    red_abs      ,/* M_ABS ,      /* ABS            18  */
    red_trunc    ,/* M_TRUNCATE , /* TRUNC          19  */
    red_floor    ,/* M_FLOOR ,    /* FLOOR          20  */
    red_ceil     ,/* M_CEIL ,     /* CEIL           21  */
    red_frac     ,/* M_FRAC ,     /* FRAC           22  */

    red_empty    ,/* M_EMPTY ,    /* EMPTY          23  */

    red_ip       ,/* IP ,         /* IP             24  */
    red_class    ,/* CLASS ,      /* CLASS          25  */
    red_type     ,/* TYPE ,       /* TYPE           26  */
    /* Feld-Funktionen: */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  27 */
    red_dim      ,/* DIMENSION ,  /* DIMENSION      28  */
    red_transpose,/* TRANSPOSE ,  /* TRANSPOSE      29  */
    red_cut      ,/* CUT ,        /* CUT            30  */
    red_transform,/* TRANSFORM ,  /* TRANSFORM      31  */
    red_rotate   ,/* ROTATE ,     /* ROTATE         32  */
    red_select   ,/* SELECT ,     /* SELECT         33  */
    red_replace  ,/* REPLACE ,    /* ex INSERT      34  */
    red_mre_c    ,/* MREPLACE_C , /* MREPLACE_C     35  */
    red_mre_r    ,/* MREPLACE_R , /* MREPLACE_R     36  */
    red_dummy    ,/* UNI_TREE ,   /* ex UNI_TREE    37  */
    red_unite    ,/* UNI ,        /* UNITE          38  */

    red_vc       ,/* VC_MAX ,     /* VC_MAX         39  */
    red_vc       ,/* VC_MIN ,     /* VC_MIN         40  */
    red_vc       ,/* VC_PLUS ,    /* VC_PLUS        41  */
    red_vc       ,/* VC_MINUS ,   /* VC_MINUS       42  */
    red_vc       ,/* VC_MULT ,    /* VC_MULT        43  */
    red_vc       ,/* VC_DIV ,     /* VC_DIV         44  */
    red_c        ,/* C_MAX ,      /* MAX            45  */
    red_c        ,/* C_MIN ,      /* MIN            46  */
    red_c        ,/* C_PLUS ,     /* PLUS           47  */
    red_c        ,/* C_MINUS ,    /* MINUS          48  */
    red_c        ,/* C_MULT ,     /* MULT           49  */
    red_c        ,/* C_DIV ,      /* DIV            50  */
    red_f_eq     ,/* F_EQ ,       /* F_EQ           51  */
    red_f_ne     ,/* F_NE ,       /* F_NEQ          52  */
    red_neg      ,/* M_NEG ,      /* NEG            53  */
    red_vcut     ,/* VCUT ,       /* VCUT           54  */
    red_vrotate  ,/* VROTATE ,    /* VROTATE        55  */
    red_vselect  ,/* VSELECT ,    /* VSELECT        56  */
    red_vreplace ,/* VREPLACE ,   /* VREPLACE       57  */
    red_vunite   ,/* VUNI ,       /* VUNI           58  */
    red_vdim     ,/* VDIMENSION , /* VDIMENSION     59  */
    red_mdim     ,/* MDIMENSION , /* MDIMENSION     60  */
    red_substr   ,/* SUBSTR ,     /* SUBSTR         61  */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  62 */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  63 */
    red_to_vect  ,/* TO_VECT ,    /* TO_VECT        64  */
    red_to_tvect ,/* TO_TVECT ,   /* TO_TVECT       65  */
    red_to_mat   ,/* TO_MAT ,     /* TO_MAT         66  */
    red_to_scal  ,/* TO_SCAL ,    /* TO_SCAL        67  */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  68 */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  69 */
    /* Listenfunktionen */
    red_lcut     ,/* LCUT ,       /* LCUT           70  */
    red_lselect  ,/* LSELECT ,    /* LSELECT        71  */
    red_lrotate  ,/* LROTATE ,    /* LROTATE        72  */
    red_lunite   ,/* LUNI ,       /* LUNITE         73  */
    red_lreplace ,/* LREPLACE ,   /* LREPLACE       74  */
    red_reverse  ,/* REVERSE ,    /* REVERSE        75  */
    red_ltransform,/*LTRANSFORM , /* LTRANSFORM     76  */
    red_ldim     ,/* LDIMENSION , /* LDIM           77  */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  78 */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  79 */
    red_sin      ,/* M_SIN ,      /* SIN            80  */
    red_cos      ,/* M_COS ,      /* COS            81  */
    red_tan      ,/* M_TAN ,      /* TAN            82  */
    red_ln       ,/* M_LN ,       /* LN             83  */
    red_exp      ,/* M_EXP ,      /* EXP            84  */
    red_mcut     ,/* MCUT ,       /* MCUT           85  */
    red_mselect  ,/* MSELECT ,    /* MSELECT        86  */
    red_mrotate  ,/* MROTATE ,    /* MROTATE        87  */
    red_munite   ,/* MUNI ,       /* MUNI           88  */
    red_mreplace ,/* MREPLACE ,   /* MREPLACE       89  */
    red_repstr   ,/* REPSTR ,     /* REPSTR         90  */
    red_to_list  ,/* TO_LIST,     /* TO_LIST        91  */
    red_to_field ,/* TO_FIELD,    /* TO_FIELD       92  */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  93 */
    red_mk_string,/* MK_STRING,   /* MK_STRING      94  */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  95 */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  96 */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  97 */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  98 */
    red_dummy    ,/* 0 ,          /* Reserve fuer spaeter  99 */
  }  ;
#else
 extern FCTPTR red_func_tab1[100] ;
#endif


#else
extern red_dummy();

extern red_plus(); 
extern red_minus();
extern red_mult();
extern red_div();
extern red_mod();
extern red_or();
extern red_and();
extern red_xor();
extern red_lt();
extern red_ge();
extern red_le();
extern red_gt();
extern red_eq();
extern red_neq();
extern red_min();
extern red_max();
extern red_abs();
extern red_floor();
extern red_trunc();
extern red_ceil();
extern red_frac();
extern red_not();

extern red_empty();
extern red_class();
extern red_type();
extern red_ldim();
extern red_lcut();
extern red_ltransform();
extern red_lselect();
extern red_lrotate();
extern red_lunite();
extern red_lreplace();
extern red_reverse();
extern red_ldim();
extern red_substr();
extern red_repstr();    /* replace-sub-structure */

#ifdef EAR
FCTPTR  red_func_tab1[100] = 
    { (FCTPTR)0,
    red_plus  ,/* D_PLUS ,   /*   PLUS           1   */
    red_minus ,/* D_MINUS ,    /* MINUS          2   */
    red_mult  ,/* D_MULT ,     /* MULT           3   */
    red_div   ,/* D_DIV ,      /* DIV            4   */
    red_mod   ,/* D_MOD ,      /* MOD            5   */
    red_or    ,/* D_OR ,       /* OR             6   */
    red_and   ,/* D_AND ,      /* AND            7   */
    red_xor   ,/* D_XOR ,      /* XOR            8   */
    red_eq    ,/* D_EQ ,       /* EQ             9   */
    red_neq   ,/* D_NEQ ,      /* NEQ            10  */
    red_lt    ,/* D_LESS ,     /* LT             11  */
    red_le    ,/* D_LEQ ,      /* LE             12  */
    red_gt    ,/* D_GREAT ,    /* GT             13  */
    red_ge    ,/* D_GEQ ,      /* GEQ            14  */
    red_max   ,/* D_MAX ,      /* MAX            15  */
    red_min   ,/* D_MIN ,      /* MIN            16  */

    red_not   ,/* M_NOT ,      /* NOT            17  */
    red_abs   ,/* M_ABS ,      /* ABS            18  */
    red_trunc ,/* M_TRUNCATE , /* TRUNC          19  */
    red_floor ,/* M_FLOOR ,    /* FLOOR          20  */
    red_ceil  ,/* M_CEIL ,     /* CEIL           21  */
    red_frac  ,/* M_FRAC ,     /* FRAC           22  */

    red_empty ,/* M_EMPTY ,    /* EMPTY          23  */

    red_dummy ,/* IP ,         /* IP             24  */
    red_class ,/* CLASS ,      /* CLASS          25  */
    red_type  ,/* TYPE ,       /* TYPE           26  */
    /* Feld-Funktionen: */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  27 */
    red_dummy ,/* DIMENSION ,  /* DIMENSION      28  */
    red_dummy ,/* TRANSPOSE ,  /* TRANSPOSE      29  */
    red_dummy ,/* CUT ,        /* CUT            30  */
    red_dummy ,/* TRANSFORM ,  /* TRANSFORM      31  */
    red_dummy ,/* ROTATE ,     /* ROTATE         32  */
    red_dummy ,/* SELECT ,     /* SELECT         33  */
    red_dummy ,/* REPLACE ,    /* ex INSERT      34  */
    red_dummy    ,/* MREPLACE_C , /* MREPLACE_C     35  */
    red_dummy    ,/* MREPLACE_R , /* MREPLACE_R     36  */
    red_dummy ,/* UNI_TREE ,   /* ex UNI_TREE    37  */
    red_dummy ,/* UNI ,        /* UNITE          38  */

    red_dummy ,/* VC_MAX ,     /* VC_MAX         39  */
    red_dummy ,/* VC_MIN ,     /* VC_MIN         40  */
    red_dummy ,/* VC_PLUS ,    /* VC_PLUS        41  */
    red_dummy ,/* VC_MINUS ,   /* VC_MINUS       42  */
    red_dummy ,/* VC_MULT ,    /* VC_MULT        43  */
    red_dummy ,/* VC_DIV ,     /* VC_DIV         44  */
    red_dummy ,/* C_MAX ,      /* MAX            45  */
    red_dummy ,/* C_MIN ,      /* MIN            46  */
    red_dummy ,/* C_PLUS ,     /* PLUS           47  */
    red_dummy ,/* C_MINUS ,    /* MINUS          48  */
    red_dummy ,/* C_MULT ,     /* MULT           49  */
    red_dummy ,/* C_DIV ,      /* DIV            50  */
    red_dummy     ,/* F_EQ ,       /* F_EQ           51  */
    red_dummy     ,/* F_NE ,       /* F_NEQ          52  */
    red_dummy ,/* M_NEG ,      /* NEG            53  */
    red_dummy ,/* VCUT ,       /* VCUT           54  */
    red_dummy ,/* VROTATE ,    /* VROTATE        55  */
    red_dummy ,/* VSELECT ,    /* VSELECT        56  */
    red_dummy ,/* VREPLACE ,   /* VREPLACE       57  */
    red_dummy ,/* VUNI ,       /* VUNI           58  */
    red_dummy ,/* VDIMENSION , /* VDIMENSION     59  */
    red_dummy ,/* MDIMENSION , /* MDIMENSION     60  */
    red_substr,/* SUBSTR ,     /* SUBSTR         61  */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  62 */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  63 */
    red_dummy  ,/* TO_VECT ,    /* TO_VECT        64  */
    red_dummy ,/* TO_TVECT ,   /* TO_TVECT       65  */
    red_dummy   ,/* TO_MAT ,     /* TO_MAT         66  */
    red_dummy  ,/* TO_SCAL ,    /* TO_SCAL        67  */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  68 */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  69 */
    /* Listenfunktionen */
    red_lcut  ,/* LCUT ,       /* LCUT           70  */
    red_lselect,/* LSELECT ,    /* LSELECT        71  */
    red_lrotate ,/* LROTATE ,    /* LROTATE        72  */
    red_lunite ,/* LUNI ,       /* LUNITE         73  */
    red_lreplace ,/* LREPLACE ,   /* LREPLACE       74  */
    red_reverse ,/* REVERSE ,    /* REVERSE        75  */
    red_dummy ,/*LTRANSFORM , /* LTRANSFORM     76  */
    red_ldim  ,/* LDIMENSION , /* LDIM           77  */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  78 */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  79 */
    red_dummy ,/* M_SIN ,      /* SIN            80  */
    red_dummy ,/* M_COS ,      /* COS            81  */
    red_dummy ,/* M_TAN ,      /* TAN            82  */
    red_dummy ,/* M_LN ,       /* LN             83  */
    red_dummy ,/* M_EXP ,      /* EXP            84  */
    red_dummy ,/* MCUT ,       /* MCUT           85  */
    red_dummy ,/* MSELECT ,    /* MSELECT        86  */
    red_dummy ,/* MROTATE ,    /* MROTATE        87  */
    red_dummy ,/* MUNI ,       /* MUNI           88  */
    red_dummy ,/* MREPLACE ,   /* MREPLACE       89  */
    red_repstr,/* REPSTR ,     /* REPSTR         90  */
    red_dummy  ,/* TO_LIST,     /* TO_LIST        91  */
    red_dummy ,/* TO_FIELD,    /* TO_FIELD       92  */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  93 */
    red_dummy,/* MK_STRING,   /* MK_STRING      94  */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  95 */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  96 */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  97 */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  98 */
    red_dummy ,/* 0 ,          /* Reserve fuer spaeter  99 */
  };
#else
 extern FCTPTR red_func_tab1[100] ;
#endif

#endif 




#ifdef EAR
 STACKELEM red_func_tab2[100] = 
  { (STACKELEM)0,
    D_PLUS ,     /* PLUS           1   */
    D_MINUS ,    /* MINUS          2   */
    D_MULT ,     /* MULT           3   */
    D_DIV ,      /* DIV            4   */
    D_MOD ,      /* MOD            5   */
    D_OR ,       /* OR             6   */
    D_AND ,      /* AND            7   */
    D_XOR ,      /* XOR            8   */
    D_EQ ,       /* EQ             9   */
    D_NEQ ,      /* NEQ            10  */
    D_LESS ,     /* LT             11  */
    D_LEQ ,      /* LE             12  */
    D_GREAT ,    /* GT             13  */
    D_GEQ ,      /* GEQ            14  */
    D_MAX ,      /* MAX            15  */
    D_MIN ,      /* MIN            16  */

    M_NOT ,      /* NOT            17  */
    M_ABS ,      /* ABS            18  */
    M_TRUNCATE , /* TRUNC          19  */
    M_FLOOR ,    /* FLOOR          20  */
    M_CEIL ,     /* CEIL           21  */
    M_FRAC ,     /* FRAC           22  */
  
    M_EMPTY ,    /* EMPTY          23  */

    IP ,         /* IP             24  */
    CLASS ,      /* CLASS          25  */
    TYPE ,       /* TYPE           26  */
    /* Feld-Funktionen: */
    0 ,          /* Reserve fuer spaeter  27 */
    DIMENSION ,  /* DIMENSION      28  */
    TRANSPOSE ,  /* TRANSPOSE      29  */
    CUT ,        /* CUT            30  */
    TRANSFORM ,  /* TRANSFORM      31  */
    ROTATE ,     /* ROTATE         32  */
    SELECT ,     /* SELECT         33  */
    REPLACE ,    /* ex INSERT      34  */
    MREPLACE_C , /* MREPLACE_C     35  */
    MREPLACE_R , /* MREPLACE_R     36  */
    UNI_TREE ,   /* ex UNI_TREE    37  */
    UNI ,        /* UNITE          38  */

    VC_MAX ,     /* VC_MAX         39  */
    VC_MIN ,     /* VC_MIN         40  */
    VC_PLUS ,    /* VC_PLUS        41  */
    VC_MINUS ,   /* VC_MINUS       42  */
    VC_MULT ,    /* VC_MULT        43  */
    VC_DIV ,     /* VC_DIV         44  */
    C_MAX ,      /* MAX            45  */
    C_MIN ,      /* MIN            46  */
    C_PLUS ,     /* PLUS           47  */
    C_MINUS ,    /* MINUS          48  */
    C_MULT ,     /* MULT           49  */
    C_DIV ,      /* DIV            50  */
    F_EQ ,       /* F_EQ           51  */
    F_NE ,       /* F_NEQ          52  */
    M_NEG ,      /* NEG            53  */
    VCUT ,       /* VCUT           54  */
    VROTATE ,    /* VROTATE        55  */
    VSELECT ,    /* VSELECT        56  */
    VREPLACE ,   /* VREPLACE       57  */
    VUNI ,       /* VUNI           58  */
    VDIMENSION , /* VDIMENSION     59  */
    MDIMENSION , /* MDIMENSION     60  */
    SUBSTR ,     /* SUBSTR         61  */
    0 ,          /* Reserve fuer spaeter  62 */
    0 ,          /* Reserve fuer spaeter  63 */
    TO_VECT ,    /* TO_VECT        64  */
    TO_TVECT ,   /* TO_TVECT       65  */
    TO_MAT ,     /* TO_MAT         66  */
    TO_SCAL ,    /* TO_SCAL        67  */
    0 ,          /* Reserve fuer spaeter  68 */
    0 ,          /* Reserve fuer spaeter  69 */
    /* Listenfunktionen */
    LCUT ,       /* LCUT           70  */
    LSELECT ,    /* LSELECT        71  */
    LROTATE ,    /* LROTATE        72  */
    LUNI ,       /* LUNITE         73  */
    LREPLACE ,   /* LREPLACE       74  */
    REVERSE ,    /* REVERSE        75  */
    LTRANSFORM , /* LTRANSFORM     76  */
    LDIMENSION , /* LDIM           77  */
    0 ,          /* Reserve fuer spaeter  78 */
    0 ,          /* Reserve fuer spaeter  79 */
    M_SIN ,      /* SIN            80  */
    M_COS ,      /* COS            81  */
    M_TAN ,      /* TAN            82  */
    M_LN ,       /* LN             83  */
    M_EXP ,      /* EXP            84  */
    MCUT ,       /* MCUT           85  */
    MSELECT ,    /* MSELECT        86  */
    MROTATE ,    /* MROTATE        87  */
    MUNI ,       /* MUNI           88  */
    MREPLACE ,   /* MREPLACE       89  */
    REPSTR ,     /* REPSTR         90  */
    TO_LIST,     /* TO_LIST        91  */
    TO_FIELD,    /* TO_FIELD       92  */
    0 ,          /* Reserve fuer spaeter  93 */
    MK_STRING,   /* MK_STRING      94  */
    0 ,          /* Reserve fuer spaeter  95 */
    0 ,          /* Reserve fuer spaeter  96 */
    0 ,          /* Reserve fuer spaeter  97 */
    0 ,          /* Reserve fuer spaeter  98 */
    0 ,          /* Reserve fuer spaeter  99 */
  }  ;
#else
 extern  STACKELEM red_func_tab2[100] ;
#endif

