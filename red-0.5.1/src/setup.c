/* $Log: setup.c,v $
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

#define CLOSE_SETUPFILE(fp) if (fp) fclose(fp); SetupAccess = NO_ACCESS

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

char CodeFile[BUFLEN]       = "codefile";   /* name codefile */
char CodeDump[BUFLEN]       = "codedump";   /* name codedump */
char TraceFile[BUFLEN]      = "tracefile";  /* name tracefile */
int  JumpPrefered           = 0;            /* jump prefered/not prefered */
int  MaxRedcnt              = MAX_REDCNT;   /* no reduction counting if redcnt greater */
int  LazyLists              = 0;            /* lazy lists */
int  CodeAreaPercent        = 10;           /* percent of heap */
int  Changes_Default        = 1;            /* editor: Changes sometimes (0), always (1) */
int  Normalform             = 1;            /* Normalform-Flag */

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

#if D_DIST
int cube_dim                = 3;            /* dimension of the subcube */
int d_nodistnr              = 1;            /* nr. of not-distributed expressions in letpar */
#endif

#if D_MESS
char m_remote_host[BUFLEN]  = "amun";       /* measurement file will be copied /tmp here */
char m_target_dir[BUFLEN]   = "/tmp/measurement"; /* target dir */
int  m_del_files            = 0;            /* remove temp files */
#endif

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

  /* setups for generation scavenging */
  get_entry(fp," CreationAreaPercent : %lu", (int) &CreationAreaPercent, 30);
  get_entry(fp," SurvivorSpacePercent : %lu", (int) &SurvivorSpacePercent, 9);
  get_entry(fp," MaxAge  : %u",  (int) &MaxAge, 2);
  get_entry(fp," RSBlock : %lu", (int) &RSBlock, 32);

#if DEBUG
  /* read in some debug setups */
  get_entry(fp," CodeFile : %s ",(int) CodeFile,(int) "/dev/null");   /* "codefile" */
  get_entry(fp," CodeDump : %s ",(int) CodeDump,(int) "/dev/null");   /* "codedump" */
  get_entry(fp," TraceFile : %s ",(int) TraceFile,(int) "/dev/null"); /* "tracefile" */
  get_entry(fp," OutLen   : %d ",(int) &OutLen,3);
  get_entry(fp," OutInstr : %s ",(int) buf,(int) "yes"); OutInstr = !strcmp(buf,"yes");
#endif /* DEBUG */

#if D_DIST
  get_entry(fp," CubeDim : %d ",(int) &cube_dim,3);
  get_entry(fp," NoDistNr : %d ", (int) &d_nodistnr,1);
#endif

#if D_MESS
  get_entry(fp," RemoteHost : %s ", (int) m_remote_host, (int) "amun"); /* remote host */
                                         /* measurement file will be copied in /tmp */
  get_entry(fp," MeasureTargetDir : %s ", (int) m_target_dir, (int) "/tmp/measurement/"); /* target directory for measurement files */
  get_entry(fp," DelTmpFiles : %d ", (int) &m_del_files, (int) 0); /* del files */
#endif

  CLOSE_SETUPFILE(fp);
}

/* end of setup.c */
