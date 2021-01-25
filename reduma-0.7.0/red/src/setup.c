/* $Log: setup.c,v $
 * Revision 1.25  1996/03/18  16:49:11  cr
 * new entry KiR_lib (library of precompiled KiR programs)
 *
 * Revision 1.24  1996/02/15  15:35:19  cr
 * read DBUG_options from red.setup
 *
 * Revision 1.23  1996/02/15  12:11:57  sf
 * new keyword in red.setup: Execname determines the name of generated executable
 * of tasm (default: a.out)
 *
 * Revision 1.22  1996/02/05  13:18:17  sf
 * nCubelib: path to nctred.o
 * Redumalib: path to lred.o
 * Tasmlib: path to tasm runtime environment
 *
 * Revision 1.21  1995/12/07  16:49:23  rs
 * NEW ENTRY !!!
 *
 * Ticketfile : location (absolute path) of the tickets configuration file
 *
 * NEW ENTRY !!!
 *
 * Revision 1.20  1995/11/06  17:07:42  stt
 * filepointers are set to NULL after closing
 *
 * Revision 1.19  1995/08/28  10:29:45  sf
 * default for TasmOpt changed (now set on default)
 *
 * Revision 1.18  1995/06/29  09:30:54  rs
 * another pvm config variable: pvmcoding
 *
 * Revision 1.17  1995/06/28  08:57:13  rs
 * added a few pvm configs
 *
 * Revision 1.16  1995/05/18  10:04:23  sf
 * Keyword (TasmRuntime) removed, no longer necessary
 *
 * Revision 1.15  1995/05/03  11:10:01  sf
 * new red.setup keyword: CodeDup : int
 * if not set -> no code duplication while using tasm
 * if 0 -> unlimited code duplication
 * if > 0 -> max number of duplication
 *
 * Revision 1.14  1995/03/07  14:47:08  sf
 * new keywords for red.setup : UseTasm TasmOpt UseTasmRedCount Tasm2KiR TasmRuntime
 *
 * Revision 1.13  1995/02/09  16:15:13  rs
 * changed "correct" output of UH's PM statistics
 *
 * Revision 1.12  1994/11/01  14:25:47  car
 * generate codefile in non-DEBUG-version, 2nd
 *
 * Revision 1.11  1994/11/01  14:10:49  car
 * generate codefile in non-DEBUG-version
 *
 * Revision 1.10  1994/08/04  10:21:41  mah
 * closing strategie of tilde version
 * selectable by CloseGoal entry
 *
 * Revision 1.9  1994/06/21  10:03:04  car
 * *** empty log message ***
 *
 * Revision 1.8  1994/06/21  07:17:04  car
 * CodeFile2 implemented
 *
 * Revision 1.7  1994/06/16  08:41:44  car
 * *** empty log message ***
 *
 * Revision 1.5  1994/06/15  07:33:24  car
 * optimizer flags implemented
 *
 * Revision 1.4  1994/01/19  16:50:43  mah
 * extended CodeFile generation added
 *
 * Revision 1.2  1992/12/16  12:51:30  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* file setup.c */

#include <stdio.h>
#include <string.h>
#include <unistd.h> /* TB, 2.11.1992 */

#include "dbug.h"

/*--- defines --- */

#define SETUPFILE  "red.setup"
#define BUFLEN     80

#define NO_ACCESS 0
#define RD_ACCESS 1
#define RW_ACCESS 2

/*--- externe funktionen --- */

extern char *getenv();
extern FILE *fopen();
extern int  fclose();

/* RS 02/11/92 */ 
static int get_entry();
/* END of RS 02/11/92 */ 

/*--- globale variablen --- */

       char *SetupFile  = NULL;      /* name of setupfile */

#if UH_ZF_PM
extern FILE *ofp;
extern char pmanal_name[100];
#endif /* UH_ZF_PM */

/*--- lokale variablen --- */

static int  SetupAccess = NO_ACCESS; /* no access */

/* --- makros --- */

#define WARN(msg)           printf("\nWARNING: %s. Wait ... ",msg), fflush(stdout), sleep(2)
#define GET_SETUPFILE()     if (!SetupFile && !(SetupFile = getenv("SETUPFILE"))) \
                              SetupFile = SETUPFILE
#define OPEN_SETUPFILE(fp)  GET_SETUPFILE(); \
                            if ((fp = fopen(SetupFile,"r+")) \
                            || ((fp = fopen(SetupFile,"a")) && ((fp = freopen(SetupFile,"r+",fp))))) \
                              SetupAccess = RW_ACCESS; \
                            else \
                            if ((fp = fopen(SetupFile,"r"))) \
                              SetupAccess = RD_ACCESS; \
                            else \
                              SetupAccess = NO_ACCESS, WARN("can't open red.setup")

#define CLOSE_SETUPFILE(fp) if (fp) {fclose(fp); fp = NULL;} SetupAccess = NO_ACCESS
  /* geaendert von stt */

/*------------------------------------------------------------------------*/
/* get_setup : liest vorgegebenes setup aus setupfile ein                 */
/* ~~~~~~~~~                                                              */
/*    - der name des setupfiles wird aus der shell-variablen SETUPFILE    */
/*      eingelesen; gibt's diese nicht wird "red.setup" genommen.         */
/*    - mithilfe der funktion "get_entry" wird aus dem setupfile ein wert */
/*      (string, int ...) eingelesen, so wie im (sscanf)-formatstring     */
/*      "format" spezifiziert.                                            */
/*    - wird kein eintrag gefunden, so wird der defaultwert "stdvalue"    */
/*      genommen.                                                         */
/*------------------------------------------------------------------------*/

int get_setup(format,value,stdvalue)     /* int eingefuegt von RS 02/11/92 */ 
char *format;
int   value;
int   stdvalue;
{
  FILE *fp;

  OPEN_SETUPFILE(fp);
  if (get_entry(fp,format,value,stdvalue))
    WARN("get_setup: function get_entry failed");
  CLOSE_SETUPFILE(fp);
  return (0);            /* RS 02/11/92 */ 
}

/*-------------------------------------------------------------------------------*/
/* get_entry : liest aus einem filepointer fp mithilfe des fomatstrings "format" */
/*             einen wert in "value" ein. wird kein eintrag gefunden, so wird    */
/*             "stdvalue" genommen und ein eintrag in fp gemacht.                */
/*             bei erfolg wird 0 und bei misserfolg wird 1 geliefert.            */
/*-------------------------------------------------------------------------------*/

static int get_entry(fp,format,value,stdvalue)
FILE *fp;
char *format;
int   value;
int   stdvalue;
{
  char buf[BUFLEN];
  DBUG_ENTER ("get_entry");

  DBUG_PRINT ("SETUP", ("Es war: %s, %d, %d", format, value, stdvalue));

  if (fp) {
    rewind(fp);
    while (!feof(fp)) {
      if (fgets(buf,BUFLEN,fp) == NULL)
        continue;                             /* empty line */
      if (*buf == '#')
        continue;                             /* comment line */
      if (sscanf(buf,format,value) == 1)
        DBUG_RETURN(0);                            /* SETUP_OKAY */
    }
    /* fp is at eof */
    if (SetupAccess == RW_ACCESS) {
      fprintf(fp,format,stdvalue);
      fprintf(fp,"\n");
    }
  }
  sprintf(buf,format,stdvalue);
  DBUG_RETURN(sscanf(buf,format,value) != 1);
}

/*-----------------------------------------------------------------------------*/
/*  getentry : liest aus einem filepointer fp mithilfe des fomatstrings format */
/*             einen string (wert!) in value ein. bei erfolg wird 0            */
/*             zurueckgegeben und bei misserfolg wird 1 geliefert.             */
/*-----------------------------------------------------------------------------*/

static int getentry(fp,format,value)
FILE *fp;
char *format;
int   value;
{
  char buf[BUFLEN];
  if (fp) {
    fseek(fp,0,0);
    while (!feof(fp)) {
      if (fgets(buf,BUFLEN,fp) == NULL)
        continue;                                        /* empty line */
      if (*buf == '#')
        continue;                                        /* comment line */
      if (sscanf(buf,format,value) == 1)
        return(0); /* SETUP_OKAY */
    }
  }
  return(1); /* SETUP_FAIL */
}

/*-----------------------------------------------------------------------------*/
/*  putentry : macht eintrag in (offene) datei mit filepointer fp              */
/*-----------------------------------------------------------------------------*/

static int putentry(fp,format,value)
FILE *fp;
char *format;
int   value;
{
  if (SetupFile && (fp = freopen(SetupFile,"r+",fp)) != NULL) {
    fseek(fp,0,2);            /* at eof */
    fprintf(fp,format,value);
    fprintf(fp,"\n");
    return(0);
  }
  return(1);
}

/*-----------------------------------------------------------------------*/
/* getsetup : liest vorgegebenes setup aus setupfile ein                 */
/*    - der name des setupfiles wird aus der shell-variablen SETUPFILE   */
/*      eingelesen; gibt's diese nicht wird red.setup genommen           */
/*    - mithilfe der funktion getentry wird aus dem setupfile ein wert   */
/*      (string, int ...) eingelesen, so wie im (sscanf)-formatstring    */
/*      format spezifiziert                                              */
/*    - kann der setupfile nicht geoeffnet werden, wird 1 zurueckgegeben */
/*-----------------------------------------------------------------------*/

int getsetup(format,value)
char *format;
int   value;
{
  FILE *fp;
  int ret = 1; /* init to FAIL */

  GET_SETUPFILE();
  if ((fp = fopen(SetupFile,"r")) != NULL) {
    ret = getentry(fp,format,value);
    fclose(fp);
  }
  return(ret);
}

/*-----------------------------------------------------------------------*/
/* putsetup : schreibt vorgegebenes setup in setupfile                   */
/*-----------------------------------------------------------------------*/

int putsetup(format,value)
char *format;
int   value;
{
  FILE *fp;

  GET_SETUPFILE();
  if ((fp = fopen(SetupFile,"a")) != NULL) {
    fprintf(fp,format,value);
    fclose(fp);
    return(0);
  }
  return(1); /* FAIL */
}

/*----------------------------------------------------------------------------*/
/* readsetups : liest die folgenden setups aus setupfile ein                  */
/*----------------------------------------------------------------------------*/

#define MAX_REDCNT 2000000001

FILE *TraceFp = NULL;                /* tracefilepointer */

char CodeFile[BUFLEN]       = "/dev/null";   /* name codefile */
char CodeDump[BUFLEN]       = "codedump";   /* name codedump */
char TraceFile[BUFLEN]      = "tracefile";  /* name tracefile */
int  JumpPrefered           = 0;            /* jump prefered/not prefered */
int  MaxRedcnt              = MAX_REDCNT;   /* no reduction counting if redcnt greater */
int  LazyLists              = 0;            /* lazy lists */
int  CodeAreaPercent        = 10;           /* percent of heap */
int  Changes_Default        = 1;            /* editor: Changes sometimes (0), always (1) */
int  Normalform             = 1;            /* Normalform-Flag */
int  UseTasm                = 0;            /* compile to C (1) [sf 23.02.95]*/
int  TasmOpt                = 1;            /* use Optimizer in tasm (1) [sf 23.02.95] */
int  CodeDup                = -1;           /* use Code duplication  (1) [sf 03.05.95] */
int  UseTasmRedCount        = 0;            /* use reduction counter (1) [sf 23.02.95] */
char Tasm2KiR[BUFLEN]       = "tasm2kir";   /* output of tasm-compiled prg [sf 23.02.95]*/
char nCubelib[BUFLEN]       = "/home/base/lib/";   /* where to find nctred.o */
char Redumalib[BUFLEN]      = "/home/base/lib/";   /* where to find lred.o */
char Tasmlib[BUFLEN]        = "../lib/";    /* runtime library for executable */
char Execname[BUFLEN]       = "a.out";      /* generated executable */
char DBUG_options[BUFLEN]   = "";           /* options for DBUG, cr 15.02.96 */
char KiR_lib[BUFLEN]        = "lib/";       /* library of precompiled KiR programs, 
                                               cr 12.03.96 */

#if DEBUG
int  OutInstr               = 1;            /* for inter: show instructions */
int  OutLen                 = 10;           /* for inter: len of instruction area */
#endif

char TargetDir[BUFLEN]      = "./";         /* target directory for load_heap */
char HeapDumpFile[BUFLEN]   = "/dev/null";  /* no output */
char HeapLoadFile[BUFLEN]   = "/dev/null";  /* no input */
char TestDumpFile[BUFLEN]   = "/dev/null";  /* no test-output */
char HeapDumpFormat[BUFLEN] = "ascii";      /* format: ascii, binary, cast */
char HeapLoadFormat[BUFLEN] = "ascii";      /* format: ascii, binary, cast */
char ExecShell[BUFLEN]      = "none";       /* no shell to execute (in place of inter) */
char TagSysName[BUFLEN]     = "hardware";   /* hardware tag system */
int  HeapBase               = 0;            /* start at HEAPBASE (=0) */
int  HeapAlign              = 1;            /* byte alignment 1, word alignment 4 */
long CreationAreaPercent    = 30;           /* percent of heap for creation area (scavenging) */
long SurvivorSpacePercent   = 9;            /* percent of heap for survivor space (scavenging) */
long MaxAge                 = 2;            /* maximal age of objects (scavenging) */
long RSBlock                = 32;           /* (scavenging) */

int UseTypes              = 0;            /* infer types by interpreting ASM code */
int ReduceRefCnt            = 0;            /* reduce reference counting */

#if D_DIST
int cube_dim                = 3;            /* dimension of the subcube */
int d_nodistnr              = 1;            /* nr. of not-distributed expressions in letpar */
int cube_stack_size         = -1;           /* size of the nCUBE stack area */
int cube_stack_seg_size     = 8192;         /* size (in elements) of an nCUBE stack segment */
char ticket_file[80] = "red.tickets";        /* absolute path to ticket file */
#endif

#if D_MESS
char m_remote_host[BUFLEN]  = "amun";       /* measurement file will be copied /tmp here */
char m_target_dir[BUFLEN]   = "/tmp/measurement/"; /* target dir */
char m_remote_dir[BUFLEN]   = "/tmp/";     /* temporary remote dir */
char m_remote_bin[BUFLEN]   = "/home/majestix/rs/red/src/m_remote_merge"; /* program of remote merging */
int  m_del_files            = 0;            /* remove temp files */
char m_mesfilepath[BUFLEN] = "//d02/red/messdateien/";   /* path for measurement files */
char m_mesfilehea[BUFLEN]  = "measure_node"; /* prefix for measurement files */
int m_ackno                 = 1;            /* acknowledge for measuring data */
int m_merge_strat           = 0;            /* merging strategy */
#endif

#if AUTODIST
int d_autodist_on = 0;                                            /* insert letpar on/off */
int d_autodist_unary = 0;                          /* insert letpar for unary application */
char d_autodist_name[BUFLEN] = "autodist";                   /* prefix for variable names */
int d_autodist_ack = 0;                              /* autodist statistics output on/off */
#endif /* AUTODIST */

int ExtendedCodeFile = 0;
#if WITHTILDE
int CloseGoal = 0;
#endif /* WITHTILDE */

#if D_PVM
int pvmspawn_flag = 0;
char pvmspawn_where[BUFLEN] = "";
int pvmspawn_ntask = 0;
int pvmcoding = 0;
#endif /* D_PVM */

void readsetups()
{
  char buf[BUFLEN];
  FILE *fp;

  OPEN_SETUPFILE(fp);  /* casts in get_entry-Aufrufen von RS 3.12.1992 */
  get_entry(fp," MaxRedcnt : %d ",(int) &MaxRedcnt,MAX_REDCNT);
  get_entry(fp," JumpPrefered : %s ",(int) buf,(int) "no"); JumpPrefered = !strcmp(buf,"yes");
  get_entry(fp," LazyLists : %s ",(int) buf,(int) "no");    LazyLists    = !strcmp(buf,"yes");
  get_entry(fp," CodeAreaPercent : %d ",(int) &CodeAreaPercent,10);
  get_entry(fp," Changes_default : %d ",(int) &Changes_Default,1);
  get_entry(fp," Normalform : %d ",(int) &Normalform,1);        /* Normalform-Flag */

  /* setups for dumpheap.c */
  get_entry(fp," Targetdir : %s",(int) TargetDir,(int) "./");
  get_entry(fp," HeapDumpFile : %s",(int) HeapDumpFile,(int) "/dev/null");
  get_entry(fp," HeapLoadFile : %s",(int) HeapLoadFile,(int) "/dev/null");
  get_entry(fp," HeapBase : %d",(int) &HeapBase,0);
  get_entry(fp," HeapAlign : %d",(int) &HeapAlign,1);
  get_entry(fp," HeapDumpFormat : %s",(int) HeapDumpFormat,(int) "ascii");
  get_entry(fp," HeapLoadFormat : %s",(int) HeapLoadFormat,(int) "ascii");
  get_entry(fp," TagSystem : %s",(int) TagSysName,(int) "hardware");

/* TB, 2.11.1992, Vorher:
  getentry(fp," ExecShell : %[^\n]",ExecShell) && putentry(fp," ExecShell : %s","none");
Nachher: */
  if(getentry(fp," ExecShell : %[^\n]",(int)ExecShell)) /* int RS 3.12.1992 */
    putentry(fp," ExecShell : %s",(int) "none");  /* int RS 3.12.1992 */
/* sf 23.02.95 */
  getentry(fp, " UseTasm : %d", (int) &UseTasm, 0); 
  getentry(fp, " TasmOpt : %d", (int) &TasmOpt, 1);
  getentry(fp, " CodeDup : %d", (int) &CodeDup, -1);
  getentry(fp, " UseTasmRedCount : %d", (int) &UseTasmRedCount, 0);
  get_entry(fp," Tasm2KiR : %s ",(int) Tasm2KiR, (int) "tasm2kir");
  get_entry(fp," nCubelib : %s ",(int) nCubelib, (int) "/home/base/lib/");
  get_entry(fp," Redumalib : %s ",(int) Redumalib, (int) "/home/base/lib/");
  get_entry(fp," Tasmlib : %s ",(int) Tasmlib, (int) "../lib/");
  get_entry(fp," Execname : %s ",(int) Execname, (int) "a.out");
  get_entry(fp," DBUG_options : %s ",(int) DBUG_options, (int) ""); /* cr 15.02.96 */
  get_entry(fp," KiR_lib : %s ",(int) KiR_lib, (int) "lib/"); /* cr 12.03.96 */
/***************/

  /* setups for generation scavenging */
  get_entry(fp," CreationAreaPercent : %lu", (int) &CreationAreaPercent, 30);
  get_entry(fp," SurvivorSpacePercent : %lu", (int) &SurvivorSpacePercent, 9);
  get_entry(fp," MaxAge  : %u",  (int) &MaxAge, 2);
  get_entry(fp," RSBlock : %lu", (int) &RSBlock, 32);

  get_entry(fp," UseTypes : %d", (int)&UseTypes, 0);
  get_entry(fp," ReduceRefCnt : %d", (int)&ReduceRefCnt, 0);

  /* read in some debug setups */
  get_entry(fp," CodeFile : %s ",(int) CodeFile,(int) "/dev/null");   /* "codefile" */
#if DEBUG
  get_entry(fp," CodeDump : %s ",(int) CodeDump,(int) "/dev/null");   /* "codedump" */
  get_entry(fp," TraceFile : %s ",(int) TraceFile,(int) "/dev/null"); /* "tracefile" */
  get_entry(fp," OutLen   : %d ",(int) &OutLen,3);
  get_entry(fp," OutInstr : %s ",(int) buf,(int) "yes"); OutInstr = !strcmp(buf,"yes");
#endif /* DEBUG */

#if D_DIST
  get_entry(fp," CubeDim : %d ",(int) &cube_dim,3);
  get_entry(fp," NoDistNr : %d ", (int) &d_nodistnr,1);
  get_entry(fp," CubeStackSize : %d ", (int) &cube_stack_size,-1);
  get_entry(fp," StackSegSize : %d ", (int) &cube_stack_seg_size,8192);
  get_entry(fp," Ticketfile : %s ", (int) ticket_file, (int) "red.tickets");
#endif

#if D_MESS
  get_entry(fp," RemoteHost : %s ", (int) m_remote_host, (int) "amun"); /* remote host */
                                         /* measurement file will be copied in /tmp */
  get_entry(fp," MeasureTargetDir : %s ", (int) m_target_dir, (int) "/tmp/measurement/"); /* target directory for measurement files */
  get_entry(fp," MeasureRemoteDir : %s ", (int) m_remote_dir, (int) "/tmp/"); /* remote temporary dir */
  get_entry(fp," RemoteMergeBin : %s ", (int) m_remote_bin, (int) "/home/majestix/rs/red/src/m_remote_merge"); /* bin for remote merging program */
  get_entry(fp," MeasFilePath : %s ", (int) m_mesfilepath, (int) "//d02/red/messdateien/"); /* path for measurement files */
  get_entry(fp," MeasFileHeader : %s ", (int) m_mesfilehea, (int) "measure_node"); /* prefix for measurement files */
  get_entry(fp," DelTmpFiles : %d ", (int) &m_del_files, (int) 0); /* del files */
  get_entry(fp," M_Acknowledge : %d ", (int) &m_ackno, (int) 1); /* acknowledge measuring data */
  get_entry(fp," MergingStrategy : %d ", (int) &m_merge_strat, (int) 0); /* choose mergine strategy */
#endif /* D_MESS */

#if AUTODIST
  get_entry(fp," AutoDist : %d ", (int) &d_autodist_on, (int) 0);
  get_entry(fp," AutoDistUnary : %d ", (int) &d_autodist_unary, (int) 0);
  get_entry(fp," AutoDistPrefix : %s ", (int) &d_autodist_name, (int) "autodist");
  get_entry(fp," AutoDistAck : %d ", (int) &d_autodist_ack, (int) 0);
#endif /* AUTODIST */

  get_entry(fp," ExtendedCodeFile : %d ", (int) &ExtendedCodeFile, (int) 0); /* show list and matrix contents in CodeFile */
#if WITHTILDE
  get_entry(fp," CloseGoal : %d ", (int) &CloseGoal, (int) 0); /* lrec closing method */
#endif /* WITHTILDE */
#if UH_ZF_PM
#if DEBUG
  get_entry(fp," PManalyze statistics: %s", &pmanal_name,"/dev/null");
#endif
#endif
#if D_PVM
  get_entry(fp," Pvm_Spawn_Flag : %d ", (int) &pvmspawn_flag, (int) 0);
  get_entry(fp," Pvm_Spawn_Where : %s ", (int) &pvmspawn_where, (int) "");
  get_entry(fp," Pvm_Spawn_Ntask : %d ", (int) &pvmspawn_ntask, (int) 0);
  get_entry(fp," Pvm_Encoding : %d ", (int) &pvmcoding, (int) 0);
#endif /* D_PVM */
  CLOSE_SETUPFILE(fp);
}

/* end of setup.c */
