/*****************************************************************************
  FILE              : co_constants.h
  SHORTNAME         : constants
  VERSION           : 2.0

  PURPOSE           : contains compiler constant and macro definitions.
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

                         /**** c standard include files ****/
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
//#include <struct.h>
#include <time.h>
#include <memory.h>


        /**** macro definitions for float and integer components of nessus strings ****/
#define ISINTCOMP(x) (isdigit(x) || ((x) == '-') ||  ((x) == '+'))
#define ISFLOATCOMP(x) (isdigit(x) || ((x) == '-') || ((x) == '+') || ((x) == '.') \
|| ((x) == 'e')  || ((x) == 'E'))
#define ISSTRINGCOMP(x) (isdigit(x) || isalpha(x) || ((x) == '_'))
#define SNNSLEADSTRING(x) ( ! isdigit(x) && ((x) != ',') && ((x) != '|') && ((x) != ' ')) 
#define SNNSILLEGALSTRING(x) (((x) == ' ') || ((x) == '|') || ((x) == '\n') || \
((x) == ',') || ((x) == '\t') || ((x) == '-'))

     /**** macro definition of random value for unit activation or connection weight ****/
extern double drand48();
#define GETRANDOM ((float) (2.0 * drand48() - 1.0))


                           /**** boolean  values ****/
#define TRUE 1
#define FALSE 0

                             /**** error types ****/

#define Fatal 1                 /* fatal errors : stop compiling */
#define Serious 2               /* serious errors: continue compiling, no output generation */
#define Warning 3               /* warnings: may cause semantic errors, net generation */

                   /**** constants for symbol table management ****/
#define InitSymTabLength 200    /* initial length of symbol table (including keywords!) */
#define SymTabAdd        100    /* increment of symbol table length when it is expanded */
#define NoOfKeywords     67     /* no. of keywords inserted into symbol table */
#define SymTabCheckDist  10     /* interval of symbol table density checks (insertions) */
#define ExpandDensity    0.9    /* max. density of symbol table entries */

                     /**** token value definitions for xxx ****/
#include "co_tokens.h"        /* definitions set by YACC */

           /**** expression value definitions for symbol table entries ****/
#define Undef 90                /* undefined data type */
#define UNITTYPE 91             /* unit type definition */
#define Conn 92                 /* simple connection */ 
#define ArrInt 93               /* array of integers */
#define ArrFloat 94             /* array of floats */
#define ArrString 95            /* array of strings */
#define ArrStruct 96            /* array of structures */
#define InOut 97                /* unit s_type: input AND output */
#define Boolean 98              /* type for operands of logical expressions on parser value stack */ 
#define FILEVAL 99              /* type keyword for file */
#define Selection 100           /* type keyword for selection result */
#define ArrUnit 101             /* array of strings */
#define ArrArrStruct 102        /* array of arrays of structures (->  FOREACH loop) */
#define ArrArrInt 103           /* array of ArrInt */
#define ArrArrFloat 104         /* array of ArrFloat */
#define ArrArrString 105        /* array of ArrString */
#define XDOMAIN 106             /* keyword for map domain - not used in parser */
#define RANGE 107               /* keyword for map or array range - not used in parser */


          /**** LaTeX environment types for listing under TeX option ****/
#define Verbatim 0
#define Math 1
#define Keyword 2
#define Comment 3
#define Eol 4
#define Blank 5
#define Identifier 6
#define Eof 7
#define Illegal 8

                    /**** constants for topology evaluation ****/
#define NoChars 62              /* no. of strings with length == 1 (array bounds) */

         /**** constants for undefined unit, connection and type parameters ****/
#define ActNullValue 9.0               /* undefined activation value -- MUST BE INTEGER !! */
#define STypeNullValue Undef           /* value to mark undefined  value */
#define DirectionNullValue PT          /* value for default connection direction */
#define CenterNullValue 0              /* value for default star center index */
/* undefined value for name, actfunc, outfunc, sitefunc, and sites is NULL */
/* undefined value for SiteNo is 0 */

                     /**** constant values for nessus file i/o ****/
#define BackBufSize 50                /* size of background input buffer for overlapping inputs */
#define OutBufFactor 128              /* OutBufFactor * BUFSIZ = size of output buffer */
#define FileArrayExpand 200           /* increment for array sizes of file i/o */
#define InitMapLines 5                /* initial no. of allocated map lines */
#define AddMapLines 5                 /* increment for no. of allocated map lines */
#define MaxLineLength 90              /* max length of output line in conn section (up to 250) */

                   /**** constants for connection table management ****/
#define InitTargetNo 200.0            /* initial length of target table */
#define InitSourceNo 200.0            /* initial length of source tables (one per target and site) */
#define MaxSourceDensity 0.9          /* maximal density of source tables */
#define MaxTargetDensity 0.9          /* maximal density of target table */
#define ExpandTarget 200              /* expand size of target table (if density is too high) */
#define ExpandSource 200              /* expand size of source tables (if density is too high) */

                      /**** constant values for nessus i/o ****/
#define NetCompLength 100   /* initial length of network components array */
#define IncrNetComp 50      /* increment size for network components array */

               /**** exit codes for compiler (REALLY fatal errors) ***/
#define NetGenerated 0      /* Friede -- Freude -- Eierkuchen */
#define NoInput 1           /* input file not found */
#define NoSubnet 2          /* input file for subnet not found */
#define NoFile 3            /* input file not found (not used) */
#define IllegalProgram 4    /* parser detected (fatal) syntax errors) */
#define IllegalCall 5       /* illegal compiler options */
#define OutOfSpace 6        /* malloc failed */
#define NoOutput 7          /* could not create output file */

                           /**** exit codes for bugs ****/
#define IllPopInputStack 100          /* attempt to remove inexisting element */
#define ArraySizeMismatch 101         /* array element counter is incorrect */
#define Bug 102                       /* unspecific bug - just to avoid core dumps */
