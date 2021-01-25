#include <stdio.h>
#include "server-modul.h"

Xudi_searchreplace() { }
int Xudi_getparms(char *buf) { return (0); }
void Xudi_setparms(char *buf) { }
Xudi_seteditmode() {}
Xudi_lineeditmode() {}
char *EXT_ERRORMSG;
int EXT_ERROR;

int Xudi_redcmd(char *buf,int buflen,long lastdate)
{
 printf("udi_getcmd(%s,%d,%ld)\n",buf,buflen,lastdate);
}

int main(int argc,char *argv[])
{ /* Server testprogramm *********************************************************/
 fprintf(stderr,"Server PID = %d\n",getpid());
 if (InstallServer()) {                                    /* Server installed ? */
  for (;;);
 } else {
  fprintf(stderr,"Server NOT installed !\n");
 }
} /*******************************************************************************/
