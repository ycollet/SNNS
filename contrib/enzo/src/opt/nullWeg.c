/*
 * File:     (%W%    %G%)
 * Purpose:  delete nets without links
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
#include "nullWeg.h"


#define NULLWEG_KEY  "nullWeg"


#define MAX_DEL_NETS  100


int nullWeg_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    MODULE_KEY( NULLWEG_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT  ) { /* nothing to do */ }

    END_MSG;

    return( INIT_USED );
}


int nullWeg_work( PopID *parents, PopID *offsprings, PopID *ref )
{
    NetID net;
    NetID delList[MAX_DEL_NETS];
    
    int no_of_links, delCnt = 0, i;
    
    FOR_ALL_OFFSPRINGS( net )
    {
	GET_NO_OF_LINKS( no_of_links );
	if( no_of_links < 1 )
	{
	    delList[delCnt++] = net;
	}
    }

    for( i = 0; i < delCnt; i++ ) kpm_deleteNet( delList[i] );

    return( MODULE_NO_ERROR );
}


char *nullWeg_errMsg( int err_code )
{
    /* supply the caller with some information about an error */

    static int   err_cnt   = 2;   /* number of recognized errors */
    static char *err_msg[] =
    { "no error (nullWeg)", "unknown error (nullWeg)",
      "specific error message -- not used"};

    return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}
