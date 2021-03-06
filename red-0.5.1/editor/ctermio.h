/*                                                 ctermio.h */


/*
 * Einige ASCII Control-Codes
 * ==========================
 */

#define NUL      0
#define ETX      3
#define BEL      7
#define BS       8
#define HT       9
#define LF       10
#define VT       11
#define FF       12
#define CR       13
#define CAN      24
#define SUBM     26
#define ESC      27
#define FS       28
#define GS       29
#define RS       30
#define DEL      127

/*
 * Bildschirmsteuerung: Belegung der ASCII Controlcodes, Makros
 * ============================================================
 */

#define UP     FS
#define DOWN   VT
#define RIGHT  FF
#define LEFT   BS
#define HOME   RS

#define CNTC   ETX
#define CNTZ   SUBM

/*
 * Kodierung der Funktionstasten
 * =============================
 */

#define F1              350
#define SHIFT_F1        351
#define F2              352
#define SHIFT_F2        353
#define CTRL_F2         354
#define F3              355
#define SHIFT_F3        356
#define CTRL_F3         357
#define F4              358
#define SHIFT_F4        359
#define F5              360
#define SHIFT_F5        361
#define F6              362
#define SHIFT_F6        363
#define F7              364
#define SHIFT_F7        365
#define F8              366
#define SHIFT_F8        367

#define UNKNOWNKEY      999

/*
 * Tastaturabfrage: Belegung der Funktionstasten
 * =============================================
 */

#define      _red1           F1
#define      _red1000        SHIFT_F1
#define      _rdfile         F2
#define      _wrfile         SHIFT_F2
#define      _displ          F3
#define      _displf         SHIFT_F3
#define      _help           F4
#define      _put            F5
#define      _pick           SHIFT_F5
#define      _put1           F6
#define      _pick1          SHIFT_F6
#define      _put2           F7
#define      _pick2          SHIFT_F7
#define      _command        F8
#define      _EXIT           SHIFT_F8

/*
 * Spezielle Tastenbelegungen fuer den Zeileneditor (getline)
 * ==============================================================
 */

#define      _insert         F3
#define      _delete         F4
#define      _DELETE         SHIFT_F4

#if BATCH

#define CURSOR_UP()
#define CURSOR_DOWN()
#define CURSOR_RIGHT()
#define CURSOR_LEFT()
#define CURSOR_HOME()
#define CLEARSCREEN()
#define cleartoeol()
#define cleartoeos()
#define insertchar()
#define deletechar()
#define reverse()
#define noreverse()
#define underline()
#define nounderline()
#define CURSORXY(r,c)
#define beep()
#define putrev(c) putchar(c)

#define      HELP            F4
#define      EXIT            SHIFT_F8

#else /* !BATCH	*/

#define      HELP            F4
#define      EXIT            SHIFT_F8

#if UNIX

/*
 * TERMCAP
 * =======
 */

char *getenv(),*tgetstr(),*tgoto();
int  tgetent(),tgetnum(),tputs(),putch();
extern int  li,co;
extern char *cl,*ce,*cd,*ic,*dc,*cm,*up,*down,*nd,*le,*ho,*so,*se,*us,*ue;
extern char *k0,*k1,*k2,*k3,*k4,*k5,*k6,*k7,*k8,*k9,*l0,*l1,*l2,*l3,*l4,*l5;

#define CURSOR_UP()       tputs(up,1,putch)
#define CURSOR_DOWN()     tputs(down,1,putch)
#define CURSOR_RIGHT()    tputs(nd,1,putch)
#define CURSOR_LEFT()     tputs(le,1,putch)
#define CURSOR_HOME()     tputs(ho,1,putch)
#define CLEARSCREEN()     tputs(cl,1,putch)
#define cleartoeol()      tputs(ce,1,putch)
#define cleartoeos()      tputs(cd,1,putch)
#define insertchar()      tputs(ic,1,putch)
#define deletechar()      tputs(dc,1,putch)
#define reverse()         tputs(so,1,putch)
#define noreverse()       tputs(se,1,putch)
#define underline()       tputs(us,1,putch)
#define nounderline()     tputs(ue,1,putch)
#define CURSORXY(r,c)     tputs(tgoto(cm,c,r),1,putch)
#define beep()            putchar(BEL)

/*
 * Funktionen:
 *    switchterm(mode)  :  Terminalmodus umschalten        (--> "terminal:c")
 *    putrev(c)         :  Zeichen revers                  (--> "terminal:c")
 */

#endif /* UNIX	*/

#if SINTRAN3
#define cfree(x) free(x)

/*
 * Bildschirmsteuerung : Escape-Sequenzen und Makros (ND Tandberg-Terminal)
 * ========================================================================
 */

#define      CLRSCRN         "\33[2J\35"
#define      CLRTEOS         "\33[0J"
#define      CLRTEOL         "\33[0K"
#define      INSCHAR         "\33[@"
#define      DELCHAR         "\33[P"
#define      REVMODE         "\33[7m"
#define      NORMMODE        "\33[0m"

#undef       RIGHT
#define      RIGHT           CAN

#undef       HOME
#define      HOME            GS

#define      CURSOR_UP()     putchar(UP)
#define      CURSOR_DOWN()   putchar(DOWN)
#define      CURSOR_LEFT()   putchar(LEFT)
#define      CURSOR_RIGHT()  putchar(RIGHT)
#define      CURSOR_HOME()   putchar(HOME)
#define      beep()          putchar(BEL)

#define      CLEARSCREEN()   fputs(CLRSCRN,stdout)
#define      cleartoeol()    fputs(CLRTEOL,stdout)
#define      cleartoeos()    fputs(CLRTEOS,stdout)
#define      deletechar()    fputs(DELCHAR,stdout)
#define      insertchar()    fputs(INSCHAR,stdout)
#define      reverse()       fputs(REVMODE,stdout)
#define      noreverse()     fputs(NORMMODE,stdout)

/*
 * Funktionen:
 *    switchterm(mode)  : Terminalmodus umschalten         (--> "terminal:c")
 *    CURSORXY(row,col) : Cursor positionieren             (--> "terminal:c")
 *    putrev(c)         : Zeichen revers                   (--> "terminal:c")
 */

/*
 * Tastaturabfrage: Verschluesselung der Tastatur-Escape-Sequenzen (Tandberg)
 * ==========================================================================
 */

#define      MARK            300
#define      SHIFT_MARK      301
#define      FIELDK          302
#define      SHIFT_FIELDK    303
#define      PARA            304
#define      SHIFT_PARA      305
#define      SENT            306
#define      SHIFT_SENT      307
#define      WORDK           308
#define      SHIFT_WORD      309
#define      DELETE          310
#define      SHIFT_DELETE    311
#define      COPY            312
#define      SHIFT_COPY      313
#define      MOVE            314
#define      SHIFT_MOVE      315
#define      TAB             316
#define      SHIFT_TAB       317
#define      POINTS          318
#define      SHIFT_POINTS    319
#define      AAA             320
#define      SHIFT_AAA       321
#define      LELE            322
#define      SHIFT_LELE      323
#define      JUST            324
#define      SHIFT_JUST      325
#define      LERI            326
#define      SHIFT_LERI      327
#define      PGUP            328
#define      SHIFT_PGUP      329
#define      CANCEL          330
#define      SHIFT_CANCEL    331
#define      PGDOW           332
#define      SHIFT_PGDOW     333
#define      PGLE            334
#define      SHIFT_PGLE      335
#define      PGRI            336
#define      SHIFT_PGRI      337
#define      TABLEF          338
#define      SHIFT_TABLEF    339
#define      TABRIG          340
#define      SHIFT_TABRIG    341
#define      FUNC            342
#define      SHIFT_FUNC      343
#define      T_PRINT         344
#define      SHIFT_PRINT     345
#define      HELP            346
#define      SHIFT_HELP      347
#define      EXIT            348
#define      SHIFT_EXIT      349

/*
 * Tastaturabfrage: spezielle Belegungen einiger Funktionstasten (Tandberg)
 */

#define      DISPL           T_PRINT
#define      FDISPL          SHIFT_PRINT

#endif

#endif /* BATCH	*/

/* end of ctermio.h */
