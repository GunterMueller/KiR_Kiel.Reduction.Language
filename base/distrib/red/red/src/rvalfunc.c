/* $Log: rvalfunc.c,v $
 * Revision 1.10  1996/03/14 13:48:12  stt
 * refcounting for new primfuncs.
 *
 * Revision 1.9  1996/03/01  17:49:15  stt
 * red_sprintf and red_sscanf completed.
 *
 * Revision 1.8  1996/02/28  11:02:30  stt
 * prim.funcs red_to_char, red_to_ord, red_sprintf and red_sscanf added.
 *
 * Revision 1.7  1996/02/27  10:29:28  car
 * mod fixed
 *
 * Revision 1.6  1995/05/22  11:33:51  rs
 * changed nCUBE to D_SLAVE
 *
 * Revision 1.5  1995/05/03  12:13:02  car
 * real types and mod corrected
 *
 * Revision 1.4  1993/10/28  14:08:29  rs
 * removed warnings update
 *
 * Revision 1.3  1993/09/01  12:37:17  base
 * ANSI-version mit mess und verteilungs-Routinen
 *
 * Revision 1.2  1992/12/16  12:51:21  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */

        
/*****************************************************************************/
/*                          MODUL FOR PROCESSING PHASE                       */
/*---------------------------------------------------------------------------*/
/* Diese Datei enthaelt alle werttransformierden Funktionen, mit Ausnahme    */
/* derjenigen, welche auf boolean arbeiten, bzw. denen Vergleiche zugrunde   */
/* liegen, sowie die tranzendenten Funktionen                                */
/*                      red_sin,  red_cos,  red_tan,  red_ln,  red_exp       */
/*                      welche sich in dem File rbibfunc:c befinden.         */
/*                                                                           */
/* rvalfunc.c  -- externals : red_add,  red_minus,  red_mult,  red_div,      */
/*                            red_mod,  red_ip,     red_abs,   red_trunc,    */
/*                            red_frac, red_floor,  red_ceil,  red_c,        */
/*                            red_vc                                         */
/*             -- called by : rear.c                                         */
/*             -- internals : sind in rval-int.c zu finden.                  */
/*****************************************************************************/

#include <math.h>
#include "rstdinc.h"
#if D_SLAVE
#include "rncstack.h"
#else
#include "rstackty.h"
#endif
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rops.h"
#include "rmeasure.h"
#include <setjmp.h>
#include "rintact.h"

#if D_MESS
#include "d_mess_io.h"
#endif

#include "dbug.h"

/*--------------------------------------------------------------------------
 * Allgemein verwendete externe Variablen:
 *--------------------------------------------------------------------------
 */
extern jmp_buf _interruptbuf;             /* <r>, Sprungadresse bei heapoverflow */
extern PTR_DESCRIPTOR _desc;              /* <w>, der Ergebnisdeskriptor */
extern BOOLEAN _digit_recycling;          /* <w>, Steuerflag fuer die Digitroutinen */
extern int _prec;

/*--------------------------------------------------------------------------
 * Allgemein verwendete externe Routinen:
 *--------------------------------------------------------------------------
 */
extern PTR_DESCRIPTOR newdesc();            /* rheap:c */
extern int newheap();                       /* rheap:c */
extern PTR_DESCRIPTOR  digit_add();
extern PTR_DESCRIPTOR  digit_sub();
extern PTR_DESCRIPTOR  digit_div();
extern PTR_DESCRIPTOR  digit_mul();
extern PTR_DESCRIPTOR  digit_mod();
extern PTR_DESCRIPTOR  digit_abs();         /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_neg();         /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_trunc();       /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_frac();        /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_floor();       /* rdig-v2:c */
extern PTR_DESCRIPTOR  digit_ceil();        /* rdig-v2:c */
extern int             conv_digit_int();    /* rdig-v1:c */
extern BOOLEAN         digit_eq();          /* rdig-v1:c */
extern BOOLEAN         digit_lt();          /* rdig-v1:c */
extern BOOLEAN         digit_le();          /* rdig-v1:c */

#if DEBUG
extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
#endif

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

/* ---- ('mvt' steht fuer Matrix/Vektor/TVektor) ---- */
extern PTR_DESCRIPTOR scal_mvt_op();        /* rval-int:c */
extern PTR_DESCRIPTOR digit_mvt_op();       /* rval-int:c */
extern PTR_DESCRIPTOR mvt_mvt_op();         /* rval-int:c */
extern PTR_DESCRIPTOR mvt_op();             /* rval-int:c */

/* ---- Routine zur Erzeugung indirekter Deskriptoren: ---- */
extern PTR_DESCRIPTOR gen_many_id();        /* rconvert:c */

/* RS 30/10/92 */
extern void stack_error();                  /* rstack.c */
extern BOOLEAN str_lt();                    /* rlogfunc.c */
extern void freedesc();                     /* rheap.c */
/* END of RS 30/10/92 */

/*--------------------------------------------------------------------------
 * Allgemein verwendete Makros und Definitionen:
 *--------------------------------------------------------------------------
 */
/* Makros zum Abfangen von heapoverflows */
#if (D_SLAVE && D_MESS && D_MHEAP)
#define NEWHEAP(size,adr)  {if ((*d_m_newheap)(size,adr) == 0)  longjmp(_interruptbuf,0);}
#else
#define NEWHEAP(size,adr)  {if (newheap(size,adr) == 0)  longjmp(_interruptbuf,0);}
#endif
#define TEST_DESC          {if (_desc == NULL)           longjmp(_interruptbuf,0);}
#define TEST(desc)         {if ((desc) == NULL)          longjmp(_interruptbuf,0);}

#define DESC(x)  (* (PTR_DESCRIPTOR) x)
#define IPNEW    ((int *) R_MVT((*_desc),ptdv,class))
#define IP2      ((int *) R_MVT(DESC(arg2),ptdv,C_MATRIX))

/* digit_add und digit_sub wird die Vorzeichenbetrachtung abgenommen: */
#define DI_ADD(x,y) (R_DIGIT(DESC(x),sign)==0  \
                    ? (R_DIGIT(DESC(y),sign)==0  \
                      ? digit_add(x,y) : digit_sub(x,y) ) \
                    : (R_DIGIT(DESC(y),sign)==0 \
                      ? digit_sub(y,x) : digit_add(x,y) ) )
#define DI_SUB(x,y)  (R_DIGIT(DESC(x),sign) == 0   \
                     ? (R_DIGIT(DESC(y),sign) == 0  \
                       ? digit_sub(x,y) : digit_add(x,y) ) \
                     : (R_DIGIT(DESC(y),sign) == 0  \
                       ? digit_add(x,y) : digit_sub(y,x) ) )

/* ---- Makro zur Klassenbestimmung in numerischen Skalaren: ---- */
#define NUM_CLASS(type)  ((type) == TY_DIGIT ? C_DIGIT : C_SCALAR)

/*---------------------------------------------------------------------------*/
/* Grundsaetzliche Bemerkungen zu den arithmetrischen Operationen:           */
/*   Zwei Argumente koennen verknuepft werden: wenn sie entweder vom         */
/*   Typ Digit sind, oder vom Typ Real und Integer. Digits und Reals bzw.    */
/*   Digits und Integer sind nicht kompatibel.                               */
/*   Falls Integer und Reals miteinander verknuepft werden so entsteht ein   */
/*   Datenobjekt des Typs Real.                                              */
/*   Matrizen, Vektoren, Tvektoren und Scalare bzw. Digits werden            */
/*   elementweise verknuepft. Falls eine Verknuepfung mit einem bezueglich   */
/*   dieser Verknuepfung neutralen Element geschieht, so wird dieses erkannt.*/
/*                                                                           */
/* Grundsaetzlich werden saemmtliche Operationen, welche Felder betreffen,   */
/*   an spezielle Feldroutinen weitergereicht, die die Operation als letzten */
/*   Parameter erhalten.                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* red_plus -- zwei Objekte werden addiert.                                  */
/*                                                                           */
/* Funktionsbeschreibung fuer  +, -, *, /, mod :                             */
/*                            -----------------                              */
/*                                                                           */
/*    Diese Funktionen sind nur auf Zahlen und Matrizen bzw. Vektoren von    */
/*    Zahlen anwendbar. Zahlen koennen mit Matrizen bzw. Vektoren von Zahlen */
/*    verknuepft werden. Es entsteht eine neue Matrix bzw. Vektor dessen     */
/*    Elemente aus der Verknuepfung der Zahl mit den einzelnen Komponenten   */
/*    hervorgehen. Werden zwei Matrizen bzw. Vektoren verknuepft, so muessen */
/*    beide die gleiche Zeilen- und Spaltenzahl haben. Die Modulo-           */
/*    Funktion ist auch auf Digit-Zahlen anwendbar. Ist der Parameter        */
/*    varformat gesetzt, werden alle Operationen, bis auf die Division       */
/*    und die Moduloberechnung exakt ausgefuehrt.                            */
/*    Bei fixformat erfolgt die Berechnung im Rahmen der                     */
/*    Integer- bzw. Realarithmetik (siehe auch Decimal).                     */
/*                                                                           */
/* Beispiele:                                                                */
/*                  ( 1.5 / 6)                  => 0.25                      */
/*            ( vect<1,2,~1> + 1 )              => vect<2,3,0>               */
/*            ( tvect<1,2,~1> + 1 )             => tvect<2,3,0>              */
/*           ( 4 - mat<1,2>,<3,4>. )            => mat<3,2>,<1,0>.           */
/*    ( mat<1,2>,<3,4>. * mat<1,2>,<3,4>. )     => mat<1,4>,<9,16>.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int red_plus(arg1,arg2)            /* int eingefuegt RS 30/10/92 */
register STACKELEM arg1,arg2;
{
/*int i1,i2;              RS 30/10/92 */
  double r1/*,r2*/;    /* RS 30/10/92 */

  DBUG_ENTER ("red_plus");

  START_MODUL("red_plus");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK ueberall auskommentiert RS 3.12.1992 */

  if (T_INT(arg1))
     { 								/* INT + ? */

#ifdef ARITHCHECK
 								printf("/* INT + ? */\n");
#endif /* ARITHCHECK */

      if (T_INT(arg2))
	 { 							/* INT + INT */

#ifdef ARITHCHECK
 								printf("/* INT + INT */\n");
#endif /* ARITHCHECK */

	  _desc = (PTR_DESCRIPTOR) ADD_INT(arg1,arg2) ;
    	  END_MODUL("red_plus");
          DBUG_RETURN(1) ;
         }
	 else
	 { 							/* INT + ~INT */

#ifdef ARITHCHECK
 								printf("/* INT + ~INT */\n");
#endif /* ARITHCHECK */

          if (T_POINTER(arg2))
	     { 							/* INT + DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* INT + DESCRIPTOR */\n");
#endif /* ARITHCHECK */

	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT + SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* INT + SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT + REAL */

#ifdef ARITHCHECK
 								printf("/* INT + REAL */\n");
#endif /* ARITHCHECK */

		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = VAL_INT(arg1) + R_SCALAR(DESC(arg2),valr) ;
                      goto scalar_success_int1 ;
                     }
                     else
                     { 						/* INT + UNDEF */

#ifdef ARITHCHECK
 								printf("/* INT + UNDEF */\n");
#endif /* ARITHCHECK */

                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT + ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* INT + ~SCALAR */\n");
#endif /* ARITHCHECK */

                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {					/* INT + MVT */
		             int help ;

#ifdef ARITHCHECK
 								printf("/* INT + MVT */\n");
#endif /* ARITHCHECK */

		             help = VAL_INT(arg1) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1);
#else
		             arg1 = (STACKELEM)newdesc(); TEST(arg1);
#endif
		             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;

                            }
			    break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {
              goto fail ;
             }
	 } 
     }
     else
     { /* ~INT + ? */ 

#ifdef ARITHCHECK
 								printf("/* ~INT + ? */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR + ? */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR + ? */\n");
#endif /* ARITHCHECK */
 
          if (R_DESC(DESC(arg1),class) == C_SCALAR)
	     {	 						/* SCALAR-DESC + ? */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR + ? */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL + ? */

#ifdef ARITHCHECK
 								printf("/* REAL + ? */\n");
#endif /* ARITHCHECK */

                  r1 = R_SCALAR(DESC(arg1),valr) ;

                  if (T_INT(arg2))
                     {						/* REAL + INT */

#ifdef ARITHCHECK
 								printf("/* REAL + INT */\n");
#endif /* ARITHCHECK */

		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = r1 + VAL_INT(arg2) ;
                      goto scalar_success_int2 ;
                     }
                     else
		     { 						/* REAL + ~INT */

#ifdef ARITHCHECK
 								printf("/* REAL + ~INT */\n");
#endif /* ARITHCHECK */

                      if ( T_POINTER(arg2) )
			 {					/* REAL + DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL + DESCRIPTOR */\n");
#endif /* ARITHCHECK */

			  if ( R_DESC(DESC(arg2),class) == C_SCALAR )
			     {					/* REAL + SCALAR-DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL + SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

                              if (R_DESC(DESC(arg2),type) == TY_REAL)
                                 {				/* REAL + REAL */

#ifdef ARITHCHECK
 								printf("/* REAL + REAL */\n");
#endif /* ARITHCHECK */

                                  NEWDESC(_desc); TEST_DESC;
                                  DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                                  L_SCALAR((*_desc),valr) = r1 + R_SCALAR(DESC(arg2),valr) ;
                                  goto scalar_success;
                                 }
			     }
			 }
		     }
	         }
		 else
                 { 						/* UNDEF + ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF + ? */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 							/* ~SCALAR + ? */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR + ? */\n");
#endif /* ARITHCHECK */

	      if (T_INT(arg2))
		 { 						/* ~SCALAR + INT */
		  int help ;

#ifdef ARITHCHECK
 								printf("/* ~SCALAR + INT */\n");
#endif /* ARITHCHECK */

		  help = VAL_INT(arg2) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2);
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2);
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF + ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF + ? */\n");
#endif /* ARITHCHECK */

	  goto fail ;
         }
     }

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
	   {
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if ((_desc = scal_mvt_op(arg1,arg2,OP_ADD)))
                      goto success;
                    else
                      goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  TEST(_desc = DI_ADD(arg1,arg2));
                  goto success;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (digit_eq(_dig0,arg1))
                    goto res_arg2;
                  if ((_desc = digit_mvt_op(arg1,arg2,OP_ADD)))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if ((_desc = scal_mvt_op(arg2,arg1,OP_ADD)))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig0,arg2))
                    goto res_arg1;
                  if ((_desc = digit_mvt_op(arg2,arg1,OP_ADD)))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if ((_desc = mvt_mvt_op(arg1,arg2,OP_ADD)))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }




fail:
    END_MODUL("red_plus");
    DBUG_RETURN(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
/* res_arg1_int:                            RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_plus");
    
res_arg2:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
/* res_arg2_int:                            RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_plus");
    DBUG_RETURN(1);
success:
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_plus");
    DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_plus");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_plus");
    DBUG_RETURN(1);
}

/*---------------------------------------------------------------------------
 * red_minus -- zwei Datenobjekte werden voneinander subtrahiert.
 *              Die Aufgabe der Funktion wird bei red_plus erlaeutert.
 *---------------------------------------------------------------------------
 */
int red_minus(arg1,arg2)  /* int eingefuegt RS 30/10/92 */
register STACKELEM arg1,arg2;
{
/*int i1,i2;     RS 30/10/92 */
  double r1/*,r2*/;  /* RS 30/10/92 */

  DBUG_ENTER ("red_minus");

  START_MODUL("red_minus");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT - ? */

#ifdef ARITHCHECK
 								printf("/* INT - ? */\n");
#endif /* ARITHCHECK */

      if (T_INT(arg2))
	 { 							/* INT - INT */

#ifdef ARITHCHECK
 								printf("/* INT - INT */\n");
#endif /* ARITHCHECK */

	  _desc = (PTR_DESCRIPTOR) SUB_INT(arg1,arg2) ;
    	  END_MODUL("red_minus");
          DBUG_RETURN(1) ;
         }
	 else
	 { 							/* INT - ~INT */

#ifdef ARITHCHECK
 								printf("/* INT - ~INT */\n");
#endif /* ARITHCHECK */

          if (T_POINTER(arg2))
	     { 							/* INT - DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* INT - DESCRIPTOR */\n");
#endif /* ARITHCHECK */

	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT - SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* INT - SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT - REAL */

#ifdef ARITHCHECK
 								printf("/* INT - REAL */\n");
#endif /* ARITHCHECK */


		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = VAL_INT(arg1) - R_SCALAR(DESC(arg2),valr) ;
                      goto scalar_success_int1 ;
                     }
                     else
                     { 						/* INT - UNDEF */

#ifdef ARITHCHECK
 								printf("/* INT - UNDEF */\n");
#endif /* ARITHCHECK */

                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT - ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* INT - ~SCALAR */\n");
#endif /* ARITHCHECK */

                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {						/* INT - MVT */
		             int help ;

#ifdef ARITHCHECK
 								printf("/* INT - MVT */\n");
#endif /* ARITHCHECK */

                             help = VAL_INT(arg1) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1);
#else
		             arg1 = (STACKELEM)newdesc(); TEST(arg1);
#endif
		             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;
                            }
                            break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {
              goto fail ;
             }
	 } 
     }
     else
     { /* ~INT - ? */ 

#ifdef ARITHCHECK
 								printf("/* ~INT - ? */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR - ? */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR - ? */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC - ? */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR - ? */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL - ? */

#ifdef ARITHCHECK
 								printf("/* REAL - ? */\n");
#endif /* ARITHCHECK */

	          r1 = R_SCALAR(DESC(arg1),valr) ;

                  if (T_INT(arg2))
                     {						/* REAL - INT */

#ifdef ARITHCHECK
 								printf("/* REAL - INT */\n");
#endif /* ARITHCHECK */

		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = r1 - VAL_INT(arg2) ;
                      goto scalar_success_int2 ;
                     }
                     else
		     { 						/* REAL - ~INT */

#ifdef ARITHCHECK
 								printf("/* REAL - ~INT */\n");
#endif /* ARITHCHECK */

                      if ( T_POINTER(arg2) )
			 {					/* REAL - DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL - DESCRIPTOR */\n");
#endif /* ARITHCHECK */

			  if ( R_DESC(DESC(arg2),class) == C_SCALAR )
			     {					/* REAL - SCALAR-DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL - SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

                              if (R_DESC(DESC(arg2),type) == TY_REAL)
                                 {				/* REAL - REAL */

#ifdef ARITHCHECK
 								printf("/* REAL - REAL */\n");
#endif /* ARITHCHECK */

                                  NEWDESC(_desc); TEST_DESC;
                                  DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                                  L_SCALAR((*_desc),valr) = r1 - R_SCALAR(DESC(arg2),valr) ;
                                  goto scalar_success;
                                 }
			     }
			 }
		     }
	         }
		 else
                 { 						/* UNDEF - ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF - ? */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                                	/* ~SCALAR - ? */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR - ? */\n");
#endif /* ARITHCHECK */

	      if (T_INT(arg2))
		 { 						/* ~SCALAR - INT */
		  int help ;

#ifdef ARITHCHECK
 								printf("/* ~SCALAR - INT */\n");
#endif /* ARITHCHECK */

		  help = VAL_INT(arg2) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2);
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2);
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF - ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF - ? */\n");
#endif /* ARITHCHECK */

	  goto fail ;
         }
     }

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if ((_desc = scal_mvt_op(arg1,arg2,OP_SUB)))
                      goto success;
                    else
                      goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  TEST(_desc = DI_SUB(arg1,arg2));
                  goto success;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if ((_desc = digit_mvt_op(arg1,arg2,OP_SUB)))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if ((_desc = scal_mvt_op(arg2,arg1,OP_SUBI)))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig0,arg2))
                    goto res_arg1;
                  if ((_desc = digit_mvt_op(arg2,arg1,OP_SUBI)))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if ((_desc = mvt_mvt_op(arg1,arg2,OP_SUB)))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }




fail:
    END_MODUL("red_minus");
    DBUG_RETURN(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
/* res_arg1_int:                             RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_minus");
    DBUG_RETURN(1);
/* res_arg2:                                 RS 30/10/92 */
 /* DEC_REFCNT((PTR_DESCRIPTOR)arg1);  */          /* RS 04/11/92 */ 
/*
res_arg2_int:                                RS 30/10/92 */
/*  _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_minus");
    DBUG_RETURN(1);     RS 04/11/92 */ 
success:
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_minus");
    DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_minus");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_minus");
    DBUG_RETURN(1);
}

/*---------------------------------------------------------------------------
 * red_mult -- zwei Datenobjekte werden multipliziert.
 *              Die Aufgabe der Funktion wird bei red_plus erlaeutert.
 *---------------------------------------------------------------------------
 */
int red_mult(arg1,arg2)               /* int eingefuegt RS 30/10/92 */
register STACKELEM arg1,arg2;
{
/*int i1,i2;
  double r1,r2;               RS 30/10/92 */

  DBUG_ENTER ("red_mult");

  START_MODUL("red_mult");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT * ? */

#ifdef ARITHCHECK
 								printf("/* INT * ? */\n");
#endif /* ARITHCHECK */

      if (T_INT(arg2))
	 { 							/* INT * INT */

#ifdef ARITHCHECK
 								printf("/* INT * INT */\n");
#endif /* ARITHCHECK */

	  _desc = (PTR_DESCRIPTOR) MUL_INT(arg1,arg2) ;
    	  END_MODUL("red_mult");
          DBUG_RETURN(1) ;
         }
	 else
	 { 							/* INT * ~INT */

#ifdef ARITHCHECK
 								printf("/* INT * ~INT */\n");
#endif /* ARITHCHECK */

          if (T_POINTER(arg2))
	     { 							/* INT * DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* INT * DESCRIPTOR */\n");
#endif /* ARITHCHECK */

	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT * SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* INT * SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT * REAL */

#ifdef ARITHCHECK
 								printf("/* INT * REAL */\n");
#endif /* ARITHCHECK */

		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = VAL_INT(arg1) * R_SCALAR(DESC(arg2),valr) ;
                      goto scalar_success_int1 ;
                     }
                     else
                     { 						/* INT * UNDEF */

#ifdef ARITHCHECK
 								printf("/* INT * UNDEF */\n");
#endif /* ARITHCHECK */

                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT * ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* INT * ~SCALAR */\n");
#endif /* ARITHCHECK */

                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {					/* INT * MVT */
		             int help ;

#ifdef ARITHCHECK
 								printf("/* INT * MVT */\n");
#endif /* ARITHCHECK */

		             help = VAL_INT(arg1) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1);
#else
		             arg1 = (STACKELEM)newdesc(); TEST(arg1);
#endif
		             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;
                            }
                            break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {
              goto fail ;
             }
	 } 
     }
     else
     { /* ~INT * ? */ 

#ifdef ARITHCHECK
 								printf("/* ~INT * ? */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR * ? */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR * ? */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC * ? */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR * ? */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL * ? */

#ifdef ARITHCHECK
 								printf("/* REAL * ? */\n");
#endif /* ARITHCHECK */

                  if (T_INT(arg2))
                     {						/* REAL * INT */

#ifdef ARITHCHECK
 								printf("/* REAL * INT */\n");
#endif /* ARITHCHECK */

		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) * VAL_INT(arg2) ;
                      goto scalar_success_int2 ;
                     }
                     else
		     { 						/* REAL * ~INT */

#ifdef ARITHCHECK
 								printf("/* REAL * ~INT */\n");
#endif /* ARITHCHECK */

                      if ( T_POINTER(arg2) )
			 {					/* REAL * DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL * DESCRIPTOR */\n");
#endif /* ARITHCHECK */

			  if ( R_DESC(DESC(arg2),class) == C_SCALAR )
			     {					/* REAL * SCALAR-DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL * SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

                              if (R_DESC(DESC(arg2),type) == TY_REAL)
                                 {				/* REAL * REAL */

#ifdef ARITHCHECK
 								printf("/* REAL * REAL */\n");
#endif /* ARITHCHECK */

                                  NEWDESC(_desc); TEST_DESC;
                                  DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                                  L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) * R_SCALAR(DESC(arg2),valr) ;
                                  goto scalar_success;
                                 }
			     }
			 }
		     }
	         }
		 else
                 { 						/* UNDEF * ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF * ? */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                                        /* ~SCALAR * ? */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR * ? */\n");
#endif /* ARITHCHECK */

	      if (T_INT(arg2))
		 { 						/* ~SCALAR * INT */
		  int help ;

#ifdef ARITHCHECK
 								printf("/* ~SCALAR * INT */\n");
#endif /* ARITHCHECK */

		  help = VAL_INT(arg2) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2);
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2);
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF * ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF * ? */\n");
#endif /* ARITHCHECK */

	  goto fail ;
         }
     }

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if ((_desc = scal_mvt_op(arg1,arg2,OP_MULT)))
                      goto success;
                    else
                      goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  TEST(_desc = digit_mul(arg1,arg2));
                  goto success;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (digit_eq(_dig1,arg1))
                    goto res_arg2;
                  if ((_desc = digit_mvt_op(arg1,arg2,OP_MULT)))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if ((_desc = scal_mvt_op(arg2,arg1,OP_MULT)))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig1,arg2))
                    goto res_arg1;
                  if ((_desc = digit_mvt_op(arg2,arg1,OP_MULT)))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if ((_desc = mvt_mvt_op(arg1,arg2,OP_MULT)))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }




fail:
    END_MODUL("red_mult");
    DBUG_RETURN(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
/*res_arg1_int:                               RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_mult");
    DBUG_RETURN(1);
res_arg2:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
/* res_arg2_int:                              RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_mult");
    DBUG_RETURN(1);
success:
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_mult");
    DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_mult");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_mult");
    DBUG_RETURN(1);
}

/*---------------------------------------------------------------------------
 * red_div -- zwei Datenobjekte werden dividiert. Dabei ist zu beachten,
 *            dass das Ergebnis einer Division zweier ganzzahliger Zahlen
 *            in einer nicht ganzzahligem Zahlenformat abgelegt wird.
 *            Will man z.B. den Ganzzahligen Anteil von int3 / int2 haben,
 *            so ist anschliessend die Operation trunc anzuwenden.
 *            Der Grund fuer die Abweichung vom C-Standard liegt darin, dass
 *            digit 3 / digit2 = digit 1.5 ergibt.
 *            Ansonsten wird die Aufgabe der Funktion bei red_plus erlaeutert.
 *---------------------------------------------------------------------------
 */
int red_div(arg1,arg2)                 /* int eingefuegt von RS 30/10/92 */
register STACKELEM arg1,arg2;
{
/*  int i1,i2;                 RS 30/10/92 */
  double /*r1,*/ r2;        /* RS 30/10/92 */

  DBUG_ENTER ("red_div");

  START_MODUL("red_div");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT / ? */

#ifdef ARITHCHECK
 								printf("/* INT / ? */\n");
#endif /* ARITHCHECK */

      if (T_INT(arg2))
	 { 							/* INT / INT */

#ifdef ARITHCHECK
 								printf("/* INT / INT */\n");
#endif /* ARITHCHECK */
          if (T_ZERO(arg2))
             {
              goto fail ;
             }
	  NEWDESC(_desc); TEST_DESC ;
          DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
          L_SCALAR((*_desc),valr) = DIV_INT(arg1,arg2) ;
    	  END_MODUL("red_div");
          DBUG_RETURN(1) ;
         }
	 else
	 { 							/* INT / ~INT */

#ifdef ARITHCHECK
 								printf("/* INT / ~INT */\n");
#endif /* ARITHCHECK */

          if (T_POINTER(arg2))
	     { 							/* INT / DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* INT / DESCRIPTOR */\n");
#endif /* ARITHCHECK */

	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT / SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* INT / SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT / REAL */

#ifdef ARITHCHECK
 								printf("/* INT / REAL */\n");
#endif /* ARITHCHECK */

                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0 )
                         { 					/* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
 								printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

			  goto fail ;
                         }
		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = VAL_INT(arg1) / r2 ;
                      goto scalar_success_int1 ;
                     }
                     else
                     { 						/* INT / UNDEF */

#ifdef ARITHCHECK
 								printf("/* INT / UNDEF */\n");
#endif /* ARITHCHECK */

                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT / ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* INT / ~SCALAR */\n");
#endif /* ARITHCHECK */

                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {					/* INT / MVT */
		             int help ;

#ifdef ARITHCHECK
 								printf("/* INT / MVT */\n");
#endif /* ARITHCHECK */

/* CHANGED arg2 to arg1 by RS in June 1993 */
/* why should arg2 be a INT if the switch matches ? */

		             help = VAL_INT(arg1) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1);
#else
		             arg1 = (STACKELEM)newdesc(); TEST(arg1);
#endif
		             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;
     DBUG_PRINT ("red_div", ("Here we are !!"));
                            }
                            break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {
              goto fail ;
             }
	 } 
     }
     else
     { /* ~INT / ? */ 

#ifdef ARITHCHECK
 								printf("/* ~INT / ? */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR / ? */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR / ? */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC / ? */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR / ? */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL / ? */

#ifdef ARITHCHECK
 								printf("/* REAL / ? */\n");
#endif /* ARITHCHECK */

                  if (T_INT(arg2))
                     {						/* REAL / INT */

#ifdef ARITHCHECK
 								printf("/* REAL / INT */\n");
#endif /* ARITHCHECK */

                      if (T_ZERO(arg2))
                         { 					/* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
 								printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

			  goto fail ;
                         }

		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) / VAL_INT(arg2) ;
                      goto scalar_success_int2 ;
                     }
                     else
		     { 						/* REAL / ~INT */

#ifdef ARITHCHECK
 								printf("/* REAL / ~INT */\n");
#endif /* ARITHCHECK */

                      if ( T_POINTER(arg2) )
			 {					/* REAL / DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL / DESCRIPTOR */\n");
#endif /* ARITHCHECK */

			  if ( R_DESC(DESC(arg2),class) == C_SCALAR )
			     {					/* REAL / SCALAR-DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL / SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

                              if (R_DESC(DESC(arg2),type) == TY_REAL)
                                 {				/* REAL / REAL */

#ifdef ARITHCHECK
 								printf("/* REAL / REAL */\n");
#endif /* ARITHCHECK */

                                  if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                                     { 				/* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
 								printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

			              goto fail ;
                                     }

                                  NEWDESC(_desc); TEST_DESC;
                                  DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                                  L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) / r2 ;
                                  goto scalar_success;
                                 }
			     }
			 }
		     }
	         }
		 else
                 { 						/* UNDEF / ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF / ? */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                               		/* ~SCALAR / ? */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR / ? */\n");
#endif /* ARITHCHECK */

	      if (T_INT(arg2))
		 { 						/* ~SCALAR / INT */
		  int help ;

#ifdef ARITHCHECK
 								printf("/* ~SCALAR / INT */\n");
#endif /* ARITHCHECK */

		  help = VAL_INT(arg2) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2);
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2);
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF / ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF / ? */\n");
#endif /* ARITHCHECK */

	  goto fail ;
         }
     }

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if ((_desc = scal_mvt_op(arg1,arg2,OP_DIV)))
                      goto success;
                    else
                      goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  if ((_desc = digit_div(arg1,arg2)))
                    goto success;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (digit_eq(_dig1,arg1))
                    goto res_arg2;
                  if ((_desc = digit_mvt_op(arg1,arg2,OP_DIV)))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if ((_desc = scal_mvt_op(arg2,arg1,OP_DIVI)))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig1,arg2))
                    goto res_arg1;
                  if ((_desc = digit_mvt_op(arg2,arg1,OP_DIVI)))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if ((_desc = mvt_mvt_op(arg1,arg2,OP_DIV)))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }




fail:
    END_MODUL("red_div");
    DBUG_RETURN(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
/* res_arg1_int:                          RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_div");
    DBUG_RETURN(1);
res_arg2:
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
/* res_arg2_int:                          RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_div");
    DBUG_RETURN(1);
success:
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_div");
    DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_div");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_div");
    DBUG_RETURN(1);
}

/*---------------------------------------------------------------------------
 * red_quot -- zwei Datenobjekte werden dividiert. Dabei ist zu beachten,
 *            dass das Ergebnis einer Division zweier ganzzahliger Zahlen
 *            in einem ganzzahligem Zahlenformat abgelegt wird.
 *            Ansonsten wird die Aufgabe der Funktion bei red_plus erlaeutert.
 *---------------------------------------------------------------------------
 */
int red_quot(arg1,arg2)                 /* int eingefuegt von RS 30/10/92 */
register STACKELEM arg1,arg2;
{
/*  int i1,i2;                 RS 30/10/92 */
 /*  double r1, r2; */        /* RS 30/10/92 & 28/10/93 */

  int save_prec = _prec;

  DBUG_ENTER ("red_quot");

  START_MODUL("red_quot");

#ifdef ARITHCHECK
                                                                printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     {                                                          /* INT / ? */

#ifdef ARITHCHECK
                                                                printf("/* INT / ? */\n");
#endif /* ARITHCHECK */

      if (T_INT(arg2))
         {                                                      /* INT / INT */

#ifdef ARITHCHECK
                                                                printf("/* INT / INT */\n");
#endif /* ARITHCHECK */
          if (T_ZERO(arg2))
             {
              goto fail ;
             }
          _desc = (PTR_DESCRIPTOR) TAG_INT(VAL_INT(arg1) / VAL_INT(arg2));
/*
          NEWDESC(_desc); TEST_DESC ;
          DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
          L_SCALAR((*_desc),valr) = DIV_INT(arg1,arg2) ; */
          END_MODUL("red_quot");
          DBUG_RETURN(1) ;
         }
         else
         {                                                      /* INT / ~INT */

         goto fail;   /* no data objects supported other than int's */

#ifdef NOT_IMPLEMENTED_FOR_QUOT

#ifdef ARITHCHECK
                                                                printf("/* INT / ~INT */\n");
#endif /* ARITHCHECK */

          if (T_POINTER(arg2))
             {                                                  /* INT / DESCRIPTOR */

#ifdef ARITHCHECK
                                                                printf("/* INT / DESCRIPTOR */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg2),class) == C_SCALAR)
                 {                                              /* INT / SCALAR-DESC */

#ifdef ARITHCHECK
                                                                printf("/* INT / SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

                  if (R_DESC(DESC(arg2),type) == TY_REAL)
                     {                                          /* INT / REAL */

#ifdef ARITHCHECK
                                                                printf("/* INT / REAL */\n");
#endif /* ARITHCHECK */

                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0 )
                         {                                      /* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
                                                                printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

                          goto fail ;
                         }
                      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = VAL_INT(arg1) / r2 ;
                      goto scalar_success_int1 ;
                     }
                     else
                     {                                          /* INT / UNDEF */

#ifdef ARITHCHECK
                                                                printf("/* INT / UNDEF */\n");
#endif /* ARITHCHECK */

                      goto fail ;
                     }
                 }
                 else
                 {                                              /* INT / ~SCALAR */

                 goto fail;

#ifdef ARITHCHECK
                                                                printf("/* INT / ~SCALAR */\n");
#endif /* ARITHCHECK */

                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {                                   /* INT / MVT */
                             int help ;

#ifdef ARITHCHECK
                                                                printf("/* INT / MVT */\n");
#endif /* ARITHCHECK */

/* CHANGED arg2 to arg1 by RS in June 1993 */
/* why should arg2 be a INT if the switch matches ? */

                             help = VAL_INT(arg1) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1);
#else
                             arg1 = (STACKELEM)newdesc(); TEST(arg1);
#endif
                             DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
                             L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;
     DBUG_PRINT ("red_quot", ("Here we are !!"));
                            }
                            break ;
                      default: goto fail;
                     }
                 }
             }
             else
             {
              goto fail ;
             }
#endif /* NOT_IMPLEMENTED_FOR_QUOT */
         }
     }
     else
     { /* ~INT / ? */

#ifdef ARITHCHECK
                                                                printf("/* ~INT / ? */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         {                                                      /* DESCRIPTOR / ? */

#ifdef ARITHCHECK
                                                                printf("/* DESCRIPTOR / ? */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {                                                  /* SCALAR-DESC / ? */

             goto fail;  /* no data objects supported other than int's */

#ifdef NOT_IMPLEMENTED_FOR_QUOT

#ifdef ARITHCHECK
                                                                printf("/* SCALAR-DESCRIPTOR / ? */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
                 {                                              /* REAL / ? */

                  goto fail;  /* no data objects supported other than int's */

#ifdef ARITHCHECK
                                                                printf("/* REAL / ? */\n");
#endif /* ARITHCHECK */

                  if (T_INT(arg2))
                     {                                          /* REAL / INT */

#ifdef ARITHCHECK
                                                                printf("/* REAL / INT */\n");
#endif /* ARITHCHECK */

                      if (T_ZERO(arg2))
                         {                                      /* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
                                                                printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

                          goto fail ;
                         }

                      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) / VAL_INT(arg2) ; 
                      goto scalar_success_int2 ;
                     }
                     else
                     {                                          /* REAL / ~INT */

#ifdef ARITHCHECK
                                                                printf("/* REAL / ~INT */\n");
#endif /* ARITHCHECK */

                      if ( T_POINTER(arg2) )
                         {                                      /* REAL / DESCRIPTOR */

#ifdef ARITHCHECK
                                                                printf("/* REAL / DESCRIPTOR */\n");
#endif /* ARITHCHECK */

                          if ( R_DESC(DESC(arg2),class) == C_SCALAR )
                             {                                  /* REAL / SCALAR-DESCRIPTOR */

#ifdef ARITHCHECK
                                                                printf("/* REAL / SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

                              if (R_DESC(DESC(arg2),type) == TY_REAL)
                                 {                              /* REAL / REAL */

#ifdef ARITHCHECK
                                                                printf("/* REAL / REAL */\n");
#endif /* ARITHCHECK */

                                  if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0)
                                     {                          /* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
                                                                printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

                                      goto fail ;
                                     }

                                  NEWDESC(_desc); TEST_DESC;
                                  DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                                  L_SCALAR((*_desc),valr) = R_SCALAR(DESC(arg1),valr) / r2 ;
                                  goto scalar_success;
                                 }
                             }
                         }
                     }
                 }
                 else
                 {                                              /* UNDEF / ? */

#ifdef ARITHCHECK
                                                                printf("/* UNDEF / ? */\n");
#endif /* ARITHCHECK */

                  goto fail ;
                 }

#endif /* NOT_IMPLEMENTED_FOR_QUOT */

             }
             else
             {                                                  /* ~SCALAR / ? */

#ifdef ARITHCHECK
                                                                printf("/* ~SCALAR / ? */\n");
#endif /* ARITHCHECK */

              if (T_INT(arg2))
                 {                                              /* ~SCALAR / INT */
                  /* int help */ ;

                  goto fail;  /* no data objects supported other than int's */

#ifdef NOT_IMPLEMENTED_FOR_QUOT

#ifdef ARITHCHECK
                                                                printf("/* ~SCALAR / INT */\n");
#endif /* ARITHCHECK */

                  help = VAL_INT(arg2) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2);
#else
                  arg2 = (STACKELEM)newdesc(); TEST(arg2);
#endif
                  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
                  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;

#endif /* NOT_IMPLEMENTED_FOR_QUOT */

                 }
             }
         }
         else
         {                                                      /* UNDEF / ? */

#ifdef ARITHCHECK
                                                                printf("/* UNDEF / ? */\n");
#endif /* ARITHCHECK */

          goto fail ;
         }
     }

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
       /*      case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if ((_desc = scal_mvt_op(arg1,arg2,OP_DIV)))
                      goto success;
                    else
                      goto fail; */
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
                  if (((R_DIGIT(DESC(arg1),Exp) + 1) == R_DIGIT(DESC(arg1),ndigit)) &&
                      ((R_DIGIT(DESC(arg2),Exp) + 1) == R_DIGIT(DESC(arg2),ndigit))) {
                    _prec = 0;
                  if ((_desc = digit_div(arg1,arg2))) {
                    _prec = save_prec;
                    goto success; }
                  else
                    _prec = save_prec; }
/*
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if (digit_eq(_dig1,arg1))
                    goto res_arg2;
                  if ((_desc = digit_mvt_op(arg1,arg2,OP_DIV)))
                    goto success;
                  goto fail;  */
             default:
                  goto fail;
           }
/*    case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if ((_desc = scal_mvt_op(arg2,arg1,OP_DIVI)))
                    goto success;
                  goto fail;
             case C_DIGIT:
                  if (digit_eq(_dig1,arg2))
                    goto res_arg1;
                  if ((_desc = digit_mvt_op(arg2,arg1,OP_DIVI)))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if ((_desc = mvt_mvt_op(arg1,arg2,OP_DIV)))
                    goto success;
                  goto fail;
             default: goto fail;
           }  */
      default: goto fail;
    }
  }




fail:
    END_MODUL("red_quot");
    DBUG_RETURN(0);
/* res_arg1: */

#ifdef NOT_IMPLEMENTED_FOR_QUOT

    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
/* res_arg1_int:                          RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_quot");
    DBUG_RETURN(1);
/* res_arg2: */
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
/* res_arg2_int:                          RS 30/10/92 */
    _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_quot");
    DBUG_RETURN(1);

#endif /* NOT_IMPLEMENTED_FOR_QUOT */

/* scalar_success: */
success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_quot");
    DBUG_RETURN(1);
/* scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_quot");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_quot");
    DBUG_RETURN(1); */
}


/*---------------------------------------------------------------------------*/
/* red_mod -- berechnet den Modulo zweier Datenobjekte vom Typ Integer und   */
/*            vom Typ Digit.                                                 */
/*            Die Aufgabe der Funktion wird bei red_plus erlaeutert.         */
/*---------------------------------------------------------------------------*/
/* corrected for real args car 3.05.95 */
int red_mod(arg1,arg2)               /* int eingefuegt von RS 30/10/92 */
register STACKELEM arg1,arg2;
{
  double r1,r2=(double)0,help ; /* Initialisierung von TB, 6.11.1992 */

  DBUG_ENTER ("red_mod");

  START_MODUL("red_mod");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT modulo ? */

#ifdef ARITHCHECK
 								printf("/* INT modulo ? */\n");
#endif /* ARITHCHECK */

      if (T_INT(arg2))
	 { 							/* INT modulo INT */

#ifdef ARITHCHECK
 								printf("/* INT modulo INT */\n");
#endif /* ARITHCHECK */

	  if ( T_ZERO(arg2) )
	     {							/* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
 								printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

	      goto fail ;
	     }

	  _desc = (PTR_DESCRIPTOR) MOD_INT(arg1,arg2) ;
    	  END_MODUL("red_mod");
          DBUG_RETURN(1) ;
         }
	 else
	 { 							/* INT modulo ~INT */

#ifdef ARITHCHECK
 								printf("/* INT modulo ~INT */\n");
#endif /* ARITHCHECK */

          if (T_POINTER(arg2))
	     { 							/* INT modulo DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* INT modulo DESCRIPTOR */\n");
#endif /* ARITHCHECK */

	      if (R_DESC(DESC(arg2),class) == C_SCALAR)
		 { 						/* INT modulo SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* INT modulo SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

		  if (R_DESC(DESC(arg2),type) == TY_REAL)
		     { 						/* INT modulo REAL */

#ifdef ARITHCHECK
 								printf("/* INT modulo REAL */\n");
#endif /* ARITHCHECK */

                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0 )
                         { 					/* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
 								printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

			  goto fail ;
                         }
		      help  = (double)VAL_INT(arg1) ;
		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = help - (floor(help/r2))*r2 ;
                      goto scalar_success_int1 ;

                     }
                     else
                     { 						/* INT modulo UNDEF */

#ifdef ARITHCHECK
 								printf("/* INT modulo UNDEF */\n");
#endif /* ARITHCHECK */

                      goto fail ;
		     }
                 }
                 else
                 { 						/* INT modulo ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* INT modulo ~SCALAR */\n");
#endif /* ARITHCHECK */

                  switch(R_DESC(DESC(arg2),class))
                     {
                      case C_MATRIX:
                      case C_VECTOR:
                      case C_TVECTOR:
                             /* Zu ersten Testzwecken wird erstmal ein Descriptor gebastelt */
                            {					/* INT modulo MVT */
		  	     int help ;

#ifdef ARITHCHECK
 								printf("/* INT modulo MVT */\n");
#endif /* ARITHCHECK */

		  	     help = VAL_INT(arg1) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                             arg1 = (STACKELEM)(*d_m_newdesc)(); TEST(arg1);
#else
		  	     arg1 = (STACKELEM)newdesc(); TEST(arg1);
#endif
		  	     DESC_MASK(arg1,1,C_SCALAR,TY_INTEGER) ;
		  	     L_SCALAR((* (PTR_DESCRIPTOR) arg1),vali) = help ;
                            }
			    break ;
                      default: goto fail;
                     }
                 }
	     }
             else
             {							/* INT modulo UNDEF */
              goto fail ;
             }
	 } 
     }
     else
     { 								/* ~INT modulo ? */ 

#ifdef ARITHCHECK
 								printf("/* ~INT modulo ? */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR modulo ? */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR modulo ? */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC modulo ? */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR modulo ? */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL modulo ? */

#ifdef ARITHCHECK
 								printf("/* REAL modulo ? */\n");
#endif /* ARITHCHECK */

                  r1 = R_SCALAR(DESC(arg1),valr) ;

                  if (T_INT(arg2))
                     {						/* REAL modulo INT */

#ifdef ARITHCHECK
 								printf("/* REAL modulo INT */\n");
#endif /* ARITHCHECK */

                      if (T_ZERO(arg2))
                         { 					/* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
 								printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

			  goto fail ;
                         }

		      help = (double)VAL_INT(arg2) ;
		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = r1 - (floor(r1/help)*help) ;
                      goto scalar_success_int2 ;

                     }
                     else
		     { 						/* REAL modulo ~INT */

#ifdef ARITHCHECK
 								printf("/* REAL modulo ~INT */\n");
#endif /* ARITHCHECK */

                      if ( T_POINTER(arg2) )
			 {					/* REAL modulo DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL modulo DESCRIPTOR */\n");
#endif /* ARITHCHECK */

			  if ( R_DESC(DESC(arg2),class) == C_SCALAR )
			     {					/* REAL modulo SCALAR-DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* REAL modulo SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

                              if (R_DESC(DESC(arg2),type) == TY_REAL)
                                 { 					/* REAL modulo REAL */

#ifdef ARITHCHECK
 								printf("/* REAL modulo REAL */\n");
#endif /* ARITHCHECK */

                      if ((r2 = R_SCALAR(DESC(arg2),valr)) == 0.0 )
                         { 					/* arg2 == 0 ==> Fehler */

#ifdef ARITHCHECK
 								printf("/* arg2 == 0 ==> Fehler */\n");
#endif /* ARITHCHECK */

			  goto fail ;
                         }
		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = r1 - floor(r1/r2)*r2 ;
                      goto scalar_success ;
                                 }
                             }
			 }
		     }
	         }
		 else
                 { 						/* UNDEF modulo ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF modulo ? */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                               		/* ~SCALAR modulo ? */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR modulo ? */\n");
#endif /* ARITHCHECK */

	      if (T_INT(arg2))
		 { 						/* ~SCALAR modulo INT */
		  int help ;

#ifdef ARITHCHECK
 								printf("/* ~SCALAR modulo INT */\n");
#endif /* ARITHCHECK */

		  help = VAL_INT(arg2) ;
#if (D_SLAVE && D_MESS && D_MHEAP)
                  arg2 = (STACKELEM)(*d_m_newdesc)(); TEST(arg2);
#else
		  arg2 = (STACKELEM)newdesc(); TEST(arg2);
#endif
		  DESC_MASK(arg2,1,C_SCALAR,TY_INTEGER) ;
		  L_SCALAR((* (PTR_DESCRIPTOR) arg2),vali) = help ;
		 }
	     }
         } 
	 else
	 { 							/* UNDEF modulo ? */

#ifdef ARITHCHECK
 								printf("/* UNDEF modulo ? */\n");
#endif /* ARITHCHECK */

	  goto fail ;
         }
     }

  if (T_POINTER(arg1) && T_POINTER(arg2))
  {
    switch( R_DESC(DESC(arg1),class))
    {
      case C_SCALAR:
           switch( R_DESC(DESC(arg2),class))
           {
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                    if ((_desc = scal_mvt_op(arg1,arg2,OP_MOD)))
                      goto success;
                    else
                      goto fail;
             default: goto fail;
           } /* Ende der Fallunterscheidung bzgl. Class von arg2 */
      case C_DIGIT:
           switch(R_DESC(DESC(arg2),class) )
           {
             case C_DIGIT:
		  if ( digit_eq( _dig0 , arg1 ) )
		     {						/* arg1 == 0 ==> Res = arg1 */
		      goto res_arg1 ;
		     }
		  if ( digit_eq( _dig0 , arg2 ) )
		     {						/* arg1 == 0 ==> Fehler */
		      goto fail ;
		     }
                  if ((_desc = digit_mod(arg1,arg2,OP_MOD)))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
		  if ( digit_eq( _dig0 , arg1 ) )
		     {						/* arg1 == 0 ==> Res = arg1 */
		      goto res_arg1 ;
		     }
                  if ((_desc = digit_mvt_op(arg1,arg2,OP_MOD)))
                    goto success;
                  goto fail;
             default:
                  goto fail;
           }
      case C_MATRIX:
      case C_VECTOR:
      case C_TVECTOR:
           switch(R_DESC(DESC(arg2),class))
           {
             case C_SCALAR:
                  if ((_desc = scal_mvt_op(arg2,arg1,OP_MODI)))
                    goto success;
                  goto fail;
             case C_DIGIT:
		  if ( digit_eq( _dig0 , arg2 ) )
		     {						/* arg1 == 0 ==> Fehler */
		      goto fail ;
		     }
                  if ((_desc = digit_mvt_op(arg2,arg1,OP_MODI)))
                    goto success;
                  goto fail;
             case C_MATRIX:
             case C_VECTOR:
             case C_TVECTOR:
                  if ((_desc = mvt_mvt_op(arg1,arg2,OP_MOD)))
                    goto success;
                  goto fail;
             default: goto fail;
           }
      default: goto fail;
    }
  }




fail:
    END_MODUL("red_mod");
    DBUG_RETURN(0);
res_arg1:
    DEC_REFCNT((PTR_DESCRIPTOR)arg2);
/* res_arg1_int:                       RS 30/10/92 */ 
    _desc = (PTR_DESCRIPTOR) arg1;
    END_MODUL("red_mod");
    DBUG_RETURN(1);
/* res_arg2:                           RS 30/10/92 */ 
/*  DEC_REFCNT((PTR_DESCRIPTOR)arg1);     RS 04/11/92 */ 
/* res_arg2_int:                       RS 30/10/92 */
/*  _desc = (PTR_DESCRIPTOR) arg2;
    END_MODUL("red_mod");
    DBUG_RETURN(1);    RS 04/11/92 */ 
success:
scalar_success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_mod");
    DBUG_RETURN(1);
scalar_success_int1:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg2);
    END_MODUL("red_mod");
    DBUG_RETURN(1);
scalar_success_int2:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_mod");
    DBUG_RETURN(1);
}

/*------------------------------------------------------------------------
 * red_abs --
 *
 * Anwendung:                 abs(argument)
 *
 * Funktion:
 *
 *    abs liefert den Absolutwert von Zahlen,Matrizen und Vektoren.
 *    Der Absolutwert einer Struktur ist eine Struktur. Ihre Komponenten
 *    sind die Absolutwerte der Komponenten der Argumentstruktur.
 *
 *    Beispiele:
 *
 *        abs( 1 )               =>  1
 *        abs( ~2 )              =>  2
 *        abs( vect<1,~2,~3> )   =>  vect<1,2,3>
 *        abs( tvect<1,~2,~3> )  =>  tvect<1,2,3>
 *        abs( mat<1,~2,~3>. )   =>  mat<1,2,3>.
 *
 *------------------------------------------------------------------------
 */
int red_abs(arg1)              /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1;
{
  double r1;

  DBUG_ENTER ("red_abs");

  START_MODUL("red_abs");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT */

#ifdef ARITHCHECK
 								printf("/* INT */\n");
#endif /* ARITHCHECK */

      if ( VAL_INT(arg1) < 0 )
	 {
	  _desc = (PTR_DESCRIPTOR)NEG_INT(arg1) ;
	  goto success ;
	 }
	 else
	 {
	  _desc = (PTR_DESCRIPTOR)arg1 ;
	  goto success ;
	 }

     }
     else
     { /* ~INT */ 

#ifdef ARITHCHECK
 								printf("/* ~INT */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL */

#ifdef ARITHCHECK
 								printf("/* REAL */\n");
#endif /* ARITHCHECK */

		  if (( r1 = R_SCALAR(DESC(arg1),valr) ) < 0.0 )
		     {
		      NEWDESC(_desc); TEST_DESC ;
                      DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                      L_SCALAR((*_desc),valr) = -r1 ;
                      goto success ;
		     }
		     else
		     {
	  	      _desc = (PTR_DESCRIPTOR)arg1 ;
	  	      goto success ;
		     }
	         }
		 else
                 { 						/* UNDEF */

#ifdef ARITHCHECK
 								printf("/* UNDEF */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                               		/* ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR */\n");
#endif /* ARITHCHECK */

    	      switch(R_DESC(DESC(arg1),class))
    	      {
      	       case C_DIGIT:
           	      TEST(_desc = digit_abs(arg1));
           	      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           	      goto success;
      	       case C_MATRIX:
      	       case C_VECTOR:
      	       case C_TVECTOR:
           	      if ((_desc = mvt_op(arg1,OP_ABS)) == 0) goto fail;
           	      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           	      goto success;
      	       default: goto fail;
    	      }
	     }
         }
     }

fail:
    END_MODUL("red_abs");
    DBUG_RETURN(0);
success:
    END_MODUL("red_abs");
    DBUG_RETURN(1);
}

/*------------------------------------------------------------------------
 * red_neg --
 *
 * Anwendung:                 neg(argument)
 *
 * Funktion:
 *
 *    neg ist nur auf Zahlen und Matrizen bzw. Vektoren von Zahlen anwendbar.
 *    Er erzeugt eine Kopie des Argumentes, in der alle Vorzeichen
 *    umgekehrt worden sind.
 *
 *    Beispiele:
 *
 *        neg( 1 )               =>  ~1
 *        neg( ~2 )              =>  2
 *        neg( vect<1,~2,~3> )   =>  vect<~1,2,3>
 *        neg( tvect<1,~2,~3> )  =>  tvect<~1,2,3>
 *        neg( mat<1,~2,~3>. )   =>  mat<~1,2,3>.
 *
 *------------------------------------------------------------------------
 */
int red_neg(arg1)                /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1;
{
/*double r1;               RS 30/10/92 */ 

  DBUG_ENTER ("red_neg");

  START_MODUL("red_neg");
  if (T_INT(arg1))
     { 								/* INT */
       _desc = (PTR_DESCRIPTOR)NEG_INT(arg1) ;
        goto scalar_success_int1 ;
     }
     else
     { /* ~INT */ 
      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR */
          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC */
              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL */
		  NEWDESC(_desc); TEST_DESC ;
                  DESC_MASK(_desc,1,C_SCALAR,TY_REAL) ;
                  L_SCALAR((*_desc),valr) = - R_SCALAR(DESC(arg1),valr) ;
                  goto success ;
	         }
		 else
                 { 						/* UNDEF */
                  goto fail ;
		 }
             }
             else
	     { 		                               		/* ~SCALAR */
    	      switch(R_DESC(DESC(arg1),class))
    	      {
      	       case C_DIGIT:
           	      TEST(_desc = digit_neg(arg1));
           	      /* dg 14.08.92 DAS GIBT EINEN FEHLER!!!  DEC_REFCNT((PTR_DESCRIPTOR)arg1); */
           	      goto success;
      	       case C_MATRIX:
      	       case C_VECTOR:
      	       case C_TVECTOR:
           	      if ((_desc = mvt_op(arg1,OP_NEG)) == 0) goto fail;
           	      goto success;
      	       default: goto fail;
    	      }
	     }
         }
     }

fail:
    END_MODUL("red_neg");
    DBUG_RETURN(0);
success:
    DEC_REFCNT_SCALAR((PTR_DESCRIPTOR)arg1);
    END_MODUL("red_neg");
    DBUG_RETURN(1);
scalar_success_int1:
    END_MODUL("red_neg");
    DBUG_RETURN(1);
}

/*------------------------------------------------------------------------
 * red_trunc --
 *
 * Anwendung:      trunc(argument)  oder  frac(argument)
 *
 * Funktion:
 *
 *    Beide Funktionen sind nur auf Zahlen und Matrizen bzw. Vektoren
 *    von Zahlen definiert.
 *    Die Funktion trunc erzeugt eine Kopie des Argumentes ohne Nach-
 *    kommastellen, waehrend frac eine ohne Vorkommastellen erzeugt.
 *
 * Beispiele:
 *    trunc( 1.23 )           =>  1
 *    trunc( vect<1,2.4> )    =>  vect<1,2>
 *    trunc( tvect<1,2.4> )   =>  tvect<1,2>
 *    trunc( mat<1,2.4>. )    =>  mat<1,2>.
 *     frac( 1.23 )           =>  0.23
 *     frac( vect<1,2.4> )    =>  vect<0,0.4>
 *     frac( tvect<1,2.4> )   =>  tvect<0,0.4>
 *     frac( mat<1,2.4>. )    =>  mat<0,0.4>.
 *
 *------------------------------------------------------------------------
 */
int red_trunc(arg1)                 /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1;
{

  DBUG_ENTER ("red_trunc");

  START_MODUL("red_trunc");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT */

#ifdef ARITHCHECK
 								printf("/* INT */\n");
#endif /* ARITHCHECK */

	_desc = (PTR_DESCRIPTOR)arg1 ;
	goto success ;

     }
     else
     { /* ~INT */ 

#ifdef ARITHCHECK
 								printf("/* ~INT */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL */

#ifdef ARITHCHECK
 								printf("/* REAL */\n");
#endif /* ARITHCHECK */

		  _desc = (PTR_DESCRIPTOR)TAG_INT((int)R_SCALAR(DESC(arg1),valr)) ;
           	  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
                  goto success ;
	         }
		 else
                 { 						/* UNDEF */

#ifdef ARITHCHECK
 								printf("/* UNDEF */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                               		/* ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR */\n");
#endif /* ARITHCHECK */

    	      switch(R_DESC(DESC(arg1),class))
    	      {
      	       case C_DIGIT:
           	      TEST(_desc = digit_trunc(arg1));
           	      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           	      goto success;
      	       case C_MATRIX:
      	       case C_VECTOR:
      	       case C_TVECTOR:
           	      if ((_desc = mvt_op(arg1,OP_TRUNC)) == 0) goto fail;
           	      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           	      goto success;
      	       default: goto fail;
    	      }
	     }
         }
     }

fail:
    END_MODUL("red_trunc");
    DBUG_RETURN(0);
success:
    END_MODUL("red_trunc");
    DBUG_RETURN(1);
}

/*------------------------------------------------------------------------
 * red_frac -- berechnet den faktional Part.
 *             Ihre Funktion wird in red_trunc erlaeutert.
 *------------------------------------------------------------------------
 */
int red_frac(arg1)               /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1;
{
  double r ;

  DBUG_ENTER ("red_frac");

  START_MODUL("red_frac");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT */

#ifdef ARITHCHECK
 								printf("/* INT */\n");
#endif /* ARITHCHECK */

	_desc = (PTR_DESCRIPTOR)ZERO_INT ;
	goto scalar_success_int1 ;

     }
     else
     { /* ~INT */ 

#ifdef ARITHCHECK
 								printf("/* ~INT */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL */

#ifdef ARITHCHECK
 								printf("/* REAL */\n");
#endif /* ARITHCHECK */

                  NEWDESC(_desc); TEST_DESC;
                  DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                  r = R_SCALAR(DESC(arg1),valr);
                  L_SCALAR((*_desc),valr) = r - (int) r;
                  goto success;
	         }
		 else
                 { 						/* UNDEF */

#ifdef ARITHCHECK
 								printf("/* UNDEF */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                               		/* ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR */\n");
#endif /* ARITHCHECK */

    	      switch(R_DESC(DESC(arg1),class))
    	      {
      	       case C_DIGIT:
           	      TEST(_desc = digit_frac(arg1));
           	      goto success;
      	       case C_MATRIX:
      	       case C_VECTOR:
      	       case C_TVECTOR:
           	      if ((_desc = mvt_op(arg1,OP_FRAC)) == 0) goto fail;
           	      goto success;
      	       default: goto fail;
    	      }
	     }
         }
     }

fail:
    END_MODUL("red_frac");
    DBUG_RETURN(0);
scalar_success_int1:
    END_MODUL("red_frac");
    DBUG_RETURN(1);
success:
    END_MODUL("red_frac");
    DEC_REFCNT((PTR_DESCRIPTOR)arg1);
    DBUG_RETURN(1);
}

/*------------------------------------------------------------------------
 * red_floor --
 *
 * Anwendung:    ceil(argument)   oder   floor(argument)
 *
 * Funktion:
 *
 *    Diese beiden Funktionen sind nur auf Zahlen und Matrizen bzw. Vektoren
 *    von Zahlen definiert.
 *    ceil angewendet auf eine Zahl liefert die naechst
 *    groessere ganze Zahl. floor dagegen liefert die naechst kleinere
 *    Zahl. Auf Strukturen werden die Funktionen elementweise angewendet.
 *
 * Beispiele:
 *     ceil( 5.7 )               =>  6
 *     ceil( vect<~0.1,0,0.2> )  => vect<0,0,1>
 *     ceil( tvect<~0.1,0,0.2> ) => tvect<0,0,1>
 *     ceil( mat<~0.1,0,0.2>. )  => mat<0,0,1>.
 *    floor( 5.7 )               =>  5
 *    floor( vect<~0.1,0,0.2> )  => vect<~1,0,0>.
 *    floor( tvect<~0.1,0,0.2> ) => tvect<~1,0,0>.
 *    floor( mat<~0.1,0,0.2>. )  => mat<~1,0,0>.
 *
 *------------------------------------------------------------------------
 */
int red_floor(arg1)                 /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1;
{
  double r ;

  DBUG_ENTER ("red_floor");

  START_MODUL("red_floor");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT */

#ifdef ARITHCHECK
 								printf("/* INT */\n");
#endif /* ARITHCHECK */

	_desc = (PTR_DESCRIPTOR)arg1 ;
	goto success ;

     }
     else
     { /* ~INT */ 

#ifdef ARITHCHECK
 								printf("/* ~INT */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL */

#ifdef ARITHCHECK
 								printf("/* REAL */\n");
#endif /* ARITHCHECK */

                  r = R_SCALAR(DESC(arg1),valr);
		  _desc = (PTR_DESCRIPTOR)TAG_INT((int)( r>=0.0 ? r : r-1 )) ;
    		  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
                  goto success;
	         }
		 else
                 { 						/* UNDEF */

#ifdef ARITHCHECK
 								printf("/* UNDEF */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                               		/* ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR */\n");
#endif /* ARITHCHECK */

    	      switch(R_DESC(DESC(arg1),class))
    	      {
      	       case C_DIGIT:
           	      TEST(_desc = digit_floor(arg1));
    		      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           	      goto success;
      	       case C_MATRIX:
      	       case C_VECTOR:
      	       case C_TVECTOR:
           	      if ((_desc = mvt_op(arg1,OP_FLOOR)) == 0) goto fail;
    		      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           	      goto success;
      	       default: goto fail;
    	      }
	     }
         }
     }

fail:
    END_MODUL("red_floor");
    DBUG_RETURN(0);
success:
    END_MODUL("red_floor");
    DBUG_RETURN(1);
}

/*------------------------------------------------------------------------
 * red_ceil -- berechnet die zu einer Zahl gehoerige naechst groessere
 *             ganze Zahl.
 *             Ihre Funktion wird bei red_floor erlaeutert.
 *------------------------------------------------------------------------
 */
int red_ceil(arg1)                   /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1;
{
  int i ;
  double r ;

  DBUG_ENTER ("red_ceil");

  START_MODUL("red_ceil");

#ifdef ARITHCHECK
 								printf("\n");
#endif /* ARITHCHECK */

  if (T_INT(arg1))
     { 								/* INT */

#ifdef ARITHCHECK
 								printf("/* INT */\n");
#endif /* ARITHCHECK */

	_desc = (PTR_DESCRIPTOR)arg1 ;
	goto success ;

     }
     else
     { /* ~INT */ 

#ifdef ARITHCHECK
 								printf("/* ~INT */\n");
#endif /* ARITHCHECK */

      if (T_POINTER(arg1))
         { 							/* DESCRIPTOR */

#ifdef ARITHCHECK
 								printf("/* DESCRIPTOR */\n");
#endif /* ARITHCHECK */

          if (R_DESC(DESC(arg1),class) == C_SCALAR)
             {	 						/* SCALAR-DESC */

#ifdef ARITHCHECK
 								printf("/* SCALAR-DESCRIPTOR */\n");
#endif /* ARITHCHECK */

              if (R_DESC(DESC(arg1),type) == TY_REAL)
	         { 						/* REAL */

#ifdef ARITHCHECK
 								printf("/* REAL */\n");
#endif /* ARITHCHECK */

                  i = r = R_SCALAR(DESC(arg1),valr);
		  _desc = (PTR_DESCRIPTOR)TAG_INT(( r<=0.0 ? i : i+1 )) ;
    		  DEC_REFCNT((PTR_DESCRIPTOR)arg1);
                  goto success;
	         }
		 else
                 { 						/* UNDEF */

#ifdef ARITHCHECK
 								printf("/* UNDEF */\n");
#endif /* ARITHCHECK */

                  goto fail ;
		 }
             }
             else
	     { 		                               		/* ~SCALAR */

#ifdef ARITHCHECK
 								printf("/* ~SCALAR */\n");
#endif /* ARITHCHECK */

    	      switch(R_DESC(DESC(arg1),class))
    	      {
      	       case C_DIGIT:
           	      TEST(_desc = digit_ceil(arg1));
    		      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           	      goto success;
      	       case C_MATRIX:
      	       case C_VECTOR:
      	       case C_TVECTOR:
           	      if ((_desc = mvt_op(arg1,OP_CEIL)) == 0) goto fail;
    		      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
           	      goto success;
      	       default: goto fail;
    	      }
	     }
         }
     }

fail:
    END_MODUL("red_ceil");
    DBUG_RETURN(0);
success:
    END_MODUL("red_ceil");
    DBUG_RETURN(1);
}


/*--------------------------------------------------------------------------
 * red_ip --
 *
 * Anwendung:             (... ip ...)
 *
 * Funktion:
 *
 *    ip bildet das innere Produkt zwischen Feldern und Matrizen vom Typ Decimal.
 *        (siehe Jacobsen, Struck: "Zur Diskussion der Einfuehrung des Datentyps
 *                                  Vektor in die HUSCH-Reduktionsmaschine"
 *                                  (Interner Bericht 5.Oktober '88, S.24 ff) )
 *
 * Beispiele:
 *                 ...
 *
 *--------------------------------------------------------------------------
 */
int red_ip(arg1,arg2)                    /* int eingefuegt von RS 30/10/92 */ 
register PTR_DESCRIPTOR arg1,arg2;
{
  register int i,j,k;
  int pos,nrows1,nrows2,ncols1,ncols2;
  int     isum,*ipnew,*ip1,*ip2;
  double  rsum,*rpnew,*rp1,*rp2;
  DESC_CLASS   class1,                             /*        class of arg1        */
               class2,                             /*        class of arg2        */
               res_class;                          /*       class of result       */
  PTR_DESCRIPTOR digit_sum,digit1,digit2;

  DBUG_ENTER("red_ip");

  START_MODUL("red_ip");

  if (!T_POINTER((STACKELEM)arg1))
    goto fail;
  if (!T_POINTER((STACKELEM)arg2))
    goto fail;

  class1 = R_DESC((*arg1),class);
  class2 = R_DESC((*arg2),class);

  nrows1 = R_MVT((*arg1),nrows,class1);
  nrows2 = R_MVT((*arg2),nrows,class2);
  ncols1 = R_MVT((*arg1),ncols,class1);
  ncols2 = R_MVT((*arg2),ncols,class2);

  if (ncols1 != nrows2) goto fail;

  switch(class1)                              /* Bestimmung der Klasse des   */
  {                                           /* Resultats; Erzeugung der    */
  /*---------------*/                         /* Resultatsdeskriptoren;      */
    case C_VECTOR:
  /*---------------*/
        switch(class2)
        {
          case C_VECTOR:
          case C_TVECTOR:
              res_class = NUM_CLASS( R_DESC((*arg1),type) );
              break;
          case C_MATRIX:
              if (ncols2 == 1)
                res_class = NUM_CLASS( R_DESC((*arg1),type) );
              else
                res_class = C_VECTOR;
              break;
          default:
              goto fail;
        }
        break;
  /*---------------*/
    case C_TVECTOR:
  /*---------------*/
        switch(class2)
        {
          case C_VECTOR:
          case C_MATRIX:
              res_class = C_MATRIX;
              break;
          case C_TVECTOR:
              res_class = NUM_CLASS( R_DESC((*arg1),type) );
              break;
          default:
              goto fail;
        }
        break;
  /*---------------*/
    case C_MATRIX:
  /*---------------*/
        switch(class2)
        {
          case C_MATRIX:
          case C_VECTOR:
              res_class = C_MATRIX;
              break;
          case C_TVECTOR:
              if (nrows1 == 1)
                res_class = NUM_CLASS( R_DESC((*arg1),type) );
              else
                res_class = C_TVECTOR;
              break;
          default:
              goto fail;
        }
        break;
  /*---------------*/
    default:
  /*---------------*/
        goto fail;
  }

  switch(R_DESC((*arg1),type))
  {
    case TY_INTEGER:
         switch(R_DESC((*arg2),type))
         {
           case TY_INTEGER:
                ip1   = (int *) R_MVT((*arg1),ptdv,class1);
                ip2   = (int *) R_MVT((*arg2),ptdv,class2);

                if (res_class == C_SCALAR)     /*     Resultat ist Skalar     */
                {
                  isum = 0;

                  for (k=0; k < ncols1; ++k)
                    isum += ip1[k] * ip2[k];
                  _desc = (PTR_DESCRIPTOR)TAG_INT(isum) ;
                }
                else                           /*    Resultat ist Struktur    */
                {
                  NEWDESC(_desc); TEST_DESC;
                  MVT_DESC_MASK(_desc,1,res_class,TY_INTEGER);
                  L_MVT((*_desc),nrows,res_class) = nrows1;
                  L_MVT((*_desc),ncols,res_class) = ncols2;
                  NEWHEAP(nrows1 * ncols2,A_MVT((*_desc),ptdv,res_class));

                  RES_HEAP;
                  ipnew = (int *) R_MVT((*_desc),ptdv,res_class);

                  for (i = 0; i < nrows1; ++i)
                  {
                    for (j = 0; j < ncols2; ++j)
                    {
                      pos = j;
                      isum = ip1[0] * ip2[pos];
                      for (k = 1; k < ncols1; ++k)
                      {
                        pos += ncols2;
                        isum += ip1[k] * ip2[pos];
                      }
                      ipnew[j] = isum;
                    }
                    ip1   += ncols1;
                    ipnew += ncols2;
                  }
                }
                REL_HEAP;
                goto success;
           case TY_REAL:
                ip1   = (int *) R_MVT((*arg1),ptdv,class1);
                rp2   = (double *) R_MVT((*arg2),ptdv,class2);

                if (res_class == C_SCALAR)     /*     Resultat ist Skalar     */
                {
                  NEWDESC(_desc); TEST_DESC;
                  DESC_MASK(_desc,1,res_class,TY_REAL);
                  rsum = 0;

                  RES_HEAP;
                  for (k=0; k < ncols1; ++k)
                    rsum += ip1[k] * rp2[k];
                  L_SCALAR((*_desc),valr) = rsum;
                }
                else                           /*    Resultat ist Struktur    */
                {
                  NEWDESC(_desc); TEST_DESC;
                  MVT_DESC_MASK(_desc,1,res_class,TY_REAL);
                  L_MVT((*_desc),nrows,res_class) = nrows1;
                  L_MVT((*_desc),ncols,res_class) = ncols2;
                  NEWHEAP((nrows1 * ncols2)<<1,A_MVT((*_desc),ptdv,res_class));

                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,res_class);

                  for (i = 0; i < nrows1; ++i)
                  {
                    for (j = 0; j < ncols2; ++j)
                    {
                      pos = j;
                      rsum = ip1[0] * rp2[pos];
                      for (k = 1; k < ncols1; ++k)
                      {
                        pos += ncols2;
                        rsum += ip1[k] * rp2[pos];
                      }
                      rpnew[j] = rsum;
                    }
                    ip1   += ncols1;
                    rpnew += ncols2;
                  }
                }
                REL_HEAP;
                goto success;
           default: goto fail;
         }
    case TY_REAL:
         switch(R_DESC((*arg2),type))
         {
           case TY_INTEGER:
                rp1   = (double *) R_MVT((*arg1),ptdv,class1);
                ip2   = (int *) R_MVT((*arg2),ptdv,class2);

                if (res_class == C_SCALAR)     /*     Resultat ist Skalar     */
                {
                  NEWDESC(_desc); TEST_DESC;
                  DESC_MASK(_desc,1,res_class,TY_REAL);
                  rsum = 0;

                  RES_HEAP;
                  for (k=0; k < ncols1; ++k)
                    rsum += rp1[k] * ip2[k];
                  L_SCALAR((*_desc),valr) = rsum;
                }
                else                           /*    Resultat ist Struktur    */
                {
                  NEWDESC(_desc); TEST_DESC;
                  MVT_DESC_MASK(_desc,1,res_class,TY_REAL);
                  L_MVT((*_desc),nrows,res_class) = nrows1;
                  L_MVT((*_desc),ncols,res_class) = ncols2;
                  NEWHEAP((nrows1 * ncols2)<<1,A_MVT((*_desc),ptdv,res_class));

                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,res_class);

                  for (i = 0; i < nrows1; ++i)
                  {
                    for (j = 0; j < ncols2; ++j)
                    {
                      pos = j;
                      rsum = rp1[0] * ip2[pos];
                      for (k = 1; k < ncols1; ++k)
                      {
                        pos += ncols2;
                        rsum += rp1[k] * ip2[pos];
                      }
                      rpnew[j] = rsum;
                    }
                    rp1   += ncols1;
                    rpnew += ncols2;
                  }
                }
                REL_HEAP;
                goto success;
           case TY_REAL:
                rp1   = (double *) R_MVT((*arg1),ptdv,class1);
                rp2   = (double *) R_MVT((*arg2),ptdv,class2);

                if (res_class == C_SCALAR)     /*     Resultat ist Skalar     */
                {
                  NEWDESC(_desc); TEST_DESC;
                  DESC_MASK(_desc,1,res_class,TY_REAL);
                  rsum = 0;

                  RES_HEAP;
                  for (k=0; k < ncols1; ++k)
                    rsum += rp1[k] * rp2[k];
                  L_SCALAR((*_desc),valr) = rsum;
                }
                else                           /*    Resultat ist Struktur    */
                {
                  NEWDESC(_desc); TEST_DESC;
                  MVT_DESC_MASK(_desc,1,res_class,TY_REAL);
                  L_MVT((*_desc),nrows,res_class) = nrows1;
                  L_MVT((*_desc),ncols,res_class) = ncols2;
                  NEWHEAP((nrows1 * ncols2)<<1,A_MVT((*_desc),ptdv,res_class));

                  RES_HEAP;
                  rpnew = (double *) R_MVT((*_desc),ptdv,res_class);

                  for (i = 0; i < nrows1; ++i)
                  {
                    for (j = 0; j < ncols2; ++j)
                    {
                      pos = j;
                      rsum = rp1[0] * rp2[pos];
                      for (k = 1; k < ncols1; ++k)
                      {
                        pos += ncols2;
                        rsum += rp1[k] * rp2[pos];
                      }
                      rpnew[j] = rsum;
                    }
                    rp1   += ncols1;
                    rpnew += ncols2;
                  }
                }
                REL_HEAP;
                goto success;
           default: goto fail;
         }
    case TY_DIGIT:
         if (R_DESC((*arg2),type) != TY_DIGIT)
           goto fail;
         {
           register int pos1,pos2;

           _digit_recycling = FALSE;

           if (res_class == C_DIGIT)           /*     Resultat ist Skalar     */
           {
             digit_sum = _dig0;
             INC_REFCNT(_dig0);
             for (k=0; k < ncols1; ++k)
             {
               TEST(digit1 = digit_mul(R_MVT((*arg1),ptdv,class1)[k],R_MVT((*arg2),ptdv,class2)[k]));
               _digit_recycling = TRUE;
               TEST(digit2 = DI_ADD(digit_sum,digit1));
               _digit_recycling = FALSE;
               DEC_REFCNT(digit1);
               DEC_REFCNT(digit_sum);
               digit_sum = digit2;
             }
             _desc = digit_sum;
           }
           else                                /*    Resultat ist Struktur    */
           {
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,res_class,TY_DIGIT);
             L_MVT((*_desc),nrows,res_class) = nrows1;
             L_MVT((*_desc),ncols,res_class) = ncols2;
             NEWHEAP(nrows1 * ncols2,A_MVT((*_desc),ptdv,res_class));

             for (i = 0; i < nrows1; ++i)
               for (j = 0; j < ncols2; ++j)
               {
                 pos1 = i * ncols1;
                 pos2 = j;
                 TEST(digit_sum = digit_mul(R_MVT((*arg1),ptdv,class1)[pos1],R_MVT((*arg2),ptdv,class2)[pos2]));
                 for (k = 1; k < ncols1; ++k)
                 {
                   TEST(digit1 = digit_mul(R_MVT((*arg1),ptdv,class1)[++pos1],R_MVT((*arg2),ptdv,class2)[pos2+=ncols2]));
                   _digit_recycling = TRUE;
                   TEST(digit2 = DI_ADD(digit_sum,digit1));
                   _digit_recycling = FALSE;
                   DEC_REFCNT(digit1);
                   DEC_REFCNT(digit_sum);
                   digit_sum = digit2;
                 }
                 L_MVT((*_desc),ptdv,res_class)[i*ncols2 + j] = (int) digit_sum;
               }
           }
           _digit_recycling = TRUE;
         }
         goto success;
    default: goto fail;
  }

fail:
  END_MODUL("red_ip");
  DBUG_RETURN(0);

success:
  DEC_REFCNT(arg1);
  DEC_REFCNT(arg2);
  END_MODUL("red_ip");
  DBUG_RETURN(1);
}

/*---------------------------------------------------------------------------
 * red_c -- ist die Sammelfunktion fuer die Koordinatenfunktionen
 *          c_+,c_*,c_-,c_div,c_max,c_min.
 *
 * Anwendung:   ap c_? [coor,matrix]
 *
 * Funktion:
 *
 *    Dieses sind die dyadischen arithmetischen Funktionen, die entlang
 *    einer Koordinate einer Matrix ausgefuehrt werden. Ist 'coor' gleich
 *    Eins, so werden die Zeilen und bei 'coor' gleich Zwei die Spalten
 *    verknuepft. Das Ergebnis ist entweder ein Vektor oder
 *    ein transponierter Vektor.
 *    Waehrend mit Hilfe von c_+ Zeilen (bzw. Spalten) addiert werden, so
 *    werden durch c_- dieselben abwechselnd subtrahiert und addiert.
 *    Analoges gilt fuer c_* und c_/.
 *
 * Beispiele:
 *    ap c_+  [1, mat<1,2>,<3,4>.]  =>  vect<4,6>
 *    ap c_-  [2, mat<1,2>,<3,4>.]  =>  tvect<~1,~1>
 *    ap c_*  [1, mat<1>,<2>,<3>.]  =>  vect<6>
 *    ap c_/  [2, mat<1,2,3>.    ]  =>  tvect<1.5>
 *    ap c_+  [1, mat<7>.        ]  =>  vect<7>
 *
 *-------------------------------------------------------------------------
 */
int red_c(arg1,arg2)           /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  DBUG_ENTER ("red_c");

  DBUG_RETURN(0);
}

int red_c_max(arg1,arg2)         /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  register int i,j;
  PTR_DESCRIPTOR digit1/* ,digit2,digit3*/;  /* RS 30/10/92 */ 
  int nrows,ncols,/*dim,*/pos;         /* RS 30/10/92 */ 
  int vali,coordinate,*ip,*ipnew;
  double valr,*rp,*rpnew;
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_c_max");

  START_MODUL("red_c_max");

  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2))
  {
    if (R_DESC(DESC(arg2),class) != C_MATRIX)
      goto fail;

    if (T_INT(arg1))
      coordinate = VAL_INT(arg1) ;
    else
    {
     switch(R_DESC(DESC(arg1),class))
     {
       case C_SCALAR:
              if ((coordinate = R_SCALAR(DESC(arg1),valr)) != R_SCALAR(DESC(arg1),valr))
                goto fail;
            break;
       case C_DIGIT:
            if (conv_digit_int(arg1,&coordinate) == 0)
              goto fail;
            break;
       default: goto fail;
     }
    }

    nrows = R_MVT(DESC(arg2),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg2),ncols,C_MATRIX);

    if (coordinate == 1)
    {
      /* ein Zeilenvektor entsteht */
      class = C_VECTOR;

      if (nrows == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'Vector' anlegen: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }

      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               ipnew[i] = ip[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (ip[i] < ipnew[i])
                           ipnew[i] = ip[i];
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               rpnew[i] = rp[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (rp[i] < rpnew[i])
                           rpnew[i] = rp[i];
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *) R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
             }

                    pos = 0;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (digit_lt(IP2[i+pos],(IPNEW[i])))
                        {
                          DEC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
                          INC_REFCNT((PTR_DESCRIPTOR)(IP2[i+pos]));
                          (IPNEW[i]) = IP2[i+pos];
                        }
                    }
             goto success;
        case TY_BOOL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_BOOL);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
                ipnew[i] = ip[i];

               for (j = 1; j < nrows; ++j)
               {
                 ip += ncols;
                 for (i = 0; i < ncols; ++i)
                   if (ip[i] > ipnew[i])
                      ipnew[i] = ip[i];
               }

             REL_HEAP;
             goto success;
        case TY_STRING:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_STRING);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;
             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               (IPNEW[i]) = IP2[i];
               INC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
             }
             pos = 0;
               for (j = 1; j < nrows; ++j)
               {
                 pos += ncols;
                 for (i = 0; i < ncols; ++i)
                   if (str_lt((IPNEW[i]),IP2[i+pos]))
                   {
                     DEC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
                     INC_REFCNT((PTR_DESCRIPTOR)IP2[i+pos]);
                   (IPNEW[i]) = IP2[i+pos];
                   }
               }
               REL_HEAP;
               goto success;
        default: goto fail;
      }
      /*---------------------------------*/
    } /* Ende des Falles coordinate == 1 */
      /*---------------------------------*/
    if (coordinate == 2)
    {
      /* ein Spaltenvektor entsteht */
      class = C_TVECTOR;

      if (ncols == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'TVector' anlegen: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }
      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                        if (vali > ip[j])
                          vali = ip[j];
                      ipnew[i] = vali;
                      ip += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                        if (valr > rp[j])
                          valr = rp[j];
                      rpnew[i] = valr;
                      rp += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

                    RES_HEAP;
                    ipnew = (int *)R_MVT((*_desc),ptdv,class);
                    ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) ip[0];
                      for (j = 1; j < ncols; ++j)
                        if (digit_lt(ip[j],digit1))
                          digit1 = (PTR_DESCRIPTOR)ip[j];
                      INC_REFCNT((PTR_DESCRIPTOR)digit1);
                      ipnew[i] = (int) digit1;
                      ip += ncols;
                    }
                    REL_HEAP;
             goto success;
        case TY_BOOL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_BOOL);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

               for (i = 0; i < nrows; ++i)
               {
                 vali = ip[0];
                 for (j = 1; j < ncols; ++j)
                   if (vali < ip[j])
                     vali = ip[j];
                 ipnew[i] = vali;
                 ip += ncols;
               }
             REL_HEAP;
             goto success;
        case TY_STRING:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_STRING);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;
             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

               for (i = 0; i < nrows; ++i)
               {
                 digit1 = (PTR_DESCRIPTOR) ip[0];
                 for (j = 1; j < ncols; ++j)
                   if (str_lt(digit1,ip[j]))
                     digit1 = (PTR_DESCRIPTOR) ip[j];
                 INC_REFCNT((PTR_DESCRIPTOR)digit1);
                 ipnew[i] = (int) digit1;
                 ip += ncols;
               }
             REL_HEAP;
             goto success;
        default: goto fail;
      }
    } /* Ende des Falles coordinate == 1 */
    else goto fail;
  } /* Ende von T_POINTER */

fail:
  END_MODUL("red_c_max");
  DBUG_RETURN(0);

success:
  if ( T_POINTER(arg1) )
     {
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
     }
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_c_max");
  DBUG_RETURN(1);
}

int red_c_min(arg1,arg2)               /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  register int i,j;
  PTR_DESCRIPTOR digit1/*,digit2,digit3*/;     /* RS 30/10/92 */ 
  int nrows,ncols,/*dim,*/pos;                 /* RS 30/10/92 */ 
  int vali,coordinate,
      *ip=(int *)0,*ipnew=(int *)0; /* Initialisierung von TB, 6.11.1992 */
  double valr,*rp,*rpnew;
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_c_min");

  START_MODUL("red_c_min");

  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2))
  {
    if (R_DESC(DESC(arg2),class) != C_MATRIX)
      goto fail;

    if (T_INT(arg1))
      coordinate = VAL_INT(arg1) ;
    else
    {
     switch(R_DESC(DESC(arg1),class))
     {
       case C_SCALAR:
              if ((coordinate = R_SCALAR(DESC(arg1),valr)) != R_SCALAR(DESC(arg1),valr))
                goto fail;
            break;
       case C_DIGIT:
            if (conv_digit_int(arg1,&coordinate) == 0)
              goto fail;
            break;
       default: goto fail;
     }
    }

    nrows = R_MVT(DESC(arg2),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg2),ncols,C_MATRIX);

    if (coordinate == 1)
    {
      /* ein Zeilenvektor entsteht */
      class = C_VECTOR;

      if (nrows == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'Vector' anlegen: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }

      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               ipnew[i] = ip[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (ip[i] < ipnew[i])
                           ipnew[i] = ip[i];
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               rpnew[i] = rp[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (rp[i] < rpnew[i])
                           rpnew[i] = rp[i];
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *) R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
             }

                    pos = 0;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                        if (digit_lt(IP2[i+pos],(IPNEW[i])))
                        {
                          DEC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
                          INC_REFCNT((PTR_DESCRIPTOR)(IP2[i+pos]));
                          (IPNEW[i]) = IP2[i+pos];
                        }
                    }
             goto success;
        case TY_BOOL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_BOOL);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
                ipnew[i] = ip[i];

               for (j = 1; j < nrows; ++j)
               {
                 ip += ncols;
                 for (i = 0; i < ncols; ++i)
                   if (ip[i] < ipnew[i])
                     ipnew[i] = ip[i];
               }
             REL_HEAP;
             goto success;
        case TY_STRING:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_STRING);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;
             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               (IPNEW[i]) = IP2[i];
               INC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
             }
             pos = 0;
                for (j = 1; j < nrows; ++j)
                {
                  pos += ncols;
                  for (i = 0; i < ncols; ++i)
                    if (str_lt(IP2[i+pos],(IPNEW[i])))
                    {
                      DEC_REFCNT((PTR_DESCRIPTOR)(IPNEW[i]));
                      INC_REFCNT((PTR_DESCRIPTOR)IP2[i+pos]);
                      (IPNEW[i]) = IP2[i+pos];
                    }
                }
               REL_HEAP;
               goto success;
        default: goto fail;
      }
      /*---------------------------------*/
    } /* Ende des Falles coordinate == 1 */
      /*---------------------------------*/
    if (coordinate == 2)
    {
      /* ein Spaltenvektor entsteht */
      class = C_TVECTOR;

      if (ncols == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'TVector' anlegen: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }
      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                        if (vali > ip[j])
                          vali = ip[j];
                      ipnew[i] = vali;
                      ip += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                        if (valr > rp[j])
                          valr = rp[j];
                      rpnew[i] = valr;
                      rp += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

                    RES_HEAP;
                    ipnew = (int *)R_MVT((*_desc),ptdv,class);
                    ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) ip[0];
                      for (j = 1; j < ncols; ++j)
                        if (digit_lt(ip[j],digit1))
                          digit1 = (PTR_DESCRIPTOR)ip[j];
                      INC_REFCNT((PTR_DESCRIPTOR)digit1);
                      ipnew[i] = (int) digit1;
                      ip += ncols;
                    }
                    REL_HEAP;
             goto success;
        case TY_BOOL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_BOOL);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

               for (i = 0; i < nrows; ++i)
               {
                 vali = ip[0];
                 for (j = 1; j < ncols; ++j)
                   if (vali > ip[j])
                     vali = ip[j];
                 ipnew[i] = vali;
                 ip += ncols;
               }
             REL_HEAP;
             goto success;
        case TY_STRING:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_STRING);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;
             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

               for (i = 0; i < nrows; ++i)
               {
                 digit1 = (PTR_DESCRIPTOR) ip[0];
                 for (j = 1; j < ncols; ++j)
                   if (str_lt(ip[j],digit1))
                     digit1 = (PTR_DESCRIPTOR)ip[j];
                 INC_REFCNT((PTR_DESCRIPTOR)digit1);
                 ipnew[i] = (int) digit1;
                 ip += ncols;
               }
             REL_HEAP;
             goto success;
        default: goto fail;
      }
    } /* Ende des Falles coordinate == 1 */
    else goto fail;
  } /* Ende von T_POINTER */

fail:
  END_MODUL("red_c_min");
  DBUG_RETURN(0);

success:
  if ( T_POINTER(arg1) )
     {
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
     }
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_c_min");
  DBUG_RETURN(1);
}

int red_c_plus(arg1,arg2)                    /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  register int i,j;
  PTR_DESCRIPTOR digit1,digit2,digit3;
  int nrows,ncols,/*dim,*/pos;               /* RS 30/10/92 */ 
  int vali,coordinate,*ip,*ipnew;
  double valr,*rp,*rpnew;
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_c_plus");

  START_MODUL("red_c_plus");

  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2))
  {
    if (R_DESC(DESC(arg2),class) != C_MATRIX)
      goto fail;

    if (T_INT(arg1))
      coordinate = VAL_INT(arg1) ;
    else
    {
     switch(R_DESC(DESC(arg1),class))
     {
       case C_SCALAR:
              if ((coordinate = R_SCALAR(DESC(arg1),valr)) != R_SCALAR(DESC(arg1),valr))
                goto fail;
            break;
       case C_DIGIT:
            if (conv_digit_int(arg1,&coordinate) == 0)
              goto fail;
            break;
       default: goto fail;
     }
    }

    nrows = R_MVT(DESC(arg2),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg2),ncols,C_MATRIX);

    if (coordinate == 1)
    {
      /* ein Zeilenvektor entsteht */
      class = C_VECTOR;

      if (nrows == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'Vector' anlegen: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }

      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               ipnew[i] = ip[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                       for (i = 0; i < ncols; ++i)
                      ipnew[i] += ip[i];
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               rpnew[i] = rp[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        rpnew[i] += rp[i];
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *) R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
             }

                    REL_HEAP;
                    pos = 0;
                    _digit_recycling = FALSE;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                      {
                        digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos];
                        TEST(digit3 = DI_ADD(digit1,digit2));
                        DEC_REFCNT(digit1);
                        L_MVT((*_desc),ptdv,class)[i] = (int) digit3;
                      }
                    }
                    _digit_recycling = TRUE;
             goto success;
        default: goto fail;
      }
      /*---------------------------------*/
    } /* Ende des Falles coordinate == 1 */
      /*---------------------------------*/
    if (coordinate == 2)
    {
      /* ein Spaltenvektor entsteht */
      class = C_TVECTOR;

      if (ncols == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'TVector' anlegen: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }
      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                        vali += ip[j];
                      ipnew[i] = vali;
                      ip += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                        valr += rp[j];
                      rpnew[i] = valr;
                      rp += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

                    _digit_recycling = FALSE;
                    pos = 0;
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos];
                      INC_REFCNT(digit1);
                      for (j = 1; j < ncols; ++j)
                      {
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[j+pos];
                        TEST(digit3 = DI_ADD(digit1,digit2));
                        DEC_REFCNT(digit1);
                        digit1 = digit3;
                      }
                      L_MVT((*_desc),ptdv,class)[i] = (int) digit1;
                      pos += ncols;
                    }
                    _digit_recycling = TRUE;
             goto success;
        default: goto fail;
      }
    } /* Ende des Falles coordinate == 1 */
    else goto fail;
  } /* Ende von T_POINTER */

fail:
  END_MODUL("red_c_plus");
  DBUG_RETURN(0);

success:
  if ( T_POINTER(arg1) )
     {
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
     }
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_c_plus");
  DBUG_RETURN(1);
}

int red_c_minus(arg1,arg2)               /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  register int i,j;
  PTR_DESCRIPTOR digit1,digit2,digit3;
  int nrows,ncols,/*dim,*/pos;              /* RS 30/10/92 */ 
  int vali,coordinate,*ip,*ipnew;
  double valr,*rp,*rpnew;
/*int op;                                      RS 30/10/92 */ 
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_c_minus");

  START_MODUL("red_c_minus");

  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2))
  {
    if (R_DESC(DESC(arg2),class) != C_MATRIX)
      goto fail;

    if (T_INT(arg1))
      coordinate = VAL_INT(arg1) ;
    else
    {
     switch(R_DESC(DESC(arg1),class))
     {
       case C_SCALAR:
              if ((coordinate = R_SCALAR(DESC(arg1),valr)) != R_SCALAR(DESC(arg1),valr))
                goto fail;
            break;
       case C_DIGIT:
            if (conv_digit_int(arg1,&coordinate) == 0)
              goto fail;
            break;
       default: goto fail;
     }
    }

    nrows = R_MVT(DESC(arg2),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg2),ncols,C_MATRIX);

    if (coordinate == 1)
    {
      /* ein Zeilenvektor entsteht */
      class = C_VECTOR;

      if (nrows == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'Vector' anlegen: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }

      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               ipnew[i] = ip[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                      for (i = 0; i < ncols; ++i)
                        ipnew[i] -= ip[i];
                      if (++j < nrows)
                      {
                        ip +=ncols;
                        for (i = 0; i < ncols; ++i)
                          ipnew[i] += ip[i];
                      }
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               rpnew[i] = rp[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        rpnew[i] -= rp[i];
                      if (++j < nrows)
                      {
                        rp +=ncols;
                        for (i = 0; i < ncols; ++i)
                          rpnew[i] += rp[i];
                      }
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *) R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
             }

                    REL_HEAP;
                    pos = 0;
                    _digit_recycling = FALSE;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                      {            /* eine Zeile abziehen */
                        digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos];
                        TEST(digit3 = DI_SUB(digit1,digit2));
                        DEC_REFCNT(digit1);
                        L_MVT((*_desc),ptdv,class)[i] = (int) digit3;
                      }
                      if (++j < nrows)
                      {
                        pos += ncols;
                        for (i = 0; i < ncols; ++i)  /* Zeile addieren */
                        {
                          digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                          digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos];
                          TEST(digit3 = DI_ADD(digit1,digit2));
                          DEC_REFCNT(digit1);
                          L_MVT((*_desc),ptdv,class)[i] = (int) digit3;
                        }
                      }
                    }
                    _digit_recycling = TRUE;
             goto success;
        default: goto fail;
      }
      /*---------------------------------*/
    } /* Ende des Falles coordinate == 1 */
      /*---------------------------------*/
    if (coordinate == 2)
    {
      /* ein Spaltenvektor entsteht */
      class = C_TVECTOR;

      if (ncols == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'TVector' anlegen: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }
      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                      {
                        vali -= ip[j];
                        if (++j >= ncols) break;
                        vali += ip[j];
                      }
                      ipnew[i] = vali;
                      ip += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                      {
                        valr -= rp[j];
                        if (++j >= ncols) break;
                        valr += rp[j];
                      }
                      rpnew[i] = valr;
                      rp += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

                    _digit_recycling = FALSE;
                    pos = 0;
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos];
                      INC_REFCNT(digit1);
                      for (j = 1; j < ncols; ++j)
                      {
                        digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos+j];
                        TEST(digit3 = DI_SUB(digit1,digit2));
                        DEC_REFCNT(digit1);
                        digit1 = digit3;
                      }
                      L_MVT((*_desc),ptdv,class)[i] = (int) digit1;
                      pos += ncols;
                    }
                    _digit_recycling = TRUE;
             goto success;
        default: goto fail;
      }
    } /* Ende des Falles coordinate == 1 */
    else goto fail;
  } /* Ende von T_POINTER */

fail:
  END_MODUL("red_c_minus");
  DBUG_RETURN(0);

success:
  if ( T_POINTER(arg1) )
     {
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
     }
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_c_minus");
  DBUG_RETURN(1);
}

int red_c_mult(arg1,arg2)                /* int eingefuegt von RS 30/10/92 */
register STACKELEM arg1,arg2;
{
  register int i,j;
  PTR_DESCRIPTOR digit1,digit2/*,digit3*/;       /* RS 30/10/92 */
  int nrows,ncols,/*dim,*/pos;
  int vali,coordinate,*ip,*ipnew;
  double valr,*rp,*rpnew;
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_c_mult");

  START_MODUL("red_c_mult");

  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2))
  {
    if (R_DESC(DESC(arg2),class) != C_MATRIX)
      goto fail;

    if (T_INT(arg1))
      coordinate = VAL_INT(arg1) ;
    else
    {
     switch(R_DESC(DESC(arg1),class))
     {
       case C_SCALAR:
              if ((coordinate = R_SCALAR(DESC(arg1),valr)) != R_SCALAR(DESC(arg1),valr))
                goto fail;
            break;
       case C_DIGIT:
            if (conv_digit_int(arg1,&coordinate) == 0)
              goto fail;
            break;
       default: goto fail;
     }
    }

    nrows = R_MVT(DESC(arg2),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg2),ncols,C_MATRIX);

    if (coordinate == 1)
    {
      /* ein Zeilenvektor entsteht */
      class = C_VECTOR;

      if (nrows == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'Vector' anlegen: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }

      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               ipnew[i] = ip[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      ip += ncols;
                      for (i = 0; i < ncols; ++i)
                        ipnew[i] *= ip[i];
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               rpnew[i] = rp[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        rpnew[i] *= rp[i];
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *) R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
             }

                    REL_HEAP;
                    pos = 0;
                    _digit_recycling = FALSE;
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i)
                      {
                        digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                        TEST(digit2 = digit_mul(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos]));
                        DEC_REFCNT(digit1);
                        L_MVT((*_desc),ptdv,class)[i] = (int) digit2;
                      }
                    }
                    _digit_recycling = TRUE;
             goto success;
        default: goto fail;
      }
      /*---------------------------------*/
    } /* Ende des Falles coordinate == 1 */
      /*---------------------------------*/
    if (coordinate == 2)
    {
      /* ein Spaltenvektor entsteht */
      class = C_TVECTOR;

      if (ncols == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'TVector' anlegen: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }
      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_INTEGER);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             ipnew = (int *)R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      vali = ip[0];
                      for (j = 1; j < ncols; ++j)
                        vali *= ip[j];
                      ipnew[i] = vali;
                      ip += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                        valr *= rp[j];
                      rpnew[i] = valr;
                      rp += ncols;
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

                    _digit_recycling = FALSE;
                    pos = 0;
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos];
                      INC_REFCNT(digit1);
                      for (j = 1; j < ncols; ++j)
                      {
                        TEST(digit2 = digit_mul(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos+j]));
                        DEC_REFCNT(digit1);
                        digit1 = digit2;
                      }
                      L_MVT((*_desc),ptdv,class)[i] = (int) digit1;
                      pos += ncols;
                    }
                    _digit_recycling = TRUE;
             goto success;
        default: goto fail;
      }
    } /* Ende des Falles coordinate == 1 */
    else goto fail;
  } /* Ende von T_POINTER */

fail:
  END_MODUL("red_c_mult");
  DBUG_RETURN(0);

success:
  if ( T_POINTER(arg1) )
     {
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
     }
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_c_mult");
  DBUG_RETURN(1);
}

int red_c_div(arg1,arg2)               /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  register int i,j;
  PTR_DESCRIPTOR digit1,digit2/*,digit3*/;        /* RS 30/10/92 */  
  int nrows,ncols/*,dim*/,pos;                    /* RS 30/10/92 */ 
  int /*vali,*/coordinate,*ip,*ipnew;             /* RS 30/10/92 */ 
  double valr,*rp,*rpnew;
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_c_div");

  START_MODUL("red_c_div");

  if ((T_POINTER(arg1) || T_INT(arg1)) && T_POINTER(arg2))
  {
    if (R_DESC(DESC(arg2),class) != C_MATRIX)
      goto fail;

    if (T_INT(arg1))
      coordinate = VAL_INT(arg1) ;
    else
    {
     switch(R_DESC(DESC(arg1),class))
     {
       case C_SCALAR:
              if ((coordinate = R_SCALAR(DESC(arg1),valr)) != R_SCALAR(DESC(arg1),valr))
                goto fail;
            break;
       case C_DIGIT:
            if (conv_digit_int(arg1,&coordinate) == 0)
              goto fail;
            break;
       default: goto fail;
     }
    }

    nrows = R_MVT(DESC(arg2),nrows,C_MATRIX);
    ncols = R_MVT(DESC(arg2),ncols,C_MATRIX);

    if (coordinate == 1)
    {
      /* ein Zeilenvektor entsteht */
      class = C_VECTOR;

      if (nrows == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'Vector' anlegen: ---- */
          _desc = gen_many_id(C_VECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }

      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
               NEWDESC(_desc); TEST_DESC;
               MVT_DESC_MASK(_desc,1,class,TY_REAL);
               NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
               L_MVT((*_desc),ncols,class) = ncols;
               L_MVT((*_desc),nrows,class) = 1;

               RES_HEAP;
               rpnew = (double *)R_MVT((*_desc),ptdv,class);
               ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

               /* 1.Zeile kopieren */
               for (i = 0; i < ncols; ++i)
                 rpnew[i] = (double) ip[i];

               for (j = 1; j < nrows; ++j)
               {
                 ip += ncols;
                 for (i = 0; i < ncols; ++i)
                   rpnew[i] /= (double) ip[i];
                 if (++j < nrows)
                 {
                   ip +=ncols;
                   for (i = 0; i < ncols; ++i)
                     rpnew[i] *= (double) ip[i];
                 }
               }
               REL_HEAP;
               goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(ncols<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
               rpnew[i] = rp[i];

                    for (j = 1; j < nrows; ++j)
                    {
                      rp += ncols;
                      for (i = 0; i < ncols; ++i)
                        rpnew[i] /= rp[i];
                      if (++j < nrows)
                      {
                        rp +=ncols;
                        for (i = 0; i < ncols; ++i)
                          rpnew[i] *= rp[i];
                      }
                    }

             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(ncols,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = ncols;
             L_MVT((*_desc),nrows,class) = 1;

             RES_HEAP;
             ipnew = (int *) R_MVT((*_desc),ptdv,class);
             ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);
             /* 1.Zeile kopieren */
             for (i = 0; i < ncols; ++i)
             {
               ipnew[i] = ip[i];
               INC_REFCNT((PTR_DESCRIPTOR)ipnew[i]);
             }

                    REL_HEAP;
                    pos = 0;
                    _digit_recycling = FALSE;  /* da durch Null geteilt werden kann !*/
                    for (j = 1; j < nrows; ++j)
                    {
                      pos += ncols;
                      for (i = 0; i < ncols; ++i) /* mit einer Zeile multiplizieren */
                      {
                        digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                        if ((digit2 = digit_mul(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos])) == 0)
                        {
                          for (i= 0; i < ncols; ++i)
                            DEC_REFCNT((PTR_DESCRIPTOR)R_MVT((*_desc),ptdv,class)[i]);
                          _digit_recycling = TRUE;
                          DEC_REFCNT(_desc);
                          goto fail;
                        }
                        DEC_REFCNT(digit1);
                        L_MVT((*_desc),ptdv,class)[i] = (int) digit2;
                      }
                      if ( ++j < nrows)
                      {
                        pos += ncols;
                        for (i = 0; i < ncols; ++i)  /* durch eine Zeile teilen */
                        {
                          digit1 = (PTR_DESCRIPTOR) R_MVT((*_desc),ptdv,class)[i];
                          if ((digit2 = digit_div(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[i+pos])) == 0)
                          {
                            for (i= 0; i < ncols; ++i)
                              DEC_REFCNT((PTR_DESCRIPTOR)R_MVT((*_desc),ptdv,class)[i]);
                            _digit_recycling = TRUE;
                            DEC_REFCNT(_desc);
                            goto fail;
                          }
                          DEC_REFCNT(digit1);
                          L_MVT((*_desc),ptdv,class)[i] = (int) digit2;
                        }
                      }
                    }
                    _digit_recycling = TRUE;
             goto success;
        default: goto fail;
      }
      /*---------------------------------*/
    } /* Ende des Falles coordinate == 1 */
      /*---------------------------------*/
    if (coordinate == 2)
    {
      /* ein Spaltenvektor entsteht */
      class = C_TVECTOR;

      if (ncols == 1)
      {
        if (R_DESC(DESC(arg2),type) == TY_MATRIX)
          goto fail;
        else
        {
          /* ---- indirekten Desk. vom Typ 'TVector' anlegen: ---- */
          _desc = gen_many_id(C_TVECTOR,(PTR_DESCRIPTOR) arg2);
          TEST_DESC;
          goto success;
        }
      }
      switch(R_DESC(DESC(arg2),type))
      {
        case TY_INTEGER:
               NEWDESC(_desc); TEST_DESC;
               MVT_DESC_MASK(_desc,1,class,TY_REAL);
               NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
               L_MVT((*_desc),ncols,class) = 1;
               L_MVT((*_desc),nrows,class) = nrows;

               RES_HEAP;
               rpnew = (double *)R_MVT((*_desc),ptdv,class);
               ip = (int *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

               for (i = 0; i < nrows; ++i)
               {
                 valr = (double) ip[0];
                 for (j = 1; j < ncols; ++j)
                 {
                   valr /= (double) ip[j];
                   if (++j >= ncols) break;
                   valr *= (double) ip[j];
                 }
                 rpnew[i] = valr;
                 ip += ncols;
               }
               REL_HEAP;
               goto success;
        case TY_REAL:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_REAL);
             NEWHEAP(nrows<<1,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

             RES_HEAP;
             rpnew = (double *)R_MVT((*_desc),ptdv,class);
             rp = (double *) R_MVT(DESC(arg2),ptdv,C_MATRIX);

                    for (i = 0; i < nrows; ++i)
                    {
                      valr = rp[0];
                      for (j = 1; j < ncols; ++j)
                      {
                        valr /= rp[j];
                        if (++j >= ncols) break;
                        valr *= rp[j];
                      }
                      rpnew[i] = valr;
                      rp += ncols;
                    }
             REL_HEAP;
             goto success;
        case TY_DIGIT:
             NEWDESC(_desc); TEST_DESC;
             MVT_DESC_MASK(_desc,1,class,TY_DIGIT);
             NEWHEAP(nrows,A_MVT((*_desc),ptdv,class));
             L_MVT((*_desc),ncols,class) = 1;
             L_MVT((*_desc),nrows,class) = nrows;

                    _digit_recycling = FALSE;
                    pos = 0;
                    for (i = 0; i < nrows; ++i)
                    {
                      digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos];
                      INC_REFCNT(digit1);
                      for (j = 1; j < ncols; ++j)
                      {
                        if ((digit2 = digit_div(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[pos+j])) == 0)
                        {
                          DEC_REFCNT(digit1);
                          for (--i; i>=0; --i)
                            DEC_REFCNT((PTR_DESCRIPTOR)R_MVT((*_desc),ptdv,class)[i]);
                          _digit_recycling = TRUE;
                          DEC_REFCNT(_desc);
                          goto fail;
                        }
                        DEC_REFCNT(digit1);
                        digit1 = digit2;
                        if (++j >= ncols) break;
                        TEST(digit2 = digit_mul(digit1,R_MVT(DESC(arg2),ptdv,C_MATRIX)[j+pos]));
                        DEC_REFCNT(digit1);
                        digit1 = digit2;
                      }
                      L_MVT((*_desc),ptdv,class)[i] = (int) digit1;
                      pos += ncols;
                    }
                    _digit_recycling = TRUE;
             goto success;
        default: goto fail;
      }
    } /* Ende des Falles coordinate == 1 */
    else goto fail;
  } /* Ende von T_POINTER */

fail:
  END_MODUL("red_c_div");
  DBUG_RETURN(0);

success:
  if ( T_POINTER(arg1) )
     {
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
     }
  DEC_REFCNT((PTR_DESCRIPTOR)arg2);
  END_MODUL("red_c_div");
  DBUG_RETURN(1);
}


/*---------------------------------------------------------------------------
 * red_vc -- ist die Sammelfunktion fuer die Koordinatenfunktionen
 *           vc_+,vc_*,vc_-,vc_div,vc_max,vc_min.
 *
 * Anwendung:   ap vc_? [vector]
 *              ap vc_? [tvector]
 *
 * Funktion:
 *
 *    Dieses sind die dyadischen arithmetischen Funktionen, die entlang
 *    eines Vektors oder transponierten Vektors ausgefuehrt werden.
 *    Das Ergebnis ist ein Skalar.
 *    Waehrend mit Hilfe von c_+ alle Komponenten addiert werden, so
 *    werden durch c_- dieselben abwechselnd subtrahiert und addiert.
 *    Analoges gilt fuer c_* und c_/.
 *
 * Beispiele:
 *    ap vc_+  [vect<1,2,3,4>]   =>  10
 *    ap vc_-  [tvect<1,2,3,4>]  =>  ~2
 *    ap vc_*  [vect<1,2,3>]     =>   6
 *    ap vc_/  [tvect<1,2,3>]    =>   1.5
 *    ap vc_+  [tvect<7>]        =>   7
 *
 *-------------------------------------------------------------------------
 */
int red_vc(arg1,arg2)                       /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg1,arg2;
{
  DBUG_ENTER ("red_vc");

  DBUG_RETURN(0);
}

int 	red_vc_max(arg)                   /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg;
{
  register int i/*,j*/;                               /* RS 30/10/92 */ 
  PTR_DESCRIPTOR digit1/*,digit2,digit3*/;            /* RS 30/10/92 */ 
  int /*nrows,ncols,*/dim/*,pos*/;                    /* RS 30/10/92 */ 
  int vali/*,coordinate*/,*ip/*,*ipnew*/;             /* RS 30/10/92 */
  double valr,*rp/*,*rpnew*/;                         /* RS 30/10/92 */ 
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_vc_max");

  START_MODUL("red_vc_max");

  if (T_POINTER(arg))
  {
    if (
         ((class = R_DESC(DESC(arg),class)) != C_VECTOR ) &&
         ( class                            != C_TVECTOR)
       )
      goto fail;

    if (class == C_VECTOR)
      dim = R_MVT(DESC(arg),ncols,C_VECTOR);
    else
      dim = R_MVT(DESC(arg),nrows,C_TVECTOR);

    if (dim == NULL) {     /* if argument is a nilvect or nitvect */
      INC_REFCNT(_dig0);   /* ==> result is 0                     */
      _desc = _dig0;
      goto success;
    }


  switch(R_DESC(DESC(arg),type))
  {
    case TY_INTEGER:

         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);

         vali = ip[0];

                for (i = 1; i < dim; ++i)
                  if (vali < ip[i])
                    vali = ip[i];

         REL_HEAP;
         _desc = (PTR_DESCRIPTOR)TAG_INT(vali) ;
         goto success ;
    case TY_REAL:
         NEWDESC(_desc); TEST_DESC;
         DESC_MASK(_desc,1,C_SCALAR,TY_REAL);

         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg),ptdv,class);

         valr = rp[0];

                for (i = 1; i < dim; ++i)
                  if (valr < rp[i])
                    valr = rp[i];

         REL_HEAP;
         L_SCALAR((*_desc),valr) = valr;
         goto success;
    case TY_DIGIT:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

                for (i = 1; i < dim; ++i)
                {
                  digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  if (digit_lt(_desc,digit1))
                  {
                    DEC_REFCNT(_desc);
                    _desc = digit1;
                    INC_REFCNT(_desc);
                  }
                }
         goto success;
    case TY_BOOL:
         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);
         vali = ip[0];

                for (i = 1; i < dim; ++i)
                  if (vali < ip[i])
                    vali = ip[i];

         REL_HEAP;
         _desc = (PTR_DESCRIPTOR) (vali ? SA_TRUE : SA_FALSE);
         goto success;
    case TY_STRING:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

                RES_HEAP;
                for (i = 1; i < dim; ++i)
                {
                  digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  if (str_lt(_desc,digit1))
                  {
                    DEC_REFCNT(_desc);
                    _desc = digit1;
                    INC_REFCNT(_desc);
                  }
                }
                REL_HEAP;
                goto success;
    default: goto fail;
  }  /* Ende von switch(type) */
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("red_vc_max");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg);
  END_MODUL("red_vc_max");
  DBUG_RETURN(1);

}

int 	red_vc_min(arg)                   /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg;
{
  register int i/*,j*/;                     /* RS 30/10/92 */ 
  PTR_DESCRIPTOR digit1/*,digit2,digit3*/;  /* RS 30/10/92 */ 
  int /*nrows,ncols,*/dim/*,pos*/;          /* RS 30/10/92 */ 
  int vali/*,coordinate*/,*ip/*,*ipnew*/;   /* RS 30/10/92 */ 
  double valr,*rp/*,*rpnew*/;               /* RS 30/10/92 */ 
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_vc_min");

  START_MODUL("red_vc_min");

  if (T_POINTER(arg))
  {
    if (
         ((class = R_DESC(DESC(arg),class)) != C_VECTOR ) &&
         ( class                            != C_TVECTOR)
       )
      goto fail;

    if (class == C_VECTOR)
      dim = R_MVT(DESC(arg),ncols,C_VECTOR);
    else
      dim = R_MVT(DESC(arg),nrows,C_TVECTOR);

    if (dim == NULL) {     /* if argument is a nilvect or nitvect */
      INC_REFCNT(_dig0);   /* ==> result is 0                     */
      _desc = _dig0;
      goto success;
    }


  switch(R_DESC(DESC(arg),type))
  {
    case TY_INTEGER:

         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);

         vali = ip[0];

                for (i = 1; i < dim; ++i)
                  if (vali > ip[i])
                    vali = ip[i];

         REL_HEAP;
         _desc = (PTR_DESCRIPTOR)TAG_INT(vali) ;
         goto success ;
    case TY_REAL:
         NEWDESC(_desc); TEST_DESC;
         DESC_MASK(_desc,1,C_SCALAR,TY_REAL);

         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg),ptdv,class);

         valr = rp[0];

                for (i = 1; i < dim; ++i)
                  if (valr > rp[i])
                    valr = rp[i];

         REL_HEAP;
         L_SCALAR((*_desc),valr) = valr;
         goto success;
    case TY_DIGIT:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

                for (i = 1; i < dim; ++i)
                {
                  digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  if (digit_lt(digit1,_desc))
                  {
                    DEC_REFCNT(_desc);
                    _desc = digit1;
                    INC_REFCNT(_desc);
                  }
                }
         goto success;
    case TY_BOOL:
         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);
         vali = ip[0];

                for (i = 1; i < dim; ++i)
                  if (vali > ip[i])
                    vali = ip[i];

         REL_HEAP;
         _desc = (PTR_DESCRIPTOR) (vali ? SA_TRUE : SA_FALSE);
         goto success;
    case TY_STRING:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);


                RES_HEAP;
                for (i = 1; i < dim; ++i)
                {
                  digit1 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  if (str_lt(digit1,_desc))
                  {
                    DEC_REFCNT(_desc);
                    _desc = digit1;
                    INC_REFCNT(_desc);
                  }
                }
                REL_HEAP;
                goto success;
    default: goto fail;
  }  /* Ende von switch(type) */
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("red_vc_min");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg);
  END_MODUL("red_vc_min");
  DBUG_RETURN(1);

}

int red_vc_plus(arg)                 /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg;
{
  register int i/*,j*/;                     /* RS 30/10/92 */ 
  PTR_DESCRIPTOR digit1,digit2/*,digit3*/;  /* RS 30/10/92 */ 
  int /*nrows,ncols,*/dim/*,pos*/;          /* RS 30/10/92 */ 
  int vali/*,coordinate*/,*ip/*,*ipnew*/;   /* RS 30/10/92 */ 
  double valr,*rp/*,*rpnew*/;               /* RS 30/10/92 */ 
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_vc_plus");

  START_MODUL("red_vc_plus");

  if (T_POINTER(arg))
  {
    if (
         ((class = R_DESC(DESC(arg),class)) != C_VECTOR ) &&
         ( class                            != C_TVECTOR)
       )
      goto fail;

    if (class == C_VECTOR)
      dim = R_MVT(DESC(arg),ncols,C_VECTOR);
    else
      dim = R_MVT(DESC(arg),nrows,C_TVECTOR);

    if (dim == NULL) {     /* if argument is a nilvect or nitvect */
      INC_REFCNT(_dig0);   /* ==> result is 0                     */
      _desc = _dig0;
      goto success;
    }


  switch(R_DESC(DESC(arg),type))
  {
    case TY_INTEGER:

         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);

         vali = ip[0];

                for (i = 1; i < dim; ++i)
                  vali += ip[i];

         REL_HEAP;
         _desc = (PTR_DESCRIPTOR)TAG_INT(vali) ;
         goto success ;
    case TY_REAL:
         NEWDESC(_desc); TEST_DESC;
         DESC_MASK(_desc,1,C_SCALAR,TY_REAL);

         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg),ptdv,class);

         valr = rp[0];

                for (i = 1; i < dim; ++i)
                  valr += rp[i];

         REL_HEAP;
         L_SCALAR((*_desc),valr) = valr;
         goto success;
    case TY_DIGIT:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

                _digit_recycling = FALSE;
                for (i = 1; i < dim; ++i)
                {
                  digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  TEST(digit1 = DI_ADD(_desc,digit2));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                }
                _digit_recycling = TRUE;
         goto success;
    case TY_STRING:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
                goto fail;
    default: goto fail;
  }  /* Ende von switch(type) */
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("red_vc_plus");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg);
  END_MODUL("red_vc_plus");
  DBUG_RETURN(1);

}
  
int red_vc_minus(arg)                 /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg;
{
  register int i/*,j*/;                    /* RS 30/10/92 */ 
  PTR_DESCRIPTOR digit1,digit2/*,digit3*/; /* RS 30/10/92 */ 
  int /*nrows,ncols,*/dim/*,pos*/;         /* RS 30/10/92 */ 
  int vali,/*coordinate,*/*ip/*,*ipnew*/;  /* RS 30/10/92 */ 
  double valr,*rp/*,*rpnew*/;              /* RS 30/10/92 */ 
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_vc_minus");

  START_MODUL("red_vc_minus");

  if (T_POINTER(arg))
  {
    if (
         ((class = R_DESC(DESC(arg),class)) != C_VECTOR ) &&
         ( class                            != C_TVECTOR)
       )
      goto fail;

    if (class == C_VECTOR)
      dim = R_MVT(DESC(arg),ncols,C_VECTOR);
    else
      dim = R_MVT(DESC(arg),nrows,C_TVECTOR);

    if (dim == NULL) {     /* if argument is a nilvect or nitvect */
      INC_REFCNT(_dig0);   /* ==> result is 0                     */
      _desc = _dig0;
      goto success;
    }


  switch(R_DESC(DESC(arg),type))
  {
    case TY_INTEGER:

         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);

         vali = ip[0];

                for (i = 1; i < dim; ++i)
                {
                  vali -= ip[i];
                  if (++i >= dim) break;
                  vali += ip[i];
                }

         REL_HEAP;
         _desc = (PTR_DESCRIPTOR)TAG_INT(vali) ;
         goto success ;
    case TY_REAL:
         NEWDESC(_desc); TEST_DESC;
         DESC_MASK(_desc,1,C_SCALAR,TY_REAL);

         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg),ptdv,class);

         valr = rp[0];

                for (i = 1; i < dim; ++i)
                {
                  valr -= rp[i];
                  if (++i >= dim) break;
                  valr += rp[i];
                }

         REL_HEAP;
         L_SCALAR((*_desc),valr) = valr;
         goto success;
    case TY_DIGIT:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

                _digit_recycling = FALSE;
                for (i = 1; i < dim; ++i)
                {
                  digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  TEST(digit1 = DI_SUB(_desc,digit2));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                  /* new by RS in June 1993 */
                  digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[++i];
                  TEST(digit1 = DI_ADD(_desc,digit2));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                }
                _digit_recycling = TRUE;
         goto success;
    case TY_STRING:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
                goto fail;
    default: goto fail;
  }  /* Ende von switch(type) */
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("red_vc_minus");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg);
  END_MODUL("red_vc_minus");
  DBUG_RETURN(1);

}

int red_vc_mult(arg)                /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg;
{
  register int i/*,j*/;                      /* RS 30/10/92 */  
  PTR_DESCRIPTOR digit1,digit2/*,digit3*/;   /* RS 30/10/92 */ 
  int /*nrows,ncols,*/dim/*,pos*/;           /* RS 30/10/92 */ 
  int vali,/*coordinate,*/*ip/*,*ipnew*/;    /* RS 30/10/92 */ 
  double valr,*rp/*,*rpnew*/;                /* RS 30/10/92 */ 
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_vc_mult");

  START_MODUL("red_vc_mult");

  if (T_POINTER(arg))
  {
    if (
         ((class = R_DESC(DESC(arg),class)) != C_VECTOR ) &&
         ( class                            != C_TVECTOR)
       )
      goto fail;

    if (class == C_VECTOR)
      dim = R_MVT(DESC(arg),ncols,C_VECTOR);
    else
      dim = R_MVT(DESC(arg),nrows,C_TVECTOR);

    if (dim == NULL) {     /* if argument is a nilvect or nitvect */
      INC_REFCNT(_dig0);   /* ==> result is 0                     */
      _desc = _dig0;
      goto success;
    }


  switch(R_DESC(DESC(arg),type))
  {
    case TY_INTEGER:

         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);

         vali = ip[0];

                for (i = 1; i < dim; ++i)
                  vali *= ip[i];

         REL_HEAP;
         _desc = (PTR_DESCRIPTOR)TAG_INT(vali) ;
         goto success ;
    case TY_REAL:
         NEWDESC(_desc); TEST_DESC;
         DESC_MASK(_desc,1,C_SCALAR,TY_REAL);

         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg),ptdv,class);

         valr = rp[0];

                for (i = 1; i < dim; ++i)
                  valr *= rp[i];

         REL_HEAP;
         L_SCALAR((*_desc),valr) = valr;
         goto success;
    case TY_DIGIT:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

                _digit_recycling = FALSE;
                for (i = 1; i < dim; ++i)
                {
                  digit2 = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[i];
                  TEST(digit1 = digit_mul(_desc,digit2));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                }
                _digit_recycling = TRUE;
         goto success;
    case TY_STRING:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
                goto fail;
    default: goto fail;
  }  /* Ende von switch(type) */
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("red_vc_mult");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg);
  END_MODUL("red_vc_mult");
  DBUG_RETURN(1);

}

int red_vc_div(arg)                             /* int eingefuegt von RS 30/10/92 */ 
register STACKELEM arg;
{
  register int i/*,j*/;                       /* RS 30/10/92 */ 
  PTR_DESCRIPTOR digit1/*,digit2,digit3*/;    /* RS 30/10/92 */   
  int /*nrows,ncols,*/dim/*,pos*/;            /* RS 30/10/92 */ 
  int vali,/*coordinate,*/*ip/*,*ipnew*/;     /* RS 30/10/92 */ 
  double valr,*rp/*,*rpnew*/;                 /* RS 30/10/92 */ 
  DESC_CLASS class;                            /*    Klasse des Resultats    */

  DBUG_ENTER ("red_vc_div");

  START_MODUL("red_vc_div");

  if (T_POINTER(arg))
  {
    if (
         ((class = R_DESC(DESC(arg),class)) != C_VECTOR ) &&
         ( class                            != C_TVECTOR)
       )
      goto fail;

    if (class == C_VECTOR)
      dim = R_MVT(DESC(arg),ncols,C_VECTOR);
    else
      dim = R_MVT(DESC(arg),nrows,C_TVECTOR);

    if (dim == NULL) {     /* if argument is a nilvect or nitvect */
      INC_REFCNT(_dig0);   /* ==> result is 0                     */
      _desc = _dig0;
      goto success;
    }


  switch(R_DESC(DESC(arg),type))
  {
    case TY_INTEGER:

         RES_HEAP;
         ip = (int *) R_MVT(DESC(arg),ptdv,class);

         vali = ip[0];

                DESC_MASK(_desc,1,C_SCALAR,TY_REAL);
                valr = (double) vali;
                for (i = 1; i < dim; ++i)
                {
                  valr /= (double) ip[i];
                  if (++i >= dim) break;
                  valr *= (double) ip[i];
                }
                REL_HEAP;
                L_SCALAR((*_desc),valr) = valr;
                goto success;
    case TY_REAL:
         NEWDESC(_desc); TEST_DESC;
         DESC_MASK(_desc,1,C_SCALAR,TY_REAL);

         RES_HEAP;
         rp = (double *) R_MVT(DESC(arg),ptdv,class);

         valr = rp[0];

                for (i = 1; i < dim; ++i)
                {
                  valr /= rp[i];
                  if (++i >= dim) break;
                  valr *= rp[i];
                }

         REL_HEAP;
         L_SCALAR((*_desc),valr) = valr;
         goto success;
    case TY_DIGIT:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
         INC_REFCNT(_desc);

                _digit_recycling = FALSE;
                for (i = 1; i < dim; ++i)
                {
                  if ((digit1 = digit_div(_desc,R_MVT(DESC(arg),ptdv,class)[i])) == 0)
                  {
                    _digit_recycling = TRUE;
                    DEC_REFCNT(_desc);
                    goto fail;
                  }
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                  if (++i >= dim) goto success;
                  TEST(digit1 = digit_mul(_desc,R_MVT(DESC(arg),ptdv,class)[i]));
                  DEC_REFCNT(_desc);
                  _desc = digit1;
                }
                _digit_recycling = TRUE;
         goto success;
    case TY_STRING:
         _desc = (PTR_DESCRIPTOR) R_MVT(DESC(arg),ptdv,class)[0];
                goto fail;
    default: goto fail;
  }  /* Ende von switch(type) */
  }  /* Ende von T_POINTER */
fail:
  END_MODUL("red_vc_div");
  DBUG_RETURN(0);

success:
  DEC_REFCNT((PTR_DESCRIPTOR)arg);
  END_MODUL("red_vc_div");
  DBUG_RETURN(1);

}


/*------------------------------------------------------------------------
 * red_to_char --                                       stt 28.02.96
 *
 * Anwendung:                char(argument)
 *
 * Funktion:
 *
 *    char ist nur auf ganze Zahlen im Bereich 0 bis 255 anwendbar.
 *    Zurueckgeliefert wird ein einelementiger String mit gleichem 
 *    ASCII-Wert. Ist dieser nicht druckbar, so zeigt der Editor seine
 *    Octaldarstellung an (z.B. '\012`). Der String ist aber in jedem Fall
 *    einelementig.
 *
 *------------------------------------------------------------------------
 */
int red_to_char(register STACKELEM arg1)
{
  int ret = 0;
  char s[2];
  
  DBUG_ENTER ("red_to_char");
  START_MODUL("red_to_char");
  
  if ( T_INT(arg1) &&
       (GET_INT(arg1) >= 0) &&
       (GET_INT(arg1) < 255) )
  {
    s[0] = GET_INT(arg1);
    s[1] = '\0';
    MAKESTRDESC(_desc, s);
    ret = 1;
  }

  END_MODUL("red_to_char");
  DBUG_RETURN(ret);
}

/*------------------------------------------------------------------------
 * red_to_ord --                                       stt 28.02.96
 *
 * Anwendung:                ord(argument)
 *
 * Funktion:
 *
 *    ord ist nur auf einen einelementigen String anwendbar.
 *    Zurueckgeliefert wird eine ganze Zahl im Bereich 0 bis 255, die dem
 *    ASCII-Wert des Charakters entspricht.
 *
 *------------------------------------------------------------------------
 */
int red_to_ord(register STACKELEM arg1)
{
  int ret = 0;
  int val;
  PTR_HEAPELEM  ptdv;
  
  DBUG_ENTER ("red_to_ord");
  START_MODUL("red_to_ord");
  
  if ( T_POINTER(arg1) &&
       (R_DESC(DESC(arg1),class) == C_LIST) &&
       (R_DESC(DESC(arg1),type)  == TY_STRING) &&
       (R_LIST(DESC(arg1),dim) == 1) )
  {
    ptdv = R_LIST(DESC(arg1),ptdv);
    if (T_STR_CHAR(ptdv[0]))
    {
      val = (int) VALUE(ptdv[0]);
      _desc = (PTR_DESCRIPTOR) TAG_INT(val);
      DEC_REFCNT((PTR_DESCRIPTOR)arg1);
      ret = 1;
    }
  }

  END_MODUL("red_to_ord");
  DBUG_RETURN(ret);
}

/*------------------------------------------------------------------------
 * red_sprintf --                                       stt 28.02.96
 *
 * Anwendung:         sprintf(Formatstring, arg)
 *
 * Funktion:
 *
 *      Soll das sprintf von C nachbilden. 2.Argument ist dann eine Liste
 *      mit der zum Formatstring passenden Anzahl von Argumenten. Resultat
 *      ist ein String.
 *      Zunaechst wird aber nur die Umwandlung einer Zahl in einen String
 *      ermoeglicht, d.h. arg muss vom Typ Integer oder Real sein und der
 *      Formatstring darf nur genau ein Pattern enthalten. Dabei wird das
 *      '%' automatisch vorgehaengt, d.h. der User muss/darf es nicht
 *      angeben.
 *
 *      Syntax des Formatstring:  '[-+ ]DIGITS[.]DIGITS{diouxXfeEgG}`
 *
 *      Dabei ist: DIGITS ein evt. leerer String aus '0'..'9'
 *                 [ab..] Optional a oder b oder ...
 *                 {ab..} a oder b oder ...
 *      Wenn als Typ {diouxX} angegeben wird, dann muss arg ein Interger
 *      sein, sonst ein Real.
 *
 *------------------------------------------------------------------------
 */
int red_sprintf(register STACKELEM fs, register STACKELEM arg)
{
  int ret = 0, len;
  int contens;   /* 0: fs not valid, 1: int Pattern, 2: double Pattern */
  char s[200], format[12], *fstmp;
  
  DBUG_ENTER ("red_sprintf");
  START_MODUL("red_sprintf");
  
  if ( T_POINTER(fs) &&
      (R_DESC(DESC(fs),class) == C_LIST) &&
      (R_DESC(DESC(fs),type)  == TY_STRING) )
  {
    GETSTRING(fstmp, (PTR_DESCRIPTOR) fs);
    if (fstmp != NULL)
    {
      len = strlen(fstmp);
      if (len < 10)
      {
        strcpy(format, "%");
        strcat(format, fstmp);
        contens = test_format(format, 1);

        if (contens == 1 &&
            T_INT(arg))
        {
          sprintf(s, format, (int)GET_INT(arg));
          MAKESTRDESC(_desc, s);
          DEC_REFCNT((PTR_DESCRIPTOR)fs);
          ret = 1;
        }
        else if ( contens == 2 &&
                  T_POINTER(arg) &&
                  (R_DESC(DESC(arg),class) == C_SCALAR) &&
                  (R_DESC(DESC(arg),type)  == TY_REAL) )
        {
          format[len+2] = '\0';         /* Insert l for double value */
          format[len+1] = format[len];
          format[len] = 'l';
          sprintf(s, format, (double)R_SCALAR(DESC(arg),valr));
          MAKESTRDESC(_desc, s);
          DEC_REFCNT((PTR_DESCRIPTOR)fs);
          DEC_REFCNT((PTR_DESCRIPTOR)arg);
          ret = 1;
        }
      }
      free(fstmp);
    }
  }

  END_MODUL("red_sprintf");
  DBUG_RETURN(ret);
}

/*------------------------------------------------------------------------
 * red_sscanf --                                       stt 28.02.96
 *
 * Anwendung:         sscanf(Formatstring, Quellstring)
 *
 * Funktion:
 *
 *      Soll das sscanf von C nachbilden. Resultat ist eine dem Formatstring
 *      entsprechende Liste von Zahlen und Strings, die aus Quellstring
 *      ausgelesen werden.
 *      Zunaechst wird aber nur die Umwandlung eines Strings in eine Zahl
 *      ermoeglicht, d.h. der Formatstring darf nur genau ein Pattern
 *      enthalten und Resultat ist ein Integer- oder Real-Wert. Dabei wird
 *      das '%' automatisch vorgehaengt, d.h. der User muss/darf es nicht
 *      angeben.
 *
 *      Syntax des Formatstring:  'DIGITS{diouxfeg}`
 *
 *      Dabei ist: DIGITS ein evt. leerer String aus '0'..'9'
 *                 {ab..} a oder b oder ...
 *      Wenn als Typ {dioux} angegeben wird, dann ist das Resultat ein
 *      Interger, sonst ein Real.
 *
 *------------------------------------------------------------------------
 */
int red_sscanf(register STACKELEM fs, register STACKELEM arg)
{
  int ret = 0, len;
  int contens;   /* 0: fs not valid, 1: int Pattern, 2: double Pattern */
  int vali;
  double valr;
  char *argstr, format[12], *fstmp;
  
  DBUG_ENTER ("red_sscanf");
  START_MODUL("red_sscanf");
  
  if ( T_POINTER(fs) &&
       (R_DESC(DESC(fs),class) == C_LIST) &&
       (R_DESC(DESC(fs),type)  == TY_STRING) &&
       T_POINTER(arg) &&
       (R_DESC(DESC(arg),class) == C_LIST) &&
       (R_DESC(DESC(arg),type)  == TY_STRING) )
  {
    GETSTRING(fstmp, (PTR_DESCRIPTOR) fs);
    GETSTRING(argstr, (PTR_DESCRIPTOR) arg);
    if (fstmp != NULL && argstr != NULL)
    {
      len = strlen(fstmp);
      if (len < 10)
      {
        strcpy(format, "%");
        strcat(format, fstmp);
        contens = test_format(format, 0);

        if (contens == 1)
        {
          len = sscanf(argstr, format, &vali);
          if (len == 1)
          {
            _desc = (PTR_DESCRIPTOR) TAG_INT(vali);
            DEC_REFCNT((PTR_DESCRIPTOR)fs);
            DEC_REFCNT((PTR_DESCRIPTOR)arg);
            ret = 1;
          }
        }
        else if (contens == 2)
        {
          format[len+2] = '\0';         /* Insert l for double value */
          format[len+1] = format[len];
          format[len] = 'l';
          len = sscanf(argstr, format, &valr);
          if (len == 1)
          {
            MAKEDESC(_desc, 1, C_SCALAR, TY_REAL);
            L_SCALAR(*_desc, valr) = valr;
            DEC_REFCNT((PTR_DESCRIPTOR)fs);
            DEC_REFCNT((PTR_DESCRIPTOR)arg);
            ret = 1;
          }
        }
      }
      free(fstmp);
      free(argstr);
    }
  }

  END_MODUL("red_sscanf");
  DBUG_RETURN(ret);
}

/*****************************  end of file rvalfunc.c  **********************/
