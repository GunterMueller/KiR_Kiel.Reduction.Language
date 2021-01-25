/* help-funktion, cat, initedit                 chelp.c */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    10.06.88                        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include "cedit.h"
#include "cfiles.h"
#include "cscred.h"
#include "ctermio.h"

extern char *strcpy();
static char helpfile[LENFN];

/* --------------- cat -------------------------------------- */
/* ausgabe der datei mit dem namen file auf stdout            */
int cat(file)
char *file;
{
  FILE *fp,*fopen();
  int c;

  if ((fp = fopen(file,"r")) != NULLFILE) {
    while ((c = getc(fp)) != EOF) {
      if (c == '\n') fputc('\r',stdout);
      fputc(c,stdout);
    }
    fclose(fp);
    return(TRUE);
   }
  else {
    fprintf(stdout,"cat: Can't open  %s\r\n",file);
    return(FALSE);
  }
}
#ifdef UnBenutzt
/* ---------- filelength ------------------------------------ */
/* laenge der datei fp                                        */
int filelength(fp)
FILE *fp;
{
 register int c;

 c = 0;
 while (getc(fp) != EOF) {
   c++;
 }
 rewind(fp);
 return(c);
}
#endif
/* -------------- help ----------------------------------------------- */
void help(str,dsp)
char *str;                /* help-anforderung                          */
int dsp;                  /* danach display ?                          */
{
  FILE *fp,*fopen();
  static char buf[80],hlp[80];
  char *skipblanks();
  int c,lc,lines,notfound;

  if ((fp = fopen(helpfile,"r")) != NULLFILE) {
   if (str != NULLPTR) {
     if (*str == '\0') {
       sprintf(hlp,"help");
       str = hlp;
     }
     do {
       notfound = TRUE;
       CLEARSCREEN();
       fprintf(stdout,"Searching for \"%s\"\r\n",str);
       lines = li - 3;
       while ((c = getc(fp)) != EOF) {
         if (c == '^') {
           fscanf(fp,"%s",buf);
           if (strcmp(str,buf) == SUCCESS) {
             notfound = FALSE;
             while ((c = getc(fp)) != '\n')   /* skip line */
               ;
             lc = c;
             while ((c = getc(fp)) != EOF) {
               if (c != '^' || lc == '\\') {
                 if (c == '\n') {
                   fputc('\r',stdout); fputc('\n',stdout);
                   if (--lines == 0) {
                     fprintf(stdout,"\r\nHit <SPACE> to continue or <CR> to break ");
                     fflush(stdout);
                     if (getkey() == CR) break;
                     fputc('\r',stdout);
                     cleartoeol();
                     lines = li - 3;
                   }
                  }
                 else {
                   if (c != '\\' || lc == '\\') fputc(c,stdout);
                 }
                }
               else break;
               lc = c;
              }
              fputc('\r',stdout); fputc('\n',stdout);
              break;
           }
         }
       }    /* end while */
       if (notfound)
         fprintf(stdout,"\r\n\nThere's no help available for \"%s\"\r\n\n",str);
       CURSORXY(li-1,0);
       fprintf(stdout,"Enter a new Help Keyword or hit <CR> to break: ");
       fflush(stdout);
       if (getline(hlp,60,0,0,li-1,47,32) > 0) {
         str = skipblanks(hlp);
         if (*str != '\0') {
           fseek(fp,0L,0);
           notfound = TRUE;
           fputc('\n',stdout);
           continue;
          }
         else break;
        }
       else break;
     } while (notfound);
   }
   fclose(fp);
   if (dsp) {
     initscreen();
     dispfa();
   }
  }
 else {
   sprintf(buf,"Can't find the Helpfile \"%s\"\r\n",helpfile);
   message(MROW,MCOL,buf);
   getunget();
   clrmsg(MROW,MCOL);
 }
}
/* ------------- initedit -------------------------------------------- */
/* initialisiere editor                                                */
/* return: letztes aenderungsdatum des helpfiles (yymmdd)              */
long int initedit(all)
 int all;
 {
  FILE *fp,*fopen();
  long int newdate;
  char *ptr,*getenv();

#if SINTRAN3
  sprintf(helpfile,"%s",HELPFILE);
#else
  if ((ptr = getenv("FHELPFILE")) != NULLPTR) strcpy(helpfile,ptr);
  else strcpy(helpfile,HELPFILE);
  if (all) {
    if ((ptr = getenv("FEDITLIB")) != NULLPTR) strcpy(editlib[FN_ED],ptr);
  }
#endif
  if ((fp = fopen(helpfile,"r")) != (FILE *)0) {
    if (fscanf(fp,"# %ld",&newdate) != 1)   /* letztes update-datum    */
      newdate = 0l;                         /* helpfiles               */
    fclose(fp);
   }
  else {
    newdate = 0l;
  }
  return(newdate);
}
/* end of        chelp.c */

