#include "nc_nn_sched.h"
#include "rncmessage.h"
#include "dbug.h"
#include <stdio.h>
extern float *Net_Input;
extern int proc_id;                  /* rncmain.c */
typedef int (* FCTPTR) ();
extern FCTPTR dealloc_tab[];
extern void dealloc_li();

static char DBUG_HELP_STR[256];
int proc_org;
  
/* --- MT_STAT_REQ - Message ------------------------------------------ */
void nn_msg_send(int dest, MESSAGE_TYPE type)
{
 DBUG_ENTER("nn_msg_send");
 if (type == MT_STAT_REQ) {
     DBUG_PRINT("NN",("MT_STAT_REQ to Node %d", dest));
     nwrite(&proc_id, sizeof(int), dest, type, NULL);
 } else if (type == MT_STAT_SEND) {
     DBUG_PRINT("NN",("MT_STAT_SEND to dest %d from %d ", dest, proc_id));
     nwrite(&proc_id, sizeof(int), dest, type, NULL);
     nwrite(&(proc_stat[P_RUN]), sizeof(int), dest, type, NULL);
     nwrite(&(proc_stat[P_READY]), sizeof(int), dest, type, NULL);
     nwrite(&(proc_stat[P_WAIT]), sizeof(int), dest, type, NULL);
 }
 DBUG_VOID_RETURN;
}

void nn_msg_receive(arg_type)
int arg_type;
{
  int receive=0;
  int p_org=0; 
  int Neighb_cnt;
  int src=-1;
  int  type;

  DBUG_ENTER("nn_msg_receive");
  type=arg_type;
  if (type == MT_STAT_REQ) {
     DBUG_PRINT("NN",("MT_STAT_REQ "));
     nread (&proc_org, sizeof(int), &src, &type, NULL);
     DBUG_PRINT("NN",("from Proc %d", proc_org));
  }

  if (type == MT_STAT_SEND) {
     DBUG_PRINT("NN",("MT_STAT_SEND "));
     nread (&p_org, sizeof(int), &src, &type, NULL);
     DBUG_PRINT("NN",("from Proc %d",p_org));
     if ( (Neighb_cnt=Neighbours[p_org]) == NO_NEIGHB ) {
        sprintf(DBUG_HELP_STR,"Illegal Neighbour (%d) to Node %d", p_org, proc_id);
        post_mortem(DBUG_HELP_STR);
     }
     nread (&receive, sizeof(int), &src, &type, NULL);
     Net_Input[Neighb_cnt+Run_Offset] = ((float)receive / NN_SCALE);
     nread (&receive, sizeof(int), &src, &type, NULL);
     Net_Input[Neighb_cnt+Ready_Offset] = ((float )receive/NN_SCALE);
     nread (&receive, sizeof(int), &src, &type, NULL);
     Net_Input[Neighb_cnt+Wait_Offset] = ((float )receive/NN_SCALE);
     Message_SENT--;
     sprintf(DBUG_HELP_STR,"Running  %f \t Ready %f \t Waiting %f\n",
                      Net_Input[Neighb_cnt+Run_Offset],
                      Net_Input[Neighb_cnt+Ready_Offset],
                      Net_Input[Neighb_cnt+Wait_Offset]);
     DBUG_PRINT("NN",("%s",DBUG_HELP_STR));
  }
  DBUG_VOID_RETURN;
}
