/*****************************************************************************
  FILE              : co_sites.h
  SHORTNAME         : sites
  VERSION           : 2.0

  PURPOSE           : Header file for co_sites.c
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : 
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_include.h"


/**** extern variable declarations ****/
extern char *nst;        /* is always NULL - for lint */

extern int SiteTypeCtr;  /* no. of entries in sites list */

extern SiteListType *SiteList;
