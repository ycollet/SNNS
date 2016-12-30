/*
 * File:     (%W%    %G%)
 * Purpose:  HiddenInitPop crates random net for the initial population;
 *           the lower bound on the number of hidden units is 0 if not set,
 *           the upper is given by the reference net.
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
#include "hiddenInitPop.h"

/*--------------------------------------------------------------macros-------*/

#define HIDDEN_INIT_POP_KEY       "hiddenInitPop"
#define LOWER_BOUND             "minNoHidden"

#define MAX_DEL_UNITS  200

/*--------------------------------------------------------------variables----*/

static int   lowerBound = 0;

/*--------------------------------------------------------------functions----*/

/*---------------------------------------------------------------------------*/
/* void deleteHiddenUnits (int count)                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/

static void deleteHiddenUnits( int cnt ) {
  int i, j, h, n;
  int delList[MAX_DEL_UNITS];

  /* create a list of all hidden units */
  for( h=0, i = ksh_getFirstUnit(); i != 0; i = ksh_getNextUnit() ) {
    if( ksh_getUnitTType( i ) == HIDDEN )  delList[h++] = i;
  }

  /* shuffle this list */
  for( i = 0; i < h; i++ ) {
    j = (lrand48() % (h-i)) + i;
    n = delList[i];
    delList[i] = delList[j];
    delList[j] = n;
  }

  /* and delete the first count elements */
  ksh_deleteUnitList( cnt, delList );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int hiddenInitPop_init ( ModuleTableEntry *self, int msgc, char *msgv[]    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int hiddenInitPop_init (ModuleTableEntry *self, int msgc, char *msgv[] ) {
  MODULE_KEY( HIDDEN_INIT_POP_KEY );

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

  MSG_CASE( LOWER_BOUND )     {
    if( msgc > 1 )
      lowerBound = atoi( msgv[1] );
  }
  END_MSG;

  return ( INIT_USED );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* int hiddenInitPop_work ( PopID *parents, PopID *offsprings,                */
/*                        PopID *reference)                                 */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int hiddenInitPop_work (PopID *parents, PopID *offsprings, PopID *reference) {
  NetID activeNet;
  int hiddenUnits;
  int maxHiddenUnits;

  FOR_ALL_OFFSPRINGS( activeNet )
  {

    kpm_setCurrentNet( activeNet );
    maxHiddenUnits = ksh_getNoOfTTypeUnits( HIDDEN );
    hiddenUnits = (int) RANDOM( lowerBound, maxHiddenUnits+1 );

    deleteHiddenUnits( maxHiddenUnits - hiddenUnits );

  }

  return (MODULE_NO_ERROR);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* char *hiddenInitPop_errMsg( int err_code )                                  */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

char *hiddenInitPop_errMsg (int err_code) {
  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return ("hiddenInitPop : No error found");
  }
  return ("hiddenInitPop : unknown error");
}
