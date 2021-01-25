/* hauptprogramm des reduktions-editors        cmain.c  */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    29.08.88                        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include "cedit.h"
#include "cscred.h"
/* #include "ctermio.h"   */
#include "cestack.h"
#include "cparm.h"
#include "cedstate.h"
#include "cglobals.h"
#include "ccommand.h"

/* externe routinen       */
extern void error();

/* --------- main -------------------------------------------------- */
main(argc,argv)
 int argc;
 char *argv[];
 {
  extern int Counter;
  int erg;



  if ((p_editstate = createdst()) != (P_EditState)0) { /* erzeuge zustand */
    p_editstate->neu_  = ED_NEU;
    p_editstate->argc_ = argc;
    p_editstate->argv_ = argv;
    STARTREDUMA();
    do {
      if ((erg = editor(p_editstate)) <= FAILURE) {  /* editor-aufruf  */
        fprintf(stdout,"Can't call editor\n");
       }         else
      if (p_editstate->result_ == CALL_RED) {        /* reduma aufrufen ?  */
/*        p_editstate = storeedst(FALSE,p_editstate); /* editor-state retten */
        /* call reduma      */
        Parms.redcmd = RED_RD;
        CALLREDUCE(Bottom,&Counter,&Parms);
        /* was ist mit ueberlauf der seite ?         */
        p_editstate->neu_ = ED_RED_FROM;
        p_editstate->Counter_ = Counter;    /* Counter von reduce ?    */
        p_editstate->Parms_.redcnt = Parms.redcnt;
      }
    } while (erg >= SUCCESS  &&
             p_editstate->result_ != ED_EXIT   &&
             p_editstate->result_ != SAVE_GRAPH  &&
             p_editstate->result_ != EXIT_GRAPH);
    STOPREDUMA();
   }
  else {
    error("Can't create editor-state !\n");
  }
  if (erg == ED_EXIT) {
/*  CLEARSCREEN();        */
/*    switchterm(LINEMODE);	*/
  }
/*fprintf(stdout,"Good bye\n");    */
  if (erg <= FAILURE) exit(1);                  /* !! vorsicht */
  exit(0);
 }
/* end of       cmain.c  */

