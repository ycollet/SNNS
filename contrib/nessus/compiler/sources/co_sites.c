/*****************************************************************************
  FILE              : co_sites.c
  SHORTNAME         : sites
  VERSION           : 2.0

  PURPOSE           : Contains functions that manage the list of site types for a network. This list is 
                        local to co_sites.c but sites are global for the whole net including eventual 
			subnets. Contains
	                    - SI_MakeSite
			    - SI_CountSiteTypes
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/


#include "co_sites.h"




/*****************************************************************************************
 Function:                     SI_MakeSite
 Purpose: Build a SiteListRecord and check if site name is unique. Insert new record into 
          sorted list of site description records (SiteList) and return pointer to it.
 Returns: Pointer to new site description record (SiteListType) or NULL, if there's been 
          an error.
 Side effects: Modifies SiteList and records appended to list.
*****************************************************************************************/
ParserStackType *SI_MakeSite(Name, Function)
     ParserStackType *Name;        /* points to parser stack value holding name of site */
     ParserStackType *Function;    /* parser stack value holding identifier for site */
{				   /* function */
  register SiteListType *ActSite;  /* scan already existing sites on list */
  register SiteListType *NewSite;  /* points to new site record */
  register short CompVal;          /* hold value of string comparison */

  NewSite = NULL;
  if(Name->ytype != Undef && Function->ytype != Undef)  /* site components are correct */
    if(SiteList)                                            /* there are sites already */
      /* compare name of new site type to name of first site type in SiteList */
      if( ! (CompVal = strcmp(SiteList->SiteName, Name->yvalue->string))) {
	/* name of new site type is not unique -> semantic error */
	ER_StackError("cannot redefine type of site with name %s\n", Serious,
		      STR_Save(SiteList->SiteName), nst, nst);
	/* DO NOT  REMOVE PARSER STACK ELEMENT - return previously defined site */
	Function->ytype = SITE;
	Function->yvalue->site = SiteList;
	return Function;
      }
      else if(CompVal > 0) {    /* new site! (list is in lexical order of their names) */
	/* build new site list  element - it becomes first element in list */

	NewSite = (SiteListType *) M_alloc(sizeof(SiteListType));
	NewSite->NextSite = SiteList;
	(SiteList = NewSite)->SiteName =  Name->yvalue->string;    /* name of new site */
	SiteList->SiteFunction = Function->yvalue->var;      /* pointer to function id */
      }
      else {                 /* entry for site type not found - scan tail of site list */
	/* compare new name always to ActSite->NextSite - maybe we must insert our new */
	/* site type between ActSite and ActSite->NextSite */
	for(ActSite = SiteList; ActSite->NextSite && 
	    (CompVal = strcmp(ActSite->NextSite->SiteName, Name->yvalue->string)) < 0; 
	    ActSite = ActSite->NextSite)
	  ; 
	if( ! CompVal && ActSite->NextSite) {      
	  /* name of new site type is not unique -> semantic error */
	  ER_StackError("multiple types for site with name %s\n", Serious,
		      STR_Save(ActSite->NextSite->SiteName), nst, nst);
	  /* DO NOT  REMOVE PARSER STACK ELEMENT - return previously defined site */
	  Function->ytype = SITE;
	  Function->yvalue->site = ActSite->NextSite;
	  return Function;
      }
	else {   
	  /* 1) search exhausted site list -> site is new and becomes new last element */
	  /* 2) ActSite->NextSite->SiteName < new name -> site is new, too */
	  /* insert new site after ActSite, maybe in front of ActSite->NextSite */
	  NewSite = (SiteListType *) M_alloc(sizeof(SiteListType));
	  NewSite->SiteName =  Name->yvalue->string; 
	  NewSite->SiteFunction = Function->yvalue->var;       /* pointer to function id */
	  NewSite->NextSite = ActSite->NextSite;
	  ActSite->NextSite = NewSite;
	}
      }
    else {   /* there are no elemmentson site list - insert first site type */
      NewSite = (SiteListType *) M_alloc(sizeof(SiteListType));
      NewSite->NextSite = NULL;
      (SiteList = NewSite)->SiteName =  Name->yvalue->string;       /* name of new site */
      SiteList->SiteFunction = Function->yvalue->var;         /* pointer to function id */
    }
  PA_FreeParserStack(Name);           /* remove parser stack record holding name string */
  if(NewSite) {            /* use parser stack record holding function to return result */
    Function->ytype = SITE;
    Function->yvalue->site = NewSite;
    return Function;
  }
  else {
    PA_FreeParserStack(Function);
    return NULL;
  }
}






/*****************************************************************************************
 Function:                     SI_CountSiteTypes
 Purpose: Count sites and set SiteTypeCtr.
 Returns: void.
*****************************************************************************************/
void SI_CountSiteTypes()
{
  register SiteListType *ActSite;

  for(SiteTypeCtr = 0, ActSite = SiteList; ActSite; 
      ActSite = ActSite->NextSite, SiteTypeCtr ++)
    ;
}
