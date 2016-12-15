/*
 * File:     (%W%    %G%)
 * Purpose:  declaration of optimized init population  routines
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
#include "optInitPop.h"

/*--------------------------------------------------------------macros-------*/

#define OPT_INIT_POP_KEY       "optInitPop"
#define MAX_TSS                "maxtss"
#define LEARN_MODUL            "learnModul"

#define ERROR_MODUL            14

#define MAX_DEL_UNITS         200
#define MAX_MODUL_NAME        100

/*--------------------------------------------------------------variables----*/

static float maxTss                     = 0.5;
static char  learnModul[MAX_MODUL_NAME] = "learnSNNS";
static int   modulNo                    = -1;


/*--------------------------------------------------------------functions----*/

/*---------------------------------------------------------------------------*/
/* void deleteHiddenUnits (int count)                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#if 0
static void old_deleteHiddenUnits ( int count )
{
    int delcnt = 0,
        rm_cnt, k;
    int delList[MAX_DEL_UNITS];
    float randsel;

    randsel = 0.5 * (1.0 - ((float ) count / ksh_getNoOfTTypeUnits( HIDDEN )));

    while( delcnt < count )
    {
	rm_cnt = 0;
	for( k = ksh_getFirstUnit(); k != 0; k = ksh_getNextUnit() )
	{
	    if( (RAND_01 < randsel) && 
	        (ksh_getUnitTType( k ) == HIDDEN) &&
	        (delcnt+rm_cnt < count))
	    {
		delList[rm_cnt++] = k;
	    }
	}
	delcnt += rm_cnt;
	ksh_deleteUnitList( rm_cnt, delList );
    }
}
#endif

static void deleteHiddenUnits( int cnt )
{
    int i, j, h, n;
    int delList[MAX_DEL_UNITS];

    /* create a list of all hidden units */
    for( h=0, i = ksh_getFirstUnit(); i != 0; i = ksh_getNextUnit() )
    {
	if( ksh_getUnitTType( i ) == HIDDEN )  delList[h++] = i;
    }

    /* shuffle this list */
    for( i = 0; i < h; i++ )
    {
	j = (lrand48() % (h-i)) + i;
	n = delList[i];	delList[i] = delList[j]; delList[j] = n;
    }

    /* and delete the first count elements */
    ksh_deleteUnitList( cnt, delList );

}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int optInitPop_init ( ModuleTableEntry *self, int msgc, char *msgv[]    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int optInitPop_init (ModuleTableEntry *self, int msgc, char *msgv[] )
{
    int i;
    
    MODULE_KEY( OPT_INIT_POP_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }

    MSG_CASE( EVOLUTION_INIT  ) { /* nothing to do */ }

    MSG_CASE( MAX_TSS         ) { if( msgc > 1 )
				      maxTss = (float) atof( msgv[1] );
			        }

    MSG_CASE( LEARN_MODUL       ) { if( msgc > 1 )
				      strcpy(learnModul, msgv[1]);
      	                           for(i = 0; ModuleTable[i].name!= NULL; i++ )
				   if( !strcmp(ModuleTable[i].name,learnModul) )
				       modulNo = i;
				  }

    END_MSG;

    return ( INIT_USED );
}



/*--------------------------------------------------------------------------*/
/*                                                                          */
/* int optInitPop_work ( PopID *parents, PopID *offsprings,                 */
/*                       PopID *reference)                                  */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/


int optInitPop_work (PopID *parents, PopID *offsprings, PopID *reference)
{
  NetID refNet, activeNet;
  NetworkData *data;
  PopID learn;
  int i,hiddenUnits;
  int maxHiddenUnits;
  int lowerBound, upperBound;

  learn = kpm_newPopID( );
    
  refNet = kpm_popFirstMember( *reference );
  maxHiddenUnits = ksh_getNoOfTTypeUnits( HIDDEN );

  
  i = 0;
  lowerBound = 0;
  upperBound = maxHiddenUnits;

  /* Searching for the min. of hidden-units with binary search               */

  activeNet = kpm_popFirstMember( *offsprings );
  while( (lowerBound < upperBound) && (activeNet != NULL))
    {
      kpm_setCurrentNet( activeNet );
      data =  kpm_getNetData( activeNet ); 
      hiddenUnits = (lowerBound + upperBound) / 2;

      /* delete not used hidden units                                 */

      maxHiddenUnits = ksh_getNoOfTTypeUnits( HIDDEN );
      deleteHiddenUnits( maxHiddenUnits - hiddenUnits );

      kpm_setPopMember( activeNet, learn );

      /*learnSNNS_work( parents, &learn, reference );*/

      if (modulNo == -1)
	return(ERROR_MODUL);

      ModuleTable[modulNo].workFct(parents, &learn, reference ); 

      kpm_setPopMember( activeNet, *parents );

      /* Check if the training was sucessful                            */

      activeNet = kpm_popNextMember( *offsprings, activeNet );
      if( data->tss < maxTss )	upperBound = hiddenUnits;
      else        	        lowerBound = hiddenUnits + 1;
    }

  while( activeNet  != NULL )
  {
      kpm_setCurrentNet( activeNet );

      maxHiddenUnits = ksh_getNoOfTTypeUnits( HIDDEN );     
      hiddenUnits = (int) RANDOM( upperBound, maxHiddenUnits+1 );
      
      deleteHiddenUnits( maxHiddenUnits - hiddenUnits );

      activeNet = kpm_popNextMember( *offsprings, activeNet );
  }

  FOR_ALL_PARENTS( activeNet )
    kpm_setPopMember (activeNet, *offsprings);

  
  return (MODULE_NO_ERROR);

}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* char *optInitPop_errMsg( int err_code )                                  */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

char *optInitPop_errMsg (int err_code)
{
  static char error[MAX_ERR_MSG_LEN]; /* TR: should be static or global variable, if returned */
  switch ( err_code )       
    {                   
    case MODULE_NO_ERROR :
      return ("optInitPop : No error found");

    case ERROR_MODUL :
      sprintf (error, "optInitPop : Can't activate Module %s", learnModul);
      return error; /* TR: return was missing */

    }
  return ("optInitPop : unknown error");
}
