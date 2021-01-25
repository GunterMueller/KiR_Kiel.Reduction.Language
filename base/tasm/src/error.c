#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"

/* Abbruch bei fatalen Fehlern, z.B. Dateioeffnung nicht moeglich
*/

static char buf[256];

int yywarn(const char *fmt, ...)
  {
  va_list ap;

  va_start(ap, fmt);
  sprintf(buf, "%s:%d: warning: ", infile, lineno);
  vsprintf(&buf[strlen(buf)], fmt, ap);
  fprintf(stderr, "%s\n", buf);
  return 0;
  }

int yyerror(const char *fmt, ...)
  {
  va_list ap;

  va_start(ap, fmt);
/*
  if (strncmp("parse error", fmt, 11)) {
*/
    sprintf(buf, "%s:%d: ", infile, lineno);
    vsprintf(&buf[strlen(buf)], fmt, ap);
    fprintf(stderr, "%s\n", buf);
/*
    }
*/
  return 1;
  }

int yyfail(const char *fmt, ...)
  {
  va_list ap;

  va_start(ap, fmt);
  if (lineno)
    sprintf(buf, "%s:%d: FATAL ERROR: ", infile, lineno);
  else
    sprintf(buf, "%s: FATAL ERROR: ", infile);
  vsprintf(&buf[strlen(buf)], fmt, ap);
  fprintf(stderr, "%s\n", buf);
  exit(1);
  }

int yymessage(const char *fmt, ...)
  {
  va_list ap;

  if (verbose) {
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    fprintf(stderr, "%s\n", buf);
    }
  return 0;
  }

/* wird aufgerufen, um das Programmskelett auszugeben
*/

extern FILE *yyout;

void dump_data(char **array)
{
  while (*array)
    fprintf(yyout, "%s\n",*array++);
}
