/*
 * File:     (%W%    %G%)
 * Purpose:  definition of prefer selection routines
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
#include "preferSel.h"


#define PREFER_SEL_KEY   "preferSel"
#define ALPHA            "preferfactor"


#define ERROR_INDEX      10
#define ERROR_NO_PARENTS 11
#define PAT_MEM_ERROR    12


static int   gensize        = OFF_SIZE_VALUE;
static float preferfactor   = 3.0;


int preferSel_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
    MODULE_KEY( PREFER_SEL_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT ) {
        /* nothing to do */
    }
    MSG_CASE( GENERAL_EXIT ) {
        /* nothing to do */
    }
    MSG_CASE( EVOLUTION_INIT ) {
        /* nothing to do */
    }

    MSG_CASE( OFF_SIZE     ) {
        if( msgc > 1 ) gensize = atoi( msgv[1] );
    }
    MSG_CASE( ALPHA        ) {
        if( msgc > 1 ) preferfactor = atof( msgv[1] );
    }

    END_MSG;

    return( INIT_USED );
}


int preferSel_work( PopID *parents, PopID *offsprings, PopID *reference ) {
    int off_cnt = 0;
    int popsize = 0;
    NetID net, offspring;
    NetworkData *pData, *oData;
    int i, k, index, no_of_patterns;
    int *osp, *psp;

    off_cnt = 0;

    /* First determine the size of the actual parent population   */

    FOR_ALL_PARENTS( net )  {
        popsize++;
    }


    if( popsize == 0 )    return( ERROR_NO_PARENTS );


    no_of_patterns = ksh_getNoOfPatterns();   /* max number of patterns */

    while( off_cnt < gensize ) {
        do {
            index = (int) (pow (RAND_01, preferfactor) * popsize);
        } while (index >= popsize);

        i = 0;
        FOR_ALL_PARENTS( net ) {
            if( i == index ) {
                oData = utils_getNewNetData();
                pData = kpm_getNetData( net );
                oData->histRec.parent1 = pData->histID;
                oData->parent1         = net;           /* masch 13.05.94 */

                /* if there are selected patterns copy them to the offspring */
                if( pData->selectedPattern ) {
                    oData->selThresh = pData->selThresh;
                    oData->selectedPattern = (int *) calloc( no_of_patterns,
                                             sizeof( int )   );
                    if( !oData->selectedPattern ) {
                        return( PAT_MEM_ERROR );
                    }
                    osp = oData->selectedPattern;
                    psp = pData->selectedPattern;
                    for(  k=0; (osp[k] = psp[k]) != 0; k++ ) /* nix */;
                }

                offspring = kpm_copyNet( net, oData );
                kpm_setPopMember( offspring, *offsprings );
                off_cnt++;
                break;
            } else
                i++;
        }
    }

    return( MODULE_NO_ERROR );
}


char *preferSel_errMsg( int err_code ) {
    switch (err_code) {
    case MODULE_NO_ERROR :
        return ("preferSel : No error found");
    case ERROR_INDEX :
        return ("preferSel : Selected index is bigger then popsize");
    case ERROR_NO_PARENTS :
        return ("preferSel : No population where offsprings can selected from");
    case PAT_MEM_ERROR:
        return ("preferSel : Can't allocate memory for pattern list");
    }
    return ( "preferSel : Unknown error");
}
