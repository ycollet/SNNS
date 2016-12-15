#include "xvis.h"
#include <math.h>

/*****************************************/
/* Antworte TRUE gdw. u = 2^n fuer eine  */
/* ganze Zahl n >= 0.                    */
/*****************************************/
Boolean isPowerOf2(unsigned u)
{ 
  if(u == 1) return TRUE;
  do 
  { if(u % 2) return FALSE;
    u /= 2;
  } while(u > 1);
  return TRUE;   
}       /* isPowerOf2 */


/*****************************************/
/* Antworte TRUE gdw. u eine Quadratzahl */
/* ist.                                  */
/*****************************************/
Boolean isSquare(unsigned u)
{ unsigned sqroot;

  sqroot = (unsigned) floor(sqrt((double) u));
  return (sqroot * sqroot) == u;
}        /* isSquare */


/*****************************************/
/* Antworte eine Collection von Strings. */
/* Jedes Element stellt eine Token von   */
/* str1 dar.                             */
/* Die Token sind dabei durch die Zeichen*/
/* von str2 begrenzt.                    */
/* antworte NULL, falls zu wenig Speicher*/
/* verfuegbar.                           */
/*****************************************/
Collection tokens(char *str1, char *str2)
{ Collection answer;
  char *tok;

  if(! (answer = newColl())) return NULL;

  /* Lese die Token der aktuellen Zeile */
  if(tok = strtok(str1, str2))
    do
    { if(! add(answer, tok))
      { freeColl(answer);
	return NULL;
      }
      tok = strtok(NULL, str2);
    } while(tok);

  return answer;
}          /* tokens */


/*************************************************/
/* Copy the contents of file f to String s.      */
/*************************************************/
void diskToStr(FILE *f, char *s)
{ char c;

  c = fgetc(f);
  while(c != EOF)
  { *s++ = c;
    c = fgetc(f);
  }
  *s = '\0';
}           /* diskToStr */


/*******************************************/
/* Antworte die Laenge von File f.         */
/* Setze Variable error.                   */
/*******************************************/
long flen(FILE *f)
{ long answer;

  if(fseek(f, 0L, 2)) error(2);
  answer = ftell(f);
  if(answer == -1L) error(2);
  rewind(f);

  error = 0;
  return answer;
}           /* flen */


/*****************************************************/
/* Setze die ersten num Zeichen von s auf ch und     */
/* antworte einen Pointer aufs erste Zeichen         */
/* dahinter.                                         */
/*****************************************************/
char *printnchr(char *cp, unsigned num, char ch)
{ unsigned i;

  for(i = 1; i <= num; i++) *cp++ = ch;
  return cp;
}        /* printnchr */


/*****************************************************/
/* Antworte eine Kopie von str. Der Speicher fuer    */
/* diese Kopie wird mittels malloc() angefordert.    */
/* Antworte NULL, bei zuwenig Speicher.              */
/*****************************************************/
char *my_strdup(char *str)
{ char *answer;
 
  if(! (answer = (char *) malloc(strlen(str)+1))) return NULL;
  strcpy(answer, str);
  return answer; 
}        /* my_strdup */


/*****************************************************/
/* Wandle alle Kleinbuchstaben des Strings str in    */
/* Grossbuchstaben um. Antworte str.                 */
/*****************************************************/
char *strupr(char *str)
{ char *cp;

  for(cp = str; *cp; cp++)
    *cp = toupper(*cp);

  return str;
}        /* strupr */


/*********************************************************/
/* Antworte TRUE genau dann, wenn die Strings s1 und s2  */
/* gleich sind.                                          */
/*********************************************************/
Boolean streq(char *s1, char *s2)
{
  return strcmp(s1, s2) == 0;
}         /* streq */


/*************************************************/
/* Ueberspringe alle Whitespace Character        */
/* (Blanks, Newlines, Tabs) im File f bis zum    */
/* naechsten Non-Whitespace Character.           */
/*************************************************/
void skipWhiteSpace(FILE *f)
{ char c;

  while(isspace(c = fgetc(f)));
  if(c != EOF) fseek(f, -1L, 1);
}           /* skipWhiteSpace */


/*************************************************/
/* Ueberspringe alle Whitespace Character        */
/* (Blanks, Newlines, Tabs) im File f bis zum    */
/* naechsten Non-Whitespace Character. Erhoehe   */
/* rowCount dabei um die Anzahl gelesener        */
/* Newlines.                                     */
/*************************************************/
void skipSpaceAndCountNl(FILE *f)
{ char c;

  do
  { if((c = fgetc(f)) == '\n') rowCount++;
  } while(isspace(c));
  if(c != EOF)
    fseek(f, -1L, 1);             /* setze ein Zeichen zurueck */
}          /* skipSpaceAndCountNl */


/**************************************************/
/* Antworte mit TRUE gdw. ab der aktuellen File   */
/* Position nur noch white space kommt.           */
/* Ueberlese den white space.                     */
/**************************************************/
Boolean atEnd(FILE *f)
{ char ch;

  skipWhiteSpace(f);
  ch = fgetc(f);
  fseek(f, -1L, 1); 
  return ch == EOF;
}           /* atEnd */


/**************************************************/
/* Ueberlese alle white space Zeichen bis zum     */
/* naechsten Zeichen, das kein white space ist.   */
/* Erhoehe dabei rowCount um die Anzahl gelesener */
/* Newlines.                                      */
/* Antworte TRUE gdw. das erste Non white space   */
/* Zeichen EOF ist.                               */
/**************************************************/
Boolean atEndNl(FILE *f)
{ char ch;

  skipSpaceAndCountNl(f);
  ch = fgetc(f);
  fseek(f, -1L, 1);
  return ch == EOF;
}           /* atEndNl */


/**************************************************/
/* Antworte einen String, der die Matrix mat dar- */
/* stellt.                                        */
/* Antworte NULL, falls nicht genuegend Speicher  */
/* vorhanden.                                     */
/**************************************************/
char *matAsString(Number **mat, unsigned rows, unsigned cols)
{ unsigned i, j, lineLength;
  char *answer, *cp, buf[NUMBER_STR_LENGTH + 1];

  lineLength = NUM_OF_PATS_STR_MAXLEN + cols * (NUMBER_STR_LENGTH + 2) + 1;
  if(! (answer = (char *) malloc((rows + 2) * lineLength + 1)))
    return NULL;
  /* schreibe die Headline */
  cp = answer;
  cp = printnchr(cp, NUM_OF_PATS_STR_MAXLEN, ' ');
  *cp++ = '|';
  for(i = 1; i <= cols; i++)
  { sprintf(cp, "  %6u   ", i);
    cp += NUMBER_STR_LENGTH + 1;
  }
  *cp++ = '\n';
  cp = printnchr(cp, NUM_OF_PATS_STR_MAXLEN, '-');
  *cp++ = '+';
  cp = printnchr(cp, cols * (NUMBER_STR_LENGTH + 1), '-');
  *cp++ = '\n';
  
  for(i = 0; i < rows; i++)
  { sprintf(cp, NUM_OF_PATS_FORMAT, i+1);
    cp += NUM_OF_PATS_STR_MAXLEN;
    *cp++ = '|';
    for(j = 0; j < cols; j++)
    { sprintf(cp, NUMBER_FORMAT, mat[i][j]);
      cp += NUMBER_STR_LENGTH; 
      *cp++ = ' ';
    }
    *cp++ = '\n';
  }  
  *cp++ = '\0';
 
  return answer;
}          /* matAsString */


/*******************************************************/
/* Fuelle so mit Werten, die eine Abbildung des Werte- */
/* bereichs from in den Wertebereich to erlauben. so   */
/* muss dann beim Umrechnen eines Wertes in den neuen  */
/* Bereich angegeben werden.                           */
/*******************************************************/
void getScaleOp(ScaleOp *so, Range from, Range to)
{ float stretchFactor;

  if(to.highest <= to.lowest)
  { so->mult = 0.0;
    so->add = (float) to.highest;
  }
  else if(from.highest <= from.lowest)
  { so->mult = 0.0;
    so->add = to.lowest;
  }
  else
  { stretchFactor = ((float) (to.highest - to.lowest))
			/ (from.highest - from.lowest);
    so->mult = (float) stretchFactor;
    so->add = (float) (to.lowest - (stretchFactor * from.lowest));
  }
}          /* getScaleOp */



