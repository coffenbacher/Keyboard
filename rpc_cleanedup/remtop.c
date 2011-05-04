/* Remote average client code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 */

/*
  CURRENTLY WORKING ON:
  TODO: Just create/destroy the two clients in remoteLoop ... only place you
  need them!
 */

/*
TODO: strcmp vs. strncmp
 */

/*
TODO: struct for clients
 */

/*
 TODO: Struct for host_data
 */

/*
 TODO: split switch_host to switch_to_prev_host, next_host, and localhost
 */

/*
TODO: destroy clients when quitting...
 */

#include "remtop.h"
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>

#define XC_arrow 2
#define NOHOST 0
#define LOCALHOST -1
#define REMOTEHOST 1
#define MAXHOSTLENGTH 1024

#define NEXT 1
#define PREV -1
#define LOCAL 0

int hostType = NOHOST; 

struct hosts_data_s{
	int cur_host_type;
	int num_hosts;
	char **hostnames;	
	int cur_host_index; 
};

typedef struct hosts_data_s *hosts_data_t;


void switch_hosts(int which_host, Display *dpy, hosts_data_t hosts_data); 

/* Destroys the given keyboard and mouse clients */
void destroy_clients(CLIENT *clnt_keyboard, CLIENT *clnt_mouse) 
{
        clnt_destroy( clnt_keyboard );
	clnt_destroy( clnt_mouse ); 
} /* end destroy_clients */

/* Creates the keyboard and mouse clients based on given host name. */
void create_clients(char *host, CLIENT **clnt_keyboard, CLIENT **clnt_mouse)
{
        *clnt_keyboard = clnt_create(host, KEYBOARDPROG, KEYBOARDVERS, "udp");
	if (*clnt_keyboard == NULL) {
		clnt_pcreateerror(host);
                exit(1);
	}
	*clnt_mouse = clnt_create(host, MOUSEPROG, MOUSEVERS, "udp");
        if (*clnt_mouse == NULL) {
                clnt_pcreateerror(host);
                exit(1);
	} 
} /* end create_clients */

/* grab single keycombo with given modifiers and keycode */
void grab_keycombo(Display *dpy, int keycode, unsigned int modifiers) {
	int res; 
	res = XGrabKey(dpy, keycode, modifiers, DefaultRootWindow(dpy),
		       True, GrabModeAsync, GrabModeAsync);
	if (res == BadAccess) {
		fprintf(stderr, "BadAccess error with XGrabKey\n");
		exit(1); 
	} else if (res == BadValue) {
		fprintf(stderr, "BadValue error with XGrabKey\n");
		exit(1); 
	} else if (res == BadWindow) {
		fprintf(stderr, "BadWindow error with XGrabKey\n");
		exit(1); 
	}
}

/* Grabs all required key combinations so can detect when they have
   occurred. */
void grab_keycombos(Display *dpy)
{
	int kc, res;
	unsigned int modifiers;
	modifiers = ControlMask | Mod1Mask;
	kc = XKeysymToKeycode(dpy, XStringToKeysym("q"));
	grab_keycombo(dpy, kc, modifiers);
	
	modifiers = ControlMask | ShiftMask;
	kc = XKeysymToKeycode(dpy, XStringToKeysym("Up"));
	grab_keycombo(dpy, kc, modifiers);

	kc = XKeysymToKeycode(dpy, XStringToKeysym("Down"));
	grab_keycombo(dpy, kc, modifiers);
	
	kc = XKeysymToKeycode(dpy, XStringToKeysym("l"));
	grab_keycombo(dpy, kc, modifiers);	
}

/* ungrabs all key combinations  */
void ungrab_keycombos(Display *dpy)
{
	int res = XUngrabKey(dpy, AnyKey, AnyModifier, DefaultRootWindow(dpy));
	if (res == BadValue) {
		fprintf(stderr, "BadValue error with XUngrabKey\n");
		exit(1); 
	} else if (res == BadWindow) {
		fprintf(stderr, "BadWindow error with XUngrabKey\n");
		exit(1); 
	}
}

/* grab keyboard and mouse */
void grab_hardware(Display *dpy)
{
	Cursor cursor =  XCreateFontCursor(dpy, XC_arrow); 
	int res = XGrabKeyboard(dpy, DefaultRootWindow(dpy), True,
				GrabModeAsync, GrabModeAsync, CurrentTime);
	if (res == BadValue) {
		fprintf(stderr, "BadValue error with XGrabKeyboard\n");
		exit(1); 
	} else if (res == BadWindow) {
		fprintf(stderr, "BadWindow error with XGrabKeyboard\n");
		exit(1); 
	}
	res = XGrabPointer(dpy, DefaultRootWindow(dpy), True,
			   PointerMotionMask| ButtonPressMask |
			   ButtonReleaseMask, GrabModeAsync, GrabModeAsync,
			   DefaultRootWindow(dpy), cursor, CurrentTime);
	if (res == BadCursor) {
		fprintf(stderr, "BadCursor error with XGrabPointer"); 
	} else if (res == BadValue) {
		fprintf(stderr, "BadValue error with XGrabPointer\n");
		exit(1); 
	} else if (res == BadWindow) {
		fprintf(stderr, "BadWindow error with XGrabPointer\n");
		exit(1); 
	}
}

/* ungrab keyboard and mouse */
void ungrab_hardware(Display *dpy) 
{
	XUngrabPointer(dpy, CurrentTime);  
	XUngrabKeyboard(dpy, CurrentTime); 
}


void localhostLoop(Display *dpy, hosts_data_t hosts_data)
{
	int quit_loop = 0, quit_program = 0; 
	int kc;
	XEvent ev;
	char *s;
	int which_host = 0;
	
	grab_keycombos(dpy); 
	while(!quit_loop) {
		XNextEvent(dpy, &ev);
		switch (ev.type) {
		case KeyPress:
			kc = ((XKeyPressedEvent*)&ev)->keycode; 
			printf("\n%x\n", kc);
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			
			if(s) printf("KEY: %s\n", s);
			/* TODO: make sure shouldn't be strncmp*/
			if(!strcmp(s, "q")) {
				
				quit_loop = 1;
				quit_program = 1; 
			} else if(!strcmp(s, "Up")) {
				/*host = get_next_host(cur_host_index, argc,
				  argv, 1);*/
				which_host = NEXT; 
				
				quit_loop = 1;
			} else if (!strcmp(s, "Down")) {
				/*host = get_next_host(cur_host_index, argc,
						     argv, 0);
						     switch_to_rem = 1; 	*/

				which_host = PREV; 
				quit_loop = 1;
			} else if (!strcmp(s, "l")) {
				/*switch_to_loc = 1;*/
				
				which_host = LOCAL; 
				quit_loop = 1; 
			}
			break;
		}
	}
	ungrab_keycombos(dpy);
	
	if (!quit_program) {
		switch_hosts(which_host, dpy, hosts_data);   
	}
}


/* TODO: get proper inputs in here (change argv and argc)*/
void remoteHostLoop(Display *dpy, hosts_data_t hosts_data) 
{
	int quit_program = 0;
	int quit_loop = 0; 
	key_input keyboard_1_arg; 
	mouse_input mouse_1_arg;
	XEvent ev;
	CLIENT *clnt_keyboard;
	CLIENT *clnt_mouse;
	int button;
	int x, y, kc;
	char *s; 
	int shift_down = 0, ctrl_down = 0, alt_down = 0;
	int which_host = 0;
	char *host = hosts_data->hostnames[hosts_data->cur_host_index]; 

	grab_hardware(dpy);
	create_clients(host, &clnt_keyboard, &clnt_mouse); 
	printf("created clients\n");	

	while(!quit_loop) { 

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
			if (!strncmp(s, "Alt", 3)) alt_down = 1;
			if (!strncmp(s, "Control", 7)) ctrl_down = 1;
			if (!strncmp(s, "Shift", 5)) shift_down = 1;

			if(!strncmp(s, "q", 1) && ctrl_down && alt_down) {
				quit_loop = 1;
				quit_program = 1;
			} else if(!strcmp(s, "Up") && ctrl_down && shift_down) {
				which_host = NEXT;
				quit_loop = 1;
			} else if (!strcmp(s, "Down") && ctrl_down
				   && shift_down) {
				which_host = PREV;
				quit_loop = 1;
			} else if (!strcmp(s, "l") && ctrl_down
				   && shift_down) {
				which_host = LOCAL; 
				quit_loop = 1;
			}
			break;
			
		case KeyRelease: 
			kc = ((XKeyReleasedEvent*)&ev)->keycode;
			keyboard_1_arg.on_press = 0;
			keyboard_1_arg.keycode = kc;
			keyboard_1(&keyboard_1_arg, clnt_keyboard); 
			
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			if (!strncmp(s, "Alt", 3)) alt_down = 0;
			if (!strncmp(s, "Control", 7)) ctrl_down = 0;
			if (!strncmp(s, "Shift", 5)) shift_down = 0;
			break;
		} 
		
	}/*end while */
	ungrab_hardware(dpy);
	destroy_clients(clnt_keyboard, clnt_mouse);

	if (!quit_program) {
		switch_hosts(which_host, dpy, hosts_data); 
	}
	

}

void desktopprog_1(hosts_data_t hosts_data)
{
	Display *dpy;

	if((dpy = XOpenDisplay(NULL)) == NULL) {
		perror("desktopprog_1"); 
		exit(1); 
	} 
	switch_hosts(NEXT, dpy, hosts_data);
	if (XCloseDisplay(dpy)) { 
		perror("desktopprog_1"); 
		exit(1); 
	} 
}

/* Borrowed the line:
   "printf( "%s ", inet_ntoa( *( struct in_addr*)
   ( hp -> h_addr_list[i])));"
   from http://paulschreiber.com/blog/2005/10/28/
   simple-gethostbyname-example/
   
   I added the (char *) parsing to make it work properly */
/*
  Return true if given host name is actually just the local host.
  Needs to be given the hostent for the localhost.

  Note: I used the example code in:
    http://paulschreiber.com/blog/2005/10/28/simple-gethostbyname-example/
  to help write one of the lines.  It told me that I needed to cast addr to
  inet_ntoa(* (struct in_addr*).  I still had to add the (char *) to that line
  to make it work fully.  
 */
int is_localhost(char *host, struct hostent *host_info)
{
	char *addr;
	int counter = 0;
	if (!strncmp(host, "localhost", 10)) return 1; 
	
	/*if (!strncmp(host, host_info->h_name, MAXHOSTLENGTH)) return 1;*/

	while ((addr =  host_info->h_addr_list[counter++]) != NULL) {
		if (!strncmp(host, (char *)inet_ntoa(
				     *( struct in_addr*)(addr)),
			    MAXHOSTLENGTH)) return 1;
	}
	counter = 0;
	while ((addr =  host_info->h_aliases[counter++]) != NULL) {
		if (!strncmp(host, (char *)(addr), MAXHOSTLENGTH)) return 1; 
		
	}
	return 0;
}

char **create_hostname_list(int argc, char *argv[])
{
	char **hostnames;
	int i = 0;
	int len;
	char hostname[MAXHOSTLENGTH];
	struct hostent *host_info;

	/* TODO: Check for errors */
	gethostname(hostname, MAXHOSTLENGTH);
	printf("%s\n", hostname);
	host_info = gethostbyname(hostname);
	hostnames = (char **) malloc((argc-1) * sizeof(char *));

	for (i = 0; i < argc-1; i++) {
		if (is_localhost(argv[i+1], host_info)) {
			hostnames[i] = (char *)malloc((strlen("localhost")+1)*
						      sizeof(char));
			strcpy(hostnames[i], "localhost"); 
		} else {
			hostnames[i] = (char *)
				malloc((strlen(argv[i+1]) + 1) * sizeof(char));
			strcpy(hostnames[i], argv[i+1]); 
		}
	}
	return hostnames; 
}


void destroy_hostname_list(int num_hosts, char **hostnames)
{
	int i = 0;
	for (i = 0; i < num_hosts; i++) {
		free(hostnames[i]); 
	}
	free(hostnames);
}

hosts_data_t create_hosts_data(int argc, char* argv[])
{
	hosts_data_t hosts_data;

	if ((hosts_data = (hosts_data_t) malloc(sizeof(struct hosts_data_s)))
	     == NULL ) {
		perror("main"); 
		exit(errno); 
	}
	hosts_data->cur_host_type = NOHOST;
	hosts_data->num_hosts = argc-1;
	hosts_data->cur_host_index = -1;
	hosts_data->hostnames = create_hostname_list(argc, argv);

	return hosts_data; 
}

void destroy_hosts_data(hosts_data_t hosts_data)
{
	destroy_hostname_list(hosts_data->num_hosts, hosts_data->hostnames);
	free(hosts_data); 
}

main( int argc, char* argv[] )
{
        char *host;
	hosts_data_t hosts_data;

        if (argc < 2) {
                printf("usage: %s server_host value \n",
                        argv[0]);
                exit(EINVAL);
        }
        if (argc > MAXHOSTS + 1) {
                printf("Too many input values\n");
                exit(EINVAL);
        }

	hosts_data = create_hosts_data(argc, argv); 
        desktopprog_1(hosts_data);

	destroy_hosts_data(hosts_data);
	
}


void switch_hosts(int which_host, Display *dpy, hosts_data_t hosts_data) {
	int next_host_index;
	if (which_host == LOCAL) {
		localhostLoop(dpy, hosts_data);
		return; 
	}

	next_host_index = ((hosts_data->cur_host_index + which_host
			     + hosts_data->num_hosts) %
			   hosts_data->num_hosts);
	
	hosts_data->cur_host_index = next_host_index;

	if (!strncmp(hosts_data->hostnames[next_host_index],
		     "localhost", MAXHOSTLENGTH)) {
		localhostLoop(dpy, hosts_data);
	} else {
		remoteHostLoop(dpy, hosts_data);
	}
}
