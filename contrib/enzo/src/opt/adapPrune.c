/*
 * File:     (%W%    %G%)
 * Purpose:  definition of adaptive pruning functions
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
#include "adapPrune.h"

#define ADAPPRUNE_KEY     "adapPrune"
#define ADAPPRUNE_THRESH  "threshold"
#define ADAPPRUNE_DELTA   "deltaThreshold"

static float thresh       = 0.0;
static float deltaThresh  = 0.2;

int adapPrune_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
    MODULE_KEY( ADAPPRUNE_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT   ) {
        /* nothing to do */
    }
    MSG_CASE( GENERAL_EXIT   ) {
        /* nothing to do */
    }
    MSG_CASE( EVOLUTION_INIT ) {
        /* nothing to do */
    }

    MSG_CASE( ADAPPRUNE_THRESH   ) {
        if( msgc > 1 ) thresh = atof( msgv[1] );
    }
    MSG_CASE( ADAPPRUNE_DELTA    ) {
        if( msgc > 1 ) deltaThresh=atof( msgv[1] );
    }

    END_MSG;

    return( INIT_USED );
}

int adapPrune_work( PopID *parents, PopID *offsprings, PopID *ref ) {
    int       t, s;
    FlintType weight;
    NetID net;
    NetworkData *netData;
    int adapPruned;
    float th, change;
    int  *predbuf;
    int   npred, k;

    FOR_ALL_OFFSPRINGS(net) {
        adapPruned  = 0;
        netData = GET_NET_DATA( net );

        netData->histRec.firstEpochs += netData->histRec.learnEpochs;
        if (netData->parent1 == 0)
            th = thresh;
        else {
            change = (RAND_01 - 0.5) * 2.0 * deltaThresh;
            th = thresh * (1.0 + change);
        }
        /* For every unit t, only walk its actual input (predecessor) links   */
        /* instead of testing all unit pairs (O(units+links) vs O(units^2)).  */
        /* The small-weight predecessors are collected first, then deleted,   */
        /* since deleting a link while iterating the predecessor list would   */
        /* invalidate the kernel's internal link iterator.                    */

        predbuf = (int *) malloc( sizeof(int) * (ksh_getNoOfUnits() + 1) );

        for( t = ksh_getFirstUnit(); t != 0; t = ksh_getNextUnit() ) {
            ksh_setCurrentUnit( t );
            npred = 0;
            for( s = ksh_getFirstPredUnit( &weight ); s != 0;
                    s = ksh_getNextPredUnit( &weight ) ) {
                if( (weight < 0 ? -weight : weight) < th )
                    predbuf[ npred++ ] = s;   /* source of link s->t */
            }
            for( k = 0; k < npred; k++ ) {
                if( ksh_isConnected( predbuf[k] ) ) { /* sets current link */
                    ksh_deleteLink();
                    adapPruned++;
                }
            }
        }

        free( predbuf );

        netData->histRec.pruned     = adapPruned;
        netData->histRec.threshold  = th;
    }
    return( MODULE_NO_ERROR );
}

char *adapPrune_errMsg( int err_code ) {
    return( "NO ERR MESS AVAILABLE" );
}
