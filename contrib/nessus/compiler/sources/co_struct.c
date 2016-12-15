/*****************************************************************************
  FILE              : co_struct.c
  SHORTNAME         : struct
  VERSION           : 2.0

  PURPOSE           : Contains functions to evaluate the type and structure definition section of a
                      nessus program.
	                - ST_InsertSite                    - ST_InitExplUnits
			- ST_MakeSiteDef                   - ST_InsertWeight
			- ST_MakeSite                      - ST_CheckSite
			- ST_MakeSType                     - ST_InsertThrough
			- ST_AssertPos                     - ST_InsertDirection
			- ST_MakePos                       - ST_InsertCenter
			- ST_InitUnitType                  - ST_InitStructFType
			- ST_InsertUnitTypeParam           - ST_InitStructExpl
			- ST_CheckString                   - ST_CheckUnitCompleteness
			- ST_CheckFloat                    - ST_InsertStructName
			- ST_FinishUpUnitType              - ST_InsertStructAFunction
			- ST_DefineType                    - ST_InsertStructSType
			- ST_AssignStructures              - ST_InsertStructSType
			- ST_InitUnitValue                 - ST_InsertStructSites
			- ST_InsertStructSize              - ST_CopyUnitValue
			- ST_InsertStructAct               - ST_CopyExplicitUnitList
			- ST_CopyFType                     - ST_InsertMap
			- ST_CheckFunction                 - ST_BuildRangeSelection
			- ST_InsertMapConnections          - ST_StackType
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_struct.h"


/**** static variable definitions ****/
static UnitType *ST_TempUnit = NULL;                   /* holds unit defined in typedef */

/**** variables used by parser ****/
UnitValueType *ST_TempStruct = NULL;                   /* holds units defined in struct */
ParserStackType *ST_ActMap = NULL;        /* hold map constant for irregular structures */
int ST_SiteNo = 0;                            /* no. of sites of unit structure or type */

ParserStackType *ST_SiteTypes = NULL;                            /* stack of site types */

SiteListType *NULLSite = NULL;                               /* dummy variable for lint */ 

/*****************************************************************************************
 Function:                     ST_InsertSite
 Purpose: Builds list of site values (pointers to their correspondent symbol table entries).
 Returns: ParserStackType record (first element of site list).
 Side effects: Modifies last ->ynext entry in list.
 Note: To minimize no. of later error messages, correct sites are not removed when a
        single site leads to an error. 
*****************************************************************************************/
ParserStackType *ST_InsertSite(List, New)
     ParserStackType *List;                              /* points to head of site list */
     ParserStackType *New;                                /* points to new list element */   
{
  register ParserStackType *Scan;                              /* to search end of list */
  /* first, check if the list is empty or not */
  if(New)                                        /* list not empty - New == new element */
    if(ST_AssertSite(New))     /* new site is correct - check if there are  other sites */
      if(List) {                          /* there have been other correct sites before */
	for(Scan = List; Scan->ynext; Scan = Scan->ynext)     /* append new site at end */
	  ;                                     /* scan now points to last site in list */
	Scan->ynext = New;           /* new site becomes new last element of sites list */
	ST_SiteNo ++;                                  /* counts no. of (correct) sites */
	return List;
      }
      else {             /* new site becomes first (and - by now - unique) site of list */
	ST_SiteNo = 1;
	return New;
      }
    else {                /* New is not a correct site - remove it, List is not changed */
      PA_FreeParserStack(New);
      return List;
    }
  else {                  /* New == NULL => List is first Site, but it may be incorrect */
    if(ST_AssertSite(List)) {           /* List  becomes first and unique) site of list */
      ST_SiteNo = 1;
      return List;
    }
    else {                  /* List is incorrect - site list remains empty, return NULL */
      /* this leads to case List == NULL  &&  New != NULL at next call of ST_InsertSite */
      PA_FreeParserStack(List);                            /* ST_SiteNo value remains 0 */
      return NULL;
    }
  }
}





/*****************************************************************************************
 Function:                     ST_MakeSiteDef
 Purpose: Receives a list of sites and copies them into an array of &SymTabLineType.
 Returns: ParserStackType of type SITE holding the new array, or NULL.
*****************************************************************************************/
ParserStackType *ST_MakeSiteDef(List)
     ParserStackType *List;                              /* points to head of site list */
{
  register SymTabLineType **ActPos;       /* points to current insert position of array */
  register ParserStackType *Site;           /* points to currently inserted site record */
  ParserStackType *Res;               /* pointer to result record holding the new array */

  Res = NULL;                 /* !!! this function returns NULL if there's an error !!! */
  if(List) {                                   /* there have been correct sites in list */
    Res = PA_GetParserStack();            /* allocate ParserStackType record for result */
    Res->ytype = SITE;
    Res->yvalue->slist = (SymTabLineType **)    /* allocate array of pointers to symbol */
      M_alloc( (unsigned) sizeof(SymTabLineType *) * ST_SiteNo);       /* table entries */
    for(ActPos = Res->yvalue->slist, Site = List; Site; ActPos++, Site = Site->ynext)
      *ActPos = Site->yvalue->var;                /* copy pointer to symbol table entry */
    EX_RemoveListTail(List);                              /* List is not needed anymore */
  }
  return Res;
}






/*****************************************************************************************
 Function:                     ST_InitUnitType
 Purpose: Initialize a unit description record. If yyparse comes to the begin of a
          unit type definition, a record is allocated, into which all defined
	  unit parameters will be inserted. At the end of the type definition, this record
	  becomes the symbol table value entry for the correspondent identifier(s). 
 Returns: void.
 Side effects: Modifies ST_TempUnit.
*****************************************************************************************/
void ST_InitUnitType()
{
  if( ! ST_TempUnit) {         /* should be NULL at begin of a new unit type definition */
    /* allocate data structures (unit record and record for explicit ftype */
    ST_TempUnit = (UnitType *) M_alloc(sizeof(UnitValueType));
    ST_TempUnit->FType = FALSE;        /* a type definition cannot contain a ftype-def. */
    /* not necessary to allocate ->FParam, is part of UnitType */
    ST_TempUnit->FParam.VarFT = (ExplicitFType*) M_alloc(sizeof(ExplicitFType));
    /* mark entries as undefined */
    ST_TempUnit->FParam.VarFT->SiteNo = 0;              /* default is to have no sites */
    ST_TempUnit->FParam.VarFT->ActFunc = NULL;                    /* must be specified */
    ST_TempUnit->FParam.VarFT->OutFunc = NULL;                    /* must be specified */
    ST_TempUnit->FParam.VarFT->Sites = NULL;
    ST_TempUnit->UName = NULL;
    ST_TempUnit->SType = Undef;           /* NOT default HIDDEN - detect redefinitions */ 
    ST_TempUnit->Act = ActNullValue;    /* only for SNNS with -1.0 <= Act <= 1.0  !!!! */ 
    ST_TempUnit->XPos = 0;                 /* needed to store position of single units */
    ST_TempUnit->YPos = 0;                                /* x and y position for unit */
  }
  else
    (void) printf("******bug: ST_TempUnit not NULL in ST_InitUnitType\n");
}





/*****************************************************************************************
 Function:                     ST_InsertUnitTypeParam
 Purpose: Insert a unit parameter value into ST_TempUnit. Argument is assumed to have the
          correct data type. Param->ytype holds the unit component which is defined.
	  Check if the parameter is redefined.
 Returns: void
 Side effects: Modifies  ST_TempUnit.
*****************************************************************************************/
void ST_InsertUnitTypeParam(Param)
     ParserStackType *Param;        /* ->yvalue : value,   ->ytype : unit component key */
{
  if(Param) {                   /* can be NULL for illegal position or site definitions */
    switch(Param->ytype) {           /* which component is defined with Param->yvalue ? */
    case NAME:
      if(ST_TempUnit->UName)               /* redefinition of unit parameter -> warning */
	ER_StackError("typedef: redefinition of unit name\n", Warning, nst, nst, nst);
      ST_TempUnit->UName = Param->yvalue->string;
      break;
    case ACTFUNC:
      if(ST_TempUnit->FParam.VarFT->ActFunc)               /* redefinition of unit parameter -> warning */
	ER_StackError("typedef: redefinition of activation function\n", Warning, nst, nst, nst);
      ST_TempUnit->FParam.VarFT->ActFunc = Param->yvalue->var;
      break;
    case OUTFUNC:
      if(ST_TempUnit->FParam.VarFT->OutFunc)               /* redefinition of unit parameter -> warning */
	ER_StackError("typedef: redefinition of output function\n", Warning, nst, nst, nst);
      ST_TempUnit->FParam.VarFT->OutFunc = Param->yvalue->var;
      break;
    case ACT:
      if(ST_TempUnit->Act != ActNullValue)   /* redefinition of unit parameter -> warning */
	ER_StackError("typedef: redefinition of activation value in type\n", Warning, nst, nst, nst);
      ST_TempUnit->Act = Param->yvalue->floatval;
      break;
    case HIDDEN:
    case INPUT:
    case OUTPUT:
    case InOut:
      if(ST_TempUnit->SType != Undef)
	ER_StackError("typedef: redefinition of unit topology type\n", Warning, nst, nst, nst);
      ST_TempUnit->SType = Param->ytype;
      break;
    case SITE:
      if(ST_TempUnit->FParam.VarFT->Sites)
	ER_StackError("typedef: redefinition of unit sites\n", Warning, nst, nst, nst);
      ST_TempUnit->FParam.VarFT->Sites = Param->yvalue->slist;
      ST_TempUnit->FParam.VarFT->SiteNo = ST_SiteNo;
      ST_SiteNo = 0;                                           /*  (!!!!!) */
      break;
    case Undef:
      break;
    default:
      (void) printf("******bug: illegal unit parameter %s\n", STR_TypeString(Param->ytype));
    }
    PA_FreeParserStack(Param);
  }
}





/*****************************************************************************************
 Function:                     ST_CheckString
 Purpose: Check if a string contains any symbol not allowed in the compiler output (unit
          name, function name, etc.)
 Returns: &ParserStackType - the argument itself.
 Side effects: May set Argument->ytype to Undef (error case).
*****************************************************************************************/
ParserStackType *ST_CheckString(StrPtr, Key)
     register ParserStackType *StrPtr;     /* points to record which holds string value */
     register SymbolType Key;         /* how string is used - for better error messages */
{
  register char *String;                        /* local copy for fast access to string */
  register char *ActSymb;                                    /* actual symbol of string */
  register short Error;           /* true <=> there have been illegal symbols in string */

  String = NULL;
  if(Key == NAME)
    EX_AssertValue(StrPtr);                /* name must be written explicitly as string */
  if(StrPtr->ytype == IDENTIFIER) /* functions are written as identifiers - make string */
    if(strlen(String = STR_Save(StrPtr->yvalue->var->Name)) <= 1)
      StrPtr->ytype = SIMPLESTRING;
    else
      StrPtr->ytype = LONGSTRING;
  else if(StrPtr->ytype == SIMPLESTRING || StrPtr->ytype == LONGSTRING)/* normal string */
    String = StrPtr->yvalue->string;
  else {                            /*  type error : parameter value type is not string */
    ER_StackError("type mismatch: illegal data type %s for %s\n", Serious,
		  STR_TypeString(StrPtr->ytype), STR_TypeString(Key), nst);
    StrPtr->ytype = Undef;
  }
  if(String) {         /* until now, no errors found */
    for(ActSymb = String, Error = FALSE; *ActSymb; ActSymb++)  /* search illegal symbols*/
      if(SNNSILLEGALSTRING(*ActSymb)) {
	*ActSymb = '_';                         /* replace illegal symbol by underscore */ 
	Error = TRUE;
      }
    if(Error)            /* do NOT print an error message for each illegal symbol found */
      ER_StackError("string: illegal symbol(s) in %s replaced by '_'\n", Warning,
		    STR_TypeString(Key), nst, nst);
    for(ActSymb = String; ! SNNSLEADSTRING(*String); String++)
      ;                            /* first symbol of an output string must be a letter */
    if(ActSymb != String)     /* string has been shifted left */
      ER_StackError("string: illegal symbol at beginning of %s removed\n", Warning,
		    STR_TypeString(Key), nst, nst);
    StrPtr->yvalue->string = String;
    StrPtr->ytype = Key;                              /* used in ST_InsertUnitTypeParam */
  }
  return StrPtr;
}





/*****************************************************************************************
 Function:                     ST_CheckFloat
 Purpose: Checks if a float no. is an acceptable unit activation or connection weight.
 Returns: Argument.
 Side effects: May modify argument.
*****************************************************************************************/
ParserStackType *ST_CheckFloat(Expr, Key)
     register ParserStackType *Expr  ;      /* points to record which holds float value */
     register SymbolType Key;          /* how float is used - for better error messages */
{
  if(Expr->ytype == RANDOM) {           /* random numbers generated later */
    Expr->ytype = FLOATNUM;
    Expr->yvalue->floatval = (float) RANDOM;
    return Expr;
  }
  if(Expr->ytype == INTEGER) {          /* cast operation without warning */
    Expr->ytype = FLOATNUM;
    Expr->yvalue->floatval = (float) Expr->yvalue->intval;
  }
  if(Expr->ytype == FLOATNUM) {       /* floats for activation or weight in [-1.0, 1.0] */
    if(Expr->yvalue->floatval != RANDOM) 
      if(Expr->yvalue->floatval > 1.0) {
	ER_StackError("SNNS: %s not in [-1.0, 1.0] (set to 1.0)\n", Warning,
		      STR_TypeString(Key), nst, nst);
	Expr->yvalue->floatval = 1.0;
      }
      else if(Expr->yvalue->floatval < -1.0) {
	ER_StackError("SNNS: %s not in [-1.0, 1.0] (set to -1.0)\n", Warning,
		      STR_TypeString(Key), nst, nst);
	Expr->yvalue->floatval = -1.0;
      }
    Expr->ytype = Key;                      /* needed in ST_InsertUnitTypeParam */
  }
  else {
    ER_StackError("type mismatch: illegal data type %s for %s\n", Serious,
		  STR_TypeString(Expr->ytype), STR_TypeString(Key), nst);
    Expr->ytype = Undef;
  }
  return Expr;
}





/*****************************************************************************************
 Function:                     ST_FinishUpUnitType
 Purpose: Build a SymbolValueType entry for a unit type, append it to a parser stack value.
 Returns: &ParserStackType, holding unit type.
 Side effects: Modifies ST_TempUnit.
*****************************************************************************************/
ParserStackType *ST_FinishUpUnitType()
{
  ParserStackType *Result;

  Result = PA_GetParserStack();
  Result->ytype = UNITTYPE;             /* distinguish UNIT (var), STRUCT, UNITTYPE !!! */
  if( ! ST_TempUnit->FParam.VarFT->OutFunc) {
    ER_StackError("typedef: missing output function set to %s\n", Warning,
		  STR_Save(DefOutFunc), nst, nst);
  }
  if( ! ST_TempUnit->FParam.VarFT->ActFunc) {
    ER_StackError("typedef: missing activation function set to %s\n", Warning,
		  STR_Save(DefActFunc), nst, nst);
  }
  Result->yvalue->unit = ST_TempUnit;
  ST_TempUnit = NULL;
  return Result;
}





/*****************************************************************************************
 Function:                     ST_DefineType
 Purpose: Insert a unit type definition into the symbol table.
 Returns: void.
 Side effects: Symbol table entry of Id.
*****************************************************************************************/
void ST_DefineType(Val, Id)
     ParserStackType *Val;     /* ->ytype must be UNITTYPE or SITE */
     ParserStackType *Id;      /* ->ytype must be IDENTIFIER */
{
  if( ! Id->yvalue->var->UndefType)
    ER_StackError("typedef: attempt to redefine '%s' (ignored)\n", Serious,
		  STR_Save(Id->yvalue->var->Name), nst, nst);
  else {
    Id->yvalue->var->UndefType = FALSE;
    if((Id->yvalue->var->Type = Val->ytype) == UNITTYPE || Val->ytype == SITE) {
      Id->yvalue->var->UndefValue = FALSE;
      *(Id->yvalue->var->Value) = *(Val->yvalue);
      ST_StackType(Id, Val->ytype);                           /* build list of types */
      if(Val->ytype == UNITTYPE) 
	OUT_SetTypeLength(Id);               /* increment column width, if necessary */
      return;                          /* DO NOT REMOVE ParserStackType records !!!! */
    }
  }
  PA_FreeParserStack(Id);                  /* remove both parser value stack records */
  PA_FreeParserStack(Val);
}






/*****************************************************************************************
 Function:                     ST_AssignStructures
 Purpose: Insert a structure description into one or several symbol table entries
          ( ARRAY of ... : var1, var2,...; -> one copy per variable 
 Returns: void.
 Side effects: Modifies some symbol table entries and ST_TempStruct is reset to NULL.
*****************************************************************************************/
void ST_AssignStructures(Structure, List)
     ParserStackType *Structure;                  /* pointer to structure specification */
     ParserStackType *List;                 /* pointer to identifier list for structure */
{
  int XCenterLoc, YCenterLoc; /* save center of original - modified by ST_CopyUnitValue */
  register ParserStackType *Scan;                             /* scan list of variables */
  register UnitValueType *Temp;
  
  if(Structure->ytype != Undef)
    if(Structure->ytype == ArrStruct) 
      ST_AssignArrayOfStructure(Structure, List);
    else if(Structure->ytype == SUBNET)
      ST_AssignSubnet(Structure, List);
    else {
      if(Structure->yvalue->structure) {              /* structure definition has been ok */
	ST_UpdateConnectionCounter(Structure->yvalue->structure); /* update conn. counter */
	XCenterLoc = Structure->yvalue->structure->Topology.XCenter;        /*save center */
	YCenterLoc = Structure->yvalue->structure->Topology.YCenter;      /* of structure */
	if( ! List->yvalue->var->UndefType)             /* identifier is constant or type */
	  ER_StackError("redefinition of '%s'\n", Warning, STR_Save(List->yvalue->var->Name),
			nst, nst); 
	List->yvalue->var->UndefType = FALSE;
	List->yvalue->var->Lock = FALSE;
	List->yvalue->var->Constant = FALSE;            /* UNIT is stored as STRUCTURE !! */
	List->yvalue->var->UndefValue = FALSE;
	if((List->yvalue->var->Type = Structure->ytype) == STRUCTURE)        /* structure */
	  List->yvalue->var->Value->structure = Structure->yvalue->structure;
	else {                                                             /* single unit */
	  List->yvalue->var->Value->unit = Structure->yvalue->structure->UnitDef;
	  List->yvalue->var->Value->unit->UnitNo = Structure->yvalue->structure->Begin;
	  List->yvalue->var->Value->unit->XPos = XCenterLoc; 
	  List->yvalue->var->Value->unit->YPos = YCenterLoc;        /* copy unit position */
	}
	ST_InsertNetComponent(List->yvalue->var);/* insert id into global structures list */
	for(Scan = List->ynext; Scan; Scan = Scan->ynext) {
	  if( ! Scan->yvalue->var->UndefType)           /* identifier is constant or type */
	    ER_StackError("redefinition of '%s'\n", Warning,
			  STR_Save(Scan->yvalue->var->Name), nst, nst); 
	  Scan->yvalue->var->UndefType = FALSE;    /* generate an explicit new record for */
	  Scan->yvalue->var->UndefValue = FALSE;   /* each new variable - later modif. !! */
	  Scan->yvalue->var->Lock = FALSE;
	  Scan->yvalue->var->Constant = FALSE;
	  if((Scan->yvalue->var->Type = Structure->ytype) == STRUCTURE)      /* structure */
	  Scan->yvalue->var->Value->structure =
	      ST_CopyUnitValue(Structure->yvalue->structure);           /* copy structure */
	  else {          /*single units are first regarded as structures and then copied */
	    Temp = ST_CopyUnitValue(Structure->yvalue->structure);
	    Scan->yvalue->var->Value->unit = Temp->UnitDef;
	    Scan->yvalue->var->Value->unit->UnitNo = Temp->Begin;
	    Scan->yvalue->var->Value->unit->XPos = Temp->Topology.XCenter;  /* x position */
	    Scan->yvalue->var->Value->unit->YPos = Temp->Topology.YCenter;  /* y position */
	    (void) M_free((char *) Temp);                      /* remove structure record */
	  }
	  ST_InsertNetComponent(Scan->yvalue->var);  /* insert id into global struct list */
	}
	/* Center value has been modified - reset it to original value */
	Structure->yvalue->structure->Topology.XCenter = XCenterLoc;
	Structure->yvalue->structure->Topology.YCenter = YCenterLoc;
	if(Structure->yvalue->structure->StructType == IRREGULAR &&
	   Structure->yvalue->structure->Map.Low)         /* correct map specification */
	  ST_InsertMapConnections(Structure->yvalue->structure->Map.Low, 
				  Structure->yvalue->structure->Map.LineNo, 
				  Structure->yvalue->structure);
      }    /* if there have been errors in structure definition - do not define variables */
      EX_RemoveListTail(List);             /* remove parser stack values of variable list */
    }
}






/*****************************************************************************************
 Function:                     ST_AssignSubnet
 Purpose: Insert a subnet description into structure entries of outer program.
 Returns: void.
 Side effects: Modifies some symbol table entries and ST_TempStruct is reset to NULL.
*****************************************************************************************/
void ST_AssignSubnet(Structure, List)
     ParserStackType *Structure;                     /* pointer to subnet specification */
     ParserStackType *List;                 /* pointer to identifier list for structure */
{
  register ParserStackType *Scan;                             /* scan list of variables */
  if(Structure->ytype != Undef) {
    if(Structure->yvalue->subnet)      {               /* subnet definition has been ok */
      if( ! List->yvalue->var->UndefType)             /* identifier is constant or type */
	ER_StackError("redefinition of '%s'\n", Warning, STR_Save(List->yvalue->var->Name),
		      nst, nst); 
      List->yvalue->var->UndefType = FALSE;
      List->yvalue->var->UndefValue = FALSE;
      List->yvalue->var->Type = SUBNET;         /* structure or UNIT, but net component */
      List->yvalue->var->Value->subnet = Structure->yvalue->subnet;
      for(Scan = List->ynext; Scan; Scan = Scan->ynext) {
	if( ! Scan->yvalue->var->UndefType)           /* identifier is constant or type */
	  ER_StackError("redefinition of '%s'\n", Warning,
			STR_Save(Scan->yvalue->var->Name), nst, nst); 
	Scan->yvalue->var->UndefType = FALSE;    /* generate an explicit new record for */
	Scan->yvalue->var->UndefValue = FALSE;   /* each new variable - later modif. !! */
	Scan->yvalue->var->Type = Structure->ytype;                /* structure or UNIT */
	Scan->yvalue->var->Value->subnet =
	  ST_CopySubnet(Structure->yvalue->subnet);   
	/****  calls ST_InsertNetComponent foreach  structure of subnet ****/
      }
    }
  }
  EX_RemoveListTail(List);             /* remove parser stack values of variable list */
}





/*****************************************************************************************
 Function:                     ST_CopySubnet
 Purpose: Copy symbol table and  identifier values of subnet. Insert subnet into net component
          list.
 Returns: void
*****************************************************************************************/
SymTabType *ST_CopySubnet(Subnet)
     SymTabType *Subnet;                             /* pointer to subnet specification */
{
  SymTabType *Res;                                      /* will point to copy of subnet */
  register SymTabLineType **Line;
  register SymbolValueType *NewVal;             /* new value record  for var and struct */
  int MaxUnitNo;                         /* save max.  unit no. before subnet is copied */

  Res = (SymTabType *) M_alloc(sizeof(SymTabType));
  Res = (SymTabType *) memcpy((char*) Res, (char*) Subnet, (int) (sizeof(SymTabType)));
  /* allocate a copy of the subnets symbol table */
  Res->SymTab = (SymTabLineType **) 
    M_alloc((unsigned) Res->ActLength * sizeof(SymTabLineType *));
  (void) memcpy((char*) Res->SymTab, (char*) Subnet->SymTab,
		(int) Subnet->ActLength * sizeof(SymTabLineType *));
  MaxUnitNo = UnitCtr;
  for(Line = Res->SymTab; Line < Res->SymTab + (int) Res->ActLength; Line++) 
    if( *Line && ! (*Line)->UndefType && ! (*Line)->UndefValue && ! (*Line)->Constant) {
      *Line = (SymTabLineType *) (memcpy(M_alloc(sizeof(SymTabLineType)), (char*) *Line, 
					 (int) sizeof(SymTabLineType)));
      /* allocate new value record for variable or structure */
      NewVal = (SymbolValueType *) M_alloc(sizeof(SymbolValueType));
      /* network components must be copied, since they may be modified later */
      switch((*Line)->Type) {
      case STRUCTURE:        /* copy structure and insert it into net components list */
	NewVal->structure =  ST_CopyUnitValue((*Line)->Value->structure);
	(*Line)->Value = NewVal;
	ST_InsertNetComponent(*Line);
	break;
      case ArrStruct:        /* copy array of structures -- !! -- units may have been */
	NewVal->array =                              /*  modified -> scan whole array */
	  ST_CopyArrayOfStructures((*Line)->Value->array);
	(*Line)->Value = NewVal;
	ST_InsertNetComponent(*Line);
	break;
      case SUBNET:
	NewVal->subnet = ST_CopySubnet((*Line)->Value->subnet);
	(*Line)->Value = NewVal;
	break;
      default:   /* other entries must not be copied: arrays other than treated above */
	/* are constants which cannot  be modified, or simple types, which have been  */
	break;               /*  copied by memcpy(Res->SymTab,Subnet->SymTab) already */
      }
    }
  if(Subnet->MinUnitNo <= MaxUnitNo) 
    CONN_CopySubnetConnections(Subnet->MinUnitNo, MaxUnitNo);     /* copy connections */
  Res->MinUnitNo = MaxUnitNo+1;
  return Res;
}




/*****************************************************************************************
 Function:                     ST_CopyArrayOfStructures
 Purpose: Copy an array of structures, whose structure units may have been modified
 Returns: Ptr. to ArrayValueType (new array).
*****************************************************************************************/
ArrayValueType *ST_CopyArrayOfStructures(Array)
     ArrayValueType *Array;
{
  ArrayValueType *ArrPtr;
  register UnitValueType **Old, **New;                  /* scan arrays and copy values */
  
  ArrPtr = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
  ArrPtr = (ArrayValueType *) memcpy((char *) ArrPtr, (char *) Array, sizeof(ArrayValueType));
  ArrPtr->aValue.astruct = (UnitValueType **) 
    M_alloc((unsigned) sizeof(UnitValueType *) * Array->aSize);
  for(Old = Array->aValue.astruct, New = ArrPtr->aValue.astruct; 
      Old < Array->aValue.astruct + Array->aSize; Old++, New++)
    *New = ST_CopyUnitValue(*Old);
  return ArrPtr;
}




/*****************************************************************************************
 Function:                     ST_AssignArrayOfStructure
 Purpose: Insert an array of structures into one or several symbol table entries
          ( ARRAY of ... : var1, var2,...;)  -> one copy per variable 
 Returns: void.
 Side effects: Modifies some symbol table entries and ST_TempStruct is reset to NULL.
*****************************************************************************************/
void ST_AssignArrayOfStructure(Array, List)
     ParserStackType *Array;                          /* pointer to array of structures */
     ParserStackType *List;                 /* pointer to identifier list for structure */
{
  ParserStackType *Length;
  ParserStackType *TempStruct;
  int XCenterLoc, YCenterLoc;/* save center values of original - modified by ST_CopyUnitValue */
  register ParserStackType *Scan;                             /* scan list of variables */
  
  Length = PA_GetParserStack();
  Length->ytype = INTEGER;
  Length->yvalue->intval = Array->yvalue->array->aSize;
  TempStruct = PA_GetParserStack();                             /* save first structure */
  TempStruct->ytype = STRUCTURE;
  TempStruct->yvalue->structure = *Array->yvalue->array->aValue.astruct;
  XCenterLoc = TempStruct->yvalue->structure->Topology.XCenter;              /*save center */
  YCenterLoc = TempStruct->yvalue->structure->Topology.YCenter;      /* of first structure */
  if( ! List->yvalue->var->UndefType)                 /* identifier is constant or type */
    ER_StackError("redefinition of '%s'\n", Warning, STR_Save(List->yvalue->var->Name),
		  nst, nst); 
  List->yvalue->var->UndefType = FALSE;
  List->yvalue->var->UndefValue = FALSE;
  List->yvalue->var->Type = ArrStruct;        
  List->yvalue->var->Value->array = Array->yvalue->array;
  ST_InsertNetComponent(List->yvalue->var);         /* insert identifier into global structures list */
  Array->yvalue->array->Named = TRUE;                    /* do not remove array anymore */
  Scan = List->ynext;
  while(Scan) {
    if( ! Scan->yvalue->var->UndefType)               /* identifier is constant or type */
      ER_StackError("redefinition of '%s'\n", Warning,
		    STR_Save(Scan->yvalue->var->Name), nst, nst); 
    /* get new position in direction of LARGER dimension for array */
    if((TempStruct->yvalue->structure->Topology.XDim) >= 
       (TempStruct->yvalue->structure->Topology.YDim))
      TempStruct->yvalue->structure->Topology.XCenter += 
	TempStruct->yvalue->structure->Topology.XDim + 1;
    else
      TempStruct->yvalue->structure->Topology.YCenter += 
	TempStruct->yvalue->structure->Topology.YDim + 1;
    Scan->yvalue->var->UndefType = FALSE;        /* generate an explicit new record for */
    Scan->yvalue->var->UndefValue = FALSE;       /* each new variable - later modif. !! */
    Scan->yvalue->var->Type = ArrStruct;        /* structure or UNIT, but net component */
    /* increment center in direction of larger dimension */
    /* generate new unit numbers for new array entry */
    Scan->yvalue->var->Value->array = 
      ST_MakeArrayOfStructures(TempStruct, Length, TRUE)->yvalue->array;
    ST_InsertNetComponent(Scan->yvalue->var);       /* insert identifier into global structures list */
    Scan = Scan->ynext;
  }
  /* Center value of very first structure has been modified - reset it to original value */
  TempStruct->yvalue->structure->Topology.XCenter = XCenterLoc;
  TempStruct->yvalue->structure->Topology.YCenter = YCenterLoc;
  if(TempStruct->yvalue->structure->StructType == IRREGULAR &&
	    TempStruct->yvalue->structure->Map.Low)        /* correct map specification */
	   ST_InsertMapConnections(TempStruct->yvalue->structure->Map.Low, 
				   TempStruct->yvalue->structure->Map.LineNo, 
				   TempStruct->yvalue->structure);
  EX_RemoveListTail(List);               /* remove parser stack values of variable list */
  PA_FreeParserStack(TempStruct);
}




/*****************************************************************************************
 Function:                     ST_InitUnitValue
 Purpose: Allocates a new UnitValueType record to collect parameters of structure
          definitions.  ST_TempStruct will point to this new record.
 Returns: void.
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InitUnitValue(Type)
     SymbolType Type;           /* type of structure */
{
#ifdef TEST
  if(ST_TempStruct)
    (void) printf("******bug: ST_TempStruct is not NULL in ST_InitUnitValue\n");
#endif
  ST_TempStruct = (UnitValueType *) M_alloc(sizeof(UnitValueType));
  ST_TempStruct->StructType = Type;     /* unit, cluster, chain, ring, star, NOT array */
  ST_TempStruct->Direction = PT;                       /* forward is default direction */
  ST_TempStruct->Length = 1;                        /* default for illegal size values */
  ST_TempStruct->Map.Low = NULL;             /* first map line for irregular structure */
  ST_TempStruct->Map.LineNo = 0;           /* no. of map lines for irregular structure */
  ST_TempStruct->Through = NULL;                  /* default: no sites at target units */
  ST_TempStruct->Weight = 0.0;  /* default, NULL value not needed, cannot be redefined */
  ST_TempStruct->ExplUnits = NULL;
  ST_TempStruct->Center = 0;
  ST_TempStruct->Begin = ++UnitCtr;          /* UnitCtr == No. of units defined before */
  /* initialize example UnitType record for structure units  attached to ST_TempStruct */
  ST_TempStruct->UnitDef = (UnitType *) M_alloc(sizeof(UnitType));
  ST_TempStruct->UnitDef->UName = NULL;        /* special values and NOT ddefaults are */
  ST_TempStruct->UnitDef->SType = STypeNullValue;    /* needed to detect redefinitions */
  ST_TempStruct->Topology.Form = Undef;      /* topology form of structure not defined */
  ST_TempStruct->UnitDef->Act = ActNullValue;         /* ACT MUST BE IN [-1.0, 1.0] !! */
}





/*****************************************************************************************
 Function:                     ST_InsertStructSize
 Purpose: Insert size definition into ST_TempStruct. The size expression is assumed to be
          an integer value >= 0 (already checked by EX_CorrectSize
 Returns: void.
 Side effects: Modifies *ST_TempStruct.
*****************************************************************************************/
void ST_InsertStructSize(Size)
     ParserStackType *Size;       /* pointer to parser stack value holding integer >= 0 */
{
  if(Size->ytype != Undef)
    if( ! Size->yvalue->intval)                                     /* size must be > 0 */
      ER_StackError("struct: illegal null sized structure\n", Serious, nst, nst, nst);
    else
      ST_TempStruct->Length = Size->yvalue->intval;          /* assign structure length */
  /* else case : do NOT remove ST_TempStruct, let size be 1 */
  PA_FreeParserStack(Size);
  UnitCtr += ST_TempStruct->Length -1;       /*  UnitCtr == No. of units defined before */
}





/*****************************************************************************************
 Function:                     ST_InsertStructAct
 Purpose: Insert activation value into structure. Activation value may be a random number.
          Check if activation is parameter of an eventual FType, do NOT modify
	  FType-parameters.  Function parameter Act is assumed to hold a float value of
	  [-1.0, 1.0] if ytype is float, else ytype is RANDOM or Undef.
 Returns: void.
 Side effects: Modifies *Struc.
*****************************************************************************************/
void ST_InsertStructAct(Act)
     ParserStackType *Act;
{
  if(ST_TempStruct) {                          /* until now, structure has been correct */
    if(ST_TempStruct->UnitDef->FType)      /* structure parameters defined through type */
      if(ST_TempStruct->UnitDef->FParam.NamedFT->Value->unit->Act != ActNullValue) {
	ER_StackError("struct: attempt to redefine type parameter (act), ignored\n",
		      Warning, nst, nst, nst);
	PA_FreeParserStack(Act);                      /* remove  ParserStackType record */
	return;                               /* DO NOT OVERWRITE TYPE PARAMETER VALUES */
      }
    if(ST_TempStruct->UnitDef->Act != ActNullValue)            /* redefinition of act ? */ 
      ER_StackError("struct: redefinition of unit activation\n", Warning, nst, nst, nst); 
    if(Act->ytype != Undef)                       /* no errors in activation definition */
      /* activation may be (float) RANDOM - random number is  generated at output, only */
      ST_TempStruct->UnitDef->Act = Act->yvalue->floatval;         /* normal activation */
  }  /* else : previous errors in structure definition - cannot insert activation value */
  PA_FreeParserStack(Act);
  return;
}





/*****************************************************************************************
 Function:                     ST_InitExplUnits
 Purpose: Build an array of UnitType records - one for each unit of the structure.
          Initialize the UnitType records (NOT default values). Does nothing, if this
	  array already exists.
 Returns: void.
 Side effects: Modifies Node->ExplUnits and array it points to.
*****************************************************************************************/
void ST_InitExplUnits(Node)
     UnitValueType *Node;                /* ST_TempStruct (may be NULL) or a copy of it */
{
  register UnitType **Scan;         /* scan new array of UnitType records to initialize */

  if(Node)                                    /* may be NULL if there has been an error */
    if( ! Node->ExplUnits) {                /* array of UnitType records does not exist */
      Node->ExplUnits = (UnitType **) M_alloc((unsigned) (sizeof(UnitType*) * Node->Length));
      for(Scan = Node->ExplUnits; Scan < Node->ExplUnits + Node->Length; Scan ++) {
	*Scan = NULL;    /* list of explicit units is allocated, but the entries remain */
      }             /* undefined (NULL) for units without any explicitly defined values */
    }
}





/*****************************************************************************************
 Function:              ST_InsertWeight       
 Purpose: Insert weight of implicit connections into UnitValueType Record. If weight is
          RANDOM, the randomly distributed values are generated later in
	  ST_AssignStructures. 
 Returns: void.
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InsertWeight(Weight)
     ParserStackType *Weight;                            /* checked by EX_CorrectWeight */
{
  if(ST_TempStruct) {                                  /* no previously detected errors */
    if( ! Weight) {
      ST_TempStruct->Weight = 0.0;        /* not defined in xxx-program, red. fr. empty */
      return;
    }
    else if(Weight->ytype == Undef)     /* there has been an error in weight definition */
      ST_TempStruct->Weight = 0.0;
    else
      ST_TempStruct->Weight = Weight->yvalue->floatval;      /* float or (float) RANDOM */
  }
  PA_FreeParserStack(Weight);
  return;
}







/*****************************************************************************************
 Function:                     ST_CheckFunction
 Purpose: Checks if an identifier to be used as function is undefined. It must be
          undefined, since functions are defined at the end of a nessus program.
	  If a conflict is detected, the function prints an error message. The identifier
	  is redefined !!
 Returns: Pointer to argument, ParserStackType.
 Side effects: Modifies symbol table entry for identifier.
*****************************************************************************************/
ParserStackType *ST_CheckFunction(Function, FuncType)
     ParserStackType *Function; /* pt. to value stack element holding function identifier */
     SymbolType FuncType;              /* type of function: SITEFUNC, ACTFUNC, or OUTFUNC */

{
  if( ! Function->yvalue->var->UndefType && ! Function->yvalue->var->Type == Undef)
    /* identifier is redefined */
    ER_StackError("redefinition of '%s' - is now %s\n", Warning,
		  STR_Save(Function->yvalue->var->Name), STR_TypeString(FuncType), nst);
  /* insert funcType into ssymbol table entry */
  Function->yvalue->var->UndefType = FALSE;
  Function->yvalue->var->Type = FuncType;
  CF_MarkFunction(Function, FuncType);                        /* build stack of functions */
  return Function;
}






/*****************************************************************************************
 Function:                     ST_InitStructFType
 Purpose: Insert named type into ST_TempStruct.
 Returns: void
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InitStructFType(Type)
     ParserStackType *Type;     /* ParserStack holds pointer to identifier for unit type */

{
  /* check correctness of type identifier */
  if( ! Type->yvalue->var->UndefType)                  /* type of identifier is defined */
    if(Type->yvalue->var->Type == UNITTYPE) {          /* type of identifier is correct */
      ST_TempStruct->UnitDef->FParam.NamedFT = Type->yvalue->var;   /* assign unit type */
      ST_TempStruct->UnitDef->FType = TRUE;
      return;
    }
    else           /* error: identifier has been defined before, but is not a unit type */
      ER_StackError("type mismatch: '%s' os not a unit type\n", Serious,
		    STR_Save(Type->yvalue->var->Name), nst, nst);
  else                                 /* error: identifier has not been defined before */
    ER_StackError("identifier '%s' used but not defined\n", Serious,
		    STR_Save(Type->yvalue->var->Name), nst, nst);
  ST_InitStructTypeExpl();                      /* structure remains without named type */
  return;
}






/*****************************************************************************************
 Function:                     ST_InitStructTypeExpl
 Purpose: Insert record for explicit unit type parameters into ST_TempStruct.
 Returns: void
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InitStructTypeExpl()
{
  ST_TempStruct->UnitDef->FType = FALSE;        /* no named type for units of structure */
  ST_TempStruct->UnitDef->FParam.VarFT = (ExplicitFType *)   /* allocate record for */
    M_alloc(sizeof(ExplicitFType));                /* explicit unit type parameters */
  /* initialize explicit unit type parameters */
  ST_TempStruct->UnitDef->FParam.VarFT->SiteNo = 0;
  ST_TempStruct->UnitDef->FParam.VarFT->OutFunc = NULL;
  ST_TempStruct->UnitDef->FParam.VarFT->ActFunc = NULL;
  ST_TempStruct->UnitDef->FParam.VarFT->Sites = NULL;
}






/*****************************************************************************************
 Function:                     ST_InsertStructFunction
 Purpose: Insert activation or output function definition  into ST_TempStruct.
 Returns: void
 Side effects: Modifies ST_TempStruct.
 Note: Functions are not defined before their first use -- identifier data type must be
       undefined in symbol table ! 
*****************************************************************************************/
void ST_InsertStructFunction(Function, Type)
     ParserStackType *Function;              /* ParserStack holds pointer to identifier */
     SymbolType Type;                               /* function type (ACTFUNC, OUTFUNC) */
{
  if( ! Function->yvalue->var->UndefType && Function->yvalue->var->Type != Type) {
    ER_StackError("type mismatch: %s expected, identifier: '%s'\n", Serious,
		  STR_TypeString(Type), STR_Save(Function->yvalue->var->Name), nst); 
    return;                          /* output or activation function remains undefined */
  }
  else {                                                /* identifier can be a function */
    Function->yvalue->var->UndefType = FALSE;
    Function->yvalue->var->Type = Type;
    if(ST_TempStruct->UnitDef->FType) {       /* cannot redefine function for unit type */
      ER_StackError("cannot redefine %s for unit type\n", Warning, STR_TypeString(Type),
		    nst, nst);
      PA_FreeParserStack(Function);
      return;
    }        /* `' `':  structure parameters are redefined if defined already (warning) */
    else         /* units of structure are not of a named type - check for redefinition */
      if(Type == ACTFUNC) {                                      /* activation function */
	if(ST_TempStruct->UnitDef->FParam.VarFT->ActFunc)         /* is defined already */
	  ER_StackError("redefinition of 'actvation function'\n", Warning, nst, nst, nst);
	ST_TempStruct->UnitDef->FParam.VarFT->ActFunc = Function->yvalue->var; 
      }
      else if(Type == OUTFUNC) {                                     /* output function */
	if(ST_TempStruct->UnitDef->FParam.VarFT->OutFunc)         /* is defined already */
	  ER_StackError("redefinition of 'output function'\n", Warning, nst, nst, nst);
	ST_TempStruct->UnitDef->FParam.VarFT->OutFunc = Function->yvalue->var; 
      }
#ifdef TEST
      else 
	(void) printf("******bug: type in ST_InsertStructActFunc is %s\n", STR_TypeString(Type)); 
#endif
    PA_FreeParserStack(Function);
    return;
  }
}






/*****************************************************************************************
 Function:                     ST_InsertStructSType
 Purpose: Insert S type (INPUT, OUTPUTM, HIDDEN, InOut) into ST_TempStruct.
          Check if there is a redefinition.
 Returns: void
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InsertStructSType(TypePtr)
     ParserStackType *TypePtr;          /* holds S type (INPUT, OUTPUTM, HIDDEN, InOut) */
{
  if(ST_TempStruct->UnitDef->FType)                    /* structure units of named type */
    if(ST_TempStruct->UnitDef->FParam.NamedFT->Value->unit->SType != Undef) {
      ER_StackError("cannot redefine topology type for unit type\n", Warning, 
		    nst, nst, nst);
      PA_FreeParserStack(TypePtr);
      return;
    }        /* there is a type, but S type is not a type parameter, OR there's no type */
  if(ST_TempStruct->UnitDef->SType != Undef)
    ER_StackError("redefinnition of topology type\n", Warning, nst, nst, nst);
  ST_TempStruct->UnitDef->SType = TypePtr->ytype;
  PA_FreeParserStack(TypePtr);
  return;
}
    







/*****************************************************************************************
 Function:                     ST_InsertStructName
 Purpose: Insert unit name into ST_TempStruct.
          Check if there is a redefinition.
 Returns: void
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InsertStructName(NamePtr)
     ParserStackType *NamePtr;                                   /* holds name of units */
{
  if(NamePtr->ytype != Undef) {          /* name definition expression has been correct */
    if(ST_TempStruct->UnitDef->FType)                  /* structure units of named type */
      if(ST_TempStruct->UnitDef->FParam.NamedFT->Value->unit->UName) {
	ER_StackError("cannot redefine name (type parameter)\n", Warning, nst, nst, nst);
	PA_FreeParserStack(NamePtr);
	return;
      }        /* there is a type, but name is not a type parameter, OR there's no type */
    if(ST_TempStruct->UnitDef->UName)
      ER_StackError("redefinition of unit name\n", Warning, nst, nst, nst);
    ST_TempStruct->UnitDef->UName = NamePtr->yvalue->string;
  }
  PA_FreeParserStack(NamePtr);
  return;
}







/*****************************************************************************************
 Function:                     ST_MakeSType
 Purpose: Build parser value stack  record holding topology type (INPUT, OUTPUTM, HIDDEN,
          InOut) in ->ytype field.
 Returns: ParserStackType record with undefined value, ytype holding unit topology type.
*****************************************************************************************/
ParserStackType *ST_MakeSType(Type)
     SymbolType Type;                                /* (INPUT, OUTPUTM, HIDDEN, InOut) */
{
  ParserStackType *TypePtr;             /* holds S type (INPUT, OUTPUTM, HIDDEN, InOut) */
  TypePtr = PA_GetParserStack();         /* get pointer to a new ParserStackType record */
  TypePtr->ytype = Type;
#ifdef TEST
  if(Type != HIDDEN && Type != INPUT && Type != OUTPUT && Type != InOut) {
    (void) printf("******bug: type %s in ST_MakeSType\n", STR_TypeString(Type));
    TypePtr->ytype = Undef;
  }
#endif

  return TypePtr;
}






/*****************************************************************************************
 Function:                     ST_InsertStructDirection
 Purpose: Insert S type (INPUT, OUTPUTM, HIDDEN, InOut) into ST_TempStruct.
          Check if there is a redefinition.
 Returns: void
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InsertStructDirection(Direction)
     SymbolType Direction;          /* holds S type (INPUT, OUTPUTM, HIDDEN, InOut) */
{
  if(ST_TempStruct->StructType == CLIQUE)
    ER_StackError("illegal connection direction for clique ignored\n", Warning, nst, nst, nst);
  else
    ST_TempStruct->Direction = Direction;
  /* redefinition of direction is syntactically impossible */
}






/*****************************************************************************************
 Function:                     ST_InsertStructCenter
 Purpose: Insert center for STAR struct into ST_TempStruct.
          Check if structure is really a STAR.
 Returns: void
 Side effects: Modifies ST_TempStruct.
 Special assumptions: CenterPtr->yvalue->intval >= 0 !!  (EX_CheckFieldSize();)
*****************************************************************************************/
void ST_InsertStructCenter(CenterPtr)
     ParserStackType *CenterPtr;                                 /* holds name of units */
{
  if(CenterPtr->ytype != Undef)      /* set to Undef by EX_CheckFieldSize() if negative */
    if(CenterPtr->yvalue->intval < ST_TempStruct->Length) /* center in structure bounds */
      if(ST_TempStruct->StructType == STAR)                  /* center for STAR, only ! */
	ST_TempStruct->Center = CenterPtr->yvalue->intval;          /* ok, assign value */
      else                                         /* structure type is other than STAR */
	ER_StackError("illegal center specification for %s ignored\n", Warning, 
		      STR_TypeString(ST_TempStruct->StructType), nst, nst);
    else                                        /* center index out of structure bounds */
	ER_StackError("index of center is out of structure bounds (ignored)\n", Warning, 
		      nst, nst, nst);
  PA_FreeParserStack(CenterPtr);
}





/*****************************************************************************************
 Function:                     ST_AssertSite
 Purpose: Checks wether a site specification is correct (defined previously, correct data
          type) or not.
 Returns: TRUE if site has been correct, else FALSE.
*****************************************************************************************/
short ST_AssertSite(Site)
     ParserStackType *Site;                           /* holds identifier for site type */
{
  if( ! Site->yvalue->var->UndefType)             /* identifier has been defined before */
    if (Site->yvalue->var->Type == SITE)                             /* data type is ok */
      return TRUE;
    else                                               /* identifier is not a site type */
      ER_StackError("type mismatch: site type expected \n", Serious, nst, nst, nst); 
  else                                 /* error: identifier has not been defined before */
    ER_StackError("identifier '%s' used but not defined\n", Serious,
		  STR_Save(Site->yvalue->var->Name), nst, nst);
  return FALSE;
}






/*****************************************************************************************
 Function:                     ST_InsertStructSites
 Purpose: Insert sites into ST_TempStruct.
 Returns: void
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InsertStructSites(SiteList)
     ParserStackType *SiteList;              /* ParserStack holds pointer to identifier */
{
  if(SiteList) {                                 /* site list exists at least partially */
    if(ST_TempStruct->UnitDef->FType || ST_TempStruct->UnitDef->FParam.VarFT->SiteNo) {
      /* sites attached to structure units are defined already */
      ER_StackError("attempt to redefine unit sites (ignored)\n", Warning, nst, nst, nst);
      M_free((char *) SiteList->yvalue->slist);
      ST_SiteNo = 0;                     /* !!! */
    }
    else {                                                       /* new sites -- insert */
      ST_TempStruct->UnitDef->FParam.VarFT->SiteNo = ST_SiteNo;
      ST_SiteNo = 0;
      ST_TempStruct->UnitDef->FParam.VarFT->Sites = SiteList->yvalue->slist;
    }
    PA_FreeParserStack(SiteList);
  }
}






/*****************************************************************************************
 Function:                     ST_InsertThrough
 Purpose: Insert target site for implicit connections of structure.
 Returns: void
 Side effects: Modifies ST_TempStruct.
*****************************************************************************************/
void ST_InsertThrough(Site)
     ParserStackType *Site;                  /* ParserStack holds pointer to identifier */
{
  register SymTabLineType **End;                        /* pointer to end of site array */
  register SymTabLineType **ActSite;                     /* to scan sites array of unit */

  if( ! Site ) {
    if(ST_TempStruct->StructType != CLUSTER)
      if(ST_TempStruct->UnitDef->FType &&                      /* check if unit has sites */
	 ST_TempStruct->UnitDef->FParam.NamedFT->Value->unit->FParam.VarFT->Sites ||
	 ! ST_TempStruct->UnitDef->FType && 
	 ST_TempStruct->UnitDef->FParam.VarFT->Sites) 
	/* missing specification of target site for implicit connections,units have sites */
	ER_StackError("structure: missing target site specification for %s\n", Serious,
		      STR_TypeString(ST_TempStruct->StructType), nst, nst);
    return;
  }
  /* else : there is a target site specification, structure cannot be a cluster */
  if(ST_AssertSite(Site)) {           /* target for implicit connections must be a site */
    if(ST_TempStruct->UnitDef->FType) {                /* strucure with named unit type */
      ActSite = ST_TempStruct->UnitDef->FParam.NamedFT->Value->unit->FParam.VarFT->Sites;    
      End = ActSite +                 /*   '`  '`  start */
	ST_TempStruct->UnitDef->FParam.NamedFT->Value->unit->FParam.VarFT->SiteNo; /*end*/   
    }
    else {                                          /* no named unit type for structure */
      ActSite = ST_TempStruct->UnitDef->FParam.VarFT->Sites;                   /* start */
      End = ActSite + ST_TempStruct->UnitDef->FParam.VarFT->SiteNo;                /*end*/
    }
    for(; ActSite < End; ActSite ++)       /* scan sites attached to units of structure */
      if((*ActSite)->Name == Site->yvalue->var->Name) {      /* target is site of units */
	ST_TempStruct->Through = Site->yvalue->var;
	PA_FreeParserStack(Site);
	return;
      }
    /* site not found -- tried to use a nonexistent site as target for connections */
    ER_StackError("site '%s' not found in %s\n", Serious,STR_Save(Site->yvalue->var->Name),
		  STR_TypeString(ST_TempStruct->StructType), nst);
    PA_FreeParserStack(Site);
  }
  return;
}





/*****************************************************************************************
 Function:                     ST_CopyUnitValue
 Purpose: Copies a UnitValueType record including explicit units and VarFT parameters.
 Returns: &UnitValueType -- pointer to copy.
 Side effects: Modifies center specifificaation of original value !!
*****************************************************************************************/
UnitValueType *ST_CopyUnitValue(Org)
     UnitValueType *Org;                              /* original record - to be copied */
{
  UnitValueType *Copy;                                            /* will point to copy */
  Copy = (UnitValueType *) M_alloc(sizeof(UnitValueType));           /* allocate record */
  Copy->StructType = Org->StructType;                            /* copy structure type */
  Copy->Direction = Org->Direction;                                   /* copy direction */
  /* copy topology, top. of copy is same as org., shifted in direction of <<< dimension */
  Copy->Topology.Form = Org->Topology.Form;
  Copy->Topology.XDim = Org->Topology.XDim;
  Copy->Topology.YDim = Org->Topology.YDim;
  if(Org->Topology.XDim >= Org->Topology.YDim) {     /* x dim larger -> shift to y dir. */
    Copy->Topology.YCenter = Org->Topology.YCenter + Org->Topology.YDim + 2;  
    Copy->Topology.XCenter = Org->Topology.XCenter;     
  } 
  else {
    Copy->Topology.YCenter = Org->Topology.YCenter;      
    Copy->Topology.XCenter = Org->Topology.XCenter + Org->Topology.XDim + 2;  
  }
  Copy->Begin = ++UnitCtr;                       /* nos. of units must not be identical */
  UnitCtr += (Copy->Length = Org->Length) - 1;    /* copy length, increase unit counter */
  Copy->Through = Org->Through;            /* copy target site for implicit connections */
  Copy->Weight = Org->Weight;                    /* copy weight of implicit connections */
  Copy->Map.Low = Org->Map.Low;          /* copy connection specification for irregular */
  Copy->Map.LineNo = Org->Map.LineNo;    /* copy connection specification for irregular */
  Copy->Center = Org->Center;                 /* copy center (set if structure is STAR) */
  Copy->UnitDef = (UnitType *) M_alloc(sizeof(UnitType)); /* allocate new UnitType rec. */
  Copy->UnitDef->FType = Org->UnitDef->FType;            /* copy information about type */
  Copy->UnitDef->UName = Org->UnitDef->UName;                         /* copy unit name */
  Copy->UnitDef->SType = Org->UnitDef->SType;                     /* copy topology type */
  Copy->UnitDef->Act = Org->UnitDef->Act;           /* activation copied even if RANDOM */
  if(Copy->UnitDef->FType)
    Copy->UnitDef->FParam.NamedFT = Org->UnitDef->FParam.NamedFT;  /* ptr. to unit type */
  else {                   /* type parameters have been defined explicitly -- copy them */
    /* to  new record -- do not  use same record - parameters  might be  modified later */
    Copy->UnitDef->FParam.VarFT = (ExplicitFType *)              /* allocate new record */
      M_alloc(sizeof(ExplicitFType));
    Copy->UnitDef->FParam.VarFT->SiteNo = Org->UnitDef->FParam.VarFT->SiteNo;  
    Copy->UnitDef->FParam.VarFT->Sites = Org->UnitDef->FParam.VarFT->Sites;  
    Copy->UnitDef->FParam.VarFT->ActFunc = Org->UnitDef->FParam.VarFT->ActFunc;
    Copy->UnitDef->FParam.VarFT->OutFunc = Org->UnitDef->FParam.VarFT->OutFunc;
  }
  if(Org->ExplUnits)
    Copy->ExplUnits = ST_CopyExplicitUnitList(Org, Copy->Begin);
  else 
    Copy->ExplUnits = NULL;
  /* generate new position, increment x if structure is vertical, else increment y */
  if(Org->Topology.XDim >= Org->Topology.YDim)  {                     /* increment x */
    Copy->Topology.YCenter = (Org->Topology.YCenter += Org->Topology.YDim + 1);
    OUT_SetPositionLength(Org->Topology.YCenter+Org->Topology.YDim);
  }
  else {
    Copy->Topology.XCenter = (Org->Topology.XCenter += Org->Topology.XDim + 1);
    OUT_SetPositionLength(Org->Topology.XCenter+Org->Topology.XDim);
  }   /* if structure is IRREGULAR, insert connections for copy */
  if(Copy->StructType == IRREGULAR &&
	    Copy->Map.Low)                                /* correct map specification */
	   ST_InsertMapConnections(Copy->Map.Low, 
				   Copy->Map.LineNo, 
				   Copy);
  ST_UpdateConnectionCounter(Copy);        /* update counters of units and connections */
  return Copy;
}





/*****************************************************************************************
  Function:                     ST_CopyExplicitUnitList
  Purpose: Builds a new list of explicit units and copies the units parameters of the old
	   list to the correspondent new units.
  Returns: pointer to array of explicit units (& UnitType).
*****************************************************************************************/
UnitType **ST_CopyExplicitUnitList(Structure, NewBegin)
     UnitValueType *Structure;      /* pointer to structure record (length is needed !) */
     int NewBegin;                                /* index of new structures first unit */
 {
   register int i;
   register UnitType **ScanOld, **ScanNew;       /* scan both arrays of UnitType records */
   UnitType **NewList;                       /* pointer to new array of UnitType records */
   NewList = (UnitType **) M_alloc((unsigned) (sizeof(UnitType*) * Structure->Length));
   /* scan both arrays of explicit values - copy unit per unit */
   for(ScanNew = NewList, ScanOld = Structure->ExplUnits, i=NewBegin;
       ScanOld < Structure->ExplUnits + Structure->Length; ScanOld ++, ScanNew ++,  i++) { 
     if(*ScanOld) {             /* explicitly defined unit - build a new record for copy */
       *ScanNew = ST_SingleExplicitUnit(Structure->UnitDef);
       (*ScanNew)->FType = (*ScanOld)->FType;
       (*ScanNew)->UName = (*ScanOld)->UName;
       (*ScanNew)->SType = (*ScanOld)->SType;
       (*ScanNew)->Act = (*ScanOld)->Act;
       (*ScanNew)->UnitNo = i;             /* insert unit no. into  explicit unit record */
       /* explicit values cannot be defined for type parameters with "get .. from"       */
       (*ScanNew)->FParam.VarFT = (*ScanOld)->FParam.VarFT; 
     }
     else 
       (*ScanNew) = NULL;                                  /* unit has no explicit value */
   }
   return NewList;
 }







 /*****************************************************************************************
  Function:                     ST_SingleExplicitUnit
  Purpose: Allocate a single record for an explicit unit. Initialize fields of record.
		ALL ENTRIES ARE SET TO THOSE OF STRUCTURE DESCRIPTION RECORD !!!
  Returns: pointer to explicit unit (& UnitType).
 *****************************************************************************************/
 UnitType *ST_SingleExplicitUnit(MainUnit)
      UnitType *MainUnit;    /* type information is copied because site information must */
 {                                            /* be preserved for connection definitions */
   UnitType *New;                                                 /* pointer to new unit */

   New = (UnitType *) M_alloc(sizeof(UnitType));
   New->UName = MainUnit->UName;
   New->SType = MainUnit->SType;                 /* NOT default, detect redefinitions !! */
   New->Act = MainUnit->Act;
   New->FType = MainUnit->FType; /* preserve site information for connection definitions */
   New->FParam = MainUnit->FParam;
   New->UnitNo = 0;
   return New;
 }





 /*****************************************************************************************
  Function:                     ST_InsertTopCenter
  Purpose: Insert  center specification for structure topology into structure description
	   record (UnitValueType). Center must be defined in nessus program.
  Returns: void.
  Side effects: Modifies ST_TempStruct->Topology-> {XCenter, YCenter}. Removes argument
		records. 
 *****************************************************************************************/
 void ST_InsertTopCenter(XPos, YPos)
      ParserStackType *XPos, *YPos; /* parser value stack elems holding positions (int) */
 {                        
   ST_CheckPosition(XPos);            /* Both values are guaranteed to be integers >= 0 */
   ST_CheckPosition(XPos);                              /* else ->ytype is set to Undef */
   if(XPos->ytype != Undef && YPos->ytype != Undef) {             /* coordinates are ok */
     /* position is  incremented by networks or subnets position (is (0,0) for network) */
     ST_TempStruct->Topology.XCenter = XPos->yvalue->intval+XCenter;      /* x position */
     ST_TempStruct->Topology.YCenter = YPos->yvalue->intval+YCenter;      /* y position */
   }
   PA_FreeParserStack(XPos);                      /* remove parser value stack elements */
   PA_FreeParserStack(YPos); 
 }





 /*****************************************************************************************
  Function:                     ST_InsertTopology
  Purpose: Insert  topology specification for structure topology into structure description
	   record (UnitValueType). Topology must be defined in nessus program.
	   A structure topology specification consists of top_form (plane, matrix,
	   ellipse), extensions of this form (in x and y directions, > 0).
  Returns: void.
  Side effects: Modifies ST_TempStruct->Topology. {XCenter, YCenter}. Removes argument
		records. 
  Special assumptions: The form argument is MATRIX or ELLIPSE. The form PLANE is converted
		       into a matrix with extension 1 in either x or y direction
		       (horizontal or vertical plane).
 *****************************************************************************************/
 void ST_InsertTopology(Form, XExt, YExt)
      SymbolType Form;       /* Form of topology, IS NOT PLANE, can be matrix or ellipse */
      ParserStackType *XExt, *YExt;  /* parser value stack elems holding positions (int) */
 {
   ST_CheckExtension(XExt);                  /* extension must be integer >= 1 (not 0 !) */
   ST_CheckExtension(YExt);                              /* else ->ytype is set to Undef */
   if(XExt->ytype != Undef && YExt->ytype != Undef) {               /* extensions are ok */
     /* structure dimensions must be large enaugh for all units */
     if(Form == MATRIX)             /* units can be displayed at integer positions, only */
       if(XExt->yvalue->intval * YExt->yvalue->intval < ST_TempStruct->Length) {
	 /* there's not enaugh space in matrix - enlarge it (both dimensions) */
	 ER_StackError("display: matrix is not large enaugh for structure (enlarged)\n",
		       Warning, nst, nst, nst);
	 for(; XExt->yvalue->intval * YExt->yvalue->intval < ST_TempStruct->Length;
	     (XExt->yvalue->intval)++, (YExt->yvalue->intval)++)
	   ;
       }
     else if(Form == ELLIPSE)   /* ellipse: L ~ pi * (3/2 * (a+b) - sqrt(a*b)), but a,b : radius */
       /* ellipse is not filled ==> perim. must be long enaugh to keep all units */
       if( 1.5 * M_PI * ( 1.5 * (XExt->yvalue->intval + YExt->yvalue->intval)/2 -
		   sqrt((float) XExt->yvalue->intval * YExt->yvalue->intval / 4)) + 1 < 
		   ST_TempStruct->Length) {
	 /* there's not enaugh space in ellipse - enlarge it (both dimensions) */
	 ER_StackError("display: ellipse is not large enaugh for structure (enlarged)\n",
		       Warning, nst, nst, nst);
	 for(; M_PI * ( 1.5 * (XExt->yvalue->intval + YExt->yvalue->intval)/2 -
		   sqrt((float) XExt->yvalue->intval * YExt->yvalue->intval/ 4)) < 
		   ST_TempStruct->Length;
	     (XExt->yvalue->intval)++, (YExt->yvalue->intval)++)
	   ;
       }
     ST_TempStruct->Topology.XDim = XExt->yvalue->intval;          /* assign x extension */
     ST_TempStruct->Topology.YDim = YExt->yvalue->intval;          /* assign y extension */
     ST_TempStruct->Topology.Form = Form;                        /* is MATRIX or ELLIPSE */
   }
   else 
     ST_TempStruct->Topology.Form = Undef;           /* undefined topology due to errors */
   PA_FreeParserStack(XExt);                       /* remove parser value stack elements */
   PA_FreeParserStack(YExt); 
 }






 /*****************************************************************************************
  Function:                     ST_CheckPosition
  Purpose: Checks wether a parser stack value is an integer >= 0 which can be used as
	   position for a structures topology specification or not.
  Returns: void.
  Side effects: Sets Argument->ytype if argument is not suitable to be used as position
		coordinate. 
 *****************************************************************************************/
 void ST_CheckPosition(Pos)
      ParserStackType *Pos;              /* parser value stack element holding position */
 {
   if(Pos->ytype == FLOATNUM) {          /* correctable error: position must be integer */
     ER_StackError("truncation: position coordinate of structure be integer\n", Warning,
		   nst, nst, nst);
     Pos->yvalue->intval = (int) Pos->yvalue->floatval;    /* truncate float to integer */
     Pos->ytype = INTEGER;
   }
   if(Pos->ytype != INTEGER) {                  /* type error: position must be integer */
     ER_StackError("type mismatch: illegal type for position coordinate\n", Serious,
		   nst, nst, nst);
     Pos->ytype = Undef;
   }
   else if(Pos->ytype == INTEGER)                        /* Pos is of correct data type */
     if(Pos->yvalue->intval < 0) {
     ER_StackError("position coordinate must be >= 0 and unique\n", Serious, nst, nst, nst);
     Pos->ytype = Undef;
   }
 }






 /*****************************************************************************************
  Function:                     ST_CheckExtension
  Purpose: Checks wether a parser stack value is an integer > 0 which can be used as
	   dimension for a structures topology specification or not.
  Returns: void.
  Side effects: Sets Argument->ytype if argument is not suitable to be used as dimension
		coordinate. 
 *****************************************************************************************/
 void ST_CheckExtension(Dim)
      ParserStackType *Dim;              /* parser value stack element holding dimension */
 {
   if(Dim->ytype == FLOATNUM) {          /* correctable error: dimension must be integer */
     ER_StackError("truncation: extension of structure area must be integer\n", Warning,
		   nst, nst, nst);
     Dim->yvalue->intval = (int) Dim->yvalue->floatval;    /* truncate float to integer */
     Dim->ytype = INTEGER;
   }
   if(Dim->ytype != INTEGER) {                  /* type error: dimension must be integer */
     ER_StackError("type mismatch: illegal type for dimension coordinate\n", Serious,
		   nst, nst, nst);
     Dim->ytype = Undef;
   }
   else if(Dim->ytype == INTEGER)                        /* Dim is of correct data type */
     if(Dim->yvalue->intval <= 0) {
     ER_StackError("dimension coordinate must be positive and not 0\n", Serious, nst, nst, nst);
     Dim->ytype = Undef;
   }
 }





 /*****************************************************************************************
  Function:                     ST_MakePos
  Purpose: Receives an integer and inserts it into a new ParserStackType record to put it
	   on the parsers value stack.
  Returns: &  ParserStackType holding integer.
 *****************************************************************************************/
 ParserStackType *ST_MakePos(Val)
      int Val;
 {
   ParserStackType *Result;
   Result = PA_GetParserStack();
   Result->ytype = INTEGER;
   Result->yvalue->intval = Val;
   return Result;
 }





 /*****************************************************************************************
  Function:                     ST_InsertMap
  Purpose: Inserts connections into an IRREGULAR structure. The structure is transformed
	   into CLUSTER. 
  Returns: void
  Side effects: Modifies ST_TempStruct->StructType, removes argument. 
 *****************************************************************************************/
void ST_InsertMap(MapId, Structure)
     ParserStackType *MapId;         /* should hold an identifier of type map int to int */
     UnitValueType *Structure;                  /* pointer to irregular structure record */
 {
   if(MapId) {
     STMT_GetIdValue(MapId);             /* if argument is an identifier - get its value */
     if(Structure->StructType == IRREGULAR) {
       if(MapId->ytype == RANGE) {                     /* definition like MapName[1..30] */
	 if(MapId->yvalue->range->FieldType == MAP)
	   if(MapId->yvalue->range->FieldPtr->Value->map->DType ==  XINT &&
	      MapId->yvalue->range->FieldPtr->Value->map->RType ==  XINT) {
	     /* correct map constant - insert connections */
	     Structure->Map.Low = MapId->yvalue->range->FieldPtr->Value->map->mLines +
	       MapId->yvalue->range->Lower;
	     Structure->Map.LineNo = MapId->yvalue->range->Upper - 
	       MapId->yvalue->range->Lower + 1;
	   }
	   else   /* map is not of type INT to INT */
	     ER_StackError("type mismatch: map INT to INT expected\n",Serious,nst,nst,nst);
	 else     /* MapId holds a field range */
	   ER_StackError("type mismatch: map constant expected\n", Serious, nst, nst, nst);
	 (void) M_free( (char*) MapId->yvalue->range);         /* remove range type record */
       }
       else if(MapId->ytype != Undef)                   /* mapId must hold an identifier */
	 if(MapId->ytype == MAP)
	   if(MapId->yvalue->map->DType ==  XINT && 
	      MapId->yvalue->map->DType ==  XINT) {
	     /* insert connections -- lower bound is first map line (with index 0) */
	     Structure->Map.Low = MapId->yvalue->map->mLines;
	     Structure->Map.LineNo = MapId->yvalue->map->mSize;
	   }
	   else   /* map is not of type XINT to XINT */
	     ER_StackError("type mismatch: map XINT to XINT expected\n",Serious,nst,nst,nst);
	 else     /* MapId holds a field range */
	   ER_StackError("type mismatch: map constant expected\n", Serious, nst, nst, nst);
     }
     else   /* structure type is not IRREGULAR - cannot specifiy connections through map */
       ER_StackError("syntax error: map specification for %s ignored\n", Serious,
		     STR_TypeString(Structure->StructType), nst, nst);
   }
   else       /* MapId == NULL - there must have been some error in the range definition */
     if(Structure->StructType == IRREGULAR) {
       ER_StackError("type mismatch: missing map specification for structure\n", Serious,
		     nst, nst, nst); 
     }
 }





 /*****************************************************************************************
  Function:                     ST_BuildRangeSelection
  Purpose: Build a ParserStackType record holding an array or map range in a RangeType
	   record. Check if range delimiters ly within the array or map bounds.
  Returns: Pointer to new ParserStackType record.
  Side effects: Removes all argument records.
 *****************************************************************************************/
 ParserStackType *ST_BuildRangeSelection(Field, Low, High)
      ParserStackType *Field;              /* pointer to PST record holding array or map */
      ParserStackType *Low;                /* lower range delimiter (is an integer >= 0) */
      ParserStackType *High;               /* upper range delimiter (is an integer >= 0) */
 {
   ParserStackType *Result = NULL;
   if(Low->ytype != Undef && High->ytype != Undef)       /* range delimiters are correct */
     if(High->yvalue->intval >= Low->yvalue->intval){ /* upper bound must not be < lower */
       if(Field->ytype != Undef)                         /* argument evaluated correctly */
	 if(Field->ytype == IDENTIFIER)                     /* argument is an identifier */
	   if ( ! Field->yvalue->var->UndefType) /* identifier has been defined, already */
	     if(Field->yvalue->var->Type == MAP)           /* identifier is map constant */
	       if(Field->yvalue->var->Value->map->mSize > High->yvalue->intval) {
		 Result = PA_GetParserStack();           /* range lies within map bounds */
		 Result->ytype = RANGE;                /* build range description record */
		 Result->yvalue->range = (RangeType*) M_alloc(sizeof(RangeType));
		 Result->yvalue->range->FieldType = MAP;
		 Result->yvalue->range->FieldPtr = Field->yvalue->var;
		 Result->yvalue->range->Lower = Low->yvalue->intval;
		 Result->yvalue->range->Upper = High->yvalue->intval;
	       }
	       else                          /* range is out of bounds (upper delimiter) */
		 ER_StackError("empty range: upper delimiter out of bounds\n", Serious,
			       nst, nst, nst);
	     else if(Field->yvalue->var->Type == ArrInt ||
		     Field->yvalue->var->Type == ArrFloat ||
		     Field->yvalue->var->Type == ArrString ||
		     Field->yvalue->var->Type == ArrStruct ||
		     Field->yvalue->var->Type == STRUCTURE)    /* identifier is an array */
	       if(Field->yvalue->var->Type != STRUCTURE && 
		  Field->yvalue->var->Value->array->aSize > High->yvalue->intval ||
		  Field->yvalue->var->Type == STRUCTURE &&
		  Field->yvalue->var->Value->structure->Length  > High->yvalue->intval) {
		 Result = PA_GetParserStack();         /* range lies within array bounds */
		 Result->ytype = RANGE;                /* build range description record */
		 Result->yvalue->range = (RangeType*) M_alloc(sizeof(RangeType));
		 Result->yvalue->range->FieldType = Field->yvalue->var->Type;
		 Result->yvalue->range->FieldPtr = Field->yvalue->var;
		 Result->yvalue->range->Lower = Low->yvalue->intval;
		 Result->yvalue->range->Upper = High->yvalue->intval;
	       }
	       else                          /* range is out of bounds (upper delimiter) */
		 ER_StackError("empty range: upper delimiter out of bounds\n", Serious,
			       nst, nst, nst);
	     else                     /* identifier is some other constant => type error */
	       ER_StackError("type mismatch: array or map constant expected\n", Serious,
			     nst, nst, nst);
	   else                       /* identifier has undefined type and value entries */
	     ER_StackError("unknown identifier: '%s'\n", Serious,
			   STR_Save(Field->yvalue->var->Name), nst, nst);
	 else                            /* constant must be an identifier => type error */
	   ER_StackError("type mismatch: array or map constant expected\n", Serious,
			 nst, nst, nst);
     }
     else
       ER_StackError("empty range: lower bound > upper bound\n", Serious, nst, nst, nst);
   if( ! Result) {
     Result = PA_GetParserStack();  
     Result->ytype = Undef; 
   }
   /* do not remove Field - may be ST_ActMap which is reused */
   PA_FreeParserStack(Low);
   PA_FreeParserStack(High);
   return Result;
 }





 /*****************************************************************************************
  Function:                     ST_InsertMapConnections
  Purpose: Generate explicit connections among those units of ST_TempStruct, whose indices
	   are related.
  Returns: void.
 *****************************************************************************************/
 void ST_InsertMapConnections(FirstLine, NoOfLines, Structure)
     MapLineType *FirstLine;                                  /* first line of map range */
     int NoOfLines;                                         /* no. of lines in map range */
     UnitValueType *Structure;                                    /* irregular structure */
     
 {
   register MapLineType *ActLine;                           /* points to actual map line */
   register int *ActDom;                  /* pointer to actual domain value (INTEGER !!) */
   register int *ActRange;                 /* pointer to actual range value (INTEGER !!) */
   register int DomInt;
   register int RangeInt;

   for(ActLine = FirstLine; ActLine < FirstLine + NoOfLines; ActLine ++)   /* scan lines */
     if(ActLine->Domain->aRange)                /* domain has been defined through range */
       for(DomInt = *ActLine->Domain->aValue.aint;
	   DomInt < *(ActLine->Domain->aValue.aint) + ActLine->Domain->aSize;
	   DomInt ++) 
	 if(ActLine->Range->aRange)              /* range has been defined through range */
	   for(RangeInt = *ActLine->Range->aValue.aint;
	       RangeInt < *(ActLine->Range->aValue.aint) + ActLine->Range->aSize;
	       RangeInt ++)                    /* insert connection, maybe bidirectional */
	      ST_InsertPair(DomInt, RangeInt, Structure);  
	 else                                 /* range array has been defined explicitly */
	   for(ActRange = ActLine->Range->aValue.aint; 
	       ActRange < ActLine->Range->aValue.aint + ActLine->Range->aSize; 
	       ActRange ++)                    /* insert connection, maybe bidirectional */
	     ST_InsertPair(DomInt, *ActRange, Structure); 
     else                                    /* domain array has been defined explicitly */
       for(ActDom = ActLine->Domain->aValue.aint; 
	   ActDom < ActLine->Domain->aValue.aint + ActLine->Domain->aSize; 
	   ActDom ++)
	 if(ActLine->Range->aRange)              /* range has been defined through range */
	   for(RangeInt = *ActLine->Range->aValue.aint;
	       RangeInt < *(ActLine->Range->aValue.aint) + ActLine->Range->aSize;
	       RangeInt ++)                    /* insert connection, maybe bidirectional */
	     ST_InsertPair(*ActDom, RangeInt, Structure); 
	 else                                 /* range array has been defined explicitly */
	   for(ActRange = ActLine->Range->aValue.aint; 
	       ActRange < ActLine->Range->aValue.aint + ActLine->Range->aSize; 
	       ActRange ++)                    /* insert connection, maybe bidirectional */
	     ST_InsertPair(*ActDom, *ActRange, Structure); 
 }





 /*****************************************************************************************
  Function:                     ST_InsertPair
  Purpose: Insert a connection between two units into list of explicit connections.
  Returns: void.
 *****************************************************************************************/
 void ST_InsertPair(Source, Target, Structure)
     int Source;                       /* first unit - source unit if connections are PT */
     int Target;                        /* 2nd. unit - target unit if connections are PT */
     UnitValueType *Structure;                                    /* irregular structure */
 {
   float Weight;                                                    /* connection weight */

   /* map refers to structure indexes, add no. of first unit to get the real unit no. */
   Source += Structure->Begin; 
   Target += Structure->Begin;
   /* check wether source and target units are structure members or not */
   if(Source >= Structure->Begin &&  
      Source < Structure->Begin + Structure->Length && /* source is in structure */
      Target >= Structure->Begin &&  
      Target < Structure->Begin + Structure->Length) { /* target is in structure */
     Weight = ((int) Structure->Weight == RANDOM) ?        /* if weight is not set - */
       GETRANDOM  : Structure->Weight;                      /* generate random value */
      switch(Structure->Direction) {
      case PT:
	if(Structure->Through)                         /* structure units have sites */
	  CONN_InsertConnection(Target, Structure->Through->Value->site, Source, Weight);
	else 
	  CONN_InsertConnection(Target, NULLSite, Source, Weight);
	break;
      case PF:
	if(Structure->Through)                         /* structure units have sites */
	  CONN_InsertConnection(Source, Structure->Through->Value->site, Target, Weight);
	else 
	  CONN_InsertConnection(Source, NULLSite, Target, Weight);
	break;
      case PTF:
	if(Structure->Through)  {                      /* structure units have sites */
	  CONN_InsertConnection(Target, Structure->Through->Value->site, Source, Weight);
	  CONN_InsertConnection(Source, Structure->Through->Value->site, Target, Weight);
	}
	else {
	  CONN_InsertConnection(Target, NULLSite, Source, Weight);
	  CONN_InsertConnection(Source, NULLSite, Target, Weight);
	}
	break;
      }
   }
 }






 /*****************************************************************************************
  Function:                     ST_FinishUpStructure
  Purpose: Takes ST_TempStruct and builds a ParserStackType record for it. ST_TempStruct
	   holds a linear structure.
  Returns: &ParserStackType.
  Side effects: ST_TempStruct is reset to NULL;
 *****************************************************************************************/
 ParserStackType *ST_FinishUpStructure()
 {
   ParserStackType *Result;
   Result = PA_GetParserStack();
   if(ST_TempStruct) {
     Result->ytype = STRUCTURE;
     Result->yvalue->structure = ST_TempStruct;
     ST_TempStruct = NULL;
   }
   else
     Result->ytype = Undef;
   return Result;
 }






 /*****************************************************************************************
  Function:                     ST_MakeSingleUnit
  Purpose: Takes ST_TempStruct and builds a ParserStackType record for it. ST_TempStruct
	   holds a single unit.
  Returns: &ParserStackType.
  Side effects: ST_TempStruct is reset to NULL;
 *****************************************************************************************/
 ParserStackType *ST_MakeSingleUnit()
 {
   ParserStackType *Result;
   Result = PA_GetParserStack();
   Result->ytype = UNIT;
   ST_TempStruct->Length = 1;
   Result->yvalue->structure = ST_TempStruct;
   ST_TempStruct = NULL;
   return Result;
 }






 /*****************************************************************************************
  Function:                     ST_MakeArrayOfStructures
  Purpose: Takes a structure as first argument and builds an array of copies of this
	   structure. The 2nd. argument defines the length of this array.
	   For each copy of the structure, a new position is generated.
  Returns: &ParserStackType holding new array of structures.
 *****************************************************************************************/
 ParserStackType *ST_MakeArrayOfStructures(Structure, Length, Copy)
     ParserStackType *Structure;             /* holds UnitValueType record for structure */
     ParserStackType *Length;                                /* holds integer value >= 0 */
     short Copy;                              /* TRUE <==> all array elements are copied */
 {
   ParserStackType *Result;          /* &ParserStackType holding new array of structures */
   int XPos, YPos;                             /* save old x and y position of structure */
   register UnitValueType **ActStruct;  /* actual insert position in array of structures */
   register int ActInd;        /* index of actual insert position in array of structures */

   if(Structure->ytype != Undef)                           /* structure has been correct */
     if(Length->ytype != Undef)    /* check size - size must be > 0, is int >=0, already */
       if( ! Length->yvalue->intval)   
	 ER_StackError("struct: illegal null sized structure\n", Serious, nst, nst, nst);
       else  {
	 Result = PA_GetParserStack();
	 Result->ytype = ArrStruct;
	 Result->yvalue->array = (ArrayValueType *) M_alloc(sizeof(ArrayValueType));
	 Result->yvalue->array->aRange = FALSE;
	 Result->yvalue->array->Named = FALSE;
	 Result->yvalue->array->aSize = Length->yvalue->intval;           /* insert size */
	 Result->yvalue->array->aValue.astruct = (UnitValueType **)    /* alloc elements */
	   M_alloc((unsigned) (Length->yvalue->intval * sizeof(UnitValueType*)));
	 /* save structure positions - they are modified by ST_CopyUnitValue */
	 XPos = Structure->yvalue->structure->Topology.XCenter;
	 YPos = Structure->yvalue->structure->Topology.YCenter;
	 if(Copy)                          /* make a copy of the original structure, too */
	   *(Result->yvalue->array->aValue.astruct) = 
	     ST_CopyUnitValue(Structure->yvalue->structure);
	 else                                        /* original structure is not copied */
	   *(Result->yvalue->array->aValue.astruct) = Structure->yvalue->structure;
	 /* insert copies of structure into array fields */
	 for(ActStruct = Result->yvalue->array->aValue.astruct+1, ActInd = 1; 
	     ActInd < Length->yvalue->intval; ActStruct ++, ActInd++)
	   *ActStruct = ST_CopyUnitValue(Structure->yvalue->structure);
	 /* reset structure positions - they have been modified by ST_CopyUnitValue */
	 Structure->yvalue->structure->Topology.XCenter = XPos;
	 Structure->yvalue->structure->Topology.YCenter = YPos;
	 if(Structure->yvalue->structure->StructType == IRREGULAR &&
	    Structure->yvalue->structure->Map.Low)         /* correct map specification */
	   ST_InsertMapConnections(Structure->yvalue->structure->Map.Low, 
				   Structure->yvalue->structure->Map.LineNo, 
				   Structure->yvalue->structure);
	 return Result;
       }
     else {                                /* previous error in definition of array size */
       ST_RemoveStructure(Structure->yvalue->structure);
       PA_FreeParserStack(Structure);
       return Length;
     }
   PA_FreeParserStack(Length);
   return Structure;
 }






 /*****************************************************************************************
  Function:                     ST_RemoveStructure
  Purpose: Removes a structure.
  Returns: void.
  Side effects: ! :-) !
 *****************************************************************************************/
 void ST_RemoveStructure(Str)
      UnitValueType *Str;
 {
   UnitType **Expl;            /* to scan and eventually remove explicitly defined units */

   for(Expl = Str->ExplUnits; Expl && Expl < Str->ExplUnits + Str->Length; Expl ++)
     if(*Expl) {                                     /* explicitly defined unit - remove */
       if( ! (*Expl)->FType)
	 if((*Expl)->FParam.VarFT != Str->UnitDef->FParam.VarFT)   /* extra fType record */
	   (void) M_free((char*) (*Expl)->FParam.VarFT);   /* remove explicit fType record */
       (void) M_free((char*) *Expl);                        /* remove explicit unit itself */
     }
   if( ! Str->UnitDef->FType)                  /* remove an eventual common fType record */
     (void) M_free((char*) Str->UnitDef->FParam.VarFT);
   (void) M_free((char*) Str->UnitDef);                       /* remove unit specification */
   (void) M_free((char*) Str);                                         /* remove structure */
 }






/*****************************************************************************************
  Function:                          ST_StackType
  Purpose: Stack and count site and unit types.
  Returns: void.
  Side effects: ST_UnitTypes, ST_SiteTypes, UnitTypeCtr, SiteTypeCtr.
*****************************************************************************************/
void ST_StackType(Id, Type)
     ParserStackType *Id;
     SymbolType Type;
{
  if(Type == UNITTYPE) {
    UnitTypeCtr ++;
    Id->ynext = UnitTypes;
    UnitTypes = Id;
  }
  else {   /* Type == SITE */
    SiteTypeCtr ++;
    Id->ynext = SiteTypes;
    SiteTypes = Id;
  }
}





/*****************************************************************************************
 Function:                     ST_UpdateConnectionCounter
 Purpose: Increment ConnectionCtr for new structure (implicit connections).
*****************************************************************************************/
void ST_UpdateConnectionCounter(Structure)
     UnitValueType *Structure;  
{
  switch(Structure->StructType) {
  case IRREGULAR:
  case CLUSTER:
  case UNIT:
    break;
  case CLIQUE:
    ConnectionCtr += (Structure->Direction == PTF) ?
      2 * (Structure->Length * (Structure->Length-1)) :
	Structure->Length * (Structure->Length-1);

    break;
  case CHAIN:
  case STAR:
    ConnectionCtr += (Structure->Direction == PTF) ?
      2 * (Structure->Length-1) : Structure->Length-1;
    break;
  case RING:
    ConnectionCtr += (Structure->Direction == PTF) ?
      2 * Structure->Length : Structure->Length;
    break;
  }
  return;
}
    
    




/*****************************************************************************************
 Function:                     ST_InsertNetComponent
 Purpose: Insert an identifier into list of net components. Identifier must be of type 
          ArrStruct, UNIT or STRUCTURE.
 Returns: void.
 Side effects: NetComponents.
*****************************************************************************************/
void ST_InsertNetComponent(Id)
     SymTabLineType *Id;                         /* & Symbol table entry for identifier */
{
  static SymTabLineType **NewEntry = NULL;           /* pointer to next free list entry */
  static SymTabLineType **ListEnd;                           /* pointer to  end of list */

  if( ! NetComponents)  {                    /* first entry into list of net components */
    NetComponents = (SymTabLineType **) M_alloc((unsigned) sizeof(SymTabLineType *) *
						NoNetComponents); 
    NewEntry = NetComponents;                             /* set pointer to first entry */
    *(NewEntry++) = Id;                               /* insert entry at first position */
    ListEnd = NetComponents + NoNetComponents;                     /* mark end of table */
  }
  else if(NewEntry < ListEnd)                                     /* table not full yet */
    *(NewEntry++) = Id;                 /* insert entry at actual position */
  else  {    /* table is full - copy all entries into bigger table and insert new entry */
    NewEntry = NetComponents;                                        /*  save old table */
    NoNetComponents += IncrNetComp;
    /* increment table length  and allocate table of new size */
    NetComponents = (SymTabLineType **) realloc((char *) NetComponents, 
						(unsigned) sizeof(SymTabLineType *) *
						NoNetComponents);
    NewEntry = NetComponents + NoNetComponents - IncrNetComp;        /* next free entry */
    ListEnd = NetComponents + NoNetComponents;                     /* mark end of table */
    *(NewEntry++) = Id;                 /* insert entry at actual position */
  }
  NetCompCtr ++;                                            /* increment no. of entries */
}







/*****************************************************************************************
 Function:                     ST_InsertExplValues
 Purpose: Insert explicit values of an array into structure units. 
 Returns: void.
 Side effects: Modifies structure units and removes the values array if there's no name
               for it. 
*****************************************************************************************/
void ST_InsertExplValues(Comp, Field)
     SymbolType Comp;                                  /* unit parameter to be modified */
     ParserStackType *Field;   /* array containing values for structure unit parameters */
{
  register UnitType **ActUnit;                    /* scan list of explicit unit records */
  register float *ActValF;                      /* scan list of float activation values */
  register int *ActValI;                      /* scan list of integer activation values */
  register char **NameVal;                                   /* scan list of unit names */
  register ArrayElemType *First;                 /* pointer to first value list element */
  register ArrayElemType *Last;                   /* pointer to last value list element */
  short IntFlg;                                 /* TRUE <==> Field is array of integers */

  STMT_GetIdValue(Field);
  IntFlg = FALSE;                              /* set if activation values are integers */
  if(Field && Field->ytype != Undef) {                            /* no previous errors */
    First = (ArrayElemType *) M_alloc(sizeof(ArrayElemType));   
    Last = (ArrayElemType *) M_alloc(sizeof(ArrayElemType));   
    if(ST_IsCorrectParamList(Comp, Field, &IntFlg)) {   /* length and data types are ok */
      ST_GetFirstValue(Field, First);                     /* get pointer to first value */
      ST_GetLastValue(Field, Last);                        /* get pointer to last value */
      switch(Comp) {
      case NAME:                 /* unit name is set explicitly, field contains strings */
	for(ActUnit = ST_TempStruct->ExplUnits, NameVal = First->astring; 
	    NameVal <= Last->astring; NameVal++, ActUnit++ ) { /* scan units and values */
	  OUT_SetUNameLength(*NameVal);
	  if(*ActUnit)       /* there has been an explicit value for this unit, already */
	    (*ActUnit)->UName = *NameVal;
	  else {       /* first explicit value for this unit - allocate a record for it */
	    ((*ActUnit) = ST_SingleExplicitUnit(ST_TempStruct->UnitDef))->UName = *NameVal;
	    (*ActUnit)->UnitNo = ST_TempStruct->Begin + NameVal - (First->astring);
	  }
	}
	break;
      case ACT:      /* unit activation value is set explicitly, field contains strings */
	if(IntFlg)                      /* actvations out of an array of integer values */
	  for(ActUnit = ST_TempStruct->ExplUnits, ActValI = First->aint; 
	      ActValI <= Last->aint; ActValI++, ActUnit++ ) {    /* scan units and values */
	    if(*ActValI < (int) -1.0 || *ActValI > (int) 1.0) {    /* illegal act value */
	      ER_StackError("activation value not in [-1.0, 1.0]\n", Warning, nst, nst, nst);
	      *ActValI = *ActValI / abs(*ActValI);
	    }
	    if(*ActUnit)       /* there has been an explicit value for this unit, already */
	      (*ActUnit)->Act = *ActValI;
	    else {       /* first explicit value for this unit - allocate a record for it */
	      ((*ActUnit) = ST_SingleExplicitUnit(ST_TempStruct->UnitDef))->Act = *ActValI;
	      (*ActUnit)->UnitNo = ST_TempStruct->Begin + ActValI - (First->aint);
	    }
	  }
	else                              /* actvations out of an array of float values */
	  for(ActUnit = ST_TempStruct->ExplUnits, ActValF = First->afloat; 
	      ActValF <= Last->afloat; ActValF++, ActUnit++ ) {    /* scan units and values */
	    if(*ActValF < (int) -1.0 || *ActValF > (int) 1.0) {    /* illegal act value */
	      ER_StackError("activation value not in [-1.0, 1.0]\n", Warning, nst, nst, nst);
	      *ActValF = (*ActValF < 0)? -1.0 : 1.0;
	    }
	    if(*ActUnit)       /* there has been an explicit value for this unit, already */
	      (*ActUnit)->Act = *ActValF;
	    else {       /* first explicit value for this unit - allocate a record for it */
	      ((*ActUnit) = ST_SingleExplicitUnit(ST_TempStruct->UnitDef))->Act = *ActValF;
	      (*ActUnit)->UnitNo = ST_TempStruct->Begin + ActValF - (First->afloat);
	    }
	  }
      break;
    default:
      (void) printf("******bug: illegal parameter type in ST_InsertExplValues: %s\n",
		    STR_TypeString(Comp));
    }
    (void) M_free((char*) First);
    (void) M_free((char*) Last);
    if(Field->ytype == RANGE) 
      (void) M_free((char*) Field->yvalue->range);
    else   /* must be array */
      if( ! Field->yvalue->array->Named) {
	(void) M_free((char*) Field->yvalue->array->aValue.aint);
	(void) M_free((char*) Field->yvalue->array);
      }
    }
  }
  if(Field) PA_FreeParserStack(Field);
}






/*****************************************************************************************
 Function:                     ST_IsCorrectParamList
 Purpose: Checks if a parameter list defined through an array or a range fits into the 
          structure of ST_TempStruct. Checks data type of range elements and unit parameter.
 Returns: TRUE <==> ok, else FALSE.
 Side effects: Call to STMT_GetIdValue.
*****************************************************************************************/
short ST_IsCorrectParamList(Comp, Field, IntFlg)
     SymbolType Comp;                                  /* unit parameter to be modified */
     ParserStackType *Field;   /* array containing values for structure unit parameters */
     short *IntFlg;                  /* *IntFlg == TRUE <==> Field is array of integers */
{
  register short Error;

  Error = FALSE;
  STMT_GetIdValue(Field);
  if(Comp == NAME) {                  /* unit component is name - values must be string */
    if(Field->ytype == RANGE && Field->yvalue->range->FieldType != ArrString &&
       Field->ytype != ArrString) {                                       /* type error */
      ER_StackError("type mismatch: array of strings expected for unit names\n", Serious,
		    nst, nst, nst);
      Error = TRUE;
    }
  }
  else if(Comp == ACT)
    if(Field->ytype == RANGE && Field->yvalue->range->FieldType != ArrFloat)       
      /* integer values or type error */
      if(Field->ytype == RANGE && Field->yvalue->range->FieldType == ArrInt)
	 if(Field->yvalue->range->FieldPtr->Value->array->aRange) {   /* range definition */
	   ER_StackError("senseless range definition for act values ignored\n", Warning,
			 nst, nst, nst);
	   Error = TRUE;
	 }
	 else
	   *IntFlg = TRUE;
      else {                                                                /* type error */
	ER_StackError("type mismatch: array of strings expected for unit names\n", Serious,
		      nst, nst, nst);
	Error = TRUE;
      }
    else if(Field->ytype == ArrInt)              /* field may be array of integer values */
      *IntFlg = TRUE;
  /* data types match -- check if field length == structure length */
  if(Field->ytype == RANGE && 
     Field->yvalue->range->Upper - Field->yvalue->range->Lower + 1 != ST_TempStruct->Length  ||
     (Field->ytype == ArrString || Field->ytype == ArrFloat || Field->ytype == ArrInt) &&
     Field->yvalue->array->aSize != ST_TempStruct->Length) {
    ER_StackError("no. of explicit values does not match no. of units\n", Serious, nst, nst, nst);
    Error = TRUE;
  }
  return ( ! Error);
}





/*****************************************************************************************
 Function:                     ST_GetFirstValue
 Purpose: Get first array or array range value.
 Returns: void
 Side effects: Modifies *Val; 
*****************************************************************************************/
void ST_GetFirstValue(Field, Val)
     ParserStackType *Field;
     ArrayElemType *Val;
{
  switch(Field->ytype) {
  case ArrInt:        /* cannot be defined through range */
    Val->aint = Field->yvalue->array->aValue.aint;
    break;
  case ArrFloat:
    Val->afloat = Field->yvalue->array->aValue.afloat;
    break;
  case ArrString:
    Val->astring = Field->yvalue->array->aValue.astring;
    break;
  case RANGE:
    switch(Field->yvalue->range->FieldType) {
    case ArrInt:      /* cannot be defined through range */
      Val->aint = (Field->yvalue->range->FieldPtr->Value->array->aValue).aint +
	(Field->yvalue->range->Lower);
      break;
    case ArrFloat:
      Val->afloat = Field->yvalue->range->FieldPtr->Value->array->aValue.afloat +
	Field->yvalue->range->Lower;
      break;
    case ArrString:
      Val->astring = Field->yvalue->range->FieldPtr->Value->array->aValue.astring +
	Field->yvalue->range->Lower;
      break;
    default:
      (void) printf("******bug: illegal range type in ST_GetFirstValue: %s\n",
		    STR_TypeString(Field->ytype));
      exit(Bug);
    }
    break;
  case LONGSTRING:
  case SIMPLESTRING:
    Val->astring = &(Field->yvalue->string);
    break;
  case INTEGER:
    Val->aint = &(Field->yvalue->intval);
    break;
  case FLOATNUM:
    Val->afloat = &(Field->yvalue->floatval);
    break;
  default:
    (void) printf("******bug: illegal field type in ST_GetFirstValue: %s\n",
		  STR_TypeString(Field->ytype));
    exit(Bug);
  }
}






/*****************************************************************************************
 Function:                     ST_GetLastValue
 Purpose: Get last array or array range value.
 Returns: void
 Side effects: Modifies *Val; 
*****************************************************************************************/
void ST_GetLastValue(Field, Val)
     ParserStackType *Field;
     ArrayElemType *Val;
{
  switch(Field->ytype) {
  case ArrInt:        /* cannot be defined through range */
    Val->aint = Field->yvalue->array->aValue.aint + Field->yvalue->array->aSize - 1;
    break;
  case ArrFloat:
    Val->afloat = Field->yvalue->array->aValue.afloat + Field->yvalue->array->aSize - 1;
    break;
  case ArrString:
    Val->astring = Field->yvalue->array->aValue.astring + Field->yvalue->array->aSize - 1;
    break;
  case RANGE:
    switch(Field->yvalue->range->FieldType) {
    case ArrInt:      /* cannot be defined through range */
      Val->aint = Field->yvalue->range->FieldPtr->Value->array->aValue.aint +
	Field->yvalue->range->Upper;
      break;
    case ArrFloat:
      Val->afloat = Field->yvalue->range->FieldPtr->Value->array->aValue.afloat +
	Field->yvalue->range->Upper;
      break;
    case ArrString:
      Val->astring = Field->yvalue->range->FieldPtr->Value->array->aValue.astring +
	Field->yvalue->range->Upper;
      break;
    default:
      (void) printf("******bug: illegal range type in ST_GetLastValue: %s\n",
		    STR_TypeString(Field->ytype));
      exit(Bug);
    }
    break;
  case LONGSTRING:
  case SIMPLESTRING:
    Val->astring = &(Field->yvalue->string);
    break;
  case INTEGER:
    Val->aint = &(Field->yvalue->intval);
    break;
  case FLOATNUM:
    Val->afloat = &(Field->yvalue->floatval);
    break;
  default:
    (void) printf("******bug: illegal field type in ST_GetLastValue: %s\n",
		  STR_TypeString(Field->ytype));
    exit(Bug);
  }
}




/*****************************************************************************************
 Function:                      ST_FinishUpSubnet()
 Purpose: Removes a symbol table and an input stream from  their correspondent stacks and 
          inserts them into a parser value stack record (will be inserted into outer
	  symbol table).
 Returns: pointer to  parser value stack record holding subnet symbol table.
 Side effects:  Modifies SymTabPtr.
*****************************************************************************************/
ParserStackType *ST_FinishUpSubnet(Res)
  ParserStackType *Res;                                /* will hold subnet symbol table */
{
  if( ! Res->ytype) {                  /*  parsing of subnet has returned 0, no errors  */
    Res->ytype  =  SUBNET;
    Res->yvalue->subnet = SymTabPtr;
    SymTabPtr = SymTabPtr->NextSymTab;
    Res->yvalue->subnet->NextSymTab = NULL;
  }
  else 
    Res->ytype = Undef;
    return Res;
}







#ifdef MAPTEST   /* functions to print map constants */


void ST_PrintMapLine(Line, Type)
     ArrayValueType *Line;
     SymbolType Type;
{
  register int *iVal;
  register float *fVal;
  register char **sVal;

  switch(Type) {
  case XINT:
    if(Line->aRange) {
      (void) printf("  %d .. %d", *Line->aValue.aint, (*Line->aValue.aint) + Line->aSize-1);
    }
    else 
      for(iVal = Line->aValue.aint; iVal < Line->aValue.aint + Line->aSize; iVal ++)
	(void) printf("  %d", *iVal);
    break;
  case XFLOAT:
    for(fVal = Line->aValue.afloat; fVal < Line->aValue.afloat + Line->aSize; fVal ++)
      (void) printf("  %8.5", *fVal);
    break;
  case STRING:
    for(sVal = Line->aValue.astring; sVal < Line->aValue.astring + Line->aSize; sVal ++)
      (void) printf("  %s", *sVal);
    break;
  default :
    (void) printf("******bug: type is %s in ST_PrintMapLine\n", STR_TypeString(Type));
  }
}






void ST_PrintMap(Map)
     ParserStackType *Map;
{
  register MapLineType *Line;
  
  if(Map && Map->ytype != Undef) {
    for(Line = Map->yvalue->map->mLines, (void) printf("map lines: \n");
	Line < Map->yvalue->map->mLines + Map->yvalue->map->mSize;
	Line ++) {
      ST_PrintMapLine(Line->Domain, Map->yvalue->map->DType);
      (void) printf(" : ");
      ST_PrintMapLine(Line->Range, Map->yvalue->map->RType);
      (void) printf("\n");
      (void) printf("confirm:"); scanf("*\n"); (void) printf("\n");
    }
  }
  else 
    (void) printf("map lines : \n (null, undefined) \n");
}

#endif    
