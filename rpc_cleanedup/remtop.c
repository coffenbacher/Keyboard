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




void switch_hosts_new(int which_host, Display *dpy, hosts_data_t hosts_data); 
void switch_hosts_unused(char *host, Display *dpy, /*CLIENT **clnt_keyboard,
					      CLIENT **clnt_mouse,*/ int *cur_host_index,
		  int argc, char *argv[]);

char *get_next_host(int *cur_host_index, int argc, char *argv[], int up)
{
	*cur_host_index = get_next_host_index(*cur_host_index, argc, up);
	/* TODO: Check if legit host */
	return argv[*cur_host_index]; 
	
}

int get_next_host_index(int cur_host_index, int num_hosts, int up)
{

	int next_host_index;

	printf("num hosts: %d\n", num_hosts); 
	if (up) {
		next_host_index = (cur_host_index - 1 + num_hosts)%num_hosts; 
	} else {
		next_host_index = (cur_host_index + 1 + num_hosts)%num_hosts;
	}
	printf("next host index: %d\n", next_host_index); 
	return next_host_index;
}


void destroy_clients(CLIENT *clnt_keyboard, CLIENT *clnt_mouse) 
{
	/* TODO: confirm that we aren't supposed to be passing ** */
	
        clnt_destroy( clnt_keyboard );
	clnt_destroy( clnt_mouse ); 
} /* end destroy_clients */

/*
 Creates the keyboard and mouse clients based on given host name.
 */
void create_clients(char *host, CLIENT **clnt_keyboard, CLIENT **clnt_mouse)
{
/*
	printf("in create clients %s\n", host);
	destroy_clients(*clnt_keyboard, *clnt_mouse); 
	printf("past destroy\n"); */
	
        *clnt_keyboard = clnt_create(host, KEYBOARDPROG, KEYBOARDVERS, "udp");
	printf("created keyboard\n"); 
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

/*
  Destroys the keyboard and mouse clients.
 */

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
		fprintf(stderr, "Bad Acces with Down\n"); 
	}
	kc = XKeysymToKeycode(dpy, XStringToKeysym("l"));
	res = XGrabKey(dpy, kc, ControlMask | ShiftMask,
	    DefaultRootWindow(dpy), True, GrabModeAsync, 
	    GrabModeAsync);
	if (res == BadAccess) {
		fprintf(stderr, "Bad Acces with l\n"); 
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

/*void localhostLoop(Display *dpy, *//*CLIENT **clnt_keyboard, CLIENT **clnt_mouse,*/
/*int *cur_host_index, int argc, char *argv[]) */


void localhostLoop(Display *dpy, hosts_data_t hosts_data)
{
	int quit_loop = 0;
	int quit_program = 0; 
	int kc;
	XEvent ev;
	char *host;
	char *s;
	/*int switch_to_rem = 0;
	  int switch_to_loc = 0;*/
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
		switch_hosts_new(which_host, dpy, hosts_data);   
	}
	
	/*if (switch_to_rem) {
		switch_hosts(host, dpy, /*clnt_keyboard,
		clnt_mouse, */ /*cur_host_index,
			     argc,  argv);
	} else if (switch_to_loc) {
		switch_hosts("localhost", dpy, /*clnt_keyboard,
						 clnt_mouse, */ /*cur_host_index,
			     argc,  argv);
			     } */
	
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
	/*int switch_to_rem = 0;
	  int switch_to_loc = 0; */
	printf("starting loop\n"); 
/*
	CLIENT *clnt_keyboard;
	CLIENT *clnt_mouse;
	
	create_clients(host, &clnt_keyboard, &clnt_mouse);*/
	grab_hardware(dpy);
	create_clients(host, &clnt_keyboard, &clnt_mouse); 
	printf("created clients\n");	
	/*TODO: change key_input and mouse_input to be * so more like what
	 we learned -> don't forget to malloc! */
	
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
				/*destroy_clients(*clnt_keyboard, *clnt_mouse);*/ 
				quit_loop = 1;
				quit_program = 1;
			} else if(!strcmp(s, "Up") && ctrl_down && shift_down) {
				/*host = get_next_host(cur_host_index, argc,
						     argv, 1);
						     switch_to_rem = 1; 	*/
				which_host = NEXT;

				quit_loop = 1;
			} else if (!strcmp(s, "Down") && ctrl_down
				   && shift_down) {

				/*host = get_next_host(cur_host_index, argc,
						     argv, 0); 
						     switch_to_rem = 1;*/
				which_host = PREV;
				quit_loop = 1;

			} else if (!strcmp(s, "l") && ctrl_down
				   && shift_down) {

				which_host = LOCAL; 
				/*switch_to_loc = 1; */

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
		switch_hosts_new(which_host, dpy, hosts_data); 
	}
	
	/*if (switch_to_rem) {
		switch_hosts(host, dpy, /*clnt_keyboard,
					  clnt_mouse,*/ /*cur_host_index,
			     argc,  argv);
	} else if (switch_to_loc) {
		switch_hosts("localhost", dpy, /*clnt_keyboard,
		clnt_mouse, */ /*cur_host_index,
			     argc, argv);
			     } */
}



/* TODO: Change argc, argv...*/
void switch_hosts_unused(char *host, Display *dpy, /*CLIENT **clnt_keyboard,  CLIENT **clnt_mouse,*/ int *cur_host_index,
		  int argc, char *argv[]) 
{

	if (hostType == LOCALHOST) {
		/*ungrab_keycombos(dpy);*/
	} else if (hostType == REMOTEHOST) {
		/*ungrab_hardware(dpy);*/
		/*destroy_clients(*clnt_keyboard, *clnt_mouse); */
	}

	if (strncmp(host, "localhost", 10) == 0) {
		printf("is localhost\n");
		hostType = LOCALHOST; 
		/*grab_keycombos(dpy);  */ /* TODO: put this line back in */


		/**************************************************************/
		/* localhostLoop(dpy, /*clnt_keyboard, clnt_mouse,*/
		/*		     cur_host_index, argc, argv);  */
		/**************************************************************/


		/*create_clients(host, clnt_keyboard, clnt_mouse); */
		/*grab_hardware(dpy); /* TODO: take this and above line out*/
		/*remoteHostLoop(dpy, clnt_keyboard, clnt_mouse,
			       cur_host_index, argc, argv); /* TODO: take out*/ 
	} else {
		printf("isn't localhost\n");
		hostType = REMOTEHOST;
		/*create_clients(host, clnt_keyboard, clnt_mouse); */
		/*grab_hardware(dpy);*/

		/*******************************************************************/
		/**** remoteHostLoop(dpy, /*clnt_keyboard, clnt_mouse,*/
			       /****cur_host_index, argc, argv, host); ******/
		/******************************************************************/	       
	}
			
}

void get_localIPs()
{
	int MAXHOSTNAMELENGTH = 2040;
	char hostname[MAXHOSTNAMELENGTH];
	struct hostent *host_info;
	char *addr;
	char **h_addr_list;
	int counter = 0; 
	/* TODO: Check for errors */
	gethostname(hostname, MAXHOSTNAMELENGTH);
	printf("%s\n", hostname);
	host_info = gethostbyname(hostname);

	/*while ((addr = h_addr_list[counter++]) != NULL) {
		printf("%s\n", addr); 
		}*/
	printf("%s\n", host_info->h_name);
	

	/* Borrowed the line:
	   "printf( "%s ", inet_ntoa( *( struct in_addr*)
	   ( hp -> h_addr_list[i])));"
	 from http://paulschreiber.com/blog/2005/10/28/
	 simple-gethostbyname-example/
	
	I added the (char *) parsing to make it work properly */
	while ((addr =  host_info->h_addr_list[counter++])!=NULL) {
		printf("%s\n",
		       (char *)inet_ntoa( *( struct in_addr*)
					  (addr))); 
	}
	counter = 0;
	while ((addr =  host_info->h_aliases[counter++])!=NULL) {
		printf("%s\n",
		       (char *)(addr)); 
	}
}

void desktopprog_1( char* host, int argc, char *argv[], hosts_data_t hosts_data)
{
/*	      CLIENT *clnt_keyboard, *clnt_mouse;       */ 
        
	key_input keyboard_1_arg; 
	mouse_input mouse_1_arg;
	int cur_host_index = -1; 
	Display *dpy;
	int quit = 0;
	char *s;
	int on_press; 
	int kc, button; 

	int x, y;
	int isLocalhost = 0;

/*	create_clients("localhost", &clnt_keyboard, &clnt_mouse);
	destroy_clients(clnt_keyboard, clnt_mouse); */
   
	get_localIPs(); 

	/*******************************************/
	/** XCapture stuff **/
	if((dpy = XOpenDisplay(NULL)) == NULL) {
		printf("Fialing here\n");
		perror(argv[0]); 
		exit(1); 
	} 

	/*
	  grab_hardware(dpy); */

	switch_hosts_new(NEXT, dpy, /*&clnt_keyboard, &clnt_mouse, */hosts_data);

	
	
/*	ungrab_hardware(dpy); */
	
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

int is_localhost(char *host, struct hostent *host_info)
{
	char *addr;
	int counter = 0;
	if (!strncmp(host, "localhost", 10)) return 1; 
	
	/*if (!strncmp(host, host_info->h_name, MAXHOSTLENGTH)) return 1;*/

	/* Borrowed the line:
	   "printf( "%s ", inet_ntoa( *( struct in_addr*)
	   ( hp -> h_addr_list[i])));"
	 from http://paulschreiber.com/blog/2005/10/28/
	 simple-gethostbyname-example/
	
	I added the (char *) parsing to make it work properly */
	while ((addr =  host_info->h_addr_list[counter++])!=NULL) {
		if (!strncmp(host, (char *)inet_ntoa(
				     *( struct in_addr*)(addr)),
			    MAXHOSTLENGTH)) return 1;

	}
	
	counter = 0;
	while ((addr =  host_info->h_aliases[counter++])!=NULL) {
		if (!strncmp(host, (char *)(addr), MAXHOSTLENGTH)) return 1; 
		
	}
	return 0;
}

char **create_hostname_list(int argc, char *argv[])
{
	char **hostnames;
	int i = 0;
	int len;

	int MAXHOSTNAMELENGTH = 2040;
	char hostname[MAXHOSTNAMELENGTH];
	struct hostent *host_info;
	char *addr;
	char **h_addr_list;
	int counter = 0; 
	/* TODO: Check for errors */
	gethostname(hostname, MAXHOSTNAMELENGTH);
	printf("%s\n", hostname);
	host_info = gethostbyname(hostname);
	hostnames = (char **) malloc((argc-1) * sizeof(char *));

	for (i = 0; i < argc-1; i++) {
		if (is_localhost(argv[i+1], host_info)) {
			hostnames[i] = (char *)malloc((strlen("localhost")+1)*
						      sizeof(char));
			strcpy(hostnames[i], "localhost"); 
		} else {
		/*
		printf("%s is localhost :%d\n", argv[i+1],
		is_localhost(argv[i+1], host_info)); */ 
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
	int num_hosts = argc-1; 
	
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

	host = argv[1];
	
        desktopprog_1(host, hosts_data->num_hosts, hosts_data->hostnames,
		hosts_data);


	printf("done w/ prog...\n"); 
	destroy_hosts_data(hosts_data);
	printf("totally done...\n"); 
	
}


void switch_hosts_new(int which_host, Display *dpy, hosts_data_t hosts_data) {
	int next_host_index;
	if (which_host == LOCAL) {
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
