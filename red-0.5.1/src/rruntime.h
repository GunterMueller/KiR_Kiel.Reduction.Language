/* $Log: rruntime.h,v $
 * Revision 1.2  1992/12/16  12:50:54  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */



/* Makros fuer das DEBUG-System. Sie werden in runtime:c und breakpoint:c
   benutzt. Makros, die in dieser Datei benutzt werden ohne hier definiert
   zu sein, stammen alle aus ctermio:h!! */

/* last update  by     why

   copied       by     to

*/

#define CMD_MONITOR  250           /* Mode im Runtimesystem */
#define CMD_DISPLAY  110
#define  MESSPROT "mess:prot"                 /* Fuer Messsystem    */

#define EL_CLEAR   fprintf(stdout,"                ");   /* Stackelem loeschen */
#define POS(col,row)        cursorxy(row-1,col-1)        /* terminal:h */
#define REFCOUNT(p)  ((int) ((PTR_DESCRIPTOR) p)->ref_count)

        /* Ausgabe einer (Fehler-)Meldung */
#define WRONGINPUT(text)                    \
                  (                         \
                     (mode!= CMD_MONITOR)       \
                     ? (                    \
                         POS(ERRSPALTE,ERRLINE),   \
                         fprintf(stdout,"%-50s", #text)  \
                       )                    \
                     : fprintf(stdout,"\n ?" #text "\n")\
                  )

        /* Bildschirm auf neue Befehlseingabe vorbereiten */
#define NEWCMD  (  \
                     (mode!= CMD_MONITOR)               \
                     ? (                            \
                         POS(CMDSPALTE,CMDLINE),    \
                         fprintf(stdout,"Eingabe: ")  ,      \
                         fprintf(stdout,"%26s"," "),\
                         POS(INPSPALTE,CMDLINE)      \
                       )                            \
                : fprintf(stdout,"\nCommand: %55s Mode: MONITOR\n"," ")  \
                  )

        /* alte (Fehler-)Meldung entfernen */
#define NEWERR  (                                 \
                     (mode!= CMD_MONITOR)               \
                     ? (                            \
                         POS(ERRSPALTE,ERRLINE),    \
                         fprintf(stdout,"%80s"," ")         \
                       )                            \
                     : 0                  \
                  )

        /* neues Modul anzeigen */
#define NEWMODUL  (                               \
                    (mode != CMD_MONITOR)     \
                     ? (                       \
                         POS(MODULSPALTE,CMDLINE),  \
                      fprintf(stdout,"Modul:%-10.10s",modullist[modulnr - 1]) \
                       )     \
                      : 0 \
                  )

        /* neuen Mode anzeigen */
#define NEWMODE (                                 \
                     (mode!= CMD_MONITOR)               \
                     ? (                            \
                         POS(MODESPALTE,CMDLINE),   \
                         fprintf(stdout,"Mode: %-7.7s","DISPLAY"), \
                         POS(ERRSPALTE,ERRLINE)         \
                       )                            \
                     : 0                        \
                  )

#define NEWDES    (                                 \
                     (mode!= CMD_MONITOR)               \
                     ? (                            \
                         screen_descnt = descnt,    \
                         POS(DESSPALTE,CMDLINE),   \
                         fprintf(stdout,"Descnt: %-5d",descnt - stdesccnt), \
                         POS(ERRSPALTE,ERRLINE)         \
                       )                            \
                     : 0                        \
                  )



#define PRINTSTEL(x,y)   printstel(x,y);    /* printstel:c */
#define CMDSPALTE 1                         /* Bildschirmpositionen fuer das */
#define INPSPALTE 10      /* Eingabe */     /* Debugsystem */
#define ERRSPALTE 1       /* Fehlermeldung */
#define ERRLINE 2         /* Fehlermeldung */
#define MODULSPALTE (co - 43)   /* Modulname */
#define MODESPALTE (co - 27)     /* aktueller Mode */
#define DESSPALTE (co - 13)      /* aktuelle Deskriptorzahl */

#define MAXSTACKS 50
#define STACKANZ (stackanzahl) /* Maximalzahl Stacks (auf Bildschirm) */
#define CMDLINE 1         /* Eingabezeile */

#define NAMELINE (li - 1)     /* Stacknamen   li: Zeilenzahl  */
#define LINELENGTH (co)   /*              co: Spaltenzahl */

#define XLINE    (NAMELINE - 1 )            /* Bildschirmpositionen fuer das */
#define NULLINE  (NAMELINE - 2 )            /* Debugsystem */







