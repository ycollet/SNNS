#include "xvis.h"
#include <math.h>


/**************************************************/
/* Returns a new vector of dimension dim.         */
/* If not enough memory is available for this,    */
/* returns NULL.                                  */
/**************************************************/
Vector newVector(long dim) {
    Vector vec;
    Number *array;

    if(! (vec = (Vector) malloc(sizeof(*vec)))) return NULL;
    if(! (array = (Number *) malloc(dim * sizeof(Number)))) {
        free(vec);
        return NULL;
    }
    vec->dimensions = dim;
    vec->elements = array;

    return vec;
}                   /* newVector */


/**************************************************/
/* Frees the memory for vector v.                 */
/**************************************************/
void freeVector(Vector v) {
    free(v->elements);
    free(v);
}                   /* freeVector */


/**************************************************/
/* Writes vector v to file f. If an error occurs, */
/* returns a value <> 0, otherwise 0.             */
/**************************************************/
int fprintVector(Vector v, FILE *f) {
    long i;

    for(i = 0L; i < v->dimensions-1; i++)
        if(fprintf(f, "%g ", v->elements[i]) < 0) return 1;
    if( fprintf(f, "%g", v->elements[v->dimensions-1]) < 0) return 1;
    return 0;
}              /* fprintVector */


/**************************************************/
/* Returns the dimension of vector v.             */
/**************************************************/
long dims(Vector v) {
    return v->dimensions;
}             /* dim */


/**************************************************/
/* Returns the i-th dimension of vector v.        */
/**************************************************/
Number atDim(Vector v, long i) {
    return v->elements[i - 1];
}             /* atDim */


/**************************************************/
/* Sets the i-th dimension of vector v to the     */
/* value val.                                     */
/**************************************************/
void putDim(Vector v, long i, Number val) {
    v->elements[i - 1] = val;
}            /* putDim */


/**************************************************/
/* Sets every dimension of vector v to the        */
/* value val.                                     */
/**************************************************/
void putAllDim(Vector v, Number val) {
    long numDims, i;

    numDims = dims(v);
    for(i = 0L; i < numDims; i++)
        v->elements[i] = val;
}            /* putAllDim */


/**************************************************/
/* Returns the smallest element of v.             */
/**************************************************/
Number minimum(Vector v) {
    long numDims, i;
    Number answer;

    answer = v->elements[0];
    numDims = dims(v);
    for(i = 1L; i < numDims; i++)
        answer = min(answer, v->elements[i]);
    return answer;
}            /* minimum */


/**************************************************/
/* Returns the largest element of v.              */
/**************************************************/
Number maximum(Vector v) {
    long numDims, i;
    Number answer;

    answer = v->elements[0];
    numDims = dims(v);
    for(i = 1L; i < numDims; i++)
        answer = max(answer, v->elements[i]);
    return answer;
}            /* maximum */


/**************************************************/
/* Returns the sum of vector v.                   */
/**************************************************/
Number sum(Vector v) {
    Number answer;
    long i, numDims;

    answer = 0;
    numDims = dims(v);
    for(i = 1L; i <= numDims; i++) answer += atDim(v, i);
    return answer;
}        /* sum */


/**************************************************/
/* Returns the average of vector v.               */
/**************************************************/
Number avg(Vector v) {
    return sum(v) / dims(v);
}        /* avg */


/**************************************************/
/* Compares vectors v1 and v2. If v1<v2, returns  */
/* V_LESS; if they are equal, returns V_EQUAL;    */
/* and if v1>v2, returns V_GREATER.               */
/* v1 < v2 iff there exists i such that for all   */
/* j<i (j>=1) atDim(v1, i)=atDim(v2, i) and       */
/* atDim(v1, j) < atDim(v2, j).                   */
/* Vectors v1 and v2 must have the same           */
/* dimensionality. No check is performed for      */
/* this.                                          */
/**************************************************/
int vcmp(Vector v1, Vector v2) {
    long dimensions, j;
    Number e1, e2;

    dimensions = dims(v1);
    for(j = 1L; j <= dimensions; j++) {
        e1 = atDim(v1, j);
        e2 = atDim(v2, j);
        if(e1 < e2) return V_LESS;
        else if(e1 > e2) return V_GREATER;
    }
    return V_EQUAL;
}          /* vcmp */


/***************************************************/
/***************************************************/
Boolean veq(Vector v1, Vector v2) {
    return vcmp(v1, v2) == V_EQUAL;
}          /* veq */


/**************************************************/
/* Returns the length of vector v.                */
/**************************************************/
Number length(Vector v) {
    Number sum = 0;
    long i, dim;

    dim = dims(v);
    for(i = 0L; i < dim; i++)
        sum += square(v->elements[i]);

    return sqrt(sum);
}          /* length */


/**************************************************/
/* Multiplies every element of v by n.            */
/**************************************************/
void multiply(Vector v, Number n) {
    long i, dim;

    dim = dims(v);
    for(i = 0L; i < dim; i++)
        v->elements[i] = n * v->elements[i];
}          /* multiply */


/**************************************************/
/* Multiplies every element of v2 with the        */
/* corresponding element of v1 and stores the     */
/* result in v1. v2 remains unchanged.            */
/* The dimensions of v1 and v2 must match; no     */
/* check is performed for this.                   */
/**************************************************/
void multVector(Vector v1, Vector v2) {
    long i, dim;

    dim = dims(v1);
    for(i = 0L; i < dim; i++)
        v1->elements[i] *= v2->elements[i];
}          /* multVector */


/**************************************************/
/* Divides every element of v1 by the             */
/* corresponding element of v2 and stores the     */
/* result in v1. v2 remains unchanged.            */
/* The dimensions of v1 and v2 must match; no     */
/* check is performed for this.                   */
/**************************************************/
void divVector(Vector v1, Vector v2) {
    long i, dim;

    dim = dims(v1);
    for(i = 0L; i < dim; i++)
        v1->elements[i] /= v2->elements[i];
}          /* divVector */


/**************************************************/
/* Adds n to every element of vector v.           */
/**************************************************/
void addConst(Vector v, Number n) {
    long i, dim;

    dim = dims(v);
    for(i = 0L; i < dim; i++)
        v->elements[i] += n;
}        /* addConst */


/**************************************************/
/* Adds vector v2 element-wise to vector v1.      */
/* v1 subsequently holds the sum, v2 remains      */
/* unchanged.                                     */
/* v1 and v2 must have the same dimension; no     */
/* check is performed for this.                   */
/**************************************************/
void addVector(Vector v1, Vector v2) {
    long i, dim;

    dim = dims(v1);
    for(i = 0L; i < dim; i++)
        v1->elements[i] += v2->elements[i];
}        /* addVector */


/*****************************************************/
/* Subtracts vector v2 element-wise from vector      */
/* v1. v1 subsequently holds the result, v2          */
/* remains unchanged.                                */
/* v1 and v2 must have the same dimension; no        */
/* check is performed for this.                      */
/*****************************************************/
void subVector(Vector v1, Vector v2) {
    long i, dim;

    dim = dims(v1);
    for(i = 0L; i < dim; i++)
        v1->elements[i] -= v2->elements[i];
}        /* subVector */


/**************************************************/
/* Negates all elements of v.                     */
/**************************************************/
void neg(Vector v) {
    long i, dim;

    dim = dims(v);
    for(i = 0L; i < dim; i++)
        v->elements[i] = - (v->elements[i]);
}        /* neg */


/**************************************************/
/* Replaces all elements of v with their          */
/* reciprocal. It is not checked whether an       */
/* element is 0.                                  */
/**************************************************/
void invert(Vector v) {
    long i, dim;

    dim = dims(v);
    for(i = 0L; i < dim; i++)
        v->elements[i] = 1 / (v->elements[i]);
}        /* invert */


/**************************************************/
/* Returns the variance of vector v.              */
/**************************************************/
Number var(Vector v) {
    double average, sum = 0;
    long dim, i;

    average = avg(v);
    dim = dims(v);
    for(i = 0; i < dim; i++)
        sum += square(v->elements[i] - average);

    return sum / dim;
}          /* var */


/**************************************************/
/* Returns the standard deviation of vector v.    */
/**************************************************/
Number sigma(Vector v) {
    return sqrt(var(v));
}        /* sigma */


/**************************************************/
/* Scales vector v using so.                      */
/**************************************************/
void scaleVec(Vector v, ScaleOp so) {
    long dim, i;

    dim = dims(v);
    for(i = 0L; i < dim; i++)
        v->elements[i] = scale(so, v->elements[i]);
}        /* scaleVec */


/**************************************************/
/* Normalizes vector v to length 1.               */
/**************************************************/
void normalize(Vector v) {
    Number len;

    len = length(v);
    if(len != 0.0) multiply(v, 1 / len);
}        /* normalize */


/**************************************************/
/* Returns the position of the first occurrence   */
/* of n in vector v. Returns 0L if n is not       */
/* contained as an element of v.                  */
/**************************************************/
long detectFirst(Vector v, Number n) {
    long i, dim;

    dim = dims(v);
    for(i = 0L; i < dim; i++)
        if(v->elements[i] == n) return i+1;

    return 0L;
}        /* detectFirst */


/**************************************************/
/* Writes a string to buf that contains exactly   */
/* one element of v per line.                     */
/**************************************************/
void asStringVert(Vector v, char *buf) {
    long dim, i;

    dim = dims(v);
    for(i = 0L; i < dim; i++) {
        sprintf(buf, NUMBER_FORMAT_NL, v->elements[i]);
        buf += NUMBER_STR_LENGTH + SIZEOF_NL;
    }
    *(--buf) = '\0';
}        /* asStringVert */


/**************************************************/
/* Copies all elements of coll into vector v.     */
/* The size of coll and the number of dimensions  */
/* of v must be identical. No check is performed  */
/* for this.                                      */
/**************************************************/
void copyFromColl(Vector v, Collection coll) {
    long i;

    for(i = 1L; i <= size(coll); i++)
        putDim(v, i, *(Number *) at(coll, i));
}           /* copyFromColl */


/******************************************************/
/* Copies the elements of v2 to v1.                   */
/******************************************************/
void copyVec(Vector v1, Vector v2) {
    long i;

    for(i = 0L; i < v2->dimensions; i++)
        v1->elements[i] = v2->elements[i];
}           /* copyVec */


/******************************************************/
/* Returns vector v as a collection of its            */
/* elements (Numbers).                                */
/* Returns NULL if there is not enough memory for     */
/* the collection.                                    */
/******************************************************/
Collection asColl(Vector v) {
    Collection answer;
    long i, dim;

    if(! (answer = newColl())) return NULL;
    dim = dims(v);
    for(i = 0L; i < dim; i++)
        if(! add(answer, &(v->elements[i]))) {
            freeColl(answer);
            return NULL;
        }

    return answer;
}           /* asColl */


/******************************************************/
/* Returns a new vector that has dim1 + dim2          */
/* dimensions, where dim1 is the dimensionality of    */
/* vector v1 and dim2 that of vector v2.              */
/* The new vector's elements are the elements of v1   */
/* followed by those of v2.                           */
/* Returns NULL if not enough memory is available     */
/* for the vector to be returned.                     */
/******************************************************/
Vector expand(Vector v1, Vector v2) {
    Vector answer;
    long i, dim1, dim2;

    dim1 = dims(v1);
    dim2 = dims(v2);
    answer = newVector(dim1 + dim2);
    if(! answer) return NULL;

    /* copy elements of v1 into answer */
    for(i = 0L; i < dim1; i++)
        answer->elements[i] = v1->elements[i];
    /* copy elements of v2 into answer */
    for(i = dim1; i < dim1+dim2; i++)
        answer->elements[i] = v2->elements[i - dim1];

    return answer;
}           /* expand */


/****************************************************/
/* Removes all dimensions given by dims from        */
/* vector v.                                        */
/* ds points to an array of integers of type long.  */
/* Each element of the array specifies a dimension  */
/* to be removed. The elements of ds must be sorted */
/* in ascending order. E.g. (1 3 4) means that      */
/* dimensions 1, 3 and 4 are to be removed.         */
/* To mark the end of the array, a 0L must appear   */
/* there.                                            */
/* num gives the number of dimensions to be         */
/* removed.                                          */
/* Sets the global variable error.                  */
/****************************************************/
void removeDims(Vector v, long *ds, long num) {
    long i, dim, outCount;
    Number *newElems;

    dim = dims(v);
    if(! (newElems = (Number *) malloc(sizeof(*newElems) * (dim-num))))
        error(1);

    outCount = 0L;
    for(i = 1L; i <= dim; i++)
        if(i == *ds) ds++;
        else newElems[outCount++] = v->elements[i-1];

    free(v->elements);
    v->elements = newElems;
    v->dimensions = outCount;
    error = 0;
}           /* removeDims */


/******************************************************/
/* Removes all dimensions from from to to inclusive   */
/* from vector v.                                     */
/* Sets the global variable error.                    */
/******************************************************/
void removeDimRange(Vector v, unsigned from, unsigned to) {
    unsigned i, numToRemove, firstToRemove, lastToRemove, count;
    Number *newElems;

    if((from<=to) && (from<=v->dimensions) && (to>=1)) {
        firstToRemove = max(1, from);
        lastToRemove = min(v->dimensions, to);
        numToRemove = lastToRemove - firstToRemove + 1;
        newElems = (Number *) malloc((v->dimensions-numToRemove) * sizeof(*newElems));
        if(! newElems) error(1);

        count = 0;
        for(i = 0; i < firstToRemove-1; i++)
            newElems[count++] = v->elements[i];
        for(i = lastToRemove; i < v->dimensions; i++)
            newElems[count++] = v->elements[i];

        free(v->elements);
        v->elements = newElems;
        v->dimensions -= numToRemove;
    }

    error = 0;
}           /* removeDimRange */


/************************************************************/
/* Performs a Fast Fourier Transform on vector v.           */
/* Two arrays of doubles must be passed to the function,    */
/* whose memory has already been allocated and which have  */
/* room for at least as many elements as the number of      */
/* dimensions of v.                                         */
/************************************************************/
void fft(Vector v, double *VecRe, double *VecIm) {
    int Size, Forward;
    long ii;
    int LenHalf,Stage,But,ButHalf;
    int i,j=1,k,ip,pot2=0;
    double ArcRe,ArcIm,dArcRe,dArcIm,ReBuf,ImBuf,ArcBuf,Arc;

    Size = dims(v);
    Forward = 1;
    for(ii = 0L; ii < Size; ii++) {
        VecRe[ii] = (double) v->elements[ii];
        VecIm[ii] = 0.0;
    }

    while(Size!=(1<<pot2)) {
        pot2++;
    }

    LenHalf = Size >> 1;
    for(i=1; i<Size; i++) {
        if(i<j) {
            ReBuf = VecRe[j-1];
            ImBuf = VecIm[j-1];
            VecRe[j-1] = VecRe[i-1];
            VecIm[j-1] = VecIm[i-1];
            VecRe[i-1] = ReBuf;
            VecIm[i-1] = ImBuf;
        }
        k = LenHalf;
        while(k<j) {
            j -= k;
            k = k >> 1;
        }
        j += k;
    }
    for(Stage=1; Stage<=pot2; Stage++) {
        But = 1 << Stage;
        ButHalf = But >> 1;
        ArcRe = 1.0;
        ArcIm = 0.0;
        Arc = (double)(PI/ButHalf);
        dArcRe = (double)cos(Arc);
        dArcIm = (double)sin(Arc);

        if(Forward) {
            dArcIm = -dArcIm;
        }

        for(j=1; j<=ButHalf; j++) {
            i = j;
            while(i<=Size) {
                ip = i + ButHalf;
                ReBuf = ((VecRe[ip-1] * ArcRe) - (VecIm[ip-1] * ArcIm));
                ImBuf = ((VecRe[ip-1] * ArcIm) + (VecIm[ip-1] * ArcRe));
                VecRe[ip-1] = VecRe[i-1] - ReBuf;
                VecIm[ip-1] = VecIm[i-1] - ImBuf;
                VecRe[i-1] = VecRe[i-1] + ReBuf;
                VecIm[i-1] = VecIm[i-1] + ImBuf;
                i += But;
            }
            ArcBuf = ArcRe;
            ArcRe  = ArcRe * dArcRe - ArcIm * dArcIm;
            ArcIm  = ArcBuf * dArcIm + ArcIm * dArcRe;
        }
    }
    if(Forward) {
        for(j=1; j<=Size; j++) {
            VecRe[j-1] /= Size;
            VecIm[j-1] /= Size;
        }
    }

    for(ii = 0L; ii < v->dimensions; ii++)
        v->elements[ii] = (Number) sqrt(VecRe[ii]*VecRe[ii] + VecIm[ii]*VecIm[ii]);
}         /* fft */


/*************************************************************/
/* Performs an hlog operation on vector vec.                 */
/*************************************************************/
void hlog(Vector vec) {
    Number *hilfelems;
    int BreiteHoehe,x,y,u,v;
    double BreiteHoeheHalbe,StepRadius,StepAngel;
    double Radius,Angel;

    /* HilfArray =(double **)calloc(BreiteHoehe,sizeof(double *));
     for(y=0;y<BreiteHoehe;y++){
       HilfArray[y] = (double *)calloc(BreiteHoehe,sizeof(double));
     }
     Field = *Array; */

    BreiteHoehe = (int) sqrt((double) vec->dimensions);
    hilfelems = (Number *) malloc(vec->dimensions * sizeof(*hilfelems));
    if(! hilfelems) handleErr(1);

    BreiteHoeheHalbe = BreiteHoehe/2.0;
    StepRadius = exp(log(BreiteHoeheHalbe)/(BreiteHoehe-1));
    StepAngel = 2.0 * PI / BreiteHoehe;

    for(v=0; v<BreiteHoehe; v++) {
        for(u=0; u<BreiteHoehe; u++) {
            Radius = pow(StepRadius,(double)v);
            Angel  = StepAngel * u;
            x = BreiteHoeheHalbe + Radius * cos(Angel);
            x = min(x,(BreiteHoehe-1));
            y = BreiteHoeheHalbe + Radius * sin(Angel);
            y = min(y,(BreiteHoehe-1));
            hilfelems[u*BreiteHoehe+v] = vec->elements[y*BreiteHoehe+x];
        }
    }

    free(vec->elements);
    vec->elements = &hilfelems[0];
}       /* hlog */


/*******************************************************/
/* Writes all elements of v one after another into the */
/* buffer buf. The elements are separated from each    */
/* other by numBlanks blanks.                          */
/* Returns a pointer to the first character after the  */
/* written string.                                     */
/*******************************************************/
char *printVec(Vector v, char *buf, unsigned numBlanks) {
    long i;
    char *cp = buf;

    for(i = 0L; i < v->dimensions - 1; i++) {
        sprintf(cp, NUMBER_FORMAT, v->elements[i]);
        cp += NUMBER_STR_LENGTH;
        cp = printnchr(cp, numBlanks, ' ');
    }
    sprintf(cp, NUMBER_FORMAT, v->elements[v->dimensions - 1]);
    return cp + NUMBER_STR_LENGTH;
}           /* printVec */


