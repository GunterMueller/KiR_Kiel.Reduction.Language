/* $Log: d_mess_init.c,v $
 * Revision 1.8  1996/02/23 14:18:48  rs
 * some minor fixes and more DBUG-output
 *
 * Revision 1.7  1996/02/21  17:19:09  rs
 * more DBUG-output
 *
 * Revision 1.6  1995/10/02  16:46:05  rs
 * some more pvm + measure modifications
 *
 * Revision 1.5  1995/09/22  12:46:10  rs
 * additional pvm + measure changes
 *
 * Revision 1.4  1995/09/11  14:21:50  rs
 * some changes for the pvm measure version
 *
 * Revision 1.3  1995/05/22  08:55:43  rs
 * changed nCUBE to D_SLAVE
 *
 * */

/***************************************************************************/
/*                                                                         */
/*   Datei  : d_mess_init.c                                                */
/*                                                                         */
/*   Inhalt : Messroutinen initialisieren und aufraeumen                   */ 
/*                                                                         */
/*   Letzte Aenderung :                                                    */
/*                                                                         */
/*         30.03.1993 RS                                                   */
/*                                                                         */
/***************************************************************************/

#if D_MESS

#include <stdlib.h>

#if (!D_SLAVE)
#include <sys/stat.h>
#endif

#if D_PVM
#include <sys/time.h>
#endif
#include "rheapty.h"
#include "rncstack.h"
#include "rncsched.h"

extern post_mortem();
extern int newheap();
extern void freeheap();
extern T_DESCRIPTOR *newdesc();
extern void freedesc();
extern int compheap();
extern void allocate_d_stack_segment();
extern void free_d_stack_segment();
extern PROCESS_CONTEXT *process_create();
extern void process_terminate();
extern void process_sleep();
extern void process_wakeup();
extern void process_run();
extern void msg_send();
extern void msg_receive();
extern INSTR *distribute();

#if (!D_SLAVE)
extern char *_errmes;
unsigned int m_redcnt;
int m_heapsize, m_heapdes;
extern int cube_dim;
extern int d_nodistnr;
char mess_file[120]; /* real name measure file on host */
#endif
extern unsigned int _redcnt;

/* System-Include-Dateien: */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

/* ben.-def. Include-Dateien: */

#include "d_mess_io.h"
#include "dbug.h"

/* globale Variablen (auch extern benutzt) */

#include "d_mess_names.h"

FILE *d_mess_dat;       /* eine Messdatei */
char d_mess_name[80];   /* name of measure-file */

char d_bitmask[D_MAX_NAMES+1];    /* really great bitmask (that what it should be) for 
                                     measuring */

int d_bib_mask = 0;             /* real bitmask for measure functions (<32 !!!!!!) */

char *d_mess_buff;

int m_base;

#if D_SLAVE

#if D_PVM
struct timeval pvm_time;
struct timeval pvm_time_host;
struct timeval pvm_time_delta;
long pvm_dsec, pvm_dusec;
#endif

/* merging variables */

int m_sendto;        /* nr. of node to send the measure result to (or host-file) */
int m_max_files;     /* nr. of measure files to read from */
int m_max_message;   /* nr. of pre-sorted measure files to read from */

/* measure variables */

int _d_dummy;
double _d_ddummy;
char _d_cdummy;
char *_d_sdummy;
int _m_tmp;

int m_curr_pid = -1;      /* measure current pid */

char m_mesfilehea[255]; /* prefix of measurement file name */
int m_merge_strat;
char m_mesfilepath[255]; /* path of measurement file */
int m_ackno;      /* acknowledge measuring data */

int (*d_m_newheap)() = newheap;   /* function-pointer for heap alloc measure */
void (*d_m_freeheap)() = freeheap; /* function-pointer for heap free measure */
void (*d_m_freedesc)() = freedesc;  /* function-pointer for desc free measure */
T_DESCRIPTOR *(*d_m_newdesc)() = newdesc; /* function-pointer for desc alloc measure */
int (*d_m_compheap)() = compheap;  /* function-pointer for heap compact measure */
void (*d_m_allocate_d_stack_segment)() = allocate_d_stack_segment; /* function-pointer for stack_segment_allocation measure */
extern void (*d_m_free_d_stack_segment)() = free_d_stack_segment; /* function-pointer for stack_segment_free measure */
int d_m_stack_pop = 0;   /* for stack pop measure (no function available) */
int d_m_stack_push = 0;  /* for stack push measure (no function available) */
/* special for stack push and pop */
int m_stack_pop = 1;
int m_stack_push = 1;
INSTR *(*d_m_distribute)() = distribute;
PROCESS_CONTEXT *(*d_m_process_create)() = process_create;
void (*d_m_process_terminate)() = process_terminate;
void (*d_m_process_run)() = process_run;
void (*d_m_process_sleep)() = process_sleep;
void (*d_m_process_wakeup)() = process_wakeup;
void (*d_m_msg_send)() = msg_send;
void (*d_m_msg_receive)() = msg_receive;
int m_msg_send_begin = 0;     /* dynamic switches */
int m_msg_send_end = 0;
int m_msg_receive_begin = 0;
int m_msg_receive_end = 0;

#endif

/* Funktionen */

#if (!D_SLAVE)

/***************************************************************************/
/*                                                                         */
/* function : d_write_header                                               */
/*                                                                         */
/* work     : writes header                                                */
/*                                                                         */
/* last change      :                                                      */
/*                                                                         */
/*       09.03.1993 RS                                                     */
/*                                                                         */
/***************************************************************************/

void d_write_header(datei)

FILE *datei;

{
 time_t now;
 char *s;

 DBUG_ENTER ("d_write_header");

 now = time(NULL);

 fprintf (datei, "file: %s\n", _errmes);
 s = ctime(&now);
 fprintf (datei, "time: %s", s);
 fprintf (datei, "cubedim: %d\n", cube_dim);
 fprintf (datei, "reduction steps: %d\n", (m_redcnt - _redcnt));
 fprintf (datei, "time factor: 1000000\n");
 fprintf (datei, "nodistnum: %d\n", d_nodistnr);

#ifdef M_BINARY
 fprintf (datei, "binary: 1\n");
#else
 fprintf (datei, "binary: 0\n");
#endif

 fprintf (datei, "heapsize: %d\n", m_heapsize);
 fprintf (datei, "heapdes: %d\n", m_heapdes); 

 fprintf (datei, "---\n");

 DBUG_VOID_RETURN;
}

#endif

#if D_SLAVE

/***************************************************************************/
/*                                                                         */
/* Funktion : d_init_mess                                                  */
/*                                                                         */
/* Aufgabe  : Initialisierungsroutinen (Messdatei oeffnen)                 */
/*                                                                         */
/* Letzte Aenderung :                                                      */
/*                                                                         */
/*       05.03.1993 RS  aeh, programmiert...                               */
/*                                                                         */
/***************************************************************************/

void d_init_mess (proc)

int proc;

{
 static char m_joke[120];

 DBUG_ENTER("d_init_mess");

 sprintf (d_mess_name, "%s%s_uid%ld_%i", m_mesfilepath, m_mesfilehea, getuid(), proc);

 unlink (d_mess_name);

 if ((d_mess_dat = fopen(d_mess_name, "w")) == NULL) {
   DBUG_PRINT ("d_init_mess", ("Prozessor: %d; Messdatei konnte nicht geoeffnet werden !", proc));
   post_mortem ("cannot open measurement-file"); }
 
 DBUG_PRINT ("d_init_mess", ("Prozessor: %d; Messdatei erfolgreich zum Schreiben geoeffnet.", proc));

  /* initialization of global measure-variables */

 DBUG_VOID_RETURN;
}


/***************************************************************************/
/*                                                                         */
/* Funktion : d_close_mess                                                 */
/*                                                                         */
/* Aufgabe  : Geordnetes Schliessen der Messaktivitaeten                   */
/*                                                                         */
/* Letzte Aenderung :                                                      */
/*                                                                         */
/*       18.01.1993 RS  aeh, programmiert...                               */
/*                                                                         */
/***************************************************************************/

void d_close_mess ()

{
 DBUG_ENTER("d_close_mess");

 if (fclose(d_mess_dat) == EOF)
   post_mortem ("cannot close measurement-File");
 
 DBUG_PRINT ("d_init_mess", ("Messdatei erfolgreich geschlossen."));
 
 DBUG_VOID_RETURN;
}

#else /* D_SLAVE */

/***************************************************************************/
/*                                                                         */
/* function : d_read_init                                                  */
/*                                                                         */
/* work     : reads init-file for measure-preferences                      */
/*                                                                         */
/* last change      :                                                      */
/*                                                                         */
/*       01.03.1993 RS                                                     */
/*                                                                         */
/***************************************************************************/

void d_read_init ()

{
 FILE *initdatei;
 int i;
 char line[120];
 int tilt = 0;

 DBUG_ENTER("d_read_init");

 /* allocating Message-Buffer for sending arguments to nodes for merging ! */
 if ((d_mess_buff = malloc(D_MESS_MAXBUF)) == NULL)
   post_mortem ("d_read_init: cannot allocate measure-merging-buffer !");
 
 for (i=1; i<=D_MAX_NAMES; i++)
   D_BITMASK[i] = '0';

 if ((initdatei = fopen(D_MESS_INIT, "r")) == NULL)
   post_mortem ("cannot open measure-token-file");

 DBUG_PRINT("d_read_init", ("Mess-Init-Datei erfolgreich zum Lesen geoeffnet !"));

 d_bib_mask = 0;

 while (fgets(line, 120, initdatei) != NULL) {
   tilt = 0;
   if (line[0]=='\n') {
     DBUG_PRINT ("d_read_init", ("empty line..."));  /* empty line */
     tilt = 1; 
     }
   else if (line[0]=='#') {
     DBUG_PRINT ("d_read_init", ("comment..."));  /* comment */
     tilt = 1;
     }
   else {
        line[strlen(line)-1] = '\0';   /* smash '\n' */
        for (i=1; i<=D_MAX_NAMES; i++)
          if (strcmp(mess_names[i], line) == 0) {
            tilt = 1;
            DBUG_PRINT ("d_read_init", ("Nr. %d = %s erkannt !", i, mess_names[i]));
            D_BITMASK[i] = '1';
            if (i < 33)
              d_bib_mask += (1 << i);
            continue; }
        }
  if (!tilt)
    post_mortem ("d_read_init: unkown INIT-token");
  }
  
  if (fclose(initdatei) == EOF)
    post_mortem ("cannot close measure-token-file");
 
  DBUG_PRINT ("d_read_init", ("bitmask: %s", D_BITMASK));
  DBUG_PRINT ("d_read_init", ("real bitmask: %x", d_bib_mask));

  /* initialization of global measure-variables */

  DBUG_VOID_RETURN;
}

#endif /* D_SLAVE */

#if (!D_SLAVE)

/* here are some functions for rmessage.c, 'cause there may be strange include files */

void host_flush()
{
  DBUG_ENTER ("host_flush");
  fflush(stdout);
  DBUG_VOID_RETURN;
}

double host_time()
{
 struct timeval t0;
 DBUG_ENTER ("host_time");

 gettimeofday (&t0, NULL);
 DBUG_RETURN (t0.tv_sec + t0.tv_usec/1000000.0);
}

long host_stat(n)

char *n;

{
 struct stat stbuf;
 long doedel;
 DBUG_ENTER ("host_stat");

 if (stat(n, &stbuf) == -1)
   post_mortem ("merge: cannot access host measure file");

 doedel = stbuf.st_size;

 DBUG_PRINT ("MERGE", ("Angebliche Dateilaenge: %ld", doedel));

 DBUG_RETURN (doedel);
}

#endif /* D_SLAVE */

#if (!D_SLAVE)

/* this function moves the measurement file to specified directories (red.setup) */

void m_remote_copy()

{
  static char my_buffer[255];
  static int m_ret_value;

  DBUG_ENTER ("m_remote_copy");
  

  sprintf (my_buffer, "rcp %s %s:%s", mess_file, m_remote_host, m_remote_dir);

  if ((m_ret_value = system(my_buffer)) != 0) {
    fprintf (stderr, " command failed: %s, return: %d\n", my_buffer, m_ret_value);
    fflush (stderr);
    }

  DBUG_PRINT ("MERGE", ("command: %s, ret-value: %d", my_buffer, m_ret_value));

  if (m_del_files) {
    sprintf (my_buffer, "rm %s", mess_file);

      if ((m_ret_value = system(my_buffer)) != 0) {
        fprintf (stderr, " command failed: %s, return: %d\n", my_buffer, m_ret_value);
        fflush (stderr);
        }

    DBUG_PRINT ("MERGE", ("command: %s, ret-value: %d", my_buffer, m_ret_value));
    }

  sprintf (my_buffer, "rsh %s cp %s %s", m_remote_host, mess_file, m_target_dir);

  if ((m_ret_value = system(my_buffer)) != 0) {
    fprintf (stderr, " command failed: %s, return: %d\n", my_buffer, m_ret_value);
    fflush (stderr);
    }

  DBUG_PRINT ("MERGE", ("command: %s, ret-value: %d ", my_buffer, m_ret_value));

  if (m_del_files) {
    sprintf (my_buffer, "rsh %s rm %s", m_remote_host, mess_file);

    if ((m_ret_value = system(my_buffer)) != 0) {
      fprintf (stderr, " command failed: %s, return: %d\n", my_buffer, m_ret_value);
      fflush (stderr);
      }

    DBUG_PRINT ("MERGE", ("command: %s, ret-value: %d", my_buffer, m_ret_value));
    }

  DBUG_VOID_RETURN;
}

#endif

#endif /* D_MESS */
