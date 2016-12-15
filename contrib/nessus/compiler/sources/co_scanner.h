/*****************************************************************************
  FILE              : co_scanner.h
  SHORTNAME         : scanner
  VERSION           : 2.0

  PURPOSE           : header file for co_scanner.c generated by LEX
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



/**** extern variable declarations ****/
extern short   NewInput;              /* NewInput==TRUE <=> scanner must begin to read a new file */
extern int     ListDepth;             /* depth of nested subnets (input files) */
extern FILE   *ListFile;              /* pointer to listing file */
extern short   ListOption;            /* TRUE if source is listed into *ListFile */
extern short   TeXOption;             /* TRUE if source is listed in LaTeX environment */
extern ParserStackType *yylval;       /* upper element of parser value stack */
extern char *nst;                     /* always NULL - supress lint error message */
extern SymTabType *SymTabPtr;         /* pointer to top of symbol tables stack */



/* RISC */                           /* instead of 10**x for asdec */
#define exp10(x) pow(10, x) 

