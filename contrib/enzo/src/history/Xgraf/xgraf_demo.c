#include <stdio.h> 
#include <math.h> 
#include "graf.h"

int main(argc, argv)
int argc;
char *argv[];
{
float i;

        start_graf("xgraf_demo", 800, 600, 100, 100);
	
	setcoord_graf(-5.0, 100.0, -50.0, 50.0);

	for(i=.0; i<100.5; i+=.05) point_graf((float)i, 50*(float)sin((double)i));

	text_graf(50.0, 15.0, "HALLO_SINUS");

	axis_graf("X", "50*sin(X)");

	sleep(10000);
	end_graf();

	return (0);

} /* main() */

