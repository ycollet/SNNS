/*****************************************************/
/* Write the patterns p in LVQ format to file f.     */
/* Sets the global variable error.                   */
/*****************************************************/
void writeLVQ(Patterns p, FILE *f) {
    lvqWrite(p, f, TRUE);
    if(error) return;

    error = 0;
}        /* writeLVQ */


/*****************************************************/
/* Read in the pattern file f, which is in LVQ       */
/* format. Return the patterns.                      */
/* Sets the global variable error.                   */
/*****************************************************/
Patterns readLVQ(FILE *f) {
    Patterns answer;
    long i, vecCount;
    Number n;
    Vector v;
    char buf[CLASS_MAXLEN + 1], *copy;

    answer = newPatterns();
    if(error) return NULL;

    rowCount = 1L;
    skipSpaceAndCountNl(f);
    if(fscanf(f, "%ld", &(answer->inputDims)) != 1) {
        strcpy(errorInfo, "Integer expected!");
        freePatterns(answer);
        errorR(5,NULL);
    }

    vecCount = 0L;
    while(! atEndNl(f)) {
        /* read in the input vector */
        if(! (v = newVector(answer->inputDims))) errorR(1,NULL);
        for(i = 1L; i <= answer->inputDims; i++) {
            if(fscanf(f, "%f", &n) != 1) {
                sprintf(errorInfo, "Floating number expected!");
                freePatterns(answer);
                errorR(5,NULL);
            }
            putDim(v, i, n);
            skipSpaceAndCountNl(f);
        }        /* for */
        if(! add(answer->inputs, v)) errorR(1,NULL);
        vecCount++;

        /* read in the class symbol */
        if(fscanf(f, CLASS_SCANF_FORMAT, buf) != 1) {
            strcpy(errorInfo, "Class name expected!");
            freePatterns(answer);
            errorR(5,NULL);
        }
        copy = addSymbol(answer->symtab, buf);
        if(error) return NULL;
        if(! add(answer->classes, copy)) errorR(1,NULL);
    }          /* while */

    /* check whether at least 1 input vector was read. */
    if(vecCount == 0L) {
        strcpy(errorInfo, "No input pattern!");
        freePatterns(answer);
        errorR(5,NULL);
    }

    answer->count = vecCount;
    answer->classCount = (long) numSymbols(answer->symtab);
    answer->outputDims = 0L;
    genClassNosFromNames(answer);
    if(error) return NULL;

    error = 0;
    return answer;
}        /* readLVQ */


/*****************************************************/
/* Read in the vectors of the LVQ file f.            */
/* inVecs should be set to TRUE if the vectors are   */
/* input vectors, and to FALSE for output vectors.   */
/* Sets the global variable error.                   */
/*****************************************************/
void lvqRead(Patterns p, FILE *f, Boolean inVecs) {
    VecColl newVecs;
    long i, vecCount, ndims;
    Number n;
    Vector v;
    char buf[CLASS_MAXLEN + 1], *copy;

    rowCount = 1L;
    skipSpaceAndCountNl(f);
    if(fscanf(f, "%ld", &ndims) != 1) {
        strcpy(errorInfo, "Integer expected!");
        error(5);
    }

    if(! (newVecs = newColl())) error(1);
    vecCount = 0L;
    while(! atEndNl(f)) {
        /* read in the input vector */
        if(! (v = newVector(ndims))) error(1);
        for(i = 1L; i <= ndims; i++) {
            if(fscanf(f, "%f", &n) != 1) {
                sprintf(errorInfo, "Floating number expected!");
                freeDeep(newVecs, (void(*)(void*))freeVector);
                error(5);
            }
            putDim(v, i, n);
            skipSpaceAndCountNl(f);
        }        /* for */
        if(! add(newVecs, v)) error(1);
        vecCount++;

        /* read in the class symbol */
        if(fscanf(f, CLASS_SCANF_FORMAT, buf) != 1) {
            strcpy(errorInfo, "Class name expected!");
            freeDeep(newVecs, (void(*)(void*))freeVector);
            error(5);
        }
    }          /* while */

    /* discard the previous vectors */
    if(inVecs) {
        freeDeep(p->inputs, (void(*)(void*))freeVector);
        p->inputs = newVecs;
        p->inputDims = ndims;
    } else {
        freeDeep(p->outputs, (void(*)(void*))freeVector);
        p->outputs = newVecs;
        p->outputDims = ndims;
    }

    error = 0;
}        /* lvqRead */


/************************************************************/
/* If inVecs is TRUE, then write the input vectors of p     */
/* in LVQ format to f, otherwise the output vectors.        */
/* Sets the global variable error.                          */
/************************************************************/
void lvqWrite(Patterns p, FILE *f, Boolean inVecs) {
    Boolean symbols;
    long i, nvecs;
    VecColl vc;

    vc = (inVecs ? p->inputs : p->outputs);
    symbols = hasClassNames(p);
    nvecs = size(vc);
    if(fprintf(f, "%ld\n", numberOfCols(vc)) < 0) error(7);
    for(i = 1L; i <= nvecs; i++) {
        if(fprintVector((Vector) at(vc, i), f)) error(7);
        if(fputc(' ', f) == EOF) error(7);
        if(symbols) {
            if(fprintf(f, "%s\n", (char *) at(p->classes, i)) < 0) error(7);
        } else if(fprintf(f, "%ld\n", *((long *) at(p->classNos, i))) < 0) error(7);
    }
}            /* lvqWrite */





