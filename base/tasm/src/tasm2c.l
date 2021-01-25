%{
extern int lineno;              /* aus tasmt2c.y */
#include <math.h>               /* fuer den atof()-Call */
#include <string.h>
#include <ctype.h>
#include "mytypes.h"
#include "tasm2c.tab.h"
#include "error.h"
int isatty();
static int atox(const char *);
static int str2type(char *);
%}

DIGIT    [0-9]
SIGN     [-+]
HASHME   [A-Z a-z0-9]*
ID       [A-Za-z][A-Za-z0-9]*
LABELC   [A-Za-z0-9][_A-Za-z0-9]+
OPTTYPES (\.[?a-z$]*)?
HEXA     [a-fA-F0-9]*
OCTAL    \\[0-7][0-7][0-7]
MODULE   (M[A-Za-z0-9]+_)


%%
-?{DIGIT}+                      {yylval.intv=atoi(yytext); return(INTV);}
{SIGN}{DIGIT}+"."{DIGIT}*       {yylval.realv=atof(yytext); return(REALV);}
{SIGN}{DIGIT}+"."{DIGIT}+"e"{SIGN}{DIGIT}+ {yylval.realv=atof(yytext); return(REALV);}
{SIGN}{DIGIT}+"e"{SIGN}{DIGIT}+ {yylval.realv=atof(yytext); return(REALV);}
"'"."'"                         {yylval.intv=yytext[1]; return SELEM;}
"'"\\n"'"                       {yylval.intv=(int)'\n'; return SELEM;}
"'"\\t"'"                       {yylval.intv=(int)'\t'; return SELEM;}
"'"{OCTAL}"'"                   {yylval.intv=(int)strtol(yytext+2,NULL,8);
                                 return SELEM;}
\"[^"]*\"                       {yylval.helpstring=malloc(strlen(yytext)-1);
                                 if (NULL==yylval.helpstring)
                                   yyfail("%s","Couldn't allocate memory !");
                                 strncpy(yylval.helpstring,&yytext[1],strlen(yytext)-2);
				 yylval.helpstring[strlen(yytext)-2] = '\0';
                                 return CLIST;
                                }
[pP]ush_w                         {return PUSHW;}
[pP]ushc_w                        {return PUSHCW;}
[pP]ush_aw                        {return PUSHAW;}
[pP]ush_aw0                       {return PUSHAW0;}
[pP]ush_tw                        {return PUSHTW;}
[pP]ush_tw0                       {return PUSHTW0;}
[pP]ush_r                         {return PUSHR;}
[pP]ush_ar                        {return PUSHAR;}
[pP]ush_ar0                       {return PUSHAR0;}
[pP]ush_tr                        {return PUSHTR;}
[pP]ush_tr0                       {return PUSHTR0;}
[pP]ushret                        {return PUSHRET;}
[pP]ushaux                        {return PUSHAUX;}

[mM]ove_aw                        {return MOVEAW;}
[mM]ove_ar                        {return MOVEAR;}
[mM]ove_tw                        {return MOVETW;}
[mM]ove_tr                        {return MOVETR;}

[jJ]false2{OPTTYPES}              {yylval.primf.types=str2type(yytext); return JFALSE2;}
[jJ]true2{OPTTYPES}               {yylval.primf.types=str2type(yytext); return JTRUE2;}
[jJ]cond2{OPTTYPES}               {yylval.primf.types=str2type(yytext); return JCOND2;}
[jJ]false{OPTTYPES}               {yylval.primf.types=str2type(yytext); return JFALSE;}
[jJ]true{OPTTYPES}                {yylval.primf.types=str2type(yytext); return JTRUE;}
[jJ]cond{OPTTYPES}                {yylval.primf.types=str2type(yytext); return JCOND;}
[jJ]ump                           {yylval.primf.types=str2type(yytext); return JUMP;}

[fF]ree_r                         {return FREER;}
[fF]ree1_r                        {return FREE1R;}
[fF]ree_a                         {return FREEA;}
[fF]ree1_a                        {return FREE1A;}
[fF]ree_t                         {return FREET;}
[fF]ree_w                         {return FREEW;}
[fF]reesw_t                       {return FREESWT;}
[fF]ree1sw_t                      {return FREE1SWT;}

[rR]tt                            {return RTT;}
[tT]ail                           {return TAIL;}
[rR]tf                            {return RTF;}
[rR]tm                            {return RTM;}
[rR]tc                            {return RTC;}
[rR]tp                            {return RTP;}
[aA]pply                          {return APPLY;}
[mM]kap                           {return MKAP;}
[mM]klist                         {return MKLIST;}
[mM]kilist                        {return MKILIST;}
[mM]kdclos                        {return MKDCLOS;}
[mM]kiclos                        {return MKICLOS;}
[mM]kcclos                        {return MKCCLOS;}
[mM]kbclos                        {return MKBCLOS;}
[mM]ksclos                        {return MKSCLOS;}
[mM]kgaclos                       {return MKGACLOS;}
[mM]kgsclos                       {return MKGSCLOS;}
[dD]elta1                         {return DELTA1;}
[dD]elta2                         {return DELTA2;}
[dD]elta3                         {return DELTA3;}
[dD]elta4                         {return DELTA4;}
[iI]ntact                         {return INTACT;}
[bB]eta                           {return BETA;}
[bB]etaq                          {return BETA;}
[bB]etaq0                         {return BETA;}
[bB]etaq1                         {return BETA;}
[uU]pdate                         {return BETA0UPDATE;}
[tT]ailq                          {return TAIL;}
[tT]ailq0                         {return TAIL;}
[tT]ailq1                         {return TAIL;}
[gG]amma                          {return GAMMA;}
[gG]ammabeta                      {return GAMMABETA;}

[iI]nca                           {return INCA;}
[iI]ncr                           {return INCR;}
[iI]nct                           {return INCT;}
[iI]ncw                           {return INCW;}
[tT]inca                          {return TINCA;}
[tT]incr                          {return TINCR;}
[tT]inct                          {return TINCT;}
[tT]incw                          {return TINCW;}
[dD]eca                           {return DECA;}
[dD]ecr                           {return DECR;}
[dD]ect                           {return DECT;}
[dD]ecw                           {return DECW;}
[tT]deca                          {return TDECA;}
[tT]decr                          {return TDECR;}
[tT]dect                          {return TDECT;}
[tT]decw                          {return TDECW;}
[kK]illa                          {return KILLA;}
[kK]illr                          {return KILLR;}
[kK]illt                          {return KILLT;}
[kK]illw                          {return KILLW;}
[tT]killa                         {return TKILLA;}
[tT]killr                         {return TKILLR;}
[tT]killt                         {return TKILLT;}
[tT]killw                         {return TKILLW;}

[eE]xt                            {return EXIT;}
[eE]nd                            {return END;}

[aA]dvance                        {return ADVANCE;}
[aA]tend                          {return ATEND;}
[aA]tstart                        {return ATSTART;}
[bB]ackup                         {return BACKUP;}
[bB]ind                           {return BIND;}
[bB]inds                          {return BINDS;}
[bB]indsubl                       {return BINDSUBL;}
[cC]ase                           {return CASE;}
[gG]ammacase                      {return GAMMACASE;}
[dD]ereference                    {return DEREFERENCE;}
[dD]rop                           {return DROP;}
[eE]ndlist                        {return ENDLIST;}
[eE]ndsubl                        {return ENDSUBL;}
[fF]etch                          {return FETCH;}
[iI]nitbt                         {return INITBT;}
[mM]atcharb{OPTTYPES}             {yylval.types=str2type(yytext); return MATCHARB;}
[mM]atcharbs{OPTTYPES}            {yylval.types=str2type(yytext); return MATCHARBS;}
[mM]atchbool{OPTTYPES}            {yylval.types=str2type(yytext); return MATCHBOOL;}
[mM]atchc{OPTTYPES}               {yylval.types=str2type(yytext); return MATCHC;}
[mM]atchfalse{OPTTYPES}           {yylval.types=str2type(yytext); return MATCHFALSE;}
[mM]atchin{OPTTYPES}              {yylval.types=str2type(yytext); return MATCHIN;}
[mM]atchint{OPTTYPES}             {yylval.types=str2type(yytext); return MATCHINT;}
[mM]atchlist{OPTTYPES}            {yylval.types=str2type(yytext); return MATCHLIST;}
[mM]atchnil{OPTTYPES}             {yylval.types=str2type(yytext); return MATCHNIL;}
[mM]atchnils{OPTTYPES}            {yylval.types=str2type(yytext); return MATCHNILS;}
[mM]atchprim{OPTTYPES}            {yylval.types=str2type(yytext); return MATCHPRIM;}
[mM]atchstr{OPTTYPES}             {yylval.types=str2type(yytext); return MATCHSTR;}
[mM]atchtrue{OPTTYPES}            {yylval.types=str2type(yytext); return MATCHTRUE;}
[mM]kaframe                       {return MKAFRAME;}
[mM]kbtframe                      {return MKBTFRAME;}
[mM]kcase                         {return MKCASE;}
[mM]kwframe                       {return MKWFRAME;}
[nN]estlist                       {return NESTLIST;}
[pP]ick                           {return PICK;}
[rR]estorebt                      {return RESTOREBT;}
[rR]estoreptr                     {return RESTOREPTR;}
[rR]mbtframe                      {return RMBTFRAME;}
[rR]mwframe                       {return RMWFRAME;}
[sS]avebt                         {return SAVEBT;}
[sS]aveptr                        {return SAVEPTR;}
[sS]tartsubl                      {return STARTSUBL;}
[tT]guard                         {return TGUARD;}
[mM]kframe                        {return MKFRAME;}
[mM]kslot                         {return MKSLOT;}
[iI]nter                          {return INTER;}
[pP]ushh                          {return PUSHH;}
[cC]ount                          {return COUNT;}
[dD]ist                           {return DIST;}
[dD]istb                          {return DISTB;}
[dD]ist_end                       {return DISTEND;}
[wW]ait                           {return WAIT;}
[pP]oph                           {return POPH;}
[mM]sdistend                      {return MSDISTEND;}
[mM]snodist                       {return MSNODIST;}

real                              {return REAL;}
int                               {return INT;}
char                              {return CHAR;}
digit                             {return DIGIT;}
bool                              {return BOOL;}
string                            {return STRING;}
none                              {return NONE;}

_desc                             {return FUN_DESC;}
0x{HEXA}                          {yylval.intv = atox(yytext);
                                   return HEX_NUM;}
start{DIGIT}*_{LABELC}             {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return GOAL_LABEL;}
{MODULE}start{DIGIT}*_{LABELC}     {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return FUNC_LABEL;}
{MODULE}?func{DIGIT}*_{LABELC}     {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return FUNC_LABEL;}
{MODULE}?jcond{DIGIT}*_{LABELC}    {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return JCOND_LABEL;}
{MODULE}?cond{DIGIT}*_{LABELC}     {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return COND_LABEL;}
{MODULE}?cont{DIGIT}*_{LABELC}     {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return CONT_LABEL;}
{MODULE}?case{DIGIT}*_{LABELC}     {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return CASE_LABEL;}
{MODULE}?when{DIGIT}*_{LABELC}     {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return CASE_LABEL;}
{MODULE}?fail{DIGIT}*_{LABELC}     {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return FAIL_LABEL;}
{MODULE}?undecided{DIGIT}*_{LABELC} {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return UND_LABEL;}
{MODULE}?bt{DIGIT}*_{LABELC}       {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return BT_LABEL;}
{MODULE}?inloop{DIGIT}*_{LABELC}   {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return INLOOP_LABEL;}
{MODULE}?distarg{DIGIT}*_{LABELC}   {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return DISTARG_LABEL;}
{MODULE}?distend{DIGIT}*_{LABELC}   {yylval.func_lab = (char*)malloc(strlen(yytext)+1);
                                   strcpy(yylval.func_lab,yytext);
                                   return DISTEND_LABEL;}
ptd                               {return PTD;}
_real                             {return D_REAL;}
_vect                             {return D_VECT;}
_tvect                            {return D_TVECT;}
_mat                              {return D_MAT;}
_list                             {return D_LIST;}
_str                              {return D_STRING;}
_name                             {return D_NAME;}
TY_INTEGER                        {return TY_INT;}
TY_BOOL                           {return TY_BO;}
TY_REAL                           {return TY_RE;}
TY_UNDEF                          {return TY_UN;}
FUNC                              {return D_FUNC;}
COMB                              {return D_COMB;}
CONDI                             {return D_CONDI;}
CASE                              {return D_CASE;}

true                              {yylval.intv = 1; return TRUE_Y;}
false                             {yylval.intv = 0; return FALSE_Y;}

abs{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return ABS;}
neg{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return NEG;}
not{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return NOT;}
exp{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return EXP;}
ln{OPTTYPES}                      {yylval.primf.types=str2type(yytext); return LN;}
sin{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return SIN;}
cos{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return COS;}
tan{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return TAN;}
floor{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return FLOOR;}
ceil{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return CEIL;}
frac{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return FRAC;}
trunc{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return TRUNC;}
vc_plus{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return VC_PLUS;}
vc_minus{OPTTYPES}                {yylval.primf.types=str2type(yytext); return VC_MINUS;}
vc_mult{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return VC_MULT;}
vc_div{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return VC_DIV;}
vc_min{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return VC_MIN;}
vc_max{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return VC_MAX;}
ldim{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return LDIM;}
vdim{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return VDIM;}
mdim{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return MDIM;}
mdimension{OPTTYPES}              {yylval.primf.types=str2type(yytext); return MDIM;}
class{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return CLASS;}
type{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return TYPE;}
empty{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return EMPTY;}
transpose{OPTTYPES}               {yylval.primf.types=str2type(yytext); return TRANSPOSE;}
reverse{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return REVERSE;}
to_list{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return TO_LIST;}
to_scal{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return TO_SCAL;}
to_vect{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return TO_VECT;}
to_tvect{OPTTYPES}                {yylval.primf.types=str2type(yytext); return TO_TVECT;}
to_mat{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return TO_MAT;}
c_plus{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return C_PLUS;}
c_minus{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return C_MINUS;}
c_mult{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return C_MULT;}
c_div{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return C_DIV;}
c_max{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return C_MAX;}
c_min{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return C_MIN;}
plus{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return PLUS;}
minus{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return MINUS;}
mult{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return MULT;}
div{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return DIV;}
quot{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return QUOT;}
mod{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return MOD;}
ip{OPTTYPES}                      {yylval.primf.types=str2type(yytext); return IP;}
max{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return MAX;}
min{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return MIN;}
and{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return AND;}
or{OPTTYPES}                      {yylval.primf.types=str2type(yytext); return OR;}
xor{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return XOR;}
eq{OPTTYPES}                      {yylval.primf.types=str2type(yytext); return EQ;}
neq{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return NE;}
f_eq{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return F_EQ;}
f_ne{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return F_NE;}
geq{OPTTYPES}                     {yylval.primf.types=str2type(yytext); return GE;}
gt{OPTTYPES}                      {yylval.primf.types=str2type(yytext); return GT;}
le{OPTTYPES}                      {yylval.primf.types=str2type(yytext); return LE;}
lt{OPTTYPES}                      {yylval.primf.types=str2type(yytext); return LT;}
lselect{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return LSELECT;}
lcut{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return LCUT;}
lrotate{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return LROTATE;}
lunite{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return LUNITE;}
vselect{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return VSELECT;}
vcut{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return VCUT;}
vrotate{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return VROTATE;}
vunite{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return VUNITE;}
lreplace{OPTTYPES}                {yylval.primf.types=str2type(yytext); return LREPLACE;}
mselect{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return MSELECT;}
mcut{OPTTYPES}                    {yylval.primf.types=str2type(yytext); return MCUT;}
mrotate{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return MROTATE;}
munite{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return MUNITE;}
mre_r{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return MRE_R;}
mre_c{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return MRE_C;}
mreplace_r{OPTTYPES}              {yylval.primf.types=str2type(yytext); return MRE_R;}
mreplace_c{OPTTYPES}              {yylval.primf.types=str2type(yytext); return MRE_C;}
ltransform{OPTTYPES}              {yylval.primf.types=str2type(yytext); return LTRANSFORM;}
transform{OPTTYPES}               {yylval.primf.types=str2type(yytext); return TRANSFORM;}
mreplace{OPTTYPES}                {yylval.primf.types=str2type(yytext); return MREPLACE;}
vreplace{OPTTYPES}                {yylval.primf.types=str2type(yytext); return VREPLACE;}
repstr{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return REPSTR;}

    /* cr 22/03/95, kir(ff), START */
fselect{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return FSELECT;}
fupdate{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return UPDATE;}
ftest{OPTTYPES}                   {yylval.primf.types=str2type(yytext); return TEST;}
fslots{OPTTYPES}                  {yylval.primf.types=str2type(yytext); return SLOTS;}
fdelete{OPTTYPES}                 {yylval.primf.types=str2type(yytext); return DELETE;}

    /* cr 22/03/95, kir(ff), END */

    /* stt, 09.05.96 */
sprintf{OPTTYPES}      {yylval.primf.types=str2type(yytext); return SPRINTF;}
sscanf{OPTTYPES}       {yylval.primf.types=str2type(yytext); return SSCANF;}
to_char{OPTTYPES}      {yylval.primf.types=str2type(yytext); return TO_CHAR;}
to_ord{OPTTYPES}       {yylval.primf.types=str2type(yytext); return TO_ORD;}

done                              {return DONE;}
get                               {return GET;}
put                               {return PUT;}

    /* stt, 13.02.96 */
ia_fopen{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_FOPEN;}
ia_fclose{OPTTYPES}    {yylval.primf.types=str2type(yytext); return IA_FCLOSE;}
ia_fgetc{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_FGETC;}
ia_fputc{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_FPUTC;}
ia_ungetc{OPTTYPES}    {yylval.primf.types=str2type(yytext); return IA_UNGETC;}
ia_fgets{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_FGETS;}
ia_read{OPTTYPES}      {yylval.primf.types=str2type(yytext); return IA_READ;}
ia_fputs{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_FPUTS;}
ia_fprintf{OPTTYPES}   {yylval.primf.types=str2type(yytext); return IA_FPRINTF;}
ia_fredirect{OPTTYPES} {yylval.primf.types=str2type(yytext); return IA_FREDIRECT;}
ia_fseek{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_FSEEK;}
ia_ftell{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_FTELL;}
ia_exit{OPTTYPES}      {yylval.primf.types=str2type(yytext); return IA_EXIT;}
ia_eof{OPTTYPES}       {yylval.primf.types=str2type(yytext); return IA_EOF;}
ia_bind{OPTTYPES}      {yylval.primf.types=str2type(yytext); return IA_BIND;}
ia_unit{OPTTYPES}      {yylval.primf.types=str2type(yytext); return IA_UNIT;}
ia_finfo{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_FINFO;}
ia_status{OPTTYPES}    {yylval.primf.types=str2type(yytext); return IA_STATUS;}
ia_remove{OPTTYPES}    {yylval.primf.types=str2type(yytext); return IA_REMOVE;}
ia_rename{OPTTYPES}    {yylval.primf.types=str2type(yytext); return IA_RENAME;}
ia_copy{OPTTYPES}      {yylval.primf.types=str2type(yytext); return IA_COPY;}
ia_mkdir{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_MKDIR;}
ia_rmdir{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_RMDIR;}
ia_chdir{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_CHDIR;}
ia_getwd{OPTTYPES}     {yylval.primf.types=str2type(yytext); return IA_GETWD;}
ia_dir{OPTTYPES}       {yylval.primf.types=str2type(yytext); return IA_DIR;}
ia_get{OPTTYPES}       {yylval.primf.types=str2type(yytext); return IA_GET;}
ia_put{OPTTYPES}       {yylval.primf.types=str2type(yytext); return IA_PUT;}


"#"[ \t]*args                     {return HASHARGS;}
"#"[ \t]*tildeargs                {return HASHTILDEARGS;}
"#"[ \t]*restype                  {return HASHRESTYPE;}
"#"[ \t]*setref                   {return HASHSETREF;}
"#"[ \t]*addr                     {return HASHLOCATION;}
"#"[ \t]*update                   {return HASHUPDATE;}
implicit                          {return IMPLICIT;}
explicit                          {return EXPLICIT;}
keepimplicit                      {return KEEPIMPL;}
{ID}                              {printf( "unknown identifier: %s\n", yytext );}
"/*"[^\n]*"*/"                    /* Kommentare fressen */
[ \t]+                            /* Leerzeichen fressen */
\n                                lineno++;

"("                               {return LBRACKET;}
")"                               {return RBRACKET;}
"<"                               {return C_LBRACK;}
">"                               {return C_RBRACK;}
"["                               {return LBRACK;}
"]"                               {return RBRACK;}
"'"                               {return LQUOTE;}
"`"                               {return RQUOTE;}
";"                               {return SEMICOLON;}
","                               {return ',';}
":"                               {return COLON;}
.                                 {printf( "Ungueltiges Zeichen: %s\n", yytext );}
%%

#ifdef FLEX_SCANNER
static void yy_flex_strcpy( char * a, const char * b)
  {
  }
void dummy()
  {
  yyunput(0, NULL);
  yy_push_state(0);
  yy_pop_state();
  yy_top_state();
  yy_flex_strcpy(NULL, NULL);
  }
#endif /* FLEX_SCANNER */

static int getatomtype(char t)
  {
  switch(t) {
    case 'b': return tp_bool;
    case 'i': return tp_int;
    case 'r': return tp_real;
    case 's': return tp_string;
    case 'd': return tp_digit;
    case 'e': return tp_empty;
    case '?': return tp_none;
    }
  yywarn("illegal type specifier '%c' encountered", t);
  return tp_none;
  }

/* convert short ascii notation of operand types
 * for primitive functions into internal integer
 * coded notation
 */
static int str2type(char * str)
  {
  int i, exttype, types = TYPES2LONG(tp_none, tp_none, tp_none, tp_none);

  /* first: skip the command itself (means all stuff until '_')
   */
  str = strchr(str, '.');
  if (str) str++;
  for(i = 0; str && *str && (i < MAXDELTAARGS); i++, str++) {
    switch(*str) {
        case 'l': exttype = tp_list; break;
        case 'v': exttype = tp_vect; str++; break;
        case 't': exttype = tp_tvect; str++; break;
        case 'm': exttype = tp_matrix; str++; break;
        default:  exttype = tp_none;
        }
    types = MERGETYPE(DELTYPE(i, types), SETTYPE(i, MAKETYPE(exttype,
                          (exttype == tp_list ? tp_none : getatomtype(*str)))));
    }
  if (str && *str)
    yywarn("too many type for primitive function");
  return types;
  }

static int atox(const char * s)
  {
  int n = 0;
  extern int toupper();

  while(*s) {
    n = n*16 + (isdigit(*s) ? *s-'0' : toupper(*s)-'A'+10);
    s++;
    }
  return n;
  }

