static int val[2], from[2], to[2];
static Widget int2Cancel;

/***********************************************/
/* Kreiere und antworte eine neue Popup-Shell  */
/* fuer eine Dialogbox, bei der der Benutzer   */
/* 2 Integers eingibt. Der Benutzer kann den   */
/* Vorgang ueber einen 'Cancel'-Button         */
/* abbrechen.                                  */
/***********************************************/
static Widget createInt2DlgShell(Widget parent)
{ Widget answer, int2Form;

  answer = XtVaCreatePopupShell(
      "int2Dlg", transientShellWidgetClass, parent,
      XtNallowShellResize, TRUE,
      NULL);
  int2Form = XtVaCreateManagedWidget(
      "int2Form", formWidgetClass, answer,
      NULL);
  int2Headline = XtVaCreateManagedWidget(
      "int2Headline", labelWidgetClass, int2Form,
      XtNjustify, XtJustifyLeft,
      XtNresizable, TRUE,
      XtNfromVert, NULL,
      XtNvertDistance, INT2_TOPMARGIN,
      XtNfromHoriz, NULL,
      XtNhorizDistance, INT2_LEFTMARGIN,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNborderWidth, 0,
      NULL);
  int2Label1 = XtVaCreateManagedWidget(
      "int2Label1", labelWidgetClass, int2Form,
      XtNjustify, XtJustifyLeft,
      XtNwidth, INT2_LABELWIDTH,
      XtNresize, FALSE,
      XtNfromVert, int2Headline,
      XtNvertDistance, INT2_HDLINE_LABEL_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, INT2_LEFTMARGIN,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNborderWidth, 0,
      NULL);
  int2Label2 = XtVaCreateManagedWidget(
      "int2Label2", labelWidgetClass, int2Form,
      XtNjustify, XtJustifyLeft,
      XtNwidth, INT2_LABELWIDTH,
      XtNresize, FALSE,
      XtNfromVert, int2Label1,
      XtNvertDistance, INT2_DIST_BETWEEN_LABELS,
      XtNfromHoriz, NULL,
      XtNhorizDistance, INT2_LEFTMARGIN,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNborderWidth, 0,
      NULL);
  int2Text[0] = XtVaCreateManagedWidget(
      "int2Text0", asciiTextWidgetClass, int2Form,
      XtNdisplayCaret, TRUE,
      XtNeditType, XawtextEdit,
      XtNwidth, INT2_TEXTWIDTH,
      XtNfromVert, int2Headline,
      XtNvertDistance, INT2_HDLINE_LABEL_DIST,
      XtNfromHoriz, int2Label1,
      XtNhorizDistance, INT2_LABEL_TEXT_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  int2Text[1] = XtVaCreateManagedWidget(
      "int2Text1", asciiTextWidgetClass, int2Form,
      XtNdisplayCaret, TRUE,
      XtNeditType, XawtextEdit,
      XtNwidth, INT2_TEXTWIDTH,
      XtNfromVert, int2Label1,
      XtNvertDistance, INT2_DIST_BETWEEN_LABELS,
      XtNfromHoriz, int2Label1,
      XtNhorizDistance, INT2_LABEL_TEXT_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  XtOverrideTranslations(int2Text[0], textTransl);
  XtOverrideTranslations(int2Text[1], textTransl);
  
  int2Up[0] = XtVaCreateManagedWidget(
      "int2Up0", commandWidgetClass, int2Form,
      XtNjustify, XtJustifyCenter,
      XtNbitmap, up,
      XtNfromVert, int2Headline,
      XtNvertDistance, INT2_HDLINE_LABEL_DIST,
      XtNfromHoriz, int2Text[0],
      XtNhorizDistance, INT2_TEXT_ARROW_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainRight,
      XtNright, XtChainRight,
      NULL);
  int2Down[0] = XtVaCreateManagedWidget(
      "int2Down0", commandWidgetClass, int2Form,
      XtNjustify, XtJustifyCenter,
      XtNbitmap, down,
      XtNfromVert, int2Headline,
      XtNvertDistance, INT2_HDLINE_LABEL_DIST,
      XtNfromHoriz, int2Up[0],
      XtNhorizDistance, 0,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainRight,
      XtNright, XtChainRight,
      NULL);
  int2Up[1] = XtVaCreateManagedWidget(
      "int2Up1", commandWidgetClass, int2Form,
      XtNjustify, XtJustifyCenter,
      XtNbitmap, up,
      XtNfromVert, int2Label1,
      XtNvertDistance, INT2_DIST_BETWEEN_LABELS,
      XtNfromHoriz, int2Text[1],
      XtNhorizDistance, INT2_TEXT_ARROW_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainRight,
      XtNright, XtChainRight,
      NULL);
  int2Down[1] = XtVaCreateManagedWidget(
      "int2Down1", commandWidgetClass, int2Form,
      XtNjustify, XtJustifyCenter,
      XtNbitmap, down,
      XtNfromVert, int2Label1,
      XtNvertDistance, INT2_DIST_BETWEEN_LABELS,
      XtNfromHoriz, int2Up[1],
      XtNhorizDistance, 0,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainRight,
      XtNright, XtChainRight,
      NULL);
  int2Ok = XtVaCreateManagedWidget(
      "int2Ok", commandWidgetClass, int2Form,
      XtNwidth, INT2_COMMANDWITH,
      XtNresizable, TRUE,
      XtNfromVert, int2Label2,
      XtNvertDistance, INT2_LABEL_COMMAND_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, INT2_LEFTMARGIN,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  int2Cancel = XtVaCreateManagedWidget(
      "int2Cancel", commandWidgetClass, int2Form,
      XtNwidth, INT2_COMMANDWITH,
      XtNlabel, "cancel",
      XtNfromVert, int2Label2,
      XtNvertDistance, INT2_LABEL_COMMAND_DIST,
      XtNfromHoriz, int2Ok,
      XtNhorizDistance, INT2_DIST_BETWEEN_COMMANDS,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);

  return answer;
}         /* createInt2DlgShell */


/***********************************************/
/* Oeffne die Dialogbox int2DlgShell, die vom  */
/* Benutzer zwei Integers erfragt. Der Benutzer*/
/* hat die Moeglichkeit, den Vorgang ueber     */
/* 'Cancel' abzubrechen.                       */
/***********************************************/
static void popupInt2Dlg(w, title, headline, item1, item2,
			 from1, to1, from2, to2, default1, default2,
			 okButtonLabel,
			 up1_cb, down1_cb, up2_cb, down2_cb, 
                         ok_cb, ok_data, 
                         cancel_cb, cancel_data)
Widget w;
char *title, *headline, *item1, *item2, *okButtonLabel;
int from1, to1, from2, to2, default1, default2;
void (*up1_cb)(), (*down1_cb)(), (*up2_cb)(), (*down2_cb)(), 
     (*ok_cb)(), (*cancel_cb)();
XtPointer ok_data, cancel_data;
{ Position x, y;
  char buf[20];

  getPopupPosition(w, &x, &y);
  XtVaSetValues(int2DlgShell,
      XtNtitle, title,
      XtNx, x, XtNy, y,
      NULL);
  XtVaSetValues(int2Headline, XtNlabel, headline, NULL);
  XtVaSetValues(int2Label1, XtNlabel, item1, NULL);
  XtVaSetValues(int2Label2, XtNlabel, item2, NULL);
  XtVaSetValues(int2Ok, XtNlabel, okButtonLabel, NULL);

  from[0] = from1;
  from[1] = from2;
  to[0] = to1;
  to[1] = to2;
  val[0] = default1;
  val[1] = default2;
  updateInt2Text();

  /* Installiere die Callbacks */
  XtRemoveAllCallbacks(int2Ok, XtNcallback);
  XtAddCallback(int2Ok, XtNcallback, ok_cb, ok_data);
  XtRemoveAllCallbacks(int2Up[0], XtNcallback);
  XtAddCallback(int2Up[0], XtNcallback, up1_cb, (XtPointer) 0);
  XtRemoveAllCallbacks(int2Down[0], XtNcallback);
  XtAddCallback(int2Down[0], XtNcallback, down1_cb, (XtPointer) 0);
  XtRemoveAllCallbacks(int2Up[1], XtNcallback);
  XtAddCallback(int2Up[1], XtNcallback, up1_cb, (XtPointer) 1);
  XtRemoveAllCallbacks(int2Down[1], XtNcallback);
  XtAddCallback(int2Down[1], XtNcallback, down1_cb, (XtPointer) 1);
  XtRemoveAllCallbacks(int2Cancel, XtNcallback);
  XtAddCallback(int2Cancel, XtNcallback, cancel_cb, cancel_data);
  XtPopup(int2DlgShell, XtGrabNonexclusive);
}        /* popupInt2Dlg */


/*************************************************************/
/*************************************************************/
static void updateInt2Text()
{ char buf[20];
  unsigned i;

  for(i = 0; i <= 1; i++)
  { sprintf(buf, "%d", val[i]);
    XtVaSetValues(int2Text[i], XtNstring, buf, NULL);
  }
}        /* updateInt2Text */


/********************************************************/
/* client_data == 0:  oberes Command-Widget.            */
/* client_data == 1:  unteres Command-Widget.           */
/********************************************************/
static void arrowUp(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ unsigned id = (unsigned) client_data;
  char buf[20];

  val[id] = (val[id] == to[id] ? from[id] : val[id] + 1);
  sprintf(buf, "%d", val[id]);
  XtVaSetValues(int2Text[id], XtNstring, buf, NULL);
}       /* arrowUp */


/********************************************************/
/* client_data == 0:  oberes Command-Widget.            */
/* client_data == 1:  unteres Command-Widget.           */
/********************************************************/
static void arrowDown(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ unsigned id = (unsigned) client_data;
  char buf[20];

  val[id] = (val[id] == from[id] ? to[id] : val[id] - 1);
  sprintf(buf, "%d", val[id]);
  XtVaSetValues(int2Text[id], XtNstring, buf, NULL);
}       /* arrowDown */


/********************************************************/
/* client_data == 0:  oberes Command-Widget.            */
/* client_data == 1:  unteres Command-Widget.           */
/********************************************************/
static void arrowUpMat(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ unsigned id = (unsigned) client_data;

  val[id] = (val[id] == to[id] ? from[id] : val[id] + 1);
  val[1-id] = (to[id] % val[id] ? to[id] / val[id] + 1 : to[id] / val[id]);
  updateInt2Text();
}       /* arrowUpMat */


/********************************************************/
/* client_data == 0:  oberes Command-Widget.            */
/* client_data == 1:  unteres Command-Widget.           */
/********************************************************/
static void arrowDownMat(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ unsigned id = (unsigned) client_data;

  val[id] = (val[id] == from[id] ? to[id] : val[id] - 1);
  val[1-id] = (to[id] % val[id] ? to[id] / val[id] + 1 : to[id] / val[id]);
  updateInt2Text();
}       /* arrowDownMat */
















