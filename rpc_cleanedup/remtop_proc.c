/* Software Systems Final Project
   remtop_proc.c
   Remtop Server code
   
   Authors: Charles Offenbacher, Poorva Singal, and Jon Reed
   Last updated: 5/4/2011

   This is the server side code for remtop.  It fakes key and mouse events
   on the remote computer running the service as well as capture screenshots
   on the remote computer to be sent to the local computer.  
   
   Notes:
   Poorva worked on the keyboard/mouse interactions and thus the one primarily
   responsible for the contents of this file.  Charlie and Poorva pair
   programmed to integrate the image part of the service.  
   
   Credits:
   The basic structure for how the server side is set up was taken from: 
   Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html

   We used it essentially just for the structure but modified all of its
   methods.  We added necessary service methods (such as for mouse and
   graphics).
   --
   
 */


#include <rpc/rpc.h>
#include "remtop.h"
#include <stdio.h>
#include <unistd.h>
#include "X11/extensions/XTest.h"
#define KEY_DOWN True
#define KEY_UP   False
#define KEYCODE_ESC 9

#define MAXHOSTLENGTH 1024

static int ret_val = 0;

/* Fakes key events on remote computer */
int *keyboard_1(key_input *input, CLIENT *client)
{

	Display *dpy = XOpenDisplay(NULL);
	char *s; /* take out */
	int on_press; 
	int keycode; 	

	on_press = input->on_press;
	keycode = input->keycode;

	XTestFakeKeyEvent(dpy, keycode, on_press, CurrentTime);

	XCloseDisplay(dpy);
        return &ret_val;
}


/* Fakes mouse events on remote computer */
int *mouse_1(mouse_input *input, CLIENT *client)
{
	Display *dpy = XOpenDisplay(NULL);

	int on_press; 

	int x,y;
	int button; 
	
	if (input->button_event == 0) {
		x = input->x;
		y = input->y;
		
		XTestFakeMotionEvent(dpy, 0, x, y, CurrentTime);
		 
	} else {
		button = input->button;
		on_press = input->on_press;

 		XTestFakeButtonEvent(dpy, button, on_press, CurrentTime);  
		
	}
	
	XCloseDisplay(dpy);
	return &ret_val; 
}

/*
 Initializes loop that captures images on remote computer 
 */
int *image_1(image_input *input, CLIENT *client)
{
	pid_t forkpid;
	fprintf(stderr, "Connected to host: %s\n", input->host);	
	if (input->init) {
		if ( (forkpid = fork()) < 0)
			fprintf(stderr, "Can't create fork.");
		else if  (forkpid == 0){
			execl("./rscreenshot", "rscreenshot",
			      input->host, NULL);
			exit(1);
		}
		else {
			return &ret_val;
		}
	} else {
		system("pkill rscreenshot");
		fprintf(stderr, "Done screenshotting");	
	}
	return &ret_val;
}

/* The following are svc methods for keyboard, mouse, and graphics related
   RPC.  They are kept in the format provided in the tutorial from:
   http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html */
int *keyboard_1_svc(key_input *input, struct svc_req *svc)
{
        CLIENT *client;
        return keyboard_1(input, client);
}

int *mouse_1_svc(mouse_input *input, struct svc_req *svc)
{
	CLIENT *client;
	return mouse_1(input, client); 
}

int *image_1_svc(image_input *input, struct svc_req *svc)
{
	CLIENT *client;
	return image_1(input, client); 
}
