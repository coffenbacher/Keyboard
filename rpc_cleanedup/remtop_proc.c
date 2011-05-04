/* Remote average server code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 *
 * Reformatted and loggin by Mark A. Sheldon.
 *
 */
#include <rpc/rpc.h>
#include "remtop.h"
#include <stdio.h>
#include <unistd.h>
#include "X11/extensions/XTest.h"
#define KEY_DOWN True
#define KEY_UP   False
#define KEYCODE_ESC 9

#define LOGFILENAME "/tmp/avg_log"
#define LOG(msg, ...) fprintf(logfile, (msg), ##__VA_ARGS__)
#define MAXHOSTLENGTH 1024

FILE *logfile;

static int ret_val = 0;

/*double *average_1(input_data *input, CLIENT *client) */
int *keyboard_1(key_input *input, CLIENT *client)
{

	Display *dpy = XOpenDisplay(NULL);
	char *s; /* take out */
	int on_press; 
	int keycode; 	

	on_press = input->on_press;
	keycode = input->keycode;
	s = XKeysymToString(XKeycodeToKeysym(dpy, keycode, 0));
/*	printf("key pressed: %d %s\n", on_press, s);*/

	XTestFakeKeyEvent(dpy, keycode, on_press, CurrentTime);

	XCloseDisplay(dpy);
        return &ret_val;
}



int *mouse_1(mouse_input *input, CLIENT *client)
{
        /*double *dp = input->input_data.input_data_val; */
/*        u_int i;
	  int val; */
	Display *dpy = XOpenDisplay(NULL);

	int on_press; 

	int x,y;
	int button; 
	
	if (input->button_event == 0) {
		x = input->x;
		y = input->y;
		/*printf("x: %d, y: %d\n", x, y); */
		
		
		XTestFakeMotionEvent(dpy, 0, x, y, CurrentTime);
		 
	} else {
		button = input->button;
		on_press = input->on_press;
		/* printf("button: %d, pressed? %d\n", input->button, on_press);   */

 		XTestFakeButtonEvent(dpy, button, on_press, CurrentTime);  
		
	}


	
	XCloseDisplay(dpy);
	return &ret_val; 

	/*return 0; */

}


int *image_1(image_input *input, CLIENT *client)
{
	pid_t forkpid;
	fprintf(stderr, "Connected to host: %s\n", input->host);	
	if (input->init) {
		if ( (forkpid = fork()) < 0)
			fprintf(stderr, "Can't create fork.");
		else if  (forkpid == 0){
			execl("./rscreenshot", "rscreenshot", input->host, NULL);
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


/*double *average_1_svc(input_data *input, struct svc_req *svc) */


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
