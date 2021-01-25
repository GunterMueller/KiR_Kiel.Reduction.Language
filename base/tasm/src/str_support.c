#include <string.h>
#include <stdlib.h>
#include "error.h"
#include "dbug.h"

char *get_str_space(char *s)
{
  char *g;

  DBUG_ENTER("get_str_space");
  
  g=(char *)malloc(strlen(s)+1);
  if (NULL==g)
    yyfail("couldn't allocate memory in get_str_space");
  DBUG_RETURN(g);
}

char *strrepl(char *rep_here, char *rep_me, int count)
{
  int i;
  DBUG_ENTER("strrepl");
  for (i = 0; i < count; i++)
  {
    if (0 == rep_here[i])
      yyfail("insufficient lenth of string in strrepl()");
    rep_here[i] = rep_me[i];
  }
  DBUG_RETURN(&rep_here[i]);
}

int find_substr(char *find_in_me, char *find_me)
{
  int i  = 0;
  int l1 = strlen(find_in_me), 
      l2 = strlen(find_me);
      
  DBUG_ENTER("find_substr");
  while (i < l1 - l2)
  {
    if (!strncmp(&find_in_me[i], find_me, l2))
      DBUG_RETURN(i);
    i++;
  }
  DBUG_RETURN(-1);
}
    
  
