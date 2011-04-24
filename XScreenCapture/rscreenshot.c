/* Remote screenshot client code.
 * Taken from http://www.linuxjournal.com/articles/lj/0042/2204/2204l4.html
 */
#include "screenshot.h"
#include <stdlib.h>
#include <gdk/gdk.h>

#define LOGFILENAME "/tmp/screenshot_log"
#define LOG(msg, ...) fprintf(logfile, (msg), ##__VA_ARGS__)

FILE *logfile;
FILE *fp;
static CLIENT *clnt;
static struct timeval tv;

static GdkPixbuf * screenshot;

GdkPixbuf * get_screenshot(){
    GdkPixbuf *screenshot;
    GdkWindow *root_window;
    gint x_orig, y_orig;
    gint width, height;
    root_window = gdk_get_default_root_window ();
    gdk_drawable_get_size (root_window, &width, &height);      
    gdk_window_get_origin (root_window, &x_orig, &y_orig);

    screenshot = gdk_pixbuf_get_from_drawable (NULL, root_window, NULL,
                                           x_orig, y_orig, 0, 0, width, height);
    return screenshot;
}

gboolean save_func(const gchar *buf, gsize count, GError **error, gpointer data){	
        char *host;
        input_data  screenshot_1_arg;
 	gboolean result;
        host = "localhost";	
	logfile = fopen(LOGFILENAME, "a");
	LOG("%d", count);
	LOG("starting save func\n");

	/* RPC SECTION */

        screenshot_1_arg.input_data.input_data_val = (gchar *) malloc(4096*sizeof(gchar));
	memcpy(screenshot_1_arg.input_data.input_data_val, buf, 4096);
        screenshot_1_arg.input_data.input_data_len = 4096;

        result = screenshot_1(&screenshot_1_arg, clnt);
	if (result == NULL) {
	/*
	* An error occurred while calling the server.
	* Print error message and die.
	*/
		clnt_perror(clnt, "localhost");
		exit(1);
	}
	LOG("ending save func\n");
    return TRUE;
}

void screenshotprog_1( char* host, int argc, char *argv[])
{
        double  *result_1, *dp, f;
        char *endptr;
        int i;
        input_data  screenshot_1_arg;
	

	gtk_init(NULL, NULL);

	clnt = clnt_create("localhost", SCREENSHOTPROG, SCREENSHOTVERS, "udp");
	tv.tv_sec = 250;
	tv.tv_usec = 0;
	clnt_control(clnt, CLSET_TIMEOUT, &tv);

	if (clnt == NULL) {
	        clnt_pcreateerror(host);
	        exit(1);
	}
	screenshot = get_screenshot();

    	gdk_pixbuf_save_to_callback(screenshot, save_func, NULL, "jpeg", NULL, "quality", "20", NULL);
    	wait(10000);
}


main( int argc, char* argv[] )
{
        char *host;
/*
        if (argc < 3) {
                printf("usage: %s server_host value ...\n",
                        argv[0]);
                exit(1);
        }
        if (argc > MAXAVGSIZE + 2) {
                printf("Two many input values\n");
                exit(2);
        }*/
        host = argv[1];
        screenshotprog_1(host, argc, argv);
}
