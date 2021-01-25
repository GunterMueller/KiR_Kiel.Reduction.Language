/* file pmsupport.c */

/* Dieses File enthaelt Funktions-Definitionen, die zur Unterstuetzung des
   PatternMatchings benoetigt werden. */

/* Revision History



Rev 1.0  26.10.1990 uh - initial revision	
 
*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rstelem.h"
#include "pmanalyze.h"
#include <setjmp.h>

extern BOOLEAN _interrupt;    /* reduct.c */
extern jmp_buf _interruptbuf; /* reduct.c */
extern stack_error();
extern st_list();
extern DescDump();
extern ppopstack();
extern newdesc();
extern test_dec_refcnt();


/* Es folgt die Definition der Funktion load_list, die eine Patternliste
   aus dem Heap in den Stack traversiert. Wenn dabei auf oberster Ebene
   ein LA_SKSKIP angetroffen wird, so werden die folgende Listenelemente
   vom Ende der Liste bis zum LA_SKSKIP also rueckwaerts geladen.

   < 1 2 3 4 last... 5 6 7 >  ->    1 2 3 4 last... 7 6 5 | E

*/

#define IS_LASTDOTDOTDOT(ptr) \
   (ptr) && T_POINTER(ptr) && (R_DESC(*(T_PTD)(ptr),class)==C_PATTERN) && \
   (R_DESC(*(T_PTD)(ptr),type)==TY_LASTDOTDOTDOT)

#define IS_LASTDOTPLUS(ptr) \
   (ptr) && T_POINTER(ptr) && (R_DESC(*(T_PTD)(ptr),class)==C_PATTERN) && \
   (R_DESC(*(T_PTD)(ptr),type)==TY_LASTDOTPLUS)

#define IS_LASTDOTSTAR(ptr) \
   (ptr) && T_POINTER(ptr) && (R_DESC(*(T_PTD)(ptr),class)==C_PATTERN) && \
   (R_DESC(*(T_PTD)(ptr),type)==TY_LASTDOTSTAR)

#define IS_LAST(ptr) ((IS_LASTDOTDOTDOT(ptr))||(IS_LASTDOTPLUS(ptr))||(IS_LASTDOTSTAR(ptr)))

void load_list(ptd)
T_PTD ptd;    /* pointer auf Listendescriptor */
{
  int i,j;
  PTR_HEAPELEM pth,ph;
  int dim;
  int el;

  START_MODUL("load_list");

  dim = R_LIST(*ptd,dim);    if (!dim) { END_MODUL("load_list"); return; }
  pth = R_LIST(*ptd,ptdv);

  for (i=0; i<dim-1; i++) {  /* Suche Elemente hinter LA_SKSKIP */
    el=pth[i];
    if (el && T_POINTER(el) && (R_DESC(*(T_PTD)el,type)==TY_EXPR)) {
      ph=R_EXPR( *(T_PTD)el,pte)+2;
      while (T_PM_AS(*ph)) ph++;
      if (T_PM_IN(*ph)) ph++;
      if (IS_LAST(*ph)) break;
    }

    if (IS_LAST(el)) break;  /* last... last.+*/
  }

  for (j=i+1; j<dim; j++) { /* Elemente danach "rueckwaerts" */
    PUSHSTACK(S_e,pth[j]);
  }

  for (; i>=0; i--) { /* Elemente davor */
    PUSHSTACK(S_e,pth[i]);
  } 

  END_MODUL("load_list");
}

/* convert_pattern lagert in einem auf dem A-Stack eingelagerten Patternausdruck 
   die Listen/Strings aus. Dies wird zur Konvertierung des interpretierenden
   Patternmatch zum (auf ASM-) compilierten PMes benoetigt */

void convert_pattern(entries)
int *entries;
{
  register STACKELEM           x /*,y*/;
  register unsigned short    arity;
  extern   jmp_buf   _interruptbuf;                       /* reduct.c */
  volatile int counter=0;

  START_MODUL("convert_pattern");

  setjmp(_interruptbuf);         /* Interruptbehandlung Preprocessing */
  if (_interrupt)                                /* Interrupt erfolgt */
  {
    _interrupt = FALSE;
    post_mortem("ea_create: Interrupt received");
  }

  PUSHSTACK(S_m,DOLLAR);                               /* Ende-Symbol */

  main_e:

  x = READSTACK(S_e);

  if (T_CON(x))
   {
     if (ARITY(x) >0 )
     {
        PPOPSTACK(S_e);
        PUSHSTACK(S_m,x);
        PUSHSTACK(S_m1,x);
        goto main_e;
     }
     else
     {
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;
     }
   }
 
  if (T_POINTER(x))
  {
     PPOPSTACK(S_e);
     if (R_DESC(*(T_PTD)x,type)==TY_NAME) { /* a Variable ! */
       INC_REFCNT((T_PTD)x); 
       *(entries--)=x;
       if ((x = newdesc()) == NULL)
         post_mortem("convert_pattern: heap out of space");
       DESC_MASK((T_PTD)x,1,C_EXPRESSION,TY_NAME);
       L_NAME((*(T_PTD)x), ptn) = (PTR_HEAPELEM) TAG_INT(counter++);
       }
     PUSHSTACK(S_a,x);
     goto main_m;
  }

  if (T_ATOM(x))
  {
     PPOPSTACK(S_e);
     PUSHSTACK(S_a,x);
     goto main_m;
  }

  post_mortem("ea_create: No match successful on main_e");

  main_m:

  x = READSTACK(S_m);
  arity = ARITY(x);

  if (arity > 1)            /* dh die Arity des Konstruktors war >= 2 */
  {
    WRITE_ARITY(S_m,arity-1);
    goto main_e;
  }

  if (arity > 0)                              /* DOLLAR hat arity = 0 */
  {
    PPOPSTACK(S_m);
    x = POPSTACK(S_m1);
    PUSHSTACK(S_a,x);
 
    if ( (T_LIST(x))||(T_STRING(x)) )
      if ((x = st_list() ) == 0)              /* Wenn ja, auslagern */
        post_mortem("ea_create: heap out of space in st_list");
      else
        PUSHSTACK(S_a,x);
    goto main_m;
  }

  /* Ein DOLLAR. Dann ist ea_create zu beenden */
  PPOPSTACK(S_m);                                        /* DOLLAR */

  END_MODUL("convert_pattern");
}
