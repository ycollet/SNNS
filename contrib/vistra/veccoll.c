#include "xvis.h"
#include <math.h>


/*******************************************************/
/* Antworte die Anzahl der Zeilenvektoren von vc.      */
/*******************************************************/
long numberOfRows(VecColl vc)
{
  return size(vc);
}         /* numberOfRows */


/*******************************************************/
/* Antworte die Anzahl der Spaltenvektoren von vc.     */
/*******************************************************/
long numberOfCols(VecColl vc)
{
  return dims((Vector) at(vc, 1));
}         /* numberOfCols */


/*******************************************************/
/* Antworte die Anzahl konstanter Dimensionen von vc.  */
/* Schreibe die Nummern dieser Dimensionen in auf-     */
/* steigender Folge ins Feld lp und setze ans Ende eine*/
/* 0.                                                  */
/*******************************************************/
unsigned constCols(VecColl vc, long *lp)
{ long i, nvecs, ndims;
  unsigned count;

  nvecs = size(vc);
  ndims = numberOfCols(vc);
  for(count = 0, i = 1L; i <= ndims; i++)
    if(dimMin(vc, i) == dimMax(vc, i)) lp[count++] = i;
  lp[count] = 0;

  return count;  
}         /* constCols */


/*******************************************************/
/* Antworte den Mittelwert der Elemente aller Vektoren,*/
/* die durch vc gegeben sind.                          */
/*******************************************************/
Number overallAvg(VecColl vc)
{ long i, numVectors;
  Number sum = 0;

  numVectors = size(vc);
  for(i = 1L; i <= numVectors; i++)
    sum += avg((Vector) at(vc, i));

  return sum / numVectors;
}          /* overallAvg */


/*******************************************************/
/* Antworte das kleinste Element aller Vektoren        */
/* aus vc.                                             */
/*******************************************************/
Number overallMin(VecColl vc)
{ long numVectors, i;
  Number answer;

  numVectors = size(vc);
  answer = minimum((Vector) at(vc, 1));
  for(i = 2L; i <= numVectors; i++)
    answer = min(answer, minimum((Vector) at(vc, i)));
  return answer;
}          /* overallMin */


/*******************************************************/
/* Antworte das groesste Element aller Vektoren        */
/* aus vc.                                             */
/*******************************************************/
Number overallMax(VecColl vc)
{ long numVectors, i;
  Number answer;

  numVectors = size(vc);
  answer = maximum((Vector) at(vc, 1));
  for(i = 2L; i <= numVectors; i++)
    answer = max(answer, maximum((Vector) at(vc, i)));
  return answer;
}          /* overallMax */


/*******************************************************/
/* Antworte die globale Standardabweichung von vc.     */
/*******************************************************/
Number overallStddev(VecColl vc)
{ long i, j, nvecs, ncols;
  unsigned long nnums;
  double totalAvg, totalVar, sum;
  Vector v;

  nvecs = size(vc);
  ncols = numberOfCols(vc);
  nnums = (unsigned long) nvecs * (unsigned long) ncols;
  totalAvg = overallAvg(vc);

  totalVar = 0;
  for(i = 1L; i <= nvecs; i++)
  { sum = 0;
    v = (Vector) at(vc, i);
    for(j = 1L; j <= ncols; j++) sum += square(atDim(v, j) - totalAvg);
    totalVar += sum / nnums;
  }

  return sqrt(totalVar);
}          /* overallStddev */


/*******************************************************/
/* Antworte den Mittelwert der dim-ten Elemente aller  */
/* Vektoren, die durch vc gegeben sind.                */
/*******************************************************/
Number dimAvg(VecColl vc, long dim)
{ long i, numVectors;
  Number sum = 0;

  numVectors = size(vc);
  for(i = 1L; i <= numVectors; i++)
    sum += atDim((Vector) at(vc, i), dim);

  return sum / numVectors;
}          /* dimAvg */


/*******************************************************/
/* Antworte das kleinste Element, das irgendein Vektor */
/* aus vc in der Dimension dim enthaelt.               */
/*******************************************************/
Number dimMin(VecColl vc, long dim)
{ long i, numVectors;
  Number answer;

  numVectors = size(vc);
  answer = atDim((Vector) at(vc, 1L), dim);
  for(i = 2L; i <= numVectors; i++)
    answer = min(answer, atDim((Vector) at(vc, i), dim));

  return answer;
}          /* dimMin */


/*******************************************************/
/* Antworte das groesste Element, das irgendein Vektor */
/* aus vc in der Dimension dim enthaelt.               */
/*******************************************************/
Number dimMax(VecColl vc, long dim)
{ long i, numVectors;
  Number answer;

  numVectors = size(vc);
  answer = atDim((Vector) at(vc, 1L), dim);
  for(i = 2L; i <= numVectors; i++)
    answer = max(answer, atDim((Vector) at(vc, i), dim));

  return answer;
}          /* dimMax */


/*******************************************************/
/* Setze das i-te Element von v auf das Skalar, das man*/
/* erhaelt, wenn auf den i-ten Zeilenvektor von vc die */
/* Funktion vecFunc angewendet wird.                   */
/* vecFunc muss eine Number antworten und darf nur     */
/* einen Vektor als einzigen Parameter benutzen.       */
/*******************************************************/
void collectRows(vc, vecFunc, v)
VecColl vc;
Number (*vecFunc)();
Vector v;
{ long numVectors, i;

  numVectors = size(vc);
  for(i = 1L; i <= numVectors; i++)
    putDim(v, i, (*vecFunc)((Vector) at(vc, i)));
}        /* collectRows */


/*******************************************************/
/* Setze das i-te Element von v auf das Skalar, das man*/
/* erhaelt, wenn auf den i-ten Spaltenvektor von vc die*/
/* Funktion vecFunc angewendet wird.                   */
/* vecFunc muss eine Number antworten und darf nur     */
/* einen Vektor als einzigen Parameter benutzen.       */
/*******************************************************/
void collectCols(vc, vecFunc, v)
VecColl vc;
Number (*vecFunc)();
Vector v;
{ Vector col;
  long i, numCols;

  numCols = numberOfCols(vc);
  if(! (col = newVector(size(vc)))) handleErr(1);

  for(i = 1L; i <= numCols; i++)
  { colVec(vc, i, col);
    putDim(v, i, (*vecFunc)(col));
  }

  freeVector(col);
}        /* collectRows */


/*******************************************************/
/* Rufe die Funktion vecFunc fuer jeden Zeilenvektor   */
/* von vc auf. vecFunc erwartet diesen Vektor als      */
/* einzigen Parameter.                                 */
/*******************************************************/
void doRows(vc, vecFunc)
VecColl vc;
void (*vecFunc)();
{ long i, numVectors;

  numVectors = numberOfRows(vc);
  for(i = 1L; i <= numVectors; i++)
    (*vecFunc)((Vector) at(vc, i));
}        /* doRows */


/*******************************************************/
/* Fuelle Vektor v mit dem dim-ten Spaltenvektor von   */
/* vc. Die Dimension von v muss groesser oder gleich   */
/* der Anzahl Zeilen von vc sein. Eine Ueberpruefung   */
/* findet nicht statt.                                 */
/*******************************************************/
void colVec(VecColl vc, long dim, Vector v)
{ long numVectors, i;

  numVectors = size(vc);
  for(i = 1L; i <= numVectors; i++)
    putDim(v, i, atDim((Vector) at(vc, i), dim));
}         /* colVec */


/*********************************************************/
/* Addiere/subtrahiere/multipliziere oder dividiere jeden*/
/* Zeilenvektor von vc mit der Konstanten an             */
/* der entsprechenden Position in v.                     */
/* Die Rechenart ist durch Character c gegeben.          */
/* Erlaubte Werte von c: '+', '-', '*' oder '/'.         */
/*********************************************************/
void compScalarsRow(VecColl vc, Vector v, char c)
{ long numVectors, i;

  numVectors = size(vc);
  switch(c)
  {
    case '+': for(i = 1L; i <= numVectors; i++)
                addConst((Vector) at(vc, i), atDim(v, i));
              break;
    case '-': for(i = 1L; i <= numVectors; i++)
                addConst((Vector) at(vc, i), - atDim(v, i));
              break;
    case '*': for(i = 1L; i <= numVectors; i++)
                multiply((Vector) at(vc, i), atDim(v, i));
              break;
    case '/': for(i = 1L; i <= numVectors; i++)
                multiply((Vector) at(vc, i), 1 / atDim(v, i));
              break;
  }       /* switch */
}         /* compScalarsRow */


/*********************************************************/
/* Addiere/subtrahiere/multipliziere oder dividiere jeden*/
/* Spaltenvektor von vc mit der Konstanten an            */
/* der entsprechenden Position in v.                     */
/* Die Rechenart ist durch Character c gegeben.          */
/* Erlaubte Werte von c: '+', '-', '*' oder '/'.         */
/*********************************************************/
void compScalarsCol(VecColl vc, Vector v, char c)
{ long numVectors, i;

  numVectors = size(vc);
  switch(c)
  {
    case '+': for(i = 1L; i <= numVectors; i++)
                addVector((Vector) at(vc, i), v);
              break;
    case '-': for(i = 1L; i <= numVectors; i++)
                subVector((Vector) at(vc, i), v);
              break;
    case '*': for(i = 1L; i <= numVectors; i++)
                multVector((Vector) at(vc, i), v);
              break;
    case '/': for(i = 1L; i <= numVectors; i++)
                divVector((Vector) at(vc, i), v);
              break;
  }       /* switch */
}         /* compScalarsCol */


/****************************************************/
/* Skaliere alle Elemente von vc auf den Werte-     */
/* bereich von from bis to.                         */
/****************************************************/
void scaleAll(VecColl vc, Number from, Number to, ScaleOp *sop)
{ Range old, new;
  long numVectors, i;

  /* Berechne den bisherigen Wertebereich */
  old.lowest = overallMin(vc);
  old.highest = overallMax(vc);

  new.lowest = from;
  new.highest = to;

  getScaleOp(sop, old, new);
  numVectors = size(vc);
  for(i = 1L; i <= numVectors; i++)
    scaleVec((Vector) at(vc, i), *sop);
}          /* scaleAll */


/*****************************************************/
/* Fuehre auf allen Zeilenvektoren von vc eine       */
/* Fast Fourier Transformation durch.                */
/* Variable error wird gesetzt.                      */
/*****************************************************/
void fftRows(VecColl vc)
{ double *real, *imag;
  long nrows, ncols, i;

  nrows = numberOfRows(vc);
  ncols = numberOfCols(vc);
  if(! (real = (double *) malloc(ncols * sizeof(*real)))) error(1);
  if(! (imag = (double *) malloc(ncols * sizeof(*imag))))
  { free(real);
    error(1);
  }

  for(i = 1L; i <= nrows; i++)
    fft((Vector) at(vc, i), real, imag);

  error = 0;
}


/*****************************************************/
/* Entferne alle Zeilenvektoren von vc von Zeile     */
/* from bis Zeile to einschliesslich.                */
/*****************************************************/
void removeRowRange(VecColl vc, unsigned from, unsigned to)
{
  freeFromTo(vc, from, to, freeVector);
}          /* removeRowRange */


/*****************************************************/
/* Entferne alle Spaltenvektoren von vc von Spalte   */
/* from bis Spalte to einschliesslich.               */
/* Variable error wird gesetzt.                      */
/*****************************************************/
void removeColRange(VecColl vc, unsigned from, unsigned to)
{ unsigned numVectors, i;
  
  numVectors = size(vc);
  for(i = 1; i <= numVectors; i++)
  { removeDimRange((Vector) at(vc, i), from, to);
    if(error) return;
  }

  error = 0;
}          /* removeColRange */


/******************************************************/
/* Ersetze den Inhalt des row-ten Zeilenvektors durch */
/* den Inhalt von v.                                  */
/******************************************************/
void replaceRow(VecColl vc, unsigned row, Vector v)
{
  copyVec((Vector) at(vc, row), v);
}          /* replaceRow */


/******************************************************/
/* Ersetze den Inhalt des col-ten Spaltenvektors durch*/
/* den Inhalt von v.                                  */
/******************************************************/
void replaceCol(VecColl vc, unsigned col, Vector v)
{ unsigned nrows, i;

  nrows = size(vc);
  for(i = 1; i <= nrows; i++)
    putDim((Vector) at(vc, i), col, atDim(v, i));
}          /* replaceCol */


/******************************************************/
/* Schreibe die Elemente from bis to des col-ten      */
/* Spaltenvektors von vc nach buf.                    */
/* Jedes Element steht in einer Zeile des Strings.    */
/******************************************************/
void colAsStringVert(VecColl vc, long col, long from, long to, char *buf)
{ long i;

  for(i = from; i <= to; i++)
  { sprintf(buf, NUMBER_FORMAT_NL, atDim((Vector) at(vc, i), col));
    buf += NUMBER_STR_LENGTH + SIZEOF_NL;
  }
  *(--buf) = '\0';
}          /* colAsStringVert */


/******************************************************/
/* Berechne die Kovarianz-Matrix der Vektoren von vc. */
/* Der Speicher fuer die Matrix muss vom Aufrufer     */
/* allokiert werden und ein Pointer durch Parameter   */
/* covMatrix uebergeben werden.                       */
/* Variable error wird gesetzt.                       */
/******************************************************/
void covariance(VecColl vc, Number **covMatrix)
{ int i, j, k;
  int n = (int) numberOfCols(vc);
  int m = (int) numberOfRows(vc);
  Number *mean;
  Vector vec;

  /* berechne die Mittelwerte der einzelnen Spalten (Dimensionen) */
  if(! (mean = (Number *) malloc(sizeof(*mean) * n))) error(1);
  for(i = 0; i < n; i++)
    mean[i] = dimAvg(vc, (long) i+1);

  /* initialisiere covMatrix */
  for(i = 0; i < n; i++)
    for(j = 0; j <= i; j++)
      covMatrix[i][j] = 0.0;

  /* berechne eine Haelfte der Matrix */
  for(k = 0; k < m; k++)
  { vec = (Vector) at(vc, k+1);
  
    for(i = 0; i < n; i++)
      for(j = 0; j <= i; j++)
        covMatrix[i][j] += (atDim(vec, i+1) - mean[i]) *
                           (atDim(vec, j+1) - mean[j]);
  }

  /* spiegel die Matrix an der Diagonalen */
  for(i = 0; i < n; i++)
  { for(j = 0; j < i; j++)
    { covMatrix[i][j] /= m;
      covMatrix[j][i] = covMatrix[i][j];
    }
    covMatrix[i][i] /= m;
  }

  free(mean);
  error = 0;  
}          /* covariance */


