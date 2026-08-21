/**********************************************************************/
/**********************************************************************/
/***** VISTRA - Main file                                         *****/
/***** ---------------------------------------------------------- *****/
/***** This file contains the main function and all callbacks     *****/
/***** related to the main window of Vistra.                      *****/
/*****                                                            *****/
/***** The following files are included:                          *****/
/*****   "crtwdgts.c" - Functions that create widgets.            *****/
/*****                  (in particular those of the main window)  *****/
/*****   "uif.c"  - Functions of the graphical user interface     *****/
/*****              that are not callbacks.                       *****/
/*****             (among others, update routines for the main    *****/
/*****              window widgets)                                *****/
/*****   "init.c" - Initialization of Vistra.                     *****/
/*****   "draw.c" - Drawing routines for the graphics windows.    *****/
/*****   "gw.c"   - Creation, control and destruction of graphics *****/
/*****              windows.                                      *****/
/*****   "int2.c" - Routines of the dialog box in which 2         *****/
/*****              integers are entered.                         *****/
/*****   "ssw.c"  - Routines of the SpreadSheet window.           *****/
/*****              The SpreadSheet window is the window that     *****/
/*****              contains the vectors that are currently not   *****/
/*****              being edited in the main window.               *****/
/**********************************************************************/
/**********************************************************************/

#include "xvis.h"
#include "def.h"
#include <math.h>
#include <time.h>
#include <unistd.h>
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
static void initialize(int argc, char **argv);                             /* sets error */
static void initialize2(Widget);
/*** UIF.C ***/
static void fillPatternView(void);                        /* sets error */
static void changeInOut(void);                            /* sets error */
static Format nameToFormat(char *);                   /* sets error */
#ifdef XFWF_FILE_SELECTION
static void popupFileSelector(Widget w, char *title, XtCallbackProc ok_cb);
#endif
static void popupStr2Dlg(Widget w, char *title, char *headline, char *item1, char *item2, char *default1, char *default2, char *okButtonLabel, XtCallbackProc callback);
static void popupErrDlg(Widget, char *, char *);
static void getStr2DlgValues(char **, char **);
static void setActivation(Patterns);
static Vector scalarVec(void);                            /* sets error */
static Vector scalarsVertAsVec(void);                     /* sets error */
static Vector scalarsHorizAsVec(void);                    /* sets error */
static void fixSize(Widget);
static void updateInfoWidgets(void);
static void updateScalarsHoriz(void);
static void updateScalarsVert(void);
static void updateDimLabels(void);
static void updateNrLabels(void);
static void updateClassLabels(void);
static void updateElems(void);
static void updateVertScrollBar(void);
static void updateHorizScrollBar(void);
static void updateTW(Widget, char *, char *);
static void showInfo(char *);
static void exposeWidget(Widget);
/*** SSW.C ***/
static void updateSSW(SSW);
static void popupSSW(SSW);
/*** CRTWDGTS.C ***/
static void createAllWidgets(Widget);                 /* sets error */
/*** INT2.C ***/
static void popupInt2Dlg(Widget w, char *title, char *headline, char *item1, char *item2, int from1, int to1, int from2, int to2, int default1, int default2, char *okButtonLabel, XtCallbackProc up1_cb, XtCallbackProc down1_cb, XtCallbackProc up2_cb, XtCallbackProc down2_cb, XtCallbackProc ok_cb, XtPointer ok_data, XtCallbackProc cancel_cb, XtPointer cancel_data);
static void updateInt2Text(void);
/*** STRDLG.C ***/
static void popupStrDlg(Widget w, char *title, char *defaultValue, char *buttonLabel, XtCallbackProc callback);
/*** GW.C ***/
static GW openGW(Widget, char *);
static void freeGW(GW);
/*** DRAW.C ***/
static void initDrawer(Widget);
static void drawColorArray(Drawable, Boolean, Vector, unsigned, unsigned,
                           unsigned, unsigned, Range, long, long);
static void drawHistogram(GW, Vector, Range, PixelNo);   /* sets error */
static void drawPolyLine(GW, Vector, Range, PixelNo);    /* sets error */
static void draw2DProjection(GW, VecColl, long, long, PixelNo);
/* sets error */
static unsigned drawHistogrammes(GW, VecColl, Range, PixelNo);
/* sets error */
static unsigned drawPolyLines(GW, VecColl, Range, PixelNo);
/* sets error */
static unsigned drawMatrices(GW, Boolean, VecColl, Range, long, long);
/* sets error */

/***************************************************************************/
/******** CALLBACKs ********************************************************/
/***************************************************************************/
static void toggle(Widget, XtPointer, XtPointer);
static void toggleScalars(Widget, XtPointer, XtPointer);
static void loadPatterns(Widget, XtPointer, XtPointer);
static void writePatterns(Widget, XtPointer, XtPointer);
static void loadSymbols(Widget, XtPointer, XtPointer);
static void writeSymbols(Widget, XtPointer, XtPointer);
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
/******* User interface variables that are not widgets ***********************/
/****************************************************************************/
static int inOutSwitch;
/* EDIT_INPUT:  edit input pattern       */
/* EDIT_OUTPUT: edit output pattern      */
static int scalarSwitch;
/* FILL_HORIZ:  column operation         */
/* FILL_VERT:   row operation            */
static Patterns pats;
/* The patterns that are currently loaded.            */
/* NULL, if none are loaded.                          */
static char *fileName;
/* Name of the pattern file that is currently loaded. */
/* NULL, if no patterns are currently loaded.         */
static Collection formatNames;
/* Collection('see Suffix' <fmt1> <fmt2> ... <fmt3>)  */
/* <fmti>: Name of a format file without the '.fmt'   */
/* suffix. The strings simultaneously serve as the    */
/* labels of the format menu's items.                 */
static Collection formats;
/* Collection of all syntactically correct ASCII      */
/* formats found on disk.                             */
/* size(formats) == size(formatNames) - 3 always      */
/* holds.                                              */
static Collection formatMenuItems;
/* Collection of all smeBSBObjects that represent the */
/* items of the format menu.                          */
/* size(formatMenuItems) == size(formatNames) always  */
/* holds.                                              */
static int selectedFormat;
/* Index of the currently selected format menu item   */
/* in the formatNames collection.                     */
static time_t lastChanged;
/* Timestamp of the last modification of the input or */
/* output vectors that results in a new global        */
/* minimum or maximum.                                */
/* GWs compare their timestamp with lastChanged to    */
/* know whether the minima and maxima need to be      */
/* recomputed.                                        */
static Pixmap hook;
/* Checkmark for selected menu items.                 */
static SSW swapSSW;   /* SSW for displaying the input or output vectors     */
/* (read-only window with spreadsheet).               */
static Boolean swapSSWOpened;
/* True: input/output vectors are displayed in a      */
/*       separate window (swapSSW) (read only).       */
/* False: swap window is not open.                    */
static FILE *lf;      /* Log file                                           */
static Boolean logFirstTime;
/* TRUE iff first log period                          */
static unsigned lastNr;
/* Number of the row or column vector most recently   */
/* loaded via 'Load Nr...'.                           */
static char infoBuf[512];
/* for formatting the info messages of infoLabel.     */
static XFontStruct *gwFont;
/* Font for the commands of the graphics windows      */
static Collection openGWs;
/* Collection of all currently open GWs.              */
static unsigned depth;
/* Depth of the screen.                               */
static int screen;    /* The screen on which Vistra runs.                   */
static Display *display;    /* The display (needed by X)                    */

/****************************************************************************/
/***** VARIABLES for controlling the MAIN WINDOW. ***************************/
/****************************************************************************/
static Vector vert, horiz;
/* Vertical and horizontal scalar vector.             */
static int firstVec;  /* Contains the number of the first vector that is    */
/* currently visible in the main window.              */
static int firstDim;  /* Contains the number of the dimension that is still */
/* visible on the left in the main window.            */
static int visVecs;   /* Number of visible vectors.                         */
static int visDims;   /* Number of visible dimensions.                      */
static int usedRows;  /* Number of rows used in the pattern view.           */
static int usedCols;  /* Number of columns used in the pattern view.        */

/***** variables required for drawing ****************************************/
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
/************* M A I N   P R O G R A M ********************************/
/*********************************************************************/
/*********************************************************************/
int main(int argc, char **argv) {
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
/* Called as soon as the 'Load' button of the "dialog"       */
/* dialog is clicked.                                        */
/* Loads the patterns from the file whose name was typed     */
/* into the dialog. On success, the patterns are stored in   */
/* the variable pats. The format of the pattern file is      */
/* given by the variable format.                              */
/*************************************************************/
static void loadPatterns(Widget w, XtPointer client_data, XtPointer ret) {
    char *suffix, *format_name;
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

    /* determine the format name and, if applicable, the corresponding ASCII format */
    if(selectedFormat == 1) {
        /* 'See Suffix' */
        suffix = strrchr(value, '.');
        if(! suffix) {
            sprintf(errorInfo, "File:  %s", value);
            xhandleErr(21);
            return;
        }
        format_name = ++suffix;
    }                                    /* if(selectedFormat == 1) */
    else format_name = (char *) at(formatNames, selectedFormat);
    readFmt = nameToFormat(format_name);
    if(error) {
        xhandleErr(error);
        return;
    }

    /* open the pattern file */
    f = fopen(value, "r");
    if(! f) {
        sprintf(errorInfo, "Pattern file:  %s", value);
        xhandleErr(11);
        return;
    }

    /* read in the file */
    showInfo("Loading patterns...");
    if(streq(format_name, N01_MENU_LABEL)) p = readN01(f);
    else if(streq(format_name, LVQ_MENU_LABEL)) p = readLVQ(f);
    else {
        p = newPatterns();
        if(error) {
            xhandleErr(error);
            return;
        }
        fileIn(p, readFmt, f);
    }
    fclose(f);
    clearInfo();
    if(error) {
        free(value);
        xhandleErr(error);
        return;
    }
    /* patterns loaded successfully */
    if(fileName) {
        free(fileName);
        fileName = NULL;
    }
    fileName = my_strdup(value);

    if(pats) freePatterns(pats);
    pats = p;
    /* close all open graphics windows */
    if(notEmpty(openGWs))
        freeFromTo(openGWs, 1L, size(openGWs), (void(*)(void*))freeGW);

    fillPatternView();
    if(error) handleErr(error);

    /* close or update the swap window, if open */
    if(swapSSWOpened) {
        if(hasOutputs(pats)) updateSSW(swapSSW);
        else {
            Widget dummy;
            popdownSwapSSW(dummy, NULL, NULL);
        }
    }
}       /* loadPatterns */


/*************************************************************/
/* Called as soon as the 'Write' button of the string        */
/* dialog is clicked.                                        */
/*************************************************************/
static void writePatterns(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    char *value, *suffix, *format_name;
    FILE *f;
    Format writeFmt;

    XtPopdown(strDlgShell);
    value = XawDialogGetValueString((Widget) client_data);

    /* determine the format name and, if applicable, the write ASCII format */
    if(selectedFormat == 1) {
        /* 'See Suffix' */
        suffix = strrchr(value, '.');
        if(! suffix) {
            sprintf(errorInfo, "File:  %s", value);
            xhandleErr(21);
            return;
        }
        format_name = ++suffix;
    }                                  /* if(selectedFormat == 1) */
    else format_name = (char *) at(formatNames, selectedFormat);
    writeFmt = nameToFormat(format_name);
    if(error) {
        xhandleErr(error);
        return;
    }

    /* open the output file */
    f = fopen(value, "w");
    if(! f) {
        sprintf(errorInfo, "Output file:  %s", value);
        xhandleErr(12);
        return;
    }

    /* write the pattern file */
    sprintf(infoBuf, "Writing patterns to %s...", value);
    showInfo(infoBuf);
    if(streq(format_name, N01_MENU_LABEL)) writeN01(pats, f);
    else if(streq(format_name, LVQ_MENU_LABEL)) writeLVQ(pats, f);
    else fileOut(pats, writeFmt, f);
    fclose(f);
    clearInfo();
    if(error) {
        xhandleErr(error);
        return;
    }
}       /* writePatterns */


/******************************************************/
/* Reads in the symtab file whose name was entered    */
/* via the client_data dialog widget.                 */
/******************************************************/
static void loadSymbols(w, client_data, ret)
Widget w;
XtPointer client_data, ret;
{
    FILE *f;
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
    if(! f) {
        xhandleErr(14);
        return;
    }

    /* read the symtab file */
    sprintf(infoBuf, "Loading symbols from %s...", value);
    showInfo(infoBuf);
    st = readSymtab(f);
    fclose(f);
    clearInfo();
    if(error) xhandleErr(error);
    if(numSymbols(st) < maxClassNo(pats)) {
        freeSymtab(st);
        xhandleErr(33);
        return;
    }
    /* replace the previous class names with the new ones from st */
    XtVaSetValues(infoLabel, XtNlabel, "Replacing symbols...", NULL);
    replaceClasses(pats, st);
    clearInfo();
    if(error) {
        freeSymtab(st);
        xhandleErr(error);
        return;
    }
    /* update the widgets */
    updateClassLabels();
}        /* loadSymbols */


/******************************************************/
/* Writes the symbols of the patterns pats to the file*/
/* whose name was entered via the client_data dialog  */
/* widget.                                            */
/******************************************************/
static void writeSymbols(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    char *value;
    FILE *f;

    XtPopdown(strDlgShell);
    value = XawDialogGetValueString((Widget) client_data);

    f = fopen(value, "w");
    if(! f) {
        sprintf(errorInfo, "Output file:  %s", value);
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
{
    Vector vec;
    VecColl edited = editedVecColl(pats);

    vec = scalarVec();
    if(error) {
        xhandleErr(error);
        return;
    }
    showInfo("Adding scalars...");
    if(scalarSwitch == FILL_VERT) {
        compScalarsRow(edited, vec, '+');
        if(app_data.log)
            fprintf(lf, "%c add vert\n", inOutChar());
    } else {
        compScalarsCol(edited, vec, '+');
        if(app_data.log)
            fprintf(lf, "%c add horiz\n", inOutChar());
    }
    clearInfo();
    time(&lastChanged);            /* reset timestamp */
    updateElems();
}       /* addScalars */


/******************************************************/
/******************************************************/
static void subScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    Vector vec;
    VecColl edited = editedVecColl(pats);

    vec = scalarVec();
    if(error) {
        xhandleErr(error);
        return;
    }
    showInfo("Subtracting scalars...");
    if(scalarSwitch == FILL_VERT) {
        compScalarsRow(edited, vec, '-');
        if(app_data.log)
            fprintf(lf, "%c subtract vert\n", inOutChar());
    } else {
        compScalarsCol(edited, vec, '-');
        if(app_data.log)
            fprintf(lf, "%c subtract horiz\n", inOutChar());
    }
    clearInfo();
    time(&lastChanged);            /* reset timestamp */
    updateElems();
}       /* subScalars */


/******************************************************/
/******************************************************/
static void multScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    Vector vec;
    VecColl edited = editedVecColl(pats);

    vec = scalarVec();
    if(error) {
        xhandleErr(error);
        return;
    }
    showInfo("Multiplying scalars...");
    if(scalarSwitch == FILL_VERT) {
        compScalarsRow(edited, vec, '*');
        if(app_data.log)
            fprintf(lf, "%c multiply vert\n", inOutChar());
    } else {
        compScalarsCol(edited, vec, '*');
        if(app_data.log)
            fprintf(lf, "%c multiply horiz\n", inOutChar());
    }
    clearInfo();
    time(&lastChanged);            /* reset timestamp */
    updateElems();
}       /* multScalars */


/******************************************************/
/******************************************************/
static void divScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    Vector vec;
    VecColl edited = editedVecColl(pats);

    vec = scalarVec();
    if(error) {
        xhandleErr(error);
        return;
    }
    if(detectFirst(vec, 0)) {
        xhandleErr(24);
        return;
    }
    showInfo("Dividing by scalars...");
    if(scalarSwitch == FILL_VERT) {
        compScalarsRow(edited, vec, '/');
        if(app_data.log)
            fprintf(lf, "%c divide vert\n", inOutChar());
    } else {
        compScalarsCol(edited, vec, '/');
        if(app_data.log)
            fprintf(lf, "%c divide horiz\n", inOutChar());
    }
    clearInfo();
    time(&lastChanged);            /* reset timestamp */
    updateElems();
}       /* divScalars */


/******************************************************/
/******************************************************/
static void replaceScalars(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    char buf[15];
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
{
    String value;
    unsigned nr, maxAllowed;
    Vector vec;
    char buf[70];

    XtPopdown(strDlgShell);
    value = XawDialogGetValueString((Widget) client_data);
    if(sscanf(value, "%u", &nr) != 1) {
        popupErrDlg(strDlgShell, "vistra info",
                    "Value must be a positive integer!");
        return;
    }
    maxAllowed = (scalarSwitch == FILL_VERT ? numCols(pats) : num(pats));
    if(nr < 1 || nr > maxAllowed) {
        sprintf(buf, "Value must be between 1 and %u!", maxAllowed);
        popupErrDlg(strDlgShell, "vistra info", buf);
        return;
    }

    vec = scalarVec();
    if(error) {
        xhandleErr(error);
        return;
    }
    if(scalarSwitch == FILL_VERT) {
        replaceCol(editedVecColl(pats), nr, vec);
        if(app_data.log)
            fprintf(lf, "%c replace vert %u\n", inOutChar(), nr);
    } else {
        replaceRow(editedVecColl(pats), nr, vec);
        if(app_data.log)
            fprintf(lf, "%c replace horiz %u\n", inOutChar(), nr);
    }

    time(&lastChanged);            /* reset timestamp */
    if((scalarSwitch == FILL_VERT && nr>=firstDim && nr<firstDim+usedCols) ||
            (scalarSwitch == FILL_HORIZ && nr>=firstVec && nr<firstVec+usedRows))
        updateElems();
}       /* replVec */


/*******************************************************/
/*******************************************************/
static void scaleVectors(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    char *smallest, *biggest;
    Number small, big;
    ScaleOp so;

    getStr2DlgValues(&smallest, &biggest);
    if(sscanf(smallest, "%f", &small) != 1) {
        popupErrDlg(str2DlgShell, "vistra info",
                    "Value for lowest is not a number!");
        return;
    }
    if(sscanf(biggest, "%f", &big) != 1) {
        popupErrDlg(str2DlgShell, "vistra info",
                    "Value for highest is not a number!");
        return;
    }
    if(small > big) {
        popupErrDlg(str2DlgShell, "vistra info",
                    "Highest number smaller than lowest!");
        return;
    }

    XtPopdown(str2DlgShell);
    sprintf(infoBuf, "Scaling to range [%g,%g]...", small, big);
    showInfo(infoBuf);
    scaleAll(editedVecColl(pats), small, big, &so);
    if(app_data.log) {
        fprintf(lf, "%c multConst %g\n", inOutChar(), so.mult);
        fprintf(lf, "%c addConst %g\n", inOutChar(), so.add);
    }
    clearInfo();

    time(&lastChanged);            /* reset timestamp */
    updateElems();
}       /* scaleVectors */


/*******************************************************/
/*******************************************************/
static void removeVectors(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    char *smallest, *biggest;
    unsigned small, big, ret;

    XtVaGetValues(int2Text[0], XtNstring, &smallest, NULL);
    XtVaGetValues(int2Text[1], XtNstring, &biggest, NULL);
    if(sscanf(smallest, "%u", &small) != 1 ||
            sscanf(biggest, "%u", &big) != 1) {
        popupErrDlg(int2DlgShell, "vistra info",
                    "Values must be positive integers!");
        return;
    }

    XtPopdown(int2DlgShell);
    if(small <= big) {
        Vector v;

        v = scalarsVertAsVec();
        if(error) {
            xhandleErr(error);
            return;
        }
        sprintf(infoBuf, "Removing patterns %u-%u...", small, big);
        showInfo(infoBuf);
        ret = removePatterns(pats, small, big);
        clearInfo();
        if(error) {
            xhandleErr(error);
            return;
        }
        removeDimRange(v, small, big);
        if(error) {
            xhandleErr(error);
            return;
        }

        /* update the affected widgets */
        if(ret) {
            firstVec = 1;
            usedRows = min(num(pats), visVecs);
            time(&lastChanged);            /* reset timestamp */
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
{
    char *smallest, *biggest;
    unsigned small, big;

    XtVaGetValues(int2Text[0], XtNstring, &smallest, NULL);
    XtVaGetValues(int2Text[1], XtNstring, &biggest, NULL);
    if(sscanf(smallest, "%u", &small) != 1 ||
            sscanf(biggest, "%u", &big) != 1) {
        popupErrDlg(int2DlgShell, "vistra info",
                    "Values must be positive integers!");
        return;
    }

    XtPopdown(int2DlgShell);
    if(small <= big) {
        unsigned ret;
        Vector v;

        v = scalarsHorizAsVec();
        if(error) {
            xhandleErr(error);
            return;
        }
        ret = removeCols(pats, inOutSwitch==EDIT_INPUT, small, big);
        if(error) {
            xhandleErr(error);
            return;
        }
        sprintf(infoBuf, "Removing dimensions %u-%u...", small, big);
        showInfo(infoBuf);
        removeDimRange(v, small, big);
        clearInfo();
        if(error) {
            xhandleErr(error);
            return;
        }
        if(app_data.log)
            fprintf(lf, "%c rmDimRange %u %u\n", inOutChar(), small, big);

        /* update the affected widgets */
        if(ret) {
            firstDim = 1;
            usedCols = min(numCols(pats), visDims);
            time(&lastChanged);            /* reset timestamp */
            updateDimLabels();
            updateElems();
            updateScalarsHoriz();
            updateHorizScrollBar();
        }
    }
}       /* removeDimensions */


/***************************************************************************/
/* Always called when any item of the File menu is selected. garbage is   */
/* meaningless. client_data returns the number of the selected menu item. */
/***************************************************************************/
static void fileMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    int itemNo = (int) client_data;

    switch(itemNo) {
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
        if(! app_data.log) {
            char *buf, *info;
            if(! (buf = (char *) malloc(strlen(fileName) + 6))) {
                xhandleErr(1);
                return;
            }
            sprintf(buf, "%s.log", fileName);
            if(! (lf = fopen(buf, (logFirstTime?"w":"a")))) {
                if(! (info = (char *) malloc(strlen(buf) + 50))) {
                    xhandleErr(1);
                    return;
                }
                sprintf(info, "Cannot open log file %s!", buf);
                popupErrDlg(topLevel, "vistra warning", info);
                free(buf);
                free(info);
                return;
            }
            free(buf);
        } else {
            fclose(lf);
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
    default:
        sprintf(errorInfo, "fileMenuSelect() called with item number %d !",
                itemNo);
        xhandleErr(20);
        break;
    }   /* switch */
}     /* fileMenuSelect */


/***************************************************************************/
/* Always called when any item of the Transform menu is selected. garbage */
/* is meaningless. client_data gives the number of the selected menu item.*/
/***************************************************************************/
static void transformMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data;
XtPointer garbage;
{
    int itemNo = (int) client_data;

    switch(itemNo) {
    case 0:  /* HLOG */
        if(! isSquare((unsigned) numCols(pats))) {
            popupErrDlg(topLevel, "vistra info",
                        "HLOG can only be applied to quadratic patterns!");
            return;
        }

        showInfo("Performing HLOG transformation...");
        doRows(editedVecColl(pats), hlog);
        if(app_data.log)
            fprintf(lf, "%c hlog\n", inOutChar());
        clearInfo();

        time(&lastChanged);            /* reset timestamp */
        updateElems();
        break;
    case 1:  /* FFT */
        if(! isPowerOf2((unsigned) numCols(pats))) {
            popupErrDlg(topLevel, "vistra info",
                        "FFT can only be applied to patterns with 2^N dimensions!");
            return;
        }

        showInfo("Performing FFT transformation...");
        fftRows(editedVecColl(pats));
        clearInfo();
        if(error) {
            xhandleErr(error);
            return;
        }
        if(app_data.log)
            fprintf(lf, "%c fft\n", inOutChar());

        time(&lastChanged);            /* reset timestamp */
        updateElems();
        break;
    case 2: { /* PCA */
        FILE *pipe, *pcaIn, *pcaOut;
        char *tempIn, *tempOut, buf[512];
        char tempInBuf[] = "@PCA_INXXXXXXXX", tempOutBuf[] = "@PCA_OUTXXXXXXXX";
        int tempInFd, tempOutFd;

        /* write patterns to a temporary LVQ file */
        if((tempInFd = mkstemp(tempInBuf)) == -1) {
            xhandleErr(39);
            return;
        }
        close(tempInFd);
        tempIn = tempInBuf;
        if((tempOutFd = mkstemp(tempOutBuf)) == -1) {
            xhandleErr(39);
            return;
        }
        close(tempOutFd);
        tempOut = tempOutBuf;
        if(! (pcaIn = fopen(tempIn, "w"))) {
            sprintf(errorInfo, "Temporary file: %s", tempIn);
            xhandleErr(34);
            return;
        }
        showInfo("Writing temporary PCA input file...");
        lvqWrite(pats, pcaIn, inOutSwitch==EDIT_INPUT);
        fclose(pcaIn);
        clearInfo();
        if(error) {
            unlink(tempIn);
            xhandleErr(35);
            return;
        }
        showInfo("Performing PCA transformation...");
        sprintf(buf, "pca %s > %s", tempIn, tempOut);
        pipe = popen(buf, "r");
        clearInfo();
        if(! pipe) {
            unlink(tempIn);
            unlink(tempOut);
            xhandleErr(36);
            return;
        }
        pclose(pipe);
        unlink(tempIn);
        if(! (pcaOut = fopen(tempOut, "r"))) {
            unlink(tempOut);
            xhandleErr(38);
            return;
        }
        showInfo("Reading temporary PCA output file...");
        lvqRead(pats, pcaOut, inOutSwitch==EDIT_INPUT);
        fclose(pcaOut);
        unlink(tempOut);
        clearInfo();
        if(error) {
            xhandleErr(37);
            return;
        }
        if(app_data.log) fprintf(lf, "%c pca\n", inOutChar());
        time(&lastChanged);            /* reset timestamp */
        updateElems();
    }
    break;
    case 3:  /* Normalize */
        showInfo("Normalizing...");
        doRows(editedVecColl(pats), normalize);
        if(app_data.log)
            fprintf(lf, "%c normalize\n", inOutChar());
        clearInfo();

        time(&lastChanged);            /* reset timestamp */
        updateElems();
        break;
    case 4:  /* Scale */
        popupStr2Dlg(topLevel,
                     "scale vectors",                /* Title */
                     "Specify the new range:",       /* Headline */
                     "lowest number:",               /* Label for 1st string */
                     "highest number:",              /* Label for 2nd string */
                     "", "",                         /* Default values */
                     "scale",                        /* Button label */
                     scaleVectors);                  /* Callback */
        break;
    case 5:  /* Randomize */
        vert = scalarsVertAsVec();
        if(error) {
            xhandleErr(error);
            return;
        }
        showInfo("Randomizing...");
        randomize(pats, vert);
        clearInfo();
        if(error) {
            xhandleErr(error);
            return;
        }
        if(app_data.log)
            fprintf(lf, "i randomize\n");

        updateElems();
        updateClassLabels();
        updateScalarsVert();
        if(swapSSWOpened) updateSSW(swapSSW);
        break;
    case 6: { /* Expand with class vector */
        Vector h, pad;

        h = scalarsHorizAsVec();
        if(error) {
            xhandleErr(error);
            return;
        }
        if(! (pad = newVector(maxClassNo(pats)))) {
            xhandleErr(1);
            return;
        }
        putAllDim(pad, 0);

        showInfo("Expanding with class vectors...");
        expandWithClassVectors(pats, inOutSwitch==EDIT_INPUT);
        clearInfo();
        if(error) xhandleErr(error);
        if(! (horiz = expand(h, pad))) {
            xhandleErr(1);
            return;
        }
        freeVector(h);
        freeVector(pad);
        if(app_data.log)
            fprintf(lf, "%c classExpand\n", inOutChar());

        time(&lastChanged);            /* reset timestamp */
        updateHorizScrollBar();
        if(usedCols != visDims) {
            usedCols = min(visDims, numCols(pats) - firstDim + 1);
            updateDimLabels();
            updateElems();
            updateScalarsHoriz();
        }
    }
    break;
    case 7: { /* Expand with output vector */
        Vector h, pad;

        h = scalarsHorizAsVec();
        if(error) {
            xhandleErr(error);
            return;
        }
        if(! (pad = newVector(outputDims(pats)))) {
            xhandleErr(1);
            return;
        }
        putAllDim(pad, 0);

        showInfo("Expanding with output vectors...");
        expandWithOutputs(pats);
        clearInfo();
        if(error) xhandleErr(error);
        if(! (horiz = expand(h, pad))) {
            xhandleErr(1);
            return;
        }
        freeVector(h);
        freeVector(pad);
        if(app_data.log)
            fprintf(lf, "i outputExpand\n");

        time(&lastChanged);            /* reset timestamp */
        updateHorizScrollBar();
        if(usedCols != visDims) {
            usedCols = min(visDims, numCols(pats) - firstDim + 1);
            updateDimLabels();
            updateElems();
            updateScalarsHoriz();
        }
    }
    break;
    case 8:  /* Recompute the class numbers */
        if(! hasClassNames(pats) && hasOutputs(pats)) {
            showInfo("Refreshing class numbers...");
            genClassNosFromVectors(pats);
            clearInfo();
            if(error) {
                xhandleErr(error);
                return;
            }
            if(app_data.log)
                fprintf(lf, "%c refreshClasses\n", inOutChar());
            updateClassLabels();
        }
        break;
    default:
        sprintf(errorInfo, "transformMenuSelect() called with item number %d!",
                itemNo);
        xhandleErr(20);
        return;
        break;
    }   /* switch */
}     /* transformMenuSelect */


/***************************************************************************/
/* Always called when any item of the Remove menu is selected. garbage is */
/* meaningless. client_data gives the number of the selected menu item.   */
/***************************************************************************/
static void removeMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data;
XtPointer garbage;
{
    int itemNo = (int) client_data;
    unsigned numDims = (unsigned) numCols(pats);

    switch(itemNo) {
    case 0:  /* Remove dimensions */
        popupInt2Dlg(topLevel, "remove dimensions",
                     "Remove all dimensions", "from:", "to:",
                     1, numDims, 1, numDims, 1, 1, "remove",
                     arrowUp, arrowDown, arrowUp, arrowDown,
                     removeDimensions, NULL,
                     popdown, (XtPointer) int2DlgShell);
        break;
    case 1: { /* Remove constant dimensions */
        Vector v;
        unsigned ret;
        long *lp, i;

        v = scalarsHorizAsVec();
        if(error) {
            xhandleErr(error);
            return;
        }
        if(! (lp = (long *) malloc((numDims+1) * sizeof(*lp)))) {
            xhandleErr(1);
            return;
        }
        showInfo("Removing constant dimensions...");
        ret = constCols(editedVecColl(pats), lp);
        if(ret == 0) {
            clearInfo();
            return;
        }
        if(ret == numDims) {
            free(lp);
            xhandleErr(25);
            clearInfo();
            return;
        }
        removeDimList(pats, inOutSwitch==EDIT_INPUT, lp, ret);
        clearInfo();
        if(error) {
            xhandleErr(error);
            return;
        }
        if(app_data.log) {
            fprintf(lf, "%c rmDims", inOutChar());
            for(i = 0L; i < ret; i++) fprintf(lf, " %ld", lp[i]);
            fprintf(lf, "\n");
        }
        free(lp);

        firstDim = 1;
        usedCols = min(numCols(pats), visDims);
        time(&lastChanged);            /* reset timestamp */
        updateDimLabels();
        updateElems();
        updateScalarsHoriz();
        updateHorizScrollBar();
    }
    break;
    case 2:  /* Remove vectors */
        popupInt2Dlg(topLevel, "remove vectors",
                     "Remove all vectors", "from nr:", "to nr:",
                     1, num(pats), 1, num(pats), 1, 1, "remove",
                     arrowUp, arrowDown, arrowUp, arrowDown,
                     removeVectors, NULL,
                     popdown, (XtPointer) int2DlgShell);
        break;
    default:
        sprintf(errorInfo, "removeMenuSelect() called with item number %d!",
                itemNo);
        xhandleErr(20);
        return;
        break;
    }   /* switch */
}     /* removeMenuSelect */


/***************************************************************************/
/* Always called when any item of the Show menu is selected. garbage is   */
/* meaningless. client_data gives the number of the selected menu item.   */
/***************************************************************************/
static void showMenuSelect(Widget w, XtPointer client_data, XtPointer garbage) {
    int itemNo = (int) client_data;
    GW gw;

    switch(itemNo) {
    case 0:
        if(! (gw = openGW(topLevel, "vistra graphics"))) {
            xhandleErr(1);
            return;
        }
        if(! add(openGWs, gw)) {
            xhandleErr(1);
            return;
        }
        break;
    case 1: { /* Statistics */
        char *contents;
        Position x, y;

        showInfo("Computing statistics...");
        if(! (contents = statString(pats, fileName))) {
            xhandleErr(1);
            return;
        }
        clearInfo();
        updateTW(statistics, "statistics", contents);
        free(contents);
        getPopupPosition(topLevel, &x, &y);
        XtVaSetValues(statistics, XtNx, x, XtNy, y, NULL);
        XtPopup(statistics, XtGrabNonexclusive);
    }
    break;
    case 2: { /* Covariance matrix */
        char *contents, buf[60];
        Number **cov;
        unsigned i, ndims = (unsigned) numCols(pats);
        Position x, y;

        if(! (cov = (Number **) malloc(ndims * sizeof(*cov)))) {
            xhandleErr(1);
            return;
        }
        for(i = 0; i < ndims; i++)
            if(! (cov[i] = (Number *) malloc(ndims * sizeof(Number)))) {
                xhandleErr(1);
                return;
            }
        showInfo("Computing covariance matrix...");
        covariance(editedVecColl(pats), cov);
        clearInfo();
        if(error) {
            xhandleErr(error);
            return;
        }
        if(! (contents = matAsString(cov, ndims, ndims))) {
            xhandleErr(1);
            return;
        }
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
    case 3:  /* Show the swap window (input or output vectors) */
        popupSSW(swapSSW);
        swapSSWOpened = TRUE;
        break;
    default:
        sprintf(errorInfo, "showMenuSelect() called with item number %d!",
                itemNo);
        xhandleErr(20);
        return;
        break;
    }   /* switch */
}     /* showMenuSelect */


#define LOG_LOADCOMM   (scalarSwitch == FILL_VERT ? "loadVert" : "loadHoriz")

/***************************************************************************/
/* Always called when any item of the Load menu is selected. garbage is   */
/* meaningless. client_data gives the number of the selected menu item.   */
/***************************************************************************/
static void loadMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    int itemNo = (int) client_data;
    void (*collectFunc)(VecColl, Number (*)(Vector), Vector);
    VecColl vc = editedVecColl(pats);
    Number konst;

    if(scalarSwitch == FILL_VERT) collectFunc = collectRows;
    else collectFunc = collectCols;

    switch(itemNo) {
    case 0:  /* Minimum */
        showInfo("Loading minima...");
        (*collectFunc)(vc, minimum, selVec());
        clearInfo();
        if(app_data.log)
            if(scalarSwitch == FILL_VERT)
                fprintf(lf, "%c loadVert min\n", inOutChar());
            else {
                fprintf(lf, "%c loadHoriz values ", inOutChar());
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
            else {
                fprintf(lf, "%c loadHoriz values ", inOutChar());
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
            else {
                fprintf(lf, "%c loadHoriz values ", inOutChar());
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
            else {
                fprintf(lf, "%c loadHoriz values ", inOutChar());
                fprintVector(horiz, lf);
                fprintf(lf, "\n");
            }
        break;
    case 4:  /* Standard deviation */
        showInfo("Loading standard deviations...");
        (*collectFunc)(vc, sigma, selVec());
        clearInfo();
        if(app_data.log)
            if(scalarSwitch == FILL_VERT)
                fprintf(lf, "%c loadVert stddev\n", inOutChar());
            else {
                fprintf(lf, "%c loadHoriz values ", inOutChar());
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
            else {
                fprintf(lf, "%c loadHoriz values ", inOutChar());
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
    default:
        sprintf(errorInfo, "loadMenuSelect() called with item number %d!",
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
{
    String value;
    Number n;

    value = XawDialogGetValueString((Widget) client_data);
    if(sscanf(value, "%f", &n) != 1) {
        popupErrDlg(strDlgShell, "vistra info",
                    "Specified value is not a number!");
        return;
    }

    XtPopdown(strDlgShell);
    putAllDim(selVec(), n);
    if(app_data.log)
        fprintf(lf, "%c %s const %g\n", inOutChar(), LOG_LOADCOMM, n);

    /* update the corresponding text widgets */
    if(scalarSwitch == FILL_VERT) updateScalarsVert();
    else updateScalarsHoriz();
}      /* loadConstant */


/**************************************************************/
/**************************************************************/
static void loadNr(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    String value;
    unsigned nr, maxAllowed;

    value = XawDialogGetValueString((Widget) client_data);
    if(sscanf(value, "%u", &nr) != 1) {
        popupErrDlg(strDlgShell, "vistra info",
                    "Specified value is not a positive integer!");
        return;
    }
    maxAllowed = (unsigned) (scalarSwitch == FILL_VERT ?
                             numCols(pats) : num(pats));
    if(nr < 1 || nr > maxAllowed) {
        char buf[60];
        sprintf(buf, "Nr must be between 1 and %u!", maxAllowed);
        popupErrDlg(strDlgShell, "vistra info", buf);
        return;
    }

    XtPopdown(strDlgShell);
    if(scalarSwitch == FILL_VERT)
        colVec(editedVecColl(pats), (long) nr, vert);
    else copyVec(horiz, (Vector) at(editedVecColl(pats), (long) nr));
    if(app_data.log) {
        fprintf(lf, "%c %s values ", inOutChar(), LOG_LOADCOMM);
        fprintVector(selVec(), lf);
        fprintf(lf, "\n");
    }

    /* update the corresponding text widgets */
    if(scalarSwitch == FILL_VERT) updateScalarsVert();
    else updateScalarsHoriz();

    lastNr = nr;
}      /* loadNr */


/***************************************************************************/
/* Always called when any item of the Format menu is selected. garbage is */
/* meaningless. client_data gives the number of the selected menu item.   */
/***************************************************************************/
static void formatMenuSelect(w, client_data, garbage)
Widget w;
XtPointer client_data;
XtPointer garbage;
{
    unsigned itemNo = (unsigned) client_data;
    long numItems = size(formatNames);

    if(numItems < itemNo) {
        sprintf(errorInfo, "formatMenuSelect() called with item number %d which is greater than the formatNames size!");
        xhandleErr(20);
        return;
    }

    if(itemNo != selectedFormat) {
        XtVaSetValues((Widget) at(formatMenuItems, selectedFormat),
                      XtNleftBitmap, None, NULL);
        XtVaSetValues((Widget) at(formatMenuItems, itemNo),
                      XtNleftBitmap, hook, NULL);
        selectedFormat = itemNo;
        updateInfoWidgets();
    }      /* if */
}        /* formatMenuSelect */


/***************************************************************/
/* Always called when inputToggle or outputToggle is set or    */
/* clicked.                                                     */
/***************************************************************/
static void toggle(Widget w, XtPointer client_data, XtPointer garbage) {
    int currentlySet;

    currentlySet = (int) XawToggleGetCurrent(inputToggle);
    /* currentlySet can be NULL, since unsetting a toggle also causes    */
    /* a call to this function.                                          */
    if(currentlySet) {
        if(currentlySet != inOutSwitch) {
            /* swap the contents of the shells */
            inOutSwitch = currentlySet;
            changeInOut();
            if(error) {
                xhandleErr(error);
                return;
            }
            if(swapSSWOpened) updateSSW(swapSSW);
        }   /* if(currentlySet != inOutSwitch) */
    }     /* if(currentlySet) */
}       /* toggle */


/***************************************************************/
/* Always called when vertToggle or horizToggle is set or      */
/* clicked.                                                     */
/***************************************************************/
static void toggleScalars(Widget w, XtPointer client_data, XtPointer garbage) {
    int currentlySet;

    currentlySet = (int) XawToggleGetCurrent(vertToggle);
    /* currentlySet can be NULL, since unsetting a toggle also causes    */
    /* a call to this function.                                          */
    if(currentlySet) {
        if(currentlySet != scalarSwitch) {
            scalarSwitch = currentlySet;
            if(pats) {
                XtVaSetValues(loadMenuButtonVert,
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
/* Closes the dialog box or shell that is passed via          */
/* client_data.                                               */
/**************************************************************/
static void popdown(Widget w, XtPointer client_data, XtPointer garbage) {
    XtPopdown((Widget) client_data);
}       /* popdown */


/*************************************************************/
/*************************************************************/
static void popdownSwapSSW(w, client_data, garbage)
Widget w;
XtPointer client_data, garbage;
{
    swapSSWOpened = FALSE;
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
/* Called as soon as a button is released after having been  */
/* pressed over the vertical scrollbar.                       */
/* pos contains the relative pointer position in pixels.     */
/*************************************************************/
static void scrollVertical(w, client_data, pos)
Widget w;
XtPointer client_data, pos;
{
    int ptrpos = (int) pos, previous = firstVec;

    scalarsVertAsVec();
    if(error) {
        xhandleErr(error);
        return;
    }
    if(ptrpos > 0)                /* button 1 was pressed */
        firstVec = max(1, firstVec - visVecs);
    else firstVec = min(firstVec + visVecs, num(pats));
    usedRows = min(visVecs, num(pats) - firstVec + 1);

    if(firstVec != previous) {
        updateVertScrollBar();
        updateNrLabels();
        updateElems();
        updateClassLabels();
        updateScalarsVert();
    }
}      /* scrollVertical */


/*************************************************************/
/* Called as soon as button 1 or 3 is released after having  */
/* been pressed over the horizontal scrollbar.                */
/* pos contains the relative pointer position in pixels.     */
/*************************************************************/
static void scrollHorizontal(w, client_data, pos)
Widget w;
XtPointer client_data, pos;
{
    int ptrpos = (int) pos, previous = firstDim;

    scalarsHorizAsVec();
    if(error) {
        xhandleErr(error);
        return;
    }
    if(ptrpos > 0)             /* button 1 was pressed */
        firstDim = max(1, firstDim - visDims);
    else firstDim = min(firstDim + visDims, numCols(pats));
    usedCols = min(visDims, numCols(pats) - firstDim + 1);

    if(firstDim != previous) {
        updateHorizScrollBar();
        updateDimLabels();
        updateElems();
        updateScalarsHoriz();
    }
}      /* scrollHorizontal */


/*************************************************************/
/* Called as soon as mouse button 2 is released after having */
/* been pressed over the vertical scrollbar.                  */
/* percent gives the position of the bar and lies between    */
/* 0.0 and 1.0.                                                */
/*************************************************************/
static void jumpVertical(w, client_data, percent)
Widget w;
XtPointer client_data, percent;
{
    float top;
    int previous = firstVec;
    long nrows = num(pats);

    scalarsVertAsVec();
    if(error) {
        xhandleErr(error);
        updateVertScrollBar();
        return;
    }
    top = * (float *) percent;
    firstVec = min(nrows, (int) (top * nrows) + 1);
    usedRows = min(visVecs, nrows - firstVec + 1);

    if(firstVec != previous) {
        updateNrLabels();
        updateElems();
        updateClassLabels();
        updateScalarsVert();
    }
}      /* jumpVertical */


/*************************************************************/
/* Called as soon as mouse button 2 is released after having */
/* been pressed over the horizontal scrollbar.                */
/* percent gives the position of the bar and lies between    */
/* 0.0 and 1.0.                                                */
/*************************************************************/
static void jumpHorizontal(w, client_data, percent)
Widget w;
XtPointer client_data, percent;
{
    float top;
    int previous = firstDim;
    long ncols = numCols(pats);

    scalarsHorizAsVec();
    if(error) {
        xhandleErr(error);
        updateHorizScrollBar();
        return;
    }
    top = * (float *) percent;
    firstDim = min(ncols, (int) (top * ncols) + 1);
    usedCols = min(visDims, ncols - firstDim + 1);

    if(firstDim != previous) {
        updateDimLabels();
        updateElems();
        updateScalarsHoriz();
    }
}      /* jumpHorizontal */


/*************************************************************/
/*************************************************************/
/*           E N D  of  C A L L B A C K s                    */
/*************************************************************/
/*************************************************************/

/*************************************************************/
/*************************************************************/
/*******  E R R O R - H A N D L I N G  ***********************/
/*************************************************************/
/*************************************************************/
static void xhandleErr(int err) {
    static char message[MAX_LENGTH_ERROR_MESSAGE];

    /* if there is not enough memory: terminate the program */
    if(err == 1) handleErr(err);

    genErrorMessage(err, message);
    popupErrDlg(topLevel, "vistra info", message);
}        /* xhandleErr */

