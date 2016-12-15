/*
 * File:    (%W%    %G%) 
 * Purpose: histFitness writes the fitness-values of all networks in two files 
 *          The first file contains the fitness of all tested networks according   
 *          to their generation. The second file contains the information about    
 *          the best, worst and average fitness of the parent-population. 
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
#include "histFitness.h"

/*------------------------------------------------------------------macros---*/

#define HIST_FITNESS_KEY  "histFitness"



#define ERROR_MEM         2
#define ERROR_FILEOPEN    3



#define OUTELEM_TEXT   "#  No.          fitness  \n"
#define OUTELEM_FORMAT "%4d    %14.6f \n"

#define OUTPOP_TEXT    "#  No.   best fitness     worst fitness   \
average fitness\n"
#define OUTPOP_FORMAT  "%4d  %14.6f    %14.6f    %14.6f\n"


/*-----------------------------------------------------------------variables-*/


static FILE        *hfpFit, *hfpPopfit;
static char        histFileName[MAX_FILENAME_LEN] = HISTORY_FILE_VALUE;

/*-----------------------------------------------------------------functions-*/


int histFitness_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
  char fileName[MAX_FILENAME_LEN];
  
  MODULE_KEY( HIST_FITNESS_KEY );

  SEL_MSG( msgv[0] )
 
  MSG_CASE( GENERAL_INIT )   { /* nothing to do */ }
  MSG_CASE( EVOLUTION_INIT )
  {
      sprintf(fileName,"%s.fit",histFileName);
      if( (hfpFit = fopen( fileName, "w" )) == NULL )
          return( ERROR_FILEOPEN );
      setlinebuf( hfpFit );
      fprintf( hfpFit, OUTELEM_TEXT );
      
      sprintf(fileName,"%s.popfit",histFileName);
      if( (hfpPopfit = fopen( fileName, "w" )) == NULL )
          return( ERROR_FILEOPEN );
      setlinebuf( hfpPopfit );
      fprintf( hfpPopfit, OUTPOP_TEXT );
  }
  
  MSG_CASE( GENERAL_EXIT )
  {
      if( hfpFit )     fclose( hfpFit );
      if( hfpPopfit )  fclose( hfpPopfit );
  }

  MSG_CASE( HISTORY_FILE    ) { if( msgc > 1 ) strcpy ( histFileName,msgv[1] );}
  
  END_MSG;
  
  return( INIT_USED );
}

/*---------------------------------------------------------------------------*/

int histFitness_work( PopID *parents, PopID *offsprings, PopID *ref )
{
  NetID  net;
  NetworkData *netData;
  int pars = 0;
  
  static int   genCnt     = 0;          /* count no of generations */
  float aveFitness = 0.0,
        maxFitness = -INFINITY,
        minFitness =  INFINITY;

  
  FOR_ALL_PARENTS( net )
  {
      pars ++;
      netData = GET_NET_DATA( net );
      
      aveFitness += netData->fitness;
      if( netData->fitness > maxFitness ) { maxFitness = netData->fitness;  }
      if( netData->fitness < minFitness ) { minFitness = netData->fitness;  }
  }
  
  if( pars )
  {
      aveFitness /= pars; 

      fprintf( hfpPopfit, OUTPOP_FORMAT,
	      genCnt, minFitness, maxFitness, aveFitness );
  }
  
  FOR_ALL_OFFSPRINGS( net )
  {
      netData = GET_NET_DATA( net );
      fprintf( hfpFit, OUTELEM_FORMAT, netData->histID, netData->fitness );
  }
  
  genCnt++;
 
  return( MODULE_NO_ERROR );
}

/*---------------------------------------------------------------------------*/

char *histFitness_errMsg( int err_code )
{
  switch (err_code) 
    {
    case MODULE_NO_ERROR :
      return ("histFitness : No Error found");
    case ERROR_FILEOPEN :
      return ("histFitness : Can't open history-file for writing");
    case ERROR_MEM :
      return ("histFitness : Memory excess");
    }
  return( "histFitness : Unknown error" );
}

