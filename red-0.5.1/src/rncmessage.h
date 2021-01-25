/***************************************************************/
/*                                                             */
/* rncmessage.h --- header file for the nCube message routines */
/*                                                             */
/* ach 26/01/93                                                */
/*                                                             */
/***************************************************************/

#ifndef _rncmessage_h_included
#define _rncmessage_h_included

#if D_MESS
typedef enum {MT_NEW_GRAPH=100, MT_NEW_PROCESS, MT_RESULT, MT_END_RESULT, MT_PARAM, MT_NCUBE_READY, MT_CLOSE_MEASURE, MT_OPEN_MEASURE, MT_SEND_MEASURE, MT_POST_MORTEM, MT_ABORT} MESSAGE_TYPE;
#else
typedef enum {MT_NEW_GRAPH=100, MT_NEW_PROCESS, MT_RESULT, MT_END_RESULT, MT_PARAM, MT_NCUBE_READY,MT_POST_MORTEM, MT_ABORT} MESSAGE_TYPE;
#endif

typedef enum {CM_ONCE, CM_CONTINUOUSLY} CHECK_MODE;

#define END_MARKER      0x42424242
#define PM_DESC_MARKER  0x26000001         /* for sending of pattern match code */

#endif






