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

static char rememberFile[100], rememberTime[100], rememberSteps[100],
            rememberHeapsize[100], rememberHeapdes[100], temp[1000];
static int heapsize, heapdes;
#define DESC_SIZE 16
#define NUM_PROCESSORS 32
#define NUM_FRAGS 500
#define NUM_BLOCKS 10000
static int i, j, k, heapBases[NUM_PROCESSORS];
static struct {
	int size, address, reference;
} blockSizes[NUM_PROCESSORS][NUM_BLOCKS];
static int numBlockSizes[NUM_PROCESSORS];
int blockSize(processor, reference)
int processor, reference;
{
	int i;
	for(i=0; i<numBlockSizes[processor]; ++i) {
		if(blockSizes[processor][i].reference == reference) {
			return(i);
		}
	}
	return(-1);
}
int computeSize(s)
int s;
{
	return((int)(NUM_FRAGS * (double)s/(double)heapsize) + 1);
}
int computeFrag(p, a)          /* Compute fragment from heap address */
int p, a;
{
	return((int)((((double)(a-heapBases[p]))/
	              (double)heapsize)*
	             NUM_FRAGS));
}
void fill(processor, address, size, color)
int processor, address, size, color;
{
	int i, j, k;
	j = computeFrag(processor, address);
	k = computeSize(size);
	for(i=j; i < j+k; ++i) {
		FILL_BLOCK(processor, i, color);
	}
	DRAW_FRAGMENTATION(processor, j);
	return;
}


void eval()
{
  /*DBUG_ENTER("eval");*/

  switch(ID) {

  case MINDEX_BEGIN:
/* Colors for "Heapblock", "Descriptor" and "Available" */
SET_COLORS(LIGHT_RED, LIGHT_YELLOW, GREEN);
HEADER_FILE(rememberFile);
HEADER_TIME(rememberTime);
HEADER_STEPS(rememberSteps);
sprintf(temp, "%s (%s) -- %s reduction steps",
        rememberFile, rememberTime, rememberSteps);
SET_TITLE(temp);
HEADER_HEAPSIZE(rememberHeapsize);
heapsize = atoi(rememberHeapsize);
HEADER_HEAPDES(rememberHeapdes);
heapdes = atoi(rememberHeapdes);
SET_DIMENSIONS(NUM_PROCESSORS, NUM_FRAGS);
SET_SEPARATION(ON);
FILL_ALL(FREE);    /* Initialize "memory area"; Only allowed here!!! */
for(i=0; i<NUM_PROCESSORS; ++i) {
	heapBases[i] = GET_HEAPBASE(i);
	numBlockSizes[i] = 0;
}
NEXT_FRAME();
break;

  case MINDEX_DESC_ALLOC:
fill(PROCESSOR, DA_ADR, DESC_SIZE, DESCRIPTOR);
NEXT_FRAME();
break;

  case MINDEX_DESC_FREE:
fill(PROCESSOR, DF_ADR, DESC_SIZE, FREE);
NEXT_FRAME();
break;

  case MINDEX_HEAP_ALLOC:
if((j = blockSize(PROCESSOR, HA_REF)) == -1) {
	blockSizes[PROCESSOR]
	          [numBlockSizes[PROCESSOR]].size = HA_SIZE;
	blockSizes[PROCESSOR]
	          [numBlockSizes[PROCESSOR]].address = HA_ADR;
	blockSizes[PROCESSOR]
	          [numBlockSizes[PROCESSOR]].reference = HA_REF;
	++numBlockSizes[PROCESSOR];
} else {
	blockSizes[PROCESSOR][j].size = HA_SIZE;
	blockSizes[PROCESSOR][j].address = HA_ADR;
}
fill(PROCESSOR, HA_ADR, HA_SIZE, HEAPBLOCK);
NEXT_FRAME();
break;

  case MINDEX_HEAP_FREE:
if((i = blockSize(PROCESSOR, HF_REF)) != -1) {
	fill(PROCESSOR, 
	     blockSizes[PROCESSOR][i].address /* != HF_ADR !!! */, 
	     blockSizes[PROCESSOR][i].size, FREE);
} else {
	printf("!!! --> %lf\n", TIME);
}
NEXT_FRAME();
break;

  case MINDEX_END:
NEXT_FRAME();
break;
  }

  /*DBUG_VOID_RETURN*/

  return;
}


void iNeed(needed)
char needed[IDS_LEN][MAX_IDS]; {
  int i = 0;

  strcpy(needed[i++], "DESC_ALLOC");
  strcpy(needed[i++], "DESC_FREE");
  strcpy(needed[i++], "HEAP_ALLOC");
  strcpy(needed[i++], "HEAP_FREE");
  strcpy(needed[i++], "");

  return;
}
