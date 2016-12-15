/*****************************************************************************
  FILE              : co_output.c
  SHORTNAME         : output
  VERSION           : 2.1

  PURPOSE           : contains functions for target code generation:
                       - OUT_GenOutput              - OUT_PrintHeader
		       - OUT_SetSNameLength         - OUT_AppendSource
		       - OUT_SetUNameLength         - OUT_SetSource
		       - OUT_SetActFuncLength       - OUT_InsertNetName
		       - OUT_SetOutFuncLength       - OUT_PrintSiteSection
		       - OUT_SetTypeLength          - OUT_PrintTypeSection
		       - OUT_SetSiteFuncLength      - OUT_PrintUnitSection 
		       - OUT_SetPositionLength      - OUT_UnitSep
		       - OUT_UNoLength              - OUT_PrintNetUnits
		       - OUT_CalcSitesColumn        - OUT_PrintStructUnits
		       - OUT_CalcSourcesColumn      - OUT_PrintSingleUnit
		       - OUT_GetFType               - OUT_ConnSep
		       - OUT_GetName                - OUT_PrintConnectionSection
		       - OUT_GetSType               - OUT_PrintStructTargets
		       - OUT_EvalSType              - OUT_PrintUnitConns
		       - OUT_GetActFunc             - OUT_PrintImplicitConns
		       - OUT_GetOutFunc             - OUT_PrintCliqueImplicit
		       - OUT_GetAct                 - OUT_PrintChainImplicit
		       - OUT_GetFirstSite           - OUT_PrintRingImplicit
		       - OUT_GetPosition            - OUT_PrintStarImplicit
		       - OUT_PrintExplicitConns
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : Alex Hummler
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_output.h"

/* initial values of column widths are lengths of column headers */

static short SNameLength = 9;                /* max. width of site names in output file */
static short SitefuncLength = 13;   /* max. width of site function names in output file */
static short TypeLength = 8;                 /* max. width of type names in output file */
static short ActfuncLength = 8;    /* max. width of activation functions in output file */
static short OutfuncLength = 8;        /* max. width of output functions in output file */
static short UNoLength = 6;                   /* max. width of unit no.s in output file */
static short UNameLength = 8;                 /* max. width of unit no.s in output file */
static short PositionLength = 4;         /* max. width of unit positions in output file */
static short UnitSitesColumn = 1;   /* position at which sites colmumn starts for types */
static short TypeSitesColumn = 1;   /* position at which sites colmumn starts for units */
static short SourcesColumn = 1;           /* position at which "sources" colmumn starts */


static char *NetName = "";                                           /* name of network */
static char *SourcesString = "";           /* string of input files separated by blanks */


/*****************************************************************************************
 Function:                     OUT_GenOutput
 Purpose: Generate output file.
 Returns: void.
*****************************************************************************************/
void OUT_GenOutput(OutFileName)
     char *OutFileName;
{
  FILE *OutFile;                                             /* pointer to output file */
  if(! (OutFile = fopen(OutFileName,"w"))) {
    (void) printf("file i/o: could not open output file %s\n", OutFileName);
    exit(NoOutput);
  }
  else {
    (void) setvbuf(OutFile, NULL, _IOFBF, BUFSIZ);
    (void) printf("writing output into file %s", OutFileName);
    /* set output buffer */
    OUT_PrintHeader(OutFile); 
    OUT_CalcSitesColumn();      /* calculates first position of sites column */
    if(UnitCtr) {
      (void) printf(" ."); (void) fflush(stdout);
      if(SiteTypeCtr)
	OUT_PrintSiteSection(OutFile); 
      (void) printf(" ."); (void) fflush(stdout);
      if(UnitTypeCtr)
	OUT_PrintTypeSection(OutFile); 
      (void) printf(" ."); (void) fflush(stdout);
      if(UnitCtr)
	OUT_PrintUnitSection(OutFile); 
      (void) printf(" ."); (void) fflush(stdout);
      if(ConnectionCtr) {
	OUT_CalcSourcesColumn();
	OUT_PrintConnectionSection(OutFile); 
      }
      (void)  printf(" ."); (void) fflush(stdout);
      (void) fclose(OutFile);
      (void) printf("done\n");
    }
    else
      (void) printf("\n");
    (void) fclose(OutFile);
  }
}




/*****************************************************************************************
 Function:                     OUT_SetSNameLength
 Purpose: Receives a ParserStackType record holding a sites name (or Undef).  If
          length of this new name is > SNameLength, set SNameLength to new length.
 Returns: void.
 Side effects: Modifies SNameLength.
*****************************************************************************************/
void OUT_SetSNameLength(Name)
     ParserStackType *Name; /* & ParserStackType record holding a sites name (or Undef) */
{
  short NewLength;
  if(Name->ytype != Undef)
    NewLength = strlen(Name->yvalue->string);
  if(NewLength > SNameLength)
    SNameLength = NewLength;
}






/*****************************************************************************************
 Function:                     OUT_SetUNameLength
 Purpose: Receives a ParserStackType record holding a units name (or Undef).  If
          length of this new name is > UNameLength, set UNameLength to new length.
 Returns: void.
 Side effects: Modifies UNameLength.
*****************************************************************************************/
void OUT_SetUNameLength(Name)
     char *Name;            /* & ParserStackType record holding a units name (or Undef) */
{
  short NewLength;
  if(Name)
    NewLength = strlen(Name);
  else
    NewLength = 0;
  if(NewLength > UNameLength)
    UNameLength = NewLength;
}





/*****************************************************************************************
 Function:                     OUT_SetActFuncLength
 Purpose: Receives a ParserStackType record holding a units Actfunc (or Undef).  If
          length of this new ActFunc is > ActfuncLength, set ActfuncLength to new length.
 Returns: void.
 Side effects: Modifies ActfuncLength.
*****************************************************************************************/
void OUT_SetActFuncLength(ActFunc)
     ParserStackType *ActFunc;      /* & ParserStackType record holding a units ActFunc */
{
  short NewLength;
  if(ActFunc->ytype != Undef)
    NewLength = strlen(ActFunc->yvalue->var->Name);
  if(NewLength > ActfuncLength)
    ActfuncLength = NewLength;
}




/*****************************************************************************************
 Function:                     OUT_SetOutFuncLength
 Purpose: Receives a ParserStackType record holding a units Outfunc (or Undef).  If
          length of this new OutFunc is > OutfuncLength, set OutfuncLength to new length.
 Returns: void.
 Side effects: Modifies OutfuncLength.
*****************************************************************************************/
void OUT_SetOutFuncLength(OutFunc)
     ParserStackType *OutFunc;      /* & ParserStackType record holding a units OutFunc */
{
  short NewLength;
  if(OutFunc->ytype != Undef)
    NewLength = strlen(OutFunc->yvalue->var->Name);  
  if(NewLength > OutfuncLength)
    OutfuncLength = NewLength;
}





/*****************************************************************************************
 Function:                     OUT_SetTypeLength
 Purpose: Receives a ParserStackType record holding a units Type (or Undef).  If
          length of this new Type is > TypeLength, set TypeLength to new length.
 Returns: void.
 Side effects: Modifies TypeLength.
*****************************************************************************************/
void OUT_SetTypeLength(Type)
     ParserStackType *Type;      /* & ParserStackType record holding a units Type */
{
  short NewLength;
  if(Type->ytype != Undef)
    NewLength = strlen(Type->yvalue->var->Name);
  if(NewLength > TypeLength)
    TypeLength = NewLength;
}





/*****************************************************************************************
 Function:                     OUT_SetSiteFuncLength
 Purpose: Receives a ParserStackSitefunc record holding a units Sitefunc (or Undef).  If
          length of this new Sitefunc is > SitefuncLength, set SitefuncLength to new length.
 Returns: void.
 Side effects: Modifies SitefuncLength.
*****************************************************************************************/
void OUT_SetSiteFuncLength(Sitefunc)
     ParserStackType *Sitefunc;    /* & ParserStackType record holding a units Sitefunc */
{
  short NewLength;
  if(Sitefunc->ytype != Undef)
    NewLength = strlen(Sitefunc->yvalue->var->Name); 
  if(NewLength > SitefuncLength)                          
    SitefuncLength = NewLength;
}






/*****************************************************************************************
 Function:                     OUT_SetPositionLength
 Purpose: Receives an integer position value.  If length of this new position  is >
          PositionLength,  set PositionLength to new length.
 Returns: void.
 Side effects: Modifies PositionLength.
*****************************************************************************************/
void OUT_SetPositionLength(Position)
     int Position;      
{
  short NewLength;
  for(NewLength = 0; Position > 0; NewLength ++, Position /= 10)
    ;
  if(NewLength > PositionLength)
    PositionLength = NewLength;
}






/*****************************************************************************************
 Function:                     OUT_UNoLength
 Purpose: Calculates length of unit no. column.
 Returns: void.
 Side effects: Modifies UNoLength.
*****************************************************************************************/
void OUT_UNoLength()
{
  register MaxNo;
  MaxNo = UnitCtr;
  for(UNoLength = 0; MaxNo > 0; UNoLength ++, MaxNo /= 10)
    ;
}






/*****************************************************************************************
 Function:                     OUT_CalcSitesColumn
 Purpose: Calculates position at which sites column starts.
 Returns: void.
 Side effects: Modifies SitesColumn.
*****************************************************************************************/
void OUT_CalcSitesColumn()
{
  UnitSitesColumn = UNoLength + 3 +                    /* width of column for unit no.s */
    TypeLength + 3 +                             /* width of column for unit type names */
      UNameLength +  3 +                              /* width of column for unit names */
	2 * 8  +   6 +                      /* width of columns for activation and bias */
	  2 +    3 +                                /* width of column for unit S type  */
	    2*(PositionLength) + 1 + 4 + 3 +      /* width of column for unit positions */   /* v2.1 */
	      ActfuncLength +  3 +     /* width of column for unit activation functions */
		OutfuncLength +  3;        /* width of column for unit output functions */

  TypeSitesColumn = TypeLength +                 /* width of column for unit type names */
    ActfuncLength +                    /* width of column for unit activation functions */
      OutfuncLength +                      /* width of column for unit output functions */
	3 * 3;                                       /* separators of columns " | " */
}







/*****************************************************************************************
 Function:                     OUT_CalcSourcesColumn
 Purpose: Calculates position at which sources column starts.
 Returns: void.
 Side effects: Modifies SourcesColumn.
*****************************************************************************************/
void OUT_CalcSourcesColumn()
{
  SourcesColumn =  UNoLength +                         /* width of column for unit no.s */
    SNameLength +                           /* width of column for sites at target unit */
      2 * 3;                                             /* separators of columns " | " */
}






/*****************************************************************************************
 Function:                     OUT_PrintHeader
 Purpose: Print output file header.
 Returns: void.
*****************************************************************************************/
void OUT_PrintHeader(FilePtr)
     FILE *FilePtr;
{
  (void) fprintf(FilePtr, "SNNS network definition file %s\n", VersionNo);
  (void) fprintf(FilePtr, "generated at %s", ctime(&poi));   /* ctime string contains '\n' */
  (void) fprintf(FilePtr, "network name : %s\n", SymTabPtr->Network);
  (void) fprintf(FilePtr, "source files : %s\n", SourcesString);
  (void) fprintf(FilePtr, "no. of units : %d\n", UnitCtr);
  (void) fprintf(FilePtr, "no. of connections : %d\n", ConnectionCtr);
  (void) fprintf(FilePtr, "no. of unit types : %d\n", UnitTypeCtr);
  (void) fprintf(FilePtr, "no. of site types : %d\n", SiteTypeCtr);
  (void) fprintf(FilePtr, "\n\nlearning function : %s\n", DEFAULT_LEARNING_FUNCTION);    /* v2.1 */
  (void) fprintf(FilePtr, "update function : %s\n", DEFAULT_UPDATE_FUNCTION);            /* v2.1 */
}






/*****************************************************************************************
 Function:                     OUT_AppendSource
 Purpose: Append new source file to string of source files.
 Returns: void.
*****************************************************************************************/
void OUT_AppendSource(NewSource)
     char *NewSource;                  /* string holding (physical) name of subnet file */
{
    SourcesString = STR_Append( STR_Append(SourcesString, ", "), NewSource);
}






/*****************************************************************************************
 Function:                     OUT_SetSource
 Purpose: Append new source file to string of source files.
 Returns: void.
*****************************************************************************************/
void OUT_SetSource(FirstSource)
     char *FirstSource;                  /* string holding (physical) name of subnet file */
{
    SourcesString = STR_Save(FirstSource);
}







/*****************************************************************************************
 Function:                     OUT_InsertNetName
 Purpose: Insert network name into string.
 Returns: void.
*****************************************************************************************/
void OUT_InsertNetName(Name)
     ParserStackType *Name;           /* parser value stack record holding network name */
{
    NetName = Name->yvalue->var->Name;                 /* network name is an identifier */
}






/*****************************************************************************************
 Function:                     OUT_PrintSiteSection
 Purpose: Print sites section.
 Returns: void.
*****************************************************************************************/
void OUT_PrintSiteSection(FilePtr)
     FILE *FilePtr;
{
  register SiteListType *ActSite;
  register char *Format;
  register short i;

  Format = STR_Append( STR_Append("%", STR_ItoA(SNameLength)), "s | %s\n");

  (void) fprintf(FilePtr, "\n\n\nsite definition section :\n\n");
  (void) fprintf(FilePtr, Format, "site name", "site function");     /* section header */
  for(i=0; i<SNameLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|---------------\n");             /* separation line */
  for(ActSite = SiteList; ActSite; ActSite = ActSite->NextSite)
    (void) fprintf(FilePtr, Format, ActSite->SiteName, 
		   (ActSite->SiteFunction)? ActSite->SiteFunction->Name : "");
  /* entry lines - block may be empty */
  for(i=0; i<SNameLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|---------------\n");             /* separation line */
}







/*****************************************************************************************
 Function:                     OUT_PrintTypeSection
 Purpose: Print unit type section.
 Returns: void.
*****************************************************************************************/
void OUT_PrintTypeSection(FilePtr)
     FILE *FilePtr;
{
  register ParserStackType *ActPtr;
  register SymTabLineType **SPtr;
  register char *Format;
  register short i, Ctr;
  register char *BlankBegin;

  BlankBegin = M_alloc((unsigned) TypeLength+1);
  for(i=0; i<TypeLength-1; i++)
    *(BlankBegin+i) = ' ';
  *(BlankBegin+TypeLength-1) = '\0';
  Format = STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(STR_Append( "%",
									     STR_ItoA(TypeLength)), 
								  "s | %"),
						       STR_ItoA(ActfuncLength)),
					    "s | %"),
				 STR_ItoA(OutfuncLength)),
		      "s | %s");
  /* missing \n ==> if there's more than one site, followup lines are built */
  (void) fprintf(FilePtr, "\n\n\ntype definition section :\n\n");
  (void) fprintf(FilePtr, Format, "name", "act func", "out func", "sites");  
  (void) fprintf(FilePtr, "\n");
  for(i=0; i<TypeLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  for(i=0; i<ActfuncLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  for(i=0; i<OutfuncLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|--------\n");                    /* separation line */
  for(ActPtr = UnitTypes; ActPtr; ActPtr = ActPtr->ynext) {
    (void) fprintf(FilePtr, Format, ActPtr->yvalue->var->Name,  
		   (ActPtr->yvalue->var->Value->unit->FParam.VarFT->ActFunc) ?
		   ActPtr->yvalue->var->Value->unit->FParam.VarFT->ActFunc->Name : "",  
		   (ActPtr->yvalue->var->Value->unit->FParam.VarFT->OutFunc) ?
		   ActPtr->yvalue->var->Value->unit->FParam.VarFT->OutFunc->Name : "",
		   (ActPtr->yvalue->var->Value->unit->FParam.VarFT->SiteNo) ? 
		   (*(ActPtr->yvalue->var->Value->unit->FParam.VarFT->Sites))->Value->site->SiteName :
		   "");
    if(ActPtr->yvalue->var->Value->unit->FParam.VarFT->Sites)    
      for(SPtr = ActPtr->yvalue->var->Value->unit->FParam.VarFT->Sites + 1,
	  Ctr = 1; Ctr < ActPtr->yvalue->var->Value->unit->FParam.VarFT->SiteNo; SPtr ++, Ctr++) {
      (void) fprintf(FilePtr, ",\n");
      for(i=1; i < TypeSitesColumn; i++)
	(void) fprintf(FilePtr, " ");/* fill until position where sites column starts */
      (void) fprintf(FilePtr, " %s", (*SPtr)->Value->site->SiteName);
    }
    (void) fprintf(FilePtr, "\n");
  }
  for(i=0; i<TypeLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  for(i=0; i<ActfuncLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  for(i=0; i<OutfuncLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|--------\n");                    /* separation line */
  (void) M_free(BlankBegin);
}




/*****************************************************************************************
 Function:                     OUT_PrintUnitSection
 Purpose: Print units section.
 Returns: void.
*****************************************************************************************/
void OUT_PrintUnitSection(FilePtr)
     FILE *FilePtr;
{
  register i; 
  register char *Format,            /* format for entries */ 
                *Header;            /* format for header */

  register char *BlankBegin;

  BlankBegin = M_alloc((unsigned) UnitSitesColumn+1);
  for(i=0; i<UnitSitesColumn-1; i++)
    *(BlankBegin+i) = ' ';
  *(BlankBegin+UnitSitesColumn-1) = '\0';

  Format = STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(
	     STR_Append(STR_Append(STR_Append(	STR_Append(STR_Append(STR_Append(STR_Append(
	        "%", STR_ItoA(UNoLength)), "d | %"), STR_ItoA(TypeLength)),  "s | %"),
	        STR_ItoA(UNameLength)), "s | %8.5f |          | %c  | %"), STR_ItoA(PositionLength)),
        	"d,%"), STR_ItoA(PositionLength)), "d,  0 | %"), STR_ItoA(ActfuncLength)),             /* v2.1 */
		"s | %"), STR_ItoA(OutfuncLength)), "s | %s");

  Header=STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(
                    STR_Append(STR_Append(STR_Append(STR_Append("%", STR_ItoA(UNoLength)),
                    "s | %"), STR_ItoA(TypeLength)),  "s | %"), STR_ItoA(UNameLength)), 
		    "s |    act   |   bias   | st | %"), STR_ItoA(2*PositionLength+1+4)),              /* v2.1 */ 
 		    "s | %"), STR_ItoA(ActfuncLength)), "s | %"), STR_ItoA(OutfuncLength)), 
		    "s | sites\n");

  OUT_PrintNetUnits(FilePtr, Header, Format, BlankBegin);
  (void) M_free(Header);
  (void) M_free(Format);
  (void) M_free(BlankBegin);
}



/*****************************************************************************************
 Function:                     OUT_UnitSep
 Purpose: Separation line for unit section.
 Returns:  void.
*****************************************************************************************/
void OUT_UnitSep(FilePtr)
     FILE *FilePtr;
{
  register i; 
  for(i=0; i<UNoLength; i++)                   /* print upper separation line of section */
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  for(i=0; i<TypeLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  for(i=0; i<UNameLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  (void) fprintf(FilePtr, "%s", "---------|----------|----|-");
  for(i=0; i<2*PositionLength+1+4; i++)                                                              /* v2.1 */
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  for(i=0; i<ActfuncLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|-");
  for(i=0; i<OutfuncLength; i++)
    (void) fprintf(FilePtr, "%c", '-');
  (void) fprintf(FilePtr, "%s", "-|--------\n");                     /* separation line */
}




/*****************************************************************************************
  Function:                     OUT_PrintNetUnits
  Purpose: Print units section.
  Returns: void.
  *****************************************************************************************/
void OUT_PrintNetUnits(FilePtr, Header, Format, BlankBegin)
     FILE *FilePtr;
     register char *Header;         /* format for header */
     register char *Format;         /* format for entries */
     register char *BlankBegin;    /* blank begin of line (for more than one site) */
{
  register SymTabLineType **ActEntry;
  register UnitValueType **ActStruct;

  /* print header for units section */
  (void) fprintf(FilePtr, "\n\n\nunit definition section :\n\n");
  (void) fprintf(FilePtr, Header, "no.", "typeName", "unitName", "position", 
		 "act func", "out func", "sites");         /* header contains \n already */
  OUT_UnitSep(FilePtr);
  for(ActEntry = NetComponents; ActEntry < NetComponents + NetCompCtr; ActEntry ++)
    if((*ActEntry)->Type != SUBNET)  /* subnet components are inserted into global list */
      if((*ActEntry)->Type ==  ArrStruct)                        /* array of structures */
	for(ActStruct = (*ActEntry)->Value->array->aValue.astruct; 
	    ActStruct < (*ActEntry)->Value->array->aValue.astruct + 
	    (*ActEntry)->Value->array->aSize;
	    ActStruct ++)                                 /* scan all structure entries */
	  OUT_PrintStructUnits(*ActStruct, FilePtr, Format, BlankBegin);
      else if( (*ActEntry)->Type == STRUCTURE)                             /* structure */
	OUT_PrintStructUnits((*ActEntry)->Value->structure,  FilePtr, Format, BlankBegin);
      else if( (*ActEntry)->Type == UNIT)                                /* single unit */
	OUT_PrintSingleUnit((*ActEntry)->Value->unit,  FilePtr, Format, BlankBegin);
      else
	(void) printf("******bug: type %s in net component list\n", 
		      STR_TypeString((*ActEntry)->Type));
    else
      (void) printf("******bug: type %s in net component list\n", 
		    STR_TypeString((*ActEntry)->Type));
  OUT_UnitSep(FilePtr);
}







/*****************************************************************************************
 Function:                     OUT_PrintStructUnits
 Purpose: Print unit lines for a structure.
 Returns: void.
*****************************************************************************************/
void OUT_PrintStructUnits(Structure, FilePtr, Format, BlankBegin)
     UnitValueType *Structure;                  /* ptr. to structure description record */
     FILE *FilePtr;           
     char *Format;                            /* format string for unit section entries */
     register char *BlankBegin;        /* blank begin of line (for more than one site) */
{
  register int i, UNo;                                    /* index of unit and unit no. */
  register int *XPos, *YPos;                            /* pointers to x and y position */
  register SymTabLineType **Site;                          /* pointer to act. unit site */
  register char *AFName, *OFName;            /* name of activation and output functions */
  
  XPos = (int*) M_alloc(sizeof(int));           /* allocate space for integer positions */
  YPos = (int*) M_alloc(sizeof(int));        /* & XPos illegal on register variables !! */

  for(UNo = Structure->Begin, i=0; i < Structure->Length; UNo ++, i++) { 
    /* scan all unit no.s of structure */
    AFName = OUT_GetActFunc(Structure->UnitDef);
    OFName = OUT_GetOutFunc(Structure->UnitDef);   
    OUT_GetPosition(XPos, YPos, Structure, UNo - Structure->Begin);
    (void) fprintf(FilePtr, Format, UNo, OUT_GetFType(Structure->UnitDef), 
		   OUT_GetName(Structure, i), OUT_GetAct(Structure, i), 
		   OUT_GetSType(Structure, i), *XPos, *YPos, AFName, OFName, 
		   (OUT_GetFirstSite(Structure->UnitDef, Site)) ? 
		   (*Site)->Value->site->SiteName : "");  
    if(*AFName) (void) M_free(AFName);
    if(*OFName) (void) M_free(OFName);
    /* print explicitly defined sites in unit section (there's no type for structure units */
    if( ! Structure->UnitDef->FType && Structure->UnitDef->FParam.VarFT->SiteNo > 1)
      for(Site = Structure->UnitDef->FParam.VarFT->Sites + 1;
	  Site < Structure->UnitDef->FParam.VarFT->Sites + 
	  Structure->UnitDef->FParam.VarFT->SiteNo; Site ++) { /*print sites*/
	(void) fprintf(FilePtr, ",\n%s%s", BlankBegin, (*Site)->Value->site->SiteName);
      }
    (void) fprintf(FilePtr, "\n");
  }
  (void) M_free((char*) XPos);
  (void) M_free((char*) YPos);
}







/*****************************************************************************************
 Function:                     OUT_PrintSingleUnit
 Purpose: Print unit lines for a structure.
 Returns: void.
*****************************************************************************************/
void OUT_PrintSingleUnit(Unit, FilePtr, Format, BlankBegin)
     UnitType *Unit;                            /* ptr. to structure description record */
     FILE *FilePtr;           
     char *Format;                            /* format string for unit section entries */
     register char *BlankBegin;        /* blank begin of line (for more than one site) */
{
  register SymTabLineType **Site;                          /* pointer to act. unit site */
  register char *AFName, *OFName;            /* name of activation and output functions */
  
  AFName = OUT_GetActFunc(Unit);
  OFName = OUT_GetOutFunc(Unit);
  (void) fprintf(FilePtr, Format, Unit->UnitNo, OUT_GetFType(Unit), 
		 (Unit->FType && Unit->FParam.NamedFT->Value->unit->UName) ? 
		 Unit->FParam.NamedFT->Value->unit->UName : (Unit->UName) ? 
		   Unit->UName : " ",
		 (Unit->FType && Unit->FParam.NamedFT->Value->unit->Act != ActNullValue) ?
		 (Unit->FParam.NamedFT->Value->unit->Act == RANDOM) ? 
		 GETRANDOM : Unit->FParam.NamedFT->Value->unit->Act :
		 (Unit->Act != ActNullValue) ? (Unit->Act == RANDOM) ? GETRANDOM : Unit->Act : 0.0,
		 (Unit->FType && Unit->FParam.NamedFT->Value->unit->SType != Undef) ?
		 OUT_EvalSType(Unit->FParam.NamedFT->Value->unit->SType) : 
		 OUT_EvalSType(Unit->SType),
		 Unit->XPos, Unit->YPos, AFName, OFName, 
		 (OUT_GetFirstSite(Unit, Site)) ? (*Site)->Value->site->SiteName : "");  
  if(*AFName) (void) M_free(AFName);
  if(*OFName) (void) M_free(OFName);
  /* print explicitly defined sites in unit section (there's no type for structure units */
  if( ! Unit->FType && Unit->FParam.VarFT->SiteNo > 1)
    for(Site = Unit->FParam.VarFT->Sites + 1;
	Site < Unit->FParam.VarFT->Sites + 
	Unit->FParam.VarFT->SiteNo; Site ++) { /*print sites*/
      (void) fprintf(FilePtr, ",\n%s %s", BlankBegin, (*Site)->Value->site->SiteName);
    }
  (void) fprintf(FilePtr, "\n");
}





/*****************************************************************************************
 Function:                     OUT_GetFType
 Purpose: Get F type of a structure.
 Returns: String.
*****************************************************************************************/
char *OUT_GetFType(S)
     UnitType *S;                                     /* pointer to structure node */
{
  /* it does not matter wether the unit has been defined explicitly or not -- same type */
  if(S->FType)                           /* there s a named type for this unit */
    if(S->FParam.NamedFT->Name)     
      return S->FParam.NamedFT->Name;
    else {
      (void) printf("******bug: named type without name ? (OUT_GetFType)\n");
      return "****";                                       /* error mark in output file */
    }
  else
    return "";                                                          /* empty entry */
}






/*****************************************************************************************
 Function:                     OUT_GetName
 Purpose: Get name of a unit.
 Returns: String.
*****************************************************************************************/
char *OUT_GetName(S, Ctr)
     UnitValueType *S;                                     /* pointer to structure node */
     int Ctr;                                     /* index of current unit in structure */
{
 
  if(S->UnitDef->FType)                           /* there s a named type for this unit */
    if(S->UnitDef->FParam.NamedFT->Value->unit->UName)      /* name is a type parameter */ 
      return S->UnitDef->FParam.NamedFT->Value->unit->UName;
  if(S->ExplUnits && *(S->ExplUnits+Ctr) )     /* THIS unit has been defined explicitly */
    if((*(S->ExplUnits+Ctr))->UName)          /* explicit name and not a type parameter */
      return (*(S->ExplUnits+Ctr))->UName;
  if(S->UnitDef->UName)                                /* name is a structure parameter */
    return S->UnitDef->UName;
  return "";                                            /* undefined name for this unit */
}






/*****************************************************************************************
 Function:                     OUT_GetSType
 Purpose: Get s type of a unit.
 Returns: character.
*****************************************************************************************/
char OUT_GetSType(S, Ctr)
     UnitValueType *S;                                     /* pointer to structure node */
     int Ctr;                                     /* index of current unit in structure */
{
  SymbolType sType;

  sType = HIDDEN;
  if(S->UnitDef->FType)                           /* there's a named type for this unit */
    if(S->UnitDef->FParam.NamedFT->Value->unit->SType != Undef)
      sType = S->UnitDef->FParam.NamedFT->Value->unit->SType;             /* type param */
    else
      if(S->ExplUnits && *(S->ExplUnits+Ctr) ) /* THIS unit has been defined explicitly */
	if((*(S->ExplUnits+Ctr))->SType != Undef)
	  sType = (*(S->ExplUnits+Ctr))->SType;        /* explicit S type for this unit */
	else
	  sType = S->UnitDef->SType;          /* same S type for all units in structure */
      else
	sType = S->UnitDef->SType; 
  else                                           /* there's no named type for this unit */
    if(S->ExplUnits && *(S->ExplUnits+Ctr) )   /* THIS unit has been defined explicitly */
      if((*(S->ExplUnits+Ctr))->SType != Undef)
	sType = (*(S->ExplUnits+Ctr))->SType;          /* explicit S type for this unit */
      else
	sType = S->UnitDef->SType;            /* same S type for all units in structure */
    else
      sType = S->UnitDef->SType; 
  return OUT_EvalSType(sType);
}





/*****************************************************************************************
 Function:                     OUT_EvalSType
 Purpose: Get s type of a unit. Argument must be in {Undef, INPUT, OUTPUT, HIDDEN, InOut}.
 Returns: character.
*****************************************************************************************/
char OUT_EvalSType(T)
     SymbolType T;                /* is one of {Undef, INPUT, OUTPUT, HIDDEN, and InOut */
{
  switch(T) {        
  case Undef:
  case HIDDEN:                                                     /* hidden is default */
    return 'h';
  case INPUT: 
    return 'i';
  case OUTPUT: 
    return 'o';
  case InOut:  
    return 'g';
  default:
      (void) printf("******bug: unknown s type ? (OUT_EvalSType)\n");
      return '?';                                          /* error mark in output file */
  }
}






/*****************************************************************************************
 Function:                     OUT_GetActFunc
 Purpose: Get activation function name of a unit.
 Returns: String.
*****************************************************************************************/
char *OUT_GetActFunc(S)
     UnitType *S;                                     /* pointer to structure node */
{
  if(S->FType)                           /* there s a named type for this unit */
    return "";                      /* if there's a f type, actfunc cannot be redefined */
  else
    if(S->FParam.VarFT->ActFunc)      /* activation function cannot be defined */
      return S->FParam.VarFT->ActFunc->Name;                     /* explicitly */
    else
  return "";                        /* undefined activation function name for this unit */
}






/*****************************************************************************************
 Function:                     OUT_GetOutFunc
 Purpose: Get output function name of a unit.
 Returns: String.
*****************************************************************************************/
char *OUT_GetOutFunc(S)
     UnitType *S;                                     /* pointer to structure node */
{
  if(S->FType)                           /* there s a named type for this unit */
    return "";                              /* f type - cannot redefine output function */
  else
    if(S->FParam.VarFT->OutFunc)          /* output function cannot be defined */
      return S->FParam.VarFT->OutFunc->Name;                     /* explicitly */
    else
  return "";                            /* undefined output function name for this unit */
}







/*****************************************************************************************
 Function:                     OUT_GetAct
 Purpose: Get Activation function of a unit. If the units activation has been defined
          through keyword RANDOM, generate a new random number in [-1.0. 1.0].
 Returns: float.
*****************************************************************************************/
float OUT_GetAct(S, Ctr)
     UnitValueType *S;                                     /* pointer to structure node */
     int Ctr;                                     /* index of current unit in structure */
{
  register float Val;                            /* temporary var. for activation value */
  Val = 0.0;                     /* standard output value for undefined unit activation */
  if(S->UnitDef->FType)                           /* there's a named type for this unit */
    if(S->UnitDef->FParam.NamedFT->Value->unit->Act != ActNullValue) 
      /* act is a type parameter */ 
      Val =  S->UnitDef->FParam.NamedFT->Value->unit->Act;
  if(Val == 0.0)
    if(S->ExplUnits && *(S->ExplUnits+Ctr))         /* unit has been defined explicitly */
      if((*(S->ExplUnits+Ctr))->Act != ActNullValue)/*explicit value can also be RANDOM */
	Val = (*(S->ExplUnits+Ctr))->Act;
  if(Val == 0.0)
    if(S->UnitDef->Act != ActNullValue)                 /* act is a structure parameter */
      Val =  S->UnitDef->Act;
  if(Val == RANDOM)                             /* generate random value for activation */
    Val = GETRANDOM;
  return Val;
}








/*****************************************************************************************
 Function:                     OUT_GetFirstSite
 Purpose: Get and return first site of uni,if there are sites. Else, return NULL.
 Returns: TRUE <==> structure units have sites.
 Side effects: & SiteValueType or NULL;
*****************************************************************************************/
short OUT_GetFirstSite(S, Site)
     UnitType *S;                                     /* pointer to structure node */
     SymTabLineType **Site;
{                        /* sites cannot be defined explicitly - unit no. not necessary */
  if( ! S->FType && S->FParam.VarFT->SiteNo > 0) {
                               /* type parameters are defined explicitly, no named type */
    Site = S->FParam.VarFT->Sites;                       /* ptr. to first site */
    return TRUE;
  }
  else {
    Site = NULL;
    return FALSE;                     /* sites are type parameter or unit without sites */
  }
}






/*****************************************************************************************
 Function:                     OUT_GetPosition
 Purpose: Calculates x and y position for a single unit.
 Returns: void. 
 Side effects: *XPos and *YPos will hold x and y positions.
 Special assumption: The structures topological dimension is large enaugh for S->Length !
                     PLANE is regarded as MATRIX(1,y) or MATRIX(x,1).
*****************************************************************************************/
void OUT_GetPosition(XPos, YPos, S, Index)
     register int *XPos, *YPos;                             /* to  return result values */
     register UnitValueType *S;                 /* ptr. to structure description record */
     register int Index;                          /* index of current unit in structure */
{
  float TempX, TempY;                         /* for ellipse - avoid float -> int error */
  if(S->Topology.Form == MATRIX) {
    *XPos = S->Topology.XCenter - (int) S->Topology.XDim / 2  +    /* upper left corner */
      Index % S->Topology.XDim;                                 /* matrix line for unit */
    *YPos = S->Topology.YCenter - (int) S->Topology.YDim / 2  +    /* upper left corner */
      Index / S->Topology.XDim;                         /* unit position in matrix line */
  }
  else if(S->Topology.Form == ELLIPSE) {
    TempX = S->Topology.XCenter + S->Topology.XDim / 2 * cos((float) 2 * M_PI * Index / S->Length);
    TempY = S->Topology.YCenter + S->Topology.YDim / 2 * sin((float) 2 * M_PI *Index / S->Length);
    *XPos = (int) TempX;
    *YPos = (int) TempY;
  }
  else if(S->Topology.Form == Undef) {                    /* structure is a single unit */
    *XPos = S->Topology.XCenter;
    *YPos = S->Topology.YCenter;
  }
  else {                                                  /* error -- should not happen */
    (void) printf("******bug: illegal topology form in OUT_GetPosition: %s\n",
		  STR_TypeString(S->Topology.Form));
    *XPos = 0;
    *YPos = 0;
  }
}





/*****************************************************************************************
 Function:                     OUT_ConnSep
 Purpose: Separation line for connection section.
 Returns:  void.
*****************************************************************************************/
void OUT_ConnSep(FilePtr)
     FILE *FilePtr;
{
  register i; 
  for(i=0; i < UNoLength; i++)
    (void) fprintf(FilePtr, "-");
  (void) fprintf(FilePtr, "-|-");
  for(i=0; i < SNameLength; i++)
    (void) fprintf(FilePtr, "-");
  (void) fprintf(FilePtr, "-|--------\n"); 
}





/*****************************************************************************************
  Function:                     OUT_PrintConnectionSection
  Purpose: Print connection section.
  Returns: void.
  *****************************************************************************************/
void OUT_PrintConnectionSection(FilePtr)
     FILE *FilePtr;                                             /* pointer to output file */
{
  register SymTabLineType **ActEntry;
  register UnitValueType **ActStruct;
  register short i;
  register char *Format;                             /* format for entry target - site - 1st. source */
  register char *SourceFormat;
  register char *Header;
  register int SourPerLine;
  register char *BlankBegin;

  BlankBegin = M_alloc((unsigned) SourcesColumn+1);
  for(i=0; i<SourcesColumn-1; i++)
    *(BlankBegin+i) = ' ';
  *(BlankBegin+SourcesColumn-1) = '\0';

  Format = STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(STR_Append("%", STR_ItoA(UNoLength)),
		      "d | %"), STR_ItoA(SNameLength)), "s | %"), STR_ItoA(UNoLength)), "d:%8.5f");
                                         /* format for line including first source unit */
  Header = STR_Append(STR_Append(STR_Append(STR_Append(STR_Append(STR_Append("%", STR_ItoA(UNoLength)),
		      "s | %"), STR_ItoA(SNameLength)), "s | %"), STR_ItoA(UNoLength)), "s:%s\n");      
  SourceFormat = STR_Append(STR_Append(", %", STR_ItoA(UNoLength)), "d:%8.5f");     /* other sources */
  SourPerLine = (MaxLineLength - SourcesColumn) / (10+UNoLength);    /* no. of source entries / line */
  (void) fprintf(FilePtr, "\n\n\nconnection definition section :\n\n");
  (void) fprintf(FilePtr, Header, "target", "site", "source", "weight");

  OUT_ConnSep(FilePtr);
  /* scan all structure components and look for potential target sites */
  for(ActEntry = NetComponents; ActEntry < NetComponents + NetCompCtr; ActEntry ++)
    if((*ActEntry)->Type != SUBNET)   
      if((*ActEntry)->Type ==  ArrStruct)                        /* array of structures */
	for(ActStruct = (*ActEntry)->Value->array->aValue.astruct; 
	    ActStruct < (*ActEntry)->Value->array->aValue.astruct + 
	    (*ActEntry)->Value->array->aSize; 
	    ActStruct ++)                                 /* scan all structure entries */
	  OUT_PrintStructTargets(FilePtr, Format, SourceFormat, SourPerLine, *ActStruct, BlankBegin);
      else if( (*ActEntry)->Type == STRUCTURE)                             /* structure */
	OUT_PrintStructTargets(FilePtr,Format,SourceFormat,SourPerLine, 
			       (*ActEntry)->Value->structure, BlankBegin);
      else if( (*ActEntry)->Type == UNIT)                                /* single unit */
	  OUT_PrintUnitConns(FilePtr, Format, SourceFormat, SourPerLine,
			     (*ActEntry)->Value->unit, BlankBegin); 
      else 
	(void) printf("******bug: type %s in net component list\n", 
		      STR_TypeString((*ActEntry)->Type));
  OUT_ConnSep(FilePtr);
  (void) M_free(BlankBegin);
}





/*****************************************************************************************
  Function:                     OUT_PrintStructTargets
  Purpose: Print entry into connection section for target units.
  Returns: void.
  *****************************************************************************************/
void OUT_PrintStructTargets(FilePtr, Format, SourceFormat, SourPerLine, Structure, BlankBegin)
     FILE *FilePtr;                                             /* pointer to output file */
     register char *Format;               /* format for entry target - site - 1st. source */
     register char *SourceFormat;         /* format for ", sou:-x.yyyyy", further sources */
     register int SourPerLine;           /* no. of sources which fit into aan output line */
     register UnitValueType *Structure;                              /* ptr. to structure */
     register char *BlankBegin;     /* blank begin of line (for more than one conn line) */
{
  register SymTabLineType **Site;                     /* look for sites of each unit type */
  register int UnitNo;                           /* no. of actual (potential) target unit */
  register int SiteMax;          /* maximum no. of sites attached to units (same for all) */
  short Inserted;                                /* no. of sources printed into last line */
  register SymTabLineType **FirstSite;   /* & first site attached to units (same for all) */
  
  if(Structure->UnitDef->FType)  {           /* sites of units defined through named type */
    FirstSite = Structure->UnitDef->FParam.NamedFT->Value->unit->FParam.VarFT->Sites;
    SiteMax = Structure->UnitDef->FParam.NamedFT->Value->unit->FParam.VarFT->SiteNo;
  }
  else {                               /* sites of units defined explicitly for structure */
    FirstSite = Structure->UnitDef->FParam.VarFT->Sites;
    SiteMax = Structure->UnitDef->FParam.VarFT->SiteNo;
  }
  /* scan units  in structure (units are  sorted in ascendant order in connection section */
  for(UnitNo = Structure->Begin; UnitNo < Structure->Begin + Structure->Length; UnitNo ++) {
    if(FirstSite) {                               /* unit has sites -- scan list of sites */
      for(Site = FirstSite; Site < FirstSite + SiteMax; Site ++) {
	Inserted = FALSE;
	if((*Site) == Structure->Through)      /* site is target for implicit connections */
	  Inserted = OUT_PrintImplicitConns(FilePtr, Format, SourceFormat, SourPerLine, 
			        Structure, UnitNo, *Site, BlankBegin);   /* conns printed */
	OUT_PrintExplicitConns(FilePtr, Format, SourceFormat, SourPerLine, UnitNo, *Site,
			       &Inserted, BlankBegin); 
	if(Inserted)                                                    /* finish up line */
	  (void) fprintf(FilePtr, "\n");  
      }
    }
    else {  
      Inserted = FALSE;                                              /* unit has no sites */
      Inserted = OUT_PrintImplicitConns(FilePtr, Format, SourceFormat, SourPerLine, 
	      Structure, UnitNo, (SymTabLineType *) NULL, BlankBegin);   /* conns printed */
      OUT_PrintExplicitConns(FilePtr, Format, SourceFormat, SourPerLine, UnitNo,  
			     (SymTabLineType *) NULL, &Inserted, BlankBegin);
      if(Inserted)                                                      /* finish up line */
	(void) fprintf(FilePtr, "\n");  
    }
  }
}




/*****************************************************************************************
  Function:                     OUT_PrintUnitConns
  Purpose: Print entry into connection section for target units.
  Returns: void.
*****************************************************************************************/
void OUT_PrintUnitConns(FilePtr, Format, SourceFormat, SourPerLine, Unit, BlankBegin)
     FILE *FilePtr;                                           /* pointer to output file */
     register char *Format;             /* format for entry target - site - 1st. source */
     register char *SourceFormat;       /* format for ", sou:-x.yyyyy", further sources */
     register int SourPerLine;         /* no. of sources which fit into aan output line */
     register UnitType *Unit;                                      /* ptr. to structure */
     register char *BlankBegin;     /* blank begin of line (for more than one conn line) */
{
  register SymTabLineType **Site;                   /* look for sites of each unit type */
  register int SiteMax;        /* maximum no. of sites attached to units (same for all) */
  short Inserted;                              /* no. of sources printed into last line */
  register SymTabLineType **FirstSite; /* & first site attached to units (same for all) */
  
  if(Unit->FType)  {                       /* sites of units defined through named type */
    FirstSite = Unit->FParam.NamedFT->Value->unit->FParam.VarFT->Sites;
    SiteMax = Unit->FParam.NamedFT->Value->unit->FParam.VarFT->SiteNo;
  }
  else {                             /* sites of units defined explicitly for structure */
    FirstSite = Unit->FParam.VarFT->Sites;
    SiteMax = Unit->FParam.VarFT->SiteNo;
  }
  if(FirstSite) {                               /* unit has sites -- scan list of sites */
    for(Site = FirstSite; Site < FirstSite + SiteMax; Site ++) {
      Inserted = FALSE;
      OUT_PrintExplicitConns(FilePtr, Format, SourceFormat, SourPerLine, 
			       Unit->UnitNo, *Site, &Inserted, BlankBegin);
      if(Inserted)                                                    /* finish up line */
	(void) fprintf(FilePtr, "\n");  
    }
  }
  else {  
    Inserted = FALSE;                                              /* unit has no sites */
    OUT_PrintExplicitConns(FilePtr, Format, SourceFormat, SourPerLine, Unit->UnitNo,  
			   (SymTabLineType *) NULL, &Inserted, BlankBegin);
    if(Inserted)                                                     /* finish up line */
      (void) fprintf(FilePtr, "\n");  
  }
}




/*****************************************************************************************
  Function:                     OUT_PrintImplicitConns
  Purpose: Print entry into connection section for sources of implicit connections.
  Returns: int: SourPerLine - no. of sources printed into last line.
  *****************************************************************************************/
int OUT_PrintImplicitConns(FilePtr, Format, SourceFormat, SourPerLine, Structure, UnitNo,
			   Site, BlankBegin) 
     FILE *FilePtr;                                             /* pointer to output file */
     register char *Format;               /* format for entry target - site - 1st. source */
     register char *SourceFormat;         /* format for ", sou:-x.yyyyy", further sources */
     register int SourPerLine;           /* no. of sources which fit into aan output line */
     register UnitValueType *Structure;                              /* ptr. to structure */
     register int UnitNo;                               /* index of potential target unit */
     register SymTabLineType *Site;                                        /* target site */
     register char *BlankBegin;     /* blank begin of line (for more than one conn line) */
{
  switch(Structure->StructType) {
  case IRREGULAR:
  case CLUSTER:                   /* note that SUBNET and IRREGULAR are stored as CLUSTER */
    return 0;                                /* clusters do not have implicit connections */
  case CLIQUE:
    return OUT_PrintCliqueImplicit(FilePtr, Format, SourceFormat, SourPerLine, Structure,
				   UnitNo, Site, BlankBegin); 
  case CHAIN:
    return OUT_PrintChainImplicit(FilePtr, Format, SourceFormat, Structure, UnitNo, Site); 
  case RING:
    return OUT_PrintRingImplicit(FilePtr, Format, SourceFormat, Structure, UnitNo, Site); 
  case STAR:
    return OUT_PrintStarImplicit(FilePtr, Format, SourceFormat, SourPerLine, Structure,
				 UnitNo, Site, BlankBegin);
  case UNIT:
    return 0;
  default:
    (void) printf("******bug: unknown structure type %s\n", STR_TypeString(Structure->StructType));
    return 0;
  }
}





/*****************************************************************************************
  Function:                     OUT_PrintCliqueImplicit
  Purpose: Print entry into connection section for sources of implicit connections.
  Returns: int: SourPerLine - no. of sources printed into last line.
  *****************************************************************************************/
short OUT_PrintCliqueImplicit(FilePtr, Format, SourceFormat, SourPerLine, Structure, UnitNo,
			      Site, BlankBegin) 
     FILE *FilePtr;                                             /* pointer to output file */
     register char *Format;
     register char *SourceFormat;         /* format for ", sou:-x.yyyyy", further sources */
     register int SourPerLine;           /* no. of sources which fit into aan output line */
     register UnitValueType *Structure;                              /* ptr. to structure */
     register int UnitNo;                               /* index of potential target unit */
     register SymTabLineType *Site;                                        /* target site */
     register char *BlankBegin;     /* blank begin of line (for more than one conn line) */
{
  register int SNo;                                             /* actual source unit no. */
  register short LineEntries;            /* source units written into current output line */
  register short First;                  /* TRUE <==> target and site entries are printed */

  /* site is already known as target site for implicit connections */
  /* unitNo is  target of all other structure units - print conn.  line until 1st. source */
  /* print other connections with other format SourceFormat */
  for(SNo = Structure->Begin, LineEntries = 0, First = TRUE; 
      SNo < Structure->Begin + Structure->Length; SNo ++) 
    if(SNo != UnitNo)                            /* a unit cannot be connected to itself */
      if(First) {
	(void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "", 
		       SNo, (Structure->Weight == RANDOM) ? 
		       GETRANDOM  : Structure->Weight);
	LineEntries ++;
	First = FALSE;
      }
      else
	if(LineEntries < SourPerLine) {      /* print next source into same output line */
	  (void) fprintf(FilePtr, SourceFormat, SNo, 
			 (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
	  LineEntries ++;
	}
	else {                                       /* initialize new line for sources */
	  (void) fprintf(FilePtr, ",\n%s", BlankBegin);/* finish up old line, begin new */
	  /* do not print first symbol of sourceFormat for this single print statement (",") */
	  (void) fprintf(FilePtr, SourceFormat+1, SNo, 
			 (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
	  LineEntries = 1;
	}
  return LineEntries;
}





/*****************************************************************************************
  Function:                     OUT_PrintChainImplicit
  Purpose: Print entry into connection section for sources of implicit connections.
  Returns: int: SourPerLine - no. of sources printed into last line.
  *****************************************************************************************/
short OUT_PrintChainImplicit(FilePtr, Format, SourceFormat, Structure, UnitNo, Site) 
     FILE *FilePtr;                                             /* pointer to output file */
     char *Format;                        /* format for entry target - site - 1st. source */
     register char *SourceFormat;         /* format for ", sou:-x.yyyyy", further sources */
     register UnitValueType *Structure;                              /* ptr. to structure */
     register int UnitNo;                               /* index of potential target unit */
     register SymTabLineType *Site;                                        /* target site */
{
  register short LineEntries;            /* source units written into current output line */
  LineEntries = 0;
  /* site is already known as target site for implicit connections */
  switch(Structure->Direction) {
  case PT:
    /* unitNo is  target of unitNo -1 if this unit is part of structure */
    if(Structure->Begin <= UnitNo - 1) {
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "", 
		     UnitNo-1, 
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      return 1;
    }
  case PF:
    /* unitNo is  target of unitNo + 1 if this unit is part of structure */
    if(Structure->Begin + Structure->Length > UnitNo + 1) {
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "", 
		     UnitNo+1,
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      return 1;
    }
  case PTF: 
    /* for forward: unitNo is  target of unitNo - 1 if this unit is part of structure */
    if(Structure->Begin <= UnitNo - 1) {
      (void) fprintf(FilePtr, Format, UnitNo, Site->Value->site->SiteName, UnitNo-1,
		 (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      LineEntries = 1;
    }
    /*  for backward: unitNo is  target of unitNo + 1 if this unit is part of structure */
    if(Structure->Begin + Structure->Length > UnitNo + 1) {
      (void) fprintf(FilePtr, SourceFormat, UnitNo, UnitNo+1,(Structure->Weight == RANDOM) ? 
		     GETRANDOM  : Structure->Weight);
      LineEntries ++;
    }
    return LineEntries;
  default:
    (void) printf("******bug: illegal direction (implicit): %s\n", STR_TypeString(Structure->Direction));
    return 0;
  }
}







/*****************************************************************************************
  Function:                     OUT_PrintRingImplicit
  Purpose: Print entry into connection section for sources of implicit connections.
  Returns: int: SourPerLine - no. of sources printed into last line.
  *****************************************************************************************/
short OUT_PrintRingImplicit(FilePtr, Format, SourceFormat, Structure, UnitNo, Site) 
     FILE *FilePtr;                                             /* pointer to output file */
     char *Format;                        /* format for entry target - site - 1st. source */
     register char *SourceFormat;         /* format for ", sou:-x.yyyyy", further sources */
     register UnitValueType *Structure;                              /* ptr. to structure */
     register int UnitNo;                               /* index of potential target unit */
     register SymTabLineType *Site;                                        /* target site */
{
  register short LineEntries;            /* source units written into current output line */
  LineEntries = 0;
  /* site is already known as target site for implicit connections */
  switch(Structure->Direction) {
  case PT:
    /* unitNo is  target of unitNo -1 if this unit is part of structure */
    if(Structure->Begin <= UnitNo - 1) {
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "", 
		     UnitNo-1,
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      return 1;
    }
    else if(UnitNo == Structure->Begin) {                     /* first is target of last */
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "", 
		     Structure->Begin + Structure->Length - 1,              /* last unit */
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      return 1;
    }
  case PF:
    /* unitNo is  target of unitNo + 1 if this unit is part of structure */
    if(Structure->Begin + Structure->Length > UnitNo + 1) {
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "", 
		     UnitNo+1,
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      return 1;
    }
    else if(UnitNo == Structure->Begin + Structure->Length - 1) { /* last is target of 1st */
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "", 
		     Structure->Begin,
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      return 1;
    }
  case PTF: 
    /* for forward: unitNo is  target of unitNo - 1 if this unit is part of structure */
    if(Structure->Begin <= UnitNo - 1) {
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "",
		     UnitNo-1,
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight); 
      LineEntries = 1;
    }
    else if(UnitNo == Structure->Begin) {                     /* first is target of last */
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "",
		     Structure->Begin + Structure->Length - 1,              /* last unit */
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
       LineEntries = 1;
    }
    /*  for backward: unitNo is  target of unitNo + 1 if this unit is part of structure */
    if(Structure->Begin + Structure->Length > UnitNo + 1) {
      (void) fprintf(FilePtr, SourceFormat, UnitNo+1, (Structure->Weight == RANDOM) ? 
		     GETRANDOM  : Structure->Weight);
      LineEntries ++;
    }
    else if(UnitNo == Structure->Begin) {                     /* first is target of last */
      (void) fprintf(FilePtr, SourceFormat, UnitNo, Structure->Begin + Structure->Length - 1, /* last unit */
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      LineEntries ++;
    }
    return LineEntries;
  default:
    (void) printf("******bug: illegal direction (implicit): %s\n", STR_TypeString(Structure->Direction));
    return 0;
  }
}









/*****************************************************************************************
  Function:                     OUT_PrintStarImplicit
  Purpose: Print entry into connection section for sources of implicit connections.
  Returns: int: SourPerLine - no. of sources printed into last line.
  *****************************************************************************************/
short OUT_PrintStarImplicit(FilePtr, Format, SourceFormat, SourPerLine, Structure, UnitNo,
			    Site, BlankBegin) 
     FILE *FilePtr;                                             /* pointer to output file */
     char *Format;                        /* format for entry target - site - 1st. source */
     register char *SourceFormat;         /* format for ", sou:-x.yyyyy", further sources */
     register int SourPerLine;           /* no. of sources which fit into aan output line */
     register UnitValueType *Structure;                              /* ptr. to structure */
     register int UnitNo;                               /* index of potential target unit */
     register SymTabLineType *Site;                                        /* target site */
     register char *BlankBegin;     /* blank begin of line (for more than one conn line) */
{
  /* site is already known as target site for implicit connections */
  switch(Structure->Direction) {
  case PT:
    if(Structure->Center + Structure->Begin != UnitNo) {
      /* unitno is not center of star ==> there is a conn. from center to unitNo. */
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "",
		     Structure->Center + Structure->Begin,
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      return 1;
    }
    return 0;
  case PF:
    if(Structure->Begin + Structure->Center == UnitNo) {
      /* unitNo is center of star ==> there are conns. from all units to unitNo ==> like clique */
      return OUT_PrintCliqueImplicit(FilePtr, Format, SourceFormat, SourPerLine, Structure,
				     UnitNo, Site, BlankBegin); 
    }
    return 0;
  case PTF:
    if(Structure->Begin + Structure->Center == UnitNo) {
      /* unitNo is center of star ==> there are conns. from all units to unitNo ==> like clique */
      return OUT_PrintCliqueImplicit(FilePtr, Format, SourceFormat, SourPerLine, Structure,
				     UnitNo, Site, BlankBegin); 
    }
    else {   /* Structure->Center + Structure->Begin != UnitNo */
      (void) fprintf(FilePtr, Format, UnitNo, (Site) ? Site->Value->site->SiteName : "",
		     Structure->Center + Structure->Begin,
		     (Structure->Weight == RANDOM) ? GETRANDOM  : Structure->Weight);
      return 1;
    }
  default:
    (void) printf("******bug: illegal direction (implicit): %s\n", STR_TypeString(Structure->Direction));
    return 0;
  }  
}





/*****************************************************************************************
 Function:                     OUT_PrintExplicitConns
 Purpose: Look for explicit connections to a given unit and print them into the output 
          file.
 Returns: void. 
*****************************************************************************************/
void OUT_PrintExplicitConns(FilePtr, Format, SourceFormat, SourPerLine, UnitNo, Site,
			    Inserted, BlankBegin) 
     register FILE *FilePtr;                                    /* pointer to output file */
     register char *Format;               /* format for entry target - site - 1st. source */
     register char *SourceFormat;         /* format for ", sou:-x.yyyyy", further sources */
     register int SourPerLine;           /* no. of sources which fit into aan output line */
     int UnitNo;                                        /* index of potential target unit */
     SymTabLineType *Site;                                                 /* target site */
     register short *Inserted;    /* no. of inserted pairs source:weight into output line */
     register char *BlankBegin;      /* blank begin of line (for more than one conn line) */
{
  register SourceEntryType  *ActS;      /* pointer to a sources table entry with SNo != 0 */
  register SourceEntryType  *SEnd;                    /* pointer to end of source entries */
  register TargetSiteType *ActSite;                             /* pointer to sites entry */
  register TargetUnitType *TPos;   /* position of unit with no. TNo. in target unit table */
  register SiteListType *ASite;
 
  if( ! CONN_Connections)                 /* there has been no explicit connection before */
    return;                                /* there are no explicitly defined connections */
  TPos = CONN_LookUpTarget(CONN_Connections, UnitNo);        /* delivers pointer to entry */
  if( ! TPos->TNo)
    return;
  /* look for site - works for units without sites: Sites == NULL and there's only */
  /* TPos->Site  set with TPos->Site == NULL. Correctness  has been checked before */
  for(ActSite = TPos->Site, ASite = (Site) ? Site->Value->site : NULL; 
      ActSite; ActSite = ActSite->tNext) 
    if(ActSite->tSite == ASite)
      break;                             /* look up target site in UNSORTED list of sites */
  if(! ActSite)                                  /* there are no connections to this site */
    return;
  else {                                                 /* site is target of connections */
    for(ActS = ActSite->tInput.Sources, SEnd = ActS + (int) (ActSite->tInput.Size );
	ActS < SEnd; ActS ++)
      if(ActS->SNo) 
	if( ! (*Inserted)) {   /* there have been no mplicit connections - write target.. */
	  (void) fprintf(FilePtr, Format, UnitNo, (ASite) ? Site->Value->site->SiteName : "", 
			 ActS->SNo, ActS->Weight);            /* .. at first call, only ! */
	  (*Inserted) = 1;
	}
	else {
	  if(*Inserted < SourPerLine)  {                    /* source fits into same line */
	    (void) fprintf(FilePtr, SourceFormat, ActS->SNo, ActS->Weight);  
	    (*Inserted) ++;
	  }
	  else {                                       /* initialize new line for sources */
	    (void) fprintf(FilePtr, ",\n%s", BlankBegin);/* finish up old line, begin new */
	    (void) fprintf(FilePtr, " ");   /* begin of new line is filled up with blanks */
	    /* do not print first symbol of sourceFormat for single print statement (",") */
	    (void) fprintf(FilePtr, SourceFormat+1, ActS->SNo, ActS->Weight);  
	    (*Inserted) = 1;
	  }
	}
  }
}
  
	
