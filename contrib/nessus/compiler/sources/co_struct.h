/*****************************************************************************
  FILE              : co_struct.h
  SHORTNAME         : struct
  VERSION           : 2.0

  PURPOSE           : header file for co_struct.c
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

/* include statement common to all modules */
#include "co_include.h"



extern char *nst;
extern int UnitCtr;
extern int UnitTypeCtr;
extern int SiteTypeCtr;
extern int ConnectionCtr;
extern int NetCompCtr;
extern int NoNetComponents;
extern int XCenter;
extern int YCenter;


extern char *DefActFunc;
extern char *DefOutFunc;


extern ParserStackType *UnitTypes;       /* stack of unit types */
extern ParserStackType *SiteTypes;       /* stack of site types */
extern SymTabLineType **NetComponents;   /* point to symbol table entries of structures and units */
extern SymTabType *SymTabPtr;            /* pointer to top of symbol tables stack */
