/*
 * File:     (%W%    %G%)
 * Purpose:  definition of learn rating evaluation functions
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
#include "learnRating.h"

#define LEARNRATING_KEY "learnRating"
#define NOLEARN_RATING  "noLearnRating"
#define EPOCH_RATING    "epochRating"
#define TSS_RATING      "tssRating"
#define MAX_TSS         "maxtss"

#define ERROR_NETDATA     13

/*--------------------------------------------------------------variables----*/

static float    nolearnRating    = 200.0;
static float    epochRating      =   0.0;
static float    tssRating        =   0.0;
static float    maxTss           =   0.5;

/*--------------------------------------------------------------functions----*/

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* int learnRating_init( ModuleTableEntry *self,int msgc,char *msgv[] )  */
/*                                                                          */
/*  Reads the incoming string and handles all initial work                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int learnRating_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( LEARNRATING_KEY );

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

  MSG_CASE( NOLEARN_RATING ) {
    if( msgc > 1 )
      nolearnRating = (float) atof( msgv[1] );
  }
  MSG_CASE( EPOCH_RATING   ) {
    if( msgc > 1)
      epochRating = (float) atof( msgv[1] );
  }
  MSG_CASE( TSS_RATING     ) {
    if( msgc > 1)
      tssRating = (float) atof( msgv[1] );
  }

  MSG_CASE( MAX_TSS        ) {
    if( msgc > 1)
      maxTss = (float) atof( msgv[1] );
  }
  END_MSG;

  return ( INIT_USED );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int learnRating_work (PopID *reference PopID *parents,                  */
/*                           PopID *offsprings)                             */
/*                                                                          */
/*  This function rates the learning-qualities of the offspring-generation. */
/*  Possible ratings are : Can the net learn the problem                    */
/*                         How much epochs were needed                      */
/*                         What was the tss.                                */
/*                                                                          */
/*  Each rating is scaled by a special factor.                              */
/*  The value will be added to the total fitness-value.                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int learnRating_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetID        activeMember;
  NetworkData  *ndata;

  FOR_ALL_OFFSPRINGS ( activeMember ) {
    if ((ndata = kpm_getNetData( activeMember )) == NULL) {
      return ( ERROR_NETDATA);
    }

    if (ndata->tss > maxTss)
      ndata->fitness += nolearnRating;

    ndata->fitness += epochRating * ndata->epochs
      + tssRating   * ndata->tss;
  }

  return( MODULE_NO_ERROR );
}

/***************************************************************************/
/*                                                                         */
/*  char *learnRating_errMsg(int err_code)                                   */
/*                                                                         */
/*  returns the actual error message of the error code.                    */
/*                                                                         */
/*                                                                         */
/***************************************************************************/

char *learnRating_errMsg(int err_code) {

  switch (err_code) {
  case MODULE_NO_ERROR :
    return ( "learnRating : No Error found" );

  case ERROR_NETDATA :
    return ("learnRating : Can't get the data of an offspring-net");

  }

  return ( "learnRating : Unknown error" );

}
