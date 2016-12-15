/*
 * File:     mutLinks.c
 * Purpose:  mutate the weights of all offsprings                                 
 *             - existing weights can be deleted              
 *             - deleted weights between living units can be added  
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
#include "mutLinks.h"



#define SIMPLE_MUT_KEY "mutLinks"
#define PROBDEL        "probdel"
#define PROBDEL_START  "probdelStart"
#define PROBDEL_ENDGEN "probdelEndGen"
#define SIGMA          "sigmaDel"
#define PROBADD        "probadd"
#define RANGE          "initRange"


#define ERROR_ACTIVATE 10
#define ERROR_NO_REF   11


/* ------------------------------------------------------------- variables ---*/

static float     probDel   = 0.0;  /* prob. for deleting an existing link     */
static float     probDelStart = 0.0;   /* start with this prob for deleting */
static int       probDelEnd   = 0;     /* generation to reach probDel */
static float     ssigma    = 1.0;  
static float     probAdd   = 0.0;  /* prob. for adding an absend link         */
static NetDescr  refDesc;          /* Pointer to the description of the       */
                                   /* reference net, not the net itself       */
static float     range     = 0.5;


/* -------------------------------------------------------------------------- */
/*                                                                            */

int mutLinks_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    float lprobDel;
    float lprobAdd;
    NetID refNet;
    PopID *reference;
    
    MODULE_KEY( SIMPLE_MUT_KEY );
    
    SEL_MSG( msgv[0] )
    
    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT  ) {
	reference = (PopID *) msgv[1];
	refNet = kpm_popFirstMember ( *reference );
	if (refNet == NULL)
	return ( ERROR_NO_REF );
	kpm_getNetDescr ( refNet, &refDesc );
    }
    
    MSG_CASE( PROBDEL         ) { if( msgc > 1 ) 
				  {
                                      lprobDel= atof( msgv[1] );
				      if (( lprobDel >= 0.0) && (lprobDel <= 1.0))
				      probDel = lprobDel;
				  }
			      }
    MSG_CASE( PROBADD         ) { if( msgc > 1 )
				  {
				      lprobAdd= atof( msgv[1] );
				      if (( lprobAdd > 0.0) && (lprobAdd < 1.0))
				      probAdd = lprobAdd;
				  }
			      }
    MSG_CASE( SIGMA ) { if( msgc > 1 ) ssigma = atof(msgv[1])*atof(msgv[1]); }
    
    MSG_CASE( PROBDEL_START  ) { if( msgc > 1 ) probDelStart = atof(msgv[1]); }
    MSG_CASE( PROBDEL_ENDGEN ) { if( msgc > 1 ) probDelEnd = atoi(msgv[1]); }

    MSG_CASE( RANGE           ) { if( msgc > 1)
				  range = atof ( msgv[1] );
			      }
    END_MSG;
    
    return ( INIT_USED );
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int mutLinks_work (PopID *reference PopID *parents, PopID *offsprings) */
/*                                                                          */
/*  The mutLinks-function mutates all offspring-nets. If the link from     */
/*  the reference-net exists in the offspring , it will be delete if        */
/*  rnd() < probDel.                                                        */
/*  If the link from the reference-net is absend, when the link will be     */
/*  added then both units are active and rnd() < probAdd.                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int mutLinks_work( PopID *parents, PopID *offsprings, PopID *reference )
{
    NetworkData *data;
    NetID activeMember;
    float weight, pd;
    int i;
    int del,add;
    int unit1,unit2;
    char *uname1,*uname2;
    static int genCnt = 0;
    

    if( genCnt < probDelEnd )
        pd = probDelStart+(probDel-probDelStart)*(genCnt/(float)probDelEnd);
    else
        pd = probDel;
    genCnt++;
    
    
    FOR_ALL_OFFSPRINGS ( activeMember )
    
    {
	if (kpm_setCurrentNet( activeMember ) != KPM_NO_ERROR)
	{
	    return ( ERROR_ACTIVATE );
	}
	
	add = del = 0;
	data = kpm_getNetData ( activeMember );
	
	/* Just testing all links from the reference-net                */
	
	for (i = 0; i < refDesc.no_of_links; i++)
	{
	    /* Search via the unitname the corrosponding units in the    */
	    /* offspring-net                                             */
	    /* Be careful : Unitno. don't match Untino. in links !!      */
	    
	    uname1 = refDesc.units[ refDesc.weights[i].source - 1 ].name;
	    uname2 = refDesc.units[ refDesc.weights[i].target - 1 ].name;
	    
	    unit1 = ksh_searchUnitName(uname1);
	    unit2 = ksh_searchUnitName(uname2);
	    
	    /* If one of the two units doesn't exist then nothing can    */
	    /* happen to the connection.                                 */
	    
	    if ((!unit1) || (!unit2))	    continue;
	    
	    if( ksh_areConnectedWeight( unit1, unit2, &weight ) )
	    {
		if( RAND_01 < pd * exp(-weight*weight/ssigma) )
		{
		    ksh_deleteLink();    del++;
		}
	    }
	    else
	    { 
		if(   ( RAND_01 < probAdd )
		   && (!subul_deadInputUnit (unit1))
		   && (!subul_deadInputUnit (unit2))  )
		{
		    ksh_setCurrentUnit( unit2 );
		    ksh_createLink( unit1, RANDOM ( -range , range ));
		    add++;
		}
		
	    }
	    
	} /* endfor no_of_links */
	
	data->histRec.added   += add;
	data->histRec.deleted += del;
	data->histRec.threshold = pd;

    } /* endfor FOR ALL OFFSPRINGS */
    
    return( MODULE_NO_ERROR ); 
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  char *mutLinks_errMsg(int err_code)                                     */
/*                                                                           */
/*  returns the actual error message of the error code.                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *mutLinks_errMsg(int err_code)
{
    switch ( err_code )
    {
      case MODULE_NO_ERROR :
      return("mutLinks : No Error found");
      
    case ERROR_ACTIVATE :
    return ("mutLinks : Can't activate a offspring-net");
      
    case ERROR_NO_REF :
    return ("mutLinks : Can't activate the reference-net");
  }
    
    return ("mutLinks : Unknown error!");
}
