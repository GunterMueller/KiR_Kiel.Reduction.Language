/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
/**********************************************************************/
/*  postfunc.c                                                        */ 
/*                                                                    */
/*  MODUL der Postprocessing Phase                                    */
/*                                                                    */
/*          external :  ILIST_end ; gen_ILIST_block ; gen_ILIST_lrec  */
/*                      gen_ILIST_case ; gen_ILIST_casrec ;           */
/*                      free_ILIST_block ;                            */
/*                      TLIST_end ; gen_TLIST_block ; gen_TLISt_clos  */
/*                      free_TLIST_block ;                            */
/*                      inc_protects ; dec_protects ;                 */
/*                      test_NUM ; test_TILDE ; test_LIND ; test_CASE */
/*                      p_load_expr ; ret_var ; ret_cond ;            */
/*                      p_ret_expr                                    */
/*                                                                    */                 
/*   Aufgerufen durch : ea_retrieve ;                                 */
/*                      switch_post ;                                 */
/*                      nomat_post                                    */
/*                                                                    */
/**********************************************************************/




#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "rmeasure.h"
#include "list.h"



/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

extern postlist      *ILIST;
extern postlist_1    *TLIST;                   




extern void test_NUM();                        /* forward declaration */
extern void test_TILDE();                      /* forward declaration */


#define TET 0 
                                    


/*********************************************************************/
/*                                                                   */
/*  ILIST               ILIST_end                                    */
/*                      gen_ILIST_block                              */
/*                      gen_ILIST_lrec                               */ 
/*                      gen_ILIST_case                               */
/*                      gen_ILIST_caserec                            */
/*                      free_ILIST_block                             */
/*                                                                   */
/*********************************************************************/

/* typedef struct POSTLIST                                           */
/* {                                                                 */
/*  STACKELEM              var;                                      */
/*  struct POSTLIST      *link;                                      */
/*  struct POSTLIST      *next;                                      */
/* } postlist;                                                       */
                                                                        
/* ILIST ist als Zeiger auf eine Struktur vom Typ postlist definiert */

/* Enthaelt der in der Postprocessing Phase zu rekonstruierende Aus- */
/* druck noch Nummern- bzw Tildevariable, so sind an ihre Stelle die */
/* urspruenglichen Variablennamen oder Argumente ( Ausfuehrung par-  */
/* tieller Substitutionen ) zu setzen.                               */

/* In ILIST werden fuer ein SUB die urspruenglichen Variablennamen   */
/* bzw zu substituierende Argumente ( Aufloesen von Closures ) , fuer*/
/* ein LREC bzw CASEREC die Funktionsnamen bzw der Funktionsname und */
/* fuer jede Pi - Abstraktion die Namen der im pattern auftretenden  */
/* Variablen eingetragen.                                            */     
/* ILIST enthaelt somit die Substitute fuer Nummernvariable.         */


/**********************************************************************/
/* ILIST_end traegt in ILIST eine Endemarkierung ein                  */
/**********************************************************************/

void ILIST_end()

{
  register postlist *p;

   p = (postlist *)malloc(sizeof(postlist));
   p->var = NULL;  
   p->link = (postlist *)ILIST;
   p->next = (postlist *)ILIST;
   ILIST = (postlist *)p;
  return;
}


/**********************************************************************/
/* gen_ILIST_block traegt die in einem SUB - Descr. nachgehaltenen    */
/* Variablennamen in ILISt ein                                        */
/**********************************************************************/

void gen_ILIST_block(nbound,name)
int       nbound;
STACKELEM  *name;
{ 
  postlist *p;

  /* Trennsymbol generieren */
     ILIST_end();
     
  /* Variablennamen in ILIST eintragen */
     for( ; nbound > 0 ; nbound--)
     {
       p = (postlist *)malloc(sizeof(postlist));
       p->var = (STACKELEM) name[nbound + 1];
       INC_REFCNT((PTR_DESCRIPTOR)name[nbound + 1]);
       p->link = (postlist *)ILIST;
       p->next = (postlist *)ILIST;
       ILIST = p;
     }
  return;
}


/**********************************************************************/
/* gen_ILISt_lrec traegt die in einem LREC - Descr. nachgehaltenen    */
/* Funktionsnamen in ILIST ein                                        */
/**********************************************************************/

void gen_ILIST_lrec(name,desc)
STACKELEM        *name;
PTR_DESCRIPTOR    desc;
{ 
  postlist  *p;
  int    i,anz;

  /* Trennsymbol generieren */
     ILIST_end();

     anz = (int)name[0];
     
  /* Funktionsnamen in ILIST eintragen */
     for( i = 1 ; i <= anz ; i ++)
     {
       p = (postlist *)malloc(sizeof(postlist));
       p->var = (STACKELEM) name[i];
       INC_REFCNT((PTR_DESCRIPTOR)name[i]);
       p->link = (postlist *)ILIST;
       p->next = (postlist *)ILIST;
       ILIST = p;
     }

     /* LREC - Desc. in ILIST eintragen */
        p = (postlist *)malloc(sizeof(postlist));
        p->var = (STACKELEM)desc;
        INC_REFCNT(desc);
        p->link = (postlist *)ILIST;
        p->next = (postlist *)ILIST;
        ILIST = p;
    return;
 }
        

/**********************************************************************/
/* gen_ILIST_case traegt die Patternvariablen einer Pi - Abstraktion  */
/* in ILIST ein.                                                      */
/* Ein CASE - Descr. wird aufgeloest, indem die einzelnen Pi - Abstr. */  
/* plus einem Case - Konstruktor auf den E- Stack gelegt werden .     */
/* Hierbei werden fuer jeden Match - Descriptor die Patternvariablen  */
/* in ILIST eingetragen. Um die Bindungstrukturen des zu rekon-       */
/* struierenden Ausdruckes nicht zu zerstoeren, muss ein Teil der     */
/* Bloecke " unsichtbar " eingehaengt werden . Ueber die Parameter    */
/* link und next_block wird dieses " unsichtbare " Einhaengen         */
/* gesteuert.                                                         */
/* Beispiel :  Rekonstruktion des Ausdruckes                          */
/*              case                                                  */
/*              when < x > guard ...                                  */
/*              when < x, y > guard ...                               */
/*              when < x, y, z > guard ...                            */
/*             end                                                    */
/* Auf dem E - Stack wird ein Ausdruck der Form                       */
/*                                                                    */
/* case(4) when(3) list(1) p_x  p_guard_expr_1  p_body_expr_1         */
/*         when(3) list(2) p_x p_y  p_guard_expr_2  p_body_expr_2     */
/*         when(3) list(3) p_x p_y p_z  p_guard_expr_3 p_body_expr_3  */
/*         end(1)                                                     */
/*                                                                    */
/* aufgebaut.                                                         */
/*                                                                    */
/* In ILIST werden die Patternvariablen in Form von Bloecken einge-   */
/* tragen ( @ = Blockendezeichen ):                                   */
/*                                                                    */
/*  ... | @ | p_x | p_y | p_z | @ | p_x | p_y | @ | p_x     var       */
/*  ----|---|-----|-----|-----|---|-----|-----|---|----               */
/*      | ! |  !  |  !  |  !  | ! |  !  |  !  | ! |  !      link      */
/*        !____!_____!_____!    !____!_____!    !____!                */
/*        !_____________________!               !                     */
/*        !_____________________________________!                     */
/*                                                                    */   
/* Die Variablenbloecke fuer die zweite und die dritte Pi - Abstr.    */ 
/* werden unsichtbar eingehaengt und bei der Rekonstruktion des guard-*/
/* bzw body - Ausdruckes der ersten Pi - Abstr. ueberlesen.           */
/*                                                                    */
/* ( Bemerkung  : dieses Verfahren ist durch die Rekonstruktion von   */
/*   zusammengefassten Case - Funktionen notwendig, da in diesem Fall */
/*   die Pattern der einzelnen Pi - Abstr. modifiziert werden muessen */
/*   und die Information hierueber in dem Case - descr. nicht jedoch  */
/*   in den Match - descr. enthalten ist. )                           */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

void gen_ILIST_case( arg_list , index ,link ,next_block)
STACKELEM    *arg_list;                /* Liste von Patternvariablen  */
int              index;                /* Anzahl der Patternvariablen */      
BOOLEAN           link;
postlist   *next_block;
{
  STACKELEM        x;
  postlist        *p;
  int              i;      
                 
  if ( link )
  { 
    /* der Block ist unsichtbar einzuhaengen */
       x = arg_list[index];
      if ( T_POINTER(x) )
          INC_REFCNT((PTR_DESCRIPTOR)x);
   
      p = ( postlist *) malloc(sizeof(postlist));
      p->var = x;                   
      p->link = (postlist *)next_block;      /* logische Folgeadresse */
      p->next = (postlist *)ILIST;
      ILIST = p ;   

      for( i = index - 1 ; i > 0 ; i-- )
      {
        x = arg_list[i];
        if ( T_POINTER(x) )
            INC_REFCNT((PTR_DESCRIPTOR)x);
   
        p = ( postlist *) malloc(sizeof(postlist));
        p->var = x;                   
        p->link = (postlist *)ILIST;
        p->next = ILIST;
        ILIST = p ;
     } 
  }
  else  

    for( i = index ; i > 0 ; i-- )
    {
      x = arg_list[i];
      if ( T_POINTER(x) )
          INC_REFCNT((PTR_DESCRIPTOR)x);
   
      p = ( postlist *) malloc(sizeof(postlist));
      p->var = x;                   
      p->link = (postlist *)ILIST;
      p->next = ILIST;
      ILIST = p ;
   }
}


/**********************************************************************/
/* gen_ILIST_casrec traegt den in einem CASEREC- Descr. nachgehaltenen*/
/* Funktionsnamen in ILIST ein                                        */
/**********************************************************************/

void gen_ILIST_caserec(desc)
PTR_DESCRIPTOR desc;    /* SWITCH - Descriptor */
{ 
  PTR_DESCRIPTOR  func_name;
  postlist               *p;
  int                 i,anz;

  /* Trennsymbol generieren */
     ILIST_end();

     anz = (int)R_SWITCH((*desc),nwhen); 
     func_name = (PTR_DESCRIPTOR)R_SWITCH((*desc),ptse)[anz + 1];
     
  /* Funktionsnamen in ILIST eintragen */
     p = (postlist *)malloc(sizeof(postlist));
     p->var = (STACKELEM)func_name;
     INC_REFCNT(func_name);
     p->link = (postlist *)ILIST;
     p->next = (postlist *)ILIST;
     ILIST = p;

   /* CASEREC - Desc. in ILIST eintragen */
      p = (postlist *)malloc(sizeof(postlist));
      p->var = (STACKELEM)desc;
      INC_REFCNT(desc);
      p->link = (postlist *)ILIST;
      p->next = (postlist *)ILIST;
      ILIST = p;
 
   return;
 }


/**********************************************************************/
/* free_ILIST_block entfernt den ersten Block aus ILIST               */
/**********************************************************************/

void free_ILIST_block()

{
 register postlist *p;

   while ( ILIST->var != (STACKELEM) NULL )
   {
     p = (postlist *)ILIST->next;
     if ( T_POINTER (ILIST->var) )
        DEC_REFCNT((PTR_DESCRIPTOR)ILIST->var);
     free(ILIST);
     ILIST = (postlist *)p;
   }

  /* Blockende-zeichen entfernen    */
     p = (postlist *) ILIST->next;
     free(ILIST);
     ILIST = (postlist *)p;

  return;
}
  



/*********************************************************************/
/*                                                                   */
/*  TLIST               TLIST_end                                    */
/*                      gen_TLIST_block                              */
/*                      gen_TLIST_clos                               */ 
/*                                                                   */
/*********************************************************************/

/* typedef struct POSTLIST_1                                         */
/* {                                                                 */
/*  STACKELEM              var;                                      */
/*  int                  nlbar;                                      */
/*  struct POSTLIST_1    *next;                                      */
/* } postlist_1;                                                     */ 

/* TLIST ist als Zeiger auf eine Struktur des Types postlist_1       */
/* definiert.                                                        */

/* Enthaelt der in der Postprocessing Phase zu rekonstruierende Aus- */
/* druck noch Nummern- bzw Tildevariable, so sind an ihre Stelle die */
/* urspruenglichen Variablennamen oder Argumente ( Ausfuehrung par-  */
/* tieller Substitutionen ) zu setzen.                               */

/* In TLIST werden die Substitute fuer Tildevariable eingetragen.    */        

/**********************************************************************/
/* TLIST_end traegt in TLIST eine Endemarkierung ein                  */
/**********************************************************************/

void TLIST_end()

{
 register postlist_1 *q;

   q = (postlist_1 *)malloc(sizeof(postlist_1));
   q->var = NULL;
   q->nlbar = 0;
   q->next = (postlist_1 *)TLIST;
   TLIST = (postlist_1 *)q;
 return;
}


/**********************************************************************/
/*  gen_TLIST_block traegt n auf dem E - Stack liegende Argumente in  */
/*  TLIST ein                                                         */
/**********************************************************************/

void gen_TLIST_block(n) 
int n;
{
 postlist_1  *q,*q_1;
 STACKELEM         x;
 int           nlbar;

      
    /* Trennzeichen generieren */
       q = (postlist_1 *)malloc(sizeof(postlist_1));
       q->var = NULL;
       q->nlbar = 0;
       q->next=TLIST;

    /* auf S_e liegen n Argumente fuer rel. freie Variable ; */
    /* diese werden in TLIST eingetragen                     */
       for(  ; n > 0 ; n-- )
       {
         /* liegt auf S_e eine Nummmer - bzw Tilde - Variable , */
         /* so wird sie durch das entsprechende Argument ersetzt */
            x=READSTACK(S_e);
       
            if ( T_NUM(x) )                                
            {
              POPSTACK(S_e);
              test_NUM( ARITY(x) );
            }

            if ( T_TILDE(x) )
            {
              POPSTACK(S_e);
              test_TILDE( ARITY(x) ) ;
            }

            nlbar=0;
            while( T_PROTECT(( x=POPSTACK(S_e))) )  nlbar++ ;
         
            q_1=(postlist_1 *)malloc(sizeof(postlist_1));
   
            if ( T_POINTER(x) )
              INC_REFCNT((PTR_DESCRIPTOR)x);

            q_1->var=x;
            q_1->nlbar=nlbar;
            q_1->next=(postlist_1 *)q;
            q=q_1;
       } 

       TLIST=q;
    return;
}


/**********************************************************************/
/*  gen_TLIST_clos traegt die in einer Closure nachgehaltenen Argu-   */
/*  mente in TLISt ein                                                */
/**********************************************************************/

void gen_TLIST_clos(nfv,p_arg)
int           nfv;                            /* Anzahl der Argumente */
STACKELEM  *p_arg;                            /* Liste deer Argumente */
{
  postlist_1   *q;
  STACKELEM     y;
  int       i,anz;

   /* Trennzeichen generieren */
      TLIST_end();

      anz=(int) p_arg[0];
     
   /* Argumente fuer rel. freie Variable werden aus der  */
   /* pta - Liste des Closure - Descriptors in die TLIST */
   /* uebernommen                                        */
      for( i=anz ; i > (anz - nfv ) ; i-- )
      {
        y=p_arg[i];
        if ( T_POINTER(y) )
            INC_REFCNT((PTR_DESCRIPTOR) y);

        q=(postlist_1 *) malloc(sizeof(postlist_1));
        q->var=y  ;
        q->nlbar=0;
        q->next=TLIST;
        TLIST=q;
       }   

   return;
}



/**********************************************************************/
/* free_TLIST_block entfernt einen Block aus TLIST                    */
/**********************************************************************/

void free_TLIST_block()

{
 register postlist_1 *p;

  while ( TLIST->var != (STACKELEM) NULL )
  {
    p = (postlist_1 *)TLIST->next;
    if ( T_POINTER(TLIST->var) )
       DEC_REFCNT((PTR_DESCRIPTOR) TLIST->var );
    free(TLIST);
    TLIST = (postlist_1 *)p;
  }

  /* Blockende-zeichen entfernen    */
     p = (postlist_1 *) TLIST->next;
     free(TLIST);
     TLIST = (postlist_1 *)p;

  return;

}




/*********************************************************************/
/*                                                                   */
/*  dec_protects                                                     */
/*  inc_protects                                                     */
/*                                                                   */
/*********************************************************************/



/**********************************************************************/
/*                                                                    */
/*                                                                    */
/**********************************************************************/

void inc_protects() 
{
 postlist   *p;
 postlist_1 *q;

    /* treten im ersten Block von TLIST Variablen auf , so wird  */
    /* beim Eintritt in einen neuen Bindungsbereich ueberprueft ,*/
    /* ob diese mit protects versehen werden muessen             */
       q=TLIST;
       while( q->var != (STACKELEM)NULL )
       {
         if ( T_POINTER( q->var) &&
            ( R_DESC(( *(PTR_DESCRIPTOR) q->var),type) == TY_NAME ))
         {
           p=ILIST;
           while( p->var != (STACKELEM) NULL )
           {
             if ( p->var == q->var )
                  q->nlbar++;
             p=p->next;
           }
         }
         q=q->next;
       }  

    return;
}


/**********************************************************************/
/*                                                                    */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

void dec_protects() 
{
 postlist   *p;
 postlist_1 *q;

    /* treten im ersten Block von TLIST Variablen auf , so wird  */
    /* beim Austritt aus einem Bindungsbereich ueberprueft , ob */
    /* protects entfernt werden muessen                          */
       q=TLIST;
       while( q->var != (STACKELEM)NULL )
       {
         if ( T_POINTER( q->var) &&
           ( R_DESC(( *(PTR_DESCRIPTOR) q->var),type) == TY_NAME ))
         {
           p=ILIST;
           while( p->var != (STACKELEM) NULL )
           {
             if ( (p->var) == q->var )
                  q->nlbar--;
             p=p->next;
           }
         } 
         q=q->next;
       }
    return;
}



 
/*********************************************************************/
/*                                                                   */
/*  TEST                test_NUM                                     */
/*                      test_TILDE                                   */
/*                      test_LIND                                    */ 
/*                      test_CASE                                    */
/*                                                                   */
/*********************************************************************/


/**********************************************************************/
/* test_NUM bestimmt fuer NUM(i) den zu substituierenden Variablen-   */
/* namen ( plus evtl. zu setzende protects ) oder das zu subst.       */
/* Argument                                                           */
/**********************************************************************/

void test_NUM(i)
int i;
{
 postlist       *p;
 STACKELEM       y;
 int       j,nlbar;


   p=ILIST;

  /* teste , ob der erste Block von ILIST fuer ein LREC oder CASEREC */
  /* angelegt wurde ; bestimme ersten Block der nicht fuer ein LREC  */
  /* oder CASEREC angelegt wurde                                     */
     if( T_POINTER(p->var) )
       while (( R_DESC((*(PTR_DESCRIPTOR)p->var),type) == TY_LREC ) ||
              ( R_DESC((*(PTR_DESCRIPTOR)p->var),type) == TY_SWITCH ) )   
       {
         while (  p->var != (STACKELEM) NULL )
                p=p->link;
         p=p->link;
        /* p zeigt jetzt auf den Anfang des naechsten Blockes */
       }

  /* bestimme die zu NUM(i) gehoehrige Variable */
     for ( j = i ; j > 0 ; j-- )
          p = p->link;

     y = p->var;
     nlbar = 0;  

    if (( T_POINTER(y) ) && ( R_DESC( ( *(PTR_DESCRIPTOR)y ) , type ) == TY_NAME ))
    {                
     /* bestimme fuer einen Variablennamen die Anzahl der protect's */
        p = ILIST;

        if( T_POINTER(p->var) )
        {  
          while ( R_DESC((*(PTR_DESCRIPTOR)p->var),type) == TY_LREC )
          {
            while (  p->var != (STACKELEM) NULL )
            {     if (p->var == y ) nlbar++;
                   p=p->link;
            }
            p=p->link;
          }

          while ( i >0 )
          {
            if (p->var == y ) nlbar++;
            p=p->link;
            i--;
          }
        }                  
    }
    /* Ergebnis auf S_e legen */
       if ( T_POINTER(y) )
         INC_REFCNT((PTR_DESCRIPTOR)y); 
       PUSHSTACK(S_e, y);
       for( ; nlbar > 0 ; nlbar-- )
           PUSHSTACK(S_e,SET_ARITY(PROTECT,1));
  return;
}


/**********************************************************************/
/* test_TILDE bestimmt fuer TILDE(i) das zu substituierende Argument  */
/**********************************************************************/

void test_TILDE(i)
int i;
{
 postlist_1  *q;
 STACKELEM    y;
 int      nlbar;

  
  /* bestimme das zu TILDE(i) gehoehrige Argument */
     q = TLIST;
     for( ; i > 0 ; i-- )
        q = q->next;
     y = (STACKELEM)q->var;
     nlbar = q->nlbar;

  /* y wird auf S_e gelegt ; handelt es sich um einen Namens - */
  /* Descriptor , so werden moeglicherweise vorhandene protects */
  /* ebenfalls auf S_e gelegt                                   */

     if ( T_POINTER(y) )
       INC_REFCNT((PTR_DESCRIPTOR)y);
     PUSHSTACK(S_e, y);
     for (  ; nlbar > 0 ; nlbar-- )
        PUSHSTACK(S_e,SET_ARITY(PROTECT,1));

  return;
}
  

/**********************************************************************/
/* test_LIND ueberprueft, ob ein LREC-IND _ Descr nur durch den Fkt-  */
/* namen zu ersetzen ist oder ob zusaetzlich die LREC - Struktur auf- */
/* zubauen ist                                                        */
/**********************************************************************/

int test_LIND(desc)
PTR_DESCRIPTOR desc;    /* LREC-IND - Descriptor */
{
  postlist                            *p;
  PTR_DESCRIPTOR    lrec_desc,funct_name;
  int             anz,index,nlbar,result;


   START_MODUL("test_LIND");

   /* zugehoehriger LREC - Desc. */
      lrec_desc = (PTR_DESCRIPTOR)R_LREC_IND((*desc),ptdd);

      anz = (int)R_LREC((*lrec_desc),nfuncs);
      index = (int)R_LREC_IND((*desc),index);
     
/*     if (( anz == 1 )  && ( (int) R_LREC((*lrec_desc),special ) != 0 ))
     {

      funct_name = (PTR_DESCRIPTOR)R_LREC((*lrec_desc),namelist)[index];
      PUSHSTACK(S_a,(STACKELEM)funct_name);
      INC_REFCNT(funct_name);
      PUSHSTACK(S_a,SET_ARITY(LREC,1)); 
      DEC_REFCNT(desc);
      END_MODUL("test_LIND");
      return(1);
     }                                         */

      nlbar = 0;
      result = 0;
      p = ILIST;

      while( p != NULL )
      {
        if ((PTR_DESCRIPTOR) p->var == lrec_desc )
        {
          result = 1;
          break;
        }
        else
          p = p->link ;
      }

   /* ersetze LREC-IND - Desc. durch Funktionsnamen */
      funct_name = (PTR_DESCRIPTOR)R_LREC((*lrec_desc),namelist)[index];

      PUSHSTACK(S_e,(STACKELEM)funct_name);
      INC_REFCNT(funct_name);

   if (result == 0 )
   {
     /* LREC(2) lrec_desc auf S_e legen */
        PUSHSTACK(S_e,(STACKELEM)lrec_desc);
        INC_REFCNT(lrec_desc);
        PUSHSTACK (S_e, (STACKELEM)(LREC | SET_EDIT( (int)R_LREC( (*lrec_desc), special))));
        WRITE_ARITY (S_e, 2);

   }
   else
   {
      /* bestimme Anzahl der protects    */
         p = ILIST;
         while((PTR_DESCRIPTOR) p->var != lrec_desc )
         {
           if ((PTR_DESCRIPTOR) p->var == funct_name )  nlbar++;
           p = p->link;
         }

      index = anz-index+1;
      for (  ; index > 1 ; index-- )
      {
         if ((PTR_DESCRIPTOR) p->var == funct_name )  nlbar++;
         p = p->link;
      }

    /* protects auf S_e legen */
       for(  ; nlbar > 0 ; nlbar-- )
          PUSHSTACK(S_e,SET_ARITY(PROTECT,1));
   } 
                  
   DEC_REFCNT(desc);
   END_MODUL("test_LIND");
   return(0);
}  
       

/**********************************************************************/
/*  analog zu test_LIND                                               */
/**********************************************************************/

int test_CASE(desc)
PTR_DESCRIPTOR desc;                          /* CASEREC- Descriptor */
{
  postlist                            *p;
  PTR_DESCRIPTOR              funct_name;
  int             anz,index,nlbar,result;


   START_MODUL("test_CASE");

    nlbar = 0;
    result = 0;
    p = ILIST;

    while( p != NULL )
    {
      if ((PTR_DESCRIPTOR) p->var == desc )
      {
        result = 1;
        break;
      }
      else
        p = p->link;

    }   

    if( result == 1 )
    {
     /* ersetze CASEREC- Desc. durch Funktionsnamen */
        anz = (int)R_SWITCH((*desc),nwhen); 
        funct_name = (PTR_DESCRIPTOR)R_SWITCH((*desc),ptse)[anz + 1];

        PUSHSTACK(S_e,(STACKELEM)funct_name);
        INC_REFCNT(funct_name);

     /* bestimme Anzahl der protects    */
        p = ILIST;
        while((PTR_DESCRIPTOR) p->var != desc )
        {
          if ((PTR_DESCRIPTOR) p->var == funct_name )  nlbar++;
          p=p->link;
        }

     /* protects auf S_e legen */
        for(  ; nlbar > 0 ; nlbar-- )
           PUSHSTACK(S_e,SET_ARITY(PROTECT,1));
  
      DEC_REFCNT(desc);
      END_MODUL("test_CASE");
      return(1);
    }  
    else
    {               
      END_MODUL("test_CASE");
      return(0);
    }
}
   


/*********************************************************************/
/*                                                                   */
/*  p_load_expr                                                      */
/*  ret_var                                                          */
/*  ret_cond                                                         */ 
/*  p_ret_expr                                                       */
/*                                                                   */
/*********************************************************************/


/**********************************************************************/
/*  p_load_expr lagert eine Ausdruck aus dem Heap in Stacksystem ein, */
/*  ohne systeminterne Variablen zu ersetzen                          */
/*  ( leicht modifizierte Version von load_expr )                     */
/**********************************************************************/

void p_load_expr(pptr)
STACKELEM ** pptr;
{
  register int          size;
  register STACKELEM       x;
  register STACKELEM    *ptr;

  START_MODUL("p_load_expr");
#if MEASURE
  measure(LOAD_EXPR);
#endif

  RES_HEAP;
  ptr = *pptr;
  size = ptr[0] - TET;

  IS_SPACE(S_e,size);

#if HEAP
  --size;
#endif
 
  while (size > 0)
  {
    x = ptr[size];
    if (T_POINTER(x))
    {
      INC_REFCNT((PTR_DESCRIPTOR)x);
      PPUSHSTACK(S_e,x);
      size--;
    } 
    else
    { /* ansonsten */
      PPUSHSTACK(S_e,x);
      size--;
    } 
  }
  REL_HEAP;

  END_MODUL("p_load_expr");
}


/**********************************************************************/
/*  ret_var loest einen Variablen - Descr. auf. Der hinter diesem     */
/*  Descr. liegende Namens - Descr. wird auf den E - Stack gelegt     */
/*  zusammen mit den evtl zu setzenden protects                       */
/**********************************************************************/

 void ret_var(pdesc)
    STACKELEM pdesc;
 {
    postlist             *p; 
    PTR_DESCRIPTOR     desc = (PTR_DESCRIPTOR) pdesc;
    PTR_DESCRIPTOR    ndesc;
    register int     nlbar ;

    START_MODUL("ret_var");

    /* den Namensdeskriptor auf den E-Stack legen */
       ndesc = R_VAR((*desc),ptnd);
       PUSHSTACK(S_e,(STACKELEM)ndesc);
       INC_REFCNT(ndesc);                               

    /* bestimme die Anzahl der zu setzenden protects  */
       nlbar = R_VAR((*desc),nlabar);
       p = ILIST;
       while( p != NULL )
       {
         if ( p->var == (STACKELEM)ndesc ) nlbar++;
         p = p->link;
       }   

    for ( ; nlbar > 0; nlbar--  )
       PUSHSTACK(S_e,SET_ARITY(PROTECT,1));

    DEC_REFCNT(desc);
    END_MODUL("ret_var");        
    return;
 }
            

/**********************************************************************/
/* ret_cond loest einen Cond - Descr. auf. der then- und der else -   */
/* Teil weden mit einen COND - Konstruktor auf den E - Stack gelegt   */
/**********************************************************************/


 void ret_cond(pdesc)
    STACKELEM pdesc;
 {
    PTR_DESCRIPTOR     desc = (PTR_DESCRIPTOR) pdesc;
    int cond;

    START_MODUL("ret_cond");

    p_load_expr((STACKELEM **)A_COND((*desc),ptee));
    p_load_expr((STACKELEM **)A_COND((*desc),ptte));

    cond = (int) R_COND((*desc),special);        /* SJ-ID */
    cond = COND | SET_EDIT(cond);  /* SJ-ID  setze Edit-Feld in Stackelement */

    PUSHSTACK(S_e,(STACKELEM) SET_ARITY(cond,2));

    DEC_REFCNT(desc);
    END_MODUL("ret_cond");
 }


/**********************************************************************/
/* p_ret_expr loest einen EXPRESSION - Descr. auf. Der hinter diesem  */
/* Descr. verborgene Ausdruck wird auf den E - Stack gelegt.          */
/**********************************************************************/

 void p_ret_expr(pdesc)
    STACKELEM pdesc;
 {
    PTR_DESCRIPTOR     desc = (PTR_DESCRIPTOR) pdesc;

    START_MODUL("p_ret_expr");

    p_load_expr((STACKELEM **) A_EXPR( (*desc),pte));
    DEC_REFCNT(desc);
    END_MODUL("p_ret_expr");
 }



