/*                                        cglobals.h */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

/* fuer "meinen" editor	*/
struct Fedstate {
  STACK E_,A_,M_,M1_,M2_,B_,U_,U2_,V_,S_,S1_,S2_,R_,R1_,R2_,EADR_,AADR_;
  int dmode_,rc_,rca_,news_;
  int status_;
  char editfil_[FN_ANZ][LENLIB],
       editlib_[FN_ANZ][LENFIL],
       editext_[FN_ANZ][LENEXT];
};

typedef struct Fedstate * P_FedState;

#ifdef EDITSTATE                    /* vorsicht !     */
P_FedState p_fedstate;
P_EditState p_editstate;
#else
extern P_FedState p_fedstate;
extern P_EditState p_editstate;
#endif

#define ST_RAW	0x01	/* im raw-mode		*/
#define ST_TERM	0x02	/* termcaps gelesen	*/
#define ST_EDP	0x04	/* editparms gelesen	*/
#define ST_REDP	0x08	/* redparms gelesen	*/
#define ST_EDIT	0x10	/* environment-variablen gelesen	*/

#define EDSTATUS(status,typ)	((((P_FedState)((status)->edit_))->status_) & (typ))
#define SETSTATUS(status,typ)	((P_FedState)((status)->edit_))->status_ |= (typ)

/* end of       cglobals.h */

