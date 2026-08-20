#ifndef _XVIS_H
#define _XVIS_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*-------------------------------------------------------------------*/
/* CONSTANTS                                                         */
/*-------------------------------------------------------------------*/

#define SIZEOF_NL                  1
#define MAX_VAL(type)              ((type) (1 << (sizeof(type) * 8 - 1)))
#define V_LESS                     -1        /* return value of vcmp() */
#define V_GREATER                  1         /*          "             */
#define V_EQUAL                    0         /*          "             */
#define PI                         3.141592
#define NUM_COLORS                 100
/* number of distinct color levels */
#define NUM_GRAYS                  50
/* number of distinct gray levels */
#define MAX_LENGTH_ERROR_MESSAGE   512
/* max. length of an error message */
#define MAX_LENGTH_ERRORINFO       256
/* max. length of the global variable errorInfo */
#define MAX_LENGTH_TOKENVAL        1024
/* max. length of the global variable tokenval */
#define CLASS_MAXLEN               80
/* max. length of a class name */
#define CLASS_SCANF_FORMAT         "%80s"
/* scanf() format for reading classes */
#define NO_BUCKETS                 100
/* number of buckets for class name hashing */
#define FORMAT_DIR_ENVVAR          "VISTRAFORMATS"
/* Name of the environment variable that      */
/* specifies the directory in which the FDL   */
/* format files are to be found.              */
#define FORMAT_EXTENSION           ".fmt"
#define TEMP_DIR                   "/tmp/"

#define NUMBER_STR_LENGTH          10
#define NUMBER_FORMAT              "%10.4g"
#define NUMBER_FORMAT_NL           "%10.4g\n"
#define NUMBER_FORMAT_L            "%-10.4g"
#define NUMBER_FORMAT_L_NL         "%-10.4g\n"
#define CLASS_FORMAT               "%-80s"
#define CLASSNO_FORMAT             "%6ld"
#define DIM_STR_MAXLEN             6
#define NUM_OF_PATS_STR_MAXLEN     6
#define NUM_OF_PATS_FORMAT         "%6u"
#define NUM_OF_PATS_FORMAT_NL      "%6u\n"
#define NUM_OF_PATS_FORMAT_L       "%-6u"
#define NUM_OF_PATS_FORMAT_L_NL    "%-6u\n"


/*--------------------------------------------------------------------*/
/* MACROS                                                             */
/*--------------------------------------------------------------------*/

/* Check the return value of ftell() */
#define checkPos(pos)  if((pos) == -1L) { error = 6; return; }
/* Same, for use inside a function that returns a value on error paths */
#define checkPosR(pos,retval)  if((pos) == -1L) { error = 6; return (retval); }

/* Set the file pointer of file f to position pos, checking for errors */
#define setPos(f,pos)  if(fseek((f), (pos), 0)) { error = 6; return; }
/* Same, for use inside a function that returns a value on error paths */
#define setPosR(f,pos,retval)  if(fseek((f), (pos), 0)) { error = 6; return (retval); }

/* Terminate the function and set the error code nr */
#define error(nr)      { error = (nr); return; }
/* Same, for use inside a function that returns a value on error paths */
#define errorR(nr,retval)      { error = (nr); return (retval); }

/* Compute the value of n scaled by so (a ScaleOp structure) */
#define scale(so, n)      ((n) * (so).mult + (so).add)

#define max(a,b)  ((a) >= (b) ? (a) : (b))
#define min(a,b)  ((a) <= (b) ? (a) : (b))
#define square(n) ((n) * (n))

/*--------------------------------------------------------------------*/
/* TYPE DECLARATIONS                                                  */
/*--------------------------------------------------------------------*/

typedef float Number;

enum Token {
    inputDim,
    outputDim,
    numPatterns,
    input,
    output,
    class,
    questionMark,
    newLine,
    writeNewLine,
    aString,
    asterisk,
    altBegin,
    altEnd,
    or,
    loopBegin,
    loopEnd,
    unknownControl,
    endOfString,
    nil
};

struct Member {
    void *data;
    struct Member *next;
    struct Member *previous;
};

struct CollType {
    long count;
    struct Member *first;
    struct Member *last;
    struct Member *lastVisited;
    long lastIndex;
};
typedef struct CollType * Collection;
typedef Collection VecColl;

struct VectorType {
    long dimensions;
    Number *elements;
};
typedef struct VectorType * Vector;

struct FormatType {
    char *contents;
    char *pos;
};
typedef struct FormatType * Format;

struct Node {
    char *name;
    long index;               /* 1-based position of name in the order list */
    struct Node *next;
};

struct SymtabType {
    struct Node **buckets;
    long numBuckets;
    Collection order;
};
typedef struct SymtabType * Symtab;

struct PatternsType {
    long count;
    long inputDims;
    long outputDims;
    VecColl inputs;
    VecColl outputs;
    Collection classes;
    Collection classNos;
    long classCount;
    Symtab symtab;
};
typedef struct PatternsType * Patterns;

typedef struct {
    int originX;              /* X coordinate of the origin relative to x */
    int originY;              /* Y coordinate  "      "     "    "     "  */
    float x_mult;             /* number of pixels for one X length unit   */
    float y_mult;             /*   "      "     "    "    Y      "     "  */
    int x;
    int y;
    unsigned width;
    unsigned height;
} CoordSystem;

typedef unsigned long PixelNo;

typedef struct {
    float mult;
    float add;
} ScaleOp;

typedef struct {
    Number lowest;
    Number highest;
} Range;

enum Graphic {
    proj2D = 0,
    histo = 1,
    poly = 2,
    clrMat = 3,
    grayMat = 4,
    noKind = 5,
};

typedef struct {
    Widget shell;
    Widget commForm;
    Widget infoForm;
    Widget core;
    unsigned displayed;
    Boolean input;
    enum Graphic kind;
    Pixmap pm;
    unsigned pmWidth;
    unsigned pmHeight;
    unsigned curWidth;
    float size;
    unsigned widthPerVector;
    unsigned heightPerVector;
    Number minInputs;
    Number maxInputs;
    Number minOutputs;
    Number maxOutputs;
    time_t patVersion;
    int arg1;
    int arg2;
} * GW;

typedef struct {
    Widget shell;
    Widget rowTitles;
    Widget *colTitles;
    Widget *cols;
    Widget scrollVert;
    Widget scrollHoriz;
    unsigned firstRow;
    unsigned firstCol;
    unsigned visRows;
    unsigned visCols;
    unsigned usedRows;
    unsigned usedCols;
} * SSW;

/*---------------------------------------------------------------------*/
/* PUBLIC FUNCTION HEADERS                                             */
/*---------------------------------------------------------------------*/

/* Public functions of COLL.O */
Collection newColl();
void freeColl(Collection);
void freeCollAll(Collection);
void freeDeep(Collection coll, void (*func)(void *));
Collection add(Collection, void *);
void *at(Collection, long);
void put(Collection, long, void *);
Collection rmv(Collection, void *);
Collection removeAt(Collection, long);
Collection removeComplete(Collection, long);
Collection removeFromTo(Collection, long, long);
Collection freeFromTo(Collection coll, long from, long to, void (*func)(void *));
long indexOf(Collection, void *);
long detectPos(Collection coll, void *search, Boolean (*equals)(void *, void *));
long size(Collection);
Boolean isEmpty(Collection);
Boolean notEmpty(Collection);

/* Public functions of FORMAT.O */
Format newFormat(FILE *);                      /* sets error */
void freeFormat(Format);
long getPosition(Format);
void setPosition(Format, long);
enum Token lookUp(Format);
enum Token lookUpNextDesc(Format, long);
enum Token nextToken(Format);
void checkSyntax(Format);                      /* sets error */
long posAfter(Format, enum Token);
Boolean isDesc(enum Token);

/* Public functions of PATTERNS.O */
Patterns newPatterns();                        /* sets error */
void fileIn(Patterns, Format, FILE *);         /* sets error */
void fileOut(Patterns, Format, FILE *);        /* sets error */
void freePatterns(Patterns);
unsigned removePatterns(Patterns, unsigned, unsigned);
/* sets error */
unsigned removeCols(Patterns, Boolean, unsigned, unsigned);
/* sets error */
void removeDimList(Patterns, Boolean, long *, long);
/* sets error */
void randomize(Patterns, Vector);              /* sets error */
long num(Patterns);
VecColl inputs(Patterns);
VecColl outputs(Patterns);
Collection classes(Patterns);
Collection classNos(Patterns);
void genClassNos(Patterns);                    /* sets error */
void genClassNosFromNames(Patterns);           /* sets error */
void genClassNosFromVectors(Patterns);         /* sets error */
void replaceClasses(Patterns, Symtab);         /* sets error */
long maxClassNo(Patterns);
char *classString(Patterns, long);
char *statString(Patterns, char *);
void writeSymtab(Patterns, FILE *);            /* sets error */
long inputDims(Patterns);
long outputDims(Patterns);
Boolean hasClassNames(Patterns);
Boolean hasOutputs(Patterns);
void expandWithClassVectors(Patterns, Boolean);/* sets error */
void expandWithOutputs(Patterns);              /* sets error */
/* N01.C */
Patterns readN01(FILE *);                      /* sets error */
void writeN01(Patterns, FILE *);               /* sets error */
/* LVQ.C */
Patterns readLVQ(FILE *);                      /* sets error */
void writeLVQ(Patterns, FILE *);               /* sets error */
void lvqRead(Patterns, FILE *, Boolean);       /* sets error */
void lvqWrite(Patterns, FILE *, Boolean);      /* sets error */

/* Public functions of VECTOR.O */
Vector newVector(long);
void freeVector(Vector);
int fprintVector(Vector, FILE *);
long dims(Vector);
Number atDim(Vector, long);
void putDim(Vector, long, Number);
void putAllDim(Vector, Number);
int vcmp(Vector, Vector);
Boolean veq(Vector, Vector);
long detectFirst(Vector, Number);
Number minimum(Vector);
Number maximum(Vector);
Number sum(Vector);
Number avg(Vector);
Number length(Vector);
Number var(Vector);
Number sigma(Vector);
void neg(Vector);
void invert(Vector);
void multiply(Vector, Number);
void addConst(Vector, Number);
void normalize(Vector);
void addVector(Vector, Vector);
void multVector(Vector, Vector);
void subVector(Vector, Vector);
void divVector(Vector, Vector);
void scaleVec(Vector, ScaleOp);
void asStringVert(Vector, char *);
void copyFromColl(Vector, Collection);
void copyVec(Vector, Vector);
Collection asColl(Vector);
Vector expand(Vector, Vector);
void removeDims(Vector, long *, long);                  /* sets error */
void removeDimRange(Vector, unsigned, unsigned);        /* sets error */
void fft(Vector, double *, double *);
void hlog(Vector v);
char *printVec(Vector, char *, unsigned);

/* Public functions of VECCOLL.O */
long numberOfRows(VecColl);
long numberOfCols(VecColl);
void compScalarsRow(VecColl, Vector, char);
void compScalarsCol(VecColl, Vector, char);
void collectRows(VecColl vc, Number (*vecFunc)(Vector), Vector v);
void collectCols(VecColl vc, Number (*vecFunc)(Vector), Vector v);
void doRows(VecColl vc, void (*vecFunc)(Vector));
Number overallAvg(VecColl);
Number overallMin(VecColl);
Number overallMax(VecColl);
Number overallStddev(VecColl);
Number dimAvg(VecColl, long);
Number dimMin(VecColl, long);
Number dimMax(VecColl, long);
unsigned constCols(VecColl, long *);
void colVec(VecColl, long, Vector);
void scaleAll(VecColl, Number, Number, ScaleOp *);
void fftRows(VecColl);                                  /* sets error */
void removeRowRange(VecColl, unsigned, unsigned);
void removeColRange(VecColl, unsigned, unsigned);       /* sets error */
void replaceRow(VecColl, unsigned, Vector);
void replaceCol(VecColl, unsigned, Vector);
void covariance(VecColl, Number **);                    /* sets error */
void colAsStringVert(VecColl, long, long, long, char *);

/* Public functions of SYMTAB.O */
Symtab newSymtab(long);                                 /* sets error */
void freeSymtab(Symtab);
char *addSymbol(Symtab, char *);                        /* sets error */
unsigned numSymbols(Symtab);
Collection sequence(Symtab);
char *locateSymbol(Symtab, char *);
long symbolIndex(Symtab, char *);
Symtab readSymtab(FILE *);                              /* sets error */
void fprintSymbols(Symtab, FILE *);

/* Public functions of BATCH.O */
void interpret(Collection, Patterns);                   /* sets error */

/* Public functions of EH.O */
void handleErr(int);
void genErrorMessage(int, char *);

/* Public functions of MISC.O */
void diskToStr(FILE *, char *);
Collection tokens(char *, char *);
long flen(FILE *);                                      /* sets error */
char *printnchr(char *, unsigned, char);
char *strupr(char *);
char *my_strdup(char *);
Boolean streq(char *, char *);
Boolean atEnd(FILE *);
Boolean atEndNl(FILE *);
void skipWhiteSpace(FILE *);
void skipSpaceAndCountNl(FILE *);
Boolean isSquare(unsigned);
Boolean isPowerOf2(unsigned);
char *matAsString(Number **, unsigned, unsigned);
void getScaleOp(ScaleOp *, Range, Range);


/*---------------------------------------------------------------------*/
/* GLOBAL VARIABLES                                                    */
/*---------------------------------------------------------------------*/

extern int error;
extern char errorInfo[];
extern long rowCount;
extern char tokenval[];
extern Boolean isDEC;

#endif _XVIS_H
