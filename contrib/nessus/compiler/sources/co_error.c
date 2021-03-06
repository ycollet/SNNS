/*****************************************************************************
  FILE              : co_error.c
  SHORTNAME         : error
  VERSION           : 2.0

  PURPOSE           : contains functions for error handling
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_error.h"


             /**** local static variables ****/
static ErrorStackType *ER_Errors = NULL;              /* stack of error messages */
static ErrorStackType *ER_FreeErrors = NULL;    /* stack of unused error records */


void ER_StackError(Format, Type, S1, S2, S3)
     char *Format;            /* format expression needed to print error message */
     short Type;                /* error type, one of fatal, serious, or warning */
     char *S1, *S2, *S3;        /* error message may contain up to three strings */
{
  register ErrorStackType *NewErr;     /* points to new error description record */

  if(ER_Errors)      /* suppress standard yacc error message if there are others */  
    if( ! strcmp( ER_Errors->Format, "syntax\n")) {
      /* upper message is the extremely helpful "syntax error\n" generated by */
      /* yacc - overwrite it */
      ER_PushFreeError();   
      FatalErrs --;
    }
    else if(! strcmp( Format, "syntax\n")) 
      /* upper message is the extremely helpful "syntax error\n" generated by */
      /* yacc - since there are  other messages, ignore it */
      return;
  NewErr = ER_PopErrorStack();        /* allocate a new error description record */
  NewErr->NextErr = ER_Errors;          /* new record becomes top of error stack */
  ER_Errors = NewErr;
  NewErr->Format = Format;                       /* format expr of error message */
  NewErr->Strs[0] = S1;            /* Save strings to be included into the */
  NewErr->Strs[1] = S2;                    /* error message. Can be NULL ! */
  NewErr->Strs[2] = S3;     
  switch(Type){          /* set type of error, one of fatal, serious, or warning */
  case Fatal:
    NewErr->Type = "fatal: ";
    FatalErrs ++;      /* increment counter of fatal errors, if > 0 no synthesis */
    AbortLoop = TRUE;         /* if parser is currently  evaluating a loop: stop */
    Topology = FALSE;                            /*  no further semantic actions */
    break;
  case Serious:
    NewErr->Type = "serious: ";
    SeriousErrs ++;    /* if counter of serious errors > 0: no output generation */
    AbortLoop = TRUE;         /* if parser is currently  evaluating a loop: stop */
    break;
  case Warning:
    NewErr->Type = "warning: ";
    WarningErrs ++;              /* continue compiling, possibly semantic errors */
    break;
  default:
    (void) printf("******>> bug: illegal error type %d in ER_StackError()\n", Type);
    exit(102);
  }
}



/*****************************************************************************************
 Function:                     ER_PopErrorStack
 Purpose: Handles stack of unused error records.
 Returns: Pointer to ErrorStackType record.
 Side effects: Modifies ER_FreeErrors.
*****************************************************************************************/
ErrorStackType *ER_PopErrorStack()
{
  register ErrorStackType *New; 
  if(ER_FreeErrors) {
    /* there are free records - use first of them */
    New = ER_FreeErrors;
    ER_FreeErrors = ER_FreeErrors->NextErr;
  }
  else
    New = (ErrorStackType*) (M_alloc(sizeof(ErrorStackType)));
  return New;
}




/*****************************************************************************************
 Function:                     ER_ListErrors
 Purpose: Print error messages on terminal and into an eventual listing file. 
 Returns: void
*****************************************************************************************/
void ER_ListErrors(LineNo, FileName)
{
  while(ER_Errors) {    /* print error messages  until stack is empty */
    if( ! SymTabPtr) {
      (void) printf("\"%s\"(%d): %s", "******", LineNo, ER_Errors->Type);   /* screen */
      if(ListOption)                   /* print message header into listing file */
	(void) fprintf(ListFile, "\"%s\"(%d): %s", "******", yylineno-1, ER_Errors->Type);
    }
    else {
      (void) printf("\"%s\"(%d): %s", FileName, LineNo, ER_Errors->Type);   /* screen */
      if(ListOption)                   /* print message header into listing file */
	(void) fprintf(ListFile, "\"%s\"(%d): %s", FileName, LineNo, ER_Errors->Type);
    }
    if(ER_Errors->Strs[2]) {               /* error message with 3 strings */
      /* print it on terminal */
      (void) printf(ER_Errors->Format, ER_Errors->Strs[0], ER_Errors->Strs[1], ER_Errors->Strs[2]);
      if(ListOption)        
	(void) fprintf(ListFile, ER_Errors->Format, ER_Errors->Strs[0], ER_Errors->Strs[1], ER_Errors->Strs[2]);
    }
    else if(ER_Errors->Strs[1]) {           /* error message with 2 strings */
      /* print it on terminal */
      (void) printf(ER_Errors->Format, ER_Errors->Strs[0], ER_Errors->Strs[1]);
      if(ListOption)        
	(void) fprintf(ListFile, ER_Errors->Format, ER_Errors->Strs[0], ER_Errors->Strs[1]);
    }
    else  if(ER_Errors->Strs[0]) {          /* error message with a single string */
      /* print it on terminal */
      (void) printf(ER_Errors->Format, ER_Errors->Strs[0]);
      if(ListOption)        
	(void) fprintf(ListFile, ER_Errors->Format, ER_Errors->Strs[0]);
    }
    else {                                  /* error message without strings */
      /* print it on terminal */
      (void) printf(ER_Errors->Format);
      if(ListOption)        
	(void) fprintf(ListFile, ER_Errors->Format);
    }
    ER_PushFreeError();                /* remove upper message on stack, ER_Errors points to next */
  }
}




/*****************************************************************************************
 Function:                     ER_PushFreeError
 Purpose: Remove the upper error description record from stack and place it on the stack of
          unused error description records. Free the records string components.
 Returns: void
 Side effects: Modifies ER_Errors, *ER_Errors, ER_FreeErrors.
*****************************************************************************************/
void ER_PushFreeError()
{
  register ErrorStackType *Old;         /* temp. point to upper error record on stack */
  register short i;                     /* to scan array of messages */

  ER_Errors = (Old = ER_Errors)->NextErr;
  Old->NextErr = ER_FreeErrors;
  /* do NOT free Old->Type and Old->Format - they are string constants ! */
  for(i=0; i<3; i++)
    if(Old->Strs[i])
      (void) M_free(Old->Strs[i]);
  ER_FreeErrors = Old;
}
