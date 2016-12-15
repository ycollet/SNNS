/*
 * File:    (%W%    %G%) 
 * Purpose: This function writes all information about the testing of the nets   
 *          in two files. The first file contains all information about the        
 *          generated nets (.cross), the second file contains the best, worst       
 *          and average fitness of the parent-population (.popcross).               
 *          The information are written by the testing functions e.g.              
 *          bestGuessHigh, bestGuessLow or classes. 
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
#include "histCross.h"

/*------------------------------------------------------------macros---------*/

#define HIST_CROSS_KEY     "histCross"

#define ERROR_MEM         2
#define ERROR_FILEOPEN    3



#define OUTELEM_TEXT   "# No.      hit    miss    none    %%correct\n"
#define OUTELEM_FORMAT "%4d    %6d  %6d  %6d  %10.4f\n"

#define OUTPOP_TEXT    "# No.     best         worst       average %%correct\n"
#define OUTPOP_FORMAT  "%4d  %10.4f    %10.4f    %10.4f\n"


/*-----------------------------------------------------------------types-----*/

static FILE        *hfpCross = NULL, *hfpPopcross = NULL;
static char        histFileName[255] = "enzo_hist";

/*-----------------------------------------------------------------functions-*/


int histCross_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    char fileName[MAX_FILENAME_LEN];
    
    MODULE_KEY( HIST_CROSS_KEY );

    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT )
    {
	if( hfpCross )    fclose( hfpCross );
	if( hfpPopcross ) fclose( hfpPopcross );
    }

    MSG_CASE( EVOLUTION_INIT )
    {
	sprintf(fileName,"%s.cross",histFileName);
	if( (hfpCross = fopen( fileName, "w" )) == NULL )
	    return( ERROR_FILEOPEN );
	setlinebuf( hfpCross );
	fprintf( hfpCross, OUTELEM_TEXT );

	sprintf(fileName,"%s.popcross",histFileName);
	if( (hfpPopcross = fopen( fileName, "w" )) == NULL )
	    return( ERROR_FILEOPEN );
	setlinebuf( hfpPopcross );
	fprintf( hfpPopcross, OUTPOP_TEXT );
    }
    
    MSG_CASE( HISTORY_FILE ) { if( msgc > 1 )  strcpy ( histFileName,msgv[1] );}
    
    END_MSG;

    return( INIT_USED );
}


/*---------------------------------------------------------------------------*/

int histCross_work( PopID *parents, PopID *offsprings, PopID *ref )
{
  NetID  net;
  NetworkData *netData;
  int pars = 0;
  
  static int genCnt = 0;
  float maxHit = -INFINITY,
        minHit =  INFINITY,
        aveHit =  0.0;
    

  FOR_ALL_PARENTS( net )
  {
      pars ++;
      netData = GET_NET_DATA( net );
      
      aveHit += netData->histRec.testFitness;
	
      if( netData->histRec.testFitness > maxHit )
          maxHit = netData->histRec.testFitness;
      
      if( netData->histRec.testFitness < minHit )
	  minHit = netData->histRec.testFitness;
  }
  
  if( pars )
  {
      aveHit /= pars;
      fprintf( hfpPopcross, OUTPOP_FORMAT, genCnt, maxHit, minHit, aveHit );
      genCnt++;
  }
  
  
  FOR_ALL_OFFSPRINGS( net )
  {
      netData = GET_NET_DATA( net );
      fprintf( hfpCross, OUTELEM_FORMAT, netData->histID,
  	                                netData->histRec.testHit,
	                                netData->histRec.testMiss,
	                                netData->histRec.testNone,
	                                netData->histRec.testFitness  );
  }
  
  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/

char *histCross_errMsg( int err_code )
{
  switch (err_code) 
    {
    case MODULE_NO_ERROR :
      return ("histCross : No Error found");
    case ERROR_FILEOPEN :
      return ("histCross : Can't open history-file for writing");
    case ERROR_MEM :
      return ("histCross : Memory excess");
    }
  return( "histCross : Unknown error" );
}
 
