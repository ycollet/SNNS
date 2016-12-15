/*****************************************************************************
  FILE              : co_parser.c
  SHORTNAME         : parser
  VERSION           : 2.0

  PURPOSE           : header file for co_parser.c 
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


/**** extern variables defined in co_main.c ****/
extern char *nst;               /* is always NULL - supress lint error message */
extern ParserStackType *psNULL; /* is always NULL - supress lint error message */
extern short Topology;          /* TRUE <=> parsing topology definition and evaluation still in progress */
extern short Functions;         /* TRUE <=> parsing function definitions and translation  still in progress */
                                /* Topology and Functions are needed to decide if expressions must be */
			        /* evaluated, rather than translated into C functions. After a fatal error */
			        /* Topology == Functions == FALSE. */
extern short RightSide;         /* true if yyparse is parsing the right side of a statement or while parsing */
                                /* constants or structures */
extern short Structures;        /* is set while structures are beeing parsed - needed to supress */
			        /* redefinition of connection messages in map-defined structures (IRREGULAR) */
extern SymTabType *SymTabPtr;   /* pointer to upper sumbol table on stack (iner ssubnet) */

extern short InitMode;          /* parser states to control execution of loops and conditional  statements */
extern short StackMode;         /* stack tokens, entered after WHILE, FOR, or FOREACH has been read */
extern short EvalLoopMode;      /* getting tokens rather from stack than from scanner */
extern short AbortLoop;         /* errors in EvalLoopMode (at least Serious) -- exit nested loops */
extern short Execute;           /* stacking has been finished up - StackMode -> Execute -> EvalLoopMode */
extern short ExitMode;          /* AbortLoop && at the end of a statement -->  exit loops */
extern short ForeachWaitFlag;   /* insert an (illegal) ';' between END and FOREACH -> lookahead of parser */


/**** extern variables defined in co_scanner.c, lex.yy.c, Tokens.l ****/
extern int yylineno;
extern InputStackType *SC_InputStack;     /* stack of source file names */


/**** extern variables defined in co_struct.c ****/
extern int ST_SiteNo;                          /* count no. of sites of unit structure or type definitions */
extern UnitValueType *ST_TempStruct;           /* hold structure definition record */


/**** extern variables defined in Comp.ParseFunctions.c ****/
extern ParserStackType *PA_ProgramArguments;       /* stack of actual arguments for program */


/**** extern variables defined in co_stmt.c ****/
extern SymTabType *InnerSubnet;


/**** macro definitions for parser ****/
#define yyerror(x) ER_StackError(STR_Append(x, "\n"), Fatal, nst, nst, nst)      /* fatal syntax errors */
#define yyserious(x) ER_StackError(STR_Append(x, "\n"), Serious, nst, nst, nst)  /* serious errors */
#define yywarning(x) ER_StackError(STR_Append(x, "\n"), Warning, nst, nst, nst)  /* warnings */
#define yylex() LOOP_GetNextToken()

