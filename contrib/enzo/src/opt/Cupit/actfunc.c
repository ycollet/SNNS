/* external procedures for cupit program cuoitlearn.nn */
/* Thomas Ragg 1995/11/16	             	     */

#include <stdio.h>
#include <cupittypes.h>
#include <math.h>

Real sigmoid (Real x)
{
  Real y;
  if (x < -16.0)
    y = 0;
  else if (x > 16.0)
    y =  1;

  else y =  (1.0/ (1.0 + exp ((double) -x) ) );
  return y;

  /*return x;*/
}

Real sigmoidPrime (Real x)
{
  Real y;
  y = (x * (1-x) );
  
  return y;
}

