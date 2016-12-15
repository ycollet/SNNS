/*****************************************************************************
  FILE              : co_pFuncs.c
  SHORTNAME         : pFuncs
  VERSION           : 2.0

  PURPOSE           : Auxiliar functions for use with parser generated by yacc.
                      Included into co_parser.c.
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_pFuncs.h"


                               /**** static variables ****/

ParserStackType *PA_ProgramArguments;           /* compile time defined argument values */
static ParserStackType *PA_FreeParserPtr; /* stack of unused parser value stack records */




/*****************************************************************************************
 Function:                     PA_Parser
 Purpose: Initializes parser and scanner input. Calls function yyparse built by yacc.
 Returns: Result of yyparse.
 Side effects: Modifies stack of input files.
*****************************************************************************************/
int PA_Parser(InFile, NetArgStack, XAdd, YAdd)
     ParserStackType *InFile;                          /* holds constant for input file */
     ParserStackType *NetArgStack;                       /* stack of program attributes */
     ParserStackType *XAdd;                            /* to transform subnet positions */
     ParserStackType *YAdd;
{ 
  int val;                                             /* holds result value of yyparse */
  short Temp_Topology;                     /* save variable "Topology" of outer program */
  int Temp_yylineno;                              /* save line no. of outer source file */
  char *FName;                       /* will hold physical  name of  subnet source file */

  ParserStackType *Temp_yylval, *Temp_yyval; 
  ParserStackType *Temp_yyv[YYMAXDEPTH], *New_yyv;                      /* static parse */
  int Temp_yychar, Temp_yynerrs, Temp_yyerrflag;             /* variables must be saved */
  
  register int i;

  char *Temp_ActChr, *Temp_InputStream, *Temp_yysptr;                 /* static scanner */
  struct yysvf **Temp_yylsp, **Temp_yyolsp, *Temp_yyestate;  /* variables must be saved */
  struct yywork *Temp_yytop;
  char *Temp_yytext, *Temp_yysbuf; /* *Temp_yylstate[] */
  int Temp_yyprevious, Temp_yyfnd, Temp_yytchar, Temp_yymorfg, Temp_yyleng;

  STMT_GetIdValue(InFile);
  if( ! XAdd || XAdd->ytype != Undef && XAdd->ytype != INTEGER) {
    ER_StackError("type mismatch: x position for subnet expected\n",Serious, nst,nst,nst);
    XAdd->yvalue->intval = 0;
  } 
  else
    XCenter += XAdd->yvalue->intval;
  if( ! YAdd || YAdd->ytype != Undef && YAdd->ytype != INTEGER) {
    ER_StackError("type mismatch: y position for subnet expected\n",Serious, nst,nst,nst);
    YAdd->yvalue->intval = 0;
  }
  else
    YCenter += YAdd->yvalue->intval;
  if(InFile->ytype != SIMPLESTRING  && InFile->ytype != LONGSTRING && 
     InFile->ytype != FILEVAL ) {
    ER_StackError("type mismatch: name of nessus source file expected\n",Serious, nst,nst,nst);
    val = TRUE;
  }
  else {
    FName = (InFile->ytype == FILEVAL) ? InFile->yvalue->fileval->FileName :
      InFile->yvalue->string;
    if(SC_PutInputStack(FName)) {                         /* begin to scan new input file */
      PA_ProgramArguments = NetArgStack;         /* string of program or subnet arguments */
      ListDepth ++;                       /* increment subnet depth (initialized with -1) */
      Structures = FALSE;                                  /* reset environment variables */
      XCenter += XAdd->yvalue->intval;  YCenter += YAdd->yvalue->intval;
      Temp_yylval = yylval; Temp_yyval = yyval; Temp_yychar = yychar;  
      Temp_yynerrs = yynerrs; Temp_yyerrflag = yyerrflag;
      (void) memcpy((char*) Temp_yyv, (char*) yyv, YYMAXDEPTH *  sizeof(ParserStackType *));
      Temp_yylineno = yylineno; Temp_yytchar = yytchar; Temp_yymorfg = yymorfg; Temp_yyleng = yyleng;
      Temp_yytext = STR_Save(yytext); yylineno = 1;  /* reset global counter of input lines */
      Temp_ActChr = SC_ActChr; Temp_InputStream = SC_InputStream; Temp_yysptr = yysptr;
      Temp_yysbuf = M_alloc((unsigned) YYLMAX); Temp_yysbuf = memcpy(Temp_yysbuf, yysbuf, YYLMAX);
      Temp_yyfnd = (yyfnd) ? *yyfnd : 0;
      SC_ActChr = NULL; Temp_InputStream = SC_InputStream; yysptr = yysbuf; 
      Temp_yylsp = yylsp; Temp_yyolsp = yyolsp; Temp_yyestate = yyestate; 
      Temp_yyprevious  = yyprevious;
      Temp_Topology = Topology;                  /* there have been fatal syntax errors - */
      Topology = TRUE; NewInput = TRUE;  /* set input variable for scanner i/o-management */
      val = yyparse();                                    /* parse subnet definition file */
      if(ListDepth-- > 0)    /* file has defined a subnet - there must be others on stack */
	SC_PopInputStack();              /* remove parsed input file from stack of inputs */
      Structures = TRUE;                                   /* reset environment variables */
      (void) memcpy((char*) yyv, (char*) Temp_yyv, YYMAXDEPTH *  sizeof(ParserStackType *));
      yylval = Temp_yylval; yyval = Temp_yyval; yychar = Temp_yychar; 
      (void) memcpy(yysbuf, Temp_yysbuf, YYLMAX);
      yytchar = Temp_yytchar; yymorfg = Temp_yymorfg; yyleng = Temp_yyleng;
      yynerrs = Temp_yynerrs; yyerrflag = Temp_yyerrflag; yylineno = Temp_yylineno;
      SC_ActChr = Temp_ActChr; SC_InputStream = Temp_InputStream; yysptr = Temp_yysptr;
      yylsp = Temp_yylsp; yyolsp = Temp_yyolsp; yyestate = Temp_yyestate;
      XCenter -= XAdd->yvalue->intval; YCenter -= YAdd->yvalue->intval;
      if(yyfnd) *yyfnd = Temp_yyfnd;  *yyfnd = Temp_yyfnd;  strcpy(yytext, Temp_yytext);
      yyprevious = Temp_yyprevious;
      XCenter -= XAdd->yvalue->intval; YCenter -= YAdd->yvalue->intval;
      Topology = Temp_Topology; NewInput = FALSE; 
    }
    else
      val = TRUE;                                          /* error: recursive subnets  */
  }
  PA_FreeParserStack(XAdd);
  PA_FreeParserStack(YAdd);
  PA_FreeParserStack(InFile);
  return val;
}





/*****************************************************************************************
 Function:                     PA_GetParserStack
 Purpose: Allocates and initializes a new parser stack record, if there are no records free.
 Returns: & ParserStackType.
 Side effects: Modifies PA_FreeParserPtr
*****************************************************************************************/
ParserStackType *PA_GetParserStack()
{
  register ParserStackType *p;       /* pointer to new parser stack record */

  if(PA_FreeParserPtr)
    PA_FreeParserPtr = (p=PA_FreeParserPtr)->ynext;
  else {
    p = (ParserStackType *) (M_alloc(sizeof(ParserStackType)));
    p->yvalue = (SymbolValueType *) (M_alloc(sizeof(SymbolValueType)));
  }
  p->ynext = NULL;
  p->ytype = Undef;
  return p;
}





/*****************************************************************************************
 Function:                     PA_FreeParserStack
 Purpose: Push an unused ParserStackType record on the stack PA_FreeParserPtr.
          ->Value is preserved.
 Returns: void
 Side effects: Modifies PA_FreeParserPtr. 
*****************************************************************************************/
void PA_FreeParserStack(p)
     register ParserStackType *p;
{
  if(p) {
    p->ynext = PA_FreeParserPtr;
    PA_FreeParserPtr = p;
  }
}




/*****************************************************************************************
 Function:                     PA_GetUndefValue
 Purpose: Allocates and initializes a new parser stack record whose ytype component is 
          Undef. This is needed to signalize errors to functions that perform other
	  semantic actions.
 Returns: & ParserStackType.
*****************************************************************************************/
ParserStackType *PA_GetUndefValue()
{
  register ParserStackType *p;       /* pointer to new parser stack record */

  p = PA_GetParserStack();
  p->ytype = Undef;
  return p;
}





/*****************************************************************************************
 Function:                     PA_GetUndefString
 Purpose: Allocates and initializes a new parser stack record whose ytype component is 
          string. Needed to distinguish undefined strings from illegal expressions. The 
	  string value is set to NULL.
 Returns: & ParserStackType.
*****************************************************************************************/
ParserStackType *PA_GetUndefString()
{
  register ParserStackType *p;       /* pointer to new parser stack record */

  p = PA_GetParserStack();
  p->ytype = SIMPLESTRING;
  p->yvalue->string = NULL;
  return p;
}








/*****************************************************************************************
 Function:                     yyNewerror
 Purpose: imprved error reports.
 NOTE: copied from Schreiner, Friedman: Introduction to Compiler Construction with UNIX,
       (C) 1985 Prentice-Hall, pp. 235-239. Modified.
*****************************************************************************************/
yyNewerror(s,t)
     register char *s, *t;
{
  extern int yynerrs;
  static int list = 0;

  char *ErrBuf, *Old;
  
  if(s  || ! list) {                                /* header for error message needed? */
    ErrBuf = STR_Save("syntax: ");
    /* yywhere(); not copied */
    if(s)  {                                                    /* simple error message */
      Old = ErrBuf;
      ErrBuf = STR_Append(ErrBuf, s);
      M_free(Old);
      ER_StackError(STR_Append(ErrBuf, "\n"), Fatal, nst,  nst, nst);  /* print message */
      ErrBuf = NULL;
      return;
    }
    if(t) {                                                           /* first symbol ? */
      Old = ErrBuf;
      ErrBuf = STR_Append(ErrBuf, "expecting '");
      M_free(Old); Old = ErrBuf;
      ErrBuf = STR_Append(ErrBuf, t);
      M_free(Old); 
      list  = 1;        /* do  not print error message, wait for other expected symbols */
      return;                 
    }
    /* no symbols can be inserted for a legal reduction */
    ER_StackError(STR_Save("syntax\n"), Fatal, nst, nst, nst); return;
  }
  if(t) {                                                    /* more symbols expected ? */
    Old = ErrBuf;
    ErrBuf = STR_Append(ErrBuf, " ");
    M_free(Old); Old = ErrBuf;
    ErrBuf = STR_Append(ErrBuf, t);
    M_free(Old); 
  }
  ER_StackError(STR_Append(ErrBuf, "'\n"), Fatal, nst,  nst, nst);/* no further  symbols */
  ErrBuf = NULL;                                                 /* print error message */
  list = 0;
}
