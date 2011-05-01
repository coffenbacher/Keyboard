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

void switch_hosts(char *host, Display *dpy, CLIENT **clnt_keyboard,
		  CLIENT **clnt_mouse, int *cur_host_index,
		  int argc, char *argv[]);

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



        *clnt_keyboard = clnt_create(host, KEYBOARDPROG, KEYBOARDVERS, "udp");
	*clnt_mouse = clnt_create(host, MOUSEPROG, MOUSEVERS, "udp");

	/* TODO: Check if null independently */

        if (*clnt_keyboard == NULL || *clnt_mouse == NULL) {
                clnt_pcreateerror(host);
                exit(1);
	} 
	
} /* end create_clients */

/*
  Destroys the keyboard and mouse clients.
 */
void destroy_clients(CLIENT *clnt_keyboard, CLIENT *clnt_mouse) 
{
	/* TODO: confirm that we aren't supposed to be passing ** */
	
        clnt_destroy( clnt_keyboard );
	clnt_destroy( clnt_mouse ); 
} /* end destroy_clients */

void grab_keycombos(Display *dpy)
{
	/*TODO: check for badaccess errors and stuff */
	int kc;
	int res;
	
	kc = XKeysymToKeycode(dpy, XStringToKeysym("q"));
	res = XGrabKey(dpy, kc, ControlMask | Mod1Mask,
	    DefaultRootWindow(dpy), True, GrabModeAsync, 
	    GrabModeAsync);
	if (res == BadAccess) {
		fprintf(stderr, "Bad Acces with q\n"); 
	}
	kc = XKeysymToKeycode(dpy, XStringToKeysym("Up"));
	res = XGrabKey(dpy, kc, ControlMask | ShiftMask,
	    DefaultRootWindow(dpy), True, GrabModeAsync, 
	    GrabModeAsync);
	if (res == BadAccess) {
		fprintf(stderr, "Bad Acces with Up\n"); 
	} 
	kc = XKeysymToKeycode(dpy, XStringToKeysym("Down"));
	res = XGrabKey(dpy, kc, ControlMask | ShiftMask,
	    DefaultRootWindow(dpy), True, GrabModeAsync, 
	    GrabModeAsync);
	if (res == BadAccess) {
		fprintf(stderr, "Bad Acces with q\n"); 
	}
	printf("Made through all keys..\n"); 
	
}

void ungrab_keycombos(Display *dpy)
{
	XUngrabKey(dpy, AnyKey, AnyModifier, DefaultRootWindow(dpy)); 
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

void localhostLoop(Display *dpy, CLIENT **clnt_keyboard, CLIENT **clnt_mouse,
		    int *cur_host_index, int argc, char *argv[])
{
	int quit = 0;
	int kc;
	XEvent ev;
	char *host;
	char *s; 
	
	while(!quit) {
		XNextEvent(dpy, &ev);
		switch (ev.type) {
		case KeyPress:
			kc = ((XKeyPressedEvent*)&ev)->keycode; 
			printf("\n%x\n", kc);
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			if(s) printf("KEY: %s\n", s);
			/* TODO: make sure shouldn't be strncmp*/
			if(!strcmp(s, "q")) {
				ungrab_keycombos(dpy); 
				quit=1;
			} else if(!strcmp(s, "Up")) {
				host = get_next_host(cur_host_index, argc,
						     argv, 1);
					
				switch_hosts(host, dpy, clnt_keyboard,
					     clnt_mouse, cur_host_index,
					     argc,  argv);
				quit = 1;
			} else if (!strcmp(s, "Down")) {
				host = get_next_host(cur_host_index, argc,
						     argv, 0);
					
				switch_hosts(host, dpy, clnt_keyboard,
					     clnt_mouse, cur_host_index,
					     argc,  argv);
				quit = 1;
			}
			break;
		}
	}
	
}


/* TODO: get proper inputs in here (change argv and argc)*/
void remoteHostLoop(Display *dpy, CLIENT **clnt_keyboard, CLIENT **clnt_mouse,
		    int *cur_host_index, int argc, char *argv[]) 
{
	int quit = 0;
	key_input keyboard_1_arg; 
	mouse_input mouse_1_arg;
	XEvent ev;
	char *host;
	int button;
	int x, y, kc;
	char *s; 
	int shift_down = 0, ctrl_down = 0, alt_down = 0; 
	/*TODO: change key_input and mouse_input to be * so more like what
	 we learned -> don't forget to malloc! */
	
	while(!quit) { 

		XNextEvent(dpy, &ev); 

		switch (ev.type) {
		case ButtonPress:
			button = ((XButtonPressedEvent*)&ev)->button;
			mouse_1_arg.button_event = 1;
			mouse_1_arg.on_press = 1;
			mouse_1_arg.button = button; 
			mouse_1(&mouse_1_arg, *clnt_mouse);
			break; 
		case ButtonRelease:
			mouse_1_arg.button_event = 1;
			mouse_1_arg.on_press = 0;
			mouse_1_arg.button = 
				((XButtonPressedEvent*)&ev)->button;
			mouse_1(&mouse_1_arg, *clnt_mouse);
			break; 
		case MotionNotify:
			x = ((XPointerMovedEvent*)&ev)->x;
			y = ((XPointerMovedEvent*)&ev)->y;
		        mouse_1_arg.x = x;
			mouse_1_arg.y = y;
			mouse_1_arg.button_event = 0; 
			mouse_1(&mouse_1_arg, *clnt_mouse);
			break;

		case KeyPress: 
			
			kc = ((XKeyPressedEvent*)&ev)->keycode;
			keyboard_1_arg.on_press = 1;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, *clnt_keyboard);
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0));
			if (!strncmp(s, "Alt", 3)) alt_down = 1;
			if (!strncmp(s, "Control", 7)) ctrl_down = 1;
			if (!strncmp(s, "Shift", 5)) shift_down = 1;

			if(!strcmp(s, "q") && ctrl_down && alt_down) {
				destroy_clients(*clnt_keyboard, *clnt_mouse); 
				quit=1;
			} else if(!strcmp(s, "Up") && ctrl_down && shift_down) {
				host = get_next_host(cur_host_index, argc,
						     argv, 1);
					
				switch_hosts(host, dpy, clnt_keyboard,
					     clnt_mouse, cur_host_index,
					     argc,  argv);
				quit = 1;
			} else if (!strcmp(s, "Down") && ctrl_down
				   && shift_down) {

				host = get_next_host(cur_host_index, argc,
						     argv, 0); 

				switch_hosts(host, dpy, clnt_keyboard,
					     clnt_mouse, cur_host_index,
					     argc, argv);
				quit = 1;

			}

			break;
			
			
		case KeyRelease: 
			
			kc = ((XKeyReleasedEvent*)&ev)->keycode;
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, *clnt_keyboard); 
			
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			if (!strncmp(s, "Alt", 3)) alt_down = 0;
			if (!strncmp(s, "Control", 7)) ctrl_down = 0;
			if (!strncmp(s, "Shift", 5)) shift_down = 0;
			
			
			break;

		 
		} 
		
	}/*end while */

}


/* TODO: Change argc, argv...*/
void switch_hosts(char *host, Display *dpy, CLIENT **clnt_keyboard,
		  CLIENT **clnt_mouse, int *cur_host_index,
		  int argc, char *argv[]) 
{

	if (hostType == LOCALHOST) {
		ungrab_keycombos(dpy);
	} else if (hostType == REMOTEHOST) {
		ungrab_hardware(dpy);
		destroy_clients(*clnt_keyboard, *clnt_mouse); 
	}

	if (strncmp(host, "localhost", 10) == 0) {
		printf("is localhost\n");
		hostType = LOCALHOST; 
		grab_keycombos(dpy);   /* TODO: put this line back in */
		localhostLoop(dpy, clnt_keyboard, clnt_mouse,
			      cur_host_index, argc, argv); 
		/*create_clients(host, clnt_keyboard, clnt_mouse); */
		/*grab_hardware(dpy); /* TODO: take this and above line out*/
		/*remoteHostLoop(dpy, clnt_keyboard, clnt_mouse,
			       cur_host_index, argc, argv); /* TODO: take out*/ 
	} else {
		printf("isn't localhost\n");
		hostType = REMOTEHOST;
		create_clients(host, clnt_keyboard, clnt_mouse);
		grab_hardware(dpy);
		remoteHostLoop(dpy, clnt_keyboard, clnt_mouse,
			       cur_host_index, argc, argv);
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
     


	/*******************************************/
	/** XCapture stuff **/
	if((dpy = XOpenDisplay(NULL)) == NULL) {
		printf("Fialing here\n");
		perror(argv[0]); 
		exit(1); 
	} 

	/*
	  grab_hardware(dpy); */

	switch_hosts(host, dpy, &clnt_keyboard, &clnt_mouse, &cur_host_index,
		     argc, argv);

/*
	while(!quit) { 
		printf("in while\n"); 
		XEvent ev; 
		
		XNextEvent(dpy, &ev); 

		switch (ev.type) {
		case ButtonPress:
			button = ((XButtonPressedEvent*)&ev)->button;
			mouse_1_arg.button_event = 1;
			mouse_1_arg.on_press = 1;
			mouse_1_arg.button = button; 
			mouse_1(&mouse_1_arg, clnt_mouse);
			break; 
		case ButtonRelease:
			mouse_1_arg.button_event = 1;
			mouse_1_arg.on_press = 0;
			mouse_1_arg.button = 
				((XButtonPressedEvent*)&ev)->button;
			mouse_1(&mouse_1_arg, clnt_mouse);
			break; 
		case MotionNotify:
			x = ((XPointerMovedEvent*)&ev)->x;
			y = ((XPointerMovedEvent*)&ev)->y;
		        mouse_1_arg.x = x;
			mouse_1_arg.y = y;
			mouse_1_arg.button_event = 0; 
			mouse_1(&mouse_1_arg, clnt_mouse);
			break;

		case KeyPress: 
			
			kc = ((XKeyPressedEvent*)&ev)->keycode;
			keyboard_1_arg.on_press = 1;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, clnt_keyboard);
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			break;
			
			
		case KeyRelease: 
			
			kc = ((XKeyReleasedEvent*)&ev)->keycode;
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, clnt_keyboard); 
			
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			if(!strcmp(s, "q")) quit=1;
			if(!strcmp(s, "Up")) {
				host = get_next_host(&cur_host_index, argc,
						     argv, 1);
				
				printf("host%s\n", argv[cur_host_index+1]);
				switch_hosts(host, dpy, &clnt_keyboard,
					     &clnt_mouse);
			} else if (!strcmp(s, "Down")) {

				host = get_next_host(&cur_host_index, argc,
						     argv, 0); 
				printf("host%s\n", argv[cur_host_index+1]);
				switch_hosts(host, dpy, &clnt_keyboard,
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

	/*destroy_clients(clnt_keyboard, clnt_mouse); */
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
