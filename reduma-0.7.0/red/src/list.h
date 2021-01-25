/* $Log: list.h,v $
 * Revision 1.1  1994/03/08  15:08:59  mah
 * Initial revision
 *
 * Revision 1.1  1994/03/08  14:27:28  mah
 * Initial revision
 * */
/*  list.h    */


typedef struct LISTE
{
 PTR_DESCRIPTOR         var;
 STACKELEM               nr;
 struct LISTE         *next;
} list;


typedef struct FVLIST
{
 PTR_DESCRIPTOR         var;
 STACKELEM               nr;
 int                  nlbar;
 struct FVLIST        *next;
} fvlist;

typedef struct HLIST
{
 PTR_DESCRIPTOR         var;
 struct HLIST         *next;
} hlist;

typedef struct FREE_VAR_LIST
{
 PTR_DESCRIPTOR         var;
 int                  nlbar;
 struct FREE_VAR_LIST *next;
} free_var_list;
 

typedef struct ARGLIST
{
 PTR_DESCRIPTOR        lrec;
 free_var_list     *fv_lrec;  
 struct ARGLIST       *next;
} arglist;

 
typedef struct POSTLIST
{
 STACKELEM              var;        
 struct POSTLIST      *link;
 struct POSTLIST      *next;
} postlist;

typedef struct POSTLIST_1
{
 STACKELEM              var;
 int                  nlbar;
 struct POSTLIST_1    *next;
} postlist_1;


typedef struct FNLIST
{
 PTR_DESCRIPTOR      fdesc;
 PTR_DESCRIPTOR      edesc;
 struct FNLIST       *next;
} fnlist;
                    


#define close_fct        1;
#define  uncl_fct        0;

