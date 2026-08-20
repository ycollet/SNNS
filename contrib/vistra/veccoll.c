#include "xvis.h"
#include <math.h>


/*******************************************************/
/* Returns the number of row vectors of vc.            */
/*******************************************************/
long numberOfRows(VecColl vc) {
    return size(vc);
}         /* numberOfRows */


/*******************************************************/
/* Returns the number of column vectors of vc.         */
/*******************************************************/
long numberOfCols(VecColl vc) {
    return dims((Vector) at(vc, 1));
}         /* numberOfCols */


/*******************************************************/
/* Returns the number of constant dimensions of vc.    */
/* Writes the numbers of these dimensions in            */
/* ascending order into the array lp and puts a 0      */
/* at the end.                                         */
/*******************************************************/
unsigned constCols(VecColl vc, long *lp) {
    long i, nvecs, ndims;
    unsigned count;

    nvecs = size(vc);
    ndims = numberOfCols(vc);
    for(count = 0, i = 1L; i <= ndims; i++)
        if(dimMin(vc, i) == dimMax(vc, i)) lp[count++] = i;
    lp[count] = 0;

    return count;
}         /* constCols */


/*******************************************************/
/* Returns the average of the elements of all vectors, */
/* given by vc.                                        */
/*******************************************************/
Number overallAvg(VecColl vc) {
    long i, numVectors;
    Number sum = 0;

    numVectors = size(vc);
    for(i = 1L; i <= numVectors; i++)
        sum += avg((Vector) at(vc, i));

    return sum / numVectors;
}          /* overallAvg */


/*******************************************************/
/* Returns the smallest element of all vectors         */
/* in vc.                                              */
/*******************************************************/
Number overallMin(VecColl vc) {
    long numVectors, i;
    Number answer;

    numVectors = size(vc);
    answer = minimum((Vector) at(vc, 1));
    for(i = 2L; i <= numVectors; i++)
        answer = min(answer, minimum((Vector) at(vc, i)));
    return answer;
}          /* overallMin */


/*******************************************************/
/* Returns the largest element of all vectors          */
/* in vc.                                              */
/*******************************************************/
Number overallMax(VecColl vc) {
    long numVectors, i;
    Number answer;

    numVectors = size(vc);
    answer = maximum((Vector) at(vc, 1));
    for(i = 2L; i <= numVectors; i++)
        answer = max(answer, maximum((Vector) at(vc, i)));
    return answer;
}          /* overallMax */


/*******************************************************/
/* Returns the overall standard deviation of vc.       */
/*******************************************************/
Number overallStddev(VecColl vc) {
    long i, j, nvecs, ncols;
    unsigned long nnums;
    double totalAvg, totalVar, sum;
    Vector v;

    nvecs = size(vc);
    ncols = numberOfCols(vc);
    nnums = (unsigned long) nvecs * (unsigned long) ncols;
    totalAvg = overallAvg(vc);

    totalVar = 0;
    for(i = 1L; i <= nvecs; i++) {
        sum = 0;
        v = (Vector) at(vc, i);
        for(j = 1L; j <= ncols; j++) sum += square(atDim(v, j) - totalAvg);
        totalVar += sum / nnums;
    }

    return sqrt(totalVar);
}          /* overallStddev */


/*******************************************************/
/* Returns the average of the dim-th elements of all   */
/* vectors given by vc.                                */
/*******************************************************/
Number dimAvg(VecColl vc, long dim) {
    long i, numVectors;
    Number sum = 0;

    numVectors = size(vc);
    for(i = 1L; i <= numVectors; i++)
        sum += atDim((Vector) at(vc, i), dim);

    return sum / numVectors;
}          /* dimAvg */


/*******************************************************/
/* Returns the smallest element that any vector        */
/* in vc contains in dimension dim.                    */
/*******************************************************/
Number dimMin(VecColl vc, long dim) {
    long i, numVectors;
    Number answer;

    numVectors = size(vc);
    answer = atDim((Vector) at(vc, 1L), dim);
    for(i = 2L; i <= numVectors; i++)
        answer = min(answer, atDim((Vector) at(vc, i), dim));

    return answer;
}          /* dimMin */


/*******************************************************/
/* Returns the largest element that any vector         */
/* in vc contains in dimension dim.                    */
/*******************************************************/
Number dimMax(VecColl vc, long dim) {
    long i, numVectors;
    Number answer;

    numVectors = size(vc);
    answer = atDim((Vector) at(vc, 1L), dim);
    for(i = 2L; i <= numVectors; i++)
        answer = max(answer, atDim((Vector) at(vc, i), dim));

    return answer;
}          /* dimMax */


/*******************************************************/
/* Sets the i-th element of v to the scalar obtained   */
/* by applying the function vecFunc to the i-th row    */
/* vector of vc.                                       */
/* vecFunc must return a Number and may only take a    */
/* vector as its single parameter.                     */
/*******************************************************/
void collectRows(vc, vecFunc, v)
VecColl vc;
Number (*vecFunc)(Vector);
Vector v;
{
    long numVectors, i;

    numVectors = size(vc);
    for(i = 1L; i <= numVectors; i++)
        putDim(v, i, (*vecFunc)((Vector) at(vc, i)));
}        /* collectRows */


/*******************************************************/
/* Sets the i-th element of v to the scalar obtained   */
/* by applying the function vecFunc to the i-th        */
/* column vector of vc.                                */
/* vecFunc must return a Number and may only take a    */
/* vector as its single parameter.                     */
/*******************************************************/
void collectCols(vc, vecFunc, v)
VecColl vc;
Number (*vecFunc)(Vector);
Vector v;
{
    Vector col;
    long i, numCols;

    numCols = numberOfCols(vc);
    if(! (col = newVector(size(vc)))) handleErr(1);

    for(i = 1L; i <= numCols; i++) {
        colVec(vc, i, col);
        putDim(v, i, (*vecFunc)(col));
    }

    freeVector(col);
}        /* collectRows */


/*******************************************************/
/* Calls the function vecFunc for every row vector     */
/* of vc. vecFunc expects this vector as its only      */
/* parameter.                                          */
/*******************************************************/
void doRows(vc, vecFunc)
VecColl vc;
void (*vecFunc)(Vector);
{
    long i, numVectors;

    numVectors = numberOfRows(vc);
    for(i = 1L; i <= numVectors; i++)
        (*vecFunc)((Vector) at(vc, i));
}        /* doRows */


/*******************************************************/
/* Fills vector v with the dim-th column vector of     */
/* vc. The dimension of v must be greater than or      */
/* equal to the number of rows of vc. No check is      */
/* performed for this.                                 */
/*******************************************************/
void colVec(VecColl vc, long dim, Vector v) {
    long numVectors, i;

    numVectors = size(vc);
    for(i = 1L; i <= numVectors; i++)
        putDim(v, i, atDim((Vector) at(vc, i), dim));
}         /* colVec */


/*********************************************************/
/* Adds/subtracts/multiplies or divides each             */
/* row vector of vc with the constant at the             */
/* corresponding position in v.                          */
/* The operation is given by character c.                */
/* Allowed values of c: '+', '-', '*' or '/'.            */
/*********************************************************/
void compScalarsRow(VecColl vc, Vector v, char c) {
    long numVectors, i;

    numVectors = size(vc);
    switch(c) {
    case '+':
        for(i = 1L; i <= numVectors; i++)
            addConst((Vector) at(vc, i), atDim(v, i));
        break;
    case '-':
        for(i = 1L; i <= numVectors; i++)
            addConst((Vector) at(vc, i), - atDim(v, i));
        break;
    case '*':
        for(i = 1L; i <= numVectors; i++)
            multiply((Vector) at(vc, i), atDim(v, i));
        break;
    case '/':
        for(i = 1L; i <= numVectors; i++)
            multiply((Vector) at(vc, i), 1 / atDim(v, i));
        break;
    }       /* switch */
}         /* compScalarsRow */


/*********************************************************/
/* Adds/subtracts/multiplies or divides each             */
/* column vector of vc with the constant at the          */
/* corresponding position in v.                          */
/* The operation is given by character c.                */
/* Allowed values of c: '+', '-', '*' or '/'.            */
/*********************************************************/
void compScalarsCol(VecColl vc, Vector v, char c) {
    long numVectors, i;

    numVectors = size(vc);
    switch(c) {
    case '+':
        for(i = 1L; i <= numVectors; i++)
            addVector((Vector) at(vc, i), v);
        break;
    case '-':
        for(i = 1L; i <= numVectors; i++)
            subVector((Vector) at(vc, i), v);
        break;
    case '*':
        for(i = 1L; i <= numVectors; i++)
            multVector((Vector) at(vc, i), v);
        break;
    case '/':
        for(i = 1L; i <= numVectors; i++)
            divVector((Vector) at(vc, i), v);
        break;
    }       /* switch */
}         /* compScalarsCol */


/****************************************************/
/* Scales all elements of vc to the value range     */
/* from from to to.                                 */
/****************************************************/
void scaleAll(VecColl vc, Number from, Number to, ScaleOp *sop) {
    Range old, new;
    long numVectors, i;

    /* compute the previous value range */
    old.lowest = overallMin(vc);
    old.highest = overallMax(vc);

    new.lowest = from;
    new.highest = to;

    getScaleOp(sop, old, new);
    numVectors = size(vc);
    for(i = 1L; i <= numVectors; i++)
        scaleVec((Vector) at(vc, i), *sop);
}          /* scaleAll */


/*****************************************************/
/* Performs a Fast Fourier Transform on all          */
/* row vectors of vc.                                */
/* Sets the global variable error.                   */
/*****************************************************/
void fftRows(VecColl vc) {
    double *real, *imag;
    long nrows, ncols, i;

    nrows = numberOfRows(vc);
    ncols = numberOfCols(vc);
    if(! (real = (double *) malloc(ncols * sizeof(*real)))) error(1);
    if(! (imag = (double *) malloc(ncols * sizeof(*imag)))) {
        free(real);
        error(1);
    }

    for(i = 1L; i <= nrows; i++)
        fft((Vector) at(vc, i), real, imag);

    error = 0;
}


/*****************************************************/
/* Removes all row vectors of vc from row            */
/* from to row to inclusive.                         */
/*****************************************************/
void removeRowRange(VecColl vc, unsigned from, unsigned to) {
    freeFromTo(vc, from, to, (void(*)(void*))freeVector);
}          /* removeRowRange */


/*****************************************************/
/* Removes all column vectors of vc from column      */
/* from to column to inclusive.                      */
/* Sets the global variable error.                   */
/*****************************************************/
void removeColRange(VecColl vc, unsigned from, unsigned to) {
    unsigned numVectors, i;

    numVectors = size(vc);
    for(i = 1; i <= numVectors; i++) {
        removeDimRange((Vector) at(vc, i), from, to);
        if(error) return;
    }

    error = 0;
}          /* removeColRange */


/******************************************************/
/* Replaces the contents of the row-th row vector     */
/* with the contents of v.                            */
/******************************************************/
void replaceRow(VecColl vc, unsigned row, Vector v) {
    copyVec((Vector) at(vc, row), v);
}          /* replaceRow */


/******************************************************/
/* Replaces the contents of the col-th column vector  */
/* with the contents of v.                            */
/******************************************************/
void replaceCol(VecColl vc, unsigned col, Vector v) {
    unsigned nrows, i;

    nrows = size(vc);
    for(i = 1; i <= nrows; i++)
        putDim((Vector) at(vc, i), col, atDim(v, i));
}          /* replaceCol */


/******************************************************/
/* Writes the elements from from to to of the         */
/* col-th column vector of vc into buf.               */
/* Each element is on its own line of the string.     */
/******************************************************/
void colAsStringVert(VecColl vc, long col, long from, long to, char *buf) {
    long i;

    for(i = from; i <= to; i++) {
        sprintf(buf, NUMBER_FORMAT_NL, atDim((Vector) at(vc, i), col));
        buf += NUMBER_STR_LENGTH + SIZEOF_NL;
    }
    *(--buf) = '\0';
}          /* colAsStringVert */


/******************************************************/
/* Computes the covariance matrix of the vectors of   */
/* vc. The memory for the matrix must be allocated    */
/* by the caller, and a pointer must be passed via    */
/* the parameter covMatrix.                           */
/* Sets the global variable error.                    */
/******************************************************/
void covariance(VecColl vc, Number **covMatrix) {
    int i, j, k;
    int n = (int) numberOfCols(vc);
    int m = (int) numberOfRows(vc);
    Number *mean;
    Vector vec;

    /* compute the averages of the individual columns (dimensions) */
    if(! (mean = (Number *) malloc(sizeof(*mean) * n))) error(1);
    for(i = 0; i < n; i++)
        mean[i] = dimAvg(vc, (long) i+1);

    /* initialize covMatrix */
    for(i = 0; i < n; i++)
        for(j = 0; j <= i; j++)
            covMatrix[i][j] = 0.0;

    /* compute one half of the matrix */
    for(k = 0; k < m; k++) {
        vec = (Vector) at(vc, k+1);

        for(i = 0; i < n; i++)
            for(j = 0; j <= i; j++)
                covMatrix[i][j] += (atDim(vec, i+1) - mean[i]) *
                                   (atDim(vec, j+1) - mean[j]);
    }

    /* mirror the matrix across the diagonal */
    for(i = 0; i < n; i++) {
        for(j = 0; j < i; j++) {
            covMatrix[i][j] /= m;
            covMatrix[j][i] = covMatrix[i][j];
        }
        covMatrix[i][i] /= m;
    }

    free(mean);
    error = 0;
}          /* covariance */


