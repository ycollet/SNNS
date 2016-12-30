/*
 * File:     (@(#)kr_shell.c	1.2    8/23/95)
 * Purpose:  definition of the functional interface from knete to SNNSv4.0.
 *           Interaction takes place solely via these functions.
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
 *               Implementation:   knete 1.0
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <math.h>
#include <malloc.h>

#include "glob_typ.h"

#include "kr_ui.h"
#include "kr_typ.h"       /*  Kernel Types and Constants  */
#include "kr_const.h"     /*  Constant Declarators for SNNS-Kernel  */
#include "kr_def.h"       /*  Default Values  */
#include "kernel.h"       /*  kernel function prototypes  */
#include "kr_mac.h"       /*  Kernel Macros   */
#include "enzo_mem_typ.h" /*  PutNet and GetNet data structures */
/*#include "enzo_mem_ext.h"*/ /*  PutNet and GetNet functionality */

#include "kr_shell.h"

/* Netzparameter */
static int
no_sites,
  no_links,     /* Anzahl Gewichte im Netz */
  no_units,     /* Gesamtzahl Neuronen im Netz */
  no_inputs,    /* Anzahl Eingabe-Neuronen */
  no_outputs,   /* Anzahl Ausgabe-Neuronen */
  no_patterns;  /* Anzahl Muster */

/* Kernel-Interface Funktionsparameter */
static int  no_updateparams,   /* Anzahl Parameter der Updatefunktion */
  no_learnparams;    /* Anzahl Parameter der Lernfunktion */

static float learn_params[5],  /* Lern-Parameter */
  init_params[5],  /* Init-Parameter */
  update_params[5];     /* Update-Parameter */

/* Filenames */
static char *netname;

/* kernel_shell Variable */
static float error_bound = 0.5;

/* Dynamisch allokierte Kommunikationsvariable */
static float *netin = NULL;
static float *netout = NULL;
static float *target = NULL;

/* Dynamisch allokierte interne Varialbe */
static int *inputs = NULL;   /* Liste der Eingabeunits */
static int *outputs = NULL;  /* Liste der Ausgabeunits */

#define MAXPARAMS 5

/************************************************************/
/* NETWORK MAMANGEMENT FUNCTIONS                            */
/************************************************************/

void ksh_getNet( memNet *n ) {
  krm_getNet( n );
}

void ksh_putNet( memNet *n ) {
  krm_putNet( n );
}

void ksh_getPattern( memPat *p ) {
  krm_getPattern( p );
}

void ksh_putPattern( memPat *p ) {
  krm_putPattern( p );
}

/************************************************************/
/* COMMUNICATION  FUNCTIONS                                 */
/************************************************************/

float ksh_netout(int index) {
  if (index >=0 && index < no_outputs)
    return netout[index];
  fprintf(stderr, "Parameter out of range in ksh_netout()");
  return 0.0;
}

float ksh_target(int index) {
  if (index >=0 && index < no_outputs)
    return target[index];
  fprintf(stderr, "Parameter out of range in ksh_target()");
  return 0.0;
}

int   ksh_no_patterns() {
  return no_patterns;
}

int   ksh_no_outputs() {
  return no_outputs;
}

char   *ksh_netname() {
  return netname;
}

float  ksh_error_bound(float eb) {
  error_bound = eb;
  return error_bound;
}

/************************************************************/
/* SYSTEM FUNCTIONS                                         */
/************************************************************/

int ksh_readNetinfo( void ) {
  int i,dummy;
  int *ip,*op;

  /*
    vorbelegen der Parameterarrays mit Defaultwerten,
    allokieren der dynamischen Variablen
  */
  krui_getNetInfo( &no_sites, &no_links, &dummy, &dummy );
  no_units = krui_getNoOfUnits();
  no_outputs =krui_getNoOfTTypeUnits( OUTPUT);
  no_inputs =krui_getNoOfTTypeUnits( INPUT);
  (void) krui_getFuncParamInfo( krui_getUpdateFunc(), UPDATE_FUNC,
				&no_updateparams, &dummy);
  (void) krui_getFuncParamInfo( krui_getLearnFunc(), LEARN_FUNC,
				&no_learnparams, &dummy);

  (void) krui_getFuncParamInfo( krui_getLearnFunc(), INIT_FUNC,
				&dummy, &dummy);

  /* Generierung eines Dummy-Patterns */
  /* --- js, 25.1.94 --- */
  /* --- js, 28.2.94: set no_pattern --- */
  if( (no_patterns = krui_getNoOfPatterns()) == 0 ) {
    (void) krui_newPattern();
  }

  /* ridi 93 : ueberfluessig und gefaehrlich (reinit)            */
  /* js 94:    n"otig und wichtig, falls ohne reinit() verwendet */
  /*           vgl. die "Anderung dort                           */
  if (netin)   free (netin);
  netin  = NULL;
  if (netout)  free (netout);
  netout = NULL;
  if (target)  free (target);
  target = NULL;
  if (inputs)     free (inputs);
  inputs    = NULL;
  if (outputs)    free (outputs);
  outputs   = NULL;
  /* --- js, 25.1.94 --- */

  netin  = (float *) malloc((unsigned)(sizeof(float) * no_inputs));
  netout = (float *) malloc((unsigned)(sizeof(float) * no_outputs));
  target = (float *) malloc((unsigned)(sizeof(float) * no_outputs));
  inputs    = (int *)   malloc((unsigned)(sizeof(int) * no_inputs));
  outputs   = (int *)   malloc((unsigned)(sizeof(int) * no_outputs));

  /* Erstellen der Liste der Ein-/Ausgabeunits */
  ip = inputs;
  op = outputs;
  for(i=1; i<=no_units; i++) {
    if(krui_getUnitTType(i) == INPUT)
      *ip ++ = i;
    if(krui_getUnitTType(i) == OUTPUT)
      *op ++ = i;
  }

  for(i=0; i<MAXPARAMS; i++) {
    update_params[i] = 0.0;
    learn_params[i] = 0.0;
    init_params[i] = 0.0;
  }

  /* Default-Belegungen */
  init_params[0] = -.25;
  init_params[1] = .25;
  learn_params[0] = 0.1;
  error_bound = 0.5;
  return(KRERR_NO_ERROR);
}

int ksh_set_learning_function(char *fname) {
  /* sets learning function and determines no of parameters */
  int dummy,ret_code;

  if ((ret_code = krui_setLearnFunc(fname)) != KRERR_NO_ERROR)
    return (ret_code);
  (void) krui_getFuncParamInfo( krui_getLearnFunc(), LEARN_FUNC,
				&no_learnparams, &dummy);
  return (KRERR_NO_ERROR);
}

int ksh_set_update_function(char *fname) {
  /* sets update function and determines no of parameters */
  int dummy,ret_code;

  if ((ret_code = krui_setUpdateFunc(fname)) != KRERR_NO_ERROR)
    return (ret_code);
  (void) krui_getFuncParamInfo( krui_getUpdateFunc(), UPDATE_FUNC,
				&no_updateparams, &dummy);
  return (KRERR_NO_ERROR);
}

int ksh_set_init_function(char *fname, int *no_initparams) {
  /* sets initialization function and determines no of parameters */
  int dummy,ret_code;

  if ((ret_code = krui_setInitialisationFunc(fname)) != KRERR_NO_ERROR)
    return (ret_code);
  (void) krui_getFuncParamInfo( krui_getInitialisationFunc(), INIT_FUNC,
				no_initparams, &dummy);

  return (KRERR_NO_ERROR);
}

int ksh_propagate_pattern (int patno) {
  /* clamps pattern <patno> on Input-Nodes
     propagates Input-Activation-Forward through MLP-FF-Net */
  int i,ret_code=0;
  int *op;   /* Hilfszeiger */

  op = outputs;
  if (!strcmp (krui_getUpdateFunc(),"Forward_Backward_Prop") ||
      !strcmp (krui_getUpdateFunc(),"ilkd_prop")) {
    update_params[0] = 0;  /* forward pass */
    no_updateparams = 1;   /* security */
  }

  /* clamp inputs */
  if ((ret_code = krui_setPatternNo(patno)) != KRERR_NO_ERROR) {
    return (ret_code);
  }

  krui_showPattern( OUTPUT_NOTHING);
  krui_updateNet(update_params,no_updateparams);
  for (i= 0; i< no_outputs; i++) {
    netout[i] = krui_getUnitOutput(*op++);
  }
  return ret_code;
}

/************************************************************/
/* ACCESS FUNCTIONS                                         */
/************************************************************/

void ksh_get_target_pattern (int patno) {
  int i;
  int *op;   /* Hilfszeiger */

  op = outputs;
  krui_setPatternNo (patno); /* Auswahl des p.ten Musters  */
  krui_showPattern( OUTPUT_ACT); /* Muster an Eingabeschicht anlegen, Target an Ausgabe */
  for (i= 0; i< no_outputs; i++) {
    target[i] = krui_getUnitActivation(*op++); /* Speichern der Zielwerte */
  }
}

/************************************************************/
/* PERFORMANCE FUNCTIONS                                    */
/************************************************************/

void ksh_get_pattern_error(patno,pss,phamdis)
     int patno;
     float *pss;
     int *phamdis;
{
  int i;
  float error;

  *pss = 0.0;
  *phamdis = 0;
  ksh_get_target_pattern(patno);
  ksh_propagate_pattern(patno);
  for (i=0; i< no_outputs; i++) {
    error = netout[i] - target[i];
    *pss+= 0.5 * error * error;
    if ((float)fabs(error)>error_bound) (*phamdis) ++;
  }
}

void ksh_get_epoch_error (tss,thamdis)
     float *tss;
     int *thamdis;
{
  int p,phamdis;
  float pss;

  *tss = 0.0;
  *thamdis = 0;
  for (p=1; p<=krui_getNoOfPatterns(); p++) {
    ksh_get_pattern_error(p,&pss,&phamdis);
    *tss += pss;
    *thamdis += phamdis;
  }
}

/************************************************************/

/* Interface FUNCTIONS  to SNNS   via krui.h                */

/************************************************************/

krui_err  ksh_allocateUnits(int no_of_units) {
  return krui_allocateUnits(no_of_units);
}

/************************************************************/

bool      ksh_areConnectedWeight(int source_unit_no,int target_unit_no, FlintType*weight) {
  return krui_areConnectedWeight(source_unit_no,target_unit_no, weight);
}

/************************************************************/

int       ksh_createDefaultUnit(void) {
  return krui_createDefaultUnit();
}

/************************************************************/

krui_err  ksh_createLink(int source_unit_no, FlintTypeParam weight) {
  return  krui_createLink(source_unit_no, weight);
}

/************************************************************/

int       ksh_createUnit(char *unit_name, char *out_func_name,
                         char *act_func_name, FlintTypeParam i_act,
                         FlintTypeParam bias) {
  return krui_createUnit(unit_name, out_func_name, act_func_name, i_act, bias);
}

/************************************************************/

krui_err  ksh_deleteAllOutputLinks(void) {
  return  krui_deleteAllOutputLinks();
}

/************************************************************/

void      ksh_deleteAllPatterns(void) {
  krui_deleteAllPatterns();
}

/************************************************************/

krui_err  ksh_deleteLink(void) {
  return krui_deleteLink();
}

/************************************************************/

krui_err  ksh_deleteUnitList(int no_of_units, int unit_list[]) {
  return krui_deleteUnitList( no_of_units, unit_list);
}

/************************************************************/


krui_err  ksh_freezeUnit(int unit_no) {
  return krui_freezeUnit( unit_no);
}

/************************************************************/

int       ksh_getCurrentUnit(void) {
  return krui_getCurrentUnit();
}

/************************************************************/

int       ksh_getFirstPredUnit(FlintType *strength) {
  return krui_getFirstPredUnit(strength);
}

/************************************************************/

int       ksh_getFirstSuccUnit(int source_unit_no, FlintType *weight) {
  return krui_getFirstSuccUnit(source_unit_no, weight);
}

/************************************************************/

char     *ksh_getLearnFunc(void) {
  return krui_getLearnFunc();
}

/************************************************************/

FlintType ksh_getLinkWeight(void) {
  return krui_getLinkWeight();
}

/************************************************************/

void      ksh_getNetInfo(int *no_of_sites, int *no_of_links,
                         int *no_of_STable_entries,
                         int *no_of_FTable_entries) {
  krui_getNetInfo(no_of_sites, no_of_links, no_of_STable_entries, no_of_FTable_entries);
}

/************************************************************/

int       ksh_getNextUnit(void) {
  return krui_getNextUnit();
}

/************************************************************/

int       ksh_getNextPredUnit(FlintType *strength) {
  return krui_getNextPredUnit(strength);
}

/************************************************************/

int       ksh_getNextSuccUnit(FlintType *weight) {
  return krui_getNextSuccUnit(weight);
}

/************************************************************/

int       ksh_getNoOfPatterns(void) {
  return krui_getNoOfPatterns();
}

/************************************************************/

int       ksh_getNoOfTTypeUnits(int UnitTType) {
  return krui_getNoOfTTypeUnits(UnitTType);
}

/************************************************************/

int       ksh_getNoOfUnits(void) {
  return krui_getNoOfUnits();
}

/************************************************************/

char     *ksh_getUnitActFuncName(int UnitNo) {
  return krui_getUnitActFuncName(UnitNo);
}

/************************************************************/

FlintType ksh_getUnitBias(int UnitNo) {
  return krui_getUnitBias(UnitNo);
}

/************************************************************/

int       ksh_getUnitInputType(int unit_no) {
  return krui_getUnitInputType(unit_no);
}

/************************************************************/

char     *ksh_getUnitName(int UnitNo) {
  return krui_getUnitName( UnitNo);
}

/************************************************************/

char     *ksh_getUnitOutFuncName(int UnitNo) {
  return krui_getUnitOutFuncName(UnitNo);
}

/************************************************************/

void      ksh_getUnitPosition(int UnitNo, struct PosType *position) {
  krui_getUnitPosition(UnitNo,position);
}

/************************************************************/

int       ksh_getUnitTType(int unit_no) {
  return krui_getUnitTType(unit_no);
}

/************************************************************/

krui_err  ksh_initializeNet(float *parameterInArray, int NoOfInParams) {
  return krui_initializeNet(parameterInArray, NoOfInParams);
}

/************************************************************/

bool      ksh_isConnected(int source_unit_no) {
  return krui_isConnected(source_unit_no);
}

/************************************************************/

krui_err  ksh_learnAllPatterns(float *parameterInArray,int NoOfInParams,
                               float **parameterOutArray,
                               int *NoOfOutParams) {
  return krui_learnAllPatterns(parameterInArray,NoOfInParams,
			       parameterOutArray,NoOfOutParams);
}

/************************************************************/

krui_err  ksh_learnSinglePattern(int pattern_no,float *parameterInArray,
                                 int NoOfInParams,
                                 float **parameterOutArray,
                                 int *NoOfOutParams) {
  return krui_learnSinglePattern(pattern_no,parameterInArray, NoOfInParams,
				 parameterOutArray, NoOfOutParams);
}

/************************************************************/

krui_err  ksh_loadNet(char *filename, char **netname) {
  return krui_loadNet(filename, netname);
}

/************************************************************/

krui_err  ksh_newPattern(void) {
  return krui_newPattern();
}

/************************************************************/

krui_err  ksh_saveNet(char *filename, char *netname) {
  return krui_saveNet(filename, netname);
}

/************************************************************/

krui_err  ksh_setCurrentUnit(int unit_no) {
  return  krui_setCurrentUnit(unit_no);
}

/************************************************************/

int       ksh_searchUnitName(char *unit_name) {
  return  krui_searchUnitName(unit_name);
}

/************************************************************/

krui_err  ksh_setLearnFunc(char *learning_func) {
  return krui_setLearnFunc(learning_func);
}

/************************************************************/

void      ksh_setLinkWeight(FlintTypeParam weight) {
  krui_setLinkWeight(weight);
}

/************************************************************/

void      ksh_setSeedNo(long int seed) {
  krui_setSeedNo(seed);
}

/************************************************************/

krui_err  ksh_setUnitActFunc(int unit_no, char *unitActFuncName) {
  return krui_setUnitActFunc(unit_no, unitActFuncName);
}

/************************************************************/

krui_err  ksh_setUnitActivation(int UnitNo, FlintTypeParam unit_activation) {
  return krui_setUnitActivation(UnitNo,unit_activation);
}

/************************************************************/

void      ksh_setUnitBias(int UnitNo, FlintTypeParam unit_bias) {
  krui_setUnitBias(UnitNo,unit_bias);
}

/************************************************************/

krui_err  ksh_setUnitFType(int unit_no, char *Ftype_symbol) {
  return krui_setUnitFType(unit_no, Ftype_symbol);
}

/************************************************************/

void      ksh_setUnitInitialActivation(int UnitNo,
                                       FlintTypeParam unit_i_activation) {
  krui_setUnitInitialActivation(UnitNo, unit_i_activation);
}

/************************************************************/

void      ksh_setUnitLayerNo(int UnitNo, int layer_no) {
  krui_setUnitLayerNo(UnitNo, layer_no);
}

/************************************************************/

krui_err  ksh_setUnitName(int unit_no, char *unit_name) {
  return krui_setUnitName(unit_no, unit_name);
}

/************************************************************/

krui_err  ksh_setUnitOutFunc(int unit_no, char *unitOutFuncName) {
  return  krui_setUnitOutFunc(unit_no,unitOutFuncName);
}

/************************************************************/

krui_err  ksh_setUnitOutput(int unit_no, FlintTypeParam unit_output) {
  return krui_setUnitOutput(unit_no, unit_output);
}

/************************************************************/

void      ksh_setUnitPosition(int UnitNo, struct PosType *position) {
  krui_setUnitPosition(UnitNo,position);
}

/************************************************************/

void      ksh_setUnitSubnetNo(int UnitNo, int subnet_no) {
  krui_setUnitSubnetNo(UnitNo, subnet_no);
}

/************************************************************/

krui_err  ksh_setUnitTType(int unit_no, int UnitTType) {
  return krui_setUnitTType(unit_no, UnitTType);
}

/************************************************************/

krui_err  ksh_setUpdateFunc(char *update_func) {
  return krui_setUpdateFunc(update_func);
}

/************************************************************/

krui_err  ksh_shufflePatterns(bool on_or_off) {
  return krui_shufflePatterns(on_or_off);
}

void      ksh_deleteNet(void) {
  krui_deleteNet();
}

/************************************************************/

void      ksh_getUnitDefaults(FlintType *act, FlintType *bias, int *st,
                              int *subnet_no, int *layer_no,
                              char **act_func, char **out_func) {
  krui_getUnitDefaults(act, bias,st, subnet_no, layer_no,act_func, out_func);

}

/************************************************************/

int       ksh_getFirstUnit(void) {
  return krui_getFirstUnit();
}

/************************************************************/

char     *ksh_getUnitFTypeName(int UnitNo) {
  return krui_getUnitFTypeName(UnitNo);
}

/************************************************************/

FlintType ksh_getUnitInitialActivation(int UnitNo) {
  return krui_getUnitInitialActivation(UnitNo);

}

/************************************************************/

FlintType ksh_getUnitActivation(int UnitNo) {
  return krui_getUnitActivation(UnitNo);
}

/************************************************************/

FlintType ksh_getUnitOutput(int UnitNo) {
  return krui_getUnitOutput(UnitNo);
}

/************************************************************/

int       ksh_getUnitSubnetNo(int UnitNo) {
  return krui_getUnitSubnetNo(UnitNo);
}

/************************************************************/

unsigned short      ksh_getUnitLayerNo(int UnitNo) {
  return krui_getUnitLayerNo(UnitNo);
}

/************************************************************/

bool      ksh_isUnitFrozen(int unit_no) {
  return  krui_isUnitFrozen(unit_no);
}

/************************************************************/

krui_err  ksh_loadNewPatterns(char *filename, int *number) {
  return  krui_loadNewPatterns(filename, number);

}

/************************************************************/

krui_err  ksh_saveNewPatterns(char *filename, int number) {
  return  krui_saveNewPatterns(filename, number);

}

/************************************************************/

krui_err  ksh_setUnitDefaults(FlintTypeParam act, FlintTypeParam bias,
                              int st, int subnet_no, int layer_no,
                              char *act_func, char *out_func) {
  return krui_setUnitDefaults(act, bias, st, subnet_no, layer_no, act_func, out_func);
}

bool      ksh_getFuncParamInfo(char *func_name, int func_type,
                               int *no_of_input_params,
                               int *no_of_output_params) {
  return krui_getFuncParamInfo(func_name, func_type,
			       no_of_input_params, no_of_output_params);
}

void      ksh_jogWeights(FlintTypeParam minus, FlintTypeParam plus) {
  krui_jogWeights(minus, plus);
}
