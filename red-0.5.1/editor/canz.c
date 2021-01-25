/* anzeigen                                              anz.c */
/* letzte aenderung:So  18.01.87        11:37:59        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include "cedit.h"
#include "cestack.h"
#include "cencod.h"
#include "ctred.h"

extern char msg[];

/* ------------- anzeigen -------------------------------------------- */
/* anzeigen und cursor auf ce plazieren (wenn moeglich)                */
anzeigen()
{
 long scrto();
 register long int moves;
 register STACKELEM m;

 push(MST,ENDSYMB);
 moves = scrto(EST,AST);                /* an richtige Stelle   */
 scrba(AST,EST);
 display(TRUE);
 /* ev. ndisp-Konstruktor auf M   */
 while ((m = topof(MST)) != STBOTTOM) {
   move(MST,EST);        pop(M1ST);
   if (isdispld(m)) move(AADRST,EADRST);
 }
 scrm(EST,AST,moves);
 while ((m = topof(M1ST)) != STBOTTOM  &&  arity(m) == 0) {
   move(MST,AST);        pop(M1ST);
 }
 while (notdisp(topof(EST)) && !isabbrev(topof(EST)))
  scrlright();
 putcursor();
}
/* ------------- scrm ------------------------------------------------ */
static scrm(e,a,moves)
 STACK *e,*a;
 register long int moves;
 {
  register STACKELEM m;
  register int w;

  do {
    w = TRUE;
    while ( w  &&  (moves > 0l  ||  !isdispld(topof(e)))) {
      moves--;
      m = topof(e);
      if (isdispld(m)) move(EADRST,AADRST);
      if (isconstr(m)) {
        move2(e,MST,M1ST);
        w = FALSE;
       }
      else {     /* !isconstr      */
        if (single_atom(m) || marked0(m))
          move(e,a);
        else {
          push(a,mark0(pop(e)));
          while(marked1(move(e,a)))
          ;
          push(a,mark1(pop(a)));
        }
        w = FALSE;
      }
    }            /* end while w ...         */
    if ( !w ) {
      w = TRUE;
      while ( w  &&  moves > 0l ) {
        if ( isconstr(m = topof(M1ST)) ) {
          if (arity(m) > 0) {
            decrarity(M1ST);
            w = FALSE;
           }
          else {        /* arity = 0    */
            moves--;
            move(MST,a);
            pop(M1ST);
          }
         }
        else {
          sprintf(msg,"Illegal element %08lx in scrm",m);
          error(msg);
        }
      }          /* end while w && moves > 0l        */
    }
   } while ( !w );
 }
/* end of       anz.c */

