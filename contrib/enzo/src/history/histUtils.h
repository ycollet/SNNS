/*
 * File:    (%W%    %G%)
 * Purpose: This file contains the description of the histRecord struct.  
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

#ifndef __HIST_UTILS__
#define __HIST_UTILS__

/* a marker for newly created nets */

#define NEW_NET_HISTID  -1    


/*--------------------------------------------------------------types--------*/

typedef int HistID;


typedef struct _HistRecord
{
    /* history data */

    /* some kind of garbage collector */

    /* write only structure read by history-functions */


    HistID parent1, parent2;
    
    int learnEpochs;
    int firstEpochs;  /* sum of epochs; updated if any pruning takes place */

    float threshold;   /* threshold for pruning, added masch 13.05.94       */

    /* information about the weights and what were done with them          */
    int added;
    int deleted;
    int pruned;
    int cleaned;

    /* information about the testing of the neural net */
    int   testHit;
    int   testMiss;
    int   testNone;
    int   testHam;
    float testTss;
    float testFitness;

    float slots[20];
    float crossFitness;
} HistRecord;

/*--------------------------------------------------------------functions----*/


/* --- hist_newHistID ------------------------------------------------------- */
/*                                                                            */
/*   returns a new HistID; these are numbered 0, 1, 2, 3 ...                  */
/*                                                                            */
/*                                                                            */

extern HistID hist_newHistID( void );

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- hist_maxHistID ------------------------------------------------------- */
/*                                                                            */
/*   returns the number of used HistID's                                      */
/*                                                                            */
/*                                                                            */

extern HistID hist_maxHistID( void );

/*                                                                            */
/* -------------------------------------------------------------------------- */



#endif






