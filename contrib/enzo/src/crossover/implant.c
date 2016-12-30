/*
 * File:     (%W%    %G%)
 * Purpose:  definition of implanting crossover routines
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
#include "implant.h"

#define IMPLANT_KEY  "implant"
#define IMPLANT_PROB "implantProb"

float implantProb = 0.2;

int select_feature( NetID donor, NetID net ) {
    int u, p;
    int no_units;
    char *dname;
    FlintType dummy;

    kpm_setCurrentNet( donor );
    no_units = ksh_getNoOfUnits();

    for( u = ksh_getFirstUnit(); u != 0; u = ksh_getNextUnit() ) {
        if( ksh_getUnitTType( u ) != HIDDEN ) continue;

        kpm_setCurrentNet( donor );
        dname = ksh_getUnitName( u );

        kpm_setCurrentNet( net );
        if( ksh_searchUnitName( dname ) > 0 ) continue;

        kpm_setCurrentNet( donor );

        for(p=ksh_getFirstPredUnit(&dummy); p!=0 && ksh_getUnitTType(p)==INPUT; p=ksh_getNextPredUnit(&dummy))  /* search */;
        if( p != 0 ) continue;    /* there ex. an non-input predecessor */

        break;
    }

    return( u );
}

void implant( NetID net, NetID donor, int feature ) {
    char *unit_name, *out_func_name, *act_func_name;
    FlintType  bias, weight;
    struct PosType pos;
    int u, new, source;

    kpm_setCurrentNet( donor );

    unit_name     = ksh_getUnitName( feature );
    out_func_name = ksh_getUnitOutFuncName( feature );
    act_func_name = ksh_getUnitActFuncName( feature );
    bias          = ksh_getUnitBias( feature );
    ksh_getUnitPosition( feature, &pos );

    kpm_setCurrentNet( net );

    new = ksh_createUnit( unit_name, out_func_name, act_func_name, 0.0, bias );
    ksh_setUnitPosition( new, &pos );

    if( new < 0 ) /* error */;

    kpm_setCurrentNet( donor );
    ksh_setCurrentUnit( feature );

    for( u  = ksh_getFirstPredUnit( &weight );
            u != 0;
            u  = ksh_getNextPredUnit( &weight )   ) {
        unit_name = ksh_getUnitName( u );
        kpm_setCurrentNet( net );

        source = ksh_searchUnitName( unit_name );
        ksh_setCurrentUnit( new );

        ksh_createLink( source, weight );

        kpm_setCurrentNet( donor );
    }

    for( u  = ksh_getFirstSuccUnit( feature, &weight );
            u != 0;
            u  = ksh_getNextSuccUnit( &weight )   ) {
        unit_name = ksh_getUnitName( u );
        kpm_setCurrentNet( net );

        source = ksh_searchUnitName( unit_name );
        ksh_setCurrentUnit( source );

        ksh_createLink( new, weight );

        kpm_setCurrentNet( donor );
    }
}

int implant_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
    MODULE_KEY( IMPLANT_KEY );

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

    MSG_CASE( IMPLANT_PROB    ) {
        if( msgc > 1 )
            implantProb = (float) atof( msgv[1] );
    }
    END_MSG;

    return( INIT_USED );
}

int implant_work( PopID *parents, PopID *offsprings, PopID *ref ) {
    NetID net, donor;
    int   feature;

    if( RAND_01 > implantProb ) {
        donor   = kpm_popFirstMember( *parents );
        net     = kpm_popFirstMember( *offsprings );
        feature = select_feature( donor, net );

        if( feature != 0 )  implant( net, donor, feature );
    }

    return( MODULE_NO_ERROR );
}

char *implant_errMsg( int err_code ) {
    /* supply the caller with some information about an error */

    static int   err_cnt   = 2;   /* number of recognized errors */
    static char *err_msg[] = {
        "no error (implant)", "unknown error (implant)",
        "specific error message -- not used"
    };

    return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}
