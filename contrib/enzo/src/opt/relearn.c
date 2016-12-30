/*
 * File:     (%W%    %G%)
 * Purpose:  definition of relearning function
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
#include "kernel.h"
#include "kr_mac.h"
#include "relearn.h"

#define RELEARN_KEY     "relearn"
#define RELEARN_FACTOR  "relearnfactor"

#define ERROR           2
#define ERROR_ACTIVATE 10

/*--------------------------------------------------------------variables----*/

static int   result  = 0;         /* no of the error-code -> internal use    */
static float relearn = 1.0;

/*--------------------------------------------------------------functions----*/

/***************************************************************************/
/*                                                                         */
/*  int relearn_init (ModuleTableEntry *self, char *s)                   */
/*                                                                         */
/*  Reads the incoming string and handles all initial work                 */
/*                                                                         */
/***************************************************************************/

int relearn_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  float lrelearn;

  MODULE_KEY( RELEARN_KEY );

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
  MSG_CASE( RELEARN_FACTOR  ) {
    if( msgc > 1 ) {
      lrelearn= (float) atof( msgv[1] );
      if (( lrelearn > 0.0) && (lrelearn <10.0))
	relearn = lrelearn;
    }
  }

  END_MSG;

  return ( INIT_USED );
}

/***************************************************************************/
/*                                                                         */
/*  int relearn_work (PopID *reference PopID *parents, PopID *offsprings)*/
/*                                                                         */
/*  The relearn-function multiplies all weights (the biases to) with a     */
/*  factor.                                                                */
/*                                                                         */
/***************************************************************************/

int relearn_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetID activeMember;
  struct Unit *activeUnit;
  struct Link *activeLink;

  /* using a Nepomuk-macro do step over all offspring-nets            */

  FOR_ALL_OFFSPRINGS ( activeMember )

  {
    if (kpm_setCurrentNet( activeMember ) != KPM_NO_ERROR) {
      result = ERROR_ACTIVATE;
      return ( ERROR );
    }

    /* Fast version of the relearnfunction                          */
    /* uses special macros and internal data representation of SNNS */

    FOR_ALL_UNITS( activeUnit ) {
      activeUnit->bias *= relearn;
      FOR_ALL_LINKS ( activeUnit, activeLink ) {
	activeLink->weight *= relearn;
      }
    }

    /* slower version for this problem
       using the kernel-interface of SNNS

       for( t = ksh_getFirstUnit(); t != 0; t = ksh_getNextUnit () )
       {
       ksh_setCurrentUnit( t );
       weight = ksh_getUnitBias( t );
       ksh_setUnitBias ( t , weight * relearn );

       for( s = ksh_getFirstPredUnit(&weight);
       s != 0;
       s = ksh_getNextPredUnit(&weight))
       {
       ksh_setLinkWeight ( weight * relearn );
       }
       }

    */
  }

  return( MODULE_NO_ERROR );
}

/***************************************************************************/
/*                                                                         */
/*  char *relearn_errMsg(int err_code)                                   */
/*                                                                         */
/*  returns the actual error message of the error code.                    */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

char *relearn_errMsg(int err_code) {
  switch (result) {
  case 0 :
    return ("relearn : No error found");

  case ERROR_ACTIVATE :
    return ("relearn : Can't activate a offspring-net");
  }

  return ("relearn : Unknown error, please consulte your systemadmin");
}
