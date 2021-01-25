#ifndef _DYNHSEARCH_
#define _DYNHSEARCH_

typedef struct entry { char *key, *data; } DH_ENTRY;
typedef enum { DH_FIND, DH_ENTER } DH_ACTION;

extern int      DH_hcreate();
extern void     DH_hdestroy();
extern DH_ENTRY *DH_hsearch();
extern void	DH_hwalk();
extern int	DH_hdelete();

#endif
