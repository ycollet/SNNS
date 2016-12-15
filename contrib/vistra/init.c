#include <dirent.h>

static void doBatch();                                 /* error gesetzt */
static void getFormatsInDir(char *, Collection, Collection);
                                                       /* error gesetzt */
static void addFormat(char *, char *, Collection, Collection);
                                                       /* error gesetzt */

#define XtNnumberFont                "numberFont"
#define XtNoutFile                   "outFile"
#define XtNlogFile                   "logFile"
#define XtNreadFmt                   "readFmt"
#define XtNwriteFmt                  "writeFmt"
#define XtCNumberFont                "NumberFont"
#define XtCOutFile                   "OutFile"
#define XtCLogFile                   "LogFile"
#define XtCReadFmt                   "ReadFmt"
#define XtCWriteFmt                  "WriteFmt"
#define XtNlog                       "log"
#define XtCLog                       "Log"

typedef struct {
  XFontStruct *numberFont;
  String outFile;
  String logFile;
  String readFmt;
  String writeFmt;
  Boolean log;
} AppData, *AppDataPtr;

static AppData app_data;

static XtResource resources[] = {
  { XtNnumberFont,
    XtCNumberFont,
    XtRFontStruct,
    sizeof(XFontStruct *),
    XtOffset(AppDataPtr, numberFont),
    XtRString,
    "7x13",
  },
  { XtNoutFile,
    XtCOutFile,
    XtRString,
    sizeof(String),
    XtOffset(AppDataPtr, outFile),
    XtRImmediate,
    NULL,
  },
  { XtNlogFile,
    XtCLogFile,
    XtRString,
    sizeof(String),
    XtOffset(AppDataPtr, logFile),
    XtRImmediate,
    NULL,
  },
  { XtNreadFmt,
    XtCReadFmt,
    XtRString,
    sizeof(String),
    XtOffset(AppDataPtr, readFmt),
    XtRImmediate,
    NULL,
  },
  { XtNwriteFmt,
    XtCWriteFmt,
    XtRString,
    sizeof(String),
    XtOffset(AppDataPtr, writeFmt),
    XtRImmediate,
    NULL,
  },
  { XtNlog,
    XtCLog,
    XtRBoolean,
    sizeof(Boolean),
    XtOffset(AppDataPtr, log),
    XtRImmediate,
    (XtPointer) FALSE,
  },
};

static XrmOptionDescRec options[] = {
  { "-nf",    "*numberFont", XrmoptionSepArg, NULL },
  { "-out",   "*outFile",    XrmoptionSepArg, NULL },
  { "-batch", "*logFile",    XrmoptionSepArg, NULL },
  { "-rfmt",  "*readFmt",    XrmoptionSepArg, NULL },
  { "-wfmt",  "*writeFmt",   XrmoptionSepArg, NULL },
  { "-log",   "*log",        XrmoptionNoArg,  (XPointer) "TRUE" },
};


/****************************************************/
/* Initialisiere das Programm.                      */
/* Setze Variable error.                            */
/****************************************************/
static void initialize(int argc, char **argv)
{ Format readFmt, writeFmt;
  char *suffix, *format_dir;
  char decTest[] = "N01";

  app_data.numberFont = NULL;
  topLevel = XtVaAppInitialize(
     &app_context,
     "vistra",                     /* Application's class name */
     options, XtNumber(options),   /* Command line option list */
     &argc, argv,                  /* Command line args */
     NULL,                         /* for missing app-defaults file */
     NULL);                        /* Terminate varargs list */

  XtVaGetApplicationResources(topLevel,
     &app_data, resources, XtNumber(resources),
     NULL);

  if(argc > 2) error(30);
  if(argc == 2) fileName = argv[1];
  else fileName = NULL;

  isDEC = *((int *) decTest) == 0x0031304e;

  /* Initialisiere formats und formatNames. */
  if(! (formatNames = newColl())) error(1);
  if(! (formats = newColl())) error(1);

  /* Fuege 3 Menupunkte hinzu, die nicht mit einem Format auf Platte korr. */
  if(! add(formatNames, SEE_SUFFIX_MENU_LABEL)) error(1);
  if(! add(formatNames, N01_MENU_LABEL)) error(1);
  if(! add(formatNames, LVQ_MENU_LABEL)) error(1);

  /* Lese alle durch ASCII-Files definierten Formate */
  if(! (format_dir = getenv(FORMAT_DIR_ENVVAR)))
    printf("Vistra warning: environment variable %s not defined!\n",
                       FORMAT_DIR_ENVVAR); 
  else 
  { getFormatsInDir(format_dir, formatNames, formats);
    if(error) return;
  }

  if(app_data.logFile)
  { /* BATCH MODUS */
    doBatch();
    if(error) handleErr(error);
    else exit(0);
  }

  /* INTERAKTIVER MODUS */
  if(fileName)
  { FILE *pf;
    char *format_name;

    /* bestimme Namen des Lese-Formats */
    if(app_data.readFmt) format_name = app_data.readFmt;
    else
    { /* see suffix */
      suffix = strrchr(fileName, '.');
      if(! suffix)
      { sprintf(errorInfo, "Cannot derive format from suffix of '%s'!", fileName);
        error(21);
      }
      format_name = ++suffix;
    }
    readFmt = nameToFormat(format_name);
    if(error) return;
    selectedFormat = (app_data.readFmt ? 
                      detectPos(formatNames, format_name, streq) : 1);

    /* oeffne das Pattern File */
    if(! (pf = fopen(fileName, "r")))
    { sprintf(errorInfo, "Pattern file:  %s", fileName);
      error(11);
    }

    /* lese Pattern File ein */
    printf("Reading pattern file %s...\n", fileName);
    if(streq(format_name, N01_MENU_LABEL)) pats = readN01(pf);
    else if(streq(format_name, LVQ_MENU_LABEL)) pats = readLVQ(pf);
    else
    { pats = newPatterns();
      if(error) return;
      fileIn(pats, readFmt, pf);
    }
    fclose(pf);
    if(error) return;
  }         /* if(fileName) */
  else
  { pats = NULL;
    selectedFormat = 1;
  }

  /* Initialisiere lf */
  if(app_data.log && fileName)
  { char *buf;
    if(! (buf = (char *) malloc(strlen(fileName) + 6))) error(1);
    sprintf(buf, "%s.log", fileName);
    if(! (lf = fopen(buf, "w")))
    { printf("Vistra warning: cannot open log file %s\n", buf);
      app_data.log = FALSE;
    }
    free(buf);
  }
  else { app_data.log = FALSE; lf = NULL; }

  /* Initialisiere weitere Variablen */
  swapSSWOpened = FALSE;
  lastNr = 1;
  vert = horiz = NULL;
  logFirstTime = TRUE;
  if(! (openGWs = newColl())) error(1);

  error = 0;
}         /* initialize */


/*************************************************************/
/* Initialisiere alles, wofuer ein Widget des Programms      */
/* notwendig ist.                                            */
/*************************************************************/
static void initialize2(Widget w)
{
  display = XtDisplay(w);
  screen = DefaultScreen(display);
  depth = DefaultDepth(display, screen);
  gwFont = XLoadQueryFont(display, "7x13");
  initDrawer(w);
}        /* initialize2 */


/*************************************************************/
/* Variable error wird gesetzt.                              */
/*************************************************************/
static void doBatch()
{ FILE *outFile, *inFile, *logFile;
  char *suffix, *log, *readFmt_name, *writeFmt_name;
  Format readFmt, writeFmt;
  Patterns p;
  Collection opStrings;
  long len;

  if(! fileName) error(31);
  if(! (logFile = fopen(app_data.logFile, "r")))
  { sprintf(errorInfo, "Log-file:  %s", app_data.logFile);
    error(32);
  }

  /* bestimme den Namen des Lese-Formats */
  if(app_data.readFmt) readFmt_name = app_data.readFmt;
  else
  { /* see suffix */
    suffix = strrchr(fileName, '.');
    if(! suffix)
    { sprintf(errorInfo, "Cannot derive format from suffix of %s!", fileName);
      error(21);
    }
    readFmt_name = ++suffix;
  }
  readFmt = nameToFormat(readFmt_name);
  if(error) return;

  /* bestimme den Namen des Ausgabe-Formats */
  if(app_data.writeFmt) writeFmt_name = app_data.writeFmt;
  else
  { /* benutze Suffix des Ausgabefiles oder readFmt */
    Boolean suffixIsFormat = FALSE;

    if(app_data.outFile)
    { suffix = strrchr(app_data.outFile, '.');
      if(suffix)
      { nameToFormat(++suffix);
        if(! error) suffixIsFormat = TRUE;
      }
    }
    if(suffixIsFormat) writeFmt_name = suffix;
    else writeFmt_name = readFmt_name;
  }
  writeFmt = nameToFormat(writeFmt_name);
  if(error) return;

  /* oeffne Input-File */
  if(! (inFile = fopen(fileName, "r")))
  { sprintf(errorInfo, "Pattern file:  %s", fileName);
    error(11);
  }

  /* oeffne Output-File */
  if(! app_data.outFile)
  { if(! (app_data.outFile = (char *) malloc(strlen(fileName) + 6))) error(1);
    sprintf(app_data.outFile, "%s.out", fileName);
  }
  if(! (outFile = fopen(app_data.outFile, "w")))
  { sprintf(errorInfo, "Output file:  %s", app_data.outFile);
    error(12);
  }

  /* lese Patterns ein */
  if(streq(readFmt_name, N01_MENU_LABEL)) p = readN01(inFile);
  else if(streq(readFmt_name, LVQ_MENU_LABEL)) p = readLVQ(inFile);
  else
  { p = newPatterns();
    if(error) return;
    fileIn(p, readFmt, inFile);
  }
  fclose(inFile);
  if(error) return;

  /* transformiere Patterns gemaess logFile */
  len = flen(logFile);
  if(error) return;
  if(! (log = (char *) malloc((unsigned) len + 5))) error(1);
  diskToStr(logFile, log);
  if(! (opStrings = tokens(log, "\n"))) error(1);
  interpret(opStrings, p);
  if(error) return;
  free(log);
  freeColl(opStrings);

  /* schreibe Patterns nach outFile */
  if(streq(writeFmt_name, N01_MENU_LABEL)) writeN01(p, outFile);
  else if(streq(writeFmt_name, LVQ_MENU_LABEL)) writeLVQ(p, outFile);
  else fileOut(p, writeFmt, outFile);
  if(error) return;

  error = 0;
}        /* doBatch */


/*********************************************************/
/* Fuege der Collection fmts alle Formate hinzu, deren   */
/* ASCII-Definitions-File sich im Directory dirname be-  */
/* finden und eine korrekte Syntax besitzen.             */
/* Die Namen der korrekten Format-Dateien werden (ohne   */
/* Suffix) der Collection names angehaengt.              */
/* Variable error wird gesetzt.                          */
/*********************************************************/
static void getFormatsInDir(dirname, names, fmts)
char *dirname;
Collection names, fmts;
{ DIR *dirp;
  struct dirent *dp;
  char *ext, *fmtName;
  unsigned len, lenExt;

  if(! (dirp = opendir(dirname)))
  { printf("Vistra warning: cannot open/find format directory %s!\n", dirname);
    return;
  }

  lenExt = strlen(FORMAT_EXTENSION);
  while(dp = readdir(dirp))
  {
    len = strlen(dp->d_name);
    if(len >= lenExt)
    { 
      ext = dp->d_name + len - lenExt;
      if(strcmp(ext, FORMAT_EXTENSION) == 0)
      {          /* Extension stimmt */
        addFormat(dirname, dp->d_name, fmts, names);
        if(error)
        { closedir(dirp);
          return;
        }
      }  /* if */
    }    /* if */
  }      /* while */

  closedir(dirp);
  error = 0;
}        /* getFormatsInDir */


/*****************************************************/
/* Ueberpruefe, ob es sich bei der Datei namens fname*/
/* im Directory dirname um eine Format-Datei mit     */
/* korrekter Syntax handelt.                         */
/* Falls ja, so fuege das Format bzw. den Format-    */
/* Namen den Collections fmtColl bzw. nameColl hinzu.*/
/* Variable error wird gesetzt.                      */
/*****************************************************/
static void addFormat(char *dirname, char *fname, Collection fmtColl, 
                      Collection nameColl)
{ FILE *f;
  Format fmt;
  char *fullyQualified, *fmtName;
  Boolean noSlash;
  unsigned dirnameLen, len, lenWithoutExt;

  /* berechne den vollstaendigen Pfadnamen in fullyQualified */
  dirnameLen = strlen(dirname);
  len = strlen(fname); 
  noSlash = (dirname[dirnameLen-1] != '/');
  fullyQualified = (char *) malloc(dirnameLen + len + 2);
  if(! fullyQualified) error(1);
  if(noSlash) sprintf(fullyQualified, "%s/%s", dirname, fname);
  else sprintf(fullyQualified, "%s%s", dirname, fname);

  /* ueberpruefe die Syntax des Formats */
  if(f = fopen(fullyQualified, "r"))
  {
    fmt = newFormat(f);
    if(error) return;
    checkSyntax(fmt);
    if(error) 
    { printf("Vistra warning: syntax error in format %s!\n", fullyQualified);
      printf("%s\n", errorInfo);
      freeFormat(fmt);
      error(0);  
    }
  }
  else 
  { printf("Vistra warning: cannot open format %s!\n", fname);
    error(0);
  }

  /* fuege Format und Namen den Collections fmtColl bzw. nameColl hinzu. */
  if(! add(fmtColl, fmt)) error(1);
  lenWithoutExt = len - strlen(FORMAT_EXTENSION);
  if(! (fmtName = (char *) malloc(lenWithoutExt + 1))) error(1);
  strncpy(fmtName, fname, lenWithoutExt);
  fmtName[lenWithoutExt] = '\0';
  if(! add(nameColl, fmtName)) error(1);

  error = 0;
}        /* addFormat */

