/*****************************************************************************
  FILE              : co_cFuncs.c
  SHORTNAME         : cFuncs
  VERSION           : 2.0

  PURPOSE           : contains functions to translate network activation, output, and learning
                      functions into C.
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_cFuncs.h"

struct FuncListStruct *FUNC_RemoveTab()
{ return NULL; }

static FuncListType *SiteFuncList = NULL;             /* ptr. to list of site functions */
static FuncListType *ActFuncList = NULL;        /* ptr. to list of activation functions */
static FuncListType *OutFuncList = NULL;            /* ptr. to list of output functions */

void CF_IncludeNewFunctions()
{}





/*****************************************************************************************
 Function:                     CF_MarkFunction
 Purpose: Insert a new function into list of functions.
 Returns: void..
 Side effects: Modifies lists of functions.
*****************************************************************************************/
CF_MarkFunction(Function, FuncType)
     ParserStackType *Function;
     SymbolType FuncType;
{
  switch(FuncType) {                                 /* select list to insert function */
  case SITEFUNC:
    CF_InsertSiteFuncList(Function);
    break;
  case ACTFUNC:
    CF_InsertActFuncList(Function);
    break;
  case OUTFUNC:
    CF_InsertOutFuncList(Function);
    break;
  }
}




/*****************************************************************************************
 Function:                     CF_InsertSiteFuncList
 Purpose: Insert a new function into list of site functions.
 Returns: void..
 Side effects: Modifies lists of site functions.
*****************************************************************************************/
void CF_InsertSiteFuncList(Function)
     ParserStackType *Function;
{
  register FuncListType *Act;
  
  if( ! SiteFuncList) {                             /* no site functions defined before */
    SiteFuncList = (FuncListType *) M_alloc(sizeof(FuncListType));
    SiteFuncList->FuncName = Function->yvalue->var->Name;
    SiteFuncList->NextFunc = NULL;
  }
  else {
    for(Act = SiteFuncList; Act->NextFunc; Act = Act->NextFunc)
      if( ! strcmp(Act->FuncName, Function->yvalue->var->Name))
	return;         /* function already in list -- strcmp necessary for subnets !! */
    if( ! Act->NextFunc && strcmp(Act->FuncName, Function->yvalue->var->Name)) {/* new */
      Act->NextFunc = (FuncListType *) M_alloc(sizeof(FuncListType));
      Act->NextFunc->FuncName = Function->yvalue->var->Name;
      Act->NextFunc->NextFunc = NULL;
    }
  }
  return;
}




/*****************************************************************************************
 Function:                     CF_InsertActFuncList
 Purpose: Insert a new function into list of Act functions.
 Returns: void..
 Side effects: Modifies lists of activation functions.
*****************************************************************************************/
void CF_InsertActFuncList(Function)
     ParserStackType *Function;
{
  register FuncListType *Act;
  
  if( ! ActFuncList) {                        /* no activation functions defined before */
    ActFuncList = (FuncListType*) M_alloc(sizeof(FuncListType));
    ActFuncList->FuncName = Function->yvalue->var->Name;
    ActFuncList->NextFunc = NULL;
  }
  else {
    for(Act = ActFuncList; Act->NextFunc; Act = Act->NextFunc)
      if( ! strcmp(Act->FuncName, Function->yvalue->var->Name))
	return;         /* function already in list -- strcmp necessary for subnets !! */
    if( ! Act->NextFunc && strcmp(Act->FuncName, Function->yvalue->var->Name)) {    /* new */
      Act->NextFunc = (FuncListType*) M_alloc(sizeof(FuncListType));
      Act->NextFunc->FuncName = Function->yvalue->var->Name;
      Act->NextFunc->NextFunc = NULL;
    }
  }
  return;
}




/*****************************************************************************************
 Function:                     CF_InsertOutFuncList
 Purpose: Insert a new function into list of output functions.
 Returns: void..
 Side effects: Modifies lists of output functions.
*****************************************************************************************/
void CF_InsertOutFuncList(Function)
     ParserStackType *Function;
{
  register FuncListType *Act;

  if( ! OutFuncList) {                            /* no output functions defined before */
    OutFuncList = (FuncListType*) M_alloc(sizeof(FuncListType));
    OutFuncList->FuncName = Function->yvalue->var->Name;
    OutFuncList->NextFunc = NULL;
  }
  else {
    for(Act = OutFuncList; Act->NextFunc; Act = Act->NextFunc)
      if( ! strcmp(Act->FuncName, Function->yvalue->var->Name))
	return;         /* function already in list -- strcmp necessary for subnets !! */
    if( ! Act->NextFunc && strcmp(Act->FuncName, Function->yvalue->var->Name)) {/* new */
      Act->NextFunc = (FuncListType*) M_alloc(sizeof(FuncListType));
      Act->NextFunc->FuncName = Function->yvalue->var->Name;
      Act->NextFunc->NextFunc = NULL;
    }
  }
  return;
}
     
