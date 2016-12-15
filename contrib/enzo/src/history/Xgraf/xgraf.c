/************************************************************************
 
xgraf.c - open a X-Window to display lines, dots and text using a simple
          syntax. Commands are read from stdin.
        
          Author: Johannes Feulner, May 1991
          Last Revision: Jul 92

	  XGRAF reads its stdin line by line. Commands are:
	  To draw a point
	  	p <x> <y>

	  
 
************************************************************************/

#include <stdio.h> 
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> 
#include <X11/Xlib.h> 
#include <xview/xview.h> 
#include <xview/canvas.h> 
#include <xview/xv_xrect.h> 
#include <xview/notify.h>

static Canvas canvas;
static Display *glob_dpy;
static Window glob_xwin;
static GC glob_gc;	

typedef struct protokoll {
	char *message;
        struct protokoll *next;
        } Protokoll;

Notify_value read_proc();

static Protokoll protokoll = {NULL, NULL}, *next_prot = &protokoll;
static window_open = 0; 


int main(argc, argv)
int argc;
char *argv[];
{
Frame frame;
Notify_client client = (Notify_client)4711;
void canvas_repaint_proc();

	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
	
	frame = (Frame)xv_create(NULL, FRAME, 
				 NULL);

	(void) xv_create(frame, CANVAS,
		CANVAS_REPAINT_PROC,	canvas_repaint_proc,
		CANVAS_X_PAINT_WINDOW,	TRUE, /* more params for repaint_proc */
		NULL);

	/* monitor stdin in non-blocking mode */
	fcntl(fileno(stdin), F_SETFL, O_NDELAY);
	(void) notify_set_input_func(client, read_proc, fileno(stdin));
  
	xv_main_loop(frame);

	return (0);

} /* main() */



static void process_message(string)
char *string;
{
char message_type, text[BUFSIZ];
int x1=0, y1=0, x2=0, y2=0; 
Protokoll *act_message, *next_message;


	sscanf(string, "%c", &message_type);

	switch (message_type) {
	case 'p': /* a single dot */
		sscanf(string, "%c%d%d", &message_type, &x1, &y1);
		XDrawPoint(glob_dpy, glob_xwin, glob_gc, x1, y1);
		break;
	case 'l': /* a simple line */
		sscanf(string, "%c%d%d%d%d", &message_type, &x1, &y1, &x2, &y2);
		XDrawLine(glob_dpy, glob_xwin, glob_gc, x1, y1, x2, y2);
		break;
	case 't': /* a text */
		text[0] = '\0';
		sscanf(string, "%c%d%d%s", &message_type, &x1, &y1, text);
		XDrawImageString(glob_dpy, glob_xwin, glob_gc, 
			    x1, y1, text, strlen(text)); 
		break;
	case 'c': /* clear window, and reset protocol */
		XClearWindow(glob_dpy, glob_xwin);

		 /* remove protocol */
		next_message = protokoll.next;
		while(next_message != NULL) {
			act_message  = next_message;
			next_message = next_message->next;
		/* deleting 'strdup'ed strings. strdup() uses malloc()!! */
                        free((char *)act_message->message);
			free((char *)act_message);
		}
		protokoll.message = NULL;
		protokoll.next    = NULL;
		next_prot = &protokoll;
		break;
	default:
		fprintf(stderr, "xgraf: unknown message %s ignored\n", string);
		break;
	}
} /* process_message() */




Notify_value read_proc(client, fd) 
Notify_client 	client;
int		fd;
{
char buf[BUFSIZ];
char buf_ch;
int result;
static int next_buf_place = 0;

	if (!window_open) return ((Notify_value)0);

	while ((result = read(fd, &buf_ch, 1)) == 1) {
		if (buf_ch == '\n') {
			/* make string out of message */
			buf[next_buf_place] = '\0';
			next_buf_place = 0;

			/* message im Protokoll speichern, fuer evtl. repaint */
			next_prot->message = strdup(buf);
			next_prot->next=(Protokoll *)malloc((unsigned)sizeof(Protokoll));
			if (next_prot->next == NULL) {
				 fprintf(stderr, "xgraf: No more memory, Trying my best without\n");
		perror(" haha");

			}
			else {
				next_prot = next_prot->next;
				next_prot->next = NULL;
			} 
	
			process_message(buf);
		}
		else {
			buf[next_buf_place] = buf_ch;
			next_buf_place++;
		}
	} /* while */

	if (result == -1 && (errno != EWOULDBLOCK && errno != EAGAIN)) exit(1); /* Error */
	if (result == 0) exit(0); /* pipe has been closed by sender */

} /* read_proc() */



void canvas_repaint_proc(canvas, paint_window, dpy, xwin, xrects)
Canvas 		canvas;
Xv_Window	paint_window;
Display		*dpy;
Window		xwin;
Xv_xrectlist	*xrects;
{
GC gc;
Notify_value  read_proc();
Protokoll *next_message;

	gc = DefaultGC(dpy, DefaultScreen(dpy));

	/* save in global variables */
	glob_dpy  = dpy;
	glob_xwin = xwin;
	glob_gc   = gc;	

	/* replay messages */
	next_message = &protokoll;
	while(next_message->next != NULL) {
		process_message(next_message->message);
		next_message = next_message->next;
	}
	window_open = 1;
	
} /* canvas_repaint_proc() */
