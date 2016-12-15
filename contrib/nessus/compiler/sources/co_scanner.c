# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
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

# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
       { LIST(Eol);
		 ER_ListErrors(yylineno-1, SymTabPtr->FileStr);
		 if(ListOption) {
		   for(i=0; i<ListDepth; i++)
		     (void) fprintf(ListFile, (TeXOption)?"~~~~~":"     ");   /* indent listings of  subnets */
		     if( ! TeXOption)
		       (void) fprintf(ListFile, "%5d   ", yylineno);
		   }
	       }
break;
case 2:
         { LIST(Blank);                               /* tab or blank */}
break;
case 3:
           { LIST(Verbatim); return SC_Recog( PLUS );      /* addition operator */}
break;
case 4:
           { LIST(Verbatim); return SC_Recog( MINUS );     /* subtraction operator */ }
break;
case 5:
           { LIST(Verbatim); return SC_Recog( MULT );      /* multiplication operator */ }
break;
case 6:
           { LIST(Verbatim); return SC_Recog( RDIV );      /* division operator */ }
break;
case 7:
            { LIST(Verbatim); return SC_Recog( XSTRCAT );   /* string concatenation */ }
break;
case 8:
           { LIST(Verbatim); return SC_Recog( GT );        /* greater than */ }
break;
case 9:
           { LIST(Verbatim); return SC_Recog( LT );        /* less than */ }
break;
case 10:
          { LIST(Verbatim); return SC_Recog( GT );        /* greater than or equal */ }
break;
case 11:
          { LIST(Verbatim); return SC_Recog( LT );        /* less than or equal */ }
break;
case 12:
          { LIST(Verbatim); return SC_Recog( NE );        /* not equal */ }
break;
case 13:
           { LIST(Verbatim); return SC_Recog( EQ );        /* equal */ }
break;
case 14:
          { LIST(Verbatim); return SC_Recog( PT );        /* forward connection , connected to*/ }
break;
case 15:
         { LIST(Verbatim); return SC_Recog( PTF );       /* bidirectional connection */ }
break;
case 16:
          { LIST(Verbatim); return SC_Recog( PF );        /* backward connection */ }
break;
case 17:
           { LIST(Verbatim); return SC_Recog( POINT );     /* selection operator or end of program */ }
break;
case 18:
           { LIST(Verbatim); return SC_Recog( PERIOD );    /* list element separator, foreach-variables */ }
break;
case 19:
           { LIST(Verbatim); return SC_Recog( SEMICOLON ); /* statement separator*/ }
break;
case 20:
           { LIST(Verbatim); return SC_Recog( COLON );     /* map type specification end marker */ }
break;
case 21:
           { LIST(Verbatim); return SC_Recog( OPARENT );   /* begin of arrays, expressions */ }
break;
case 22:
           { LIST(Verbatim); return SC_Recog( CPARENT );   /* end of arrays, expressions */ }
break;
case 23:
           { LIST(Verbatim); return SC_Recog( OBRACKET );  /* begin of field size definition or array */ }
break;
case 24:
           { LIST(Verbatim); return SC_Recog( CBRACKET );  /* end of field size definition or array */ }
break;
case 25:
          { LIST(Verbatim); return SC_Recog( ASSIGN );    /* assignment operator */ }
break;
case 26:
          { LIST(Verbatim); return SC_Recog( ARANGE );     /* range definition in array size specification */}
break;
case 27:
   { LIST(Comment); /* comment */ 
#ifdef YYDEBUG
                 (void) printf("comment in line %d\n", yylineno);
#endif
}
break;
case 28:
                   { LIST(Math); return SC_Recog( FLOATNUM );   /* digits after point, no exp*/}
break;
case 29:
     	           { LIST(Math); return SC_Recog( FLOATNUM );   /* recognize range in 2..3 */}
break;
case 30:
          { LIST(Math); return SC_Recog( FLOATNUM );   /* recognize float 2.E3 */}
break;
case 31:
    { LIST(Math); return SC_Recog( FLOATNUM );   /* digits after point + exp*/}
break;
case 32:
            { LIST(Math); return SC_Recog( FLOATNUM );   /* integer + exponent */}
break;
case 33:
                            { LIST(Identifier); return SC_Recog( INTEGER ); }
break;
case 34:
                  { LIST(Verbatim); return SC_Recog( LONGSTRING ); }
break;
case 35:
                              { LIST(Verbatim); return SC_WarnEmptyString(); }
break;
case 36:
             { return SC_GetType(yytext);  /* calls LIST */}
break;
case 37:
                                 { LIST(Illegal); return SC_Recog( ILLEGAL ); }
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */


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
    char *sprintf();
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
int yyvstop[] = {
0,

37,
0,

2,
37,
0,

1,
0,

37,
0,

21,
37,
0,

22,
37,
0,

5,
37,
0,

3,
37,
0,

18,
37,
0,

4,
37,
0,

17,
37,
0,

6,
37,
0,

33,
37,
0,

20,
37,
0,

19,
37,
0,

9,
37,
0,

13,
37,
0,

8,
37,
0,

36,
37,
0,

23,
37,
0,

24,
37,
0,

37,
0,

7,
37,
0,

35,
0,

14,
0,

26,
0,

28,
0,

-29,
0,

33,
0,

25,
0,

16,
0,

11,
0,

12,
0,

10,
0,

36,
0,

27,
0,

34,
0,

34,
0,

29,
0,

28,
29,
0,

29,
0,

32,
0,

15,
0,

31,
0,

30,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,6,	
0,0,	26,43,	0,0,	28,44,	
0,0,	1,7,	1,8,	1,9,	
1,10,	1,11,	1,12,	1,13,	
1,14,	1,15,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,16,	
1,17,	1,18,	1,19,	1,20,	
12,29,	16,35,	1,21,	2,7,	
2,8,	2,9,	1,21,	2,11,	
2,12,	13,30,	2,14,	13,31,	
13,31,	13,31,	13,31,	13,31,	
13,31,	13,31,	13,31,	13,31,	
13,31,	2,16,	2,17,	2,18,	
2,19,	2,20,	6,26,	20,39,	
1,22,	36,51,	1,23,	26,28,	
1,3,	28,28,	6,26,	6,26,	
18,36,	0,0,	44,43,	0,0,	
15,32,	0,0,	15,33,	15,33,	
15,33,	15,33,	15,33,	15,33,	
15,33,	15,33,	15,33,	15,33,	
18,37,	18,38,	2,22,	0,0,	
2,23,	0,0,	0,0,	6,27,	
1,24,	1,25,	1,3,	15,34,	
0,0,	0,0,	0,0,	0,0,	
6,26,	0,0,	34,49,	6,26,	
34,49,	6,26,	0,0,	34,50,	
34,50,	34,50,	34,50,	34,50,	
34,50,	34,50,	34,50,	34,50,	
34,50,	0,0,	2,24,	2,25,	
0,0,	0,0,	6,26,	0,0,	
0,0,	0,0,	6,26,	15,34,	
44,28,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	0,0,	0,0,	
0,0,	6,28,	0,0,	0,0,	
6,26,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	0,0,	
0,0,	0,0,	6,26,	21,40,	
0,0,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	21,40,	
21,40,	21,40,	21,40,	24,41,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	24,41,	
24,41,	31,31,	31,31,	31,31,	
31,31,	31,31,	31,31,	31,31,	
31,31,	31,31,	31,31,	32,46,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	32,46,	
32,46,	0,0,	31,45,	0,0,	
24,41,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	24,41,	0,0,	0,0,	
24,41,	0,0,	24,41,	0,0,	
0,0,	0,0,	0,0,	0,0,	
32,46,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	32,46,	31,45,	24,41,	
32,0,	0,0,	32,47,	24,41,	
45,52,	0,0,	45,52,	0,0,	
0,0,	45,53,	45,53,	45,53,	
45,53,	45,53,	45,53,	45,53,	
45,53,	45,53,	45,53,	32,46,	
0,0,	0,0,	0,0,	32,48,	
0,0,	48,54,	0,0,	48,54,	
0,0,	24,41,	48,55,	48,55,	
48,55,	48,55,	48,55,	48,55,	
48,55,	48,55,	48,55,	48,55,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	32,46,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	24,42,	
49,50,	49,50,	49,50,	49,50,	
49,50,	49,50,	49,50,	49,50,	
49,50,	49,50,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	32,46,	
52,53,	52,53,	52,53,	52,53,	
52,53,	52,53,	52,53,	52,53,	
52,53,	52,53,	54,55,	54,55,	
54,55,	54,55,	54,55,	54,55,	
54,55,	54,55,	54,55,	54,55,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-27,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+0,	0,		yyvstop+6,
yycrank+-89,	0,		yyvstop+8,
yycrank+0,	0,		yyvstop+10,
yycrank+0,	0,		yyvstop+13,
yycrank+0,	0,		yyvstop+16,
yycrank+0,	0,		yyvstop+19,
yycrank+0,	0,		yyvstop+22,
yycrank+2,	0,		yyvstop+25,
yycrank+27,	0,		yyvstop+28,
yycrank+0,	0,		yyvstop+31,
yycrank+58,	0,		yyvstop+34,
yycrank+4,	0,		yyvstop+37,
yycrank+0,	0,		yyvstop+40,
yycrank+55,	0,		yyvstop+43,
yycrank+0,	0,		yyvstop+46,
yycrank+30,	0,		yyvstop+49,
yycrank+120,	0,		yyvstop+52,
yycrank+0,	0,		yyvstop+55,
yycrank+0,	0,		yyvstop+58,
yycrank+-242,	0,		yyvstop+61,
yycrank+0,	0,		yyvstop+63,
yycrank+-3,	yysvec+6,	0,	
yycrank+0,	0,		yyvstop+66,
yycrank+-5,	yysvec+6,	0,	
yycrank+0,	0,		yyvstop+68,
yycrank+0,	0,		yyvstop+70,
yycrank+205,	0,		yyvstop+72,
yycrank+-262,	0,		yyvstop+74,
yycrank+0,	yysvec+15,	yyvstop+76,
yycrank+91,	0,		0,	
yycrank+0,	0,		yyvstop+78,
yycrank+31,	0,		yyvstop+80,
yycrank+0,	0,		yyvstop+82,
yycrank+0,	0,		yyvstop+84,
yycrank+0,	0,		yyvstop+86,
yycrank+0,	yysvec+21,	yyvstop+88,
yycrank+0,	yysvec+24,	0,	
yycrank+0,	0,		yyvstop+90,
yycrank+0,	0,		yyvstop+92,
yycrank+-68,	yysvec+6,	yyvstop+94,
yycrank+269,	0,		0,	
yycrank+0,	0,		yyvstop+96,
yycrank+0,	yysvec+31,	yyvstop+98,
yycrank+290,	0,		yyvstop+101,
yycrank+320,	0,		0,	
yycrank+0,	yysvec+49,	yyvstop+103,
yycrank+0,	0,		yyvstop+105,
yycrank+340,	0,		0,	
yycrank+0,	yysvec+52,	yyvstop+107,
yycrank+350,	0,		0,	
yycrank+0,	yysvec+54,	yyvstop+109,
0,	0,	0};
struct yywork *yytop = yycrank+407;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
011 ,01  ,'"' ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,'+' ,01  ,'+' ,'.' ,01  ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'A' ,'A' ,'A' ,'A' ,'E' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,01  ,'_' ,
01  ,'A' ,'A' ,'A' ,'A' ,'E' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,'}' ,01  ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,1,0,0,
0,0,0,0,0,0,0,0,
0};
#ifndef lint
static	char ncform_sccsid[] = "@(#)ncform 1.6 88/02/08 SMI"; /* from S5R2 1.2 */
#endif

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
