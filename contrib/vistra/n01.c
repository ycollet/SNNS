/******************************************************/
/******************************************************/
/******* Routinen zum Lesen und Schreiben von *********/
/*******        N 0 1 - F o r m a t e n       *********/
/******************************************************/
/******************************************************/

#define N01Number   float
#define N01Class    unsigned

typedef struct { unsigned InputDimension;
                 unsigned OutputDimension;
                 unsigned NumberOfData;
                 unsigned PlaceOfInputData;
                 unsigned PlaceOfOutputData;
                 unsigned PlaceOfClassData;
                 unsigned NumberOfClasses;
               } N01Type;

static void checkN01Header(N01Type *);             /* error gesetzt */
static N01Number reverseNumber(N01Number);
static unsigned reverseUnsigned(unsigned);
static void writeN01_SUN(Patterns, FILE *);        /* error gesetzt */
static void writeN01_DEC(Patterns, FILE *);        /* error gesetzt */

/**********************************************************/
/* Lese das File f ein, das im N01-Format vorliegt.       */
/* Antworte die gelesenen Vektoren und Klassen in Form    */
/* eines Patterns-Typs.                                   */
/* Variable error wird gesetzt.                           */
/**********************************************************/
Patterns readN01(FILE *f)
{ N01Type header;
  Patterns p;
  Vector v;
  long i, j;
  char n01Tag[4];

  if(fread(n01Tag, sizeof(char), 4, f) != 4) error(16);

  if(fread(&header, sizeof(header), 1, f) != 1) error(16);
  if(! isDEC)
  { header.NumberOfData = reverseUnsigned(header.NumberOfData);
    header.PlaceOfInputData = reverseUnsigned(header.PlaceOfInputData);
    header.PlaceOfOutputData = reverseUnsigned(header.PlaceOfOutputData);
    header.PlaceOfClassData = reverseUnsigned(header.PlaceOfClassData);
    header.NumberOfClasses = reverseUnsigned(header.NumberOfClasses);
    header.InputDimension = reverseUnsigned(header.InputDimension);
    header.OutputDimension = reverseUnsigned(header.OutputDimension);
  }

  /* Ueberpruefe den Inhalt des Headers */
  checkN01Header(&header);
  if(error) return NULL;

  p = newPatterns();
  if(error) return NULL;
  p->inputDims = header.InputDimension;
  p->outputDims = header.OutputDimension;
  p->count = header.NumberOfData;
  p->classCount = header.NumberOfClasses;

  /* Lese die Input Patterns */
  if(fseek(f, header.PlaceOfInputData, 0)) error(16);
  for(i = 1L; i <= header.NumberOfData; i++)
  {
    if(! (v = newVector(header.InputDimension))) error(1);
    if(fread(v->elements, sizeof(N01Number), header.InputDimension, f)
        != header.InputDimension) error(16);
    if(! isDEC)
      for(j = 0L; j < header.InputDimension; j++)
        v->elements[j] = reverseNumber(v->elements[j]);
    if(! add(inputs(p), v)) error(1);
  }

  /* Lese die Output Patterns */
  if(header.PlaceOfOutputData)
  {                   /* Output Vektoren vorhanden */
    if(fseek(f, header.PlaceOfOutputData, 0)) error(16);
    for(i = 1L; i <= header.NumberOfData; i++)
    {
      if(! (v = newVector(header.OutputDimension))) error(1);
      if(fread(v->elements, sizeof(N01Number), header.OutputDimension, f)
          != header.OutputDimension) error(16);
      if(! isDEC)
        for(j = 0L; j < header.OutputDimension; j++)
          v->elements[j] = reverseNumber(v->elements[j]);
      if(! add(outputs(p), v)) error(1);
    }    /* for */
  }      /* if */

  /* Lese die Klassen-Nummern */
  if(header.PlaceOfClassData)
  {                   /* Klassen-Nummern vorhanden */
    if(fseek(f, header.PlaceOfClassData, 0)) error(16);
    for(i = 1L; i <= header.NumberOfData; i++)
    { long *classNo;
      N01Class cls;

      classNo = (long *) malloc(sizeof(*classNo));
      if(! classNo) error(1);
      if(fread(&cls, sizeof(cls), 1, f) != 1) error(16);
      /* kein Fehler */
      if(! isDEC) cls = reverseUnsigned(cls);
      *classNo = (long) cls;
      if(! add(classNos(p), classNo)) error(1);
    }    /* for */
  }      /* if */

  error = 0;
  return p;
}          /* readN01 */


/*****************************************************/
/* Ueberpruefe den N01 Header hd auf Plausibilitaet. */
/* Setze Variable error entsprechend.                */
/*****************************************************/
static void checkN01Header(N01Type *hd)
{
  if(hd->InputDimension == 0)
  { strcpy(errorInfo, "Input dimension of 0 specified!");
    error(18);
  }
  if(hd->NumberOfData == 0)
  { strcpy(errorInfo, "0 specified for the number of pattern!");
    error(18);
  }
  if(! hd->PlaceOfOutputData && ! hd->PlaceOfClassData)
  { strcpy(errorInfo, "Both output and class data is missing!");
    error(18); 
  }

  error = 0;
}          /* checkN01Header */


/*******************************************************/
/* Schreibe die Patterns p im N01-Format nach File f   */
/* ab der aktuellen File Position von f.               */
/* Variable error wird gesetzt.                        */
/*******************************************************/
void writeN01(Patterns p, FILE *f)
{ 
  if(isDEC) writeN01_DEC(p, f);
  else writeN01_SUN(p, f);
}       /* writeN01 */ 
 
  
/*******************************************************/
/*******************************************************/
static void writeN01_SUN(Patterns p, FILE *f)
{ N01Type header;
  long i, j, nums;
  N01Number *elems;
  Vector v;
  Collection clsnos = classNos(p);
  char n01Tag[] = "N01";
  unsigned offsetAfterInputs, offsetAfterOutputs;

  /* schreibe die N01-Kennung */
  if(fwrite(n01Tag, sizeof(char), 4, f) != 4) error(17);

  /* schreibe den Header */
  header.InputDimension = (unsigned) inputDims(p);
  header.OutputDimension = (unsigned) outputDims(p);
  header.NumberOfData = (unsigned) num(p);
  header.NumberOfClasses = (unsigned) maxClassNo(p);
  header.PlaceOfInputData = sizeof(header) + 4;
  offsetAfterInputs = header.PlaceOfInputData + 
                     num(p) * header.InputDimension * sizeof(N01Number);
  if(hasOutputs(p))
  { header.PlaceOfOutputData = offsetAfterInputs;
    offsetAfterOutputs = header.PlaceOfOutputData + 
        num(p) * header.OutputDimension * sizeof(N01Number);
  }
  else
  { header.PlaceOfOutputData = 0;
    offsetAfterOutputs = offsetAfterInputs;
  }
  header.PlaceOfClassData = (isEmpty(clsnos) ? 0 : offsetAfterOutputs);

  header.NumberOfData = reverseUnsigned(header.NumberOfData);
  header.PlaceOfInputData = reverseUnsigned(header.PlaceOfInputData);
  header.PlaceOfOutputData = reverseUnsigned(header.PlaceOfOutputData);
  header.PlaceOfClassData = reverseUnsigned(header.PlaceOfClassData);
  header.NumberOfClasses = reverseUnsigned(header.NumberOfClasses);
  header.InputDimension = reverseUnsigned(header.InputDimension);
  header.OutputDimension = reverseUnsigned(header.OutputDimension);
  if(fwrite(&header, sizeof(header), 1, f) != 1) error(17);

  /* Schreibe die Input Vektoren */
  if(! (elems = (N01Number *) malloc(p->inputDims * sizeof(*elems))))
    error(1);
  nums = size(p->inputs);
  for(i = 1L; i <= nums; i++)
  { 
    v = (Vector) at(p->inputs, i);
    for(j = 0; j < p->inputDims; j++)
      elems[j] = reverseNumber(v->elements[j]);
    if(fwrite(elems, sizeof(N01Number), p->inputDims, f)
        != p->inputDims) error(17);
  }
  free(elems);

  /* Schreibe die Output Vektoren, falls welche vorhanden */
  if(hasOutputs(p))
  {
    if(! (elems = (N01Number *) malloc(p->outputDims * 
          sizeof(*elems)))) error(1);
    nums = size(p->outputs);
    for(i = 1L; i <= nums; i++)
    {
      v = (Vector) at(p->outputs, i);
      for(j = 0; j < p->outputDims; j++)
        elems[j] = reverseNumber(v->elements[j]);
      if(fwrite(elems, sizeof(Number), p->outputDims, f) != p->outputDims) 
        error(17);
    }
    free(elems);
  }        /* if */
  
  
  /* Schreibe die Klassen-Nummern */
  if(notEmpty(clsnos))
  { N01Class c;
    nums = size(clsnos);
    for(i = 1L; i <= nums; i++)
    {
      c = (N01Class) *((long *) at(clsnos, i));
      c = reverseUnsigned(c);
      if(fwrite(&c, sizeof(c), 1, f) != 1)
        error(17);
    }      /* for */
  }        /* if */

  error = 0;
}          /* writeN01_SUN */


/*******************************************************/
/*******************************************************/
static void writeN01_DEC(Patterns p, FILE *f)
{ N01Type header;
  long i, nums;
  Vector v;
  Collection clsnos = classNos(p);
  char n01Tag[] = "N01";
  unsigned offsetAfterInputs, offsetAfterOutputs;

  /* schreibe die N01-Kennung */
  if(fwrite(n01Tag, sizeof(char), 4, f) != 4) error(17);

  /* schreibe den Header */
  header.InputDimension = (unsigned) inputDims(p);
  header.OutputDimension = (unsigned) outputDims(p);
  header.NumberOfData = (unsigned) num(p);
  header.NumberOfClasses = (unsigned) maxClassNo(p);
  header.PlaceOfInputData = sizeof(header) + 4;
  offsetAfterInputs = header.PlaceOfInputData + 
                     num(p) * header.InputDimension * sizeof(N01Number);
  if(hasOutputs(p))
  { header.PlaceOfOutputData = offsetAfterInputs;
    offsetAfterOutputs = header.PlaceOfOutputData + 
        num(p) * header.OutputDimension * sizeof(N01Number);
  }
  else
  { header.PlaceOfOutputData = 0;
    offsetAfterOutputs = offsetAfterInputs;
  }
  header.PlaceOfClassData = (isEmpty(clsnos) ? 0 : offsetAfterOutputs);
  if(fwrite(&header, sizeof(header), 1, f) != 1) error(17);

  /* Schreibe die Input Vektoren */
  nums = size(p->inputs);
  for(i = 1L; i <= nums; i++)
  { v = (Vector) at(p->inputs, i);
    if(fwrite(v->elements, sizeof(N01Number), header.InputDimension, f)
        != header.InputDimension) error(17);
  }

  /* Schreibe die Output Vektoren, falls welche vorhanden */
  if(hasOutputs(p))
  { nums = size(p->outputs);
    for(i = 1L; i <= nums; i++)
    { v = (Vector) at(p->outputs, i);
      if(fwrite(v->elements, sizeof(Number), header.OutputDimension, f)
        != header.OutputDimension) error(17);
    }
  }        /* if */
  
  
  /* Schreibe die Klassen-Nummern */
  if(notEmpty(clsnos))
  { N01Class c;
    nums = size(clsnos);
    for(i = 1L; i <= nums; i++)
    {
      c = (N01Class) *((long *) at(clsnos, i));
      if(fwrite(&c, sizeof(c), 1, f) != 1)
        error(17);
    }      /* for */
  }        /* if */

  error = 0;
}          /* writeN01_DEC */


/**************************************************/
/**************************************************/
static N01Number reverseNumber(N01Number n)
{ N01Number answer;
  char i, *bp1, *bp2;

  bp1 = (char *) (&n) + sizeof(n);
  bp2 = (char *) &answer;
  for(i = 0; i < sizeof(n); i++)
    *bp2++ = *(--bp1);

  return answer;
}          /* reverseNumber */


/**************************************************/
/**************************************************/
static unsigned reverseUnsigned(unsigned u)
{ unsigned answer;
  char i, *bp1, *bp2;

  bp1 = (char *) (&u) + sizeof(u);
  bp2 = (char *) &answer;
  for(i = 0; i < sizeof(u); i++)
    *bp2++ = *(--bp1);
  
  return answer;
}          /* reverseUnsigned */
