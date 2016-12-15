/*
 * File:     (%W%    %G%)
 * Purpose:  Definition  of the global module-table, where all modules have to register. 
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

/* -- pre-evolution ------------------------------------------------------------- */

#include "pre/genpopNepomuk.h"
#include "pre/initPop.h"
#include "pre/loadPop.h"
#include "pre/startPop.h"
#include "pre/optInitPop.h"
#include "pre/hiddenInitPop.h"
#include "pre/inputInit.h"
#include "pre/weightInit.h"
#include "pre/loadSNNSPat.h"
#include "pre/initTrain.h"

/* -- stop-evolution ------------------------------------------------------------- */

#include "stop/stopIt.h"
#include "stop/stopErr.h"

/* -- selection ------------------------------------------------------------------- */

#include "selection/uniformSel.h"
#include "selection/preferSel.h"

/* -- mutation ------------------------------------------------------------------ */

#include "mutation/simpleMut.h"
#include "mutation/mutLinks.h"
#include "mutation/mutUnits.h"
#include "mutation/mutInputs.h"

/* -- crossover ----------------------------------------------------------------- */

#include "crossover/implant.h"
#include "crossover/linCross.h"

/* -- optimization --------------------------------------------------------------- */

#include "opt/learnSNNS.h"
#include "opt/learnINIT.h"
#include "opt/learnCV.h"

#include "opt/prune.h"
#include "opt/adapPrune.h"

#include "opt/nullWeg.h"
#include "opt/relearn.h"
#include "opt/jogWeights.h"
#include "opt/cleanup.h"

/* -- evaluation ----------------------------------------------------------------- */

#include "eval/tssEval.h"
#include "eval/topoEval.h"
#include "eval/learnRating.h"
#include "eval/topologyRating.h"
#include "eval/bestGuessLow.h"
#include "eval/bestGuessHigh.h"
#include "eval/classes.h"

/* -- history -------------------------------------------------------------------- */

#include "history/histSimple.h"
#include "history/histFitness.h"
#include "history/histWeights.h"
#include "history/histInputs.h"
#include "history/histCross.h"
#include "history/Xhist.h"
#include "history/saveThem.h"
#include "history/ancestry.h"

/* -- survival ------------------------------------------------------------------- */

#include "survival/fittest.h"

/* -- post-evolution-------------------------------------------------------------- */

#include "post/saveAll.h"





ModuleTableEntry ModuleTable[] =
{
    { "genpopNepomuk", genpopNepomuk_init, genpopNepomuk_work, genpopNepomuk_errMsg, MTYPE_PRE,       MFLAG_NULL},
    { "initPop",       initPop_init,       initPop_work,       initPop_errMsg,       MTYPE_PRE,       MFLAG_NULL},
    { "loadPop",       loadPop_init,       loadPop_work,       loadPop_errMsg,       MTYPE_PRE,       MFLAG_NULL},
    { "startPop",      startPop_init,      startPop_work,      startPop_errMsg,      MTYPE_PRE,       MFLAG_NULL},
    { "optInitPop",    optInitPop_init,    optInitPop_work,    optInitPop_errMsg,    MTYPE_PRE,       MFLAG_NULL},
    { "hiddenInitPop",   hiddenInitPop_init,   hiddenInitPop_work,   hiddenInitPop_errMsg,   MTYPE_PRE,       MFLAG_NULL},
    { "inputInit",     inputInit_init,     inputInit_work,     inputInit_errMsg,     MTYPE_PRE,       MFLAG_NULL},
    { "weightInit",    weightInit_init,    weightInit_work,    weightInit_errMsg,    MTYPE_PRE,       MFLAG_NULL},
    { "loadSNNSPat",   loadSNNSPat_init,   loadSNNSPat_work,   loadSNNSPat_errMsg,   MTYPE_PRE,       MFLAG_NULL},
    { "initTrain",     initTrain_init,     initTrain_work,     initTrain_errMsg,     MTYPE_PRE,       MFLAG_NULL},
    
    { "stopIt",        stopIt_init,        stopIt_work,        stopIt_errMsg,        MTYPE_STOP,      MFLAG_NULL },
    { "stopErr",       stopErr_init,       stopErr_work,       stopErr_errMsg,       MTYPE_STOP,         1       },

    { "unifSel",       unifSel_init,       unifSel_work,       unifSel_errMsg,       MTYPE_SELECTION, MFLAG_NULL },
    { "preferSel",     preferSel_init,     preferSel_work,     preferSel_errMsg,     MTYPE_SELECTION, MFLAG_NULL },


    { "simpleMut",     simpleMut_init,     simpleMut_work,     simpleMut_errMsg,     MTYPE_MUTATION,  MFLAG_NULL },
    { "mutLinks",      mutLinks_init,      mutLinks_work,      mutLinks_errMsg,      MTYPE_MUTATION,  MFLAG_NULL },
    { "mutUnits",      mutUnits_init,      mutUnits_work,      mutUnits_errMsg,      MTYPE_MUTATION,  MFLAG_NULL },
    { "mutInputs",     mutInputs_init,     mutInputs_work,     mutInputs_errMsg,     MTYPE_MUTATION,  MFLAG_NULL },

    { "implant",       implant_init,       implant_work,       implant_errMsg,       MTYPE_CROSSOVER, MFLAG_NULL },
    { "linCross",       linCross_init,       linCross_work,       linCross_errMsg,       MTYPE_CROSSOVER, MFLAG_NULL },

    { "nullWeg",       nullWeg_init,       nullWeg_work,       nullWeg_errMsg,       MTYPE_OPT,       MFLAG_NULL },
    { "learnSNNS",     learnSNNS_init,     learnSNNS_work,     learnSNNS_errMsg,     MTYPE_OPT,       MFLAG_NULL },
   { "learnINIT",     learnINIT_init,     learnINIT_work,     learnINIT_errMsg,     MTYPE_OPT,       MFLAG_NULL },
    { "learnCV",       learnCV_init,       learnCV_work,       learnCV_errMsg,       MTYPE_OPT,       MFLAG_NULL },
    { "prune",         prune_init,         prune_work,         prune_errMsg,         MTYPE_OPT,       MFLAG_NULL },
    { "adapPrune",     adapPrune_init,     adapPrune_work,     adapPrune_errMsg,     MTYPE_OPT,       MFLAG_NULL },
    { "relearn",       relearn_init,       relearn_work,       relearn_errMsg,       MTYPE_OPT,       MFLAG_NULL },
    { "jogWeight",     jogWeights_init,    jogWeights_work,    jogWeights_errMsg,    MTYPE_OPT,       MFLAG_NULL },
    { "cleanup",       cleanup_init,       cleanup_work,       cleanup_errMsg,       MTYPE_OPT,       MFLAG_NULL },

    { "tssEval",       tssEval_init,       tssEval_work,       tssEval_errMsg,       MTYPE_EVAL,      MFLAG_NULL },
    { "learnRating",   learnRating_init,   learnRating_work,   learnRating_errMsg,   MTYPE_EVAL,      MFLAG_NULL },
    { "topologyRating",topologyRating_init,topologyRating_work,topologyRating_errMsg,MTYPE_EVAL,      MFLAG_NULL },
    { "bestGuessLow",  bestGuessLow_init,  bestGuessLow_work,  bestGuessLow_errMsg,  MTYPE_EVAL,      MFLAG_NULL },
    { "bestGuessHigh", bestGuessHigh_init, bestGuessHigh_work, bestGuessHigh_errMsg, MTYPE_EVAL,      MFLAG_NULL },
    { "classes",       classes_init,       classes_work,       classes_errMsg,       MTYPE_EVAL,      MFLAG_NULL },

    { "histSimple",    histSimple_init,    histSimple_work,    histSimple_errMsg,    MTYPE_HISTORY,   MFLAG_NULL }, 
    { "histFitness",   histFitness_init,   histFitness_work,   histFitness_errMsg,   MTYPE_HISTORY,   MFLAG_NULL }, 
    { "histWeights",   histWeights_init,   histWeights_work,   histWeights_errMsg,   MTYPE_HISTORY,   MFLAG_NULL }, 
    { "histInputs",    histInputs_init,    histInputs_work,    histInputs_errMsg,    MTYPE_HISTORY,   MFLAG_NULL }, 
    { "histCross",      histCross_init,      histCross_work,      histCross_errMsg,      MTYPE_HISTORY,   MFLAG_NULL }, 
   { "Xhist",         Xhist_init,         Xhist_work,         Xhist_errMsg,         MTYPE_HISTORY,   MFLAG_NULL }, 
    { "saveThem",      saveThem_init,      saveThem_work,      saveThem_errMsg,      MTYPE_HISTORY,   MFLAG_NULL }, 
    { "ancestry",      ancestry_init,      ancestry_work,      ancestry_errMsg,      MTYPE_HISTORY,   MFLAG_NULL }, 

    { "fittest",       fittest_init,       fittest_work,       fittest_errMsg,       MTYPE_SURVIVAL,  MFLAG_NULL }, 

    { "saveAll",       saveAll_init,       saveAll_work,       saveAll_errMsg,       MTYPE_POST,      MFLAG_NULL },

    { NULL,            NULL,               NULL,               NULL,                 MTYPE_NONE,      MFLAG_NULL }
};



