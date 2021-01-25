/* file-io                                     cfiles.h */
/* (C) Copyright by Christian-Albrechts-University of Kiel 1993		*/
/* letzte aenderung:    10.06.88                        */

/* resultate von file-io                             */
#define RD_OK(c) (((c) & 15) == 0)       /* resultat ist ok         */
#define RDOK    0
#define DIFFVERS 16
#define FILEERR 1
#define WRONG   2
#define NOFN    3
#define IOERR   4

#define EXNPAR   (-2)
#define EXINT    (-3)
#define NOLIST   (-4)
#define ILLVERS  (-5)
#define NO_NAME  (-6)
#define OLDVERS  (-7)

#define EXTCH    '.'               /* extension-zeichen (Vorsicht)     */
#define EXTCHS   ':'               /*       "           SINTRAN3       */
#define DIRCH    '/'               /* directory-zeichen                */
#define DIRCHS   ')'               /*       "           SINTRAN3       */

extern char editlib[FN_ANZ][LENLIB],
            editfil[FN_ANZ][LENFIL],
            editext[FN_ANZ][LENEXT];

#ifdef isfile
char *editname[FN_ANZ] = {
  "Input/Output",
  "Prettyprint",
  "Document",
  "Protocoll",
  "Portable/Ascii",
  "Reduction",
  "Preprocessed"
 };
#else
extern char *editname[FN_ANZ];
#endif

/* end of      cfiles.h */


