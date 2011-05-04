/* Remote average client code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
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

static char *localhostname; 

struct hosts_data_s{
	int num_hosts;
	char **hostnames;	
	int cur_host_index; 
};

typedef struct hosts_data_s *hosts_data_t;


void switch_hosts(int which_host, Display *dpy, hosts_data_t hosts_data); 

/* Destroys the given keyboard and mouse clients */
void destroy_clients(CLIENT *clnt_keyboard, CLIENT *clnt_mouse, CLIENT *clnt_image) 
{
        clnt_destroy( clnt_keyboard );
	clnt_destroy( clnt_mouse );
	clnt_destroy( clnt_image ); 
} /* end destroy_clients */

/* Creates the keyboard and mouse clients based on given host name. */
void create_clients(char *host, CLIENT **clnt_keyboard, CLIENT **clnt_mouse, CLIENT **clnt_image)
{
        *clnt_keyboard = clnt_create(host, KEYBOARDPROG, KEYBOARDVERS, "tcp");
	if (*clnt_keyboard == NULL) {
		clnt_pcreateerror(host);
                exit(1);
	}
	*clnt_mouse = clnt_create(host, MOUSEPROG, MOUSEVERS, "tcp");
        if (*clnt_mouse == NULL) {
                clnt_pcreateerror(host);
                exit(1);
	}
	*clnt_image = clnt_create(host, IMAGEPROG, IMAGEVERS, "tcp");
        if (*clnt_image == NULL) {
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

/* Run loop for when on localhost.  Check for key combinations being
   pressed and give client full control of their own computer */
void localhostLoop(Display *dpy, hosts_data_t hosts_data)
{
	int quit_loop = 0, quit_program = 0, which_host = 0; 
	int kc;
	XEvent ev;
	char *s;
	grab_keycombos(dpy); 
	while(!quit_loop) {
		XNextEvent(dpy, &ev);
		switch (ev.type) {
		case KeyPress:
			kc = ((XKeyPressedEvent*)&ev)->keycode; 
			s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
			if(!strncmp(s, "q", 2)) {
				quit_loop = 1;
				quit_program = 1; 
			} else if(!strncmp(s, "Up", 3)) {
				which_host = NEXT; 
				quit_loop = 1;
			} else if (!strncmp(s, "Down", 5)) {
				which_host = PREV; 
				quit_loop = 1;
			} else if (!strncmp(s, "l", 2)) {
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

/* Perform necessary actions for when a key is pressed when running on
 remote server */
void remoteHost_KeyPress(Display *dpy, XEvent *ev, CLIENT *clnt_keyboard,
			 int *alt_down, int *ctrl_down, int *shift_down,
			 int *quit_loop, int *quit_program, int *which_host)
{
	key_input keyboard_1_arg; 
	int kc  = ((XKeyPressedEvent*)ev)->keycode;
	char *s;
	
	keyboard_1_arg.on_press = 1;
	keyboard_1_arg.keycode = kc;
	keyboard_1(&keyboard_1_arg, clnt_keyboard);
	s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0));
	if (!strncmp(s, "Alt", 3)) *alt_down = 1;
	if (!strncmp(s, "Control", 7)) *ctrl_down = 1;
	if (!strncmp(s, "Shift", 5)) *shift_down = 1;
	
	if(!strncmp(s, "q", 1) && *ctrl_down && *alt_down) {
		*quit_loop = 1;
		*quit_program = 1;
	} else if(!strcmp(s, "Up") && ctrl_down && shift_down) {
		*which_host = NEXT;
		*quit_loop = 1;
	} else if (!strcmp(s, "Down") && *ctrl_down
		   && *shift_down) {
		*which_host = PREV;
		*quit_loop = 1;
	} else if (!strcmp(s, "l") && *ctrl_down
		   && *shift_down) {
		*which_host = LOCAL; 
		*quit_loop = 1;
	}

}

/* Perform necessary actions for when a key is released when running on
 remote server */
void remoteHost_KeyRelease(Display *dpy, XEvent *ev, CLIENT *clnt_keyboard,
			 int *alt_down, int *ctrl_down, int *shift_down)
{
	key_input keyboard_1_arg; 
	int kc = ((XKeyReleasedEvent*)ev)->keycode;
	char *s; 
	keyboard_1_arg.on_press = 0;
	keyboard_1_arg.keycode = kc;
	keyboard_1(&keyboard_1_arg, clnt_keyboard); 
	
	s = XKeysymToString(XKeycodeToKeysym(dpy, kc, 0)); 
	if (!strncmp(s, "Alt", 3)) *alt_down = 0;
	if (!strncmp(s, "Control", 7)) *ctrl_down = 0;
	if (!strncmp(s, "Shift", 5)) *shift_down = 0;
}

/* Perform necessary actions for when button is pressed or released  when
   running on remote server */
void remoteHost_Button(Display *dpy, XEvent *ev, CLIENT *clnt_mouse,
			 int on_press)
{
	mouse_input mouse_1_arg;
	/* XButtonPressedEvent and XButtonReleasedEvent have same structure */
	int button = ((XButtonPressedEvent*)ev)->button;
	mouse_1_arg.button_event = 1;
	mouse_1_arg.on_press = on_press;
	mouse_1_arg.button = button; 
	mouse_1(&mouse_1_arg, clnt_mouse);
}

/* Perform necessary actions for when mouse is moved  when
   running on remote server */
void remoteHost_MotionNotify(Display *dpy, XEvent *ev, CLIENT *clnt_mouse)
{
	int x, y;
	mouse_input mouse_1_arg;
	x = ((XPointerMovedEvent*)ev)->x;
	y = ((XPointerMovedEvent*)ev)->y;
	mouse_1_arg.x = x;
	mouse_1_arg.y = y;
	mouse_1_arg.button_event = 0; 
	mouse_1(&mouse_1_arg, clnt_mouse);
}

/* TODO: get proper inputs in here (change argv and argc)*/
void remoteHostLoop(Display *dpy, hosts_data_t hosts_data) 
{
	int quit_program = 0;
	int quit_loop = 0; 
	key_input keyboard_1_arg; 
	mouse_input mouse_1_arg;
	image_input image_1_arg; 
	XEvent ev;
	CLIENT *clnt_keyboard;
	CLIENT *clnt_mouse;
	CLIENT *clnt_image; 
	int button;
	int x, y, kc;
	char *s; 
	int shift_down = 0, ctrl_down = 0, alt_down = 0;
	int which_host = 0;
	char *host = hosts_data->hostnames[hosts_data->cur_host_index];
	


	create_clients(host, &clnt_keyboard, &clnt_mouse, &clnt_image);
	image_1_arg.init = 1;
	image_1_arg.host = localhostname; 
	image_1(&image_1_arg, clnt_image);
	grab_hardware(dpy);

	while(!quit_loop) { 
		XNextEvent(dpy, &ev); 
		switch (ev.type) {
		case ButtonPress:
			remoteHost_Button(dpy, &ev, clnt_mouse, 1); 
			break; 
		case ButtonRelease:
			remoteHost_Button(dpy, &ev, clnt_mouse, 0); 
			break; 
		case MotionNotify:
			remoteHost_MotionNotify(dpy, &ev, clnt_mouse);
			break;
		case KeyPress:
			remoteHost_KeyPress(dpy, &ev, clnt_keyboard, &alt_down,
					    &ctrl_down, &shift_down, &quit_loop,
					    &quit_program, &which_host); 
			break;
			
		case KeyRelease:
			remoteHost_KeyRelease(dpy, &ev, clnt_keyboard,
					      &alt_down, &ctrl_down,
					      &shift_down); 
			break;
		} 
		
	}/*end while */
	ungrab_hardware(dpy);
	image_1_arg.init = 0; 
	image_1(&image_1_arg, clnt_image);
	destroy_clients(clnt_keyboard, clnt_mouse, clnt_image); 

	if (!quit_program) {
		switch_hosts(which_host, dpy, hosts_data); 
	}
}

/* Open display and switch to first host */
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


/* Return malloced copy of given string
 */
char *copy_string(char *to_copy)
{
	int str_len = strlen(to_copy) + 1;
	char *copied_str; 
	if ((copied_str = (char *)malloc(str_len * sizeof(char))) == NULL) {
		perror("copy_string");
		exit(errno);
	}
	strncpy(copied_str, to_copy, str_len);
	copied_str[str_len - 1] = '\0';
	return copied_str; 
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
	localhostname = copy_string((char *)host_info->h_name); 
	
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

/* Create list of hostnames from main method arguments
 */
char **create_hostname_list(int argc, char *argv[])
{
	char **hostnames;
	int len, i = 0;
	char hostname[MAXHOSTLENGTH];
	struct hostent *host_info;
	gethostname(hostname, MAXHOSTLENGTH);
	host_info = gethostbyname(hostname);
	if ((hostnames = (char **) malloc((argc-1) * sizeof(char *))) == NULL) {
		perror("create_hostname_list");
		exit(errno);
	}
	for (i = 0; i < argc-1; i++) {
		if (is_localhost(argv[i+1], host_info)) {
			hostnames[i] = copy_string("localhost"); 
		} else {
			hostnames[i] = copy_string(argv[i+1]); 
		}
	}
	return hostnames; 
}

/* Free hostnames list */
void destroy_hostname_list(int num_hosts, char **hostnames)
{
	int i = 0;
	for (i = 0; i < num_hosts; i++) {
		free(hostnames[i]); 
	}
	free(hostnames);
}

/* return host_data struct based on main method arguments  */
hosts_data_t create_hosts_data(int argc, char* argv[])
{
	hosts_data_t hosts_data;

	if ((hosts_data = (hosts_data_t) malloc(sizeof(struct hosts_data_s)))
	     == NULL ) {
		perror("main"); 
		exit(errno); 
	}
	hosts_data->num_hosts = argc-1;
	hosts_data->cur_host_index = -1;
	hosts_data->hostnames = create_hostname_list(argc, argv);

	return hosts_data; 
}

/* Free space from host_data struct
 */
void destroy_hosts_data(hosts_data_t hosts_data)
{
	destroy_hostname_list(hosts_data->num_hosts, hosts_data->hostnames);
	free(hosts_data); 
}

/* Check for proper command line arguments and start program
 */
int main( int argc, char* argv[] )
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
	return 0; 
}

/* Switch to host indicated (NEXT, PREV, or LOCAL) and run appropriate
   loop.
 */
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
		     "localhost", 10)) {
		localhostLoop(dpy, hosts_data);
	} else {
		remoteHostLoop(dpy, hosts_data);
	}
}
