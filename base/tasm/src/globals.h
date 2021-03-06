#ifndef __GLOBALS__
#define __GLOBALS__

#include "mytypes.h"
#include <stdlib.h>
#include <stdio.h>

CODE *code = NULL;
CODE first_ft = {"/* program automatically generated by tasm */\n",NULL}, 
        *functable = &first_ft;

CODE second_ft = {"/* table of closures for existing comb 'n' cond   */"
                    ,NULL}, *funcclostable = &second_ft;


  
/* DO NOT CHANGE THE ORDER OF THE PRIMFUNCTIONS, THEY'RE SORTED */ 
/* sf 21.11.94 */
char *primf_tab[]=
{
/*********************/
/* DYADIC FUNCTIONS  */
/*********************/
"D_PLUS",
"D_MINUS",
"D_MULT",
"D_DIV",
"D_MOD",
"D_OR",
"D_AND",
"D_XOR",
"D_EQ",
"D_NEQ",
"D_LESS",
"D_LEQ",
"D_GREAT",
"D_GEQ",
"D_MAX",
"D_MIN",
"D_QUOT",
/*********************/
/* MONADIC FUNCTIONS */
/*********************/
"M_NOT",
"M_ABS",
"M_NEG",
"M_TRUNCATE",
"M_FLOOR",
"M_CEIL",
"M_FRAC",
"M_EMPTY",
"M_SIN",
"M_COS",
"M_TAN",
"M_LN",
"M_EXP",
/*****************/
/* INNER PRODUCT */
/*****************/
"IP",
/***********/
/* QUERIES */
/***********/
"CLASS",
"TYPE",
"DIMENSION",
"VDIMENSION",
"MDIMENSION",
"LDIMENSION",
/***********/
/* GROUP 1 */
/***********/
"TRANSPOSE",
"REVERSE",
/***********/
/* GROUP 2 */
/***********/
"CUT",
"MCUT",
"VCUT",
"LTRANSFORM",
"ROTATE",
"VROTATE",
"MROTATE",
"SELECT",
"VSELECT",
"MSELECT",
"SUBSTR",
/************************_LGEN_UH**************/
"LCUT",
"TRANSFORM",
"LROTATE",
"LSELECT",
/************************_D_CONS***************/
/***********/
/* GROUP 3 */
/***********/
"LREPLACE",
"REPSTR",
"REPLACE",
"MREPLACE_C",
"MREPLACE_R",
"VREPLACE",
"MREPLACE",
/***********/
/* GROUP 4 */
/***********/
/************************UNI_VECT***************/
/***********/
/* GROUP 5 */
/***********/
/************************UNI_MATRIX***************/
/***********/
/* GROUP 6 */
/***********/
/************************UNI_TREE***************/
/***********/
/* GROUP 7 */
/***********/
"UNI",
"VUNI",
"MUNI",
"LUNI",
/************************/
/* CONVERSION FUNCTIONS */
/************************/
"TO_VECT",
"TO_TVECT",
"TO_MAT",
"TO_SCAL",
"TO_LIST",
"TO_FIELD",
/************************/
/* ALONG A COORDINATE   */
/************************/
"C_MAX",
"VC_MAX",
"C_MIN",
"VC_MIN",
"C_PLUS",
"VC_PLUS",
"C_MINUS",
"VC_MINUS",
"C_MULT",
"VC_MULT",
"C_DIV",
"VC_DIV",
/************************/
/* FIELD (NOT) EQUAL    */
/************************/
"F_EQ",
"F_NE",
/* cr 22/03/95, kir(ff), START */
/************************/
/* FRAME FUNCTIONS      */
/************************/
"FR_UPDATE",
"FR_SELECT",
"FR_TEST",
"FR_DELETE",
"FR_SLOTS",
/* cr 22/03/95, kir(ff), END */
"SPRINTF",
"SSCANF",
"TO_CHAR",
"TO_ORD"
};

char *opt_primf_tab[]=
{
/*********************/
/* DYADIC FUNCTIONS  */
/*********************/
"TA_PLUS(%s, %s)",
"TA_MINUS(%s, %s)",
"TA_MULT(%s, %s)",
"TA_DIV(%s, %s)",
"TA_MOD(%s, %s)",
"TA_OR(%s, %s)",
"TA_AND(%s, %s)",
"TA_XOR(%s, %s)",
"TA_EQ(%s, %s)",
"TA_NEQ(%s, %s)",
"TA_LESS(%s, %s)",
"TA_LEQ(%s, %s)",
"TA_GREAT(%s, %s)",
"TA_GEQ(%s, %s)",
"TA_MAX(%s, %s)",
"TA_MIN(%s, %s)",
"TA_QUOT(%s, %s)",
/*********************/
/* MONADIC FUNCTIONS */
/*********************/
"TA_NOT(%s)",
"TA_ABS(%s)",
"TA_NEG(%s)",
"TA_TRUNCATE(%s)",
"TA_FLOOR(%s)",
"TA_CEIL(%s)",
"TA_FRAC(%s)",
"TA_EMPTY(%s)",
"TA_SIN(%s)",
"TA_COS(%s)",
"TA_TAN(%s)",
"TA_LN(%s)",
"TA_EXP(%s)",
/*****************/
/* INNER PRODUCT */
/*****************/
"TA_IP(%s, %s)",
/***********/
/* QUERIES */
/***********/
"TA_CLASS(%s)",
"TA_TYPE(%s)",
"TA_DIMENSION(%s)",
"TA_VDIMENSION(%s)",
"TA_MDIMENSION(%s, %s)",
"TA_LDIMENSION(%s)",
/***********/
/* GROUP 1 */
/***********/
"TA_TRANSPOSE(%s)",
"TA_REVERSE(%s)",
/***********/
/* GROUP 2 */
/***********/
"TA_CUT(%s, %s, %s)",
"TA_MCUT(%s, %s, %s)",
"TA_VCUT(%s, %s)",
"TA_LTRANSFORM(%s, %s, %s)",
"TA_ROTATE(%s, %s, %s)",
"TA_VROTATE(%s, %s)",
"TA_MROTATE(%s, %s, %s)",
"TA_SELECT(%s, %s, %s)",
"TA_VSELECT(%s, %s)",
"TA_MSELECT(%s, %s, %s)",
"TA_SUBSTR(%s, %s, %s)",
/************************_LGEN_UH**************/
"TA_LCUT(%s, %s)",
"TA_TRANSFORM(%s)",
"TA_LROTATE(%s, %s)",
"TA_LSELECT(%s, %s)",
/************************_D_CONS***************/
/***********/
/* GROUP 3 */
/***********/
"TA_LREPLACE(%s, %s, %s)",
"TA_REPSTR(%s, %s, %s, %s)",
"TA_REPLACE(%s, %s, %s, %s)",
"TA_MREPLACE_C(%s, %s, %s)",
"TA_MREPLACE_R(%s, %s, %s)",
"TA_VREPLACE(%s, %s, %s)",
"TA_MREPLACE(%s, %s, %s, %s)",
/***********/
/* GROUP 4 */
/***********/
/************************UNI_VECT***************/
/***********/
/* GROUP 5 */
/***********/
/************************UNI_MATRIX***************/
/***********/
/* GROUP 6 */
/***********/
/************************UNI_TREE***************/
/***********/
/* GROUP 7 */
/***********/
"TA_UNI(%s, %s, %s)",
"TA_VUNI(%s, %s)",
"TA_MUNI(%s, %s, %s)",
"TA_LUNI(%s, %s)",
/************************/
/* CONVERSION FUNCTIONS */
/************************/
"TA_TO_VECT(%s)",
"TA_TO_TVECT(%s)",
"TA_TO_MAT(%s)",
"TA_TO_SCAL(%s)",
"TA_TO_LIST(%s)",
"TA_TO_FIELD(%s)",
/************************/
/* ALONG A COORDINATE   */
/************************/
"TA_C_MAX(%s, %s)",
"TA_VC_MAX(%s)",
"TA_C_MIN(%s, %s)",
"TA_VC_MIN(%s)",
"TA_C_PLUS(%s, %s)",
"TA_VC_PLUS(%s)",
"TA_C_MINUS(%s, %s)",
"TA_VC_MINUS(%s)",
"TA_C_MULT(%s, %s)",
"TA_VC_MULT(%s)",
"TA_C_DIV(%s, %s)",
"TA_VC_DIV(%s)",
/************************/
/* FIELD (NOT) EQUAL    */
/************************/
"TA_F_EQ(%s, %s)",
"TA_F_NE(%s, %s)",
/* cr 22/03/95, kir(ff), START */
/************************/
/* FRAME FUNCTIONS      */
/************************/
"TA_FR_UPDATE(%s, %s, %s)",
"TA_FR_SELECT(%s, %s)",
"TA_FR_TEST(%s, %s)",
"TA_FR_DELETE(%s, %s)",
"TA_FR_SLOTS(%s)",
/* cr 22/03/95, kir(ff), END */
"TA_SPRINTF(%s, %s)",
"TA_SSCANF(%s, %s)",
"TA_TO_CHAR(%s)",
"TA_TO_ORD(%s)"
};


/* stt 13.02.96 */
/* The elements of IACTION (mytypes.h) are used as indexes for intact_tab[]. */
/* Make sure that IACTION and intact_tab[] match each other.                 */
char *intact_tab[]=
{
"IA_FOPEN",
"IA_FCLOSE",
"IA_FGETC",
"IA_FPUTC",
"IA_UNGETC",
"IA_FGETS",
"IA_READ",
"IA_FPUTS",
"IA_FPRINTF",
"IA_FREDIRECT",
"IA_FSEEK",
"IA_FTELL",
"IA_EXIT",
"IA_EOF",
"IA_BIND",
"IA_UNIT",
"IA_FINFO",
"IA_STATUS",
"IA_REMOVE",
"IA_RENAME",
"IA_COPY",
"IA_MKDIR",
"IA_RMDIR",
"IA_CHDIR",
"IA_GETWD",
"IA_DIR",
"IA_GET",
"IA_PUT"
};

#endif
