#include "xvis.h"
#include <ctype.h>
#include <time.h>

static FILE *patternFile;          /* pattern file being processed */
static Format format;
static Patterns pats;
static enum Token tok;
static long inDims, outDims;       /* dimensions of the input and output vectors. */
static long inCount, outCount, classCount;
/* Count the input/output vectors and   */
/* class names that have been read or   */
/* written, respectively.               */
static long numPats;               /* number of patterns */

/* Private function headers */
static long classNoOfVec(Collection, Vector);
static void addMapping(Collection, Vector, long);  /* sets error */
static char *printStatVals(char *, VecColl);

#include "fin.c"
#include "fout.c"
#include "n01.c"
#include "lvq.c"


/*************************************************/
/* Returns a new patterns object.                */
/* Sets the global variable error.               */
/*************************************************/
Patterns newPatterns() {
    Patterns answer;

    answer = (Patterns) malloc(sizeof(*answer));
    if(answer == NULL) errorR(1,NULL);                /* not enough memory */

    if(! (answer->inputs = newColl())) errorR(1,NULL);
    if(! (answer->outputs = newColl())) errorR(1,NULL);
    if(! (answer->classes = newColl())) errorR(1,NULL);
    if(! (answer->classNos = newColl())) errorR(1,NULL);
    answer->symtab = newSymtab(NO_BUCKETS);
    if(error) return NULL;
    answer->count = 0L;
    answer->classCount = 0L;

    return answer;
}         /* newPatterns */


/********************************************/
/* Frees the memory of the patterns object  */
/* p.                                       */
/********************************************/
void freePatterns(Patterns p) {
    freeDeep(p->inputs, (void(*)(void*))freeVector);
    freeDeep(p->outputs, (void(*)(void*))freeVector);
    freeCollAll(p->classNos);
    freeColl(p->classes);
    freeSymtab(p->symtab);
    free(p);
}         /* freePatterns */


double drand48();
void srand48(long);

/*******************************************************/
/* Randomly change the order of the patterns.          */
/* If v != NULL, change the order of the elements of   */
/* v according to the change made to the patterns.     */
/* Sets the global variable error.                     */
/*******************************************************/
void randomize(Patterns p, Vector v) {
    VecColl randInputs, randOutputs;
    Collection randClasses, randClassNos, randVec, vColl;
    long i, index;
    double randNum;
    Number *newElems;

    if(! (randInputs = newColl())) error(1);
    if(! (randOutputs = newColl())) error(1);
    if(! (randClasses = newColl())) error(1);
    if(! (randClassNos = newColl())) error(1);
    if(v) {
        if(! (vColl = asColl(v))) error(1);
        if(! (randVec = newColl())) error(1);
    }

    srand48((long) time(NULL));
    for(i = p->count; i >= 1; i--) {
        /* generate random number between 0.0 and 1.0 */
        randNum = drand48();
        index = min(((long) (randNum * i)) + 1L, i);
        if(! add(randInputs, at(p->inputs, index))) error(1);
        removeAt(p->inputs, index);
        if(! add(randClassNos, at(p->classNos, index))) error(1);
        removeAt(p->classNos, index);
        if(hasOutputs(p)) {
            if(! add(randOutputs, at(p->outputs, index))) error(1);
            removeAt(p->outputs, index);
        }
        if(hasClassNames(p)) {
            if(! add(randClasses, at(p->classes, index))) error(1);
            removeAt(p->classes, index);
        }
        if(v) {
            if(! add(randVec, at(vColl, index))) error(1);
            removeAt(vColl, index);
        }
    }        /* for */

    freeColl(p->inputs);
    freeColl(p->classNos);
    freeColl(p->outputs);
    freeColl(p->classes);
    if(v) {
        copyFromColl(v, randVec);
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
/* Removes all patterns of p with numbers   */
/* from to to inclusive.                    */
/* Returns the number of removed patterns.  */
/* Sets the global variable error.          */
/********************************************/
unsigned removePatterns(Patterns p, unsigned from, unsigned to) {
    unsigned i, firstToRemove, lastToRemove, numToRemove;
    unsigned answer = 0;

    if(from <= to && from <= p->count && to >= 1) {
        firstToRemove = max(1, from);
        lastToRemove = min(p->count, to);
        numToRemove = lastToRemove - firstToRemove + 1;
        if(numToRemove >= p->count) errorR(28,0);
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
/* Removes all dimensions from from to      */
/* to inclusive from the input vectors, if  */
/* flag==TRUE, or from the output vectors,  */
/* if flag==FALSE.                          */
/* Returns the actual number of removed     */
/* dimensions.                              */
/* Sets the global variable error.          */
/********************************************/
unsigned removeCols(Patterns p, Boolean flag, unsigned from, unsigned to) {
    unsigned ndims, firstToRemove, lastToRemove, numToRemove = 0;
    VecColl vc;

    ndims = (flag ? p->inputDims : p->outputDims);
    if((from<=to) && (from<=ndims) && (to>=1)) {
        firstToRemove = max(1, from);
        lastToRemove = min(ndims, to);
        numToRemove = lastToRemove - firstToRemove + 1;
        if(numToRemove >= ndims) errorR(29,0);
        vc = (flag ? p->inputs : p->outputs);
        removeColRange(vc, firstToRemove, lastToRemove);
        if(error) return 0;
        if(flag) p->inputDims -= numToRemove;
        else p->outputDims -= numToRemove;
    }

    error = 0;
    return numToRemove;
}          /* removeCols */


/**************************************************************/
/* Removes all dimensions given by the array lp.              */
/* Removes them from the input vectors, if flag==TRUE, or     */
/* from the output vectors, if flag==FALSE.                   */
/* The array lp is terminated by a 0.                         */
/* count gives the size of the array lp.                      */
/* Returns the number of removed dimensions.                  */
/* Sets the global variable error.                             */
/**************************************************************/
void removeDimList(Patterns p, Boolean flag, long *lp, long count) {
    long numVectors, numDims, i;
    VecColl vc;

    numDims = (flag ? p->inputDims : p->outputDims);
    vc = (flag ? p->inputs : p->outputs);
    numVectors = num(p);

    /* remove the dimensions from all vectors */
    for(i = 1L; i <= numVectors; i++) {
        removeDims((Vector) at(vc, i), lp, count);
        if(error) return;
    }
    if(flag) p->inputDims -= count;
    else p->outputDims -= count;
    error = 0;
}          /* removeDimList */


/********************************************/
/* Returns the number of input patterns     */
/* of p.                                    */
/********************************************/
long num(Patterns p) {
    return p->count;
}         /* num */


/********************************************/
/* Returns a VecColl of all input vectors   */
/* of p.                                    */
/********************************************/
VecColl inputs(Patterns p) {
    return p->inputs;
}          /* inputs */


/********************************************/
/* Returns a VecColl of all output vectors  */
/* of p. Returns an empty VecColl if none   */
/* are present.                             */
/********************************************/
VecColl outputs(Patterns p) {
    return p->outputs;
}          /* outputs */


/********************************************/
/* Returns a collection of all class names  */
/* of p. Returns an empty collection if     */
/* none are present.                        */
/********************************************/
Collection classes(Patterns p) {
    return p->classes;
}          /* classes */


/********************************************/
/* Returns a collection of all class        */
/* numbers of p. Returns an empty           */
/* collection if these have not yet been    */
/* computed.                                */
/********************************************/
Collection classNos(Patterns p) {
    return p->classNos;
}          /* classNos */


/********************************************/
/* Returns the number of distinct output    */
/* classes of p. This also corresponds to   */
/* the maximum class number.                */
/* If there are no output vectors and       */
/* classes, or if the numbers have not yet  */
/* been computed, 0 is returned.            */
/********************************************/
long maxClassNo(Patterns p) {
    return p->classCount;
}


/********************************************/
/* Returns true iff the patterns p also     */
/* have output vectors.                     */
/********************************************/
Boolean hasOutputs(Patterns p) {
    return notEmpty(outputs(p));
}       /* hasOutputs */


/********************************************/
/* Returns true iff the patterns p also     */
/* have class names.                        */
/********************************************/
Boolean hasClassNames(Patterns p) {
    return notEmpty(classes(p));
}       /* hasClassNames */


/********************************************/
/* Returns a string representing the class  */
/* of the i-th pattern of p. This is either */
/* a class name or a string representing    */
/* the class number.                        */
/********************************************/
char *classString(Patterns p, long i) {
    static char buf[NUM_OF_PATS_STR_MAXLEN + 1];

    if(hasClassNames(p)) return (char *) at(p->classes, i);
    else {
        sprintf(buf, "%ld", *(long *) at(p->classNos, i));
        return buf;
    }
}         /* classString */


#define ROW_TITLE_LEN      21

/********************************************/
/* Returns a string containing the text to  */
/* be displayed in the statistics window.   */
/* Returns NULL if not enough memory is     */
/* available.                               */
/********************************************/
char *statString(Patterns p, char *fn) {
    char *buf, *cp;
    unsigned headlen, vallen, buflen;

    headlen = 5*ROW_TITLE_LEN + 6*SIZEOF_NL + strlen(fn) +
              4*NUM_OF_PATS_STR_MAXLEN;
    vallen = 5*ROW_TITLE_LEN + 6*SIZEOF_NL + 4*NUMBER_STR_LENGTH;

    buflen = headlen + vallen + (hasOutputs(p) ? vallen : 0) +
             100;               /* for safety */
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

    sprintf(cp, "Inputs:\n");
    cp += 7 + SIZEOF_NL;
    cp = printStatVals(cp, inputs(p));
    if(hasOutputs(p)) {
        sprintf(cp, "Outputs:\n");
        cp += 8 + SIZEOF_NL;
        cp = printStatVals(cp, outputs(p));
    }

    return buf;
}         /* statString */


/********************************************/
/********************************************/
static char *printStatVals(char *cp, VecColl vc) {
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
/* Writes all symbols (without duplicates)  */
/* to file f starting at the current file   */
/* position. Sets the global variable       */
/* error.                                   */
/********************************************/
void writeSymtab(Patterns p, FILE *f) {
    long i, nvecs, diffVecs, nclassNos;
    Collection set;
    Vector v;

    if(hasClassNames(p)) {
        fprintSymbols(p->symtab, f);
        if(error) return;
    } else {          /* write the output vectors as symbols */
        if(hasOutputs(p)) {
            if(! (set = newColl())) error(1);
            nvecs = size(p->outputs);
            for(i = 1L; i <= nvecs; i++) {
                v = (Vector) at(p->outputs, i);
                if(detectPos(set, v, (Boolean(*)(void*,void*))veq) == -1L)
                    if(! add(set, v)) {
                        freeColl(set);
                        error(1);
                    }
            }     /* for */
            diffVecs = size(set);
            for(i = 1L; i <= diffVecs; i++) {
                if(fprintVector((Vector) at(set, i), f) != 0) {
                    freeColl(set);
                    error(15);
                }
                fprintf(f, "\n");
            }     /* for */
            freeColl(set);
        }       /* if(hasOutputs(p)) */
        else {  /* write the class numbers */
            for(i = 1L; i <= p->classCount; i++)
                fprintf(f, "%ld\n", i);
        }
    }         /* else */

    error = 0;
}           /* writeSymtab */


/********************************************/
/* Returns the dimension of the input       */
/* vectors of p.                            */
/********************************************/
long inputDims(Patterns p) {
    return p->inputDims;
}          /* inputDims */


/********************************************/
/* Returns the dimension of the output      */
/* vectors of p.                            */
/********************************************/
long outputDims(Patterns p) {
    return p->outputDims;
}          /* outputDims */


/********************************************/
/* Generates class numbers for the output   */
/* vectors of p. Identical output vectors   */
/* get identical numbers.                   */
/* The numbers start at 1 and are then      */
/* incremented by 1 each time an output     */
/* vector has not occurred before.          */
/* If there are no output vectors, the      */
/* class names are considered instead.      */
/* The function sets the variables          */
/* classNos and classCount.                 */
/* Sets the global variable error           */
/********************************************/
void genClassNos(Patterns p) {
    if(hasClassNames(p)) {
        genClassNosFromNames(p);
        if(error) return;
    } else if(hasOutputs(p)) {
        genClassNosFromVectors(p);
        if(error) return;
    }

    error = 0;
}          /* genClassNos */


/**************************************************/
/* Generates class numbers based on the class     */
/* names of the patterns p and sets the variables */
/* classNos and classCount accordingly.           */
/* Sets the global variable error.                */
/**************************************************/
void genClassNosFromNames(Patterns p) {
    long *classNo, numClasses, i;
    Collection namesOrder, clsnos;

    namesOrder = sequence(p->symtab);
    freeCollAll(classNos(p));
    if(! (p->classNos = newColl())) error(1);

    clsnos = p->classNos;
    numClasses = size(classes(p));
    for(i = 1L; i <= numClasses; i++) {
        if(! (classNo = (long *) malloc(sizeof(*classNo)))) error(1);
        *classNo = indexOf(namesOrder, at(classes(p), i));
        if(! add(clsnos, classNo)) error(1);
    }        /* for */

    p->classCount = size(namesOrder);
    error = 0;
}          /* genClassNosFromNames */


/**************************************************/
/* Generates class numbers based on the output    */
/* vectors of p and sets the variables            */
/* classNos and classCount accordingly.           */
/* Sets the global variable error.                */
/**************************************************/
void genClassNosFromVectors(Patterns p) {
    long *classNo, numVectors, i, noCount = 0L;
    Vector v;
    Collection map;

    if(! (map = newColl())) error(1);
    freeCollAll(classNos(p));
    if(! (p->classNos = newColl())) error(1);

    numVectors = size(outputs(p));
    for(i = 1L; i <= numVectors; i++) {
        if(! (classNo = (long *) malloc(sizeof(*classNo)))) error(1);
        v = (Vector) at(outputs(p), i);
        *classNo = classNoOfVec(map, v);
        if(*classNo == -1L) {
            /* vector v occurs for the first time at position i */
            *classNo = ++noCount;
            addMapping(map, v, *classNo);
            if(error) return;
        }     /* if */
        if(! add(classNos(p), classNo)) error(1);
    }       /* for */

    freeCollAll(map);             /* also frees the memory of the mapping structures */
    p->classCount = noCount;
    error = 0;
}         /* genClassNosFromVectors */


struct Mapping {                /* needed by classNoOfVec()        */
    Vector vec;                   /* and addMapping().                */
    long no;
};

/****************************************************/
/* Returns the class number of the output vector v. */
/* If v has not yet been assigned a class in coll,  */
/* returns -1L.                                     */
/****************************************************/
static long classNoOfVec(Collection coll, Vector v) {
    long index;
    struct Mapping *m;

    for(index = 1L; index <= size(coll); index++) {
        m = (struct Mapping *) at(coll, index);
        if(vcmp(v, m->vec) == V_EQUAL) return m->no;
    }
    return -1L;
}        /* classNoOfVec */


/****************************************************/
/* Adds the mapping of vector v to class number     */
/* classNo to the collection coll.                  */
/* Sets the global variable error.                  */
/****************************************************/
static void addMapping(Collection coll, Vector v, long classNo) {
    struct Mapping *m;

    m = (struct Mapping *) malloc(sizeof(*m));
    if(m == NULL) error(1);
    m->vec = v;
    m->no = classNo;
    if(! add(coll, m)) error(1);
}        /* addMapping */


/******************************************************/
/* Replaces the class names of p with the symbols     */
/* from the symtab st. st must contain at least as    */
/* many distinct symbols as the largest class number  */
/* of p.                                              */
/* Sets the global variable error.                    */
/******************************************************/
void replaceClasses(Patterns p, Symtab st) {
    long i, numClasses;
    Collection newClassColl, symbols;

    if(! (newClassColl = newColl())) error(1);
    symbols = sequence(st);
    numClasses = size(classNos(p));
    for(i = 1L; i <= numClasses; i++) {
        if(! add(newClassColl,
                 (char *) at(symbols, * (long *) (at(p->classNos, i))))) {
            freeColl(newClassColl);
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
/* Expands all input vectors of p with the corresponding  */
/* class vectors, if flag == TRUE. If flag == FALSE, the  */
/* output vectors are expanded instead.                   */
/* The class vector consists of classCount elements and   */
/* has a 1 in the i-th dimension when the corresponding   */
/* input vector has class number i. All other elements    */
/* are 0.                                                 */
/* Sets the global variable error.                        */
/**********************************************************/
void expandWithClassVectors(Patterns p, Boolean flag) {
    VecColl vc;
    Vector v, expandedVec;
    long clsNo, i, numVectors;

    vc = (flag ? inputs(p) : outputs(p));
    if(! (v = newVector(p->classCount))) error(1);
    putAllDim(v, 0);

    numVectors = size(vc);
    for(i = 1L; i <= numVectors; i++) {
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
/* Expands all input vectors of p with the         */
/* corresponding output vectors.                   */
/* Sets the global variable error.                 */
/***************************************************/
void expandWithOutputs(Patterns p) {
    long numVectors, i;
    Vector expandedVec;

    if(hasOutputs(p)) {
        numVectors = size(p->inputs);
        for(i = 1L; i <= numVectors; i++) {
            if(! (expandedVec =
                        expand((Vector) at(p->inputs, i), (Vector) at(p->outputs, i))))
                error(1);
            put(p->inputs, i, expandedVec);
        }
    }      /* if(hasOutputs(p)) */

    p->inputDims += p->outputDims;
    error = 0;
}               /* expandWithOutputs */







