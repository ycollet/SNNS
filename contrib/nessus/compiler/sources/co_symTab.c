/*****************************************************************************
  FILE              : co_symTab.c
  SHORTNAME         : symTab
  VERSION           : 2.0

  PURPOSE           : contains functions for symbol table management:
                      - SYM_LookUp      	  - SYM_AllocLine     
		      - SYM_CopyLine    	  - SYM_Hash          
		      - SYM_GetNewPrime 	  - SYM_CheckTab      
		      - SYM_IsPrime     	  - SYM_RemoveTab     
		      - SYM_InitTab     	  - SYM_ExpandTab     
		      - SYM_InsertKeywords	  - SYM_UpdateLine 
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_symTab.h"


                  /**** global variables for this file ****/

                       /**** test variables ****/
#ifdef TEST
  static short SYM_Collisions = 0;  /* no. of collisions in symbol tables */
#endif
  static long SYM_AllocSpace = 0;  /* space occupied by symbol tables */


         /**** token strings and their correspondent token numbers ****/
static char *Keywords[NoOfKeywords] = {
#include "co_keywords.h"
};
static short KeywordTokenTypes[NoOfKeywords] = {
#include "co_keyTok.h"
};



/*****************************************************************************************
 Function:                     SYM_LookUp
 Purpose:  Search a key in the symbol table. If there's an entry for it, simply returns a
           pointer to the line of the symbol table. If the key is a new identifier, a new
	   line is allocated.
 Returns:  returns a pointer to symbol table line
 Side effects: modifies upper symbol table on stack
*****************************************************************************************/
SymTabLineType *SYM_LookUp(Key, Tab)
     char *Key;            /* string to be searched */
     SymTabType *Tab;      /* symbol table to be searched in */
{
  register SymTabLineType **Line;      /* points to a line of the symbol table */

  /* get adress of symbol table entry, hash function returns relative adress for key */
  Line = Tab->SymTab + SYM_Hash(Key, Tab->ActPrime);
  if(*Line)
      /* there is an entry at this position - is it the key itself or a collision */
      if(! strcmp(Key, (*Line)->Name))
	  /* there is an entry for the key - return it */
	  return *Line;
      else {
	/* there is a colision - position for key is occupied by some other entry */
	/* the symbol table is scanned until the key or the first unused entry is found */
	/* collision handling: store entry at first free position after original */
	Line = (Line < Tab->SymTab + (int)(Tab->ActLength-1))?
	  Line+1 : Tab->SymTab;
	/* symbol table is a ring structure - first entry is successor of last entry */
	for(;;)
	    if(!(*Line)) {
	      /* an empty line has been found - key is a new entry */
	      /* check density of actual symbol table */
	      if(SYM_CheckTab(Tab))
		/* symbol table has been expanded - apply hash function again */
		return SYM_LookUp(Key, Tab);
	      else {
	      /* old symbol table - allocate a new line and insert a copy of the key into it */
		(*Line = SYM_AllocLine())->Name = STR_Save(Key);
		Tab->ActEntries ++;
	      }
#ifdef SCANTEST
	      SYM_Collisions ++;
#endif
	      return *Line;
	    }
	    else if(! strcmp(Key, (*Line)->Name))
		/* entry has been found - it had been inserted after a collision */
		return *Line;
	    else
		/* continue scanning the entries */
		Line = (Line < Tab->SymTab + (int)(Tab->ActLength-1))?
		  Line+1 : Tab->SymTab;
      }
  else {
    /* there is no entry at the position - normal insertion */
    /* check density of actual symbol table */
    if(SYM_CheckTab(Tab))
      /* symbol table has been expanded - apply hash function again */
      return SYM_LookUp(Key, Tab);
    else {
      /* old symbol table - allocate a new line and insert a copy of the key into it */
      (*Line = SYM_AllocLine())->Name = STR_Save(Key);
      Tab->ActEntries ++;
    }
    return *Line;
  }
}




/*****************************************************************************************
 Function:                     SYM_CopyLine
 Purpose:  looks for the new position of an old symbol table line in the new expanded
           symbol table, procedure is the same as SYM_LookUp.
 Returns:  void
 Side effects: modifies new symbol table (SymTabPtr->SymTab)
 Special assumptions: Each line is copied exactly once. CopyLine does no strcmp to decide
                      wether there is a collision or not. CopyLine assumes that the argument
		      is not an entry of the new symbol table !
*****************************************************************************************/
void SYM_CopyLine(NewTab, OldLine)
     SymTabLineType **OldLine;         /* points to table entry which is copied */
     SymTabType *NewTab;          /* points to symbol table into which the entry is copied */
{
  register SymTabLineType **Line;      /* points to a line of NEW symbol table */

  /* get adress of symbol table entry, hash function returns new relative adress for it */
  Line = NewTab->SymTab + SYM_Hash((*OldLine)->Name, NewTab->ActPrime);
  if(*Line) {
      /* there is an entry at this position - collision, since entry is new */
      /* the symbol table is scanned until the first unused entry is found */
      /* collision handling: store entry at first free position after original */
      for(; *Line; Line = (Line < NewTab->SymTab + (int)((NewTab->ActLength)-1))? Line+1 : NewTab->SymTab)
	  ;
#ifdef SCANTEST
      SYM_Collisions ++;
#endif
      /* free position found - copy old line into new position */
    }
  *Line = *OldLine;
}




/*****************************************************************************************
 Function:                     SYM_GetNewPrime
 Purpose:  calculates the greatest prime < Sup
 Returns:  short
*****************************************************************************************/
short SYM_GetNewPrime(Sup)
     int Sup;           /* prime returned must be < Sup */
{
  register int ActPrime;
  for(ActPrime=Sup; ! SYM_IsPrime(ActPrime); ActPrime --)
      ;
  return ActPrime;
}




/*****************************************************************************************
 Function:                     SYM_IsPrime
 Purpose:  checks if the argument is a prime
 Returns:  TRUE if arg. is prime, else FALSE
*****************************************************************************************/
short SYM_IsPrime(Prime)
     short Prime;           
{
  register short Div;
  for(Div = Prime-1; Div > 1; Div --)
      if( !(Prime % Div))
	  return FALSE;
  return TRUE;
}




/*****************************************************************************************
 Function:                     SYM_InitTab
 Purpose:  Allocates and initializes a new symbol table.
 Returns:  void
 Side effects: Modifies SymTabPtr and the new table.
*****************************************************************************************/
void SYM_InitTab()
{
  SymTabType *NewTab;

  /* allocate new record to hold symbol table */
  NewTab = (SymTabType*) (M_alloc(sizeof(SymTabType)));
  /* new table becomes upper element on stack */
  NewTab->NextSymTab = SymTabPtr;
  /* allocate new symbol table - initial size is InitSymTabLength */
  NewTab->SymTab = SYM_AllocSymTab((int) InitSymTabLength);
  SYM_AllocSpace += InitSymTabLength;    /* for statistics, only */
  /* initialize surrounding information for new symbol table */
  NewTab->ActLength = InitSymTabLength;
  NewTab->ActPrime = SYM_GetNewPrime((int) InitSymTabLength);
  NewTab->Insertions = 0;
  NewTab->ActEntries = 0;
  NewTab->MinUnitNo = UnitCtr+1;
  SymTabPtr = NewTab;
  /* insert keywords into new symbol table */
  SYM_InsertKeywords(SymTabPtr); 
#ifdef SCANTEST
  (void) printf("\nnew symbol table after keyword insertion:  (collisions %d) \n", SYM_Collisions);
  SYM_PrintTab(NewTab->SymTab, (int) SYM_ActTabLength);
#endif
}





/*****************************************************************************************
 Function:                     SYM_InsertKeywords
 Purpose:  inserts the keywords of nessus into the new symbol table
 Returns:  void
 Side effects: modifies some entries of the new  table
*****************************************************************************************/
void SYM_InsertKeywords(Tab)
     SymTabType *Tab;             /* points to the new symbol table */
{
  register SymTabLineType **ScanTab;   /* to scan the new table, look for free position */
  register short Kwd;                  /* index of actual keyword to be inserted */
  register SymTabLineType *Pos;

  /* initialize new symbol table (NULL pointers - no entries) */
  for(ScanTab = Tab->SymTab; ScanTab<(Tab->SymTab)+InitSymTabLength; ScanTab++)
    *ScanTab = NULL;
  /* insert all entries of the old table into the new - use hash function */
  for(Kwd = 0, Pos = NULL; Kwd < NoOfKeywords; Kwd++) {
    /* Keywords is an array of keywords, KeywordTokenTypes holds the corresponding */
    /* token values, both arrays have length NoOfKeywords */
    /* LookUp allocates new line and inserts the keyword string into its name field */
    Pos = SYM_LookUp(Keywords[Kwd], Tab);
    /* insert keyword type - same field as for variable data types */
    Pos->Type = KeywordTokenTypes[Kwd];
    Pos->UndefType = FALSE;
    Pos->UndefValue  = TRUE;
    Pos->isKeyword = TRUE;
;
  }
}





/*****************************************************************************************
 Function:                     SYM_AllocLine
 Purpose:  allocates a new symbol table line and initializes its fields
 Returns:  pointer to new symbol table line
*****************************************************************************************/
SymTabLineType *SYM_AllocLine()
{
  register SymTabLineType *TabLine;

  TabLine = (SymTabLineType*) (M_alloc(sizeof(SymTabLineType)));
  TabLine->Type = Undef;
  /* allocate value record -- it is NOT INITIALIZED !! */

  TabLine->Value = (SymbolValueType*) M_alloc(sizeof(SymbolValueType));
  TabLine->UndefType = TRUE;
  TabLine->UndefValue = TRUE;
  TabLine->isKeyword = FALSE;
  return TabLine;
}




/*****************************************************************************************
 Function:                     SYM_Hash
 Purpose:  Hash function for symbol table management: P.J. Weinberger's 'hashpjw'.
           Algorithm in Aho, Sethi, Ullman: Compilers Principles, Techniques, and Tools.
	   Addison-Wesley, 1986. pg. 435 - 437.
 Returns:  Index corresponding to key.
*****************************************************************************************/
int SYM_Hash(Key, Prime)
     char *Key;        /* key for which the index is calculated */
     int Prime;
{
  register char *p;
  register unsigned h=0, g;
  for(p=Key; *p != '\0'; p++) {
    h = (h << 4) + (*p);
    if(g = h&0xf0000000) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h % Prime;
}


/*****************************************************************************************
 Function:                     SYM_CheckTab
 Purpose:  Checks if a symbol table must be expanded. It is expanded if its density
           (Entries / Length) is greater than ExpandDensity.
	   The Tabs actual density is calculated after each SymTabChecksDist insertions,
	   rather than at each call of LookUp.
 Returns:  Returns TRUE if symbol table has been expanded, else FALSE.
 Side effects:  modifies symbol table length SYM_ActTabLength, actual divisor for hash
                function SYM_ActPrime, counter of insertions since last density check
		SYM_Insertions, entries counter SYM_ActEntries, and the symbol table itself.
*****************************************************************************************/
short SYM_CheckTab(Tab)
     SymTabType *Tab;                       /* actual symbol table */
{
  if(Tab->Insertions == SymTabCheckDist) {
    /* sufficient insertions since last density check - calculate new density */
    Tab->Insertions = 0;                     /* reset counter of insertions */
    if(Tab->ActEntries > Tab->ActLength * ExpandDensity) {
      /* expand symbol table - it is increased by SymTabAdd lines */
      Tab->ActPrime = SYM_GetNewPrime((int) (Tab->ActLength += SymTabAdd));
      SYM_AllocSpace += SymTabAdd;          /* increase counter of allocated symTab space */
#ifdef SCANTEST
      (void) printf("old symbol table, density %f: , collisions %d \n\n",
                    Tab->ActEntries/(Tab->ActLength-SymTabAdd), SYM_Collisions);
      SYM_PrintTab(Tab, (int) (Tab->ActLength-SymTabAdd));
      SYM_Collisions = 0;                   /* collisions counted for each table size */
#endif
      /* build new symbol table, entries of old table are copied */
      Tab->SymTab = SYM_ExpandTab(Tab, Tab->SymTab, (int) (Tab->ActLength-SymTabAdd), (int) Tab->ActLength);
#ifdef SCANTEST
      (void) printf("\n\nnew symbol table:  density %f: , collisions %d \n\n",
                    Tab->ActEntries/(Tab->ActLength), SYM_Collisions); 
      SYM_PrintTab(Tab->SymTab, (int) Tab->SymTab->ActLength);
#endif
      return TRUE;
    }
  }
  else
    /* no density check needed yet - increase insertions counter */
    Tab->Insertions ++;
  return FALSE;
}




/*****************************************************************************************
 Function:                     SYM_RemoveTab
 Purpose:  Removes a symbol table corresponding to an INNER net (subnet) from the stack of
           symbol tables. Resets environment to outer symbol table, saves environment of
	   inner table to its SymTabType record.
	   Does nothing if there's only one symbol table on the tables stack.
 Returns:  TRUE if symbol table has been part of a subnet, else FALSE.
 Side effects: SYM_ActPrime, SYM_ActTabLength, SYM_Insertions, and SYM_ActEntries are
               reset tyo the values of the outer symbol tables.
	       SymTabPtr will point to outer symbol table.
	       Inner symbol table record is inserted into outer symbol table (subnet
	       entry).
*****************************************************************************************/
short SYM_RemoveTab()
{
  register SymTabType *InnerTab;     /* ptr. to old top of symbol table stack */
  register SymTabLineType *Line;     /* ptr. to line of subnet entry in outer table */

  if(SymTabPtr->NextSymTab) {
    /* there is an outer symbol table - remove the inner one */
    SymTabPtr = (InnerTab = SymTabPtr)->NextSymTab;
    /* the name of the subnet, to which the inner symbol table corresponds is an entry of */
    /* the outer symbol table. It will hold complete informations about the subnet read */
    /* look for subnet entry in the outer symbol table */
    Line = SYM_LookUp(InnerTab->Network, SymTabPtr);
    /* insert the subnets symbol table into value field */
    Line->Value->subnet->SymTab = InnerTab->SymTab;
    /* connections are global - not modified */
    return TRUE;
  }
  /* the outer symbol table, that corresponding to the whole net, is not removed */
  return FALSE;
}





/*****************************************************************************************
 Function:                     SYM_ExpandTab
 Purpose:  expands symbol table and copies all entries of old into new table.
 Returns:  &&SymTabLineType, pointer to new symbol table
*****************************************************************************************/
SymTabLineType **SYM_ExpandTab(Tab, OldTab, OldLength, NewLength)
     SymTabType      *Tab;           /* points to symbol table information record */
     SymTabLineType **OldTab;        /* pointer to old symbol table */
     int              OldLength;     /* length of old table */
     int              NewLength;     /* length of new table */
{
  register SymTabLineType **Scan;    /* points to a symbol table line pointer - scans tables */
  register SymTabLineType **NewTab;  /* points to new symbol table*/

  /* allocate new record to hold symbol table */
  NewTab = (SymTabLineType**) (M_alloc((unsigned) (NewLength * sizeof(SymTabLineType*))));
  /* initialize new symbol table entries */
  for(Scan = NewTab; Scan < NewTab + NewLength; Scan ++)
    *Scan = NULL;
  Tab->SymTab = NewTab; /* record must hold new symbol table for SYM_CopyLine */
  /* copy all entries of OldTab into NewTab - use hash function, adresses may change */
  for(Scan = OldTab; Scan < OldTab + OldLength; Scan ++)
    if(*Scan)
      /* there is an entry in the old table - copy it into new table - use same record */
      SYM_CopyLine(Tab, Scan);
  return NewTab;
}





/*****************************************************************************************
 Function:                     SYM_AllocSymTab
 Purpose:  Allocates a new symbol table of specific length and returns a
           pointer to it. The table is not initialized !
 Returns:  pointer to new symbol table.
*****************************************************************************************/
SymTabLineType **SYM_AllocSymTab(Length)
     int Length;                    /* length of new symbol table */
{
  register SymTabLineType **Scan;     /* scan new symbol table */
  register SymTabLineType **NewTab;   /* point to new symbol table */
  
  /* allocate new symbol table */
  NewTab = (SymTabLineType **) (M_alloc((unsigned) (Length * sizeof(SymTabLineType *))));
  /* initialize new table with NULL pointers */
  for(Scan = NewTab; Scan < NewTab + Length; Scan ++)
    *Scan = NULL;
  return NewTab;
}





/*****************************************************************************************
 Function:                     SYM_UpdateLine
 Purpose: Update a symbol table entry (assign type and value to corresponding fields).
          Perform type checking (redefinition of variable data types).
 Returns: void
 Side effects: Modifies symbol table entry. 
*****************************************************************************************/
void SYM_UpdateLine(Line, Type, Value)
     register SymTabLineType  *Line;    /* pointer to symbol table entry */
     register SymbolType       Type;    /* variable data type */
     register SymbolValueType *Value;   /* new value for variable */
{
  if(! Line->UndefType) 
    /* variable has been defined already - check if there is a redefinition */
    if(Line->Type != Type) 
      ER_StackError("redefinition of variable %s\n",
		    Warning, Line->Name, nst, nst);
  /* if there has been an error - forget old variable definition */
  Line->UndefType = FALSE;
  Line->Type = Type;
  Line->Value = Value;
}





/*****************************************************************************************
 Function:                     SYM_InsertNetName
 Purpose: Inserts network or subnet name into upper SumTabType record on stack of symbol
          tables.
 Returns: void
 Side effects: Modifies SymTabPtr->Network. 
*****************************************************************************************/
void SYM_InsertNetName(Name)
     ParserStackType *Name;
{
  SymTabPtr->Network = Name->yvalue->var->Name;
  Name->yvalue->var->UndefType = FALSE;
  Name->yvalue->var->Type = NETWORK;
  Name->yvalue->var->isKeyword = TRUE;
}




#ifdef TEST

/*****************************************************************************************
 Function:                     SYM_PrintTab
 Purpose: Print symbol table.
 Returns: void
*****************************************************************************************/
void SYM_PrintTab(Tab, Length)
     SymTabLineType **Tab;
     int Length;
{
  register SymTabLineType **Line;
  register int no;
  
  for(Line = Tab, no=0; Line < Tab + Length; Line ++, no ++)
    if(*Line)
      (void) printf("%4d %10s %10s\n", no, STR_Save((*Line)->Name),
		    ((*Line)->Type)? STR_TypeString((*Line)->Type): STR_Save("--"));
    else
      (void) printf("--------------------------\n");
}




/*****************************************************************************************
 Function:                     SYM_PrintEndTab
 Purpose: Print symbol table, information about collisions, no. of entries, etc.
 Returns: void
*****************************************************************************************/
void SYM_PrintEndTab()
{
  (void) printf("\n %d symbol table entries with %.2f %% of collisions, density %.2f\n\n",
		(int) SymTabPtr->ActEntries, (float) SYM_Collisions/SymTabPtr->ActEntries*100,
		(float) SymTabPtr->ActEntries/SymTabPtr->ActLength);
  SYM_PrintTab(SymTabPtr->SymTab, (int) SymTabPtr->ActLength);
}

#endif




