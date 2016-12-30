/*****************************************************************************
  FILE           : $Source: /projects/higgs1/SNNS/CVS/SNNS/kernel/sources/art_ui.ph,v $
  SHORTNAME      : 
  SNNS VERSION   : 4.2

  PURPOSE        : SNNS-Kernel User Interface Function Prototypes for ART networks
  NOTES          :

  AUTHOR         : Kai-Uwe Herrmann 
  DATE           : 17.05.92

  CHANGED BY     : Sven Doering
  RCS VERSION    : $Revision: 2.6 $
  LAST CHANGE    : $Date: 1998/02/25 15:25:43 $

    Copyright (c) 1990-1995  SNNS Group, IPVR, Univ. Stuttgart, FRG
    Copyright (c) 1996-1998  SNNS Group, WSI, Univ. Tuebingen, FRG

******************************************************************************/

#ifndef _ART_UI_DEFINED_
#define  _ART_UI_DEFINED_

/* begin global definition section */

/* funcname: artui_getClassifiedStatus ()
   Purpose : Returns the classification status of the actual network
   in-Par  : none
   out-Par : status :   Possible results are:   ART_NO_CLASSIFICATION
                                                ART_CLASSIFIED
                                                ART_NOT_CLASSIFIABLE
                                                ART_DONT_KNOW
   ret_val : krui_err:  Returns an error if no Units defined.
*/
krui_err artui_getClassifiedStatus (
                                    art_cl_status *status
                                   );

/* funcname: artui_getClassNo ()
   Purpose : Returns the index of the F2-winner-unit
   in-par  : none
   out-par : class_no:  If class_no is negative, then no winning unit exists.
   ret-val : krui_err:  Returns an error if no Units defined.
*/
krui_err artui_getClassNo (
                           int *class_no
                          );

/* funcname: artui_getN ()
   Purpose : Returns the number of F1-units in an ART1 or ART2 network
   in-par  : none
   out-par : N : number of F1-Units (-1 is returned if not topologically sorted)
   ret-val : krui_err:  Returns an error if no Units defined.
*/
krui_err artui_getN (
                     int *N
                    );

/* funcname: artui_getM ()
   Purpose : Returns the number of F2-units in an ART1 or ART2 network
   in-par  : none
   out-par : M : number of F2-Units (-1 is returned if not topologically sorted)
   ret-val : krui_err:  Returns an error if no Units defined.
*/
krui_err artui_getM (
                     int *M
                    );

/* funcname: artui_getNa ()
   Purpose : Returns the number of F1a-units in an ARTMAP network
   in-par  : none
   out-par : Na: number of F1a-Units (-1 is returned if not topologically sorted)
   ret-val : krui_err:  Returns an error if no Units defined.
*/
krui_err artui_getNa (
                     int *Na
                    );

/* funcname: artui_getNb ()
   Purpose : Returns the number of F1b-units in an ARTMAP network
   in-par  : none
   out-par : Nb: number of F1b-Units (-1 is returned if not topologically sorted)
   ret-val : krui_err:  Returns an error if no Units defined.
*/
krui_err artui_getNb (
                     int *Nb
                    );

/* funcname: artui_getMa ()
   Purpose : Returns the number of F2a-units in an ARTMAP network
   in-par  : none
   out-par : Ma: number of F2a-Units (-1 is returned if not topologically sorted)
   ret-val : krui_err:  Returns an error if no Units defined.
*/
krui_err artui_getMa (
                     int *Ma
                    );

/* funcname: artui_getMb ()
   Purpose : Returns the number of F2b-units in an ARTMAP network
   in-par  : none
   out-par : Mb: number of F2b-Units (-1 is returned if not topologically sorted)
   ret-val : krui_err:  Returns an error if no Units defined.
*/
krui_err artui_getMb (
                     int *Mb
                    );

/* end global definition section */

/* begin private definition section */

/* end private definition section */

#endif 
