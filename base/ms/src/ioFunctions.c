/*****************************************************************************/
/*                                                                           */
/*  ascanfTime: (exported function)                                          */
/*                                                                           */
/*  Read in an acsii time stamp from file...                                 */
/*                                                                           */
/*****************************************************************************/

int ascanfTime(f, d)
FILE *f;
double *d;
{
  fscanf(f, "%lf", d);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfTime: (exported function)                                          */
/*                                                                           */
/*  Read in a binary time stamp (8 Byte) from file...                        */
/*                                                                           */
/*****************************************************************************/

int bscanfTime(f, d)
FILE *f;
double *d;
{
  fread((char *)d, 8, 1, f);

  return(8);
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfProcessor: (exported function)                                     */
/*                                                                           */
/*  Read in an ascii processor id from file...                               */
/*                                                                           */
/*****************************************************************************/

int ascanfProcessor(f, p)
FILE *f;
short *p;
{
  fscanf(f, "%hd", p);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfProcessor: (exported function)                                     */
/*                                                                           */
/*  Read in an binary processor id (2 byte) from file...                     */
/*                                                                           */
/*****************************************************************************/

int bscanfProcessor(f, p)
FILE *f;
short *p;
{
  char c;

  fread(&c, 1, 1, f);
  *p = (short)c;

  return(1);
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfProcess: (exported function)                                       */
/*                                                                           */
/*  Read in an ascii process id from file...                                 */
/*                                                                           */
/*****************************************************************************/

int ascanfProcess(f, p)
FILE *f;
int *p;
{
  fscanf(f, "%d", p);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfProcess: (exported function)                                       */
/*                                                                           */
/*  Read in an binary process id (4 byte) from file...                       */
/*                                                                           */
/*****************************************************************************/

int bscanfProcess(f, p)
FILE *f;
int *p;
{
  fread((char *)p, 4, 1, f);

  return(4);
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfId: (exported function)                                            */
/*                                                                           */
/*  Read in an ascii Id (String) from file...                                */
/*                                                                           */
/*****************************************************************************/

int ascanfId(f, i)
FILE *f;
short *i;
{
  char s[IDS_LEN];

  fscanf(f, "%s", s);
  *i = id2Index(s);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfId: (exported function)                                            */
/*                                                                           */
/*  Read in an binary Id (1 byte) from file...                               */
/*                                                                           */
/*****************************************************************************/

int bscanfId(f, i)
FILE *f;
short *i;
{
  char c;

  fread(&c, 1, 1, f);
  *i = (short)c;

  return(1);
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfInt: (exported function)                                           */
/*                                                                           */
/*  Read in an ascii integer parameter from file...                          */
/*                                                                           */
/*****************************************************************************/

int ascanfInt(f, i)
FILE *f;
int *i;
{
  fscanf(f, "%d", i);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfInt: (exported function)                                           */
/*                                                                           */
/*  Read in an binary integer parameter (4 byte) from file...                */
/*                                                                           */
/*****************************************************************************/

int bscanfInt(f, i)
FILE *f;
int *i;
{
  fread((char *)i, 4, 1, f);

  return(4);
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfString: (exported function)                                        */
/*                                                                           */
/*  Read in an ascii string parameter from file...                           */
/*                                                                           */
/*****************************************************************************/

int ascanfString(f, s)
FILE *f;
char *s;
{
  fscanf(f, "%s", s);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfString: (exported function)                                        */
/*                                                                           */
/*  Read in an binary string parameter (terminated by \0) from file...       */
/*                                                                           */
/*****************************************************************************/

int bscanfString(f, s)
FILE *f;
char *s;
{
  int i=1;

  while((*s = fgetc(f))) {
    ++i;
    ++s;
  }

  return(i);
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfLength: (exported function)                                        */
/*                                                                           */
/*  Read in length of line from file (does nothing for ascii)...             */
/*                                                                           */
/*****************************************************************************/

int ascanfLength(f, i)
FILE *f;
short *i;
{
  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfLength: (exported function)                                        */
/*                                                                           */
/*  Read in length of line (1 byte) from file...                             */
/*                                                                           */
/*****************************************************************************/

int bscanfLength(f, i)
FILE *f;
short *i;
{
  char c;

  fread(&c, 1, 1, f);
  *i = (short)c;

  return(1);
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfTime: (exported function)                                         */
/*                                                                           */
/*  Write an acsii time stamp to file...                                     */
/*                                                                           */
/*****************************************************************************/

int aprintfTime(f, d)
FILE *f;
double d;
{
  fprintf(f, "%.0f", d);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfTime: (exported function)                                         */
/*                                                                           */
/*  Write a binary time stamp (8 Byte) to file...                            */
/*                                                                           */
/*****************************************************************************/

int bprintfTime(f, d)
FILE *f;
double d;
{
  fwrite((char *)&d, 8, 1, f);

  return(8);
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfProcessor: (exported function)                                    */
/*                                                                           */
/* Write an ascii processor id to file...                                    */
/*                                                                           */
/*****************************************************************************/

int aprintfProcessor(f, p)
FILE *f;
short p;
{
  fprintf(f, "%hd", p);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfProcessor: (exported function)                                    */
/*                                                                           */
/*  Write an binary processor id (2 byte) to file...                         */
/*                                                                           */
/*****************************************************************************/

int bprintfProcessor(f, p)
FILE *f;
short p;
{
  char c = (char)p;

  fwrite(&c, 1, 1, f);

  return(1);
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfProcess: (exported function)                                      */
/*                                                                           */
/*  Write an ascii process id to file...                                     */
/*                                                                           */
/*****************************************************************************/

int aprintfProcess(f, p)
FILE *f;
int p;
{
  fprintf(f, "%d", p);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfProcess: (exported function)                                      */
/*                                                                           */
/*  Write an binary process id (4 byte) to file...                           */
/*                                                                           */
/*****************************************************************************/

int bprintfProcess(f, p)
FILE *f;
int p;
{
  fwrite((char *)&p, 4, 1, f);

  return(4);
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfId: (exported function)                                           */
/*                                                                           */
/*  Write an ascii Id (String) to file...                                    */
/*                                                                           */
/*****************************************************************************/

int aprintfId(f, i)
FILE *f;
short i;
{
  fprintf(f, "%s", ids[i]);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfId: (exported function)                                           */
/*                                                                           */
/*  Write an binary Id (1 byte) to file...                                   */
/*                                                                           */
/*****************************************************************************/

int bprintfId(f, i)
FILE *f;
short i;
{
  char c = (char)i;

  fwrite(&c, 1, 1, f);

  return(1);
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfInt: (exported function)                                          */
/*                                                                           */
/*  Write an ascii integer parameter to file...                              */
/*                                                                           */
/*****************************************************************************/

int aprintfInt(f, i)
FILE *f;
int i;
{
  fprintf(f, "%d", i);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfInt: (exported function)                                          */
/*                                                                           */
/*  Write an binary integer parameter (4 byte) to file...                    */
/*                                                                           */
/*****************************************************************************/

int bprintfInt(f, i)
FILE *f;
int i;
{
  fwrite((char *)&i, 4, 1, f);

  return(4);
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfString: (exported function)                                       */
/*                                                                           */
/*  Write an ascii string parameter to file...                               */
/*                                                                           */
/*****************************************************************************/

int aprintfString(f, s)
FILE *f;
char *s;
{
  fprintf(f, "%s", s);

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfString: (exported function)                                       */
/*                                                                           */
/*  Write an binary string parameter (terminated by \0) to file...           */
/*                                                                           */
/*****************************************************************************/

int bprintfString(f, s)
FILE *f;
char *s;
{
  fprintf(f, "%s", s);
  fputc('\0', f);

  return(strlen(s));
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfLength: (exported function)                                       */
/*                                                                           */
/*  Write length of line to file (does nothing for ascii)...                 */
/*                                                                           */
/*****************************************************************************/

int aprintfLength(f, i)
FILE *f;
short i;
{
  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfLength: (exported function)                                       */
/*                                                                           */
/*  Write length of line (1 byte) to file...                                 */
/*                                                                           */
/*****************************************************************************/

int bprintfLength(f, i)
FILE *f;
short i;
{
  char c = (char)i;

  fwrite(&c, 1, 1, f);

  return(1);
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfDelimiter: (exported function)                                    */
/*                                                                           */
/*  Write a space character to file...                                       */
/*                                                                           */
/*****************************************************************************/

int aprintfDelimiter(f)
FILE *f;
{
  fprintf(f, " ");

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfDelimiter: (exported function)                                    */
/*                                                                           */
/*  No delimiters in binary files --> do nothing...                          */
/*                                                                           */
/*****************************************************************************/

int bprintfDelimiter(f)
FILE *f;
{
  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  aprintfNewLine: (exported function)                                      */
/*                                                                           */
/*  Write a new line character to file...                                    */
/*                                                                           */
/*****************************************************************************/

int aprintfNewLine(f)
FILE *f;
{
  fprintf(f, "\n");

  return(0);
}


/*****************************************************************************/
/*                                                                           */
/*  bprintfNewLine: (exported function)                                      */
/*                                                                           */
/*  No lines in binary files --> do nothing...                               */
/*                                                                           */
/*****************************************************************************/

int bprintfNewLine(f)
FILE *f;
{
  return(0);
}

