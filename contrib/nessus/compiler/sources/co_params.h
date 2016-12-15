/*****************************************************************************
  FILE              : co_params.h
  SHORTNAME         : params
  VERSION           : 2.0

  PURPOSE           : header file for co_params.c 
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_include.h"


/**** extern variable declarations */
extern SymTabType *SymTabPtr;         /* pointer to top of symbol tables stack */

/**** avoid lint error message ****/
extern NullStr nst;
