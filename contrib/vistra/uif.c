/****************************************************/
/* Variable error wird gesetzt.                     */
/****************************************************/
static void fillPatternView()
{ Dimension width, height;

  if(! hasOutputs(pats) && inOutSwitch == EDIT_OUTPUT)
  { XtRemoveAllCallbacks(inputToggle, XtNcallback);
    XtRemoveAllCallbacks(outputToggle, XtNcallback);
    XawToggleSetCurrent(inputToggle, (XtPointer) EDIT_INPUT);
    inOutSwitch = EDIT_INPUT;
    XtAddCallback(inputToggle, XtNcallback, toggle, NULL);
    XtAddCallback(outputToggle, XtNcallback, toggle, NULL);
  }

  /* Fuelle die Widgets mit neuem Inhalt */
  firstVec = firstDim = 1;
  usedRows = min(num(pats), visVecs);
  usedCols = min(numCols(pats), visDims);

  /* Initialisiere die Skalar-Vektoren */
  if(vert) freeVector(vert);
  if(horiz) freeVector(horiz);
  if(! (vert = newVector(num(pats)))) error(1);
  if(! (horiz = newVector(numCols(pats)))) error(1);
  putAllDim(vert, 0);
  putAllDim(horiz, 0);

  updateInfoWidgets();
  updateVertScrollBar();
  updateHorizScrollBar();
  updateNrLabels();
  updateDimLabels();
  updateElems();
  updateClassLabels();
  updateScalarsVert();
  updateScalarsHoriz();

  /* Mache bestimmte Funktionen aktiv/deaktiv, je nachdem, ob Ausgabe- */
  /* vektoren existieren oder nicht. */
  setActivation(pats);

  /* Oeffne neues Log-File */
  if(app_data.log)
  { char *buf;
    fclose(lf);
    if(! (buf = (char *) malloc(strlen(fileName) + 6))) error(1);
    sprintf(buf, "%s.log", fileName);
    if(! (lf = fopen(buf, "w")))
    { app_data.log = FALSE;
      XtVaSetValues(XtNameToWidget(fileMenu, "fileWriteToLogFile"),
          XtNleftBitmap, None, NULL);
      printf("Vistra warning: cannot open log file %s\n", buf);
    }
    free(buf);
  }
  logFirstTime = TRUE;
  time(&lastChanged);            /* setze Zeitstempel neu */

  error = 0;
}       /* fillPatternView */


/***********************************************************/
/* Variable error wird gesetzt.                            */
/***********************************************************/
static void changeInOut()
{
  /* Initialisiere die Skalar-Vektoren */
  if(vert) freeVector(vert);
  if(horiz) freeVector(horiz);
  if(! (vert = newVector(num(pats)))) error(1);
  if(! (horiz = newVector(numCols(pats)))) error(1);
  putAllDim(vert, 0);
  putAllDim(horiz, 0);
  if(app_data.log)
    fprintf(lf, "%c loadVert const 0\n%c loadHoriz const 0\n", 
                inOutChar(), inOutChar());

  if(firstDim > numCols(pats))
  { firstDim = 1;
    usedCols = min(visDims, numCols(pats));
  }
  else usedCols = min(visDims, numCols(pats) - firstDim + 1);

  updateHorizScrollBar();
  updateDimLabels();
  updateElems();
  updateScalarsVert();
  updateScalarsHoriz();

  error = 0;
}           /* changeInOut */


/*************************************************************/
/* Speicher in x und y die Koordinaten relativ zum Root      */
/* Window, die ein Popup Widget haben muss, um vor dem       */
/* Widget w positioniert zu werden.                          */
/*************************************************************/
static void getPopupPosition(Widget w, Position *x, Position *y)
{ Dimension width, height;

  /* Berechne Koordinaten des Mittelpunkts von w in x und y */
  XtVaGetValues(w,
      XtNwidth, &width,
      XtNheight, &height,
      NULL);

  /* Berechne Koordinaten in Bezug auf das Root Window */
  XtTranslateCoords(w,
      (Position) width/4,
      (Position) height/4,
      x, y);
}         /* getPopupPosition */


/*************************************************************/
/*************************************************************/
static void popupErrDlg(Widget w, char *title, char *message)
{ Position x, y;

  getPopupPosition(w, &x, &y);
  XtVaSetValues(errDlgShell,
      XtNx, x, XtNy, y,
      XtNtitle, title,
      NULL);
  XtVaSetValues(errDlgLabel, XtNlabel, message, NULL);

  XtPopup(errDlgShell, XtGrabNonexclusive);
}         /* popupErrDlg */


/***********************************************/
/* Oeffne die Dialogbox str2DlgShell, die vom  */
/* Benutzer zwei Strings erfragt. Der Benutzer */
/* hat die Moeglichkeit, den Vorgang ueber     */
/* 'Cancel' abzubrechen.                       */
/***********************************************/
static void popupStr2Dlg(w, title, headline, item1, item2, default1, default2,
                         okButtonLabel, callback)
Widget w;
char *title, *headline, *item1, *item2, *default1, *default2, *okButtonLabel;
void (*callback)();
{ Position x, y;

  getPopupPosition(w, &x, &y);
  XtVaSetValues(str2DlgShell,
      XtNtitle, title,
      XtNx, x, XtNy, y,
      NULL);
  XtVaSetValues(str2Headline, XtNlabel, headline, NULL);
  XtVaSetValues(str2Label1, XtNlabel, item1, NULL);
  XtVaSetValues(str2Label2, XtNlabel, item2, NULL);
  XtVaSetValues(str2Text1, XtNstring, default1, NULL);
  XtVaSetValues(str2Text2, XtNstring, default2, NULL);
  XtVaSetValues(str2Ok, XtNlabel, okButtonLabel, NULL);
  XtRemoveAllCallbacks(str2Ok, XtNcallback);
  XtAddCallback(str2Ok, XtNcallback, callback, NULL);

  XtPopup(str2DlgShell, XtGrabNonexclusive);
}        /* popupStr2Dlg */


/***********************************************/
/* Setze val1 und val2 auf die Strings, die in */
/* den beiden Text-Widgets der str2DlgShell    */
/* momentan enthalten sind.                    */
/***********************************************/
static void getStr2DlgValues(char **val1, char **val2)
{
  XtVaGetValues(str2Text1, XtNstring, val1, NULL);
  XtVaGetValues(str2Text2, XtNstring, val2, NULL);
}        /* getStr2DlgValues */


/***********************************************/
/* Aktiviere/deaktiviere Menupunkte, Toggles   */
/* etc., je nachdem, ob p Ausgabevektoren be-  */
/* sitzt oder nicht.                           */
/***********************************************/
static void setActivation(Patterns p)
{ Boolean loaded, outputsExist, classNamesExist;
  int i; 

  loaded = (p != NULL);
  outputsExist = (p ? hasOutputs(p) : FALSE);
  classNamesExist = (p ? hasClassNames(p) : FALSE);

  XtVaSetValues(outputToggle, XtNsensitive, outputsExist, NULL);
  XtVaSetValues(inputToggle, XtNsensitive, loaded, NULL);
  XtVaSetValues(transformMenuButton, XtNsensitive, loaded, NULL);
  XtVaSetValues(removeMenuButton, XtNsensitive, loaded, NULL);
  XtVaSetValues(showMenuButton, XtNsensitive, loaded, NULL);
  XtVaSetValues(loadMenuButtonVert,
                  XtNsensitive, loaded && (scalarSwitch == FILL_VERT),
                  NULL);
  XtVaSetValues(loadMenuButtonHoriz,
                  XtNsensitive, loaded && (scalarSwitch == FILL_HORIZ),
                  NULL);
  XtVaSetValues(addCommand, XtNsensitive, loaded, NULL);
  XtVaSetValues(subCommand, XtNsensitive, loaded, NULL);
  XtVaSetValues(multCommand, XtNsensitive, loaded, NULL);
  XtVaSetValues(divCommand, XtNsensitive, loaded, NULL);
  XtVaSetValues(replaceCommand, XtNsensitive, loaded, NULL);
  XtVaSetValues(XtNameToWidget(fileMenu, "fileWritePatterns"),
                  XtNsensitive, loaded, NULL);
  XtVaSetValues(XtNameToWidget(fileMenu, "fileLoadSymbols"),
                  XtNsensitive, loaded, NULL);
  XtVaSetValues(XtNameToWidget(fileMenu, "fileWriteSymbols"),
                  XtNsensitive, loaded, NULL);
  XtVaSetValues(XtNameToWidget(fileMenu, "fileWriteToLogFile"),
                  XtNsensitive, loaded, NULL);
  XtVaSetValues(XtNameToWidget(transformMenu, "transformExpandWithOutput"),
                  XtNsensitive, outputsExist, NULL);
  XtVaSetValues(XtNameToWidget(transformMenu, "transformRefreshClassNumbers"),
                  XtNsensitive, outputsExist && ! hasClassNames(pats), NULL);
  XtVaSetValues(XtNameToWidget(showMenu, "showReadShell"),
                  XtNsensitive, outputsExist, NULL);
  for(i = 0; i < visVecs; i++)
    XtVaSetValues(scalarsVert[i], XtNsensitive, loaded, NULL);
  for(i = 0; i < visDims; i++)
    XtVaSetValues(scalarsHoriz[i], XtNsensitive, loaded, NULL);
}     /* setActivation */


/*********************************************************/
/* Antworte das Format namens name. Antworte NULL, falls */
/* es sich um das N01 oder LVQ-Format handelt.           */
/* Variable error wird gesetzt.                          */
/*********************************************************/
static Format nameToFormat(char *name)
{ long pos;

  pos = detectPos(formatNames, name, streq);
  if(pos < 2L)
  {                                /* kein Format mit diesem Suffix */
    sprintf(errorInfo, "Unknown format %s!", name);
    error(22);
  }

  error = 0;
  if(pos == 2L || pos == 3L) return NULL; 
  else  return (Format) at(formats, pos - 3);
}           /* nameToFormat */


/**********************************************************/
/* Setze Variable error.                                  */
/**********************************************************/
static Vector scalarVec()
{
  return (scalarSwitch==FILL_VERT ? scalarsVertAsVec() : scalarsHorizAsVec());
}           /* scalarVec */


/******************************************************/
/* Variable error wird gesetzt.                       */
/******************************************************/
static Vector scalarsVertAsVec()
{ String s;
  int i;
  char buf[NUMBER_STR_LENGTH + 1];
  Number n;

  for(i = 0; i < usedRows; i++)
  { sprintf(buf, NUMBER_FORMAT, atDim(vert, firstVec + i));
    XtVaGetValues(scalarsVert[i], XtNstring, &s, NULL);
    if(strcmp(s, buf))
    { if(sscanf(s, "%f", &n) != 1) 
      { sprintf(errorInfo, "vertical scalar nr:  %u", firstVec + i);
        error(23);
      }
      else
      { putDim(vert, (long) (firstVec + i), n);
        sprintf(buf, NUMBER_FORMAT, n);
        XtVaSetValues(scalarsVert[i], XtNstring, buf, NULL); 
        if(app_data.log)
          fprintf(lf, "%c loadVert dim %u %g\n", inOutChar(), firstVec + i, n);
      } 
    }
  }

  error = 0;
  return vert;
}           /* scalarsVertAsVec */


/******************************************************/
/* Antworte einen Vektor, der als Elemente die Numbers*/
/* enthaelt, deren String-Repraesentationen in den    */
/* Text Widgets scalarsHoriz[] stehen.                */
/* Variable error wird gesetzt.                       */
/******************************************************/
static Vector scalarsHorizAsVec()
{ int i;
  char *s, buf[NUMBER_STR_LENGTH + 1];
  Number n;

  for(i = 0; i < usedCols; i++)
  {
    XtVaGetValues(scalarsHoriz[i], XtNstring, &s, NULL);
    sprintf(buf, NUMBER_FORMAT, atDim(horiz, i + firstDim));
    if(strcmp(s, buf))
    { if(sscanf(s, "%f", &n) != 1) 
      { sprintf(errorInfo, "horizontal scalar nr:  %u", firstDim + i);
        error(23);
      }
      else
      { putDim(horiz, (long)(i + firstDim), n);
        sprintf(buf, NUMBER_FORMAT, n);
        XtVaSetValues(scalarsHoriz[i], XtNstring, buf, NULL); 
        if(app_data.log)
          fprintf(lf, "%c loadHoriz dim %u %g\n", inOutChar(), firstDim+i, n);
      }
    }
  }      /* for */

  error = 0;
  return horiz;
}           /* scalarsHorizAsVec */


/*********************************************************/
/* Mache das Shell-Widget w groessenunveraenderlich.     */
/*********************************************************/
static void fixSize(Widget sw)
{ Dimension w, h;

  XtVaGetValues(sw, XtNwidth, &w, XtNheight, &h, NULL);
  XtVaSetValues(sw,
      XtNminWidth, w, XtNmaxWidth, w,
      XtNminHeight, h, XtNmaxHeight, h,
      NULL);
}          /* fixSize */


/****************************************************/
/****************************************************/
static void updateInfoWidgets()
{ char buf[NUM_OF_PATS_STR_MAXLEN];

  if(fileName)
  { char *name;
    if(! (name = strrchr(fileName, '/'))) name = fileName;
    else ++name;
    XtVaSetValues(fileLabel, XtNlabel, name, NULL);
  }
  else XtVaSetValues(fileLabel, XtNlabel, "---", NULL);

  if(pats)
  { sprintf(buf, "%ld", num(pats));
    XtVaSetValues(numOfPatternsLabel, XtNlabel, buf, NULL);
  }
  else XtVaSetValues(numOfPatternsLabel, XtNlabel, "---", NULL);

  XtVaSetValues(formatLabel,
      XtNlabel, (char *) at(formatNames, selectedFormat),
      NULL);
}       /* updateInfoWidgets */


/********************************************************/
/********************************************************/
static void updateScalarsHoriz()
{ char buf[NUMBER_STR_LENGTH + 1];
  int i;

  for(i = 0; i < usedCols; i++)
  { sprintf(buf, NUMBER_FORMAT, atDim(horiz, firstDim + i));
    XtVaSetValues(scalarsHoriz[i],
       XtNstring, buf,
       XtNinsertPosition, NUMBER_STR_LENGTH,
       NULL);
  }
  for(i = usedCols; i < visDims; i++)
    XtVaSetValues(scalarsHoriz[i], XtNstring, "", NULL);
}       /* updateScalarsHoriz */


/*********************************************************/
/*********************************************************/
static void updateScalarsVert()
{ char buf[NUMBER_STR_LENGTH + 1];
  int i;

  for(i = 0; i < usedRows; i++)
  { sprintf(buf, NUMBER_FORMAT, atDim(vert, firstVec + i));
    XtVaSetValues(scalarsVert[i],
        XtNstring, buf,
        XtNinsertPosition, NUMBER_STR_LENGTH,
        NULL);
  }
  for(i = usedRows; i < visVecs; i++)
    XtVaSetValues(scalarsVert[i], XtNstring, "", NULL);
}       /* updateScalarsVert */


/********************************************************/
/********************************************************/
static void updateDimLabels()
{ char buf[NUM_OF_PATS_STR_MAXLEN + 1];
  int i;

  for(i = 0; i < usedCols; i++)
  { sprintf(buf, "%d", firstDim + i);
    XtVaSetValues(dimLabels[i], XtNlabel, buf, NULL);
  }
  for(i = usedCols; i < visDims; i++)
    XtVaSetValues(dimLabels[i], XtNlabel, "", NULL);
}       /* updateDimLabels */


/********************************************************/
/********************************************************/
static void updateNrLabels()
{ char buf[NUM_OF_PATS_STR_MAXLEN + 1];
  int i;

  for(i = 0; i < usedRows; i++)
  { sprintf(buf, "%d", firstVec + i);
    XtVaSetValues(nrLabels[i], XtNlabel, buf, NULL);
  }
  for(i = usedRows; i < visVecs; i++)
    XtVaSetValues(nrLabels[i], XtNlabel, "", NULL);
}        /* updateNrLabels */


/********************************************************/
/********************************************************/
static void updateElems()
{ VecColl vc;
  Vector v;
  int i, j;
  char buf[NUMBER_STR_LENGTH + 1];

  vc = editedVecColl(pats);
  for(i = 0; i < usedRows; i++)
  { v = (Vector) at(vc, firstVec + i);
    for(j = 0; j < usedCols; j++)
    { sprintf(buf, NUMBER_FORMAT, atDim(v, firstDim + j));
      XtVaSetValues(elems[i][j], XtNlabel, buf, NULL);
    }
    for(j = usedCols; j < visDims; j++)
      XtVaSetValues(elems[i][j], XtNlabel, "", NULL);
  }
  for(i = usedRows; i < visVecs; i++)
    for(j = 0; j < visDims; j++)
      XtVaSetValues(elems[i][j], XtNlabel, "", NULL);
}        /* updateElems */


/********************************************************/
/********************************************************/
static void updateClassLabels()
{ int i;

  for(i = 0; i < usedRows; i++)
    XtVaSetValues(classLabels[i],
         XtNlabel, classString(pats, firstVec + i),
         NULL);
  for(i = usedRows; i < visVecs; i++)
    XtVaSetValues(classLabels[i], XtNlabel, "", NULL);
}        /* updateClassLabels */


/********************************************************/
/********************************************************/
static void updateVertScrollBar()
{ float shown, top;

  shown = (float) usedRows / (float) num(pats);
  top = (float) (firstVec - 1) / (float) num(pats);
  XawScrollbarSetThumb(vertScrollBar, top, shown);
}        /* updateVertScrollBar */


/********************************************************/
/********************************************************/
static void updateHorizScrollBar()
{ float shown, top;

  shown = (float) usedCols / (float) numCols(pats);
  top = (float) (firstDim - 1) / (float) numCols(pats);
  XawScrollbarSetThumb(horizScrollBar, top, shown);
}        /* updateHorizScrollBar */


/*******************************************************/
/*******************************************************/
static void updateTW(Widget tw, char *title, char *text)
{ Widget twText;

  twText = XtNameToWidget(XtNameToWidget(tw, "twForm"), "twText");
  XtVaSetValues(tw, XtNtitle, title, NULL);
  XtVaSetValues(twText, XtNstring, text, NULL);
}       /* updateTW */


/*******************************************************/
/*******************************************************/
static void showInfo(char *str)
{
  XtVaSetValues(infoLabel, XtNlabel, str, NULL);
  exposeWidget(infoLabel);
}       /* showInfo */


/*******************************************************/
/*******************************************************/
static void exposeWidget(Widget W)
{
  XEvent event;
  XExposeEvent *e = &(event.xexpose);
  Dimension width, height;

  XtVaGetValues( W,
       XtNwidth, &width,
       XtNheight, &height,
       NULL );
	
  event.type = e->type = Expose;
  e->serial = e->x = e->y = e->count = 0;
  e->send_event = FALSE;
  e->display = display;
  e->window = XtWindow( W );
  e->width = (int) width;
  e->height = (int) height;

  XtDispatchEvent(&event);
  XFlush(display);
}              /* exposeWidget*/


#ifdef XFWF_FILE_SELECTION
/***********************************************/
/***********************************************/
static void popupFileSelector(w, title, ok_cb)
Widget w;
char *title;
void (*ok_cb)();
{ Position x, y;
  
  getPopupPosition(w, &x, &y);
  XtVaSetValues(fileSelShell, 
       XtNx, x, XtNy, y, 
       XtNtitle, title,
       NULL);
  XtRemoveAllCallbacks(fileSelector, XtNokButtonCallback);
  XtAddCallback(fileSelector, XtNokButtonCallback,
      (XtCallbackProc) ok_cb, NULL);
 
  XtPopup(fileSelShell, XtGrabNonexclusive);
}         /* popupFileSelector */
#endif 
