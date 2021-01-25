#ifndef lint
static char *rcsid = "$Header: /home/omega/rcs-for-workgroups/bin/RCS/su-rcs.c,v 1.8 1995/11/20 15:49:01 omega Exp $";
#endif


/*
 * $Log: su-rcs.c,v $
 * Revision 1.8  1995/11/20  15:49:01  omega
 * Changed Path to rcs to /opt/gnu/bin
 *
 * Revision 1.7  1993/05/04  14:44:46  apply
 * Nur das 1. Zeichen einer Option pruefen.
 *
 * Revision 1.6  1993/04/23  08:25:07  apply
 * Neue Optionen c, m, n, q ,I, t. Kein Abbruch, wenn <= 2 Arg.
 *
 * Revision 1.5  1992/10/12  10:36:14  apply
 * Fehlermeldung bei illegaler Option veraendert, um klarzumachen,
 * dass su-rcs die Fehlermeldung erzeugt hat.
 *
 * Revision 1.4  1992/06/23  14:05:11  uho
 * Relaxed filename restrictions
 *
 * Revision 1.3  1992/06/12  12:01:33  uho
 * array initialisierung auf top level
 *
 * Revision 1.2  1992/06/12  11:59:56  uho
 * Nun mit execve
 *
 * Revision 1.1  1992/06/12  11:18:42  uho
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define RCS "/opt/gnu/bin/rcs" 

#define ILLEGAL_OPTION(i) \
            { \
                 fprintf(stderr,"%s: illegal suid option: %s\nAccess denied!\n",\
                         argv[0],argv[i]);\
                 exit(3);\
            }        

char *envp[] = { NULL }; /* empty environment */
 
main(argc,argv)
int argc;
char *argv[];
{
   char *str, ch;
   int i;
   
   /* check for valid options */
   for (i=1; i<argc; i++) 
   {
      str=argv[i];
   
      /* an option? */
      if (str[0] == '-') 
      {
         switch (str[1]) 
         {
         case 'i':              /* create + initialize */
         case 'c':              /* set comment leader */
         case 'l':              /* lock a revision */
         case 'u':              /* unlock a revision */
         case 'm':              /* replace log message */
         case 'n':              /* associate or delete symbolic name */
         case 'q':              /* run quietly */
         case 'I':              /* run interactively */
         case 't':              /* replace descriptive text */
            break;
         default: ILLEGAL_OPTION(i);
         }
      }
   }
   
   exit(execve(RCS,argv,envp));  
}

 
