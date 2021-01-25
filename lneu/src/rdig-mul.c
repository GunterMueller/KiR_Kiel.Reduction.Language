/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */

/***************************************************************************/
/*                DIGIT - MULTIPLICATION     (digit-arithmetic)            */
/*-------------------------------------------------------------------------*/
/*  rdig-mul.c  - external :  digit_mul;                                   */
/*                called by:  red_mult,    digit_mvt_op, mvt_mvt_op,       */
/*                            digit_div,   digit_mod                       */
/*              - internal :  none                                         */
/***************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"
#include "rmeasure.h"
#include "rdigit.h"

/*----------------------------------------------------------------------------*/
/*                        (add.) extern variables                             */
/*----------------------------------------------------------------------------*/
extern int _prec_mult;                        /* precision for multiplication */

/*----------------------------------------------------------------------------*/
/*                             extern functions                               */
/*----------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR newdesc();
#if DEBUG
  extern PTR_DESCRIPTOR digit_test();
#endif


PTR_DESCRIPTOR digit_mul(desc1,desc2)
PTR_DESCRIPTOR desc1, desc2;
{
   PTR_DESCRIPTOR res_desc;                     /*      result descriptor     */
   PTR_HEAPELEM op1_start,                      /* start address of 1.operand */ 
                op1_start_save,  /* start address of data-vector of 1.operand */
                op1_start_prec,        /* possibly start address of 1.operand */
                op1_end,                        /* end address  of  1.operand */
                op2_start,                      /* start address of 2.operand */
                op2_end,                        /* end address  of  2.operand */
                res_start,                      /*  start address of  result  */
                res_start_save,     /* start address of data-vector of result */
                res_end,                        /*   end address  of  result  */
                ptr_op1,                        /*    points into 1.operand   */
                ptr_op2,                        /*    points into 2.operand   */
                ptr_res,                        /*    points into result      */
                hlp_ptr;
   T_HEAPELEM   op2_digit,                      /*      digit of 2.operand    */
                temp,                           /*      temporary result      */
                carry;                          /*  carry of temporary result */
   int          i,j,                            /*     counter variables      */
                ndigit_op1,                     /* no. of digits in 1.operand */
                ndigit_op2,                     /* no. of digits in 2.operand */
                ndigit_res,                     /*   no. of digits in result  */
                maxdigit;                     /* max. no. of digits in result */
   extern BOOLEAN _digit_recycling;             /* flag for possibly over-
                                                   writing of argument desc.  */

#if DEBUG
   extern int debconf;

  if (debconf) printf("\n\n\n*****  digit_multiplication started! *****\n\n");
/*if (debconf) DescDump(stderr,desc1,1);*/      /*     output of arguments    */
/*if (debconf) DescDump(stderr,desc2,1);*/
#endif

  if (_prec_mult > 0)
      maxdigit = _prec_mult + 1;
  else
      maxdigit = _prec_mult;
#if DEBUG
  if (debconf) printf("maximum number of digits in result:%d\n", maxdigit);
#endif
  if (    (ndigit_op2 = R_DIGIT((*desc2),ndigit)) >
          (ndigit_op1 = R_DIGIT((*desc1),ndigit))   )
  {
      res_desc = desc1;                         /* 2.arg. has more digits     */ 
      desc1 = desc2;                            /* than 1.argument, i.e. they */
      desc2 = res_desc;                         /* get exchanged;             */
      i = ndigit_op1;
      ndigit_op1 = ndigit_op2;
      ndigit_op2 = i;
  }

  op1_start = op1_start_save = (PTR_HEAPELEM) R_DIGIT((*desc1),ptdv);
  op2_start = (PTR_HEAPELEM) R_DIGIT((*desc2),ptdv);

  if ( (*op1_start == 0) || (*op2_start == 0) ) /*  one argument equal to 0   */
  {
      INC_REFCNT(_dig0);
#if DEBUG
      if (debconf) printf("one argument equal to 0\n");
#endif
      DIGNULL;
      return(_dig0);
  }

  if (*op2_start == 1  &&  ndigit_op2 == 1)     /* 2.argument equal to 1 or ~1*/ 
  {
#if DEBUG
      if (debconf) printf("2.argument has absolute value of 1\n");
#endif
      if (R_DESC((*desc1),ref_count) == 1  &&  _digit_recycling)
      {
          if (R_DIGIT((*desc2),sign) == MINUS_SIGN)
          L_DIGIT((*desc1),sign) = (R_DIGIT((*desc1),sign) ? PLUS_SIGN : MINUS_SIGN);
          L_DIGIT((*desc1),Exp) += R_DIGIT((*desc2),Exp);
          INC_REFCNT(desc1);
          return(DIGIT_TEST(desc1,"digit_mul",desc1,desc2));
      }
      else
      {
          DIGDESC(res_desc,ndigit_op1);         /*      create descriptor     */
          if (R_DIGIT((*desc2),sign) == MINUS_SIGN)
              L_DIGIT((*res_desc),sign) = (R_DIGIT((*desc1),sign) ?
                                                        PLUS_SIGN : MINUS_SIGN);
          else
              L_DIGIT((*res_desc),sign) = R_DIGIT((*desc1),sign);

          L_DIGIT((*res_desc),Exp) = R_DIGIT((*desc1),Exp) + R_DIGIT((*desc2),Exp);
          L_DIGIT((*res_desc),ndigit) = ndigit_op1;

          RES_HEAP;
          ptr_res = (PTR_HEAPELEM) R_DIGIT((*res_desc),ptdv);
          ptr_op1 = (PTR_HEAPELEM) R_DIGIT((*desc1),ptdv);

          for ( i=0; i<ndigit_op1; i++)
              *ptr_res++ = *ptr_op1++;
          REL_HEAP;
          return(DIGIT_TEST(res_desc,"digit_mul",desc1,desc2));
      }
  }

  /*----------------------------------------------------*/
  /*   special cases solved; now has to be calculated   */
  /*----------------------------------------------------*/

  op1_end   = op1_start_save + ndigit_op1 - 1;
  op2_end   = op2_start + ndigit_op2 - 1;

  /*---- adjusting in 1st operand: ----*/
  if (maxdigit > 0)
  {
      if (maxdigit <= ndigit_op2)
      {
          if (ndigit_op1 > ndigit_op2)
              op1_start_prec = op1_end - ndigit_op2;
          else
              op1_start_prec = op1_start_save;
      }
      else
          op1_start_prec = op1_end - maxdigit + 1;
  }
  else
      op1_start_prec = op1_start_save;

  /*---- adjusting in result vector: ----*/
  if (op1_start_prec < op1_start_save)
      ndigit_res = ndigit_op1 + ndigit_op2;
  else
      ndigit_res = (op1_end - op1_start_prec + 1) + ndigit_op2;

  DIGDESC(res_desc,ndigit_res);           /* get descriptor incl. heapspace */
  L_DIGIT((*res_desc),sign) = (R_DIGIT((*desc1),sign) ?
               (R_DIGIT((*desc2),sign) ? PLUS_SIGN : MINUS_SIGN)   :
                             R_DIGIT((*desc2),sign) );
  L_DIGIT((*res_desc),Exp) = R_DIGIT((*desc1),Exp) + R_DIGIT((*desc2),Exp) + 1;

  RES_HEAP;
  res_start_save = (PTR_HEAPELEM)R_DIGIT((*res_desc),ptdv);
  res_end   = res_start_save + ndigit_res -1;

#if DEBUG
  if (debconf)
  {
      printf("start of result vector:%d\n",res_start_save);
      printf("end   of result vector:%d\n",res_end);
  }   
#endif
  /*---- initialization for 'do-loop': ----*/
  if (op1_start_prec < op1_start_save)
      res_start = res_start_save + ndigit_op2;
  else
      res_start = res_start_save + ndigit_op2 - 1;

  op2_digit = *op2_end;                      /* last digit of 2nd argument */

for (ptr_res = res_start_save; ptr_res <= res_end; ) /*  preset of result vector  */
    *ptr_res++ = 0;

ptr_op2 = op2_start;

do
{
    if (maxdigit > 0)
    {
        if (op1_start_prec < op1_start_save)
        {
            op1_start = op1_start_save;
            res_start--;                       /* correct exponent */
        }
        else
            op1_start = op1_start_prec++;
        ptr_res = res_start;
    }
    else
        ptr_res = res_start--;

    ptr_op1 = op1_start;
    op2_digit = *op2_end--;

#if DEBUG
    if (debconf)
    {
        printf("actual start of 1st operand  :%d\n",op1_start);
        printf("actual start of result vector:%d\n\n",res_start);
        printf("actual digit of 2nd operand  :%d\n",op2_digit);
    }
#endif
  no_carry:

#if DEBUG
    if (debconf) printf("actual digit of 1.operand:%d\n",*ptr_op1);
#endif
    if ((temp = *ptr_op1++ * op2_digit + *ptr_res) > MAXNUM)
    {
        *ptr_res++ = temp % BASE;                     /* digit without carry */
        carry      = temp / BASE;
        if (ptr_op1 > op1_end)
            goto finish_with_carry;
        goto with_carry;
    }
    else  
    {
        *ptr_res++ = temp;                    
        if (ptr_op1 > op1_end)
            goto finish_no_carry;
        goto no_carry;
    }
  with_carry:
#if DEBUG
    if (debconf) printf("actual digit of 1.operand:%d\n",*ptr_op1);
#endif

    if ((temp = *ptr_op1++ * op2_digit + carry + *ptr_res) > MAXNUM)
    {
        *ptr_res++ = temp % BASE;                     /* digit without carry */
        carry      = temp / BASE;
        if (ptr_op1 > op1_end)
            goto finish_with_carry;
        goto with_carry;
    }
    else  
    {
        *ptr_res++ = temp;                    
        if (ptr_op1 > op1_end)
            goto finish_no_carry;
        goto no_carry;
    }
  finish_with_carry:
    if ((temp = carry + *ptr_res) > MAXNUM)
    {
        *ptr_res++ = temp % BASE;
        carry      = temp / BASE;
        goto finish_with_carry;
    }
    else
    {
        *ptr_res = temp;
    }
  finish_no_carry:

#if DEBUG
  if (debconf)
  {
      printf("partial sum of multiplication: ");
      for (ptr_res = res_end; ptr_res >= res_start_save; )
          printf("%4d",*ptr_res--);
      printf("\n");
  }
#else
  ;
#endif

} while (ptr_op2 <= op2_end);

if (*res_end == 0)                       /*      no carry at highest digit   */
{
    res_end--;     /* wieder wegnehmen!!! */
    ndigit_res--;
    L_DIGIT((*res_desc),Exp)--;
}
while (*res_start_save == 0)             /*          drop LSB zeros          */
{
    for (ptr_res = res_start_save; 
         ptr_res <= res_end;
         ptr_res++)
        *ptr_res = *(ptr_res + 1);
    ndigit_res--;
    res_end--; 
}
while (*res_end == 0)                    /*          drop MSB zeros          */
{
    res_end--;
    ndigit_res--;
}
while (    (maxdigit > 0 ? 
            maxdigit < ndigit_res :
            FALSE)                       /*      cut to maxdigit digits      */
       || (*res_start_save == 0)   )     /*          drop LSB zeros          */
{
    for (ptr_res = res_start_save; 
         ptr_res <= res_end;
         ptr_res++)
        *ptr_res = *(ptr_res + 1);
    ndigit_res--;
    res_end--; 
}
L_DIGIT((*res_desc),ndigit) = ndigit_res;
L_DIGIT((*res_desc),ptdv) = res_start_save;


REL_HEAP;
return(DIGIT_TEST(res_desc,"digit_mul",desc1,desc2));

}
/***************************  end of file rdig-mul.c *************************/
