/*
 * File:     (%W%    %G%)
 * Purpose:  This function loads pattern from file.
 *           These patterns are needed in several functions like learning and
 *           testing networks.
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
#include "loadSNNSPat.h"

/*--------------------------------------------------------------macros-------*/

#define LOAD_SNNS_KEY     "loadSNNSPat"
#define LEARN_PATTERN     "learnpattern"
#define TEST_PATTERN      "testpattern"
#define CROSS_PATTERN     "crosspattern"

#define ERROR_LOAD_LEARN  11
#define ERROR_LOAD_TEST   12
#define ERROR_LOAD_CROSS  13

/*--------------------------------------------------------------variables----*/

static char  patternLearn[MAX_FILENAME_LEN]  = "";
static char  patternTest[MAX_FILENAME_LEN]   = "";
static char  patternCross[MAX_FILENAME_LEN]  = "";

/*--------------------------------------------------------------functions----*/

/*---------------------------------------------------------------------------*/
/* int loadSNNSPat_init( ModuleTableEntry *self, int msgc, char *msgv[] )    */
/*                                                                           */
/*  Reads the incoming string and handles all initial work                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int loadSNNSPat_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( LOAD_SNNS_KEY );

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

  MSG_CASE( LEARN_PATTERN   ) {
    if( msgc > 1 )
      strcpy( patternLearn,msgv[1] );
  }
  MSG_CASE( TEST_PATTERN    ) {
    if( msgc > 1 )
      strcpy( patternTest,msgv[1] );
  }
  MSG_CASE( CROSS_PATTERN   ) {
    if( msgc > 1 )
      strcpy( patternCross,msgv[1] );
  }
  END_MSG;

  return ( INIT_USED );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  int loadSNNSPat_work (PopID *reference PopID *parents, PopID *offsprings)*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

int loadSNNSPat_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  PatID patLearn = NULL;
  PatID patTest  = NULL;
  PatID patCross = NULL;
  PatID actPat;

  /* just testing if the pattern already exist                              */

  for ( actPat = kpm_getFirstPat(); actPat != NULL;
	actPat = kpm_getNextPat( actPat )) {
    if (!strcmp(kpm_getPatName( actPat ),PATTERN_LEARN))
      patLearn = actPat;

    if (!strcmp(kpm_getPatName( actPat ),PATTERN_TEST))
      patTest = actPat;

    if (!strcmp(kpm_getPatName( actPat ),PATTERN_CROSS))
      patCross = actPat;
  }

  /* load the patterns                                                      */

  if ((patLearn == NULL) && (strlen(patternLearn) > 0)) {
    if ((patLearn = kpm_loadPat ( patternLearn, NULL ) ) == NULL)
      return ( ERROR_LOAD_LEARN );
    kpm_setPatName ( patLearn, PATTERN_LEARN );
  }

  if ((patTest == NULL) && (strlen(patternTest) > 0)) {
    if ((patTest = kpm_loadPat ( patternTest, NULL ) ) == NULL)
      return ( ERROR_LOAD_TEST );
    kpm_setPatName ( patTest, PATTERN_TEST );
  }

  if ((patCross == NULL) && (strlen(patternCross) > 0)) {
    if ((patCross = kpm_loadPat ( patternCross, NULL ) ) == NULL)
      return ( ERROR_LOAD_CROSS );
    kpm_setPatName ( patCross, PATTERN_CROSS );
  }

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  char *loadSNNSPat_errMsg(int err_code)                                   */
/*                                                                           */
/*  returns the actual error message of the error code.                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *loadSNNSPat_errMsg(int err_code) {
  static char msg[MAX_ERR_MSG_LEN]; /* TR: 13.08.95:  if pointer is returned, should be static ! */

  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return ( "loadSNNSPat : No Error found" );

  case ERROR_LOAD_LEARN :
    sprintf(msg,"loadSNNSPat : Can't open the patternfile %s via Nepomuk",
	    patternLearn);
    return( msg );

  case ERROR_LOAD_TEST :
    sprintf(msg,"loadSNNSPat : Can't open the patternfile %s via Nepomuk",
	    patternTest);
    return( msg );

  case ERROR_LOAD_CROSS :
    sprintf(msg,"loadSNNSPat : Can't open the patternfile %s via Nepomuk",
	    patternCross);
    return( msg );
  }

  return ( "loadSNNSPat : Unknown error" );
}
