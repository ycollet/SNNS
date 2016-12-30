/*
 * File:     (%W%    %G%)
 * Purpose:  definition of topology evaluation functions
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
#include "topoEval.h"

#define TOPO_EVAL_KEY  "topoEval"
#define PP_W            "ppW"
#define PP_U            "ppU"
#define PP_I            "ppI"

#define ERROR_NETDATA    2

/* ------------------------------------------------------ local variables --- */
/*                                                                            */

static float    ppw  = 0.0, ppu = 0.0, ppi = 0.0;

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- toploRating ---------------------------------------------------------- */
/*                                                                            */

int topoEval_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( TOPO_EVAL_KEY );

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

  MSG_CASE( PP_W ) {
    if( msgc > 1 )  ppw = (float) atof( msgv[1] );
  }
  MSG_CASE( PP_U ) {
    if( msgc > 1 )  ppu = (float) atof( msgv[1] );
  }
  MSG_CASE( PP_I ) {
    if( msgc > 1 )  ppi = (float) atof( msgv[1] );
  }
  END_MSG;

  return ( INIT_USED );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- topoEval_work -------------------------------------------------------- */
/*                                                                            */
/*                                                                            */

int topoEval_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetworkData *data;
  NetID activeMember;
  int i, input, deadInput, noOfLinks;

  FOR_ALL_OFFSPRINGS (activeMember) {
    if( (data = kpm_getNetData( activeMember )) == NULL ) {
      return( ERROR_NETDATA );
    }

    input = 0;
    deadInput = 0;

    for( i = ksh_getFirstUnit(); i != 0; i = ksh_getNextUnit() ) {
      if( ksh_getUnitTType( i ) == INPUT ) {
	input++;
	if( subul_deadInputUnit( i ) ) deadInput++;
      }
    }

    GET_NO_OF_LINKS( noOfLinks );

    data->fitness +=  ppu*ksh_getNoOfUnits() + ppw*noOfLinks;

    if( input > 0 ) data->fitness += ppi*(input - deadInput);
  }

  return( MODULE_NO_ERROR );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

char *topoEval_errMsg( int err_code ) {
  return( "no err handling (topoEval)" );
}
