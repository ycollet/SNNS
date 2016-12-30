/*
 * File:     (%W%    %G%)
 * Purpose:   Write some informations about the Inputs of all nets (parents and
 *            offsprings) in a file.
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
#include "histInputs.h"

/*-----------------------------------------------------------------macros----*/

#define HIST_Inputs_KEY  "histInputs"

#define EXTENSION         "inputs"

#define ERROR_FILEOPEN    3

#define OUTELEM_TEXT   "#  No.  Inputarray\n"
#define OUTELEM_FORMAT "  %4d   "

/*-----------------------------------------------------------------types-----*/

/*-----------------------------------------------------------------variables-*/

static FILE        *hfpWeight;
static char        histFileName[255] = HISTORY_FILE_VALUE;

/*-----------------------------------------------------------------functions-*/

int histInputs_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  char fileName[MAX_FILENAME_LEN];

  MODULE_KEY( HIST_Inputs_KEY );

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

int histInputs_work( PopID *parents, PopID *offsprings, PopID *ref ) {
  NetID  net;
  NetworkData *netData;
  int k;

  FOR_ALL_OFFSPRINGS( net ) {
    netData = (NetworkData *) kpm_getNetData( net );

    fprintf(hfpWeight,OUTELEM_FORMAT,netData->histID);

    for ( k = ksh_getFirstUnit(); k != 0; k = ksh_getNextUnit()) {
      if(ksh_getUnitTType( k ) == INPUT) {
	if ( subul_deadInputUnit( k ) )
	  fprintf(hfpWeight," D ");
	else
	  fprintf(hfpWeight," X ");
      }
    }

    fprintf(hfpWeight,"\n");
  }

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/

char *histInputs_errMsg( int err_code ) {
  switch (err_code) {
  case MODULE_NO_ERROR :
    return ("histInput : No Error found");
  case ERROR_FILEOPEN :
    return ("histInput : Can't open history-file for writing");
  }
  return( "histFitness : Unknown error" );
}
