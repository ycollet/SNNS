/*****************************************************************************
  FILE              : co_mem.c
  SHORTNAME         : mem
  VERSION           : 2.0

  PURPOSE           : 
  NOTES             :

  AUTHOR            : Thomas Korb 
  DATE              : 27.6.1991

  CHANGED BY        : contains functions for memory management
  IDENTIFICATION    : %W% %G%
  SCCS VERSION      : %I%
  LAST CHANGE       : %G%

             (c) 1991 by Thomas Korb and the SNNS-Group

******************************************************************************/

#include "co_mem.h"


/*****************************************************************************************
 Function:                     M_alloc
 Purpose: Same as malloc, with error control and exit.
 Returns: &char
*****************************************************************************************/
char *M_alloc(s)
     register unsigned s;
{
  char *Sp;

  if(Sp = malloc(s)) {
    HeapSize += s;
    MaxHeapSize = (HeapSize > MaxHeapSize) ? HeapSize : MaxHeapSize;
    return Sp;
  }
  else {
    (void) printf("out of space, allocated memory %l B. Stopped.\n", HeapSize);
    exit(OutOfSpace);
    return NULL;
  }
}




/*****************************************************************************************
 Function:                     M_free
 Purpose: Same as free, with error control and exit.
 Returns: int
*****************************************************************************************/
M_free(s)
     register char * s;
{
  if( ! s) {
    (void) printf("******bug: NULL string in M_free\n");
    exit(Bug);
  }
  else
    (void) free(s);
  return 0;
}






/*****************************************************************************************
 Function:                     M_SetHeapSize
 Purpose: Increment actual and maxuimum heapsize.
 Returns: void.
*****************************************************************************************/
void M_SetHeapSize(s)
     unsigned s;
{
  HeapSize += s;
  MaxHeapSize = (HeapSize > MaxHeapSize) ? HeapSize : MaxHeapSize;
}
