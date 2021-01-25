/* $Log: rhinout.c,v $
 * Revision 1.10  1996/02/08 17:14:31  stt
 * latest bugfix for 0-ary interactions removed because
 * they are no longer put into descriptors. (intact(ia))
 *
 * Revision 1.9  1996/01/08  15:36:59  stt
 * error with 0-ary interactions fixed
 *
 * Revision 1.8  1995/12/11  18:25:10  cr
 * support for empty frames: NILSTRUCT
 *
 * Revision 1.7  1995/11/20  17:07:42  cr
 * ret_slot: INC_REFCNT(..name..)
 *
 * Revision 1.6  1995/11/15  15:04:10  cr
 * postprocessing for frames (ret_frame) and slots (ret_slot)
 *
 * Revision 1.5  1995/09/12  14:30:03  stt
 * interactions supported
 *
 * Revision 1.4  1994/03/08  18:19:21  mah
 * *** empty log message ***
 *
 * Revision 1.3  1994/01/04  11:52:24  mah
 * ret_lname, ret_fname added
 * ret_var excluded for tilde postprocessor
 *
 * Revision 1.2  1992/12/16  12:50:01  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*****************************************************************************/
/*     MODUL FOR  PREPROCESSING- / PROCESSING- / POSTPROCESSING - PHASE      */
/* ------------------------------------------------------------------------- */
/* rhinout:c - external: mvheap,      load_expr,    inc_expr,                */
/*                       ret_cond,    ret_dec,   ret_mvt     ret_expr,       */
/*                       ret_name,    ret_list,  ret_scal,   ret_var,        */
/*                       ret_frame,   ret_slot                               */
/*          - internal: none                                                 */
/*                                                                           */
/*            mvheap tritt in allen Phasen auf,                              */
/*            load_expr und ret_expr in Processing und Postprocessing        */
/*            der Rest im Postprocessing                                     */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"

/* mah 121093 fuer TILDE-praeprozessor */
#if WITHTILDE
  STACKELEM lname[5] = { 4194309 , 3014661 , 5242885 , 5373957 , 4521988 } ; 
#endif /* WITHTILDE */

/*-----------------------------------------------------------------------------
 * Spezielle externe Routinen:
 *-----------------------------------------------------------------------------
 */
extern int newheap();
extern void trav_hilf_a();
extern PTR_DESCRIPTOR newdesc();
extern int freeheap();                 /* heap:c */
extern void freedesc();                /* heap:c */
extern void convi();
extern void convr();
extern void ret_dec();                 /* rret-dec.c */
void ret_list();                       /* forward declaration */

/* RS 30/10/92 */ 
extern void stack_error();             /* rstack.c */
/* END of RS 30/10/92 */ 

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

#if DEBUG
extern void res_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void rel_heap(); /* TB, 4.11.1992 */        /* rheap.c */
extern void test_inc_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern void DescDump(); /* TB, 4.11.1992 */        /* rdesc.c */
extern void test_dec_refcnt(); /* TB, 4.11.1992 */ /* rheap.c */
extern STACKELEM *ppopstack(); /* TB, 4.11.1992 */ /* rstack.c */
#endif

#define TET  0                     /* was ist das ?  */

/*-----------------------------------------------------------------------------
 * mvheap  -- ist eine Funktion mit Tradition, Sie legt den Ausdruck auf dem
 *            Hilfstack in den Heap. Die Laenge dieses Ausdrucks legt sie an
 *            den Anfang des Heapbereichs.
 *            Auf "desc_entry" wird die Adresse des Heapbereichs abgelegt
 *            in dem die Daten nun stehen.
 *            Ist der Heap voll, so liefert mvheap eine 0 zurueck, eine 1 sonst.
 * globals -- S_hilf        <w>
 *-----------------------------------------------------------------------------
 */
int mvheap(desc_entry)
int  * desc_entry;
{
  register PTR_HEAPELEM p;
  register int          i;
  int      size = SIZEOFSTACK(S_hilf);

  if (!newheap(1 + size,desc_entry))
  {
    trav_hilf_a();
    return(0);
  }
  p = (PTR_HEAPELEM) *desc_entry;

  for (i = 1; i <= size; )
    p[i++] = (T_HEAPELEM) POPSTACK(S_hilf);

  p[0] = (T_HEAPELEM) (size + TET);
  return(1);
}

/*---------------------------------------------------------------------------
 * load_expr -- kopiert einen Heapausdruck auf den E-Stack.
 *              Der Parameter pptr ist eine Adresse in einem Deskriptor,
 *              in der ein Zeiger auf einen Heapausdruck mit
 *              vorangestellter Laenge.
 * called by -- ret_cond, ear, ua.
 *--------------------------------------------------------------------------
 */

void load_expr(pptr)
STACKELEM ** pptr;
{
  register int          size;
  register STACKELEM       x;
  register STACKELEM    *ptr;

  START_MODUL("load_expr");
#if MEASURE
  measure(LOAD_EXPR);
#endif

  RES_HEAP;
  ptr = *pptr;
  size = ptr[0] - TET;

  IS_SPACE(S_e,size);

  while (size > 0)
  {
    x = ptr[size];
    if (T_POINTER(x))
      INC_REFCNT((PTR_DESCRIPTOR)x);
    PPUSHSTACK(S_e,x);
    size--;
  }
  REL_HEAP;

  END_MODUL("load_expr");
}

/*-----------------------------------------------------------------------------
 *  inc_expr   --  bearbeitet einen Ausdruck auf dem A-Stack. Handelt es sich
 *                 um ein einzelnes Stackelement, so wird er zurueckgegeben,
 *                 und im Fall einer Nummervariable um increment erhoeht.
 *                 Ein Expressionpointer wird auf das Nicht-Vorhandensein von
 *                 freien Variablen ueberprueft.
 *                 Ansonsten wird der Ausdruck ausgelagert und der Pointer
 *                 zurueckgegeben. Falls er hier eine relativ freie Variable
 *                 findet, gibt eine Fehlermeldung, da dies nicht passieren soll.
 *  globals    --  bindings    <w>
 *-----------------------------------------------------------------------------
 */

/**********************************************************************
  Man koennte die Funktion auch so schreiben:
STACKELEM inc_expr(increment)
int increment;
{
  STACKELEM x;

  if T_CON(READSTACK(S_a))
  {
    x = st_expr();
    if (x == NULL) post_mortem("inc_expr: Heap out of Space");
    return(x);
  }
  return( (T_NUM(x = POPSTACK(S_a)) ? SET_VALUE(x, VALUE(x) + increment) : x));
}
  Wir sind uns aber nicht sicher ob wir dann nicht evt. eine Nummernvariable
  verpassen.
*******************************************************************/
/* Hier ueberpruefen wir noch unsere Annahme !!! */
 STACKELEM inc_expr(increment)
 int increment;
 {
    STACKELEM x;
    PTR_DESCRIPTOR desc;
    STACKELEM * p;
    int i;

    START_MODUL("inc_expr");
    x = POPSTACK(S_a);

    if (T_POINTER(x) &&  R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_EXPR)
    {
       RES_HEAP;
       p = (STACKELEM *) R_EXPR((* (PTR_DESCRIPTOR)x),pte);
       for (i = (int)p[0] - TET; i>0; i--)
          if T_NUM(p[i])  post_mortem("inc_expr: freie Variable hinter Expressionpointer");
       REL_HEAP;
    }

    if (!T_CON(x))
    {
       END_MODUL("inc_expr");
       return( (T_NUM(x) ? SET_VALUE(x, VALUE(x) + increment) : x));
    }

    if ((desc = newdesc()) == NULL)
       post_mortem("inc_expr: Heap out of Space");
    DESC_MASK(desc,1,C_EXPRESSION,TY_EXPR);

    PUSHSTACK(S_m,KLAA);
    PUSHSTACK(S_m,x);
    PUSHSTACK(S_m1,x);

    main_a:
    x = POPSTACK(S_a);

    if (T_NUM(x))
    {
       post_mortem("inc_expr: Nummernvariable hinter Konstruktor gefunden");
    }
    if (T_CON(x))
    {
       PUSHSTACK(S_m,x);
       PUSHSTACK(S_m1,x);
       goto main_a;
    }
    else
    {
       PUSHSTACK(S_hilf,x);
    }


    main_m:
    x = READSTACK(S_m);

    if (T_CON(x))
    {
       if (ARITY(x) > 1)
       {
          WRITESTACK(S_m,DEC(x));
          goto main_a;
       }
       else
       {
          PPOPSTACK(S_m);
          x = POPSTACK(S_m1);
          PUSHSTACK(S_hilf,x);
          goto main_m;
       }
    }

    if (T_KLAA(x))
    {
       PPOPSTACK(S_m);   /* KLAA */
       if (mvheap((int *)A_EXPR((*desc),pte)) == 0)  /* int * by RS 30.11.1992 */
         post_mortem("inc_expr: Heap out of space");
       END_MODUL("inc_expr");
       return((STACKELEM)desc);
    }

    post_mortem("inc_expr main_m: no match");
    return (NULL);              /* RS 30/10/92 */ 
 }


 /*-----------------------------------------------------------------------------
   * ret_cond -- erhaelt einen Descriptor, der ein Expressiondesriptor sein
   *             sein sollte als Parameter. Der hinter diesem Descriptor
   *             liegende Ausdruck wird auf den E-Stack gelegt.
   * globals --  S_e           <w>
   *-----------------------------------------------------------------------------
   */
 void ret_cond(pdesc)
    STACKELEM pdesc;
 {
    PTR_DESCRIPTOR     desc = (PTR_DESCRIPTOR) pdesc;
    int cond;

    START_MODUL("ret_cond");

    load_expr((STACKELEM **)A_COND((*desc),ptee));
    load_expr((STACKELEM **)A_COND((*desc),ptte));

    cond = (int) R_COND((*desc),special);       /*  SJ-ID */
    cond = COND | SET_EDIT(cond);  /* SJ-ID  setze Edit-Feld in Stackelement */

    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(cond,2));

    DEC_REFCNT(desc);
    END_MODUL("ret_cond");
 }


 /*-----------------------------------------------------------------------------
   * ret_expr -- erhaelt einen Descriptor, der ein Expressiondesriptor sein
   *             sein sollte als Parameter. Der hinter diesem Descriptor
   *             liegende Ausdruck wird auf den E-Stack gelegt.
   * globals --  S_e           <w>
   * called by -- reduct, ear, ea_retrieve.
   *-----------------------------------------------------------------------------
   */
 void ret_expr(pdesc)
    STACKELEM pdesc;
 {
    PTR_DESCRIPTOR     desc = (PTR_DESCRIPTOR) pdesc;
    register STACKELEM *pte ;
    register int          i ;

    START_MODUL("ret_expr");

    RES_HEAP;
    pte = (STACKELEM *) R_EXPR((*desc),pte);
    i = pte[0] - TET;
    IS_SPACE(S_e,i);

    if (R_DESC((*desc),ref_count) == 1)
    {
           /* in diesem Fall kann man sich die Refcountbehandlung sparen. */
       for ( ; i > 0; )
          PPUSHSTACK(S_e,pte[i--]);
       REL_HEAP;
       freeheap((PTR_HEAPELEM)pte);
       freedesc(desc);
    }
    else
    {
       for ( ; i > 0; )
       {
          if (T_POINTER(pte[i])) INC_REFCNT(((PTR_DESCRIPTOR)pte[i]));
          PPUSHSTACK(S_e,pte[i--]);
       }
       REL_HEAP;
       DEC_REFCNT(desc);
    }
    END_MODUL("ret_expr");
 } /**** end of function ret_expr ****/

/*****************************************************************************/
/*         ret_mvt  (retrieve   matrix or vector or tvector descriptor)      */
/* ------------------------------------------------------------------------- */
/* function  :  erhaelt einen Matrix bzw. Vektor bzw. einen TVektor          */
/*              Deskriptor. Die Matrix bzw. der Vektor bzw. der TVektor      */
/*              wird auf dem E-Stack aufgebaut.                              */
/*              Die Funktion ist rekursiv.                                   */
/*                                                                           */
/* globals   :  S_e       <w>,                                               */
/* parameter :  desc    - descriptor                                         */
/*              class1  - C_MATRIX oder C_VECTOR oder C_TVECTOR              */
/* called by :  ret_mvt , rea-ret.c , lfun-def.c                             */
/*****************************************************************************/
void ret_mvt(desc,class1)
PTR_DESCRIPTOR desc;
char           class1;                                /* class of descriptor */
{
  register int   i,row,col,nrows,ncols;
  STACKELEM      type = (STACKELEM) 0;     /* Initialisierung von RS 6.11.1992 */
  PTR_DESCRIPTOR element;

  START_MODUL("ret_mvt");

  if (class1 == C_TVECTOR) {
   ncols = R_MVT((*desc),nrows,class1);
   nrows = R_MVT((*desc),ncols,class1);
  }
  else {
   nrows = R_MVT((*desc),nrows,class1);        /* class C_MATRIX und C_VECTOR */
   ncols = R_MVT((*desc),ncols,class1);
  }

  i = ncols * nrows ;

  if (i == 0) {
    switch(class1) {
      case C_MATRIX  : PUSHSTACK(S_e,NILMAT);
                       break;
      case C_VECTOR  : PUSHSTACK(S_e,NILVECT);
                       break;
      case C_TVECTOR : PUSHSTACK(S_e,NILTVECT);
                       break;
    }
    DEC_REFCNT(desc);
    END_MODUL("ret_mvt");
    return;
  }
  switch (R_DESC((*desc),type)) {
    case TY_INTEGER:
         for ( row = nrows; --row >= 0; ) {
           for (col = ncols; --col >= 0; )
             convi(R_MVT((*desc),ptdv,class1)[--i]);
                                            /* Integer auf den Stack legen */
           PUSHSTACK(S_e,SET_ARITY(LIST,ncols));
         }
         type = DECIMAL;
         break;
    case TY_REAL:
         for ( row = nrows; --row >= 0; ) {
           for (col = ncols; --col >= 0; ) /* Double auf den Stack legen: */
             convr(((double *)R_MVT((*desc),ptdv,class1))[--i]);
           PUSHSTACK(S_e,SET_ARITY(LIST,ncols));
         }
         type = DECIMAL;
         break;
    case TY_DIGIT:
         for ( row = nrows; --row >= 0; ) {
           for (col = ncols; --col >= 0; ) {
             element = (PTR_DESCRIPTOR)R_MVT((*desc),ptdv,class1)[--i];
             INC_REFCNT(element);
             ret_dec(element);
           }
           PUSHSTACK(S_e,SET_ARITY(LIST,ncols));
         }
         type = DECIMAL;
         break;
    case TY_BOOL:
         for ( row = nrows; --row >= 0; ) {
           for (col = ncols; --col >= 0; )
             if (R_MVT((*desc),ptdv,class1)[--i] == FALSE)
               PUSHSTACK(S_e,SA_FALSE);
             else
               PUSHSTACK(S_e,SA_TRUE);
           PUSHSTACK(S_e,SET_ARITY(LIST,ncols));
         }
         type = BOOL;
         break;
    case TY_STRING:
         for ( row = nrows; --row >= 0; ) {
           for (col = ncols; --col >= 0; ) {
             i--;
             INC_REFCNT((PTR_DESCRIPTOR)R_MVT((*desc),ptdv,class1)[i]);
             ret_list((R_MVT((*desc),ptdv,class1)[i]));
           }
           PUSHSTACK(S_e,SET_ARITY(LIST,ncols));
         }
         type = CHAR;
         break;
    case TY_MATRIX:
         for ( row = nrows; --row >= 0; ) {
           for (col = ncols; --col >= 0; ) {
             element = (PTR_DESCRIPTOR)R_MVT((*desc),ptdv,class1)[--i];
             INC_REFCNT(element);
             ret_mvt(element,class1);            /* recursive function call */
           }
           PUSHSTACK(S_e,SET_ARITY(LIST,ncols));
         }
         type = TMATRIX;
         break;
    default: switch(class1) {
               case C_MATRIX  :  post_mortem("ret_mvt: unknown matrix - type");
                                 break;
               case C_VECTOR  :  post_mortem("ret_mvt: unknown vector - type");
                                 break;
               case C_TVECTOR :  post_mortem("ret_mvt: unknown tvector - type");
                                 break;
             }
  }

  PUSHSTACK(S_e,type);
  switch(class1) {
     case C_MATRIX  :  PUSHSTACK(S_e,SET_ARITY(MATRIX,nrows+1));
                       break;
     case C_VECTOR  :  PUSHSTACK(S_e,SET_ARITY(VECTOR,nrows+1));
                       break;
     case C_TVECTOR :  PUSHSTACK(S_e,SET_ARITY(TVECTOR,nrows+1));
                       break;
  }
  DEC_REFCNT(desc);
  END_MODUL("ret_mvt");
} /**** end of function ret_mvt ****/



 /*-----------------------------------------------------------------------------
   * ret_name -- erhaelt einen Descriptor, der ein Namensdesriptor sein
   *             sein sollte als Paramter. Der hinter diesem Descriptor
   *             liegende Name wird auf den E-Stack gelegt.
   * globals -- S_e           <w>
   *-----------------------------------------------------------------------------
   */
 void ret_name(desc)
 PTR_DESCRIPTOR desc;
 {
    register int i;
    STACKELEM y;                                                       /* CS */
    extern STACKELEM _sep_int,_sep_ext;                                /* CS */

    START_MODUL("ret_name");
    RES_HEAP;
    for ( i = R_NAME((*desc),ptn)[0]; i > 0; i--)                      /* CS */
    {                                                                  /* CS */
       y = R_NAME((*desc),ptn)[i];                                     /* CS */
       if (y == _sep_int)                                              /* CS */
          PUSHSTACK(S_e,_sep_ext);                                     /* CS */
       else                                                            /* CS */
          PUSHSTACK(S_e,y);                                            /* CS */
    }                                                                  /* CS */
    REL_HEAP;

    DEC_REFCNT(desc);
    END_MODUL("ret_name");
 }

#if WITHTILDE

/* mah 121093 fuer TILDE-postprozessor */
/************************************************************************/
/*                                                                      */
/************************************************************************/

void ret_lname(desc)
 PTR_DESCRIPTOR desc;     /* Namensdescriptor */
 {
    register int i,j; 
    STACKELEM y;                                                       /* CS */
    extern STACKELEM _sep_int,_sep_ext;                                /* CS */

    START_MODUL("ret_lname");   
   
    /* lege .PRE auf S_e */

    for ( i = 4 ; i > 0 ; i-- )
       PUSHSTACK(S_e,lname[i]);

    RES_HEAP;
    j = R_NAME((*desc),ptn)[0]; 
    i =( (int)R_NAME((*desc),ptn)[j] + 1 ) ;  
    y=( ( (STACKELEM)i ) & ( ~F_EDIT ) );
    PUSHSTACK(S_e,y);
    for ( i = (j - 1 ) ; i > 0; i--)                                   /* CS */
    {                                                                  /* CS */
       y = R_NAME((*desc),ptn)[i];                                     /* CS */

       if (y == _sep_int)                                              /* CS */
          PUSHSTACK(S_e,_sep_ext);                                     /* CS */
       else  
          PUSHSTACK(S_e,(STACKELEM)( y & (~F_EDIT) ) );      
    }                                                                  /* CS */
    REL_HEAP;

    DEC_REFCNT(desc); 

    /* lege @ auf S_e */

    PUSHSTACK(S_e ,lname[0]);

    END_MODUL("ret_lname");
 }

 /*-----------------------------------------------------------------------------
   * ret_fname -- erhaelt einen Descriptor, der ein Namensdesriptor sein
   *             sein sollte als Paramter. Der hinter diesem Descriptor
   *             liegende Name wird auf den E-Stack gelegt.
   * globals -- S_e           <w>
   *-----------------------------------------------------------------------------
   */
 void ret_fname(desc)
 PTR_DESCRIPTOR desc;
 {
    register int i;
    STACKELEM y;                                                       /* CS */
    extern STACKELEM _sep_int,_sep_ext;                                          

    START_MODUL("ret_fname");
    RES_HEAP;
    for ( i = R_FNAME((*desc),ptn)[0]; i > 0; i--)
    {                                                                                
       y = R_FNAME((*desc),ptn)[i];                                                 
       if (y == _sep_int)                                                            
          PUSHSTACK(S_e,_sep_ext);                                                  
       else                                                                         
          PUSHSTACK(S_e,y);                                                           
    }                                                                              
    REL_HEAP;                                          
    DEC_REFCNT(desc);

    END_MODUL("ret_fname");
  }

#endif /* WITHTILDE */

 /*-----------------------------------------------------------------------------
   * ret_scal -- erhaelt einen Descriptor, der ein Scalardesriptor sein
   *             sein sollte als Paramter. Der hinter diesem Descriptor
   *             liegende Scalar wird auf den E-Stack gelegt.
   * globals -- S_e       <w>,
   *-----------------------------------------------------------------------------
   */
 void ret_scal(pdesc)
    STACKELEM pdesc;
 {
    register PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) pdesc;

    START_MODUL("ret_scal");

    if (R_DESC((*desc),type) == TY_INTEGER)
    {
       convi(R_SCALAR((*desc),vali));
    }
    else
    {
       convr(R_SCALAR((*desc),valr));
    }

    DEC_REFCNT(desc);
    END_MODUL("ret_scal");
 }

 /*-----------------------------------------------------------------------------
   * ret_list -- erhaelt einen Descriptor, der ein Listendesriptor sein
   *             sein sollte als Paramter. Der hinter diesem Descriptor
   *             liegende Baum wird auf den E-Stack gelegt.
   * globals -- S_e           <w>
   *-----------------------------------------------------------------------------
   */
 void ret_list(pdesc)
    STACKELEM pdesc;
 {
    register PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) pdesc;
    /* PTR_DESCRIPTOR ptdd;                       RS 30/10/92 */ 
    register STACKELEM x;
    register int i,dim;
    int list;

    START_MODUL("ret_list");

    dim = R_LIST((*desc),dim);

    if (dim == 0) {
      if (desc == _nil)
        PUSHSTACK(S_e,NIL);
      else
        PUSHSTACK(S_e,NILSTRING);
      DEC_REFCNT(desc);
      END_MODUL("ret_list");
      return;
    }

    IS_SPACE(S_e,dim);
    RES_HEAP;
    for ( i = dim; i > 0; )
    {
      PPUSHSTACK(S_e,(x = R_LIST((*desc),ptdv)[--i]));
      if T_POINTER(x) INC_REFCNT((PTR_DESCRIPTOR)x);
    }
    REL_HEAP;

    if (R_DESC((*desc),type) == TY_STRING)
      list = STRING;
    else
      list = LIST;

    if (R_LIST((*desc),ptdd) != NULL)
      list = list | SET_EDIT( (int) R_LIST((*R_LIST((*desc),ptdd)),special));
    else
      list = list | SET_EDIT( (int) R_LIST((*desc),special));

    PUSHSTACK(S_e,SET_ARITY(list,dim));

    DEC_REFCNT(desc);
    END_MODUL("ret_list");
 }

/*-----------------------------------------------------------------------------
 * ret_slot  -- erhaelt einen Descriptor, der ein Slotdesriptor sein
 *              sein sollte als Paramter. Der hinter diesem Descriptor
 *              liegende Baum wird auf den E-Stack gelegt.
 *              cr 14.11.95
 * globals -- S_e           <w> 
 *-----------------------------------------------------------------------------
 */
void ret_slot(pdesc)
  STACKELEM pdesc;
{
  register PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) pdesc;
  register STACKELEM x;

  START_MODUL("ret_slot");

  IS_SPACE(S_e,4);  /* entry + string(2) + something */

  PPUSHSTACK(S_e,(x = R_SLOT((*desc),value)));
  if T_POINTER(x) INC_REFCNT((PTR_DESCRIPTOR)x);
  INC_REFCNT((PTR_DESCRIPTOR)R_SLOT((*desc),name));
  ret_list(R_SLOT((*desc),name));

  PUSHSTACK(S_e,SET_ARITY(ENTRY,2));

  DEC_REFCNT(desc);
  END_MODUL("ret_slot");
}

/*-----------------------------------------------------------------------------
 * ret_frame -- erhaelt einen Descriptor, der ein Framedesriptor sein
 *              sein sollte als Paramter. Der hinter diesem Descriptor
 *              liegende Baum wird auf den E-Stack gelegt.
 *              cr 14.11.95
 * globals -- S_e           <w> 
 *-----------------------------------------------------------------------------
 */
void ret_frame(pdesc)
  STACKELEM pdesc;
{
  register PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) pdesc;
  /* PTR_DESCRIPTOR ptdd;                       RS 30/10/92 */ 
  register STACKELEM x;
  register int i,dim;

  START_MODUL("ret_frame");

  dim = R_FRAME((*desc),dim);

  if (dim == 0) {
    PUSHSTACK(S_e,NILSTRUCT);
    DEC_REFCNT(desc);
    END_MODUL("ret_frame");
    return;
  }

  IS_SPACE(S_e,dim+2);

  PUSHSTACK(S_e,PM_END);

  RES_HEAP;
  for ( i = dim; i > 0; )
  {
    PPUSHSTACK(S_e,(x = R_FRAME((*desc),slots)[--i]));
    if T_POINTER(x) INC_REFCNT((PTR_DESCRIPTOR)x);
  }
  REL_HEAP;

  PUSHSTACK(S_e,SET_ARITY(STRUCT,dim+1));

  DEC_REFCNT(desc);
  END_MODUL("ret_frame");
}

 /*-----------------------------------------------------------------------------
   * ret_var -- erhaelt einen Descriptor, der ein Variablendesriptor
   *            sein sollte als Parameter. Der hinter diesem Descriptor
   *            liegende Namensdesriptor wird zusammen mit evtl. Schutzstrichen
   *            auf den E-Stack gelegt.
   * globals --  S_e           <w>
   *-----------------------------------------------------------------------------
   */
#if !WITHTILDE
void ret_var(pdesc)
    STACKELEM pdesc;
 {
    PTR_DESCRIPTOR     desc = (PTR_DESCRIPTOR) pdesc;
    register int     nlabar ;

    START_MODUL("ret_var");

/*     den Namensdeskriptor auf den E-Stack legen  */
    PUSHSTACK(S_e,(STACKELEM)R_VAR((*desc),ptnd));
    INC_REFCNT((PTR_DESCRIPTOR)READSTACK(S_e));

    for (nlabar = R_VAR((*desc),nlabar); --nlabar >= 0; )
       PUSHSTACK(S_e,SET_ARITY(PROTECT,1));

    DEC_REFCNT(desc);
    END_MODUL("ret_var");
 } /* auskommentiert fuer tilde wegen Redundanz mit ret_var in pos-postfunc.c */
   /* mah 131093 */
#endif /* !WITHTILDE */
 /*-----------------------------------------------------------------------------
   * ret_func -- erhaelt einen Descriptor, der ein Funktionsdescriptor
   *             sein sollte, als Parameter. Der hinter diesem Descriptor
   *             liegende Ausdruck wird auf den E-Stack gelegt.
   * globals --  S_e           <w>
   *-----------------------------------------------------------------------------
   */
 void ret_func(pdesc)
    STACKELEM pdesc;
 {
    PTR_DESCRIPTOR     desc = (PTR_DESCRIPTOR) pdesc;
    register STACKELEM *pte ;
    register int          i ;

    START_MODUL("ret_func");

    RES_HEAP;
    pte = (STACKELEM *) R_FUNC((*desc),pte);
    i = pte[0] - TET;
    IS_SPACE(S_e,i);

    if (R_DESC((*desc),ref_count) == 1)
    {
           /* in diesem Fall kann man sich die Refcountbehandlung sparen. */
       for ( ; i > 0; )
          PPUSHSTACK(S_e,pte[i--]);
       REL_HEAP;
       freeheap((PTR_HEAPELEM)pte);
       freedesc(desc);
    }
    else
    {
       for ( ; i > 0; )
       {
          if (T_POINTER(pte[i])) INC_REFCNT(((PTR_DESCRIPTOR)pte[i]));
          PPUSHSTACK(S_e,pte[i--]);
       }
       REL_HEAP;
       DEC_REFCNT(desc);
    }
    END_MODUL("ret_func");
 }  /**** end of function ret_func ****/

/*--------------------------------------------------------------------------
 * ret_intact -- erhaelt einen Deskriptor, der ein Interaktionsdeskriptor
 *               sein sollte, als Parameter. Der hinter diesem Deskriptor
 *               liegende Ausdruck wird auf den E-Stack gelegt.
 *               stt  11.09.95
 * globals -- S_e <w>
 *--------------------------------------------------------------------------
 */

void ret_intact(STACKELEM pdesc)
{
  PTR_DESCRIPTOR     desc = (PTR_DESCRIPTOR) pdesc;
  register STACKELEM *args;
  register int         dim;
  register int           i;

  START_MODUL("ret_intact");

  RES_HEAP;
  args = (STACKELEM *) R_INTACT((*desc),args);
  dim = R_INTACT((*desc),dim);

  if (R_DESC((*desc),ref_count) == 1)
  {
    /* in diesem Fall kann man sich die Refcountbehandlung sparen. */
    for (i=0; i<dim; i++)
      PPUSHSTACK(S_e,args[i]);
    PPUSHSTACK(S_e,SET_ARITY(AP,dim));
    REL_HEAP;
    freeheap((PTR_HEAPELEM)args);
    freedesc(desc);
  }
  else
  {
    for (i=0; i<dim; i++)
    {
      if (T_POINTER(args[i]))
        INC_REFCNT(((PTR_DESCRIPTOR)args[i]));
      PPUSHSTACK(S_e,args[i]);
    }
    if (dim > 1)  /* kein Ap fuer nullstellige Interaktionen */
      PPUSHSTACK(S_e,SET_ARITY(AP,dim));
    REL_HEAP;
    DEC_REFCNT(desc);
  }
  END_MODUL("ret_intact");
}

/***************************  end of file  rhinout.c  ***********************/


