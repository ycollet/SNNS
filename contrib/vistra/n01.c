/******************************************************/
/******************************************************/
/*******   Routines for reading and writing   *********/
/*******        N 0 1   f o r m a t s         *********/
/******************************************************/
/******************************************************/

#define N01Number   float
#define N01Class    unsigned

typedef struct {
    unsigned InputDimension;
    unsigned OutputDimension;
    unsigned NumberOfData;
    unsigned PlaceOfInputData;
    unsigned PlaceOfOutputData;
    unsigned PlaceOfClassData;
    unsigned NumberOfClasses;
} N01Type;

static void checkN01Header(N01Type *);             /* sets error */
static N01Number reverseNumber(N01Number);
static unsigned reverseUnsigned(unsigned);
static void writeN01_SUN(Patterns, FILE *);        /* sets error */
static void writeN01_DEC(Patterns, FILE *);        /* sets error */

/**********************************************************/
/* Read in file f, which is in N01 format.                */
/* Return the vectors and classes read, in the form of    */
/* a Patterns type.                                       */
/* Sets the global variable error.                        */
/**********************************************************/
Patterns readN01(FILE *f) {
    N01Type header;
    Patterns p;
    Vector v;
    long i, j;
    char n01Tag[4];

    if(fread(n01Tag, sizeof(char), 4, f) != 4) errorR(16,NULL);

    if(fread(&header, sizeof(header), 1, f) != 1) errorR(16,NULL);
    if(! isDEC) {
        header.NumberOfData = reverseUnsigned(header.NumberOfData);
        header.PlaceOfInputData = reverseUnsigned(header.PlaceOfInputData);
        header.PlaceOfOutputData = reverseUnsigned(header.PlaceOfOutputData);
        header.PlaceOfClassData = reverseUnsigned(header.PlaceOfClassData);
        header.NumberOfClasses = reverseUnsigned(header.NumberOfClasses);
        header.InputDimension = reverseUnsigned(header.InputDimension);
        header.OutputDimension = reverseUnsigned(header.OutputDimension);
    }

    /* Check the contents of the header */
    checkN01Header(&header);
    if(error) return NULL;

    p = newPatterns();
    if(error) return NULL;
    p->inputDims = header.InputDimension;
    p->outputDims = header.OutputDimension;
    p->count = header.NumberOfData;
    p->classCount = header.NumberOfClasses;

    /* Read the input patterns */
    if(fseek(f, header.PlaceOfInputData, 0)) errorR(16,NULL);
    for(i = 1L; i <= header.NumberOfData; i++) {
        if(! (v = newVector(header.InputDimension))) errorR(1,NULL);
        if(fread(v->elements, sizeof(N01Number), header.InputDimension, f)
                != header.InputDimension) errorR(16,NULL);
        if(! isDEC)
            for(j = 0L; j < header.InputDimension; j++)
                v->elements[j] = reverseNumber(v->elements[j]);
        if(! add(inputs(p), v)) errorR(1,NULL);
    }

    /* Read the output patterns */
    if(header.PlaceOfOutputData) {
        /* output vectors present */
        if(fseek(f, header.PlaceOfOutputData, 0)) errorR(16,NULL);
        for(i = 1L; i <= header.NumberOfData; i++) {
            if(! (v = newVector(header.OutputDimension))) errorR(1,NULL);
            if(fread(v->elements, sizeof(N01Number), header.OutputDimension, f)
                    != header.OutputDimension) errorR(16,NULL);
            if(! isDEC)
                for(j = 0L; j < header.OutputDimension; j++)
                    v->elements[j] = reverseNumber(v->elements[j]);
            if(! add(outputs(p), v)) errorR(1,NULL);
        }    /* for */
    }      /* if */

    /* Read the class numbers */
    if(header.PlaceOfClassData) {
        /* class numbers present */
        if(fseek(f, header.PlaceOfClassData, 0)) errorR(16,NULL);
        for(i = 1L; i <= header.NumberOfData; i++) {
            long *classNo;
            N01Class cls;

            classNo = (long *) malloc(sizeof(*classNo));
            if(! classNo) errorR(1,NULL);
            if(fread(&cls, sizeof(cls), 1, f) != 1) errorR(16,NULL);
            /* no error */
            if(! isDEC) cls = reverseUnsigned(cls);
            *classNo = (long) cls;
            if(! add(classNos(p), classNo)) errorR(1,NULL);
        }    /* for */
    }      /* if */

    error = 0;
    return p;
}          /* readN01 */


/*****************************************************/
/* Check the N01 header hd for plausibility.         */
/* Sets the global variable error accordingly.       */
/*****************************************************/
static void checkN01Header(N01Type *hd) {
    if(hd->InputDimension == 0) {
        strcpy(errorInfo, "Input dimension of 0 specified!");
        error(18);
    }
    if(hd->NumberOfData == 0) {
        strcpy(errorInfo, "0 specified for the number of pattern!");
        error(18);
    }
    if(! hd->PlaceOfOutputData && ! hd->PlaceOfClassData) {
        strcpy(errorInfo, "Both output and class data is missing!");
        error(18);
    }

    error = 0;
}          /* checkN01Header */


/*******************************************************/
/* Write the patterns p in N01 format to file f,       */
/* starting at the current file position of f.         */
/* Sets the global variable error.                     */
/*******************************************************/
void writeN01(Patterns p, FILE *f) {
    if(isDEC) writeN01_DEC(p, f);
    else writeN01_SUN(p, f);
}       /* writeN01 */


/*******************************************************/
/*******************************************************/
static void writeN01_SUN(Patterns p, FILE *f) {
    N01Type header;
    long i, j, nums;
    N01Number *elems;
    Vector v;
    Collection clsnos = classNos(p);
    char n01Tag[] = "N01";
    unsigned offsetAfterInputs, offsetAfterOutputs;

    /* write the N01 tag */
    if(fwrite(n01Tag, sizeof(char), 4, f) != 4) error(17);

    /* write the header */
    header.InputDimension = (unsigned) inputDims(p);
    header.OutputDimension = (unsigned) outputDims(p);
    header.NumberOfData = (unsigned) num(p);
    header.NumberOfClasses = (unsigned) maxClassNo(p);
    header.PlaceOfInputData = sizeof(header) + 4;
    offsetAfterInputs = header.PlaceOfInputData +
                        num(p) * header.InputDimension * sizeof(N01Number);
    if(hasOutputs(p)) {
        header.PlaceOfOutputData = offsetAfterInputs;
        offsetAfterOutputs = header.PlaceOfOutputData +
                             num(p) * header.OutputDimension * sizeof(N01Number);
    } else {
        header.PlaceOfOutputData = 0;
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

    /* Write the input vectors */
    if(! (elems = (N01Number *) malloc(p->inputDims * sizeof(*elems))))
        error(1);
    nums = size(p->inputs);
    for(i = 1L; i <= nums; i++) {
        v = (Vector) at(p->inputs, i);
        for(j = 0; j < p->inputDims; j++)
            elems[j] = reverseNumber(v->elements[j]);
        if(fwrite(elems, sizeof(N01Number), p->inputDims, f)
                != p->inputDims) error(17);
    }
    free(elems);

    /* Write the output vectors, if any are present */
    if(hasOutputs(p)) {
        if(! (elems = (N01Number *) malloc(p->outputDims *
                                           sizeof(*elems)))) error(1);
        nums = size(p->outputs);
        for(i = 1L; i <= nums; i++) {
            v = (Vector) at(p->outputs, i);
            for(j = 0; j < p->outputDims; j++)
                elems[j] = reverseNumber(v->elements[j]);
            if(fwrite(elems, sizeof(Number), p->outputDims, f) != p->outputDims)
                error(17);
        }
        free(elems);
    }        /* if */


    /* Write the class numbers */
    if(notEmpty(clsnos)) {
        N01Class c;
        nums = size(clsnos);
        for(i = 1L; i <= nums; i++) {
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
static void writeN01_DEC(Patterns p, FILE *f) {
    N01Type header;
    long i, nums;
    Vector v;
    Collection clsnos = classNos(p);
    char n01Tag[] = "N01";
    unsigned offsetAfterInputs, offsetAfterOutputs;

    /* write the N01 tag */
    if(fwrite(n01Tag, sizeof(char), 4, f) != 4) error(17);

    /* write the header */
    header.InputDimension = (unsigned) inputDims(p);
    header.OutputDimension = (unsigned) outputDims(p);
    header.NumberOfData = (unsigned) num(p);
    header.NumberOfClasses = (unsigned) maxClassNo(p);
    header.PlaceOfInputData = sizeof(header) + 4;
    offsetAfterInputs = header.PlaceOfInputData +
                        num(p) * header.InputDimension * sizeof(N01Number);
    if(hasOutputs(p)) {
        header.PlaceOfOutputData = offsetAfterInputs;
        offsetAfterOutputs = header.PlaceOfOutputData +
                             num(p) * header.OutputDimension * sizeof(N01Number);
    } else {
        header.PlaceOfOutputData = 0;
        offsetAfterOutputs = offsetAfterInputs;
    }
    header.PlaceOfClassData = (isEmpty(clsnos) ? 0 : offsetAfterOutputs);
    if(fwrite(&header, sizeof(header), 1, f) != 1) error(17);

    /* Write the input vectors */
    nums = size(p->inputs);
    for(i = 1L; i <= nums; i++) {
        v = (Vector) at(p->inputs, i);
        if(fwrite(v->elements, sizeof(N01Number), header.InputDimension, f)
                != header.InputDimension) error(17);
    }

    /* Write the output vectors, if any are present */
    if(hasOutputs(p)) {
        nums = size(p->outputs);
        for(i = 1L; i <= nums; i++) {
            v = (Vector) at(p->outputs, i);
            if(fwrite(v->elements, sizeof(Number), header.OutputDimension, f)
                    != header.OutputDimension) error(17);
        }
    }        /* if */


    /* Write the class numbers */
    if(notEmpty(clsnos)) {
        N01Class c;
        nums = size(clsnos);
        for(i = 1L; i <= nums; i++) {
            c = (N01Class) *((long *) at(clsnos, i));
            if(fwrite(&c, sizeof(c), 1, f) != 1)
                error(17);
        }      /* for */
    }        /* if */

    error = 0;
}          /* writeN01_DEC */


/**************************************************/
/**************************************************/
static N01Number reverseNumber(N01Number n) {
    N01Number answer;
    char i, *bp1, *bp2;

    bp1 = (char *) (&n) + sizeof(n);
    bp2 = (char *) &answer;
    for(i = 0; i < sizeof(n); i++)
        *bp2++ = *(--bp1);

    return answer;
}          /* reverseNumber */


/**************************************************/
/**************************************************/
static unsigned reverseUnsigned(unsigned u) {
    unsigned answer;
    char i, *bp1, *bp2;

    bp1 = (char *) (&u) + sizeof(u);
    bp2 = (char *) &answer;
    for(i = 0; i < sizeof(u); i++)
        *bp2++ = *(--bp1);

    return answer;
}          /* reverseUnsigned */
