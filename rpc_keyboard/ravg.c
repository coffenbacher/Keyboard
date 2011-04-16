/* Remote average client code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 */
#include "avg.h"
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <string.h>

#define XC_arrow 2
void
averageprog_1( char* host, int argc, char *argv[])
{
        CLIENT *clnt, *clnt2;
        double  *result_1, *dp, f;
        char *endptr;
        int i;
	key_input keyboard_1_arg; 
	mouse_input mouse_1_arg;
	
	Display *dpy;
	int quit = 0;
	char *s;
	int on_press; 
	int kc;
	Cursor cursor;
	int x, y, x_root, y_root;
     
        /*average_1_arg.input_data.input_data_val =
                (double*) malloc(MAXAVGSIZE*sizeof(double));
        dp = average_1_arg.input_data.input_data_val;

	average_1_arg.input_data.input_data_len = 1; */

	

/*        average_1_arg.input_data.input_data_len = argc - 2;
        for ( i = 1; i <= (argc - 2); i++) {
                f = strtod(argv[i + 1], &endptr);
                printf("value   = %e\n", f);
                *dp = f;
                dp++;
		} */


        clnt = clnt_create(host, AVERAGEPROG, AVERAGEVERS, "udp");
	/*clnt2 = clnt_create(host, MOUSEPROG, MOUSEVERS, "udp"); */

        if (clnt == NULL) {
                clnt_pcreateerror(host);
                exit(1);
        }

	/*******************************************/
	/** XCapture stuff **/
	if((dpy = XOpenDisplay(NULL)) == NULL) { 
		perror(argv[0]); 
		exit(1); 
	} 
	
	cursor = XCreateFontCursor(dpy, XC_arrow);
	XGrabKeyboard(dpy, DefaultRootWindow(dpy), 
		      False, GrabModeAsync, GrabModeAsync, CurrentTime);
	XGrabPointer(dpy, DefaultRootWindow(dpy), False,
		     PointerMotionMask | ButtonPressMask | ButtonReleaseMask,
		     GrabModeAsync, GrabModeAsync, DefaultRootWindow(dpy),
		     cursor, CurrentTime); 
		   

	
	while(!quit) { 
		XEvent ev; 
		
		XNextEvent(dpy, &ev); 
		
		switch (ev.type) {
		case ButtonPress:
			keyboard_1_arg.keyboard = 0;
			keyboard_1_arg.button_event = 1;
			keyboard_1_arg.on_press = 1;
			keyboard_1_arg.button = ((XButtonPressedEvent*)&ev)->button; 
			average_1(&keyboard_1_arg, clnt);
			break;
		case ButtonRelease:
			keyboard_1_arg.keyboard = 0;
			keyboard_1_arg.button_event = 1;
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.button = ((XButtonReleasedEvent*)&ev)->button; 
			average_1(&keyboard_1_arg, clnt);
			break;
		case MotionNotify:
			/*printf("in motion notify \n"); */
			keyboard_1_arg.keyboard = 0;
			keyboard_1_arg.button_event = 0;
			x = ((XPointerMovedEvent*)&ev)->x;
			y = ((XPointerMovedEvent*)&ev)->y;
		        keyboard_1_arg.x = x;
			keyboard_1_arg.y = y;
			average_1(&keyboard_1_arg, clnt);
			/*mouse_1(&mouse_1_arg, clnt2); */ 
			break;

		case KeyPress: 
			
			kc = ((XKeyPressedEvent*)&ev)->keycode;
			keyboard_1_arg.keyboard = 1;
			keyboard_1_arg.on_press = 1;
			keyboard_1_arg.keycode = kc;
			average_1(&keyboard_1_arg, clnt);
			/*printf("\n%x\n", kc); */
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			/*if(s) printf("KEY: %s\n", s);*/ 
	
			break;
			
			
		case KeyRelease: 
			
			kc = ((XKeyReleasedEvent*)&ev)->keycode; 
			keyboard_1_arg.keyboard = 1;
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.keycode = kc;
			average_1(&keyboard_1_arg, clnt); 
			
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			/*if(s) printf("KEY released: %s\n", s); */ 
			if(!strcmp(s, "q")) quit=1;  
			break;

		 
		}
		

		
	}/*end while */
	
	
	XUngrabPointer(dpy, CurrentTime); 
	XUngrabKeyboard(dpy, CurrentTime); 
	
	if (XCloseDisplay(dpy)) { 
		perror(argv[0]); 
		exit(1); 
	} 


	/** End XCapture stuff **/
	/*******************************************/

/*	result_1 = average_1(&average_1_arg, clnt);
        if (result_1 == NULL) {
                clnt_perror(clnt, "call failed:");
		} */

        clnt_destroy( clnt );
        /*printf("average = %e\n", *result_1); */
	printf("done? \n"); 
}


main( int argc, char* argv[] )
{
        char *host;

        if (argc < 3) {
                printf("usage: %s server_host value ...\n",
                        argv[0]);
                exit(1);
        }
        if (argc > MAXAVGSIZE + 2) {
                printf("Two many input values\n");
                exit(2);
        }
        host = argv[1];
        averageprog_1(host, argc, argv);
}
