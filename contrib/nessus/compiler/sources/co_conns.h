/*****************************************************************************
  FILE              : co_conns.h
  SHORTNAME         : conns
  VERSION           : 2.0

  PURPOSE           : header file for co_conns.c
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

/**** include files common to all modules ****/
#include "co_include.h"

extern short Structures;   /* is set while structures are beeing parsed - needed to */
			   /* supress redefinition of connection messages in */
			   /* map-defined structures (IRREGULAR) */

extern char *nst;          /* NULL string for lint */

extern int ConnectionCtr;  /* no. of connections generated */
