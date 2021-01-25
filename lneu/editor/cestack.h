/* include file fuer stack-handling            cestack.h */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    04.10.88                        */

typedef long STACKELEM;

struct astack
 { STACKELEM	*stptr;		/* stackpointer         */
   long		cnt;		/* counter		*/
   long		siz;		/* pagesize		*/
   STACKELEM	*pred;		/* predecessing page    */
 };

typedef struct astack STACK;
#define SNULL   (STACKELEM *)0
#define EST &E
#define AST &A
#define BST &B
#define MST &M
#define M1ST &M1
#define M2ST &M2
#define VST &V
#define U1ST &U
#define U2ST &U2
#define SST &S0
#define S1ST &S1
#define S2ST &S2
#define RST &R
#define R1ST &R1
#define R2ST &R2
#define EADRST &EADR
#define AADRST &AADR
#define XST &X
#define STKPAGE 256	/* alt: als int !		*/
/* vereinbarungen in globals.c		*/
extern STACK E,A,M,M1,M2,B,U,U2,V,S0,S1,S2,R,R1,R2,EADR,AADR;

#ifndef isstack
STACKELEM pop(),topof(),second(),move(),move2(),popnf(),movenf(),
          bottom()
#if !LETREC
          ,movech(),chmove(),chmov2()
#endif
          ;
void ppop(),push(),clearstack(),killstack(),incrarity(),decrarity(),pushi();
#endif
/* end of              cestack.h		*/

