
/* ************************************************************************ */
/*                                                                          */
/*  os-send.c                                                               */
/*  """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""" */
/*                                                                          */
/*    - external:                                                           */
/*        OS_send_pf                                                        */
/*        OS_receive_pf                                                     */
/*        OS_send                                                           */
/*        OS_receive                                                        */
/*        OS_create_list                                                    */
/*        release_list                                                      */
/*    - internal:                                                           */
/*        print_desc_c_t                                                    */
/*        write_intern                                                      */
/*        already_sended                                                    */
/*        send_header                                                       */
/*        send_descr                                                        */
/*        send_data                                                         */
/*        send_list                                                         */
/*        send_heapelem                                                     */
/*        send_code                                                         */
/*        sendrec                                                           */
/*        read_intern                                                       */
/*        already_received                                                  */
/*        receive_header                                                    */
/*        receive_descr                                                     */
/*        receive_data                                                      */
/*        receive_list                                                      */
/*        receive_heapelem                                                  */
/*        receive_code                                                      */
/*        receiverec                                                        */
/*    - Aufgerufen durch:  ?? in os-dist.c, os-comm.c                       */
/*                                                                          */
/* ************************************************************************ */

/* last update  by              why
   20.11.88     jd/ks           send / receive  ( Anfangsversion )
   30.1. 89     Jan Dannemann   Einbau in kred
   13.2. 89     Jan Dannemann   Einbau des dbug-Paketes
   04.04.89                     Listenfunktionen
   27.09.89     jd/ks           Preloaded Functions
   13.10.89                     StandAlone-Version
   copied       by     to
*/


/*
  ! os-send.c verwendet als einzige OS-Datei ein Compiler-Flag "STANDALONE".
  ! Wenn STANDALONE definiert ist, dann wird diese Datei fuer die Verwendung
  ! ausserhalb des OS uebersetzt. Es wird dann noch benoetigt:
  !     dbug.h  und dbug.c              fuer die DBUG-Makros
  !     rheapty.ch7                     fuer den Deskriptor TY_SPEC.
  ! Die Version der Damen muss mit dem zusaetzlichen Compiler-Flag "DREDUMA=1"
  ! uebersetzt werden!
  ! Die im OS angebotene Moeglichkeit fuer PreLoaded Functions ist bei
  ! STANDALONE blockiert, die Blockade kann aber leicht aufgehoben werden,
  ! falls dies gewuenscht wird.
*/


#ifndef STANDALONE

#include "os.h"
#include "os-meas.h"
#include "rstelem.h"                /* fuer den I-Stack (?)         */

#else   /* also STANDALONE */

#define ERROR -1
#define NOERROR 1

#endif  /* keine STANDALONE - Version */


#include <sys/errno.h>
#include <setjmp.h>
#include "rstdinc.h"
#include "rstackty.h"
#include "rheapty.h"
#include "rextern.h"                /* Standarddeskriptoren         */
#include "rinstr.h"                 /* fuer send_code, receive_code */
#include "dbug.h"
#include "case.h"

/* ************************************************************************ */
/*                                                                          */
/*  Externe Funktionen                                                      */
/*                                                                          */
/* ************************************************************************ */

extern PTR_DESCRIPTOR newdesc();                        /* in os-heap.c     */
extern int inq_desmax();                                /* in os-heap.c     */
#if DEBUG
extern int isdesc();                                    /* in os-heap.c     */
#endif
extern int newheap();                                   /* in os-heap.c     */
extern void freeheap();                                 /* in os-heap.c     */
extern void freedesc();                                 /* in os-heap.c     */


/* ************************************************************************ */
/*                                                                          */
/*  Externe Variablen                                                       */
/*                                                                          */
/* ************************************************************************ */

extern char * sys_errlist[];


/* ************************************************************************ */
/*                                                                          */
/*  Forward declarations                                                    */
/*                                                                          */
/* ************************************************************************ */

int w_n_ints();
int r_n_ints();
static PTR_DESCRIPTOR receiverec();
static void sendrec();
static void write_intern();
static void read_intern();


/* ************************************************************************ */
/*                                                                          */
/*  Defines und Konstanten                                                  */
/*                                                                          */
/* ************************************************************************ */

#define NOT_SENDED -1
#define TABELLENGROESSE 500     /* Die Groesse wird dynamisch vergroessert! */
#define INTS_TO_READ 256        /* Zahl der Integer, die auf einmal gelesen */
                                /* oder geschrieben werden soll.            */
#define NEXT_TAB_ENTRY \
  *((PTR_DESCRIPTOR*)(R_LIST((*local_tab),ptdv)) + local_filled++)
#define post_mortem_intern() longjmp(_error_label,1)


/* ************************************************************************ */
/*                                                                          */
/*  globale Variablen                                                       */
/*                                                                          */
/* ************************************************************************ */

FCTPTR write_n_ints = w_n_ints;
FCTPTR read_n_ints  = r_n_ints;


/* ************************************************************************ */
/*                                                                          */
/*  Statische globale Variablen                                             */
/*                                                                          */
/* ************************************************************************ */

/*
  Damit mit diesen globalen Variablen gearbeitet werden kann, muss gewaehr-
  leistet sein, dass nur jeweils ein SEND oder RECEIVE zur Zeit ablaeuft.
  Sollen mehrere Inkarnationen von SEND oder RECEIVE gleichzeitig aktiv
  sein, so muessen die Variablen als Var-Parameter an die Prozeduren
  Sendrec und Receiverec uebergeben werden.
*/

#ifndef STANDALONE
static PTR_DESCRIPTOR pf_tab;   /* Tabelle fuer die "preloaded'ten" Desks.  */
static int pf_filled;           /* Zahl der Deskriptoren in der Tabelle.    */
#endif

static PTR_DESCRIPTOR local_tab;/* Tabelle fuer die "normal" verschickten   */
                                /* Deskriptoren                             */
static int local_filled;        /* Zahl der Elemente in local_tab           */

static jmp_buf _error_label;    /* fuer longjmp bei post_mortem_intern      */
static int packetno;            /* zaehlt die Aufrufe von .._intern         */
static int kdesc;               /* zaehlt das Versenden von bekannten Desk. */
static int ndesc;               /* zaehlt das Versenden von neuen Deskript. */

static int debug = 0;           /* Sollen Pakete mit Debug-Informationen    */
                                /* gelesen werden?                          */
                                /* gesetzt in _OS_receive (OS_receive)      */

/* ************************************************************************ */
/*                                                                          */
/*  Funktionen zum Schreiben und Lesen auf Dateien                          */
/*  """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""" */
/*                                                                          */
/* ************************************************************************ */

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  w_n_ints  -  Schreibt n integer in Hostdarstellung in eine Datei.       */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int w_n_ints( fd, arg, n )
  int fd;
  int * arg;
  int n;
{
  int wb;   /* Written bytes.   */

  DBUG_ENTER( "w_n_ints       " );
  if ( (wb = write( fd, arg, n * sizeof(int) ) ) != n * sizeof(int) )
  {
    DBUG_PRINT
    (
      "ERRS",
      ("ERROR : writing %d integer(s) : %s !", n, sys_errlist[errno])
    );
    DBUG_RETURN( wb / sizeof(int) );
  }
  DBUG_PRINT( "SEND", ("Wrote %d integer(s).", n) );
  DBUG_RETURN( n );
}  /* w_n_ints */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  r_n_ints  -  Liest n integer in Hostdarstellung aus einer Datei.        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int r_n_ints( fd, arg, n )
  int fd;
  int * arg;
  int n;
{
  int rb;   /* Read bytes.  */

  DBUG_ENTER( "r_n_ints       " );
  if ( (rb = read( fd, arg, n * sizeof(int) ) ) != n * sizeof(int) )
  {
    DBUG_PRINT
    (
      "ERRS",
      ("ERROR : writing %d integer(s) : %s !", n, sys_errlist[errno])
    );
    DBUG_RETURN( rb / sizeof(int) );
  }
  DBUG_PRINT( "SEND", ("Read %d integer(s).", n) );
  DBUG_RETURN( n );
}  /* r_n_ints */


/* ************************************************************************ */
/*                                                                          */
/*  Funktionen zum Anlegen und Beseitigen von Listen.                       */
/*  """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""" */
/*                                                                          */
/* ************************************************************************ */

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  OS_create_list - Erzeugen einer Liste im Heap                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */

PTR_DESCRIPTOR OS_create_list( size )
  int size;

  /*
    legt im Heap einen Listendeskriptor und eine angehaengte Liste der
    Groesse size an.
    Klappt dies, wird der Deskriptorzeiger zurueckgeliefert, sonst NULL.
  */

{
  PTR_DESCRIPTOR desc;

  DBUG_ENTER( "OS_create_list    ");
  desc = newdesc();      /* zunaechst einmal einen Deskriptor beschaffen */
  if ( desc == (PTR_DESCRIPTOR) 0 )
  {
    DBUG_PRINTE( "ERRS", ("ERROR : can't allocate list-descriptor !") );
    DBUG_RETURN( (PTR_DESCRIPTOR) 0 );
  }
  LI_DESC_MASK( desc, 1, C_LIST, TY_UNDEF );
  L_LIST( (*desc), dim) = (COUNT) size;
  if ( !newheap( size, A_LIST( (*desc), ptdv ) ))
  {
    freedesc( desc );
    DBUG_PRINTE
    (
      "ERRS",
      ("ERROR : can't allocate heapspace for %d elements !", size)
    );
    DBUG_RETURN( (PTR_DESCRIPTOR) 0 );
  }
  DBUG_RETURN( desc );
}  /* OS_create_list */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  release_list  -  Liste wieder freigeben                                 */
/*                                                                          */
/* ------------------------------------------------------------------------ */

void release_list( l )
  PTR_DESCRIPTOR l;

  /*
    gibt die mit OS_create_list erzeugte Liste wieder frei.
    Es ist noch nicht sicher, ob eine derartige Liste gesharet wird, deshalb
    wird sicherheitshalber eine Ref-Count-Abfrage vorgenommen.
  */

{
  DBUG_ENTER( "release_list   ");

  if ( R_DESC( (*l), ref_count ) > 1 )           /* UEBEL, UEBEL!    */
  {
    DBUG_PRINT
    (
      "HEAP",
      ("WARNING : Ref-count of desc %8.8x is > 1 !", *l)
    );
    DEC_REFCNT( l );
  }
  else
  {
    freeheap( R_LIST( (*l), ptdv) );  /* Platz zurueckgeben           */
    freedesc( l );
  }
  DBUG_VOID_RETURN;
}  /* release_list */


#if DEBUG

/* ************************************************************************ */
/*                                                                          */
/*  MISC                                                                    */
/*  """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""" */
/*                                                                          */
/* ************************************************************************ */

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  print_desc_c_t  -  Klasse und Typ eines Deskriptors ausgeben            */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void print_desc_c_t( to_send, desc )
  BOOLEAN to_send;
  PTR_DESCRIPTOR desc;

{
  char * tys, * cas;

  switch ( R_DESC( (*desc), class ) )
  {
  case C_SCALAR      : cas = "C_SCALAR";        break;
  case C_DIGIT       : cas = "C_DIGIT";         break;
  case C_LIST        : cas = "C_LIST";          break;
  case C_MATRIX      : cas = "C_MATRIX";        break;
  case C_EXPRESSION  : cas = "C_EXPRESSION";    break;
  case C_CONSTANT    : cas = "C_CONSTANT";      break;
  case C_VECTOR      : cas = "C_VECTOR";        break;
  case C_TVECTOR     : cas = "C_TVECTOR";       break;
#if !DREDUMA
  case C_SWAPHEAP    : cas = "C_SWAPHEAP";      break;
  case C_PROCESS     : cas = "C_PROCESS";       break;
  case C_STREAM      : cas = "C_STREAM";        break;
  case C_GFIELD      : cas = "C_GFIELD";        break;
  case C_MERGE_FIELD : cas = "C_MERGE_FIELD";   break;
  case C_SEMAPHORE   : cas = "C_SEMAPHORE";     break;
  case C_MERGE_STREAM: cas = "C_MERGE_STREAM";  break;
  case C_EDIT        : cas = "C_EDIT";          break;
#endif  /* !DREDUMA */
  default            : cas = "unknown!";        break;
  }  /* CASE */
  switch ( R_DESC( (*desc), type ) )
  {
  case TY_FREE     : tys = "TY_FREE";           break;
  case TY_UNDEF    : tys = "TY_UNDEF";          break;
  case TY_INTEGER  : tys = "TY_INTEGER";        break;
  case TY_REAL     : tys = "TY_REAL";           break;
  case TY_BOOL     : tys = "TY_BOOL";           break;
  case TY_STRING   : tys = "TY_STRING";         break;
  case TY_DIGIT    : tys = "TY_DIGIT";          break;
  case TY_SUB      : tys = "TY_SUB";            break;
  case TY_COND     : tys = "TY_COND";           break;
  case TY_EXPR     : tys = "TY_EXPR";           break;
  case TY_VAR      : tys = "TY_VAR";            break;
  case TY_NAME     : tys = "TY_NAME";           break;
  case TY_MATRIX   : tys = "TY_MATRIX";         break;
  case TY_SWITCH   : tys = "TY_SWITCH";         break;
  case TY_MATCH    : tys = "TY_MATCH";          break;
  case TY_NOMAT    : tys = "TY_NOMAT";          break;
  case TY_LREC     : tys = "TY_LREC";           break;
  case TY_LREC_IND : tys = "TY_LREC_IND";       break;
  case TY_SPEC     : tys = "TY_SPEC";           break;
#if DREDUMA
  case TY_FNAME    : tys = "TY_FNAME";          break;
#endif
#if !DREDUMA
  case TY_REC      : tys = "TY_REC";            break;
  case TY_LREC_ARGS: tys = "TY_LREC_ARGS";      break;
#endif
  default          : tys = "unknown!";          break;      
  }  /* CASE */
  if ( to_send )
  {
    DBUG_PRINT( "SEND", ("Sending   %08x (%s : %s).", (int)desc, cas, tys) );
  }
  else
  {
    DBUG_PRINT( "SEND", ("Receiving %08x (%s : %s).", (int)desc, cas, tys) );
  }
}  /* print desc_c_t */

#endif  /* DEBUG */


/* ************************************************************************ */
/*                                                                          */
/*  Funktionen zum Senden und Empfangen von Reduktionssprachenausdruecken.  */
/*  """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""" */
/*                                                                          */
/* ************************************************************************ */


#ifdef STANDALONE

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  OS_send  -  Verschicken eines Ausdrucks                                 */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int OS_send( fdescr, expr )
  int fdescr;
  PTR_DESCRIPTOR expr;

#else

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  _OS_send  -  Verschicken eines Ausdrucks mit preloaded functions        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static int _OS_send( fdescr, pf_Id, expr, anz_pf, pf )
  Id_pf pf_Id;
  int fdescr;
  PTR_DESCRIPTOR expr;
  int anz_pf;
  PTR_DESCRIPTOR pf;

#endif

{

  DBUG_ENTER( "_OS_send       " );
  START_MODUL( "_send" );

#ifndef STANDALONE
  pf_filled = anz_pf;
  pf_tab = pf;
  DBUG_PRINT
  (
    "COMM",
    ("pf    Table-descriptor %08x with %d elements.", pf_tab, anz_pf )
  );
#endif

  local_filled = 0;
  if ( (local_tab = OS_create_list( inq_desmax() )) == (PTR_DESCRIPTOR) 0 )
  {
    END_MODUL( "_send" );
    DBUG_RETURN( ERROR );
  }
  DBUG_PRINT( "COMM", ("local Table-descriptor %08x.", local_tab) );

  if
  (
    setjmp( _error_label )
  )
  {
    END_MODUL( "_send" );
    DBUG_RETURN( ERROR );
  }

#ifndef STANDALONE
  if ( pf_filled == 0  ||  pf_tab == (PTR_DESCRIPTOR)0 )
#endif
  {
    NEXT_TAB_ENTRY = _dig0;
    NEXT_TAB_ENTRY = _dig1;
    NEXT_TAB_ENTRY = _nil;
    NEXT_TAB_ENTRY = _nilstring;
    NEXT_TAB_ENTRY = _nilmat;
    NEXT_TAB_ENTRY = _nilvect;
    NEXT_TAB_ENTRY = _niltvect;
    NEXT_TAB_ENTRY = _digPI;
    NEXT_TAB_ENTRY = _digRG;
  }

  packetno = 0;                             /* Bisher kein Paket versendet. */
  kdesc = 0;                                /* Keine bekannten Desk. vers.  */
  ndesc = 0;                                /* Keine neuen Desk. versendet. */

  {                                 /* Wird im DEBUG-Modus geschrieben?     */
    int _debug = DEBUG;
    write_n_ints( fdescr, &_debug, 1 );     /* !!! EIN Aufruf von write...  */
  }
#ifndef STANDALONE
  write_intern( fdescr, &pf_Id, sizeof(pf_Id) / sizeof(int) );
#endif
  sendrec( fdescr, expr );                  /* Ausdruck verschicken         */
  DBUG_PRINT
  (
    "COMM",
    ("Sent %d packets, %d descriptors (%d several times).",
      packetno, ndesc+kdesc, kdesc)
  );
#ifdef STANDALONE
    DBUG_PRINT
      ( "COMM", ("local Table-descriptor %08x will be released.", local_tab) );
    release_list( local_tab );
#endif
  END_MODUL( "_send" );
  DBUG_RETURN( NOERROR );
}  /* _OS_send */


#ifndef STANDALONE

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  OS_send_pf  -  Verschicken eines Ausdrucks als preloaded function       */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int OS_send_pf( fdescr, pf_Id, expr, anzPtr, pfPtr )
  Id_pf pf_Id;
  int fdescr;
  PTR_DESCRIPTOR expr;
  int * anzPtr;
  PTR_DESCRIPTOR * pfPtr;
{
  int error;
  DBUG_ENTER( "OS_send_pf     " );
  START_MEASURE( SEND_TIME, NO_TIME );
  START_MODUL( "send_pf" );
  error = _OS_send( fdescr, pf_Id, expr, 0, (PTR_DESCRIPTOR)0 );
  *anzPtr = local_filled;
  *pfPtr = local_tab;
  if ( !error )
    INC_REFCNT( expr );
  END_MODUL( "send_pf" );
  END_MEASURE( SEND_TIME, NO_TIME );
  DBUG_RETURN( error );
}  /* OS_send_pf */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  OS_send_pf_again  -  Verschicken eines Ausdrucks                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int OS_send_pf_again( fdescr, pf_id, expr /* , anz_pf, pf */ )
  int fdescr;
  Id_pf pf_id;
  PTR_DESCRIPTOR expr;
/*  int anz_pf;         */
/*  PTR_DESCRIPTOR pf;  */
{
  int error;
  DBUG_ENTER( "OS_send_pf_ag'n" );
  START_MEASURE( SEND_TIME, NO_TIME );
  START_MODUL( "send" );
  error = _OS_send( fdescr, pf_id, expr, 0, (PTR_DESCRIPTOR)0 );
  if ( local_tab )
  {
    DBUG_PRINT
      ( "COMM", ("local Table-descriptor %08x will be released.", local_tab) );
    release_list( local_tab );
  }
  END_MODUL( "send" );
  END_MEASURE( SEND_TIME, NO_TIME );
  DBUG_RETURN( error );
}  /* OS_send_pf_again */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  OS_send  -  Verschicken eines Ausdrucks                                 */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int OS_send( fdescr, expr, anz_pf, pf )
  int fdescr;
  PTR_DESCRIPTOR expr;
  int anz_pf;
  PTR_DESCRIPTOR pf;
{
  int error;
  DBUG_ENTER( "OS_send        " );
  START_MEASURE( SEND_TIME, SAME_TIME );
  START_MODUL( "send" );
  error = _OS_send( fdescr, 0, expr, anz_pf, pf );
  if ( local_tab )
  {
    DBUG_PRINT
      ( "COMM", ("local Table-descriptor %08x will be released.", local_tab) );
    release_list( local_tab );
  }
  END_MODUL( "send" );
  END_MEASURE( SEND_TIME, SAME_TIME );
  DBUG_RETURN( error );
}  /* OS_send */

#endif  /* keine STANDALONE - Version */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  _OS_receive  -  Empfangen eines mit _OS_send verschickten Ausdrucks     */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#ifdef STANDALONE


int OS_receive( fdescr, exprPtr )
  int fdescr;
  PTR_DESCRIPTOR * exprPtr;

#else

static int _OS_receive( fdescr, pf_IdPtr, exprPtr, anz, pf )
  int fdescr;
  Id_pf * pf_IdPtr;
  PTR_DESCRIPTOR *exprPtr;
  int anz;
  PTR_DESCRIPTOR pf;

#endif

  /*
    Liefert *exprPtr den gelesenen Ausdruck zurueck.
    Das Funktionsergebnis ist ERROR oder NOERROR.
  */

{
  DBUG_ENTER( "_OS_receive    " );
  START_MODUL( "_receive" );

#ifndef STANDALONE
  pf_filled = anz;
  pf_tab = pf;
  DBUG_PRINT( "COMM", ("pf    Table-descriptor %08x.", pf_tab) );
#endif

  local_filled = 0;
  if ( (local_tab = OS_create_list( TABELLENGROESSE )) == (PTR_DESCRIPTOR) 0 )
  {
    END_MODUL( "_receive" );
    DBUG_RETURN( ERROR );
  }
  DBUG_PRINT( "COMM", ("local Table-descriptor %08x.", local_tab) );

  if
  (
    setjmp( _error_label )
  )
  {
    END_MODUL( "_receive" );
    DBUG_RETURN( ERROR );
  }

#ifndef STANDALONE
  if ( pf_filled == 0  ||  pf_tab == (PTR_DESCRIPTOR)0 )
#endif
  {
    NEXT_TAB_ENTRY = _dig0;
    NEXT_TAB_ENTRY = _dig1;
    NEXT_TAB_ENTRY = _nil;
    NEXT_TAB_ENTRY = _nilstring;
    NEXT_TAB_ENTRY = _nilmat;
    NEXT_TAB_ENTRY = _nilvect;
    NEXT_TAB_ENTRY = _niltvect;
    NEXT_TAB_ENTRY = _digPI;
    NEXT_TAB_ENTRY = _digRG;
  }
  packetno = 0;                             /* Bisher kein Paket empfangen. */
  kdesc = 0;                                /* Keine bekannten Desk. empf.  */
  ndesc = 0;                                /* Keine neuen Desk. empfangen. */

                                    /* Wird im DEBUG-Modus gelesen?         */
  read_n_ints( fdescr, &debug, 1 );         /* !!! EIN Aufruf von read...   */
  DBUG_PRINT
  (
    "COMM",
    ("Data will contain%s debug information.", (debug) ? "" : " no" )
  );

#ifndef STANDALONE
  read_intern( fdescr, (int *)pf_IdPtr, sizeof(Id_pf) / sizeof(int) );
#endif
  *exprPtr = receiverec( fdescr );
  DBUG_PRINT( "COMM", ("received : %08x.", (*exprPtr)) );
  DBUG_PRINT
  (
    "COMM",
    ("Received %d packets, %d descriptors (%d several times).",
     packetno, ndesc+kdesc, kdesc)
  );
#ifdef STANDALONE
  DBUG_PRINT
    ( "COMM", ("local Table-descriptor %08x will be released.", local_tab) );
  release_list( local_tab );
#endif
  END_MODUL( "_receive" );
  DBUG_RETURN( NOERROR );
}  /* _OS_receive */


#ifndef STANDALONE

/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  OS_receive_pf  -  Empfangen eines Ausdrucks als preloaded function      */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int OS_receive_pf( fdescr, pf_IdPtr, exprPtr, anzPtr, pfPtr )
  Id_pf * pf_IdPtr;         /* eventuell nicht ganz gluecklich, den mit     */
                            /* reinzunehmen.                                */
  int fdescr;
  PTR_DESCRIPTOR *exprPtr;
  int * anzPtr;
  PTR_DESCRIPTOR *pfPtr;
{
  int error;
  DBUG_ENTER( "OS_receive_pf  " );
  START_MEASURE( RECV_TIME, NO_TIME );
  START_MODUL( "receive_pf" );
  error = _OS_receive( fdescr, pf_IdPtr, exprPtr, 0, (PTR_DESCRIPTOR)0 );
  *anzPtr = local_filled;
  *pfPtr = local_tab;
  END_MODUL( "receive_pf" );
  END_MEASURE( RECV_TIME, NO_TIME );
  DBUG_RETURN( error );
}  /* receive_pf */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  OS_receive  -  Empfangen eines mit send verschickten Ausdrucks          */
/*                                                                          */
/* ------------------------------------------------------------------------ */

int OS_receive( fdescr, exprPtr, anz, pf )
  int fdescr;
  PTR_DESCRIPTOR *exprPtr;
  int anz;
  PTR_DESCRIPTOR pf;

  /*
    Liefert *exprPtr den gelesenen Ausdruck zurueck.
    Das Funktionsergebnis ist ERROR oder NOERROR.
  */
{
  int error;
  Id_pf pf_id;

  DBUG_ENTER( "OS_receive     " );
  START_MEASURE( RECV_TIME, SAME_TIME );
  START_MODUL( "receive" );
  error = _OS_receive( fdescr, &pf_id, exprPtr, anz, pf );
  if ( local_tab )
  {
    DBUG_PRINT
      ( "COMM", ("local Table-descriptor %08x will be released.", local_tab) );
    release_list( local_tab );
  }
  END_MODUL( "receive" );
  END_MEASURE( RECV_TIME, SAME_TIME );
  DBUG_RETURN( error );
}  /* receive */

#endif  /* keine STANDALONE - Version */


/*
  ! Die Prozeduren fuer das Senden und Empfangen sind weitgehend aehnlich,
  ! d.h. zu jeder Prozedur "..send.." gibt es eine
  ! entsprechende Prozedur "..receive..", mit aehnlichem Aufbau.
  ! Vor allem beim Aendern von "sendrec" / "receiverec" ist dies zu beachten!
*/


/* ************************************************************************ */
/*                                                                          */
/*  Funktionen zum Versenden von Reduktionssprachenausdruecken              */
/*  """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""" */
/*                                                                          */
/* ************************************************************************ */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/* write_intern - zentrale Senderoutine ( erzeugt Bloecke )                 */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void write_intern( fdesc, data, num )
  int fdesc;            /* Filedeskriptor der Zieldatei bzw. des Sockets.   */
  int * data;           /* Zu schreibende Daten.                            */
  int num;              /* Zahl der zu schreibenden Integers.               */

  /*
    Write_intern ist dafuer vorgesehen, den Daten gegebenenfalls Debug-In-
    formationen voranzustellen.
    Im Fehlerfall wird mit post_mortem abgebrochen.
    Den Daten wird im DEBUG-Fall eine Laengenangabe mitgegeben, um besser
    ueberpruefen zu koennen, ob die sendende und die empfangende Prozedur
    die Struktur in gleicher Weise durchlaufen.
    Aus diesem Grunde muss im DEBUG-Fall zu jedem write_intern genau ein
    entsprechendes read_intern gehoeren.
    Im Nicht-DEBUG-Fall wird auf diese Information verzichtet.
  */

{
  int gesendet;         /* Anzahl der tatsaechlich gesendeten Integers.     */
#if DEBUG
  int info[2];

  info[0] = packetno;
  info[1] = num;
  if( write_n_ints( fdesc, info, 2 ) != 2 )
  {
    DBUG_PRINTE
    (
      "ERRS",
      ("ERROR : can't write debug info for packet no. %d !", packetno)
    );
    post_mortem_intern();
  }
  packetno++;
#endif  /* DEBUG */
  if ( (gesendet = write_n_ints( fdesc, data, num )) != num  )
  {
    DBUG_PRINTE
    (
      "ERRS",
      ("ERROR : wrote only %d of %d ints of data !", gesendet, num)
    );
    post_mortem_intern();
  }
}  /* write_intern */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  already_sended  -  Deskriptor schon verschickt?                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static int already_sended( dPtr )
  PTR_DESCRIPTOR dPtr;

  /*
    sieht in der pf_tab und in der local_tab jeweils bis zu den Nummern
    pf_filled bzw. local_filled ( ausschliesslich ) nach, ob
    die Adresse dPtr schon eingetragen ist.
    Ist sie's nicht, wird der Wert NOT_SENDED zurueckgeliefert, ansonsten
    die Position in den Tabellen, wobei diese wie eine behandelt werden,
    und die pf- vor der lokalen Tabelle kommt.
  */

{
  register int h = 0;
  register PTR_DESCRIPTOR * hp;
  int offset;

  RES_HEAP;
#ifndef STANDALONE
  /* preloaded functions durchsuchen */
  for
  (
    h = 0;
    h < pf_filled 
      && *((PTR_DESCRIPTOR *)(R_LIST( (* pf_tab), ptdv)) + h) != dPtr;
    h++
  );
  if ( h < pf_filled )
  {
    REL_HEAP;
    return( h );
  }
#endif
  /* lokale Tabelle durchsuchen */
  for
  (
    offset = h, h = 0, hp = (PTR_DESCRIPTOR *)(R_LIST( (* local_tab), ptdv) );
    h < local_filled && *(hp + h) != dPtr;
    h++
  );
  REL_HEAP;
  return ( (h == local_filled) ? NOT_SENDED : h+offset );
}  /* already_sended */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  send_header  -  Vorinformation verschicken                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void send_header( fdescr, Nr )
  int fdescr;
  int Nr;

  /* schreibt den Header fuer einen Deskriptor */

{
  write_intern( fdescr, &Nr, 1 );
}  /* send_header */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  send_descr  -  Deskriptor verschicken                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void send_descr( fdescr, dPtr )
  int fdescr;
  PTR_DESCRIPTOR dPtr;

  /* verschickt einen Deskriptor */

{
  write_intern( fdescr, dPtr, sizeof(T_DESCRIPTOR) / sizeof(int) );
}  /* send_descr */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  send_data  -  "gewoehnliche" Daten verschicken                          */
/*                                                                          */
/* ------------------------------------------------------------------------ */

/* der Symmetrie halber: */
#define send_data( fdescr, data, length ) \
  write_intern( fdescr, (int *)(data), length )


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  send_list  -  Listen verschicken                                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void send_list( fdescr, list, elements )
  int fdescr;
  T_HEAPELEM * list;
  int elements;	             	/* Anzahl der Elemente in der Liste */

  /*
    verschickt die Liste list.
    Danach wird die Liste durchgegangen, ob sie noch Zeiger auf weitere
    Deskriptoren enthaelt und "sendrec" wird dann fuer diese Deskriptoren
    wieder aufgerufen.
  */

{
  int h;
  write_intern( fdescr, list, (int)elements );
  for (h = 0; h < elements; h++)
    if ( T_POINTER(list[h]) && (list[h] != (T_HEAPELEM) 0) )
      sendrec( fdescr, (PTR_DESCRIPTOR)(list[h]) );
}  /* send_list */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  send_heapelem  -  Heapelemente verschicken                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void send_heapelem( fdescr, elem )
  int fdescr;
  T_HEAPELEM * elem;

  /*
    verschickt den Heapausdruck elem.
    Danach wird dieser durchgegangen, ob er noch Zeiger auf weitere
    Deskriptoren enthaelt und "sendrec" wird dann fuer diese Deskriptoren
    wieder aufgerufen.
  */

{
  int laenge = * (int *) elem;
  int h;
#if DEBUG
  write_intern( fdescr, &laenge, 1 );       /* ! Diese Aufteilung */
  write_intern( fdescr, elem+1, laenge );   /*   ist notwendig    */
#else
  write_intern( fdescr, elem, laenge + 1 );
#endif
  for (h = 1; h <= laenge; h++)
    if ( T_POINTER(elem[h]) && (elem[h] != (T_HEAPELEM) 0) )
      sendrec( fdescr, (PTR_DESCRIPTOR)(elem[h]) );
}  /* send_heapelem */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  send_code  -  Codevektor des Matchdeskriptors verschicken               */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void send_code( fdescr, code )
  int fdescr;
  T_HEAPELEM * code;

  /*
    verschickt den Codevektor code.
    Danach wird dieser durchgegangen, ob er noch Zeiger auf weitere
    Deskriptoren enthaelt und "sendrec" wird dann fuer diese Deskriptoren
    wieder aufgerufen.
  */

{
  int laenge = (int) (code[VEKTORLAENGE]);
  int h;                                 
  int code_ende;
  T_HEAPELEM *namelist;
  int anz_bv_var;

#if DEBUG
  write_intern( fdescr, &laenge, 1 );       /* ! Diese Auftei- */
  write_intern( fdescr, code+1, laenge );   /* lung ist notwendig */
#else
  write_intern( fdescr, code, laenge + 1 );
#endif  

/* US */
#if DREDUMA                                                      

  if ( code[OPAT] != 0 )                                   /* Verweis auf das Orginalpattern */
     sendrec( fdescr, (PTR_DESCRIPTOR) (code[OPAT]) );                                         

  /* senden des Codes */

  h = OFFSETS;     /* Anfang des Codes */        
  code_ende = h + code[CODELAENGE];  
 
  while ( h < code_ende )
  {
    switch( (int) (code[h]) )
    {
      case EOT:
        if ( h != code_ende )
          DBUG_PRINT
          (
            "SEND",
            ("WARNING : length of codevector (%d) should be %d.", code_ende, h)
          );
        h = code_ende + 1;     /* Um die for-Schleife zu beenden.  */
        break;
      case KTEST:
      case A_KTEST:
      case STR_TEST:
      case DIG_TEST:
      case SCAL_TEST:
      case MATRIX_TEST:
      case BOOL_TEST:
      case VECTOR_TEST:
      case TVECTOR_TEST:
      case IN_TEST:
      case IN_TEST1:
      case A_STR_TEST:
      case A_DIG_TEST:
      case A_SCAL_TEST:
      case A_MATRIX_TEST:
      case A_VECTOR_TEST:
      case A_TVECTOR_TEST:
        if ( T_POINTER(code[h+1]) && (code[h+1] != (T_HEAPELEM) 0) )
          sendrec( fdescr, (PTR_DESCRIPTOR) (code[h+1]) );
        h += 2;
        break;
      case JMP2:
      case INCR:
      case SETBASE:
      case A_SETBASE:
      case DECR:
      case LOAD:
      case A_LOAD:
      case SUBR:
      case RETURN:
      case NIL_TEST:
      case NILM_TEST:
      case NILV_TEST:
      case NILTV_TEST:
      case NILSTRING_TEST:
      case A_NIL_TEST:
      case A_NILM_TEST:
      case A_NILV_TEST:
      case A_NILTV_TEST:
      case A_NILSTRING_TEST:
      case A_LI_CHECK:
      case LI_CHECK:
      case A_STR_CHECK:
      case STR_CHECK:
      case LOAD_ARITY:
      case A_LOAD_ARITY:
      case L_LOAD_B:
      case L_LOAD_A:
      case AS_LOAD:
      case PM_CUTT:
      case END_DOLL:
      case GEN_BLOCK:
        h++;
        break;
      case JMP:
      case TY_TEST:
      case A_BOOL_TEST:
      case A_TY_TEST:
      case SKIPSKIP:
      case IN_SKIPSKIP:
      case LA_SKIPSKIP:
      case LA_IN_SKIPSKIP:
      case PM_DOLL:
      case IN_PM_DOLL:
      case PM_PUSH:
      case ATOM_TEST:
      case A_ATOM_TEST:
      case DEL_BLOCKS:
        h += 2;
        break;
      case TESTN:
      case A_TESTN:
      case TESTV:
      case A_TESTV:
        h += 3;
        break;
      default:
        h++;
        DBUG_PRINTE
        (
          "ERRS",
          ("ERROR : unknown matchcode (%8.8x) at position %d !", code[h], h)
        );
        break;
    }  /* switch */
  }  /* while */      
 
  /* senden der im pattern auftretenden Variablen */
       

  namelist = (code + (int)code[NAMELIST]);
  /* namelist verweist auf die Liste der gebundenen Variablen. */

  anz_bv_var = namelist[0];
  /* Anzahl der Variablen */

/*
    for ( h = (int)code[NAMELIST] + 1; h <= laenge ; h++ )  
     sendrec( fdescr, (PTR_DESCRIPTOR) (code[h]) );   
*/
    for ( h = 1; h <= anz_bv_var ; h++ )  
       sendrec( fdescr, (PTR_DESCRIPTOR) (namelist[h]) );   
  

#else
  if ( code[1] != 0 )
    sendrec( fdescr, (PTR_DESCRIPTOR) (code[STELLIGKEIT]) );
  h = 2;
  while ( h <= laenge )
  {
    switch( (int) (code[h]) )
    {
      case EOT:
        if ( h != laenge )
          DBUG_PRINT
          (
            "SEND",
            ("WARNING : length of codevector (%d) should be %d.", laenge, h)
          );
        h = laenge + 1;     /* Um die for-Schleife zu beenden.  */
        break;
      case KTEST:
      case A_KTEST:
      case STR_TEST:
      case DIG_TEST:
      case SCAL_TEST:
      case MATRIX_TEST:
      case BOOL_TEST:
      case VECTOR_TEST:
      case TVECTOR_TEST:
      case IN_TEST:
      case IN_TEST1:
      case A_STR_TEST:
      case A_DIG_TEST:
      case A_SCAL_TEST:
      case A_MATRIX_TEST:
      case A_VECTOR_TEST:
      case A_TVECTOR_TEST:
        if ( T_POINTER(code[h+1]) && (code[h+1] != (T_HEAPELEM) 0) )
          sendrec( fdescr, (PTR_DESCRIPTOR) (code[h+1]) );
        h += 2;
        break;
      case JMP2:
      case INCR:
      case SETBASE:
      case A_SETBASE:
      case DECR:
      case LOAD:
      case A_LOAD:
      case SUBR:
      case RETURN:
      case NIL_TEST:
      case NILM_TEST:
      case NILV_TEST:
      case NILTV_TEST:
      case NILSTRING_TEST:
      case A_NIL_TEST:
      case A_NILM_TEST:
      case A_NILV_TEST:
      case A_NILTV_TEST:
      case A_NILSTRING_TEST:
      case A_LI_CHECK:
      case LI_CHECK:
      case A_STR_CHECK:
      case STR_CHECK:
      case LOAD_ARITY:
      case A_LOAD_ARITY:
      case L_LOAD_B:
      case L_LOAD_A:
      case AS_LOAD:
      case PM_CUTT:
      case END_DOLL:
      case GEN_BLOCK:
        h++;
        break;
      case JMP:
      case TY_TEST:
      case A_BOOL_TEST:
      case A_TY_TEST:
      case SKIPSKIP:
      case IN_SKIPSKIP:
      case LA_SKIPSKIP:
      case LA_IN_SKIPSKIP:
      case PM_DOLL:
      case IN_PM_DOLL:
      case PM_PUSH:
      case ATOM_TEST:
      case A_ATOM_TEST:
      case DEL_BLOCKS:
        h += 2;
        break;
      case TESTN:
      case A_TESTN:
      case TESTV:
      case A_TESTV:
        h += 3;
        break;
      default:
        h++;
        DBUG_PRINTE
        (
          "ERRS",
          ("ERROR : unknown matchcode (%8.8x) at position %d !", code[h], h)
        );
        break;
    }  /* switch */
  }  /* while */      
#endif
}  /* send_code */

                    
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  sendrec  -  Graphen verschicken                                         */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void sendrec( fdescr, dPtr )
  int fdescr;               /* File-Deskriptor der Zieldatei                */
  PTR_DESCRIPTOR dPtr;      /* Adresse des zu verschickenden Deskriptors    */

  /*
    verschickt rekursiv einen Graphen, dessen Wurzel ein Deskriptor ist.
    Jeder erreichte Deskriptor wird sofort verschickt, hat er Daten, folgen
    ihm diese sofort. Die Laenge der Daten ist in dem Deskriptor direkt oder
    indirekt enthalten.
  */

{
  DESC_CLASS class1;
  PTR_DESCRIPTOR dirdesc;
  int h;

#if DEBUG
  if ( ! isdesc( dPtr ))
  {
    DBUG_PRINTE
    (
      "ERRS",
      ("ERROR : sendrec doesn't get a pointer to descriptor (%08x) !",
        (int)dPtr)
    );
    post_mortem_intern();
  }
#endif
  if ( ( h = already_sended( dPtr) ) != NOT_SENDED )
  {
    kdesc++;                    /*Zahl der wiederholt versch. Desk. erhoehen*/
    send_header( fdescr, h );   /* Nur der Index wird verschickt.       */
  }
  else
  {
    ndesc++;                    /* Zahl der neu verschickten Desk. erhoehen.*/
    send_header( fdescr, NOT_SENDED );
#if DEBUG
    print_desc_c_t( TRUE, dPtr );
#endif
    *((PTR_DESCRIPTOR *)(R_LIST((*local_tab), ptdv)) + local_filled++) = dPtr;
    /* Deskriptor komplett verschicken      */
    /*
      Der Deskriptor kann bereits hier als Ganzes verschickt werden, da
      in ihm keine Aenderungen vorgenommen werden!
    */
    send_descr( fdescr, dPtr );
    switch( class1 = R_DESC( (*dPtr), class ) )
    {
    case C_SCALAR:
      break;
    case C_DIGIT:
      send_data
        ( fdescr, R_DIGIT((*dPtr), ptdv), (int)R_DIGIT((*dPtr),ndigit) );
      break;
    case C_LIST:
      if ( (dirdesc = R_LIST( (*dPtr), ptdd )) != (T_PTD) 0 ) /* indirekt ? */
      {
        int offset = R_LIST( (*dPtr), ptdv ) - R_LIST( (*dirdesc), ptdv );
        
        /*
          Im Gegensatz zu anderen Versionen wird hier der Offset direkt
          hinter dem Deskriptor geschrieben, damit "alles zusammenbleibt".
        */
        write_intern( fdescr, &offset, sizeof(offset) );
        sendrec( fdescr, dirdesc );
      }
      else
      {
        send_list
        (
          fdescr,
          (T_HEAPELEM *)(R_LIST( (*dPtr), ptdv )),
          (int)(R_LIST( (*dPtr), dim ) )
        );
      }
      break;
    case C_MATRIX:
    case C_VECTOR:
    case C_TVECTOR:
      if ( (dirdesc = R_MVT( (*dPtr), ptdd, class1 )) != (T_PTD) 0 )
      {
        int offset = R_MVT((*dPtr),ptdv,class1)-R_MVT((*dirdesc),ptdv,class1);
        write_intern( fdescr, &offset, sizeof(offset) );
        sendrec( fdescr, dirdesc );
      }
      else
      {
        send_list
        (
          fdescr,
          (T_HEAPELEM *)R_MVT( (*dPtr), ptdv,  class1 ),
          (int)(R_MVT((*dPtr), nrows, class1)*R_MVT((*dPtr), ncols, class1) )
        );
      }
      break;
    case C_CONSTANT:
    case C_EXPRESSION:
      switch ( R_DESC( (*dPtr), type ))
      {
      case TY_COND:
        send_heapelem( fdescr, R_COND( (*dPtr), ptte ));
        send_heapelem( fdescr, R_COND( (*dPtr), ptee ));
        break;
      case TY_EXPR:
        send_heapelem( fdescr, R_EXPR( (*dPtr), pte ));
        break;
      case TY_SPEC:
        if
        (
          R_SPEC( (*dPtr), ptdd ) !=0
          && T_POINTER( (T_HEAPELEM)(R_SPEC( (*dPtr), ptdd )))
        )
          sendrec( fdescr, R_SPEC( (*dPtr), ptdd ));   /* !Deskriptor      */
        if ( R_SPEC( (*dPtr), i_stack ) != (PTR_DESCRIPTOR)0 )
          sendrec( fdescr, R_SPEC( (*dPtr), i_stack ));   /* !Deskriptor */
        if ( R_SPEC( (*dPtr), tilde_stack) != (PTR_DESCRIPTOR)0 )
          sendrec( fdescr, R_SPEC( (*dPtr), tilde_stack ));
        break;
      case TY_VAR:
        sendrec( fdescr, R_VAR( (*dPtr), ptnd ));
        break;
/* US*/
#if DREDUMA
      case TY_FNAME: 
        send_heapelem( fdescr, R_FNAME( (*dPtr), ptn)); 
        break;  
#endif             
      case TY_NAME:
        send_heapelem( fdescr, R_NAME( (*dPtr), ptn ));
        break;   

      case TY_SUB:
#if !DREDUMA
      case TY_REC:
#endif
        send_heapelem( fdescr, R_FUNC( (*dPtr), namelist ));
        if ( R_FUNC( (*dPtr), pte ) != NULL )                   /* DREDUMA  */
          send_heapelem( fdescr, R_FUNC( (*dPtr), pte ));
        break;
      case TY_SWITCH:
        send_heapelem( fdescr, R_SWITCH( (*dPtr), ptse ));
        break;
      case TY_MATCH:
        if ( R_MATCH( (*dPtr), code ) != (PTR_HEAPELEM) 0 )
          send_code( fdescr, R_MATCH( (*dPtr), code ));         /*   Code   */
        if ( R_MATCH( (*dPtr), guard ) != (PTR_HEAPELEM) 0 )
          send_heapelem( fdescr, R_MATCH( (*dPtr), guard ));    /*  Guard   */
        send_heapelem( fdescr, R_MATCH( (*dPtr), body ));       /*   Body   */
        break;
      case TY_NOMAT:
        if ( R_NOMAT( (*dPtr), guard_body) != (PTR_HEAPELEM) 0 )
          send_heapelem( fdescr, R_NOMAT( (*dPtr), guard_body ));
        sendrec( fdescr, R_NOMAT( (*dPtr), ptsdes ));   /* !Deskriptor      */
        break;
      case TY_LREC:
        send_heapelem( fdescr,  R_LREC( (*dPtr), namelist ));      
/* US */
#if DREDUMA 
        send_list( fdescr, R_LREC( (*dPtr), ptdv), R_LREC( (* dPtr), nfuncs ) + 1 );
#else
        send_list( fdescr, R_LREC( (*dPtr), ptdv), R_LREC( (* dPtr), nfuncs ));
#endif
        break;
      case TY_LREC_IND:
        sendrec( fdescr, R_LREC_IND( (*dPtr), ptdd ));
        sendrec( fdescr, R_LREC_IND( (*dPtr), ptf ));
        break;
#if !DREDUMA
      case TY_LREC_ARGS:
        sendrec( fdescr, R_LREC_ARGS( (*dPtr), ptdd ));
        send_list
        (
          fdescr,
          R_LREC_ARGS( (*dPtr), ptdv ),
          R_LREC_ARGS( (*dPtr), nargs )
        );
        break;                 

#endif  /* !DREDUMA */
/*      case TY_CLOSURE:
        break;   */
      }  /* SWITCH ( Typ des Deskriptors )   */
      break;
    default:
      DBUG_PRINTE
      (
        "ERRS",
        ("ERROR : descriptor %08x has unknown class %d !", (int)dPtr, class1)
      );
      post_mortem_intern();
      break;
    }  /* SWITCH ( Klasse des Deskriptors ) */
  }  /* ELSE ( Deskriptor verschicken )   */
}  /* sendrec */


/* ************************************************************************ */
/*                                                                          */
/*  Funktionen zum Empfangen von Reduktionssprachenausdruecken              */
/*  """"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""" */
/*                                                                          */
/* ************************************************************************ */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  already_received  -  Zeiger auf einen Deskriptor in einer Tabelle       */
/*                                                                          */
/* ------------------------------------------------------------------------ */

#ifdef STANDALONE

#define already_received( index ) \
  (( index < local_filled ) \
   ? (PTR_DESCRIPTOR) ((R_LIST((* local_tab), ptdv))[index]) \
   : (PTR_DESCRIPTOR)0 )

#else

static PTR_DESCRIPTOR already_received( index )
  int index;

{
  if ( index < pf_filled )
    return( (PTR_DESCRIPTOR) ((R_LIST((* pf_tab), ptdv))[index]));
  else if ( (index -= pf_filled) < local_filled )
    return( (PTR_DESCRIPTOR) ((R_LIST((* local_tab), ptdv))[index]));
  else
    return( (PTR_DESCRIPTOR)0 );
}  /* already_received */

#endif

/* ------------------------------------------------------------------------ */
/*                                                                          */
/* read_intern - zentrale Empfangsroutine ( fuer Bloecke )                  */
/*                                                                          */
/* ------------------------------------------------------------------------ */


static void read_intern( fdesc, data, num )
  int fdesc;            /* Filedeskriptor der Quelldatei bzw. des Sockets.  */
  int * data;           /* Zu lesende Daten.                                */
  int num;              /* Zahl der zu lesenden Integers.                   */

  /*
    Read_intern ist dafuer vorgesehen, die Daten gegebenenfalls mit Debug-
    Informationen zu empfangen.
    Im Fehlerfall wird mit post_mortem abgebrochen.
  */

{
  int empfangen;        /* Anzahl der tatsaechlich empfangenen Integers.    */

/* #if DEBUG */
  if ( debug )
  {
    int info[2];
  
    if( read_n_ints( fdesc, info, 2 ) != 2 )
    {
      DBUG_PRINT
      (
        "ERRS",
        ("ERROR : can't read debug info for packet no. %d !", packetno)
      );
      post_mortem_intern();
    }
    if ( info[0] != packetno )
      DBUG_PRINT
      (
        "SEND",
        ("WARNING : bad packetnumber (%d expected, %d read) !", packetno, info[0])
      )
    if ( info[1] != num )
      DBUG_PRINT
      (
        "SEND",
        ("WARNING : bad packetsize (%d expected, %d read) !", num, info[1])
      )
    packetno++;
  }  /* debug */
/* #endif  /* DEBUG */
  if ( (empfangen = read_n_ints( fdesc, data, num )) < num )
  {
    DBUG_PRINTE
    (
      "ERRS",
      ("ERROR : read only %d of %d ints of data !", empfangen, num)
    );
    post_mortem_intern();
  }
}  /* read_intern */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  receive_header  -  Vorinformation empfangen                             */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static int receive_header( fdescr)
  int fdescr;

  /* liest den Header fuer einen Deskriptor */

{
  int Nr;
  read_intern( fdescr, &Nr, 1 );
  return( Nr );
}  /* receive_header */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  receive_descr  -  Deskriptor empfangen                                  */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void receive_descr( fdescr, dPtr)
  int fdescr;
  PTR_DESCRIPTOR dPtr;

  /*
    empfaengt einen Deskriptor.
    Die DESCR-Teile sind gesetzt, der Union-Teil nur, wo keine Zeiger sind.
  */

{
  read_intern( fdescr, dPtr, sizeof(T_DESCRIPTOR) / sizeof(int) );
  * (COUNT *) dPtr = 1;                             /* !! RefCount auf 1    */
}  /* receive_descr */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  receive_data  -  Daten empfangen                                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void receive_data( fdescr, length, data)
  int fdescr;
  COUNT length;
  T_HEAPELEM ** data;

  /*
    reserviert Platz und empfaengt die Daten in der Laenge length.
    Es wird keine Interpretation vorgenommen.
  */

{
  if ( !newheap( (int)length, data ))
  {
    DBUG_PRINTE( "ERRS", ("ERROR : no space in heap for the data !") );
    post_mortem_intern();
  }
  read_intern( fdescr, (int*)(*data), (int)length );
}  /* receive_data */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  receive_list  -  Liste empfangen                                        */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void receive_list( fdescr, elements, list)
  int fdescr;
  int elements;                 /* Anzahl der Elemente in der Liste */
  T_HEAPELEM ** list;

  /*
    empfaengt die Liste list.
    Danach wird die Liste durchgegangen, ob sie noch Zeiger auf weitere
    Deskriptoren enthaelt und "receiverec" wird dann fuer diese Deskripto-
    ren wieder aufgerufen.
  */

{
  int h;
  if ( !newheap( elements, list ))
  {
    DBUG_PRINTE( "ERRS", ("ERROR : no space in heap for the list !") );
    post_mortem_intern();
  }
  read_intern( fdescr, * list, elements );
  for (h = 0; h < elements; h++)
    if ( T_POINTER((*list)[h]) && ((*list)[h] != (T_HEAPELEM) 0) )
      (* list)[h] = (T_HEAPELEM) receiverec( fdescr);
}  /* receive_list */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  receive_heapelem  -  Heapelement empfangen                              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void receive_heapelem( fdescr, elem )
  int fdescr;
  T_HEAPELEM ** elem;

  /*
    empfaengt den Heapausdruck elem.
    Danach wird dieser durchgegangen, ob er noch Zeiger auf weitere
    Deskriptoren enthaelt und "receiverec" wird dann fuer diese Deskripto-
    ren wieder aufgerufen.
  */

{
  int laenge;
  int h;

  read_intern( fdescr, &laenge, 1 );    /* auch hier read_intern ! */
  if ( !newheap( laenge+1, elem ))
  {
    DBUG_PRINTE( "ERRS", ("ERROR : no space in heap for the heap-elements !") );
    post_mortem_intern();
  }
  (* elem)[0] = laenge;
  read_intern( fdescr, (* elem)+1, laenge );
  for (h = 1; h <= laenge; h++)
    if ( T_POINTER((*elem)[h]) && ((*elem)[h] != (T_HEAPELEM) 0) )
      (* elem)[h] = (T_HEAPELEM) receiverec( fdescr );
}  /* receive_heapelem */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  receive_code  -  Codevektor des Matchdeskriptors empfangen              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static void receive_code( fdescr, code )
  int fdescr;
  T_HEAPELEM ** code;

  /*
    empfaengt den Codevektor code.
    Danach wird dieser durchgegangen, ob er noch Zeiger auf weitere
    Deskriptoren enthaelt und "receiverec" wird dann fuer diese Deskripto-
    ren wieder aufgerufen.
  */

{
  int laenge;
  int h;    
  int code_ende;
  int anz_bv_var;
  int offset;

  read_intern( fdescr, &laenge, 1 );    /* auch hier read_intern ! */
  if ( !newheap( laenge+1, code ))
  {
    DBUG_PRINTE( "ERRS", ("ERROR : no space in heap for the code-vector !") );
    post_mortem_intern();
  }
  (* code)[VEKTORLAENGE] = laenge;     

  read_intern( fdescr, (* code)+1, laenge );

/* US */
#if DREDUMA                                               

  if ( (* code)[OPAT] != (T_HEAPELEM) 0 )
    (* code)[OPAT] = (T_HEAPELEM) receiverec( fdescr );    /* Orginalpattern */

  /* empfangen des Codes */  

  h = OFFSETS;                                        /* Anfang des Codes im Codevektor */
  code_ende = h + (int) ((*code)[CODELAENGE] );          /* Laenge des Codes */
  
  while ( h < code_ende )
  {
    switch( (int) ((*code)[h]) )
    {
      case EOT:
        if ( h != code_ende )
          DBUG_PRINT
          (
            "SEND",
            ("WARNING : length of codevector (%d) should be %d.", code_ende, h)
          );
        h = code_ende + 1;     /* Um die while-Schleife zu beenden.  */
        break;
      case KTEST:
      case A_KTEST:
      case STR_TEST:
      case DIG_TEST:
      case SCAL_TEST:
      case MATRIX_TEST:
      case BOOL_TEST:
      case VECTOR_TEST:
      case TVECTOR_TEST:
      case IN_TEST:
      case IN_TEST1:
      case A_STR_TEST:
      case A_DIG_TEST:
      case A_SCAL_TEST:
      case A_MATRIX_TEST:
      case A_VECTOR_TEST:
      case A_TVECTOR_TEST:
        if ( T_POINTER((*code)[h+1]) && ((*code)[h+1] != (T_HEAPELEM) 0) )
          (* code)[h+1] = (T_HEAPELEM) receiverec( fdescr );
        h += 2;
        break;
      case JMP2:
      case INCR:
      case SETBASE:
      case A_SETBASE:
      case DECR:
      case LOAD:
      case A_LOAD:
      case SUBR:
      case RETURN:
      case NIL_TEST:
      case NILM_TEST:
      case NILV_TEST:
      case NILTV_TEST:
      case NILSTRING_TEST:
      case A_NIL_TEST:
      case A_NILM_TEST:
      case A_NILV_TEST:
      case A_NILTV_TEST:
      case A_NILSTRING_TEST:
      case A_LI_CHECK:
      case LI_CHECK:
      case A_STR_CHECK:
      case STR_CHECK:
      case LOAD_ARITY:
      case A_LOAD_ARITY:
      case L_LOAD_B:
      case L_LOAD_A:
      case AS_LOAD:
      case PM_CUTT:
      case END_DOLL:
      case GEN_BLOCK:
        h++;
        break;
      case JMP:
      case TY_TEST:
      case A_BOOL_TEST:
      case A_TY_TEST:
      case SKIPSKIP:
      case IN_SKIPSKIP:
      case LA_SKIPSKIP:
      case LA_IN_SKIPSKIP:
      case PM_DOLL:
      case IN_PM_DOLL:
      case PM_PUSH:
      case ATOM_TEST:
      case A_ATOM_TEST:
      case DEL_BLOCKS:
        h += 2;
        break;
      case TESTN:
      case A_TESTN:
      case TESTV:
      case A_TESTV:
        h += 3;
        break;
      default:
        h++;
        DBUG_PRINTE
        (
          "ERRS",
          ("ERROR : unknown matchcode (%8.8x) at position %d !", (*code)[h], h)
        );
        break;
    }  /* switch */
  }  /* while */
  
  /* empfangen der im pattern auftretenden Variablen */


   /* Offset auf die Liste der gebundenen Variablen */
   offset = (int)((* code)[NAMELIST]);

   /* Anzahl der gebundenen Variablen */
   anz_bv_var = (int) ((* code)[offset]);

  /*
  for ( h = (int)((*code)[2])  + 1 ; h <= laenge ; h++ )    
    (*code)[h] = (T_HEAPELEM) receiverec( fdescr );
  */
  for ( h = 1; h <= anz_bv_var ; h++ )  
     (* code)[offset + h] = (T_HEAPELEM) receiverec( fdescr );
   


#else

  if ( (* code)[1] != (T_HEAPELEM) 0 )
    (* code)[1] = (T_HEAPELEM) receiverec( fdescr );
  h = 2;
  while ( h <= laenge )
  {
    switch( (int) ((*code)[h]) )
    {
      case EOT:
        if ( h != laenge )
          DBUG_PRINT
          (
            "SEND",
            ("WARNING : length of codevector (%d) should be %d.", laenge, h)
          );
        h = laenge + 1;     /* Um die for-Schleife zu beenden.  */
        break;
      case KTEST:
      case A_KTEST:
      case STR_TEST:
      case DIG_TEST:
      case SCAL_TEST:
      case MATRIX_TEST:
      case BOOL_TEST:
      case VECTOR_TEST:
      case TVECTOR_TEST:
      case IN_TEST:
      case IN_TEST1:
      case A_STR_TEST:
      case A_DIG_TEST:
      case A_SCAL_TEST:
      case A_MATRIX_TEST:
      case A_VECTOR_TEST:
      case A_TVECTOR_TEST:
        if ( T_POINTER((*code)[h+1]) && ((*code)[h+1] != (T_HEAPELEM) 0) )
          (* code)[h+1] = (T_HEAPELEM) receiverec( fdescr );
        h += 2;
        break;
      case JMP2:
      case INCR:
      case SETBASE:
      case A_SETBASE:
      case DECR:
      case LOAD:
      case A_LOAD:
      case SUBR:
      case RETURN:
      case NIL_TEST:
      case NILM_TEST:
      case NILV_TEST:
      case NILTV_TEST:
      case NILSTRING_TEST:
      case A_NIL_TEST:
      case A_NILM_TEST:
      case A_NILV_TEST:
      case A_NILTV_TEST:
      case A_NILSTRING_TEST:
      case A_LI_CHECK:
      case LI_CHECK:
      case A_STR_CHECK:
      case STR_CHECK:
      case LOAD_ARITY:
      case A_LOAD_ARITY:
      case L_LOAD_B:
      case L_LOAD_A:
      case AS_LOAD:
      case PM_CUTT:
      case END_DOLL:
      case GEN_BLOCK:
        h++;
        break;
      case JMP:
      case TY_TEST:
      case A_BOOL_TEST:
      case A_TY_TEST:
      case SKIPSKIP:
      case IN_SKIPSKIP:
      case LA_SKIPSKIP:
      case LA_IN_SKIPSKIP:
      case PM_DOLL:
      case IN_PM_DOLL:
      case PM_PUSH:
      case ATOM_TEST:
      case A_ATOM_TEST:
      case DEL_BLOCKS:
        h += 2;
        break;
      case TESTN:
      case A_TESTN:
      case TESTV:
      case A_TESTV:
        h += 3;
        break;
      default:
        h++;
        DBUG_PRINTE
        (
          "ERRS",
          ("ERROR : unknown matchcode (%8.8x) at position %d !", (*code)[h], h)
        );
        break;
    }  /* switch */
  }  /* while */
#endif
}  /* receive_code */


/* ------------------------------------------------------------------------ */
/*                                                                          */
/*  receiverec  -   Graphen empfangen                                       */
/*                                                                          */
/* ------------------------------------------------------------------------ */

static PTR_DESCRIPTOR receiverec( fdescr )
  int fdescr;

  /*
    holt aus der Datei, die durch fdescr beschrieben wird, einen Deskriptor,
    alloziert ihn und liest, falls noetig, rekursiv seine Unterbaeume.
    receive gibt die Adresse des allozierten Deskriptors zurueck.
  */

{
  int h;
  DESC_CLASS class1;
  PTR_DESCRIPTOR dirdesc;

  if (( h = receive_header( fdescr) ) != NOT_SENDED )
  {
    PTR_DESCRIPTOR hd;

    kdesc++;                    /* Zahl der wiederholt empf. Desk. erhoehen */
    hd = already_received( h );
   /* Der Deskriptor ist schon frueher alloziert worden       */
    INC_REFCNT( hd );
   /* Es ist also nur notwendig, seine Adresse zurueckzugeben */
    return( hd );
  }
  else
  {
    PTR_DESCRIPTOR dPtr;
    int dim;

    dPtr = newdesc();
    if ( dPtr == (PTR_DESCRIPTOR) 0 )
    {
      DBUG_PRINTE( "ERRS", ("ERROR : can't allocate descriptors anymore !") );
      post_mortem_intern();
    }
    ndesc++;                    /* Zahl der neu empfangenen Desk. erhoehen. */
    /* Deskriptor einlesen  */
    receive_descr( fdescr, dPtr );
#if DEBUG
    print_desc_c_t( FALSE, dPtr );
#endif
    if ( (dim = R_LIST( (* local_tab), dim )) == local_filled )
    { /* local_tab voll! */
      PTR_DESCRIPTOR hT, * sp, * dp;
      register int i;

      if ( (hT = OS_create_list( dim * 3 / 2 )) == (PTR_DESCRIPTOR) 0 )
        post_mortem_intern();
      DBUG_PRINT
      (
        "SEND",
        ("Allocated space for %d table entries.", dim * 3 / 2)
      );
      sp = ( PTR_DESCRIPTOR *)( R_LIST( (*local_tab), ptdv));
      dp = ( PTR_DESCRIPTOR *)( R_LIST( (*hT), ptdv ));
      for ( i = dim; i > 0; i-- )
        *(dp++) = *(sp++);
      freeheap( R_LIST( (*local_tab), ptdv ));        /* Platz zurueckgeben   */
      freedesc( local_tab );                          /* Deskriptor zurueck   */
      local_tab = hT;
    }  /* THEN ( local_tab voll ) */

    /* in die local_tab eintragen u. Nummer erhoehen */
    *((PTR_DESCRIPTOR*)(R_LIST((*local_tab), ptdv)) + local_filled++) = dPtr;
    class1 = R_DESC( (*dPtr), class );
    switch( class1 )
    {
    case C_SCALAR:
      break;
    case C_DIGIT:
      receive_data
      (
        fdescr,
        R_DIGIT( (*dPtr), ndigit),
        (T_HEAPELEM **)(A_DIGIT( (*dPtr), ptdv))
      );
      break;
    case C_LIST:
      if ( R_LIST( (*dPtr), ptdd ) != (T_PTD) 0 )           /* indirekt ?   */
      {
        int offset;
        INC_IDCNT;
        read_intern( fdescr, &offset, sizeof(offset) );
        L_LIST( (*dPtr), ptdd ) = dirdesc = receiverec( fdescr );
        L_LIST( (*dPtr), ptdv ) =  R_LIST( (*dirdesc), ptdv ) + offset;
      }
      else
        receive_list
        (
          fdescr,
          (int)(R_LIST( (*dPtr), dim )),
          (T_HEAPELEM **)(A_LIST( (*dPtr), ptdv ))
        );
      break;
    case C_MATRIX:
    case C_VECTOR:
    case C_TVECTOR:
      if ( R_MVT( (*dPtr), ptdd, class1 ) != (T_PTD) 0 )    /* indirekt ?   */
      {
        int offset;
        INC_IDCNT;
        read_intern( fdescr, &offset, sizeof(offset) );
        L_MVT( (*dPtr), ptdd, class1 ) = dirdesc = receiverec( fdescr );
        L_MVT( (*dPtr), ptdv, class1 ) = 
          R_MVT( (*dirdesc), ptdv, class1 ) + offset;
      }
      else
        receive_list
        (
          fdescr,
          (int)(R_MVT((*dPtr), nrows, class1)*R_MVT((*dPtr), ncols, class1) ),
          (T_HEAPELEM **)(A_MVT( (*dPtr), ptdv,  class1 ))
        );
      break;
    case C_CONSTANT:
    case C_EXPRESSION:
      switch( R_DESC( (*dPtr), type ))
      {
      case TY_COND:
        receive_heapelem( fdescr, A_COND( (*dPtr), ptte ));
        receive_heapelem( fdescr, A_COND( (*dPtr), ptee ));
        break;
      case TY_EXPR:
        receive_heapelem( fdescr, A_EXPR( (*dPtr), pte ));
        break;
      case TY_SPEC:
        if
        (
          R_SPEC( (*dPtr), ptdd ) !=0
          && T_POINTER( (T_HEAPELEM)(R_SPEC( (*dPtr), ptdd )))
        )
          L_SPEC( (*dPtr), ptdd ) = receiverec( fdescr );  /* !Deskriptor  */
        if ( R_SPEC( (*dPtr), i_stack ) != (PTR_DESCRIPTOR)0 )
          L_SPEC( (*dPtr), i_stack ) = receiverec( fdescr );
        if ( R_SPEC( (*dPtr), tilde_stack ) != (PTR_DESCRIPTOR)0 )
          L_SPEC( (*dPtr), tilde_stack ) = receiverec( fdescr );
        break;
      case TY_VAR:
        /* HEAP-Region noetig fuer die naechste Zuweisung??                 */
        L_VAR( (*dPtr), ptnd ) = receiverec( fdescr );
        break;

/* US */
#if DREDUMA
      case TY_FNAME:  
        receive_heapelem( fdescr, A_FNAME( (*dPtr), ptn) ); 
        break;  

#endif 
      case TY_NAME:
        receive_heapelem( fdescr, A_NAME( (*dPtr), ptn ));
        break;     
      case TY_SUB:
#if !DREDUMA
      case TY_REC:
#endif
        receive_heapelem( fdescr, A_FUNC( (*dPtr), namelist ));
        if ( R_FUNC( (*dPtr), pte ) != NULL )                   /* DREDUMA  */
          receive_heapelem( fdescr, A_FUNC( (*dPtr), pte ));
        break;
      case TY_SWITCH:
        receive_heapelem( fdescr, A_SWITCH( (*dPtr), ptse ));
        break;
      case TY_MATCH:
        if ( R_MATCH( (*dPtr), code ) != (PTR_HEAPELEM) 0 )
          receive_code( fdescr, A_MATCH( (*dPtr), code ));      /* Code     */
        if ( R_MATCH( (*dPtr), guard ) != (PTR_HEAPELEM) 0 )
          receive_heapelem( fdescr, A_MATCH( (*dPtr), guard )); /* Guard    */
        receive_heapelem( fdescr, A_MATCH( (*dPtr), body ));    /* Body     */
        break;
      case TY_NOMAT:
        if ( R_NOMAT( (*dPtr), guard_body ) != (PTR_HEAPELEM) 0 )
          receive_heapelem( fdescr, A_NOMAT( (*dPtr), guard_body ));
        /* HEAP-Region noetig fuer die naechste Zuweisung??                 */
        L_NOMAT( (*dPtr), ptsdes ) = receiverec( fdescr );  /* !Deskriptor  */
        break;
      case TY_LREC:
        receive_heapelem( fdescr, A_LREC( (*dPtr), namelist )); 
/* US */
#if DREDUMA
        receive_list
        (
          fdescr,
          ( R_LREC( (*dPtr), nfuncs ) + 1 ),
          A_LREC( (*dPtr), ptdv )
        );        
#else
        receive_list
        (
          fdescr,
          R_LREC( (*dPtr), nfuncs ),
          A_LREC( (*dPtr), ptdv )
        );        
#endif
        break;
      case TY_LREC_IND:
        /* HEAP-Region noetig fuer die naechste Zuweisung??                 */
        L_LREC_IND( (*dPtr), ptdd) = receiverec( fdescr );
        /* HEAP-Region noetig fuer die naechste Zuweisung??                 */
        L_LREC_IND( (*dPtr), ptf) = receiverec( fdescr );
        break;
#if !DREDUMA
      case TY_LREC_ARGS:
        /* HEAP-Region noetig fuer die naechste Zuweisung??                 */
        L_LREC_ARGS( (*dPtr), ptdd) = receiverec( fdescr );
        receive_list
        (
          fdescr,
          R_LREC_ARGS( (*dPtr), nargs ),
          A_LREC_ARGS( (*dPtr), ptdv )
        );
        break;
#endif
/*    case TY_CLOSURE:
        break;   */
      }  /* SWITCH ( Typ des Deskriptors )   */
      break;
      default:
        DBUG_PRINTE
        (
          "ERRS",
          ("ERROR : descriptor %8.8x has unknown class %d !", (int)dPtr, class1)
        );
        post_mortem_intern();
        break;
    }  /* SWITCH ( Klasse des Deskriptors ) */
    return (dPtr);
  }  /* ELSE ( Descriptor empfangen )   */
}  /* receiverec */

/* End of os-send.c */
