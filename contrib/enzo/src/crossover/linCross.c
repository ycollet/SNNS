/*
 * File:      (%W%    %G%)
 * Purpose:  definition of linear  crossover routines
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
#include "linCross.h"

/*--------------------------------------------------------------macros-------*/

#define LIN_CROSS_KEY  "linCross"
#define PROBCROSS      "probCross"

#define ERROR_ACTIVATE 10
#define ERROR_NO_REF   11

/*--------------------------------------------------------------variables----*/

static float     probCross = 0.5;  /* prob. for inserting an existing link   */

static NetDescr  refDesc;          /* Pointer to the description of the      */
                                   /* reference net, not the net itself      */

/*--------------------------------------------------------------functions----*/

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int linCross_init (ModuleTableEntry *self,  int msgc, char *msgv[] )    */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int linCross_init( ModuleTableEntry *self, int msgc, char *msgv[] ) {
  NetID refNet;
  PopID *reference;

  MODULE_KEY( LIN_CROSS_KEY );

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

  MSG_CASE( PROBCROSS         ) {
    if( msgc > 1 )
      probCross= atof( msgv[1] );
  }

  END_MSG;

  return ( INIT_USED );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  int linCross_work (PopID *reference PopID *parents, PopID *offsprings)  */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

int linCross_work( PopID *parents, PopID *offsprings, PopID *reference ) {
  NetworkData *data;
  NetID activeMember;
  float weight1, weight2;
  int i, in1, in2;
  int source1, source2, target1, target2;
  char *uname1,*uname2;

  /* using a Nepomuk-macro to step over all offspring-nets            */

  FOR_ALL_OFFSPRINGS ( activeMember )

  {
    if (kpm_setCurrentNet( activeMember ) != KPM_NO_ERROR) {
      return ( ERROR_ACTIVATE );
    }

    data = kpm_getNetData ( activeMember );

    /* Just testing all links from the reference-net                */

    for (i = 0; i < refDesc.no_of_links; i++) {

      if ((refDesc.units[ refDesc.weights[i].source-1 ].TType == INPUT) &&
	  (refDesc.units[ refDesc.weights[i].target-1 ].TType == OUTPUT)) {
	in1 = in2 = 0;
	uname1 = refDesc.units[ refDesc.weights[i].source - 1 ].name;
	uname2 = refDesc.units[ refDesc.weights[i].target - 1 ].name;

	/* Testing Connection in parent1                          */

	source1 = ksh_searchUnitName(uname1);
	target1 = ksh_searchUnitName(uname2);

	if ( ksh_areConnectedWeight ( source1, target1, &weight1 ) )
	  in1 = 1;

	/* Testing Connection in in parent2                        */

	kpm_setCurrentNet( data->parent2 );

	source2 = ksh_searchUnitName(uname1);
	target2 = ksh_searchUnitName(uname2);

	if ( ksh_areConnectedWeight ( source2, target2, &weight2 ) )
	  in2 = 1;

	kpm_setCurrentNet( activeMember );

	if (in1 && in2) {
	  ksh_areConnectedWeight ( source1, target1 , &weight1 );
	  ksh_setLinkWeight ( (weight1+weight2) * 0.5);
	}

	else if (in1 && !in2 && (RAND_01 > probCross)) {
	  ksh_areConnectedWeight ( source1, target1 , &weight1 );
	  ksh_deleteLink ();
	}

	else if (!in1 && in2 && (RAND_01 < probCross)) {
	  ksh_setCurrentUnit( target1 );
	  ksh_createLink( source1, weight2);
	}

      } /* endif */

    } /* endfor ALL LINKS */

  } /* endfor FOR ALL OFFSPRINGS */

  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  char *linCross_errMsg(int err_code)                                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *linCross_errMsg(int err_code) {
  switch ( err_code ) {
  case MODULE_NO_ERROR :
    return("linCross : No Error found");

  case ERROR_ACTIVATE :
    return ("linCross : Can't activate a offspring-net");

  case ERROR_NO_REF :
    return ("linCross : Can't activate the reference-net");
  }

  return ("linCross : Unknown error!");
}
