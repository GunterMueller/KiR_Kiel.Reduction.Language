

/*-----------------------------------------------------------------------------
 * hinout:c - external: mvheap, load_expr, inc_expr und
 *                      ret_cond;ret_string,ret_dec,ret_expr,,
 *                      ret_name,ret_list,ret_scal,ret_var;
 *
 *                      mvheap tritt in allen Phasen auf,
 *                      load_expr und ret_expr in Processing und Postprocessing
 *                      der Rest im Postprocessing
 *
 * hinout:c - internal: keine;
 *-----------------------------------------------------------------------------
 */

/* last update  by     why
   31.05.88     sj     "edit" durch special ersetzt
                       wegen Aenderung des Edit-Feldes in heaptypes:h
                       typedef struct ST_COND  / ST_FIELD
                       SJ-ID
   01.05.88     sj     in ret_list wurde der Zugriff auf das Edit-Feld
                       mit READ_EDIT Makro durch  R_LIST Makro ersetzt.
                       Weiter ist eine Anpassung an die rheapty:h
                       erfolgt (special).             SJ-ID
   02.05.88     sj     Makro SET_EDIT  eingefuegt  (defined in rstelem:h)
                       SJ-ID

   copied       by     to

*/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"  
/**********************************************************/
   STACKELEM lname[5] = { 4194309 , 3014661 , 5242885 , 5373957 , 4521988 } ;                           
/************************************************************/
 
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
#if LARGE
extern void convr();
#endif
extern void ret_dec();                 /* rret-dec.c */
void ret_list();                       /* forward declaration */

extern PTR_DESCRIPTOR _desc;

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
    else  
    {
       if (T_NUM(x))    

          {
             READ_I(VALUE(x),x) ;
}
        else if (T_TILDE(x))
                READ_TILDE(VALUE(x),x);   
     }

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
       if (mvheap(A_EXPR((*desc),pte)) == 0)
         post_mortem("inc_expr: Heap out of space");
       END_MODUL("inc_expr");
       return((STACKELEM)desc);
    }

    post_mortem("inc_expr main_m: no match");
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
#if HEAP
    --i;
#endif
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
 }
 /*----------------------------------------------------------------------------- */
 /* ret_pexpr legt einen hinter einem Expression -Descriptor verborgenen         */
 /* bereits preproc. Ausruck auf den E - Stack                                   */
 /*------------------------------------------------------------------------------*/

 void ret_pexpr(desc)
 PTR_DESCRIPTOR  desc;
 {
    register STACKELEM *pte ;
    register int          i ;

    START_MODUL("ret_pexpr");

    RES_HEAP;
    pte = (STACKELEM *) R_EXPR((*desc),pte);
    i = pte[0] - TET;
    IS_SPACE(S_e,i);

    for ( ; i > 0; )
    {
      if (T_POINTER(pte[i])) INC_REFCNT(((PTR_DESCRIPTOR)pte[i]));
      PPUSHSTACK(S_e,pte[i--]);
    }
    REL_HEAP;
    END_MODUL("ret_pexpr");
 }

#if LARGE 
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
  STACKELEM      type;
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
#endif




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
 
/************************************************************************/
/*                                                                      */
/************************************************************************/

void ret_lname(desc)
 PTR_DESCRIPTOR desc;     /* Namensdescriptor */
 {
    register int i,j; 
    STACKELEM x,y;                                                       /* CS */
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
    PTR_DESCRIPTOR ptdd;
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
   * ret_match_list -- erhaelt einen Descriptor, der ein Listendesriptor sein
   *             sein sollte als Paramter. 
   * globals -- S_e           <w>
   *-----------------------------------------------------------------------------
   */
 void ret_match_list(pdesc)
    STACKELEM pdesc;
{
    register PTR_DESCRIPTOR desc = (PTR_DESCRIPTOR) pdesc;
    PTR_DESCRIPTOR ptdd;
    register STACKELEM x;
    register int i,dim;
    int list;

    START_MODUL("ret_match_list");

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
    for ( i = 0 ; i < dim;i++ )
    {
      PPUSHSTACK(S_e,(x = R_LIST((*desc),ptdv)[i]));
      if T_POINTER(x) INC_REFCNT((PTR_DESCRIPTOR)x);
    }
    REL_HEAP;

    if (R_DESC((*desc),type) == TY_STRING)
      list = STRING;
    else
      list = LIST;

    list = list | SET_EDIT( (int) R_LIST((*desc),special));    /* SJ-ID */

    PUSHSTACK(S_e,SET_ARITY(list,dim));

    DEC_REFCNT(desc);
    END_MODUL("ret_match_list");
  }

 /*-----------------------------------------------------------------------------
   * ret_var -- erhaelt einen Descriptor, der ein Variablendesriptor
   *            sein sollte als Parameter. Der hinter diesem Descriptor
   *            liegende Namensdesriptor wird zusammen mit evtl. Schutzstrichen
   *            auf den E-Stack gelegt.
   * globals --  S_e           <w>
   *-----------------------------------------------------------------------------
   */

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
 }


