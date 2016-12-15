/*****************************************************************************
  FILE              : co_main.h
  SHORTNAME         : main
  VERSION           : 2.0

  PURPOSE           : header file for co_main.c
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

/* include common files for compiler */
#include "co_include.h"


/* declaration for time function */
extern long time();

/* declaration for random generator intialization */
extern void srand48();



/* macros to deal with program parameters */
#define isfloat(x,y) (sscanf(x, "%f", &y) != 0) /* TRUE if string x contains a float no. */
                                                          /* float no. is returned in y  */

/* counter of current input line (see Tokens.l, Syntax.y) */
extern int yylineno;
