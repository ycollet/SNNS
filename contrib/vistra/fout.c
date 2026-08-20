static Boolean numPatsWritten, inDimsWritten, outDimsWritten;
/* TRUE iff the corresponding values     */
/* have already been written.            */
static Collection patch;
/* patch contains the file positions at which the dimensionality of the  */
/* output vectors was written.                                          */
/* At these file positions, a 0 may have to be changed to a 1 if class   */
/* numbers were used instead of output vectors.                         */
static Boolean classNosAsOutputs;
/* TRUE iff a class number was written in place of an output vector      */
/* (because none exist).                                                 */


static Boolean writeNextList(Boolean);
static void writeDescList(void);
static void backpatch(FILE *, Collection, char);

/***************************************************/
/* Write the patterns pttrns to file f in format   */
/* form. Sets the global variable error.           */
/***************************************************/
void fileOut(Patterns pttrns, Format form, FILE *f) {
    Boolean descListWritten, bodyExecuted;
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
    while(tok != endOfString) {
        switch(tok) {
        case loopBegin:
            beginningOfLoop = getPosition(format);
            bodyExecuted = FALSE;
            while(writeNextList(bodyExecuted)) {
                /* writeNextList has moved the format pointer */
                setPosition(format, beginningOfLoop);
                tok = nextToken(format);
                writeDescList();
                if(error) {
                    freeCollAll(patch);
                    return;
                }
                bodyExecuted = TRUE;
                /* writeDescList has moved the format pointer */
                setPosition(format, beginningOfLoop);
            }       /* while */
            tok = nextToken(format);
            break;
        case altBegin:
            beginningOfLoop = getPosition(format);
            bodyExecuted = FALSE;
            do {
                setPosition(format, beginningOfLoop);
                descListWritten = FALSE;
                do {
                    beginningOfAlt = getPosition(format);
                    if(writeNextList(bodyExecuted)) {
                        setPosition(format, beginningOfAlt);
                        tok = nextToken(format);
                        writeDescList();
                        if(error) {
                            freeCollAll(patch);
                            return;
                        }
                        descListWritten = TRUE;
                    }
                } while(tok != altEnd);
                bodyExecuted = TRUE;
            } while(descListWritten);
            tok = nextToken(format);
            break;
        default:
            writeDescList();
            if(error) {
                freeCollAll(patch);
                return;
            }
            break;
        }       /* switch */
    }         /* while */

    if(inCount != num(pats)) error(8);
    if(outCount && outCount != size(pats->classNos)) error(9);
    if(classCount && classCount != size(pats->classNos)) error(10);

    error = 0;
}           /* fileOut */


/***************************************************/
/* Return TRUE iff the list of descriptors,        */
/* starting at the current position of format,     */
/* can be written to the file. The format pointer  */
/* of format is moved past the first token that    */
/* is not a descriptor.                            */
/***************************************************/
static Boolean writeNextList(Boolean bodyExecuted) {
    long inputsInList = 0L, outputsInList = 0L, classesInList = 0L;
    Boolean descToWrite = FALSE;
    Boolean endLoop = FALSE;

    tok = nextToken(format);
    while(isDesc(tok) && ! endLoop) {
        switch(tok) {
        case input:
            if(++inputsInList + inCount > size(pats->inputs))
                endLoop = TRUE;
            else descToWrite = TRUE;
            break;
        case output:
            if(++outputsInList + outCount > size(pats->classNos))
                endLoop = TRUE;
            else descToWrite = TRUE;
            break;
        case class:
                    if(++classesInList + classCount > size(pats->classNos))
                        endLoop = TRUE;
                    else descToWrite = TRUE;
            break;
        case numPatterns:
            if(! numPatsWritten) descToWrite = TRUE;
            break;
        case inputDim:
            if(! inDimsWritten) descToWrite = TRUE;
            break;
        case outputDim:
            if(! outDimsWritten) descToWrite = TRUE;
            break;
        case aString:
            if(! bodyExecuted) descToWrite = TRUE;
            break;
        default:
            break;
        }          /* switch */

        tok = nextToken(format);
    }            /* while */

    if(endLoop) {
        while(isDesc(tok)) tok = nextToken(format);
        return FALSE;
    } else return descToWrite;
}              /* writeNextList */


/***************************************************/
/* Write all descriptors, one after another, to    */
/* the file patternFile until a token is           */
/* encountered that is not a descriptor. The       */
/* format pointer of format is moved past this     */
/* token. Sets the global variable error.          */
/***************************************************/
static void writeDescList() {
    Vector vec;
    char *str;

    while(isDesc(tok)) {
        switch(tok) {
        case input:
            if(inCount < size(pats->inputs)) {
                vec = (Vector) at(pats->inputs, ++inCount);
                if(fprintVector(vec, patternFile)) error(7);
            }
            if(fputc(' ', patternFile) == EOF) error(7);
            break;
        case output:
            if(outCount < size(pats->classNos)) {
                if(hasOutputs(pats)) {
                    vec = (Vector) at(pats->outputs, ++outCount);
                    if(fprintVector(vec, patternFile)) error(7);
                } else {   /* write the class number */
                    fprintf(patternFile, "%ld",
                            *(long *) at(pats->classNos, ++outCount));
                    if(! classNosAsOutputs && notEmpty(patch)) {
                        backpatch(patternFile, patch, '1');
                        if(error) return;
                        freeFromTo(patch, 1L, size(patch), free);
                        classNosAsOutputs = TRUE;
                    }
                }
            }
            if(fputc(' ', patternFile) == EOF) error(7);
            break;
        case class:
                    if(hasClassNames(pats) &&
                            classCount < size(pats->classes)) {
                        if(fprintf(patternFile, "%s ",
                                   (char *) at(pats->classes, ++classCount)) < 0)
                            error(7);
                    } else if(classCount < size(pats->classNos))
                        if(fprintf(patternFile, "%ld ",
                                   *((long *) at(pats->classNos, ++classCount)))<0)
                            error(7);
            break;
        case numPatterns:
            if(fprintf(patternFile, "%ld ", num(pats)) < 0)
                error(7);
            numPatsWritten = TRUE;
            break;
        case inputDim:
            if(fprintf(patternFile, "%ld ", pats->inputDims) < 0)
                error(7);
            inDimsWritten = TRUE;
            break;
        case outputDim:    {
            long toWrite, *lp;
            if(! hasOutputs(pats) && ! classNosAsOutputs) {
                if(! (lp = (long *) malloc(sizeof(*lp))))
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
        case newLine:
            if(fprintf(patternFile, "\n") < 0) error(7);
            break;
        case questionMark:
            if(fprintf(patternFile, "? ") < 0) error(7);
            break;
        case aString:
            if(fprintf(patternFile, "%s", tokenval) < 0) error(7);
            break;
        }         /* switch */
        tok = nextToken(format);
    }           /* while */

    error = 0;
}             /* writeDescList */


/*************************************************************************/
/* Replace all characters of file f located at the positions in f given  */
/* by positions (a Collection of long pointers) with the character      */
/* patchChar.                                                            */
/* Sets the global variable error.                                       */
/*************************************************************************/
static void backpatch(FILE *f, Collection positions, char patchChar) {
    long initialPos, *pos, i, n = size(positions);

    initialPos = ftell(f);
    for(i = 1L; i <= n; i++) {
        pos = (long *) at(positions, i);
        if(fseek(f, *pos, 0) == -1) error(7);
        fputc(patchChar, f);
    }
    if(fseek(f, initialPos, 0) == -1) error(7);
    error = 0;
}             /* backpatch */













