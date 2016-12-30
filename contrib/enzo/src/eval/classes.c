/*
 * File:     (%W%    %G%)
 * Purpose:  definition  of classification routines
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
#include "classes.h"

#define CLASSES_KEY     "classes"
#define HIGH_VALUE      "highDec"
#define LOW_VALUE       "lowDec"
#define HIT_VALUE       "hitRating"
#define MISS_VALUE      "missRating"
#define NONE_VALUE      "noneRating"
#define DISTANCE        "decisionThreshold"

#define ERROR_ACTIVATE_PAT  10
#define ERROR_ACTIVATE_NET  11
#define ERROR_NO_DATA       12

#define MISS  1
#define HIT   2
#define NONE  3

#define POS_CORRECT (ksh_netout(i) > average+dist && ksh_target(i) > average)
#define NEG_CORRECT (ksh_netout(i) < average-dist && ksh_target(i) < average)
#define NO_DECISION (ksh_netout(i) < average+dist && ksh_netout(i) > average-dist)

/*-------------------------------------------------------- variables ------- */

static float highDec    = 1.0;
static float lowDec     = -1.0;          /* this means check for the maximum */
static float hitRating  = 0.0;
static float missRating = 1.0;
static float noneRating = 1.0;
static float dist   = 0.2;
static PatID crossPats   = NULL;

/*-------------------------------------------------------------- functions ---*/

int classes_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( CLASSES_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) {
    /* nothing to do */
  }
  MSG_CASE( GENERAL_EXIT    ) {
    /* nothing to do */
  }
  MSG_CASE( EVOLUTION_INIT  ) {
    /* First check whether the crosspatterns are loaded   */
    if (crossPats == NULL)
      crossPats = subul_getPatID( PATTERN_CROSS );
  }

  MSG_CASE( HIGH_VALUE ) {
    if( msgc > 1 ) highDec = atof(msgv[1]);
  }
  MSG_CASE( LOW_VALUE )  {
    if( msgc > 1 ) lowDec = atof(msgv[1]);
  }
  MSG_CASE( HIT_VALUE )  {
    if (msgc > 1)  hitRating = (float) atof(msgv[1]);
  }
  MSG_CASE( MISS_VALUE ) {
    if (msgc > 1) missRating = (float) atof(msgv[1]);
  }
  MSG_CASE( NONE_VALUE ) {
    if (msgc > 1) noneRating = (float) atof(msgv[1]);
  }
  MSG_CASE( DISTANCE )   {
    if (msgc > 1) dist = fabs( atof (msgv[1]));
  }

  END_MSG;

  return( INIT_USED );
}


int classes_work ( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetID activeMember;
  NetworkData *data;
  float average;
  int i, hit, miss, none, noPattern, result;

  /* activate the cross-patterns */
  if( kpm_setCurrentPattern( crossPats ) != KPM_NO_ERROR ) {
    return (ERROR_ACTIVATE_PAT);
  }

  ksh_readNetinfo();

  average = (highDec + lowDec) / 2.0;

  FOR_ALL_OFFSPRINGS( activeMember ) {
    hit = miss = none = 0;

    if( (data = GET_NET_DATA( activeMember )) == NULL ) {
      return(ERROR_NO_DATA);
    }

    for( noPattern = 1; noPattern <= ksh_no_patterns(); noPattern++) {
      ksh_propagate_pattern(  noPattern);
      ksh_get_target_pattern( noPattern);

      result = NONE;

      for( i = 0; i < ksh_no_outputs(); i++ ) {
	if     ( POS_CORRECT || NEG_CORRECT ) {
	  result = HIT;
	} else if( NO_DECISION                ) {
	  result = NONE;
	  break;
	} else		                      {
	  result = MISS;
	  break;
	}
      }

      switch( result ) {
      case HIT:
	hit++;
	break;
      case NONE:
	none++;
	break;
      case MISS:
	miss++;
	break;
      }
    }

    data->fitness += hit*hitRating + miss*missRating + none*noneRating;
    data->histRec.testHit  = hit;
    data->histRec.testNone = none;
    data->histRec.testMiss = miss;

  } /* endfor ALL_OFFSPRINGS */

  return( MODULE_NO_ERROR );
}


char *classes_errMsg( int err_code ) {
  switch ( err_code ) {

  case MODULE_NO_ERROR :
    return ("classes : No error found");

  case ERROR_ACTIVATE_PAT :
    return("classes : Can't activate crossvalidation pattern");

  case ERROR_ACTIVATE_NET :
    return ("classes : Can't activate an offspringnet");

  case ERROR_NO_DATA :
    return ("classes : Can't get the networkdata");

  }

  return("classes : Unknown error");
}
