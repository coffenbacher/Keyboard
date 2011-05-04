/* Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l1.html
 */

/*
 * The average procedure receives an array of real 
 * numbers and returns the average of their
 * values. This toy service handles a maximum of 
 * 200 numbers.
 */

const MAXHOSTS  = 10;

/*
struct input_data {
        double input_data<200>;
};
typedef struct input_data input_data;
*/

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
       char *host; 
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