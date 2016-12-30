/*
 * File:     (%W%    %G%)
 * Purpose:  Definition of several useful Macros
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

#ifndef _SUBUTILS_H
#define _SUBUTILS_H

#include <math.h>

/* -------------------------------------------------------------------------- */

#define MAX_GENERATIONS  "maxGenerations"
#define OFF_SIZE         "gensize"
#define POP_SIZE         "popsize"
#define HISTORY_FILE     "historyFile"

#define MAX_GENERATIONS_VALUE  30
#define OFF_SIZE_VALUE         10
#define POP_SIZE_VALUE         30
#define HISTORY_FILE_VALUE     "enzo.hst"

#define DEFAULT_ACT_FUNC   "Act_Logistic"
#define DEFAULT_OUT_FUNC   "Out_Identity"
#define DEFAULT_LEARN_FUNC "Rprop"
#define DEFAULT_UPDATE_FUNC "ENZO_prop"
#define DEFAULT_INIT_FUNC  "Randomize_Weights"
#define EMPTY_INIT_FUNC  "ENZO_noinit"

/* -------------------------------------------------------------------------- */

#define MAX_FILENAME_LEN 128   /* length for filenames                        */
#define MAX_ERR_MSG_LEN  256   /* length for err messages                     */

#define GET_NET_DATA(id)      (NetworkData *) kpm_getNetData( id )

#define GET_NO_OF_LINKS(n)  {int d; ksh_getNetInfo( &d, &n, &d, &d );}

#define REFERENCE  ((PopID *) msgv[1])  /* reference net in EVOLUTION_INIT    */

/* -------------------------------------------------------------------------- */

/* drand48() return non-negative double-precision floating-point              */
/* values uniformly distributed over the interval [0.0, 1.0).                 */

#define RAND_01          ((float) drand48())
#define RANDOM(min,max)  (RAND_01*(max-min) + min)

/* -------------------------------------------------------------------------- */

#define FOR_ALL_PARENTS(p)     for( p  = kpm_popFirstMember( *parents ); \
                                    p != NULL;				\
                                    p  = kpm_popNextMember( *parents, p ) )

#define FOR_ALL_OFFSPRINGS(o)  for( o  = kpm_popFirstMember( *offsprings ); \
                                    o != NULL;				\
                                    o  = kpm_popNextMember(*offsprings, o) )

/* -------------------------------------------------------------------------- */

#define MODULE_KEY(s) if( !strcmp(msgv[0], s) )			\
    {								\
      if( msgc > 1 ) enzo_actModule( self, atoi(msgv[1]) );	\
      else           enzo_actModule( self, MFLAG_ACTIVE  );	\
      enzo_logprint( "Module %s activated\n", s );		\
      return( INIT_USED );					\
    }



#define SEL_MSG(a)  {   /* begin option case */	\
  char *MSG_p = a;				\
  if( FALSE );

#define MSG_CASE(s)     else if( strcmp( s, MSG_p ) == 0 )

#define END_MSG         else			\
    {						\
      return( INIT_NOT_USED );			\
    }						\
  }   /* end option case */

/* -------------------------------------------------------------------------- */

#define FLAG_VALUE(s) (strcmp(s,"no")||strcmp(s,"NO")||strcmp(s,"No"))

/* -------------------------------------------------------------------------- */

/* --- subul_createNet ------------------------------------------------------ */
/*                                                                            */
/*                                                                            */
/*  Create a feedforward neural network;                                      */
/*  all possible connections acording to flag `shortcuts' are created;        */
/*  the net isn't initialized at all.                                         */
/*                                                                            */
/*  Parameters                                                                */
/*    layers: number of layers to build                                       */
/*    neurons[]: number of neurons in layers                                  */
/*      first layer == input layer, last layer == output layer                */
/*    shortcuts: TRUE/FALSE enabling/disabling shortcut-connections           */
/*                                                                            */
/*                                                                            */
/*                              --- Klaus Fleischer; modif. js, 6.2.94 ---    */
/*                                                                            */
/*                                                                            */

extern krui_err subul_createNet( int layers, int neurons[], int shortcuts );

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- subul_netcmp --------------------------------------------------------- */
/*                                                                            */
/*   default compare-function for use in nepomuk                              */
/*   compares just the fitness values in networkData                          */
/*                                                                            */
/*                                                                            */

int subul_netcmp( NetID id_1, NetID id_2 );

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- subul_deadInputUnit -------------------------------------------------- */
/*                                                                            */
/*   Checks whether a input unit is completly disconnected or not.            */
/*                                                                            */

int subul_deadInputUnit( int unit );

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- subul_getPatID ------------------------------------------------------- */
/*                                                                            */
/*   Checks whether a input unit is completly disconnected or not.            */
/*                                                                            */

PatID subul_getPatID( char *name);

/*                                                                            */
/* -------------------------------------------------------------------------- */

#endif
