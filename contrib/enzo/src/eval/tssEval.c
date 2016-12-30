/*
 * File:     (%W%    %G%)
 * Purpose:  definition of tss evaluation routines
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
#include "tssEval.h"

#define TSS_EVAL_KEY  "tssEval"
#define TSS_RATE      "crossTssRating"
#define HAM_RATE      "crossHamRating"
#define HAM_THRESH    "crossHamThresh"
#define RND_SUBSET    "rndSubset"

#define ERR_NO_CROSS_PAT 2

static float crossTSS  = 0.0;
static float crossHam  = 0.0;
static float hamThresh = 0.0;
static PatID crossPats = NULL;
static int rndSubset = 0;

int tssEval_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( TSS_EVAL_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) {
    /* nothing to do */
  }
  MSG_CASE( GENERAL_EXIT    ) {
    /* nothing to do */
  }
  MSG_CASE( EVOLUTION_INIT  ) {
    /* First check if the crosspatterns are loaded   */
    if( crossPats == NULL )
      crossPats =  subul_getPatID( PATTERN_CROSS );
  }

  MSG_CASE( TSS_RATE        ) {
    if( msgc > 1 )
      crossTSS = (float) atof( msgv[1] );
  }
  MSG_CASE( HAM_RATE        ) {
    if( msgc > 1 )
      crossHam = (float) atof( msgv[1] );
  }
  MSG_CASE( HAM_THRESH      ) {
    if( msgc > 1 )
      hamThresh = (float) atof( msgv[1] );
  }
  MSG_CASE( RND_SUBSET      ) {
    if( msgc > 1 )
      rndSubset = atoi( msgv[1] );
  }
  END_MSG;

  return( INIT_USED );
}

static int get_error( float *tss, int *ham, int cnt ) {
  int i, pham;
  float pss;

  if( cnt == 0 ) {
    ksh_get_epoch_error( tss, ham );
    return( ksh_no_patterns() );
  } else {
    ksh_shufflePatterns( TRUE );
    *tss = *ham = 0;
    for( i = 1; i <= cnt; i++ ) {
      ksh_get_pattern_error( i, &pss, &pham );
      *tss += pss;
      *ham += pham;
    }
    return( cnt );
  }
}

int tssEval_work( PopID *parents, PopID *offsprings, PopID *ref ) {
  NetID net;
  NetworkData *data;
  float tss;
  int ham, no_of_patterns;

  if( kpm_setCurrentPattern( crossPats ) != KPM_NO_ERROR ) {
    return( ERR_NO_CROSS_PAT );
  }
  ksh_readNetinfo();
  ksh_error_bound(hamThresh);

  for( net  = kpm_popFirstMember( *offsprings ); net != NULL; net  = kpm_popNextMember(*offsprings, net) )
    /*     FOR_ALL_OFFSPRINGS( net )*/
    {
      data = GET_NET_DATA( net );

      no_of_patterns = get_error( &tss, &ham, rndSubset );

      tss /= no_of_patterns;

      data->fitness += crossTSS * tss + crossHam * ham;

      data->histRec.testMiss    = ham;
      data->histRec.testHit     = no_of_patterns-ham;
      data->histRec.testFitness = ham/(float) no_of_patterns;
      data->histRec.testTss     = tss;
      data->histRec.testHam     = ham;
    }

  return( MODULE_NO_ERROR );
}

char *tssEval_errMsg( int err_code ) {
  static int   err_cnt   = 3;   /* number of recognized errors */
  static char *err_msg[] = {
    "no error (tssEval)", "unknown error (tssEval)",
    "no crossvalidation patterns present (tssEval)"
  };

  return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}
