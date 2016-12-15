/*
 * File:     (%W%    %G%)
 * Purpose:  main include file, included by all modules
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

#ifndef __ENZO__
#define __ENZO__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

#include "proto.h"


#include "glob_typ.h"
#include "kr_typ.h"
#include "enzo_mem_typ.h"

#include "kr_shell.h"
#include "nepomuk.h"
#include "history/histUtils.h"
#include "networkData.h"
#include "subUtils.h"


extern int    setlinebuf( FILE *stream );
/*  extern time_t time( time_t *tloc );  */



/* ------------------------------------------------------------ constants --- */
/*                                                                            */

/* some errors are all the same for every module                              */

#define MODULE_NO_ERROR    0
#define MODULE_UNKNOWN_ERR 1


/* return codes for the init-functions                                        */
/*    INIT_return < 0  -- warning                                             */
/*    INIT_return > 1  -- serious error                                       */

#define INIT_NOT_USED 0
#define INIT_USED     1  /* note: this is also the code for UNKNOWN_ERROR     */


/* The following strings are used for general initialization/exit.            */
/* Do NOT use them as tokens!                                                 */

#define GENERAL_INIT   "initialize"       /* issued before reading cmd-file   */
#define EVOLUTION_INIT "start_evolution"  /* issued after pre-evolution       */
#define GENERAL_EXIT   "exit"             /* issued at the very end           */


#define WARN_EVOLUTION  -1    /* a warning has occured                        */
#define DO_EVOLUTION     0    /* the struggle for life continues              */
#define STOP_EVOLUTION   1    /* regular termination of evolution             */
#define ERR_EVOLUTION   42    /* something's gone somewhere terribly wrong    */


/* Module-flags indicating the status of a module                             */

#define MFLAG_NULL        0
#define MFLAG_ACTIVE     99   /* general active-flag; greater than all others */

#define MODULE_ACTIVE(m) (m->flags)  /* check if module's active              */

/* Names for three pattern sets are reserved                                  */

#define PATTERN_LEARN "learn"            /* training the neural net           */
#define PATTERN_TEST  "test"             /* evaluating generalization fitness */
#define PATTERN_CROSS "crossvalidation"  /* independent validation patterns   */
                                         /* not used for optimizing           */


/* Indicators for the type of a module                                        */
/* only one type per module permitted! what else.                             */

#define MTYPE_NONE       0
#define MTYPE_PRE        1
#define MTYPE_STOP       2
#define MTYPE_SELECTION  3
#define MTYPE_CROSSOVER  4
#define MTYPE_MUTATION   5
#define MTYPE_OPT        6
#define MTYPE_EVAL       7
#define MTYPE_HISTORY    8
#define MTYPE_SURVIVAL   9
#define MTYPE_POST      10


/*                                                                            */
/* -------------------------------------------------------------------------- */



/* ---------------------------------------------------------------- types --- */
/*                                                                            */

typedef int   ModuleWorkFunction( PopID *parents,
				  PopID *offsprings,
				  PopID *ref);
typedef char *ModuleErrFunction ( int err_code );


typedef struct _ModuleTableEntry
{
    char *name;
    
    /*  ModuleInitFunction *initFct;    this doesn't work ... */
    int (*initFct)( struct _ModuleTableEntry *self, int msgc, char *msgv[] );

    ModuleWorkFunction *workFct;
    ModuleErrFunction  *errFct;

    int type;
    int flags;
    
} ModuleTableEntry;

typedef int ModuleInitFunction( ModuleTableEntry *self,
			        int msgc, char *msgv[]   );



/*                                                                            */
/* -------------------------------------------------------------------------- */



/* ---------------------------------------------------------- global vars --- */
/*                                                                            */

                                        /*               def'd in ...         */
extern ModuleTableEntry ModuleTable[];  /*                          m_table.c */


/*   If a signal occured, this variable is set to TRUE,                       */
/*   so time consuming modules can check for it and stop working.             */
extern int signal_evolution;            /* TRUE iff a signal occured;         */

/*                                                                            */
/* -------------------------------------------------------------------------- */



/* ------------------------------------------------------------ functions --- */
/*                                                                            */

/* --- enzo_actModule ------------------------------------------------------ */
/*                                                                            */
/*   If a module detects its keyword, it should activate itself with          */
/*   this function. This is done automatically if you use the macro           */
/*   MODULE_KEY() in subUtils.h.                                              */
/*                                                                            */

extern void enzo_actModule( ModuleTableEntry *module, int prio );

/*                                                                            */
/* -------------------------------------------------------------------------- */


/* --- enzo_logprint ------------------------------------------------------- */
/*                                                                            */
/*   Function for writing into the log-file; use just like printf().          */
/*   Shoudn't be used a lot!                                                  */
/*                                                                            */

extern void enzo_logprint( char *fmt, ... );

/*                                                                            */
/* -------------------------------------------------------------------------- */



extern double drand48(void);
extern long   lrand48(void);
extern void   srand48(long seedval);


/*                                                                            */
/* -------------------------------------------------------------------------- */



#endif
