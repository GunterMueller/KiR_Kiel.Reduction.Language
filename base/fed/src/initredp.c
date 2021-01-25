/*							initredp.c	*/
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* lesen oder schreiben der reduktionsmaschinen-parameter		*/
/* letzte aenderung:	03.03.89					*/

#include <stdio.h>
#include "cparm.h"
#include "cedstate.h"

#define I_READ	0
#define I_WRITE	1
#define I_FAIL	(-1)
#define NULLFILE (FILE *)0

#define INITFILE	"red.init"

#define R_READ(f,s,x) if (fscanf(f,s,&(x)) != 1) { fclose(f); return(I_FAIL); } else

/* ------ initredp ----------------------------------------------------	*/
/* lesen oder schreiben der reduktionsmaschinen-parameter		*/
int initredp(initfile,dir,p_Parms)
 char	* initfile;		/* name der init-datei		*/
 int	dir;			/* lesen oder schreiben		*/
 PTR_UEBERGABE	p_Parms;	/* zeiger auf parameter-paket	*/
 {
  FILE *fp,*fopen();

  if (initfile == (char *)0) {
    initfile = INITFILE;
  }
  if (dir == I_READ) {
    if ((fp = fopen(initfile,"r")) != NULLFILE) {
      R_READ(fp,"redcount        %d\n",p_Parms->redcnt);
      R_READ(fp,"div_precision   %d\n",p_Parms->precision);
      R_READ(fp,"mult_precision  %d\n",p_Parms->prec_mult);
      R_READ(fp,"truncation      %d\n",p_Parms->truncation);
/*
      R_READ(fp,"indentation     %d\n",p_Parms->indentation);
*/
      R_READ(fp,"heapdes         %d\n",p_Parms->heapdes);
      R_READ(fp,"heapsize        %d\n",p_Parms->heapsize);
      R_READ(fp,"qstacksize      %d\n",p_Parms->qstacksize);
      R_READ(fp,"mstacksize      %d\n",p_Parms->mstacksize);
      R_READ(fp,"istacksize      %d\n",p_Parms->istacksize);
      R_READ(fp,"formated        %d\n",p_Parms->formated);
      R_READ(fp,"base            %d\n",p_Parms->base);
      R_READ(fp,"beta_count      %d\n",p_Parms->beta_count);
      R_READ(fp,"debug_info      %d\n",p_Parms->debug_info);
      R_READ(fp,"add_par1        %d\n",p_Parms->add_par1);
      R_READ(fp,"add_par2        %d\n",p_Parms->add_par2);
     }
    else {
      return(I_FAIL);
    }
   } else 
  if (dir == I_WRITE) {
    if ((fp = fopen(initfile,"w")) != NULLFILE) {
      fprintf(fp,"redcount        %d\n",p_Parms->redcnt);
      fprintf(fp,"div_precision   %d\n",p_Parms->precision);
      fprintf(fp,"mult_precision  %d\n",p_Parms->prec_mult);
      fprintf(fp,"truncation      %d\n",p_Parms->truncation);
/*
      fprintf(fp,"indentation     %d\n",p_Parms->indentation);
*/
      fprintf(fp,"heapdes         %d\n",p_Parms->heapdes);
      fprintf(fp,"heapsize        %d\n",p_Parms->heapsize);
      fprintf(fp,"qstacksize      %d\n",p_Parms->qstacksize);
      fprintf(fp,"mstacksize      %d\n",p_Parms->mstacksize);
      fprintf(fp,"istacksize      %d\n",p_Parms->istacksize);
      fprintf(fp,"formated        %d\n",p_Parms->formated);
      fprintf(fp,"base            %d\n",p_Parms->base);
      fprintf(fp,"beta_count      %d\n",p_Parms->beta_count);
      fprintf(fp,"debug_info      %d\n",p_Parms->debug_info);
      fprintf(fp,"add_par1        %d\n",p_Parms->add_par1);
      fprintf(fp,"add_par2        %d\n",p_Parms->add_par2);
     }
    else {
      return(I_FAIL);
    }
   }
  else {
   return(I_FAIL);
  }
  fclose(fp);
  return(dir);
}
/* end of      initredp.c */
