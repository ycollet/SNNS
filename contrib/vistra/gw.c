#define PIXMAP_MAXWIDTH             1500
#define PIXMAP_MAXHEIGHT            1000
#define PM_LEFTMARGIN               48
#define PM_TOPMARGIN                48 
#define LINE_WIDTH                  1
#define X_DIST_FROM_EDGE            16    /* minimaler Abstand der Koor- */
#define Y_DIST_FROM_EDGE            16    /* dinatenachsen von den Raendern.*/
#define VECS_PER_ROW                100
#define POLY_HISTO_HEIGHT           100
#define DRAWFONT_WIDTH              6
#define DRAWFONT_HEIGHT             10
#define DRAW_NR_LEFTMARGIN          0
#define DRAW_NR_BOTTOMMARGIN        6
#define GW_SHOW_INPUT               1
#define GW_SHOW_OUTPUT              2
#define GW_VP_WIDTH                 600
#define GW_VP_HEIGHT                400
#define GW_GROW_FACTOR              0.5 
#define GW_WIDECOMM_WIDTH           162
#define GW_SMALLCOMM_WIDTH          77
#define GW_COMM_HORIZ_DIST          8
#define GW_COMM_VERT_DIST           8
#define GW_MINHEIGHT                425

#define maxDim(gw)         ((gw)->input ? inputDims(pats) : outputDims(pats))
#define displayedVecs(gw)  ((gw)->input ? inputs(pats) : outputs(pats)) 
#define selMin(gw)         ((gw)->input ? (gw)->minInputs : (gw)->minOutputs)
#define selMax(gw)         ((gw)->input ? (gw)->maxInputs : (gw)->maxOutputs)
#define allInOne(kind)     ((kind) == proj2D)

/* Local to GW.C */
static void initGW(GW);
static void setGWToggles(GW, enum Graphic);
static void setGWActivation(GW);
static void updateGW(GW);
static void computeMinMax(GW);
static void computeExtents(GW);
static Boolean notPrintableGW(GW);
/* Local Callbacks */
static void gwCommClose(Widget, XtPointer, XtPointer);
static void gwCommNext(Widget, XtPointer, XtPointer);
static void gwCommPrevious(Widget, XtPointer, XtPointer);
static void gwCommFirst(Widget, XtPointer, XtPointer);
static void gwCommLast(Widget, XtPointer, XtPointer);
static void gwCommGoto(Widget, XtPointer, XtPointer);
static void gwCommGrow(Widget, XtPointer, XtPointer);
static void gwCommShrink(Widget, XtPointer, XtPointer);
static void gwCommResize(Widget, XtPointer, XtPointer);
static void gwCommFillUp(Widget, XtPointer, XtPointer);
static void gwCommRedisplay(Widget, XtPointer, XtPointer);
static void gwCommHisto(Widget, XtPointer, XtPointer);
static void gwCommPoly(Widget, XtPointer, XtPointer);
static void gwCommGrayMat(Widget, XtPointer, XtPointer);
static void gwCommClrMat(Widget, XtPointer, XtPointer);
static void gwComm2DProj(Widget, XtPointer, XtPointer);
static void gwShowGrayMat(Widget, XtPointer, XtPointer);
static void gwShowClrMat(Widget, XtPointer, XtPointer);
static void gwShow2DProj(Widget, XtPointer, XtPointer);
static void gwInOutToggle(Widget, XtPointer, XtPointer);
static void gwPopdownInt2(Widget, XtPointer, XtPointer);
/* Local Actions */
static void redrawGW(Widget, XtPointer, XEvent *, Boolean *);

static unsigned gwWidths[][2] = {
  { 64, 1000, },                  /* Groessen der Darstellungsarten:     */
  { 1, 40, },                     /*  - (Minimalgroesse, Maximalgroesse) */
  { 1, 40, },
  { 1, 40, },
  { 1, 40, },
};

static unsigned gwDists[] = {
  48,      /* proj2D */
  48,      /* histo */
  48,      /* poly */
  24,      /* clrMat */
  24,      /* grayMat */
};      

#include "draw.c" 

/********************************************************/
/* Oeffne ein neues Graphik-Fenster.                    */
/* Antworte NULL, falls dies nicht moeglich ist (weil   */
/* zu wenig Speicher vorhanden).                        */
/********************************************************/
static GW openGW(Widget parent, char *title)
{ GW answer;
  Widget gwForm, gwLabelVector, gwNrLabel, gwLabelSize, gwSizeLabel, 
         gwViewport, gwInputToggle, gwOutputToggle,
         gwResize, gwGoto, gwResizeText, gwGotoText, gwLabelPercent,
         gwNext, gwPrevious, gwGrow, gwShrink, gwFirst, gwLast, 
         gwFillUp, gwRedisplay, gwClose, gwHisto, gwPoly,
         gw2DProj, gwGrayMat, gwClrMat;
  char buf[10];

  if(! (answer = (GW) malloc(sizeof(*answer)))) return NULL;

  answer->shell = XtVaCreatePopupShell(
      "gwShell", transientShellWidgetClass, parent,
      XtNtitle, title,
      XtNminWidth, GW_WIDECOMM_WIDTH + 4 * GW_COMM_HORIZ_DIST,
      XtNminHeight, GW_MINHEIGHT,
      NULL);
  gwForm = XtVaCreateManagedWidget(
      "gwForm", formWidgetClass, answer->shell,
      NULL);
  answer->infoForm = XtVaCreateManagedWidget(
      "gwInfoForm", formWidgetClass, gwForm,
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, NULL,
      XtNhorizDistance, 0,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNborderWidth, 0,
      NULL);
  gwLabelVector = XtVaCreateManagedWidget(
      "gwLabelVector", labelWidgetClass, answer->infoForm,
      XtNborderWidth, 0,
      XtNlabel, "Displayed pattern(s):",
      XtNfont, gwFont,
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  gwNrLabel = XtVaCreateManagedWidget(
      "gwNrLabel", labelWidgetClass, answer->infoForm,
      XtNborderWidth, 0,
      XtNfont, gwFont,
      XtNfromVert, NULL,
      XtNresize, TRUE,
      XtNresizable, TRUE,
      XtNvertDistance, 0,
      XtNfromHoriz, gwLabelVector,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  gwLabelSize = XtVaCreateManagedWidget(
      "gwLabelSize", labelWidgetClass, answer->infoForm,
      XtNborderWidth, 0,
      XtNlabel, "Size:",
      XtNfont, gwFont,
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, gwNrLabel,
      XtNhorizDistance, 4 * GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  gwSizeLabel = XtVaCreateManagedWidget(
      "gwSizeLabel", labelWidgetClass, answer->infoForm,
      XtNborderWidth, 0,
      XtNfont, gwFont,
      XtNresize, TRUE,
      XtNresizable, TRUE, 
      XtNfromVert, NULL,
      XtNvertDistance, 0,
      XtNfromHoriz, gwLabelSize,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  answer->commForm = XtVaCreateManagedWidget(
      "gwCommForm", formWidgetClass, gwForm,
      XtNfromVert, answer->infoForm,
      XtNvertDistance, 0,
      XtNfromHoriz, NULL,
      XtNhorizDistance, 0,
      XtNtop, XtChainTop,
      XtNbottom, XtChainBottom,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNborderWidth, 0,
      NULL);
  gwInputToggle = XtVaCreateManagedWidget(
      "gwInputToggle", toggleWidgetClass, answer->commForm,
      XtNlabel, "input",
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter,
      XtNradioData, (XtPointer) GW_SHOW_INPUT,
      XtNstate, TRUE,
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, NULL,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  gwOutputToggle = XtVaCreateManagedWidget(
      "gwOutputToggle", toggleWidgetClass, answer->commForm,
      XtNlabel, "output",
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter,
      XtNradioData, (XtPointer) GW_SHOW_OUTPUT,
      XtNradioGroup, gwInputToggle,
      XtNstate, FALSE,
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, NULL,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, gwInputToggle,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtOverrideTranslations(gwInputToggle, toggleTransl);
  XtOverrideTranslations(gwOutputToggle, toggleTransl);
  XtAddCallback(gwInputToggle, XtNcallback, 
                gwInOutToggle, (XtPointer) answer);
  XtAddCallback(gwOutputToggle, XtNcallback, 
                gwInOutToggle, (XtPointer) answer);

  gwGoto = XtVaCreateManagedWidget(
      "gwGoto", commandWidgetClass, answer->commForm,
      XtNlabel, "goto nr ->",
      XtNjustify, XtJustifyCenter,
      XtNfont, gwFont,
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwInputToggle,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL); 
  XtAddCallback(gwGoto, XtNcallback, gwCommGoto, (XtPointer) answer);

  gwGotoText = XtVaCreateManagedWidget( 
      "gwGotoText", asciiTextWidgetClass, answer->commForm,
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwInputToggle,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, gwGoto,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNdisplayCaret, TRUE,
      XtNeditType, XawtextEdit,
      XtNscrollVertical, XawtextScrollNever,
      XtNscrollHorizontal, XawtextScrollNever,
      XtNtype, XawAsciiString,
      XtNfont, gwFont,
      XtNstring, "", 
      NULL);
  XtOverrideTranslations(gwGotoText, textTransl);

  gwFirst = XtVaCreateManagedWidget(
      "gwFirst", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNlabel, "first",
      XtNjustify, XtJustifyCenter, 
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwGoto,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL, 
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwFirst, XtNcallback, gwCommFirst, (XtPointer) answer);

  gwLast = XtVaCreateManagedWidget(
      "gwLast", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter, 
      XtNlabel, "last",
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwGoto,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, gwFirst, 
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwLast, XtNcallback, gwCommLast, (XtPointer) answer);
  
  gwPrevious = XtVaCreateManagedWidget(
      "gwPrevious", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter, 
      XtNlabel, "previous",
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwFirst,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL, 
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwPrevious, XtNcallback, gwCommPrevious, (XtPointer) answer);
  
  gwNext = XtVaCreateManagedWidget(
      "gwNext", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNlabel, "next",
      XtNjustify, XtJustifyCenter, 
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwFirst,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, gwPrevious, 
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwNext, XtNcallback, gwCommNext, (XtPointer) answer);

  gwResize = XtVaCreateManagedWidget(
      "gwResize", commandWidgetClass, answer->commForm,
      XtNlabel, "size to ->",
      XtNjustify, XtJustifyCenter,
      XtNfont, gwFont,
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwNext,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL); 
  XtAddCallback(gwResize, XtNcallback, gwCommResize, (XtPointer) answer);

  gwResizeText = XtVaCreateManagedWidget( 
      "gwResizeText", asciiTextWidgetClass, answer->commForm,
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwNext,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, gwResize,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNdisplayCaret, TRUE,
      XtNeditType, XawtextEdit,
      XtNscrollVertical, XawtextScrollNever,
      XtNscrollHorizontal, XawtextScrollNever,
      XtNtype, XawAsciiString,
      XtNfont, gwFont,
      XtNstring, "",
      NULL);
  XtOverrideTranslations(gwResizeText, textTransl);

  gwLabelPercent = XtVaCreateManagedWidget(
      "gwLabelPercent", labelWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNjustify, XtJustifyLeft,
      XtNlabel, "%",
      XtNborderWidth, 0,
      XtNfromVert, gwNext,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, gwResizeText,
      XtNhorizDistance, 0,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  gwGrow = XtVaCreateManagedWidget(
      "gwGrow", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter, 
      XtNlabel, "grow",
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwResize,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL, 
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwGrow, XtNcallback, gwCommGrow, (XtPointer) answer);

  gwShrink = XtVaCreateManagedWidget(
      "gwShrink", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNlabel, "shrink",
      XtNjustify, XtJustifyCenter, 
      XtNwidth, GW_SMALLCOMM_WIDTH,
      XtNfromVert, gwResize,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, gwGrow, 
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwShrink, XtNcallback, gwCommShrink, (XtPointer) answer);
  
  gwFillUp = XtVaCreateManagedWidget(
      "gwFillUp", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNlabel, "fill up",
      XtNwidth, GW_WIDECOMM_WIDTH,
      XtNjustify, XtJustifyCenter,
      XtNfromVert, gwGrow,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwFillUp, XtNcallback, gwCommFillUp, (XtPointer) answer);

  gwRedisplay = XtVaCreateManagedWidget(
      "gwRedisplay", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNlabel, "redisplay",
      XtNwidth, GW_WIDECOMM_WIDTH,
      XtNjustify, XtJustifyCenter,
      XtNfromVert, gwFillUp,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL, 
      XtNhorizDistance, GW_COMM_HORIZ_DIST,   
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwRedisplay, XtNcallback, gwCommRedisplay, (XtPointer) answer); 

  gwClose = XtVaCreateManagedWidget(
      "gwClose", commandWidgetClass, answer->commForm,
      XtNfont, gwFont,
      XtNlabel, "close",
      XtNwidth, GW_WIDECOMM_WIDTH,
      XtNjustify, XtJustifyCenter,
      XtNfromVert, gwRedisplay,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL, 
      XtNhorizDistance, GW_COMM_HORIZ_DIST,   
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(gwClose, XtNcallback, gwCommClose, (XtPointer) answer); 

  gwHisto = XtVaCreateManagedWidget(
      "gwHisto", toggleWidgetClass, answer->commForm,
      XtNlabel, "histogram",
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter,
      XtNstate, FALSE,
      XtNwidth, GW_WIDECOMM_WIDTH,
      XtNfromVert, gwClose,
      XtNvertDistance, 3 * GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  XtAddCallback(gwHisto, XtNcallback, gwCommHisto, (XtPointer) answer);
  XtOverrideTranslations(gwHisto, toggleTransl); 

  gwPoly = XtVaCreateManagedWidget(
      "gwPoly", toggleWidgetClass, answer->commForm,
      XtNlabel, "poly line",
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter,
      XtNstate, FALSE,
      XtNwidth, GW_WIDECOMM_WIDTH,
      XtNfromVert, gwHisto,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  XtAddCallback(gwPoly, XtNcallback, gwCommPoly, (XtPointer) answer);
  XtOverrideTranslations(gwPoly, toggleTransl); 

  gwGrayMat = XtVaCreateManagedWidget(
      "gwGrayMat", toggleWidgetClass, answer->commForm,
      XtNlabel, "gray matrix...",
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter,
      XtNstate, FALSE,
      XtNwidth, GW_WIDECOMM_WIDTH,
      XtNfromVert, gwPoly,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  XtAddCallback(gwGrayMat, XtNcallback, gwCommGrayMat, (XtPointer) answer);
  XtOverrideTranslations(gwGrayMat, toggleTransl); 

  gwClrMat = XtVaCreateManagedWidget(
      "gwClrMat", toggleWidgetClass, answer->commForm,
      XtNlabel, "color matrix...",
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter,
      XtNstate, FALSE,
      XtNwidth, GW_WIDECOMM_WIDTH,
      XtNfromVert, gwGrayMat,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  XtAddCallback(gwClrMat, XtNcallback, gwCommClrMat, (XtPointer) answer);
  XtOverrideTranslations(gwClrMat, toggleTransl); 

  gw2DProj = XtVaCreateManagedWidget(
      "gw2DProj", toggleWidgetClass, answer->commForm,
      XtNlabel, "2D projection...",
      XtNfont, gwFont,
      XtNjustify, XtJustifyCenter,
      XtNstate, FALSE,
      XtNwidth, GW_WIDECOMM_WIDTH,
      XtNfromVert, gwClrMat,
      XtNvertDistance, GW_COMM_VERT_DIST,
      XtNfromHoriz, NULL,
      XtNhorizDistance, GW_COMM_HORIZ_DIST,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      NULL);
  XtAddCallback(gw2DProj, XtNcallback, gwComm2DProj, (XtPointer) answer);
  XtOverrideTranslations(gw2DProj, toggleTransl); 
 
  gwViewport = XtVaCreateManagedWidget(
      "gwViewport", viewportWidgetClass, gwForm,
      XtNvertDistance, 0,
      XtNfromVert, answer->infoForm,
      XtNhorizDistance, 0,
      XtNfromHoriz, answer->commForm,
      XtNtop, XtChainTop,
      XtNbottom, XtChainBottom,
      XtNleft, XtChainLeft,
      XtNright, XtChainRight,
      XtNforceBars, TRUE,
      XtNallowHoriz, TRUE,
      XtNallowVert, TRUE,
      XtNuseRight, TRUE,
      XtNuseBottom, TRUE,
      XtNwidth, GW_VP_WIDTH,
      XtNheight, GW_VP_HEIGHT,
      NULL);
  answer->core = XtVaCreateManagedWidget(
      "gwCore", coreWidgetClass, gwViewport,
      NULL);
  XtAddEventHandler(answer->core, ExposureMask, FALSE, 
      redrawGW, (XtPointer) answer);
 
  computeMinMax(answer);
  answer->patVersion = lastChanged;

  XtPopup(answer->shell, XtGrabNone);
  initGW(answer);      
 
  return answer;
}     /* openGW */


/*********************************************************/
/* Berechne globale Minima und Maxima der Patterns pats. */
/*********************************************************/
static void computeMinMax(GW gw)
{
  gw->minInputs = overallMin(inputs(pats));
  gw->maxInputs = overallMax(inputs(pats));
  if(hasOutputs(pats))
  { gw->minOutputs = overallMin(outputs(pats));
    gw->maxOutputs = overallMax(outputs(pats));
  }
}      /* computeMinMax */ 


/*********************************************************/
/*********************************************************/
static void initGW(GW gw)
{ char buf[10];

  gw->kind = noKind;
  gw->pm = NULL;
  gw->pmWidth = 0;
  gw->pmHeight = 0;
  gw->curWidth = 0;
  gw->displayed = 1;
  gw->input = TRUE;
  gw->size = 0.1;

  XClearWindow(display, XtWindow(gw->core));
  sprintf(buf, "%u%%", (unsigned) (gw->size * 100.0));
  XtVaSetValues(XtNameToWidget(gw->infoForm, "gwSizeLabel"),
                XtNlabel, buf, NULL);
  XtVaSetValues(XtNameToWidget(gw->infoForm, "gwNrLabel"), 
                XtNlabel, "1", NULL);
  XawToggleSetCurrent(XtNameToWidget(gw->commForm, "gwInputToggle"),
                (XtPointer) GW_SHOW_INPUT);                
  setGWActivation(gw);
  setGWToggles(gw, noKind);
}         /* initGW */


/****************************************************/
/* Schliesse das Graphik-Fenster, das durch         */
/* client_data uebergeben wird und gebe saemtliche  */
/* Ressourcen frei.                                 */
/****************************************************/
static void gwCommClose(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  long pos;
  
  rmv(openGWs, gw);
  freeGW(gw);
}           /* gwCommClose */


/****************************************************/
/* Gebe alle Ressourcen von gw frei.                */
/****************************************************/
static void freeGW(GW gw)
{
  if(gw->pm) XFreePixmap(display, gw->pm);
  XtDestroyWidget(gw->shell);
  free(gw);
}           /* freeGW */


/****************************************************/
/****************************************************/
static void setGWToggles(GW gw, enum Graphic kind)
{
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwHisto"),
                XtNstate, kind == histo, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwPoly"),
                XtNstate, kind == poly, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwGrayMat"),
                XtNstate, kind == grayMat, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwClrMat"),
                XtNstate, kind == clrMat, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gw2DProj"),
                XtNstate, kind == proj2D, NULL);
}       /* setGWToggles */


/****************************************************/
/****************************************************/
static void setGWActivation(GW gw)
{ Boolean kindChosen, outputsExist, displayAll;
  
  kindChosen = (gw->kind != noKind); 
  displayAll = allInOne(gw->kind);
  outputsExist = hasOutputs(pats);

  XtVaSetValues(XtNameToWidget(gw->commForm, "gwOutputToggle"),
      XtNsensitive, outputsExist, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwNext"),
      XtNsensitive, kindChosen && !displayAll, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwPrevious"),
      XtNsensitive, kindChosen && !displayAll, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwFirst"),
      XtNsensitive, kindChosen && !displayAll, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwLast"),
      XtNsensitive, kindChosen && !displayAll, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwGoto"),
      XtNsensitive, kindChosen && !displayAll, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwResize"),
      XtNsensitive, kindChosen, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwGrow"),
      XtNsensitive, kindChosen, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwShrink"),
      XtNsensitive, kindChosen, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwFillUp"),
      XtNsensitive, kindChosen && !displayAll, NULL);
  XtVaSetValues(XtNameToWidget(gw->commForm, "gwRedisplay"),
      XtNsensitive, kindChosen, NULL);
}       /* setGWActivation */


/****************************************************/
/****************************************************/
static void gwCommNext(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned maxNr = (unsigned) num(pats);  
  char buf[10];

  if(gw->displayed >= maxNr) 
    gw->displayed = 1;
  else gw->displayed++;
  updateGW(gw);  
}       /* gwCommNext */ 


/****************************************************/
/****************************************************/
static void gwCommPrevious(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned maxNr = (unsigned) num(pats);
  char buf[10];

  gw->displayed = min(gw->displayed, maxNr);
  gw->displayed = (gw->displayed == 1 ? maxNr : gw->displayed - 1);
  updateGW(gw);  
}       /* gwCommPrevious */ 


/****************************************************/
/****************************************************/
static void gwCommFirst(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;

  gw->displayed = 1;
  updateGW(gw);  
}       /* gwCommFirst */ 


/****************************************************/
/****************************************************/
static void gwCommLast(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned maxNr = (unsigned) num(pats);
  char buf[10];

  gw->displayed = maxNr;
  updateGW(gw);  
}       /* gwCommLast */ 


/****************************************************/
/****************************************************/
static void gwCommGoto(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  String gotoVal;
  unsigned vecNr, maxNr = (unsigned) num(pats);
  char buf[60];

  XtVaGetValues(XtNameToWidget(gw->commForm, "gwGotoText"),
      XtNstring, &gotoVal, NULL);
  if(sscanf(gotoVal, "%u", &vecNr) != 1)
  { popupErrDlg(gw->shell, "vistra info",
           "Number must be a positive integer!");
    return;
  }
  if(vecNr < 1 || vecNr > maxNr)
  { sprintf(buf, "Number must be between 1 and %u!", maxNr);
    popupErrDlg(gw->shell, "vistra info", buf);
    return;
  }

  gw->displayed = vecNr;
  updateGW(gw);  
}       /* gwCommGoto */ 


/****************************************************/
/****************************************************/
static void gwCommGrow(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned previous = gw->curWidth;
  char buf[10];
  
  gw->curWidth = (unsigned) max(gw->curWidth+1, 
                                (1 + GW_GROW_FACTOR) * gw->curWidth);
  gw->curWidth = min(gw->curWidth, gwWidths[(int) gw->kind][1]);
  if(previous != gw->curWidth)
  { gw->size = gw->curWidth / (float) gwWidths[(int) gw->kind][1];
    updateGW(gw);
  }
}       /* gwCommGrow */ 


/****************************************************/
/****************************************************/
static void gwCommShrink(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned previous = gw->curWidth;
  char buf[10];
  
  gw->curWidth = (unsigned) min((float) gw->curWidth / (1 + GW_GROW_FACTOR), 
                      gw->curWidth - 1);
  gw->curWidth = max(gw->curWidth, gwWidths[(int) gw->kind][0]);
  if(previous != gw->curWidth)
  { gw->size = gw->curWidth / (float) gwWidths[(int) gw->kind][1];
    updateGW(gw);
  }
}       /* gwCommShrink */ 


/****************************************************/
/****************************************************/
static void gwCommResize(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned previous = gw->curWidth;
  String sizeVal;
  unsigned percentage; 
  char buf[10];
  
  XtVaGetValues(XtNameToWidget(gw->commForm, "gwResizeText"),
      XtNstring, &sizeVal, NULL);
  if(sscanf(sizeVal, "%u", &percentage) != 1)
  { popupErrDlg(gw->shell, "vistra info",
           "Size specification must be a positive integer!");
    return;
  }
  if(percentage > 100)
  { popupErrDlg(gw->shell, "vistra info",
           "Size percentage must be <= 100!");
    return;
  }

  gw->curWidth = (unsigned) max(gwWidths[(int) gw->kind][0], 
                   (percentage/100.0) * gwWidths[(int) gw->kind][1]);
  if(previous != gw->curWidth)
  { gw->size = gw->curWidth / (float) gwWidths[(int) gw->kind][1];
    updateGW(gw);
  }
}       /* gwCommResize */ 


/****************************************************/
/****************************************************/
static void gwCommRedisplay(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
 
  updateGW(gw);
}       /* gwCommRedisplay */ 


/****************************************************/
/****************************************************/
static void gwCommHisto(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  char buf[10];

  gw->kind = histo;
  gw->curWidth = max(gwWidths[(int) histo][0], 
                     (unsigned) (gw->size * gwWidths[(int) histo][1]));
  XtVaSetValues(gw->shell, XtNtitle, "vistra histogram", NULL);
  setGWToggles(gw, histo);
  updateGW(gw);     
  setGWActivation(gw);
}       /* gwCommHisto */ 


/****************************************************/
/****************************************************/
static void gwCommPoly(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  char buf[10];

  gw->kind = poly;
  gw->curWidth = max(gwWidths[(int) poly][0],
                     (unsigned) (gw->size * gwWidths[(int) poly][1]));
  XtVaSetValues(gw->shell, XtNtitle, "vistra poly line", NULL);
  setGWToggles(gw, poly);
  updateGW(gw);     
  setGWActivation(gw);
}       /* gwCommPoly */ 


/****************************************************/
/****************************************************/
static void gwCommGrayMat(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned ndims = (unsigned) maxDim(gw);

  popupInt2Dlg(gw->shell, "matrix form dialog",
               "Specify the number of rows and columns:",
               "rows:", "cols:",
               1, ndims, 1, ndims,                  /* min, max */
               1, ndims,                            /* defaults */
               "show", arrowUpMat, arrowDownMat,    /* arrow callbacks */
               arrowUpMat, arrowDownMat,            /* arrow callbacks */
               gwShowGrayMat, (XtPointer) gw,       /* OK callback, data */
               gwPopdownInt2, (XtPointer) gw);      /* CANCEL callback,data */ 
}       /* gwCommGrayMat */ 


/*************************************************************/
/*************************************************************/
static void gwShowGrayMat(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char *str1, *str2, buf[70];
  unsigned int1, int2, numDims;
  int ret1, ret2;
  GW gw = (GW) client_data;

  numDims = (unsigned) maxDim(gw);
  XtVaGetValues(int2Text[0], XtNstring, &str1, NULL);
  XtVaGetValues(int2Text[1], XtNstring, &str2, NULL);
  if((ret1 = sscanf(str1, "%u", &int1)) != 1 &&
     (ret2 = sscanf(str2, "%u", &int2)) != 1)
  { popupErrDlg(int2DlgShell, "vistra info",
            "At least one field must be a positive integer!");
    setGWToggles(gw, gw->kind);
    return;
  }
  if(ret1 != 1)
    int1 = (numDims % int2 ? numDims/int2 + 1 : numDims/int2);
  if(ret2 != 1)
    int2 = (numDims % int1 ? numDims/int1 + 1 : numDims/int1);

  if( (int1<1) || (int1>numDims) || (int2<1) || (int2>numDims) )
  {
    sprintf(buf, "Values must be between 1 and %u!", numDims);
    popupErrDlg(int2DlgShell, "vistra info", buf);
    setGWToggles(gw, gw->kind);
    return;
  }
  if(int1*int2 < numDims) 
  { sprintf(buf, "#rows * #cols must be >= %u!", numDims);
    popupErrDlg(int2DlgShell, "vistra info", buf);
    setGWToggles(gw, gw->kind);
    return; 
  }

  XtPopdown(int2DlgShell);
  gw->kind = grayMat;
  gw->arg1 = (int) int1;
  gw->arg2 = (int) int2;
  gw->curWidth = max(gwWidths[(int) grayMat][0],
                     (unsigned) (gw->size * gwWidths[(int) grayMat][1]));
  sprintf(buf, "vistra gray matrix (rows: %u, cols: %u)", int1, int2);
  XtVaSetValues(gw->shell, XtNtitle, buf, NULL);
  setGWToggles(gw, grayMat);
  updateGW(gw);     
  setGWActivation(gw);
}       /* gwShowGrayMat */


/*************************************************************/
/*************************************************************/
static void gwShowClrMat(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char *str1, *str2, buf[70];
  unsigned int1, int2, numDims;
  int ret1, ret2;
  GW gw = (GW) client_data;
 
  numDims = (unsigned) maxDim(gw);
  XtVaGetValues(int2Text[0], XtNstring, &str1, NULL);
  XtVaGetValues(int2Text[1], XtNstring, &str2, NULL);
  ret1 = sscanf(str1, "%u", &int1);
  ret2 = sscanf(str2, "%u", &int2);
  if(ret1 != 1 && ret2 != 1)
  { popupErrDlg(int2DlgShell, "vistra info",
            "At least one field must be a positive integer!");
    setGWToggles(gw, gw->kind);
    return;
  }
  if(ret1 != 1)
    int1 = (numDims % int2 ? numDims/int2 + 1 : numDims/int2);
  if(ret2 != 1)
    int2 = (numDims % int1 ? numDims/int1 + 1 : numDims/int1);

  if( (int1<1) || (int1>numDims) || (int2<1) || (int2>numDims) )
  { char buf[50];
    sprintf(buf, "Values must be between 1 and %u!", numDims);
    popupErrDlg(int2DlgShell, "vistra info", buf);
    setGWToggles(gw, gw->kind);
    return;
  }
  if(int1*int2 < numDims) 
  { sprintf(buf, "#rows * #cols must be >= %u!", numDims);
    popupErrDlg(int2DlgShell, "vistra info", buf);
    setGWToggles(gw, gw->kind);
    return; 
  }

  XtPopdown(int2DlgShell);
  gw->kind = clrMat;
  gw->arg1 = (int) int1;
  gw->arg2 = (int) int2;
  gw->curWidth = max(gwWidths[(int) clrMat][0],
                     (unsigned) (gw->size * gwWidths[(int) clrMat][1]));
  sprintf(buf, "vistra color matrix (rows: %u, cols: %u)", int1, int2);
  XtVaSetValues(gw->shell, XtNtitle, buf, NULL);
  setGWToggles(gw, clrMat);
  updateGW(gw);     
  setGWActivation(gw);
}       /* gwShowClrMat */


/****************************************************/
/****************************************************/
static void gwCommClrMat(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned ndims = (unsigned) maxDim(gw);

  popupInt2Dlg(gw->shell, "matrix form dialog",
               "Specify the number of rows and columns:",
               "rows:", "cols:",
               1, ndims, 1, ndims,                  /* min, max */
               1, ndims,                            /* defaults */
               "show", arrowUpMat, arrowDownMat,
               arrowUpMat, arrowDownMat, 
               gwShowClrMat, (XtPointer) gw,
               gwPopdownInt2, (XtPointer) gw);
}       /* gwCommClrMat */ 


/****************************************************/
/****************************************************/
static void gwComm2DProj(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  unsigned numDims = (unsigned) maxDim(gw); 

  popupInt2Dlg(gw->shell, "show 2D projection",
               "Specify the dimensions to project:",
               "x-axes:", "y-axes:",
               1, numDims, 1, numDims, 1, min(numDims, 2),
               "show",
               arrowUp, arrowDown, arrowUp, arrowDown,
               gwShow2DProj, (XtPointer) gw,
               gwPopdownInt2, (XtPointer) gw);
}       /* gwComm2DProj */ 


/*********************************************************/
/*********************************************************/
static void gwShow2DProj(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char *strdim1, *strdim2, buf[70];
  unsigned dim1, dim2;
  long numDims;
  GW gw = (GW) client_data;

  XtVaGetValues(int2Text[0], XtNstring, &strdim1, NULL);
  XtVaGetValues(int2Text[1], XtNstring, &strdim2, NULL);
  if(sscanf(strdim1, "%u", &dim1) != 1 ||
     sscanf(strdim2, "%u", &dim2) != 1)
  { popupErrDlg(int2DlgShell, "vistra info",
                "Values must be positive integers!");
    setGWToggles(gw, gw->kind);
    return;
  }
  numDims = maxDim(gw);
  if(dim1 < 1 || dim2 < 1 || dim1 > numDims || dim2 > numDims)
  { sprintf(buf, "Only dimensions 1 through %ld possible!", numDims);
    popupErrDlg(int2DlgShell, "vistra info", buf);
    setGWToggles(gw, gw->kind);
    return;
  }

  XtPopdown(int2DlgShell);
  gw->kind = proj2D;
  gw->arg1 = (int) dim1;
  gw->arg2 = (int) dim2;
  gw->curWidth = max(gwWidths[(int) proj2D][0],
                     (unsigned) (gw->size * gwWidths[(int) proj2D][1]));
  sprintf(buf, "vistra 2D proj. (x: dim %u, y: dim %u)", dim1, dim2);
  XtVaSetValues(gw->shell, XtNtitle, buf, NULL);
  setGWToggles(gw, proj2D);
  updateGW(gw);
  setGWActivation(gw);     
}       /* gwShow2DProj */


/****************************************************/
/****************************************************/
static void gwInOutToggle(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  int currentlySet;

  currentlySet = (int) XawToggleGetCurrent(
                             XtNameToWidget(gw->commForm, "gwInputToggle"));
  /* currentlySet kann NULL enthalten, da auch ein Unset eines Toggles */
  /* einen Aufruf dieser Funktion bewirkt.                             */
  if(currentlySet)
  {
    if((gw->input && currentlySet==GW_SHOW_OUTPUT) ||
       (! gw->input && currentlySet==GW_SHOW_INPUT)) 
    {
      gw->input = (currentlySet == GW_SHOW_INPUT);
      updateGW(gw);
    }   /* if(currentlySet != gw->input) */
  }     /* if(currentlySet) */
}       /* gwInOutToggle */


/****************************************************/
/* Aktualisiere die Graphik des gw->core Widgets je */
/* gewaehlter Darstellungsart, Pattern-Nummer,      */
/* Groesse etc.                                     */
/****************************************************/
static void updateGW(GW gw)
{ char buf[90];
  XEvent fake_event;
  unsigned maxNr = (unsigned) num(pats);
  Range r;  

  if(gw->kind == noKind) return;
  if(notPrintableGW(gw)) return;   

  /* aktualisiere das Nr-Label und Size-Label */
  gw->displayed = min(maxNr, gw->displayed);
  if(allInOne(gw->kind)) strcpy(buf, "all");
  else sprintf(buf, "%u", gw->displayed);
  XtVaSetValues(XtNameToWidget(gw->infoForm, "gwNrLabel"),
                XtNlabel, buf, NULL);
  sprintf(buf, "%u%%", (unsigned) (gw->size * 100.0));
  XtVaSetValues(XtNameToWidget(gw->infoForm, "gwSizeLabel"), 
                  XtNlabel, buf, NULL);

  /* berechne die Breite/Hoehe fuer ein Diagramm EINES Vektors. */
  /* Setze die Felder widthPerVector und heightPerVector.       */  
  computeExtents(gw);

  gw->pmWidth = gw->widthPerVector + PM_LEFTMARGIN + gwDists[gw->kind];
  gw->pmHeight = gw->heightPerVector + PM_TOPMARGIN + gwDists[gw->kind];

  /* Kreiere neue Pixmap */
  if(gw->pm) XFreePixmap(display, gw->pm);
  gw->pm = XCreatePixmap(display, XtWindow(topLevel),
                         gw->pmWidth, gw->pmHeight, depth);

  /* Initialisiere die Pixmap */
  XSetForeground(display, gc, white);
  XFillRectangle(display, gw->pm, gc, 0, 0, gw->pmWidth, gw->pmHeight);

  /* berechne evtl. Minima und Maxima neu */
  if(gw->patVersion != lastChanged)
  {                  /* die Ein- oder Ausgabevektoren haben sich zwischen- */
                     /* zeitlich geaendert.                                */ 
    computeMinMax(gw);
    gw->patVersion = lastChanged;
  }
 
  /* zeichne in die Pixmap */
  switch(gw->kind)
  {
    case proj2D:  draw2DProjection(gw, displayedVecs(gw),
                       gw->arg1, gw->arg2, black);
                  if(error) { xhandleErr(error); return; }
                  break;
    case histo:   r.lowest = selMin(gw);
                  r.highest = selMax(gw);
                  drawHistogram(gw, 
                        (Vector) at(displayedVecs(gw), gw->displayed),
                        r, black);
                    if(error) { xhandleErr(error); return; }
                  break;
    case poly:    r.lowest = selMin(gw);
                  r.highest = selMax(gw);
                  drawPolyLine(gw, 
                     (Vector) at(displayedVecs(gw), gw->displayed),
                     r, black); 
                  if(error) { xhandleErr(error); return; }
                  break;
    case grayMat: 
    case clrMat:  r.lowest = selMin(gw);
                  r.highest = selMax(gw);
                  drawColorArray(gw->pm, gw->kind == grayMat,
                       (Vector) at(displayedVecs(gw), gw->displayed),
                       PM_LEFTMARGIN, PM_TOPMARGIN,
                       gw->curWidth, gw->curWidth, r, gw->arg1, gw->arg2);   
                  break;
    default:      break;
  }          /* switch */

  redrawGW(gw->core, gw, &fake_event, NULL); 
}            /* updateGW */


/********************************************************/
/* Berechne die Werte der Felder widthPerVector und     */
/* heightPerVector von gw.                              */
/********************************************************/
static void computeExtents(GW gw)
{ unsigned ndims = (unsigned) maxDim(gw);

  switch(gw->kind)
  {
    case proj2D:  gw->heightPerVector = gw->widthPerVector = gw->curWidth;
                  break;
    case poly:
    case histo:   gw->widthPerVector = ndims * gw->curWidth +
                                   (2 * X_DIST_FROM_EDGE) + 1;
                  gw->heightPerVector = POLY_HISTO_HEIGHT;
                  break;
    case clrMat:
    case grayMat: gw->widthPerVector = (long) gw->arg2 * gw->curWidth +
                      (2 * LINE_WIDTH);
                  gw->heightPerVector = (long) gw->arg1 * gw->curWidth +
                      (2 * LINE_WIDTH);
                  break;
  }       /* switch */
}         /* computeExtents */
  

/*******************************************************/
/*******************************************************/
static void gwCommFillUp(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;
  char buf[90];
  XEvent fake_event;
  unsigned maxNr = (unsigned) num(pats);
  unsigned vecsPerLine, toShow, shown; 
  Range r;  

  if(gw->kind == noKind) return;
  if(notPrintableGW(gw)) return;   

  gw->displayed = min(maxNr, gw->displayed);

  /* berechne die Breite/Hoehe fuer ein Diagramm EINES Vektors. */
  /* Setze die Felder widthPerVector und heightPerVector.       */  
  computeExtents(gw);

  toShow = maxNr - gw->displayed + 1; 
  vecsPerLine = (PIXMAP_MAXWIDTH - PM_LEFTMARGIN) 
                 / (gw->widthPerVector + gwDists[gw->kind]);
  gw->pmWidth = (maxNr-gw->displayed+1 > vecsPerLine ? 
                   PIXMAP_MAXWIDTH : 
                   toShow * (gw->widthPerVector + gwDists[gw->kind]) 
                   + PM_LEFTMARGIN); 
  gw->pmHeight = min(PIXMAP_MAXHEIGHT, 
                     ((toShow-1)/vecsPerLine + 1) * 
                     (gw->heightPerVector + gwDists[gw->kind]) 
                     + PM_TOPMARGIN);
  
  /* Kreiere neue Pixmap */
  if(gw->pm) XFreePixmap(display, gw->pm);
  gw->pm = XCreatePixmap(display, XtWindow(topLevel),
                         gw->pmWidth, gw->pmHeight, depth);

  /* Initialisiere die Pixmap */
  XSetForeground(display, gc, white);
  XFillRectangle(display, gw->pm, gc, 0, 0, gw->pmWidth, gw->pmHeight);

  /* berechne evtl. Minima und Maxima neu */
  if(gw->patVersion != lastChanged)
  {                  /* die Ein- oder Ausgabevektoren haben sich zwischen- */
                     /* zeitlich geaendert.                                */ 
    computeMinMax(gw);
    gw->patVersion = lastChanged;
  }
 
  /* zeichne in die Pixmap */
  switch(gw->kind)
  {
    case histo:   r.lowest = selMin(gw);
                  r.highest = selMax(gw);
                  shown = drawHistogrammes(gw, displayedVecs(gw), r, black); 
                  if(error) { xhandleErr(error); return; }
                  break;
    case poly:    r.lowest = selMin(gw);
                  r.highest = selMax(gw);
                  shown = drawPolyLines(gw, displayedVecs(gw), r, black); 
                  if(error) { xhandleErr(error); return; }
                  break;
    case grayMat: 
    case clrMat:  r.lowest = selMin(gw);
                  r.highest = selMax(gw);
                  shown = drawMatrices(gw, gw->kind == grayMat, 
                               displayedVecs(gw),
                               r, gw->arg1, gw->arg2);   
                  break;
    default:      break;
  }          /* switch */

  sprintf(buf, "%u-%u", gw->displayed, shown);
  XtVaSetValues(XtNameToWidget(gw->infoForm, "gwNrLabel"),
                XtNlabel, buf, NULL);
  redrawGW(gw->core, gw, &fake_event, NULL); 
}            /* gwCommFillUp */


/**************************************************************/
/**************************************************************/
static Boolean notPrintableGW(GW gw)
{ unsigned ndims = (unsigned) maxDim(gw);
  Boolean answer = FALSE;  
  char buf[90];

  switch(gw->kind)
  {
    proj2D:  if(gw->arg1 > ndims || gw->arg2 > ndims)
             { sprintf(buf, "Dimension %u not existing!", 
                            max(gw->arg1, gw->arg2));
               answer = TRUE;
             }
             break; 
    grayMat:
    clrMat:  if(gw->arg1 * gw->arg2 < ndims)
             { sprintf(buf, "%u dimensions cannot be displayed within a %ux%u matrix!", ndims, gw->arg1, gw->arg2);
               answer = TRUE;
             }
             break;
    default: break;
  }       /* switch */
  
  if(answer)
  { popupErrDlg(gw->shell, "vistra info", buf);
    initGW(gw);
  }    

  return answer;
}         /* notPrintableGW */


/**************************************************************/
/**************************************************************/
static void redrawGW(Widget w, XtPointer client_data, XEvent *event,
                     Boolean *bool)
{ GW gw = (GW) client_data;
  
  XClearWindow(display, XtWindow(gw->core));
  XtVaSetValues(gw->core, 
       XtNwidth, gw->pmWidth, XtNheight, gw->pmHeight, NULL);
  if(gw->pm) 
    XCopyArea(display, gw->pm, XtWindow(gw->core), gc, 0, 0,
            gw->pmWidth, gw->pmHeight, 0, 0);
}       /* redrawGW */


/*************************************************************/
/* Callback, das aufgerufen, wenn der Benutzer im int2Dlg    */
/* 'Cancel' waehlt.                                          */
/*************************************************************/
static void gwPopdownInt2(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ GW gw = (GW) client_data;

  setGWToggles(gw, gw->kind);
  XtPopdown(int2DlgShell);
}       /* gwPopdownInt2 */


