#include "msFileBrowser.h"


static Cardinal listId = 0;
Boolean illegalPath;

static short actualWhich;                      /* Who opened actual browser? */
static Display *display;


extern char *getlogin();
extern int getuid();
extern pclose();
extern char *index();
extern char *rindex();
extern void XukcDestroyList();
extern Cardinal XukcAddObjectToList();
extern XtPointer *XukcGetListPointer();


/*****************************************************************************/
/*                                                                           */
/*  expandTilde: (local function)                                            */
/*                                                                           */
/*  Get path in "in". Expand it ("~") and return new path in "out"...        */
/*                                                                           */
/*****************************************************************************/

static void expandTilde(in, out)
char *in, *out;
{
  DBUG_ENTER("expandTilde");

  DBUG_PRINT("expandTilde", ("old=%s", in));

  *out = '\0';
  if (in) {
    if (*in == '~') {
      char user[128];
      struct passwd *pw = NULL;
      int i = 0;
      
      user[0] = '\0';
      in++;
      while (*in != '/' && *in != '\0' && i < sizeof(user))
        user[i++] = *in++;
      user[i] = '\0';
      if (i == 0) {
        char *login = (char *)getlogin();
	
        if (login == NULL && (pw = getpwuid(getuid())) == NULL)
	  DBUG_VOID_RETURN;
	if (login != NULL)
	  strcpy(user, login);
      }
      if (pw == NULL && (pw = getpwnam(user)) == NULL)
        DBUG_VOID_RETURN;
      strcpy(out, pw->pw_dir);
    }
    strcat(out, in);

    DBUG_PRINT("expandTilde", ("new=%s", out));
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  makeAbsolute: (local function)                                           */
/*                                                                           */
/*  Get path in "in". Get the absolute path and return it in "out"...        */
/*                                                                           */
/*****************************************************************************/

static void makeAbsolute(in, out)
char *in, *out;
{
  char temp[FILE_LEN], command[FILE_LEN], error[ERROR_LEN];
  FILE *f;

  DBUG_ENTER("makeAbsolute");

  DBUG_PRINT("makeAbsolute", ("old=%s", in));

  strcpy(temp, "");
  expandTilde(in, temp);

  sprintf(command, "cd %s ; pwd", temp);
  if((f = popen(command, "r")) == NULL) {
    sprintf(error, "Can't create pipe for 'pwd'...");
    dieHard(error);
  }

  if(fscanf(f, "%s\n", out) == EOF) {
    strcpy(out, temp);
  }
  pclose(f);

  /* append '/' */
  if(strlen(out)) {
    if(out[strlen(out)-1] != '/') {
      strcat(out, "/");
    }
  } else {
    strcat(out, "/");
  }

  DBUG_PRINT("makeAbsolute", ("new=%s", out));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFileBrowserUpdate: (callback)                                          */
/*                                                                           */
/*  Update contents of scroll-list after changing mask or path...            */
/*                                                                           */
/*****************************************************************************/

void msFileBrowserUpdate(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char buffer[FILE_LEN], path[FILE_LEN], mask[FILE_LEN],
       error[ERROR_LEN], s[FILE_LEN];
  String *t;
  Cardinal i, k;
  FILE *f;
  struct stat statBuffer;

  DBUG_ENTER("msFileBrowserUpdate");

  /* Get mask and path from input-fields... */
  GET_LABEL(WI_my_requester_select_enter_path, s);
  if(!strlen(s)) {
    makeAbsolute(".", path);
  } else {
    makeAbsolute(s, path);
  }
  SET_LABEL(WI_my_requester_select_enter_path, path);

  GET_LABEL(WI_my_requester_select_enter_mask, s);
  if(index(s, '/')) {
    /* ..... */

    SET_LABEL(WI_my_requester_select_enter_mask, mask);
  }
  strcpy(mask, (char *)s);
  if(!strlen(mask)) {
    strcpy(mask, "*");
    SET_LABEL(WI_my_requester_select_enter_mask, mask);
  }

  DBUG_PRINT("msFileBrowserUpdate", ("mask=%s path=%s", mask, path));

  UNSELECT_ITEMS(WI_my_requester_select_scroll);

  /* Destroy old list... */
  DBUG_PRINT("msFileBrowserUpdate",
	     ("Destroying old list... (listId=%d)", listId));
  DESTROY_LIST(&listId);
  DBUG_PRINT("msFileBrowserUpdate",
	     ("Destroyed old list... (listId=%d)", listId));

  DBUG_PRINT("msFileBrowserUpdate",
	     ("Building new list... (listId=%d)", listId));
  
  /* path O.K.? */
  if(!stat(path, &statBuffer) && S_ISDIR(statBuffer.st_mode)) {
    /* Get entries for scroll-list... */

    /* Directories... */
    DBUG_PRINT("msFileBrowserUpdate", ("Get directories to pipe..."));
    sprintf(buffer, "cd %s ; ls -1F 2>/dev/null", path);

    if((f = popen(buffer, "r")) == NULL) {
      sprintf(error, "Can't create pipe for 'ls'...");
      dieHard(error);
    }

    ADD_OBJECT_TO_LIST(&listId, NEW_STRING("../"));
    i = 1;
  
    strcpy(buffer, "");
    while(!feof(f)) {
      fscanf(f, "%s\n", buffer);
      if(strcmp(buffer, "./") && strcmp(buffer, "../") &&
	 buffer[strlen(buffer)-1] == '/') {
	ADD_OBJECT_TO_LIST(&listId, NEW_STRING(buffer));
	++i;
      }
    }

    pclose(f);

    /* Files... */
    DBUG_PRINT("msFileBrowserUpdate", ("Get files to pipe..."));
    sprintf(buffer, "cd %s ; ls -1dF %s 2>/dev/null", path, mask);

    if((f = popen(buffer, "r")) == NULL) {
      sprintf(error, "Can't create pipe for 'ls'...");
      dieHard(error);
    }

    strcpy(buffer, "");
    while(!feof(f)) {
      fscanf(f, "%s\n", buffer);
      if(buffer[strlen(buffer)-1] != '/') {
	ADD_OBJECT_TO_LIST(&listId, NEW_STRING(buffer));
	++i;
      }
    }

    pclose(f);

    DBUG_PRINT("msFileBrowserUpdate",
	       ("Built new list... (listId=%d)", listId));

    /* Add list to scroll-list... */
    t = (String *)GET_LIST_POINTER(listId, &k);
    SET_LIST(WI_my_requester_select_scroll, t, i);

    /* Just to force update... */
    SELECT_ITEM(WI_my_requester_select_scroll, 1);
    UNSELECT_ITEMS(WI_my_requester_select_scroll);

    illegalPath = FALSE;
  } else {
    DBUG_PRINT("msFileBrowserUpdate",
	       ("Illegal path --> no list... (listId=%d)", listId));

    ADD_OBJECT_TO_LIST(&listId, NEW_STRING("Illegal path!!!"));
    i = 1;
    t = (String *)XukcGetListPointer(listId, &k);
    SET_LIST(WI_my_requester_select_scroll, t, i);

    illegalPath = TRUE;
  }

  DBUG_PRINT("msFileBrowserUpdate", ("Added list to scroll-list..."));

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFileBrowserInitialize: (exported function)                             */
/*                                                                           */
/*  Initialize scroll-list and update contents of scrollview...              */
/*                                                                           */
/*****************************************************************************/

void msFileBrowserInitialize(which, title, mask, path, file)
short which;
char *title, *mask, *path, *file;
{
  DBUG_ENTER("msFileBrowserInitialize");

  actualWhich = which;

  SET_LABEL(WI_my_requester_select_label, title);

  SET_LABEL(WI_my_requester_select_enter_mask, mask);
  if(!strlen(path)) {
    SET_LABEL(WI_my_requester_select_enter_path, "./");
  } else {
    SET_LABEL(WI_my_requester_select_enter_path, path);
  }
  SET_LABEL(WI_my_requester_select_enter_file, file);

  msFileBrowserUpdate();

  display = GET_DISPLAY(WI_my_requester_box);

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFileBrowserSelectFile: (callback)                                      */
/*                                                                           */
/*  User clicked in scroll-list. Determine entry-type (directory/file) and   */
/*  update contents of scroll-list, path and file...                         */
/*                                                                           */
/*****************************************************************************/

void msFileBrowserSelectFile(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char selectedFile[FILE_LEN], path[FILE_LEN], s[FILE_LEN], *c;
  int l;

  DBUG_ENTER("msFileBrowserSelectFile");

  XFlush(display);

  /* illegal path (empty list)? */
  if(illegalPath) {
    UNSELECT_ITEMS(WI_my_requester_select_scroll);
    DBUG_VOID_RETURN;
  }

  strcpy(selectedFile, ((XawListReturnStruct *)call_data)->string);
  DBUG_PRINT("msFileBrowserSelectFile", ("selected=%s", selectedFile));

  /* File or Directory? */
  l = strlen(selectedFile);

  switch(selectedFile[l-1]) {

  case '/': /* Directory */
    GET_LABEL(WI_my_requester_select_enter_path, s);
    strcpy(path, s);

    /* Update path... */
    if(strcmp(selectedFile, "../") || !strlen(path)) {
      /* No ".." --> append to path */
      strcat(path, selectedFile);
    } else {
      /* ".." */
      l = strlen(path);
      path[l-1] = '\0';
      if((c = (char *)rindex(path, '/'))) {
	if(l>3) {
	  if((path[l-2] == '.') && (path[l-3] == '.')) {
	    strcat(path, "/../");
	  } else {
	    *(c+1) = '\0';
	  }
	} else {
	  *(c+1) = '\0';
	}
      } else {
	if(!strcmp(path, ".")) {
	  strcpy(path, "../");
	} else {
	  if(!strcmp(path, "..")) {
	    strcat(path, "/../");
	  } else {
	    strcpy(path, "./");
	  }
	}
      }
    }

    SET_LABEL(WI_my_requester_select_enter_path, path);
    msFileBrowserUpdate((Widget)NULL,
			(XtPointer)NULL, (XtPointer)NULL);

    /* Clear file... */
    SET_LABEL(WI_my_requester_select_enter_file, "");

    break;

  case '*':
  case '@':
  case '=': /* File */
    selectedFile[l-1] = '\0';

  default:
    SET_LABEL(WI_my_requester_select_enter_file, selectedFile);
    break;
  }

  DBUG_VOID_RETURN;
}


/*****************************************************************************/
/*                                                                           */
/*  msFileBrowserAccept: (callback)                                          */
/*                                                                           */
/*  Copy new path and file into the fields in the main window...             */
/*                                                                           */
/*****************************************************************************/

void msFileBrowserAccept(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  char path[FILE_LEN], file[FILE_LEN], s[FILE_LEN];

  DBUG_ENTER("msFileBrowserAccept");

  GET_LABEL(WI_my_requester_select_enter_file, s);
  strcpy(file, s);

  GET_LABEL(WI_my_requester_select_enter_path, s);
  strcpy(path, s);

  /* Make the selected file known to others... */
  fileSelectedHook(actualWhich, path, file);

  DBUG_VOID_RETURN;
}
    
