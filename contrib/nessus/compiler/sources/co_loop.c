/*****************************************************************************
  FILE              : co_loop.c
  SHORTNAME         : loop
  VERSION           : 2.0

  PURPOSE           : Contains functions to evaluate loop statements and conditional statements within
                         loops:
			 - LOOP_GetNextStackValue          - LOOP_EvalWhile
			 - LOOP_EvalCountLoop              - LOOP_EvalDo
			 - LOOP_EvalTo                     - LOOP_EvalEnd
			 - LOOP_StackToken                 - LOOP_InitLoopElem
			 - LOOP_CopySimpleValue            - LOOP_PushLoopStack
			 - LOOP_PopLoopStack               - LOOP_GetNextToken
			 - LOOP_InsertToken                - LOOP_SetBackPtr
			 - LOOP_AssignCondValue            - LOOP_RemoveTokenStack 
			 - LOOP_InitForLoop                - LOOP_CalcForIterations

			 - LOOP_GetTokenStack              - LOOP_FreeTokenStack
			 - LOOP_GetLoopHeader              - LOOP_FreeLoopHeader
			 - LOOP_GetLoopValue               - LOOP_FreeLoopValue
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_loop.h"

/**** variable to store result value of logicaal expressions ****/
static short ConditionValue = FALSE;       /* TRUE <==> execute statements of loop body */
static short EvaluatedIfCondition = FALSE; /* TRUE <==> 2nd. entry into LOOP_EvalThen() */
static short Evaluated = FALSE;              /* TRUE <==> 2nd. entry into LOOP_EvalDo() */
static short FirstValue = FALSE;        /* TRUE <==> recently initialized loop variable */
static short RecentDo = FALSE;  /* TRUE <==> DO recently read (2nd. pass through header */

/**** variables for stack of tokens ****/
static LoopVarStackType *FreeLoopVarStack = NULL;   /* stack of unused tokens (foreach) */
static BackPtrType *NestedBackPtr =  NULL;            /* stack of first tokens of loops */
static BackPtrType *FreeBackPtr   =  NULL;                /* unused BackPtrType records */
static TokenStackType *FreeTokenStack = NULL;     /* pointer to  stack of unused tokens */
static TokenStackType *TokenStackPtr = NULL;          /* pointer to "stack"  of  tokens */
static TokenStackType *LastToken = NULL;       /* pointer  to last token which has been */
         /* inserted (StackMode) or next token  to be returned to parser (EvalLoopMode) */

/**** variable used in Syntax.y - inhibit antecipated semantic actions ****/
short MissingSemi = FALSE;                /* TRUE <==> parser expects ';' between loops */
short NestingDepth = 0;
short ExitMode = FALSE;                    /* TRUE <==> AbortLoop && begin of new stmt. */
ForeachWaitFlag = FALSE;           /* TRUE <==> do not yet initialize new loop variable */




/*****************************************************************************************
 Function:                     LOOP_GetNextStackValue
 Purpose: Get next token from stack. Change state of parser, if necessary (state depends
          on token, only). Let LastToken point to next token to be returned.
 Returns: Token.
 Side effects: Sets yylval to token value on stack, modifies LastToken,  changes state
               variables. 
*****************************************************************************************/
int LOOP_GetNextStackValue()
{
  SymbolType Temp;                           /* temporarily holds value of curren token */

  if(ExitMode && NestingDepth > 0) {     /* exit nested loops - can be  done at the end */
    NestingDepth --;                      /* of a statement, only <==> ExitMode == TRUE */
    return END;
  }
  else if(ExitMode) {           /* parser has leaved nested loops - remove token stack  */
    LOOP_RemoveTokenStack(TokenStackPtr);                     /* remove stack of tokens */
    StackMode = FALSE;
    LastToken = (TokenStackPtr = NULL);
    NestedBackPtr = NULL;
    EvalLoopMode = FALSE;  AbortLoop = FALSE; ExitMode = FALSE; MissingSemi = TRUE;
  }
  if(LastToken) {                            /* evaluation of loop is still in progress */
    yylineno = LastToken->Line;          /* reset line no. to line no. of current token */
    switch(LastToken->Token) {
    case WHILE:                          /* at the begin of a while loop on token stack */
      if(AbortLoop) {
	ExitMode = TRUE;/* end of statement: exit nested loops and condition statements */
	return LOOP_GetNextStackValue();
      }
      return LOOP_EvalWhile();
    case FOR:
    case FOREACH:
      if(AbortLoop) {
	ExitMode = TRUE;/* end of statement: exit nested loops and condition statements */
	return LOOP_GetNextStackValue();
      }
      return LOOP_EvalCountLoop();
    case IF:
    case ELSEIF:
    case ELSE:
      return LOOP_EvalCondition();
    case DO:              /* at the begin of a loop body -- execute statements or not ? */
      return LOOP_EvalDo();
    case THEN:            /* at the begin of a loop body -- execute statements or not ? */
      return LOOP_EvalThen();
    case END:              /* at the end of a condition  -- execute statements or not ? */
      if(AbortLoop) {
	ExitMode = TRUE;/* end of statement: exit nested loops and condition statements */
	return LOOP_GetNextStackValue();
      }
      return LOOP_EvalEnd();
    case SEMICOLON:
      if(AbortLoop)
	ExitMode = TRUE;/* end of statement: exit nested loops and condition statements */
      else
	RecentDo = TRUE;
    default:                                                        /* all other tokens */
      yylval =  LOOP_CopySimpleValue((LastToken->Value).Simple);     /* set token value */
      Temp = LastToken->Token; 
      if(yylval && yylval->ytype == IDENTIFIER && yylval->yvalue->var->Type == UNIT)
	yylval = yylval;
      yylineno = LastToken->Line;        /* reset line no. to line no. of current token */
      LastToken = LastToken->NextToken;    /* LastToken  becomes NULL at outer loop end */
      return Temp;
    }
  }
  else {            /* parser came to the end of the outer loop, remove stack of tokens */
    /* and return control to normal scanner function SC_Scanner (via LOOP_GetNextToken) */
#ifdef LOOPTEST
    (void) printf("end of  loop\n");
#endif
    LOOP_RemoveTokenStack(TokenStackPtr);
    StackMode = FALSE;
    LastToken = (TokenStackPtr = NULL);
    NestedBackPtr = NULL;
    EvalLoopMode = FALSE;  AbortLoop = FALSE; ExitMode = FALSE; MissingSemi = TRUE;
    return LOOP_GetNextToken();                     /* calls scanner to read next token */
  }
}





/*****************************************************************************************
 Function:                     LOOP_EvalWhile
 Purpose: Semantic  actions for current token WHILE.
 Returns: See LOOP_GetNextToken.
 Side effects: See LOOP_GetNextToken.
*****************************************************************************************/
int LOOP_EvalWhile()
{
  if(AbortLoop) {           /* error: try to leave nested  loops without further errors */
    LastToken = (LastToken->Value).Loop->Abort;
    NestingDepth  --;     /* decrement depth of nested loops and conditional statements */
    return LOOP_GetNextStackValue();
  }
  else {
    if(MissingSemi) {              /* semicolon must be returned to  separate two loops */
      ConditionMode = TRUE;
      yylval = NULL;
      MissingSemi = FALSE;
      return SEMICOLON;
    }                             /* semicolon  has been returned or not been necessary */
    ConditionMode = TRUE;                /* parser wwill now evaluate the condition and */
    yylval = psNULL;                      /* write the result into var. ConditionValue  */
    LastToken = (LastToken->Value).Loop->ConditionPtr;  /* parse and evaluate condition */
    NestingDepth  ++;     /* increment depth of nested loops and conditional statements */
    return WHILE;
  }
}






/*****************************************************************************************
 Function:                     LOOP_EvalCountLoop
 Purpose: Semantic  actions for current token FOR or FOREACH.
 Returns: See LOOP_GetNextToken.
 Side effects: See LOOP_GetNextToken.
*****************************************************************************************/
int LOOP_EvalCountLoop()
{
  SymbolType Temp;
  short EmptyLoop;
  
  if(! AbortLoop) {
    if( ! (LastToken->Value).Loop->LoopHeader->ActIter) {
/*** neu, 26.11. */
      if(NestingDepth)                /* beginning to evaluate outer loop - insert ';' */
	MissingSemi = FALSE;
      if(MissingSemi) {
	MissingSemi = FALSE;
	yylval = NULL;
	return SEMICOLON;   
      }
      else {
	InitMode = TRUE;          /* initialization expression musst be evaluated first */
	yylval = psNULL;          /* save ptr. to begin of loop (NO  STACK, simple var. */
	(NestedBackPtr = (BackPtrType*) M_alloc(sizeof(BackPtrType)))->TokenPtr = LastToken; 
	Temp = LastToken->Token;
	LastToken = (LastToken->Value).Loop->InitialPtr;    /*parse initialization stmt.*/
	NestingDepth  ++;                            /* increment depth of nested loops */
	return Temp;                                                  /* FOR or FOREACH */
      }
    }           /* for loop has been initialized, already. Look for further iterations. */ 
    else {/* LOOP_Incrementxxx checks for recent initialization and legal initial value */
      if(LastToken->Token == FOR)
	EmptyLoop = LOOP_IncrementFor((LastToken->Value).Loop->LoopHeader);
      else      /* LastToken->Token == FOREACH -- increment variables in foreach header */
	if(ForeachWaitFlag) {
	  EmptyLoop = LOOP_IncrementForeach((LastToken->Value).Loop->LoopHeader);
	  ForeachWaitFlag = FALSE;
	}
	else { 
	  ForeachWaitFlag = TRUE;
	  yylval = NULL;
	  return SEMICOLON;
	}
      if(ConditionValue) {                                        /* further iterations */
	if(AbortLoop) {
	  if(LastToken = (LastToken->Value).Loop->Abort) {    /* error in end condition */
	    NestingDepth  --;                        /* decrement depth of nested loops */
	    return END; /*->Abort = END,->Abort->NextToken = NULL for an outer FOR loop */
	  }
	  else {
	    EvalLoopMode = FALSE;
	    AbortLoop = FALSE;
	    return LOOP_GetNextToken();
	  }
	}
	else {                                                   /* continue iterations */
	  if((LastToken->Value).Loop->Continue) {
	    LastToken = (LastToken->Value).Loop->Continue;   /* 1st. token of loop body */
	    if( ! RecentDo || MissingSemi) { 
	      yylval = NULL;
	      return  SEMICOLON;                        /* loop iteration - missing ';' */
	    }
	    else {
	      RecentDo = FALSE;
	      return LOOP_GetNextStackValue();   
	    }                                   /* in loop following DO: no ';' missing */
	  }
	  else {
	    LastToken = (LastToken->Value).Loop->Abort;             /* empty loop  body */
	    ER_StackError("loop: empty loop body (aborting evaluation)\n", Warning, nst, nst, nst);
	    AbortLoop = TRUE;
	    return LOOP_GetNextStackValue();        /* abort, empty loop makes no sense */
	  }
	}    /* parser sees a statement list rather than iterations of a FOR(EACH) loop */
      }
      else {  /* no more iterations, but parser still needs an END  token  for the loop */
	LastToken = (LastToken->Value).Loop->Abort->NextToken;
	if( LastToken && LOOPBEGIN(LastToken->Token) && EmptyLoop) 
	  MissingSemi = TRUE;
	NestingDepth  --; /* decrement depth of nested loops and conditional statements */
	return END;  /* ->Abort = END,  ->Abort->NextToken = NULL for an outer FOR loop */
      }
    }
    
  }
  else              /* errors: leave nested loops, but provide for necessary END tokens */
    if(LastToken = (LastToken->Value).Loop->Abort->NextToken) {
      MissingSemi = FALSE;
      NestingDepth  --; /* decrement depth of nested loops and conditional statements */
      return END;  /* ->Abort = END,  ->Abort->NextToken = NULL for an outer FOR loop */
    }
    else {
      EvalLoopMode = FALSE;
      AbortLoop = FALSE;
      /*** neu, 26.11. */
      NestingDepth  --;   /* decrement depth of nested loops and conditional statements */
      return END;
      /*       return LOOP_GetNextToken();  */
    }
}





/*****************************************************************************************
 Function:                     LOOP_EvalCondition
 Purpose: Semantic  actions for current token IF, ELSEIF, or ELSE.
 Returns: See LOOP_GetNextToken.
 Side effects: See LOOP_GetNextToken.
*****************************************************************************************/
int LOOP_EvalCondition()
{
  TokenStackType *Act;           /* needed to look for END token of condition statement */

  yylval = NULL;
  if(MissingSemi) {  /* return SEMICOLON to parser : separate evaluation and stack mode */
    MissingSemi  =  FALSE;
    yylval  =  NULL;
    return SEMICOLON;
  }    
  if(AbortLoop || (LastToken->Value).Cond->Executed) {        /* look for 'virtual' END */  
    (LastToken->Value).Cond->Executed = FALSE;
    for(Act = LastToken; (Act->Value).Cond->NextIf; Act = ((Act->Value).Cond)->NextIf)
      ;                              /* look for IF which begins contidional  statement */
    LastToken = Act->NextToken;    /* THE END OF CONDITIONAL STATEMENTS IS NOT ON STACK */
    NestingDepth --;
    return END;    /* return an END, next token is that which follows conditional stmt. */
  }
  else if(LastToken->Token==ELSE) {                             /* evaluate ELSE branch */
    (LastToken->Value).Cond->Executed = TRUE;                            /* mark branch */
    LastToken = (LastToken->Value).Cond->ThenStmts;
    return ELSE;                                      /* not needed, for debugging only */
  }
  else if(EvaluatedIfCondition) {/* if condition evaluated, ConditionValue holds result */
    EvaluatedIfCondition = FALSE;              /* for 2 entries into LOOP_EvalThen(); */
    if(ConditionValue) {                  /* TRUE: execute statements which follow THEN */
      (LastToken->Value).Cond->Executed = TRUE;                          /* mark branch */
      LastToken = (LastToken->Value).Cond->ThenStmts;
      return LOOP_GetNextStackValue();               /* return first token of stmt-list */
    }
    else                                   /* FALSE: continue with ELSEIF, ELSE, or END */
      if((LastToken->Value).Cond->NextIf) {
	LastToken = (LastToken->Value).Cond->NextIf;
	NestingDepth  --; /* decrement depth of nested loops and conditional statements */
	return LOOP_GetNextStackValue();                    /* following ELSEIF or ELSE */
      }
      else {                    /* 'virtual' END token of conditional statement reached */
	LastToken = LastToken->NextToken; /* END for conditional statement NOT ON STACK */
	NestingDepth  --; /* decrement depth of nested loops and conditional statements */
	return END;                       /* return final end for conditional statement */
      } 
  }
  else {                               /* first time at this token - evaluate condition */
    ConditionMode = TRUE;
    Act = LastToken;
    NestingDepth  ++;     /* increment depth of nested loops and conditional statements */
    LastToken = (LastToken->Value).Cond->IfCondition;
    return Act->Token;
  }
}





/*****************************************************************************************
 Function:                     LOOP_EvalDo
 Purpose: Semantic  actions for current token DO.
 Returns: See LOOP_GetNextToken.
 Side effects: See LOOP_GetNextToken.
*****************************************************************************************/
int LOOP_EvalDo()
{
  yylval = psNULL; 
  if(! Evaluated) {           /* logical expression has not been evaluated (look ahead) */
    Evaluated = TRUE;
    yylval = NULL;
    return DO;
  }
  else {
    RecentDo = TRUE;               /* needed to distinguish passes through for, foreach */
    ConditionMode = FALSE;                        /* next tokens are got from loop body */
    /* LastToken points to  DO, set  it to WHILE, FOR, or FOREACH */
    Evaluated = FALSE;
/*** neu, 4..12. -- if war  vorher nur fuer WHILE */
    if(STACKBEGIN(LastToken->NextToken->Token))/*begin to execute loop body immediately */
      if(ConditionValue)                          /* condition == TRUE, more iterations */
	if(! (LastToken->NextToken->Value).Loop->Continue) {
	  ER_StackError("loop: empty loop body (aborting evaluation)\n", Warning, nst, nst, nst);
	  AbortLoop = TRUE;
	  LastToken = (LastToken->NextToken->Value).Loop->Abort;  /* next token is END  */
	}
	else
	  LastToken = (LastToken->NextToken->Value).Loop->Continue; /* -> body of  loop */
      else      /* loop has been evaluated - leave it (parser needs an END after DO !!) */
	LastToken = (LastToken->NextToken->Value).Loop->Abort;    /* next token is END  */
    else 
      LastToken = LastToken->NextToken;
  }
/*** neu, 5.12. */
  FirstValue = FALSE;                       /* continue with stmts, increment next time */
/*** neu, 5.12. */
  return LOOP_GetNextStackValue();      /* 2nd. entry, DO NOT RETURN "DO" (next symbol) */
}





/*****************************************************************************************
 Function:                     LOOP_EvalThen
 Purpose: Semantic  actions for current token THEN.
 Returns: See LOOP_GetNextToken.
 Side effects: See LOOP_GetNextToken.
*****************************************************************************************/
int LOOP_EvalThen()
{
  yylval = psNULL; 
  if(! EvaluatedIfCondition) {/* logical expression has not been evaluated (look ahead) */
    EvaluatedIfCondition = TRUE;                             /* signal to LOOP_EvalCond */
    yylval = NULL;
    return THEN;
  }
  else {
    ConditionMode = FALSE; 
    Evaluated = FALSE;
    LastToken = LastToken->NextToken;      /* pointer back to correponding IF or ELSEIF */
    return LOOP_GetNextStackValue();  /* 2nd. entry, DO NOT RETURN "THEN" (next symbol) */
  }
}






/*****************************************************************************************
 Function:                     LOOP_EvalEnd
 Purpose: Semantic  actions for current token END.
 Returns: See LOOP_GetNextToken.
 Side effects: See LOOP_GetNextToken.
*****************************************************************************************/
int LOOP_EvalEnd()
{
  yylval = psNULL;
  if(AbortLoop) {    /* there have been semantic errors (Serious, at least) - exit loop */
    if(LastToken = LastToken->NextToken)                  /* end finishes up loop  body */
      if(LOOPBEGIN(LastToken->Token)) {  
	if(LastToken->Token == FOREACH)               /* unlock stack of loop variables */ 
	  LOOP_UnlockLoopVars((LastToken->Value).Loop->LoopHeader, TRUE); 
	else if(LastToken->Token == FOR)                 /* unlock single loop variable */
	  LOOP_UnlockLoopVars((LastToken->Value).Loop->LoopHeader, FALSE);   
	if((LastToken->Value).Loop->Abort->NextToken &&        /* not end of outer loop */
	   (LastToken->Value).Loop->Abort->NextToken->Token != END)
	  MissingSemi = TRUE;     /* other stmts. follow END in outer loop - insert ';' */
	LastToken = (LastToken->Value).Loop->Abort->NextToken;  /* return to outer loop */
	/* else : return END without ';' and continue  in outer loop body */
    }
    NestingDepth  --;     /* decrement depth of nested loops and conditional statements */
    return END;
  }
  if(LastToken = LastToken->NextToken)
    if(LOOPBEGIN(LastToken->Token)) {
      /* LastToken  becomes NULL at outer loop end */
      /* LastToken has been set to LastToken->NextToken already. Now, points to WHILE.  */
      if(LastToken->Token == WHILE) {
	MissingSemi = TRUE; 
	NestingDepth  --;                            /* decrement depth of nested loops */
	return END;                /* while loop: parser sees a sequence of while loops */
      }
      else {                   /* do not decrement NestingDepth - continue in same loop */
/*** neu, 26.11. */
	MissingSemi = TRUE;
	return LOOP_GetNextStackValue(); /* for, foreach: iterate loop body, ignore end */
      }
    }
    else {
      NestingDepth  --;   /* decrement depth of nested loops and conditional statements */
      return END;
    }
  else {
    NestingDepth  --;     /* decrement depth of nested loops and conditional statements */
    return  END; /* at next call to LOOP_GetNextStackValue() the token stack is removed */
  }
}






/*****************************************************************************************
 Function:                     LOOP_StackToken
 Purpose: Insert a new token at the end of the tokens list. For structure of list, see 
          nessus programmers manual.
 Returns: void.
 Side effects: Modifies tokens list.
*****************************************************************************************/
void LOOP_StackToken(Token, Value)
     SymbolType Token;                                                         /* token */
     ParserStackType *Value;                               /* token value, may be  NULL */
{
  TokenStackType *New;                                  /* new entry to list of tokens  */

  New = LOOP_GetTokenStack();                                 /* get new entry to  list */
  New->Token = Token;
  New->Line = yylineno;    /* keep track of line, since "\n" is not returned by scanner */
  switch(Token) {                              /* action and value depend on token type */
  case  WHILE:
    LOOP_InitLoopElem(New);                    /* append a LoopValueType  record to New */
    ConditionMode = TRUE;
    break;
  case FOREACH:               /* for and foreach headers are handled in the same manner */
  case FOR:
    LOOP_InitLoopElem(New);                    /* append a LoopValueType  record to New */
    (New->Value).Loop->LoopHeader = LOOP_GetLoopHeader();/*append LoopHeaderType record */
    InitMode =  TRUE;
    break;
  case DO: 
    LOOP_InsertToken(New); 
    break;
  case ELSEIF:
  case ELSE:                               /* set pointer back to IF or previous ELSEIF */
    if(NestedBackPtr)                 /* error case: else or elseif without previous if */
      if(NestedBackPtr->TokenPtr != LastToken)               /* continue with IF case!! */
	LastToken = (LastToken->NextToken = LOOP_PopLoopStack());
      else
	((LastToken->Value).Cond)->ThenStmts = LOOP_PopLoopStack();  /* empty stmt list */
  case IF:
    LOOP_InitCondElem(New); 
    break;
  case THEN:   
    LOOP_SetBackPtr(New, Token); 
    return;
  case END:         
    LOOP_SetBackPtr(New, Token); 
    return;                                          /* TOKEN HAS ALREADY BEEN INSERTED */
  default:           /* save token value, since semantic actions remove their arguments */
    (New->Value).Simple = LOOP_CopySimpleValue(Value);
    break;
  }
  if(StackMode)                             /* mode may have changed, when Token is END */
    LOOP_InsertToken(New);         /* let LastToken point to "last token" in list, this */
}          /* may well be the first token of a  loop or condition (see LOOP_SetBackPtr) */





/*****************************************************************************************
 Function:                     LOOP_InitLoopElem
 Purpose: Initialize a  new LoopValueType record and append it to the TokenStackType
          record  (1st. argument). 
 Returns: void.
 Side effects: Modifies 1st. argument.
*****************************************************************************************/
void LOOP_InitLoopElem(New)
     TokenStackType *New;
{
  (New->Value).Loop = LOOP_GetLoopValue();      /* initialized new LoopValueType record */
  LOOP_PushLoopStack(New);          /* push onto stack of  first tokens of nested loops */
  StackMode = TRUE;                     /* has been FALSE if token begins an outer loop */
  Execute = FALSE;                         /* do not evaluate statements or expressions */
}





/*****************************************************************************************
 Function:                     LOOP_InitCondElem
 Purpose: Initialize a  new LoopValueType record and append it to the TokenStackType
          record  (1st. argument). 
 Returns: void.
 Side effects: Modifies 1st. argument.
*****************************************************************************************/
void LOOP_InitCondElem(New)
     TokenStackType *New;
{
  (New->Value).Cond = LOOP_GetCondValue();      /* initialized new CondValueType record */
  LOOP_PushLoopStack(New);          /* push onto stack of  first tokens of nested loops */
  StackMode = TRUE;                     /* has been FALSE if token begins an outer loop */
  Execute = FALSE;                         /* do not evaluate statements or expressions */
}





/*****************************************************************************************
 Function:                     LOOP_CopySimpleValue
 Purpose: Copy a simple parser value stack record (argument). The argument may be NULL.
 Returns: Pointer to ParserStackType record or NULL.
*****************************************************************************************/
ParserStackType *LOOP_CopySimpleValue(Val)
     ParserStackType *Val;   /* value to  be copied -- is NULL for keywords & operators */
{
  ParserStackType *Copy;                                  /* will point  to copy of Val */

  if(Val) {
    Copy = PA_GetParserStack();
    Copy->ytype = Val->ytype;
    *(Copy->yvalue) = *(Val->yvalue);
    return Copy;
  }
  else
    return NULL;
}





/*****************************************************************************************
 Function:                     LOOP_PushLoopStack
 Purpose: Push a new TokenStackType record onto the stack of loop or condition begins.
 Returns: void.
 Side effects: Modifies NestedBackPtr and FreeBackPtr.
*****************************************************************************************/
void LOOP_PushLoopStack(TPtr)
     TokenStackType *TPtr;
{
  BackPtrType *New = NULL;
  if(FreeBackPtr)                            /* there are unused records of BackPtrType */
    FreeBackPtr = (New = FreeBackPtr)->Outer;
  else                                                  /* no unused records - allocate */
    New = (BackPtrType *) M_alloc(sizeof(BackPtrType));    
  New->Outer = NestedBackPtr;                              /* insert new at top of stack */
  (NestedBackPtr = New)->TokenPtr = TPtr;
}





/*****************************************************************************************
 Function:                     LOOP_PopLoopStack
 Purpose: Pop a TokenStackType record from the stack of loop or condition begins.
 Returns: TokenStackType record which has been on top of the stack.
 Side effects: Modifies NestedBackPtr and FreeBackPtr.
*****************************************************************************************/
TokenStackType *LOOP_PopLoopStack()
{
  BackPtrType *Temp;

  if(Temp = NestedBackPtr) {                      /* there are more loop begins on stack */
    NestedBackPtr = NestedBackPtr->Outer;     /* remove top element,  put it on stack of */
    Temp->Outer = FreeBackPtr;                                         /* unused redcords */
    FreeBackPtr = Temp;
    return Temp->TokenPtr;                         /* return ptr.  to 1st. token of loop */
  }
  else {            /* no more loop begins on stack -- there must be a bug somewhere... */
    (void) printf("******bug: NestedBackPtr == NULL in LOOP_PopLoopStack\n");
    return NULL;
  }
}





/*****************************************************************************************
 Function:                     LOOP_GetNextToken
 Purpose: Get next token - from either scanner or token stack.
 Returns: Token.
*****************************************************************************************/
int LOOP_GetNextToken()
{
  int val;                                                               /* token value */
  
  if( ! EvalLoopMode)  {             /* currently not evaluating a  loop, call scanner  */
    val  = SC_Scanner();
    if(Topology) 
      if(Topology && (STACKBEGIN(val) || StackMode)) {
#ifdef LOOPTEST
	if(val == IDENTIFIER) {
	  (void) printf("%s ", yylval->yvalue->var->Name); 
	  (void) fflush(stdout);
	}
	else if(val == END) {
	  (void) printf("END(%2d)  ", yylineno); 
	  (void) fflush(stdout);
	}
	else {
	  (void) printf("%s   ", STR_TypeString(val)); 
	  (void) fflush(stdout);
	}
#endif
	LOOP_StackToken(val, yylval);                                      /* stack token */
      }
    return val;
  }
  else  {                    /* currently evaluating a loop, get next  token from stack */
    val = LOOP_GetNextStackValue();
#ifdef LOOPTEST
    if(val == IDENTIFIER)
      (void) printf("%s ", yylval->yvalue->var->Name);
    else {
      if(STACKBEGIN(val))
	(void) printf("\n");
      (void) printf("%s ", STR_TypeString(val));
    }
#endif
    return val;
  }
}






/*****************************************************************************************
 Function:                     LOOP_InsertToken
 Purpose: Set LastToken to list position after which the next token will be inserted.
 Returns: void.
 Side effects: Modifies LastToken and, eventually, TokenStackPtr.
*****************************************************************************************/
void  LOOP_InsertToken(TokenNode)
     TokenStackType *TokenNode;                         /* pointer to new token in list */
{
  if(TokenStackPtr)                          /* there have been other tokens previously */
    switch(LastToken->Token) {                 /* insertion depends on last token read  */
    case WHILE:
      if(ConditionMode && ! LOOPBEGIN(TokenNode->Token)) {              /* token which */
	/* follows WHILE immediately and ConditionMode is not set due to new TokenNode  */
	((LastToken->Value).Loop)->ConditionPtr = TokenNode;
	LastToken = TokenNode;
      }
      else if(TokenNode->Token == DO) {
	ConditionMode = FALSE;
	break;                   /* state has changed recently, let LastToken --> WHILE */
      }
      else {       /* TokenNode is the token which follows DO, first token of loop body */
	((LastToken->Value).Loop)->Continue = TokenNode;
	LastToken = TokenNode;
      }
      break;   
    case FOR:   
    case FOREACH:
      if(InitMode && ! LOOPBEGIN(TokenNode->Token)) {              /* token which */
	/* follows FOR immediately and InitMode is not set due to new TokenNode - next */
	((LastToken->Value).Loop)->InitialPtr = TokenNode;  /* token is initialization */
	LastToken = TokenNode;
      }
      else if(TokenNode->Token == DO) {
	InitMode = FALSE;
	break;                   /* state has changed recently, let LastToken --> WHILE */
      }
      else {       /* TokenNode is the token which follows DO, first token of loop body */
	((LastToken->Value).Loop)->Continue = TokenNode;
	LastToken = TokenNode;
      }
      break;                                   /* insertion depends on last token read  */
    case IF:
    case ELSEIF:
      if( ! ((LastToken->Value).Cond)->IfCondition)/* next tokens are part of condition */
	LastToken = (((LastToken->Value).Cond)->IfCondition = TokenNode);
      else if( ! ((LastToken->Value).Cond)->ThenStmts)   /* now follows part of if_body */
	if(NEXTBRANCH(TokenNode->Token)) {                      /* empty statement list */
	  LastToken = ((LastToken->Value).Cond)->ThenStmts = LOOP_PopLoopStack();
	  LastToken = (((LastToken->Value).Cond)->NextIf = TokenNode);
	}
	else                                 /* there are statements for true condition */
	  LastToken = (((LastToken->Value).Cond)->ThenStmts = TokenNode);
      else if(NEXTBRANCH(TokenNode->Token))
	LastToken = (((LastToken->Value).Cond)->NextIf = TokenNode);    /* ELSEIF, ELSE */
      else                          /* after END (which has NOT been inserted into list */
	LastToken = (LastToken->NextToken = TokenNode);
      break;
    case ELSE:                     /* next tokens are part of statement list after ELSE */
      if(  ! (((LastToken->Value).Cond)->ThenStmts))
	LastToken = (((LastToken->Value).Cond)->ThenStmts = TokenNode);    /* stmt list */
      else
	LastToken = (LastToken->NextToken = TokenNode);             /* end of cond stmt */
      break;
    case DO:                       /* do not change LastToken, it points to loop header */
      TokenNode->NextToken = NestedBackPtr->TokenPtr; /* DO --> WHILE, FOR,  or FOREACH */
      LastToken = TokenNode->NextToken;
      ConditionMode = FALSE;
      InitMode = FALSE;
      break;              
    default:
      LastToken = (LastToken->NextToken = TokenNode);
      break;     /* in normal constellations, LastToken already points to correct token */
    }
  else {                                                                /* first of all */
    TokenStackPtr = (LastToken = TokenNode);
    AbortLoop = FALSE;
  }
}



/*****************************************************************************************
 Function:                     LOOP_SetBackPtr
 Purpose: Finish up processing of a parser state and leave it. Called on tokens END, DO,...
 Returns: void.
 Side effects: Modifies its argument and LastToken.
*****************************************************************************************/
void LOOP_SetBackPtr(TPtr, Type)
     TokenStackType  *TPtr;                      /* pointer to new token record in list */
     SymbolType Type;                                                          /* token */
{
  switch(Type) {
  case DO:                                               /* at the begin of a loop body */
    if(ConditionMode) {        /* MUST BE in ConditionMode, do not remove NestedBackPtr */
      TPtr->NextToken = NestedBackPtr->TokenPtr;          /* pointer back  to loop head */
      LastToken = TPtr->NextToken;   /* LastToken --> head  of for, foreach, while loop */
      ConditionMode = FALSE;
    }
    else
      (void) printf("******bug: not in condition mode at DO\n");
    break;
  case THEN:                                        /* set pointer back to IF or ELSEIF */
/*** neu, 3.12. */
    LastToken = ((LastToken->NextToken = TPtr)->NextToken = NestedBackPtr->TokenPtr);
/*** neu, 3.12. */
    break;              /* do not remove ptr. to IF or ELSEIF from stack (wait for END) */
  case END:
    LOOP_SetEndBackPtr(TPtr);
    break;
  default:
    (void) printf("******bug: illegal token type in LOOP_SetBackPtr %s\n",
		  STR_TypeString(Type));
    break;
  }
  if(! NestedBackPtr) {                       /* this END is finishes up the outer loop */
    LastToken = TokenStackPtr;          /* back to upper nest level - end of stack mode */
    StackMode = FALSE;
    if(! AbortLoop) {
      EvalLoopMode = TRUE;                                             /* evaluate loop */
      MissingSemi = TRUE;
    }
    else {         /* there have been syntax errors in loop - return control to scanner */
      LOOP_RemoveTokenStack(TokenStackPtr);                   /* remove stack of tokens */
      StackMode = FALSE;
      LastToken = (TokenStackPtr = NULL);
      NestedBackPtr = NULL;
      EvalLoopMode = FALSE;  AbortLoop = FALSE; ExitMode = FALSE; MissingSemi = TRUE;
    }
#ifdef LOOPTEST
    (void) printf("\n\n"); 
#endif
  }
}









/*****************************************************************************************
 Function:                     LOOP_SetEndBackPtr
 Purpose: Finish up processing of a parser state and leave it. Called on tokens END, DO,...
 Returns: void.
 Side effects: Modifies its argument and LastToken.
*****************************************************************************************/
void LOOP_SetEndBackPtr(TPtr)
     TokenStackType  *TPtr;                      /* pointer to new token record in list */
{
    
  if(CONDITIONBEGIN(NestedBackPtr->TokenPtr->Token))            /* end of IF..ELSE..END */
    if(CONDITIONBEGIN(LastToken->Token))          /* empty statement list at COND node */
      LastToken = (((LastToken->Value).Cond)->ThenStmts = LOOP_PopLoopStack()); 
    else
      LastToken = (LastToken->NextToken = LOOP_PopLoopStack()); 
  /* END OF CONDITION STATEMENT IS NOT INSERTED INTO LIST */
  /* implicit && ! CONDITIONBEGIN(NestedBackPtr->TokenPtr->Token) */
  else if(CONDITIONBEGIN(LastToken->Token)) {       /* END NOT of conditional statement */
    LastToken = (LastToken->NextToken = TPtr);
    TPtr = LOOP_GetTokenStack();
    (TPtr->Value).Simple = NULL;         /* generate new end to be appended at loop end */
    TPtr->Token = END;
    TPtr->Line = yylineno;
    LOOP_SetEndBackPtr(TPtr);    /* append new end at for loop which encloses cond stmt */
  }
  else if(! LOOPBEGIN(LastToken->Token) && LastToken->Token != END && ! AbortLoop) {
    LastToken = ((LastToken->NextToken = TPtr)->NextToken = LOOP_PopLoopStack());
    LastToken = ((LastToken->Value).Loop->Abort  = LOOP_GetTokenStack());   /* generate */
    (LastToken->Value).Simple = NULL;                       /* final end record for end */
    LastToken->Token = END;
    LastToken->Line = yylineno;
  }
  else if(LastToken->Token == END) {
    if(NestedBackPtr)
      LastToken = (LastToken->NextToken = LOOP_PopLoopStack());
    LastToken = ((LastToken->Value).Loop->Abort = TPtr);         /* append a single end */
  }
  else {
    (void) LOOP_PopLoopStack();     /* empty loop body, remove begin of loop from stack */
    LastToken = ((LastToken->Value).Loop->Abort  = TPtr);
  }
}




/*****************************************************************************************
 Function:                     LOOP_AssignCondValue
 Purpose: Assign result of a boolean or logical expression to ConditionValue.
 Returns: void.
 Side effects: Modifies ConditionValue.
*****************************************************************************************/
void LOOP_AssignCondValue(Val)
     ParserStackType *Val;
{
  if(Val->ytype  == Boolean) {
    ConditionValue = (Val->yvalue->intval) ? TRUE : FALSE;
    ConditionMode = FALSE;
  }
  else {
    if(Val->ytype  != Undef)
      ER_StackError("type  mismatch: logical or boolean expression expected\n",  Serious,
		    nst, nst,  nst);
    AbortLoop = TRUE;
    ConditionValue = FALSE;
  }
  PA_FreeParserStack(Val);
}





/*****************************************************************************************
 Function:                     LOOP_InitForLoop
 Purpose: Initialize loop variable of FOR loop. Then lock this variable, since  it may not
          be modified by a statement in the loop.
 Returns: void.
 Side effects: Modifies LoopHeader appended to NestedBackPtr.
*****************************************************************************************/
void LOOP_InitForLoop(Id, Val)
     ParserStackType *Id;  /* lest side of initialization statement, must be IDENTIFIER */
     ParserStackType *Val;     /* right side, must be an  INTEGER or SIMPLESTRING value */
{
  if(Id->ytype  == IDENTIFIER)
    if(Val->ytype == INTEGER || Val->ytype == SIMPLESTRING) {
      /* STMT_AssignValue checks if  both sides of assignment operator are of same type */
      STMT_AssignValue(Id, Val);     /* Id has been  put on stack of unused  records !! */
      Id->yvalue->var->Lock =  TRUE;                                   /* lock variable */
      NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->ActIter = 1;  /* first iteration */
      NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->LoopVars.Single =
	Id->yvalue->var;       /* hold pointer to  loop variable (increment and unlock) */
      Evaluated = TRUE;                             /* continue  parsing with loop body */
      FirstValue = TRUE;       /* indicates that loop var has been initialized recently */
      return;
    }
    else 
      ER_StackError("type mismatch: illegal data type for loop variable\n", Serious,
		    nst, nst, nst);
  else
    ER_StackError("type mismatch: illegal data type for loop variable\n", Serious,
		  nst, nst, nst);
  AbortLoop = TRUE;        /* there  have been errors - do not execute loop statemments */
  Evaluated = TRUE;                                 /* continue  parsing with loop body */
}
    





/*****************************************************************************************
 Function:                     LOOP_CalcForIterations
 Purpose: Calculate no. of iterations of  a FOR loop and insert it into the correspondent
          loop header.
 Returns: void.
 Side effects: Modifies loop header and ConditionValue.
*****************************************************************************************/
void LOOP_CalcForIterations(Val)
     ParserStackType *Val;            /* value of <expr> in "for <init> to <expr> do .. */
{
  if( ! AbortLoop) {
    if(Val->ytype != Undef) {
      if(Val->ytype == FLOATNUM) {
	Val->ytype = INTEGER;
	ER_StackError("truncation: loop variable must be integer\n",  Warning, nst,  nst,  nst);
	Val->yvalue->intval = (int) Val->yvalue->floatval;
      }
      if(Val->ytype ==
	 NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->LoopVars.Single->Type) {
	if(Val->ytype == INTEGER)
	  NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->TotalIter = Val->yvalue->intval -
	    NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->LoopVars.Single->Value->intval + 1;
	else                                               /* Val->ytype == SIMPLESTRING */
	  NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->TotalIter =
	    STR_Ord(*Val->yvalue->string) -
	      STR_Ord(*NestedBackPtr->TokenPtr->Value.Loop
			   ->LoopHeader->LoopVars.Single->Value->string) + 1;
	if(NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->TotalIter >= 
	   NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->ActIter) 
	  ConditionValue = TRUE;                                       /* evaluate loop */
      }
      else {
	ER_StackError("type mismatch: initial and end value for loop variable\n",
		      Serious, nst, nst, nst);
	ConditionValue = FALSE; AbortLoop = TRUE;           /* do not execute loop body */
	NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->TotalIter = - 1; 
      }
    }
    else  {                       /* there have been errors - try to leave nested loops */
      ConditionValue = FALSE; AbortLoop = TRUE;             /* do not execute loop body */
      NestedBackPtr->TokenPtr->Value.Loop->LoopHeader->TotalIter = - 1; 
    }
  }
}
    





/*****************************************************************************************
 Function:                     LOOP_IncrementFor
 Purpose: Increment no. of iterations in for header and update the variables value  entry.
          Then check  wether ActIter <= TotalIter and set ConditionValue.
 Returns: TRUE if there are 0 iterations, else FALSE.
 Side effects: Modifies loop header and ConditionValue.
*****************************************************************************************/
short LOOP_IncrementFor(Head)
     LoopHeaderType *Head;
{
  short Val;             /* result value: TRUE <=> initial loop var value >> end  value */
  Val = FALSE;
  if( ! FirstValue) {
    if(Head->LoopVars.Single->Type == INTEGER)   /* first, increment value and  counter */
      Head->LoopVars.Single->Value->intval ++;
    else 
      *(Head->LoopVars.Single->Value->string) =
	STR_Chr(STR_Ord(*((Head->LoopVars).Single->Value->string))+1);
    Head->ActIter++;
  } 
  else {                         /* firstValue is reset to  FALSE in LOOP_EvalCountLoop */
    if(Head->ActIter > Head->TotalIter)                /* loop of type for  ... do end; */
      Val = TRUE;                  /* empty loop, must not add additional ';' after end */
    FirstValue = FALSE;
  }
  if(Head->ActIter <= Head->TotalIter)                /* new value -> legal iteration ? */
    ConditionValue = TRUE;
  else  {
    Head->ActIter = 0;                             /* will  be  reinitialized next time */
    ConditionValue = FALSE;                                         /* stop  iterations */
    (Head->LoopVars).Single->Lock = FALSE;
  }
  return Val;
}
      




/*****************************************************************************************
 Function:                     LOOP_CheckLoopVar
 Purpose: Checks data type of loop variable and its loop values range. If there is an
          error, Id->ytype is set to Undef.
 Returns: void. 
 Side effects: May modify Id->ytype.
*****************************************************************************************/
void LOOP_CheckLoopVar(Id, Val)
     ParserStackType *Id;                 /* holds symbol table entry for loop variable */
     ParserStackType *Val;   /* should hold range or array of  values for loop variable */
{
  STMT_GetIdValue(Val);                   /* if Val->ytype == IDENTIFIER, get its value */
  if(Val->ytype  != Undef)                                      /* Val has been defined */
    if( ! Id->yvalue->var->UndefType)                /* loop variable has been declared */
      if( ! Id->yvalue->var->Constant)
	if(Val->ytype ==  RANGE)                        /* expression like arrName[1..20] */
	  if(Val->yvalue->range->FieldType  != Id->yvalue->var->Type && 
	     ! Id->yvalue->var->Type == STRING && 
	     (Val->yvalue->range->FieldType == LONGSTRING || 
	      Val->yvalue->range->FieldType == SIMPLESTRING))               /* type error */
	    ER_StackError("type mismatch: loop variable '%s'\n", Serious,
			  STR_Save(Id->yvalue->var->Name), nst, nst);
	  else
	    return;                                                 /* data types are ok */
	/* now, compare data type of loop variable to that of its value range definition */
	else if(CORRECTSETTYPE(Val->ytype))
	  if( ! SETMEMBERMATCH(Val->ytype, Id->yvalue->var->Type))
	    ER_StackError("type mismatch: loop variable '%s'\n", Serious,   /* type error */
			  STR_Save(Id->yvalue->var->Name), nst, nst);
	  else
	    return;                                                 /* data types are ok  */
	else {                                                              /* type error */
	  ER_StackError("redefinition of loop variable '%s'\n", Warning,
			STR_Save(Id->yvalue->var->Name), nst, nst);
	  Id->yvalue->var->Type = GETELEMTYPE(Val->ytype);
	}
      else
	ER_StackError("'%s' is constant, cannot be loop variable\n", Serious,
		      STR_Save(Id->yvalue->var->Name), nst, nst);
    else
      ER_StackError("undeclared variable '%s'\n", Serious,
		    STR_Save(Id->yvalue->var->Name), nst, nst);
  Id->ytype = Undef;                   /* there has been an error,  do not execute loop */
}






/*****************************************************************************************
 Function:                     LOOP_InsertLoopVar
 Purpose: Inserts a loop variable and its loop values range size into the loop header.
          Previously to a call  to this function, Id and Val data  types must have  been
	  checked by LOOP_CheckLoopVar.
 Returns: void. 
 Side effects: Modofies loop header record. May  set AbortLoop = TRUE if there's an error.
*****************************************************************************************/
void LOOP_InsertLoopVar(Id, Val)
     ParserStackType *Id;                 /* holds symbol table entry for loop variable */
     ParserStackType *Val;   /* should hold range or array of  values for loop variable */
{
  int Size;                          /* no.  of elements in range of new  loop variable */
  ArrayElemType First;                       /* initialization value for loop  variable */
  LoopVarStackType *New;                   /* will hold new loop variable specification */
  register UnitType **ActUnit;
  register int i;

  if(Id->ytype != Undef) {  /* data types of  variable and its values have been correct */
    if(Val->ytype == RANGE && Val->yvalue->range->FieldType == STRUCTURE || 
       Val->ytype == STRUCTURE)   {               /* must build array of explicit units */
      if(Val->ytype == STRUCTURE)
	Size = Val->yvalue->structure->Length;
      else
	Size = Val->yvalue->range->Upper - Val->yvalue->range->Lower + 1;
      if(Val->ytype == STRUCTURE) {               /* foreach range is a whole structure */
	if(! Val->yvalue->structure->ExplUnits) 
	  ST_InitExplUnits(Val->yvalue->structure);      /* build explicit unit records */
	for(ActUnit = Val->yvalue->structure->ExplUnits, i=0;
	    i < Val->yvalue->structure->Length; ActUnit ++, i++)       /* for all units */
	  if( ! *ActUnit) {
	    *ActUnit = ST_SingleExplicitUnit(Val->yvalue->structure->UnitDef);
	    (*ActUnit)->UnitNo = Val->yvalue->structure->Begin + i;
	  }
	First.aunit = Val->yvalue->structure->ExplUnits;
      }
      else {             /* range of a structure - build explicit records for ALL units */
	if(! Val->yvalue->range->FieldPtr->Value->structure->ExplUnits) 
	  ST_InitExplUnits(Val->yvalue->range->FieldPtr->Value->structure);
	for(ActUnit = Val->yvalue->range->FieldPtr->Value->structure->ExplUnits, i=0;
	    i < Val->yvalue->range->FieldPtr->Value->structure->Length; ActUnit ++, i++)
	  if( ! *ActUnit) {
	    *ActUnit = ST_SingleExplicitUnit(Val->yvalue->range->FieldPtr->Value->structure->UnitDef);
	    (*ActUnit)->UnitNo = Val->yvalue->range->FieldPtr->Value->structure->Begin + i;
	  }
	First.aunit = Val->yvalue->range->FieldPtr->Value->structure->ExplUnits +
	    Val->yvalue->range->Lower;                      /* mark first unit of range */
      }
    }
    else if(Val->ytype == RANGE) {                    /* range of some other array type */
      Size = Val->yvalue->range->Upper - Val->yvalue->range->Lower + 1;
      LOOP_AssignIncr(&First, Val->yvalue->range->FieldPtr->Value->array->aValue, 
	Val->yvalue->range->Lower, Val->ytype);
    }      
    else {                    /* since  data types are correct,  Val must hold an array */
      Size = Val->yvalue->array->aSize;                               /*  no. of values */
      First = Val->yvalue->array->aValue;                      /* initial value (UNION) */
    }
    if(! Size) {
      (void) printf("*****bug: null  sized array in foreach loop (id = %s)\n",
		    Id->yvalue->var->Name);
      exit(105);
    }
    New = LOOP_GetLoopVar();               /* allocate  a new record for loop variable  */
    *(New->InitValue) = First;             /* New->InitValue has been allocated already */
    New->Remove = (Val->ytype == STRUCTURE) ? FALSE : (Val->ytype != RANGE) ? 
      Val->yvalue->array->Named : FALSE;            /* can array be removed at loop end */
    New->Variable = Id->yvalue->var;                  /* pointer to  symbol table entry */
    if( ! ((NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->LoopVars).Multi) {
                       /* new loop  variable is the first, set TotalIter of loop header */
      ((NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->LoopVars).Multi = New;
    (NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->TotalIter = Size;
    }
    else {  /* New is not the first variable, no. of values should be identical for all */
      if( Size != (NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->TotalIter) {
	ER_StackError("foreach: distinct iteration  no.'s (minimum used)\n", 
		      Warning, nst, nst, nst);       /* use minimum and try to continue */
	if(Size < (NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->TotalIter)
	  (NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->TotalIter = Size;
      }                             /* insert new variable into stack of loop variables */
      New->NextVar = ((NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->LoopVars).Multi; 
      ((NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->LoopVars).Multi = New;
    }
  }
  else 
    AbortLoop = TRUE;      /* errors in loop variable definition -- do not execute loop */
  PA_FreeParserStack(Id);
  PA_FreeParserStack(Val);
}






/*****************************************************************************************
 Function:                     LOOP_InitForeachLoop
 Purpose: Initialize and lock  all loop variables of FOREACH loop.
 Returns: void.
 Side effects: Modifies LoopHeader appended to NestedBackPtr and symbol table  entries for
               loop variables.
*****************************************************************************************/
void LOOP_InitForeachLoop()
{
  register LoopVarStackType *Act;                    /* to scan  list of loop variables */
  
  /* check FIRST, if there are (more) iterations, or not */
  if((NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->TotalIter > 0) {
    (NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->ActIter = 1; 
    for(Act = ((NestedBackPtr->TokenPtr->Value).Loop->LoopHeader->LoopVars).Multi; Act;
	Act =  Act->NextVar) {   
      if(Act->Variable->Lock)                                /* scan all loop variables */
	ER_StackError("illegal attempt to modify loop variable '%s'\n", Serious,
		      STR_Save(Act->Variable->Name), nst, nst);
      else
	LOOP_AssignValUnion(Act->Variable, *(Act->InitValue));
      Act->Variable->Lock =  TRUE;                               /* lock loop variables */
      Act->Variable->UndefValue = FALSE;
      *(Act->ActValue = (ArrayElemType *) M_alloc(sizeof(ArrayElemType))) = 
	*(Act->InitValue);                       /* mark actual value of loop  variable */
    }
    ConditionValue = TRUE;
    FirstValue = TRUE;         /* indicates that loop var has been initialized recently */
  }
  else                                                       /* there are no iterations */
    ConditionValue =  FALSE;
}




/*****************************************************************************************
 Function:                     LOOP_AssignValUnion
 Purpose: Assign value to a variable.
 Returns: void.
 Side effects: variables symbol  table entry.
*****************************************************************************************/
void LOOP_AssignValUnion(Var, Val)
     SymTabLineType *Var;                           /* symbol  table entry for variable */
     ArrayElemType Val;                                       /* new value for variable */
{
  switch(Var->Type) {
  case XINT:
    Var->Value->intval = *(Val.aint);  break;                         /* simple integer */
  case XFLOAT:
    Var->Value->floatval = *(Val.afloat);  break;                       /* simple float */
  case STRING:
  case LONGSTRING:
  case SIMPLESTRING:
    Var->Value->string = *(Val.astring);  break;                        /* string types */
  case UNIT:
    Var->Value->unit = *(Val.aunit);  break;                             /* single unit */
  case STRUCTURE:
    Var->Value->structure = *(Val.astruct);  break;       
  case ArrStruct:              
  case ArrUnit:
  case ArrInt:                          /* array types - same action for all base types */   
  case ArrFloat:
  case ArrString:
    Var->Value->array = *(Val.aarray);  break;
  }
}





/*****************************************************************************************
 Function:                     LOOP_AssignIncr
 Purpose: Assign value to a variable.
 Returns: void.
 Side effects: variables symbol  table entry.
*****************************************************************************************/
void LOOP_AssignIncr(AVal, Val, Inc, Type)
     ArrayElemType *AVal;                                /* pointer to hold array value */
     ArrayElemType Val;                                       /* new value for variable */
     int Inc;                                     /* index of first array value element */
     SymbolType Type;
{
  switch(Type) {
  case XINT:
    AVal->aint = (Val.aint + Inc);  break;
  case XFLOAT:
    AVal->afloat = (Val.afloat + Inc);  break;
  case STRING:
  case LONGSTRING:
  case SIMPLESTRING:
    AVal->astring = (Val.astring + Inc);  break;
  case UNIT:
    AVal->aunit = (Val.aunit + Inc);  break;
  case STRUCTURE:
    AVal->astruct = (Val.astruct + Inc);  break;
  case ArrStruct:          
  case ArrUnit:
  case ArrInt:                          /* array types - same action for all base types */   
  case ArrFloat:
  case ArrString:
    AVal->aarray = (Val.aarray + Inc);  break;
  }
}






/*****************************************************************************************
 Function:                     LOOP_IncrementForeach
 Purpose: Increment values of foreach variables and set ConditionValue.
 Returns: TRUE if there are 0 iterations, else FALSE.
 Side effects: ConditionValue, symbol table and  ActValue entries for loop  variables.
*****************************************************************************************/
short LOOP_IncrementForeach(Head)
     LoopHeaderType *Head;
{
  register LoopVarStackType *Act;                    /* to scan  list of loop variables */
  short Val;             /* result value: TRUE <=> initial loop var value >> end  value */

  Val = FALSE;
  if( ! FirstValue) {          /* value will be incremented, loop done for first values */
    /* check FIRST, if there are (more) iterations, or not */
    if(Head->TotalIter > Head->ActIter) {                         /* further iterations */
      Head->ActIter ++;  
      /* scan all loop variables in list and increment them */
      for(Act = (Head->LoopVars).Multi; Act; Act =  Act->NextVar)                             
	LOOP_IncrementValUnion(Act);                               /* assign next value */
      ConditionValue = TRUE;
    }
    else {                   /* no more iterations, unlock variables, reset loop header */
      LOOP_RemoveVarList((Head->LoopVars.Multi));
      (Head->LoopVars).Multi = NULL;
      Head->ActIter = 0;                      /* causes reinitialization at next  entry */
      ConditionValue = FALSE;
    }
  }
  else {                         /* first value combination for loop - do not increment */
    FirstValue =  FALSE;                                  /* no iterations of loop body */
    if(Head->TotalIter >= Head->ActIter)   /* >=, since at least 1st. iteration is done */
      ConditionValue = TRUE;
    else {                                                           /* no iterations ! */
      Val = TRUE;
      LOOP_RemoveVarList((Head->LoopVars.Multi));
      (Head->LoopVars).Multi = NULL;
      Head->ActIter = 0;                      /* causes reinitialization at next  entry */
      ConditionValue =  FALSE;
    }
  }
  return Val;
}




/*****************************************************************************************
 Function:                     LOOP_IncrementValUnion
 Purpose: Increment actual value.
 Returns: void.
 Side effects: variables symbol  table entry, 2nd. parameter
*****************************************************************************************/
void LOOP_IncrementValUnion(Act)
     LoopVarStackType *Act;
{
  switch(Act->Variable->Type) {
  case XINT:
    Act->ActValue->aint++;
    Act->Variable->Value->intval = *(Act->ActValue->aint);  
    break;
  case XFLOAT:
    Act->ActValue->afloat++;
    Act->Variable->Value->floatval = *(Act->ActValue->afloat);  
    break;
  case STRING:
  case LONGSTRING:
  case SIMPLESTRING:
    Act->ActValue->astring++;
    Act->Variable->Value->string = *(Act->ActValue->astring);  
    break;
  case UNIT:
    Act->ActValue->aunit++;
    Act->Variable->Value->unit = *(Act->ActValue->aunit);  
    break;
  case STRUCTURE:
    Act->ActValue->astruct++;
    Act->Variable->Value->structure = *(Act->ActValue->astruct);  
    break;
  case ArrStruct:          
  case ArrUnit:
  case ArrInt:                          /* array types - same action for all base types */   
  case ArrFloat:
  case ArrString:
    Act->ActValue->aarray++;
    Act->Variable->Value->array = *(Act->ActValue->aarray);  
    break;
  }
}






/*****************************************************************************************
 Function:                     LOOP_RemoveVarList
 Purpose: Remove a list of FOREACH loop variables and unlocks the correspondent symbol
          table entries.
 Returns: void.  
*****************************************************************************************/
void LOOP_RemoveVarList(Var)
     LoopVarStackType *Var;           /* pointer to first element of loop variable list */
{
  if( ! FatalErrs && ! SeriousErrs && ! WarningErrs ) {
    if(Var) {
      if(Var->NextVar)
	LOOP_RemoveVarList(Var->NextVar);                        /* remove tail of  list  */
      Var->Variable->Lock =  FALSE;                              /* unlock loop variables */
      LOOP_FreeLoopVar(Var);                 /* remove loop  variable description  record */
    }
  }
}





/*****************************************************************************************
 Function:                     LOOP_UnlockLoopVars
 Purpose: Unlock loop variables of a for or foreach loop.
 Returns: void.
 Side effects: Modifies loop variable symbol taable entries ( ->Lock).
*****************************************************************************************/
void LOOP_UnlockLoopVars(Head, Multi)
     LoopHeaderType *Head;                    /* header for loop - holds loop variables */
     short Multi;                    /* foreach loop - more than a single loop variable */
{
  register LoopVarStackType *Act;     /* points to entry for actually unlocked loop var */
  
  if(Head)
    if(Multi) {     /* foreach loop - more than a single loop variable - scan variables */  
      for(Act = (Head->LoopVars).Multi; Act; Act =  Act->NextVar) {
	if(Act->Variable)
	  Act->Variable->Lock = FALSE;                         /* unlock loop variables */
	LOOP_FreeLoopVar(Act);             /* remove loop  variable description  record */
      }
      (Head->LoopVars).Multi = NULL; 
    }
    else {                 /* for loop - header holds pointer to a single loop variable */
      (Head->LoopVars).Single->Lock  = FALSE;                   /* unlock loop variable */
      (Head->LoopVars).Single = NULL;
    }
}






/*****************************************************************************************
 Function:                     LOOP_GetTokenStack
 Purpose: Allocate a new TokenStackType record, if necessary.
 Returns: & Initialized TokenStackType record.
 Side effects: Modifies FreeTokenStack.
*****************************************************************************************/
TokenStackType *LOOP_GetTokenStack()
{
  TokenStackType *New;

  if(FreeTokenStack)
    FreeTokenStack = (New = FreeTokenStack)->NextToken;
  else 
    New = (TokenStackType *)  M_alloc(sizeof(TokenStackType));
  New->NextToken = NULL;        /* other values aare always set, must not be initialized */
  New->Value.Loop  = NULL;
  New->Value.Simple  = NULL;
  New->Value.Cond  = NULL;
  return New;
}






/*****************************************************************************************
 Function:                     LOOP_FreeTokenStack
 Purpose:  Save an unused TokenStackType record on the stack FreeTokenStack points to.
 Returns: void.
 Side effects: Modifies FreeTokenStack.
*****************************************************************************************/
void LOOP_FreeTokenStack(Tok)
     TokenStackType *Tok;
{
  Tok->NextToken = FreeTokenStack;
  FreeTokenStack = Tok;
  /* other components must not be set  to NULL, are always initialized */
}





/*****************************************************************************************
 Function:                     LOOP_GetLoopHeader
 Purpose: Allocate and initialize a new LoopHeaderType record.
 Returns: &LoopHeaderType.
*****************************************************************************************/
LoopHeaderType  *LOOP_GetLoopHeader()
{
  LoopHeaderType  *New;
  
  New = (LoopHeaderType  *) M_alloc(sizeof(LoopHeaderType ));
  New->ActIter = 0;    /* 0 indicates that FOR  loop variable has not been initialized */
  New->TotalIter = -1;                                         /* must be < ActIter !! */
  New->LoopVars.Single = NULL;
  return New;
}





/*****************************************************************************************
 Function:                     LOOP_FreeLoopHeader
 Purpose: Free a LoopHeaderType record.
 Returns: void.
 Side effects: Removes argument.
*****************************************************************************************/
void LOOP_FreeLoopHeader(Arg)
  LoopHeaderType *Arg;
{
  (void) free(  (char*) Arg);
}






/*****************************************************************************************
 Function:                     LOOP_GetLoopValue
 Purpose: Allocate and initialize a new LoopValueType record.
 Returns: &LoopValueType.
*****************************************************************************************/
LoopValueType  *LOOP_GetLoopValue()
{
  LoopValueType  *New;
  
  New = (LoopValueType  *) M_alloc(sizeof(LoopValueType));
  New->ConditionPtr =  NULL;
  New->InitialPtr =  NULL;
  New->Continue =  NULL;
  New->Abort =  NULL;
  New->LoopHeader  = NULL;
  return New;
}





/*****************************************************************************************
 Function:                     LOOP_FreeLoopValue
 Purpose: Free a LoopValueType record.
 Returns: void.
 Side effects: Removes argument.
*****************************************************************************************/
void LOOP_FreeLoopValue(Arg)
  LoopValueType *Arg;
{
  (void) free(  (char*) Arg);
}






/*****************************************************************************************
 Function:                     LOOP_GetCondValue
 Purpose: Allocate and initialize a new CondValueType record.
 Returns: &LoopValueType.
*****************************************************************************************/
CondValueType  *LOOP_GetCondValue()
{
  CondValueType  *New;
  
  New = (CondValueType  *) M_alloc(sizeof(CondValueType));
  New->IfCondition =  NULL;
  New->ThenStmts =  NULL;
  New->NextIf =  NULL;
  New->Executed = FALSE;
  return New;
}





/*****************************************************************************************
 Function:                     LOOP_FreeCondValue
 Purpose: Free a CondValueType record.
 Returns: void.
 Side effects: Removes argument.
*****************************************************************************************/
void LOOP_FreeCondValue(Arg)
  CondValueType *Arg;
{
  (void) free(  (char*) Arg);
}







/*****************************************************************************************
 Function:                     Loop_GetLoopVar
 Purpose: Get a new LoopVarStackType record.  Initialize InitValue, set ActValue to NULL.
 Returns: & to LoopVarStackType.
 Side effects: Modifies FreeLoopVarStack.
*****************************************************************************************/
LoopVarStackType *LOOP_GetLoopVar()
{
  LoopVarStackType *New;

  if(FreeLoopVarStack) 
    FreeLoopVarStack = (New = FreeLoopVarStack)->NextVar;   /* get free record (stack) */
  else {
    New = (LoopVarStackType *) M_alloc(sizeof(LoopVarStackType));      /* allocate new */
    New->InitValue = (ArrayElemType *) M_alloc(sizeof(ArrayElemType));
    New->ActValue  = NULL;                /* mark that variable is not yet initialized */
    New->Variable = NULL;
  }
  New->NextVar =  NULL;
  return  New;
}





/*****************************************************************************************
 Function:                     LOOP_FreeLoopVar
 Purpose: Push a LoopVarStackType record onto the stack of unused records. Free ActValue.
 Returns: void.
 Side effects: Modifies FreeLoopVarStack and <argument> -> ActValue.
*****************************************************************************************/
void LOOP_FreeLoopVar(Arg)
     LoopVarStackType *Arg;
{
  if(Arg->ActValue)
    M_free((char *) Arg->ActValue);  /* remove actual value record for reinitialization */
  Arg->ActValue = NULL;
  Arg->Variable = NULL;
  Arg->NextVar = FreeLoopVarStack;                               /* stack unused record */
  FreeLoopVarStack = Arg;
}






/*****************************************************************************************
 Function:                     LOOP_RemoveTokenStack
 Purpose: Remove list of tokens.
 Returns: void. 
 Side effects: Removes list of tokens  appended to its argument.
*****************************************************************************************/
void LOOP_RemoveTokenStack(Arg)
     TokenStackType  *Arg;
{
  register TokenStackType  *Act; /* search pointer back to IF node (loop without keyw.) */
  if(Arg) {
    switch(Arg->Token) {
    case FOR:
      if(((Arg->Value).Loop)->LoopHeader->LoopVars.Single) {/* not removed - abort case */
	LOOP_UnlockLoopVars(((Arg->Value).Loop)->LoopHeader, FALSE);
	((Arg->Value).Loop)->LoopHeader->LoopVars.Single = FALSE;
      }                                                       /* continue in WHILE case */
    case  WHILE:
      LOOP_RemoveTokenStack(((Arg->Value).Loop)->ConditionPtr);          /* may be NULL */
      LOOP_RemoveTokenStack(((Arg->Value).Loop)->InitialPtr);            /* may be NULL */
      LOOP_RemoveTokenStack(((Arg->Value).Loop)->Continue);
      LOOP_RemoveTokenStack(((Arg->Value).Loop)->Abort);
      break;
    case FOREACH:
      LOOP_RemoveTokenStack(((Arg->Value).Loop)->ConditionPtr);
      LOOP_RemoveTokenStack(((Arg->Value).Loop)->InitialPtr);
      LOOP_RemoveTokenStack(((Arg->Value).Loop)->Continue);
      LOOP_RemoveTokenStack(((Arg->Value).Loop)->Abort);
      if(((Arg->Value).Loop)->LoopHeader->LoopVars.Multi) {  /* not removed - abort case */
	LOOP_UnlockLoopVars(((Arg->Value).Loop)->LoopHeader, TRUE);
	LOOP_RemoveVarList(((Arg->Value).Loop)->LoopHeader->LoopVars.Multi);
      }
      break;
    case IF:
    case ELSEIF:
      for(Act = ((Arg->Value).Cond)->IfCondition; Act && Act->NextToken != Arg; 
	  Act = Act->NextToken) ;
      if(Act)
	Act->NextToken = NULL;
      LOOP_RemoveTokenStack(((Arg->Value).Cond)->IfCondition); /* continue in ELSE case */
    case ELSE:
      if(((Arg->Value).Cond)->ThenStmts != Arg) {             /* stmt list is not empty */
	for(Act = ((Arg->Value).Cond)->ThenStmts; Act && Act->NextToken != Arg; 
	    Act = Act->NextToken)	;
	if(Act)
	  Act->NextToken = NULL;
	LOOP_RemoveTokenStack(((Arg->Value).Cond)->ThenStmts);
      }
      ((Arg->Value).Cond)->ThenStmts =  NULL;
      LOOP_RemoveTokenStack(((Arg->Value).Cond)->NextIf);
      LOOP_FreeCondValue((Arg->Value).Cond);
      break;
    case END:
    case THEN:
    case DO:
      LOOP_FreeTokenStack(Arg);
      break;
    default:
      LOOP_RemoveTokenStack(Arg->NextToken);
      LOOP_FreeTokenStack(Arg);
      break;
    }
  }
}
    
