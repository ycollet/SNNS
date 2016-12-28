/**********************************************************************/
/**********************************************************************/
/***** VISTRA - Hauptfile                                         *****/
/***** ---------------------------------------------------------- *****/
/***** Diese Datei enthaelt die main-Funktion und alle Callbacks  *****/
/***** die das Hauptfenster von Vistra betreffen.                 *****/
/*****                                                            *****/
/***** Folgende Dateien werden included:                          *****/
/*****   "crtwdgts.c" - Fkt., die Widgets kreieren.               *****/
/*****                  (insbesondere die des Hauptfensters)      *****/
/*****   "uif.c"  - Fkt. der graphischen Benutzer-Oberflaeche,    *****/
/*****              die keine Callbacks sind.                     *****/
/*****             (u.a. update-Routinen der Hauptfenster-Widgets)*****/
/*****   "init.c" - Initialisierung von Vistra.                   *****/
/*****   "draw.c" - Zeichen-Routinen der Graphik-Fenster.         *****/
/*****   "gw.c"   - Kreation, Steuerung und Loeschen der Graphik- *****/
/*****              Fenster.                                      *****/
/*****   "int2.c" - Routinen der Dialogbox, in der 2 Integers     *****/
/*****              eingegeben werden.                            *****/
/*****   "ssw.c"  - Routinen des SpreadSheet-Windows.             *****/
/*****              Das SpreadSheet-Window ist das Fenster, das   *****/
/*****              die Vektoren enthaelt, die momentan nicht im  *****/
/*****              Hauptfenster bearbeitet werden.               *****/     
/**********************************************************************/  
/**********************************************************************/

#include "xvis.h"
#include "def.h"
#include <math.h>
/* #include <sys/stdtypes.h> */   
#ifdef XFWF_FILE_SELECTION
#include <Xfwf/FileSel.h>
#endif
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Core.h>

/***********************************************************************/
/****** FUNCTIONS CALLED BY VISTRA.C ***********************************/
/***********************************************************************/
static void xhandleErr(int);
/*** INIT.C ***/
static void initialize();                             /* error gesetzt */
static void initialize2(Widget);
/*** UIF.C ***/
static void fillPatternView();                        /* error gesetzt */
static void changeInOut();                            /* error gesetzt */
static Format nameToFormat(char *);                   /* error gesetzt */
#ifdef XFWF_FILE_SELECTION
static void popupFileSelector();
#endif
static void popupStr2Dlg();
static void popupErrDlg(Widget, char *, char *);
static void getStr2DlgValues(char **, char **);
static void setActivation(Patterns);
static Vector scalarVec();                            /* error gesetzt */
static Vector scalarsVertAsVec();                     /* error gesetzt */
static Vector scalarsHorizAsVec();                    /* error gesetzt */
static void fixSize(Widget);
static void updateInfoWidgets();
static void updateScalarsHoriz();
static void updateScalarsVert();
static void updateDimLabels();
static void updateNrLabels();
static void updateClassLabels();
static void updateElems();
static void updateVertScrollBar();
static void updateHorizScrollBar();
static void updateTW(Widget, char *, char *);       
static void showInfo(char *);      
static void exposeWidget(Widget);
/*** SSW.C ***/
static void updateSSW(SSW);
static void popupSSW(SSW);
/*** CRTWDGTS.C ***/
static void createAllWidgets(Widget);                 /* error gesetzt */
/*** INT2.C ***/
static void popupInt2Dlg();
static void updateInt2Text();
/*** STRDLG.C ***/
static void popupStrDlg();
/*** GW.C ***/
static GW openGW(Widget, char *);
static void freeGW(GW); 
/*** DRAW.C ***/
static void initDrawer(Widget);
static void drawColorArray(Drawable, Boolean, Vector, unsigned, unsigned,
                           unsigned, unsigned, Range, long, long);
static void drawHistogram(GW, Vector, Range, PixelNo);   /* error gesetzt */
static void drawPolyLine(GW, Vector, Range, PixelNo);    /* error gesetzt */
static void draw2DProjection(GW, VecColl, long, long, PixelNo);
                                                         /* error gesetzt */
static unsigned drawHistogrammes(GW, VecColl, Range, PixelNo); 
                                                         /* error gesetzt */
static unsigned drawPolyLines(GW, VecColl, Range, PixelNo);  
                                                         /* error gesetzt */
static unsigned drawMatrices(GW, Boolean, VecColl, Range, long, long);
                                                         /* error gesetzt */

/***************************************************************************/
/******** CALLBACKs ********************************************************/
/***************************************************************************/
static void toggle(Widget, XtPointer, XtPointer);
static void toggleScalars(Widget, XtPointer, XtPointer);
static void loadPatterns(Widget, XtPointer, XtPointer);
static void writePatterns(Widget, XtPointer, XtPointer);
static void loadSymbols(Widget, XtPointer, XtPointer);
static void writeSymbols(Widget, XtPointer, XtPointer);
static void changeFormat(Widget, XtPointer, XtPointer);
static void scaleVectors(Widget, XtPointer, XtPointer);
static void removeVectors(Widget, XtPointer, XtPointer);
static void removeDimensions(Widget, XtPointer, XtPointer);
static void fileMenuSelect(Widget, XtPointer, XtPointer);
static void transformMenuSelect(Widget, XtPointer, XtPointer);
static void removeMenuSelect(Widget, XtPointer, XtPointer);
static void showMenuSelect(Widget, XtPointer, XtPointer);
static void loadMenuSelect(Widget, XtPointer, XtPointer);
static void formatMenuSelect(Widget, XtPointer, XtPointer);
static void loadConstant(Widget, XtPointer, XtPointer);
static void loadNr(Widget, XtPointer, XtPointer);
static void addScalars(Widget, XtPointer, XtPointer);
static void subScalars(Widget, XtPointer, XtPointer);
static void multScalars(Widget, XtPointer, XtPointer);
static void divScalars(Widget, XtPointer, XtPointer);
static void replaceScalars(Widget, XtPointer, XtPointer);
static void replVec(Widget, XtPointer, XtPointer);
static void popdown(Widget, XtPointer, XtPointer);
static void popdownSwapSSW(Widget, XtPointer, XtPointer);
#ifdef XFWF_FILE_SELECTION
static void popdownFileSelector(Widget, XtPointer, XtPointer);
#endif
static void destroyTextWindow(Widget, XtPointer, XtPointer);
static void scrollVertical(Widget, XtPointer, XtPointer);
static void scrollHorizontal(Widget, XtPointer, XtPointer);
static void jumpVertical(Widget, XtPointer, XtPointer);
static void jumpHorizontal(Widget, XtPointer, XtPointer); 
/* defined in INT2.C */
static void arrowUp(Widget, XtPointer, XtPointer);
static void arrowDown(Widget, XtPointer, XtPointer);
static void arrowUpMat(Widget, XtPointer, XtPointer);
static void arrowDownMat(Widget, XtPointer, XtPointer);

/****************************************************************************/
/********* WIDGETS **********************************************************/
/****************************************************************************/
static XtAppContext app_context;
static Widget errDlgShell, errDlgLabel;
static Widget strDlgShell, strDlg, strDlgButtonOk;
#ifdef XFWF_FILE_SELECTION
static Widget fileSelShell, fileSelector;
#endif
static Widget str2DlgShell, str2Headline, str2Label1, str2Label2, str2Text1,
              str2Text2, str2Ok;
static Widget int2DlgShell, int2Headline, int2Label1, int2Label2, int2Text[2],
	      int2Ok, int2Up[2], int2Down[2];
static Widget statistics;
static Widget topLevel, form, menuBar, infoForm, patternForm, infoLabel;
static Widget fileMenuButton, transformMenuButton, removeMenuButton,
              showMenuButton, formatMenuButton;
static Widget fileMenu, transformMenu, removeMenu, showMenu, formatMenu;
static Widget inputToggle, outputToggle, vertToggle, horizToggle;
static Widget labelFile, fileLabel, labelNumOfPatterns, numOfPatternsLabel,
              labelFormat, formatLabel;
static Widget *nrLabels, labelClass, *classLabels, *dimLabels,
              *scalarsVert, *scalarsHoriz, **elems,
              vertScrollBar, horizScrollBar,
              loadMenuButtonVert, loadMenuButtonHoriz;
static Widget operationBox, addCommand, subCommand, multCommand, divCommand,
              replaceCommand;

/****************************************************************************/
/******* Variablen des User-Interface, die keine Widgets enthalten **********/
/****************************************************************************/
static inOutSwitch;
                      /* EDIT_INPUT:  Input Pattern editieren  */
                      /* EDIT_OUTPUT: Output Pattern editieren */
static scalarSwitch;
                      /* FILL_HORIZ:  Spalten-Operation        */
                      /* FILL_VERT:   Zeilen-Operation         */
static Patterns pats;
                      /* Die Patterns, die momentan geladen sind.           */
                      /* NULL, falls keine geladen.                         */
static char *fileName;
                      /* Name des Pattern files, das gerade geladen ist.    */
                      /* NULL, falls momentan keine Patterns geladen sind.  */
static Collection formatNames;
                      /* Collection('see Suffix' <fmt1> <fmt2> ... <fmt3>)  */
                      /* <fmti>: Name eines Format-Files ohne Suffix '.fmt' */
                      /* Die Strings stellen gleichzeitig die Labels der    */
                      /* Menu-Items des Format-Menus dar.                   */
static Collection formats;
		      /* Collection aller syntaktisch korrekten ASCII-      */
                      /* Formate, die auf Platte gefunden wurden.           */
                      /* size(formats) == size(formatNames) - 3 gilt stets. */
static Collection formatMenuItems;
                      /* Collection aller smeBSBObjects, die die Items des  */
                      /* Format-Menus darstellen.                           */
                      /* size(formatMenuItems) == size(formatNames) gilt    */
                      /* stets.                                             */
static int selectedFormat;
                      /* Index des momentan ausgewaehlten Format-Menuitems  */
                      /* in Collection formatNames.                         */
static time_t lastChanged;
                      /* Zeitstempel der letzten Modifikation der Eingabe-  */
                      /* oder Ausgabevektoren, die ein neues globales       */
                      /* Minimum oder Maximum zur Folge hat.                */
                      /* GWs vergleichen ihren Zeitstempel mit lastChanged  */
                      /* um zu wissen, ob die Minima und Maxima neu be-     */
                      /* rechnet werden muessen.                            */ 
static Pixmap hook;
                      /* Haken fuer selektierte Menupunkte.                 */
static SSW swapSSW;   /* SSW zur Anzeige der Input oder Output Vektoren     */
                      /* (READ ONLY-Fenster mit SpreadSheet.                */
static Boolean swapSSWOpened;
                      /* True: Input/Output-Vektoren werden im separaten    */
                      /*       Fenster (swapSSW) angezeigt (Read Only).     */
                      /* False: Swap-Fenster nicht geoeffnet.               */
static FILE *lf;      /* Log-File                                           */
static Boolean logFirstTime;
                      /* TRUE gdw. erste Log-Periode                        */
static unsigned lastNr;
                      /* Nummer des zuletzt mittels 'Load Nr...' geladenem  */
                      /* Spalten- bzw. Zeilenvektors.                       */
static char infoBuf[512];
                      /* zum Formatieren der Infos von infoLabel.           */
static XFontStruct *gwFont;
                      /* Font fuer die Commands der Graphik-Fenster         */
static Collection openGWs;
                      /* Collection aller momentan geoeffneten GWs.         */
static unsigned depth;
                      /* Tiefe des Screens.                                 */
static int screen;    /* Der Screen, auf dem Vistra laeuft.                 */
static Display *display;    /* Der Display (benoetigt von X)                */ 

/****************************************************************************/
/***** VARIABLEN zur Steuerung des HAUPTFENSTERS. ***************************/
/****************************************************************************/
static Vector vert, horiz;
                      /* Vertikaler und horizontaler Skalar-Vektor.         */
static int firstVec;  /* Enthaelt die Nummer des ersten Vektors, der        */
                      /* momentan sichtbar ist im Hauptfenster.             */
static int firstDim;  /* Enthaelt die Nummer der Dimension, die gerade noch */
                      /* links sichtbar ist im Hauptfenster.                */
static int visVecs;   /* Anzahl der sichtbaren Vektoren.                    */
static int visDims;   /* Anzahl der sichtbaren Dimensionen.                 */
static int usedRows;  /* Anzahl der benutzten Zeilen in der Pattern View.   */
static int usedCols;  /* Anzahl der benutzten Spalten in der Pattern View.  */

/***** zum Zeichnen notwendige Variablen ************************************/
static PixelNo colorpixel[NUM_COLORS];
static PixelNo graypixel[NUM_GRAYS];
static Colormap cmap;
static PixelNo black, white;
static GC gc, textGC;
static Window root;
static Font textFont;
static XFontStruct *drawFont;

#include "init.c"
#include "crtwdgts.c"
#include "uif.c"
#include "int2.c" 
#include "gw.c" 
#include "ssw.c"
#include "strdlg.c"

/*********************************************************************/
/*********************************************************************/
/************* H A U P T - P R O G R A M M ***************************/
/*********************************************************************/
/*********************************************************************/
main(int argc, char **argv)
{
  initialize(argc, argv);
  if(error) handleErr(error);

  createAllWidgets(topLevel);
  if(error) handleErr(error);

  updateInfoWidgets();
  if(pats) fillPatternView();
  if(error) handleErr(error);
  setActivation(pats);

  XtRealizeWidget(topLevel);
  fixSize(topLevel);
  initialize2(topLevel);

  XtAppMainLoop(app_context);
}    /* main */


/****************************************************************************/
/****************************************************************************/
/************ C A L L B A C K  -  F U N C T I O N S *************************/
/****************************************************************************/
/****************************************************************************/

/*************************************************************/
/* Wird aufgerufen, sobald der 'Load'-Button des Dialogs     */
/* "dialog" angeklickt wird.                                 */
/* Lade die Patterns des Files, dessen Namen im Dialog       */
/* eingetippt wurde. Bei Erfolg werden die Patterns in der   */
/* Variable pats gespeichert. Das Format des Pattern Files   */
/* ist durch die Variable format gegeben.                    */
/*************************************************************/
static void loadPatterns(Widget w, XtPointer client_data, XtPointer ret)
{ char *suffix, *format_name;
  Patterns p;
  FILE *f;
  Format readFmt;
#ifdef XFWF_FILE_SELECTION
  char value[2048];
  XfwfFileSelectorOkButtonReturnStruct *fsrs = 
             (XfwfFileSelectorOkButtonReturnStruct *) ret;
  XtPopdown(fileSelShell);
  sprintf(value, "%s%s", fsrs->path, fsrs->file);
#else
  char *value;
  XtPopdown(strDlgShell);
  value = XawDialogGetValueString((Widget) client_data);
#endif

  /* stelle Format-Namen fest und ggf. zugehoeriges ASCII-Format */
  if(selectedFormat == 1)
  {                                            /* 'See Suffix' */
    suffix = strrchr(value, '.');
    if(! suffix)
    { sprintf(errorInfo, "File:  %s", value);
      xhandleErr(21);
      return;
    }
    format_name = ++suffix;
  }                                    /* if(selectedFormat == 1) */
  else format_name = (char *) at(formatNames, selectedFormat);
  readFmt = nameToFormat(format_name);
  if(error) { xhandleErr(error); return; }

  /* oeffne Pattern File */
  f = fopen(value, "r");
  if(! f)
  { sprintf(errorInfo, "Pattern file:  %s", value);
    xhandleErr(11);
    return;
  }

  /* lese File ein */
  showInfo("Loading patterns..."); 
  if(streq(format_name, N01_MENU_LABEL)) p = readN01(f);
  else if(streq(format_name, LVQ_MENU_LABEL)) p = readLVQ(f);
  else
  { p = newPatterns();
    if(error) { xhandleErr(error); return; }
    fileIn(p, readFmt, f);
  }
  fclose(f);
  clearInfo(); 
  if(error) { free(value); xhandleErr(error); return; }
  /* Patterns erfolgreich geladen */
  if(fileName) { free(fileName); fileName = NULL; }
  fileName = my_strdup(value);

  if(pats) freePatterns(pats);
  pats = p;
  /* schliesse alle geoeffneten Graphik-Fenster */
  if(notEmpty(openGWs))
    freeFromTo(openGWs, 1L, size(openGWs), freeGW);  
    
  fillPatternView();
  if(error) handleErr(error);

  /* schliesse oder aktualisiere das Swap-Fenster, falls geoeffnet */   
  if(swapSSWOpened)
  { if(hasOutputs(pats)) updateSSW(swapSSW);
    else 
    { Widget dummy;
      popdownSwapSSW(dummy, NULL, NULL);
    }
  }
}       /* loadPatterns */


/*************************************************************/
/* Wird aufgerufen, sobald der 'Write'-Button des String-    */
/* Dialogs angeklickt wird.                                  */
/*************************************************************/
static void writePatterns(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char *value, *suffix, *format_name;
  FILE *f;
  long pos;
  Format writeFmt;

  XtPopdown(strDlgShell);
  value = XawDialogGetValueString((Widget) client_data);

  /* stelle den Format-Namen fest und ggf. das Schreib-ASCII-Format */
  if(selectedFormat == 1)
  {                                            /* 'See Suffix' */
    suffix = strrchr(value, '.');
    if(! suffix)
    { sprintf(errorInfo, "File:  %s", value);
      xhandleErr(21);
      return;
    }
    format_name = ++suffix;
  }                                  /* if(selectedFormat == 1) */
  else format_name = (char *) at(formatNames, selectedFormat);
  writeFmt = nameToFormat(format_name);
  if(error) { xhandleErr(error); return; }

  /* Oeffne Output File */
  f = fopen(value, "w");
  if(! f)
  { sprintf(errorInfo, "Output file:  %s", value);
    xhandleErr(12);
    return;
  }

  /* Schreibe Pattern File */
  sprintf(infoBuf, "Writing patterns to %s...", value);
  showInfo(infoBuf);
  if(streq(format_name, N01_MENU_LABEL)) writeN01(pats, f);
  else if(streq(format_name, LVQ_MENU_LABEL)) writeLVQ(pats, f);
  else fileOut(pats, writeFmt, f);
  fclose(f);
  clearInfo();
  if(error) { xhandleErr(error); return; }
}       /* writePatterns */


/******************************************************/
/* Lese das Symtab-File ein, dessen Name ueber das    */
/* Dialog-Widget client_data eingegeben wurde.        */
/******************************************************/
static void loadSymbols(w, client_data, ret)
Widget w;
XtPointer client_data, ret;
{ FILE *f;
  Symtab st;
#ifdef XFWF_FILE_SELECTION
  char value[2048];
  XfwfFileSelectorOkButtonReturnStruct *fsrs = 
             (XfwfFileSelectorOkButtonReturnStruct *) ret;

  XtPopdown(fileSelShell);
  sprintf(value, "%s%s", fsrs->path, fsrs->file);
#else
  char *value;
  XtPopdown(strDlgShell);
  value = XawDialogGetValueString((Widget) client_data);
#endif

  f = fopen(value, "r");
  if(! f) { xhandleErr(14); return; }

  /* Lese das Symtab-File */
  sprintf(infoBuf, "Loading symbols from %s...", value);
  showInfo(infoBuf);
  st = readSymtab(f);
  fclose(f);
  clearInfo();
  if(error) xhandleErr(error);
  if(numSymbols(st) < maxClassNo(pats))
  { freeSymtab(st);
    xhandleErr(33);
    return;
  }
  /* Ersetze die bisherigen Klassennamen durch die neuen von st */
  XtVaSetValues(infoLabel, XtNlabel, "Replacing symbols...", NULL);
  replaceClasses(pats, st);
  clearInfo();
  if(error)
  { freeSymtab(st);
    xhandleErr(error);
    return;
  }
  /* Aktualisiere die Widgets */
  updateClassLabels();
}        /* loadSymbols */


/******************************************************/
/* Schreibe die Symbole der Patterns pats in das File,*/
/* dessen Name ueber das Dialog-Widget client_data    */
/* eingegeben wurde.                                  */
/******************************************************/
static void writeSymbols(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char *value;
  FILE *f;

  XtPopdown(strDlgShell);
  value = XawDialogGetValueString((Widget) client_data);

  f = fopen(value, "w");
  if(! f)
  { sprintf(errorInfo, "Output file:  %s", value);
    xhandleErr(12);
    return;
  }

  sprintf(infoBuf, "Writing symbols to %s...", value);
  showInfo(infoBuf);
  writeSymtab(pats, f);
  fclose(f);
  clearInfo();
  if(error) xhandleErr(error);
}        /* writeSymbols */


/******************************************************/
/******************************************************/
static void addScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ Vector vec;
  VecColl edited = editedVecColl(pats);
  Dimension width, h;

  vec = scalarVec();
  if(error) { xhandleErr(error); return; }
  showInfo("Adding scalars...");
  if(scalarSwitch == FILL_VERT)
  { compScalarsRow(edited, vec, '+');
    if(app_data.log)
      fprintf(lf, "%c add vert\n", inOutChar());
  }
  else
  { compScalarsCol(edited, vec, '+');
    if(app_data.log)
      fprintf(lf, "%c add horiz\n", inOutChar());
  }
  clearInfo();
  time(&lastChanged);            /* setze Zeitstempel neu */
  updateElems();
}       /* addScalars */


/******************************************************/
/******************************************************/
static void subScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ Vector vec;
  VecColl edited = editedVecColl(pats);

  vec = scalarVec();
  if(error) { xhandleErr(error); return; }
  showInfo("Subtracting scalars...");
  if(scalarSwitch == FILL_VERT)
  { compScalarsRow(edited, vec, '-');
    if(app_data.log)
      fprintf(lf, "%c subtract vert\n", inOutChar());
  }
  else
  { compScalarsCol(edited, vec, '-');
    if(app_data.log)
      fprintf(lf, "%c subtract horiz\n", inOutChar());
  }
  clearInfo();
  time(&lastChanged);            /* setze Zeitstempel neu */
  updateElems();
}       /* subScalars */


/******************************************************/
/******************************************************/
static void multScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ Vector vec;
  VecColl edited = editedVecColl(pats);

  vec = scalarVec();
  if(error) { xhandleErr(error); return; }
  showInfo("Multiplying scalars...");
  if(scalarSwitch == FILL_VERT)
  { compScalarsRow(edited, vec, '*');
    if(app_data.log)
      fprintf(lf, "%c multiply vert\n", inOutChar());
  }
  else
  { compScalarsCol(edited, vec, '*');
    if(app_data.log)
      fprintf(lf, "%c multiply horiz\n", inOutChar());
  }
  clearInfo();
  time(&lastChanged);            /* setze Zeitstempel neu */
  updateElems();
}       /* multScalars */


/******************************************************/
/******************************************************/
static void divScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ Vector vec;
  VecColl edited = editedVecColl(pats);

  vec = scalarVec();
  if(error) { xhandleErr(error); return; }
  if(detectFirst(vec, 0)) { xhandleErr(24); return; }
  showInfo("Dividing by scalars...");
  if(scalarSwitch == FILL_VERT)
  { compScalarsRow(edited, vec, '/');
    if(app_data.log)
      fprintf(lf, "%c divide vert\n", inOutChar());
  }
  else
  { compScalarsCol(edited, vec, '/');
    if(app_data.log)
      fprintf(lf, "%c divide horiz\n", inOutChar());
  }
  clearInfo();
  time(&lastChanged);            /* setze Zeitstempel neu */
  updateElems();
}       /* divScalars */


/******************************************************/
/******************************************************/
static void replaceScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char buf[15];
  unsigned dflt;

  if(scalarSwitch == FILL_VERT)
    dflt = (unsigned) min(lastNr, numCols(pats));
  else dflt = min(lastNr, num(pats));
  sprintf(buf, "%u", dflt);

  popupStrDlg(topLevel, "Replace row/column nr:", buf, "replace", replVec);
}       /* replaceScalars */


/*******************************************************/
/*******************************************************/
void replVec(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ String value;
  unsigned nr, maxAllowed;
  Vector vec;
  char buf[70];

  XtPopdown(strDlgShell);
  value = XawDialogGetValueString((Widget) client_data);
  if(sscanf(value, "%u", &nr) != 1)
  { popupErrDlg(strDlgShell, "vistra info",
                        "Value must be a positive integer!");
    return;
  }
  maxAllowed = (scalarSwitch == FILL_VERT ? numCols(pats) : num(pats));
  if(nr < 1 || nr > maxAllowed)
  { sprintf(buf, "Value must be between 1 and %u!", maxAllowed);
    popupErrDlg(strDlgShell, "vistra info", buf);
    return;
  }

  vec = scalarVec();
  if(error) { xhandleErr(error); return; }
  if(scalarSwitch == FILL_VERT)
  { replaceCol(editedVecColl(pats), nr, vec);
    if(app_data.log)
      fprintf(lf, "%c replace vert %u\n", inOutChar(), nr);
  }
  else
  { replaceRow(editedVecColl(pats), nr, vec);
    if(app_data.log)
      fprintf(lf, "%c replace horiz %u\n", inOutChar(), nr);
  }

  time(&lastChanged);            /* setze Zeitstempel neu */
  if((scalarSwitch == FILL_VERT && nr>=firstDim && nr<firstDim+usedCols) ||
     (scalarSwitch == FILL_HORIZ && nr>=firstVec && nr<firstVec+usedRows))
    updateElems();
}       /* replVec */


/*******************************************************/
/*******************************************************/
static void scaleVectors(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char *smallest, *biggest;
  Number small, big;
  ScaleOp so;

  getStr2DlgValues(&smallest, &biggest);
  if(sscanf(smallest, "%f", &small) != 1)
  { popupErrDlg(str2DlgShell, "vistra info",
                "Value for lowest is not a number!");
    return;
  }
  if(sscanf(biggest, "%f", &big) != 1)
  { popupErrDlg(str2DlgShell, "vistra info",
                "Value for highest is not a number!");
    return;
  }
  if(small > big)
  { popupErrDlg(str2DlgShell, "vistra info",
                "Highest number smaller than lowest!");
    return;
  }

  XtPopdown(str2DlgShell);
  sprintf(infoBuf, "Scaling to range [%g,%g]...", small, big);
  showInfo(infoBuf);
  scaleAll(editedVecColl(pats), small, big, &so);
  if(app_data.log)
  { fprintf(lf, "%c multConst %g\n", inOutChar(), so.mult);
    fprintf(lf, "%c addConst %g\n", inOutChar(), so.add);
  }
  clearInfo();

  time(&lastChanged);            /* setze Zeitstempel neu */
  updateElems();
}       /* scaleVectors */


/*******************************************************/
/*******************************************************/
static void removeVectors(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char *smallest, *biggest;
  unsigned small, big, ret;

  XtVaGetValues(int2Text[0], XtNstring, &smallest, NULL);
  XtVaGetValues(int2Text[1], XtNstring, &biggest, NULL);
  if(sscanf(smallest, "%u", &small) != 1 ||
     sscanf(biggest, "%u", &big) != 1)
  { popupErrDlg(int2DlgShell, "vistra info",
                "Values must be positive integers!");
    return;
  }

  XtPopdown(int2DlgShell);
  if(small <= big)
  { Vector v;

    v = scalarsVertAsVec();
    if(error) { xhandleErr(error); return; }
    sprintf(infoBuf, "Removing patterns %u-%u...", small, big);
    showInfo(infoBuf);
    ret = removePatterns(pats, small, big);
    clearInfo();
    if(error) { xhandleErr(error); return; }
    removeDimRange(v, small, big);
    if(error) { xhandleErr(error); return; }

    /* Aktualisiere die betroffenen Widgets */
    if(ret)
    { firstVec = 1;
      usedRows = min(num(pats), visVecs);
      time(&lastChanged);            /* setze Zeitstempel neu */
      updateNrLabels();
      updateElems();
      updateClassLabels();
      updateScalarsVert();
      updateVertScrollBar();
      updateInfoWidgets();
      if(swapSSWOpened) updateSSW(swapSSW);
    }   /* if(ret) */
  }
}       /* removeVectors */


/*******************************************************/
/*******************************************************/
static void removeDimensions(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ char *smallest, *biggest;
  unsigned small, big;

  XtVaGetValues(int2Text[0], XtNstring, &smallest, NULL);
  XtVaGetValues(int2Text[1], XtNstring, &biggest, NULL);
  if(sscanf(smallest, "%u", &small) != 1 ||
     sscanf(biggest, "%u", &big) != 1)
  { popupErrDlg(int2DlgShell, "vistra info",
                "Values must be positive integers!");
    return;
  }

  XtPopdown(int2DlgShell);
  if(small <= big)
  { unsigned ret;
    Vector v;

    v = scalarsHorizAsVec();
    if(error) { xhandleErr(error); return; }
    ret = removeCols(pats, inOutSwitch==EDIT_INPUT, small, big);
    if(error) { xhandleErr(error); return; }
    sprintf(infoBuf, "Removing dimensions %u-%u...", small, big);
    showInfo(infoBuf);
    removeDimRange(v, small, big);
    clearInfo();
    if(error) { xhandleErr(error); return; }
    if(app_data.log)
      fprintf(lf, "%c rmDimRange %u %u\n", inOutChar(), small, big);

    /* Aktualisiere die betroffenen Widgets */
    if(ret)
    { firstDim = 1;
      usedCols = min(numCols(pats), visDims);
      time(&lastChanged);            /* setze Zeitstempel neu */
      updateDimLabels();
      updateElems();
      updateScalarsHoriz();
      updateHorizScrollBar();
    }
  }
}       /* removeDimensions */


/***************************************************************************/
/* Wird immer aufgerufen, wenn irgendein Menupunkt des File-Menus ausge-   */
/* waehlt wird. garbage ist ohne Bedeutung. client_data gibt die Nummer    */
/* des ausgewaehlten Menuepunkts zurueck.                                  */
/***************************************************************************/
static void fileMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ int itemNo = (int) client_data;

  switch(itemNo)
  {
    case 0:  /* Load Patterns */
#ifndef XFWF_FILE_SELECTION
             popupStrDlg(topLevel, "Load patterns from file:", "",
                         "load", loadPatterns); 
#else
             popupFileSelector(topLevel, "load pattern file",
                         loadPatterns); 
#endif
             break;
    case 1:  /* Write Patterns */
             popupStrDlg(topLevel, "Write patterns to file:", "", "write",
                         writePatterns);
             break;
    case 3:  /* Load Symbols */
#ifndef XFWF_FILE_SELECTION
             popupStrDlg(topLevel, "Load symbols from file:", "", "load",
                         loadSymbols);
#else
             popupFileSelector(topLevel, "load symbols from file",
                         loadSymbols); 
#endif
             break;
    case 4:  /* Write Symbols */
             popupStrDlg(topLevel, "Write symbols to file:", "", "write",
                         writeSymbols);
             break;
    case 5:  /* Write To Log File */
             if(! app_data.log)
             { char *buf, *info;
               if(! (buf = (char *) malloc(strlen(fileName) + 6)))
                 { xhandleErr(1); return; }
               sprintf(buf, "%s.log", fileName);
               if(! (lf = fopen(buf, (logFirstTime?"w":"a"))))
               { if(! (info = (char *) malloc(strlen(buf) + 50)))
                   { xhandleErr(1); return; }
                 sprintf(info, "Cannot open log file %s!", buf);
                 popupErrDlg(topLevel, "vistra warning", info);
                 free(buf);
                 free(info);
                 return;
               }
               free(buf);
             }
             else
             { fclose(lf);
               lf = NULL;
               logFirstTime = FALSE;
             }

             app_data.log = ! app_data.log;
             if(app_data.log)
               XtVaSetValues(w, XtNleftBitmap, hook, NULL);
             else XtVaSetValues(w, XtNleftBitmap, None, NULL);
             break;
    case 7:  /* Quit Program */
             exit(0);
             break;
    default: sprintf(errorInfo, "fileMenuSelect() called with item number %d !",
                       itemNo);
             xhandleErr(20);
             break;
  }   /* switch */
}     /* fileMenuSelect */


/***************************************************************************/
/* Wird immer aufgerufen, wenn irgendein Menupunkt des Transform-Menus aus-*/
/* gewaehlt wird. garbage ist ohne Bedeutung. client_data gibt die Nummer  */
/* des ausgewaehlten Menuepunkts an.                                       */
/***************************************************************************/
static void transformMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data;
XtPointer garbage;
{ int itemNo = (int) client_data;

  switch(itemNo)
  {
    case 0:  /* HLOG */
             if(! isSquare((unsigned) numCols(pats)))
             { popupErrDlg(topLevel, "vistra info",
                           "HLOG can only be applied to quadratic patterns!");
               return;
             }

             showInfo("Performing HLOG transformation...");
             doRows(editedVecColl(pats), hlog);
             if(app_data.log)
               fprintf(lf, "%c hlog\n", inOutChar());
             clearInfo();

             time(&lastChanged);            /* setze Zeitstempel neu */
             updateElems();
             break;
    case 1:  /* FFT */
             if(! isPowerOf2((unsigned) numCols(pats)))
	     { popupErrDlg(topLevel, "vistra info",
                  "FFT can only be applied to patterns with 2^N dimensions!");
               return;
             }

             showInfo("Performing FFT transformation...");
             fftRows(editedVecColl(pats));
             clearInfo();
             if(error) { xhandleErr(error); return; }
             if(app_data.log)
               fprintf(lf, "%c fft\n", inOutChar());

             time(&lastChanged);            /* setze Zeitstempel neu */
             updateElems();
             break;
    case 2:  /* PCA */
             { FILE *pipe, *pcaIn, *pcaOut;
               char *tempIn, *tempOut, buf[512];

               /* Schreibe Patterns in ein temporaeres LVQ-File */
               if(! (tempIn = tempnam(NULL,"@PCA_IN")))
                 { xhandleErr(39); return; }
               if(! (tempOut = tempnam(NULL, "@PCA_OUT")))
                 { free(tempIn); xhandleErr(39); return; }
               if(! (pcaIn = fopen(tempIn, "w")))
               { free(tempIn); free(tempOut);
                 sprintf(errorInfo, "Temporary file: %s", tempIn);
                 xhandleErr(34); return; 
               }
               showInfo("Writing temporary PCA input file...");
               lvqWrite(pats, pcaIn, inOutSwitch==EDIT_INPUT);
               fclose(pcaIn);
               clearInfo();
               if(error) 
               { unlink(tempIn); free(tempIn); free(tempOut);
                 xhandleErr(35); return; 
               }
               showInfo("Performing PCA transformation...");
               sprintf(buf, "pca %s > %s", tempIn, tempOut);
               pipe = popen(buf, "r");
               clearInfo();
               if(! pipe) 
               { unlink(tempIn); free(tempIn); 
                 unlink(tempOut); free(tempOut);
                 xhandleErr(36); return; 
               }
               pclose(pipe);
               unlink(tempIn);
               free(tempIn);
               if(! (pcaOut = fopen(tempOut, "r")))
                 { unlink(tempOut); xhandleErr(38); return; }
               showInfo("Reading temporary PCA output file...");
               lvqRead(pats, pcaOut, inOutSwitch==EDIT_INPUT);
               fclose(pcaOut);
               unlink(tempOut);
               free(tempOut);
               clearInfo();
               if(error) { xhandleErr(37); return; }
               if(app_data.log) fprintf(lf, "%c pca\n", inOutChar());
               time(&lastChanged);            /* setze Zeitstempel neu */
	       updateElems();
             }
             break;
    case 3:  /* Normalisiere */
             showInfo("Normalizing...");
             doRows(editedVecColl(pats), normalize);
             if(app_data.log)
               fprintf(lf, "%c normalize\n", inOutChar());
             clearInfo();

             time(&lastChanged);            /* setze Zeitstempel neu */
             updateElems();
             break;
    case 4:  /* Skaliere */
             popupStr2Dlg(topLevel,
                  "scale vectors",                /* Titel */
                  "Specify the new range:",       /* Headline */
                  "lowest number:",               /* Label fuer 1.String */
                  "highest number:",              /* Label fuer 2.String */
                  "", "",                         /* Default values */
                  "scale",                        /* Button label */
                  scaleVectors);                  /* Callback */
             break;
    case 5:  /* Randomize */
             vert = scalarsVertAsVec();
             if(error) { xhandleErr(error); return; }
             showInfo("Randomizing...");
             randomize(pats, vert);
             clearInfo();
             if(error) { xhandleErr(error); return; }
             if(app_data.log)
               fprintf(lf, "i randomize\n");

             updateElems();
             updateClassLabels();
             updateScalarsVert();
             if(swapSSWOpened) updateSSW(swapSSW);
             break;
    case 6:  /* Erweiter mit Klassenvektor */
             { Vector h, pad;

               h = scalarsHorizAsVec();
               if(error) { xhandleErr(error); return; }
               if(! (pad = newVector(maxClassNo(pats))))
                 { xhandleErr(1); return; }
               putAllDim(pad, 0);

               showInfo("Expanding with class vectors...");
               expandWithClassVectors(pats, inOutSwitch==EDIT_INPUT);
               clearInfo();
               if(error) xhandleErr(error);
	       if(! (horiz = expand(h, pad))) { xhandleErr(1); return; }
               freeVector(h);
               freeVector(pad);
               if(app_data.log)
                 fprintf(lf, "%c classExpand\n", inOutChar());

               time(&lastChanged);            /* setze Zeitstempel neu */
               updateHorizScrollBar();
               if(usedCols != visDims)
	       { usedCols = min(visDims, numCols(pats) - firstDim + 1);
		 updateDimLabels();
                 updateElems();
                 updateScalarsHoriz();
               }
             }
             break;
    case 7:  /* Erweiter mit Ausgabevektor */
             { Vector h, pad;

               h = scalarsHorizAsVec();
               if(error) { xhandleErr(error); return; }
               if(! (pad = newVector(outputDims(pats))))
                 { xhandleErr(1); return; }
               putAllDim(pad, 0);

               showInfo("Expanding with output vectors...");
               expandWithOutputs(pats);
               clearInfo();
               if(error) xhandleErr(error);
               if(! (horiz = expand(h, pad))) { xhandleErr(1); return; }
               freeVector(h);
               freeVector(pad);
               if(app_data.log)
                 fprintf(lf, "i outputExpand\n");

               time(&lastChanged);            /* setze Zeitstempel neu */
               updateHorizScrollBar();
               if(usedCols != visDims)
	       { usedCols = min(visDims, numCols(pats) - firstDim + 1);
		 updateDimLabels();
                 updateElems();
                 updateScalarsHoriz();
               }
             }
             break;
    case 8:  /* Berechne die Klassen-Nummern neu */
             if(! hasClassNames(pats) && hasOutputs(pats))
             { showInfo("Refreshing class numbers...");
               genClassNosFromVectors(pats);
               clearInfo();
               if(error) { xhandleErr(error); return; }
               if(app_data.log)
                 fprintf(lf, "%c refreshClasses\n", inOutChar()); 
               updateClassLabels();
             }
             break;
    default: sprintf(errorInfo, "transformMenuSelect() called with item number %d!",
                       itemNo);
             xhandleErr(20);
             return;
             break;
  }   /* switch */
}     /* transformMenuSelect */


/***************************************************************************/
/* Wird immer aufgerufen, wenn irgendein Menupunkt des Remove-Menus aus-   */
/* gewaehlt wird. garbage ist ohne Bedeutung. client_data gibt die Nummer  */
/* des ausgewaehlten Menuepunkts an.                                       */
/***************************************************************************/
static void removeMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data;
XtPointer garbage;
{ int itemNo = (int) client_data;
  unsigned numDims = (unsigned) numCols(pats);

  switch(itemNo)
  {
    case 0:  /* Entferne Dimensionen */
	     popupInt2Dlg(topLevel, "remove dimensions",
		  "Remove all dimensions", "from:", "to:",
		  1, numDims, 1, numDims, 1, 1, "remove",
                  arrowUp, arrowDown, arrowUp, arrowDown,
                  removeDimensions, NULL,
                  popdown, (XtPointer) int2DlgShell);
             break;
    case 1:  /* Entferne konstante Dimensionen */
             { Vector v;
               unsigned ret;
               long *lp, i;
  
               v = scalarsHorizAsVec();
               if(error) { xhandleErr(error); return; }
               if(! (lp = (long *) malloc((numDims+1) * sizeof(*lp))))
                 { xhandleErr(1); return; }
               showInfo("Removing constant dimensions...");
               ret = constCols(editedVecColl(pats), lp);
               if(ret == 0) { clearInfo(); return; }
               if(ret == numDims)
               { free(lp);
                 xhandleErr(25);
                 clearInfo();
                 return;
               }
               removeDimList(pats, inOutSwitch==EDIT_INPUT, lp, ret); 
               clearInfo();
               if(error) { xhandleErr(error); return; }
               if(app_data.log)
               { fprintf(lf, "%c rmDims", inOutChar());
                 for(i = 0L; i < ret; i++) fprintf(lf, " %ld", lp[i]);
                 fprintf(lf, "\n");
               }   
               free(lp);

               firstDim = 1;
               usedCols = min(numCols(pats), visDims);
               time(&lastChanged);            /* setze Zeitstempel neu */
               updateDimLabels();
               updateElems();
               updateScalarsHoriz();
               updateHorizScrollBar();
             }
             break;
    case 2:  /* Entferne Vektoren */
	     popupInt2Dlg(topLevel, "remove vectors",
		  "Remove all vectors", "from nr:", "to nr:",
		  1, num(pats), 1, num(pats), 1, 1, "remove",
		  arrowUp, arrowDown, arrowUp, arrowDown,
		  removeVectors, NULL,
                  popdown, (XtPointer) int2DlgShell);
             break;
    default: sprintf(errorInfo, "removeMenuSelect() called with item number %d!",
                       itemNo);
             xhandleErr(20);
             return;
	     break;
  }   /* switch */
}     /* removeMenuSelect */


/***************************************************************************/
/* Wird immer aufgerufen, wenn irgendein Menupunkt des Show-Menus ausge-   */
/* waehlt wird. garbage ist ohne Bedeutung. client_data gibt die Nummer    */
/* des ausgewaehlten Menuepunkts an.                                       */
/***************************************************************************/
static void showMenuSelect(Widget w, XtPointer client_data, XtPointer garbage)
{ int itemNo = (int) client_data;
  unsigned numDims = (unsigned) numCols(pats);
  GW gw;

  switch(itemNo)
  {
    case 0:  if(! (gw = openGW(topLevel, "vistra graphics"))) 
               { xhandleErr(1); return; }
             if(! add(openGWs, gw)) { xhandleErr(1); return; } 
             break;
    case 1:  /* Statistik */
	     { char *contents;
               Position x, y;

               showInfo("Computing statistics...");
	       if(! (contents = statString(pats, fileName)))
		 { xhandleErr(1); return; }
               clearInfo();
	       updateTW(statistics, "statistics", contents);
	       free(contents);
               getPopupPosition(topLevel, &x, &y);
               XtVaSetValues(statistics, XtNx, x, XtNy, y, NULL);
	       XtPopup(statistics, XtGrabNonexclusive);
	     }
             break;
    case 2:  /* Kovarianz-Matrix */
             { char *contents, buf[60];
               Number **cov;
               unsigned i, j, ndims = (unsigned) numCols(pats); 
               Position x, y;

               if(! (cov = (Number **) malloc(ndims * sizeof(*cov)))) 
                 { xhandleErr(1); return; }
               for(i = 0; i < ndims; i++)
                 if(! (cov[i] = (Number *) malloc(ndims * sizeof(Number))))
                   { xhandleErr(1); return; }
               showInfo("Computing covariance matrix...");
               covariance(editedVecColl(pats), cov);
               clearInfo();
               if(error) { xhandleErr(error); return; }
               if(! (contents = matAsString(cov, ndims, ndims))) 
                 { xhandleErr(1); return; }
               sprintf(buf, "%s covariance matrix", inOutString()); 
               updateTW(statistics, buf, contents);
               free(contents);
               for(i = 0; i < ndims; i++) free(cov[i]);
               free(cov);
               getPopupPosition(topLevel, &x, &y);
               XtVaSetValues(statistics, XtNx, x, XtNy, y, NULL);
               XtPopup(statistics, XtGrabNonexclusive);            
             }
             break;
    case 3:  /* Zeige das Swap-Fenster (Input oder Output-Vektoren) */
             popupSSW(swapSSW);
             swapSSWOpened = TRUE;
             break;
    default: sprintf(errorInfo, "showMenuSelect() called with item number %d!",
                       itemNo);
             xhandleErr(20);
             return;
             break;
  }   /* switch */
}     /* showMenuSelect */


#define LOG_LOADCOMM   (scalarSwitch == FILL_VERT ? "loadVert" : "loadHoriz")

/***************************************************************************/
/* Wird immer aufgerufen, wenn irgendein Menupunkt des Load-Menus ausge-   */
/* waehlt wird. garbage ist ohne Bedeutung. client_data gibt die Nummer    */
/* des ausgewaehlten Menuepunkts an.                                       */
/***************************************************************************/
static void loadMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ int itemNo = (int) client_data;
  void (*collectFunc)();
  VecColl vc = editedVecColl(pats);
  Number konst;

  if(scalarSwitch == FILL_VERT) collectFunc = collectRows;
  else collectFunc = collectCols;

  switch(itemNo)
  {
    case 0:  /* Minimum */
	     showInfo("Loading minima...");
	     (*collectFunc)(vc, minimum, selVec());
	     clearInfo();
             if(app_data.log)
               if(scalarSwitch == FILL_VERT)
                 fprintf(lf, "%c loadVert min\n", inOutChar());
               else
               { fprintf(lf, "%c loadHoriz values ", inOutChar());
                 fprintVector(horiz, lf);
                 fprintf(lf, "\n");
               }  
             break;
    case 1:  /* Maximum */
	     showInfo("Loading maxima...");
             (*collectFunc)(vc, maximum, selVec());
	     clearInfo();
	     if(app_data.log)
               if(scalarSwitch == FILL_VERT)
                 fprintf(lf, "%c loadVert max\n", inOutChar());
               else
               { fprintf(lf, "%c loadHoriz values ", inOutChar());
                 fprintVector(horiz, lf);
                 fprintf(lf, "\n");
               }  
             break;
    case 2:  /* Average */
	     showInfo("Loading averages...");
	     (*collectFunc)(vc, avg, selVec());
	     clearInfo();
             if(app_data.log)
               if(scalarSwitch == FILL_VERT)
                 fprintf(lf, "%c loadVert avg\n", inOutChar());
               else
               { fprintf(lf, "%c loadHoriz values ", inOutChar());
                 fprintVector(horiz, lf);
                 fprintf(lf, "\n");
               }  
             break;
    case 3:  /* Length */
	     showInfo("Loading lengths...");
	     (*collectFunc)(vc, length, selVec());
	     clearInfo();
             if(app_data.log)
               if(scalarSwitch == FILL_VERT)
                 fprintf(lf, "%c loadVert length\n", inOutChar());
               else
               { fprintf(lf, "%c loadHoriz values ", inOutChar());
                 fprintVector(horiz, lf);
                 fprintf(lf, "\n");
               }  
             break;
    case 4:  /* Standardabweichung */
	     showInfo("Loading standard deviations...");
	     (*collectFunc)(vc, sigma, selVec());
	     clearInfo();
             if(app_data.log)
               if(scalarSwitch == FILL_VERT) 
                 fprintf(lf, "%c loadVert stddev\n", inOutChar());
               else
               { fprintf(lf, "%c loadHoriz values ", inOutChar());
                 fprintVector(horiz, lf);
                 fprintf(lf, "\n");
               }  
             break;
    case 5:  /* Sum */
	     showInfo("Loading sums...");
	     (*collectFunc)(vc, sum, selVec());
	     clearInfo();
             if(app_data.log)
               if(scalarSwitch == FILL_VERT)
                 fprintf(lf, "%c loadVert sum\n", inOutChar());
               else
               { fprintf(lf, "%c loadHoriz values ", inOutChar());
                 fprintVector(horiz, lf);
                 fprintf(lf, "\n");
               }  
             break;
    case 6:  /* Load Nr */
             popupStrDlg(topLevel, "Load row/column nr:",
                         "", "load", loadNr);
             return;
    case 7:  /* Overall Minimum */
	     XtVaSetValues(infoLabel, 
                 XtNlabel, "Loading overall minimum...", NULL);
             putAllDim(selVec(), (konst = overallMin(vc)));
             clearInfo();
             if(app_data.log)
               fprintf(lf, "%c %s const %g\n",inOutChar(),LOG_LOADCOMM,konst);
             break;
    case 8:  /* Overall Maximum */
	     showInfo("Loading overall maximum...");
	     putAllDim(selVec(), (konst = overallMax(vc)));
	     clearInfo();
             if(app_data.log)
               fprintf(lf, "%c %s const %g\n",inOutChar(),LOG_LOADCOMM,konst);
             break;
    case 9:  /* Overall Average */
	     showInfo("Loading overall average...");
	     putAllDim(selVec(), (konst = overallAvg(vc)));
	     clearInfo();
             if(app_data.log)
               fprintf(lf, "%c %s const %g\n",inOutChar(),LOG_LOADCOMM,konst);
	     break;
    case 10: /* Overall Standard Deviation */
	     showInfo("Loading overall standard deviation...");
	     putAllDim(selVec(), (konst = overallStddev(vc)));
	     clearInfo();
             if(app_data.log)
               fprintf(lf, "%c %s const %g\n",inOutChar(),LOG_LOADCOMM,konst);
             break;
    case 11: /* Load Constant */
	     popupStrDlg(topLevel, "Load vector with constant:",
			 "0", "load", loadConstant);
	     return;
    default: sprintf(errorInfo, "loadMenuSelect() called with item number %d!",
                       itemNo);
             xhandleErr(20);
             return;
             break;
  }   /* switch */

  if(scalarSwitch == FILL_VERT) updateScalarsVert();
  else updateScalarsHoriz();
}     /* loadMenuSelect */


/**************************************************************/
/**************************************************************/
static void loadConstant(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ String value;
  Number n;

  value = XawDialogGetValueString((Widget) client_data);
  if(sscanf(value, "%f", &n) != 1)
  { popupErrDlg(strDlgShell, "vistra info",
		"Specified value is not a number!");
    return;
  }

  XtPopdown(strDlgShell);
  putAllDim(selVec(), n);
  if(app_data.log)
    fprintf(lf, "%c %s const %g\n", inOutChar(), LOG_LOADCOMM, n);

  /* Aktualisiere die entsprechenden Text-Widgets */
  if(scalarSwitch == FILL_VERT) updateScalarsVert();
  else updateScalarsHoriz();
}      /* loadConstant */


/**************************************************************/
/**************************************************************/
static void loadNr(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ String value;
  unsigned nr, maxAllowed;

  value = XawDialogGetValueString((Widget) client_data);
  if(sscanf(value, "%u", &nr) != 1)
  { popupErrDlg(strDlgShell, "vistra info",
		"Specified value is not a positive integer!");
    return;
  }
  maxAllowed = (unsigned) (scalarSwitch == FILL_VERT ?
                           numCols(pats) : num(pats));
  if(nr < 1 || nr > maxAllowed)
  { char buf[60];
    sprintf(buf, "Nr must be between 1 and %u!", maxAllowed);
    popupErrDlg(strDlgShell, "vistra info", buf);
    return;
  }

  XtPopdown(strDlgShell);
  if(scalarSwitch == FILL_VERT)
    colVec(editedVecColl(pats), (long) nr, vert);
  else copyVec(horiz, (Vector) at(editedVecColl(pats), (long) nr));
  if(app_data.log)
  { fprintf(lf, "%c %s values ", inOutChar(), LOG_LOADCOMM);
    fprintVector(selVec(), lf);
    fprintf(lf, "\n");
  }

  /* Aktualisiere die entsprechenden Text-Widgets */
  if(scalarSwitch == FILL_VERT) updateScalarsVert();
  else updateScalarsHoriz();

  lastNr = nr;
}      /* loadNr */


/***************************************************************************/
/* Wird immer aufgerufen, wenn irgendein Menupunkt des Format-Menus aus-   */
/* gewaehlt wird. garbage ist ohne Bedeutung. client_data gibt die Nummer  */
/* des ausgewaehlten Menuepunkts an.                                       */
/***************************************************************************/
static void formatMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data;
XtPointer garbage;
{ unsigned itemNo = (unsigned) client_data;
  long numItems = size(formatNames);

  if(numItems < itemNo)
  { sprintf(errorInfo, "formatMenuSelect() called with item number %d which is greater than the formatNames size!");
    xhandleErr(20);
    return;
  }

  if(itemNo != selectedFormat)
  {
    XtVaSetValues((Widget) at(formatMenuItems, selectedFormat),
      XtNleftBitmap, None, NULL);
    XtVaSetValues((Widget) at(formatMenuItems, itemNo),
      XtNleftBitmap, hook, NULL);
    selectedFormat = itemNo;
    updateInfoWidgets();
  }      /* if */
}        /* formatMenuSelect */


/***************************************************************/
/* Wird immer aufgerufen, wenn inputToggle oder outputToggle   */
/* gesetzt bzw. angeklickt wird.                               */
/***************************************************************/
static void toggle(Widget w, XtPointer client_data, XtPointer garbage)
{ int currentlySet;

  currentlySet = (int) XawToggleGetCurrent(inputToggle);
  /* currentlySet kann NULL enthalten, da auch ein Unset eines Toggles */
  /* einen Aufruf dieser Funktion bewirkt.                             */
  if(currentlySet)
  {
    if(currentlySet != inOutSwitch)
    {
      /* tausche Inhalte der Shells aus */
      inOutSwitch = currentlySet;
      changeInOut();
      if(error) { xhandleErr(error); return; }
      if(swapSSWOpened) updateSSW(swapSSW);
    }   /* if(currentlySet != inOutSwitch) */
  }     /* if(currentlySet) */
}       /* toggle */


/***************************************************************/
/* Wird immer aufgerufen, wenn vertToggle oder horizToggle     */
/* gesetzt bzw. angeklickt wird.                               */
/***************************************************************/
static void toggleScalars(Widget w, XtPointer client_data, XtPointer garbage)
{ int currentlySet;

  currentlySet = (int) XawToggleGetCurrent(vertToggle);
  /* currentlySet kann NULL enthalten, da auch ein Unset eines Toggles */
  /* einen Aufruf dieser Funktion bewirkt.                             */
  if(currentlySet)
  {
    if(currentlySet != scalarSwitch)
    {
      scalarSwitch = currentlySet;
      if(pats)
      { XtVaSetValues(loadMenuButtonVert,
            XtNsensitive, (scalarSwitch == FILL_VERT),
            NULL);
        XtVaSetValues(loadMenuButtonHoriz,
            XtNsensitive, (scalarSwitch == FILL_HORIZ),
            NULL);
      } /* if(pats) */
    }   /* if(currentlySet != inOutSwitch) */
  }     /* if(currentlySet) */
}       /* toggleScalars */


/**************************************************************/
/* Schliesse die Dialogbox bzw. Shell, die durch client_data  */
/* uebergeben wird.                                           */
/**************************************************************/
static void popdown(Widget w, XtPointer client_data, XtPointer garbage)
{
  XtPopdown((Widget) client_data);
}       /* popdown */


/*************************************************************/
/*************************************************************/
static void popdownSwapSSW(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ swapSSWOpened = FALSE;
  XtPopdown(swapSSW->shell);
}      /* popdownSwapSSW */


#ifdef XFWF_FILE_SELECTION
/*************************************************************/
/*************************************************************/
static void popdownFileSelector(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{ 
  XtPopdown(fileSelShell);
}       /* popdownFileSelector */
#endif


/*************************************************************/
/*************************************************************/
static void destroyTextWindow(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
  XtDestroyWidget((Widget) client_data);
}       /* destroyTextWindow */


/*************************************************************/
/* Wird aufgerufen, sobald ein Button losgelassen wird,      */
/* nachdem dieser zuvor ueber der vertikalen Scrollbar       */
/* gedrueckt wurde.                                          */
/* pos enthaelt die relative Pointer-Position in Pixels.     */
/*************************************************************/
static void scrollVertical(w, client_data, pos)
Widget w;
XtPointer client_data, pos;
{ int ptrpos = (int) pos, previous = firstVec;

  scalarsVertAsVec();
  if(error) { xhandleErr(error); return; }
  if(ptrpos > 0)                /* Button 1 wurde gedrueckt */
    firstVec = max(1, firstVec - visVecs);
  else firstVec = min(firstVec + visVecs, num(pats));
  usedRows = min(visVecs, num(pats) - firstVec + 1);

  if(firstVec != previous)
  { updateVertScrollBar();
    updateNrLabels();
    updateElems();
    updateClassLabels();
    updateScalarsVert();
  }
}      /* scrollVertical */


/*************************************************************/
/* Wird aufgerufen, sobald Button 1 oder 3 losgelassen wird, */
/* nachdem dieser zuvor ueber der horizontalen Scrollbar     */
/* gedrueckt wurde.                                          */
/* pos enthaelt die relative Pointer-Position in Pixels.     */
/*************************************************************/
static void scrollHorizontal(w, client_data, pos)
Widget w;
XtPointer client_data, pos;
{ int ptrpos = (int) pos, previous = firstDim;
 
  scalarsHorizAsVec();
  if(error) { xhandleErr(error); return; }
  if(ptrpos > 0)             /* Button 1 wurde gedrueckt */
    firstDim = max(1, firstDim - visDims);
  else firstDim = min(firstDim + visDims, numCols(pats));
  usedCols = min(visDims, numCols(pats) - firstDim + 1);
  
  if(firstDim != previous)
  { updateHorizScrollBar();
    updateDimLabels();
    updateElems();
    updateScalarsHoriz();
  } 
}      /* scrollHorizontal */


/*************************************************************/
/* Wird aufgerufen, sobald der Mouse Button 2 losgelassen    */
/* wir, nachdem dieser zuvor ueber der vertikalen Scrollbar  */
/* gedrueckt wurde.                                          */
/* percent gibt die Position des Balkens an und liegt        */
/* zwischen 0.0 und 1.0.                                     */
/*************************************************************/
static void jumpVertical(w, client_data, percent)
Widget w;
XtPointer client_data, percent;
{ float top;
  int previous = firstVec;
  long nrows = num(pats);

  scalarsVertAsVec();
  if(error) 
  { xhandleErr(error);
    updateVertScrollBar();
    return;
  } 
  top = * (float *) percent;
  firstVec = min(nrows, (int) (top * nrows) + 1);
  usedRows = min(visVecs, nrows - firstVec + 1);

  if(firstVec != previous)
  { updateNrLabels();
    updateElems();
    updateClassLabels();
    updateScalarsVert();
  }
}      /* jumpVertical */


/*************************************************************/
/* Wird aufgerufen, sobald der Mouse Button 2 losgelassen    */
/* wir, nachdem dieser zuvor ueber der horizontalen Scrollbar*/
/* gedrueckt wurde.                                          */
/* percent gibt die Position des Balkens an und liegt        */
/* zwischen 0.0 und 1.0.                                     */
/*************************************************************/
static void jumpHorizontal(w, client_data, percent)
Widget w;
XtPointer client_data, percent;
{ float top;
  int previous = firstDim;
  long ncols = numCols(pats);
  
  scalarsHorizAsVec();
  if(error) 
  { xhandleErr(error);
    updateHorizScrollBar();
    return;
  }
  top = * (float *) percent;
  firstDim = min(ncols, (int) (top * ncols) + 1);
  usedCols = min(visDims, ncols - firstDim + 1);

  if(firstDim != previous)
  { updateDimLabels();
    updateElems();
    updateScalarsHoriz();
  } 
}      /* jumpHorizontal */


/*************************************************************/
/*************************************************************/
/*           E N D E der C A L L B A C Ks                    */
/*************************************************************/
/*************************************************************/

/*************************************************************/
/*************************************************************/
/*******  E R R O R - H A N D L I N G  ***********************/
/*************************************************************/
/*************************************************************/
static void xhandleErr(int err)
{ static char message[MAX_LENGTH_ERROR_MESSAGE];
  Position x, y;

  /* bei zuwenig Speicher: beende das Programm */
  if(err == 1) handleErr(err);

  genErrorMessage(err, message);
  popupErrDlg(topLevel, "vistra info", message);
}        /* xhandleErr */

