/*****************************************************************************
  FILE              : co_strings.c
  SHORTNAME         : strings
  VERSION           : 2.0

  PURPOSE           : functions for string manipulation
                          - STR_Save           - STR_TypeString
			  - STR_AtoI           - STR_RemDelimiters
			  - STR_ItoA           - STR_OrderedString
			  - STR_ATOF           - STR_Ord
			  - STR_Append         - STR_Chr
			  - STR_Remove
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_strings.h"
#define DIM(x) (sizeof x / sizeof x[0])


/*****************************************************************************************
 Function:                     STR_Save
 Purpose: Save string.
 Returns: Pointer to copy of string.
 Note : This function is copied from Kernighan,Ritchie: Programming in C, pg. 111.
*****************************************************************************************/
char *STR_Save(s)
     char *s;
{
  char *p;
  if ((p = M_alloc((unsigned)(strlen(s)+1))) != NULL) {
    *p = '\0';
    p = strcpy(p,s);
  }
  return(p);
}




/****************************************************************************************
 FUNCTION :             STR_AtoI
 PURPOSE : Transforms a string into the corresponding integer value;
 RETURNS : Integer
 NOTE : Algorithm from Kernighan,Ritchie: Programming in C, pg. 44.
 ****************************************************************************************/
int STR_AtoI (StringPtr)
     char *StringPtr;            /* points to 'numeric' string to be transformed */
{
    register char *ActCharPtr;   /* points to current position in the string */
    register int Number;         /* resulting integer value */

    Number = 0;
    /* string corresponding to integer value is terminated by EndOfWord '\0' */
    for ( ActCharPtr = StringPtr; *ActCharPtr; ActCharPtr ++ )
	/* increment result value and shift one position (*10) left */
	/* digits 0 to 9 must be compact in the binary code (e.g., ASCII) */
	Number = 10 * Number + *ActCharPtr - '0';
    return Number;
}





/*****************************************************************************************
 Function:                     STR_ItoA
 Purpose: Transform an integer to its corresponding string.
 Returns: String.
*****************************************************************************************/
char *STR_ItoA(num)
     int num;
{
  register short i, pos;
  register char *res;
  /* calculate length of string (digits of integer + 1) */
  for(pos=1, i=num; i/=10; pos ++)
    ;
  if(num<0)
    pos++;         /* extra character for sign */
  res = M_alloc((unsigned)(pos+1));
  (void) sprintf(res, "%d", num);
  return res;
}
  



/********************************************************************
 FUNCTION :             STR_ATOF
 PURPOSE : Transform string 'abcd.xyz' into the corresponding real value
           (data type is double).
 RETURNS : Double. 
 NOTE : Algorithm from Kernighan,Ritchie: Programming in C, pg. 65.
 *******************************************************************/

double STR_AtoF (StringPtr)
     char *StringPtr;               /* points to 'numeric' string to be transformed */
{
    register char *ActCharPtr;      /* points to current position in the string */
    register double Number;         /* resulting double precision real value */
    register double Divisor; 

    Number = 0.0;
    /* string must have the form '<integer part>.<fraction>', no exponent */
    /* first, convert the integer part */
    for ( ActCharPtr = StringPtr; *ActCharPtr && *ActCharPtr != '.'; ActCharPtr ++ )
	Number = Number * 10 + *ActCharPtr - '0';
    if ( *ActCharPtr != '.' ) {
	/* we tried to convert an integer string into a double value - there */
	/* must be something wrong with the scanner */
      (void)printf("******>> bug: STR_AtoF applied on integer %s\n", StringPtr);
      return 0.0;
    }
    else {
	/* add fraction to the result, the first digit after the dot is divided */
	/* by 10, the subsequent by 100, and so on. The string must be */
	/* terminated by EndOfWord '\0' */
	for ( Divisor = 10, ActCharPtr ++; *ActCharPtr; 
	     Divisor *= 10, ActCharPtr ++ )
	    Number += (*ActCharPtr - '0') / Divisor;
	return Number;
    }
}




/*****************************************************************************************
 Function:                     STR_Append
 Purpose: Appends 2nd. string argumnent to first. Returns new string.
 Returns: &char
*****************************************************************************************/
char *STR_Append(s1, s2)
     char *s1, *s2;
{
  char *res;
  *(res = M_alloc((unsigned) strlen(s1)+strlen(s2)+1)) = '\0';
  res = strcat(strcat(res, s1), s2);
  return res;
}




/*****************************************************************************************
 Function:                     STR_Remove
 Purpose: Removes 2nd. string argumnent of first. Returns new string.
 Returns: &char
*****************************************************************************************/
char *STR_Remove(s1, s2)
     char *s1, *s2;
{
  register char *res, *p, *b, *e, *c;
  res = M_alloc((unsigned) strlen(s1)+1);

  for(b=s1, p=res, c=s2; *b;) {
    for(; *b && *b != *c; *p++ = *b++)  /* copy s1 into res until first letter of s2 is */
      ;                                 /* found in s1*/ 
    for(e=b; *e && *c && *e ++ == *c ++;)  /* look ahead in s2 and s1 for the first */
      ; 				   /* difference is found */
    if(*e && ! *c)                         /* s2 found in s1 */
      for(c=s2; b<e && *++b != *c;)       /* skip piece of s1 corresponding to s2 */
	;
    else if(*e)                            /* not found */
      for(c=s2; b<e; *p++ = *b++)          /* copy look ahead into res (common to s1 and */
	;				   /* s2, but != s2) */
    else
      b=c;                                 /* coincident ends of string */
  }
  *p = '\0';                               /* append end of word symbol */
  if(strlen(res) == strlen(s1))
    ER_StackError("remove: string %s not found in %s\n", Warning, STR_Save(s1),
		  STR_Save(s2), nst);
  return res;
}


/*****************************************************************************************
 Function:                     STR_TypeString
 Purpose: Builds a string to describe a token or variable value type.
 Returns: string.
*****************************************************************************************/
char *STR_TypeString(Token)
     SymbolType Token;
{
  static char *TokenStr[] = {
#include "co_tokPrint.h"
    , 0};
  if(Token > 256 && Token < 256+DIM(TokenStr))
	return STR_Save(TokenStr[Token-257]);
  else
    switch(Token) {
    case Undef:
      return STR_Save("'undefined'");
    case UNIT:
      return STR_Save("'unit'");
    case Conn:
      return STR_Save("'connection'");
    case ArrInt:
      return STR_Save("'array of integers'");
    case ArrFloat:
      return STR_Save("'array of floats'");
    case ArrString:
      return STR_Save("'array of strings'");
    case ArrStruct:
      return STR_Save("'array of structures'");
    default:
      return STR_Save("' ?? '");
    }
}
      



/*****************************************************************************************
 Function:                     STR_RemDelimiters
 Purpose:  Removes string delimiters. 
 Returns:  Pointer to 2nd. character of the string (first is assumed to be ").
 Side effects: Changes last " of string to \0.
*****************************************************************************************/
char *STR_RemDelimiters(s)
     register char *s;    /* string to be modified */
{
  register char *r;       /* points to modified string */
  /* remove first " and look for last one */
  for(r = ++s; *s && *s != '\"'; s++)
    ;
  if(*s)
    *s = '\0';      /* delimiter found and removed */
  else
    (void) printf("****** bug: illegal string %s in STR_RemDelimiters\n", r-1);
  return r;
}





/*****************************************************************************************
 Function:                     STR_OrderedString
 Purpose: Checks if a string consists only of characters and digits.
 Returns: TRUE if so, else FALSE.
*****************************************************************************************/
short STR_OrderedString(s)
     char *s;
{
  for(; *s; s++)
    if( ! isalpha(*s) && ! isdigit(*s))
      return FALSE;
  return TRUE;
}




/*****************************************************************************************
 Function:                     STR_New
 Purpose: Save string. Similar to STR_Save, but pairs "\x" in strings are transformed into
          '\x' symbols.
 Returns: Pointer to copy of string.
*****************************************************************************************/
char *STR_New(s)
     char *s;                                                             /* old string */
{
  char *p;                                                                /* new string */
  register char *u, *v;                         /* copy symbols of old into  new string */
  unsigned l;                                                     /* size of new string */
  char  extra[2];                       /* to print unpreviewed '\x' symbols (warning)  */
  extra[1] = '\0';                        /*  terminate string to print unknown symbols */
  if ((p = M_alloc((unsigned)(l = strlen(s)+1))) != NULL) {
    /* make explicit copy of string, transform any "\x" into '\x' */
    for(u = p, v = s; *v; u++, v++)
      if(*v == '\\') {
	switch(*(v+1)) {
	case 't': 
	case 'T':
	  *u = '\t';
	  break;
	case 'n':
	case 'N': 
	  *u = '\n';
	  break;
	default:
	  ER_StackError("unpreviewed '\\%s' in string transformed into '_'\n", Warning,
			extra, nst, nst);
	  *u = '_';
	  break;
	}                          /* v now points to first symbol after "\x"  sequence */
	l--;           /* decrease length of  string, since "\x"  have been two symbols */
	v++;
      }
      else
	*u = *v;                                             /* normal symbol is copied */
    *u = '\0';                                                  /* terminate new string */
    p = realloc(p, l);
  }
  return(p);                         /* returns new string or NULL if malloc has failed */
}





/*****************************************************************************************
 Function:                     STR_Ord
 Purpose: Returns the order no. of a character.
 Returns: int
*****************************************************************************************/
int STR_Ord(c)
     char c;
{
  register int i;
  for(i=0; i<NoChars && *(StringOrder[i]) != c; i++)
    ;
  return i;
}





/*****************************************************************************************
 Function:                     STR_Chr
 Purpose: Returns the character corresponding to a given order no.
 Returns: char
*****************************************************************************************/
char STR_Chr(i)
     int i;
{
  if(i < NoChars && i  >= 0)
    return *(StringOrder[i]);
  else
    return '\0';
}
