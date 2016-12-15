/*
 * File:     (%W%    %G%)
 * Purpose:  Learning  in the offspring-subpopulation with an error           
 *           and max_epochs ended version of a SNNS-learning function.   
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
#include "learnCV.h"


#define LEARN_SNNS_KEY  "learnCV"
#define LEARN_FCT       "learnfct"
#define LEARN_PARAMS    "learnparam"
#define MAX_EPOCHS      "maxepochs"
#define SHUFFLE         "shuffle"
#define WRITE           "writeLearn"
#define TMP_FILE        "tmpFile"
#define CV_EPOCHS       "CVepochs"

#define EXTENSION       ".learn"

#define ERROR_LEARN     10
#define ERROR_ACTIVATE  13
#define ERROR_PAT       14
#define ERROR_CROSSPAT   15

#define MAX_SNNSFCT     30

/*--------------------------------------------------------------variables----*/


static PatID learnPattern                   = NULL;
static PatID crossPattern                    = NULL; 
static char  learnfct[MAX_SNNSFCT]          = DEFAULT_LEARN_FUNC;
static float learnParam[5]                  = {0.0, 0.0, 0.0, 5.0, 0.0};
static int   maxEpochs                      = 50;
static int   cvEpochs                       = 2;
static int   shuffleFlag                    = TRUE;
static int   writeFlag                      = FALSE;
static char  tmpFileName[MAX_FILENAME_LEN]  = "enzo.tmp";

/*--------------------------------------------------------------functions----*/


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* int learnCV_init( ModuleTableEntry *self, int msgc, char *msgv[] )        */
/*                                                                           */
/*  Reads the incoming string and handles all initial work                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/


int learnCV_init( ModuleTableEntry *self, int msgc, char *msgv[] )
{
    int i;
    
    MODULE_KEY( LEARN_SNNS_KEY );
    
    SEL_MSG( msgv[0] )

    MSG_CASE( GENERAL_INIT    ) { /* nothing to do */ }
    MSG_CASE( GENERAL_EXIT    ) { /* nothing to do */ }
    MSG_CASE( EVOLUTION_INIT  ) { /* nothing to do */ }

    MSG_CASE( LEARN_FCT       ) { if( msgc > 1 )
				      strcpy( learnfct, msgv[1] );
			        }

    MSG_CASE( MAX_EPOCHS      ) { if( msgc > 1 )
				      maxEpochs = atoi( msgv[1] );
			        }
    MSG_CASE( CV_EPOCHS       ) { if( msgc > 1 )
				      cvEpochs = atoi( msgv[1] );
			        }
    MSG_CASE( SHUFFLE         ) { if( msgc > 1 )
				      shuffleFlag = FLAG_VALUE( msgv[1] );
			        }
    MSG_CASE( WRITE           ) { if( msgc > 1)
				      writeFlag = FLAG_VALUE( msgv[1] );
				}
    MSG_CASE( TMP_FILE        ) { if (msgc > 1)
				      strcpy( tmpFileName , msgv[1] );
				}
    MSG_CASE( LEARN_PARAMS    ) { for( i=0; i< 5 && i<(msgc-1); i++ )
				      {
					  learnParam[i] = atof( msgv[i+1] );
				      }
			        }
    END_MSG;

    return ( INIT_USED );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  int learnCV_work (PopID *reference PopID *parents, PopID *offsprings)  */
/*                                                                           */
/*  This function learns the offspring-subpopulation with an error           */
/*  and max_epochs ended version of a SNNS-learning function.                */
/*                                                                           */
/*  The learnfunction is defined in the global part of the module,           */
/*  the learnparameters too.                                                 */
/*---------------------------------------------------------------------------*/

int learnCV_work( PopID *parents, PopID *offsprings, PopID *reference )
{
    NetworkData *data;
    
    NetID activeMember;

    int   noLearnParam;
    int   OutParams;
    float *return_values;
    int   epochs,i;
    float last[4],tss, skalar,average;


    if (learnPattern == NULL)
      learnPattern = subul_getPatID( PATTERN_LEARN );
    
    if (crossPattern == NULL)
      crossPattern = subul_getPatID( PATTERN_CROSS );
 	  
    /* sets learning patterns */

    if( (kpm_setCurrentPattern( learnPattern )) != KPM_NO_ERROR )
      {
	return( ERROR_PAT );
      }
    

    /* the learning routine */ 
    /* get the first net of the offspring-population */
    
    FOR_ALL_OFFSPRINGS( activeMember )
    {
	data = ( kpm_getNetData( activeMember ));

	if( kpm_setCurrentNet( activeMember) != KPM_NO_ERROR )
	  {
	    return ( ERROR_ACTIVATE );
	  }
	

	if( ( ksh_setLearnFunc(learnfct)) != KRERR_NO_ERROR )
	  {
	    return ( ERROR_LEARN );
	  }

	/* sets learning function and determines no of parameters */
    
	ksh_getFuncParamInfo( ksh_getLearnFunc(), LEARN_FUNC,
			       &noLearnParam, &OutParams );

	for (i = 0; i < 4; i++)
	  last[i] = -INFINITY;
	average = -INFINITY;

	

	for( epochs = 0; (epochs < maxEpochs) && (average < skalar); epochs++ )
	  {
	    ksh_learnAllPatterns( learnParam, noLearnParam,
				   &return_values, &OutParams );
	    tss = return_values[0];
	    

	    /* determine the Tss for the crossPatterns                  */
	    /* afterwards print the whole information in the tmp-file      */

	    if ((epochs % cvEpochs) == 0)
	      {	    
		kpm_setCurrentPattern( crossPattern );
		ksh_readNetinfo();    
		skalar = 0.0;
		for ( i = 1; i <= ksh_no_patterns(); i++)
		  {
		    ksh_propagate_pattern  ( i );
		    ksh_get_target_pattern ( i );
                    
		    skalar += ksh_netout(0) * ksh_target(0);
		  }

		kpm_setCurrentPattern( learnPattern );
		
		average = 0;
		for (i=0;i<3;i++)
		  {
		    last[i]  = last[i+1];
		    average += last[i] * 0.25;
		  }
		last[3]  = skalar;
		average += skalar * 0.25;

	      }

	    if( signal_evolution ) break; 

	  }
	

	
	data->epochs             += epochs;
	data->histRec.learnEpochs = epochs;
	data->tss                 = tss;
	data->histRec.testTss     = skalar;
	
      }

    
    return( MODULE_NO_ERROR ); 
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  char *learnCV_errMsg(int err_code)                                       */
/*                                                                           */
/*  returns the actual error message of the error code.                      */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

char *learnCV_errMsg(int err_code)
{
  static char msg[MAX_ERR_MSG_LEN];
  
  switch ( err_code )
    {
    case MODULE_NO_ERROR :
      return ( "learnCV : No Error found" );

    case ERROR_PAT :
      return ("learnCV : Can't set the learnpattern via Nepomuk");

    case ERROR_CROSSPAT :
      return ("learnCV : Can't set the crosspattern via Nepomuk");

    case ERROR_LEARN :
      sprintf(msg,"learnCV : Can't activate the SNNS-learnfucntion %s",
	      learnfct);
      return ( msg );

    case ERROR_ACTIVATE :
      return ("learnCV : Can't activate a offspring-net");
    }

  return ( "learnCV : Unknown error" );

}



