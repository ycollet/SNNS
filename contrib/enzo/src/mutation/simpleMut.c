/*
 * File:      simpleMut.c
 * Purpose:   mutate the weights of all offsprings
 *             - existing weights can be deleted
 *             - deleted weights between living units can be added
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
#include "simpleMut.h"

/*--------------------------------------------------------------macros-------*/

#define SIMPLE_MUT_KEY "simpleMut"
#define PROBDEL        "probdel"
#define PROBADD        "probadd"
#define RANGE          "initRange"

#define ERROR_ACTIVATE 10
#define ERROR_NO_REF   11

/*--------------------------------------------------------------variables----*/

static float     probDel   = 0.0;  /* prob. for deleting an existing link     */
static float     probAdd   = 0.0;  /* prob. for adding an absend link         */
static NetDescr  refDesc;          /* Pointer to the description of the       */
/* reference net, not the net itself       */
static float     range     = 0.5;

/*--------------------------------------------------------------functions----*/

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int simpleMut_init (ModuleTableEntry *self,  int msgc, char *msgv[] )   */
/*                                                                          */
/*  Reads the incoming string and handles all initial / exit work           */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int simpleMut_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  float lprobDel;
  float lprobAdd;
  NetID refNet;
  PopID *reference;

  MODULE_KEY( SIMPLE_MUT_KEY );

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
    if (refNet == NULL)
      return ( ERROR_NO_REF );
    kpm_getNetDescr ( refNet, &refDesc );
  }

  MSG_CASE( PROBDEL         ) {
    if( msgc > 1 ) {
      lprobDel= atof( msgv[1] );
      if (( lprobDel > 0.0) && (lprobDel < 1.0))
	probDel = lprobDel;
    }
  }
  MSG_CASE( PROBADD         ) {
    if( msgc > 1 ) {
      lprobAdd= atof( msgv[1] );
      if (( lprobAdd > 0.0) && (lprobAdd < 1.0))
	probAdd = lprobAdd;
    }
  }
  MSG_CASE( RANGE           ) {
    if( msgc > 1)
      range = atof ( msgv[1] );
  }
  END_MSG;

  return ( INIT_USED );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int simpleMut_work (PopID *reference PopID *parents, PopID *offsprings) */
/*                                                                          */
/*  The simpleMut-function mutates all offspring-nets. If the link from     */
/*  the reference-net exists in the offspring , it will be delete if        */
/*  rnd() < probDel.                                                        */
/*  If the link from the reference-net is absend, when the link will be     */
/*  added then both units are active and rnd() < probAdd.                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int simpleMut_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetworkData *data;
  NetID activeMember;
  float weight;
  int i;
  int del,add;
  int unit1,unit2;
  char *uname1,*uname2;

  /* using a Nepomuk-macro to step over all offspring-nets            */

  FOR_ALL_OFFSPRINGS ( activeMember )

  {
    if (kpm_setCurrentNet( activeMember ) != KPM_NO_ERROR) {
      return ( ERROR_ACTIVATE );
    }

    add = del = 0;
    data = kpm_getNetData ( activeMember );

    /* Just testing all links from the reference-net                */

    for (i = 0; i < refDesc.no_of_links; i++) {
      /* Search via the unitname the corrosponding units in the    */
      /* offspring-net                                             */
      /* Be careful : Unitno. don't match Untino. in links !!      */

      uname1 = refDesc.units[ refDesc.weights[i].source - 1 ].name;
      uname2 = refDesc.units[ refDesc.weights[i].target - 1 ].name;

      unit1 = ksh_searchUnitName(uname1);
      unit2 = ksh_searchUnitName(uname2);

      /* If one of the two units doesn't exist then nothing can    */
      /* happen to the connection.                                 */

      if ((!unit1) || (!unit2))
	continue;

      /* Testing if the exists in the offspring-net                */


      if ( ksh_areConnectedWeight ( unit1, unit2, &weight ) ) {
	/* the link exists between the two units, now test if the */
	/* link will be delete                                    */

	if (RAND_01 < probDel ) {
	  ksh_deleteLink ();
	  del++;
	} /* endif probDel */

      } /* endif areConnected */

      else

	{
	  /* the link between the units is absend, so try to insert the */
	  /* link if RAND_01 < probAdd and the units are in use.        */

	  if (( RAND_01 < probAdd ) &&
	      (!subul_deadInputUnit (unit1)) &&
	      (!subul_deadInputUnit (unit2))) {
	    ksh_setCurrentUnit( unit2 );
	    ksh_createLink( unit1, RANDOM ( -range , range ));
	    add++;
	  } /* endif RAND_01 < probADD */

	} /* end else ksh_areConnected */

    } /* endfor no_of_links */

    data->histRec.added   += add;
    data->histRec.deleted += del;

  } /* endfor FOR ALL OFFSPRINGS */

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  char *simpleMut_errMsg(int err_code)                                     */
/*                                                                           */
/*  returns the actual error message of the error code.                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *simpleMut_errMsg(int err_code) {
  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return("simpleMut : No Error found");

  case ERROR_ACTIVATE :
    return ("simpleMut : Can't activate a offspring-net");

  case ERROR_NO_REF :
    return ("simpleMut : Can't activate the reference-net");
  }

  return ("simpleMut : Unknown error!");
}
