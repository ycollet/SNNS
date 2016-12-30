/*
 * File:     (%W%    %G%)
 * Purpose:  Xhist opens an Xwindow on a X11 - Desktop and writes the actual
 *           fitness result in it (best , average and worst).
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
#include "Xgraf/graf.h"
#include "Xhist.h"

/*------------------------------------------------------------- macros ------*/

#define X_HIST_KEY        "Xhist"
#define GEOMETRY          "Xgeometry"
#define COORD             "Xcoord"

#define WIN_X_POS 10
#define WIN_Y_POS 10
#define WIN_X_LEN 600
#define WIN_Y_LEN 300

/* ------------------------------------------------------------ variables --- */

static int   win_x_pos = WIN_X_POS,
  win_y_pos = WIN_Y_POS,
  win_x_len = WIN_X_LEN,
  win_y_len = WIN_Y_LEN;

static float xll = 0.0,   yll = 0.0,               /* lower left corner  */
  xur = (float) MAX_GENERATIONS_VALUE,  /* upper right corner */
  yur = 1000.0;

/* ------------------------------------------------------------ functions --- */

int Xhist_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( X_HIST_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT   ) {
    /* nothing to do */
  }
  MSG_CASE( GENERAL_EXIT   ) {
    if( MODULE_ACTIVE(self) )  end_graf();
  }
  MSG_CASE( EVOLUTION_INIT ) {
    if( start_graf( "x-history",
		    win_x_len, win_y_len, win_x_pos, win_y_pos ) == NULL )
      /* warning: can't open Xhist-window  */;
    setcoord_graf( xll, xur, yll, yur );
    axis_graf( "generations", "fitness" );
  }

  MSG_CASE( GEOMETRY ) {
    if( msgc > 4 ) {
      win_x_pos = atoi( msgv[1] );
      win_y_pos = atoi( msgv[2] );
      win_x_len = atoi( msgv[3] );
      win_y_len = atoi( msgv[4] );
    }
  }
  MSG_CASE( COORD ) {
    if( msgc > 4 ) {
      xll = (float) atof( msgv[1] );
      yll = (float) atof( msgv[2] );
      xur = (float) atof( msgv[3] );
      yur = (float) atof( msgv[4] );
    }
  }

  END_MSG;

  return( INIT_USED );
}

/*---------------------------------------------------------------------------*/

int Xhist_work( PopID *parents, PopID *offsprings, PopID *ref ) {
  NetID  net;
  NetworkData *netData;

  char text[64];
  int  pars = 0, weights, dummy;

  float        aveFitness =  0.0,
    minFitness =  INFINITY,
    maxFitness = -INFINITY;
  static float lastAveFitness = 0.0,
    lastMinFitness = 0.0,
    lastMaxFitness = 0.0;
  static int gen = 0;

  if( signal_evolution )  end_graf();  /* neccessary to avoid `broken pipe' */

  kpm_popFirstMember( *parents );
  ksh_getNetInfo( &dummy, &weights, &dummy, &dummy );

  FOR_ALL_PARENTS( net ) {
    pars ++;
    netData = kpm_getNetData( net );

    aveFitness += netData->fitness;

    if( netData->fitness > maxFitness ) {
      maxFitness = netData->fitness;
    }

    if( netData->fitness < minFitness ) {
      minFitness = netData->fitness;
    }
  }

  if( pars > 0 )  {
    aveFitness /= pars;
  }

  if( lastAveFitness != 0.0 && !signal_evolution ) {
    line_graf( (float) gen-1, lastAveFitness, (float) gen, aveFitness );
    line_graf( (float) gen-1, lastMinFitness, (float) gen, minFitness );
    line_graf( (float) gen-1, lastMaxFitness, (float) gen, maxFitness );
  }

  gen++;  /* count number of generations */

  lastAveFitness = aveFitness;
  lastMinFitness = minFitness;
  lastMaxFitness = maxFitness;

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/

char *Xhist_errMsg( int err_code ) {
  static int   err_cnt   = 2;   /* number of recognized errors */
  static char *err_msg[] = {
    "no error (Xhist)", "unknown error (Xhist)",
    "specific error message -- not used"
  };

  return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}
