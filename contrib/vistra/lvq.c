/*****************************************************/
/* Schreibe die Patterns p im LVQ-Format nach File f.*/
/* Variable error wird gesetzt.                      */
/*****************************************************/
void writeLVQ(Patterns p, FILE *f)
{ 
  lvqWrite(p, f, TRUE);
  if(error) return;

  error = 0;
}        /* writeLVQ */


/*****************************************************/
/* Lese das Pattern File f ein, das im LVQ-Format    */
/* vorliegt. Antworte die Patterns.                  */
/* Variable error wird gesetzt.                      */
/*****************************************************/
Patterns readLVQ(FILE *f)
{ Patterns answer;
  long i, vecCount;
  Number n;
  Vector v;
  char buf[CLASS_MAXLEN + 1], *copy;

  answer = newPatterns();
  if(error) return;

  rowCount = 1L;
  skipSpaceAndCountNl(f);
  if(fscanf(f, "%ld", &(answer->inputDims)) != 1)
  { strcpy(errorInfo, "Integer expected!");
    freePatterns(answer);
    error(5);
  }

  vecCount = 0L;
  while(! atEndNl(f))
  {
    /* lese Eingabevektor ein */
    if(! (v = newVector(answer->inputDims))) error(1);
    for(i = 1L; i <= answer->inputDims; i++)
    {
      if(fscanf(f, "%f", &n) != 1)
      { sprintf(errorInfo, "Floating number expected!");
        freePatterns(answer);
	error(5);
      }
      putDim(v, i, n);
      skipSpaceAndCountNl(f);
    }        /* for */
    if(! add(answer->inputs, v)) error(1);
    vecCount++;

    /* lese Klassen-Symbol ein */
    if(fscanf(f, CLASS_SCANF_FORMAT, buf) != 1)
    { strcpy(errorInfo, "Class name expected!");
      freePatterns(answer);
      error(5);
    }
    copy = addSymbol(answer->symtab, buf);
    if(error) return;
    if(! add(answer->classes, copy)) error(1);
  }          /* while */

  /* teste, ob mindestens 1 Eingabevektor gelesen wurde. */
  if(vecCount == 0L) 
  { strcpy(errorInfo, "No input pattern!");
    freePatterns(answer);
    error(5);
  }
 
  answer->count = vecCount;
  answer->classCount = (long) numSymbols(answer->symtab);
  answer->outputDims = 0L;
  genClassNosFromNames(answer);
  if(error) return;

  error = 0;
  return answer;
}        /* readLVQ */


/*****************************************************/
/* Lese die Vektoren des LVQ-Files f ein.            */
/* inVecs ist auf TRUE zu setzen, wenn es sich bei   */
/* den Vektoren um Eingabevektoren handelt, bei Aus- */
/* gabevektoren auf FALSE.                           */
/* Variable error wird gesetzt.                      */
/*****************************************************/
void lvqRead(Patterns p, FILE *f, Boolean inVecs)
{ VecColl newVecs;
  long i, vecCount, ndims;
  Number n;
  Vector v;
  char buf[CLASS_MAXLEN + 1], *copy;

  rowCount = 1L;
  skipSpaceAndCountNl(f);
  if(fscanf(f, "%ld", &ndims) != 1)
  { strcpy(errorInfo, "Integer expected!");
    error(5);
  }

  if(! (newVecs = newColl())) error(1);
  vecCount = 0L;
  while(! atEndNl(f))
  {
    /* lese Eingabevektor ein */
    if(! (v = newVector(ndims))) error(1);
    for(i = 1L; i <= ndims; i++)
    {
      if(fscanf(f, "%f", &n) != 1)
      { sprintf(errorInfo, "Floating number expected!");
        freeDeep(newVecs, freeVector);
	error(5);
      }
      putDim(v, i, n);
      skipSpaceAndCountNl(f);
    }        /* for */
    if(! add(newVecs, v)) error(1);
    vecCount++;

    /* lese Klassen-Symbol ein */
    if(fscanf(f, CLASS_SCANF_FORMAT, buf) != 1)
    { strcpy(errorInfo, "Class name expected!");
      freeDeep(newVecs, freeVector);
      error(5);
    }
  }          /* while */

  /* werfe die bisherigen Vektoren weg */
  if(inVecs) 
  { freeDeep(p->inputs, freeVector);
    p->inputs = newVecs;
    p->inputDims = ndims;
  }
  else 
  { freeDeep(p->outputs, freeVector);
    p->outputs = newVecs;
    p->outputDims = ndims;
  }

  error = 0;
}        /* lvqRead */


/************************************************************/
/* Falls inVecs TRUE ist, dann schreibe die Eingabevektoren */
/* von p im LVQ-Format nach f, sonst die Ausgabevektoren.   */
/* Variable error wird gesetzt.                             */
/************************************************************/
void lvqWrite(Patterns p, FILE *f, Boolean inVecs)
{ Boolean symbols;
  long i, nvecs;
  VecColl vc;

  vc = (inVecs ? p->inputs : p->outputs);
  symbols = hasClassNames(p);
  nvecs = size(vc);
  if(fprintf(f, "%ld\n", numberOfCols(vc)) < 0) error(7);
  for(i = 1L; i <= nvecs; i++)
  { if(fprintVector((Vector) at(vc, i), f)) error(7);
    if(fputc(' ', f) == EOF) error(7);
    if(symbols)
    { if(fprintf(f, "%s\n", (char *) at(p->classes, i)) < 0) error(7);
    }
    else
      if(fprintf(f, "%ld\n", *((long *) at(p->classNos, i))) < 0) error(7);
  }
}            /* lvqWrite */





