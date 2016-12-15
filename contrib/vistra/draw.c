#define translateX(cs, n)   (unsigned) ((cs).x + (cs).originX + (n) * (cs).x_mult)
#define translateY(cs, n)   (unsigned) ((cs).y + (cs).originY - (n) * (cs).y_mult)

/* Private function headers */
static PixelNo colorPixel(Number, Range);
static PixelNo grayPixel(Number, Range);
static void loadCS(CoordSystem *, unsigned, unsigned, Range, Range);
static void drawCoordinateSystem(Drawable, CoordSystem *);
static void drawHisto(Drawable, CoordSystem, Vector);
static void drawConnectedLines(Drawable, CoordSystem, Vector);
static Number getBelow(Number, Number);
static Number getAbove(Number, Number);
static void drawMarkX(Drawable, CoordSystem *, Number);
static void drawMarkY(Drawable, CoordSystem *, Number);


/*********************************************************/
/* Initialisiere die lokalen Variablen von DRAW.C.       */
/* Die Funktionen von DRAW.C sollen den Screen und das   */
/* Display von Widget w verwenden.                       */
/*********************************************************/
void initDrawer(Widget w)
{ unsigned i;

  for(i = 0; i < NUM_COLORS; i++) colorpixel[i] = 0;
  for(i = 0; i < NUM_GRAYS; i++) graypixel[i] = 0;

  cmap = DefaultColormap(display, screen);
  black = BlackPixel(display, screen);
  white = WhitePixel(display, screen);
  root = RootWindow(display, screen);
  gc = XCreateGC(display, XtWindow(w), 0, 0);
  textGC = XCreateGC(display, XtWindow(w), 0, 0);
  drawFont = XLoadQueryFont(display, "6x12");
  XSetFont(display, gc, drawFont->fid);
  textFont = XLoadFont(display, "6x12");
  XSetFont(display, textGC, textFont);
  XSetForeground(display, textGC, black);
}          /* initDrawer */


/********************************************************/
/* Antworte die Pixel-Nummer der Farbintensitaet, die   */
/* dem Wert n entspricht bezogen auf den Wertebereich r.*/
/********************************************************/
static PixelNo colorPixel(Number n, Range r)
{ XColor xcol;
  int colorindex;
  Boolean ret;
  double v, red, green, blue;

  /* if(monochrome) return BlackPixel(display ,screen); */
  /* Berechne die Farbintensitaet in einer Skala von 0.0 - 1.0 */
  if(r.lowest >= r.highest) v = 0.0;
  else v = (n - r.lowest) / (r.highest - r.lowest);

  if(v > 1.0) v = 1.0;
  if(v < 0.0) v = 0.0;
  colorindex = NUM_COLORS * v;
  colorindex = min(NUM_COLORS - 1, colorindex);

  if((unsigned long) 0 != colorpixel[colorindex])
    return colorpixel[colorindex];

  v = 1.0 - ceil(v * (double) NUM_COLORS) / (double) NUM_COLORS;
  if(v < 0.2)
  { red = (1.0-v) / 0.2;
    green = 0.0;
    blue = 1.0;
  }
  else if(v < 0.4)
  { red = 0.0;
    green = (v-0.2) / 0.2;
    blue = 1.0;
  }
  else if(v < 0.6)
  { red = 0.0;
    green = 1.0;
    blue = 1.0 - (v-0.4) / 0.2;
  }
  else if(v < 0.8)
  { red = (v-0.6) / 0.2;
    green = 1.0;
    blue = 0.0;
  }
  else
  { red = 1.0;
    green = 1.0 - (v-0.8) / 0.2;
    blue = 0.0;
  }
  xcol.red = (unsigned short) (red * 65535.0);
  xcol.green = (unsigned short) (green * 65535.0);
  xcol.blue = (unsigned short) (blue * 65535.0);
  ret = XAllocColor(display, cmap, &xcol);
  if(ret)
  { colorpixel[colorindex] = xcol.pixel;
    return xcol.pixel;
  }
  else return BlackPixel(display, screen);
}            /* colorPixel */


/********************************************************/
/* Antworte die Pixel-Nummer der Graustufe, die         */
/* dem Wert n entspricht bezogen auf den Wertebereich r.*/
/********************************************************/
static PixelNo grayPixel(Number n, Range r)
{ XColor xcol;
  int grayindex;
  Boolean ret;
  double v, red, green, blue;

  /* if(monochrome) return BlackPixel(display ,screen); */
  /* Berechne die Farbintensitaet in einer Skala von 0.0 - 1.0 */
  if(r.lowest >= r.highest) v = 0.0;
  else v = (n - r.lowest) / (r.highest - r.lowest);

  if(v > 1.0) v = 1.0;
  if(v < 0.0) v = 0.0;
  grayindex = NUM_GRAYS * v;
  grayindex = min(NUM_GRAYS - 1, grayindex);

  if((unsigned long) 0 != graypixel[grayindex])
    return graypixel[grayindex];

  xcol.red = xcol.green = xcol.blue = (unsigned short) (v * 65535.0);
  ret = XAllocColor(display, cmap, &xcol);
  if(ret)
  { graypixel[grayindex] = xcol.pixel;
    return xcol.pixel;
  }
  else return BlackPixel(display, screen);
}            /* grayPixel */


/*********************************************************/
/*********************************************************/
static void drawColorArray(Drawable d, Boolean gray, Vector v,
                    unsigned x, unsigned y, unsigned width, unsigned height,
                    Range r, long rows, long cols)
{ long i, v_dims, cellX, cellY;
  PixelNo pixel;

  /* Zeichne die Farbzellen */
  v_dims = dims(v);
  for(i = 1L; i <= rows*cols; i++)
  {
    cellY = (i - 1) / cols;
    cellX = (i - 1) % cols;
    if(i > v_dims) pixel = white;
    else
       pixel = (gray ? grayPixel(atDim(v, i), r) : colorPixel(atDim(v, i), r));
    XSetForeground(display,                  /* Display */
	gc, pixel);                          /* PixelNo */
    XFillRectangle(display,                  /* Display */
	d, gc,                               /* Drawable, GC */
        x + LINE_WIDTH + (cellX * width),    /* x */
        y + LINE_WIDTH + (cellY * height),   /* y */
        width, height);                      /* width, height */
  }       /* for */

  /* Zeichne ein Rechteck um das Farbmuster */
  XSetForeground(display, gc, black);
  XDrawRectangle(display,                      /* Display */
	d, gc, x+1, y+1,                       /* Drawable, GC, x , y */
        (cols * width),                        /* width */
        (rows * height));                      /* height */
}        /* drawColorArray */


/********************************************************/
/********************************************************/
static unsigned drawMatrices(gw, gray, vc, range, rows, cols)
GW gw;
Boolean gray;
VecColl vc;
Range range;
long rows, cols;
{ char buf[NUM_OF_PATS_STR_MAXLEN + 5];
  unsigned i, answer, numVectors, posX, posY;
  
  numVectors = (unsigned) numberOfRows(vc);
  posX = PM_LEFTMARGIN;
  posY = PM_TOPMARGIN;
  for(i = gw->displayed; i <= numVectors; i++)
  { if(posX > gw->pmWidth) 
    { posX = PM_LEFTMARGIN;
      posY += (gw->heightPerVector + gwDists[gw->kind]);
    }
    if(posY > gw->pmHeight) break;
    sprintf(buf, "%u", i);
    XDrawString(display, gw->pm, textGC, 
                posX + DRAW_NR_LEFTMARGIN,
                posY - DRAW_NR_BOTTOMMARGIN,
                buf, strlen(buf));
    drawColorArray(gw->pm, gray, (Vector) at(vc, i), posX, posY,
                gw->curWidth, gw->curWidth, range, rows, cols); 
    posX += (gwDists[gw->kind] + gw->widthPerVector);
  }    /* for */ 
  answer = i - 1;

  return answer;  
}         /* drawMatrices */


/*********************************************************/
/*********************************************************/
static void loadCS(cs, width, height, rangeX, rangeY)
CoordSystem *cs;
unsigned width, height;
Range rangeX, rangeY;
{ unsigned innerWidth, innerHeight;
  ScaleOp scaleX, scaleY;
  Range pixelX, pixelY;

  /* Achsen sollen in jedem Fall sichtbar sein */
  rangeX.lowest = min(rangeX.lowest, 0);
  rangeX.highest = max(rangeX.highest, 0);
  rangeY.lowest = min(rangeY.lowest, 0);
  rangeY.highest = max(rangeY.highest, 0);

  if(rangeX.highest == rangeX.lowest) rangeX.highest += 1.0;
  if(rangeY.highest == rangeY.lowest) rangeY.highest += 1.0;
    
  innerWidth = width - (2 * X_DIST_FROM_EDGE);
  innerHeight = height - (2 * Y_DIST_FROM_EDGE);

  /* berechne die Position des Ursprungs. */
  pixelX.lowest = 0.0; pixelX.highest = innerWidth - 1.0;
  pixelY.lowest = 0.0; pixelY.highest = innerHeight - 1.0;

  getScaleOp(&scaleX, rangeX, pixelX);
  getScaleOp(&scaleY, rangeY, pixelY);
  cs->originX = X_DIST_FROM_EDGE + (unsigned) scale(scaleX, 0);
  cs->originY = (height - Y_DIST_FROM_EDGE - 1)
                 - (unsigned) scale(scaleY, 0);
  cs->x_mult = (float) innerWidth / (rangeX.highest - rangeX.lowest);
  cs->y_mult = (float) innerHeight / (rangeY.highest - rangeY.lowest);
  cs->x = cs->y = 0;
  cs->width = width;
  cs->height = height;
}        /* loadCS */


/*********************************************************/
/*********************************************************/
static void drawCoordinateSystem(d, cs)
Drawable d;
CoordSystem *cs;
{
  /* zeichne die x-Achse */
  XDrawLine(display, d, gc,                   /* Display, Drawable, GC */
            cs->x, cs->y + cs->originY,                  /* Startpunkt */
            cs->x + cs->width - 1, cs->y + cs->originY);   /* Endpunkt */

  /* zeichne die y-Achse */
  XDrawLine(display, d, gc,
            cs->x + cs->originX, cs->y,
            cs->x + cs->originX, cs->y + cs->height - 1);
}         /* drawCoordinateSystem */


/********************************************************/
/* Variable error wird gesetzt.                         */
/********************************************************/
static void drawHisto(d, cs, v)
Drawable d;
CoordSystem cs;
Vector v;
{ long v_dims, i, index;
  Number elem;
  XPoint *points;

  v_dims = dims(v);
  points = (XPoint *) malloc((2 * v_dims + 1) * sizeof(*points));
  if(! points) error(27);

  index = 0L;
  for(i = 0L; i < v_dims; i++)
  {
    elem = atDim(v, i+1);
    points[index].x = (short) translateX(cs, i);
    points[index].y = (short) translateY(cs, elem);
    index++;
    points[index].x = (short) translateX(cs, i+1);
    points[index].y = (short) translateY(cs, elem);
    index++;
  }       /* for */
  points[index].x = (short) translateX(cs, v_dims);
  points[index].y = (short) translateY(cs, 0);

  XDrawLines(display, d, gc,                    /* Display, Drawable, GC */
             points, index+1,                   /* XPoint[], npoints */
             CoordModeOrigin);                  /* mode */

  free(points);
  error = 0;
}         /* drawHisto */


/********************************************************/
/* Variable error wird gesetzt.                         */
/********************************************************/
static void drawHistogram(GW gw, Vector v, Range rangeY, PixelNo color)
{ unsigned numDims;
  Range rangeX;
  CoordSystem cs;
  Number markX_High, markX_Low, markY_High, markY_Low;

  numDims = dims(v);

  rangeX.lowest = 0.0;
  rangeX.highest = (Number) numDims;
  loadCS(&cs, gw->widthPerVector, gw->heightPerVector, rangeX, rangeY);
  markX_High = getBelow(cs.x_mult, rangeX.highest);
  markX_Low = getAbove(cs.x_mult, rangeX.lowest);
  markY_High = getBelow(cs.y_mult, rangeY.highest);
  markY_Low = getAbove(cs.y_mult, rangeY.lowest);

  cs.x = PM_LEFTMARGIN;
  cs.y = PM_TOPMARGIN;
  XSetForeground(display, gc, black);
  drawCoordinateSystem(gw->pm, &cs);
  drawMarkX(gw->pm, &cs, markX_Low);
  drawMarkX(gw->pm, &cs, markX_High);
  drawMarkY(gw->pm, &cs, markY_Low);
  drawMarkY(gw->pm, &cs, markY_High);
  XSetForeground(display, gc, color);
  drawHisto(gw->pm, cs, v);
  if(error) return;

  error = 0;
}         /* drawHistogram */


/********************************************************/
/* Variable error wird gesetzt.                         */
/********************************************************/
static unsigned drawHistogrammes(GW gw, VecColl vc, Range rangeY, 
                                 PixelNo color)
{ unsigned i, answer, numVectors, numDims, posX, posY;
  Range rangeX;
  CoordSystem cs;
  Number markX_High, markX_Low, markY_High, markY_Low;
  char buf[NUM_OF_PATS_STR_MAXLEN + 5];

  numDims = (unsigned) numberOfCols(vc);
  numVectors = (unsigned) numberOfRows(vc);
  rangeX.lowest = 0.0;
  rangeX.highest = (Number) numDims;
  loadCS(&cs, gw->widthPerVector, gw->heightPerVector, rangeX, rangeY);
  markX_High = getBelow(cs.x_mult, rangeX.highest);
  markX_Low = getAbove(cs.x_mult, rangeX.lowest);
  markY_High = getBelow(cs.y_mult, rangeY.highest);
  markY_Low = getAbove(cs.y_mult, rangeY.lowest);

  cs.x = PM_LEFTMARGIN;
  cs.y = PM_TOPMARGIN;
  for(i = gw->displayed; i <= numVectors; i++)
  {
    if(cs.x > gw->pmWidth)
    { cs.x = PM_LEFTMARGIN;
      cs.y += (gwDists[gw->kind] + gw->heightPerVector);
    }
    if(cs.y > gw->pmHeight) break;
    sprintf(buf, "%u", i);
    XDrawString(display, gw->pm, textGC,
                cs.x + DRAW_NR_LEFTMARGIN,
                cs.y - DRAW_NR_BOTTOMMARGIN,
                buf, strlen(buf)); 
    XSetForeground(display, gc, black);
    drawCoordinateSystem(gw->pm, &cs);
    drawMarkX(gw->pm, &cs, markX_Low);
    drawMarkX(gw->pm, &cs, markX_High);
    drawMarkY(gw->pm, &cs, markY_Low);
    drawMarkY(gw->pm, &cs, markY_High);
    XSetForeground(display, gc, color);
    drawHisto(gw->pm, cs, (Vector) at(vc, i));
    if(error) return;
    cs.x += (gwDists[gw->kind] + gw->widthPerVector);
  }      /* for */
  answer = i - 1;

  error = 0;
  return answer;
}         /* drawHistogrammes */


/********************************************************/
/* Variable error wird gesetzt.                         */
/********************************************************/
static void drawConnectedLines(d, cs, v)
Drawable d;
CoordSystem cs;
Vector v;
{ XPoint *points;
  long v_dims, i;
  Number elem;

  v_dims = dims(v);
  points = (XPoint *) malloc(v_dims * sizeof(XPoint));
  if(! points) error(27);

  for(i = 1L; i <= v_dims; i++)
  {
    points[i-1].x = (short) translateX(cs, i);
    points[i-1].y = (short) translateY(cs, atDim(v, i));
  }       /* for */

  if(v_dims > 1)
    XDrawLines(display, d, gc,                  /* Display, Drawable, GC */
             points, v_dims, CoordModeOrigin);  /* XPoints, numPoints, mode */
  else XFillRectangle(display, d, gc, 
             points[0].x, points[0].y, 2, 2); 

  free(points);
  error = 0;
}         /* drawConnectedLines */


/********************************************************/
/* Variable error wird gesetzt.                         */
/********************************************************/
static void drawPolyLine(GW gw, Vector v, Range rangeY, PixelNo color)
{ unsigned numDims;
  Range rangeX;
  CoordSystem cs;
  Number markX_High, markX_Low, markY_High, markY_Low;

  numDims = (unsigned) dims(v);

  rangeX.lowest = 1.0;
  rangeX.highest = (Number) numDims;
  loadCS(&cs, gw->widthPerVector, gw->heightPerVector, rangeX, rangeY);
  markX_High = getBelow(cs.x_mult, rangeX.highest);
  markX_Low = getAbove(cs.x_mult, rangeX.lowest);
  markY_High = getBelow(cs.y_mult, rangeY.highest);
  markY_Low = getAbove(cs.y_mult, rangeY.lowest);

  cs.x = PM_LEFTMARGIN;
  cs.y = PM_TOPMARGIN;
  XSetForeground(display, gc, black);
  drawCoordinateSystem(gw->pm, &cs);
  drawMarkX(gw->pm, &cs, markX_Low);
  drawMarkX(gw->pm, &cs, markX_High);
  drawMarkY(gw->pm, &cs, markY_Low);
  drawMarkY(gw->pm, &cs, markY_High);
  XSetForeground(display, gc, color);
  drawConnectedLines(gw->pm, cs, v);
  if(error) return;

  error = 0;
}         /* drawPolyLine */


/********************************************************/
/* Variable error wird gesetzt.                         */
/********************************************************/
static unsigned drawPolyLines(GW gw, VecColl vc, Range rangeY, PixelNo color)
{ unsigned i, answer, numVectors, numDims, posX, posY;
  Range rangeX;
  CoordSystem cs;
  Number markX_High, markX_Low, markY_High, markY_Low;
  char buf[NUM_OF_PATS_STR_MAXLEN + 5];

  numDims = (unsigned) numberOfCols(vc);
  numVectors = (unsigned) numberOfRows(vc);
  rangeX.lowest = 1.0;
  rangeX.highest = (Number) numDims;
  loadCS(&cs, gw->widthPerVector, gw->heightPerVector, rangeX, rangeY);
  markX_High = getBelow(cs.x_mult, rangeX.highest);
  markX_Low = getAbove(cs.x_mult, rangeX.lowest);
  markY_High = getBelow(cs.y_mult, rangeY.highest);
  markY_Low = getAbove(cs.y_mult, rangeY.lowest);

  cs.x = PM_LEFTMARGIN;
  cs.y = PM_TOPMARGIN;
  for(i = gw->displayed; i <= numVectors; i++)
  {
    if(cs.x > gw->pmWidth)
    { cs.x = PM_LEFTMARGIN;
      cs.y += (gwDists[gw->kind] + gw->heightPerVector);
    }
    if(cs.y > gw->pmHeight) break; 
    sprintf(buf, "%u", i);
    XDrawString(display, gw->pm, textGC,
                cs.x + DRAW_NR_LEFTMARGIN,
                cs.y - DRAW_NR_BOTTOMMARGIN,
                buf, strlen(buf)); 
    XSetForeground(display, gc, black);
    drawCoordinateSystem(gw->pm, &cs);
    drawMarkX(gw->pm, &cs, markX_Low);
    drawMarkX(gw->pm, &cs, markX_High);
    drawMarkY(gw->pm, &cs, markY_Low);
    drawMarkY(gw->pm, &cs, markY_High);
    XSetForeground(display, gc, color);
    drawConnectedLines(gw->pm, cs, (Vector) at(vc, i));
    if(error) return;
    cs.x += (gwDists[gw->kind] + gw->widthPerVector);
  }      /* for */
  answer = i - 1;
  
  error = 0;
  return answer;
}         /* drawPolyLines */


/********************************************************/
/* Variable error wird gesetzt.                         */
/********************************************************/
static void draw2DProjection(gw, vc, dimX, dimY, color)
GW gw;
VecColl vc;
long dimX, dimY;
PixelNo color;
{ XRectangle *rects;
  unsigned numVectors, i;
  Vector vec;
  CoordSystem cs;
  Range rangeX, rangeY;
  Number markX_High, markX_Low, markY_High, markY_Low;

  numVectors = (unsigned) size(vc);
  rects = (XRectangle *) malloc(numVectors * sizeof(XRectangle));
  if(! rects) error(27);

  rangeX.lowest = dimMin(vc, dimX);
  rangeX.highest = dimMax(vc, dimX);
  rangeY.lowest = dimMin(vc, dimY);
  rangeY.highest = dimMax(vc, dimY);
  loadCS(&cs, gw->curWidth, gw->curWidth, rangeX, rangeY);
  markX_High = getBelow(cs.x_mult, rangeX.highest);
  markX_Low = getAbove(cs.x_mult, rangeX.lowest);
  markY_High = getBelow(cs.y_mult, rangeY.highest);
  markY_Low = getAbove(cs.y_mult, rangeY.lowest);

  /* Zeichne das Koordinatensystem */
  cs.x = PM_LEFTMARGIN;
  cs.y = PM_TOPMARGIN;
  XSetForeground(display, gc, black);
  drawCoordinateSystem(gw->pm, &cs);
  drawMarkX(gw->pm, &cs, markX_Low);
  drawMarkX(gw->pm, &cs, markX_High);
  drawMarkY(gw->pm, &cs, markY_Low);
  drawMarkY(gw->pm, &cs, markY_High);

  for(i = 0L; i < numVectors; i++)
  {
    vec = (Vector) at(vc, i+1);
    rects[i].x = (short) translateX(cs, atDim(vec, dimX));
    rects[i].y = (short) translateY(cs, atDim(vec, dimY));
    rects[i].width = 2;
    rects[i].height = 2;
  }       /* for */

  XSetForeground(display, gc, color);
  XFillRectangles(display, gw->pm, gc,          /* Display, Drawable, GC */
             rects, numVectors);                /* XPoints, numPoints, */
  free(rects);
  error = 0;
}          /* draw2DProjection */


/**************************************************/
/**************************************************/
static Number getBelow(Number unitLength, Number n)
{ int distInPixels, zehnerPotenz = 1, expo = -1;
  Number answer;

  do
  { answer = floor(n * zehnerPotenz) / zehnerPotenz;
    distInPixels = (int) (unitLength * (n - answer));
    zehnerPotenz *= 10;
    expo++;
  } while(distInPixels > 8);

  return answer;
}          /* getBelow */


/**************************************************/
/**************************************************/
static Number getAbove(Number unitLength, Number n)
{ int distInPixels, zehnerPotenz = 1;
  Number answer;

  do
  { answer = floor(n * zehnerPotenz) / zehnerPotenz;
    distInPixels = (int) (unitLength * (n - answer));
    zehnerPotenz *= 10;
  } while(distInPixels > 8);

  return answer;
}          /* getAbove */


/***************************************************/
/***************************************************/
static void drawMarkX(Drawable d, CoordSystem *cs, Number n)
{ char buf[NUMBER_STR_LENGTH + 5];
  int width, x, textY, textX, len;

  if(n != 0)
  { sprintf(buf, "%g", n);
    len = strlen(buf);
    width = XTextWidth(drawFont, buf, len);
    x = (int) translateX(*cs, n);
    XDrawLine(display, d, gc,
              x, cs->y + cs->originY - 2,
              x, cs->y + cs->originY + 2);
    if(cs->originY > cs->height/2)
      textY = cs->y + cs->originY + DRAWFONT_HEIGHT + 2;
    else textY = cs->y + cs->originY - 4;
    textX = x - width/2;
    XDrawString(display, d, gc, textX, textY, buf, len);
  }
}          /* drawMarkX */


/***************************************************/
/***************************************************/
static void drawMarkY(Drawable d, CoordSystem *cs, Number n)
{ char buf[NUMBER_STR_LENGTH + 5];
  int width, y, textY, textX, len;

  if(n != 0)
  { sprintf(buf, "%g", n);
    len = strlen(buf);
    width = XTextWidth(drawFont, buf, len);
    y = (int) translateY(*cs, n);
    XDrawLine(display, d, gc,
              cs->x + cs->originX - 2, y,
              cs->x + cs->originX + 2, y);
    if(cs->originX > cs->width/2)
      textX = cs->x + cs->originX + 6;
    else textX = cs->x + cs->originX - width - 4;
    textY = y + 4;
    XDrawString(display, d, gc, textX, textY, buf, len);
  }
}          /* drawMarkY */






