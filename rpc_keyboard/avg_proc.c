/* Remote average server code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 *
 * Reformatted and loggin by Mark A. Sheldon.
 *
 */
#include <rpc/rpc.h>
#include "avg.h"
#include <stdio.h>

#include "X11/extensions/XTest.h"
#define KEY_DOWN True
#define KEY_UP   False
#define KEYCODE_ESC 9

#define LOGFILENAME "/tmp/avg_log"
#define LOG(msg, ...) fprintf(logfile, (msg), ##__VA_ARGS__)
FILE *logfile;
static double sum_avg = 0;

/*double *average_1(input_data *input, CLIENT *client) */
double *average_1(key_input *input, CLIENT *client)
{
        /*double *dp = input->input_data.input_data_val; */
/*        u_int i;
	  int val; */
	Display *dpy = XOpenDisplay(NULL);
	char *s; /* take out */
	int on_press; 
	int keycode; 
	int x,y;
	int button; 
	
	if (input->keyboard == 1) {
		on_press = input->on_press;
		keycode = input->keycode;
		s = XKeysymToString(XKeycodeToKeysym(dpy, keycode, 0));
/*		printf("key pressed: %d %s\n", on_press, s);*/

		XTestFakeKeyEvent(dpy,val , on_press, CurrentTime);

	} else if (input->button_event == 0) {
		x = input->x;
		y = input->y;
		/*printf("x: %d, y: %d\n", x, y); */
		
		
		  XTestFakeMotionEvent(dpy, 0, x, y, CurrentTime); 
		 
	} else {
		button = input->button;
		on_press = input->on_press;
		/*printf("button: %d, pressed? %d\n", input->button, on_press);  */

		
		  XTestFakeButtonEvent(dpy, button, on_press, CurrentTime); 
		
	}

	logfile = fopen(LOGFILENAME, "a");
	LOG("Average called with:  ");

	/*for (i = 1; i <= input->input_data.input_data_len; i++) {
		LOG("%f ", *dp);
                val = (int)*dp;
		
		

		if (!dpy) return 1;
		s = XKeysymToString(XKeycodeToKeysym(dpy, val, 0));
		printf("key pressed: %s\n", s);
		
		
		dp++;
		} */

	

/*	XTestFakeKeyEvent(dpy,keycode , on_press, CurrentTime); */
	/*if (on_press)*/
	
	XCloseDisplay(dpy);

	/*
        sum_avg = 0;
        for (i = 1; i <= input->input_data.input_data_len; i++) {
		LOG("%f ", *dp);
                sum_avg = sum_avg + *dp;
		dp++;
        }
        sum_avg = sum_avg / input->input_data.input_data_len;
	*/
/*	LOG("\nReturning:  %f\n", sum_avg); */
	fclose(logfile);
        return &sum_avg;
}

double *mouse_1(mouse_input *input, CLIENT *client)
{
	int x = input->x;
	int y = input->y;

	Display *dpy = XOpenDisplay(NULL);
	x = input->x;
	y = input->y;
	printf('x: %d; y: %d\n', x, y); 
	XCloseDisplay(dpy);
	return &sum_avg; 
	
}

/*double *average_1_svc(input_data *input, struct svc_req *svc) */
double *average_1_svc(key_input *input, struct svc_req *svc)
{
        CLIENT *client;
        return average_1(input, client);
}

double *mouse_1_svc(mouse_input *input, struct svc_req *svc)
{
	CLIENT *client;
	return mouse_1(input, client); 
}
