/*
 * File:     (%W%    %G%)
 * Purpose:  definition of weight init routines
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
#include "weightInit.h"

/*--------------------------------------------------------------macros-------*/

#define WEIGHT_INIT_KEY      "weightInit"
#define INIT_PROB            "weightProb"

/*--------------------------------------------------------------variables----*/

static float weightProb      = 1.0;

/*--------------------------------------------------------------functions----*/

/*--------------------------------------------------------------------------*/
/*  int weightInit_init ( ModuleTableEntry *self, char *message)               */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int weightInit_init (ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( WEIGHT_INIT_KEY );

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

  MSG_CASE( INIT_PROB      ) {
    if( msgc > 1 )
      weightProb = fabs( atof ( msgv[1] ));
  }

  END_MSG;

  return ( INIT_USED );
}

/*--------------------------------------------------------------------------*/

int weightInit_work (PopID *parents, PopID *offsprings, PopID *reference) {
  NetID  activeNet;
  int s, t;

  FOR_ALL_OFFSPRINGS( activeNet ) {
    for( s = ksh_getFirstUnit(); s != 0; s = ksh_getNextUnit() ) {
      for( t = ksh_getNextUnit(); t != 0; t = ksh_getNextUnit() ) {
	if( ksh_isConnected( s ) && weightProb < RAND_01 ) {
	  ksh_deleteLink();
	}
      }

      /* getNextUnit will get the succ of the current Unit */
      ksh_setCurrentUnit( s );
    }
  }

  return (MODULE_NO_ERROR);
}

/*--------------------------------------------------------------------------*/

char *weightInit_errMsg (int err_code) {
  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return ("weightInit: No error found");

  }

  return ("weightInit: unknown error");
}
