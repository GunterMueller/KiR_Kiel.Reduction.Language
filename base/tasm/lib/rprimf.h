/* $Log: rprimf.h,v $
 * Revision 1.15  1997/06/06 09:13:11  rs
 * some more (complete) modifications concerning speculative evaluation
 * (don't say you have missed my talk ? ;-)
 *
 * Revision 1.14  1996/03/25  14:25:34  cr
 * C_FRAME in classname
 * TY_FRAME,TY_SLOT in typename
 *
 * Revision 1.13  1996/03/12  15:38:19  stt
 * ord, char -> to_ord, to_char
 *
 * Revision 1.12  1996/02/28  11:02:30  stt
 * red_to_char, red_to_ord, red_sprintf and red_sscanf inserted.
 *
 * Revision 1.11  1996/02/16  13:24:35  cr
 * update,delete,slots,test --> fupdate,fdelete,fslots,ftest
 *
 * Revision 1.10  1995/12/11  13:13:46  cr
 * two more operations on frames: FR_SLOTS,FR_DELETE
 *
 * Revision 1.9  1995/11/15  15:10:30  cr
 * length of function-tabs adapted (99->109) to conform to editor
 * moved FR_TEST,FR_SELECT,FR_UPDATE to positions specified in cencod.h
 *
 * Revision 1.8  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.7  1995/05/12  12:23:05  cr
 * added (ahem..): red_update, red_fselect, red_test
 *
 * Revision 1.6  1995/03/27  14:28:55  sf
 * output of char's in matchprim now quoted
 *
 * Revision 1.5  1994/11/30  10:15:00  car
 * ascii represenation of booleans
 *
 * Revision 1.4  1994/11/01  15:54:30  car
 * generate codefile in non-DEBUG-version
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:50:33  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* file rprimf.h */

/* primitive funktionen */

extern int red_plus();
extern int red_minus();
extern int red_mult();
extern int red_div();
extern int red_mod();
extern int red_or();
extern int red_and();
extern int red_xor();
extern int red_eq();
extern int red_neq();
extern int red_lt();
extern int red_le();
extern int red_gt();
extern int red_geq();
extern int red_max();
extern int red_min();
extern int red_not();
extern int red_quot();

extern int red_f_eq();
extern int red_abs();
extern int red_neg();
extern int red_trunc();
extern int red_floor();
extern int red_ceil();
extern int red_frac();

extern int red_empty();
extern int red_ip();
extern int red_class();
extern int red_type();
extern int red_dim();

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

extern int red_vc_max();
extern int red_vc_min();
extern int red_vc_plus();
extern int red_vc_minus();
extern int red_vc_mult();
extern int red_vc_div();
extern int red_c_max();
extern int red_c_min();
extern int red_c_plus();
extern int red_c_minus();
extern int red_c_mult();
extern int red_c_div();

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
extern int red_repstr();

extern int red_sin();
extern int red_cos();
extern int red_tan();
extern int red_ln();
extern int red_exp();

/* cr 12/05/95, kir(ff), START */
extern int red_update();
extern int red_fselect();
extern int red_test();
extern int red_slots(); /* cr 08.12.95 */
extern int red_delete(); /* cr 08.12.95 */
/* cr 12/05/95, kir(ff), END */

/* stt 28.02.96 */
extern int red_to_char();
extern int red_to_ord();
extern int red_sprintf();
extern int red_sscanf();
extern int red_specmap();

extern void DescDump(); /* TB, 3.11.1992 */ /*rdesc.c */

/*uh*/ extern int red_lgen();

#ifdef INTER

/*---------------------------------------------------------------------------*/
/* red_dummy - wird aufgerufen, falls eine Funktion nicht implementiert ist. */
/*             red_dummy scheitert immer und liefert deshalb Null zurueck.   */
/*---------------------------------------------------------------------------*/

static int red_dummy()                   /* ach 05/11/92: int */
{
  START_MODUL("red_dummy");
  END_MODUL("red_dummy");
  return (0);
}

/*---------------------------------------------------------------------------*/
/* tabelle der primitiven funktionen                                         */
/*---------------------------------------------------------------------------*/

FCTPTR red_func_tab[] = {
  0              /* unbenutzt      0  */
, red_plus       /* PLUS           1  */
, red_minus      /* MINUS          2  */
, red_mult       /* MULT           3  */
, red_div        /* DIV            4  */
, red_mod        /* MOD            5  */
, red_or         /* OR             6  */
, red_and        /* AND            7  */
, red_xor        /* XOR            8  */
, red_eq         /* EQ             9  */
, red_neq        /* NEQ            10 */
, red_lt         /* LT             11 */
, red_le         /* LE             12 */
, red_gt         /* GT             13 */
, red_geq        /* GEQ            14 */
, red_max        /* MAX            15 */
, red_min        /* MIN            16 */
, red_not        /* NOT            17 */
, red_abs        /* ABS            18 */
, red_trunc      /* TRUNC          19 */
, red_floor      /* FLOOR          20 */
, red_ceil       /* CEIL           21 */
, red_frac       /* FRAC           22 */
, red_empty      /* EMPTY          23 */
, red_ip         /* IP             24 */
, red_class      /* CLASS          25 */
, red_type       /* TYPE           26 */
, red_dummy      /* Reserve        27 */
, red_dim        /* DIMENSION      28 */
, red_transpose  /* TRANSPOSE      29 */
, red_cut        /* CUT            30 */
, red_transform  /* TRANSFORM      31 */
, red_rotate     /* ROTATE         32 */
, red_select     /* SELECT         33 */
, red_replace    /* REPLACE        34 */
, red_mre_c      /* MREPLACE_C     35 */
, red_mre_r      /* MREPLACE_R     36 */
, red_dummy      /* ex UNI_TREE    37 */
, red_unite      /* UNITE          38 */
, red_vc_max     /* VC_MAX         39 */
, red_vc_min     /* VC_MIN         40 */
, red_vc_plus    /* VC_PLUS        41 */
, red_vc_minus   /* VC_MINUS       42 */
, red_vc_mult    /* VC_MULT        43 */
, red_vc_div     /* VC_DIV         44 */
, red_c_max      /* MAX            45 */
, red_c_min      /* MIN            46 */
, red_c_plus     /* PLUS           47 */
, red_c_minus    /* MINUS          48 */
, red_c_mult     /* MULT           49 */
, red_c_div      /* DIV            50 */
, red_f_eq       /* F_EQ           51 */
, red_f_eq       /* F_NE           52 */ /* geht nicht !!! */
, red_neg        /* NEG            53 */
, red_vcut       /* VCUT           54 */
, red_vrotate    /* VROTATE        55 */
, red_vselect    /* VSELECT        56 */
, red_vreplace   /* VREPLACE       57 */
, red_vunite     /* VUNI           58 */
, red_vdim       /* VDIMENSION     59 */
, red_mdim       /* MDIMENSION     60 */
, red_substr     /* SUBSTR         61 */
, red_lgen       /* lgen           62 */ /*uh*/
, red_dummy      /* Reserve        63 */
, red_to_vect    /* TO_VECT        64 */
, red_to_tvect   /* TO_TVECT       65 */
, red_to_mat     /* TO_MAT         66 */
, red_to_scal    /* TO_SCAL        67 */
, red_dummy      /* Reserve        68 */
, red_dummy      /* Reserve        69 */
, red_lcut       /* LCUT           70 */
, red_lselect    /* LSELECT        71 */
, red_lrotate    /* LROTATE        72 */
, red_lunite     /* LUNI           73 */
, red_lreplace   /* LREPLACE       74 */
, red_reverse    /* REVERSE        75 */
, red_ltransform /* LTRANSFORM     76 */
, red_ldim       /* LDIMENSION     77 */
, red_dummy      /* Reserve        78 */
, red_quot       /* QUOT           79 */
, red_sin        /* SIN            80 */
, red_cos        /* COS            81 */
, red_tan        /* TAN            82 */
, red_ln         /* LN             83 */
, red_exp        /* EXP            84 */
, red_mcut       /* MCUT           85 */
, red_mselect    /* MSELECT        86 */
, red_mrotate    /* MROTATE        87 */
, red_munite     /* MUNI           88 */
, red_mreplace   /* MREPLACE       89 */
, red_repstr     /* REPSTR         90 */
, red_to_list    /* TO_LIST        91 */
, red_to_field   /* TO_FIELD       92 */
, red_dummy      /* Reserve        93 */
, red_mk_string  /* MK_STRING      94 */
, red_dummy      /* Reserve        95 */
, red_dummy      /* Reserve        96 */
, red_dummy      /* Reserve        97 */
, red_dummy      /* Reserve        98 */
, red_dummy      /* Reserve        99 */
/* length adapted (99->109), editor lists 110 primitive functions, cr 13.11.95 */
, red_dummy      /* Reserve       100 */
, red_dummy      /* Reserve       101 */
, red_dummy      /* Reserve       102 */
, red_dummy      /* Reserve       103 */
, red_dummy      /* Reserve       104 */
, red_dummy      /* Reserve       105 */
, red_dummy      /* Reserve       106 */
/* test, select, update moved to conform with cencod.h, cr 13.11.95 */
, red_test       /* FR_TEST       107 */
, red_fselect    /* FR_SELECT     108 */
, red_update     /* FR_UPDATE     109 */
, red_slots      /* FR_SLOTS      110 */ /* cr 08.12.95 */
, red_delete     /* FR_DELETE     111 */ /* cr 08.12.95 */

/* stt 28.02.96 */
, red_to_char    /* TO_CHAR       112 */
, red_to_ord     /* TO_ORD        113 */
, red_sprintf    /* SPRINTF       114 */
, red_sscanf     /* SSCANF        115 */
, red_specmap    /* SPECMAP       116 */
};

#ifdef NotUsed
STACKELEM pf_tab[] = {
  0              /* unbenutzt      0  */
, D_PLUS         /* PLUS           1  */
, D_MINUS        /* MINUS          2  */
, D_MULT         /* MULT           3  */
, D_DIV          /* DIV            4  */
, D_MOD          /* MOD            5  */
, D_OR           /* OR             6  */
, D_AND          /* AND            7  */
, D_XOR          /* XOR            8  */
, D_EQ           /* EQ             9  */
, D_NEQ          /* NEQ            10 */
, D_LESS         /* LT             11 */
, D_LEQ          /* LE             12 */
, D_GREAT        /* GT             13 */
, D_GEQ          /* GEQ            14 */
, D_MAX          /* MAX            15 */
, D_MIN          /* MIN            16 */
, M_NOT          /* NOT            17 */
, M_ABS          /* ABS            18 */
, M_TRUNCATE     /* TRUNC          19 */
, M_FLOOR        /* FLOOR          20 */
, M_CEIL         /* CEIL           21 */
, M_FRAC         /* FRAC           22 */
, M_EMPTY        /* EMPTY          23 */
, IP             /* IP             24 */
, CLASS          /* CLASS          25 */
, TYPE           /* TYPE           26 */
, 0              /* Reserve        27 */
, DIMENSION      /* DIMENSION      28 */
, TRANSPOSE      /* TRANSPOSE      29 */
, CUT            /* CUT            30 */
, TRANSFORM      /* TRANSFORM      31 */
, ROTATE         /* ROTATE         32 */
, SELECT         /* SELECT         33 */
, REPLACE        /* REPLACE        34 */
, MREPLACE_C     /* MREPLACE_C     35 */
, MREPLACE_R     /* MREPLACE_R     36 */
, 0              /* ex UNI_TREE    37 */
, UNI            /* UNITE          38 */
, VC_MAX         /* VC_MAX         39 */
, VC_MIN         /* VC_MIN         40 */
, VC_PLUS        /* VC_PLUS        41 */
, VC_MINUS       /* VC_MINUS       42 */
, VC_MULT        /* VC_MULT        43 */
, VC_DIV         /* VC_DIV         44 */
, C_MAX          /* MAX            45 */
, C_MIN          /* MIN            46 */
, C_PLUS         /* PLUS           47 */
, C_MINUS        /* MINUS          48 */
, C_MULT         /* MULT           49 */
, C_DIV          /* DIV            50 */
, F_EQ           /* F_EQ           51 */
, F_NE           /* F_NE           52 */
, M_NEG          /* NEG            53 */
, VCUT           /* VCUT           54 */
, VROTATE        /* VROTATE        55 */
, VSELECT        /* VSELECT        56 */
, VREPLACE       /* VREPLACE       57 */
, VUNI           /* VUNI           58 */
, VDIMENSION     /* VDIMENSION     59 */
, MDIMENSION     /* MDIMENSION     60 */
, SUBSTR         /* SUBSTR         61 */
, 0              /* Reserve        62 */
, 0              /* Reserve        63 */
, TO_VECT        /* TO_VECT        64 */
, TO_TVECT       /* TO_TVECT       65 */
, TO_MAT         /* TO_MAT         66 */
, TO_SCAL        /* TO_SCAL        67 */
, 0              /* Reserve        68 */
, 0              /* Reserve        69 */
, LCUT           /* LCUT           70 */
, LSELECT        /* LSELECT        71 */
, LROTATE        /* LROTATE        72 */
, LUNI           /* LUNI           73 */
, LREPLACE       /* LREPLACE       74 */
, REVERSE        /* REVERSE        75 */
, LTRANSFORM     /* LTRANSFORM     76 */
, LDIMENSION     /* LDIMENSION     77 */
, 0              /* Reserve        78 */
, D_QUOT         /* QUOT           79 */
, M_SIN          /* SIN            80 */
, M_COS          /* COS            81 */
, M_TAN          /* TAN            82 */
, M_LN           /* LN             83 */
, M_EXP          /* EXP            84 */
, MCUT           /* MCUT           85 */
, MSELECT        /* MSELECT        86 */
, MROTATE        /* MROTATE        87 */
, MUNI           /* MUNI           88 */
, MREPLACE       /* MREPLACE       89 */
, REPSTR         /* REPSTR         90 */
, TO_LIST        /* TO_LIST        91 */
, TO_FIELD       /* TO_FIELD       92 */
, 0              /* Reserve        93 */
, MK_STRING      /* MK_STRING      94 */
, 0              /* Reserve        95 */
, 0              /* Reserve        96 */
, 0              /* Reserve        97 */
, 0              /* Reserve        98 */
, 0              /* Reserve        99 */
/* length adapted (99->109), editor lists 110 primitive functions, cr 13.11.95 */
, 0              /* Reserve       100 */
, 0              /* Reserve       101 */
, 0              /* Reserve       102 */
, 0              /* Reserve       103 */
, 0              /* Reserve       104 */
, 0              /* Reserve       105 */
, 0              /* Reserve       106 */
/* test, select, update moved to conform with cencod.h, cr 13.11.95 */
, FR_TEST        /* FR_TEST       107 */
, FR_SELECT      /* FR_SELECT     108 */
, FR_UPDATE      /* FR_UPDATE     109 */
, FR_SLOTS       /* FR_SLOTS      110 */ /* cr 08.12.95 */
, FR_DELETE      /* FR_DELETE     111 */ /* cr 08.12.95 */

/* stt 28.02.96 */
, TO_CHAR        /* TO_CHAR       112 */
, TO_ORD         /* TO_ORD        113 */
, TO_SPRINTF     /* SPRINTF       114 */
, TO_SSCANF      /* SSCANF        115 */
};
#endif /* NotUsed */

#endif /* INTER */


#ifdef EACOMP

/* temporaer, die funktionstabelle enthaelt zusaetzlich die namen der  */
/* primitiven funktionen. wird nur in rear.c benutzt.                  */

    /* VVV "int" von TB, 3.11.1992 */
static int red_dummy() { return(0); }                    /* dummy primfunc */

/* tabelle mit den primitiven funktionen */

struct {
  FCTPTR fkt;
  char   *fnam;
} func_tab[] =
{{0             , "NULL"     } /* NIX            0  */
,{red_plus      , "plus"      }/* PLUS           1  */
,{red_minus     , "minus"     }/* MINUS          2  */
,{red_mult      , "mult"      }/* MULT           3  */
,{red_div       , "div"       }/* DIV            4  */
,{red_mod       , "mod"       }/* MOD            5  */
,{red_or        , "or"        }/* OR             6  */
,{red_and       , "and"       }/* AND            7  */
,{red_xor       , "xor"       }/* XOR            8  */
,{red_eq        , "eq"        }/* EQ             9  */
,{red_neq       , "neq"       }/* NEQ            10 */
,{red_lt        , "lt"        }/* LT             11 */
,{red_le        , "le"        }/* LE             12 */
,{red_gt        , "gt"        }/* GT             13 */
,{red_geq       , "geq"       }/* GEQ            14 */
,{red_max       , "max"       }/* MAX            15 */
,{red_min       , "min"       }/* MIN            16 */
,{red_not       , "not"       }/* NOT            17 */
,{red_abs       , "abs"       }/* ABS            18 */
,{red_trunc     , "trunc"     }/* TRUNC          19 */
,{red_floor     , "floor"     }/* FLOOR          20 */
,{red_ceil      , "ceil"      }/* CEIL           21 */
,{red_frac      , "frac"      }/* FRAC           22 */
,{red_empty     , "empty"     }/* EMPTY          23 */
,{red_ip        , "ip"        }/* IP             24 */
,{red_class     , "class"     }/* CLASS          25 */
,{red_type      , "type"      }/* TYPE           26 */
,{red_dummy     , "dummy"     }/* Reserve        27 */
,{red_dim       , "dim"       }/* DIMENSION      28 */
,{red_transpose , "transpose" }/* TRANSPOSE      29 */
,{red_cut       , "cut"       }/* CUT            30 */
,{red_transform , "transform" }/* TRANSFORM      31 */
,{red_rotate    , "rotate"    }/* ROTATE         32 */
,{red_select    , "select"    }/* SELECT         33 */
,{red_replace   , "replace"   }/* REPLACE        34 */
,{red_mre_c     , "mre_c"     }/* MREPLACE_C     35 */
,{red_mre_r     , "mre_r"     }/* MREPLACE_R     36 */
,{red_dummy     , "dummy"     }/* ex UNI_TREE    37 */
,{red_unite     , "unite"     }/* UNITE          38 */
/* koordinatenfunktionen */
,{red_vc_max    , "vc_max"    }/* VC_MAX         39 */
,{red_vc_min    , "vc_min"    }/* VC_MIN         40 */
,{red_vc_plus   , "vc_plus"   }/* VC_PLUS        41 */
,{red_vc_minus  , "vc_minus"  }/* VC_MINUS       42 */
,{red_vc_mult   , "vc_mult"   }/* VC_MULT        43 */
,{red_vc_div    , "vc_div"    }/* VC_DIV         44 */
,{red_c_max     , "c_max"     }/* MAX            45 */
,{red_c_min     , "c_min"     }/* MIN            46 */
,{red_c_plus    , "c_plus"    }/* PLUS           47 */
,{red_c_minus   , "c_minus"   }/* MINUS          48 */
,{red_c_mult    , "c_mult"    }/* MULT           49 */
,{red_c_div     , "c_div"     }/* DIV            50 */
,{red_f_eq      , "f_eq"      }/* F_EQ           51 */
,{red_f_eq      , "f_eq"      }/* geht nicht !!! */
,{red_neg       , "neg"       }/* NEG            53 */
,{red_vcut      , "vcut"      }/* VCUT           54 */
,{red_vrotate   , "vrotate"   }/* VROTATE        55 */
,{red_vselect   , "vselect"   }/* VSELECT        56 */
,{red_vreplace  , "vreplace"  }/* VREPLACE       57 */
,{red_vunite    , "vunite"    }/* VUNI           58 */
,{red_vdim      , "vdim"      }/* VDIMENSION     59 */
,{red_mdim      , "mdim"      }/* MDIMENSION     60 */
,{red_substr    , "substr"    }/* SUBSTR         61 */
,{red_lgen      , "lgen"      }/*uh*/
,{red_dummy     , "dummy"     }/* Reserve        63 */
,{red_to_vect   , "to_vect"   }/* TO_VECT        64 */
,{red_to_tvect  , "to_tvect"  }/* TO_TVECT       65 */
,{red_to_mat    , "to_mat"    }/* TO_MAT         66 */
,{red_to_scal   , "to_scal"   }/* TO_SCAL        67 */
,{red_dummy     , "dummy"     }/* Reserve        68 */
,{red_dummy     , "dummy"     }/* Reserve        69 */
/* Listenfunktionen */
,{red_lcut      , "lcut"      }/* LCUT           70 */
,{red_lselect   , "lselect"   }/* LSELECT        71 */
,{red_lrotate   , "lrotate"   }/* LROTATE        72 */
,{red_lunite    , "lunite"    }/* LUNITE         73 */
,{red_lreplace  , "lreplace"  }/* LREPLACE       74 */
,{red_reverse   , "reverse"   }/* REVERSE        75 */
,{red_ltransform , "ltransform" }/* LTRANSFORM     76 */
,{red_ldim      , "ldim"      }/* LDIM           77 */
,{red_dummy     , "dummy"     }/* Reserve        78 */
,{red_quot      , "quot"      }/* QUOT           79 */
/* transzendente Funktionen (rbibfunc.c) */
,{red_sin       , "sin"       }/* SIN            80 */
,{red_cos       , "cos"       }/* COS            81 */
,{red_tan       , "tan"       }/* TAN            82 */
,{red_ln        , "ln"        }/* LN             83 */
,{red_exp       , "exp"       }/* EXP            84 */
,{red_mcut      , "mcut"      }/* MCUT           85 */
,{red_mselect   , "mselect"   }/* MSELECT        86 */
,{red_mrotate   , "mrotate"   }/* MROTATE        87 */
,{red_munite    , "munite"    }/* MUNI           88 */
,{red_mreplace  , "mreplace"  }/* MREPLACE       89 */
,{red_repstr    , "repstr"    }/* REPSTR         90 */
,{red_to_list   , "to_list"   }/* TO_LIST        91 */
,{red_to_field  , "to_field"  }/* TO_FIELD       92 */
,{red_dummy     , "dummy"     }/* Reserve        93 */
,{red_mk_string , "mk_string" }/* MK_STRING      94 */
,{red_dummy     , "dummy"     }/* Reserve        95 */
,{red_dummy     , "dummy"     }/* Reserve        96 */
,{red_dummy     , "dummy"     }/* Reserve        97 */
,{red_dummy     , "dummy"     }/* Reserve        98 */
,{red_dummy     , "dummy"     }/* Reserve        99 */
/* length adapted (99->109), editor lists 110 primitive functions, cr 13.11.95 */
,{red_dummy     , "dummy"     }/* Reserve       100 */
,{red_dummy     , "dummy"     }/* Reserve       101 */
,{red_dummy     , "dummy"     }/* Reserve       102 */
,{red_dummy     , "dummy"     }/* Reserve       103 */
,{red_dummy     , "dummy"     }/* Reserve       104 */
,{red_dummy     , "dummy"     }/* Reserve       105 */
,{red_dummy     , "dummy"     }/* Reserve       106 */
/* test, select, update moved to conform with cencod.h, cr 13.11.95 */
,{red_test      , "ftest"     }/* FR_TEST       107 */
,{red_fselect   , "fselect"   }/* FR_SELECT     108 */
,{red_update    , "fupdate"   }/* FR_UPDATE     109 */
,{red_slots     , "fslots"    }/* FR_SLOTS      110 */ /* cr 08.12.95 */
,{red_delete    , "fdelete"   }/* FR_DELETE     111 */ /* cr 08.12.95 */

/* stt 28.02.96 */
,{red_to_char   , "to_char"   }/* TO_CHAR       112 */
,{red_to_ord    , "to_ord"    }/* TO_ORD        113 */
,{red_sprintf   , "sprintf"   }/* SPRINTF       114 */
,{red_sscanf    , "sscanf"    }/* SSCANF        115 */
,{red_specmap   , "specmap"   }/* SPECMAP       116 */
};

/* --- symbolische namen fuer classen und typen --- */

char *classname(p)
T_PTD p;
{
  switch (R_DESC(*p,class)) {
    case C_SCALAR:    return("C_SCALAR");
    case C_DIGIT:     return("C_DIGIT");
    case C_LIST:      return("C_LIST");
    case C_MATRIX:    return("C_MATRIX");
    case C_FUNC:      return("C_FUNC");
    case C_EXPRESSION:return("C_EXPRESSION");
    case C_CONSTANT:  return("C_CONSTANT");
    case C_VECTOR:    return("C_VECTOR");
    case C_TVECTOR:   return("C_TVECTOR");
    case C_MATCHING:  return("C_MATCHING");
    case C_PATTERN:   return("C_PATTERN");
    case C_CONS:      return("C_CONS");
    case C_INTACT:    return("C_INTACT");
    case C_FRAME:     return("C_FRAME");
    default:          return("??");
  }
}

char *typename(p)
T_PTD p;
{
  switch (R_DESC(*p,type)) {
    case TY_FREE:          return("TY_FREE");
    case TY_UNDEF:         return("TY_UNDEF");
    case TY_INTEGER:       return("TY_INTEGER");
    case TY_REAL:          return("TY_REAL");
    case TY_BOOL:          return("TY_BOOL");
    case TY_STRING:        return("TY_STRING");
    case TY_DIGIT:         return("TY_DIGIT");
    case TY_REC:           return("TY_REC");
    case TY_SUB:           return("TY_SUB");
    case TY_COND:          return("TY_COND");
    case TY_EXPR:          return("TY_EXPR");
    case TY_VAR:           return("TY_VAR");
    case TY_NAME:          return("TY_NAME");
    case TY_MATRIX:        return("TY_MATRIX");
    case TY_SWITCH:        return("TY_SWITCH");
    case TY_MATCH:         return("TY_MATCH");
    case TY_NOMAT:         return("TY_NOMAT");
    case TY_LREC:          return("TY_LREC");
    case TY_LREC_IND:      return("TY_LREC_IND");
    case TY_LREC_ARGS:     return("TY_LREC_ARGS");
    case TY_COMB:          return("TY_COMB");
    case TY_CLOS:          return("TY_CLOS");
    case TY_CONDI:         return("TY_CONDI");
    case TY_CASE:          return("TY_CASE");
    case TY_WHEN:          return("TY_WHEN");
    case TY_GUARD:         return("TY_GUARD");
    case TY_BODY:          return("TY_BODY");
    case TY_PRIM:          return("TY_PRIM");
    case TY_ZF:            return("TY_ZF");
    case TY_ZFCODE:        return("TY_ZFCODE");
    case TY_LIST:          return("TY_LIST");
    case TY_AS:            return("TY_AS");
    case TY_DOTDOTDOT:     return("TY_DOTDOTDOT");
    case TY_LASTDOTDOTDOT: return("TY_LASTDOTDOTDOT");
    case TY_DOTPLUS:       return("TY_DOTPLUS");
    case TY_LASTDOTPLUS:   return("TY_LASTDOTPLUS");
    case TY_DOTSTAR:       return("TY_DOTSTAR");
    case TY_LASTDOTSTAR:   return("TY_LASTDOTSTAR");
    case TY_SELECTION:     return("TY_SELECTION");
    case TY_CLAUSE:        return("TY_CLAUSE");
    case TY_CONS:          return("TY_CONS");
    case TY_INTACT:        return("TY_INTACT");
    case TY_FRAME:         return("TY_FRAME");
    case TY_SLOT:          return("TY_SLOT");
    default:               return("??");
  }
}

/* --- stelname: erzeugt symbolische namen fuer stackelemente --- */

char *stelname(x)
STACKELEM x;
{
  extern INTACT_TAB intact_tab[];           /* stt 11.09.95 */
  static char hilf[80];
  
  if (T_FUNC(x))     return(func_tab[FUNC_INDEX(x)].fnam);
  if (T_IAFUNC(x))   return(intact_tab[FUNC_INDEX(x)].name);    /* stt */
  if (T_INT(x))      sprintf(hilf,"int(%d)",VAL_INT(x)); else   
  if (T_SA_TRUE(x))  sprintf(hilf,"bool(%s)","true");          else
  if (T_SA_FALSE(x)) sprintf(hilf,"bool(%s)","false");         else
  if (x && T_POINTER(x))  {
    sprintf(hilf,"ptd(0x%08x), [%s,%s]",x,classname((T_PTD)x),typename((T_PTD)x));  /* T_PTD eingefuegt von RS 7.12.1992 */
    if (R_DESC(*(T_PTD)x,class) == C_LIST) {
      if (R_LIST(*(T_PTD)x,dim) == 0)
        if (R_DESC(*(T_PTD)x,type) == TY_STRING)
          sprintf(hilf,"nilstring");
        else
          sprintf(hilf,"nil");
    }
    else
    if (R_DESC(*(T_PTD)x,class)==C_SCALAR && R_DESC(*(T_PTD)x,type)==TY_REAL)
      sprintf(hilf,"real(%f)",R_SCALAR(*(T_PTD)x,valr));
  } /* end pointer */
  else
  if (T_NUM(x))    sprintf(hilf,"#%d",VALUE(x));        else
  if (T_STR(x))    sprintf(hilf,"'%c'",VALUE(x));         else
  if (T_LREC(x))   sprintf(hilf,"LREC(%d)",ARITY(x));   else
  if (T_AP(x))     sprintf(hilf,"AP(%d)",ARITY(x));     else
  if (T_LIST(x))   sprintf(hilf,"LIST(%d)",ARITY(x));   else
  if (T_CON(x))    sprintf(hilf,"CON(%d)",ARITY(x));    else
  sprintf(hilf,"atom(0x%08x)",x);
  return(hilf); 
}

#endif /* EACOMP */

/* end of file */
