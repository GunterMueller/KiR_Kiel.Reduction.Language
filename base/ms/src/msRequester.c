#include "msRequester.h"

char lastRequestAnswer[DISPLAY_LEN]="";

static void (*rememberContinuation)() = NULL;

void callbackOK(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("callbackOK");

  GET_LABEL(WI_ms_requester_action_o_k, temp);
  DBUG_PRINT("callbackOK", ("answer=%s", temp));
  
  strcpy(lastRequestAnswer, temp);

  XtPopdown(WI_ms_requester);

  if(rememberContinuation) {
    (*rememberContinuation)();
  }

  DBUG_VOID_RETURN;
}


void callbackCancel(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("callbackCancel");

  GET_LABEL(WI_ms_requester_action_cancel, temp);
  DBUG_PRINT("callbackCancel", ("answer=%s", temp));
  
  strcpy(lastRequestAnswer, temp);

  XtPopdown(WI_ms_requester);

  if(rememberContinuation) {
    (*rememberContinuation)();
  }

  DBUG_VOID_RETURN;
}


void callbackHelp(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     caddr_t call_data;
{
  char temp[DISPLAY_LEN];

  DBUG_ENTER("callbackHelp");

  GET_LABEL(WI_ms_requester_action_help, temp);
  DBUG_PRINT("callbackHelp", ("answer=%s", temp));
  
  strcpy(lastRequestAnswer, temp);

  XtPopdown(WI_ms_requester);

  if(rememberContinuation) {
    (*rememberContinuation)();
  }

  DBUG_VOID_RETURN;
}


void doRequest(textTitle1, textTitle2, textTitle3,
		textOK, textCancel, textHelp, continuation)
char *textTitle1, *textTitle2, *textTitle3,
     *textOK, *textCancel, *textHelp;
void (*continuation)();
{
  DBUG_ENTER("doRequest");

  rememberContinuation = continuation;
  strcpy(lastRequestAnswer, "");

  SET_VALUE(WI_ms_requester_title1, XtNlabel, textTitle1);
  SET_VALUE(WI_ms_requester_title2, XtNlabel, textTitle2);
  SET_VALUE(WI_ms_requester_title3, XtNlabel, textTitle3);

  if(strlen(textOK)) {
    SET_VALUE(WI_ms_requester_action_o_k, XtNlabel, textOK);
    XtManageChild(WI_ms_requester_action_o_k);
  } else {
    XtUnmanageChild(WI_ms_requester_action_o_k);
  }

  if(strlen(textCancel)) {
    SET_VALUE(WI_ms_requester_action_cancel, XtNlabel, textCancel);
    XtManageChild(WI_ms_requester_action_cancel);
  } else {
    XtUnmanageChild(WI_ms_requester_action_cancel);
  }

  if(strlen(textHelp)) {
    SET_VALUE(WI_ms_requester_action_help, XtNlabel, textHelp);
    XtManageChild(WI_ms_requester_action_help);
  } else {
    XtUnmanageChild(WI_ms_requester_action_help);
  }

  ADD_CALLBACK(WI_ms_requester_action_o_k, XtNcallback, callbackOK);
  ADD_CALLBACK(WI_ms_requester_action_cancel, XtNcallback, callbackCancel);
  ADD_CALLBACK(WI_ms_requester_action_help, XtNcallback, callbackHelp);
  
  XtPopup(WI_ms_requester, XtGrabExclusive);

  DBUG_VOID_RETURN;
}
