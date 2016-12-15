/*****************************************************************************
  FILE              : co_stmt.h
  SHORTNAME         : stmt
  VERSION           : 2.0

  PURPOSE           : Header file for co_stmt.c
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



/**** extern variable declarations ****/
extern char *nst;          /* is always NULL - for lint */



extern short RightSide;    /* TRUE <=> 1) parsing right side of an assignment statement */
                           /*          2) parsing constant or structure definition section */

extern short AbortLoop;    /* TRUE <=> cannot execute loop body (semantic errors)  */
