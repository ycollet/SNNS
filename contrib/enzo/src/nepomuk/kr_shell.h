/*
 * File:     (@(#)kr_shell.h	1.2    8/23/95)
 * Purpose:  declaration of functional interface to SNNSv4.0
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

#ifndef _KR_SHELL_H
#define _KR_SHELL_H

#define USE_SNNS  0

/************************************************************/
/* NETWORK MAMANGEMENT FUNCTIONS                            */
/************************************************************/

extern void ksh_getNet( memNet *n );
extern void ksh_putNet( memNet *n );

extern void ksh_getPattern( memPat *p );
extern void ksh_putPattern( memPat *p );

/************************************************************/
/* NETWORK FUNCTIONS                                        */
/************************************************************/

extern float ksh_netout(int index);
extern float ksh_target(int index);
extern int    ksh_no_patterns();
extern int    ksh_no_outputs();
extern char    *ksh_netname();
extern float  ksh_error_bound(float error_bound);

extern int ksh_readNetinfo( void );
extern int ksh_init_net( char *filename );

/* Einlesen des Netzes <filename>.net und initialisieren der Parameter */

extern int ksh_set_learning_function( char *fname );

/* sets learning function and determines no of parameters */

extern int ksh_set_update_function( char *fname );

/* sets update function and determines no of parameters */

extern int ksh_set_init_function( char *fname , int *no_init_params);

/* sets initialization function and determines no of parameters */

extern int ksh_propagate_pattern( int patno  );

/*   clamps pattern <patno> on Input-Nodes
     propagates Input-Activation-Forward through MLP-FF-Net
*/

extern void ksh_get_epoch_error ( float *tss, int *thamdis );
extern void ksh_get_target_pattern ( int patno );

/************************************************************/
/* Interface FUNCTIONS  to SNNS   via krui.h                */
/************************************************************/

extern krui_err  ksh_allocateUnits(int no_of_units);
extern bool      ksh_areConnectedWeight(int source_unit_no,int target_unit_no, FlintType *weight);
extern int       ksh_createDefaultUnit(void);
extern krui_err  ksh_createLink(int source_unit_no, FlintTypeParam weight);
extern int       ksh_createUnit(char *unit_name, char *out_func_name,
                                char *act_func_name, FlintTypeParam i_act,
                                FlintTypeParam bias);
extern krui_err  ksh_deleteAllOutputLinks(void);
extern void      ksh_deleteAllPatterns(void);
extern krui_err  ksh_deleteLink(void);
extern krui_err  ksh_deleteUnitList(int no_of_units, int unit_list[]);
extern krui_err  ksh_freezeUnit(int unit_no);
extern int       ksh_getCurrentUnit(void);
extern int       ksh_getFirstPredUnit(FlintType *strength);
extern int       ksh_getFirstSuccUnit(int source_unit_no, FlintType *weight);
extern char     *ksh_getLearnFunc(void);
extern FlintType ksh_getLinkWeight(void);
extern void      ksh_getNetInfo(int *no_of_sites, int *no_of_links,
                                int *no_of_STable_entries,
                                int *no_of_FTable_entries);
extern int       ksh_getNextUnit(void);
extern int       ksh_getNextPredUnit(FlintType *strength);
extern int       ksh_getNextSuccUnit(FlintType *weight);
extern int       ksh_getNoOfPatterns(void);
extern int       ksh_getNoOfTTypeUnits(int UnitTType);
extern int       ksh_getNoOfUnits(void);
extern char     *ksh_getUnitActFuncName(int UnitNo);
extern FlintType ksh_getUnitBias(int UnitNo);
extern int       ksh_getUnitInputType(int unit_no);
extern char     *ksh_getUnitName(int UnitNo);
extern char     *ksh_getUnitOutFuncName(int UnitNo);
extern void      ksh_getUnitPosition(int UnitNo, struct PosType *position);
extern int       ksh_getUnitTType(int unit_no);
extern krui_err  ksh_initializeNet(float *parameterInArray, int NoOfInParams);
extern bool      ksh_isConnected(int source_unit_no);
extern bool      ksh_isConnected(int source_unit_no);
extern krui_err  ksh_learnAllPatterns(float *parameterInArray,int NoOfInParams,
                                      float **parameterOutArray,
                                      int *NoOfOutParams);
extern krui_err  ksh_learnSinglePattern(int pattern_no,float *parameterInArray,
                                        int NoOfInParams,
                                        float **parameterOutArray,
                                        int *NoOfOutParams);
extern krui_err  ksh_loadNet(char *filename, char **netname);
extern krui_err  ksh_newPattern(void);
extern krui_err  ksh_saveNet(char *filename, char *netname);
extern krui_err  ksh_setCurrentUnit(int unit_no);
extern int       ksh_searchUnitName(char *unit_name);
extern krui_err  ksh_setLearnFunc(char *learning_func);
extern void      ksh_setLinkWeight(FlintTypeParam weight);
extern void      ksh_setSeedNo(long int seed);
extern krui_err  ksh_setUnitActFunc(int unit_no, char *unitActFuncName);
extern krui_err  ksh_setUnitActivation(int UnitNo,
                                       FlintTypeParam unit_activation);
extern void      ksh_setUnitBias(int UnitNo, FlintTypeParam unit_bias);
extern krui_err  ksh_setUnitFType(int unit_no, char *Ftype_symbol);
extern void      ksh_setUnitInitialActivation(int UnitNo,
					      FlintTypeParam unit_i_activation);
extern void      ksh_setUnitLayerNo(int UnitNo, int layer_no);
extern krui_err  ksh_setUnitName(int unit_no, char *unit_name);
extern krui_err  ksh_setUnitOutFunc(int unit_no, char *unitOutFuncName);
extern krui_err  ksh_setUnitOutput(int unit_no, FlintTypeParam unit_output);
extern void      ksh_setUnitPosition(int UnitNo, struct PosType *position);
extern void      ksh_setUnitSubnetNo(int UnitNo, int subnet_no);
extern krui_err  ksh_setUnitTType(int unit_no, int UnitTType);
extern krui_err  ksh_setUpdateFunc(char *update_func);
extern krui_err  ksh_shufflePatterns(bool on_or_off);


extern void      ksh_deleteNet(void);
extern void      ksh_getUnitDefaults(FlintType *act, FlintType *bias, int *st,
                                     int *subnet_no, int *layer_no,
                                     char **act_func, char **out_func);
extern int       ksh_getFirstUnit(void);
extern char     *ksh_getUnitFTypeName(int UnitNo);
extern FlintType ksh_getUnitInitialActivation(int UnitNo);
extern FlintType ksh_getUnitActivation(int UnitNo);
extern FlintType ksh_getUnitOutput(int UnitNo);
extern int       ksh_getUnitSubnetNo(int UnitNo);
extern unsigned short       ksh_getUnitLayerNo(int UnitNo);
extern bool      ksh_isUnitFrozen(int unit_no);
extern krui_err  ksh_loadNewPatterns(char *filename, int *number);
extern krui_err  ksh_saveNewPatterns(char *filename, int number);
extern krui_err  ksh_setUnitDefaults(FlintTypeParam act, FlintTypeParam bias,
                                     int st, int subnet_no, int layer_no,
                                     char *act_func, char *out_func);
extern bool      ksh_getFuncParamInfo(char *func_name, int func_type,
                                      int *no_of_input_params,
                                      int *no_of_output_params);

extern void      ksh_jogWeights(FlintTypeParam minus, FlintTypeParam plus);

#endif
