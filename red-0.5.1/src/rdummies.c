/* $Log: rdummies.c,v $
 * Revision 1.2  1992/12/16  12:49:45  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* rdummies.c */

#include <stdio.h>

/* dummy functions for wb's real functions */

/* TB, 2.11.1992 */
/* Typen und returns */
void loleila()    { return; }
void set_bp()     { return; }
void reset_bps()  { return; }
int get_table()  { return(0); }
int get_tablen() { return(0); }
/* end of TB */

#ifdef NotUsed
/* dummy main function */
main() {
  reduce();
  exit(1);
}
#endif /* NotUsed   auskommentiert RS 4.12.1992 */

#ifdef NotUsed
#if    SCAVENGE

extern char *malloc();

init_scavenge() {}
reinit_scavenge() {}

/***************************************************************************/

char *get_desc()
{
  register char *p;
  if (p = malloc(32)) {
    /* memalign macht's besser !!! */
    p = (char *)(((int)p + 15) & 0xffffff0); /* align to 16 byte boundary */
    return(p);
  }
  else
    post_mortem("get_desc: malloc failed");
}

/***************************************************************************/

char *get_heap(n,p)
unsigned long n, **p;
{
  if (*p = (unsigned long *)malloc(n * 4)) {
    return((char *)*p);
  }
  else
    post_mortem("get_heap: malloc failed");
  return(0);
}

/***************************************************************************/

char *get_buff(n)
unsigned n;
{
  register char *p;
  if (p = malloc(n * 4)) {
    return(p);
  }
  else
    post_mortem("get_buff: malloc failed");
}

#else

init_scavenge() {}
reinit_scavenge() {}

#endif /* SCAVENGE   auskommentiert RS 4.12.1992 */
#endif /* NotUsed    auskommentiert RS 4.12.1992 */

/* end of file */
