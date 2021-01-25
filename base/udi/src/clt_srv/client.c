#include <stdio.h>
#include "client-modul.h"

char BUF[256];
int PID;

XR_MessageDisplay(){}

main(int argc,char *argv[])
{
 int i=2;
 PID=atoi(argv[1]);
 printf("<%s>=%d\n",argv[1],PID);
 if (GetAndConnectServer(PID)) { /* Server connection success ? */
  while (i<argc) {
   sprintf(BUF,"%s",argv[i]);
   printf("Send <%s>\n",BUF);
   ExecuteRedCommand(BUF);
   printf("DONE <%d>\n",i);
   i++;
  }
 } else {
  fprintf(stderr,"Is the server running ?\n");
 }
}
