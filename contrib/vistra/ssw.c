#define swapVecs(p) (inOutSwitch == EDIT_INPUT ? outputs(p) : inputs(p))
#define swapCols(p) (inOutSwitch == EDIT_INPUT ? outputDims(p) : inputDims(p))

/* Functions locat to SSW.C */
static void updateSSW(SSW);
static void updateSSWRowTitles(SSW);
static void updateSSWColTitles(SSW);
static void updateSSWElems(SSW);
static void updateSSWscrollVert(SSW);
static void updateSSWscrollHoriz(SSW);
/* Callbacks */
static void sswScrollVert(Widget, XtPointer, XtPointer);
static void sswScrollHoriz(Widget, XtPointer, XtPointer);
static void sswJumpVert(Widget, XtPointer, XtPointer);
static void sswJumpHoriz(Widget, XtPointer, XtPointer);


/*************************************************************/
/*************************************************************/
static SSW createSSW(parent, width, height, popdown_cb, data)
Widget parent;
unsigned width, height;
void (*popdown_cb)();
XtPointer data;
{ SSW ssw;
  Widget sswForm, sswColForm, sswClose;
  unsigned i, lineHeight = textHeight - 2 * TEXT_MARGINS;
  unsigned colFormWidth;       

  if(! (ssw = (SSW) malloc(sizeof(*ssw)))) return NULL;
  ssw->visRows = (height / lineHeight) + 1;  
  ssw->visCols = (width / (textWidth + SSW_COL_SKIP)) + 1; 
  if(! (ssw->colTitles = (Widget *) malloc(sizeof(Widget) * ssw->visCols)))
    return NULL;
  if(! (ssw->cols = (Widget *) malloc(sizeof(Widget) * ssw->visCols)))
    return NULL;

  ssw->shell = XtVaCreatePopupShell(
      "sswShell", transientShellWidgetClass, parent,
      NULL);
  sswForm = XtVaCreateManagedWidget(
      "sswForm", formWidgetClass, ssw->shell,
      NULL);
  ssw->rowTitles = XtVaCreateManagedWidget(
      "sswRowTitles", asciiTextWidgetClass, sswForm,
      XtNborderWidth, 0,
      XtNwidth, textWidth,
      XtNheight, height,
      XtNfont, app_data.numberFont,
      XtNleftMargin, TEXT_MARGINS,
      XtNrightMargin, TEXT_MARGINS,
      XtNtopMargin, TEXT_MARGINS,
      XtNbottomMargin, TEXT_MARGINS,
      XtNdisplayCaret, FALSE,
      XtNeditType, XawtextRead,
      XtNtype, XawAsciiString, 
      XtNscrollVertical, XawtextScrollNever,
      XtNscrollHorizontal, XawtextScrollNever,
      XtNstring, "",
      XtNresize, XawtextResizeNever,
      XtNtop, XtChainTop,
      XtNbottom, XtChainTop,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      XtNfromVert, NULL,
      XtNvertDistance, 8 + textHeight + BORDER_WIDTH,
      XtNfromHoriz, NULL,
      XtNhorizDistance, 8,
      NULL);
   for(i = 0; i < ssw->visCols; i++)
     ssw->colTitles[i] = XtVaCreateManagedWidget(
         "sswColTitle", labelWidgetClass, sswForm,
         XtNborderWidth, 0,
         XtNwidth, textWidth,
         XtNheight, textHeight,
         XtNfont, app_data.numberFont,
         XtNinternalWidth, TEXT_MARGINS,
         XtNlabel, "",
         XtNjustify, XtJustifyRight,
         XtNresize, FALSE,
         XtNfromVert, NULL,
         XtNvertDistance, 8,
         XtNfromHoriz, (i ? ssw->colTitles[i-1] : ssw->rowTitles),
         XtNhorizDistance, (i ? SSW_COL_SKIP : BORDER_WIDTH),
         XtNtop, XtChainTop,
         XtNbottom, XtChainTop,
         XtNleft, XtChainLeft,
         XtNright, XtChainLeft,
         NULL);
   colFormWidth = ssw->visCols * (textWidth + SSW_COL_SKIP) - SSW_COL_SKIP;
   sswColForm = XtVaCreateManagedWidget(
       "sswColForm", formWidgetClass, sswForm,
       XtNborderWidth, BORDER_WIDTH,
       XtNwidth, colFormWidth,
       XtNheight, height,
       XtNfromVert, ssw->colTitles[0],
       XtNvertDistance, 0,
       XtNfromHoriz, ssw->rowTitles,
       XtNhorizDistance, 0,
       XtNtop, XtChainTop,
       XtNbottom, XtChainTop,
       XtNleft, XtChainLeft,
       XtNright, XtChainRight,
       NULL);
  for(i = 0; i < ssw->visCols; i++)
    ssw->cols[i] = XtVaCreateManagedWidget(
         "sswCol", asciiTextWidgetClass, sswColForm,
         XtNborderWidth, 0,
         XtNwidth, textWidth,
         XtNheight, height,
         XtNfont, app_data.numberFont,
         XtNleftMargin, TEXT_MARGINS,
         XtNrightMargin, TEXT_MARGINS,
         XtNtopMargin, TEXT_MARGINS,
         XtNbottomMargin, TEXT_MARGINS,
         XtNdisplayCaret, FALSE,
         XtNeditType, XawtextRead,
         XtNtype, XawAsciiString, 
         XtNscrollVertical, XawtextScrollNever,
         XtNscrollHorizontal, XawtextScrollNever,
         XtNstring, "",
         XtNresize, XawtextResizeNever,
         XtNfromVert, NULL,
         XtNvertDistance, 0,
         XtNfromHoriz, (i ? ssw->cols[i-1] : NULL),
         XtNhorizDistance, (i ? SSW_COL_SKIP : 0),
         XtNtop, XtChainTop,
         XtNbottom, XtChainTop,
         XtNleft, XtChainLeft,
         XtNright, XtChainLeft,
         NULL);
  ssw->scrollVert = XtVaCreateManagedWidget(
      "sswScrollVert", scrollbarWidgetClass, sswForm,                        
      XtNorientation, XtorientVertical,
      XtNfromVert, ssw->colTitles[0],
      XtNvertDistance, BORDER_WIDTH,
      XtNfromHoriz, sswColForm,
      XtNhorizDistance, 0,
      XtNheight, height + 2 * BORDER_WIDTH, 
      NULL);
  XtAddCallback(ssw->scrollVert, XtNscrollProc, sswScrollVert, 
                (XtPointer) ssw);
  XtAddCallback(ssw->scrollVert, XtNjumpProc, sswJumpVert, (XtPointer) ssw);

  ssw->scrollHoriz = XtVaCreateManagedWidget(
      "sswScrollHoriz", scrollbarWidgetClass, sswForm,                        
      XtNorientation, XtorientHorizontal,
      XtNfromVert, sswColForm,
      XtNvertDistance, 0,
      XtNfromHoriz, ssw->rowTitles,
      XtNhorizDistance, 0,
      XtNwidth, colFormWidth + 2 * BORDER_WIDTH,
      NULL);
  XtAddCallback(ssw->scrollHoriz, XtNscrollProc, sswScrollHoriz, 
                (XtPointer) ssw);
  XtAddCallback(ssw->scrollHoriz, XtNjumpProc, sswJumpHoriz, (XtPointer) ssw);

  sswClose = XtVaCreateManagedWidget(
      "sswClose", commandWidgetClass, sswForm,
      XtNlabel, "close",
      XtNwidth, colFormWidth + 2 * BORDER_WIDTH, 
      XtNfromVert, ssw->scrollHoriz,
      XtNvertDistance, 16,
      XtNfromHoriz, ssw->rowTitles,
      XtNhorizDistance, 0,
      XtNtop, XtChainBottom,
      XtNbottom, XtChainBottom,
      XtNleft, XtChainLeft,
      XtNright, XtChainLeft,
      NULL);
  XtAddCallback(sswClose, XtNcallback, popdown_cb, data);
  
  return ssw;  
}      /* createSSW */   


/***********************************************/
/***********************************************/
static void popupSSW(SSW ssw)
{ Position x, y;

  getPopupPosition(topLevel, &x, &y);
  XtVaSetValues(ssw->shell,
      XtNx, x, XtNy, y, NULL);
  XtPopup(ssw->shell, XtGrabNone);
  fixSize(ssw->shell);
  updateSSW(ssw);
}       /* popupSSW */


/*************************************************************/
/*************************************************************/
static void updateSSW(SSW ssw)
{ 
  ssw->firstRow = ssw->firstCol = 1;
  ssw->usedRows = (unsigned) min(ssw->visRows, num(pats));
  ssw->usedCols = (unsigned) min(ssw->visCols, swapCols(pats));  
  XtVaSetValues(ssw->shell, XtNtitle, SWAP_TITLE, NULL);
  updateSSWRowTitles(ssw);
  updateSSWColTitles(ssw);
  updateSSWElems(ssw);
  updateSSWscrollVert(ssw);
  updateSSWscrollHoriz(ssw); 
}      /* updateSSW */


/**********************************************************/
/**********************************************************/
static void updateSSWRowTitles(SSW ssw)
{ char *buf, *cp;
  unsigned i;

  if(! (buf = (char *) malloc(ssw->visRows * (NUM_OF_PATS_STR_MAXLEN+1)+1)))
    handleErr(1);
  
  cp = buf;
  for(i = ssw->firstRow; i < ssw->firstRow + ssw->usedRows; i++)
  { sprintf(cp, NUM_OF_PATS_FORMAT_NL, i);
    cp += NUM_OF_PATS_STR_MAXLEN + SIZEOF_NL;  
  } 
  for(i = ssw->firstRow+ssw->usedRows; i < ssw->firstRow+ssw->visRows; i++)
  { sprintf(cp, "      \n", i);
    cp += NUM_OF_PATS_STR_MAXLEN + SIZEOF_NL;
  }
  *cp = '\0';   

  XtVaSetValues(ssw->rowTitles, 
      XtNstring, buf, XtNdisplayPosition, 0, NULL);
  free(buf);
}      /* updateSSWRowTitles */
 

/**********************************************************/
/**********************************************************/
static void updateSSWColTitles(SSW ssw)
{ char buf[NUM_OF_PATS_STR_MAXLEN + 5];
  unsigned i;

  for(i = 0; i < ssw->usedCols; i++)
  { sprintf(buf, "%u", ssw->firstCol + i);
    XtVaSetValues(ssw->colTitles[i], XtNlabel, buf, NULL);  
  }
  for(i = ssw->usedCols; i < ssw->visCols; i++)
    XtVaSetValues(ssw->colTitles[i], XtNlabel, "", NULL);  
}      /* updateSSWColTitles */


/**********************************************************/
/**********************************************************/
static void updateSSWElems(SSW ssw)
{ char *buf;
  unsigned i;

  if(! (buf = (char *) malloc(ssw->usedRows * (NUMBER_STR_LENGTH + 1) + 1)))
    handleErr(1);

  for(i = 0; i < ssw->usedCols; i++)
  { colAsStringVert(swapVecs(pats), ssw->firstCol + i, ssw->firstRow,
                    ssw->firstRow + ssw->usedRows - 1, buf); 
    XtVaSetValues(ssw->cols[i], 
        XtNstring, buf, XtNdisplayPosition, 0, NULL);  
  }
  for(i = ssw->usedCols; i < ssw->visCols; i++)
    XtVaSetValues(ssw->cols[i], XtNstring, "", NULL);

  free(buf);
}      /* updateSSWElems */
 
 
/**********************************************************/
/**********************************************************/
static void updateSSWscrollVert(SSW ssw)
{ float shown, top;

  shown = (float) ssw->usedRows / (float) num(pats);
  top = (float) (ssw->firstRow - 1) / (float) num(pats);
  XawScrollbarSetThumb(ssw->scrollVert, top, shown);
}      /* updateSSWscrollVert */


/**********************************************************/
/**********************************************************/
static void updateSSWscrollHoriz(SSW ssw)
{ float top, shown;

  shown = (float) ssw->usedCols / (float) swapCols(pats);
  top = (float) (ssw->firstCol - 1) / (float) swapCols(pats);
  XawScrollbarSetThumb(ssw->scrollHoriz, top, shown);
}      /* updateSSWscrollHoriz */


/*************************************************************/
/* Wird aufgerufen, sobald ein Button losgelassen wird,      */
/* nachdem dieser zuvor ueber der vertikalen Scrollbar       */
/* gedrueckt wurde.                                          */
/* pos enthaelt die relative Pointer-Position in Pixels.     */
/*************************************************************/
static void sswScrollVert(w, client_data, pos)
Widget w;
XtPointer client_data, pos;
{ SSW ssw = (SSW) client_data;
  int ptrpos = (int) pos, previous = ssw->firstRow;

  if(ptrpos > 0)                /* Button 1 wurde gedrueckt */
    ssw->firstRow = max(1, (int) (ssw->firstRow - ssw->visRows));
  else ssw->firstRow = min(ssw->firstRow + ssw->visRows, num(pats));
  ssw->usedRows = min(ssw->visRows, num(pats) - ssw->firstRow + 1);

  if(ssw->firstRow != previous)
  { updateSSWscrollVert(ssw);
    updateSSWRowTitles(ssw);
    updateSSWElems(ssw);
  }
}      /* sswScrollVert */


/*************************************************************/
/* Wird aufgerufen, sobald Button 1 oder 3 losgelassen wird, */
/* nachdem dieser zuvor ueber der horizontalen Scrollbar     */
/* gedrueckt wurde.                                          */
/* pos enthaelt die relative Pointer-Position in Pixels.     */
/*************************************************************/
static void sswScrollHoriz(w, client_data, pos)
Widget w;
XtPointer client_data, pos;
{ SSW ssw = (SSW) client_data;
  int ptrpos = (int) pos, previous = ssw->firstCol;
 
  if(ptrpos > 0)             /* Button 1 wurde gedrueckt */
    ssw->firstCol = max(1, (int) (ssw->firstCol - ssw->visCols));
  else ssw->firstCol = min(ssw->firstCol + ssw->visCols, swapCols(pats));
  ssw->usedCols = min(ssw->visCols, swapCols(pats) - ssw->firstCol + 1);
  
  if(ssw->firstCol != previous)
  { updateSSWscrollHoriz(ssw);
    updateSSWColTitles(ssw);
    updateSSWElems(ssw);
  } 
}      /* sswScrollHoriz */


/*************************************************************/
/* Wird aufgerufen, sobald der Mouse Button 2 losgelassen    */
/* wir, nachdem dieser zuvor ueber der vertikalen Scrollbar  */
/* gedrueckt wurde.                                          */
/* percent gibt die Position des Balkens an und liegt        */
/* zwischen 0.0 und 1.0.                                     */
/*************************************************************/
static void sswJumpVert(w, client_data, percent)
Widget w;
XtPointer client_data, percent;
{ SSW ssw = (SSW) client_data;
  float top;
  int previous = ssw->firstRow;
  long nrows = num(pats);
  
  top = * (float *) percent;
  ssw->firstRow = min(nrows, (int) (top * nrows) + 1);
  ssw->usedRows = min(ssw->visRows, nrows - ssw->firstRow + 1);

  if(ssw->firstRow != previous)
  { updateSSWRowTitles(ssw);
    updateSSWElems(ssw);
  }
}      /* sswJumpVert */


/*************************************************************/
/* Wird aufgerufen, sobald der Mouse Button 2 losgelassen    */
/* wir, nachdem dieser zuvor ueber der horizontalen Scrollbar*/
/* gedrueckt wurde.                                          */
/* percent gibt die Position des Balkens an und liegt        */
/* zwischen 0.0 und 1.0.                                     */
/*************************************************************/
static void sswJumpHoriz(w, client_data, percent)
Widget w;
XtPointer client_data, percent;
{ SSW ssw = (SSW) client_data;
  float top;
  int previous = ssw->firstCol;
  long ncols = swapCols(pats);
  
  top = * (float *) percent;
  ssw->firstCol = min(ncols, (int) (top * ncols) + 1);
  ssw->usedCols = min(ssw->visCols, ncols - ssw->firstCol + 1);

  if(ssw->firstCol != previous)
  { updateSSWColTitles(ssw);
    updateSSWElems(ssw);
  } 
}      /* sswJumpHoriz */



