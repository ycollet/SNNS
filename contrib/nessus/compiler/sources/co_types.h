/*****************************************************************************
  FILE              : co_types.h
  SHORTNAME         : types
  VERSION           : 2.0

  PURPOSE           : contains type definitions for nessus compiler.
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_constants.h"


                      /**** to avoid lint error messages ****/
typedef char *NullStr;

          /**** definitions of symbol types and variable data types ****/

typedef short SymbolType;              /* enumeration data type not possible (yacc) */


                    /**** structure types for symbol table ****/

typedef struct SymTabLineStruct {      /* symbol table is an array of pointers to these lines */
  char                   *Name;        /* name of token */
  SymbolType              Type;        /* if keyword token no. else variable data type */
  union SymbolValueUnion *Value;       /* pointer to value description record */
  short                   UndefType ;  /* TRUE <=> variable data type is undefined */
  short                   UndefValue;  /* TRUE <=> variable value is still undefined */
  short                   Constant;    /* TRUE <=> is constant, must not be modified */
  short                   Lock;        /* TRUE <=> is loop variable, must not be modified */
  short                   isKeyword;   /* TRUE <=> is keyword */
} SymTabLineType;

typedef struct SymTabStruct {          /* record to describe currently parsed network */
  char                 *Network;       /* name of currently parsed network */
  char                 *FileStr;       /* name of current input file */
  SymTabLineType      **SymTab;        /* pointer to symbol table (array of &SymTabLineType) */
  struct SymTabStruct  *NextSymTab;    /* a stack of parsed networks is built (inner net on top) */
  float                 ActLength;     /* length of THIS symbol table */
  int                   ActPrime;      /* greatest prime < ActLength (for hash function) */
  float                 ActEntries;    /* to expand symbol table before its density is too high */
  int                   Insertions;    /* check density at each 10th. insertion */
  int                   MinUnitNo;     /* no. of first unit (to copy subnet connections) */
} SymTabType;


                    /**** structure types for data value representations ****/

typedef struct FileValueStruct {       /**** may change ****/
  char        *FileName;               /* physical name of file */
  FILE        *FilePtr;                /* file pointer */
  SymbolType   FileType;               /* data type of file entries */
} FileValueType;

typedef struct SiteListStruct {        /* record to describe a site list element */
  char                  *SiteName;     /* name of site (NOT name of site TYPE !!) */
  SymTabLineType        *SiteFunction; /* name of site function (identifier) */
  struct SiteListStruct *NextSite;     /* pointer to next site record */
} SiteListType;


                    /**** types for unit structures representation ****/

typedef struct Topologystruct {        /* structure topology description record */
  SymbolType Form;                     /* one of MATRIX or ELLIPSE (plane is matrix(1,y)) */
  int        XDim;                     /* extension of topology area in x direction */
  int        YDim;                     /* extension of topology area in y direction */
  int        XCenter;                  /* x coordinate of topology area center */
  int        YCenter;                  /* y coordinate of topology area center */
} TopologyType;

typedef union FTypeUnion {
  struct SymTabLineStruct   *NamedFT;  /* pointer to symbol table entry of named fType */
  struct ExplicitTypeStruct *VarFT;    /* pointer to FType parameters definition record */
} FTypeType;

typedef struct UnitStruct {            /* data type to define unit parameters */
  short             FType;             /* TRUE <=> fType is set for these units */
  union FTypeUnion  FParam;            /* explicit or named fType */
  char             *UName;             /* unit name */
  SymbolType        SType;             /* topology type, one of 'i', 'o', or 'h' */
  float             Act;               /* initial activation value */
  int               UnitNo;            /* internal unitNo inserted for eventual connection definition */
  int               XPos;              /* used only if structure is a single unit */
  int               YPos;              /* mark units x and y position */
} UnitType;

typedef struct MapStructStruct {
  struct MapLineStruct *Low;           /* First map line for irregular structure */
  int                   LineNo;        /* no. of map lines for irregular structure */
} MapStructType;

typedef struct ExplicitTypeStruct {    /* FType parameters definition record */
  short                     SiteNo;    /* no. of sites */
  SymTabLineType           *OutFunc;   /* name of output function */
  SymTabLineType           *ActFunc;   /* name of activation function */
  struct SymTabLineStruct **Sites;     /* pointer to symbol table entries of site types */
} ExplicitFType;

typedef struct UnitValueStruct {       /* record to describe a stucture of units and connections */
  SymbolType      StructType;          /* type of structure: cluster, ring, chain, star */
  SymbolType      Direction;           /* forward, backward, or bidirectional */
  UnitType       *UnitDef;             /* pointer to unit parameters definition record */
  int             Length;              /* length of structure (no. of units) */
  int             Begin;               /* no. of first unit of this structure */
  SymTabLineType *Through;             /* target site type for implicit connections in structure */
  float           Weight;              /* weight of implicit connections */
  int             Center;              /* rel. index of center unit if structure is STAR */
  UnitType      **ExplUnits;           /* pointer to units whose parameters have been modified */
  TopologyType    Topology;            /* structure topology description reord */
  MapStructType   Map;                 /* holds map lines for irregular structure */
} UnitValueType;

                 /**** internal representation of array data types ****/

typedef union ArrayElemUnion {
  char                    **astring;   /* array of strings */
  int                      *aint;      /* array of integer values */
  float                    *afloat;    /* array of float values */
  UnitValueType           **astruct;   /* array of structures */
  UnitType                **aunit;     /* array of units (FOREACH loop, explicit list only) */
  struct ArrayValueStruct **aarray;    /* array of structure arrays (FOREACH loop) */
} ArrayElemType;

typedef struct ArrayValueStruct {
  short          Named;                /* TRUE <=> value may not be removed (named array) */
  short          aRange;               /* TRUE <=> array list = lower + upper bound (integer) */
  int            aSize;                /* no. of array elements in *avalue */
  ArrayElemType  aValue;               /* array of values */
} ArrayValueType;

             /**** internal representation of map constant definitions ****/

typedef struct MapLineStruct {         /* describes one map line "domain TO range" */
  ArrayValueType *Domain;              /* points to array of domain set elements */
  ArrayValueType *Range;               /* points to array of range set elements */
} MapLineType;

typedef struct MapValueStruct {        /* record to describe a relation constant */
  int           mSize;                 /* no. of lines of map constant definition */
  SymbolType    DType;                 /* data type of the domain set elements */
  SymbolType    RType;                 /* data type of the range set elements */
  MapLineType  *mLines;                /* points to the array of map def. line descriptions */
} MapValueType;


           /**** selection of unit components at left side of assignment ****/
typedef struct SiteSelectionStruct {   /* record for unit no. and site (connection target) */
  SiteListType *site;                  /* pointer to site */
  int           unit;                  /* no. of unit */
} SiteSelectionType;

typedef union RecordUnion {            /* selection result can be a site or a unit component */
  UnitType          *unit;             /* unit component */
  SiteSelectionType  site;             /* site of a unit (in connection specification) */
} RecordType;

typedef struct SelectionStruct {
  SymbolType   Component;              /* unit component which has been selected */   
  RecordType   Record;                 /* ptr. to unit whose component has been selected */
} SelectionType;


                   /**** representation of array or map range  ****/

typedef struct RangeStruct {           /* parser stack type record for range of map or array */
  SymbolType       FieldType;          /* MAP or ARRAY - type of field */
  SymTabLineType  *FieldPtr;           /* pointer to symbol table entry */
  int              Lower;              /* first element of range (lower index) */
  int              Upper;              /* lasr element of range (upper index) */
} RangeType;

             /**** representation of variable values in symbol table ****/

typedef union SymbolValueUnion {   
  int                    intval;       /* integer value */
  float                  floatval;     /* float value */
  char                  *string;       /* string value */
  SymTabLineType        *var;          /* variable value - pointer to symbol table entry */
  SiteListType          *site;         /* site type */
  UnitType              *unit;         /* simple unit element */
  MapValueType          *map;          /* map constant */
  FileValueType         *fileval;      /* file specification */
  SymTabType            *subnet;       /* subnet inclusion into outer symbol table */
  ArrayValueType        *array;        /* array values (size of array and value list) */
  UnitValueType         *structure;    /* arrays of units (incl. implicit connections) */
  struct ListValStruct  *list;         /* parser stack type - stack representation of lists */
  SymTabLineType       **slist;        /* temp. for list of ptrs. to site entries */
  SelectionType         *selection;    /* to hold result of selection operator on units */
  RangeType             *range;        /* to hold value of field or map range specification */
} SymbolValueType, *SymbolValuePType;


                       /**** connection definition data types ****/

typedef struct ConnectionStruct {
  int                      Prime;      /* greatest prime <= Size, for hash function */
  float                    Size;       /* length of array of target entries */
  float                    Entries;    /* no. of entries in target array */
  struct TargetUnitStruct *Targets;    /* pointer to array of target entries */
} ConnectionType;

typedef struct TargetUnitStruct {
  int                      TNo;        /* unique no. of target unit */
  struct TargetSiteStruct *Site;       /* ptr. to list of input sites for target unit */
} TargetUnitType;

typedef struct SourceListStruct {
  int                       Prime;     /* greatest prime <= Size, for hash function */
  float                     Size;      /* length of array of source units */
  float                     Entries;   /* no. of entries in source units array */
  struct SourceEntryStruct *Sources;   /* pointer to array of sources */
} SourceListType;

typedef struct TargetSiteStruct {
  struct TargetSiteStruct *tNext;      /* pointer to next input site of same target unit */
  SiteListType            *tSite;      /* pointer to (global!!) NAME of site (not type!) */
  struct SourceListStruct  tInput;     /* pointer to list of sources connected to site */
} TargetSiteType;

typedef struct SourceEntryStruct {
  int   SNo;                           /* unique no. of source unit */
  float Weight;                        /* connnection weight in [-1.0, 1.0] */
} SourceEntryType;
  

                       /**** function definition data types ****/

typedef struct FuncListStruct {      /* global list of functions used in a network definition */
  char                  *FuncName;   /* name of function (without prefix) */
  struct FuncListStruct *NextFunc;   /* pointer to next function entry in list */
} FuncListType;



                       /**** data types for Comp.Scanner.c ****/

typedef struct InputStackStruct {      /* one record for each source input file */
  struct InputStackStruct *NextInp;    /* pointer to next input description record */
  char  *InFileName;                   /* name of input file */
  FILE  *InFilePtr;                    /* points to input file */
  char   InFileBuf[BUFSIZ];            /* input buffer for this file */
  char  *ActChar;                      /* save actual read position if another file is read */
} InputStackType;



                       /**** data types for Comp.Error.c ****/

typedef struct ErrorStackStruct {      /* describes an error message */
  struct ErrorStackStruct *NextErr;    /* pointer to next stack element */
  char                    *Format;     /* format to print error message */
  char                    *Type;       /* to print error type fatal, serious, or warning */
  char                    *Strs[3];    /* string variables included into message */
} ErrorStackType;



                       /**** data types for parser value stack ****/

typedef struct ParserStackStruct {     /* type of parser value stack elements */
  SymbolType                ytype;     /* type of expression or token */
  SymbolValueType          *yvalue;    /* value of expression or token */
  struct ParserStackStruct *ynext;     /* ptr. to next parser stack record (FREE or list) */
} ParserStackType, *ParserStackPType;

typedef struct ListValStruct {         /* list of values: list of tokens before an array is */
  ParserStackType      *Elements;      /* built or elements of an explicit list in a FOREACH */
  int                   LCount;        /* expression */
} ListValType;



                            /**** data types for loop stack ****/

typedef struct LoopVarStackStruct {        /* structure for list of FOREACH loop variables */
  SymTabLineType            *Variable;     /* pointer to symbol table entry for variable */
  ArrayElemType             *ActValue;     /* pointer to (!) actual array element value */
  ArrayElemType             *InitValue;    /* pointer to (!) initial array element value */
  short                      Remove;       /* TRUE <=> array may be removed at the end of loop */
  struct LoopVarStackStruct *NextVar;      /* pointer to next loop variable */
} LoopVarStackType;

typedef struct LoopHeaderStruct {          /* header to control iterations of a FOR or FOREACH loop */
  int                      ActIter;        /* counter of iterations */
  int                      TotalIter;      /* total no. of iterations */    
  union LoopVarsUnion {                    /* there are no special loop variables allocaated */
    SymTabLineType            *Single;     /* ptr. to loop variable (FOR-loop) */
    LoopVarStackType          *Multi;      /* ptr. to list of loop variables (FOREACH-loop) */
  }                        LoopVars;       /* one or more loop variables, depending on loop type */
} LoopHeaderType;

typedef struct CondValueStruct {           /* token stack value entry for IF .. THEN .. ELSE */
  short                   Executed;        /* TRUE <==> one branch has been done - look for END */
  struct TokenStackStruct *IfCondition;    /* stack of tokens which form logical expression */
  struct TokenStackStruct *ThenStmts;      /* stack of tokens of statement list after THEN */
  struct TokenStackStruct *NextIf;         /* pointer to (next) Elseif or else token */
} CondValueType;  

typedef union TokenValueUnion {            /* different value types for tokens */
  struct LoopValueStruct  *Loop;           /* token value for loop */
  CondValueType           *Cond;           /* token value for condition */
  ParserStackType         *Simple;         /* token value for simple token */
} TokenValueType;

typedef struct TokenStackStruct {          /* data type to store tokens and their values */
  SymbolType               Token;          /* token type (normal token, LOOP, or CONDITION */
  TokenValueType           Value;          /* value entry for tokens */
  int                      Line;           /* store input line for each token */
  struct TokenStackStruct *NextToken;      /* pointer to next token */
} TokenStackType;


typedef struct LoopValueStruct {           /* token stack value entry for LOOP */
  TokenStackType *ConditionPtr;            /* holds tokens which form condition (WHILE) */
  TokenStackType *InitialPtr;              /* holds tokens which form initialization (FOR, FOREACH) */
  TokenStackType *Continue;                /* pointer to list of statements IN loop */
  TokenStackType *Abort;                   /* pointer to list of statements AFTER loop */
  LoopHeaderType *LoopHeader;              /* pointer to header (FOR and FOREACH, only) */
} LoopValueType;


typedef struct BackPtrStruct {             /* stack first TokenStackType records for each loop */
  TokenStackType       *TokenPtr;          /* points to WHILE, FOR, FOREACH, or IF tokens  */
  struct BackPtrStruct *Outer;             /* points to begin of containing loop or condition */
} BackPtrType;
