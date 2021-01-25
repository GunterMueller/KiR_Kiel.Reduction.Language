/**********************************************************************************
***********************************************************************************
**
**   File        : marco-misc.c
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 7.5.1993
**   Editornote  : Read this with vi mode set number and 92 character terminal !
**   Contents    : Implementation of functions used for standart functions
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
*/ 

/*********************************************************************************/
/********** INTERNAL PROGRAM INFORMATIONS FOR THIS MODULE ************************/
/*********************************************************************************/

/*********************************************************************************/
/********** USED INCLUDES ********************************************************/
/*********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "marco-options.h"
#include "marco-misc.h"
#include "marco-retrans.h"
#include "marco-print.h"

#include "dbug.h"

/*********************************************************************************/
/********** TYPEDEFINITIONS ******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROGRAM DEFINES ******************************************************/
/*********************************************************************************/

#define R_READ(f,s,x) if (fscanf(f,s,&(x)) != 1) {fclose(f);return(0);}

/*********************************************************************************/
/********** PROGRAM MACROS *******************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL FUNCTION PROTOTYPES ********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** LOCAL VARIABLES IN MODULE ********************************************/
/*********************************************************************************/

static STACKELEM new_stel_tab[] = {
   POINTER         /* 0 -> 0x00000000 ~ 0..0 0000 0000 */
  ,FUNC            /* 1 -> 0x00000008 ~ 0..0 0000 1000 */
  ,FUNC            /* 2 -> 0x00000008 ~ 0..0 0000 1000 */
  ,NORMAL_CONST    /* 3 -> 0x00000074 ~ 0..0 0111 0100 */
  ,LET0            /* 4 -> 0x0000000c ~ 0..0 0000 1100 */
  ,LET1            /* 5 -> 0x0000002c ~ 0..0 0010 1100 */
  ,DIG0            /* 6 -> 0x0000004c ~ 0..0 0100 1100 */
  ,DIG1            /* 7 -> 0x0000006c ~ 0..0 0110 1100 */
  ,VAR_CONSTR      /* 8 -> 0x00000002 ~ 0..0 0000 0010 */
  ,VAR_CCONSTR     /* 9 -> 0x0000000a ~ 0..0 0000 1010 */
  ,VAR_CCONSTR     /* a -> 0x0000000a ~ 0..0 0000 1010 */
  ,VAR_CCONSTR     /* b -> 0x0000000a ~ 0..0 0000 1010 */
  ,FIX_CONSTR      /* c -> 0x00000006 ~ 0..0 0000 0110 */
  ,FIX_CONSTR      /* d -> 0x00000006 ~ 0..0 0000 1110 */
  ,FIX_CCONSTR     /* e -> 0x0000000e ~ 0..0 0000 0110 */
  ,FIX_CCONSTR     /* f -> 0x0000000e ~ 0..0 0000 1110 */
};

static STACKELEM old_stel_tab[] = {
   OLD_POINTER     /* 0: 0000 0000 POINTER     */
  ,1               /* 1: 0000 0001 INT         */
  ,OLD_VAR_CONSTR  /* 2: 0000 1000 VAR_CONSTR  */
  ,3               /* 3: 0000 0011 INT         */
  ,OLD_CONSTANT    /* 4: 0000 0011 CONSTANT    */
  ,5               /* 5: 0000 0101 INT         */
  ,OLD_FIX_CONSTR  /* 6: 0000 1100 FIX_CONSTR  */
  ,7               /* 7: 0000 0111 INT         */
  ,OLD_FUNC        /* 8: 0000 0001 FUNC        */
  ,9               /* 9: 0000 1001 INT         */
  ,OLD_VAR_CCONSTR /* a: 0000 1010 VAR_CCONSTR */
  ,11              /* b: 0000 1011 INT         */
  ,OLD_STRING      /* c: 0000 0100 STRING      */
  ,13              /* d: 0000 1101 INT         */
  ,OLD_FIX_CCONSTR /* e: 0000 1110 FIX_CCONSTR */
  ,15              /* f: 0000 1111 INT         */
};

/*********************************************************************************/
/********** GLOBALE VARIABLES IN MODULE ******************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** IMPLEMENTATIONS OF LOCALE FUNCTIONS **********************************/
/*********************************************************************************/

#ifdef PRINT_PARAS
static void PrintRedParameter(FILE *fh,PTR_UEBERGABE parms)
{ /* Prints values of reduma parameter values **/
 if (parms) { /* Pointer valid ? */
  fprintf(fh,"****** REDUMA PARAMETER ********************************\n");
  fprintf(fh,"pg_size       = %d\n",parms->pg_size);
  fprintf(fh,"redcmd        = %d\n",parms->redcmd);
  fprintf(fh,"redcnt        = %d\n",parms->redcnt);
  fprintf(fh,"precision     = %d\n",parms->precision);
  fprintf(fh,"truncation    = %d\n",parms->truncation);
  fprintf(fh,"heapdes       = %d\n",parms->heapdes);
  fprintf(fh,"heapsize      = %d\n",parms->heapsize);
  fprintf(fh,"qstacksize    = %d\n",parms->qstacksize);
  fprintf(fh,"mstacksize    = %d\n",parms->mstacksize);
  fprintf(fh,"istacksize    = %d\n",parms->istacksize);
  fprintf(fh,"formated      = %d\n",parms->formated);
  fprintf(fh,"base          = %d\n",parms->base);
  fprintf(fh,"beta_count    = %d\n",parms->beta_count);
  fprintf(fh,"errflag       = %d errmes = %s\n",parms->errflag,parms->errmes);
  fprintf(fh,"preproc_time  = %3.3g\n",parms->preproc_time);
  fprintf(fh,"process_time  = %3.3g\n",parms->process_time);
  fprintf(fh,"postproc_time = %3.3g\n",parms->postproc_time);
  fprintf(fh,"total_time    = %3.3g\n",parms->total_time);
  fprintf(fh,"********************************************************\n");
 }
} /****/
#endif

/*********************************************************************************/
/********** IMPLEMENTATIONS OF EXTERNAL USED FUNCTIONS ***************************/
/*********************************************************************************/

STACKELEM misc_conv2new(STACKELEM x)
{ /* Converts old to new format **/
  STACKELEM y = x & F_TYPE;
  if ((x & ~F_EDIT) == OLD_SA_FALSE) return(SA_FALSE);
  if ((x & ~F_EDIT) == OLD_SA_TRUE)  return(SA_TRUE);
  return(new_stel_tab[y] | (x & ~F_TYPE));
} /******/
 
STACKELEM misc_conv2old(STACKELEM x)
{ /* Convert new to old format **/
  STACKELEM y = x & F_TYPE;
  if (T_MA(x)) /* string? */
    return(old_stel_tab[y] | (x & OLD_MA_MASK) |
           ((x & (F_MARK_FLAG | F_DIGLET_FLAG)) >> O_MARK_FLAG));
  if (T_CONSTANT(x)) {
    if (T_SA_FALSE(x)) return(OLD_SA_FALSE);
    if (T_SA_TRUE(x))  return(OLD_SA_TRUE);
    return(old_stel_tab[y] | ( x & ~F_CLASS));
  }
  return(old_stel_tab[y] | (x & ~F_TYPE));
} /*******/

int misc_CallReduma(void)
{ /* Calls reduma : init and reduction **/
 int counter=0; /* Reductioncounter */
 DBUG_ENTER("misc_CallReduma");

 DBUG_PRINT("Vor der REDUMA initialisierung",("counter = %d",counter));
 OPTIONS.Red_Params->redcmd=RED_INIT; /* Init red-machine */
 OPTIONS.Red_Params->errflag=0; /* No errors */
 reduce(NULL,&counter,OPTIONS.Red_Params); /* Init Reduma */
 if (OPTIONS.Red_Params->errflag) { /* REDUMA init error ? */
  fprintf(stderr,"REDUMA-ERROR : %s\n",OPTIONS.Red_Params->errmes);
  DBUG_RETURN(0);
 } /******/

 DBUG_PRINT("Nach der REDUMA initialisierung",("counter = %d",counter));
 OPTIONS.Red_Params->redcmd=RED_RD; /* Reduce this (last and only) page */
 if (OPTIONS.red_counter>=0) { /* Are user definied counter value ? */
  OPTIONS.Red_Params->redcnt=OPTIONS.red_counter; /* Set red-counter */
 }
 counter=OPTIONS.stackelems; /* Number of elements on E-Stack */
 OPTIONS.Red_Params->pg_size=OPTIONS.stacksize; /* Set page size */

 DBUG_PRINT("Vor der REDUMA Reduzierung",("counter = %d",counter));
 reduce(OPTIONS.stack,&counter,OPTIONS.Red_Params); /* Reduce expression */
 DBUG_PRINT("Nach der REDUMA Reduzierung",("counter = %d",counter));

 OPTIONS.result_stacksize=counter; /* Size of result */

 DBUG_RETURN(OPTIONS.Red_Params->errflag);
} /****/

void misc_OutputResults(void)
{ /* Generate output **/
 int i,counter,offset;
 DBUG_ENTER("misc_OutputResults");

 if (OPTIONS.time) { /* Print timing after reduction ? */
  fprintf(stderr,"PreProc Time  : %3.3g\n",OPTIONS.Red_Params->preproc_time);
  fprintf(stderr,"Process Time  : %3.3g\n",OPTIONS.Red_Params->process_time);
  fprintf(stderr,"PostProc Time : %3.3g\n",OPTIONS.Red_Params->postproc_time);
  fprintf(stderr,"Total Time    : %3.3g\n",OPTIONS.Red_Params->total_time);
 }

 offset=0;

 for (;;) { /* Loop forever ;-) */

  if (OPTIONS.Red_Params->errflag) { /* Error occurred ? */
   fprintf(stderr,"REDUCE-ERROR : %s\n",OPTIONS.Red_Params->errmes);
   DBUG_VOID_RETURN;
  } 
 
  for (i=0;i<OPTIONS.result_stacksize;i++) {
   if (OPTIONS.outputformat==OF_STACKELEMS)
    fprintf(OPTIONS.output_fh,"%08lx\n",(unsigned long)OPTIONS.stack[i]);
   else if (OPTIONS.outputformat==OF_OREL0) {
    prstel(OPTIONS.output_fh,(int)misc_conv2new(OPTIONS.stack[i]),0);
   } else if (OPTIONS.outputformat==OF_SRC) {
    OPTIONS.stack[i+offset]=misc_conv2new(OPTIONS.stack[i]);
   }
  }

  offset+=OPTIONS.result_stacksize;
  
  if (OPTIONS.Red_Params->redcmd==RED_LRP) break; /* Exit loop */
  if (OPTIONS.Red_Params->redcmd!=RED_NRP) { /* Cmd not next reduced page ? */
   fprintf(stderr,"REDUCE-ERROR : Unsupported RED-CMD <%d>\n",OPTIONS.Red_Params->redcmd);
   DBUG_VOID_RETURN;
  }

  counter=OPTIONS.result_stacksize; /* Set number of stackelems */
  reduce(OPTIONS.stack,&counter,OPTIONS.Red_Params); /* Reduce expression */
  if (OPTIONS.Red_Params->errflag) { /* REDUMA init error ? */
   fprintf(stderr,"REDUMA-ERROR : %s\n",OPTIONS.Red_Params->errmes);
   DBUG_VOID_RETURN;
  } /******/
  OPTIONS.result_stacksize=counter; /* Size of result */

 }
 
 if (OPTIONS.outputformat==OF_SRC) { /* Retransform program to source ? */
  if (udi_Orel0_2_Source()) { /* Error on ReTransformation ? */
   fprintf(stderr,"Can't retransform Orel0 to source !\n");
  }
 }

 DBUG_VOID_RETURN;
} /******/

void misc_SetDefRedParms(PTR_UEBERGABE p_Parms)
{ /* Set defaultvalues to RedParameter **/
 DBUG_ENTER("misc_SetDefRedParms");
 p_Parms->redcnt=1000000000;
 p_Parms->precision=5;
 p_Parms->prec_mult=-1;
 p_Parms->truncation=5;
 p_Parms->heapdes=200;
 p_Parms->heapsize=500000;
 p_Parms->qstacksize=250000;
 p_Parms->mstacksize=250000;
 p_Parms->istacksize=250000;
 p_Parms->formated=0;
 p_Parms->base=250000;
 p_Parms->beta_count=0;
 p_Parms->debug_info=0;
 p_Parms->add_par1=0;
 p_Parms->add_par2=0;
 DBUG_VOID_RETURN;
} /********/

int misc_InitRedParms(char *initfile,int dir,PTR_UEBERGABE p_Parms)
{ /* Read or write of reduction-parameters **/
 FILE *fp;
 DBUG_ENTER("misc_InitRedParms");

 misc_SetDefRedParms(p_Parms);

 if (initfile==NULL) { /* No initfilename ? */
  initfile = INITFILE; /* Use default filename */
 }

 if (dir==I_READ) { /* Read parameter ? */
  if ((fp = fopen(initfile,"r"))!= NULLFILE) {
   R_READ(fp,"redcount        %d\n",p_Parms->redcnt)
   R_READ(fp,"div_precision   %d\n",p_Parms->precision)
   R_READ(fp,"mult_precision  %d\n",p_Parms->prec_mult)
   R_READ(fp,"truncation      %d\n",p_Parms->truncation)
   R_READ(fp,"heapdes         %d\n",p_Parms->heapdes)
   R_READ(fp,"heapsize        %d\n",p_Parms->heapsize)
   R_READ(fp,"qstacksize      %d\n",p_Parms->qstacksize)
   R_READ(fp,"mstacksize      %d\n",p_Parms->mstacksize)
   R_READ(fp,"istacksize      %d\n",p_Parms->istacksize)
   R_READ(fp,"formated        %d\n",p_Parms->formated)
   R_READ(fp,"base            %d\n",p_Parms->base)
   R_READ(fp,"beta_count      %d\n",p_Parms->beta_count)
   R_READ(fp,"debug_info      %d\n",p_Parms->debug_info)
   R_READ(fp,"add_par1        %d\n",p_Parms->add_par1)
   R_READ(fp,"add_par2        %d\n",p_Parms->add_par2)
  } else {
   DBUG_RETURN(0);
  }
 } else if (dir==I_WRITE) {
  if ((fp = fopen(initfile,"w")) != NULLFILE) {
   fprintf(fp,"redcount        %d\n",p_Parms->redcnt);
   fprintf(fp,"div_precision   %d\n",p_Parms->precision);
   fprintf(fp,"mult_precision  %d\n",p_Parms->prec_mult);
   fprintf(fp,"truncation      %d\n",p_Parms->truncation);
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
  } else {
   fprintf(stderr,"WARNING : Can't write red initfile <%s> !\n",initfile);
   DBUG_RETURN(0);
  }
 } else {
  DBUG_RETURN(0);
 }

 fclose(fp);
 DBUG_RETURN(1);
} /*******/

int ExistsInfixPF(int pf)
{ /* Tests, if an infix operations exits for the code pf **/
 switch (pf) {
  case RED_IPF_QUOT : return (1);break;
  case RED_IPF_AND : return (1);break;
  case RED_IPF_CDIV : return (1);break;
  case RED_IPF_CMINUS : return (1);break;
  case RED_IPF_CMULT : return (1);break;
  case RED_IPF_CONS : return (1);break;
  case RED_IPF_CPLUS : return (1);break;
  case RED_IPF_DIV : return (1);break;
  case RED_IPF_EQ : return (1);break;
  case RED_IPF_FEQ : return (1);break;
  case RED_IPF_FNE : return (1);break;
  case RED_IPF_GE : return (1);break;
  case RED_IPF_GT : return (1);break;
  case RED_IPF_IP : return (1);break;
  case RED_IPF_LE : return (1);break;
  case RED_IPF_LSELECT : return (1);break;
  case RED_IPF_LT : return (1);break;
  case RED_IPF_MINUS : return (1);break;
  case RED_IPF_MOD : return (1);break;
  case RED_IPF_MULT : return (1);break;
  case RED_IPF_NE : return (1);break;
  case RED_IPF_OR : return (1);break;
  case RED_IPF_PLUS : return (1);break;
  case RED_IPF_LUNITE : return (1);break;
  case RED_IPF_VECTORDIV : return (1);break;
  case RED_IPF_VECTORMINUS : return (1);break;
  case RED_IPF_VECTORMULT : return (1);break;
  case RED_IPF_VECTORPLUS : return (1);break;
  case RED_IPF_XOR : return (1);break;
  default : return (0);
 }
} /*****/
