/*
 * File:     (%W%    %G%)
 * Purpose:   loadPop loads a whole population of networks from file
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
#include "loadPop.h"

/*----------------------------------------------------------macros-----------*/

#define LOAD_POP_KEY  "loadPop"
#define NET_NAME "network"

#define ERROR_LOAD 10

/*----------------------------------------------------------variables--------*/

static int popsize                     = POP_SIZE_VALUE;
static char fileName[MAX_FILENAME_LEN] = "enzo";
static char filename[MAX_FILENAME_LEN] = "";

/*----------------------------------------------------------functions--------*/

/*---------------------------------------------------------------------------*/
/* int loadPop_init( ModuleTableEntry *self, int msgc, char *msgv[] )        */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int loadPop_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( LOAD_POP_KEY );

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

  MSG_CASE( NET_NAME   )    {
    if( msgc > 1 )
      strcpy( fileName , msgv[1] );
  }
  MSG_CASE( POP_SIZE )      {
    if ( msgc > 1)
      popsize = atoi( msgv[1] );
  }
  END_MSG;

  return( INIT_USED );
}

/*---------------------------------------------------------------------------*/
/*  int loadPop_work( PopID *parents, PopID *offsprings, PopID *reference )  */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int loadPop_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  int i;
  NetworkData *data;
  NetID net;

  /* First step is to load the reference-net        */

  data = utils_getNewNetData();
  sprintf( filename, "%s_ref.net", fileName);
  if ((net = kpm_loadNet( filename, data)) == NULL) {
    return ( ERROR_LOAD );
  }
  kpm_setPopMember( net, *reference );

  /* Afterwards try to load populationsize nets for the parent-population */

  for ( i = 0; i < popsize; i++) {
    data = utils_getNewNetData();
    sprintf( filename, "%s_%04d.net", fileName,i);
    if ((net = kpm_loadNet( filename, data)) == NULL) {
      if( i > 0 ) break;
      return ( ERROR_LOAD );
    }
    kpm_setPopMember ( net, *offsprings);
  }

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
/*  char *loadPop_errMsg( int err_code )                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *loadPop_errMsg( int err_code ) {
  static char msg[MAX_ERR_MSG_LEN];
  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return ("loadPop : No error found");

  case ERROR_LOAD :
    sprintf(msg,"loadPop : Can't load the net %s",filename);
    return ( msg );
  }

  return ("loadPop : Unknown error");
}
