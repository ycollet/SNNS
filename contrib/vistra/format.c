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

static void definition(void);                        /* sets error */
static void stmtList(void);                          /* sets error */
static void stmt(void);                              /* sets error */
static void altList(void);                           /* sets error */
static void descList(void);                          /* sets error */
static void descListCdr(void);                       /* sets error */
static void desc(void);                              /* sets error */
static Boolean isStmtBegin(enum Token);


/**************************************************/
/* Returns a format from the format file ff.      */
/* Sets the global variable error.                */
/**************************************************/
Format newFormat(FILE *ff) {
    long fileLength;
    Format f;
    char *str;

    if(! (f = (Format) malloc(sizeof(*f)))) errorR(1,NULL);

    fileLength = flen(ff);
    if(error) return NULL;
    if(fileLength > MAX_VAL(unsigned)) errorR(3,NULL);   /* format file too long */

    str = (char *) malloc((unsigned) fileLength);
    if(str == NULL) errorR(1,NULL);                 /* not enough memory */

    /* copy the contents of the format file to String str */
    diskToStr(ff, str);

    f->contents = f->pos = str;
    error = 0;
    return f;
}          /* newFormat */


/**************************************************/
/* Frees the memory of the format f.              */
/**************************************************/
void freeFormat(Format f) {
    free(f->contents);
    free(f);
}            /* freeFormat */


/**************************************************/
/* Returns the current position of the input.     */
/**************************************************/
long getPosition(Format f) {
    return f->pos - f->contents;
}            /* getPosition */


/**************************************************/
/* Sets the format position pointer of f          */
/* to pos.                                        */
/**************************************************/
void setPosition(Format f, long pos) {
    f->pos = f->contents + pos;
}         /* setPosition */


/**************************************************/
/* Returns the next token from format f.          */
/* Does not move the format pointer, however.     */
/**************************************************/
enum Token lookUp(Format f) {
    long savePos;
    enum Token answer;

    savePos = getPosition(f);
    answer = nextToken(f);
    setPosition(f, savePos);
    return answer;
}            /* lookUp */


/************************************************************/
/* Returns the first descriptor that appears in f            */
/* starting at position pos. If no more descriptor           */
/* follows, returns endOfString.                             */
/************************************************************/
enum Token lookUpNextDesc(Format f, long pos) {
    long savePos;
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
/* Reads the next token from format f.            */
/* Moves the format pointer past this token in    */
/* the process.                                   */
/* The following tokens are recognized:           */
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
/* "<anything>"   aString                         */
/* "\0"           endOfString                     */
/* e.g. "%G"      unknownControl                  */
/*                                                */
/* In the case of aString, the variable           */
/* tokenval is set to <anything>.                 */
/* The following character combinations are       */
/* replaced by characters:                        */
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
enum Token nextToken(Format f) {
    char *p2;                  /* string pointer for tokenval */
    Boolean whitespace;        /* TRUE iff only white space has been read so far */
    enum Token found;
    char *cp1, *cp2;
    unsigned strCount;         /* remembers the number of characters read for aString */
    /* so that the length STRINGLENGTH of tokenval */
    /* is not exceeded.                            */

    strcpy(tokenval, "");
    p2 = tokenval;

    strCount = 0;
    whitespace = TRUE;
    found = nil;
    while(found == nil) {
        cp1 = f->pos++;
        switch(*cp1) {
        case '%':
            cp2 = f->pos++;
            switch(toupper(*cp2)) {
            case '{':
            case '|':
            case '}':
            case '[':
            case ']':
            case '*':
            case '?':
            case '<':
            case '\\':
            case '%':
                *p2++ = *cp2;
                whitespace = FALSE;
                if(++strCount == MAX_LENGTH_TOKENVAL - 1)
                    found = aString;
                break;
            case 'I':
                found = input;
                break;
            case 'O':
                found = output;
                break;
            case 'C':
                found = class;
                break;
            case 'E':
                found = inputDim;
                break;
            case 'A':
                found = outputDim;
                break;
            case 'N':
                found = numPatterns;
                break;
            default:
                tokenval[0] = *cp2;
                return unknownControl;
            }    /* switch */
            break;
        case '[':
            found = altBegin;
            break;
        case '|':
            found = or;
            break;
        case ']':
            found = altEnd;
            break;
        case '{':
            found = loopBegin;
            break;
        case '}':
            found = loopEnd;
            break;
        case '?':
            found = questionMark;
            break;
        case '*':
            found = asterisk;
            break;
        case '<':
            found = newLine;
            break;
        case '\\':
            found = writeNewLine;
            break;
        case '\0':
            found = endOfString;
            break;
        default:
            if(! whitespace || ! isspace(*cp1)) {
                whitespace = FALSE;
                *p2++ = *cp1;
                if(++strCount == MAX_LENGTH_TOKENVAL - 1) found = aString;
            }        /* if */
            break;
        }      /* switch */
    }        /* while(found == nil) */
    if(whitespace || found == aString) return found;
    else {
        *p2 = '\0';
        f->pos = cp1;
        return aString;
    }
}      /* nextToken */


/*****************************************/
/* Returns the position after the first  */
/* occurrence of the token tk in format f*/
/* starting from the current position.   */
/* If tk does not occur again, -1L is    */
/* returned.                             */
/* Sets the global variable error.       */
/*****************************************/
long posAfter(Format f, enum Token searched) {
    enum Token reached;
    long savePos, answer;

    savePos = getPosition(f);
    reached = nextToken(f);
    while(reached != endOfString && reached != searched) reached = nextToken(f);

    answer = reached == endOfString ? -1L : getPosition(f);
    setPosition(f, savePos);
    return answer;
}                   /* posAfter */


/****************************************************/
/* Checks the syntax of format f.                   */
/* Sets error to a nonzero number in case of a      */
/* syntax error, otherwise sets error to 0.         */
/****************************************************/
void checkSyntax(Format f) {
    long oldPos;

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
static void definition() {
    stmtList();
    if(error) return;
    if(tok != endOfString) {
        error = 4;
        strcpy(errorInfo, "End of the file expected!");
        return;
    }

    error = 0;
}             /* definition */


/***********************************************************/
/***********************************************************/
static void stmtList() {
    do {
        stmt();
        if(error) return;
    } while(isStmtBegin(tok));

    error = 0;
}             /* stmtList */


/***********************************************************/
/***********************************************************/
static void stmt() {
    switch(tok) {
    case altBegin:
        tok = nextToken(format);
        check(tok);
        altList();
        if(error) return;
        if(tok != altEnd) {
            error = 4;
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
        if(tok != loopEnd) {
            error = 4;
            strcpy(errorInfo, "Missing '}'!");
            return;
        }
        tok = nextToken(format);
        check(tok);
        break;
    default:
        descList();
        if(error) return;
    }           /* switch */

    error = 0;
}             /* stmt */


/***********************************************************/
/***********************************************************/
static void altList() {
    Boolean ende = FALSE;

    do {
        descList();
        if(error) return;
        if(tok == or) {
            tok = nextToken(format);
            check(tok);
        } else ende = TRUE;
    } while(! ende);

    error = 0;
}             /* altList */


/***********************************************************/
/***********************************************************/
static void descList() {
    desc();
    if(error) return;
    descListCdr();
    if(error) return;

    error = 0;
}             /* descList() */


/***********************************************************/
/***********************************************************/
static void descListCdr() {
    while(isDesc(tok)) {
        descList();
        if(error) return;
    }

    error = 0;
}             /* descListCdr */


/***********************************************************/
/***********************************************************/
static void desc() {
    if(! isDesc(tok)) {
        error = 4;
        strcpy(errorInfo, "Descriptor expected!");
        return;
    }
    tok = nextToken(format);
    check(tok);

    error = 0;
}             /* desc */


/***********************************************************/
/***********************************************************/
Boolean isDesc(enum Token tk) {
    static Boolean tokenClass[] = {
        TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
        FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
    };

    return tokenClass[tk];
}


/***********************************************************/
/***********************************************************/
static Boolean isStmtBegin(enum Token tk) {
    return isDesc(tk) || tk==loopBegin || tk==altBegin;
}




