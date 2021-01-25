/* file list-depth.c */
/* Revision History

Dieses File enthaelt die Definition der Funktion LDEPTH, die
die Schachtelungstiefe einer Liste ermittelt.

Rev 1.0  9.10.1990 uh - initial revision	
 
*/

/*--------------------------------------------------------------------------- */
/*  list-depth.c  - external:  ldepth                                         */
/*--------------------------------------------------------------------------- */

#define TAGGED_INT(x) ( (int)(x) & 1 )


#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"

int ldepth(ptdesc)
/* ptdesc ist ein Zeiger auf einen beliebigen Descriptor, oder ein getaggter Integer.
   Ist ptdesc kein Listen-Descriptor (Liste, ind. Liste, String), so ist die
   Tiefe der referenzierten Struktur 0.
   Referenziert ptdesc eine Liste (also auch NIL), so ist die Tiefe >=1 
*/
    PTR_DESCRIPTOR ptdesc;
    {
       PTR_HEAPELEM ptel;
       int i; int d, maxd=0;

       if (TAGGED_INT(ptdesc)) return(0);
       if (ptdesc==_nil) return(1);
       if (!T_POINTER(ptdesc)) post_mortem("ldepth: pointer to descriptor expected!");
       if (R_DESC(*ptdesc,class)!=C_LIST) return(0);
       switch (R_DESC(*ptdesc,type)) {

         case TY_STRING:  /* String or indirect String */ 
         case TY_UNDEF :  /* proper or indirect list */
                          ptel = R_LIST(*ptdesc,ptdv); 
                          for (i=R_LIST(*ptdesc,dim); i>0; i--, ptel++) {
                            if (T_POINTER(*ptel)) {
                              d=ldepth(*ptel);
                              if (d>maxd) maxd=d;
                            }
                          }
                          return(maxd+1);

         default:         post_mortem("ldepth: unknown Descriptor-Type of class LIST");
                          break;
       } 
   }
