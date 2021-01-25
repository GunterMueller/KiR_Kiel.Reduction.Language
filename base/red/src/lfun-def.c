/* $Log: lfun-def.c,v $
 * Revision 1.3  1993/09/01 12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:48:58  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*-----------------------------------------------------------------------------
 *  lfun-def.c  - external:  lfun_def;
 *                internal:  check_ea;
 *-----------------------------------------------------------------------------
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
extern void ldel_expr();           /* ldel-expr.c */
extern void lrec_args();           /* lr-args.c */
static T_HEAPELEM *check_ea();     /* lfun_def.c */

extern T_HEAPELEM * newbuff();
extern void ret_func();
extern void ret_cond();
extern void ret_list();
extern void ret_mvt();           /* rhinout.c */
extern void ret_expr();

/* RS 30/10/92 */ 
extern void trav_a_e();                   /* rtrav.c */
extern void stack_error();                /* rstack.c */
extern void freeheap();                   /* rheap.c */
extern void pm_ret_switch();              /* retrieve.c */
extern void pm_ret_match();               /* retrieve.c */
extern void pm_ret_nomatch();             /* retrieve.c */
extern void trav_e_a();                   /* rtrav.c */
/* END of RS 30/10/92 */ 

#if DEBUG
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
extern void display_stack(); /* TB, 4.11.1992 */   /* rruntime.c */
#endif

/*-----------------------------------------------------------------------------
 *  lfun_def
 *-----------------------------------------------------------------------------
 */
T_HEAPELEM * lfun_def()
{
  PTR_DESCRIPTOR px,pd=(PTR_DESCRIPTOR)0; /* Initialisierung von TB, 6.11.1992 */
  STACKELEM  nf,x;
  T_HEAPELEM *tab,*f_tab;
  int funcs,tab_dim,j,i;

  START_MODUL("lfun_def");

  x = READSTACK(S_a);              /* plrec(_args) */
  px = (PTR_DESCRIPTOR)x;
  if (R_DESC((*px),type) == TY_LREC)
     pd = px;
  else
  if (R_DESC((*px),type) == TY_LREC_ARGS)
     pd = (PTR_DESCRIPTOR)R_LREC_ARGS((*px),ptdd);
  else {
     post_mortem("lfun_def:illegal lrec-type");
  }
  funcs = R_LREC((*pd),nfuncs);
                                /* initialisiere tab und f_tab : */
  tab_dim = funcs + 2;
  if ((tab = newbuff(tab_dim)) == NULL)
     post_mortem("lfun_def:heap out of space");
  tab[0] = (T_HEAPELEM)pd;   /* plrec */
  tab[1] = (T_HEAPELEM)px;   /* plrec(_args) */
  for (i = 2; i < tab_dim; i++)
     tab[i] = 0;

  if ((f_tab = newbuff(funcs)) == NULL)
     post_mortem("lfun_def:heap out of space");
  for (i = 0; i < funcs; i++)
     f_tab[i] = 0;

#if PRINT
fprintf(stderr,"newline          \n");
fprintf(stderr,"newline          \n");
fprintf(stderr,"newline          \n");
fprintf(stderr,"tab[0] = %08lx          \n",tab[0]);
fprintf(stderr,"tab[1] = %08lx          \n",tab[1]);
for (j = 2; j < tab_dim;j++)
fprintf(stderr,"tab[%ld] = %ld          \n",j,tab[j]);
#endif
                      /* fuehre check_ea aus */
  check_ea(tab,1);    /* erster Aufruf : traversiere goal_expr und suche alle */
                      /*                 plrec_ind : tab[index]++             */
#if PRINT
fprintf(stderr,"newline          \n");
fprintf(stderr,"newline          \n");
fprintf(stderr,"newline          \n");
fprintf(stderr,"tab[0] = %08lx          \n",tab[0]);
fprintf(stderr,"tab[1] = %08lx          \n",tab[1]);
for (j = 2; j < tab_dim;j++)
fprintf(stderr,"tab[%ld] = %ld          \n",j,tab[j]);
#endif
/*---------------------------------------------------------------------------*/
  TRAV_A_E;
/*---------------------------------------------------------------------------*/
  if (T_PRELIST(READSTACK(S_m))) {  /* falls goal == plrec_ind */
     PPOPSTACK(S_m);
     PPOPSTACK(S_e); /* TB, 2.11.1992 */ /* plrec_ind wieder entfernen ! */
  }
  /* else ;     auskommentiert RS 18.1.1993 */

  j = 0;
  while (j < funcs) {
     j = 0;
     while ( (j < funcs)
           && ( (tab[j + 2] == 0) || (f_tab[j] != 0) ) )
            /* func_def wird nicht referenziert oder ist schon traversiert */
        j++;
     if (j < funcs) {
        f_tab[j]++;                  /* func_def wird jetzt traversiert */
        nf = (STACKELEM)R_LREC((*pd),ptdv)[j];
        if (T_POINTER(nf))
           INC_REFCNT((PTR_DESCRIPTOR)nf);
        /* else ;      auskommentiert RS 18.1.1993 */
        PUSHSTACK(S_e,nf);            /* check_ea fuer jedes pl_org- */
        check_ea(tab,0);              /* func_def einzeln            */
        ldel_expr(&S_a);              /* func_def nf */
     }
     /* else ;          auskommentiert RS 18.1.1993 */
  }        /* end while */
  freeheap(f_tab);
#if PRINT
fprintf(stderr,"newline          \n");
fprintf(stderr,"newline          \n");
fprintf(stderr,"newline          \n");
fprintf(stderr,"tab[0] = %08lx          \n",tab[0]);
fprintf(stderr,"tab[1] = %08lx          \n",tab[1]);
for (j = 2; j < tab_dim;j++)
fprintf(stderr,"tab[%ld] = %ld          \n",j,tab[j]);
#endif
  END_MODUL("lfun_def");
  return (tab);
        /* first version : lfun_def uebergibt tab an ea_retrieve :
                           func_defs sowie func_names werden nur
                           fuer  tab[ind] != 0  rekonstruiert !
                           (earet_lrec entfaellt in dieser Version ! ) */
        /* neue heap-Strukturen anlegen wuerde bedeuten, dass auch wieder
           plrec_ind's ausgetauscht werden muessten                      */
}       /* end of lfun_def */

/*$P*/
/*----------------------------------------------------------------------------*/
/*  check_ea() --  loest C_LIST und TY_EXPR, sowie temporaer
 *                 TY_LREC, TY_LREC_ARGS, TY_COND, TY_SUB und TY_REC auf
 *  globals    --  S_e,S_a,S_m,S_m1 <w>
 *----------------------------------------------------------------------------*/

static T_HEAPELEM *check_ea(tab,goal)
   T_HEAPELEM *tab;
   int goal;              /* goal == 1 : goal_expr wird traversiert :      */
                          /*             ersetze (solange kein PRELIST auf */
                          /*             S_m steht) #-variablen durch args */
                          /*             aber nur temporaer !              */
                          /*             diese ersetzungen muessen wg.     */
                          /*             ea_retrieve wieder rueckgaengig   */
                          /*             gemacht werden                    */
                          /* goal == 0 : func_def's : keine ersetzung      */
                          /* SWITCH, NOMATCH setzen goal auf 0, falls sie  */
                          /* nachtraeglich geschlossen worden sind; sonst  */
                          /* bleibt goal wie bei match unveraendert        */
{
  PTR_DESCRIPTOR px,w;
  STACKELEM  x,y,wz,na,nf;
  int funcs,args,i,ind;
  char class1;

  START_MODUL("check_ea");

  PUSHSTACK(S_m,KLAA);             /* Endemarkierung @ */

  funcs = 0;
  args = 0;
  ind = 0;

main_ea:
  x = POPSTACK(S_e);

  if (T_NUM(x)) {
     if (goal) {          /* ersetzung ausfuehren */
        READ_I(VALUE(x),y);
        if (T_NUM(y)) {   /* args ist eine #-variable */
           PUSHSTACK(S_a,x);       /* fuehre ersetzung nicht aus */
           goto main_mea;
        }
        else {            /* weiter mit check_ea fuer args */
           PUSHSTACK(S_e,y);
           PUSHSTACK(S_a,x);
           PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));   /* rette wert von goal */
                                   /* goal wird jedoch nicht veraendert     */
           goto main_ea;
        }
     }
     else {               /* keine ersetzung */
        PUSHSTACK(S_a,x);
        goto main_mea;
     }
  }
  else
  if (T_CON(x)) {
     if (T_PM_WHEN(x)) {
        TRAV_E_A;         /* pattern_expr */
        PUSHSTACK(S_m,DEC(x));
        PUSHSTACK(S_m1,x);
        goto main_ea;
     }
     else {
        PUSHSTACK(S_m,x);
        PUSHSTACK(S_m1,x);
        goto main_ea;
     }
  }
  else
  if (T_POINTER(x)) {
     px = (PTR_DESCRIPTOR)x;
     switch(R_DESC((*px),type)) {
      case TY_LREC_IND: {          /* vergleiche p_lrec_ind@.ptdd mit */
                                   /* tab[0] == px :                  */
                                   /* Gleichheit: tab[index]++        */
                          w = (PTR_DESCRIPTOR)R_LREC_IND((*px),ptdd);/* plrec */
                          if (w == (PTR_DESCRIPTOR)tab[0]) {                         /* plrec */
                             ind = R_LREC_IND((*px),index);
                             tab[ind + 2]++;
                          }
                        /*   else ;       / plrec_ind gehoert nicht zu plrec pd /  auskommentiert  RS 18.1.1993 */
                          PUSHSTACK(S_a,x); /* p_lrec_ind */
                          goto main_mea;
                        }
      case TY_LREC:      {
                            PUSHSTACK(S_a,x);        /* p_lrec */
                            PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                            goal = 0;
                            funcs = R_LREC((*px),nfuncs);
                            for (i = funcs - 1; i >= 0; i--) {
                               nf = (STACKELEM)R_LREC((*px),ptdv)[i];
                               PUSHSTACK(S_e,nf);
                               if (T_POINTER(nf))
                                  INC_REFCNT((PTR_DESCRIPTOR)nf);
                            }
                            PUSHSTACK(S_e,SET_ARITY(LIST,funcs));
                                   /* listenkonstruktor fuer func_defs */
                            goto main_ea;
                         }
      case TY_LREC_ARGS: {
                            PUSHSTACK(S_a,x);        /* p_lrec_args */
                            PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                            goal = 0;
                            wz = (STACKELEM)R_LREC_ARGS((*px),ptdd);
                            PUSHSTACK(S_e,wz);
                            args = R_LREC_ARGS((*px),nargs);
                            for (i = 0; i < args; i++) {
                                na = (STACKELEM)R_LREC_ARGS((*px),ptdv)[i];
                                PUSHSTACK(S_e,na);
                                if (T_POINTER(na))
                                   INC_REFCNT((PTR_DESCRIPTOR)na);
                            }
                            PUSHSTACK(S_e,SET_ARITY(SNAP,args + 1));
                            goto main_ea;
                         }
      case TY_EXPR:
                                      /* PRELIST wg. SNAP-args in plrec_args */
                                      /* wenn hier die verweise entfernt     */
                                      /* werden, muss in ea_retrieve()/TY_   */
                                      /* PLREC_IND ein literaler Vergleich   */
                                      /* von EXPR ausgefuehrt werden !       */
                                      /* (in lr_args() gibt es keine kon-    */
                                      /* struktoren in den SNAP-args mehr !) */
                         PUSHSTACK(S_a,x);           /* p_expr */
                         PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                         INC_REFCNT(px);
                         ret_expr(x);       /* expression einlagern */
                         goto main_ea;
      case TY_REC:
      case TY_SUB:       PUSHSTACK(S_a,x);
                         INC_REFCNT(px);
                         PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                         goal = 0;
                         ret_func(x);
                         goto main_ea;
      case TY_COND:      PUSHSTACK(S_a,x);
                         INC_REFCNT(px);
                         PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                         goal = 0;
                         ret_cond(x);
                         goto main_ea;
      case TY_SWITCH:    {                  /* case-konstrukt */
                            PUSHSTACK(S_a,x);
                            INC_REFCNT(px);
                            PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                            if (T_SNAP(READSTACK(S_m1)))
                               goal = 0;
                            /* else ;       auskommentiert RS 18.1.1993 */
                                   /* goal wird nicht veraendert wg. */
                                   /* schliessen von when_expr       */
                            pm_ret_switch(x);
                            goto main_ea;
                         }
      case TY_MATCH:     {                  /* when_expr */
                            PUSHSTACK(S_a,x);
                            INC_REFCNT(px);
                            PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                                   /* goal wird nicht veraendert wg. */
                                   /* schliessen von when_expr       */
                            pm_ret_match(x);
                            goto main_ea;
                         }
      case TY_NOMAT:     {                  /* no_match-deskriptor */
                            PUSHSTACK(S_a,x);
                            INC_REFCNT(px);
                            PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                            if (T_SNAP(READSTACK(S_m1)))
                               goal = 0;
                            /* else ;    auskommentiert RS 18.1.1993 */
                                   /* goal wird nicht veraendert wg. */
                                   /* schliessen von when_expr       */
                            pm_ret_nomatch(x);
                            goto main_ea;
                         }
      case TY_STRING :   class1 = R_DESC((*px),class);
                         switch(class1) {
                           case C_VECTOR :     /* if vect<'abc`> */
                           case C_TVECTOR:
                           case C_MATRIX :
                              ret_mvt(x,class1);
                              break;
                           case C_LIST :
                              ret_list(x);     /* nilstring einlagern */
                              break;
                           default :
                              post_mortem("check_ea: unexpected pointer");
                            }
                         goto main_ea;
      case TY_UNDEF:     if ((class1 = R_DESC((*px),class)) == C_LIST) {
                                      /* PRELIST wg. SNAP-args in plrec_args */
                                      /* wenn hier die verweise entfernt     */
                                      /* werden, muss in ea_retrieve()/TY_   */
                                      /* PLREC_IND ein literaler Vergleich   */
                                      /* von LIST ausgefuehrt werden !       */
                                      /* (in lr_args() gibt es keine kon-    */
                                      /* struktoren in den SNAP-args mehr !) */
                           PUSHSTACK(S_a,x);         /* p_list */
                           PUSHSTACK(S_m,SET_ARITY(PRELIST,goal));
                                            /* rette wert von goal */
                           INC_REFCNT(px);
                           ret_list(x);     /* Liste einlagern */
                         }
                         else
                           switch(class1) {
                             case C_VECTOR :           /* nilvect  einlagern */
                             case C_TVECTOR:           /* niltvect einlagern */
                             case C_MATRIX :           /* nilmat   einlagern */
                                   ret_mvt(x,class1);
                                   break;
                             default       :
                                   post_mortem("check_ea: unexpected pointer");
                           }
                         goto main_ea;
      default:           PUSHSTACK(S_a,x);
                         goto main_mea;
    }
  }
  else
  {
    PUSHSTACK(S_a,x);
    goto main_mea;
  }

main_mea:
  x = READSTACK(S_m);

  if (T_PRELIST(x)) {
     goal = ARITY(POPSTACK(S_m));  /* restauriere wert von goal */
     ldel_expr(&S_a);
     goto main_mea;
  }
  else
  if (T_CON(x))
     if (ARITY(x) > 1)  /* dh die Arity des Konstruktors auf M war >= 2 */
     {
       WRITESTACK(S_m,DEC(x));            /* das uebliche */
       goto main_ea;
     }
     else
     {
       PPOPSTACK(S_m);
       PUSHSTACK(S_a,POPSTACK(S_m1));
       goto main_mea;
     }
  else
  if (T_KLAA(x))   /* goal_expr vollstaendig traversiert */
     PPOPSTACK(S_m);
  else {
     post_mortem("check_ea:no match successful on main_mea");
  }

  END_MODUL("check_ea");
  return (tab);
}       /* end of check_ea */

/* end of file lfun-def.c */
