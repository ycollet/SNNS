static Boolean exitLoop;           /* for interpreting loops               */
static Boolean exitAlt;            /*   "         "         alternatives   */

static Boolean matchDescList(void);               /* sets error */
static Boolean matchDesc(enum Token, char *); /* sets error */
static Boolean matchString(char *);
static Boolean matchAsterisk();
static Boolean matchQuestionMark(void);
static Boolean matchNewLine(void);
static Boolean matchLong(long);
static Boolean matchVector(void);
static Boolean matchVectorAll(long);          /* sets error */
static Boolean matchClassName(void);
static Boolean my_eof(FILE *);
static void doDesc(enum Token);               /* sets error */
static void doDescList(void);                     /* sets error */
static void doAsterisk(void);
static void doQuestionMark(void);                 /* sets error */
static void doString(char *);                 /* sets error */
static void doNewLine(void);                      /* sets error */
static long readLong(long);                   /* sets error */
static Vector readVector(long);               /* sets error */
static void readClassName(char *);            /* sets error */

/********************************************************/
/* Reads the pattern file f according to format form    */
/* and stores the data in pttrns.                       */
/* The syntax of form is not checked.                   */
/* Sets the global variable error.                      */
/********************************************************/
void fileIn(Patterns pttrns, Format form, FILE *f) {
    enum Token firstDesc, descAfter;
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
    while(tok != endOfString) {
        switch(tok) {
        case loopBegin:
            beginningOfLoop = getPosition(format);
            endOfLoop = posAfter(format, loopEnd);
            firstDesc = lookUpNextDesc(format, beginningOfLoop);
            if(firstDesc == aString) strcpy(saveVal, tokenval);
            exitLoop = FALSE;
            while(! exitLoop && ! my_eof(patternFile)) {
                matched = matchDesc(firstDesc, saveVal);
                if(error) return;
                if(matched) {
                    setPosition(format, beginningOfLoop);
                    doDescList();
                    if(error) return;
                } else {
                    exitLoop = TRUE;
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
            do {           /* repeat body */
                if(my_eof(patternFile)) {
                    setPosition(format, endOfLoop);
                    break;
                }
                matched = matchDesc(descAfter, saveVal);
                if(error) return;
                if(matched) {
                    setPosition(format, endOfLoop);
                    exitAlt = TRUE;
                } else {      /* try alternatives */
                    altDone = FALSE;
                    do {
                        matched = matchDescList();
                        if(error) return;
                        if(matched) {
                            doDescList();
                            if(error) return;
                            /* tok now contains 'or' or altEnd */
                            altDone = TRUE;
                            setPosition(format, beginningOfLoop);
                        } else {   /* try next alternative */
                            while(tok != or && tok != altEnd)
                                tok = nextToken(format);
                            if(tok == altEnd) {
                                /* sprintf(errorInfo,
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
        default:             /* tok must be a descriptor */
            doDesc(tok);
            if(error) return;
            break;
        }         /* switch */
        tok = nextToken(format);
    }       /* while */

    if(! atEnd(patternFile)) {
        sprintf(errorInfo, "End of file expected!");
        error(5);
    }

    if(inCount == 0L) {
        strcpy(errorInfo, "No input pattern!");
        error(5);
    }

    if(numPats && inCount != numPats) {
        strcpy(errorInfo, "Less patterns read than specified!");
        error(5);
    }

    if(outCount == 0L && classCount == 0L) {
        strcpy(errorInfo, "Missing both output patterns and class names!");
        error(5);
    }

    if(outCount && outCount != inCount) {
        strcpy(errorInfo, "Different number of input and output patterns!");
        error(5);
    }

    if(classCount && classCount != inCount) {
        strcpy(errorInfo, "Different number of input patterns and class names!");
        error(5);
    }

    pats->inputDims = inDims;
    pats->outputDims = outDims;
    pats->count = inCount;
    genClassNos(pats);
    if(error) return;
    /* classNos and classCount are now also initialized */

    error = 0;
}             /* fileIn */


/***************************************************/
/* Returns TRUE iff all descriptors up to the next */
/* token that is not a descriptor can be matched   */
/* against the remaining characters of the pattern */
/* file. Neither the file pointer of patternFile   */
/* nor the format pointer is moved.                */
/* Sets the global variable error.                 */
/***************************************************/
static Boolean matchDescList() {
    Boolean matched;
    long filePos, formatPos;

    filePos = ftell(patternFile);
    checkPosR(filePos,FALSE);
    formatPos = getPosition(format);
    tok = nextToken(format);

    while(isDesc(tok)) {
        enum Token next;

        switch(tok) {
        case input:
            next = lookUp(format);
            if(isDesc(next)) {
                matched = matchVectorAll(inDims);
                if(error) return FALSE;
            } else matched = matchVector();
            break;
        case output:
            next = lookUp(format);
            if(isDesc(next)) {
                matched = matchVectorAll(outDims);
                if(error) return FALSE;
            } else matched = matchVector();
            break;
        case outputDim:
            matched = matchLong(0L);   /* Minimum: 0 */
            break;
        case inputDim:
        case numPatterns:
            matched = matchLong(1L);   /* Minimum: 1 */
            break;
        case asterisk:
            next = lookUp(format);
            matched = TRUE;
            if(isDesc(next)) doAsterisk();
            break;
        case questionMark:
        case class:
                    next = lookUp(format);
            if(isDesc(next)) {
                char c;

                skipWhiteSpace(patternFile);
                if(fgetc(patternFile) == EOF) matched = FALSE;
                else {
                    if(fseek(patternFile, -1L, 1)) errorR(6,FALSE);
                    matched = TRUE;
                    fscanf(patternFile, "%*s");
                }
            } else matched = matchQuestionMark();
            break;
        case newLine:
            matched = matchNewLine();
            break;
        case writeNewLine:   /* ignore when reading */
            matched = TRUE;
            break;
        case aString:
            matched = matchString(tokenval);
            break;
        }         /* switch */

        if(! matched) {
            setPosR(patternFile, filePos,FALSE);
            error = 0;
            return FALSE;
        }
        tok = nextToken(format);
    }         /* while */

    setPosR(patternFile, filePos,FALSE);
    setPosition(format, formatPos);
    error = 0;
    return TRUE;
}          /* matchDescList */


/***************************************************/
/* Returns TRUE iff a string could be read from    */
/* patternFile that can be matched against the     */
/* token desc.                                     */
/* The file pointer of patternFile is not moved.   */
/* Sets the global variable error.                 */
/***************************************************/
static Boolean matchDesc(enum Token desc, char *tokvalue) {
    Boolean answer;
    long filePos;

    filePos = ftell(patternFile);
    checkPosR(filePos,FALSE);

    switch(desc) {
    case outputDim:
        answer = matchLong(0L);
        break;
    case inputDim:
    case numPatterns:
        answer = matchLong(1L);
        break;
    case input:
    case output:
        answer = matchVector();
        break;
    case class:
                answer = matchClassName();
        break;
    case newLine:
        answer = matchNewLine();
        break;
    case writeNewLine:
    case asterisk:
        answer = TRUE;
        break;
    case questionMark:
        answer = matchQuestionMark();
        break;
    case aString:
        answer = matchString(tokvalue);
        break;
    case endOfString:
        answer = atEnd(patternFile);
        break;
    }      /* switch */

    setPosR(patternFile, filePos,FALSE);

    error = 0;
    return answer;
}               /* matchDesc */


/********************************************************/
/* Tests whether the string str can be read from the    */
/* pattern file. One or more white space characters in  */
/* str correspond to any number of white space          */
/* characters in the pattern file.                      */
/********************************************************/
static Boolean matchString(char *str) {
    char *cp;
    char c;

    cp = str;
    skipWhiteSpace(patternFile);              /* overread white space */
    while(isspace(*cp)) cp++;
    while(*cp) {
        if(isspace(*cp)) {
            while(isspace(*(++cp)));
            skipWhiteSpace(patternFile);
        } else {
            c = fgetc(patternFile);
            if(c != *cp) return FALSE;
            cp++;
        }
    }       /* while */
    /* end of str reached */

    return TRUE;
}            /* matchString */


/************************************************/
/* Skips over the next string in the pattern    */
/* file. Returns TRUE iff such a string exists  */
/* before EOF is reached.                        */
/************************************************/
static Boolean matchQuestionMark() {
    return ! atEnd(patternFile);
}          /* matchQuestionMark */


/*************************************************/
/* Returns TRUE iff the next character is a      */
/* newline.                                       */
/*************************************************/
static Boolean matchNewLine() {
    char c;

    c = fgetc(patternFile);
    while(c == ' ' || c == '\t') c = fgetc(patternFile);
    return c == '\n';
}               /* matchNewLine */


/*************************************************/
/* Returns TRUE iff an integer greater than or   */
/* equal to min could be read from the pattern   */
/* file.                                          */
/*************************************************/
static Boolean matchLong(long min) {
    long value;
    int result;

    result = fscanf(patternFile, "%ld", &value);
    if(result != 1) return FALSE;
    return value >= min;
}               /* matchLong */


/***********************************************/
/* Returns TRUE iff the next item in the       */
/* pattern file is a floating point number.    */
/* Sets the global variable error.             */
/***********************************************/
static Boolean matchVector() {
    int dummy;

    return fscanf(patternFile, "%f", &dummy) == 1;
}              /* matchVector */


/***********************************************/
/* Returns TRUE iff dim floats come up next in */
/* the pattern file, if dim > 0.               */
/* If dim == 0, returns TRUE iff at least one  */
/* float comes up. In this case, the file      */
/* pointer is placed exactly after the last    */
/* floating point number.                      */
/* Sets the global variable error.             */
/***********************************************/
static Boolean matchVectorAll(long dim) {
    int dummy;
    long pos;          /* store file positions */
    int result;
    long count, i;

    if(dim) {
        for(i = 0L; i < dim; i++)
            if(fscanf(patternFile, "%f", &dummy) != 1) {
                error = 0;
                return FALSE;
            }
    } else {           /* input dimension is unknown so far */
        count = 0L;
        do {
            pos = ftell(patternFile);
            checkPosR(pos,FALSE);

            result = fscanf(patternFile, "%f", &dummy);
            if(result != 1) setPosR(patternFile, pos,FALSE)
                else count++;
        } while(result == 1);

        if(count == 0L) {
            error = 0;
            return FALSE;
        }
    }             /* else */

    error = 0;
    return TRUE;
}               /* matchVectorAll */


/*************************************************/
/* Reads the next word from the pattern file and */
/* returns TRUE iff such a word still comes up   */
/* at all.                                       */
/*************************************************/
static Boolean matchClassName() {
    return matchQuestionMark();
}             /* matchClassName */


/***********************************************/
/* Performs semantic actions for all           */
/* descriptors up to the next token that is    */
/* not a descriptor.                           */
/* Sets the global variable error.             */
/***********************************************/
static void doDescList() {
    tok = nextToken(format);
    while(isDesc(tok)) {
        doDesc(tok);
        if(error) return;
        tok = nextToken(format);
    }     /* while */

    error = 0;
}            /* doDescList */


/***********************************************/
/* Performs the semantic actions of the        */
/* interpreter. Sets the global variable       */
/* error.                                      */
/***********************************************/
static void doDesc(enum Token tk) {
    long l;
    Vector vec;
    static char buffer[CLASS_MAXLEN + 1];
    char *copy;

    switch(tk) {
    case inputDim:
        l = readLong(1);
        if(error) return;
        if(inDims && l != inDims) {
            error = 5;
            sprintf(errorInfo, "Input dimension declared twice or does not conform to the size of input vectors!");
            return;
        }
        inDims = l;
        exitLoop = TRUE;
        break;
    case outputDim:
        l = readLong(0);
        if(error) return;
        if(outDims && l != outDims) {
            error = 5;
            sprintf(errorInfo, "Output dimension declared twice or does not conform to the size of output vectors!");
            return;
        }
        outDims = l;
        exitLoop = TRUE;
        break;
    case numPatterns:
        l = readLong(1);
        if(error) return;
        if(numPats && numPats != l) {
            error = 5;
            sprintf(errorInfo, "Number of patterns already specified!");
            return;
        }
        numPats = l;
        if(inCount > numPats || outCount > numPats
                || classCount > numPats) {
            error = 5;
            sprintf(errorInfo, "More patterns than specified!");
            return;
        }
        exitLoop = TRUE;
        break;
    case input:
        vec = readVector(inDims);
        if(error) return;
        inCount++;
        if(numPats && numPats < inCount) {
            error = 5;
            sprintf(errorInfo, "More input patterns than specified!");
            return;
        }
        if(! add(pats->inputs, vec)) error(1);
        if(! inDims) inDims = dims(vec);
        if(inCount == numPats) exitLoop = TRUE;
        break;
    case output:
        vec = readVector(outDims);
        if(error) return;
        outCount++;
        if(numPats && numPats < outCount) {
            error = 5;
            sprintf(errorInfo, "More output patterns than specified!");
            return;
        }
        if(! add(pats->outputs, vec)) error(1);
        if(outDims == 0L) outDims = dims(vec);
        if(outCount == numPats) exitLoop = TRUE;
        break;
    case class:
                readClassName(buffer);
        if(error) return;
        classCount++;
        if(numPats && numPats < classCount) {
            error = 5;
            sprintf(errorInfo, "More class names than specified!");
            return;
        }
        copy = addSymbol(pats->symtab, buffer);
        if(error) return;
        if(! add(pats->classes, copy)) error(1);
        if(classCount == numPats) exitLoop = TRUE;
        break;
    case aString:
        doString(tokenval);
        if(error) return;
        break;
    case asterisk:
        doAsterisk();
        break;
    case questionMark:
        doQuestionMark();
        if(error) return;
        break;
    case newLine:
        doNewLine();
        if(error) return;
        break;
    case writeNewLine:                     /* do nothing */
        break;
    }          /* switch */

    error = 0;
}             /* doDesc */


/********************************************************/
/* Reads a string in the format str from the pattern    */
/* file. One or more white space characters in str      */
/* correspond to any number of white space characters   */
/* in the pattern file. Increments rowCount by the      */
/* number of newlines read. Sets the global variable    */
/* error.                                               */
/********************************************************/
static void doString(char *str) {
    char *cp;
    char c;

    cp = str;
    skipSpaceAndCountNl(patternFile);              /* overread white space */
    while(isspace(*cp)) cp++;

    while(*cp) {
        if(isspace(*cp)) {
            while(isspace(*(++cp)));
            if(*cp) skipSpaceAndCountNl(patternFile);
        } else {
            c = fgetc(patternFile);
            if(c != *cp) {
                error = 5;
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
/* Moves the file pointer of patternFile        */
/* to exactly after the next newline character  */
/* or to the end of the file. Increments        */
/* rowCount by 1 in the first case.             */
/************************************************/
static void doAsterisk() {
    char c;

    do {
        if((c = fgetc(patternFile)) == '\n') rowCount++;
    } while(c != EOF && c != '\n');
}          /* doAsterisk */


/************************************************/
/* Skips over the next string in the pattern    */
/* file and sets the global variable error.     */
/* Increments rowCount by the number of         */
/* newlines read.                                */
/************************************************/
static void doQuestionMark() {
    char c;

    skipSpaceAndCountNl(patternFile);
    if(fgetc(patternFile) == EOF) {
        error = 5;
        sprintf(errorInfo, "Any word expected!");
        return;
    }

    if(fseek(patternFile, -1L, 1)) error(6);
    fscanf(patternFile, "%*s");

    error = 0;
}           /* doQuestionMark */


/**************************************************/
/* Moves the file pointer of patternFile exactly  */
/* after the next newline or EOF character.       */
/* Sets the global variable error to nonzero iff  */
/* a character that is neither a tab nor a blank  */
/* comes before it. Increments rowCount by 1 if   */
/* it stops right after a newline.                */
/**************************************************/
static void doNewLine() {
    char c;

    do {
        if((c = fgetc(patternFile)) == '\n') rowCount++;
        if(!isspace(c) && c != EOF) {
            error = 5;
            sprintf(errorInfo, "End of line expected!");
            return;
        }
    } while(c == ' ' || c == '\t');

    error = 0;
}               /* doNewLine */


/*************************************************/
/* Reads an integer from the pattern file that   */
/* is greater than or equal to min.              */
/* Increments rowCount by the number of          */
/* newlines read beforehand.                     */
/* Sets the global variable error.               */
/*************************************************/
static long readLong(long min) {
    long value;
    int result;

    skipSpaceAndCountNl(patternFile);
    result = fscanf(patternFile, "%ld", &value);
    if(result != 1) {
        error = 5;
        sprintf(errorInfo, "Integer expected!");
        return 0L;
    }
    if(value < min) {
        error = 5;
        sprintf(errorInfo, "Number must be %ld at minimum!", min);
        return 0L;
    }

    error = 0;
    return value;
}               /* readLong */


/***********************************************/
/* Reads dim floats from the pattern file, if  */
/* dim > 0. Otherwise, reads all consecutive   */
/* floats until something else comes up.       */
/* Sets the global variable error.             */
/***********************************************/
static Vector readVector(long dim) {
    long i;
    Vector answer;
    long pos;               /* store file positions */
    int result;
    Number *np, n;

    if(dim) {               /* dimension already known */
        if(! (answer = newVector(dim))) errorR(1,NULL);

        for(i = 1L; i <= dim; i++) {
            skipSpaceAndCountNl(patternFile);
            if(fscanf(patternFile, "%f", &n) != 1) {
                error = 5;
                sprintf(errorInfo, "Floating number expected!");
                return NULL;
            }
            putDim(answer, i, n);
        }        /* for */
    }      /* if */
    else {             /* input dimension is unknown by now */
        Collection coll;       /* can dynamically grow in contrast to answer */

        if(! (coll = newColl())) errorR(1,NULL);
        do {
            long saveRowCount = rowCount;

            if(! (np = (Number *) malloc(sizeof(*np)))) errorR(1,NULL);
            pos = ftell(patternFile);
            checkPosR(pos,NULL);
            skipSpaceAndCountNl(patternFile);
            if((result = fscanf(patternFile, "%f", np)) == 1) {
                if(! add(coll, np)) errorR(1,NULL);
            } else {
                /* reset the file pointer to right after the last number read */
                setPosR(patternFile, pos,NULL);
                rowCount = saveRowCount;
            }
        } while(result == 1);

        if(size(coll) == 0L) {
            error = 5;
            sprintf(errorInfo, "Vector expected!");
            freeColl(coll);
            return NULL;
        } else {
            if(! (answer = newVector(size(coll)))) errorR(1,NULL);
            copyFromColl(answer, coll);
            freeCollAll(coll);
        }    /* else */
    }      /* else */

    error = 0;
    return answer;
}               /* readVector */


/*************************************************/
/* Reads the next word from the pattern file     */
/* into the memory area pointed to by buf.       */
/* Sets the global variable error.               */
/*************************************************/
static void readClassName(char *buf) {
    int result;

    skipSpaceAndCountNl(patternFile);
    result = fscanf(patternFile, CLASS_SCANF_FORMAT, buf);
    if(result != 1) {
        error = 5;
        sprintf(errorInfo, "Class name expected!");
        return;
    }
    error = 0;
}               /* readClassName */


/*************************************************/
/*************************************************/
static Boolean my_eof(FILE *f) {
    if(fgetc(f) == EOF) return TRUE;
    fseek(f, -1L, 1);
    return FALSE;
}              /* my_eof */
