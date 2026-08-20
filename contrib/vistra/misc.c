#include "xvis.h"
#include <math.h>
#include <ctype.h>

/*****************************************/
/* Return TRUE iff u = 2^n for some      */
/* integer n >= 0.                       */
/*****************************************/
Boolean isPowerOf2(unsigned u) {
    if(u == 1) return TRUE;
    do {
        if(u % 2) return FALSE;
        u /= 2;
    } while(u > 1);
    return TRUE;
}       /* isPowerOf2 */


/*****************************************/
/* Return TRUE iff u is a perfect        */
/* square.                               */
/*****************************************/
Boolean isSquare(unsigned u) {
    unsigned sqroot;

    sqroot = (unsigned) floor(sqrt((double) u));
    return (sqroot * sqroot) == u;
}        /* isSquare */


/*****************************************/
/* Return a Collection of strings.       */
/* Each element represents a token of    */
/* str1.                                 */
/* The tokens are delimited by the       */
/* characters in str2.                   */
/* Return NULL if not enough memory      */
/* is available.                         */
/*****************************************/
Collection tokens(char *str1, char *str2) {
    Collection answer;
    char *tok;

    if(! (answer = newColl())) return NULL;

    /* Read the tokens of the current line */
    if(tok = strtok(str1, str2))
        do {
            if(! add(answer, tok)) {
                freeColl(answer);
                return NULL;
            }
            tok = strtok(NULL, str2);
        } while(tok);

    return answer;
}          /* tokens */


/*************************************************/
/* Copy the contents of file f to String s.      */
/*************************************************/
void diskToStr(FILE *f, char *s) {
    char c;

    c = fgetc(f);
    while(c != EOF) {
        *s++ = c;
        c = fgetc(f);
    }
    *s = '\0';
}           /* diskToStr */


/*******************************************/
/* Return the length of file f.            */
/* Sets the global variable error.         */
/*******************************************/
long flen(FILE *f) {
    long answer;

    if(fseek(f, 0L, 2)) errorR(2,0);
    answer = ftell(f);
    if(answer == -1L) errorR(2,0);
    rewind(f);

    error = 0;
    return answer;
}           /* flen */


/*****************************************************/
/* Set the first num characters of s to ch and       */
/* return a pointer to the first character after     */
/* them.                                              */
/*****************************************************/
char *printnchr(char *cp, unsigned num, char ch) {
    unsigned i;

    for(i = 1; i <= num; i++) *cp++ = ch;
    return cp;
}        /* printnchr */


/*****************************************************/
/* Return a copy of str. The memory for this copy    */
/* is requested via malloc().                        */
/* Return NULL if there is not enough memory.        */
/*****************************************************/
char *my_strdup(char *str) {
    char *answer;

    if(! (answer = (char *) malloc(strlen(str)+1))) return NULL;
    strcpy(answer, str);
    return answer;
}        /* my_strdup */


/*****************************************************/
/* Convert all lowercase letters of string str to    */
/* uppercase letters. Return str.                    */
/*****************************************************/
char *strupr(char *str) {
    char *cp;

    for(cp = str; *cp; cp++)
        *cp = toupper(*cp);

    return str;
}        /* strupr */


/*********************************************************/
/* Return TRUE iff the strings s1 and s2 are equal.      */
/*********************************************************/
Boolean streq(char *s1, char *s2) {
    return strcmp(s1, s2) == 0;
}         /* streq */


/*************************************************/
/* Skip all whitespace characters (blanks,       */
/* newlines, tabs) in file f up to the next      */
/* non-whitespace character.                     */
/*************************************************/
void skipWhiteSpace(FILE *f) {
    char c;

    while(isspace(c = fgetc(f)));
    if(c != EOF) fseek(f, -1L, 1);
}           /* skipWhiteSpace */


/*************************************************/
/* Skip all whitespace characters (blanks,       */
/* newlines, tabs) in file f up to the next      */
/* non-whitespace character. Increment rowCount  */
/* by the number of newlines read in the         */
/* process.                                      */
/*************************************************/
void skipSpaceAndCountNl(FILE *f) {
    char c;

    do {
        if((c = fgetc(f)) == '\n') rowCount++;
    } while(isspace(c));
    if(c != EOF)
        fseek(f, -1L, 1);             /* back up one character */
}          /* skipSpaceAndCountNl */


/**************************************************/
/* Return TRUE iff, from the current file         */
/* position onward, only white space follows.     */
/* Skip over the white space.                     */
/**************************************************/
Boolean atEnd(FILE *f) {
    char ch;

    skipWhiteSpace(f);
    ch = fgetc(f);
    fseek(f, -1L, 1);
    return ch == EOF;
}           /* atEnd */


/**************************************************/
/* Skip over all white space characters up to     */
/* the next character that is not white space.    */
/* Increment rowCount by the number of newlines   */
/* read in the process.                           */
/* Return TRUE iff the first non-white-space      */
/* character is EOF.                              */
/**************************************************/
Boolean atEndNl(FILE *f) {
    char ch;

    skipSpaceAndCountNl(f);
    ch = fgetc(f);
    fseek(f, -1L, 1);
    return ch == EOF;
}           /* atEndNl */


/**************************************************/
/* Return a string that represents the matrix     */
/* mat.                                           */
/* Return NULL if not enough memory is            */
/* available.                                     */
/**************************************************/
char *matAsString(Number **mat, unsigned rows, unsigned cols) {
    unsigned i, j, lineLength;
    char *answer, *cp;

    lineLength = NUM_OF_PATS_STR_MAXLEN + cols * (NUMBER_STR_LENGTH + 2) + 1;
    if(! (answer = (char *) malloc((rows + 2) * lineLength + 1)))
        return NULL;
    /* write the headline */
    cp = answer;
    cp = printnchr(cp, NUM_OF_PATS_STR_MAXLEN, ' ');
    *cp++ = '|';
    for(i = 1; i <= cols; i++) {
        sprintf(cp, "  %6u   ", i);
        cp += NUMBER_STR_LENGTH + 1;
    }
    *cp++ = '\n';
    cp = printnchr(cp, NUM_OF_PATS_STR_MAXLEN, '-');
    *cp++ = '+';
    cp = printnchr(cp, cols * (NUMBER_STR_LENGTH + 1), '-');
    *cp++ = '\n';

    for(i = 0; i < rows; i++) {
        sprintf(cp, NUM_OF_PATS_FORMAT, i+1);
        cp += NUM_OF_PATS_STR_MAXLEN;
        *cp++ = '|';
        for(j = 0; j < cols; j++) {
            sprintf(cp, NUMBER_FORMAT, mat[i][j]);
            cp += NUMBER_STR_LENGTH;
            *cp++ = ' ';
        }
        *cp++ = '\n';
    }
    *cp++ = '\0';

    return answer;
}          /* matAsString */


/*******************************************************/
/* Fill so with values that allow a mapping from the   */
/* value range "from" to the value range "to". so must */
/* then be supplied when converting a value into the   */
/* new range.                                          */
/*******************************************************/
void getScaleOp(ScaleOp *so, Range from, Range to) {
    float stretchFactor;

    if(to.highest <= to.lowest) {
        so->mult = 0.0;
        so->add = (float) to.highest;
    } else if(from.highest <= from.lowest) {
        so->mult = 0.0;
        so->add = to.lowest;
    } else {
        stretchFactor = ((float) (to.highest - to.lowest))
                        / (from.highest - from.lowest);
        so->mult = (float) stretchFactor;
        so->add = (float) (to.lowest - (stretchFactor * from.lowest));
    }
}          /* getScaleOp */



