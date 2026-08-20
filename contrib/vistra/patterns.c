#include "xvis.h"
#include <ctype.h>
#include <time.h>
#include <math.h>

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
static char *printStatVals(char *, VecColl);

/*
 * A small open-hashing table that maps output Vectors to class numbers,
 * used to speed up the O(n * distinctClasses) linear scans that were
 * previously performed by genClassNosFromVectors() and writeSymtab().
 *
 * The hash function agrees with the vector equality test used elsewhere
 * (vcmp()/veq(), i.e. element-wise numeric equality): two vectors that
 * vcmp() considers V_EQUAL always land in the same bucket, so collisions
 * are the only thing that ever separates them and those are resolved by a
 * vcmp() comparison inside the bucket. The hash is derived from the numeric
 * (not bit-level) vector contents, so numerically equal values such as
 * -0.0 and 0.0 hash identically, matching veq(). Non-finite hash sums
 * (NaN/Inf) are routed to a single fixed bucket to stay consistent.
 */
struct VecHashNode {
    Vector vec;
    long no;
    struct VecHashNode *next;
};

struct VecHash {
    struct VecHashNode **buckets;
    long numBuckets;
};

static long vecHashBuckets(long n) {
    long b = 16L;
    while(b < n) b <<= 1;         /* smallest power of two >= max(16, n) */
    return b;
}

static long vecHashOf(Vector v, long numBuckets) {
    double s = 0.0;
    long i, dim = dims(v);
    union { double d; unsigned long long u; } conv;
    unsigned long long h;

    for(i = 1L; i <= dim; i++)
        s += (double) atDim(v, i) * (double) i;
    if(! isfinite(s)) return 0L;              /* NaN / Inf -> fixed bucket */
    conv.d = s;
    h = conv.u;
    h ^= h >> 32;
    h ^= h >> 16;
    return (long) (h & (unsigned long long) (numBuckets - 1L));
}

static struct VecHash *newVecHash(long n) {
    struct VecHash *h;
    long nb = vecHashBuckets(n), i;

    if(! (h = (struct VecHash *) malloc(sizeof(*h)))) return NULL;
    h->numBuckets = nb;
    if(! (h->buckets =
              (struct VecHashNode **) malloc(sizeof(struct VecHashNode *) * nb))) {
        free(h);
        return NULL;
    }
    for(i = 0L; i < nb; i++) h->buckets[i] = NULL;
    return h;
}

static void freeVecHash(struct VecHash *h) {
    long i;
    struct VecHashNode *n, *tmp;

    for(i = 0L; i < h->numBuckets; i++) {
        n = h->buckets[i];
        while(n) {
            tmp = n;
            n = n->next;
            free(tmp);
        }
    }
    free(h->buckets);
    free(h);
}

/* Returns the class number stored for v, or -1L if v is not present. */
static long vecHashLookup(struct VecHash *h, Vector v) {
    struct VecHashNode *n = h->buckets[vecHashOf(v, h->numBuckets)];

    while(n) {
        if(vcmp(v, n->vec) == V_EQUAL) return n->no;
        n = n->next;
    }
    return -1L;
}

/* Inserts the mapping v -> no. Returns 0 on success, -1 on out of memory. */
static int vecHashInsert(struct VecHash *h, Vector v, long no) {
    long b = vecHashOf(v, h->numBuckets);
    struct VecHashNode *n = (struct VecHashNode *) malloc(sizeof(*n));

    if(! n) return -1;
    n->vec = v;
    n->no = no;
    n->next = h->buckets[b];
    h->buckets[b] = n;
    return 0;
}

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
    VecColl randInputs = NULL, randOutputs = NULL;
    Collection randClasses = NULL, randClassNos = NULL, vColl = NULL;
    void **inArr = NULL, **outArr = NULL, **clsArr = NULL, **noArr = NULL;
    Number **vArr = NULL, *vVals = NULL;
    long *perm = NULL;
    long n, i, k;
    Boolean outs, names;

    n = p->count;
    outs = hasOutputs(p);
    names = hasClassNames(p);

    if(n <= 0L) {
        error = 0;
        return;
    }

    /*
     * Rather than repeatedly picking a random index and removing it from
     * the live collections (which resets the cursor cache and makes every
     * subsequent at() an O(n) traversal, i.e. O(n^2) overall), copy each
     * collection into a plain array with a single cache-friendly
     * sequential pass, build one random permutation with Fisher-Yates,
     * and rebuild the collections from the permuted arrays via O(1)
     * tail-insert add() calls.
     */
    perm  = (long *)   malloc((size_t) n * sizeof(*perm));
    inArr = (void **)  malloc((size_t) n * sizeof(*inArr));
    noArr = (void **)  malloc((size_t) n * sizeof(*noArr));
    if(outs)  outArr = (void **) malloc((size_t) n * sizeof(*outArr));
    if(names) clsArr = (void **) malloc((size_t) n * sizeof(*clsArr));
    if(! perm || ! inArr || ! noArr ||
            (outs && ! outArr) || (names && ! clsArr)) goto oom;

    if(! (randInputs = newColl())) goto oom;
    if(! (randClassNos = newColl())) goto oom;
    if(outs && ! (randOutputs = newColl())) goto oom;
    if(names && ! (randClasses = newColl())) goto oom;

    if(v) {
        if(! (vColl = asColl(v))) goto oom;
        vArr  = (Number **) malloc((size_t) n * sizeof(*vArr));
        vVals = (Number *)  malloc((size_t) n * sizeof(*vVals));
        if(! vArr || ! vVals) goto oom;
    }

    /* single sequential (cursor-cache friendly) pass into the arrays */
    for(i = 0L; i < n; i++) {
        inArr[i] = at(p->inputs, i + 1L);
        noArr[i] = at(p->classNos, i + 1L);
        if(outs)  outArr[i] = at(p->outputs, i + 1L);
        if(names) clsArr[i] = at(p->classes, i + 1L);
    }
    if(v)
        for(i = 0L; i < n; i++) vArr[i] = (Number *) at(vColl, i + 1L);

    /* build the identity permutation, then Fisher-Yates shuffle it */
    for(i = 0L; i < n; i++) perm[i] = i;
    srand48((long) time(NULL));
    for(i = n - 1L; i >= 1L; i--) {
        long j = (long) (drand48() * (double) (i + 1L));
        long tmp;
        if(j > i) j = i;               /* guard against drand48() == 1.0 */
        tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }

    /* rebuild the collections in permuted order (same perm keeps them aligned) */
    for(k = 0L; k < n; k++) {
        long idx = perm[k];
        if(! add(randInputs, inArr[idx])) goto oom;
        if(! add(randClassNos, noArr[idx])) goto oom;
        if(outs  && ! add(randOutputs, outArr[idx])) goto oom;
        if(names && ! add(randClasses, clsArr[idx])) goto oom;
    }
    if(v) {
        for(k = 0L; k < n; k++) vVals[k] = *vArr[perm[k]];
        for(k = 0L; k < n; k++) putDim(v, k + 1L, vVals[k]);
    }

    /* replace the old collections; element memory is retained (freeColl) */
    freeColl(p->inputs);
    p->inputs = randInputs;
    freeColl(p->classNos);
    p->classNos = randClassNos;
    if(outs) {
        freeColl(p->outputs);
        p->outputs = randOutputs;
    }
    if(names) {
        freeColl(p->classes);
        p->classes = randClasses;
    }

    if(v) {
        freeColl(vColl);
        free(vArr);
        free(vVals);
    }
    free(perm);
    free(inArr);
    free(noArr);
    if(outArr) free(outArr);
    if(clsArr) free(clsArr);

    error = 0;
    return;

oom:
    if(randInputs)   freeColl(randInputs);
    if(randOutputs)  freeColl(randOutputs);
    if(randClasses)  freeColl(randClasses);
    if(randClassNos) freeColl(randClassNos);
    if(vColl) freeColl(vColl);
    free(perm);
    free(inArr);
    free(noArr);
    if(outArr) free(outArr);
    if(clsArr) free(clsArr);
    if(vArr)  free(vArr);
    if(vVals) free(vVals);
    error(1);
}          /* randomize */


/********************************************/
/* Removes all patterns of p with numbers   */
/* from to to inclusive.                    */
/* Returns the number of removed patterns.  */
/* Sets the global variable error.          */
/********************************************/
unsigned removePatterns(Patterns p, unsigned from, unsigned to) {
    unsigned firstToRemove, lastToRemove, numToRemove;
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
    long nvecs, ncols, i, j;
    unsigned long nnums;
    Number omin, omax, vmin, vmax, avgSum, overallAvgVal, stddev;
    double totalAvg, totalVar, s;
    Vector vec;

    /*
     * Compute overall minimum, maximum and average in a single pass over
     * the collection (was 3 separate passes), then the overall standard
     * deviation in one further pass reusing the average already computed
     * (overallStddev() used to recompute overallAvg() itself, so this
     * replaces 5-6 traversals with 2). The numeric formulas and the float
     * accumulation used by overallMin/Max/Avg/Stddev are preserved exactly.
     */
    nvecs = size(vc);
    ncols = numberOfCols(vc);
    nnums = (unsigned long) nvecs * (unsigned long) ncols;

    vec = (Vector) at(vc, 1L);
    omin = minimum(vec);
    omax = maximum(vec);
    avgSum = 0;
    for(i = 1L; i <= nvecs; i++) {
        vec = (Vector) at(vc, i);
        vmin = minimum(vec);
        vmax = maximum(vec);
        omin = min(omin, vmin);
        omax = max(omax, vmax);
        avgSum += avg(vec);
    }
    overallAvgVal = avgSum / nvecs;      /* same value overallAvg() yields */
    totalAvg = overallAvgVal;

    totalVar = 0;
    for(i = 1L; i <= nvecs; i++) {
        s = 0;
        vec = (Vector) at(vc, i);
        for(j = 1L; j <= ncols; j++) s += square(atDim(vec, j) - totalAvg);
        totalVar += s / nnums;
    }
    stddev = sqrt(totalVar);             /* rounded to Number, as before */

    sprintf(cp, "Overall minimum:     ");
    cp += ROW_TITLE_LEN;
    sprintf(cp, NUMBER_FORMAT_L_NL, omin);
    cp += NUMBER_STR_LENGTH + SIZEOF_NL;
    sprintf(cp, "Overall maximum:     ");
    cp += ROW_TITLE_LEN;
    sprintf(cp, NUMBER_FORMAT_L_NL, omax);
    cp += NUMBER_STR_LENGTH + SIZEOF_NL;
    sprintf(cp, "Overall average:     ");
    cp += ROW_TITLE_LEN;
    sprintf(cp, NUMBER_FORMAT_L_NL, overallAvgVal);
    cp += NUMBER_STR_LENGTH + SIZEOF_NL;
    sprintf(cp, "Overall std.dev.:    ");
    cp += ROW_TITLE_LEN;
    sprintf(cp, NUMBER_FORMAT_L_NL, stddev);
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
    long i, nvecs, diffVecs;
    Collection set;
    struct VecHash *seen;
    Vector v;

    if(hasClassNames(p)) {
        fprintSymbols(p->symtab, f);
        if(error) return;
    } else {          /* write the output vectors as symbols */
        if(hasOutputs(p)) {
            if(! (set = newColl())) error(1);
            nvecs = size(p->outputs);
            /*
             * Use a hashed set to detect duplicate output vectors in O(1)
             * instead of the former O(n) detectPos() scan. The ordered
             * "set" collection still records first-occurrence order so the
             * output written below is identical to before.
             */
            if(! (seen = newVecHash(nvecs))) {
                freeColl(set);
                error(1);
            }
            for(i = 1L; i <= nvecs; i++) {
                v = (Vector) at(p->outputs, i);
                if(vecHashLookup(seen, v) == -1L) {
                    if(vecHashInsert(seen, v, 1L) != 0) {
                        freeVecHash(seen);
                        freeColl(set);
                        error(1);
                    }
                    if(! add(set, v)) {
                        freeVecHash(seen);
                        freeColl(set);
                        error(1);
                    }
                }
            }     /* for */
            freeVecHash(seen);
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
        /*
         * The class name at position i is a canonical pointer owned by the
         * symtab, so its 1-based position in sequence(symtab) is exactly
         * what indexOf(namesOrder, ...) used to return by a linear scan.
         * symbolIndex() looks it up through the symtab hash in O(1).
         */
        *classNo = symbolIndex(p->symtab, (char *) at(classes(p), i));
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
    struct VecHash *map;

    numVectors = size(outputs(p));
    if(! (map = newVecHash(numVectors))) error(1);
    freeCollAll(classNos(p));
    if(! (p->classNos = newColl())) {
        freeVecHash(map);
        error(1);
    }

    for(i = 1L; i <= numVectors; i++) {
        if(! (classNo = (long *) malloc(sizeof(*classNo)))) {
            freeVecHash(map);
            error(1);
        }
        v = (Vector) at(outputs(p), i);
        /*
         * O(1) hashed lookup instead of the former linear map scan. The
         * hash agrees with vcmp()/veq(), and first-seen order is preserved
         * exactly: the first occurrence of a distinct vector gets the next
         * consecutive number, identical to the old linear-scan behaviour.
         */
        *classNo = vecHashLookup(map, v);
        if(*classNo == -1L) {
            /* vector v occurs for the first time at position i */
            *classNo = ++noCount;
            if(vecHashInsert(map, v, *classNo) != 0) {
                free(classNo);
                freeVecHash(map);
                error(1);
            }
        }     /* if */
        if(! add(classNos(p), classNo)) {
            free(classNo);
            freeVecHash(map);
            error(1);
        }
    }       /* for */

    freeVecHash(map);
    p->classCount = noCount;
    error = 0;
}         /* genClassNosFromVectors */


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







