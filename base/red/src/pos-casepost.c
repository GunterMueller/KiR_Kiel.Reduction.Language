/* $Log: pos-casepost.c,v $
 * Revision 1.4  1995/03/01 16:50:24  rs
 * more DBUG-output added
 *
 * Revision 1.3  1995/01/16  15:08:59  rs
 * UH PM bugfixes
 *
 * Revision 1.2  1994/03/29  17:09:16  mah
 * *** empty log message ***
 *
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 * */
/**********************************************************************/
/*                                                                    */
/* MODULE DER POSTPROCESSING PHASE                                    */
/* casepost.c : externals:keine                                       */
/* casepost.c : internals : switch_post , nomat_post                  */
/*                                                                    */
/**********************************************************************/


#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include "list.h"
#include "case.h"
#include "dbug.h"


/**********************************************************************/



extern void p_load_expr();         /* rhinout.c */
extern void ILIST_end();           /* postfunc.c */
extern void gen_ILIST_caserec();
extern void inc_protects();
extern void trav_a_hilf();         /* rtrav.c   */
extern void trav_a_e();            /* rtrav.c   */
extern void trav_hilf_a();         /* rtrav.c   */
extern void gen_ILIST_case();

extern STACKELEM st_switch_clos(); /* p-fail.c */

#if UH_ZF_PM
extern StackDesc S_pm;
#endif /* UH_ZF_PM */

extern void DescDump();
extern void test_inc_refcnt();
extern void ppopstack();
extern void test_dec_refcnt();
extern void ret_expr();
extern T_HEAPELEM * newbuff();
extern int newheap();
extern void stack_error();

/**********************************************************************/

extern postlist *ILIST;
/* US */
extern BOOLEAN xxx;
extern BOOLEAN part_sub_var;
extern int anz_sub_bv;
extern STACKELEM *argumentlist;
extern int otherwise;

/**********************************************************************/
/*                                                                    */
/*  switch_post: postprocessed einen Switch-Descriptor                */
/*                                                                    */
/*    aufgerufen durch : ea_retrieve                                  */
/*                                                                    */
/*    In der Postprocessing-Phase tritt nur dann ein Switch-          */
/*    Deskriptor auf,falls entweder der Reduktionszaehler             */
/*    abgelaufen war oder falls dem case waehrend der Reduktion       */
/*    zu wenig Argumente zugefuehrt wurden.                           */
/*                                                                    */
/*    In switch_post wird daher der urspruengliche case-Ausdruck      */
/*    auf dem E-Stack aufgebaut.                                      */
/*                                                                    */
/**********************************************************************/

void switch_post(desc)

PTR_DESCRIPTOR desc; /* Der Switch-Deskriptor */

{

  PTR_HEAPELEM *match_list,*code,*offset;
  PTR_DESCRIPTOR                    y,pf;
  STACKELEM                     casetype;
  int                j,anz_when,anz_bv,i;      
  BOOLEAN                          ncase;
  postlist                    *ret_ILIST;
  int                             anz_pv;


  START_MODUL("switch_post");

  casetype = (STACKELEM)R_SWITCH((*desc),casetype);
  /* casetype in {case,case_e,caserec} */

  ncase = (R_SWITCH((*desc),anz_args) > 1);
  /* ncase = Funktion mehrstellig? */

  DBUG_PRINT ("switch_post", ("Yo, ncase %i", ncase));

  anz_when = R_SWITCH((*desc),nwhen);
  /* anz_when = Anzahl der when-Klauseln + 1 (fuer END bzw. OTHERWISE) */

  match_list = (PTR_HEAPELEM *)R_SWITCH((*desc),ptse);
  /* match_list verweist auf die Liste der Match-Deskriptoren */
  /* des Switch.                                              */

  /* US */    

  /* initialiesieren */           

  ret_ILIST = NULL; 

  if ( otherwise == 1 )
  {
    /* Der otherwise - Ausdruck wurde in der Preprocessing-Phase in eine   */
    /* aequivalente pi - Abstraktion umgeformt :                           */
    /*                                                                     */
    /* OTHERWISE expr  =>   WHEN  pattern  true  expr                      */
    /*                                                                     */
    /* pattern enthaelt die sub-gebundenen Variablen plus einem oder       */
    /* mehreren SKIP's                                                     */
    /* Rekonstruktion der urspruenglichen Struktur                         */
 
    anz_when--;

    pf = (PTR_DESCRIPTOR)match_list[anz_when];
    /* pf entspricht dem letzten Eintrag in match_list.                 */
    /* Dieser ist ein Match-Deskriptor, der fuer den otherwise-Ausdruck */
    /* generiert wurde.                                                 */
    
    p_load_expr((STACKELEM **)A_MATCH((*pf),body));
    /* otherwise-Ausdruck einlagern */
    
    PUSHSTACK(S_e,SET_ARITY(PM_OTHERWISE,1));
    
    /* Trennsymbol generieren */
    
    ILIST_end();    
    
    /* Adresse merken */
    
    ret_ILIST = ILIST;
    
    code = (PTR_HEAPELEM *)R_MATCH((*pf),code);
    /* code entspricht dem Code-Vektor von pf,in dem das         */
    /* Originalpattern,die Patternvariablen (= die in der        */
    /* when-Klausel gebundenen Variablen) und der Match-Code     */
    /* eingetragen sind.                                         */
    
    offset = (PTR_HEAPELEM *)(code + (int)code[NAMELIST]);
    /* offset verweist auf die Liste der Patternvariablen: */

    anz_bv = (int)offset[0];  /* = Anzahl der Patternvariablen */

    if ( part_sub_var )   
      /* Argumente fuer  durch sub gebundene Variable in */ 
      /* ILIST eintragen                                 */
      gen_ILIST_case((STACKELEM *)argumentlist , anz_sub_bv , FALSE , NULL);
    else   
      /* " Patternvariable " in ILIST eintragen          */
      gen_ILIST_case((STACKELEM *) offset , anz_bv , FALSE , NULL);
  }
  else
  {
    
    /* otherwise-Ausdruck bzw. END auf den E-Stack :*/

    pf = (PTR_DESCRIPTOR)match_list[anz_when];
    /* pf entspricht dem letzten Eintrag in match_list.                */
    /* Ist dieser ein Match-Deskriptor,so wurde ein otherwise-Ausdruck */
    /* spezifiziert,handelt es sich um einen NULL-Zeiger, so war kein  */
    /* otherwise-Ausdruck angegeben.                                   */
    
    if (pf != NULL)
      /* also war ein otherwise-Ausdruck angegeben */
    {
      p_load_expr((STACKELEM **)A_MATCH((*pf),body));
      /* otherwise-Ausdruck einlagern */

      PUSHSTACK(S_e,SET_ARITY(PM_OTHERWISE,1));

      /* Trennsymbol generieren */

      ILIST_end();      

      /* Adresse merken */

      ret_ILIST = ILIST;
    }

    else /* also war kein otherwise-Ausdruck angegeben */

      if (casetype != WHEN_WITHOUT_CASE)
	/* simple when: no end needed */
	PUSHSTACK(S_e,PM_END);

  } /* if ( otherwise ) */

  /* when-Klauseln auf den E-Stack : */
                      
  for (i = anz_when - 1 ; i > 0 ; i--)
    /* anz_when - 1 , da das END bzw. der otherwise-Ausdruck schon */
    /* bearbeitet ist .                                            */
  {
    pf = (PTR_DESCRIPTOR)match_list[i];
    /* pf verweist auf den betrachteten Match-Deskriptor. */

    code = (PTR_HEAPELEM *)R_MATCH((*pf),code);
    /* code entspricht dem Code-Vektor von pf,in dem das         */
    /* Originalpattern,die Patternvariablen (= die in der        */
    /* when-Klausel gebundenen Variablen) und der Match-Code     */
    /* eingetragen sind.                                         */
    
    offset = (PTR_HEAPELEM *)(code + (int)code[NAMELIST]);
    /* offset verweist auf die Liste der Patternvariablen: */
    
    anz_bv = (int)offset[0];  /* = Anzahl der Patternvariablen */

    /* Trennsymbol generieren */

    ILIST_end();
    /*                          */

    if ( ret_ILIST == NULL )
      ret_ILIST = ILIST;

    /* Patternvariable in ILIST eintragen*/

    /* US */
    if ( part_sub_var )   
    {       
      /* Argumente bzw. durch sub gebundene Variable in */ 
      /* ILIST eintragen                                */
#if 0
      gen_ILIST_case((STACKELEM *)argumentlist , anz_sub_bv , TRUE ,ret_ILIST ) ;

      /* Patternvariable in ILIST eintragen */     
      if ( (anz_bv - anz_sub_bv ) > 0 ) 
	gen_ILIST_case((STACKELEM *)offset ,anz_bv - anz_sub_bv , FALSE , NULL);
#else
      if ( (anz_bv - anz_sub_bv ) > 0 ) 
	gen_ILIST_case((STACKELEM *)offset ,anz_bv - anz_sub_bv , TRUE , ret_ILIST);

      gen_ILIST_case((STACKELEM *)argumentlist , anz_sub_bv , FALSE ,NULL ) ;
#endif      
    }
    else    
      /* Patternvariable in ILIST eintragen */     
      if ( anz_bv > 0 )
	gen_ILIST_case((STACKELEM *)offset ,anz_bv , TRUE , ret_ILIST );
    
    /* body expression einlagern: */
    p_load_expr((STACKELEM **)A_MATCH((*pf),body));

    if (R_MATCH((*pf),guard) != (PTR_HEAPELEM)0)
      /* guard expression einlagern : */
      p_load_expr((STACKELEM **)A_MATCH((*pf),guard));
    else
      PUSHSTACK(S_e,SA_TRUE); 
    
    if (ncase)
    {
      y = (PTR_DESCRIPTOR)code[OPAT];
      /* Verweis auf das 'Original'-pattern; in der Preprocessing- */  
      /* Phase wurden die verschiedenen Pattern einer when-Klausel */
      /* als Listenstruktur ausgelagert.                           */   
      
      INC_REFCNT(y);
      /* da in ret_list der refcount dekrementiert wird */
      ret_expr(y);
      
      if (T_LIST(READSTACK(S_e)))
	PPOPSTACK(S_e);
      /* US */ 
      if ( xxx || part_sub_var )
      {
	/* anz_sub_bv Variable werden vom E - Stack entfernt => */
	/* das urspruengliche pattern wird rekonstruiert        */
	
	for ( j = anz_sub_bv ; j > 0 ; j-- )
	{
	  y = (PTR_DESCRIPTOR)POPSTACK(S_e);
	  DEC_REFCNT(y);
	}

	anz_pv = (int)R_SWITCH((*desc),anz_args) - anz_sub_bv;
	PUSHSTACK(S_e,SET_ARITY(PM_WHEN,anz_pv + 2));
      }
      else
	PUSHSTACK(S_e,SET_ARITY(PM_WHEN,(int)R_SWITCH((*desc),anz_args) + 2));
    }
    else
    {
      PUSHSTACK(S_e,(PTR_DESCRIPTOR)code[OPAT]);
      /* Pointer auf Originalpattern */
      
      INC_REFCNT((PTR_DESCRIPTOR)code[OPAT]);
      
      /* jetzt noch WHEN(3) auf den E-Stack : */
      
      PUSHSTACK(S_e,SET_ARITY(PM_WHEN,3));
    }
                       
  } /* when-Klauseln liegen jetzt auf dem E-Stack */
  


  /* Zum Schluss das Switch bzw. das When auf den E-Stack .           */
  /* Dabei wird das Edit-Feld des SWITCH_Konstruktors aus dem Switch- */
  /* Descriptor 'geholt'.                                             */

  if (T_PM_REC(casetype))
  {
    y = (PTR_DESCRIPTOR)match_list[anz_when + 1];
    PUSHSTACK(S_e,y);
    /* den Funktionsnamen */    
    INC_REFCNT((PTR_DESCRIPTOR)y);
  }

  if (casetype != WHEN_WITHOUT_CASE)
    /* simple when: no additional constructor needed */
    PUSHSTACK(S_e,casetype);                 

  if ( otherwise )
    WRITE_ARITY(S_e,READ_ARITY(S_e) - 1 );

  DEC_REFCNT(desc);

  END_MODUL("switch_post");

  return;

}




/*********************************************************************/
/*                                                                   */
/*   nomat_post: postprocessed einen Nomatch-Deskriptor              */
/*                                                                   */
/*   aufgerufen durch: ea_retrieve                                   */
/*                                                                   */
/*   Ein Nomatch-Deskriptor tritt in der Postprocessing-Phase dann   */
/*   auf,falls in der Processing-Phase entweder das Matching fuer    */
/*   keine when-Klausel eines case geklappt hat,oder falls das       */
/*   Matching geklappt hat,der guard aber nicht zu TRUE oder FALSE   */
/*   reduziert werden konnte.                                        */
/*                                                                   */
/*   Der Nomatch-Deskriptor enthaelt folgende Eintraege:             */
/*                                                                   */
/*    ptsdes : entsprechender Switch-Deskriptor                      */
/*                                                                   */
/*    act_nomat : gibt die when-Klausel an,fuer die das Matching     */
/*                geklappt hat,der guard sich aber werder zu TRUE    */
/*                noch zu FALSE reduziert hat.                       */
/*                                                                   */
/*         Bei act_nomat = 1 werden zwei Faelle unterschieden:       */
/*                                                                   */
/*         1) guard_body = 0: das Matching hat fuer keine            */
/*                                    when-Klausel geklappt.         */
/*                                                                   */
/*         2) guard_body != 0: das Matching hat fuer die durch       */
/*                         act_nomat bezeichnete when-Klausel        */
/*                         klappt ; hinter guard_body stehen         */
/*                         der reduzierte guard und der body,in      */
/*                         dem die Patternvariablen dem Argument     */
/*                         entsprechend substituiert wurden.         */
/*                                                                   */
/*    Der case-Ausdruck wird nun folgendermassen auf dem E-Stack     */
/*    aufgebaut:                                                     */
/*                                                                   */
/*    1) Hat kein Pattern mit dem Argument gematcht,so wird der      */
/*       urspruengliche case-Ausdruck aufgebaut (Aufruf von          */
/*       switch_post(ptsdes)).                                       */
/*                                                                   */
/*    2) Das Pattern der durch act_nomat bezeichnenten when-         */
/*       Klausel hat gepasst,der guard konnte aber nicht zu TRUE     */
/*       oder FALSE ausgewertet werden.                              */
/*       In diesem Fall wird :                                       */
/*         1: das END auf den E-Stack gelegt,                        */
/*         2: als otherwise-Ausdruck das urspruengliche case mit     */
/*            einer Applikation des Argumentes aufgebaut,            */
/*         3: alle when-Klauseln,die nach der act_nomat-Klausel      */
/*            definiert wurden,( 'von hinten') auf den E-Stack       */
/*            gelegt,                                                */
/*         4: der body der act_nomat-Klausel,in dem alle Pattern-    */
/*            variable dem Argument entsprechend substituiert wurden,*/
/*            der reduzierte guard und statt des Patterns ein        */
/*            SKIP ('.') auf den E-Stack gelegt,und                  */
/*         5: das SWITCH auf den E-Stack gelegt.                     */
/*                                                                   */
/*                                                                   */
/*  Beispiel:                                                        */
/*                                                                   */
/*   ap case                                                         */ 
/*      when pat_1 guard guard_1 do body_1                           */
/*                                                                   */
/*           ..           ..        ..                               */
/*                                                                   */
/*      when pat_n guard guard_n do body_n                           */
/*      end                                                          */
/*   ARG                                                             */
/*                                                                   */
/*  und act_nomat = i                                                */
/*                                                                   */
/*                        ==>                                        */
/*                                                                   */
/*   ap case                                                         */
/*      when   .     guard red_guard_i do mod_body_i                 */
/*      when pat_i+1 guard guard_i+1 do body_i+1                     */
/*                                                                   */
/*           ..           ..        ..                               */
/*                                                                   */
/*      when pat_n guard guard_n do body_n                           */
/*      otherwise ap case                                            */ 
/*                   when pat_1 guard guard_1 do body_1              */
/*                                                                   */
/*                        ..           ..        ..                  */
/*                                                                   */
/*                   when pat_n guard guard_n do body_n              */
/*                   end                                             */
/*                ARG                                                */
/*   ARG                                                             */
/*                                                                   */
/*********************************************************************/

void nomat_post(desc)

     PTR_DESCRIPTOR desc; /* der Nomatch-Deskriptor */

{
  VOLATILE PTR_HEAPELEM *match_list=NULL;
  PTR_HEAPELEM *guard_body,*code,*offset;
  PTR_DESCRIPTOR              sw_desc,pf;
  PTR_DESCRIPTOR arg_desc, sub_desc=NULL;
  STACKELEM                        y,x=0;
  int              j,anz_when=0,anz_bv,i;
  int                dim,act_nomat,arity;
  BOOLEAN                     when,ncase;
  postlist               *ret_ILIST=NULL;
  int                      reason,anz_pv;      
  STACKELEM                     casetype;
  int                            index=0;


  START_MODUL("nomat_post");

  guard_body = (PTR_HEAPELEM *)R_NOMAT((*desc),guard_body);
  arg_desc = (PTR_DESCRIPTOR) guard_body[3];

  act_nomat = R_NOMAT((*desc),act_nomat);
  /* act_nomat gibt,falls vorhanden,die when-Klausel an,fuer die  */
  /* das Argument gematcht wurde,deren guard aber nicht zu        */
  /* TRUE oder FALSE reduziert werden konnte .                    */
  /* act_nomat = -1 : Das Pattern einer when-Funktion hat nicht   */
  /* gepasst.                                                     */
  /* act_nomat = -2 : Der guard einer when-Funktion konnte nicht  */
  /* zu TRUE oder FALSE reduziert werden.                         */ 

  sw_desc = (PTR_DESCRIPTOR)R_NOMAT((*desc),ptsdes);

  /* sw_desc verweist auf den zugehoerigen Switch-Deskriptor */

  reason =  R_NOMAT((*desc),reason);

  /* reason = 3 oder 4: das Pattern einer pi-Abstraktion hat gepasst,    */
  /* der guard-Ausdruck wurde aber nicht zu TRUE oder FALSE reduziert.   */
  /* (4: der Reduktionszaehler war abgelaufen)                           */ 
  /* In diesem Falls wird ein case-Ausdruck aufgebaut, der als           */
  /* otherwise-Ausdruck dei Applikation der urspruenglichen case-        */
  /* Funktion auf das Argument enthaelt.                                 */ 

  /* reason = 1:  keines der Pattern hat gepasst,(oder,falls ein Pattern */
  /* gepasst hat,war der guard-Ausdruck zu FALSE reduziert worden) es    */
  /* wird der urspruengliche case-Ausdruck aufgebaut.                    */  
  /* ( dieser Fall wird bereits im ea_retrieve behandelt !!! )           */

  /* reason = 2: das Matching mit dem Pattern der act_nomat-ten pi-Ab-   */
  /* straktion hat UNDECIDED ergeben. In diesem Fall werden die ersten   */
  /* (act_nomat - 1) pi-Abstraktionen mit aufgebaut.                     */  
  /* siehe reason gleich 3 oder gleich 4                                 */

  when = (R_DESC((*sw_desc),type) == TY_MATCH );                                                    

  /* when = TRUE: Nomatch-Deskriptor wurde fuer eine pi-Abstraktion angelegt */
  /* when = FALSE:Nomatch-Deskriptor wurde fuer eine case-Funktionangelegt   */

  if (when)                      
  {
    arity = R_MATCH((*sw_desc),code)[STELLIGKEIT];
    ncase = ( arity > 1);
  }
  else
  {  
    arity = R_SWITCH((*sw_desc),anz_args);
    ncase = ( arity > 1);
  }

  DBUG_PRINT("nomat_post", ("ncase: %i", ncase));

  if (!when)
  {                      

    /* US */ /* initialisieren  von ret_ILIST */
    
    ret_ILIST = NULL;
    
    /* leeren Block fuer   fuer OTHERWISE generieren  , da       */
    /* im ea_retrieve nach dem Traversieren des Otherwise -      */
    /* Ausdruckes von E nach A aus der Bindungsliste fuer die    */
    /* gebundenen Variablen ( ILIST ) der erste Block entfernt   */
    /* wird                                                      */
        
    /* reason > 1 =>                                              */
    /* in diesen Faellen wird die urspruengliche Applikation des  */
    /* case-Ausdruckes auf das/die Argument/e als otherwise-Aus-  */
    /* druck zurueckgegeben.                                      */

    ILIST_end();


    /* US */ 
    if( ncase )
    {                                                               
      /* teste, ob es sich bei der case - Funktion um eine zu -     */
      /* sammengefasste Funktion handelt                            */   
      /* wenn ja => setze part_sub_var auf TRUE ( default Wert : FALSE )*/ 
      
      casetype=(STACKELEM)R_SWITCH((*sw_desc),casetype);      
      index=(int)R_SWITCH((*sw_desc),nwhen) + 1;
      if( T_PM_REC(casetype) )  index++;
      sub_desc = (PTR_DESCRIPTOR )( R_SWITCH((*sw_desc),ptse) )[index];   
      otherwise = (int)( R_SWITCH((*sw_desc),ptse) )[index + 1];
      
      part_sub_var = ( sub_desc != NULL );
    }   

    if ( part_sub_var )
    {
      /* die ehemals durch sub gebundenen Variablen muessen substituiert */
      /* werden , die urspruengliche case - Funktion muss restauriert    */
      /* werden    => Realisierung siehe Aufloesen von Closure - Desc.   */
      /* fuer Case - Funktionen und switch_post                          */
      
      if ( ( x = st_switch_clos( desc )) == NULL )    
	post_mortem("nomat_post : heap out of space" );
      else
	PUSHSTACK(S_e,x);
    }
    else
    {
      /* auf dem E - Stack wird ein Ausdruck der Form    */
      /* ap argumente  p_switch aufgebaut                */
      
      PUSHSTACK(S_e,(STACKELEM)sw_desc);
      INC_REFCNT(sw_desc);         
      
      if ( ncase )
      {
	/* in der Pro. Phase wurden die Argument fuer die mehrstellige */
	/* case - Fkt in einer Liste zusammengefasst                   */
	/* diese Liste wird aufgeloest                                 */
	
	i = R_LIST((*arg_desc),dim);
	for ( j = 0 ; j < i ; j++ )
	{
	  PUSHSTACK(S_e,(x = R_LIST((*arg_desc),ptdv)[j]));
	  if T_POINTER(x) INC_REFCNT((PTR_DESCRIPTOR)x);
	}  
      }
      else
      {
	/* arg_desc auf S_e legen */
        
	PUSHSTACK(S_e,(STACKELEM)arg_desc);
	T_INC_REFCNT(arg_desc);
	i = 1;
      }

      /* Applikator auf den E - Stack legen */
      
      PUSHSTACK(S_e ,SET_ARITY(AP,i +1));

      
    } /* ende !part_sub_var */


    /* Applikator fuer den otherwise-Ausdruck auf den E-Stack: */

    PUSHSTACK(S_e,SET_ARITY(PM_OTHERWISE,1));

    /* us */       
    /*                              &&&&&&            */

    if ( part_sub_var ) 
    {
      
      otherwise = (int)( R_SWITCH((*sw_desc),ptse) )[index + 1];
      
      anz_sub_bv = (int) R_FUNC((*sub_desc),nargs);
      
      /* arity muss berichtigt werden: */
      arity -= anz_sub_bv;
      
      /* in argumentlist werden die zur Substitution benoetigten */
      /* Argumenten eingetragen                                  */
      
      if ( ( argumentlist = (STACKELEM *)newbuff( anz_sub_bv + 1 ) ) == (STACKELEM)NULL )
	post_mortem("nomat_post: heap out of space ");
      
      argumentlist[0] = anz_sub_bv;                          

      for ( i = anz_sub_bv ; i > 0 ; i--)
      {
	argumentlist[anz_sub_bv - i + 1] = ( x = (STACKELEM) R_LIST((*arg_desc),ptdv)[i-1]) ; 
	if ( T_POINTER(x) )
	  INC_REFCNT((PTR_DESCRIPTOR)x);
      }
    } /* ende if( part_sub_var ) */
    

    /* jetzt die benoetigten when-Klauseln: */
    
    anz_when = R_SWITCH((*sw_desc),nwhen); /* = Anzahl aller when-  */
    /*   Klauseln            */
    if ( otherwise ) anz_when--;  
    
    /* match_list verweist auf die Liste der Match-Deskriptoren des */
    /* Switch : */
    match_list = (PTR_HEAPELEM *)R_SWITCH((*sw_desc),ptse);
    
    /* benoetigte when-Klauseln auf den E-Stack : */
                      
    for (i = anz_when - 1 ; i > act_nomat ; i--)
      /* anz_when - 1,da das otherwise schon 'da' ist ;               */
      /* >act_nomat,da nur die nach der act_nomat-Klausel definierten */
      /* when-Klausekn benoetigt werden.                              */
    {
      pf = (PTR_DESCRIPTOR)match_list[i];
      /* pf verweist auf den jeweiligen Match-Deskriptor */
    
      code = (PTR_HEAPELEM *)R_MATCH((*pf),code);
      /* code entspricht dem code-Vektor von pf,in dem das       */
      /* Originalpattern,der Match-Code und die Liste der        */
      /* Patternvariablen ( = der in der when-Klausel gebundenen */
      /* Variablen ) eingetragen sind.                           */
      
      /* offset verweist auf die Liste der Patternvariablen: */
      offset = (PTR_HEAPELEM *)(code + (int)code[NAMELIST]);
      
      anz_bv = (int)offset[0];  /* = Anzahl der Patternvariablen */
      
      
      /* Trennsymbol generieren */
      
      ILIST_end();
      
      /* Adresse merken */

      ret_ILIST = ILIST;
      
      /* Patternvariable in ILIST eintragen*/
    
      /* US */
      if ( part_sub_var )   
      {       
	/* Argumente bzw  durch sub gebundene Variable in */
	/* ILIST eintragen                                */
#if 0	
	gen_ILIST_case((STACKELEM *)argumentlist , anz_sub_bv , TRUE ,ret_ILIST );


        /* Patternvariable in ILIST eintragen  */    
	if ((anz_bv - anz_sub_bv) > 0 )
	  gen_ILIST_case((STACKELEM *)offset ,anz_bv - anz_sub_bv , FALSE, NULL );
#else
	if ((anz_bv - anz_sub_bv) > 0 )
	  gen_ILIST_case((STACKELEM *)offset ,anz_bv - anz_sub_bv , TRUE, ret_ILIST );

	gen_ILIST_case((STACKELEM *)argumentlist , anz_sub_bv , FALSE , NULL );
#endif
      }
      else    
	/* Patternvariable in ILIST eintragen  */    
	if ( anz_bv > 0 )
	  gen_ILIST_case((STACKELEM)offset ,anz_bv , TRUE , ret_ILIST );

    
      p_load_expr((STACKELEM **)A_MATCH((*pf),body));
      /* Einlagern des body der betrachteten when-Klausel */
      
      if (R_MATCH((*pf),guard) != (PTR_HEAPELEM)0)
	p_load_expr((STACKELEM **)A_MATCH((*pf),guard));
      /* Einlagern des guard */
      else
	PUSHSTACK(S_e,SA_TRUE);
      
      if (ncase)
      {
	y = (STACKELEM)code[OPAT];
	/* Verweis auf das 'Original'-pattern; in der Preprocessing- */  
	/* Phase wurden die verschiedenen Pattern einer when-Klausel */
	/* als Listenstruktur ausgelagert.                           */   
	
	INC_REFCNT((PTR_DESCRIPTOR)y);
	/* da in ret_list der refcount dekrementiert wird */
	ret_expr(y);
	
	if (T_LIST(READSTACK(S_e)))
	  PPOPSTACK(S_e);
	
	/* US */            dim = (int) R_SWITCH((*sw_desc),anz_args);

	if ( part_sub_var )
	{
	  /* anz_sub_bv Variable werden vom E - Stack entfernt => */
	  /* das urspruengliche pattern wird rekonstruiert        */
          
	  for ( j = anz_sub_bv ; j > 0 ; j-- )
	  {
	    y = POPSTACK(S_e);
	    DEC_REFCNT((PTR_DESCRIPTOR)y);
	  }
	  
	  anz_pv = dim - anz_sub_bv;
	  PUSHSTACK(S_e,SET_ARITY(PM_WHEN,anz_pv + 2));
	}
	else
	  
	  PUSHSTACK(S_e,SET_ARITY(PM_WHEN,dim + 2));
      }
      else  /* also kein n-stelliges case */
      {
	PUSHSTACK(S_e,(PTR_DESCRIPTOR)code[OPAT]);
	/* Pointer auf das Originalpattern */
	
	INC_REFCNT((PTR_DESCRIPTOR)code[OPAT]);
	
	/* jetzt noch WHEN(3) auf den E-Stack : */
	
	PUSHSTACK(S_e,SET_ARITY(PM_WHEN,3));
      }
      
    } /* when-Klauseln liegen jetzt auf dem E-Stack */
    
  }/* !when */

  /* jetzt die act_nomat-Klausel auf den E-Stack legen : */



  guard_body = (PTR_HEAPELEM *)R_NOMAT((*desc),guard_body);
  /* guard_body verweist auf den modifizierten guard und den   */
  /* modifizierten body der act_nomat-Klausel.                 */
     
  switch( reason ) 
  {
  case 3 :
  case 4 :
    /* d.h.,guard und body sind modifiziert worden und  */
    /* stehen hinter guard_body[1] bzw guard_body[2]    */

#if 0
    /* mah 010394: guard has been reduced but body */
    /* expression is NOT modified                  */

    RES_HEAP;
		      
    /* Trennsymbol generieren */
		      
    ILIST_end();

    /* vorhandene Argumente in ILIST eintragen  */

    for (i = 1; i <= anz_ges - nfv; i++)
    {
      y = p_arg[i];
      if (T_POINTER(y))
	INC_REFCNT((PTR_DESCRIPTOR)y);
      
      p = (postlist*)malloc(sizeof(postlist));
      p->var = y; 
      p->link = (postlist *) ILIST;
      p->next = (postlist *) ILIST;
      ILIST = p;
    }
    
    REL_HEAP;

    p_load_expr((STACKELEM **)A_MATCH((*pf),body));
    /* body einlagern */
#else

#if 1
    {
      PTR_DESCRIPTOR sub;
      PTR_HEAPELEM *offset;

      sub = (PTR_DESCRIPTOR)R_CLOS(*(PTR_DESCRIPTOR)guard_body[2],pta)[0]; /* COMB */
      sub = (PTR_DESCRIPTOR)R_COMB(*sub,ptd);                              /* SUB  */

      if (when)
	pf = sw_desc;
      else 
	pf = (PTR_DESCRIPTOR)match_list[act_nomat];
      /* pf verweist auf den Match-Deskriptor der when-Klausel */
    
      code = (PTR_HEAPELEM *)R_MATCH((*pf),code);
      /* code entspricht dem Code-Vektor */

      offset = (PTR_HEAPELEM *)(code + (int)code[NAMELIST]);

      INC_REFCNT(sub);
      PUSHSTACK(S_i,sub);

      GET_HEAP((int)offset[0] + 2,A_FUNC(*sub,namelist));
      L_FUNC(*sub,namelist)[0] = (int)offset[0];
      L_FUNC(*sub,namelist)[1] = 0;          /* no combined subs */


      for (i=1; i<=(int)offset[0]; i++)
	L_FUNC(*sub,namelist)[i+1] = (T_HEAPELEM)offset[(int)offset[0] - i + 1];

    }
#endif
    PUSHSTACK(S_e,(PTR_DESCRIPTOR)guard_body[2]);
    INC_REFCNT((PTR_DESCRIPTOR)guard_body[2]);
    /* Pointer auf modifizierten body auf den E-Stack */

#endif /* WITHTILDE */
    
    x = (STACKELEM)guard_body[1];
    PUSHSTACK(S_e,x);
    /* reduzierten guard auf den E-Stack */
    if (T_POINTER(x))
      INC_REFCNT((PTR_DESCRIPTOR)x);
    
    /* statt des/der Originalpatterns wird ein bzw. mehrere (bei  */
    /* n-stelligen case-Funktionen bzw.pi-Abstraktionen) SKIP's   */
    /* als Pattern aufgebaut.                                     */
    
    for ( i = arity; i > 0; i-- )
      PUSHSTACK(S_e,PM_SKIP); /* statt Pattern */
    
    /* in der when - Klausel treten keine gebundenen Variablen auf */
    /* => in ILIST wird ein leerer Block eingehaengt               */

    ILIST_end(); 

    /* ILIST-> link auf ret_ILIST umsetzen, um die fuer die when - */
    /* Klauseln eingehaengten Bloecke " unsichtbar zu machen       */
    
    ILIST->link = ret_ILIST;   
    
    PUSHSTACK(S_e,SET_ARITY(PM_WHEN, arity + 2));
    break;

 case 2 :
   /* guard und body wurden nicht modifiziert,also wird die    */
   /* urspruengliche when-Klausel,auf die act_nomat verweist,  */
   /* aufgebaut :                                              */
   
   if (when)
     pf = sw_desc;
   else 
     pf = (PTR_DESCRIPTOR)match_list[act_nomat];
    /* pf verweist auf den Match-Deskriptor der when-Klausel */
    
    code = (PTR_HEAPELEM *)R_MATCH((*pf),code);
    /* code entspricht dem Code-Vektor */
    
    offset = (PTR_HEAPELEM *)(code + (int)code[NAMELIST]);
    /* offset verweist auf die Liste der Patternvariablen: */
    
    anz_bv = (int)offset[0];  /* = Anzahl der Patternvariablen */
    
    /* Trennsymbol generieren */
    
    ILIST_end();    
    
    /* Adresse merken */
    
    ret_ILIST = ILIST;
    

    /* Patternvariable in ILIST eintragen*/
    /* US */
    if ( part_sub_var )   
    {       
      /* Argumente bzw  durch sub gebundene Variable  in */
      /* ILIST eintragen                                 */

#if 0
      gen_ILIST_case((STACKELEM *) argumentlist , anz_sub_bv , TRUE , ret_ILIST);

      /* Patternvariable in ILIST eintragen */
      if ((anz_bv - anz_sub_bv ) > 0 )                                 
	gen_ILIST_case((STACKELEM *) offset , anz_bv - anz_sub_bv , FALSE, ret_ILIST );
#else
      if ((anz_bv - anz_sub_bv ) > 0 )                                 
	gen_ILIST_case((STACKELEM *) offset , anz_bv - anz_sub_bv , TRUE, ret_ILIST );
      gen_ILIST_case((STACKELEM *) argumentlist , anz_sub_bv , FALSE , ret_ILIST);
#endif
    }
    else    
      /* Patternvariable in ILIST eintragen */
      if ( anz_bv > 0 )                                  
	gen_ILIST_case((STACKELEM *) offset , anz_bv , TRUE , ret_ILIST );

    p_load_expr((STACKELEM **)A_MATCH((*pf),body));
    /* body einlagern */

    if (R_MATCH((*pf),guard) != (PTR_HEAPELEM)0)
      p_load_expr((STACKELEM **)A_MATCH((*pf),guard));
    /* guard einlagern */
    else
      PUSHSTACK(S_e,SA_TRUE);
    
    if (ncase)
    {
      y = (STACKELEM)code[OPAT];
      /* Verweis auf das 'Original'-pattern; in der Preprocessing- */  
      /* Phase wurden die verschiedenen Pattern einer when-Klausel */
      /* als Listenstruktur ausgelagert.                           */   
      
      INC_REFCNT((PTR_DESCRIPTOR)y);
      /* da in ret_list der refcount dekrementiert wird */
      
      ret_expr(y);
      if (T_LIST(READSTACK(S_e)))
	PPOPSTACK(S_e);
      /* US */
      dim = (int) R_SWITCH((*sw_desc),anz_args);
      
      if ( part_sub_var )
      {
	/* anz_sub_bv Variable werden vom E - Stack entfernt => */
	/* das urspruengliche pattern wird rekonstruiert        */
	
	for ( j = anz_sub_bv ; j > 0 ; j-- )
	{
	  y = POPSTACK(S_e);
	  DEC_REFCNT((PTR_DESCRIPTOR)y);
	}
	
	anz_pv = dim - anz_sub_bv;
	PUSHSTACK(S_e,SET_ARITY(PM_WHEN,anz_pv + 2));
      }
      else
	PUSHSTACK(S_e,SET_ARITY(PM_WHEN,dim + 2));
    }
    else  /* also kein n-stelliges case */
    {
      PUSHSTACK(S_e,code[OPAT]); /* Originalpattern */
      INC_REFCNT((PTR_DESCRIPTOR)code[OPAT]);
      
      PUSHSTACK(S_e,SET_ARITY(PM_WHEN,3));
    }

  } /* ende switch ( reason ) */ 


  if (!when)
  {
    /* Zum Schluss das Switch auf den E-Stack .                         */
    /* Dabei wird das Edit-Feld des SWITCH-Konstruktors aus dem Switch- */
    /* Descriptor 'geholt'.                                             */
    
    x = R_SWITCH((*sw_desc),casetype);
                                  
    arity = ARITY(x);
    
    if (otherwise) arity--;

    if (T_PM_REC(x))
    {
      y = (STACKELEM)match_list[anz_when + 1];
      
      /* den Funktionsnamen */    
      
      PUSHSTACK(S_e,y);
      INC_REFCNT((PTR_DESCRIPTOR)y);
      
      PUSHSTACK(S_e,x);
      
      arity -= (act_nomat - 1);
      
      WRITE_ARITY (S_e, arity );
      
    }
    else
    {
      PUSHSTACK(S_e,x);
      
      arity -= (act_nomat - 1);
      
      WRITE_ARITY (S_e, arity );
    }
  } /* !when */


  DEC_REFCNT(desc);

  if ( part_sub_var )
  {
    /* zuruecksetzen der Werte */

    part_sub_var = FALSE;
    anz_sub_bv = 0;   
    otherwise = NULL ;
  }
  
  END_MODUL("nomat_post");
  
  return;
}
