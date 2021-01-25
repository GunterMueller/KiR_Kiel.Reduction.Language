int (*mscanfTime)();
int (*mscanfProcessor)();
int (*mscanfProcess)();
int (*mscanfId)();
int (*mscanfInt)();
int (*mscanfString)();
int (*mscanfLength)();

int (*mprintfTime)();
int (*mprintfProcessor)();
int (*mprintfProcess)();
int (*mprintfId)();
int (*mprintfInt)();
int (*mprintfString)();
int (*mprintfDelimiter)();
int (*mprintfNewLine)();
int (*mprintfLength)();

void (*eval)();


#define Boolean short
#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <dlfcn.h>
#include <varargs.h>


FILE *inF = stdin, *outF = stdout;


#include "main.h"

#include "msMacros.h"
#include "scriptMacros.h"
#include "dbug.h"
#include "groups.h"
#include "id2index.c"
#include "convert.c"
#include "trevnoc.c"
#include "vars.h"
#include "ioFunctions.c"


Boolean binaryIn, binaryOut, limit, filterId[MAX_IDS];


void doConversion();


void doNothing()
{
  return;
}


void readINIT()
{
  FILE *init;
  char temp[IDS_LEN];
  int i;

  DBUG_ENTER("readINIT");

  for(i=0; i<MAX_IDS; ++i) {
    filterId[i] = TRUE;
  }

  if((init = fopen("INIT", "r")) == NULL) {
    puts("Can't open 'INIT'...");
    exit(1);
  }

  while(!feof(init)) {
    fscanf(init, "%s", temp);
    filterId[id2Index(temp)] = FALSE;
  }

  fclose(init);

  DBUG_VOID_RETURN;
}


void MAIN
{
  binaryIn = TRUE;
  binaryOut = FALSE;
  limit = FALSE;

  OPT
  ARG '#': /* Turn on Fred Fish's debugging routines... */
    PARM
    DBUG_PUSH(*argv);
    NEXTOPT
  ARG 'i': /* Specify in-format... */
    PARM
    binaryIn = (((char *)*argv)[0] == 'b');
    NEXTOPT
  ARG 'o': /* Specify out-format... */
    PARM
    binaryOut = (((char *)*argv)[0] == 'b');
    NEXTOPT
  ARG 'l':
    limit = TRUE;
  ARG '?':
  ARG 'h': /* help the DAU */
    printf("Options:\n");
    printf("-#: Turn on Fred Fish's debugging routines.\n");
    printf("    (with parameters following the '-#')\n");
    printf("-i: Specify in-format (-ia for ascii, -ib for binary [default])\n");
    printf("-o: Specify out-format (-oa for ascii [default], -ob for binary)\n");
    printf("-l: Limit output to measure lines with ids in INIT-file\n");
    exit(0);
  OTHER
    printf("Unknown option on command line: %c\n",**argv);
    printf("Use option '-h' for list of valid options...\n");
  ENDOPT

  if(binaryIn) {
    mscanfTime = &bscanfTime;
    mscanfProcessor = &bscanfProcessor;
    mscanfProcess = &bscanfProcess;
    mscanfId = &bscanfId;
    mscanfInt = &bscanfInt;
    mscanfString = &bscanfString;
    mscanfLength = &bscanfLength;
  } else {
    mscanfTime = &ascanfTime;
    mscanfProcessor = &ascanfProcessor;
    mscanfProcess = &ascanfProcess;
    mscanfId = &ascanfId;
    mscanfInt = &ascanfInt;
    mscanfString = &ascanfString;
    mscanfLength = &ascanfLength;
  }

  if(binaryOut) {
    mprintfTime = &bprintfTime;
    mprintfProcessor = &bprintfProcessor;
    mprintfProcess = &bprintfProcess;
    mprintfId = &bprintfId;
    mprintfInt = &bprintfInt;
    mprintfString = &bprintfString;
    mprintfDelimiter = &bprintfDelimiter;
    mprintfNewLine = &bprintfNewLine;
    mprintfLength = &bprintfLength;
  } else {
    mprintfTime = &aprintfTime;
    mprintfProcessor = &aprintfProcessor;
    mprintfProcess = &aprintfProcess;
    mprintfId = &aprintfId;
    mprintfInt = &aprintfInt;
    mprintfString = &aprintfString;
    mprintfDelimiter = &aprintfDelimiter;
    mprintfNewLine = &aprintfNewLine;
    mprintfLength = &aprintfLength;
  }

  if(limit) {
    readINIT();
  }

  doConversion();

  return;
}


void doConversion()
{
  short expect = HEADER;
  char line[DATA_LEN], temp[IDS_LEN];
  int TEMP;
  short LENGTH, length;
  long rememberPosition;
  double rememberTime;


  DBUG_ENTER("doConversion");

  DBUG_PRINT("doConversion", 
	     ("Formats: binaryIn=%d binaryOut=%d", binaryIn, binaryOut));

  while(!feof(inF)) {
    switch(expect) {
    case HEADER:
      gets(line);

      if(sscanf(line, "binary: %d", &TEMP)) {
	printf("binary: %d\n", binaryOut);
      } else {
	puts(line);
      }

      if(!strcmp(line, "---")) {
	expect = INIT;
      }
      break;

    case INIT:
      gets(line);
      
      sscanf(line, "%s", temp);
      if(limit) {
	if(!filterId[id2Index(temp)] || !strcmp(line, "---")) {
	  puts(line);
	}
      } else {
	puts(line);
      }

      if(!strcmp(line, "---")) {
	expect = TICKET;
      }
      break;

    case TICKET:
      gets(line);
      puts(line);
      if(!strcmp(line, "---")) {
	expect = DATA;
      }
      break;

    case DATA:
      length = MSCAN(inF, &TIME, &PROCESSOR, &PROCESS, &ID);
      rememberTime = TIME;
      if(feof(inF)) {
	break;
      }
      DBUG_PRINT("doConversion",
		 ("%l.0f %d %d %d\n", TIME, PROCESSOR, PROCESS, ID));

      rememberPosition = ftell(outF);
      if(ID) {
	if(limit && filterId[ID]) {
	  eval = &doNothing;
	} else {
	  length = MPRINT(outF, TIME, PROCESSOR, PROCESS, ID);
	  eval = &trevnoc;
	}

	length = convert(inF) + 14;                    /* 14 = 8 + 1 + 4 + 1 */
	MSCAN_LENGTH(inF, &LENGTH);

	if(binaryIn && (length != LENGTH)) {
	  fprintf(stderr,
		  "Error at timestamp %.0f: Found length %d (should be %d)!\n",
		  TIME, LENGTH, length);
	  exit(99);
	}

	if(!(limit && filterId[ID])) {
	  MPRINT_LENGTH(outF, LENGTH);
	}
      } else {
	/* SPECIAL */
	mprintfTime(outF, TIME);
	mprintfDelimiter(outF);
	mprintfProcessor(outF, PROCESSOR);
	mprintfDelimiter(outF);
	mprintfProcess(outF, PROCESS);
	mprintfDelimiter(outF);
	if(binaryOut) {
	  mprintfId(outF, ID);
	} else {
	  printf("SPECIAL");
	}
	mprintfDelimiter(outF);
	mscanfInt(inF, &TEMP);
	mprintfInt(outF, TEMP);
	mprintfDelimiter(outF);
	mscanfInt(inF, &TEMP);
	mprintfInt(outF, TEMP);
	mscanfLength(inF, &LENGTH);
	mprintfLength(outF, (short)22);
	mprintfNewLine(outF);
      }
      break;
    }
  }

  DBUG_VOID_RETURN;
}
