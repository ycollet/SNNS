%{
/*****************************************************************************
  FILE              : Tokens.l
  SHORTNAME         : Tokens
  VERSION           : 2.0

  PURPOSE           : lex definition file for scanner generation
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_scanner.h"
#define getc() SC_NextSymb()
#define DIM(x) (sizeof x / sizeof x[0])
#define YYERRCODE 256
#define LIST(x) SC_List(yytext, x)


                    /**** static variables for scanner ****/

char *SC_ActChr = NULL;        /* points to next character of actual input stream */
char *SC_InputStream = NULL;                    /* points to current input stream */
InputStackType *SC_InputStack = NULL;    /* points to stack of input descriptions */
static InputStackType *SC_FreeInput = NULL;   /* points to stack of unused input records */
short i;                                             /* auxiliar integer variable */

%}

%%
\n	       { LIST(Eol);
		 ER_ListErrors(yylineno-1, SymTabPtr->FileStr);
		 if(ListOption) {
		   for(i=0; i<ListDepth; i++)
		     (void) fprintf(ListFile, (TeXOption)?"~~~~~":"     ");   /* indent listings of  subnets */
		     if( ! TeXOption)
		       (void) fprintf(ListFile, "%5d   ", yylineno);
		   }
	       }
[ \t]          { LIST(Blank);                               /* tab or blank */}

"+"            { LIST(Verbatim); return SC_Recog( PLUS );      /* addition operator */}
"-"            { LIST(Verbatim); return SC_Recog( MINUS );     /* subtraction operator */ }
"*"            { LIST(Verbatim); return SC_Recog( MULT );      /* multiplication operator */ }
"/"            { LIST(Verbatim); return SC_Recog( RDIV );      /* division operator */ }
\|             { LIST(Verbatim); return SC_Recog( XSTRCAT );   /* string concatenation */ }
">"            { LIST(Verbatim); return SC_Recog( GT );        /* greater than */ }
"<"            { LIST(Verbatim); return SC_Recog( LT );        /* less than */ }
">="           { LIST(Verbatim); return SC_Recog( GT );        /* greater than or equal */ }
"<="           { LIST(Verbatim); return SC_Recog( LT );        /* less than or equal */ }
"<>"           { LIST(Verbatim); return SC_Recog( NE );        /* not equal */ }
"="            { LIST(Verbatim); return SC_Recog( EQ );        /* equal */ }
"->"           { LIST(Verbatim); return SC_Recog( PT );        /* forward connection , connected to*/ }
"<->"          { LIST(Verbatim); return SC_Recog( PTF );       /* bidirectional connection */ }
"<-"           { LIST(Verbatim); return SC_Recog( PF );        /* backward connection */ }

"."            { LIST(Verbatim); return SC_Recog( POINT );     /* selection operator or end of program */ }
","            { LIST(Verbatim); return SC_Recog( PERIOD );    /* list element separator, foreach-variables */ }
";"            { LIST(Verbatim); return SC_Recog( SEMICOLON ); /* statement separator*/ }
":"            { LIST(Verbatim); return SC_Recog( COLON );     /* map type specification end marker */ }

"("            { LIST(Verbatim); return SC_Recog( OPARENT );   /* begin of arrays, expressions */ }
")"            { LIST(Verbatim); return SC_Recog( CPARENT );   /* end of arrays, expressions */ }
"["            { LIST(Verbatim); return SC_Recog( OBRACKET );  /* begin of field size definition or array */ }
"]"            { LIST(Verbatim); return SC_Recog( CBRACKET );  /* end of field size definition or array */ }

":="           { LIST(Verbatim); return SC_Recog( ASSIGN );    /* assignment operator */ }
".."           { LIST(Verbatim); return SC_Recog( ARANGE );     /* range definition in array size specification */}


"{"[^}]*"}"    { LIST(Comment); /* comment */ 
#ifdef YYDEBUG
                 (void) printf("comment in line %d\n", yylineno);
#endif
}

[0-9]*"."[0-9]+                    { LIST(Math); return SC_Recog( FLOATNUM );   /* digits after point, no exp*/}
[0-9]+"."/[^.]      	           { LIST(Math); return SC_Recog( FLOATNUM );   /* recognize range in 2..3 */}
[0-9]+"."[Ee][+-]?[0-9]+           { LIST(Math); return SC_Recog( FLOATNUM );   /* recognize float 2.E3 */}
[0-9]*"."[0-9]+[Ee][+-]?[0-9]+     { LIST(Math); return SC_Recog( FLOATNUM );   /* digits after point + exp*/}
[0-9]+[Ee][+\-]?[0-9]+             { LIST(Math); return SC_Recog( FLOATNUM );   /* integer + exponent */}

[0-9]+                             { LIST(Identifier); return SC_Recog( INTEGER ); }


\"([^"]|\\\")+\"                   { LIST(Verbatim); return SC_Recog( LONGSTRING ); }
\"\"                               { LIST(Verbatim); return SC_WarnEmptyString(); }

[A-Za-z][A-Za-z0-9_]*              { return SC_GetType(yytext);  /* calls LIST */}

.                                  { LIST(Illegal); return SC_Recog( ILLEGAL ); }


%%


/*****************************************************************************************
 Function:                  SC_GetType   
 Purpose: Searches an identifier in the symbol table. Returns the identifiers type, which
          is one of IDENTIFIER, VARIABLE, or one of the keyword types. An IDENTIFIER is an
	  identifier which has not been found in the table (has been inserted by
	  SYM_LookUp), VARIABLE is an identifier, whose value and data type have been
	  defined already. SC_GetType calls list function.
 Returns:  integer, corresponding to token type
*****************************************************************************************/
int SC_GetType(Key)
  char *Key;                          /* identifier or keyword found */
{
  register SymTabLineType *Entry;     /* points to symbol table entry for key */

  Entry = SYM_LookUp(Key, SymTabPtr);
  if( ! Entry->isKeyword) {
    /* entry is not a keyword - get a parser value stack element */
    yylval = PA_GetParserStack();
    /* key is name of a defined variable, allocate a value  field for symbol table entry */
    yylval->ytype = IDENTIFIER;
    yylval->yvalue->var = Entry;
    LIST(Verbatim);
    return IDENTIFIER;
  }
  else {
    /* key is a keyword - Entry->Type holds the keywords token number */
    yylval = NULL;
    LIST(Keyword);
    return  Entry->Type;
  }
}




/*****************************************************************************************
 Function:                     SC_Recog
 Purpose: Builds a parser stack element and assigns a value to it. If the token is a
          literal yylval->ytype is integer and yylval->yvalue->intval the token number.
 Returns: Token number.
 Side effects: Modifies yylval.
*****************************************************************************************/
int SC_Recog(Token)
     int Token;     /* no. of token */
{
  char *SaveStr;    /* copy of string without delimiters */
  switch(Token) {
  case LONGSTRING:
    yylval = PA_GetParserStack();
    SaveStr = STR_RemDelimiters(yytext);
    if(strlen(SaveStr) == 1)
      /* string is of simple type - may be used with range operator */
      Token = SIMPLESTRING;
    yylval->ytype = Token;
    yylval->yvalue->string = STR_New(SaveStr);
    break;
  case INTEGER:
    yylval = PA_GetParserStack();
    yylval->ytype = Token;
    yylval->yvalue->intval = STR_AtoI(yytext);
    break;
  case FLOATNUM:
    yylval = PA_GetParserStack();
    yylval->ytype = Token;
    yylval->yvalue->floatval = SC_CalcFloat(yytext);
    break;
  default: 
    yylval = NULL;
    break;
  }
  return Token;
}




/*****************************************************************************************
 Function:                     SC_WarnEmptyString
 Purpose: Strings in nessus must not be empty. Generate a warning message and set string to 
          " " to avoid further error messages.
 Returns: Token number.
 Side effects: Modifies yylval.
*****************************************************************************************/
int SC_WarnEmptyString()
{
  yylval->ytype = SIMPLESTRING;
  yylval->yvalue->string = STR_Save(" ");
  ER_StackError("illegal empty string set to \" \"\n", Warning, nst, nst, nst);
  return SIMPLESTRING;
}


  



/*****************************************************************************************
 Function:                     SC_NextSymb()
 Purpose:  Buffered input for scanner.
 Returns:  Returns next symbol of input stream. 
*****************************************************************************************/
char SC_NextSymb()
{
  /* static variable ActChr is a pointer to next character to be returned by SC_NextSymb */
  /* static variable InputStream is a record with a pointer to the upper input buffer */
  if(NewInput) {
    NewInput = FALSE;
    if(! SC_InitInputStream()) {
      SC_PopInputStack();
      return -1;                                      /* could not open input file */
    }
    SYM_InitTab();                                    /* initialize new symbol table */
    SymTabPtr->FileStr = SC_InputStack->InFileName;   /* name of input file for symbol table */
  }
  /* check if it's necessary to reload the input buffer */
  if(SC_ActChr >= SC_InputStream + BUFSIZ)
    SC_ReloadInputStream();
  /* set ActChr to next symbol and return the one it has pointed to before */
  SC_ActChr ++;
  return *(SC_ActChr-1);
}




/*****************************************************************************************
 Function:                   SC_InitInputStream  
 Purpose:  Initializes a new input stream. Expects the name of a new input file to be on
           top of the input file names stack. This function is called whenever the parser
	   generated by yacc begins to parse a new input file (<=> NewInput == TRUE).
 Returns:  Returns TRUE if a new input stream has been opened, else FALSE (errors,
            nonexisting input file, etc.).
 Side effects:  Modifies stack of input streams (InStreamTop), InputStream, and Ac
tChr.
*****************************************************************************************/
short SC_InitInputStream()
{
  /* there is a new input description record on the stack already */
  /* open the input stream - get new file name from top of input stack */
  if(! (SC_InputStack->InFilePtr=fopen(SC_InputStack->InFileName,"r"))) {
    if(! SC_InputStack->NextInp) {
      (void) printf("fatal error: input file \"%s\" not found\n", SC_InputStack->InFileName);
       exit(NoInput);   
    }
    else {
      ER_StackError("file i/o: could not open input file %s\n", Fatal,
		    STR_Save(SC_InputStack->InFileName), nst, nst);
      return FALSE;
/*       exit(NoSubnet);   */
    }
    return FALSE;
  }
  /* set input buffer */
#ifdef SYSTEMV
  setbuf(SC_InputStack->InFilePtr, SC_InputStack->InFileBuf);
#else
  setbuffer ( SC_InputStack->InFilePtr, SC_InputStack->InFileBuf, BUFSIZ);
#endif
  /* fill input file buffer */
  SC_ReloadInputStream();
  return TRUE;         
}




/*****************************************************************************************
 Function:                     SC_ReloadInputStream
 Purpose: Reloads current input stream. Appends a NULL character to the input stream if
          less than BUFSIZ charcters have been read.
 Returns: void
 Side effects: Modifies input stream of upper input stack record. 
*****************************************************************************************/
void SC_ReloadInputStream()
{
  register int CharsRead;
  
  /*  read, if possible, <BUFSIZ>  symbols from the file <InputFile> */
  /*  into  <Inputbuffer>  and returns the number of bytes read */
  if((CharsRead=fread(SC_InputStack->InFileBuf,1,BUFSIZ,SC_InputStack->InFilePtr))<BUFSIZ)
    /* end of file read - append NULL character to finish up scanning of this file */
    *(SC_InputStack->InFileBuf+CharsRead) = '\0';
  /* set static variables which control SC_NextSymb(): SC_InputStream points to current */
  /* input buffer and SC_ActChar to actual symbol in it */
  SC_InputStream = SC_InputStack->InFileBuf;
  SC_ActChr = SC_InputStack->InFileBuf;
  return;
}




/*****************************************************************************************
 Function:                     SC_PutInputStack
 Purpose: Allocates a new input file desciption record and puts it on the stack of inputs
          (SC_InputStack). 
 Returns: void 
 Side effects: Modifies pointer to input stack SC_InputStack and SC_FreeInput. 
*****************************************************************************************/
short SC_PutInputStack(InName)
     char *InName;                                            /* name of new input file */
{
  register InputStackType *New;                   /* pointer to new input stack  record */

  for(New = SC_InputStack; New; New = New->NextInp)       /* look for recursive subnets */
    if(! strcmp(InName, New->InFileName)) {                   /* subnet contains itself */
      ER_StackError("recursive subnet structure on file '%s'\n",  Fatal, 
		    STR_Save(InName), nst, nst);
      return FALSE;                                           /* do not parse this file */
    }
  /* InputStackType records are  preserved after their remotion from stack - there is a */
  if(SC_FreeInput) {                            /* stack of unused records of this type */
    /* there are unused records on the stack SC_FreeInput */
    New = SC_FreeInput;
    SC_FreeInput = SC_FreeInput->NextInp;
  }
  else                                          /* no free records - allocate a new one */
    New = (InputStackType*) (malloc(sizeof(InputStackType)));
  New->InFileName = InName;                                   /* set name of input file */
  /* save pointer to next input character of the old  input file which remains on stack */
  if(SC_InputStack) {
    OUT_AppendSource(InName);                   /* append new file to string of sources */
    SC_InputStack->ActChar = SC_ActChr;
  }
  New->NextInp = SC_InputStack;  /* other fields are not initialized */
  SC_InputStack = New;           /* put record on stack */
  return TRUE;         
}



/*****************************************************************************************
 Function:                     SC_PopInputStack
 Purpose: Pops upper element from stack of input files and stores it on the stack of
          unused input records (SC_FreeInput points to its top).
 Returns: void
 Side effects: Modifies both stacks, of current input (SC_InputStack) and of unused
               records (SC_FreeInput). 
*****************************************************************************************/
void SC_PopInputStack()
{
  register InputStackType *Old;      /* points to record which is removed */
  /* remove record from stack of input files */
  if(SC_InputStack) {
    SC_InputStack = (Old = SC_InputStack)->NextInp;
    Old->NextInp = SC_FreeInput;
    SC_FreeInput = Old;
  }
  else
    (void) printf("******bug: unexpected call to SC_PopInputStack, no File\n");
}




/*****************************************************************************************
 Function:                     SC_Scanner
 Purpose: This function controls the flow of tokens from an input stream to the parser. In
          normal input mode it calls yylex (scanner generated by LEX), when processing a
	  program loop it may call functions either to stack tokens and their values or to
	  simulate a flow of tokens for parsing.
 Returns: int, token no.
*****************************************************************************************/
int SC_Scanner()
{
  int val;   /* token value */
  /* if scanner returns an irregular symbol value, print an error message. Illegal symbols */
  /* are not overread. */
  val = yyyylex();
#ifdef SCANTEST
  if(val <= 0)     /* eof reached */
    SYM_PrintEndTab();
#endif
  return val;
}




/*****************************************************************************************
 Function:                     yyyylex
 Purpose: yyyylex calls the scanner and displays the returned token in a readable form. 
 Returns: Same token number yylex returns.
 Note: Function is found in Schreiner,Friedman :
       Introduction to Compiler Construction with UNIX. 1985, Prentice-Hall.
*****************************************************************************************/
int yyyylex()
{
  register int yychar;
  yychar = yylex();
#ifdef YYDEBUG
    (void) printf("%s %s \n", "[yydebug] recognized token is",
		  yydisplay(yychar));
#endif
  return yychar;
}




/*****************************************************************************************
 Function:                     yydisplay
 Purpose: yydisplay translates the parsers numeric representation of tokens into characters.
 Returns: Token string.
 Note: Function is found in Schreiner,Friedman :
       Introduction to Compiler Construction with UNIX. 1985, Prentice-Hall.
*****************************************************************************************/
char *yydisplay(ch)
     register int ch;
{
    //char *sprintf();
    static char buf[20];
    static char *token[] = {
#include "co_tokPrint.h"
	, 0 };

    switch(ch) {
      case 0: 
	return "[end of file]";
      case YYERRCODE: 
	return "[ERROR]";
      case '\b':
	return "'\\b'";
      case '\f':
	return "'\\f'";
      case '\n':
	return "'\\n'";
      case '\r':
	return "'\\r'";
      case '\t':
	return "'\\t'";
    }    
    if(ch > 256 && ch < 256+DIM(token))
	return token[ch-257];
    if(isascii(ch) && isprint(ch))
	(void) sprintf(buf, "'%c'",ch);
    else if(ch < 256)
	(void) sprintf(buf, "char %04.30", ch);
    else 
	(void) sprintf(buf, " %d", ch);
    return buf;
}




/*****************************************************************************************
 Function:                     SC_List
 Purpose: If ListOption list token. If TeXOption: include LaTeX environment definition.
 Returns: void
*****************************************************************************************/
void SC_List(str, env)
     char *str;
     short env;
{
  register char *s;   /* needed to replace last } in comments by \0 */
  if(ListOption) {
    if(TeXOption) 
      switch(env) {
      case Verbatim:
	(void) fprintf(ListFile, "\\verb&%s&", str);
	break;
      case Math:
	(void) fprintf(ListFile, "$%s$", str);
	break;
      case Keyword:
	(void) fprintf(ListFile, "{\\bf %s}", str);
	break;
      case Comment:
	/* {} as comment delimiters are not accepted by TeX - we must write \{ and \} */
	for(s=str+1; *s; s++)
	  ;
	*(s-1) = '\0';     /* last '}' is eliminated */
	(void) fprintf(ListFile, "{\\small \\\{\\verb{%s{\\\}}", str+1);
	break;
      case Eol:
	(void) fprintf(ListFile, "\\\\%s", str);
	break;
      case Eof:
	(void) fprintf(ListFile, "\\\\ \n \\end{tabbing}\n");
	break;
      case Illegal:
	(void) fprintf(ListFile, "\\begin{verbatim}%s\\end{verbatim}", str);
	break;
      case Blank:
	(void) fprintf(ListFile, "~");
	break;
      default:
	(void) fprintf(ListFile, "%s", str);
      }
    else
      (void) fprintf(ListFile, "%s", str);
  }
}


/*****************************************************************************************
 Function:                     SC_CalcFloat
 Purpose: Convert string representation of FLOATNUM into double.
 Returns: double.
 Side effects: Original string is destroyed.
*****************************************************************************************/
double SC_CalcFloat(Str)
     char           *Str;          /* string value */
{
  register char *Scan;             /* points to a symbol of numerical string */
  short IsFloat;                   /* TRUE <=> string contains decimal point */
  char *Expon;                     /* points to 1st. symbol of exponent, if there is one */
  register double ExpSign;         /* sign of an eventual exponent, basis must be unsigned */

  IsFloat = FALSE;
  Expon = nst;
  ExpSign = 1.0;
  for(Scan = (*Str == '+' || *Str == '-')? Str+1: Str; *Scan; Scan ++)
    /* scan string and look for illegal symbols, overread a leading sign */
    if( ! isdigit(*Scan))
      if(*Scan == '.')              /* decimal point */
	if( ! IsFloat)              /* therer have been no decimal points before */
	  IsFloat = TRUE;
	else                        /* more than a single decimal point in float argument */
	  if(Expon)                 /* 2nd. '.' decimal point in exponent */
	    ER_StackError("exponent in argument %s must be integer\n", Warning,
			  STR_Save(Str), nst, nst);
	  else {                    /* cannot evaluate ergument, basis contains two '.' */
	    ER_StackError((Expon)?"exponent must be integer in argument %s\n":
	    "too many decimal points in argument %s\n", Serious, STR_Save(Str), nst, nst);
	    return 0.0;                 /* formal parameter remains undefined ! */
	  }
      else if(*Scan == 'e' || *Scan == 'E')  {          /* exponent follows */
	if(! Expon) {               /* first (and unique?) exponent */
	  *(Scan++) = '\0';             /* basis must be NULL terminated for evaluation */
	  if(*Scan == '-') {
	    ExpSign = -1.;
	    Expon = ++Scan;                /* overread eventual sign of exponent */
	  }
	  else if(*Scan == '+') 
	    Expon = ++Scan;           /* mark begin of exponent, it is terminated, already */
	}
	else {                      /* 2nd. exponent detected */
	  ER_StackError("too many exponents in numerical argument %s\n", Serious,
			STR_Save(Str), nst, nst);
	  return 0.0;
	}
      }
      else {                        /* illegal symbol */
	ER_StackError("illegal symbol in numerical argument %s\n", Serious,
		      STR_Save(Str), nst, nst);
	return 0.0;
      }
  /* string is a correct representation for a numerical argument */
  if(IsFloat)
    return (Expon)? STR_AtoF(Str)*exp10(ExpSign * (double)STR_AtoI(Expon)): STR_AtoF(Str);
  else
    return (double) ((Expon)? STR_AtoI(Str)*exp10(ExpSign * (double)STR_AtoI(Expon)): STR_AtoI(Str));
}
