/* Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l1.html
 */

/*
 * The average procedure receives an array of real 
 * numbers and returns the average of their
 * values. This toy service handles a maximum of 
 * 200 numbers.
 */

const MAXAVGSIZE  = 1024;

struct input_data {
        char input_data<200>;
};

typedef struct input_data input_data;

program AVERAGEPROG {
        version AVERAGEVERS {
                double AVERAGE(input_data) = 1;
        } = 1;
} = 22855;
