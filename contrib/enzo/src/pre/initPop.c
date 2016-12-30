/*
 * File:     (%W%    %G%)
 * Purpose:  initPop loads the reference-net and copies it to the parents.
 *           Afterwards all parents will be initalized and learned with the
 *           SNNS-functions.
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
#include "initPop.h"

/*--------------------------------------------------------------macros-------*/

#define INIT_POP_KEY         "initPop"
#define NETWORK              "network"
#define INIT_FUNCTION        "initFct"
#define INIT_PARAMS          "initParam"

#define ERROR_NOT_LOAD_REF   10
#define ERROR_NOT_COPY_NET   11
#define ERROR_INIT_FCT       12
#define ERROR_CANT_INIT_NET  13
#define ERROR_NOT_NET        14

/*--------------------------------------------------------------variables----*/

static char fileName[MAX_FILENAME_LEN] = "enzo.net";
static int population_size             = POP_SIZE_VALUE;
static char  initFct[128]  = DEFAULT_INIT_FUNC;
static float initParams[5] = {-1.0, 1.0, 0.0, 0.0, 0.0};
static int   no_initParams = 4;

/*--------------------------------------------------------------functions----*/

/*--------------------------------------------------------------------------*/
/*  int initPop_init ( ModuleTableEntry *self, char *message)               */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int initPop_init (ModuleTableEntry *self, int msgc, char *msgv[] ) {
  int i;

  MODULE_KEY( INIT_POP_KEY );

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

  MSG_CASE( POP_SIZE        ) {
    if( msgc > 1 )
      population_size = atoi( msgv[1] );
  }
  MSG_CASE( NETWORK         ) {
    if( msgc > 1 )
      strcpy( fileName, msgv[1] );
  }

  MSG_CASE( INIT_FUNCTION   ) {
    if( msgc > 1 )
      strcpy( initFct, msgv[1] );
  }


  MSG_CASE( INIT_PARAMS    ) {
    for( i=0; i<5 && i<(msgc-1); i++ ) {
      initParams[i] = atof( msgv[i+1] );
    }
  }
  END_MSG;

  return ( INIT_USED );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* int initPop_work ( PopID *parents, PopID *offsprings,P opID *reference)  */
/*                                                                          */
/*                                                                          */
/* Initailization of the whole parent-subpopulation.                        */
/* First load the reference net (SNNS - format) with a Nepomuk-function and */
/* then copy the network to each member of the parent-population. After     */
/* this init each parentnet with the specified SNNS-initfunction.           */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int initPop_work (PopID *parents, PopID *offsprings, PopID *reference) {
  NetID refNet, activeNet;
  NetworkData *data;
  int i,dummy;
  char unitname[15];

  /* starting with the reference net, the only member of its subpopulation */
  /* Set the net active for SNNS via Nepomuk and then load the net         */

  /* first load refNet; need not to be equal to the net to start with */
  data = utils_getNewNetData();
  if( (refNet = kpm_loadNet( fileName, data )) == NULL ) {
    return ( ERROR_NOT_LOAD_REF );
  }
  kpm_setPopMember ( refNet, *reference);

  /* Setting for all existing units the unitno. as the unitname       */
  /* with a leading "a"                                               */
  /* This identification is needed everythere, you are searching for  */
  /* corrosponding units in different neural networks                 */

  for ( i = ksh_getFirstUnit(); i != 0; i = ksh_getNextUnit()) {
    sprintf(unitname,"a%d",i);
    ksh_setUnitName(i, unitname);
  }

  /* Copying the information from the reference-net to all members of  */
  /* the offspring-subpopulation                                       */

  for ( i = 0; i < population_size; i++) {
    data = utils_getNewNetData ();
    if( (activeNet = kpm_copyNet( refNet , data )) == NULL) {
      return ( ERROR_NOT_COPY_NET );
    }

    kpm_setPopMember( activeNet, *offsprings );
    kpm_setCurrentNet( activeNet );

    /* the clean version of initialize a net */

    if (ksh_set_init_function(initFct,&no_initParams) != KRERR_NO_ERROR)
      return ( ERROR_INIT_FCT );

    if( ksh_initializeNet(initParams,no_initParams) != KRERR_NO_ERROR )
      return ( ERROR_CANT_INIT_NET );
    ksh_set_init_function(EMPTY_INIT_FUNC,&dummy);
  }

  return (MODULE_NO_ERROR);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* char *initPop_errMsg( int err_code )                                     */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

char *initPop_errMsg (int err_code) {
  static char error[MAX_ERR_MSG_LEN];
  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return ("initPop: No error found");

  case ERROR_NOT_COPY_NET :
    return ("initPop: Can't copy a net to the parent population via Nepomuk");

  case ERROR_NOT_LOAD_REF :
    sprintf(error,"initPop: Can't load reference-net: %s via Nepomuk",
	    fileName);
    return( error );

  case   ERROR_INIT_FCT :
    sprintf(error,"initPop: Can't activate the initfunction %s",initFct);
    return ( error );

  case ERROR_CANT_INIT_NET :
    return ("initPop: Can't initialize net with SNNS");

  }

  return ("initPop: unknown error");
}
