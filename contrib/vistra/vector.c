#include "xvis.h"
#include <math.h>


/**************************************************/
/* Antworte einen neuen Vektor der Dimension dim. */
/* Falls nicht genuegend Speicher hierfuer vor-   */
/* handen, antworte NULL.                         */
/**************************************************/
Vector newVector(long dim)
{ Vector vec;
  Number *array;

  if(! (vec = (Vector) malloc(sizeof(*vec)))) return NULL;
  if(! (array = (Number *) malloc(dim * sizeof(Number))))
  { free(vec);
    return NULL;
  }
  vec->dimensions = dim;
  vec->elements = array;

  return vec;
}                   /* newVector */


/**************************************************/
/* Gebe den Speicherplatz fuer Vektor v frei.     */
/**************************************************/
void freeVector(Vector v)
{
  free(v->elements);
  free(v);
}                   /* freeVector */


/**************************************************/
/* Schreibe Vektor v in die Datei f. Tritt ein    */
/* Fehler auf, so gebe einen Wert <> 0 zurueck,   */
/* sonst 0.                                       */
/**************************************************/
int fprintVector(Vector v, FILE *f)
{ long i;

  for(i = 0L; i < v->dimensions-1; i++)
    if(fprintf(f, "%g ", v->elements[i]) < 0) return 1;
  if( fprintf(f, "%g", v->elements[v->dimensions-1]) < 0) return 1;
  return 0;
}              /* fprintVector */


/**************************************************/
/* Antworte die Dimension des Vektors v.          */
/**************************************************/
long dims(Vector v)
{
  return v->dimensions;
}             /* dim */


/**************************************************/
/* Antworte die i-te Dimension des Vektors v.     */
/**************************************************/
Number atDim(Vector v, long i)
{
  return v->elements[i - 1];
}             /* atDim */


/**************************************************/
/* Setze die i-te Dimension des Vektors v auf     */
/* den Wert val.                                  */
/**************************************************/
void putDim(Vector v, long i, Number val)
{
  v->elements[i - 1] = val;
}            /* putDim */


/**************************************************/
/* Setze jede Dimension des Vektors v auf den     */
/* Wert val.                                      */
/**************************************************/
void putAllDim(Vector v, Number val)
{ long numDims, i;

  numDims = dims(v);
  for(i = 0L; i < numDims; i++)
    v->elements[i] = val;
}            /* putAllDim */


/**************************************************/
/* Antworte das kleinste Element von v.           */
/**************************************************/
Number minimum(Vector v)
{ long numDims, i;
  Number answer;

  answer = v->elements[0];
  numDims = dims(v);
  for(i = 1L; i < numDims; i++)
    answer = min(answer, v->elements[i]);
  return answer;
}            /* minimum */


/**************************************************/
/* Antworte das groesste Element von v.           */
/**************************************************/
Number maximum(Vector v)
{ long numDims, i;
  Number answer;

  answer = v->elements[0];
  numDims = dims(v);
  for(i = 1L; i < numDims; i++)
    answer = max(answer, v->elements[i]);
  return answer;
}            /* maximum */


/**************************************************/
/* Antworte die Summe des Vektors v.              */
/**************************************************/
Number sum(Vector v)
{ Number answer;
  long i, numDims;

  answer = 0;
  numDims = dims(v);
  for(i = 1L; i <= numDims; i++) answer += atDim(v, i);
  return answer;
}        /* sum */


/**************************************************/
/* Antworte den Mittelwert des Vektors v.         */
/**************************************************/
Number avg(Vector v)
{
  return sum(v) / dims(v);
}        /* avg */


/**************************************************/
/* Vergleiche die Vektoren v1 und v2. Ist v1<v2   */
/* so antworte V_LESS, sind sie gleich, so        */
/* antworte V_EQUAL, und ist v1>v2, so antworte   */
/* V_GREATER.                                     */
/* v1 < v2 gdw. es gibt i, sodass fuer alle j<i   */
/* (j>=1) atDim(v1, i)=atDim(v2, i) und           */
/* atDim(v1, j) < atDim(v2, j).                   */
/* Die Vektoren v1 und v2 muessen die gleiche     */
/* Dimensionalitaet besitzen. Ein Test findet     */
/* nicht statt.                                   */
/**************************************************/
int vcmp(Vector v1, Vector v2)
{ long dimensions, j;
  Number e1, e2;

  dimensions = dims(v1);
  for(j = 1L; j <= dimensions; j++)
  {
    e1 = atDim(v1, j);
    e2 = atDim(v2, j);
    if(e1 < e2) return V_LESS;
    else if(e1 > e2) return V_GREATER;
  }
  return V_EQUAL;
}          /* vcmp */


/***************************************************/
/***************************************************/
Boolean veq(Vector v1, Vector v2)
{
  return vcmp(v1, v2) == V_EQUAL;
}          /* veq */


/**************************************************/
/* Antworte die Laenge des Vektors v.             */
/**************************************************/
Number length(Vector v)
{ Number sum = 0;
  long i, dim;

  dim = dims(v);
  for(i = 0L; i < dim; i++)
    sum += square(v->elements[i]);

  return sqrt(sum);
}          /* length */


/**************************************************/
/* Multipliziere jedes Element von v mit n.       */
/**************************************************/
void multiply(Vector v, Number n)
{ long i, dim;

  dim = dims(v);
  for(i = 0L; i < dim; i++)
    v->elements[i] = n * v->elements[i];
}          /* multiply */


/**************************************************/
/* Multipliziere jedes Element von v2 mit dem ent-*/
/* sprechenden Element von v1 und speicher das    */
/* Ergebnis in v1. v2 bleibt unveraendert.        */
/* Die Dimensionen von v1 und v2 muessen ueberein-*/
/* stimmen, eine Ueberpruefung findet nicht statt.*/
/**************************************************/
void multVector(Vector v1, Vector v2)
{ long i, dim;

  dim = dims(v1);
  for(i = 0L; i < dim; i++)
    v1->elements[i] *= v2->elements[i];
}          /* multVector */


/**************************************************/
/* Dividiere jedes Element von v1 durch das ent-  */
/* sprechende Element von v2 und speicher das     */
/* Ergebnis in v1. v2 bleibt unveraendert.        */
/* Die Dimensionen von v1 und v2 muessen ueberein-*/
/* stimmen, eine Ueberpruefung findet nicht statt.*/
/**************************************************/
void divVector(Vector v1, Vector v2)
{ long i, dim;

  dim = dims(v1);
  for(i = 0L; i < dim; i++)
    v1->elements[i] /= v2->elements[i];
}          /* divVector */


/**************************************************/
/* Addiere n von jedem Element des Vektors v.     */
/**************************************************/
void addConst(Vector v, Number n)
{ long i, dim;

  dim = dims(v);
  for(i = 0L; i < dim; i++)
    v->elements[i] += n;
}        /* addConst */


/**************************************************/
/* Addiere elementweise den Vektor v2 zum Vektor  */
/* v1 hinzu. v1 erhaelt anschliessend die Summe,  */
/* v2 bleibt unveraendert.                        */
/* v1 und v2 muessen die gleiche Dimension haben, */
/* eine Ueberpruefung findet nicht statt.         */
/**************************************************/
void addVector(Vector v1, Vector v2)
{ long i, dim;

  dim = dims(v1);
  for(i = 0L; i < dim; i++)
    v1->elements[i] += v2->elements[i];
}        /* addVector */


/*****************************************************/
/* Subtrahiere elementweise den Vektor v2 vom Vektor */
/* v1 hinzu. v1 erhaelt anschliessend die Summe,     */
/* v2 bleibt unveraendert.                           */
/* v1 und v2 muessen die gleiche Dimension haben,    */
/* eine Ueberpruefung findet nicht statt.            */
/*****************************************************/
void subVector(Vector v1, Vector v2)
{ long i, dim;

  dim = dims(v1);
  for(i = 0L; i < dim; i++)
    v1->elements[i] -= v2->elements[i];
}        /* subVector */


/**************************************************/
/* Negiere alle Elemente von v.                   */
/**************************************************/
void neg(Vector v)
{ long i, dim;

  dim = dims(v);
  for(i = 0L; i < dim; i++)
    v->elements[i] = - (v->elements[i]);
}        /* neg */


/**************************************************/
/* Ersetze alle Elemente von v durch deren Kehr-  */
/* wert. Es wird nicht getestet, ob ein Element 0 */
/* ist.                                           */
/**************************************************/
void invert(Vector v)
{ long i, dim;

  dim = dims(v);
  for(i = 0L; i < dim; i++)
    v->elements[i] = 1 / (v->elements[i]);
}        /* invert */


/**************************************************/
/* Antworte die Varianz des Vektors v.            */
/**************************************************/
Number var(Vector v)
{ double average, sum = 0;
  long dim, i;

  average = avg(v);
  dim = dims(v);
  for(i = 0; i < dim; i++)
    sum += square(v->elements[i] - average);

  return sum / dim;
}          /* var */


/**************************************************/
/* Antworte die Standardabweichung des Vektors v. */
/**************************************************/
Number sigma(Vector v)
{
  return sqrt(var(v));
}        /* sigma */


/**************************************************/
/* Skaliere den Vektor v mittels so.              */
/**************************************************/
void scaleVec(Vector v, ScaleOp so)
{ long dim, i;

  dim = dims(v);
  for(i = 0L; i < dim; i++)
    v->elements[i] = scale(so, v->elements[i]);
}        /* scaleVec */


/**************************************************/
/* Normiere den Vektor v auf Laenge 1.            */
/**************************************************/
void normalize(Vector v)
{ Number len;

  len = length(v);
  if(len != 0.0) multiply(v, 1 / len);
}        /* normalize */


/**************************************************/
/* Antworte die Position des ersten Auftretens von*/
/* n im Vektor v. Antworte 0L, falls n nicht als  */
/* Element von v enthalten ist.                   */
/**************************************************/
long detectFirst(Vector v, Number n)
{ long i, dim;

  dim = dims(v);
  for(i = 0L; i < dim; i++)
    if(v->elements[i] == n) return i+1;

  return 0L;
}        /* detectFirst */


/**************************************************/
/* Schreibe einen String nach buf, der in jeder   */
/* Zeile genau ein Element von v enthaelt.        */
/**************************************************/
void asStringVert(Vector v, char *buf)
{ long dim, i;

  dim = dims(v);
  for(i = 0L; i < dim; i++)
  { sprintf(buf, NUMBER_FORMAT_NL, v->elements[i]);
    buf += NUMBER_STR_LENGTH + SIZEOF_NL;
  }
  *(--buf) = '\0';
}        /* asStringVert */


/**************************************************/
/* Kopiere alle Elemente von coll in den Vektor v.*/
/* Die Groesse von coll und die Anzahl der        */
/* Dimensionen von v muessen identisch sein. Eine */
/* Ueberpruefung findet nicht statt.              */
/**************************************************/
void copyFromColl(Vector v, Collection coll)
{ long i;

  for(i = 1L; i <= size(coll); i++)
    putDim(v, i, *(Number *) at(coll, i));
}           /* copyFromColl */


/******************************************************/
/* Kopiere die Elemente von v2 nach v1.               */
/******************************************************/
void copyVec(Vector v1, Vector v2)
{ long i;

  for(i = 0L; i < v2->dimensions; i++)
    v1->elements[i] = v2->elements[i];
}           /* copyVec */


/******************************************************/
/* Antworte den Vektor v als eine Collection der      */
/* Elemente (Numbers).                                */
/* Antworte NULL, falls der Speicher fuer die         */
/* Collection nicht ausreicht.                        */
/******************************************************/
Collection asColl(Vector v)
{ Collection answer;
  long i, dim;

  if(! (answer = newColl())) return NULL;
  dim = dims(v);
  for(i = 0L; i < dim; i++)
    if(! add(answer, &(v->elements[i])))
    { freeColl(answer);
      return NULL;
    }

  return answer;
}           /* asColl */


/******************************************************/
/* Antworte einen neuen Vektor, der dim1 + dim2       */
/* Dimensionen besitzt, wobei dim1 die Dimension-     */
/* alitaet des Vektors v1 und dim2 die des Vektors v2 */
/* darstellen. Der neue Vektor bekommt als Elemente   */
/* die Elemente von v1 gefolgt von denen von v2.      */
/* Antworte NULL, falls nicht genuegend Speicher ver- */
/* fuegbar fuer den zurueckzugebenden Vektor.         */
/******************************************************/
Vector expand(Vector v1, Vector v2)
{ Vector answer;
  long i, dim1, dim2;

  dim1 = dims(v1);
  dim2 = dims(v2);
  answer = newVector(dim1 + dim2);
  if(! answer) return NULL;

  /* copy elements of v1 into answer */
  for(i = 0L; i < dim1; i++)
    answer->elements[i] = v1->elements[i];
  /* copy elements of v2 into answer */
  for(i = dim1; i < dim1+dim2; i++)
    answer->elements[i] = v2->elements[i - dim1];

  return answer;
}           /* expand */


/****************************************************/
/* Entferne vom Vektor v alle durch dims gegebenen  */
/* Dimensionen.                                     */
/* ds zeigt auf ein Array von Integern des Typs     */
/* long. Jedes Element des Arrays gibt eine zu      */
/* loeschende Dimension an. Die Elemente von ds     */
/* muessen aufsteigend sortiert sein. (1 3 4) z.B.  */
/* besagt, dass die Dimensionen 1, 3 und 4 geloescht*/
/* werden sollen. Um das Ende des Arrays zu mar-    */
/* kieren, muss dort eine 0L erscheinen.            */
/* num gibt die Anzahl der zu entfernenden Dim. an. */
/* Variable error wird gesetzt.                     */
/****************************************************/
void removeDims(Vector v, long *ds, long num)
{ long i, dim, outCount;
  Number *newElems;

  dim = dims(v);
  if(! (newElems = (Number *) malloc(sizeof(*newElems) * (dim-num))))
    error(1);

  outCount = 0L;
  for(i = 1L; i <= dim; i++)
    if(i == *ds) ds++;
    else newElems[outCount++] = v->elements[i-1];

  free(v->elements);
  v->elements = newElems;
  v->dimensions = outCount;
  error = 0;
}           /* removeDims */


/******************************************************/
/* Entferne vom Vektor v alle Dimensionen von from bis*/
/* to einschliesslich.                                */
/* Variable error wird gesetzt.                       */
/******************************************************/
void removeDimRange(Vector v, unsigned from, unsigned to)
{ unsigned i, numToRemove, firstToRemove, lastToRemove, count;
  Number *newElems;

  if((from<=to) && (from<=v->dimensions) && (to>=1))
  { 
    firstToRemove = max(1, from);
    lastToRemove = min(v->dimensions, to);
    numToRemove = lastToRemove - firstToRemove + 1;
    newElems = (Number *) malloc((v->dimensions-numToRemove) * sizeof(*newElems));
    if(! newElems) error(1);

    count = 0;
    for(i = 0; i < firstToRemove-1; i++)
      newElems[count++] = v->elements[i];
    for(i = lastToRemove; i < v->dimensions; i++)
      newElems[count++] = v->elements[i];

    free(v->elements);
    v->elements = newElems;
    v->dimensions -= numToRemove;
  }

  error = 0;
}           /* removeDimRange */


/************************************************************/
/* Fuehre auf dem Vektor v eine Fast Fourier Transformation */
/* durch.                                                   */
/* Der Funktion muessen zwei Arrays von doubles uebergeben  */
/* werden, deren Speicher bereits allokiert wurde und die   */
/* mindestens sovielen Elementen Platz bieten wie die An-   */
/* zahl Dimensionen von v.                                  */
/************************************************************/
void fft(Vector v, double *VecRe, double *VecIm)
{
 int Size, Forward;
 long ii;
 int LenHalf,Stage,But,ButHalf;
 int i,j=1,k,ip,pot2=0;
 double ArcRe,ArcIm,dArcRe,dArcIm,ReBuf,ImBuf,ArcBuf,Arc;

 Size = dims(v);
 Forward = 1;
 for(ii = 0L; ii < Size; ii++)
 { VecRe[ii] = (double) v->elements[ii];
   VecIm[ii] = 0.0;
 }

 while(Size!=(1<<pot2)){
   pot2++;
 }

 LenHalf = Size >> 1;
 for(i=1;i<Size;i++){
   if(i<j){
     ReBuf = VecRe[j-1];
     ImBuf = VecIm[j-1];
     VecRe[j-1] = VecRe[i-1];
     VecIm[j-1] = VecIm[i-1];
     VecRe[i-1] = ReBuf;
     VecIm[i-1] = ImBuf;
   }
   k = LenHalf;
   while(k<j){
     j -= k;
     k = k >> 1;
   }
   j += k;
 }
 for(Stage=1;Stage<=pot2;Stage++){
   But = 1 << Stage;
   ButHalf = But >> 1;
   ArcRe = 1.0;
   ArcIm = 0.0;
   Arc = (double)(PI/ButHalf);
   dArcRe = (double)cos(Arc);
   dArcIm = (double)sin(Arc);

   if(Forward){
     dArcIm = -dArcIm;
   }

   for(j=1;j<=ButHalf;j++){
     i = j;
     while(i<=Size){
       ip = i + ButHalf;
       ReBuf = ((VecRe[ip-1] * ArcRe) - (VecIm[ip-1] * ArcIm));
       ImBuf = ((VecRe[ip-1] * ArcIm) + (VecIm[ip-1] * ArcRe));
       VecRe[ip-1] = VecRe[i-1] - ReBuf;
       VecIm[ip-1] = VecIm[i-1] - ImBuf;
       VecRe[i-1] = VecRe[i-1] + ReBuf;
       VecIm[i-1] = VecIm[i-1] + ImBuf;
       i += But;
     }
     ArcBuf = ArcRe;
     ArcRe  = ArcRe * dArcRe - ArcIm * dArcIm;
     ArcIm  = ArcBuf * dArcIm + ArcIm * dArcRe;
   }
 }
 if(Forward){
   for(j=1;j<=Size;j++){
     VecRe[j-1] /= Size;
     VecIm[j-1] /= Size;
   }
 }

 for(ii = 0L; ii < v->dimensions; ii++)
   v->elements[ii] = (Number) sqrt(VecRe[ii]*VecRe[ii] + VecIm[ii]*VecIm[ii]);
}         /* fft */


/*************************************************************/
/* Fuehre auf Vektor vec eine hlog-Operation aus.            */
/*************************************************************/
void hlog(Vector vec)
{ Number *hilfelems;
  int BreiteHoehe,x,y,u,v;
  double BreiteHoeheHalbe,StepRadius,StepAngel;
  double Radius,Angel;

/* HilfArray =(double **)calloc(BreiteHoehe,sizeof(double *));
 for(y=0;y<BreiteHoehe;y++){
   HilfArray[y] = (double *)calloc(BreiteHoehe,sizeof(double));
 }
 Field = *Array; */

 BreiteHoehe = (int) sqrt((double) vec->dimensions);
 hilfelems = (Number *) malloc(vec->dimensions * sizeof(*hilfelems));
 if(! hilfelems) handleErr(1);

 BreiteHoeheHalbe = BreiteHoehe/2.0;
 StepRadius = exp(log(BreiteHoeheHalbe)/(BreiteHoehe-1));
 StepAngel = 2.0 * PI / BreiteHoehe;

 for(v=0;v<BreiteHoehe;v++){
   for(u=0;u<BreiteHoehe;u++){
     Radius = pow(StepRadius,(double)v);
     Angel  = StepAngel * u;
     x = BreiteHoeheHalbe + Radius * cos(Angel); x = min(x,(BreiteHoehe-1));
     y = BreiteHoeheHalbe + Radius * sin(Angel); y = min(y,(BreiteHoehe-1));
     hilfelems[u*BreiteHoehe+v] = vec->elements[y*BreiteHoehe+x];
   }
 }

 free(vec->elements);
 vec->elements = &hilfelems[0];
}       /* hlog */


/*******************************************************/
/* Schreibe alle Elemente von v nacheinander in den    */
/* Puffer buf. Die Elemente werden durch numBlanks     */
/* Blanks voneinander getrennt.                        */
/* Antworte einen Zeiger auf erste Zeichen hinter dem  */
/* geschriebenen String.                               */
/*******************************************************/
char *printVec(Vector v, char *buf, unsigned numBlanks)
{ long i;
  char *cp = buf;

  for(i = 0L; i < v->dimensions - 1; i++)
  { sprintf(cp, NUMBER_FORMAT, v->elements[i]);
    cp += NUMBER_STR_LENGTH;
    cp = printnchr(cp, numBlanks, ' ');
  }
  sprintf(cp, NUMBER_FORMAT, v->elements[v->dimensions - 1]);
  return cp + NUMBER_STR_LENGTH;
}           /* printVec */


