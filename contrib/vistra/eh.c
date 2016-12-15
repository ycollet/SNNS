#include "xvis.h"

static char *errorMessages[] =
  { "DUMMY",                   /* error numbers start at 1 */
    "Not enough memory!",                           /*  1 */
    "Error reading format file!",                   /*  2 */
    "Format file too long!",                        /*  3 */
    "Syntax error!",                                /*  4 */
    "Pattern file not of correct format!",          /*  5 */
    "Error reading pattern file!",                  /*  6 */
    "Error writing pattern file!",                  /*  7 */
    "Cannot write all input patterns!",             /*  8 */
    "Cannot write all output patterns!",            /*  9 */
    "Cannot write all class names!",                /* 10 */
    "Cannot open pattern file!",                    /* 11 */
    "Cannot open output file!",                     /* 12 */
    "Cannot open format file!",                     /* 13 */
    "Cannot open symtab file!",                     /* 14 */
    "Error writing symtab file!",                   /* 15 */
    "Error reading N01 file!",                      /* 16 */
    "Error writing N01 file!",                      /* 17 */
    "N01 file of wrong format!",                    /* 18 */
    "Cannot open or read directory!",               /* 19 */
    "Programming error!",                           /* 20 */
    "Pattern file name has no suffix!",             /* 21 */
    "Suffix not a name of a valid format!",         /* 22 */
    "A scalar field does not contain a number!",    /* 23 */
    "Cannot divide by 0!",                          /* 24 */
    "All dimensions are constant. Cannot remove all!",  /* 25 */
    "No standard color map available!",             /* 26 */
    "Not enough memory to create pixmap!",          /* 27 */
    "Cannot remove all vectors!",                   /* 28 */
    "Cannot remove all dimensions!",                /* 29 */
    "Unknown option(s) specified!",                 /* 30 */
    "No pattern file specified!",                   /* 31 */
    "Cannot open log file!",                        /* 32 */
    "Symtab file contains not enough symbols!",     /* 33 */
    "Cannot open temporary LVQ file as pipe to program pca!",  /* 34 */
    "Error writing temporary LVQ file!",            /* 35 */
    "Cannot open pipe from pca to vistra!",         /* 36 */
    "Program pca produced no correct LVQ format!",  /* 37 */
    "Cannot open output file of program pca!",      /* 38 */
    "Cannot get a temporary file name!",            /* 39 */
  };


/*****************************************************************/
/* Schreibe die zum Fehler err gehoerende Fehlermeldung nach     */
/* stdout und beende das Programm.                               */
/* Diese Funktion muss anstatt xhandleErr() benutzt werden, wenn */
/* die Widgets des Programms nicht kreiert sind, z.B. in der     */
/* Initialisierungsphase oder bei Batch-Aufrufen.                */
/*****************************************************************/
void handleErr(int err)
{ char buf[MAX_LENGTH_ERROR_MESSAGE];

  genErrorMessage(err, buf);
  printf("\n%s", buf);
  exit(err);
}              /* handleErr */


/******************************************************************/
/* Erzeuge die Fehlermeldung zum Fehlercode err und schreibe diese*/
/* in den Puffer, auf den buf zeigt.                              */
/* Die Funktion liest die globalen Variablen errorInfo und        */
/* rowCount.                                                      */
/******************************************************************/
void genErrorMessage(int err, char *buf)
{
  switch(err)
  {
    case 8:
    case 9:
    case 10:
             sprintf(buf, "Vistra warning: %s\n", errorMessages[err]);
             break;
    case 4:
    case 11:
    case 12:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 32:
    case 34:
             sprintf(buf, "Vistra error %d:  %s\n%s\n",
                          err, errorMessages[err], errorInfo);
             break;
    case 5:  sprintf(buf, "Vistra error %d:  %s\nLine %ld:  %s\n",
                          err, errorMessages[err], rowCount, errorInfo);
             break;
    default: sprintf(buf, "Vistra error %d:  %s\n", err, errorMessages[err]);
             break;
  }      /* switch */
}        /* genErrorMessage */
