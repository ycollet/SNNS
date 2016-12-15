/*****************************************************************************
  FILE              : co_logical.c
  SHORTNAME         : logical
  VERSION           : 2.0

  PURPOSE           : contains functions to evaluaate expressions with result of type boolean.
                            - LOG_EvalLogicalExpr            - LOG_EvalIntInclusion   
			    - LOG_EvalLogicalNot             - LOG_EvalFloatInclusion 
			    - LOG_EvalRelation               - LOG_EvalStringInclusion
			    - LOG_EvalInclusion              - LOG_CompareIntegers
			    - LOG_EvalMapRelation            - LOG_CompareFloats
			    - LOG_MapError                   - LOG_CompareStrings
			    - LOG_EvalSimpleInclusion 
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/


#include "co_logical.h"



/*****************************************************************************************
 Function:                     LOG_EvalLogicalExpr
 Purpose: Evaluates a logical expression. Both operands must be of type "Boolean".
          Operator must be AND or OR.
 Returns: Pointer to ParserStackType record of type "Boolean", holding TRUE or FALSE.
 Side effects: Both operands are removed.
*****************************************************************************************/
ParserStackType *LOG_EvalLogicalExpr(Operator, Left, Right)
     SymbolType Operator;      /* holds AND or OR - specifies operation to be performed */
     ParserStackType *Left;        /* points to ParserStackType record for left operand */
     ParserStackType *Right;      /* points to ParserStackType record for right operand */
{
  if(Left->ytype != Undef && Right->ytype != Undef)               /* no previous errors */
    if(Left->ytype != Boolean ||  Right->ytype != Boolean) {              /* type error */
      ER_StackError("type mismatch: operand of %s must be of type boolean\n", Serious,
		    STR_TypeString(Operator), nst, nst);
      Left->ytype = Undef;
    }      
    else                            /* correct data types - evaluate logical expression */
      if(Operator == AND)
	Left->yvalue->intval = (Left->yvalue->intval && Right->yvalue->intval);
      else   /* Operator == OR */
	Left->yvalue->intval = (Left->yvalue->intval || Right->yvalue->intval);
  else
    Left->ytype = Undef;
  PA_FreeParserStack(Right);
  return Left;
}




/*****************************************************************************************
 Function:                     LOG_EvalLogicalNot
 Purpose: Evaluates  logical negation. Operand must be of type "Boolean".
 Returns: Pointer to ParserStackType record of type "Boolean", holding TRUE or FALSE.
 Side effects: Operand is removed.
*****************************************************************************************/
ParserStackType *LOG_EvalLogicalNot(Operand)
     ParserStackType *Operand;         /* points to ParserStackType record left operand */
{
  if(Operand->ytype != Undef)                                     /* no previous errors */
    if(Operand->ytype != Boolean) {                                       /* type error */
      ER_StackError("type mismatch: operand of 'NOT' must be of type boolean\n", Serious,
		    nst, nst, nst);
      Operand->ytype = Undef;
    }      
    else                             /* correct data type - evaluate logical expression */
      Operand->yvalue->intval = !(Operand->yvalue->intval);
  else
    Operand->ytype = Undef;
  return Operand;
}





/*****************************************************************************************
 Function:                     LOG_EvalRelation
 Purpose: Evaluates a relational expression. Both operands must be of same data type.
 Returns: Pointer to ParserStackType record of type "Boolean", holding TRUE or FALSE.
 Side effects: Both operands are removed.
*****************************************************************************************/
ParserStackType *LOG_EvalRelation(Operator, Left, Right)
     SymbolType Operator;                         /* specifies relation to be evaluated */
     ParserStackType *Left;        /* points to ParserStackType record for left operand */
     ParserStackType *Right;      /* points to ParserStackType record for right operand */
{
  if(Left->ytype != Undef && Right->ytype != Undef)               /* no previous errors */
    if(Left->ytype != Right->ytype &&
       ! (Left->ytype==SIMPLESTRING && Right->ytype==LONGSTRING ||
	  Left->ytype==LONGSTRING && Right->ytype==SIMPLESTRING))
      /* error - operands must be of same data type */
      ER_StackError("type mismatch: arguments of %s must be of same type\n", Serious,
		    STR_TypeString(Operator), nst, nst);
    else if(Left->ytype == INTEGER)
      return LOG_CompareIntegers(Operator, Left, Right);        /* relation of integers */
    else if(Left->ytype == FLOATNUM)
      return LOG_CompareFloats(Operator, Left, Right);            /* relation of floats */
    else if(Left->ytype == STRING || Left->ytype == SIMPLESTRING || Left->ytype == LONGSTRING) 
      return LOG_CompareStrings(Operator, Left, Right);          /* relation of strings */
    else                     /* error - data type of operands must be INTEGER or STRING */
      ER_StackError("type mismatch: arguments of %s must be of same type\n", Serious,
		    STR_TypeString(Operator), nst, nst);
  /* there has been a type error - return undefined value, remove arguments */
  Left->ytype = Undef;
  PA_FreeParserStack(Right);
  return Left;
}





/*****************************************************************************************
 Function:                     LOG_EvalInclusion
 Purpose: Evaluates a relational expression. Both operands must be of same data type.
 Returns: Pointer to ParserStackType record of type "Boolean", holding TRUE or FALSE.
 Side effects: Both operands are removed.
*****************************************************************************************/
ParserStackType *LOG_EvalInclusion(Elem, Field)
     ParserStackType *Elem;             /* points to ParserStackType record for element */
     ParserStackType *Field;              /* points to ParserStackType record for array */
{
  ParserStackType *Temp;

  STMT_GetIdValue(Field);
  if(Elem->ytype != Undef && Field->ytype != Undef) {
    Temp = STMT_BuildArrayType(Elem);             /* transforms INTEGER => ArrInt, etc. */
    if(Temp->ytype != Field->ytype) {
      ER_StackError("type mismatch: %s IN %s \n", Serious, STR_TypeString(Elem->ytype),
		    STR_TypeString(Field->ytype), nst);
      Temp->ytype = Undef;
    }
    else {
      Temp->ytype = Boolean;                   /* result value will be returned in Temp */
      Temp->yvalue->intval = LOG_EvalSimpleInclusion(Field->yvalue->array, Elem);
      if(Temp->yvalue->intval == Undef)       /* error found in LOG_EvalSimpleInclusion */
	Temp->ytype = Undef;
    }
    PA_FreeParserStack(Elem);
    PA_FreeParserStack(Field);
    return Temp;                          /* return correct value of boolean expression */
  }
  else {                                                         /* some previous error */
    PA_FreeParserStack(Elem);
    Field->ytype = Undef;
    return Field;
  }
}





/*****************************************************************************************
 Function:                     LOG_EvalMapRelation
 Purpose: Evaluates a map relation.
 Returns: Pointer to ParserStackType record of type "Boolean", holding TRUE or FALSE.
 Side effects: All arguments are removed.
*****************************************************************************************/
ParserStackType *LOG_EvalMapRelation(DomVal, RangeVal, Map)
     ParserStackType *DomVal;                   /* domain value of pair (domain, range) */
     ParserStackType *RangeVal;                  /* range value of pair (domain, range) */
     ParserStackType *Map;                                              /* map constant */
{
  register MapLineType *First;        /* pointer to first line of map or range of lines */
  register MapLineType *Last;          /* pointer to last line of map or range of lines */
  register MapLineType *Line;                                         /* scan map lines */
  SymbolType  DType, RType;                 /* data type of map domain and range values */

  STMT_GetIdValue(Map);
  if(Map->ytype == RANGE) {         /* already checked: range is not out of  map bounds */
    DType = Map->yvalue->range->FieldPtr->Value->map->DType;        /* hide an eventual */
    RType = Map->yvalue->range->FieldPtr->Value->map->RType;         /* range definition */
    First = Map->yvalue->range->FieldPtr->Value->map->mLines +
      Map->yvalue->range->Lower;
    Last = Map->yvalue->range->FieldPtr->Value->map->mLines + 
      Map->yvalue->range->Upper;
  }
  else if(Map->ytype == MAP) {
    DType = Map->yvalue->map->DType;
    RType = Map->yvalue->map->RType;
    First = Map->yvalue->map->mLines;
    Last = Map->yvalue->map->mLines + Map->yvalue->map->mSize - 1;
  }
  else {
    if(Map->ytype !=  Undef)
      ER_StackError("type mismatch: map constant or range of map lines expected\n",
		    Serious, nst, nst, nst);
      return LOG_MapError(DomVal, RangeVal, Map);
  }
  if(DomVal->ytype != Undef && RangeVal->ytype != Undef) 
    if(DomVal->ytype != DType && 
       ! ((DomVal->ytype == LONGSTRING ||  DomVal->ytype == SIMPLESTRING) && DType == STRING)) {
      ER_StackError("type mismatch: map domain type <> element type\n", Serious, nst, nst, nst);
      return LOG_MapError(DomVal, RangeVal, Map);
    }
    else if(RangeVal->ytype != RType &&
       ! ((RangeVal->ytype == LONGSTRING ||  RangeVal->ytype == SIMPLESTRING) && RType == STRING)) {
      ER_StackError("type mismatch: map range type <> element type\n", Serious, nst,
		    nst, nst);
      return LOG_MapError(DomVal, RangeVal, Map);
    }
    else {                   /* data types are ok - scan map lines and look for entries */
      for(Line = First; Line <=  Last; Line ++)
	if(LOG_EvalSimpleInclusion(Line->Domain, DomVal))
	  if(LOG_EvalSimpleInclusion(Line->Range, RangeVal)) {
	    /* element pair is contained in map constant - return TRUE */
	    DomVal->ytype = Boolean;
	    DomVal->yvalue->intval = TRUE;
	    PA_FreeParserStack(RangeVal);
	    PA_FreeParserStack(Map);
	    return DomVal;
	  }
      DomVal->ytype = Boolean;
      DomVal->yvalue->intval = FALSE;
      PA_FreeParserStack(RangeVal);
      PA_FreeParserStack(Map);
      return DomVal;
    }
  else                    /* there have been errors in definition of DomVal or RangeVal */
      return LOG_MapError(DomVal, RangeVal, Map);
}






/*****************************************************************************************
 Function:                     LOG_MapError
 Purpose: Removes argument records and returns Undef.
 Returns: Pointer to ParserStackType record of type "Undef".
 Side effects: All arguments are removed.
*****************************************************************************************/
ParserStackType *LOG_MapError(DomVal, RangeVal, Map)
     ParserStackType *DomVal;                   /* domain value of pair (domain, range) */
     ParserStackType *RangeVal;                  /* range value of pair (domain, range) */
     ParserStackType *Map;                                              /* map constant */
{
  DomVal->ytype = Undef;
  PA_FreeParserStack(RangeVal);
  PA_FreeParserStack(Map);
  return DomVal;
}
 









/*****************************************************************************************
 Function:                     LOG_EvalSimpleInclusion
 Purpose: Evaluate inclusion relation for an integer and an array of integers.
 Returns: TRUE <==> element In array.
 Special assumptions: Elem is of Field's basis type.
*****************************************************************************************/
short LOG_EvalSimpleInclusion(Field, Elem)
     ArrayValueType *Field;
     ParserStackType *Elem;
{
  switch(Elem->ytype) {
  case INTEGER:                                /* integer element and array of integers */
    return LOG_EvalIntInclusion(Field, Elem);
  case FLOATNUM:                                  /* float element and array of floats */
    return LOG_EvalFloatInclusion(Field, Elem);
  case SIMPLESTRING:                               /* string element and array of strings */
  case LONGSTRING:
    return LOG_EvalStringInclusion(Field, Elem);
  case UNIT:
  case STRUCTURE:
    ER_StackError("type mismatch: cannot apply IN on array of units or structures\n",
		  Serious, nst, nst, nst);
    return Undef;
  default:
    (void) printf("******bug: illegal element type %s in LOG_EvalInclusion\n",
		  STR_TypeString(Elem->ytype));
    return Undef;
  }
}




/*****************************************************************************************
 Function:                     LOG_EvalIntInclusion
 Purpose: Evaluate inclusion relation for an integer and an array of integers.
 Returns: TRUE <==> element In array.
*****************************************************************************************/
short LOG_EvalIntInclusion(Field, Elem)
     ArrayValueType *Field;
     ParserStackType *Elem;
{
  register int *ActInt;                                    /* to scan array of integers */

  if(Field->aRange)             /* integer array specified through range */
    if(Elem->yvalue->intval >= *(Field->aValue.aint) && 
       Elem->yvalue->intval < *(Field->aValue.aint) + Field->aSize)
      return TRUE;
    else                                /*  integer array specified through enumeration */
      for(ActInt = Field->aValue.aint; ActInt < Field->aValue.aint + Field->aSize; 
	  ActInt ++)
	if( *ActInt == Elem->yvalue->intval)
	  return TRUE;
  return FALSE;
}







/*****************************************************************************************
 Function:                     LOG_EvalFloatInclusion
 Purpose: Evaluate inclusion relation for an float number and an array of float values.
 Returns: TRUE <==> element In array.
*****************************************************************************************/
short LOG_EvalFloatInclusion(Field, Elem)
     ArrayValueType *Field;
     ParserStackType *Elem;
{
  register float *ActFloat;                                  /* to scan array of floats */

  for(ActFloat = Field->aValue.afloat; ActFloat < Field->aValue.afloat + Field->aSize; 
      ActFloat ++)
    if( *ActFloat == Elem->yvalue->floatval)
      return TRUE;
  return FALSE;
}







/*****************************************************************************************
 Function:                     LOG_EvalStringInclusion
 Purpose: Evaluate inclusion relation for a string and an array of strings.
 Returns: TRUE <==> element In array.
*****************************************************************************************/
short LOG_EvalStringInclusion(Field, Elem)
     ArrayValueType *Field;
     ParserStackType *Elem;
{
  register char **ActString;                                /* to scan array of strings */
  for(ActString = Field->aValue.astring; ActString < Field->aValue.astring + Field->aSize;
      ActString ++)
    if( ! strcmp(*ActString, Elem->yvalue->string))
      return TRUE;
  return FALSE;
}






/*****************************************************************************************
 Function:                     LOG_CompareIntegers
 Purpose: Compares two integer expressions. Both operands must be of same data type.
 Returns: Pointer to ParserStackType record of type "Boolean", holding TRUE or FALSE.
 Side effects: Both operands are removed.
*****************************************************************************************/
ParserStackType *LOG_CompareIntegers(Operator, Left, Right)
     SymbolType Operator;                         /* specifies relation to be evaluated */
     ParserStackType *Left;        /* points to ParserStackType record for left operand */
     ParserStackType *Right;      /* points to ParserStackType record for right operand */
{
  switch(Operator) {
  case GT:
    Left->yvalue->intval = (Left->yvalue->intval > Right->yvalue->intval);
    break;
  case GE:
    Left->yvalue->intval = (Left->yvalue->intval >= Right->yvalue->intval);
    break;
  case EQ:
    Left->yvalue->intval = (Left->yvalue->intval == Right->yvalue->intval);
    break;
  case NE:
    Left->yvalue->intval = (Left->yvalue->intval != Right->yvalue->intval);
    break;
  case LT:
    Left->yvalue->intval = (Left->yvalue->intval < Right->yvalue->intval);
    break;
  case LE:
    Left->yvalue->intval = (Left->yvalue->intval <= Right->yvalue->intval);
    break;
  }
  Left->ytype = Boolean;
  PA_FreeParserStack(Right);
  return Left;
}







/*****************************************************************************************
 Function:                     LOG_CompareFloats
 Purpose: Compares two float expressions. Both operands must be of same data type.
 Returns: Pointer to ParserStackType record of type "Boolean", holding TRUE or FALSE.
 Side effects: Both operands are removed.
*****************************************************************************************/
ParserStackType *LOG_CompareFloats(Operator, Left, Right)
     SymbolType Operator;                         /* specifies relation to be evaluated */
     ParserStackType *Left;        /* points to ParserStackType record for left operand */
     ParserStackType *Right;      /* points to ParserStackType record for right operand */
{
  switch(Operator) {
  case GT:
    Left->yvalue->intval = (Left->yvalue->floatval > Right->yvalue->floatval);
    break;
  case GE:
    Left->yvalue->intval = (Left->yvalue->floatval >= Right->yvalue->floatval);
    break;
  case EQ:
    Left->yvalue->intval = (Left->yvalue->floatval == Right->yvalue->floatval);
    break;
  case NE:
    Left->yvalue->intval = (Left->yvalue->floatval != Right->yvalue->floatval);
    break;
  case LT:
    Left->yvalue->intval = (Left->yvalue->floatval < Right->yvalue->floatval);
    break;
  case LE:
    Left->yvalue->intval = (Left->yvalue->floatval <= Right->yvalue->floatval);
    break;
  }
  Left->ytype = Boolean;
  PA_FreeParserStack(Right);
  return Left;
}







/*****************************************************************************************
 Function:                     LOG_CompareStrings
 Purpose: Compares two string expressions. Both operands must be of same data type.
 Returns: Pointer to ParserStackType record of type "Boolean", holding TRUE or FALSE.
 Side effects: Both operands are removed.
*****************************************************************************************/
ParserStackType *LOG_CompareStrings(Operator, Left, Right)
     SymbolType Operator;                         /* specifies relation to be evaluated */
     ParserStackType *Left;        /* points to ParserStackType record for left operand */
     ParserStackType *Right;      /* points to ParserStackType record for right operand */
{
  if(STR_OrderedString(Left->yvalue->string) && STR_OrderedString(Left->yvalue->string)) {
    Left->yvalue->intval = strcmp(Left->yvalue->string, Right->yvalue->string);
    switch(Operator) {
    case GT:
      Left->yvalue->intval = (Left->yvalue->intval > 0);
      break;
    case GE:
      Left->yvalue->intval = (Left->yvalue->intval >= 0);
      break;
    case EQ:
      Left->yvalue->intval = (Left->yvalue->intval == 0);
      break;
    case NE:
      Left->yvalue->intval = (Left->yvalue->intval != 0);
      break;
    case LT:
      Left->yvalue->intval = (Left->yvalue->intval < 0);
      break;
    case LE:
      Left->yvalue->intval = (Left->yvalue->intval <= 0);
      break;
    }
    Left->ytype = Boolean;
  }
  else {
    ER_StackError("string error: cannot determin lexical order for \n", Serious,
		  STR_TypeString(Operator), nst, nst);
    /* there has been a type error - return undefined value, remove arguments */
    Left->ytype = Undef;
  }
  PA_FreeParserStack(Right);
  return Left;
}








