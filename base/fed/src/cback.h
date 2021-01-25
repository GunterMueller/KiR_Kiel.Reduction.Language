/* include-file for cgetexp,cscanexp,cinput       cback.h  */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/

#define fehl(n,eno)     ((n)-((eno)<<4))
#define fehlnr(n)       (((-(n)) & 0x0f)-2)
#define feno(n)         (((-(n)) & 0x0ff0)>>4)

#define MDEPTH  32768    /* maximum parse depth		*/
/* attention: maximum is 15 !  */
#define FLET    (-2)
#define FDIG    (-3)
#define FNUMB   (-4)
#define FINT    (-5)
#define FREAL   (-6)
#define FSTR    (-7)
#define FKEY    (-8)
#define FLETN   (-9)
#define FDIGN   (-10)
#define FNUMBN  (-11)
#define FINTN   (-12)
#define FREALN  (-13)
#define FSTRN   (-14)
#define FPRED   (-15)

struct pars {
  short enop,enip,count;
 };
#ifdef isback
char *emsg[] = {
        "Letter expected",
        "Digit expected",
        "Number expected",
        "Integer expected",
        "Real expected",
        "String expected",
        "Keyword not expected",
        "Letter not expected",
        "Digit not expected",
        "Number not expected",
        "Integer not expected",
        "Real not expected",
        "String not expected",
        "Predefined functionname not expected"
       };
int level,mlev,debug=FALSE;
struct pars parse[MDEPTH];
STACKELEM tm,sm;
#else
extern char *emsg[];
extern int level,mlev,debug;
extern struct pars parse[MDEPTH];
extern STACKELEM tm,sm;
#endif
/* end of               back.h */

