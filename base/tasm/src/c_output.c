#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dbug.h"
#include "mytypes.h"
#include "buildtree.h"
#include "comp_support.h"
#include "error.h"
/*************************************/
/* integers, tag: 1                  */
/*************************************/

#define INT                0x00000001
#define P_INT              0x00000001
#define T_INT(x)           (((x) & P_INT) == INT)
#define T_2INT(x,y)        (((x) & (y) & P_INT) == INT)

#define O_INT              1
#define FAC_INT            2

#define TAG_INT(x)         (((x) * FAC_INT) + INT)
/*************************************/
/* constants, tag: 0100              */
/*************************************/

#define CONSTANT          0x00000004
#define P_CONSTANT        0x0000000f
#define T_CONSTANT(x)     (((x) & P_CONSTANT) == CONSTANT)

#define F_CNST_GROUP      0xff000000
#define O_CNST_GROUP      24

#define F_CNST_NAME       0x00ff0000
#define O_CNST_NAME       16

/* special constants */

#define F_BOOL_FLAG        0x00000010

#define SA_FALSE           CONSTANT
#define SA_TRUE            (SA_FALSE | F_BOOL_FLAG)

/*************************************************************************/

extern char *modulfile;
extern CODE *functable;
extern char *outfile;
extern char *outfiletmp;
extern char *execfile;
extern char *execfilepath;
extern char *tasmlib;
extern char *redumalib;
extern char *ncubelib;
extern char *primf_tab[];
extern char *intact_tab[];
extern char *funcprot;
extern int FRED_FISH, NOSTACK2REG;
extern int TEST_STACK;
extern int ODDSEX;
extern int DIST;
extern int WATCH;
extern int MODUL;
extern PROGRAM *program_start;
extern FILE *yyout;
extern int ref_inferred;
extern int optimize;
extern int mess_enabled;
extern int find_substr(char *, char *);
extern char *strrepl(char *, char *, int);
extern int search_funcname(char *);
extern ORDER *search_label(char *s, ORDER *);

void c_out_desc_list(FUNDESC *);
void c_out_order_list(FUNCTION *);
int searchdata(int);
#define MAX_LINES     300  /* max. number of lines in a function */
int lines;                   /* line counter */
char makefilename[L_tmpnam];         /* temp name for the makefile */
typedef struct tagoutname {
                            char *name;
                            struct tagoutname *next;
                          } OUTNAME;

OUTNAME *first_file, *act_file, *last_file;

/* cr 22/03/95, kir(ff), START */
/* returns the ascii form of interaction types
 */
static char * c_out_intertype(INTERT n)
  {
  DBUG_ENTER("out_intertype");
  switch(n) {
    case INdone: DBUG_RETURN("TY_DONE");
    case INget:  DBUG_RETURN("TY_GET");
    case INput:  DBUG_RETURN("TY_PUT");
    default: yyfail("illegal interaction type received");
    }
  DBUG_RETURN((char *)0);
  }
/* cr 22/03/95, kir(ff), END */

/*************************************************************************/
/* Prints out the prototypes of the generated functions to include       */
/* them into the generated C-file.                                       */
/*************************************************************************/

void c_build_func_prot(PROGRAM *p, char *modulfile)
{
  FUNCTION *f;
  FILE *file_to_open;
  char *name_to_open, *targetdir;

  DBUG_ENTER("c_build_func_prot");
  if (NULL == (name_to_open = (char *)malloc(256)))
    yyfail("unable to allocate memory");
  if (NULL == (targetdir = (char *)malloc(256)))
    yyfail("unable to allocate memory");
  if (dogcc)
    strcpy(targetdir, P_tmpdir);
  else
    strcpy(targetdir, execfilepath);
  sprintf(name_to_open, "%s%s%s", targetdir, modulfile, funcprot);
  file_to_open=fopen(name_to_open,"w");
  if (NULL==file_to_open)
    yyfail("%s", "unable to open prototype file");
  fprintf(file_to_open,"extern int f_apply();\n");
  f=GETFIRSTFUNCTION(p);
  while (NULL!=f)
  {
    if (strcmp(GETNAME(f),"apply"))
      fprintf(file_to_open,"int %s();\n",GETNAME(f));
    else
      fprintf(file_to_open,"int apply();\n");
    
    f=GETNEXTFUNCTION(f);
  }
  fflush(file_to_open);
  
  if(EOF==fclose(file_to_open)) 
    yyfail("%s","unable to close prototype file");
  
  DBUG_VOID_RETURN;
}


/***********************************************************************/
/* Prints out data building function calls.                            */
/***********************************************************************/

void c_out_data(void)
{
  int i = 0, size, j, help, mvt, cnt;
  DATANODE *data = GETFIRSTDATA(program_start);
  char s[256];
  char class[20], type[20];
  DBUG_ENTER("c_out_data");
  lines = 0;
  if (outfile)
  {
    if (NULL == (first_file = (OUTNAME *)malloc(sizeof(OUTNAME))))
      yyfail("unable to allocate memory");
    first_file->next = NULL;
    if (NULL == (first_file->name = 
                            (char *)malloc(strlen(outfiletmp) + strlen(outfile) + 3)))
      yyfail("unable to allocate memory");
    sprintf(s, "%s", outfiletmp);  
    strcpy(first_file->name, s);
    last_file = first_file;
  }
  fprintf(yyout, "void initdata(void)\n{\n");
  mvt = 0;
  while (NULL != data)
  {
    switch(GETDATAENTRY(data, tag))
    {
      case d_mat:
      case d_vect:
      case d_tvect:
        switch (GETDATAENTRY(data, u.w.tag))
        {
          case d_undef:
            fprintf(yyout, "int mvt%d[] = {};\n", mvt++);
            break;
          case d_int:
          case d_bool:
            fprintf(yyout, "int mvt%d[] = { %d", mvt++, 
                                GETDATAENTRY(data, u.w.m.idata)[0]);
            for(j=1; j<GETDATAENTRY(data, u.w.rows) * GETDATAENTRY(data, u.w.cols); j++)
            {
              fprintf(yyout, ", %d", GETDATAENTRY(data, u.w.m.idata)[j]);
              cnt++;
              if (cnt > 40)
              {
                cnt = 0;
                fprintf(yyout, "\n");
              }
            }
            fprintf(yyout, "};\n");
            break;
          case d_real:
            fprintf(yyout, "double mvt%d[] = { %.16e", mvt++, 
                                GETDATAENTRY(data, u.w.m.rdata)[0]);
            for(j=1; j < GETDATAENTRY(data, u.w.rows) * GETDATAENTRY(data, u.w.cols); j++)
            {
              fprintf(yyout, ", %.16e", GETDATAENTRY(data, u.w.m.rdata)[j]);
              cnt++;
              if (cnt > 15)
              {
                cnt = 0;
                fprintf(yyout, "\n");
              }
            }
            fprintf(yyout, "};\n");
            break;
          default:;
        }
        break;
      default:;
    }
    data = GETNEXTDATA(data);
  }
  data = GETFIRSTDATA(program_start);
  fprintf(yyout, "int *l;\n");
  if (FRED_FISH)
    fprintf(yyout," DBUG_ENTER(\"initdata\");\n");
  mvt = 0;
  while (NULL!=data)
  {
    lines++;
    switch(GETDATAENTRY(data, tag))
    {
    case d_mat:
    case d_vect:
    case d_tvect:
      if (d_mat == GETDATAENTRY(data, tag))
        strcpy(class, "C_MATRIX");
      else if (d_vect == GETDATAENTRY(data, tag))
        strcpy(class, "C_VECTOR");
      else 
        strcpy(class, "C_TVECTOR");

      if (d_int == GETDATAENTRY(data, u.w.tag))
        strcpy(type, "TY_INTEGER");
      else if (d_bool == GETDATAENTRY(data, u.w.tag))
        strcpy(type, "TY_BOOL");
      else if (d_real == GETDATAENTRY(data, u.w.tag))
        strcpy(type, "TY_REAL");
      else if (d_string == GETDATAENTRY(data, u.w.tag))
        strcpy(type, "TY_STRING");
      else
        strcpy(type, "TY_UNDEF");
      fprintf(yyout, "%sdata_table[%d] = initdata_matrix(%s, %s, %d, %d, "
                     "(int *)mvt%d);\n",
                      modulfile, i, class, type,
                      GETDATAENTRY(data, u.w.rows), 
                      GETDATAENTRY(data, u.w.cols), mvt++);
      break;
      break;
    case d_real:
    {
      fprintf(yyout, "%sdata_table[%d] = initdata_scalar(%.16e);\n", modulfile, i,
              GETDATAENTRY(data, u.x));
      break;
    }
    case d_string:
    {
      size = GETDATAENTRY(data, u.v.size);
      if (0 != size)
      {
        fprintf(yyout, "if (NULL == (l = (int *)malloc(%d*sizeof(int))))",
                size);
        fprintf(yyout, "{ fprintf(stderr, \"FATAL ERROR: couldn't allocate memory.\\n\");\nexit(1);\n}\n");
      }
      else  fprintf(yyout, "l = NULL;\n");
      for (j = 0; j < size; j++)
      {
        help =  GETDATAENTRY(data, u.v.data[j]);
        
        if (ISCHAR(help))
          fprintf(yyout, "l[%d] = MAKE_CHAR('\\%o');\n", j, GETCHAR(help));
        else if(ISPTR(help))
          fprintf(yyout, "l[%d] = %sdata_table[%d];\n", j, modulfile, searchdata(help));
      }
      fprintf(yyout, "%sdata_table[%d] = initdata_string(%d, l);\n", modulfile, i, size);
      
      if (0 != size)
        fprintf(yyout, "free(l);\n");
      break;
    }
    case d_name:
    {
      size = GETDATAENTRY(data, u.v.size);
      fprintf(yyout, "if (NULL == (l = malloc(%d)))", size + 1);
      fprintf(yyout, "{ fprintf(stderr, \"FATAL ERROR: couldn't allocate memory.\\n\");\n
exit(1);\n}\n");
      fprintf(yyout, "strcpy((char *)l, \"%s\");\n", (char *)GETDATAENTRY(data, u.v.data));
      fprintf(yyout, "%sdata_table[%d] = initdata_free_var(%d, l);\n", modulfile, i, size);
      fprintf(yyout, "free(l);\n");
      break;
    }
    case d_list:
    {
      size = GETDATAENTRY(data, u.v.size);
      if (0 != size)
      {
        fprintf(yyout, "if (NULL == (l = (int *)malloc(%d*sizeof(int))))",
                size);
        fprintf(yyout, "{ fprintf(stderr, \"FATAL ERROR: couldn't allocate memory.\\n\");\nexit(1);\n}\n");
      }
      else  fprintf(yyout, "l = NULL;\n");
      for (j = 0; j < size; j++)
      {
        help =  GETDATAENTRY(data, u.v.data[j]);
        
        if (ISINT(help))
          fprintf(yyout, "l[%d] = %d;\n", j, help);
        else if (ISBOOL(help))
          fprintf(yyout, "l[%d] = %d;\n", j, help);
        else if (ISPRF(help))
          fprintf(yyout, "l[%d] = %s;\n", j, primf_tab[GETPRF(help)]);
        else if (ISIA(help))
          fprintf(yyout, "l[%d] = %s;\n", j, intact_tab[GETIA(help)]);
        else
          fprintf(yyout, "l[%d] = %sdata_table[%d];\n", j, modulfile, searchdata(help));
      }
      fprintf(yyout, "%sdata_table[%d] = initdata_list_initial(%d, l);\n", modulfile, i, size);
      
      if (0 != size)
        fprintf(yyout, "free(l);\n");
      break;
    }
    default:
      ;
    } /* END switch(GETDATAENTRY(data, tag)) */
    i++;
    data=GETNEXTDATA(data);
  } /* END while (NULL!=data) */
  if (DIST)
    fprintf(yyout, "SET_STATIC_BORDER(highdesc);\n");
  if (FRED_FISH)
    fprintf(yyout," DBUG_VOID_RETURN;\n");
  fprintf(yyout, "}\n");
  DBUG_VOID_RETURN;
}


/***********************************************************************/
/* Prints out ready-made functionsdescriptors.                         */
/***********************************************************************/

void c_out_functable(CODE *f, int ShallICount)
{
  FUNDESC *d = GETFIRSTDESC(program_start);
  
  int i = 0;
  int j;
  
  DBUG_ENTER("out_functable");

  while (NULL!=f)
  {
    fprintf(yyout,"%s\n",GETORDERENTRY(f, code));
    f=GETNEXTFUNCTION(f);
  }
  while (d != NULL)
  {
    d=GETNEXTDESC(d);
    i++;
  }
  
  if (ShallICount) 
  {
    fprintf(yyout,"int %snumber_of_descs = %d;\n",modulfile, i);
    fprintf(yyout,"PTR_DESCRIPTOR %sfuncdesctable[%d];\n", modulfile, i);
    fprintf(yyout,"int %sfuncclostable[%d];\n", modulfile, i);
    fprintf(yyout,"int %sptc_table[] = {\n", modulfile);
    d = GETFIRSTDESC(program_start);
    for (j = 0; j < 2 * i - 2; j += 2)
    {
      fprintf(yyout, "0x%08x,\n", GETDESCENTRY(d, graph));
      if (GETDESCENTRY(d, address) & 1)
        fprintf(yyout, "0x%08x,\n", GETDESCENTRY(d, address) - 1);
      else
        fprintf(yyout, "0x%08x,\n", GETDESCENTRY(d, address));
      d=GETNEXTDESC(d);
    }
    fprintf(yyout, "0x%08x,\n", GETDESCENTRY(d, graph));
    if (GETDESCENTRY(d, address) & 1)
      fprintf(yyout, "0x%08x};\n", GETDESCENTRY(d, address) - 1);
    else
      fprintf(yyout, "0x%08x};\n", GETDESCENTRY(d, address));

  }

  DBUG_VOID_RETURN;
}


/***********************************************************************/
/* Prints out the syntaxtree using out_order_list()                    */
/* (to control the parsing).                                           */
/***********************************************************************/

void c_out_tree(PROGRAM *out_me)
{
  void out_order_list();
  void out_desc_list();
  
  FUNCTION *fhelp;
  ORDER *ohelp; 

  DBUG_ENTER("out_tree");

  c_out_desc_list(GETFIRSTDESC(out_me));
  fhelp=GETFIRSTFUNCTION(out_me);
  while (NULL!=fhelp)
  {
    fprintf(yyout,"function %s()\n", GETNAME(fhelp));
    ohelp=GETFIRSTORDER(fhelp);
    c_out_order_list(fhelp);
    fhelp=GETNEXTFUNCTION(fhelp);
  }
  DBUG_VOID_RETURN;
}

/***********************************************************************/
/* Prints out a list of descriptors of functions                       */
/* (used by out_tree()).                                               */
/***********************************************************************/

void c_out_desc_list(FUNDESC *desc)
{
  FUNDESC *help;
  
  DBUG_ENTER("out_desc_list");
  help=desc;
  while (NULL!=help)
  {
    fprintf(yyout, "descriptor: ");
    switch (GETDESCENTRY(help, tag))
    {
    case DT_COMBINATOR:
      fprintf(yyout, "COMB, ");
      break;
    case DT_CONDITIONAL:
      fprintf(yyout, "CONDI, ");
      break;
    case DT_PRIMITIV:
      fprintf(yyout, "PRIM, ");
      break;
    case DT_CASE:
      yyfail("DT_CASE not handled, please call sf");
      break;
    }
    fprintf(yyout, "%d, %d, ",GETDESCENTRY(help, nfv), GETDESCENTRY(help, nv));
    fprintf(yyout, "%08x, ", GETDESCENTRY(help, graph));
    fprintf(yyout, "%s\n", GETDESCENTRY(help,label));
    help=GETNEXTDESC(help);
  }
  DBUG_VOID_RETURN;
} 

/****************************************************************/    
/* looks for a func, desc, cond or data which fits the argument */
/****************************************************************/    

int searchdata(int adr)
{
  int i = 0;
  DATANODE *data = GETFIRSTDATA(program_start);
 
  while (NULL != data)
  {
    if (adr == GETDATAENTRY(data, address)) break;
    data = GETNEXTDATA(data);
    i++;
  }
  if (NULL == data) return(-1);
  return(i);
} 

char *search_desc(int search_me)
{    
  FUNDESC *h = GETFIRSTDESC(program_start);
  
  while (NULL != h)
  {
    if (GETDESCENTRY(h, address) == search_me)
      return(GETDESCENTRY(h, label));
    h = GETNEXTDESC(h);
  }
  return (NULL);
}  

int search_funcdesc(int adr)
{
  int i = 0;
  
  FUNDESC *d = GETFIRSTDESC(program_start);
  while (NULL != d)
  {
    if (adr == GETDESCENTRY(d, address)) break;
    d = GETNEXTDESC(d);
    i++;
  }
  if (NULL == d) return(-1);
  return(i);
}


int search_case(ORDER *o)
{
  while (NULL != o)
  {
    if (label == GETORDERENTRY(o, command))
    {
      if (!strncmp(GETORDERARGS(o, label), "undecided", 9))
      {
        while(mkcase != GETORDERENTRY(o, command))
        {
          o = GETNEXTORDER(o);
          if (NULL == o) yyfail("%s", "mkcase expected after undecide");
        }
        return(search_funcdesc((int)GETORDERARGS(o, desc)));
      }
    }
  o = GETNEXTORDER(o);
  }
  yyfail("%s", "undecided expected after match");
  return(-1);
}


int search_conddesc(char *label)
{
  char *cc;
  int i = 0;
  FUNDESC *d = GETFIRSTDESC(program_start);

  if (NULL == (cc = (char *)malloc(strlen(label) + 2)))
    yyfail("memory allocation failure");
  strcpy(cc, "j");
  strcat(cc, label);  
  while (NULL != d)
  {
    if (!strcmp(cc, GETDESCENTRY(d, label))) break;
    d = GETNEXTDESC(d);
    i++;
  }
  if (NULL == d) return(-1);
  return(i);
}
/****************************************************************/
/* builds a makefile                                            */
/****************************************************************/

void build_makefile(void)
{
  FILE *mkfile;
  char t[300];
  char h_t[16];
  DBUG_ENTER("build_makefile");
  tmpnam(makefilename);
  if (NULL == (mkfile = fopen(makefilename, "w")))
    yyfail("unable to open new Makefile for writing");
  if (DIST)
    fprintf(mkfile, "CC\t\t= ncc\n");
  else if (useacc)
    fprintf(mkfile, "CC\t\t= /usr/local/lang/acc\n");
  else
    fprintf(mkfile, "CC\t\t= gcc\n");
  sprintf(t, "CFLAGS\t\t= -c -D_ANSI_ -DUNIX=1 -DODDSEX=%d \\"
               "-DPI_RED_PLUS=1 -DRED_TO_NF=1 -DUNIX=1 \\"
               "-DSCAVENGE=0 -DSCAV_DEBUG=0 -DWITHTILDE=1 -DCLOSEGOAL=0 \\"
               "-DUH_ZF_PM=1 -DNO_STACK_REG=%d -DD_SLAVE=%d -DNOREF=1 -DSTORE=0", 
                ODDSEX, NOSTACK2REG, DIST);
  if (mess_enabled)
    strcat(t, " -DD_MESS=1 -DM_BINARY -DD_MHEAP=0 -DD_MSTACK=0 -DD_MSCHED=1 -DD_MCOMMU=1");
  if (DIST)
    strcat(t, " -Xc -DnCUBE=1");
  if (!FRED_FISH)
  {
    strcat(t, " -DDBUG_OFF -O");
    sprintf(h_t, "%d ", optimize);
    strcat(t, h_t);
  }
  else
    strcat(t, " -g ");
  strcat(t, "\n");
  fprintf(mkfile, t);
  fprintf(mkfile, "LREDPATH\t= %s\n",redumalib);
  fprintf(mkfile, "LIBPATH\t\t= %s\n",tasmlib);
  fprintf(mkfile, "NCUBELIBPATH\t\t= %s\n",ncubelib);
  fprintf(mkfile, "RTPATH\t\t= %s\n",tasmlib);
  fprintf(mkfile, "RTINC\t\t= %s\n",tasmlib);
  fprintf(mkfile, "LIBS\t\t= -lm\n");
  fprintf(mkfile, "CODEDIR\t\t= .\n");
  fprintf(mkfile, "PROTDIR\t\t= .\n");
  if (DIST)
  fprintf(mkfile, "FILES\t\t= $(RTPATH)/rapply.o $(RTPATH)/rmkclos.o $(RTPATH)/initdata.o "
                "$(RTPATH)/ncdbug.o $(RTPATH)/tlstruct.o "
                "$(RTPATH)/tmvtstruct.o $(RTPATH)/tmvt_val.o $(RTPATH)/intact.o "
                "$(RTPATH)/nr_rbibfunc.o $(RTPATH)/nr_rconvert.o $(RTPATH)/nr_rlogfunc.o "
                "$(RTPATH)/nr_rlstruct.o $(RTPATH)/nr_rmstruct.o $(RTPATH)/nr_rquery.o "
                "$(RTPATH)/nr_rvalfunc.o $(RTPATH)/nr_rvstruct.o $(RTPATH)/nr_rfstruct.o ");
  else
  fprintf(mkfile, "FILES\t\t= $(RTPATH)/rapply.o $(RTPATH)/rmkclos.o $(RTPATH)/initdata.o "
                "$(RTPATH)/dbug.o $(RTPATH)/tlstruct.o "
                "$(RTPATH)/tmvtstruct.o $(RTPATH)/tmvt_val.o $(RTPATH)/intact.o "
                "$(RTPATH)/nr_rbibfunc.o $(RTPATH)/nr_rconvert.o $(RTPATH)/nr_rlogfunc.o "
                "$(RTPATH)/nr_rlstruct.o $(RTPATH)/nr_rmstruct.o $(RTPATH)/nr_rquery.o "
                "$(RTPATH)/nr_rvalfunc.o $(RTPATH)/nr_rvstruct.o $(RTPATH)/nr_rfstruct.o ");
  act_file = first_file;
  while (NULL != act_file)
  {
    fprintf(mkfile, " %s.o", act_file->name);
    act_file = act_file->next;
  }
  if (FRED_FISH)
    fprintf(mkfile, " $(RTPATH)/debug.o");
  fprintf(mkfile, "\n\n");
  fprintf(mkfile, "%s%s:\t$(FILES)\n", execfilepath, execfile);
  if (FRED_FISH && !DIST)
  {
   fprintf(mkfile,"\t\t$(CC) -g -o %s%s $(FILES) "
                  "$(LREDPATH)/lred.o %s -L$(LIBPATH) -DNO_STACK_REG=%d -lm\n",
          execfilepath, execfile, (dostaticlink ? (useacc ? "-Bstatic" :
          "-static") : ""), NOSTACK2REG);
  }
  else if (DIST)
  {
   fprintf(mkfile,"\t\t$(CC) -o %s%s $(FILES) "
                  "$(NCUBELIBPATH)/nctred.o %s -lm -L$(LIBPATH) \n",
          execfilepath, execfile, (dostaticlink ? (useacc ? "-Bstatic" : "-static") : ""));
  }
  else
  {
   fprintf(mkfile,"\t\t$(CC) -o %s%s $(FILES) $(LREDPATH)/lred.o %s -lm "
                  "-L$(LIBPATH) \n",
          execfilepath, execfile, (dostaticlink ? (useacc ? "-Bstatic" : "-static") : ""));
  }
  fprintf(mkfile, "$(RTPATH)/tlstruct.o:\t$(RTPATH)/tlstruct.c\n\t\t$(CC) "
                  "-o $(RTPATH)/tlstruct.o $(CFLAGS) $(RTPATH)/tlstruct.c\n");
  fprintf(mkfile, "$(RTPATH)/rapply.o:\t$(RTPATH)/rapply.c\n\t\t$(CC) "
                  "-o $(RTPATH)/rapply.o $(CFLAGS) $(RTPATH)/rapply.c\n");
  fprintf(mkfile, "$(RTPATH)/rmkclos.o:\t$(RTPATH)/rmkclos.c\n\t\t$(CC) "
                  "-o $(RTPATH)/rmkclos.o $(CFLAGS) $(RTPATH)/rmkclos.c\n");
  fprintf(mkfile, "$(RTPATH)/initdata.o:\t$(RTPATH)/initdata.c\n\t\t$(CC) "
                  "-o $(RTPATH)/initdata.o $(CFLAGS) $(RTPATH)/initdata.c\n");
  fprintf(mkfile, "$(RTPATH)/intact.o:\t$(RTPATH)/intact.c\n\t\t$(CC) "
                  "-o $(RTPATH)/intact.o $(CFLAGS) $(RTPATH)/intact.c\n");
  if (DIST)
    fprintf(mkfile, "$(RTPATH)/ncdbug.o:\t$(RTPATH)/ncdbug.c\n\t\t$(CC) "
                  "-o $(RTPATH)/ncdbug.o $(CFLAGS) $(RTPATH)/ncdbug.c -I$(RTINC)\n");
  else
    fprintf(mkfile, "$(RTPATH)/dbug.o:\t$(RTPATH)/dbug.c\n\t\t$(CC) "
                  "-o $(RTPATH)/dbug.o $(CFLAGS) $(RTPATH)/dbug.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/tmvtstruct.o:\t$(RTPATH)/tmvtstruct.c\n\t\t$(CC) "
               "-o $(RTPATH)/tmvtstruct.o $(CFLAGS) $(RTPATH)/tmvtstruct.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/tmvt_val.o:\t$(RTPATH)/tmvt_val.c\n\t\t$(CC) "
               "-o $(RTPATH)/tmvt_val.o $(CFLAGS) $(RTPATH)/tmvt_val.c -I$(RTINC)\n");

  fprintf(mkfile, "$(RTPATH)/nr_rbibfunc.o:\t$(RTPATH)/nr_rbibfunc.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rbibfunc.o $(CFLAGS) $(RTPATH)/nr_rbibfunc.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/nr_rconvert.o:\t$(RTPATH)/nr_rconvert.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rconvert.o $(CFLAGS) $(RTPATH)/nr_rconvert.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/nr_rlogfunc.o:\t$(RTPATH)/nr_rlogfunc.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rlogfunc.o $(CFLAGS) $(RTPATH)/nr_rlogfunc.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/nr_rlstruct.o:\t$(RTPATH)/nr_rlstruct.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rlstruct.o $(CFLAGS) $(RTPATH)/nr_rlstruct.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/nr_rmstruct.o:\t$(RTPATH)/nr_rmstruct.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rmstruct.o $(CFLAGS) $(RTPATH)/nr_rmstruct.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/nr_rquery.o:\t$(RTPATH)/nr_rquery.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rquery.o $(CFLAGS) $(RTPATH)/nr_rquery.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/nr_rvalfunc.o:\t$(RTPATH)/nr_rvalfunc.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rvalfunc.o $(CFLAGS) $(RTPATH)/nr_rvalfunc.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/nr_rvstruct.o:\t$(RTPATH)/nr_rvstruct.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rvstruct.o $(CFLAGS) $(RTPATH)/nr_rvstruct.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/nr_rfstruct.o:\t$(RTPATH)/nr_rfstruct.c\n\t\t$(CC) "
              "-o $(RTPATH)/nr_rfstruct.o $(CFLAGS) $(RTPATH)/nr_rfstruct.c -I$(RTINC)\n");
  fprintf(mkfile, "$(RTPATH)/debug.o:\t$(RTPATH)/debug.c\n\t\t$(CC) "
              "-o $(RTPATH)/debug.o $(CFLAGS) $(RTPATH)/debug.c -I$(RTINC)\n");
  
  act_file = first_file;
  while (NULL != act_file)
  {
    fprintf(mkfile, "%s.o:\t%s.c\n", act_file->name, act_file->name);
    fprintf(mkfile, "\t\t$(CC) $(CFLAGS) -o %s.o %s.c -I$(RTINC) -I$(PROTDIR)\n", 
                        act_file->name, act_file->name);
    act_file = act_file->next;
  }
  fclose(mkfile);
  DBUG_VOID_RETURN;
}

/****************************************************************/
/* calls a makefile                                             */
/****************************************************************/
void call_makefile(void)
{
  char *make_call;
  DBUG_ENTER("call_makefile");
  if (NULL == (make_call = (char *)malloc(strlen(makefilename) + 10)))
    yyfail("unable to allocate memory");
  sprintf(make_call, "make -f %s", makefilename);
  system(make_call);
  DBUG_VOID_RETURN;
}

/****************************************************************/
/* cleans up the temporarily built files                        */
/****************************************************************/
void clean_up(void)
{
  char s[L_tmpnam + 3], t[40];
  DBUG_ENTER("clean_up");
  act_file = first_file;
  while (NULL != act_file) 
  {
    strcpy(s, act_file->name);
    strcat(s, ".c");
    remove(s);
    strcpy(s, act_file->name);
    strcat(s, ".o");
    remove(s);
    act_file = act_file->next;
  }
  sprintf(t, "%s%s", P_tmpdir, funcprot);
  remove(t);
  remove(makefilename);
  DBUG_VOID_RETURN;
}
 
/***********************************************************************/
/* Prints out a list of ASM~-commands (i.e. a whole function)          */
/* (used by out_tree()).                                               */
/***********************************************************************/
void c_out_order_list(FUNCTION *func)
{
  ORDER *help, *out;
  char text[10240], t[240], s[240], s1[4], t2[40], t1[] = "T_DEC_REFCNT((T_PTD)%s);\n";
  char filename[L_tmpnam + 3];
  int i;

  DBUG_ENTER("out_order_list");
  if ((MAX_LINES < lines) && (!MODUL) && (stdout != yyout) && dogcc)
  {
    fclose(yyout);
    lines = 0;
    if (NULL == (act_file = (OUTNAME *)malloc(sizeof(OUTNAME))))
      yyfail("unable to allocate memory");
    if (NULL == (act_file->name = (char *)malloc(L_tmpnam + 3)))
      yyfail("unable to allocate memory");
    tmpnam(act_file->name);
    last_file->next = act_file;
    act_file->next = NULL;
    last_file = act_file;
    strcpy(filename, act_file->name);
    strcat(filename, ".c");
    if (NULL == (yyout = fopen(filename, "w")))
      yyfail("unable to open file %s", filename);
    
    fprintf(yyout, "#define PROLOG_EXTERN\n");
    if (ISASM_TILDE())
      fprintf(yyout, "#define WITHTILDE 1\n");
    if (TEST_STACK)
      fprintf(yyout, "#define TEST_STACK 1\n");
    if (DIST)
      fprintf(yyout, "#define D_SLAVE 1\n");
    if (dogcc && !DIST && !useacc)
      fprintf(yyout, "#define USES_GCC 1\n");
    fprintf(yyout, "#include \"prolog.h\"\n");
    fprintf(yyout, "#include \"%s\"\n", funcprot);
    fprintf(yyout, "extern int %sdata_table[];\n", modulfile);
    fprintf(yyout, "extern int ta_heapsize, ta_no_of_desc, ta_stacksize;\n");
    fprintf(yyout, "extern PTR_DESCRIPTOR %sfuncdesctable[];\n", modulfile);
    fprintf(yyout, "extern int %sfuncclostable[];\n", modulfile);
    
  }  
  out = GETFIRSTORDER(func);
  help=out;
  while (NULL!=help)
  {
    lines++;
    switch(GETORDERENTRY(help, command))
    {
    case stack_op:
/*****************************************************************/
/* it becomes a push_and_kill macro                              */
/* push_and_kill(term_to_be_computed, kill_w, kill_a)            */
/* with kill_w : entries to be cleared from w                    */
/* and  kill_a : entries to be cleared from a                    */
/*****************************************************************/
      if (0 != GETORDERARGS(help, k))
        strcpy(s, "{\nint ");
      else
        strcpy(s, "");
      if (0 == GETORDERENTRY(help, opti))
        strcpy(s, "");
      strcpy(t, "");
      for (i = 0; i < GETORDERARGS(help, k); i++)
      {
        if (0 != GETORDERENTRY(help, opti))
        {
          sprintf(s1, "__t%02x", i);
          strcat(s, s1);
          strcat(s, "=1");
          if (i < GETORDERARGS(help, k) - 1)
          {
            sprintf(t2, t1, s1);
            strcat(t, t2);
          }
          if (i != GETORDERARGS(help, k) - 1)
            strcat(s, ", ");
          strrepl(&GETORDERENTRY(help,code)[find_substr(GETORDERENTRY(help, code),
                                                          "#####")], s1, 5);
        }
        else
         strrepl(&GETORDERENTRY(help,code)[find_substr(GETORDERENTRY(help, code),
                                                           "#####")], "t    ", 5);
          
      }
      if ((0 != GETORDERENTRY(help, opti)) && (0 < GETORDERARGS(help, k)))
        {
          strcat(s, ";\n");
          strcat(t, "}\n");
        }
      if ((0 == GETORDERENTRY(help, opti)) && (0 == GETORDERARGS(help, m)))
        sprintf(text, "%spush_and_no_w_kill_atom(%s, %d, %d);\n%s", s, 
                                                    GETORDERENTRY(help, code),
                                                    GETORDERARGS(help, m),
                                                    GETORDERARGS(help, n), t);
      else if ((0 == GETORDERENTRY(help, opti)) && (0 != GETORDERARGS(help, m)))
        sprintf(text, "%spush_and_kill_atom(%s, %d, %d);\n%s", s, 
                                              GETORDERENTRY(help, code), 
                                              GETORDERARGS(help, m), 
                                              GETORDERARGS(help, n), t);
      else if ((0 != GETORDERENTRY(help, opti)) && (0 == GETORDERARGS(help, m)))
        sprintf(text, "%spush_and_no_w_kill(%s, %d, %d);\n%s", s, 
                                              GETORDERENTRY(help, code),
                                              GETORDERARGS(help, m), 
                                              GETORDERARGS(help, n), t);
      else if ((0 != GETORDERENTRY(help, opti)) && (0 != GETORDERARGS(help, m)))
        sprintf(text, "%spush_and_kill(%s, %d, %d);\n%s", s, 
                                              GETORDERENTRY(help, code),  
                                              GETORDERARGS(help, m),
                                              GETORDERARGS(help, n), t);
        
        
      break;
    case ris_stack_op:
/*****************************************************************/
/* it becomes a keep_and_kill macro                              */
/* keep_and_kill(term_to_be_computed, kill_w, kill_a)            */
/* with kill_w : entries to be cleared from w                    */
/* and  kill_a : entries to be cleared from a                    */
/*****************************************************************/
      if (0 != GETORDERARGS(help, k))
        strcpy(s, "{\nint ");
      else
        strcpy(s, "");
      if (0 == GETORDERENTRY(help, opti))
        strcpy(s, "");
      strcpy(t, "");
      for (i = 0; i < GETORDERARGS(help, k); i++)
      {
        if (0 != GETORDERENTRY(help, opti))
        {
          sprintf(s1, "__t%02x", i);
          strcat(s, s1);
          strcat(s, "=1");
          if (i < GETORDERARGS(help, k) - 1)
          {
            sprintf(t2, t1, s1);
            strcat(t, t2);
          }
          if (i != GETORDERARGS(help, k) - 1)
            strcat(s, ", ");
          strrepl(&GETORDERENTRY(help,code)[find_substr(GETORDERENTRY(help, code),
                                                          "#####")], s1, 5);
        }
        else
         strrepl(&GETORDERENTRY(help,code)[find_substr(GETORDERENTRY(help, code),
                                                           "#####")], "t    ", 5);
          
      }
      if ((0 != GETORDERENTRY(help, opti)) && (0 < GETORDERARGS(help, k)))
        {
          strcat(s, ";\n");
          strcat(t, "}\n");
        }
      if ((0 == GETORDERENTRY(help, opti)) && (0 == GETORDERARGS(help, m)))
        sprintf(text, "%skeep_and_no_w_kill_atom(%s, %d, %d);\n%s", s, 
                                                    GETORDERENTRY(help, code),
                                                    GETORDERARGS(help, m),
                                                    GETORDERARGS(help, n), t);
      else if ((0 == GETORDERENTRY(help, opti)) && (0 != GETORDERARGS(help, m)))
        sprintf(text, "%skeep_and_kill_atom(%s, %d, %d);\n%s", s, 
                                              GETORDERENTRY(help, code), 
                                              GETORDERARGS(help, m), 
                                              GETORDERARGS(help, n), t);
      else if ((0 != GETORDERENTRY(help, opti)) && (0 == GETORDERARGS(help, m)))
        sprintf(text, "%skeep_and_no_w_kill(%s, %d, %d);\n%s", s, 
                                              GETORDERENTRY(help, code),
                                              GETORDERARGS(help, m), 
                                              GETORDERARGS(help, n), t);
      else if ((0 != GETORDERENTRY(help, opti)) && (0 != GETORDERARGS(help, m)))
        sprintf(text, "%skeep_and_kill(%s, %d, %d);\n%s", s, 
                                              GETORDERENTRY(help, code),  
                                              GETORDERARGS(help, m),
                                              GETORDERARGS(help, n), t);
        
        
      break;
    case uses_aux_var: 
      sprintf(text,"/* next delta uses pushaux */");
      break;
    case mklist: 
    {
      sprintf(text,"mklist(%d);",GETORDERARGS(help, n));
      break;
    }
    case mkilist: 
    {
      sprintf(text,"mkilist(%d);",GETORDERARGS(help, n));
      break;
    }
/* cr 22/03/95, kir(ff), START */
    case mkslot: 
    {
      sprintf(text,"mkslot();");
      break;
    }
    case mkframe: 
    {
      sprintf(text,"mkframe(%d);",GETORDERARGS(help, n));
      break;
    }
    case Inter: 
    {
      sprintf(text,"inter(%s);", c_out_intertype( GETORDERARGS(help, n)));
      break;
    }
/* cr 22/03/95, kir(ff), END */
    case pushcw_pf:
    {
      if (ISIA(GETORDERARGS(help,n)))
        sprintf(text,"push((int)%s);",intact_tab[GETIA(GETORDERARGS(help,n))]);
      else
        sprintf(text,"push((int)%s);",primf_tab[GETPRF(GETORDERARGS(help,n))]);
      break;
    }
    case pushcw_i: 
    {
      sprintf(text,"push((int)TAG_INT(%d));",  
              GETINT(GETORDERARGS(help, n)));
      break;
    }
    case pushcr_i: 
    {
      sprintf(text,"pushr(TAG_INT(%d));", 
              GETINT(GETORDERARGS(help, n)));
      break;
    }
    case pushh:
      sprintf(text,"push_h(%d);", GETORDERARGS(help, n));
      break;
    case count:
      sprintf(text,"count(%d);", GETORDERARGS(help, n));
      break;
    case poph:
      sprintf(text,"pop_h();");
      break;
    case msdistend:
      sprintf(text,"msdistend();");
      break;
    case msnodist:
      sprintf(text,"msnodist();");
      break;
    case pushw_p: 
    {
      int n;
      
      n = search_funcdesc((int)GETORDERARGS(help, desc));
      if (-1 != n)
      {
        if (ref_inferred)
          sprintf(text,"nr_push_p((T_PTD)%sfuncclostable[%d]);", modulfile, n);
        else
          sprintf(text,"push_p((T_PTD)%sfuncclostable[%d]);", modulfile, n);
      }
      else
      {
        n = searchdata((int)GETORDERARGS(help, desc));
        if (-1 != n)
        {
          if (ref_inferred)
            sprintf(text,"nr_push_p((T_PTD)%sdata_table[%d]);", modulfile, n);
          else
            sprintf(text,"push_p((T_PTD)%sdata_table[%d]);", modulfile, n);
        }
        else 
        { 
          if (stdout != yyout)
          {
            fclose(yyout);
            remove(outfile);
            yyfail("push(0x%08x) unrecognized pointer\n%s removed",
                   (int)GETORDERARGS(help, desc), outfile);
          }
          yyfail("push(0x%08x) unrecognized pointer",
                 (int)GETORDERARGS(help, desc));
        }
      }
        
      break;
    }
    case pushr_p: 
    {
      int n;
      
      n = search_funcdesc((int)GETORDERARGS(help, desc));
      if (-1 != n)
      {
        if (ref_inferred)
          sprintf(text,"nr_pushr_p((T_PTD)%sfuncclostable[%d]);", modulfile, n);
        else
          sprintf(text,"pushr_p((T_PTD)%sfuncclostable[%d]);", modulfile, n);
      }
      else
      {
        n = searchdata((int)GETORDERARGS(help, desc));
        if (-1 != n)
        {
          if (ref_inferred)
            sprintf(text,"nr_pushr_p((T_PTD)%sdata_table[%d]);", modulfile, n);
          else
            sprintf(text,"pushr_p((T_PTD)%sdata_table[%d]);", modulfile, n);
        }
        else 
        { 
          if (stdout != yyout)
          {
            fclose(yyout);
            remove(outfile);
            yyfail("pushr_p(0x%08x) unrecognized pointer\n%s removed",
                   (int)GETORDERARGS(help, desc), outfile);
          }
          yyfail("pushr_p(0x%08x) unrecognized pointer",
                 (int)GETORDERARGS(help, desc));
        }
      }
      break;
    }
      
    case i_rtp:
    {
      int n;
      
      n = search_funcdesc((int)GETORDERARGS(help, desc));
      if (-1 != n)
        sprintf(text,"i_rtp((T_PTD)%sfuncclostable[%d], %d);", modulfile, n, GETORDERARGS(help, m));
      else
      {
        n = searchdata((int)GETORDERARGS(help, desc));
        if (-1 != n)
          sprintf(text,"i_rtp((T_PTD)%sdata_table[%d], %d);", modulfile, n, GETORDERARGS(help, m));
        else 
        { 
          if (stdout != yyout)
          {
            fclose(yyout);
            remove(outfile);
            yyfail("rtp(0x%08x) unrecognized pointer\n%s removed",
                   (int)GETORDERARGS(help, desc), outfile);
          }
          yyfail("rtp(0x%08x) unrecognized pointer",
                 (int)GETORDERARGS(help, desc));
        }
      }
      break;
    }
    
    case rtp: 
    {
      int n;
      
      n = search_funcdesc((int)GETORDERARGS(help, desc));
      if (-1 != n)
        sprintf(text,"rtp((T_PTD)%sfuncclostable[%d]);", modulfile, n);
      else
      {
        n = searchdata((int)GETORDERARGS(help, desc));
        if (-1 != n)
          sprintf(text,"rtp((T_PTD)%sdata_table[%d]);", modulfile, n);
        else 
        { 
          if (stdout != yyout)
          {
            fclose(yyout);
            remove(outfile);
            yyfail("rtp(0x%08x) unrecognized pointer\n%s removed",
                   (int)GETORDERARGS(help, desc), outfile);
          }
          yyfail("rtp(0x%08x) unrecognized pointer",
                 (int)GETORDERARGS(help, desc));
        }
      }
      break;
    }
      case pushaw:
    {
      if (ref_inferred)
        sprintf(text,"nr_pushaw(%d);",GETORDERARGS(help,n));
      else
        sprintf(text,"pushaw(%d);",GETORDERARGS(help,n));
      break;
    }
      case pushtw:
    {
      if (ref_inferred)
        sprintf(text,"nr_pushtw(%d);",GETORDERARGS(help,n));
      else
        sprintf(text,"pushtw(%d);",GETORDERARGS(help,n));
      break;
    }
      case pushar:
    {
      if (ref_inferred)
        sprintf(text,"nr_pushar(%d);",GETORDERARGS(help,n));
      else
        sprintf(text,"pushar(%d);",GETORDERARGS(help,n));
      break;
    }
      case pushtr:
    {
      if (ref_inferred)
        sprintf(text,"nr_pushtr(%d);",GETORDERARGS(help,n));
      else
        sprintf(text,"pushtr(%d);",GETORDERARGS(help,n));
      break;
    }
      case moveaw:
    {
      sprintf(text,"moveaw();");
      break;
    }
      case movear:
    {
      sprintf(text,"movear();");
      break;
    }
      case delta1:
    {
      sprintf(text,"delta1();");
      break;
    }
      case delta2:
    {
      sprintf(text,"delta2();");
      break;
    }
      case delta3:
    {
      sprintf(text,"delta3();");
      break;
    }
      case delta4:
    {
      sprintf(text,"delta4();");
      break;
    }
      case intact:
    {
      sprintf(text,"mkintact(%s);\n",
                                intact_tab[GETIA(GETORDERARGS(help, primf))]);
      break;
    }
      case freeswt:
    {
      if (ref_inferred)
        sprintf(text,"nr_freeswt(%d);",GETORDERARGS(help,n));
      else
        sprintf(text,"freeswt(%d);",GETORDERARGS(help,n));
      break;
    }
      case freea:
    {
      if (ref_inferred)
        sprintf(text,"nr_freea(%d);",GETORDERARGS(help,n));
      else
      {
        sprintf(text, "T_DEC_REFCNT((T_PTD)top(a)); drop(a, 1);");
        for (i = 1; i < GETORDERARGS(help,n); i++)
          strcat(text, "T_DEC_REFCNT((T_PTD)top(a)); drop(a, 1);");
      }
      break;
    }
      case freer:
    {
      if (ref_inferred)
        sprintf(text,"nr_freer(%d);",GETORDERARGS(help,n));
      else
      {
        sprintf(text, "T_DEC_REFCNT((T_PTD)top(r)); drop(r, 1);\n");
        for (i = 1; i < GETORDERARGS(help,n); i++)
          strcat(text, "T_DEC_REFCNT((T_PTD)top(r)); drop(r, 1);\n");
      }
      break;
    }
      case freet:
    {
      if (ref_inferred)
        sprintf(text,"nr_freet(%d);",GETORDERARGS(help,n));
      else
      {
        sprintf(text, "T_DEC_REFCNT((T_PTD)top(t)); drop(t, 1);\n");
        for (i = 1; i < GETORDERARGS(help,n); i++)
          strcat(text, "T_DEC_REFCNT((T_PTD)top(t)); drop(t, 1);\n");
      }
      break;
    }
      case freew:
        sprintf(text, "freew(%d);", GETORDERARGS(help, n));
        break;
      case snap:
    {
      sprintf(text,"snap(%d);",GETORDERARGS(help,n));
      break;
    }
      case rtt:
    {
      sprintf(text,"rtt();");
      break;
    }
      case i_rtt:
    {
      sprintf(text,"i_rtt(%d);", GETORDERARGS(help,m));
      break;
    }
      case i_rtf:
    {
      sprintf(text,"i_rtf(%d);", GETORDERARGS(help,m));
      break;
    }
      case rtf:
    {
      sprintf(text,"rtf();");
      break;
    }
      case i_rtm:
    {
      sprintf(text,"i_rtm(%d);", GETORDERARGS(help,m));
      break;
    }
      case rtm:
    {
      sprintf(text,"rtm();");
      break;
    }
      case i_rtc_i:
    {
      sprintf(text,"i_rtc(TAG_INT(%d), %d);",
              GETINT(GETORDERARGS(help,n)), \
              GETORDERARGS(help,m));
      break;
    }
      case rtc_i:
    {
      sprintf(text,"rtc(TAG_INT(%d));",
              GETINT(GETORDERARGS(help,n)));
      break;
    }
      case mkdclos: 
    {
      sprintf(text,"f_mkdclos(-1, %d, %d, -1);",GETORDERARGS(help,m), \
              GETORDERARGS(help,n));
      break;
    }
/* Commands with only one bool argument, treated as integer */
      case pushcw_b:
    {
      if (SA_FALSE == GETORDERARGS(help,n))
        sprintf(text,"push((int)%d); /* SA_FALSE */", SA_FALSE);
      else
        sprintf(text,"push((int)%d); /* SA_TRUE */", SA_TRUE);
      break;
    }
      case i_rtc_b:
    {
      if (SA_TRUE == GETORDERARGS(help, n))
        sprintf(text,"rtc(%d, %d); /* SA_TRUE */", SA_TRUE, GETORDERARGS(help,m));
      else
        sprintf(text,"rtc(%d, %d); /* SA_FALSE */", SA_FALSE, GETORDERARGS(help,m));
      break;
    }
      case rtc_b:
    {
      if (SA_TRUE == GETORDERARGS(help, n))
        sprintf(text,"rtc(%d); /* SA_TRUE */", SA_TRUE);
      else
        sprintf(text,"rtc(%d); /* SA_FALSE */", SA_FALSE);
      break;
    }
      case rtc_pf:
    {
        if (ISIA(GETORDERARGS(help,n)))
          sprintf(text,"rtc(%s);", intact_tab[GETIA(GETORDERARGS(help,n))]);
        else
          sprintf(text,"rtc(%s);", primf_tab[GETPRF(GETORDERARGS(help,n))]);
        break;
    }
      case pushcr_b:
    {
      if (SA_FALSE == GETORDERARGS(help, n))
        sprintf(text,"pushr(%d); /* SA_FALSE */", SA_FALSE);
      else
        sprintf(text,"pushr(%d); /* SA_TRUE */", SA_TRUE);
      break;
    }
      case mkgaclos:
    {
      sprintf(text,"f_mkgaclos(%d, %sfuncdesctable[%d], %d, %d);",GETORDERARGS(help, n), 
        modulfile, search_funcdesc((int)GETORDERARGS(last_push_r_before(help), desc)), 1, 0);
      break;
    }
      case mkgsclos:
    {
      sprintf(text,"f_mkgaclos(%d, %sfuncdesctable[%d], %d, %d);",GETORDERARGS(help, n), 
        modulfile, search_funcdesc((int)GETORDERARGS(last_push_r_before(help), desc)), 1, 1);
      break;
    }
      case mkcclos:
    {
      sprintf(text,"mkcclos(%d, %sfuncdesctable[%d]);",GETORDERARGS(help, n),
        modulfile, search_funcdesc((int)GETORDERARGS(GETPREVORDER(help), desc)));
      break;
    }
      case mkiclos:
    {
      sprintf(text,"f_mkiclos(%d);", GETORDERARGS(help, n));
      break;
    }
      case mkbclos:
    {
      sprintf(text,"dyn_mkbclos(%d);",GETORDERARGS(help, n));
      break;
    }
      case mksclos:
    {
      sprintf(text,"mksclos(%d, %d, %d);", GETORDERARGS(help, n),
                                           GETORDERARGS(help, m),
                                           GETORDERARGS(help, k));
      break;
    }
    case jump:
    {
      sprintf(text, "goto %s;", GETORDERARGS(help, label));
      break;
    }
      case jcond2:
    {
      sprintf(text,"jcond2(%s, %s, %sfuncdesctable[%d]);", GETORDERARGS(help, label), 
                    GETORDERARGS(help, ret),
                    modulfile, search_conddesc(GETORDERARGS(help, label)));
      break;
    }
      case jcond:
    {
      sprintf(text,"jcond(%s, %sfuncdesctable[%d]);",GETORDERARGS(help, label),
                    modulfile, search_conddesc(GETORDERARGS(help, label)));

      break;
    }
      case jfalse:
    {
      sprintf(text,"jfalse(%s, %sfuncdesctable[%d]);",GETORDERARGS(help, label),
                    modulfile, search_conddesc(GETORDERARGS(help, label)));

      break;
    }
      case jtrue:
    {
      sprintf(text,"jtrue(%s, %sfuncdesctable[%d]);",GETORDERARGS(help, label),
                    modulfile, search_conddesc(GETORDERARGS(help, label)));
      break;
    }
      case beta:
    {
      sprintf(text,"beta(%s, %d);", GETORDERARGS(help, label), 
                                    GETORDERARGS(help, n));
      break;
    }
      case betanear:
    {
      sprintf(text,"betanear(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
      break;
    }
      case betafar:
    {
      sprintf(text,"betafar(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
      break;
    }
      case gammabeta:
        sprintf(text,"gammabeta(%s, %d);", GETORDERARGS(help, label), 
                                      GETORDERARGS(help, n));
        break;
      case gammabetanear:
    {
      sprintf(text,"gammabetanear(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
      break;
    }
      case gammabetafar:
    {
      sprintf(text,"gammabetafar(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
      break;
    }
      case mkap:
    {
      sprintf(text, "mkap(%d);", GETORDERARGS(help, n));
      break;
    }
      case apply:
    {
      sprintf(text,"ap(%d,%d,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, m),
              GETORDERARGS(help, ret));
      break;
    }
      case Gamma:
    {
      sprintf(text,"Gamma(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
      break;
    }
      case tailnear:
    {
      sprintf(text,"tailnear();");
      break;
    }
      case tailfar:
    {
      sprintf(text,"tailfar(&%s, &%s);",GETORDERARGS(help, label),
              GETORDERARGS(help, ret));
      break;
    }
      case tail:
    {
      sprintf(text,"tail(%s);",GETORDERARGS(help, label));
      break;
    }
      case label:
    {
      sprintf(text,"%s:",GETORDERARGS(help, label));
      if (0 != GETORDERARGS(help, m))
      {
        sprintf(s1, "freew(%d);\n", GETORDERARGS(help, m));
        strcat(text, s1);
      }
      if (0 != GETORDERARGS(help, l))
      {
        sprintf(s1, "freea(%d);\n", GETORDERARGS(help, l));
        strcat(text, s1);
      }
      break;
    }
      case ext:
    {
      sprintf(text,"ext();");
      break;
    }
      case end:
    {
      sprintf(text,"end();");
      break;
    }
      case code_ok:
    {
      if (0 == GETORDERARGS(help, k))
        sprintf(text,"%s", GETORDERENTRY(help, code));
      else
      {
        if (0 != GETORDERENTRY(help, opti))
        {
          strcpy(s, "{\nint ");
          for (i = 0; i < GETORDERARGS(help, k); i++)
          {
            sprintf(s1, "__t%02x", i);
            strcat(s, s1);
            strcat(s, "=1");
            if (i < GETORDERARGS(help, k) - 1)
            {
              sprintf(t2, t1, s1);
              strcat(text, t2);
            }
            if (i != GETORDERARGS(help, k) - 1)
              strcat(s, ", ");
           strrepl(&GETORDERENTRY(help,code)[find_substr(GETORDERENTRY(help, code),
                                                                "#####")], s1, 5);
          }
          strcat(s, ";\n");
        }
        else
        {
          strcpy(s, "");
          for (i = 0; i < GETORDERARGS(help, k); i++)
            strrepl(&GETORDERENTRY(help,code)[find_substr(GETORDERENTRY(help, code),
                                                               "#####")], "t    ", 5);
        }
        sprintf(text, "%s %s\n", s, GETORDERENTRY(help,code));
      }
      break;
    }
      case hashargs:
    {
      sprintf(text,"/* hashargs */");
      break;
    }
      case hashtildeargs:
    {
      sprintf(text,"/* hashtildeargs */");
      break;
    }
      case hashrestype:
    {
      sprintf(text,"/* hashrestype */");
      break;
    }
      case hashsetref:
    {
      if (SREFexplicit == GETORDERARGS(help, n))
        ref_inferred = TRUE;
      if ((SREFimplicit == GETORDERARGS(help, n)) || (SREFkeepimpl==GETORDERARGS(help, n)))
        ref_inferred = FALSE;
      if (0 == GETORDERARGS(help, n))
        sprintf(text,"SET_REFCNT_IMPL();");
      else
        sprintf(text,"SET_REFCNT_EXPL();");
      break;
    }
      case casenear:
    {
      sprintf(text,"casenear(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
      break;
    }
      case casefar:
    {
      sprintf(text,"casefar(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
      break;
      case Case:
        sprintf(text,"Case(%s, %d);", GETORDERARGS(help, label), 
                                      GETORDERARGS(help, n));
        break;
    }
      case advance:
        sprintf(text, "advance(%d);", GETORDERARGS(help, n));
        break;
      case atend:
        sprintf(text, "atend(%d, %s);", GETORDERARGS(help, n), 
                                        GETORDERARGS(help, label));
        break;
      case atstart:
        sprintf(text, "atstart(%d, %s);", GETORDERARGS(help, n), 
                                          GETORDERARGS(help, label));
        break;
      case bind:
        if (ref_inferred)
          sprintf(text, "nr_bind(%d);", GETORDERARGS(help, n));
        else
          sprintf(text, "bind(%d);", GETORDERARGS(help, n));
        break;
      case binds:
        if (ref_inferred)
          sprintf(text, "nr_binds(%d);", GETORDERARGS(help, n));
        else
          sprintf(text, "binds(%d);", GETORDERARGS(help, n));
        break;
      case bindsubl:
        if (ref_inferred)
          sprintf(text, "nr_bindsubl(%d, %d, %d, %d);", GETORDERARGS(help, n),
                                                   GETORDERARGS(help, m),
                                                   GETORDERARGS(help, k),
                                                   GETORDERARGS(help, l));
        else
          sprintf(text, "bindsubl(%d, %d, %d, %d);", GETORDERARGS(help, n),
                                                   GETORDERARGS(help, m),
                                                   GETORDERARGS(help, k),
                                                   GETORDERARGS(help, l));
        break;
      case dereference:
        if (pick == GETORDERENTRY(GETNEXTORDER(help), command))
          sprintf(text, "/* DEREFENCE REMOVED */");
        else
          sprintf(text, "dereference();");
        break;
      case drop:
        if (ref_inferred)
          sprintf(text, "freew(1);");
        else
          sprintf(text, "T_DEC_REFCNT((T_PTD)top(w));\nfreew(1);");
        break;
      case endlist:
        sprintf(text, "endlist();");
        break;
      case endsubl:
        sprintf(text, "endsubl(%d);", GETORDERARGS(help, n));
        break;
      case fetch:
        if (ref_inferred)
          sprintf(text, "nr_fetch();");
        else
          sprintf(text, "fetch();");
        break;
      case gammacasenear:
        sprintf(text,"gammacasenear(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
        break;
      case gammacasefar:
        sprintf(text,"gammacasefar(%d,&%s,&%s);",GETORDERARGS(help, n),
              GETORDERARGS(help, ret),
              GETORDERARGS(help, label));
        break;
      case gammacase:
        sprintf(text,"gammacase(%s, %d);", GETORDERARGS(help, label),
                                           GETORDERARGS(help, n));
        break;
      case initbt:
        sprintf(text, "initbt(%d, %d, %d, %d, %d);", GETORDERARGS(help, n),
                                                     GETORDERARGS(help, m),
                                                     GETORDERARGS(help, k),
                                                     GETORDERARGS(help, l),
                                                     GETORDERARGS(help, j));
        break;
      case matcharb:
        if (0 == GETORDERARGS(help, n))

          sprintf(text,"matcharb_0(%d, %s, %s, %d);", GETORDERARGS(help, n),
                                              GETORDERARGS(help, label),
                                              GETORDERARGS(help, ret),
                                              search_case(help));
        else
          sprintf(text,"matcharb(%d, %s, %s, %d);", GETORDERARGS(help, n),
                                              GETORDERARGS(help, label),
                                              GETORDERARGS(help, ret),
                                              search_case(help));
        break;
      case matcharbs:
        sprintf(text,"matcharbs(%d, %s, %s, %d);", GETORDERARGS(help, n),
                                               GETORDERARGS(help, label),
                                               GETORDERARGS(help, ret),
                                               search_case(help));
        break;
      case matchbool:
        sprintf(text,"matchbool(%d, %s, %s, %d);", GETORDERARGS(help, n),
                                            GETORDERARGS(help, label),
                                            GETORDERARGS(help, ret),
                                            search_case(help));
        break;
      case matchin:
        sprintf(text,"matchin((T_PTD)%sdata_table[%d], %s, %s, %d);", 
                                             modulfile, 
                                             searchdata((int)GETORDERARGS(help, n)),
                                             GETORDERARGS(help, label),
                                             GETORDERARGS(help, ret),
                                             search_case(help));
        break;
      case matchint:
        sprintf(text,"matchint(%d, %s, %s, %d);", GETORDERARGS(help, n),
                                              GETORDERARGS(help, label),
                                              GETORDERARGS(help, ret),
                                              search_case(help));
        break;
      case matchlist:
        sprintf(text,"matchlist(%d, %s, %s, %d);", GETORDERARGS(help, n),
                                               GETORDERARGS(help, label),
                                               GETORDERARGS(help, ret),
                                               search_case(help));
        break;
      case matchprim:
        if (ISPRF(GETORDERARGS(help,n)))
          sprintf(text,"matchprim(%s, %s, %s, %d);", 
                                   primf_tab[GETPRF(GETORDERARGS(help,n))],
                                   GETORDERARGS(help, label),
                                   GETORDERARGS(help, ret),
                                   search_case(help));
        else if (ISIA(GETORDERARGS(help,n)))
          sprintf(text,"matchprim(%s, %s, %s, %d);",
                                   intact_tab[GETIA(GETORDERARGS(help,n))],
                                   GETORDERARGS(help, label),
                                   GETORDERARGS(help, ret),
                                   search_case(help));
        else if (ISCHAR(GETORDERARGS(help,n)))
          sprintf(text,"matchprim(MAKE_CHAR('%c'), %s, %s, %d);", 
                                                     GETCHAR(GETORDERARGS(help,n)), 
                                                     GETORDERARGS(help, label),
                                                     GETORDERARGS(help, ret),
                                                     search_case(help));
        break;
      case matchstr:
        sprintf(text,"matchstr(%d, %s, %s, %d);", GETORDERARGS(help, n),
                                              GETORDERARGS(help, label),
                                              GETORDERARGS(help, ret),
                                              search_case(help));
        break;
      case mkaframe:
        sprintf(text, "mkaframe(%d);", GETORDERARGS(help, n));
        break;
      case mkbtframe:
        sprintf(text, "mkbtframe(%d);", GETORDERARGS(help, n));
        break;
      case mkcase:
        sprintf(text,"mkcase(%d,%sfuncdesctable);", 
                     search_funcdesc((int)GETORDERARGS(help, desc)), modulfile);
        break;
      case mkwframe:
        sprintf(text, "mkwframe(%d);", GETORDERARGS(help, n));
        break;
      case nestlist:
        sprintf(text, "nestlist();");
        break;
      case pick:
        sprintf(text, "pick(%d);", GETORDERARGS(help, n));
        break;
      case restorebt:
        sprintf(text, "restorebt(%d);", GETORDERARGS(help, n));
        break;
      case restoreptr:
        sprintf(text, "restoreptr(%d);", GETORDERARGS(help, n));
        break;
      case rmbtframe:
        sprintf(text, "rmbtframe(%d);", GETORDERARGS(help, n));
        break;
      case rmwframe:
        sprintf(text, "rmwframe(%d);", GETORDERARGS(help, n));
        break;
      case savebt:
        sprintf(text, "savebt(%d);", GETORDERARGS(help, n));
        break;
      case saveptr:
        sprintf(text, "saveptr(%d);", GETORDERARGS(help, n));
        break;
      case startsubl:
        sprintf(text, "startsubl(%d, %d);", GETORDERARGS(help, n), 
                                            GETORDERARGS(help, m));
        break;
      case tguard:
        sprintf(text, "tguard(%s, %d, %d, %d, %d, %sfuncdesctable);", GETORDERARGS(help, label), 
                                 search_funcdesc((int)GETORDERARGS(help, desc)),
                                                     GETORDERARGS(help, n),
                                                     GETORDERARGS(help, m),
                                                     GETORDERARGS(help, l),
                                                     modulfile );
        break;
       case pushaux:
         sprintf(text, "pushaux();");
         break;
       case decw:
         sprintf(text, "decw(%d);", GETORDERARGS(help, n));
         break;
       case deca:
         sprintf(text, "deca(%d);", GETORDERARGS(help, n));
         break;
       case decr:
         sprintf(text, "decr(%d);", GETORDERARGS(help, n));
         break;
       case dect:
         sprintf(text, "dect(%d);", GETORDERARGS(help, n));
         break;
       case tdecw:
         sprintf(text, "tdecw(%d);", GETORDERARGS(help, n));
         break;
       case tdeca:
         sprintf(text, "tdeca(%d);", GETORDERARGS(help, n));
         break;
       case tdecr:
         sprintf(text, "tdecr(%d);", GETORDERARGS(help, n));
         break;
       case tdect:
         sprintf(text, "tdect(%d);", GETORDERARGS(help, n));
         break;

       case tincw:
         sprintf(text, "tincw(%d);", GETORDERARGS(help, n));
         strcpy(t, text);
         for (i = 1; i < GETORDERARGS(help, m); i++)
           strcat(text, t);
         break;
       case tinca:
         sprintf(text, "tinca(%d);", GETORDERARGS(help, n));
         strcpy(t, text);
         for (i = 1; i < GETORDERARGS(help, m); i++)
           strcat(text, t);
         break;
       case tincr:
         sprintf(text, "tincr(%d);", GETORDERARGS(help, n));
         strcpy(t, text);
         for (i = 1; i < GETORDERARGS(help, m); i++)
           strcat(text, t);
         break;
       case tinct:
         sprintf(text, "tinct(%d);", GETORDERARGS(help, n));
         strcpy(t, text);
         for (i = 1; i < GETORDERARGS(help, m); i++)
           strcat(text, t);
         break;
       case incw:
         sprintf(text, "incw(%d);", GETORDERARGS(help, n));
         strcpy(t, text);
         for (i = 1; i < GETORDERARGS(help, m); i++)
           strcat(text, t);
         break;
       case inca:
         sprintf(text, "inca(%d);", GETORDERARGS(help, n));
         strcpy(t, text);
         for (i = 1; i < GETORDERARGS(help, m); i++)
           strcat(text, t);
         break;
       case incr:
         sprintf(text, "incr(%d);", GETORDERARGS(help, n));
         strcpy(t, text);
         for (i = 1; i < GETORDERARGS(help, m); i++)
           strcat(text, t);
         break;
       case inct:
         sprintf(text, "inct(%d);", GETORDERARGS(help, n));
         strcpy(t, text);
         for (i = 1; i < GETORDERARGS(help, m); i++)
           strcat(text, t);
         break;

       case killw:
         sprintf(text, "killw(%d);", GETORDERARGS(help, n));
         break;
       case killa:
         sprintf(text, "killa(%d);", GETORDERARGS(help, n));
         break;
       case killr:
         sprintf(text, "killr(%d);", GETORDERARGS(help, n));
         break;
       case killt:
         sprintf(text, "killt(%d);", GETORDERARGS(help, n));
         break;
       case tkillw:
         sprintf(text, "tkillw(%d);", GETORDERARGS(help, n));
         break;
       case tkilla:
         sprintf(text, "tkilla(%d);", GETORDERARGS(help, n));
         break;
       case tkillr:
         sprintf(text, "tkillr(%d);", GETORDERARGS(help, n));
         break;
       case tkillt:
         sprintf(text, "tkillt(%d);", GETORDERARGS(help, n));
         break;
       case dist:
         sprintf(text, "dist(%d, %d, %d, %d, %d, %d, %d);", 
                        search_funcname(GETORDERENTRY(help, code)),    /* funktionindex */
                        GETORDERARGS(help, j),     /* function label behind this order */
                        GETORDERARGS(GETPREVORDER(GETPREVORDER(
                        search_label(GETORDERARGS(help, ret), help))), j),
                        GETORDERARGS(help, n),
                        GETORDERARGS(help, m),
                        GETORDERARGS(help, k),
                        GETORDERARGS(help, l));

         if (!ISASM_TILDE())
           strcat(text, "/* last two args ignored (no tilde version) */");
         break;
       case distb:
         sprintf(text, "distb(%d, %d, %d, %d, %d, %d, %d);", 
                        search_funcname(GETORDERENTRY(help, code)),    /* funktionindex */
                        GETORDERARGS(help, j),     /* function label behind this order */
                        GETORDERARGS(GETPREVORDER(GETPREVORDER(
                        search_label(GETORDERARGS(help, ret), help))), j),
                        GETORDERARGS(help, n),
                        GETORDERARGS(help, m),
                        GETORDERARGS(help, k),
                        GETORDERARGS(help, l));
         if (!ISASM_TILDE())
           strcat(text, "/* last two args ignored (no tilde version) */");
         break;
       case distend:
         sprintf(text, "distend();");
         break; 
       case wait:
         sprintf(text, "wait(%d, %d, %d);", 
                        search_funcname(GETORDERENTRY(help, code)),    /* funktionindex */
                        GETORDERARGS(help, j),     /* function label behind this order */
                        GETORDERARGS(help, n));
         break;
       case stflip:
         switch (GETORDERARGS(help, n))
         {
           case 0:
             sprintf(text, "/* no stack switch */");
             break;
           case 1:
             sprintf(text, "stflip(a, w);");
             break;
           case 2:
             sprintf(text, "stflip(r, t);");
             break;
           case 3:
             sprintf(text, "stflip(a, w);\nstflip(r, t);");
             break;
           default:
             yyfail("unknown STFLIP parameter in c_out_order_list");
          }
          break;
    default:
    {
      yywarn("%s" "unknown command in c_out_order_list"); 
      sprintf(text,"ERROR_OCCURED");
      break;
    }
      
    }
  if (WATCH && (code_ok!=GETORDERENTRY(help, command)))
  {
    fprintf(yyout,"DBUG_PRINT(\"WATCH\",(\"%s\"));\n",text);
  }
  fprintf(yyout,"%s\n",text);
  help=GETNEXTORDER(help);
  }
  DBUG_VOID_RETURN;
}







