/* $Log ruserf.c,v $
 */

/* file ruserf.c
 * -------------
 * car  1.06.94
 * 
 * auxiliary functions to deal with types of user defined functions
 * - lookupuserf: find type entry for user defined function
 * - getretuserf: check if arguments are of correct type
 *                compute result type for given arguments
 */

#include <string.h>
#include "rstdinc.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rstackty.h"
#include "roptasm.h"
#include "rextern.h"
#include "dbug.h"

/* find type entry for user defined function
 */
T_PTD lookupuserf(T_PTD func)
  {
  int i, n;
  T_PTD ptype;

  DBUG_ENTER("lookupuserf");

  /* create temporary function type (using integer args from stack
   * and integer results) */
  /* ------------------------*/
  n = R_COMB((*func), nargs);
  if (SIZEOFSTACK(*ps_w) >= n) {
    MAKEDESC(ptype, 1, C_COMPTYPE, TY_FUNC);
    GET_HEAP(n+1, A_COMPTYPE((*ptype), ptypes));
    L_COMPTYPE((*ptype), nargs) = n;
    L_COMPTYPE((*ptype), cnt) = n+1;
    for(i = 0; i < n; i++) {
      L_COMPTYPE((*ptype), ptypes)[i+1]
          = (T_HEAPELEM)(R_LINKNODE((*(T_PTD)MID_W(i)), ptype));
      INC_REFCNT(R_LINKNODE((*(T_PTD)MID_W(i)), ptype));
      }
    L_COMPTYPE((*ptype), ptypes)[0] = R_COMPTYPE((*ptype), ptypes)[n];
    INC_REFCNT((T_PTD)(L_COMPTYPE((*ptype), ptypes)[0]));
    PUSHSTACK(S_hilf, ptype);
    PUSHSTACK(S_hilf, func);
    INC_REFCNT(ptype);
    INC_REFCNT(func);
    mkcomment("assuming type %s.", type2str(ptype, NULL));
    }
  /* ------------------------*/

  /* find type entry on hilf stack */
  for(i = 0; i < SIZEOFSTACK(S_hilf); i += 2)
    if ((T_PTD)MIDSTACK(S_hilf, i) == func) {
      DBUG_RETURN((T_PTD)MIDSTACK(S_hilf, i+1));
      }
  DBUG_RETURN(undef);
  }

/* compute return type of a polymorphic typed function
 */
T_PTD getretuserf(T_PTD func)
  {
  T_PTD ptype;

  DBUG_ENTER("getretuserf");
  if ((ptype = lookupuserf(func)))
    DBUG_RETURN((T_PTD)R_COMPTYPE((*ptype), ptypes)[0]);
  DBUG_RETURN(undef);
  }
