/*
 * File:     (%W%    %G%)
 * Purpose:  inputInit crates random net for the initial population;         
 *           the lower bound on the number of hidden units is 0 if not set,         
 *           the upper is given by the reference net. 
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

/*    inputInit crates random net for the initial population;              */
/*    the lower bound on the number of hidden units is 0 if not set,         */
/*    the upper is given by the reference net.                               */
/*                                                                           */
/*    Author: Matthias Schubert                                              */
/*    Date:   26. Apr. 1994                                                  */
/*                                                                           */
/*    Changes:                                                               */
/*                                                                           */
/*      date        | author          | description                          */
/*    --------------+-----------------+------------------------------------  */
/*      dd. mon. yy | name of author  | Short description of changes made.   */
/*                  | (initials)      | Mark changed parts with initials.    */
/*                  |                 |                                      */
/*                                                                           */
/*                                                                           */
/* ------------------------------------------------------------------------- */


#include "enzo.h"
#include "inputInit.h"

/*--------------------------------------------------------------macros-------*/

#define RAND_INIT_POP_KEY      "inputInit"
#define LOWER_BOUND            "minNoInput"
#define UPPER_BOUND            "maxNoInput"




/*--------------------------------------------------------------variables----*/

static int   lowerBound = 0;
static int   upperBound = 0;

/*--------------------------------------------------------------functions----*/


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int inputInit_init ( ModuleTableEntry *self, int msgc, char *msgv[]    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int inputInit_init (ModuleTableEntry *self, int msgc, char *msgv[] )
{
    
    MODULE_KEY( RAND_INIT_POP_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT  ) { /* nothing to do */ }

    MSG_CASE( LOWER_BOUND )     {  if( msgc > 1 )
	                               lowerBound = atoi( msgv[1] );
			        }
    MSG_CASE( UPPER_BOUND )    {   if (msgc > 1)
				       upperBound = atoi( msgv[1] );
			       }
      

    END_MSG;

    return ( INIT_USED );
}



/*--------------------------------------------------------------------------*/
/*                                                                          */
/* int inputInit_work ( PopID *parents, PopID *offsprings,                  */
/*                      PopID *reference)                                   */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/


int inputInit_work (PopID *parents, PopID *offsprings, PopID *reference)
{
  NetID activeNet;
  int   maxInputUnits, inputUnits;
  int   i,count;
  int   delcnt;
  float randsel;

  FOR_ALL_OFFSPRINGS( activeNet )
    {  
      /* first termine the number of existing input units, which
	 aren't dead                                               */

      maxInputUnits = 0;
      for( i = ksh_getFirstUnit(); i != 0; i = ksh_getNextUnit() )
	{
	  if(( ksh_getUnitTType( i ) == INPUT ) && 
	     !subul_deadInputUnit( i ))
	    maxInputUnits++;
	}
	 
      /* now determine the number of used units                    */

      if ( upperBound )
	{
	  inputUnits = (int) RANDOM( lowerBound, upperBound+1 );
	  if (inputUnits > maxInputUnits)
	    inputUnits = maxInputUnits;
	}
      else
	inputUnits = (int) RANDOM( lowerBound, maxInputUnits+1 );
      
      /* now delete the random input units                          */
      delcnt = 0;
      count = maxInputUnits - inputUnits;
      randsel = 0.75 * (1.0 - (inputUnits / (float) maxInputUnits));

      while (delcnt < count)
	{
	  for ( i = ksh_getFirstUnit(); i != 0; i = ksh_getNextUnit())
	    {
	      if ((RAND_01 < randsel) && 
		  (ksh_getUnitTType( i ) == INPUT) &&
		  (!subul_deadInputUnit( i )) &&
		  (delcnt < count))
		{
		  ksh_setCurrentUnit( i );
		  ksh_deleteAllOutputLinks();
		  delcnt++;
		}
	    }
	}
    }

  
  return (MODULE_NO_ERROR);

}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* char *inputInit_errMsg( int err_code )                                   */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

char *inputInit_errMsg (int err_code)
{
  switch ( err_code )       
    {                   
    case MODULE_NO_ERROR :
      return ("inputInit : No error found");
    }
  return ("inputInit : unknown error");
}
