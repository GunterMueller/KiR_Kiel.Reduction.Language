



/*----------------------------------------------------------------------------*/
/*  pm_red_eq steuert den Vergleich von Konstanten wie 'k` , true, und        */
/*            Zahlen in fix- und varformat Darstellung. Das Programm          */
/*            entspricht red_eq in (hur)boolfunc:c ohne Refcountbehandlg.     */
/*            fuer die Argumente                                              */
/*----------------------------------------------------------------------------*/



#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "pminstr.h"




#define DESC(x)  (* (PTR_DESCRIPTOR) x)

extern PTR_DESCRIPTOR newdesc ();         /* rheap.c    */
extern int  newheap ();                   /* rheap.c    */
extern BOOLEAN str_eq();                  /* rlogfunc.c */
/*---------------------------------------------------------*/
/* pm_scal_eq vergleicht Zahlen im varformat               */
/*---------------------------------------------------------*/


BOOLEAN pm_scal_eq (arg1, arg2)
register    STACKELEM arg1,
            arg2;
{

    START_MODUL ("pm_scal_eq");


        switch (R_DESC (DESC (arg2), class))
        {
            case C_SCALAR:
                 if (R_DESC (DESC (arg1), type) == TY_INTEGER)
                 {
                     if (R_DESC (DESC (arg2), type) == TY_INTEGER)
                     {
                          END_MODUL ("pm_scal_eq");
                          return (R_SCALAR (DESC (arg1), vali) ==
                                   R_SCALAR (DESC (arg2), vali));
                     }
                     else
                     {
                          END_MODUL ("pm_scal_eq");
                          return (R_SCALAR (DESC (arg1), vali)
                                   == R_SCALAR (DESC (arg2), valr));
                     }
                 }
                 else
                 {
                     if (R_DESC (DESC (arg2), type) == TY_INTEGER)
                     {
                          END_MODUL ("pm_scal_eq");
                          return (R_SCALAR (DESC (arg1), valr)
                                   == R_SCALAR (DESC (arg2), vali));
                     }
                     else
                     {
                          END_MODUL ("pm_scal_eq");
                          return (R_SCALAR (DESC (arg1), valr)
                                   == R_SCALAR (DESC (arg2), valr));
                     }
                 }
        }
    END_MODUL ("pm_scal_eq");
    return (FALSE);                /* fuer alle sonstigen Faelle 0
                                      zurueckgeben      */

}

/*------------------------------------------------------------*/
/*pm_bool_eq vergleicht zwei boolsche Werte                   */
/*------------------------------------------------------------*/
BOOLEAN pm_bool_eq (arg1, arg2)
register    STACKELEM arg1,
            arg2;
{
    if (T_BOOLEAN (arg1))
    {
        return ((arg1 & ~F_EDIT) == (arg2 & ~F_EDIT));
    }
    return (FALSE);
}






#if LARGE

/*****************************************************************************/
/*                           pm_red_f_eq                                     */
/*---------------------------------------------------------------------------*/
/* function  :  Vergleichen von zwei Matrizen bzw. Vektoren bzw. Tvektoren   */
/*              mit der Hilfe des patterm_matches.                           */
/*              fast die gleiche routine existiert in rlogfunc.c (red_eq)    */
/* parameter :  arg1 --  argument from stack                                 */
/*              arg2 --  argument from stack                                 */
/* examples  :  ( smat<'1,2,3`>. pm_red_f_eq  smat<'1`,'2`,'3`>.)  => false  */
/*              ( vect< 1,2,3 >  pm_red_f_eq  vect< 1,2,3 > )      => true   */
/*              (tvect<'1,2,3`>  pm_red_f_eq tvect<'1,2,3`> )      => true   */
/*                                                                           */
/* returns   :  '0' :  if reduction wasn't successful  (fail )               */
/*              '1' :  if reduction was successful     (equal)               */
/* calls     :  pm_digit_eq, str_eq                                          */
/*****************************************************************************/
pm_red_f_eq (arg1, arg2)
register    STACKELEM arg1,arg2;
{
  register int  i, dim;
  int    *ip1,  *ip2;
  double *rp1,  *rp2;
  char   class1,class2;            /* fuer die Klasse von   arg1 / arg2    */

  START_MODUL ("pm_red_f_eq");

  if (!T_POINTER(arg1) || !T_POINTER(arg2))
    goto fail;

  if ((class1 = R_DESC(DESC(arg1),class)) !=
      (class2 = R_DESC(DESC(arg2),class)))
    goto fail;              /* fail, wenn arg1 und arg2 nicht der selben     */
                            /*       Klasse entsprechen                      */
  switch(class1) {
    case  C_MATRIX :
    case  C_VECTOR :
    case  C_TVECTOR: break;
    default : goto fail;        /* fail, wenn arg2 nicht in den Klassen      */
  } /** switch class1 **/       /*     C_MATRIX oder C_VECTOR oder C_TVECTOR */

  switch(class2) {
    case  C_MATRIX : if ((dim = R_MVT(DESC(arg1),nrows,class1)) !=
                                R_MVT(DESC(arg2),nrows,class2))
                       goto fail;         /* nicht die selbe Zeilenanzahl    */
                     if ((i = R_MVT(DESC(arg1),ncols,class1)) !=
                              R_MVT(DESC(arg2),ncols,class2))
                       goto fail;         /* nicht die selbe  Spaltenanzahl  */
                     break;
    case  C_VECTOR : if ((i = R_MVT(DESC(arg1),ncols,class1)) !=
                              R_MVT(DESC(arg2),ncols,class2))
                       goto fail;                /* nicht dieselbe Dimension */
                     dim = 1;
                     break;
    case  C_TVECTOR: if ((dim = R_MVT(DESC(arg1),nrows,class1)) !=
                                R_MVT(DESC(arg2),nrows,class2))
                       goto fail;                /* nicht dieselbe Dimension */
                     i = 1;
                     break;
    default : goto fail;        /* fail, wenn arg2 nicht in den Klassen      */
  } /** switch class2 **/       /*     C_MATRIX oder C_VECTOR oder C_TVECTOR */

  dim *= i;

  switch (R_DESC (DESC (arg1), type)) {

      case TY_DIGIT:
          if (R_DESC(DESC(arg2),type) != TY_DIGIT)
               goto fail;

          for (i = 0; i < dim; ++i)
               if (!pm_digit_eq (R_MVT(DESC(arg1),ptdv,class1)[i],
                                 R_MVT(DESC(arg2),ptdv,class2)[i]))
                   goto fail;
          goto equal;

      case TY_INTEGER:
          switch(R_DESC(DESC(arg2),type)) {
               case TY_INTEGER:
                   ip1 = (int *) R_MVT(DESC(arg1),ptdv,class1);
                   ip2 = (int *) R_MVT(DESC(arg2),ptdv,class2);

                   for (i = 0; i < dim; ++i)
                        if (ip1[i] != ip2[i])
                            goto fail;
                   goto equal;
               case TY_REAL:
                   ip1 = (int *) R_MVT(DESC(arg1),ptdv,class1);
                   rp2 = (double *) R_MVT(DESC(arg2),ptdv,class2);

                   for (i = 0; i < dim; ++i)
                        if (ip1[i] < rp2[i])
                            goto fail;
                   goto equal;
               default:
                   goto fail;
          } /** switch type of arg2 **/
      case TY_REAL:
          switch(R_DESC(DESC(arg2),type)) {
               case TY_INTEGER:
                   rp1 = (double *) R_MVT(DESC(arg1),ptdv,class1);
                   ip2 = (int *) R_MVT(DESC(arg2),ptdv,class2);

                   for (i = 0; i < dim; ++i)
                        if (rp1[i] != ip2[i])
                            goto fail;
                   goto equal;
               case TY_REAL:
                   rp1 = (double *) R_MVT(DESC(arg1),ptdv,class1);
                   rp2 = (double *) R_MVT(DESC(arg2),ptdv,class2);

                   for (i = 0; i < dim; ++i)
                        if (rp1[i] < rp2[i])
                            goto fail;
                   goto equal;
               default:
                   goto fail;
          } /** switch type of arg2 **/
      case TY_BOOL:
          if (R_DESC(DESC(arg2),type) != TY_BOOL)
               goto fail;
          ip1 = (int *) R_MVT(DESC(arg1),ptdv,class1);
          ip2 = (int *) R_MVT(DESC(arg2),ptdv,class2);

          for (i = 0; i < dim; ++i)
               if (ip1[i]) {
                   if (!ip2[i])
                        goto fail;
               }
               else
                   if (ip2[i])
                        goto fail;
          goto equal;
      case TY_STRING:
          if (R_DESC(DESC(arg2),type) != TY_STRING)
               goto fail;

          for (i = 0; i < dim; ++i)
               if (!str_eq(R_MVT(DESC(arg1),ptdv,class1)[i],
                           R_MVT(DESC(arg2),ptdv,class2)[i]))
                   goto fail;
          goto equal;
      case TY_MATRIX:        /* Matrizen von Matrizen sin nicht vergleichbar */
               goto fail;
      case TY_UNDEF:  /* nilmat  or  nilvect  or niltvect */
          if (R_DESC(DESC(arg2),type) != TY_UNDEF)
               goto fail;
          goto equal;
      default:
          goto fail;
  } /** end switch type of arg1 **/

fail:
    END_MODUL ("pm_red_f_eq");             /* not_equal   or  fail            */
    return (0);

equal:
    END_MODUL ("pm_red_f_eq");
    return (1);

} /**** end of function pm_red_f_eq ****/





/*---------------------------------------------------------------------------
    pm_digit-eq  --  prueft, ob zwei Dezimalzahlen gleich sind
  ---------------------------------------------------------------------------
*/

BOOLEAN pm_digit_eq(desc1,desc2)
PTR_DESCRIPTOR desc1,desc2;
{
   int *data1, *data2, i;

   if (desc1 == desc2)
      return(TRUE);

   if ((data1 = R_DIGIT((*desc1),ptdv))[0] != (data2 = R_DIGIT((*desc2),ptdv))[0])
   {
      return(FALSE);
   }
   if (R_DIGIT((*desc1),ndigit) != R_DIGIT((*desc2),ndigit) )
   {
      return(FALSE);
   }
   if (R_DIGIT((*desc1),Exp ) != R_DIGIT((*desc2),Exp ) )
   {
      return(FALSE);
   }
   if (R_DIGIT((*desc1),sign) != R_DIGIT((*desc2),sign) )
   {
      return(FALSE);
   }

   for (i=1; R_DIGIT((*desc1),ndigit) > i; i++)      /* Ziffern vergleichen */
       if (data1[i] != data2[i])
       {
          return(FALSE);
       }

   return(TRUE);
}

#endif 


