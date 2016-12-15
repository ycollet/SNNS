/*
 * File:     (%W%    %G%)
 * Purpose:  definition of jog weights functions
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
#include "jogWeights.h"


#define JOGWEIGHTS_KEY     "jogWeights"
#define JOGWEIGHTS_FACTOR  "jogLimit"


#define ERROR           2
#define ERROR_ACTIVATE 10

/*--------------------------------------------------------------variables----*/


static int   result  = 0;         /* no of the error-code -> internal use    */
static FlintType jogLimit = 0.01;



/*--------------------------------------------------------------functions----*/


/***************************************************************************/
/*                                                                         */
/*  int jogWeights_init (ModuleTableEntry *self, char *s)                   */
/*                                                                         */
/*  Reads the incoming string and handles all initial work                 */
/*                                                                         */
/***************************************************************************/


int jogWeights_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    
    MODULE_KEY( JOGWEIGHTS_KEY );
    
    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT  ) { /* nothing to do */ }
    MSG_CASE( JOGWEIGHTS_FACTOR  ) { if( msgc > 1 ) 
				    {
                                      jogLimit = (FlintType) atof( msgv[1] );
				    }
                                }
    END_MSG;

    return ( INIT_USED );
}



int jogWeights_work( PopID *parents, PopID *offsprings, PopID *reference )
{
    
  NetID activeMember;

  FOR_ALL_OFFSPRINGS ( activeMember )
  {
      ksh_jogWeights( -jogLimit, jogLimit );
  }
    
  return( MODULE_NO_ERROR ); 
}


/***************************************************************************/
/*                                                                         */
/*  char *jogWeights_errMsg(int err_code)                                   */
/*                                                                         */
/*  returns the actual error message of the error code.                    */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

char *jogWeights_errMsg(int err_code)
{
  
  switch (result)
    {
    case 0 :
      return ("jogWeights : No error found");

    case ERROR_ACTIVATE :
      return ("jogWeights : Can't activate a offspring-net");
    }
  
  return ("jogWeights : Unknown error, please consulte your systemadmin");
}
