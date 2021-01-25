/* This file is part of the reduma package. Copyright (C)
 * 1988, 1989, 1992, 1993  University of Kiel. You may copy,
 * distribute, and modify it freely as long as you preserve this copyright
 * and permission notice.
 */
/**********************************************************************/
/*  testf.c                                                           */ 
/*                                                                    */
/*          external :  start_PRE_LIST ; extend_PRE_LIST ;            */
/*                      gen_PRE_LIST ; test_fname                     */
/*                                                                    */                 
/*   Aufgerufen durch : ear ;                                         */
/*                      ea-create ;                                   */
/*                      lrecfunc ;                                    */
/*                      casefunc ;                                    */
/*                      condfunc                                      */
/*                                                                    */
/**********************************************************************/


#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rstelem.h"
#include "rextern.h"
#include <fcntl.h>     
#include "list.h"


#define max_PRE   50  /* initiale Groesse des Listen - Descriptors */


typedef union
{
int   element;
struct

#if ODDSEX
{
  char  class;
  char  edit_info;
  short   value;
}
#else
{
  short   value;
  char  edit_info;
  char  class;
}
#endif

          feld;
}
I_STACKELEM;


/**********************************************************************/
/*    externe Variable                                                */
/**********************************************************************/

extern PTR_DESCRIPTOR    PRE_LIST;      /* Verweis auf Listen - Desc. */
extern int                anz_PRE;      /* Verweis auf freie Stelle im*/
                                        /* Listen - Descriptor        */ 



/**********************************************************************/
/* Spezielle externe Routinen:                                        */
/**********************************************************************/

extern void           ret_pexpr();                      /* rhinout.c */  
                                        
extern int           OS_receive();                      /* os-send.c */

extern T_HEAPELEM      *newbuff();                      /* rheap.c   */
extern PTR_DESCRIPTOR   newdesc();                  
extern int              newheap();
extern void            freeheap(); 


/****************************************************************************/
/* start_PRE_LIST                                                           */
/*   legt einen Listen - Descriptor an , welcher waehrend der Reduktion     */
/*   eines Ausdruckes Informationen ueber bereits eingelagerte preproc.     */
/*   Ausruecke nachhaelt ; der Descriptor wird freigegeben , wenn kein      */
/*   verpointertes Wiederaufsetzen stattfindet                              */
/*   die globale Variable PRE_LIST verweist auf diesen Descriptor           */
/****************************************************************************/

void start_PRE_LIST()
{
  PTR_DESCRIPTOR      desc;
  int                    i;
    
  if ((desc = newdesc()) == NULL)
     post_mortem("start_PRE_LIST : heap out of space ");

  LI_DESC_MASK( desc , 1 , C_LIST , TY_UNDEF );   /*      string or list ?    */
  L_LIST((*desc),dim) = max_PRE ;

  if (newheap( max_PRE ,A_LIST((*desc),ptdv)) == 0)
        post_mortem("start_PRE_LIST : heap out of space ");

  for ( i = 0 ; i < max_PRE ; i++ )
        L_LIST((*desc),ptdv)[i] = NULL;
 
  PRE_LIST = desc ; 
  anz_PRE = 0 ;
}

/****************************************************************************/
/* extend_PRE_LIST                                                          */
/*   legt einen Listen - Descriptor mit vergroesserter Dimension an ,wenn   */
/*   in " PRE_LIST" kein Platz mehr vorhanden ist ; die Eintraege aus       */
/*   "PRE_LIST" werden uebernommen                                          */
/****************************************************************************/

void extend_PRE_LIST()
{
  PTR_DESCRIPTOR       *hilf,*hilf1;
  PTR_DESCRIPTOR              ldesc;
  int                         i,dim; 

    hilf=(PTR_DESCRIPTOR *) R_LIST( (*PRE_LIST) , ptdv);         
    dim = (int) R_LIST(( *PRE_LIST ),dim);

    /* neuen Listen - Descriptor mit vergroesserter Dimension anlegen */
       if ((ldesc = newdesc()) == NULL)
           post_mortem("extend_PRE_LIST : heap out of space ");

       LI_DESC_MASK( ldesc , 1 , C_LIST , TY_UNDEF );   
       L_LIST((*ldesc),dim) = dim + max_PRE ;

       if (newheap( dim + max_PRE ,A_LIST((*ldesc),ptdv)) == 0)
                post_mortem("test_fname : heap out of space ");
       hilf1 = (PTR_DESCRIPTOR *)R_LIST((*ldesc),ptdv);

    /* Inhalt der alten Liste kopieren */
       for ( i = 0 ; i <  dim ; i ++ )
       {
         hilf1[i] = hilf[i];
         if ( hilf[i] != NULL )
               INC_REFCNT(hilf[i]);
       }

       dim = dim + max_PRE;
   
    /* restlichen Listenplatz mit NULL initialisieren */
       for ( i = dim - max_PRE ; i < dim ; i++ )
            hilf1[i] = NULL;

    /* alten Listen - Descriptor freigeben */
       DEC_REFCNT(PRE_LIST);  
       
     PRE_LIST = ldesc ; 
    
    return;
}

/****************************************************************************/
/* test_fname                                                               */
/*   testet , ob zu dem durch den FNAME - Descriptor gegebenen Namen ein    */
/*   preprocesster Ausdruck existiert ;                                     */
/*   ist dies der Fall , so wird dieser Ausdruck anstelle des FNAME -       */
/*   Descriptors in den zu reduzierenden Ausdruck eingesetzt ;              */
/****************************************************************************/

int test_fname(desc)
PTR_DESCRIPTOR desc;        /* Funktion-name Descr. */
{
   PTR_DESCRIPTOR              *hilf;
   PTR_DESCRIPTOR              edesc;
   char                   *func_name; 
   STACKELEM           *name1,*name2;
   int               n1,n2,i,fd,stop;
   int                     dim,index; 
   I_STACKELEM                   Top;

    START_MODUL("test_fname");
                         
     name1=(STACKELEM *)R_FNAME((*desc),ptn);
     n1=(int)name1[0]; 

     hilf=(PTR_DESCRIPTOR *) R_LIST( (*PRE_LIST) , ptdv);         
     dim = (int) R_LIST(( *PRE_LIST ),dim);
     index = 0 ;

     /* ueberpruefe ,ob in PRE_LIST der entsprechende Fkt - name */
     /* eingetragen ist                                       */
        while ( index < anz_PRE )
        {
          n2 = 0;

          if ( hilf[index] != desc )
          {
            /* evtl Descr. mit gleichem Namenseintrag */
               name2 = (STACKELEM *)R_FNAME(( *hilf[index]) ,ptn);
               n2 = (int)name2[0];
          
               if ( n1 == n2 )    /* gleicher Eintrag ? */
               {
                 while ( ( n2 > 0 ) &&
                         ( ( name1[n2]  & ~F_EDIT ) == ( name2[n2] & ~F_EDIT ) ) ) n2--;
               }
          }                                                                

         /* n2 = 0 => hilf[index] = desc oder hilf[index] und desc haben */
         /* gleichen Namenseintrag                                       */
            if ( n2 > 0 )
              index = index + 2 ;
            else
              break;       

        } /* Ende while */
                                                  


        if ( index == anz_PRE )
        {
          /* gesuchter Eintrag nicht gefunden => */
          /* neues Element eintragen             */
             if ( anz_PRE == dim )
             {
              /* Liste muss erweitert werden */
                 extend_PRE_LIST();
                 hilf=(PTR_DESCRIPTOR *) R_LIST( (*PRE_LIST) , ptdv);         
             }
         
             hilf[index] = desc;
             INC_REFCNT(desc);
             anz_PRE = anz_PRE + 2 ;
        }

        /* ist der zugehoehrige Ausdruck bereits im heap vorhanden , so  */
        /* wird er auf den E - Stack traversiert                         */
           if ( hilf[index + 1] != NULL )
           {
              ret_pexpr( hilf[ index + 1] );
              END_MODUL("test_fname");
              return(0);  
           } 

        /* ansonsten wird getestet , ob eine Datei mit entsprechendem    */
        /* Namen existiert                                               */
        /*   hilf[index + 1] verweist auf das Listenelement , welches    */
        /*   aufgefuellt werden muss                                     */   
           if ( ( func_name=(char *)newbuff(n1-1) ) == (char *) 0 ) 
              post_mortem("test_fname : heap out of space ");      

           for ( i=0 ; i < ( n1 - 1 ) ; i++ )
               func_name[i]=' ' ;                                     

        /* name1[1]=@ => i startet bei 2 */
           for ( i=2 ; i <= n1 ; i++ )
           {
             Top.element=(STACKELEM)name1[i];
             func_name[i-2]=(char)Top.feld.value;
           }        
                                
           func_name[n1-1]='\0' ;

           fd=open( func_name ,O_RDONLY ); 
                                            
        /* Speicherplatz wieder freigeben */
           freeheap((PTR_HEAPELEM)func_name);   
                                  
           if ( fd >= 0 )    
           {
            /* ex eine Datei mit entsprechendem Namen => */
            /* Ausdruck in den Heap einlagern            */
              if ( OS_receive(fd,&edesc) > 0 ) 
              {
                /* Ausdruck korrekt gelesen */ 
                   close(fd);                   
         
                /* Ausdruck in PRE_LIST eintragen */
                   hilf[index  + 1 ] = edesc;

                /* Ausdruck einlagern                         */
                   ret_pexpr(edesc);
                   END_MODUL("test_fname");
                   return(0);                  
              }
              else
              {
                /* Fehler beim Lesen */      
                   close(fd);
                   post_mortem(" testf : unable to read preproc. expression from file ");
              }
           }
           else
           {
             /* ex kein zugehoeriger preproc. Ausdruck => */
             /* Funktion-name - Descriptor wird traversiert */
                PUSHSTACK(S_a,(STACKELEM)desc) ;
                END_MODUL("test_fname");
                return(1);
           }
}
           
        

/**********************************************************************/
/* gen_PRE_LIST                                                       */
/*   traegt einen Fname - Descriptor in " PRE_LIST ein                */
/*   ist in PRE_LIST nicht genuegend Platz vorhanden , so wird ein    */
/*   neuer Listen - Descriptor mit groesserer Dimension angelegt      */
/**********************************************************************/
 
void gen_PRE_LIST(fdesc)  
PTR_DESCRIPTOR fdesc;                   /* Funktionname - Descriptor */
{
  PTR_DESCRIPTOR   *hilf;
  int                dim;

  dim = R_LIST((*PRE_LIST),dim);

  if ( anz_PRE >= dim )
      extend_PRE_LIST();        /* neuen Listen - Descriptor anlegen */

       
  hilf = (PTR_DESCRIPTOR *)R_LIST((*PRE_LIST),ptdv);

  hilf[ anz_PRE] = fdesc; 

  INC_REFCNT(fdesc);             /* FNAME - Descriptor wird beim     */
                              /* freigeben der Listenstuktur decrem. */  
  anz_PRE = anz_PRE + 2 ;

  return;
}



