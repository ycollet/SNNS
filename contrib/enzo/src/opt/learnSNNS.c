/*
 * File:     (%W%    %G%)
 * Purpose:  definition of learning routines for SNNS - networks
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
#include "learnSNNS.h"
#ifdef DEBUG
#include "kernel.h"
#include "kr_mac.h"

#endif

#define LEARN_SNNS_KEY  "learnSNNS"
#define LEARN_FCT       "learnfct"
#define LEARN_PARAMS    "learnparam"
#define MAX_EPOCHS      "maxepochs"
#define MAX_TSS         "maxtss"
#define SHUFFLE         "shuffle"
#define WRITE           "writeLearn"
#define TMP_FILE        "tmpFile"

#define EXTENSION       "learn"

#define ERROR_LEARN     10
#define ERROR_LEARNFUNC 11
#define ERROR_ACTIVATE  13
#define ERROR_PAT       14
#define ERROR_TESTPAT   15

#define MAX_SNNSFCT     30

/*--------------------------------------------------------------variables----*/

static PatID learnPattern                   = NULL;
static PatID testPattern                    = NULL;
static char  learnfct[MAX_SNNSFCT]          = DEFAULT_LEARN_FUNC;
static float learnParam[5]                  = {0.0, 0.0, 0.0, 5.0, 0.0};
static int   maxEpochs                      = 50;
static float maxTss                         = 0.5;
static int   shuffleFlag                    = TRUE;
static int   writeFlag                      = FALSE;
static char  tmpFileName[MAX_FILENAME_LEN]  = "enzo.tmp";

/*--------------------------------------------------------------functions----*/

static int debug = 0;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* int learnSNNS_init( ModuleTableEntry *self, int msgc, char *msgv[] )      */
/*                                                                           */
/*  Reads the incoming string and handles all initial work                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int learnSNNS_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  int i;
  debug = 0;

  MODULE_KEY( LEARN_SNNS_KEY );

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

  MSG_CASE( LEARN_FCT       ) {
    if( msgc > 1 )
      strcpy( learnfct, msgv[1] );
  }

  MSG_CASE( MAX_EPOCHS      ) {
    if( msgc > 1 )
      maxEpochs = atoi( msgv[1] );
  }
  MSG_CASE( MAX_TSS         ) {
    if( msgc > 1 )
      maxTss = (float) atof( msgv[1] );
  }
  MSG_CASE( SHUFFLE         ) {
    if( msgc > 1 )
      shuffleFlag = FLAG_VALUE( msgv[1] );
  }
  MSG_CASE( WRITE           ) {
    if( msgc > 1)
      writeFlag = FLAG_VALUE( msgv[1] );
  }
  MSG_CASE( TMP_FILE        ) {
    if (msgc > 1)
      strcpy( tmpFileName , msgv[1] );
  }
  MSG_CASE( LEARN_PARAMS    ) {
    for( i=0; i< 5 && i<(msgc-1); i++ ) {
      learnParam[i] = atof( msgv[i+1] );
    }
  }
  END_MSG;

  return ( INIT_USED );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  int learnSNNS_work (PopID *reference PopID *parents, PopID *offsprings)  */
/*                                                                           */
/*  This function learns the offspring-subpopulation with an error           */
/*  and max_epochs ended version of a SNNS-learning function.                */
/*                                                                           */
/*  The learnfunction is defined in the global part of the module,           */
/*  the learnparameters too.                                                 */
/*---------------------------------------------------------------------------*/

int learnSNNS_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetworkData *data;

  NetID activeMember;

  int   noLearnParam, no_of_patterns,ret=KRERR_NO_ERROR;
  int   OutParams;
  float *return_values;
  int   epochs;
  float tss, testTss;
  int ham;
  char  fileName[MAX_FILENAME_LEN];
  FILE  *output;

  if (learnPattern == NULL)
    learnPattern = subul_getPatID( PATTERN_LEARN );

  if (testPattern == NULL)
    testPattern = subul_getPatID( PATTERN_TEST );


  /* sets learning patterns */

  if( (kpm_setCurrentPattern( learnPattern )) != KPM_NO_ERROR ) {
    return( ERROR_PAT );
  }

  no_of_patterns = ksh_getNoOfPatterns();

  /* the learning routine */
  /* get the first net of the offspring-population */

  FOR_ALL_OFFSPRINGS( activeMember ) {
    data = ( kpm_getNetData( activeMember ));

    if( kpm_setCurrentNet( activeMember) != KPM_NO_ERROR ) {
      return ( ERROR_ACTIVATE );
    }

    if( ( ksh_setLearnFunc(learnfct)) != KRERR_NO_ERROR ) {
      return ( ERROR_LEARN );
    }

    /* sets learning function and determines no of parameters */

    ksh_getFuncParamInfo( ksh_getLearnFunc(), LEARN_FUNC,
			  &noLearnParam, &OutParams );

    if( (kpm_setCurrentPattern( learnPattern )) != KPM_NO_ERROR ) {
      return( ERROR_PAT );
    }

    ksh_shufflePatterns( shuffleFlag );

    tss = INFINITY;

    /* if demanded, open the tmp-file for the learnoutput */

    for( epochs = 0; ((epochs < maxEpochs) && (tss > maxTss)); epochs++ ) {
      ret = ksh_learnAllPatterns( learnParam, noLearnParam,
				  &return_values, &OutParams );

      if (ret != KRERR_NO_ERROR &&  ret != KRERR_DEAD_UNITS &&  ret != KRERR_NET_DEPTH) {
	return ( ERROR_LEARNFUNC );
      }
      tss = return_values[0]/no_of_patterns;

      if( signal_evolution ) break;  /* --- js, 28.2.94 --- */
    }

    if ( writeFlag )
      fclose( output );                /* -- masch, 22.3.94 -- */

    data->epochs += epochs;
    data->histRec.learnEpochs = epochs;
    data->tss    = tss;
    /* determine the testTss for the testPatterns                  */
    /* afterwards print the whole information in the tmp-file      */

    if( (kpm_setCurrentPattern( testPattern )) == KPM_NO_ERROR ) {
      ksh_get_epoch_error( &testTss, &ham );
      testTss /= ksh_getNoOfPatterns();
      data->histRec.testTss = testTss;
    }
    kpm_setCurrentPattern ( learnPattern );
  }

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  char *learnSNNS_errMsg(int err_code)                                     */
/*                                                                           */
/*  returns the actual error message of the error code.                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *learnSNNS_errMsg(int err_code) {
  static char msg[MAX_ERR_MSG_LEN];

  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return ( "learnSNNS : No Error found" );

  case ERROR_PAT :
    return ("learnSNNS : Can't set the learnpattern via Nepomuk");

  case ERROR_LEARN :
    sprintf(msg,"learnSNNS : Can't activate the SNNS-learnfunction %s",
	    learnfct);
    return ( msg );

  case ERROR_LEARNFUNC :
    sprintf(msg,"learnSNNS : An error occurred during learning  with SNNS-learnfunction %s",
	    learnfct);
    return ( msg );

  case ERROR_ACTIVATE :
    return ("learnSNNS : Can't activate a offspring-net");
  }

  return ( "learnSNNS : Unknown error" );
}
