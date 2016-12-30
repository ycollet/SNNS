/*
 * File:     (%W%    %G%)
 * Purpose:  These funcions write a simple history of all networks in a file
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
#include "histSimple.h"

/* -------------------------------------------------------------- macros --- */

#define HIST_SIMPLE_KEY  "histSimple"
#define HIST_FILE        "historyFile"

#define FILE_EXTENSION   "simple"

#define HIST_MEM_ERR      2
#define HISTFILE_OPEN_ERR 3

#define OUT_TEXT							\
  " No : (first) | parent1 | fitness | learn epochs | relearn epochs |\
 tss | testTss | testHam | noUnits | noWeights \n"
#define OUT_FORMAT							\
  "%4d: (%3d)\t| %4d\t| %f\t| %d\t| %d\t| %f\t| %f\t |%4d | %3d | %4d  \n"

/* ------------------------------------------------------------ variables ---*/

static char *histfile;
static FILE *hfp;

/*---------------------------------------------------------------------------*/

int histSimple_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  char filename[MAX_FILENAME_LEN];

  MODULE_KEY( HIST_SIMPLE_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT   ) {
    /* nothing to do */
  }
  MSG_CASE( GENERAL_EXIT   ) {
    if( hfp )  fclose( hfp );
  }

  MSG_CASE( EVOLUTION_INIT ) {
    sprintf( filename, "%s.%s", histfile, FILE_EXTENSION);
    if( (hfp = fopen( filename, "w" )) == NULL )
      return( HISTFILE_OPEN_ERR );
    setlinebuf( hfp );
    fprintf( hfp, OUT_TEXT );
  }

  MSG_CASE( HISTORY_FILE )    {
    if( msgc > 1 ) histfile = strdup( msgv[1] );
  }

  END_MSG;

  return( INIT_USED );
}

/*---------------------------------------------------------------------------*/

int histSimple_work( PopID *parents, PopID *offsprings, PopID *ref ) {
  static int generationCnt = 0;

  NetID  net;
  NetworkData *netData;
  int dummy, weights;

  generationCnt++;

  FOR_ALL_OFFSPRINGS( net ) {
    netData = GET_NET_DATA( net );

    ksh_getNetInfo( &dummy, &weights, &dummy, &dummy );
    fprintf( hfp, OUT_FORMAT , netData->histID,
	     generationCnt,
	     netData->histRec.parent1,
	     netData->fitness,
	     netData->histRec.firstEpochs,
	     netData->histRec.learnEpochs,
	     netData->tss,
	     netData->histRec.testTss,
	     netData->histRec.testHam,
	     ksh_getNoOfUnits(),
	     weights                        );
  }

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/

char *histSimple_errMsg( int err_code ) {
  static int err_cnt = 4;
  static char *err_msg[] = {
    "no error (history)", "unknown error (history)",
    "memory excess in module history", "can't open history-file"
  };

  return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}
