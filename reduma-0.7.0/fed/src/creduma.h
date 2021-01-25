# 890404   last change, format: yymmdd
# creduma.hlp
#
# format of a help-entry for xxx:
# ^xxx
#   Info zu xxx
# ^next-help
#
#-------------------------------------+
# help for editor and reduma          |
#-------------------------------------+

^help
+-----------------------------------------------------------------------------+
|                         help                              April - 4 - 1989  |
+-----------------------------------------------------------------------------+

please type one of the following numbers or names for further
information:

 1. editor  ( calling the editor )
 2. led     ( the handling of the line-editor )
 3. keys    ( function keys )
 4. cmd     ( the command interpreter )
 5. sys     ( definition of the system parameters )
 6. files   ( the file system )
 7. syntax  ( the syntax of the reduction language )
 8. reduma  ( reduction language elements )
 9. index   ( index of detailled help information )

^editor ^1 ^1.
+-----------------------------------------------------------------------------+
|                         editor                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 call :  reduma [-options] [ <file> ]
 exit :  CTRL-C, CTRL-Z or command exit

 operation:
  Syntax-directed editing of expressions of the reduction language.
  The basic principle is to replace the cursor expression by another
  expression. The input from the keyboard is echoed in the Input field.
  It is analyzed and the cursor expression is replaced by it if the input
  is syntactically correct.

 options:
  t: reduce one file and display the time needed (batch-mode).
  i: use the specified file as initialization file instead of red.init.
  c: reduce the first file and compare the result with the second
     (batch-mode).
  p: print all specified editor files in the prettyprint format
     using the extension .pp instead of .ed (batch-mode).

^led ^2 ^2.
+-----------------------------------------------------------------------------+
|                         led                               April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 The line editor handles all inputs in the Input field.
 The line editor will be terminated by <cr> to enter the input
 or <CTRL>-C to cancel the input.
 The bounds of the currently displayed segment of the input line
 (which can be longer than the physical input line) is shown
 in the right corner of the Message field.

 Special functions:

 F3,<CTRL>-E    insertmode on/off.
 DEL            deletes the character left of the cursor.
 F4             deletes the character at the cursor position.
 <CTRL>-D       deletes all character from the cursor position
                to the end of the input line.
 <cursor-home>  positions the cursor at the first input character.
 <cursor-up>    moves the displayed segment a half line to the left.
 <cursor-down>  moves the displayed segment a half line to the right.
 <tab>          moves the cursor to the next tab stop.

^keys ^3 ^3.
+-----------------------------------------------------------------------------+
|                         keys                              April - 4 - 1989  |
+-----------------------------------------------------------------------------+

function keys:

 F1 : reduce                         F9  : reduce 1000 steps
 F2 : read file                      F10 : write file
 F3 : display                        F11 : display father-expression
 F4 : -                              F12 : -
 F5 : read backupstack               F13 : write backupstack
 F6 : read auxiliary stack 1         F14 : write auxiliary stack 1
 F7 : read auxiliary stack 2         F15 : write auxiliary stack 2
 F8 : execute command                F16 : exit

 ( PCS: F1 - F8 => SHIFT+F1 - SHIFT+F8, F9 - F16 => CTRL-F1 - CTRL-F8 )

 DEL : delete (list)expression

All function keys may be simulated by typing : and the number of the
number of the wanted key plus <cr>.

^cmd ^4 ^4.
+-----------------------------------------------------------------------------+
|                         cmd                               April - 4 - 1989  |
+-----------------------------------------------------------------------------+

The command line interpreter of the editor:

 call : function key F8

 editor commands:
  append <file>    : append prettyprint to <file>
  arity            : display constructor arity

  cat <file>       : show the content of <file>
  cmd <shell_cmd>  : issue <shell_cmd> to a shell
  comp             : compare expression on auxiliarystack 1 with the
                     cursor expression

  defkeys          : define function and cursor keys
  dmode [<number>] : define depth to display expressions (-1: all)
  doc [<file>]     : edit a file (file type 2) with the editor
                     specified by the Shell variable EDITOR.

  editparms        : show editor parameters
  exit             : leave editor
  exitgraph        : leave editor and return to GRAPH
  ext [<i> [<extension>]] : define the file extension for the various
                     types of files.
    <i> may be one of the following values (default):
     0: Input/Output   (.ed)
     1: Prettyprint    (.pp)
     2: Document       (.doc)
     3: Protocoll      (.prt)
     4: Portable/Ascii (.asc)
     5: Reduction      (.red)
     6: Preprocessed   (.pre)

  find             : try to find the expression on auxiliarystack 1 and
                     select it as the new cursor expression if found

  help [<text>]    : search for help

  initfile [<file>]: change the name of the initilization file
                     to <file> [default: red.init]
  initparms        : read parameter from initfile

  load [<file>]    : load an expression from file (file type 5)
  lib [<i> [<directory>]] : define the directory for the various
                     types of files.

  news             : show news (from help file)
  next             : try to find the next occurrence of the expression on
                     auxiliarystack 1 right of the cursor expression

  pp     <file>    : prettyprint to <file> (file type 1)
  print  <file>    : print to <file> (file type 4)
  protocol <file>  : append after each reduction the result
                     to the specified file
  protocol         : stop protocolling

  read  [<file>]   : read textexpression from <file> (file type 4)
  redcnt <number>  : set the default reduction counter to <number>
  reduce <rc>      : perform <rc> reduction steps
  redparms         : show reduction parameters
  refresh          : refresh screen

  saveparms        : save reduction and editor parameters
  shell            : call another UNIX shell
  small on | off   : set display mode
  stack            : show stack content
  store [<file>]   : save an expression (file type 5)

  time             : show time used by last reduction
  version          : show current editor version
  width <number>   : define width for output files (file types 1, 4)

 Most of the commands may be abbreviated.
^sys ^syscmd ^5 ^5.
+-----------------------------------------------------------------------------+
|                 system modifications only                 April - 4 - 1989  |
+-----------------------------------------------------------------------------+

  heapdes <number>         : number of heap descriptors
  heapsize <number>        : heap size
  qstacksize <number>      : stacksize e,a,h
  mstacksize <number>      : stacksize m,m1
  istacksize <number>      : stacksize i
  fixformat                : formatted numbers
  varformat                : unformatted numbers
  base                     : base of the numerical representation
                             (varformat only)
  beta_count               : count only beta-reductions
  truncation               : number of decimal digits to be displayed
  precision                : precision for division       (see div_prec)
  mult_precision           : precision for multiplication (see mult_prec)

^file ^files ^6 ^6.
+-----------------------------------------------------------------------------+
|                            files                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 Following file types exist:

  type | comment                             | call                | default-
       |                                     |                     | extension
-------+-------------------------------------+---------------------+----------
  0    | input/output files                  | F2                  | .ed
       | internal editor format              | F10                 |
       | including version                   | $FILE               |
-------+-------------------------------------+---------------------+----------
  1    | prettyprint files                   | F8 pp [<file>]      | .pp
-------+-------------------------------------+---------------------+----------
  2    | document files                      | F8 doc [<file>]     | .doc
       | abbreviation for the system command |                     |
       | $EDITOR <file>.doc                  |                     |
-------+-------------------------------------+---------------------+----------
  3    | protocol files                      | F8 prot <file>      | .prt
       |                                     | F8 prot             |
-------+-------------------------------------+---------------------+----------
  4    | portable/ascii files                | F8 print [<file>]   | .asc
       | special external format for         | F8 read  [<file>]   |
       | editing with text editors           |                     |
-------+-------------------------------------+---------------------+----------
  5    | input/output files                  | F8 store [<file>]   | .red
       | internal reduction format           | F8 load [<file>]    |
       |                                     | ^FILE               |
-------+-------------------------------------+---------------------+----------
  6    | input/output files                  | F8 prepr [<file>]   | .pre
       | preprocessed format                 | !FILE               |
------------------------------------------------------------------------------

For each file type exists a default directory and a default extension.
The initialization file contains these values.

^beta ^beta_count
+-----------------------------------------------------------------------------+
|                         beta_count                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 This command enables or disables the counting of delta reductions
 which are applications of primitive functions.
 For each applicator of a user defined function application the
 reduction counter is decremented by one.
 If beta_count is off, each delta reduction decrements the
 reduction counter by one, independant of the number of
 specified applicators.

 usage: beta on, beta off

^reduce ^r ^trunc ^prec ^rc ^pr ^tr
+-----------------------------------------------------------------------------+
|                    reduce                                 April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 A reduction is started by entering the command reduce or hitting
 F1 (number of reduction steps set by -> redcnt)
 or F9 (thousand reduction steps).
 Optional parameters for the command reduce are the number of reduction
 steps, -> div_precision and -> truncation.

^reduma ^7 ^7.
+-----------------------------------------------------------------------------+
|                          reduma                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+

Informations are available for the the following reduction language elements:

1. Constructors:
===============================================================================
   sub, \, ap, <, ', if, then,
   mmat, mat, bmat, smat, vect, bvect, svect, tvect, btvect, stvect,
   case, caserec, when, as, in, <(, otherwise, match, "as, "in

2. Primitive functions:
===============================================================================
                value transforming functions              | queries
   -------------------------------------------------------|--------------------
     arithmetic/      | trans-   |   relational    | logic| univer- | special
     transfer         | cendental|                 |      | sal     |
   -------------------+----------+-----------------+------+---------+----------
   +   , c_+  , vc_+  |   sin    |     eq ,     ne | and  |  dim    | ldim
   -   , c_-  , vc_-  |   cos    |   f_eq ,   f_ne | not  |         | mdim
   *   , c_*  , vc_*  |   tan    |     ge ,     gt | or   |         | vdim
   /   , c_/  , vc_/  |   exp    |     le ,     lt | xor  |  class  |
   mod , ip   ,       |   ln     |                 |      |  empty  |
   abs , ceil , floor |          |                 |      |  type   |
   neg , frac , trunc |          |                 |      |         |
      max ,    min    |          |                 |      |         |
    c_max ,  c_min    |          |                 |      |         |
   vc_max , vc_min    |          |                 |      |         |

                        structure transforming functions
   ----------------------------------------------------------------------------
   universal   |             special functions                | conver-
   functions   | lists/strings  | matrices/vectors/Tvectors   | ting
   ------------+----------------+-----------------------------+----------------
   cut         |   lcut         |  mcut          vcut         | to_scal
   replace     |   lreplace     |  mreplace      vreplace     | to_vect
               |                |  mreplace_c                 | to_tvect
               |                |  mreplace_r                 | to_mat
   rotate      |   lrotate      |  mrotate       vrotate      | ltransform
   select      |   lselect      |  mselect       vselect      | transform
   unite       |   lunite       |  munite        vunite       |
               |   reverse      |  transpose     transpose    |
               |   repstr       |                             |
               |   substruct    |                             |


3. Other elements of the language:
===============================================================================
   true     , false    , <> , '` , nilmat    , nilvect   , niltvect ,
   Primfunc , Userfunc , Number  , String    , Bool      , List     ,
   Matrix   , Vector   , Tvector , Function  , Scalar    , 1e~10    ,
   ~        , guard    , do      , end       , ...       ,  .       ,
   $        , "...     , ".      , "$        , fail      , ok       ,
   $FILE    , #

^structure ^structure transforming ^structure transforming functions
+-----------------------------------------------------------------------------+
|                structure transforming functions           April - 4 - 1989  |
+-----------------------------------------------------------------------------+

Following structure transformations may be specified:

 - generation of substructures
   (lcut , repstr ,substruct , vcut , mcut , cut)

 - selection of substructures
   (substruct , lselect , vselect , mselect , select)

 - generation of a new structure by replacing a substructure
   (lreplace , repstr , vreplace , mreplace , mreplace_c , mreplace_r,
    replace)

 - changing the positions of the elements of a structure
   (lrotate , reverse , vrotate , mrotate , transpose , rotate)

 - concatenation of two structures of the same type
   (lunite , repstr , vunite , munite , unite)

 - converting a structure to another class
   (ltransform , transform , to_scal , to_vect , to_tvect , to_mat)

^ordering
+-----------------------------------------------------------------------------+
|                      defined orderings                    April - 4 - 1989  |
+-----------------------------------------------------------------------------+

boolean values :
    true  >  false

strings :
 Strings may be lexicographically ordered. The underlying alphabet
 corresponds to the ASCII-code.
 Nested strings are ordered with respect to their length. Elements
 of strings which are strings themselves are greater than a single
 ASCII-character.

^relational functions ^relational ^rela
+-----------------------------------------------------------------------------+
|                    relational functions                   April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 The result of a function application is a boolean value or a matrix, vector
 or transposed vector of type boolean:
              eq , ne , f_eq , f_ne , ge , gt , le , lt

 (-> ordering)

^< ^list
+-----------------------------------------------------------------------------+
|                            <                              April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 Lists are constructed with the symbol < . The subexpressions of this
 constructor may be arbitrary expressions of the reduction language.

 Examples:    <1, 2, (1 + 2), true>
               < < 'a` >, < 'b`> >

^' ^strings ^text ^char ^characters ^char_string
+-----------------------------------------------------------------------------+
|                         strings                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 Character strings (texts) are constructed with the symbol ' .
 They may be hierarchically nested like list structures.
 The elements of character strings may be all legal ASCII-characters.

 Examples:        'This is a string`
                  'This is a 'nested` string`
                  'This 'is` a string too`
^$FILE ^$file
+-----------------------------------------------------------------------------+
|                         $<file>                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 $<file> is an abbreviation for a syntactically correct expression of the
 reduction language which is stored in the specified <file> with file type 0.
 
 Attention: the file name must be written in upper case letters !

 Example :  computation of the number 'PI` , where the applicative program
            is stored in the file PIPROG.ed

             ap sub [ RG ]
                in   if (RG gt 0)
                then { 'File PIPROG.ed` } $PIPROG
                else 'The argument (relative error) has to be greater than 0`
             to [ 1e~63 ]

^#
+-----------------------------------------------------------------------------+
|                              #                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 The hash symbol # is a placeholder. It represents an unspecified
 expression.

 A reduction language expression which contains this placeholder
 can`t be reduced.

 Examples :             cut( # , # , # )

                        if #
                        then #
                        else #

^\
+-----------------------------------------------------------------------------+
|                                \                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 The constructor \ (protect) is the complement to the binding operator of the
 lambda-calculus.
 Each \ protect the variable against one binding of a variable with the
 same name.

 Example:   ap sub [ V ] 
               in sub [ V ]
                  in < V , \V >
            to [ X ]
   reduces to
            sub [ V ]
            in < V , X >

^Number ^number ^fixformat ^varformat ^fix ^var ^real ^integer ^digit ^1e~10 ^decimal
+-----------------------------------------------------------------------------+
|                          numbers                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 There are two different internal representation of numbers, which
 can be selected by one of the following commands:

  1. fixformat
   All numbers will be stored in the internal format of the host machine.
   If a number has a decimal part or it is too big for an integer represen-
   tation, it will be stored in the internal real-format.

  2. varformat
   All numbers will be stored as a sequence of digits.
   The base of the representation is defined by the value of (->) base.
   The number may be arbitrary large, only the functions

            / , c_/ , vc_/ , sin , cos , tan , exp , ln

     may not be computed exactly.
     ( / , c_/ , vc_/ , mod  are exact for integers)

     The parameter  div_precision specifies, how many decimal digits
     are to be computed exact.

 Examples:   1         1987
             12.345    0.1234
            ~12 (-> ~)
             1e12      3.14e~10      ~0.987e10

^~
+-----------------------------------------------------------------------------+
|                                 ~                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 This character is the unary minus in a representation of a number
 (do not use the symbol -, which is the binary minus function).

 Examples:    ~2
              1e~2 ( = 0.01 )

^truncation
+-----------------------------------------------------------------------------+
|                         truncation                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 This parameter defines the number of decimal digits to be displayed
 after a reduction on the screen.
 If it is -1, all decimal digits will be displayed (this means no
 truncation will be performed).

Example :   truncation = 5
            Expression : 10.000002
            will be reduced to :  10

^mult_prec ^mult_precision
+-----------------------------------------------------------------------------+
|                         mult_precision                    April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 This reduction machine parameter specifies the precision of the
 multiplication, if the varformat is selected.

 If the (default) value -1 is selected, the computation will be exact.

^div_prec ^div_precision ^precision
+-----------------------------------------------------------------------------+
|                      (div_)precision                      April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 This reduction machine parameter specifies the precision of the
 division, if the varformat is selected.

 If the (default) value -1 is selected, the computation will be exact.

 The value determines the number of decimal digits, which are computed
 "exactly" during a division.

 Examples :   div_prec = 10,      truncation  = 10
              ( 1 / 3 ) reduces to 0.333333333

              div_prec = 3,       truncation  = 3
              ( 1 / 3 ) reduces to 0.333

^base
+-----------------------------------------------------------------------------+
|                             base                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 This reduction machine parameter specifies the base of the numerical
 representation (varformat only).
 Each number is internal represented by a list of digits to this base.

 A value of base equal to 10000 is highly recommended.

                          10 <= base < 65536

    Achtung: Besteht die Primfaktorzerlegung der Basis nicht genau aus
    2'en und 5'en, so kann es durch Rundungsfehler gegenueber dem
    Dezimalsystem zu verfaelschten Ergebnissen kommen.

^generic
+-----------------------------------------------------------------------------+
|                              generic                      April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 
^abs
+-----------------------------------------------------------------------------+
|                               abs                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 Examples: abs( 1 )                  =>  1
           abs( ~2 )                 =>  2
           abs( mat
                < 1, ~2, ~3 >. )     =>  mat
                                         < 1, 2, 3 >.
           abs( vect< 1, ~2, ~3 > )  =>  vect< 1, 2, 3>
           abs( tvect< 1,
                       ~2,
                       ~3 > )        =>  tvect< 1,
                                                2,
                                                3 >

^neg
+-----------------------------------------------------------------------------+
|                               neg                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 Examples: neg( 1 )                  =>  ~1
           neg( ~2 )                 =>  2
           neg( mat
                < 1, ~2, ~3 >. )     =>  mat
                                         < ~1, 2, 3 >.
           neg( vect< 1, ~2, ~3 > )  =>  vect< ~1, 2, 3>
           neg( tvect< 1,
                       ~2,
                       ~3 > )        =>  tvect< ~1,
                                                2,
                                                3 >


^and ^or ^xor
+-----------------------------------------------------------------------------+
|                   and, or, xor                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 Examples :
 ( true and true )                                =>  true
 ( true and false )                               =>  false
 ( false and true )                               =>  false
 ( false and false )                              =>  false
 ( false and btvect< true,
                     false > )                    =>  btvect< false,
                                                              false >
 ( bvect< true, false > and bvect< false, false > )  =>  bvect< false, false >
 ( false and  bmat
              < true, false >, 
              < false, true >. )                   =>  bmat
                                                       < false, false >, 
                                                       < false, false >.

 ( true or true )                                  =>  true
 ( true or false )                                 =>  true
 ( false or true )                                 =>  true
 ( false or false )                                =>  false
 ( bvect< false, true > or bvect< true, false > )  =>  bvect< true, true >

 ( true xor true )                                 =>  false
 ( false xor false )                               =>  false
 ( false xor true )                                =>  true
 ( false xor false )                               =>  true
 ( true xor bvect< true, false > )                 =>  bvect< false, true >

^ap ^apply
+-----------------------------------------------------------------------------+
|                               ap                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:      ap function
             to [ arg1 , arg2 , ... , argn ]

 The constructor ap specifies the application of a function to a list
 of arguments.
 The above usage shows the explicit notation of an application.
 For easier use, the language supplies prefix and infix application too.

 explicit                  infix               prefix
 -------------------------+-------------------+-------------------------
 ap -                     | ( 1 - 2 )         | minus( 1, 2)
 to [ 1, 2]               |                   |
                          |                   |
 ap transpose             |                   | transpose( vect< 1, 2, 3 > )
 to [ vect< 1, 2, 3 > ]   |                   |
                          |                   |
 ap F                     |                   | F [ 3 ]
 to [ 3 ]                 |                   |
                          |                   |
 ap F                     | ( 1 F 2 )         | F [ 1, 2 ]
 to [ 1, 2 ]              |                   |
                          |                   |
 ap ap cut                |                   | cut( 0, 1, < 1, 2, 3 > )
    to [ 0 ]              |                   |
 to [ 1, < 1, 2, 3 > ]    |                   |

^prefix
+-----------------------------------------------------------------------------+
|                        prefix notation                    April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 The following table shows the deviation from the normal notation, if the
 primitive function is used as a prefix application:

 +   ==>  plus  |  c_+   ==>  c_plus    |  vc_+   ==>  vc_plus
 -   ==>  minus |  c_-   ==>  c_minus   |  vc_-   ==>  vc_minus
 *   ==>  mult  |  c_*   ==>  c_mult    |  vc_*   ==>  vc_mult
 /   ==>  div   |  c_/   ==>  c_div     |  vc_/   ==>  vc_div


^if ^if_ ^then ^else ^if_then_else ^conditional
+-----------------------------------------------------------------------------+
|                      if then else                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+

^Pattern-Matching ^match ^Pattern ^Matching ^OK ^FAIL ^UNDECIDED ^pattern ^pattern_matching
+-----------------------------------------------------------------------------+
|                      pattern - matching                   April - 4 - 1989  |
+-----------------------------------------------------------------------------+

Fuer  die  usage    der  Case-function muss ueberprueft werden, ob
das Argument im Definitionsbereich einer   der  Pattern-Matches    der
Case-function        liegt,  d.h.  ob  einer der Patternausdruecke der
Pattern-Matches  auf  den Argumentausdruck passt.  Der  Vergleich  der
Patternausdruecke  mit  dem  Argumentausdruck  erfolgt  mit  Hilfe des
Pattern-Matching Algorithmus. Waehrend  dieses Vergleichs  werden  die
Variablen    des    Pattern    mit  Unterausdruecken    des  Arguments
assoziiert.

Anmerkungen:
- Die Patternausdruecke muessen linear sein, d.h. eine Variable darf
  nur einmal in einem Patternausdruck vorkommen.
- Auf die Listenelemente '$`, as '$` <:variable:>, '...`,
  as '...` variable, muss ein Listenelement ungleich $`,
  as '$` <:variable:>, '...`, as '...` variable, folgen.

Vergleicht      der    Pattern-Matching Algorithmus  eine  Applikation
oder eine Variable des  Argumentausdruck  mit  einer  Konstanten  oder
einer  Liste  des  Patternausdrucks,  kann nicht  entschieden  werden,
ob  das Pattern auf das Argument passt. Nach weiteren Transformationen
des   Programmausdrucks  kann  der Argumentausdruck so umgeformt sein,
dass ein Vergleich fehlschlaegt oder aber das Pattern auf das Argument
passt.

Die    function    "match"  beschreibt in den folgenden Examplesn den
Vergleich des Argumentausdrucks mit dem Patternausdruck. Die  function
liefert  ein  Paar,    dessen    erste  Komponente  das  Ergebnis  des
Matching  anzeigt. Steht in der ersten Komponenten
- OK, passt der Patternausdruck auf  das  Argument
- FAIL passt der Patternausdruck nicht auf das Argument
- UNDECIDED,  kann  das  Matching  nicht  entschieden werden, weil ein
  Element eines Patternausdrucks  mit  einer  Applikation  oder  einer
  Variablen des Argumentausdrucks verglichen wird.
Die  zweite  Komponente  enthaelt eine Menge von Paaren, durch die den
Variablen des Pattern Reduktionssprachenausdruecke zugeordnet werden.

Informationen  zum Matching der Patternausdr}cke sind unter den Stich-
worten  pattern_variable, ., pattern_constant, pattern_list,  ...,  $,
as und List zu finden.

^pattern_variable ^. ^'.`
+-----------------------------------------------------------------------------+
|                pattern - variable, .                      April - 4 - 1989  |
+-----------------------------------------------------------------------------+

- Fuer      eine     Variable    oder    ein  Symbol  "."  duerfen  im
  Argumentausdruck beliebige   Reduktionssprachenausdruecke    stehen.
  Im   Gegensatz zum Symbol "."  assoziiert  der  Matching Algorithmus
  die Variable des Patternausdrucks mit dem Argumentausdruck.

  Examples:
  match( A,  ap  1 2 ) = <OK, ( A <-- ap  1 2 )>
  match( .,  ap  1 2 ) = <OK, ()>

- Eine  Variable kann mit einer Typangabe versehen sein. Dann passt die
  Variable nur dann auf das Argument, falls dieses den angegebenen Typ
  hat.

  Examples:
  match( . : Number , true ) = < FAIL, () >
  match( A : Number ,  3  ) = < OK, ( A <-- 3) >
  match( A : Number ,  B  ) = < UNDECIDED, () >

  weitere Informationen unter dem Stichwort Pattern-Matching

^Konstante ^pattern_constant ^'pattern_constant`
+-----------------------------------------------------------------------------+
|                    pattern - constant                     April - 4 - 1989  |
+-----------------------------------------------------------------------------+

- Ist  das  Pattern  eine  Konstante  (<:pattern_constant:>),  muss  im
  Argumentausdruck die gleiche Konstante stehen.

  Examples:
  match ( 1, 1) = <OK, ()>
  match ( 1, 'a`) = <FAIL, ()>
  match ( 1, ap  A B) = <UNDECIDED, ()>

  weitere Informationen unter dem Stichwort Pattern-Matching

^pattern_list  ^'pattern_list`
+-----------------------------------------------------------------------------+
|                 pattern - list                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+

- Der    Pattern-Matching  Algorithmus  vergleicht   Patternlisten mit
  Argumentausdruecken    wie  folgt:  Ist    das    Argument      eine
  Listenstruktur,    wird    ueberprueft,    ob  die Stelligkeiten des
  Listenkonstruktors des Arguments groesser gleich  der  Anzahl    der
  Elemente    ist,  die fuer ein Matching des Pattern auf das Argument
  mindestens   notwendig   sind.   Sind   ausreichend   Listenelemente
  vorhanden,     erfolgt   die Ueberpruefung   der Listenelemente.

  match( <  1  A  < B > > ,  <  1  2  <  3 > > ) =
                                        < OK, ( A <-- 2, B <-- 3) >

  Der Vergleich der Listenelemente ergibt:
  match( 1, 1 ) = < OK, () >, match( A, 2 ) = < OK, (A <-- 2) >
  match( < B >, < 3 > ) = < OK, ( B <-- 3 ) >


  Weitere  Informationen    unter  ...,  $ und as   oder allgemeine
  Informationen unter dem Stichwort Pattern-Matching.


^...  ^'...`
+-----------------------------------------------------------------------------+
|                        ...                                April - 4 - 1989  |
+-----------------------------------------------------------------------------+

  Das  Symbol "..."  ueberdeckt  beliebig    viele Listenelemente. Der
  Patternausdruck  < 1  ...  5 >   passt    beispielsweise    auf  das
  Argument    < 1  2  3  4  5 >. Das Patternelement ... ueberdeckt die
  Listenelemente 2, 3 und 4. Der Pattern-Matching Algorithmus versucht
  eine  Uebereinstimmung  der  Patternliste  mit  der Argumentliste zu
  erzielen,  bei  der  eine  minimale   Anzahl     von  Elementen  der
  Argumentliste    durch  das  Symbol  ...  ueberdeckt wird. Falls das
  Matching  der  auf   das   ...      folgenden  Patternlistenelemente
  fehlschlaegt, ueberdeckt ... ein weiteres Element der Argumentliste,
  und es erfolgt ein erneuter Versuch des Matching. Bedingung fuer den
  erneuten  Versuch  ist eine ausreichende Anzahl von Elementen in der
  Argumentliste, die auf  die noch  zu  vergleichenden   Elemente  der
  Patternliste    passen koennten.  Das Symbol ... an letzter Position
  der Patternliste ueberdeckt die Listenelemente der Argumentliste bis
  zum Listenende.

  Fuer  das  erste  Auftreten des ... im folgenden Example setzt der
  Pattern-Matching Algorithmus den Vergleich mit  dem  auf  das   ...
  folgende Patternelement fort.

  Example: Patternausdruck <   1   2  ...   3     4   5  ...    >
            Argumentausdruck <  1   2        3     3   4    5   5  >

  Das   Matching   der   4   mit   der   3   schlaegt   fehl,  und  das
  Argumentlistenelement   3  wird   zu  den   durch   ... ueberdeckten
  Listenelementen  gezaehlt.  In der Argmentliste sind noch ausreichend
  Elemente fuer  den Vergleich mit der 3,  4  und  5  der  Patternliste
  vorhanden.    Es    erfolgt    ein    erneutes    Matching    dieser
  Patternlistenelemente.

            Patternausdruck  <  1   2    ...  3     4    5  ... >
            Argumentausdruck <  1   2     3   3     4    5  5     >


  Examples:
  match( <  1 ...  3  4 ... , <  1 2 3 4 5 6 ) = <OK, ()>
  match( <  1 ...  3  5 ... , <  1 2 3 4 5 ) = <FAIL, ()>
  match( <  1 ...  3 B ... , <  1 2 3 4 5 ) = <OK, (B <-- 4)>

  Weitere Informationen sind unter dem Stichwort Pattern-Matching
  zu finden.

^$ ^'$`
+-----------------------------------------------------------------------------+
|                            $                              April - 4 - 1989  |
+-----------------------------------------------------------------------------+

  Bei  einem  Auftreten von "$" in der Patternliste sucht der Pattern-
  Matching Algorithmus nach einem ersten Vorkommen  des  auf    das  $
  folgenden  Listenelements.  Das  Symbol $ ueberdeckt genauso wie ...
  beliebig viele Argumentlistenelemente. Im Gegensatz  zur  Behandlung
  des      ...    wird  ein  erneuter  Versuch  des  Matching nur dann
  vorgenommen, falls der Vergleich des ersten auf  das    $  folgenden
  Patternlistenelements         mit    einem    Argumentlistenelements
  fehlschlaegt. Passt eines der anderen folgenden Listenelemente nicht
  auf das  Element der Argumentliste,  erfolgt  kein  erneuter Versuch
  des  Matching.

  Im  folgenden  Example  setzt  der Pattern-Matching Algorithmus den
  Vergleich mit dem auf das $ folgenden Patternelement fort.

    Example: Patternausdruck  <  1   2    $  3     4   5   ...    >
              Argumentausdruck <  1   2       3     3   4    5   5 >

  Das    auf   das   $   folgende  Patternlistenelement  3  passt  auf
  das Argumentlistenelement 3.  Bei  einem  Fehlschlag  des   Matching
  der weiteren  Patternlistenelemente  erfolgt  kein  erneuter Versuch
  des Matching (Gegensatz zu  ... ). Der Vergleich der  4  mit  der  3
  schlaegt   fehl,  also  passt  der  Patternausdruck  nicht  auf  den
  Argumentausdruck.

  Examples:
  match( <  1  $  3  B  ...  >, < 1 2 3 4 5 6 >) = <OK, (B <-- 4)>
  match( <  1  ...   3  5  ...  > , < 1 2 3 4 3 5 > ) = <OK, ()>
  match( <  1   $    3  5  ...  > , < 1 2 3 4 3 5 > ) = <FAIL, ()>

  Weitere Informationen sind unter den Stichworten Pattern-Matching,
  ... und as  zu finden

^as ^'as`
+-----------------------------------------------------------------------------+
|                            as                             April - 4 - 1989  |
+-----------------------------------------------------------------------------+

- Ueber  das  Pattern   as  <:Pattern:> <:Variable:>   wird  ein  Name
  fuer  einen  Ausdruck  eingefuehrt.  Der  Ausdruck muss auf das  mit
  Hilfe des as-Konstruktors   angegebene  Pattern  passen.  In  diesem
  Pattern  koennen  weitere Variablen  auftreten, so dass der user
  neben einer Liste auch  Unterstrukturen  dieser  Liste  durch  Namen
  referenzieren kann.

    Example : match( as   < 1, Z >   X,    < 1, 3 > ) =
                       < OK, ( X <-- < 1, 3 >, Z <-- 3 ) >


- Elemente    einer  Patternliste  der   Form  as ... <:variable:> und
  as $ <:variable:> koennen  beim Vergleich  des  Pattern   mit  einem
  Argument  genauso  behandelt werden wie die Elemente ... bzw. $. Das
  Reduktionssystem  erzeugt  fuer    die    mit    dem    ...  oder  $
  ueberdeckten  Listenelemente  eine  Liste.

  Example: match (<1, as  ...  X,  3, ... >, < 1, 2, 2, 3, 3 >) =
                                           < OK, ( X <-- < 2, 2 > ) >

  Weitere  Informationen  sind unter dem Stichwort Pattern-Matching zu
  finden.

^(<
+-----------------------------------------------------------------------------+
|                          <(                               April - 4 - 1989  |
+-----------------------------------------------------------------------------+

  Das  Pattern  List M N  passt  auf Listen der Reduktionssprache. Die
  Variable N wird  mit  der  Stelligkeit  der  Argumentliste  und  die
  Variable  M   mit  der  Liste  selbst assoziiert.  Damit erhaelt der
  Programmierer   die   Moeglichkeit,   auf   die   Stelligkeit    der
  Argumentliste Bezug zu nehmen.

  Beipiel: match( List  M N  , <  1, 2, 3 > ) =
                     < OK, ( N <-- 3 , M <-- < 1, 2, 3 > >

  weitere Informationen sind unter dem Stichwort  Pattern-Matching  zu
  finden

^Pattern-Match ^when ^guard ^do
+-----------------------------------------------------------------------------+
|                         when                              April - 4 - 1989  |
+-----------------------------------------------------------------------------+

Pattern-Matches
---------------

Ein Pattern-Match hat  die folgende Form.

 when Pattern-Ausdruck guard Guard-Ausdruck do Rumpf-Ausdruck

Guard-Ausdruecke      sind   Reduktionssprachenausdruecke,  mit  denen
der Programmierer    neben    den    ueber    die    Patternausdruecke
spezifizierten Bedingungen  weitere  Bedingungen  fuer  die  usage
eines  Pattern-Matches  angeben  kann.  Ein     Pattern-Match      ist
ausfuehrbar,    falls  das Pattern  auf  das  Argument  passt, und der
Guard-Ausdruck sich zu true auswerten laesst.

Das   Vorkommen   einer  Variablen  im  Patternausdruck  bindet  freie
Vorkommen dieser Variablen im Guard- und Rumpfausdruck.  Der  Pattern-
Matching    Algorithmus    baut    eine   Umgebung  fuer die durch das
Pattern gebundenen  Variablen    auf.  Falls  das    Pattern    passt,
werden    die  Variablen   im  Guard- und Rumpfausdruck nach der Beta-
Reduktionsregel des Lambda-Kalkuels  durch  die  Instantiierungen  der
Patternvariablen ersetzt.

Stellt    der    Pattern-Matching   Algorithmus  die  Uebereinstimmung
eines Pattern mit dem Argument fest, reduziert das System  den  Guard-
Ausdruck  in  der durch den Matching Algorithmus aufgebauten Umgebung.
Ergibt die Auswertung des Guard-Ausdrucks
- true,    wird    der    Pattern-Match  ausgefuehrt. Das Ergebnis der
  Ausf}hrung ist der  Rumpfausdruck  des  Pattern-Match,  in  dem  die
  (durch  das  Pattern)  gebundenen Variablen durch die entsprechenden
  Argumente ersetzt sind.
- false, ist der Pattern-Match nicht ausfuehrbar
- einen  Ausdruck  ungleich true oder false, so kann nicht entschieden
  werden, ob der Pattern-Match ausfuehrbar ist.

weitere  Informationen sind unter den Stichworten Pattern-Matching und
Case-function zu finden.

^Case ^Case-function ^Otherwise
+-----------------------------------------------------------------------------+
|                     case - function                       April - 4 - 1989  |
+-----------------------------------------------------------------------------+
Die Case-function
-----------------

Die  Applikation  einer  Case-function auf ein Argument wird wie folgt
reduziert: Das Reduktionssystem versucht die in einem    Case-Ausdruck
zusammengefassten   Pattern-Matches   nacheinander   auf  das Argument
anzuwenden. Der erste Pattern-Matche, dessen Patternausdruck auf   das
Argument    passt  und  dessen Guard-Ausdruck zu true reduzierbar ist,
wird  ausgefuehrt.  Ist  keines   der   Pattern-Matches   ausfuehrbar,
restauriert das Reduktionssystem  die  Applikation  de  Case-Ausdrucks
oder  gibt  den  durch  den  Programmierer  spezifizierten  "Otherwise
Ausdruck" zurueck.

Example: ap  case
              when <  1, X   guard ( X < 2 ) do 1
              when <  Y, Z   guard ( Z < 2 ) do 2
              otherwise 3
          to < 2, 3 >

Das    zweite   Pattern  passt  auf  das  Argument.  Im Guard-Ausdruck
des zweiten Pattern-Matches erhaelt Z den value 3  und  der  reduzierte
Guard- Ausdruck  (3  <  2)  ist  false.  Also  ist keines der Pattern-
Matches anwendbar, und  das  Ergebnis  der  Applikation   ist      die
3.  Ohne  die Angabe des Otherwise Ausdrucks  bleibt  die  Applikation
der Case-function auf das Argument erhalten, da  keines  der  Pattern-
Matches anwendbar  ist.

Es kann nicht entschieden werden, ob ein Pattern-Match ausf}hrbar ist,
falls
- nicht  entschieden  werden  kann,  ob der  Patternausdruck  auf  das
  Argument passt.
- das  Reduktionssystem den Guard-Ausdruck weder zu true noch zu false
  reduzieren kann.

Falls      nicht    entschieden  werden  kann,  ob  ein  Pattern-Match
ausfuehrbar ist, darf auch die  Ausfuehrung der Case-function    nicht
erfolgen.  Im Case-Ausdruck   kann   es  zwar  weitere Pattern-Matches
geben,  die ausfuehrbar sind, aber durch die  sequentiellen  usage
der    Pattern-Matches    auf  das  Argument    ist keine Entscheidung
moeglich, welches der Pattern-Matches das Reduktionssystem  ausfuehren
soll.

Das  Reduktionssystem  stellt  dem  user    mit  Hilfe eines Case-
Ausdrucks den Pattern-Match dar, dessen  usage  auf  das  Argument
nicht entschieden werden kann.

Example:
  ap   case
       when <  X, Y > guard true do 1
       when  <  B >  guard ( B < 2 ) do 2
       end
  to < X >

Der   Guard-Ausdruck  des zweiten Pattern-Match ist weder zu true noch
zu false reduzierbar. Die Applikation  der  Case-function    wird  auf
den folgenden Ausdruck transformiert.

         ap   case
              when . guard ( X < 2 ) true do 2
              otherwise
                ap   case
                     when < X, Y > guard true do 1
                     when < X > guard ( B < 2 ) do 2
                     end
                to < X >
         to < X >

Im   oberen   Ausdruck   ist   der   nicht  ausfuehrbare  Pattern-Match
fortgelassen. Statt des Pattern-Matches, dessen  Guard-Ausdruck  weder
true noch false ist, wird folgender Pattern-Match dargestellt:

    when  . guard (X < 2) do 2

X  < 2  ist  der  weder  zu  true  noch  zu  false ausgewertete Guard-
Ausdruck. 2 ist der  Rumpfausdruck    des    urspruenglichen  Pattern-
Matches.

Mit  dem  Otherwise  Ausdruck  wird die urspruengliche Applikation der
Case-function  dargestellt.  Ist    keines    der      Pattern-Matches
anwendbar,     gibt  das  Reduktionssystem  damit  die  urspruengliche
Applikation der Case-function auf das Argument zurueck.

Weitere Informationen sind unter den Stichworten  Pattern-Matches  und
Pattern-Matching zu finden.



Matrizen , Vektoren und Tvektoren koennen mit Hilfe des pattern_matching
nur auf Gleichheit getestet werden.

Examples :
     ap case when mat<1,2,3>. guard true do 'success`
             otherwise 'no match`
             end
     to [ mat<1,2,3>. ]                   ==>  'success`

     ap case when < (X) Y  guard true do < X , Y >
             end
     to [ < bvect<true,false> > ]         ==>  < 1 , < bvect<true,false> > >

     ap case when <nilmat,nilvect,niltvect> guard true do 'success`
             otherwise 'no match`
             end
     to [ <nilmat,nilvect,niltvect> ]     ==>  'success`

^"in ^"as ^"... ^". ^"$  ^" ^pattern-string
+-----------------------------------------------------------------------------+
|           "in, "as, "..., "$, "., "<:var:>                April - 4 - 1989  |
+-----------------------------------------------------------------------------+
Diese Konstruktoren koennen ausschliesslich beim  pattern-match  auf  Strings
verwendet werden. " ist nur ein  vorangestelltes Zeichen, um eine Unterschei-
dung zwichen characters und den  Konstruktoren  in / as / bzw.  ...  / .  / $
vornehmen zu koennen. Soll in  einem  pattern-string eine Variable eingegeben
werden, so muss dieses  innerhalb eines Strings mit " erfolgen.  Der   Editor
uebergibt das Zeichen "  nicht an die Reduktionseinheit.  Mit  dem "in - Kon-
strukt wird das Matchen von regulaeren  Ausdruecken auf Strings ermoeglicht.
"as     "...    ".  und  "$   haben   die  entsprechende  Bedeutung  wie  bei
der usage auf Listen (see as). Soll in dem pattern-string  das  Zeichen
" oder das Zeichen # editiert werden, so muss dieses mit "" bzw. "# erfolgen.
Die Syntax des pattern-matches auf Strings ist unter 9 angegeben.

Example :   ap case
                when '"as "in "... 'aeiouxyz`"  X"` guard  X
                otherwise fail
                end
             to [ 'aeiou` ]          ==>   'aeiou`

^fail ^ok
+-----------------------------------------------------------------------------+
|                         ok, fail                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+
ok  und  fail  kann als verkuerzende Schreibweise in Programmausdruecken
fuer die Strings 'richtig` und 'falsch` eingesetzt werden.

Example :
     ap case when <nilmat,nilvect,niltvect> guard true do ok
             otherwise fail
             end
     to [ <nilmat,nilvect,niltvect> ]     ==>  ok

^match
+-----------------------------------------------------------------------------+
|                         match                             April - 4 - 1989  |
+-----------------------------------------------------------------------------+
                       future use in pattern_match
                      -----------------------------

^mmat ^mat ^bmat ^smat ^matrix ^matrices ^(mxn)-mat ^(mx1)-mat ^(1xn)-mat
+-----------------------------------------------------------------------------+
|                      matrix                               April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 syntax:        mat                              [ n >=1, n number of columns ]
                < e11, ... ,e1n >,                  [ m >=1, m number of rows ]
                < e21, ... ,e2n >,
                  ...  ...  ...
                < em1, ... ,emn >.

 mmat   | matrices of arbitrary type
 mat    | Number
 bmat   | Bool
 smat   | String
 nilmat |

    Die Elemente einer  Matrix von Matrizen  duerfen  Matrizen  mit  beliebiger
    Zeilen- und Spaltenzahl, sowie vom Typ Number, Typ Bool oder von Typ String
    sein. Aus  diesem  Grund  sind  keine  value transforming functions auf
    ihnen defined.

^vect ^bvect ^svect ^vector ^vectors ^n-vect ^1-vect
+-----------------------------------------------------------------------------+
|                       vector                              April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 syntax:        vect< e1, ... ,en>               [ n >=1, n number of columns ]

 function:
    Mit Hilfe dieser Konstruktoren werden Zeilenvektoren dargestellt.

 vect    | Number
 bvect   | Bool
 svect   | String
 nilvect |

^tvect ^btvect ^stvect ^Tvector ^Tvectoren ^transposed vector
+-----------------------------------------------------------------------------+
|              transposed vector                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 syntax:         tvect< e1,                         [ m >=1, m number of rows ]
                        e2,
                        ...
                        em >

 function:
    Mit Hilfe dieser Konstruktoren werden transponierte Vektoren
    (Spaltenvektoren) dargestellt.

 tvect    | Number
 btvect   | Bool
 stvect   | String
 niltvect |

^c_+ ^c_- ^c_* ^c_/ ^c_plus ^c_minus ^c_mult ^c_div ^c_
+-----------------------------------------------------------------------------+
|                c - functions ???                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:             ap c_? to [ coor,mat ]

 function:
    Dieses sind die arithmetischen functions, die entlang einer Koordi-
    nate einer Matrix ausgefuehrt werden.  Ist 'coor'  gleich  Eins,  so
    werden die Zeilen und bei 'coor' gleich Zwei die Spalten verknuepft.
    Das Ergebnis ist ein Vektor,  wenn  'coor'  gleich 1  ist  bzw.  ein
    Tvektor, wenn coor gleich 2 ist.
    c_+
         Es  erfolgt  eine  Summation  }ber alle  Elemente  entlang  einer
         ausgew{hlten Koordinate der Matrix.
    c_-
         Alternierend wird eine Subtraktion und eine Addition der Elemente
         entlang einer Koordinate der Matrix durchgef}hrt.
    c_*
         Entlang   einer ausgew{hlten Koordinate einer Matrix erfolgt eine
         Multiplikation der Elemente.
    c_/
         Abwechselnd wird eine  Division und eine Multiplikation der
         Elemente entlang einer Koordinate der Matrix ausgef}hrt.

 Examples:     ap c_+  to [ 1, mat<1,2>,<3,4>. ]  =>  vect<4,6>
               ap c_+  to [ 1, mat<7>.         ]  =>  vect<7>
               ap c_-  to [ 2, mat<1,2>,<3,4>. ]  =>  tvect<~1,~1>
               ap c_*  to [ 1, mat<1>,<2>,<3>. ]  =>  vect<6>
               ap c_/  to [ 2, mat<1,2,3>.     ]  =>  tvect<1.5>


^vc_+ ^vc_- ^vc_* ^vc_/ ^vc_plus ^vc_minus ^vc_mult ^vc_div ^vc ^vc_
+-----------------------------------------------------------------------------+
|                 vc - functions ???                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:              ap vc_? to [ tvect ]
                         ap vc_? to [ vect ]

 function:
    Dieses sind die arithmetischen functions, die entlang der Elemente
    eines Vektors bzw. eines transponierten Vektors  ausgefuehrt werden
    koennen. Das Ergebnis der Reduktion ist ein  Skalar.
    vc_+
         Es  erfolgt  eine  Summation  }ber  alle Elemente des Argumentes.
    vc_-
         Alternierend wird eine Subtraktion und eine Addition der Elemente
         des Argumentes durchgef}rht.
    vc_*
         Die Elemente des Argumentes werden miteinander multipliziert.
    vc_/
         Abwechselnd  wird eine  Division und eine Multiplikation der Ele-
         mente des Argumentes ausgef}hrt.

 Examples:          ap vc_+  to [ vect<1,2,3,4> ]  =>  10
                     ap vc_+  to [ tvect<7>      ]  =>   7
                     ap vc_-  to [ vect<1,2,3,4> ]  =>  ~2
                     ap vc_*  to [ vect<1,2,3>   ]  =>   6
                     ap vc_/  to [ tvect<1,2,3>  ]  =>   1.5

^c_max ^c_min ^c_m
+-----------------------------------------------------------------------------+
|                                                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
                           c_max, c_min
                          --------------
 usage:            ap c_? to [coor,mat]

 function:
    Dieses sind die relationalen functions,  die  entlang einer Koordi-
    nate einer Matrix ausgefuehrt werden.  Ist 'coor'  gleich  Eins,  so
    werden  die Zeilen verknuepft. Ist 'coor' gleich Zwei, so werden die
    Spalten verknuepft. Das Ergebnis ist ein Vektor vom  Typ Bool,  wenn
    coor gleich Eins ist. Ein  Tvektor vom Typ Bool,  wenn  coor  gleich
    Zwei ist.   c_max und c_min  sind auf Matrizen  vom Typ Bool, Number
    und String defined. [ Bei Matrizen vom Typ Bool  entspricht  c_max
    einem logischen c_or, und das c_min einem logischen c_and. ]

 Examples:  ap c_max to [ 1, bmat<true,false>. ]   =>  bvect<true,false>
             ap c_max to [ 2, bmat<true,false>. ]   =>  btvect<true>
             ap c_min to [ 1, mat<1,2>,<3,4>.   ]   =>  vect<1,2>
             ap c_min to [ 2, mat<1,~1,0>.      ]   =>  tvect<~1>

^vc_max ^vc_min ^vc_m
+-----------------------------------------------------------------------------+
|                                                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
                              vc_max , vc_min
                             -----------------
 usage:                 ap vc_? to [ vect ]
                            ap vc_? to [ tvect ]

 function:
    Dieses sind die relationalen  functions,  die  entlang  der  Elemente
    eines Vektors bzw. der Elemente  eines  Tvektors  ausgefuehrt  werden.
    vc_max und vc_min sind  auf  Vektoren bzw. Tvektoren  vom  Typ   Bool,
    Number und String defined. [ Bei Vektoren  bzw.  Tvektoren  vom  Typ
    Bool entspricht vc_max einem  logischen vc_or, und  das  vc_min  einem
    logischen vc_and. ]

    vc_max  bestimmt das Maximum der Elemente des Argumentes  hinsichtlich
            der auf den  Elementen  des  Argumentes definedn Ordnung.

    vc_min  bestimmt das Minimum der Elemente  des Argumetes  hinsichtlich
            der auf den  Elementen  des  Argumentes definedn Ordnung.

    Das  Resultat  der  function  ist  je nach dem Typ des Argumentes eine
    Zahl, ein boolean value oder ein String.


 Examples:     ap vc_max to [ bvect<true,false>  ]   =>  true
                ap vc_max to [ btvect<true,false> ]   =>  true
                ap vc_min to [ vect<1,~1,0>       ]   =>  ~1
                ap vc_min to [ tvect<1,3>         ]   =>  1

^ceil ^floor
+-----------------------------------------------------------------------------+
|                      ceil, floor                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:                    ceil(argument)
                              floor(argument)
 function:
    Diese functions  sind  auf  Zahlen,  Matrizen von Zahlen,  Vektoren von
    Zahlen und Tvektoren von Zahlen anwendbar.
    ceil   angewandt  auf eine Zahl liefert die naechst groessere ganze Zahl.
    floor  dagegen  berechnet  die naechst kleinere Zahl.  Auf  Matrizen bzw.
    Vektoren bzw. Tvektoren werden die functions auf jedes einzelene Element
    angewandt.

 Examples:   ceil( 5.7               )      =>  6
              ceil( mat<~0.1,0,0.2>.  )      =>  mat<0,0,1>.
              ceil( vect<~0.1,0,0.2>  )      =>  vect<0,0,1>
              ceil( tvect<~0.1,0,0.2> )      =>  tvect<0,0,1>

             floor( 5.7               )      =>  5
             floor( mat<~0.1,0,0.2>.  )      =>  mat<~1,0,0>.
             floor( vect<~0.1,0,0.2>  )      =>  vect<~1,0,0>
             floor( tvect<~0.1,0,0.2> )      =>  tvect<~1,0,0>

^class ^Function ^Scalar ^List ^Matrix ^Vector ^Tvector
+-----------------------------------------------------------------------------+
|                         class                             April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:              class( arg_expr )

 function:
    Mit Hilfe dieser function kann der Banutzer die  Klassenzugehoerigkeit
    von einigen Ausdruecken der Reduktionssprache ermitteln. Die Zuordnung
    erfolgt  mittels einer Klassenkonstante. Nur fuer folgende Reduktions-
    sprachenausdruecke ist eine Klassenzugehoerigkeit defined :

 |              arg_expr                      zugeordnete Klassenkonstante |
 |-------------------------------------------------------------------------|
 |  - primitiven und userdefinedn functions.         --> Function  |
 |  - Zahlen, Wahrheitswerte.                                --> Scalar    |
 |  - Listen,   einschliesslich der leeren Liste bzw.                      |
 |    Strings,  einschliesslich des leeren Strings.          --> List      |
 |  - Matrizen, einschliesslich der leeren Matrix.           --> Matrix    |
 |  - Vektoren, einschliesslich des leeren Vektors.          --> Vector    |
 |  - transp. Vektoren, einschliesslich des leeren Tvektors. --> Tvector   |

    Laesst sich der Ausdruck nicht einer dieser Klassen zuordnen, so ist
    die function nicht anwendbar.

^cut
+-----------------------------------------------------------------------------+
|                         cut                               April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:    cut( parameter1 , parameter2 , Struktur )   ==>   Struktur

 function : Dieses ist die universelle cut-function,  die auf die mit 'Struktur'
            spezifizierte Struktur angewandt  werden kann. Wenn die universelle
            cut - function auf  die  Strukturen  List ,  String ,  Vector  oder
            Tvector angewandt wird,  dann  muss der erste Parameter  eine  Null
            sein.

            Parameter   Struktur      Struktur      weitere Erlaeuterungen
                                                    see:
 cut    :   0  x  Z  x  List     ==>  List      ---------   lcut
            0  x  Z  x  String   ==>  String    ---------   lcut
            0  x  Z  x  Vector   ==>  Vector    ---------   vcut
            0  x  Z  x  Tvector  ==>  Tvector   ---------   vcut
            Z  x  Z  x  MMatrix  ==>  MMatrix   ---------   mcut
            Z  x  Z  x  Matrix   ==>  Matrix    ---------   mcut
            Z  x  0  x  Matrix   ==>  Matrix    ---------   mcut
            0  x  Z  x  Matrix   ==>  Matrix    ---------   mcut

^lcut
+-----------------------------------------------------------------------------+
|                            lcut                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:             lcut( index , list   )
                        lcut( index , string )

    Die function lcut entfernt Elemente aus einer Liste ('list'). Das
    Vorzeichen des  Parameters  'index'  entscheidet darueber, ob ein
    'cutfirst'  (positives Vorzeichen) oder ein 'cutlast'  (negatives
    Vorzeichen)  durchgefuehrt  wird.   Ein 'cutfirst'  entfernt  die
    ersten, ein  'cutlast'  die letzten  Elemente  in der  Kopie  der
    Liste. Der Absolutwert des Parameters  gibt an, wieviele Elemente
    abgeschnitten  werden  sollen.  Allerdings   duerfen  nicht  mehr
    Elemente als vorhanden  entfernt  werden.  Werden  alle  Elemente
    abgeschnitten, so entsteht eine leere Liste ( <> ). Die usage
    der function auf auf die Struktur 'string' erfolgt analog.

 Examples:          lcut(  2 , <1,2,3>       )  =>  <3>
                     lcut( ~1 , <1,2,3>       )  =>  <1,2>
                     lcut( ~3 , <1,2,3>       )  =>  <>
                     lcut( ~4 , 'ich bin ich` )  =>  'ich bin`
                     lcut(  4 , 'ich bin ich` )  =>  'bin ich`

^mcut
+-----------------------------------------------------------------------------+
|                          mcut                             April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:         mcut( rowpar, columnpar, mat  )
                    mcut( rowpar, columnpar, mmat )

 function:
    Diese function ist auf Matrizen vom  Typ  Number, Bool  und  String
    defined. mcut entfernt Elemente  einer Matrix ('mat'). Es koennen
    sowohl Zeilen als auch Spalten abgeschnitten werden. Das Vorzeichen
    eines Parameters entscheidet darueber, ob in der betreffenden Koor-
    dinate ein  'cutfirst'  (positives Vorzeichen)  oder ein  'cutlast'
    (negatives Vorzeichen) durchgefuehrt wird. Ein 'cutfirst'  entfernt
    die ersten, ein 'cutlast' die letzten Elemente entlang einer  Koor-
    dinate. Der Absolutwert eines Parameters gibt an, wieviele Elemente
    abgeschnitten werden sollen. Allerdings duerfen nicht mehr Elemente
    als vorhanden  entfernt werden.  Werden alle Elemente einer Koordi-
    nate abgeschnitten, so entsteht ein leere Matrix  (nilmat).

 Examples:       mcut( 1,~1, mat<1,2>,<3,4>. )  =>  mat<3>.
                  mcut( 0, 1, mat<1,2>,<3,4>. )  =>  mat<2>,<4>.
                  mcut( 2, 2, mat<1,2>,<3,4>. )  =>  nilmat

^vcut
+-----------------------------------------------------------------------------+
|                            vcut                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:           vcut( rowpar   , vect  )
                      vcut( columnpar, tvect )

 function:
    Diese function ist auf Vektoren und Tvektoren vom  Typ  Number , Bool
    und String defined. Sie  schneidet  entsprechend  der  Eingabe  des
    1.Argumentes Elemente des Vektors bzw. des Tvektor ab. Ein 'cutfirst'
    (arg1 mit positiven Vorzeichen) schneidet  die ersten mit arg1 spezi-
    fizierten Elemente ab.  Ein 'cutlast' (arg1 mit negativen Vorzeichen)
    schneidet die letzten mit arg1 spezifizierten Elemente ab.

 Examples:      vcut( ~1, vect<1,2,3,4>  )  =>  vect<1,2,3>.
                 vcut(  1, tvect<1,2,3,4> )  =>  tvect<2,3,4>
                 vcut(  2, vect<5,6>      )  =>  nilvect
                 vcut(  2, tvect<5,6>     )  =>  niltvect

^ldim
+-----------------------------------------------------------------------------+
|                            ldim                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage :               ldim( struct )

 function :
    Die function ldim ist  auf die Strukturen Liste und String  defined.
    Sie liefert als Ergebnis die Laenge der mit  'struct'  spezifizierten
    Liste bzw. des Strings.

 Example :  ldim( 'Dies ist ein String.`   )  ==>  20
             ldim( 'Dies 'ist` ein String.` )  ==>  18
             ldim( < 1 , 2 , < 3 , 3 > , 4  )  ==>   4

^dim
+-----------------------------------------------------------------------------+
|                              dim                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:              dim( coor, Struktur )  == >  Number

 function : Dieses ist die universelle function zur Berechnung der Dimension
            der mit 'Struktur' spezifizierten Struktur. Wenn die universelle
            dim - function auf  die  Strukturen  List , String , Vector oder
            Tvector angewandt wird,  dann muss der erste Parameter eine Null
            sein.

                coor      Struktur      Number        weitere Erlaeuterungen
                                                      see:
 dim        :   0      x  List     ==>  Number     ------    ldim
                0      x  String   ==>  Number     ------    ldim
                0      x  Vector   ==>  Number     ------    vdim
                0      x  Tvector  ==>  Number     ------    vdim
                {1,2}  x  MMatrix  ==>  Number     ------    mdim
                {1,2}  x  Matrix   ==>  Number     ------    mdim

^mdim
+-----------------------------------------------------------------------------+
|                             mdim                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:              mdim( coor, struct )

 function :
    Die function dim ist nur auf  Matrizen defined. Sie liefert
    die Zahl der Zeilen (bzw. Spalten), falls der value von 'coor'
    gleich Eins (bzw. Zwei) ist.

 Example :             dim( 1 , mat<1,2,3>     => 2
                                    <4,5,6>. )

^vdim
+-----------------------------------------------------------------------------+
|                           vdim                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:                vdim( struct )

 function:
    Die function vdim ist nur auf Vektoren  bzw. Tvektoren defined.
    Sie liefert die Zahl der Spalten bei Vektoren
    bzw.        die Zahl der Zeilen  bei Tvektoren.

 Example :          vdim( vect<1,2,3>            )  => 3
                     vdim( tvect<true,false,true> )  => 3

^empty
+-----------------------------------------------------------------------------+
|                         empty                             April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:                   empty(struct)

 function:
    Die function empty ist auf Listen, Strings , Matrizen, Vektoren und
    Tvektoren anwendbar. Das Ergebnis der Reduktion ist true, wenn  die
    entsprechende Struktur 'struct' leer ist, ansonsten  false.

 Examples:              empty(  <>    )  => true
                         empty(  '`    )  => true
                         empty(nilmat  )  => true
                         empty(nilvect )  => true
                         empty(niltvect)  => true

             Ansonsten reduziert sich die Applikation zu false.

^f_eq ^f_ne
+-----------------------------------------------------------------------------+
|                      field - equal                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:   (mat   f_eq mat  )       (mat   f_ne mat  )
              (vect  f_eq vect )       (vect  f_ne vect )
              (tvect f_eq tvect)       (tvect f_ne tvect)
 function:
    Diese beiden functions sind auf Matrizen, Vektoren  und Tvektoren
    des Typs Bool, Number und  String defined. Die zu vergleichenden
    Matrizen muessen die gleiche Zeilen- und Spaltenzahl. Bei Vektoren
    bzw. Tvektoren muss die Dimension der beiden Argumente gleich sein.
    Das Ergebnis des Vergleichs ist im Gegensatz zu den functions  eq
    und ne ...  eine Matrix oder ein Vektor oder ein Tvektor mit Wahr-
    heitswerten, deren Elemente aus Einzelvergleichen der Argumentele-
    mente bestimmt wird. Der Vergleich von Matrizen mit mehr als  zwei
    Dimensionen ist nicht erlaubt.

 Examples:
       (smat<'1,2,3`>.  f_eq   smat<'4,5,6`>.)  => bmat<false>.
       (bmat<true>.     f_eq   bmat<false>.  )  => bmat<false>.
       (btvect<true>    f_eq   btvect<false> )  => btvect<false>.
       (mat<1,9,3>.     f_ne   mat<1,2,3>.   )  => bmat<false,true,false>.
       (vect<1,2,3>     f_ne   vect<1,2,3>   )  => bvect<false,false,false>.

^ge ^>= ^gt ^> ^le ^<= ^lt ^< ^lesser ^greater
+-----------------------------------------------------------------------------+
|                        relational                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 function:
     Diese functions vergleichen Argumente  vom Typ Bool,  Number  und
     String. Die usage dieser function auf  einen  Skalar  und eine
     Matrix gleichen Typs ist erlaubt. Es  entsteht  dann  eine  Matrix
     vom Typ Bool, deren Elemente aus der Verknuepfung des Skalars  mit
     den einzelnen  Matrizenelementen  hervorgehen.  Bei dem  Vergleich
     zweier Matrizen muessen beide die gleiche  Zeilen- und Spaltenzahl
     haben. Die usage  der  functions  auf  Vektoren und Tvektoren
     erfolgt analog.

 Examples:
      ( 2                gt  1              )  =>  true
      ( 2                le  mat<1,2,3>.    )  =>  bmat<false,true,true>
      ( tvect<1,2,3>     le  tvect<1,~2,3>  )  =>  btvect<true,false,true>
      ( false            lt  true           )  =>  true
      ( 'b`              gt  'a`            )  =>  true
      ( 'b`              lt  'a`            )  =>  false

^eq ^ne ^= ^!= ^equal ^not equal
+-----------------------------------------------------------------------------+
|                          relational                       April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 function:
     Diese functions vergleichen  Argumente  vom  Typ Bool,  Number  und
     String. Die usage dieser  function  auf  einen  Skalar  und  eine
     Matrix gleichen Typs ist erlaubt. Bei dem  Vergleich  zweier Matrizen
     muessen beide die  selbe  Zeilen- und Spaltenzahl  haben.  Die Anwen-
     dung der functions  auf  Vektoren und Tvektoren  erfolgt analog. Nur
     der Vergleich (eq) von leeren Listen ergibt true.  Zwei  Listen   mit
     einer Dimension groesser 0 sind nicht vergleichbar. Typ- und Klassen-
     konstanten sowie functionssymbole  primitiver  functions  sind  ver-
     gleichbar. Das Ergebnis des Vergleiches ist immer ein Wahrheitswert.

 Examples:
      ( <>               eq  <>             )  =>  true
      ( <>               eq  <1,2,3>        )  =>  false
      ( Primfunc         eq  select         )  =>  false
      ( Number           eq  Number         )  =>  true
      ( true             eq  false          )  =>  false
      ( vect<1,2,3>      eq  vect<1,2,3>    )  =>  true
      ( mat<1,2>,<3,4>.  eq  mat<1,3>,<2,4>.)  =>  false
      ( 'a'a`bc`         eq  'a'a`bc`       )  =>  true
      ( 'a'a`bc`         eq  'a'aa`bc`      )  =>  false
      ( 'abcdz`          eq  'abcde`        )  =>  false

^max ^min ^maximum ^minimum
+-----------------------------------------------------------------------------+
|                        max, mim                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:        max( argument1 , argument2 )
                   min( argument1 , argument2 )
 function:
    max und min  sind auf Argumente vom  Typ Bool und Typ Number anwendbar.
    Die functions ist nicht auf Argmente mit Strings  anwendbar.  Matrizen
    und Skalare des gleichen Typs koennen  verknuepft  werden.  Es entsteht
    eine neue Matrix, deren Elemente aus der Verknuepfung des  Skalares mit
    dem jeweiligen Matrizenelement hervorgeht. Soll  das  Maximum  bzw. das
    Minimum zweier Matrizen ermittelt  werden, so muessen beide die gleiche
    Zeilen- und Spaltenzahl haben.  max  bzw. min  wird  dann  elementweise
    angewandt.  Die usage der  functions auf  Vektoren  und  Tvektoren
    erfolgt analog.

 Examples:
         max(true            , false       )  => true
         max(smat<'heino`>.  , 'otto`      )  => smat<'otto`>.
         max(stvect<'heino`> , 'otto`      )  => stvect<'otto`>
         max('aa`            , 'ab`        )  => 'ab`
         min('b`             , 'a`         )  => 'a`
         min(mat<1,2,3>.     , mat<3,2,1>. )  => mat<1,2,1>.
         min(vect<1,2,3>     , vect<3,2,1> )  => vect<1,2,1>

^false ^true ^boolean ^boolean value ^boolean values
+-----------------------------------------------------------------------------+
|                         boolean                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 function:
              false   ist der Repraesentant fuer den Wahrheitswert falsch.
              true    ist der Repraesentant fuer den Wahrheitswert wahr.

^ip ^inner ^innerproduct
+-----------------------------------------------------------------------------+
|                            ip                             April - 4 - 1989  |
+-----------------------------------------------------------------------------+
    function:                     Definitions- und Wertebereich:
    ==========================    ============================================
    Produkt von zwei Matrizen     ip :  (mxn)-mat x (mxn)-mat --> (mxn)- mat

    Produkt von Vektoren          ip :  n-vect  x m-tvect  -->  Number
                                        1-vect  x 1-vect   -->  Number
                                        1-tvect x 1-tvect  -->  Number
                                        m-tvect x n-vect   -->  (mxn)-mat
    Produkt zwischen Vektoren
    und Matrizen                  ip : (m-tvect   x (1xn)-mat) --> (mxn)-mat
                                       ((mx1)-mat x n-vect   ) --> (mxn)-mat
                                       (n-vect    x (mxn)-mat) --> n-vect
                                       ((mxn)-mat x m-tvect  ) --> m-tvect
                                       (n-vect    x (mx1)-mat) --> Number
                                       ((1xn)-mat x m-tvect  ) --> Number
    [ m >=1, m Anzahl der Zeilen  ]
    [ n >=1, n Anzahl der Spalten ]

  Examples:
    (mat<1,2>,<3,4>. ip mat<1,2>,<3,4> ) => mat<7,10>,<15,22>.
    (mat<1>,<3>.     ip mat<1,2>.      ) => mat<1,2>,<3,6>.
    (mat<1,2>.       ip mat<1>,<3>.    ) => mat<7>.

    (vect<1>         ip vect<2>.       ) => 2
    (vect<2>         ip vect<32>.      ) => 64
    (vect<1,2,3>     ip tvect<3,3,3>   ) => 18
    (tvect<1,2,3>    ip vect<2,2,2>    ) => mat<2,2,2>,<4,4,4>,<6,6,6>.

    (tvect<1,2,3>    ip mat<1,2,3>.    ) => mat<1,2,3>,<2,4,6>,<3,6,9>.
    (mat<4>,<5>,<6>. ip vect<4,5,6>    ) => mat<16,20,24>,<20,25,30>,<24,30,36>.
    (vect<5,5>       ip mat<1,2>,        => vect<25,35>
                           <4,5>.      )
    (mat<1,2>,       ip tvect<5,         => tvect<15,45>
        <4,5>.                5>       )
    (vect<7,8,9>     ip mat<1>,<2>,<3>.) => 50
    (mat<1,2>.       ip tvect<5,6>     ) => 17

 ^+ ^- ^* ^/ ^mod ^plus ^minus ^mult ^div
+-----------------------------------------------------------------------------+
|                        arithmetic                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 function:
    Diese functions sind nur  auf Zahlen, Matrizen von Zahlen, Vektoren von
    Zahlen und  Tvektoren von Zahlen anwendbar. Zahlen koennen  mit Matrizen
    von Zahlen  verknuepft werden.  Es entsteht ein neue  Matrix, deren Ele-
    mente aus  der Verknuepfung der Zahl mit den einzelnen Matrizenelementen
    hervorgeht.  Werden  zwei  Matrizen  verknuepft,  so  muessen  beide die
    gleiche  Zeilen-  und  Spaltenzahl  haben.  Ist  der Parameter varformat
    gesetzt, werden alle Operationen, bis auf die Division und  die  Modulo-
    funktion, exakt berechnet. Im fixformat erfolgt die Berechnung im Rahmen
    der Integer- bzw. Realarithmetik  (see auch Number). Die usage der
    functions auf Vektoren und Tvektoren erfolgt analog.

 Examples:
    ( 1.5  /  6 )    =>  0.25
    ( 4 - mat
          < 1, 2 >,
          < 3, 4 >.  )    =>  mat
                              < 3, 2 >,
                              < 1, 0 >.
    ( mat
      < 1, 2 >,
      < 3, 4 >.  *  mat
                    < 1, 2 >,
                    < 3, 4 >. )    =>  mat
                                       < 1, 4 >,
                                       < 9, 16 >.
    ( mat<1,2,~1>. + 1 )    =>  mat
                                < 2, 3, 0>.
    ( vect<1,2,~1> + 1 )    =>  vect< 2, 3, 0>
    ( tvect<1,2,~1> + 1 )    =>  tvect< 2,
                                        3,
                                        0>

    ( 6.78 mod 3.14 )    =>  0.5
    ( 6.28 mod 3.14 )    =>  0
    ( 6 mod 3 )    =>  0
    ( vect< 3.3, 6, 1.1> mod 3 )    =>  vect<0.3,0,1.1>
    ( tvect< 6,
             3 > mod tvect< 1,
                            0 > )    =>  ( tvect< 6,
                                                  3 > mod tvect< 1,
                                                                 0 > )
    ( mat
      < 3.3, 6 >,
      < ~5, 3 >.  mod mat
                      < 2, 5.9>,
                      < ~3, 1.1>. ) =>  mat
                                      < 1.3, 0.1 >,
                                      < ~2.0, 0.8 >.

^nil ^<> ^nilmat ^nilvect ^niltvect ^'`
+-----------------------------------------------------------------------------+
|                         nil                               April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 function:
    <>       repraesentiert die leere  Liste.
    '`       repraesentiert den leeren String.
    nilmat   repraesentiert die leere  Matrix.
    nilvect  repraesentiert den leeren Vektor.
    niltvect repraesentiert den leeren transponierten Vektor.

^not
+-----------------------------------------------------------------------------+
|                          not                              April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:               not(argument)

 function:
    not ist auf Argumente vom Typ Bool anwendbar. Auf Matrizen, Vektoren,
    Tvektoren  vom Typ Bool  erfolgt die usage der function element-
    weise.

 Examples:         not(true               )  =>  false
                    not(bmat<true>,           =>  bmat<false>,
                            <false>.       )          <true>.
                    not(bvect<true>        )  =>  bvect<false>
                    not(btvect<true,false> )  =>  btvect<false,true>

^replace
+-----------------------------------------------------------------------------+
|                            replace                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:
      replace( parameter1 , parameter2 , Substitute, Struktur )   ==>   Struktur

 function : Dieses ist die universelle  replace-function, die  auf  die mit
            'Struktur' spezifizierte Struktur angewandt  werden kann.  Wenn
            die universelle universelle replace-function auf die Strukturen
            List , String , Vector oder Tvector angewandt wird,  dann  muss
            der erste Parameter eine Null sein.

            Parameter   Substitute  Struktur      Struktur   weitere Erlaeuter-
                                                             ungen see:
 replace  : 0  x  N  x  Object  x  List     ==>  List    ---------  lreplace
            0  x  N  x  String  x  String   ==>  String  ---------  lreplace
            0  x  N  x  Scalar  x  Vector   ==>  Vector  ---------  vreplace
            0  x  N  x  Scalar  x  Tvector  ==>  Tvector ---------  vreplace
            0  x  N  x  String  x  Tvector  ==>  Tvector ---------  vreplace
            N  x  N  x  Matrix  x  MMatrix  ==>  MMatrix ---------  mreplace
            N  x  N  x  Scalar  x  Matrix   ==>  Matrix  ---------  mreplace
            N  x  N  x  String  x  Matrix   ==>  Matrix  ---------  mreplace
            0  x  N  x  Tvector x  Matrix   ==>  Matrix  ---------  mreplace_c
            0  x  N  x  Vector  x  Matrix   ==>  Matrix  ---------  mreplace_r

^lreplace
+-----------------------------------------------------------------------------+
|                           lreplace                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:     lreplace( n , substitute , list   )   ==>  list
                lreplace( n , substitute , string )   ==>  string

 function:
    lreplace ersetzt Unterstrukturen eines Strings 'string' (einer Liste
    'list'). Der Parameter 'n' bestimmt, welches Stringelement  (Listen-
    element) durch das mit 'substitute' spezifizierte  Argument  ersetzt
    werden soll. Bei usage der function auf Listen kann 'substitute'
    ein beliebiger Ausdruck der Reduktionssprache  sein.  Bei  usage
    der function auf Strings muss 'substitute'  ein  String  sein.  Wenn
    'substitute' ein leerer String ist, so wird der String  'string'  um
    das mit 'n' festgelegte Stringelement verkuerzt.

 Example:
            lreplace(  2 ,  4         , <1,2,3>  )  =>  <1, 4 ,3>
            lreplace(  2 , <4>        , <1,2,3>  )  =>  <1,<4>,3>

            lreplace( 14 , 'U`        , 'Ich gehe zur Tniversitaet`  )
                                   ==>  'Ich gehe zur Universitaet`

            lreplace( 14 , '`         , 'Ich gehe zur Universitaet`  )
                                   ==>  'Ich gehe zur niversitaet`

            lreplace( 14 , ''Schule`` , 'Ich gehe zur Universitaet`  )
                                   ==>  'Ich gehe zur 'Schule`niversitaet`

^mreplace
+-----------------------------------------------------------------------------+
|                           mreplace                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:     mreplace( rowpar, columnpar, mat    , mmat )
                mreplace( rowpar, columnpar, Scalar , mat  )

 function:
    mreplace ersetzt ein Element einer in Matrix ('mat'), ('mmat'). Wenn
    ein Scalar oder ein String in einer Matrix ersetzt werden soll, dann
    muss der Typ des dritten Parameters  mit  der  Matrix  (des  vierten
    Parameters) uebereinstimmen. Bei Matrizen mit mehr  als  zwei Dimen-
    sionen kann der Typ  des  Substitutes  (mat)  und  der des Matrizen-
    lementes  der Matrix auf Matrizen (mmat) verschieden sein.

 Examples:
    mreplace( 1, 2,true, bmat<false,false>. ) => bmat<false,true>.

    mreplace( 1, 2, 9,    mat<1,2,3>,
                             <4,5,6>.       ) => mat<1,9,3>,
                                                    <4,5,6>.
^mreplace_c
+-----------------------------------------------------------------------------+
|                        mreplace_c                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:  mreplace_c( columnpar, tvect, mat )

 function :  mreplace_c ersetzt eine Spalte in einer  Matrix.  Das  zweite
             Argument muss ein transponierter Vektor sein. Mit 'columnpar'
             wird die zu ersetzende Spalte der  Matrix  bestimmt.  Der Typ
             des Tvektors und der Matrix muss gleich sein.
 Example :
             mreplace( 2, tvect<5, , mat<1,2>,      => mat<1,5>,
                                6>      <3,4>. )          <3,6>.

^mreplace_r
+-----------------------------------------------------------------------------+
|                      mreplace_r                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:     mreplace_r( rowpar, vect, mat )

 function :  mreplace_r ersetzt eine Zeile in einer Matrix. Das zweite
             Argument  muss  ein  Vektor  sein.  Mit 'rowpar' wird die
             zu ersetzende Zeile  der  Matrix  bestimmt.  Der Typ  des
             Vektors und der Matrix muss gleich sein.
 Example :
             mreplace_r( 1, vect<5,6>  , mat<1,2>,     => mat<5,6>,
                                            <3,4>. )         <3,4>.

^vreplace
+-----------------------------------------------------------------------------+
|                           vreplace                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:     vreplace( position , substitute , vect  )
                vreplace( position , substitute , tvect )

 function :
    Diese function ersetzt ein Element ('substitute')  in  einem Vektor  bzw.
   Tvektor. Das erste Argument gibt die Position des zu ersetzenden Elementes
   in dem Vektor bzw. Tvektor an. Das Substitut muss vom Typ Number bzw.  vom
   Typ Bool bzw. vom Typ String  sein. Die Elemente des Vektors bwz. Tvektors
   muessen mit dem Typ des Substitutes uebereinstimmen.

 Examples:
     vreplace( 1, true  , bvect<false,true>   )   -->  bvect<true , true>
     vreplace( 2,   3   , vect< 1 , 2 , 3 >   )   -->  vect<1 , 3 , 3>
     vreplace( 3,   9   , tvect< 1 , 2 , 3 >  )   -->  tvect<1 , 3 , 9>
     vreplace( 1,'heute`, stvect<'no` ,'yes`> )   -->  stvect<'heute` , 'yes`>

^rotate
+-----------------------------------------------------------------------------+
|                           rotate                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage: rotate( parameter1 , parameter2 , Struktur )   ==>   Struktur

 function : Dieses ist die  universelle  rotate-function, die  auf  die  mit
            'Struktur' spezifizierte Struktur angewandt  werden  kann.  Wenn
            die universelle rotate-function auf die Strukturen List, String,
            Vector oder  Tvector angewandt  wird,  dann muss der erste Para-
            meter eine Null sein.

            Parameter   Struktur      Struktur       weitere Erlaeuterungen
                                                     see:
 rotate   : 0  x  Z  x  List     ==>  List     --------  lrotate
            0  x  Z  x  String   ==>  String   --------  lrotate
            0  x  Z  x  Vector   ==>  Vector   --------  vrotate
            0  x  Z  x  Tvector  ==>  Tvector  --------  vrotate
            Z  x  Z  x  MMatrix  ==>  MMatrix  --------  mrotate
            Z  x  Z  x  Matrix   ==>  Matrix   --------  mrotate

^lrotate
+-----------------------------------------------------------------------------+
|                            lrotate                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:           lrotate( index , list   )    ==> list
                      lrotate( index , string )    ==> string
 function:
    Die function lrotate erzeugt eine zyklisch rotierte  Kopie  von 'list',
    bzw. 'string' . Der Parameter 'index' bestimmt, das  wievielte  Element
    nach dem ersten das neue erste Element werden soll. Bei einem negativen
    Parameter wird die Kopie entgegengesetzt rotiert.

 Examples:        lrotate( 1 , <1,2,3>     )  =>  <2,3,1>
                   lrotate( 7 , <1,2,3>     )  =>  <2,3,1>
                   lrotate(~1 , <1,2,3>     )  =>  <3,1,2>
                   lrotate( 1 , 'abcde`     )  =>  'bcdea`
                   lrotate(~1 , 'abcde`     )  =>  'eabcd`
                   lrotate( 1 , ''du`'ich`` )  =>  ''ich`'du``

^mrotate
+-----------------------------------------------------------------------------+
|                            mrotate                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:     mrotate( rowpar, columnpar, mat  )
                mrotate( rowpar, columnpar, mmat )

 function :
    Diese function ist auf Matrizen und Matrizen von Matrizen  vom  Typ
    Number, Bool, String defined. Sie erzeugt eine zyklisch  rotierte
    Matrix von 'mat' bzw. 'mmat'. Dabei kann gleichzeitig eine  Zeilen-
    und eine Spaltenrotation durchgefuehrt werden. Der Parameter  einer
    Koordinate bestimmt, das wievielte  Element  nach dem  ersten,  das
    neue erste Element dieser Koordinate werden soll. Bei  einem  nega-
    tiven  Parameter  wird die Kopie entgegengesetzt rotiert.

 Examples:
    mrotate( 1, 1, mat<1.1,1.2,1.3>,     =>   mat<2.2,2.3,2.1>,
                      <2.1,2.2,2.3>. )           <1.2,1.3,1.1>.

    mrotate( 0,~1, mat<1.1,1.2,1.3>,     =>   mat<1.3,1.1,1.2>,
                      <2.1,2.2,2.3>. )           <2.3,2.1,2.2>.

^vrotate
+-----------------------------------------------------------------------------+
|                            vrotate                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:         vrotate( rowpar,    vect )
                    vrotate( columnpar, tvect )
 function :
    Diese function ist auf Vektoren  bzw. Tvektoren  vom Typ  Number,
    Bool und String defined. Sie  erzeugt  eine  zyklisch  rotierte
    Kopie eines Vektors bzw. eines Tvektors. Das 1.Argument bestimmt,
    das  wievielte Element nach  dem  ersten, das neue erste  Element
    werden soll. Wenn das 1.Argument negativ ist, wird der Vektor bzw.
    Tvektor entgegengesetzt rotiert.

 Examples:
          vrotate(1,vect<1,2,3>)            =>  vect<2,3,1>
          vrotate(1,vect<false,true,true>)  =>  bvect<true,false,true>
          vrotate(~1,tvect<1,2,3>)          =>  tvect<3,2,1>
          vrotate(1,vect<false,true>)       =>  btvect<true,false>

^select
+-----------------------------------------------------------------------------+
|                             select                        April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage: select( parameter1 , parameter2 , Struktur ) ==>  Struktur / Objekt

 function : Dieses ist  die  universelle  select-function, die  auf  die  mit
            'Struktur' spezifizierte Struktur  angewandt  werden  kann.  Wenn
            die universelle select-function  auf die Strukturen List, String,
            Vector oder Tvector angewandt  wird,  dann  muss  der erste Para-
            meter eine Null sein.

            Parameter   Struktur      Struktur /Objekt   weitere Erlaeuterungen
                                                         see:
 select   : 0  x  N  x  List     ==>  Object             ----  lselect
            0  x  N  x  String   ==>  String             ----  lselect
            0  x  N  x  Vector   ==>  Scalar / String    ----  vselect
            0  x  N  x  Tvector  ==>  Scalar / String    ----  vselect
            N  x  N  x  MMatrix  ==>  Matrix / MMatrix   ----  mselect
            N  x  N  x  Matrix   ==>  Scalar / String    ----  mselect
            N  x  0  x  Matrix   ==>  Vector             ----  mselect
            0  x  N  x  Matrix   ==>  Tvector            ----  mselect

^lselect
+-----------------------------------------------------------------------------+
|                             lselect                       April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:            lselect( n , List   )  ==>  Listelement
                       lselect( n , String )  ==>  String

 function:
    Da extern nicht zwischen einzelenen Zeichen  eines Strings und Variablen
    der Reduktionssprache unterschieden  werden kann, ist die usage  der
    function auf Strings 'String' nur dann erfolgreich, wenn das  zu  selek-
    tierende Stringelement selbst aus einem String besteht. Es entsteht dann
    ein String, der aus dieser Unterstruktur dieses Teilstrings besteht. Bei
    usage der function auf Listen ist das Resultat der Reduktion das  zu
    selektiernde Listenelement selbst. Ist der Parameter 'n' von  Null  ver-
    schieden und kleiner der Dimension des Strings (der Liste), so kann  mit
    diesem das Selektionselement bestimmt werden.

 Examples:
        lselect(  3 , < 1 , 2 , 3 >           ) ==>  3
        lselect( 10 , 'Dies ist 'ein` String` ) ==>  'ein`
        lselect(  2 , 'abcdef`                ) ==>  lselect(  2 , 'abcdef` )
^mselect
+-----------------------------------------------------------------------------+
|                             mselect                       April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:        mselect( rowpar, columnpar, mat  )
                   mselect( rowpar, columnpar, mmat )

 function:
    Mit Hilfe der function mselect koennen einzelen Elemente, Zeilen
    oder Spalten der Matrix  'mat' selektiert  werden. Ist der  Wert
    von 'columnpar' (bzw. 'rowpar') gleich  Null, so liefert mselect
    die durch den zweiten Parameter bestimmte  Zeile  (bzw. Spalte).
    Ist 'rowpar` und 'columnpar` ungleich Null,  so wird ein Element
    (spezifiziert durch rowpar,columnpar) der Matrix selektiert. Bei
    Matrizen  auf  Matrizen  ist  nur  die  Selektion  einer  Matrix
    moeglich.

 Examples:       mselect( 2, 1, mat<1,2>,<3,4>.) =>  3
                  mselect( 2, 0, mat<1,2>,<3,4>.) =>  vect<3,4>
                  mselect( 0, 1, mat<1,2>,<3,4>.) =>  tvect<1,3>

^vselect
+-----------------------------------------------------------------------------+
|                             vselect                       April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:        vselect( rowpar,    vect )
               vselect( columnpar, tvect)

 function :
    Mit Hilfe dieser function koennen Elemente aus einem Vektor bzw.
    Tvektor selektiert  werden.  Das  erste  Argument  bestimmt  das
    Selektionselement in dem Vektor bzw. Tvektor.

 Examples:
    vselect( 4, vect<1,2,3,4>                               ) =>  4
    vselect( 1, vect<true,false>                            ) =>  true
    vselect( 2, tvect<1,2,3>                                ) =>  2
    vselect( 2, stvect<'heute heute nur nicht','morgen...'> ) =>  'morgen...`

^substruct ^substr
+-----------------------------------------------------------------------------+
|                         substruct                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
usage :  substruct( struct_par1 x struct_par2 x List  )    ==> List
         substruct( struct_par1 x struct_par2 x String )   ==> String

Die Selektion eines oder mehrerer Stringelemente (Listenelemente) wird
mit Hilfe dieser function erm|glicht. Die resultierende  Struktur  ist
immer ein String (eine Liste).
position     legt die Anfangsposition der zu  selektierenden  Elemente
             der  Argumentstruktur  fest.  position muss > 0  und
             <= Dimension(arg_struct) sein.
length       spezifiziert die Anzahl der (des) zu selektierenden  Ele-
             mente(s)  beginnend  ab  der mit 'position'  festgelegten
             Anfangsposition  der  Argumentstruktur .  'length'   muss
             > 0, <= Dimension(arg_struct) und > (Dimension(arg_struct)
             - position + 1)  sein.  Gleichzeitig   entspricht  dieser
             Parameter der Dimension der resultierenden Struktur.

Examples :
   substruct(  1 ,  2 , < 1 , 2 , < 3 ,3 > , 6 > )   ==> < 1 , 2 >
   substruct(  3 ,  2 , < 1 , 2 , < 3 ,3 > , 6 > )   ==> < < 3 , 3 > , 6 >
   substruct(  2 ,  1 , < 1 , <> >               )   ==> < <> >
   substruct(  1 ,  4 , 'abc'def`haus`           )   ==> 'abc'def``
   substruct(  3 ,  1 , 'abc'def`haus`           )   ==> ''def``
   substruct(  2 ,  2 , 'abc'def`haus`           )   ==> 'bc`
   substruct(  4 ,  1 , 'abc'``                  )   ==> ''``
   substruct(  1 , 11 , 'Wintersport ist prima in Meribell.`) ==> 'Wintersport`

^repstr
+-----------------------------------------------------------------------------+
|                          repstr                           April - 4 - 1989  |
+-----------------------------------------------------------------------------+
usage :  repstr( position x length x substitute  x  List   )  ==> List
         repstr( position x length x substitute  x  String )  ==> List

Mit  Hilfe  dieser  function (replace structure) k|nnen vier Arten von
Strukturver{nderungen  mittels  unterschiedlicher  Spezifizierung  der
Strukturierungsparameter auf Listen oder Strings realisiert werden.
postion      legt die  Anfangsposition fest,  ab  der die Strukturver-
             {nderung erfolgen soll.
length       legt die Anzahl der zu ersetzenden Elemente  bzw. die Art
             der Strukturver{nderung fest.
substitute   spezifiziert  das  Substitut und  muss bei  usage der
             function  auf  Strings als String ('char_string`) und bei
             usage   der   function   auf   Listen    als    Liste
             (<expression>)  spezifiziert  werden.  Sollen die mit den
             Strukturierungsparametern    bestimmten    Stringelemente
             (Listenelemente)  der Argumentstruktur durch einen String
             (eine Liste) ersetzt werden, so muss das Substitut bei An-
             wendung auf Strings mit  ''char_string`` und  auf  Listen
             mit < < expression > > spezifiziert werden.

Folgende Arten der Strukturver{nderung werden erm|glicht :

REPLACE Die Ersetzung eines einzigen Stringelementes (Listenelementes)
   oder  die  Ersetzung  mehrer  hintereinander  angeordneter Elemente
   eines Strings (einer Liste) durch ein Substitut.
   position     legt die Anfangsposition fest, ab  der  die  Ersetzung
                erfolgen    soll    fest.   'position' muss  > 0   und
                <= (Dimension(arg_struct) - position ) sein.
   length       spezifiziert die Anzahl der zu ersetzenden  Stringele-
                mente (Listenelemente), ab  der  mit  'position' fest-
                gelegten  Anfangsposition  innerhalb    der  Argument-
                struktur.  'length'        muss   in    diesem    Fall
                <= (Dimension(arg_struct) - position + 1) sein.

DROP Durch  die  Ersetzung  eines leeren Strings  (einer leeren Liste)
   in  der  Argumentstruktur  kann  eine  Verk}rzung des (der) mit den
   Parametern  'position'  und    'length'   festgelegten  Element(en)
   eines Strings (einer Liste) erfolgen. Die Parameter 'position'  und
   'length' m}ssen in den Grenzen wie unter REPACE beschrieben liegen.

APPEND Ein Anh{ngen der Elemente des Substitutes an die Argumentstruk-
   tur wird durch die Parameterspezifizierung
   position    = (Dimension(arg_struct) + 1) und
   length      = 0 erreicht.

INSERT Ein Einf}gen des Substitutes in eine mit 'position'   bestimmte
   Position  in der Argumentstruktur erfolgt mittels der Parameterspe-
   zifizierung :
   'position'   > 0  und 'position'  <= Dimension(arg_struct)
   'length'     = 0 .


Examples : repstr( 3 , 2 , 'xy`      , 'abcd`     )   ==>  'abxy`
            repstr( 3 , 1 , ''xy``    , 'abcd`     )   ==>  'ab'xy`d`
            repstr( 3 , 2 , ''xy`z`   , 'abcd`     )   ==>  'ab'xy`z`
            repstr( 1 , 4 , '`        , 'abcd`     )   ==>  '`
            repstr( 3 , 4 , '`        , 'abDROPcd` )   ==>  'abcd`
            repstr( 1 , 4 , 'REPLACE` , 'abcdefg`  )   ==>  'REPLACEefg`
            repstr( 5 , 0 , 'APPEND`  , 'abcd`     )   ==>  'abcdAPPEND`
            repstr( 2 , 0 , 'INSERT`  , 'abcd`     )   ==>  'aINSERTbcd`
            repstr( 1 , 2 , < 8 , 9 > , <1,2,3>    )   ==>  <8,9,3>
            repstr( 1 , 3 , < <> >    , <1,2,3>    )   ==>  < <> >
            repstr( 1 , 3 ,   <>      , <1,2,3>    )   ==>    <>
            repstr( 1 , 2 ,   <>      , <1,2,3>    )   ==>  <1>
            repstr( 1 , 0 , <1,2>     , <>         )   ==>  <1,2>
            repstr( 4 , 0 , < true >  , <1,2,3>    )   ==>  <1,2,3,true>
            repstr( 2 , 0 , < true >  , <1,2,3>    )   ==>  <1,true,2,3>
            repstr( 2 , 0 , <<true>>  , <1,2,3>    )   ==>  <1,<true>,2,3>

^sub
+-----------------------------------------------------------------------------+
|                               sub                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:      sub name1,name2, ... ,namen functionbody      [n >= 1]

 function:
     Mit Hilfe des Konstruktors 'sub' kann der user nichtrekursive
     functions definieren. Das sub entspricht dabei einem n-stelligen
     lambda des Lambdakalkuels. name1 bis namen sind die formalen Parameter
     einer namenlosen function, welche durch beliebige Argumentausdruecke
     im Rumpf der function substituierbar sind.
     Diese sind dann mit einem ap (oder auch mehreren!) an die function
     gebunden.

 Example:          sub n,m (n + m)

                ap  sub n,m (n + m) to [7,true]  ==>  (7 + true)

^transform
+-----------------------------------------------------------------------------+
|                      transform                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:            transform( mmat  )
                       transform( mat   )
                       transform( vect  )
                       transform( tvect )
 function:
    Mit Hilfe von transform kann aus einer Matrix oder aus einem Vektor
    oder aus einem Tvektor eine Liste erzeugt werden.

 Examples:    transform(  nilmat       )    =>  <>
               transform(  niltvect     )    =>  <>
               transform(mat<1,2,3>.    )    =>  <1,2,3>
               transform(mat<1>,<2>.    )    =>  <1,2>
               transform(mat<1,2>,<2,3>.)    =>  <1,2,3,4>
               transform(vect<1,2>      )    =>  <1,2>
               transform(tvect<1,2>     )    =>  <1,2>

^ltransform
+-----------------------------------------------------------------------------+
|                     ltransform                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:     ltransform( rowpar, columnpar, list )

 function:
   Mit Hilfe von ltransform kann aus einer Liste eine Matrix erzeugt werden.
   'rowpar' gibt die Anzahl der Zeilen und 'columnpar' gibt die  Anzahl  der
   Spalten der zu erzeugenden Matrix an. Die Anzahl der  Elemente der  Liste
   muss  mit der Angabe von  ('rowpar' * 'columpar')  uebereinstimmen.  Alle
   Elemente der Liste muessen  vom  gleichen Typ sein. Wenn  ein Vektor oder
   ein  Tvektor  als  Ergebnis  der  function  gewuenscht  wird,  muss  eine
   (1xn)-mat bzw. eine (mx1)-mat erzeugt werden und diese mit Hilfe der Kon-
   versions-functions to_vect bzw. to_tvect in einen  Vektor  bzw.  Tvektor
   konvertiert werden.

 Examples:      ltransform(0,0,     <>        )  =>  nilmat
                 ltransform(2,2, <1,2,3,4>     )  =>  mat<1,2>,
                                                         <3,4>.
                 ltransform(4,1, <1,2,3,4>     )  =>  mat<1>,<2>,<3>,<4>.
                 ltransform(1,4, <1,2,3,4>     )  =>  mat<1,2,3,4>.
                 ltransform(0,0,     '`        )  =>  nilmat
                 ltransform(1,1, <   '`  >     )  =>  smat<'`>
                 ltransform(2,1, <'aa`,'bb` >  )  =>  smat<'aa`>,
                                                          <'bb`>.
^transpose
+-----------------------------------------------------------------------------+
|                         transpose                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:               transpose( mmat )
                          transpose( mat  )
                          transpose( vect )
                          transpose( tvect)
 function:
    Die function transpose ist nur auf Matrizen, Vektoren  oder Tvektoren
    anwendbar. Das Ergebnis der usage dieser function ist eine trans-
    ponierte Matrix, bzw. ein tranponierter Vektor bzw. ein Vektor.

 Examples:      transpose(     nilmat       )  =>  nilmat
                 transpose( mat<1,2>,           =>  mat<1,3>,
                               <3,4>.        )         <2,4>.
                 transpose(     nilvect      )  =>  niltvect
                 transpose( vect<1,2,3>      )  =>  tvect<1,2,3>
                 transpose(     niltvect     )  =>  nilvect
                 transpose( tvect<1,2,3>     )  =>  vect<1,2,3>

^reverse
+-----------------------------------------------------------------------------+
|                           reverse                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:                 reverse(list)       ==>  list
                            reverse(string)     ==>  string
 function:
    Die function reverse ist auf Listen und Strings anwendbar. Als Ergebnis
    entsteht eine Liste bzw. ein String , in der die Elemente der Argument-
    liste bzw. des Argumentstrings in genau  entgegengesetzter  Reihenfolge
    angeordnet sind. Das erste Element steht an letzter Stelle, das  zweite
    an vorletzter usw. Ist ein Element eine Liste  bzw. ein Teilstring,  so
    bleiben diese unveraendert.

 Examples:          reverse(     <>         )  =>  <>
                     reverse(  <1,2,3,4>     )  =>  <4,3,2,1>
                     reverse( <1,<2,3>,4>    )  =>  <4,<2,3>,1>
                     reverse(     '`         )  =>  '`
                     reverse( 'suaM`         )  =>  'Maus`
                     reverse( 'hci 'und` ud` )  =>  'du 'und` ich`

^trunc ^frac
+-----------------------------------------------------------------------------+
|                       trunc, frac                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:                    frac(argument)
                              trunc(argument)
 function:
    Beide functions sind nur auf Zahlen, Matrizen von  Zahlen, Vektoren von
    Zahlen oder Tvektoren von Zahlen defined. Bei Matrizen  bzw.  Vektoren
    bzw. Tvektoren werden die functions auf jedes Element angewandt.
    trunc erzeugt eine Kopie des Argumentes  ohne Nachkommastellen.
    frac  erzeugt eine Kopie des  Argumentes ohne Vorkommastellen.

 Examples:       frac( 1.23         )   =>  0.23
                  frac( mat<1,2.4>.  )   =>  mat<0,0.4>.
                  frac( vect<1,2.4>  )   =>  vect<0,0.4>
                  frac( tvect<1,2.4> )   =>  tvect<0,0.4>

                 trunc( 1.23         )   =>  1
                 trunc( mat<1,2.4>.  )   =>  mat<1,2>.
                 trunc( vect<1,2.4>  )   =>  vect<1,2>
                 trunc( tvect<1,2.4> )   =>  tvect<1,2>


^type ^Primfunc ^primfunc ^Userfunc ^userfunc ^Bool ^bool ^String ^Typkonstanten
+-----------------------------------------------------------------------------+
|                             type                          April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:              type( arg_expr )

 function:
    Mit Hilfe dieser function kann der user den Typ von Skalaren  und
    nicht leeren strukturierten Datenstrukturen  ermitteln. Die  function
    ist nicht auf  die  Datenstruktur  Liste  defined.  Ausserdem  kann
    ermittelt werden, ob  ein functionssymbol  einer  primitiven function
    oder eine userdefined function vorliegt.

 |           arg_expr                             zugeordnete Typkonstante |
 |-------------------------------------------------------------------------|
 | - functionssymbole primitiver functions.                 --> Primfunc  |
 | - userdefinedn functions.                         --> Userfunc  |
 | - Zahlen,  Matrizen von Zahlen,  Vektoren von Zahlen und                |
 |   Tvektoren von Zahlen.                                   --> Number    |
 | - Wahrheitswerte, Matrizen von Wahrheitswerten, Vektoren                |
 |   von Wahrheitswerten und Tvektoren von Wahrheitswerten.  --> Bool      |
 | - Strings (Texte),  Matrizen von Strings,   Vektoren von                |
 |   Strings und Tvektoren von Strings.                      --> String    |

    Kann das Argument nicht einem dieser Typen zugeordnet werden, so ist
    die functionsanwendung nicht reduzierbar.

^unite
+-----------------------------------------------------------------------------+
|                               unite                       April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage: unite( parameter1 , Struktur , Struktur )    ==>  Struktur

 function : Dieses ist  die  universelle  unite-function, die  auf  die  mit
            'Struktur' spezifizierte Struktur angewandt  werden  kann.  Wenn
            die universelle unite-function  auf  die  Strukturen  List  oder
            String angewandt wird,  dann muss der erste Parameter  eine Null
            sein. Wenn   sie  auf  die  Strukturen    Vector x Vector   oder
            Tvector x Tvector  angewandt  wird und  als  Ergebnis  wird  ein
            Vektor gewuenscht, dann muss der erste  Parameter ebenfalls eine
            Null sein.

            Parameter Struktur   Struktur      Structur     weitere Erlaeuter-
                                                            ungen see:
 unite    : 0      x  List    x  List     ==>  List     ------    lunite
            0      x  String  x  String   ==>  String   ------    lunite
            0      x  Vector  x  Vector   ==>  Vector   ------    vunite
            0      x  Tvector x  Tvector  ==>  Tvector  ------    vunite
            {1,2}  x  MMatrix x  MMatrix  ==>  MMatrix  ------    munite
            {1,2}  x  Matrix  x  Matrix   ==>  Matrix   ------    munite
            {1,2}  x  Matrix  x  Vector   ==>  Matrix   ------    munite
            {1,2}  x  Matrix  x  Tvector  ==>  Matrix   ------    munite
            {1,2}  x  Vector  x  Matrix   ==>  Matrix   ------    munite
            {1,2}  x  Vector  x  Vector   ==>  Matrix   ------    munite
            {1,2}  x  Vector  x  Tvector  ==>  Matrix   ------    munite
            {1,2}  x  Tvector x  Matrix   ==>  Matrix   ------    munite
            {1,2}  x  Tvector x  Tvector  ==>  Matrix   ------    munite
            {1,2}  x  Tvector x  Vector   ==>  Matrix   ------    munite

^lunite
+-----------------------------------------------------------------------------+
|                              lunite                       April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:              lunite( Liste1  , Liste2  )
                         lunite( String1 , String2 )

 function:  lunite kann Listen bzw. Strings vereinigen. Das Ergebnis der
            Reduktion ist die Konkatenation von zwei  beliebigen  Listen
            bzw. Strings.

 Examples: lunite( <>  , <>  )  =>  <>
            lunite( <5> , <>  )  =>  <5>
            lunite( <>  , <5> )  =>  <5>
            lunite( <7> , <9> )  =>  <7,9>
            lunite( '`  , '`  )  =>  '`
            lunite( 'Skifahren in Meribell ` , 'ist prima.` )
                                 ==> 'Skifahren in Meribell ist prima.`
^munite
+-----------------------------------------------------------------------------+
|                            munite                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:          munite( coor, arg1, arg2 )

 function:
    Diese function vereinigt Matrizen, Vektoren und Tvektoren. Die Argumente
    koennen  mit  Aussnahme  von  Matrizen auf Matrizen beliebig  kombiniert
    werden, d.h. es koennen z.B. Vektoren und Matrizen vereinigt werden. Die
    Vereinigung ist nur moeglich, wenn der Typ  der Elemente  von den beiden
    Argumenten der gleiche ist. Es wird  bei 'coor'  gleich Eins zeilenweise
    und bei 'coor' gleich Zwei spaltenweise vereinigt. munite  arbeitet  nur
    auf Kopien der Argumente 'arg1' und 'arg2'. Falls eine Matrix  des  Typs
    Integer mit einer Matrix des Typs Real vereinigt werden  soll,  so  ent-
    steht eine Matrix mit  Elementen  des Typs Real. Wenn  eine  Matrix  mit
    einem Vektor bzw. Tvektor oder ein  Vektor  mit  einem Tvektor vereinigt
    werden soll, dann  muss  die  Dimension der "entsprechenden" Spalte bzw.
    Zeile der beiden Argumente gleich sein. Das Ergebnis einer erfolgreichen
    Reduktion ist immer eine Matrix.

 Examples:  munite( 1, nilmat     , nilmat      )  =>  nilmat
             munite( 1, nilvect    , nilvect     )  =>  nilmat
             munite( 1, niltvect   , niltvect    )  =>  nilmat
             munite( 2, mat<true>  , nilmat      )  =>  bmat<true>.
             munite( 1, mat<1>.    , mat<2>.     )  =>  mat<1>,<2>.
             munite( 2, mat<1>.    , mat<2>.     )  =>  mat<1,2>.
             munite( 2, mat<1,2,3>., vect<1,1,2> )  =>  mat<1,2,3,1,1,3>.
             munite( 2, vect<1>    , vect<2>     )  =>  mat<1,2>.
             munite( 1, vect<1>    , vect<2>     )  =>  mat<1>,<2>.
             munite( 2, tvect<1>   , tvect<2>    )  =>  mat<1,2>.

^vunite
+-----------------------------------------------------------------------------+
|                            vunite                         April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:           vunite(  vect ,  vect )
                      vunite( tvect , tvect )
 function:
     Diese function vereinigt entweder zwei Vektoren  oder zwei Tvektoren.
     Die Anzahl  der  Elemente in  den  Argumenten  der  zu  vereinigenden
     Vektoren bzw. Tvektoren ist beliebig. Wenn Vektoren vereinigt werden,
     ist das  Ergebnis der Reduktion ein Vektor. Wenn Tvektoren  vereinigt
     werden, ist das Ergebnis der Reduktion ein Tvektor.

 Examples:    vunite( nilvect       , nilvect   )  =>  nilvect
               vunite( niltvect      , niltvect  )  =>  nilvect
               vunite( bvect<true>   , nilvect   )  =>  bvect<true>
               vunite( vect<1,2>     , vect<3,4> )  =>  vect<1,2,3,4>
               vunite( tvect<1,2>    , tvect<2>  )  =>  tvect<1,2,2>

^to_scal ^to_vect ^to_tvect  ^to_mat
+-----------------------------------------------------------------------------+
|              type - conversion                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:      to_scal( argument )       to_tvect( argument )
                 to_vect( argument )       to_mat( argument )
 function :
    Diese functions konvertieren das Argument in eine andere Datenstruktur
    bzw. in einen Scalar. Der nach folgende  Definition-  und  Wertebereich
    legt die Konvertierungsmoeglichkeiten  der  functions fest.  Matrizen,
    Vektoren und Tvektoren  koennen  vom  Typ Number, vom Typ Bool oder vom
    Typ String sein. Die  function  to_scal ist nicht  auf  Strukturen  des
    Typs String anwendbar. (Scalar: -> Scalar)

to_scal : (1x1)-mat     -->  Scalar  | to_tvect : (mx1)-mat     -->  m-tvect
          1-vect        -->  Scalar  |            1-vect        -->  1-tvect
          1-tvect       -->  Scalar  |            Scalar/String -->  1-tvect
          Scalar        -->  Scalar  |            1-tvect       -->  1-tvect
                                     |
to_vect : (1xn)-mat     -->  n-vect  | to_mat   : n-vect        -->  (1xn)-mat
          1-tvect       -->  1-vect  |            m-tvect       -->  (mx1)-mat
          Scalar/String -->  1-vect  |            Scalar/String -->  (1x1)-mat
          1-vect        -->  1-vect  |            (mxn)-mat     -->  (mxn)-mat

 Examples :         to_scal(mat<1>.     ) =>  1
                     to_scal(bvect<true> ) =>  true
                     to_scal(smat<'abc`>.) =>  to_scal(smat<'abc`>.)
                     to_vect(true        ) =>  bvect<true>
                     to_vect(mat<1,2>.   ) =>  vect<1,2>
                     to_tvect('heute`    ) =>  svect<'heute`>
                     to_tvect(mat<1>,<2>.) =>  tvect<1,2>
                     to_mat('heute`)     ) =>  smat<'heute`>
                     to_mat(vect<6,7>    ) =>  mat<6,7>.

^sin ^cos ^tan ^exp ^ln ^sinus ^cosinus ^tangens ^logarithmus ^exponential
+-----------------------------------------------------------------------------+
|                  sin, con, tan, exp, ln                   April - 4 - 1989  |
+-----------------------------------------------------------------------------+
 usage:           sin(radiant)         exp(arg1)
                      cos(radiant)          ln(arg1)
                      tan(radiant)

                                function
                               ==========
  fixformat
    Die Berechnung der o. a. transzendenten functions  werden im  "fixformat"
    mit Hilfe der  Standard C-Bibliothek-functions  durchgefuehrt,  d.h.  die
    Definitions- und Wertebereiche sowie die Korrektheit  der  Berechnung  ist
    abhaenig  von  der jeweiligen  Standard C-Bibliothek-function.
    (see auch: real oder integer).

  varformat
    Im "varformat" erfolgt die Berechnung der functions mit Hilfe von Taylor-
    Reihenentwicklungen. Da die  Division  der  Digit-Arithmetik  nicht  exakt
    durchgefuehrt wird (see auch: digit), ist das  Ergebnis  der  Berechnung
    der functions  mit einem  'relativer Fehler'  behaftet. Dieser  ist  aber
    nicht  schlechter als  der  vom  user vorgegebene  Reduktionsparameter
    "div_prec".  Der user kann durch die Angabe von "div_prec"  die Genau-
    igkeit der Berechnung  der transzendenten functions  bestimmen,  d.h.  er
    kann angeben, auf wieviele Stellen die functions berechnet werden sollen.

  Die Argumente von sin , cos , tan  muessen im Bogenmass eingegeben werden.
  Bei truncation = -1 werden alle bei der Berechnung entstehenden Nachkomma-
  stellen auf dem Bildschirm ausgegeben, falls diese <> 0 sind.

Examples :
  Berechnung der Sinusfunktion  auf 30 Stellen.
    Reduktionsparameter - Festlegungnen :
     varformat
     truncation = 30  sin(0.8)  =>  0.717356090899522761627174610581
     div_prec   = 30
                      sin(2000) =>  0.930039504416137007920803711941

  Berechnung der Exponentialfunktion  auf  35 Stellen.
     varformat
     truncation = 35  exp(1)    =>  2.71828182845904523536028747135266249
     div_prec   = 35

  Berechnung der Logarithmusfunktion  auf  40 Stellen.
     varformat
     truncation = 40  ln(0.5)   => ~0.6931471805599453094172321214581765680754
     div_prec   = 40

^7 ^7. ^syntax ^Syntax
+-----------------------------------------------------------------------------+
|                         syntax                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+
Folgende Backus-Naur-Form spezifiziert die vollstaendige interne Syntax
der Reduktionssprache. Die fuer den user zur  Verfuegung  gestellte
hoehere   Programmiersprache    (externe   Syntax   auf    Editorebene)
unterscheidet sich  lediglich  in  der  Darstellung  von   Programmaus-
druecken. Die  Semantik  der  Reduktionssprache  auf  Editor-  und  der
Maschinenebene  ist  die  selbe.  Durch   Transliteration   wird   ein
Programmausdruck  in  externer  Syntax  in  die  interne  Syntax  (und
umgekehrt) ueberfuehrt.

Notation                 | Bedeutung
-------------------------+----------------------------------------
<:symbol:>               | nicht terminale Symbole
          n              |
{ symbol }               | genau n-maliges Auftreten von 'symbol'(n ge 0)
symbol1 | symbol2        | symbol1 oder symbol2
[ symbol1 | symbol2 ]    | genau eine Alternative ist auszuw{hlen
[ symbol ]               | optional zu verwenden



<:prg:>                 ::= [ <:def_part:> ] <:expr:>

<:def_part:>            ::= 'def'  <:funct_list:>

<:funct_list:>          ::= [ <:funct_list:> ',' ]  <:funct_decl:>

<:funct_decl:>          ::= <:variable:> '['  [ <:var_list:> ] ']' '=' <:prg:>

<:funct_call:>          ::= <:variable:> '[' [ <:expr_list:> ] ']'

<:expr:>                ::= [ <:comment:> ] <:exp:>

<:comment:>             ::= '{' <:char_string:> '}'

<:exp:>                 ::= <:atomic_expr:>      |
                            <:constructor_expr:> |
                            <:user_kon:>         |
                            <:funct_call:>

<:user_kon:>            ::= <:variable:> '{' <:expr_list:> '}'

<:atomic_expr:>         ::= <:single_atom:>      |
                            <:decimal_number:>   |
                            <:variable:>

<:single_atom:>         ::= <:primitive_func:> | <:attributes:> |
                            <:boolean_const:>  | <:nil_const:>  |
                            ok | fail

<:primitive_function:>  ::= <:val_func:> | <:query:> | <:struct_func:>

<:val_func:>            ::= <:arithmetic:>  | <:transcendental:> |
                            <:transfer:>    | <:logic:> | <:relational:>

<:arithmetic:>          ::= +   | c_+   | vc_+   | mod |
                            -   | c_-   | vc_-   | ip  |
                            *   | c_*   | vc_*   |
                            /   | c_/   | vc_/
                            max | c_max | vc_max |
                            min | c_min | vc_min

<:transcendental:>      ::= sin | cos | tan | ln | exp

<:transfer:>            ::= abs | ceil | floor | frac | trunc | neg

<:logic:>               ::= and | not | or | xor

<:relational:>          ::= eq | ne | f_eq | f_ne |
                            ge | gt | le   | lt

<:query:>               ::= <:uni_query:> | <:special_query:>

<:uni_query:>           ::= class | dim | empty | type

<:special_query:>       ::= ldim | mdim | vdim

<:struct_func:>         ::= <:uni_struct_func:> | <:special_struct_func:> |
                            <:convert:>

<:uni_struct_func:>     ::= cut | replace | rotate | select | unite

<:special_struct_func:> ::= lcut       | mcut       | vcut       |
                            lreplace   | mreplace   | vreplace   |
                            reverse    | mreplace_c | mreplace_r |
                            lrotate    | mrotate    | vrotate    |
                            lselect    | mselect    | vselect    |
                            lunite     | munite     | vunite     |
                            repstr     | substr     | transpose

<:convert:>             ::= to_scal    | to_vect   | to_tvect | to_mat |
                            ltransform | transform

<:attributes:>          ::= <:type:> | <:class:>

<:type:>                ::= Primfunc | Userfunc | Tmatrix |
                            Number   | String   | Bool

<:class:>               ::= Function | Scalar | List    |
                            Matrix   | Vector | Tvector

<:boolean_const:>       ::= true | false

<:nil_const:>           ::= nilmat | nilvect | niltvect | <> | '`

<:variable:>            ::= <:letter:>{<:name_rest:>}

<:letter:>              ::= A  | B  | ... | Z

<:name_rest:>           ::= <:letter:> | _ | <:digit:>

<:constructor_expr:>    ::= <:ap_expr:>          |
                            <:sub_expr:>         |
                            <:cond_expr:>        |
                            <:case_expr:>        |
                            <:lambda_bar_expr:>  |
                            <:data_structure:>

<:ap_expr:>             ::= ap <:expr:> to '[' <:expr_list:> ']'

<:expr_list:>           ::= <:expr:> {, <:expr:> }

<:sub_expr:>            ::= sub '[' <:var_list:> ']' <:expr:>

<:var_list:>            ::= <:variable:> {, <:variable:> }

<:cond_expr:>           ::= then <:expr:> else <:expr:>

<:case_expr:>           ::= case <:when_list:> [ otherwise <:expr:> ] end

<:when_list:>           ::= <:when_expr:> {, <:when_expr:> }

<:when_expr:>           ::= when <:pattern:> guard <:expr:> do <:expr:>

<:pattern:>             ::= <:pattern1:> | <:pattern2:> | <user_konp>

<:pattern1:>            ::= <:variable:>                    |
                            <:pattern_const:>               |
                            < <:pattern_list:> >            |
                            .                               |
                            as <:pattern1:> <:variable:>    |
                            < ( <:variable:> ) <:variable:>

<:pattern_const:>       ::= <:single_atom:>    |
                            <:decimal_number:> |
                            <:char_string:>    |
                            <:matrix:>         |
                            <:vector:>

<:pattern_list:>        ::= <:pat_list_elem:> {, <:pat_list_elem:> }

<:pat_list_elem:>       ::= <:pattern:> | ... | $ |
                            as ... <:variable:>   |
                            as $ <:variable:>

<:pattern2:>            ::= <:variable:>                    |
                            '`                              |
                            <:pattern_string:>              |
                            .                               |
                            as <:pattern2:> <:variable:>    |
                            < ( <:variable:> ) <:variable:>

<:pattern_string:>      ::= ' { <:pat_string_elem:> } `

<:pat_string_elem:>     ::= <:char:>                           |
                            <:pattern_string:>                 |
                            "<:variable:>"                     |
                            "... | "$                          |
                            "as "... <:variable:>"             |
                            "as "$ <:variable:>"               |
                            "as <:in_cons_expr:> <:variable:>" |
                            <:in_cons_expr:>

<:in_cons_expr:>        ::= "in ". <:char_string:>"   |
                            "in "... <:char_string:>" |
                            "in "$ <:char_string:>"

<:user_konp:>           ::= <:variable:>'{' <:pattern_list:> '}'

<:lambda_bar_expr:>     ::= \ <:variable:>        |
                            \ <:lambda_bar_expr:>

<:decimal_number:>      ::= <:dec_digit_str:> ['e' <:int_dec_digit_string:>]

<:dec_digit_string:>    ::= <:int_dec_digit_str:>   |
                            <:real_dec_digit_str:>

<:int_dec_digit_str:>   ::= [ minus ] <:digit_string:>

<:real_dec_digit_str:>  ::= [ minus ] <:digit_string:> . <:digit_string:>

<:digit_string:>        ::= <:digit:> | <:digit_string:> <digit:>

<:digit:>               ::= 0  | 1  | ... | 9

<:data_structure:>      ::= <:list:>        |
                            <:char_string:> |
                            <:matrix:>      |
                            <:vector:>

<:list:>                ::= < <:expr_list:> >

<:char_string :>        ::= ' { <:char:> | <:char_string:> } ` |
                            '`

<:char:>                ::= any character without ''', '`'

<:matrix:>              ::= mmat < <:matrix_list:> >.     |
                            mat < <:number_row_list:> >.  |
                            bmat < <:bool_row_list:> >.   |
                            smat < <:string_row_list:> >.

<:matrix_list:>         ::= <:matrix:> {>, < <:matrix:> }

<:number_row_list:>     ::= <:decimal_number:> {>, < <:decimal_number:> }

<:bool_row_list:>       ::= <:boolean_const:> {>, < <:boolean_const:> }

<:string_row_list:>     ::= <:char_string:> {>, < <:char_string:> }

<:vector:>              ::= [t]vect < <:number_list:> >  |
                            b[t]vect < <:bool_list:> >   |
                            s[t]vect < <:string_list:> >

<:number_list:>         ::= <:decimal_number:> {, <:decimal_number:> }

<:bool_list:>           ::= <:boolean_const:> {, <:boolean_const:> }

<:string_list:>         ::= <:char_string:> {, <:char_string:> }

^usercon ^defined ^user
+-----------------------------------------------------------------------------+
|                   user definable constructors                               |
+-----------------------------------------------------------------------------+

The internal represantation of an user definable constructor

<:var:> '{' <:expr_list:> '}'

is

'<' ''' '_' <:var:> '`' , <:expr_list:> 

This means: the variable is transformed into a string, whose first
character is an underscore ('_').
For this reason, user defined constructors are in the domain of all list
functions, and a list of this type is transformed by the editor to an user
defined constructor.

Example: KON{1, 2} is translated to < '_KON`, 1, 2 >

^cursor
+-----------------------------------------------------------------------------+
|                   positioning the cursor                                    |
+-----------------------------------------------------------------------------+

key              | cursor expression gets
-----------------|-------------------------------------------------------------
<cursor-up>      : father expression
<cursor-down>    : right brother expression
<cursor-left>    : left neighbor expression
<cursor-right>,
<CR>             : right neighbor expression
<cursor-home>    : screen expression
<CTRL> F         : right subexpression
<CTRL> G         : goal

^funktionen ^funktion ^fkt ^def ^8 ^8.
+-----------------------------------------------------------------------------+
| Eingabe und Editieren von functions:                                       |
+-----------------------------------------------------------------------------+
Ist der Cursor auf einem functionsnamen plaziert, so ist bei fast allen
Editor-Operationen der komplette Ausdruck gemeint. Nur bei der Eingabe
einer Variablen wird lediglich der Name ueberschrieben.
Leere Parameter- oder Argument-Listen werden durch ein ' ' gekennzeichnet,
dieses Zeichen ist aber nicht eingebbar und auch nicht editierbar.
Interne functionsnamen zeichnen sich dadurch aus, dass sie mit '&' enden.
Diese sind nicht in functions-Deklarationen verwendbar, ebenso wie vor-
defined functions, die aber in Grossschrift ihre Vordefinedheit ver-
lieren.

^index
+-----------------------------------------------------------------------------+
|                          index                            April - 4 - 1989  |
+-----------------------------------------------------------------------------+

 A:
   abs
 B:
   base
 C:
   cmd (editor command)
   cursor (positioning the ~)
 D:
 E:
   editor (calling the ~)
 F:
 G:
   generic
 H:
 I:
 J:
 K:
 L:
   line editor (-> led)
 M:
 N:
   number
 O:
 P:
 Q:
 R:
   reduma
 S:
   syntax
 T:
 U:
 V:
 W:
 X:
 Y:
 Z:

^news ^new
+-----------------------------------------------------------------------------+
| News                                                       May - 12 - 1989  |
+-----------------------------------------------------------------------------+

There are some new commands to control the display of def's.
These are:
abb_def :  abbreviate def's.
abb_idef:  abbreviate only inner def's.
dgoal   :  show the goal-expression of abbreviated def's.
dfct    :  show the function-definition-header of abbreviated def's.

Second, there is a new syntactic item: let.
It is an sugared version of a sub.
It's syntax is the following:

let <var> = <expr>,
    .
    .
    <var> = <expr>
in <expr>

