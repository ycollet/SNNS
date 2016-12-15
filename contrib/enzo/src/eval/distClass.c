/*
 * File:     (%W%    %G%)
 * Purpose:  definition of classification routines
 * Notes:
 *    correct <=> output \in U( target, radius )                             
 *    wrong   <=> \exists vector != target: output \in U( vector, radius )   
 *    reject  <=> \forall vector: output not \in U( vector, radius )         
 *    Target for class i:  (0, ..., 0,  1, 0, ..., 0)                        
 *                          0  ... i-1  i  i+1 ... n                         
 *    U( x, r ) := { y : |y-x| < r }                                         
 *                                                                           
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
#include "distClass.h"


#define DISTCLASS_KEY     "distClass"

#define DISTANCE        "radius"
#define HIT_VALUE       "hitRating"
#define MISS_VALUE      "missRating"
#define NONE_VALUE      "noneRating"
#define RND_SUBSET      "rndSubset"    /* rndSubset <size> */

#define ERR_NO_TEST_PATS 2
#define ERR_NO_DATA      3

#define DELTA(i,j)  (i==j ? 1.0 : 0.0)   /* the Kronnecker-delta */


/* ------------------------------------------------------------ variables --- */

static float radius     = 0.5;     /* (1/sqrt(2))^2 */
static PatID testPats   = NULL;

static float hitRating  = 0.0;
static float missRating = 1.0;
static float noneRating = 1.0;

static int rndSubset = 0;
     
/* ------------------------------------------------------------ functions --- */
     
int distClass_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    MODULE_KEY( DISTCLASS_KEY );
    
    SEL_MSG( msgv[0] )
    
    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT  )
    {
	if( testPats == NULL ) testPats = subul_getPatID( PATTERN_TEST );
    }
    
    MSG_CASE( DISTANCE )   { if (msgc > 1)
			     {
				 radius  = fabs( atof (msgv[1]));
				 radius *= radius;
			     }
			   }
    MSG_CASE( HIT_VALUE  ){ if( msgc > 1 )  hitRating = (float) atof(msgv[1]); }
    MSG_CASE( MISS_VALUE ){ if( msgc > 1 ) missRating = (float) atof(msgv[1]); }
    MSG_CASE( NONE_VALUE ){ if( msgc > 1 ) noneRating = (float) atof(msgv[1]); }
    MSG_CASE( RND_SUBSET ){ if( msgc > 1 )  rndSubset =         atoi(msgv[1]); }
    
    END_MSG;
    
    return( INIT_USED );
}


int distClass_work ( PopID *parents, PopID *offsprings, PopID *reference )
{
    NetID activeMember;
    NetworkData *data;
    float dist;
    int i, j, hit, miss, none, noPattern, cnt;
    
    if( kpm_setCurrentPattern( testPats ) != KPM_NO_ERROR )
    {
	return( ERR_NO_TEST_PATS );
    }
    
    if( rndSubset == 0 )   cnt = ksh_no_patterns();
    else
    {
	ksh_shufflePatterns( TRUE );
	cnt = rndSubset;
    }
    
    FOR_ALL_OFFSPRINGS( activeMember )
    {
	hit = miss = none = 0;
	
	if( (data = GET_NET_DATA( activeMember )) == NULL )
	{
	    return( ERR_NO_DATA );
	}

	
	for( noPattern = 1; noPattern <= cnt; noPattern++)
	{
	    ksh_propagate_pattern ( noPattern);
	    ksh_get_target_pattern( noPattern);

	    dist = 0.0;
	    for( i = 0; i < ksh_no_outputs(); i++ )
	    {
		dist += (ksh_target(i)-ksh_netout(i))*(ksh_target(i)-ksh_netout(i));
	    }

	    if( dist == 0.0 )
	    {
		enzo_logprint( "net %s\n", ksh_netname() );
		enzo_logprint( "dist == 0.0 for pattern %d\n", noPattern );
	    }
	    if( dist < radius ) { hit++; continue; }

	    for( i = 0; i < ksh_no_outputs()
		        && dist > radius;  i++ )  /* for_all_classes */
	    {
		dist = 0.0;
		for( j = 0; j < ksh_no_outputs(); j++ )
		{
		    dist += (DELTA(i,j)-ksh_netout(j))*(DELTA(i,j)-ksh_netout(j));
		}
	    }
	    if( dist < radius ) { miss++; /* this isn't the target class! */ }
	    else                { none++; }
	}	    
	
	data->fitness += hit*hitRating + miss*missRating + none*noneRating;
	
	data->histRec.testHit     = hit;
	data->histRec.testNone    = none;
	data->histRec.testMiss    = miss;
	data->histRec.testFitness = ((float) hit)/(hit+none+miss);
    }
    
    return( MODULE_NO_ERROR );
}


char *distClass_errMsg( int err_code )
{
    /* supply the caller with some information about an error */
    
    static int   err_cnt   = 2;   /* number of recognized errors */
    static char *err_msg[] =
    { "no error (distClass)", "unknown error (distClass)",
      "specific error message -- not used"};
    
    return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}

