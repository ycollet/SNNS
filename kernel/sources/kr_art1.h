/*****************************************************************************
  FILE           : $Source: /projects/higgs1/SNNS/CVS/SNNS/kernel/sources/kr_art1.h,v $
  SHORTNAME      :
  SNNS VERSION   : 4.2

  PURPOSE        : SNNS Kernel Function Prototypes for ART1-Networks
  NOTES          :

  AUTHOR         : Kai-Uwe Herrmann
  DATE           : 17.05.92

  CHANGED BY     : Sven Doering
  RCS VERSION    : $Revision: 2.6 $
  LAST CHANGE    : $Date: 1998/02/25 15:26:34 $

    Copyright (c) 1990-1995  SNNS Group, IPVR, Univ. Stuttgart, FRG
    Copyright (c) 1996-1998  SNNS Group, WSI, Univ. Tuebingen, FRG

******************************************************************************/
#ifndef _KR_ART1_DEFINED_
#define  _KR_ART1_DEFINED_


extern int             Art1_NoOfRecUnits;

extern  struct Unit     *Art1_cl_unit; /* Pointer to ART1 cl-unit */
extern  struct Unit     *Art1_nc_unit; /* Pointer to ART1 nc-unit */



/***************************************************************************/
/* kra1_sort ()

   Set logical layer numbers and logical unit numbers in an ART1 network.
   Also this function checks, whether the network is an ART1 network or not.
   Returns an error code, when actual network is no ART1 architecture.
*/
extern krui_err  kra1_sort (

    void

);

/***************************************************************************/
/* kra1_init_i_act ()

   Sets the initial activation values of the units
*/
extern krui_err  kra1_init_i_act (

    double rho

);


/***************************************************************************/
/* kra1_getClassNo ()

   Returns the number of the actually activated class J, 1 <= J <= M
*/
extern int  kra1_getClassNo (

    void

);






#endif

/* 74 lines generated by deleteprivatedefinitions.awk */

