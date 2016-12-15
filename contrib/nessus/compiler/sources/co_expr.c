/*****************************************************************************
  FILE              : co_expr.c
  SHORTNAME         : expr
  VERSION           : 2.0

  PURPOSE           : Contains functions to evaluate expressions in nessus.
                               - EX_AssertValue                             - EX_EvalSign
                	       - EX_EvalNumExpr                             - EX_EvalIntExpr
			       - EX_EvalStringExpr                          - EX_InitExprList  
			       - EX_AppendExprList                          - EX_MakeExplicitArray
			       - EX_MakeRangeArray                          - EX_CopyStringArray
			       - EX_CopyIntArray                            - EX_CopyFloatArray
			       - EX_BuildIntRange                           - EX_InitMap
			       - EX_InsertMapLine                           - EX_FinishUpMap
			       - EX_CorrectSize                             - EX_GetFieldElem
			       - EX_SelectFieldElem                         - EX_ArctanFunc
			       - EX_CosFunc                                 - EX_SinFunc
			       - EX_ExpFunc                                 - EX_SqrFunc     
			       - EX_SqrtFunc                                - EX_MaxFunc
			       - EX_ConcatFunc                              - EX_GetIntListFunc
			       - EX_GetFloatListFunc                        - EX_GetStringListFunc 
			       - EX_GetIntArrayFunc                         - EX_FuncFloatArrayVal
			       - EX_FuncStringArrayVal                      - EX_ConcatList
			       - EX_ConcatArray                             - EX_EvalFloatOper    (local)
			       - EX_EvalIntOper      (local)                - EX_StringOper       (local)
			       - EX_RemoveList       (local)                - EX_RemoveListTail
			       - EX_RandomFunc
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_expr.h"

            /**** static variable definitions for expression list evaluation */
static ListValType *EX_ListHead = NULL;  /* holds head of expression or identifier list */
            /**** static variable definitions for map constant  evaluation */
static MapValueType *EX_MapConstant = NULL;            /* hold map constant temporarily */
static ParserStackType *EX_ActMapLine = NULL;    /* points to last map relation in list */
static ParserStackType *EX_MapList = NULL;          /* temporary list of map def. lines */
static int EX_ActMapSize = 0;                                /* actual no. of map lines */





/*****************************************************************************************
 Function:             EX_AssertValue        
 Purpose: If the argument is the parser stack value of an variable or constant identifier
          (symbol table entry) checks if it has been defined. The value is put on the
	  stack, if there has been a definition before, else EX_AssertValue displays an
	  error message and marks the variable as undefined.
 Returns: void
 Side effects: Modifies yvalue and ytype of its argument. 
*****************************************************************************************/
void EX_AssertValue(Arg)
     ParserStackType *Arg;    /* parser stack value which may be a variable or constant */
{			      /* identifier */
  SelectionType *Save;
  if(Arg->ytype == IDENTIFIER)
    if(Arg->yvalue->var->UndefType) {
      /* this identifier has not been declared or defined before */
      if(Arg->yvalue->var->Constant)
	ER_StackError("error: missing definition of constant '%s'\n", Serious,
		      STR_Save(Arg->yvalue->var->Name), nst, nst);
      else
	ER_StackError("error: missing declaration for variable '%s'\n", Serious,
		      STR_Save(Arg->yvalue->var->Name), nst, nst);
      Arg->ytype = Undef;       /* supress further error messages */
    }
    else if(Arg->yvalue->var->UndefValue) {
      ER_StackError("%s used but not defined\n", Serious,
		    STR_Save(Arg->yvalue->var->Name), nst, nst);
      Arg->ytype = Undef;       /* supress further error messages */
    }
    else {                      /* variable has been declared and defined - pass value */
      Arg->ytype = Arg->yvalue->var->Type;
      *(Arg->yvalue) = *(Arg->yvalue->var->Value);

    }
  else if(Arg->ytype == Selection) {     /* perform selection => from now on right side */
    Save = Arg->yvalue->selection;
    if(Arg->yvalue->selection->Component == ACT) {
      if((int) Arg->yvalue->selection->Record.unit->Act == RANDOM) {
	ER_StackError("selection: cannot select random unit activation\n", Serious, nst, nst, nst);
	Arg->ytype = Undef;
	return;
      }
      Arg->ytype = FLOATNUM;
      Arg->yvalue->floatval = Arg->yvalue->selection->Record.unit->Act;
    }
    else if(Arg->yvalue->selection->Component == NAME) {
      if( ! Arg->yvalue->selection->Record.unit->UName) { 
	ER_StackError("unit name used but not defined\n", Serious, nst, nst, nst);
	Arg->ytype = Undef;
	return;
      }
      Arg->yvalue->string = Arg->yvalue->selection->Record.unit->UName;
      Arg->ytype = (strlen(Arg->yvalue->string) <= 1) ? SIMPLESTRING : LONGSTRING;
    }
    (void) M_free((char*) Save);                       /* remove selection type record */
  }
  else if(Arg->ytype == RANGE) {
    ER_StackError("type mismatch: array or map range cannot be used as expression\n", 
		  Serious, nst, nst, nst);
    Arg->ytype = Undef;
  }
}






/*****************************************************************************************
 Function:                     EX_EvalSign
 Purpose: Evaluate unary sign. Check if the operands data type is INTEGER or FLOATNUM.
 Returns: Expression result assigned to operand.
 Side effects: Modifies operand.
*****************************************************************************************/
ParserStackType *EX_EvalSign(Op, Arg)
     SymbolType Op;                 /* Op is either PLUS or MINUS */
     ParserStackType *Arg;          /* expression value, cannot be pointer to symbol table */
{
  switch(Arg->ytype) {
  case INTEGER:
    Arg->yvalue->intval *= (Op == MINUS)? -1 : 1;
    break;
  case FLOATNUM:
    Arg->yvalue->floatval *= (Op == MINUS)? -1. : 1.;
    break;
  case Undef:
    break;    /* there has been something wrong - error message already displayed */
  default:    /* new type error - display error message and supress further messages */
    ER_StackError("type mismatch: sign %s, operand of type %s\n", Serious,
		  STR_TypeString(Op), STR_TypeString(Arg->ytype), nst);
    Arg->ytype = Undef;
  }
  return Arg;
}





/*****************************************************************************************
 Function:                     EX_EvalNumExpr
 Purpose: Evaluates a num. expression. The result is returned in the first operand record,
          the second operand is removed.
 Returns: Pointer to ParserStackType holding result.
 Side effects: Modifies both operands.
*****************************************************************************************/
ParserStackType *EX_EvalNumExpr(Oper, Arg1, Arg2)
     SymbolType Oper;            /* one of PLUS, MINUS, MULT, RDIV */
     ParserStackType *Arg1;      /* holds value of left operand, will hold result */
     ParserStackType *Arg2;      /* holds value of right operand */
{
  if(Arg1->ytype == FLOATNUM) {
    if(Arg2->ytype == INTEGER)          /* cast operation on argument 2, ytype is not updated */
      Arg2->yvalue->floatval = (float) Arg2->yvalue->intval;
    else if(Arg2->ytype != FLOATNUM) {  /* type error, operand must be integer or float */
      if(Arg2->ytype != Undef)          /* avoid multiple error messages */
	ER_StackError("type mismatch: operator %s, operand of type %s\n", Serious,
		      STR_TypeString(Oper), STR_TypeString(Arg2->ytype), nst);
      Arg1->ytype = Undef;              /* supress further error messages */
      PA_FreeParserStack(Arg2);         /* remove 2nd. operand */
      return Arg1;
    }                                   /* now, both operands are of type float */
    return EX_EvalFloatOper(Oper, Arg1, Arg2);
  }
  else if(Arg1->ytype == INTEGER)
    if(Arg2->ytype == INTEGER)          /* both operands are integers */
      return EX_EvalIntOper(Oper, Arg1, Arg2);
    else if(Arg2->ytype == FLOATNUM){   /* cast operation on argument 1 needed */
      Arg1->ytype = FLOATNUM;           /* update ytype - Arg1 will hold result ! */
      Arg1->yvalue->floatval = (float) Arg1->yvalue->intval;
      return EX_EvalFloatOper(Oper, Arg1, Arg2);
    }
    else {                              /* right argument of illegal type */
      if(Arg2->ytype != Undef)          /* avoid multiple error messages */
	ER_StackError("type mismatch: operator %s, operand of type %s\n", Serious,
		      STR_TypeString(Oper), STR_TypeString(Arg2->ytype), nst);
      Arg1->ytype = Undef;              /* supress further error messages */
      PA_FreeParserStack(Arg2);         /* remove 2nd. operand */
      return Arg1;
    }
  else {                                /* left argument of illegal type */
    if(Arg1->ytype != Undef) {          /* avoid multiple error messages */
      ER_StackError("type mismatch: operator %s, operand of type %s\n", Serious,
		    STR_TypeString(Oper), STR_TypeString(Arg1->ytype), nst);
      Arg1->ytype = Undef;              /* supress further error messages */
    }
    PA_FreeParserStack(Arg2);           /* remove 2nd. operand */
    return Arg1;
  }
}





/*****************************************************************************************
 Function:                     EX_EvalIntExpr
 Purpose: Evaluates DIV and MOD.
 Returns: First operand holding the result value.
 Side effects: Modifies both operands.
*****************************************************************************************/
ParserStackType *EX_EvalIntExpr(Oper, Arg1, Arg2)
     SymbolType Oper;            /* one of PLUS, MINUS, MULT, RDIV */
     ParserStackType *Arg1;      /* holds value of left operand, will hold result */
     ParserStackType *Arg2;      /* holds value of right operand */
{
  register ParserStackType *Act;
  register short ErrFlg = FALSE;

  for(Act=Arg1; Act; Act=(Act==Arg1)?Arg2:NULL)     /* check both arguments */
    if(Act->ytype == FLOATNUM) {                    /* operand is of type float - error */
      ER_StackError("truncation: operand of %s must be integer\n", Warning,
		    STR_TypeString(Oper), nst, nst);
      Act->ytype = INTEGER;
      Act->yvalue->intval = (int) Act->yvalue->floatval;    /* truncated */
    }
  else if(Act->ytype != INTEGER) {     /* other type => serious type error */
    ER_StackError("type mismatch: operator %s, operand of type %s\n", Serious,
		  STR_TypeString(Oper), STR_TypeString(Act->ytype), nst);
    ErrFlg = TRUE;
  }
  if( ! ErrFlg)                        /* operands ok or truncated - evaluate expression */
    return EX_EvalIntOper(Oper, Arg1, Arg2);
  else {           /* type error : cannot evaluate expression */
    Arg1->ytype = Undef;               /* supress further error messages */
    PA_FreeParserStack(Arg2);          /* remove 2nd. operand */
    return Arg1;
  }        
}





/*****************************************************************************************
 Function:                     EX_EvalStringExpr
 Purpose: Evaluates STRCAT and REMOVE.
 Returns: First operand holding the result value.
 Side effects: Modifies both operands.
*****************************************************************************************/
ParserStackType *EX_EvalStringExpr(Oper, Arg1, Arg2)
     SymbolType Oper;            /* one of PLUS, MINUS, MULT, RDIV */
     ParserStackType *Arg1;      /* holds value of left operand, will hold result */
     ParserStackType *Arg2;      /* holds value of right operand */
{
  if((Arg1->ytype == LONGSTRING || Arg1->ytype == SIMPLESTRING) &&
     (Arg2->ytype == LONGSTRING || Arg2->ytype == SIMPLESTRING))   /* operand types ok */
    if( strlen(Arg1->yvalue->string = EX_StringOper(Oper, Arg1->yvalue->string,
						    Arg2->yvalue->string)) <= 1)
      Arg1->ytype = SIMPLESTRING;
    else
      Arg1->ytype = LONGSTRING;
  else if(Arg1->ytype != LONGSTRING && Arg1->ytype != SIMPLESTRING && Arg1->ytype != Undef) {
    ER_StackError("type mismatch: operator %s, operand of type %s\n", Serious,
		  STR_TypeString(Oper), STR_TypeString(Arg1->ytype), nst);
    Arg1->ytype = Undef;
  }
  else if(Arg2->ytype != LONGSTRING && Arg2->ytype != SIMPLESTRING && Arg2->ytype != Undef) {
    ER_StackError("type mismatch: operator %s, operand of type %s\n", Serious,
		  STR_TypeString(Oper), STR_TypeString(Arg2->ytype), nst);
    Arg1->ytype = Undef;
  }
  else /* arg1 or arg2 of type Undef */
    Arg1->ytype = Undef;
  PA_FreeParserStack(Arg2);          /* remove 2nd. operand */
  return Arg1;
}





/*****************************************************************************************
 Function:                     EX_ListFunc
 Purpose: Evaluates 'min' and 'max' functions. Argument is the expression list appended to
          EX_Listhead. This expression list may consist of a single ParserStackValue
	  record whose value must be an array.
 Returns: Pointer to ParserStackType record holding result value.
*****************************************************************************************/
ParserStackType *EX_ListFunc(Oper)
     SymbolType Oper;                      /* operand must be either MIN or MAX */
{
  register ParserStackType *Result;        /* pointer to result record */

  Result = PA_GetParserStack();            /* allocate result record */
  if(! EX_ListHead->Elements)              /* there has been an error in expression_list */
    Result->ytype = Undef;
  else {
    switch(EX_ListHead->Elements->ytype) {
    case INTEGER:                          /* argument = list of integers */
      Result->ytype = INTEGER;
      Result->yvalue->intval = EX_GetIntListFunc(Oper, EX_ListHead->Elements);
      break;
    case FLOATNUM:                         /* argument = list of floats */
      Result->ytype = FLOATNUM;
      Result->yvalue->intval = EX_GetFloatListFunc(Oper, EX_ListHead->Elements);
      break;
    case SIMPLESTRING:
    case LONGSTRING:                       /* argument = list of strings */
      Result->yvalue->string = EX_GetStringListFunc(Oper, EX_ListHead->Elements);
      if(strlen(Result->yvalue->string) <= 1)
        Result->ytype = SIMPLESTRING;      /* get resulting string type */
      else
        Result->ytype = LONGSTRING;
      break;
    case ArrInt:                           /* argument = array of integers */
      EX_GetIntArrayFunc(Oper, Result);    /* if arg = LIST OF array of integers: error */
      break;
    case ArrFloat:                         /* argument = array of floats */
      EX_GetFloatArrayFunc(Oper, Result);  /* if arg = LIST OF array of floats: error */
      break;
    case ArrString:                        /* argument = array of strings */
      EX_GetStringArrayFunc(Oper, Result); /* if arg = LIST OF array of floats: error */
      break;
    case Undef:                            /* if there has been an error in expression_list, */
      Result->ytype = Undef;               /* the list should be empty, but may be */
      break;                               /* Undef if a file could not be read */
    default:                               /* other argument types lead to a type error */
      Result->ytype = Undef;
      ER_StackError("type mismatch: %s function undefined on type %s\n", Serious,
                    STR_TypeString(Oper), STR_TypeString(EX_ListHead->Elements->ytype), nst);
      break;
    }
    EX_RemoveList();                       /* remove the argument list */
  }
  return Result;
}





/*****************************************************************************************
 Function:                     EX_GetIntListFunc
 Purpose: Evaluates 'min' and 'max' functions on integer arguments. The first element of 
          expression list appended to EX_Listhead is one of the function arguments.
 Returns: Integer - maximum or minimum value of expression_list.
****************************************************************************************/
int EX_GetIntListFunc(Oper, Act)
     SymbolType Oper;                             /* operand must be either MIN or MAX */
     ParserStackType *Act;       /* pointer to first expression value in argument list */
{
  register int Result;

  if(Oper == XMIN) {                             /* get minimum value of argument list */
    for(Result = Act->yvalue->intval; Act; Act = Act->ynext)
      if(Act->yvalue->intval < Result)                          /* old value > minimum */
        Result = Act->yvalue->intval;
  }
  else                                             /* Oper == XMAX , get maximum value */
    for(Result = Act->yvalue->intval; Act; Act = Act->ynext)
      if(Act->yvalue->intval > Result)                          /* old value < maximum */
        Result = Act->yvalue->intval;
  return Result;
}





/*****************************************************************************************
 Function:                     EX_GetFloatListFunc
 Purpose: Evaluates 'min' and 'max' functions on float arguments. The first element of 
          expression list appended to EX_Listhead is one of the function arguments.
 Returns: Float - maximum or minimum value of expression_list.*
****************************************************************************************/
float EX_GetFloatListFunc(Oper, Act)
     SymbolType Oper;                             /* operand must be either MIN or MAX */
     ParserStackType *Act;       /* pointer to first expression value in argument list */
{
  register float Result;

  if(Oper == XMIN) {                             /* get minimum value of argument list */
    for(Result = Act->yvalue->floatval; Act; Act = Act->ynext)
      if(Act->yvalue->floatval < Result)                        /* old value > minimum */
        Result = Act->yvalue->floatval;
  }
  else                                             /* Oper == XMAX , get maximum value */
    for(Result = Act->yvalue->floatval; Act; Act = Act->ynext)
      if(Act->yvalue->floatval > Result)                        /* old value < maximum */
        Result = Act->yvalue->floatval;
  return Result;
}





/*****************************************************************************************
 Function:                     EX_GetStringListFunc
 Purpose: Evaluates 'min' and 'max' functions on string arguments. The first element of 
          expression list appended to EX_Listhead is one of the function arguments.
          MIN and MAX on string lists follow lexical order.
 Returns: String - maximum or minimum value of expression_list.*
****************************************************************************************/
char *EX_GetStringListFunc(Oper, Act)
     SymbolType Oper;                             /* operand must be either MIN or MAX */
     ParserStackType *Act;       /* pointer to first expression value in argument list */
{
  register char *Result;

  if(Oper == XMIN) {                             /* get minimum value of argument list */
    for(Result = Act->yvalue->string; Act; Act = Act->ynext) 
      if(strcmp(Act->yvalue->string, Result) < 0)               /* old value > minimum */
        Result = Act->yvalue->string;
  }
  else                           /* Oper == XMAX , get maximum value */
    for(Result = Act->yvalue->string; Act; Act = Act->ynext)
      if(strcmp(Act->yvalue->string, Result) > 0)               /* old value < maximum */
        Result = Act->yvalue->string;
  return STR_Save(Result);   /* distinction SIMPLESTRING vs. LONGSTRING in EX_ListFunc */
}





/*****************************************************************************************
 Function:                     EX_GetIntArrayFunc
 Purpose: Evaluates 'min' and 'max' functions on integer arrays. There must be exactly one 
          element of type ArrInt in the expression list appended to EX_Listhead.
 Returns: void
 Side effects: Modifies Result.
****************************************************************************************/
void EX_GetIntArrayFunc(Oper, Result)
     SymbolType Oper;                             /* operand must be either MIN or MAX */
     ParserStackType *Result;/* pointer to (unique?) expression value in argument list */
{
  register int *Act;                     /* pointer to actual element of integer array */
  register int Val;                             /* fast access to temporary max or min */
  
  if(EX_ListHead->Elements->ynext) {      /* further elements in list: list of arrays! */
    ER_StackError("type mismatch: %s function undefined on type %s\n", Serious,
                  STR_TypeString(Oper), STR_TypeString(EX_ListHead->Elements->ytype), nst);
    Result->ytype = Undef;
  }
  else {                                             /* single argument of type ArrInt */
    Result->ytype = INTEGER;
    if(EX_ListHead->Elements->yvalue->array->aRange)   /* argument is an integer range */  
      if(Oper == XMIN)                        /* minimum of a range is its lower bound */
        Result->yvalue->intval = *(EX_ListHead->Elements->yvalue->array->aValue.aint);
      else     /* Oper == XMAX : maximum of a range is its upper bound */
        Result->yvalue->intval = *(EX_ListHead->Elements->yvalue->array->aValue.aint) +
                                 EX_ListHead->Elements->yvalue->array->aSize;
    else {
      if(Oper == XMIN) {                              /* argument is an explicit array */
	for(Val = *(Act = EX_ListHead->Elements->yvalue->array->aValue.aint);
	    Act < EX_ListHead->Elements->yvalue->array->aValue.aint + 
	          EX_ListHead->Elements->yvalue->array->aSize; Act++ )
	  if(*Act < Val)
	    Val = *Act;
      }
      else                                                            /* Oper == XMAX */
	for(Val = *(Act = EX_ListHead->Elements->yvalue->array->aValue.aint);
	    Act < EX_ListHead->Elements->yvalue->array->aValue.aint +
	    EX_ListHead->Elements->yvalue->array->aSize; Act++ )
	  if(*Act > Val)
	    Val = *Act;
      Result->yvalue->intval = Val;
    }
  }
}





/*****************************************************************************************
 Function:                     EX_GetFloatArrayFunc
 Purpose: Evaluates 'min' and 'max' functions on float arrays. There must be exactly one 
          element of type ArrFloat in the expression list appended to EX_Listhead.
 Returns: void
 Side effects: Modifies Result.
****************************************************************************************/
void EX_GetFloatArrayFunc(Oper, Result)
     SymbolType Oper;                             /* operand must be either MIN or MAX */
     ParserStackType *Result;/* pointer to (unique?) expression value in argument list */
{
  register float *Act;                   /* pointer to actual element of integer array */
  register float Val;                           /* fast access to temporary max or min */
  
  if(EX_ListHead->Elements->ynext) {      /* further elements in list: list of arrays! */
    ER_StackError("type mismatch: %s function undefined on type %s\n", Serious,
                  STR_TypeString(Oper), STR_TypeString(EX_ListHead->Elements->ytype), nst);
    Result->ytype = Undef;
  }
  else {                                          /* single argument of type ArrFloat */
    Result->ytype = FLOATNUM;
    if(Oper == XMIN) {                                /* argument is an explicit array */
      for(Val = *(Act = EX_ListHead->Elements->yvalue->array->aValue.afloat);
	  Act < EX_ListHead->Elements->yvalue->array->aValue.afloat +
	        EX_ListHead->Elements->yvalue->array->aSize; Act++ )
	if(*Act < Val)
	  Val = *Act;
    }
    else   /* Oper == XMAX */
      for(Val = *(Act = EX_ListHead->Elements->yvalue->array->aValue.afloat);
	  Act < EX_ListHead->Elements->yvalue->array->aValue.afloat +
	  EX_ListHead->Elements->yvalue->array->aSize; Act++ )
	if(*Act > Val)
	  Val = *Act;
    Result->yvalue->floatval = Val;
  }
}





/*****************************************************************************************
 Function:                     EX_GetStringArrayFunc
 Purpose: Evaluates 'min' and 'max' functions on string arrays. There must be exactly one 
          element of type ArrString in the expression list appended to EX_Listhead.
 Returns: void
 Side effects: Modifies Result.
****************************************************************************************/
void EX_GetStringArrayFunc(Oper, Result)
     SymbolType Oper;                             /* operand must be either MIN or MAX */
     ParserStackType *Result;/* pointer to (unique?) expression value in argument list */
{
  register char  **Act;                  /* pointer to actual element of integer array */
  register char *Val;                           /* fast access to temporary max or min */
  
  if(EX_ListHead->Elements->ynext) {      /* further elements in list: list of arrays! */
    ER_StackError("type mismatch: %s function undefined on type %s\n", Serious,
                  STR_TypeString(Oper), STR_TypeString(EX_ListHead->Elements->ytype), nst);
    Result->ytype = Undef;
  }
  else {                                          /* single argument of type ArrString */
    Result->ytype = FLOATNUM;
    if(Oper == XMIN) {                                /* argument is an explicit array */
      for(Val = *(Act = EX_ListHead->Elements->yvalue->array->aValue.astring);
	  Act < EX_ListHead->Elements->yvalue->array->aValue.astring +
	        EX_ListHead->Elements->yvalue->array->aSize; Act++ )
	if(strcmp(*Act, Val) < 0)
	  Val = *Act;
    }
    else                                                               /* Oper == XMAX */
      for(Val = *(Act = EX_ListHead->Elements->yvalue->array->aValue.astring);
	  Act < EX_ListHead->Elements->yvalue->array->aValue.astring +
	  EX_ListHead->Elements->yvalue->array->aSize; Act++ )
	if(strcmp(*Act, Val) > 0)
	  Val = *Act;
    Result->yvalue->string = Val;
  }
}






/*****************************************************************************************
 Function:                     EX_ConcatList
 Purpose: Evaluate CONCAT function on an array or list of integers.
 Returns: Pointer to ParserStackType record holding resulting string.
*****************************************************************************************/
ParserStackType *EX_ConcatList()
{
  register ParserStackType *Result;         /* pointer to result record, of string type */
  register int Length;                                    /* length of resulting string */
  register ParserStackType *Act;                      /* pointer to actual list element */

  Result = PA_GetParserStack();                               /* allocate result record */
  if(EX_ListHead->Elements)                                      /* argument is correct */
    switch(EX_ListHead->Elements->ytype) {
    case SIMPLESTRING:
    case LONGSTRING:                       /* argument is a list of one or more strings */
      for(Length = 0, Act = EX_ListHead->Elements; Act; Act = Act->ynext)
	Length += strlen(Act->yvalue->string);         /* count length of concatenation */
      Result->yvalue->string = M_alloc((unsigned) (Length+1));/* allocate result string */
      for(*(Result->yvalue->string) = '\0', Act = EX_ListHead->Elements; Act;
	  Act = Act->ynext)                     /* concatenate strings of list elements */
	Result->yvalue->string = strcat(Result->yvalue->string, Act->yvalue->string);
      if(Length <= 1)
	Result->ytype = SIMPLESTRING;
      else     /* (Length > 1) */
	Result->ytype = LONGSTRING;
      break;
    case ArrString:     /* argument is an array of strings or a list of arrays (error) */
      if(! EX_ListHead->Elements->ynext) {
	EX_ConcatArray(Result);                     /* argument is an array of strings */
	break;
      }   /* else: argument = list of arrays of strings - continue with default case ! */
    default:    /* type error - cannot apply operator on other types or list of arrays */
      ER_StackError("type mismatch: 'concat' function undefined on type %s\n", Serious,
		    STR_TypeString(EX_ListHead->Elements->ytype), nst, nst);
      Result->ytype = Undef;
    }
  else
    Result->ytype = Undef;
  return Result;
}





/*****************************************************************************************
 Function:                     EX_ConcatArray
 Purpose: Evaluates CONCAT on string arrays. There must be exactly one 
          element of type ArrString in the expression list appended to EX_Listhead.
 Returns: void
 Side effects: Modifies Result.
****************************************************************************************/
void EX_ConcatArray(Result)
     ParserStackType *Result; /* pointer to (unique?) expression value in argument list */
{
  register char **Act;                    /* pointer to actual element of integer array */
  register int Length;                                       /* length of result string */
  
  /* there is exactly one element of type ArrString on the list stack EX_ListHead */
  for(Length = 0, Act = EX_ListHead->Elements->yvalue->array->aValue.astring;
      Act < EX_ListHead->Elements->yvalue->array->aValue.astring +
            EX_ListHead->Elements->yvalue->array->aSize; Act++ )
    Length += strlen(*Act);             /* count length of concatenation */
  Result->yvalue->string = M_alloc((unsigned) (Length+1));    /* allocate result string */
  for(Length = 0, Act = EX_ListHead->Elements->yvalue->array->aValue.astring;
      Act < EX_ListHead->Elements->yvalue->array->aValue.astring +
            EX_ListHead->Elements->yvalue->array->aSize; Act++ )/* concatenate array elements */
    Result->yvalue->string = strcat(Result->yvalue->string, *Act);
  if(Length <= 1)
    Result->ytype = SIMPLESTRING;
  else    /* (Length > 1) */
    Result->ytype = LONGSTRING;
}







/*****************************************************************************************
 Function:                     EX_EvalFloatOper
 Purpose: Evaluate '+', '-', '*', or '/'. Both operands are of type FLOATNUM.
 Returns: First Argument holding the result value.
 Side effects: Both operands are modified.
*****************************************************************************************/
ParserStackType *EX_EvalFloatOper(Op, A1, A2)
     SymbolType Op  ;            /* one of PLUS, MINUS, MULT, RDIV */
     ParserStackType *A1;        /* holds value of left operand, will hold result */
     ParserStackType *A2;        /* holds value of right operand */
{
  switch(Op) {
  case PLUS:
    A1->yvalue->floatval += A2->yvalue->floatval;
    break;
  case MINUS:
    A1->yvalue->floatval -= A2->yvalue->floatval;
    break;
  case MULT:
    A1->yvalue->floatval *= A2->yvalue->floatval;
    break;
  case RDIV:
    if(! A2->yvalue->floatval) {   /* attempt to divide by 0 */
      ER_StackError("math: division by 0\n", Serious, nst, nst, nst);
      A1->ytype = Undef;           /* don't ignore the division, supress further errors */
    }
    else       
      A1->yvalue->floatval /= A2->yvalue->floatval;
  }
  PA_FreeParserStack(A2);          /* remove right operand */
  return A1;                       /* left operand holds result and is returned */
}





/*****************************************************************************************
 Function:                     EX_EvalIntOper
 Purpose: Evaluate '+', '-', '*', or '/'. Both operands are of type INTEGER.
 Returns: First Argument holding the result value.
 Side effects: Both operands are modified.
*****************************************************************************************/
ParserStackType *EX_EvalIntOper(Op, A1, A2)
     SymbolType Op  ;            /* one of PLUS, MINUS, MULT, RDIV */
     ParserStackType *A1;        /* holds value of left operand, will hold result */
     ParserStackType *A2;        /* holds value of right operand */
{
  switch(Op) {
  case PLUS:
    A1->yvalue->intval += A2->yvalue->intval;
    break;
  case MINUS:
    A1->yvalue->intval -= A2->yvalue->intval;
    break;
  case MULT:
    A1->yvalue->intval *= A2->yvalue->intval;
    break;    
  default:
    if(! A2->yvalue->intval) {  /* attempt to divide by 0 */
      ER_StackError("math: division by 0\n", Serious, nst, nst, nst);
      A1->ytype = Undef;         /* don't ignore the division, supress further errors */
    }
    else
      switch(Op) {
      case RDIV:
      case DIV:                  /* RDIV and DIV are the same on integer operands */
	A1->yvalue->intval /= A2->yvalue->intval;
	break;
      case MOD:
	A1->yvalue->intval %= A2->yvalue->intval;
      }
  }
  PA_FreeParserStack(A2);          /* remove right operand */
  return A1;                       /* left operand holds result and is returned */
}






/*****************************************************************************************
 Function:                     EX_StringOper
 Purpose: Evaluate '/' or '\'. Both operands are strings.
 Returns: Result string.
*****************************************************************************************/
char *EX_StringOper(Op, S1, S2)
     SymbolType Op  ;           /* STRCAT or REMOVE */
     char *S1;                  /* left operand */
     char *S2;                  /* right operand */
{
  if(Op == XSTRCAT)
    return STR_Append(S1, S2);
  else {
     (void) printf("******bug: illegal operator %s in EX_StringOper\n", STR_TypeString(Op));
     return "**";
   }
}






/*****************************************************************************************
 Function:                     EX_InitExprList
 Purpose: Initialize stack of expressions (parser stack value for expression_list).
 Returns: void
 Side effects: Header of list EX_ListHead. 
*****************************************************************************************/
void EX_InitExprList(Elem)
     ParserStackType *Elem;        /* pointer to first expression parser stack value */
{
  if(!EX_ListHead)
    EX_ListHead = (ListValType *) M_alloc(sizeof(ListValType));
  if(Elem->ytype != Undef) {       /* expression has been evaluated correctly */
    EX_ListHead->LCount = 1;       /* element list has been empty before */
    EX_ListHead->Elements = Elem;  /* Elem becomes first list element */
  }
}






/*****************************************************************************************
 Function:                     EX_AppendExprList
 Purpose: Append an element to the stack of expressions (parser stack value for expression_list).
 Returns: void
 Side effects: Modifies header of list EX_ListHead. 
*****************************************************************************************/
void EX_AppendExprList(Elem)
     ParserStackType *Elem;       /* pointer to new expression parser stack value */
{
  if(EX_ListHead->LCount)          /* expression_list has correct value until now */
    if(Elem->ytype != Undef)       /* new expression has been evaluated correctly */
      if(Elem->ytype == EX_ListHead->Elements->ytype ||
	 Elem->ytype == SIMPLESTRING && EX_ListHead->Elements->ytype == LONGSTRING ||
	 EX_ListHead->Elements->ytype == SIMPLESTRING && Elem->ytype == LONGSTRING) {  
	/* compatible data types in element list */
	Elem->ynext = EX_ListHead->Elements;  /*** order of elements is reversed! */
	EX_ListHead->Elements = Elem;     /* new elements becomes first of list!! */
	EX_ListHead->LCount++;                       /* increase elements counter */
      }
      else {          /* error - array or list elements must be of same data type */
	ER_StackError("type mismatch: mixed types in expression list\n", Serious, nst,
		      nst, nst);
	PA_FreeParserStack(Elem);        /* cannot build element list */
	EX_RemoveList();
      }
    else {                               /* could not evaluate element expression */
      PA_FreeParserStack(Elem);          /* cannot build element list */
      EX_RemoveList();
    }
  else                      /* there has been a previous error in expression list */
    PA_FreeParserStack(Elem);
}





/*****************************************************************************************
 Function:                     EX_RemoveList
 Purpose: Remove list appended to EX_ListHead.
 Returns: void
 Side effects: Modifies  EX_ListHead.
*****************************************************************************************/
void EX_RemoveList()
{
  if(EX_ListHead->Elements)       /* remove list elements first */
    EX_RemoveListTail(EX_ListHead->Elements);
  EX_ListHead->LCount = 0;        /* list head ready for new expression list */
}





/*****************************************************************************************
 Function:                     EX_RemoveListTail
 Purpose: Remove list of ParserStackType records.
 Returns: void
*****************************************************************************************/
void EX_RemoveListTail(Elem)
     register ParserStackType *Elem;         /* points to head of (tail of) list */
{
  if(Elem) {
    if(Elem->ynext)
      EX_RemoveListTail(Elem->ynext);          /* remove tail of list */
    PA_FreeParserStack(Elem);                  /* remove head of list */
  }
}





/*****************************************************************************************
 Function:                     EX_MakeExplicitArray
 Purpose: Builds an array of values contained in an expression_list parser stack value.
          Does not deal with arrays defined by range.
 Returns: ParserStackType record holding the array structure, NULL if there is an error.
*****************************************************************************************/
ParserStackType *EX_MakeExplicitArray()
{
  register ParserStackType *Result;            /* will hold record with array structure */

  Result = PA_GetParserStack();                               /* allocate result record */
  if(EX_ListHead->LCount)                                          /* list is not empty */
    switch(EX_ListHead->Elements->ytype) {                      /* resultdata type is ? */
    case LONGSTRING:
    case SIMPLESTRING:
      Result->ytype = ArrString;         /* in arrays string types aren't distinguished */
      Result->yvalue->array = EX_CopyStringArray();
      break;
    case INTEGER:
      Result->ytype = ArrInt;    
      Result->yvalue->array = EX_CopyIntArray();                   /* array of integers */
      break;
    case FLOATNUM:
      Result->ytype = ArrFloat;    
      Result->yvalue->array = EX_CopyFloatArray();                   /* array of floats */
      break;
    case ArrStruct:
      Result->ytype = ArrArrStruct;    
      Result->yvalue->array = EX_CopyArrayArray(); /* array of arrays of structures */
    case ArrInt:
      if( ! ConstantSection) {         /* successors : FOREACH stmt. with explicit list */
	Result->ytype = ArrArrInt;    
	Result->yvalue->array = EX_CopyArrayArray();           /* build array of ArrInt */
      }
      else                           /* EX_ListHead->Elements holds array value already */
	return EX_ListHead->Elements;
      break;
    case ArrFloat:
      if( ! ConstantSection) {         /* successors : FOREACH stmt. with explicit list */
	Result->ytype = ArrArrFloat;    
	Result->yvalue->array = EX_CopyArrayArray();           /* build array of ArrInt */
      }
      else                           /* EX_ListHead->Elements holds array value already */
	return EX_ListHead->Elements;
      break;
    case ArrString:
      if( ! ConstantSection) {         /* successors : FOREACH stmt. with explicit list */
	Result->ytype = ArrArrString;    
	Result->yvalue->array = EX_CopyArrayArray();           /* build array of ArrInt */
      }
      else                           /* EX_ListHead->Elements holds array value already */
	return EX_ListHead->Elements;
      break;
    case STRUCTURE:
      Result->ytype = ArrStruct;    
      Result->yvalue->array = EX_CopyStructArray();              /* array of structures */
      break;
    case UNIT:
      Result->ytype = ArrUnit;    
      Result->yvalue->array = EX_CopyUnitArray();/*array of units (explicit definition) */
      break;
    default:
      (void) printf("******bug: illegal array type %s\n",
		    STR_TypeString(EX_ListHead->Elements->ytype));
      EX_RemoveList();
      Result->ytype = Undef;
    }
  else
    Result->ytype = Undef;
  return Result;
}





/*****************************************************************************************
 Function:                     EX_CopyIntArray
 Purpose: Copies list of integer type elements into an array. Order of list elements is reversed!
 Returns: Pointer to ArrayValueType record.
*****************************************************************************************/
ArrayValueType *EX_CopyIntArray()
{
  ArrayValueType *Result;                  /* pointer to result record */
  register ParserStackType *Val;           /* pointer to actually inserted list element */
  register int Pos;                        /* actual insertion position in array */

  Result = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));   /* build result record */
  Result->Named = FALSE;                                  /* array may be removed later */
  Result->aSize = EX_ListHead->LCount;          /* LCount >= 1, no null sized arrays !! */
  Result->aRange = FALSE;                  /* array is built explicitly */
  /* allocate a sufficiently long field for array */
  Result->aValue.aint = (int *) M_alloc((unsigned) (sizeof(int) * EX_ListHead->LCount));  
  /* copy values. First list value is inserted at the last array position - reverse order */
  for(Pos = EX_ListHead->LCount-1, Val = EX_ListHead->Elements; Val && Pos >= 0;
      Val = Val->ynext, Pos--)
    *(Result->aValue.aint+Pos) = Val->yvalue->intval;     /* copy list element into array field */
  if(Pos >= 0 || Val) {                            /* error in EX_AppendExprList?? */
    (void) printf("******bug: no. of elements in int array does not match\n");
    exit(ArraySizeMismatch);
  }
  EX_RemoveList();                                 /* list values are removed */
  return Result;
}





/*****************************************************************************************
 Function:                     EX_CopyFloatArray
 Purpose: Copies list of float type elements into an array. Order of list elements is reversed!
 Returns: Pointer to ArrayValueType record.
*****************************************************************************************/
ArrayValueType *EX_CopyFloatArray()
{
  ArrayValueType *Result;                  /* pointer to result record */
  register ParserStackType *Val;           /* pointer to actually inserted list element */
  register int Pos;                        /* actual insertion position in array */

  Result = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));   /* build result record */
  Result->Named = FALSE;                                  /* array may be removed later */
  Result->aSize = EX_ListHead->LCount;     /* LCount >= 1, no null sized arrays !! */
  Result->aRange = FALSE;                  /* array is built explicitly */
  /* allocate a sufficiently long field for array */
  Result->aValue.afloat = (float *) M_alloc((unsigned) (sizeof(float) * EX_ListHead->LCount));  
  /* copy values. First list value is inserted at the last array position - reverse order */
  for(Pos = EX_ListHead->LCount-1, Val = EX_ListHead->Elements; Val && Pos >= 0;
      Val = Val->ynext, Pos--)
    *(Result->aValue.afloat+Pos) = Val->yvalue->floatval;     /* copy list element into array field */
  if(Pos >= 0 || Val) {                            /* error in EX_AppendExprList?? */
    (void) printf("******bug: no. of elements in float array does not match\n");
    exit(ArraySizeMismatch);
  }
  EX_RemoveList();                                 /* list values are removed */
  return Result;
}





/*****************************************************************************************
 Function:                     EX_CopyStringArray
 Purpose: Copies list of # type elements into an array. Order of list elements is reversed!
 Returns: Pointer to ArrayValueType record.
*****************************************************************************************/
ArrayValueType *EX_CopyStringArray()
{
  ArrayValueType *Result;                  /* pointer to result record */
  register ParserStackType *Val;           /* pointer to actually inserted list element */
  register int Pos;                        /* actual insertion position in array */

  Result = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));   /* build result record */
  Result->Named = FALSE;                                  /* array may be removed later */
  Result->aSize = EX_ListHead->LCount;     /* LCount >= 1, no null sized arrays !! */
  Result->aRange = FALSE;                  /* array is built explicitly */
  /* allocate a sufficiently long field for array */
  Result->aValue.astring = (char **) M_alloc((unsigned) (sizeof(char*) * EX_ListHead->LCount));  
  /* copy values. First list value is inserted at the last array position - reverse order */
  for(Pos = EX_ListHead->LCount-1, Val = EX_ListHead->Elements; Val && Pos >= 0;
      Val = Val->ynext, Pos--)
    *(Result->aValue.astring+Pos) = Val->yvalue->string;     /* copy list element into array field */
  if(Pos >= 0 || Val) {                            /* error in EX_AppendExprList?? */
    (void) printf("******bug: no. of elements in string array does not match\n");
    exit(ArraySizeMismatch);
  }
  EX_RemoveList();                                 /* list values are removed */
  return Result;
}





/*****************************************************************************************
 Function:                     EX_CopyStructArray
 Purpose: Copies list of structures into an array. Order of list elements is reversed!
 Returns: Pointer to ArrayValueType record.
*****************************************************************************************/
ArrayValueType *EX_CopyStructArray()
{
  ArrayValueType *Result;                  /* pointer to result record */
  register ParserStackType *Val;           /* pointer to actually inserted list element */
  register int Pos;                        /* actual insertion position in array */

  Result = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));   /* build result record */
  Result->Named = FALSE;                                  /* array may be removed later */
  Result->aSize = EX_ListHead->LCount;          /* LCount >= 1, no null sized arrays !! */
  Result->aRange = FALSE;                                  /* array is built explicitly */
                                        /* allocate a sufficiently long field for array */
  Result->aValue.astruct = 
    (UnitValueType **) M_alloc((unsigned) (sizeof(UnitValueType *) * EX_ListHead->LCount));  
             /* First list value is inserted at the last array position - reverse order */
  for(Pos = EX_ListHead->LCount-1, Val = EX_ListHead->Elements; Val && Pos >= 0;
      Val = Val->ynext, Pos--)
    *(Result->aValue.astruct+Pos) = Val->yvalue->structure;        /* copy list element */
  if(Pos >= 0 || Val) {                                 /* error in EX_AppendExprList?? */
    (void) printf("******bug: no. of elements in int array does not match\n");
    exit(ArraySizeMismatch);
  }
  EX_RemoveList();                                           /* list values are removed */
  return Result;
}





/*****************************************************************************************
 Function:                     EX_CopyUnitArray
 Purpose: Copies list of explicitly defined units (FOREACH loop!) into an array. Order of 
          list elements is reversed!
 Returns: Pointer to ArrayValueType record.
*****************************************************************************************/
ArrayValueType *EX_CopyUnitArray()
{
  ArrayValueType *Result;                  /* pointer to result record */
  register ParserStackType *Val;           /* pointer to actually inserted list element */
  register int Pos;                        /* actual insertion position in array */

  Result = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));   /* build result record */
  Result->Named = FALSE;                                  /* array may be removed later */
  Result->aSize = EX_ListHead->LCount;          /* LCount >= 1, no null sized arrays !! */
  Result->aRange = FALSE;                                  /* array is built explicitly */
                                        /* allocate a sufficiently long field for array */
  Result->aValue.aunit = 
    (UnitType **) M_alloc((unsigned) (sizeof(UnitType *) * EX_ListHead->LCount));  
             /* First list value is inserted at the last array position - reverse order */
  for(Pos = EX_ListHead->LCount-1, Val = EX_ListHead->Elements; Val && Pos >= 0;
      Val = Val->ynext, Pos--)
    *(Result->aValue.aunit+Pos) = Val->yvalue->unit;               /* copy list element */
  if(Pos >= 0 || Val) {                                 /* error in EX_AppendExprList?? */
    (void) printf("******bug: no. of elements in int array does not match\n");
    exit(ArraySizeMismatch);
  }
  EX_RemoveList();                                           /* list values are removed */
  return Result;
}





/*****************************************************************************************
 Function:                     EX_CopyArrayArray
 Purpose: Copies list of explicitly defined array of 2-dimensional structures (arrays of
          structures) into an array. Order of list elements is reversed!
 Returns: Pointer to ArrayValueType record.
*****************************************************************************************/
ArrayValueType *EX_CopyArrayArray()
{
  ArrayValueType *Result;                  /* pointer to result record */
  register ParserStackType *Val;           /* pointer to actually inserted list element */
  register int Pos;                        /* actual insertion position in array */

  Result = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));   /* build result record */
  Result->Named = FALSE;                                  /* array may be removed later */
  Result->aSize = EX_ListHead->LCount;          /* LCount >= 1, no null sized arrays !! */
  Result->aRange = FALSE;                                  /* array is built explicitly */
                                        /* allocate a sufficiently long field for array */
  Result->aValue.aarray = (ArrayValueType **) 
    M_alloc((unsigned) (sizeof(ArrayValueType *) * EX_ListHead->LCount));  
             /* First list value is inserted at the last array position - reverse order */
  for(Pos = EX_ListHead->LCount-1, Val = EX_ListHead->Elements; Val && Pos >= 0;
      Val = Val->ynext, Pos--)
    *(Result->aValue.aarray+Pos) = Val->yvalue->array;         /* copy list element */
  if(Pos >= 0 || Val) {                                 /* error in EX_AppendExprList?? */
    (void) printf("******bug: no. of elements in int array does not match\n");
    exit(ArraySizeMismatch);
  }
  EX_RemoveList();                                           /* list values are removed */
  return Result;
}





/*****************************************************************************************
 Function:                     EX_MakeRangeArray
 Purpose: Builds an array of a range definition. This array is not built explicitly.
 Returns: Pointer to ParserStackType record holding the array value or NULL, if there has
          been an error in one of the two bound definition expressions.
*****************************************************************************************/
ParserStackType *EX_MakeRangeArray(Lower, Upper)
     ParserStackType *Lower;  /* parser stack value of expression to define lower bound */
     ParserStackType *Upper;  /* parser stack value of expression to define upper bound */
{
  ParserStackType *Result;    /* parser stack value of array */

  if(Lower->ytype != Undef && Upper->ytype != Undef)    /* both expressions are correct */
    if(Lower->ytype != Upper->ytype) {                  /* bounds must be of same type */
      if(Upper->ytype == LONGSTRING && Lower->ytype == SIMPLESTRING ||
	 Lower->ytype == LONGSTRING && Upper->ytype == SIMPLESTRING)
	ER_StackError("array error: string array bound must be of length 1\n", Serious,
		      nst, nst, nst);
      else  /* really distinct data types of lower and upper bounds */
	ER_StackError("type mismatch: array of types %s and %s\n", Serious,
		      STR_TypeString(Lower->ytype), STR_TypeString(Upper->ytype), nst);
      Result = NULL;         /* both errors inhibit the array construction */
    }
    else                    /* bounds defined and of same data type */
      switch(Lower->ytype) {
      case FLOATNUM:         /* float bounds are transformed into integers */
	ER_StackError("truncation: array bounds of type 'float', must be integers\n",
		      Warning, nst, nst, nst);
	Lower->yvalue->intval = (int) Lower->yvalue->floatval;
	Upper->yvalue->intval = (int) Upper->yvalue->floatval;
	/* type is not needed anymore - bounds are treated as integers (NO BREAK HERE!) */
      case INTEGER:
	Result = EX_BuildIntRange(Lower, Upper);      /* data types are ok, build array */
	break;
      case SIMPLESTRING:
	Result = EX_BuildStringRange(Lower, Upper);   /* data types are ok, build array */
	break;
      case LONGSTRING:                    /* if BOTH bounds are longer than 1 character */
	ER_StackError("array error: string array bound must be of length 1\n", Serious,
		      nst, nst, nst);
	Result = NULL;
	break;
      default:
	(void) printf("******bug: illegal types in range array\n");
	Result = NULL;
      }
  else                               /* at least one of the bouns could not be evaluated */
    Result = NULL;
  PA_FreeParserStack(Lower);         /* remove parser stack values for bound expressions */ 
  PA_FreeParserStack(Upper);
  return Result;
}





/*****************************************************************************************
 Function:                     EX_BuildIntRange
 Purpose: Build integer range array: Store the array length and its first value, rather
          than building it explicitly.
 Returns: Pointer to ParserStackType record holding array structure.
*****************************************************************************************/
ParserStackType *EX_BuildIntRange(Lower, Upper)
     ParserStackType *Lower;  /* parser stack value of expression to define lower bound */
     ParserStackType *Upper;  /* parser stack value of expression to define upper bound */
{
  ParserStackType *Result;                               /* parser stack value of array */

  if(Lower->yvalue->intval > Upper->yvalue->intval) {           /* illegal array bounds */
    ER_StackError("array error: lower bound > upper bound\n", Serious, nst, nst, nst);
    return NULL;              /* cannot evaluate array value - illegal null sized array */
  }
  else {
    Result = PA_GetParserStack();       /* allocate parser stack value record for array */
    Result->ytype = ArrInt;
    Result->yvalue->array = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
    Result->yvalue->array->Named = FALSE;                 /* array may be removed later */
    /* calculate length of integer array */
    Result->yvalue->array->aSize = Upper->yvalue->intval - Lower->yvalue->intval + 1;
    Result->yvalue->array->aRange = TRUE;  /* means that array is not stored explicitly */
    Result->yvalue->array->aValue.aint = (int *) M_alloc(sizeof(int));/* single element */
    *(Result->yvalue->array->aValue.aint) = Lower->yvalue->intval; /* store lower bound */
    return Result;
  }   /* do NOT remove Lower and Upper, since they will be removed in EX_MakeRangeArray */
}





/*****************************************************************************************
 Function:                     EX_BuildStringRange
 Purpose: Build an array out of a range definition. The array is stored explicitly.
 Returns: Pointer to parser stack value holfing the array.
*****************************************************************************************/
ParserStackType *EX_BuildStringRange(Lower, Upper)
     ParserStackType *Lower;  /* parser stack value of expression to define lower bound */
     ParserStackType *Upper;  /* parser stack value of expression to define upper bound */
{
  ParserStackType *Result;                               /* parser stack value of array */
  register short LowInd, UpInd;     /* index of lower (upper) bound in sorted char list */
  register short Act;                       /* index of currently generated array entry */

  for(LowInd = 0; LowInd < NoChars &&
      *(StringOrder[LowInd]) != *(Lower->yvalue->string); LowInd ++)
    ;                     /* look for index of lower bound in the sorted character list */
  if(LowInd >= NoChars) {            /* search exhausted character list - illegal bound */ 
    ER_StackError("array error: illegal symbol '%s' for lower bound\n", Serious,
		  STR_Save(Lower->yvalue->string), nst, nst);
    return NULL;
  }
  for(UpInd = 0; UpInd < NoChars &&                     /* do the same with upper bound */
      *(StringOrder[UpInd]) != *(Upper->yvalue->string); UpInd ++)
    ;                     /* look for index of upper bound in the sorted character list */
  if(UpInd >= NoChars) {            /* search exhausted character list - illegal bound */ 
    ER_StackError("array error: illegal symbol '%s' for upper bound\n", Serious,
		  STR_Save(Upper->yvalue->string), nst, nst);
    return NULL;
  }
  else if(LowInd > UpInd) {                                     /* illegal array bounds */
    ER_StackError("array error: lower bound > upper bound\n", Serious, nst, nst, nst);
    return NULL;              /* cannot evaluate array value - illegal null sized array */
  }
  else {                                         /*  bounds of string array are correct */
    Result = PA_GetParserStack();             /* allocate parser stack value for result */
    Result->ytype = ArrString;
    Result->yvalue->array = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
    Result->yvalue->array->Named = FALSE;                 /* array may be removed later */
    Result->yvalue->array->aSize = UpInd - LowInd + 1;               /* length of array */
    Result->yvalue->array->aRange = FALSE;            /* array will be built explicitly */
    Result->yvalue->array->aValue.astring =        /* allocate space for array elements */
      (char **) M_alloc((unsigned) (sizeof(char*) * Result->yvalue->array->aSize));
    for(Act=0; LowInd <= UpInd; LowInd++, Act++)                   /* copy array values */
      *(Result->yvalue->array->aValue.astring+Act) = StringOrder[LowInd];
    return Result;
  }
}





/*****************************************************************************************
 Function:                     EX_InitMap
 Purpose: Initialize a map description record with range and domain data types. Range and
          domain data types are ArrInt, ArrFloat, and ArrString rather than INTEGER,
	  FLOATNUM, LONGSTRING, or SHORTSTRING. The no. of lines of a map constant is
	  unknown initially. EX_MapConstant->mSize, ->mLines will be set by
	  EX_FinishUpMap, only.
 Returns: void.  
 Side effects: Modifies EX_MapConstant.
*****************************************************************************************/
void EX_InitMap(DType, RType)
     register ParserStackType *DType; /* domain: parser value stack record, simple type */
     register ParserStackType *RType;  /* range: parser value stack record, simple type */
{
    EX_MapConstant = (MapValueType *) M_alloc(sizeof(MapValueType));
    EX_MapConstant->DType = DType->ytype;                           /* domain data type */
    EX_MapConstant->RType = RType->ytype;                            /* range data type */
    PA_FreeParserStack(DType);
    PA_FreeParserStack(RType);
}





/*****************************************************************************************
 Function:                     EX_InsertMapLine
 Purpose: Append a new domain and a new range array to the list of map lines (EX_MapList).
          EX_ActMapLine points to the last element of this list. Therefore, the list will
	  become like
	  domain[0]-range[0]-domain[1]-range[1]- ... -domain[n]-range[n].
 Returns: void.  
 Side effects: Modifies EX_ActMapLine (increment) *(EX_ActMapLine) (insert).
*****************************************************************************************/
void EX_InsertMapLine(Domain, Range)
     ParserStackType *Domain;                                 /* array of domain values */
     ParserStackType *Range;                                   /* array of range values */
{
  EX_SetSimpleType(Domain);                /* tarnsform ARRAY OF ... into ... (->ytype) */
  EX_SetSimpleType(Range);                 /* tarnsform ARRAY OF ... into ... (->ytype) */
  if(EX_MapConstant) {                    /* until now, correct map constant definition */
    if(Domain->ytype == EX_MapConstant->DType && Range->ytype == EX_MapConstant->RType) {
      /* correct data types - chack if there are more lines than specified */
      if(EX_MapList) {                   /* there have been read other map lines before */
	EX_ActMapLine = (EX_ActMapLine->ynext = Domain);   /* append new domain to list */
	EX_ActMapLine = (EX_ActMapLine->ynext = Range);     /* append new range to list */
	EX_ActMapSize ++;          /* increment counter of map lines (domain AND range) */
      }
      else {                                        /* first map line - initialize list */
	EX_MapList = Domain;
	EX_ActMapLine = (EX_MapList->ynext = Range);  /* 2nd. entry is range of line */
	EX_ActMapSize = 1;                                            /* first map line */
      }
      return;
    }
    else if(Domain->ytype != EX_MapConstant->DType)
      ER_StackError("type mismatch: map domain type is %s must be %s\n", Serious,
		    STR_TypeString(Domain->ytype),STR_TypeString(EX_MapConstant->DType),nst);
    else if(Range->ytype != EX_MapConstant->RType)
      ER_StackError("type mismatch: map range type is %s must be %s\n", Serious,
		    STR_TypeString(Range->ytype),STR_TypeString(EX_MapConstant->RType),nst);
    /* there has been an error - remove previously inserted lines */
    if(EX_MapList)
      EX_RemoveListTail(EX_MapList);
    EX_MapList = NULL;
    EX_ActMapSize = 0;
    M_free((char *) EX_MapConstant);
    EX_MapConstant = NULL;      /* set pointer to NULL to avoid further line insertion */ 
    EX_ActMapLine = NULL;
  }
  PA_FreeParserStack(Domain);  /* parser stack value records are not used anymore */
  PA_FreeParserStack(Range);
  return;
}





/*****************************************************************************************
 Function:                    EX_SetSimpleType 
 Purpose: Transform array type into simple type. Needed to insert arrays into map constant.
 Returns: void.
 Side effects: Modifies <argument>->ytype.
*****************************************************************************************/
void EX_SetSimpleType(Array)
     ParserStackType *Array;
{
  switch(Array->ytype) {
  case ArrInt:
    Array->ytype = XINT;
    break;
  case ArrFloat:
    Array->ytype = XFLOAT;
    break;
  case ArrString:
    Array->ytype = STRING;
    break;
  default:
    (void) printf("******bug: type in EX_SetSimpleType is %s\n", STR_TypeString(Array->ytype));
    Array->ytype = Undef;
  }
}





/*****************************************************************************************
 Function:                     EX_FinishUpMap
 Purpose: Build a ParserValueType record holding the evaluated map constant. Copy the
          domain and range values of EX_MapList into MapValueType->mLines and remove the
	  list. 
 Returns: Pointer to ParserValueType record holding the evaluated map constant.
          NULL, if there has been an error in the map definition (EX_MapConstant == NULL).
 Side effects: EX_MapConstant, EX_ActMapLine are set to NULL.
*****************************************************************************************/
ParserStackType *EX_FinishUpMap()
{
  register MapLineType *Line;                        /* points to current insertion line */
  register ParserStackType *ActElem;     /* points to currently inserted domain or range */
  register ParserStackType *Result;               /* points to result parser stack value */

  if(EX_MapConstant) {                          /* constant has been evaluated correctly */
    Result = PA_GetParserStack();                /* allocate record to hold result value */
    Result->ytype = MAP;
    EX_MapConstant->mSize = EX_ActMapSize;               /* copy no. of definition lines */
    EX_MapConstant->mLines = (MapLineType*)
      M_alloc( (unsigned) EX_ActMapSize * sizeof(MapLineType));  /* allocate value field */
    EX_ActMapSize = 0;                                             /* reset line counter */
    Line = EX_MapConstant->mLines;        /* initialize ptr. to insert position of lines */
    ActElem = EX_MapList;          /* initialize var. to scan list of domains and ranges */
    while(ActElem) {           /* copy each pair (domain, range) of the list into result */
      Line->Domain = ActElem->yvalue->array;                  /* first element is domain */
      ActElem = ActElem->ynext;           /* do NOT increment Line - one pair ~ one line */
      Line->Range = ActElem->yvalue->array;                     /* nest element is range */
      ActElem = ActElem->ynext;                              /* now, increment Line, too */
      Line ++;
    }                 /* list of map lines not needed anymore - values copied into array */
    EX_RemoveListTail(EX_MapList);              /* remove list of map line parser values */
    EX_MapList = NULL;                                 /* !! re-initialize EX_MapList !! */
    Result->yvalue->map = EX_MapConstant;          /* copy map constant to result record */
    EX_MapConstant = NULL;                         /* !! re-initialize EX_MapConstant !! */
    EX_ActMapLine = NULL;
#ifdef MAPTEST
    ST_PrintMap(Result);
#endif
    return Result;
  }
  else  /* there has been an error - could not evaluate map constant definition */
    return NULL;
}






/*****************************************************************************************
 Function:                     EX_EvalFileMap
 Purpose: Evaluate a map read from a file.  Return the maps value in a parser value stack 
          record.
 Returns: &ParserStackType record. 
*****************************************************************************************/
ParserStackType *EX_EvalFileMap(FPtr, Sep1, Sep2)
     ParserStackType *FPtr;                   /* parser value stack record for map file */
     ParserStackType *Sep1;     /* parser value stack record for domain/range separator */
     ParserStackType *Sep2;     /* parser value stack record for separator of map lines */
{
  ParserStackType *Res;
  Res = PA_GetParserStack();
  if(EX_MapConstant)
    if(FI_ReadMap(EX_MapConstant, FPtr, Sep1, Sep2)) {     /* map values read correctly */
      Res->ytype = MAP;
      Res->yvalue->map = EX_MapConstant;
    }
    else {
      if(FPtr && FPtr->ytype == FILEVAL && FPtr->yvalue->fileval->FilePtr)
	FI_CloseFile(FPtr);
      PA_FreeParserStack(FPtr);
      Res->ytype = Undef;
    }
  else {
    PA_FreeParserStack(Sep1);
    PA_FreeParserStack(Sep2);
    if(FPtr && FPtr->ytype == FILEVAL && FPtr->yvalue->fileval->FilePtr)
      FI_CloseFile(FPtr);
    PA_FreeParserStack(FPtr);
    Res->ytype = Undef;
  }
  EX_MapConstant = NULL;
#ifdef MAPTEST
    ST_PrintMap(Res);
#endif

  return Res;
}
  
  



/*****************************************************************************************
 Function:                     EX_CorrectSize
 Purpose: Checks for an expression if it defines a correct map or structure size.
          The expression value must be of type integer and >= 0.
 Returns: Argument (pointer to ParserStackType).
 Side effects: If there is an uncorrectable error, Size->ytype is set to Undef to avoid
               further error messages.
*****************************************************************************************/
ParserStackType *EX_CorrectSize(Size)
     register ParserStackType *Size;/* pointer to parser stack value holding expression */
{
  if(Size->ytype == FLOATNUM) {              /* correctable error: size must be integer */
    ER_StackError("truncation: size of map constant must be integer\n", Warning, nst, nst,
		  nst);
    Size->yvalue->intval = (int) Size->yvalue->floatval;   /* truncate float to integer */
    Size->ytype = INTEGER;
  }
  if(Size->ytype != INTEGER) {                 /* type error: size must be integer */
    ER_StackError("type mismatch: illegal type for map or structure size\n", Serious,
		  nst, nst, nst);
    Size->ytype = Undef;
  }
  else if(Size->ytype == INTEGER)                            /* size is of correct data type */
    if(Size->yvalue->intval < 0) {
    ER_StackError("size or index of array must be nonnegative\n", Serious, nst, nst, nst);
    Size->ytype = Undef;
  }
  return Size;
}



  



/*****************************************************************************************
 Function:                     EX_GetFieldElem
 Purpose: Gets an array element specified by an expression like xx[yy].
 Returns: ParserStackType record holding element value.
*****************************************************************************************/
ParserStackType *EX_GetFieldElem(Field,Index)
     ParserStackType *Field;       /* points to array description on parser value stack */
     ParserStackType *Index;             /* points to index value on parser value stack */
{
  if(Index->ytype == FLOATNUM) {      /* type error: index should be integer - truncate */
    ER_StackError("truncation: array index of type 'float', must be 'integer'\n",
		  Warning, nst, nst, nst);
    Index->yvalue->intval = (int) Index->yvalue->floatval;    /* transform into integer */
    Index->ytype = INTEGER;
  }
  else if(Index->ytype != INTEGER) {             /* type error: illegal index data type */
    if(Index->ytype !=Undef)            /* supress multiple messages for the same error */
      ER_StackError("type mismatch: array index of type %s, must be 'integer'\n",
		    Serious, STR_TypeString(Index->ytype), nst, nst);
    Field->ytype = Undef;        /* error cannot be corrected - array remains undefined */
  }   /* check if first value isan array */
  if(Field->ytype != Undef && Field->ytype != ArrInt && Field->ytype != ArrFloat &&
     Field->ytype != ArrString && Field->ytype != STRUCTURE) {    /* illegal, not array */
    ER_StackError("type mismatch: type %s, array type expected\n", Serious,
		  STR_TypeString(Field->ytype), nst, nst);
    Field->ytype = Undef;        /* error cannot be corrected - array remains undefined */
  }
  if(Field->ytype != Undef)     /* until now, data types of field and index are correct */
    if(! Index->yvalue->intval < Field->yvalue->array->aSize) {  /* index out of bounds */
      ER_StackError("array error: index %s out of bounds\n", Serious,
		    STR_ItoA(Index->yvalue->intval), nst, nst);
      Field->ytype = Undef;      /* error cannot be corrected - array remains undefined */
    }
  if(Field->ytype == Undef) {
    PA_FreeParserStack(Index);     /* there has been some error */
    return Field;
  }
  else                             /* correct element selection */
    return EX_SelectFieldElem(Field, Index);
}





/*****************************************************************************************
 Function:                     EX_SelectFieldElem
 Purpose: Return an array element. 
 Returns: ParserStack type, holdign an array element.
*****************************************************************************************/
ParserStackType *EX_SelectFieldElem(Field,Index)
     ParserStackType *Field;       /* points to array description on parser value stack */
     ParserStackType *Index;             /* points to index value on parser value stack */
{
  register ParserStackType *Result;
  Result = PA_GetParserStack();        /* allocate parser value stack record for result */
  switch(Field->ytype) {
  case STRUCTURE:          /* selection of a unit */
    (void) printf("***--- selection of units still not implemented\n");
    Result->ytype = Undef;
    break;
  case ArrInt:             /* array of integers */
    Result->ytype = INTEGER;      /* array of integers can contain explicit elements or */
    if(Field->yvalue->array->aRange)       /* only the array size and its first element */
      Result->yvalue->intval = *(Field->yvalue->array->aValue.aint) + Index->yvalue->intval;
    else                                 /* array is an explicit list of integer values */
      Result->yvalue->intval = *(Field->yvalue->array->aValue.aint + Index->yvalue->intval);
    break;
  case ArrFloat:           /* array of floats */
    Result->ytype = FLOATNUM;  
    Result->yvalue->floatval = *(Field->yvalue->array->aValue.afloat + Index->yvalue->intval);
    break;
  case ArrString:          /* array of strings */
    Result->yvalue->string = *(Field->yvalue->array->aValue.astring + Index->yvalue->intval);
    if(strlen(Result->yvalue->string) <= 1)
      Result->ytype = SIMPLESTRING;
    else
      Result->ytype = SIMPLESTRING;  
    break;
  default:
    (void)  printf("******bug: illegal array type %s in EX_SelectFieldElem\n",
		   STR_TypeString(Field->ytype));
    Result->ytype = Undef;
  }
  PA_FreeParserStack(Field);
  PA_FreeParserStack(Index);
  return Result;
}





/*****************************************************************************************
 Function:                     EX_ArctanFunc
 Returns: ParserStackType element with function result.
*****************************************************************************************/
ParserStackType *EX_ArctanFunc(Arg)
     ParserStackType *Arg;              /* pointer to argument value parser stack value */
{
  if(Arg->ytype != Undef)                                /* previous errors are ignored */
    if(Arg->ytype == INTEGER) {
      Arg->yvalue->floatval = (float) atan((double) Arg->yvalue->intval);
      Arg->ytype = FLOATNUM;
    }                              /* previous errors are ignored */
    else if(Arg->ytype == FLOATNUM)
      Arg->yvalue->floatval = (float) atan(Arg->yvalue->floatval);
    else {
      ER_StackError("type mismatch: argument of 'arctan' is of type %s\n", Serious,
		    STR_TypeString(Arg->ytype), nst, nst);
      Arg->ytype = Undef;
    }
  return Arg;
}





/*****************************************************************************************
 Function:                     EX_CosFunc
 Returns: ParserStackType element with function result.
*****************************************************************************************/
ParserStackType *EX_CosFunc(Arg)
     ParserStackType *Arg;              /* pointer to argument value parser stack value */
{
  if(Arg->ytype != Undef)                                /* previous errors are ignored */
    if(Arg->ytype == INTEGER) {
      Arg->yvalue->floatval = (float) cos((double) Arg->yvalue->intval);
      Arg->ytype = FLOATNUM;
    }                              /* previous errors are ignored */
    else if(Arg->ytype == FLOATNUM)
      Arg->yvalue->floatval = (float) cos(Arg->yvalue->floatval);
    else {
      ER_StackError("type mismatch: argument of 'cos' is of type %s\n", Serious,
		    STR_TypeString(Arg->ytype), nst, nst);
      Arg->ytype = Undef;
    }
  return Arg;
}





/*****************************************************************************************
 Function:                     EX_SinFunc
 Returns: ParserStackType element with function result.
*****************************************************************************************/
ParserStackType *EX_SinFunc(Arg)
     ParserStackType *Arg;              /* pointer to argument value parser stack value */
{
  if(Arg->ytype != Undef)                                /* previous errors are ignored */
    if(Arg->ytype == INTEGER) {
      Arg->yvalue->floatval = (float) sin((double) Arg->yvalue->intval);
      Arg->ytype = FLOATNUM;
    }                              /* previous errors are ignored */
    else if(Arg->ytype == FLOATNUM)
      Arg->yvalue->floatval = (float) sin(Arg->yvalue->floatval);
    else {
      ER_StackError("type mismatch: argument of 'sin' is of type %s\n", Serious,
		    STR_TypeString(Arg->ytype), nst, nst);
      Arg->ytype = Undef;
    }
  return Arg;
}





/*****************************************************************************************
 Function:                     EX_ExpFunc
 Returns: ParserStackType element with function result.
*****************************************************************************************/
ParserStackType *EX_ExpFunc(Arg)
     ParserStackType *Arg;              /* pointer to argument value parser stack value */
{
  if(Arg->ytype != Undef)                                /* previous errors are ignored */
    if(Arg->ytype == INTEGER) {
      Arg->yvalue->floatval = (float) exp((double) Arg->yvalue->intval);
      Arg->ytype = FLOATNUM;
    }                              /* previous errors are ignored */
    else if(Arg->ytype == FLOATNUM)
      Arg->yvalue->floatval = (float) exp(Arg->yvalue->floatval);
    else {
      ER_StackError("type mismatch: argument of 'exp' is of type %s\n", Serious,
		    STR_TypeString(Arg->ytype), nst, nst);
      Arg->ytype = Undef;
    }
  return Arg;
}





/*****************************************************************************************
 Function:                     EX_LnFunc
 Returns: ParserStackType element with function result.
*****************************************************************************************/
ParserStackType *EX_LnFunc(Arg)
     ParserStackType *Arg;              /* pointer to argument value parser stack value */
{
  if(Arg->ytype != Undef)                                /* previous errors are ignored */
    if(Arg->ytype == INTEGER) {
      if(Arg->yvalue->intval <= 0) {                    /* argument must not be negative */
	ER_StackError("math: argument of 'ln' <= 0\n", Serious, nst, nst, nst);
	Arg->ytype = Undef;
      }
      else {
	Arg->yvalue->floatval = (float) log((double) Arg->yvalue->intval);
	Arg->ytype = FLOATNUM;
      }
    }                              /* previous errors are ignored */
    else if(Arg->ytype == FLOATNUM)
      if(Arg->yvalue->floatval <= 0.0) {                /* argument must not be negative */
	ER_StackError("math: argument of 'ln' <= 0\n", Serious, nst, nst, nst);
	Arg->ytype = Undef;
      }
      else
	Arg->yvalue->floatval = (float) log(Arg->yvalue->floatval);
    else {
      ER_StackError("type mismatch: argument of 'ln' is of type %s\n", Serious,
		    STR_TypeString(Arg->ytype), nst, nst);
      Arg->ytype = Undef;
    }
  return Arg;
}





/*****************************************************************************************
 Function:                     EX_SqrFunc
 Returns: ParserStackType element with function result.
*****************************************************************************************/
ParserStackType *EX_SqrFunc(Arg)
     ParserStackType *Arg;              /* pointer to argument value parser stack value */
{
  if(Arg->ytype != Undef)                                /* previous errors are ignored */
    if(Arg->ytype == INTEGER)
      Arg->yvalue->intval *= Arg->yvalue->intval;
    else if(Arg->ytype == FLOATNUM)
      Arg->yvalue->floatval *= Arg->yvalue->floatval;
    else {
      ER_StackError("type mismatch: argument of 'sqrt' is of type %s\n", Serious,
		    STR_TypeString(Arg->ytype), nst, nst);
      Arg->ytype = Undef;
    }
  return Arg;
}





/*****************************************************************************************
 Function:                     EX_SqrtFunc
 Returns: ParserStackType element with function result.
*****************************************************************************************/
ParserStackType *EX_SqrtFunc(Arg)
     ParserStackType *Arg;              /* pointer to argument value parser stack value */
{
  if(Arg->ytype != Undef)                                /* previous errors are ignored */
    if(Arg->ytype == INTEGER) {
      if(Arg->yvalue->intval < 0) {                    /* argument must not be negative */
	ER_StackError("math: negative argument of 'sqrt'\n", Serious, nst, nst, nst);
	Arg->ytype = Undef;
      }
      else {
	Arg->yvalue->floatval = (float) Arg->yvalue->intval;
	Arg->yvalue->floatval = (float) sqrt((double) Arg->yvalue->floatval);
	Arg->ytype = FLOATNUM;
      }
    }                              /* previous errors are ignored */
    else if(Arg->ytype == FLOATNUM)
      if(Arg->yvalue->floatval < 0.0) {                /* argument must not be negative */
	ER_StackError("math: negative argument of 'sqrt'\n", Serious, nst, nst, nst);
	Arg->ytype = Undef;
      }
      else
	Arg->yvalue->floatval = (float) sqrt(Arg->yvalue->floatval);
    else {
      ER_StackError("type mismatch: argument of 'sqrt' is of type %s\n", Serious,
		    STR_TypeString(Arg->ytype), nst, nst);
      Arg->ytype = Undef;
    }
  return Arg;
}





/*****************************************************************************************
 Function:                     EX_RandomFunc
 Purpose: Return random float value in [-1.0, 1.0].
 Returns: ParserStackType holding random value.
*****************************************************************************************/
ParserStackType *EX_RandomFunc()
{
  ParserStackType *Pt;

  Pt = PA_GetParserStack();
  Pt->ytype = FLOATNUM;
  Pt->yvalue->floatval = GETRANDOM;
  return Pt;
}
  
