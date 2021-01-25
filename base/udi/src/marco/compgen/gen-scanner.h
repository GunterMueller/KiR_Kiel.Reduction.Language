/**********************************************************************************
***********************************************************************************
**
**   File        : gen-scanner.h
**   Version     : 0.0
**   Author      : Ulrich Diedrichsen
**   Date        : 11.1.1993
**   Contents    : 
**   Changes 
**    Date       : Changes                             : Name
**    --------------------------------------------------------------
**
*/ 

#ifndef _GENSCANNER_HEADER_
#define _GENSCANNER_HEADER_ 1

/*********************************************************************************/
/********** BENOETIGTE INCLUDES **************************************************/
/*********************************************************************************/

#include <sys/types.h> 
#include <fcntl.h>

#include "gen-options.h"

/*********************************************************************************/
/********** TYPENDEFINITIONEN ****************************************************/
/*********************************************************************************/

#define BLANKS_NORMAL		0
#define BLANKS_LEADING		-1
#define BLANKS_FOLLOWING	1

/*********************************************************************************/
/********** PROGRAMM DEFINES *****************************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** MODUL GLOBALE VARIABLEN **********************************************/
/*********************************************************************************/

/*********************************************************************************/
/********** PROTOTYPE FUER EXTERNE FUNKTIONEN ************************************/
/*********************************************************************************/

/* Initialize scanner generator (called by gen-options) **************************/
void InitScannerGenerator(int htablesize);

/* Scanner description complete (called by parser ccomp.y) ***********************/
int ScannerDescComplete(void);

/* Tidy up module (called by gen-options) ****************************************/
void CleanUpScannerGenerator(void);

/* Fuegt ein Element in die Symboltabelle ein ************************************/
int AddSymbol(int type,char *name,char *text,int blank);

/* Fuegt ein Regulaeren Ausdruck als Element in die Symboltabelle ein ************/
int AddRegExprSymbol(int type,char *name,char *text);

/* Erzeugt Code zum lesen einzelner Zeichen **************************************/
int AddGetCharSymbol(char *name,char *chars);

/* Ueberprueft ob fuer die angegebene Zahl eine Standartregel existiert **********/
int CheckStdType(int sy_type,int number);

/* Test if symbol is in HashTable ************************************************/
int LookUpSymbol(char *symbolname);

/* Is the token name a stop character symbol ? ***********************************/
int IsStopCharToken(char *name);

 char *GetSymbolText(char *symbolname);

void WriteToken(FILE *fh);
void WriteScannerUnionMembers(FILE *fh);
void WriteScannerExportVars(FILE *fh);

#endif
