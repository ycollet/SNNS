/*
 * File:     (%W%    %G%)
 * Purpose:  definition of uniform selection routines
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
#include "uniformSel.h"


#define UNIF_SEL_KEY   "unifSel"
#define NO_OF_OFFS     "gensize"
#define SEL_PROB       "selProb" 


#define SEL_ERROR 2



static int   offSize = OFF_SIZE_VALUE;
static float selProb = 0.5;



int unifSel_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    MODULE_KEY( UNIF_SEL_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT   ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT   ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT ) { /* nothing to do */ }
    
    MSG_CASE( OFF_SIZE       ) { if( msgc > 1 ) offSize = atoi( msgv[1] ); }
    MSG_CASE( SEL_PROB       ) { if( msgc > 1 ) selProb = atof( msgv[1] ); }
    
    END_MSG;

    return( INIT_USED );
}


int unifSel_work( PopID *parents, PopID *offsprings, PopID *ref )
{
    int off_cnt = 0;
    NetID net, offspring;
    NetworkData *pData, *oData;

    while( off_cnt < offSize )
    {
	FOR_ALL_PARENTS( net )
	{
	    if( RAND_01 < selProb )
	    {
		oData = utils_getNewNetData();
		pData = GET_NET_DATA( net );
		oData->histRec.parent1 = pData->histID;
		
		offspring = kpm_copyNet( net, oData );
		kpm_setPopMember( offspring, *offsprings );
		off_cnt++;    
	    }
	    if( off_cnt >= offSize ) break;
	}
    }

    return( MODULE_NO_ERROR );
}


char *unifSel_errMsg( int err_code )
{
    static int err_cnt = 3;
    static char *err_msg[] =
    {"no error", "unknown error",
     "there are no parents/offsprings present for selection!\n"};

    return( err_msg[err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR] );
}
