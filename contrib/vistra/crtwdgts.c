#include "hook"
#include "up"
#include "down"
#include "left"

static void getPopupPosition(Widget, Position *, Position *);
static Widget createErrDlgShell(Widget);
static Widget createStrDlgShell(Widget);
static Widget createStr2DlgShell(Widget);
static Widget createInt2DlgShell(Widget);
#ifdef XFWF_FILE_SELECTION
static Widget createFileSelShell(Widget);
#endif
static SSW createSSW();
static Widget createTextWindow();
static void addMenuItems();                         /* error gesetzt */
static void addFileMenu(Widget);
static void addTransformMenu(Widget);
static void addRemoveMenu(Widget);
static void addShowMenu(Widget);
static void addFormatMenu(Widget);
static void addInfoWidgets(Widget);
static void addPatternFormWidgets(Widget);
static void addLoadMenu(Widget);
static void addOperationWidgets(Widget);
static void noop(Widget, XEvent *, String *, Cardinal *);

static int textHeight, textWidth, labelHeight, labelWidth;
static XtTranslations toggleTransl, textTransl;
static Pixmap left, up, down;
static int vertScrollBarLength, horizScrollBarLength;


/****************************************************/
/* Kreiere alle Widgets des Programms und weise sie */
/* den entsprechenden globalen Widget Variablen zu. */
/****************************************************/
static void createAllWidgets(Widget top_level)
{ static char toggleTransStr[] = "#override\n\
  <Btn1Down>,<Btn1Up>: set() notify()";

  static char textTransStr[] =
 "<Key>Return: noop() \n\
  <Key>Linefeed: noop() \n\
  <Key>Down: noop() \n\
  <Key>Tab: noop() \n\
  Ctrl<Key>C: noop() \n\
  Ctrl<Key>I: noop() \n\
  Ctrl<Key>J: noop() \n\
  Ctrl<Key>M: noop() \n\
  Ctrl<Key>O: noop() \n\
  Ctrl<Key>Q: noop() \n\
  Ctrl<Key>R: noop() \n\
  Ctrl<Key>S: noop() \n\
  Meta<Key>I: noop()";

  XtActionsRec actiontable[] =
    { { "noop", noop },
    };
  XtAppAddActions(app_context, actiontable, 1);

  toggleTransl = XtParseTranslationTable(toggleTransStr);
  textTransl = XtParseTranslationTable(textTransStr);

  hook = XCreateBitmapFromData(XtDisplay(topLevel),
            RootWindowOfScreen(XtScreen(topLevel)),
            hook_bits, hook_width, hook_height);
  up = XCreateBitmapFromData(XtDisplay(topLevel),
            RootWindowOfScreen(XtScreen(topLevel)),
            up_bits, up_width, up_height);
  down = XCreateBitmapFromData(XtDisplay(topLevel),
            RootWindowOfScreen(XtScreen(topLevel)),
            down_bits, down_width, down_height);
  left = XCreateBitmapFromData(XtDisplay(topLevel),
            RootWindowOfScreen(XtScreen(topLevel)),
            left_bits, left_width, left_height);

  form = XtVaCreateManagedWidget(
     "form",            /* name */
     formWidgetClass,   /* class */
     top_level,          /* parent */
     NULL);             /* terminate list */

  menuBar = XtVaCreateManagedWidget(
     "menuBar",         /* name */
     boxWidgetClass,    /* class */
     form,              /* parent */
     XtNorientation, XtorientHorizontal,
     XtNvertDistance, 0,
     XtNfromVert, NULL,
     XtNhorizDistance, 0,
     XtNfromHoriz, NULL,
     XtNtop, XtChainTop,
     XtNbottom, XtChainTop,
     XtNleft, XtChainLeft,
     XtNright, XtChainLeft,
     XtNborderWidth, 0,
     NULL);             /* terminate list */
  addFileMenu(menuBar);
  addTransformMenu(menuBar);
  addRemoveMenu(menuBar);
  addShowMenu(menuBar);
  addFormatMenu(menuBar);
  if(error) return;

  infoForm = XtVaCreateManagedWidget(
     "infoForm", formWidgetClass, form,
     XtNvertDistance, 0,
     XtNfromVert, menuBar,
     XtNhorizDistance, 0,
     XtNfromHoriz, NULL,
     XtNtop, XtChainTop,
     XtNbottom, XtChainTop,
     XtNleft, XtChainLeft,
     XtNright, XtChainLeft,
     XtNresizable, TRUE,
     XtNborderWidth, 0,
     NULL);
  addInfoWidgets(infoForm);

  patternForm = XtVaCreateManagedWidget(
    "patternForm", formWidgetClass, form,
    XtNvertDistance, 0,
    XtNfromVert, infoForm,
    XtNhorizDistance, 0,
    XtNfromHoriz, NULL,
    XtNtop, XtChainTop,
    XtNbottom, XtChainBottom,
    XtNleft, XtChainLeft,
    XtNright, XtChainRight,
    XtNborderWidth, BORDER_WIDTH,
    NULL);
  addPatternFormWidgets(patternForm);

  infoLabel = XtVaCreateManagedWidget(
    "infoLabel", labelWidgetClass, form,
    XtNresize, TRUE,
    XtNresizable, TRUE,
    XtNborderWidth, 0,
    XtNlabel, "",
    XtNjustify, XtJustifyLeft,
    XtNfromVert, patternForm,
    XtNvertDistance, 16,
    XtNfromHoriz, NULL,
    XtNhorizDistance, 0,
    NULL);

  errDlgShell = createErrDlgShell(top_level);
  strDlgShell = createStrDlgShell(top_level);
  str2DlgShell = createStr2DlgShell(top_level);
  int2DlgShell = createInt2DlgShell(top_level);

  if(! (swapSSW = createSSW(top_level, SSW_WIDTH, SSW_HEIGHT, 
                    popdownSwapSSW, NULL))) error(1);
  statistics = createTextWindow("", "", STATISTICS_WIDTH, STATISTICS_HEIGHT,
				top_level, popdown);
#ifdef XFWF_FILE_SELECTION  
  fileSelShell = createFileSelShell(top_level); 
#endif 

  error = 0;
}     /* createAllWidgets */


/***********************************************/
/***********************************************/
static void addPatternFormWidgets(Widget parent)
{ int symbolWidth, i, j;
  unsigned vertBytes, horizBytes;
  int ascent, descent, dummy;
  XCharStruct char_struct;

  textWidth = XTextWidth(app_data.numberFont, EXAMPLE_NUMBER,
		    strlen(EXAMPLE_NUMBER)) + (2 * TEXT_MARGINS);
  labelWidth = textWidth + (2 * BORDER_WIDTH);
  XTextExtents(app_data.numberFont, EXAMPLE_NUMBER, strlen(EXAMPLE_NUMBER),
               &dummy, &ascent, &descent, &char_struct);
  textHeight = ascent + descent + (2 * TEXT_MARGINS);
  labelHeight = textHeight + (2 * BORDER_WIDTH);
  symbolWidth = XTextWidth(app_data.numberFont, EXAMPLE_CLASSNAME,
                            strlen(EXAMPLE_CLASSNAME));

  visVecs = (PATTERNS_HEIGHT / (textHeight + BORDER_WIDTH)) + 1;
  visDims = (PATTERNS_WIDTH / (textWidth + BORDER_WIDTH)) + 1;

  /* Allokiere Speicher fuer alle Widget-Arrays */
  vertBytes = visVecs * sizeof(Widget);
  horizBytes = visDims * sizeof(Widget);
  if(! (nrLabels = (Widget *) malloc(vertBytes))) error(1);
  if(! (dimLabels = (Widget *) malloc(horizBytes))) error(1);
  if(! (scalarsHoriz = (Widget *) malloc(horizBytes))) error(1);
  if(! (scalarsVert = (Widget *) malloc(vertBytes))) error(1);
  if(! (classLabels = (Widget *) malloc(vertBytes))) error(1);
  if(! (elems = (Widget **) malloc(visVecs * sizeof(Widget *)))) error(1);
  for(i = 0; i < visVecs; i++)
    if(! (elems[i] = (Widget *) malloc(horizBytes))) error(1);

  /* Kreiere die Widgets der Arrays */
  for(i = 0; i < visVecs; i++)
  {
    nrLabels[i] = XtVaCreateManagedWidget(
      "nrLabel", labelWidgetClass, parent,
      XtNfont, app_data.numberFont,
      XtNjustify, XtJustifyRight,
      XtNborderWidth, 0,
      XtNlabel, "",
      XtNtop, XtChainTop,
      XtNbottom, XtChainBottom,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNresize, FALSE,
      XtNwidth, labelWidth,
      XtNheight, labelHeight,
      XtNfromVert, (i == 0 ? NULL : nrLabels[i-1]),
      XtNvertDistance, (i == 0 ? PV_TOPMARGIN + labelHeight : - BORDER_WIDTH),
      XtNfromHoriz, NULL,
      XtNhorizDistance, PV_LEFTMARGIN,
      NULL);
  }

  for(i = 0; i < visDims; i++)
  {
    dimLabels[i] = XtVaCreateManagedWidget(
      "dimLabel", labelWidgetClass, parent,
      XtNlabel, "", 
      XtNborderWidth, 0,
      XtNjustify, XtJustifyCenter,
      XtNfont, app_data.numberFont,
      XtNresize, FALSE,
      XtNwidth, labelWidth,
      XtNheight, labelHeight,
      XtNfromVert, NULL,
      XtNvertDistance, PV_TOPMARGIN,
      XtNfromHoriz, (i == 0 ? nrLabels[0] : dimLabels[i-1]),
      XtNhorizDistance, - BORDER_WIDTH,
      NULL);
  }
  for(i = 0; i < visVecs; i++)
    for(j = 0; j < visDims; j++)
    {
      elems[i][j] = XtVaCreateManagedWidget(
        "elem", labelWidgetClass, parent,
        XtNjustify, XtJustifyRight,
        XtNborderWidth, BORDER_WIDTH,
        XtNfont, app_data.numberFont,
        XtNinternalWidth, TEXT_MARGINS,
        XtNresize, FALSE,
        XtNlabel, "",
        XtNwidth, textWidth,
        XtNheight, textHeight,
        XtNfromVert, (i == 0 ? dimLabels[j] : elems[i-1][j]),
        XtNvertDistance, (i == 0 ? 0 : - BORDER_WIDTH),
        XtNfromHoriz, (j == 0 ? nrLabels[i] : elems[i][j-1]),
        XtNhorizDistance, (j == 0 ? 0 : - BORDER_WIDTH),
        NULL);
    }

  for(i = 0; i < visDims; i++)
  {
    scalarsHoriz[i] = XtVaCreateManagedWidget(
        "scalarsHoriz", asciiTextWidgetClass, parent,
        XtNborderWidth, BORDER_WIDTH,
	XtNtopMargin, TEXT_MARGINS,
	XtNbottomMargin, TEXT_MARGINS,
	XtNleftMargin, TEXT_MARGINS,
	XtNrightMargin, TEXT_MARGINS,
        XtNdisplayCaret, TRUE,
        XtNeditType, XawtextEdit,
        XtNfont, app_data.numberFont,
        XtNstring, "",
        XtNwidth, textWidth,
        XtNheight, textHeight,
        XtNresize, XawtextResizeNever,
        XtNscrollHorizontal, XawtextScrollNever,
        XtNscrollVertical, XawtextScrollNever,
        XtNtype, XawAsciiString,
        XtNfromVert, elems[visVecs-1][i],
        XtNvertDistance, VERT_SKIP,
        XtNfromHoriz, (i == 0 ? nrLabels[0] : scalarsHoriz[i-1]),
        XtNhorizDistance, (i == 0 ? 0 : - BORDER_WIDTH),
        NULL);
    XtOverrideTranslations(scalarsHoriz[i], textTransl);
  }

  labelClass = XtVaCreateManagedWidget(
      "labelClass", labelWidgetClass, parent,
      XtNlabel, "Class:",
      XtNfont, app_data.numberFont,
      XtNborderWidth, 0,
      XtNjustify, XtJustifyCenter,
      XtNresize, FALSE,
      XtNfromVert, NULL,
      XtNvertDistance, PV_TOPMARGIN,
      XtNfromHoriz, dimLabels[visDims-1],
      XtNhorizDistance, 0,
      XtNwidth, symbolWidth,
      XtNheight, labelHeight,
      NULL);

  for(i = 0; i < visVecs; i++)
    classLabels[i] = XtVaCreateManagedWidget(
        "classLabel", labelWidgetClass, parent,
        XtNlabel, "",
        XtNfont, app_data.numberFont,
        XtNborderWidth, 0,
        XtNjustify, XtJustifyCenter,
        XtNresize, FALSE,
        XtNwidth, symbolWidth,
        XtNheight, labelHeight,
        XtNfromVert, (i == 0 ? labelClass : classLabels[i-1]),
        XtNvertDistance, (i == 0 ? 0 : - BORDER_WIDTH),
        XtNfromHoriz, elems[i][visDims-1],
        XtNhorizDistance, 0,
        NULL);

  for(i = 0; i < visVecs; i++)
  { scalarsVert[i] = XtVaCreateManagedWidget(
        "scalarsVert", asciiTextWidgetClass, parent,
        XtNborderWidth, BORDER_WIDTH,
	XtNtopMargin, TEXT_MARGINS,
	XtNbottomMargin, TEXT_MARGINS,
	XtNleftMargin, TEXT_MARGINS,
	XtNrightMargin, TEXT_MARGINS,
        XtNdisplayCaret, TRUE,
        XtNeditType, XawtextEdit,
        XtNstring, "",
        XtNfont, app_data.numberFont,
        XtNwidth, textWidth,
        XtNheight, textHeight,
        XtNresize, XawtextResizeNever,
        XtNscrollHorizontal, XawtextScrollNever,
        XtNscrollVertical, XawtextScrollNever,
        XtNtype, XawAsciiString,
        XtNfromVert, (i == 0 ? NULL : scalarsVert[i-1]),
        XtNvertDistance, (i == 0 ? PV_TOPMARGIN+labelHeight : -BORDER_WIDTH),
        XtNfromHoriz, classLabels[i],
        XtNhorizDistance, 0,
        NULL);
    XtOverrideTranslations(scalarsVert[i], textTransl);
  }

  vertScrollBarLength = visVecs * (textHeight + BORDER_WIDTH);
  vertScrollBar = XtVaCreateManagedWidget(
      "vertScrollBar", scrollbarWidgetClass, parent,
      XtNorientation, XtorientVertical,
      XtNfromVert, labelClass,
      XtNvertDistance, 0,
      XtNfromHoriz, scalarsVert[0],
      XtNhorizDistance, HORIZ_SKIP,
      XtNheight, vertScrollBarLength,
      NULL);
  XtAddCallback(vertScrollBar, XtNscrollProc, scrollVertical, NULL);
  XtAddCallback(vertScrollBar, XtNjumpProc, jumpVertical, NULL);

  horizScrollBarLength = visDims * (textWidth + BORDER_WIDTH);
  horizScrollBar = XtVaCreateManagedWidget(
      "horizScrollBar", scrollbarWidgetClass, parent,
      XtNorientation, XtorientHorizontal,
      XtNfromVert, scalarsHoriz[0],
      XtNvertDistance, VERT_SKIP,
      XtNfromHoriz, nrLabels[0],
      XtNhorizDistance, 0,
      XtNwidth, horizScrollBarLength,
      NULL);
  XtAddCallback(horizScrollBar, XtNscrollProc, scrollHorizontal, NULL);
  XtAddCallback(horizScrollBar, XtNjumpProc, jumpHorizontal, NULL);

  horizToggle = XtVaCreateManagedWidget(
      "horizToggle", toggleWidgetClass, parent,
      XtNradioData, (XtPointer) FILL_HORIZ,
      XtNstate, FALSE,
      XtNjustify, XtJustifyCenter,
      XtNbitmap, left,
      XtNwidth, textWidth,
      XtNheight, textHeight,
      XtNfromVert, classLabels[visVecs-1],
      XtNvertDistance, VERT_SKIP,
      XtNfromHoriz, scalarsHoriz[visDims-1],
      XtNhorizDistance, 0,
      NULL);
  vertToggle = XtVaCreateManagedWidget(
      "vertToggle", toggleWidgetClass, parent,
      XtNradioData, (XtPointer) FILL_VERT,
      XtNradioGroup, horizToggle,
      XtNstate, TRUE,
      XtNjustify, XtJustifyCenter,
      XtNbitmap, up,
      XtNwidth, textWidth,
      XtNheight, textHeight,
      XtNfromVert, scalarsVert[visVecs-1],
      XtNvertDistance, 0,
      XtNfromHoriz, classLabels[0],
      XtNhorizDistance, 0,
      NULL);
  XtOverrideTranslations(horizToggle, toggleTransl);
  XtOverrideTranslations(vertToggle, toggleTransl);
  XtAddCallback(horizToggle, XtNcallback, toggleScalars, NULL);
  XtAddCallback(vertToggle, XtNcallback, toggleScalars, NULL);
  scalarSwitch = FILL_VERT;

  loadMenuButtonVert = XtVaCreateManagedWidget(
      "loadMenuButtonVert", menuButtonWidgetClass, parent,
      XtNlabel, "load",
      XtNmenuName, "loadMenu",
      XtNsensitive, TRUE,
      XtNfromVert, vertToggle,
      XtNvertDistance, 0,
      XtNfromHoriz, classLabels[0],
      XtNhorizDistance, 0,
      XtNwidth, textWidth,
      NULL);
  addLoadMenu(loadMenuButtonVert);

  loadMenuButtonHoriz = XtVaCreateManagedWidget(
      "loadMenuButtonHoriz", menuButtonWidgetClass, parent,
      XtNlabel, "load",
      XtNmenuName, "loadMenu",
      XtNsensitive, FALSE,
      XtNfromVert, horizToggle,
      XtNvertDistance, 0,
      XtNfromHoriz, scalarsHoriz[visDims-1],
      XtNhorizDistance, 0,
      XtNwidth, textWidth,
      NULL);
  addLoadMenu(loadMenuButtonHoriz);

  operationBox = XtVaCreateManagedWidget(
      "operationBox", boxWidgetClass, parent,
      XtNorientation, XtorientHorizontal,
      XtNfromVert, horizScrollBar,
      XtNvertDistance, VERT_SKIP,
      XtNfromHoriz, nrLabels[0],
      XtNhorizDistance, 0,
      XtNborderWidth, 0,
      NULL);
  addOperationWidgets(operationBox);
}        /* addPatternFormWidgets */


/***********************************************/
/* Erzeuge Label-Widgets als Kinder von parent.*/
/***********************************************/
static void addInfoWidgets(Widget parent)
{
  labelFile = XtVaCreateManagedWidget(
      "labelFile", labelWidgetClass, parent,
      XtNlabel, "File:",
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, NULL,
      XtNhorizDistance, 4,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNjustify, XtJustifyLeft,
      XtNborderWidth, 0,
      NULL);
  fileLabel = XtVaCreateManagedWidget(
      "fileLabel", labelWidgetClass, parent,
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, labelFile,
      XtNhorizDistance, DIST_TITLE_VALUE,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNjustify, XtJustifyLeft,
      XtNlabel, "",
      XtNresizable, TRUE,
      XtNborderWidth, 0,
      NULL);
  labelNumOfPatterns = XtVaCreateManagedWidget(
      "labelNumOfPatterns", labelWidgetClass, parent,
      XtNlabel, "Number of patterns:",
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, fileLabel,
      XtNhorizDistance, DIST_BETWEEN_INFOS,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNjustify, XtJustifyLeft,
      XtNborderWidth, 0,
      NULL);
  numOfPatternsLabel = XtVaCreateManagedWidget(
      "numOfPatternsLabel", labelWidgetClass, parent,
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, labelNumOfPatterns,
      XtNhorizDistance, DIST_TITLE_VALUE,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNjustify, XtJustifyLeft,
      XtNlabel, "",
      XtNresizable, TRUE,
      XtNborderWidth, 0,
      NULL);
  labelFormat = XtVaCreateManagedWidget(
      "labelFormat", labelWidgetClass, parent,
      XtNlabel, "Format:",
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, numOfPatternsLabel,
      XtNhorizDistance, DIST_BETWEEN_INFOS,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNjustify, XtJustifyLeft,
      XtNborderWidth, 0,
      NULL);
  formatLabel = XtVaCreateManagedWidget(
      "formatLabel", labelWidgetClass, parent,
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, labelFormat,
      XtNhorizDistance, DIST_TITLE_VALUE,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNjustify, XtJustifyLeft,
      XtNlabel, "",
      XtNresizable, TRUE,
      XtNborderWidth, 0,
      NULL);
  inputToggle = XtVaCreateManagedWidget(
      "inputToggle", toggleWidgetClass, parent,
      XtNlabel, "Input",
      XtNradioData, (XtPointer) EDIT_INPUT,
      XtNstate, TRUE,
      XtNhorizDistance, 4,
      XtNfromHoriz, NULL,
      XtNvertDistance, 4,
      XtNfromVert, labelFile,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNresizable, FALSE,
      NULL);
  outputToggle = XtVaCreateManagedWidget(
      "outputToggle",  toggleWidgetClass, parent,
      XtNlabel, "Output",
      XtNradioGroup, inputToggle,
      XtNradioData, (XtPointer) EDIT_OUTPUT,
      XtNstate, FALSE,
      XtNhorizDistance, 4,
      XtNfromHoriz, inputToggle,
      XtNvertDistance, 4,
      XtNfromVert, labelFile,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNresizable, FALSE,
      NULL);
  XtOverrideTranslations(inputToggle, toggleTransl);
  XtOverrideTranslations(outputToggle, toggleTransl);
  XtAddCallback(inputToggle, XtNcallback, toggle, NULL);
  XtAddCallback(outputToggle, XtNcallback, toggle, NULL);

  inOutSwitch = EDIT_INPUT;
}        /* addInfoWidgets */


/***********************************************/
/* Fuege dem Widget parent alle Buttons als    */
/* Kinder hinzu, die Operationen der Skalar-   */
/* Vektoren auf der VecColl (Input- oder Out-  */
/* putvektoren), die momentan im Window darge- */
/* stellt sind, anstossen.                     */
/***********************************************/
static void addOperationWidgets(Widget parent)
{
  addCommand = XtVaCreateManagedWidget(
      "addCommand", commandWidgetClass, parent,
      XtNlabel, "add",
      NULL);
  XtAddCallback(addCommand, XtNcallback, addScalars, NULL);

  subCommand = XtVaCreateManagedWidget(
      "subCommand", commandWidgetClass, parent,
      XtNlabel, "subtract",
      NULL);
  XtAddCallback(subCommand, XtNcallback, subScalars, NULL);

  multCommand = XtVaCreateManagedWidget(
      "multCommand", commandWidgetClass, parent,
      XtNlabel, "multiply",
      NULL);
  XtAddCallback(multCommand, XtNcallback, multScalars, NULL);

  divCommand = XtVaCreateManagedWidget(
      "divCommand", commandWidgetClass, parent,
      XtNlabel, "divide",
      NULL);
  XtAddCallback(divCommand, XtNcallback, divScalars, NULL);

  replaceCommand = XtVaCreateManagedWidget(
      "replaceCommand", commandWidgetClass, parent,
      XtNlabel, "replace...",
      NULL);
  XtAddCallback(replaceCommand, XtNcallback, replaceScalars, NULL);
}         /* addOperationWidgets */


#define NUM_FILEMENU_ITEMS 8

/****************************************************************/
/* Fuege dem Widget w das File Menu als Kind hinzu.             */
/****************************************************************/
static void addFileMenu(Widget w)
{ int i;
  Widget entry;
  static String fileMenuItems[] = {
    "fileLoadPatterns",
    "fileWritePatterns",
    "line",
    "fileLoadSymbols",
    "fileWriteSymbols",
    "fileWriteToLogFile",
    "line",
    "fileQuit",
  };
  static String itemLabels[] = {
    "Load patterns...",
    "Write patterns...",
    "",
    "Load symbols...",
    "Write symbols...",
    "Write to log file",
    "",
    "Quit"
  };

  fileMenuButton = XtVaCreateManagedWidget(
     "fileMenuButton", menuButtonWidgetClass, w,
     XtNlabel, "File",
     XtNmenuName, "fileMenu",
     NULL);                 /* terminate list */

  fileMenu = XtCreatePopupShell(
     "fileMenu",                /* name */
     simpleMenuWidgetClass,     /* class */
     fileMenuButton,            /* parent */
     NULL, 0);

  for(i = 0; i < NUM_FILEMENU_ITEMS; i++)
  {
    String item = fileMenuItems[i];

    if(strcmp(item, "line"))
    { entry = XtVaCreateManagedWidget(
        item, smeBSBObjectClass, fileMenu,
        XtNlabel, itemLabels[i],
        XtNleftMargin, MENU_LEFTMARGIN,
        XtNrightMargin, MENU_RIGHTMARGIN,
        NULL);
      XtAddCallback(entry, XtNcallback, fileMenuSelect, (XtPointer) i);
    }
    else
      entry = XtVaCreateManagedWidget(
        item, smeLineObjectClass, fileMenu,
        NULL);
  }   /* for */

  if(app_data.log)
    XtVaSetValues(XtNameToWidget(fileMenu, "fileWriteToLogFile"),
        XtNleftBitmap, hook, NULL);
}     /* addFileMenu */


#define NUM_TRANSFORMMENU_ITEMS 9

/****************************************************************/
/* Fuege dem Widget w das Transform-Menu als Kind hinzu.        */
/****************************************************************/
static void addTransformMenu(Widget w)
{ int i;
  Widget entry;
  static String transformMenuItems[] = {
    "transformHLOG",
    "transformFFT",
    "transformPCA",
    "transformNormalize",
    "transformScale",
    "transformRandomize",
    "transformExpandWithClass",
    "transformExpandWithOutput",
    "transformRefreshClassNumbers"
  };
  static String itemLabels[] = {
    "HLOG",
    "FFT",
    "PCA",
    "Normalize",
    "Scale...",
    "Randomize",
    "Expand with class vector",
    "Expand with output vector",
    "Refresh class numbers"
  };

  transformMenuButton = XtVaCreateManagedWidget(
     "transformMenuButton", menuButtonWidgetClass, w,
     XtNlabel, "Transform",
     XtNmenuName, "transformMenu",
     NULL);

  transformMenu = XtCreatePopupShell(
     "transformMenu", simpleMenuWidgetClass, transformMenuButton,
     NULL, 0);

  for(i = 0; i < NUM_TRANSFORMMENU_ITEMS; i++)
  {
    String item = transformMenuItems[i];

    if(strcmp(item, "line"))
    { entry = XtVaCreateManagedWidget(
        item, smeBSBObjectClass, transformMenu,
        XtNlabel, itemLabels[i],
        XtNleftMargin, MENU_LEFTMARGIN,
        XtNrightMargin, MENU_RIGHTMARGIN,
        NULL);
      XtAddCallback(entry, XtNcallback, transformMenuSelect, (XtPointer) i);
    }
    else
      entry = XtVaCreateManagedWidget(
        item, smeLineObjectClass, transformMenu,
        NULL);
  }   /* for */
}     /* addTransformMenu */


#define NUM_REMOVEMENU_ITEMS 3

/****************************************************************/
/* Fuege dem Widget w das Remove-Menu als Kind hinzu.           */
/****************************************************************/
static void addRemoveMenu(Widget w)
{ int i;
  Widget entry;
  static String removeMenuItems[] = {
    "removeDimensions",
    "removeConstantDims",
    "removeVectors"
  };
  static String itemLabels[] = {
    "Dimensions...",
    "Constant dims",
    "Vectors...",
  };

  removeMenuButton = XtVaCreateManagedWidget(
     "removeMenuButton", menuButtonWidgetClass, w,
     XtNlabel, "Remove",
     XtNmenuName, "removeMenu",
     NULL);

  removeMenu = XtCreatePopupShell(
     "removeMenu", simpleMenuWidgetClass, removeMenuButton,
     NULL, 0);

  for(i = 0; i < NUM_REMOVEMENU_ITEMS; i++)
  {
    String item = removeMenuItems[i];

    if(strcmp(item, "line"))
    { entry = XtVaCreateManagedWidget(
        item, smeBSBObjectClass, removeMenu,
        XtNlabel, itemLabels[i],
        XtNleftMargin, MENU_LEFTMARGIN,
        XtNrightMargin, MENU_RIGHTMARGIN,
        NULL);
      XtAddCallback(entry, XtNcallback, removeMenuSelect, (XtPointer) i);
    }
    else
      entry = XtVaCreateManagedWidget(
        item, smeLineObjectClass, removeMenu,
        NULL);
  }   /* for */
}     /* addRemoveMenu */


#define NUM_SHOWMENU_ITEMS 4

/****************************************************************/
/* Fuege dem Widget w das Show-Menu als Kind hinzu.             */
/****************************************************************/
static void addShowMenu(Widget w)
{ int i;
  Widget entry;
  static String showMenuItems[] = {
    "showGraphics",
    "showStatistics",
    "showCovariances",
    "showReadShell"
  };
  static String itemLabels[] = {
    "Graphics",
    "Statistics",
    "Covariance matrix",
    "Input/output vectors"
  };

  showMenuButton = XtVaCreateManagedWidget(
     "showMenuButton", menuButtonWidgetClass, w,
     XtNlabel, "Show",
     XtNmenuName, "showMenu",
     NULL);

  showMenu = XtCreatePopupShell(
     "showMenu", simpleMenuWidgetClass, showMenuButton,
     NULL, 0);

  for(i = 0; i < NUM_SHOWMENU_ITEMS; i++)
  {
    String item = showMenuItems[i];

    if(strcmp(item, "line"))
    { entry = XtVaCreateManagedWidget(
        item, smeBSBObjectClass, showMenu,
        XtNlabel, itemLabels[i],
        XtNleftMargin, MENU_LEFTMARGIN,
        XtNrightMargin, MENU_RIGHTMARGIN,
        NULL);
      XtAddCallback(entry, XtNcallback, showMenuSelect, (XtPointer) i);
    }
    else
      entry = XtVaCreateManagedWidget(
        item, smeLineObjectClass, showMenu,
        NULL);
  }   /* for */
}     /* addShowMenu */


#define NUM_LOADMENU_ITEMS 12

/***********************************************/
/* Fuege das 'Load'-Menu mit all seinen Menu-  */
/* Items dem Widget w als Kind hinzu.          */
/***********************************************/
static void addLoadMenu(Widget parent)
{ int i;
  Widget entry, loadMenu;

  static String loadMenuItems[] = {
    "loadMinimum",
    "loadMaximum",
    "loadAverage",
    "loadLength",
    "loadStandardDeviation",
    "loadSum",
    "loadPattern",
    "loadOverallMinimum",
    "loadOverallMaximum",
    "loadOverallAverage",
    "loadOverallStdDev",
    "loadConstant",
  };
  static String itemLabels[] = {
    "minimum",
    "maximum",
    "average",
    "length",
    "standard deviation",
    "sum",
    "pattern...",
    "overall minimum",
    "overall maximum",
    "overall average",
    "overall std dev",
    "constant...",
  };

  loadMenu = XtCreatePopupShell(
     "loadMenu", simpleMenuWidgetClass, parent,
     NULL, 0);

  for(i = 0; i < NUM_LOADMENU_ITEMS; i++)
  {
    String item = loadMenuItems[i];

    entry = XtVaCreateManagedWidget(
      item, smeBSBObjectClass, loadMenu,
      XtNlabel, itemLabels[i],
      XtNleftMargin, MENU_LEFTMARGIN,
      XtNrightMargin, MENU_RIGHTMARGIN,
      NULL);
    XtAddCallback(entry, XtNcallback, loadMenuSelect, (XtPointer) i);
  }   /* for */
}         /* addLoadMenu */


/******************************************************/
/* Fuege dem Widget w das Format-Menu als Kind hinzu. */
/* Die Anzahl der Menu-Items und deren Labels sind    */
/* durch die Variable formatNames gegeben, die vor    */
/* Aufruf der Funktion initialisiert sein muss.       */
/* Gleichzeitig wird die Variable formatMenuItems     */
/* initialisiert.                                     */
/* Variable error wird gesetzt.                       */
/******************************************************/
static void addFormatMenu(Widget w)
{
  formatMenuButton = XtVaCreateManagedWidget(
     "formatMenuButton", menuButtonWidgetClass, w,
     XtNlabel, "Format",
     XtNmenuName, "formatMenu",
     NULL);

  formatMenu = XtCreatePopupShell(
     "formatMenu", simpleMenuWidgetClass, formatMenuButton,
     NULL, 0);

  if(! (formatMenuItems = newColl())) error(1);

  addMenuItems(formatMenu, formatNames, formatMenuSelect, formatMenuItems);
  if(error) return;

  error = 0;
}     /* addFormatMenu */


/******************************************************/
/* Fuege dem SimpleMenu sm die Menu Items hinzu,      */
/* deren Namen in der Collection labels enthalten     */
/* sind. Die Funktion callback soll aufgerufen werden */
/* wenn irgendeiner der Items ausgewaehlt wird.       */
/* Fuege die neu erzeugten smeBSBObjects zu widgetColl*/
/* hinzu.                                             */
/* Setze Variable error.                              */
/******************************************************/
static void addMenuItems(sm, labels, callback, widgetColl)
Widget sm;
Collection labels, widgetColl;
void (*callback)();
{ int i, num = size(labels);
  String itemName;
  Widget entry;
  Pixmap left;

  for(i = 1; i <= num; i++)
  {
    itemName = (String) at(labels, i);
    left = (i == selectedFormat ? hook : None);
    entry = XtVaCreateManagedWidget(
      itemName, smeBSBObjectClass, sm,
      XtNlabel, itemName,
      XtNleftBitmap, left,
      XtNleftMargin, MENU_LEFTMARGIN,
      XtNrightMargin, MENU_RIGHTMARGIN,
      NULL);
    XtAddCallback(entry, XtNcallback, callback, (XtPointer) i);
    if(! add(widgetColl, entry)) error(1);
  }    /* for */

  error = 0;
}      /* addMenuItems */


/*************************************************************/
/* Antworte eine Popup-Shell fuer den Fehlerdialog.          */
/* Das Label Widget fuer den Text der Fehlermeldung wird     */
/* noch nicht gesetzt.                                       */
/*************************************************************/
static Widget createErrDlgShell(Widget parent)
{ Widget errDlgForm, errDlgOk, answer;

  answer = XtVaCreatePopupShell(
      "errorinfo", transientShellWidgetClass, parent,
       XtNtitle, "vistra error",
       XtNallowShellResize, TRUE,
       NULL);

  errDlgForm = XtVaCreateManagedWidget(
      "errDlgForm", formWidgetClass, answer,
      NULL);

  errDlgLabel = XtVaCreateManagedWidget(
      "errDlgLabel", labelWidgetClass, errDlgForm,
      XtNjustify, XtJustifyLeft,
      XtNborderWidth, 0,
      XtNfont, app_data.numberFont,
      XtNvertDistance, 10,
      XtNfromVert, NULL,
      XtNhorizDistance, 10,
      XtNfromHoriz, NULL,
      XtNtop, XtChainTop,
      XtNbottom, XtChainBottom,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight, 
      XtNresizable, TRUE,
      NULL);

  errDlgOk = XtVaCreateManagedWidget(
      "errDlgOk", commandWidgetClass, errDlgForm,
      XtNlabel, "OK",
      XtNvertDistance, 8,
      XtNfromVert, errDlgLabel,
      XtNhorizDistance, 8,
      XtNfromHoriz, NULL,
      XtNtop, XtChainBottom,
      XtNbottom, XtChainBottom,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(errDlgOk, XtNcallback, popdown, answer);

  return answer;
}         /* createErrDlgShell */


/***********************************************/
/* Kreiere und antworte eine neue Popup-Shell  */
/* fuer eine Dialogbox, bei der der Benutzer   */
/* 2 Strings eingibt. Der Benutzer kann den    */
/* Vorgang ueber einen 'Cancel'-Button         */
/* abbrechen.                                  */
/***********************************************/
static Widget createStr2DlgShell(Widget parent)
{ Widget answer, str2Form, str2Cancel;

  answer = XtVaCreatePopupShell(
      "str2Dlg", transientShellWidgetClass, parent,
      XtNallowShellResize, TRUE,
      NULL);
  str2Form = XtVaCreateManagedWidget(
      "str2Form", formWidgetClass, answer,
      NULL);
  str2Headline = XtVaCreateManagedWidget(
      "str2Headline", labelWidgetClass, str2Form,
      XtNjustify, XtJustifyLeft,
      XtNresize, TRUE,
      XtNfromVert, NULL,
      XtNvertDistance, STR2_TOPMARGIN,
      XtNfromHoriz, NULL,
      XtNhorizDistance, STR2_LEFTMARGIN,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNborderWidth, 0,
      NULL);
  str2Label1 = XtVaCreateManagedWidget(
      "str2Label1", labelWidgetClass, str2Form,
      XtNjustify, XtJustifyLeft,
      XtNwidth, STR2_LABELWIDTH,
      XtNfromVert, str2Headline,
      XtNvertDistance, STR2_HDLINE_LABEL_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, STR2_LEFTMARGIN,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNborderWidth, 0,
      NULL);
  str2Label2 = XtVaCreateManagedWidget(
      "str2Label2", labelWidgetClass, str2Form,
      XtNjustify, XtJustifyLeft,
      XtNwidth, STR2_LABELWIDTH,
      XtNfromVert, str2Label1,
      XtNvertDistance, STR2_DIST_BETWEEN_LABELS,
      XtNfromHoriz, NULL,
      XtNhorizDistance, STR2_LEFTMARGIN,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNborderWidth, 0,
      NULL);
  str2Text1 = XtVaCreateManagedWidget(
      "str2Text1", asciiTextWidgetClass, str2Form,
      XtNdisplayCaret, TRUE,
      XtNeditType, XawtextEdit,
      XtNwidth, STR2_TEXTWIDTH,
      XtNfromVert, str2Headline,
      XtNvertDistance, STR2_HDLINE_LABEL_DIST,
      XtNfromHoriz, str2Label1,
      XtNhorizDistance, STR2_LABEL_TEXT_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  XtOverrideTranslations(str2Text1, textTransl);

  str2Text2 = XtVaCreateManagedWidget(
      "str2Text2", asciiTextWidgetClass, str2Form,
      XtNdisplayCaret, TRUE,
      XtNeditType, XawtextEdit,
      XtNwidth, STR2_TEXTWIDTH,
      XtNfromVert, str2Label1,
      XtNvertDistance, STR2_DIST_BETWEEN_LABELS,
      XtNfromHoriz, str2Label1,
      XtNhorizDistance, STR2_LABEL_TEXT_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  XtOverrideTranslations(str2Text2, textTransl);

  str2Ok = XtVaCreateManagedWidget(
      "str2Ok", commandWidgetClass, str2Form,
      XtNresizable, TRUE,
      XtNfromVert, str2Label2,
      XtNvertDistance, STR2_LABEL_COMMAND_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, STR2_LEFTMARGIN,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  str2Cancel = XtVaCreateManagedWidget(
      "str2Cancel", commandWidgetClass, str2Form,
      XtNlabel, "cancel",
      XtNresizable, TRUE,
      XtNfromVert, str2Label2,
      XtNvertDistance, STR2_LABEL_COMMAND_DIST,
      XtNfromHoriz, str2Ok,
      XtNhorizDistance, STR2_DIST_BETWEEN_COMMANDS,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(str2Cancel, XtNcallback, popdown, answer);

  return answer;
}         /* createStr2DlgShell */


/******************************************************/
/******************************************************/
static Widget createTextWindow(title, text, width, height, parent, callback)
char *title, *text;
unsigned width, height;
Widget parent;
void (*callback)();
{ Widget answer, twForm, twText, twCloseButton;

  answer = XtVaCreatePopupShell(
      "readShell", transientShellWidgetClass, parent,
      XtNwidth, width,
      XtNheight, height,
      XtNtitle, title,
      NULL);
  twForm = XtVaCreateManagedWidget(
      "twForm", formWidgetClass, answer,
      NULL);
  twText = XtVaCreateManagedWidget(
      "twText", asciiTextWidgetClass, twForm,
      XtNfont, app_data.numberFont,
      XtNstring, text,
      XtNvertDistance, 0,
      XtNfromVert, NULL,
      XtNhorizDistance, 0,
      XtNfromHoriz, NULL,
      XtNtop, XtChainTop,
      XtNbottom, XtChainBottom,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      XtNdisplayCaret, FALSE,
      XtNeditType, XawtextRead,
      XtNscrollHorizontal, XawtextScrollWhenNeeded,
      XtNscrollVertical, XawtextScrollWhenNeeded,
      XtNtype, XawAsciiString,
      NULL);
  twCloseButton = XtVaCreateManagedWidget(
      "twCloseButton", commandWidgetClass, twForm,
      XtNlabel, "close",
      XtNvertDistance, 8,
      XtNfromVert, twText,
      XtNhorizDistance, 8,
      XtNfromHoriz, NULL,
      XtNtop, XtChainBottom,
      XtNbottom, XtChainBottom,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNresizable, TRUE,
      NULL);
  XtAddCallback(twCloseButton, XtNcallback, callback, answer);

  return answer;
}     /* createTextWindow */


/*******************************************************/
/*******************************************************/
static void noop(Widget w, XEvent *e, String *s, Cardinal *c)
{
}     /* noop */


#ifdef XFWF_FILE_SELECTION
/*******************************************************/
/*******************************************************/
static Widget createFileSelShell(Widget parent)
{ Widget answer;

  answer = XtVaCreatePopupShell(
            "fileSelShell", transientShellWidgetClass, parent,
            NULL);
  fileSelector = XtVaCreateManagedWidget(       
            "fileSelector", xfwfFileSelectorWidgetClass, answer,
            XtNtitle, "vistra file selector", 
            XtNheight, 400,
            NULL);
  XtAddCallback(fileSelector, XtNcancelButtonCallback,
      (XtCallbackProc) popdown, (XtPointer) answer);

  return answer;
}       /* createFileSelShell */       
#endif
