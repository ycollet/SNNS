#include "xvis.h"
#include <ctype.h>

#define check(tk) { if((tk) == unknownControl)    \
		    { error = 4;                  \
		      sprintf(errorInfo, "Unknown control: %%%c!", tokenval[0]);  \
		      return;                     \
		    }                             \
		  }

static enum Token tok;
static Format format;

static void definition();                        /* error gesetzt */
static void stmtList();                          /* error gesetzt */
static void stmt();                              /* error gesetzt */
static void altList();                           /* error gesetzt */
static void descList();                          /* error gesetzt */
static void descListCdr();                       /* error gesetzt */
static void desc();                              /* error gesetzt */
static Boolean isStmtBegin(enum Token);


/**************************************************/
/* Antworte ein Format vom Format-File ff.        */
/* Setze Variable error.                          */
/**************************************************/
Format newFormat(FILE *ff)
{ long fileLength;
  Format f;
  char *str;

  if(! (f = (Format) malloc(sizeof(*f)))) error(1);

  fileLength = flen(ff);
  if(error) return NULL;
  if(fileLength > MAX_VAL(unsigned)) error(3);   /* format file too long */

  str = (char *) malloc((unsigned) fileLength);
  if(str == NULL) error(1);                 /* not enough memory */

  /* copy the contents of the format file to String str */
  diskToStr(ff, str);

  f->contents = f->pos = str;
  error = 0;
  return f;
}          /* newFormat */


/**************************************************/
/* Gebe den Speicher des Formats f wieder frei.   */
/**************************************************/
void freeFormat(Format f)
{
  free(f->contents);
  free(f);
}            /* freeFormat */


/**************************************************/
/* Gebe die aktuelle Position der Eingabe zurueck.*/
/**************************************************/
long getPosition(Format f)
{
  return f->pos - f->contents;
}            /* getPosition */


/**************************************************/
/* Setze den Format-Positionszeiger von f         */
/* auf pos.                                       */
/**************************************************/
void setPosition(Format f, long pos)
{
  f->pos = f->contents + pos;
}         /* setPosition */


/**************************************************/
/* Antworte das naechste Token vom Format f.      */
/* Bewege den Format Pointer jedoch nicht.        */
/**************************************************/
enum Token lookUp(Format f)
{ long savePos;
  enum Token answer;

  savePos = getPosition(f);
  answer = nextToken(f);
  setPosition(f, savePos);
  return answer;
}            /* lookUp */


/************************************************************/
/* Antworte den ersten Deskriptor, der in f ab              */
/* Position pos kommt. Wenn kein Deskriptor mehr            */
/* danach kommt, so antworte endOfString.                   */
/************************************************************/
enum Token lookUpNextDesc(Format f, long pos)
{ long savePos;
  enum Token answer;

  savePos = getPosition(f);
  setPosition(f, pos);
  answer = nextToken(f);
  while(! isDesc(answer) && answer != endOfString) answer = nextToken(f);
  setPosition(f, savePos);
  return answer;
}          /* lookUpNextDesc */


/**************************************************/
/* enum Token nextToken(Format f)                 */
/*------------------------------------------------*/
/* Lese das naechste Token vom Format f.          */
/* Bewege dabei den Format-Zeiger hinter dieses   */
/* Token.                                         */
/* Folgende Tokens werden erkannt:                */
/*                                                */
/* Token          return                          */
/* -----------------------------------------------*/
/* "%E"           inputDim                        */
/* "%A"           outputDim                       */
/* "%N"           numPatterns                     */
/* "%I"           input                           */
/* "%O"           output                          */
/* "%C"           class                           */
/* "*"            asterisk                        */
/* "?"            questionMark                    */
/* "<"            newLine                         */
/* "\"            writeNewLine                    */
/* "["            altBegin                        */
/* "]"            altEnd                          */
/* "{"            loopBegin                       */
/* "}"            loopEnd                         */
/* "|"            or                              */
/* "<beliebig>"   aString                         */
/* "\0"           endOfString                     */
/* z.B. "%G"      unknownControl                  */
/*                                                */
/* Im Falle von aString wird die Variable         */
/* tokenval auf <beliebig> gesetzt.               */
/* Folgende Zeichenkombinationen werden durch     */
/* durch Characters ersetzt:                      */
/*                                                */
/*   "%%" -> '%'                                  */
/*   "%*" -> '*'                                  */
/*   "%<" -> '<'                                  */
/*   "%\" -> '\'                                  */
/*   "%?" -> '?'                                  */
/*   "%[" -> '['                                  */
/*   "%]" -> ']'                                  */
/*   "%{" -> '{'                                  */
/*   "%}" -> '}'                                  */
/*   "%|" -> '|'                                  */
/*                                                */
/**************************************************/
enum Token nextToken(Format f)
{
  char *p2;                  /* String pointer fuer tokenval */
  int index;
  Boolean whitespace;        /* TRUE gdw. bisher nur white space gelesen */
  enum Token found;
  char *cp1, *cp2;
  unsigned strCount;         /* merke Anzahl gelesener Zeichen fuer aString */
                             /* damit die Laenge STRINGLENGTH von tokenval  */
                             /* nicht ueberschritten wird.                  */

  strcpy(tokenval, "");
  p2 = tokenval;

  strCount = 0;
  whitespace = TRUE;
  found = nil;
  while(found == nil)
  {
    cp1 = f->pos++;
    switch(*cp1)
    {
      case '%':
		cp2 = f->pos++;
		switch(toupper(*cp2))
                {
                  case '{':
                  case '|':
                  case '}':
                  case '[':
                  case ']':
                  case '*':
                  case '?':
                  case '<':
		  case '\\':
		  case '%': *p2++ = *cp2;
                            whitespace = FALSE;
                            if(++strCount == MAX_LENGTH_TOKENVAL - 1)
                              found = aString;
                            break;
                  case 'I': found = input;
                            break;
                  case 'O': found = output;
                            break;
                  case 'C': found = class;
                            break;
                  case 'E': found = inputDim;
                            break;
                  case 'A': found = outputDim;
                            break;
                  case 'N': found = numPatterns;
                            break;
		  default:  tokenval[0] = *cp2;
			    return unknownControl;
                }    /* switch */
                break;
      case '[': found = altBegin;
                break;
      case '|': found = or;
                break;
      case ']': found = altEnd;
                break;
      case '{': found = loopBegin;
                break;
      case '}': found = loopEnd;
                break;
      case '?': found = questionMark;
                break;
      case '*': found = asterisk;
                break;
      case '<': found = newLine;
		break;
      case '\\': found = writeNewLine;
		break;
      case '\0': found = endOfString;
                break;
      default:  if(! whitespace || ! isspace(*cp1))
                { whitespace = FALSE;
		  *p2++ = *cp1;
		  if(++strCount == MAX_LENGTH_TOKENVAL - 1) found = aString;
                }        /* if */
                break;
    }      /* switch */
  }        /* while(found == nil) */
  if(whitespace || found == aString) return found;
  else
  { *p2 = '\0';
    f->pos = cp1;
    return aString;
  }
}      /* nextToken */


/*****************************************/
/* Antworte die Position nach dem ersten */
/* Auftreten des Tokens tk im Format f   */
/* ab der aktuellen Position.            */
/* Falls tk nicht mehr vorkommt, dann    */
/* wird -1L zurueckgegeben.              */
/* Setze Variable error.                 */
/*****************************************/
long posAfter(Format f, enum Token searched)
{ enum Token reached;
  long savePos, answer;

  savePos = getPosition(f);
  reached = nextToken(f);
  while(reached != endOfString && reached != searched) reached = nextToken(f);

  answer = reached == endOfString ? -1L : getPosition(f);
  setPosition(f, savePos);
  return answer;
}                   /* posAfter */


/****************************************************/
/* Ueberpruefe die Syntax von Format f.             */
/* Setze error auf eine Zahl ungleich 0 bei einem   */
/* Syntax-Fehler, sonst setze error auf 0.          */
/****************************************************/
void checkSyntax(Format f)
{ long oldPos;

  format = f;
  oldPos = getPosition(format);
  setPosition(format, 0L);

  tok = nextToken(format);
  check(tok);
  definition();
  setPosition(format, oldPos);
}       /* checkSyntax */


/***********************************************************/
/***********************************************************/
static void definition()
{
  stmtList();
  if(error) return;
  if(tok != endOfString)
  { error = 4;
    strcpy(errorInfo, "End of the file expected!");
    return;
  }

  error = 0;
}             /* definition */


/***********************************************************/
/***********************************************************/
static void stmtList()
{
  do
  {
    stmt();
    if(error) return;
  } while(isStmtBegin(tok));

  error = 0;
}             /* stmtList */


/***********************************************************/
/***********************************************************/
static void stmt()
{
  switch(tok)
  {
    case altBegin:
                  tok = nextToken(format);
                  check(tok);
                  altList();
                  if(error) return;
                  if(tok != altEnd)
                  { error = 4;
                    strcpy(errorInfo, "Missing ']'!");
                    return;
                  }
                  tok = nextToken(format);
                  check(tok);
                  break;
    case loopBegin:
                  tok = nextToken(format);
                  check(tok);
                  descList();
                  if(error) return;
                  if(tok != loopEnd)
                  { error = 4;
                    strcpy(errorInfo, "Missing '}'!");
                    return;
                  }
                  tok = nextToken(format);
                  check(tok);
                  break;
    default:      descList();
		  if(error) return;
  }           /* switch */

  error = 0;
}             /* stmt */


/***********************************************************/
/***********************************************************/
static void altList()
{ Boolean ende = FALSE;

  do
  {
    descList();
    if(error) return;
    if(tok == or)
    { tok = nextToken(format);
      check(tok);
    }
    else ende = TRUE;
  } while(! ende);

  error = 0;
}             /* altList */


/***********************************************************/
/***********************************************************/
static void descList()
{
  desc();
  if(error) return;
  descListCdr();
  if(error) return;

  error = 0;
}             /* descList() */


/***********************************************************/
/***********************************************************/
static void descListCdr()
{
  while(isDesc(tok))
  {
    descList();
    if(error) return;
  }

  error = 0;
}             /* descListCdr */


/***********************************************************/
/***********************************************************/
static void desc()
{
  if(! isDesc(tok))
  { error = 4;
    strcpy(errorInfo, "Descriptor expected!");
    return;
  }
  tok = nextToken(format);
  check(tok);

  error = 0;
}             /* desc */


/***********************************************************/
/***********************************************************/
Boolean isDesc(enum Token tk)
{ static tokenClass[] = {
    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
  };

  return tokenClass[tk];
}


/***********************************************************/
/***********************************************************/
static Boolean isStmtBegin(enum Token tk)
{
  return isDesc(tk) || tk==loopBegin || tk==altBegin;
}




