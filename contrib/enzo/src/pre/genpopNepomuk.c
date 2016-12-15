/*
 * File:     (%W%    %G%)
 * Purpose:  genpopNepomuk generates all size for the networks and pattern   
 *           via Nepomuk. 
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
#include "genpopNepomuk.h"

/*--------------------------------------------------------------macros-------*/

#define GENPOP_NEPOMUK_KEY  "genpopNepo"

#define ERROR_NO_POPULATION 2
#define ERROR_NEPOMUK       3


/*--------------------------------------------------------------variables----*/

static int population_size = POP_SIZE_VALUE;
static int generation_size = OFF_SIZE_VALUE;

/*--------------------------------------------------------------functions----*/

/*--------------------------------------------------------------------------*/
/*  int genpopNepomuk_init( ModuleTableEntry *self, int msgc, char *msgv[] )*/
/*                                                                          */
/*--------------------------------------------------------------------------*/


int genpopNepomuk_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{

    MODULE_KEY( GENPOP_NEPOMUK_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT  ) { /* nothing to do */ }

    MSG_CASE( OFF_SIZE        ) { if( msgc > 1 )
				      generation_size = atoi( msgv[1] );
			        }
    MSG_CASE( POP_SIZE        ) { if( msgc > 1 )
				      population_size = atoi( msgv[1] );
			        }
    END_MSG;

    return( INIT_USED ); 
}
 
/*--------------------------------------------------------------------------*/
/*                                                                          */
/* int genpopNepomuk_work(PopID *parents,PopID *offsprings,PopID *reference)*/
/*                                                                          */
/*                                                                          */
/* Generates a population of neural networks in Nepomuk and divides it      */
/* into the three subpopulation parents, offsprings and reference.          */
/* The PopID will be assigned to the calling parameters.                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
 
int genpopNepomuk_work( PopID *parents, PopID *offsprings, PopID *reference) 
{
  if ( ( population_size == 0 ) || ( generation_size == 0 ) )
    {
      return ( ERROR_NO_POPULATION );
    }

  /* 
     allocate memory for the reference ent, the population and the generation
     at this point, the patternsize is 3 for the learn, the test and the
     crossvalidation-patterns.
  */

  if (kpm_initialize( population_size+generation_size+1,3) != MODULE_NO_ERROR)
    {
      return (ERROR_NEPOMUK);
    }

  /* 
     creating the three subpopulation from the global one :  
     The first population_size elements are assigned to the
     parent-population, the tail of the nets are assigned to the
     offspring-population.                                        
     The first net of the queue is the reference net (contains to no sub-
     population).
     the third population contains tonly on net, the reference net.
  */


  *parents    = kpm_newPopID ();
  *offsprings = kpm_newPopID ();
  *reference  = kpm_newPopID ();

  return(MODULE_NO_ERROR);

}

/*--------------------------------------------------------------------------*/
/* char *genpopNepomuk_errMsg( int err_code )                               */
/*                                                                          */

/*--------------------------------------------------------------------------*/

char *genpopNepomuk_errMsg( int err_code) 
{
  switch ( err_code )
    {                
    case MODULE_NO_ERROR :
      return ("genPopNepomuk : No error found");

    case ERROR_NO_POPULATION :
      return("genPopNepomuk: No populationsize / generationsize is assigned");

    case ERROR_NEPOMUK :
      return("genPopNepomuk: Error from Nepomuk");
    }

  return("genPopNepomuk: I don't known much more about the error then you do");
}

