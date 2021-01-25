/* This file is automatically generated (by scr2c.awk)
** DO NOT EDIT BELOW!
*/

#include <dbug.h>

#include <msMacros.h>
#include <scriptMacros.h>
#include <groups.h>
#include <externVars.h>

extern void SET_MODE();
extern void SET_X_LABEL();
extern void SET_Y_LABEL();
extern void SET_X_GRID();
extern void SET_Y_GRID();
extern void SET_X_MIN();
extern void SET_X_MAX();
extern void SET_Y_MIN();
extern void SET_Y_MAX();
extern void SET_LEGEND();
extern void SET_STYLE();
extern void SET_LINESTYLE();
extern void SET_MARKER();
extern void SET_SHADE();
extern void CREATE_BAR();
extern void INCREMENT();
extern void REMOVE_XY();
extern void ADD_XY();
extern void UPDATE_XY();
extern void CREATION_ARROW();
extern void FILL_SLOT();
extern void SHOW_HISTORY();
extern void SHOW_FUTURE();
extern void SET_METER();
extern void DRAW_METER();
extern void SET_DIMENSIONS();
extern void FILL_ALL();
extern void FILL_BLOCK();
extern int GET_HEAPBASE();
extern void DRAW_FRAGMENTATION();
extern void SET_CACHING();
extern void SET_STACKING();
extern void SET_SEPARATION();
extern void SET_FRAGMENTATION();
extern void ADD_NODE();
extern void REMOVE_NODE();
extern void SET_NODES();
extern void SET_LABELS();
extern void SET_DEPTH();
extern void ZOOM_IN();
extern void ZOOM_OUT();
extern void SHOW_TOP();
extern void NEXT_FRAME();
extern void NEXT_FRAME_BUT_CONTINUE();
extern void SET_DELAY();
extern void SET_RATE();
extern void SET_TITLE();
extern void SET_STYLE();
extern void SET_COLORS();
extern void GET_TIME();
extern void HEADER_FILE();
extern void HEADER_TIME();
extern void HEADER_NODISTNUM();
extern void HEADER_DIMENSION();
extern void HEADER_STEPS();
extern void HEADER_FACTOR();
extern void HEADER_BINARY();
extern void HEADER_HEAPSIZE();
extern void HEADER_HEAPDES();
extern void FLUSH();

extern void SAVE();
extern void PRINT();
extern void CLEAR();

static int pids[MAX_PROCESSORS][MAX_PROCESSES],
           numProcesses[MAX_PROCESSORS];
static struct {
	int count, doSleep;
} delay[MAX_PROCESSORS][MAX_PROCESSES];
#define LOAD 0
#define COMM 1
#define IDLE 2

static int numProcessors, flag[MAX_PROCESSORS];
static struct {
	double since, load, comm;
	short addTo, oldAddTo;
} lOMStuff[MAX_PROCESSORS];
static double startTime;
static int i, j, k, arrowVisible;
static short rememberFromProcessor, rememberFromProcess,
             rememberToProcessor, rememberToProcess;
static char rememberFile[100], rememberTime[100], rememberSteps[100],
            rememberDimension[100], temp[1000];
int pid(processor, realPid)
int processor, realPid;
{
	int i;
	for(i=0; i<numProcesses[processor]; ++i) {
		if(pids[processor][i] == realPid) {
			return(i);
		}
	}
	return(-1);
}
void clearArrow()
{
	/* Erase arrow and fill destination slot with ready color */
	if(arrowVisible) {
		CREATION_ARROW(rememberFromProcessor,
		               rememberFromProcess,
		               rememberToProcessor,
		               rememberToProcess);
		arrowVisible = 0;
	}
	return;
}

void drawTheLoadOMeter(p)
int p;
{
	double l = lOMStuff[p].load,
	       c = lOMStuff[p].comm;
	switch(lOMStuff[p].addTo) {
		case LOAD:
			l += ((TIME - startTime) - lOMStuff[p].since);
			break;
		case COMM:
			c += ((TIME - startTime) - lOMStuff[p].since);
			break;
	}
	DRAW_METER(p, l, c, TIME - startTime - l - c);
	return;
}
void drawTheLoadOMeters()
{
	int p;
	for(p=0; p<numProcessors; ++p) {
		drawTheLoadOMeter(p);
	}
	return;
}


void eval()
{
  /*DBUG_ENTER("eval");*/

  switch(ID) {

  case MINDEX_BEGIN:
for(i=0; i<numProcessors; ++i) {
	for(j=0; j<numProcessors; ++j) {
		pids[i][j] = -1;
		delay[i][j].count = 
		delay[i][j].doSleep = 0;
	}
	numProcesses[i] = 0;
	flag[i] = 0;
}
/* Colors for "Ready", "Active", "Suspended" and "Terminated" */
SET_COLORS(LIGHT_YELLOW, GREEN, LIGHT_RED, WHITE);
SET_METER(ON);
HEADER_FILE(rememberFile);
HEADER_TIME(rememberTime);
HEADER_STEPS(rememberSteps);
sprintf(temp, "%s (%s) -- %s reduction steps",
        rememberFile, rememberTime, rememberSteps);
SET_TITLE(temp);
HEADER_DIMENSION(rememberDimension);
numProcessors = (1 << atoi(rememberDimension));
SET_STYLE(TEXT);
arrowVisible = 0;
startTime = 0.0;
NEXT_FRAME();
for(i=0; i<numProcessors; ++i) {
	lOMStuff[i].load = lOMStuff[i].comm = 0.0;
	lOMStuff[i].addTo = IDLE;
	drawTheLoadOMeter(i);
}
break;

  case MINDEX_SEND_BEGIN:
if(startTime == 0.0) {
	startTime = TIME;
}
if(flag[PROCESSOR] == 2) {
	if(lOMStuff[PROCESSOR].addTo == LOAD) {
		lOMStuff[PROCESSOR].oldAddTo = LOAD;
		lOMStuff[PROCESSOR].load += ((TIME - startTime) - 
	                                    lOMStuff[PROCESSOR].since);
	} else {
		lOMStuff[PROCESSOR].oldAddTo = IDLE;
	}
	lOMStuff[PROCESSOR].addTo = COMM;
	lOMStuff[PROCESSOR].since = (TIME - startTime);
	drawTheLoadOMeters();
}
if(!strcmp(S_TYPE, "MT_NEW_PROCESS")) {
	++delay[PROCESSOR][pid(PROCESSOR, PROCESS)].count;
}
break;

  case MINDEX_RECEIVE_BEGIN:
if(startTime == 0.0) {
	startTime = TIME;
}
if(!flag[PROCESSOR]) {
	flag[PROCESSOR] = 1;
} else {
	if(flag[PROCESSOR] == 1) {
		if(PROCESSOR == 0) {
			startTime = TIME;
		}
		flag[PROCESSOR] = 3;
	}
}
if(flag[PROCESSOR] >= 2) {
	if((flag[PROCESSOR] == 2) &&
	   (lOMStuff[PROCESSOR].addTo == LOAD)) {
		lOMStuff[PROCESSOR].oldAddTo = LOAD;
		lOMStuff[PROCESSOR].load += ((TIME - startTime) - 
		                            lOMStuff[PROCESSOR].since);
	} else {
		lOMStuff[PROCESSOR].oldAddTo = IDLE;
	}
	flag[PROCESSOR] = 2;
	lOMStuff[PROCESSOR].addTo = COMM;
	lOMStuff[PROCESSOR].since = (TIME - startTime);
	drawTheLoadOMeters();
}
break;	

  case MINDEX_SEND_END:
if(startTime == 0.0) {
	startTime = TIME;
}
if(flag[PROCESSOR] == 2) {
	lOMStuff[PROCESSOR].comm += ((TIME - startTime) - 
	                             lOMStuff[PROCESSOR].since);
	if(lOMStuff[PROCESSOR].oldAddTo == LOAD) {
		lOMStuff[PROCESSOR].addTo = LOAD;
		lOMStuff[PROCESSOR].since = (TIME - startTime);
	} else {
		lOMStuff[PROCESSOR].addTo = IDLE;
	}
	drawTheLoadOMeters();
}
break;	

  case MINDEX_PROC_CREATE:
if(startTime == 0.0) {
	startTime = TIME;
}
clearArrow();
/* Is this pid in the table yet? */
if(pid(PROCESSOR, PC_PID) == -1) {
	pids[PROCESSOR][numProcesses[PROCESSOR]++] = PC_PID;
}
break;

  case MINDEX_RECEIVE_END:
if(startTime == 0.0) {
	startTime = TIME;
}
if(flag[PROCESSOR] == 2) {
	lOMStuff[PROCESSOR].comm += ((TIME - startTime) -
	                             lOMStuff[PROCESSOR].since);
	if(lOMStuff[PROCESSOR].oldAddTo == LOAD) {
		lOMStuff[PROCESSOR].addTo = LOAD;
		lOMStuff[PROCESSOR].since = (TIME - startTime);
	} else {
		lOMStuff[PROCESSOR].addTo = IDLE;
	}
	drawTheLoadOMeters();
}
/*   PROCESSOR: processor with new process */
/*   R_FOR_PID: the new process on this processor */
/* R_FROM_PROC: initiating processor */
/*  R_FROM_PID: initiating process on this processor */
clearArrow();
if(!strcmp(R_TYPE, "MT_NEW_PROCESS")) {
	/* Fill slot and draw arrow... */
	rememberFromProcessor = R_FROM_PROC;
	rememberFromProcess = pid(R_FROM_PROC, R_FROM_PID);
	rememberToProcessor = PROCESSOR;
	rememberToProcess = pid(PROCESSOR, R_FOR_PID);
	FILL_SLOT(rememberToProcessor, rememberToProcess,
	          rememberFromProcessor, rememberFromProcess,
	          READY);
	CREATION_ARROW(rememberFromProcessor, rememberFromProcess,
	               rememberToProcessor, rememberToProcess);
	arrowVisible = 1;
	NEXT_FRAME_BUT_CONTINUE();
}
break;

  case MINDEX_CONTINUE:
if(startTime == 0.0) {
	startTime = TIME;
}
/* Is there any communication initiated by R_FROM_PROC left? */
if(!--delay[R_FROM_PROC][pid(R_FROM_PROC, R_FROM_PID)].count) {
	if(delay[R_FROM_PROC]
	        [pid(R_FROM_PROC, R_FROM_PID)].doSleep) {
		delay[R_FROM_PROC]
		     [pid(R_FROM_PROC, R_FROM_PID)].doSleep = 0;
		/* No --> let it sleep */
		clearArrow();
		FILL_SLOT(R_FROM_PROC, pid(R_FROM_PROC, R_FROM_PID),
		          NOCARE, NOCARE, SUSPENDED);
		NEXT_FRAME();
	}
}
break;

  case MINDEX_PROC_RUN:
if(startTime == 0.0) {
	startTime = TIME;
}
if(PROCESSOR == 0) {
	flag[PROCESSOR] = 2;
}
lOMStuff[PROCESSOR].addTo = LOAD;
lOMStuff[PROCESSOR].since = (TIME - startTime);
drawTheLoadOMeters();
clearArrow();
FILL_SLOT(PROCESSOR, pid(PROCESSOR, PR_PID),
          NOCARE, NOCARE, ACTIVE);
NEXT_FRAME();
break;

  case MINDEX_PROC_SLEEP:
if(startTime == 0.0) {
	startTime = TIME;
}
lOMStuff[PROCESSOR].addTo = IDLE;
lOMStuff[PROCESSOR].load += ((TIME - startTime) -
       	                     lOMStuff[PROCESSOR].since);
drawTheLoadOMeters();
/* Is there any communication initiated by this process? */
if(delay[PROCESSOR][pid(PROCESSOR, PS_PID)].count) {
	/* Yes --> Delay and draw half slot */
	delay[PROCESSOR][pid(PROCESSOR, PS_PID)].doSleep = 1;
	clearArrow();
	FILL_SLOT(PROCESSOR, pid(PROCESSOR, PS_PID),
	          NOCARE, NOCARE, HALF_SUSPENDED);
	NEXT_FRAME();
} else {
	/* No --> Draw */
	clearArrow();
	FILL_SLOT(PROCESSOR, pid(PROCESSOR, PS_PID),
	          NOCARE, NOCARE, SUSPENDED);
	NEXT_FRAME();
}
break;

  case MINDEX_PROC_WAKEUP:
if(startTime == 0.0) {
	startTime = TIME;
}
clearArrow();
FILL_SLOT(PROCESSOR, pid(PROCESSOR, PW_PID),
          NOCARE, NOCARE, READY);
NEXT_FRAME();
break;

  case MINDEX_PROC_TERMINATE:
if(startTime == 0.0) {
	startTime = TIME;
}
lOMStuff[PROCESSOR].addTo = IDLE;
lOMStuff[PROCESSOR].load += ((TIME - startTime) -
       	                     lOMStuff[PROCESSOR].since);
drawTheLoadOMeters();
/* Is there any communication initiated by this process? */
clearArrow();
FILL_SLOT(PROCESSOR, pid(PROCESSOR, PT_PID),
          NOCARE, NOCARE, TERMINATED);
NEXT_FRAME();
break;

  case MINDEX_END:
clearArrow();
NEXT_FRAME();
break;
  }

  /*DBUG_VOID_RETURN*/

  return;
}


void iNeed(needed)
char needed[IDS_LEN][MAX_IDS]; {
  int i = 0;

  strcpy(needed[i++], "SEND_BEGIN");
  strcpy(needed[i++], "RECEIVE_BEGIN");
  strcpy(needed[i++], "SEND_END");
  strcpy(needed[i++], "PROC_CREATE");
  strcpy(needed[i++], "RECEIVE_END");
  strcpy(needed[i++], "CONTINUE");
  strcpy(needed[i++], "PROC_RUN");
  strcpy(needed[i++], "PROC_SLEEP");
  strcpy(needed[i++], "PROC_WAKEUP");
  strcpy(needed[i++], "PROC_TERMINATE");
  strcpy(needed[i++], "");

  return;
}