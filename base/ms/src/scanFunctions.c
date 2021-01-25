/*****************************************************************************/
/*                                                                           */
/*  ascanfTime: (exported function)                                          */
/*                                                                           */
/*  Read in an acsii time stamp from file...                                 */
/*                                                                           */
/*****************************************************************************/

void ascanfTime(f, d)
FILE *f;
double *d;
{
  fscanf(f, "%lf", d);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfTime: (exported function)                                          */
/*                                                                           */
/*  Read in a binary time stamp (8 Byte) from file...                        */
/*                                                                           */
/*****************************************************************************/

void bscanfTime(f, d)
FILE *f;
double *d;
{
  fread((char *)d, 8, 1, f);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfProcessor: (exported function)                                     */
/*                                                                           */
/*  Read in an ascii processor id from file...                               */
/*                                                                           */
/*****************************************************************************/

void ascanfProcessor(f, p)
FILE *f;
short *p;
{
  fscanf(f, "%hd", p);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfProcessor: (exported function)                                     */
/*                                                                           */
/*  Read in an binary processor id (1 byte) from file...                     */
/*                                                                           */
/*****************************************************************************/

void bscanfProcessor(f, p)
FILE *f;
short *p;
{
  fread((char *)p, 1, 1, f);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfProcess: (exported function)                                       */
/*                                                                           */
/*  Read in an ascii process id from file...                                 */
/*                                                                           */
/*****************************************************************************/

void ascanfProcess(f, p)
FILE *f;
int *p;
{
  fscanf(f, "%d", p);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfProcess: (exported function)                                       */
/*                                                                           */
/*  Read in an binary process id (4 byte) from file...                       */
/*                                                                           */
/*****************************************************************************/

void bscanfProcess(f, p)
FILE *f;
short *p;
{
  fread((char *)p, 4, 1, f);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfId: (exported function)                                            */
/*                                                                           */
/*  Read in an ascii Id (String) from file...                                */
/*                                                                           */
/*****************************************************************************/

void ascanfId(f, i)
FILE *f;
short *i;
{
  char s[IDS_LEN];

  fscanf(f, "%s", s);
  *i = id2Index(s);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfId: (exported function)                                            */
/*                                                                           */
/*  Read in an binary Id (1 byte) from file...                               */
/*                                                                           */
/*****************************************************************************/

void bscanfId(f, i)
FILE *f;
short *i;
{
  fread((char *)i, 1, 1, f);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfInt: (exported function)                                           */
/*                                                                           */
/*  Read in an ascii integer parameter from file...                          */
/*                                                                           */
/*****************************************************************************/

void ascanfInt(f, i)
FILE *f;
int *i;
{
  fscanf(f, "%d", i);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfInt: (exported function)                                           */
/*                                                                           */
/*  Read in an binary integer parameter (4 byte) from file...                */
/*                                                                           */
/*****************************************************************************/

void bscanfInt(f, i)
FILE *f;
int *i;
{
  fread((char *)i, 4, 1, f);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  ascanfString: (exported function)                                        */
/*                                                                           */
/*  Read in an ascii string parameter from file...                           */
/*                                                                           */
/*****************************************************************************/

void ascanfString(f, s)
FILE *f;
char *s;
{
  fscanf(f, "%s", s);

  return;
}


/*****************************************************************************/
/*                                                                           */
/*  bscanfString: (exported function)                                        */
/*                                                                           */
/*  Read in an binary string parameter (terminated by \0) from file...       */
/*                                                                           */
/*****************************************************************************/

void bscanfString(f, s)
FILE *f;
char *s;
{
  fscanf(f, "%s", s); /* ..... (\0 =?= white space character) */

  return;
}
