/* $Log: pre-pmpatvar.c,v $
 * Revision 1.2  1995/01/13  16:41:02  rs
 * uh pm bugfixes
 *
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 * */
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rinstr.h"


extern StackDesc S_pm;
#if UH_ZF_PM
extern StackDesc S_tilde;
#endif /* UH_PM_ZF */

extern void      DescDump();
extern STACKELEM st_name();
extern STACKELEM st_list();
extern void      res_heap();
extern void      rel_heap();
extern void      test_inc_refcnt();
extern BOOLEAN   check_list_for_store();
extern void      ret_name();
extern void      stack_error();
extern void      ppopstack();

/**************************************************************************/
/*                                                                        */
/*  pm_patvar -- traversiert ein Pattern vom E-stack auf den A-Stack,     */
/*            und pusht die im pattern vorkommenden Variablen auf         */
/*            den pm-stack.                                               */
/*            Variable aus einem as ... X liegen dabei ueber den          */
/*            anderen Variablen des Pattern .                             */
/*            Ausserdem werden in dem Pattern vorkommende Strings ausge-  */
/*            lagert.                                                     */
/*                                                                        */
/**************************************************************************/


void  pm_patvar ()
{
    int i;
    register    STACKELEM x,y;
    register    STACKELEM hilf;
    STACKELEM  *str1, *str2;


    START_MODUL ("pm_patvar");

    PUSHSTACK (S_m, KLAA);

trav_source:

    x = POPSTACK (S_e);

    if (T_CON (x))
    {
        if( (T_PM_AS(x))
            && ((T_PM_SKSKIP(READSTACK(S_e)))
                ||  (T_PM_DOLLAR(READSTACK(S_e))))    )
        {
             PUSHSTACK(S_m, x); /* das as */
             PUSHSTACK(S_a, POPSTACK(S_e)); /* das SKIPSKIP   */
                                            /* oder Dollar  */

            /* Kopieren der Variablen */
             x = POPSTACK (S_e);
             if (T_POINTER (x))
             {
                 if (R_DESC ((*(PTR_DESCRIPTOR) x), type) == TY_NAME)
                 {
                   y = (R_NAME((*(PTR_DESCRIPTOR)x),ptn))[1];
                   if ((T_LET1(y)) && (VALUE(y) == '%'))
                      {
/* ---> car <--- */
                      if ((R_DESC((*(PTR_DESCRIPTOR)x),ref_count)) > 1) { 
                       ret_name(x);
                       PPOPSTACK(S_e);  /* % */
                       x = st_name();
                       }
                      else {
						int namelength=0;
                        PTR_DESCRIPTOR desc;
                        int desc_counter,
                            j;
                        STACKELEM  * descname;
                       desc_counter=SIZEOFSTACK(S_v);
                       str1=R_NAME((*(PTR_DESCRIPTOR)x), ptn);
     				   str2=str1+1;
					   i=(*str1);
					   i--; /* i=laenge-1*/
					   namelength=i;
					   for ( j = 0; j<desc_counter; j++)
  						{
					       READ_V(j, (STACKELEM)desc);
					       RES_HEAP;
 
					       if (R_DESC((*desc),type) == TY_NAME )
           					descname = (STACKELEM *) R_NAME((*desc),ptn);
       				   	   else
           					descname= (STACKELEM *)R_FNAME((*desc),ptn);
 
					       for (i = 1; i <= namelength; i++)
         					if ((str2[i] & ~F_EDIT) != (descname[i] & ~F_EDIT))
           					break;
       					  REL_HEAP;
 
       					  if ( i > namelength)  /* Descriptor gefunden: */
       					    {
         					INC_REFCNT(desc);
        					x=(STACKELEM)desc;
       					    }
  						}
 
   					  if (j==desc_counter) /* Ein neuer Name! */
						{
						(*str1)--;  /* laenge wird 1 kuerzer */
                        while(i--) *++str1 = *++str2;
						}
                      }
                      }
                    PUSHSTACK (S_i, x);
#if UH_ZF_PM
                 PUSHSTACK (S_tilde, x);
#endif /* UH_ZF_PM */
                    PUSHSTACK (S_a, x);
                    INC_REFCNT ((PTR_DESCRIPTOR) x);
                    PUSHSTACK(S_a, POPSTACK(S_m));
                    goto trav_m;
                 }

            } /* T_POINTER */


           /* Pattern hat die Form as ... . */

            if (T_PM_SKIP(x))
            {
                PUSHSTACK(S_a, x);
        
                /* as vom m-stack auf den a-stack*/
                hilf = POPSTACK(S_m);
                PUSHSTACK(S_a, hilf);

                goto trav_m;
            }    

        } /* as ... oder as $ */

        PUSHSTACK (S_m, x);
        PUSHSTACK (S_m1, x);
        goto trav_source;

    } /* T_CON */


    if (T_ATOM (x)) {                /*  ein Atom (ausser letter und digit)  */
                                     /*  also z.B. character,TRUE,FALSE ...  */
      if (T_POINTER (x)) {           /*  hier muss Variable auf Hilfstack    */
                                     /*  gelegt werden;                      */
        if (R_DESC ((*(PTR_DESCRIPTOR) x), type) == TY_NAME)
           {
             y = (R_NAME((*(PTR_DESCRIPTOR)x),ptn))[1];
             if ((T_LET1(y)) && (VALUE(y) == '%'))
                {
/* ---> car <--- */
                      if ((R_DESC((*(PTR_DESCRIPTOR)x),ref_count)) > 1) {
                       ret_name(x);
                       PPOPSTACK(S_e);  /* % */
                       x = st_name();
                       }
                      else {
						int namelength=0;
                        PTR_DESCRIPTOR desc;
                        int desc_counter,
                            j;
                        STACKELEM  * descname;
                       desc_counter=SIZEOFSTACK(S_v);
                       str1=R_NAME((*(PTR_DESCRIPTOR)x), ptn);
					   str2=str1+1;
					   i=(*str1);
					   i--; /* i=laenge-1*/
                       namelength=i;
                       for ( j = 0; j<desc_counter; j++)
                        {
                           READ_V(j, (STACKELEM)desc);
                           RES_HEAP;
 
                           if (R_DESC((*desc),type) == TY_NAME )
                            descname = (STACKELEM *) R_NAME((*desc),ptn);
                           else
                            descname= (STACKELEM *)R_FNAME((*desc),ptn);
 
                           for (i = 1; i <= namelength; i++)
                            if ((str2[i] & ~F_EDIT) != (descname[i] & ~F_EDIT))
                            break;
                          REL_HEAP;
 
                          if ( i > namelength)  /* Descriptor gefunden: */                            {
                            INC_REFCNT(desc);
                            x=(STACKELEM)desc;
                            }
                        }
 
                      if (j==desc_counter) /* Ein neuer Name! */
						{
						(*str1)--; /* laenge wird 1 kuerzer */ 
                        while(i--) *++str1 = *++str2;
						}
                       }
                 PUSHSTACK(S_i,x);                 
#if UH_ZF_PM  
                 PUSHSTACK (S_tilde, x);
#endif /* UH_ZF_PM */
                }
             else          
#if UH_ZF_PM  
              {  PUSHSTACK (S_tilde, x);
#endif /* UH_ZF_PM */
                 PUSHSTACK (S_pm, x);
#if UH_ZF_PM
              }
#endif UH_ZF_PM

            INC_REFCNT ((PTR_DESCRIPTOR) x);
         }
      }
      PUSHSTACK (S_a, x);
      goto trav_m;
    }  /** Ende von T_ATOM **/

    post_mortem ("pm_patvar: No match successfull on trav_source");

trav_m:

    x = READSTACK (S_m);

    if (T_CON (x))                                                  
    {
        if (ARITY (x) > 1)
        {
            WRITESTACK (S_m, DEC (x));
            goto trav_source;
        }
        else
        {
            PPOPSTACK (S_m);
            x = POPSTACK (S_m1);
            PUSHSTACK (S_a,x); 

            if ( T_STRING(x) )
               if (check_list_for_store())  /* darf String ausgelagert werden? */
               {
                 if ((x = st_list() ) == 0)   /* Wenn ja, auslagern */
                    post_mortem("ea_create: heap out of space in st_list");
                 else
                    PUSHSTACK(S_a,x);
               }

            goto trav_m;
        }

    } /* T_CON */

    if (T_KLAA (x))
    {
        PPOPSTACK (S_m);
        END_MODUL ("pm_patvar");
        return ;
    }

    post_mortem ("pm_patvar: No match successfull on trav_m");
}
