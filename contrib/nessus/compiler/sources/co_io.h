/*****************************************************************************
  FILE              : co_io.h
  SHORTNAME         : io
  VERSION           : 2.0

  PURPOSE           : header file for co_io.c
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

extern SymTabType *SymTabPtr;   /* pointer to upper symbol table on stack (inner ssubnet) */
extern int yylineno;            /* current input line no. of source file */


extern char *nst;          /* NULL char pointer for lint */


