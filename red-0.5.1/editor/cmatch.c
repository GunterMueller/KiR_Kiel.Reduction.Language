/*                                              match.c */
/* letzte aenderung:Do  11.12.86        20:46:00        */

#include "cswitches.h"
#include "cportab.h"
#include <stdio.h>
#include <ctype.h>
#include "ctermio.h"
#include "cedit.h"

/* -------------- matchwrds ------------------------------------------ */
/*                                                                     */
matchwrds(t,s)
 register char t[],s[];
 {
  register int i;

  i = 0;
  if (t[0] == s[0]) {
    do {
      if (t[i] == MUST) return(i+1);          else    /* neu  */
      if (iscntrl(t[i]) || t[i] == ' ')  return(i);
      else i++;
    } while (t[i] == s[i]);
    switch(t[i]) {
      case '\0' :
      case ' '  : if (isalpha(t[i-1]))
                    if (isalnum(s[i]) || s[i] == EXTRENN)
                      return(0);
                    else return(i);
                  else return(i);
      case ETX : return(i);
      default  : return(0);
    }
   }
  else return(0);
 }
/* -------------- skipblanks ----------------------------------------- */
char *skipblanks(s)
 register char *s;
 {
  while (*s == ' ')
    s++;
  return(s);
 }
/* -------------- skipcntsp ------------------------------------------ */
/* used by: skipword,cutword                                           */
static char *skipcntsp(s)
 register char *s;
 {
  register char c;

  while ((c = *s) != '\0' && c <= ' ' && c != ESC && c != MUST)
    s++;
  return(s);
 }
/* -------------- skipword -------------------------------------------- */
char *skipword(s)
 register char *s;
 {
  register char c;

  while ((c = *s) != '\0')
    if (c != ' ' && isprint(c))
      s++;
    else return(skipcntsp(s));
  return(s);
 }
/* -------------- cutword -------------------------------------------- */
cutword(t,s,noskip)
 register char *(*t),*(*s);
 int noskip;
 {
  char *skipcntsp(),*skipword();
  register int i,must;

  *t = skipcntsp(*t);
  if (!noskip) *s = skipcntsp(*s);
  if (must = *(*t) == MUST) ++*t;
  if (i = matchwrds(*t,*s)) {
    *t = skipcntsp(*t + i);
    if (noskip) *s = (*s + i);
    else *s = skipcntsp(*s + i);
    return(TRUE);
   }
  else {
    *t = skipword(*t);
    return(must ? FAILURE : FALSE);     /* vorsicht     */
  }
 }
/* end of               match.c */

