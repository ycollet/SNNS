static Boolean exitLoop;           /* fuers Interpretieren von Loops       */
static Boolean exitAlt;            /*   "         "         " Alternativen */

static Boolean matchDescList();               /* error gesetzt */
static Boolean matchDesc(enum Token, char *); /* error gesetzt */
static Boolean matchString(char *);
static Boolean matchAsterisk();
static Boolean matchQuestionMark();
static Boolean matchNewLine();
static Boolean matchLong(long);
static Boolean matchVector();
static Boolean matchVectorAll(long);          /* error gesetzt */
static Boolean matchClassName();
static Boolean my_eof(FILE *);
static void doDesc(enum Token);               /* error gesetzt */
static void doDescList();                     /* error gesetzt */
static void doAsterisk();
static void doQuestionMark();                 /* error gesetzt */
static void doString(char *);                 /* error gesetzt */
static void doNewLine();                      /* error gesetzt */
static long readLong(long);                   /* error gesetzt */
static Vector readVector(long);               /* error gesetzt */
static void readClassName(char *);            /* error gesetzt */

/********************************************************/
/* Lese das Pattern File f vom Format form ein und      */
/* speicher die Daten in pttrns.                        */
/* Die Syntax von form wird nicht ueberprueft.          */
/* Die Variable error wird gesetzt.                     */
/********************************************************/
void fileIn(Patterns pttrns, Format form, FILE *f)
{ enum Token firstDesc, descAfter;
  long curPos, beginningOfLoop, endOfLoop;
  Boolean matched, altDone;
  char saveVal[MAX_LENGTH_TOKENVAL];

  inDims = outDims = 0L;
  numPats = inCount = outCount = classCount = 0L;
  patternFile = f;
  format = form;
  pats = pttrns;
  rowCount = 1L;

  setPosition(format, 0L);

  tok = nextToken(format);
  while(tok != endOfString)
  {
    switch(tok)
    {
      case loopBegin:
	  beginningOfLoop = getPosition(format);
	  endOfLoop = posAfter(format, loopEnd);
          firstDesc = lookUpNextDesc(format, beginningOfLoop);
          if(firstDesc == aString) strcpy(saveVal, tokenval);
          exitLoop = FALSE;
          while(! exitLoop && ! my_eof(patternFile))
          {
            matched = matchDesc(firstDesc, saveVal);
            if(error) return;
            if(matched)
            { setPosition(format, beginningOfLoop);
              doDescList();
              if(error) return;
            }
            else
            { exitLoop = TRUE;
              setPosition(format, endOfLoop);
            }
          }             /* while */
          break;        /* case loopBegin */
      case altBegin:
          beginningOfLoop = getPosition(format);
          endOfLoop = posAfter(format, altEnd);
          descAfter = lookUpNextDesc(format, endOfLoop);
          if(descAfter == aString) strcpy(saveVal, tokenval);
          exitAlt = FALSE;
          do             /* wiederhole Rumpf */
          { if(my_eof(patternFile))
            { setPosition(format, endOfLoop);
              break;
            }
            matched = matchDesc(descAfter, saveVal);
            if(error) return;
            if(matched)
            { setPosition(format, endOfLoop);
              exitAlt = TRUE;
            }
            else          /* Alternativen probieren */
            {
              altDone = FALSE;
              do
              { matched = matchDescList();
                if(error) return;
                if(matched)
	        {
                  doDescList();
                  if(error) return;
                  /* tok enthaelt jetzt or oder altEnd */
                  altDone = TRUE;
                  setPosition(format, beginningOfLoop);
                }
                else       /* probiere naechste Alternative */
                {
                  while(tok != or && tok != altEnd)
                    tok = nextToken(format);
                  if(tok == altEnd)
                  { /* sprintf(errorInfo,
                      "No alternative could be applied!");
                    error(5); */
                    exitAlt = TRUE;
                    setPosition(format, endOfLoop);
                  }
                }       /* else */
	      } while(! altDone && ! exitAlt);
            }           /* else */
          } while(! exitAlt);
          break;
      default:             /* tok muss ein Deskriptor sein */
          doDesc(tok);
          if(error) return;
          break;
    }         /* switch */
    tok = nextToken(format);
  }       /* while */

  if(! atEnd(patternFile))
  { sprintf(errorInfo, "End of file expected!");
    error(5);
  }

  if(inCount == 0L)
  { strcpy(errorInfo, "No input pattern!");
    error(5);
  }

  if(numPats && inCount != numPats)
  { strcpy(errorInfo, "Less patterns read than specified!");
    error(5);
  }

  if(outCount == 0L && classCount == 0L)
  { strcpy(errorInfo, "Missing both output patterns and class names!");
    error(5);
  }

  if(outCount && outCount != inCount)
  { strcpy(errorInfo, "Different number of input and output patterns!");
    error(5);
  }

  if(classCount && classCount != inCount)
  { strcpy(errorInfo, "Different number of input patterns and class names!");
    error(5);
  }

  pats->inputDims = inDims;
  pats->outputDims = outDims;
  pats->count = inCount;
  genClassNos(pats);
  if(error) return;
  /* jetzt sind auch classNos und classCount initialisiert */

  error = 0;
}             /* fileIn */


/***************************************************/
/* Antworte TRUE gdw. alle Deskriptoren bis zum    */
/* naechsten Token, das kein Deskriptor ist,       */
/* mit den weiteren Zeichen des Pattern Files in   */
/* Einklang gebracht werden koennen. Der File      */
/* Pointer von patternFile wird genauso nicht      */
/* bewegt wie der Format Pointer.                  */
/* Setze Variable error.                           */
/***************************************************/
static Boolean matchDescList()
{ Boolean matched;
  long filePos, formatPos;

  filePos = ftell(patternFile);
  checkPos(filePos);
  formatPos = getPosition(format);
  tok = nextToken(format);

  while(isDesc(tok))
  { enum Token next;

    switch(tok)
    {
      case input:     next = lookUp(format);
		      if(isDesc(next))
                      { matched = matchVectorAll(inDims);
		        if(error) return FALSE;
                      }
                      else matched = matchVector();
                      break;
      case output:    next = lookUp(format);
		      if(isDesc(next))
                      { matched = matchVectorAll(outDims);
			if(error) return FALSE;
                      }
                      else matched = matchVector();
		      break;
      case outputDim: matched = matchLong(0L);   /* Minimum: 0 */
                      break;
      case inputDim:
      case numPatterns:
                      matched = matchLong(1L);   /* Minimum: 1 */
                      break;
      case asterisk:  next = lookUp(format);
		      matched = TRUE;
                      if(isDesc(next)) doAsterisk();
		      break;
      case questionMark:
      case class:     next = lookUp(format);
		      if(isDesc(next))
                      { char c;

                        skipWhiteSpace(patternFile);
		        if(fgetc(patternFile) == EOF) matched = FALSE;
                        else
			{ if(fseek(patternFile, -1L, 1)) error(6);
			  matched = TRUE;
                          fscanf(patternFile, "%*s");
                        }
                      }
                      else matched = matchQuestionMark();
                      break;
      case newLine:   matched = matchNewLine();
                      break;
      case writeNewLine:   /* beim Lesen nicht beachten */
                      matched = TRUE;
                      break;
      case aString:   matched = matchString(tokenval);
                      break;
    }         /* switch */

    if(! matched)
    { setPos(patternFile, filePos);
      error = 0;
      return FALSE;
    }
    tok = nextToken(format);
  }         /* while */

  setPos(patternFile, filePos);
  setPosition(format, formatPos);
  error = 0;
  return TRUE;
}          /* matchDescList */


/***************************************************/
/* Antworte mit TRUE gdw. ein String von           */
/* patternFile eingelesen werden koennte, das mit  */
/* dem Token                                       */
/* desc in Einklang gebracht werden kann.          */
/* Der File Pointer von patternFile wird nicht     */
/* bewegt. Setze Variable error.                   */
/***************************************************/
static Boolean matchDesc(enum Token desc, char *tokvalue)
{ Boolean answer;
  long filePos;

  filePos = ftell(patternFile);
  checkPos(filePos);

  switch(desc)
  {
    case outputDim:   answer = matchLong(0L);
                      break;
    case inputDim:
    case numPatterns: answer = matchLong(1L);
                      break;
    case input:
    case output:      answer = matchVector();
                      break;
    case class:       answer = matchClassName();
                      break;
    case newLine:     answer = matchNewLine();
                      break;
    case writeNewLine:
    case asterisk:    answer = TRUE;
                      break;
    case questionMark: answer = matchQuestionMark();
                      break;
    case aString:     answer = matchString(tokvalue);
                      break;
    case endOfString: answer = atEnd(patternFile);
                      break;
  }      /* switch */

  setPos(patternFile, filePos);

  error = 0;
  return answer;
}               /* matchDesc */


/********************************************************/
/* Teste, ob der String str vom Pattern File            */
/* eingelesen werden kann. Ein oder mehrere white space */
/* Characters in str entsprechen einer beliebigen       */
/* Anzahl von white space Charactern des Pattern        */
/* Files.                                               */
/********************************************************/
static Boolean matchString(char *str)
{ char *cp;
  char c;

  cp = str;
  skipWhiteSpace(patternFile);              /* overread white space */
  while(isspace(*cp)) cp++;
  while(*cp)
  {
    if(isspace(*cp))
    {
      while(isspace(*(++cp)));
      skipWhiteSpace(patternFile);
    }
    else
    { c = fgetc(patternFile);
      if(c != *cp) return FALSE;
      cp++;
    }
  }       /* while */
  /* end of str reached */

  return TRUE;
}            /* matchString */


/************************************************/
/* Ueberlese den naechsten String im Pattern    */
/* File. Antworte mit TRUE gdw. ein solcher     */
/* existiert bevor EOF erreicht ist.            */
/************************************************/
static Boolean matchQuestionMark()
{
  return ! atEnd(patternFile);
}          /* matchQuestionMark */


/*************************************************/
/* Antworte TRUE gdw. als naechstes ein newline  */
/* Character kommt.                              */
/*************************************************/
static Boolean matchNewLine()
{ char c;

  c = fgetc(patternFile);
  while(c == ' ' || c == '\t') c = fgetc(patternFile);
  return c == '\n';
}               /* matchNewLine */


/*************************************************/
/* Antworte mit TRUE gdw. eine Integer Zahl      */
/* groesser oder gleich min vom Pattern File     */
/* gelesen werden konnte.                        */
/*************************************************/
static Boolean matchLong(long min)
{ long value;
  int result;

  result = fscanf(patternFile, "%ld", &value);
  if(result != 1) return FALSE;
  return value >= min;
}               /* matchLong */


/***********************************************/
/* Antworte TRUE gdw. im Pattern File als      */
/* naechstes eine Float Zahl kommt.            */
/* Setze Variable error.                       */
/***********************************************/
static Boolean matchVector()
{ int dummy;

  return fscanf(patternFile, "%f", &dummy) == 1;
}              /* matchVector */


/***********************************************/
/* Antworte TRUE gdw. dim Floats im Pattern    */
/* File als naechstes kommen, falls dim > 0.   */
/* Falls dim == 0 ist, dann antworte TRUE gdw. */
/* mindestens ein Float kommt. Der File Pointer*/
/* wird in diesem Fall genau hinter die letzte */
/* Floating Point Zahl gesetzt.                */
/* Setze Variable error.                       */
/***********************************************/
static Boolean matchVectorAll(long dim)
{ int dummy;
  long pos;          /* store file positions */
  int result;
  long count, i;

  if(dim)
  {
    for(i = 0L; i < dim; i++)
      if(fscanf(patternFile, "%f", &dummy) != 1)
      { error = 0;
	return FALSE;
      }
  }
  else               /* input dimension is unknown so far */
  {
    count = 0L;
    do
    { pos = ftell(patternFile);
      checkPos(pos);

      result = fscanf(patternFile, "%f", &dummy);
      if(result != 1) setPos(patternFile, pos)
      else count++;
    } while(result == 1);

    if(count == 0L)
    { error = 0;
      return FALSE;
    }
  }             /* else */

  error = 0;
  return TRUE;
}               /* matchVectorAll */


/*************************************************/
/* Lese das naechste Wort vom Pattern File und   */
/* antworte TRUE gdw. ein solches ueberhaupt     */
/* noch kommt.                                   */
/*************************************************/
static Boolean matchClassName()
{
  return matchQuestionMark();
}             /* matchClassName */


/***********************************************/
/* Fuehre semantische Aktionen fuer alle       */
/* Deskriptoren aus bis zum naechsten Token,   */
/* das kein Deskriptor ist.                    */
/* Setze Variable error.                       */
/***********************************************/
static void doDescList()
{
  tok = nextToken(format);
  while(isDesc(tok))
  {
    doDesc(tok);
    if(error) return;
    tok = nextToken(format);
  }     /* while */

  error = 0;
}            /* doDescList */


/***********************************************/
/* Fuehre semantische Aktionen des Interpreters*/
/* aus. Setze Variable error.                  */
/***********************************************/
static void doDesc(enum Token tk)
{ long l;
  Vector vec;
  static char buffer[CLASS_MAXLEN + 1];
  char *copy;

  switch(tk)
  {
    case inputDim:   l = readLong(1);
                     if(error) return;
                     if(inDims && l != inDims)
                     { error = 5;
                       sprintf(errorInfo, "Input dimension declared twice or does not conform to the size of input vectors!");
                       return;
                     }
                     inDims = l;
                     exitLoop = TRUE;
                     break;
    case outputDim:  l = readLong(0);
                     if(error) return;
                     if(outDims && l != outDims)
                     { error = 5;
	               sprintf(errorInfo, "Output dimension declared twice or does not conform to the size of output vectors!");
                       return;
                     }
                     outDims = l;
                     exitLoop = TRUE;
                     break;
    case numPatterns: l = readLong(1);
                     if(error) return;
                     if(numPats && numPats != l)
                     { error = 5;
                       sprintf(errorInfo, "Number of patterns already specified!");
                       return;
                     }
                     numPats = l;
		     if(inCount > numPats || outCount > numPats
		        || classCount > numPats)
                     { error = 5;
		       sprintf(errorInfo, "More patterns than specified!");
                       return;
		     }
                     exitLoop = TRUE;
                     break;
    case input:      vec = readVector(inDims);
                     if(error) return;
                     inCount++;
		     if(numPats && numPats < inCount)
		     { error = 5;
                       sprintf(errorInfo, "More input patterns than specified!");
                       return;
	             }
		     if(! add(pats->inputs, vec)) error(1);
		     if(! inDims) inDims = dims(vec);
		     if(inCount == numPats) exitLoop = TRUE;
                     break;
    case output:     vec = readVector(outDims);
                     if(error) return;
                     outCount++;
                     if(numPats && numPats < outCount)
                     { error = 5;
                       sprintf(errorInfo, "More output patterns than specified!");
                       return;
                     }
		     if(! add(pats->outputs, vec)) error(1);
                     if(outDims == 0L) outDims = dims(vec);
                     if(outCount == numPats) exitLoop = TRUE;
                     break;
    case class:      readClassName(buffer);
                     if(error) return;
                     classCount++;
                     if(numPats && numPats < classCount)
                     { error = 5;
                       sprintf(errorInfo, "More class names than specified!");
		       return;
                     }
                     copy = addSymbol(pats->symtab, buffer);
                     if(error) return;
		     if(! add(pats->classes, copy)) error(1);
                     if(classCount == numPats) exitLoop = TRUE;
                     break;
    case aString:    doString(tokenval);
                     if(error) return;
                     break;
    case asterisk:   doAsterisk();
                     break;
    case questionMark: doQuestionMark();
                     if(error) return;
                     break;
    case newLine:    doNewLine();
                     if(error) return;
		     break;
    case writeNewLine:                     /* do nothing */
                     break;
  }          /* switch */

  error = 0;
}             /* doDesc */


/********************************************************/
/* Lese einen String vom Format str vom Pattern File    */
/* ein. Ein oder mehrere white space                    */
/* Characters in str entsprechen einer beliebigen       */
/* Anzahl von white space Charactern des Pattern        */
/* Files. Erhoehe rowCount um die Anzahl gelesener      */
/* Newlines. Setze Variable error.                      */
/********************************************************/
static void doString(char *str)
{ char *cp;
  char c;

  cp = str;
  skipSpaceAndCountNl(patternFile);              /* overread white space */
  while(isspace(*cp)) cp++;

  while(*cp)
  {
    if(isspace(*cp))
    {
      while(isspace(*(++cp)));
      if(*cp) skipSpaceAndCountNl(patternFile);
    }
    else
    { c = fgetc(patternFile);
      if(c != *cp)
      { error = 5;
        sprintf(errorInfo, "Character '%c' expected!", *cp);
        return;
      }
      cp++;
    }
  }       /* while */
  /* end of str reached */
  error = 0;
}            /* doString */


/************************************************/
/* Bewege den File Pointer von patternFile      */
/* genau hinter das naechste Newline Zeichen    */
/* bzw. ans Ende des Files. Erhoehe rowCount    */
/* im ersten Fall um 1.                         */
/************************************************/
static void doAsterisk()
{ char c;

  do
  { if((c = fgetc(patternFile)) == '\n') rowCount++;
  } while(c != EOF && c != '\n');
}          /* doAsterisk */


/************************************************/
/* Ueberlese den naechsten String im Pattern    */
/* File und setze Variable error. Erhoehe       */
/* rowCount um die Anzahl gelesener Newlines.   */
/************************************************/
static void doQuestionMark()
{ char c;

  skipSpaceAndCountNl(patternFile);
  if(fgetc(patternFile) == EOF)
  { error = 5;
    sprintf(errorInfo, "Any word expected!");
    return;
  }

  if(fseek(patternFile, -1L, 1)) error(6);
  fscanf(patternFile, "%*s");

  error = 0;
}           /* doQuestionMark */


/**************************************************/
/* Bewege den File Pointer von patternFile genau  */
/* hinter das naechste Newline bzw. EOF Zeichen.  */
/* Setze Variable error ungleich 0 gdw. davor     */
/* ein Zeichen kommt, das kein Tab und kein Blank */
/* ist. Erhoehe rowCount um 1, falls hinter einem */
/* Newline stehengeblieben wird.                  */
/**************************************************/
static void doNewLine()
{ char c;

  do
  { if((c = fgetc(patternFile)) == '\n') rowCount++;
    if(!isspace(c) && c != EOF)
    { error = 5;
      sprintf(errorInfo, "End of line expected!");
      return;
    }
  } while(c == ' ' || c == '\t');

  error = 0;
}               /* doNewLine */


/*************************************************/
/* Lese eine Integerzahl vom Pattern File, die   */
/* groesser oder gleich min ist.                 */
/* Erhoehe dabei rowCount um die Anzahl          */
/* vorher gelesener Newlines.                    */
/* Setze Variable error.                         */
/*************************************************/
static long readLong(long min)
{ long value;
  int result;

  skipSpaceAndCountNl(patternFile);
  result = fscanf(patternFile, "%ld", &value);
  if(result != 1)
  { error = 5;
    sprintf(errorInfo, "Integer expected!");
    return 0L;
  }
  if(value < min)
  { error = 5;
    sprintf(errorInfo, "Number must be %ld at minimum!", min);
    return 0L;
  }

  error = 0;
  return value;
}               /* readLong */


/***********************************************/
/* Lese dim floats vom Pattern File ein,       */
/* falls dim > 0. Ansonsten lese alle          */
/* aufeinanderfolgenden floats ein bis etwas   */
/* anderes kommt. Setze Variable error.        */
/***********************************************/
static Vector readVector(long dim)
{ long i;
  Vector answer;
  long pos;               /* store file positions */
  int result;
  Number *np, n;

  if(dim)                 /* dimension already known */
  {
    if(! (answer = newVector(dim))) error(1);

    for(i = 1L; i <= dim; i++)
    {
      skipSpaceAndCountNl(patternFile);
      if(fscanf(patternFile, "%f", &n) != 1)
      { error = 5;
        sprintf(errorInfo, "Floating number expected!");
	return NULL;
      }
      putDim(answer, i, n);
    }        /* for */
  }      /* if */
  else               /* input dimension is unknown by now */
  { Collection coll;       /* can dynamically grow in contrast to answer */

    if(! (coll = newColl())) error(1);
    do
    { long saveRowCount = rowCount;

      if(! (np = (Number *) malloc(sizeof(*np)))) error(1);
      pos = ftell(patternFile);
      checkPos(pos);
      skipSpaceAndCountNl(patternFile);
      if((result = fscanf(patternFile, "%f", np)) == 1)
      { if(! add(coll, np)) error(1);
      }
      else
      { /* setze File Pointer zurueck; hinter die zuletzt gelesene Number */
        setPos(patternFile, pos);
        rowCount = saveRowCount;
      }
    } while(result == 1);

    if(size(coll) == 0L)
    { error = 5;
      sprintf(errorInfo, "Vector expected!");
      freeColl(coll);
      return NULL;
    }
    else
    { if(! (answer = newVector(size(coll)))) error(1);
      copyFromColl(answer, coll);
      freeCollAll(coll);
    }    /* else */
  }      /* else */

  error = 0;
  return answer;
}               /* readVector */


/*************************************************/
/* Lese das naechste Wort vom Pattern File in    */
/* den Speicherbereich, auf den buf zeigt.       */
/* Setze Variable error.                         */
/*************************************************/
static void readClassName(char *buf)
{ int result;

  skipSpaceAndCountNl(patternFile);
  result = fscanf(patternFile, CLASS_SCANF_FORMAT, buf);
  if(result != 1)
  { error = 5;
    sprintf(errorInfo, "Class name expected!");
    return;
  }
  error = 0;
}               /* readClassName */


/*************************************************/
/*************************************************/
static Boolean my_eof(FILE *f)
{ if(fgetc(f) == EOF) return TRUE;
  fseek(f, -1L, 1);
  return FALSE;
}              /* my_eof */
