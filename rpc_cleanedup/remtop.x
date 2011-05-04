/* Software Systems Final Project
   remtop.x
   
   Authors: Charles Offenbacher, Poorva Singal, and Jon Reed
   Last updated: 5/4/2011

   The remtop procedure receives an array of host addresses and allows for
   the user to switch between the hosts using the Ctrl-Shift-Up key
   combination for going to the next host and the Ctrl-Shift-Down key
   combination for going to the previous host.  The user can then
   remotely control the mouse and keyboard of the other computer as well
   as receive continuous screenshots to work like a remote desktop.  A
   maximum of 10 host names/IP addresses can be provided.  These must all be
   valid hosts and be running the remtop server (unless the host is the local
   host).

   At any time, the user can switch to the localhost using the Ctrl-Shift-L
   key combination (and go back to the array of hosts provided initially using
   Ctrl-Shift-Up and Ctrl-Shift-Down).  The user may also quit at any time
   using the Ctrl-Alt-q key combination.  

   Notes:
   Poorva worked on modifying this file and worked with Charlie to figure out
   the appropriate input structure for images.  
   
   Credits:
   The basic structure for this code was taken from: 
   Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html

   We followed the layout but had to change the structs and create our own
   for the mouse, keyboard, and images.  
   --
   
 */



/* Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l1.html
 */

/*
 * The average procedure receives an array of real 
 * numbers and returns the average of their
 * values. This toy service handles a maximum of 
 * 200 numbers.
 */

const MAXHOSTS  = 10;

struct key_input {
       int on_press;
       int keycode; 
       int x;
       int y;
};
typedef struct key_input key_input; 

struct mouse_input {
       int x;
       int y;
       int button;
       int on_press;
       int button_event;
};
typedef struct mouse_input mouse_input;

struct image_input {
       int init;
       char host[1024]; 
};
typedef struct image_input image_input; 


program KEYBOARDPROG {
        version KEYBOARDVERS {
                int KEYBOARD(key_input) = 1;
        } = 1;
} = 22855;


program MOUSEPROG {
        version MOUSEVERS {
                int MOUSE(mouse_input) = 1;
        } = 1;
} = 22856;

program IMAGEPROG {
	version IMAGEVERS {
		int IMAGE(image_input) = 1;
	} = 1;
} = 22857; 