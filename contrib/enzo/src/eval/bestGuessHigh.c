/*
 * File:     (%W%    %G%)
 * Purpose:  definition of best guess evaluation functions
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
#include "bestGuessHigh.h"

/*---------------------------------------------------------  macros ---------*/

#define BEST_GUESS_KEY  "bestGuessHigh"
#define HIT_VALUE       "hitRating"
#define MISS_VALUE      "missRating"
#define NONE_VALUE      "noneRating"
#define DIFF_DESC       "hitThreshold"
#define DIFF_DIST       "hitDistance"

#define ERROR_ACTIVATE_PAT  10
#define ERROR_ACTIVATE_NET  11
#define ERROR_NO_DATA       12

/*-------------------------------------------------------- variables ------- */

static float hitRating  =  0.0;
static float missRating = 10.0;
static float noneRating = 10.0;
static float threshold  =  0.3;
static float distance   =  0.2;
static PatID crossPats   = NULL;

/*--------------------------------------------------------- functions -------*/

int bestGuessHigh_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( BEST_GUESS_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT   ) {
    /* nothing to do */
  }
  MSG_CASE( GENERAL_EXIT   ) {
    /* nothing to do */
  }
  MSG_CASE( EVOLUTION_INIT ) {
    /* First check whether the crosspatterns are loaded   */
    if (crossPats == NULL)
      crossPats =  subul_getPatID( PATTERN_CROSS );
  }

  MSG_CASE( HIT_VALUE  ) {
    if( msgc > 1 ) hitRating = (float) atof(msgv[1]);
  }
  MSG_CASE( MISS_VALUE ) {
    if( msgc > 1 ) missRating = (float) atof(msgv[1]);
  }
  MSG_CASE( NONE_VALUE ) {
    if( msgc > 1 ) noneRating = (float) atof(msgv[1]);
  }
  MSG_CASE( DIFF_DESC  ) {
    if( msgc > 1 ) threshold  = fabs( atof(msgv[1]) );
  }
  MSG_CASE( DIFF_DIST  ) {
    if( msgc > 1 ) distance   = fabs( atof(msgv[1]) );
  }

  END_MSG;

  return( INIT_USED );
}

int bestGuessHigh_work ( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetID activeMember;
  NetworkData *data;
  int noPattern, noOutputs, out, target;
  float maxtarget, maxout;
  int hit, miss, none, noDesc;
  int no_pattern;

  /* activate the cross-patterns */
  if (kpm_setCurrentPattern( crossPats ) != KPM_NO_ERROR) {
    return (ERROR_ACTIVATE_PAT);
  }

  ksh_readNetinfo();                  /* kernel-shell-function to determine */
  /* all needed values from SNNS        */

  no_pattern = ksh_getNoOfPatterns();

  FOR_ALL_OFFSPRINGS( activeMember ) {
    hit = miss = none = noDesc = 0;

    if ((data = GET_NET_DATA( activeMember )) == NULL) {
      return (ERROR_NO_DATA);
    }

    for ( noPattern = 0; noPattern < no_pattern; noPattern++) {
      maxout     = -INFINITY;
      maxtarget  = -INFINITY;

      ksh_propagate_pattern  ( noPattern + 1 );
      ksh_get_target_pattern ( noPattern + 1 );

      for (noOutputs = 0; noOutputs < ksh_no_outputs(); noOutputs ++) {
	if (ksh_netout(noOutputs) > (maxout+distance)) {
	  maxout = ksh_netout(noOutputs);
	  out    = noOutputs;
	  noDesc = 0;
	} else if (ksh_netout(noOutputs) > maxout) {
	  maxout = ksh_netout(noOutputs);
	  out    = noOutputs;
	  noDesc = 1;
	} else if (ksh_netout(noOutputs) > (maxout - distance)) {
	  noDesc = 1;
	}

	if (ksh_target(noOutputs) > maxtarget) {
	  maxtarget = ksh_target(noOutputs);
	  target    = noOutputs;
	}
      }

      /* After finding the outputs, now check if the answer was  */
      /* right, false or none and change the fitness             */

      if ((maxout  < threshold ) || (noDesc)) {
	none++;
      } else if ( target != out ) {
	miss++;
      } else {
	hit++;
      }
    } /* endfor allpattern */

    data->fitness         += hit * hitRating + miss * missRating +
      none * noneRating;
    data->histRec.testHit     = hit;
    data->histRec.testMiss    = miss;
    data->histRec.testNone    = none;
    data->histRec.testFitness = 100.0 * hit / (hit + miss + none);

  } /* endfor ALL_OFFSPRINGS */

  return( MODULE_NO_ERROR );
}

char *bestGuessHigh_errMsg( int err_code ) {
  switch ( err_code) {

  case MODULE_NO_ERROR :
    return ("bestGuess : No error found");

  case ERROR_ACTIVATE_PAT :
    return("bestGuess : Can't activate crossvalidation pattern");

  case ERROR_ACTIVATE_NET :
    return ("bestGuess : Can't activate an offspringnet");

  case ERROR_NO_DATA :
    return ("bestGuess : Can't get the networkdata");

  }

  return("bestGuess : Unknown error");
}
