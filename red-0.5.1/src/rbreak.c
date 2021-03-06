/* $Log: rbreak.c,v $
 * Revision 1.2  1992/12/16  12:49:22  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */





/*****************************************************************************/
/*             MANAGING THE BREAKPOINTS FOR THE RUNTIME-SYSTEM               */
/* ------------------------------------------------------------------------- */
/* rbreak.c  - external  :  set_breakpoint(),   del_breakpoint(),            */
/*                          list_breakpoint(),  list_module(),               */
/*                          clear_breakpoints()                              */
/*             called by :  rruntime.c   [ monitor_call ]                    */
/*                                                                           */
/*           - external  :  start_modul(),      end_modul()                  */
/*             called by :  stackmoduls                                      */
/*                                                                           */
/*           - internal  :  druck_measure, measure , stacknr, m_init,        */
/*                          messta,        mfree,    m_heap                  */
/*****************************************************************************/

#include "rstdinc.h"
#include "rmeasure.h"
#include "rruntime.h"
#include "rmodulnr.h"
#include <string.h>

#if DEBUG
/*----------------------------------------------------------------------------
 * Lokalen Variablen:
 *----------------------------------------------------------------------------
 */
#define MAXBREAKNR 10                 /* Maximal 10  Breakpoints              */
static char breaklist[MAXBREAKNR][20];/* Breakpoints a 20 Charakter           */
       int  breaknr = 0;              /* Die Anzahl der gesetzten Breakpoints */

#define MAXMODULNR 50                 /* Maximal eine Hierachie von 50 Modulen*/
 char modullist[MAXMODULNR][20]; /* Der Modulstack */
int  modulnr = 0;             /* Die Groesse des Stacks */
int modul_id;                 /* Messsystem: Nr des aktiven Moduls */

/*---------------------------------------------------------------------------
 * Externe Funktionen:
 *---------------------------------------------------------------------------
 */
extern monitor_call();               /* runtime:c    */
extern int cursorxy(); /* TB, 4.11.1992 */
extern void clearscreen(); /* TB, 4.11.1992 */

extern int i_status;                 /* runtime:c    */
extern int mode;                     /* runtime:c    */
extern int breakred;                 /* runtime:c    */
extern int co ;
extern int li ;
/*----------------------------------------------------------------------------
 * find_breakpoint -- sucht einen Namen in der Breakpointliste. Als Ergebnis
 *                     wird der Breakpointindex bzw -1 ausgeben.
 *----------------------------------------------------------------------------
 */
static int find_breakpoint(modulname)
char modulname[];
{
  int i;

  for ( i = breaknr-1; i >= 0; i-- )
    if (strcmp(breaklist[i],modulname) == 0) break;  /* wenn Name gefunden */

  return (i);
}

/*----------------------------------------------------------------------------
 * set_breakpoint -- traegt einen Modulnamen in die Breakpointliste ein.
 *----------------------------------------------------------------------------
 */
void set_breakpoint(modulname)
char modulname[];
{
  if (breaknr == MAXBREAKNR)
  {
    printf("Maximale Anzahl von Breakpoint ueberschritten.\n");
    return;
  }

  if (find_breakpoint(modulname) >= 0)      /* breakpoint bereits gesetzt? */
  {
    printf("Fuer das Modul ist bereits ein Breakpoint gesetzt.\n");
    return;
  }
  strcpy(breaklist[breaknr++],modulname);   /* breakpoint setzen */
}

/*----------------------------------------------------------------------------
 * del_breakpoint -- entfernt einen Breakpoint aus der Liste.
 *----------------------------------------------------------------------------
 */
void del_breakpoint(modulname)
char modulname[];
{
  int i;

  if ((i= find_breakpoint(modulname)) < 0)  /* existiert breakpoint ueberhaupt? */
  {
    printf("Fuer dieses Modul ist kein Breakpoint gesetzt.\n");
    return;
  }

  breaknr--;                       /* Anzahl reduzieren */
  for (  ; i < breaknr;  )                  /* Loch in der breakpointlist */
    strcpy(breaklist[i],breaklist[++i]);    /* beseitigen */
}

/*---------------------------------------------------------------------------
 * clear_breakpoints  --  alle eingetragenen Breakpoints werden geloescht
 *---------------------------------------------------------------------------
 */
void clear_breakpoints()
{
 breaknr = 0;
 breakred = 0;
}

/*----------------------------------------------------------------------------
 * start_modul -- Das Modul wird in den Modulstack eingetragen, und falls ein
 *                Breakpoint gesetzt worden ist, so wird monitor_call
 *                aufgerufen.
 *----------------------------------------------------------------------------
 */
void start_modul(modulname)
char modulname[];
{
  if (strcmp(modulname,"ear") == 0) i_status = 1;           /* Zaehler richtig */
  if (strcmp(modulname,"ea_retrieve") == 0) i_status = 2;   /* setzen fuer zaehlen */
                                                            /* der Stackop's */
  if (modulnr == MAXMODULNR)                                /* s.a. printsteps() in runtime:c */
  {
    printf("Zu tiefe Schachtelung der Module.\n");   /* Aufrufhierarchie zu */
    monitor_call();                                  /* gross */
    return;
  }

  strcpy(modullist[modulnr++],modulname);            /* Modul eintragen */

  NEWMODUL;
  if (find_breakpoint(modulname) >= 0)             /* ist Modul breakpoint? */
    {
     WRONGINPUT(breakpoint reached);
     monitor_call();                                 /* command-Schleife */
    }
#if MEASURE
  if (modulnr > 1  &&  !strcmp(modulname,"failure")) {
    modul_id = fetch_modul(modullist[modulnr-2]); /* fuer welches modul messen? */
    if (modul_id <= 0) printf("Modul %s nicht erkannt\n",modullist[modulnr-2]);
    measure(MS_FAIL);
  }
  modul_id = fetch_modul(modulname);                 /* fuer welches modul messen? */
  if (modul_id <= 0)  printf("Modul %s nicht erkannt\n",modulname);
  measure(MS_CALL);
#endif

}

/*-----------------------------------------------------------------------------
 * end_modul -- entfernt den Modulnamen aus dem Stack und ruft monitor_call
 *              falls auf den Vorgaenger ein Breakpoint gesetzt ist.
 *-----------------------------------------------------------------------------
 */
void end_modul(modulname)
char modulname[];
{
  if (strcmp(modulname,"?") == 0   &&  modulnr > 0)
  {                                /* Moeglichkeit, einfach das oberste Modul */
      modulnr--;                   /* zu entfernen */
  }
  else  if ( (modulnr <= 0) || (strcmp(modulname,modullist[--modulnr]) != 0))
  {
    printf("%s ist nicht oberstes Modul der Aufrufhierachie.\n",modulname);
    monitor_call();                /* command-Schleife (runtime:c) */
    return;
  }
  if (modulnr == 0)  /* no newmodul available !!!!! */
    return;

 NEWMODUL;

#if MEASURE
 modul_id = fetch_modul(modullist[modulnr-1]);
  if (modul_id <= 0)  printf("Modul %s nicht erkannt\n",modullist[modulnr-1]);
#endif                             /* welche Nummer hat das neue Modul? */
}



/*-----------------------------------------------------------------------------
 *  list_breakpoint()  --  listet die gesetzten breakpoints
 *-----------------------------------------------------------------------------
 */
void list_breakpoint()
{
 int i;

 clearscreen();                             /* Bildschirm loeschen */
 cursorxy(3,1);                             /* Cursor positionieren */
 printf("breakpoints:\n");
 for (i = 0; i < breaknr; )                 /* aktuellen breakpoints drucken */
      printf("   %s\n",breaklist[i++]);

 if (breakred)
     printf("\n\nBreakpoint auf Redcount-Stand %d\n",breakred);
}


/*------------------------------------------------------------------------------
 *  list_moduls  --  listet die aktuelle Aufrufhierarchie.
 *-----------------------------------------------------------------------------
 */
void list_moduls(fp)
FILE * fp;
{
 int i;

 if (fp == stdout)        /* auf Bildschirm listen? */
   {
    clearscreen();        /* Bildschirm loeschen */
    cursorxy(3,1);        /* Cursor positionieren */
   }
 fprintf(fp,"Aufrufhierarchie:\n");
 for ( i = 0; i < modulnr; )                /* Aufrufhierarchie drucken */
     fprintf(fp,"   %s\n",modullist[i++]);
 if (fp != stdout) fprintf(fp,"\n\n\n");
}


#if MEASURE

int messtab[NO_ALG+1][MAXMESS+1];      /* Messtabelle */

#define _M_HEAP 16
#define _MAX_H  256
#define _DIV_H  (_MAX_H / _M_HEAP)

int messheap[2][_M_HEAP+1];
#define _M_STACK 8
int mstack[_M_STACK];     /* max. stack-groesse      */
int minc  = 0;            /* max. ink.block-groesse  */
int mheap = 0;            /* max. heap-groesse       */
int mdesc = 0;            /* max. anzahl deskr.      */
static int msize = 0;     /* mom. heap-groesse       */


int fetch_modul(p)
char *p;
{
   if (modulnr < 0) return(NO_ALG);         /* ausserhalb des Messraumes */
                                            /* z.B. Initialisierung */
   if (strcmp(p,"ear"      ) == 0)  return (EAR);
   if (strcmp(p,"beta_collect") == 0)  return (COLLECT_ARGS);
   if (strcmp(p,"delta_collect") == 0)  return (COLLECT_ARGS);
   if (strcmp(p,"closecond") == 0)  return (CLOSECOND);
   if (strcmp(p,"red_plus" ) == 0)  return (RED_PLUS);
   if (strcmp(p,"red_minus") == 0)  return (RED_MINUS);
   if (strcmp(p,"red_mult" ) == 0)  return (RED_MULT);
   if (strcmp(p,"red_div"  ) == 0)  return (RED_DIV);
   if (strcmp(p,"red_mod"  ) == 0)  return (RED_MOD);
   if (strcmp(p,"red_ip") == 0)       return(RED_IP);
   if (strcmp(p,"red_abs"  ) == 0)  return (RED_ABS);
   if (strcmp(p,"red_trunc") == 0)  return (RED_TRUNC);
   if (strcmp(p,"red_frac" ) == 0)  return (RED_FRAC);
   if (strcmp(p,"red_floor") == 0)  return (RED_FLOOR);
   if (strcmp(p,"red_ceil" ) == 0)  return (RED_CEIL);
   if (strcmp(p,"red_eq"   ) == 0)  return (RED_EQ);
   if (strcmp(p,"red_le"   ) == 0)  return (RED_LE);
   if (strcmp(p,"red_lt"   ) == 0)  return (RED_LT);
   if (strcmp(p,"red_not"  ) == 0)  return (RED_NOT);
   if (strcmp(p,"red_or"   ) == 0)  return (RED_OR);
   if (strcmp(p,"red_and"  ) == 0)  return (RED_AND);
   if (strcmp(p,"red_xor" ) == 0)  return(RED_XOR);
   if (strcmp(p,"red_select") == 0)  return (RED_SELECT);
   if (strcmp(p,"red_rotate") == 0)  return (RED_ROTATE);
   if (strcmp(p,"red_cut"   ) == 0)  return (RED_CUT);
   if (strcmp(p,"red_unite") == 0)  return (RED_UNITE);
   if (strcmp(p,"red_transpose") == 0)  return (RED_TRANSPOSE);
   if (strcmp(p,"red_replace") == 0)  return (RED_REPLACE);
   if (strcmp(p,"red_mcut") == 0)       return(RED_MCUT);
   if (strcmp(p,"red_mreplace") == 0)   return(RED_MREPLACE);
   if (strcmp(p,"red_mre_c") == 0)      return(RED_MRE_C); /* replace column */
   if (strcmp(p,"red_mre_r") == 0)      return(RED_MRE_R); /* replace row    */
   if (strcmp(p,"red_mrotate") == 0)    return(RED_MROTATE);
   if (strcmp(p,"red_mselect") == 0)    return(RED_MSELECT);
   if (strcmp(p,"red_munite") == 0)     return(RED_MUNITE);
   if (strcmp(p,"red_vcut") == 0)       return(RED_VCUT);
   if (strcmp(p,"red_vselect") == 0)    return(RED_VSELECT);
   if (strcmp(p,"red_vreplace") == 0)   return(RED_VREPLACE);
   if (strcmp(p,"red_vrotate") == 0)    return(RED_VROTATE);
   if (strcmp(p,"red_vunite") == 0)     return(RED_VUNITE);
   if (strcmp(p,"red_substr") == 0)     return(RED_SUBSTR);
   if (strcmp(p,"red_repstr") == 0)     return(RED_REPSTR);
   if (strcmp(p,"red_neg") == 0)  return (RED_NEG);
   if (strcmp(p,"red_min") == 0)  return (RED_MIN);
                 /* LETREC         */
   if (strcmp(p,"lpa_sub") == 0) return(LPA_SUB);
   if (strcmp(p,"set_protect") == 0) return(SET_PROTECT);
   if (strcmp(p,"ldel_expr") == 0) return(LDEL_EXPR);
   if (strcmp(p,"lte_freev") == 0) return(LTE_FREEV);
   if (strcmp(p,"lr1eapre") == 0) return(1);
   if (strcmp(p,"lr_close") == 0) return(1);
   if (strcmp(p,"lfun_def") == 0) return(LFUN_DEF);
   if (strcmp(p,"check_ea") == 0) return(CHECK_EA);
   if (strcmp(p,"trav_goal") == 0) return(TRAV_GOAL);
   if (strcmp(p,"lrecfunc") == 0) return(1);
   if (strcmp(p,"ret_nint") == 0) return(RET_NINT);
   if (strcmp(p,"lst_lrec") == 0) return(LST_LREC);
   if (strcmp(p,"test_num") == 0) return(1);
   if (strcmp(p,"lrec_args") == 0) return(LREC_ARGS);
   if (strcmp(p,"earet_lrec") == 0) return(EARET_LREC);
   if (strcmp(p,"var_def") == 0) return(VAR_DEF);
   if (strcmp(p,"comp_name") == 0) return(COMP_NAME);
   if (strcmp(p,"aeret_lrec") == 0) return(2);
   if (strcmp(p,"test_lrec") == 0) return(TEST_LREC);
   if (strcmp(p,"check_goal_expr") == 0) return(CHECK_GOAL_EXPR);
   if (strcmp(p,"trav_goal") == 0) return(TRAV_GOAL);
   if (strcmp(p,"check_ea") == 0) return(CHECK_EA);
   if (strcmp(p,"check_ae") == 0) return(CHECK_AE);
   if (strcmp(p,"st_nint") == 0) return(ST_NINT);
   if (strcmp(p,"new_lrec") == 0) return(NEW_LREC);
   if (strcmp(p,"che_ptdd") == 0) return(CHE_PTDD);
   if (strcmp(p,"che_plargs") == 0) return(CHE_PLARGS);
   if (strcmp(p,"new_heapstruc") == 0) return(NEW_HEAPSTRUC);
   if (strcmp(p,"travae") == 0) return(TRAVAE);
   if (strcmp(p,"store_e") == 0) return(STORE_E);
   if (strcmp(p,"store_l") == 0) return(STORE_L);
   if (strcmp(p,"red_uni_tree") == 0)  return (RED_UNI_TREE);
   if (strcmp(p,"red_transform") == 0)  return (RED_TRANSFORM);
   if (strcmp(p,"reduct"    ) == 0)  return (REDUCT);
   if (strcmp(p,"failure"   ) == 0)  return (FAILURE);
   if (strcmp(p,"red_f_eq"  ) == 0)  return (RED_F_EQ);
   if (strcmp(p,"load_expr" ) == 0)  return (modul_id);     /* keine eigene Messung */
   if (strcmp(p,"red_lselect") == 0)  return (RED_LSELECT);
   if (strcmp(p,"red_lcut"  ) == 0)  return (RED_LCUT);
   if (strcmp(p,"red_lrotate") == 0)  return (RED_LROTATE);
   if (strcmp(p,"red_lunite" ) == 0)  return (RED_LUNITE);
   if (strcmp(p,"red_lreplace") == 0)  return (RED_LREPLACE);
   if (strcmp(p,"red_reverse") == 0)  return (RED_REVERSE);
   if (strcmp(p,"red_ldim"    ) == 0)  return (RED_LDIM);
   if (strcmp(p,"red_ltransform") == 0)  return (RED_LTRANSFORM);
   if (strcmp(p,"red_dim"    ) == 0)  return (RED_DIM);
   if (strcmp(p,"red_mdim") == 0)     return(RED_MDIM);
   if (strcmp(p,"red_vdim") == 0)     return(RED_VDIM);
   if (strcmp(p,"red_to_mat") == 0)   return(RED_TO_MAT);
   if (strcmp(p,"red_to_vect") == 0)  return(RED_TO_VECT);
   if (strcmp(p,"red_to_tvect") == 0) return(RED_TO_TVECT);
   if (strcmp(p,"red_to_scal") == 0)  return(RED_TO_SCAL);
   if (strcmp(p,"red_to_list") == 0)   return(RED_TO_LIST);
   if (strcmp(p,"red_to_field") == 0)  return(RED_TO_FIELD);
   if (strcmp(p,"red_to_string") == 0) return(RED_TO_STRING);
   if (strcmp(p,"red_empty"  ) == 0)  return (RED_EMPTY);
   if (strcmp(p,"red_class"  ) == 0)  return (RED_CLASS);
   if (strcmp(p,"red_type"   ) == 0)  return (RED_TYPE);
   if (strcmp(p,"red_sin"     ) == 0)  return (RED_SIN);
   if (strcmp(p,"red_cos"     ) == 0)  return (RED_COS);
   if (strcmp(p,"red_tan"     ) == 0)  return (RED_TAN);
   if (strcmp(p,"red_ln"     ) == 0)  return (RED_LN);
   if (strcmp(p,"red_exp"    ) == 0)  return (RED_EXP);
   if (strcmp(p,"pm_am"    ) == 0)  return (PM_AM);
   if (strcmp(p,"pm_dimli"    ) == 0)  return (PM_DIMLI);
   if (strcmp(p,"pm_close_nomat"    ) == 0)  return (PM_CLOSE_NOMAT);
   if (strcmp(p,"pm_closeswitch"    ) == 0)  return (PM_CLOSESWITCH);
   if (strcmp(p,"pm_scal_eq"    ) == 0)  return (PM_SCAL_EQ);
   if (strcmp(p,"pm_red_f_eq"    ) == 0)  return (PM_RED_F_EQ);
   if (strcmp(p,"pm_red_nomat"    ) == 0)  return (PM_RED_NOMAT);
   if (strcmp(p,"pmmat"    ) == 0)  return (PMMAT);
   if (strcmp(p,"pmredswi"    ) == 0)  return (PMREDSWI);
   if (strcmp(p,"red_dummy"  ) == 0)  return (modul_id); /* keine Messung */

   if (strcmp(p,"ea_create") == 0)  return (1);
   if (strcmp(p,"eac"      ) == 0)  return (1);
   if (strcmp(p,"subfunc"  ) == 0)  return (1);
   if (strcmp(p,"recfunc"  ) == 0)  return (1);
   if (strcmp(p,"closefunc") == 0)  return (1);
   if (strcmp(p,"rr1pr"    ) == 0)  return (1);
   if (strcmp(p,"r1pr"    ) == 0)   return (1);
   if (strcmp(p,"rr1pr"     ) == 0) return (1);
   if (strcmp(p,"st_string") == 0)  return (1);
   if (strcmp(p,"st_cond"  ) == 0)  return (1);
   if (strcmp(p,"st_dec"   ) == 0)  return (1);
   if (strcmp(p,"sts_dec")  == 0)  return (1);
   if (strcmp(p,"st_expr"  ) == 0)  return (1);
   if (strcmp(p,"st_mvt") == 0)     return(1); /* matrix / vector / tvector */
   if (strcmp(p,"st_scal"  ) == 0)  return (1);
   if (strcmp(p,"st_name"  ) == 0)  return (1);
   if (strcmp(p,"st_var"   ) == 0)  return (1);
   if (strcmp(p,"st_vare"  ) == 0)  return (1);
   if (strcmp(p,"st_list"  ) == 0)  return (1);
   if (strcmp(p,"vali"     ) == 0)  return (1);
   if (strcmp(p,"valr"     ) == 0)  return (1);
   if (strcmp(p,"check_list") == 0)  return (1);
   if (strcmp(p,"pm_out_befehl") == 0)  return (1);
   if (strcmp(p,"pm_gen") == 0)  return (1);
   if (strcmp(p,"pm_pattern") == 0)  return (1);
   if (strcmp(p,"pmprepa") == 0)  return (1);
   if (strcmp(p,"pm_Apatter") == 0)  return (1);
   if (strcmp(p,"pm_comp") == 0)  return (1);
   if (strcmp(p,"pm_patvar") == 0)  return (1);
   if (strcmp(p,"pm_st_match") == 0)  return (1);
   if (strcmp(p,"pm_st_switch") == 0)  return (1);
   if (strcmp(p,"pm_put") == 0)  return (1);
   if (strcmp(p,"pm_trav_spec") == 0)  return (1);
   if (strcmp(p,"pm_trav") == 0)  return (1);
   if (strcmp(p,"pm_prep") == 0)  return (1);
   if (strcmp(p,"pm_out_befehl") == 0)  return (1);

   if (strcmp(p,"ea_retrieve") == 0)  return (2);
   if (strcmp(p,"ae_retrieve") == 0)  return (2);
   if (strcmp(p,"pos_func"  ) == 0)  return (2);
   if (strcmp(p,"part_sub" ) == 0)  return (2);
   if (strcmp(p,"part_rec") == 0)  return (2);
   if (strcmp(p,"r1eapos"  ) == 0)  return (2);
   if (strcmp(p,"inc_expr" ) == 0)  return (2);
   if (strcmp(p,"ret_func" ) == 0)  return (2);
   if (strcmp(p,"ret_cond" ) == 0)  return (2);
   if (strcmp(p,"ret_dec"  ) == 0)  return (2);
   if (strcmp(p,"ret_expr" ) == 0)  return (2);
   if (strcmp(p,"ret_scal" ) == 0)  return (2);
   if (strcmp(p,"ret_mvt") == 0)    return(2); /* matrix / vector / tvector */
   if (strcmp(p,"ret_list" ) == 0)  return (2);
   if (strcmp(p,"ret_name" ) == 0)  return (2);
   if (strcmp(p,"ret_var"  ) == 0)  return (2);
   if (strcmp(p,"convi"    ) == 0)  return (2);
   if (strcmp(p,"convr"    ) == 0)  return (2);
   if (strcmp(p,"pm_ret_switch"    ) == 0)  return (2);
   if (strcmp(p,"pm_ret_match"    ) == 0)  return (2);
   if (strcmp(p,"pm_ret_nomatch"    ) == 0)  return (2);

   if (strcmp(p,"trav"     ) == 0)  return (modul_id);  /* keine eigene Messung */
   if (strcmp(p,"trav_a_e" ) == 0)  return (modul_id);
   if (strcmp(p,"trav_a_hilf") == 0)  return (modul_id);
   if (strcmp(p,"trav_e_a" ) == 0)  return (modul_id);
   if (strcmp(p,"gen_id")  == 0)       return (modul_id);      /* rlstruct.c */
   if (strcmp(p,"gen_many_id")  == 0)  return (modul_id);      /* rconvert.c */
   if (strcmp(p,"pm_element")  == 0)   return (modul_id);      /* rlogfunc.c */

   printf("\n\n\nModul nicht gefunden: %s\n",p);
   return(NO_ALG);
}


/*----------------------------------------------------------------------------
 * measure --  ein Aufruf bewirkt einen Eintrag in die Messtabelle
 *----------------------------------------------------------------------------
 */
void measure(l)
int l;
{
   messtab[modul_id][l]++;
}
 void m_init()
 {
  register int i,j;

  for (i = NO_ALG ; i >= 0 ; i-- )
    for ( j = MAXMESS ; j >= 0 ; j-- )
      messtab[i][j] = 0;
  for (i = _M_HEAP ; i >= 0 ; i-- ) {
    messheap[0][i] = 0;
    messheap[1][i] = 0;
  }
  for (i = _M_STACK-1 ; i >= 0 ; i-- ) {
    mstack[i] = 0;
  }
  minc = mheap = mdesc = msize = 0;
 }
 static int stacknr(s)
  char *s;
  {
   register int i;

   switch(*s) {
     case 'e' : i = 0; break;
     case 'a' : i = 1; break;
     case 'm' : if (*++s) i = 3; else i = 2;         break;
     case 'v' : i = 4; break;
     case 'i' : i = 5; break;
     case 'h' : i = 6; break;
     case 'p' : i = 7; break;
     default  :
       printf("stack_nr: id = \"%s\"\n",s);
       post_mortem("unknown stack-id");
   }
   return(i);
  }
 void messta(s,size)
  char *s;
  int size;
  {
   register int i,m;

   if (modul_id > 2) {    /* nur processing-anforderungen     */
     i = stacknr(s);
     if ((m = mstack[i]) < size) mstack[i] = size;
   }
  }

 void mfree(size)
  int size;
  {
   msize -= size;
  }

 void m_heap(l,size)
  int l,size;
  {
   int i;

   measure(l);
   msize += size;
   if (modul_id > 2) {    /* nur processing-anforderungen     */
     if (msize > mheap) mheap = msize;
     i = (l == MS_NEWHEAP) ? 0 : 1;
     if (size <= _MAX_H) {
       messheap[i][ size / _DIV_H ]++;
      }
     else {
       messheap[i][_M_HEAP]++;
     }
   }
  }


/*-------------------------------------------------------------------------------
 *  druck_measure  --  Ausdrucken der Messtabelle. Den Befehl dazu (monitor_call)
 *                     am besten nach verpointerten Wiederaufsetzen geben.
 */
int druck_measure()
{
   int i,j;
   int res[MAXMESS+1],dr;
   FILE *fp;

   if ((fp = fopen("protokoll.mess","w")) == NULL)
      if ((fp = fopen("protokoll.mess","a")) == NULL)
         WRONGINPUT(PROTOKOLL.MESS konnte nicht geoeffnet werden);
      else;
   else;
   for (i=0; i<= MAXMESS; i++)
      res[i] = 0;
   for ( i=0; i<=MAXMESS; i++)
     for ( j = 0 ; j <= NO_ALG ; j++)
         res[i] += messtab[j][i];

            fprintf(fp,"          ***  M E S S E R G E B N I S S E  ***\n\n\n\n");
            fprintf(fp,"            |  CALL|  FAIL|   POP|  PUSH|  READ| WRITE|  IPOP| IPUSH| IREAD|IWRITE|NEWDES|REFCNT|DESCRE|NWHEAP|HSEARCH|LOAD_EX|STORE_E\n");

               fprintf(fp,"Gesamt      |");
               for (j=0; j<=MAXMESS; j++)  fprintf(fp,"%6d|",res[j]);
               fprintf(fp,"\n");
            for (i = 0; i <= 2; i++) {
              dr = FALSE;
              for (j=0; j<=MAXMESS; j++)  dr |= messtab[i][j];
              if (dr) {
                fprintf(fp,"%-12s|",name[i]);
                for (j=0; j<=MAXMESS; j++)  fprintf(fp,"%6d|",messtab[i][j]);
                fprintf(fp,"\n");
              }
            }


            for (i = 10; i <= BIGGEST_NUM; i++) {
              dr = FALSE;
              for (j=0; j<=MAXMESS; j++)  dr |= messtab[i][j];
              if (dr) {
                fprintf(fp,"%-12s|",name[i]);
                for (j=0; j<=MAXMESS; j++) fprintf(fp,"%6d|",messtab[i][j]);
                fprintf(fp,"\n");
              }
            }

            dr = FALSE;
            for (j=0; j<=MAXMESS; j++)  dr |= messtab[i][j];
            if (dr) {
              fprintf(fp,"NO_ALG      |");
              for (j=0; j<=MAXMESS; j++)  fprintf(fp,"%6d|",messtab[NO_ALG][j]);
              fprintf(fp,"\n");
            }
   fprintf(fp,"-------------- Heap- Anforderungen -----------------\n");
   for (i = 1 ; i <= _M_HEAP ; i++ ) {
     fprintf(fp," -%3d|",i * _DIV_H);
   }
   fprintf(fp,">%3d\n",_MAX_H);
   for (i = 0 ; i <= _M_HEAP ; i++ ) {
     fprintf(fp,"%5d|",messheap[0][i]);
   }
   fputc('\n',fp);
   for (i = 0 ; i <= _M_HEAP ; i++ ) {
     fprintf(fp,"%5d|",messheap[1][i]);
   }
   fprintf(fp,"\n\n");
   fprintf(fp,"-------------- Weitere Groessen --------------------\n");
   fprintf(fp,"         E|    A|    M|   M1|    V|    I| Hilf|\nMax.:");
   for (i = 0 ; i < _M_STACK ; i++ ) {
     fprintf(fp,"%5d|",mstack[i]);
   }
   fputc('\n',fp);
   fprintf(fp,"Max. Ink.blockgroesse: %6ld\n",minc);
   fprintf(fp,"Max. Heapgroesse     : %6ld\n",mheap);
   fprintf(fp,"Max. Anzahl Desk.    : %6ld\n",mdesc);
   fclose(fp);
 }

#endif  /*  MEASURE */



#else /* DEBUG */
int sbreakdummy() { return(0); }   /* RS 02/11/92 */ 
#endif /* DEBUG  */
 /* end of rbreak.c */
