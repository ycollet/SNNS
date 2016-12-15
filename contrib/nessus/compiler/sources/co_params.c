/*****************************************************************************
  FILE              : co_params.c
  SHORTNAME         : params
  VERSION           : 2.0

  PURPOSE           : contains functions to evaluate the programs argument list.
                        - PA_AssignParamVal
			- PA_GetParameterValue
			- PA_UpdateParameter
			- PA_StackPrm
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_params.h"



/*****************************************************************************************
 Function:                     PA_AssignParamVal
 Purpose: Assigns actual to formal program parameters.
 Returns: void
*****************************************************************************************/
void PA_AssignParamVal(ListPtr, ArgPtr)
     ParserStackType *ListPtr;      /* pointer to identifier_list value on parser stack */
     ParserStackType *ArgPtr;                             /* stack of program arguments */
{
  register ParserStackType *ActId;                       /* points to actual identifier */
  register ParserStackType *ActVal;                           /* points to actual value */
  register ParserStackType *Diff;      
  
  for(ActId=ListPtr, ActVal=ArgPtr; ActId && ActVal; ActId=ActId->ynext, ActVal=ActVal->ynext)
    PA_UpdateParameter(ActId->yvalue->var, ActVal);
  if(ActId && ! ActVal) {               /* there are more formal than actual parameters */
    for(; ActId; ActId=ActId->ynext)          /* print an error message for each formal */
      if(ListPtr->yvalue->var->UndefType) {                      /* undefined parameter */
	ER_StackError("missing actual value for parameter %s in net %s\n", Serious,
		      STR_Save(ListPtr->yvalue->var->Name), 
		      STR_Save(SymTabPtr->Network), nst);
	ListPtr->yvalue->var->Constant = TRUE;
	ListPtr->yvalue->var->UndefType = FALSE;        /* avoid further error messages */
	/* but ListPtr->yvalue->var->UndefValue remains TRUE */
      }
  }
  else if(!ActId && ActVal)             /* there are more actual than formal parameters */
    ER_StackError("too many actual parameters for net '%s' (ignored)\n", Serious,
		  STR_Save(SymTabPtr->Network), nst, nst);
  EX_RemoveListTail(ListPtr);
  EX_RemoveListTail(ArgPtr);
}






/*****************************************************************************************
 Function:                     PA_GetParameterValue
 Purpose: Assign a parameter value to corresponding symbol table entry.
 Returns: Pointer to parser value stack record holding new value.
*****************************************************************************************/
ParserStackType *PA_GetParameterValue(Str)
     char           *Str;              /* string which contains catual; parameter value */
{
  register char *Scan;                        /* points to a symbol of numerical string */
  ParserStackType *Value;                      /* hold parameters actual value and type */
  register short FloatFlg;                     /* true <==> parameter cannot be integer */
  register short StringFlg;      /* true <==> parameter cannot be float, must be string */

  FloatFlg = FALSE;
  StringFlg = FALSE;
  for(Scan = (*Str == '+' || *Str == '-')? Str+1: Str; *Scan; Scan ++) {
    /* scan string and look for illegal symbols, overread a leading sign */
    if( ! FloatFlg) 
      if( ! ISINTCOMP(*Scan) )
	FloatFlg = TRUE;
    if(FloatFlg)
      if( ! ISFLOATCOMP(*Scan) )
	StringFlg = TRUE;
  }
  Value = PA_GetParserStack();
  if(StringFlg) {               /* parameter is string, insert a copy into symbol table */
    Value->ytype = (strlen(Str) > 1) ? LONGSTRING : SIMPLESTRING;
    Value->yvalue->string = STR_New(Str);
  }
  else if(FloatFlg) {               /* parameter type is float */
    (void) sscanf(Str, "%f", &(Value->yvalue->floatval));
    Value->ytype = FLOATNUM;
  }
  else {                            /* parameter is an integer */
    (void) sscanf(Str, "%d", &(Value->yvalue->intval));
    Value->ytype = INTEGER;
  }
  return Value;
}




/*****************************************************************************************
 Function:                     PA_UpdateParameter
 Purpose:  Insert a parameter value into symbol table entry. Type of formal and actual
           parameters must match. 
 Returns:  void.
 Side effects: Modifies symbol table entry.
*****************************************************************************************/
void PA_UpdateParameter(Param, Value)
     SymTabLineType *Param;                         /* symbol table entry for parameter */
     ParserStackType *Value;           /* parser value stack record for parameter value */
{
  if( ! Param->UndefType) {                             /* constant already defined */
    if(Param->Type != Value->ytype && ! 
       ((Param->Type == SIMPLESTRING && Value->ytype ==  LONGSTRING) ||
	(Param->Type == LONGSTRING && Value->ytype == SIMPLESTRING))) {
      ER_StackError("type mismatch: actual value for formal parameter ignored\n", Serious,
		    STR_Save(Param->Name), nst, nst);
      return;
    }
  }
  Param->UndefType = FALSE;
  Param->UndefValue = FALSE;
  Param->Constant = TRUE;                     /* constant should not be modified later */
  Param->Type = Value->ytype;
  *(Param->Value) = *(Value->yvalue); 
  /* do NOT use SymbolValueType of Expr - it is reused !! */
}
 




/*****************************************************************************************
 Function:                     PA_StackPrm
 Purpose: First argument points to head of a parser value stack records list. Appends 2nd.
          argument to this list.
 Returns: Ptr. to head of parser value stack records list.
 Side effects: Modifies ynext entry of last list element.
*****************************************************************************************/
ParserStackType *PA_StackPrm(List, New)
     ParserStackType *List;
     ParserStackType *New;
{
  register ParserStackType *Scan;  

  for(Scan = List; Scan->ynext; Scan  = Scan->ynext)
    ;
  Scan->ynext = New;
  return  List;
}
