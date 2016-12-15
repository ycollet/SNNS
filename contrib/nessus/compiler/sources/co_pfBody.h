/*****************************************************************************
  FILE              : co_pfBody.h
  SHORTNAME         : pfBody
  VERSION           : 2.0

  PURPOSE           : Body of header file for co_pFuncs.c.
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

/**** standard include statement ****/
#include "co_include.h"

/**** extern variables defined in Comp.Main.c ****/
extern short Topology;
extern short NewInput;
extern short Structures;
extern int ListDepth;
extern int XCenter;
extern int YCenter;

/**** extern variables defined in Comp.Scanner.c, lex.yy.c, Tokens.l ****/
extern int yylineno;

/**** macro definitions for parser ****/
#define yyerror(x) ER_StackError(x, yylineno)    /* new error reporting function */
#define yylex() SC_Scanner()

extern ParserStackType *yylval, *yyval, yyv[];
extern int yychar, yynerrs, yyerrflag;

/**** global variable declarations for recursive  lex calls */
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};

extern char *SC_ActChr, *SC_InputStream;
extern int yyprevious, *yyfnd, yytchar, yymorfg, yyleng;
extern struct yysvf  **yylsp, **yyolsp, *yyestate, *yylstate[];
extern char *yysptr, yytext[], yysbuf[];

extern char *nst;     /* null string for lint */

