#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "error.h"
#include "dbug.h"

int verbose, lineno, optimize = 0;
int FRED_FISH = FALSE, WATCH = FALSE, RED_CNT = FALSE, ODDSEX = FALSE, MODUL = FALSE;
int NOSTACK2REG = FALSE;
int DIST = 0;
int TEST_STACK = 0;
char * infile, * outfile, * execfilepath = "", * modulfile = "";
char * default_name = "a.out";
char * execfile = "";
char * very_tmp;
char * targetname;
char * ncubelib = "/home/base/lib";
char * redumalib = "/home/base/lib";
char * tasmlib = "../lib";
char * outfiletmp;
extern char *get_str_space(char *);
int dogeorg = 0, dotis = 0,  doris = 0, dotasm = 0, ref_inferred = 0, dogcc = 0;
int dostaticlink = 0, useacc = 0;
int dontremove = FALSE;
int uses_apply = FALSE;
int mess_enabled = 0;

int asm_mode = ASM_SK;

FILE *yyin, *yyout;

char * version = VERSION,
     * program;

int stacksize = 16384, heapsize = 128*1024;

int main(int argc, char * argv[])
{
  int errflg = 0;
  char c;
  char * dbug_opt = "";
  extern char * optarg;
  extern int optind;
  extern int getopt(int, char **, char *);
  extern int dup_num;
  extern int do_dupe;
  extern int arg_check;

  do_dupe=0;
  program = argv[0];
  infile = outfile = targetname = NULL;
  {
    short a = 0x1234;
    if ((char)a == 0x12) ODDSEX = TRUE;
    else ODDSEX = FALSE;
  }
  while((c = getopt(argc, argv, "ZN:L:l:mno:O:Vvh#:Dad:WRgGtrcCM:XAST")) != -1)
    switch(c) {
      case 'V':
      case '#': /* enter Fred Fish DBUG-Options */
        dbug_opt = optarg; break;
      case 'N': ncubelib = get_str_space(optarg);
                strcpy(ncubelib, optarg);
                break;
      case 'n': /* initiate distribution */
        DIST = TRUE;
        ODDSEX = TRUE;
        break;
      case 'L': redumalib = get_str_space(optarg);
                strcpy(redumalib, optarg);
                break;
      case 'm': mess_enabled = 1; /* measurements for the distributed version */
                break;
      case 'l': tasmlib = get_str_space(optarg);
                strcpy(tasmlib, optarg);
                break;
      case 'O': /* set (compiler) optimizer level */
        optimize = atoi(optarg); break;
      case 'M': /* specify modul output filename */
        modulfile = optarg; 
        MODUL = TRUE; 
        break;
      case 'X': /* don`t remove the C-files */
        dontremove = TRUE; break;
      case 'T': /* test for stackoverflow */  
        TEST_STACK = TRUE; break;
      case 'o': /* specify output filename */
        targetname = optarg; break;
      case 'v': /* set verbose mode */
        verbose = 1; break;
      case 'Z': /* Don't put Stack pointers into registers*/
        NOSTACK2REG=TRUE; break;
      case 'D': /* enable Fred Fish DBUGging for compiled output */
        FRED_FISH=TRUE; break;
      case 'a': /* check arguments for duplication, too */
        arg_check = 1;
        do_dupe = 1;
        break;
      case 'd': /* set number of duplicated functions */
        dup_num = atoi(optarg);
        do_dupe = 1;
        break;
      case 'W':
        WATCH=TRUE; break;
      case 'R': /* enable reduction counting for compiled output */
        RED_CNT=TRUE; break;
      case 'G': /* run reorganizer: convert conditional jump to rtf */
        dogeorg = GEORG_GENRTF;
        break;
      case 'g': /* run reorganizer: convert conditional rtf to jump */
        dogeorg = GEORG_GENJUMP;
        break;
      case 't': /* run type inference system */
        dotis = 1; break;
      case  'r': /* run reference count inference system */
        doris = 1; ref_inferred = 1; break;
      case 'c': /* do backend compilation */
        dotasm = 1; break;
      case 'C': /* do backend compilation and compile to C */
        dotasm = 1; dogcc = 1; break;
      case 'h': /* display usage */
        errflg++; break;
      case 'A': /* set acc mode */
        useacc = 1; break;
      case 'S': /* set static linking */
        dostaticlink = 1; break;
      }  
  if (dogeorg+dotis+doris+dotasm == 0) {
    dogeorg = GEORG_GENJUMP; dotis = doris = dotasm = dogcc = 1;
    }
  if ((WATCH == TRUE) && (optimize >0)) {
    yymessage ("incompatible options -W and -O, disabling -W");
    WATCH = FALSE;
    }
  if ((RED_CNT == TRUE) && (optimize >0)) {
    yymessage ("incompatible options -R and -O, disabling -R");
    RED_CNT = FALSE;
    }
  if ((RED_CNT == TRUE) && (1 == doris)) {
    yymessage ("incompatible options -R and -r, disabling -R");
    RED_CNT = FALSE;
    }
  if (optind+1 < argc)
    errflg++;
  if (errflg) {
    fprintf(stderr, "Usage: %s [options] file\n", argv[0]);
    fprintf(stderr, "    for options see http://www.informatik.uni-kiel.de/~car/project/usage.html\n");
    exit(1);
    }

  DBUG_PUSH(dbug_opt);
  yymessage("%s version %s", argv[0], version);
  if (optind < argc) {
    infile = argv[optind];
    if ((yyin = fopen(argv[optind], "r")) == NULL) {
      fprintf(stderr, "%s: ", argv[0]);
      perror(argv[optind]);
      exit(1);
      }
    }
  else {
    yyin = stdin;
    infile = "<stdin>";
    }
  if (!targetname && dogcc)
    targetname = default_name;
  if (targetname) 
  {
    very_tmp = strrchr(targetname, '/');
    if (NULL != very_tmp)
    {
      execfilepath = (char *)malloc(strlen(targetname) - strlen(very_tmp) + 3);
      strncpy(execfilepath, targetname, strlen(targetname) - strlen(very_tmp) + 1);
      outfile = (char *)malloc(strlen(very_tmp) + 3);
      strcpy(outfile, &very_tmp[1]);
      execfile = (char *)malloc(strlen(very_tmp) + 3);
      strcpy(execfile, &very_tmp[1]);
    }
    else
    {
      execfilepath = (char *)malloc(5);
      strcpy(execfilepath, "./");
      outfile = (char *)malloc(strlen(targetname) + 3);
      strcpy(outfile, targetname);
      execfile = (char *)malloc(strlen(targetname) + 3);
      strcpy(execfile, targetname);
    }
    outfiletmp = (char *)malloc(strlen(outfile) + strlen(P_tmpdir) + 3);
    sprintf(outfiletmp, "%s%s", P_tmpdir, outfile);

    if (dotasm && ((strlen(outfile) < 2) || strcmp(&outfile[strlen(outfile)-2], ".c")))
      strcat(outfile, ".c");
    if (dogcc)
    {
      free(very_tmp);
      very_tmp = (char *)malloc(strlen(outfiletmp) + 3);
      strcpy(very_tmp, outfiletmp);
      strcat(very_tmp, ".c");
      if ((yyout = fopen(very_tmp, "w")) == NULL) {
        fprintf(stderr,"unable to open %s\n", very_tmp);
        perror(argv[0]);
        exit(1);
      }
    }
    else
    {
      free(very_tmp);
      very_tmp = (char *)malloc(strlen(execfilepath) + strlen(outfile) + 3);
      sprintf(very_tmp, "%s%s", execfilepath, outfile);
      if ((yyout = fopen(very_tmp, "w")) == NULL) {
        perror(argv[0]);
        exit(1);
      }
    }
  }
  else
    yyout = stdout;
  lineno = 1;
  if (yyparse()) {
    fprintf(stderr, "parse failed for file %s.\n", infile);
    exit(1);
    }
  lineno = 0;
  if (correct_t_frame()) /* tilde frame size correction !!! */
    fprintf(stderr, "at least one tilde-frame-size corrected.\n");
  yymessage("code reorganizer version %s", vgeorg);
  if (georg(GEORG_GENJUMP)) {
    fprintf(stderr, "reorganizer failed for file %s.\n", infile);
    exit(1);
    }
  if (dotis) {
    yymessage("type inference system version %s", vtis);
    if (tis()) {
      fprintf(stderr, "type inference failed for file %s.\n", infile);
      exit(1);
      }
    }
  if (doris) {
    yymessage("reference count inference system version %s", vris);
    if (ris()) {
      fprintf(stderr, "reference count inference failed for file %s.\n", infile);
      exit(1);
      }
    }
  if (dotasm) {
    yymessage("TildeASM code compiler version %s", vcompile);
    if (compile(modulfile)) {
      fprintf(stderr, "compilation failed for file %s.\n", infile);
      exit(1);
      }
    }
  else {
    if (dogeorg) {
      yymessage("code reorganizer version %s", vgeorg);
      if (georg(dogeorg)) {
        fprintf(stderr, "reorganizer failed for file %s.\n", infile);
        exit(1);
        }
      }
    out_tree(program_start);
    }
  return(0);
}
