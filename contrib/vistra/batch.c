#include "xvis.h"

#define numCols(p)      (operateOnInputs ? inputDims(p) : outputDims(p))

typedef struct {
  char *name;
  int nparams;
} Command_Struct;

static int execLine();                           /* error gesetzt */
static int commandNr(char *, Command_Struct *);
static void batchWarn(char *);
static void execLoad(int, VecColl, Boolean);

static Command_Struct commands[] = {
  { "SCALE",         4, },      /* 0 */
  { "NORMALIZE",     2, },      /* 1 */
  { "FFT",           2, },      /* 2 */
  { "HLOG",          2, },      /* 3 */
  { "PCA",           2, },      /* 4 */
  { "CLASSEXPAND",   2, },      /* 5 */
  { "OUTPUTEXPAND",  2, },      /* 6 */
  { "RMDIMRANGE",    4, },      /* 7 */
  { "RMCONSTDIMS",   2, },      /* 8 */
  { "RANDOMIZE",     2, },      /* 9 */
  { "LOADVERT",      0, },      /* 10 */
  { "LOADHORIZ",     0, },      /* 11 */
  { "MULTIPLY",      3, },      /* 12 */
  { "DIVIDE",        3, },      /* 13 */
  { "SUBTRACT",      3, },      /* 14 */
  { "ADD",           3, },      /* 15 */
  { "REPLACE",       4, },      /* 16 */
  { "REFRESHCLASSES",2, },      /* 17 */
  { "MULTCONST",     3, },      /* 18 */
  { "DIVCONST",      3, },      /* 19 */
  { "ADDCONST",      3, },      /* 20 */
  { "SUBCONST",      3, },      /* 21 */
  { "RMDIMS",        0, },      /* 22 */
  { NULL, 0 },
};

static Command_Struct loadCommands[] = {
  { "MIN",           3, },      /* 0 */
  { "MAX",           3, },      /* 1 */
  { "AVG",           3, },      /* 2 */
  { "LENGTH",        3, },      /* 3 */
  { "SUM",           3, },      /* 4 */
  { "STDDEV",        3, },      /* 5 */
  { "OVERALLMIN",    3, },      /* 6 */
  { "OVERALLMAX",    3, },      /* 7 */
  { "OVERALLAVG",    3, },      /* 8 */
  { "OVERALLSTDDEV", 3, },      /* 9 */
  { "CONST",         4, },      /* 10 */
  { "PATTERN",       4, },      /* 11 */
  { "VALUES",        0, },      /* 12 */ 
  { "DIM",           5, },      /* 13 */
  { NULL, 0 },
};

static Patterns pats;
static Vector vert, horiz;
static Boolean operateOnInputs;
static char *line;
static long n;
static Collection toks;


/**************************************************/
/* Interpretiere die Batch-Kommandos von coll und */
/* fuehre sie auf den Patterns p durch.           */
/* Jedes Element von coll ist ein String, der eine*/
/* Zeile eines Log-Files enthaelt.                */
/* Variable error wird gesetzt.                   */
/**************************************************/
void interpret(Collection coll, Patterns p)
{ long ncomms;
  Collection toks;
  char *copy;

  /* Initialisierung */
  pats = p;
  operateOnInputs = TRUE;
  if(! (horiz = newVector(inputDims(p)))) error(1);
  putAllDim(horiz, 0);
  if(! (vert = newVector(num(p)))) error(1);
  putAllDim(vert, 0);

  /* Interpretiere Zeile fuer Zeile */
  ncomms = size(coll);
  for(n = 1L; n <= ncomms; n++)
  {
    line = (char *) at(coll, n);
    execLine();
    if(error) return;
  }         /* for */

  error = 0;
}           /* interpret */


/**************************************************/
/* Antworte 0, falls eine Operation erfolgreich   */
/* ausgefuehrt wurde, sonst 1.                    */ 
/* Variable error wird gesetzt.                   */
/**************************************************/
static int execLine()
{ char *copy, *first, *second, buf[512], *direction;
  int nr;
  Number n1, n2;
  unsigned u1, u2, ntoks;
  VecColl vc;
  long i, nvecs;
  
  error = 0;

  if(! (copy = my_strdup(line))) error(1);
  strupr(copy);
  if(! (toks = tokens(copy, " \t"))) error(1);
  if(isEmpty(toks)) return;

  first = (char *) at(toks, 1L);
  if(streq(first, "%")) return;                /* Kommentar-Zeile */ 
  printf("%s ... ", line);
  ntoks = (unsigned) size(toks);
  if(ntoks == 1L)
  { batchWarn("too few parameters!");
    return;
  }

  /* Ueberpruefe ersten Parameter */
  if(streq(first, "I"))
  { if(! operateOnInputs)
    { freeVector(horiz);
      if(! (horiz = newVector(inputDims(pats)))) error(1);
    } 
    vc = inputs(pats);
    operateOnInputs = TRUE;
  }
  else if(streq(first, "O"))
  { if(! hasOutputs(pats))
    { batchWarn("No output patterns existing!");
      return;
    }
    if(operateOnInputs)
    { freeVector(horiz);
      if(! (horiz = newVector(outputDims(pats)))) error(1);
    }
    operateOnInputs = FALSE;
    vc = outputs(pats);
  }
  else
  { batchWarn("first parameter must be 'i', 'o' or '%'!");
    return;
  }

  /* Ueberpruefe zweiten Parameter */
  second = (char *) at(toks, 2L);
  if((nr = commandNr(second, commands)) == -1)
  {
    sprintf(buf, "unknown keyword '%s' as second parameter!", second);
    batchWarn(buf);
    return;
  }

  /* Ueberpruefe die Anzahl der Parameter */
  if(commands[nr].nparams && ntoks != commands[nr].nparams)
  { sprintf(buf, "expected exactly %d parameters!",
                  commands[nr].nparams);
    batchWarn(buf);
    return;
  }

  /* Parameter 1 und 2 bereits getestet. Weitere Tests abhaengig von  */
  /* jeweiligem Command.                                              */
  switch(nr)
  {
    case 0:  /* SCALE */
             { ScaleOp sop;
    
               if(sscanf((char *) at(toks, 3), "%f", &n1) != 1)
               { batchWarn("third parameter not a number!");
                 return;
               }
               if(sscanf((char *) at(toks, 4), "%f", &n2) != 1)
               { batchWarn("fourth parameter not a number!");
                 return;
               }
               if(n1 > n2)
               { batchWarn("first number greater than second!");
                 return;
               }
               scaleAll(vc, n1, n2, &sop);
             }
             break;
    case 1:  /* NORMALIZE */
             doRows(vc, normalize);
             break;
    case 2:  /* FFT */
             if(! isPowerOf2((unsigned) numCols(pats)))
	     { batchWarn("FFT can only be applied to patterns with 2^N dimensions!");
               return;
             } 
             fftRows(vc);
             if(error) return;
             break;
    case 3:  /* HLOG */
             if(! isSquare((unsigned) numCols(pats)))
             { batchWarn("HLOG can only be applied to quadratic patterns!");
               return;
             }
             doRows(vc, hlog);
             break;
    case 4:  /* PCA */
             { FILE *pipe, *pcaIn, *pcaOut;
               char *tempIn, *tempOut, buf[512];

               /* Schreibe Patterns in ein temporaeres LVQ-File */
               if(! (tempIn = tempnam(NULL,"@PCA_IN"))) error(39);
               if(! (tempOut = tempnam(NULL, "@PCA_OUT"))) error(39);
	       if(! (pcaIn = fopen(tempIn, "w"))) 
               { sprintf(errorInfo, "Temporary file: %s", tempIn);
                 error(34);
               }
               lvqWrite(pats, pcaIn, operateOnInputs);
               fclose(pcaIn);
               if(error) { unlink(tempIn); error(35); }
               sprintf(buf, "pca %s > %s", tempIn, tempOut);
               pipe = popen(buf, "r");
               if(! pipe) { unlink(tempIn); unlink(tempOut); error(36); }
               pclose(pipe);
               unlink(tempIn);
               free(tempIn);
               if(! (pcaOut = fopen(tempOut, "r"))) 
                 { unlink(tempOut); error(38); }
               lvqRead(pats, pcaOut, operateOnInputs);
               fclose(pcaOut);
               unlink(tempOut);
               free(tempOut);
               if(error) error(37);
             }
             break;
    case 5:  /* CLASSEXPAND */
             { Vector pad, hilf;

               if(! (pad = newVector(maxClassNo(pats)))) error(1);
               putAllDim(pad, 0);
               expandWithClassVectors(pats, operateOnInputs);
               if(error) return;
               hilf = horiz;
               if(! (horiz = expand(hilf, pad))) error(1);
               freeVector(hilf);
               freeVector(pad);
             }
             break;
    case 6:  /* OUTPUTEXPAND */
             if(! hasOutputs(pats))
             { batchWarn("no output patterns existing");
               return;
             }
             { Vector hilf, pad;

               if(! (pad = newVector(outputDims(pats)))) error(1);
               putAllDim(pad, 0);
               expandWithOutputs(pats);
               if(error) return;
               hilf = horiz;
               if(! (horiz = expand(hilf, pad))) error(1);
               freeVector(hilf);
               freeVector(pad);
             }
             break;
    case 7:  /* RMDIMRANGE */
             if(sscanf((char *) at(toks, 3), "%u", &u1) != 1)
             { batchWarn("third parameter not a positive integer!");
               return;
             }
             if(sscanf((char *) at(toks, 4), "%u", &u2) != 1)
             { batchWarn("fourth parameter not a positive integer!");
               return;
             }
 
             if(u1 > u2)
	     { batchWarn("third parameter greater than fourth!");
               return;
             } 
             removeCols(pats, operateOnInputs, u1, u2);
             if(error == 29) 
	     { batchWarn("cannot remove all dimensions!");
               error(0);
             }
             if(error) return;
             removeDimRange(horiz, u1, u2);
             if(error) return;
             break;
    case 8:  /* RMCONSTDIMS */
             { long *lp;
               unsigned nremovables;

               if(! (lp = (long *) malloc((numCols(pats)+1) * sizeof(*lp))))
                 error(1);
               nremovables = constCols(vc, lp);
               if(nremovables == 0) { free(lp); break; }
               if(nremovables == numCols(pats))
               { batchWarn("All dimensions are constant. Cannot remove all!");
                 free(lp);
                 return;
               } 
               removeDimList(pats, operateOnInputs, lp, nremovables);
               if(error) return;
               removeDims(horiz, lp, nremovables);
               if(error) return;
               free(lp);
             } 
             break;
    case 9:  /* RANDOMIZE */
             randomize(pats, vert);
             if(error) return;
             break;
    case 10: /* LOADVERT */
    case 11: /* LOADHORIZ */
             { char *third;
               int loadNr;

               if(ntoks < 3)
               { batchWarn("too few parameters!");
                 return;
               }
               third = (char *) at(toks, 3);
               if((loadNr = commandNr(third, loadCommands)) == -1)
               { sprintf(buf, "unknown keyword '%s' as third parameter!", 
                         third);
                 batchWarn(buf);
                 return;
               }
               if(loadCommands[loadNr].nparams && 
                  loadCommands[loadNr].nparams != ntoks)
               { sprintf(buf, "expected exactly %d parameters!",
                              loadCommands[loadNr].nparams);
                 batchWarn(buf);
                 return;
               }
               
               execLoad(loadNr, vc, nr == 10);
               return;
             }
             break;
    case 12: /* MULTIPLY */
             direction = (char *) at(toks, 3);
             if(streq("VERT", direction))
               compScalarsRow(vc, vert, '*');
             else if(streq("HORIZ", direction))
               compScalarsCol(vc, horiz, '*');
             else
             { batchWarn("third parameter must be 'vert' or 'horiz'!");
               return;
             }
             break;
    case 13: /* DIVIDE */
             direction = (char *) at(toks, 3);
             if(streq("VERT", direction))
             { if(detectFirst(vert, 0.0)) error(24);
               compScalarsRow(vc, vert, '/');
             }
             else if(streq("HORIZ", direction))
             { if(detectFirst(horiz, 0.0)) error(24);
               compScalarsCol(vc, horiz, '/');
             }
             else
             { batchWarn("third parameter must be 'vert' or 'horiz'!");
               return;
             }
             break;
    case 14: /* SUBTRACT */
             direction = (char *) at(toks, 3);
             if(streq("VERT", direction))
               compScalarsRow(vc, vert, '-');
             else if(streq("HORIZ", direction))
               compScalarsCol(vc, horiz, '-');
             else
             { batchWarn("third parameter must be 'vert' or 'horiz'!");
               return;
             }
             break;
    case 15: /* ADD */
             direction = (char *) at(toks, 3);
             if(streq("VERT", direction))
               compScalarsRow(vc, vert, '+');
             else if(streq("HORIZ", direction))
               compScalarsCol(vc, horiz, '+');
             else
             { batchWarn("third parameter must be 'vert' or 'horiz'!");
               return;
             }
             break;
    case 16: /* REPLACE */
             { unsigned maxAllowed;
               Boolean vertDir;

               direction = (char *) at(toks, 3);
               if(! streq(direction, "VERT") && ! streq(direction, "HORIZ"))
               { batchWarn("third parameter must be 'vert' or 'horiz'!");
                 return;
               }
               else vertDir = streq(direction, "VERT");
               if(sscanf((char *) at(toks, 4), "%u", &u1) != 1)
               { batchWarn("fourth parameter must be a positive integer!");
                 return;
               }
               maxAllowed = (unsigned) (vertDir ? numCols(pats) : num(pats));
               if(u1 < 1 || u1 > maxAllowed)
               { sprintf(buf, "fourth parameter must be between 1 and %u!",
                              maxAllowed);
                 batchWarn(buf);
                 return;
               }

               if(vertDir) replaceCol(vc, u1, vert);
               else replaceRow(vc, u1, horiz);
             }
             break;
    case 17: /* REFRESHCLASSES */
             if(hasClassNames(pats))
	     { batchWarn("class symbols existing!");
               return;
             }
             if(! hasOutputs(pats))
	     { batchWarn("no output patterns existing!");
               return;
             }
             genClassNosFromVectors(pats);
             if(error) return; 
             break;
    case 18: /* MULTCONST */
             if(sscanf((char *) at(toks, 3), "%f", &n1) != 1)
             { batchWarn("third parameter not a number!");
               return;
             }
             nvecs = size(vc);
             for(i = 1L; i <= nvecs; i++)
               multiply((Vector) at(vc, i), n1); 
             break;
    case 19: /* DIVCONST */
             if(sscanf((char *) at(toks, 3), "%f", &n1) != 1)
             { batchWarn("third parameter not a number!");
               return;
             }
             if(n1 == 0.0)
             { batchWarn("cannot divide by 0!");
               return;
             } 
             nvecs = size(vc);
             for(i = 1L; i <= nvecs; i++)
               multiply((Vector) at(vc, i), 1 / n1); 
             break;
    case 20: /* ADDCONST */
             if(sscanf((char *) at(toks, 3), "%f", &n1) != 1)
             { batchWarn("third parameter not a number!");
               return;
             }
             nvecs = size(vc);
             for(i = 1L; i <= nvecs; i++)
               addConst((Vector) at(vc, i), n1); 
             break;
    case 21: /* SUBCONST */
             if(sscanf((char *) at(toks, 3), "%f", &n1) != 1)
             { batchWarn("third parameter not a number!");
               return;
             }
             nvecs = size(vc);
             for(i = 1L; i <= nvecs; i++)
               addConst((Vector) at(vc, i), - n1); 
             break;
    case 22: /* RMDIMS */
             { unsigned count, maxAllowed, dim;
               long *lp;
               
               maxAllowed = (unsigned) numCols(pats);
               if(! (lp = (long *) malloc((maxAllowed+1) * sizeof(*lp))))
                 error(1); 
               for(count = 0, i = 3L; i <= ntoks; i++)
               { if(sscanf((char *) at(toks, i), "%u", &dim) != 1)
                 { sprintf(buf, "parameter %ld not a positive integer!", i);
                   batchWarn(buf);
                   free(lp);
                   return;
                 }
                 if(dim < 1 || dim > maxAllowed)
                 { sprintf(buf, "parameter %ld must be between 1 and %u!", 
                           i, maxAllowed);
                   batchWarn(buf);
                   free(lp);
                   return;
                 }
                 if(count && lp[count-1] >= dim)
                 { batchWarn("sequence of dimensions not in ascending order!");
                   free(lp);
                   return;
                 }
                 lp[count++] = (long) dim;  
               }      /* for */
               lp[count] = 0L;
               
               if(count == 0) { free(lp); break; } 
               if(count == maxAllowed) 
               { batchWarn("Cannot remove all dimensions!");
                 free(lp);
                 return;
               }
               removeDimList(pats, operateOnInputs, lp, (long) count);
               if(error) return;
               removeDims(horiz, lp, count);
               if(error) return;
               free(lp); 
             }
             break;  
    default: sprintf(errorInfo, "In function execLine():  commandNr = %d!", nr);
             error(20);
             break;
  }         /* switch(nr) */

  free(copy);
  printf("done\n");
}           /* execLine */


/**************************************************/
/**************************************************/
static void execLoad(int nr, VecColl vc, Boolean loadVert)
{ Vector scalarVec;
  void (*collectFunc)();
  unsigned u, maxAllowed;
  Number n1;
  char buf[512];
  long i, ndims;

  if(loadVert)
  { scalarVec = vert;
    collectFunc = collectRows;
  }
  else
  { scalarVec = horiz;
    collectFunc = collectCols;
  }

  switch(nr)
  {
    case 0:  /* MIN */
             (*collectFunc)(vc, minimum, scalarVec);
             break;
    case 1:  /* MAX */
             (*collectFunc)(vc, maximum, scalarVec);
             break;
    case 2:  /* AVG */
             (*collectFunc)(vc, avg, scalarVec);
             break;
    case 3:  /* LENGTH */
             (*collectFunc)(vc, length, scalarVec);
             break;
    case 4:  /* SUM */
             (*collectFunc)(vc, sum, scalarVec);
             break;
    case 5:  /* STDDEV */
             (*collectFunc)(vc, sigma, scalarVec);
             break;
    case 6:  /* OVERALLMIN */
             putAllDim(scalarVec, overallMin(vc));
             break;
    case 7:  /* OVERALLMAX */
             putAllDim(scalarVec, overallMax(vc));
             break;
    case 8:  /* OVERALLAVG */
             putAllDim(scalarVec, overallAvg(vc));
	     break;
    case 9:  /* OVERALLSTDDEV */
             putAllDim(scalarVec, overallStddev(vc));
             break;
    case 10: /* CONST */
             if(sscanf((char *) at(toks, 4), "%f", &n1) != 1)
             { batchWarn("fourth parameter not a number!");
               return;
             }
             putAllDim(scalarVec, n1);
	     break;
    case 11: /* PATTERN */
             if(sscanf((char *) at(toks, 4), "%u", &u) != 1)
             { batchWarn("fourth parameter must be a positive integer!");
               return;
             }
             maxAllowed = (unsigned) (loadVert ? numCols(pats) : num(pats));
             if(u < 1 || u > maxAllowed)
             { sprintf(buf, "fourth parameter must be between 1 and %u!",
                             maxAllowed);
               batchWarn(buf);
               return;
             }

             if(loadVert) colVec(vc, u, vert);
             else copyVec(horiz, (Vector) at(vc, (long) u));
             break;
    case 12: /* VALUES */
             ndims = dims(scalarVec); 
             if(size(toks)-3 != ndims)
             { sprintf(buf, "exactly %ld vector elements required!", ndims);
               batchWarn(buf);
               return;
             }
             for(i = 1L; i <= ndims; i++)
             { if(sscanf((char *) at(toks, i+3), "%f", &n1) != 1)
               { sprintf(buf, "parameter %ld not a number!", i+3);
                 batchWarn(buf);
                 return;
               } 
               putDim(scalarVec, i, n1);
             }    
             break;
    case 13: /* DIM */
             maxAllowed = dims(scalarVec);
             if(sscanf((char *) at(toks, 4), "%u", &u) != 1)
	     { batchWarn("fourth parameter not a positive integer!");
               return;
             }
             if(u < 1 || u > maxAllowed)
	     { sprintf(buf, "fourth parameter must be between 1 and %u!",
                       maxAllowed);
               batchWarn(buf);
               return;
             }
             if(sscanf((char *) at(toks, 5), "%f", &n1) != 1)
             { batchWarn("fifth parameter not a number!");
               return;
             }
             putDim(scalarVec, u, n1);
             break;  
  }
  printf("done\n");
}           /* execLoad */


/**************************************************/
/**************************************************/
static void batchWarn(char *msg)
{
  printf("\nVistra warning: %s\n", msg);
}           /* batchWarn */


/**************************************************/
/**************************************************/
static int commandNr(char *s, Command_Struct *array)
{ int i;

  for(i = 0; array[i].name; i++)
    if(streq(array[i].name, s)) return i;
  return -1;
}           /* commandNr */

