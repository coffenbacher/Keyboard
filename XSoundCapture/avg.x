/* Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l1.html
 */

/* Software Systems Final Project
   avg.x
   RPC strucutre
   
   Authors: Jon Reed
   Last updated: 5/4/2011

   This code provides the foundation for creating the RPC client. This is mostly
   taken from the example provided by Mark.
*/

const MAXBUFFERSIZE  = 1024;

struct input_data {
        char input_data<MAXBUFFERSIZE>;
};
typedef struct input_data input_data;

struct control_data {
        int flag;
};
typedef struct control_data control_data;

program SERVERPROG {
        version SERVERVERS {
                int SERVER(input_data) = 1;
        } = 1;
} = 22855;

program CONTROLPROG {
        version CONTROLVERS {
                int CONTROL(control_data) = 1;
        } = 1;
} = 22856;
