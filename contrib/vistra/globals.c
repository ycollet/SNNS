#include "xvis.h"

int error;                     /* Error iff error <> 0            */
char errorInfo[MAX_LENGTH_ERRORINFO];
/* Additional information about   */
/* the error                       */
long rowCount;                 /* If the pattern file being read  */
/* has an incorrect format,        */
/* rowCount indicates the number   */
/* of the line in which the error  */
/* occurred.                       */
char tokenval[MAX_LENGTH_TOKENVAL];
/* Additional information about   */
/* the token                       */
Boolean isDEC;                 /* TRUE iff the program is         */
/* compiled on DEC.                */
