/*****************************************************************************
  FILE              : co_include.h
  SHORTNAME         : include
  VERSION           : 2.0

  PURPOSE           : contains include statements for files common to all compiler modules.
  NOTES             :

  AUTHOR            : Thomas Korb
  DATE              : 27.6.1991

  CHANGED BY        :
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

/**** compiler type definitions ****/
#include "co_types.h"

/**** function prototypes ****/
#include "co_prototypes.h"


/**** extern random functions ****/
extern double drand48();

/* Bison (invoked with -p pp for Syntax.y) renames its exposed globals
   from yy* to pp* (e.g. yylval -> pplval). Code outside the generated
   parser/scanner still refers to the plain yylval name; redirect it to
   the real symbol bison actually emits. */
#define yylval pplval
