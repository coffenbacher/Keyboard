/* Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l1.html
 */

struct input_data {
        char buffer[1024];
};

struct control_data {
	int init;
};

typedef struct input_data input_data;

program SOUNDSERVERPROG {
        version SOUNDSERVERVERS {
                int SOUNDSERVER(input_data) = 1;
        } = 1;
} = 22855;

program CONTROLPROG {
        version CONTROLVERS {
                int CONTROL(control_data) = 1;
        } = 1;
} = 22856;
