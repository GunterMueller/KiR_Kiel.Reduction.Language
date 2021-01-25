/*
 * $Log: pre-hfunc.c,v $
 * Revision 1.4  2001/07/02 15:32:29  base
 * compiler warning eliminated
 *
 * Revision 1.3  2001/03/13 16:06:34  base
 * signature of READ_M, READ_E, READ_V modified
 *
 * Revision 1.2  1994/06/15 11:21:22  mah
 * both closing strategies in one binary: CloseGoal
 *
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 *
 */

/**********************************************************************/
/*  hfunc.c                                                           */ 
/*                                                                    */
/*  MODULE der Preprocessing Phase                                    */
/*                                                                    */
/*          external :  list_end ; gen_BV_list ; gen_LBV_list ;       */
/*                      gen_CRBV_list ; element_VAR ; count_var ;     */
/*                      free_bound_var ; free_funct_name ;            */
/*                      hallo ;                                       */
/*                      fv_list_end ; gen_Fv_list ; element_FV ;      */
/*                      anz_FV ; gen_LREC_FV_list ;  free_FV_end ;    */
/*                      move_fv_e ; movefv ;                          */
/*                      gen_arg_list ; find_arg ; free_arg ;          */
/*                      snap_args ;                                   */
/*                      test_lrec_ind ; test_caserec ; test_var ;     */
/*                      test_cond ; test_abstr ;                      */
/*                      check_list_for_store ;                        */
/*                      hlist_end ; gen_BVAR ; gen_BVAR_pattern ;     */
/*                      free_BVAR ; gen_FV_LREC ; yyyyy ;             */    
/*                      test_fv ; find_free_var                       */     
/*                                                                    */                 
/*   Aufgerufen durch : subfunc ;                                     */
/*                      ea-create ;                                   */
/*                      lrecfunc ;                                    */
/*                      casefunc ;                                    */
/*                      condfunc                                      */
/*                                                                    */
/**********************************************************************/

#include <malloc.h>
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "list.h"


/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

/* extern int test_pre entfernt: 200993 mah */

extern list                 *VAR;                          /* rstate.c */
extern fvlist                *FV;
extern arglist              *ARG;
extern hlist               *BVAR;                
extern free_var_list    *FV_LREC;              

extern PTR_DESCRIPTOR          newdesc();
extern void            test_inc_refcnt();
extern void                   DescDump();
extern int                      mvheap();
extern void            test_dec_refcnt();
extern void                  ptrav_a_e();
extern void                    nst_var();
extern void                stack_error();
extern void                  ppopstack();

void movefv();                                 /* forward declaration */
void test_var();                               /* forward declaration */



/*********************************************************************/
/*                                                                   */
/*  VAR                 list_end                                     */
/*                      gen_BV_list                                  */
/*                      gen_LBV_list                                 */ 
/*                      gen_CRBV_list                                */
/*                      element_VAR                                  */
/*                      count_var                                    */
/*                      free_bound_var                               */
/*                      free_funct_name                              */
/*                      hallo                                        */
/*                                                                   */
/*********************************************************************/
/*********************************************************************/

/* struct {                                                          */
/*   PTR_DESCRIPTOR  var;                                            */
/*   STACKELEM       nr;                                             */
/*   struct list     *next;                                          */
/*  } list ;                                                         */


/* VAR ist definiert als Zeiger auf eine Struktur vom Typ list        */

/* Aufbau der Struktur :                                              */

/*   (1) initial leer                                                 */
/*   (2) neue Elemente werden immer am Anfang der Struktur            */
/*       eingehaengt                                                  */
/*   (3) das Entfernen von Elementen erfolgt immer am Anfang          */
/*       der Struktur                                                 */
/*   (4) vor dem Eintritt in einen Bindungsbereich wird ein sog.      */
/*       Blockendezeichen in die Struktur eingehaengt ; mit Hilfe     */
/*       dieser Markierung trennt man gebundene Variable              */
/*       verschiedener Bindungsbereiche( => Einteilung in Bloecke )   */
/*   (5) sind x_1, .. ,x_n die in dem betrachteten Bindungsbereich    */
/*       gebundenen Variablen , so wird die Struktur erweitert        */
/*       (5.1) sind die x_i durch ein SUB/WHEN gebunden , so wird     */
/*             fuer i=n, .. ,1 das Element ( x_i , NUM( n-i ) )       */
/*             erzeugt und in die Struktur eingehaengt                */
/*       (5.2) sind die x_i durch ein LREC gebunden ,so wird          */
/*             fuer i=n, .. ,1 das Element (x_i , p-lrec-ind_i )      */
/*             erzeugt und in die Struktur eingehaengt                */
/*   (6) beim Verlassen eines Bindungsbereichs wird der erste Block   */
/*       samt Blockendezeichen entfernt ;                             */
/*       Ausnahme : nach dem Anlegen eines Lrec - Descriptors wird    */
/*                  der erste Block nicht entfernt , da die Fkt -     */
/*                  namen fuer die Bearbeitung des Startausdruckes    */
/*                  noch benoetigt werden ; die beiden ersten Bloecke */
/*                  werden zu einem zusammengefasst ( das Blockende - */
/*                  zeichen wird in eine nichttrennende Markierung    */
/*                  umgewandelt ) ; der erste Block enthaelt nun die  */
/*                  gebundenen Variablen des Startausdrucks ; ist     */
/*                  dieser abgearbeitet , so werden die Fktnamen      */
/*                  entfernt                                          */

/* die Struktur enthaelt genau diejenigen Variablen , die bis zu      */
/* diesem Zeitpunkt durch eine Funktion gebunden werden , zusammen    */
/* mit den zugehoehrigen Nummervariablen / LREC-IND-Descriptoren ;    */
/* insbesondere enthaelt der erste Block die gebundenen Variablen     */
/* des aktuellen Bindungsbereiches                                    */


/**********************************************************************/
/* list_end traegt ein Blockendezeichen in die VAR - Liste ein        */
/**********************************************************************/

void list_end(close)
BOOLEAN close;  /* der Paramter close gibt an, ob die betrachtete Fkt.*/ 
                /* zu schliessen ist; diese Angabe wird in der Ende - */
                /* Markierung nachgehalten                            */ 
{
 register list *p;

 p = (list *)malloc(sizeof(list));
 p->var = (PTR_DESCRIPTOR)KLAA;

 if ( close ) p->nr = 1; else p->nr = 0;

 p->next = (list *)VAR;
 VAR = (list *)p;
 return;
}



/*********************************************************************/
/* gen_BV_list traegt die durch ein SUB/WHEN gebundenen Variablen in */
/* die VAR - Liste ein                                               */
/*********************************************************************/

void gen_BV_list(m,name)  
int           m;                             /* Anzahl der Variablen */
STACKELEM *name;             /* Liste , welche Namensdescr. enthaelt */
{
 register list *p;

 while ( m>0)
  {
    p=(list *)malloc(sizeof(list));
    p->var=(PTR_DESCRIPTOR)name[m + 1];
    p->nr=SET_ARITY(NUM,m-1);
    p->next= VAR;
    VAR = (list *)p;
    m--;
  }
   
 return;
} 


    
/**********************************************************************/
/* gen_LBV_list  traegt die durch ein LREC gebundenen Variablen in    */
/* die VAR - Liste ein                                                */
/**********************************************************************/

void gen_LBV_list(ldesc,name,LBV )

  PTR_DESCRIPTOR ldesc;              /* Verweis auf LREC - Descriptor */
  STACKELEM *name,*LBV;    /* Liste von Fkt.namen bzw LREC-IND - Desc.*/        
{
 list *hilf;
 int    i,j;

 i = 0 /*mah 070993 : war i=1 */;
 j = name[0];
 while ( i < j) /*mah 070993: war <=*/
  {
   hilf = (list *)malloc(sizeof(list));
   hilf->var = (PTR_DESCRIPTOR)name[i+1]; /*mah 070993: war [i]*/
   hilf->nr = LBV[i+1];                   /*mah 070993: war [i]*/
   hilf->next = VAR;
   VAR = (list *)hilf;
   i++;
  }
  hilf = (list *)malloc(sizeof(list));
  hilf->var = ldesc;
  hilf->nr = (STACKELEM)0;
  hilf->next = VAR;
  VAR = (list *)hilf;
  
 return;
}


/**********************************************************************/  
/* gen_CRBV_list traegt den durch ein CASEREC gebundenen Fkt.namen in */
/* die VAR - Liste ein                                                */
/**********************************************************************/

void gen_CRBV_list(desc,name)
STACKELEM        desc;                             /* CASEREC - Desc. */
PTR_DESCRIPTOR   name;                             /* Namensdescr.    */
{
 list *hilf;

  hilf = (list *)malloc(sizeof(list));
  hilf->var = name;
  hilf->nr =  desc;
  hilf->next = VAR;
  VAR = (list *)hilf;
  
 return;
}


/**********************************************************************/
/* element_VAR sucht in der VAR - Liste das i-te Vorkommen von x      */
/* Ergebnis : entsprechende Nr - Variable bzw entsprechender          */
/*             LREC_IND Descriptor                                    */
/**********************************************************************/

STACKELEM element_VAR(x,i)
PTR_DESCRIPTOR x;
int            i;
{
 register list *q;

 q=VAR;
  while(!(( i == 1) && (q->var == (PTR_DESCRIPTOR)x )))
   {
     if (q->var == (PTR_DESCRIPTOR)x )
        i--;
      q=q->next;
   }
     return(q->nr);
}


/*********************************************************************/
/* count_var liefert als Ergebnis die Anzahl der Vorkommen von x     */
/* in  - dem ersten Block der VAR - Liste , wenn j gleich 0 ist      */
/*     - der gesamten VAR - Liste , wenn j gleich 1 ist              */   
/*     - der inneren Maximalen Einheit ansonsten                     */ 
/* Aufruf : count_var(x,VAR)                                         */
/*********************************************************************/

int count_var(x,j)
PTR_DESCRIPTOR x;
int            j;
{
  register list *q;
  int            i;

  q = (list *)VAR;
  i = 0;
  switch ( j )
  {
   case 0 :/* bestimme die Anzahl der Vorkommen von x im ersten Block*/
           /* von VAR                                                */
              while (!(T_KLAA((STACKELEM)q->var)))
              {
                if (q->var == (PTR_DESCRIPTOR)x) i++;
                q = q->next;
              } 
              return( i );  

   case 1: /* bestimme die Anzahl der Vorkommen von x in VAR */
              while (q != NULL)
              {
                if (q->var == (PTR_DESCRIPTOR)x) i++;
                q = q->next;
              }
              return( i);                                  

   case 2 :/* bestimme die Anzahl der Vorkommen von x in der inneren */
           /* maximalen Fkt.einheit                                  */
              while(  !( ( (int)q->nr == 1 )   && ( T_KLAA((STACKELEM)q->var) ) ) ) 
              {   
                if (q->var == (PTR_DESCRIPTOR)x) i++;
                q = q->next;
              }          
              return(i);

  }
  post_mortem("count_var: index too high");
  return(0); /* never reached */
}


/**********************************************************************/
/* free_bound_var entfernt die durch ein SUB/WHEN gebundenen Variablen*/
/* aus der VAR - Liste                                                */
/**********************************************************************/

void free_bound_var()

{
 register list *p;

 while (!(T_KLAA((STACKELEM)VAR->var)))
   {
    p = (list *)VAR->next;
    free(VAR);
    VAR = (list *)p;
   }

 /* Blockende-zeichen entfernen    */
    p = (list *) VAR->next;
    free(VAR);
    VAR = (list *)p;

 return;
 
}


/**********************************************************************/
/* free_funct_name entfernt die durch ein LREC gebundenen Fktnamen    */
/* aus der VAR - Liste                                                */
/**********************************************************************/

void free_funct_name()

{
 register list *p;

 while (VAR->var != (STACKELEM)0)
   {
    p = (list *)VAR->next;
    free(VAR);
    VAR = (list *)p;
   }

 /* Trennsymbol entfernen    */
    p = (list *) VAR->next;
    free(VAR);
    VAR = (list *)p;

 return;
 
}


/**********************************************************************/
/* hallo wandelt ein Blockendezeichen in eine nichttrennende          */
/* Markierung um ( => Zusammenfassen zweier Bloecke )                 */
/**********************************************************************/

void hallo()

{
 register list *p;

 p = VAR;

 while (!(T_KLAA((STACKELEM)p->var)))
   p = p->next;

 p->var = (PTR_DESCRIPTOR)0;

return;

}





/**********************************************************************/
/*                                                                    */
/*     FV                          fvlist_end                         */
/*                                 gen_FV_list                        */
/*                                 element_FV                         */
/*                                 anz_FV                             */
/*                                 gen_LREC_FV_list                   */ 
/*                                 free_fv_end                        */
/*                                 move_fv_e                          */
/*                                 movefv                             */
/*                                                                    */
/**********************************************************************/
/**********************************************************************/


/* struct {                                                           */
/*  PTR_DESCRIPTOR var;                                               */
/*  int            nlbar;                                             */
/*  STACKELEM      nr;                                                */
/*  struct fvlist  *next;                                             */
/* } fvlist ;                                                         */

/* FV ist definiert als Zeiger auf eine Struktur vom Typ fvlist       */

/* Aufbau der Struktur :                                              */

/*   (1) initial leer                                                 */
/*   (2) neue Elemente werden immer am Anfang der Struktur            */
/*       eingehaengt                                                  */
/*   (3) das Entfernen von Elementen erfolgt immer am Anfang          */
/*       der Struktur                                                 */
/*   (4) vor dem Eintritt in einen Bindungsbereich unterscheidet man  */
/*       (4.1) ist die Funktion zu schliessen , so wird ein Block -   */
/*             endezeichen in die Struktur eingehaengt                */
/*       (4.2) ist die Funktion nicht .... kein ....                  */
/*   (5) trifft man beim Abarbeiten eines Fkt-rumpfes auf eine rel.   */
/*       freie Variable ,so ueberprueft man , ob diese im ersten      */
/*       Block der Struktur auftritt ; ist dies nicht der Fall , so   */
/*       erzeugt man eine neue Tildevariable und haengt ein entspr.   */
/*       Element in die Struktur ein                                  */
/*   (6) beim Verlassen einens Bindungsbereiches unterscheidet man    */
/*       (6.1) ist die Funktion zu schliessen , so wird der erste     */
/*             Block samt Blockendezeichen aus der Struktur entfernt  */
/*       (6.2) ist die Funktion nicht zu schliessen , so bleibt die   */
/*             Struktur unveraendert                                  */





/**********************************************************************/
/* fvlist_end traegt ein Blockendezeichen in die FV - Liste ein       */
/**********************************************************************/

void fvlist_end()

{
 register fvlist *p;

 p = (fvlist *)malloc(sizeof(fvlist));
 p->var = (PTR_DESCRIPTOR)KLAA;
 p->nlbar = 0;
 p->nr = 0;
 p->next = (fvlist *)FV;
 FV = (fvlist *)p;
 return;
}


/**********************************************************************/
/* gen_FV_list haengt in die FV - Liste das Element                   */
/*  ( x, nlbar ,TILDE ) ein und liefert als Ergebnis die neu          */
/* erzeugte Tilde - Variable                                          */
/**********************************************************************/

void gen_FV_list(x,nlbar,index)
PTR_DESCRIPTOR           x;
int            nlbar,index;
{
 register fvlist *hilf;
          int        i;
   
  /* lesen des Index der zuletzt erzeugten Tildevar. */
     if ( ! T_KLAA((STACKELEM)FV->var) )
         i = ARITY(FV->nr) + 1;
     else
         i=0; 

  hilf = (fvlist *)malloc(sizeof(fvlist));
  hilf->var = (PTR_DESCRIPTOR)x;
  hilf->nlbar = nlbar;
  hilf->nr = SET_ARITY(TILDE,i);
  hilf->next = FV;
  FV = (fvlist *)hilf;   

  /* gen_LREC_FV_list ruft gen_FV_list mit index = 0 auf             */ 
  /* ansonsten : neugenerierte Tildevariable wird auf S_a gelegt     */
    if ( index != 0 )
       PUSHSTACK(S_a,FV->nr);
  return;
}


/*********************************************************************/
/* element_FV testet,ob x mit i protects versehen,in der FV - Liste  */
/* vorkommt                                                          */
/* Ergebnis : entsprechende Tilde - Variable oder 0                  */
/*********************************************************************/

STACKELEM element_FV(x,i)
PTR_DESCRIPTOR x;
int            i;
{
 register fvlist *q;
 STACKELEM      erg;
 
 q=FV;
 erg=(STACKELEM)0;
 while (!(T_KLAA((STACKELEM)q->var)))
 {
  if (( q->var == (PTR_DESCRIPTOR)x ) && ( q->nlbar == i ))
  {
    erg=q->nr;
    break;
  }
    q=q->next;
 }
 return(erg);

}


/*********************************************************************/
/* anz_FV bestimmt die Maechtigkeit der "Menge",auf die p verweist . */
/*********************************************************************/
 
int anz_FV()

{
 register fvlist *q;
          int     i;
 
 i = 0;
 q = FV;
 while ((!T_KLAA((STACKELEM)q->var)))
 {
   i++;
   q=q->next;
 }
 return(i);
}


/*********************************************************************/
/* gen_LREC_FV_list traegt die fuer ein LREC ermittelten rel. freien */ 
/* Variablen in FV ein                                               */
/*********************************************************************/

void gen_LREC_FV_list(fv_lrec)
free_var_list *fv_lrec;        /* Liste der relativ freien Variablen */
                               /* der batrachteten LREC - Struktur   */
{                                 
  if ( fv_lrec-> next != NULL )
       gen_LREC_FV_list( fv_lrec->next );
  
  if ( element_FV( fv_lrec->var,fv_lrec->nlbar ) == NULL )
        gen_FV_list( fv_lrec->var,fv_lrec->nlbar, 0); 
                                                                    
  return;
}


/**********************************************************************/
/* free_fv_end entfernt in der FV - Liste  ein Blockende-Zeichen      */
/**********************************************************************/

void free_fv_end()

{
 register fvlist *hilf;

  hilf = (fvlist *) FV->next;
  free(FV);
  FV = (fvlist *)hilf;

  return;

}


/**********************************************************************/
/* move_fv_e bzw movefv legt die gefundenen relativ freien Variablen  */
/* plus evtl. vorhandene Protects auf den E-Stack;                    */
/* der erste Block der FV - Liste wird samt Blockendezeichen entfernt */
/**********************************************************************/

void move_fv_e()

{ 
 register fvlist *ret;

 ret = FV;

 while (!(T_KLAA((STACKELEM)FV->var)))
   FV = FV->next;

 /* FV zeigt jetzt auf den Anfang des freien Var. - Blocks der   */
 /* umgebenen Fkt                                                */

 FV = FV->next;
 movefv(ret);

 return;
}


/**********************************************************************/

void movefv(ret)
fvlist *ret;

{
 int i;

 if (T_KLAA((STACKELEM)ret->var))
 {
    free(ret);
    return;
 }
 else
   {
    movefv (ret->next);
    INC_REFCNT((PTR_DESCRIPTOR)ret->var);
    PUSHSTACK(S_e,ret->var);
    for ( i = 1 ; i <= ret->nlbar ; i++ )
        PUSHSTACK(S_e,SET_ARITY(PROTECT,1));
    free(ret);
    return;
   }

}

/**********************************************************************/
/*                                                                    */
/*     ARG                       gen_arg_list                         */
/*                               find_arg                             */
/*                               free_arg                             */
/*                                                                    */
/*                                                                    */
/**********************************************************************/

/* typedef struct ARGLIST                                             */
/* {                                                                  */
/*  PTR_DESCRIPTOR        lrec;                                       */
/*  free_var_list     *fv_lrec;                                       */
/*  struct ARGLIST       *next;                                       */
/* } arglist;                                                         */
                                                                         

/* ARG ist als Zeiger auf eine Struktur vom Typ arglist definiert     */   

/* In ARG wird fuer jede LREC - Struktur die Menge der in dieser      */
/* Struktur auftretenden  relativ freien Variablen nachgehalten.      */
/* Diese Variablenmenge wird evtl benoetigt, um angewandte Vorkommen  */
/* rekursiver Funktionen waehrend der linearen Abarbeitung des Prg.-  */
/* ausdruckes schliessen zu koennen.                                  */

 
/**********************************************************************/
/* gen_arg_list haengt in ARG ein Element der Form                    */
/*   ( LREC - Descriptor, Verweis auf Liste der rel. freien Var )     */
/* ein                                                                */
/**********************************************************************/

void gen_arg_list(ldesc)
PTR_DESCRIPTOR ldesc;
{
  arglist *p;
  
   p=(arglist*)malloc(sizeof(arglist));
   p->lrec=ldesc;

   /* beim Aufruf von gen_arg_list verweist FV_LREC auf die Liste  */
   /* der relativ freien Variablen des betrachteten LREC's         */ 
   /* FV_LREC = NULL moeglich                                      */

     p->fv_lrec=FV_LREC;
     p->next=ARG;
     ARG=p;
     FV_LREC=NULL;        

   return;
}


/*******************************************************************/
/* find_arg sucht in der ARG - Liste die zu ldesc gehoerige        */
/* Liste der rel. freien Variablen                                 */
/*******************************************************************/

free_var_list *find_arg(ldesc)
PTR_DESCRIPTOR ldesc;
{
  arglist *p;

  p=ARG;
  
  while ( p != NULL )
    if ( p->lrec == ldesc )
        break;
    else
        p=p->next;
 
  return(p->fv_lrec);
}


/**********************************************************************/
/* free_arg entfernt ein Element aus der ARG - Liste                  */
/**********************************************************************/

void free_arg()       
{
 arglist *p;
 free_var_list *q,*q1;


     q=ARG->fv_lrec;             
   
     if ( ! ( ( ARG->next != NULL ) && ( q == ARG->next->fv_lrec ) ) )
     {  
      /* Liste der rel freien Variablen freigeben */
        while ( q != NULL )
        {
          q1=q->next;
          free(q);
          q=q1;
       }

       p=ARG->next;
       free(ARG);
       ARG=p;
    }  /* ansonsten wird die Liste beim Loeschen des vorhergehenden */
       /* Elementes der ARG - Liste freigegeben                     */

  return;
}
   







/**********************************************************************/
/*                                                                    */
/*   Ausgabe                                                          */
/*                               ausgabe_modul                        */
/*                                                                    */
/**********************************************************************/


void ausgabe_modul(i,j)
int i,j;

{
 list *p;
 fvlist *q;
 FILE *aus;

 aus = fopen("AUSGABE","a");

 switch(i)
  {
   case 0: fprintf(aus,"nea-create\n");
               fprintf(aus,"************************** \n");
           break;
   case 1: fprintf(aus,"nsubfunc \n");
               fprintf(aus,"************************** \n");
           break;
   case 2: fprintf(aus,"lrecfunc \n");
               fprintf(aus,"************************** \n");
           break;
   case 3: fprintf(aus,"guard-body-func\n");
               fprintf(aus,"************************** \n");
           break;
   case 4: fprintf(aus,"FV vor nsubfunc \n");
           break;
   case 5: fprintf(aus,"FV nach nsubfunc \n");
           break;
   case 6: fprintf(aus,"FV vor lrecfunc \n");
           break;
   case 7: fprintf(aus,"FV nach lrecfunc \n");
           break;
   case 8: fprintf(aus,"FV vor casefunc \n");
           break;
   case 9: fprintf(aus,"FV nach casefunc \n");
           break;
   case 10: fprintf(aus,"Ende nsubfunc \n");
           break;
   case 11: fprintf(aus,"Ende lrecfunc \n");
           break;
   case 12: fprintf(aus,"Ende guard-body-func \n");
           break;
   case 13: fprintf(aus,"gebundene Var. nach Abarbeiten des Startausdruckes \n");
           break;
   case 14: fprintf(aus,"gebundene Var. vor Abarbeiten des Startausdruckes \n");
           break;
   case 15: fprintf(aus,"in nsubfunc gefundene freie Var.\n");
           break;
   case 16: fprintf(aus,"in test_var\n");
           break;
  }

 if (j == 0 )
 {
  p=(list *)VAR;
 fprintf(aus," gebundene Variable \n");
 fprintf(aus," ****************************************\n");

 while (p != NULL )
 {
 if( p->var == (STACKELEM) 0 )
     fprintf(aus,"markierung \n");
 if ( T_KLAA((STACKELEM)p->var) )
      fprintf(aus," Blockende \n"); 
 else   
       {
        fprintf(aus,"  var : %x \n",(int)p->var);
        if (T_NUM(p->nr))
           fprintf(aus," Nrn.var. mit Stelligkeit %d \n",ARITY(p->nr));
        else
           fprintf(aus," LREC_IND-Desc.  %x \n",p->nr);
       }
    p = p->next;
  }
 fprintf(aus,"**************************************\n");
}

if (j == 1)
{
 q=(fvlist *)FV; 
 fprintf(aus," freie Variable \n");
 fprintf(aus," ****************************************\n");

 while (!(T_KLAA((STACKELEM)q->var)))
   {
    fprintf(aus,"var  %x  \n",(int)q->var);
    fprintf(aus,"Anzahl nlbar  %d  \n",q->nlbar);
    fprintf(aus," Tilde mit Stelligkeit   %d  \n",ARITY(q->nr));
    q = q->next;
   }
 fprintf(aus,"**************************************\n");
}
 fclose(aus);
 return;
}





/**********************************************************************/
/*                                                                    */
/*  snap_args                                                         */
/*                                                                    */
/**********************************************************************/



/**********************************************************************/
/* snap_args bindet an das angewandte Vorkommen einer Funktion, welche*/
/* durch ein LREC oder ein CASEREC gebunden ist, die relativ freien   */
/* der zugehoerigen Struktur                                          */ 
/**********************************************************************/

void snap_args(x)      /* LREC-IND - Desc. oder CASEREC - Desc. */
STACKELEM x;
{
  arglist                  *p;
  free_var_list            *q;
  list                  *hilf;
  PTR_DESCRIPTOR    lrec_desc;
  PTR_DESCRIPTOR    crec_desc;
  PTR_DESCRIPTOR        snsub; /* mah 231193 */
  STACKELEM               var;
  int             nlbar,arity;
  int                  nono=0; /* lrec_ind im zugehoerigen lrec ? */

  START_MODUL("snap_args");

  if ( R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_LREC_IND )
  {   
     /* entspr. LREC - Desc. */
    lrec_desc = R_LREC_IND((*(PTR_DESCRIPTOR)x),ptdd);
   
     /* bestimme zugeh. Variablenliste */
    p = ARG;
    while( p->lrec!= lrec_desc ) p=p->next; 
    q = p->fv_lrec;        /* Liste der in der LREC- Struktur auf- */
                           /* tretenden rel. freien Variablen      */

#if 0
    { /* mah 281293 */
      int nfunc;
      PTR_HEAPELEM funclist;
      PTR_DESCRIPTOR func;

      nfunc = R_LREC(*lrec_desc, nfuncs);  /* Anzahl der Funktionen des lrec */
      funclist = R_LREC(*lrec_desc, ptdv); /* Zeiger auf die Ruempfe         */
      func = R_LREC_IND((*(PTR_DESCRIPTOR)x), ptf);
                                           /* Zeiger auf zu schliessende     */
                                           /* Funktion                       */

      funclist++;

      while (--nfunc > 0) { /* nfunc > 0 ! */
	if ((PTR_DESCRIPTOR)*funclist == func) {
	  /* Aufgerufene Funktion liegt im definierenden lrec */
	  nono = 1;
	  break;
	}
	funclist++;
      } /* while */
    } /* mah 281293 */
#endif
     if (( q != NULL ) && !nono)
     {
       arity = 0;
       while( q != NULL )
       {
	 var=(STACKELEM)q->var;
	 nlbar=q->nlbar;
      
	 /* da var moeglicherweise innerhalb des LREC's als gebundene Variable  */
	 /* auftritt ( Mehrfachdeklarationen ) , muss evtl. die Anzahl der      */
	 /* protects erhoeht werden                                             */
	 /* var mit entsprechend vielen protects versehen , wird an den         */
	 /* LREC-IND - Desc. gebunden , tritt in dem derzeitigen Bindungs -     */
	 /* bereich also rel. frei auf und wird somit in die Liste der rel.     */
	 /* freien Variablen dieses Bereiches aufgenommen und durch eine        */
	 /* Tilde - Variable ersetzt                                            */

	 hilf=VAR;
	 
	 /* leere fuer Conditionals angelegte Bloecke werden ueberlesen */
	  while ( T_KLAA((STACKELEM)hilf->var )) hilf=hilf->next;

	 if ( lrec_desc != hilf->var )
         {                   

	   /* betrachte nur diejenigen Variablenbloecke ,die durch Funktionen */
	   /* innerhalb des LREC's gebunden sind                              */

	   while(lrec_desc != hilf->var )
           {
	     if ( hilf->var == (PTR_DESCRIPTOR)var )
	       nlbar++;
	     hilf=hilf->next;
	   }
          
         }     

         /* ueberpruefe die durch das lrec gebundenen Variablen auf Namens- */
         /* gleichheit                                                      */  

	 while(( ! T_KLAA((STACKELEM) hilf->var )) &&
	       ( hilf->var !=(PTR_DESCRIPTOR) 0 )   )
	 {
	   if ( hilf->var == (PTR_DESCRIPTOR)var )
	     nlbar++;
	   hilf=hilf->next;
	 }
            
         /* lege die Variable var mit nlbar protects versehen auf den E-Stack */
         /* und rufe test_var auf                                             */

	 PUSHSTACK(S_e,var);
	 INC_REFCNT((PTR_DESCRIPTOR)var);
	 for ( ; nlbar > 0 ; nlbar -- )
	   PUSHSTACK(S_e,SET_ARITY(PROTECT,1));

	 test_var(READSTACK(S_e));

	 arity++;
	 q=q->next;
       } /* ende while ( q!= NULL) */
     
       PUSHSTACK(S_a,x);     /* LREC-IND - Desc. */

       /* mah 231193 */
       /*  Funktionsdeskriptor fuer sub~ anlegen */
       
       if ((snsub = newdesc()) == NULL)
	 post_mortem("subfunc: Heap out of space");


       DESC_MASK(snsub,1,C_EXPRESSION,TY_SNSUB);                               
       L_FUNC((*snsub),special) = 0;
       L_FUNC((*snsub),nargs)   = arity;
       L_FUNC((*snsub),namelist)= NULL;

       PUSHSTACK(S_hilf, POPSTACK(S_a));             /* das Lrec-ind */
       if (mvheap(A_FUNC((* snsub),pte)) == 0)       /* Auslagern */
	 post_mortem("subfunc: Heap out of space");
       
       PUSHSTACK(S_a, (STACKELEM) snsub);
       PUSHSTACK(S_a,SET_ARITY(SNAP,(arity+1)));
     }
     else /* q == NULL */
       PUSHSTACK(S_a,x);
    
     END_MODUL("snap_args");
     return;
  } 
  else
  {   
    crec_desc=(PTR_DESCRIPTOR)x;
    p=ARG;
    while( p->lrec!= crec_desc ) p=p->next; 

    /* bestimme zugeh. Variablenliste */

       q=p->fv_lrec;

     if ( q != NULL )
     {
       arity=0;
       while( q != NULL )
       {
         var=(STACKELEM)q->var;
         nlbar=q->nlbar;
      
        /* da var moeglicherweise innerhalb des CASEREC's als gebundene Variable  */
        /* auftritt ( Mehrfachdeklarationen ) , muss evtl. die Anzahl der      */
        /* protects erhoeht werden                                             */
        /* var mit entsprechend vielen protects versehen , wird an den         */
        /* CASEREC - Desc. gebunden , tritt in dem derzeitigen Bindungs -     */
        /* bereich also rel. frei auf und wird somit in die Liste der rel.     */
        /* freien Variablen dieses Bereiches aufgenommen und durch eine        */
        /* Tilde - Variable ersetzt                                            */

           hilf=VAR;

           while ( T_KLAA((STACKELEM)hilf->var )) hilf=hilf->next;

           if ( crec_desc != (PTR_DESCRIPTOR)hilf->nr )
           {

            /* betrachte nur diejenigen Variablenbloecke ,die durch Funktionen */
            /* innerhalb des CASEREC's gebunden sind                              */

               while(crec_desc != (PTR_DESCRIPTOR)hilf->nr )
               {
                if ( hilf->var == (PTR_DESCRIPTOR)var )
                     nlbar++;
                hilf=hilf->next;
               }
          
            }  
         /* ueberpruefe  durch caserec gebundene Variable auf Namens- */
         /* gleichheit                                                */  

               if ( hilf->var == (PTR_DESCRIPTOR)var )
                    nlbar++;
            
         /* lege die Variable var mit nlbar protects versehen auf den E-Stack */
         /* und rufe test_var auf                                             */
   
            INC_REFCNT((PTR_DESCRIPTOR)var);
   
            PUSHSTACK(S_e,var); 

            for ( ; nlbar > 0 ; nlbar -- )
                PUSHSTACK(S_e,SET_ARITY(PROTECT,1));

            test_var(READSTACK(S_e));

            arity++;
            q=q->next;
        } /* ende while ( q!= NULL) */
     
        PUSHSTACK(S_a,x);     /* CASEREC - Desc. */
        PUSHSTACK(S_a,SET_ARITY(SNSUB,1)); /* mah 281093 */
        PUSHSTACK(S_a,SET_ARITY(SNAP,(arity+1)));
      }
      else
       PUSHSTACK(S_a,x);
    
      END_MODUL("snap_args");
      return;
   }
}





/**********************************************************************/
/*                                                                    */
/*   TEST                     test_lrec_ind                           */  
/*                            test_caserec                            */
/*                            test_var                                */
/*                            test_cond                               */
/*                            test_abstr                              */
/*                                                                    */
/**********************************************************************/



/**********************************************************************/
/* test_lrec_ind() ueberprueft,ob ein durch ein LREC gebundener       */
/* Fkt-name in einer zu schliessenden Funktion  vorkommt ;            */
/* ist dies der Fall , so wird auf dem A-stack ein Ausdruck der Form  */
/* snap(n+1) p-fv_1 .. p-fv_n p-lrec-ind in transponierter Form       */
/* aufgebaut                                                          */
/**********************************************************************/

void test_lrec_ind(x) 
STACKELEM x;
{
  int               test;
  register list       *q;

      
   START_MODUL("test_lrec_ind");
   q = VAR;
   test = 0;


#if HEAP  

   if ( T_KLAA((STACKELEM) q->nr ))
             test=0;
  else
  {

   while( !T_KLAA((STACKELEM) q->var) )
          q=q->next;
   q=q->next;

   /* q zeigt nun auf den zweiten Block der VAR - Liste  */
   /* teste , ob  x im zweiten Block vorkommt            */

   while  ( ! (T_KLAA((STACKELEM) q->var )) &&
           ( (STACKELEM)q->var != (STACKELEM) 0 ) )
   {
     if ( (STACKELEM)q->nr== x )
           test=1;
     q=q->next;
   }

  /* ist q->var = 0 , so ist der zu x gehoerige Fkt-name in dem      */
  /* betrachteten Bindungsbreich relativ frei , da der Startausdruck */
  /* bearbeitet wird ( geb. Variable des Startausdrucks : Fkt-namen  */
  /* plus gebundene Var der umgebenden Funktion )                    */

    if ( (STACKELEM)q->var == (STACKELEM) 0 )
       test=0;
   }
#else                         

   /* teste, ob x in einer zu schiessenden Funktion auftritt */

   while ( ! (   ( (T_KLAA((STACKELEM) q->var ))  ||  ((STACKELEM)q->var == (STACKELEM) 0) )
               &&( (int)q->nr == 1 )  ) )
   {
     if ( (STACKELEM)q->nr== x )
           test=1;
     q=q->next;
   }

#endif
/*    if ( test == 1 )
       PUSHSTACK(S_a,x);
    else
--->    sbs    <--- */

  /***********************/
  /* A  C  H  T  U  N  G */
  /***********************/

  /* Dieser Test war urspruenglich auskommentiert, so   */
  /* dass in jedem Fall snap_args ausgefuehrt wurde.    */
  /* Sollten mit der Rueckaenderung Probleme auftreten, */
  /* auf keinen Fall wieder hier auskommentieren, denn  */
  /* das fuehrte das Tildekonzept total ad absurdum.    */
  /* mah 281293                                         */

  if ( test == 1 )
    PUSHSTACK(S_a,x);
  else
    snap_args(x);

  END_MODUL("test_lrec_ind");
  return;
}


/**********************************************************************/
/*  analog zu test_lrec_ind                                           */
/**********************************************************************/

void test_caserec(x) 
STACKELEM x;        /* CASEREC - Desc. */
{
  int                test;
  register list        *q;

      
   START_MODUL("test_caserec");
   q = VAR;
   test = 0;


   /* teste, ob x in einer zu schiessenden Funktion auftritt */
   /* wenn ja ,muss x geschlossen werden                     */

   while  ( ! ( ( (T_KLAA((STACKELEM) q->var ))       ||  
                ((STACKELEM)q->var == (STACKELEM) NULL )      ) && ( (int)q->nr == 1 ) ) )
   {
     if ( (STACKELEM)q->nr== x )
           test = 1;
     q = q->next;
   }


    if ( test == 1 )
       PUSHSTACK(S_a,x);
    else
      snap_args(x);

   END_MODUL("test_caserec");
   return;
}



/**********************************************************************/
/* test_var prueft,ob die gegebene Variable im betrachteten Bindungs- */
/* bereich gebunden , relativ frei oder frei ist und ersetzt sie      */
/* durch die entsprechende Nummer/Tilde-variable bzw LREC-IND Desc.   */
/* gefundene rel. freie Var. werden in FV eingetragen                 */
/**********************************************************************/

void test_var(x)
STACKELEM x;
{
  STACKELEM             y,z;
  int       n,nbv=0,nvar,nmfe=0;


  START_MODUL("test_var");

 /* n    : Anzahl der moeglicherweise vorhandenen protects            */
 /* y    : verweist nach der while - Schleife auf einen Namensdesc.   */
 /* nbv  : Anzahl der Vorkommen von y in BV                           */  
 /* nvar : Anzahl der Vorkommen von y in VAR                          */
 /* nmfe : Anzahl der Vorkommen von y in der inneren max. Fkt.einheit */

    n = 0;
    while( T_PROTECT(( y = POPSTACK(S_e))) )  n++ ;

    nbv  = count_var((PTR_DESCRIPTOR) y , 0 );
    nvar = count_var((PTR_DESCRIPTOR) y , 1 );
    nmfe = count_var((PTR_DESCRIPTOR) y , 2 );

    if ( nbv > n )         /* d.h. y ist gebundene Variable   */
       {
         /* ersetzt y durch die entsprechende Nr. - Variable  */
         /* bzw durch den entsprechenden LREC-IND Desc.       */
         /* entsprechende Ref.-count - Behandlung             */
          
          z = element_VAR(y,n + 1);
/*         if( ! T_NUM(z))
              {
              INC_REFCNT((PTR_DESCRIPTOR)z);
              }                                               
mah 31/08/93: Anpassung an Refcounting der lred*/

         DEC_REFCNT((PTR_DESCRIPTOR)y);
         PUSHSTACK(S_a,z);
         END_MODUL("test_var");
         return;
       }

    if (( nvar > n) )       /* d.h. y ist relativ freie Variable */
       {
         /* suche in VAR das entsprechende definierende Vorkommen */

  
          z = element_VAR(y,n + 1);

         /* ist y eine durch ein lrec gebundene Variable,so       */
         /* ersetze y durch den zugehoerigen LREC-IND Descriptor  */
         /* und trage y nicht in die Liste der relativ freien     */
         /* Variablen ein                                         */

         if (!T_NUM(z))
          {
/*              INC_REFCNT((PTR_DESCRIPTOR)z); 090993 mah */
              DEC_REFCNT((PTR_DESCRIPTOR)y);

          /* test_lrec_ind() ueberprueft,ob die Funktion zu     */
          /* schliessen ist  ; ist dies der Fall , so           */
          /* werden die in dem betrachteten LREC vorkommenden   */
          /* relativ freien Variablen ueber ein SNAP an die     */
          /* Funktion gebunden                                  */
          /* analog : test_caserec                              */                                                              
 
             if ( R_DESC((*(PTR_DESCRIPTOR)z),type ) == TY_LREC_IND )
                test_lrec_ind(z);                  
             else
                test_caserec(z);
                                          
          }
         else
         
         /* ist y eine durch ein sub gebundene Variable,so teste;   */
         /* ob y schon Element von FV ist ; wenn ja , ersetze y     */
         /* durch die entsprechende Tilde - Variable ; wenn nein ,  */
         /* generiere eine neue Tilde - Variable ; trage das        */
         /* Element (y,(n-nbv),TILDE) in FV ein  und ersetze        */
         /* y durch die neue Tilde Variable (generieren und         */
         /* Ersetzen erfolgt in gen_FV_list                         */
            { 
             DEC_REFCNT((PTR_DESCRIPTOR)y);
             if ( ( z=element_FV(y,(n-nmfe)) ) != (STACKELEM)0 )
                   PUSHSTACK(S_a,z);
              else
                  {
                   gen_FV_list(y,(n-nmfe),1);
                  }
            }
      }
  else                           /* d.h. y ist freie Variable     */

      /* nst_var erzeugt einen Variablendescriptor und legt       */
      /* diesen auf den A Stack                                   */
      {
/*
       test_pre=1;  entfernt 200993 mah 
*/
       nst_var(y,(n - nvar));  
      } 
           
  END_MODUL("test_var");
 return;
}


/**********************************************************************/
/* test_cond ueberprueft , ob ein Conditional auf oberster Ebene in   */
/* einem Startausdruck auftritt                                       */
/*********************************************************************/

int test_cond()
{
 STACKELEM         x;
 int        i,result;

  START_MODUL("test_cond");

  i = 0;
  result = 1;

  x = READ_M( i); 
  while ( ! T_KLAA(x) )
  {
     if ( (T_LREC(x)) &&  (ARITY(x) == 1 )) 
     {
        result = 0;
        break;     
      }
     else
        if ( T_PRELIST(x) || T_LREC(x) || T_PM_CASE(x) || T_PM_WHEN(x) )
          break;
        else
        {
           i++;  
           x = READ_M( i);
        }
  }

     END_MODUL("test_cond");
     return(result);
}
     

/**********************************************************************/
/* test_abstr testet , ob mehrere lambda - Abstraktionen dierkt auf-  */
/* einander folgen ;                                                  */
/* liefert als Ergebnis eine                                          */
/*  1 , falls mehrere geschachtelte sub's vorliegen                   */
/*  2 , falls ein oder mehrere sub's gefolgt von einem case vorliegen */
/*  0 , ansonsten                                                     */
/**********************************************************************/
                 
int test_abstr()
{
 int        tiefe,arity;
 STACKELEM            x;

 tiefe = 0;
 arity=READ_ARITY(S_e);

 main_e:

  tiefe += arity;
  x = READ_E( tiefe);
  
  if ( T_SUB(x) )
  {
    arity=ARITY(x);
    goto main_e;
  }
  if ( ( T_PM_CASE(x) ) && ( !( T_PM_REC(x) ) ) )
     return(2);

 /* ansonsten */

  if ( tiefe == arity )
      return(0);
  else
      return(1);
}

    
/**********************************************************************/
/*                                                                    */
/*  check_list_for_store                                              */
/*                                                                    */
/**********************************************************************/
 
/**********************************************************************/
/* check_list_for_store analysiert eine Liste auf dem A-Stack.        */
/* Ein TRUE wird zurueckgegeben, falls die Liste weder Konstruktoren  */
/* noch Nummern- oder Tildevariablen enthaelt                         */
/**********************************************************************/

BOOLEAN check_list_for_store()
{
  register unsigned short arity;
  register STACKELEM liste;
  register STACKELEM x;
  BOOLEAN  result;

  START_MODUL("check_list");

  liste = POPSTACK(S_a);            /* LIST */
  arity = ARITY(liste);

  for ( ; arity > 0; arity --)
  {
    x = READSTACK(S_a);
    if T_CON(x) break;    /* alles Gruende, das eine Liste nicht ausgelagert */
    if T_TILDE(x) break;
    if T_NUM(x) break;    /* werden darf */
    /* mah 291293: der lred-Praeprozessor verbietet das auslagern aller */
    /* Zeiger der class C_EXPRESSION, nicht nur von LREC_IND, REC und   */
    /* COND.                                                            */
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),class) == C_EXPRESSION)) break;
#if 0
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_LREC_IND)) break;
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_REC)) break;
    if (T_POINTER(x) && (R_DESC((* (PTR_DESCRIPTOR) x),type) == TY_COND)) break;
#endif
    PUSHSTACK(S_hilf,POPSTACK(S_a));
  }
  result = (arity == 0);  /* result == TRUE heisst: darf ausgelagert werden */

  while (arity < ARITY(liste))
  {
    PUSHSTACK(S_a,POPSTACK(S_hilf));     /* Liste wieder korrekt auf A legen */
    arity++;
  }
  PUSHSTACK(S_a,liste);             /* LIST */
  END_MODUL("check_list");

  return(result);
}




/**********************************************************************/
/*                                                                    */
/*   BVAR                     hlist_end                               */  
/*                            gen_BVAR                                */
/*                            gen_BVAR_pattern                        */
/*                            free_BVAR                               */
/*                                                                    */
/**********************************************************************/

                                                                        
/* typedef struct HLIST                                               */
/* {                                                                  */
/*  PTR_DESCRIPTOR         var;                                       */
/*  struct HLIST         *next;                                       */
/* } hlist;                                                           */

/* BVAR ist als Zeiger auf eine Struktur vom Typ hlist definiert      */

/* der Aufbau und die Verwaltung von BVAR sind im wesentlichen analog */
/* zu dem Aufbau und der Verwaltung von VAR ( BVAR ist eine " abgema- */
/* gerte " Version von VAR ).                                         */
                                                                  



/**********************************************************************/
/* hlist_end traegt ein Blockendezeichen in die BVAR - Liste ein      */
/**********************************************************************/

void hlist_end()

{
 register hlist *p;

  p = (hlist *)malloc(sizeof(hlist));
  p->var = (PTR_DESCRIPTOR)KLAA;
  p->next = BVAR; ;
  BVAR= p;
 return;
}


/**********************************************************************/
/* gen_BVAR traversiert n durch ein SUB / LREC gebundene Variable von */
/* E nach A und traegt sie gleichzeitig in BVAR ein                   */
/**********************************************************************/

void gen_BVAR(n)
int n;
{
 STACKELEM     x;
 hlist     *hilf;
 
   hlist_end();    /* Blockendezeichen einhaengen */
   
   for( ; n > 0 ; n-- )
   {
     x=POPSTACK(S_e);    /* durch SUB / LREC gebundene Var. */

     hilf=(hlist*)malloc(sizeof(hlist));
     hilf->var=(PTR_DESCRIPTOR)x;
     hilf->next=BVAR;
     BVAR=hilf;
     
     PUSHSTACK(S_a,x);
   }
   return;
 }
   

/**********************************************************************/
/* gen_BVAR_pattern traversiert ein pattern von E nach A und traegt   */
/* auftretende gebundene Variable in BVAR ein                         */
/**********************************************************************/

void gen_BVAR_pattern()
{
 STACKELEM    x;
 hlist    *hilf;
 int      arity;

  hlist_end();   /* Blockendezeichen eintragen */

 main_e:
  x=READSTACK(S_e);

  if (T_CON(x))  
  {
    if (ARITY(x) >0 ) /* bel. Konstruktor */
    {
      PPOPSTACK(S_e);
      PUSHSTACK(S_m,x);
      PUSHSTACK(S_m1,x);
      goto main_e;
    }
    else
    {
      PPOPSTACK(S_e);
      PUSHSTACK(S_a,x);
      goto main_m;         
    }
  }
  if ( ( T_POINTER(x) )                                 && 
       ( (R_DESC((*(PTR_DESCRIPTOR)x),type)) == TY_NAME ) )
  {
    /* => durch pattern gebundene Variable wird in BVAR eingetragen  */

      PPOPSTACK(S_e);   

      hilf = (hlist*)malloc(sizeof(hlist));
      hilf->var = (PTR_DESCRIPTOR)x;
      hilf->next = BVAR;
      BVAR = hilf;
     
      PUSHSTACK(S_a,x);
      goto main_m;
  }
  else
  {
   /* ansonsten nur traversieren */
    
      PPOPSTACK(S_e);
      PUSHSTACK(S_a,x);
      goto main_m;
  } 

 main_m:
  x = READSTACK(S_m);
  arity = ARITY(x);

  if ( ( T_PM_WHEN(x) ) && ( arity  == 3 ) )   /* evtl. n-stelliges pattern */
  {
    WRITE_ARITY(S_m,arity-1);  /* when(2) auf S_e  */
    return;
  }
  else
  {
    if (arity > 1 )                                             
    {
      WRITE_ARITY(S_m,arity-1);
      goto main_e;
    }
    else
    {
      PPOPSTACK(S_m);
      x = POPSTACK(S_m1);
      PUSHSTACK(S_a,x);
      goto main_m;
     }
  }
}


/**********************************************************************/
/* free_BVAR entfernt die durch ein SUB/WHEN/LREC gebundenen Variablen*/
/* aus der BVAR - Liste                                               */
/**********************************************************************/

void free_BVAR()
{
 register hlist *p;

   while (!(T_KLAA((STACKELEM)BVAR->var)))
   {
     p = BVAR->next;
     free(BVAR);
     BVAR=p;
   }

   /* Blockende-zeichen entfernen    */
      p = BVAR->next;
      free(BVAR);
      BVAR= p;

   return;
 
}




/**********************************************************************/
/*                                                                    */
/*  FV_LREC                   gen_FV_LREC                             */  
/*                            yyyyy                                   */
/*                            test_fv                                 */
/*                            find_free_var                           */
/*                                                                    */
/**********************************************************************/

/* typedef struct FREE_VAR_LIST                                       */
/* {                                                                  */
/*  PTR_DESCRIPTOR         var;                                       */
/*  int                  nlbar;                                       */
/*  struct FREE_VAR_LIST *next;                                       */
/* } free_var_list;                                                   */ 

/* FV_LREC ist als Zeiger auf eine Struktur vom Typ free_var_list     */
/* definiert                                                          */

/* Fuer jede LREC - Struktur werden die in den Fkt.definitionen auf-  */
/* tretenden relativ freien Variablen bestimmt und in FV_LREC ge-     */
/* sammelt. Der Algorithmus find_free_var traversiert die Fkt.def.    */
/* von E nach A; ueber test_fv wird fuer jede Variable bestimmt,ob    */
/* in der LREC - Struktur gebunden, relativ frei oder absolut frei    */
/* auftritt.                                                          */
                                                                        
/* analog dazu : Behandlung von CASEREC's                             */



/**********************************************************************/
/* gen_FV_LREC testet , ob \(n) var bereits Element von FV_LREC ist ; */
/* ist dies nicht der Fall , so wird das Element eingetragen          */
/**********************************************************************/

void gen_FV_LREC(n,var)
int              n;
PTR_DESCRIPTOR var;
{
  free_var_list *hilf;

    hilf = FV_LREC;  

    while(hilf != NULL )
    {
      if ( ( hilf->var == var ) && ( hilf->nlbar == n ) )
            break;
      else
           hilf = hilf->next;
    }

   if ( hilf == NULL )
   {
    /* d.h. \(n) var ist nicht Element von FV_LREC        */ 
    /* das element ( var , nlbar , NULL ) wird an FV_LREC */
    /* angehaengt                                         */
        hilf = (free_var_list *) malloc(sizeof(free_var_list));
        hilf->var = var;
        hilf->nlbar = n;
        hilf->next = FV_LREC;
        FV_LREC = hilf;
   }
   
   return;
}


/***************************************************************************/
/* yyyyy bestimmt fuer ein LREC/CASEREC die in ARG eingetragene Menge der  */
/* relativ freien Variablen und traegt diese unter Beruecksichtigung der   */
/* Bindungen in FV_LREC ein                                                */
/***************************************************************************/

void yyyyy( v , x )
STACKELEM                  v,x;                 /* v : Namens - Descriptor */
                                      /* x LREC-IND / CASEREC - Descriptor */
{
 PTR_DESCRIPTOR           desc;
 free_var_list           *hilf;  
 STACKELEM                 var;
 int            nlbar, anz_var;
 list                       *q;


  START_MODUL("yyyyy");

  if ( R_DESC((*(PTR_DESCRIPTOR)x),type) == TY_LREC_IND )
      desc = R_LREC_IND((*(PTR_DESCRIPTOR)x),ptdd);     /* LREC-Descriptor */
  else
      desc = (PTR_DESCRIPTOR)x;

  /* bestimme zugehoehrige Liste der rel .freien Variablen */
     hilf = find_arg(desc);
     while( hilf != NULL )
     {
        var = (STACKELEM)hilf->var;
        nlbar = hilf->nlbar;

        /* bestimme Anzahl der Vorkommen von var in VAR bis zu dem Block , */
        /* welcher v enthaelt                                              */
           q = VAR;
           anz_var  = 0;
           while( q->nr != x )
           {
             if ( q->var == (PTR_DESCRIPTOR) var )
                 anz_var++;
             q = q->next;
           }

        /* var mit nlbar plus anz_var protects versehen wird in FV_LREC    */
        /* aufgenommen                                                     */
           gen_FV_LREC( ( nlbar + anz_var ) , (PTR_DESCRIPTOR)var );
           
        hilf = hilf->next;
     }

  END_MODUL("yyyyy");
  return;
}
 

/***************************************************************************/
/* test_fv traversiert eine Variable von E nach A und testet hierbei, ob   */
/* sie in der betrachteten LREC-Struktur relativ frei auftritt. Ist die    */
/* betrachtete Variable durch ein aeusseres LREC gebunden, so wird die     */
/* Menge der in der aeusseren Struktur auftretenden relativ freien Var.    */
/* in FV_LREC eingetragen ( Realisierung ueber yyyyy )                     */    
/* analog hierzu : durch CASEREC gebundene Variable                        */
/***************************************************************************/

void test_fv() 
{
 hlist                 *p;
 list                  *q;
 int  n,anz_hlist,anz_var;
 STACKELEM          var,x;

   START_MODUL("test_fv");

   /* nach der while Schleife              */
   /* n   : Anzahl der protects            */
   /* var : Verweis auf Namensdescriptor   */

      n = 0;
      while ( T_PROTECT( (var=POPSTACK(S_e) ) ) ) n++;

   /* nach der while - Schleife            */
   /* anz_hlist : Anzahl der Vorkommen von var in BVAR */
      anz_hlist = 0;
      p = BVAR;
      while ( p != NULL )
      {
        if ( p->var == (PTR_DESCRIPTOR)var )
            anz_hlist++;
         p=p->next;
       }

   /* var ist durch eine Funktion innerhalb des LREC's gebunden,  */             
   /* wenn gilt : n < anz_hlist                                   */
   /*  => (i) \(n) var auf den A - Stack legen                    */   

     if( n>= anz_hlist )
     {
       /* nach der while - Schleife                                  */
       /* anz_var : Anzahl der Vorkommen von var in VAR              */
          anz_var = 0;
          q = VAR;
          while ( q != NULL )
          {
           if ( q->var ==(PTR_DESCRIPTOR)var )
                anz_var++;
           q=q->next;
          } 

       /* var wird durch eine das LREC umgebende Funktion gebunden ,  */
       /* wenn gilt : anz_hlist <= n < anz_hlist + anz_var            */
       /*  => (i)  \(n) var auf den A - Stack legen                   */
       /*     (ii) \(n-anz_hlist) var in FV_LREC eintragen            */
   
         if ( anz_var > ( n- anz_hlist) )
         {
             x = element_VAR(var,(n - anz_hlist + 1));
             if ( T_NUM(x) )
             {
               /* d.h. var ist durch ein SUB/WHEN gebunden  */
               /*  => var mit n-anz_hlist protects wird in FV_LREC aufgenommen */
                 gen_FV_LREC((n-anz_hlist),(PTR_DESCRIPTOR)var) ;
             }
             else
              /* var ist durch LREC / CASEREC gebunden ; die relativ */
              /* freien Variablen des LREC / CASEREC muessen in      */
              /* FV_LREC aufgenommen werden                          */
                 yyyyy(var,x);

        }
        /* var ist freie Variable,wenn gilt : n> anz_var + anz_hlist  */
        /*  => (i) \(n) var auf den A - Stack legen                   */
     }
   
     /* \(n) var auf den A - Stack legen  */
        PUSHSTACK(S_a,var);
        for( ; n>0 ; n-- )
            PUSHSTACK(S_a,SET_ARITY(PROTECT,1));

   END_MODUL("test_fv");
   return;
}


/***************************************************************************/
/*  find_free_var traversiert die Fkt-definitionen eines LREC's bzw die    */
/*  when - Klauseln eines CASEREC's von E nach A und bestimmt dabei die    */
/*  relativ freien Variablen ; anschliessend wird der urspruengliche       */
/*  Zustand wieder hergestellt                                             */
/*  index = 1 => LREC traversieren                                         */
/*  index = 2 => CASEREC traversieren                                      */
/***************************************************************************/

 void find_free_var(index)
 int index;
 {
  STACKELEM     x,y;
  int       n,arity;
  

   START_MODUL("find_free_var");

    /* Initialisieren der Liste */
       BVAR=NULL;

   /* Markierung auf S_m legen */
      PUSHSTACK(S_m,DOLLAR);

    if ( index == 1)
    {
     /* Fkt-definitionen werden auf relativ freie Variable untersucht */
     /* Startausdruck darf nicht traversiert werden                   */
 
        PPOPSTACK(S_e);
	if (CloseGoal) {
	  PUSHSTACK(S_m,SET_ARITY(LREC,2));   /* auch die goal-expression! */
	  PUSHSTACK(S_m1,SET_ARITY(LREC,3));  /* LREC(3) auf s_m1 legen    */
	}
	else {
	  PUSHSTACK(S_m,SET_ARITY(LREC,1));   /* LREC(1) auf S_m legen     */
	  PUSHSTACK(S_m1,SET_ARITY(LREC,2));  /* LREC(2) auf s_m1 legen    */
	}
        y=POPSTACK(S_e);                      /* LIST(n) von S_e entfernen */
        n=ARITY(y);

       /* die n durch das LREC gebundenen Funktionsnamen werden von E */
       /* nach A traversiert und gleichzeitig in BVAR eingetragen   m */

           gen_BVAR( n );
 
       PUSHSTACK(S_a,y);                      /* LIST(n) auf S_a legen     */
       PPOPSTACK(S_e);                        /* LIST(n) von S_e entfernen */
       PUSHSTACK(S_m,y);                      /* LIST(n) auf S_m legen     */
       PUSHSTACK(S_m1,y);                     /* LIST(N) auf S_m1 legen    */
   }
   else
   {
     /* when -Klauseln werden auf relativ freie Variable untersucht */
 
        x=POPSTACK(S_e);
        PUSHSTACK(S_m,x);                   /* PM_REC(m+1+1) auf S_m legen */
        PUSHSTACK(S_m1,x);                    
        n=ARITY(x);

       /* der durch das CASEREC gebundene Funktionsname wird von E */
       /* nach A traversiert und gleichzeitig in BVAR eingetragen  */

           gen_BVAR( 1 );
                                 
       /* Stelligkeit des CASEREC auf S_m herunterzaehlen */

          WRITE_ARITY(S_m,n-1);     
   }
   main_e:

   x=READSTACK(S_e);

   if( T_SUB(x) )
   {
     PPOPSTACK(S_e);                                                         
     PUSHSTACK(S_m,SET_ARITY(SUB,1));             /* SUB(1) auf S_m legen  */
     PUSHSTACK(S_m1,x);                           /* SUB(n) auf S_m1 legen */
     n=ARITY(x);

     /* die n-1 durch das SUB gebundenen Variablen werden von E  */
     /* nach A traversiert und gleichzeitig in BVAR eingetragen */

         gen_BVAR( (n-1) );

     goto main_e;
    }

   if( T_LREC(x) )
   {
    PPOPSTACK(S_e);                                                           
    PUSHSTACK(S_m,SET_ARITY(LREC,2));         /* LREC(2) auf S_m legen     */
    PUSHSTACK(S_m1,x);                        /* LREC(3) auf s_m1 legen    */
    y=POPSTACK(S_e);                          /* LIST(n) von S_e entfernen */
    n=ARITY(y);

    /* die n durch das LREC gebundenen Funktionsnamen werden von E */
    /* nach A traversiert und gleichzeitig in BVAR eingetragen     */

        gen_BVAR( n );
                                                                            
    PUSHSTACK(S_a,y);                         /* LIST(n) auf S_a legen     */
    PPOPSTACK(S_e);                           /* LIST(n) von S_e entfernen */
    PUSHSTACK(S_m,y);                         /* LIST(n) auf S_m legen     */
    PUSHSTACK(S_m1,y);                        /* LIST(N) auf S_m1 legen    */
    goto main_e;                              /* auf dem E - Stack folgen  */ 
  }                                           /* die Fkt.-definitionen     */             
  
  if ( T_PM_REC(x) )
  {
    PPOPSTACK(S_e);
    PUSHSTACK(S_m,x);                         /* PM_REC(n) auf S_m legen   */
    PUSHSTACK(S_m1,x);                    
    n=ARITY(x);

    /* der durch das CASEREC gebundene Funktionsname wird von E */
    /* nach A traversiert und gleichzeitig in BVAR eingetragen    */

        gen_BVAR( 1 );
                                 
    /* Stelligkeit des CASEREC auf S_m herunterzaehlen */

        WRITE_ARITY(S_m,n-1);     
    
    goto main_e;
  }

  if ( T_PM_WHEN(x) )
  {
   PPOPSTACK(S_e);                                                           
   PUSHSTACK(S_m,x);                         /* WHEN(n) auf S_m legen      */
   PUSHSTACK(S_m1,x);                        /* WHEN(n) auf S_m1 legen     */

   /* pattern wird von E nach A traversiert,dabei werden auftretende    */
   /* gebundene Variable in BVAR eingetragen                            */

        gen_BVAR_pattern();

   goto main_e;
  }
if ( T_PROTECT(x) ||                          /* Variable */       
    ((T_POINTER(x)) && (R_DESC((*(PTR_DESCRIPTOR)x),type)) == TY_NAME))
   {
    /* Namensdescriptor mit n ( >= 0 ) protects versehen wird von E nach   */
    /* A traversiert und daraufhin geprueft,ob er zu der Menge der relativ */
    /* freien Variablen des Ausgangs-LREC's gehoehrt                       */
  
       test_fv();

    goto main_m;
   }
                                                                            
   if (T_CON(x))  
   {
      if (ARITY(x) >0 )                                /* bel. Konstruktor */
         {
     	  PPOPSTACK(S_e);
          PUSHSTACK(S_m,x);
          PUSHSTACK(S_m1,x);
          goto main_e;
         }
      else
        {
         PPOPSTACK(S_e);
         PUSHSTACK(S_a,x);
         goto main_m;         
        }
   }


   if (T_POINTER(x))
      {
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;
      }
    
   if (T_ATOM(x))
      {
       PPOPSTACK(S_e);
       PUSHSTACK(S_a,x);
       goto main_m;    
      }

   main_m:

    x=READSTACK(S_m);
    arity = ARITY(x);
                                                                            
    if (arity > 1)               /* dh die Arity des Konstruktors war >= 2 */
    {
      WRITE_ARITY(S_m,arity-1);
      goto main_e;
    }                                                                       
    if (arity > 0)                                 /* DOLLAR hat arity = 0 */
    {
      PPOPSTACK(S_m);
      x = POPSTACK(S_m1);
      PUSHSTACK(S_a,x);
      if ( T_LREC(x) || T_SUB(x) || T_PM_WHEN(x) || T_PM_REC(x) )
            free_BVAR();
      goto main_m;
     }
   /* arity=0 => DOLLAR auf S_m */

    PPOPSTACK(S_m);                                     /* DOLLAR entfernen */
    PTRAV_A_E;                 /* Ausgangssituation auf dem E - Stack wieder*/
                               /*                                herstellen */
   
    if ( index == 1 )
    { 
       PPOPSTACK(S_e);                         /* LREC(2) von S_e entfernen */
       PUSHSTACK(S_e,SET_ARITY(LREC,3));           /* LREC(3) auf S_e legen */
    }    
   
    END_MODUL("find_free_var");
    return;
   }

                                            




/**********************************************************************/
/*                                                                    */
/* anz_tilde bestimmt die Anzahl der Elemente des obersten Blockes    */
/* des Tilde - Stacks                                                 */
/*                                                                    */
/**********************************************************************/
/**********************************************************************/



int anz_tilde()
{
 STACKELEM x;
 int i,j;

 START_MODUL("anz_tilde");
 i=0;
 if ( SIZEOFSTACK_TILDE >0  )
 {
  while ( ! T_KLAA( (x=READSTACK(S_tilde))) )
  {
   i++;
   PUSHSTACK(S_hilf,POPSTACK(S_tilde));
  }
  for ( j=i ; j>0 ; j-- )
   PUSHSTACK(S_tilde,POPSTACK(S_hilf));
 }
 END_MODUL("anz_tilde");
 return(i);
}

