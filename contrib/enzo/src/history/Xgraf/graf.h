/************************************************************************/
/*                                                                      */
/* The Neuro-Book Project by Braun-Feulner-Malaka                       */
/*                                                                      */
/* Part:        Grafic-Interface                                        */
/* Module:      graf.h                                                  */
/* Author:      Johannes Feulner, May 1991                              */
/* Date:                                                                */
/*      Integration by O.Ibens into the Neuro-Book Project, 5/10/1993   */
/* Comments:                                                            */
/*      Contains the header information for the file graf.c .         	*/
/* Changes:                                                             */
/*      Jul 92 by jofeu                                                 */
/*      Sept 93 by ortrun: converted to ansi-C                          */
/*                                                                      */
/************************************************************************/



/************************************************************************/
/* Exported Functions:	*/

extern char *start_graf(char  *name, int  x_size, int  y_size,
                        int  x_pos, int  y_pos);
/* open and initialize the window

   char *start_graf(name, x_size, y_size, x_pos, y_pos)
   char *name;
   int x_size, y_size, x_pos, y_pos;

   name: string to appear as window title
   x_size, y_size, x_pos, y_pos: Size and position in
                                     pixlels

   Returns NULL if Window couldn't be started.
   No graphics commands should be issued before start_graf().

*/

extern void  end_graf(void);
/* close graphics window

void end_graf()

No graphics commands should be issued after end_graf().
*/

extern void clear_graf(void);
/* clear the graphics area

    void clear_graf()
*/

extern void setcoord_graf(float  x_min, float  x_max,
                          float  y_min, float  y_max);
/* define a coordinate system

   void setcoord_graf(float  x_min, float  x_max,
                          float  y_min, float  y_max);

    x_min,x_max,y_min,y_max: extremal values that may appear
		    on screen.

    note: a few pixel of the window will be reserved as a
        border area. The drawing area will display coordinates
        that fall within x_min,x_max,y_min,y_max.

    However: Before the first call to setcoord_graf() there
        will be no border and any coordinates passed to other
    functions are treated as pixel coordinates.

    note: All coordinates have (0.0,0.0) in the lower left corner
        (unlike X, but much like math) x growing to the left,
    y growing upwards. Never forget that coordiates are floating
        point numbers (float). Write 1.0, never 1!
*/

extern void point_graf(float  x, float  y);
/* draw a point as a pixel

    void point_graf(x,y)
    foat  x,y;

*/

extern void line_graf(float  x1, float  y1, float  x2, float  y2);
/* draw a line

   void line_graf(x1,y1,x2,y2)
   float  x1,y1,x2,y2;
*/

extern void text_graf(float  x, float  y,char  *text);
/* print a non-white string of ascii text

    static void text_graf(x, y, non_white_text)
    foat x, y;
  	    char *non_white_text;

    prints through %s non_white_text \0 terminated string starting
    at coordinate (x, y).

    note: there is only one fixed font, independent of the
    scale of the coordinate system you use.
*/

extern void number_graf(float  x, float  y, int  number);
/* print a number

    void number_graf(x,y,number)
    float  x,y;
    int number;

    prints the value of number starting at coordinate (x, y).
    this is merely a call to text_graf().
*/

extern void axis_graf(char  *x_text, char  *y_text);
/* draw a coordinate system onto the screen

   void axis_graf(x_text,y_text)
   char  *x_text,*y_text;

   x_text, y_text: labels for x and y axis
*/
