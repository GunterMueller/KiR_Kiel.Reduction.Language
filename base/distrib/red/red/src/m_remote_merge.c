/* remote merging program

   arguments:    filename for appending
                 source directory
                 target directory
                 prefix
                 number of files
                 0 or 1 for ascii/binary
                 0 or 1 for deleting temporary files
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "dbug.h"
#define M_SWAP(a) (_m_tmp = (a),(((_m_tmp & 0xff) << 24) | ((_m_tmp & 0xff00) << 8) | ((_m_tmp & 0xff0000) >> 8) | ((_m_tmp & 0xff000000) >> 24)))

char m_file_name[255];
char m_remote_dir[255];
char m_target_dir[255];
char m_mesfilehea[255];
int m_number;
int m_binary;
int m_del_files;
int _m_tmp;

int m_ret_value;

FILE *m_result;
FILE *m_files[32];
double m_werte[32];
double m_reference[32];
char m_system[255];

extern getuid();

/* functions from reduction system */

long host_stat(n)

char *n;

{
 struct stat stbuf;
 long doedel;
 DBUG_ENTER ("host_stat");

 if (stat(n, &stbuf) == -1)
   fprintf (stderr, "error: cannot access host measure file\n");

 doedel = stbuf.st_size;

 DBUG_PRINT ("MERGE", ("Angebliche Dateilaenge: %ld", doedel));

 DBUG_RETURN (doedel);
}

double host_time()
{
 struct timeval t0;
 DBUG_ENTER ("host_time");

 gettimeofday (&t0, NULL);
 DBUG_RETURN (t0.tv_sec + t0.tv_usec/1000000.0);
}

/* m_open_files */

void m_open_files (nr)

int nr;

{
  int i;
  char m_name[255];

  DBUG_ENTER ("m_open_files");

  for (i = 0; i < m_number; i++) {
    sprintf (m_name, "%s%s_uid%ld_%i", m_remote_dir, m_mesfilehea, getuid(), i);
    DBUG_PRINT ("RMERGE", ("opening %s !", m_name));
    if ((m_files[i] = fopen(m_name, "r")) == NULL)
      fprintf (stderr, "error: cannot not open %s !\n", m_name);
    }

  DBUG_VOID_RETURN;
}

/* m_close_files */

void m_close_files (nr)

int nr;

{
  int i;

  DBUG_ENTER ("m_close_files");

  for (i = 0; i < m_number; i++)
    if (fclose(m_files[i]) == EOF)
       fprintf (stderr, "error: cannot not close file %i !", i);

  DBUG_VOID_RETURN;
}

/* m_merge_ascii */

void m_merge_ascii()

{
  int m_noch_da;
  int i;
  int take_me;
  double m_ref;
  int m_c;
  FILE *m_buf;

  DBUG_ENTER ("m_merge_ascii");

  /* read initial values */

  for (i = 0; i < m_number; i++) {
    fscanf (m_files[i], "%lf", &m_werte[i]);  
    m_reference[i]=m_werte[i];
/*     m_werte[i]=0; */
    DBUG_PRINT ("RMERGE", ("initial value for %i: %lf, original: %lf", i, m_werte[i]-m_reference[i],m_werte[i]));
    }

  m_noch_da = m_number;

  /* merge ! */

  while (m_noch_da) {
    take_me = 0;
    m_ref = m_werte[0]-m_reference[0];
    for (i = 1; i < m_noch_da; i++)
      if ((m_werte[i]-m_reference[i]) < m_ref) {
        m_ref = m_werte[i]-m_reference[i];
        take_me = i;
        }
    DBUG_PRINT ("RMERGE", ("nr %i taken: %lf", take_me, m_werte[take_me]-m_reference[take_me]));

    fprintf (m_result, "%.lf", m_werte[take_me]-m_reference[take_me]);
    while ((m_c = fgetc(m_files[take_me])) != '\n') 
      fputc (m_c, m_result);
    fputc ('\n', m_result);

    if (fscanf (m_files[take_me], "%lf", &m_werte[take_me]) == EOF) {
      DBUG_PRINT ("RMERGE", ("file %d empty !", take_me));
      m_buf = m_files[m_noch_da-1];
      m_files[m_noch_da-1] = m_files[take_me];
      m_files[take_me] = m_buf;
      m_werte[take_me] = m_werte[m_noch_da-1];
      m_reference[take_me] = m_reference[m_noch_da-1];
      m_noch_da -=1; 
      }
    }

  DBUG_VOID_RETURN;
}

/* binary merge */

void m_merge_binary()

{
  int m_noch_da;
  int i;
  int take_me;
  double m_ref;
  int m_c2;
  FILE *m_buf;
  int m_binary_buffer;

  DBUG_ENTER ("m_merge_binary");

  /* read initial values */

  for (i = 0; i < m_number; i++) {
    fread((char *)&m_werte[i], sizeof(double), 1, m_files[i]);
    m_binary_buffer = *((int *) &m_werte[i]);
    *((int *) &m_werte[i]) = M_SWAP (*(((int *) &m_werte[i]) + 1));
    *(((int *) &m_werte[i]) + 1) = M_SWAP (m_binary_buffer);
    DBUG_PRINT ("RMERGE", ("initial value for %i: %lf", i, m_werte[i]));
    }

  m_noch_da = m_number;

  /* merge ! */

  while (m_noch_da) {
    take_me = 0;
    m_ref = m_werte[0];
    for (i = 1; i < m_noch_da; i++)
      if (m_werte[i] < m_ref) {
        m_ref = m_werte[i];
        take_me = i;
        }
    DBUG_PRINT ("RMERGE", ("nr %i taken: %lf", take_me, m_werte[take_me]));

    fwrite((char *)&m_werte[take_me], sizeof(double), 1, m_result);

    m_c2 = fgetc(m_files[take_me]);
    for (i = 0; i < (int) m_c2; i++) 
      fputc(fgetc(m_files[take_me]), m_result);  
    fputc(m_c2+8, m_result);     /* append length of line + 8 (time value) */

    fread((char *)&m_werte[take_me], sizeof(double), 1, m_files[take_me]);

    if (feof(m_files[take_me])) {
      DBUG_PRINT ("RMERGE", ("file %d empty !", take_me));
      m_buf = m_files[m_noch_da-1];
      m_files[m_noch_da-1] = m_files[take_me];
      m_files[take_me] = m_buf;
      m_werte[take_me] = m_werte[m_noch_da-1];
      m_noch_da -=1;
      }
    else {
      m_binary_buffer = *((int *) &m_werte[take_me]);
      *((int *) &m_werte[take_me]) = M_SWAP (*(((int *) &m_werte[take_me]) + 1));
      *(((int *) &m_werte[take_me]) + 1) = M_SWAP (m_binary_buffer);
      }
    }

  DBUG_VOID_RETURN;
}

/* main */

void main(argc, argv)

int argc;

char *argv[];

{
 double zeit1, zeit2;
 
 long m_l;

 DBUG_PUSH ("d:t");

 zeit1 = host_time();

 strcpy(m_file_name, argv[1]);
 strcpy(m_remote_dir, argv[2]);
 strcpy(m_target_dir, argv[3]);
 strcpy(m_mesfilehea, argv[4]);
 sscanf (argv[5], "%d", &m_number);
 sscanf (argv[6], "%d", &m_binary);
 sscanf (argv[7], "%d", &m_del_files);

 DBUG_PRINT ("RMERGE", ("m_file_name: %s", m_file_name));
 DBUG_PRINT ("RMERGE", ("m_remote_dir: %s", m_remote_dir));
 DBUG_PRINT ("RMERGE", ("m_target_dir: %s", m_target_dir));
 DBUG_PRINT ("RMERGE", ("m_mesfilehea: %s", m_mesfilehea));
 DBUG_PRINT ("RMERGE", ("m_number: %d", m_number));
 DBUG_PRINT ("RMERGE", ("m_binary: %d", m_binary));
 DBUG_PRINT ("RMERGE", ("m_del_files: %d", m_del_files));

 m_open_files();

 sprintf (m_system, "mv %s%s %s", m_remote_dir, m_file_name, m_target_dir);

 DBUG_PRINT ("RMERGE", ("command: %s", m_system));

 if ((m_ret_value = system(m_system)) != 0) 
    fprintf (stderr, " command failed: %s, return: %d\n", m_system, m_ret_value);

 sprintf (m_system, "%s%s", m_target_dir, m_file_name);

 if ((m_result = fopen(m_system, "a")) == NULL)
   fprintf (stderr, " cannot open %s !\n", m_system);

 /* merging */

 if (m_binary)
   m_merge_binary();
 else
   m_merge_ascii();

 if (fclose(m_result) == EOF)
   fprintf (stderr, " cannot close host measurement file !\n");

 sprintf (m_system, "%s%s", m_target_dir, m_file_name);

 m_l = host_stat(m_system);

 m_close_files();
 
 /* remove /tmp files */
  
 if (m_del_files) {
   sprintf (m_system, "rm %s%s_uid%ld_*", m_remote_dir, m_mesfilehea, getuid());
   DBUG_PRINT ("RMERGE", ("command: %s", m_system));

   if ((m_ret_value = system(m_system)) != 0)
     fprintf (stderr, " command failed: %s, return: %d\n", m_system, m_ret_value);
   }
 
 /* hier fehlt noch was ! , aeh, was eigentlich ? */

 zeit2 = host_time();

 printf ("%4.2f s, %4.2f kb/s\n", (zeit2 - zeit1), m_l/(1000.0 * (zeit2 - zeit1)));

}
