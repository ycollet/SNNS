/*****************************************************************************
  FILE           : $Source: /projects/higgs1/SNNS/CVS/SNNS/kernel/sources/kr_io.h,v $
  SHORTNAME      : kr_io.h
  SNNS VERSION   : 4.2

  PURPOSE        : SNNS-Kernel User Interface File I/O Function Prototypes
  NOTES          :

  AUTHOR         : Niels Mache
  DATE           : 28.05.90

  CHANGED BY     : Sven Doering
  RCS VERSION    : $Revision: 2.8 $
  LAST CHANGE    : $Date: 1998/02/25 15:26:48 $

    Copyright (c) 1990-1995  SNNS Group, IPVR, Univ. Stuttgart, FRG
    Copyright (c) 1996-1998  SNNS Group, WSI, Univ. Tuebingen, FRG

******************************************************************************/
#ifndef _KR_IO_DEFINED_
#define  _KR_IO_DEFINED_


/*  write complete network to disk
*/
extern krui_err  krio_saveNet( char  *filename, char  *netname );

/*  load network form disk
*/
extern krui_err krio_loadNet( char  *filename, char  **netname, char  **netfile_version );

/*  loading pattern file from disk
*/
extern krui_err krio_loadPatterns( char  *filename );

/*  write patterns to disk
*/
extern krui_err krio_savePatterns( char  *filename );

extern krui_err  krio_saveResult( char *filename,
                                  bool create,
                                  int  startpattern,
                                  int  endpattern,
                                  bool includeinput,
                                  bool includeoutput,
                                  float *Update_param_array,
                                  int NoOfUpdateParam);


/*  Function for the 3D-Kernel	*/
/*  Return the base address of the XY-Translation Table
*/
extern struct TransTable  *krio_getXYTransTablePtr( void );


extern void krio_readTimeDelayDefs(void);



#endif



/* 61 lines generated by deleteprivatedefinitions.awk */
