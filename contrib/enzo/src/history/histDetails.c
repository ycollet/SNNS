/*
 * File:    (%W%    %G%)
 * Purpose: histDetails writes the Details-values of all networks in two files
 *          The first file contains the Details of all tested networks according
 *          to their generation. The second file contains the information about
 *          the best, worst and average Details of the parent-population.
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

# TR:  This module is not finished ! DO NOT USE.

#include "enzo.h"
#include "histDetails.h"

/*------------------------------------------------------------------macros---*/

#define HIST_Details_KEY  "histDetails"

#define ERROR_MEM         2
#define ERROR_FILEOPEN    3

#define OUTELEM_TEXT   "#  No.          Details  \n"
#define OUTELEM_FORMAT "%4d    %14.6f \n"

#define OUTPOP_TEXT    "#  No.   best Details     worst Details   \
average Details\n"
#define OUTPOP_FORMAT  "%4d  %14.6f    %14.6f    %14.6f\n"

/*-----------------------------------------------------------------variables-*/

static FILE        *hfpFit, *hfpPopfit;
static char        histFileName[MAX_FILENAME_LEN] = HISTORY_FILE_VALUE;

/*-----------------------------------------------------------------functions-*/

int histDetails_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  char fileName[MAX_FILENAME_LEN];

  MODULE_KEY( HIST_Details_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT )   {
    /* nothing to do */
  }
  MSG_CASE( EVOLUTION_INIT ) {
    sprintf(fileName,"%s.fit",histFileName);
    if( (hfpFit = fopen( fileName, "w" )) == NULL )
      return( ERROR_FILEOPEN );
    setlinebuf( hfpFit );
    fprintf( hfpFit, OUTELEM_TEXT );

    sprintf(fileName,"%s.popfit",histFileName);
    if( (hfpPopfit = fopen( fileName, "w" )) == NULL )
      return( ERROR_FILEOPEN );
    setlinebuf( hfpPopfit );
    fprintf( hfpPopfit, OUTPOP_TEXT );
  }

  MSG_CASE( GENERAL_EXIT ) {
    if( hfpFit )     fclose( hfpFit );
    if( hfpPopfit )  fclose( hfpPopfit );
  }

  MSG_CASE( HISTORY_FILE    ) {
    if( msgc > 1 ) strcpy ( histFileName,msgv[1] );
  }

  END_MSG;

  return( INIT_USED );
}

/*---------------------------------------------------------------------------*/

int histDetails_work( PopID *parents, PopID *offsprings, PopID *ref ) {
  NetID  net;
  NetworkData *netData;
  int pars = 0;

  static int   genCnt     = 0;          /* count no of generations */
  float aveDetails = 0.0,
    maxDetails = -INFINITY,
    minDetails =  INFINITY;


  FOR_ALL_PARENTS( net ) {
    pars ++;
    netData = GET_NET_DATA( net );

    aveDetails += netData->Details;
    if( netData->Details > maxDetails ) {
      maxDetails = netData->Details;
    }
    if( netData->Details < minDetails ) {
      minDetails = netData->Details;
    }
  }

  if( pars ) {
    aveDetails /= pars;

    fprintf( hfpPopfit, OUTPOP_FORMAT,
	     genCnt, minDetails, maxDetails, aveDetails );
  }

  FOR_ALL_OFFSPRINGS( net ) {
    netData = GET_NET_DATA( net );
    fprintf( hfpFit, OUTELEM_FORMAT, netData->histID, netData->Details );
  }

  genCnt++;

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/

char *histDetails_errMsg( int err_code ) {
  switch (err_code) {
  case MODULE_NO_ERROR :
    return ("histDetails : No Error found");
  case ERROR_FILEOPEN :
    return ("histDetails : Can't open history-file for writing");
  case ERROR_MEM :
    return ("histDetails : Memory excess");
  }
  return( "histDetails : Unknown error" );
}
