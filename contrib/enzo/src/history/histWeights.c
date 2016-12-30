/*
 * File:    (%W%    %G%)
 * Purpose: Write some informations about the weights of all nets (parents and
 *    offsprings) in a file.
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
#include "histWeights.h"

/*-----------------------------------------------------------------macros----*/

#define HIST_WEIGHTS_KEY  "histWeights"
#define EXTENSION         "weight"
#define ERROR_FILEOPEN    3

#define OUTELEM_TEXT   "#  No.  Weights    added  deleted   pruned  cleaned  threshold  hUnit  iUnit\n"
#define OUTELEM_FORMAT "  %4d  %7d  %7d  %7d  %7d  %7d  %8.6f  %4d  %4d\n"

/*-----------------------------------------------------------------types-----*/

/*-----------------------------------------------------------------variables-*/

static FILE        *hfpWeight;
static char        histFileName[255] = HISTORY_FILE_VALUE;

/*-----------------------------------------------------------------functions-*/

int histWeights_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  char fileName[MAX_FILENAME_LEN];

  MODULE_KEY( HIST_WEIGHTS_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT ) {
    /* nothing to do */
  }
  MSG_CASE( GENERAL_EXIT ) {
    fclose(hfpWeight);
  }

  MSG_CASE( EVOLUTION_INIT ) {
    sprintf(fileName,"%s.%s",histFileName,EXTENSION);
    if( (hfpWeight = fopen( fileName, "w" )) == NULL )
      return( ERROR_FILEOPEN );
    setlinebuf( hfpWeight );
    fprintf(hfpWeight,OUTELEM_TEXT);
  }


  MSG_CASE( HISTORY_FILE ) {
    if( msgc > 1 ) strcpy ( histFileName, msgv[1] );
  }

  END_MSG;

  return( INIT_USED );
}

/*---------------------------------------------------------------------------*/

int histWeights_work( PopID *parents, PopID *offsprings, PopID *ref ) {
  NetID  net;
  NetworkData *netData;
  int weights;
  int hiddenUnits,inputUnits;   /* masch 4.94 */
  int k;

  FOR_ALL_OFFSPRINGS( net ) {
    netData = (NetworkData *) kpm_getNetData( net );
    hiddenUnits = inputUnits = 0;

    for ( k = ksh_getFirstUnit(); k != 0; k = ksh_getNextUnit()) {
      if((ksh_getUnitTType( k ) == INPUT) && (!subul_deadInputUnit( k )))
	inputUnits++;
      else if (ksh_getUnitTType( k ) == HIDDEN)
	hiddenUnits++;
    }     /* masch 4.94 */

    GET_NO_OF_LINKS( weights );
    fprintf(hfpWeight,OUTELEM_FORMAT,netData->histID,weights,
	    netData->histRec.added,
	    netData->histRec.deleted,
	    netData->histRec.pruned,
	    netData->histRec.cleaned,
	    netData->histRec.threshold,/*masch 5.94*/
	    hiddenUnits,               /*masch 5.94*/
	    inputUnits);               /*masch 5.94*/
  }

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/

char *histWeights_errMsg( int err_code ) {
  switch (err_code) {
  case MODULE_NO_ERROR :
    return ("histFitness : No Error found");
  case ERROR_FILEOPEN :
    return ("histFitness : Can't open history-file for writing");
  }
  return( "histFitness : Unknown error" );
}
