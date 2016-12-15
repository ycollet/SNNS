/*****************************************************************************
  FILE              : co_symTab.h
  SHORTNAME         : co_sym
  VERSION           : 2.0

  PURPOSE           : header file for co_symTab.c
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


/**** extern variables defined in co_main.c ****/
extern SymTabType *SymTabPtr;  /* pointer to top of symbol tables stack */
extern char *nst;              /* always NULL - supress lint error message */
extern int UnitCtr;            /* no. of units */

/**** upper element of parser value stack ****/
extern ParserStackType *yylval;
