/*-----------------------------------------------------------------*/
/* pm_st_slice legt neuen Listendeskriptor an                      */
/*  Parameter arg liefert Listendeskriptor dessen elemete          */
/*  o_offs bis offs in den Ergebnisdeskriptor kopiert werden       */
/*-----------------------------------------------------------------*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "pminstr.h"



STACKELEM pm_st_slice (pm_arg, pm_o_offs, pm_offs)
STACKELEM pm_arg;
int     pm_o_offs;
int     pm_offs;
{
STACKELEM help;
    register    PTR_DESCRIPTOR desc;
    register int    dim;
    STACKELEM x;

    START_MODUL ("pm_st_slice");


    if ((desc = (PTR_DESCRIPTOR)newdesc ()) == NULL)
    {
	END_MODUL ("pm_st_slice");
	return ((STACKELEM) NULL);
    }
    DESC_MASK (desc, 1, C_LIST, TY_UNDEF);
    /*STORE_EDIT ((*desc), ((x & F_EDIT) >> O_EDIT)); */
    dim = pm_offs - pm_o_offs + 1;  
    L_LIST ((*desc), dim) = dim;

    if (newheap (dim, A_LIST ((*desc), ptdv)) == 0)
    {
	END_MODUL ("pm_st_slice");
	return ((STACKELEM) NULL);
    }

    while ((pm_offs - pm_o_offs) >= 0)
    {
	    help = R_LIST ((*(PTR_DESCRIPTOR) pm_arg), ptdv)[pm_offs];
    dim = pm_offs - pm_o_offs;
	L_LIST ((*desc), ptdv)[dim] = help;
	    --pm_offs;
   if(T_POINTER( help))
	   {
    	   INC_REFCNT( (PTR_DESCRIPTOR)help);
	   }
	   }

    END_MODUL ("pm_st_slice");
    return ((STACKELEM) desc);
}
