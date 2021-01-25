/*                                                        ctransl1.c   */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung: 12.05.89                                          */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include "cedit.h"
#include "cestack.h"
#include "cencod.h"
#include "ckwtb.h"
#include "ctrans.h"
#include "ctred.h"
#include "cscred.h"
#include "cfiles.h"
#include "cetb.h"
#include "cparm.h"

/* --- dg 19.05.92 letp in pi-red+ */
extern T_UEBERGABE Parms; /* cparm.h, cglobals.c */

extern char msg[],edfilbuf[],errfn[];

#if N_STR
#ifndef N_U_STR
#define N_U_STR 1
#endif
#endif

#define STD_END	0xffffffffL

static STACK *ST1,*ST2;
static int BACK,ergto;
static STACKELEM el1;
int doub_def = FALSE;

#define HST U3ST
#define PFIELD(x) push(ST2,x);incrarity(MST);for(n=arity(m);n>0;n--) to_red(); \
              push(ST2,(pop(MST) & VALUE_FIELD) | AR0(FIELD)); break;
#define PVECT(x) push(ST2,x);for(n=arity(m);n>0;n--) to_red(); \
              push(ST2,(pop(MST) & VALUE_FIELD) | AR0(LIST));  \
              push(ST2,toarity(2) | AR0(VECTOR)); break;
#define PTVECT(x) push(ST2,x);for(n=arity(m);n>0;n--) to_red(); \
              push(ST2,(pop(MST) & VALUE_FIELD) | AR0(LIST));   \
              push(ST2,toarity(2) | AR0(TVECTOR)); break;

static push_stdes(st1,st2)
STACK *st1,*st2;
{
  pushi((STACKELEM)(st1->pred),st2);
  pushi((STACKELEM)(st1->siz),st2);
  pushi((STACKELEM)(st1->cnt),st2);
  pushi((STACKELEM)(st1->stptr),st2);
}

static del_stdes(st1)
STACK *st1;
{
  ppop(st1);
  ppop(st1);
  ppop(st1);
  ppop(st1);
}

static mov1_stdes(st1,st2)
STACK *st1,*st2;
{
  push(st2,pop(st1));	/* nicht moven, da ev. STBOTTOM !   */
  push(st2,pop(st1));
  push(st2,pop(st1));
  push(st2,pop(st1));
}

static popnf_stdes(st1,st2)
STACK *st1,*st2;
{
  st1->pred  = (STACKELEM *)(popnf(st2));
  st1->siz  = (long)(popnf(st2));
  st1->cnt  = (long)(popnf(st2));
  st1->stptr  = (STACKELEM *)(popnf(st2));
#define ASSERT(c)	if (!(c)) { sprintf(lbuf,"%s %d %s",__FILE__,__LINE__,"c"); error(lbuf); } else
if (!((st1->cnt >= 0L) && (st1->cnt <= st1->siz)))
{
error("fehler");
}
ASSERT((st1->cnt >= 0L) && (st1->cnt <= st1->siz));
}

static move_stdes(st1,st2)
STACK *st1,*st2;
{
 register STACKELEM m;

  while ((m = pop(st1)) != STD_END) {
    push(st2,m);
    push(st2,pop(st1));
    push(st2,pop(st1));
    push(st2,pop(st1));
  }
}

/*****************************************************************************/
/* to_red : Uebersetzt die interne Darstellung des Editors in die            */
/*         interne Darstellung der Reduktionsmaschine (von ST1 nach ST2)     */
/* use: BST, U1ST                                                            */
/*****************************************************************************/

static to_red()
{
#if TRACE
 extern int trace;
#endif
 static STACKELEM m;
 static int c;
 long n;

 m = topof(ST1);
 if (isconstr(m)) {
    move(ST1,MST);
    switch(m & CLASS_FIELD) {
       case AR0(SYSAP)  :
       case AR0(AP)     : pushi(pop(MST) & ~EDIT_FIELD,MST);goto trav;
#ifdef UnBenutzt
       case AR0(APVAR)  : pushi(AR0(AP_VAR) | (pop(MST) & VALUE_FIELD),MST);
                          goto trav;
#endif
       case AR0(CALL)   :
         if ((n = arity(m)) == 2) {
           to_red();	/* fkt   */
           if (noedit(topof(ST1)) == NOPAR) {
             pop(ST1);	/* NOPAR */
             ppop(MST);	/* CALL           */
            }
           else {
             push(MST,AR0(AP_CALL)| (pop(MST) & VALUE_FIELD));
             push(BST,STD_END);
             push_stdes(ST2,BST);
             to_red();	/* argument       */
             push_stdes(ST2,BST);
             move_stdes(BST,ST2);
             move(MST,ST2);
           }
          }
         else {
           push(MST,AR0(AP_CALL)| (pop(MST) & VALUE_FIELD));
           pushi(STD_END,BST);
           for ( ; n > 0L ; --n) {
             to_red();	/* f,a_1..a_n  */
             push_stdes(ST2,BST);
           }
           move_stdes(BST,ST2);
           move(MST,ST2);
         }
         break;
/* special wg. head, tail	*/
       case AR0(CALL1)  :
         if (noedit(topof(EST)) == M_HEAD) {
           ppop(ST1);
           push(ST2,R_HEAD);
           to_red();
           push(ST2,AR0(AP_NORM)| (pop(MST) & VALUE_FIELD));
           break;	/* stop it	*/
         }
         if (noedit(topof(EST)) == M_TAIL) {
           ppop(ST1);
           push(ST2,R_TAIL);
           to_red();
           push(ST2,AR0(AP_NORM)| (pop(MST) & VALUE_FIELD));
           break;	/* stop it	*/
         }
	/* else: FALLTHROUGH !!	*/
       case AR0(CALL2)  :
       case AR0(CALL3)  :
       case AR0(CALL4)  : pushi(AR0(AP_PRED)| (pop(MST) & VALUE_FIELD),MST);
	/*   goto trav;      */
        trav : pushi(STD_END,BST);
          for (n = arity(m) ; n > 0L ; --n) {
             to_red();	/* f,a_1..a_n  */
             push_stdes(ST2,BST);
          }
          move_stdes(BST,ST2);
          move(MST,ST2);
          break;
       case AR0(E_DOT) :
         ppop(MST);
         to_red(); to_red();
         push(ST2,R_DOT);
         break;
       case AR0(E_MAP) :
         ppop(MST);
         push(ST2,MAP);
         to_red(); to_red();
         push(ST2,AR0(AP_NORM) | toarity(3));
         break;
       case AR0(E_MAP_) :
         ppop(MST);
         push(ST2,MAP_);
         to_red(); to_red();
         push(ST2,AR0(AP_NORM) | toarity(3));
         break;
       case AR0(E_ONEOF) :
         ppop(MST);
         push(ST2,ONEOF);
         to_red();
         push(ST2,AR0(AP_NORM) | toarity(2));
         break;
       case AR0(E_SUCCESS) :
         m = pop(MST);
         pushi(STD_END,BST);
         push(MST,AR0(AP_SUCC) | toarity(2));
         push(ST2,_SUCCESS);
         push_stdes(ST2,BST);
         to_red();
         push_stdes(ST2,BST);
         move_stdes(BST,ST2);
         move(MST,ST2);
         break;
       case AR0(E_DISON) : {
         int ard;

           ard = n = arity(pop(MST));
           push(ST2,DISON);
	/* neu 25.4.90: 4. parameter	*/
           while (n--) {
             to_red();
           }
           push(ST2,AR0(AP_NORM) | toarity(ard+1));
         }
         break;
       case AR0(E_ERROR) :
         ppop(MST);	/* error entfernen	*/
         to_red();
         break;
#ifdef AltV
       case AR0(E_LET) :
         ppop(MST);	/* LET entfernen	*/
         transport(ST1,HST);	/* variable	*/
         to_red();	/* 1. unterausdruck	*/
         transport(HST,ST1);
         transport(ST1,ST2);
         to_red();	/* 2. unterausdruck	*/
         push(ST2,SUB);
         push(ST2,AP_LET);
         break;
#else
       case AR0(E_LET) :
         push(BST,STD_END);
         for (n = arity(m) - 1; n > 0 ; n-- ) {
           pop(ST1);		/* ldec		*/
           transport(ST1,HST);	/* variable	*/
           to_red();		/* rechte seite	*/
           push_stdes(ST2,BST);
         }
         move_stdes(BST,ST2);
         for (n = arity(topof(MST)) - 1; n > 0 ; n-- ) {
           transport(HST,ST1);	/* variablen zurueck	*/
         }
         for (n = arity(topof(MST)) - 1; n > 0 ; n-- ) {
           transport(ST1,ST2);
         }
         to_red();	/* "goal"	*/
         push(ST2,AR0(SUB) | toarity(arity(topof(MST))));
         push(ST2,AR0(AP_LET) | toarity(arity(pop(MST))));	/* LET entfernen	*/
         break;
       case AR0(LETPAR) :
         push(BST,STD_END);
         for (n = arity(m) - 1; n > 0 ; n-- ) {
           pop(ST1);		/* ldec		*/
           transport(ST1,HST);	/* variable	*/
           to_red();		/* rechte seite	*/
           push_stdes(ST2,BST);
         }
         move_stdes(BST,ST2);
         for (n = arity(topof(MST)) - 1; n > 0 ; n-- ) {
           transport(HST,ST1);	/* variablen zurueck	*/
         }
         for (n = arity(topof(MST)) - 1; n > 0 ; n-- ) {
           transport(ST1,ST2);
         }
         to_red();	/* "goal"	*/
         push(ST2,AR0(SUB) | toarity(arity(topof(MST))));
         push(ST2,AR0(AP_LETPAR) | toarity(arity(pop(MST))));	/* LET entfernen	*/
         break;
#endif
#if LET_P
       case AR0(E_LETP) :
         push(BST,STD_END);
         for (n = arity(m) - 1; n > 0 ; n-- ) {
           pop(ST1);		/* pdec		*/
           transport(ST1,HST);	/* pattern	*/
           to_red();		/* rechte seite	*/
           push_stdes(ST2,BST);
         }
         move_stdes(BST,ST2);
         for (n = arity(topof(MST)) - 1; n > 0 ; n-- ) {
           transport(HST,ST1);	/* pattern zurueck	*/
         }
         for (n = arity(topof(MST)) - 1; n > 0 ; n-- ) {
           to_red();
           /* transport(ST1,ST2);	*/
         }
         push(ST2,SA_TRUE);
         to_red();	/* "goal"	*/
         push(ST2,AR0(PM_WHEN) | toarity(arity(topof(MST)) + 1));
         push(ST2,AR0(AP_LETP) | toarity(arity(pop(MST))));	/* LETP entfernen	*/
         break;
#endif
       case AR0(APAL)   : { STACK X;
         pushi(STD_END,BST);
         to_red();	/* a_1 */
         asgnst(&X,ST2);
         to_red();	/* fnc */
         push_stdes(ST2,BST);	/* fnc */
         push_stdes(&X,BST);	/* a_1 */
         to_red();	/* a_2 */
         push_stdes(ST2,BST);	/* a_2 */
         ppop(MST);
         move_stdes(BST,ST2);
         pushi(AP_AL,ST2);
         break;
       }
#if LETREC
       case AR0(DECL):
         {
           register int ar,j,ar_decl,new_defs=0,name_length=0;

            ar_decl =  arity(m)-1;
            for (n = ar_decl ; n > 0L ; --n) {
               if (noedit(topof(ST1)) == PLACEH) {
                 if (ergto >= SUCCESS) ergto = EXPLACE;
                  --ar_decl;
                 ppop(ST1);
                }
               else {   m = pop(ST1); /* FDEC || DECL_CR */
                        ar = arity(m);
                        if (noedit(topof(ST1)) == PLACEH && ergto >= SUCCESS)
                          ergto = EXPLACE;
#if ClausR
                switch(m & CLASS_FIELD)
                { case AR0(DECL_CR):
                     new_defs += ar-1; /* hier werden 'neue' fkts-defs erzeugt */
                     /* local eindeutiger Modulname (&(n)) */
                     push(HST,tovalue((int)'&') | toedit6(DECL_CR) | LSTR0 );
                     push(HST,tovalue((int) n ) | toedit6(DECL_CR) | LSTR1 );
                     push(HST,tovalue((int) (ar-1) ) | toedit6(DECL_CR) | LSTR1 );
                     to_red();	/* modul */
                     for (j = ar-1 ; j > 0 ; j-- ) {
                       if (noedit(topof(ST1)) == PLACEH && ergto >= SUCCESS)
                         ergto = EXPLACE;
                       transp2(ST1,HST,ST2);	/* name der importierten fktn */

                       /* name fuer Selektion in String konvertieren */
                       name_length = 0;
                       while (marked1(topof(ST2))) {
                          push(ST1,markstr(noedit(pop(ST2)) | toedit(171)));
                          name_length++;
                       }
                       push(ST1,markstr(noedit(pop(ST2)) | toedit(171)));
                       push(ST1,AR0(KQUOTE) | toarity(name_length + 1) ); 
                       push(ST1,tovalue((int) (ar-1) ) | toedit6(DECL_CR) | LSTR0 );
                       push(ST1,tovalue((int) n ) | toedit6(DECL_CR) | LSTR1 );
                       push(ST1,tovalue((int)'&') | toedit6(DECL_CR) | LSTR1 );
                       push(ST1,FR_SELECT | toedit6(n_fcn) | DSPLD_FLAG );
                       push(ST1,CALL2 | toedit6(n_dcex) | DSPLD_FLAG );
                       to_red(); /* do whatever neccessary to CALL2 */
                     }
                     break;
                  default:
#endif /* ClausR */
                     transport(ST1,HST);	/* fktsname       */
                     if (noedit(topof(ST1)) == NOPAR) {
                       ppop(ST1);
                       ar = 1;
                      }
                     else {
                       for (j = ar-2 ; j > 0 ; j-- ) {
                         if (noedit(topof(ST1)) == PLACEH && ergto >= SUCCESS)
                           ergto = EXPLACE;
                         transport(ST1,ST2);	/* parameter      */
                       }
                     }
                     to_red();	/* rumpf */
                     if (ar > 1) push(ST2,AR0(SUB) | toarity(ar-1));
#if ClausR 
                }
#endif /* ClausR */
               }
            }
            pop(MST);	/* pop DECL !     */
#if ClausR 
            m = AR0(LIST) | toarity(ar_decl + new_defs); /* pop DECL !     */
#else
            m = AR0(LIST) | toarity(ar_decl);	/* pop DECL !     */
#endif /* ClausR */
            push(ST2,m);	/* listen-konstruktor mit anzahl funktionen	*/
            push(BST,STD_END);
            push_stdes(ST2,BST);
            push(HST,m);	/* listen-konstruktor mit anzahl funktionen	*/
            transport(HST,ST1); transport(ST1,ST2);
            if (ergto >= SUCCESS &&  !doub_def && !chkdopp(ST2,ST1))
              ergto = EXDOPP;
            move_stdes(BST,ST2);
            to_red();	/* goal  */
            push(ST2,AR0(LREC) | toarity(3));
            break;
          }
#endif	/* LETREC */
#if ClausR
       case AR0(SUB_CR)   :
         if ((n = arity(m)) == 2) {
           if (noedit(topof(ST1)) == NOPAR) {
             pop(ST1);	/* NOPAR	 */
             decrarity(MST);
             --n;
            }
         }
         for ( ; n > 0L ; --n) {
           to_red();
         }
         pushi(pop(MST) & ~EDIT_FIELD,ST2);
         break;
#endif	/* ClausR */

#if N_U_STR
/* H */case AR0(US_KON):
         n = arity(m);
         if (value(topof(ST1)) != STRBEGIN
              &&  value(topof(ST1)) != EXTRENN) {	/* noch nicht pre-proc. ? */
           int count;

           count = 1;
           push(ST2,markstr(US_MARK0));
           do {
             ++count;
             el1 = pop(ST1);
             push(ST2,markstr(el1));
           } while (marked1(el1));
           push(ST2,KQUOTE | toarity(count));
           n--;
         }
         for ( ; n > 0L ; --n) {
           if (noedit(topof(ST1)) == NOPAR) {
             ppop(ST1);
             decrarity(MST);
            }
           else to_red();
         }
         push(ST2,AR0(US_LIST) | (pop(MST) & VALUE_FIELD));
         break;
#else
/* H */case AR0(US_KON):
         n = arity(m);
         if (value(topof(ST1)) != STRBEGIN
              &&  value(topof(ST1)) != EXTRENN) {	/* noch nicht pre-proc. ? */
           push(ST2,AQUOTE); push(ST2,US_MARK);
           do {
             el1 = pop(ST1);
             push(ST2,mark1(el1));
           } while (marked1(el1));
           push(ST2,EQUOTE);
           n--;
         }
         for ( ; n > 0L ; --n) {
           if (noedit(topof(ST1)) == NOPAR) {
             ppop(ST1);
             decrarity(MST);
            }
           else to_red();
         }
         push(ST2,AR0(US_LIST) | (pop(MST) & VALUE_FIELD));
         break;
#endif

#if N_STR
/* H */case AR0(EPM_REC):
/* H */case AR0(EPM_EREC):
/* H */case AR0(EPM_FREC):
/* H */case AR0(EPM_NREC):
        {
         int ar_when,i;

         n = arity(m) - 1;	/* anzahl when's incl. ev. otherwise         */
         to_red();	/* name (rec)	*/
#if M_PATT
         if (isconstr(second(ST1)) && class(second(ST1)) == class(NPATT))
           ar_when = arity(second(ST1));
         else ar_when = 0;
#else
         if (isconstr(second(ST1)) && class(second(ST1)) == class(NPATT) && ergto >= SUCCESS)
           ergto = NO_M_PATT;
         ar_when = 0;
#endif	/* M_PATT	*/
         for ( ; n > 0L ; --n) {
           if (ar_when && isconstr(second(ST1)) && class(second(ST1)) == class(NPATT)) {
             if (n > 1L && ar_when != arity(second(ST1)) && ergto >= SUCCESS) {
#if TRACE
 if (trace > 0) {
   fprintf(stderr,"ar_when %d new arity %d",ar_when,arity(second(ST1)));
   waitkey();
 }
#endif
               ergto = WR_WHEN;
             }
             m = pop(ST1);	/* WHEN  */
             i = arity(m) + arity(topof(ST1)) - 1;
             push(MST,AR0(PM_WHEN) | toarity(i));
             ppop(ST1);	/* NPATT */
             for ( ; i > 0 ; i--) {
               to_red();
             }
             move(MST,ST2);
            }
           else {
             if (ar_when) {
               if (isconstr(topof(ST1)) && class(topof(ST1)) == class(EPM_WHEN))
                 ergto = WR_WHEN;
              }
             else {
               if (isconstr(topof(ST1)) && class(topof(ST1)) == class(EPM_WHEN)  &&
                   isconstr(second(ST1)) && class(second(ST1)) == class(NPATT))
                 ergto = WR_WHEN;
             }
             to_red();
           }
         }
         m = noedit(pop(MST));
         switch(m & CLASS_FIELD) {
           case AR0(EPM_REC) : 
             push(ST2,AR0(PM_REC) | (m & VALUE_FIELD));
             break;
           case AR0(EPM_EREC) : 
             push(ST2,AR0(PM_EREC) | (m & VALUE_FIELD));
             break;
           case AR0(EPM_FREC) : 
             push(ST2,AR0(PM_FREC) | (m & VALUE_FIELD));
             break;
           case AR0(EPM_NREC) : 
             push(ST2,AR0(PM_NREC) | (m & VALUE_FIELD));
             break;
           default : if (ergto >= SUCCESS) ergto = FAILURE;
             break;
         }
        }
         break;
/* H */case AR0(EPM_SWITCH):
/* H */case AR0(EPM_FCASE):
/* H */case AR0(EPM_ECASE):
        {
         int ar_when,i;

         n = arity(m);	/* anzahl when's incl. ev. otherwise         */
#if M_PATT
         if (isconstr(second(ST1)) && class(second(ST1)) == class(NPATT))
           ar_when = arity(second(ST1));
         else ar_when = 0;
#else
         if (isconstr(second(ST1)) && class(second(ST1)) == class(NPATT) && ergto >= SUCCESS)
           ergto = NO_M_PATT;
         ar_when = 0;
#endif	/* M_PATT	*/
         for ( ; n > 0L ; --n) {
           if (ar_when && isconstr(second(ST1)) && class(second(ST1)) == class(NPATT)) {
             if (n > 1L && ar_when != arity(second(ST1)) && ergto >= SUCCESS) {
#if TRACE
 if (trace > 0) {
   fprintf(stderr,"ar_when %d new arity %d",ar_when,arity(second(ST1)));
   waitkey();
 }
#endif
               ergto = WR_WHEN;
             }
             m = pop(ST1);	/* WHEN  */
             i = arity(m) + arity(topof(ST1)) - 1;
             push(MST,AR0(PM_WHEN) | toarity(i));
             ppop(ST1);	/* NPATT */
             for ( ; i > 0 ; i--) {
               to_red();
             }
             move(MST,ST2);
            }
           else {
             if (ar_when) {
               if (isconstr(topof(ST1)) && class(topof(ST1)) == class(EPM_WHEN))
                 ergto = WR_WHEN;
              }
             else {
               if (isconstr(topof(ST1)) && class(topof(ST1)) == class(EPM_WHEN)  &&
                   isconstr(second(ST1)) && class(second(ST1)) == class(NPATT))
                 ergto = WR_WHEN;
             }
             to_red();
           }
         }
         m = noedit(pop(MST));
         switch(m & CLASS_FIELD) {
           case AR0(EPM_SWITCH) : 
             push(ST2,AR0(PM_SWITCH) | (m & VALUE_FIELD));
             break;
           case AR0(EPM_FCASE):
             push(ST2,AR0(PM_FCASE) | (m & VALUE_FIELD));
             break;
           case AR0(EPM_ECASE):
             push(ST2,AR0(PM_ECASE) | (m & VALUE_FIELD));
             break;
           default : if (ergto >= SUCCESS) ergto = FAILURE;
             break;
         }
        }
         break;
#else
/* H */case AR0(EPM_SWITCH):
         push(MST,AR0(PM_SWITCH) | (pop(MST) & VALUE_FIELD));
         goto tordef;
/* H */case AR0(EPM_REC):
         push(MST,AR0(PM_REC) | (pop(MST) & VALUE_FIELD));
         goto tordef;
/* H */case AR0(EPM_EREC):
         push(MST,AR0(PM_EREC) | (pop(MST) & VALUE_FIELD));
         goto tordef;
/* H */case AR0(EPM_FREC):
         push(MST,AR0(PM_FREC) | (pop(MST) & VALUE_FIELD));
         goto tordef;
/* H */case AR0(EPM_NREC):
         push(MST,AR0(PM_NREC) | (pop(MST) & VALUE_FIELD));
         goto tordef;
/* H */case AR0(EPM_FCASE):
         push(MST,AR0(PM_FCASE) | (pop(MST) & VALUE_FIELD));
         goto tordef;
/* H */case AR0(EPM_ECASE):
         push(MST,AR0(PM_ECASE) | (pop(MST) & VALUE_FIELD));
         goto tordef;
#endif
/* H */case AR0(EPM_LIST):
         push(MST,AR0(PM_LIST) | (pop(MST) & VALUE_FIELD));
         goto tordef;
/* H */case AR0(EPM_AS):
         push(MST,AR0(PM_AS) | (pop(MST) & VALUE_FIELD));
         goto tordef;
#if N_STR
/* H */case AR0(EPM_SAS):
         push(MST,AR0(PM_AS) | (pop(MST) & VALUE_FIELD));
          for (n = arity(m) ; n > 1 ; n-- ) {
             to_red();
          }
          push(ST1,tovalue(STR_VAR) | LSTR1);
          to_red();
          move(MST,ST2);
          break;
#endif
/* H */case AR0(EPM_WHEN):
        {
         int i;

#if TRACE
 if (trace > 2) dsplstack(ST1,"ST1 vor EPM_WHEN ",1,0);         
#endif
           if (isconstr(topof(ST1)) && class(topof(ST1)) == class(NPATT)) {
             ppop(MST);	/* EPM_WHEN	*/
             i = arity(m) + arity(topof(ST1)) - 1;
#if TRACE
 if (trace > 2) printf("neue arity = %d\n",i);  
#endif
             push(MST,AR0(PM_WHEN) | toarity(i));
             ppop(ST1);	/* NPATT */
             for ( ; i > 0 ; i--) {
               to_red();
             }
             move(MST,ST2);
            }
           else {
	/*  to_red();	*/
             push(MST,AR0(PM_WHEN) | (pop(MST) & VALUE_FIELD));
             goto tordef;
           }
#if TRACE
 if (trace > 2) getkey();
#endif
        }
        break;
/* H */case AR0(EPM_MATCH):
         push(MST,AR0(PM_MATCH) | (pop(MST) & VALUE_FIELD));
         goto tordef;
/* H */case AR0(EPM_OTHERWISE):
         push(MST,AR0(PM_OTHERWISE) | (pop(MST) & VALUE_FIELD));
       tordef :
          for (n = arity(m) ; n > 0 ; n-- ) {
             to_red();
          }
          move(MST,ST2);
          break;
       case AR0(IF) :
         to_red();	/* pred */
         to_red();	/* cond */
         pushi(AP_IF,ST2);
         ppop(MST);
         break;
       case AR0(FILPRE):
         if (multi_atom(topof(ST1))) {
           register int i;

           ppop(MST);
           push(ST2,LSTR0 | tovalue(FILIPRE));
           if (travmastr(ST1,ST2,lbuf,LENFIL) < LENFIL) {
	/* dateiname nach ST2 und lbuf */
             delete(ST2);	/* dateiname loeschen */
             chkfilname(lbuf,FN_PRE);
             i = 0;
             while (c = editfil[FN_PRE][i++])
               push(ST2,LSTR1 | tovalue(c));
            }
           else {
           }
          }
         else {	/* kein multi_atom auf ST1:falsch   */
           to_red();
           move(MST,ST2);	/* FILPRE nach ST2 ? */
         }
         break;
       case AR0(FILNAM) :
         if (multi_atom(topof(ST1))) {
           if (travmastr(ST1,ST2,lbuf,LENFIL) < LENFIL) {
	/* dateiname nach ST2 und lbuf */
             if (RD_OK(loadexpr(U1ST,lbuf,1,FN_ED)) &&	/* load ok ?      */
                 isexpr(etptb[edit6(topof(U1ST))].exptp)) {
               transport(U1ST,ST1);	/* ja: ausdruck nach ST1   */
               delete(ST2);	/* dateiname loeschen      */
               to_red();	/* eingelesenen ausdruck uebersetzen*/
               ppop(MST);	/* FILNAM loeschen         */
              }
             else {	/* load nicht ok                    */
               delete(U1ST);	/* fehlerhaften ausdruck loeschen ? */
               transport(ST2,ST1);	/* dateiname zurueck                */
               push(ST1,LSTR1 | tovalue(FIL_DOLLAR));
               transport(ST1,ST2);	/* dateiname mit $ nach ST2         */
               ppop(MST);	/* FILNAM loeschen         */
               if (ergto >= SUCCESS) {
                 ergto = UND_FIL;
                 strcpy(errfn,lbuf);	/* remember error file name	*/
               }
             }
            }
           else {
           }
          }
         else {	/* kein multi_atom auf ST1:falsch   */
           if (ergto >= SUCCESS) ergto = FAILURE;
           to_red();
           move(MST,ST2);	/* FILNAM nach ST2                  */
         }
         break;
       case AR0(PROTECT):
         for (n = 0; isconstr(m = topof(ST1)); n++ )
           ppop(ST1);
         if (noedit(m) == PLACEH && ergto >= SUCCESS)
           ergto = EXPLACE;
         transport(ST1,ST2);	/* variable	*/
         for ( ; n > 0 ; n-- )
           push(ST2,PROTECT);
         move(MST,ST2);
         break;
	/* daten  */
       case AR0(NUMBFIELD) : PFIELD(NUMBER);
       case AR0(BOOLFIELD) : PFIELD(BOOL);
       case AR0(STRFIELD)  : PFIELD(STRING);
       case AR0(FIELDFIELD): PFIELD(TFIELD);
       case AR0(NVECT) : PVECT(NUMBER);
       case AR0(BVECT) : PVECT(BOOL);
       case AR0(SVECT) : PVECT(STRING);
       case AR0(NTVECT) : PTVECT(NUMBER);
       case AR0(BTVECT) : PTVECT(BOOL);
       case AR0(STVECT) : PTVECT(STRING);
#if N_STR
       case AR0(EPM_VAR):
          ppop(MST);
          push(ST1,tovalue(STR_VAR) | LSTR1);
          to_red();
          break;
#if N_U_STR
       case AR0(KQUOTE) :
          for (n = arity(m) ; n > 0 ; n-- ) {
            if (!isconstr(topof(ST1)) && multi_atom(topof(ST1))) {
              push(ST2,markstr(pop(ST1)));
             }
            else to_red();
          }
          push(ST2,noedit(pop(MST)));
          break;
#else
       case AR0(KQUOTE) :
          pop(MST);
          push(ST2,AQUOTE);
          for (n = arity(m) ; n > 0 ; n-- ) {
             to_red();
          }
          push(ST2,EQUOTE);
          break;
#endif
#endif
       default          :
                           { for (n = arity(m) ; n > 0L ; --n)
                                to_red();
                             pushi(pop(MST) & ~EDIT_FIELD,ST2);
                           }
     }	/* end switch */
  }
 else {	/* atom */
    if (single_atom(m)) {	/* single atom   */
      if (noedit(m) == PM_SSKSKIP)
        { ppop(ST1); push(ST2,PM_SKSKIP); }    else
      if (noedit(m) == PM_SSKPLUS)
        { ppop(ST1); push(ST2,PM_SKPLUS); }    else
      if (noedit(m) == PM_SSKSTAR)
        { ppop(ST1); push(ST2,PM_SKSTAR); }    else
      if (noedit(m) == PM_SSKIP)
        { ppop(ST1); push(ST2,PM_SKIP); }      else
      if (noedit(m) == PM_SDOLLAR)
        { ppop(ST1); push(ST2,PM_DOLLAR); }    else
      if (noedit(m) == DQUOTE)
        { ppop(ST1); push(ST2,markstr(tovalue((int)'"') | LSTR0)); }	else
      if (noedit(m) == DHASH)
        { ppop(ST1); push(ST2,markstr(tovalue((int)'#') | LSTR0)); }	else
      if (noedit(m) == DNOPAR)
        { ppop(ST1); push(ST2,markstr(tovalue((int)'@') | LSTR0)); }	else
      if (noedit(m) == DPERC)
        { ppop(ST1); push(ST2,markstr(tovalue((int)'%') | LSTR0)); }	else
      if (noedit(m) == DSINGLE)
        { ppop(ST1); push(ST2,markstr(tovalue((int)'\'') | LSTR0)); }	else
      if (noedit(m) == DBACK)
        { ppop(ST1); push(ST2,markstr(tovalue((int)'`') | LSTR0)); }	else
      if (noedit(m) == DBSLASH)
        { ppop(ST1); push(ST2,markstr(tovalue((int)'\\') | LSTR0)); }	else
      {
        if (noedit(m) == PLACEH && ergto >= SUCCESS) ergto = EXPLACE;
        move(ST1,ST2);
      }
     }
    else {	/* multi_atom */
       if (letterbit(m)) {	/* letter_string */
          if (value(m) == STRBEGIN) {	/* char_string   */
             pushi(AQUOTE,ST2);
             ppop(ST1);
             do {
                m = move(ST1,ST2);
                switch(value(m)) {
                   case STRBEGIN : ppop(ST2);pushi(AQUOTE,ST2);break;
                   case STREND   : ppop(ST2);pushi(EQUOTE,ST2);break;
                   default       : ;
                }
             } while (marked1(m));
           }
          else transport(ST1,ST2);	/* variable      */
        }
       else {	/* digitbit */	/* digit_string  */
          int point;
          point = NO;

          if ((char)value(m) == NEGSIGN) {
             pushi(MINUS,MST);
             ppop(ST1);
             m = topof(ST1);
           }
          else pushi(PLUS,MST);

          if (marked0(m))
             move(ST1,ST2);
          else {	/* marked1 */
             pushi(pop(ST1) & ~MARK_FLAG,ST2);	/* first        */
             do {	/* rest         */
                m = topof(ST1);
                c = (char)value(m);

                if ((point == NO) && (c == DECPOINT))	/* decimalpoint */
                 { pushi(pop(ST1) | MARK_FLAG,ST2);
                   point = YES;
                 }
                else
                 { if (c == DECEXP)	/* exponent     */
                    { ppop(ST1);
                      if (point == NO)
                       { pushi(DSTR1 | tovalue(DECPOINT),ST2);
                         point = YES;
                       }
                      move(MST,ST2);
                      pushi(EXP,MST);
                      transport(ST2,U1ST);
                      to_red();
                      transport(U1ST,ST2);
                      break;
                    }
                   else pushi(pop(ST1) | MARK_FLAG,ST2);	/* digit    */
                 }
              } while (marked1(m));
           }	/* end marked1 */

          if (point == NO)
           { pushi(DSTR1 | tovalue(DECPOINT),ST2);
	/* unused laut optimizer:	point = YES;	*/
           }
          move(MST,ST2);
        }	/* end digitbit */
     }	/* end multi_atom */
  }	/* end atom */
}

/**************************************************************************/
/* from_red : Uebersetzt die interne Darstellung der Reduktionsmaschine   */
/*           in die interne Darstellung des Editors (von R nach B)        */
/* use: BST, U1ST                                                         */
/**************************************************************************/

static int isinstr;

static from_red()
{
#if TRACE
 extern int ftrace;
#endif
 STACKELEM callkon(),checkf(),checkia();
 static STACKELEM m;
 static int c;
 long n;

 m = topof(ST1);
/*
 m = noedit(topof(ST1));
*/
 if (isconstr(m)) {
    if (class(m) == class(SYSAP)) {
      m &= ~LOWBIT;
      push(MST,m);
      ppop(ST1);
     }
    else move(ST1,MST);
#if TRACE
if (ftrace > 8) {
sprintf(msg,"ST1 mit kon %08lx und instr %d ",m,isinstr);
dsplstack(ST1,msg,1,0);
}
#endif
    switch(m & ~VALUE_FIELD) {
       case AR0(SYSAP) :
#if !LETREC
       case AR0(AP) :
         pushi(STD_END,BST);
         for (n = arity(m) ; n > 0L ; --n) {
           from_red();	/* arg_n..arg_1,func */
           push_stdes(ST2,BST);
         }
         move_stdes(BST,ST2);
         move(MST,ST2);
         break;
#else
       case AR0(AP_NORM) :
         if (arity(m) == arity(E_DISON)+1 && topof(ST1) == DISON) {
           n = arity(pop(MST));	/* AP_NORM entfernen	*/
           ppop(ST1);	/* DISON entfernen	*/
           while (--n) {	/* 25.4.90: 4. parameter	*/
             from_red();
           }
           push(ST2,E_DISON);
           break;
         }
         if (arity(m) == 2 && topof(ST1) == R_HEAD) {
           ppop(ST1);
           push(ST2,M_HEAD);
           from_red();
	/* edit-feld verhindert umdrehen in ftransp	*/
           push(ST2,AR0(CALL1)| (pop(MST) & VALUE_FIELD) | toedit(1));
           break;
         }
         if (arity(m) == 2 && topof(ST1) == R_TAIL) {
           ppop(ST1);
           push(ST2,M_TAIL);
           from_red();
	/* edit-feld verhindert umdrehen in ftransp	*/
           push(ST2,AR0(CALL1)| (pop(MST) & VALUE_FIELD) | toedit(1));
           break;
         }
	/* im else-fall: hier weiter !	*/
       case AR0(AP)     :
         if (arity(m) == 2 && topof(ST1) == ONEOF) {
           ppop(MST);	/* AP entfernen	*/
           ppop(ST1);	/* ONEOF entfernen	*/
           from_red();
           push(ST2,E_ONEOF);
          }	else
         if (arity(m) == 2 && topof(ST1) == _SUCCESS) {
           ppop(MST);	/* AP entfernen	*/
           ppop(ST1);	/* _SUCCESS entfernen	*/
           from_red();
           push(ST2,E_SUCCESS);
          }	else
         if (arity(m) == 3 && topof(ST1) == MAP) {
           ppop(MST);	/* AP entfernen	*/
           ppop(ST1);	/* MAP entfernen	*/
           from_red();
           from_red();
           push(ST2,E_MAP);
          }	else
         if (arity(m) == 3 && topof(ST1) == MAP_) {
           ppop(MST);	/* AP entfernen	*/
           ppop(ST1);	/* MAP entfernen	*/
           from_red();
           from_red();
           push(ST2,E_MAP_);
          }	else
         if (arity(m) == arity(E_DISON)+1 && topof(ST1) == DISON) {
           n = arity(pop(MST));	/* AP_NORM entfernen	*/
           ppop(ST1);	/* DISON entfernen	*/
           while (--n) {	/* 25.4.90: 4. parameter	*/
             from_red();
           }
           push(ST2,E_DISON);
          }
         else {
           STACKELEM el,el2;

           el = m;
           pushi(STD_END,BST);
           for (n = arity(m) -1L ; n > 0L ; --n) {
             from_red();	/* arg_n,..,arg_1 */
             push_stdes(ST2,BST);
           }
           el2 = noedit(topof(ST1));	/* funktion auf ST1 !      */
	/* success ?        */
           if (el2 == _SUCCESS  &&  arity(el) == 2) {
             del_stdes(BST);
             ppop(BST);
             ppop(MST);
             ppop(ST1);
             push(ST2,E_SUCCESS);
            }
           else {
            from_red();	/* funktion       */
            push_stdes(ST2,BST);
	/* call ?        */
            if ((isletstr(el2) && value(el2) != FIL_DOLLAR && value(el2) != FILIPRE)
              ||  el2 == PROTECT) {
              move_stdes(BST,ST2);
              push(ST2,AR0(CALL) | (pop(MST) & VALUE_FIELD));
             }                                                            else
	/* cond ?        */
	/* mit edit wg. DOT !	*/
            if (el2 == COND  &&  arity(el) == 2) {
              del_stdes(BST);	/* cond  */
              del_stdes(BST);	/* pred  */
              ppop(BST);	/* STD_END       */
              push(ST2,AR0(IF) | (pop(MST) & VALUE_FIELD));
             }                                                            else
	/* primitve fkt ?         */
             if (arity(el) == 3  &&  isprim(el2,kwtb,kwtblen)) {
               mov1_stdes(BST,HST);	/* fkt   */
               mov1_stdes(BST,ST2);	/* arg1  */
               mov1_stdes(HST,BST);	/* fkt zurueck    */
               move_stdes(BST,ST2);	/* arg2  */
               push(ST2,AR0(APAL) | (pop(MST) & VALUE_FIELD));
             }                                                            else
	/* predefined function ?  */
             if (single_atom(el2) &&
               (el1 = checkf(el2,el)) != STBOTTOM) {
              move_stdes(BST,ST2);
              push(ST2,el1 | (pop(MST) & VALUE_FIELD));
             }                                                            else
	/* predefined interaction ?  */
             if (single_atom(el2) &&
               (el1 = checkia(el2,el)) != STBOTTOM) {
              move_stdes(BST,ST2);
              push(ST2,el1 | (pop(MST) & VALUE_FIELD));
             }
            else {	/* keine lbar oder var ?        */
              move_stdes(BST,ST2);
              push(ST2,pop(MST) & ~EDIT_FIELD);
            }
           }
         }
         break;
#endif	/* LETREC	*/
       case AR0(AP_PRED):	/* predefined functions	*/
           {
            STACKELEM el;

            el = m;
            pushi(STD_END,BST);
            for (n = arity(m) - 1L ; n > 0L ; --n) {
               from_red();	/* arg_n,..,arg_1 */
               push_stdes(ST2,BST);
            }
            if (single_atom(topof(ST1)) &&
               (el1 = checkf(topof(ST1),el)) != STBOTTOM) {
               pushi(el1 | (pop(MST) & VALUE_FIELD),MST);
             }
            else if (single_atom(topof(ST1)) &&
               (el1 = checkia(topof(ST1),el)) != STBOTTOM) {
               pushi(el1 | (pop(MST) & VALUE_FIELD),MST);
             }
            else {
              sprintf(msg,"from_red: Error in AP_PRED (%08lx,%08lx,%08lx)",
                      topof(ST1),el,el1);
              error(msg);
            }
            from_red();	/* func */
            push_stdes(ST2,BST);
            move_stdes(BST,ST2);
            move(MST,ST2);
            break;
          }
#ifdef UnBenutzt
       case AR0(AP_VAR) :
        {
         STACKELEM el2;

         pushi(STD_END,BST);
         for (n = arity(m) - 1L ; n > 0L ; --n) {
            from_red();	/* arg_n,..,arg_1 */
            push_stdes(ST2,BST);
         }
         el2 = noedit(topof(ST1));	/* funktion auf ST1 !      */
         if ((isletstr(el2) && value(el2) != FIL_DOLLAR && value(el2) != FILIPRE)
            ||  el2 == PROTECT) {
            pushi(AR0(CALL) | (pop(MST) & VALUE_FIELD),MST);
          }
         else {
           pushi(pop(MST) & ~EDIT_FIELD,MST);
         }
         from_red();	/* func */
         push_stdes(ST2,BST);
         move_stdes(BST,ST2);
         move(MST,ST2);
         break;
        }
#endif
       case AR0(AP_SUCC) :
        {
         STACKELEM el2;

         pushi(STD_END,BST);
         for (n = arity(m) - 1L ; n > 0L ; --n) {
            from_red();	/* arg_n,..,arg_1 */
            push_stdes(ST2,BST);
         }
         el2 = noedit(topof(ST1));	/* funktion auf ST1 !      */
         if (el2 == _SUCCESS) {
            del_stdes(BST);
            ppop(BST);
            ppop(MST);
            ppop(ST1);
            push(ST2,E_SUCCESS);
          }
         else {
           pushi(pop(MST) & ~EDIT_FIELD,MST);
           from_red();	/* func */
           push_stdes(ST2,BST);
           move_stdes(BST,ST2);
           move(MST,ST2);
         }
         break;
        }
       case AR0(AP_CALL):
        {
         STACKELEM el,el2;

         pushi(STD_END,BST);
         el = m;
         for (n = arity(m) - 1L ; n > 0L ; --n) {
           from_red();	/* arg_n,..,arg_1 */
           push_stdes(ST2,BST);
         }
         el2 = noedit(topof(ST1));	/* funktion auf ST1 !      */
         from_red();	/* func */
         push_stdes(ST2,BST);
         if ((isletstr(el2) && value(el2) != FIL_DOLLAR && value(el2) != FILIPRE)
            ||  el2 == PROTECT) {
           move_stdes(BST,ST2);
           push(ST2,AR0(CALL) | (pop(MST) & VALUE_FIELD));
          }	else
	/* primitve fkt ?         */
         if (arity(el) == 3  &&  isprim(el2,kwtb,kwtblen)) {
            mov1_stdes(BST,HST);	/* fkt   */
            mov1_stdes(BST,ST2);	/* arg1  */
            mov1_stdes(HST,BST);	/* fkt zurueck    */
            move_stdes(BST,ST2);	/* arg2  */
            push(ST2,AR0(APAL) | (pop(MST) & VALUE_FIELD));
          }	else
	/* cond ?        */
         if (el2 == COND  &&  arity(el) == 2) {
            del_stdes(BST);	/* cond  */
            del_stdes(BST);	/* pred  */
            ppop(BST);	/* STD_END       */
            push(ST2,AR0(IF) | (pop(MST) & VALUE_FIELD));
          }                                                            else
	/* predefined function ?  */
         if (single_atom(el2) &&
            (el1 = checkf(el2,el)) != STBOTTOM) {
           move_stdes(BST,ST2);
           push(ST2,el1 | (pop(MST) & VALUE_FIELD));
          }
        /* predefined function ?  */
         else if (single_atom(el2) &&
                  (el1 = checkia(el2,el)) != STBOTTOM) {
           move_stdes(BST,ST2);
           push(ST2,el1 | (pop(MST) & VALUE_FIELD));
          }
         else {	/* keine lbar oder var ?        */
           move_stdes(BST,ST2);
           push(ST2,pop(MST) & ~EDIT_FIELD);
         }
         break;
        }
       case AR0(AP_AL)  :
         if (arity(m) != 3L) {
            pushi(AR0(AP) | (pop(MST) & VALUE_FIELD),ST1);
            from_red();
            return;
          }
         else {
            STACK X;
            pushi(STD_END,BST);
            from_red();	/* argument 2 */
            push_stdes(ST2,BST);
            from_red();	/* function   */
            asgnst(&X,ST2);
            from_red();	/* argument 1 */
            push_stdes(ST2,BST);	/* argument 1 */
            push_stdes(&X,BST);	/* function   */
            ppop(MST);
            move_stdes(BST,ST2);
            pushi(APAL,ST2);
          }
         break;
#ifdef AltV
       case AR0(AP_LET) :
         if (noedit(topof(ST1)) == PLACEH && ergto >= SUCCESS)
           ergto = EXPLACE;
         from_red();	/* 1. unterausdruck	*/
         if (isconstr(topof(ST1)) && class(topof(ST1)) == AR0(SUB)
             && arity(topof(ST1)) == 2) {
           ppop(ST1);	/* SUB entfernen	*/
           ppop(MST);	/* A_LET entfernen	*/
           transport(ST1,ST2);	/* variable	*/
           from_red();	/* 2. unterausdruck	*/
           push(ST2,E_LET);
          }
         else {
           pushi(STD_END,BST);
           push_stdes(ST2,BST);
           for (n = arity(m) ; n > 1L ; --n) {
             from_red();	/* arg_n..arg_1,func */
             push_stdes(ST2,BST);
           }
           move_stdes(BST,ST2);
           push(ST2,(pop(MST) & VALUE_FIELD) | AR0(AP));
         }
         break;
#else
       case AR0(AP_LET) :
         push(BST,STD_END);
         /* retranslate arguments of AP	*/
         for (n = arity(m) ; n > 1L ; --n) {
           if (noedit(topof(ST1)) == PLACEH && ergto >= SUCCESS)
             ergto = EXPLACE;
           from_red();	/* arg_n..arg_1	*/
           push_stdes(ST2,BST);
         }
         /* now we should have a SUB on top	*/
         if (isconstr(topof(ST1)) && class(topof(ST1)) == AR0(SUB)
             && arity(topof(ST1)) == arity(topof(MST))) {
           move_stdes(BST,ST2);
           for (n = arity(pop(ST1)) ; n > 1L ; --n) {	/* SUB	*/
             transport(ST1,ST2);			/* variablen	*/
           }
           from_red();					/* "goal"	*/
           push(ST2,AR0(E_LET) | (pop(MST) & VALUE_FIELD));	/* pop AP	*/
          }
         else {
           from_red();	/* func	*/
           push_stdes(ST2,BST);
           move_stdes(BST,ST2);
           push(ST2,(pop(MST) & VALUE_FIELD) | AR0(AP));
         }
         break;
       case AR0(AP_LETPAR) :
         push(BST,STD_END);
         for (n = arity(m) ; n > 1L ; --n) {
           if (noedit(topof(ST1)) == PLACEH && ergto >= SUCCESS)
             ergto = EXPLACE;
           from_red();	/* arg_n..arg_1	*/
           push_stdes(ST2,BST);
         }
         if (isconstr(topof(ST1)) && class(topof(ST1)) == AR0(SUB)
             && arity(topof(ST1)) == arity(topof(MST))) {
           move_stdes(BST,ST2);
           for (n = arity(pop(ST1)) ; n > 1L ; --n) {	/* SUB	*/
             transport(ST1,ST2);			/* variablen	*/
           }
           from_red();					/* "goal"	*/
           push(ST2,AR0(LETPAR) | (pop(MST) & VALUE_FIELD));	/* pop AP	*/
          }
         else {
           from_red();	/* func	*/
           push_stdes(ST2,BST);
           move_stdes(BST,ST2);
           push(ST2,(pop(MST) & VALUE_FIELD) | AR0(AP));
         }
         break;
#endif
#if LET_P
       case AR0(AP_LETP) :
         push(BST,STD_END);
         for (n = arity(m) ; n > 1L ; --n) {
           from_red();	/* arg_n..arg_1	*/
           push_stdes(ST2,BST);
         }
         /* --- dg 18.05.92 letp in pi-red+ */
         if (isconstr(topof(ST1)) && class(topof(ST1)) == AR0(PM_SWITCH) && arity(topof(ST1)) == 2) {
           pop(ST1);                    /* PM_SWITCH */
           transport(ST1,HST);          /* WHEN expr */
           pop(ST1);			/* PM_END */
           transport(HST,ST1);          /* WHEN expr back */
         }
         /* --- dg 18.05.92 letp in pi-red+ */
         if (isconstr(topof(ST1)) && class(topof(ST1)) == AR0(PM_WHEN)
             && arity(topof(ST1))-1 == arity(topof(MST))) {
           move_stdes(BST,ST2);
           for (n = arity(pop(ST1)) ; n > 2L ; --n) {	/* WHEN	*/
             from_red();			/* pattern	*/
           }
           if (noedit(m = pop(ST1)) != SA_TRUE) {	/* guard true	*/
             sprintf(msg,"from_red: Error in LETP missing TRUE (%08lx)",M);
             error(msg);
           }
           from_red();					/* "goal"	*/
           push(ST2,AR0(E_LETP) | (pop(MST) & VALUE_FIELD));	/* pop AP	*/
          }
         else {
           from_red();	/* func	*/
           push_stdes(ST2,BST);
           move_stdes(BST,ST2);
           push(ST2,(pop(MST) & VALUE_FIELD) | AR0(AP));
         }
         break;
#endif
#if LETREC
       case AR0(LREC) :
         {
           register int ar,j,i;

           transport(ST1,ST2); transport(ST2,HST);
           ppop(HST);
           n = arity(pop(ST1));	/* LIST  */
           for ( i = n ; i > 0 ; --i) {
#if ClausR
             if (value(topof(HST)) == '&')      /* DECL_CR ? */
             { /* ja, zurueckwandeln */
               /* Aufbau des LRECs darf sich nicht geaendert haben !! */
               ppop(HST);            /* '&' */
               ppop(HST);            /* local eindeutige Modul-Nummer */
               ar = value(pop(HST)); /* Anzahl der importierten Funktionen */
               from_red();	     /* Modul-Ausdruck */
               for (j = ar ; j > 0 ; j-- ) {
                  transport(HST,ST2);           /* die importierten Namen */
                  delete(ST1);                  /* fselect( &nm , 'Name' ) */
                  n--; i--;                     /* weniger definitionen im */
                                                /* def! */
               }
               push(ST2,AR0(DECL_CR) | toarity(ar+1));
             }
             else
             {
               transport(HST,ST2);	/* fktsname       */
               if (class(topof(ST1)) == class(SUB)) {
                 ar = arity(pop(ST1));
                 for (j = ar - 1 ; j > 0 ; j-- ) {
                    transport(ST1,ST2);
                 }
                }
               else {
                 ar = 2;
                 push(ST2,NOPAR);
               }
               from_red();	/* rumpf */
               push(ST2,AR0(FDEC) | toarity(ar+1));
             }
#else	/* ClausR */
             transport(HST,ST2);	/* fktsname       */
             if (class(topof(ST1)) == class(SUB)) {
               ar = arity(pop(ST1));
               for (j = ar - 1 ; j > 0 ; j-- ) {
                  transport(ST1,ST2);
               }
              }
             else {
               ar = 2;
               push(ST2,NOPAR);
             }
             from_red();	/* rumpf */
             push(ST2,AR0(FDEC) | toarity(ar+1));
#endif	/* ClausR */
           }
           from_red();	/* goal           */
           push(ST2,DECL | toarity(n+1));
           ppop(MST);	/* LREC           */
           break;
         }
#endif
#if ClausR
       case AR0(SUB_CR)   :
         if ((n = arity(m)) == 1) {
           push(ST2,NOPAR);
           incrarity(MST);
           ++n;
         }
         for ( ; n > 0L ; --n) {
           from_red();
         }
         pushi(pop(MST) & ~EDIT_FIELD,ST2);
         break;
#endif	/* ClausR */
/* H */case AR0(PM_LIST):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_LIST) | (pop(MST) & VALUE_FIELD));
         break;
/* H */case AR0(PM_AS):
#if TRACE
if (ftrace > 6) {
 sprintf(msg,"ST1 bei PM_AS mit isinstr %d",isinstr);
 dsplstack(ST1,msg,1,0);
}
#endif
          if (isinstr) push(MST,AR0(EPM_SAS) | (pop(MST) & VALUE_FIELD));
          else push(MST,AR0(EPM_AS) | (pop(MST) & VALUE_FIELD));
          from_red();
          if (value(topof(ST1)) == STR_VAR) ppop(ST1);
          from_red();
          move(MST,ST2);
          break;
#if N_STR
/* H */case AR0(PM_SWITCH) : 
/* H */case AR0(PM_REC) : 
/* H */case AR0(PM_EREC) : 
/* H */case AR0(PM_FREC) : 
/* H */case AR0(PM_NREC) : 
/* H */case AR0(PM_FCASE):
/* H */case AR0(PM_ECASE):
        {
         int ar_when,i;

         n = arity(m);	/* anzahl when's incl. ev. otherwise         */
#if TRACE
if (ftrace > 6) dsplstack(ST1,"ST1 in PM_SWITCH ",1,0);
#endif
         if (isconstr(topof(ST1)) && class(topof(ST1)) == class(PM_WHEN)
             && arity(topof(ST1)) > 3)
           ar_when = arity(topof(ST1));
         else ar_when = 0;
         for ( ; n > 0L ; --n) {
           if (ar_when && isconstr(topof(ST1))  &&  class(topof(ST1)) != class(PM_OTHERWISE)) {
#if TRACE
if (ftrace > 6) dsplstack(ST1,"ST1 vor PM_WHEN ",1,0);
#endif
	/* was dann ? */
             if (n > 1L && ar_when != arity(topof(ST1)) && ergto >= SUCCESS) ergto = FAILURE;
             m = pop(ST1);	/* WHEN  */
             i = arity(m);
             push(MST,AR0(EPM_WHEN) | toarity(3));
             push(MST,AR0(NPATT) | toarity(i-2));
             for ( ; i > 0 ; i--) {
               from_red();
               if (i == 3) move(MST,ST2);	/* NPATT */
             }
             move(MST,ST2);
            }
           else {
             from_red();
           }
         }
         m = noedit(pop(MST));
         switch(m & CLASS_FIELD) {
           case AR0(PM_SWITCH) : 
             push(ST2,AR0(EPM_SWITCH) | (m & VALUE_FIELD));
             break;
           case AR0(PM_REC) : 
             push(ST2,AR0(EPM_REC) | (m & VALUE_FIELD));
             break;
           case AR0(PM_EREC) : 
             push(ST2,AR0(EPM_EREC) | (m & VALUE_FIELD));
             break;
           case AR0(PM_FREC) : 
             push(ST2,AR0(EPM_FREC) | (m & VALUE_FIELD));
             break;
           case AR0(PM_NREC) : 
             push(ST2,AR0(EPM_NREC) | (m & VALUE_FIELD));
             break;
           case AR0(PM_FCASE):
             push(ST2,AR0(EPM_FCASE) | (m & VALUE_FIELD));
             break;
           case AR0(PM_ECASE):
             push(ST2,AR0(EPM_ECASE) | (m & VALUE_FIELD));
             break;
           default : if (ergto >= SUCCESS) ergto = FAILURE;
             break;
         }
        }
         break;
#else
/* H */case AR0(PM_SWITCH):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_SWITCH) | (pop(MST) & VALUE_FIELD));
         break;
/* H */case AR0(PM_FCASE):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_FCASE) | (pop(MST) & VALUE_FIELD));
         break;
/* H */case AR0(PM_ECASE):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_ECASE) | (pop(MST) & VALUE_FIELD));
         break;
/* H */case AR0(PM_REC):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_REC) | (pop(MST) & VALUE_FIELD));
         break;
/* H */case AR0(PM_EREC):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_EREC) | (pop(MST) & VALUE_FIELD));
         break;
/* H */case AR0(PM_FREC):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_FREC) | (pop(MST) & VALUE_FIELD));
         break;
/* H */case AR0(PM_NREC):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_NREC) | (pop(MST) & VALUE_FIELD));
         break;
#endif
/* H */case AR0(PM_WHEN):
        {
         int i;

         if (arity(m) > 3) {
           ppop(MST);	/* PM_WHEN	*/
#if TRACE
if (ftrace > 6) dsplstack(ST1,"ST1 vor PM_WHEN ",1,0);
#endif
	/* was dann ? */
           i = arity(m);
           push(MST,AR0(EPM_WHEN) | toarity(3));
           push(MST,AR0(NPATT) | toarity(i-2));
           for ( ; i > 0 ; i--) {
             from_red();
             if (i == 3) move(MST,ST2);	/* NPATT */
           }
           move(MST,ST2);
          }
         else {
           for (n = arity(m) ; n > 0L ; --n)
               from_red();
           push(ST2,AR0(EPM_WHEN) | (pop(MST) & VALUE_FIELD));
         }
         break;
        }
/* H */case AR0(PM_MATCH):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_MATCH) | (pop(MST) & VALUE_FIELD));
         break;
/* H */case AR0(PM_OTHERWISE):
         for (n = arity(m) ; n > 0L ; --n)
             from_red();
         push(ST2,AR0(EPM_OTHERWISE) | (pop(MST) & VALUE_FIELD));
         break;
/* H */
       case AR0(LIST) :	/* ohne editinfo ?!        */
       case AR0(US_LIST) :
#if N_U_STR
         if (class(topof(ST1)) == class(KQUOTE)  &&  second(ST1) == markstr(US_MARK0)) {
           if (BACK) {
             n = arity(m);
            }
           else {
             n = arity(pop(ST1));	/* KQUOTE         */
             ppop(ST1);	/* US_MARK0       */
             push(ST2,mark0(pop(ST1)));	/* ende markieren */
             while (--n > 1) {
               push(ST2,mark1(pop(ST1)));
             }
             n = arity(m) - 1L;
           }
           if (n) {
             for ( ; n > 0L ; --n)
               from_red();
            }
           else {
             push(ST2,NOPAR);
             incrarity(MST);
           }
           pushi(AR0(US_KON) | (pop(MST) & VALUE_FIELD),ST2);
          }               else
         if (value(topof(ST1)) == EXTRENN) {	/* ??    */
           n = arity(m);
           if (n) {
             for ( ; n > 0L ; --n)
               from_red();
            }
           else {
             push(ST2,NOPAR);
             incrarity(MST);
           }
           pushi(AR0(US_KON) | (pop(MST) & VALUE_FIELD),ST2);
          }
         else {	/* kein '_ auf ST1 !       */
           for ( n = arity(m) ; n > 0L ; --n )
             from_red();
           push(ST2,noedit(pop(MST)));
         }
         break;
#else
         if (topof(ST1) == AQUOTE  &&  second(ST1) == US_MARK) {
           if (BACK) {
             n = arity(m);
            }
           else {
             ppop(ST1);	/* AQUOTE         */
             ppop(ST1);	/* US_MARK        */
             push(ST2,mark0(pop(ST1)));	/* ende markieren */
             while ((el1 = pop(ST1)) != EQUOTE)
               push(ST2,el1);
             n = arity(m) - 1L;
           }
           if (n) {
             for ( ; n > 0L ; --n)
               from_red();
            }
           else {
             push(ST2,NOPAR);
             incrarity(MST);
           }
           pushi(AR0(US_KON) | (pop(MST) & VALUE_FIELD),ST2);
          }               else
         if (value(topof(ST1)) == EXTRENN) {
           n = arity(m);
           if (n) {
             for ( ; n > 0L ; --n)
               from_red();
            }
           else {
             push(ST2,NOPAR);
             incrarity(MST);
           }
           pushi(AR0(US_KON) | (pop(MST) & VALUE_FIELD),ST2);
          }
         else {	/* kein '_ auf ST1 !       */
           for ( n = arity(m) ; n > 0L ; --n )
             from_red();
           push(ST2,noedit(pop(MST)));
         }
         break;
#endif
       case AR0(AP_IF)  : ppop(MST);
         from_red();	/* predicate */
         from_red();	/* conditional */
         pushi(IF,ST2);
         break;
       case AR0(PLUS)   : ppop(MST);
         from_red();
         break;
       case AR0(MINUS)  : pushi(DSTR1 | tovalue(NEGSIGN),ST1);
         ppop(MST);
         from_red();
         break;
       case AR0(EXP)    : ppop(MST);
         transport(ST1,U1ST);
         from_red();
         transport(U1ST,ST1);
         pushi(DSTR1 | tovalue(DECEXP),ST2);
         if ((pop(ST1) & ~EDIT_FIELD) == MINUS)
            pushi(DSTR1 | tovalue(NEGSIGN),ST2);
         do
          { m = pop(ST1);
            pushi(m | MARK_FLAG,ST2);
          } while (marked1(m));
         if ((char)value(topof(ST2)) == DECPOINT)
            ppop(ST2);
         break;
       case AR0(PROTECT): transport(ST1,ST2);
         move(MST,ST2);
         break;
       case AR0(SCALAR) : ppop(MST);
         pop(ST1);
         push(MST,m);
         from_red();
         move(MST,ST2);
         break;
       case AR0(FIELD) :  decrarity(MST);
         m = pop(MST) & VALUE_FIELD;
         switch(pop(ST1) & ~EDIT_FIELD) {
            case NUMBER  : pushi(AR0(NUMBFIELD) | m,MST);break;
            case BOOL    : pushi(AR0(BOOLFIELD) | m,MST);break;
            case STRING  : pushi(AR0(STRFIELD) | m,MST);break;
            case TFIELD  : pushi(AR0(FIELDFIELD) | m,MST);break;
            default      : pushi(AR0(LIST) | m,MST);
          }
         for (n = arity(m); n > 0; n--)
             from_red();
         move(MST,ST2);
         break;
       case AR0(VECTOR) : pop(MST);
         m = second(ST1) & VALUE_FIELD;
         switch(pop(ST1) & ~EDIT_FIELD) {
            case NUMBER  : pushi(AR0(NVECT) | m,MST);break;
            case BOOL    : pushi(AR0(BVECT) | m,MST);break;
            case STRING  : pushi(AR0(SVECT) | m,MST);break;
            default      : pushi(AR0(LIST) | m,MST);
          }
         pop(ST1);
         for (n = arity(m); n > 0; n--)
             from_red();
         move(MST,ST2);
         break;
       case AR0(TVECTOR) :pop(MST);
         m = second(ST1) & VALUE_FIELD;
         switch(pop(ST1) & ~EDIT_FIELD) {
            case NUMBER  : pushi(AR0(NTVECT) | m,MST);break;
            case BOOL    : pushi(AR0(BTVECT) | m,MST);break;
            case STRING  : pushi(AR0(STVECT) | m,MST);break;
            default      : pushi(AR0(LIST) | m,MST);
          }
         pop(ST1);
         for (n = arity(m); n > 0; n--)
             from_red();
         move(MST,ST2);
         break;
#if N_STR
       case AR0(KQUOTE) :
          for (n = arity(m) ; n > 0 ; n-- ) {
            isinstr = TRUE;
#if TRACE
if (ftrace > 5)   dsplstack(ST1,"ST1 in KQUOTE",1,0);
#endif
            if (isconstr(topof(ST1))) {
              from_red();
             }
            else {
              if (is_r_str(topof(ST1))) {
                if (value(topof(ST1)) == (int)'"') {
                  push(ST2,DQUOTE);
                  ppop(ST1);
                 }	else
                if (value(topof(ST1)) == (int)'#') {
                  push(ST2,DHASH);
                  ppop(ST1);
                 }	else
                if (value(topof(ST1)) == (int)'@') {
                  push(ST2,DNOPAR);
                  ppop(ST1);
                 }	else
                if (value(topof(ST1)) == (int)'%') {
                  push(ST2,DPERC);
                  ppop(ST1);
                 }	else
                if (value(topof(ST1)) == (int)'\'') {
                  push(ST2,DSINGLE);
                  ppop(ST1);
                 }	else
                if (value(topof(ST1)) == (int)'`') {
                  push(ST2,DBACK);
                  ppop(ST1);
                 }	else
                if (value(topof(ST1)) == (int)'\\') {
                  push(ST2,DBSLASH);
                  ppop(ST1);
                 }	else
                push(ST2,clrstr(pop(ST1)));
               } else
              if (is_letter(topof(ST1))) {
                if (value(topof(ST1)) == STR_VAR) ppop(ST1);
                from_red();
                push(ST2,AR0(EPM_VAR) | toarity(1));
               }
              else from_red();
            }
          }
          isinstr = FALSE;
          move(MST,ST2);
          break;
#endif
       case AR0(R_DOT) :
         ppop(MST);
         from_red(); from_red();
         push(ST2,E_DOT);
         break;
       default          : for (n = arity(m) ; n > 0L ; --n)
                             from_red();
                          move(MST,ST2);
     }	/* end switch */
  }	/* end constructor */
 else {	/* atom */
   if (single_atom(m)) {
#if N_STR
      if (isinstr  &&  m == PM_SKSKIP)
        { ppop(ST1); push(ST2,PM_SSKSKIP); }          else
      if (isinstr  &&  m == PM_SKPLUS)
        { ppop(ST1); push(ST2,PM_SSKPLUS); }          else
      if (isinstr  &&  m == PM_SKSTAR)
        { ppop(ST1); push(ST2,PM_SSKSTAR); }          else
      if (isinstr  &&  m == PM_SKIP)
        { ppop(ST1); push(ST2,PM_SSKIP); }          else
      if (isinstr  &&  m == PM_DOLLAR)
        { ppop(ST1); push(ST2,PM_SDOLLAR); }          else
#endif
      if (m == AQUOTE) {
#if N_STR
#define MAXL 64
#define ISAQUOTE(m) (isletstr(m) && value(m) == STRBEGIN)
#define ISEQUOTE(m) (isletstr(m) && value(m) == STREND)
     int len[MAXL];

        n = 0;
        do {
          m = pop(ST1);
          if (m == AQUOTE || ISAQUOTE(m)) {
            ++len[n];
            ++n;
            len[n] = 0;
           }  else
          if (m == EQUOTE || ISEQUOTE(m)) {
            push(ST2,KQUOTE | toarity(len[n]));
            --n;
           }
          else {
            push(ST2,mark0(m));
            ++len[n];
          }
        } while (n);
#else
         n = 1L;
         ppop(ST1);
         pushi(LSTR0 | tovalue(STRBEGIN),ST2);
         do {
             switch(m = move(ST1,ST2)) {
                case AQUOTE : ++n;
                              ppop(ST2);
                              pushi(LSTR1 | tovalue(STRBEGIN),ST2);
                              break;
                case EQUOTE : --n;
                              ppop(ST2);
                              pushi(LSTR1 | tovalue(STREND),ST2);
                              break;
                default     : ;
              }
           } while (n != 0L);
#endif
        }
       else move(ST1,ST2);
     }
    else {	/* multi_atom */
       if (!isinstr && value(m) == FIL_DOLLAR) {
         ppop(ST1);
         transport(ST1,ST2);
         push(ST2,FILNAM);
        }        else
       if (!isinstr && value(m) == FIL_RED) {
         ppop(ST1);
         transport(ST1,ST2);
         push(ST2,FILRED);
        }        else
       if (!isinstr && value(m) == FILIPRE) {
         register int i;

         ppop(ST1);
         if (travmastr(ST1,ST2,editfil[FN_PRE],LENFIL) < LENFIL) {
	/* dateiname nach ST2 und editfil */
           delete(ST2);	/* dateiname loeschen */
           getbase(lbuf,FN_PRE);
#if TRACE
if (ftrace > 5) {
   fprintf(stdout,"name vor getbase %s nachher: %s\n",editfil[FN_PRE],lbuf);
   getkey();
}
#endif
           i = 0;
           c = lbuf[i++];
           push(ST2,LSTR0 | tovalue(c));
           while (c = lbuf[i++])
             push(ST2,LSTR1 | tovalue(c));
           push(ST2,FILPRE);
          }
         else {
         }
        }        else
       if (letterbit(m)) transport(ST1,ST2);
       else {	/* digitbit */
          if (marked0(m))
             move(ST1,ST2);
          else {
             c = (char)value(m);
             if (c == DECPOINT)
                pushi(DSTR0 | tovalue('0'),ST2);
             else if (c == NEGSIGN)
                   { pushi(pop(ST1) & ~MARK_FLAG,ST2);
                     if ((char)value(topof(ST1)) == DECPOINT)
                        pushi(DSTR1 | tovalue('0'),ST2);
                   }
                  else pushi(pop(ST1) & ~MARK_FLAG,ST2);
             do
              { m = pop(ST1);
                pushi(DSTR1 | m,ST2);
              } while (marked1(m));
             if ((char)value(topof(ST2)) == DECPOINT)
                ppop(ST2);
           }
        }	/* end digitbit */
     }	/* end multi_atom */
  }	/* end atom */
}

#ifdef UnBenutzt
/**************************************************************************/
/* mkdimvec : Erzeugt den Dimensionsvector von Matrizen                   */
/**************************************************************************/

static mkdimvec(st1,st2)
STACK *st1,*st2;
{
 char *longtoasc();
 STACKELEM popnf(),move(),topof();
 static STACKELEM m;
 STACK X;

 if ((topof(st1) & CLASS_FIELD) == AR0(LIST))
  { asgnst(&X,st1);
    pushi(AR0(LIST),MST);
    while (((m = popnf(&X)) & CLASS_FIELD) == AR0(LIST))
     { dstrtostk(longtoasc((long)arity(m)),st2);
       incrarity(MST);
     }
    move(MST,st2);
  }
 else	/* error */
    pushi(PLACEH,st2);
}
#endif	/* UnBenutzt       */

/***/

static ttransp(st1,st2)
STACK *st1,*st2;
{
#if TRACE
  extern int trace;
#endif
 static STACKELEM m;
 long n;

#if TRACE
if (trace > 6) dsplstack(st1,"st1 in ttransp",1,0);
#endif
 if (isconstr(m = topof(st1)))
  { movenf(st1,MST);
    switch(m & ~VALUE_FIELD) {
       case AR0(SYSAP)  :
       case AR0(AP)     :
       case AR0(AP_CALL):
       case AR0(AP_PRED):
#ifdef UnBenutzt
       case AR0(AP_FNC) :
       case AR0(AP_VAR) :
#endif
       case AR0(AP_SUCC) :
         { STACK X,Y;
           n = (long)arity(m)-1L;
           popnf_stdes(&X,st1);
           ttransp(&X,st2);
           for ( ; n > 0L ; --n)
            { popnf_stdes(&Y,st1);
              ttransp(&Y,st2);
            }
           asgnst(st1,&X);
           move(MST,st2);
           break;
         }
       case AR0(AP_LET) :
       case AR0(AP_LETPAR) :
         { STACK X,Y;

           n = (long)arity(m)-2L;
           ttransp(st1,st2);	/* SUB	*/
           popnf_stdes(&X,st1);
           ttransp(&X,st2);
           for ( ; n > 0L ; --n)
            { popnf_stdes(&Y,st1);
              ttransp(&Y,st2);
            }
           asgnst(st1,&X);
           move(MST,st2);
           break;
         }
#if LET_P
       case AR0(AP_LETP) :
         { STACK X,Y;

           n = (long)arity(m)-2L;
           ttransp(st1,st2);	/* SUB	*/
           popnf_stdes(&X,st1);
           ttransp(&X,st2);
           for ( ; n > 0L ; --n)
            { popnf_stdes(&Y,st1);
              ttransp(&Y,st2);
            }
           asgnst(st1,&X);
           move(MST,st2);
           break;
         }
#endif
       case AR0(AP_AL)  :
         { STACK X,Y;
           popnf_stdes(&Y,st1);
           ttransp(&Y,st2);	/* func  */
           popnf_stdes(&X,st1);
           ttransp(&X,st2);	/* arg 1 */
           popnf_stdes(&Y,st1);
           ttransp(&Y,st2);	/* arg 2 */
           asgnst(st1,&X);
           move(MST,st2);
           break;
         }
       case AR0(LREC)  :
         { STACK X;
           ttransp(st1,st2);	/* goal  */
           popnf_stdes(&X,st1);
           ttransp(&X,st2);	/* fkten  */
           ttransp(st1,st2);	/* arg 2 */
           asgnst(st1,&X);
           move(MST,st2);
           break;
         }
        case AR0(FILRED) :
          if (multi_atom(topof(st1))) {
            transport(st1,st2);
            if (travmastr(st2,BST,lbuf,LENFIL) < LENFIL) {
	/* dateiname nach BST und lbuf */
              if (RD_OK(loadexpr(U1ST,lbuf,1,FN_RED))) {	/* load ok ?      */
                rtransp(U1ST,st2);
                delete(BST);	/* dateiname loeschen      */
                ppop(MST);	/* FILRED loeschen         */
               }
              else {	/* load nicht ok                    */
                delete(U1ST);	/* fehlerhaften ausdruck loeschen ? */
                transport(BST,st2);	/* dateiname zurueck                */
                push(st2,LSTR1 | tovalue(FIL_RED));
                ppop(MST);	/* FILNAM loeschen         */
                if (ergto >= SUCCESS) {
                  ergto = UND_FIL;
                  strcpy(errfn,lbuf);	/* remember error file name	*/
                }
              }
             }
            else {
            }
           }
          else {	/* kein multi_atom auf st1          */
            ttransp(st1,st2);	/* ???????        */
            move(MST,st2);	/* FILNAM nach st2                  */
          }
          break;
       default          :
         for (n = (long)arity(m) ; n > 0L ; --n)
            ttransp(st1,st2);
         move(MST,st2);
         break;
     }	/* end switch */
  }
 else
  { if (single_atom(m))
     { n = 0L;
       do
        { switch(movenf(st1,st2))
           { case AQUOTE : ++n;break;
             case EQUOTE : --n;break;
             default     : ;
           }
        } while (n != 0L);
     }
    else
     { if (marked0(m))
          movenf(st1,st2);
       else
        { pushi(popnf(st1) & ~MARK_FLAG,st2);
          while(marked1(movenf(st1,st2)));
          pushi(pop(st2) | MARK_FLAG,st2);
        }
     }
  }
}
/* ------------ tored ------------------------------------------------ */
/* st1 -> st2                                                          */
/* use: BST, U1ST, U2ST                                                */
tored(st1,st2)	/* st1 --> st2 */
STACK *st1,*st2;
{
#if TRACE
  extern int trace;
#endif
  char *strcpy();
  STACK X;

  clearstack(BST);
  clearstack(U2ST);
  strcpy(edfilbuf,editfil[FN_ED]);	/* filenamen retten */
  ST1 = st1;
  ST2 = U2ST;
  ergto = SUCCESS;
#if TRACE
if (trace > 1)  dsplstack(ST1,"ST1 vor to_red",1,0);
#endif
  to_red();	/* ST1 -> ST2 */
  strcpy(editfil[FN_ED],edfilbuf);
  asgnst(&X,U2ST);
#if TRACE
if (trace > 1) dsplstack(U2ST,"U2ST vor ttransp",1,0);
#endif
  ttransp(&X,st2);
#if TRACE
if (trace > 1) dsplstack(st2,"st2 nach to_red",1,0);
#endif
  clearstack(U2ST);
  return(ergto);
}

/* hier kein transport , pop o.ae. !!!!!!!!!!!!!!!!!!!!!!!	*/
static ftransp(st1,st2)
STACK *st1,*st2;
{
#if TRACE
  extern int ftrace;
#endif
 static STACKELEM m;
 long n;

#if TRACE
ASSERT(st1!=st2);
if (ftrace > 7) dsplstack(st1,"st1 in ftransp",1,0);
#endif
 if (isconstr(m = topof(st1)))
  { movenf(st1,MST);
    switch(m & ~VALUE_FIELD) {
       case AR0(SYSAP) :
       case AR0(AP)     :
       case AR0(APAL)   :
       case AR0(CALL)   :
       case AR0(CALL1)  :
       case AR0(CALL2)  :
       case AR0(CALL3)  :
       case AR0(CALL4)  :
#ifdef UnBenutzt
       case AR0(APVAR)  :
#endif
       { STACK X,Y;
         n = (long)arity(m)-1L;
         popnf_stdes(&X,st1);
         ftransp(&X,st2);
         for ( ; n > 0L ; --n)
          { popnf_stdes(&Y,st1);
            ftransp(&Y,st2);
          }
         asgnst(st1,&X);
         move(MST,st2);
         break;
       }
       case AR0(E_LET)  :
       case AR0(AP_LET)  :
       case AR0(AP_LETPAR)  :
       { STACK X,Y;

         n = (long)arity(m)-1L;
         ftransp(st1,st2);	/* goal	*/
         for ( ; n > 0 ; --n ) {	/* variablen	*/
           ftransp(st1,st2);
         }
         for ( n = (long)arity(topof(MST))-1L ; n > 0L ; --n) {
           ftransp(st2,HST);
         }

         popnf_stdes(&X,st1);
         ftransp(&X,st2);
         ftransp(HST,st2);	/* variable	*/
         push(st2,LDEC);
         for ( n = (long)arity(topof(MST))-2L ; n > 0L ; --n) {
           popnf_stdes(&Y,st1);
           ftransp(&Y,st2);
           ftransp(HST,st2);	/* variable	*/
           push(st2,LDEC);
         }
         asgnst(st1,&X);
         move(MST,st2);
         break;
       }
#if LET_P
       case AR0(E_LETP)  :
       { STACK X,Y;

         n = (long)arity(m)-1L;
         ftransp(st1,st2);	/* goal	*/
         for ( ; n > 0 ; --n ) {	/* pattern	*/
           ftransp(st1,st2);
         }
         for ( n = (long)arity(topof(MST))-1L ; n > 0L ; --n) {
           ftransp(st2,HST);
         }

         popnf_stdes(&X,st1);
         ftransp(&X,st2);
         ftransp(HST,st2);	/* pattern	*/
         push(st2,PDEC);
         for ( n = (long)arity(topof(MST))-2L ; n > 0L ; --n) {
           popnf_stdes(&Y,st1);
           ftransp(&Y,st2);
           ftransp(HST,st2);	/* pattern	*/
           push(st2,PDEC);
         }
         asgnst(st1,&X);
         move(MST,st2);
         break;
       }
#endif
     default          :
       for (n = (long)arity(m) ; n > 0L ; --n)
          ftransp(st1,st2);
       move(MST,st2);
       break;
     }	/* end switch */
  }
 else {
   if (single_atom(m)) {
      n = 0L;
       do
        { switch(movenf(st1,st2))
           { case AQUOTE : ++n;break;
             case EQUOTE : --n;break;
             default     : ;
           }
        } while (n != 0L);
     }
    else {
       if (marked0(m)) {
          movenf(st1,st2);
        }
       else {
          pushi(popnf(st1) & ~MARK_FLAG,st2);
          while(marked1(movenf(st1,st2)));
          pushi(pop(st2) | MARK_FLAG,st2);
       }
    }
  }
}

/* ------------- fromred --------------------------------------------- */
/* st1 -> st2                                                          */
/* use: BST, U1ST, VST                                                 */
fromred(st1,st2,back)	/* R --> E */
STACK *st1,*st2;
int back;
{
  STACK X;
#if TRACE
  extern int ftrace;
#endif

  clearstack(BST);
  clearstack(VST);
  ST1 = st1;
  ST2 = VST;
  BACK = back;

/* ftrace = 2;	*/

#if TRACE
if (ftrace > 1)   dsplstack(ST1,"ST1 vor from_red",1,0);
#endif
  isinstr = FALSE;
  from_red();	/* R --> B */
  asgnst(&X,VST);
#if TRACE
if (ftrace > 1) dsplstack(&X,"X vor ftransp",1,0);
#endif
  ftransp(&X,st2);
#if TRACE
if (ftrace > 1)  dsplstack(st2,"st2 nach ftransp",1,0);
#endif
  clearstack(BST);
  clearstack(VST);
}

/* end of ctransl1.c */
