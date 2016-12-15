/*****************************************************************************
  FILE              : co_loop.h
  SHORTNAME         : loop
  VERSION           : 2.0

  PURPOSE           : Header file for co_loop.c
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/


/**** include file common to all modules */
#include "co_include.h"



extern ParserStackType *yylval;                           /* parser value stack records */
extern ParserStackType *psNULL;                                        /* NULL for lint */
extern char *nst;                                                      /* NULL for lint */
extern short Topology;                                /* TRUE <==> no fatal errors yet  */

extern short InitMode;    /* parser states to control execution of loops and */
extern short StackMode;   /* conditional  statements */
extern short EvalLoopMode;
extern short ConditionMode;
extern short AbortLoop;
extern short Execute;

extern int FatalErrs;
extern int SeriousErrs;
extern int WarningErrs;


extern int yylineno;      /* get and restore line no. for token */

/* makro for tokens which make scanner enter condition mode */
#define CONDITIONBEGIN(x) ((x) == IF || (x) == ELSEIF || (x) == ELSE)

/* makro for tokens which make scanner enter loop mode */
#define LOOPBEGIN(x) ((x) == WHILE || (x) == FOR || (x) == FOREACH)

/* makro for tokens which make scanner enter stack mode */
#define STACKBEGIN(x) (LOOPBEGIN(x) || CONDITIONBEGIN(x))

/* makro for tokens ELSEIF and ELSE */
#define NEXTBRANCH(x) ((x) == ELSEIF || (x) == ELSE)

/* makro for legal data types for loop variables */
#define CORRECTSETTYPE(x) ((x) ==  ArrStruct || (x) ==  ArrInt || (x) ==  ArrFloat ||\
                           (x) ==  ArrString  || (x) ==  STRUCTURE || (x) ==  ArrUnit ||\
		           (x) == ArrArrStruct || (x) == ArrArrInt  ||\
			   (x) == ArrArrFloat || (x) ==  ArrArrString)

/* makro for legal combinations of elemnt and field data types */
#define SETMEMBERMATCH(x, y) ((x) == ArrStruct && (y) == ArrUnit || \
			      (x) == ArrStruct && (y) == STRUCTURE || \
			      (x) == ArrInt && (y) ==  XINT || \
			      (x) == ArrString && (y) ==  STRING || \
			      (x) == ArrString && (y) == LONGSTRING || \
			      (x) == ArrString && (y) == SIMPLESTRING || \
			      (x) == STRUCTURE && (y) == UNIT || \
			      (x) == ArrUnit && (y) == UNIT || \
			      (x) == ArrArrStruct && (y) == ArrStruct || \
			      (x) == ArrArrInt && (y) == ArrInt || \
			      (x) == ArrArrFloat && (y) == ArrFloat || \
			      (x) == ArrArrString && (y) == ArrString)

#define GETELEMTYPE(x) (((x) == ArrStruct) ? ArrUnit : \
			((x) == ArrInt) ? XINT : \
			((x) == ArrString) ? STRING  : \
			((x) == STRUCTURE) ?  UNIT : \
			((x) == ArrUnit) ? UNIT : \
			((x) == ArrArrStruct) ? ArrStruct  : \
			((x) == ArrArrInt) ? ArrInt  :  \
			((x) == ArrArrFloat) ? ArrFloat : \
			((x) == ArrArrString) ? ArrString : Undef)

