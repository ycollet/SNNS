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
                                /* Anzahl versch. Farbstufen */
#define NUM_GRAYS                  50
                                /* Anzahl versch. Graustufen */
#define MAX_LENGTH_ERROR_MESSAGE   512
                                /* Max. Laenge einer Fehlermeldung */
#define MAX_LENGTH_ERRORINFO       256
                                /* Max. Laenge der globalen Var. errorInfo */
#define MAX_LENGTH_TOKENVAL        1024
                                /* Max. Laenge der globalen Var. tokenval */
#define CLASS_MAXLEN               80
                                /* Max. Laenge eines Klassen-Namens */
#define CLASS_SCANF_FORMAT         "%80s"
                                /* scanf()-Format zum Lesen von Klassen */
#define NO_BUCKETS                 100
                                /* Anzahl Buckets fuer Klassen-Namen-Hashing */
#define FORMAT_DIR_ENVVAR          "VISTRAFORMATS"
                                /* Namen der Environment Variable, die das */
                                /* Directory angibt, in dem die FDL-Format */
                                /* Dateien zu finden sind.                 */
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

/* Ueberpruefe den Return-Wert von ftell() */
#define checkPos(pos)  if((pos) == -1L) { error = 6; return; }

/* Setze den Filepointer von File f auf Position pos, teste auf Fehler */
#define setPos(f,pos)  if(fseek((f), (pos), 0)) { error = 6; return; }

/* Beende Funktion und setze den Fehlercode nr */
#define error(nr)      { error = (nr); return; }

/* Berechne den durch so (eine ScaleOp structure) skalierten Wert von n */
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
    int originX;              /* X-Koordinate des Ursprungs rel. zu x */
    int originY;              /* Y-Koordinate  "      "     "    "  " */
    float x_mult;             /* Anzahl Pixels fuer eine X-Laengeneinheit */
    float y_mult;             /*   "      "     "    "   Y-      "        */
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
void freeDeep();
Collection add(Collection, void *);
void *at(Collection, long);
void put(Collection, long, void *);
Collection rmv(Collection, void *);
Collection removeAt(Collection, long);
Collection removeComplete(Collection, long);
Collection removeFromTo(Collection, long, long);
Collection freeFromTo();
long indexOf(Collection, void *);
long detectPos();
long size(Collection);
Boolean isEmpty(Collection);
Boolean notEmpty(Collection);

/* Public functions of FORMAT.O */
Format newFormat(FILE *);                      /* error gesetzt */
void freeFormat(Format);
long getPosition(Format);
void setPosition(Format, long);
enum Token lookUp(Format);
enum Token lookUpNextDesc(Format, long);
enum Token nextToken(Format);
void checkSyntax(Format);                      /* error gesetzt */
long posAfter(Format, enum Token);
Boolean isDesc(enum Token);

/* Public functions of PATTERNS.O */
Patterns newPatterns();                        /* error gesetzt */
void fileIn(Patterns, Format, FILE *);         /* error gesetzt */
void fileOut(Patterns, Format, FILE *);        /* error gesetzt */
void freePatterns(Patterns);
unsigned removePatterns(Patterns, unsigned, unsigned);
                                               /* error gesetzt */
unsigned removeCols(Patterns, Boolean, unsigned, unsigned);
                                               /* error gesetzt */
void removeDimList(Patterns, Boolean, long *, long);
                                               /* error gesetzt */
void randomize(Patterns, Vector);              /* error gesetzt */
long num(Patterns);
VecColl inputs(Patterns);
VecColl outputs(Patterns);
Collection classes(Patterns);
Collection classNos(Patterns);
void genClassNos(Patterns);                    /* error gesetzt */
void genClassNosFromNames(Patterns);           /* error gesetzt */
void genClassNosFromVectors(Patterns);         /* error gesetzt */
void replaceClasses(Patterns, Symtab);         /* error gesetzt */
long maxClassNo(Patterns);
char *classString(Patterns, long);
char *statString(Patterns, char *);
void writeSymtab(Patterns, FILE *);            /* error gesetzt */
long inputDims(Patterns);
long outputDims(Patterns);
Boolean hasClassNames(Patterns);
Boolean hasOutputs(Patterns);
void expandWithClassVectors(Patterns, Boolean);/* error gesetzt */
void expandWithOutputs(Patterns);              /* error gesetzt */
/* N01.C */
Patterns readN01(FILE *);                      /* error gesetzt */
void writeN01(Patterns, FILE *);               /* error gesetzt */
/* LVQ.C */
Patterns readLVQ(FILE *);                      /* error gesetzt */
void writeLVQ(Patterns, FILE *);               /* error gesetzt */
void lvqRead(Patterns, FILE *, Boolean);       /* error gesetzt */
void lvqWrite(Patterns, FILE *, Boolean);      /* error gesetzt */

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
void removeDims(Vector, long *, long);                  /* error gesetzt */
void removeDimRange(Vector, unsigned, unsigned);        /* error gesetzt */
void fft(Vector, double *, double *);
void hlog(Vector v);
char *printVec(Vector, char *, unsigned);

/* Public functions of VECCOLL.O */
long numberOfRows(VecColl);
long numberOfCols(VecColl);
void compScalarsRow(VecColl, Vector, char);
void compScalarsCol(VecColl, Vector, char);
void collectRows();
void collectCols();
void doRows();
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
void fftRows(VecColl);                                  /* error gesetzt */
void removeRowRange(VecColl, unsigned, unsigned);
void removeColRange(VecColl, unsigned, unsigned);       /* error gesetzt */
void replaceRow(VecColl, unsigned, Vector);
void replaceCol(VecColl, unsigned, Vector);
void covariance(VecColl, Number **);                    /* error gesetzt */
void colAsStringVert(VecColl, long, long, long, char *);

/* Public functions of SYMTAB.O */
Symtab newSymtab(long);                                 /* error gesetzt */
void freeSymtab(Symtab);
char *addSymbol(Symtab, char *);                        /* error gesetzt */
unsigned numSymbols(Symtab);
Collection sequence(Symtab);
char *locateSymbol(Symtab, char *);
Symtab readSymtab(FILE *);                              /* error gesetzt */
void fprintSymtab(Symtab, FILE *);

/* Public functions of BATCH.O */
void interpret(Collection, Patterns);                   /* error gesetzt */

/* Public functions of EH.O */
void handleErr(int);
void genErrorMessage(int, char *);

/* Public functions of MISC.O */
void diskToStr(FILE *, char *);
Collection tokens(char *, char *);
long flen(FILE *);                                      /* error gesetzt */
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

extern error;
extern char errorInfo[];
extern long rowCount;
extern char tokenval[];
extern Boolean isDEC;

#endif _XVIS_H
