/* Remote average client code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 */
#include "remtop.h"
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <string.h>

#define XC_arrow 2
#define NOHOST 0
#define LOCALHOST -1
#define REMOTEHOST 1

int hostType = NOHOST; 

char *get_next_host(int *cur_host_index, int argc, char *argv[], int up)
{
	*cur_host_index = get_next_host_index(*cur_host_index, argc, up);
	/* TODO: Check if legit host */
	return argv[*cur_host_index+1]; 
	
}

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


/*
 Creates the keyboard and mouse clients based on given host name.
 */
void create_clients(char *host, CLIENT **clnt_keyboard, CLIENT **clnt_mouse)
{
	struct timeval maxtime = {3,0};

	printf("creating keyboard\n"); 
        *clnt_keyboard = clnt_create(host, KEYBOARDPROG, KEYBOARDVERS, "udp");
	printf("end creating keyboard\n"); 
	*clnt_mouse = clnt_create(host, MOUSEPROG, MOUSEVERS, "udp");
	printf("end creating mouse\n");
	/*
	clnt_control( *clnt_keyboard, CLSET_TIMEOUT,(char *)&maxtime);
	clnt_control( *clnt_mouse, CLSET_TIMEOUT, (char *)&maxtime);
	*/
/*
	if (strncmp(host, "localhost", 10) == 0) {
		printf("is localhost\n");
		*isLocalhost = 1;
	} else {
		printf("isn't localhost\n");
		*isLocalhost = 0;
		} */

/*	
	if (*clnt_keyboard == NULL) {
		printf("YEP ITS NULL!\n");
	} else {
		printf("Nope it's not null cuz host is %s\n", host);
	}

        if (*clnt_keyboard == NULL || *clnt_mouse == NULL) {
                clnt_pcreateerror(host);
                exit(1);
		} */

} /* end create_clients */

/*
  Destroys the keyboard and mouse clients.
 */
void destroy_clients(CLIENT *clnt_keyboard, CLIENT *clnt_mouse) 
{
        clnt_destroy( clnt_keyboard );
	clnt_destroy( clnt_mouse ); 
} /* end destroy_clients */

void grab_keycombos(Display *dpy)
{
	/* method stub */
}

void ungrab_keycombos(Display *dpy)
{
	/* method stub */
}

/* grab keyboard and mouse */
void grab_hardware(Display *dpy)
{
	Cursor cursor =  XCreateFontCursor(dpy, XC_arrow); 
	
	XGrabKeyboard(dpy, DefaultRootWindow(dpy), 
		      True, GrabModeAsync, GrabModeAsync, CurrentTime);
	
	XGrabPointer(dpy, DefaultRootWindow(dpy), True,
			     PointerMotionMask| ButtonPressMask |
			     ButtonReleaseMask,
			     GrabModeAsync, GrabModeAsync,
			     DefaultRootWindow(dpy),
			     cursor, CurrentTime);
		   
}

/* ungrab keyboard and mouse */
void ungrab_hardware(Display *dpy) 
{
	XUngrabPointer(dpy, CurrentTime);  
	XUngrabKeyboard(dpy, CurrentTime); 
}



int switch_hosts(char *host, Display *dpy, CLIENT *clnt_keyboard, CLIENT *clnt_mouse) 
{
	if (hostType == LOCALHOST) {
		ungrab_keycombos(dpy);
	} else if (hostType == REMOTEHOST) {
		ungrab_hardware(dpy);
		destroy_clients(clnt_keyboard, clnt_mouse); 
	}

	if (strncmp(host, "localhost", 10) == 0) {
		printf("is localhost\n");
		hostType = REMOTEHOST; /* TODO: change this to = LOCALHOST */
		grab_keycombos(dpy); 
	} else {
		printf("isn't localhost\n");
		hostType = REMOTEHOST;
		create_clients(host, &clnt_keyboard, &clnt_mouse);
		grab_hardware(dpy); 
	}
			
}


void desktopprog_1( char* host, int argc, char *argv[])
{
        CLIENT *clnt_keyboard, *clnt_mouse;        
        
	key_input keyboard_1_arg; 
	mouse_input mouse_1_arg;
	int cur_host_index = 0; 
	Display *dpy;
	int quit = 0;
	char *s;
	int on_press; 
	int kc, button; 

	int x, y;
	int isLocalhost = 0; 
     

/*        clnt_keyboard = clnt_create(host, KEYBOARDPROG, KEYBOARDVERS, "udp");
	  clnt_mouse = clnt_create(host, MOUSEPROG, MOUSEVERS, "udp"); */

	printf("Creating clients \n"); 
	create_clients(host, &clnt_keyboard, &clnt_mouse); 
	printf("End Creating clients \n"); 

	/*
        if (clnt_keyboard == NULL || clnt_mouse == NULL) {
                clnt_pcreateerror(host);
                exit(1);
		} */


	/*******************************************/
	/** XCapture stuff **/
	if((dpy = XOpenDisplay(NULL)) == NULL) {
		printf("Fialing here\n");
		perror(argv[0]); 
		exit(1); 
	} 

	grab_hardware(dpy);

/*	XTestFakeButtonEvent(dpy, 3, 1, CurrentTime);
	XTestFakeButtonEvent(dpy, 3, 0, CurrentTime); */
/*	printf("grabbing!\n"); 
	grab_hardware(dpy, isLocalhost);
	printf("grabbed!\n"); */
/*	ungrab_hardware(dpy); */


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
			button = ((XButtonPressedEvent*)&ev)->button;
			/*	ungrab_hardware(dpy);
			XTestFakeButtonEvent(dpy, button, 1, CurrentTime);
			printf("ungrabbed\n"); 
			grab_hardware(dpy, 1);
			printf("grabbed again\n"); 
			break; */ 

			mouse_1_arg.button_event = 1;
			mouse_1_arg.on_press = 1;
			mouse_1_arg.button = button; 
			mouse_1(&mouse_1_arg, clnt_mouse);
			break; 
		case ButtonRelease:
/*			keyboard_1_arg.keyboard = 0;
			keyboard_1_arg.button_event = 1;
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.button =
			((XButtonReleasedEvent*)&ev)->button; */

/*			button = ((XButtonPressedEvent*)&ev)->button;
			ungrab_hardware(dpy);
			XTestFakeButtonEvent(dpy, button, 0, CurrentTime);  
			grab_hardware(dpy, 1); 
			break; */

			mouse_1_arg.button_event = 1;
			mouse_1_arg.on_press = 0;
			mouse_1_arg.button = 
				((XButtonPressedEvent*)&ev)->button;
			mouse_1(&mouse_1_arg, clnt_mouse);
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
			mouse_1(&mouse_1_arg, clnt_mouse);
			/*mouse_1(&mouse_1_arg, clnt2); */ 
			break;

		case KeyPress: 
			
			kc = ((XKeyPressedEvent*)&ev)->keycode;
		
			
/*			if (isLocalhost) {
				XTestFakeKeyEvent(dpy, kc, 1, CurrentTime);
				XNextEvent(dpy, &ev);
				break;
				
				} */
/*			keyboard_1_arg.keyboard = 1; */
			keyboard_1_arg.on_press = 1;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, clnt_keyboard);
			/*printf("\n%x\n", kc); */
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			/*if(s) printf("KEY: %s\n", s);*/ 
	
			break;
			
			
		case KeyRelease: 
			
			kc = ((XKeyReleasedEvent*)&ev)->keycode;
/*			if (isLocalhost) {
				XTestFakeKeyEvent(dpy, kc, 1, CurrentTime);
				XNextEvent(dpy, &ev);
				break;
				
				} */
/*			keyboard_1_arg.keyboard = 1; */
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, clnt_keyboard); 
			
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			/*if(s) printf("KEY released: %s\n", s); */ 
			if(!strcmp(s, "q")) quit=1;
			if(!strcmp(s, "Up")) {
/*				cur_host_index = get_next_host_index(
					cur_host_index, argc, 1); 
					host = argv[cur_host_index+1]; */
				host = get_next_host(&cur_host_index, argc,
						     argv, 1); 
				printf("host%s\n", argv[cur_host_index+1]);
/*				clnt_destroy( clnt_keyboard );
				clnt_destroy( clnt_mouse ); */
				destroy_clients(clnt_keyboard, clnt_mouse); 
				create_clients(host, &clnt_keyboard,
					       &clnt_mouse); 
			} else if (!strcmp(s, "Down")) {
/*				cur_host_index = get_next_host_index(
					cur_host_index, argc, 0);
					host = argv[cur_host_index+1]; */
				host = get_next_host(&cur_host_index, argc,
						     argv, 0); 
				printf("host%s\n", argv[cur_host_index+1]);
				destroy_clients(clnt_keyboard, clnt_mouse); 
				/*clnt_destroy( clnt_keyboard );
				  clnt_destroy( clnt_mouse ); */
				create_clients(host, &clnt_keyboard,
					       &clnt_mouse); 

			}
			break;

		 
			} 
		

		
	}/*end while */
	
	
	ungrab_hardware(dpy); 
	
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

/*        clnt_destroy( clnt_keyboard );
	  clnt_destroy( clnt_mouse );  */

	destroy_clients(clnt_keyboard, clnt_mouse); 
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
