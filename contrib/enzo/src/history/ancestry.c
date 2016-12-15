/*
 * File:     (%W%    %G%)
 * Purpose:  definition of ancestry history routines
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
#include "ancestry.h"


#define ANCESTRY_KEY  "ancestry"
#define PS_OUTPUT     "ancestryPS"


#define MAX_GEN_SIZE  512


#define HISTFILE_OPEN_ERROR 2
#define PSFILE_OPEN_ERROR   3


#define EXTENSION "ancestry"
#define OUT_TEXT " # no.    generation members ...\n"

#define PS_EXTENSION "ancestry.ps"


#define PS_HEADER \
 \
"\
/radius 2.5 def \n\
/scaleup {13 mul 10 add exch 10 mul 10 add exch} def \n\
/popel  {scaleup radius 0 360 arc stroke } def \n\
/parent {scaleup moveto scaleup lineto stroke    } def \n\
/myself {gsave 0.5 setgray scaleup moveto scaleup lineto stroke grestore} def\n\
0 setlinewidth 10 10 translate 1.5 1.5 scale \n\
"


static char histfile[MAX_FILENAME_LEN] = HISTORY_FILE_VALUE;
static FILE *hfp;

static int PS = FALSE;    /* flag for postscript output */
static HistID *genList;
static FILE   *psf;

static int genSize = POP_SIZE_VALUE;


int ancestry_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    char filename[MAX_FILENAME_LEN];

    
    MODULE_KEY( ANCESTRY_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { if( hfp ) fclose( hfp ); }
    MSG_CASE( EVOLUTION_INIT  )
    {
	sprintf( filename, "%s.%s", histfile, EXTENSION );
	if( (hfp = fopen( filename, "w" )) == NULL )
	    return( HISTFILE_OPEN_ERROR );
	setlinebuf( hfp );
	fprintf( hfp, OUT_TEXT );

	if( PS )
	{
	    genList = (HistID *) calloc( genSize+1, sizeof( HistID ) );
	    sprintf( filename, "%s.%s", histfile, PS_EXTENSION );
	    if( (psf = fopen( filename, "w" )) == NULL )
	        return( PSFILE_OPEN_ERROR );
	    setlinebuf( psf );
	    fprintf( psf, PS_HEADER );
	}
    }
    
    MSG_CASE( HISTORY_FILE ) { if( msgc > 1 ) strcpy( histfile, msgv[1] );  }
    MSG_CASE( PS_OUTPUT    ) { if( msgc > 1 ) PS  = FLAG_VALUE( msgv[1] );  }
    MSG_CASE( POP_SIZE     ) { if( msgc > 1 ) genSize   = atoi( msgv[1] );  }
    
    END_MSG;

    return( INIT_USED );
}


int ancestry_work( PopID *parents, PopID *offsprings, PopID *ref )
{
    NetID net;
    NetworkData *netData;
    static int genCnt = 0;
    static int x, y = 0, i;

    if( genCnt )
    {
	if( PS && genCnt > 1 )
	{
	    x = 0;
	    FOR_ALL_PARENTS( net )
	    {
		netData = GET_NET_DATA( net );
		fprintf( psf, "%04d %04d popel\n", x, y );
		
		for( i=0; i<genSize && genList[i] != 0; i++ )
		{
		    if( genList[i] == netData->histID )
		    {
			fprintf( psf, "%04d %04d %04d %04d myself\n",
				          x,   y,   i, y-1             );
			break;
		    }
		}

		if( i >= genSize )
		for( i=0; i<genSize && genList[i] != 0; i++ )
		{
		    if( genList[i] == netData->histRec.parent1 )
		    {
			fprintf( psf, "%04d %04d %04d %04d parent\n",
				          x,   y,   i, y-1             );
		    }
		}
		
		x++;
	    }
	    y++;
	}

	fprintf( hfp, "%3d\t ", genCnt );
	i = 0;
	FOR_ALL_PARENTS( net )
	{
	    netData = GET_NET_DATA( net );
	    if( PS ) genList[ i ] = netData->histID; i++;
	    fprintf( hfp, "%4d ", netData->histID );
	}
	fprintf( hfp, "\n" );
    }

    
    genCnt++;

    return( MODULE_NO_ERROR );
}


char *ancestry_errMsg( int err_code )
{
    /* supply the caller with some information about an error */

    static int   err_cnt   = 4;   /* number of recognized errors */
    static char *err_msg[] =
    { "no error (ancestry)", "unknown error (ancestry)",
      "Can't open history File (ancestry)", "Can't open PS-file (ancestry)",
      "specific error message -- not used"};

    return( err_msg[ err_code < err_cnt ? err_code : MODULE_UNKNOWN_ERR ] );
}
