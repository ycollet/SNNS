/*****************************************************************************
  FILE              : co_main.c
  SHORTNAME         : main
  VERSION           : 2.1

  PURPOSE           : contains upper level functions and global variable definitions. 
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : Alex Hummler
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_main.h"

/* variable to get and print out compilation time, poi will be seed for random function */
long poi;



/*****************************************************************************************
                                 global variable definitions
*****************************************************************************************/

/**** avoid lint error messages ****/
NullStr nst = NULL;
ParserStackType *psNULL = NULL;

/**** compiler identification ****/
char *VersionDate  = "6/27/91";
char *VersionNo  = "V2.1-3D";                                     /* v2.1 */

/* sorted list of simple strings for array range definitions */
char *StringOrder[NoChars] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
				  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
				  "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
				  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
				  "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

/**** error counters ****/
short FatalErrs = 0;              /* errors which inhibit network evaluation */
short SeriousErrs = 0;            /* network is evaluated as far as possible, no output */
short WarningErrs = 0;            /* compiling is done completely - may cause semantic errors */

/**** intern compiler report ****/
long HeapSize = 0;                /* current size of data structures allocated by malloc() */
long MaxHeapSize = 0;             /* maximum size of data structures allocated by malloc() */

/**** statistics for output file ****/
int UnitCtr = 0;                  /* total number of units generated - used to number new units */
int ConnectionCtr = 0;            /* total number of connections generated */
int SiteTypeCtr = 0;              /* total number of sites generated */
int UnitTypeCtr = 0;              /* total number of named unit types */
int OutFuncCtr = 0;               /* number of defined output functions */
int ActFuncCtr = 0;               /* number of defined activation functions */
int SiteFuncCtr = 0;              /* number of defined site functions */
    
/**** default values for unit, site, and connection parameters ****/
char *DefActFunc="Act_Logistic";  /* default activation function */
char *DefOutFunc="Out_Identity";  /* default output function */

/* default values are defined as constants => Comp.Constants.h */

/**** variables used to synchronize and interface scanner,parser and output ****/
short NewInput = TRUE;            /* NewInput==TRUE <=> scanner must begin to read a new file */
FILE *ListFile;                   /* pointer to listing file */
int   ListDepth = -1;             /* depth of nested subnets */
short RightSide = TRUE;           /* TRUE when parsing constant or structure definitions or */
			          /* when at right side  of an assignment statement */
short Structures = FALSE;         /* is set while structures are beeing parsed - needed to */
			          /* supress redefinition of connection messages in */
			          /* map-defined structures (IRREGULAR) */
int XCenter = 0;                  /* actual X center of network or subnet */
int YCenter = 0;                  /* actual Y center of network or subnet */

/**** variables which control parser states ****/
short Topology = TRUE;            /* TRUE <==> parsing topology definition, no errors */
short Functions = FALSE;          /* TRUE <==> parsing function definitions => Topology = FALSE */
short StackMode  =  FALSE;        /* TRUE <==> stack tokens, do not execute */
short ConditionMode =  FALSE;     /* TRUE <==> currently evaluating a condition */
short InitMode =  FALSE;          /* TRUE <==> currently initializing loop  variaables */
short EvalLoopMode = FALSE;       /* TRUE <==> currently evaluating loop statements */
short AbortLoop = FALSE;          /* TRUE <==> serious error, exit EvalLoopMode (error cascades) */
short Execute = TRUE;             /* FALSE <==> stacking tokens, no semantic actions */




/**** options ****/
short ListOption = FALSE;     /* source listing if ListOption, only */
short TeXOption = FALSE;      /* generate LaTeX-Output, when TEST is defined: no semantic actions */
  
/**** file names ****/
char *InFileName = NULL;                     /* name of (main) input file */
char *OutFileName = NULL;                    /* name of output file (target) */
char *ListFileName = NULL;                   /* name of file for source listing - if NULL -> stdout */

/**** variable to hold actual program arguments (defined at compile time) ****/
ParserStackType *NetArguments = NULL;        /* stack of actual program parameter values */

/**** variable to hold list of network components ****/
SymTabLineType **NetComponents = NULL;       /* array of net components symbol table entries */
int NoNetComponents = NetCompLength;         /* initial length of NetComponents */
int NetCompCtr = 0;                          /* no. of insertion into net components array */
SymTabType *SymTabPtr;                       /* pointer to top of symbol tables stack */

ParserStackType *UnitTypes = NULL;           /* stack of unit types */
ParserStackType *SiteTypes = NULL;           /* stack of site types */
SiteListType *SiteList = NULL;               /* top of sites stack*/




/*****************************************************************************************
                                local variable definitions
*****************************************************************************************/




/*****************************************************************************************
 Function:                     main
 Purpose: main function of nessus compiler, calls parser and output functions
 Returns: void
*****************************************************************************************/
void main(ArgCtr, ArgVector)
     int ArgCtr;         /* number of arguments */
     char *ArgVector[];  /* vector of argument strings */
{
  ParserStackType  *InFile;
  /* evaluate arguments - proceed compiling if the arguments were correct */
  (void) printf("\nnessus compiler %s of %s\n\n", VersionNo, VersionDate);
  if(EvalArguments(ArgCtr, ArgVector)) {
    /* now, set up the compiling environment (InFileName, OutFileName, options, etc.) */
    if(InitCompiler()) {
      InFile = PA_GetParserStack();             /* allocate parser value stack record */
      InFile->ytype = (strlen(InFileName) > 1) ?  LONGSTRING : SIMPLESTRING;
      InFile->yvalue->string = InFileName;
      if(PA_Parser(InFile, NetArguments, PA_GetUndefValue(), PA_GetUndefValue())) {
	/* parser returns TRUE if some fatal (syntax) error has been detected */
	Statistics();
	exit(IllegalProgram);
      }
      if(SeriousErrs || FatalErrs) {
	Statistics();
	exit(IllegalProgram);
      }
      else {
	/* no errors or warnings, only - write network representation */
	if( ! WarningErrs)
	  (void) printf("no syntax errors\n\n");
	else
	  (void) printf("\n");
	OUT_GenOutput(OutFileName);
	Statistics();
	/* if new functions have been defined, include them into the kernels library */
	CF_IncludeNewFunctions();
      }
#ifdef TEST
      TE_Statistics();
#endif
      exit(NetGenerated);
    }
  }
  else {
    (void) printf( "Usage: nessus <input file> [ -<option> ...]\n\n" );
    exit(IllegalCall);
  }
}

     


/*****************************************************************************************
 Function:                     EvalArguments
 Purpose:  Evaluates the compilers argument list 
 Returns:  TRUE if argument list has been correct, else FALSE.
 Side effects: Modifies InFileName and ListOption.
*****************************************************************************************/
short EvalArguments(Ctr, List)
     int Ctr;         /* no. of arguments (but first argument is the compiler itself */
     char *List[];    /* vector of arguments - "-x" strings  -  separator is " " */
{
  register int ActArgNo;    /* no. of actually evaluated argument */
  
  /* the first (real) argument is the name of the network definition file - if */
  /* EvaluateArguments encounters the first argument beginning with '-' this and all */
  /* further arguments will be considered as options. They are evaluated by */
  /* EvaluateOptions */
  if(Ctr<2 || Ctr >= 2 && *List[1] == '-') {
    (void) printf( "Usage: nessus <input file> [ -<option> ...]\n\n" );
    exit(IllegalCall);
  }
  else {
    /* set name of input file - string is not saved !! */
    InFileName = List[1];
    OutFileName = STR_Append(InFileName, ".snns");         /* if not redefined later */
    /* other arguments must be options */
    for(ActArgNo = 2; ActArgNo < Ctr ;)
      if(* List[ActArgNo] != '-') 
	/* argument is not an option - missing input file name */
	return FALSE;
      else {
	switch(*(List[ActArgNo]+1)) {
	  /* evaluate 2nd. letter of argument option */
	case 'd':    /* program parameters defined at compile time */
	case 'D':
	  ActArgNo = EvalParamOption(ActArgNo+1, Ctr, List);
	  break;
	case 'l':    /* source listing */
	case 'L':
	  ListOption = TRUE;
	  ActArgNo = EvalListOption(ActArgNo+1, Ctr, List);
	  break;
	case 'o':    /* specification of target code file */
	case 'O':
	  ActArgNo = EvalOutOption(ActArgNo+1, Ctr, List);
	  break;
	case 't':    /* generate listing for LaTeX - default is simple listing */
	case 'T':
	  TeXOption = TRUE;
	  ActArgNo ++;
	  break;
	default:
	  /* error - illegal option */
	  (void) printf("illegal option %s.\n", List[ActArgNo]);
	  return FALSE;
	}
	if(! ActArgNo)
	  /* error - one of the options has illegal arguments */
	  return FALSE;
      }
  }
  return TRUE;
}





/*****************************************************************************************
 Function:                     EvalListOption
 Purpose:  Evaluates the argument of the -l option.
 Returns:  No. of next option if argument is correct, else FALSE;
 Side effects: Modifies ListFileName;
*****************************************************************************************/
short EvalListOption(Act, Max, List)
     short Act;    /* actual argument */
     short Max;    /* no. of arguments */
     char *List[]; /* program argument list */
{
  /* the -l option has at most one argument. */
  if(Act < Max)
    /* argument exists */
    if(*List[Act] == '-')
      /* no file name defined for listing - listing is directed to stdout */
      return Act;
    else {
      /* name of listing file specified */
      ListFileName = List[Act];
      return Act+1;
    }
  else
    /* no further arguments - no listing file name, listing directed to stdout */
    return Act;
}





/*****************************************************************************************
 Function:                     EvalOutOption
 Purpose:  Evaluates the argument of the -o option.
 Returns:  No. of next option if argument is correct, else FALSE;
 Side effects: Modifies OutFileName;
*****************************************************************************************/
short EvalOutOption(Act, Max, List)
     short Act;    /* actual argument */
     short Max;    /* no. of arguments */
     char *List[]; /* program argument list */
{
  /* the -o option has at most one argument. */
  if(Act < Max)
    /* argument exists */
    if(*List[Act] == '-')
      /* no output file name specified - default is "<input file>.o" */
      return Act;
    else {
      OutFileName = List[Act];
      return Act+1;
    }
  else {
      /* no output file name specified - default is "<input file>.o" */
      OutFileName = STR_Append(InFileName, ".o");
      return Act+1;
    }
}




/*****************************************************************************************
 Function:                     EvalParamOption
 Purpose: Builds a stack of parameter values specified in command line.
 Returns: No. of next option.
 Side effects: Parameter values are stored in NetArguments.
*****************************************************************************************/
short EvalParamOption(Act, Max, List)
     short Act;                                                      /* actual argument */
     short Max;                                                     /* no. of arguments */
     char *List[];                                             /* program argument list */
{
  register short Scan;                                      /* index of actual argument */
  float *fNum;                                      /* dummy value for sprint (isfloat) */
  ParserStackType *Temp;                            /* hold parameter value temporarily */
  ParserStackType *Last;        /* pointer to last element on list of actual parameters */

  /* build stack of parameter values */
  for(Scan = Act; Scan < Max && 
      (*(List[Scan]) != '-' || isfloat(List[Scan], fNum)); Scan++) {
    Temp = PA_GetParameterValue(List[Scan]);
    if( ! NetArguments)
      Last = (NetArguments = Temp);
    else 
      Last = (Last->ynext = Temp);
  }
  if(Scan == Act)
    /* there have been no arguments - print a warning message */
    ER_StackError("parameter values expected after -d option\n", Warning, nst,nst,nst);
  return Scan;    /* index of next option */
}




/*****************************************************************************************
 Function:                     InitCompiler
 Purpose: Initializes compiler.
 Returns: TRUE after successful initialization, else FALSE.
*****************************************************************************************/
short InitCompiler()
{
  NetComponents = NULL;       /* array of network components */
  SymTabPtr = NULL;           /* pointer to top of symbol tables stack */
  OUT_SetSource(InFileName);  /* initialize string of source file names */
  if(ListOption)              /* try to open listing file */
    if(! (ListFile = fopen(ListFileName, "w"))) {        /* out of space? */
      (void) printf("warning: cannot open listing file %s. No input listing.\n");
      ListOption = FALSE;
    }
    if(ListFileName) {
      if(! (ListFile = fopen(ListFileName, "w"))) {        /* out of space? */
	(void) printf("warning: cannot open listing file %s. No input listing.\n");
	ListOption = FALSE;
      }
      else if(TeXOption)
	(void) fprintf(ListFile, "\\begin{tabbing}bbbb\\=bbbb\\=bbbb\\=bbbb\\=bbbb\\=bbbb\\=\\kill\n");
    }
    else   /* no file name specified - direct listing to standard output */
      ListFile = stdout;
#ifdef NOACTIONS              /* for documentation and test: no semantic actions */
  if(TeXOption)
    Topology = FALSE;
#endif
  (void) time(&poi);
  /* initialize random function drand48 for connection weights and activation values */
  srand48(poi);    
  return TRUE;
}



/*****************************************************************************************
 Function:                     Statistics
 Purpose: Print no. of errors, etc. on screen and into listing file.
 Returns: void
*****************************************************************************************/
void Statistics()
{
  ER_ListErrors(yylineno, SymTabPtr->FileStr);
  /* get time at end of compilation */
  (void) time(&poi);                      /* get time in seconds since UNIX 0 hour (GMT) */
  /* ctime will transform GMT time into local time string  to be printed out */
  if(FatalErrs) (void) printf("fatal errors:   %d\n", FatalErrs);
  if(SeriousErrs) (void) printf("serious errors: %d\n", SeriousErrs);
  if(WarningErrs) (void) printf("warnings:       %d\n", WarningErrs);
  if(FatalErrs || SeriousErrs)
    (void) printf("no network generated due to errors\n");
  else
    (void) printf("net of %d units and %d connections generated\n", UnitCtr, ConnectionCtr);
  (void) printf("\ncompilation finished at %s \n\n", ctime(&poi));
  if(ListOption && ListFile != stdout && ! TeXOption) {
    if(TeXOption) 
      (void) printf("LaTeX listing file is %s\n", ListFileName);
    else
      (void) printf("source listing file is %s\n", ListFileName);
    (void) fprintf(ListFile, "\nfatal errors:  %d\n serious errors:  %d\n warnings:  %d\n",
		   FatalErrs, SeriousErrs, WarningErrs);
    if(FatalErrs || SeriousErrs)
      (void) fprintf(ListFile, "no network generated due to errors\n");
    else
      (void) fprintf(ListFile, "net of %d units and %d connections generated\n", UnitCtr, ConnectionCtr);
    (void) printf("\ncompilation finished at %s \n\n", ctime(&poi));
  }
#ifdef HEAPTEST
  (void) printf("Speicherflaechen am Ende : \n\n");
  mallocmap();
#endif
}





/*****************************************************************************************
 Function:                     TE_Statistics
 Purpose: 
 Returns:  
 Side effects: 
*****************************************************************************************/
void TE_Statistics()
{}
