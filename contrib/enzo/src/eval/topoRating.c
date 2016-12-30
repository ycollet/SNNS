/*
 * File:     (%W%    %G%)
 * Purpose:  definition of topology (rating) evaluation functions
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

#include "topoRating.h"

#define LEARN_SNNS_KEY  "topologyRating"
#define WEIGHT_RATING   "weightRating"
#define UNIT_RATING     "unitRating"
#define INPUT_RATING    "inputRating"

#define ERROR_NO_REFNET   10
#define ERROR_NO_REFDATA  11   /* no longer used, js */
#define ERROR_DESCR       12   /* no longer used, js */
#define ERROR_NETDATA     13
#define ERROR_ILLEGALREF  14

/* ------------------------------------------------------ local variables --- */
/*                                                                            */

static float    weightrating     = 0.0;
static float    unitrating       = 0.0;
static float    inputrating      = 0.0;

static int      refInputUnits, refNoOfUnits, refNoOfLinks;

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------- interface functions --- */
/*                                                                            */

/* --- toplogyRating -------------------------------------------------------- */
/*                                                                            */
/*  Reads the incoming string and handles all initial work                    */
/*                                                                            */

int topologyRating_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( LEARN_SNNS_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT   ) {
    /* nothing to do */
  }
  MSG_CASE( GENERAL_EXIT   ) {
    /* nothing to do */
  }
  MSG_CASE( EVOLUTION_INIT ) {
    if( kpm_popFirstMember( *REFERENCE ) == NULL )
      return( ERROR_NO_REFNET );

    refInputUnits = ksh_getNoOfTTypeUnits( INPUT );
    refNoOfUnits  = ksh_getNoOfUnits();
    GET_NO_OF_LINKS( refNoOfLinks );
    if( refNoOfLinks == 0 || refNoOfUnits == 0 || refInputUnits == 0 ) {
      return ( ERROR_ILLEGALREF );
    }
  }

  MSG_CASE( WEIGHT_RATING ) {
    if( msgc > 1 )
      weightrating = (float) atof( msgv[1] );
  }
  MSG_CASE( UNIT_RATING   ) {
    if( msgc > 1)
      unitrating = (float) atof( msgv[1] );
  }
  MSG_CASE( INPUT_RATING  ) {
    if( msgc > 1)
      inputrating = (float) atof( msgv[1] );
  }
  END_MSG;

  return ( INIT_USED );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- topologyRating_work -------------------------------------------------- */
/*                                                                            */
/*  This function rates the topology of the offspring-generation by           */
/*  comparing it to the topology of the reference-net                         */
/*  Each rating is scaled by a special factor.                                */
/*  The value will be added to the total fitness-value.                       */
/*                                                                            */

int topologyRating_work( PopID *parents, PopID *offsprings, PopID *reference ) {
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

    if( input > 0 ) data->fitness += inputrating*(input- deadInput)/input;

    GET_NO_OF_LINKS( noOfLinks );
    data->fitness +=    unitrating   * ksh_getNoOfUnits() / refNoOfUnits
      + weightrating * noOfLinks           / refNoOfLinks;
  }

  return( MODULE_NO_ERROR );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- topologyRating_errMsg ------------------------------------------------ */
/*                                                                            */
/*  returns the actual error message of the error code.                       */
/*                                                                            */
/*                                                                            */

char *topologyRating_errMsg(int err_code) {
  switch( err_code ) {
  case MODULE_NO_ERROR:
    return( "topoRating: No Error found" );
  case ERROR_NO_REFNET:
    return( "topoRating: Can't get reference net" );
  case ERROR_NETDATA:
    return( "topoRating: Can't get offspring data" );
  case ERROR_ILLEGALREF:
    return( "topoRating: Illegal reference net: no links or no units");
  }

  return ( "topologyRating : Unknown error" );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */
