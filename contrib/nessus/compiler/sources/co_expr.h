/*****************************************************************************
  FILE              : co_expr.h
  SHORTNAME         : expr
  VERSION           : 2.0

  PURPOSE           : Header file for co_expr.c
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

                      /**** include file common to all modules ****/

#include "co_include.h"


                              /**** extern variables ****/

extern char *nst;             /* is always NULL - supress lint error message */

extern char *StringOrder[];

extern short ConstantSection;  /* true <==> currently parsing constant, type, or structure */
			       /* definition, or variable declaration section --> arrays */
			       /* of  arrays cannot be defined --> any array type in */
			       /* EX_MakeExplicitArray must have been read from a file */
