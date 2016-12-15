/************************************************************************/
/*                                                                      */
/* The Neuro-Book Project by Braun-Feulner-Malaka                       */
/*                                                                      */
/* Part:        Grafic-Interface                                        */
/* Module:      graf.c                                                  */
/* Author:      Johannes Feulner, May 1991                              */
/* Date:                                                                */
/*      Integration by O.Ibens into the Neuro-Book Project, 5/10/1993   */
/* Comments:                                                            */
/*      An interface to xgraf with arbitrary coordinate systems.        */
/* Changes:                                                             */
/*      Jul 92 by jofeu                                                 */
/*      Sept 93 by ortrun: converted to ansi-C                          */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/* Import:      */

#include <stdlib.h> 
#include <stdio.h> 
#include <math.h>
#include "graf.h" 

#ifndef SURFACE
extern double pow(double, double);
extern double log10(double);
#endif


/************************************************************************/
/* Constants:      */

#define  ABSTAND_X  50    /* Abstand der Achsen vom Rand des Fensters   */
#define  ABSTAND_Y  50


/************************************************************************/
/* Static data structures: */

static int      achsen_ur_x;  /* x-Koordinatenursprung in Pixeln        */
static int      achsen_ur_y;  /* y- ...                                 */
static int      achsen_len_x; /* Laenge des x-Bereichs - Rand in Pixeln */
static int      achsen_len_y; /* dto fuer y-Bereich                     */
static float    x_coord_min, x_coord_max; /* x-Koordinaten des Randes   */ 
static float    y_coord_min, y_coord_max; /* dto. fuer y                */
static float    scale_x, scale_y;         /* Koordinateneinheiten/Pixel */
static float    window_x_size, window_y_size; /* Groesse des ganzen Fensters */
static FILE *pfp; /* Filepointer fuer 'popen' auf Fensterprogramm       */


/************************************************************************/
/* Implementation of exported and static functions:     */

/* returns NULL if not successfull */
char *start_graf(char  *name, int  x_size, int  y_size, int  x_pos, int  y_pos)
{
char window_start_string[200];

	sprintf(window_start_string,
		"xgraf -Wl %50s -WL %50s -Ws %d %d -Wp %d %d", 
		name, name, x_size, y_size, x_pos, y_pos);
	pfp = (FILE *) popen(window_start_string , "w");
        achsen_ur_x = 0;	/*Initializing for enabling painting without */
	achsen_ur_y = y_size;	/*using setcoord_graf()                      */
  	scale_x = 1.0;
	scale_y = 1.0;
	window_x_size = x_size;
	window_y_size = y_size;
	return ((char *) pfp); 

} /* start_graf */

/************************************************************************/

void end_graf(void) 
{
	pclose(pfp);
} 

/************************************************************************/

static void pixel_line_graf(int  x1, int  y1, int  x2, int  y2) 
{
	fprintf(pfp, "%c %d %d %d %d\n", 'l', x1, y1, x2, y2);
	fflush(pfp);

} /* pixel_line_graf() */

/************************************************************************/

static void pixel_dot_graf(int  x1, int  y1)
{
	fprintf(pfp, "%c %d %d\n", 'p', x1, y1);
	fflush(pfp);

} /* pixel_dot_graf() */

/************************************************************************/

static void pixel_text_graf(int  x1, int  y1, char  *non_white_text)
{
	fprintf(pfp, "%c %d %d %s\n", 't', x1, y1, non_white_text);
	fflush(pfp);

} /* pixel_text_graf() */

/************************************************************************/

/* Clear grafics window */
void clear_graf(void)
{
    fprintf(pfp, "c\n");
    fflush(pfp);
} 

/************************************************************************/

void line_graf(float  x1, float  y1, float  x2, float  y2) 
{
int  pixel_x1,pixel_y1,pixel_x2,pixel_y2;

    pixel_x1 = (int)(achsen_ur_x + scale_x*x1);
    pixel_y1 = (int)(achsen_ur_y - scale_y*y1);
    pixel_x2 = (int)(achsen_ur_x + scale_x*x2);
    pixel_y2 = (int)(achsen_ur_y - scale_y*y2);

    pixel_line_graf(pixel_x1,pixel_y1,pixel_x2,pixel_y2);
}

/************************************************************************/

void point_graf(float  x, float  y)  
{
int  pixel_x,pixel_y;

    pixel_x = (int)(achsen_ur_x + scale_x*x);
    pixel_y = (int)(achsen_ur_y - scale_y*y);

    pixel_dot_graf(pixel_x,pixel_y);
}

/************************************************************************/

void text_graf(float  x, float  y,char  *text)  
{
    pixel_text_graf((int)(achsen_ur_x + scale_x*x), 
	            (int)(achsen_ur_y - scale_y*y), 
	            text);
}

/************************************************************************/

void number_graf(float  x, float  y, int  number)  
{
char  text[10];

    sprintf(text, "%d", number);
    text_graf(x,y,text);
} 

/************************************************************************/

/*
void number_graf(float  x, float  y, int  number)   
{      
int   pixel_x,pixel_y;
char  text[10];

    pixel_x = achsen_ur_x + scale_x*x;
    pixel_y = achsen_ur_y - scale_y*y;    
    if (0<=number  &&  number<10)    { pixel_x = pixel_x-3;  pixel_y = pixel_y-1; }
    if (10<=number  &&  number<100)  { pixel_x = pixel_x-7;  pixel_y = pixel_y-1; }

    pixel_number_graf(pixel_x,pixel_y,number);
}
*/

/************************************************************************/

void axis_graf(char  *x_text, char  *y_text)  
{
int  i;
int main_ordinate_dist_pixels;
float ordinate, main_ordinate_dist_units;

    /* x-axis */
    line_graf(x_coord_min ,  0.0, x_coord_max , 0.0);  

    /* x-label */
    pixel_text_graf((int)(achsen_ur_x+x_coord_max*scale_x-25) , 
		    (int)(achsen_ur_y+49), 
                    x_text);

    /* mark x ordinates */
    main_ordinate_dist_pixels = (int)(window_x_size / 10);
    main_ordinate_dist_units  = main_ordinate_dist_pixels / scale_x;
    /* round main_ordinate_dist_units to a power of 10 */
    main_ordinate_dist_units  = log10(main_ordinate_dist_units);
    main_ordinate_dist_units  = (int)(main_ordinate_dist_units + 0.6);
    main_ordinate_dist_units  = pow(10, main_ordinate_dist_units);

	/* positive x ordinates */
    for(ordinate = main_ordinate_dist_units; 
        ordinate <= x_coord_max; 
        ordinate+=main_ordinate_dist_units) {
	/* print main mark */
   	line_graf(ordinate, -main_ordinate_dist_pixels/4/scale_y,
                  ordinate, main_ordinate_dist_pixels/4/scale_y);
	/* print value below main mark */
	number_graf(ordinate, -35/scale_y, (int)ordinate);
	/* print small intermediate mark */
	line_graf(ordinate-main_ordinate_dist_units/2, -main_ordinate_dist_pixels/6/scale_y,
                  ordinate-main_ordinate_dist_units/2, main_ordinate_dist_pixels/6/scale_y);
    }
	/* negative x ordinates */
    for(ordinate = -main_ordinate_dist_units; 
        ordinate >= x_coord_min; 
        ordinate-=main_ordinate_dist_units) {
	/* print main mark */
   	line_graf(ordinate, -main_ordinate_dist_pixels/4/scale_y,
                  ordinate, main_ordinate_dist_pixels/4/scale_y);
	/* print value below main mark */
	number_graf(ordinate, -35/scale_y, (int)ordinate);
	/* print small intermediate mark */
	line_graf(ordinate+main_ordinate_dist_units/2, -main_ordinate_dist_pixels/6/scale_y,
                  ordinate+main_ordinate_dist_units/2, main_ordinate_dist_pixels/6/scale_y);
    }


    /* y-axis */
    line_graf(0.0, y_coord_min , 0.0, y_coord_max);  

    /* y-label */
    pixel_text_graf((int)(achsen_ur_x-38), 
		    (int)(achsen_ur_y-y_coord_max*scale_y-20), y_text); 

    /* mark ordinates */
    main_ordinate_dist_pixels = (int)(window_y_size / 10);
    main_ordinate_dist_units  = main_ordinate_dist_pixels / scale_y;
    /* round main_ordinate_dist_units to a power of 10 */
    main_ordinate_dist_units  = log10(main_ordinate_dist_units);
    main_ordinate_dist_units  = (int)(main_ordinate_dist_units + 0.6);
    main_ordinate_dist_units  = pow(10, main_ordinate_dist_units);

	/* positive y ordinates */
    for(ordinate = main_ordinate_dist_units; 
        ordinate <= y_coord_max; 
        ordinate+=main_ordinate_dist_units) {
	/* print main mark */
   	line_graf(-main_ordinate_dist_pixels/4/scale_x, ordinate,
                  main_ordinate_dist_pixels/4/scale_x, ordinate);
	/* print value to the left of a main mark */
	number_graf(-45/scale_x, ordinate, (int)ordinate);
	/* print small intermediate mark */
	line_graf(-main_ordinate_dist_pixels/6/scale_x, ordinate-main_ordinate_dist_units/2,
                   main_ordinate_dist_pixels/6/scale_x, ordinate-main_ordinate_dist_units/2);
    }
	/* negative y ordinates */
    for(ordinate = -main_ordinate_dist_units; 
        ordinate >= y_coord_min; 
        ordinate-=main_ordinate_dist_units) {
	/* print main mark */
   	line_graf(-main_ordinate_dist_pixels/4/scale_x, ordinate,
                  main_ordinate_dist_pixels/4/scale_x, ordinate);
	/* print value to the left of a main mark */
	number_graf(-45/scale_x, ordinate, (int)ordinate);
	/* print small intermediate mark */
	line_graf(-main_ordinate_dist_pixels/6/scale_x, ordinate+main_ordinate_dist_units/2,
                   main_ordinate_dist_pixels/6/scale_x, ordinate+main_ordinate_dist_units/2);
    }
}

/************************************************************************/

void setcoord_graf(float  x_min, float  x_max, float  y_min, float  y_max)
{
    /* Laenge der Achsen */
    achsen_len_x = (int)(window_x_size - 2*ABSTAND_X);     
    achsen_len_y = (int)(window_y_size - 2*ABSTAND_Y);

    /* dargestellter Koordinatenbereich */
    x_coord_min = x_min;
    x_coord_max = x_max;
    y_coord_min = y_min;
    y_coord_max = y_max;

    /* Skalierfaktoren in Pixel/Koordinateneinheit */
    scale_x = achsen_len_x/(x_coord_max - x_coord_min); 
    scale_y = achsen_len_y/(y_coord_max - y_coord_min);

    /* Ursprung des Achsenkreuzes in Bezug auf Fenster: */
    achsen_ur_x = (int)(ABSTAND_X - x_coord_min*scale_x);    
    achsen_ur_y = (int)(ABSTAND_Y + y_coord_max*scale_y);    
}
