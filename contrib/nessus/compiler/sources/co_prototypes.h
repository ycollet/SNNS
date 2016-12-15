/*****************************************************************************
  FILE              : co_prototypes.h
  SHORTNAME         : prototypes
  VERSION           : 2.0

  PURPOSE           : contains prototypes for all compiler functions 
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/



               /**** functions of co_symTab.c - symbol table management ****/

void SYM_InitSymTab();     /* Initialize symbol table manager and scanner functions. Returns: void */

SymTabLineType *SYM_LookUp();
                           /* Searches a key in the symbol table. If there's an entry for */
			   /* it, simply returns a pointer to the line of the symbol */
			   /* table. If the key is a new identifier, a new line is */
			   /* allocated. Returns a pointer to symbol table line */

void SYM_UpdateLine();     /* Store a new identifier value into symbol table. Type */
			   /* conflicts are detected if the identifier has been defined */
			   /* already. SymTab entries are not modified by other functions. */

void SYM_CopyLine();
                           /* looks for the new position of an old symbol table line in */
			   /* the new expanded symbol table, procedure is the same as */
			   /* SYM_LookUp. */

short SYM_GetNewPrime();
                           /* Calculates the greatest prime < Sup. */

short SYM_IsPrime();
                           /* Returns TRUE <=> argument is a prime. */ 

void SYM_InitTab();
                           /* Allocates and initializes a new symbol table. */

void SYM_InsertKeywords();
                           /* Inserts the keywords of nessus into the new symbol table. */

SymTabLineType *SYM_AllocLine();
                           /* Allocates a new symbol table line and initializes its */
			   /* fields. */

int SYM_Hash();
                           /* Hash function for symbol table management: string keys are */
			   /* transformed in an integer value adding up the characters */
			   /* numeric values. This integer value is then divided through */
			   /* the greatest prime no. < TabSize. */

short SYM_CheckTab();
                           /* Checks if a symbol table must be expanded. It is expanded if */
			   /* its density (Entries / Length) is greater than */
			   /* ExpandDensity. The Tabs actual density is calculated after */
			   /* each SymTabChecksDist insertions, rather than at each call */
			   /* of SYM_LookUp. Returns TRUE if symbol table has been expanded, */
                           /* else FALSE. */

short SYM_RemoveTab();
                           /* Removes a symbol table corresponding to an INNER net */
			   /* (subnet) from the stack of symbol tables. Resets environment */
			   /* to outer symbol table, saves environment of inner table to */
			   /* its SymTabType record. Does nothing if there's only one */
			   /* symbol table on the tables stack. */

SymTabLineType **SYM_ExpandTab();
                           /* Expands symbol table and copies all entries of old into new */
			   /* table. Modifies SymTabPtr (stack of symbol tables). */

SymTabLineType **SYM_AllocSymTab();
                           /* Allocates a new symbol table of specific length and returns */
			   /* a pointer to it. The table is not initialized ! */

void SYM_InsertNetName();  /* Inserts network or subnet name into upper SumTabType record */
			   /* on stack of symbol tables. Returns: void. Side effects: */
			   /* Modifies SymTabPtr->Network. */

#ifdef TEST
void SYM_PrintTab();       /* print symbol table on screen */
void SYM_PrintEndTab();    /* final call: SYM_PrintTab */
#endif


               /**** functions of co_connections.c - connection handling ****/

struct ConnListStruct *CONN_RemoveTab();

void CONN_InsertConnection();
                           /* Inserts a connection into the list. Receives target and */
			   /* source unit numbers and the connection weight. Returns: */
			   /* void. Side effects: Modifies CONN_Connections. */

void CONN_InsertSource();
                           /* Inserts a connections source unit and and weight into the */
			   /* table of source units attached to a determined site. */
			   /* Returns: void. Side effects: Table of source units. */

TargetUnitType *CONN_AllocTargetList();
                           /* Allocates table of connection targets (TargetUnitType) and */
			   /* initializes it. Returns: Pointer to new table. */

SourceEntryType *CONN_AllocSourceList();
                           /* Allocates table of connection sources (SourceEntryType) and */
			   /* initializes it. Returns: Pointer to new table. */

TargetSiteType *CONN_InitTargetSite();
                           /* Allocates new site record for a target unit entry in table */
			   /* of connection targets and initializes it. Returns: Pointer */
			   /* to new entry (TargetSiteType). */


ConnectionType *CONN_InitConnectionList();
                           /* Initialize list of connections (i.e. list of connection */
			   /* targets). Returns: Pointer to new list (ConnectionType). */

int CONN_Hash();           /* Hash function, similar to SYM_Hash. Returns: integer. */

TargetUnitType *CONN_LookUpTarget();
                           /* Look for an entry in target units table. Returns: pointer to */
			   /* entry (TargetUnitType). */

SourceEntryType *CONN_LookUpSource();
                           /* Look for an entry in source units table. Returns: pointer to */
			   /* entry (SourceEntryType). */

void CONN_CheckTargetDensity();
                           /* Checks wether a table of target units is full or not. */
			   /* If necessary, expands the table and inserts entries into new */
			   /* table. Returns: Pointer to new table. Side effects: Old */
			   /* table is removed, modifies CONN_Connections. */

TargetSiteType *CONN_CheckSourceDensity();
                           /* Checks wether a table of source units is full or not. */
			   /* Inecessary, expands the table and inserts entries into new */
			   /* table. Returns: Pointer to new table. Side effects: Old */
			   /* table is removed, modifies *OldSite. */

SourceEntryType *CONN_GetNextConnSource();
                           /* Look for list of explicit connections to a given unit. If */
			   /* there are connections, return pointer to next connection of */
			   /* list, else return NULL. Returns: & SourceEntryType (next */
			   /* connection) or NULL. */

void CONN_CopySubnetConnections();
                           /* Copy inner connections of a subnet. Receives upper and lower */
			   /* unit index. All connections whose target unit no. lies */
			   /* between upper and lower index are copied. "Copied" means */
			   /* that for a connection I_s -> I_t, I_s, I_t integer unit */
			   /* no.s,  a new connection I_s' -> I_t' is generated, with */
                           /* I_s' = I_s + Max - Min + 1 and I_t' = I_t + Max - Min + 1. */
			   /* Note that for all subnets, if the target unit index lies in */
			   /* [Min, Max], the same holds for the corresponding source unit */
			   /* index. Returns: void. Side effects: Modifies some connection */
			   /* table entries. Increments ConnectionCtr. */


                /**** functions of co_functions.c - functions handling ****/

struct FuncListStruct *FUNC_RemoveTab();     /* -- */
void CF_IncludeNewFunction();                /* -- */

void CF_InsertSiteFuncList();
                           /* Insert a new function into list of site functions. Returns: */
			   /* void. Side effects: Modifies lists of site functions. */

void CF_InsertActFuncList();
                           /* Insert a new function into list of activation functions. */
			   /* Returns: void. Side effects: Modifies lists of activation */
			   /* functions. */ 

void CF_InsertOutFuncList();
                           /* Insert a new function into list of output functions. */
			   /* Returns: void. Side effects: Modifies lists of site */
			   /* functions. */  
     

           /**** functions of co_scanner.c - for scanner generated by LEX ****/

int SC_GetType();
                           /* Searches an identifier in the symbol table. Returns the */
			   /* identifiers type, which is one of IDENTIFIER, VARIABLE, or */
			   /* one of the keyword types. An IDENTIFIER  is an identifier */
			   /* which has not been found in the table (has been inserted by */
			   /* SYM_LookUp), VARIABLE is an identifier, whose value and data */
			   /* type have been defined already. */

char SC_NextSymb();        /* Private input function for scanner generated by LEX. The */
			   /* input is buffered, input streams are mantained on a stack, */
			   /* since parser calls may be recursive. Returns: Next character */
			   /* of upper input stream on stack. */

short SC_InitInputStream();
                           /* Initializes a new input stream. Expects the name of a new */
			   /* input file to be on top of the input file names stack. This */
			   /* function is called whenever the parser generated by yacc */
			   /* begins to parse a new input file (<=> NewInput == TRUE). */
			   /* Returns TRUE if a new input stream has been opened, else */
			   /* FALSE (errors, nonexisting input file, etc.). Side effects: */
			   /* Modifies stack of input streams (InStreamTop), InputStream, */
			   /* and ActChr. */

void SC_ReloadInputStream();
                           /* Reloads current input stream. Appends a NULL character to */
			   /* the input stream if less than BUFSIZ charcters have been */
			   /* read. Modifies input stream of upper input stack record. */

short SC_PutInputStack();  /* Allocates a new input file desciption record and puts it on */
			   /* the stack of inputs (SC_InputStack). Modifies pointer to */
			   /* input stack SC_InputStack and SC_FreeInput. */

void SC_PopInputStack();   /* Pops upper element from stack of input files and stores it */
			   /* on the stack of unused input records (SC_FreeInput points to */
			   /* its top).Modifies both stacks, of current input */
			   /* (SC_InputStack) and of unused records (SC_FreeInput). */

int SC_Scanner();          /* Function which calls yylex() or simulates pseudo-input if */
			   /* there is a loop in input (FOR, FOREACH) */

int yylex();               /* scanner generated by LEX */

void SC_List();            /* list token string */

int SC_Recog();            /* Assign value to parser value stack record. Return token (for */
			   /* constants). */

int SC_GetType();          /* Assign value to parser value stack record. Return token (for */
			   /* identifier, keyword). */ 

double SC_CalcFloat();     /* Convert string representation of FLOATNUM into double. */
			   /* Returns: double. */
     



                        /**** functions of co_main.c  ****/

void main();

short EvalArguments();
                           /* Evaluates the compilers argument list. Returns TRUE if */
			   /* argument list has been correct, else FALSE. Side effects: */
			   /* Modifies InFileName, ListOption, and RCSOption. */

short EvalListOption();
                           /* Evaluates the argument of the -l option. Returns no. of next */
			   /* option if argument is correct, else FALSE. Side effects: */
			   /* Modifies ListFileName. */

short EvalOutOption();
                           /* Evaluates the argument of the -o option. Returns no. of next */
			   /* option if argument is correct, else FALSE. Side effects: */
			   /* Modifies OutFileName. */

short EvalParamOption();   /* Builds a string of parameter values specified in command */
			   /* line. Returns: No. of next option. Parameter value string */
			   /* is stored in NetArgVector. */

short InitCompiler();      /* Open source listing file. Returns FALSE, if there has been */
			   /* an error, else TRUE */

void Statistics();         /* Print some statistics on screen and into source listing file. */

void TE_Statistics();      /* suesses Nichtstun (bis jetzt) */

float GetRandom();         /* generate a new random number */



     

         /**** functions of co_strings.c  - functions for string manipulation ****/

char *STR_Save();          /* Purpose: Save string.  Returns: Pointer to copy of string. */
			   /* Note : This function is copied from Kernighan,Ritchie: */
			   /* Programming in C, pg. 111. */

int STR_AtoI ();           /* Transforms a string into the corresponding integer value; */
			   /* RETURNS : Integer. NOTE : Algorithm from Kernighan,Ritchie: */
			   /* Programming in C, pg. 44. */

char *STR_ItoA();          /* Purpose: Transform an integer to its corresponding string. */
			   /* Returns: String. */

double STR_AtoF ();        /* PURPOSE : Transform string 'abcd.xyz' into the corresponding */
			   /* real value (data type is double). RETURNS : Double. NOTE : */
			   /* Algorithm from Kernighan,Ritchie: Programming in C, pg. 65. */

char *STR_Append();        /* Appends 2nd. argument to a new copy of 1st. argument. The */
			   /* arguments are not affected. */

char *STR_Remove();        /* Removes 2nd. argument of first argument. The arguments are */
			   /* not affected. Returns: new string. */

char *STR_TypeString();    /* Builds a string to describe a token or variable value type. */
                           /* Returns: string. */

char *STR_RemDelimiters(); /* Removes string delimiters. Returns:  Pointer to 2nd. */
			   /* character of the string (first is assumed to be "). Side */
			   /* effects: Changes last " of string to \0. */

short STR_OrderedString(); /* Checks if a string consists only of characters and digits. */
			   /* Returns: TRUE if so, else FALSE. */

char *STR_New();           /* Save string. Similar to STR_Save, but pairs "\x" in strings */
			   /* are transformed into '\x' symbols. Returns: Pointer to copy */
			   /* of string. */

int STR_Ord();             /* Returns the order no. of a character. Returns: int. */

char STR_Chr();            /* Returns the character corresponding to a given order no. Returns: char */



     

      /**** functions of co_parseFunctions.c  - auxiliar functions for parser ****/

int yyparse();
                           /* parser generated by YACC */

int yyyylex();             /* Function to improve parser transparence. yyyylex calls yylex */
			   /* and prints the token if in debug mode. Returns integer value */
			   /* for character */

char *yydisplay();         /* function to display readable tokens */

int PA_StackTokens();      /* Same as yyyylex, but builds a stack of tokens to simulate */
			   /* for loops to yacc. */

int PA_GetLoopTokens();    /* still not implemented */

ParserStackType *PA_GetParserStack();
                           /* Allocates and initializes a new parser stack record, if */
			   /* there are no records free. Returns a pointer to */
			   /* ParserStackType record. Modifies PA_FreeParserPtr. */

void PA_FreeParserStack(); /* Push an unused ParserStackType record on the stack */
			   /* PA_FreeParserPtr. ->Value is preserved. Returns: void . Side */
			   /* effects: Modifies PA_FreeParserPtr.  

ParserStackType *PA_GetUndefValue();
                           /* Allocates and initializes a new parser stack record whose */
			   /* ytype component is Undef. This is needed to signalize errors */
			   /* to functions that perform other semantic actions. */
                           /* Returns: & ParserStackType. */

ParserStackType *PA_GetUndefString();
                           /* Allocates and initializes a new parser stack record whose */
			   /* ytype component is string. Needed to distinguish undefined */
			   /* strings from illegal expressions. The string value is set to */
			   /* NULL. Returns: & ParserStackType. */




         /**** functions of co_progArgs.c   -  evaluate program parameters ****/

void PA_AssignParamVal();  /* Assigns actual to formal program parameters. Returns: void. */

ParserStackType *PA_GetParameterValue();
                           /* Assign a parameter value to corresponding symbol table */
			   /* entry. Returns: Pointer to parser value stack record holding */
			   /* new value. */

void PA_UpdateParameter(); /* Insert a parameter value into symbol table entry. Type of */
			   /* formal and actual parameters must match.  Returns: void. */

ParserStackType *PA_StackPrm();
                           /* First argument points to head of a parser value stack */
			   /* records list. Appends 2nd. argument to this list. Returns: */
			   /* Ptr. to head of parser value stack records list. Side */
			   /* effects: Modifies ynext entry of last list element. */

void PA_AssignString();    /* Assign a string value to corresponding symbol table entry.*/

void PA_AssignNumber();    /* Assign a numerical value to corresponding symbol table entry.*/
     


     

            /**** functions of co_error.c  - functions for error handling ****/

void ER_StackError();
                            /* Puts an error message on the error messages stack. They are */
			    /* included into source listing before the next source line. */

ErrorStackType *ER_PopErrorStack();
                            /* allocates a new ErrorStackType record */

void ER_ListErrors();       /* Print error messages on terminal and into an eventual */
			    /* listing file. */

void ER_PushFreeError();    /* Removes the upper error description record from stack and */
			    /* place it on the stack of unused error description records. */
			    /* Free the records string components. Modifies ER_Errors, */
			    /* *ER_Errors, ER_FreeErrors. */




                        /**** co_memory.c - memory management ****/

char *M_alloc();            /* Allocate space, if possible, else print an error message */
			    /* and stop compiling. Modifies memory counter HeapSize. */
     
void M_SetHeapSize();       /* Increment actual and maxuimum heapsize. Returns: void. */

M_free();                   /* Same as free, with error control and exit. Returns: int */



                       /**** co_strings.c - string manipulation ****/

char *STR_Save();           /* Save string - from Kernighan Richie: Programming in C. */

int STR_AtoI ();            /* Transforms a string into the corresponding integer value. */
			    /* Algorithm from Kernighan,Ritchie: Programming in C, pg. 44. */
 
char *STR_ItoA();           /* Transform an integer to its corresponding string. */

double STR_AtoF();          /* Transform string 'abcd.xyz' into the corresponding real */
			    /* value. Returns double. Algorithm from Kernighan,Ritchie: */
			    /* Programming in C, pg. 65. */

char *STR_Append();         /* Appends 2nd. string argumnent to first. Returns new string. */





                     /**** co_expr - evaluate expressions ****/
void EX_AssertValue();      /* If the argument is the parser stack value of an variable or */
			    /* constant identifier (symbol table entry) checks if it has */
			    /* been defined. The value is put on the stack, if there has */
			    /* been a definition before, else EX_AssertValue displays an */
			    /* error message and marks the variable as undefined. */
                            /*Returns: void. Side effects: Modifies *yvalue and ytype of */
			    /*its argument. */

ParserStackType *EX_EvalSign();
                            /* Evaluates unary sign. Check if the operands data type is */
			    /* INTEGER or FLOATNUM. Returns: Expression result assigned to */
			    /* operand.  Side effects: Modifies operand. */

ParserStackType *EX_EvalNumExpr();
                            /* Evaluates a num. expression. The result is returned in the */
			    /* first operand record, the second operand is removed. */
			    /* Returns: Pointer to ParserStackType holding result. Side */
			    /* effects: Modifies both operands. */

ParserStackType *EX_EvalIntExpr();
                            /* Evaluates DIV and MOD. Returns: First operand holding the */
			    /* result value. Side effects: Modifies both operands. */

ParserStackType *EX_EvalStringExpr();
                            /* Evaluates STRCAT and REMOVE. Returns: First operand holding */
			    /* the result value. Side effects: Modifies both operands. */

ParserStackType *EX_EvalFloatOper();
                            /* Evaluate '+', '-', '*', or '/'. Both operands are of type */
			    /* FLOATNUM. Returns: First Argument holding the result value. */
			    /* Side effects: Both operands are modified. */

ParserStackType *EX_EvalIntOper();
                            /* Evaluate '+', '-', '*', or '/'. Both operands are of type */
			    /* INTEGER. Returns: First Argument holding the result value. */
			    /* Side effects: Both operands are modified. */

char *EX_StringOper();      /* Evaluate '/' or '\'. Both operands are strings. Returns: */
			    /* Result string. */

ParserStackType *EX_ListFunc();
                            /* Evaluates 'min' and 'max' functions. Argument is the */
			    /* expression list appended to EX_Listhead. This expression */
			    /* list may consist of a single ParserStackValue record whose */
			    /* value must be an array. Returns: Pointer to ParserStackType */
			    /* record holding result value. */

int EX_GetIntListFunc();    /* Evaluates 'min' and 'max' functions on integer arguments. */
			    /* The first element of expression list appended to */
			    /* EX_Listhead is one of the function arguments. Returns: */
			    /* Integer - maximum or minimum value of expression_list. */
float EX_GetFloatListFunc();   /* s.a. */
char *EX_GetStringListFunc();  /* s.a. */

void EX_GetIntArrayFunc();  /* Evaluates 'min' and 'max' functions on integer arrays. */
			    /* There must be exactly one element of type ArrInt in the */
			    /* expression list appended to EX_Listhead. Returns: void. */
			    /* Side effects: Modifies Result. */
void EX_GetFloatArrayFunc();   /* s.a. */
void EX_GetStringArrayFunc();  /* s.a. */

ParserStackType *EX_ConcatList();
                            /* Evaluates CONCAT function on an array or list of integers. */
			    /* Returns: Pointer to ParserStackType record holding */
			    /* resulting string. */

void EX_ConcatArray();      /* Evaluates CONCAT on string arrays. There must be exactly */
			    /* one element of type ArrString in the expression list */
			    /* appended to EX_Listhead. Returns: void. Side effects: */
			    /* Modifies Result. */


void EX_InitExprList();     /* Initialize stack of expressions (parser stack value for */
			    /* expression_list). Returns: void. Side effects: Header of */
			    /* list EX_ListHead. */

void EX_AppendExprList();   /* Append an element to the stack of expressions (parser stack */
			    /* value for expression_list). Returns: void. Side effects: */
			    /* Modifies header of list EX_ListHead. */

void EX_RemoveList();       /* Remove list appended to EX_ListHead. Returns: void. Side */
			    /* effects: Modifies  EX_ListHead. */

void EX_RemoveListTail();   /* Remove list of ParserStackType records. Returns: void. */

ParserStackType *EX_MakeExplicitArray();
                            /* Builds an array of values contained in an expression_list */
			    /* parser stack value. Does not deal with arrays defined by */
			    /* range. Returns: ParserStackType record holding the array */
			    /* structure, NULL if there is an error. */

ArrayValueType *EX_CopyIntArray();
                            /* Copies list of integer type elements into an array. Order */
			    /* of list elements is reversed! Returns: Pointer to */
			    /* ArrayValueType record. */

ArrayValueType *EX_CopyFloatArray();
                            /* Copies list of float type elements into an array. Order */
			    /* of list elements is reversed! Returns: Pointer to */
			    /* ArrayValueType record. */

ArrayValueType *EX_CopyStringArray();
                            /* Copies list of string type elements into an array. Order */
			    /* of list elements is reversed! Returns: Pointer to */
			    /* ArrayValueType record. */

ArrayValueType *EX_CopyStructArray();
                            /* Copies list of structures into an array. Order of list */
			    /* elements is reversed! Returns: Pointer to ArrayValueType */
			    /* record. */

ArrayValueType *EX_CopyUnitArray();
                            /* Copies list of explicitly defined units (FOREACH loop!) */
			    /* into an array. Order of list elements is reversed! Returns: */
			    /* Pointer to ArrayValueType record. */

ArrayValueType *EX_CopyArrayArray();
                            /* Copies list of explicitly defined array of 2-dimensional */
			    /* structures (arrays of structures) into an array. Order of */
			    /* list elements is reversed! Returns: Pointer to */
			    /* ArrayValueType record. */

ParserStackType *EX_MakeRangeArray();
                            /* Builds an array of a range definition. This array is not */
			    /* built explicitly. Returns: Pointer to ParserStackType */
			    /* record holding the array value or NULL, if there has been */
			    /* an error in one of the two bound definition expressions. */

ParserStackType *EX_BuildIntRange();
                            /* Build integer range array: Store the array length and its */
			    /* first value, rather than building it explicitly. Returns: */
			    /* Pointer to ParserStackType record holding array structure. */

ParserStackType *EX_BuildStringRange();
                            /* Build an array out of a range definition. The array is */
			    /* stored explicitly. Returns: Pointer to parser stack value */
			    /* holfing the array. */

void EX_InitMap();          /* Initialize a map description record with no. of lines, */
			    /* range, and domain data type. Range and domain data types */
			    /* are ArrInt, ArrFloat, and ArrString rather than INTEGER, */
			    /* FLOATNUM, LONGSTRING, or SHORTSTRING. */
			    /* Returns: void. Side effects: Modifies EX_MapConstant. */

void EX_InsertMapLine();    /*  Append a new domain and a new range array to the list of */
			    /*  map lines (EX_MapList). EX_ActMapLine points to the last */
			    /*  element of this list. Therefore, the list will become like */
	                    /* domain[0]-range[0]-domain[1]-range[1]- ... -domain[n]-range[n]. */
                            /* Returns: void. */
			    /* Side effects: Modifies EX_ActMapLine (increment) and */
			    /* *(EX_ActMapLine) (insert). */

ParserStackType *EX_FinishUpMap();
                            /* Build a ParserValueType record holding the evaluated map */
			    /* constant. */
			    /* Returns: Pointer to ParserValueType record holding the */
			    /* evaluated map constant. NULL, if there has been an error in */
			    /* the map definition (EX_MapConstant == NULL). */
			    /* Side effects: EX_MapConstant, EX_ActMapLine are set to NULL. */

ParserStackType *EX_EvalFileMap();
                            /* Evaluate a map read from a file.  Return the maps value in */
			    /* a parser value stack record. Returns: &ParserStackType */
			    /* record. */

void EX_SetSimpleType();    /* Transform array type into simple type. Needed to insert */
			    /* arrays into map constant. Returns: void. Side effects: */
			    /* Modifies <argument>->ytype. */

ParserStackType *EX_CorrectSize(); 
                            /* Checks for an expression if it defines a correct map or */
			    /* structure size. The expression value must be of type */
			    /* integer and > 0. Returns: &ParserStackType (argument). */
			    /* Side effects: If there is an uncorrectable error, */
			    /* Size->ytype is set to Undef to avoid further error */
			    /* messages. */

ParserStackType *EX_GetFieldElem();
                            /* Gets an array element specified by an expression like */
			    /* xx[yy]. Returns: ParserStackType record holding element */
			    /* value. */

ParserStackType *EX_SelectFieldElem();
                            /* Return an array element. Returns: ParserStack type, holdign */
			    /* an array element. */

ParserStackType *EX_ArctanFunc();

ParserStackType *EX_CosFunc();

ParserStackType *EX_SinFunc();

ParserStackType *EX_ExpFunc();

ParserStackType *EX_LnFunc();

ParserStackType *EX_SqrFunc();

ParserStackType *EX_SqrtFunc();

ParserStackType *EX_MinFunc();

ParserStackType *EX_MaxFunc();

ParserStackType *EX_ConcatFunc();

ParserStackType *EX_RandomFunc();
                             /* Return random float value in [-1.0, 1.0]. Returns: */
			     /* ParserStackType holding random value. */



            /**** co_logical.c - functions to evaluate logical expressions ****/

ParserStackType *LOG_EvalLogicalExpr();
                             /* Evaluates a logical expression. Both operands must be of */
			     /* type "Boolean". Operator must be AND or OR. Returns: */
			     /* Pointer to ParserStackType record of type "Boolean", */
			     /* holding TRUE or FALSE. Side effects: Both operands are */
			     /* removed. */

ParserStackType *LOG_EvalLogicalNot();
                             /* Evaluates  logical negation. Operand must be of type */
			     /* "Boolean". Returns: Pointer to ParserStackType record of */
			     /* type "Boolean", holding TRUE or FALSE. Side effects: */
			     /* Operand is removed. */

ParserStackType *LOG_EvalLogicalFunction();
                             /* Evaluates a logical function. All arguments in list must */
			     /* be of type "Boolean". Function keyword must be FORALL  or */
			     /* EXISTS. Returns: Pointer to ParserStackType record of type */
			     /* "Boolean", holding TRUE or FALSE. Side effects: Operand */
			     /* list is removed. */

ParserStackType *LOG_EvalRelation();
                             /* Evaluates a relational expression. Both operands must be */
			     /* of same data type. Returns: Pointer to ParserStackType */
			     /* record of type "Boolean", holding TRUE or FALSE.  Side */
			     /* effects: Both operands are removed. */

ParserStackType *LOG_EvalInclusion();
                             /* Evaluates a relational expression. Both operands must be */
			     /* of same data type. Returns: Pointer to ParserStackType */
			     /* record of type "Boolean", holding TRUE or FALSE. Side */
			     /* effects: Both operands are removed. */

ParserStackType *LOG_EvalMapRelation();
                             /* Evaluates a map relation. Returns: Pointer to */
			     /* ParserStackType record of type "Boolean", holding TRUE or */
			     /* FALSE. Side effects: All arguments are removed. */

ParserStackType *LOG_MapError();
                             /* Removes argument records and returns Undef. Returns: */
			     /* Pointer to ParserStackType record of type "Undef". Side */
			     /* effects: All arguments are removed. */

short LOG_EvalSimpleInclusion();
                             /* Evaluate inclusion relation for an integer and an array of */
			     /* integers. Returns: TRUE <==> element In array. Special */
			     /* assumptions: Elem is of Field's basis type. */

short LOG_EvalIntInclusion();
                             /* Evaluates inclusion relation for an integer and an array */
			     /* of integers. Returns: TRUE <==> element In array. */

short LOG_EvalFloatInclusion();
                             /* Evaluates inclusion relation for an float no. and an array */
			     /* of float values. Returns: TRUE <==> element In array. */

short LOG_EvalStringInclusion();
                             /* Evaluates inclusion relation for a string and an array of */
			     /* strings. Returns: TRUE <==> element In array. */

ParserStackType *LOG_CompareIntegers();
                             /* Compares two integer expressions. Both operands must be of */
			     /* same data type. Returns: Pointer to ParserStackType record */
			     /* of type "Boolean", holding TRUE or FALSE. Side effects: */
			     /* Both operands are removed. */

ParserStackType *LOG_CompareFloats(); 
			     /* Compares two float expressions. Both operands must be of */
			     /* same data type. Returns: Pointer to ParserStackType record */
			     /* of type "Boolean", holding TRUE or FALSE. Side effects: */
			     /* Both operands are removed. */

ParserStackType *LOG_CompareStrings();
			     /* Compares two string expressions. Both operands must be of */
			     /* same data type. Returns: Pointer to ParserStackType record */
			     /* of type "Boolean", holding TRUE or FALSE. Side effects: */
			     /* Both operands are removed. */




       /**** co_struct.c - functions to evaluate structure and type definitions ****/
ParserStackType *ST_InsertSite();
                             /* Builds list of site values (pointers to their */
			     /* correspondent symbol table entries). Returns: */
			     /* ParserStackType record (first element of site list). Side */
			     /* effects: Modifies last ->ynext entry in list. */

ParserStackType *ST_MakeSiteDef();

ParserStackType *ST_MakeSType();
                             /* Builds a parser value stack element to hold a units S type */
			     /* (input, output, hidden, or input and output). Returns: */
			     /* Pointer new ParserStackType record. This record holds S */
			     /* type in ->ytype, ->yvalue is not set. */

void ST_InitStructFType();
                             /* Inserts named type into ST_TempStruct. Returns: void */
                             /* Side effects: Modifies ST_TempStruct. */

void ST_AssertPos();

ParserStackType *ST_MakePos();

void ST_InitUnitType();

void ST_InsertUnitTypeParam();

ParserStackType *ST_CheckString();

ParserStackType *ST_CheckFloat();

ParserStackType *ST_FinishUpUnitType();

void ST_DefineType();       /* Insert a unit type definition into the symbol table. */
			    /* Returns: void. Side effects: Symbol table entry of Id. */

void ST_AssignStructures(); /* Insert a structure description into one or several symbol */
			    /* table entries ( CLUSTER of ... : var1, var2,...; -> one */
			    /* copy per variable ).  Returns: void. */
                            /* Side effects: Modifies some symbol table entries and */
			    /* ST_TempStruct is reset to NULL. */

void ST_AssignSubnet();     /* Insert a subnet description into structure entries of outer */
			    /* program. Returns: void. Side effects: Modifies some symbol */
			    /* table entries and ST_TempStruct is reset to NULL. */

SymTabType *ST_CopySubnet();/* Copy a subnets symbol table and the parser value stack */
			    /* entries of its variables (NOT CONSTANTS !!) or structures. */
			    /* Insert the  subnets structures into global net  components */
			    /* list */

ArrayValueType *ST_CopyArrayOfStructures();
                            /* Copy an array of structures, whose structure units may have */
			    /* been modified. Returns: Ptr. to ArrayValueType (new array). */



void ST_AssignArrayOfStructure();
                            /* Insert an array of structures into one or several symbol */
			    /* table entries ( ARRAY of ... : var1, var2,...; -> one copy */
			    /* per variable). Returns: void. Side effects: Modifies some */
			    /* symbol table entries and ST_TempStruct is reset to NULL. */

void ST_InitUnitValue();    /* Allocates a new UnitValueType record to collect parameters */
			    /* of structure definitions.  ST_TempStruct will point to this */
			    /* new record. Returns: void. Side effects: Modifies ST_TempStruct. */

void ST_InsertStructSize(); /* Insert size definition into ST_TempStruct. The size */
			    /* expression is assumed to be an integer value > 0 (already */
			    /* checked by EX_CorrectSize. Returns: void. */
                            /* Side effects: Modifies *ST_TempStruct. */

void ST_InitExplUnits();    /* Build an array of UnitType records - one for each unit of */
			    /* the structure. Initialize the UnitType records (NOT default */
			    /* values). Does nothing, if this array already exisits. */
			    /* Returns: void. */
                            /* Side effects: Modifies Node->ExplUnits and array it points to. */

void ST_InsertWeight();     /* Insert weight of implicit connections into UnitValueType */
			    /* Record. If weight is RANDOM, the randomly distributed */
			    /* values are generated later in ST_AssignStructures. */
                            /* Returns: void. Side effects: Modifies ST_TempStruct. */

void ST_CheckSite();        /* Checks if an identifier is name of a site type. Returns: */
			    /* void. */

ParserStackType *ST_CheckFunction();
                            /* Checks if an identifier to be used as function is */
			    /* undefined. It must be undefined, since functions are */
			    /* defined at the end of a nessus program. If a conflict is */
			    /* detected, the function prints an error message. The */
			    /* identifier is redefined !! */
                            /* Returns: Pointer to argument, ParserStackType. */
                            /* Side effects: Modifies symbol table entry for identifier. */

UnitValueType *ST_CopyUnitValue();
                            /* Copies a UnitValueType record including explicit units and */
			    /* VarFT parameters. Returns: &UnitValueType -- pointer to */
			    /* copy. Side effects: Modifies center specifificaation of */
			    /* original structure record !! */

UnitType **ST_CopyExplicitUnitList();
                            /* Builds a new list of explicit units and copies the units */
			    /* parameters of the old list to the correspondent new units. */
			    /* Returns: pointer to array of explicit units (& UnitType). */

UnitType *ST_SingleExplicitUnit();
                            /* Allocate a single record for an explicit unit. Initialize */
			    /* fields of record. ALL ENTRIES ARE SET TO THOSE OF */
			    /* STRUCTURE DESCRIPTION RECORD !!! Returns: pointer to */
			    /* explicit unit (& UnitType). */

void ST_InitStructExpl();   /* initialize activation values and connection weights for a */
			    /* new copy of an UnitValueType record. */
void ST_InitStructTypeExpl();
                            /* Insert record for explicit unit type parameters into */
			    /* ST_TempStruct. Returns: void. Side effects: Modifies */
			    /* ST_TempStruct. */

void ST_InsertStructSites();/* Insert sites into ST_TempStruct. Returns: void. Side */
			    /* effects: Modifies ST_TempStruct. */

void ST_InsertMap();        /* ??? */

void ST_InsertStructName(); /* Inserts a  unit name into ST_TempStruct. Check if there is */
			    /* a redefinition. Returns: void. Side effects: Modifies */
			    /* ST_TempStruct. */

void ST_CheckUnitCompleteness();

void ST_InsertStructSType ();
                            /* Insert S type (INPUT, OUTPUTM, HIDDEN, InOut) into */
			    /* ST_TempStruct. Check if there is a redefinition. Returns: */
			    /* void. Side effects: Modifies ST_TempStruct. */

void ST_InsertDirection();  /* Inserts S type (INPUT, OUTPUTM, HIDDEN, InOut) into */
			    /* ST_TempStruct. Check if there is a redefinition. Returns: */
			    /* void. Side effects: Modifies ST_TempStruct. */
void ST_InsertThrough();

short ST_AssertSite();      /* Checks wether a site specification is correct (defined */
			    /* previously, correct data type) or not. Returns: TRUE if */
			    /* site has been correct, else FALSE. */
void ST_InsertStructCenter();
                            /* Inserts center for STAR struct into ST_TempStruct. Check if */
			    /* structure is really a STAR. Returns: void. Side effects: */
			    /* Modifies ST_TempStruct. Special assumptions: */
			    /* CenterPtr->yvalue->intval >= 0 !!  (EX_CheckFieldSize();) */

void ST_InsertStructFunction(); 
                             /* Insert activation or output function definition  into */
			     /* ST_TempStruct. Returns: void. Side effects: Modifies */
			     /* ST_TempStruct. Note: Functions are not defined before */
			     /* their first use -- identifier data type must be undefined */
			     /* in symbol table !  */

void ST_InsertTopCenter();   /* Insert  center specification for structure topology into */
			     /* structure description record (UnitValueType). Center must */
			     /* be defined in nessus program. Returns: void. Side effects: */
			     /* Modifies ST_TempStruct->Topology-> {XCenter, YCenter}. */
			     /* Removes argument records. */

void ST_InsertTopology();    /* Insert  topology specification for structure topology into */
			     /* structure description record (UnitValueType). Topology */
			     /* must be defined in nessus program. A structure topology */
			     /* specification consists of top_form (plane, matrix, */
			     /* ellipse), extensions of this form (in x and y directions, */
			     /* > 0). Returns: void. Side effects: Modifies */
			     /* ST_TempStruct->Topology-> {XCenter, YCenter}. Removes */
			     /* argument records. Special assumptions: The form argument */
			     /* is MATRIX or ELLIPSE. The form PLANE is converted into a */
			     /* matrix with extension 1 in either x or y direction */
			     /* (horizontal or vertical plane). */

void ST_CheckPosition();     /* Checks wether a parser stack value is an integer >= 0 */
			     /* which can be used as position for a structures topology */
			     /* specification or not. Returns: void. Side effects: Sets */
			     /* Argument->ytype if argument is not suitable to be used as */
			     /* position coordinate. */

void ST_CheckExtension();    /* Checks wether a parser stack value is an integer > 0 which */
			     /* can be used as dimension for a structures topology */
			     /* specification or not. Returns: void. Side effects: Sets */
			     /* Argument->ytype if argument is not suitable to be used as */
			     /* dimension coordinate. */

void ST_InsertMap();         /* Inserts connections into an IRREGULAR structure. The */
			     /* structure is transformed into CLUSTER. Returns: void. Side */
			     /* effects: Modifies ST_TempStruct->StructType, removes */
			     /* argument. */
void ST_InsertMapConnections();
                             /* Generate explicit connections among those units of */
			     /* ST_TempStruct, whose indices are related. */
                             /* Returns: void. */

void ST_InsertPair();        /* Insert a connection between two units into list of */
			     /* explicit connections. Returns: void. */

ParserStackType *ST_BuildRangeSelection();
                             /* Builds a ParserStackType record holding an array or map */
			     /* range in a RangeType record. Check if range delimiters ly */
			     /* within the array or map bounds. Returns: Pointer to new */
			     /* ParserStackType record. */

ParserStackType *ST_FinishUpStructure();
                             /* Takes ST_TempStruct and builds a ParserStackType record */
			     /* for it. Returns: &ParserStackType. Side effects: */
			     /* ST_TempStruct is reset to NULL; */

ParserStackType *ST_MakeSingleUnit();

                             /* Takes ST_TempStruct and builds a ParserStackType record */
			     /* for it. ST_TempStruct holds a single unit. Returns: */
			     /* &ParserStackType. Side effects: ST_TempStruct is reset to */
			     /* NULL; */

ParserStackType *ST_MakeArrayOfStructures();
                             /* Takes a structure as first argument and builds an array of */
			     /* copies of this structure. The 2nd. argument defines the */
			     /* length of this array. For each copy of the structure, a */
			     /* new position is generated. Returns: &ParserStackType */
			     /* holding new array of structures. */

void ST_RemoveStructure();   /* Removes a structure. Returns: void. Side effects: ! :-) ! */

void ST_StackType();         /* Stack and count site and unit types. Returns: void. */
                             /* Side effects: ST_UnitTypes, ST_SiteTypes, UnitTypeCtr, */
			     /* SiteTypeCtr.*/ 

void ST_UpdateConnectionCounter();
                             /* Increment ConnectionCtr for new structure (implicit */
			     /* connections). */

void ST_InsertNetComponent(); 
                             /* Insert an identifier into list of net components. */
			     /* Identifier must be of type ArrStruct, UNIT or STRUCTURE. */
			     /* Returns: void. Side effects: NetComponents. */

void ST_InsertTypeList();    /* Insert an identifier into global list of types. Returns: */
			     /* void. Side effects: UnitTypes. */

void ST_InsertStructAct();   /* Insert activation value into structure. Activation value */
			     /* may be a random number. Check if activation is parameter */
			     /* of an eventual FType, do NOT modify FType-parameters. */
			     /* Function parameter Act is assumed to hold a float value of */
			     /* [-1.0, 1.0] if ytype is float, else ytype is RANDOM or */
			     /* Undef. Returns: void. Side effects: Modifies *Struc. */

void ST_InsertExplValues();  /* Insert explicit values of an array into structure units. */
			     /* Returns: void. Side effects: Modifies structure units and */
			     /* removes the values array if there's no name for it.  */

short ST_IsCorrectParamList();
                             /* Checks if a parameter list defined through an array or a */
			     /* range fits into the structure of ST_TempStruct. Checks */
			     /* data type of range elements and unit parameter. Returns: */
			     /* TRUE <==> ok, else FALSE. Side effects: Call to */
			     /* STMT_GetIdValue. */

void ST_GetFirstValue();     /* Get first array or array range value. Returns: void. Side */
			     /* effects: Modifies *Val;  */

void ST_GetLastValue();      /* Get last array or array range value. Returns: void. Side */
			     /* effects: Modifies *Val;  */

void ST_InsertStructDirection();
                             /* Insert S type (INPUT, OUTPUTM, HIDDEN, InOut) into */
			     /* ST_TempStruct. Check if there is a redefinition. Returns: */
			     /* void. Side effects: Modifies ST_TempStruct. */

ParserStackType *ST_FinishUpSubnet();
                             /* Removes a symbol table and an input stream from  their */
			     /* correspondent stacks and inserts them into a parser value */
			     /* stack record (will be inserted into outer symbol table). */
			     /* Returns: pointer to  parser value stack record holding */
			     /* subnet symbol table. Side effects:  Modifies SymTabPtr. */




                     /**** co_ites.c - functions to manage list of sites ****/
ParserStackType *SI_MakeSite();
                             /* Purpose: Build a SiteListRecord and check if site name is */
			     /* unique. Insert new record into sorted list of site */
			     /* description records (SiteList) and return pointer to it. */
			     /* Returns: Pointer to new site description record */
			     /* (SiteListType) or NULL, if there's been an error. Side */
			     /* effects: Modifies SiteList and records appended to list. */

void SI_CountSiteTypes();    /* Count sites and set SiteTypeCtr. Returns: void. */

void ST_SaveMap();           /* Save map for building connections according to it. */
			     /* Returns: void. Side effects: Modifies ST_ActMap. */

void ST_ClearMap();          /* Remove map when the connections are built. Returns: void. */
			     /* Side effects: Modifies ST_ActMap. */






                   /**** co_stmt.c - functions to evaluate statements ****/
void STMT_ValConstant();    /* Assign an expression value to a constants symbol table */
			    /* entry. Check if the constant has been defined previously. */
			    /* Returns: void. Side effects: Modifies  symbol table entry */
			    /* for constant. */

ParserStackType *STMT_BuildType();
                            /* build a parser value stack type record holding INTEGER, */
			    /* FLOATNUM, or STRING in ->ytype. */

ParserStackType *STMT_BuildArrayType();
                            /* transform parser value stack type record returned by */
			    /* STMT_BuildType into equivalent record, where ->ytype holds */
			    /* ARRAY OF simple_type */

void STMT_RemoveValue();    /* If a variable or constant is redefined, its old value can */
			    /* be removed since an identifiers value cannot be on parser */
			    /* value stack while variable declarations are parsed. */
			    /* Returns: void. Side effects: Removes value of symbol table */
			    /* entry. */

void STMT_GetIdValue();     /* Evaluates an identifier. Returns: Parser value stack record */
			    /* holding the identifiers value which may be undefined. Side */
			    /* effects: Changes the arguments type and value. */

ParserStackType *STMT_GetElement();
                            /* Evaluates selection of an array or structure element. */
			    /* Returns: Parser value stack record holding element value. */
			    /* Side effects: Removes both arguments (parser value stack */
			    /* records holding array and index values). Special */
			    /* assumptions: Index value is integer and >= 0. */

ParserStackType *STMT_GetArrayElement();
                            /* Evaluates selection of an array element. Returns: Parser */
			    /* value stack record holding element value. Side effects: */
			    /* Removes both arguments (parser value stack records holding */
			    /* array and index values). Special assumptions: Index value */
			    /* is not out of the array bounds. */

ParserStackType *STMT_GetStructureElem();
                            /* Evaluates selection of a structure element. The value */
			    /* returnde depends on wether the expression is at the left */
			    /* side of a statement or not. Suppose our element has not */
			    /* been changed before - then there's no explicit UnitType */
			    /* record for it. If this element is selected at the right */
			    /* side of a statement, we can return a pointer to the */
			    /* structure units description record (f.i. */
			    /* ST_TempStruct->UnitDef). The unit value and its components */
			    /* can be read only. On the left side of an assignment */
			    /* statement the unit value cannot be read, it has been */
			    /* selected to be changed. Therefore, we must return a pointer */
			    /* to the units explicit description record, although it may */
			    /* not contain any significant value.  Returns: Parser value */
			    /* stack record whose value points to a UnitType record. This */
			    /* record is "read-only" if we are currently parsing the right */
			    /* side of an assignment  statement, it is "write-only" if we */
			    /* are on the left side. Side effects: Removes both arguments */
			    /* (parser value stack records holding array and index */
			    /* values). */

ParserStackType *STMT_GetUnitComponent();
                            /* Evaluates selection on an argument of type UNIT (act or */
			    /* name). Returns: &ParserStackType holding result. Side */
			    /* effects: Removes its first argument. */

ParserStackType *STMT_SelectIdentifierComponent();
			    /* Evaluates selection of a subnet component or unit site. */
			    /* Selection of a subnet component is done immediately, */
			    /* selection of a unit site is stored into a SelectionType */
			    /* record. Returns: &ParserStackType holding result. Side */
			    /* effects: Removes both first arguments. */

short STMT_SeekSite();      /* Looks for a site of a unit. */
			    /* Returns: TRUE <==> site has been found. */


void STMT_DeclareVar();     /* Inserts type information about variables into their */
			    /* correspondent symbol table entries. Checks if there is a */
			    /* conflict (var_name is a constant, redefinition of variable */
			    /* type, etc.). Returns: void. Side effects: Modifies symbol */
			    /* table entries of variables. */

void STMT_AssignValue();    /* Assign an expression value to a variables symbol table */
			    /* entry. Check previous data type of variable. Returns: void. */
			    /* Side effects: Modifies symbol table entry for variable. */

void STMT_AssignToComponent();
                            /* Assign an expression value to a unit component. Returns: */
			    /* void. Side effects: Modifies unit component, allocates */
			    /* explicit unit record, if necessary. */

void STMT_MakeExplicitConnection();
                            /* Build an explicit connection. Returns: void. */





                    /**** co_io.c - functions for nessus i/o ****/

ParserStackType *FI_OpenFile();
                            /* Open a file, return pointer to file, type of entries, and */
			    /* name in a FileValueType entry of a ParserStackType record. */
			    /* Returns: ParserStackType record holding FileValueType */
			    /* record. */
void FI_CloseFile();        /* Close file described by argument. Returns: void. */
                            /* Side effects: Modifies argument record. */
ParserStackType *FI_ReadArrayFile();
                            /* Reads a file of integers, floats, or strings which are to */
			    /* be used as array. Builds an array ParserStackType record */
			    /* and returns it (to return values AND length of array). */
			    /* Returns: Pointer to ParserStackType record holding array or */
			    /* NULL if file cannot be read. */
ParserStackType *FI_ReadIntFile();
                            /* Reads a file of integers which is to be used as array of */
			    /* int. Builds an array ParserStackType record and returns it */
			    /* (to return values AND length of array). Returns: Pointer to */
			    /* ParserStackType record holding array of integer values. */
			    /* Side effects: File is closed, file pointer in <parameter> */
			    /* -> FilePtr is set to NULL. File can be read only once. */
ParserStackType *FI_MakeArrayOfInt();
                            /* Builds a ParserStackType record to hold input array. Copy */
			    /* array to get exact length. Returns: Pointer to */
			    /* ParserStackType record holding array values, size and type. */
			    /* Side effects: Array passed as argument is removed. */
char *FI_EvalBackInt();     /* Evaluates the short background buffer. First posititions of */
			    /* buffer are occupied by the first digits of an integer to */
			    /* which all leading digits of input stream must be appended. */
			    /* FI_EvalBackInt then reads this integer, assigns it to the */
			    /* actual array position and clears the background buffer. */
			    /* Returns: Pointer to next symbol in input stream (after read */
			    /* integer). Side effects: *IntPos is modified. */
int  FI_ReadIntegerBuffer();/* Read a set of integers from a buffer range market by 2 */
			    /* &char. Assign these integers to array of int elements. */
			    /* Expand this array of int if necessary. Returns: int - no. */
			    /* of integer values read. Side effects: Modifies *IntField */
			    /* and *LenPtr if *IntField is expanded. The old *Intfield is */
			    /* removed. */
int *FI_IntCopy();          /* Copy an array of integers (size OldLength) into an array of */
			    /* integers (size NewLength). Remove old array of integers. */
			    /* Exactly OldLength elements are copied. Returns: &int - new */
			    /* array of integers. Side effects: *OldArray is modified */
			    /* (removed). */
ParserStackType *FI_ReadFloatFile();
                            /* see FI_ReadIntFile() */
ParserStackType *FI_MakeArrayOfFloat();
                            /* see FI_MakeArrayOfInt() */
char *FI_EvalBackFloat();   /* see FI_EvalBackInt() */
int FI_ReadFloatBuffer();   /* see FI_ReadIntegerBuffer() */
float *FI_FloatCopy();      /* see FI_IntCopy() */
ParserStackType *FI_ReadStringFile();
                            /* see FI_ReadIntFile() */
ParserStackType *FI_MakeArrayOfString();
                            /* see FI_MakeArrayOfInt() */
char *FI_EvalBackString();  /* see FI_EvalBackInt() */
int FI_ReadStringBuffer();  /* see FI_ReadIntegerBuffer() */
char **FI_StringCopy();     /* see FI_IntCopy() */

short FI_ReadMap();         /* Read a file, containing a map constant. Returns: TRUE <==> */
			    /* map constant has been read correctly, else FALSE. Side */
			    /* effects: Modifies MapHead,  removes MapSep and LineSep. */

short FI_MakeError();       /* Error handling: Remove arguments and return FALSE. Returns: */
			    /* FALSE. Side effects: Arguments are removed. */

short FI_CorrectMap();      /* Check if parameters are a vaalid specification of a map */
			    /* file and header. Returns: TRUE <=> parameters are ok, file */
			    /* can be read, else FALSE. Side effects: Modifies LineSep, if */
			    /* it has been NULL  before (==> "\n" is default). */

int FI_ReadMapBuffer();     /* Read  entries of input buffer  form Low to Up. The input */
			    /* buffer section does not contain any map domain / range or */
			    /* line separator strings. Returns: Total no. of values read */
			    /* for this domain or range set. Side effects: Modifies */
			    /* *ActVPtr. */

char *FI_MarkEnd();         /* Look for last separator (NOT MAP sepatator !) symbol of */
			    /* input buffer. Returns: Pointer to this symbol. */

char *FI_MarkSep();         /* Look for next separator string in input buffer section. */
			    /* Returns: Pointer to begin of separator string, if it has */
			    /* been found. NULL if there has been no separator string. */
			    /* Upper bound + 1 if there has been an error (line separator */
			    /* in DOMAIN state or domain/range separator in RANGE state). */

char *FI_EvalSep();         /* Eval correct separator string: change  state, allocate new */
			    /* array for  values, store old array of values into current */
			    /* map line and - if state has been RANGE - generate a new map */
			    /* line. Returns: Pointer to first symbol in input buffer */
			    /* which follows separator string. Side effects: Modifies */
			    /* *StPtr, *Line, *LNo, *LMax, *ActVPtr, *Values, *VNo, *VMax. */

void FI_InitMapLine();      /* Initialize new Values list. Returns: Pointer to new values */
			    /* list. Side effects: Modifies *Act, *NoPtr, *MaxPtr. */

short FI_EvalList();        /* Finish up an aarray of  values and insert it into map line */
			    /* domain or range. Returns: void. Side effects: Modifies */
			    /* *Line, Values. */

char *FI_EvalBackMap();     /* Evaluate background buffer. Background buffer contains the */
			    /* leading symbols of either an input value or a separation */
			    /* string. FI_EvalBackMap completes the contents of background */
			    /* buffer and evaluates it. Returns: Pointer to next input */
			    /* symbol of main buffer. Side effects: Modifies *Values, */
			    /* *ActVPtr. */

short FI_IsSeparator();     /* Checks if the string overlapping Back and Main buffers is a */
			    /* given separator. If so, all leading symbols of Main */
			    /* pertencing to the separator string  are copied into  Back. */
			    /* Returns: TRUE <==> string overlapping Back and Main buffers */
			    /* is separator. Side effects: Modifies Back. */

ArrayValueType *FI_MakeMapArray();
                           /* Finissh up a value list. Allocate an ArrayValueType record */
			   /* and let it hold the values list. Returns: Pointer to an */
			   /* ArrayValueType record which holds values array. Side */
			   /* effects: Original array  "Values" is reallocated. */

short FI_MakeMap();        /* Finish up map. Returns: TRUE. Side effects: Modifies Map. */



                /**** co_loop.c - evaluate FOR, WHILE, aand FOREACH  statements ****/

int LOOP_GetNextStackValue();
                           /* Get next token from stack. Change state of parser, if */
			   /* necessary (state depends on token, only). Let LastToken */
			   /* point to next token to be returned. Returns: Token. Side */
			   /* effects: Sets yylval to token value on stack, modifies */
			   /* LastToken,  changes state variables. */

int LOOP_EvalWhile();      /* Semantic actions for current token WHILE.  Returns: See */
			   /* LOOP_GetNextTokenStack. Side effects: See */
			   /* LOOP_GetNextTokenStack. */

int LOOP_EvalCountLoop();  /* Semantic actions for current token FOR.  Returns: See */
			   /* LOOP_GetNextTokenStack. Side effects: See */
			   /* LOOP_GetNextTokenStack. */

int LOOP_EvalDo();         /* Semantic actions for current token DO.  Returns: See */
			   /* LOOP_GetNextTokenStack. Side effects: See */
			   /* LOOP_GetNextTokenStack. */

int LOOP_EvalThen();       /* Semantic  actions for current token THEN. Returns: See */
			   /* LOOP_GetNextToken. Side effects: See LOOP_GetNextToken. */
  
int LOOP_EvalEnd();        /* Semantic actions for current token END.  Returns: See */
			   /* LOOP_GetNextTokenStack. Side effects: See */
			   /* LOOP_GetNextTokenStack. */

void LOOP_StackToken();    /* Insert a new token at the end of the tokens list. For */
			   /* structure of list, see nessus programmers manual. Returns: */
			   /* void. Side effects: Modifies tokens list. */

void LOOP_InitLoopElem();  /* Initialize a  new LoopValueType record and append it to the */
			   /* TokenStackType record  (1st. argument). Returns: void. Side */
			   /* effects: Modifies 1st. argument. */

ParserStackType *LOOP_CopySimpleValue();
			   /* Copy a simple parser value stack record (argument). The */
			   /* argument may be NULL. Returns: Pointer to ParserStackType */
			   /* record or NULL. */

void LOOP_PushLoopStack(); /* Push a new TokenStackType record onto the stack of loop or */
			   /* condition begins. Returns: void. Side effects: Modifies */
			   /* NestedBackPtr and FreeBackPtr. */

TokenStackType *LOOP_PopLoopStack();  
                           /* Pop a TokenStackType record from the stack of loop or */
			   /* condition begins. Returns: TokenStackType record which has */
			   /* been on top of the stack. Side effects: Modifies */
			   /* NestedBackPtr and FreeBackPtr. */

int LOOP_GetNextToken();   /* Get next token - from either scanner or token stack. */
			   /* Returns: Token. */

void LOOP_InsertToken();   /* Set LastToken to list position after which the next token */
			   /* will be inserted. Returns: void. Side effects: Modifies */
			   /* LastToken and, eventually, TokenStackPtr. */

void LOOP_SetBackPtr();    /* Finish up processing of a parser state and leave it. Calld */
			   /* on tokens THEN, DO,... Returns: void. Side effects: Modifies */
			   /* its argument and LastToken. */

void LOOP_SetEndBackPtr(); /* Finish up processing of a parser state and leave it. Called */
                           /* on token END. Returns: void. Side effects: Modifies its */
			   /* argument and LastToken. */

void LOOP_AssignCondValue();
                           /* Assign result of a boolean or logical expression to */
			   /* ConditionValue. Returns: void. Side effects: Modifies */
			   /* ConditionValue. */

void LOOP_RemoveTokenStack();
                           /* Remove list of tokens. Returns: void. Side effects: Removes */
			   /* list of tokens  appended to its argument. */

void LOOP_InitForLoop();   /* Initialize loop variable of FOR loop. Then lock this */
			   /* variable, since  it may not be modified ybby aa statement in */
			   /* the loop. Returns: void. Side effects: Modifies LoopHeader */
			   /* appended to NestedBackPtr. */

void LOOP_CalcForIterations();
                           /* Calculate no. of iterations of  a FOR loop and insert it */
			   /* into the correspondent loop header. Returns: void. Side */
			   /* effects: Modifies loop header and ConditionValue. */

short LOOP_IncrementFor(); /* Increment no. of iterations in for header and update the */
			   /* variables value  entry. Then check  wether ActIter <= */
			   /* TotalIter and set ConditionValue. Returns: void. Side */
			   /* effects: Modifies loop header and ConditionValue. */

void LOOP_InsertLoopVar(); /* Inserts a loop variable and its loop values range size into */
			   /* the loop header. Previously to a call  to this function, Id */
			   /* and Val data  types must have  been checked by */
			   /* LOOP_CheckLoopVar. Returns: void. Side effects: Modofies */
			   /* loop header record. May  set AbortLoop = TRUE if there's an */
			   /* error. */

void LOOP_InitForeachLoop(); 
                           /* Initialize and lock  all loop variables of FOREACH loop. */
			   /* Returns: void. Side effects: Modifies LoopHeader appended to */
			   /* NestedBackPtr and symbol table  entries for loop variables. */

void LOOP_AssignValUnion();
                           /* Assign value to a variable. Returns: void. Side effects: */
			   /* variables symbol  table entry. */

void LOOP_AssignIncr();    /* Assign value to an array element. Returns: void. Side effects: */
			   /* Modifies its first parameter. */

short LOOP_IncrementForeach(); 
                           /* Increment values of foreach variables and set */
			   /* ConditionValue. Returns: void. Side effects: ConditionValue, */
			   /* symbol table and  ActValue entries for loop  variables. */

void LOOP_IncrementValUnion(); 
                           /* Increment actual value. Returns: void. Side effects: */
			   /* variables symbol  table entry, 2nd. parameter */

void LOOP_UnlockLoopVars(); 
                           /* Unlock loop variables of a for or foreach loop. Returns: */
			   /* void.  Side effects: Modifies loop variable symbol taable */
			   /* entries ( ->Lock). */

void LOOP_RemoveVarList(); /* Remove a list of FOREACH loop variables and unlocks the */
			   /* correspondent symbol table entries. Returns: void. */

TokenStackType *LOOP_GetTokenStack();
                           /* Allocate a new TokenStackType record, if necessary. Returns: */
			   /* & Initialized TokenStackType record. Side effects: Modifies */
			   /* FreeTokenStack. */

void LOOP_FreeTokenStack();/* Save an unused TokenStackType record on the stack */
			   /* FreeTokenStack points to. Returns: void. Side effects: */
			   /* Modifies FreeTokenStack. */

LoopHeaderType *LOOP_GetLoopHeader();
                           /* Allocate and initialize a new LoopHeaderType record. */
			   /* Returns: &LoopHeaderType. */

void LOOP_FreeLoopHeader();/* Free a LoopHeaderType record. Returns: void. Side effects: */
			   /* Removes argument. */

CondValueType *LOOP_GetCondValue();
                           /* Allocate and initialize a new CondHeaderType record. */
			   /* Returns: &CondHeaderType. */

void LOOP_FreeCondValue(); /* Free a CondHeaderType record. Returns: void. Side effects: */
			   /* Removes argument. */


LoopValueType *LOOP_GetLoopValue();
                           /* Allocate and initialize a new LoopValueType record. Returns: */
			   /* &LoopValueType. */

void LOOP_FreeLoopValue(); /* Free a LoopValueType record. Returns: void. Side effects: */
			   /* Removes argument. */

void LOOP_InitCondElem();

LoopVarStackType *LOOP_GetLoopVar();
                           /* Get a new LoopVarStackType record.  Initialize InitValue, */
			   /* set ActValue to NULL. Returns: & to LoopVarStackType. */
			   /* Side effects: Modifies FreeLoopVarStack. */

void LOOP_FreeLoopVar();   /* Push a LoopVarStackType record onto the stack of unused */
			   /* records. Free ActValue. Returns: void. Side effects: */
			   /* Modifies FreeLoopVarStack and <argument> -> ActValue. */




                /**** co_output.c - generate target representation of net ****/

void OUT_GenOutput();       /* Generate output file. Returns: void. */

void OUT_SetSNameLength(); /* Receives a ParserStackType record holding a sites name (or */
			   /* Undef).  If length of this new name is > SNameLength, set */
			   /* SNameLength to new length. Returns: void. Side effects: */
			   /* Modifies SNameLength. */


void OUT_SetUNameLength(); /* Receives a ParserStackType record holding a units name (or */
			   /* Undef).  If length of this new name is > UNameLength, set */
			   /* UNameLength to new length. Returns: void. Side effects: */
			   /* Modifies UNameLength. */

void OUT_SetActFuncLength();
                           /* Receives a ParserStackType record holding a units Actfunc */
			   /* (or Undef).  If length of this new ActFunc is > */
			   /* ActfuncLength, set ActfuncLength to new length. Returns: */
			   /* void. Side effects: Modifies ActfuncLength. */

void OUT_SetOutFuncLength();
                           /* Receives a ParserStackType record holding a units Outfunc */
			   /* (or Undef).  If length of this new OutFunc is > */
			   /* OutfuncLength, set OutfuncLength to new length. Returns: */
			   /* void. Side effects: Modifies OutfuncLength. */

void OUT_SetTypeLength();  /* Receives a ParserStackType record holding a units Type (or */
			   /* Undef).  If length of this new Type is > TypeLength, set */
			   /* TypeLength to new length. Returns: void. Side effects: */
			   /* Modifies TypeLength. */

void OUT_SetSitefuncLength();
                           /* Receives a ParserStackSitefunc record holding a units */
			   /* Sitefunc (or Undef).  If length of this new Sitefunc is > */
			   /* SitefuncLength, set SitefuncLength to new length. Returns: */
			   /* void. Side effects: Modifies SitefuncLength. */

void OUT_SetPositionLength();
                           /* Receives an integer position value.  If length of this new */
			   /* position  is > PositionLength,  set PositionLength to new */
			   /* length. Returns: void. Side effects: Modifies */
			   /* PositionLength. */

void OUT_UNoLength();      /* Calculates length of unit no. column. Returns: void. Side */
			   /* effects: Modifies UNoLength. */
void OUT_CalcSitesColumn();
                           /* Calculates position at which sites column starts. Returns: */
			   /* void. Side effects: Modifies SitesColumn. */

void OUT_CalcSourcesColumn();
                           /* Calculates position at which sources column starts. Returns: */
			   /* void. Side effects: Modifies SourcesColumn. */

void OUT_PrintHeader();    /* Print output file header. Returns: void. */

void OUT_AppendSource();   /* Append new source file to string of source files. Returns: */
			   /* void. */

void OUT_SetSource();      /* Append new source file to string of source files. Returns: */
			   /* void. */

void OUT_InsertNetName();  /* Insert network name into string. Returns: void. */

void OUT_PrintSiteSection();
                           /* Print sites section. Returns: void. */

void OUT_PrintTypeSection();
                           /* Print type section. Returns: void. */

void OUT_PrintUnitSection();
                           /* Print units section. Returns: void. */

void OUT_PrintNetUnits();  /* Print units section. Returns: void. */

void OUT_PrintStructUnits(); 
                           /* Print unit lines for a structure. Returns: void. */

void OUT_PrintSingleUnit();/* Print a single unit line. Returns: void. */

char*OUT_GetFType();       /* Get F type of a structure. Returns: String. */

char*OUT_GetName();        /* Get name of a unit. Returns: String. */

char OUT_GetSType();       /* Get s type of a unit. Returns: character, 'h' for HIDDEN, */
			   /* 'i' for INPUT, 'o' for OUTPUT, and 'g' for INPUT and OUTPUT */

char OUT_EvalSType();      /* Get s type of a unit. Returns: character, 'h' for HIDDEN, */
			   /* 'i' for INPUT, 'o' for OUTPUT, and 'g' for INPUT and OUTPUT */
			   /* Expects an argument which is one of these tokens. */

char*OUT_GetActFunc();     /* Get activation function name of a unit. Returns: String. */

char*OUT_GetOutFunc();     /* Get output function name of a unit. Returns: String. */

float OUT_GetAct();         /* Get Activation function of a unit. If the units activation */
			   /* has been defined through keyword RANDOM, generate a new */
			   /* random number in [-1.0. 1.0]. Returns: float. */

short OUT_GetFirstSite();
                           /* Get first site of unit,if there are sites return TRUE. Else, */
			   /* return FALSE. */

void OUT_GetPosition();    /* Calculates x and y position for a single unit. Returns: */
			   /* void.  Side effects: *XPos and *YPos will hold x and y */
			   /* positions. Special assumption: The structures topological */
			   /* dimension is large enaugh for S->Length ! PLANE is regarded */
			   /* as MATRIX(1,y) or MATRIX(x,1). */

void OUT_PrintConnectionSection(); 
                           /* Print connection section. Returns: void. */

void OUT_PrintStructTargets(); 
                           /* Print entries into connection section for target units. */
			   /* Returns: void. */ 

void OUT_PrintUnitConns(); /* Print entries into connection section for a target unit. */
			   /* Returns: void. */ 

short OUT_PrintCliqueImplicit(); 
                           /* Print entry into connection section for sources of implicit */
			   /* connections. Returns: int: SourPerLine - no. of sources */
			   /* printed into last line. */

short OUT_PrintChainImplicit(); 
                           /* Print entry into connection section for sources of implicit */
			   /* connections. Returns: int: SourPerLine - no. of sources */
			   /* printed into last line. */

short OUT_PrintRingImplicit(); 
                           /* Print entry into connection section for sources of implicit */
			   /* connections. Returns: int: SourPerLine - no. of sources */
			   /* printed into last line. */

short OUT_PrintStarImplicit(); 
                           /* Print entry into connection section for sources of implicit */
			   /* connections. Returns: int: SourPerLine - no. of sources */
			   /* printed into last line. */

void OUT_PrintExplicitConns(); 
                           /* Look for explicit connections to a given unit and print them */
			   /* into the output file. Returns: void. */




           /**** co_cFuncs.c - recompile simulator kernel and new functions ****/
void CF_IncludeNewFunctions();
                            /* fehlt auch noch */
