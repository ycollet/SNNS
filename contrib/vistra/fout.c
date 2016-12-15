static Boolean numPatsWritten, inDimsWritten, outDimsWritten;
                                   /* TRUE gdw. die entsprechenden Werte   */
                                   /* bereits geschrieben wurden.          */
static Collection patch;
  /* patch enthaelt die File-Positionen, an denen die Dimensionalitaet der */
  /* Ausgabevektoren ausgegeben wurde.                                     */
  /* An diesen File-Positionen muss eventuell eine 0 in eine 1 umgewandelt */
  /* werden, falls anstatt Ausgabevektoren die Klassennummern verwendet    */
  /* wurden.                                                               */
static Boolean classNosAsOutputs;
  /* TRUE gdw. eine Klassennummer anstelle eines Ausgabevektors (weil      */
  /* keine existieren) geschrieben wurde.                                  */
                                    

static Boolean writeNextList(Boolean);
static void writeDescList();
static void backpatch(FILE *, Collection, char);

/***************************************************/
/* Schreibe die Patterns pttrns nach File f im     */
/* Format form. Variable error wird gesetzt.       */
/***************************************************/
void fileOut(Patterns pttrns, Format form, FILE *f)
{ Boolean descListWritten, bodyExecuted;
  long beginningOfLoop, beginningOfAlt;

  pats = pttrns;
  patternFile = f;
  format = form;
  inCount = outCount = classCount = 0L;  /* count number of written objects */
  inDimsWritten = outDimsWritten = numPatsWritten = FALSE;
  classNosAsOutputs = FALSE;
  if(! (patch = newColl())) error(1);

  setPosition(format, 0L);
  tok = nextToken(format);
  while(tok != endOfString)
  {
    switch(tok)
    {
      case loopBegin: beginningOfLoop = getPosition(format);
                      bodyExecuted = FALSE;
                      while(writeNextList(bodyExecuted))
		      { /* writeNextList hat Format Pointer bewegt */
                        setPosition(format, beginningOfLoop);
			tok = nextToken(format);
			writeDescList();
                        if(error) { freeCollAll(patch); return; }
                        bodyExecuted = TRUE;
			/* writeDescList hat Format Pointer bewegt */
                        setPosition(format, beginningOfLoop);
		      }       /* while */
		      tok = nextToken(format);
                      break;
      case altBegin:  beginningOfLoop = getPosition(format);
                      bodyExecuted = FALSE;
                      do
                      { setPosition(format, beginningOfLoop);
                        descListWritten = FALSE;
                        do
                        { beginningOfAlt = getPosition(format);
                          if(writeNextList(bodyExecuted))
                          { setPosition(format, beginningOfAlt);
                            tok = nextToken(format);
                            writeDescList();
                            if(error) { freeCollAll(patch); return; }
                            descListWritten = TRUE;
                          }
                        } while(tok != altEnd);
                        bodyExecuted = TRUE;
                      } while(descListWritten);
                      tok = nextToken(format);
                      break;
      default:        writeDescList();
                      if(error) { freeCollAll(patch); return; }
		      break;
    }       /* switch */
  }         /* while */

  if(inCount != num(pats)) error(8); 
  if(outCount && outCount != size(pats->classNos)) error(9);
  if(classCount && classCount != size(pats->classNos)) error(10);

  error = 0;
}           /* fileOut */


/***************************************************/
/* Antworte TRUE gdw. die Liste der Deskriptoren   */
/* ab der aktuellen Position von format in die     */
/* Datei geschrieben werden kann. Der Format       */
/* Pointer von format wird dabei hinter das erste  */
/* Token bewegt, das kein Deskriptor ist.          */
/***************************************************/
static Boolean writeNextList(Boolean bodyExecuted)
{ long inputsInList = 0L, outputsInList = 0L, classesInList = 0L;
  Boolean descToWrite = FALSE;
  Boolean endLoop = FALSE;

  tok = nextToken(format);
  while(isDesc(tok) && ! endLoop)
  {
    switch(tok)
    {
      case input:       if(++inputsInList + inCount > size(pats->inputs))
			  endLoop = TRUE;
			else descToWrite = TRUE;
                        break;
      case output:      if(++outputsInList + outCount > size(pats->classNos))
                          endLoop = TRUE;
                        else descToWrite = TRUE;
                        break;
      case class:       if(++classesInList + classCount > size(pats->classNos))
                          endLoop = TRUE;
                        else descToWrite = TRUE;
                        break;
      case numPatterns: if(! numPatsWritten) descToWrite = TRUE;
                        break;
      case inputDim:    if(! inDimsWritten) descToWrite = TRUE;
                        break;
      case outputDim:   if(! outDimsWritten) descToWrite = TRUE;
                        break;
      case aString:     if(! bodyExecuted) descToWrite = TRUE;
                        break;
      default:          break;
    }          /* switch */

    tok = nextToken(format);
  }            /* while */

  if(endLoop)
  { while(isDesc(tok)) tok = nextToken(format);
    return FALSE;
  }
  else return descToWrite;
}              /* writeNextList */


/***************************************************/
/* Schreibe nacheinander alle Deskriptoren in die  */
/* Datei patternFile bis ein Token angetroffen     */
/* wird, das kein Deskriptor ist. Der Format       */
/* Pointer von format wird hinter dieses Token     */
/* bewegt. Variable error wird gesetzt.            */
/***************************************************/
static void writeDescList()
{ Vector vec;
  char *str;

  while(isDesc(tok))
  {
    switch(tok)
    {
      case input:        if(inCount < size(pats->inputs))
			 { vec = (Vector) at(pats->inputs, ++inCount);
                           if(fprintVector(vec, patternFile)) error(7);
			 }
                         if(fputc(' ', patternFile) == EOF) error(7);
                         break;
      case output:       if(outCount < size(pats->classNos))
                         { if(hasOutputs(pats))
			   { vec = (Vector) at(pats->outputs, ++outCount);
                             if(fprintVector(vec, patternFile)) error(7);
                           }
                           else       /* schreibe die Klassennummer */
			   { fprintf(patternFile, "%ld", 
                                 *(long *) at(pats->classNos, ++outCount));
                             if(! classNosAsOutputs && notEmpty(patch)) 
                             { backpatch(patternFile, patch, '1');
                               if(error) return;
                               freeFromTo(patch, 1L, size(patch), free);
                               classNosAsOutputs = TRUE;
                             }
                           }  
                         }
                         if(fputc(' ', patternFile) == EOF) error(7);
                         break;
      case class:        if(hasClassNames(pats) &&
                            classCount < size(pats->classes))
			 { if(fprintf(patternFile, "%s ",
                             (char *) at(pats->classes, ++classCount)) < 0)
                             error(7);
                         }
                         else
			   if(classCount < size(pats->classNos))
                             if(fprintf(patternFile, "%ld ",
                             *((long *) at(pats->classNos, ++classCount)))<0)
                             error(7);
                         break;
      case numPatterns:  if(fprintf(patternFile, "%ld ", num(pats)) < 0)
                           error(7);
                         numPatsWritten = TRUE;
                         break;
      case inputDim:     if(fprintf(patternFile, "%ld ", pats->inputDims) < 0)
                           error(7);
                         inDimsWritten = TRUE;
                         break;
      case outputDim:    { long toWrite, *lp;
                           if(! hasOutputs(pats) && ! classNosAsOutputs)
                           { if(! (lp = (long *) malloc(sizeof(*lp)))) 
                               error(1);
                             *lp = ftell(patternFile);
                             if(! add(patch, lp)) error(1); 
                           }
                           toWrite = (classNosAsOutputs ? 
                                              1L : pats->outputDims); 
                           if(fprintf(patternFile, "%ld ", toWrite) < 0)
                             error(7);
                           outDimsWritten = TRUE;
                         }
                         break;
      case writeNewLine:
      case asterisk:
      case newLine:      if(fprintf(patternFile, "\n") < 0) error(7);
                         break;
      case questionMark: if(fprintf(patternFile, "? ") < 0) error(7);
                         break;
      case aString:      if(fprintf(patternFile, "%s", tokenval) < 0) error(7);
                         break;
    }         /* switch */
    tok = nextToken(format);
  }           /* while */

  error = 0;
}             /* writeDescList */


/*************************************************************************/
/* Ersetze alle Zeichen der Datei f, die sich auf den durch positions ge-*/
/* gebenen (eine Collection von long-Zeigern) Positionen in f befinden,  */
/* durch das Zeichen patchChar.                                          */ 
/* Variable error wird gesetzt.                                          */
/*************************************************************************/
static void backpatch(FILE *f, Collection positions, char patchChar)
{ long initialPos, *pos, i, n = size(positions);
  
  initialPos = ftell(f);
  for(i = 1L; i <= n; i++)
  { pos = (long *) at(positions, i);
    if(fseek(f, *pos, 0) == -1) error(7);
    fputc(patchChar, f);
  }
  if(fseek(f, initialPos, 0) == -1) error(7);
  error = 0;
}             /* backpatch */













