/* $Log: lr-ret.c,v $
 * Revision 1.2  1992/12/16  12:49:10  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */


/*----------------------------------------------------------------------------
 * ret-lrec:c - external: earet_lrec,
 *                        aeret_lrec,
 *                        comp_name,
 *              internal: var_def
 *----------------------------------------------------------------------------
 */
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

 /*-----------------------------------------------------------------------------
  * Spezielle externe Routinen:
  *-----------------------------------------------------------------------------
  */
extern void ret_nint();            /* lrecfunc.c */
extern void aeret_lrec();          /* lr-ret.c */
extern int comp_name();            /* lr-ret.c */
static void var_def();             /* lr-ret.c */

/* ach 30/10/92 */
extern void stack_error();
/* end of ach */

#if DEBUG
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
#endif

 /*-----------------------------------------------------------------------------
   * earet_lrec --
   * globals --  S_e           <w>
   *-----------------------------------------------------------------------------
   */
#if DORSY
int earet_lrec(first,ste,tab)
    int first;
    STACKELEM ste;
    T_HEAPELEM * tab;
#else
int earet_lrec(first,ste)
    int first;
    STACKELEM ste;
#endif
 {
    PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) ste;
    STACKELEM x/*,xd*/;                         /* ach 30/10/92 */
    int i,funcs;
#if DORSY
    STACKELEM y;
    int sfuncs;
#endif

    START_MODUL("earet_lrec");

    if (first) {     /* #if DORSY : first == 0 */
                     /* Rekonstruktion der "aeusseren" Funktionsdefinitionen:
                        u.U. mehrere p_lrec's                               */
       PUSHSTACK(S_hilf,KLAA);
       PUSHSTACK(S_a,ste);                       /* erstes p_lrec */
       while (!(T_KLAA(READSTACK(S_a))))         /* fuer alle p_lrec's : */
       {
          desc = (PTR_DESCRIPTOR)POPSTACK(S_a);
          PUSHSTACK(S_hilf,(STACKELEM)desc);
                                   /* p_lrec's werden auf S_hilf gesammelt,
                                      da sie noch fuer die Rekonstruktion
                                      der Funktionsnamen benoetigt werden  */
#if DEBUG
          if (R_DESC((*desc),type) != TY_LREC)  /* auf S_a liegen nur plrec's */
          {
             post_mortem("earet_lrec: illegal lrec type");
          }
       /* else : */
#endif
          funcs = R_LREC((*desc),nfuncs);
          var_def(desc,funcs);      /* func_var's auf S_v, func_def's auf S_e */
       }   /* end while */
       while (!(T_KLAA(x = POPSTACK(S_hilf))))    /* p_lrec's */
          PUSHSTACK(S_a,x);
    }
    else {           /* (! first) : innere plrec(_args) oder DORSY */
       funcs = R_LREC((*desc),nfuncs);
#if DORSY
       sfuncs = funcs;
       for (i=funcs-1; i >= 0; i--)
       {                                       /* func_def's auf S_e */
           if (tab[i + 2] != 0) {
              y = (STACKELEM)R_LREC((*desc),ptdv)[i];
              PUSHSTACK(S_e,y);
              if (T_POINTER(y))
                 INC_REFCNT((PTR_DESCRIPTOR)y);
           }
           else
              sfuncs--;
       }
       if (sfuncs) {      /* rekonstruiere func_defs und func_names */
          PUSHSTACK(S_e,SET_ARITY(LIST,sfuncs));
                          /* Listenkonstruktor fuer die Funktionsdefinitionen */
          for (i=1 ; i <= funcs; i++)      /* func_vars auf S_a */
          {
              if (tab[i + 1] != 0) {
                 y = (STACKELEM)R_LREC((*desc),namelist)[i];
                 PUSHSTACK(S_a,y);
                 INC_REFCNT((PTR_DESCRIPTOR)y);
              }
       /*     else ;             auskommentiert von RS 17.11.1992 */
           }
          PUSHSTACK(S_a,SET_ARITY(LIST,sfuncs));
                                 /* Listenkonstruktor fuer die Funktionsnamen */
       }         /* end : sfuncs */
       else {    /* entferne LREC-Konstruktor auf S_m und S_m1 */
                 /* sowie plrec(_args) auf S_hilf              */
          PPOPSTACK(S_m); /* TB, 30.10.92 */          /* LREC */
          PPOPSTACK(S_m1); /* TB, 30.10.92 */         /* LREC */
          PPOPSTACK(S_hilf); /* TB, 30.10.92 */       /* plrec(_args) */
       }
#else
       PUSHSTACK(S_a,ste);                     /* plrec */
       i = 0;
       while ( (R_LREC((*desc),namelist)[funcs+i] == NULL)
             && (i <= funcs) )
          i++;         /* gibt es func_def's, die restauriert werden muessen? */
       if (i > funcs)
       {               /* es gibt keine */
          xd = POPSTACK(S_a);                      /* plrec */
          DEC_REFCNT((PTR_DESCRIPTOR)xd);
          xd = POPSTACK(S_hilf);                   /* plrec(_args) */
          DEC_REFCNT((PTR_DESCRIPTOR)xd);
          POPSTACK(S_m);           /* LREC */
          POPSTACK(S_m1);          /* LREC */
          END_MODUL("earet_lrec");
          return (0);
       }
       else            /* es gibt mindestens ein func_def */
/*-----------------------------------------------------------------------------
 * es ist nicht mehr notwendig, hier die func_names zu vergleichen, da dies
 * bereits in new_lrec ausgefuehrt worden ist (Anlegen neuer heap-Strukturen
 * fuer Funktionsgleichungen)
 *-----------------------------------------------------------------------------
 *   extern STACKELEM _sep_int,_sep_ext;
 *                         * '_'-Zeichen werden an den Editor uebergebenn    *
 *                         * und im weiteren nicht mehr veraendert;          *
 *                         * '!'-Zeichen dienen nur intern der Verkettung    *
 *                         * von Funktionsnamen;                             *
 *                         * sie werden - falls moeglich - entfernt zusammen *
 *                         * mit dem Rest des Namens oder - falls notwendig -*
 *                         * in '_'-Zeichen umgewandelt                      *
 *    if (!(T_KLAA(READSTACK(S_v))))
 *    {
 *       fname = (PTR_DESCRIPTOR) R_LREC((*desc),namelist)[funcs+1];
 *                                  * erster "interner" Funktionsname *
 *       PUSHSTACK(S_a,KLAA);
 *
 *       k = R_NAME((*fname),ptn)[0];
 *       while ( (k > 0) && (R_NAME((*fname),ptn)[k] != _sep_int) )
 *          k--;      * _sep_int steht weder als erstes noch letztes Zeichen ! *
 *       k--;
 *       if (k > 0)
 *       {
 *          PUSHSTACK(S_a,TOGGLE(R_NAME((*fname),ptn)[k]));
 *          for (k--; k > 0; )
 *              PUSHSTACK(S_a,R_NAME((*fname),ptn)[k--]);
 *       }
 *
 *       if (!(T_KLAA(READSTACK(S_a))))
 *       {
 *          PUSHSTACK(S_e,KLAA);
 *          v = (PTR_DESCRIPTOR) READSTACK(S_v);
 *          ret_nint(v);             * auf S_e *
 *       }
 *
 *       if (!(comp_name(&S_e,&S_a)))
 *       {
 *          for (j=1; j <= funcs; j++)
 *          {
 *              v = (PTR_DESCRIPTOR)R_LREC((*desc),namelist)[funcs+j];
 *              L_LREC((*desc),namelist)[j] = v;
 *              INC_REFCNT(v);
 *          }
 *                                          * (neue) func_vars von             *
 *                                          * funcs+1 ... 2*funcs restaurieren *
 *       }
 *
 *    }     * end if (!(T_KLAA(READSTACK(S_v)))) *
 *-----------------------------------------------------------------------------
*/
       {
          var_def(desc,funcs);
       }
#endif
    }   /* end (!first) */

    END_MODUL("earet_lrec");
    return (0);
 }      /* end of earet_lrec */

/*$P*/
 /*-----------------------------------------------------------------------------
   * var_def
   *
   *-----------------------------------------------------------------------------
   */
static void var_def(desc,funcs)
   PTR_DESCRIPTOR desc;
   int funcs;
{
   int sfuncs,i;
   STACKELEM y;

   START_MODUL("var_def");

   sfuncs = funcs;

   for (i=2*funcs ; i > funcs; i--)      /* "neue" func_vars auf S_v */
   {
       y = (STACKELEM)R_LREC((*desc),namelist)[i];
       if (y != NULL)
       {
          PUSHSTACK(S_v,y);
          INC_REFCNT((PTR_DESCRIPTOR)y);
       }
       else
          sfuncs--;
    }

   for (i=funcs-1; i >= 0; i--)
   {                                       /* func_def's auf S_e */
       y = (STACKELEM)R_LREC((*desc),ptdv)[i];
       if (y != NULL)
       {
          PUSHSTACK(S_e,y);
          if (T_POINTER(y))
             INC_REFCNT((PTR_DESCRIPTOR)y);
       }
 /*    else ;        auskommentiert von RS 17.11.1992 */
   }
   PUSHSTACK(S_e,SET_ARITY(LIST,sfuncs));   /* Listenkonstruktor fuer die
                                              Funktionsdefinitionen      */
   END_MODUL("var_def");
   return;
}      /* end of var_def */

/*$P*/
 /*-----------------------------------------------------------------------------
   * comp_name
   *
   *-----------------------------------------------------------------------------
   */
 int comp_name(source_a,source_b)
    StackDesc *source_a,*source_b;
 {
#define S_sa *source_a
#define S_sb *source_b
                          /* "die Stackroutinen (bzw. Makros) erwarten */
                          /* immer einen StackDesc" (trac:c)           */

    START_MODUL("comp_name");

    while ( (!(T_KLAA(READSTACK(S_sa)))) && (!(T_KLAA(READSTACK(S_sb))))
          && (POPSTACK(S_sa) == POPSTACK(S_sb)) )
       ;

    if ( (T_KLAA(READSTACK(S_sa))) && (T_KLAA(READSTACK(S_sb))) )
    {
       PPOPSTACK(S_sa);
       PPOPSTACK(S_sb);

       END_MODUL("comp_name");
       return (1);
    }
    else
    {
       while ( (!(T_KLAA(POPSTACK(S_sa)))) )
          ;
       while ( (!(T_KLAA(POPSTACK(S_sb)))) )
          ;
       END_MODUL("comp_name");
       return (0);
    }
 }      /* end of comp_name */

/*$P*/
 /*-----------------------------------------------------------------------------
   * aeret_lrec --
   * globals --  S_e           <w>
   *-----------------------------------------------------------------------------
   */
 void aeret_lrec(first)
   int first;
 {
    T_HEAPELEM y;
    PTR_HEAPELEM name;
    PTR_DESCRIPTOR desc;
    STACKELEM x;
    int i,funcs,sfuncs;

    START_MODUL("aeret_lrec");

 if (first)      /* ... p_lrec(_args) ... @ auf S_a */
 {
    PUSHSTACK(S_m,SET_ARITY(LIST,0));       /* Listenkonstruktor fuer die
                                               Funktionsnamen             */
    while (!(T_KLAA(READSTACK(S_a)))) {
       desc = (PTR_DESCRIPTOR)POPSTACK(S_a);          /* p_lrec(_args) */
       if (R_DESC((*desc),type) == TY_LREC) {
          funcs = R_LREC((*desc),nfuncs);
          sfuncs = funcs;

          name = (STACKELEM *)(R_LREC((*desc),namelist));
          for (i=funcs; i > 0; i--) {       /* func_names auf S_e */
             y = name[i];
             if (y != NULL) {
                PUSHSTACK(S_e,(STACKELEM)y);
                INC_REFCNT((PTR_DESCRIPTOR)y);
             }
             else
                sfuncs--;
          }

          x = READSTACK(S_m);
          WRITESTACK(S_m,SET_ARITY(x,(ARITY(x) + sfuncs)));
                                            /* LIST(... + funcs) */
     /*   DEC_REFCNT(desc);        muss wieder "ent"kommentiert werden !! */
       }
       else {
          post_mortem("aeret_lrec: illegal lrec type");
       }
    }
    PPOPSTACK(S_a);                /* KLAA */
    for (i = (ARITY(READSTACK(S_m))); i > 0; i--)
        PUSHSTACK(S_a,POPSTACK(S_e));       /* func_names */
    PUSHSTACK(S_a,POPSTACK(S_m));           /* LIST */
 }
 else            /* innere p_lrec(_args) */
 {
    x = POPSTACK(S_a);                      /* plrec */
    funcs = R_LREC((*(PTR_DESCRIPTOR)x),nfuncs);
    sfuncs = funcs;

    name = (STACKELEM *)(R_LREC((*(PTR_DESCRIPTOR)x),namelist));
    for (i=1; i <= funcs; i++) {
        y = name[i];
        if (y != NULL) {
           PUSHSTACK(S_a,(STACKELEM)y);
           INC_REFCNT((PTR_DESCRIPTOR)y);
        }
        else
           sfuncs--;
    }
    PUSHSTACK(S_a,SET_ARITY(LIST,sfuncs));
                                   /* Listenkonstruktor fuer die func_names */
/*  DEC_REFCNT((PTR_DESCRIPTOR)x); muss wieder "ent"kommentiert werden !! */
 } /* end first == 0 */

    WRITE_ARITY(S_m,1);                /* LREC(1) */
    END_MODUL("aeret_lrec");
    return;
 }      /* end of aeret_lrec */
/* end of        ret-lrec.c        */
