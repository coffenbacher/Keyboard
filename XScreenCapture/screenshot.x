/* Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l1.html
 */

struct input_data {
        opaque input_data<>;
};

typedef struct input_data input_data;

program SCREENSHOTPROG {
        version SCREENSHOTVERS {
                int SCREENSHOT(input_data) = 1;
        } = 1;
} = 22855;

program DELETEIMAGEPROG {
        version DELETEIMAGEVERS {
                int DELETEIMAGE() = 1;
        } = 1;
} = 22856;

program REFRESHDISPLAYPROG {
        version REFRESHDISPLAYVERS {
                int REFRESHDISPLAY() = 1;
        } = 1;
} = 22857;

program INITDISPLAYPROG {
        version INITDISPLAYVERS {
                int INITDISPLAY() = 1;
        } = 1;
} = 22858;


