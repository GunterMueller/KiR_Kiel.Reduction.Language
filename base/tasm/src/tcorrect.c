#include <malloc.h>
#include <string.h>
#include "dbug.h"
#include "error.h"
#include "main.h"
#include "tcorrect.h"
#include "buildtree.h"

int correct_t_frame()
{
int count=0;
ORDER *ord;
FUNDESC *des;
FUNCTION *func;

DBUG_ENTER("correct_t_frame");
func=GETFIRSTFUNCTION(program_start);
while (NULL!=func)
  {
  des=func->desc;
  ord=func->order;
  while (end!=GETORDERENTRY(ord,command))
    {
    switch (GETORDERENTRY(ord,command))
      {
      case pushtr:
      case pushtw:
	{
	if (GETDESCENTRY(des,nfv)<=GETORDERARGS(ord,n))
	  {
	  count++;
	  yywarn("tilde-frame of %s changed!",des->label);
	  SETDESCENTRY(des,nfv)=GETORDERARGS(ord,n)+1;
	  }
	break;
	}
      case tail:
      case beta:
      case Case:
	{
	if ((0!=GETDESCENTRY(find_function(GETORDERARGS(ord,label))->desc,nfv)) &&
	    (GETDESCENTRY(find_function(GETORDERARGS(ord,label))->desc,nfv)!=
	    GETDESCENTRY(des,nfv)))
	  yyfail("wrong tilde-frame-size for %s !",GETORDERARGS(ord,label));
	break;
	}
      default:
	break;
      }
    ord=GETNEXTORDER(ord);
    }
  func=func->next;
  }
DBUG_RETURN(count);
}