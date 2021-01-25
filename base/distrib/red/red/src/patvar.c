/* file patva.c */
/* Revision History

Dieses File enthaelt die Definition der Funktion PATVAR, die
die Anzahl der (definierende) vorkommenden Variablen innerhalb eines Patterns bestimmt.
Die initiale Version nimmt an, dass Pattern linear sind; jedes Vorkommen
einer Variablen wird als ein neues Vorkommen gewertet. (Bei nichtlinearen Pattern
ist nur das erste Vorkommen definierend, die folgenden angewandt und duerfen daher 
nicht gezaehlt werden.)


Rev 1.0  24.10.1990 uh - initial revision	
 
*/

/*--------------------------------------------------------------------------- */
/*  patvar.c  - external:  patvar                                             */
/*--------------------------------------------------------------------------- */

#define TAGGED_INT(x) ( (int)(x) & 1 )


#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"

int patvar(pat)
/* pat ist ein beliebiges Stackelement oder ein Zeiger auf einen Descriptor. 
   
*/
    T_PTD pat;
    {  PTR_HEAPELEM px;
       int i,n;

       if (!pat) return (0);
       if (TAGGED_INT(pat)) return(0);
       if (T_POINTER(pat)) { /* Pointer to Descriptor */
          if (R_DESC(*pat,type) == TY_VAR ||
              R_DESC(*pat,type) == TY_NAME) return(1);
          if (R_DESC(*pat,type) == TY_EXPR) {
             px = R_EXPR(*pat,pte);
             n=0;
             for (i=(int)*px++; i>0; i--,px++) {
                n += patvar(*px);
             }
             return(n);   
          }
          if (R_DESC(*pat,class)!=C_LIST) return(0);
          switch (R_DESC(*pat,type)) {
             case TY_STRING:  /* String or indirect String */ 
             case TY_UNDEF :  /* proper or indirect list */
                              px = R_LIST(*pat,ptdv); 
                              n=0;
                              for (i=R_LIST(*pat,dim); i>0; i--, px++) {
                                n += patvar(*px);
                              }
                              return(n);

             default:         post_mortem("ldepth: unknown Descriptor-Type of class LIST");
                              break;
          }
       } 
   }
