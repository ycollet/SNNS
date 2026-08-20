/*
 * File:     (%W%    %G%)
 * Purpose:  definition of pruning functions
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
#include "prune.h"

#define PRUNE_KEY     "prune"
#define PRUNE_THRESH  "threshold"
#define PRUNE_START   "thresholdStart"
#define PRUNE_END     "pruneEndGen"

static float thresh       = 0.0;
static float threshStart  = 0.0;
static int   threshEnd     = 0;
static int   epochCnt      = 0;

int prune_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
    MODULE_KEY( PRUNE_KEY );

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

    MSG_CASE( PRUNE_THRESH   ) {
        if( msgc > 1 ) thresh = atof( msgv[1] );
    }
    MSG_CASE( PRUNE_START    ) {
        if( msgc > 1 ) threshStart = atof( msgv[1] );
    }
    MSG_CASE( PRUNE_END      ) {
        if( msgc > 1 ) threshEnd = atoi( msgv[1] );
    }

    END_MSG;

    return( INIT_USED );
}

int prune_work( PopID *parents, PopID *offsprings, PopID *ref ) {
    int       t, s;
    FlintType weight;
    NetID net;
    NetworkData *netData;
    int pruned;
    float th;
    int  *predbuf;
    int   npred, k;

    if( epochCnt < threshEnd )
        th = threshStart + (thresh - threshStart) * (epochCnt/ (float) threshEnd);
    else
        th = thresh;
    epochCnt++;

    FOR_ALL_OFFSPRINGS(net) {
        pruned  = 0;
        netData = GET_NET_DATA( net );

        netData->histRec.firstEpochs += netData->histRec.learnEpochs;

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
                    pruned++;
                }
            }
        }

        free( predbuf );

        netData->histRec.pruned    = pruned;
        netData->histRec.threshold = th;
    }
    return( MODULE_NO_ERROR );
}

char *prune_errMsg( int err_code ) {
    return( "NO ERR MESS AVAILABLE" );
}
