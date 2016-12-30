/*
 * File:     mutInputs.c
 * Purpose:  mutInputs adds or delete whole Inputs and their links
 *           of the offspring networks.
 *           Only one unit is added or deleted each time.
 *           These units can  not be removed from the network, because the
 *           input pattern structure can not be changed.
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
#include "mutInputs.h"

/*--------------------------------------------------------------macros-------*/

#define MUT_Inputs_KEY "mutInputs"
#define MUTUNIT_PROB  "probMutInputs"
#define MUTSPLIT      "probMutInputsSplit"
#define RANGE          "initRange"

/*--------------------------------------------------------------structures---*/

/*--------------------------------------------------------------variables----*/

static NetID         refNet;
static NetDescr      refDesc;     /* Pointer to the description of the       */
/* reference net, not the net itself       */

static NetDescr      activeDesc;
static float         prob      = 0.5;
static float         split     = 0.5;
static float         range     = 0.5;
/*--------------------------------------------------------------functions----*/

/*---------- getIndexAlive --------------------------------------------------*/
/*                                                                           */
/* Determines the number of existing Inputs in the current net.              */
/* All existing Inputs are filled in the indexRec                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

static int getIndexAlive() {
  int k;
  int count = 0;
  int index = -1;

  for ( k = ksh_getFirstUnit(); k != 0; k = ksh_getNextUnit()) {
    if ((ksh_getUnitTType( k ) == INPUT) &&
	!subul_deadInputUnit ( k ))
      count++;
  }

  if (count) {
    count = RANDOM(0,count);
    for (k = ksh_getFirstUnit(); k!=0; k = ksh_getNextUnit())
      if ((ksh_getUnitTType( k ) == INPUT) &&
	  !subul_deadInputUnit ( k )) {
	if (!count)
	  index = k;
	count--;
      }
  }

  return ( index );
}

/*---------- getDeletedUnitIndex --------------------------------------------*/
/*                                                                           */
/* Determines the number of a deletes unit in the current net                */
/* the no. describes the entry of the Netdescr. of the reference-net-unit    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

static int getDeletedUnitIndex () {
  int k;
  int count = 0;
  int index = -1;

  for ( k = ksh_getFirstUnit(); k != 0; k = ksh_getNextUnit()) {
    if ((ksh_getUnitTType( k ) == INPUT) &&
	subul_deadInputUnit ( k ))
      count++;
  }

  if (count) {
    count = RANDOM(0,count);
    for (k = ksh_getFirstUnit(); k!=0; k = ksh_getNextUnit())
      if ((ksh_getUnitTType( k ) == INPUT) &&
	  subul_deadInputUnit ( k )) {
	if (!count)
	  index = k;
	count--;
      }
  }

  return ( index );
}

/*---------- createAllNewLinks ----------------------------------------------*/
/*                                                                           */
/* creates all links to and from an added Unit which are defined in the      */
/* reference network                                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

static int createAllNewLinks ( int unit_no ) {
  int i,unit2, add = 0;
  char *uname;
  for (i = 0; i < refDesc.no_of_links; i++) {
    if (refDesc.weights[i].source == unit_no) {
      uname = refDesc.units[refDesc.weights[i].target -1].name;
      unit2 = ksh_searchUnitName(uname);
      if (!unit2)
	continue;
      ksh_setCurrentUnit ( unit2 );
      ksh_createLink( unit_no, RANDOM ( -range , range));
      add++;
    }
  } /*endfor all links */
  return ( add );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int mutInputs_init (ModuleTableEntry *self,  int msgc, char *msgv[] )    */
/*                                                                          */
/*  Reads the incoming string and handles all initial / exit work           */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int mutInputs_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  float lprob,lsplit;
  PopID *reference;

  MODULE_KEY( MUT_Inputs_KEY );

  SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) {
    /* nothing to do */
  }
  MSG_CASE( GENERAL_EXIT    ) {
    /* nothing to do */
  }
  MSG_CASE( EVOLUTION_INIT  ) {
    reference = (PopID *) msgv[1];
    refNet = kpm_popFirstMember ( *reference );
    kpm_getNetDescr ( refNet, &refDesc );
  }

  MSG_CASE( RANGE           ) {
    if( msgc > 1)
      range = atof ( msgv[1] );
  }

  MSG_CASE ( MUTSPLIT )      {
    if (msgc > 1) {
      lsplit = (float) atof( msgv[1] );
      if ((lsplit >= 0.0) && (lsplit <= 1.0))
	split = lsplit;
    }
  }

  MSG_CASE ( MUTUNIT_PROB )  {
    if (msgc > 1) {
      lprob = (float) atof( msgv[1] );
      if ((lprob >= 0.0) && (lprob <= 1.0))
	prob = lprob;
    }
  }

  END_MSG;

  return ( INIT_USED );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int mutInputs_work (PopID *reference PopID *parents, PopID *offsprings)  */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int mutInputs_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetworkData *data;
  NetID activeMember;
  int unit_no, add;


  /* using a Nepomuk-macro to step over all offspring-nets            */

  FOR_ALL_OFFSPRINGS ( activeMember )

  {

    data = kpm_getNetData ( activeMember );
    kpm_getNetDescr (activeMember, &activeDesc );

    /* First test if a mutation takes place                         */

    if (RAND_01 > prob) {
      kpm_freeNetDescr ( &activeDesc );
      continue;
    }

    if ( RAND_01 > split ) {
      /* Add a unit                                               */
      unit_no = getDeletedUnitIndex ();

      if (unit_no == -1) {
	kpm_freeNetDescr ( &activeDesc );
	continue;
      }
      /* check if the added unit is an inputunit (exist) or not    */

      add = createAllNewLinks ( unit_no );

    } /* endif add unit */

    else {
      unit_no = getIndexAlive();
      if (unit_no == -1) {
	kpm_freeNetDescr ( &activeDesc );
	continue;
      }
      ksh_setCurrentUnit( unit_no );
      ksh_deleteAllOutputLinks();
    }

    kpm_freeNetDescr ( &activeDesc );

  } /* endfor FOR ALL OFFSPRINGS */

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  char *simpleMut_errMsg(int err_code)                                     */
/*                                                                           */
/*  returns the actual error message of the error code.                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *mutInputs_errMsg(int err_code) {
  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return("mutInputs : No Error found");

  }

  return ("mutInputs : Unknown error!");
}
