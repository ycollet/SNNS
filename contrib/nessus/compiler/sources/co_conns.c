/*****************************************************************************
  FILE              : co_conns.c
  SHORTNAME         : conns
  VERSION           : 2.0

  PURPOSE           : contains functions to generate connections:
                                - CONN_InitConnectionList
                                - CONN_InitTargetSite
                                - CONN_AllocSourceList
                                - CONN_AllocTargetList
                                - CONN_InsertSource
                                - CONN_InsertConnection
				- CONN_Hash
				- CONN_LookUpTarget
				- CONN_LookUpSource
				- CONN_CheckTargetDensity
				- CONN_CheckSourceDensity
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_conns.h"


/**** pointer to list of explicit connections ****/
ConnectionType *CONN_Connections = NULL;



/*****************************************************************************************
 Function:                     CONN_InsertConnection
 Purpose: Inserts a connection into the list. Receives target and source unit numbers and 
          the connection weight.
 Returns: void
 Side effects: Modifies CONN_Connections.
*****************************************************************************************/
void CONN_InsertConnection(Target, Site, Source, Weight)
     int Target;                                                  /* no. of target unit */
     int Source;                                                  /* no. of source unit */
     SiteListType *Site;                   /* pointer to target site entry in site list */
     float Weight;      /* float, cannot be RANDOM, weight of connection in [-1.0, 1.0] */
{
  short NewTarget = FALSE;   /* is set if a new entry into the list of targets is built */
  TargetUnitType *TPos;        /* position of unit with no. Target in target unit table */
  TargetSiteType **ActSite;            /* to scan list of sites for a determined target */
  
  if(Target == Source)       /* warning if there is a connection from a  unit to itself */ 
    ER_StackError("connections: unit  connected to itself\n",  Warning,  nst, nst, nst);
  if( ! CONN_Connections)               /* there has been no explicit connection before */
    CONN_Connections = CONN_InitConnectionList();                   /* initialize table */
  TPos = CONN_LookUpTarget(CONN_Connections, Target);      /* delivers pointer to entry */
  if( ! TPos->TNo) {                     /* there has been no connection to this target */
    TPos->TNo = Target;
    CONN_Connections->Entries += 1.0;
    NewTarget = TRUE;                             /* new target unit inserted into list */
  }
  /* look for site - works for units without sites: Sites == NULL and there's only */
  /* TPos->Site set with TPos->Site == NULL. Correctness has been checked before */
  for(ActSite = &(TPos->Site); *ActSite; ActSite = & ((*ActSite)->tNext))
    if((*ActSite)->tSite == Site)
      break;                           /* look up target site in UNSORTED list of sites */
  if(! (*ActSite)) {                 /* there have been no connections to this site before */
    (*ActSite) = CONN_InitTargetSite();      /* allocate table of sources attached to site */
    (*ActSite)->tNext = TPos->Site;
    TPos->Site = (*ActSite);            /* new site becomes first element in list (stack!) */
    (*ActSite)->tSite = Site;
  }
  CONN_InsertSource(ActSite, Source, Weight);            /* ActSite may be modified !! */
  if(NewTarget)
    CONN_CheckTargetDensity(CONN_Connections);
}






/*****************************************************************************************
 Function:                     CONN_InsertSource
 Purpose: Inserts a connections source unit and and weight into the table of source units
          attached to a determined site.
 Returns: void.
 Side effects: Table of source units.
*****************************************************************************************/
void CONN_InsertSource(Site, Source, Weight)
     TargetSiteType **Site;      /* pointer to site stack element for which source table */
     int Source;                                                  /* no. of source unit */
     float Weight;                                 /* connection weight, in [-1.0, 1.0] */
{
  SourceEntryType *SPos;       /* position of unit with no. Source in source unit table */
  SPos = CONN_LookUpSource(*Site, Source);                 /* get entry in source table */
  SPos->Weight = Weight;                                    /* insert connection weight */
  if(SPos->SNo)                              /* this connection has been defined before */
    if(SPos->SNo == Source) {
      if( ! Structures)               /* explicit redefinition of a connection => error */
	ER_StackError("connection: redefinition of connection\n", Warning, nst, nst, nst);
    }            /* redefinitions in IRREGULAR structures are compiler made and ignored */
    else {
      (void) printf("******bug: source list overflow\n");
      exit(108);
    }
  else {
    SPos->SNo = Source;      /* insert new source unit into table of connection sources */
    (*Site)->tInput.Entries += 1.0;
    ConnectionCtr ++;                    /* count connections for header of output file */
    *Site = CONN_CheckSourceDensity(*Site);                             /* expand table ? */
  }
}





/*****************************************************************************************
 Function:                     CONN_AllocTargetList
 Purpose: Allocates table of connection targets (TargetUnitType) and initializes it.
 Returns: Pointer to new table.
*****************************************************************************************/
TargetUnitType *CONN_AllocTargetList(Size)
     float Size;                                      /* size of new target units table */
{
  register TargetUnitType *Res;                    /* pointer to new target units table */
  register TargetUnitType *Scan;              /* pointer to scan new target units table */

  Res = (TargetUnitType *) M_alloc((unsigned)(sizeof(TargetUnitType) * (int) Size));
  for(Scan = Res; Scan < Res + (int) Size; Scan ++)  {
    Scan->Site = NULL;
    Scan->TNo = 0;                      /* initial value, there's no unit with no. 0 !! */
  }
  return Res;
}





/*****************************************************************************************
 Function:                     CONN_AllocSourceList
 Purpose: Allocates table of connection sources (SourceEntryType) and initializes it.
 Returns: Pointer to new table.
*****************************************************************************************/
SourceEntryType *CONN_AllocSourceList(Size)
     float Size;                                      /* size of new source units table */
{
  register SourceEntryType *Res;                    /* pointer to new source units table */
  register SourceEntryType *Scan;              /* pointer to scan new source units table */

  Res = (SourceEntryType *) M_alloc((unsigned)(sizeof(SourceEntryType) * (int) Size));
  for(Scan = Res; Scan < Res + (int) Size; Scan ++)  
    Scan->SNo = 0;                      /* initial value, there's no unit with no. 0 !! */
  
  return Res;
}





/*****************************************************************************************
 Function:                     CONN_InitTargetSite
 Purpose: Allocates new site record for a target unit entry in table of connection targets
          and initializes it 
 Returns: Pointer to new entry (TargetSiteType).
*****************************************************************************************/
TargetSiteType *CONN_InitTargetSite()
{
  register TargetSiteType *Res;             /* pointer to new site for target unit entry */

  Res = (TargetSiteType *) M_alloc(sizeof(TargetSiteType));
  Res->tInput.Sources = CONN_AllocSourceList(InitSourceNo);    /* allocate source table */
  Res->tNext = NULL;
  Res->tSite = NULL;
  Res->tInput.Size = InitSourceNo;                      /* initial size of source table */
  Res->tInput.Prime = SYM_GetNewPrime((int) InitSourceNo);
  Res->tInput.Entries = 0.0;
  return Res;
}





/*****************************************************************************************
 Function:                     CONN_InitConnectionList
 Purpose: Initialize list of connections (i.e. list of connection targets).
 Returns: Pointer to new list (ConnectionType).
*****************************************************************************************/
ConnectionType *CONN_InitConnectionList()
{
  register ConnectionType *Res;                  /* pointer to new table of connections */
  
  Res = (ConnectionType *) M_alloc(sizeof(ConnectionType));
  Res->Size = InitTargetNo;
  Res->Entries = 0.0;    
  Res->Prime = SYM_GetNewPrime((int) InitTargetNo);
  Res->Targets = CONN_AllocTargetList(InitTargetNo);
  return Res;
}






/*****************************************************************************************
 Function:                     CONN_Hash
 Purpose: Hash function, similar to SYM_Hash.
 Returns: integer
*****************************************************************************************/
int CONN_Hash(UnitNo, Prime)
   register int UnitNo;     /* no. of unit */
   register int Prime;      /* for modulo operation */
{
  register unsigned h=0,g;
  register int act;

  for(act = UnitNo % 10; UnitNo > 0; UnitNo  /= 10, act = UnitNo % 10) {
    h = (h << 4) + act;
    if(g = h & 0xf0000000) {
      h = h ^ (g >> 24);
      h = h ^ g;
    }
  }
  return h % Prime;
}






/*****************************************************************************************
 Function:                     CONN_LookUpTarget
 Purpose: Look for an entry in target units table.
 Returns: pointer to entry (TargetUnitType).
*****************************************************************************************/
TargetUnitType *CONN_LookUpTarget(Table, UnitNo)
     ConnectionType *Table;                            /* pointer to target units table */
     int UnitNo;                         /* no. of unit which will be searched in table */
{
  register TargetUnitType *Target;          /* points to an entry of target units table */
  int Index;                        /* initial target index calculated by hash function */

  Index = CONN_Hash(UnitNo, Table->Prime);           /* get index and resolve collision */
  /* look for entry, entry is new, if there is found an entry with TNo == 0 before the */
  /* entry with TNo == UnitNo. */
  for(Target = Table->Targets + Index; Target->TNo && Target->TNo != UnitNo;)
    if(Target >= Table->Targets + (int) Table->Size)            /* end of table reached */
      Target = Table->Targets;                        /* reset Target to begin of table */
    else
      Target ++;                                                    /* increment Target */
  return Target;
}






/*****************************************************************************************
 Function:                     CONN_LookUpSource
 Purpose: Look for an entry in source units table.
 Returns: pointer to entry (SourceEntryType).
*****************************************************************************************/
SourceEntryType *CONN_LookUpSource(Site, UnitNo)
     TargetSiteType *Site;                             /* pointer to source units table */
     int UnitNo;                         /* no. of unit which will be searched in table */
{
  register SourceEntryType *Source;          /* points to an entry of source units table */
  int Index;                        /* initial source index calculated by hash function */

  Index = CONN_Hash(UnitNo, Site->tInput.Prime);            /* get index and resolve collision */
  /* look for entry, entry is new, if there  is found an entry with SNo == 0 before the */
  /* entry with SNo == UnitNo. */
  for(Source = Site->tInput.Sources + Index; Source->SNo && Source->SNo != UnitNo;)
    if(Source >= Site->tInput.Sources + (int) Site->tInput.Size)        /* end of table */
      Source = Site->tInput.Sources;                  /* reset Source to begin of table */
    else
      Source ++;                                                    /* increment Source */
  return Source;
}






/*****************************************************************************************
 Function:                     CONN_CheckTargetDensity
 Purpose: Checks wether a table of target units is full or not. Inecessary, expands the
          table and inserts entries into new table.
 Returns: Pointer to new table.
 Side effects: Old table is removed, modifies CONN_Connections.
*****************************************************************************************/
void CONN_CheckTargetDensity(OldTable)
     ConnectionType *OldTable;
{
  ConnectionType *NewTable;                              /* pointer to new target table */
  register TargetUnitType *Scan; /* pointer to scan old table and copy entries into new */
  register TargetUnitType *NewPos;               /* pointer to new position of an entry */
  if(OldTable->Entries < (int) OldTable->Size * MaxTargetDensity)
    return;                                            /* not necessary to expand table */
  else {                        /* expand new table, allocate new ConnectionType record */
    NewTable = (ConnectionType *) M_alloc(sizeof(ConnectionType));
    NewTable->Size = OldTable->Size + ExpandTarget;                /* new size of table */
    NewTable->Prime = SYM_GetNewPrime((int) NewTable->Size);        /* new mod value */
    NewTable->Entries = OldTable->Entries;
    NewTable->Targets = (TargetUnitType *) M_alloc((unsigned) (sizeof(TargetUnitType) * 
							       (int) NewTable->Size));
    for(Scan = NewTable->Targets; Scan < NewTable->Targets + (int) NewTable->Size;
	Scan++) {                                               /* initialize new table */
      Scan->TNo = 0;
      Scan->Site = NULL;
    }
    for(Scan = OldTable->Targets; Scan < OldTable->Targets + (int) OldTable->Size;
	Scan++)                            /* copy entries of old table into  new table */
      if(Scan->TNo) {                                       /* valid entry in old table */
	NewPos = CONN_LookUpTarget(NewTable, Scan->TNo);            /* get new position */
	NewPos->TNo = Scan->TNo;                    /* copy entry into new target table */
	NewPos->Site = Scan->Site;
      }                                 /* NewTable->Entries must not be incremented !! */
    (void) M_free( (char*) OldTable->Targets);                        /* remove old table */
    CONN_Connections = NewTable;
    return;
  }
}






/*****************************************************************************************
 Function:                     CONN_CheckSourceDensity
 Purpose: Checks wether a table of source units is full or not. Inecessary, expands the
          table and inserts entries into new table.
 Returns: Pointer to new table.
 Side effects: Old table is removed, modifies *OldSite.
*****************************************************************************************/
TargetSiteType *CONN_CheckSourceDensity(OldSite)
     TargetSiteType *OldSite;
{
  TargetSiteType *NewSite;                               /* pointer to new source table */
  register SourceEntryType *Scan;/* pointer to scan old table and copy entries into new */
  register SourceEntryType *NewPos;              /* pointer to new position of an entry */
  if(OldSite->tInput.Entries < (int) OldSite->tInput.Size * MaxSourceDensity)
    return OldSite;                                    /* not necessary to expand table */
  else {                        /* expand new table, allocate new TargetSiteType record */
    NewSite = (TargetSiteType *) M_alloc(sizeof(TargetSiteType));
    NewSite->tInput.Size = OldSite->tInput.Size + ExpandSource;       /* new size */
    NewSite->tInput.Prime = SYM_GetNewPrime((int) NewSite->tInput.Size);  /* new mod */
    NewSite->tInput.Entries = OldSite->tInput.Entries;  /* is incremented by CONN_LookUpSource */
    NewSite->tNext = OldSite->tNext;
    OldSite->tNext = NULL;
    NewSite->tSite = OldSite->tSite;
    NewSite->tInput.Sources = (SourceEntryType *) 
      M_alloc((unsigned) (sizeof(SourceEntryType) * (int) NewSite->tInput.Size));
    for(Scan = NewSite->tInput.Sources; Scan < NewSite->tInput.Sources + (int)
	NewSite->tInput.Size; 
	Scan++)                                                 /* initialize new table */
      Scan->SNo = 0;
    for(Scan = OldSite->tInput.Sources; Scan < OldSite->tInput.Sources + (int)
	OldSite->tInput.Size; Scan++)   /* copy entries of old table into  new table */
      if(Scan->SNo) {                                       /* valid entry in old table */
	NewPos = CONN_LookUpSource(NewSite, Scan->SNo);             /* get new position */
	NewPos->SNo = Scan->SNo;                    /* copy entry into new source table */
	NewPos->Weight = Scan->Weight;
      }                           /* NewSite->tInput.Entries must not be incremented !! */
/****    (void) M_free( (char*) OldSite->tInput.Sources);           ****/       /* remove old table */
/****    (void) M_free( (char*) OldSite);                           ****/ 
    return NewSite;
  }
}






/*****************************************************************************************
 Function:                     CONN_GetNextConnSource
 Purpose: Look for list of explicit connections to a given unit. If there are connections,
          return pointer to next connection of list, else return NULL.
 Returns: & SourceEntryType (next connection) or NULL.
*****************************************************************************************/
SourceEntryType *CONN_GetNextConnSource(TNo, Site)
     register int TNo;                                            /* no. of target unit */
     register SiteListType *Site;          /* pointer to target site entry in site list */
{
  static SourceEntryType *ActSource = NULL;    /* points to last source entry delivered */
  static SourceEntryType *SourceEnd;                  /* points to end of sources table */
  static TargetSiteType *ActSite;                    /* points to site entry for target */
  register TargetUnitType *TPos; /* position of unit with no. TNo. in target unit table */
  register SourceEntryType *Result;
  
  if(ActSource == NULL)  {           /*  first call to function -- search sources table */
    if( ! CONN_Connections)             /* there has been no explicit connection before */
      return NULL;                       /* there are no explicitly defined connections */
    TPos = CONN_LookUpTarget(CONN_Connections, TNo);       /* delivers pointer to entry */
    if( ! TPos->TNo)
      return (ActSource = NULL);
    else {
      /* look for site - works for units without sites: Sites == NULL and there's only */
      /* TPos->Site  set with TPos->Site == NULL. Correctness  has been checked before */
      for(ActSite = TPos->Site; ActSite; ActSite = ActSite->tNext) 
	if(ActSite->tSite == Site)
	  break;                       /* look up target site in UNSORTED list of sites */
      if(! ActSite)                            /* there are no connections to this site */
	return (ActSource = NULL);
      else  {                                          /* site is target of connections */
	ActSource = ActSite->tInput.Sources;      /* first source entry (may be empty!) */
	SourceEnd = ActSource + (int) (ActSite->tInput.Size );     /* mark end of table */
      }   
      for(; ActSource < SourceEnd && ! ActSource->SNo; ActSource ++); /* look for entry */
      if(ActSource >= SourceEnd)    /* end of sources - reset ActSource and return NULL */
	ActSource = NULL;
      Result = ActSource;
    }
  }
  else if(ActSite->tSite == Site) { /* subsequent call to function -look for next entry */
    for(ActSource ++; ActSource < SourceEnd && ! ActSource->SNo; ActSource ++) ;      
    if(ActSource >= SourceEnd)      /* end of sources - reset ActSource and return NULL */
      ActSource = NULL;
    Result = ActSource;
  }          
  else  {                               /* did not get all sources for last target site */
    Result = (ActSource = NULL);
    (void) printf("******bug: illegal call to CONN_GetNextConnSource for unit %d\n", TNo);
  }
  return Result;
}





/*****************************************************************************************
 Function:                     CONN_CopySubnetConnections
 Purpose: Copy inner connections of a subnet. Receives upper and lower unit index. All 
          connections whose target unit no. lies between upper and lower index are copied.
          "Copied" means that for a connection I_s -> I_t, I_s, I_t integer unit no.s, 
	  a new connection I_s' -> I_t' is generated, with 
	  I_s' = I_s + Max - Min + 1 and I_t' = I_t + Max - Min + 1. 
	  Note that for all subnets, if the target unit index lies in [Min, Max], the same 
	  holds for the corresponding source unit index.
 Returns: void.
 Side effects: Modifies some connection table entries. Increments ConnectionCtr.
 NOTE: NOT IMPLEMENTED.
*****************************************************************************************/
void CONN_CopySubnetConnections(Min, Max)
     int Min, Max;
{
  (void) printf("******bug: copies of subnet connections not yet implemented\n");
}
