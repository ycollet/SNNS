/*************************************************************/
/* Return a new popup shell for a dialog widget.             */
/*************************************************************/
static Widget createStrDlgShell(Widget parent) {
    static Widget answer, strDlgButtonCancel;

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
/* Open a dialog box with the title "title", in which the    */
/* user is prompted to enter a string.                       */
/* The default value is defaultValue. The user can cancel    */
/* the operation via 'Cancel', in which case NULL is         */
/* returned. Or the user selects the button with the label   */
/* buttonLabel, whereupon the current content of the input   */
/* field is returned.                                         */
/*************************************************************/
static void popupStrDlg(w, title, defaultValue, buttonLabel, callback)
Widget w;
char *title, *defaultValue, *buttonLabel;
XtCallbackProc callback;
{
    Position x, y;

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





