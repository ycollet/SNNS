#include "xvis.h"
#include <ctype.h>

static FILE *patternFile;          /* bearbeitetes Pattern File */
static Format format;
static Patterns pats;
static enum Token tok;
static long inDims, outDims;       /* Dimensionen der Ein- bzw. Ausgabevekt.*/
static long inCount, outCount, classCount;
                                   /* Zaehlen die gelesenen bzw. ge-       */
                                   /* schriebenen Ein/Ausgabevektoren bzw. */
                                   /* Klassen-Namen.                       */
static long numPats;               /* Anzahl von Patterns */

/* Private function headers */
static long classNoOfVec(Collection, Vector);
static void addMapping(Collection, Vector, long);  /* error gesetzt */
static char *printStatVals(char *, VecColl);

#include "fin.c"
#include "fout.c"
#include "n01.c"
#include "lvq.c"


/*************************************************/
/* Gebe eine neues Patterns-Objekt zurueck.      */
/* Setze Variable error.                         */
/*************************************************/
Patterns newPatterns()
{ Patterns answer;

  answer = (Patterns) malloc(sizeof(*answer));
  if(answer == NULL) error(1);                /* not enough memory */

  if(! (answer->inputs = newColl())) error(1);
  if(! (answer->outputs = newColl())) error(1);
  if(! (answer->classes = newColl())) error(1);
  if(! (answer->classNos = newColl())) error(1);
  answer->symtab = newSymtab(NO_BUCKETS);
  if(error) return NULL;
  answer->count = 0L;
  answer->classCount = 0L;

  return answer;
}         /* newPatterns */


/********************************************/
/* Gebe den Speicher des Patterns-Objekts   */
/* p frei.                                  */
/********************************************/
void freePatterns(Patterns p)
{
  freeDeep(p->inputs, freeVector);
  freeDeep(p->outputs, freeVector);
  freeCollAll(p->classNos);
  freeColl(p->classes);
  freeSymtab(p->symtab);
  free(p);
}         /* freePatterns */


double drand48();
void srand48(long);

/*******************************************************/
/* Aendere die Reihenfolge der Patterns willkuerlich.  */
/* Falls v != NULL, so aendere die Reihenfolge der     */
/* Elemente von v gemaess der Aenderung der Patterns.  */
/* Variable error wird gesetzt.                        */
/*******************************************************/
void randomize(Patterns p, Vector v)
{ VecColl randInputs, randOutputs;
  Collection randClasses, randClassNos, randVec, vColl;
  long i, index;
  double randNum;
  Number *newElems;

  if(! (randInputs = newColl())) error(1);
  if(! (randOutputs = newColl())) error(1);
  if(! (randClasses = newColl())) error(1);
  if(! (randClassNos = newColl())) error(1);
  if(v)
  { if(! (vColl = asColl(v))) error(1);
    if(! (randVec = newColl())) error(1);
  }

  srand48((long) time(NULL));
  for(i = p->count; i >= 1; i--)
  {
    /* Generiere Zufallszahl zwischen 0.0 und 1.0 */
    randNum = drand48();
    index = min(((long) (randNum * i)) + 1L, i);
    if(! add(randInputs, at(p->inputs, index))) error(1);
    removeAt(p->inputs, index);
    if(! add(randClassNos, at(p->classNos, index))) error(1);
    removeAt(p->classNos, index);
    if(hasOutputs(p))
    { if(! add(randOutputs, at(p->outputs, index))) error(1);
      removeAt(p->outputs, index);
    }
    if(hasClassNames(p))
    { if(! add(randClasses, at(p->classes, index))) error(1);
      removeAt(p->classes, index);
    }
    if(v)
    { if(! add(randVec, at(vColl, index))) error(1);
      removeAt(vColl, index);
    }
  }        /* for */

  freeColl(p->inputs);
  freeColl(p->classNos);
  freeColl(p->outputs);
  freeColl(p->classes);
  if(v)
  { copyFromColl(v, randVec);
    freeColl(vColl);
    freeColl(randVec);
  }

  p->inputs = randInputs;
  p->outputs = randOutputs;
  p->classes = randClasses;
  p->classNos = randClassNos;

  error = 0;
}          /* randomize */


/********************************************/
/* Entferne von p alle Patterns der Nummern */
/* from bis to einschliesslich.             */
/* Antworte die Anzahl entfernter Patterns. */
/* Variable error wird gesetzt.             */
/********************************************/
unsigned removePatterns(Patterns p, unsigned from, unsigned to)
{ unsigned i, firstToRemove, lastToRemove, numToRemove;
  unsigned answer = 0;

  if(from <= to && from <= p->count && to >= 1)
  {
    firstToRemove = max(1, from);
    lastToRemove = min(p->count, to);
    numToRemove = lastToRemove - firstToRemove + 1;
    if(numToRemove >= p->count) error(28);
    removeRowRange(p->inputs, firstToRemove, lastToRemove);
    if(hasOutputs(p))
      removeRowRange(p->outputs, firstToRemove, lastToRemove);
    if(hasClassNames(p))
      removeFromTo(p->classes, firstToRemove, lastToRemove);
    freeFromTo(p->classNos, firstToRemove, lastToRemove, free);
    p->count -= numToRemove;
    answer = numToRemove;
  }

  error = 0;
  return answer;
}         /* removePatterns */


/********************************************/
/* Entferne alle Dimensionen von from bis   */
/* to einschliesslich von den Eingabe-      */
/* vektoren, falls flag==TRUE, oder von den */
/* Ausgabevektoren, falls flag==FALSE.      */
/* Antworte die tatsaechliche Anzahl an     */
/* entfernten Dimensionen.                  */
/* Variable error wird gesetzt.             */   
/********************************************/
unsigned removeCols(Patterns p, Boolean flag, unsigned from, unsigned to)
{ unsigned ndims, firstToRemove, lastToRemove, numToRemove = 0;
  VecColl vc;

  ndims = (flag ? p->inputDims : p->outputDims);
  if((from<=to) && (from<=ndims) && (to>=1))
  { 
    firstToRemove = max(1, from);
    lastToRemove = min(ndims, to);
    numToRemove = lastToRemove - firstToRemove + 1;
    if(numToRemove >= ndims) error(29);
    vc = (flag ? p->inputs : p->outputs);
    removeColRange(vc, firstToRemove, lastToRemove);
    if(error) return;
    if(flag) p->inputDims -= numToRemove;
    else p->outputDims -= numToRemove;    
  }

  error = 0;
  return numToRemove;
}          /* removeCols */


/**************************************************************/
/* Entferne alle Dimensionen, die durchs Feld lp gegeben sind.*/
/* Entferne sie von den Eingabevektoren, falls flag==TRUE     */
/* bzw. von den Ausgabevektoren, falls flag==FALSE.           */
/* Das Feld lp wird durch eine 0 begrenzt.                    */
/* count gibt die Groesse des Feldes lp an.                   */
/* Es wird die Anzahl der entfernten Dimensionen geantwortet. */
/* Variable error wird gesetzt.                               */
/**************************************************************/
void removeDimList(Patterns p, Boolean flag, long *lp, long count)
{ long numVectors, numDims, i;
  VecColl vc;

  numDims = (flag ? p->inputDims : p->outputDims);
  vc = (flag ? p->inputs : p->outputs);
  numVectors = num(p);

  /* Entferne die Dimensionen von allen Vektoren */
  for(i = 1L; i <= numVectors; i++)
  { removeDims((Vector) at(vc, i), lp, count);
    if(error) return;
  }
  if(flag) p->inputDims -= count;
  else p->outputDims -= count;
  error = 0;
}          /* removeDimList */


/********************************************/
/* Antworte die Anzahl der Input-Patterns   */
/* von p.                                   */
/********************************************/
long num(Patterns p)
{
  return p->count;
}         /* num */


/********************************************/
/* Antworte eine VecColl aller Input        */
/* Vektoren von p.                          */
/********************************************/
VecColl inputs(Patterns p)
{
  return p->inputs;
}          /* inputs */


/********************************************/
/* Antworte eine VecColl aller Output       */
/* Vektoren von p. Antworte eine leere      */
/* VecColl, falls keine vorhanden sind.     */
/********************************************/
VecColl outputs(Patterns p)
{
  return p->outputs;
}          /* outputs */


/********************************************/
/* Antworte eine Collection aller Klassen   */
/* Namen von p. Antworte eine leere         */
/* Collection, falls keine vorhanden sind.  */
/********************************************/
Collection classes(Patterns p)
{
  return p->classes;
}          /* classes */


/********************************************/
/* Antworte eine Collection aller Klassen-  */
/* Nummern von p. Antworte eine leere       */
/* Collection, falls diese noch nicht be-   */
/* rechnet wurden.                          */
/********************************************/
Collection classNos(Patterns p)
{
  return p->classNos;
}          /* classNos */


/********************************************/
/* Antworte die Anzahl verschiedener Aus-   */
/* klassen von p. Diese entspricht auch der */
/* maximalen Klassen-Nummer.                */
/* Gibt es keine Ausgabevektoren und Klassen*/
/* oder wurden die Nummern noch nicht be-   */
/* rechnet, so wird 0 zurueckgegeben.       */
/********************************************/
long maxClassNo(Patterns p)
{
  return p->classCount;
}


/********************************************/
/* Antworte true gdw. die Patterns p auch   */
/* Ausgabevektoren besitzen.                */
/********************************************/
Boolean hasOutputs(Patterns p)
{
  return notEmpty(outputs(p));
}       /* hasOutputs */


/********************************************/
/* Antworte true gdw. die Patterns p auch   */
/* Klassennamen besitzen.                   */
/********************************************/
Boolean hasClassNames(Patterns p)
{
  return notEmpty(classes(p));
}       /* hasClassNames */


/********************************************/
/* Antworte einen String, der die Klasse des*/
/* i-ten Patterns von p darstellt. Dies ist */
/* entweder ein Klassen-Name oder ein String*/
/* der die Klassen-Nummer repraesentiert.   */
/********************************************/
char *classString(Patterns p, long i)
{ static char buf[NUM_OF_PATS_STR_MAXLEN + 1];

  if(hasClassNames(p)) return (char *) at(p->classes, i);
  else
  { sprintf(buf, "%ld", *(long *) at(p->classNos, i));
    return buf;
  }
}         /* classString */


#define ROW_TITLE_LEN      21

/********************************************/
/* Antworte einen String, der den Text ent- */
/* haelt, der im Statistik-Fenster angezeigt*/
/* werden soll.                             */
/* Antworte NULL, falls nicht genuegend     */
/* Speicher vorhanden.                      */
/********************************************/
char *statString(Patterns p, char *fn)
{ char *buf, *cp;
  unsigned headlen, vallen, buflen;

  headlen = 5*ROW_TITLE_LEN + 6*SIZEOF_NL + strlen(fn) +
            4*NUM_OF_PATS_STR_MAXLEN;
  vallen = 5*ROW_TITLE_LEN + 6*SIZEOF_NL + 4*NUMBER_STR_LENGTH;

  buflen = headlen + vallen + (hasOutputs(p) ? vallen : 0) +
           100;               /* zur Sicherheit */
  if(! (buf = (char *) malloc(buflen))) return NULL;

  cp = buf;
  sprintf(cp, "Pattern file:        %s\n", fn);
  cp += ROW_TITLE_LEN + SIZEOF_NL + strlen(fn);
  sprintf(cp, "Number of patterns:  ");
  cp += ROW_TITLE_LEN;
  sprintf(cp, NUM_OF_PATS_FORMAT_L_NL, num(p));
  cp += NUM_OF_PATS_STR_MAXLEN + SIZEOF_NL;
  sprintf(cp, "Input dimensions:    ");
  cp += ROW_TITLE_LEN;
  sprintf(cp, NUM_OF_PATS_FORMAT_L_NL, inputDims(p));
  cp += NUM_OF_PATS_STR_MAXLEN + SIZEOF_NL;
  sprintf(cp, "Output dimensions:   ");
  cp += ROW_TITLE_LEN;
  sprintf(cp, NUM_OF_PATS_FORMAT_L_NL, outputDims(p));
  cp += NUM_OF_PATS_STR_MAXLEN + SIZEOF_NL;
  sprintf(cp, "Number of classes:   ");
  cp += ROW_TITLE_LEN;
  sprintf(cp, NUM_OF_PATS_FORMAT_L_NL, maxClassNo(p));
  cp += NUM_OF_PATS_STR_MAXLEN + SIZEOF_NL;
  sprintf(cp, "\n");
  cp += SIZEOF_NL;

  sprintf(cp, "Inputs:\n"); cp += 7 + SIZEOF_NL;
  cp = printStatVals(cp, inputs(p));
  if(hasOutputs(p))
  { sprintf(cp, "Outputs:\n"); cp += 8 + SIZEOF_NL;
    cp = printStatVals(cp, outputs(p));
  }

  return buf;
}         /* statString */


/********************************************/
/********************************************/
static char *printStatVals(char *cp, VecColl vc)
{
  sprintf(cp, "Overall minimum:     ");
  cp += ROW_TITLE_LEN;
  sprintf(cp, NUMBER_FORMAT_L_NL, overallMin(vc));
  cp += NUMBER_STR_LENGTH + SIZEOF_NL;
  sprintf(cp, "Overall maximum:     ");
  cp += ROW_TITLE_LEN;
  sprintf(cp, NUMBER_FORMAT_L_NL, overallMax(vc));
  cp += NUMBER_STR_LENGTH + SIZEOF_NL;
  sprintf(cp, "Overall average:     ");
  cp += ROW_TITLE_LEN;
  sprintf(cp, NUMBER_FORMAT_L_NL, overallAvg(vc));
  cp += NUMBER_STR_LENGTH + SIZEOF_NL;
  sprintf(cp, "Overall std.dev.:    ");
  cp += ROW_TITLE_LEN;
  sprintf(cp, NUMBER_FORMAT_L_NL, overallStddev(vc));
  cp += NUMBER_STR_LENGTH + SIZEOF_NL;
  sprintf(cp, "\n");
  cp += SIZEOF_NL;

  return cp;
}         /* printStatVals */


/********************************************/
/* Schreibe alle Symbole (ohne Duplikate)   */
/* ins File f ab der aktuellen Fileposition.*/
/* Setze Variable error.                    */
/********************************************/
void writeSymtab(Patterns p, FILE *f)
{ long i, nvecs, diffVecs, nclassNos;
  Collection set;
  Vector v;

  if(hasClassNames(p))
  { fprintSymbols(p->symtab, f);
    if(error) return;
  }
  else              /* schreibe die Output-Vektoren als Symbole */
  {
    if(hasOutputs(p))
    { if(! (set = newColl())) error(1);
      nvecs = size(p->outputs);
      for(i = 1L; i <= nvecs; i++)
      { v = (Vector) at(p->outputs, i);
        if(detectPos(set, v, veq) == -1L)
          if(! add(set, v))
          { freeColl(set);
            error(1);
          }
      }     /* for */
      diffVecs = size(set);
      for(i = 1L; i <= diffVecs; i++)
      { if(fprintVector((Vector) at(set, i), f) != 0)
        { freeColl(set);
          error(15);
        }
        fprintf(f, "\n");
      }     /* for */
      freeColl(set);
    }       /* if(hasOutputs(p)) */
    else    /* schreibe die Klassen-Nummern */
    { for(i = 1L; i <= p->classCount; i++)
        fprintf(f, "%ld\n", i);       
    }
  }         /* else */
  
  error = 0;
}           /* writeSymtab */


/********************************************/
/* Antworte die Dimension der Input Vektoren*/
/* von p.                                   */
/********************************************/
long inputDims(Patterns p)
{
  return p->inputDims;
}          /* inputDims */


/********************************************/
/* Antworte die Dimension der Output        */
/* Vektoren von p.                          */
/********************************************/
long outputDims(Patterns p)
{
  return p->outputDims;
}          /* outputDims */


/********************************************/
/* Generiere Klassen-Nummern fuer die Aus-  */
/* gabevektoren von p. Gleiche Ausgabe-     */
/* vektoren bekommen gleiche Nummern.       */
/* Die Nummern beginnen bei 1 und werden    */
/* dann um je 1 hochgezaehlt, wenn ein Aus- */
/* gabevektor vorher noch nicht vorkam.     */
/* Existieren keine Ausgabevektoren, dann   */
/* werden die Klassennamen betrachtet.      */
/* Die Funktion setzt die Variablen         */
/* classNos und classCount.                 */
/* Variable error wird gesetzt              */
/********************************************/
void genClassNos(Patterns p)
{
  if(hasClassNames(p))
  {
    genClassNosFromNames(p);
    if(error) return;
  }
  else if(hasOutputs(p))
  {
    genClassNosFromVectors(p);
    if(error) return;
  }

  error = 0;
}          /* genClassNos */


/**************************************************/
/* Generiere Klassen-Nummern anhand der Klassen   */
/* Namen der Patterns p und setze die Variablen   */
/* classNos und classCount entsprechend.          */
/* Variable error wird gesetzt.                   */
/**************************************************/
void genClassNosFromNames(Patterns p)
{ long *classNo, numClasses, i;
  Collection namesOrder, clsnos;

  namesOrder = sequence(p->symtab);
  freeCollAll(classNos(p));
  if(! (p->classNos = newColl())) error(1);

  clsnos = p->classNos;
  numClasses = size(classes(p));
  for(i = 1L; i <= numClasses; i++)
  {
    if(! (classNo = (long *) malloc(sizeof(*classNo)))) error(1);
    *classNo = indexOf(namesOrder, at(classes(p), i));
    if(! add(clsnos, classNo)) error(1);
  }        /* for */

  p->classCount = size(namesOrder);
  error = 0;
}          /* genClassNosFromNames */


/**************************************************/
/* Generiere Klassen-Nummern anhand der Ausgabe-  */
/* Vektoren von p und setze die Variablen         */
/* classNos und classCount entsprechend.          */
/* Variable error wird gesetzt.                   */
/**************************************************/
void genClassNosFromVectors(Patterns p)
{ long *classNo, numVectors, i, noCount = 0L;
  Vector v;
  Collection map;

  if(! (map = newColl())) error(1);
  freeCollAll(classNos(p));
  if(! (p->classNos = newColl())) error(1);

  numVectors = size(outputs(p));
  for(i = 1L; i <= numVectors; i++)
  {
    if(! (classNo = (long *) malloc(sizeof(*classNo)))) error(1);
    v = (Vector) at(outputs(p), i);
    *classNo = classNoOfVec(map, v);
    if(*classNo == -1L)
    {                 /* Vektor v tritt an Position i zum ersten Mal auf */
      *classNo = ++noCount;
      addMapping(map, v, *classNo);
      if(error) return;
    }     /* if */
    if(! add(classNos(p), classNo)) error(1);
  }       /* for */

  freeCollAll(map);             /* auch Speicher der Mapping Strukturen */
  p->classCount = noCount;
  error = 0;
}         /* genClassNosFromVectors */


struct Mapping {                /* benoetigt von classNoOfVec()    */
  Vector vec;                   /* und addMapping().               */
  long no;
}; 
 
/****************************************************/
/* Antworte die Klassen-Nummer des Ausgabevektors v.*/
/* Ist v in coll noch keine Klasse zugeordnet, so   */
/* antworte -1L.                                    */
/****************************************************/
static long classNoOfVec(Collection coll, Vector v)
{ long index;
  struct Mapping *m;

  for(index = 1L; index <= size(coll); index++)
  {
    m = (struct Mapping *) at(coll, index);
    if(vcmp(v, m->vec) == V_EQUAL) return m->no;
  }
  return -1L;
}        /* classNoOfVec */


/****************************************************/
/* Fuege der Collection coll die Abbildung des      */
/* Vektors v nach Klassen-Nummer classNo hinzu.     */
/* Variable error wird gesetzt.                     */
/****************************************************/
static void addMapping(Collection coll, Vector v, long classNo)
{ struct Mapping *m;

  m = (struct Mapping *) malloc(sizeof(*m));
  if(m == NULL) error(1);
  m->vec = v;
  m->no = classNo;
  if(! add(coll, m)) error(1);
}        /* addMapping */


/******************************************************/
/* Ersetze die Klassen-Namen von p durch die Symbole  */
/* der Symtab st. st muss mindestens so viele unter-  */
/* schiedl. Symbole enthalten wie die groesste        */
/* Klassen-Nummer von p.                              */
/* Variable error wird gesetzt.                       */
/******************************************************/
void replaceClasses(Patterns p, Symtab st)
{ long i, numClasses;
  Collection newClassColl, symbols;

  if(! (newClassColl = newColl())) error(1);
  symbols = sequence(st);
  numClasses = size(classNos(p));
  for(i = 1L; i <= numClasses; i++)
  {
    if(! add(newClassColl,
      (char *) at(symbols, * (long *) (at(p->classNos, i)))))
    { freeColl(newClassColl);
      error(1);
    }
  }     /* for */

  freeSymtab(p->symtab);
  freeColl(p->classes);
  p->symtab = st;
  p->classes = newClassColl;
  error = 0;
}              /* replaceClasses */


/**********************************************************/
/* Erweiter alle Input-Vektoren von p um die zugehoerigen */
/* Klassenvektoren, falls flag == TRUE. Ist flag == FALSE */
/* so werden die Output-Vektoren erweitert.               */
/* Der Klassenvektor besteht aus classCount               */
/* Elementen und hat in der i-ten Dimension eine 1 stehen */
/* wenn der entsprechende Input-Vektor die Klassennummer  */
/* i besitzt. Alle anderen Elemente sind 0.               */
/* Variable error wird gesetzt.                           */
/**********************************************************/
void expandWithClassVectors(Patterns p, Boolean flag)
{ VecColl vc;
  Vector v, expandedVec;
  long clsNo, i, numVectors;

  vc = (flag ? inputs(p) : outputs(p));
  if(! (v = newVector(p->classCount))) error(1);
  putAllDim(v, 0);

  numVectors = size(vc);
  for(i = 1L; i <= numVectors; i++)
  {
    clsNo = * (long *) at(p->classNos, i);
    putDim(v, clsNo, 1);
    if(! (expandedVec = expand((Vector) at(vc, i), v))) error(1);
    put(vc, i, expandedVec);
    putDim(v, clsNo, 0);
  }

  if(flag) p->inputDims += p->classCount;
  else p->outputDims += p->classCount;
  freeVector(v);
  error = 0;
}               /* expandWithClassVectors */


/***************************************************/
/* Erweiter alle Eingabevektoren von p um die      */
/* dazugehoerigen Ausgabevektoren.                 */
/* Variable error wird gesetzt.                    */
/***************************************************/
void expandWithOutputs(Patterns p)
{ long numVectors, i;
  Vector expandedVec;

  if(hasOutputs(p))
  {
    numVectors = size(p->inputs);
    for(i = 1L; i <= numVectors; i++)
    {
      if(! (expandedVec =
              expand((Vector) at(p->inputs, i), (Vector) at(p->outputs, i))))
        error(1);
      put(p->inputs, i, expandedVec);
    }
  }      /* if(hasOutputs(p)) */

  p->inputDims += p->outputDims;
  error = 0;
}               /* expandWithOutputs */







