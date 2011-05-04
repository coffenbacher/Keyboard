/* Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l1.html
 */

/*
 * The average procedure receives an array of real 
 * numbers and returns the average of their
 * values. This toy service handles a maximum of 
 * 200 numbers.
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
