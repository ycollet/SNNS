/*****************************************************************************
  FILE              : co_stmt.c
  SHORTNAME         : stmt
  VERSION           : 2.0

  PURPOSE           : Contains functions to evaluate statements and expressions, which may be
                      l-values.
	                  - STMT_ValConstant           - STMT_RemoveValue
			  - STMT_BuildType             - STMT_GetUnitComponent
			  - STMT_BuildArrayType        - STMT_SelectIdentifierComponent
			  - STMT_DeclareVar            - STMT_SeekSite
			  - STMT_GetElement            - STMT_AssignValue
			  - STMT_GetIdValue            - STMT_AssignToComponent
			  - STMT_GetArrayElement       - STMT_MakeExplicitConnection
			  - STMT_GetStructureElem
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/


#include "co_stmt.h"


SymTabType *InnerSubnet = NULL;         /* pointer to symbol table which contains local */
				            /* identifiers (for selection "." operator) */



/*****************************************************************************************
 Function:                     STMT_ValConstant
 Purpose: Assign an expression value to a constants symbol table entry. Check if the 
          constant has been defined previously.
 Returns: void.
 Side effects: Modifies  symbol table entry for constant.
*****************************************************************************************/
void STMT_ValConstant(Left, Expr)
     ParserStackType *Left;          /* pointer to symbol table entry for constant name */
     ParserStackType *Expr;    /* pointer to parser stack value with new constant value */
{
  if(Expr)                      /* expression pointer is NULL if an array error occured */
    if(Expr->ytype != Undef) {            /* expression has been evaluated successfully */
      if( ! Left->yvalue->var->UndefType) {                 /* constant already defined */
	ER_StackError("redefinition: constant '%s' redefined\n", Warning,
		      STR_Save(Left->yvalue->var->Name), nst, nst);
	Left->yvalue->var->UndefType = FALSE;
      }
      if(Expr->ytype == ArrInt || Expr->ytype == ArrFloat || Expr->ytype == ArrFloat ||
	 Expr->ytype == ArrString)                              /* constant is an array */
	Expr->yvalue->array->Named = TRUE;               /* do not remove array anymore */
      Left->yvalue->var->UndefType = FALSE;
      Left->yvalue->var->UndefValue = FALSE;
      Left->yvalue->var->Constant = TRUE;      /* constant should not be modified later */
      Left->yvalue->var->Type = Expr->ytype;
      *(Left->yvalue->var->Value) = *(Expr->yvalue);
      /* do NOT use SymbolValueType of Expr - it is reused !! */
    }
    else {                   /* due to a previous error: cannot assign value to constant */
      ER_StackError("undefined: could not evaluate definition of '%s'\n", Warning,
		    STR_Save(Left->yvalue->var->Name), nst, nst);
      Left->yvalue->var->Constant = TRUE;      /* constant should not be modified later */
      Left->yvalue->var->UndefType = FALSE;  /* to avoid "undeclared var" message later */
      Left->yvalue->var->Type = Undef;  
    }
  else {                    /* due to a previous error: cannot assign value to constant */
    ER_StackError("undefined: could not evaluate definition of '%s'\n", Warning,
		  STR_Save(Left->yvalue->var->Name), nst, nst);
    Left->yvalue->var->Constant = TRUE;      /* constant should not be modified later */
  }
  PA_FreeParserStack(Left);
  PA_FreeParserStack(Expr);
}





/*****************************************************************************************
 Function:                     STMT_BuildType
 Purpose: Builds a ParserValueType record to pass type information or keywords from one
          grammar symbol to another on the parser value stack.
 Returns: Pointer to new ParserStackType record. The component ->ytype of this record
          holds the argument of STMT_BuildType (type or keyword), ->yvalue is not set.
*****************************************************************************************/
ParserStackType *STMT_BuildType(Type)
     SymbolType Type;
{
  ParserStackType *Result;
  Result = PA_GetParserStack();
  Result->ytype = Type;                    /* !! no checks on type !! */
  return Result;
}





/*****************************************************************************************
 Function:                     STMT_BuildArrayType
 Purpose: Modifies the type information in a ParserValueType record built by STMT_BuildType.
          A singular type is transformed into its correspondent array type.
 Returns: Pointer to the argument.
 Side effects: Modifies argument.
*****************************************************************************************/
ParserStackType *STMT_BuildArrayType(SimpleType)
     ParserStackType *SimpleType;
{
  switch(SimpleType->ytype) {
  case XINT:
    SimpleType->ytype = ArrInt;
    break;
  case XFLOAT:
    SimpleType->ytype = ArrFloat;
    break;
  case STRING:
    SimpleType->ytype = ArrString;
    break;
  case UNIT:
    SimpleType->ytype = STRUCTURE;            /* changed 28.11. - has been "= ArrUnit" */
    break;
  case STRUCTURE:
    SimpleType->ytype = ArrStruct;
    break;
  case ArrStruct:
    SimpleType->ytype = ArrArrStruct;
    break;
  default:
    (void) printf("******bug: attempt to build array of %s in STMT_BuildArrayType\n",
		  STR_TypeString(SimpleType->ytype));
    SimpleType->ytype = Undef;
  }
  return SimpleType;
}





/*****************************************************************************************
 Function:                     STMT_DeclareVar
 Purpose: Inserts type information about variables into their correspondent symbol table
          entries. Checks if there is a conflict (var_name is a constant, redefinition of
	  variable type, etc.).
 Returns: void.
 Side effects: Modifies symbol table entries of variables.
*****************************************************************************************/
void STMT_DeclareVar(VarType, IdList)
     ParserStackType *VarType;      /* parser stack value which holds the new data type */
     ParserStackType *IdList;       /* list of variables */
{
  register ParserStackType *ActId;    /* points to actually modified identifier in list */

  for(ActId = IdList; ActId; ActId = ActId->ynext) {  /* var_decl may contain 1..n id's */
    if( ! ActId->yvalue->var->UndefType) {      /* identifier declared (defined) before */
      if(ActId->yvalue->var->Type == STRUCTURE || ActId->yvalue->var->Type == ArrStruct) {
	ER_StackError("structure must not be declared as variable (ignored)\n",
		    Warning, nst, nst, nst);              /* DO NOT REDEFINE STRUCTURES */
	continue;                                      /* go to next identifier in list */
      }
      else {                              /* other variable types : redefine identifier */
	ER_StackError("redefinition of identifier '%s'\n", Warning, 
		      STR_Save(ActId->yvalue->var->Name), nst, nst);
	STMT_RemoveValue(ActId->yvalue->var);        /* cannot affect other identifiers */
      }
    }
    else {                                    /* identifier is undefined - new variable */
      ActId->yvalue->var->UndefType = FALSE; 
      ActId->yvalue->var->Type = (VarType->ytype == XINT)  ? INTEGER : 
	(VarType->ytype == XFLOAT) ? FLOATNUM  : VarType->ytype;    /* insert type info */
      ActId->yvalue->var->UndefValue = TRUE;      /* value of new variable is undefined */
    }
  }
  PA_FreeParserStack(VarType);                            /* both arguments are removed */
  EX_RemoveListTail(IdList);
}





/*****************************************************************************************
 Function:                     STMT_RemoveValue
 Purpose: If a variable or constant is redefined, its old value can be removed since an
          identifiers value cannot be on parser value stack while variable declarations
	  are parsed.
 Returns: void.
 Side effects: Removes value of symbol table entry.
*****************************************************************************************/
void STMT_RemoveValue(Var)
     SymTabLineType *Var;           /* parser stack value which holds the new data type */
{
  register MapLineType *Line;               /* to scan lines if value is a map constant */
  short Domain, Range;                           /* if map: domain and range data types */

  switch(Var->Type) {
   case ArrInt:
    (void) M_free((char *) Var->Value->array->aValue.aint);
    (void) M_free((char *) Var->Value->array);
    break;
   case ArrFloat:
    (void) M_free((char *) Var->Value->array->aValue.afloat);
    (void) M_free((char *) Var->Value->array);
    break;
   case ArrString:
    (void) M_free((char *) Var->Value->array->aValue.astring);
    (void) M_free((char *) Var->Value->array);
    break;
  case MAP:
    Domain = Var->Value->map->DType;
    Range = Var->Value->map->RType;
    for(Line = Var->Value->map->mLines; Line < Var->Value->map->mLines +
	Var->Value->map->mSize; Line ++) {                          /* scan lines of map */
      switch(Domain) {
      case ArrInt:
	(void) M_free((char *) Line->Domain->aValue.aint);
	break;
      case ArrFloat:
	(void) M_free((char *) Line->Domain->aValue.afloat);
	break;
      case ArrString:
	(void) M_free((char *) Line->Domain->aValue.astring);
	break;
      }                            /* scan lines of map */
      (void) M_free((char *) Line->Domain);
      switch(Range) {
      case INTEGER:
	(void) M_free((char *) Line->Range->aValue.aint);
	break;
      case FLOATNUM:
	(void) M_free((char *) Line->Range->aValue.afloat);
	break;
      case STRING:
	(void) M_free((char *) Line->Range->aValue.astring);
	break;
      }
      (void) M_free((char *) Line->Range);
    }
    (void) M_free((char *) Var->Value->map->mLines);
    break;
  }
}






/*****************************************************************************************
 Function:                     STMT_GetElement
 Purpose: Evaluates selection of an array or structure element.
 Returns: Parser value stack record holding element value.
 Side effects: Removes both arguments (parser value stack records holding array and
               index values).
 Special assumptions: Index value is integer and >= 0.
*****************************************************************************************/
ParserStackType *STMT_GetElement(Field, Index)
     ParserStackType *Field;       /* points to parser value stack record holding array */
     ParserStackType *Index; /* points to parser value stack record holding index (int) */
{
  if(Field->ytype == IDENTIFIER)  
    STMT_GetIdValue(Field);              /* field is 'variable' and can therefore be an */
  /* identifier assign the identifiers  type and value to field (index is 'expression') */
  if(Field->ytype != Undef && Index->ytype != Undef)               /* both args. are ok */
    switch(Field->ytype) {
    case STRUCTURE:
      return STMT_GetStructureElem(Field, Index);                /* selection of a unit */
    case ArrInt:
    case ArrFloat:         /* simple array (constant) - check if index is out of bounds */
    case ArrString:       
    case ArrStruct:                  /* array of structures is treated  as simple array */
      if(Index->yvalue->intval < Field->yvalue->array->aSize)          /* correct index */
	return STMT_GetArrayElement(Field, Index);     /* returns simple constant value */
      else {       /* error: Index->yvalue->intval is too large to be an index of field */
	ER_StackError("array index %s out of bounds\n", Serious, 
		      STR_ItoA(Index->yvalue->intval), nst, nst);
	break;
      }
    default:                   /* type error: Field does not hold an array or structure */
      ER_StackError("type mismatch: array or structure expected\n", Serious, nst, nst,
		    nst); 
    }                     /* there has been some kind of error - return undefined value */
  Index->ytype = Undef;
  PA_FreeParserStack(Field);
  return Index;
}






/*****************************************************************************************
 Function:                     STMT_GetIdValue
 Purpose: Evaluates an identifier.
 Returns: Parser value stack record holding the identifiers value which may be undefined.
 Side effects: Changes argument type and value.
*****************************************************************************************/
void STMT_GetIdValue(Id)
     ParserStackType *Id;           /* parser value stack elem holding identifier entry */
{
  if(Id->ytype == IDENTIFIER)                        /* else this function does nothing */
    if(Id->yvalue->var->UndefType) {     /* symbol table entry contains no defined TYPE */
      ER_StackError("undeclared variable: '%s'\n", Serious, STR_Save(Id->yvalue->var->Name),
		    nst, nst);    /* type is undefined -> identifier completely unknown */
      Id->ytype = Undef;
    }
    else if(Id->yvalue->var->UndefValue) {   /* identifier declared but value undefined */
      ER_StackError("'%s' used but not defined\n", Serious, STR_Save(Id->yvalue->var->Name),
		    nst, nst);    /* type is undefined -> identifier completely unknown */
      Id->ytype = Undef;
    }
    else {                    /* there is a correct type and value for the identifier - */
      Id->ytype = Id->yvalue->var->Type;                      /* insert it into argument*/
      *(Id->yvalue) = *(Id->yvalue->var->Value);                       /* copy of union */
    }
}






/*****************************************************************************************
 Function:                     STMT_GetArrayElement
 Purpose: Evaluates selection of an array element.
 Returns: Parser value stack record holding element value.
 Side effects: Removes both arguments (parser value stack records holding array and
               index values).
 Special assumptions: Index value is not out of the array bounds.
*****************************************************************************************/
ParserStackType *STMT_GetArrayElement(Field, Index)
     ParserStackType *Field;       /* points to parser value stack record holding array */
     ParserStackType *Index; /* points to parser value stack record holding index (int) */
{
  switch(Field->ytype) {
  case ArrInt:                                       /* result value will be an integer */
    Field->ytype = INTEGER;
    if(Field->yvalue->array->aRange)  /* array specified through range => element field */
           /* consists of a single pointer to an integer - the array values lower bound */
      Field->yvalue->intval= *(Field->yvalue->array->aValue.aint) + Index->yvalue->intval;
      /* adding  i to lower bound of values (value of element 0)  => the result will be */
      /* the value of element 0+i */
    else                          /* explicit array values (get value of element index) */
      Field->yvalue->intval = *(Field->yvalue->array->aValue.aint+Index->yvalue->intval);
    break;
  case ArrFloat:        /* result will be a float - array  must contain explicit values */
    Field->ytype = FLOATNUM;
    Field->yvalue->floatval = *(Field->yvalue->array->aValue.afloat +
				Index->yvalue->intval); 
    break;
  case ArrString:      /* result will be a string - array  must contain explicit values */
                           /* string arrays defined through range are stored explicitly */
    Field->yvalue->string = *(Field->yvalue->array->aValue.astring+Index->yvalue->intval); 
    if(strlen(Field->yvalue->string) == 1)
      Field->ytype = SIMPLESTRING;                               /* strings of lenght 1 */
    else
      Field->ytype = LONGSTRING;                                      /* longer strings */
    break;  
  case ArrStruct:              /* result will be a structure (STRUCTURE IS NOT COPIED!) */
    Field->ytype = STRUCTURE;
    Field->yvalue->structure = *(Field->yvalue->array->aValue.astruct+Index->yvalue->intval);
    break;  
#ifdef TEST
  default:
    (void) printf("******bug: illegal field type %s in STMT_GetArrayElement\n",
		  STR_TypeString(Field->ytype));
    Field->ytype = Undef;
#endif
  }
  PA_FreeParserStack(Index);      /* remove index and use field to return element value */
  return Field;
}






/*****************************************************************************************
 Function:                     STMT_GetStructureElem
 Purpose: Evaluates selection of a structure element. The value returnde depends on wether
          the expression is at the left side of a statement or not. Suppose our element
	  has not been changed before - then there's no explicit UnitType record for it.
	  If this element is selected at the right side of a statement, we can return a
	  pointer to the structure units description record (f.i. ST_TempStruct->UnitDef).
	  The unit value and its components can be read only.
	  On the left side of an assignment statement the unit value cannot be read, it
	  has been selected to be changed. Therefore, we must return a pointer to the
	  units explicit description record, although it may not contain any significant
	  value. 
 Returns: Parser value stack record whose value points to a UnitType record. This record
          is "read-only" if we are currently parsing the right side of an assignment
	  statement, it is "write-only" if we are on the left side.
 Side effects: Removes both arguments (parser value stack records holding array and
               index values).
*****************************************************************************************/
ParserStackType *STMT_GetStructureElem(Field, Index)
     ParserStackType *Field;       /* points to parser value stack record holding array */
     ParserStackType *Index; /* points to parser value stack record holding index (int) */
{
  if( ! (Index->yvalue->intval < Field->yvalue->structure->Length)) { 
    /*index out of bounds - return undefined value */
    Field->ytype = Undef;
    ER_StackError("structure index %s out of bounds\n", Serious, 
		  STR_ItoA(Index->yvalue->intval), nst, nst);
  }
  else  {                                             /* correct index - element exists */
    if(RightSide)                         /* read-only value needed - cannot be changed */
      if(Field->yvalue->structure->ExplUnits)               /* there are modified units */
	if( * (Field->yvalue->structure->ExplUnits + Index->yvalue->intval))
	  /* selected element has been modified - return it */
	  Field->yvalue->unit = 
	    *(Field->yvalue->structure->ExplUnits + Index->yvalue->intval); 
	else       /* selected element is unmodified - read original instead of element */
	  Field->yvalue->unit = Field->yvalue->structure->UnitDef;
      else         /* there are no modified elements - read original instead of element */
	Field->yvalue->unit = Field->yvalue->structure->UnitDef;
    else {   /* element will be modified rather than read - return explicit unit record */
      if( ! Field->yvalue->structure->ExplUnits)         /* there are no modified units */
	ST_InitExplUnits(Field->yvalue->structure);       /* generate explicit units !! */
      if( ! *(Field->yvalue->structure->ExplUnits + Index->yvalue->intval))
	/* selected element has been unmodified but will be changed now. Generate an */
	/* explicit unit record and insert it into the list of explicit units */
	*(Field->yvalue->structure->ExplUnits + Index->yvalue->intval) =
	  ST_SingleExplicitUnit(Field->yvalue->structure->UnitDef);   /* original for init */
      /* insert unit no.  into explicit unit record - needed for connection definition */
      (*(Field->yvalue->structure->ExplUnits + Index->yvalue->intval))->UnitNo = 
	Field->yvalue->structure->Begin + Index->yvalue->intval;  
                                         /* Begin holds no. of first unit in structure */
      /* assign value to return - remember that all known values of new record are set */ 
      Field->yvalue->unit = *(Field->yvalue->structure->ExplUnits +
			      Index->yvalue->intval);
    }
    Field->ytype = UNIT;
  }
  PA_FreeParserStack(Index);
  return Field;
}





/*****************************************************************************************
 Function:                     STMT_GetUnitComponent
 Purpose: Evaluate selection on an argument of type UNIT (act or name).
 Returns: &ParserStackType holding result.
 Side effects: Removes its first argument.
*****************************************************************************************/
ParserStackType *STMT_GetUnitComponent(Unit, Field)
     ParserStackType *Unit;        /* holds pointer to an identifier or unitType record */
     SymbolType Field;
{
  SelectionType *Res;                                 /* record to hold selection value */
  if(Unit->ytype == IDENTIFIER)                  /* if Unit is a variable get its value */
    STMT_GetIdValue(Unit);
  if(Unit->ytype != Undef) {
    if(Unit->ytype != UNIT) {        /* type error, selection of ACT or NAME on unit only */
      ER_StackError("type mismatch: cannot apply selection to %s\n", Serious,
		    STR_TypeString(Unit->ytype), nst, nst);
      Unit->ytype = Undef;
    }
    else {     /* correct data type of field */
      if(Field == ACT) {
	if(RightSide)
	  if(Unit->yvalue->unit->Act == ActNullValue) {
	    ER_StackError("unit activation value selected but not defined\n", Serious, nst,
			  nst, nst);
	    Unit->ytype = Undef;
	    return Unit;
	  }
	  else if(Unit->yvalue->unit->Act == RANDOM) {       /* activation is random value */
	    ER_StackError("cannot select random value\n", Serious, nst, nst, nst);
	    Unit->ytype = Undef;
	    return Unit;
	  }
      }
      else if(Field == NAME) {
	if( ! Unit->yvalue->unit->UName && RightSide) {                 /* name is undefined */
	  ER_StackError("unit name selected but not defined\n", Serious, nst, nst, nst);
	  Unit->ytype = Undef;
	  return Unit;
	}
      }
      /* else : Field ==  IOTYPE */
      Res = (SelectionType *) M_alloc(sizeof(SelectionType));      /* allocate record */
      Res->Component = Field;
      Res->Record.unit = Unit->yvalue->unit;
      Unit->ytype = Selection;
      Unit->yvalue->selection = Res;
    }
  }
  return Unit;
}











/*****************************************************************************************
 Function:                     STMT_SelectIdentifierComponent
 Purpose: Evaluate selection of a subnet component or unit site.
          Selection of a subnet component is done immediately, selection of a unit site is
	  stored into a SelectionType record.
 Returns: &ParserStackType holding res<ult.
 Side effects: Removes both first arguments.
*****************************************************************************************/
ParserStackType *STMT_SelectIdentifierComponent(Record, Field)
     ParserStackType *Record;                      /* holds pointer to left side of "." */
     ParserStackType *Field;                      /* holds pointer to right side of "." */
{
  SymTabLineType *Component;                            /* points to a subnet component */
  SelectionType *Res;                                 /* record to hold selection value */
  if(Field->ytype != Undef) {
    if(Record->ytype == IDENTIFIER)            /* if Record is a variable get its value */
      STMT_GetIdValue(Record);
    if(Record->ytype != Undef)
      if(Record->ytype == UNIT) {                           /* selection of a unit site */
	if(STMT_SeekSite(Record, Field)) {    /* identifier is a site type of this unit */
	  Res = (SelectionType *) M_alloc(sizeof(SelectionType));    /* allocate record */
	  Res->Component = SITE;
	  Res->Record.site.site = Field->yvalue->var->Value->site;
	  Res->Record.site.unit = Record->yvalue->unit->UnitNo;
	  Record->ytype = Selection;
	  Record->yvalue->selection = Res;
	}
	else
	  Record->ytype = Undef;
      }
      else if(Record->ytype == SUBNET) {             /* selection of a subnet component */
	InnerSubnet = Record->yvalue->subnet;  /* set ptr. to inner subnet symbol table */
	/* look for subnet component in the subnets symbol table */
	Component = SYM_LookUp(Field->yvalue->var->Name, InnerSubnet);
	if(Component->UndefType) {
	  ER_StackError("subnet: unknown identifier '%s' in '%s'\n", Serious,
			STR_Save(Field->yvalue->var->Name),
			STR_Save((Record->yvalue->subnet->Network) ? 
				 Record->yvalue->subnet->Network : " "), nst);
	  Record->ytype = Undef;
	}
	else if(Component->UndefValue) {
	  ER_StackError("subnet: '%s' in '%s' used but not defined\n", Serious,
			STR_Save(Field->yvalue->var->Name),
			STR_Save((Record->yvalue->subnet->Network) ? 
				 Record->yvalue->subnet->Network : " "), nst);
	  Record->ytype = Undef;
	}
	else if(Component->Type != STRUCTURE && Component->Type != UNIT && 
		Component->Type != ArrStruct && Component->Type != SUBNET) {
	  ER_StackError("subnet: cannot access '%s' in '%s' (not net component)\n", Serious,
			STR_Save(Field->yvalue->var->Name),
			STR_Save((Record->yvalue->subnet->Network) ? 
				 Record->yvalue->subnet->Network : " "), nst);
	  Record->ytype = Undef;
	}
	else {                 /* correct selection */
	  Record->ytype = IDENTIFIER;
	  Record->yvalue->var = Component;
	}
      }
      else              /* variable before '.' must be of type unit or subnet ==> error */
	ER_StackError("type mismatch: cannot select field of %s\n", Serious,
		      STR_TypeString(Record->ytype), nst, nst);
  }
  else
    Record->ytype = Undef;
  PA_FreeParserStack(Field);
  return Record;
}






/*****************************************************************************************
  Function:                     STMT_SeekSite
  Purpose: Look for a site of a unit.
  Note: Identity of sites depends on NAME OF SITE, and NOT identifier for site type !!
  Returns: TRUE <==> site has been found.
  *****************************************************************************************/
short STMT_SeekSite(Unit, Site)
     ParserStackType *Unit;                        /* ParserStack holds pointer to unit */
     ParserStackType *Site;                  /* ParserStack holds pointer to identifier */
{
  register SymTabLineType **End;                        /* pointer to end of site array */
  register SymTabLineType **ActSite;                     /* to scan sites array of unit */
  
  if(Unit->yvalue->unit->FType) {                    /* strucure with named unit type */
    ActSite = Unit->yvalue->unit->FParam.NamedFT->
      Value->unit->FParam.VarFT->Sites;    
    End = ActSite +                 /*   '`  '`  start */
      Unit->yvalue->unit->FParam.NamedFT->
	Value->unit->FParam.VarFT->SiteNo;/*end*/   
  }
  else {                                          /* no named unit type for structure */
    ActSite = Unit->yvalue->unit->FParam.VarFT->Sites;           /* start */
    End = ActSite + Unit->yvalue->unit->FParam.VarFT->SiteNo;        /*end*/
  }
  for(; ActSite < End; ActSite ++)       /* scan sites attached to units of structure */
    if((*ActSite)->Name == Site->yvalue->var->Name)
      return TRUE;                                          /* target is site of unit */
  /* site not found -- tried to use a nonexistent site as target for connections */
  ER_StackError("selected site '%s' not found in unit\n", Serious,
		STR_Save(Site->yvalue->var->Name), nst, nst);
  return FALSE;
}







/*****************************************************************************************
 Function:                     STMT_AssignValue
 Purpose: Assign an expression value to a variables symbol table entry. Check previous data
          type of variable.
 Returns: void.
 Side effects: Modifies symbol table entry for variable.
*****************************************************************************************/
void STMT_AssignValue(Left, Expr)
     ParserStackType *Left;          /* pointer to symbol table entry for variable name */
     ParserStackType *Expr;    /* pointer to parser stack value with new variable value */
{
  if(Expr)                      /* expression pointer is NULL if an array error occured */
    if(Expr->ytype != Undef)              /* expression has been evaluated successfully */
      if( Left->ytype != IDENTIFIER) 
	STMT_AssignToComponent(Left, Expr);
      else {
	if( ! Left->yvalue->var->UndefType) {               /* variable already defined */
	  if(Left->yvalue->var->Type != Expr->ytype && 
	     ! (Left->yvalue->var->Type == STRING && 
		(Expr->ytype == SIMPLESTRING || Expr->ytype == LONGSTRING))) {
	    if( ! (Expr->ytype == ArrInt || Expr->ytype == ArrFloat || Expr->ytype == ArrString)) {
	      ER_StackError("type mismatch: new value of type %s for variable '%s'\n", Warning,
			    STR_TypeString(Expr->ytype), STR_Save(Left->yvalue->var->Name), nst);
	      Left->yvalue->var->Type = Expr->ytype;
	      /* the variables data type has been redefined !! */
	    }
	    else 
	      ER_StackError("illegal data type for variable '%s'\n", Serious,
			    STR_Save(Left->yvalue->var->Name), nst, nst);
	  }
	  else if(Left->yvalue->var->Constant)
	    ER_StackError("redefinition of constant '%s'\n", Warning,
			  STR_Save(Left->yvalue->var->Name), nst, nst);	  
	}
	else {           /* first definition of variable - variable has not been declared */
	  ER_StackError("undeclared variable: '%s'\n", Warning,
			STR_Save(Left->yvalue->var->Name), nst, nst);
	  Left->yvalue->var->UndefType = FALSE;
	  Left->yvalue->var->Type = Expr->ytype;
	  if(Expr->ytype == ArrInt || Expr->ytype == ArrFloat || Expr->ytype == ArrString) {
	    ER_StackError("illegal data type for variable '%s'\n", Serious,
			  STR_Save(Left->yvalue->var->Name), nst, nst);
	    Left->yvalue->var->Type = Undef;
	  }
	}
	if( ! (Expr->ytype == ArrInt || Expr->ytype == ArrFloat || Expr->ytype == ArrString)) {
	  Left->yvalue->var->UndefType = FALSE;
	  Left->yvalue->var->UndefValue = FALSE;
	  Left->yvalue->var->Type = Expr->ytype;
	  if(AbortLoop && Left->yvalue->var->Lock)
	    Left->yvalue->var->Lock = FALSE;          /* outer for loop variable - unlock */
	  else if(Left->yvalue->var->Lock) {          /* variable of FOR or FOREACH loop  */
	    ER_StackError("illegal attempt to modify loop variable\n",Serious, nst,nst,nst);
	    AbortLoop = TRUE;                       /* inhibit further iterations of loop */
	    Left->yvalue->var->Lock = FALSE;            /* inhibit further error messages */
	  }
	  else {
	    *(Left->yvalue->var->Value) = *(Expr->yvalue);        
	    /* do NOT use SymbolValueType of Expr - it is reused !! */
	  }
	}
	else if(Left->yvalue->var->Constant)
	  ER_StackError("redefinition of constant '%s'\n", Warning,
			STR_Save(Left->yvalue->var->Name), nst, nst);	  
      }
/*    else  
      ER_StackError("undefined: could not evaluate definition of '%s'\n", Warning,
		  (Left->ytype == IDENTIFIER)  ? STR_Save(Left->yvalue->var->Name) :
		  STR_Save("unit component"), nst, nst);
  else    
    ER_StackError("undefined: could not evaluate definition of '%s'\n", Warning,
		  (Left->ytype == IDENTIFIER)  ? STR_Save(Left->yvalue->var->Name) :
		  STR_Save("unit component"), nst, nst);
*/
  PA_FreeParserStack(Left);
  PA_FreeParserStack(Expr);
}







/*****************************************************************************************
 Function:                     STMT_AssignToComponent
 Purpose: Assign an expression value to a unit component.
 Returns: void.
 Side effects: Modifies unit component, allocates explicit unit record, if necessary.
*****************************************************************************************/
void STMT_AssignToComponent(Comp, Value)
     ParserStackType *Comp;          /* pointer to symbol table entry for variable name */
     ParserStackType *Value;    /* pointer to parser stack value with new variable value */
{
  if(Comp->ytype == Selection)
    switch(Comp->yvalue->selection->Component) {
    case NAME:
      ST_CheckString(Value, NAME); 
      if(Value->ytype != Undef) {        /* string value has been correct for unit name */
	OUT_SetUNameLength(Value->yvalue->string);
	if(Comp->yvalue->selection->Record.unit->FType &&   /* named type for this unit */
	   Comp->yvalue->selection->Record.unit->FParam.NamedFT->Value->unit->UName)
	  /* unit name has been defined as type parameter, cannot be modified */
	  ER_StackError("cannot modify unit name (type parameter)\n",Warning,nst,nst,nst);
	else
	  Comp->yvalue->selection->Record.unit->UName = Value->yvalue->string;
      }
      break;
    case ACT:
      ST_CheckFloat(Value, ACT); 
      if(Value->ytype != Undef)     /* float value has been correct for unit activation */
	if(Comp->yvalue->selection->Record.unit->FType &&   /* named type for this unit */
	   Comp->yvalue->selection->Record.unit->FParam.NamedFT->Value->unit->Act != ActNullValue)
	  /* unit name has been defined as type parameter, cannot be modified */
	  ER_StackError("cannot modify unit activation (type parameter)\n",Warning,nst,nst,nst);
	else
	  if(Value->yvalue->floatval == RANDOM)
	    Comp->yvalue->selection->Record.unit->Act = GETRANDOM;
	  else
	    Comp->yvalue->selection->Record.unit->Act = Value->yvalue->floatval;
      break;
    case IOTYPE:
      if(Value->ytype != Undef) 
	if(Comp->yvalue->selection->Record.unit->FType &&   /* named type for this unit */
	   Comp->yvalue->selection->Record.unit->FParam.NamedFT->Value->unit->SType != Undef)
	  /* unit name has been defined as type parameter, cannot be modified */
	  ER_StackError("cannot modify topological type (type parameter)\n",Warning,nst,nst,nst);
	else 
	  if(Value->ytype != HIDDEN && Value->ytype != InOut && 
	     Value->ytype != INPUT && Value->ytype != OUTPUT) 
	    ER_StackError("type  mismatch: illegal topological type for unit\n", Serious,nst,nst,nst);
	  else
	    Comp->yvalue->selection->Record.unit->SType = Value->ytype;
      break;      
    case Undef:
      break;
    default:
      ER_StackError("type mismatch: illegal left side of assignment statement\n",
		    Serious,nst,nst,nst);
      break;
    }
  else
    if(Comp->ytype != Undef)
      ER_StackError("type mismatch: illegal left side of assignment statement\n",
		    Serious,nst,nst,nst);
}





/*****************************************************************************************
 Function:                     STMT_MakeExplicitConnection
 Purpose: Build an explicit connection.
 Returns: void.
*****************************************************************************************/
void STMT_MakeExplicitConnection(Source, Target, Weight, Forward)
     ParserStackType *Source;           /* source unit */
     ParserStackType *Target;           /* target unit */
     ParserStackType *Weight;           /* connection weight, may be NULL */
     short Forward;                     /* TRUE if there's no connection Target -> Source */
{
  int SNo;                /* source unit no. */
  int TNo;                /* target unit no. */
  SiteListType *Site;     /* pointer to target site at unit TNo */
  float ConnW;            /* connection weight */
  short Error;            /* TRUE <==> connection cannot be defined due to errors */
  
  Error = FALSE;          /* get source of connection */
  if(Source->ytype == Selection && Target->yvalue->selection->Component == SITE) {
    if(Forward)          
      /* there is no backward connection in statement, site specification makes no sense */
      ER_StackError("site specification for connection source ignored\n", Warning,
		    nst,nst,nst);
    SNo = Source->yvalue->selection->Record.site.unit;
  }
  else if(Source->ytype == UNIT)
    SNo = Source->yvalue->unit->UnitNo;
  else {
    if(Source->ytype != Undef) 
      ER_StackError("type mismatch: connection source unit expected\n", Serious, nst,nst,nst);
    Error = TRUE;
  }                   /* get target unit no. and - if defined - target site of this unit */
  if(Target->ytype == Selection && Target->yvalue->selection->Component == SITE) {
    TNo = Target->yvalue->selection->Record.site.unit;
    Site = Target->yvalue->selection->Record.site.site;
  }
  else if(Target->ytype == UNIT) {                 /* no target site for this connection */
    if(Target->yvalue->unit->FType &&           /* check if unit has sites */
       Target->yvalue->unit->FParam.NamedFT->Value->unit->FParam.VarFT->SiteNo  ||
       ! Target->yvalue->unit->FType && 
       Target->yvalue->unit->FParam.VarFT->SiteNo) {
      /* unit has sites,there cannot be a connection directly to the unit itself - error */
      ER_StackError("missing site specification for target unit\n", Serious, nst,nst,nst);
      Error = TRUE;
    }
    else {
      TNo = Target->yvalue->unit->UnitNo;
      Site = NULL;
    }
  }
  else {
    if(Target->ytype != Undef) 
      ER_StackError("type mismatch: connection target unit or site expected\n", Serious, nst,nst,nst);
    Error = TRUE;
  }
  if(Weight)                                       /* connection weight has been defined */
    if(Weight->ytype != Undef)            /* no error in definition of connection weight */
      if(Weight->yvalue->floatval == RANDOM)
	ConnW = GETRANDOM;                                   /* random connection weight */
      else
	ConnW = Weight->yvalue->floatval;                     /* fixed connection weight */
    else
      Error = TRUE;                  /* serious error in definition of connection weight */
  else
    ConnW = 0.0;                  /* connection weight undefined - insert default weight */
  if( ! Error)
    CONN_InsertConnection(TNo, Site, SNo, ConnW); /* generate entry into connection list */
}
    

