/***********************************************************************/
/* Compiles syntaxtree built by tasm2c.y. Call from tasm2c.y.          */
/***********************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "dbug.h"
#include "mytypes.h"
#include "globals.h"
#include "buildtree.h"
#include "comp_support.h"
#include "error.h"
#include "c_output.h"
#include "optimizer.h"

#define ONE_FUNCTION_LIMIT 600

extern int FRED_FISH, WATCH, RED_CNT, ODDSEX, MODUL, DIST, TEST_STACK;
extern PROGRAM *program_start;
extern char *primf_tab[];
extern int dontremove;
extern int do_dupe;
extern char *modulfile;

void build_functable();
void build_functable_one_function();
void compile_function1();
void compile_function2();
void insert_fred_fish();
void insert_function_header();
void insert_redcnt_info();
void build_control();
int count_command(FUNCTION *);
void insert_labels_one_function(FUNCTION *);

#ifdef VCOMPILE
  char *vcompile = VCOMPILE;
#else /* VCOMPILE */
  char *vcompile = "none";
#endif /* VCOMPILE */

char *funcprot;

/*************************************************************/
/* receives a name of a function or conditional and returns  */
/* pointer to the equivalent descriptor                      */
/* can't fail                                                */
/*************************************************************/

int search_funcname(char *find_me)
{
  int i=0;

  FUNDESC *fd = GETFIRSTDESC(program_start);
  FUNDESC *help;
  FUNCTION *fn = GETFIRSTFUNCTION(program_start);
  
  while (strcmp(GETNAME(fn), find_me))
    fn = GETNEXTFUNCTION(fn);
  help = GETDESC(fn);
  while (help != fd)  
  {
    i++;
    fd = GETNEXTDESC(fd);
  }
  return(i);
}

int search_condname(char *find_me)
{
  int i=0;

  FUNDESC *fd = GETFIRSTDESC(program_start);

  while (1)
  {
    if (strcmp(GETDESCENTRY(fd, label), find_me))
    { i++; fd = GETNEXTDESC(fd); continue;}
    return(i);
  }
}

/************************************************************/
/* counts number of data entries                            */
/************************************************************/

int count_data(PROGRAM *p)
{
  int i = -1;
  DATANODE *data = GETFIRSTDATA(p);
  
  while (NULL != data) 
  {
    data = GETNEXTDATA(data);
    i++;
  }
  return(i);
}
  
/***********************************************************************/
/* initiates the compilation and controls the single compilationsteps. */
/***********************************************************************/

int compile(char *modulfile)
{
  FUNCTION *func;
  int i;
  int number_of_commands = 0;
  char *help_str;
  
  DBUG_ENTER("compile");
  if (NULL == (funcprot = (char *)malloc(L_tmpnam + 4)))
    yyfail("unable to allocate memory");
  tmpnam(funcprot);
  strcat(funcprot, ".h");
  help_str = strrchr(funcprot, '/');
  funcprot = &help_str[1];
  DBUG_PRINT("FILE", ("function prototype file : %s", funcprot));
  func=GETFIRSTFUNCTION(program_start);
  while (NULL != func)
  {
    number_of_commands += count_command(func);
    func=GETNEXTFUNCTION(func);
  }
  if ((ONE_FUNCTION_LIMIT > number_of_commands) && !MODUL && 
      (!uses_apply) && (!do_dupe) && (FALSE == RED_CNT) && (FALSE == DIST))
    fprintf(yyout,"#define ONE_FUNCTION %d\n", number_of_commands);
  if (TEST_STACK)
    fprintf(yyout,"#define TEST_STACK 1\n");
  if (DIST)
    fprintf(yyout,"#define D_SLAVE 1\n");
  if (dogcc && !DIST && !useacc)
    fprintf(yyout,"#define USES_GCC 1\n");
  if (MODUL)
  {
    if (ISASM_TILDE())
      fprintf(yyout,"extern int withtilde;\n");
    else
    {
      fprintf(yyout,"#define WITHTILDE 1\n");
      fprintf(yyout,"extern int withtilde;\n");
    }
    fprintf(yyout, "extern int _formated;\n");
    fprintf(yyout, "extern int red_cnt;\n");
/* cr 28/03/95, kir(ff), START */
    fprintf(yyout, "extern int inter_allowed;\n");
/* cr 28/03/95, kir(ff), END */
  }
  else
  {
    if (ISASM_TILDE()) 
    {
      fprintf(yyout,"int withtilde = 1;\n");
      fprintf(yyout,"#define WITHTILDE 1\n");
    }
    else
      fprintf(yyout,"int withtilde = 0;\n");
    fprintf(yyout, "int _formated = 1;\n");
    fprintf(yyout, "int red_cnt = 10000000;\n");
/* cr 28/03/95, kir(ff), START */
    fprintf(yyout, "int inter_allowed = 1;\n");
/* cr 28/03/95, kir(ff), END */
  }

  i = count_data(program_start);
  fprintf(yyout, "int %sdata_table[%d];\n", modulfile, i + 2);   /* needs to be here `cause
                                                    data_table needs to be initialized
                                                    with a value > 0 */
  fprintf(yyout, "int %snumber_of_data = %d;\n", modulfile, i + 1);
  build_control(program_start);
  fprintf(yyout, "#include \"prolog.h\"\n");
  fprintf(yyout, "#include \"%s%s%s%s\"\n",
      P_tmpdir, P_tmpdir[strlen(P_tmpdir)-1] == '/' ? "" : "/", modulfile, funcprot);

  func=GETFIRSTFUNCTION(program_start);
  if ((ONE_FUNCTION_LIMIT > number_of_commands) && !MODUL && 
      (!uses_apply) && (!do_dupe) && (FALSE == RED_CNT) && (FALSE == DIST))
  {
    yymessage("compiling all asm-functions to one C-function");
    make_one_function(program_start);

    if (strcmp(GETNAME(func),"apply"))
    {
      if (optimize > 0) optimize_delta(func);
      insert_labels_one_function(func);
      insert_fred_fish(func);
      insert_function_header(func);
      if (RED_CNT)
	insert_redcnt_info(func);
      compile_delta(func);
    }
    if (NULL!=GETFIRSTDESC(program_start))
      build_functable_one_function(GETFIRSTDESC(program_start), modulfile);
    c_out_functable(functable, TRUE);
    fprintf(yyout, "#include \"c_main.c\"\n");
    c_out_data(program_start);
    c_out_order_list(func);
    func = GETNEXTFUNCTION(func);
    while(NULL != func)
    {
      if (!strcmp(GETNAME(func),"apply"))
        c_out_order_list(func);
      func = GETNEXTFUNCTION(func);
    }
  }
  else
  {
    if (NULL!=GETFIRSTDESC(program_start))
      build_functable(GETFIRSTDESC(program_start), modulfile);
    c_out_functable(functable, TRUE);
    fprintf(yyout, "#include \"c_main.c\"\n");
    c_out_data(program_start);
    while (NULL!=func)
    {
      if (strcmp(GETNAME(func),"apply"))
      {
        if ((!RED_CNT) && (optimize > 0))
          optimize_delta(func);
	insert_labels(func);
	insert_fred_fish(func);
	insert_function_header(func);
	if (RED_CNT)
	  insert_redcnt_info(func);
	compile_delta(func);
      }
      c_out_order_list(func);
      func=GETNEXTFUNCTION(func);
    }
  }
  if (MODUL)
    c_build_func_prot(program_start, modulfile);
  else 
     c_build_func_prot(program_start, "");
  fclose(yyout);
  if (dogcc && outfile)
  {
    build_makefile(); 
    call_makefile();
    if (!dontremove)
      clean_up();       
  }
  
  DBUG_RETURN(0);
}


/**********************************************************************/
/* Looks for a function called s in global functable                  */
/* returns pointer to location of function descriptor                 */
/**********************************************************************/

FUNDESC *search_func(char *s)
{
  FUNDESC *f;
 
  DBUG_ENTER("search_func"); 
  f=GETFIRSTDESC(program_start);
  DBUG_PRINT("SEARCH", ("looking for :%s",s));
  while (NULL!=f)
  {
    DBUG_PRINT("SEARCH", ("found:%s",GETDESCENTRY(f, label)));
    
    if (!strcmp(s,GETDESCENTRY(f, label)))
    break;
    f=GETNEXTDESC(f);
  }
  if (NULL==f)
    yyfail("%s","Unknown Function !\n");
  DBUG_RETURN(f);
}

/**********************************************************************/
/* Builds controlling function for the mutual calls of the functions. */
/* Hangs it at end of the syntaxtree.                                 */
/**********************************************************************/

void build_control(PROGRAM *p)
{
  FUNCTION *f;
  ORDER *os,*oe,*ohelp;

  DBUG_ENTER("build_control");
/* make a new function called apply */
  f=GETFIRSTFUNCTION(p);
  while (NULL!=GETNEXTFUNCTION(f)) f=GETNEXTFUNCTION(f);
  SETNEXTFUNCTION(f)=(FUNCTION *)malloc(sizeof(FUNCTION));
  if (NULL==GETNEXTFUNCTION(f))
    yyfail("%s","Couldn't allocate memory !\n");
  f=GETNEXTFUNCTION(f);
  SETNEXTFUNCTION(f)=NULL;
  if (NULL == (SETNAME(f) = (char *)malloc(strlen("apply") + 1)))
    yyfail("memory allocation failure");
  strcpy(SETNAME(f),"apply");

/* first the header */  
  os=new_order(code_ok,"int apply(int (*start)())");
/* chain first order to function */
  SETFIRSTORDER(f)=os;
  
/* then ending brace */  
  oe=new_order(code_ok,"}");
/* chain end to orderlist */
  SETNEXTORDER(os)=oe;
  SETPREVORDER(oe)=os;
/* insert opening brace */
  os=new_order(code_ok,"{");
  insert_order_list(os,oe);
  
/* now inserting the code "while (NULL!=start) start=(int(*)())(*start)();"*/
  if (DIST)
  {
    ohelp=new_order(code_ok,"while (NULL!=start)"
                            " {\n  if (sig_msg_flag) msg_check();\n"
                            "  start=(int(*)())(*start)();\n}");
  }
  else
  {
    ohelp=new_order(code_ok,"while (NULL!=start) start=(int(*)())(*start)();");
  }
  insert_order_list(ohelp,oe);

  ohelp=new_order(code_ok, "return proc_retval;");
  insert_order_list(ohelp,oe);

  DBUG_VOID_RETURN;
}
 

/**********************************************************************/
/* Inserts Fred-Fish-header at the beginning and end of each function */
/* in the syntaxtree.                                                 */
/* Also sets braces at the beginning and the end of the function.     */
/**********************************************************************/

void insert_fred_fish(FUNCTION *f)
{
  ORDER *o, *ohelp;
  char *s;
  
  char text[120];
  
  DBUG_ENTER("insert_fred_fish");

  sprintf(text,"DBUG_ENTER(\"%s\");",GETNAME(f));
  
  s=(char *)malloc(strlen(text)+1);
  if (NULL==s)
    yyfail("%s","Couldn't allocate memory !\n");
  strcpy(s,text);
  
  o=new_order(code_ok,s);  
  
  insert_order_list(o,GETFIRSTORDER(f));
  SETFIRSTORDER(f)=o;
  
  /* inserting DBUG_RETURN at end of function code */
  ohelp=new_order(code_ok,"DBUG_RETURN(0);");
  /* only dummy Return value to satisfy fred */

  while (NULL!=GETNEXTORDER(o)) o=GETNEXTORDER(o);

  SETNEXTORDER(o)=ohelp;
  SETPREVORDER(ohelp)=o;
  DBUG_VOID_RETURN;
}  



/**********************************************************************/
/* Inserts function_header at the beginning of each function in the   */
/* syntaxtree.                                                        */
/* Also sets braces at the beginning and the end of the function.     */
/**********************************************************************/

void insert_function_header(FUNCTION *f)
{
  ORDER *o, *ohelp;
  char text[120];
  char *s;

  sprintf(text,"int %s(void)",GETNAME(f));
  s=(char *)malloc(strlen(text)+1);
  if (NULL==s)
    yyfail("%s","Couldn't allocate memory !\n");
  strcpy(s,text);
  
  o=new_order(code_ok,s);
  insert_order_list(o,GETFIRSTORDER(f));
  SETFIRSTORDER(f)=o;
  if (optimize > 0)
    o=new_order(code_ok,"{ int t;");
  else
    o=new_order(code_ok,"{");
  insert_order_list(o,GETNEXTORDER(GETFIRSTORDER(f)));

/* inserting closing else brace at end of function code */
  ohelp=new_order(code_ok,"}");
  while (NULL!=GETNEXTORDER(o)) o=GETNEXTORDER(o);
  SETNEXTORDER(o)=ohelp;
  SETPREVORDER(ohelp)=o;
}  

/**********************************************************************/
/* Inserts redcounttest before each deltaX, gammabeta, beta and ap    */
/* and builds code for closure building while runtime. Inserts the    */
/* built code into the syntaxtree                                     */
/**********************************************************************/

void insert_redcnt_info(FUNCTION *f)
{
  ORDER *o, *ohelp;
  FUNDESC *fd;
  char text[120];
  char t_red[]="if (0==red_cnt) ";
  int change;
  
  o=GETFIRSTORDER(f);
  while (NULL!=o)
  {
    change=0;
    switch(GETORDERENTRY(o, command))
    {
    case delta1:
    {
      change=1;
      sprintf(text,"%s\nf_mkdclos(-1, 1, 1,%s);\nelse", \
              t_red,primf_tab[GETPRF(GETORDERARGS(o, primf))]);
      break;
    }
    case delta2:
    {
      change=1;
      sprintf(text,"%s\nf_mkdclos(-1, 2, 2,%s);\nelse", \
              t_red,primf_tab[GETPRF(GETORDERARGS(o, primf))]);
      break;
    }
     case delta3:
    {
      change=1;
      sprintf(text,"%s\nf_mkdclos(-1, 3, 3,%s);\nelse", \
              t_red,primf_tab[GETPRF(GETORDERARGS(o, primf))]);
      break;
    }
     case delta4:
    {
      change=1;
      sprintf(text,"%s\nf_mkdclos(-1, 4, 4,%s);\nelse", \
              t_red,primf_tab[GETPRF(GETORDERARGS(o, primf))]);
      break;
    }
     case betanear:
     case betafar:
    {
      change=1;
      fd=search_func(GETORDERARGS(o, label));
      sprintf(text,"%s\npush(f_mkbclos(%d, %sfuncdesctable[%d]));\nelse", t_red, \
              GETDESCENTRY(fd, nv), modulfile, search_funcname(GETDESCENTRY(fd, label)));
      break;
    }
     case Case:
    {
      change=1;
      fd=search_func(GETORDERARGS(o, label));
      sprintf(text,"%s\nf_mkcase(%d, %d, %sfuncdesctable);\nelse", t_red, \
              GETDESCENTRY(fd, nv), search_funcname(GETDESCENTRY(fd, label)), modulfile);
      break;
    }
     case tailnear:
     case tailfar:
    {
      change=1;
      fd=search_func(GETORDERARGS(o, label));
      sprintf(text,"%s\npush(f_mkbclos(%d, %sfuncdesctable[%d]));\nelse", t_red, \
              GETDESCENTRY(fd, nv), modulfile, search_funcname(GETDESCENTRY(fd, label)));
      break;
    }
     case gammabetanear:
     case gammabetafar:
    {
      change=1;
      fd=search_func(GETORDERARGS(o, label));
      sprintf(text,"%s\nf_mkgaclos(%d, %sfuncdesctable[%d], 2, 0);\nelse", \
              t_red, GETDESCENTRY(fd, nv), 
              modulfile, search_funcname(GETDESCENTRY(fd, label)));
      break;
    }
/* apply will build closures itself if red_cont == 0 *****     
     case apply:
    {
      change=1;
      sprintf(text,"%s\nf_mkclos(%d);\nelse", \
              t_red,GETORDERARGS(o, n));
      break;
    }
*/
     case jcond:
     case jfalse:
    {
      change=1;
      sprintf(text,"j%s",GETORDERARGS(o, label));
      fd=search_func(text);
      sprintf(text,"%s\n { f_condfail(%d,%sfuncdesctable[%d]);rtf();\n}\nelse", t_red, \
              GETDESCENTRY(fd, nv), modulfile, search_condname(GETDESCENTRY(fd, label)));
      break;
    }
     case jcond2:
    {
      change=1;
      sprintf(text,"j%s",GETORDERARGS(o, label));
      fd=search_func(text);
      sprintf(text,"%s {\nf_condfail(1,%sfuncdesctable[%d]);goto %s;}\nelse", t_red, \
              modulfile, search_condname(GETDESCENTRY(fd, label)), 
              GETORDERARGS(o, ret));
      break;
    }
     default:
      ;
    } /* switch */

    if (1==change)
    {
      char *s;
      
/* inserting redcount test code before application code */
      s=(char *)malloc(strlen(text)+1);
      if (NULL==s)
        yyfail("%s","Couldn't allocate memory !\n");
      strcpy(s,text);
      ohelp=new_order(code_ok,s);
      insert_order_list(ohelp, o);
/* inserting opening else brace before the application code */
      ohelp=new_order(code_ok,"{ reduce_red_cnt();\n");
      insert_order_list(ohelp, o);
/* inserting closing else brace after the application code */
      ohelp=new_order(code_ok,"}");
      insert_order_list(ohelp, GETNEXTORDER(o));
    }
    o=GETNEXTORDER(o);
  } /* while */  
}




/***********************************************************************/
/* When a function is compiled it will be transferred to structure     */
/* in which the C-code will be written. Also used by build_functable() */
/***********************************************************************/

void add_code_node(CODE *ftab, char *fmt, ...)
{
  va_list ap;
  CODE *help;
  
  DBUG_ENTER("add_code_node");
  while (NULL!=GETNEXTORDER(ftab)) ftab=GETNEXTORDER(ftab);
  
  va_start(ap, fmt);
  
  help=(CODE *)malloc(sizeof(CODE));
  if (NULL==help)
    yyfail("%s","Couldn't allocate memory !\n");
  vsprintf(GETORDERENTRY(help, code), fmt, ap);
  SETNEXTORDER(help)=NULL;
  SETNEXTORDER(ftab)=help;
  
  va_end(ap);
  DBUG_VOID_RETURN;
}  
  
/***********************************************************************/
/* Builds a functiontable for the C-code, so that while runtime a      */
/* function descriptor can be found. Uses global variable functable    */
/* from GLOBALS.H.                                                     */
/***********************************************************************/
char *findfunc(int i)
{
  FUNDESC *d = GETFIRSTDESC(program_start);

  while (NULL != d)
  {
    if (i == GETDESCENTRY(d, address))
      return(GETDESCENTRY(d, label));
    d = GETNEXTDESC(d);
  }
  return("0");
}

char *findcase(int i)
{
  FUNDESC *d = GETFIRSTDESC(program_start);

  while (NULL != d)
  {
    if (i == GETDESCENTRY(d, graph))
      return(GETDESCENTRY(d, label));
    d = GETNEXTDESC(d);
  }
  return("0");
}

void build_functable(FUNDESC *d, char *modulfile)
{

  char text[80];
  
  DBUG_ENTER("build_functable");
  
  strcpy(GETDESCENTRY(d, label),"goal");

  sprintf(text, "T_DESCRIPTOR %sstat_funcdesctable[]={", modulfile);

  add_code_node(functable, text);
  while (NULL!=d)
  {
    if (DT_CONDITIONAL==GETDESCENTRY(d, tag))
    {
      char *s;
      s = findfunc((GETDESCENTRY(d, address))+1);
      if (!strcmp(s, "0"))
        strcpy(text, "0");
      else
        sprintf(text, "(INSTR *)&%s", s); 
/*       sprintf(text,"(INSTR *)&%s",findfunc((GETDESCENTRY(d, address))+1));*/
/*      if (!strcmp(text, "(INSTR *)&0"))*/
/*        strcpy(text, "0");*/
    }
    
    if (DT_COMBINATOR == GETDESCENTRY(d, tag))
    {
      if (ODDSEX)
      {
        add_code_node(functable,"\t{TY_COMB, C_FUNC, 1,");
        add_code_node(functable,"\t%d, %d, &%s, %d }", \
                      GETDESCENTRY(d, nv), GETDESCENTRY(d, nfv), 
                      GETDESCENTRY(d, label), GETDESCENTRY(d, graph));
      }
      else
      {
        add_code_node(functable,"\t{1, C_FUNC, TY_COMB,");
        add_code_node(functable,"\t%d, %d, (INSTR *)&%s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nfv), GETDESCENTRY(d, nv), 
                      GETDESCENTRY(d, label), GETDESCENTRY(d, graph));
      }
    }
    else
    if (DT_CASE == GETDESCENTRY(d, tag))
    {
      if (ODDSEX)
      {
        add_code_node(functable,"\t{TY_CASE, C_FUNC, 1,");
        if (!strncmp(GETDESCENTRY(d, label), "when_", 4))
          add_code_node(functable,"\t%d, %d, (INSTR *)TAG_INT(%d), (T_PTD)%d }", \
                      GETDESCENTRY(d, nv), GETDESCENTRY(d, nfv),
                      atoi(&GETDESCENTRY(d, label)[5]), GETDESCENTRY(d, graph));
        else
          add_code_node(functable,"\t%d, %d, (INSTR *)&%s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nv), GETDESCENTRY(d, nfv),
                      GETDESCENTRY(d, label), GETDESCENTRY(d, graph));
      }
      else
      {
        add_code_node(functable,"\t{1, C_FUNC, TY_CASE,");
        if (!strncmp(GETDESCENTRY(d, label), "when_", 4))
          add_code_node(functable,"\t%d, %d, (INSTR *)TAG_INT(%d), (T_PTD)%d }", \
                      GETDESCENTRY(d, nfv), GETDESCENTRY(d, nv),
                      atoi(&GETDESCENTRY(d, label)[5]), GETDESCENTRY(d, graph));
        else
          add_code_node(functable,"\t%d, %d, (INSTR *)&%s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nfv), GETDESCENTRY(d, nv),
                      GETDESCENTRY(d, label), GETDESCENTRY(d, graph));
      }
    }
    else
    if (DT_CONDITIONAL == GETDESCENTRY(d, tag))
    {
      if (ODDSEX)
      {
        add_code_node(functable,"\t{TY_CONDI, C_FUNC, 1,");
        add_code_node(functable,"\t%d, %d, %s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nv), GETDESCENTRY(d, nfv), 
                      text, GETDESCENTRY(d, graph));
      }
      else
      {
        add_code_node(functable,"\t{1, C_FUNC, TY_CONDI,");
        add_code_node(functable,"\t%d, %d, %s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nfv), GETDESCENTRY(d, nv), 
                      text, GETDESCENTRY(d, graph));
      }
    }
    d=GETNEXTDESC(d);
    if (NULL != d) add_code_node(functable,",");
    else add_code_node(functable,"};");
  }
  DBUG_VOID_RETURN;
}



void build_functable_one_function(FUNDESC *d, char *modulfile)
{

  char text[80];
  
  DBUG_ENTER("build_functable_one_function");
  
  strcpy(GETDESCENTRY(d, label),"0");

  sprintf(text, "T_DESCRIPTOR %sstat_funcdesctable[]={", modulfile);

  add_code_node(functable, text);
  while (NULL!=d)
  {
    if (DT_CONDITIONAL==GETDESCENTRY(d, tag))
    {
      char *s;
      s = findfunc((GETDESCENTRY(d, address))+1);
      if (!strcmp(s, "0"))
        strcpy(text, "0");
      else
        sprintf(text, "(INSTR *)%s", s); 
/*       sprintf(text,"(INSTR *)&%s",findfunc((GETDESCENTRY(d, address))+1));*/
/*      if (!strcmp(text, "(INSTR *)&0"))*/
/*        strcpy(text, "0");*/
    }
    
    if (DT_COMBINATOR == GETDESCENTRY(d, tag))
    {
      if (ODDSEX)
      {
        add_code_node(functable,"\t{TY_COMB, C_FUNC, 1,");
        add_code_node(functable,"\t%d, %d, (INSTR *)%s, %d }", \
                      GETDESCENTRY(d, nv), GETDESCENTRY(d, nfv), 
                      GETDESCENTRY(d, label), GETDESCENTRY(d, graph));
      }
      else
      {
        add_code_node(functable,"\t{1, C_FUNC, TY_COMB,");
        add_code_node(functable,"\t%d, %d, (INSTR *)%s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nfv), GETDESCENTRY(d, nv), 
                      GETDESCENTRY(d, label), GETDESCENTRY(d, graph));
      }
    }
    else
    if (DT_CASE == GETDESCENTRY(d, tag))
    {
      if (ODDSEX)
      {
        add_code_node(functable,"\t{TY_CASE, C_FUNC, 1,");
        if (!strncmp(GETDESCENTRY(d, label), "when_", 4))
          add_code_node(functable,"\t%d, %d, (INSTR *)TAG_INT(%d), (T_PTD)%d }", \
                      GETDESCENTRY(d, nv), GETDESCENTRY(d, nfv),
                      atoi(&GETDESCENTRY(d, label)[5]), GETDESCENTRY(d, graph));
        else
          add_code_node(functable,"\t%d, %d, (INSTR *)&%s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nv), GETDESCENTRY(d, nfv),
                      GETDESCENTRY(d, label), GETDESCENTRY(d, graph));
      }
      else
      {
        add_code_node(functable,"\t{1, C_FUNC, TY_CASE,");
        if (!strncmp(GETDESCENTRY(d, label), "when_", 4))
          add_code_node(functable,"\t%d, %d, (INSTR *)TAG_INT(%d), (T_PTD)%d }", \
                      GETDESCENTRY(d, nfv), GETDESCENTRY(d, nv),
                      atoi(&GETDESCENTRY(d, label)[5]), GETDESCENTRY(d, graph));
        else
          add_code_node(functable,"\t%d, %d, (INSTR *)%s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nfv), GETDESCENTRY(d, nv),
                      GETDESCENTRY(d, label), GETDESCENTRY(d, graph));
      }
    }
    else
    if (DT_CONDITIONAL == GETDESCENTRY(d, tag))
    {
      if (ODDSEX)
      {
        add_code_node(functable,"\t{TY_CONDI, C_FUNC, 1,");
        add_code_node(functable,"\t%d, %d, %s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nv), GETDESCENTRY(d, nfv), 
                      text, GETDESCENTRY(d, graph));
      }
      else
      {
        add_code_node(functable,"\t{1, C_FUNC, TY_CONDI,");
        add_code_node(functable,"\t%d, %d, %s, (T_PTD)%d }", \
                      GETDESCENTRY(d, nfv), GETDESCENTRY(d, nv), 
                      text, GETDESCENTRY(d, graph));
      }
    }
    d=GETNEXTDESC(d);
    if (NULL != d) add_code_node(functable,",");
    else add_code_node(functable,"};");
  }
  DBUG_VOID_RETURN;
}
