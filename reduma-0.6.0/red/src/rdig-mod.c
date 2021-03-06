/* $Log: rdig-mod.c,v $
 * Revision 1.4  1995/05/22  09:06:38  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:49:37  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/***************************************************************************/
/*                DIGIT - MODULO         (digit-arithmetic)                */
/*-------------------------------------------------------------------------*/
/*  rdig-mod.c  - external :  digit_mod;                                   */
/*                called by:  red_mod,     digit_mvt_op, mvt_mvt_op,       */
/*              - internal :  none                                         */
/***************************************************************************/

#include "rstdinc.h"
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rdigit.h"

#include "dbug.h"

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 */
extern PTR_DESCRIPTOR newdesc();
extern PTR_DESCRIPTOR digit_mul();
extern PTR_DESCRIPTOR digit_sub();
extern PTR_DESCRIPTOR digit_div();
extern PTR_DESCRIPTOR digit_trunc();
#if DEBUG
extern PTR_DESCRIPTOR digit_test();
#endif

/* RS 30/10/92 */ 
extern BOOLEAN digit_lt();                     /* rdig-v1.v */
/* End of RS 30/10/92 */ 

#if DEBUG
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

/*----------------------------------------------------------------------------*/
/*  digit_mod  --  fuehrt die Modulooperation mit Digitstrings durch.         */
/*                                                                            */
/*                 Die Moduloberechnung wird entsprechend dem nachfolgendem   */
/*                 applikativen Programm durchgefuehrt :                      */
/*                                                                            */
/*                        ap sub [ X , Y ]                                    */
/*                           in ap sub [ M ]                                  */
/*                                 in ( X - ( M * Y ) )                       */
/*                              to [ trunc( ( X / Y ) ) ]                     */
/*                        to [ desc1 , desc 2 ]                               */
/*                                                                            */
/*  globals    --  _dig0       <r>                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

PTR_DESCRIPTOR digit_mod(desc1,desc2)
PTR_DESCRIPTOR desc1,desc2;
{
   PTR_DESCRIPTOR hilf1, hilf2;
   COUNT sign1, sign2;             /* Vorzeichen */
   int exp2, ndigit2;
#if DEBUG
   extern int debconf;

   DBUG_ENTER ("digit_mod");

  if (debconf) printf("\n\n\n ***  digit-mod gestartet ***\n");
#else
   DBUG_ENTER ("digit_mod");
#endif

   if (desc2 == _dig0)
      DBUG_RETURN((PTR_DESCRIPTOR)0);

   if (desc1 == _dig0)
   {
      INC_REFCNT(desc1);
      DBUG_RETURN(DIGIT_TEST(desc1,"digit_mod",desc1,desc2));
   }

   ndigit2 = R_DIGIT((*desc2),ndigit);
   exp2    = R_DIGIT((*desc2),Exp);

   if ( exp2 == 0  &&  ndigit2 == 1         /* zweite Zahl gleich 1 */
                   &&  R_DIGIT((*desc2),ptdv)[0] == 1)
   {
      INC_REFCNT(_dig0);                    /* dann muss Ergebnis 0 sein */
        DBUG_RETURN(_dig0);
   }

   sign1 = R_DIGIT((*desc1),sign);          /* Vorzeichen retten */
   sign2 = R_DIGIT((*desc2),sign);
   if (sign1)
      L_DIGIT((*desc1),sign) = PLUS_SIGN;
   if (sign2)
      L_DIGIT((*desc2),sign) = PLUS_SIGN;

   if (digit_lt(desc1,desc2))
   {
      INC_REFCNT(desc1);
      L_DIGIT((*desc1),sign) = sign1;
      L_DIGIT((*desc2),sign) = sign2;
      DBUG_RETURN(DIGIT_TEST(desc1,"digit_mod",desc1,desc2));
   }

   hilf1 = (PTR_DESCRIPTOR) digit_div(desc1,desc2);
   if (hilf1 == 0)
      DBUG_RETURN((PTR_DESCRIPTOR)0);

   hilf2 = (PTR_DESCRIPTOR) digit_trunc(hilf1);
   DEC_REFCNT(hilf1);

   hilf1 = hilf2;

   if (hilf1 == 0)
     DBUG_RETURN((PTR_DESCRIPTOR)0);
#if DEBUG
  if (debconf) printf("mod: trunc( ( desc1 / desc2 ) )  durchgefuehrt\n");
#endif

   hilf2 = (PTR_DESCRIPTOR) digit_mul(hilf1,desc2);
   if (hilf2 == 0)
   {
      DEC_REFCNT(hilf1);
      DBUG_RETURN((PTR_DESCRIPTOR)0);
   }
#if DEBUG
  if (debconf) printf("mod: ( M * desc2 ) durchgefuehrt\n");
#endif

   DEC_REFCNT(hilf1);
   hilf1 = (PTR_DESCRIPTOR) digit_sub(desc1,hilf2);
   if (hilf1 == 0)
   {
      DEC_REFCNT(hilf2);
      DBUG_RETURN((PTR_DESCRIPTOR)0);
   }
#if DEBUG
  if (debconf) printf("mod: ( desc1 - ( M * desc2 ) )  durchgefuehrt\n");
#endif

   DEC_REFCNT(hilf2);
   if (sign1)
   {
      L_DIGIT((*desc1),sign) = sign1;
      if (hilf1 != _dig0)
        L_DIGIT((*hilf1),sign) = sign1;
   }
   if (sign2)
      L_DIGIT((*desc2),sign) = sign2;
   DBUG_RETURN (DIGIT_TEST(hilf1,"digit_mod",desc1,desc2));
}
