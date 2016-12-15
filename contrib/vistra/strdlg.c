/*************************************************************/
/* Antworte eine neue Popupshell fuer eine Dialog Widget.    */
/*************************************************************/
static Widget createStrDlgShell(Widget parent)
{ static Widget answer, strDlgButtonCancel;

  answer = XtVaCreatePopupShell(
      "vistra dialog", transientShellWidgetClass, parent,
      XtNallowShellResize, TRUE,
      NULL);
  strDlg = XtVaCreateManagedWidget(
      "strDlg", dialogWidgetClass, answer,
      NULL);

  XtVaSetValues(XtNameToWidget(strDlg, "label"),
      XtNresizable, TRUE, NULL);
  strDlgButtonOk = XtVaCreateManagedWidget(
      "strDlgButtonOk", commandWidgetClass, strDlg,
      XtNresizable, TRUE,
      NULL);
  strDlgButtonCancel = XtVaCreateManagedWidget(
      "strDlgButtonCancel", commandWidgetClass, strDlg,
      XtNlabel, "cancel",
      XtNresizable, TRUE,
      NULL);
  XtAddCallback(strDlgButtonCancel, XtNcallback, popdown, answer);

  return answer;
}       /* createStrDlgShell */


/*************************************************************/
/* Oeffne eine Dialogbox mit dem Titel title, in der der     */
/* Benutzer aufgefordert wird, einen String einzugeben.      */
/* Der Standardwert ist defaultValue. Der Benutzer kann      */
/* ueber 'Cancel' den Vorgang abbrechen, wobei NULL zurueck- */
/* gegeben wird. Oder er waehlt Button mit dem Label         */
/* buttonLabel, woraufhin der momentane Inhalt des Eingabe-  */
/* felds geantwortet wird.                                   */
/*************************************************************/
static void popupStrDlg(w, title, defaultValue, buttonLabel, callback)
Widget w;
char *title, *defaultValue, *buttonLabel;
void (*callback)();
{ Position x, y;

  XtVaSetValues(strDlg,
      XtNlabel, title,
      XtNvalue, defaultValue,
      NULL);
  XtOverrideTranslations(XtNameToWidget(strDlg, "value"), textTransl);
  XtRemoveAllCallbacks(strDlgButtonOk, XtNcallback);
  XtAddCallback(strDlgButtonOk, XtNcallback, (XtCallbackProc) callback, strDlg);
  XtVaSetValues(strDlgButtonOk,
      XtNlabel, buttonLabel,
      NULL);

  getPopupPosition(w, &x, &y);
  XtVaSetValues(strDlgShell,
      XtNx, x, XtNy, y, NULL);

  XtPopup(strDlgShell, XtGrabNonexclusive);
}       /* popupStrDlg */





