/*
 * File:     (%W%    %G%)
 * Purpose:  Definition of some utility functions
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
#include "subUtils.h"



#define OFFSET 10  /* space between successive  rows of units                 */


#define ERRCHK(err)  if((err) != KRERR_NO_ERROR) return(err);


/* --- subul_createNet ------------------------------------------------------ */
/*                                                                            */
/*                                                                            */
/*  Create a feedforward neural network;                                      */
/*  all possible connections acording to flag are created;                    */
/*  the net isn't initialized at all.                                         */
/*                                                                            */
/*  Parameters                                                                */
/*    layers: number of layers to build                                       */
/*    neurons[]: number of neurons in layers                                  */
/*      first layer == input layer, last layer == output layer                */
/*    shortcuts: TRUE/FALSE enabling/disabling shortcut-connections           */
/*                                                                            */
/*                                                                            */
/*              --- "ubernommen von Klaus Fleischer; modif. js, 6.2.94 ---    */
/*                                                                            */

krui_err subul_createNet( int layers, int neurons[], int shortcuts )
{
    int i, j, k;
    int no_of_units = 0;
    int unitCnt;

    struct PosType unit_pos;

    krui_err ret_code;

    
    if( neurons[     0    ] == 0 ) return( KRERR_NO_INPUT_UNITS  );
    if( neurons[layers - 1] == 0 ) return( KRERR_NO_OUTPUT_UNITS );


    for( i = 0; i < layers; i++ )  no_of_units += neurons[i];
    
    ERRCHK( ret_code = ksh_allocateUnits( no_of_units ) );

    /* --- create units --- */
    unit_pos.x = 3;
    for ( j = 0, unit_pos.x = 3; j < layers; j++, unit_pos.x += OFFSET )
    {
	for (i = 1; i <= neurons[ j ]; i++)
	{
	    if( (k = ksh_createDefaultUnit()) < 0 ) ERRCHK( k );

	    if( j == 0 )
	        ret_code = ksh_setUnitTType( k, INPUT );
	    else if( j == layers-1 )
		ret_code = ksh_setUnitTType( k, HIDDEN );
	    else
		ret_code = ksh_setUnitTType( k, OUTPUT );

	    ERRCHK( ret_code );

	    ERRCHK( ret_code = ksh_setUnitOutFunc( k, DEFAULT_OUT_FUNC ) );
	    ERRCHK( ret_code = ksh_setUnitActFunc( k, DEFAULT_ACT_FUNC ) );
	    
	    unit_pos.y = i;
	    ksh_setUnitPosition( k, &unit_pos );
	}
    }

    /* --- create links --- */
    unitCnt = no_of_units; 
    for ( j = 1; j < layers; j++ )
    {
	unitCnt -= neurons[layers - j];

	for ( k = unitCnt + 1; k <= unitCnt + neurons[layers-j];  k++ )
	{
	    ERRCHK( ret_code  = ksh_setCurrentUnit( k ) );

	    for ( i  = (shortcuts ? 1 : unitCnt-neurons[layers-j-1]+1 );
		  i <= unitCnt; i++ )
	    {
		ERRCHK( ksh_createLink( i, 0.0 ) );
	    }
	}
    }

    ERRCHK( ret_code = ksh_setLearnFunc( DEFAULT_LEARN_FUNC ) );


    return( KRERR_NO_ERROR );
}
    
/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- subul_netcmp --------------------------------------------------------- */
/*                                                                            */
/*   default compare-function for use in nepomuk                              */
/*   compares just the fitness values in networkData                          */
/*                                                                            */
/*                                                                            */

int subul_netcmp( NetID id_1, NetID id_2 )
{
    NetworkData *d1, *d2;
    float diff;

    d1 = kpm_getNetData( id_1 );
    d2 = kpm_getNetData( id_2 );

    diff = d1->fitness-d2->fitness;
    
    if     ( diff > 0 ) return(  1 );
    else if( diff < 0 ) return( -1 );
    else                return(  0 );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* --- subul_deadInputUnit -------------------------------------------------- */
/*                                                                            */
/*   Checks whether a input unit is completly disconected or not.             */
/*                                                                            */

int subul_deadInputUnit( int unit )
{
    FlintType dummy;
    int curr, ret;

    curr = ksh_getCurrentUnit();
    
    if(    ksh_getUnitTType( unit ) == INPUT
        && ksh_getFirstSuccUnit( unit, &dummy ) == 0 )
    {
        ret = TRUE;
    }
    else ret = FALSE;

    ksh_setCurrentUnit( curr );
    return( ret );
}

/*                                                                            */
/* -------------------------------------------------------------------------- */

/* --- subul_getPatID ------------------------------------------------------- */
/*                                                                            */
/*                                                                            */

PatID subul_getPatID( char *name)
{
  PatID actPat,retPat= NULL;
  for ( actPat = kpm_getFirstPat(); actPat != NULL; 
        actPat = kpm_getNextPat( actPat ))
    {
      if (!strcmp(kpm_getPatName( actPat ),name))
	retPat = actPat;
    }
  return ( retPat );
}
	
/*                                                                            */
/* -------------------------------------------------------------------------- */






