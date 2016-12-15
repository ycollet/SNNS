/*****************************************************************************
  FILE              : co_error.h
  SHORTNAME         : error
  VERSION           : 2.0

  PURPOSE           : header file for co_error.c 
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

/**** include files common to all modules ****/
#include "co_include.h"


/**** error counters defined in Comp.Main.c ****/
extern short FatalErrs;                /* defined in Comp.Main.c */
extern short SeriousErrs;              /* defined in Comp.Main.c */
extern short WarningErrs;              /* defined in Comp.Main.c */

/****  fatal error inhibits further semantic actions */
extern short Topology;                 /* defined in Comp.Main.c */

/**** line number and pointer to listing file */
extern int     yylineno;               /* defined in Comp.Scanner.c */
extern FILE   *ListFile;               /* defined in Comp.Main.c */
extern short   ListOption;             /* defined in Comp.Main.c */


/**** pointer to upper symbol table ****/
extern SymTabType *SymTabPtr;           /* defined in Comp.Main.c */

extern short EvalLoopMode;
extern short AbortLoop;
