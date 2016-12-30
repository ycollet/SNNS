/*
 * File:     (%W%    %G%)
 * Purpose:  Survival of the fittest; offspings are inserted according to their
 *           fittness, then the least fittest are deleted.
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
#include "fittest.h"


#define FITTEST_KEY  "fittestSurvive"
#define POP_SIZE     "popsize"



int popSize;


int fittest_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
    MODULE_KEY( FITTEST_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) {
        /* nothing to do */
    }
    MSG_CASE( GENERAL_EXIT    ) {
        /* nothing to do */
    }
    MSG_CASE( EVOLUTION_INIT  ) {
        /* nothing to do */
    }

    MSG_CASE( POP_SIZE        ) {
        if( msgc > 1 )
            popSize = atoi( msgv[1] );
    }

    END_MSG;

    return( INIT_USED );
}


int fittest_work( PopID *parents, PopID *offsprings, PopID *ref ) {
    NetID net, delnet;
    int cnt;
    NetworkData *data;

    FOR_ALL_OFFSPRINGS( net )  	kpm_setPopMember( net, *parents );

    kpm_sortNets( subul_netcmp );  /* sort the nets in nepomuk */

    /* FOR_ALL_PARENTS( net ) */
    cnt = 0;
    net = kpm_popFirstMember( *parents );
    while( net != NULL ) {
        if( ++cnt > popSize ) {
            delnet = net;
            net = kpm_popNextMember( *parents, net );
            data = GET_NET_DATA( delnet );
            free( data->selectedPattern );
            free( data );
            data = NULL;
            kpm_deleteNet( delnet );
        } else net = kpm_popNextMember( *parents, net );
    }

    return( MODULE_NO_ERROR );
}


char *fittest_errMsg( int err_code ) {
    /* supply the caller with some information about an error */

    static int   err_cnt   = 2;   /* number of recognized errors */
    static char *err_msg[] = {
        "no error (fittest)", "unknown error (fittest)",
        "specific error message -- not used"
    };

    return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}
