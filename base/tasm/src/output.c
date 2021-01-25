#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mytypes.h"
#include "buildtree.h"
#include "loctypes.h"
#include "main.h"
#include "tiny.h"
#include "dbug.h"
#include "error.h"

extern char *primf_tab[];
extern char *intact_tab[];
extern int optimize; /* from main.c */
int goaladdr;

/************************************************************************/
/* Returns string of primitiv function name				*/
/* (used by out_order_list)						*/
/************************************************************************/
static char *out_primf(PRIMF n)
  {
  static char s[20];
  int i;

  DBUG_ENTER("out_primf");
  strcpy(s, primf_tab[n]);
  if (!strcmp(s,"FR_UPDATE"))
    DBUG_RETURN("fupdate");
  if (!strcmp(s,"FR_SELECT"))
    DBUG_RETURN("fselect");
  if (!strcmp(s,"FR_DELETE"))
    DBUG_RETURN("fdelete");
  if (!strcmp(s,"FR_SLOTS"))
    DBUG_RETURN("fslots");
  if (!strcmp(s,"FR_TEST"))
    DBUG_RETURN("ftest");
  if (!strcmp(s,"D_GREAT"))
    DBUG_RETURN("gt");
  if (!strcmp(s,"D_LEQ"))
    DBUG_RETURN("le");
  if (!strcmp(s,"D_LESS"))
    DBUG_RETURN("lt");
  if (!strcmp(s,"M_TRUNCATE"))
    DBUG_RETURN("trunc");
  if (!strcmp(s,"LDIMENSION"))
    DBUG_RETURN("ldim");
  if (!strcmp(s, "UNI") || !strcmp(&s[1], "UNI"))
      strcat(s,"TE");
  if ('_'== s[1] && (('D' == s[0]) || ('M' == s[0]))) {
    for (i=2; i<=strlen(s); i++)
      s[i] = tolower(s[i]);
    DBUG_RETURN(&s[2]);
    }
  else
    for (i=0; i<=strlen(s); i++)
      s[i] = tolower(s[i]);
  DBUG_RETURN(s);
  }

/************************************************************************/
/* Returns string of interaction name                                   */
/* (used by out_order_list)                                             */
/************************************************************************/
static char *out_ia(IACTION n)
  {
  static char s[20];
  int i;

  DBUG_ENTER("out_ia");
  strcpy(s, intact_tab[n]);
  for (i=0; i<=strlen(s); i++)
    s[i] = tolower(s[i]);
  DBUG_RETURN(s);
  }


static char * prselem(int x)
  {
  static char s[4];
  DBUG_ENTER("prselem");
  if (ISPRF(x))
    DBUG_RETURN(out_primf((PRIMF)GETPRF(x)));
  else if (ISIA(x))
    DBUG_RETURN(out_ia((IACTION)GETIA(x)));
  else if (ISCHAR(x))
    sprintf(s, "'%c'", (char)GETCHAR(x));
  else
    yyfail("unknown stack element in output");
  DBUG_RETURN(s);
  }

/* returns the ascii form of interaction types
 */
static char * out_intertype(INTERT n)
  {
  DBUG_ENTER("out_intertype");
  switch(n) {
    case INdone: DBUG_RETURN("done");
    case INget:  DBUG_RETURN("get");
    case INput:  DBUG_RETURN("put");
    default: yyfail("illegal interaction type received");
    }
  DBUG_RETURN((char *)0);
  }

/************************************************************************/
/* Prints out ready-made functionsdescriptors. 				*/
/************************************************************************/

void out_functable(CODE *f)
{
DBUG_ENTER("out_functable");
while (NULL!=f)
  {
  fprintf(yyout,"%s\n",f->code);
  f=f->next;
  }
DBUG_VOID_RETURN;
}


/***********************************************************************/
/* Prints out the syntaxtree using out_order_list()                    */
/* (to control the parsing).                                           */
/***********************************************************************/

void out_tree(PROGRAM * out_me)
{
void out_desc_list(FUNDESC *);
void out_data_list(DATANODE *);
void out_order_list(ORDER *);

FUNCTION *fhelp;
ORDER *ohelp; 

DBUG_ENTER("out_tree");
goaladdr=atox(searchfor_(GETDESCENTRY(GETFIRSTDESC(program_start), label)));
fprintf(yyout, "/* code processed by %s version %s */\n", program, version);
if (0 == optimize)
{
  out_desc_list(out_me->desc);
  out_data_list(out_me->data);
}
fhelp=out_me->function;
while (NULL!=fhelp)
  {
  if (strncmp(fhelp->name,"goal",4)==0)
    fprintf(yyout,"start_%x:\n",goaladdr);
  else
    fprintf(yyout,"%s:\n", fhelp->name);
  ohelp=fhelp->order;
  out_order_list(ohelp);
  fhelp=fhelp->next;
  }
DBUG_VOID_RETURN;
}

/***********************************************************************/
/* Prints out a list of descriptors of functions                       */
/* (used by out_tree()).                                               */
/***********************************************************************/

void out_desc_list(FUNDESC * desc)
{
FUNDESC *help;

DBUG_ENTER("out_desc_list");
help=desc;
while (NULL!=help)
  {
  fprintf(yyout, "\t_desc(");
  fprintf(yyout, "0x%08x,FUNC,", help->address);
  switch (help->tag)
    {
    case DT_COMBINATOR:
      fprintf(yyout, "COMB,");
      break;
    case DT_CONDITIONAL:
      fprintf(yyout, "CONDI,");
      break;
    case DT_CASE:
      fprintf(yyout, "CASE,");
      break;
    case DT_PRIMITIV:
      fprintf(yyout, "PRIM,");
      break;
    }
  fprintf(yyout, "%d,%d,",help->nfv, help->nv);
  fprintf(yyout, "0x%08x,", help->graph);
  fprintf(yyout, "%s);\n", help->label);
  help=help->next;
  }
DBUG_VOID_RETURN;
} 


/***********************************************************************/
/* Prints out a list of data descriptors                               */
/* (used by out_tree()).                                               */
/***********************************************************************/

void out_data_list(DATANODE * desc)
  {
  int i, x, copied = 0;
  char text[10];
  DATANODE *help;

  DBUG_ENTER("out_data_list");
  help=desc;
  while (NULL!=help)
    {
    fprintf(yyout, "\t");
    switch (help->tag)
      {
      case d_mat:
        if (!copied) strcpy(text, "_mat");
        copied = 1;
      case d_tvect:
        if (!copied) strcpy(text, "_tvect");
        copied = 1;
      case d_vect:
        if (!copied) strcpy(text, "_vect");
        copied = 1;
        switch(GETDATAENTRY(help, u.w.tag))
        {
          case d_bool:
            fprintf(yyout, "%s(0x%08x, TY_BOOL, %d, %d", text, GETDATAENTRY(help,address),
                                GETDATAENTRY(help, u.w.rows),
                                GETDATAENTRY(help, u.w.cols));
            for(i = 0; i < GETDATAENTRY(help, u.w.rows)*GETDATAENTRY(help, u.w.cols); i++)
              fprintf(yyout, ", %s", 
                        GETDATAENTRY(help, u.w.m.idata)[i] ? "true" : "false");
            fprintf(yyout, ");\n");
          break;
          case d_int:
            fprintf(yyout, "%s(0x%08x, TY_INTEGER, %d, %d", text, 
                                GETDATAENTRY(help,address),
                                GETDATAENTRY(help, u.w.rows),
                                GETDATAENTRY(help, u.w.cols));
            for(i = 0; i < GETDATAENTRY(help, u.w.rows)*GETDATAENTRY(help, u.w.cols); i++)
              fprintf(yyout, ", %d", GETDATAENTRY(help, u.w.m.idata)[i]);
            fprintf(yyout, ");\n");
            break;
          case d_real:
            fprintf(yyout, "%s(0x%08x, TY_REAL, %d, %d", text, GETDATAENTRY(help,address),
                                GETDATAENTRY(help, u.w.rows),
                                GETDATAENTRY(help, u.w.cols));
            for(i = 0; i < GETDATAENTRY(help, u.w.rows)*GETDATAENTRY(help, u.w.cols); i++)
              fprintf(yyout, ", %+.15g", GETDATAENTRY(help, u.w.m.rdata)[i]);
            fprintf(yyout, ");\n");
            break;
          case d_undef:
            fprintf(yyout, "%s(0x%08x, TY_UNDEF, %d, %d", text, 
                                GETDATAENTRY(help,address),
                                GETDATAENTRY(help, u.w.rows),
                                GETDATAENTRY(help, u.w.cols));
            for(i = 0; i < GETDATAENTRY(help, u.w.rows)*GETDATAENTRY(help, u.w.cols); i++)
              fprintf(yyout, ", %d", GETDATAENTRY(help, u.w.m.idata)[i]);
            fprintf(yyout, ");\n");
            break;
          default:
            fprintf(yyout, "<unknown mat/vect/tvect type>\n");
        }
        copied = 0;
        break;
      case d_real:
        fprintf(yyout, "_real(0x%08x,%+.15g);\n", (int)help->address, help->u.x);
        break;
      case d_list:
        fprintf(yyout, "_list(0x%08x,%d", (int)help->address, help->u.v.size);
        for(i = 0; i < help->u.v.size; i++) {
          x = help->u.v.data[i];
          if (ISINT(x))
            fprintf(yyout, ",int(%d)", GETINT(x));
          else if (ISBOOL(x))
            fprintf(yyout, ",bool(%s)", (GETBOOL(x) ? "true" : "false"));
          else if (ISPRF(x))
            fprintf(yyout, ",%s",out_primf(GETPRF(x)));
          else if (ISIA(x))
            fprintf(yyout, ",%s",out_ia(GETIA(x)));
          else if (ISPTR(x))
            fprintf(yyout, ",ptd(0x%08x)", GETPTR(x));
          else 
            yyfail("internal error: illegal list elementi %d", x);
          }
        fprintf(yyout, ");\n");
        break;
      case d_string:
        fprintf(yyout, "_str(0x%08x,%d", (int)help->address, help->u.v.size);
        for(i = 0; i < help->u.v.size; i++) {
          x = help->u.v.data[i];
          if (ISCHAR(x))
            fprintf(yyout, ",'%c'", GETCHAR(x));
          else if (ISPTR(x))
            fprintf(yyout, ",ptd(0x%08x)", GETPTR(x));
          else 
            yyfail("internal error: illegal string elementi %d", x);
          }
        fprintf(yyout, ");\n");
        break;
      case d_name:
        fprintf(yyout, "_name(0x%08x,%d,\"%s\" );\n",
            (int)help->address, help->u.v.size, (char *)help->u.v.data);
        break;
      default:
        fprintf(yyout, "<unknown>\n");
      }
    help=help->next;
    }
  DBUG_VOID_RETURN;
  } 



/***********************************************************************/
/* Prints out a list of ASM~-commands (i.e. a whole function)          */
/* (used by out_tree()).                                               */
/***********************************************************************/

void out_order_list(ORDER *out)
{
ORDER *help;

DBUG_ENTER("out_order_list");
help=out;
while (NULL!=help)
  {
  switch(help->command)
    {
    case pushcw_pf:
      if (ISIA(help->args.n))
        fprintf(yyout,"\tpushc_w(%s);\n",out_ia(GETIA(help->args.n)));
      else
        fprintf(yyout,"\tpushc_w(%s);\n",out_primf(GETPRF(help->args.n)));
      break;
    case pushcw_i: 
      fprintf(yyout,"\tpushc_w(int(%d));\n",GETINT(help->args.n));
      break;
    case pushcw_b:
      fprintf(yyout,"\tpushc_w(bool(%s));\n",(MKBOOL(0)==help->args.n) ? "false" : "true");
      break;
    case pushaw:
      fprintf(yyout,"\tpush_aw(%d);\n",help->args.n);
      break;
    case pushr_p:
      fprintf(yyout,"\tpush_r(ptd(0x%08x));\n",GETPTR((unsigned int)help->args.desc));
      break;
    case pushtw:
      fprintf(yyout,"\tpush_tw(%d);\n",help->args.n);
      break;
    case pushar:
      fprintf(yyout,"\tpush_ar(%d);\n",help->args.n);
      break;
    case pushtr:
      fprintf(yyout,"\tpush_tr(%d);\n",help->args.n);
      break;
    case pushw_p:
      fprintf(yyout,"\tpush_w(ptd(0x%08x));\n",GETPTR((unsigned int)help->args.desc));
      break;
    case pushret:
      fprintf(yyout,"\tpushret(%s);\n",help->args.label);
      break;
    case movear:
      fprintf(yyout,"\tmove_ar();\n");
      break;
    case moveaw:
      fprintf(yyout,"\tmove_aw();\n");
      break;
    case movetr:
      fprintf(yyout,"\tmove_tr();\n");
      break;
    case movetw:
      fprintf(yyout,"\tmove_tw();\n");
      break;
    case pushaux:
      fprintf(yyout,"\tpushaux();\n");
      break;
    case delta1:
      if (help->types == NOTYPE)
        fprintf(yyout,"\tdelta1(%s);\n",out_primf(GETPRF(help->args.primf)));
      else
        fprintf(yyout,"\t%s.%s();\n",out_primf(GETPRF(help->args.primf)),
                                      short_type_names(GETTYPE(0, help->types)));
      break;
    case delta2:
      if (help->types == NOTYPE)
        fprintf(yyout,"\tdelta2(%s);\n",out_primf(GETPRF(help->args.primf)));
      else
        fprintf(yyout,"\t%s.%s%s();\n",out_primf(GETPRF(help->args.primf)),
                                       short_type_names(GETTYPE(0, help->types)),
                                       short_type_names(GETTYPE(1, help->types)));
      break;
    case delta3:
      if (help->types == NOTYPE)
        fprintf(yyout,"\tdelta3(%s);\n",out_primf(GETPRF(help->args.primf)));
      else
        fprintf(yyout,"\t%s.%s%s%s();\n",out_primf(GETPRF(help->args.primf)),
                                         short_type_names(GETTYPE(0, help->types)),
                                         short_type_names(GETTYPE(1, help->types)),
                                         short_type_names(GETTYPE(2, help->types)));
      break;
    case delta4:
      if (help->types == NOTYPE)
        fprintf(yyout,"\tdelta4(%s);\n",out_primf(GETPRF(help->args.primf)));
      else
        fprintf(yyout,"\t%s.%s%s%s%s();\n",out_primf(GETPRF(help->args.primf)),
                                           short_type_names(GETTYPE(0, help->types)),
                                           short_type_names(GETTYPE(1, help->types)),
                                           short_type_names(GETTYPE(2, help->types)),
                                           short_type_names(GETTYPE(3, help->types)));
      break;
    case intact:
      fprintf(yyout,"\tintact(%s);\n",out_ia(GETIA(help->args.primf)));
      break;
    case freeswt:
      fprintf(yyout,"\tfreesw_t(%d);\n",help->args.n);
      break;
    case freer:
      fprintf(yyout,"\tfree_r(%d);\n",help->args.n);
      break;
    case freea:
      fprintf(yyout,"\tfree_a(%d);\n",help->args.n);
      break;
    case freet:
      fprintf(yyout,"\tfree_t(%d);\n",help->args.n);
      break;
    case freew:
      fprintf(yyout,"\tfree_w(%d);\n",help->args.n);
      break;
    case mkilist:
      fprintf(yyout,"\tmkilist(%d);\n",help->args.n);
      break;
    case mklist:
      fprintf(yyout,"\tmklist(%d);\n",help->args.n);
      break;
    case mkap:
      fprintf(yyout,"\tmkap(%d);\n",help->args.n);
      break;
    case apply:
      fprintf(yyout,"\tapply(%d);\n",help->args.n);
      break;
    case rtf:
      fprintf(yyout,"\trtf();\n");
      break;
    case rtm:
      fprintf(yyout,"\trtm();\n");
      break;
    case rtc_i:
      fprintf(yyout,"\trtc(int(%d));\n",GETINT(help->args.n));
      break;
    case rtc_b:
      fprintf(yyout,"\trtc(bool(%s));\n",(4==help->args.n) ? "false" : "true");
      break;
    case rtc_pf:
      if (ISIA(help->args.n))
        fprintf(yyout,"\trtc(%s);\n",out_ia(GETIA(help->args.n)));
      else
        fprintf(yyout,"\trtc(%s);\n",out_primf(GETPRF(help->args.n)));
      break;
    case rtp:
      fprintf(yyout,"\trtp(ptd(0x%08x));\n", GETPTR((int)help->args.desc));
      break;
    case rtt:
      fprintf(yyout,"\trtt();\n");
      break;
    case mkdclos: 
      fprintf(yyout,"\tmkdclos(%d,%d);\n",help->args.n, help->args.m);
      break;
    case mkgaclos:
      fprintf(yyout,"\tmkgaclos(%d,%d,%d);\n",help->args.n, \
              help->args.m,help->args.k);
      break;
    case mksclos:
      fprintf(yyout,"\tmksclos(%d,%d,%d);\n",help->args.n, \
              help->args.m,help->args.k);
      break;
    case mkgsclos:
      fprintf(yyout,"\tmkgsclos(%d,%d,%d);\n",help->args.n, \
              help->args.m,help->args.k);
      break;
    case mkbclos:
      if (ISASM_TILDE())
        fprintf(yyout,"\tmkbclos(%d,%d,%d);\n",help->args.n, \
                help->args.m,help->args.k);
      else
        fprintf(yyout,"\tmkbclos(%d,%d);\n",help->args.n, help->args.m);
      break;
    case mkcclos:
      if (ISASM_TILDE())
        fprintf(yyout,"\tmkcclos(%d,%d,%d);\n",help->args.n, \
                help->args.m,help->args.k);
      else
        fprintf(yyout,"\tmkcclos(%d,%d);\n",help->args.n, help->args.m);
      break;
    case mkiclos:
      fprintf(yyout,"\tmkiclos(%d,%d);\n",help->args.n, help->args.m);
      break;
    case jump:
      fprintf(yyout,"\tjump(%s);\n",help->args.label);
      break;
    case jfalse:
      if (tp_bool==help->types)
	fprintf(yyout,"\tjfalse_b(%s);\n",help->args.label);
      else  
	fprintf(yyout,"\tjfalse(%s);\n",help->args.label);
      break;
    case jtrue:
      if (tp_bool==help->types)
	fprintf(yyout,"\tjtrue_b(%s);\n",help->args.label);
      else
	fprintf(yyout,"\tjtrue(%s);\n",help->args.label);
      break;
    case jcond:
      if (tp_bool==help->types)
	fprintf(yyout,"\tjcond_b(%s);\n",help->args.label);
      else
	fprintf(yyout,"\tjcond(%s);\n",help->args.label);
      break;
    case jfalse2:
      if (tp_bool==help->types)
	fprintf(yyout,"\tjfalse2_b(%s,%s);\n",help->args.label,help->args.ret);
      else
	fprintf(yyout,"\tjfalse2(%s,%s);\n",help->args.label,help->args.ret);
      break;
    case jtrue2:
      if (tp_bool==help->types)
	fprintf(yyout,"\tjtrue2_b(%s,%s);\n",help->args.label,help->args.ret);
      else
	fprintf(yyout,"\tjtrue2(%s,%s);\n",help->args.label,help->args.ret);
      break;
    case jcond2:
      if (tp_bool==help->types)
	fprintf(yyout,"\tjcond2_b(%s,%s);\n",help->args.label,help->args.ret);
      else
	fprintf(yyout,"\tjcond2(%s,%s);\n",help->args.label,help->args.ret);
      break;
    case beta:
      fprintf(yyout,"\tbeta(%s);\n",help->args.label);
      break;
    case Gamma:
      fprintf(yyout,"\tgamma(%s);\n",help->args.label);
      break;
    case gammabeta:
      fprintf(yyout,"\tgammabeta(%s);\n",help->args.label);
      break;
    case tail:
      fprintf(yyout,"\ttail(%s);\n",help->args.label);
      break;
    case Case:
      fprintf(yyout,"\tcase(%s);\n",help->args.label);
      break;
    case gammacase:
      fprintf(yyout,"\tgammacase(%s);\n",help->args.label);
      break;
    case label:
      fprintf(yyout,"%s:\n",help->args.label);
      break;
    case ext:
      fprintf(yyout,"\text();\n");
      break;
    case end:
      fprintf(yyout,"\tend();\n");
      break;
    case code_ok:
      fprintf(yyout,"*\t%s\n", help->code);
      break;
    case hashargs:
      fprintf(yyout, "\t#args "); print_types(help->args.argtp, help->args.n);
      fprintf(yyout, "\n");
      break;
    case hashtildeargs:
      fprintf(yyout, "\t#tildeargs "); print_types(help->args.argtp, help->args.n);
      fprintf(yyout, "\n");
      break;
    case hashrestype:
      fprintf(yyout, "\t#restype "); print_types(help->args.argtp, help->args.n);
      fprintf(yyout, "\n");
      break;
    case hashsetref:
      fprintf(yyout, "\t#setref ");
      switch(GETORDERARGS(help, n)) {
        case SREFimplicit: fprintf(yyout, "implicit\n"); break;
        case SREFexplicit: fprintf(yyout, "explicit\n"); break;
        case SREFkeepimpl: fprintf(yyout, "keepimplicit\n"); break;
        default: fprintf(yyout, "?\n");
        }
      break;
    case inca:
      fprintf(yyout, "\tinca(%d,%d);\n", GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case incr:
      fprintf(yyout, "\tincr(%d,%d);\n", GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case inct:
      fprintf(yyout, "\tinct(%d,%d);\n", GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case incw:
      fprintf(yyout, "\tincw(%d,%d);\n", GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case tinca:
      fprintf(yyout, "\ttinca(%d,%d);\n", GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case tincr:
      fprintf(yyout, "\ttincr(%d,%d);\n", GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case tinct:
      fprintf(yyout, "\ttinct(%d,%d);\n", GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case tincw:
      fprintf(yyout, "\ttincw(%d,%d);\n", GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case deca:
      fprintf(yyout, "\tdeca(%d);\n", GETORDERARGS(help, n));
      break;
    case decr:
      fprintf(yyout, "\tdecr(%d);\n", GETORDERARGS(help, n));
      break;
    case dect:
      fprintf(yyout, "\tdect(%d);\n", GETORDERARGS(help, n));
      break;
    case decw:
      fprintf(yyout, "\tdecw(%d);\n", GETORDERARGS(help, n));
      break;
    case tdeca:
      fprintf(yyout, "\ttdeca(%d);\n", GETORDERARGS(help, n));
      break;
    case tdecr:
      fprintf(yyout, "\ttdecr(%d);\n", GETORDERARGS(help, n));
      break;
    case tdect:
      fprintf(yyout, "\ttdect(%d);\n", GETORDERARGS(help, n));
      break;
    case tdecw:
      fprintf(yyout, "\ttdecw(%d);\n", GETORDERARGS(help, n));
      break;
    case killa:
      fprintf(yyout, "\tkilla(%d);\n", GETORDERARGS(help, n));
      break;
    case killr:
      fprintf(yyout, "\tkillr(%d);\n", GETORDERARGS(help, n));
      break;
    case killt:
      fprintf(yyout, "\tkillt(%d);\n", GETORDERARGS(help, n));
      break;
    case killw:
      fprintf(yyout, "\tkillw(%d);\n", GETORDERARGS(help, n));
      break;
    case tkilla:
      fprintf(yyout, "\ttkilla(%d);\n", GETORDERARGS(help, n));
      break;
    case tkillr:
      fprintf(yyout, "\ttkillr(%d);\n", GETORDERARGS(help, n));
      break;
    case tkillt:
      fprintf(yyout, "\ttkillt(%d);\n", GETORDERARGS(help, n));
      break;
    case tkillw:
      fprintf(yyout, "\ttkillw(%d);\n", GETORDERARGS(help, n));
      break;
    /* PM instructions */
    case advance:
      fprintf(yyout, "\tadvance(%d);\n", GETORDERARGS(help, n));
      break;
    case atend:
      fprintf(yyout, "\tatend(%d, %s);\n",
          GETORDERARGS(help, n), GETORDERARGS(help, label));
      break;
    case atstart:
      fprintf(yyout, "\tatstart(%d, %s);\n",
          GETORDERARGS(help, n), GETORDERARGS(help, label));
      break;
    case bind:
      fprintf(yyout, "\tbind(%d);\n", GETORDERARGS(help, n));
      break;
    case binds:
      fprintf(yyout, "\tbinds(%d);\n", GETORDERARGS(help, n));
      break;
    case bindsubl:
      fprintf(yyout, "\tbindsubl(%d,%d,%d,%d);\n", GETORDERARGS(help, n),
          GETORDERARGS(help, m), GETORDERARGS(help, k), GETORDERARGS(help, l));
      break;
    case dereference:
      fprintf(yyout, "\tdereference();\n");
      break;
    case drop:
      fprintf(yyout, "\tdrop();\n");
      break;
    case endlist:
      fprintf(yyout, "\tendlist();\n");
      break;
    case endsubl:
      fprintf(yyout, "\tendsubl(%d);\n", GETORDERARGS(help, n));
      break;
    case fetch:
      fprintf(yyout, "\tfetch();\n");
      break;
    case initbt:
      fprintf(yyout, "\tinitbt(%d,%d,%d,%d,%d);\n",
          GETORDERARGS(help, n), GETORDERARGS(help, m), GETORDERARGS(help, k),
          GETORDERARGS(help, l), GETORDERARGS(help, j));
      break;
    case tguard:
      if (help->types == NOTYPE)
        fprintf(yyout, "\ttguard(%s,ptd(0x%08x),%d,%d,%d);\n",
            GETORDERARGS(help, label),
            (int)GETORDERARGS(help, desc),
            GETORDERARGS(help, n),
            GETORDERARGS(help, m),
            GETORDERARGS(help, l));
      else
        fprintf(yyout, "\ttguard_%s(%s,ptd(0x%08x),%d,%d,%d);\n",
            short_type_names(GETTYPE(0, help->types)),
            GETORDERARGS(help, label),
            (int)GETORDERARGS(help, desc),
            GETORDERARGS(help, n),
            GETORDERARGS(help, m),
            GETORDERARGS(help, l));
      break;
    case matcharb:
      if (help->types == NOTYPE)
        fprintf(yyout, "\tmatcharb(%d,%s,%s);\n", GETORDERARGS(help, n),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      else
        fprintf(yyout, "\tmatcharb_%s(%d,%s,%s);\n",
            short_type_names(GETTYPE(0, help->types)),
            GETORDERARGS(help, n), GETORDERARGS(help, label), GETORDERARGS(help, ret));
      break;
    case matcharbs:
      if (help->types == NOTYPE)
        fprintf(yyout, "\tmatcharbs(%d,%s,%s);\n", GETORDERARGS(help, n),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      else
        fprintf(yyout, "\tmatcharbs_%s(%d,%s,%s);\n",
            short_type_names(GETTYPE(0, help->types)),
            GETORDERARGS(help, n), GETORDERARGS(help, label), GETORDERARGS(help, ret));
      break;
    case matchbool:
      if (help->types == NOTYPE)
        fprintf(yyout, "\tmatch%s(%s,%s);\n",
            (MKBOOL(0) == GETORDERARGS(help, n) ? "false" : "true"),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      else
        fprintf(yyout, "\tmatch%s_%s(%s,%s);\n",
            (MKBOOL(0) == GETORDERARGS(help, n) ? "false" : "true"),
            short_type_names(GETTYPE(0, help->types)),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      break;
    case matchin:
      if (help->types == NOTYPE)
        fprintf(yyout, "\tmatchin(ptd(0x%08x),%s,%s);\n",
            GETORDERARGS(help, n), GETORDERARGS(help, label), GETORDERARGS(help, ret));
      else
        fprintf(yyout, "\tmatchin_%s(ptd(0x%08x),%s,%s);\n",
            short_type_names(GETTYPE(0, help->types)),
            GETORDERARGS(help, n), GETORDERARGS(help, label), GETORDERARGS(help, ret));
      break;
    case matchint:
      if (help->types == NOTYPE)
        fprintf(yyout, "\tmatchint(int(%d),%s,%s);\n", GETINT(GETORDERARGS(help, n)),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      else
        fprintf(yyout, "\tmatchint_%s(int(%d),%s,%s);\n",
            short_type_names(GETTYPE(0, help->types)),
            GETINT(GETORDERARGS(help, n)), GETORDERARGS(help, label),
            GETORDERARGS(help, ret));
      break;
    case matchlist:
      if (help->types == NOTYPE)
        fprintf(yyout, "\tmatchlist(%d,%s,%s);\n", GETORDERARGS(help, n),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      else
        fprintf(yyout, "\tmatchlist_%s(%d,%s,%s);\n",
            short_type_names(GETTYPE(0, help->types)),
            GETORDERARGS(help, n), GETORDERARGS(help, label), GETORDERARGS(help, ret));
      break;
    case matchprim:
      if (help->types == NOTYPE)
        fprintf(yyout, "\tmatchprim(%s,%s,%s);\n", prselem(GETORDERARGS(help, n)),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      else
        fprintf(yyout, "\tmatchprim_%s(%s,%s,%s);\n",
            short_type_names(GETTYPE(0, help->types)),
            prselem(GETORDERARGS(help, n)),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      break;
    case matchstr:
      if (help->types == NOTYPE)
        fprintf(yyout, "\tmatchstr(%d,%s,%s);\n", GETORDERARGS(help, n),
            GETORDERARGS(help, label), GETORDERARGS(help, ret));
      else
        fprintf(yyout, "\tmatchstr_%s(%d,%s,%s);\n",
            short_type_names(GETTYPE(0, help->types)),
            GETORDERARGS(help, n), GETORDERARGS(help, label), GETORDERARGS(help, ret));
      break;
    case mkcase:
      fprintf(yyout, "\tmkcase(ptd(0x%08x));\n", (int)GETORDERARGS(help, desc));
      break;
    case mkaframe:
      fprintf(yyout, "\tmkaframe(%d);\n", GETORDERARGS(help, n));
      break;
    case mkbtframe:
      fprintf(yyout, "\tmkbtframe(%d);\n", GETORDERARGS(help, n));
      break;
    case mkwframe:
      fprintf(yyout, "\tmkwframe(%d);\n", GETORDERARGS(help, n));
      break;
    case nestlist:
      fprintf(yyout, "\tnestlist();\n");
      break;
    case pick:
      fprintf(yyout, "\tpick(%d);\n", GETORDERARGS(help, n));
      break;
    case restorebt:
      fprintf(yyout, "\trestorebt(%d);\n", GETORDERARGS(help, n));
      break;
    case restoreptr:
      fprintf(yyout, "\trestoreptr(%d);\n", GETORDERARGS(help, n));
      break;
    case rmbtframe:
      fprintf(yyout, "\trmbtframe(%d);\n", GETORDERARGS(help, n));
      break;
    case rmwframe:
      fprintf(yyout, "\trmwframe(%d);\n", GETORDERARGS(help, n));
      break;
    case savebt:
      fprintf(yyout, "\tsavebt(%d);\n", GETORDERARGS(help, n));
      break;
    case saveptr:
      fprintf(yyout, "\tsaveptr(%d);\n", GETORDERARGS(help, n));
      break;
    case startsubl:
      fprintf(yyout, "\tstartsubl(%d,%d);\n",
          GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    /* ASM-FF */
    case mkframe:
      fprintf(yyout, "\tmkframe(%d);\n", GETORDERARGS(help, n));
      break;
    case mkslot:
      fprintf(yyout, "\tmkslot();\n");
      break;
    case Inter:
      fprintf(yyout, "\tinter(%s);\n", out_intertype(GETORDERARGS(help, n)));
      break;
    case pushh:
      fprintf(yyout,"\tpushh(%d);\n", help->args.n);
      break;
    case count:
      fprintf(yyout,"\tcount(%d);\n", help->args.n);
      break;
    case poph:
      fprintf(yyout,"\tpoph();\n");
      break;
    case msdistend:
      fprintf(yyout,"\tmsdistend();\n");
      break;
    case msnodist:
      fprintf(yyout,"\tmsnodist();\n");
      break;
    case wait:
      fprintf(yyout,"\twait(%d);\n", help->args.n);
      break;
    case dist:
    case distb:
      if (ISASM_TILDE())
        fprintf(yyout,"\tdist%s(%s, %s, %d, %d, %d,%d);\n", 
                help->command == dist ? "" : "b",
                GETORDERARGS(help, label), GETORDERARGS(help, ret),
                GETORDERARGS(help, n), GETORDERARGS(help, m), 
                GETORDERARGS(help, k), GETORDERARGS(help, l));
      else
        fprintf(yyout,"\tdist%s(%s, %s, %d, %d);\n", help->command == dist ? "" : "b",
          GETORDERARGS(help, label), GETORDERARGS(help, ret),
          GETORDERARGS(help, n), GETORDERARGS(help, m));
      break;
    case distend:
      fprintf(yyout,"\tdist_end(%d);\n", help->args.n);
      break;
    default:
      fprintf(yyout,"<unknown>\n");
      break;
    }
  help=help->next;
  }
DBUG_VOID_RETURN;
}
