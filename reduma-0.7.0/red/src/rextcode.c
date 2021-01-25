/*****************************************************************************/
/* file rextcode.c                                                           */
/*                                                                           */
/*****************************************************************************/

#include <ctype.h>
#include <math.h>
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rinter.h"
#include "rprimf.h"

#include "dbug.h"


extern void test_inc_refcnt();
extern void rel_heap(), res_heap();
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */

/* --- panik exit --- */

#if    DEBUG
#define POST_MORTEM(mes)      if (codefp) { fclose(codefp); codefp = NULL; post_mortem(mes); } else post_mortem(mes)
  /* geaendert von stt */
#else
#define POST_MORTEM(mes)      post_mortem(mes)
#endif /* DEBUG     immer auskommentiert von RS 3.12.1992 */


char * convrstr();

#if STORE
/* the list of module names and imported expressions */
/* has been constructed in reac.c:load_module        */

typedef struct module_list { char *name;
                             PTR_DESCRIPTOR expr;
                             struct module_list * next;} MODULE_LIST;

extern MODULE_LIST *m_start; 
#endif /* STORE */


/*---------------------------------------------------------------------*/
/* printelements: (siehe auch ret_mvt() in rhinout.c)                  */
/* Ausgabe der Elemente einer Matrix, eines Vektors, eines TVektors    */
/* oder einer Liste in das codefile                                    */
/*---------------------------------------------------------------------*/

int printelements(codefp, desc, level)
FILE *codefp;
PTR_DESCRIPTOR desc;
int level;
{
  DESC_CLASS class;
  int nrows, ncols, size, i, j;
  STACKELEM x;
  char line[64], * type = 0;
  PTR_DESCRIPTOR p;
  void s_prstel();
#if STORE
  MODULE_LIST *m;
#endif /* STORE */

  if T_POINTER(desc) {
#if STORE
    for (m = m_start; m != NULL ; m = m->next)
      if (m->expr == desc)
      {
        fprintf(codefp, "\t ");
        for(i = 0; i < level; i++) fprintf(codefp, "  ");
        fprintf(codefp, "_import(0x%08x,%d,\"%s\")",(int)desc, (int)strlen(m->name), m->name);
        if (level==0) fprintf(codefp, ";\n");
        return(0);
      };
#endif /* STORE */

    class = R_DESC(*desc,class);
    if (R_DESC(*desc,class) != C_FUNC)
      fprintf(codefp, "\t ");
    for(i = 0; i < level; i++) fprintf(codefp, "  ");
    switch(class) {
      case C_SCALAR: {
        if (R_DESC((*desc), type) == TY_REAL)
	  fprintf(codefp, "_real(0x%08x, %s)",
              (int)desc, convrstr(R_SCALAR((*desc),valr)));
        else
          POST_MORTEM("unknown scalar");
	break;
	}
      case C_LIST:  {
  	size = R_LIST((*desc),dim);
  	RES_HEAP;
  	if (R_DESC((*desc), type) == TY_UNDEF)
    	  fprintf(codefp, "_list( 0x%08x,%d", (int)desc, size);
  	else
    	  fprintf(codefp, "_str( 0x%08x,%d", (int)desc, size);
  	for ( i = 0; i < size; i++) {
    	  x = R_LIST((*desc),ptdv)[i];
    	  if T_INT(x) {
      	    x = VAL_INT(x);
      	    fprintf(codefp, ",int(%d)", (int) x);
    	    }
   	  else if T_POINTER(x) {
	    fprintf(codefp, ",\n");
	    printelements(codefp, (PTR_DESCRIPTOR)x, level+1);
	    if ((i < size-1) && !(T_POINTER(R_LIST((*desc),ptdv)[i+1]))) {
	      fprintf(codefp, "\n\t ");
              for(j = 0; j < level; j++) fprintf(codefp, "  ");
	      }
	    }
   	  else if T_BOOLEAN(x) {
      	    if (x == SA_FALSE)
	      fprintf(codefp, ",bool(false)");
      	    else
              fprintf(codefp, ",bool(true)");
	      }
   	  else if T_STR(x) {
     	    s_prstel(line, x, 1);
            if (isprint(x>>16))
       	      fprintf(codefp, ",'%c'", x>>16);
            else
              fprintf(codefp, ",'\\%03o'", x>>16);    /* stt 20.02.96 */
     	    }
   	  else {
     	    s_prstel(line, x, 1);
     	    fprintf(codefp, ",%s", line);
     	    }
#if 0
	        ret_dec(x);        /* Digitstring einlagern */
	        goto main_m;
#endif
    	  }
  	fprintf(codefp, " )");
  	REL_HEAP;
	break;
	}
      case C_TVECTOR: ;
      case C_VECTOR: ;
      case C_MATRIX: {
        nrows = R_MVT((*desc),nrows,class);
        ncols = R_MVT((*desc),ncols,class);
  	size = ncols * nrows;
  	RES_HEAP;
	switch(R_DESC((*desc), type)) {
	  case TY_INTEGER: type="TY_INTEGER"; break;
	  case TY_REAL: type="TY_REAL"; break;
	  case TY_BOOL: type="TY_BOOL"; break;
	  case TY_STRING: type="TY_STRING"; break;
          case TY_UNDEF: type="TY_UNDEF"; break;
	  }
        switch(class) {
          case C_MATRIX  : 
	    fprintf(codefp,"_mat(0x%08x,%s,%d,%d", (int)desc, type, nrows, ncols); break;
          case C_VECTOR  : 
	    fprintf(codefp,"_vect(0x%08x,%s,%d,%d", (int)desc, type, nrows, ncols); break;
          case C_TVECTOR :
	    fprintf(codefp,"_tvect(0x%08x,%s,%d,%d", (int)desc, type, nrows, ncols); break;
          }
  	for (i = 0; i < size; i++) {
	  switch(R_DESC(*desc,type)) {
    	    case TY_INTEGER:
      	      fprintf(codefp, ", %d", (int)R_MVT((*desc),ptdv, class)[i]);
	      break;
   	    case TY_REAL:
	      fprintf(codefp,", %s",
	          convrstr(((double *)R_MVT((*desc),ptdv,class))[i]));
	      break;
   	    case TY_BOOL:
	      fprintf(codefp, ", %s", R_MVT((*desc),ptdv, class)[i] ? "true" : "false");
	      break;
   	    case TY_STRING:
	      fprintf(codefp, ",\n");
	      printelements(codefp, (PTR_DESCRIPTOR)R_MVT((*desc),ptdv, class)[i], level+1);
	      break;
   	    default:
     	      s_prstel(line, R_MVT((*desc),ptdv, class)[i], 1);
     	      fprintf(codefp, ", %s", line);
#if 0
		  ret_dec(x);        /* Digitstring einlagern */
		  goto main_m;
#endif
    	      }
	  }
  	fprintf(codefp, " )");
  	REL_HEAP;
	break;
	}
      case C_CONSTANT:
        if ((R_DESC((*desc), type) == TY_VAR)
            && (R_DESC((*R_VAR((*desc), ptnd)), class) == C_EXPRESSION)
            && (R_DESC((*R_VAR((*desc), ptnd)), type) == TY_NAME)) {
          p = R_VAR((*desc), ptnd);
          fprintf(codefp, "_name( 0x%08x,%d,\"", (int)desc, R_NAME(*p, ptn)[0]);
          for(i = 1; i <= R_NAME(*p, ptn)[0]; i++)
            fprintf(codefp, "%c", (R_NAME(*p, ptn)[i])>>16);
          fprintf(codefp, "\" )");
          }
        else
          POST_MORTEM("illegal name descriptor in output");
      }
    if ((R_DESC(*desc,class) != C_FUNC) && (level == 0))
      fprintf(codefp, ";\n");
    return 0;
    }
  else {
    return 0; /* kein Pointer, also weder Matrix noch Vektor */
    }
} /* printelements */
