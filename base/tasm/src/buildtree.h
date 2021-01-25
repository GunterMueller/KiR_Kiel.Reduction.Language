#ifndef __BUILDTREE__
#define __BUILDTREE__

/* access program components */
#define GETFIRSTFUNCTION(p)  ((p)->function)
#define SETFIRSTFUNCTION(p)  GETFIRSTFUNCTION(p)
#define GETFIRSTDESC(p)      ((p)->desc)
#define SETFIRSTDESC(p)      GETFIRSTDESC(p)
#define GETFIRSTDATA(p)      ((p)->data)
#define SETFIRSTDATA(p)      GETFIRSTDATA(p)

/* access components of function list */
#define GETNAME(f)           ((f)->name)
#define SETNAME(f)           GETNAME(f)
#define GETDESC(f)           ((f)->desc)
#define SETDESC(f)           GETDESC(f)
#define GETFIRSTORDER(f)     ((f)->order)
#define SETFIRSTORDER(f)     GETFIRSTORDER(f)
#define GETNEXTFUNCTION(f)   ((f)->next)
#define SETNEXTFUNCTION(f)   GETNEXTFUNCTION(f)
#define GETFUNCINLINE(f)     ((f)->inlined)
#define SETFUNCINLINE(f)     ((f)->inlined)

/* access components of order list */
#define GETNEXTORDER(o)      ((o)->next)
#define SETNEXTORDER(o)      GETNEXTORDER(o)
#define GETPREVORDER(o)      ((o)->prev)
#define SETPREVORDER(o)      GETPREVORDER(o)
#define GETORDERARGS(o,x)    ((o)->args.x)
#define SETORDERARGS(o,x)    GETORDERARGS(o,x)
#define GETORDERENTRY(o,x)   ((o)->x)
#define SETORDERENTRY(o,x)   GETORDERENTRY(o,x)

/* access components of function descriptor list */
#define GETNEXTDESC(d)       ((d)->next)
#define SETNEXTDESC(d)       GETNEXTDESC(d)
#define GETDESCENTRY(d,x)    ((d)->x)
#define SETDESCENTRY(d,x)    GETDESCENTRY(d,x)

/* access components of data descriptor list */
#define GETNEXTDATA(d)       ((d)->next)
#define SETNEXTDATA(d)       GETNEXTDATA(d)
#define GETDATAENTRY(d,x)    ((d)->x)
#define SETDATAENTRY(d,x)    GETDATAENTRY(d,x)

extern void delete_order(ORDER *);
extern void insert_order_list(ORDER *, ORDER *);

extern PROGRAM * conc_desc_fun(FUNDESC *, DATANODE *, FUNCTION *);

extern FUNDESC * new_desc(DESCTAG, int, int, int, int, const char *);
extern FUNDESC * conc_desc(FUNDESC *, FUNDESC *);

extern DATANODE * conc_data(DATANODE *, DATANODE *);
extern DATANODE * new_data(DATATAG, int, ...);

extern FUNCTION * new_function(const char *, ORDER *);
extern FUNCTION * conc_function(FUNCTION *, FUNCTION *);

extern ORDER * new_order(COMMAND , ...);
extern ORDER * conc_order(ORDER *, ORDER *);
extern FUNCTION * find_function(char *);

#endif
