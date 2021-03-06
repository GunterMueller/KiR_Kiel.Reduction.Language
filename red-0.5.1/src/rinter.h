/* $Log: rinter.h,v $
 * Revision 1.2  1992/12/16  12:50:06  rs
 * ANSI-Version
 *
 * Revision 1.1  1992/11/04  18:12:30  base
 * Initial revision
 * */
/* file inter.h */

#ifndef _rinter_h_included
#define _rinter_h_included

#include <unistd.h>  /* TB, 2.11.1992 */ 

extern post_mortem();      /* RS 02/11/92 */ 
  
/* wird includet in: rear.c, rinter.c rdesc.c */

/* Struktur des Backtrack Frames */

#define BT_SIZE      2   /* Anzahl der benoetigten "Register" fuer Backtracking */
#define SUBL_SIZE    2   /* Anzahl der benoetigten "Register" fuer Teillisten-Bindungen */
#define BT_ITEM_SIZE (BT_SIZE + SUBL_SIZE)

#define BTFRAME 0
#define BTPTR   0
#define BTSTART 1
#define BTEND   2
#define BTDESC  3

#define BT_FRAME(n)   ((n)*BT_ITEM_SIZE+BTFRAME)
#define BT_PTR(n)     ((n)*BT_ITEM_SIZE+BTPTR)
#define BT_START(n)   ((n)*BT_ITEM_SIZE+BTSTART)
#define BT_END(n)     ((n)*BT_ITEM_SIZE+BTEND)
#define BT_DESC(n)    ((n)*BT_ITEM_SIZE+BTDESC)

/* --- codierung der abstrakten befehle --- */

/* die instruktionen muessen fortlaufend nummeriert sein */
/* 1. um damit eine effiziente compilierung der interpreterschleife und */
/* 2. um eine einfache verwaltung von tabellen (z.B. mit symbolischen */
/*    namen fuer die instruktionen; der name der instruktion i steht an */
/*    der stelle i in der tabelle) */
/* zu ermoeglichen */
/* werden neue instruktionen eingefuehrt, so muessen die tabellen am ende */
/* dieser datei aktualisiert und code-teile in rear.c, die */
/* die codierung aller instruktionen kennen muessen ergaenzt werden. */

#define I_EXT      0 /* exit instruction */
#define I_PUSH     1 /* pushes */
#define I_PUSHC    2
#define I_PUSHARG  3
#define I_PUSHARG0 4
#define I_MOVE     5
#define I_DELTA1   6 /* primfunc applications */
#define I_DELTA2   7
#define I_DELTA3   8
#define I_DELTA4   9
#define I_ADD      10 /* operators */
#define I_SUB      11
#define I_MUL      12
#define I_DIV      13
#define I_MOD      14
#define I_EQ       15
#define I_NE       16
#define I_LT       17
#define I_LE       18
#define I_GT       19
#define I_GE       20
#define I_NEG      21
#define I_NOT      22
#define I_OR       23
#define I_AND      24
#define I_XOR      25
#define I_JFALSE   26 /* jumps, function calls, returns */
#define I_JTRUE    27
#define I_JCOND    28
#define I_PUSHRET  29
#define I_BETA     30
#define I_BETA0    31
#define I_BETA1    32
#define I_JTAIL    33
#define I_JTAIL0    34
#define I_JTAIL1    35
#define I_BETAQ    36
#define I_BETAQ0   37
#define I_BETAQ1   38
#define I_JTAILQ    39
#define I_JTAILQ0   40
#define I_JTAILQ1   41
#define I_MKLIST   42
#define I_FREE     43
#define I_FREE1    44
#define I_RTF      45
#define I_RTM      46
#define I_RTP      47
#define I_RTC      48
#define I_RTS      49
#define I_MKGCLOS  50 /* closures */
#define I_MKBCLOS  51
#define I_MKCCLOS  52
#define I_MKSCLOS  53
#define I_MKDCLOS  54
#define I_MKAP     55
#define I_APPLY    56
#define I_CASE     57 /* pattern matching */
#define I_WHEN     58
#define I_GUARD    59
#define I_BODY     60
#define I_NOMATCH  61
#define I_DUPARG   62
#define I_SAPPLY   63 /* specials */
#define I_SRET     64
#define I_MKTHEN   65 /* postprocessing */
#define I_MKELSE   66
#define I_MKCOND   67
#define I_LSEL     68 /* lazy lists */
#define I_EVAL     69
#define I_EVAL0    70
#define I_CONS     71 /* binary lists */
#define I_FCONS    72
#define I_FIRST    73
#define I_REST     74
#define I_UPDAT    75
#define I_MKILIST  76 /* directives */

/* jetzt der Verteilungs-Kram        RS ab 1.1.1993 */

#define I_PUSHH    77 /* distribute commands */
#define I_DIST     78
#define I_DISTEND  79
#define I_WAIT     80
#define I_POPH     81
#define I_DISTB    82

  /* ASM-Befehle fuer Messroutinen */

#define I_COUNT    83
#define I_MHPALLON 84
#define I_MHFREEON 85
#define I_MHPCMPON 86
#define I_MDESCALLON 87
#define I_MDESCFREON 88

#define I_MPROCREON 89
#define I_MSDISTEND 90
#define I_MPRSLEEON 91
#define I_MPRORUNON 92
#define I_MPROWAKON 93
#define I_MPROTERON 94

#define I_MCOMSEBON 95
#define I_MCOMSEEON 96
#define I_MCOMREBON 97
#define I_MCOMREEON 98

#define I_MSTCKPUON 99
#define I_MSTCKPPON 100
#define I_MSTKSEGAL 101
#define I_MSTKSEGFR 102

#define I_MSDISTON  103
#define I_MSNODIST  104

  /* Ende der ASM-Befehle fuer Messroutinen */

#define I_CHKFRAME  105      /* CHECKFRAME fuer den dynamischen Stackmanager */

/* Ende des Verteilungs-Krams                       */

#define UH_START    106

#define I_APPEND    UH_START+0  /*uh*/ /* List Comprehensions uh  */
#define I_BEGINZF   UH_START+1  /*uh*/
#define I_ENDZF     UH_START+2  /*uh*/
#define I_DROPP     UH_START+3  /*uh*/
#define I_JUMP      UH_START+4  /*uh*/
#define I_TFALSE    UH_START+5  /*uh*/
#define I_TTRUE     UH_START+6  /*uh*/
#define I_EXTRACT   UH_START+7  /*uh*/
#define I_DIM       UH_START+8  /*uh*/
#define I_TLIST     UH_START+9  /*uh*/
#define I_TZERO     UH_START+10 /*uh*/
#define I_MAKEZF    UH_START+11 /*uh*/
#define I_MAKEBOOL  UH_START+12 /*uh*/
#define I_MKWFRAME  UH_START+13 /*uh*/ /* Pattern-Matching Instructions */
#define I_RMWFRAME  UH_START+14  /*uh*/
#define I_BIND      UH_START+15  /*uh*/
#define I_MATCHC    UH_START+16  /*uh*/
#define I_ADVANCE   UH_START+17  /*uh*/
#define I_DEREF     UH_START+18  /*uh*/
#define I_DROP      UH_START+19  /*uh*/
#define I_PICK      UH_START+20  /*uh*/
#define I_SAVEPTR   UH_START+21  /*uh*/
#define I_RESTPTR   UH_START+22  /*uh*/
#define I_NESTLIST  UH_START+23  /*uh*/
#define I_MATCHLIST UH_START+24  /*uh*/ /* weitere Pattern-Matching Instruktionen */
#define I_JBOOL     UH_START+25  /*uh*/
#define I_MKCASE    UH_START+26  /*uh*/
#define I_MKIFRAME  UH_START+27  /*uh*/
#define I_BACKUP    UH_START+28  /*uh*/
#define I_ENDLIST   UH_START+29  /*uh*/
#define I_MATCHARB  UH_START+30  /*uh*/
#define I_STARTSUBL UH_START+31  /*uh*/
#define I_ENDSUBL   UH_START+32  /*uh*/
#define I_BINDSUBL  UH_START+33  /*uh*/
#define I_MKBTFRAME UH_START+34  /*uh*/
#define I_RMBTFRAME UH_START+35  /*uh*/
#define I_SAVEBT    UH_START+36  /*uh*/
#define I_RESTOREBT UH_START+37  /*uh*/
#define I_INITBT    UH_START+38  /*uh*/
#define I_MATCHIN   UH_START+39  /*uh*/
#define I_ATSTART   UH_START+40  /*uh*/
#define I_ATEND     UH_START+41  /*uh*/
#define I_FETCH     UH_START+42  /*uh*/
#define I_MATCHINT  UH_START+43  /*uh*/
#define I_MATCHBOOL UH_START+44  /*uh*/
#define I_MATCHNIL  UH_START+45  /*uh*/
#define I_MATCHPRIM UH_START+46  /*uh*/
#define I_MATCHTRUE UH_START+47  /*uh*/
#define I_MATCHFALSE UH_START+48  /*uh*/

#define I_DESC     UH_START +49 /* directives */
#define I_TABSTART UH_START +50 
#define I_SYMTAB   UH_START +51 
#define I_END      UH_START +52 
#define I_CFA1     UH_START +53 /* loleila specials */
#define I_CFA2     UH_START +54 
#define I_CFA3     UH_START +55 
#define I_CFA4     UH_START +56 
#define I_CFA5     UH_START +57 
#define I_CFA6     UH_START +58 
#define I_CFA7     UH_START +59 

#define I_STOP     1000

/* synonyme */

#define I_COND     I_PUSHRET
#define I_TAIL     I_JTAIL
#define I_TAIL0    I_JTAIL0
#define I_TAIL1    I_JTAIL1
#define I_TAILQ    I_JTAILQ
#define I_TAILQ0   I_JTAILQ0
#define I_TAILQ1   I_JTAILQ1

/* --- codebereich --- */

typedef struct {
  INSTR *codvec;   /* zeiger auf den anfang des codebereichs */
  int    codsiz;   /* gesamtlaenge des codebereiches */
  INSTR *codptr;   /* zeiger in den codebereich (laufzeiger) */
  int    codcnt;   /* anzahl der freien zellen des codebereichs */
  INSTR *symtab;   /* zeiger auf den anfang der symboltabelle */
  int    symlen;   /* laenge der symboltabelle (ohne I_SYMTAB len) */
} T_CODEDESCR;

#define CODEVEC (codedesc.codvec)
#define CODESIZ (codedesc.codsiz)
#define CODEPTR (codedesc.codptr)
#define CODECNT (codedesc.codcnt)
#define SYMBTAB (codedesc.symtab)
#define SYMBLEN (codedesc.symlen)

typedef struct {
  int    instruction;
  char   *name;
  int    nparams;
  int    paramtype;
} INSTR_DESCR;

#define ZERO_PAR 0   /* no parameters */
#define NUM_PAR  1   /* numeric parameters */
#define ADDR_PAR 2   /* address parameter */
#define ATOM_PAR 3   /* atom parameter */
#define FUNC_PAR 4   /* primititve func parameter */
#define DESC_PAR 5   /* descriptor parameter */

/* der rest wird nur in rdesc.c includet */
/* der kram steht hier, damit anderungen */
/* an der codierung der instruktionen hier */
/* gleich mit nachvollzogen werden koennen */

#ifdef RNCMESSAGE
#define RDESC
#endif

#ifdef RDESC 

/* instruction table: for debugging purposes */

INSTR_DESCR instr_tab[] = {
/* instruction  name         nparams    paramtype */
/* ---------------------------------------------- */
   {I_EXT       ,"EXT"       ,0         ,ZERO_PAR}
  ,{I_PUSH      ,"PUSH"      ,1         ,DESC_PAR}
  ,{I_PUSHC     ,"PUSHC"     ,1         ,ATOM_PAR}
  ,{I_PUSHARG   ,"PUSHARG"   ,1         ,NUM_PAR}
  ,{I_PUSHARG0  ,"PUSHARG0"  ,0         ,ZERO_PAR}
  ,{I_MOVE      ,"MOVE"      ,0         ,ZERO_PAR}
  ,{I_DELTA1    ,"DELTA1"    ,1         ,FUNC_PAR}
  ,{I_DELTA2    ,"DELTA2"    ,1         ,FUNC_PAR}
  ,{I_DELTA3    ,"DELTA3"    ,1         ,FUNC_PAR}
  ,{I_DELTA4    ,"DELTA4"    ,1         ,FUNC_PAR}
  ,{I_ADD       ,"ADD"       ,0         ,ZERO_PAR}
  ,{I_SUB       ,"SUB"       ,0         ,ZERO_PAR}
  ,{I_MUL       ,"MUL"       ,0         ,ZERO_PAR}
  ,{I_DIV       ,"DIV"       ,0         ,ZERO_PAR}
  ,{I_MOD       ,"MOD"       ,0         ,ZERO_PAR}
  ,{I_EQ        ,"EQ"        ,0         ,ZERO_PAR}
  ,{I_NE        ,"NE"        ,0         ,ZERO_PAR}
  ,{I_LT        ,"LT"        ,0         ,ZERO_PAR}
  ,{I_LE        ,"LE"        ,0         ,ZERO_PAR}
  ,{I_GT        ,"GT"        ,0         ,ZERO_PAR}
  ,{I_GE        ,"GE"        ,0         ,ZERO_PAR}
  ,{I_NEG       ,"NEG"       ,0         ,ZERO_PAR}
  ,{I_NOT       ,"NOT"       ,0         ,ZERO_PAR}
  ,{I_OR        ,"OR"        ,0         ,ZERO_PAR}
  ,{I_AND       ,"AND"       ,0         ,ZERO_PAR}
  ,{I_XOR       ,"XOR"       ,0         ,ZERO_PAR}
  ,{I_JFALSE    ,"JFALSE"    ,1         ,ADDR_PAR}
  ,{I_JTRUE     ,"JTRUE"     ,1         ,ADDR_PAR}
  ,{I_JCOND     ,"JCOND"     ,1         ,ADDR_PAR}
  ,{I_PUSHRET   ,"PUSHRET"   ,1         ,ADDR_PAR}
  ,{I_BETA      ,"BETA"      ,1         ,ADDR_PAR}
  ,{I_BETA0     ,"BETA0"     ,1         ,ADDR_PAR}
  ,{I_BETA1     ,"BETA1"     ,1         ,ADDR_PAR}
  ,{I_JTAIL     ,"JTAIL"     ,1         ,ADDR_PAR}
  ,{I_JTAIL0    ,"JTAIL0"    ,1         ,ADDR_PAR}
  ,{I_JTAIL1    ,"JTAIL1"    ,1         ,ADDR_PAR}
  ,{I_BETAQ     ,"BETAQ"     ,1         ,ADDR_PAR}
  ,{I_BETAQ0    ,"BETAQ0"    ,1         ,ADDR_PAR}
  ,{I_BETAQ1    ,"BETAQ1"    ,1         ,ADDR_PAR}
  ,{I_JTAILQ    ,"JTAILQ"    ,1         ,ADDR_PAR}
  ,{I_JTAILQ0   ,"JTAILQ0"   ,1         ,ADDR_PAR}
  ,{I_JTAILQ1   ,"JTAILQ1"   ,1         ,ADDR_PAR}
  ,{I_MKLIST    ,"MKLIST"    ,1         ,NUM_PAR}
  ,{I_FREE      ,"FREE"      ,1         ,NUM_PAR}
  ,{I_FREE1     ,"FREE1"     ,0         ,ZERO_PAR}
  ,{I_RTF       ,"RTF"       ,0         ,ZERO_PAR}
  ,{I_RTM       ,"RTM"       ,0         ,ZERO_PAR}
  ,{I_RTP       ,"RTP"       ,1         ,DESC_PAR}
  ,{I_RTC       ,"RTC"       ,1         ,NUM_PAR}
  ,{I_RTS       ,"RTS"       ,0         ,ZERO_PAR}
  ,{I_MKGCLOS   ,"MKGCLOS"   ,1         ,NUM_PAR}
  ,{I_MKBCLOS   ,"MKBCLOS"   ,2         ,NUM_PAR}
  ,{I_MKCCLOS   ,"MKCCLOS"   ,2         ,NUM_PAR}
  ,{I_MKSCLOS   ,"MKSCLOS"   ,2         ,NUM_PAR}
  ,{I_MKDCLOS   ,"MKDCLOS"   ,2         ,NUM_PAR}
  ,{I_MKAP      ,"MKAP"      ,1         ,NUM_PAR}
  ,{I_APPLY     ,"APPLY"     ,1         ,NUM_PAR}
  ,{I_CASE      ,"CASE"      ,1         ,ADDR_PAR}
  ,{I_WHEN      ,"WHEN"      ,1         ,DESC_PAR}
  ,{I_GUARD     ,"GUARD"     ,1         ,ADDR_PAR}
  ,{I_BODY      ,"BODY"      ,1         ,ADDR_PAR}
  ,{I_NOMATCH   ,"NOMATCH"   ,1         ,DESC_PAR}
  ,{I_DUPARG    ,"DUPARG"    ,1         ,NUM_PAR}
  ,{I_SAPPLY    ,"SAPPLY"    ,0         ,ZERO_PAR}
  ,{I_SRET      ,"SRET"      ,0         ,ZERO_PAR}
  ,{I_MKTHEN    ,"MKTHEN"    ,0         ,ZERO_PAR}
  ,{I_MKELSE    ,"MKELSE"    ,0         ,ZERO_PAR}
  ,{I_MKCOND    ,"MKCOND"    ,0         ,ZERO_PAR}
  ,{I_LSEL      ,"LSEL"      ,0         ,ZERO_PAR}
  ,{I_EVAL      ,"EVAL"      ,0         ,ZERO_PAR}
  ,{I_EVAL0     ,"EVAL0"     ,0         ,ZERO_PAR}
  ,{I_CONS      ,"CONS"      ,0         ,ZERO_PAR}
  ,{I_FCONS     ,"FCONS"     ,0         ,ZERO_PAR}
  ,{I_FIRST     ,"FIRST"     ,0         ,ZERO_PAR}
  ,{I_REST      ,"REST"      ,0         ,ZERO_PAR}
  ,{I_UPDAT     ,"UPDAT"     ,0         ,ZERO_PAR}
  ,{I_MKILIST  ,"MKILIST"    ,1         ,NUM_PAR}  /* directives */

/* Verteilungs- und Messkram RS ab 1.1.1993 */

  ,{I_PUSHH     ,"PUSHH"     ,1         ,NUM_PAR}
  ,{I_DIST      ,"DIST"      ,4         ,NUM_PAR}
  ,{I_DISTEND   ,"DISTEND"  ,0         ,ZERO_PAR}
  ,{I_WAIT      ,"WAIT"      ,1         ,NUM_PAR}
  ,{I_POPH      ,"POPH"      ,0         ,ZERO_PAR}
  ,{I_DISTB     ,"DISTB"     ,4         ,NUM_PAR}

  /* Messkram : */

  ,{I_COUNT     ,"COUNT"     ,1         ,NUM_PAR}    /* count ASM-instructions */
  ,{I_MHPALLON  ,"MHPALLON"  ,0         ,ZERO_PAR}   /* turn on heap alloc measure */
  ,{I_MHFREEON  ,"MHFREEON"  ,0         ,ZERO_PAR}   /* turn on heap free measure */
  ,{I_MHPCMPON  ,"MHPCMPON"  ,0         ,ZERO_PAR}   /* turn on heap compaction measure */
  ,{I_MDESCALLON,"MDESCALLON",0         ,ZERO_PAR}   /* turn on descriptor alloc measure */
  ,{I_MDESCFREON,"MDESCFREON",0         ,ZERO_PAR}   /* turn on descriptor free measure */

  ,{I_MPROCREON ,"MPROCREON" ,0         ,ZERO_PAR}    /* turn on process creation measure */
  ,{I_MSDISTEND ,"MSDISTEND" ,0         ,ZERO_PAR}    /* process distend measure */
  ,{I_MPRSLEEON ,"MPRSLEEON" ,0         ,ZERO_PAR}    /* turn on process suspend measure */
  ,{I_MPRORUNON ,"MPRORUNON" ,0         ,ZERO_PAR}    /* turn on process run measure */
  ,{I_MPROWAKON, "MPROWAKON" ,0         ,ZERO_PAR}    /* turn on process wakeup measure */
  ,{I_MPROTERON, "MPROTERON" ,0         ,ZERO_PAR}    /* turn on process terminate measure */
  ,{I_MCOMSEBON ,"MCOMSEBON" ,0         ,ZERO_PAR}    /* turn on communication send measure */
  ,{I_MCOMSEEON ,"MCOMSEEON" ,0         ,ZERO_PAR}
  ,{I_MCOMREBON ,"MCOMREBON" ,0         ,ZERO_PAR}    /* turn on communication receive message */
  ,{I_MCOMREEON ,"MCOMREEON" ,0         ,ZERO_PAR}
  ,{I_MSTCKPUON ,"MSTCKPUON" ,0         ,ZERO_PAR}    /* turn on stack-push measure */
  ,{I_MSTCKPPON ,"MSTCKPPON" ,0         ,ZERO_PAR}    /* turn on stack-pop measure */
  ,{I_MSTKSEGAL ,"MSTKSEGAL" ,0         ,ZERO_PAR}    /* turn on stack-seg alloc measure */
  ,{I_MSTKSEGFR ,"MSTKSEGFR" ,0         ,ZERO_PAR}    /* turn on stack-seg free measure */
  ,{I_MSDISTON  ,"MSDISTON"  ,0         ,ZERO_PAR}    /* turn on proc-distribute measure */
  ,{I_MSNODIST  ,"MSNODIST"  ,0         ,ZERO_PAR}    /* mark nodist expressions */

  ,{I_CHKFRAME  ,"CHKFRAME"  ,1         ,NUM_PAR}     /* checks for enough stackspace */

/* Ende Verteilungs- und Messkram RS ab 1.1.1993 */

  ,{I_APPEND    ,"APPEND"    ,0         ,ZERO_PAR} /*uh*/
  ,{I_BEGINZF   ,"BEGINZF"   ,0         ,ZERO_PAR} /*uh*/
  ,{I_ENDZF     ,"ENDZF"     ,0         ,ZERO_PAR} /*uh*/
  ,{I_DROPP     ,"DROPP"     ,0         ,ZERO_PAR} /*uh*/
  ,{I_JUMP      ,"JUMP"      ,1         ,ADDR_PAR} /*uh*/
  ,{I_TFALSE    ,"TFALSE"    ,1         ,ADDR_PAR} /*uh*/
  ,{I_TTRUE     ,"TTRUE"     ,1         ,ADDR_PAR} /*uh*/
  ,{I_EXTRACT   ,"EXTRACT"   ,0         ,ZERO_PAR} /*uh*/
  ,{I_DIM       ,"DIM"       ,0         ,ZERO_PAR} /*uh*/
  ,{I_TLIST     ,"TLIST"     ,1         ,ADDR_PAR} /*uh*/
  ,{I_TZERO     ,"TZERO"     ,1         ,ADDR_PAR} /*uh*/
  ,{I_MAKEZF    ,"MAKEZF"    ,1         ,NUM_PAR}  /*uh*/
  ,{I_MAKEBOOL  ,"MAKEBOOL"  ,1         ,NUM_PAR}  /*uh*/
  ,{I_MKWFRAME  ,"MAKEWFRAME",1         ,NUM_PAR}  /*uh*/
  ,{I_RMWFRAME  ,"REMWFRAME" ,1         ,NUM_PAR}  /*uh*/
  ,{I_BIND      ,"BIND"      ,1         ,NUM_PAR}  /*uh*/
  ,{I_MATCHC    ,"MATCHC"    ,3         ,NUM_PAR}  /*uh*/
  ,{I_ADVANCE   ,"ADVANCE"   ,1         ,NUM_PAR}  /*uh*/
  ,{I_DEREF     ,"DEREF"     ,0         ,ZERO_PAR} /*uh*/
  ,{I_DROP      ,"DROP"      ,0         ,ZERO_PAR} /*uh*/
  ,{I_PICK      ,"PICK"      ,1         ,NUM_PAR}  /*uh*/
  ,{I_SAVEPTR   ,"SAVEPTR"   ,1         ,NUM_PAR}  /*uh*/
  ,{I_RESTPTR   ,"RESTPTR"   ,1         ,NUM_PAR}  /*uh*/
  ,{I_NESTLIST  ,"NESTLIST"  ,0         ,ZERO_PAR} /*uh*/
  ,{I_MATCHLIST ,"MATCHLIST" ,3         ,NUM_PAR}  /*uh*/
  ,{I_JBOOL     ,"JBOOL"     ,2         ,NUM_PAR}  /*uh*/
  ,{I_MKCASE    ,"MKCASE"    ,1         ,DESC_PAR} /*uh*/
  ,{I_MKIFRAME  ,"MAKEIFRAME",1         ,NUM_PAR}  /*uh*/
  ,{I_BACKUP    ,"BACKUP"    ,1         ,NUM_PAR}  /*uh*/
  ,{I_ENDLIST   ,"ENDLIST"   ,0         ,ZERO_PAR} /*uh*/
  ,{I_MATCHARB  ,"MATCHARB"  ,3         ,NUM_PAR}  /*uh*/
  ,{I_STARTSUBL ,"STARTSUBL" ,2         ,NUM_PAR}  /*uh*/
  ,{I_ENDSUBL   ,"ENDSUBL"   ,1         ,NUM_PAR}  /*uh*/
  ,{I_BINDSUBL  ,"BINDSUBL"  ,4         ,NUM_PAR}  /*uh*/
  ,{I_MKBTFRAME ,"MKBTFRAME" ,1         ,NUM_PAR}  /*uh*/
  ,{I_RMBTFRAME ,"RMBTFRAME" ,1         ,NUM_PAR}  /*uh*/
  ,{I_SAVEBT    ,"SAVEBT"    ,1         ,NUM_PAR}  /*uh*/
  ,{I_RESTOREBT ,"RESTOREBT" ,1         ,NUM_PAR}  /*uh*/
  ,{I_INITBT    ,"INITBT"    ,2         ,NUM_PAR}  /*uh*/
  ,{I_MATCHIN   ,"MATCHIN"   ,3         ,NUM_PAR}  /*uh*/
  ,{I_ATSTART   ,"ATSTART"   ,2         ,NUM_PAR}  /*uh*/
  ,{I_ATEND     ,"ATEND"     ,2         ,NUM_PAR}  /*uh*/
  ,{I_FETCH     ,"FETCH"     ,0         ,ZERO_PAR} /*uh*/
  ,{I_MATCHINT  ,"MATCHINT"  ,3         ,NUM_PAR}  /*uh*/
  ,{I_MATCHBOOL ,"MATCHBOOL" ,3         ,NUM_PAR}  /*uh*/
  ,{I_MATCHNIL  ,"MATCHNIL"  ,2         ,NUM_PAR}  /*uh*/
  ,{I_MATCHPRIM ,"MATCHPRIM" ,3         ,NUM_PAR}  /*uh*/
  ,{I_MATCHTRUE ,"MATCHTRUE" ,2         ,NUM_PAR}  /*uh*/
  ,{I_MATCHFALSE,"MATCHFALSE",2         ,NUM_PAR}  /*uh*/

  ,{I_DESC      ,"DESC"      ,1         ,DESC_PAR} /* directives */
  ,{I_TABSTART  ,"TABSTART"  ,1         ,ADDR_PAR}
  ,{I_SYMTAB    ,"SYMTAB"    ,1         ,NUM_PAR}
  ,{I_END       ,"END"       ,0         ,ZERO_PAR}
  ,{I_CFA1      ,"CFA1"      ,0         ,ZERO_PAR} /* loleilea specials */
  ,{I_CFA2      ,"CFA2"      ,0         ,ZERO_PAR}
  ,{I_CFA3      ,"CFA3"      ,0         ,ZERO_PAR}
  ,{I_CFA4      ,"CFA4"      ,0         ,ZERO_PAR}
  ,{I_CFA5      ,"CFA5"      ,0         ,ZERO_PAR}
  ,{I_CFA6      ,"CFA6"      ,0         ,ZERO_PAR}
  ,{I_CFA7      ,"CFA7"      ,0         ,ZERO_PAR}
};

#endif

#ifdef RNCMESSAGE
#undef RDESC
#endif

#ifdef RDESC

/* einige nuetzliche makros und funktionen */

#define CHECK_INSTR(in) if (in < 0 \
                        ||  in >= (sizeof(instr_tab) / sizeof(INSTR_DESCR)) \
                        ||  in != instr_tab[in].instruction) \
                          /* post_mortem("CHECK_INSTR: invalid instr_tab (rinter.h)") */ \
                          fprintf(stderr,"\n Warning: (rinter.h, CHECK_INSTR) invalid instruction/instr_tab. Wait ... "), sleep(2)

/* --- check_instr : gueltige instruktion? --- */
int check_instr(i)    /* int eingefuegt von RS 02/11/92 */ 
INSTR i;
{
  CHECK_INSTR(i);
  return (0);   /* RS 02/11/92 */ 
}

/* --- instrindex : findet tabellenindex einer instruktion --- */

int instrindex(i)
INSTR i;
{
  int j;

  CHECK_INSTR(i);
  if (i == instr_tab[i].instruction)
    return(i);
  for (j = 0; j < (sizeof(instr_tab) / sizeof(INSTR_DESCR)); j++)
    if (i == instr_tab[j].instruction)
      return(j);
  post_mortem("instrindex: invalid instruction/instr_tab (rinter.h)");
  return (0); /* RS 02/11/92 */ 
}

/* --- instrname : erzeugt mnemonic fuer instruktion --- */

char *instrname(i)
INSTR i;
{
  return(instr_tab[instrindex(i)].name);
}

/* --- prinstr : erzeugt mnemonic fuer instruktion incl. parameter --- */

char *prinstr(pti)
int *pti;
{
  static char str[80];                /* das ist gefaehrlich! */
  char *stelname();                   /* rear.c */
  int i = instrindex(*pti);
  switch (instr_tab[i].paramtype) {
    case ZERO_PAR:
      sprintf(str,"%s",instr_tab[i].name);
      break;
    case NUM_PAR:
      if (instr_tab[i].nparams == 1)
        sprintf(str,"%s(%d)",instr_tab[i].name,pti[1]);
      else
      if (instr_tab[i].nparams == 2)
        sprintf(str,"%s(%d,%d)",instr_tab[i].name,pti[1],pti[2]);
      else post_mortem("prinstr: invalid nparams");
      break;
    case ADDR_PAR:
      if (instr_tab[i].nparams == 1)
        sprintf(str,"%s(label(%x))  # desc: %08x",instr_tab[i].name,pti[1],((int*)pti[1])[-1]);
      else post_mortem("prinstr: invalid nparams");
      break;
    case ATOM_PAR:
    case FUNC_PAR:
    case DESC_PAR:
      if (instr_tab[i].nparams == 1)
        sprintf(str,"%s(%s)",instr_tab[i].name,stelname(pti[1]));
      else post_mortem("prinstr: invalid nparams");
      break;
    default:
      *str = '\0' ;
  }
  return(str);
}

#endif /* RDESC       auskommentiert RS 30.11.1992 */

/* end of file */

#endif
