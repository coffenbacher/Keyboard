/* Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l1.html
 */

/*
 * The average procedure receives an array of real 
 * numbers and returns the average of their
 * values. This toy service handles a maximum of 
 * 200 numbers.
 */

const MAXAVGSIZE  = 200;

/*
struct input_data {
        double input_data<200>;
};
typedef struct input_data input_data;
*/

struct key_input {
       int on_press;
       int keycode; 
};

typedef struct key_input key_input; 

program AVERAGEPROG {
        version AVERAGEVERS {
                double AVERAGE(key_input) = 1;
        } = 1;
} = 22855;
