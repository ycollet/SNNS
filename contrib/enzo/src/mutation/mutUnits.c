/*
 * File:     
 * Purpose:  mutUnits adds or delete whole units and their links    
 *           of the offspring networks.                                    
 *           Only one unit is added or deleted each time.                  
 *           Possible switches are:                                        
 *              - delete unit with bypass                                    
 *              - search weak unit for deleting (PWU)  
 *
 *    
 *           #######     #     #     #######      #####  
 *           #           ##    #          #      #     #
 *           #           # #   #         #       #     #
 *           ######      #  #  #        #        #     #
 *           #           #   # #       #         #     #
 *           #           #    ##      #          #     #
 *           #######     #     #     #######      #####
 *
 *             ( Evolutionaerer NetZwerk Optimierer )
 *
* Implementation:   1.0
 *               adapted to:       SNNSv4.0    
 *
 *                      Copyright (c) 1994 - 1995
 *      Institut fuer Logik, Komplexitaet und Deduktionssysteme
 *                        Universitaet Karlsruhe 
 *
 * Authors: Johannes Schaefer, Matthias Schubert, Thomas Ragg
 * Release: 1.0, August 1995 
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice, author statement and this permission
 * notice appear in all copies of this software and related documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS,
 * IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL THE INSTITUTE OF LOGIC, COMPLEXITY AND DEDUCTION SYSTEMS OR
 * THE UNIVERSITY OF KARSLRUHE BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF
 * DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 *      date        | author          | description                          
 *    --------------+-----------------+------------------------------------  
 *      dd. mon. yy | name of author  | Short description of changes made.   
 *                  | (initials)      | Mark changed parts with initials.    
 *                  |                 |                                      
 *                                                                           
 */                                                                           



#include "enzo.h"
#include "mutUnits.h"

/*--------------------------------------------------------------macros-------*/

#define MUT_UNITS_KEY "mutUnits"
#define BYPASS        "bypass"
#define PWU           "PWU"
#define MUTUNIT_PROB  "probMutUnits"
#define MUTSPLIT      "probMutUnitsSplit"
#define RANGE          "initRange"


/*--------------------------------------------------------------structures---*/

typedef struct {
  int index;
  float fitness;
} indexRecord;


/*--------------------------------------------------------------variables----*/

static NetID         refNet;
static NetDescr      refDesc;     /* Pointer to the description of the       */
                                  /* reference net, not the net itself       */

static NetDescr      activeDesc;
static int           bypass    = TRUE;
static int           pwu       = TRUE;
static float         prob      = 0.5;
static float         split     = 0.5;
static indexRecord   *indexRec = NULL;
static int           *succ     = NULL;
static int           *pred     = NULL;
static float         range     = 0.5;
/*--------------------------------------------------------------functions----*/

/*---------- getIndexAlive --------------------------------------------------*/
/*                                                                           */
/* Determines the number of existing units in the current net.               */
/* All existing units are filled in the indexRec                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/


int getIndexAlive()
{
  int maxUnits,i,j, unit;
  int pos = 0;

  maxUnits = refDesc.no_of_units;

  for ( i = 0; i < maxUnits; i++)
    {
      indexRec[i].index    = -1;
      indexRec[i].fitness  = INFINITY;
    }
  
  for (i = 0; i < activeDesc.no_of_units; i++)
    {
      if (    ksh_getUnitTType( activeDesc.units[i].number) != OUTPUT
	   && ksh_getUnitTType( activeDesc.units[i].number) != INPUT  )
      {
	  indexRec[pos].index = unit = activeDesc.units[i].number;
	  indexRec[pos].fitness = 0;
	  for (j = 0; j < activeDesc.no_of_links; j ++)
	  {
	      if ( (activeDesc.weights[j].target == unit) ||
		   (activeDesc.weights[j].source == unit))
		indexRec[pos].fitness++;
	    }
	  pos++;
	}
    }
  return (pos);
}


/*---------- areConnectedInRef ----------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
    
int areConnectedInRef ( int unit1, int  unit2)
{
  NetID activeNet;
  char *name1, *name2;
  int ref1 ,ref2;
  int rv = FALSE;
  float weight;

  name1 = ksh_getUnitName(unit1);
  name2 = ksh_getUnitName(unit2);

  activeNet = kpm_getCurrentNet ();
  kpm_setCurrentNet ( refNet );
  
  ref1 = ksh_searchUnitName (name1);
  ref2 = ksh_searchUnitName (name2);

  if ((ref1) && (ref2) && (ksh_areConnectedWeight( ref1, ref2, &weight)))
    rv = TRUE;

  kpm_setCurrentNet (activeNet);

  return ( rv );
}

/*---------- deleteUnitWithBypass -------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int deleteUnitWithBypass ( int delUnit )
{
  int i,j;
  int noSucc = 0;
  int noPred = 0;
  float weight;
  int delete = 0;
  int add    = 0;


  ksh_setCurrentUnit (delUnit);

  /* find all succ of the bypassed unit           */

  for (i = ksh_getFirstSuccUnit(delUnit,&weight); i != 0; 
       i = ksh_getNextSuccUnit(&weight))
    {
      delete++;
      succ[noSucc] = i;
      noSucc++;
    }
    
  /* find all pred of th bypassed unit */ 
  for (i = ksh_getFirstPredUnit(&weight); i != 0; 
       i = ksh_getNextPredUnit(&weight))
    {
      delete++;
      pred[noPred] = i;
      noPred++;
    }

  /* connect the pred with the succs                        */

  for (i = 0; i < noSucc; i++)
    for (j = 0; j < noPred; j++)
      {
	if ((!ksh_areConnectedWeight(pred[j], succ[i], &weight)) &&
	    ( areConnectedInRef(pred[j], succ[i]))             )
	  {
	    ksh_setCurrentUnit ( succ[i] );
	    ksh_createLink(pred[j], RANDOM (-1.0, 1.0));
	    add++;
	  }
      }
  ksh_deleteUnitList(1, &delUnit);
  return (add-delete);

}

/*---------- sortIndex ------------------------------------------------------*/
/*                                                                           */
/* sort the indexRec by fitness                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void sortIndex ( int bound )
{
  indexRecord change;
  int i,j;
  
  /* simple bubble sort with 0.5 n^2 com., good enough for small problems   */

  for (i = 0; i < bound ; i++)
    {
      for (j = i+1; j < bound ; j++)
	{
	  if (indexRec[i].fitness > indexRec[j].fitness)
	    {
	      change      = indexRec[i];
	      indexRec[i] = indexRec[j];
	      indexRec[j] = change;
	    }
	}
    }
}


/*---------- deleteUnit ----------------------------------------------------*/
/*                                                                           */
/* delete a SNNS unit and return the no. of deleted links                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int deleteUnit (int index)
{
  ksh_deleteUnitList( 1, &index );
  return ( 0 );
}


/*---------- getDeletedUnitIndex --------------------------------------------*/
/*                                                                           */
/* Determines the number of a deletes unit in the current net                */
/* the no. describes the entry of the Netdescr. of the reference-net-unit    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int getDeletedUnitIndex (int *index)
{

  int deadNo = 0;
  int InputUnit = 0;
  int i,search,unit;
  char *name;
  for (i=0; i < refDesc.no_of_units; i++)
    {
      name = refDesc.units[i].name;
      if((unit = ksh_searchUnitName( name )) == 0)
	{
	  deadNo++;
	}
      else if (subul_deadInputUnit ( unit ))
	{
	  deadNo++;
	}
    }

  if (deadNo == 0)
    *index = -1;

  else
    {
      do 
	{
	  search = RAND_01 * deadNo;
	}
      while (search == deadNo);

      deadNo = 0;

      for (i=0; i < refDesc.no_of_units; i++)
	{
	  name = refDesc.units[i].name;
	  if((unit = ksh_searchUnitName( name )) == NULL)
	    {
	      if (deadNo == search)
		{
		  *index = i;
		  InputUnit = 0;
		  break;
		}
	      else 
		deadNo++;
	    }
	  if (subul_deadInputUnit (unit))
	    {	    
	      if (deadNo == search)
		{
		  *index = i;
		  InputUnit = unit;
		  break;
		}
	      else
		deadNo++;
	    }
	}
    }

  return ( InputUnit );
}


void setSNNSUnitInformations (int unit_no, int index)
{
  if( refDesc.units[index].name )
    ksh_setUnitName( unit_no, refDesc.units[index].name );

  if( refDesc.units[index].FTypeName ) 
    ksh_setUnitFType( unit_no, refDesc.units[index].FTypeName );
        
  ksh_setUnitActFunc( unit_no, refDesc.units[index].actFuncName  );
  
  ksh_setUnitOutFunc( unit_no, refDesc.units[index].outFuncName  );
  
  ksh_setUnitActivation ( unit_no, refDesc.units[index].activation );
  
  ksh_setUnitInitialActivation( unit_no, refDesc.units[index].initAct );
  
  ksh_setUnitOutput ( unit_no, refDesc.units[index].output );

  ksh_setUnitBias ( unit_no, RANDOM(-1.0,1.0) );
        
  ksh_setUnitSubnetNo( unit_no, refDesc.units[index].subnetNo );

  ksh_setUnitLayerNo ( unit_no, refDesc.units[index].layerNo );

  ksh_setUnitTType( unit_no, refDesc.units[index].TType );
	  
  if( refDesc.units[index].frozen )
    ksh_freezeUnit( unit_no );

  ksh_setUnitPosition( unit_no, &refDesc.units[index].position );
}



/*---------- createAllNewLinks ----------------------------------------------*/
/*                                                                           */
/* creates all links to and from an added Unit which are defined in the      */
/* reference network                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int createAllNewLinks ( int unit_no , int index )
{
  int i,unit2, add = 0;
  char *uname;
  for (i = 0; i < refDesc.no_of_links; i++)
    {
      if (refDesc.weights[i].target == (index+1) )
	{
	  uname = refDesc.units[ refDesc.weights[i].source - 1 ].name;
	  unit2 = ksh_searchUnitName(uname);

	  /* only source-units can be dead inputunits                */
	  if ((!unit2) || subul_deadInputUnit (unit2))  /*corrected masch 5.94*/
	    continue;

	  ksh_setCurrentUnit ( unit_no);
	  ksh_createLink( unit2, RANDOM ( -range , range ));
	  add ++;
	}
      else if (refDesc.weights[i].source == (index + 1))
	{
	  uname = refDesc.units[refDesc.weights[i].target -1].name;
	  unit2 = ksh_searchUnitName(uname);
	  if (!unit2)
	    continue;
	  ksh_setCurrentUnit ( unit2 );
	  ksh_createLink(unit_no, RANDOM ( -1.0 , 1.0));
	  add++;
	}		    
    } /*endfor all links */
  return ( add );
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int mutUnits_init (ModuleTableEntry *self,  int msgc, char *msgv[] )    */
/*                                                                          */
/*  Reads the incoming string and handles all initial / exit work           */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int mutUnits_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    float lprob,lsplit;
    PopID *reference;
    
    MODULE_KEY( MUT_UNITS_KEY );
    
    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT  ) {
                                   reference = (PopID *) msgv[1];
                                   refNet = kpm_popFirstMember ( *reference );
                                   kpm_getNetDescr ( refNet, &refDesc );
                                   succ = (int *) malloc ( sizeof (int) * 
                                                           refDesc.no_of_units);
                                   pred = (int *) malloc ( sizeof (int) * 
                                   refDesc.no_of_units);
                                   indexRec = (indexRecord *) 
                                               malloc (sizeof (indexRecord) * 
						       refDesc.no_of_units);
                                }

    MSG_CASE( RANGE           ) { if( msgc > 1)
				    range = atof ( msgv[1] );
				}

    MSG_CASE ( MUTSPLIT )      {
                                 if (msgc > 1)
				   {
				     lsplit = (float) atof( msgv[1] );
				     if ((lsplit >= 0.0) && (lsplit <= 1.0))
				       split = lsplit;
				   }
			       }

    MSG_CASE ( MUTUNIT_PROB )  {
                                 if (msgc > 1)
				   {
				     lprob = (float) atof( msgv[1] );
				     if ((lprob >= 0.0) && (lprob <= 1.0))
				       prob = lprob;
				   }
			       }

    MSG_CASE( PWU )             {
                                  if (msgc > 1)
				    {
				      if FLAG_VALUE( msgv[1])
					pwu = FALSE;
				      else
					pwu = TRUE;
				    }
				}

    MSG_CASE ( BYPASS )         {
                                  if (msgc > 1)
				    {
				      if FLAG_VALUE ( msgv[1] )
					bypass = FALSE;
				      else
					bypass = TRUE;
				    }
				}
    END_MSG;

    return ( INIT_USED );
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int mutUnits_work (PopID *reference PopID *parents, PopID *offsprings)  */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int mutUnits_work( PopID *parents, PopID *offsprings, PopID *reference )
{
  NetworkData *data;
  NetID activeMember;
  int index,maxIndex;
  int add, delete,byp;
  int unit_no;
  

  /* using a Nepomuk-macro to step over all offspring-nets            */

  FOR_ALL_OFFSPRINGS ( activeMember )

    {

      data = kpm_getNetData ( activeMember );
      kpm_getNetDescr (activeMember, &activeDesc );
      add = delete = byp = 0;

      /* First test if a mutation takes place                         */
      
      if (RAND_01 > prob)
	{
	  kpm_freeNetDescr ( &activeDesc );
	  continue;
	}

      if ( RAND_01 > split )
	{
	  /* Add a unit                                               */
	  unit_no = getDeletedUnitIndex ( &index );

	  if (index == -1)
	    {
	      kpm_freeNetDescr ( &activeDesc );
	      continue;
	    }
	  /* check if the added unit is an inputunit (exist) or not    */

	  if (!unit_no)
	    {
	      unit_no = ksh_createDefaultUnit();
	      setSNNSUnitInformations ( unit_no, index);
	    }

	  add = createAllNewLinks ( unit_no, index );

	} /* endif add unit */

      else
	{
	  maxIndex = getIndexAlive();
	  if (!maxIndex)
	    {
	      kpm_freeNetDescr ( &activeDesc );
	      continue;
	    }

	  if (pwu)
	    {
	      sortIndex (maxIndex);
	      do 
		{
		  unit_no = pow(RAND_01, 3.0) * maxIndex;
		}
	      while (unit_no == maxIndex);
	    }
	  
	  else
	    do
	      {
		unit_no = RAND_01 * ((float) maxIndex);
	      }
	    while (unit_no == maxIndex);
	  
	  index = indexRec[unit_no].index;
	      
	  if (bypass)

	    byp = deleteUnitWithBypass (index);

	  else

	    delete = deleteUnit (index);
	}

	  kpm_freeNetDescr ( &activeDesc );

    } /* endfor FOR ALL OFFSPRINGS */
  
  return( MODULE_NO_ERROR ); 
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  char *simpleMut_errMsg(int err_code)                                     */
/*                                                                           */
/*  returns the actual error message of the error code.                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *mutUnits_errMsg(int err_code)
{
  switch ( err_code )
    {
    case MODULE_NO_ERROR :
      return("simpleMut : No Error found");
    
    }
  
  return ("simpleMut : Unknown error!");
}

