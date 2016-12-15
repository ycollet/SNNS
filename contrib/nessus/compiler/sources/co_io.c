/*****************************************************************************
  FILE              : co_io.c
  SHORTNAME         : io
  VERSION           : 2.0

  PURPOSE           : Contains functions to manage nessus i/o functions (read arrays, read map
                      constants). The functions contained in this file are 
			   - FI_OpenFile      
			   - FI_CloseFile     
			   - FI_ReadArrayFile 
			   - FI_Read###File   
			   - FI_Read###Buffer 
			   - FI_###Copy       
			   - FI_EvalBack###   
			   - FI_MakeArrayOf###, where ### stands for int, float, or string   
			   - FI_ReadMap     
			   - FI_MakeError
			   - FI_CorrectMap  
			   - FI_ReadMapBuffer 
			   - FI_MarkEnd
			   - FI_MarkSep
			   - FI_EvalSep
			   - FI_InitMapLine
			   - FI_EvalList
			   - FI_EvalBackMap
			   - FI_IsSeparator
			   - FI_MakeMapArray
  NOTES             : Assumption for all functions of this file: Input strings of any type are shorter
                      than  the input background  and main buffers  (constant BackBufSize in
	              co_constants.h and system defined constant BUFSIZ).

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_io.h"

static char *InName = NULL;          /* name of current input file (for error messages) */
static int IOLine = 1;   /* current no. of input line (increased whenever '\n' is read) */



/*****************************************************************************************
 Function:                     FI_OpenFile
 Purpose: Open a file, return pointer to file, type of entries, and name in a
          FileValueType entry of a ParserStackType record.
 Returns: ParserStackType record holding FileValueType record.
*****************************************************************************************/
ParserStackType *FI_OpenFile(FName, FType)
     ParserStackType *FName;                               /* name of file to be opened */
     ParserStackType *FType; /*entries type (INTEGER, FLOATNUM, STRING, SUBNET, or MAP) */
{
  ParserStackType *FileDescr;                                /* record to describe file */

  if(FName->ytype != Undef) {
    FileDescr = PA_GetParserStack();
    FileDescr->ytype = FILEVAL;
    FileDescr->yvalue->fileval = (FileValueType *) M_alloc((unsigned)sizeof(FileValueType));
    FileDescr->yvalue->fileval->FileName = FName->yvalue->string;
    FileDescr->yvalue->fileval->FileType = FType->ytype;
    if(FType->ytype != SUBNET)
      if( ! (FileDescr->yvalue->fileval->FilePtr = fopen(FName->yvalue->string, "r")))
	ER_StackError("cannot open file '%s'\n", Serious, STR_Save(FName->yvalue->string),
		      nst, nst);  
    /* subnet source files are not opened - ==> scanner i/o !! */
    PA_FreeParserStack(FName);
    PA_FreeParserStack(FType);
    return FileDescr;
  }
  else 
    return FName;
}





/*****************************************************************************************
 Function:                     FI_CloseFile
 Purpose: Close file described by argument.
 Returns: void.
 Side effects: Modifies argument record.
*****************************************************************************************/
void FI_CloseFile(FileVal)
     ParserStackType *FileVal;
{
  (void) fclose(FileVal->yvalue->fileval->FilePtr);
  FileVal->yvalue->fileval->FilePtr = NULL;
}





/*****************************************************************************************
 Function:                     FI_ReadArrayFile
 Purpose: Reads a file of integers, floats, or strings which are to be used as array.
          Builds an array ParserStackType record and returns it (to return values AND
	  length of array). 
 Returns: Pointer to ParserStackType record holding array.
          NULL if file cannot be read.
*****************************************************************************************/
ParserStackType *FI_ReadArrayFile(FileCons)
     ParserStackType *FileCons; /* points to symbol table entry value for file constant */
{
  ParserStackType *Result;
  if( ! FileCons->yvalue->fileval->FilePtr) {               /* file could not be opened */
    FileCons->ytype = Undef;
    return FileCons;
  }
  else {
    InName = FileCons->yvalue->fileval->FileName;             /* set name of input file */
    IOLine = 1;
    switch(FileCons->yvalue->fileval->FileType) {       /* look at type of file entries */
    case XINT:                                       /* file contains array of integers */
      Result = FI_ReadIntFile(FileCons->yvalue->fileval, 0);
      break;   
    case XFLOAT:                                       /* file contains array of floats */
      Result = FI_ReadFloatFile(FileCons->yvalue->fileval, 0);
      break;   
    case STRING:                                      /* file contains array of strings */
      Result = FI_ReadStringFile(FileCons->yvalue->fileval, 0);
      break;
#ifdef TEST
    default:                               /* file of MAP or SUBNET -- should not occur */
      Result = NULL;
      (void) printf("******bug: illegal file type in FI_ReadArrayFile: %s\n", 
		    STR_TypeString(FileCons->yvalue->fileval->FileType));
#endif
    }
    FI_CloseFile(FileCons);
    (void) M_free((char*) FileCons->yvalue->fileval);
    if(Result)
      PA_FreeParserStack(FileCons);
    else {
      Result = FileCons;        /* error case : FI_ReadArrayFile must not return NULL ! */
      Result->ytype = Undef;
    }
    return Result;
  }
}





/*****************************************************************************************
 Function:                     FI_ReadIntFile
 Purpose: Reads a file of integers which is to be used as array of int. Builds an array
          ParserStackType record and returns it (to return values AND length of array).
 Returns: Pointer to ParserStackType record holding array of integer values.
 Side effects: File is closed, file pointer in <parameter> -> FilePtr is set to NULL.
               File can be read only once.
*****************************************************************************************/
ParserStackType *FI_ReadIntFile(FileEntry, Length)
     FileValueType *FileEntry;  /* points to symbol table entry value for file constant */
     int Length;     /* try to guess file length: if file is used in "get .. from" file */
{                      /* length must correspond to structure length, else it will be 0 */
  char *InputStream;         /* input buffer for file i/o, buffer size is BUFSIZ (1 KB) */
  char *EOB;          /* current end of input buffer (may be < InputStream + BUFSIZ !!) */
  register char *ActIn;               /* points to symbol BEFORE next integer in buffer */
  register char *NextIn;               /* points to symbol AFTER next integer in buffer */
      /* these two pointers are needed to detect integers overlapping two input buffers */
  register int InNo;                                             /* count integers read */
  int *Inputs;                           /* array into which read integers are inserted */
  register int *ActInt;  /* points to next integer array field whose value will be read */
  int SymbRead;               /* count no. of symbols returned by fread ( != IntNo !! ) */
  char *BackInput;              /* short background buffer to hold overlapping integers */
  InNo = 1;   
  InputStream = M_alloc((unsigned) BUFSIZ+1);                  /* allocate input buffer */
  if(! Length)           /* if length is not set, set it to buffer size (1024 integers) */
    Length = FileArrayExpand;               /* allocate result array of length "Length" */
  ActInt = (Inputs = (int*) M_alloc((unsigned) Length * (sizeof(int))));
  M_SetHeapSize((unsigned) Length * (sizeof(int)));
#ifdef SYSTEMV
  setbuf(FileEntry->FilePtr, InputStream);
#else
  setbuffer(FileEntry->FilePtr, InputStream, BUFSIZ);     /* InputStream becomes buffer */
#endif
  BackInput = M_alloc((unsigned) BackBufSize);                    /* short backgr. buf. */
  BackInput = memset(BackInput, '\0',  BackBufSize);
  do {                                          /* while there's still input to be read */
    if((SymbRead = fread(InputStream, 1, BUFSIZ, FileEntry->FilePtr)) < 0) { /* error ! */
      ER_StackError("i/o error reading input file '%s'\n", Serious,
		    STR_Save(FileEntry->FileName), nst,  nst);
      (void) M_free(BackInput);
      return NULL;
    }
    else
      *(EOB = InputStream + SymbRead) = '\0';
    if(*BackInput) {           /* overlapping integer at the end of the previous buffer */
      ActIn = FI_EvalBackInt(BackInput, InputStream, ActInt, &Inputs, &Length, InNo);
      InNo ++;
      ActInt = Inputs + InNo - 1;
    }
    else
      ActIn = InputStream;             /* begin scanning from begin of the input buffer */
    for(NextIn = EOB-1; NextIn >= InputStream && ISINTCOMP(*NextIn); NextIn--); 
    /* NextIn points now to  begin of last and maybe overlapping integer in InputStream */
    /* FI_ReadIntegerBuffer scans buffer between ActIn and NextIn, inserts int values */
    /* into Inputs, expands Inputs, if necessary (=>modifies Length!), and returns no. */
    /* of integers read + old InNo */
    InNo = FI_ReadIntegerBuffer(ActIn, NextIn, &Inputs, ActInt, &Length, InNo);
    ActInt = Inputs + InNo-1;
    if(NextIn < EOB-1)              /* there is an overlapping integer at end of buffer */
      BackInput = memcpy(BackInput, NextIn + 1,  EOB - NextIn -1);
  } while(SymbRead == BUFSIZ);    /* if SymbRead < BUFSIZ: buffer came to end of file */
  /* if BackInput has been set, there is a last integer to be read in InputStream */
  InNo = FI_ReadIntegerBuffer(NextIn, EOB, &Inputs, ActInt, &Length, InNo);
  (void) M_free(BackInput);
  (void) M_free(InputStream);
  return FI_MakeArrayOfInt(Inputs, InNo);
}





/*****************************************************************************************
 Function:                      FI_ReadIntegerBuffer
 Purpose: Read a set of integers from a buffer range market by 2 &char. Assign these
          integers to array of int elements. Expand this array of int if necessary.
 Returns: int - no. of integer values read.
 Side effects: Modifies *IntField and *LenPtr if *IntField is expanded. The old *Intfield
               is removed.
*****************************************************************************************/
int FI_ReadIntegerBuffer(Lower, Upper, IntFieldPtr, ActElem, LenPtr, Entries)
     register char *Lower, *Upper;      /* lower and upper bound of range to be scanned */
     int **IntFieldPtr;                          /* pointer to array of integers (&int) */
     register int *ActElem;             /* pointer to next free element in *IntFiledPtr */
     register int *LenPtr;         /* pointer to integer holding length of *IntFiledPtr */
     register int Entries;        /* integers read before - used to return result value */
{
  while(Lower < Upper) {                              /* scan range from lower to upper */
    for(; Lower < Upper && ! ISINTCOMP(*Lower); Lower++)
      if(*Lower == '\n') IOLine ++;                    /* look for begin of next digit */
    if(Lower < Upper) {                                 /* begin of a new integer found */
      if( !(Entries <= *LenPtr)) {      /* must expand array, expand by FileArrayExpand */
	(*LenPtr) += FileArrayExpand;
	/* FI_IntCopy expands array, old array is removed, returns ptr. to new array */
	*IntFieldPtr = FI_IntCopy(*IntFieldPtr, (*LenPtr)-FileArrayExpand, *LenPtr);
	ActElem = (*IntFieldPtr)+Entries-1;       /* reset pointer to next free element */
      }
      if(sscanf(Lower, "%d", ActElem) < 0) {
	ER_StackError("i/o error reading input file\n", Serious, nst, nst, nst);
	*ActElem = 0;
      }
      ActElem++;              /* set ActElem to next free field - check length of array */
      Entries++;
      for(; Lower < Upper && ISINTCOMP(*Lower); Lower++)
	;                                                 /* look for end of read digit */
    }
  }
  return Entries;
}





/*****************************************************************************************
 Function:                      FI_IntCopy
 Purpose: Copy an array of integers (size OldLength) into an array of integers (size
          NewLength). Remove old array of integers. Exactly OldLength elements are copied.
 Returns: &int - new array of integers.
 Side effects: *OldArray is modified (removed).
*****************************************************************************************/
int *FI_IntCopy(OldArray, OldLength, NewLength)
     register int *OldArray;           /* old array whose element values will be copied */
     int OldLength, NewLength;                           /* length of old and new array */ 
{                                   /* IT CANNOT BE EXPECTED THAT OldLength < NewLength */
  int *NewArray;                                 /* pointer to new array (return value) */

  NewArray = (int*) realloc((char*) OldArray, (unsigned) NewLength * sizeof(int));    
  M_SetHeapSize((unsigned) (NewLength - OldLength) * (sizeof(int)));
  return NewArray;
}





/*****************************************************************************************
 Function:                      FI_EvalBackInt
 Purpose: Evaluate the short background buffer. First posititions of buffer are occupied
          by the first digits of an integer to which all leading digits of input stream
	  must be appended. FI_EvalBackInt then reads this integer, assigns it to the
	  actual array position and clears the background buffer. 
 Returns: Pointer to next symbol in input stream (after read integer).
 Side effects: *IntPos is modified.
*****************************************************************************************/
char *FI_EvalBackInt(Back, Front, IntPos, IntFieldPtr, LenPtr, Entries)
     int *IntPos;                              /* array insert position for new integer */
     char *Back;               /* background buffer, contains leading digits of integer */
     char *Front;    /* input stream - if there are leading digits in Front, they are a */
     int **IntFieldPtr;                                 /* pointer to array of integers */
     int Entries;                                                /* index of next input */
     int *LenPtr;                                /* actual pointer to array of integers */
{	                 
  register char *ActBack;        /* to scan Back looking for the last digits of integer */
  register char *ActFront;           /* to scan Front, look for continuation of integer */

  for(ActBack = Back; ActBack < Back + BackBufSize && *ActBack; ActBack++)
    ;                                           /* look for first free character in Back */
  for(ActFront = Front; ISINTCOMP(*ActFront); *ActBack++ = *ActFront++)
    ;                  /* copy leading symbols of Front to Back (continuation of string) */
  if(ActBack >= Back + BackBufSize) {          /* overlapping string > background buffer */
    (void) printf("******FATAL: background input buffer too small, input ignored\n");
    return ActFront;
  }
  if( ! (Entries <= *LenPtr)) {          /* must expand array, expand by FileArrayExpand */
    (*LenPtr) += FileArrayExpand;
    /* FI_IntCopy expands array, old array is removed, returns ptr. to new array */
    *IntFieldPtr = FI_IntCopy(*IntFieldPtr, (*LenPtr)-FileArrayExpand, *LenPtr);
    IntPos = (*IntFieldPtr)+Entries;        /* reset pointer to next free element */
  }
  /* read integer - check if there's been an i/o-error */
  if(sscanf(Back, "%d", IntPos) < 0) {
    ER_StackError("i/o error reading input file (value set to 0)\n", Serious, nst, nst, nst);
    *IntPos = 0;
  }
  Back = memset(Back, '\0', BackBufSize-1);                  /* clear background buffer */
  return ActFront;          /* return pointer to next symbol of input stream to be read */
}





/*****************************************************************************************
 Function:                     FI_MakeArrayOfInt
 Purpose: Builds a ParserStackType record to hold input array. Copy array to get exact
          length. 
 Returns: Pointer to ParserStackType record holding array values, size and type.
 Side effects: Array passed as argument is removed.
*****************************************************************************************/
ParserStackType *FI_MakeArrayOfInt(Array, Length)
     int *Array;                       /* array of integers whose values must be copied */
     int Length;      /* no. of integer values contained in Array (Array can be longer) */
{
  ParserStackType *Result;         /* returned value: Pointer to ParserStackType record */


  Result = PA_GetParserStack();
  if(Length) {
    Result->ytype = ArrInt;
    Result->yvalue->array = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
    Result->yvalue->array->aSize = Length;
    Result->yvalue->array->Named = FALSE;                /* there's no identifier for it */
    Result->yvalue->array->aRange = FALSE;                  /* explicit value list, only */
    Result->yvalue->array->aValue.aint = FI_IntCopy(Array, Length, Length);
  }
  else 
    Result->ytype = Undef;                          /* illegal NULL sized array or error */
  return Result;
}




/* ### = float */



/*****************************************************************************************
 Function:                     FI_ReadFloatFile
 Purpose: Reads a file of floats which is to be used as array of float. Builds an array
          ParserStackType record and returns it (to return values AND length of array).
 Returns: Pointer to ParserStackType record holding array of float values.
 Side effects: File is closed, file pointer in <parameter> -> FilePtr is set to NULL.
               File can be read only once.
*****************************************************************************************/
ParserStackType *FI_ReadFloatFile(FileEntry, Length)
     FileValueType *FileEntry;  /* points to symbol table entry value for file constant */
     int Length;     /* try to guess file length: if file is used in "get .. from" file */
{                      /* length must correspond to structure length, else it will be 0 */
  char *InputStream;         /* input buffer for file i/o, buffer size is BUFSIZ (1 KB) */
  char *EOB;          /* current end of input buffer (may be < InputStream + BUFSIZ !!) */
  register char *ActIn;                 /* points to symbol BEFORE next float in buffer */
  register char *NextIn;                 /* points to symbol AFTER next float in buffer */
        /* these two pointers are needed to detect floats overlapping two input buffers */
  register int InNo;                                               /* count floats read */
  float *Inputs;                           /* array into which read floats are inserted */
  register float *ActFloat;   /*ptr. to next float array field whose value will be read */
  int SymbRead;               /* count no. of symbols returned by fread ( != IntNo !! ) */
  char *BackInput;                /* short background buffer to hold overlapping floats */

  InNo = 1;   
  InputStream = M_alloc((unsigned) BUFSIZ + 1);                /* allocate input buffer */
  if(! Length)           /* if length is not set, set it to buffer size (1024 integers) */
    Length = FileArrayExpand;               /* allocate result array of length "Length" */
  ActFloat = (Inputs = (float*) M_alloc((unsigned) Length * sizeof(float)));
  M_SetHeapSize((unsigned) Length * (sizeof(float)));
#ifdef SYSTEMV
  setbuf(FileEntry->FilePtr, InputStream);
#else
  setbuffer(FileEntry->FilePtr, InputStream, BUFSIZ);     /* InputStream becomes buffer */
#endif
  BackInput = M_alloc((unsigned) BackBufSize);                    /* short backgr. buf. */
  BackInput =  memset(BackInput, '\0', BackBufSize);/* fill up backgr. buffer with '\0' */
  do {                                          /* while there's still input to be read */
    if((SymbRead = fread(InputStream, 1, BUFSIZ, FileEntry->FilePtr)) < 0) { /* error ! */
      ER_StackError("i/o error reading input file '%s'\n", Serious,
		    STR_Save(FileEntry->FileName), nst,  nst);
      (void) M_free(BackInput);
      return NULL;
    }
    else
      *(EOB = InputStream + SymbRead) = '\0';
    if(*BackInput) {           /* overlapping integer at the end of the previous buffer */
      ActIn = FI_EvalBackFloat(BackInput, InputStream, ActFloat, &Inputs, &Length, InNo);
      InNo ++;
      ActFloat = Inputs + InNo - 1;
    }
    else
      ActIn = InputStream;             /* begin scanning from begin of the input buffer */
    for(NextIn = EOB-1; NextIn >= InputStream && ISFLOATCOMP(*NextIn); NextIn--); 
    /* NextIn points now to  begin of last and maybe overlapping integer in InputStream */
    /* FI_ReadIntegerBuffer scans buffer between ActIn and NextIn, inserts int values */
    /* into Inputs, expands Inputs, if necessary (=>modifies Length!), and returns no. */
    /* of integers read + old InNo */
    InNo = FI_ReadFloatBuffer(ActIn, NextIn, &Inputs, ActFloat, &Length, InNo);
    ActFloat = Inputs + InNo - 1;
    if(NextIn < EOB-1)              /* there is an overlapping integer at end of buffer */
      BackInput = memcpy(BackInput, NextIn + 1, EOB - NextIn - 1);
  } while(SymbRead == BUFSIZ);    /* if SymbRead < BUFSIZ: buffer came to end of file */
  /* if BackInput has been set, there is a last integer to be read in InputStream */
  InNo = FI_ReadFloatBuffer(NextIn, EOB, &Inputs, ActFloat, &Length, InNo);
  (void) M_free(BackInput);
  (void) M_free(InputStream);
  return FI_MakeArrayOfFloat(Inputs, InNo);
}





/*****************************************************************************************
 Function:                      FI_ReadFloatBuffer
 Purpose: Read a set of floats from a buffer range market by 2 &char. Assign these
          float values to array of float elements. Expand this array of float if necessary.
 Returns: int - no. of float values read.
 Side effects: Modifies *FloatField and *LenPtr if *FloatField is expanded. The old
               *Floatfield is removed.
*****************************************************************************************/
int FI_ReadFloatBuffer(Lower, Upper, FloatFieldPtr, ActElem, LenPtr, Entries)
     register char *Lower, *Upper;      /* lower and upper bound of range to be scanned */
     float **FloatFieldPtr;                      /* pointer to array of floats (&float) */
     register float *ActElem;         /* pointer to next free element in *FloatFiledPtr */
     register int *LenPtr;         /* pointer to float holding length of *FloatFiledPtr */
     register int Entries;          /* floats read before - used to return result value */
{
  while(Lower < Upper) {                              /* scan range from lower to upper */
    for(; Lower < Upper && ! ISFLOATCOMP(*Lower); Lower++)
      if(*Lower == '\n') IOLine ++;                    /* look for begin of next digit */
    if(Lower < Upper) {                                 /* begin of a new integer found */
      if( ! (Entries <= *LenPtr)) {     /* must expand array, expand by FileArrayExpand */
	(*LenPtr) += FileArrayExpand;
	/* FI_FloatCopy expands array, old array is removed, returns ptr. to new array */
	*FloatFieldPtr = FI_FloatCopy(*FloatFieldPtr, (*LenPtr)-FileArrayExpand, *LenPtr);
	ActElem = (*FloatFieldPtr)+Entries - 1;   /* reset pointer to next free element */
      }
      if(sscanf(Lower, "%f", ActElem) < 0) {
	ER_StackError("i/o error reading input file\n", Serious, nst, nst, nst);
	*ActElem = 0;
      }
      ActElem++;              /* set ActElem to next free field - check length of array */
      Entries++;
      for(; Lower < Upper && ISFLOATCOMP(*Lower); Lower++)
	;                                                 /* look for end of read digit */
    }
  }
  return Entries;
}






/*****************************************************************************************
 Function:                      FI_FloatCopy
 Purpose: Copy an array of floats (size OldLength) into an array of floats (size
          NewLength). Remove old array. Exactly OldLength elements are copied.
 Returns: &int - new array of floats.
 Side effects: *OldArray is modified (removed).
*****************************************************************************************/
float *FI_FloatCopy(OldArray, OldLength, NewLength)
     register float *OldArray;         /* old array whose element values will be copied */
     int OldLength, NewLength;                           /* length of old and new array */ 
{                                   /* IT CANNOT BE EXPECTED THAT OldLength < NewLength */
  float *NewArray;                               /* pointer to new array (return value) */
  NewArray = (float*) realloc((char*) OldArray, (unsigned) NewLength * sizeof(float));/* expand */
  M_SetHeapSize((unsigned) (NewLength - OldLength) * (sizeof(float)));
  return NewArray;
}





/*****************************************************************************************
 Function:                      FI_EvalBackFloat
 Purpose: Evaluate the short background buffer. First posititions of buffer are occupied
          by the first digits of a float to which all leading digits of input stream
	  must be appended. FI_EvalBackInt then reads this float, assigns it to the
	  actual array position and clears the background buffer. 
 Returns: Pointer to next symbol in input stream (after read float).
 Side effects: *FloatPos is modified.
*****************************************************************************************/
char *FI_EvalBackFloat(Back, Front, FloatPos, FloatFieldPtr, LenPtr, Entries)
     float *FloatPos;                            /* array insert position for new float */
     char *Back;                 /* background buffer, contains leading digits of float */
     char *Front;    /* input stream - if there are leading digits in Front, they are a */
     float **FloatFieldPtr;                               /* pointer to array of floats */
     int Entries;                                                /* index of next input */
     int *LenPtr;                                  /* actual pointer to array of floats */
{	                 
  register char *ActBack;          /* to scan Back looking for the last digits of float */
  register char *ActFront;             /* to scan Front, look for continuation of float */

  for(ActBack = Back; ActBack < Back + BackBufSize && *ActBack; ActBack++)
    ;                                          /* look for first free character in Back */
  for(ActFront = Front; ISFLOATCOMP(*ActFront); *ActBack++ = *ActFront++)
    ;                 /* copy leading symbols of Front to Back (continuation of string) */
  if(ActBack >= Back + BackBufSize) {          /* overlapping float > background buffer */
    (void) printf("******FATAL: background input buffer too small, input ignored\n");
    return ActFront;
  }
  if( ! (Entries <= *LenPtr)) {        /* must expand array, expand by FileArrayExpand */
    (*LenPtr) += FileArrayExpand;
    /* FI_FloatCopy expands array, old array is removed, returns ptr. to new array */
    *FloatFieldPtr = FI_FloatCopy(*FloatFieldPtr, (*LenPtr)-FileArrayExpand, *LenPtr);
    FloatPos = (*FloatFieldPtr)+Entries;        /* reset pointer to next free element */
  }
  for(ActBack = Back; *ActBack; ActBack++)
    ;                                          /* look for first free character in Back */
  for(ActFront = Front; ISFLOATCOMP(*ActFront); *ActBack++ = *ActFront++)
    ;  /* copy leading DIGITS (cannot be sign) of Front to Back (continuation of float) */
  /* read float - check if there's been an i/o-error */
  if(sscanf(Back, "%f", FloatPos) < 0) {
    ER_StackError("i/o error reading input file (value set to 0)\n", Serious, nst, nst, nst);
    *FloatPos = 0;
  }
  Back = memset(Back, '\0', BackBufSize);                           /* clear background buffer */
  return ActFront;          /* return pointer to next symbol of input stream to be read */
}





/*****************************************************************************************
 Function:                     FI_MakeArrayOfFloat
 Purpose: Builds a ParserStackType record to hold input array. Copy array to get exact
          length. 
 Returns: Pointer to ParserStackType record holding array values, size and type.
 Side effects: Array passed as argument is removed.
*****************************************************************************************/
ParserStackType *FI_MakeArrayOfFloat(Array, Length)
     float *Array;                     /* array of floats whose values must be copied */
     int Length;      /* no. of float values contained in Array (Array can be longer) */
{
  ParserStackType *Result;         /* returned value: Pointer to ParserStackType record */

  Result = PA_GetParserStack();
  if(Length) {
    Result->ytype = ArrFloat;
    Result->yvalue->array = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
    Result->yvalue->array->aSize = Length;
    Result->yvalue->array->Named = FALSE;                /* there's no identifier for it */
    Result->yvalue->array->aRange = FALSE;                  /* explicit value list, only */
    Result->yvalue->array->aValue.afloat = FI_FloatCopy(Array, Length, Length);
  }
  else 
    Result->ytype = Undef;                          /* illegal NULL sized array or error */
  return Result;
}





/* ### = string */

/*****************************************************************************************
 Function:                     FI_ReadStringFile
 Purpose: Reads a file of strings which is to be used as array of string. Builds an array
          ParserStackType record and returns it (to return values AND length of array).
 Returns: Pointer to ParserStackType record holding array of string values.
 Side effects: File is closed, file pointer in <parameter> -> FilePtr is set to NULL.
               File can be read only once.
*****************************************************************************************/
ParserStackType *FI_ReadStringFile(FileEntry, Length)
     FileValueType *FileEntry; /* points to symbol table entry value for file constant */
     int Length;    /* try to guess file length: if file is used in "get .. from" file */
{                     /* length must correspond to structure length, else it will be 0 */
  char *InputStream;        /* input buffer for file i/o, buffer size is BUFSIZ (1 KB) */
  char *EOB;         /* current end of input buffer (may be < InputStream + BUFSIZ !!) */
  register char *ActIn;               /* points to symbol BEFORE next string in buffer */
  register char *NextIn;               /* points to symbol AFTER next string in buffer */
      /* these two pointers are needed to detect strings overlapping two input buffers */
  register int InNo;                                             /* count strings read */
  char **Inputs;                         /* array into which read strings are inserted */
  register char **ActString;/*ptr. to next string array field whose value will be read */
  int SymbRead;              /* count no. of symbols returned by fread ( != IntNo !! ) */
  char *BackInput;              /* short background buffer to hold overlapping strings */

  InNo = 1;   
  InputStream = M_alloc((unsigned) BUFSIZ  + 1);               /* allocate input buffer */
  if(! Length)           /* if length is not set, set it to buffer size (1024 integers) */
    Length = FileArrayExpand;               /* allocate result array of length "Length" */
  ActString = (Inputs = (char **) M_alloc((unsigned) Length * (sizeof(char *))));
  M_SetHeapSize((unsigned) Length * (sizeof(char *)));
#ifdef SYSTEMV
  setbuf(FileEntry->FilePtr, InputStream);
#else
  setbuffer(FileEntry->FilePtr, InputStream, BUFSIZ);     /* InputStream becomes buffer */
#endif
  BackInput = M_alloc((unsigned) BackBufSize);                    /* short backgr. buf. */
  BackInput = memset(BackInput, '\0', BackBufSize);     /* fill up baground buffer with '\0' */
  do {                                          /* while there's still input to be read */
    if((SymbRead = fread(InputStream, 1, BUFSIZ, FileEntry->FilePtr)) < 0) { /* error ! */
      ER_StackError("i/o error reading input file '%s'\n", Serious,
		    STR_Save(FileEntry->FileName), nst,  nst);
      (void) M_free(BackInput);
      return NULL;
    }
    else
      *(EOB = InputStream + SymbRead) = '\0';   /* terminate input buffer for later strcpy */
    if(*BackInput) {           /* overlapping integer at the end of the previous buffer */
      ActIn = FI_EvalBackString(BackInput, InputStream, ActString, &Inputs, &Length, InNo);
      InNo ++;
      ActString = Inputs + InNo - 1;
    }
    else
      ActIn = InputStream;             /* begin scanning from begin of the input buffer */
    for(NextIn = EOB-1; NextIn >= InputStream && ISSTRINGCOMP(*NextIn); NextIn--); 
    /* NextIn points now to  begin of last and maybe overlapping integer in InputStream */
    /* FI_ReadIntegerBuffer scans buffer between ActIn and NextIn, inserts int values */
    /* into Inputs, expands Inputs, if necessary (=>modifies Length!), and returns no. */
    /* of integers read + old InNo */
    InNo = FI_ReadStringBuffer(ActIn, NextIn, &Inputs, ActString, &Length, InNo);
    ActString = Inputs + InNo - 1;
    if(NextIn < EOB-1)              /* there is an overlapping integer at end of buffer */
      (void) strcpy(BackInput, NextIn+1);                 /* nextIn+1 = first symb. */
  } while(SymbRead == BUFSIZ);    /* if SymbRead < BUFSIZ: buffer came to end of file */
  /* if BackInput has been set, there is a last integer to be read in InputStream */
  InNo = FI_ReadStringBuffer(NextIn, EOB, &Inputs, ActString, &Length, InNo);
  (void) M_free(BackInput);
  (void) M_free(InputStream);
  return FI_MakeArrayOfString(Inputs, InNo-1);         /* InNo is always 1 ahead */
}




/*****************************************************************************************
 Function:                      FI_ReadStringBuffer
 Purpose: Read a set of strings from a buffer range market by 2 &char. Assign these
          string values to array of string elements. Expand this array of string if necessary.
 Returns: int - no. of string values read.
 Side effects: Modifies *StringField and *LenPtr if *StringField is expanded. The old
               *Stringfield is removed.
*****************************************************************************************/
int FI_ReadStringBuffer(Lower, Upper, StringFieldPtr, ActElem, LenPtr, Entries)
     register char *Lower, *Upper;      /* lower and upper bound of range to be scanned */
     char ***StringFieldPtr;                   /* pointer to array of strings (&string) */
     register char **ActElem;        /* pointer to next free element in *StringFiledPtr */
     register int *LenPtr;       /* pointer to string holding length of *StringFiledPtr */
     register int Entries;         /* strings read before - used to return result value */
{
  register char *End;                             /* points to (first) string separator */

  while(Lower < Upper) {                              /* scan range from lower to upper */
    for(; Lower < Upper && ! ISSTRINGCOMP(*Lower); Lower++)
      if(*Lower == '\n') IOLine ++;                   /* look for begin of next string */
    if(Lower < Upper) {                                  /* begin of a new string found */
      if( ! (Entries <= *LenPtr)) {       /* must expand array, expand by FileArrayExpand */
	(*LenPtr) += FileArrayExpand;
	/* FI_StringCopy expands array, old array is removed, returns ptr. to new array */
	*StringFieldPtr = FI_StringCopy(*StringFieldPtr, (*LenPtr)-FileArrayExpand, *LenPtr);
	ActElem = (*StringFieldPtr)+Entries - 1;  /* reset pointer to next free element */
      }
      for(End = Lower; End <= Upper && ISSTRINGCOMP(*End); End++)
	;                                                     /* look for end of string */
      if(End <= Upper) {                           /* upper points to 'end of input' + 1 */
	*ActElem = strncpy(M_alloc((unsigned) (End-Lower)+1), Lower, End-Lower); 
	/* insert input string into array (input string may  be modified later -> copy) */
      }
      else {                                 /* string does not end before Upper -- bug */
	(void) printf("******bug: mismatch upper, lower in FI_ReadStringBuffer at \n'%s'\n\n",
		      Lower);
	*ActElem = "";
      }
      Lower = End+1;                              /* set pointer to begin of new string */
      ActElem++;              /* set ActElem to next free field - check length of array */
      Entries ++;
    }
  }
  return Entries;
}






/*****************************************************************************************
 Function:                      FI_StringCopy
 Purpose: Copy an array of strings (size OldLength) into an array of strings (size
          NewLength). Remove old array. Exactly OldLength elements are copied.
 Returns: &int - new array of strings.
 Side effects: *OldArray is modified (removed).
*****************************************************************************************/
char **FI_StringCopy(OldArray, OldLength, NewLength)
     register char **OldArray;         /* old array whose element values will be copied */
     int OldLength, NewLength;                           /* length of old and new array */ 
{                                   /* IT CANNOT BE EXPECTED THAT OldLength < NewLength */
  char  **NewArray;                              /* pointer to new array (return value) */

  NewArray = (char**) realloc((char*) OldArray, (unsigned) NewLength * sizeof(char*)); 
  M_SetHeapSize((unsigned) (NewLength - OldLength) * (sizeof(char *)));
  return NewArray;
}





/*****************************************************************************************
 Function:                      FI_EvalBackString
 Purpose: Evaluate the short background buffer. First posititions of buffer are occupied
          by the first digits of a string to which all leading digits of input stream
	  must be appended. FI_EvalBackInt then reads this string, assigns it to the
	  actual array position and clears the background buffer. 
 Returns: Pointer to next symbol in input stream (after read string).
 Side effects: *StringPos is modified.
*****************************************************************************************/
char *FI_EvalBackString(Back, Front, StringPos, StringFieldPtr, LenPtr, Entries )
     char **StringPos;                           /* array insert position for new string */
     char *Back;                 /* background buffer, contains leading digits of string */
     char *Front;     /* input stream - if there are leading digits in Front, they are a */
     char ***StringFieldPtr;                              /* pointer to array of strings */
     int Entries;                                                 /* index of next input */
     int *LenPtr;                                  /* actual pointer to array of strings */
{	                    /* continuation of the string whose first digits are in Back */
  register char *ActBack;          /* to scan Back looking for the last digits of string */
  register char *ActFront;             /* to scan Front, look for continuation of string */

  for(ActBack = Back; ActBack < Back + BackBufSize && *ActBack; ActBack++)
    ;                                           /* look for first free character in Back */
  for(ActFront = Front; ISSTRINGCOMP(*ActFront); *ActBack++ = *ActFront++)
    ;                  /* copy leading symbols of Front to Back (continuation of string) */
  if(ActBack >= Back + BackBufSize) {          /* overlapping string > background buffer */
    (void) printf("******FATAL: background input buffer too small, input ignored\n");
    return ActFront;
  }
  /* read string - check if there's been an i/o-error */
  if( ! (Entries <= *LenPtr)) {       /* must expand array, expand by FileArrayExpand */
    (*LenPtr) += FileArrayExpand;
    /* FI_StringCopy expands array, old array is removed, returns ptr. to new array */
    *StringFieldPtr = FI_StringCopy(*StringFieldPtr, (*LenPtr)-FileArrayExpand, *LenPtr);
    StringPos = (*StringFieldPtr)+Entries - 1;  /* reset pointer to next free element */
  }
  *StringPos = STR_Save(Back);                            /* copy input into array value */
  Back = memset(Back, '\0', BackBufSize);                     /* clear background buffer */
  return ActFront;           /* return pointer to next symbol of input stream to be read */
}





/*****************************************************************************************
 Function:                     FI_MakeArrayOfString
 Purpose: Builds a ParserStackType record to hold input array. Copy array to get exact
          length. 
 Returns: Pointer to ParserStackType record holding array values, size and type.
 Side effects: Array passed as argument is removed.
*****************************************************************************************/
ParserStackType *FI_MakeArrayOfString(Array, Length)
     char * *Array;                     /* array of strings whose values must be copied */
     int Length;      /* no. of string values contained in Array (Array can be longer) */
{
  ParserStackType *Result;         /* returned value: Pointer to ParserStackType record */

  Result = PA_GetParserStack();
  if(Length) {
    Result->ytype = ArrString;
    Result->yvalue->array = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
    Result->yvalue->array->aSize = Length;
    Result->yvalue->array->Named = FALSE;                /* there's no identifier for it */
    Result->yvalue->array->aRange = FALSE;                  /* explicit value list, only */
    Result->yvalue->array->aValue.astring = FI_StringCopy(Array, Length, Length);
  }
  else 
    Result->ytype = Undef;                          /* illegal NULL sized array or error */
  return Result;
}



/**** static variables for read map values ****/
int *iActVal, *iValues;                /* actual and first element of integer values set */
float *fActVal, *fValues;                /* actual and first element of float values set */
char **sActVal, **sValues;              /* actual and first element of string values set */
short SlnLine = FALSE;                  /* TRUE <==> line separator string contains "\n" */
short MlnLine = FALSE;          /* TRUE <==> domain/range separator string contains "\n" */


/*****************************************************************************************
 Function:                     FI_ReadMap
 Purpose: Read a file, containing a map constant.
 Returns: TRUE <==> map constant has been read correctly, else FALSE.
 Side effects: Modifies MapHead,  removes MapSep and LineSep.
*****************************************************************************************/
short FI_ReadMap(MapHead, FilePtr, MapSep, LineSep)
     MapValueType *MapHead;         /* global temporary header record for map  constant */
     ParserStackType *FilePtr;      /* parser  value stack record for map file (opened) */
     ParserStackType *MapSep;           /* string  which separates map domain and range */
     ParserStackType *LineSep;          /*  string which separates map lines (optional) */
{
  char *InBuf, *EOB;                        /* pointer to begin and end of input buffer */
  char *ActIn, *ActEnd;          /* pointer to begin and end of non-overlapping symbols */
  char *ActSep;                    /* pointer to an eventual separation string in InBuf */
  char *BackBuf;                  /* pointer to background buffer (overlapping entries) */
  SymbolType State;     /* XDOMAIN while  domain of map line is read, RANGE  otherwise  */
  MapLineType *Line;                                         /* ptr. to actual map line */
  int LineMax, LNo, SRead;              /* allocated and used  map lines, input symbols */
  int ValMax, VNo;                        /* allocated and used value entries in Values */
  
  if(FI_CorrectMap(MapHead, FilePtr, MapSep, LineSep)) {
    InName = FilePtr->yvalue->fileval->FileName;              /* set name of input file */
    IOLine = 1;
    ER_ListErrors(yylineno, SymTabPtr->FileStr);
    State = XDOMAIN; LineMax = InitMapLines; LNo = 1;     /* initial state and line no. */
    MapHead->mLines =                           /* allocate initial  array of map lines */
      (Line = (MapLineType *) M_alloc((unsigned) LineMax * sizeof(MapLineType)));
    Line->Domain = NULL; Line->Range = NULL;      /* initialize first lines entry */
    InBuf = M_alloc(BUFSIZ+1);
    BackBuf = M_alloc((unsigned) BackBufSize);
    BackBuf = memset(BackBuf, '\0', BackBufSize);
    FI_InitMapLine(&VNo, &ValMax, (State==XDOMAIN)?MapHead->DType:MapHead->RType);
    do {                      /* while there are more symbols unread -- (re)fill buffer */
      if((SRead = fread(InBuf, 1, BUFSIZ, FilePtr->yvalue->fileval->FilePtr)) < 0) {      
	ER_StackError("i/o error reading input file '%s'\n", Serious,
		      STR_Save(FilePtr->yvalue->fileval->FileName), nst,  nst);
	return FI_MakeError(InBuf, BackBuf, MapSep, LineSep);            /* input error */
      }
      else {
	*(EOB = InBuf + SRead) = '\0';                     /* mark  end of input string */
	if(*BackBuf) {                   /* input overlapping last and new input buffer */
	  if( ! (ActIn=FI_EvalBackMap(MapHead,BackBuf,InBuf,EOB,&State,&Line,&LNo,&LineMax,
				        MapSep, LineSep, &VNo,&ValMax)))
	    return FI_MakeError(InBuf, BackBuf, MapSep, LineSep);    /* separator error */
	}
	else
	  ActIn = InBuf;
	for(ActEnd = FI_MarkEnd(MapHead, InBuf, EOB, State,  MapSep, LineSep);
	    ActIn  < ActEnd;)                        /* DO NOT REINITIALIZE MAP LINE !! */
	  if((ActSep = FI_MarkSep(ActIn, ActEnd, State, MapSep, LineSep)) > ActEnd) 
	    return FI_MakeError(InBuf, BackBuf, MapSep, LineSep);      /* sep. sequence */ 
	    /* error (e.g., XDOMAIN, XDOMAIN) separation string found in current buffer */
	  else if( ! ActSep) { /* no separator in current input buffer - read until end */
	    VNo = FI_ReadMapBuffer(ActIn,ActEnd,&ValMax,VNo,MapHead,State); 
	    break;               /* current input buffer scanned completely - exit loop */
	   }
	  else {    /* separator string found - read until separator, then change state */
	     VNo = FI_ReadMapBuffer(ActIn, ActSep, &ValMax, VNo,
				    MapHead, State);      /* change state and map  line */
	     if( ! (ActIn = FI_EvalSep(MapHead, ActSep, &State, &Line, &LNo, &LineMax, 
				MapSep, LineSep,  &VNo, &ValMax)))
	       return FI_MakeError(InBuf, BackBuf, MapSep, LineSep); /* separator error */ 

	  }
	if(ActEnd < EOB)          /* there may be an input string overlapping buffers */
	  BackBuf = memcpy(BackBuf, ActEnd, EOB-ActEnd);       /* ActEnd is copied, too */
      }
    } while(SRead == BUFSIZ);          /* end of file read, last buffer scanned already */
    if(State == XDOMAIN  && VNo == 1) {          /* extra line separator at end of file */
      ER_StackError("map: extra line separator at end of file '%s' ignored\n", Warning,
		    STR_Save(FilePtr->yvalue->fileval->FileName), nst, nst);
      ER_ListErrors(IOLine, InName);
      if(! FI_EvalList(Line, VNo, State, MapHead))      /* there has been  an empty set */
	return  FI_MakeError(InBuf, BackBuf, MapSep, LineSep);
      PA_FreeParserStack(MapSep);                  /* remove parser value stack records */
      PA_FreeParserStack(LineSep);                         /* for map separator strings */
      (void) free(InBuf); (void) free(BackBuf); 
      return FI_MakeMap(MapHead, LNo-1);
    }
    else if(State == RANGE) {       /* correct end of  map  file, pairs (domain, range) */
      VNo =  FI_ReadMapBuffer(ActEnd, EOB, &ValMax, VNo, MapHead, State);
      if( ! FI_EvalList(Line, VNo, State, MapHead))
	 return FI_MakeError(InBuf, BackBuf, MapSep, LineSep);
      PA_FreeParserStack(MapSep);                  /* remove parser value stack records */
      PA_FreeParserStack(LineSep);                         /* for map separator strings */
      (void) free(InBuf); (void) free(BackBuf); 
      return FI_MakeMap(MapHead, LNo);
    }
    else {                               /* no. of domains does not match no. of ranges */
      ER_StackError("map: missing range values at end of '%s'\n", Serious,
		    STR_Save(FilePtr->yvalue->fileval->FileName), nst, nst); 
      return FI_MakeError(InBuf, BackBuf, MapSep, LineSep);
    }
  }
  else 
    return FI_MakeError(NULL, NULL, MapSep, LineSep);
}






/*****************************************************************************************
 Function:                     FI_MakeError
 Purpose: Error handling: Remove arguments and return FALSE.
 Returns: FALSE. 
 Side effects: Arguments are removed.
*****************************************************************************************/
short FI_MakeError(Gb, Bb, Ms, Ls)
     char *Gb, *Bb;                /* main and background input buffers - will be freed */
     ParserStackType *Ms, *Ls;    /* domain/range and line separators for  map constant */
{
  if(Gb) {
    (void) free(Gb);
    ER_ListErrors(IOLine, InName);
  }
  if(Bb)
    (void) free(Bb);
  PA_FreeParserStack(Ms);
  PA_FreeParserStack(Ls);
  return FALSE;
}





/*****************************************************************************************
 Function:                     FI_CorrectMap
 Purpose: Check if parameters are a vaalid specification of a map file and header.
 Returns: TRUE <=> parameters are ok, file can be read, else FALSE.
 Side effects: Modifies LineSep, if it has been NULL  before (==> "\n" is default).
*****************************************************************************************/
short FI_CorrectMap(MapHead, FilePtr, MSep, LSep)
     MapValueType *MapHead;         /* global temporary header record for map  constant */
     ParserStackType *FilePtr;      /* parser  value stack record for map file (opened) */
     ParserStackType *MSep;             /* string  which separates map domain and range */
     ParserStackType *LSep;                     /* string to separate map lines or NULL */
{
  register  char *s;

  STMT_GetIdValue(FilePtr);       /* file ptr. stands for an  identifier, get its value */
  if(FilePtr->yvalue->fileval->FilePtr) {                       /* file has been opened */
    if(MSep)                                       /* there is a domain/range separator */
      if(MSep->ytype != SIMPLESTRING && MSep->ytype != LONGSTRING) {    /* illegal type */
	if( MSep->ytype != Undef)     /* no previous error in expression, print message */
	  ER_StackError("type mismatch: string expected\n", Serious, nst, nst, nst);
	return FALSE;                                        /* map file cannot be read */
      }
      else if(! MSep->yvalue->string) { /* missing domain/range separator specification */
	ER_StackError("map: missing domain / range separator\n", Serious, nst, nst, nst);
	return FALSE;
      }                              
    else                                    /* correctly defined domain/range separator */
      for(s=strchr(MSep->yvalue->string, '\\'); s; s=strchr(s, '\\'))
	  if(*(s++) == 'n') 
	    MlnLine = TRUE;                         /* separator string contains a "\n" */
    if(LSep)                                       /* there is a domain/range separator */
      if(LSep->ytype != SIMPLESTRING && LSep->ytype != LONGSTRING) { 
	if( LSep->ytype != Undef)           /* no previous error in expression, message */
	  ER_StackError("type mismatch: string expected\n", Serious, nst, nst, nst);
	return FALSE;   /* illegal type of map lines separator, map file cannot be read */
      }                                     /* correctly defined separator of map lines */
      else if(! LSep->yvalue->string) {
	SlnLine = TRUE;
	*(LSep->yvalue->string = M_alloc((unsigned) 1)) = '\n';/* default line separator */
      }
      else 
	for(s=strchr(LSep->yvalue->string, '\\'); s; s=strchr(s, '\\'))
	  if(*(s++) == 'n') 
	    SlnLine = TRUE;                         /* separator string contains a "\n" */
    if(! MapHead)        /* there has been some previous error in map header definition */
      return FALSE;
    return TRUE;                           /*  parameters are ok, map file can be read */
  }
  else 
    return FALSE;                          /* failed to open map file -- cannot be read */
}





/*****************************************************************************************
 Function:                     FI_ReadMapBuffer
 Purpose: Read  entries of input buffer  form Low to Up. The input buffer section does
          not contain any map domain / range or line separator strings.
 Returns: Total no. of values read for this domain or range set.
 Side effects: Modifies *ActVPtr.
*****************************************************************************************/
int FI_ReadMapBuffer(Low, Up,  MaxPtr,  VNo, Head, State)
     char *Low, *Up;    /* lower and upper delimiter of input buffer section to be read */
     int *MaxPtr, VNo;          /* maximum and current no. of input values for this set */
     MapValueType *Head;                    /* pointer to map header description record */
     SymbolType State;       /* actual state (XDOMAIN or RANGE) to get values data type */
{
  State = (State ==  XDOMAIN) ? Head->DType : Head->RType;
  switch(State) {                 /* read function depends on domain or range data type */
  case XINT:
    VNo = FI_ReadIntegerBuffer(Low, Up, &iValues, iActVal, MaxPtr, VNo);
    iActVal = iValues + VNo -1;
    break;
  case XFLOAT:
    VNo = FI_ReadFloatBuffer(Low,Up, &fValues, fActVal, MaxPtr, VNo);
    fActVal = fValues + VNo -1;
    break;
  case STRING:
    VNo = FI_ReadStringBuffer(Low,Up, &sValues, sActVal, MaxPtr, VNo);
    sActVal = sValues + VNo -1;
    break;
  }
  return  VNo;
}



/*****************************************************************************************
 Function:                     FI_MarkEnd
 Purpose: Look for last separator (NOT MAP sepatator !) symbol of input buffer.
 Returns: Pointer to this symbol.
*****************************************************************************************/
char *FI_MarkEnd(Head, Buf, Eob, State, MSep, LSep)
     MapValueType *Head;                               /* map header description record */
     char *Buf, *Eob;                        /* ptrs. to begin and end of  input buffer */
     SymbolType State;                                              /* XDOMAIN or RANGE */
     ParserStackType *MSep, *LSep;       /* separators (domain/range and between lines) */
{
  register char *Pos;                  /* input position, position of separator  string */

  State = (State ==  XDOMAIN) ? Head->DType : Head->RType;
  switch(State) {          /*  look for last symbol which is not part of an input value */
  case XINT:
    for(Pos = Eob-1; Pos >= Buf && ISINTCOMP(*Pos); Pos  --)  ;
    break;
  case XFLOAT:
    for(Pos = Eob-1; Pos >= Buf && ISFLOATCOMP(*Pos); Pos  --)  ;
    break;
  case STRING:
    for(Pos = Eob-1; Pos >= Buf && ISSTRINGCOMP(*Pos); Pos  --)  ;
    break;
  }
  if(Pos == Eob-1) {    /* input string does not terminate with (part of) an input value */
    /* look for a separator string prefix at the end of the input  buffer - if  a string */
    /* is a prefix of the other, always the shorter string is recognized */
    for(; Pos >= Buf && (! strncmp(Pos, MSep->yvalue->string, Eob-Pos-1) || 
			 ! strncmp(Pos, LSep->yvalue->string, Eob-Pos-1)); Pos--)
      ;
  }
  return Pos+1;                  /* pos now points to first symbol of overlapping string */
}





/*****************************************************************************************
 Function:                     FI_MarkSep
 Purpose: Look for next separator string in input buffer section. 
 Returns: Pointer to begin of separator string, if it has been found.
          NULL if there has been no separator string.
	  Upper bound + 1 if there has been an error (line separator in XDOMAIN state or
	  domain/range separator in RANGE state).
*****************************************************************************************/
char *FI_MarkSep(Low, Up, State, MapSep, LineSep)
     char *Low, *Up;                   /* lower and upper bound of input buffer section */
     ParserStackType *MapSep,  *LineSep;       /* domain/range or line separator string */
{
  char *Sep;                           /* expected separator string (MapSep or LineSep) */
  char *Err;               /* unexpected separator string (MapSep or LineSep) --> error */
  int SLen, ELen;                /* length of expected and unexpected separator strings */
  register char *s;               /* pointer to actual position in input buffer section */

  if(State == XDOMAIN) {                      /* domain/range separator string expected */
    SLen = strlen(Sep = MapSep->yvalue->string);
    ELen = strlen(Err = LineSep->yvalue->string);
  }
  else {                             /* state == RANGE , line separator string expected */
    SLen = strlen(Sep = LineSep->yvalue->string);
    ELen = strlen(Err = MapSep->yvalue->string);
  }
  for(s = Low; s <= Up-SLen ; s++)                        /* look for expected separator */
    if( ! strncmp(s, Sep, SLen)) {                   /* expected separator string found */
      Sep = s;                            /*  save position at which Sep has been found */
      break; 
    }
  if(s > Up-SLen)    /* not found */
    Sep = NULL;    
  for(s = Low; s <= Up-ELen ; s++)                         /* look for illegal separator */
    if( ! strncmp(s, Err, ELen)) {                 /* unexpected separator string found */
      Err = s;                            /*  save position at which Err has been found */
      break; 
    }
  if(s > Up-ELen)    /* not found */
    Err = NULL;    
  if( ! Sep && ! Err)                    /* no separator string found in buffer section */
    return NULL;
  else if((Sep && Sep < Err) || ! Err)         /* correct sequence of separator strings */
    return Sep;
  else if(Err) {                /* illegal  sequence of separator strings -- stop input */
    ER_StackError("map: illegal separator sequence in map file\n", Serious,nst,nst,nst);
    return Up+1;                  /* signal  to FI_ReadMap that there has been an error */
  }
  else {                                       /* this statement should not be executed */
    (void) printf("******bug: unpreviewed else case in FI_MarkSep\n");
    return Up+1;
  }
}





/*****************************************************************************************
 Function:                     FI_EvalSep
 Purpose: Eval correct separator string: change  state, allocate new array for  values,
          store old array of values into current map line and - if state has been RANGE - 
	  generate a new map line.
 Returns: Pointer to first symbol in input buffer which follows separator string.
 Side effects: Modifies *StPtr, *Line, *LNo, *LMax, *ActVPtr, *Values, *VNo, *VMax.
*****************************************************************************************/
char *FI_EvalSep(Head, Low, StPtr, Line, LNo, LMax, MSep, LSep, VNo, VMax)
     char *Low;                          /* pointer to first symbol of separator string */
     MapValueType *Head;                    /* pointer to map header description record */
     SymbolType  *StPtr;               /*  pointer to status variable (will be changed) */
     MapLineType **Line;                           /* pointer to actual map  line  ptr. */
     int *LNo, *LMax;               /* pointers to nos. of inserted and allocated lines */
     int *VNo, *VMax;              /* pointers to nos. of inserted and allocated values */
     ParserStackType *MSep, *LSep;                                 /* separator strings */
{
  if( ! FI_EvalList(*Line, *VNo, *StPtr,  Head))       /* finish up value list for line */
    return NULL;                             /* there has been an empty domain or range */
  FI_InitMapLine(VNo, VMax, (*StPtr == XDOMAIN) ? Head->DType : Head->RType);
  if( *StPtr == XDOMAIN) {
    *StPtr = RANGE;                                                     /* change state */
    return Low + strlen(MSep->yvalue->string);            /* map line remains unchanged */
  }
  else {                                          /* end of map line, generate new line */
    (*LNo) ++; 
    if(*LNo > *LMax) {           /* allocated maap lines have been filled, expand array */
      (*LMax) += AddMapLines;                  /* increment ctr. of allocated map lines */

      Head->mLines = (MapLineType *)                
	realloc((char *) Head->mLines, (unsigned) (*LMax) * sizeof(MapLineType));
      *Line = Head->mLines + (*LNo) -1;                 /* reset pointer to actual line */
    }
    else
      (*Line) ++;                                  /* *Line now points to next map line */
    *StPtr = XDOMAIN;                                           /* reset state to XDOMAIN */
    (*Line)->Domain = NULL;
    (*Line)->Range = NULL;                                 /* initialize new line entry */
    return Low + strlen(LSep->yvalue->string); 
  }
}






/*****************************************************************************************
 Function:                     FI_InitMapLine
 Purpose: Initialize new Values list. 
 Returns: Pointer to new values list.
 Side effects: Modifies (iActVal, iValues) or (fActVal, fValues) or (sActVal, sValues).
*****************************************************************************************/
void FI_InitMapLine(NoPtr, MaxPtr, Type)
     int *NoPtr, *MaxPtr;       /* actual insert position and no. of allocated elements */
     SymbolType Type;
{
  switch(Type) {
  case XINT:                                 /* allocate and return array of integers */
    iValues = NULL;
    iActVal = NULL;
    iValues = (int *) M_alloc((unsigned) (*MaxPtr = FileArrayExpand) * (unsigned) sizeof(int));
    iActVal = iValues;
    break;
  case XFLOAT:                                 /* allocate and return array of floats */
    fActVal = (fValues = (float *) M_alloc((unsigned) (*MaxPtr = FileArrayExpand) * sizeof(float)));
    break;
  case STRING:                               /* allocate and return array of strings */
    sActVal = (sValues = (char **) M_alloc((unsigned) (*MaxPtr = FileArrayExpand) * sizeof(char*)));
    break;
  }                                   /* there are no other types (see FI_IsCorrectMap) */
  *NoPtr = 1;                                     /* next insert position for map  line */
}







/*****************************************************************************************
 Function:                     FI_EvalList
 Purpose: Finish up an aarray of  values and insert it into map line domain or range.
 Returns: void.
 Side effects: Modifies *Line, Values.
*****************************************************************************************/
short FI_EvalList(Line, VNo, State, Head)
     MapLineType *Line;
     int  VNo;
     SymbolType State;
     MapValueType *Head;
{
  ArrayValueType *Array;

  if(VNo == 1) {    /* error: there must be at least one element between two separators */
    if(State  == XDOMAIN)
      ER_StackError("map: empty domain set in map line\n", Serious, nst, nst, nst);
    else   /*  State  == RANGE */
      ER_StackError("map: empty range set in map line\n", Serious, nst, nst, nst);
    return FALSE;
  }
  Array = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
  Array->Named = TRUE;                                           /* must not be removed */
  Array->aRange = FALSE;                                    /*  list of explicit values */
  Array->aSize = VNo - 1; 
  if(State == XDOMAIN) {
    State = Head->DType;                                          /* local modification */
    Line->Domain = Array;
  }
  else {
    State = Head->RType;
    Line->Range = Array;
  }
  switch(State) {
  case XINT:
    Array->aValue.aint = (int *)                         /* finish up array of values */
      realloc(iValues, (unsigned) (VNo-1) * sizeof(int)); 
    break;
  case XFLOAT:
    Array->aValue.afloat = (float *)                     /* finish up array of values */
      realloc((char *) fValues, (unsigned) (VNo-1) * sizeof(float));
    break;
  case STRING:
    Array->aValue.astring = (char **)                    /* finish up array of values */
      realloc((char *) sValues, (unsigned) (VNo-1) * sizeof(char *));
    break;
  }
  return TRUE;
}






/*****************************************************************************************
 Function:                     FI_EvalBackMap
 Purpose: Evaluate background buffer. Background buffer contains the leading symbols of
          either an input value or a separation string. FI_EvalBackMap completes the
	  contents of background buffer and evaluates it.
 Returns: Pointer to next input symbol of main buffer.
 Side effects: Modifies *Values, *ActVPtr.
*****************************************************************************************/
char *FI_EvalBackMap(Head, Back, Main, Up, StPtr, Line, LNo, LMax, MSep, LSep, VNo, VMax)
     MapValueType *Head;                    /* pointer to map header description record */
     char *Back, *Main;                                  /* background and  main buffer */
     char *Up;                                                   /* end of main  buffer */
     SymbolType  *StPtr;               /*  pointer to status variable (will be changed) */
     MapLineType **Line;                          /* pointer to actual map  line  ptr. */
     int *LNo, *LMax;               /* pointers to nos. of inserted and allocated lines */
     int *VNo, *VMax;              /* pointers to nos. of inserted and allocated values */
     ParserStackType *MSep, *LSep;                                 /* separator strings */
{
  char *Pos, *BPos;                 /* to copy continuation of string in Main into Back */
  int BackLen;                            /* length of string prefix inserted into back */
  SymbolType Type;       /* data type expected if overlapping string is not a separator */

  BackLen  = strlen(Back);         /* length of back buffer needed to set begin of main */
  if(FI_IsSeparator(Back, Main, MSep->yvalue->string))    
    if(*StPtr == XDOMAIN) {                        /* string is  domain/range separator */
      if( ! FI_EvalSep(Head,Back,StPtr,Line,LNo,LMax,MSep,LSep,VNo,VMax))
	return NULL;                         /* there has been an empty domain or range */
      Back = memset(Back, '\0', strlen(MSep->yvalue->string));
      return Main + strlen(MSep->yvalue->string) - BackLen;
    }
    else {                      /* illegal  sequence of separator strings -- stop input */
      ER_StackError("map: illegal separator sequence in map file\n", Serious,nst,nst,nst);
      return NULL;
    }
  else if(FI_IsSeparator(Back, Main, LSep->yvalue->string))
    if(*StPtr == RANGE) {
      if( ! FI_EvalSep(Head,Back,StPtr,Line,LNo,LMax,MSep,LSep,VNo,VMax))
	return NULL;                                     /* error in separator sequence */
      Back = memset(Back, '\0', strlen(LSep->yvalue->string));
      return Main + strlen(LSep->yvalue->string) - BackLen;
    }
    else {                      /* illegal  sequence of separator strings -- stop input */
      ER_StackError("map: illegal separator sequence in map file\n", Serious,nst,nst,nst);
      return NULL;
    }
  else {                                       /* overlapping string is not a separator */
    Type = (*StPtr  == XDOMAIN) ? Head->DType : Head->RType;
    switch(Type) {
    case XINT:                                             /* copy rest of value string */
      for(Pos = Main, BPos = Back+strlen(Back); 
	  Pos < Up && ISINTCOMP(*Pos); *BPos++ = *Pos++)  ;
      break;
    case XFLOAT:                                           /* copy rest of value string */
      for(Pos = Main, BPos = Back+strlen(Back); 
	  Pos < Up && ISFLOATCOMP(*Pos); *BPos++ = *Pos++)  ;
      break;
    case STRING:                                           /* copy rest of value string */
      for(Pos = Main, BPos = Back+strlen(Back); 
	  Pos < Up && ISSTRINGCOMP(*Pos); *BPos++ = *Pos++)  ;
      break;
    }
    *VNo = FI_ReadMapBuffer(Back, BPos, LMax, *VNo, Head, *StPtr);
    Back = memset(Back, '\0', BackBufSize);
    return Pos;
  }
}





/*****************************************************************************************
 Function:                     FI_IsSeparator
 Purpose: Checks if the string overlapping Back and Main buffers is a given separator.
          If so, all leading symbols of Main pertencing to the separator string  are
	  copied into  Back.
 Returns: TRUE <==> string overlapping Back and Main buffers is separator
 Side effects: Modifies Back.
*****************************************************************************************/
short FI_IsSeparator(Back, Main, Sep)
     char *Back;              /* pointer to background buffer, contains begin of string */
     char *Main;          /* pointer to main buffer, contains resting symbols of string */
     char *Sep;                                                     /* separator string */
{
  register int sepLen, backLen;                         /* strlen(Sep) and strlen(Back) */
  register int minLen;                                /* min(strlen(Back), strlen(Sep)) */
  
  
  minLen = ((backLen = strlen(Back)) < (sepLen = strlen(Sep))) ? backLen : sepLen;
  if( ! strncmp(Back, Sep, minLen))           /* leading symbols of Back and Sep  match */
    if(backLen <= sepLen)        /* resting symbols of separator must  be found in Main */
      if( ! strncmp (Main, Sep + backLen, sepLen - backLen)) {   /* compare rest of Sep */
	Back = strncat(Back, Sep + backLen, sepLen - backLen);    /* fill up bckgr. buf */
	/* background buffer now contains the complete separator string */
	return TRUE;
      }
      else                 /* resting symbols did not match - no  overlapping separator */
	return  FALSE;
    else         /* seplen < backlen, contents of background buffer cannot be separator */
      return  FALSE;
  else        /* leading symbols of separator did not match - no  overlapping separator */
    return  FALSE;
}






/*****************************************************************************************
 Function:                     FI_MakeMapArray
 Purpose: Finissh up a value list. Allocate an ArrayValueType record and let it hold the
          values list.
 Returns: Pointer to an ArrayValueType record which holds values array.
 Side effects: Original array  "Values" is reallocated.
*****************************************************************************************/
ArrayValueType *FI_MakeMapArray(VNo, MapHead, State)
     int VNo;           /* no. of next insert position, Values contains VNo-1 elements */
     MapValueType *MapHead;                                   /* pointer to  map header */
     SymbolType State;                               /* actual state is XDOMAIN or RANGE */
{
  ArrayValueType *Array;                   /* result record which is built and returned */
  SymbolType ValuesType;                                            /* values data type */

  ValuesType = (State == XDOMAIN) ? MapHead->DType : MapHead->RType; 
  Array = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
  Array->Named = FALSE;
  Array->aRange = FALSE;
  Array->aSize = VNo -1;
  switch(ValuesType) {
  case XINT:                                           /* array of integers is built  */
    Array->aValue.aint = (int *) realloc((char *) iValues, 
					 (unsigned) sizeof(int) * (VNo-1));
    return Array;
  case XFLOAT:                                           /* array of floats is built  */
    Array->aValue.afloat = (float *) realloc((char *) fValues, 
					     (unsigned) sizeof(float) * (VNo-1));
    return Array;
  case STRING:                                         /* array of strings is built  */
    Array->aValue.astring = (char **) realloc((char *) sValues, 
					      (unsigned) sizeof(char *) * (VNo-1));
    return Array;
  default:
    (void) printf("******bug: unexpected type %s in FI_MakeMapArray\n",
		  STR_TypeString(ValuesType));
    return NULL;         /* ------------------ this leads to an abnormal end of program */
  }
}
  




/*****************************************************************************************
 Function:                     FI_MakeMap
 Purpose: Finish up map.
 Returns: TRUE.
 Side effects: Modifies Map.
*****************************************************************************************/
short FI_MakeMap(Map, LNo)
     MapValueType *Map;          /* pointer to map header and values description record */
     int LNo;                                                       /* no. of map lines */
{
  Map->mSize = LNo;                                         /* insert  no. of map lines */
  Map->mLines = (MapLineType *) realloc((char *) Map->mLines, 
					 (unsigned) LNo * sizeof(MapLineType));
  /* Map->DType and Map->RType are set already */
  return TRUE;
}
