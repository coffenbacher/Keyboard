/* Remote average client code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 */
#include "avg.h"
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <string.h>

#define XC_arrow 2
int get_next_host_index(int cur_host_index, int argc, int up)
{
	int num_hosts = argc-1;
	int next_host_index;

	printf("num hosts: %d\n", num_hosts); 
	if (up) {
		next_host_index = (cur_host_index - 1 + num_hosts)%num_hosts; 
	} else {
		next_host_index = (cur_host_index + 1)%num_hosts;
	}
	printf("next host index: %d\n", next_host_index); 
	return next_host_index;
}

void desktopprog_1( char* host, int argc, char *argv[])
{
        CLIENT *clnt, *clnt2;        
        
	key_input keyboard_1_arg; 
	mouse_input mouse_1_arg;
	int cur_host_index = 0; 
	Display *dpy;
	int quit = 0;
	char *s;
	int on_press; 
	int kc;
	Cursor cursor;
	int x, y, x_root, y_root;
     

        clnt = clnt_create(host, KEYBOARDPROG, KEYBOARDVERS, "udp");
	printf("after clnt\n"); 
	clnt2 = clnt_create(host, MOUSEPROG, MOUSEVERS, "udp"); 



        if (clnt == NULL || clnt2 == NULL) {
		printf("Failing here\n");
                clnt_pcreateerror(host);
		
                exit(1);
        }


	/*******************************************/
	/** XCapture stuff **/
	if((dpy = XOpenDisplay(NULL)) == NULL) {
		printf("Fialing here\n");
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
/*			keyboard_1_arg.keyboard = 0;
			keyboard_1_arg.button_event = 1;
			keyboard_1_arg.on_press = 1;
			keyboard_1_arg.button =
			((XButtonPressedEvent*)&ev)->button; */
			mouse_1_arg.button_event = 1;
			mouse_1_arg.on_press = 1;
			mouse_1_arg.button = 
				((XButtonPressedEvent*)&ev)->button;
			mouse_1(&mouse_1_arg, clnt2);
			break;
		case ButtonRelease:
/*			keyboard_1_arg.keyboard = 0;
			keyboard_1_arg.button_event = 1;
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.button =
			((XButtonReleasedEvent*)&ev)->button; */
			mouse_1_arg.button_event = 1;
			mouse_1_arg.on_press = 0;
			mouse_1_arg.button = 
				((XButtonPressedEvent*)&ev)->button;
			mouse_1(&mouse_1_arg, clnt2);
			break;
		case MotionNotify:
			/*printf("in motion notify \n"); */
/*			keyboard_1_arg.keyboard = 0;
			keyboard_1_arg.button_event = 0; */
			x = ((XPointerMovedEvent*)&ev)->x;
			y = ((XPointerMovedEvent*)&ev)->y;
		        mouse_1_arg.x = x;
			mouse_1_arg.y = y;
			mouse_1_arg.button_event = 0; 
			mouse_1(&mouse_1_arg, clnt2);
			/*mouse_1(&mouse_1_arg, clnt2); */ 
			break;

		case KeyPress: 
			
			kc = ((XKeyPressedEvent*)&ev)->keycode;
			
/*			keyboard_1_arg.keyboard = 1; */
			keyboard_1_arg.on_press = 1;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, clnt);
			/*printf("\n%x\n", kc); */
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			/*if(s) printf("KEY: %s\n", s);*/ 
	
			break;
			
			
		case KeyRelease: 
			
			kc = ((XKeyReleasedEvent*)&ev)->keycode; 
/*			keyboard_1_arg.keyboard = 1; */
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, clnt); 
			
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			/*if(s) printf("KEY released: %s\n", s); */ 
			if(!strcmp(s, "q")) quit=1;
			if(!strcmp(s, "Up")) {
				cur_host_index = get_next_host_index(
					cur_host_index, argc, 1);
				host = argv[cur_host_index+1]; 
				printf("host%s\n", argv[cur_host_index+1]);
				clnt_destroy( clnt );
				clnt_destroy( clnt2 ); 
				clnt = clnt_create(host, KEYBOARDPROG,
						   KEYBOARDVERS, "udp");
				clnt2 = clnt_create(host, MOUSEPROG,
						    MOUSEVERS, "udp");
				if (clnt == NULL || clnt2 == NULL) {
		
					clnt_pcreateerror(host);
					
					exit(1);
        }
			} else if (!strcmp(s, "Down")) {
				cur_host_index = get_next_host_index(
					cur_host_index, argc, 0);
				host = argv[cur_host_index+1]; 
				printf("host%s\n", argv[cur_host_index+1]);
				clnt_destroy( clnt );
				clnt_destroy( clnt2 ); 
				clnt = clnt_create(host, KEYBOARDPROG,
						   KEYBOARDVERS, "udp");
				clnt2 = clnt_create(host, MOUSEPROG,
						    MOUSEVERS, "udp");
				if (clnt == NULL || clnt2 == NULL) {
		
					clnt_pcreateerror(host);
					
					exit(1);
				}
			}
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
	clnt_destroy( clnt2 ); 
        /*printf("average = %e\n", *result_1); */
	printf("done? \n"); 
}


main( int argc, char* argv[] )
{
        char *host;

        if (argc < 2) {
                printf("usage: %s server_host value \n",
                        argv[0]);
                exit(1);
        }
        if (argc > MAXHOSTS + 1) {
                printf("Too many input values\n");
                exit(2);
        }
        host = argv[1];
        desktopprog_1(host, argc, argv);
}
