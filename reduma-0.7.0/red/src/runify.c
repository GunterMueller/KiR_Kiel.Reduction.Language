/* $Log runify.c,v $
 */                
                  
/* file runify.c    
 * -------------    
 * car  1.06.94      
 *                  
 * auxiliary functions to deal with types in general
 * - equaltype: test literal equality of two types
 * - unify: tries to unify two types
 */

#include <string.h>
#include "rstdinc.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rstackty.h"
#include "roptasm.h"
#include "rextern.h"
#include "dbug.h"

extern int post_mortem();
#if DEBUG
extern void    DescDump();
#endif /* DEBUG */

/* convert a type descriptor into a readable ascii notation
 */
char * type2str(T_PTD ptype, char * stype)
  {
  int i, n, f = 1;
  char * s;
  static char buf[1024];
 
  DBUG_ENTER("type2str");
  if (stype == NULL)
    { stype = buf; buf[0] = '\0'; f = 0; }
  if (stype-buf > 1024)
    post_mortem("type too complex");
  switch(R_DESC((*ptype), class)) {
    case C_ATOMTYPE: {
      switch(R_DESC((*ptype), type)) {
        case TY_INTEGER:
          strcpy(stype, "int"); break;
        case TY_BOOL:
          strcpy(stype, "bool"); break;
        case TY_REAL:
          strcpy(stype, "real"); break;
        default:
          strcpy(stype, "unknown"); break;
        }
      break;
      }
    case C_COMPTYPE: {
      n = R_COMPTYPE((*ptype), cnt);
      switch(R_DESC((*ptype), type)) {
        case TY_VECT:
          strcpy(stype, "vect[");
          type2str((T_PTD)R_COMPTYPE((*ptype), ptypes)[0], &stype[5]);
          strcat(stype, "]");
          break;
        case TY_TVECT:
          strcpy(stype, "tvect[");
          type2str((T_PTD)R_COMPTYPE((*ptype), ptypes)[0], &stype[6]);
          strcat(stype, "]");
          break;
        case TY_MAT:
          strcpy(stype, "mat[");
          type2str((T_PTD)R_COMPTYPE((*ptype), ptypes)[0], &stype[4]);
          strcat(stype, "]");
          break;
        case TY_PROD:
          if (f) strcat(stype, "( ");
          if (n) {
            type2str((T_PTD)R_COMPTYPE((*ptype), ptypes)[0], &stype[f]);
            strcat(stype, "^*");
            }
          else
            strcpy(stype, "()^*");
          if (f) strcat(stype, " )");
          break;
        case TY_SUM:
          if (f) strcat(stype, "( ");
          for(i = 0, s = stype; i < n; i++) {
            if (i) strcat(s, " v ");
            s = s+strlen(s);
            type2str((T_PTD)R_COMPTYPE((*ptype), ptypes)[i], s);
            }
          if (f) strcat(stype, " )");
          break;
        case TY_FUNC:
          if (f) strcat(stype, "( ");
          if (R_COMPTYPE((*ptype), ptypes)) {
            for(i = 1, s = stype; i < n; i++) {
              if (i > 1) strcat(s, " x ");
              s = s+strlen(s);
              type2str((T_PTD)R_COMPTYPE((*ptype), ptypes)[i], s);
              }
            if (i > 1) strcat(s, " -> ");
            s = s+strlen(s);
            type2str((T_PTD)R_COMPTYPE((*ptype), ptypes)[0], s);
            }
          else {
            strcat(stype, "prim_fkt");
            }
          if (f) strcat(stype, " )");
          break;
        default:
          DBUG_RETURN(strcpy(buf, "unknown[]"));
        }
      break;
      }
    default: post_mortem("cannot convert not type descriptor to ascii");
    }
  DBUG_RETURN(stype);
  }

/* test if two types are equal
 */
int equaltype(T_PTD ptype1, T_PTD ptype2)
  {
  int i;

  DBUG_ENTER("equaltype");
  /* same atomic type or type constructor? */
  if (R_DESC((*ptype1), class) == R_DESC((*ptype2), class))
    if (R_DESC((*ptype1), type) == R_DESC((*ptype2), type))
      /* same atomic types? */
      if (R_DESC((*ptype1), class) == C_ATOMTYPE)
        DBUG_RETURN(1);
      /* inspect substructurs of constructor */
      else
        if (R_COMPTYPE((*ptype1), cnt) == R_COMPTYPE((*ptype2), cnt)) {
          for(i = 0; i < R_COMPTYPE((*ptype1), cnt); i++)
            if (equaltype((T_PTD)R_COMPTYPE((*ptype1), ptypes)[i],
                (T_PTD)R_COMPTYPE((*ptype2), ptypes)[i]) == 0)
              DBUG_RETURN(0);
          DBUG_RETURN(1);
          }
  DBUG_RETURN(0);
  }

int unify(T_PTD arg1, T_PTD arg2)
  {
  return 1;
  }
