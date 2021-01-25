#include "msMain_main.h"


static char moduleCommands[MAX_COMMANDS][COMMANDS_LEN];

static struct FilterArray filterArray[MAX_IDS];
static struct InitArray initArray[MAX_IDS];
static short numIds;
static int pidArray[MAX_PIDS], pidCount = 0;

char selectedPath[FILE_LEN], selectedName[FILE_LEN], selectedFile[2*FILE_LEN];

extern char lastRequestAnswer[];


extern void dieHard();
extern int pipe();
extern int fork();
extern int close();
extern int dup();
extern int execl();
extern int wait3();
extern void doRequest();
extern Boolean XukcUnbindToggle();
extern Boolean XukcBindToggle();
extern Boolean XukcBindButton();
extern Boolean XukcUnbindForceDown();
extern Boolean XukcChangeButtonStatus();
extern void msFileBrowserInitialize();


/*****************************************************************************/
/*                                                                           */
/*  reaper: (local function)                                                 */
/*                                                                           */
/*  Avoid defunct-processes when forked process exits (from Netrek)...       */
/*                                                                           */
/*****************************************************************************/

static void reaper(sig)
int sig;
{
  while (wait3((union wait *) 0, WNOHANG, (struct rusage *) 0) > 0)
    ;
}


/*****************************************************************************/
/*                                                                           */
/*  modules2Med: (exported function)                                         */
/*                                                                           */
/*  Scan module-definition-file 'from' and write menu structure to 'to'...   */
/*                                                                           */
/*****************************************************************************/

void modules2Med(from, to, title)
char *from, *to, *title;
{
  FILE *f, *t;
  char label[MODULES_LEN], command[COMMANDS_LEN], error[ERROR_LEN];
  int num = 1, bottom = MODULES_HEIGHT, ret = 1;

  DBUG_ENTER("modules2Med");

  if((f = fopen(from, "r")) == NULL) {
    sprintf(error, "Can't open module file '%s' for read...", from);
    dieHard(error);
  }

  if((t = fopen(to, "w")) == NULL) {
    sprintf(error, "Can't open menu file '%s' for write...", to);
    dieHard(error);
  }

  while(!feof(f)) {
    strcpy(label, "");
    fscanf(f, "%s %s", label, command);

    if(strlen(label) && strlen(command) && label[0] != '#') {
      DBUG_PRINT("modules2Med", ("label=%s command=%s", label, command));

      fprintf(t, "static MENU MM%d = {\n",
	      num++);
      fprintf(t, "\t0x40,\n\t0,\n\t\042%s\042,\n\t%d,\n\t0,\n",
	      label, ret++);
      fprintf(t, "\t%d,\n\t%d,\n\t%d,\n\t0,\n\t0,\n\t0,\n",
	      bottom-MODULES_HEIGHT, MODULES_WIDTH, bottom);
      fprintf(t, "\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n");

      if(num > 2) {
	fprintf(t, "static MENU MM%d = {\n",
		num++);
	fprintf(t, "\t0x404,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
		MODULES_WIDTH);
	fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
		bottom);
	fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
		num-3, num-2);
      }
      
      bottom += MODULES_HEIGHT;
    } 

    /* skip rest of line */
    while((fgetc(f) != '\n') && !feof(f))
      ;
  }

  fprintf(t, "static MENU MM%d = {\n",
	  num++);
  fprintf(t, "\t0x2260,\n\t0,\n\t\042%s\042,\n\t0,\n\t0,\n\t0,\n", title);
  fprintf(t, "\t-1,\n\t-1,\n\t0,\n\t0,\n\t0,\n\tNULL,\n");
  fprintf(t, "\t&MM%d,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n",
	  num-2);  

  fprintf(t, "MENU menu_struct = {\n");
  fprintf(t, "\t0x400,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n");
  fprintf(t, "\t0,\n\t0,\n\t0,\n\t0,\n\t0,\n\tNULL,\n");
  fprintf(t, "\t&MM%d,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n",
	  num-1);  

  fclose(t);
  fclose(f);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  groups2Med: (exported function)                                          */
/*                                                                           */
/*  Scan groups-definition-file 'from' and write menu structure to 'to'...   */
/*                                                                           */
/*****************************************************************************/

void groups2Med(from, to, title)
char *from, *to, *title;
{
  FILE *f, *t;
  char id[IDS_LEN], temp[IDS_LEN+2], error[ERROR_LEN], c;
  int num = 1,
      bottom = IDS_HEIGHT,
      right = IDS_WIDTH,
      ret = 1,
      rememberLastColumn = 0,
      actualGroup = 0,
      i;
  Boolean expectGroup;

  DBUG_ENTER("groups2Med");

  if((f = fopen(from, "r")) == NULL) {
    sprintf(error, "Can't open groups file '%s' for read...", from);
    dieHard(error);
  }

  if((t = fopen(to, "w")) == NULL) {
    sprintf(error, "Can't open menu file '%s' for write...", to);
    dieHard(error);
  }

  /* Popdown-item */
  fprintf(t, "static MENU MM%d = {\n", num++);
  fprintf(t, "\t0x8040,\n\t0,\n\t\042  Pop it down...\042,\n\t%d,\n", ret++);
  fprintf(t, "\t%d,\n\t%d,\n\t%d,\n\t%d,\n\t0,\n\t0,\n\t0,\n",
	  right-IDS_WIDTH, bottom-IDS_HEIGHT, right, bottom);
  fprintf(t, "\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n");
  bottom += IDS_HEIGHT;

  /* All IDs! */
  fprintf(t, "static MENU MM%d = {\n", num++);
  fprintf(t, "\t0x8040,\n\t0,\n\t\042  All IDs!\042,\n\t%d,\n", ret++);
  fprintf(t, "\t%d,\n\t%d,\n\t%d,\n\t%d,\n\t0,\n\t0,\n\t0,\n",
	  right-IDS_WIDTH, bottom-IDS_HEIGHT, right, bottom);
  fprintf(t, "\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n");

  fprintf(t, "static MENU MM%d = {\n",
	  num++);
  fprintf(t, "\t0x404,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
	  IDS_WIDTH);
  fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
	  bottom);
  fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
	  num-3, num-2);
  bottom += IDS_HEIGHT;

  expectGroup = TRUE;

  while(!feof(f)) {
    if(expectGroup) {
      strcpy(id, "");
      fscanf(f, "%s", id);

      if(strlen(id) && (id[0] != '#')) {
	DBUG_PRINT("groups2Med", ("group=%s", id));

	++actualGroup;
	
	strcpy(temp, "G ");
	strcat(temp, id);

	fprintf(t, "static MENU MM%d = {\n",
		num++);
	fprintf(t, "\t0x8040,\n\t0,\n\t\042%s\042,\n\t%d,\n",
		temp, ret++);
	fprintf(t, "\t%d,\n\t%d,\n\t%d,\n\t%d,\n\t0,\n\t0,\n\t0,\n",
		right-IDS_WIDTH, bottom-IDS_HEIGHT, right, bottom);
	fprintf(t, "\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n");

	if(((ret-1) % IDS_ROWS) != 1) {
	  fprintf(t, "static MENU MM%d = {\n",
		  num++);
	  fprintf(t, "\t0x404,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
		  IDS_WIDTH);
	  fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
		  bottom);
	  fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
		  num-3, num-2);

	  if(((ret-1) % IDS_ROWS) == 0) {
	    if(rememberLastColumn) {
	      fprintf(t, "static MENU MM%d = {\n",
		      num++);
	      fprintf(t, "\t0x408,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
		      right);
	      fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
		      bottom);
	      fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
		      rememberLastColumn, num-2);
	    }
	    right += IDS_WIDTH;
	    bottom = 0;
	    rememberLastColumn = num-1;
	  }
	} 
	bottom += IDS_HEIGHT;
      } 

      /* skip rest of line */
      while((fgetc(f) != '\n') && !feof(f))
	;

      expectGroup = FALSE;
    } else {
      c = fgetc(f);
      if(!isLF(c)) {
        ungetc(c, f);
	
        if(!isBlank(c)) {
          expectGroup = TRUE;
        } else {
          strcpy(id, "");
          fscanf(f, "%s", id);

          if(strlen(id)) {
            DBUG_PRINT("groups2Med", ("\tid=%s", id));

	    strcpy(temp, "+ ");
	    strcat(temp, id);

	    fprintf(t, "static MENU MM%d = {\n",
		    num++);
	    fprintf(t, "\t0x8040,\n\t0,\n\t\042%s\042,\n\t%d,\n",
		    temp, ret++);
	    fprintf(t, "\t%d,\n\t%d,\n\t%d,\n\t%d,\n\t0,\n\t0,\n\t0,\n",
		    right-IDS_WIDTH, bottom-IDS_HEIGHT, right, bottom);
	    fprintf(t, "\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n");
	    
	    if(((ret-1) % IDS_ROWS) != 1) {
	      fprintf(t, "static MENU MM%d = {\n",
		      num++);
	      fprintf(t, "\t0x404,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
		      IDS_WIDTH);
	      fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
		      bottom);
	      fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
		      num-3, num-2);
	      
	      if(((ret-1) % IDS_ROWS) == 0) {
		if(rememberLastColumn) {
		  fprintf(t, "static MENU MM%d = {\n",
			  num++);
		  fprintf(t,
			  "\t0x408,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
			  right);
		  fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
			  bottom);
		  fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
			  rememberLastColumn, num-2);
		}
		right += IDS_WIDTH;
		bottom = 0;
		rememberLastColumn = num-1;
	      }
	    } 
	    bottom += IDS_HEIGHT;
          }

          /* skip rest of line */
          while((fgetc(f) != '\n') && !feof(f))
            ;
        }
      }
    }
  }

  if(!strcmp("filter.med", to)) {
    /* Filter for processors... */

    /* All Processors! */
    fprintf(t, "static MENU MM%d = {\n",
	    num++);
    fprintf(t, "\t0x8040,\n\t0,\n\t\042  All Processors!\042,\n\t%d,\n",
	    ret++);
    fprintf(t, "\t%d,\n\t%d,\n\t%d,\n\t%d,\n\t0,\n\t0,\n\t0,\n",
	    right-IDS_WIDTH, bottom-IDS_HEIGHT, right, bottom);
    fprintf(t, "\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n");
      
    if(((ret-1) % IDS_ROWS) != 1) {
      fprintf(t, "static MENU MM%d = {\n",
	      num++);
      fprintf(t, "\t0x404,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
	      IDS_WIDTH);
      fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
	      bottom);
      fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
	      num-3, num-2);
      
      if(((ret-1) % IDS_ROWS) == 0) {
	if(rememberLastColumn) {
	  fprintf(t, "static MENU MM%d = {\n",
		  num++);
	  fprintf(t,
		  "\t0x408,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
		  right);
	  fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
		  bottom);
	  fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
		  rememberLastColumn, num-2);
	}
	right += IDS_WIDTH;
	bottom = 0;
	rememberLastColumn = num-1;
      }
    } 
    bottom += IDS_HEIGHT;

    for(i=0; i<NUM_PROCESSORS; ++i) {
      sprintf(id, "Processor %d", i);

      DBUG_PRINT("groups2Med", ("\tid=%s", id));

      strcpy(temp, "+ ");
      strcat(temp, id);

      fprintf(t, "static MENU MM%d = {\n",
	      num++);
      fprintf(t, "\t0x8040,\n\t0,\n\t\042%s\042,\n\t%d,\n",
	      temp, ret++);
      fprintf(t, "\t%d,\n\t%d,\n\t%d,\n\t%d,\n\t0,\n\t0,\n\t0,\n",
	      right-IDS_WIDTH, bottom-IDS_HEIGHT, right, bottom);
      fprintf(t, "\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n");
      
      if(((ret-1) % IDS_ROWS) != 1) {
	fprintf(t, "static MENU MM%d = {\n",
		num++);
	fprintf(t, "\t0x404,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
		IDS_WIDTH);
	fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
		bottom);
	fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
		num-3, num-2);
	      
	if(((ret-1) % IDS_ROWS) == 0) {
	  if(rememberLastColumn) {
	    fprintf(t, "static MENU MM%d = {\n",
		    num++);
	    fprintf(t,
		    "\t0x408,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
		    right);
	    fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
		    bottom);
	    fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
		    rememberLastColumn, num-2);
	  }
	  right += IDS_WIDTH;
	  bottom = 0;
	  rememberLastColumn = num-1;
	}
      } 
      bottom += IDS_HEIGHT;
    }
  }

  if(rememberLastColumn && ((ret-1) % IDS_ROWS)) {
    /* Fill up the last column... */
    strcpy(id, "");
    while(((ret-1) % IDS_ROWS) != 0) {
      fprintf(t, "static MENU MM%d = {\n",
	      num++);
      fprintf(t, "\t0x8040,\n\t0,\n\t\042%s\042,\n\t%d,\n",
	      id, ret++);
      fprintf(t, "\t%d,\n\t%d,\n\t%d,\n\t%d,\n\t0,\n\t0,\n\t0,\n",
	      right-IDS_WIDTH, bottom-IDS_HEIGHT, right, bottom);
      fprintf(t, "\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n");

      fprintf(t, "static MENU MM%d = {\n",
	      num++);
      fprintf(t, "\t0x404,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
	      IDS_WIDTH);
      fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
	      bottom);
      fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
	      num-3, num-2);

      bottom += IDS_HEIGHT;
    }

    fprintf(t, "static MENU MM%d = {\n",
	    num++);
    fprintf(t, "\t0x408,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n\t%d,\n",
	    right);
    fprintf(t, "\t%d,\n\t0,\n\t0,\n\t0,\n\tNULL,\n",
	    bottom-IDS_HEIGHT);
    fprintf(t, "\t&MM%d,\n\t&MM%d,\n\tNULL,\n\tNULL,\n};\n",
	    rememberLastColumn, num-2);
  }

  fprintf(t, "static MENU MM%d = {\n",
	  num++);
  fprintf(t, "\t0x2240,\n\t0,\n\t\042%s\042,\n\t0,\n\t0,\n\t0,\n", title);
  fprintf(t, "\t-1,\n\t-1,\n\t0,\n\t0,\n\t0,\n\tNULL,\n");
  fprintf(t, "\t&MM%d,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n",
	  num-2);  

  fprintf(t, "MENU menu_struct = {\n");
  fprintf(t, "\t0x400,\n\t0,\n\tNULL,\n\t0,\n\t0,\n\t0,\n");
  fprintf(t, "\t0,\n\t0,\n\t0,\n\t0,\n\t0,\n\tNULL,\n");
  fprintf(t, "\t&MM%d,\n\tNULL,\n\tNULL,\n\tNULL,\n};\n",
	  num-1);  

  fclose(t);
  fclose(f);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msMainModuleCall: (callback)                                             */
/*                                                                           */
/*  Determine selected item in module menu and call the module...            */
/*                                                                           */
/*****************************************************************************/

void msMainModuleCall(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  FILE *f;
  int pid, pipefds[2], i;
  char command[COMMANDS_LEN];
  struct stat statBuffer;

  DBUG_ENTER("msMainModuleCall");

  if(strlen(selectedFile) && 
     !stat(selectedFile, &statBuffer) &&
     (S_ISREG(statBuffer.st_mode) || S_ISLNK(statBuffer.st_mode))) {

    strcpy(command, (char *)client_data);
    if(strlen(dbugOptions)) {
      strcat(command, " -#");
      strcat(command, dbugOptions);
    }

    DBUG_PRINT("msMainModuleCall", ("command='%s'", command));

    if(pipe(pipefds) < 0) {
      dieHard("Cannot create pipe to module...");
    }

    if((pid = fork()) < 0) {
      dieHard("Cannot fork module...");
    }

    if (pid == 0) {
        close(0);
        dup(pipefds[0]);
        close(pipefds[0]);

        close(pipefds[1]);

	/*system(command);*/

	if(strlen(dbugOptions)) {
	  sprintf(command, "-#%s", dbugOptions);
	  execl((char *)client_data, (char *)client_data, command, (char *)0);
	} else {
	  execl((char *)client_data, (char *)client_data, (char *)0);
	}

	exit(0);
    }
  
    close(pipefds[0]);

    f = fdopen(pipefds[1], "w");

    fprintf(f, "%s\n", selectedFile);

    if(pidCount < MAX_PIDS-1) {
      pidArray[pidCount++] = pid;
    } else {
      dieHard("Too many modules...");
    }

    /* Pass the filter information to the module... */
    for(i=3; i<=numIds; ++i) {
      if(filterArray[i].onString[0] == 'G') {
	fprintf(f, "%s: ", filterArray[i].onString+2);
      } else {
	if((filterArray[i].state) && (filterArray[i].enabled)) {
	  fprintf(f, "%d ", filterArray[i].mIndex);
	}
      }
    }

    fprintf(f, "\n");
    fclose(f);
  } else {
    DBUG_PRINT("msMainModuleCall", ("No file selected..."));

    doRequest("Please select a", "measurement file!", "", "OK", "", "", NULL);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msMainFilterCall: (callback)                                             */
/*                                                                           */
/*  Determine selected item in filter menu (de-)select item(s)...            */
/*                                                                           */
/*****************************************************************************/

void msMainFilterCall(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  short ret = *(short *)client_data,
        group = filterArray[ret].group,
        i;
  Boolean state = filterArray[ret].state;

  DBUG_ENTER("msMainFilterCall");

  if((ret > 2) && (ret != numIds-NUM_PROCESSORS)) {
    /* "Normal" IDs */

    DBUG_PRINT("msMainFilterCall", ("ret=%d group=%d toggle=%d",
				    ret, group, state));

    if(filterArray[ret].onString[0] == 'G') {
      /* Group */
      for(i=3; i<=numIds-NUM_PROCESSORS-1; ++i) {
	if((filterArray[i].group == group) && (filterArray[i].ret != ret)) {
	  filterArray[i].state = state;
	  UNBIND_TOGGLE(WI_ms_main_file_filter_menu, i);
	  BIND_TOGGLE(WI_ms_main_file_filter_menu, i,
		      filterArray[i].onString, filterArray[i].offString,
		      &(filterArray[i].state),
		      msMainFilterCall, &(filterArray[i].ret));
	}
      }
    }
  } else {
    /* "Special" IDs */
    if(ret == 2) {
      /* Turn on/off all GROUPS-IDs... */

      for(i=3; i<=numIds-NUM_PROCESSORS-1; ++i) {
	filterArray[i].state = state;
	UNBIND_TOGGLE(WI_ms_main_file_filter_menu, i);
	BIND_TOGGLE(WI_ms_main_file_filter_menu, i,
		    filterArray[i].onString, filterArray[i].offString,
		    &(filterArray[i].state),
		    msMainFilterCall, &(filterArray[i].ret));
      }
    } else {
      /* Turn on/off all processors... */

      for(i=numIds-NUM_PROCESSORS+1; i<=numIds; ++i) {
	filterArray[i].state = state;
	UNBIND_TOGGLE(WI_ms_main_file_filter_menu, i);
	BIND_TOGGLE(WI_ms_main_file_filter_menu, i,
		    filterArray[i].onString, filterArray[i].offString,
		    &(filterArray[i].state),
		    msMainFilterCall, &(filterArray[i].ret));
      }
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msMainInitCall: (callback)                                               */
/*                                                                           */
/*  Determine selected item in init menu (de-)select item(s)...              */
/*                                                                           */
/*****************************************************************************/

void msMainInitCall(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  short ret = *(short *)client_data,
        group = initArray[ret].group,
        i;
  Boolean state = initArray[ret].state;

  DBUG_ENTER("msMainInitCall");

  if(ret > 2) {
    /* "Normal" IDs */
    DBUG_PRINT("msMainInitCall", ("ret=%d group=%d toggle=%d",
				  ret, group, state));

    if(initArray[ret].onString[0] == 'G') {
      /* Group */

      for(i=3; i<=numIds; ++i) {
	if((initArray[i].group == group) && (initArray[i].ret != ret)) {
	  initArray[i].state = state;
	  UNBIND_TOGGLE(WI_ms_main_reduma_init_menu, i);

	  BIND_TOGGLE(WI_ms_main_reduma_init_menu, i,
		      initArray[i].onString, initArray[i].offString,
		      &(initArray[i].state),
		      msMainInitCall, &(initArray[i].ret));
	}
      }
    }
  } else {
    /* "Special" ID */
    /* Turn on/off all GROUPS-IDs... */

    for(i=3; i<=numIds-NUM_PROCESSORS-1; ++i) {
      initArray[i].state = state;
      UNBIND_TOGGLE(WI_ms_main_reduma_init_menu, i);
      BIND_TOGGLE(WI_ms_main_reduma_init_menu, i,
		  initArray[i].onString, initArray[i].offString,
		  &(initArray[i].state),
		  msMainInitCall, &(initArray[i].ret));
    }
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msMainRedumaCall: (callback)                                             */
/*                                                                           */
/*  Write init file 'INIT' and start reduma...                               */
/*                                                                           */
/*****************************************************************************/

void msMainRedumaCall(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  FILE *f;
  int i;
  char error[ERROR_LEN];

  DBUG_ENTER("msMainRedumaCall");

  if((f = fopen("INIT", "w")) == NULL) {
    sprintf(error, "Can't open init file 'INIT' for write...");
    dieHard(error);
  }

  /* Write the selected IDs... */
  for(i=3; i<=numIds; ++i) {
    if((initArray[i].onString[0] != 'G') && (initArray[i].state)) {
      fprintf(f, "%s\n", initArray[i].onString+2);
    }
  }

  fclose(f);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  bindModulesToMenu: (local function)                                      */
/*                                                                           */
/*  Attach callback to menu items...                                         */
/*                                                                           */
/*****************************************************************************/

static void bindModulesToMenu()
{
  FILE *f;
  char label[MODULES_LEN], command[COMMANDS_LEN], error[ERROR_LEN];
  short n = 1;

  DBUG_ENTER("bindModulesToMenu");

  /* Scan 'MODULES' and bind menu callbacks */
  DBUG_PRINT("bindModulesToMenu", ("Scanning 'MODULES'..."));

  if((f = fopen("MODULES", "r")) == NULL) {
    sprintf(error, "Can't open module file 'MODULES' for read...");
    dieHard(error);
  }
    
  while(!feof(f)) {
    strcpy(label, "");
    fscanf(f, "%s %s", label, command);
    if(strlen(label) && strlen(command) && label[0] != '#') {
      DBUG_PRINT("bindModulesToMenu",
		 ("label=%s command=%s", label, command));
      
      strcpy(moduleCommands[n], command);
      BIND_ITEM(WI_ms_main_module_menu, n, msMainModuleCall,
		&(moduleCommands[n]));

      ++n;
    }

    /* skip rest of line */
    while((fgetc(f) != '\n') && !feof(f))
      ;
    }

  DBUG_PRINT("bindModulesToMenu", ("Scanned 'MODULES'..."));
  fclose(f);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  prepareIdMenus: (local function)                                         */
/*                                                                           */
/*  Make the items non-down-forcing and disable them...                      */
/*                                                                           */
/*****************************************************************************/

void prepareIdMenus()
{
  FILE *f;
  char id[IDS_LEN], error[ERROR_LEN], c;
  short ret = 1,
        actualGroup = 0,
        mIndex = 1;
  int i;
  Boolean expectGroup;

  DBUG_ENTER("prepareIdMenus");

  if((f = fopen("GROUPS", "r")) == NULL) {
    sprintf(error, "Can't open groups file 'GROUPS' for read...");
    dieHard(error);
  }

  numIds = 1;

  /* All IDs! */
  ++numIds;
  ++ret;

  filterArray[ret].ret = ret;
  filterArray[ret].state = TRUE;
  strcpy(filterArray[ret].onString, "  All IDs!");
  strcpy(filterArray[ret].offString, filterArray[ret].onString);

  BIND_TOGGLE(WI_ms_main_file_filter_menu, ret,
	      filterArray[ret].onString, filterArray[ret].offString,
	      &(filterArray[ret].state),
	      msMainFilterCall, &(filterArray[ret].ret));
  UNBIND_FORCE_DOWN(WI_ms_main_file_filter_menu, ret);

  initArray[ret].ret = ret;
  initArray[ret].state = TRUE;
  strcpy(initArray[ret].onString, "  All IDs!");
  strcpy(initArray[ret].offString, initArray[ret].onString);

  BIND_TOGGLE(WI_ms_main_reduma_init_menu, ret,
	      initArray[ret].onString, initArray[ret].offString,
	      &(initArray[ret].state),
	      msMainInitCall, &(initArray[ret].ret));
  UNBIND_FORCE_DOWN(WI_ms_main_reduma_init_menu, ret);


  expectGroup = TRUE;

  while(!feof(f)) {
    if(expectGroup) {
      strcpy(id, "");
      fscanf(f, "%s", id);

      if(strlen(id) && (id[0] != '#')) {
	DBUG_PRINT("prepareIdMenus", ("group=%s", id));

	++ret;
	++numIds;
	++actualGroup;

	filterArray[ret].ret = ret;
	filterArray[ret].group = actualGroup;
	filterArray[ret].mIndex = 0;
	filterArray[ret].state = TRUE;
	filterArray[ret].enabled = FALSE;
	strcpy(filterArray[ret].onString, "G ");
	strcat(filterArray[ret].onString, id);
	strcpy(filterArray[ret].offString, filterArray[ret].onString);

	BIND_TOGGLE(WI_ms_main_file_filter_menu, ret,
		    filterArray[ret].onString, filterArray[ret].offString,
		    &(filterArray[ret].state),
		    msMainFilterCall, &(filterArray[ret].ret));
	UNBIND_FORCE_DOWN(WI_ms_main_file_filter_menu, ret);
	TOGGLE_DISABLE(WI_ms_main_file_filter_menu, ret);

	initArray[ret].ret = ret;
	initArray[ret].group = actualGroup;
	initArray[ret].state = TRUE;
	strcpy(initArray[ret].onString, "G ");
	strcat(initArray[ret].onString, id);
	strcpy(initArray[ret].offString, initArray[ret].onString);

	BIND_TOGGLE(WI_ms_main_reduma_init_menu, ret,
		    initArray[ret].onString, initArray[ret].offString,
		    &(initArray[ret].state),
		    msMainInitCall, &(initArray[ret].ret));
	UNBIND_FORCE_DOWN(WI_ms_main_reduma_init_menu, ret);
      }
	
      while((fgetc(f) != '\n') && !feof(f))
	;

      expectGroup = FALSE;
    } else {
      c = fgetc(f);
      if(!isLF(c)) {
        ungetc(c, f);
	
        if(!isBlank(c)) {
          expectGroup = TRUE;
        } else {
          strcpy(id, "");
          fscanf(f, "%s", id);

          if(strlen(id)) {
            DBUG_PRINT("prepareIdMenus", ("\tid=%s", id));

	    ++ret;
	    ++numIds;

	    filterArray[ret].ret = ret;
	    filterArray[ret].group = actualGroup;
	    filterArray[ret].mIndex = mIndex;
	    filterArray[ret].state = TRUE;
	    filterArray[ret].enabled = FALSE;
	    strcpy(filterArray[ret].onString, "+ ");
	    strcat(filterArray[ret].onString, id);
	    strcpy(filterArray[ret].offString, "- ");
	    strcat(filterArray[ret].offString, id);

	    ++mIndex;

	    BIND_TOGGLE(WI_ms_main_file_filter_menu, ret,
			filterArray[ret].onString, filterArray[ret].offString,
			&(filterArray[ret].state),
			msMainFilterCall, &(filterArray[ret].ret));
	    UNBIND_FORCE_DOWN(WI_ms_main_file_filter_menu, ret);
	    TOGGLE_DISABLE(WI_ms_main_file_filter_menu, ret);

	    initArray[ret].ret = ret;
	    initArray[ret].group = actualGroup;
	    initArray[ret].state = TRUE;
	    strcpy(initArray[ret].onString, "+ ");
	    strcat(initArray[ret].onString, id);
	    strcpy(initArray[ret].offString, "- ");
	    strcat(initArray[ret].offString, id);

	    BIND_TOGGLE(WI_ms_main_reduma_init_menu, ret,
			initArray[ret].onString, initArray[ret].offString,
			&(initArray[ret].state),
			msMainInitCall, &(initArray[ret].ret));
	    UNBIND_FORCE_DOWN(WI_ms_main_reduma_init_menu, ret);
          }

          /* skip rest of line */
          while((fgetc(f) != '\n') && !feof(f))
            ;
        }
      }
    }
  }

  fclose(f);

  /* Processor entries... */

  ++numIds;
  ++ret;

  /* All Processors! */
  filterArray[ret].ret = ret;
  filterArray[ret].state = TRUE;
  strcpy(filterArray[ret].onString, "  All Processors!");
  strcpy(filterArray[ret].offString, "  All Processors!");

  BIND_TOGGLE(WI_ms_main_file_filter_menu, ret,
	      filterArray[ret].onString, filterArray[ret].offString,
	      &(filterArray[ret].state),
	      msMainFilterCall, &(filterArray[ret].ret));
  UNBIND_FORCE_DOWN(WI_ms_main_file_filter_menu, ret);


  for(i=0; i<NUM_PROCESSORS; ++i) {
    sprintf(id, "Processor %d", i);

    DBUG_PRINT("prepareIdMenus", ("\tid=%s", id));

    ++ret;
    ++numIds;

    filterArray[ret].ret = ret;
    filterArray[ret].group = actualGroup;
    filterArray[ret].mIndex = mIndex;
    filterArray[ret].state = TRUE;
    filterArray[ret].enabled = TRUE;
    strcpy(filterArray[ret].onString, "+ ");
    strcat(filterArray[ret].onString, id);
    strcpy(filterArray[ret].offString, "- ");
    strcat(filterArray[ret].offString, id);

    ++mIndex;

    BIND_TOGGLE(WI_ms_main_file_filter_menu, ret,
		filterArray[ret].onString, filterArray[ret].offString,
		&(filterArray[ret].state),
		msMainFilterCall, &(filterArray[ret].ret));
    UNBIND_FORCE_DOWN(WI_ms_main_file_filter_menu, ret);
    TOGGLE_ENABLE(WI_ms_main_file_filter_menu, ret);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  fileSelectedHook: (exported function)                                    */
/*                                                                           */
/*  Examine file after selection... (called my 'msFileBrowserSelectFile')    */
/*                                                                           */
/*****************************************************************************/

void fileSelectedHook(which, path, file)
short which;
char *path, *file;
{
  FILE *f;
  char id[IDS_LEN], error[ERROR_LEN], header[DATA_LEN];
  short expect = HEADER, i, group;
  struct stat statBuffer;

  DBUG_ENTER("fileSelectedHook");

  /* Display new path and name... */
  SET_LABEL(WI_ms_main_file_enter_name, file);
  SET_LABEL(WI_ms_main_file_enter_path, path);

  strcpy(selectedFile, path);
  strcat(selectedFile, file);
  DBUG_PRINT("fileSelectedHook", ("selected: %s", selectedFile));

  if(strlen(selectedFile) && 
     !stat(selectedFile, &statBuffer) &&
     (S_ISREG(statBuffer.st_mode) || S_ISLNK(statBuffer.st_mode))) {

    /* Disable all (but processor) items in filter menu... */
    for(i=3; i<=numIds-NUM_PROCESSORS-1; ++i) {
      TOGGLE_DISABLE(WI_ms_main_file_filter_menu, i);
    }

    /* Scan the 'INIT'-part of the selected file... */
    if((f = fopen(selectedFile, "r")) == NULL) {
      sprintf(error, "Can't open data file '%s' for read...", selectedFile);
      strcpy(selectedFile, "");
    } else {
      while(!feof(f)) {
	switch(expect) {

	case HEADER:
	  fscanf(f, "%s", header);
	  
	  if(!strcmp(header, "---")) {
	    expect = INIT;
	  }
	  break;

	case INIT:
	  fscanf(f, "%s", id);
	  
	  if(!strcmp(id, "---")) {
	    expect = TICKET;
	  } else {
	    DBUG_PRINT("fileSelectedHook", ("id=%s", id));
	    
	    /* Enable item(s)... */
	    for(i=3; i<=numIds; ++i) {
	      if(!strcmp(id, filterArray[i].onString+2)) {
		TOGGLE_ENABLE(WI_ms_main_file_filter_menu, i);
		filterArray[i].enabled = TRUE;
		break;
	      }
	    }
	    
	    /* Group? */
	    if(filterArray[i].onString[0] == 'G') {
	      /* Group --> enable all items of this group... */
	      group = filterArray[i].group;
	      for(i=3; i<=numIds; ++i) {
		if(group == filterArray[i].group) {
		  TOGGLE_ENABLE(WI_ms_main_file_filter_menu, i);
		  filterArray[i].enabled = TRUE;
		}
	      }
	    } else {
	      /* No group --> enable group... */
	      group = filterArray[i].group;
	      for(i=3; i<=numIds; ++i) {
		if((group == filterArray[i].group) &&
		   (filterArray[i].onString[0] == 'G')) {
		  TOGGLE_ENABLE(WI_ms_main_file_filter_menu, i);
		  filterArray[i].enabled = TRUE;
		  break;
		}
	      }
	    }
	  }
	  break;
	  
	case TICKET:
	  fscanf(f, "%s", header);
	  
	  if(!strcmp(header, "---")) {
	    expect = DATA;
	  }
	  break;
	  
	case DATA:
	  break;
	}
	
	if(expect == DATA) {
	  break;
	}
      }
      fclose(f);
    }
  } else {
    DBUG_PRINT("fileSelectedHook", ("No file selected..."));

    doRequest("", "No measurement file selected!", "", "OK", "", "", NULL);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  fetchPreferences: (local function)                                       */
/*                                                                           */
/*  Get initial settings...                                                  */
/*                                                                           */
/*****************************************************************************/

void fetchPreferences()
{
  FILE *f;
  char path[FILE_LEN], name[FILE_LEN], error[ERROR_LEN];
  int storeOnQuit;

  DBUG_ENTER("fetchPreferences");

  if((f = fopen("ms.prefs", "r")) == NULL) {
    sprintf(error, "Can't open preferences file 'ms.prefs' for read...");
    dieHard(error);
  }

  fgets(path, FILE_LEN, f);
  path[strlen(path)-1] = '\0';
  fgets(name, FILE_LEN, f);
  name[strlen(name)-1] = '\0';
  fscanf(f, "%d\n", &storeOnQuit);
    
  fclose(f);

  fileSelectedHook(0, path, name);
  if(storeOnQuit) {
    SET_TOGGLE(WI_ms_main_preferences_toggle, FALSE);
  } else {
    UNSET_TOGGLE(WI_ms_main_preferences_toggle, FALSE);
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msMainStorePreferences: (callback)                                       */
/*                                                                           */
/*  Store preferences now...                                                 */
/*                                                                           */
/*****************************************************************************/

void msMainStorePreferences(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], name[FILE_LEN], error[ERROR_LEN];
  Boolean storeOnQuit;
  FILE *f;

  DBUG_ENTER("msMainStorePreferences");

  GET_LABEL(WI_ms_main_file_enter_name, name);
  GET_LABEL(WI_ms_main_file_enter_path, path);

  GET_TOGGLE_STATE(WI_ms_main_preferences_toggle, &storeOnQuit);

  if((f = fopen("ms.prefs", "w")) == NULL) {
    sprintf(error, "Can't open preferences file 'ms.prefs' for write...");
    dieHard(error);
  }

  fprintf(f, "%s\n", path);
  fprintf(f, "%s\n", name);
  fprintf(f, "%d\n", (storeOnQuit ? 1 : 0));
    
  fclose(f);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  startupHook: (exported function)                                         */
/*                                                                           */
/*  Do some initialization... (called my 'main')                             */
/*                                                                           */
/*****************************************************************************/

void startupHook()
{
  DBUG_ENTER("startupHook");

  strcpy(selectedFile, "");

  /* Netrek trick... */
  (void)signal(SIGCHLD, reaper);

  /* Initialize menu-callbacks... */
  bindModulesToMenu();
  prepareIdMenus();

  /* Get preferences... */
  fetchPreferences();

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msMainExitHook: (callback)                                               */
/*                                                                           */
/*  Keep your environment tidy...                                            */
/*                                                                           */
/*****************************************************************************/

void msMainExitHook(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  Boolean storePreferences;
  int i;

  DBUG_ENTER("msMainExitHook");

  if(!strlen(lastRequestAnswer)) {
    doRequest("Do you really want to quit?",
	      "(All activated modules will be killed!)", "", 
	      "Yes", "No", "", &msMainExitHook);
  } else {
    if(!strcmp(lastRequestAnswer, "Yes")) {
      GET_TOGGLE_STATE(WI_ms_main_preferences_toggle, &storePreferences);

      if(storePreferences) {
	msMainStorePreferences();
      }

      for(i=0; i<pidCount; ++i) {
	DBUG_PRINT("msMainExitHook", ("Killing %d...", pidArray[i]));
	kill(pidArray[i], SIGKILL);
      }

      exit(0);
    }

    strcpy(lastRequestAnswer, "");
  }
}


/*****************************************************************************/
/*                                                                           */
/*  msMainFileBrowse: (callback)                                             */
/*                                                                           */
/*  Initialize the browser and bring it up...                                */
/*                                                                           */
/*****************************************************************************/

void msMainFileBrowse(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], file[FILE_LEN];

  DBUG_ENTER("msMainFileBrowse");

  GET_LABEL(WI_ms_main_file_enter_path, path);
  GET_LABEL(WI_ms_main_file_enter_name, file);

  msFileBrowserInitialize(0, "Load measure file...", FILE_MASK, path, file);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msMainFileCall: (callback)                                               */
/*                                                                           */
/*  Get path and name from fields and pass them to 'fileSelectedHook'...     */
/*                                                                           */
/*****************************************************************************/

void msMainFileCall(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], name[FILE_LEN];

  DBUG_ENTER("msMainFileCall");

  GET_LABEL(WI_ms_main_file_enter_name, name);
  GET_LABEL(WI_ms_main_file_enter_path, path);

  /* Make the selected file known to others... */
  fileSelectedHook(0, path, name);

  DBUG_VOID_RETURN;
}
