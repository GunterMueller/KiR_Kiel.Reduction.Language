/* $Log: rstpre.c,v $
 * Revision 1.3  1993/12/31  11:12:09  mah
 * some additions for tilde preprocessor
 *
 * Revision 1.2  1992/12/16  12:51:09  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/*****************************************************************************/
/*                      MODULE FOR PREPROCESSING PHASE                       */
/*-------------------------------------------------------------------------- */
/* rstpre.c   - externals: st_string,  st_cond,  st_mvt,                     */
/*                         st_name,    st_var,   st_vare                     */
/*            - internals: none                                              */
/*****************************************************************************/

#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"


/*---------------------------------------------------------------------------*/
/*                          extern functions                                 */
/*---------------------------------------------------------------------------*/
extern PTR_DESCRIPTOR newdesc();   /* rheap.c   */
extern       int newheap();        /* rheap.c   */
extern      void trav_a_hilf();    /* rtrav.c   */
extern       int mvheap();         /* rhinout.c */
extern       int vali();           /* rval.c    */
extern    double valr();           /* rval.c    */
extern STACKELEM st_dec();         /* rst-dec.c */

/* RS 30/10/92 */
extern void stack_error();         /* rstack.c */
/* END of RS 30/10/92 */
/* ach 03/11/92 */

/* RS 6.11.1992 */ 
extern void disable_scav();                /* rscavenge.c */
extern void enable_scav();                 /* rscavenge.c */
/* END of RS 6.11.1992 */ 

#if DEBUG
extern void DescDump();
extern STACKELEM *ppopstack();
extern void test_inc_refcnt();
extern void test_dec_refcnt();
extern void res_heap();
extern void rel_heap();
#endif
/*end of ach */

/*---------------------------------------------------------------------------*/
/*                          extern variables                                 */
/*---------------------------------------------------------------------------*/
extern BOOLEAN   _formated;        /* rreduct.c */

/*****************************************************************************/
/*                    st_string      (store string)                          */
/* ------------------------------------------------------------------------- */
/* function  :  preprocessses possibly hierachical string on E-stack;        */
/* parameter :  x  - stackelement which has to be string constructor         */
/* returns   :  stackelement i.e. pointer to string descriptor               */
/* calls     :  st_string      (case string of string(s))                    */
/* called by :  eac            (in reac.c)                                   */
/*****************************************************************************/
STACKELEM st_string(x)
  STACKELEM x;
{
  register PTR_DESCRIPTOR desc;
  register PTR_HEAPELEM p;                     /*  pointer to store char's   */
  STACKELEM hlp_ptr;                           /*        help-pointer        */
  COUNT dim,
        index;

  START_MODUL("st_string");

  if ((desc = newdesc()) == 0)
    /* ---- no traversal on A-stack possible, because ---- */
    /* ---- sequence of Elements would get mixed up;  ---- */
    post_mortem("st_string: heap out of space");

  LI_DESC_MASK(desc,1,C_LIST,TY_STRING);       /*   set descriptor heading   */
  L_LIST((*desc),special) = EXTR_EDIT(x);      /*       save edit-field      */
  L_LIST((*desc),dim) = dim = ARITY(x);

  if (newheap(dim,A_LIST((*desc),ptdv)) == 0)
    /* ---- no traversal on A-stack possible, because ---- */
    /* ---- sequence of Elements would get mixed up;  ---- */
    post_mortem("st_string: heap out of space");

  RES_HEAP;
  p = R_LIST((*desc),ptdv);
  index = 0;
  while (p < R_LIST((*desc),ptdv) + dim)
  {
    x = POPSTACK(S_e);
  /*----------------*/
    if (T_STRING(x))                           /* string constructor popped  */
  /*----------------*/
    {
      REL_HEAP;
      hlp_ptr = st_string(x);
      RES_HEAP;
      /*---- heap compaction may have occured in st_string; so: ----*/
      p = R_LIST((*desc),ptdv) + index;
      *p = (T_HEAPELEM) hlp_ptr;
    }
    else
  /*-------------------*/
    if (T_NILSTRING(x))                        /*     empty string popped    */
  /*-------------------*/
    {
      *p = (T_HEAPELEM) _nilstring;            /* store standard descriptor  */
      INC_REFCNT(_nilstring);
    }
    else                                       /* hopefully character popped */
    {
      *p = (T_HEAPELEM) x;                     /* 'normal' char's stored as  */
                                               /* stackelements in heap;     */
    }
    p++;
    index++;
  }  /* end of while */
  REL_HEAP;
  END_MODUL("st_string");
  return((STACKELEM) desc);
} /**** end of function st_string ****/

/*-----------------------------------------------------------------------------
 * st_cond -- legt fuer einen Condtional auf dem A-Stack einen Descriptor an.
 *            Dieser wird als Stackelement zurueckgeben.
 * Externe Variablen:
 *            S_a           <w>       Zu verarbeitender Expression
 *            S_hilf        <w>
 * Externe Funktionen:
 *            newdesc, trav_a_hilf, mvheap;
 *-----------------------------------------------------------------------------
 */

STACKELEM st_cond()
{
  register PTR_DESCRIPTOR desc;
  STACKELEM x;

  START_MODUL("st_cond");

  if ((desc = newdesc()) == NULL)
    post_mortem("st_cond: Heap out of space");

  DESC_MASK(desc,1,C_EXPRESSION,TY_COND);
  x = POPSTACK(S_a);   /* den conditonal-Konstruktor */
  L_COND((*desc),special) = EXTR_EDIT(x);   /* Edit-Feld  SJ-ID    SJ-ID*/
  /* zuerst die Verarbeitung des else-Teils: */
  TRAV_A_HILF;
  if (mvheap(A_COND((*desc),ptee)) == 0)
    post_mortem("st_cond: Heap out of space");

  /* nun der then-Teil: */
  TRAV_A_HILF;
  if (mvheap(A_COND((*desc),ptte)) == 0)
    post_mortem("st_cond: Heap out of space");

  END_MODUL("st_cond");
  return((STACKELEM)desc);
} /**** end of function st_cond ****/

/*****************************************************************************/
/*                     st_mvt  (store matrix/vector/transposed vector)       */
/* ------------------------------------------------------------------------- */
/* function  :  create descriptor and corresponding heap-element from        */
/*              stack-element matrix / vector / transposed vector.           */
/* parameter :  class - class of stack-element                               */
/* returns   :  pointer to descriptor                                        */
/* extern                                                                    */
/* variables :  _nilmat          [ rheap.c ]                                 */
/* calls     :  st_mvt           (matrix of matrices)                        */
/* called by :  reac.c                                                       */
/*****************************************************************************/
STACKELEM st_mvt(class)
  char class;
{
  register PTR_DESCRIPTOR desc;
  register int i,dim=0; /* Initialisierung von TB, 6.11.1992 */
  STACKELEM  x,type=(STACKELEM)0; /* Initialisierung von TB, 6.11.1992 */
  int      form,format;
  int      *ipnew;                             /*    new integer-pointer     */
  /* Initialisierung von TB, 6.11.1992 */
  int      rows=0,cols=0;                      /*       rows / columns       */
  double   *rpnew;                             /*      new real-pointer      */

  START_MODUL("st_mvt");

  x = POPSTACK(S_e);                           /*      get constructor       */

  switch(class)
  {
  /*~~~~~~~~~~~~~~*/
    case C_MATRIX:
  /*~~~~~~~~~~~~~~*/
        rows = ARITY(x) - 1;
        type = POPSTACK(S_e);
        cols = ARITY(POPSTACK(S_e));
        dim  = rows * cols;
        break;
  /*~~~~~~~~~~~~~~*/
    case C_VECTOR:
  /*~~~~~~~~~~~~~~*/
        rows = 1;
        type = POPSTACK(S_e);
        cols = dim = ARITY(POPSTACK(S_e));
        break;
  /*~~~~~~~~~~~~~~*/
    case C_TVECTOR:
  /*~~~~~~~~~~~~~~*/
	/* mah 311293: aus dem lneu-Praeprozessor diese */
	/* zusaetzliche Abfrage uebernommen             */
        if T_NILTVECT(x)                        /*     store empty tvector    */
        {
          INC_REFCNT(_niltvect);
          END_MODUL("st_mvt");
          return((STACKELEM)_niltvect);
        }
        cols = 1;
        type = POPSTACK(S_e);
        rows = dim = ARITY(POPSTACK(S_e));
        break;
  } /** end switch(class) **/

  if ((desc = newdesc()) == NULL)               /*    create new descriptor   */
    post_mortem("st_mvt: heap out of space");


  switch(type)                                  /* switch depending on type   */
  {                                             /* of components;             */
  /*----------*/
    case BOOL:
  /*----------*/
        MVT_DESC_MASK(desc,1,class,TY_BOOL);
        if (newheap(dim,A_MVT((*desc),ptdv,class)) == 0)
          post_mortem("st_mvt: heap out of space");
        RES_HEAP;                               /* no heap compaction in      */
                                                /* multi-processor-system;    */
        ipnew = (int *) R_MVT((*desc),ptdv,class);
        for( i = 0; i < dim; i++)               /*loop to store all components*/
        {
          if T_LIST(READSTACK(S_e))             /*  ignore list-constructors  */
             PPOPSTACK(S_e);
          ipnew[i] = (T_SA_TRUE(POPSTACK(S_e)) ?
                                      TRUE : FALSE);
        }
        REL_HEAP;                               /*   heap-compaction allowed  */
        break;
  /*-------------*/
    case DECIMAL:
  /*-------------*/
        if (! _formated)                        /*  'non-formated'-switch on  */
        {
          MVT_DESC_MASK(desc,1,class,TY_DIGIT);
          if (newheap(dim,A_MVT((*desc),ptdv,class)) == 0)
            post_mortem("st_mvt: heap out of space");
                                                      /*----------------------*/
          for( i = 0; i < dim; i++)                   /* for further comments */
          {                                           /* see 'case BOOL:'     */
            if T_LIST(READSTACK(S_e))                 /*----------------------*/
               PPOPSTACK(S_e);
            if ((L_MVT((*desc),ptdv,class)[i] = st_dec()) == NULL)
              post_mortem("st_mvt: heap out of space");
          }
          break;
        }
        /* ---- 'formated'-switch is on ---- */
        format = INTEGER;                       /* otherwise format is unde-  */
                                                /* fined if integer-MVT is to */
                                                /* be stored;                 */
        MVT_DESC_MASK(desc,1,class,TY_REAL);    /*     create real-MVT        */
        if (newheap(dim<<1,A_MVT((*desc),ptdv,class)) == 0)
          post_mortem("st_mvt: heap out of space");
        RES_HEAP;
        rpnew = (double *) R_MVT((*desc),ptdv,class);
        for( i = 0; i < dim; i++)
        {                                             /*----------------------*/
          if T_LIST(READSTACK(S_e))                   /* for further comments */
             PPOPSTACK(S_e);                          /* see 'case BOOL:'     */
          rpnew[i] = valr(&form);                     /*----------------------*/
          if (form == REAL)                     /* real-number struct. occured*/
            format = REAL;
        }
        REL_HEAP;
        if (format == REAL) break;

        {                                       /* create integer-MVT   from  */
          PTR_DESCRIPTOR desc2;                 /* real-MVT                   */

          if ((desc2 = newdesc()) == NULL)
            post_mortem("st_mvt: heap out of space");
          MVT_DESC_MASK(desc2,1,class,TY_INTEGER);
          if (newheap(dim,A_MVT((*desc2),ptdv,class)) == 0)
            post_mortem("st_mvt: heap out of space");
          RES_HEAP;                                      /*----------------------*/
          ipnew = (int *) R_MVT((*desc2),ptdv,class);    /* for further comments */
          rpnew = (double *) R_MVT((*desc),ptdv,class);  /* see 'case BOOL:'     */
          for (i = 0; i < dim; i++)                      /*----------------------*/
            ipnew[i] = (int) rpnew[i];
          REL_HEAP;
          DEC_REFCNT(desc);                     /*       free real-MVT        */
          desc = desc2;
          break;
        }
  /*----------*/
    case CHAR:
  /*----------*/
        MVT_DESC_MASK(desc,1,class,TY_STRING);
        if (newheap(dim,A_MVT((*desc),ptdv,class)) == 0)
          post_mortem("st_mvt: heap out of space");
        for( i = 0; i < dim; i++)
        {                                             /*----------------------*/
          if T_LIST(READSTACK(S_e))                   /* for further comments */
             PPOPSTACK(S_e);                          /* see 'case BOOL:'     */
          x = POPSTACK(S_e);                          /*----------------------*/
          if (T_NILSTRING(x))
          {
            L_MVT((*desc),ptdv,class)[i] = (T_HEAPELEM) _nilstring;
            INC_REFCNT(_nilstring);
          }
          else
            L_MVT((*desc),ptdv,class)[i] = st_string(x);
        }
        break;
  /*--------------*/
    case TMATRIX:
  /*--------------*/
        MVT_DESC_MASK(desc,1,class,TY_MATRIX);
        if (newheap(dim,A_MVT((*desc),ptdv,class)) == 0)
          post_mortem("st_mvt: heap out of space");  /*----------------------*/
        for( i = 0; i < dim; i++)                    /* for further comments */
        {                                            /* see 'case BOOL:'     */
          if T_LIST(READSTACK(S_e))                  /*----------------------*/
             PPOPSTACK(S_e);
          L_MVT((*desc),ptdv,class)[i] = st_mvt(C_MATRIX);
        }
        break;


    default:   post_mortem("st_mvt: Unknown componenttype");
  } /** end switch(type) **/

  L_MVT((*desc),nrows,class) = rows;
  L_MVT((*desc),ncols,class) = cols;

  END_MODUL("st_mvt");
  return((STACKELEM)desc);
}
/*-----------------------------------------------------------------------------
 * st_name -- legt fuer einen Letterstring auf dem E-Stack einen Descriptor an.
 *            Falls aber fuer diesen Namen bereits ein Descriptor existiert,
 *            so wird dieser zurueckgeben. Das bedeutet zwei Namen sind genau
 *            dann gleich, wenn die zugehoerigen Namensdescriptoren gleich sind.
 * globals -- S_e           <w>       Zu verarbeitender Name
 *            S_v           <w>       Stapel fuer angelegte Descriptoren.
 *            S_hilf        <w>       Hilfsstapel fuer dieselben
 * calls   -- newdesc, newheap;
 *-----------------------------------------------------------------------------
 */

STACKELEM st_name()
{
  register PTR_DESCRIPTOR desc;
  STACKELEM           name[81];           /* name[0] wird nicht verwendet */
  int           namelength = 0;
  STACKELEM          *descname;
  register int             i,j;
  int     desc_counter = SIZEOFSTACK(S_v);

  START_MODUL("st_name");

  /* Namen von E-Stack einlesen: */
  while (T_STR1((name[++namelength] = POPSTACK(S_e))) && (namelength < 80));

  if (namelength == 80)
    post_mortem("st_name: Letterstring too long");

  /* Testen, ob fuer den Namen bereits eine Descriptor auf dem V_Stack liegt */
  for ( j = desc_counter; j > 0; j--)
  {
     desc = (PTR_DESCRIPTOR) READSTACK(S_v);
     RES_HEAP;
#if WITHTILDE
     if (R_DESC((*desc),type) == TY_NAME )
       descname = (STACKELEM *) R_NAME((*desc),ptn);
     else
       descname= (STACKELEM *)R_FNAME((*desc),ptn);
#else
     descname = (STACKELEM *) R_NAME((*desc),ptn);
#endif /* WITHTILDE */

     for (i = 1; i <= namelength; i++)
       if ((name[i] & ~F_EDIT) != (descname[i] & ~F_EDIT))
         break;
     REL_HEAP;

     if ( i > namelength)  /* Descriptor gefunden: */
     {
       for ( ; j < desc_counter; j++)    /* Descriptoren zurueckschaufeln */
         PPUSHSTACK(S_v,POPSTACK(S_hilf));

       INC_REFCNT(desc);

       END_MODUL("st_name");
       return((STACKELEM)desc);
    }

    /* also der war es nicht. */
    PUSHSTACK(S_hilf,POPSTACK(S_v));
  }

  /* Ein neuer Name! */

  for ( ; j < desc_counter; j++)    /* Descriptoren zurueckschaufeln */
    PPUSHSTACK(S_v,POPSTACK(S_hilf));

#if WITHTILDE
 /* name[1] = 4194309 => das entsprechende Stackelement */
 /* ist ein Klammeraffe                                 */

  if ( name[1] == 4194309 )
  {
     /* Anlegen eines Funktionname - Descriptors */
  
     if ((desc = newdesc()) == NULL)
          post_mortem("st_fname: heap out of space");
     DESC_MASK(desc,1,C_EXPRESSION,TY_FNAME);

    if (newheap( 1 + namelength,(int *) A_FNAME((*desc),ptn)) == 0)
          post_mortem("st_fname: heap out of space");
     descname = (STACKELEM *) R_FNAME((*desc),ptn);

     /* String eintragen : */
     for ( i = 1; i <= namelength; i++)
         descname[i] = name[i];
     descname[0] = namelength;         /* wird nur in ret_fname gebraucht */
  
  }
  else
  {
#endif /* WITHTILDE */
  
  if ((desc = newdesc()) == NULL)
    post_mortem("st_name: heap out of space");
  DESC_MASK(desc,1,C_EXPRESSION,TY_NAME);

  if (newheap( 1 + namelength,(int *) A_NAME((*desc),ptn)) == 0)
    post_mortem("st_name: heap out of space");
  descname = (STACKELEM *) R_NAME((*desc),ptn);

  /* String eintragen : */
  for ( i = 1; i <= namelength; i++)
    descname[i] = name[i];
  descname[0] = namelength;              /* wird nur in ret_name gebraucht */

#if WITHTILDE
  }
#endif /* WITHTILDE */

  PUSHSTACK(S_v,(STACKELEM)desc); /* wichtig: Ref_count von Desc. ist 1! */

  END_MODUL("st_name");
  return((STACKELEM)desc);
}

/*-----------------------------------------------------------------------------
 * st_var -- legt fuer einen auf dem A-Stack evtl. zusammen mit Schutzstrichen
 *           versehenen Namensdescriptor einen Variablendescriptor an.
 * globals -- S_a           <w>
 * calls   -- newdesc;
 *-----------------------------------------------------------------------------
 */

STACKELEM st_var()
{
  register PTR_DESCRIPTOR desc;
  register int      nlabar = 0;
  register STACKELEM         x;

  START_MODUL("st_var");

  if ((desc = newdesc()) == NULL)
    post_mortem("st_var: Heap out of space");
  DESC_MASK(desc,1,C_CONSTANT,TY_VAR);

  /* Einlesen der Protektstriche */
  while ( T_PROTECT((x = POPSTACK(S_a))) ) nlabar++;

  L_VAR((*desc),ptnd) = (T_PTD) x;
  L_VAR((*desc),nlabar) = nlabar;

  END_MODUL("st_var");
  return((STACKELEM)desc);
}

/*-----------------------------------------------------------------------------
 * st_vare - legt fuer einen auf dem E-Stack evtl. zusammen mit Schutzstrichen
 *           versehenen Namensdescriptor einen Variablendescriptor an.
 * globals -- S_e <w>
 * calls   -- newdesc;
 *-----------------------------------------------------------------------------
 */

STACKELEM st_vare()
{
  register PTR_DESCRIPTOR desc;
  register int      nlabar = 0;
  register STACKELEM         x;

  START_MODUL("st_vare");

  if ((desc = newdesc()) == NULL) post_mortem("st_var: Heap out of space");
  DESC_MASK(desc,1,C_CONSTANT,TY_VAR);
  /* zaehlen der Protektstriche */
  while ( T_PROTECT((x = POPSTACK(S_e))) )
    nlabar++;
  L_VAR((*desc),ptnd) = (T_PTD) x;
  L_VAR((*desc),nlabar) = nlabar;
  END_MODUL("st_vare");
  return((STACKELEM)desc);
}
#if WITHTILDE
/*--------------------------------------------------------------------*/
/* nst_var -- legt fuer einen auf dem A-Stack mit n Schutzstrichen    */
/*           versehenen Namensdescriptor einen Variablendescriptor an.*/
/* globals -- S_a           <w>                                       */
/* calls   -- newdesc;                                                */
/*--------------------------------------------------------------------*/

void nst_var(x,n)
STACKELEM x;
int       n;
{
  register PTR_DESCRIPTOR desc;

  START_MODUL("nst_var");

  if ((desc = newdesc()) == NULL)
    post_mortem("nst_var: Heap out of space");
  DESC_MASK(desc,1,C_CONSTANT,TY_VAR);

  L_VAR((*desc),ptnd) = (T_PTD) x;
  L_VAR((*desc),nlabar) = n;    /* n entspricht der Anzahl der */
                                /* vorhandenen protects        */

  x=(STACKELEM)desc;
  PUSHSTACK(S_a,x);
  END_MODUL("nst_var");
  return;
}
#endif /* WITHTILDE */
