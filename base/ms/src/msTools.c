#include "msTools.h"


extern char *rindex();


/*****************************************************************************/
/*                                                                           */
/*  dieHard: (exported function)                                             */
/*                                                                           */
/*  Print error message in 's' and exit...                                   */
/*                                                                           */
/*****************************************************************************/

void dieHard(s)
char *s;
{
  puts(s);
  exit(99);
}


/*****************************************************************************/
/*                                                                           */
/*  isSpace: (exported function)                                             */
/*                                                                           */
/*  Is 'c' a space character?                                                */
/*                                                                           */
/*****************************************************************************/

Boolean isSpace(c)
char c;
{
  return(c == ' ');
}


/*****************************************************************************/
/*                                                                           */
/*  isTab: (exported function)                                               */
/*                                                                           */
/*  Is 'c' a tab character?                                                  */
/*                                                                           */
/*****************************************************************************/

Boolean isTab(c)
char c;
{
  return(c == '\t');
}


/*****************************************************************************/
/*                                                                           */
/*  isLF: (exported function)                                                */
/*                                                                           */
/*  Is 'c' a LF character?                                                   */
/*                                                                           */
/*****************************************************************************/

Boolean isLF(c)
char c;
{
  return(c == '\n');
}


/*****************************************************************************/
/*                                                                           */
/*  isBlank: (exported function)                                             */
/*                                                                           */
/*  Is 'c' a blank character (Space, Tab, LF)?                               */
/*                                                                           */
/*****************************************************************************/

Boolean isBlank(c)
char c;
{
  return(isSpace(c) || isTab(c) || isLF(c));
}


/*****************************************************************************/
/*                                                                           */
/*  split: (exported function)                                               */
/*                                                                           */
/*  Split full pathname into path and file...                                */
/*                                                                           */
/*****************************************************************************/

void split(s, p, f)
char *s, *p, *f;
{
  if(p != NULL) {
    strcpy(p, s);
  }

  if(strlen(s)) {
    if(p != NULL) {
      *((char *)(rindex(p, '/')+1)) = '\0';
    }
    strcpy(f, (char *)rindex(s, '/')+1);
  } else {
    strcpy(f, s);
  }

  return;
}
