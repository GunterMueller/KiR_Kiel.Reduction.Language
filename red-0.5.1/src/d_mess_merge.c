/***************************************************************************/
/*                                                                         */
/*   file    : d_mess_merge.c                                              */
/*                                                                         */
/*   contents: distributed merging                                         */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         16.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

#if D_MESS

/* #define M_DISPLAY */

/* system includes */

#include <stdio.h>
#include <string.h>
#include <signal.h>

/* user defined includes */

#include "d_mess_io.h"
#include "dbug.h"
#include "d_mess_merge.h"
#include "rncmessage.h"

/* extern variables */

extern int host;
extern int n_errno;

/* extern functions */

extern void handle_signal();

/* global variables */

m_input_struct m_input_array[M_INPUT_MAX];    /* array of m_input structs */
char m_line[M_INPUT_MAX][M_MAX_LINE];         /* array of m_input lines */
double m_time_marks[M_INPUT_MAX];             /* array of time marks */
char m_buffer[M_MAX_CHANNELS][M_MAX_BUFFER];  /* communication buffers */
int m_counters[M_MAX_CHANNELS];               /* counters for buffers */
int m_max_counters[M_MAX_CHANNELS];           /* max counters (=length) for buffers */
char m_result_buffer[M_MAX_BUFFER];           /* send result to channel buffer */
int m_result_counter;                         /* counter for result */
char m_host_file[M_MAX_LINE];                 /* host file name */

char m_backup[M_MAX_LINE];                    /* buffer for a measure input line */

int m_max_inputs;                             /* max input counter */

char m_display[150];                          /* DBUG display */

int m_binary_counter;                         /* length of binary measurement line */
int m_i;                                      /* global due to performance */
int m_binary_buffer;                          /* buffer for time mark before swapping and writing to host file */

int m_dummy;

FILE *mess_host;                              /* file descriptor for host measure file */

/* functions */

/* prototypes */

void m_awrite_back();    /* writes ascii back to host */
void m_asend_back();     /* writes ascii to measure channel */
int m_read_bmline();
int m_read_bcline();
void m_bsend_back();
void m_bwrite_back();
void m_merge_display();

/* definitions */

/***************************************************************************/
/*                                                                         */
/*   function : m_init_merge                                               */
/*                                                                         */
/*   contents : initializes merge dynamic memory                           */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         28.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

void m_init_merge()

{
  int i;

  DBUG_ENTER ("m_init_merge");
  
  /* ok, no more dynamically allocated memory ... */

  /* reset variables */

  for (i = 0; i < M_INPUT_MAX; i++) {
    m_line[i][0] = '\0';
    m_time_marks[i] = 0.0;
    } 

  for (i = 0; i < M_MAX_CHANNELS; i++) {
    m_buffer[i][0] = '\0';
    m_counters[i] = 0;
    m_max_counters[i] = 0;
    }

  m_result_buffer[0] = '\0';

  m_result_counter = 0;

  DBUG_VOID_RETURN;
}

/***************************************************************************/
/*                                                                         */
/*   function : m_start_merge                                              */
/*                                                                         */
/*   contents : initializes distributed merging on a node                  */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         14.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

void m_start_merge (buffer)

int *buffer;    /* buffer from the message system */

{
  int counter = 0;
  int i,j, dest;
  int big_counter = 0;
  int m_take_me;
  double m_ref_time;
  void (*m_send_back)();
  int flags, type = MT_CLOSE_MEASURE;
  static struct sigaction act = {handle_signal,0,0};     /* for the signal handler */

  DBUG_ENTER ("m_start_merge");

/*  for (i = 0; i < (proc_id * 3); i++)
    m_display[i] = ' ';
  m_display[proc_id * 3] = '\0'; */

  m_init_merge();

  DBUG_PRINT ("MERGE", ("m_display initialized: %d %d", proc_id, strlen(m_display)));

/*** don't forget to switch flag (interrupt-handler) back or switch totally off ***/

/*  sigaction (SIGMSGS,NULL,NULL);  */

/**********/
  
  m_sendto = *(buffer+counter++);
  m_max_files = *(buffer+counter++);
  m_max_message = *(buffer+counter++); 

  m_max_inputs = m_max_files + m_max_message;

  DBUG_PRINT ("MERGE", ("Send data back to proc nr. %d !", m_sendto));
  DBUG_PRINT ("MERGE", ("Open %d files !", m_max_files));
  DBUG_PRINT ("MERGE", ("Read %d message channels !", m_max_message));

  if ((m_sendto < -1) || (m_sendto >= (1 << cube_dim)))
    fprintf (stderr, "node %d: warning, illegal m_sendto !\n\n", proc_id);

  if (m_max_files > M_MAX_FILES)
    fprintf (stderr, "node %d: warning, too many open files for merging !\n\n", proc_id);

  if (m_max_message > M_MAX_CHANNELS)
    fprintf (stderr, "node %d: warning, too many open measure channels !\n\n", proc_id);

  /************************** initialize measure files for reading */

  for (i = 0; i < m_max_files; i++) {
    j = *(buffer+counter++);     /* read from that file */
    DBUG_PRINT ("MERGE", ("Read from file %d !", j));

    m_input_array[big_counter].m_entry1 = m_open_mfile(j);
#ifdef M_BINARY
     m_input_array[big_counter].p_m_read_line = m_read_bmline;
#else
    m_input_array[big_counter].p_m_read_line = m_read_amline;
#endif

    if ((*m_input_array[big_counter].p_m_read_line)(m_input_array[big_counter].m_entry1, big_counter))
      big_counter++;
    }

  /************************** initialize measure channels for reading, get first block */

  for (i = 0; i < m_max_message; i++) {
    j = *(buffer+counter++);      /* get buffer from that node */

    DBUG_PRINT ("MERGE", ("Read from node %d !", j));

    /* write to node */

    type = MT_SEND_MEASURE;

    DBUG_PRINT ("MERGE", ("Send buffer get request to node %d.", j));

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
     m_merge_display ("H", j);
#endif
/*** DBUG DBUG DBUG ***/

    nwrite (NULL, 0, j, type, flags); 

    /* get buffer and initialize */

    type = MT_SEND_MEASURE;

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
     m_merge_display ("R", j); 
#endif
/*** DBUG DBUG DBUG ***/

    dest = j;
    m_max_counters[i] = nread (&m_buffer[i], M_MAX_BUFFER, &dest, &type, &flags);

/*** DBUG DBUG DBUG ***/
    if ((dest & 0x0000ffff) != j)
      fprintf (stderr, "node %d: warning, unexpected message received !\n\n", proc_id); 

/* fprintf (stderr, "node %d: kam %d, %d !!\n", proc_id, dest, type); */

    if (m_max_counters[i] < 0)
      fprintf (stderr, "error on node %d: %x, %s\n", proc_id, n_errno, strerror(n_errno));

/*      post_mortem ("i less than zero !"); */
/*** DBUG DBUG DBUG ***/

    DBUG_PRINT ("MERGE", ("Length of buffer %d: %d, real: %d", i, m_max_counters[i], strlen(m_buffer[i])));

     DBUG_PRINT ("MERGE", ("\nBUFFER:\n*******\n\n%s\n\nEND_OF_BUFFER\n*************\n", m_buffer[i])); 

    /* initialize struct */

#ifdef M_BINARY
    m_input_array[big_counter].p_m_read_line = m_read_bcline;   /* read line function */
#else
    m_input_array[big_counter].p_m_read_line = m_read_acline;   /* read line function */
#endif
    m_input_array[big_counter].m_entry1 = i;        /* buffer and counter dereference */
    m_input_array[big_counter].m_entry2 = j;        /* source input node */

    m_counters[i] = 0;

    if (m_max_counters[i]) 
      if ((*m_input_array[big_counter].p_m_read_line)(i, big_counter))
        big_counter++;
    }

  /************************* initialize target */
    
  if (m_sendto < 0) {   /* write back to host file ? */
    DBUG_PRINT ("MERGE", ("Write to host file !"));

    strcpy (m_host_file, (char *)(buffer+counter));

    DBUG_PRINT ("MERGE", ("Hostfilename: %s !", m_host_file));

    if ((mess_host = fopen(m_host_file , "a")) == NULL) 
      fprintf (stderr, "node %d: warning, cannot open host measure file\n\n", proc_id);

    /* initialize function for write line back */

#ifdef M_BINARY
    m_send_back = m_bwrite_back;
#else
    m_send_back = m_awrite_back;
#endif

    }
  else  {  /* no */
    DBUG_PRINT ("MERGE", ("Send back to node %d !", m_sendto));

    /* initialize function for write line back through channel */

#ifdef M_BINARY
    m_send_back = m_bsend_back;
#else
    m_send_back = m_asend_back;
#endif

    }
 
  DBUG_PRINT ("MERGE", ("m_max_inputs = %d, big_counter = %d", m_max_inputs, big_counter));

  m_max_inputs = big_counter;

  DBUG_PRINT ("MERGE", ("%d merge channels ready !", big_counter));

/*****************************************************************************************/

  while (m_max_inputs) {
   DBUG_PRINT ("MERGE", ("continued ... m_max_inputs = %d.", m_max_inputs));
   m_ref_time = m_time_marks[0];  /* get first time mark as reference */
   m_take_me = 0;
   for (i = 1; i < m_max_inputs; i++) {
     if (m_time_marks[i] <= m_ref_time) {
       m_ref_time = m_time_marks[i];
       m_take_me = i;
       } 
     }

   DBUG_PRINT ("MERGE", ("line taken from nr. %d.", m_take_me));

   /* nr m_take_me chosen */
   /* write back one line and read next line */ 

   (*m_send_back)(m_take_me);


   if (!(*m_input_array[m_take_me].p_m_read_line)(m_input_array[m_take_me].m_entry1, m_take_me)) {
     DBUG_PRINT ("MERGE", ("measure stream nr. %d empty", m_take_me));

     /* switch : m_input_array, m_line, m_time_marks, m_buffer and m_counters throug dereference */

     if ((m_max_inputs > 1) && (m_max_inputs != m_take_me)) {   /* now switch */
       DBUG_PRINT ("MERGE", ("Switching....from %d to %d !", m_max_inputs-1, m_take_me));
       strcpy(m_line[m_take_me], m_line[m_max_inputs-1]); 
       m_input_array[m_take_me] = m_input_array[m_max_inputs-1];
       m_time_marks[m_take_me] = m_time_marks[m_max_inputs-1];
       }
     m_max_inputs -= 1;
     }
   }

/*****************************************************************************************/

  /* close everything */

  DBUG_PRINT ("MERGE", ("now close everything, m_sendto : %d !", m_sendto));

  if (m_sendto < 0) {
    if (fclose(mess_host) == EOF) {
      DBUG_PRINT ("MERGE", ("Oey ! Wasn nu los ?!?!?!?"));
      fprintf (stderr, "error: could not close file.... :-)\n\n");
      post_mortem ("merge: cannot close host measure file"); }
    DBUG_PRINT ("MERGE", ("Sending termination signal to host !"));
    type = MT_CLOSE_MEASURE;
    nwrite(NULL,0,host,type,flags);
    }
  else {     /* terminate communication */
    /* wait for hand-shaking */

    DBUG_PRINT ("MERGE", ("Wait for Hand-Shaking..."));

    type = MT_SEND_MEASURE;
 
/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("Wl", m_sendto);
#endif
/*** DBUG DBUG DBUG ***/

    dest = m_sendto;
    nread (&m_dummy, 0, &dest, &type, &flags);

    /* send block */
    
    DBUG_PRINT ("MERGE", ("Send block to node %d !", m_sendto));

    type = MT_SEND_MEASURE;

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("Sl", m_sendto);
#endif
/*** DBUG DBUG DBUG ***/

     DBUG_PRINT ("MERGE", ("\nBUFFER:\n*******\n\n%s\n\nEND_OF_BUFFER\n*************\n", m_result_buffer));

     DBUG_PRINT ("MERGE", ("Buffer-len: %d, m_result_counter: %d", strlen(m_result_buffer), m_result_counter));

    nwrite (m_result_buffer, m_result_counter, m_sendto, type, flags); 

    /* now send empty block for termination */

    DBUG_PRINT ("MERGE", ("Wait for Termination-Hand-Shaking..."));

    type = MT_SEND_MEASURE;

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("We", m_sendto);
#endif
/*** DBUG DBUG DBUG ***/

    nread (&m_dummy, 0, &m_sendto, &type, &flags);

    /* send block */

    DBUG_PRINT ("MERGE", ("Send empty block to node %d !", m_sendto));

    type = MT_SEND_MEASURE;

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("Se", m_sendto);
#endif
/*** DBUG DBUG DBUG ***/

    nwrite (&m_buffer[0], 0, m_sendto, type, flags);

    }
    
  sigaction (SIGMSGS,&act,NULL);   /* reinstall signal handler ... */

  DBUG_VOID_RETURN;
}

/***************************************************************************/
/*                                                                         */
/*   function : m_open_mfile                                               */
/*                                                                         */
/*   contents : opens measure file nr, returns file pointer                */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         16.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

FILE *m_open_mfile(m_nr)
int m_nr;
{
  char m_name[M_MAX_LINE];
  FILE *m_file;

  DBUG_ENTER ("m_open_mfile");

  sprintf (m_name, "%s_uid%ld_%i", M_MEASURE_NODE_FILE, getuid(), m_nr);

  if ((m_file = fopen(m_name, "r")) == NULL) {
    strcat(m_name, " cannot be opened !");
    fprintf (stderr, "node %d: warning, %s\n\n", proc_id, m_name);
    }
 
  DBUG_RETURN (m_file);
}

/***************************************************************************/
/*                                                                         */
/*   function : m_read_amline                                              */
/*                                                                         */
/*   contents : reads ascii measure line from file, extracts time mark     */
/*              and rest line, returns 1 in case of success,               */
/*              0 for EOF                                                  */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         16.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

int m_read_amline(m_file, nr)

FILE *m_file;
int nr;

{
  DBUG_ENTER ("m_read_amline");

  if (fgets(m_backup, M_MAX_LINE, m_file) == NULL) {
    DBUG_PRINT ("MERGE", ("Closing file %d !", nr));
    if (fclose(m_file) == EOF)
      fprintf (stderr, "node %d: warning, cannot close measure file\n\n", proc_id);
    DBUG_RETURN (0); }
  
  DBUG_PRINT ("MERGE", ("m_backup: %s", m_backup)); 

  sscanf (m_backup, "%lf %[^\n]", &m_time_marks[nr], &m_line[nr]);
 
  /* DBUG_PRINT with doubles don't work ? */
  /* DBUG_PRINT ("MERGE", ("Measure-line time mark: %lf", m_time_marks[nr])); */
  DBUG_PRINT ("MERGE", ("Measure-line rest line: %s", m_line[nr])); 

  DBUG_RETURN (1);
}

/***************************************************************************/
/*                                                                         */
/*   function : m_read_abline                                              */
/*                                                                         */
/*   contents : reads binary measure line from file, extracts time mark    */
/*              and rest line, returns 1 in case of success,               */
/*              0 for EOF                                                  */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         25.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

int m_read_bmline(m_file, nr)

FILE *m_file;
int nr;

{
  DBUG_ENTER ("m_read_bmline");

  fread(&m_time_marks[nr], sizeof(double), 1, m_file);

  if (feof(m_file)) {
    DBUG_PRINT ("MERGE", ("Closing file %d !", nr));
    if (fclose(m_file) == EOF)
      fprintf (stderr, "node %d: warning, cannot close measure file\n\n", proc_id);
    DBUG_RETURN (0); }

  DBUG_PRINT ("MERGE", ("m_backup: %s", m_backup)); 

  m_binary_counter = fgetc(m_file);   /* get length of restline */

  m_line[nr][0] = (char) m_binary_counter;
  for (m_i = 1; m_i <= m_binary_counter; m_i ++)
    m_line[nr][m_i] = (char) fgetc(m_file);

  /* DBUG_PRINT with doubles don't work ? */
  /* DBUG_PRINT ("MERGE", ("measure-line time mark: %lf", m_time_marks[nr])); */
  DBUG_PRINT ("MERGE", ("length of rest line: %d", m_binary_counter));
  DBUG_PRINT ("MERGE", ("measure-line rest line: %s", m_line[nr])); 

  DBUG_RETURN (1);
}

/***************************************************************************/
/*                                                                         */
/*   function : m_read_bcline                                              */
/*                                                                         */
/*   contents : reads binary channel line, extracts time mark              */
/*              and rest line, returns 1 in case of success,               */
/*              0 for EOF                                                  */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         25.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

int m_read_bcline(nr, number)

int nr, number;     /* nr dereference for buffer, number reference for input_array */

{
  int type, flags;

  DBUG_ENTER ("m_read_bcline");

  if (m_counters[nr] >= m_max_counters[nr]) {   /* get new segment ! */
    /* write to node */

    type = MT_SEND_MEASURE;

    DBUG_PRINT ("MERGE", ("Send buffer-get-request to node %d.", m_input_array[number].m_entry2));

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("H", m_input_array[number].m_entry2);
#endif
/*** DBUG DBUG DBUG ***/

    nwrite (NULL, 0, m_input_array[number].m_entry2, type, flags);

    /* get buffer and initialize */
 
    DBUG_PRINT ("MERGE", ("Get buffer from node %d.", m_input_array[number].m_entry2));

    type = MT_SEND_MEASURE;

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("R", m_input_array[number].m_entry2);
#endif
/*** DBUG DBUG DBUG ***/

    m_max_counters[nr] = nread (&m_buffer[nr], M_MAX_BUFFER, &m_input_array[number].m_entry2, &type, &flags);

    DBUG_PRINT ("MERGE", ("Length of buffer %d: %d, real: %d", nr, m_max_counters[nr], strlen(m_buffer[nr])));

    if (m_max_counters[nr] == 0)
      DBUG_RETURN (0);

    m_counters[nr] = 0;
    }

  DBUG_PRINT ("MERGE", ("vorher -- nr: %d, m_counters[nr]: %d, number: %d", nr, m_counters[nr], number));

  m_time_marks[number] = *((double *) &m_buffer[nr][m_counters[nr]]);
  m_counters[nr] += sizeof (double);

  m_binary_counter = m_buffer[nr][m_counters[nr]];
 
  m_line[number][0] = (char) m_binary_counter;
  for (m_i = 1; m_i <= m_binary_counter; m_i++)
    m_line[number][m_i] = m_buffer[nr][m_counters[nr] + m_i];
  m_counters[nr] += m_binary_counter + 1;

  DBUG_PRINT ("MERGE", ("nachher -- nr: %d, m_counters[nr]: %d, number: %d", nr, m_counters[nr], number));

  /* DBUG_PRINT with doubles don't work ? */
  /* DBUG_PRINT ("MERGE", ("measure-line time mark: %lf", m_time_marks[number])); */
  DBUG_PRINT ("MERGE", ("length of rest line: %d", m_binary_counter));
  /* DBUG_PRINT ("MERGE", ("Measure-line rest line: %s, m_counters[%d] = %d + %d", m_line[number], nr, m_counters[nr], strlen(m_backup))); */

  DBUG_RETURN (1);
}

/***************************************************************************/
/*                                                                         */
/*   function : m_read_acline                                              */
/*                                                                         */
/*   contents : reads ascii channel line, extracts time mark               */
/*              and rest line, returns 1 in case of success,               */
/*              0 for EOF                                                  */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         16.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

int m_read_acline(nr, number)

int nr, number;     /* nr dereference for buffer, number reference for input_array */

{
  int type, flags;

  DBUG_ENTER ("m_read_acline");

  if (m_counters[nr] >= m_max_counters[nr]) {   /* get new segment ! */
    /* write to node */

    type = MT_SEND_MEASURE;

    DBUG_PRINT ("MERGE", ("Send buffer-get-request to node %d.", m_input_array[number].m_entry2));

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("H", m_input_array[number].m_entry2);
#endif
/*** DBUG DBUG DBUG ***/

    nwrite (NULL, 0, m_input_array[number].m_entry2, type, flags);

    /* get buffer and initialize */
  
    DBUG_PRINT ("MERGE", ("Get buffer from node %d.", m_input_array[number].m_entry2));

    type = MT_SEND_MEASURE;

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("R", m_input_array[number].m_entry2);
#endif
/*** DBUG DBUG DBUG ***/

    m_max_counters[nr] = nread (&m_buffer[nr], M_MAX_BUFFER, &m_input_array[number].m_entry2, &type, &flags);

    DBUG_PRINT ("MERGE", ("Length of buffer %d: %d, real: %d", nr, m_max_counters[nr], strlen(m_buffer[nr])));

    if (m_max_counters[nr] == 0)
      DBUG_RETURN (0);

    m_counters[nr] = 0;
    }

  sscanf (m_buffer[nr] + m_counters[nr], "%[^\n]", m_backup);

   DBUG_PRINT ("MERGE", ("Hab eingescannt: Startnr: %d, Laenge des Strings: %d, nr: %d, number: %d", m_counters[nr], strlen(m_backup), nr, number)); 

/*  strcat (m_backup, "\n"); */

  DBUG_PRINT ("MERGE", ("m_backup: %s", m_backup)); 

  sscanf (m_backup, "%lf %[^\n]", &m_time_marks[number], &m_line[number]);

  /* DBUG_PRINT with doubles don't work ? */
  /* DBUG_PRINT ("MERGE", ("Measure-line time mark: %lf", m_time_marks[number])); */
  DBUG_PRINT ("MERGE", ("Measure-line rest line: %s, m_counters[%d] = %d + %d", m_line[number], nr, m_counters[nr], strlen(m_backup))); 

  m_counters[nr] += strlen(m_backup) + 1;

  DBUG_RETURN (1);
}

/***************************************************************************/
/*                                                                         */
/*   function : m_awrite_back                                              */
/*                                                                         */
/*   contents : writes one line back to host measure file                  */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         17.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

void m_awrite_back(nr)

int nr;

{
  DBUG_ENTER ("m_awrite_back");

  fprintf (mess_host, "%.lf %s\n", m_time_marks[nr], m_line[nr]);

  DBUG_VOID_RETURN;
}

/***************************************************************************/
/*                                                                         */
/*   function : m_bwrite_back                                              */
/*                                                                         */
/*   contents : writes one binary line back to host measure file           */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         25.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

void m_bwrite_back(nr)

int nr;

{
  DBUG_ENTER ("m_bwrite_back");

  /* SWAP time mark */
  m_binary_buffer = *((int *) &m_time_marks[nr]);
  *((int *) &m_time_marks[nr]) = M_SWAP (*(((int *) &m_time_marks[nr]) + 1));
  *(((int *) &m_time_marks[nr]) + 1) = M_SWAP (m_binary_buffer);
  
  fwrite (&m_time_marks[nr], sizeof(double), 1, mess_host);

  for (m_i = 1; m_i <= (int) m_line[nr][0]; m_i++)
    fputc ((int) m_line[nr][m_i], mess_host); 

  fputc ((int) m_line[nr][0] + 8, mess_host);  /* appending length of line */

  DBUG_PRINT ("MERGE", ("write time mark and %d bytes !", (int) m_line[nr][0]));

  DBUG_VOID_RETURN;
}


/***************************************************************************/
/*                                                                         */
/*   function : m_asend_back                                               */
/*                                                                         */
/*   contents : writes lines in message buffer, sends buffer when full     */
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         17.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

void m_asend_back(nr)

int nr;

{
  int type = MT_SEND_MEASURE;
  int flags;
  DBUG_ENTER ("m_asend_back");

  if (m_result_counter >= (M_MAX_BUFFER - M_MAX_LINE)) {
    /* write block to node */
    /* wait for hand-shaking */

    DBUG_PRINT ("MERGE", ("Wait for Hand-Shaking..."));
   
/*** DBUG DBUG DBUG ***/ 
#ifdef M_DISPLAY
    m_merge_display ("W", m_sendto);
#endif
/*** DBUG DBUG DBUG ***/

    nread (&m_dummy, 0, &m_sendto, &type, &flags);

    /* send block */

    DBUG_PRINT ("MERGE", ("Send block to node %d !", m_sendto));

    type = MT_SEND_MEASURE;

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("S", m_sendto);
#endif
/*** DBUG DBUG DBUG ***/

     DBUG_PRINT ("MERGE", ("\nBUFFER:\n*******\n\n%s\n\nEND_OF_BUFFER\n*************\n", m_result_buffer));

     DBUG_PRINT ("MERGE", ("Buffer-len: %d, m_result_counter: %d", strlen(m_result_buffer), m_result_counter));

    nwrite (m_result_buffer, m_result_counter, m_sendto, type, flags);

    m_result_counter = 0;
    }

  sprintf (m_backup, "%.lf %s\n", m_time_marks[nr], m_line[nr]);

  DBUG_PRINT ("MERGE", ("Writing to buffer at pos %d, len %d : %s", m_result_counter, strlen (m_backup), m_backup)); 

  strcpy (m_result_buffer + m_result_counter, m_backup);

  m_result_counter += strlen (m_backup); 

  DBUG_VOID_RETURN;
}

/***************************************************************************/
/*                                                                         */
/*   function : m_bsend_back                                               */
/*                                                                         */
/*   contents : writes binary line in message buffer,sends buffer when full*/
/*                                                                         */
/*   last changes:                                                         */
/*                                                                         */
/*         25.5.1993 RS                                                    */
/*                                                                         */
/***************************************************************************/

void m_bsend_back(nr)

int nr;

{
  int type = MT_SEND_MEASURE;
  int flags;
  DBUG_ENTER ("m_bsend_back");

  if (m_result_counter >= (M_MAX_BUFFER - M_MAX_LINE)) {
    /* write block to node */
    /* wait for hand-shaking */

    DBUG_PRINT ("MERGE", ("Wait for Hand-Shaking..."));
  
/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("W", m_sendto);
#endif
/*** DBUG DBUG DBUG ***/

    nread (&m_dummy, 0, &m_sendto, &type, &flags);

    /* send block */

    DBUG_PRINT ("MERGE", ("Send block to node %d !", m_sendto));

    type = MT_SEND_MEASURE;

/*** DBUG DBUG DBUG ***/
#ifdef M_DISPLAY
    m_merge_display ("S", m_sendto);
#endif
/*** DBUG DBUG DBUG ***/

    nwrite (m_result_buffer, m_result_counter, m_sendto, type, flags);

    m_result_counter = 0;
    }

  sprintf (m_backup, "%.lf %s\n", m_time_marks[nr], m_line[nr]);

  /* DBUG_PRINT ("MERGE", ("Writing to buffer at pos %d, len %d : %s", m_result_counter, strlen (m_backup), m_backup)); */

  *((double *) &m_result_buffer[m_result_counter]) = m_time_marks[nr];

  m_result_counter += sizeof (double);

  for (m_i = 0; m_i <= (int) m_line[nr][0]; m_i++)
    m_result_buffer[m_result_counter+m_i] = m_line[nr][m_i];

  m_result_counter += m_line[nr][0] + 1;

  DBUG_VOID_RETURN;
}


/* DBUG-function for merging on screen output */

void m_merge_display(s,i)

char *s;
int i;

{
 int j;
 DBUG_ENTER ("m_merge_display");

 for (j = 0; j < (i * 3); j++)
   m_display[j] = ' ';
 m_display[i * 3] = '\0';

 fprintf (stderr, "%s%s%d\n", m_display, s, proc_id);

 DBUG_VOID_RETURN;
}

#endif
