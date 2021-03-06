/*{{{  Erklaerung*/
/* pmanalyze.c */
/* Dieses File definiert die Funktion pmanalyze_list.
   Sie analysiert und annotiert ein Pattern.

   Im einzelnen werden folgende Informationen bereitgestellt:

   Tiefe des Patterns:


                               ....

*/
/*}}}  */

/*{{{  #includes*/
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rstelem.h"
#include "pmanalyze.h"
#include "dbug.h"
/*}}}  */

/*{{{  #defines*/
#define HEAPELEM  int

#define ANNOTATE(ptr,type)  \
          MAKE_PATTERN(ptr,type);\
          L_PATTERN(*(T_PTD)ptr,following)=INFO(ptdesc,elements)
/*}}}  */

#if DEBUG
 FILE *ofp;
 char pmanal_name[100];
#endif

void pmanalyze_list(); /* prototype :-) */
extern int newheap();
extern int newdesc();
extern DescDump();

/*{{{  pmanalyze*/
pattern_info pmanalyze(ptpattern)
/*===============================*/
/*{{{  Vorspann*/
 PTR_HEAPELEM ptpattern;
{ /* ptpattern zeigt auf einen Konstruktorausdruck im Heap */

 pattern_info result,p_i;

 DBUG_ENTER("pmanalyze");

#if DEBUG
 fprintf(ofp,"pmanalyze: call with ptpat=%x, *ptpat=%x\n",(int)ptpattern,(int)*ptpattern);
#endif

 result.bindings=0;  result.sublist=FALSE;

#if DEBUG
 if (!ptpattern) post_mortem("pmanalyze: pointer to pattern expected, NIL received!");
#endif
/*}}}  */

/*{{{  Pattern ist ein integer*/
 if (IS_TAGGED_INT(*ptpattern)) DBUG_RETURN(result);
/*}}}  */

/*{{{  Pattern ist AS Pattern Variable*/
 if (T_PM_AS(*ptpattern)) {  /* AS Pattern Variable */
    p_i=pmanalyze(ptpattern+1);
    result.bindings+=p_i.bindings+1;  /* Bindungen des Patterns und die AS-Variable */
    result.sublist =p_i.sublist;
    result.list_info=p_i.list_info;
    DBUG_RETURN(result);
 }
/*}}}  */

/*{{{  Pattern ist IN Skipel Pattern*/
 if (T_PM_IN(*ptpattern)) { /* IN skip charset */
    result=pmanalyze(ptpattern+1);  /* analysiere skips */
    DBUG_RETURN(result);
 }
/*}}}  */

/*{{{  Pattern ist Descriptor*/
 if (T_POINTER(*ptpattern)) {  /* Descriptor */

/* neu von RS 28.2.1995 */
  if ((R_DESC(*(T_PTD)*ptpattern,class)==C_SCALAR) || (R_DESC(*(T_PTD)*ptpattern,class)==C_DIGIT))
    DBUG_RETURN(result);

/*{{{  Pattern ist Liste*/
if (IS_LIST(*ptpattern)) {  /* Listenpattern */
   if ((T_PTD)*ptpattern==_nil) /* NIL zaehlt als Atom */
      DBUG_RETURN(result);

   pmanalyze_list((T_PTD)*ptpattern);
   result.bindings+= INFO(*ptpattern,variables);
   result.list_info= PATINFO(*ptpattern);
   result.sublist=TRUE;

   DBUG_RETURN(result);
}
/*}}}  */

/*{{{  Pattern ist Variable*/
 if (IS_VAR(*ptpattern)) { /* Variable */
    result.bindings+=1;
    DBUG_RETURN(result);
 }
/*}}}  */

/*{{{  Pattern ist unbekannter Descriptor*/
/* #ifdef DEBUG */
    post_mortem("pmanalyze: received unhandled pointer to descriptor!");
/* #endif */
 }
/*}}}  */
/*}}}  */

/*{{{  Pattern ist primitives Element*/
 if (T_PM_SKIP(*ptpattern) || T_PM_SKSKIP(*ptpattern) || T_PM_PT_PLUS(*ptpattern) ||
     T_PM_SKSTAR(*ptpattern)) {
     /* . oder ... oder .+ oder .* */
   DBUG_RETURN(result);
 }
/*}}}  */

/*{{{  Pattern ist sonstwas*/
     /* Zum Beispiel eine primitive Funktion. */
     DBUG_RETURN(result); /* uh 30.Aug 91 */
/* #ifdef DEBUG
 { char* errmsg[80];
   sprintf(errmsg,"pmananlyze: received unhandled pattern: %x!",*ptpattern);
   post_mortem(errmsg);
 }
#endif
*/
}
/*}}}  */
/*}}}  */

/*{{{  pmanalyze_list*/
void pmanalyze_list(ptdesc)
/*===============================*/
  T_PTD ptdesc;

{
   int i;
   PTR_HEAPELEM el,ppattern;
   int last=TRUE;

    DBUG_ENTER ("pmanalyze_list");

#if DEBUG
    fprintf(ofp,"pmanalyze_list: call with ptdesc=%x\n",(int)ptdesc);
#endif

#if DEBUG
    if (!ptdesc || !T_POINTER(ptdesc) || R_DESC(*ptdesc,class) != C_LIST)
       post_mortem("pmanalyze_list: Pointer to list descriptor expected!");
#endif

#if DEBUG
    if (R_LIST(*ptdesc,ptdd)) post_mortem("pmanalyze_list: PTDD already used!");
#endif

#if DEBUG
    if (ptdesc==_nil) post_mortem("pmanalyze_list: received _NIL (treat as atom!)");
#endif

    ALLOC_INFO(ptdesc);  /* Informations-Struktur an Descriptor anhaengen */

    INIT_INFO(ptdesc);

    /* Liste von hinten nach vorne durchlaufen in Teilliste rekursiv absteigen. */

   i  = R_LIST(*ptdesc,dim);
   el = R_LIST(*ptdesc,ptdv) + i;  /* auf das letzte Element */

   while (i--) { --el;
     if (T_POINTER(*el)) { /* Descriptor */

       if (IS_LIST(*el)) { /* Teilliste */
          if (*el==(int)_nil) { /* BUGFIX by RS 1995 */
            INFO(ptdesc,elements)++;
            continue;
            }
          pmanalyze_list(*el);
          /* Daten von den Unterstrukturen "hochziehen" */
          if (INFO(*el,levels) > INFO(ptdesc,levels)) { /* bisher tiefste Teilstruktur */
             INFO(ptdesc,levels)=INFO(*el,levels);
          }
          INFO(ptdesc,variables)+=INFO(*el,variables);
          INFO(ptdesc,totalskskips)+=INFO(*el,totalskskips);
          INFO(ptdesc,lastskskips)+=INFO(*el,lastskskips);
          INFO(ptdesc,elements)++;

          continue;
       }

       if ((IS_VAR(*el)) || (R_DESC(*(T_PTD)(*el),class)==C_DIGIT)) { /* Variable */
          INFO(ptdesc,variables)++;
          INFO(ptdesc,elements)++;
          continue;
       }

       if (IS_AS(*el)) { /* AS pattern Var */
          INFO(ptdesc,variables)++;
          ppattern= (R_EXPR(*(T_PTD)*el,pte)+2);

          if (T_PM_IN(*ppattern)) { /* AS IN skips charset  VAR    skips : . ... oder + */
             ppattern= (R_EXPR(*(T_PTD)*el,pte)+3); /* skips */
          }

          if (T_PM_SKSKIP(*ppattern)) {  /* AS ... Var */
             INFO(ptdesc,skskips)++;
             INFO(ptdesc,totalskskips)++;

             if (last) { /* letztes ... auf dieser Ebene */
               ANNOTATE(*ppattern,TY_LASTDOTDOTDOT);
               last=FALSE;
               INFO(ptdesc,lastskskips)++;
               continue;
             }
             /* nicht letztes */
             ANNOTATE(*ppattern,TY_DOTDOTDOT);
             continue;
          }
          if (T_PM_SKSTAR(*ppattern)) {  /* AS .* Var */
             INFO(ptdesc,skskips)++;
             INFO(ptdesc,totalskskips)++;

             if (last) { /* letztes ... auf dieser Ebene */
               ANNOTATE(*ppattern,TY_LASTDOTSTAR);
               last=FALSE;
               INFO(ptdesc,lastskskips)++;
               continue;
             }
             /* nicht letztes */
             ANNOTATE(*ppattern,TY_DOTSTAR);
             continue;
          }
          if (T_PM_PT_PLUS(*ppattern)) {  /* AS .+ Var */
             INFO(ptdesc,skskips)++;
             INFO(ptdesc,totalskskips)++;
             INFO(ptdesc,elements)++;

             if (last) { /* letztes .+ auf dieser Ebene */
               ANNOTATE(*ppattern,TY_LASTDOTPLUS);
               last=FALSE;
               INFO(ptdesc,lastskskips)++;
               INFO(ptdesc,elements)++;
               continue;
             }
             /* nicht letztes */
             ANNOTATE(*ppattern,TY_DOTPLUS);
             INFO(ptdesc,elements)++;
             continue;
          }
          if (*ppattern && T_POINTER(*ppattern)) { /* descriptor */
            if (IS_LIST(*ppattern)) {  /* AS listpattern Var */
              if (*ppattern==(int)_nil) { /* BUGFIX by RS 1995 */
                INFO(ptdesc,elements)++;
                continue;
                }
              pmanalyze_list(*ppattern);
              /* Daten von den Unterstrukturen "hochziehen" */
              if (INFO(*ppattern,levels) > INFO(ptdesc,levels)) { /* tiefste Teilstr */
                 INFO(ptdesc,levels)=INFO(*ppattern,levels);
              }
              INFO(ptdesc,variables)+=INFO(*ppattern,variables);
              INFO(ptdesc,totalskskips)+=INFO(*ppattern,totalskskips);
              INFO(ptdesc,lastskskips)+=INFO(*ppattern,lastskskips);
              INFO(ptdesc,elements)++;
              continue;
            }
            if (IS_VAR(*ppattern)) { /* AS Var Var */
               INFO(ptdesc,variables)++;
               INFO(ptdesc,elements)++;
               continue;
            }
          }
          if (T_PM_SKIP(*ppattern)) {
             INFO(ptdesc,elements)++;
             continue;
          }

/* #ifdef DEBUG */
         post_mortem("pmanalyze_list: unknown first component in AS construct!");
/* #endif */
       }  /* AS pattern Var */

       if (IS_IN(*el)) { /* IN skips charset */  /* skips : . ... oder + */
          ppattern = (R_EXPR(*(T_PTD)*el,pte)+2);  /* skips */
          if (T_PM_SKIP(*ppattern)) {  /* IN . charset */
            INFO(ptdesc,elements)++;
            continue;
          }
          if (T_PM_SKSKIP(*ppattern)) {  /* IN ... charset */
             INFO(ptdesc,skskips)++;
             INFO(ptdesc,totalskskips)++;

             if (last) { /* letztes ... auf dieser Ebene */
               ANNOTATE(*ppattern,TY_LASTDOTDOTDOT);
               last=FALSE;
               INFO(ptdesc,lastskskips)++;
               continue;
             }
             /* nicht letztes */
             ANNOTATE(*ppattern,TY_DOTDOTDOT);
             continue;
          }
          if (T_PM_SKSTAR(*ppattern)) {  /* IN .* charset */
             INFO(ptdesc,skskips)++;
             INFO(ptdesc,totalskskips)++;

             if (last) { /* letztes ... auf dieser Ebene */
               ANNOTATE(*ppattern,TY_LASTDOTSTAR);
               last=FALSE;
               INFO(ptdesc,lastskskips)++;
               continue;
             }
             /* nicht letztes */
             ANNOTATE(*ppattern,TY_DOTSTAR);
             continue;
          }
          if (T_PM_PT_PLUS(*ppattern)) {  /* IN .+ charset */
             INFO(ptdesc,skskips)++;
             INFO(ptdesc,totalskskips)++;

             if (last) { /* letztes .+ auf dieser Ebene */
               ANNOTATE(*ppattern,TY_LASTDOTPLUS);
               last=FALSE;
               INFO(ptdesc,lastskskips)++;
               INFO(ptdesc,elements)++;
               continue;
             }
             /* nicht letztes */
             ANNOTATE(*ppattern,TY_DOTPLUS);
             INFO(ptdesc,elements)++;
             continue;
          }
/* #ifdef DEBUG */
        post_mortem("pmanalyze_list: unknown 2nd componenet of IN construct!");
/* #endif */
       }

/* #ifdef DEBUG */
       post_mortem("pmanalyze_list: Illegal descriptor in list!");
/* #endif */
     }
     /* skalares Element, also Integer oder Konstruktor */
     if (T_PM_SKSKIP(*el)) {
          INFO(ptdesc,skskips)++;
          INFO(ptdesc,totalskskips)++;
          if (last) {  /* letztes ... auf dieser Ebene */
             ANNOTATE(*el,TY_LASTDOTDOTDOT);
             last=FALSE;
             INFO(ptdesc,lastskskips)++;
             continue;
          }
          ANNOTATE(*el,TY_DOTDOTDOT);
          continue;
     }
     if (T_PM_SKSTAR(*el)) {
          INFO(ptdesc,skskips)++;
          INFO(ptdesc,totalskskips)++;
          if (last) {  /* letztes .* auf dieser Ebene */
             ANNOTATE(*el,TY_LASTDOTSTAR);
             last=FALSE;
             INFO(ptdesc,lastskskips)++;
             continue;
          }
          ANNOTATE(*el,TY_DOTSTAR);
          continue;
     }
     if (T_PM_PT_PLUS(*el)) {
          INFO(ptdesc,skskips)++;
          INFO(ptdesc,totalskskips)++;
          if (last) {  /* letztes .+ auf dieser Ebene */
             ANNOTATE(*el,TY_LASTDOTPLUS);
             last=FALSE;
             INFO(ptdesc,lastskskips)++;
             INFO(ptdesc,elements)++;      /* zaehlt als ein element iG zu ... */
             continue;
          }
          /* nicht letztes .+ */
          ANNOTATE(*el,TY_DOTPLUS);
          INFO(ptdesc,elements)++;      /* zaehlt als ein element iG zu ... */
          continue;
     }
     INFO(ptdesc,elements)++;
   }

   INFO(ptdesc,levels)++;

  DBUG_VOID_RETURN;

} /* pmanalyze_list */
/*}}}  */

/*{{{  print_info*/
#if DEBUG
void print_info(info)
  listpattern_info info;
{
    fprintf(ofp,"\nPattern-Information\n");
    fprintf(ofp,"levels:       %d\n", info.levels);
    fprintf(ofp,"skskips:      %d\n", info.skskips);
    fprintf(ofp,"totalskskips: %d\n", info.totalskskips);
    fprintf(ofp,"lastskskips:  %d\n", info.lastskskips);
    fprintf(ofp,"variables:    %d\n", info.variables);
    fprintf(ofp,"elements:     %d\n", info.elements);

}

#endif
/*}}}  */
