/*****************************************************************************
  FILE              : co_output.h
  SHORTNAME         : output
  VERSION           : 2.0

  PURPOSE           : header file for co_output.c
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

/* default learning and update function */                             /* v2.2 */
#define DEFAULT_LEARNING_FUNCTION   "Std_Backpropagation"              /* v2.2 */
#define DEFAULT_UPDATE_FUNCTION     "Topological_Order"                /* v2.2 */

/* statistics to be printed into header of output file */
extern int UnitCtr;
extern int ConnectionCtr;
extern int UnitTypeCtr;
extern int SiteTypeCtr;

extern char *InFileName;
extern char *OutFileName;

extern ParserStackType *UnitTypes;
extern SiteListType *SiteList;
extern SymTabType *SymTabPtr;
extern SymTabLineType **NetComponents;
extern int NetCompCtr;
extern char *VersionNo;
extern long poi;

extern ConnectionType *CONN_Connections;


